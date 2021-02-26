/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  GTPWifiProxy assists in getting WiFi AP based postions.

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Confidential and Proprietary.
=============================================================================*/


#ifndef __IZAT_MANAGER_GTPWIFIPROXY_H__
#define __IZAT_MANAGER_GTPWIFIPROXY_H__

#include "LocTimer.h"
#include "LocationProvider.h"
#include <mq_client/IPCMessagingProxy.h>
#include "LBSAdapter.h"
#include "IFreeWifiScanObserver.h"
#include "FreeWifiScanObserver.h"

namespace izat_manager
{

using namespace qc_loc_fw;

class WifiMeasurementReport;

class GtpWifiProxy : public LocationProvider {

public:

    static ILocationProvider* getInstance(const struct s_IzatContext* izatContext);
    static int destroyInstance();

    // ILocationProvider overrides
    int setRequest(const LocationRequest *request);
    void enable();
    void disable();


    // IMQServerProxyResponse Overrides
    virtual void handleMsg(InPostcard *const in_card);

    class WifiMeasurementReport : public LocationReport {
    public:
        bool mHasNumberOfApsUsedForFix;
        int mNumberOfApsUsedForFix;
        bool mIsPreliminaryFix;

        WifiMeasurementReport()
        {
            resetInternal();
        }

        WifiMeasurementReport(const WifiMeasurementReport & wifiLocReport):
            LocationReport(wifiLocReport)
        {
            makeCopy(wifiLocReport);
        }

        WifiMeasurementReport & operator=(const WifiMeasurementReport & wifiLocReport)
        {
            LocationReport::operator=(wifiLocReport);
            makeCopy(wifiLocReport);
            return *this;
        }
        void reset()
        {
            LocationReport::reset();
            resetInternal();
        }
        virtual ~WifiMeasurementReport()
        {
            resetInternal();
        }

    protected:

    private:
        void makeCopy(const WifiMeasurementReport & wWifiLocReport)
        {
            mHasNumberOfApsUsedForFix = wWifiLocReport.mHasNumberOfApsUsedForFix;
            mNumberOfApsUsedForFix = wWifiLocReport.mNumberOfApsUsedForFix;
            mIsPreliminaryFix = wWifiLocReport.mIsPreliminaryFix;
        }

        void resetInternal()
        {
            mHasNumberOfApsUsedForFix = false;
            mIsPreliminaryFix = false;
        }
    };

protected:

private:
    typedef enum {
        WIFI_PROVIDER_STATE_DISABLED,
        WIFI_PROVIDER_STATE_IDLE,
        WIFI_PROVIDER_STATE_WAIT_FOR_LOCATION,
        WIFI_PROVIDER_STATE_WAIT_FOR_FREE_FIX
    } WifiProviderState;

    class Timer : public LocTimer {
        GtpWifiProxy* mClient;
    public:
        inline Timer(GtpWifiProxy* client) :
            LocTimer(), mClient(client) {}
        virtual void timeOutCallback();
    };
    friend class Timer;

    const std::string mProviderTag;
    static ILocationProvider* mGtpWifiProxy;

    WifiMeasurementReport mLocationReport;
    LocationError mLocationError;
    bool mInitialized;
    int mTxId;
    LBSAdapter *mLBSAdapter;
    WifiProviderState mProviderState;

    Timer mFixTimeoutTimer;

    bool mIsNlpRequestPending;

    // Config item to enable processing of free wifi scanning
    int mEnableProcessFreeWiFiScan;

    // Maximum timeout to get final wifi based position
    unsigned int mMaxTimeForWiFiFixInMsec;

    // Maximim timeout to get wifi based position based on free wifi scan
    unsigned int mMaxTimeForWiFiFixFromFreeWifiScanInMsec;

    // Minimum time between fix requests on free wifi scan notifications
    unsigned int mMinTimeBetweenFixfOnFreeWiFiScanInMsec;

    // Timestamp of last tracking request
    int64 mTimestampLastTrackingRequestInNanoSec;

    // Timestamp of last free wifi scan based request
    int64 mTimestampLastFreeFixRequestInNanoSec;


    GtpWifiProxy(const struct s_IzatContext* izatContext);
    inline ~GtpWifiProxy() {}

    void readConfig();
    void cacheFixAndReport(double latitude, double longitude, float accuracy,
        int numOfApsUsedForFix, bool isPreliminaryFix);
    void injectWifiLocation(int numOfApUsedForFix, double lat_deg, double lon_deg,
        float accuracy, InPostcard *incard);
    void reportLocationError(LocationError::LocationErrorType errorType);
    void resetTimer();
    void triggerPendingNlpRequest();
    bool canMakeFreeFixRequest();

    // LocMsg classes
    struct requestWifiLocationMsg : public LocMsg {
        LocationRequest mLocationRequest;
        GtpWifiProxy* mWifiProxy;
        bool mNeedFixFromCachedScan;

        requestWifiLocationMsg(GtpWifiProxy *pGtpWifiProxy,
            const LocationRequest* locReq, bool need_fix_from_cached_scan) :
             mWifiProxy(pGtpWifiProxy), mNeedFixFromCachedScan(need_fix_from_cached_scan) {
            memcpy(&mLocationRequest, locReq, sizeof(LocationRequest));
        }
        inline ~requestWifiLocationMsg() {}
        virtual void proc() const;
    };

    struct timeoutMsgWifi : public LocMsg {
        GtpWifiProxy* mWifiProxy;

        inline timeoutMsgWifi(GtpWifiProxy *pWifiProxy) : mWifiProxy(pWifiProxy) {
        }
        inline ~timeoutMsgWifi() {}

        virtual void proc() const;
    };

    struct positionMsgWifi : public LocMsg {
        InPostcard *mIncard;
        GtpWifiProxy *mWifiProxy;

        inline positionMsgWifi(GtpWifiProxy *proxy, InPostcard *in_card) :
             mIncard(in_card), mWifiProxy(proxy){
        }
        ~positionMsgWifi();

        virtual void proc() const;
    };

    struct disableWifiMsg : public LocMsg {
        GtpWifiProxy* mWifiProxy;

        disableWifiMsg(GtpWifiProxy *pGtpWifiProxy) : mWifiProxy(pGtpWifiProxy) {}
        virtual void proc() const;
        inline ~disableWifiMsg() {}
    };

    struct enableWifiMsg : public LocMsg {
        GtpWifiProxy* mWifiProxy;

        enableWifiMsg(GtpWifiProxy *pGtpWifiProxy) : mWifiProxy(pGtpWifiProxy) {}
        virtual void proc() const;
        inline ~enableWifiMsg() {}
    };
};



}// namespace izat_manager

#endif // #ifndef __IZAT_MANAGER_GTPWIFIPROXY_H__
