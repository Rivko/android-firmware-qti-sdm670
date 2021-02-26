/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
GENERAL DESCRIPTION
    GTPWwanProxy assists in Cell-id position.

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Confidential and Proprietary.
=============================================================================*/


#ifndef __IZAT_MANAGER_GTPWWANPROXY_H__
#define __IZAT_MANAGER_GTPWWANPROXY_H__

#include "LocationProvider.h"
#include <mq_client/IPCMessagingProxy.h>
#include "LBSAdapter.h"
#include "LocTimer.h"

namespace izat_manager
{

class WwanMeasurementReport;

class GtpWWanProxy : public LocationProvider {
public:
    static ILocationProvider* getInstance(const struct s_IzatContext* izatContext);
    static int destroyInstance();

    // ILocationProvider overrides
    int setRequest(const LocationRequest *request);
    void enable();
    void disable();

    // IMQServerProxyResponse Overrides
    virtual void handleMsg(InPostcard *const in_card);

    // Static method to be called when the WwanFixMsg comes in from the LBSAdapter
    static void handleModemWwanFix(const LocGpsLocation *gpsLocation);

protected:

private:
    typedef enum {
        WWAN_PROVIDER_STATE_DISABLED,
        WWAN_PROVIDER_STATE_IDLE,
        WWAN_PROVIDER_STATE_WAIT_FOR_QUICK_LOCATION,
        WWAN_PROVIDER_STATE_WAIT_FOR_LOCATION
    } WwanProviderState;

    class Timer : public LocTimer {
        GtpWWanProxy* mClient;
    public:
        inline Timer(GtpWWanProxy* client) :
            LocTimer(), mClient(client) {}
        virtual void timeOutCallback();
    };
    friend class Timer;

    GtpWWanProxy(const struct s_IzatContext *izatContext,
        bool apWwanServiceEnabled, bool modemWwanServiceEnabled,
        unsigned int quickFixWaitTimeMaxInMsec);
    inline ~GtpWWanProxy() {}

    const std::string mProviderTag;
    static ILocationProvider *mGtpWwanProxy;

    LocationReport mLocationReport;
    LocationError mLocationError;
    bool mInitialized;
    bool mAPWwanServiceEnabled;
    bool mModemWwanServiceEnabled;
    int mTxId;
    WwanProviderState mProviderState;
    LBSAdapter *mLBSAdapter;

    Timer mFixTimeoutTimer;

    // Maximum timeout to get modem wwan fix
    unsigned int mMaxTimeForModemWwanFix;

    // Maximum timeout to get ap wwan fix
    unsigned int mMaxTimeForApWWanFix;

    unsigned int mQuickFixWaitTimeMaxInMsec;

    void readConfig();

    struct positionMsgApWwan : public LocMsg {
        GtpWWanProxy *mWwanProxy;
        InPostcard *mIncard;

        inline positionMsgApWwan(GtpWWanProxy *proxy, InPostcard *in_card) :
            mWwanProxy(proxy), mIncard(in_card) {
        }

        ~positionMsgApWwan();
        virtual void proc() const;
    };

    struct positionMsgFromModem : public LocMsg {
        GtpWWanProxy *mWwanProxy;
        LocGpsLocation mGpsLocation;

        inline positionMsgFromModem(GtpWWanProxy *proxy,
            const LocGpsLocation *gpsLocation) : mWwanProxy(proxy) {
                memcpy(&mGpsLocation, gpsLocation, sizeof(LocGpsLocation));
    }

        virtual void proc() const;
        inline ~positionMsgFromModem() {}
    };

    struct timeoutMsgWwan : public LocMsg {
        GtpWWanProxy* mWwanProxy;

        inline timeoutMsgWwan(GtpWWanProxy *pWwanProxy) : mWwanProxy(pWwanProxy) {
        }

        virtual void proc() const;
        inline ~timeoutMsgWwan() {}
    };

    struct requestWwanLocationMsg : public LocMsg {
        GtpWWanProxy* mWwanProxy;
        LocationRequest mLocationRequest;

        requestWwanLocationMsg(GtpWWanProxy *pGtpWwanProxy, const LocationRequest* locReq) :
            mWwanProxy(pGtpWwanProxy) {
            memcpy(&mLocationRequest, locReq, sizeof(LocationRequest));
        }
        virtual void proc() const;
        inline ~requestWwanLocationMsg() {}
    };

    struct disableWwanMsg : public LocMsg {
        GtpWWanProxy* mWwanProxy;

        disableWwanMsg(GtpWWanProxy *pGtpWwanProxy) : mWwanProxy(pGtpWwanProxy) {}
        virtual void proc() const;
        inline ~disableWwanMsg() {}
    };

    struct enableWwanMsg : public LocMsg {
        GtpWWanProxy* mWwanProxy;

        enableWwanMsg(GtpWWanProxy *pGtpWwanProxy) : mWwanProxy(pGtpWwanProxy) {}
        virtual void proc() const;
        inline ~enableWwanMsg() {}
    };
};


}// namespace izat_manager

#endif // #ifndef __IZAT_MANAGER_GTPWWANPROXY_H__
