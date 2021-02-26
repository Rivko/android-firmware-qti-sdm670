/******************************************************************************
  @file  ThermalCallbackHandler.h
  @brief vendor.qti.hardware.limits@1.0 IThermal HAL clients
         callback and death handler.

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
******************************************************************************/

#include <set>
#include <hidl/HidlSupport.h>

namespace vendor {
namespace qti {
namespace hardware {
namespace limits {
namespace V1_0 {
namespace implementation {

using ::android::sp;
using on_death_cb = std::function<void(uint64_t)>;

template <typename CallbackType>
// Death handler class for callback registered with linkToDeath.
class ThermalDeathHandler : public android::hardware::hidl_death_recipient {
public:
    ThermalDeathHandler(const on_death_cb& module_cb_func)
        : cb_func(module_cb_func) {}
    ~ThermalDeathHandler() = default;

    void serviceDied(
        uint64_t cookie,
        const android::wp<android::hidl::base::V1_0::IBase>& /* who */) override
    {
        cb_func(cookie);
    }
private:
      on_death_cb cb_func;
};

template <typename CallbackType>
// Class to manage callbacks for the various clients and
// handle the death of the client hosting callback.
class ThermalCallbackHandler {
public:
    ThermalCallbackHandler()
        : death_handler(new ThermalDeathHandler<CallbackType>(
            std::bind(&ThermalCallbackHandler::clientDeathCb,
                  this, std::placeholders::_1))) {}
    ~ThermalCallbackHandler()
    {
        removeAllCallback();
    }

    void lock()
    {
        pthread_mutex_lock(&tMutex);
    }

    void unlock()
    {
        pthread_mutex_unlock(&tMutex);
    }
    const std::set<android::sp<CallbackType>>& getCallbacks()
    {
        return cb_set;
    }

    bool addCallback(const sp<CallbackType>& cb)
    {
        lock();
       // use the cookie to track the death of individual clients.
        uint64_t cookie = reinterpret_cast<uint64_t>(cb.get());

        if (cb_set.find(cb) != cb_set.end()) {
            ALOGE("Duplicate death notification registration\n");
            unlock();
            return true;
        }
        if (!cb->linkToDeath(death_handler, cookie)) {
            ALOGE("Failed to register death notification\n");
            unlock();
            return false;
        }
        cb_set.insert(cb);
        unlock();
        ALOGD("ThermalCallbackHandler:Registered Death notification\n");

        return true;
    }

    void removeCallback(const sp<CallbackType>& cb)
    {
        lock();
        cb->unlinkToDeath(death_handler);
        const auto& iter = cb_set.find(cb);

        if (iter == cb_set.end()) {
            ALOGE("ThermalCallbackHandler:Could not find callback from list\n");
            unlock();
            return;
        }
        cb_set.erase(iter);
        unlock();
        ALOGD("ThermalCallbackHandler:callback removed from list\n");
    }

    void clientDeathCb(uint64_t cookie)
    {
        CallbackType* cb = reinterpret_cast<CallbackType*>(cookie);

        lock();
        const auto& iter = cb_set.find(cb);
        if (iter == cb_set.end()) {
            ALOGE("ThermalCallbackHandler:Unknown client death notification\n");
            unlock();
            return;
        }
        cb_set.erase(iter);
        unlock();
        ALOGD("ThermalCallbackHandler:Dead callback removed from list\n");
    }

    void removeAllCallback()
    {
        lock();
        for (const sp<CallbackType>& cb : cb_set) {
            if (!cb->unlinkToDeath(death_handler)) {
                ALOGE("ThermalCallbackHandler:Failed to unlinkToDeath\n");
            }
        }
        cb_set.clear();
        unlock();
    }

private:
    pthread_mutex_t tMutex;
    std::set<sp<CallbackType>> cb_set;
    sp<ThermalDeathHandler<CallbackType>> death_handler;
};

}  // namespace implementation
}  // namespace V1_0
}  // namespace limits
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
