#include "aidl/android/aidl/tests/nested/ParcelableWithNested.h"

#include <android/binder_parcel_utils.h>

namespace aidl {
namespace android {
namespace aidl {
namespace tests {
namespace nested {
const char* ParcelableWithNested::descriptor = "android.aidl.tests.nested.ParcelableWithNested";

binder_status_t ParcelableWithNested::readFromParcel(const AParcel* parcel) {
  int32_t _aidl_parcelable_size;
  int32_t _aidl_start_pos = AParcel_getDataPosition(parcel);
  binder_status_t _aidl_ret_status = AParcel_readInt32(parcel, &_aidl_parcelable_size);
  if (_aidl_start_pos > INT32_MAX - _aidl_parcelable_size) return STATUS_BAD_VALUE;
  if (_aidl_parcelable_size < 0) return STATUS_BAD_VALUE;
  if (_aidl_ret_status != STATUS_OK) return _aidl_ret_status;

  if (AParcel_getDataPosition(parcel) - _aidl_start_pos >= _aidl_parcelable_size) {
    AParcel_setDataPosition(parcel, _aidl_start_pos + _aidl_parcelable_size);
    return _aidl_ret_status;
  }
  _aidl_ret_status = AParcel_readByte(parcel, reinterpret_cast<int8_t*>(&status));
  if (_aidl_ret_status != STATUS_OK) return _aidl_ret_status;

  AParcel_setDataPosition(parcel, _aidl_start_pos + _aidl_parcelable_size);
  return _aidl_ret_status;
}
binder_status_t ParcelableWithNested::writeToParcel(AParcel* parcel) const {
  binder_status_t _aidl_ret_status;
  size_t _aidl_start_pos = AParcel_getDataPosition(parcel);
  _aidl_ret_status = AParcel_writeInt32(parcel, 0);
  if (_aidl_ret_status != STATUS_OK) return _aidl_ret_status;

  _aidl_ret_status = AParcel_writeByte(parcel, static_cast<int8_t>(status));
  if (_aidl_ret_status != STATUS_OK) return _aidl_ret_status;

  size_t _aidl_end_pos = AParcel_getDataPosition(parcel);
  AParcel_setDataPosition(parcel, _aidl_start_pos);
  AParcel_writeInt32(parcel, _aidl_end_pos - _aidl_start_pos);
  AParcel_setDataPosition(parcel, _aidl_end_pos);
  return _aidl_ret_status;
}

}  // namespace nested
}  // namespace tests
}  // namespace aidl
}  // namespace android
}  // namespace aidl
