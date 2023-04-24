#include <binder/IBinder.h>
#include <binder/IPCThreadState.h>
#include <binder/IServiceManager.h>
#include <binder/LazyServiceRegistrar.h>
#include <utils/Log.h>
#include "LazyTestService.h"

using android::BBinder;
using android::IBinder;
using android::IPCThreadState;
using android::OK;
using android::sp;
using android::binder::LazyServiceRegistrar;
using android::binder::LazyTestService;

int main() {
  sp<LazyTestService> service1 = new LazyTestService();
  sp<LazyTestService> service2 = new LazyTestService();

  // Simulate another callback here, to test to make sure the actual instance
  // we are relying on doesn't have its state messed up when multiple client
  // callbacks are registered.
  // DO NOT COPY - INTENTIONALLY TESTING BAD BEHAVIOR
  auto extra = LazyServiceRegistrar::createExtraTestInstance();
  extra.forcePersist(true);  // don't allow this instance to handle process lifetime
  LOG_ALWAYS_FATAL_IF(OK != extra.registerService(service1, "aidl_lazy_test_1"), "");
  // DO NOT COPY - INTENTIONALLY TESTING BAD BEHAVIOR

  auto lazyRegistrar = LazyServiceRegistrar::getInstance();
  LOG_ALWAYS_FATAL_IF(OK != lazyRegistrar.registerService(service1, "aidl_lazy_test_1"), "");
  LOG_ALWAYS_FATAL_IF(OK != lazyRegistrar.registerService(service2, "aidl_lazy_test_2"), "");

  IPCThreadState::self()->joinThreadPool();

  return 1;
}
