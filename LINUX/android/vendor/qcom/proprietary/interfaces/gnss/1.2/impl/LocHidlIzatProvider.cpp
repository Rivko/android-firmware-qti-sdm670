/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#define LOG_TAG "LocSvc_HIDL_IzatProvider"
#define LOG_NDEBUG 0
#include "LocHidlIzatProvider.h"
#include <log_util.h>
#include <LocHidlUtils.h>
#include <OSFramework.h>

using namespace izat_manager;

namespace vendor {
namespace qti {
namespace gnss {
namespace V1_2 {
namespace implementation {

using ::vendor::qti::gnss::V1_0::LocHidlIzatStreamType;
using ::vendor::qti::gnss::V1_0::LocHidlIzatLocation;
using ::vendor::qti::gnss::V1_0::LocHidlIzatProviderStatus;

class Listener : public IOSListener {
    sp<ILocHidlIzatProviderCallback> mHidlCbIface;
    IzatStreamType mStreamType;
    bool mIsEnabled;
public:
    inline Listener(const sp<ILocHidlIzatProviderCallback>& hidlCbIface,
                    IzatStreamType streamType) :
        IOSListener(), mHidlCbIface(hidlCbIface),
        mStreamType(streamType), mIsEnabled (false) {}
    inline ~Listener() { mIsEnabled = false; }

    inline bool isEnabled() { return mIsEnabled; }
    inline void setEnabled(bool enabled) { mIsEnabled = enabled; }

    inline virtual IzatListenerMask listensTo() const override {
        return mStreamType;
    }

    inline sp<ILocHidlIzatProviderCallback>& getHidlCbIface() {
        return mHidlCbIface;
    }

    virtual void onLocationChanged(
            const izat_manager::IzatLocation* location,
            const izat_manager::IzatLocationStatus) override;

    virtual void onStatusChanged(
            const izat_manager::IzatProviderStatus status) override;

    inline virtual void onNmeaChanged(const IzatNmea*) override {}
};

void Listener::onLocationChanged(
        const izat_manager::IzatLocation * location,
        const izat_manager::IzatLocationStatus) {

    if (mHidlCbIface == nullptr) {
        LOC_LOGE("mHidlCbIface null !");
        return;
    }

    if (mIsEnabled) {
        LocHidlIzatLocation gnssLocation;
        LocHidlUtils::izatLocationToGnssIzatLocation(*location, gnssLocation);

        TO_HIDL_CLIENT();
        auto r = mHidlCbIface->onLocationChanged(gnssLocation);
        if (!r.isOk()) {
            LOC_LOGE("Error invoking HIDL CB [%s]", r.description().c_str());
        }
    }
}

void Listener::onStatusChanged (
        const izat_manager::IzatProviderStatus status) {

    if (mHidlCbIface == nullptr) {
        LOC_LOGE("mHidlCbIface null !");
        return;
    }

    if (mIsEnabled) {
        LocHidlIzatProviderStatus gnssStatus = LocHidlUtils::izatStatusToGnssIzatStatus(status);

        TO_HIDL_CLIENT();
        auto r = mHidlCbIface->onStatusChanged(gnssStatus);
        if (!r.isOk()) {
            LOC_LOGE("Error invoking HIDL CB [%s]", r.description().c_str());
        }
    }
}

LocHidlIzatProvider::LocHidlIzatProvider(LocHidlIzatStreamType privderType) :
        mListener(NULL),
        mIzatManager(getIzatManager(OSFramework::getOSFramework())),
        mProviderType(LocHidlUtils::gnssIzatStreamToIzatStreamType(privderType)) {
}

// Methods from ::vendor::qti::gnss::V1_0::IIzatProvider follow.
Return<bool> LocHidlIzatProvider::init(const sp<ILocHidlIzatProviderCallback>& callback) {
    static uint64_t deathCountNetwork = 0;
    static uint64_t deathCountFused = 0;
    FROM_HIDL_CLIENT();

    if (mIzatManager != NULL && callback != NULL) {
        mListener = new Listener(callback, mProviderType);
        // Register death recipient
        if (mDeathRecipient == nullptr) {
            mDeathRecipient = new LocHidlDeathRecipient([this] {
                    LOC_LOGE("%s]: ILocHidlIzatProviderCallback died.", __func__);
                    deinit();
                    });
        }
        uint64_t& deathCount =
            (mProviderType == IZAT_STREAM_FUSED ? deathCountFused : deathCountNetwork);
        mDeathRecipient->registerToPeer(mListener->getHidlCbIface(), deathCount++);
        mIzatManager->subscribeListener(mListener);
    }

    return (mListener != nullptr);
}

Return<void> LocHidlIzatProvider::deinit() {
    onDisable();
    FROM_HIDL_CLIENT();
    auto listener = mListener;
    mListener = nullptr;
    if (listener != nullptr) {
        mIzatManager->unsubscribeListener(listener);
    }
    return Void();
}

Return<bool> LocHidlIzatProvider::onEnable() {
    FROM_HIDL_CLIENT();

    bool success = false;
    if (mListener != NULL) {
        if (!mListener->isEnabled()) {
            mIzatManager->enableProvider(mProviderType);
            mListener->setEnabled(true);
        }
        success = true;
    }

    return success;
}

Return<bool> LocHidlIzatProvider::onDisable() {
    FROM_HIDL_CLIENT();

    bool success = false;
    if (mListener != NULL) {
        if (mListener->isEnabled()) {
            mIzatManager->disableProvider(mProviderType);
            mListener->setEnabled(false);
        }
        success = true;
}

    return success;
}

Return<bool> LocHidlIzatProvider::onAddRequest(const LocHidlIzatRequest& gnssRequest) {

    FROM_HIDL_CLIENT();

    bool success = false;
    if (mListener != NULL && mListener->isEnabled()) {
        IzatRequest request;
        createIzatRequest(request, gnssRequest);
        mIzatManager->addRequest(&request);
        success = true;
}

    return success;
}

Return<bool> LocHidlIzatProvider::onRemoveRequest(const LocHidlIzatRequest& gnssRequest) {

    FROM_HIDL_CLIENT();

    bool success = false;
    if (mListener != NULL && mListener->isEnabled()) {
        IzatRequest request;
        createIzatRequest(request, gnssRequest);
        mIzatManager->removeRequest(&request);
        success = true;
    }

    return success;
}

void LocHidlIzatProvider::createIzatRequest(IzatRequest& request,
                                     const LocHidlIzatRequest& gnssRequest) {
    IzatHorizontalAccuracy izatAcc;

    switch (gnssRequest.suggestedHorizontalAccuracy) {
    case V1_0::LocHidlIzatHorizontalAccuracy::FINE:
        izatAcc = IZAT_HORIZONTAL_FINE;
        break;
    case V1_0::LocHidlIzatHorizontalAccuracy::BLOCK:
        izatAcc = IZAT_HORIZONTAL_BLOCK;
        break;
    default:
        izatAcc = IZAT_HORIZONTAL_NONE;
        break;
    }

    request.setProvider(mProviderType);
    request.setHorizontalAccuracy(izatAcc);

    request.setNumUpdates(gnssRequest.numUpdates);
    request.setInterval(gnssRequest.timeIntervalBetweenFixes);
    request.setDistance(gnssRequest.smallestDistanceBetweenFixes);
}

}  // namespace implementation
}  // namespace V1_2
}  // namespace gnss
}  // namespace qti
}  // namespace vendor
