/*
 * This file is auto-generated.  DO NOT MODIFY.
 * Using: out/host/linux-x86/bin/aidl --lang=ndk --version 3 --hash 70d76c61eb0c82288e924862c10b910d1b7d8cf8 --min_sdk_version current --structured --ninja -d out/soong/.intermediates/system/tools/aidl/aidl-test-versioned-interface-V3-ndk-source/gen/staging/android/aidl/versioned/tests/Foo.cpp.d -h out/soong/.intermediates/system/tools/aidl/aidl-test-versioned-interface-V3-ndk-source/gen/include/staging -o out/soong/.intermediates/system/tools/aidl/aidl-test-versioned-interface-V3-ndk-source/gen/staging -Nsystem/tools/aidl/aidl_api/aidl-test-versioned-interface/3 system/tools/aidl/aidl_api/aidl-test-versioned-interface/3/android/aidl/versioned/tests/Foo.aidl
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