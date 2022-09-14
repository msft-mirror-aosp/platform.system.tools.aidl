#include <iostream>
#include <binder/Parcel.h>
#include <android/binder_to_string.h>
#include <android/aidl/tests/nested/INestedService.h>
namespace {
android::status_t analyzeINestedService(uint32_t _aidl_code, const android::Parcel& _aidl_data, const android::Parcel& _aidl_reply) {
  android::status_t _aidl_ret_status;
  switch(_aidl_code) {
    case ::android::IBinder::FIRST_CALL_TRANSACTION + 0:
    {
      std::cout << "Function Called: flipStatus" << std::endl;
      _aidl_ret_status = ::android::OK;
      if (!(_aidl_data.enforceInterface(android::String16("android.aidl.tests.nested.INestedService")))) {
        _aidl_ret_status = ::android::BAD_TYPE;
        std::cout << "  Failure: Parcel interface does not match." << std::endl;  break;
      }
      ::android::aidl::tests::nested::ParcelableWithNested in_p;
      ::android::binder::Status binderStatus;
      binderStatus.readFromParcel(_aidl_reply);
      ::android::aidl::tests::nested::INestedService::Result* _aidl_return = new ::android::aidl::tests::nested::INestedService::Result;
      _aidl_ret_status = _aidl_reply.readParcelable(_aidl_return);
      if (((_aidl_ret_status) != (android::NO_ERROR))) {
        std::cout << "  Failure: error in reading return value from Parcel." << std::endl;  break;
      }
      _aidl_ret_status = _aidl_data.readParcelable(&in_p);
      if (((_aidl_ret_status) != (android::NO_ERROR))) {
        std::cout << "  Failure: error in reading argument p from Parcel." << std::endl;  break;
      }
      if (!_aidl_data.enforceNoDataAvail().isOk()) {
        _aidl_ret_status = android::BAD_VALUE;
        std::cout << "  Failure: Parcel has too much data." << std::endl;
        break;
      }
      std::cout << "  Argument \"p\" has value: " << ::android::internal::ToString(in_p) << std::endl;
      std::cout << "  Return value: " << ::android::internal::ToString(_aidl_return) << std::endl;
    }
    break;
    case ::android::IBinder::FIRST_CALL_TRANSACTION + 1:
    {
      std::cout << "Function Called: flipStatusWithCallback" << std::endl;
      _aidl_ret_status = ::android::OK;
      if (!(_aidl_data.enforceInterface(android::String16("android.aidl.tests.nested.INestedService")))) {
        _aidl_ret_status = ::android::BAD_TYPE;
        std::cout << "  Failure: Parcel interface does not match." << std::endl;  break;
      }
      ::android::aidl::tests::nested::ParcelableWithNested::Status in_status;
      ::android::sp<::android::aidl::tests::nested::INestedService::ICallback> in_cb;
      ::android::binder::Status binderStatus;
      binderStatus.readFromParcel(_aidl_reply);
      _aidl_ret_status = _aidl_data.readByte(reinterpret_cast<int8_t *>(&in_status));
      if (((_aidl_ret_status) != (android::NO_ERROR))) {
        std::cout << "  Failure: error in reading argument status from Parcel." << std::endl;  break;
      }
      _aidl_ret_status = _aidl_data.readStrongBinder(&in_cb);
      if (((_aidl_ret_status) != (android::NO_ERROR))) {
        std::cout << "  Failure: error in reading argument cb from Parcel." << std::endl;  break;
      }
      if (!_aidl_data.enforceNoDataAvail().isOk()) {
        _aidl_ret_status = android::BAD_VALUE;
        std::cout << "  Failure: Parcel has too much data." << std::endl;
        break;
      }
      std::cout << "  Argument \"status\" has value: " << ::android::internal::ToString(in_status) << std::endl;
      std::cout << "  Argument \"cb\" has value: " << ::android::internal::ToString(in_cb) << std::endl;
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
  Analyzer::installAnalyzer(std::make_unique<Analyzer>("nested", "INestedService", &analyzeINestedService));
}
