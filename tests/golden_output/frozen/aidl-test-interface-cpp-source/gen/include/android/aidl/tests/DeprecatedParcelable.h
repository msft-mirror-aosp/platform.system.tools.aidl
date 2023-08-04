/*
 * This file is auto-generated.  DO NOT MODIFY.
 * Using: out/host/linux-x86/bin/aidl --lang=cpp -Weverything -Wno-missing-permission-annotation -Werror -t --min_sdk_version current --structured --ninja -d out/soong/.intermediates/system/tools/aidl/aidl-test-interface-cpp-source/gen/staging/android/aidl/tests/DeprecatedParcelable.cpp.d -h out/soong/.intermediates/system/tools/aidl/aidl-test-interface-cpp-source/gen/include/staging -o out/soong/.intermediates/system/tools/aidl/aidl-test-interface-cpp-source/gen/staging -Nsystem/tools/aidl/tests system/tools/aidl/tests/android/aidl/tests/DeprecatedParcelable.aidl
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
class __attribute__((deprecated("test"))) DeprecatedParcelable : public ::android::Parcelable {
public:
  inline bool operator!=(const DeprecatedParcelable&) const {
    return std::tie() != std::tie();
  }
  inline bool operator<(const DeprecatedParcelable&) const {
    return std::tie() < std::tie();
  }
  inline bool operator<=(const DeprecatedParcelable&) const {
    return std::tie() <= std::tie();
  }
  inline bool operator==(const DeprecatedParcelable&) const {
    return std::tie() == std::tie();
  }
  inline bool operator>(const DeprecatedParcelable&) const {
    return std::tie() > std::tie();
  }
  inline bool operator>=(const DeprecatedParcelable&) const {
    return std::tie() >= std::tie();
  }

  ::android::status_t readFromParcel(const ::android::Parcel* _aidl_parcel) final;
  ::android::status_t writeToParcel(::android::Parcel* _aidl_parcel) const final;
  static const ::android::String16& getParcelableDescriptor() {
    static const ::android::StaticString16 DESCRIPTOR (u"android.aidl.tests.DeprecatedParcelable");
    return DESCRIPTOR;
  }
  inline std::string toString() const {
    std::ostringstream os;
    os << "DeprecatedParcelable{";
    os << "}";
    return os.str();
  }
};  // class DeprecatedParcelable
}  // namespace tests
}  // namespace aidl
}  // namespace android
