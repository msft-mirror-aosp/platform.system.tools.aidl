/*
 * Copyright (C) 2022, The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "generate_cpp_analyzer.h"
#include "aidl.h"

#include <algorithm>
#include <cctype>
#include <cstring>
#include <memory>
#include <random>
#include <string>

#include <android-base/format.h>
#include <android-base/stringprintf.h>
#include <android-base/strings.h>

#include "aidl_language.h"
#include "aidl_to_cpp.h"
#include "code_writer.h"
#include "logging.h"
#include "os.h"

using std::string;
using std::unique_ptr;

namespace android {
namespace aidl {
namespace cpp {

void GenerateAnalyzerSource(CodeWriter& out, const AidlDefinedType& defined_type,
                            const AidlTypenames& /*typenames*/, const Options& /*options*/) {
  auto interface = AidlCast<AidlInterface>(defined_type);
  string q_name = GetQualifiedName(*interface, ClassNames::INTERFACE);

  out << "\n#include <Analyzer.h>\n\n";
  out << "using android::aidl::Analyzer;\n";
  out.Write(
      "__attribute__((constructor)) static void addAnalyzer() {\n"
      "  Analyzer::installAnalyzer(std::make_unique<Analyzer>(\"%s\"));\n}\n",
      q_name.c_str());
}

void GenerateAnalyzerPlaceholder(CodeWriter& out, const AidlDefinedType& /*defined_type*/,
                                 const AidlTypenames& /*typenames*/, const Options& /*options*/) {
  out << "// This file is intentionally left blank as placeholder for building an analyzer.\n";
}

bool GenerateCppAnalyzer(const string& output_file, const Options& options,
                         const AidlTypenames& typenames, const AidlDefinedType& defined_type,
                         const IoDelegate& io_delegate) {
  if (!ValidateOutputFilePath(output_file, options, defined_type)) {
    return false;
  }

  using GenFn = void (*)(CodeWriter & out, const AidlDefinedType& defined_type,
                         const AidlTypenames& typenames, const Options& options);
  auto gen = [&](auto file, GenFn fn) {
    unique_ptr<CodeWriter> writer(io_delegate.GetCodeWriter(file));
    fn(*writer, defined_type, typenames, options);
    AIDL_FATAL_IF(!writer->Close(), defined_type) << "I/O Error!";
    return true;
  };

  if (AidlCast<AidlInterface>(defined_type)) {
    return gen(output_file, &GenerateAnalyzerSource);
  } else {
    return gen(output_file, &GenerateAnalyzerPlaceholder);
  }
}

}  // namespace cpp
}  // namespace aidl
}  // namespace android
