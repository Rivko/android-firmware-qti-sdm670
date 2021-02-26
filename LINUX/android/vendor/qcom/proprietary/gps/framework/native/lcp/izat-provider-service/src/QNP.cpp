/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  Qualcomm Network Provider module

  Copyright (c) 2015 -2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Confidential and Proprietary.
=============================================================================*/

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

The MIT License (MIT)

Copyright (c) 2015 Chris Veness

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
=============================================================================*/


#define LOG_NDEBUG 0
#define LOG_TAG "IzatSvc_QNP"

#include "QNP.h"
#include "loc_cfg.h"
#include "Utils.h"
#include <log_util.h>
#include <math.h>
#include "WiperData.h"


namespace izat_manager
{

ILocationProvider* QNP::mQNP = NULL;

void QNP::enableQNPMsg::proc() const
{
    int result = -1;

    ENTRY_LOG();

    do {
        // If the current state is TRACKING_STATE_IDLE || TRACKING_STATE_TBF ||
        // TRACKING_STATE_WAIT_FOR_WIFI_FIX || TRACKING_STATE_WAIT_FOR_WWAN_FIX ||
        // TRACKING_STATE_WAIT_FOR_BEST_AVAILABLE_FIX
        // then do not do anything. Could be just a repeated enable call.
        if (TRACKING_STATE_DISABLED == mQNP->mTrackingState) {

            mQNP->mTrackingState = TRACKING_STATE_IDLE;

            if (mQNP->mGtpWifiProvider != NULL) {
                mQNP->mGtpWifiProvider->enable();
            }

            if (mQNP->mGtpWwanProvider != NULL) {
                mQNP->mGtpWwanProvider->enable();
            }

            if (mQNP->mZppProvider != NULL) {
                mQNP->mZppProvider->enable();
            }
        }

        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

void QNP::disableQNPMsg::proc() const
{
    ENTRY_LOG();
    int result = -1;

    do {
        // On-going request / timer must be stopped
        mQNP->stopTrackingSession();

        // Just change the state to disabled and exit.
        // If location comes after changing the state to disabled, location will be dropped.
        mQNP->mTrackingState = TRACKING_STATE_DISABLED;

        if (mQNP->mGtpWifiProvider != NULL) {
            mQNP->mGtpWifiProvider->disable();
        }

        if (mQNP->mGtpWwanProvider != NULL) {
            mQNP->mGtpWwanProvider->disable();
        }

        if (mQNP->mZppProvider != NULL) {
            mQNP->mZppProvider->disable();
        }

        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}


void QNP::requestLocationMsg::proc() const
{
    ENTRY_LOG();

    LOC_LOGD("Tracking State = %d", mQNP->mTrackingState);

    if (TRACKING_STATE_DISABLED == mQNP->mTrackingState) {
        LOC_LOGD("QNP is disabled, ignore");
    } else {

        // save single-shot fix information
        mQNP->mSingleShotFixRequested =
            (LocationRequestAction_SingleShot == mLocRequest.action);

        // save emergency request information
        mQNP->mInEmergencyMode = mLocRequest.emergencyRequest;

        // start tracking request
        if ((LocationRequestAction_Start == mLocRequest.action) ||
            (LocationRequestAction_SingleShot == mLocRequest.action)) {
            if ((TRACKING_STATE_IDLE == mQNP->mTrackingState) ||
                (TRACKING_STATE_TBF == mQNP->mTrackingState)) {
                LOC_LOGD("In IDLE/TBF state, start tracking");
                mQNP->startTrackingSession();
            } else {
                LOC_LOGD("In the middle of position request, continue to track.");
            }
        } else if (LocationRequestAction_Stop == mLocRequest.action) {
            mQNP->stopTrackingSession();
        } else {
            LOC_LOGE("Unhandled LocationRequest type");
        }
    }

    EXIT_LOG_WITH_ERROR("%d", 0);
}

void QNP::setIntervalMsg::proc() const
{
    ENTRY_LOG();

    // only if current TBF changes

    // Note: a setIntervalMsg is always accompanied with a setRequest msg
    // If state TRACKING_STATE_IDLE or TRACKING_STATE_TBF, this TBF will take affect
    // immediately.
    if ((0 == mQNP->mCurrentTimeBetweenFixInMsec) ||
        (mQNP->mCurrentTimeBetweenFixInMsec != mIntervalInMsec)) {
        mQNP->mCurrentTimeBetweenFixInMsec = mIntervalInMsec;

        if (mQNP->mCurrentTimeBetweenFixInMsec < mMinAllowedInterval) {
            mQNP->mCurrentTimeBetweenFixInMsec = mMinAllowedInterval;
        }
    }

    EXIT_LOG_WITH_ERROR("%d", 0);
}

void QNP::tbfTimerMsg::proc() const
{
    ENTRY_LOG();

    int result = -1;
    do {
        LOC_LOGD("QNP Tracking state = %d", mQNP->mTrackingState);
        BREAK_IF_ZERO(1, TRACKING_STATE_TBF == mQNP->mTrackingState);

        // cache the time this request is being made
         GetTimeSinceBoot(mQNP->mTimestampLastTrackingRequestInNanoSec);

        // send out the fix requests
        mQNP->requestFixes();

        result = 0;
    } while (0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

QNP::QNP(const struct s_IzatContext *izatContext,
           ILocationProvider* pGTPWifiProvider, ILocationProvider* pGTPWwanProvider,
           ILocationProvider* pZppProvider) :
    LocationProvider(izatContext),
    mNLPConfFile(LOC_PATH_XTWIFI_CONF),
    mIzatConfFile(LOC_PATH_IZAT_CONF),
    mGtpWifiProvider(pGTPWifiProvider), mGtpWwanProvider(pGTPWwanProvider),
    mZppProvider(pZppProvider),
    mTrackingState(TRACKING_STATE_DISABLED),
    mCurrentTimeBetweenFixInMsec(0), mTimestampLastTrackingRequestInNanoSec(0),
    mTBFTimer(this), mFixReportedInCurrentSession(false), mSingleShotFixRequested(false),
    mApWwanEnabled(false), mWaitForWifiFinalFix(false), mRequestId(0),
    mMinTimeBeweenTrackingFixInMsec(3500), mMaxAcuracyThresholdToFilterNlpPosition(25000),
    mMaxZppReportableHepe(10), mMaxZppWifiConsistencyCheckInMeters(200),
    mTimeForSavedPositionToBeValidInMinutes(10),
    mGtpApMode(1), mQuickFixAccuracyMinInMeters(100),
    mCountOfWifiFixInconsistentWithZPP(0), mCountOfDroppedFixesDueToLargeRadius(0),
    mCountOfDroppedSingleApWifiFix(90), mMinTimeToReportStatsCollectedInSec(3600)
{
    readConfig();

    // initialize the statistic collection start time to current time
    GetTimeSinceBoot(mCollectionStartTimestampInNanoSec);

    if (mGtpWifiProvider != NULL) {
        mGtpWifiProvider->subscribe(this);
    }

    if (mGtpWwanProvider != NULL) {
        mGtpWwanProvider->subscribe(this);
    }

    if (mZppProvider != NULL) {
        mZppProvider->subscribe(this);
    }
}


QNP::~QNP()
{
    GtpWifiProxy::destroyInstance();
    mGtpWifiProvider = NULL;

    GtpWWanProxy::destroyInstance();
    mGtpWwanProvider = NULL;

    ZppProxy::destroyInstance();
    mZppProvider = NULL;
}

ILocationProvider* QNP::getInstance(const struct s_IzatContext* izatContext)
{
    int result = -1;
    ILocationProvider *pGtpWiFiProvider = NULL;
    ILocationProvider *pGtpWwanProvider = NULL;
    ILocationProvider *pZppProvider = NULL;

    do {
        // QNP object is already created
        BREAK_IF_NON_ZERO(0, mQNP);
        BREAK_IF_ZERO(1, izatContext);

        pGtpWiFiProvider = GtpWifiProxy::getInstance(izatContext);
        pGtpWwanProvider = GtpWWanProxy::getInstance(izatContext);
        pZppProvider = ZppProxy::getInstance(izatContext);

        // QNP exists only if atleast one of the izat features is enabled
        BREAK_IF_ZERO(2,
            ((pGtpWiFiProvider != NULL) || (pGtpWwanProvider != NULL)));

        mQNP = new (nothrow) QNP(izatContext,
                pGtpWiFiProvider, pGtpWwanProvider, pZppProvider);
        BREAK_IF_ZERO(4, mQNP);

        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR(%d, result);
    return mQNP;
}

int QNP::destroyInstance()
{
    ENTRY_LOG();

    delete mQNP;
    mQNP = NULL;

    EXIT_LOG_WITH_ERROR("%d", 0);
    return 0;
}

  // ILocationProvider overrides
int QNP::setRequest(const LocationRequest *request)
{
    ENTRY_LOG();
    int result = -1;

    do {
        BREAK_IF_ZERO(1, request);

        LOC_LOGD ("LocTech-Label :: QNP :: Position Request In");
        LOC_LOGD ("LocTech-Value :: Action (Single Shot = 0, Start = 1, Stop = 2) : %d", request->action);
        LOC_LOGD ("LocTech-Value :: Interval In milliseconds %u", request->intervalInMsec);
        LOC_LOGD ("LocTech-Value :: Emergency Request %d", request->emergencyRequest);
        mIzatContext->mMsgTask->sendMsg(new (nothrow)
            setIntervalMsg(this, request->intervalInMsec, request->emergencyRequest));
        mIzatContext->mMsgTask->sendMsg(new (nothrow) requestLocationMsg(this, request));

        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}

void QNP::enable()
{
    mIzatContext->mMsgTask->sendMsg(new (nothrow) enableQNPMsg(this));
}

void QNP::disable()
{
    mIzatContext->mMsgTask->sendMsg(new (nothrow) disableQNPMsg(this));
}

// ILocationResponse overrides

// reportLocation gets called in the context of the MsgTask thread
void QNP::reportLocation(const LocationReport *locReport,
    const ILocationProvider *providerSrc )
{
    ENTRY_LOG();

    int result = -1;
    do {
      LOC_LOGD("QNP Tracking State = %d", mTrackingState);

      if ((TRACKING_STATE_WAIT_FOR_BEST_AVAILABLE_FIX != mTrackingState) &&
          (TRACKING_STATE_WAIT_FOR_WIFI_FIX != mTrackingState) &&
          (TRACKING_STATE_WAIT_FOR_IMMEDIATE_WWAN_FIX != mTrackingState) &&
          (TRACKING_STATE_WAIT_FOR_FINAL_WWAN_FIX != mTrackingState)) {
        result = 1;
        break;
      }
      LOC_LOGD ("LocTech-Label :: QNP :: Position Report In");

      string locationReport;
      locReport->stringify (locationReport);
      LOC_LOGD ("LocTech-Value :: Position Report: %s", locationReport.c_str());

      if (providerSrc == mZppProvider) {
          LOC_LOGD ("LocTech-Value :: Provider Source: ZPP");
          const ZppProxy::ZppMeasurementReport *zppReport =
                      static_cast<const ZppProxy::ZppMeasurementReport*> (locReport);
          mZppLocReport = *zppReport;
          handleReportFromZpp();
      } else if (providerSrc == mGtpWwanProvider) {
          LOC_LOGD ("LocTech-Value :: Provider Source: GTP WWAN");
          mWwanLocReport = *locReport;
          handleReportFromWwan();
      } else if (providerSrc == mGtpWifiProvider) {
          LOC_LOGD ("LocTech-Value :: Provider Source: GTP WIFI");
        const GtpWifiProxy::WifiMeasurementReport *wifiReport =
            static_cast<const GtpWifiProxy::WifiMeasurementReport*> (locReport);
          mWifiLocReport = *wifiReport;
          handleReportFromWifi();
      }

      result = 0;
    } while(0);

   EXIT_LOG_WITH_ERROR("%d", result);
}

// reportError gets called in the context of the MsgTask thread
void QNP::reportError(const LocationError *locError,
    const ILocationProvider *providerSrc)
{
    ENTRY_LOG();

    int result = -1;
    do {
      LOC_LOGD("QNP Tracking State = %d", mTrackingState);

      if ((TRACKING_STATE_WAIT_FOR_BEST_AVAILABLE_FIX != mTrackingState) &&
          (TRACKING_STATE_WAIT_FOR_WIFI_FIX != mTrackingState) &&
          (TRACKING_STATE_WAIT_FOR_IMMEDIATE_WWAN_FIX != mTrackingState) &&
          (TRACKING_STATE_WAIT_FOR_FINAL_WWAN_FIX != mTrackingState)) {
          result = 1;
          break;
      }

      LOC_LOGD ("LocTech-Label :: QNP :: Error Report In");

      if (providerSrc == mGtpWwanProvider) {
          LOC_LOGD ("LocTech-Value :: Provider Source: GTPWWAN");
          mWwanLocError = *locError;
          handleReportFromWwan();
      } else if (providerSrc == mGtpWifiProvider) {
          LOC_LOGD ("LocTech-Value :: Provider Source: GTPWIFI");
          mWifiLocError = *locError;

          // reset any previously received wifi fix.
          // Example, this could be a preliminary wifi fix that failed consistency check,
          // after which there was an error.
          mWifiLocReport.reset();
          handleReportFromWifi();
      } else if (providerSrc == mZppProvider) {
          LOC_LOGD ("LocTech-Value :: Provider Source: ZPP");
          mZppLocError = *locError;
          handleReportFromZpp();
      }
      string errorReport;
      locError->stringify (errorReport);
      LOC_LOGD ("LocTech-Value :: Error Report: %s", errorReport.c_str ());

      result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

bool QNP::isFixSupportedWhenEULAOff()
{
    return (mGtpApMode == 3);
}

void QNP::readConfig()
{
    static char conf_gtp_cell_mode[LOC_MAX_PARAM_STRING];
    static char conf_gtp_cell_proc[LOC_MAX_PARAM_STRING];
    static char conf_quick_fix_wait_time_max[LOC_MAX_PARAM_STRING];
    static char conf_quick_fix_accuracy_min[LOC_MAX_PARAM_STRING];

    static loc_param_s_type xtwifi_conf_param_table[] = {
        {
           "TIME_MSEC_NLP_MIN_TIME_BETWEEN_FIX", &mMinTimeBeweenTrackingFixInMsec,
           NULL, 'n'
        },
        {  "LARGE_ACCURACY_THRESHOLD_TO_FILTER_NLP_POSITION", &mMaxAcuracyThresholdToFilterNlpPosition,
            NULL, 'n'
        },
        {  "HEPE_MARGIN_METER_REPORT_ZPP_LOCATION", &mMaxZppReportableHepe,
            NULL, 'n'
        },
        {  "TIME_SEC_MIN_AGE_REPORTING_STATS_COLLECTED", &mMinTimeToReportStatsCollectedInSec,
            NULL, 'n'
        },
        {  "CONSISTENCY_CHECK_ZPP_WIFI_DISTANCE_MAX_THRESHOLD_METERS", &mMaxZppWifiConsistencyCheckInMeters,
            NULL , 'n'
        },
        {  "TIME_MINUTE_INVALIDATE_LAST_KNOWN_POS", &mTimeForSavedPositionToBeValidInMinutes,
            NULL , 'n'
        },
        {  "GTP_AP_MODE", &mGtpApMode,
            NULL , 'n'
        }
    };

    ENTRY_LOG();

    UTIL_READ_CONF(mNLPConfFile.c_str(), xtwifi_conf_param_table);

    LOC_LOGD("TIME_MSEC_NLP_MIN_TIME_BETWEEN_FIX = %d \
        LARGE_ACCURACY_THRESHOLD_TO_FILTER_NLP_POSITION = %d \
        HEPE_MARGIN_METER_REPORT_ZPP_LOCATION = %d \
        TIME_SEC_MIN_AGE_REPORTING_STATS_COLLECTED = %d \
        CONSISTENCY_CHECK_ZPP_WIFI_DISTANCE_MAX_THRESHOLD_METERS = %d \
        TIME_MINUTE_INVALIDATE_LAST_KNOWN_POS = %" PRId64 ,
        mMinTimeBeweenTrackingFixInMsec, mMaxAcuracyThresholdToFilterNlpPosition,
        mMaxZppReportableHepe, mMinTimeToReportStatsCollectedInSec,
        mMaxZppWifiConsistencyCheckInMeters,
        mTimeForSavedPositionToBeValidInMinutes);

    static loc_param_s_type izat_conf_param_table[] = {
        {"GTP_CELL",      &conf_gtp_cell_mode, NULL, 's'},
        {"GTP_CELL_PROC", &conf_gtp_cell_proc, NULL, 's'},
        {"NLP_COMBO_QUICK_FIX_ACCURACY_MIN", &mQuickFixAccuracyMinInMeters, NULL, 'n'}
    };

    UTIL_READ_CONF(mIzatConfFile.c_str(), izat_conf_param_table);

    LOC_LOGD("GTP cell set in izat.conf, proc: %s, mode: %s",
     conf_gtp_cell_proc, conf_gtp_cell_mode);

    LOC_LOGD("mQuickFixAccuracyMinInMeters: %u",
             mQuickFixAccuracyMinInMeters);

    if (strncmp(conf_gtp_cell_mode, "BASIC", sizeof (conf_gtp_cell_mode)) == 0) {
        if (strncmp(conf_gtp_cell_proc, "AP", sizeof (conf_gtp_cell_proc)) == 0) {
            mApWwanEnabled = true;
            LOC_LOGE("GTP Cell AP enabled");
        }
    }

    EXIT_LOG_WITH_ERROR("%d", 0);
}

void QNP::startTrackingSession()
{
    int result = -1;
    ENTRY_LOG();

    do {
        if ((TRACKING_STATE_IDLE == mTrackingState) ||
            (TRACKING_STATE_TBF == mTrackingState)) {
            // stop tbf timer and start session immediately

            mTBFTimer.stop();

            BREAK_IF_NON_ZERO(1, GetTimeSinceBoot(mTimestampLastTrackingRequestInNanoSec));

            requestFixes();
        }

        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

void QNP::stopTrackingSession()
{
    int result = -1;
    ENTRY_LOG();

    do {
        // Nothing to do if the provider state is IDLE anyways
        BREAK_IF_NON_ZERO(1, TRACKING_STATE_IDLE == mTrackingState);

        if (TRACKING_STATE_TBF == mTrackingState) {
            mTBFTimer.stop();
        }

        stopFixes();

        // change state to IDLE
        mTrackingState = TRACKING_STATE_IDLE;

        // reset single-shot fix session status.
        mSingleShotFixRequested = false;

        // reset request count
        mRequestId = 0;
        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

void QNP::resetSession()
{
    ENTRY_LOG();

    if (mSingleShotFixRequested) {
        stopTrackingSession();
    } else {
        resetTBFTimer();
    }

    EXIT_LOG_WITH_ERROR("%d", 0);
}

void QNP::resetTBFTimer()
{
    ENTRY_LOG();

    int result = -1;

    do {
        mTBFTimer.stop();

        // send stop request to other providers in its list
        stopFixes();

        // Calculate the wait time before triggering next session
        uint32_t timeToNextFixInMsec = 0;
        int64 nowInNanoSec = 0;
        BREAK_IF_NON_ZERO(1, GetTimeSinceBoot(nowInNanoSec));

        if ((mTimestampLastTrackingRequestInNanoSec <= 0) || (mTimestampLastTrackingRequestInNanoSec > nowInNanoSec)) {
            // <= 0: we have not started any fix in this tracking session
            // > now: system time adjustment?
            // retry immediately
            timeToNextFixInMsec = 0;
        } else if (mFixReportedInCurrentSession) {
            timeToNextFixInMsec = mCurrentTimeBetweenFixInMsec;
        } else {
            // we haven't reported anything in the last fix session, delay for awhile and restart
            // we do not want to squeeze the system too much by setting shorter TBF here
            // as it contributes to harmless race condition with coarse position injection
            // and potentially too fast wifi scan rate
            timeToNextFixInMsec = mCurrentTimeBetweenFixInMsec;
        }

        if (0 == timeToNextFixInMsec) {
            mIzatContext->mMsgTask->sendMsg(new (nothrow)tbfTimerMsg(this));
        } else {
            mTBFTimer.start(timeToNextFixInMsec, false);
        }

        mTrackingState = TRACKING_STATE_TBF;

        result = 0;
    }while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

void QNP::Timer::timeOutCallback()
{
    ENTRY_LOG();
    mClient->mIzatContext->mMsgTask->sendMsg(new (nothrow)tbfTimerMsg(mClient));
    EXIT_LOG_WITH_ERROR("%d", 0);
}

void QNP::requestFixes()
{
    ENTRY_LOG();

    // reset any previous position /error reports
    mLocationReport.reset();
    mWifiLocReport.reset();
    mWwanLocReport.reset();
    mZppLocReport.reset();
    mWifiLocError.reset();
    mWwanLocError.reset();
    mZppLocError.reset();
    mFixReportedInCurrentSession = false;
    mWaitForWifiFinalFix = false;

    LocationRequest locRequest;
    memset(&locRequest, 0, sizeof(LocationRequest));
    locRequest.action = LocationRequestAction_Start;
    locRequest.intervalInMsec = 0;
    locRequest.emergencyRequest = mInEmergencyMode;

    int64 nowInNanoSec = 0;
    int result = GetTimeSinceBoot(nowInNanoSec);
    if (result == 0) {
        int64 ageOfCollectedDataInNanoSec = nowInNanoSec - mCollectionStartTimestampInNanoSec;
        if (mMinTimeToReportStatsCollectedInSec <= (convertNanoSecsToSecs(ageOfCollectedDataInNanoSec))) {
             LOC_LOGI("metrics.lbsqclbs:data:wifi_fix_inconsistent_with_zpp=%d ;CT;1," \
                      "fix_dropped_too_large_radius=%d ;CT;1," \
                      "dropped_wifi_fix_due_to_single_AP=%d ;CT;1:NR",
                      mCountOfWifiFixInconsistentWithZPP, mCountOfDroppedFixesDueToLargeRadius,
                      mCountOfDroppedSingleApWifiFix);
             mCollectionStartTimestampInNanoSec = nowInNanoSec;
             mCountOfDroppedFixesDueToLargeRadius = 0;
             mCountOfDroppedFixesDueToLargeRadius = 0;
             mCountOfDroppedSingleApWifiFix = 0;
        }
    }

    mRequestId++;
    if (mRequestId > mMaxRequestId) {
        mRequestId = mMinRequestId;
    }

    // Automation -log - Please do not touch
    LOC_LOGD("fireGTPFixSession: Fire a position request to GTP-AP %d", mRequestId);


    // GET a ZPP fix first
    if (mZppProvider != NULL) {
        mZppProvider->setRequest(&locRequest);
        mTrackingState = TRACKING_STATE_WAIT_FOR_BEST_AVAILABLE_FIX;
    } else if (mGtpWwanProvider != NULL) {
        // GTP WWAN AP is present, so no immediate wwan fix
        mTrackingState = TRACKING_STATE_WAIT_FOR_FINAL_WWAN_FIX;
        mGtpWwanProvider->setRequest(&locRequest);
    } else if (mGtpWifiProvider != NULL) {
        mTrackingState = TRACKING_STATE_WAIT_FOR_WIFI_FIX;
        mGtpWifiProvider->setRequest(&locRequest);
    }

    EXIT_LOG_WITH_ERROR("%d", 0);
}

void QNP::stopFixes()
{
    ENTRY_LOG();

    LocationRequest locRequest;
    memset(&locRequest, 0, sizeof(LocationRequest));
    locRequest.action = LocationRequestAction_Stop;
    locRequest.intervalInMsec = -1;

    if (mGtpWwanProvider != NULL) {
        mGtpWwanProvider->setRequest(&locRequest);
    }

    if (mGtpWifiProvider != NULL) {
        mGtpWifiProvider->setRequest(&locRequest);
    }

    if (mZppProvider != NULL) {
        mZppProvider->setRequest(&locRequest);
    }

    EXIT_LOG_WITH_ERROR("%d", 0);
}


bool QNP::zppHasGpsContent()
{
    LOC_LOGD("Source technology mask = %d %d",
        mZppLocReport.mHasSourceTechMask, mZppLocReport.mSourceTechMask);

    if (mZppLocReport.mHasSourceTechMask &&
        (mZppLocReport.mSourceTechMask & LOC_POS_TECH_MASK_SATELLITE) > 0) {
        return true;
    }

    return false;
}

bool QNP::zppHasCPIContent()
{
    LOC_LOGD("Source technology mask = %d %d",
        mZppLocReport.mHasSourceTechMask, mZppLocReport.mSourceTechMask);

    if (mZppLocReport.mHasSourceTechMask &&
        (mZppLocReport.mSourceTechMask &
         LOC_POS_TECH_MASK_INJECTED_COARSE_POSITION) > 0) {
        return true;
    }

    return false;
}

bool QNP::isLocReportedWithinTime(LocationReport& locReport, int timeInSeconds)
{
    ENTRY_LOG();
    bool result = false;

    struct timespec time_info_current;

    do {
        int64 currentTimeInMilliSec;
        BREAK_IF_ZERO(1, locReport.isValid());
        BREAK_IF_ZERO(2, locReport.mHasUtcTimestampInMsec);
        BREAK_IF_NON_ZERO(3, clock_gettime(CLOCK_REALTIME,&time_info_current));
        currentTimeInMilliSec = (time_info_current.tv_sec)*1e3 +
                           (time_info_current.tv_nsec)/1e6;

        int64 ageOfFixInMilliSec = currentTimeInMilliSec - locReport.mUtcTimestampInMsec;
        int ageOfFixInSec = ageOfFixInMilliSec / 1000;
        LOC_LOGD("Age of Location is %d seconds", ageOfFixInSec);
        if (ageOfFixInSec <= timeInSeconds)
            result = true;

    } while (0);

    EXIT_LOG_WITH_ERROR("%d", 0);
    return result;
}

bool QNP::consistencyCheckWifiFix(LocationReport& locReportToCompareWith)
{
    ENTRY_LOG();
    bool result = true;

    // PUNC scaling from from 99% confidence to 68% confidence is (1.07/2.14) = 0.5
    const double ScaleFactorPuncConf = 0.5;

    // grow uncertainity circle of fix based on its accuracy and timestamp first
    increasePUNC(locReportToCompareWith);
    // grow the Zpp circle to accomodate the wifi fix
    float increasedPUNC = getPUNCIncrease(mWifiLocReport);
    float accuracy_99 = increasedPUNC +
        (locReportToCompareWith.mHorizontalAccuracy / ScaleFactorPuncConf);
    float accuracy_68 = accuracy_99 * ScaleFactorPuncConf;

    double distance = getGreatCircleDistanceMeter(&mWifiLocReport, &locReportToCompareWith);

    // Automation -log - Please do not touch
    LOC_LOGD("Distance between ZPP and XTWiFi fixes: %f m, "
               "while accuracy of ZPP fix is %f m (99%%), "
               "accuracy of ZPP fix is %f m (68%%)", distance,
               accuracy_99, accuracy_68);

    // 1. Wifi fix falls into the uncertainity circle of wwan/zpp fix
    // 2. Distance of wifi fix frm zpp/wwan fix is lesser than mMaxZppWifiConsistencyCheckInMeters
    if ((distance < accuracy_99) || (distance < mMaxZppWifiConsistencyCheckInMeters)) {

        // Accuracy of wifi fix must be lesser than WWAN (To account for cases where punc of wifi fix
        // keeps growing and crosses that of wwan). This needs to be checked only for cases where ZPP fix is
        // not being requested for which is if APWWAN is being used and there is no ALE on those targets.
        if ((mApWwanEnabled) &&
            (mWifiLocReport.mHorizontalAccuracy > locReportToCompareWith.mHorizontalAccuracy)) {
            LOC_LOGD("Horizontal Accuracy of WIFI report (%f) is worse than that of "
                "Cell fix (%f), hence dropping the WIFI fix and reporting Cell fix instead",
                mWifiLocReport.mHorizontalAccuracy,
                locReportToCompareWith.mHorizontalAccuracy);
            result = false;
        }
    } else {
        result = false;
    }

    // check if Wifi position being reported was got using 1 AP
    if ((mWifiLocReport.mHasNumberOfApsUsedForFix) &&
        (mWifiLocReport.mNumberOfApsUsedForFix == 1)) {
        // Change the 1 AP Wifi fix with the accuracy of the ZPP/WWAN fix
        if (mWifiLocReport.mHorizontalAccuracy < locReportToCompareWith.mHorizontalAccuracy) {
            mWifiLocReport.mHorizontalAccuracy = locReportToCompareWith.mHorizontalAccuracy;
        }
    }

    if (mWifiLocReport.mHorizontalAccuracy > mMaxAcuracyThresholdToFilterNlpPosition) {
        LOC_LOGD("Dropping final fix from src = %d Accuracy = %f AccuracyThreshold = %d",
            mWifiLocReport.mPositionSource, mWifiLocReport.mHorizontalAccuracy,
            mMaxAcuracyThresholdToFilterNlpPosition);
        mFixReportedInCurrentSession = false;
        mCountOfDroppedFixesDueToLargeRadius += 1;
        result = false;
    }

    if ((true == mInEmergencyMode) && (false == result)) {
        // Report inconsistent wifi fix if in emergency mode
        reportInconsistentGtpWifiFix();
    }

    EXIT_LOG_WITH_ERROR("%d", 0);
    return result;
}

void QNP::reportBestNonGtpWifiFix()
{
    ENTRY_LOG();
    bool reportLocation = false;
    string reportType;

    do {

        if (mZppLocReport.isValid() && !zppHasGpsContent() && !zppHasCPIContent()) {
            increasePUNC(mZppLocReport);
            if (mZppLocReport.mHorizontalAccuracy < mLocationReport.mHorizontalAccuracy || !reportLocation) {
                LOC_LOGD("Best so far is ZPP with accuracy %f meters", mZppLocReport.mHorizontalAccuracy);
                mLocationReport = mZppLocReport;
                reportLocation = true;
                reportType = "ZPP";
            }
        }

        if (!mWwanLocReport.isValid() && mLastSavedWwanLocReport.isValid()) {
            if (shouldApplyPuncGrowthModel(mLastSavedWwanLocReport)) {
                LOC_LOGD("Apply PUNC growth for WWAN fix");
                mWwanLocReport = mLastSavedWwanLocReport;
            }
        }

        if (mWwanLocReport.isValid()) {
            increasePUNC(mWwanLocReport);
            if (mWwanLocReport.mHorizontalAccuracy < mLocationReport.mHorizontalAccuracy || !reportLocation) {
                LOC_LOGD("Best so far is WWAN with accuracy %f meters", mWwanLocReport.mHorizontalAccuracy);
                mLocationReport = mWwanLocReport;
                reportLocation = true;
                reportType = "WWAN";
            }
        }

        if (mLastQnpReport.isValid()) {
            increasePUNC(mLastQnpReport);
            if (mLastQnpReport.mHorizontalAccuracy < mLocationReport.mHorizontalAccuracy || !reportLocation) {
                LOC_LOGD("Best so far is CACHED with accuracy %f meters", mLastQnpReport.mHorizontalAccuracy);
                mLocationReport = mLastQnpReport;
                reportLocation = true;
                reportType = "CACHED";
            }
        }

        if (reportLocation) {
            if (mLocationReport.mHorizontalAccuracy > mMaxAcuracyThresholdToFilterNlpPosition) {
                LOC_LOGD("Dropping final fix from src = %d Accuracy = %f AccuracyThreshold = %d",
                    mLocationReport.mPositionSource, mLocationReport.mHorizontalAccuracy,
                    mMaxAcuracyThresholdToFilterNlpPosition);
                mFixReportedInCurrentSession = false;
                mCountOfDroppedFixesDueToLargeRadius += 1;
                break;
            } else {
                LOC_LOGD ("LocTech-Label :: QNP :: Position Report Out");
                if (mLocationReport.mHasPositionSource) {
                    LOC_LOGD ("LocTech-Value :: Position source: %d", mLocationReport.mPositionSource);
                } else {
                    LOC_LOGE("No position source in QNP position report");
                }

                // Automation -log - Please do not touch
                LOC_LOGD ("Reporting %s fix (%f, %f), unc %f time (ms) %" PRId64,
                          reportType.c_str(), mLocationReport.mLatitude, mLocationReport.mLongitude,
                          mLocationReport.mHorizontalAccuracy,
                          mLocationReport.mUtcTimestampInMsec);

                // cache last reported QNP fix
                mLastQnpReport = mLocationReport;

                // Flagging Internal position source provider
                mLocationReport.mHasPositionSourceProvider = true;
                mLocationReport.mPositionSourceProvider = INTERNAL;

                broadcastLocation(&mLocationReport);
                mFixReportedInCurrentSession = true;
            }
        } else {
            LOC_LOGD("Nothing can be reported");
            break;
        }
    } while(0);

    resetSession();

    EXIT_LOG_WITH_ERROR("%d", 0);
}

void QNP::reportGtpWifiFix()
{
    // Automation - log. Please do not touch.
    LOC_LOGD ("LocTech-Label :: QNP :: WiFi Position Out");

    // Automation -log - Please do not touch
    LOC_LOGD ("Reporting XTWiFi fix (%f, %f), unc %f time (ms) %" PRIu64,
              mWifiLocReport.mLatitude, mWifiLocReport.mLongitude,
              mWifiLocReport.mHorizontalAccuracy, mWifiLocReport.mUtcTimestampInMsec);

    // cache last reported QNP fix
    mLastQnpReport = mWifiLocReport;
    mLastQnpReport.mHasPropagatedFix = true;
    mLastQnpReport.mIsPropagatedFix = true;

    // Flagging Internal position source provider
    mWifiLocReport.mHasPositionSourceProvider = true;
    mWifiLocReport.mPositionSourceProvider = INTERNAL;

    broadcastLocation(&mWifiLocReport);
    mFixReportedInCurrentSession = true;
    resetSession();
}

void QNP::reportInconsistentGtpWifiFix()
{
    LOC_LOGD ("LocTech-Label :: QNP :: Inconsistent WiFi Position Out");

    LOC_LOGD ("Reporting Inconsistent XTWiFi fix (%f, %f), unc %f time (ms) %" PRIu64,
              mWifiLocReport.mLatitude, mWifiLocReport.mLongitude,
              mWifiLocReport.mHorizontalAccuracy, mWifiLocReport.mUtcTimestampInMsec);

    // Set fix as intermediate, so it is not used by apps, but injected into modem only
    mWifiLocReport.mIsIntermediateFix = true;

    // Flagging Internal position source provider
    mWifiLocReport.mHasPositionSourceProvider = true;
    mWifiLocReport.mPositionSourceProvider = INTERNAL;

    broadcastLocation(&mWifiLocReport);
}

void QNP::handleReportFromZpp()
{
    ENTRY_LOG();
    int result = -1;

    LOC_LOGD("QNP Tracking State = %d", mTrackingState);
    do {
        BREAK_IF_ZERO(1,TRACKING_STATE_WAIT_FOR_BEST_AVAILABLE_FIX == mTrackingState);

        if (mZppLocReport.isValid() && (true == mInEmergencyMode)) {
            LOC_LOGD("Emergency mode, so do GTP Wifi/Wwan");
            mTrackingState = TRACKING_STATE_WAIT_FOR_WIFI_FIX;
        } else if (mZppLocReport.isValid() && !zppHasGpsContent() && !zppHasCPIContent()) {
          LOC_LOGD("We have a ZPP fix with no GPS or CPI content");
          if (mZppLocReport.mHorizontalAccuracy <= mQuickFixAccuracyMinInMeters &&
              isLocReportedWithinTime(mZppLocReport, 5)) {
              LOC_LOGD("We have a recent ZPP fix with good accuracy");
              if (mZppLocReport.mHasSourceTechMask &&
                    (mZppLocReport.mSourceTechMask & LOC_POS_TECH_MASK_WIFI) > 0) {
                  LOC_LOGD("Zpp fix has Wifi, so do GTP Wifi/Wwan fix");
                  mTrackingState = TRACKING_STATE_WAIT_FOR_WIFI_FIX;
              } else {
                  LOC_LOGD("Report good fix without ever requesting GTP Wifi");
                  reportBestNonGtpWifiFix();
                  result = 0;
                  break;
              }
          } else {
              LOC_LOGD("ZPP fix is stale or accuracy not good, so do GTP Wifi/Wwan");
              mTrackingState = TRACKING_STATE_WAIT_FOR_WIFI_FIX;
          }
        } else {
            if (mApWwanEnabled) {
                LOC_LOGD("Zpp fix is bad or has GPS or CPI source and AP WWAN enabled, "
                    "so do GTP Wifi/Wwan");
                mTrackingState = TRACKING_STATE_WAIT_FOR_WIFI_FIX;
            } else {
                LOC_LOGD("Zpp fix is bad or has GPS or CPI source, so do GTP Wwan");
                mTrackingState = TRACKING_STATE_WAIT_FOR_IMMEDIATE_WWAN_FIX;
            }
        }

        LocationRequest locRequest;
        memset(&locRequest, 0, sizeof(LocationRequest));
        locRequest.action = LocationRequestAction_Start;
        locRequest.intervalInMsec = 0;
        locRequest.emergencyRequest = mInEmergencyMode;

        if (TRACKING_STATE_WAIT_FOR_IMMEDIATE_WWAN_FIX == mTrackingState) {
            if (mGtpWwanProvider != NULL) {
                mGtpWwanProvider->setRequest(&locRequest);
            } else if (mGtpWifiProvider != NULL) {
                mTrackingState = TRACKING_STATE_WAIT_FOR_WIFI_FIX;
                mGtpWifiProvider->setRequest(&locRequest);
            } else {
                resetSession();
                result = 2;
                break;
            }
        } else if (TRACKING_STATE_WAIT_FOR_WIFI_FIX == mTrackingState) {
            if (mGtpWifiProvider != NULL) {
                mGtpWifiProvider->setRequest(&locRequest);
            }
            if (mGtpWwanProvider != NULL) {
                mGtpWwanProvider->setRequest(&locRequest);
            }
        } else {
            resetSession();
            result = 4;
            break;
        }
        result = 0;
    } while (0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

void QNP::handleReportFromWwan()
{
    ENTRY_LOG();
    int result = -1;
    LOC_LOGD("QNP Tracking State = %d", mTrackingState);

    if (mWwanLocReport.isValid()) {
        mLastSavedWwanLocReport = mWwanLocReport;
        mLastSavedWwanLocReport.mHasPropagatedFix = true;
        mLastSavedWwanLocReport.mIsPropagatedFix = true;
    }

    do {
        LocationRequest locRequest;
        memset(&locRequest, 0, sizeof(LocationRequest));
        locRequest.action = LocationRequestAction_Start;
        locRequest.intervalInMsec = 0;
        locRequest.emergencyRequest = mInEmergencyMode;

        if (TRACKING_STATE_WAIT_FOR_IMMEDIATE_WWAN_FIX == mTrackingState) {
            if (mWwanLocReport.isValid() && (true == mInEmergencyMode)) {
                LOC_LOGD("Emergency mode, so do GTP Wifi/Wwan");
                if (mGtpWifiProvider != NULL) {
                    mTrackingState = TRACKING_STATE_WAIT_FOR_WIFI_FIX;
                    mGtpWifiProvider->setRequest(&locRequest);
                }
            } else if (mWwanLocReport.isValid() &&
                       mWwanLocReport.mHorizontalAccuracy <= mQuickFixAccuracyMinInMeters &&
                       isLocReportedWithinTime(mWwanLocReport, 5)) {
                LOC_LOGD(" Wwan report is good, so no need for GTP Wifi request");
                reportBestNonGtpWifiFix();
            } else {
                LOC_LOGD("No Wwan fix or Wwan fix is not good enough, so do GTP Wifi");
                if (mGtpWifiProvider != NULL) {
                    mTrackingState = TRACKING_STATE_WAIT_FOR_WIFI_FIX;
                    mGtpWifiProvider->setRequest(&locRequest);
                } else {
                    resetSession();
                    result = 1;
                    break;
                }
            }
        } else if (TRACKING_STATE_WAIT_FOR_FINAL_WWAN_FIX == mTrackingState) {
            if (mWifiLocReport.isValid()) {
                if (mWwanLocReport.isValid() &&
                    isLocReportedWithinTime(mWwanLocReport, 5)) {
                    LOC_LOGD("We have a GTP Wifi and Wwan, so do consistency check");
                    if (true == consistencyCheckWifiFix(mWwanLocReport)) {
                        reportGtpWifiFix();
                    } else {
                      if (!mWifiLocReport.mIsPreliminaryFix) {
                        reportBestNonGtpWifiFix();
                      } else {
                        LOC_LOGD("We only have a preliminary GTP Wifi fix, so wait for final fix");
                      }
                    }
                } else {
                    LOC_LOGD("None can be used for GTP Wifi consistency check");
                    if ((mWifiLocReport.mHasNumberOfApsUsedForFix) &&
                        (mWifiLocReport.mNumberOfApsUsedForFix == 1)) {
                        LOC_LOGD("Dropping Wifi fix because cache hit is 1 and \
                                  no ZPP / WWAN fix available for comparison");
                         mCountOfDroppedSingleApWifiFix += 1;
                         if (!mWifiLocReport.mIsPreliminaryFix) {
                           reportBestNonGtpWifiFix();
                         } else {
                           LOC_LOGD("We only have a preliminary GTP Wifi fix, so wait for final fix");
                         }
                    } else {
                        reportGtpWifiFix();
                    }
                }
            } else {
                LOC_LOGD("We don't have GTP Wifi fix that we can use");
                if (!mWifiLocReport.mIsPreliminaryFix) {
                  reportBestNonGtpWifiFix();
                } else {
                  LOC_LOGD("We only have a preliminary GTP Wifi fix, so wait for final fix");
                }
            }
        } else if (TRACKING_STATE_WAIT_FOR_WIFI_FIX == mTrackingState) {
            LOC_LOGD("We are still waiting for a wifi fix, so do nothing");
        } else {
            result = 2;
            break;
        }
        result = 0;
    } while (0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

void QNP::handleReportFromWifi()
{
    ENTRY_LOG();
    int result = -1;
    LOC_LOGD("QNP Tracking State = %d", mTrackingState);

    do {
        BREAK_IF_ZERO(1,TRACKING_STATE_WAIT_FOR_WIFI_FIX == mTrackingState);
        if (mWifiLocReport.isValid()) {
            LOC_LOGD("We have a valid GTP Wifi fix");
            if (mZppLocReport.isValid() && !zppHasCPIContent()
                && isLocReportedWithinTime(mZppLocReport, 30)) {
                LOC_LOGD("ZPP is not CPI and is within 30s, so do consistency check");
                if (true == consistencyCheckWifiFix(mZppLocReport)) {
                    reportGtpWifiFix();
                } else {
                  if (!mWifiLocReport.mIsPreliminaryFix) {
                    reportBestNonGtpWifiFix();
                  } else {
                    LOC_LOGD("Only a GTP Wifi preliminary fix, so wait for final wifi fix");
                  }
                }
            } else if (mLastQnpReport.isValid() && isLocReportedWithinTime(mLastQnpReport, 30)) {
                LOC_LOGD("Last QNP report is within 30s, so do consistency check");
                if (true == consistencyCheckWifiFix(mLastQnpReport)) {
                    reportGtpWifiFix();
                } else {
                  if (!mWifiLocReport.mIsPreliminaryFix) {
                    reportBestNonGtpWifiFix();
                  } else {
                    LOC_LOGD("Only a GTP Wifi preliminary fix, so wait for final wifi fix");
                  }
                }
            } else if (mWwanLocReport.isValid() && isLocReportedWithinTime(mWwanLocReport, 30)) {
                LOC_LOGD("We have a GTP Wifi & Wwan fix is within 30s, so do consistency check");
                if (true == consistencyCheckWifiFix(mWwanLocReport)) {
                    reportGtpWifiFix();
                } else {
                  if (!mWifiLocReport.mIsPreliminaryFix) {
                    reportBestNonGtpWifiFix();
                  } else {
                    LOC_LOGD("Only a GTP Wifi preliminary fix, so wait for final wifi fix");
                  }
                }
            } else if ((mGtpWwanProvider != NULL) && (mApWwanEnabled) &&
                       (!mWwanLocReport.isValid()) && (!mWwanLocError.isValid())) {
                LOC_LOGD("We have GTP Wifi fix but no Wwan, so wait for wwan fix");
                mTrackingState = TRACKING_STATE_WAIT_FOR_FINAL_WWAN_FIX;
            } else if ((mGtpWwanProvider != NULL) && !mWwanLocReport.isValid() &&
                        mWwanLocError.isValid() &&
                        mWwanLocError.mLocationErrorType == LocationError::LocationError_TimeoutImmediateFix) {
                    LOC_LOGD("We have GTP Wifi fix but no Wwan, so wait for final wwan fix");
                    mTrackingState = TRACKING_STATE_WAIT_FOR_FINAL_WWAN_FIX;
            } else {
                LOC_LOGD("None can be used for GTP Wifi consistency check");
                if ((mWifiLocReport.mHasNumberOfApsUsedForFix) &&
                    (mWifiLocReport.mNumberOfApsUsedForFix == 1)) {
                    LOC_LOGD("Dropping Wifi fix because cache hit is 1 and \
                              no ZPP / WWAN fix available for comparison");
                     mCountOfDroppedSingleApWifiFix += 1;
                    if (!mWifiLocReport.mIsPreliminaryFix) {
                      reportBestNonGtpWifiFix();
                    } else {
                      LOC_LOGD("Only a GTP Wifi preliminary fix, so wait for final wifi fix");
                    }
                } else {
                    reportGtpWifiFix();
                }
            }
        } else if ((mGtpWwanProvider == NULL) ||
                 (mWwanLocReport.isValid()) ||
                 (mWwanLocError.isValid() &&
                 (mWwanLocError.mLocationErrorType == LocationError::LocationError_Timeout) ||
                 (mWwanLocError.mLocationErrorType == LocationError::LocationError_NoPosition))) {
            LOC_LOGD("GTP Wifi failed and we have final Wwan report, so report best fix we have");
            reportBestNonGtpWifiFix();
        } else {
            LOC_LOGD("GTP Wifi failed and we don't have Wwan report yet, so wait for it");
                mTrackingState = TRACKING_STATE_WAIT_FOR_FINAL_WWAN_FIX;
        }
        result = 0;
    } while (0);


    EXIT_LOG_WITH_ERROR("%d", result);
}

double QNP::getGreatCircleDistanceMeter(const LocationReport *loc1, const LocationReport *loc2)
{
    const double CONVERT_TO_RADIANS  = 3.14 / 180;
    const double earth_radius_meter = 6367435.68;
    double lat1, lon1, lat2, lon2;

    // calculation taken from http://www.movable-type.co.uk/scripts/latlong.html
    lat1 = loc1->mLatitude * CONVERT_TO_RADIANS;
    lat2 = loc2->mLatitude * CONVERT_TO_RADIANS;
    lon1 = loc1->mLongitude * CONVERT_TO_RADIANS;
    lon2 = loc2->mLongitude * CONVERT_TO_RADIANS;

    double lat_diff = lat1 - lat2;
    double lon_diff = lon1 - lon2;
    double sin_half_lat_diff = sin(lat_diff / 2);
    double sin_half_lon_diff = sin(lon_diff / 2);
    double h = sin_half_lat_diff * sin_half_lat_diff
               + sin_half_lon_diff * sin_half_lon_diff * cos(lat1) * cos(lat2);
    double m = sqrt(h);
    double m1 = sqrt(1 - h);

    double c = 2 * atan2(m, m1);
    LOC_LOGD("lat1 = %f, lat2 =%f, long1 = %f, long2 = %f, lat_diff = %f "
                  "long_diff = %f, sin_half_lat_diff = %f "
                  "sin_half_long_diff = %f, h = %f, m = %f, m1 = %f, c = %f",
                  lat1, lat2, lon1, lon2, lat_diff, lon_diff, sin_half_lat_diff,
                  sin_half_lon_diff, h, m, m1, c);
    return earth_radius_meter * c;
}

void QNP::increasePUNC(LocationReport & locReport)
{
    int result = -1;

    // Assuming an increase in punc by travelling at 56 miles / hour
    const double puncIncreaseMetersPerSecond = 25;

    // PUNC scaling from from 99% confidence to 68% confidence is (1.07/2.14) = 0.5
    const double ScaleFactorPuncConf = 0.5;
    struct timespec time_info_current;

    do {
        int64 currentTimeInMilliSec;
        BREAK_IF_ZERO(1, locReport.isValid());
        BREAK_IF_ZERO(2, locReport.mHasUtcTimestampInMsec);
        BREAK_IF_NON_ZERO(3, clock_gettime(CLOCK_REALTIME,&time_info_current));
        currentTimeInMilliSec = (time_info_current.tv_sec)*1e3 +
                           (time_info_current.tv_nsec)/1e6;

        int64 ageOfFixInMilliSec = currentTimeInMilliSec - locReport.mUtcTimestampInMsec;
        int ageOfFixInSec = ageOfFixInMilliSec / 1000;

        LOC_LOGD("currentTime = %" PRId64 ", UtcTimeInMsec = %" PRId64,
            currentTimeInMilliSec, locReport.mUtcTimestampInMsec);

        if (ageOfFixInSec > 0) {
            float puncIncreaseInMeters = ageOfFixInSec * puncIncreaseMetersPerSecond;
            LOC_LOGD("Increase punc by %f for %d secs", puncIncreaseInMeters, ageOfFixInSec);

            float accuracy_99 = puncIncreaseInMeters +
                (locReport.mHorizontalAccuracy / ScaleFactorPuncConf);
            float accuracy_68 = accuracy_99 * ScaleFactorPuncConf;

            LOC_LOGD("Previous punc (68%%) is %f, adjusted for punc growth to %f (68%%)",
                locReport.mHorizontalAccuracy, accuracy_68);

            locReport.mHorizontalAccuracy = accuracy_68;
            locReport.mUtcTimestampInMsec = currentTimeInMilliSec;
        }

        result = 0;
    } while (0);

    EXIT_LOG_WITH_ERROR("%d", result);
}



float QNP::getPUNCIncrease(const LocationReport & locReport)
{
    int result = -1;
    float puncIncreaseInMeters = 0;

    // Assuming an increase in punc by travelling at 56 miles / hour
    const double puncIncreaseMetersPerSecond = 25;
    struct timespec time_info_current;

    do {
        int64 currentTimeInMilliSec;
        BREAK_IF_ZERO(1, locReport.isValid());
        BREAK_IF_ZERO(2, locReport.mHasUtcTimestampInMsec);
        BREAK_IF_NON_ZERO(3, clock_gettime(CLOCK_REALTIME,&time_info_current));
        currentTimeInMilliSec = (time_info_current.tv_sec)*1e3 +
                           (time_info_current.tv_nsec)/1e6;

        int64 ageOfFixInMilliSec = currentTimeInMilliSec - locReport.mUtcTimestampInMsec;
        int ageOfFixInSec = ageOfFixInMilliSec / 1000;

        LOC_LOGD("currentTime = %" PRId64 " UtcTimeInMsec = %" PRId64,
            currentTimeInMilliSec, locReport.mUtcTimestampInMsec);

        if (ageOfFixInSec > 0) {
            puncIncreaseInMeters = ageOfFixInSec * puncIncreaseMetersPerSecond;
            LOC_LOGD("Increase punc by %f for %d secs", puncIncreaseInMeters, ageOfFixInSec);
        }

        result = 0;
    } while (0);

    EXIT_LOG_WITH_ERROR("%d", result);
    return puncIncreaseInMeters;
}


bool QNP::shouldApplyPuncGrowthModel(LocationReport & savedLocReport)
{
    int result = -1;
    bool applyIncreasePUNC = false;

    do {
        // if AP Cell is enabled then most probably there is no ALE/ZPP, hence we need to propagate WIFI
        // and WWAN fixes to replicate the behavior. If Modem Cell is enabled then ZPP fixes are also available
        // and there is no need for QNP to do saved fix propagation.
        BREAK_IF_ZERO(0, mApWwanEnabled);

        BREAK_IF_ZERO(0, mTimeForSavedPositionToBeValidInMinutes);
        BREAK_IF_ZERO(0, savedLocReport.mHasElapsedRealTimeInNanoSecs);

        int64 puncInvalidateTimeNanoSec =
            convertSecsToNanoSecs(mTimeForSavedPositionToBeValidInMinutes * 60);

        int64 nowInNanoSec = 0;
        BREAK_IF_NON_ZERO(2, GetTimeSinceBoot(nowInNanoSec));


        LOC_LOGD("Compare with saved postion now = %" PRId64
                 " mHasElapsedRealTimeInNanoSecs = %d mElapsedRealTimeInNanoSecs = %" PRId64
                 " Invalidate time = %" PRId64,
                     nowInNanoSec, savedLocReport.mHasElapsedRealTimeInNanoSecs,
                     savedLocReport.mElapsedRealTimeInNanoSecs,
                     puncInvalidateTimeNanoSec);
        if ((nowInNanoSec < savedLocReport.mElapsedRealTimeInNanoSecs) ||
            ((nowInNanoSec - savedLocReport.mElapsedRealTimeInNanoSecs) > puncInvalidateTimeNanoSec)) {
            savedLocReport.reset();
        } else {
            applyIncreasePUNC = true;
        }

       result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
    return applyIncreasePUNC;
}
}
