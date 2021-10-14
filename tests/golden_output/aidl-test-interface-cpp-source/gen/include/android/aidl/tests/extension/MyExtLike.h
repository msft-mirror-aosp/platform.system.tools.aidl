#pragma once

#include <android/binder_to_string.h>
#include <binder/Parcel.h>
#include <binder/Status.h>
#include <cstdint>
#include <tuple>
#include <utils/String16.h>

namespace android {
namespace aidl {
namespace tests {
namespace extension {
class MyExtLike : public ::android::Parcelable {
public:
  inline bool operator!=(const MyExtLike& rhs) const {
    return std::tie(a, b) != std::tie(rhs.a, rhs.b);
  }
  inline bool operator<(const MyExtLike& rhs) const {
    return std::tie(a, b) < std::tie(rhs.a, rhs.b);
  }
  inline bool operator<=(const MyExtLike& rhs) const {
    return std::tie(a, b) <= std::tie(rhs.a, rhs.b);
  }
  inline bool operator==(const MyExtLike& rhs) const {
    return std::tie(a, b) == std::tie(rhs.a, rhs.b);
  }
  inline bool operator>(const MyExtLike& rhs) const {
    return std::tie(a, b) > std::tie(rhs.a, rhs.b);
  }
  inline bool operator>=(const MyExtLike& rhs) const {
    return std::tie(a, b) >= std::tie(rhs.a, rhs.b);
  }

  int32_t a = int32_t(0);
  ::android::String16 b;
  ::android::status_t readFromParcel(const ::android::Parcel* _aidl_parcel) final;
  ::android::status_t writeToParcel(::android::Parcel* _aidl_parcel) const final;
  static const ::android::String16& getParcelableDescriptor() {
    static const ::android::StaticString16 DESCIPTOR (u"android.aidl.tests.extension.MyExtLike");
    return DESCIPTOR;
  }
  inline std::string toString() const {
    std::ostringstream os;
    os << "MyExtLike{";
    os << "a: " << ::android::internal::ToString(a);
    os << ", b: " << ::android::internal::ToString(b);
    os << "}";
    return os.str();
  }
};  // class MyExtLike
}  // namespace extension
}  // namespace tests
}  // namespace aidl
}  // namespace android
