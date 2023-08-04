/*
 * This file is auto-generated.  DO NOT MODIFY.
 * Using: out/host/linux-x86/bin/aidl --lang=cpp -Weverything -Wno-missing-permission-annotation -Werror -t --min_sdk_version current --structured --ninja -d out/soong/.intermediates/system/tools/aidl/aidl-test-interface-cpp-source/gen/staging/android/aidl/tests/RecursiveList.cpp.d -h out/soong/.intermediates/system/tools/aidl/aidl-test-interface-cpp-source/gen/include/staging -o out/soong/.intermediates/system/tools/aidl/aidl-test-interface-cpp-source/gen/staging -Nsystem/tools/aidl/tests system/tools/aidl/tests/android/aidl/tests/RecursiveList.aidl
 */
#pragma once

#include <android/aidl/tests/RecursiveList.h>
#include <android/binder_to_string.h>
#include <binder/Parcel.h>
#include <binder/Status.h>
#include <cstdint>
#include <optional>
#include <tuple>
#include <utils/String16.h>

namespace android::aidl::tests {
class RecursiveList;
}  // namespace android::aidl::tests
namespace android {
namespace aidl {
namespace tests {
class RecursiveList : public ::android::Parcelable {
public:
  int32_t value = 0;
  ::std::unique_ptr<::android::aidl::tests::RecursiveList> next;
  inline bool operator!=(const RecursiveList& rhs) const {
    return std::tie(value, next) != std::tie(rhs.value, rhs.next);
  }
  inline bool operator<(const RecursiveList& rhs) const {
    return std::tie(value, next) < std::tie(rhs.value, rhs.next);
  }
  inline bool operator<=(const RecursiveList& rhs) const {
    return std::tie(value, next) <= std::tie(rhs.value, rhs.next);
  }
  inline bool operator==(const RecursiveList& rhs) const {
    return std::tie(value, next) == std::tie(rhs.value, rhs.next);
  }
  inline bool operator>(const RecursiveList& rhs) const {
    return std::tie(value, next) > std::tie(rhs.value, rhs.next);
  }
  inline bool operator>=(const RecursiveList& rhs) const {
    return std::tie(value, next) >= std::tie(rhs.value, rhs.next);
  }

  ::android::status_t readFromParcel(const ::android::Parcel* _aidl_parcel) final;
  ::android::status_t writeToParcel(::android::Parcel* _aidl_parcel) const final;
  static const ::android::String16& getParcelableDescriptor() {
    static const ::android::StaticString16 DESCRIPTOR (u"android.aidl.tests.RecursiveList");
    return DESCRIPTOR;
  }
  inline std::string toString() const {
    std::ostringstream os;
    os << "RecursiveList{";
    os << "value: " << ::android::internal::ToString(value);
    os << ", next: " << ::android::internal::ToString(next);
    os << "}";
    return os.str();
  }
};  // class RecursiveList
}  // namespace tests
}  // namespace aidl
}  // namespace android
