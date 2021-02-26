/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
GENERAL DESCRIPTION
  Qualcomm Network Provider is a proprietary network location provider.

  Copyright (c) 2015 - 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Confidential and Proprietary.
=============================================================================*/


#ifndef __IZAT_MANAGER_QNP_H__
#define __IZAT_MANAGER_QNP_H__

#include "LocTimer.h"
#include "LocationProvider.h"
#include "GTPWWanProxy.h"
#include "GTPWiFiProxy.h"
#include "ZppProxy.h"

namespace izat_manager
{

class QNP : public LocationProvider {

public:

    static ILocationProvider* getInstance(const struct s_IzatContext* izatContext);
    static int destroyInstance();

    // ILocationProvider overrides
    int setRequest(const LocationRequest *request);
    void enable();
    void disable();

    // ILocationResponse overrides
    void reportLocation(const LocationReport *locReport, const ILocationProvider *providerSrc = NULL);
    void reportError(const LocationError *locError, const ILocationProvider *providerSrc = NULL);

    // invoked by combo provider
    bool isFixSupportedWhenEULAOff ();

private:
    typedef enum {
        TRACKING_STATE_DISABLED,
        TRACKING_STATE_IDLE,
        TRACKING_STATE_TBF,
        TRACKING_STATE_WAIT_FOR_BEST_AVAILABLE_FIX,
        TRACKING_STATE_WAIT_FOR_IMMEDIATE_WWAN_FIX,
        TRACKING_STATE_WAIT_FOR_FINAL_WWAN_FIX,
        TRACKING_STATE_WAIT_FOR_WIFI_FIX
    } QNPTrackingState;

    class Timer : public LocTimer {
        QNP* mClient;
    public:
        inline Timer(QNP* client) :
            LocTimer(), mClient(client) {}
        virtual void timeOutCallback();
    };
    friend class Timer;

    static ILocationProvider* mQNP;

    const std::string mNLPConfFile;
    const std::string mIzatConfFile;
    ILocationProvider* mGtpWifiProvider;
    ILocationProvider* mGtpWwanProvider;
    ILocationProvider* mZppProvider;
    QNPTrackingState mTrackingState;
    uint32_t mCurrentTimeBetweenFixInMsec;
    int64 mTimestampLastTrackingRequestInNanoSec;
    Timer mTBFTimer;
    bool mFixReportedInCurrentSession;
    bool mSingleShotFixRequested;
    bool mInEmergencyMode;
    bool mApWwanEnabled;
    bool mWaitForWifiFinalFix;

    LocationReport mWwanLocReport;
    LocationReport mLastSavedWwanLocReport;
    ZppProxy::ZppMeasurementReport mZppLocReport;
    GtpWifiProxy::WifiMeasurementReport mWifiLocReport;
    LocationError mWifiLocError;
    LocationError mWwanLocError;
    LocationError mZppLocError;
    LocationReport mLocationReport;
    LocationReport mLastQnpReport;

    unsigned int mRequestId;
    const unsigned int mMinRequestId = 1;
    const unsigned int mMaxRequestId = 1000000;

    // Minimin time between tracking fix requests (msec)
    uint32_t mMinTimeBeweenTrackingFixInMsec;

    // Mac accuracy threshold to report network location fix
    unsigned int mMaxAcuracyThresholdToFilterNlpPosition;

    // Max allowed Zpp HEPE to be reported without consolidation with wifi fix
    unsigned int mMaxZppReportableHepe;

    // Max allowed distance between zpp and wifi fix, if wifi fix does not fall within the uncertainity circle of Zpp
    unsigned int mMaxZppWifiConsistencyCheckInMeters;

    // Maximum allowed time during which a last saved position can be propagated, till made invalid
    int64 mTimeForSavedPositionToBeValidInMinutes;

    int mGtpApMode;

    unsigned int mQuickFixAccuracyMinInMeters;

    // Statistic data
    unsigned int mCountOfWifiFixInconsistentWithZPP;
    unsigned int mCountOfDroppedFixesDueToLargeRadius;
    unsigned int mCountOfDroppedSingleApWifiFix;
    int64 mCollectionStartTimestampInNanoSec;
    int mMinTimeToReportStatsCollectedInSec;

    QNP(const struct s_IzatContext *izatContext,
        ILocationProvider *pGtpWifiProvider, ILocationProvider* pGtpWwanProvider,
        ILocationProvider *pZppProvider);
    ~QNP();

    void startTrackingSession();
    void stopTrackingSession();
    void setInterval(int interval_ms);
    void resetSession();
    void resetTBFTimer();
    void requestFixes();
    void stopFixes();
    void readConfig();
    void handleReportFromWwan();
    void handleReportFromZpp();
    void handleReportFromWifi();
    void consolidateAllAndWaitForNextState();
    void compareAndReportWifiFix();
    bool zppHasGpsContent();
    bool zppHasCPIContent();
    bool isLocReportedWithinTime(LocationReport& locReport, int timeInSeconds);
    bool consistencyCheckWifiFix(LocationReport& locReportToCompareWith);
    void reportBestNonGtpWifiFix();
    void reportGtpWifiFix();
    void reportInconsistentGtpWifiFix();

    float getPUNCIncrease(const LocationReport & locReport);
    void increasePUNC(LocationReport & locReport);
    double getGreatCircleDistanceMeter(const LocationReport *loc1, const LocationReport *loc2);
    bool shouldApplyPuncGrowthModel(LocationReport & savedlocReport);

    struct requestLocationMsg : public LocMsg {
        QNP* mQNP;
        LocationRequest mLocRequest;

        requestLocationMsg(QNP *pQNP, const LocationRequest *request) :
            mQNP(pQNP) {
            memcpy(&mLocRequest, request, sizeof(LocationRequest));
        }
        virtual void proc() const;
    };

    struct setIntervalMsg : public LocMsg {
        const int EMERGENCY_FREQ_PERIOD = 2000; // 2 seconds
        QNP* mQNP;
        uint32_t mIntervalInMsec;
        uint32_t mMinAllowedInterval;

        inline setIntervalMsg(QNP *pQNP, uint32_t interval_ms, bool inEmergency) :
            mQNP(pQNP), mIntervalInMsec(interval_ms) {

            if (inEmergency) {
                mMinAllowedInterval = EMERGENCY_FREQ_PERIOD;
            } else {
                mMinAllowedInterval = pQNP->mMinTimeBeweenTrackingFixInMsec;
            }
        }

        virtual void proc() const;
    };

    struct tbfTimerMsg : public LocMsg {
        QNP* mQNP;

        inline tbfTimerMsg(QNP *pQNP) : mQNP(pQNP) {}
        virtual void proc() const;
        inline ~tbfTimerMsg() {}
    };

    struct disableQNPMsg : public LocMsg {
        QNP* mQNP;

        disableQNPMsg(QNP *pQNP) : mQNP(pQNP) {}
        virtual void proc() const;
        inline ~disableQNPMsg() {}
    };

    struct enableQNPMsg : public LocMsg {
        QNP* mQNP;

        enableQNPMsg(QNP *pQNP) : mQNP(pQNP) {}
        virtual void proc() const;
        inline ~enableQNPMsg() {}
    };
};
}// namespace izat_manager

#endif // #ifndef __IZAT_MANAGER_QNP_H__
