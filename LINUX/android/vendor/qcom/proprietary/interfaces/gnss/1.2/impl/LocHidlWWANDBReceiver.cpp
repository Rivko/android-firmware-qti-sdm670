/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#define LOG_TAG "LocSvc_HIDL_IzatWWANDBReceiver"
#define LOG_NDEBUG 0

#include "LocHidlWWANDBReceiver.h"
#include <log_util.h>
#include <gps_extended_c.h>
#include <fused_location_extended.h>
#include <LocationAPI.h>
#include <LocHidlUtils.h>
#include <izat_remote_api.h>
#include <IzatRemoteApi.h>
#include <IzatTypes.h>
#include <string>
#include "wwan_db_receiver.h"

using namespace izat_remote_api;
using namespace izat_manager;

namespace vendor {
namespace qti {
namespace gnss {
namespace V1_2 {
namespace implementation {

LocHidlWWANDBReceiver::LocHidlWWANDBReceiver() :
        mLBSAdapter(nullptr), mCallbackIface(nullptr) {

    ENTRY_LOG();
}

LocHidlWWANDBReceiver::~LocHidlWWANDBReceiver() {

    ENTRY_LOG();
}

// Methods from ::vendor::qti::gnss::V1_2::IWWANDBReceiver follow.

static sp<ILocHidlWWANDBReceiverCallback> sCallback = nullptr;
static uint64_t deathCount = 0;

Return<bool> LocHidlWWANDBReceiver::init(
        const sp<ILocHidlWWANDBReceiverCallback>& callback) {

    FROM_HIDL_CLIENT();

    if (callback == nullptr) {
        LOC_LOGE("Null callback on Init !");
    }

    // Register death recipient
    if (mDeathRecipient == nullptr) {
        mDeathRecipient = new LocHidlDeathRecipient([this] {
                LOC_LOGE("%s]: LocHidlWWANDBReceiver died.", __func__);
                sCallback = nullptr;
                mCallbackIface = nullptr;
                });
    }

    if (mCallbackIface != nullptr && mDeathRecipient != nullptr) {
        mDeathRecipient->unregisterFromPeer(mCallbackIface);
    }
    mCallbackIface = callback;
    if (mCallbackIface != nullptr && mDeathRecipient != nullptr) {
        mDeathRecipient->registerToPeer(mCallbackIface, deathCount++);
    }

    if (nullptr == mLBSAdapter) {
        mLBSAdapter = LBSAdapter::get(LOC_API_ADAPTER_BIT_FDCL_SERVICE_REQ);
        if (nullptr == mLBSAdapter) {
            LOC_LOGE("mLBSAdapter is NULL after get");
        }
    }

    return true;
}

static void DBReceiverBsListUpdateCb(BSInfo_s *bs_list, size_t bs_list_size,
                                     void * /*clientData*/) {
    std::vector<ILocHidlWWANDBReceiverCallback::LocHidlBsInfo> bsInfoList;
    int32_t bsListSize = 0;

    LOC_LOGE("Native hidl BsList Update: BS list size %d", (int) bs_list_size);
    int result = 0;

    // Sanity check
    if (sCallback == nullptr) {
        LOC_LOGE("sCallback NULL");
        return;
    }

    for (size_t i = 0; i <(size_t) bs_list_size; i++) {
         ILocHidlWWANDBReceiverCallback::LocHidlBsInfo info;
         info.cell_type = bs_list[i].cell_type;
         info.cell_id1 = bs_list[i].cell_id1;
         info.cell_id2 = bs_list[i].cell_id2;
         info.cell_id3 = bs_list[i].cell_id3;
         info.cell_id4 = bs_list[i].cell_id4;
         bsInfoList.push_back(info);
    }
    bsListSize = (int32_t) bs_list_size;
    auto r = sCallback->bsListUpdateCallback(bsInfoList, bsListSize);
    if (!r.isOk()) {
        LOC_LOGE("%s] Error from bsListUpdateCallback description=%s",
                 __func__, r.description().c_str());
    }
}

static void DBReceiverStatusUpdateCb(bool status, const char* errStr, void* /*client data*/) {
    string reason;
    if ((false == status) && (nullptr != errStr)) {
        reason.assign(errStr);
    }

    // Sanity check
    if (sCallback == nullptr) {
        LOC_LOGE("sCallback NULL");
        return;
    }
    auto r = sCallback->statusUpdateCallback(status, reason);
    if (!r.isOk()) {
        LOC_LOGE("%s] Error from statusUpdateCallback description=%s",
                 __func__, r.description().c_str());
    }
}

static void DBReceiverServiceRequestCb(void* /*client data*/) {

    // Sanity check
    if (sCallback == nullptr) {
        LOC_LOGE("sCallback NULL");
        return;
    }
    auto r = sCallback->serviceRequestCallback();
    if (!r.isOk()) {
        LOC_LOGE("%s] Error from serviceRequestCallback description=%s",
                 __func__, r.description().c_str());
    }
}

static void DBReceiverEnvNotifyCb(void* /*client data*/) {

    // Sanity check
    if (sCallback == nullptr) {
        LOC_LOGE("sCallback NULL");
        return;
    }
    auto r = sCallback->attachVmOnCallback();
    if (!r.isOk()) {
        LOC_LOGE("%s] Error from attachVmOnCallback description=%s",
                 __func__, r.description().c_str());
    }
}

Return<void> LocHidlWWANDBReceiver::registerWWANDBUpdater(
         const sp<ILocHidlWWANDBReceiverCallback>& callback) {

    FROM_HIDL_CLIENT();

    sCallback = callback;

    if (callback == nullptr) {
        LOC_LOGE("Null callback on registering null !");
    }

    if (sCallback == nullptr) {
        LOC_LOGE("Null sCallback on registering null !");
    }

    void *clientData = NULL;
    if (nullptr != mLBSAdapter) {
        FdclCallbacks fdclCallbacks {DBReceiverBsListUpdateCb,
                                     DBReceiverStatusUpdateCb,
                                     DBReceiverServiceRequestCb,
                                     DBReceiverEnvNotifyCb};
        mLBSAdapter->registerFdclCommand(fdclCallbacks, clientData);
    } else {
        LOC_LOGE("mLBSAdapter is NULL");
    }

    if (mCallbackIface != nullptr && mDeathRecipient != nullptr) {
        mDeathRecipient->unregisterFromPeer(mCallbackIface);
    }
    mCallbackIface = callback;
    if (mCallbackIface != nullptr && mDeathRecipient != nullptr) {
        mDeathRecipient->registerToPeer(mCallbackIface, deathCount++);
    }

    return Void();
}

Return<void> LocHidlWWANDBReceiver::unregisterWWANDBUpdater() {
    if (nullptr != mLBSAdapter) {
        mLBSAdapter->unregisterFdclCommand();
    } else {
        LOC_LOGE("mLBSAdapter is NULL");
    }
    sCallback = nullptr;
    return Void();
}


Return<void> LocHidlWWANDBReceiver::sendBSListRequest(int32_t expireInDays) {

    FROM_HIDL_CLIENT();

    if (nullptr != mLBSAdapter) {
        mLBSAdapter->requestFdclDataCommand(expireInDays);
    } else {
        LOC_LOGE("mLBSAdapter is NULL");
    }

    return Void();
}

Return<void>
LocHidlWWANDBReceiver::pushBSWWANDB(
        const hidl_vec<ILocHidlWWANDBReceiver::LocHidlBsLocationData>& bsLocationDataList,
        int16_t bsLocationDataListSize,
        const hidl_vec<ILocHidlWWANDBReceiver::LocHidlBsSpecialInfo>& bsSpecialInfoList,
        int16_t bsSpecialInfoListSize, int32_t daysValid) {

    FROM_HIDL_CLIENT();

    LOC_LOGE("Native hidl BsList size %d BsSpecial size %d",
              (int) bsLocationDataListSize, (int) bsSpecialInfoListSize);

    FdclData fdclData = {};
    fdclData.daysValid = daysValid;
    int result = 0;
    for (size_t i = 0; i <(size_t) bsLocationDataListSize; i++) {
        BSLocationData_s l_ad;
        l_ad.cellType = bsLocationDataList[i].cellType;
        l_ad.cellRegionID1 = bsLocationDataList[i].cellRegionID1;
        l_ad.cellRegionID2 = bsLocationDataList[i].cellRegionID2;
        l_ad.cellRegionID3 = bsLocationDataList[i].cellRegionID3;
        l_ad.cellRegionID4 = bsLocationDataList[i].cellRegionID4;
        l_ad.latitude = bsLocationDataList[i].latitude;
        l_ad.longitude = bsLocationDataList[i].longitude;
        l_ad.valid_bits = bsLocationDataList[i].valid_bits;
        l_ad.horizontal_coverage_radius = bsLocationDataList[i].horizontal_coverage_radius;
        l_ad.horizontal_confidence = bsLocationDataList[i].horizontal_confidence;
        l_ad.horizontal_reliability = bsLocationDataList[i].horizontal_reliability;
        l_ad.altitude = bsLocationDataList[i].altitude;
        l_ad.altitude_uncertainty = bsLocationDataList[i].altitude_uncertainty;
        l_ad.altitude_confidence = bsLocationDataList[i].altitude_confidence;
        l_ad.altitude_reliability = bsLocationDataList[i].altitude_reliability;

        fdclData.bsLocationDataList.push_back(l_ad);
    }

    for (size_t i = 0; i < (size_t) bsSpecialInfoListSize; i++) {
        BSSpecialInfo_s s_ad;
        s_ad.cellType = bsLocationDataList[i].cellType;
        s_ad.cellRegionID1 = bsLocationDataList[i].cellRegionID1;
        s_ad.cellRegionID2 = bsLocationDataList[i].cellRegionID2;
        s_ad.cellRegionID3 = bsLocationDataList[i].cellRegionID3;
        s_ad.cellRegionID4 = bsLocationDataList[i].cellRegionID4;
        s_ad.info = bsSpecialInfoList[i].info;
        fdclData.bsSpecialInfoList.push_back(s_ad);
    }

    if (nullptr != mLBSAdapter) {
        mLBSAdapter->injectFdclDataCommand(fdclData);
    } else {
        LOC_LOGE("mLBSAdapter is NULL");
    }

    return Void();
}


// Methods from ::android::hidl::base::V1_2::IBase follow.

ILocHidlWWANDBReceiver* HIDL_FETCH_ILocHidlWWANDBReceiver(const char* /* name */) {
    return new LocHidlWWANDBReceiver();
}

}  // namespace implementation
}  // namespace V1_2
}  // namespace gnss
}  // namespace qti
}  // namespace vendor
