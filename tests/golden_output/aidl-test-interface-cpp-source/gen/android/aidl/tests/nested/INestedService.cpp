#include <android/aidl/tests/nested/INestedService.h>
#include <android/aidl/tests/nested/BpNestedService.h>
namespace android {
namespace aidl {
namespace tests {
namespace nested {
DO_NOT_DIRECTLY_USE_ME_IMPLEMENT_META_INTERFACE(NestedService, "android.aidl.tests.nested.INestedService")
}  // namespace nested
}  // namespace tests
}  // namespace aidl
}  // namespace android
#include <android/aidl/tests/nested/BpNestedService.h>
#include <android/aidl/tests/nested/BnNestedService.h>
#include <binder/Parcel.h>
#include <android-base/macros.h>

namespace android {
namespace aidl {
namespace tests {
namespace nested {

BpNestedService::BpNestedService(const ::android::sp<::android::IBinder>& _aidl_impl)
    : BpInterface<INestedService>(_aidl_impl){
}

::android::binder::Status BpNestedService::flipStatus(const ::android::aidl::tests::nested::ParcelableWithNested& p, ::android::aidl::tests::nested::INestedService::Result* _aidl_return) {
  ::android::Parcel _aidl_data;
  _aidl_data.markForBinder(remoteStrong());
  ::android::Parcel _aidl_reply;
  ::android::status_t _aidl_ret_status = ::android::OK;
  ::android::binder::Status _aidl_status;
  _aidl_ret_status = _aidl_data.writeInterfaceToken(getInterfaceDescriptor());
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
  _aidl_ret_status = _aidl_data.writeParcelable(p);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
  _aidl_ret_status = remote()->transact(BnNestedService::TRANSACTION_flipStatus, _aidl_data, &_aidl_reply, 0);
  if (UNLIKELY(_aidl_ret_status == ::android::UNKNOWN_TRANSACTION && INestedService::getDefaultImpl())) {
     return INestedService::getDefaultImpl()->flipStatus(p, _aidl_return);
  }
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
  _aidl_ret_status = _aidl_status.readFromParcel(_aidl_reply);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
  if (!_aidl_status.isOk()) {
    return _aidl_status;
  }
  _aidl_ret_status = _aidl_reply.readParcelable(_aidl_return);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
  _aidl_error:
  _aidl_status.setFromStatusT(_aidl_ret_status);
  return _aidl_status;
}

}  // namespace nested
}  // namespace tests
}  // namespace aidl
}  // namespace android
#include <android/aidl/tests/nested/BnNestedService.h>
#include <binder/Parcel.h>
#include <binder/Stability.h>

namespace android {
namespace aidl {
namespace tests {
namespace nested {

BnNestedService::BnNestedService()
{
  ::android::internal::Stability::markCompilationUnit(this);
}

::android::status_t BnNestedService::onTransact(uint32_t _aidl_code, const ::android::Parcel& _aidl_data, ::android::Parcel* _aidl_reply, uint32_t _aidl_flags) {
  ::android::status_t _aidl_ret_status = ::android::OK;
  switch (_aidl_code) {
  case BnNestedService::TRANSACTION_flipStatus:
  {
    ::android::aidl::tests::nested::ParcelableWithNested in_p;
    ::android::aidl::tests::nested::INestedService::Result _aidl_return;
    if (!(_aidl_data.checkInterface(this))) {
      _aidl_ret_status = ::android::BAD_TYPE;
      break;
    }
    _aidl_ret_status = _aidl_data.readParcelable(&in_p);
    if (((_aidl_ret_status) != (::android::OK))) {
      break;
    }
    ::android::binder::Status _aidl_status(flipStatus(in_p, &_aidl_return));
    _aidl_ret_status = _aidl_status.writeToParcel(_aidl_reply);
    if (((_aidl_ret_status) != (::android::OK))) {
      break;
    }
    if (!_aidl_status.isOk()) {
      break;
    }
    _aidl_ret_status = _aidl_reply->writeParcelable(_aidl_return);
    if (((_aidl_ret_status) != (::android::OK))) {
      break;
    }
  }
  break;
  default:
  {
    _aidl_ret_status = ::android::BBinder::onTransact(_aidl_code, _aidl_data, _aidl_reply, _aidl_flags);
  }
  break;
  }
  if (_aidl_ret_status == ::android::UNEXPECTED_NULL) {
    _aidl_ret_status = ::android::binder::Status::fromExceptionCode(::android::binder::Status::EX_NULL_POINTER).writeToParcel(_aidl_reply);
  }
  return _aidl_ret_status;
}

}  // namespace nested
}  // namespace tests
}  // namespace aidl
}  // namespace android
#include <android/aidl/tests/nested/INestedService.h>

namespace android {
namespace aidl {
namespace tests {
namespace nested {
::android::status_t INestedService::Result::readFromParcel(const ::android::Parcel* _aidl_parcel) {
  ::android::status_t _aidl_ret_status = ::android::OK;
  [[maybe_unused]] size_t _aidl_start_pos = _aidl_parcel->dataPosition();
  int32_t _aidl_parcelable_raw_size = _aidl_parcel->readInt32();
  if (_aidl_parcelable_raw_size < 0) return ::android::BAD_VALUE;
  [[maybe_unused]] size_t _aidl_parcelable_size = static_cast<size_t>(_aidl_parcelable_raw_size);
  if (_aidl_start_pos > SIZE_MAX - _aidl_parcelable_size) return ::android::BAD_VALUE;
  if (_aidl_parcel->dataPosition() - _aidl_start_pos >= _aidl_parcelable_size) {
    _aidl_parcel->setDataPosition(_aidl_start_pos + _aidl_parcelable_size);
    return _aidl_ret_status;
  }
  _aidl_ret_status = _aidl_parcel->readByte(reinterpret_cast<int8_t *>(&status));
  if (((_aidl_ret_status) != (::android::OK))) {
    return _aidl_ret_status;
  }
  _aidl_parcel->setDataPosition(_aidl_start_pos + _aidl_parcelable_size);
  return _aidl_ret_status;
}
::android::status_t INestedService::Result::writeToParcel(::android::Parcel* _aidl_parcel) const {
  ::android::status_t _aidl_ret_status = ::android::OK;
  auto _aidl_start_pos = _aidl_parcel->dataPosition();
  _aidl_parcel->writeInt32(0);
  _aidl_ret_status = _aidl_parcel->writeByte(static_cast<int8_t>(status));
  if (((_aidl_ret_status) != (::android::OK))) {
    return _aidl_ret_status;
  }
  auto _aidl_end_pos = _aidl_parcel->dataPosition();
  _aidl_parcel->setDataPosition(_aidl_start_pos);
  _aidl_parcel->writeInt32(_aidl_end_pos - _aidl_start_pos);
  _aidl_parcel->setDataPosition(_aidl_end_pos);
  return _aidl_ret_status;
}
}  // namespace nested
}  // namespace tests
}  // namespace aidl
}  // namespace android
