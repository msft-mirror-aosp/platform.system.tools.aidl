/*
 * This file is auto-generated.  DO NOT MODIFY.
 * Using: out/host/linux-x86/bin/aidl --lang=cpp -Weverything -Wno-missing-permission-annotation -Werror -t --min_sdk_version current --ninja -d out/soong/.intermediates/system/tools/aidl/aidl-test-interface-cpp-source/gen/staging/android/aidl/tests/OtherParcelableForToString.cpp.d -h out/soong/.intermediates/system/tools/aidl/aidl-test-interface-cpp-source/gen/include/staging -o out/soong/.intermediates/system/tools/aidl/aidl-test-interface-cpp-source/gen/staging -Nsystem/tools/aidl/tests system/tools/aidl/tests/android/aidl/tests/OtherParcelableForToString.aidl
 */
#pragma once

#include <android/binder_to_string.h>
#include <binder/Parcel.h>
#include <binder/Status.h>
#include <tuple>
#include <utils/String16.h>

namespace android {
namespace aidl {
namespace tests {
class OtherParcelableForToString : public ::android::Parcelable {
public:
  ::android::String16 field;
  inline bool operator!=(const OtherParcelableForToString& rhs) const {
    return std::tie(field) != std::tie(rhs.field);
  }
  inline bool operator<(const OtherParcelableForToString& rhs) const {
    return std::tie(field) < std::tie(rhs.field);
  }
  inline bool operator<=(const OtherParcelableForToString& rhs) const {
    return std::tie(field) <= std::tie(rhs.field);
  }
  inline bool operator==(const OtherParcelableForToString& rhs) const {
    return std::tie(field) == std::tie(rhs.field);
  }
  inline bool operator>(const OtherParcelableForToString& rhs) const {
    return std::tie(field) > std::tie(rhs.field);
  }
  inline bool operator>=(const OtherParcelableForToString& rhs) const {
    return std::tie(field) >= std::tie(rhs.field);
  }

  ::android::status_t readFromParcel(const ::android::Parcel* _aidl_parcel) final;
  ::android::status_t writeToParcel(::android::Parcel* _aidl_parcel) const final;
  static const ::android::String16& getParcelableDescriptor() {
    static const ::android::StaticString16 DESCRIPTOR (u"android.aidl.tests.OtherParcelableForToString");
    return DESCRIPTOR;
  }
  inline std::string toString() const {
    std::ostringstream os;
    os << "OtherParcelableForToString{";
    os << "field: " << ::android::internal::ToString(field);
    os << "}";
    return os.str();
  }
};  // class OtherParcelableForToString
}  // namespace tests
}  // namespace aidl
}  // namespace android
