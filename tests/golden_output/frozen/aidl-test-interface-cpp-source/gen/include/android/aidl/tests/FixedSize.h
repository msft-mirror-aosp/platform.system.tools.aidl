/*
 * This file is auto-generated.  DO NOT MODIFY.
 * Using: out/host/linux-x86/bin/aidl --lang=cpp -Weverything -Wno-missing-permission-annotation -Werror -t --min_sdk_version current --ninja -d out/soong/.intermediates/system/tools/aidl/aidl-test-interface-cpp-source/gen/staging/android/aidl/tests/FixedSize.cpp.d -h out/soong/.intermediates/system/tools/aidl/aidl-test-interface-cpp-source/gen/include/staging -o out/soong/.intermediates/system/tools/aidl/aidl-test-interface-cpp-source/gen/staging -Nsystem/tools/aidl/tests system/tools/aidl/tests/android/aidl/tests/FixedSize.aidl
 *
 * DO NOT CHECK THIS FILE INTO A CODE TREE (e.g. git, etc..).
 * ALWAYS GENERATE THIS FILE FROM UPDATED AIDL COMPILER
 * AS A BUILD INTERMEDIATE ONLY. THIS IS NOT SOURCE CODE.
 */
#pragma once

#include <android/aidl/tests/FixedSize.h>
#include <android/aidl/tests/LongEnum.h>
#include <android/binder_to_string.h>
#include <array>
#include <binder/Enums.h>
#include <binder/Parcel.h>
#include <binder/Status.h>
#include <cassert>
#include <cstdint>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <utils/String16.h>
#include <variant>

#ifndef __BIONIC__
#define __assert2(a,b,c,d) ((void)0)
#endif

namespace android {
namespace aidl {
namespace tests {
class LIBBINDER_EXPORTED FixedSize : public ::android::Parcelable {
public:
  class LIBBINDER_EXPORTED FixedUnion : public ::android::Parcelable {
  public:
    enum class Tag : int8_t {
      booleanValue = 0,
      byteValue = 1,
      charValue = 2,
      intValue = 3,
      longValue = 4,
      floatValue = 5,
      intArray = 6,
      multiDimensionLongArray = 7,
      doubleValue = 8,
      enumValue = 9,
    };
    // Expose tag symbols for legacy code
    static const inline Tag booleanValue = Tag::booleanValue;
    static const inline Tag byteValue = Tag::byteValue;
    static const inline Tag charValue = Tag::charValue;
    static const inline Tag intValue = Tag::intValue;
    static const inline Tag longValue = Tag::longValue;
    static const inline Tag floatValue = Tag::floatValue;
    static const inline Tag intArray = Tag::intArray;
    static const inline Tag multiDimensionLongArray = Tag::multiDimensionLongArray;
    static const inline Tag doubleValue = Tag::doubleValue;
    static const inline Tag enumValue = Tag::enumValue;

    template<typename _Tp>
    static constexpr bool _not_self = !std::is_same_v<std::remove_cv_t<std::remove_reference_t<_Tp>>, FixedUnion>;

    FixedUnion() : _value(std::in_place_index<static_cast<size_t>(booleanValue)>, bool(false)) { }

    template <typename _Tp, typename = std::enable_if_t<
        _not_self<_Tp> &&
        std::is_constructible_v<std::variant<bool, int8_t, char16_t, int32_t, int64_t, float, std::array<int32_t, 3>, std::array<std::array<int64_t, 2>, 3>, double, ::android::aidl::tests::LongEnum>, _Tp>
      >>
    // NOLINTNEXTLINE(google-explicit-constructor)
    constexpr FixedUnion(_Tp&& _arg)
        : _value(std::forward<_Tp>(_arg)) {}

    template <size_t _Np, typename... _Tp>
    constexpr explicit FixedUnion(std::in_place_index_t<_Np>, _Tp&&... _args)
        : _value(std::in_place_index<_Np>, std::forward<_Tp>(_args)...) {}

    template <Tag _tag, typename... _Tp>
    static FixedUnion make(_Tp&&... _args) {
      return FixedUnion(std::in_place_index<static_cast<size_t>(_tag)>, std::forward<_Tp>(_args)...);
    }

    template <Tag _tag, typename _Tp, typename... _Up>
    static FixedUnion make(std::initializer_list<_Tp> _il, _Up&&... _args) {
      return FixedUnion(std::in_place_index<static_cast<size_t>(_tag)>, std::move(_il), std::forward<_Up>(_args)...);
    }

    Tag getTag() const {
      return static_cast<Tag>(_value.index());
    }

    template <Tag _tag>
    const auto& get() const {
      if (getTag() != _tag) { __assert2(__FILE__, __LINE__, __PRETTY_FUNCTION__, "bad access: a wrong tag"); }
      return std::get<static_cast<size_t>(_tag)>(_value);
    }

    template <Tag _tag>
    auto& get() {
      if (getTag() != _tag) { __assert2(__FILE__, __LINE__, __PRETTY_FUNCTION__, "bad access: a wrong tag"); }
      return std::get<static_cast<size_t>(_tag)>(_value);
    }

    template <Tag _tag, typename... _Tp>
    void set(_Tp&&... _args) {
      _value.emplace<static_cast<size_t>(_tag)>(std::forward<_Tp>(_args)...);
    }

    inline bool operator==(const FixedUnion& _rhs) const {
      return _value == _rhs._value;
    }
    inline bool operator<(const FixedUnion& _rhs) const {
      return _value < _rhs._value;
    }
    inline bool operator!=(const FixedUnion& _rhs) const {
      return !(*this == _rhs);
    }
    inline bool operator>(const FixedUnion& _rhs) const {
      return _rhs < *this;
    }
    inline bool operator>=(const FixedUnion& _rhs) const {
      return !(*this < _rhs);
    }
    inline bool operator<=(const FixedUnion& _rhs) const {
      return !(_rhs < *this);
    }

    ::android::status_t readFromParcel(const ::android::Parcel* _aidl_parcel) final;
    ::android::status_t writeToParcel(::android::Parcel* _aidl_parcel) const final;
    static const ::android::String16& getParcelableDescriptor() {
      [[clang::no_destroy]] static const ::android::StaticString16 DESCRIPTOR (u"android.aidl.tests.FixedSize.FixedUnion");
      return DESCRIPTOR;
    }
    inline std::string toString() const {
      std::ostringstream os;
      os << "FixedUnion{";
      switch (getTag()) {
      case booleanValue: os << "booleanValue: " << ::android::internal::ToString(get<booleanValue>()); break;
      case byteValue: os << "byteValue: " << ::android::internal::ToString(get<byteValue>()); break;
      case charValue: os << "charValue: " << ::android::internal::ToString(get<charValue>()); break;
      case intValue: os << "intValue: " << ::android::internal::ToString(get<intValue>()); break;
      case longValue: os << "longValue: " << ::android::internal::ToString(get<longValue>()); break;
      case floatValue: os << "floatValue: " << ::android::internal::ToString(get<floatValue>()); break;
      case intArray: os << "intArray: " << ::android::internal::ToString(get<intArray>()); break;
      case multiDimensionLongArray: os << "multiDimensionLongArray: " << ::android::internal::ToString(get<multiDimensionLongArray>()); break;
      case doubleValue: os << "doubleValue: " << ::android::internal::ToString(get<doubleValue>()); break;
      case enumValue: os << "enumValue: " << ::android::internal::ToString(get<enumValue>()); break;
      }
      os << "}";
      return os.str();
    }
  private:
    std::variant<bool, int8_t, char16_t, int32_t, int64_t, float, std::array<int32_t, 3>, std::array<std::array<int64_t, 2>, 3>, double, ::android::aidl::tests::LongEnum> _value;
  };  // class FixedUnion
  class LIBBINDER_EXPORTED EmptyParcelable : public ::android::Parcelable {
  public:
    inline bool operator==(const EmptyParcelable&) const {
      return std::tie() == std::tie();
    }
    inline bool operator<(const EmptyParcelable&) const {
      return std::tie() < std::tie();
    }
    inline bool operator!=(const EmptyParcelable& _rhs) const {
      return !(*this == _rhs);
    }
    inline bool operator>(const EmptyParcelable& _rhs) const {
      return _rhs < *this;
    }
    inline bool operator>=(const EmptyParcelable& _rhs) const {
      return !(*this < _rhs);
    }
    inline bool operator<=(const EmptyParcelable& _rhs) const {
      return !(_rhs < *this);
    }

    ::android::status_t readFromParcel(const ::android::Parcel* _aidl_parcel) final;
    ::android::status_t writeToParcel(::android::Parcel* _aidl_parcel) const final;
    static const ::android::String16& getParcelableDescriptor() {
      [[clang::no_destroy]] static const ::android::StaticString16 DESCRIPTOR (u"android.aidl.tests.FixedSize.EmptyParcelable");
      return DESCRIPTOR;
    }
    inline std::string toString() const {
      std::ostringstream _aidl_os;
      _aidl_os << "EmptyParcelable{";
      _aidl_os << "}";
      return _aidl_os.str();
    }
  };  // class EmptyParcelable
  class LIBBINDER_EXPORTED FixedParcelable : public ::android::Parcelable {
  public:
    bool booleanValue = false;
    int8_t byteValue = 0;
    char16_t charValue = '\0';
    int32_t intValue = 0;
    int64_t longValue = 0L;
    float floatValue = 0.000000f;
    std::array<int32_t, 3> intArray = {{}};
    std::array<std::array<int64_t, 2>, 3> multiDimensionLongArray = {{}};
    double doubleValue = 0.000000;
    ::android::aidl::tests::LongEnum enumValue = ::android::aidl::tests::LongEnum::FOO;
    ::android::aidl::tests::FixedSize::FixedUnion parcelableValue;
    std::array<::android::aidl::tests::FixedSize::EmptyParcelable, 3> parcelableArray = {{}};
    std::array<::android::aidl::tests::FixedSize::FixedUnion, 4> unionArray = {{}};
    inline bool operator==(const FixedParcelable& _rhs) const {
      return std::tie(booleanValue, byteValue, charValue, intValue, longValue, floatValue, intArray, multiDimensionLongArray, doubleValue, enumValue, parcelableValue, parcelableArray, unionArray) == std::tie(_rhs.booleanValue, _rhs.byteValue, _rhs.charValue, _rhs.intValue, _rhs.longValue, _rhs.floatValue, _rhs.intArray, _rhs.multiDimensionLongArray, _rhs.doubleValue, _rhs.enumValue, _rhs.parcelableValue, _rhs.parcelableArray, _rhs.unionArray);
    }
    inline bool operator<(const FixedParcelable& _rhs) const {
      return std::tie(booleanValue, byteValue, charValue, intValue, longValue, floatValue, intArray, multiDimensionLongArray, doubleValue, enumValue, parcelableValue, parcelableArray, unionArray) < std::tie(_rhs.booleanValue, _rhs.byteValue, _rhs.charValue, _rhs.intValue, _rhs.longValue, _rhs.floatValue, _rhs.intArray, _rhs.multiDimensionLongArray, _rhs.doubleValue, _rhs.enumValue, _rhs.parcelableValue, _rhs.parcelableArray, _rhs.unionArray);
    }
    inline bool operator!=(const FixedParcelable& _rhs) const {
      return !(*this == _rhs);
    }
    inline bool operator>(const FixedParcelable& _rhs) const {
      return _rhs < *this;
    }
    inline bool operator>=(const FixedParcelable& _rhs) const {
      return !(*this < _rhs);
    }
    inline bool operator<=(const FixedParcelable& _rhs) const {
      return !(_rhs < *this);
    }

    ::android::status_t readFromParcel(const ::android::Parcel* _aidl_parcel) final;
    ::android::status_t writeToParcel(::android::Parcel* _aidl_parcel) const final;
    static const ::android::String16& getParcelableDescriptor() {
      [[clang::no_destroy]] static const ::android::StaticString16 DESCRIPTOR (u"android.aidl.tests.FixedSize.FixedParcelable");
      return DESCRIPTOR;
    }
    inline std::string toString() const {
      std::ostringstream _aidl_os;
      _aidl_os << "FixedParcelable{";
      _aidl_os << "booleanValue: " << ::android::internal::ToString(booleanValue);
      _aidl_os << ", byteValue: " << ::android::internal::ToString(byteValue);
      _aidl_os << ", charValue: " << ::android::internal::ToString(charValue);
      _aidl_os << ", intValue: " << ::android::internal::ToString(intValue);
      _aidl_os << ", longValue: " << ::android::internal::ToString(longValue);
      _aidl_os << ", floatValue: " << ::android::internal::ToString(floatValue);
      _aidl_os << ", intArray: " << ::android::internal::ToString(intArray);
      _aidl_os << ", multiDimensionLongArray: " << ::android::internal::ToString(multiDimensionLongArray);
      _aidl_os << ", doubleValue: " << ::android::internal::ToString(doubleValue);
      _aidl_os << ", enumValue: " << ::android::internal::ToString(enumValue);
      _aidl_os << ", parcelableValue: " << ::android::internal::ToString(parcelableValue);
      _aidl_os << ", parcelableArray: " << ::android::internal::ToString(parcelableArray);
      _aidl_os << ", unionArray: " << ::android::internal::ToString(unionArray);
      _aidl_os << "}";
      return _aidl_os.str();
    }
  };  // class FixedParcelable
  class LIBBINDER_EXPORTED ExplicitPaddingParcelable : public ::android::Parcelable {
  public:
    int8_t byteValue = 0;
    int64_t longValue = 0L;
    char16_t charValue = '\0';
    double doubleValue = 0.000000;
    int32_t intValue = 0;
    ::android::aidl::tests::LongEnum enumValue = ::android::aidl::tests::LongEnum::FOO;
    inline bool operator==(const ExplicitPaddingParcelable& _rhs) const {
      return std::tie(byteValue, longValue, charValue, doubleValue, intValue, enumValue) == std::tie(_rhs.byteValue, _rhs.longValue, _rhs.charValue, _rhs.doubleValue, _rhs.intValue, _rhs.enumValue);
    }
    inline bool operator<(const ExplicitPaddingParcelable& _rhs) const {
      return std::tie(byteValue, longValue, charValue, doubleValue, intValue, enumValue) < std::tie(_rhs.byteValue, _rhs.longValue, _rhs.charValue, _rhs.doubleValue, _rhs.intValue, _rhs.enumValue);
    }
    inline bool operator!=(const ExplicitPaddingParcelable& _rhs) const {
      return !(*this == _rhs);
    }
    inline bool operator>(const ExplicitPaddingParcelable& _rhs) const {
      return _rhs < *this;
    }
    inline bool operator>=(const ExplicitPaddingParcelable& _rhs) const {
      return !(*this < _rhs);
    }
    inline bool operator<=(const ExplicitPaddingParcelable& _rhs) const {
      return !(_rhs < *this);
    }

    ::android::status_t readFromParcel(const ::android::Parcel* _aidl_parcel) final;
    ::android::status_t writeToParcel(::android::Parcel* _aidl_parcel) const final;
    static const ::android::String16& getParcelableDescriptor() {
      [[clang::no_destroy]] static const ::android::StaticString16 DESCRIPTOR (u"android.aidl.tests.FixedSize.ExplicitPaddingParcelable");
      return DESCRIPTOR;
    }
    inline std::string toString() const {
      std::ostringstream _aidl_os;
      _aidl_os << "ExplicitPaddingParcelable{";
      _aidl_os << "byteValue: " << ::android::internal::ToString(byteValue);
      _aidl_os << ", longValue: " << ::android::internal::ToString(longValue);
      _aidl_os << ", charValue: " << ::android::internal::ToString(charValue);
      _aidl_os << ", doubleValue: " << ::android::internal::ToString(doubleValue);
      _aidl_os << ", intValue: " << ::android::internal::ToString(intValue);
      _aidl_os << ", enumValue: " << ::android::internal::ToString(enumValue);
      _aidl_os << "}";
      return _aidl_os.str();
    }
  };  // class ExplicitPaddingParcelable
  class LIBBINDER_EXPORTED FixedUnionNoPadding : public ::android::Parcelable {
  public:
    enum class Tag : int8_t {
      byteValue = 0,
    };
    // Expose tag symbols for legacy code
    static const inline Tag byteValue = Tag::byteValue;

    template<typename _Tp>
    static constexpr bool _not_self = !std::is_same_v<std::remove_cv_t<std::remove_reference_t<_Tp>>, FixedUnionNoPadding>;

    FixedUnionNoPadding() : _value(std::in_place_index<static_cast<size_t>(byteValue)>, int8_t(0)) { }

    template <typename _Tp, typename = std::enable_if_t<
        _not_self<_Tp> &&
        std::is_constructible_v<std::variant<int8_t>, _Tp>
      >>
    // NOLINTNEXTLINE(google-explicit-constructor)
    constexpr FixedUnionNoPadding(_Tp&& _arg)
        : _value(std::forward<_Tp>(_arg)) {}

    template <size_t _Np, typename... _Tp>
    constexpr explicit FixedUnionNoPadding(std::in_place_index_t<_Np>, _Tp&&... _args)
        : _value(std::in_place_index<_Np>, std::forward<_Tp>(_args)...) {}

    template <Tag _tag, typename... _Tp>
    static FixedUnionNoPadding make(_Tp&&... _args) {
      return FixedUnionNoPadding(std::in_place_index<static_cast<size_t>(_tag)>, std::forward<_Tp>(_args)...);
    }

    template <Tag _tag, typename _Tp, typename... _Up>
    static FixedUnionNoPadding make(std::initializer_list<_Tp> _il, _Up&&... _args) {
      return FixedUnionNoPadding(std::in_place_index<static_cast<size_t>(_tag)>, std::move(_il), std::forward<_Up>(_args)...);
    }

    Tag getTag() const {
      return static_cast<Tag>(_value.index());
    }

    template <Tag _tag>
    const auto& get() const {
      if (getTag() != _tag) { __assert2(__FILE__, __LINE__, __PRETTY_FUNCTION__, "bad access: a wrong tag"); }
      return std::get<static_cast<size_t>(_tag)>(_value);
    }

    template <Tag _tag>
    auto& get() {
      if (getTag() != _tag) { __assert2(__FILE__, __LINE__, __PRETTY_FUNCTION__, "bad access: a wrong tag"); }
      return std::get<static_cast<size_t>(_tag)>(_value);
    }

    template <Tag _tag, typename... _Tp>
    void set(_Tp&&... _args) {
      _value.emplace<static_cast<size_t>(_tag)>(std::forward<_Tp>(_args)...);
    }

    inline bool operator==(const FixedUnionNoPadding& _rhs) const {
      return _value == _rhs._value;
    }
    inline bool operator<(const FixedUnionNoPadding& _rhs) const {
      return _value < _rhs._value;
    }
    inline bool operator!=(const FixedUnionNoPadding& _rhs) const {
      return !(*this == _rhs);
    }
    inline bool operator>(const FixedUnionNoPadding& _rhs) const {
      return _rhs < *this;
    }
    inline bool operator>=(const FixedUnionNoPadding& _rhs) const {
      return !(*this < _rhs);
    }
    inline bool operator<=(const FixedUnionNoPadding& _rhs) const {
      return !(_rhs < *this);
    }

    ::android::status_t readFromParcel(const ::android::Parcel* _aidl_parcel) final;
    ::android::status_t writeToParcel(::android::Parcel* _aidl_parcel) const final;
    static const ::android::String16& getParcelableDescriptor() {
      [[clang::no_destroy]] static const ::android::StaticString16 DESCRIPTOR (u"android.aidl.tests.FixedSize.FixedUnionNoPadding");
      return DESCRIPTOR;
    }
    inline std::string toString() const {
      std::ostringstream os;
      os << "FixedUnionNoPadding{";
      switch (getTag()) {
      case byteValue: os << "byteValue: " << ::android::internal::ToString(get<byteValue>()); break;
      }
      os << "}";
      return os.str();
    }
  private:
    std::variant<int8_t> _value;
  };  // class FixedUnionNoPadding
  class LIBBINDER_EXPORTED FixedUnionSmallPadding : public ::android::Parcelable {
  public:
    enum class Tag : int8_t {
      charValue = 0,
    };
    // Expose tag symbols for legacy code
    static const inline Tag charValue = Tag::charValue;

    template<typename _Tp>
    static constexpr bool _not_self = !std::is_same_v<std::remove_cv_t<std::remove_reference_t<_Tp>>, FixedUnionSmallPadding>;

    FixedUnionSmallPadding() : _value(std::in_place_index<static_cast<size_t>(charValue)>, char16_t('\0')) { }

    template <typename _Tp, typename = std::enable_if_t<
        _not_self<_Tp> &&
        std::is_constructible_v<std::variant<char16_t>, _Tp>
      >>
    // NOLINTNEXTLINE(google-explicit-constructor)
    constexpr FixedUnionSmallPadding(_Tp&& _arg)
        : _value(std::forward<_Tp>(_arg)) {}

    template <size_t _Np, typename... _Tp>
    constexpr explicit FixedUnionSmallPadding(std::in_place_index_t<_Np>, _Tp&&... _args)
        : _value(std::in_place_index<_Np>, std::forward<_Tp>(_args)...) {}

    template <Tag _tag, typename... _Tp>
    static FixedUnionSmallPadding make(_Tp&&... _args) {
      return FixedUnionSmallPadding(std::in_place_index<static_cast<size_t>(_tag)>, std::forward<_Tp>(_args)...);
    }

    template <Tag _tag, typename _Tp, typename... _Up>
    static FixedUnionSmallPadding make(std::initializer_list<_Tp> _il, _Up&&... _args) {
      return FixedUnionSmallPadding(std::in_place_index<static_cast<size_t>(_tag)>, std::move(_il), std::forward<_Up>(_args)...);
    }

    Tag getTag() const {
      return static_cast<Tag>(_value.index());
    }

    template <Tag _tag>
    const auto& get() const {
      if (getTag() != _tag) { __assert2(__FILE__, __LINE__, __PRETTY_FUNCTION__, "bad access: a wrong tag"); }
      return std::get<static_cast<size_t>(_tag)>(_value);
    }

    template <Tag _tag>
    auto& get() {
      if (getTag() != _tag) { __assert2(__FILE__, __LINE__, __PRETTY_FUNCTION__, "bad access: a wrong tag"); }
      return std::get<static_cast<size_t>(_tag)>(_value);
    }

    template <Tag _tag, typename... _Tp>
    void set(_Tp&&... _args) {
      _value.emplace<static_cast<size_t>(_tag)>(std::forward<_Tp>(_args)...);
    }

    inline bool operator==(const FixedUnionSmallPadding& _rhs) const {
      return _value == _rhs._value;
    }
    inline bool operator<(const FixedUnionSmallPadding& _rhs) const {
      return _value < _rhs._value;
    }
    inline bool operator!=(const FixedUnionSmallPadding& _rhs) const {
      return !(*this == _rhs);
    }
    inline bool operator>(const FixedUnionSmallPadding& _rhs) const {
      return _rhs < *this;
    }
    inline bool operator>=(const FixedUnionSmallPadding& _rhs) const {
      return !(*this < _rhs);
    }
    inline bool operator<=(const FixedUnionSmallPadding& _rhs) const {
      return !(_rhs < *this);
    }

    ::android::status_t readFromParcel(const ::android::Parcel* _aidl_parcel) final;
    ::android::status_t writeToParcel(::android::Parcel* _aidl_parcel) const final;
    static const ::android::String16& getParcelableDescriptor() {
      [[clang::no_destroy]] static const ::android::StaticString16 DESCRIPTOR (u"android.aidl.tests.FixedSize.FixedUnionSmallPadding");
      return DESCRIPTOR;
    }
    inline std::string toString() const {
      std::ostringstream os;
      os << "FixedUnionSmallPadding{";
      switch (getTag()) {
      case charValue: os << "charValue: " << ::android::internal::ToString(get<charValue>()); break;
      }
      os << "}";
      return os.str();
    }
  private:
    std::variant<char16_t> _value;
  };  // class FixedUnionSmallPadding
  class LIBBINDER_EXPORTED FixedUnionLongPadding : public ::android::Parcelable {
  public:
    enum class Tag : int8_t {
      longValue = 0,
    };
    // Expose tag symbols for legacy code
    static const inline Tag longValue = Tag::longValue;

    template<typename _Tp>
    static constexpr bool _not_self = !std::is_same_v<std::remove_cv_t<std::remove_reference_t<_Tp>>, FixedUnionLongPadding>;

    FixedUnionLongPadding() : _value(std::in_place_index<static_cast<size_t>(longValue)>, int64_t(0L)) { }

    template <typename _Tp, typename = std::enable_if_t<
        _not_self<_Tp> &&
        std::is_constructible_v<std::variant<int64_t>, _Tp>
      >>
    // NOLINTNEXTLINE(google-explicit-constructor)
    constexpr FixedUnionLongPadding(_Tp&& _arg)
        : _value(std::forward<_Tp>(_arg)) {}

    template <size_t _Np, typename... _Tp>
    constexpr explicit FixedUnionLongPadding(std::in_place_index_t<_Np>, _Tp&&... _args)
        : _value(std::in_place_index<_Np>, std::forward<_Tp>(_args)...) {}

    template <Tag _tag, typename... _Tp>
    static FixedUnionLongPadding make(_Tp&&... _args) {
      return FixedUnionLongPadding(std::in_place_index<static_cast<size_t>(_tag)>, std::forward<_Tp>(_args)...);
    }

    template <Tag _tag, typename _Tp, typename... _Up>
    static FixedUnionLongPadding make(std::initializer_list<_Tp> _il, _Up&&... _args) {
      return FixedUnionLongPadding(std::in_place_index<static_cast<size_t>(_tag)>, std::move(_il), std::forward<_Up>(_args)...);
    }

    Tag getTag() const {
      return static_cast<Tag>(_value.index());
    }

    template <Tag _tag>
    const auto& get() const {
      if (getTag() != _tag) { __assert2(__FILE__, __LINE__, __PRETTY_FUNCTION__, "bad access: a wrong tag"); }
      return std::get<static_cast<size_t>(_tag)>(_value);
    }

    template <Tag _tag>
    auto& get() {
      if (getTag() != _tag) { __assert2(__FILE__, __LINE__, __PRETTY_FUNCTION__, "bad access: a wrong tag"); }
      return std::get<static_cast<size_t>(_tag)>(_value);
    }

    template <Tag _tag, typename... _Tp>
    void set(_Tp&&... _args) {
      _value.emplace<static_cast<size_t>(_tag)>(std::forward<_Tp>(_args)...);
    }

    inline bool operator==(const FixedUnionLongPadding& _rhs) const {
      return _value == _rhs._value;
    }
    inline bool operator<(const FixedUnionLongPadding& _rhs) const {
      return _value < _rhs._value;
    }
    inline bool operator!=(const FixedUnionLongPadding& _rhs) const {
      return !(*this == _rhs);
    }
    inline bool operator>(const FixedUnionLongPadding& _rhs) const {
      return _rhs < *this;
    }
    inline bool operator>=(const FixedUnionLongPadding& _rhs) const {
      return !(*this < _rhs);
    }
    inline bool operator<=(const FixedUnionLongPadding& _rhs) const {
      return !(_rhs < *this);
    }

    ::android::status_t readFromParcel(const ::android::Parcel* _aidl_parcel) final;
    ::android::status_t writeToParcel(::android::Parcel* _aidl_parcel) const final;
    static const ::android::String16& getParcelableDescriptor() {
      [[clang::no_destroy]] static const ::android::StaticString16 DESCRIPTOR (u"android.aidl.tests.FixedSize.FixedUnionLongPadding");
      return DESCRIPTOR;
    }
    inline std::string toString() const {
      std::ostringstream os;
      os << "FixedUnionLongPadding{";
      switch (getTag()) {
      case longValue: os << "longValue: " << ::android::internal::ToString(get<longValue>()); break;
      }
      os << "}";
      return os.str();
    }
  private:
    std::variant<int64_t> _value;
  };  // class FixedUnionLongPadding
  inline bool operator==(const FixedSize&) const {
    return std::tie() == std::tie();
  }
  inline bool operator<(const FixedSize&) const {
    return std::tie() < std::tie();
  }
  inline bool operator!=(const FixedSize& _rhs) const {
    return !(*this == _rhs);
  }
  inline bool operator>(const FixedSize& _rhs) const {
    return _rhs < *this;
  }
  inline bool operator>=(const FixedSize& _rhs) const {
    return !(*this < _rhs);
  }
  inline bool operator<=(const FixedSize& _rhs) const {
    return !(_rhs < *this);
  }

  ::android::status_t readFromParcel(const ::android::Parcel* _aidl_parcel) final;
  ::android::status_t writeToParcel(::android::Parcel* _aidl_parcel) const final;
  static const ::android::String16& getParcelableDescriptor() {
    [[clang::no_destroy]] static const ::android::StaticString16 DESCRIPTOR (u"android.aidl.tests.FixedSize");
    return DESCRIPTOR;
  }
  inline std::string toString() const {
    std::ostringstream _aidl_os;
    _aidl_os << "FixedSize{";
    _aidl_os << "}";
    return _aidl_os.str();
  }
};  // class FixedSize
}  // namespace tests
}  // namespace aidl
}  // namespace android
namespace android {
namespace aidl {
namespace tests {
[[nodiscard]] static inline std::string toString(FixedSize::FixedUnion::Tag val) {
  switch(val) {
  case FixedSize::FixedUnion::Tag::booleanValue:
    return "booleanValue";
  case FixedSize::FixedUnion::Tag::byteValue:
    return "byteValue";
  case FixedSize::FixedUnion::Tag::charValue:
    return "charValue";
  case FixedSize::FixedUnion::Tag::intValue:
    return "intValue";
  case FixedSize::FixedUnion::Tag::longValue:
    return "longValue";
  case FixedSize::FixedUnion::Tag::floatValue:
    return "floatValue";
  case FixedSize::FixedUnion::Tag::intArray:
    return "intArray";
  case FixedSize::FixedUnion::Tag::multiDimensionLongArray:
    return "multiDimensionLongArray";
  case FixedSize::FixedUnion::Tag::doubleValue:
    return "doubleValue";
  case FixedSize::FixedUnion::Tag::enumValue:
    return "enumValue";
  default:
    return std::to_string(static_cast<int8_t>(val));
  }
}
}  // namespace tests
}  // namespace aidl
}  // namespace android
namespace android {
namespace internal {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++17-extensions"
template <>
constexpr inline std::array<::android::aidl::tests::FixedSize::FixedUnion::Tag, 10> enum_values<::android::aidl::tests::FixedSize::FixedUnion::Tag> = {
  ::android::aidl::tests::FixedSize::FixedUnion::Tag::booleanValue,
  ::android::aidl::tests::FixedSize::FixedUnion::Tag::byteValue,
  ::android::aidl::tests::FixedSize::FixedUnion::Tag::charValue,
  ::android::aidl::tests::FixedSize::FixedUnion::Tag::intValue,
  ::android::aidl::tests::FixedSize::FixedUnion::Tag::longValue,
  ::android::aidl::tests::FixedSize::FixedUnion::Tag::floatValue,
  ::android::aidl::tests::FixedSize::FixedUnion::Tag::intArray,
  ::android::aidl::tests::FixedSize::FixedUnion::Tag::multiDimensionLongArray,
  ::android::aidl::tests::FixedSize::FixedUnion::Tag::doubleValue,
  ::android::aidl::tests::FixedSize::FixedUnion::Tag::enumValue,
};
#pragma clang diagnostic pop
}  // namespace internal
}  // namespace android
namespace android {
namespace aidl {
namespace tests {
[[nodiscard]] static inline std::string toString(FixedSize::FixedUnionNoPadding::Tag val) {
  switch(val) {
  case FixedSize::FixedUnionNoPadding::Tag::byteValue:
    return "byteValue";
  default:
    return std::to_string(static_cast<int8_t>(val));
  }
}
}  // namespace tests
}  // namespace aidl
}  // namespace android
namespace android {
namespace internal {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++17-extensions"
template <>
constexpr inline std::array<::android::aidl::tests::FixedSize::FixedUnionNoPadding::Tag, 1> enum_values<::android::aidl::tests::FixedSize::FixedUnionNoPadding::Tag> = {
  ::android::aidl::tests::FixedSize::FixedUnionNoPadding::Tag::byteValue,
};
#pragma clang diagnostic pop
}  // namespace internal
}  // namespace android
namespace android {
namespace aidl {
namespace tests {
[[nodiscard]] static inline std::string toString(FixedSize::FixedUnionSmallPadding::Tag val) {
  switch(val) {
  case FixedSize::FixedUnionSmallPadding::Tag::charValue:
    return "charValue";
  default:
    return std::to_string(static_cast<int8_t>(val));
  }
}
}  // namespace tests
}  // namespace aidl
}  // namespace android
namespace android {
namespace internal {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++17-extensions"
template <>
constexpr inline std::array<::android::aidl::tests::FixedSize::FixedUnionSmallPadding::Tag, 1> enum_values<::android::aidl::tests::FixedSize::FixedUnionSmallPadding::Tag> = {
  ::android::aidl::tests::FixedSize::FixedUnionSmallPadding::Tag::charValue,
};
#pragma clang diagnostic pop
}  // namespace internal
}  // namespace android
namespace android {
namespace aidl {
namespace tests {
[[nodiscard]] static inline std::string toString(FixedSize::FixedUnionLongPadding::Tag val) {
  switch(val) {
  case FixedSize::FixedUnionLongPadding::Tag::longValue:
    return "longValue";
  default:
    return std::to_string(static_cast<int8_t>(val));
  }
}
}  // namespace tests
}  // namespace aidl
}  // namespace android
namespace android {
namespace internal {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++17-extensions"
template <>
constexpr inline std::array<::android::aidl::tests::FixedSize::FixedUnionLongPadding::Tag, 1> enum_values<::android::aidl::tests::FixedSize::FixedUnionLongPadding::Tag> = {
  ::android::aidl::tests::FixedSize::FixedUnionLongPadding::Tag::longValue,
};
#pragma clang diagnostic pop
}  // namespace internal
}  // namespace android
