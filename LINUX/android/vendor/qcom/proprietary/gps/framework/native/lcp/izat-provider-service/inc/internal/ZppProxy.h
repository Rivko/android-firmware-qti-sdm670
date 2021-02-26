/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
GENERAL DESCRIPTION
    ZppProxy assists in getting zero-power position from modem

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Confidential and Proprietary.
=============================================================================*/


#ifndef __IZAT_MANAGER_ZPPPROXY_H__
#define __IZAT_MANAGER_ZPPPROXY_H__

#include "LocationProvider.h"
#include <mq_client/IPCMessagingProxy.h>
#include "LBSAdapter.h"
#include "LocTimer.h"

namespace izat_manager
{

class ZppMeasurementReport;

class ZppProxy : public LocationProvider {
public:
    static ILocationProvider* getInstance(const struct s_IzatContext* izatContext);
    static int destroyInstance();

    // ILocationProvider overrides
    int setRequest(const LocationRequest *request);
    void enable();
    void disable();

    // Static method to be called when the ZppFixMsg comes in from the LBSAdapter
    static void handleZppFix(const LocGpsLocation *gpsLocation,
            const GpsLocationExtended *locationExtended,
            LocPosTechMask posTechMask);

    class ZppMeasurementReport : public LocationReport {
    public:
        bool  mHasSourceTechMask;
        LocPosTechMask mSourceTechMask;

        ZppMeasurementReport()
        {
            resetInternal();
        }

        ZppMeasurementReport(const ZppMeasurementReport & zppLocReport):
            LocationReport(zppLocReport)
        {
            makeCopy(zppLocReport);
        }

        ZppMeasurementReport & operator=(const ZppMeasurementReport & zppLocReport)
        {
            LocationReport::operator=(zppLocReport);
            makeCopy(zppLocReport);
            return *this;
        }
        void reset()
        {
            LocationReport::reset();
            resetInternal();
        }
        virtual ~ZppMeasurementReport()
        {
            resetInternal();
        }

    protected:

    private:
       void makeCopy(const ZppMeasurementReport & zppLocReport)
        {
            mHasSourceTechMask = zppLocReport.mHasSourceTechMask;
            mSourceTechMask = zppLocReport.mSourceTechMask;
        }

        void resetInternal()
        {
            mHasSourceTechMask = false;
        }
    };

protected:

private:
    typedef enum {
        ZPP_PROVIDER_STATE_DISABLED,
        ZPP_PROVIDER_STATE_IDLE,
        ZPP_PROVIDER_STATE_WAIT_FOR_LOCATION
    } ZppProviderState;

    class Timer : public LocTimer {
        ZppProxy* mClient;
    public:
        inline Timer(ZppProxy* client) :
            LocTimer(), mClient(client) {}
        virtual void timeOutCallback();
    };

    ZppProxy(const struct s_IzatContext *izatContext);
    inline ~ZppProxy() {}

    static ILocationProvider *mZppProxy;

    ZppMeasurementReport mLocationReport;
    LocationError mLocationError;
    bool mInitialized;
    ZppProviderState mProviderState;
    LBSAdapter *mLBSAdapter;

    Timer mFixTimeoutTimer;
    friend class Timer;

    // Maximum timeout to get zpp fix
    unsigned int mMaxTimeForZppFix;

    void readConfig();

    struct positionMsgFromModem : public LocMsg {
        ZppProxy *mZppProxy;
        LocGpsLocation mGpsLocation;
        LocPosTechMask mSourceTechMask;
        GpsLocationExtended mGpsLocationExtended;

        inline positionMsgFromModem(ZppProxy *proxy,
            const LocGpsLocation *gpsLocation, const GpsLocationExtended *locationExtended,
                    LocPosTechMask sourceTechMask) :
            mZppProxy(proxy), mSourceTechMask(sourceTechMask) {
                memcpy(&mGpsLocation, gpsLocation, sizeof(LocGpsLocation));
                memcpy(&mGpsLocationExtended, locationExtended, sizeof(GpsLocationExtended));
    }

        virtual void proc() const;
        inline ~positionMsgFromModem() {}
    };

    struct timeoutMsgZpp : public LocMsg {
        ZppProxy* mZppProxy;

        inline timeoutMsgZpp(ZppProxy *pZppProxy) : mZppProxy(pZppProxy) {
        }

        virtual void proc() const;
        inline ~timeoutMsgZpp() {}
    };

    struct requestZppLocationMsg : public LocMsg {
        ZppProxy* mZppProxy;
        LocationRequest mLocationRequest;

        requestZppLocationMsg(ZppProxy *pZppProxy, const LocationRequest* locReq) :
            mZppProxy(pZppProxy) {
            memcpy(&mLocationRequest, locReq, sizeof(LocationRequest));
        }
        virtual void proc() const;
        inline ~requestZppLocationMsg() {}
    };

    struct disableZppMsg : public LocMsg {
        ZppProxy* mZppProxy;

        disableZppMsg(ZppProxy *pZppProxy) : mZppProxy(pZppProxy) {}
        virtual void proc() const;
        inline ~disableZppMsg() {}
    };

    struct enableZppMsg : public LocMsg {
        ZppProxy* mZppProxy;

        enableZppMsg(ZppProxy *pZppProxy) : mZppProxy(pZppProxy) {}
        virtual void proc() const;
        inline ~enableZppMsg() {}
    };
};


}// namespace izat_manager

#endif // #ifndef __IZAT_MANAGER_ZPPPROXY_H__
