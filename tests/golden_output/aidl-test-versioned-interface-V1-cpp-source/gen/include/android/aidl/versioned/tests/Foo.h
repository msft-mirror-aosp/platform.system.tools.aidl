/*
 * This file is auto-generated.  DO NOT MODIFY.
 * Using: out/host/linux-x86/bin/aidl --lang=cpp --version 1 --hash 9e7be1859820c59d9d55dd133e71a3687b5d2e5b -t --min_sdk_version current --structured --ninja -d out/soong/.intermediates/system/tools/aidl/aidl-test-versioned-interface-V1-cpp-source/gen/staging/android/aidl/versioned/tests/Foo.cpp.d -h out/soong/.intermediates/system/tools/aidl/aidl-test-versioned-interface-V1-cpp-source/gen/include/staging -o out/soong/.intermediates/system/tools/aidl/aidl-test-versioned-interface-V1-cpp-source/gen/staging -Isystem/tools/aidl/aidl_api/aidl-test-versioned-interface/1 system/tools/aidl/aidl_api/aidl-test-versioned-interface/1/android/aidl/versioned/tests/Foo.aidl
 */
#pragma once

#include <android/binder_to_string.h>
#include <binder/Parcel.h>
#include <binder/Status.h>
#include <tuple>
#include <utils/String16.h>

namespace android {
namespace aidl {
namespace versioned {
namespace tests {
class Foo : public ::android::Parcelable {
public:
  inline bool operator!=(const Foo&) const {
    return std::tie() != std::tie();
  }
  inline bool operator<(const Foo&) const {
    return std::tie() < std::tie();
  }
  inline bool operator<=(const Foo&) const {
    return std::tie() <= std::tie();
  }
  inline bool operator==(const Foo&) const {
    return std::tie() == std::tie();
  }
  inline bool operator>(const Foo&) const {
    return std::tie() > std::tie();
  }
  inline bool operator>=(const Foo&) const {
    return std::tie() >= std::tie();
  }

  ::android::status_t readFromParcel(const ::android::Parcel* _aidl_parcel) final;
  ::android::status_t writeToParcel(::android::Parcel* _aidl_parcel) const final;
  static const ::android::String16& getParcelableDescriptor() {
    static const ::android::StaticString16 DESCRIPTOR (u"android.aidl.versioned.tests.Foo");
    return DESCRIPTOR;
  }
  inline std::string toString() const {
    std::ostringstream os;
    os << "Foo{";
    os << "}";
    return os.str();
  }
};  // class Foo
}  // namespace tests
}  // namespace versioned
}  // namespace aidl
}  // namespace android
