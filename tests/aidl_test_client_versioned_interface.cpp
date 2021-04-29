/*
 * Copyright (C) 2021 The Android Open Source Project
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
#include "aidl_test_client_versioned_interface.h"

#include <iostream>

#include <android/aidl/versioned/tests/IFooInterface.h>
#include <binder/IServiceManager.h>
#include <utils/StrongPointer.h>

using std::cout;
using std::endl;

using android::OK;
using android::sp;
using android::aidl::versioned::tests::Foo;
using android::aidl::versioned::tests::IFooInterface;

namespace android {
namespace aidl {
namespace tests {
namespace client {

bool ConfirmReadDataAfterParcelableWithNewField() {
  cout << "Confirming reading data after parcelable with new field." << endl;
  sp<IFooInterface> versioned;
  if (OK != android::getService(IFooInterface::descriptor, &versioned) || versioned == nullptr) {
    // skip when IFooInterface is not available
    return true;
  }

  Foo foo;
  int32_t ret;
  auto status = versioned->ignoreFooAndReturnInt(foo, 43, &ret);
  if (!status.isOk()) {
    cout << "failed to call ignoreFooAndRepeatInt(): " << endl;
    return false;
  }
  if (ret != 43) {
    cout << "IFooInterface::ignoreFooAndRepeatInt(foo, 43) should return 43, but got " << ret
         << endl;
    return false;
  }
  return true;
}

}  // namespace client
}  // namespace tests
}  // namespace aidl
}  // namespace android