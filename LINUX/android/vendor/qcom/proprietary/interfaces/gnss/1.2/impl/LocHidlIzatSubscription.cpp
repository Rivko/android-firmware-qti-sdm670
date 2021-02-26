/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#define LOG_TAG "LocSvc_HIDL_IzatSubscription"
#define LOG_NDEBUG 0
#include "LocHidlIzatSubscription.h"
#include <log_util.h>
#include "Subscription.h"
#include "DataItemConcreteTypes.h"
#include "FrameworkActionRequest.h"
#include "loc_ril.h"
#include "WiperData.h"
#include "LocHidlUtils.h"
#include "IDataItemCore.h"

namespace vendor {
namespace qti {
namespace gnss {
namespace V1_2 {
namespace implementation {

static bool sSubscribe[MAX_DATA_ITEM_ID_1_1];

LocHidlIzatSubscription::LocHidlIzatSubscription() :
        mCallbackIface(nullptr), mSubscriptionCb(NULL) {

    ENTRY_LOG();
    for (int i = 0; i < MAX_DATA_ITEM_ID_1_1; i++) {
        sSubscribe[i] = false;
    }
}

LocHidlIzatSubscription::~LocHidlIzatSubscription() {
    if (mSubscriptionCb != nullptr) {
        delete mSubscriptionCb;
        mSubscriptionCb = nullptr;
    }
}

void LocHidlIzatSubscription::IzatSubscriptionCallback::updateSubscribe(
        const std::list<DataItemId>& dataItemIdList, bool subscribe) {

    ENTRY_LOG();

    for (auto item : dataItemIdList) {
        if (item >= 0 && item < MAX_DATA_ITEM_ID_1_1) {
            sSubscribe[item] = subscribe;
        }
    }

    if (mHidlCbIface == nullptr) {
        LOC_LOGE("mHidlCbIface null !");
        return;
    }

    // Create a vector with all data items
    hidl_vec<LocHidlSubscriptionDataItemId> dataItemVec;
    dataItemVec.resize(dataItemIdList.size());
    int idx = 0;
    for (auto id : dataItemIdList) {
        LocHidlSubscriptionDataItemId_1_1 sid =
                LocHidlUtils::translateToSubscriptionDataItemId_1_1(id);
        dataItemVec[idx] = (LocHidlSubscriptionDataItemId)sid;
        idx++;
    }

    TO_HIDL_CLIENT();
    auto r = mHidlCbIface->updateSubscribe(dataItemVec, subscribe);
    if (!r.isOk()) {
        LOC_LOGE("Error invoking HIDL CB [%s]", r.description().c_str());
    }
}

void LocHidlIzatSubscription::IzatSubscriptionCallback::requestData(
        const std::list<DataItemId>& l) {

    ENTRY_LOG();

    if (mHidlCbIface == nullptr) {
        LOC_LOGE("mHidlCbIface null !");
        return;
    }

    // Create a vector with all data items
    hidl_vec<LocHidlSubscriptionDataItemId> dataItemVec;
    dataItemVec.resize(l.size());
    int idx = 0;
    for (std::list<DataItemId>::const_iterator it = l.begin();
            it != l.end(); it++, idx++) {

        DataItemId id = *it;
        LocHidlSubscriptionDataItemId_1_1 sid =
                LocHidlUtils::translateToSubscriptionDataItemId_1_1(id);
        dataItemVec[idx] = (LocHidlSubscriptionDataItemId)sid;
    }

    TO_HIDL_CLIENT();
    auto r = mHidlCbIface->requestData(dataItemVec);
    if (!r.isOk()) {
        LOC_LOGE("Error invoking HIDL CB [%s]", r.description().c_str());
    }
}

void LocHidlIzatSubscription::IzatSubscriptionCallback::unsubscribeAll() {

    ENTRY_LOG();

    if (mHidlCbIface == nullptr) {
        LOC_LOGE("mHidlCbIface null !");
        return;
    }

    TO_HIDL_CLIENT();
    auto r = mHidlCbIface->unsubscribeAll();
    if (!r.isOk()) {
        LOC_LOGE("Error invoking HIDL CB [%s]", r.description().c_str());
    }
}

void LocHidlIzatSubscription::IzatSubscriptionCallback::turnOnModule(
        DataItemId dit, int timeOut) {

    ENTRY_LOG();

    if (mHidlCbIface == nullptr) {
        LOC_LOGE("mHidlCbIface null !");
        return;
    }

    LocHidlSubscriptionDataItemId_1_1 sdi =
            LocHidlUtils::translateToSubscriptionDataItemId_1_1(dit);

    TO_HIDL_CLIENT();
    auto r = mHidlCbIface->turnOnModule((LocHidlSubscriptionDataItemId)sdi, timeOut);
    if (!r.isOk()) {
        LOC_LOGE("Error invoking HIDL CB [%s]", r.description().c_str());
    }
}

void LocHidlIzatSubscription::IzatSubscriptionCallback::turnOffModule(
        DataItemId dit) {

    ENTRY_LOG();

    if (mHidlCbIface == nullptr) {
        LOC_LOGE("mHidlCbIface null !");
        return;
    }

    LocHidlSubscriptionDataItemId_1_1 sdi =
            LocHidlUtils::translateToSubscriptionDataItemId_1_1(dit);

    TO_HIDL_CLIENT();
    auto r = mHidlCbIface->turnOffModule((LocHidlSubscriptionDataItemId)sdi);
    if (!r.isOk()) {
        LOC_LOGE("Error invoking HIDL CB [%s]", r.description().c_str());
    }
}
// Methods from ::vendor::qti::gnss::V1_0::ILocHidlIzatSubscription follow.
Return<bool> LocHidlIzatSubscription::init(const sp<ILocHidlIzatSubscriptionCallback>& callback) {
    static uint64_t deathCount = 0;
    FROM_HIDL_CLIENT();

    // Register death recipient
    if (mDeathRecipient == nullptr) {
        mDeathRecipient = new LocHidlDeathRecipient([this] {
                LOC_LOGE("%s]: ILocHidlIzatSubscriptionCallback died.", __func__);
                deinit();
                });
    }
    mDeathRecipient->registerToPeer(callback, deathCount++);
    mCallbackIface = callback;

    mSubscriptionCb =
            new LocHidlIzatSubscription::IzatSubscriptionCallback(callback);
    Subscription::setSubscriptionCallback(mSubscriptionCb);

    // create the subscription object.
    (void)Subscription::getSubscriptionObj();

    // create the framework action request object
    (void)FrameworkActionRequest::getFrameworkActionReqObj();

    std::list<DataItemId> l;
    for (int i = INVALID_DATA_ITEM_ID + 1; i < MAX_DATA_ITEM_ID_1_1; i++) {
        if (sSubscribe[i]) {
            l.push_back((DataItemId(i)));
        }
    }
    if (!l.empty()) {
        LOC_LOGV("resubscribe");
        mSubscriptionCb->updateSubscribe(l, true);
    }

    return true;
}

Return<void> LocHidlIzatSubscription::deinit() {
    FROM_HIDL_CLIENT();

    // reset SubscriptionCallback
    Subscription::setSubscriptionCallback(nullptr);
    if (mSubscriptionCb != nullptr) {
        delete mSubscriptionCb;
        mSubscriptionCb = nullptr;
    }

    return Void();
}

Return<void> LocHidlIzatSubscription::boolDataItemUpdate(
        const hidl_vec<ILocHidlIzatSubscription::BoolDataItem>& dataItemArray) {

    FROM_HIDL_CLIENT();

    if (Subscription::mObserverObj == NULL) {
        LOC_LOGE("null observer !");
    return Void();
}

    ENHDataItem enh_data_item;
    AirplaneModeDataItem airplaneMode_data_item;
    GPSStateDataItem gpsState_data_item;
    NLPStatusDataItem nlpStatus_data_item;
    WifiHardwareStateDataItem wifistate_data_item;
    AssistedGpsDataItem assistedgps_data_item;

    std::list<IDataItemCore *> dataItem_list;

    for (int idx = 0; idx < (int)dataItemArray.size(); idx++) {
        switch (dataItemArray[idx].id)
        {
            case ENH_DATA_ITEM_ID:
                enh_data_item.mEnabled = dataItemArray[idx].enabled;
                dataItem_list.push_back(&enh_data_item);
            break;
            case AIRPLANEMODE_DATA_ITEM_ID:
                airplaneMode_data_item.mMode = dataItemArray[idx].enabled;
                dataItem_list.push_back(&airplaneMode_data_item);
            break;
            case GPSSTATE_DATA_ITEM_ID:
                gpsState_data_item.mEnabled = dataItemArray[idx].enabled;
                dataItem_list.push_back(&gpsState_data_item);
            break;
            case NLPSTATUS_DATA_ITEM_ID:
                nlpStatus_data_item.mEnabled = dataItemArray[idx].enabled;
                dataItem_list.push_back(&nlpStatus_data_item);
            break;
            case WIFIHARDWARESTATE_DATA_ITEM_ID:
                wifistate_data_item.mEnabled = dataItemArray[idx].enabled;
                dataItem_list.push_back(&wifistate_data_item);
            break;
            case ASSISTED_GPS_DATA_ITEM_ID:
                assistedgps_data_item.mEnabled = dataItemArray[idx].enabled;
                dataItem_list.push_back(&assistedgps_data_item);
            break;
        }
    }

    if (dataItem_list.size() != 0) {
        Subscription::mObserverObj->notify(dataItem_list);
    }
    return Void();
}

Return<void> LocHidlIzatSubscription::stringDataItemUpdate(
        const ILocHidlIzatSubscription::StringDataItem& di) {

    FROM_HIDL_CLIENT();

    if (Subscription::mObserverObj == NULL) {
        LOC_LOGE("null observer !");
    return Void();
}

    IDataItemCore *dataItem = NULL;

    ModelDataItem modeldesc_data_item;
    ManufacturerDataItem manufacturerdesc_data_item;
    TacDataItem tacdesc_data_item;
    MccmncDataItem mccmncdesc_data_item;

    switch (di.id)
    {
        case MODEL_DATA_ITEM_ID:
            modeldesc_data_item.mModel.assign(di.str.c_str());
            dataItem = &modeldesc_data_item;
        break;
        case MANUFACTURER_DATA_ITEM_ID:
            manufacturerdesc_data_item.mManufacturer.assign(di.str.c_str());
            dataItem = &manufacturerdesc_data_item;
        break;
        case TAC_DATA_ITEM_ID:
            tacdesc_data_item.mValue.assign(di.str.c_str());
            dataItem = &tacdesc_data_item;
        break;
        case MCCMNC_DATA_ITEM_ID:
            mccmncdesc_data_item.mValue.assign(di.str.c_str());
            dataItem = &mccmncdesc_data_item;
        break;
    }

    if (dataItem != NULL) {
         std::list<IDataItemCore *> dataItem_list;
         dataItem_list.push_back(dataItem);
         Subscription::mObserverObj->notify(dataItem_list);
    }
    return Void();
}

Return<void> LocHidlIzatSubscription::networkinfoUpdate(
        const ILocHidlIzatSubscription::NetworkInfoDataItem& di) {

    FROM_HIDL_CLIENT();

    if (Subscription::mObserverObj == NULL) {
        LOC_LOGE("null observer !");
    return Void();
}

    izat_manager::NetworkInfoDataItem networkinfo_data_item;
    IDataItemCore *dataItem = NULL;

    networkinfo_data_item.mType = di.type;
    networkinfo_data_item.mAvailable = di.available;
    networkinfo_data_item.mConnected = di.connected;
    networkinfo_data_item.mRoaming = di.roaming;

    if (di.typeName.size() > 0) {
        networkinfo_data_item.mTypeName.assign(di.typeName.c_str());
    }

    if (di.subTypeName.size() > 0) {
        networkinfo_data_item.mSubTypeName.assign(di.subTypeName.c_str());
    }

    dataItem = &networkinfo_data_item;

    std::list<IDataItemCore *> dataItem_list;
    dataItem_list.push_back(dataItem);
    Subscription::mObserverObj->notify(dataItem_list);
    return Void();
}

Return<void> LocHidlIzatSubscription::serviceinfoUpdate(
        const ILocHidlIzatSubscription::RilServiceInfoDataItem& di) {

    FROM_HIDL_CLIENT();

    if (Subscription::mObserverObj == NULL) {
        LOC_LOGE("null observer !");
    return Void();
}

    LOC_RilServiceInfo serviceInfo;
    memset(&serviceInfo, 0, sizeof(LOC_RilServiceInfo));

    switch (di.type)
    {
        case LOC_RILAIRIF_CDMA:
            serviceInfo.valid_mask = LOC_RIL_SERVICE_INFO_HAS_CARRIER_AIR_IF_TYPE;
            serviceInfo.carrierAirIf_type = LOC_RILAIRIF_CDMA;

            if (di.name.size() > 0) {
                serviceInfo.valid_mask |= LOC_RIL_SERVICE_INFO_HAS_CARRIER_NAME;

                if (strlcpy(serviceInfo.carrierName, di.name.c_str(),
                        sizeof(serviceInfo.carrierName))
                        >= sizeof(serviceInfo.carrierName)) {
                    LOC_LOGD("Carrier name is longer than array size to hold it.");
                }
            }
            break;

        case LOC_RILAIRIF_GSM:
            serviceInfo.valid_mask = LOC_RIL_SERVICE_INFO_HAS_CARRIER_AIR_IF_TYPE;
            serviceInfo.carrierAirIf_type = LOC_RILAIRIF_GSM;

            // MCC cannot be zero
            if (di.mcc != 0) {
                serviceInfo.valid_mask |= LOC_RIL_SERVICE_INFO_HAS_CARRIER_MCC;
                serviceInfo.carrierMcc = di.mcc;
            }

            // Mnc can be 0
            serviceInfo.valid_mask |= LOC_RIL_SERVICE_INFO_HAS_CARRIER_MNC;
            serviceInfo.carrierMnc = di.mnc;
        break;
    }

    izat_manager::RilServiceInfoDataItem serviceinfo_data_item(&serviceInfo);
    std::list<IDataItemCore *> dataItem_list;
    dataItem_list.push_back(&serviceinfo_data_item);
    Subscription::mObserverObj->notify(dataItem_list);
    return Void();
}

Return<void> LocHidlIzatSubscription::cellLteUpdate(
        const ILocHidlIzatSubscription::CellLteDataItem& di) {

    FROM_HIDL_CLIENT();

    if (Subscription::mObserverObj == NULL) {
        LOC_LOGE("null observer !");
    return Void();
}

    LOC_RilCellInfo cellInfo;
    memset(&cellInfo, 0, sizeof(LOC_RilCellInfo));

    cellInfo.valid_mask = di.mask;
    cellInfo.nwStatus = (LOC_NWstatus)di.status;
    cellInfo.rtType = (LOC_RilTechType)di.type;

    cellInfo.u.lteCinfo.mcc = di.mcc;
    cellInfo.u.lteCinfo.mnc = di.mnc;
    cellInfo.u.lteCinfo.pci = di.pci;
    cellInfo.u.lteCinfo.tac = di.tac;
    cellInfo.u.lteCinfo.cid = di.cid;

    cellInfo.valid_mask |= LOC_RIL_CELLINFO_HAS_CELL_INFO;

    RilCellInfoDataItem cellinfo_data_item(&cellInfo);
    std::list<IDataItemCore *> dataItem_list;
    dataItem_list.push_back(&cellinfo_data_item);
    Subscription::mObserverObj->notify(dataItem_list);
    return Void();
}

Return<void> LocHidlIzatSubscription::cellGwUpdate(
        const ILocHidlIzatSubscription::CellGwDataItem& di) {

    FROM_HIDL_CLIENT();

    if (Subscription::mObserverObj == NULL) {
        LOC_LOGE("null observer !");
    return Void();
}

    LOC_RilCellInfo cellInfo;
    memset(&cellInfo, 0, sizeof(LOC_RilCellInfo));

    cellInfo.valid_mask = di.mask;
    cellInfo.nwStatus = (LOC_NWstatus)di.status;
    cellInfo.rtType = (LOC_RilTechType)di.type;

    switch (di.type)
    {
        case LOC_RILAIRIF_GSM:
            cellInfo.u.gsmCinfo.mcc = di.mcc;
            cellInfo.u.gsmCinfo.mnc = di.mnc;
            cellInfo.u.gsmCinfo.lac = di.lac;
            cellInfo.u.gsmCinfo.cid = di.cid;
            break;
        case LOC_RILAIRIF_WCDMA:
            cellInfo.u.wcdmaCinfo.mcc = di.mcc;
            cellInfo.u.wcdmaCinfo.mnc = di.mnc;
            cellInfo.u.wcdmaCinfo.lac = di.lac;
            cellInfo.u.wcdmaCinfo.cid = di.cid;
            break;
        default:
            LOC_LOGE("Invalid interface type %d", di.type);
    return Void();
}

    cellInfo.valid_mask |= LOC_RIL_CELLINFO_HAS_CELL_INFO;

    RilCellInfoDataItem cellinfo_data_item(&cellInfo);
    std::list<IDataItemCore *> dataItem_list;
    dataItem_list.push_back(&cellinfo_data_item);

    Subscription::mObserverObj->notify(dataItem_list);

    return Void();
}

Return<void> LocHidlIzatSubscription::cellCdmaUpdate(
        const ILocHidlIzatSubscription::CellCdmaDataItem& di) {

    FROM_HIDL_CLIENT();

    if (Subscription::mObserverObj == NULL) {
        LOC_LOGE("null observer !");
        return Void();
    }

    LOC_RilCellInfo cellInfo;
    memset(&cellInfo, 0, sizeof(LOC_RilCellInfo));

    cellInfo.valid_mask = di.mask;
    cellInfo.nwStatus = (LOC_NWstatus)di.status;
    cellInfo.rtType = (LOC_RilTechType)di.type;

    cellInfo.u.cdmaCinfo.sid = di.sid;
    cellInfo.u.cdmaCinfo.nid = di.nid;
    cellInfo.u.cdmaCinfo.bsid = di.bsid;
    cellInfo.u.cdmaCinfo.bslat = di.bslat;
    cellInfo.u.cdmaCinfo.bslon = di.bslong;
    cellInfo.u.cdmaCinfo.local_timezone_offset_from_utc = di.timeOffset;
    cellInfo.u.cdmaCinfo.local_timezone_on_daylight_savings = di.inDST;

    cellInfo.valid_mask |= LOC_RIL_CELLINFO_HAS_CELL_INFO;

    RilCellInfoDataItem cellinfo_data_item(&cellInfo);
    std::list<IDataItemCore *> dataItem_list;
    dataItem_list.push_back(&cellinfo_data_item);
    Subscription::mObserverObj->notify(dataItem_list);

    return Void();
}

Return<void> LocHidlIzatSubscription::cellOooUpdate(
        const ILocHidlIzatSubscription::CellOooDataItem& di) {

    FROM_HIDL_CLIENT();

    if (Subscription::mObserverObj == NULL) {
        LOC_LOGE("null observer !");
        return Void();
    }

    LOC_RilCellInfo cellInfo;
    memset(&cellInfo, 0, sizeof(LOC_RilCellInfo));

    cellInfo.valid_mask = di.mask;
    cellInfo.nwStatus = (LOC_NWstatus)di.status;

    RilCellInfoDataItem cellinfo_data_item(&cellInfo);

    std::list<IDataItemCore *> dataItem_list;
    dataItem_list.push_back(&cellinfo_data_item);

    Subscription::mObserverObj->notify(dataItem_list);

    return Void();
}

Return<void> LocHidlIzatSubscription::serviceStateUpdate(
        const ILocHidlIzatSubscription::ServiceStateDataItem& di) {

    FROM_HIDL_CLIENT();

    if (Subscription::mObserverObj == NULL) {
        LOC_LOGE("null observer !");
        return Void();
    }

    ServiceStatusDataItem service_status_data_item;
    service_status_data_item.mServiceState = (LOC_NWstatus)di.status;

    IDataItemCore *dataItem = &service_status_data_item;

    std::list<IDataItemCore *> dataItem_list;
    dataItem_list.push_back(dataItem);

    Subscription::mObserverObj->notify(dataItem_list);

    return Void();
}

Return<void> LocHidlIzatSubscription::screenStatusUpdate(
        const ILocHidlIzatSubscription::ScreenStatusDataItem& di) {

    FROM_HIDL_CLIENT();

    if (Subscription::mObserverObj == NULL) {
        LOC_LOGE("null observer !");
        return Void();
    }

    ScreenStateDataItem screenstate_data_item;
    screenstate_data_item.mState = di.status;

    IDataItemCore *dataItem = &screenstate_data_item;

    std::list<IDataItemCore *> dataItem_list;
    dataItem_list.push_back(dataItem);

    Subscription::mObserverObj->notify(dataItem_list);

    return Void();
}

Return<void> LocHidlIzatSubscription::powerConnectStatusUpdate(
        const ILocHidlIzatSubscription::PowerConnectStatusDataItem& di) {

    FROM_HIDL_CLIENT();

    if (Subscription::mObserverObj == NULL) {
        LOC_LOGE("null observer !");
        return Void();
    }

    PowerConnectStateDataItem powerconnectstate_data_item;
    powerconnectstate_data_item.mState = di.status;

    IDataItemCore *dataItem = &powerconnectstate_data_item;

    std::list<IDataItemCore *> dataItem_list;
    dataItem_list.push_back(dataItem);

    Subscription::mObserverObj->notify(dataItem_list);

    return Void();
}

Return<void> LocHidlIzatSubscription::timezoneChangeUpdate(
        const ILocHidlIzatSubscription::TimeZoneChangeDataItem& di) {

    FROM_HIDL_CLIENT();

    if (Subscription::mObserverObj == NULL) {
        LOC_LOGE("null observer !");
        return Void();
    }

    izat_manager::TimeZoneChangeDataItem timezonechange_data_item(di.curTimeMillis,
                                                    di.rawOffset, di.dstOffset);

    std::list<IDataItemCore *> dataItem_list;
    dataItem_list.push_back(&timezonechange_data_item);

    Subscription::mObserverObj->notify(dataItem_list);

    return Void();
}

Return<void> LocHidlIzatSubscription::timeChangeUpdate(
        const ILocHidlIzatSubscription::TimeChangeDataItem& di) {

    FROM_HIDL_CLIENT();

    if (Subscription::mObserverObj == NULL) {
        LOC_LOGE("null observer !");
        return Void();
    }

    izat_manager::TimeChangeDataItem timechange_data_item(di.curTimeMillis,
                                            di.rawOffset, di.dstOffset);

    std::list<IDataItemCore *> dataItem_list;
    dataItem_list.push_back(&timechange_data_item);

    Subscription::mObserverObj->notify(dataItem_list);
    return Void();
}

Return<void> LocHidlIzatSubscription::shutdownUpdate() {
    FROM_HIDL_CLIENT();

    if (Subscription::mObserverObj == NULL) {
        LOC_LOGE("null observer !");
    return Void();
}

    ShutdownStateDataItem shutdown_data_item(true);

    std::list<IDataItemCore *> dataItem_list;
    dataItem_list.push_back(&shutdown_data_item);

    Subscription::mObserverObj->notify(dataItem_list);
    return Void();
}

Return<void> LocHidlIzatSubscription::wifiSupplicantStatusUpdate(
        const ILocHidlIzatSubscription::WifiSupplicantStatusDataItem& di) {

    FROM_HIDL_CLIENT();

    if (Subscription::mObserverObj == NULL) {
        LOC_LOGE("null observer !");
        return Void();
    }

    izat_manager::WifiSupplicantStatusDataItem wifisupplicantstatus_data_item;

    wifisupplicantstatus_data_item.mState =
            (izat_manager::WifiSupplicantStatusDataItem::WifiSupplicantState)di.state;
    wifisupplicantstatus_data_item.mApMacAddressValid = di.apMacAddressValid;
    if((di.apMacAddress.size() > 0) &&
       (wifisupplicantstatus_data_item.mApMacAddressValid == 1)) {
        for(int i=0; i<MAC_ADDRESS_LENGTH; i++) {
            wifisupplicantstatus_data_item.mApMacAddress[i] = di.apMacAddress[i];
        }
    }

    wifisupplicantstatus_data_item.mWifiApSsidValid = di.apSsidValid;
    wifisupplicantstatus_data_item.mWifiApSsid = di.apSsid;

    IDataItemCore *dataItem = &wifisupplicantstatus_data_item;

    std::list<IDataItemCore *> dataItem_list;
    dataItem_list.push_back(dataItem);

    Subscription::mObserverObj->notify(dataItem_list);
    return Void();
}

Return<void> LocHidlIzatSubscription::btleScanDataInject(
        const ILocHidlIzatSubscription::BtLeDeviceScanDetailsDataItem& di) {

    FROM_HIDL_CLIENT();

    if (Subscription::mObserverObj == NULL) {
        LOC_LOGE("null observer !");
        return Void();
    }

    izat_manager::BtLeDeviceScanDetailsDataItem btle_device_data_item;

    btle_device_data_item.mValidSrnData = di.validSrnData;
    btle_device_data_item.mErrorCause = di.errorCause;
    if (di.validSrnData) {
        if (di.apSrnMacAddress.size() > 0) {
            for(int i=0;i<SRN_MAC_ADDRESS_LENGTH;i++) {
                btle_device_data_item.mApSrnMacAddress[i] = di.apSrnMacAddress[i];
            }
        }
        btle_device_data_item.mApSrnRssi = di.apSrnRssi;
        btle_device_data_item.mApSrnTimestamp = di.apSrnTimestamp;
        btle_device_data_item.mRequestTimestamp = di.requestTimestamp;
        btle_device_data_item.mReceiveTimestamp = di.receiveTimestamp;
    }

    std::list<IDataItemCore *> dataItem_list;
    dataItem_list.push_back(&btle_device_data_item);

    Subscription::mObserverObj->notify(dataItem_list);

    return Void();
}

Return<void> LocHidlIzatSubscription::btClassicScanDataInject(
        const ILocHidlIzatSubscription::BtDeviceScanDetailsDataItem& di) {

    FROM_HIDL_CLIENT();

    if (Subscription::mObserverObj == NULL) {
        LOC_LOGE("null observer !");
        return Void();
    }

    izat_manager::BtDeviceScanDetailsDataItem btclassic_device_data_item;

    btclassic_device_data_item.mValidSrnData = di.validSrnData;
    btclassic_device_data_item.mErrorCause = di.errorCause;
    if (di.validSrnData) {
        if (di.apSrnMacAddress.size() > 0) {
            for(int i=0;i<SRN_MAC_ADDRESS_LENGTH;i++) {
                btclassic_device_data_item.mApSrnMacAddress[i] = di.apSrnMacAddress[i];
            }
        }
        btclassic_device_data_item.mApSrnRssi = di.apSrnRssi;
        btclassic_device_data_item.mApSrnTimestamp = di.apSrnTimestamp;
        btclassic_device_data_item.mRequestTimestamp = di.requestTimestamp;
        btclassic_device_data_item.mReceiveTimestamp = di.receiveTimestamp;
    }

    std::list<IDataItemCore *> dataItem_list;
    dataItem_list.push_back(&btclassic_device_data_item);

    Subscription::mObserverObj->notify(dataItem_list);

    return Void();
}

Return<void> LocHidlIzatSubscription::batteryLevelUpdate(
        const ILocHidlIzatSubscription::BatteryLevelDataItem& di) {

    FROM_HIDL_CLIENT();

    if (Subscription::mObserverObj == NULL) {
        LOC_LOGE("null observer !");
        return Void();
    }

    izat_manager::BatteryLevelDataItem batterylevel_data_item;
    batterylevel_data_item.mBatteryPct = di.batteryPct;

    IDataItemCore *dataItem = &batterylevel_data_item;

    std::list<IDataItemCore *> dataItem_list;
    dataItem_list.push_back(dataItem);

    Subscription::mObserverObj->notify(dataItem_list);

    return Void();
}

}  // namespace implementation
}  // namespace V1_2
}  // namespace gnss
}  // namespace qti
}  // namespace vendor
