/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  GTP wwan proxy module

  Copyright (c) 2015 - 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Confidential and Proprietary.
=============================================================================*/


#define LOG_NDEBUG 0
#define LOG_TAG "IzatSvc_GtpWwanProxy"

#include "GTPWWanProxy.h"
#include "Utils.h"
#include "loc_cfg.h"
#include <log_util.h>
namespace izat_manager
{

using namespace qc_loc_fw;
using namespace loc_core;

ILocationProvider* GtpWWanProxy::mGtpWwanProxy = NULL;

#define BREAK_AND_DELETE_IF_NON_ZERO(ERR,X,OBJ) \
if(0!=(X)) {result = (ERR); delete (OBJ); break;}

void GtpWWanProxy::enableWwanMsg::proc() const
{
    ENTRY_LOG();

    LOC_LOGD("Provider State = %d", mWwanProxy->mProviderState);

    // If the current state is WWAN_PROVIDER_STATE_IDLE || WWAN_PROVIDER_STATE_WAIT_FOR_LOCATION,
    // then do not do anything. Could be just a repeated enable call.
    if (WWAN_PROVIDER_STATE_DISABLED == mWwanProxy->mProviderState) {
        mWwanProxy->mProviderState = WWAN_PROVIDER_STATE_IDLE;
    }

    EXIT_LOG_WITH_ERROR("%d", 0);
}

void GtpWWanProxy::disableWwanMsg::proc() const
{
    ENTRY_LOG();

    // On-going request cannot be stopped.
    // Just stop any running timeout timer
    // Just change the state to disabled and exit.
    // If location comes after changing the state to disabled, location will be dropped.

    mWwanProxy->mFixTimeoutTimer.stop();
    mWwanProxy->mProviderState = WWAN_PROVIDER_STATE_DISABLED;

    EXIT_LOG_WITH_ERROR("%d", 0);
}

void GtpWWanProxy::positionMsgApWwan::proc() const
{
    int result = -1;
    ENTRY_LOG();

    do {
        LOC_LOGD("ProviderState = %d", mWwanProxy->mProviderState);

        mWwanProxy->mFixTimeoutTimer.stop();

        BREAK_IF_ZERO(1, WWAN_PROVIDER_STATE_WAIT_FOR_LOCATION == mWwanProxy->mProviderState);

        BREAK_IF_ZERO(2, mIncard);

        int response_txid;
        BREAK_IF_NON_ZERO(3, mIncard->getInt32("TX-ID", response_txid));

        if (response_txid != mWwanProxy->mTxId) {
            LOC_LOGD("Expected TX-ID=%d Got TX-ID=%d, continue to wait", mWwanProxy->mTxId, response_txid);
            result = 4;
            break;
        }

        double lat_deg = 0;
        double lon_deg = 0;
        float accuracy = 0;

        if ((0 == mIncard->getDouble("LATITUDE_DEG", lat_deg)) &&
            (0 == mIncard->getDouble("LONGITUDE_DEG", lon_deg)) &&
            (0 == mIncard->getFloat("HOR_UNC_M", accuracy))) {

            LOC_LOGD("got final location (%.7lf,%.7lf) at error %.3f",
                lat_deg, lon_deg, accuracy);

            mWwanProxy->mLocationReport.mHasLatitude = true;
            mWwanProxy->mLocationReport.mLatitude = lat_deg;
            mWwanProxy->mLocationReport.mHasLongitude = true;
            mWwanProxy->mLocationReport.mLongitude = lon_deg;
            mWwanProxy->mLocationReport.mHasHorizontalAccuracy = true;
            mWwanProxy->mLocationReport.mHorizontalAccuracy = accuracy;
            mWwanProxy->mLocationReport.mHasPositionSource = true;
            mWwanProxy->mLocationReport.mPositionSource = CELLID;
            mWwanProxy->mLocationReport.mHasProcessorSource = true;
            mWwanProxy->mLocationReport.mProcessorSource = LocationReport::ProcessorSrc_AP;

            // utc time
            struct timespec time_info_current;
            if (0 == clock_gettime(CLOCK_REALTIME,&time_info_current)) {
                mWwanProxy->mLocationReport.mUtcTimestampInMsec = (time_info_current.tv_sec)*1e3 +
                                    (time_info_current.tv_nsec)/1e6;
                mWwanProxy->mLocationReport.mHasUtcTimestampInMsec = true;
            }

            // elapsed real time nano seconds of when the fix was made
            if (0 == GetTimeSinceBoot(mWwanProxy->mLocationReport.mElapsedRealTimeInNanoSecs)) {
                mWwanProxy->mLocationReport.mHasElapsedRealTimeInNanoSecs = true;
            }

            float vert_unc_meter;
            float altitude_wrt_ellipsoid;
            if((0 == mIncard->getFloat("VER_UNC_M", vert_unc_meter)) &&
               (0 == mIncard->getFloat("ALT_ELP_M", altitude_wrt_ellipsoid))) {
                if ((altitude_wrt_ellipsoid >= -500) && (altitude_wrt_ellipsoid <= 15800)) {
                    mWwanProxy->mLocationReport.mHasAltitudeWrtEllipsoid = true;
                    mWwanProxy->mLocationReport.mAltitudeWrtEllipsoid = altitude_wrt_ellipsoid;

                    mWwanProxy->mLocationReport.mHasVertUnc = true;
                    mWwanProxy->mLocationReport.mVertUnc = vert_unc_meter;

                    mWwanProxy->mLocationReport.mHasVerticalReliability = true;
                    mWwanProxy->mLocationReport.mVerticalReliability = RELIABILITY_MEDIUM;

                    mWwanProxy->mLocationReport.mHasVerticalConfidence = true;
                    mWwanProxy->mLocationReport.mVerticalConfidence = 68;
                }
            }

            // report locations to all subscribed clients
            mWwanProxy->broadcastLocation(&mWwanProxy->mLocationReport);

            } else {
            LOC_LOGD("no valid accuracy/location information found");

            mWwanProxy->mLocationError.mHasErrorType = true;
            mWwanProxy->mLocationError.mLocationErrorType = LocationError::LocationError_NoPosition;
            mWwanProxy->broadcastError(&mWwanProxy->mLocationError);
        }

        mWwanProxy->mProviderState = WWAN_PROVIDER_STATE_IDLE;
     result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

GtpWWanProxy::positionMsgApWwan::~positionMsgApWwan()
{
    if (mIncard) { delete mIncard; mIncard = NULL;}
}

void GtpWWanProxy::positionMsgFromModem::proc() const
{
    int result = -1;
    ENTRY_LOG();

    do {
        LOC_LOGD("ProviderState = %d", mWwanProxy->mProviderState);

        mWwanProxy->mFixTimeoutTimer.stop();

        BREAK_IF_ZERO(1, WWAN_PROVIDER_STATE_WAIT_FOR_LOCATION == mWwanProxy->mProviderState ||
            WWAN_PROVIDER_STATE_WAIT_FOR_QUICK_LOCATION == mWwanProxy->mProviderState);

        if ((mGpsLocation.flags & LOC_GPS_LOCATION_HAS_LAT_LONG) &&
             (mGpsLocation.flags & LOC_GPS_LOCATION_HAS_ACCURACY)) {

            LOC_LOGD("got final location (%.7lf,%.7lf) at error %.3f",
             mGpsLocation.latitude, mGpsLocation.longitude, mGpsLocation.accuracy);

            mWwanProxy->mLocationReport.mHasLatitude = true;
            mWwanProxy->mLocationReport.mLatitude = mGpsLocation.latitude;
            mWwanProxy->mLocationReport.mHasLongitude = true;
            mWwanProxy->mLocationReport.mLongitude = mGpsLocation.longitude;
            mWwanProxy->mLocationReport.mHasHorizontalAccuracy = true;
            mWwanProxy->mLocationReport.mHorizontalAccuracy = mGpsLocation.accuracy;
            mWwanProxy->mLocationReport.mHasPositionSource = true;
            mWwanProxy->mLocationReport.mPositionSource = CELLID;
            mWwanProxy->mLocationReport.mHasProcessorSource = true;
            mWwanProxy->mLocationReport.mProcessorSource = LocationReport::ProcessorSrc_Modem;
            mWwanProxy->mLocationReport.mHasUtcTimestampInMsec = true;
            mWwanProxy->mLocationReport.mUtcTimestampInMsec = mGpsLocation.timestamp;

            if (mGpsLocation.flags & LOC_GPS_LOCATION_HAS_ALTITUDE) {
               mWwanProxy->mLocationReport.mHasAltitudeWrtEllipsoid = true;
               mWwanProxy->mLocationReport.mAltitudeWrtEllipsoid = mGpsLocation.altitude;
            }

            if (mGpsLocation.flags & LOC_GPS_LOCATION_HAS_VERT_UNCERTAINITY) {
                mWwanProxy->mLocationReport.mHasVertUnc = true;
                mWwanProxy->mLocationReport.mVertUnc = mGpsLocation.vertUncertainity;
            }

            // Elapsed real time in nanoseconds of when the fix was made
            if (0 == GetTimeSinceBoot(mWwanProxy->mLocationReport.mElapsedRealTimeInNanoSecs)) {
                mWwanProxy->mLocationReport.mHasElapsedRealTimeInNanoSecs = true;
            }

             // report locations to all subscribed clients
            mWwanProxy->broadcastLocation(&mWwanProxy->mLocationReport);
        } else {
            LOC_LOGD("no valid accuracy/location information found");

            mWwanProxy->mLocationError.mHasErrorType = true;
            mWwanProxy->mLocationError.mLocationErrorType = LocationError::LocationError_NoPosition;
            mWwanProxy->broadcastError(&mWwanProxy->mLocationError);
        }

        mWwanProxy->mProviderState = WWAN_PROVIDER_STATE_IDLE;
     result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

void GtpWWanProxy::timeoutMsgWwan::proc() const
{
    int result = -1;
    ENTRY_LOG();

    do {
        BREAK_IF_ZERO(1, WWAN_PROVIDER_STATE_WAIT_FOR_LOCATION == mWwanProxy->mProviderState ||
            WWAN_PROVIDER_STATE_WAIT_FOR_QUICK_LOCATION == mWwanProxy->mProviderState);

        if (WWAN_PROVIDER_STATE_WAIT_FOR_QUICK_LOCATION == mWwanProxy->mProviderState) {
            // Immediate location expired, so send out timeout immediate fix error
            mWwanProxy->mLocationError.mLocationErrorType = LocationError::LocationError_TimeoutImmediateFix;
            // Final location may come later, so start a longer timer
            mWwanProxy->mProviderState = WWAN_PROVIDER_STATE_WAIT_FOR_LOCATION;
            mWwanProxy->mFixTimeoutTimer.start(mWwanProxy->mMaxTimeForModemWwanFix - mWwanProxy->mQuickFixWaitTimeMaxInMsec, false);
        } else {
            // Final location expired, so send out Timeout error
            mWwanProxy->mLocationError.mLocationErrorType = LocationError::LocationError_Timeout;
            mWwanProxy->mProviderState = WWAN_PROVIDER_STATE_IDLE;
        }

        // send out the error message
        mWwanProxy->mLocationError.mHasErrorType = true;
        mWwanProxy->broadcastError(&mWwanProxy->mLocationError);
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", 0);
}

void GtpWWanProxy::requestWwanLocationMsg::proc() const
{
    ENTRY_LOG();
    int result = -1;
    OutPostcard* card = NULL;

    do {

        LOC_LOGD("ProviderState = %d, Emergency Request = %d",
            mWwanProxy->mProviderState, mLocationRequest.emergencyRequest);

        BREAK_IF_NON_ZERO(1, WWAN_PROVIDER_STATE_DISABLED == mWwanProxy->mProviderState);

        if (LocationRequestAction_Start == mLocationRequest.action) {

            if (WWAN_PROVIDER_STATE_WAIT_FOR_LOCATION == mWwanProxy->mProviderState ||
                WWAN_PROVIDER_STATE_WAIT_FOR_QUICK_LOCATION == mWwanProxy->mProviderState) {
                LOC_LOGD("NLP Fix request is ongoing, incorrect state");
                break;
            }

            if (mWwanProxy->mAPWwanServiceEnabled) {
                // start a timer for AP WWAN request
                mWwanProxy->mFixTimeoutTimer.start(mWwanProxy->mMaxTimeForApWWanFix, false);

                // send out the position request
                card = OutPostcard::createInstance();
                BREAK_IF_ZERO(4, card);
                BREAK_IF_NON_ZERO(5, card->init());
                BREAK_IF_NON_ZERO(6, card->addString("TO", "XTWWAN-PE"));
                BREAK_IF_NON_ZERO(7, card->addString("FROM", mWwanProxy->mProviderTag.c_str()));
                BREAK_IF_NON_ZERO(8, card->addString("REQ", "POSITION"));
                BREAK_IF_NON_ZERO(9, card->addBool("EMERGENCY-REQUEST", mLocationRequest.emergencyRequest));
                BREAK_IF_NON_ZERO(10, card->addInt32("TX-ID", ++mWwanProxy->mTxId));
                BREAK_IF_NON_ZERO(11, card->finalize());
                BREAK_IF_NON_ZERO(12,
                    mWwanProxy->mIzatContext->mIPCMessagingProxyObj->sendMsg(card, "XTWWAN-PE"));

                // Automation log. Please do not touch.
                LOC_LOGD("fireGTPFixSession: Fire a position request to GTP-AP WWAN %d",
                          mWwanProxy->mTxId);

                // change provider state to tracking
                mWwanProxy->mProviderState = WWAN_PROVIDER_STATE_WAIT_FOR_LOCATION;
            } else {
                // start a timer for Modem WWAN request
                mWwanProxy->mFixTimeoutTimer.start(mWwanProxy->mQuickFixWaitTimeMaxInMsec, false);
                mWwanProxy->mLBSAdapter->getZppFixRequest(ZPP_FIX_WWAN);

                // change provider state to tracking
                mWwanProxy->mProviderState = WWAN_PROVIDER_STATE_WAIT_FOR_QUICK_LOCATION;
            }


        }
        else if (LocationRequestAction_Stop == mLocationRequest.action) {
            mWwanProxy->mFixTimeoutTimer.stop();
            mWwanProxy->mProviderState = WWAN_PROVIDER_STATE_IDLE;
        }

        // reset the LocationReport and LocationError state
        mWwanProxy->mLocationReport.reset();
        mWwanProxy->mLocationError.reset();

        result = 0;
    } while(0);

    delete card;
    card = NULL;

    EXIT_LOG_WITH_ERROR("%d", result);
}

void GtpWWanProxy::handleModemWwanFix(const LocGpsLocation *gpsLocation)
{
    GtpWWanProxy *wWanProxy = static_cast<GtpWWanProxy *> (GtpWWanProxy::mGtpWwanProxy);
    wWanProxy->mIzatContext->mMsgTask->sendMsg(new (nothrow) positionMsgFromModem(wWanProxy, gpsLocation));
}

GtpWWanProxy::GtpWWanProxy(const struct s_IzatContext* izatContext, bool apWwanServiceEnabled,
    bool modemWwanServiceEnabled, unsigned int quickFixWaitTimeMaxInMsec) : LocationProvider(izatContext),
    mProviderTag("GTP-WWAN-PROXY"), mLocationReport(), mLocationError(), mInitialized(false),
    mAPWwanServiceEnabled(apWwanServiceEnabled), mModemWwanServiceEnabled(modemWwanServiceEnabled),
    mTxId(0), mProviderState(WWAN_PROVIDER_STATE_DISABLED), mLBSAdapter(NULL),
    mFixTimeoutTimer(this), mMaxTimeForModemWwanFix(5000), mMaxTimeForApWWanFix(8000),
    mQuickFixWaitTimeMaxInMsec(quickFixWaitTimeMaxInMsec)
{
    int result = -1;

    ENTRY_LOG();

    do {
        readConfig();

        BREAK_IF_ZERO(1, mIzatContext->mIPCMessagingProxyObj);

        if (mAPWwanServiceEnabled) {
            // register self with IPCMessagingProxyObj here
            BREAK_IF_NON_ZERO(2,
                mIzatContext->mIPCMessagingProxyObj->registerResponseObj(mProviderTag.c_str(), this));
        } else if (mModemWwanServiceEnabled) {
            mLBSAdapter = LBSAdapter::get(0);
            BREAK_IF_ZERO(3, mLBSAdapter);
        }

        mInitialized = true;
        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

ILocationProvider* GtpWWanProxy::getInstance(const struct s_IzatContext* izatContext)
{
    int result = -1;

    bool apWwanServiceEnabled = false;
    bool modemWwanServiceEnabled = false;

    ENTRY_LOG();

    do {
        BREAK_IF_NON_ZERO(0, GtpWWanProxy::mGtpWwanProxy);
        BREAK_IF_ZERO(1, izatContext);

        // read config to find if service is enabled or not
        static char conf_gtp_cell_mode[16];
        static char conf_gtp_cell_proc[16];
        unsigned int quickFixWaitTimeMaxInMsec = 500;

        static loc_param_s_type izat_conf_param_table[] = {
            {"GTP_CELL",      &conf_gtp_cell_mode, NULL, 's'},
            {"GTP_CELL_PROC", &conf_gtp_cell_proc, NULL, 's'},
            {"NLP_COMBO_QUICK_FIX_WAIT_TIME_MAX", &quickFixWaitTimeMaxInMsec, NULL, 'n'},
        };

        ENTRY_LOG();

        UTIL_READ_CONF(mIzatConfFile.c_str(), izat_conf_param_table);

        LOC_LOGD("GTP cell set in izat.conf, proc: %s, mode: %s",
         conf_gtp_cell_proc, conf_gtp_cell_mode);

         // subtract 50ms to make the time slightly less than that in Combo Provider
         if (quickFixWaitTimeMaxInMsec > 50)
             quickFixWaitTimeMaxInMsec -= 50;

         LOC_LOGD("quickFixWaitTimeMaxInMsec: %u",
             quickFixWaitTimeMaxInMsec);

        if (strncmp(conf_gtp_cell_mode, "DISABLED", sizeof (conf_gtp_cell_mode)) == 0) {
            LOC_LOGE("GTP cell is disabled");
            result = 1;
            break;
        } else {
            if (strncmp(conf_gtp_cell_proc, "AP", sizeof (conf_gtp_cell_proc)) == 0) {
                apWwanServiceEnabled = true;
                LOC_LOGE("GTP Cell AP enabled");
            } else {
                modemWwanServiceEnabled = true;
                LOC_LOGE("GTP Cell Modem enabled");
            }
        }

        BREAK_IF_ZERO(2, apWwanServiceEnabled || modemWwanServiceEnabled);

        mGtpWwanProxy = new (std::nothrow) GtpWWanProxy(izatContext,
            apWwanServiceEnabled, modemWwanServiceEnabled, quickFixWaitTimeMaxInMsec);
        BREAK_IF_ZERO(3, mGtpWwanProxy);

        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
    return mGtpWwanProxy;
}

int GtpWWanProxy::destroyInstance()
{
    ENTRY_LOG();

    delete mGtpWwanProxy;
    mGtpWwanProxy = NULL;

    EXIT_LOG_WITH_ERROR("%d", 0);
    return 0;
}

// ILocationProvider overrides
int GtpWWanProxy::setRequest(const LocationRequest *request)
{
    ENTRY_LOG();

    int result = -1;
    do {
        BREAK_IF_ZERO(1, mInitialized);
        BREAK_IF_ZERO(2, request);
        mIzatContext->mMsgTask->sendMsg(new (nothrow) requestWwanLocationMsg(this, request));
        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}

void GtpWWanProxy::enable()
{
    mIzatContext->mMsgTask->sendMsg(new (nothrow) enableWwanMsg(this));
}

void GtpWWanProxy::disable()
{
    mIzatContext->mMsgTask->sendMsg(new (nothrow) disableWwanMsg(this));
}

void GtpWWanProxy::handleMsg(InPostcard * const in_card)
{
    int result = -1;
    ENTRY_LOG();

    do {
        BREAK_IF_ZERO(1, in_card);

        const char* resp = NULL;
        const char* from = NULL;
        int response_txid;

        BREAK_IF_NON_ZERO(3, in_card->getString("FROM", &from));
        BREAK_IF_NON_ZERO(4, in_card->getString("RESP", &resp));
        BREAK_IF_NON_ZERO(5, in_card->getInt32("TX-ID", response_txid));

        // Automation -log, Please do not touch
        LOC_LOGD("FROM: %s RESP: %s TX-ID: %d", from, resp, response_txid);

        if (0 == strcmp(resp, "POSITION")) {
                InPostcard * copy = InPostcard::createInstance();
                BREAK_AND_DELETE_IF_NON_ZERO(3, copyInCard(in_card, copy), copy);

                mIzatContext->mMsgTask->sendMsg(new (nothrow) positionMsgApWwan(this, copy));
         } else {
             result = 2;
             break;
         }

         result = 0;
        } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
    return;
}

void GtpWWanProxy::readConfig()
{
#ifdef __ANDROID__
        std::string confFile(LOC_PATH_XTWIFI_CONF);
#else
        std::string confFile("xtwifi.conf");
#endif

    static loc_param_s_type xtwifi_conf_param_table[] = {
        {"TIMEOUT_MSEC_NLP_WWAN_MP_FIX", &mMaxTimeForModemWwanFix, NULL, 'n'},
        {"TIMEOUT_MSEC_NLP_WWAN_AP_FIX", &mMaxTimeForApWWanFix, NULL, 'n'},
    };

    UTIL_READ_CONF(confFile.c_str(), xtwifi_conf_param_table);

    // subtract 50ms to make the time slightly less than that in Combo Provider
    if (mMaxTimeForModemWwanFix > 50)
        mMaxTimeForModemWwanFix -= 50;
    // time should not be less than mQuickFixWaitTimeMaxInMsec
    mMaxTimeForModemWwanFix = max(mQuickFixWaitTimeMaxInMsec, mMaxTimeForModemWwanFix);

    LOC_LOGD(" TIMEOUT_MSEC_NLP_WWAN_MP_FIX = %d, TIMEOUT_MSEC_NLP_WWAN_AP_FIX = %d",
        mMaxTimeForModemWwanFix, mMaxTimeForApWWanFix);

    EXIT_LOG_WITH_ERROR("%d", 0);
}

void GtpWWanProxy::Timer::timeOutCallback()
{
    ENTRY_LOG();
    mClient->mIzatContext->mMsgTask->sendMsg(new (nothrow) timeoutMsgWwan(mClient));
    EXIT_LOG_WITH_ERROR("%d", 0);
}

} // namespace izat_manager

WwanFixMsg::WwanFixMsg(LocGpsLocation *gpsLocation)
{
    memcpy(&mGpsLocation, gpsLocation, sizeof(LocGpsLocation));
}

void WwanFixMsg::proc() const
{
    int result = -1;
    ENTRY_LOG();

    do {
        izat_manager::GtpWWanProxy::handleModemWwanFix(&mGpsLocation);
        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}
