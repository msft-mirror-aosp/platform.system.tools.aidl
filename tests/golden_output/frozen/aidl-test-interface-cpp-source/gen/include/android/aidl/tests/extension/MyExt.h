/*
 * This file is auto-generated.  DO NOT MODIFY.
 * Using: out/host/linux-x86/bin/aidl --lang=cpp -Weverything -Wno-missing-permission-annotation -Werror -t --min_sdk_version current --ninja -d out/soong/.intermediates/system/tools/aidl/aidl-test-interface-cpp-source/gen/staging/android/aidl/tests/extension/MyExt.cpp.d -h out/soong/.intermediates/system/tools/aidl/aidl-test-interface-cpp-source/gen/include/staging -o out/soong/.intermediates/system/tools/aidl/aidl-test-interface-cpp-source/gen/staging -Nsystem/tools/aidl/tests system/tools/aidl/tests/android/aidl/tests/extension/MyExt.aidl
 */
#pragma once

#include <android/binder_to_string.h>
#include <binder/Parcel.h>
#include <binder/Status.h>
#include <cstdint>
#include <string>
#include <tuple>
#include <utils/String16.h>

namespace android {
namespace aidl {
namespace tests {
namespace extension {
class MyExt : public ::android::Parcelable {
public:
  int32_t a = 0;
  ::std::string b;
  inline bool operator!=(const MyExt& rhs) const {
    return std::tie(a, b) != std::tie(rhs.a, rhs.b);
  }
  inline bool operator<(const MyExt& rhs) const {
    return std::tie(a, b) < std::tie(rhs.a, rhs.b);
  }
  inline bool operator<=(const MyExt& rhs) const {
    return std::tie(a, b) <= std::tie(rhs.a, rhs.b);
  }
  inline bool operator==(const MyExt& rhs) const {
    return std::tie(a, b) == std::tie(rhs.a, rhs.b);
  }
  inline bool operator>(const MyExt& rhs) const {
    return std::tie(a, b) > std::tie(rhs.a, rhs.b);
  }
  inline bool operator>=(const MyExt& rhs) const {
    return std::tie(a, b) >= std::tie(rhs.a, rhs.b);
  }

  ::android::status_t readFromParcel(const ::android::Parcel* _aidl_parcel) final;
  ::android::status_t writeToParcel(::android::Parcel* _aidl_parcel) const final;
  static const ::android::String16& getParcelableDescriptor() {
    static const ::android::StaticString16 DESCRIPTOR (u"android.aidl.tests.extension.MyExt");
    return DESCRIPTOR;
  }
  inline std::string toString() const {
    std::ostringstream os;
    os << "MyExt{";
    os << "a: " << ::android::internal::ToString(a);
    os << ", b: " << ::android::internal::ToString(b);
    os << "}";
    return os.str();
  }
};  // class MyExt
}  // namespace extension
}  // namespace tests
}  // namespace aidl
}  // namespace android
