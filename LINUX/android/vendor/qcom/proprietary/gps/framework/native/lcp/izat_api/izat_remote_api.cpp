/*=============================================================================
  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  =============================================================================*/

#include <izat_remote_api.h>
#include <IzatRemoteApi.h>
#include <IzatTypes.h>
#include <string>

using namespace izat_remote_api;
using namespace izat_manager;
// ======== LocationUpdater ========
class LocationUpdaterWrapper {
    const remoteClientInfo *mClientInfo;
    const void* mClientData;
    LocationUpdater *pLocUpdater = NULL;
    SvInfoUpdater *pSvInfoUpdater = NULL;
public:
    inline LocationUpdaterWrapper(remoteClientInfo *pClientInfo, void* clientData) :
                                  mClientInfo(pClientInfo), mClientData(clientData) {

        if(pClientInfo->locCb){
            pLocUpdater = new LocationUpdater(pClientInfo, clientData);
        }

        if(pClientInfo->svReportCb){
            pSvInfoUpdater = new SvInfoUpdater(pClientInfo, clientData);
        }
    }

    ~LocationUpdaterWrapper(){
        if (pLocUpdater)
            delete pLocUpdater;

        if (pSvInfoUpdater)
            delete pSvInfoUpdater;
    }
};

// ======== RawLocationUpdater ========
class RawLocationUpdaterWrapper {
    const remoteClientInfo *mClientInfo;
    const void* mClientData;
    RawLocationUpdater *pRawLocUpdater = NULL;
    RawSvInfoUpdater *pRawSvInfoUpdater = NULL;
public:
    inline RawLocationUpdaterWrapper(remoteClientInfo *pClientInfo, void* clientData) :
                                     mClientInfo(pClientInfo), mClientData(clientData) {

        if (pClientInfo->locCb){
            pRawLocUpdater = new RawLocationUpdater(pClientInfo, clientData);
        }
        if (pClientInfo->svReportCb){
            pRawSvInfoUpdater = new RawSvInfoUpdater(pClientInfo, clientData);
        }
    }

    ~RawLocationUpdaterWrapper(){
        if (pRawLocUpdater)
            delete pRawLocUpdater;

        if (pRawSvInfoUpdater)
            delete pRawSvInfoUpdater;
    }
};

void* registerLocationUpdater(remoteClientInfo *pClientInfo, void* clientData) {
    return (pClientInfo && clientData) ? new LocationUpdaterWrapper(pClientInfo, clientData) : NULL;
}

void unregisterLocationUpdater(void* locationUpdaterHandle) {
    if (locationUpdaterHandle) {
        delete (LocationUpdaterWrapper*)locationUpdaterHandle;
    }
}

void* registerRawLocationUpdater(remoteClientInfo *pClientInfo, void* clientData) {
    return (pClientInfo && clientData) ? new RawLocationUpdaterWrapper(pClientInfo, clientData) : NULL;
}

void unregisterRawLocationUpdater(void* locationUpdaterHandle) {
    if (locationUpdaterHandle) {
        delete (RawLocationUpdaterWrapper*)locationUpdaterHandle;
    }
}

// ======== SstpUpdater ========
class SstpUpdaterWrapper : public SstpUpdater {
    const sstpSiteUpdateCb mSiteCb;
    const sstpMccUpdateCb mMccCb;
    const errReportCb mErrCb;
    const void* mClientData;
public:
    inline SstpUpdaterWrapper(sstpSiteUpdateCb siteCb, sstpMccUpdateCb mccCb,
                              errReportCb errCb, void* clientData) :
        SstpUpdater(), mSiteCb(siteCb), mMccCb(mccCb), mErrCb(errCb),
        mClientData(clientData) {
    }
    inline virtual void errReport(const char* errStr) override {
        if (mErrCb != nullptr) mErrCb(errStr, (void*)mClientData);
    }
    inline virtual void siteUpdate(const char* name, double lat, double lon,
                                   float unc, int32_t uncConfidence) override {
        mSiteCb(name, lat, lon, unc, uncConfidence, (void*)mClientData);
    }
    inline virtual void mccUpdate(uint32_t mcc, const char* confidence) override {
        mMccCb(mcc, confidence, (void*)mClientData);
    }
};

void* registerSstpUpdater(sstpSiteUpdateCb siteCb, sstpMccUpdateCb mccCb,
                          errReportCb errCb, void* clientData) {
    return (siteCb && mccCb) ?
        new SstpUpdaterWrapper(siteCb, mccCb, errCb, clientData) :
        NULL;
}

void unregisterSstpUpdater(void* sstpUpdaterHandle) {
    if (sstpUpdaterHandle) {
        delete (SstpUpdaterWrapper*)sstpUpdaterHandle;
    }
}

void stopSstpUpdate(void* sstpUpdaterHandle) {
    if (sstpUpdaterHandle) {
        ((SstpUpdaterWrapper*)sstpUpdaterHandle)->stop();
    }
}
