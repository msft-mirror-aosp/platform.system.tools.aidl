/*
 * Copyright (C) 2022 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <android-base/file.h>
#include <android-base/logging.h>
#include <binder/BinderRecordReplay.h>
#include <binder/BpBinder.h>
#include <binder/IServiceManager.h>
#include <binder/Parcel.h>
#include <signal.h>
#include <sstream>
#include "include/Analyzer.h"

using android::IBinder;
using android::NO_ERROR;
using android::sp;
using android::status_t;
using android::String16;
using android::aidl::Analyzer;
using android::binder::debug::RecordedTransaction;
using std::string;

namespace {

static volatile size_t gCtrlCCount = 0;
static constexpr size_t kCtrlCLimit = 3;
static const char kRecordingDir[] = "/data/local/recordings/";

status_t startRecording(const sp<IBinder>& binder, const string& instance) {
  if (auto mkdir_return = mkdir(kRecordingDir, 0666); mkdir_return != 0 && errno != EEXIST) {
    std::cout << "Failed to create recordings directory.\n";
    return android::NO_ERROR;
  }

  string instanceFileName = instance;
  std::replace(instanceFileName.begin(), instanceFileName.end(), '/', '.');
  string filePath = kRecordingDir + instanceFileName;
  int openFlags = O_WRONLY | O_CREAT | O_APPEND | O_CLOEXEC | O_BINARY;
  android::base::unique_fd fd(open(filePath.c_str(), openFlags, 0666));
  if (fd.get() == -1) {
    std::cout << "Failed to open file for recording with error: " << strerror(errno) << '\n';
    return android::BAD_VALUE;
  }

  // TODO (b/245804633): this still requires setenforce 0, but nothing above does
  if (status_t err = binder->remoteBinder()->startRecordingBinder(fd); err != android::NO_ERROR) {
    std::cout << "Failed to start recording with error: " << err << '\n';
    return err;
  } else {
    std::cout << "Recording started successfully.\n";
    return android::NO_ERROR;
  }
}

status_t stopRecording(const sp<IBinder>& binder) {
  if (status_t err = binder->remoteBinder()->stopRecordingBinder(); err != NO_ERROR) {
    std::cout << "Failed to stop recording with error: " << err << '\n';
    return err;
  } else {
    std::cout << "Recording stopped successfully.\n";
    return NO_ERROR;
  }
}

status_t inspectRecording(const string& interface, const string& fileName) {
  auto& analyzers = Analyzer::getAnalyzers();
  auto analyzer = std::find_if(
      begin(analyzers), end(analyzers),
      [&](const std::unique_ptr<Analyzer>& a) { return a->getInterfaceName() == interface; });
  if (analyzer == end(analyzers)) {
    std::cout << "Failed to find analyzer for interface: " << interface << '\n';
    return android::UNKNOWN_ERROR;
  }
  std::cout << "Found matching analyzer for interface: " << interface << '\n';

  std::string path = kRecordingDir + fileName;
  android::base::unique_fd fd(open(path.c_str(), O_RDONLY));
  if (fd.get() == -1) {
    std::cout << "Failed to open recording file with error: " << strerror(errno) << '\n';
    return android::BAD_VALUE;
  }

  int i = 1;
  while (auto transaction = RecordedTransaction::fromFile(fd)) {
    std::cout << "Transaction " << i << ":\n";
    (*analyzer)->getAnalyzeFunction()(transaction->getCode(), transaction->getDataParcel(),
                                      transaction->getReplyParcel());

    std::cout << "Status returned from this transaction: ";
    if (transaction->getReturnedStatus() == 0) {
      std::cout << "NO_ERROR\n\n";
    } else {
      std::cout << transaction->getReturnedStatus() << "\n\n";
    }
    i++;
  }
  return NO_ERROR;
}

void incrementCtrlCCount(int signum) {
  gCtrlCCount++;
  if (gCtrlCCount > kCtrlCLimit) {
    std::cout
        << "Ctrl+C multiple times, but could not quit application. If recording still running, you "
           "might stop it manually.\n";
    exit(signum);
  }
}

status_t listenToFile(const sp<IBinder>& binder, const string& instance, const string& interface) {
  auto& analyzers = Analyzer::getAnalyzers();
  auto analyzer = std::find_if(
      begin(analyzers), end(analyzers),
      [&](const std::unique_ptr<Analyzer>& a) { return a->getInterfaceName() == interface; });
  if (analyzer == end(analyzers)) {
    std::cout << "Failed to find analyzer for interface: " << interface << '\n';
    return android::UNKNOWN_ERROR;
  }

  if (auto mkdir_return = mkdir(kRecordingDir, 0666); mkdir_return != 0 && errno != EEXIST) {
    std::cout << "Failed to create recordings directory.\n";
    return android::NO_ERROR;
  }

  string instanceFileName = instance;
  std::replace(instanceFileName.begin(), instanceFileName.end(), '/', '.');
  string filePath = kRecordingDir + instanceFileName + ".listen";
  int openFlags = O_WRONLY | O_CREAT | O_APPEND | O_CLOEXEC | O_BINARY;
  android::base::unique_fd fd(open(filePath.c_str(), openFlags, 0666));
  if (fd.get() == -1) {
    std::cout << "Failed to open file for listening with error: " << strerror(errno) << '\n';
    return android::BAD_VALUE;
  }
  android::base::unique_fd listenFd(open(filePath.c_str(), O_RDONLY));
  if (fd.get() == -1) {
    std::cout << "Failed to open listening file with error: " << strerror(errno) << '\n';
    return android::BAD_VALUE;
  }

  if (status_t err = binder->remoteBinder()->startRecordingBinder(fd); err != android::NO_ERROR) {
    std::cout << "Failed to start recording with error: " << err << '\n';
    return err;
  }

  signal(SIGINT, incrementCtrlCCount);
  std::cout << "Starting to listen:\n";
  int i = 1;
  while (gCtrlCCount == 0) {
    auto transaction = RecordedTransaction::fromFile(listenFd);
    if (!transaction) {
      sleep(1);
      continue;
    }
    std::cout << "Transaction " << i << ":\n";
    (*analyzer)->getAnalyzeFunction()(transaction->getCode(), transaction->getDataParcel(),
                                      transaction->getReplyParcel());
    std::cout << "Status returned from this transaction: ";
    if (transaction->getReturnedStatus() == 0) {
      std::cout << "NO_ERROR\n\n";
    } else {
      std::cout << transaction->getReturnedStatus() << '\n\n';
    }
    i++;
  }
  binder->remoteBinder()->stopRecordingBinder();
  return NO_ERROR;
}

status_t replayFile(const sp<IBinder>& binder, const string& interface, const string& fileName) {
  auto& analyzers = Analyzer::getAnalyzers();
  auto analyzer = std::find_if(
      begin(analyzers), end(analyzers),
      [&](const std::unique_ptr<Analyzer>& a) { return a->getInterfaceName() == interface; });
  if (analyzer == end(analyzers)) {
    std::cout << "Failed to find analyzer for interface: " << interface << '\n';
    return android::UNKNOWN_ERROR;
  }
  std::cout << "Found matching analyzer for interface: " << interface << '\n';

  std::string path = kRecordingDir + fileName;
  android::base::unique_fd fd(open(path.c_str(), O_RDONLY));
  if (fd.get() == -1) {
    std::cout << "Failed to open recording file with error: " << strerror(errno) << '\n';
    return android::BAD_VALUE;
  }

  bool failure = false;
  int i = 1;
  while (auto transaction = RecordedTransaction::fromFile(fd)) {
    std::cout << "Replaying Transaction " << i << ":\n";
    (*analyzer)->getAnalyzeFunction()(transaction->getCode(), transaction->getDataParcel(),
                                      transaction->getReplyParcel());

    android::Parcel send, reply;
    send.setData(transaction->getDataParcel().data(), transaction->getDataSize());
    android::status_t status = binder->remoteBinder()->transact(transaction->getCode(), send,
                                                                &reply, transaction->getFlags());
    if (status != transaction->getReturnedStatus()) {
      std::cout << "Failure: Expected status " << transaction->getReturnedStatus()
                << " but received status " << status << "\n\n";
      failure = true;
    } else {
      std::cout << "Transaction replayed correctly."
                << "\n\n";
    }
    i++;
  }

  if (failure) {
    std::cout << "Some or all transactions failed to replay correctly. See logs for details.\n";
    return android::UNKNOWN_ERROR;
  } else {
    std::cout << "All transactions replayed correctly.\n";
    return NO_ERROR;
  }
}

void printAvailable() {
  auto& analyzers = Analyzer::getAnalyzers();
  std::cout << "Available Interfaces (" << analyzers.size() << "):\n";
  for (unsigned i = 0; i < analyzers.size(); i++) {
    std::cout << "  " << analyzers[i]->getInterfaceName() << '\n';
  }
}

void usageMessage() {
  std::cout << "Usage:\n";
  std::cout << "  start   [Instance]\n";
  std::cout << "  stop    [Instance]\n";
  std::cout << "  inspect [Interface] [File Name]\n";
  std::cout << "  listen  [Instance]  [Interface]\n";
  std::cout << "  replay  [Instance]  [Interface] [File Name]\n";
  std::cout << "(Use --list to find available interfaces)\n";
}

}  // namespace

int main(int argc, char* argv[]) {
  if (argc == 1 || (argc == 2 && strcmp(argv[1], "--help") == 0)) {
    usageMessage();
  } else if (argc == 2 && strcmp(argv[1], "--list") == 0) {
    printAvailable();
  } else if (argc == 3 && strcmp(argv[1], "start") == 0) {
    sp<IBinder> binder = android::defaultServiceManager()->checkService(String16(argv[2]));
    startRecording(binder, argv[2]);
  } else if (argc == 3 && strcmp(argv[1], "stop") == 0) {
    sp<IBinder> binder = android::defaultServiceManager()->checkService(String16(argv[2]));
    stopRecording(binder);
  } else if (argc == 4 && strcmp(argv[1], "inspect") == 0) {
    inspectRecording(argv[2], argv[3]);
  } else if (argc == 4 && strcmp(argv[1], "listen") == 0) {
    sp<IBinder> binder = android::defaultServiceManager()->checkService(String16(argv[2]));
    listenToFile(binder, argv[2], argv[3]);
  } else if (argc == 5 && strcmp(argv[1], "replay") == 0) {
    sp<IBinder> binder = android::defaultServiceManager()->checkService(String16(argv[2]));
    replayFile(binder, argv[3], argv[4]);
  } else {
    std::cout << "Error: unrecognized command or argument structure.\n";
    return 1;
  }

  return 0;
}
