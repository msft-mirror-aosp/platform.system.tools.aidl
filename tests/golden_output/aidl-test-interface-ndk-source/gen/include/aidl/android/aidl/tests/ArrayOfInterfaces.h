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
#include <android/binder_ibinder.h>
#include <android/binder_interface_utils.h>
#include <android/binder_parcelable_utils.h>
#include <android/binder_to_string.h>
#include <aidl/android/aidl/tests/ArrayOfInterfaces.h>
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
class ArrayOfInterfaces {
public:
  typedef std::false_type fixed_size;
  static const char* descriptor;

  class IEmptyInterface : public ::ndk::ICInterface {
  public:
    static const char* descriptor;
    IEmptyInterface();
    virtual ~IEmptyInterface();


    static std::shared_ptr<IEmptyInterface> fromBinder(const ::ndk::SpAIBinder& binder);
    static binder_status_t writeToParcel(AParcel* parcel, const std::shared_ptr<IEmptyInterface>& instance);
    static binder_status_t readFromParcel(const AParcel* parcel, std::shared_ptr<IEmptyInterface>* instance);
    static bool setDefaultImpl(const std::shared_ptr<IEmptyInterface>& impl);
    static const std::shared_ptr<IEmptyInterface>& getDefaultImpl();
  private:
    static std::shared_ptr<IEmptyInterface> default_impl;
  };
  class IEmptyInterfaceDefault : public IEmptyInterface {
  public:
    ::ndk::SpAIBinder asBinder() override;
    bool isRemote() override;
  };
  class BpEmptyInterface : public ::ndk::BpCInterface<IEmptyInterface> {
  public:
    explicit BpEmptyInterface(const ::ndk::SpAIBinder& binder);
    virtual ~BpEmptyInterface();

  };
  class BnEmptyInterface : public ::ndk::BnCInterface<IEmptyInterface> {
  public:
    BnEmptyInterface();
    virtual ~BnEmptyInterface();
  protected:
    ::ndk::SpAIBinder createBinder() override;
  private:
  };
  class IMyInterface : public ::ndk::ICInterface {
  public:
    static const char* descriptor;
    IMyInterface();
    virtual ~IMyInterface();

    static constexpr uint32_t TRANSACTION_methodWithInterfaces = FIRST_CALL_TRANSACTION + 0;

    static std::shared_ptr<IMyInterface> fromBinder(const ::ndk::SpAIBinder& binder);
    static binder_status_t writeToParcel(AParcel* parcel, const std::shared_ptr<IMyInterface>& instance);
    static binder_status_t readFromParcel(const AParcel* parcel, std::shared_ptr<IMyInterface>* instance);
    static bool setDefaultImpl(const std::shared_ptr<IMyInterface>& impl);
    static const std::shared_ptr<IMyInterface>& getDefaultImpl();
    virtual ::ndk::ScopedAStatus methodWithInterfaces(const std::shared_ptr<::aidl::android::aidl::tests::ArrayOfInterfaces::IEmptyInterface>& in_iface, const std::shared_ptr<::aidl::android::aidl::tests::ArrayOfInterfaces::IEmptyInterface>& in_nullable_iface, const std::vector<std::shared_ptr<::aidl::android::aidl::tests::ArrayOfInterfaces::IEmptyInterface>>& in_iface_array_in, std::vector<std::shared_ptr<::aidl::android::aidl::tests::ArrayOfInterfaces::IEmptyInterface>>* out_iface_array_out, std::vector<std::shared_ptr<::aidl::android::aidl::tests::ArrayOfInterfaces::IEmptyInterface>>* in_iface_array_inout, const std::optional<std::vector<std::shared_ptr<::aidl::android::aidl::tests::ArrayOfInterfaces::IEmptyInterface>>>& in_nullable_iface_array_in, std::optional<std::vector<std::shared_ptr<::aidl::android::aidl::tests::ArrayOfInterfaces::IEmptyInterface>>>* out_nullable_iface_array_out, std::optional<std::vector<std::shared_ptr<::aidl::android::aidl::tests::ArrayOfInterfaces::IEmptyInterface>>>* in_nullable_iface_array_inout, std::optional<std::vector<std::shared_ptr<::aidl::android::aidl::tests::ArrayOfInterfaces::IEmptyInterface>>>* _aidl_return) = 0;
  private:
    static std::shared_ptr<IMyInterface> default_impl;
  };
  class IMyInterfaceDefault : public IMyInterface {
  public:
    ::ndk::ScopedAStatus methodWithInterfaces(const std::shared_ptr<::aidl::android::aidl::tests::ArrayOfInterfaces::IEmptyInterface>& in_iface, const std::shared_ptr<::aidl::android::aidl::tests::ArrayOfInterfaces::IEmptyInterface>& in_nullable_iface, const std::vector<std::shared_ptr<::aidl::android::aidl::tests::ArrayOfInterfaces::IEmptyInterface>>& in_iface_array_in, std::vector<std::shared_ptr<::aidl::android::aidl::tests::ArrayOfInterfaces::IEmptyInterface>>* out_iface_array_out, std::vector<std::shared_ptr<::aidl::android::aidl::tests::ArrayOfInterfaces::IEmptyInterface>>* in_iface_array_inout, const std::optional<std::vector<std::shared_ptr<::aidl::android::aidl::tests::ArrayOfInterfaces::IEmptyInterface>>>& in_nullable_iface_array_in, std::optional<std::vector<std::shared_ptr<::aidl::android::aidl::tests::ArrayOfInterfaces::IEmptyInterface>>>* out_nullable_iface_array_out, std::optional<std::vector<std::shared_ptr<::aidl::android::aidl::tests::ArrayOfInterfaces::IEmptyInterface>>>* in_nullable_iface_array_inout, std::optional<std::vector<std::shared_ptr<::aidl::android::aidl::tests::ArrayOfInterfaces::IEmptyInterface>>>* _aidl_return) override;
    ::ndk::SpAIBinder asBinder() override;
    bool isRemote() override;
  };
  class BpMyInterface : public ::ndk::BpCInterface<IMyInterface> {
  public:
    explicit BpMyInterface(const ::ndk::SpAIBinder& binder);
    virtual ~BpMyInterface();

    ::ndk::ScopedAStatus methodWithInterfaces(const std::shared_ptr<::aidl::android::aidl::tests::ArrayOfInterfaces::IEmptyInterface>& in_iface, const std::shared_ptr<::aidl::android::aidl::tests::ArrayOfInterfaces::IEmptyInterface>& in_nullable_iface, const std::vector<std::shared_ptr<::aidl::android::aidl::tests::ArrayOfInterfaces::IEmptyInterface>>& in_iface_array_in, std::vector<std::shared_ptr<::aidl::android::aidl::tests::ArrayOfInterfaces::IEmptyInterface>>* out_iface_array_out, std::vector<std::shared_ptr<::aidl::android::aidl::tests::ArrayOfInterfaces::IEmptyInterface>>* in_iface_array_inout, const std::optional<std::vector<std::shared_ptr<::aidl::android::aidl::tests::ArrayOfInterfaces::IEmptyInterface>>>& in_nullable_iface_array_in, std::optional<std::vector<std::shared_ptr<::aidl::android::aidl::tests::ArrayOfInterfaces::IEmptyInterface>>>* out_nullable_iface_array_out, std::optional<std::vector<std::shared_ptr<::aidl::android::aidl::tests::ArrayOfInterfaces::IEmptyInterface>>>* in_nullable_iface_array_inout, std::optional<std::vector<std::shared_ptr<::aidl::android::aidl::tests::ArrayOfInterfaces::IEmptyInterface>>>* _aidl_return) override;
  };
  class BnMyInterface : public ::ndk::BnCInterface<IMyInterface> {
  public:
    BnMyInterface();
    virtual ~BnMyInterface();
  protected:
    ::ndk::SpAIBinder createBinder() override;
  private:
  };
  class MyParcelable {
  public:
    typedef std::false_type fixed_size;
    static const char* descriptor;

    std::shared_ptr<::aidl::android::aidl::tests::ArrayOfInterfaces::IEmptyInterface> iface;
    std::shared_ptr<::aidl::android::aidl::tests::ArrayOfInterfaces::IEmptyInterface> nullable_iface;
    std::vector<std::shared_ptr<::aidl::android::aidl::tests::ArrayOfInterfaces::IEmptyInterface>> iface_array;
    std::optional<std::vector<std::shared_ptr<::aidl::android::aidl::tests::ArrayOfInterfaces::IEmptyInterface>>> nullable_iface_array;

    binder_status_t readFromParcel(const AParcel* parcel);
    binder_status_t writeToParcel(AParcel* parcel) const;

    inline bool operator!=(const MyParcelable& rhs) const {
      return std::tie(iface, nullable_iface, iface_array, nullable_iface_array) != std::tie(rhs.iface, rhs.nullable_iface, rhs.iface_array, rhs.nullable_iface_array);
    }
    inline bool operator<(const MyParcelable& rhs) const {
      return std::tie(iface, nullable_iface, iface_array, nullable_iface_array) < std::tie(rhs.iface, rhs.nullable_iface, rhs.iface_array, rhs.nullable_iface_array);
    }
    inline bool operator<=(const MyParcelable& rhs) const {
      return std::tie(iface, nullable_iface, iface_array, nullable_iface_array) <= std::tie(rhs.iface, rhs.nullable_iface, rhs.iface_array, rhs.nullable_iface_array);
    }
    inline bool operator==(const MyParcelable& rhs) const {
      return std::tie(iface, nullable_iface, iface_array, nullable_iface_array) == std::tie(rhs.iface, rhs.nullable_iface, rhs.iface_array, rhs.nullable_iface_array);
    }
    inline bool operator>(const MyParcelable& rhs) const {
      return std::tie(iface, nullable_iface, iface_array, nullable_iface_array) > std::tie(rhs.iface, rhs.nullable_iface, rhs.iface_array, rhs.nullable_iface_array);
    }
    inline bool operator>=(const MyParcelable& rhs) const {
      return std::tie(iface, nullable_iface, iface_array, nullable_iface_array) >= std::tie(rhs.iface, rhs.nullable_iface, rhs.iface_array, rhs.nullable_iface_array);
    }

    static const ::ndk::parcelable_stability_t _aidl_stability = ::ndk::STABILITY_LOCAL;
    inline std::string toString() const {
      std::ostringstream os;
      os << "MyParcelable{";
      os << "iface: " << ::android::internal::ToString(iface);
      os << ", nullable_iface: " << ::android::internal::ToString(nullable_iface);
      os << ", iface_array: " << ::android::internal::ToString(iface_array);
      os << ", nullable_iface_array: " << ::android::internal::ToString(nullable_iface_array);
      os << "}";
      return os.str();
    }
  };
  class MyUnion {
  public:
    typedef std::false_type fixed_size;
    static const char* descriptor;

    enum Tag : int32_t {
      iface = 0,  // android.aidl.tests.ArrayOfInterfaces.IEmptyInterface iface;
      nullable_iface,  // android.aidl.tests.ArrayOfInterfaces.IEmptyInterface nullable_iface;
      iface_array,  // android.aidl.tests.ArrayOfInterfaces.IEmptyInterface[] iface_array;
      nullable_iface_array,  // android.aidl.tests.ArrayOfInterfaces.IEmptyInterface[] nullable_iface_array;
    };

    template<typename _Tp>
    static constexpr bool _not_self = !std::is_same_v<std::remove_cv_t<std::remove_reference_t<_Tp>>, MyUnion>;

    MyUnion() : _value(std::in_place_index<iface>, std::shared_ptr<::aidl::android::aidl::tests::ArrayOfInterfaces::IEmptyInterface>()) { }

    template <typename _Tp, typename = std::enable_if_t<_not_self<_Tp>>>
    // NOLINTNEXTLINE(google-explicit-constructor)
    constexpr MyUnion(_Tp&& _arg)
        : _value(std::forward<_Tp>(_arg)) {}

    template <size_t _Np, typename... _Tp>
    constexpr explicit MyUnion(std::in_place_index_t<_Np>, _Tp&&... _args)
        : _value(std::in_place_index<_Np>, std::forward<_Tp>(_args)...) {}

    template <Tag _tag, typename... _Tp>
    static MyUnion make(_Tp&&... _args) {
      return MyUnion(std::in_place_index<_tag>, std::forward<_Tp>(_args)...);
    }

    template <Tag _tag, typename _Tp, typename... _Up>
    static MyUnion make(std::initializer_list<_Tp> _il, _Up&&... _args) {
      return MyUnion(std::in_place_index<_tag>, std::move(_il), std::forward<_Up>(_args)...);
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

    inline bool operator!=(const MyUnion& rhs) const {
      return _value != rhs._value;
    }
    inline bool operator<(const MyUnion& rhs) const {
      return _value < rhs._value;
    }
    inline bool operator<=(const MyUnion& rhs) const {
      return _value <= rhs._value;
    }
    inline bool operator==(const MyUnion& rhs) const {
      return _value == rhs._value;
    }
    inline bool operator>(const MyUnion& rhs) const {
      return _value > rhs._value;
    }
    inline bool operator>=(const MyUnion& rhs) const {
      return _value >= rhs._value;
    }

    static const ::ndk::parcelable_stability_t _aidl_stability = ::ndk::STABILITY_LOCAL;
    inline std::string toString() const {
      std::ostringstream os;
      os << "MyUnion{";
      switch (getTag()) {
      case iface: os << "iface: " << ::android::internal::ToString(get<iface>()); break;
      case nullable_iface: os << "nullable_iface: " << ::android::internal::ToString(get<nullable_iface>()); break;
      case iface_array: os << "iface_array: " << ::android::internal::ToString(get<iface_array>()); break;
      case nullable_iface_array: os << "nullable_iface_array: " << ::android::internal::ToString(get<nullable_iface_array>()); break;
      }
      os << "}";
      return os.str();
    }
  private:
    std::variant<std::shared_ptr<::aidl::android::aidl::tests::ArrayOfInterfaces::IEmptyInterface>, std::shared_ptr<::aidl::android::aidl::tests::ArrayOfInterfaces::IEmptyInterface>, std::vector<std::shared_ptr<::aidl::android::aidl::tests::ArrayOfInterfaces::IEmptyInterface>>, std::optional<std::vector<std::shared_ptr<::aidl::android::aidl::tests::ArrayOfInterfaces::IEmptyInterface>>>> _value;
  };

  binder_status_t readFromParcel(const AParcel* parcel);
  binder_status_t writeToParcel(AParcel* parcel) const;

  inline bool operator!=(const ArrayOfInterfaces&) const {
    return std::tie() != std::tie();
  }
  inline bool operator<(const ArrayOfInterfaces&) const {
    return std::tie() < std::tie();
  }
  inline bool operator<=(const ArrayOfInterfaces&) const {
    return std::tie() <= std::tie();
  }
  inline bool operator==(const ArrayOfInterfaces&) const {
    return std::tie() == std::tie();
  }
  inline bool operator>(const ArrayOfInterfaces&) const {
    return std::tie() > std::tie();
  }
  inline bool operator>=(const ArrayOfInterfaces&) const {
    return std::tie() >= std::tie();
  }

  static const ::ndk::parcelable_stability_t _aidl_stability = ::ndk::STABILITY_LOCAL;
  inline std::string toString() const {
    std::ostringstream os;
    os << "ArrayOfInterfaces{";
    os << "}";
    return os.str();
  }
};
}  // namespace tests
}  // namespace aidl
}  // namespace android
}  // namespace aidl
