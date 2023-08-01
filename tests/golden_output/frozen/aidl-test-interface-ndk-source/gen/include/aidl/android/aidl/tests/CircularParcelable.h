/*
 * This file is auto-generated.  DO NOT MODIFY.
 * Using: out/host/linux-x86/bin/aidl --lang=ndk -Weverything -Wno-missing-permission-annotation -Werror --min_sdk_version current --structured --ninja -d out/soong/.intermediates/system/tools/aidl/aidl-test-interface-ndk-source/gen/staging/android/aidl/tests/CircularParcelable.cpp.d -h out/soong/.intermediates/system/tools/aidl/aidl-test-interface-ndk-source/gen/include/staging -o out/soong/.intermediates/system/tools/aidl/aidl-test-interface-ndk-source/gen/staging -Nsystem/tools/aidl/tests system/tools/aidl/tests/android/aidl/tests/CircularParcelable.aidl
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
#include <aidl/android/aidl/tests/ITestService.h>
#ifdef BINDER_STABILITY_SUPPORT
#include <android/binder_stability.h>
#endif  // BINDER_STABILITY_SUPPORT

namespace aidl::android::aidl::tests {
class ITestService;
}  // namespace aidl::android::aidl::tests
namespace aidl {
namespace android {
namespace aidl {
namespace tests {
class CircularParcelable {
public:
  typedef std::false_type fixed_size;
  static const char* descriptor;

  std::shared_ptr<::aidl::android::aidl::tests::ITestService> testService;

  binder_status_t readFromParcel(const AParcel* parcel);
  binder_status_t writeToParcel(AParcel* parcel) const;

  inline bool operator!=(const CircularParcelable& rhs) const {
    return std::tie(testService) != std::tie(rhs.testService);
  }
  inline bool operator<(const CircularParcelable& rhs) const {
    return std::tie(testService) < std::tie(rhs.testService);
  }
  inline bool operator<=(const CircularParcelable& rhs) const {
    return std::tie(testService) <= std::tie(rhs.testService);
  }
  inline bool operator==(const CircularParcelable& rhs) const {
    return std::tie(testService) == std::tie(rhs.testService);
  }
  inline bool operator>(const CircularParcelable& rhs) const {
    return std::tie(testService) > std::tie(rhs.testService);
  }
  inline bool operator>=(const CircularParcelable& rhs) const {
    return std::tie(testService) >= std::tie(rhs.testService);
  }

  static const ::ndk::parcelable_stability_t _aidl_stability = ::ndk::STABILITY_LOCAL;
  inline std::string toString() const {
    std::ostringstream os;
    os << "CircularParcelable{";
    os << "testService: " << ::android::internal::ToString(testService);
    os << "}";
    return os.str();
  }
};
}  // namespace tests
}  // namespace aidl
}  // namespace android
}  // namespace aidl
