/*
 * This file is auto-generated.  DO NOT MODIFY.
 * Using: out/host/linux-x86/bin/aidl --lang=ndk --structured --version 2 --hash da8c4bc94ca7feff0e0a65563a466787698b5891 --min_sdk_version current --ninja -d out/soong/.intermediates/system/tools/aidl/aidl-test-versioned-interface-V2-ndk-source/gen/staging/android/aidl/versioned/tests/Foo.cpp.d -h out/soong/.intermediates/system/tools/aidl/aidl-test-versioned-interface-V2-ndk-source/gen/include/staging -o out/soong/.intermediates/system/tools/aidl/aidl-test-versioned-interface-V2-ndk-source/gen/staging -Nsystem/tools/aidl/aidl_api/aidl-test-versioned-interface/2 system/tools/aidl/aidl_api/aidl-test-versioned-interface/2/android/aidl/versioned/tests/Foo.aidl
 */
#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <android/binder_interface_utils.h>
#include <android/binder_parcelable_utils.h>
#include <android/binder_to_string.h>
#ifdef BINDER_STABILITY_SUPPORT
#include <android/binder_stability.h>
#endif  // BINDER_STABILITY_SUPPORT

namespace aidl {
namespace android {
namespace aidl {
namespace versioned {
namespace tests {
class Foo {
public:
  typedef std::false_type fixed_size;
  static const char* descriptor;

  int32_t intDefault42 = 42;

  binder_status_t readFromParcel(const AParcel* parcel);
  binder_status_t writeToParcel(AParcel* parcel) const;

  inline bool operator!=(const Foo& rhs) const {
    return std::tie(intDefault42) != std::tie(rhs.intDefault42);
  }
  inline bool operator<(const Foo& rhs) const {
    return std::tie(intDefault42) < std::tie(rhs.intDefault42);
  }
  inline bool operator<=(const Foo& rhs) const {
    return std::tie(intDefault42) <= std::tie(rhs.intDefault42);
  }
  inline bool operator==(const Foo& rhs) const {
    return std::tie(intDefault42) == std::tie(rhs.intDefault42);
  }
  inline bool operator>(const Foo& rhs) const {
    return std::tie(intDefault42) > std::tie(rhs.intDefault42);
  }
  inline bool operator>=(const Foo& rhs) const {
    return std::tie(intDefault42) >= std::tie(rhs.intDefault42);
  }

  static const ::ndk::parcelable_stability_t _aidl_stability = ::ndk::STABILITY_LOCAL;
  inline std::string toString() const {
    std::ostringstream os;
    os << "Foo{";
    os << "intDefault42: " << ::android::internal::ToString(intDefault42);
    os << "}";
    return os.str();
  }
};
}  // namespace tests
}  // namespace versioned
}  // namespace aidl
}  // namespace android
}  // namespace aidl
