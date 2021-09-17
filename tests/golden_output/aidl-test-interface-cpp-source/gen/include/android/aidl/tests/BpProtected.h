#pragma once

#include <binder/IBinder.h>
#include <binder/IInterface.h>
#include <utils/Errors.h>
#include <android/aidl/tests/IProtected.h>

namespace android {

namespace aidl {

namespace tests {

class BpProtected : public ::android::BpInterface<IProtected> {
public:
  explicit BpProtected(const ::android::sp<::android::IBinder>& _aidl_impl);
  virtual ~BpProtected() = default;
  ::android::binder::Status PermissionProtected() override;
  ::android::binder::Status MultiplePermissions() override;
  ::android::binder::Status MultiplePermissions2() override;
};  // class BpProtected

}  // namespace tests

}  // namespace aidl

}  // namespace android
