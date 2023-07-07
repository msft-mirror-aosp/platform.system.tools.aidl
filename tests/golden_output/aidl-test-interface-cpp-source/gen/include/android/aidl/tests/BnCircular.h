#pragma once

#include <binder/IInterface.h>
#include <android/aidl/tests/ICircular.h>
#include <android/aidl/tests/BnCircular.h>
#include <android/aidl/tests/BnTestService.h>
#include <binder/Delegate.h>


namespace android {
namespace aidl {
namespace tests {
class BnCircular : public ::android::BnInterface<ICircular> {
public:
  static constexpr uint32_t TRANSACTION_GetTestService = ::android::IBinder::FIRST_CALL_TRANSACTION + 0;
  explicit BnCircular();
  ::android::status_t onTransact(uint32_t _aidl_code, const ::android::Parcel& _aidl_data, ::android::Parcel* _aidl_reply, uint32_t _aidl_flags) override;
};  // class BnCircular

class ICircularDelegator : public BnCircular {
public:
  explicit ICircularDelegator(const ::android::sp<ICircular> &impl) : _aidl_delegate(impl) {}

  ::android::sp<ICircular> getImpl() { return _aidl_delegate; }
  ::android::binder::Status GetTestService(::android::sp<::android::aidl::tests::ITestService>* _aidl_return) override {
    auto _status = _aidl_delegate->GetTestService(_aidl_return);
    if (*_aidl_return) {
      *_aidl_return = ::android::sp<::android::aidl::tests::ITestServiceDelegator>::cast(delegate(*_aidl_return));
    }
    return _status;
  }
private:
  ::android::sp<ICircular> _aidl_delegate;
};  // class ICircularDelegator
}  // namespace tests
}  // namespace aidl
}  // namespace android
