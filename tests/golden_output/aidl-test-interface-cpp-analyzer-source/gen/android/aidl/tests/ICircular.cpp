#include <iostream>
#include <binder/Parcel.h>
#include <android/binder_to_string.h>
#include <android/aidl/tests/ICircular.h>
namespace {
android::status_t analyzeICircular(uint32_t _aidl_code, const android::Parcel& _aidl_data, const android::Parcel& _aidl_reply) {
  android::status_t _aidl_ret_status;
  switch(_aidl_code) {
    case ::android::IBinder::FIRST_CALL_TRANSACTION + 0:
    {
      std::cout << "Function Called: GetTestService" << std::endl;
      _aidl_ret_status = ::android::OK;
      if (!(_aidl_data.enforceInterface(android::String16("android.aidl.tests.ICircular")))) {
        _aidl_ret_status = ::android::BAD_TYPE;
        std::cout << "  Failure: Parcel interface does not match." << std::endl;  break;
      }
      ::android::binder::Status binderStatus;
      binderStatus.readFromParcel(_aidl_reply);
      ::android::sp<::android::aidl::tests::ITestService>* _aidl_return = new ::android::sp<::android::aidl::tests::ITestService>;
      _aidl_ret_status = _aidl_reply.readNullableStrongBinder(_aidl_return);
      if (((_aidl_ret_status) != (android::NO_ERROR))) {
        std::cout << "  Failure: error in reading return value from Parcel." << std::endl;  break;
      }
      std::cout << "  Return value: " << ::android::internal::ToString(_aidl_return) << std::endl;
    }
    break;
    default:
    {
      std::cout << "  Transaction code " << _aidl_code << " not known." << std::endl;
    _aidl_ret_status = android::UNKNOWN_TRANSACTION;
    }
  }
  return _aidl_ret_status;
  // To prevent unused variable warnings
  (void)_aidl_ret_status; (void)_aidl_data; (void)_aidl_reply;
}

} // namespace

#include <Analyzer.h>
using android::aidl::Analyzer;
__attribute__((constructor)) static void addAnalyzer() {
  Analyzer::installAnalyzer(std::make_unique<Analyzer>("tests", "ICircular", &analyzeICircular));
}
