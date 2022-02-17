#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <android/binder_interface_utils.h>
#ifdef BINDER_STABILITY_SUPPORT
#include <android/binder_stability.h>
#endif  // BINDER_STABILITY_SUPPORT

namespace aidl {
namespace android {
namespace aidl {
namespace tests {
namespace permission {
class IProtectedInterface : public ::ndk::ICInterface {
public:
  static const char* descriptor;
  IProtectedInterface();
  virtual ~IProtectedInterface();

  static constexpr uint32_t TRANSACTION_Method1 = FIRST_CALL_TRANSACTION + 0;
  static constexpr uint32_t TRANSACTION_Method2 = FIRST_CALL_TRANSACTION + 1;

  static std::shared_ptr<IProtectedInterface> fromBinder(const ::ndk::SpAIBinder& binder);
  static binder_status_t writeToParcel(AParcel* parcel, const std::shared_ptr<IProtectedInterface>& instance);
  static binder_status_t readFromParcel(const AParcel* parcel, std::shared_ptr<IProtectedInterface>* instance);
  static bool setDefaultImpl(const std::shared_ptr<IProtectedInterface>& impl);
  static const std::shared_ptr<IProtectedInterface>& getDefaultImpl();
  virtual ::ndk::ScopedAStatus Method1() = 0;
  virtual ::ndk::ScopedAStatus Method2() = 0;
private:
  static std::shared_ptr<IProtectedInterface> default_impl;
};
class IProtectedInterfaceDefault : public IProtectedInterface {
public:
  ::ndk::ScopedAStatus Method1() override;
  ::ndk::ScopedAStatus Method2() override;
  ::ndk::SpAIBinder asBinder() override;
  bool isRemote() override;
};
}  // namespace permission
}  // namespace tests
}  // namespace aidl
}  // namespace android
}  // namespace aidl
