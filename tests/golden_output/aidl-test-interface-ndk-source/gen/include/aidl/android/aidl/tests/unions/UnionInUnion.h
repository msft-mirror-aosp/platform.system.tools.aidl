#pragma once

#include <cassert>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>
#include <android/binder_interface_utils.h>
#include <android/binder_parcelable_utils.h>
#include <android/binder_to_string.h>
#include <aidl/android/aidl/tests/unions/EnumUnion.h>
#ifdef BINDER_STABILITY_SUPPORT
#include <android/binder_stability.h>
#endif  // BINDER_STABILITY_SUPPORT

#ifndef __BIONIC__
#define __assert2(a,b,c,d) ((void)0)
#endif

namespace aidl {
namespace android {
namespace aidl {
namespace tests {
namespace unions {
class UnionInUnion {
public:
  typedef std::false_type fixed_size;
  static const char* descriptor;

  enum Tag : int32_t {
    first = 0,  // android.aidl.tests.unions.EnumUnion first;
    second,  // int second;
  };

  template<typename _Tp>
  static constexpr bool _not_self = !std::is_same_v<std::remove_cv_t<std::remove_reference_t<_Tp>>, UnionInUnion>;

  UnionInUnion() : _value(std::in_place_index<first>, ::aidl::android::aidl::tests::unions::EnumUnion()) { }

  template <typename _Tp, typename = std::enable_if_t<_not_self<_Tp>>>
  // NOLINTNEXTLINE(google-explicit-constructor)
  constexpr UnionInUnion(_Tp&& _arg)
      : _value(std::forward<_Tp>(_arg)) {}

  template <size_t _Np, typename... _Tp>
  constexpr explicit UnionInUnion(std::in_place_index_t<_Np>, _Tp&&... _args)
      : _value(std::in_place_index<_Np>, std::forward<_Tp>(_args)...) {}

  template <Tag _tag, typename... _Tp>
  static UnionInUnion make(_Tp&&... _args) {
    return UnionInUnion(std::in_place_index<_tag>, std::forward<_Tp>(_args)...);
  }

  template <Tag _tag, typename _Tp, typename... _Up>
  static UnionInUnion make(std::initializer_list<_Tp> _il, _Up&&... _args) {
    return UnionInUnion(std::in_place_index<_tag>, std::move(_il), std::forward<_Up>(_args)...);
  }

  Tag getTag() const {
    return static_cast<Tag>(_value.index());
  }

  template <Tag _tag>
  const auto& get() const {
    if (getTag() != _tag) { __assert2(__FILE__, __LINE__, __PRETTY_FUNCTION__, "bad access: a wrong tag"); }
    return std::get<_tag>(_value);
  }

  template <Tag _tag>
  auto& get() {
    if (getTag() != _tag) { __assert2(__FILE__, __LINE__, __PRETTY_FUNCTION__, "bad access: a wrong tag"); }
    return std::get<_tag>(_value);
  }

  template <Tag _tag, typename... _Tp>
  void set(_Tp&&... _args) {
    _value.emplace<_tag>(std::forward<_Tp>(_args)...);
  }

  binder_status_t readFromParcel(const AParcel* _parcel);
  binder_status_t writeToParcel(AParcel* _parcel) const;

  inline bool operator!=(const UnionInUnion& rhs) const {
    return _value != rhs._value;
  }
  inline bool operator<(const UnionInUnion& rhs) const {
    return _value < rhs._value;
  }
  inline bool operator<=(const UnionInUnion& rhs) const {
    return _value <= rhs._value;
  }
  inline bool operator==(const UnionInUnion& rhs) const {
    return _value == rhs._value;
  }
  inline bool operator>(const UnionInUnion& rhs) const {
    return _value > rhs._value;
  }
  inline bool operator>=(const UnionInUnion& rhs) const {
    return _value >= rhs._value;
  }

  static const ::ndk::parcelable_stability_t _aidl_stability = ::ndk::STABILITY_LOCAL;
  inline std::string toString() const {
    std::ostringstream os;
    os << "UnionInUnion{";
    switch (getTag()) {
    case first: os << "first: " << ::android::internal::ToString(get<first>()); break;
    case second: os << "second: " << ::android::internal::ToString(get<second>()); break;
    }
    os << "}";
    return os.str();
  }
private:
  std::variant<::aidl::android::aidl::tests::unions::EnumUnion, int32_t> _value;
};
}  // namespace unions
}  // namespace tests
}  // namespace aidl
}  // namespace android
}  // namespace aidl