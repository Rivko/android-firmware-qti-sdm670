/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright  (c) 2015 - 2016 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

#define LOG_TAG "OsNpGlue"
#define LOG_NDEBUG 0

#include <algorithm>
#include <list>
#include <comdef.h>
#include <IzatTypes.h>
#include <IzatRequest.h>
#include <IzatLocation.h>
#include <IIzatListener.h>
#include <OsNpGlue.h>
#include <loc_pla.h>
#include <log_util.h>

using namespace std;
using namespace izat_manager;
#ifdef __ANDROID__
using ::vendor::qti::gnss::V1_0::LocHidlIzatHorizontalAccuracy;
using ::vendor::qti::gnss::V1_0::LocHidlIzatOtherAccuracy;
using ::vendor::qti::gnss::V1_0::LocHidlIzatRequest;
using ::vendor::qti::gnss::V1_0::LocHidlIzatStreamType;
void izatRequestToGnssIzatRequest(
        const IzatRequest& ir, LocHidlIzatRequest& gir) {

    gir.provider = static_cast<LocHidlIzatStreamType>(ir.getProvider());
    gir.numUpdates = ir.getNumUpdates();
    gir.suggestedResponseTimeForFirstFix = ir.getTimeForFirstFix();
    gir.timeIntervalBetweenFixes = ir.getInterval();
    gir.smallestDistanceBetweenFixes = ir.getDistance();
    gir.suggestedHorizontalAccuracy =
            static_cast<LocHidlIzatHorizontalAccuracy>(ir.getHorizontalAccuracy());
    gir.suggestedAltitudeAccuracy =
            static_cast<LocHidlIzatOtherAccuracy>(ir.getAltitudeAccuracy());
    gir.suggestedBearingAccuracy =
            static_cast<LocHidlIzatOtherAccuracy>(ir.getBearingAccuracy());
}
#endif

OsNpGlue * OsNpGlue::mInstance = NULL;

OsNpGlue * OsNpGlue::getInstance() {
#ifdef __ANDROID__
    if (!mInstance) {
        mInstance = new OsNpGlue();
    }
#endif
    return mInstance;
}

void OsNpGlue::destroyInstance() {
    OsNpGlue * tmp = mInstance;
    mInstance = nullptr;
    if (tmp) delete tmp;
}

void OsNpGlue::onLocationChanged(IzatLocation * location) {
    list<IIzatListener *>::iterator it = mListeners.begin();
    for ( ; it != mListeners.end(); ++it) {
       (*it)->onLocationChanged(location, IZAT_LOCATION_STATUS_FINAL);
    }
}

void OsNpGlue::onStatusChanged(IzatProviderStatus status) {
    list<IIzatListener *>::iterator it = mListeners.begin();
    for ( ; it != mListeners.end(); ++it) {
       (*it)->onStatusChanged(status);
    }
}

int32 OsNpGlue::subscribe(IIzatListener * izatListener) {
    if (izatListener) {
        mListeners.push_back(izatListener);
        mListeners.unique();
    }
    return 0;
}

int32 OsNpGlue::setRequest(const IzatRequest * request)
{
    int rc = -1;
#ifdef __ANDROID__
    if (request && mOsNp != NULL) {
        LocHidlIzatRequest giRequest;
        izatRequestToGnssIzatRequest(*request, giRequest);
        auto r = mOsNp->onSetRequest(giRequest);
        if (!r.isOk()) {
            LOC_LOGE("Error invoking HIDL onSetRequest [%s]", r.description().c_str());
        } else {
            rc = 0;
        }
    }
#endif
    return rc;
}

void OsNpGlue::stopRequest()
{
#ifdef __ANDROID__
    if (mOsNp != NULL) {
        auto r = mOsNp->onStopRequest();
        if (!r.isOk()) {
            LOC_LOGE("Error invoking HIDL onStopRequest [%s]", r.description().c_str());
        }
    }
#endif
}

int32 OsNpGlue::unsubscribe(IIzatListener * izatListener) {
    if(izatListener) {
        list<IIzatListener *>::iterator it =
            find(mListeners.begin(), mListeners.end(), izatListener);
        if (it != mListeners.end()) {
            mListeners.erase(it);
        }
    }
    return 0;
}
