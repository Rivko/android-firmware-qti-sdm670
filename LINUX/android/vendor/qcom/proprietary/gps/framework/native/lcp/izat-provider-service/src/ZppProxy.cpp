/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  Zpp proxy module

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Confidential and Proprietary.
=============================================================================*/


#define LOG_NDEBUG 0
#define LOG_TAG "IzatSvc_ZppProxy"

#include "ZppProxy.h"
#include "Utils.h"
#include "loc_cfg.h"
#include <log_util.h>
namespace izat_manager
{

using namespace loc_core;

ILocationProvider* ZppProxy::mZppProxy = NULL;

void ZppProxy::enableZppMsg::proc() const
{
    ENTRY_LOG();

    LOC_LOGD("Provider State = %d", mZppProxy->mProviderState);

    // If the current state is ZPP_PROVIDER_STATE_IDLE || ZPP_PROVIDER_STATE_WAIT_FOR_LOCATION,
    // then do not do anything. Could be just a repeated enable call.
    if (ZPP_PROVIDER_STATE_DISABLED == mZppProxy->mProviderState) {
        mZppProxy->mProviderState = ZPP_PROVIDER_STATE_IDLE;
    }

    EXIT_LOG_WITH_ERROR("%d", 0);
}

void ZppProxy::disableZppMsg::proc() const
{
    ENTRY_LOG();

    // On-going request cannot be stopped.
    // Just stop any running timeout timer
    // Just change the state to disabled and exit.
    // If location comes after changing the state to disabled, location will be dropped.

    mZppProxy->mFixTimeoutTimer.stop();
    mZppProxy->mProviderState = ZPP_PROVIDER_STATE_DISABLED;

    EXIT_LOG_WITH_ERROR("%d", 0);
}

void ZppProxy::positionMsgFromModem::proc() const
{
    int result = -1;
    ENTRY_LOG();

    do {
        LOC_LOGD("ProviderState = %d", mZppProxy->mProviderState);

        mZppProxy->mFixTimeoutTimer.stop();

        BREAK_IF_ZERO(1, ZPP_PROVIDER_STATE_WAIT_FOR_LOCATION == mZppProxy->mProviderState);

        if ((mGpsLocation.flags & LOC_GPS_LOCATION_HAS_LAT_LONG) &&
             (mGpsLocation.flags & LOC_GPS_LOCATION_HAS_ACCURACY)) {

            LOC_LOGD("got final location (%.7lf,%.7lf) at error %.3f",
             mGpsLocation.latitude, mGpsLocation.longitude, mGpsLocation.accuracy);

            mZppProxy->mLocationReport.mHasLatitude = true;
            mZppProxy->mLocationReport.mLatitude = mGpsLocation.latitude;
            mZppProxy->mLocationReport.mHasLongitude = true;
            mZppProxy->mLocationReport.mLongitude = mGpsLocation.longitude;
            mZppProxy->mLocationReport.mHasHorizontalAccuracy = true;
            mZppProxy->mLocationReport.mHorizontalAccuracy = mGpsLocation.accuracy;
            mZppProxy->mLocationReport.mHasPositionSource = true;
            mZppProxy->mLocationReport.mPositionSource = OTHER;
            mZppProxy->mLocationReport.mHasProcessorSource = true;
            mZppProxy->mLocationReport.mProcessorSource = LocationReport::ProcessorSrc_Modem;
            mZppProxy->mLocationReport.mHasUtcTimestampInMsec = true;
            mZppProxy->mLocationReport.mUtcTimestampInMsec = mGpsLocation.timestamp;

            if (mGpsLocation.flags & LOC_GPS_LOCATION_HAS_ALTITUDE) {
               mZppProxy->mLocationReport.mAltitudeWrtEllipsoid = mGpsLocation.altitude;
               mZppProxy->mLocationReport.mHasAltitudeWrtEllipsoid = true;
            }

            if (mGpsLocation.flags & LOC_GPS_LOCATION_HAS_SPEED) {
                mZppProxy->mLocationReport.mSpeed = mGpsLocation.speed;
                mZppProxy->mLocationReport.mHasSpeed = true;
            }

            if (mGpsLocation.flags & LOC_GPS_LOCATION_HAS_BEARING) {
                mZppProxy->mLocationReport.mBearing = mGpsLocation.bearing;
                mZppProxy->mLocationReport.mHasBearing = true;
            }

            if (mGpsLocationExtended.flags & GPS_LOCATION_EXTENDED_HAS_VERT_UNC) {
                mZppProxy->mLocationReport.mVertUnc = mGpsLocationExtended.vert_unc;
                mZppProxy->mLocationReport.mHasVertUnc = true;
            }

            if (mGpsLocationExtended.flags & GPS_LOCATION_EXTENDED_HAS_SPEED_UNC) {
                mZppProxy->mLocationReport.mSpeedUnc = mGpsLocationExtended.speed_unc;
                mZppProxy->mLocationReport.mHasSpeedUnc = true;
            }

            if (mGpsLocationExtended.flags & GPS_LOCATION_EXTENDED_HAS_BEARING_UNC) {
                mZppProxy->mLocationReport.mBearingUnc = mGpsLocationExtended.bearing_unc;
                mZppProxy->mLocationReport.mHasBearingUnc = true;
            }

            // elapsed real time nano seconds of when the fix was made
            if (0 == GetTimeSinceBoot(mZppProxy->mLocationReport.mElapsedRealTimeInNanoSecs)) {
                mZppProxy->mLocationReport.mHasElapsedRealTimeInNanoSecs = true;
            }

            // LOC_POS_TECH_MASK_DEFAULT is returned if modem does not send a valid techmask
            if (LOC_POS_TECH_MASK_DEFAULT != mSourceTechMask) {
                mZppProxy->mLocationReport.mHasSourceTechMask = true;
                mZppProxy->mLocationReport.mSourceTechMask = mSourceTechMask;
            }

             // report locations to all subscribed clients
            mZppProxy->broadcastLocation(&mZppProxy->mLocationReport);
            } else {
            LOC_LOGD("no valid accuracy/location information found");

            mZppProxy->mLocationError.mHasErrorType = true;
            mZppProxy->mLocationError.mLocationErrorType = LocationError::LocationError_NoPosition;
            mZppProxy->broadcastError(&mZppProxy->mLocationError);
        }

        mZppProxy->mProviderState = ZPP_PROVIDER_STATE_IDLE;
        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

void ZppProxy::timeoutMsgZpp::proc() const
{
    int result = -1;
    ENTRY_LOG();

    do {
        BREAK_IF_ZERO(1, ZPP_PROVIDER_STATE_WAIT_FOR_LOCATION == mZppProxy->mProviderState);

        // send out the timeout error message
        mZppProxy->mLocationError.mHasErrorType = true;
        mZppProxy->mLocationError.mLocationErrorType = LocationError::LocationError_NoPosition;
        mZppProxy->broadcastError(&mZppProxy->mLocationError);

        mZppProxy->mProviderState = ZPP_PROVIDER_STATE_IDLE;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", 0);
}

void ZppProxy::requestZppLocationMsg::proc() const
{
    ENTRY_LOG();
    int result = -1;
    OutPostcard* card = NULL;

    do {

        LOC_LOGD("ProviderState = %d", mZppProxy->mProviderState);

        BREAK_IF_NON_ZERO(1, ZPP_PROVIDER_STATE_DISABLED == mZppProxy->mProviderState);

        if (LocationRequestAction_Start == mLocationRequest.action) {

            if(ZPP_PROVIDER_STATE_WAIT_FOR_LOCATION == mZppProxy->mProviderState) {
                LOC_LOGD("ZPP Fix request is ongoing, incorrect state");
                break;
            }

            // start a timer for Zpp request
            mZppProxy->mFixTimeoutTimer.start(mZppProxy->mMaxTimeForZppFix, false);

            mZppProxy->mLBSAdapter->getZppFixRequest(ZPP_FIX_BEST_AVAILABLE);

            // change provider state to tracking
            mZppProxy->mProviderState = ZPP_PROVIDER_STATE_WAIT_FOR_LOCATION;
        }
        else if (LocationRequestAction_Stop == mLocationRequest.action) {
            mZppProxy->mFixTimeoutTimer.stop();
            mZppProxy->mProviderState = ZPP_PROVIDER_STATE_IDLE;
        }

        // reset the LocationReport and LocationError state
        mZppProxy->mLocationReport.reset();
        mZppProxy->mLocationError.reset();

        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

void ZppProxy::handleZppFix(const LocGpsLocation *gpsLocation, const GpsLocationExtended *locationExtended,
        LocPosTechMask posTechMask)
{
    ZppProxy *pZppProxy = static_cast<ZppProxy *> (ZppProxy::mZppProxy);
    pZppProxy->mIzatContext->mMsgTask->sendMsg(new (nothrow) positionMsgFromModem(pZppProxy, gpsLocation,
            locationExtended, posTechMask));
}

ZppProxy::ZppProxy(const struct s_IzatContext* izatContext) :
    LocationProvider(izatContext), mInitialized(false), mProviderState(ZPP_PROVIDER_STATE_DISABLED),
    mFixTimeoutTimer(this), mMaxTimeForZppFix(1800)
{
    int result = -1;

    ENTRY_LOG();

    do {
        readConfig();

        mLBSAdapter = LBSAdapter::get(0);
        BREAK_IF_ZERO(1, mLBSAdapter);

        mInitialized = true;
        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

ILocationProvider* ZppProxy::getInstance(const struct s_IzatContext* izatContext)
{
    int result = -1;

    ENTRY_LOG();

    do {
        BREAK_IF_NON_ZERO(0, ZppProxy::mZppProxy);
        BREAK_IF_ZERO(1, izatContext);

        mZppProxy = new (std::nothrow) ZppProxy(izatContext);
        BREAK_IF_ZERO(3, mZppProxy);

        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
    return mZppProxy;
}

int ZppProxy::destroyInstance()
{
    ENTRY_LOG();

    delete mZppProxy;
    mZppProxy = NULL;

    EXIT_LOG_WITH_ERROR("%d", 0);
    return 0;
}

// ILocationProvider overrides
int ZppProxy::setRequest(const LocationRequest *request)
{
    ENTRY_LOG();

    int result = -1;
    do {
        BREAK_IF_ZERO(1, mInitialized);
        BREAK_IF_ZERO(2, request);
        mIzatContext->mMsgTask->sendMsg(new (nothrow) requestZppLocationMsg(this, request));
        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}

void ZppProxy::enable()
{
    mIzatContext->mMsgTask->sendMsg(new (nothrow) enableZppMsg(this));
}

void ZppProxy::disable()
{
    mIzatContext->mMsgTask->sendMsg(new (nothrow) disableZppMsg(this));
}

void ZppProxy::readConfig()
{
#ifdef __ANDROID__
        std::string confFile(LOC_PATH_XTWIFI_CONF);
#else
        std::string confFile("xtwifi.conf");
#endif

    static loc_param_s_type xtwifi_conf_param_table[] = {
        {"TIMEOUT_MSEC_NLP_ZPP_FIX", &mMaxTimeForZppFix, NULL, 'n'}
    };

    UTIL_READ_CONF(confFile.c_str(), xtwifi_conf_param_table);

    LOC_LOGD(" TIMEOUT_MSEC_NLP_ZPP_FIX = %d", mMaxTimeForZppFix);

    EXIT_LOG_WITH_ERROR("%d", 0);
}

void ZppProxy::Timer::timeOutCallback()
{
    ENTRY_LOG();
    mClient->mIzatContext->mMsgTask->sendMsg(new (nothrow) timeoutMsgZpp(mClient));
    EXIT_LOG_WITH_ERROR("%d", 0);
}

} // namespace izat_manager

ZppFixMsg::ZppFixMsg(LocGpsLocation *gpsLocation, GpsLocationExtended *locationExtended,
    LocPosTechMask posTechMask) :
    mPosTechMask (posTechMask) {
    memcpy(&mGpsLocation, gpsLocation, sizeof(LocGpsLocation));
    memcpy(&mGpsLocationExtended, locationExtended, sizeof(GpsLocationExtended));
}

void ZppFixMsg::proc() const
{
    int result = -1;
    ENTRY_LOG();

    do {
        izat_manager::ZppProxy::handleZppFix(&mGpsLocation, &mGpsLocationExtended, mPosTechMask);
        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}
