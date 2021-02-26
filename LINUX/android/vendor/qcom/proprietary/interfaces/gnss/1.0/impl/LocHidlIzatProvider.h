/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef VENDOR_QTI_GNSS_V1_0_LOCHIDLIZATPROVIDERENGINE_H
#define VENDOR_QTI_GNSS_V1_0_LOCHIDLIZATPROVIDERENGINE_H

#include <vendor/qti/gnss/1.0/ILocHidlIzatProvider.h>
#include <vendor/qti/gnss/1.0/ILocHidlIzatProviderCallback.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>
#include <IzatTypes.h>
#include <IIzatManager.h>
#include <IOSListener.h>
#include <IzatRequest.h>

namespace vendor {
namespace qti {
namespace gnss {
namespace V1_0 {
namespace implementation {

using ::android::hidl::base::V1_0::DebugInfo;
using ::android::hidl::base::V1_0::IBase;
using ::vendor::qti::gnss::V1_0::LocHidlIzatRequest;
using ::vendor::qti::gnss::V1_0::LocHidlIzatStreamType;
using ::vendor::qti::gnss::V1_0::ILocHidlIzatProvider;
using ::vendor::qti::gnss::V1_0::ILocHidlIzatProviderCallback;
using ::android::hardware::hidl_array;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::hardware::hidl_death_recipient;
using ::android::sp;
using ::android::wp;

using namespace izat_manager;

class Listener;
struct LocHidlDeathRecipient;
struct LocHidlIzatProvider : public ILocHidlIzatProvider {
    LocHidlIzatProvider(LocHidlIzatStreamType privderType);
    inline ~LocHidlIzatProvider() { mDeathRecipient = nullptr; }

    // Methods from ::vendor::qti::gnss::V1_0::ILocHidlIzatProvider follow.
    Return<bool> init(const sp<ILocHidlIzatProviderCallback>& callback) override;
    Return<void> deinit() override;
    Return<bool> onEnable() override;
    Return<bool> onDisable() override;
    Return<bool> onAddRequest(const LocHidlIzatRequest& request) override;
    Return<bool> onRemoveRequest(const LocHidlIzatRequest& request) override;

private:
    void createIzatRequest(
            IzatRequest& izatRequest,
            const LocHidlIzatRequest& gnssIzatRequest);

    sp<LocHidlDeathRecipient> mDeathRecipient = nullptr;
    Listener* mListener;
    IIzatManager* mIzatManager;
    const IzatStreamType mProviderType;
};

}  // namespace implementation
}  // namespace V1_0
}  // namespace gnss
}  // namespace qti
}  // namespace vendor

#endif  // VENDOR_QTI_GNSS_V1_0_LOCHIDLIZATPROVIDERENGINE_H
