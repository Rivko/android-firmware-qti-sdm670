/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef VENDOR_QTI_GNSS_V1_1_LOCHIDLSUBSCRIPTION_H
#define VENDOR_QTI_GNSS_V1_1_LOCHIDLSUBSCRIPTION_H

#include <vendor/qti/gnss/1.1/ILocHidlIzatSubscription.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>
#include <Subscription.h>
#include <vendor/qti/gnss/1.1/types.h>

namespace vendor {
namespace qti {
namespace gnss {
namespace V1_1 {
namespace implementation {

using ::android::hidl::base::V1_0::DebugInfo;
using ::android::hidl::base::V1_0::IBase;
using ::vendor::qti::gnss::V1_1::ILocHidlIzatSubscription;
using ::vendor::qti::gnss::V1_0::ILocHidlIzatSubscriptionCallback;
using ::android::hardware::hidl_array;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::hardware::hidl_death_recipient;
using ::android::sp;
using ::android::wp;
using ::vendor::qti::gnss::V1_1::LocHidlSubscriptionDataItemId_1_1;

struct LocHidlDeathRecipient;
struct LocHidlIzatSubscription : public ILocHidlIzatSubscription {
    LocHidlIzatSubscription();
    ~LocHidlIzatSubscription();
    // Methods from ::vendor::qti::gnss::V1_1::ILocHidlIzatSubscription follow.
    Return<bool> init(const sp<ILocHidlIzatSubscriptionCallback>& callback) override;
    Return<void> deinit() override;
    Return<void> boolDataItemUpdate(const hidl_vec<ILocHidlIzatSubscription::BoolDataItem>& dataItemArray) override;
    Return<void> stringDataItemUpdate(const ILocHidlIzatSubscription::StringDataItem& dataItem) override;
    Return<void> networkinfoUpdate(const ILocHidlIzatSubscription::NetworkInfoDataItem& dataItem) override;
    Return<void> serviceinfoUpdate(const ILocHidlIzatSubscription::RilServiceInfoDataItem& dataItem) override;
    Return<void> cellLteUpdate(const ILocHidlIzatSubscription::CellLteDataItem& dataItem) override;
    Return<void> cellGwUpdate(const ILocHidlIzatSubscription::CellGwDataItem& dataItem) override;
    Return<void> cellCdmaUpdate(const ILocHidlIzatSubscription::CellCdmaDataItem& dataItem) override;
    Return<void> cellOooUpdate(const ILocHidlIzatSubscription::CellOooDataItem& dataItem) override;
    Return<void> serviceStateUpdate(const ILocHidlIzatSubscription::ServiceStateDataItem& dataItem) override;
    Return<void> screenStatusUpdate(const ILocHidlIzatSubscription::ScreenStatusDataItem& dataItem) override;
    Return<void> powerConnectStatusUpdate(const ILocHidlIzatSubscription::PowerConnectStatusDataItem& dataItem) override;
    Return<void> timezoneChangeUpdate(const ILocHidlIzatSubscription::TimeZoneChangeDataItem& dataItem) override;
    Return<void> timeChangeUpdate(const ILocHidlIzatSubscription::TimeChangeDataItem& dataItem) override;
    Return<void> shutdownUpdate() override;
    Return<void> wifiSupplicantStatusUpdate(const ILocHidlIzatSubscription::WifiSupplicantStatusDataItem& dataItem) override;
    Return<void> btleScanDataInject(const ILocHidlIzatSubscription::BtLeDeviceScanDetailsDataItem& dataItem) override;
    Return<void> btClassicScanDataInject(const ILocHidlIzatSubscription::BtDeviceScanDetailsDataItem& dataItem) override;
    Return<void> batteryLevelUpdate(const ILocHidlIzatSubscription::BatteryLevelDataItem& dataItem) override;

private:
    struct IzatSubscriptionCallback: SubscriptionCallbackIface {

        IzatSubscriptionCallback(
                sp<ILocHidlIzatSubscriptionCallback> hidlCbIface) :
                    mHidlCbIface(hidlCbIface) {};
        ~IzatSubscriptionCallback() = default;
        sp<ILocHidlIzatSubscriptionCallback> mHidlCbIface;

        virtual void updateSubscribe(
                const std::list<DataItemId> & l, bool subscribe) override;
        virtual void requestData(const std::list<DataItemId> & l) override;
        virtual void unsubscribeAll() override;
        virtual void turnOnModule(DataItemId dit,int timeOut) override;
        virtual void turnOffModule(DataItemId dit) override;
};

private:
    sp<ILocHidlIzatSubscriptionCallback> mCallbackIface = nullptr;
    sp<LocHidlDeathRecipient> mDeathRecipient = nullptr;
    IzatSubscriptionCallback* mSubscriptionCb;
};

}  // namespace implementation
}  // namespace V1_1
}  // namespace gnss
}  // namespace qti
}  // namespace vendor

#endif  // VENDOR_QTI_GNSS_V1_1_LOCHIDLSUBSCRIPTION_H
