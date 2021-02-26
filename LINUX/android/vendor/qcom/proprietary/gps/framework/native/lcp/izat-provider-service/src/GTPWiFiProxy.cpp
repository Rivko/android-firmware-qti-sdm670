/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  GTP wifi proxy module

  Copyright (c) 2015 - 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Confidential and Proprietary.
=============================================================================*/

#define LOG_NDEBUG 0
#define LOG_TAG "IzatSvc_GtpWiFiProxy"

#include "GTPWiFiProxy.h"
#include "Utils.h"
#include "IzatDefines.h"
#include "loc_cfg.h"
#include "WiperData.h"
#include "lowi_response.h"
#include <log_util.h>
namespace izat_manager
{
using namespace qc_loc_fw;

ILocationProvider* GtpWifiProxy::mGtpWifiProxy = NULL;

#define BREAK_AND_DELETE_IF_NON_ZERO(ERR,X,OBJ) \
if(0!=(X)) {result = (ERR); delete (OBJ); break;}

void GtpWifiProxy::enableWifiMsg::proc() const
{
    ENTRY_LOG();

    // If the current state is WIFI_PROVIDER_STATE_IDLE || WIFI_PROVIDER_STATE_WAIT_FOR_LOCATION ||
    // WIFI_PROVIDER_STATE_WAIT_FOR_FREE_FIX
    // then do not do anything. Could be just a repeated enable call.
    LOC_LOGD("Provider State = %d", mWifiProxy->mProviderState);
    if (WIFI_PROVIDER_STATE_DISABLED == mWifiProxy->mProviderState) {
        mWifiProxy->mProviderState = WIFI_PROVIDER_STATE_IDLE;
    }

    EXIT_LOG_WITH_ERROR("%d", 0);
}

void GtpWifiProxy::disableWifiMsg::proc() const
{
    ENTRY_LOG();

    // On-going request cannot be stopped.
    // Just stop any running timeout timer
    // Just change the state to disabled and exit.
    // If location comes after changing the state to disabled, location will be dropped.

    mWifiProxy->mFixTimeoutTimer.stop();
    mWifiProxy->mProviderState = WIFI_PROVIDER_STATE_DISABLED;

    // reset any pending NLP Request
    mWifiProxy->mIsNlpRequestPending = false;

    EXIT_LOG_WITH_ERROR("%d", 0);
}

void GtpWifiProxy::requestWifiLocationMsg::proc() const
{
    ENTRY_LOG();
    int result = -1;
    OutPostcard* card = NULL;

    do {

        LOC_LOGD("ProviderState = %d NeedFixFromCachedScan = %d "
            "Emergency Request = %d", mWifiProxy->mProviderState,
            mNeedFixFromCachedScan, mLocationRequest.emergencyRequest);

        BREAK_IF_NON_ZERO(1, WIFI_PROVIDER_STATE_DISABLED == mWifiProxy->mProviderState);

        if (LocationRequestAction_Start == mLocationRequest.action) {
            if (mNeedFixFromCachedScan) {
                if ((WIFI_PROVIDER_STATE_WAIT_FOR_LOCATION == mWifiProxy->mProviderState) ||
                    (WIFI_PROVIDER_STATE_WAIT_FOR_FREE_FIX == mWifiProxy->mProviderState)){
                    LOC_LOGD("Fix request ongoing, ignore this free request.");
                    break;
                }
            } else {
                if (WIFI_PROVIDER_STATE_WAIT_FOR_FREE_FIX == mWifiProxy->mProviderState) {
                    mWifiProxy->mIsNlpRequestPending = true;
                    LOC_LOGD("Fix request is ongoing, Queue up the NLP Fix request");
                    break;
                } else if (WIFI_PROVIDER_STATE_WAIT_FOR_LOCATION == mWifiProxy->mProviderState) {
                    LOC_LOGD("NLP request already in progress, incorrect state.");
                    break;
                }
            }

           if (mNeedFixFromCachedScan) {
               // find if we can make a free scan request
               if (mWifiProxy->canMakeFreeFixRequest()) {
                   mWifiProxy->mFixTimeoutTimer.start(mWifiProxy->mMaxTimeForWiFiFixFromFreeWifiScanInMsec, false);
               } else {
                    result = 3;
                    break;
               }
           } else {
               mWifiProxy->mFixTimeoutTimer.start(mWifiProxy->mMaxTimeForWiFiFixInMsec, false);
           }

           // send out the position request
           card = OutPostcard::createInstance();
           BREAK_IF_ZERO(5, card);
           BREAK_IF_NON_ZERO(6, card->init());
           BREAK_IF_NON_ZERO(7, card->addString("TO", "XTWiFi-PE"));
           BREAK_IF_NON_ZERO(8, card->addString("FROM", mWifiProxy->mProviderTag.c_str()));
           BREAK_IF_NON_ZERO(9, card->addString("REQ", "POSITION"));
           BREAK_IF_NON_ZERO(10, card->addInt32("TX-ID", ++mWifiProxy->mTxId));
           BREAK_IF_NON_ZERO(11, card->addBool("SERVER-ACCESS-ALLOWED", !(mNeedFixFromCachedScan)));
           BREAK_IF_NON_ZERO(12, card->addBool("EMERGENCY-REQUEST", mLocationRequest.emergencyRequest));
           BREAK_IF_NON_ZERO(13, card->finalize());
           BREAK_IF_NON_ZERO(14,
               mWifiProxy->mIzatContext->mIPCMessagingProxyObj->sendMsg(card, "XTWiFi-PE"));
           LOC_LOGD("fireGTPFixSession: Fire a position request to XTWiFi-PE %d", mWifiProxy->mTxId);

            // reset the LocationReport and LocationError member structures
           mWifiProxy->mLocationReport.reset();
           mWifiProxy->mLocationError.reset();

           // change provider state
           if (mNeedFixFromCachedScan) {
               BREAK_IF_NON_ZERO(15, GetTimeSinceBoot(mWifiProxy->mTimestampLastFreeFixRequestInNanoSec));
               mWifiProxy->mProviderState = WIFI_PROVIDER_STATE_WAIT_FOR_FREE_FIX;

           } else {
               BREAK_IF_NON_ZERO(16, GetTimeSinceBoot(mWifiProxy->mTimestampLastTrackingRequestInNanoSec));
               mWifiProxy->mProviderState = WIFI_PROVIDER_STATE_WAIT_FOR_LOCATION;
               mWifiProxy->mIsNlpRequestPending = false;
           }
       } else if (LocationRequestAction_Stop == mLocationRequest.action) {
            if (WIFI_PROVIDER_STATE_WAIT_FOR_LOCATION == mWifiProxy->mProviderState) {
                mWifiProxy->mFixTimeoutTimer.stop();
                mWifiProxy->mProviderState = WIFI_PROVIDER_STATE_IDLE;
            }

           // reset any pending NLP Request
           mWifiProxy->mIsNlpRequestPending = false;
      }

        result = 0;
    } while(0);

    delete card;
    card = NULL;

    EXIT_LOG_WITH_ERROR("%d", result);
}

void GtpWifiProxy::timeoutMsgWifi::proc() const
{
    int result = -1;
    ENTRY_LOG();

    LOC_LOGD("ProviderState = %d", mWifiProxy->mProviderState);

    do {
        BREAK_IF_ZERO(1, (WIFI_PROVIDER_STATE_WAIT_FOR_FREE_FIX == mWifiProxy->mProviderState ||
            WIFI_PROVIDER_STATE_WAIT_FOR_LOCATION == mWifiProxy->mProviderState));

        if (WIFI_PROVIDER_STATE_WAIT_FOR_LOCATION == mWifiProxy->mProviderState) {
            // send out the timeout error message
            mWifiProxy->reportLocationError(LocationError::LocationError_Timeout);
        } else if (WIFI_PROVIDER_STATE_WAIT_FOR_FREE_FIX == mWifiProxy->mProviderState) {
            mWifiProxy->triggerPendingNlpRequest();
        }

        // reset the provider state
        mWifiProxy->mProviderState = WIFI_PROVIDER_STATE_IDLE;
        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", 0);
}


void GtpWifiProxy::positionMsgWifi::proc() const
{
    int result = -1;
    ENTRY_LOG();

    do {
        LOC_LOGD("ProviderState = %d", mWifiProxy->mProviderState);

        if ((WIFI_PROVIDER_STATE_WAIT_FOR_LOCATION != mWifiProxy->mProviderState) &&
          (WIFI_PROVIDER_STATE_WAIT_FOR_FREE_FIX != mWifiProxy->mProviderState)) {
            LOC_LOGD("Wrong state to receive a position in, ignore");
            result = 1;
            break;
        }

        BREAK_IF_ZERO(2, mIncard);

        int response_txid;
        BREAK_IF_NON_ZERO(3, mIncard->getInt32("TX-ID", response_txid));

        if (response_txid != mWifiProxy->mTxId) {
            LOC_LOGD("Expected TX-ID=%d Got TX-ID=%d, continue to wait", mWifiProxy->mTxId, response_txid);
            result = 4;
            break;
        }

        bool preliminary_fix;
        if (0 == mIncard->getBool("PRELIMINARY-FIX", preliminary_fix)) {

             double lat_deg = 0;
             double lon_deg = 0;
             float accuracy = 0;
             int numOfApUsedForFix = 0;

            if (preliminary_fix) {
                LOC_LOGD("Got preliminary fix");

                // Not expecting preliminary fixes for a fix based on free wifi scan, since server access is not
                // allowed at this time.
                BREAK_IF_NON_ZERO(5, mWifiProxy->mProviderState == WIFI_PROVIDER_STATE_WAIT_FOR_FREE_FIX);

                if ((0 == mIncard->getInt32("NUM-AP-USED-IN-FIX", numOfApUsedForFix)) &&
                    (0 == mIncard->getDouble("LATITUDE_DEG", lat_deg)) &&
                    (0 == mIncard->getDouble("LONGITUDE_DEG", lon_deg)) &&
                    (0 == mIncard->getFloat("HOR_UNC_M", accuracy))) {

                    LOC_LOGD("Got preliminary location"
                        "(%.7lf,%.7lf) at error %.3f, continue to wait",
                        lat_deg, lon_deg, accuracy);
                    mWifiProxy->cacheFixAndReport(lat_deg, lon_deg, accuracy,
                        numOfApUsedForFix, true);
                } else {
                    LOC_LOGD("no valid accuracy/location information found, \
                        continue to wait.");
                }
            } else {
                LOC_LOGD("Got final indicator");
                mWifiProxy->resetTimer();
                bool position_valid = false;

                if ((0 == mIncard->getInt32("NUM-AP-USED-IN-FIX", numOfApUsedForFix)) &&
                    (0 == mIncard->getDouble("LATITUDE_DEG", lat_deg)) &&
                    (0 == mIncard->getDouble("LONGITUDE_DEG", lon_deg)) &&
                    (0 == mIncard->getFloat("HOR_UNC_M", accuracy))) {
                    LOC_LOGD("got final location (%.7lf,%.7lf) at error %.3f",
                        lat_deg, lon_deg, accuracy);

                    position_valid = true;

                    if (mWifiProxy->mProviderState == WIFI_PROVIDER_STATE_WAIT_FOR_LOCATION) {
                        mWifiProxy->cacheFixAndReport(lat_deg, lon_deg, accuracy,
                            numOfApUsedForFix, false);
                    }
                } else {
                      if (mWifiProxy->mProviderState == WIFI_PROVIDER_STATE_WAIT_FOR_LOCATION) {
                          LOC_LOGD("no valid accuracy/location information found");
                         mWifiProxy->reportLocationError(LocationError::LocationError_NoPosition);
                      }
                }

                if (mWifiProxy->mProviderState == WIFI_PROVIDER_STATE_WAIT_FOR_FREE_FIX) {
                    // If number of Aps used for the fix are more than 1, only then inject the result
                    // Also if the position has valid latitude and longitude only then inject the result
                    if ((numOfApUsedForFix > 1) && (position_valid)) {
                        mWifiProxy->injectWifiLocation(numOfApUsedForFix,
                            lat_deg, lon_deg, accuracy, mIncard);
                    }

                    // trigger pending NLP request if any
                    mWifiProxy->triggerPendingNlpRequest();
                }

                mWifiProxy->mProviderState = WIFI_PROVIDER_STATE_IDLE;
            }
        } else {
                LOC_LOGD("no valid accuracy/location information found");
                mWifiProxy->resetTimer();

                if (WIFI_PROVIDER_STATE_WAIT_FOR_LOCATION == mWifiProxy->mProviderState) {
                    mWifiProxy->reportLocationError(LocationError::LocationError_NoPosition);
                } else if (WIFI_PROVIDER_STATE_WAIT_FOR_FREE_FIX == mWifiProxy->mProviderState) {
                    mWifiProxy->triggerPendingNlpRequest();
                }

                mWifiProxy->mProviderState = WIFI_PROVIDER_STATE_IDLE;
        }

     result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

GtpWifiProxy::positionMsgWifi::~positionMsgWifi()
{
    if (mIncard) { delete mIncard; mIncard = NULL;}
}

GtpWifiProxy::GtpWifiProxy(const struct s_IzatContext *izatContext) :
    LocationProvider(izatContext), mProviderTag("GTP-WIFI-PROXY"),
    mLocationError(), mLocationReport(),
    mInitialized(false), mTxId(0), mLBSAdapter(NULL), mProviderState(WIFI_PROVIDER_STATE_DISABLED),
    mFixTimeoutTimer(this), mIsNlpRequestPending(false), mEnableProcessFreeWiFiScan(false),
    mMaxTimeForWiFiFixInMsec(17000), mMaxTimeForWiFiFixFromFreeWifiScanInMsec(3000),
    mMinTimeBetweenFixfOnFreeWiFiScanInMsec(20000), mTimestampLastTrackingRequestInNanoSec(0),
    mTimestampLastFreeFixRequestInNanoSec(0)
{
    ENTRY_LOG();

    int result = -1;

    do {
        readConfig();

        BREAK_IF_ZERO(1, mIzatContext->mIPCMessagingProxyObj);

        mLBSAdapter = LBSAdapter::get(0);
        BREAK_IF_ZERO(2, mLBSAdapter);

        // register self with IPCMessagingProxyObj here
        BREAK_IF_NON_ZERO(3,
            mIzatContext->mIPCMessagingProxyObj->registerResponseObj(mProviderTag.c_str(), this));

        mInitialized = true;

        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

ILocationProvider* GtpWifiProxy::getInstance(const struct s_IzatContext* izatContext)
{
    int result = -1;

    ENTRY_LOG();

    do {
        // Provider is already created
        BREAK_IF_NON_ZERO(0, GtpWifiProxy::mGtpWifiProxy);
        BREAK_IF_ZERO(1, izatContext);

        // Read config to find if service is enabled or not
        static char conf_gtp_wifi_mode[16];
        static loc_param_s_type izat_conf_param_table[] = {
           {"GTP_WIFI", &conf_gtp_wifi_mode, NULL, 's'}
        };

        UTIL_READ_CONF(mIzatConfFile.c_str(), izat_conf_param_table);

        LOC_LOGD("GTP WiFi mode set in izat.conf: %s", conf_gtp_wifi_mode);

        if(strncmp(conf_gtp_wifi_mode, "DISABLED", sizeof (conf_gtp_wifi_mode)) == 0) {
            LOC_LOGE("GTP WiFi is disabled.");
            result = 2;
            break;
        } else {
            LOC_LOGE("GTP WiFi is enabled.");
        }

        mGtpWifiProxy = new (std::nothrow) GtpWifiProxy(izatContext);
        BREAK_IF_ZERO(3, mGtpWifiProxy);

        result = 0;
    } while (0);

    EXIT_LOG_WITH_ERROR("%d", result);
    return mGtpWifiProxy;
}

// ILocationProvider overrides
int GtpWifiProxy::destroyInstance()
{
    ENTRY_LOG();

    delete mGtpWifiProxy;
    mGtpWifiProxy = NULL;

    EXIT_LOG_WITH_ERROR("%d", 0);
    return 0;
}

  // ILocationRequest overrides
int GtpWifiProxy::setRequest(const LocationRequest *request)
{
    ENTRY_LOG();

    int result = -1;
    do {
        BREAK_IF_ZERO(1, mInitialized);
        BREAK_IF_ZERO(2, request);
        mIzatContext->mMsgTask->sendMsg(new (nothrow) requestWifiLocationMsg(this, request, false));
        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}

void GtpWifiProxy::enable()
{
    mIzatContext->mMsgTask->sendMsg(new (nothrow) enableWifiMsg(this));
}

void GtpWifiProxy::disable()
{
    mIzatContext->mMsgTask->sendMsg(new (nothrow) disableWifiMsg(this));
}


// IMQServerProxyResponse Overrides
void GtpWifiProxy::handleMsg(InPostcard * const in_card)
{
    int result = -1;
    ENTRY_LOG();

    do {
        BREAK_IF_ZERO(1, mInitialized);

        BREAK_IF_ZERO(2, in_card);

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
            BREAK_AND_DELETE_IF_NON_ZERO(4, copyInCard(in_card, copy), copy);

            mIzatContext->mMsgTask->sendMsg(new (nothrow) positionMsgWifi(this, copy));
        } else {
          result = 6;
          break;
        }

         result = 0;
        } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
    return;
}

void GtpWifiProxy::readConfig()
{
#ifdef __ANDROID__
    std::string confFile(LOC_PATH_XTWIFI_CONF);
#else
    std::string confFile("xtwifi.conf");
#endif

    // read the required configuration items
    static loc_param_s_type xtwifi_conf_param_table[] = {
        {
          "ENABLE_NLP_PROCESS_FREE_WIFI_SCAN", &mEnableProcessFreeWiFiScan,
           NULL, 'n'
        },
        {
          "TIMEOUT_MSEC_NLP_XTWIFI_FINAL_FIX", &mMaxTimeForWiFiFixInMsec,
           NULL, 'n'
        },
        {
          "TIMEOUT_MSEC_FREE_XTWIFI_FIX", &mMaxTimeForWiFiFixFromFreeWifiScanInMsec,
           NULL, 'n'
        },
        {  "TIME_MSEC_NLP_MIN_TIME_BETWEEN_FIX_REQ_ON_FREE_WIFI", &mMinTimeBetweenFixfOnFreeWiFiScanInMsec,
            NULL, 'n'
        }
    };

    UTIL_READ_CONF(confFile.c_str(), xtwifi_conf_param_table);

    LOC_LOGD("ENABLE_NLP_PROCESS_FREE_WIFI_SCAN = %d TIMEOUT_MSEC_NLP_XTWIFI_FINAL_FIX = %d \
        TIMEOUT_MSEC_FREE_XTWIFI_FIX = %d TIME_MSEC_NLP_MIN_TIME_BETWEEN_FIX_REQ_ON_FREE_WIFI = %d ",
        mEnableProcessFreeWiFiScan, mMaxTimeForWiFiFixInMsec, mMaxTimeForWiFiFixFromFreeWifiScanInMsec,
        mMinTimeBetweenFixfOnFreeWiFiScanInMsec
    );

    EXIT_LOG_WITH_ERROR("%d", 0);
}

void GtpWifiProxy::Timer::timeOutCallback()
{
    ENTRY_LOG();
    mClient->mIzatContext->mMsgTask->sendMsg(new (nothrow) timeoutMsgWifi(mClient));
    EXIT_LOG_WITH_ERROR("%d", 0);
}

bool GtpWifiProxy::canMakeFreeFixRequest()
{
    ENTRY_LOG();

    int result = 0;
    bool shouldsendRequest = true;

    do {

        if (mTimestampLastFreeFixRequestInNanoSec > 0) {
            int64 nowInNanoSec = 0;
            BREAK_IF_NON_ZERO(1, GetTimeSinceBoot(nowInNanoSec));

            if (nowInNanoSec > mTimestampLastFreeFixRequestInNanoSec) {
                int64 timeElapsedInNanoSec = nowInNanoSec - mTimestampLastFreeFixRequestInNanoSec;
                if ((convertNanoSecsToMilliSecs(timeElapsedInNanoSec)) < mMinTimeBetweenFixfOnFreeWiFiScanInMsec) {
                    LOC_LOGD("Free fix request was sent out not too long ago, ignore.");
                    shouldsendRequest = false;
                    break;
                }
            } else {
                // may be the system time has rolled back, ignore the time check for now.
            }
        }

        if (mTimestampLastTrackingRequestInNanoSec > 0) {
            int64 nowInNanoSec = 0;
            BREAK_IF_NON_ZERO(2, GetTimeSinceBoot(nowInNanoSec));

            if (nowInNanoSec > mTimestampLastTrackingRequestInNanoSec) {
                int64 timeElapsedInNanoSec = nowInNanoSec - mTimestampLastTrackingRequestInNanoSec;
                if (convertNanoSecsToMilliSecs(timeElapsedInNanoSec) < mMinTimeBetweenFixfOnFreeWiFiScanInMsec) {
                    LOC_LOGD("Tracking fix request was sent out not too long ago, ignore.");
                    shouldsendRequest = false;
                    break;
                }
            } else {
                // may be the system time has rolled back, ignore the time check for now.
            }
        }

        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
    return shouldsendRequest;
}

void GtpWifiProxy::cacheFixAndReport(double latitude,
    double longitude, float accuracy, int numOfApsUsedForFix, bool isPreliminaryFix)
{
    // fill in LocationReportBase fields first
    mLocationReport.mHasLatitude = true;
    mLocationReport.mLatitude = latitude;
    mLocationReport.mHasLongitude = true;
    mLocationReport.mLongitude = longitude;
    mLocationReport.mHasHorizontalAccuracy = true;
    mLocationReport.mHorizontalAccuracy = accuracy;
    mLocationReport.mHasPositionSource = true;
    mLocationReport.mPositionSource = WIFI;
    mLocationReport.mHasProcessorSource = true;
    mLocationReport.mProcessorSource = LocationReport::ProcessorSrc_AP;
    mLocationReport.mHasPositionSourceProvider = true;
    mLocationReport.mPositionSourceProvider = INTERNAL;
    mLocationReport.mIsPreliminaryFix = isPreliminaryFix;

    // UTC time
    struct timespec time_info_current;
    if (0 == clock_gettime(CLOCK_REALTIME,&time_info_current)) {
        mLocationReport.mUtcTimestampInMsec = (time_info_current.tv_sec)*1e3 +
                            (time_info_current.tv_nsec)/1e6;
        mLocationReport.mHasUtcTimestampInMsec = true;
    }

    // Elapsed real time in nanoseconds of when the fix was made
    if (0 == GetTimeSinceBoot(mLocationReport.mElapsedRealTimeInNanoSecs)) {
        mLocationReport.mHasElapsedRealTimeInNanoSecs = true;
    }

    // fill in WifiMeasurementReport fields
    if (numOfApsUsedForFix == 0) {
        // this should not happen, still just log it and continue
        LOC_LOGE("Received WiFi Position with Number of Aps Used for position \
            calculation as 0 ");
    }

    mLocationReport.mHasNumberOfApsUsedForFix = true;
    mLocationReport.mNumberOfApsUsedForFix = numOfApsUsedForFix;

    broadcastLocation(&mLocationReport);
}

void GtpWifiProxy::reportLocationError(LocationError::LocationErrorType errorType)
{
    mLocationError.mHasErrorType = true;
    mLocationError.mLocationErrorType = errorType;

    broadcastError(&mLocationError);
}

inline
void GtpWifiProxy::resetTimer()
{
    mFixTimeoutTimer.stop();
}

void GtpWifiProxy::injectWifiLocation(int numOfApUsedForFix, double lat_deg, double lon_deg,
        float accuracy, InPostcard *incard)
{
    int result = -1;

    ENTRY_LOG();

    WifiLocation wifiInfo;
    memset(&wifiInfo, 0, sizeof(WifiLocation));

    do {
        BREAK_IF_ZERO(1, incard);

        // position has to be valid
        wifiInfo.positionValid = 1;
        wifiInfo.latitude = lat_deg;
        wifiInfo.longitude = lon_deg;
        wifiInfo.accuracy = accuracy;
        wifiInfo.fixError = 0;

        wifiInfo.numApsUsed = numOfApUsedForFix;

        // Allow to get twice more AP's than MAX_REPORTED_APS.
        // If more than MAX_REPORTED_APS the remaining will be thrown away.
        PostcardBase::UINT8 macAddressArray[MAX_REPORTED_APS * MAC_ADDRESS_LENGTH * 2];
        memset(macAddressArray, 0, sizeof(macAddressArray));
        int macArraySize = sizeof(macAddressArray);

        PostcardBase::INT16 rssiArray[MAX_REPORTED_APS * 2];
        memset(rssiArray, 0, sizeof(rssiArray));
        int rssiArraySize = sizeof(rssiArray);

        PostcardBase::INT16 channelArray[MAX_REPORTED_APS * 2];
        memset(channelArray, 0, sizeof(channelArray));
        int channelArraySize = sizeof(channelArray);

        // check if we are able to read the 3 measurement info arrays from the card
        if ((0 == incard->getArrayUInt8("SCAN_RESULT_MAC_ADDRESS", &macArraySize, macAddressArray)) &&
            (0 == incard->getArrayInt16("SCAN_RESULT_RSSI", &rssiArraySize, rssiArray)) &&
            (0 == incard->getArrayInt16("SCAN_RESULT_CHANNEL", &channelArraySize, channelArray)))
        {
            LOC_LOGD("Inject WifiLocationInfo MacArray Size = %d, RSSI Array size = %d \
                Channel Array size = %d", macArraySize, rssiArraySize, channelArraySize);

            // add check for if the array lengths are 0.
            BREAK_IF_ZERO(2, macArraySize);
            BREAK_IF_ZERO(3, rssiArraySize);
            BREAK_IF_ZERO(4, channelArraySize);

            BREAK_IF_NON_ZERO(5, macArraySize % MAC_ADDRESS_LENGTH);

            // Number of AP's being injected
            wifiInfo.apInfo.apLen = macArraySize / MAC_ADDRESS_LENGTH;

            // add checks if the array lengths mismatch
            BREAK_IF_ZERO(6, wifiInfo.apInfo.apLen == rssiArraySize);
            BREAK_IF_ZERO(7, wifiInfo.apInfo.apLen == channelArraySize);

            LOC_LOGD("Number of AP's being injected = %d", wifiInfo.apInfo.apLen);

            if (wifiInfo.apInfo.apLen > MAX_REPORTED_APS) {
                // If more than MAX_REPORTED_APS throw the remaining away.
                wifiInfo.apInfo.apLen = MAX_REPORTED_APS;
            }

            int apIndex = 0;
            int macBoundaryIndex = 0;
            for (apIndex = 0; apIndex < wifiInfo.apInfo.apLen; apIndex++) {

                for (int iMacLenIndex = 0; iMacLenIndex < MAC_ADDRESS_LENGTH; iMacLenIndex++) {
                    wifiInfo.apInfo.mac_address[macBoundaryIndex] = macAddressArray[macBoundaryIndex];
                    macBoundaryIndex++;
                }

                wifiInfo.apInfo.rssi[apIndex] = rssiArray[apIndex];
                wifiInfo.apInfo.channel[apIndex] = channelArray[apIndex];
            }

            wifiInfo.apInfoValid = 1;

        } else {
            result = 8;
            break;
        }

        mLBSAdapter->injectWifiPosition(wifiInfo);

    result = 0;
    } while (0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

void GtpWifiProxy::triggerPendingNlpRequest()
{
    ENTRY_LOG();

    if (mIsNlpRequestPending){
        LocationRequest locRequest;
        memset(&locRequest, 0, sizeof(LocationRequest));
        locRequest.action = LocationRequestAction_Start;

        mIzatContext->mMsgTask->sendMsg(
            new (nothrow) requestWifiLocationMsg(this, &locRequest, false));
    }

   EXIT_LOG_WITH_ERROR("%d", 0);
}

} // namespace izat_manager
