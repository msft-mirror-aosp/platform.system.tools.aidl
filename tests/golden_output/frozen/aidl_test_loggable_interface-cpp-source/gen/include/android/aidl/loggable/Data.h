/*
 * This file is auto-generated.  DO NOT MODIFY.
 * Using: out/host/linux-x86/bin/aidl --lang=cpp -Weverything -Wno-missing-permission-annotation -Werror -t --min_sdk_version current --log --structured --ninja -d out/soong/.intermediates/system/tools/aidl/aidl_test_loggable_interface-cpp-source/gen/staging/android/aidl/loggable/Data.cpp.d -h out/soong/.intermediates/system/tools/aidl/aidl_test_loggable_interface-cpp-source/gen/include/staging -o out/soong/.intermediates/system/tools/aidl/aidl_test_loggable_interface-cpp-source/gen/staging -Nsystem/tools/aidl/tests system/tools/aidl/tests/android/aidl/loggable/Data.aidl
 */
#pragma once

#include <android/aidl/loggable/Enum.h>
#include <android/aidl/loggable/Union.h>
#include <android/binder_to_string.h>
#include <binder/Parcel.h>
#include <binder/Status.h>
#include <cstdint>
#include <string>
#include <tuple>
#include <utils/String16.h>

namespace android {
namespace aidl {
namespace loggable {
class Data : public ::android::Parcelable {
public:
  int32_t num = 0;
  ::std::string str;
  ::android::aidl::loggable::Union nestedUnion;
  ::android::aidl::loggable::Enum nestedEnum = ::android::aidl::loggable::Enum::FOO;
  inline bool operator!=(const Data& rhs) const {
    return std::tie(num, str, nestedUnion, nestedEnum) != std::tie(rhs.num, rhs.str, rhs.nestedUnion, rhs.nestedEnum);
  }
  inline bool operator<(const Data& rhs) const {
    return std::tie(num, str, nestedUnion, nestedEnum) < std::tie(rhs.num, rhs.str, rhs.nestedUnion, rhs.nestedEnum);
  }
  inline bool operator<=(const Data& rhs) const {
    return std::tie(num, str, nestedUnion, nestedEnum) <= std::tie(rhs.num, rhs.str, rhs.nestedUnion, rhs.nestedEnum);
  }
  inline bool operator==(const Data& rhs) const {
    return std::tie(num, str, nestedUnion, nestedEnum) == std::tie(rhs.num, rhs.str, rhs.nestedUnion, rhs.nestedEnum);
  }
  inline bool operator>(const Data& rhs) const {
    return std::tie(num, str, nestedUnion, nestedEnum) > std::tie(rhs.num, rhs.str, rhs.nestedUnion, rhs.nestedEnum);
  }
  inline bool operator>=(const Data& rhs) const {
    return std::tie(num, str, nestedUnion, nestedEnum) >= std::tie(rhs.num, rhs.str, rhs.nestedUnion, rhs.nestedEnum);
  }

  ::android::status_t readFromParcel(const ::android::Parcel* _aidl_parcel) final;
  ::android::status_t writeToParcel(::android::Parcel* _aidl_parcel) const final;
  static const ::android::String16& getParcelableDescriptor() {
    static const ::android::StaticString16 DESCRIPTOR (u"android.aidl.loggable.Data");
    return DESCRIPTOR;
  }
  inline std::string toString() const {
    std::ostringstream os;
    os << "Data{";
    os << "num: " << ::android::internal::ToString(num);
    os << ", str: " << ::android::internal::ToString(str);
    os << ", nestedUnion: " << ::android::internal::ToString(nestedUnion);
    os << ", nestedEnum: " << ::android::internal::ToString(nestedEnum);
    os << "}";
    return os.str();
  }
};  // class Data
}  // namespace loggable
}  // namespace aidl
}  // namespace android