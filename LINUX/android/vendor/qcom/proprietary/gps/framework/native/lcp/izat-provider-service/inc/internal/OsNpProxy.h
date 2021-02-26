/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  OS Network Provider Proxy module

  Copyright  (c) 2015 - 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Confidential and Proprietary.
=============================================================================*/

#ifndef __IZAT_MANAGER_OSNPPROXY_H__
#define __IZAT_MANAGER_OSNPPROXY_H__

#include "MsgTask.h"
#include "IIzatListener.h"
#include "ILocationProvider.h"
#include "IzatContext.h"
#include "LocationReport.h"
#include "WiperData.h"

namespace izat_manager {
class IzatLocation;
class ILocationResponse;
class IOSLocationProvider;
class IzatRequest;

/**
 * OsNpProxy may receive calls from different threads, hence methods may
 * be re-entrant
 * It does not support MULTI CLIENT. Every client must create its own instance
 */
class OsNpProxy :
public IIzatListener,
public ILocationProvider {

public:
    static OsNpProxy* createInstance(const struct s_IzatContext* ctx);
    virtual ~OsNpProxy();

    // IIzatListener overrides
    virtual void onLocationChanged(const IzatLocation* location, const IzatLocationStatus status);
    virtual void onStatusChanged(IzatProviderStatus status);

    // ILocationProvider overrides
    virtual int setRequest(const LocationRequest* request);
    /**
     * OsNpProxy is MONO client, hence last respObject provided will be
     * used to report locations
     */
    virtual void subscribe(const ILocationResponse* respObject);
    /**
     * unsubscribe() does not check if provided passObject matches the object
     * provided in subscribe() call
     */
    virtual void unsubscribe(const ILocationResponse* respObject);
    virtual void enable();
    virtual void disable();
    inline void onNmeaChanged(const IzatNmea* /*nmea*/) {}

private:
    // Internal methods
    OsNpProxy(const struct s_IzatContext* ctx, IOSLocationProvider *pOSNPProvider);

    // Data members
    const s_IzatContext *mIzatContext;
    ILocationResponse* mClient;
    IOSLocationProvider *mOSNPProvider;
    bool mEnabled;

    // Async messages
    struct onLocationChangedMsg : public LocMsg {
        OsNpProxy* mOsNpProxy;
        LocationReport mLocation;

        onLocationChangedMsg(OsNpProxy* pOsNpProxy, const IzatLocation* loc)
        : mOsNpProxy(pOsNpProxy), mLocation(*loc)
        {
          mLocation.mHasProcessorSource = true;
          mLocation.mProcessorSource = LocationReport::ProcessorSrc_AP;

          if ((mLocation.mHasAltitudeWrtMeanSeaLevel ||
                  mLocation.mHasAltitudeWrtEllipsoid) &&
                  mLocation.mHasVertUnc) {
              mLocation.mHasVerticalReliability = true;
              mLocation.mVerticalReliability = RELIABILITY_MEDIUM;

              mLocation.mHasVerticalConfidence = true;
              mLocation.mVerticalConfidence = 68;
          }

          // Flagging External position source provider
          mLocation.mHasPositionSourceProvider = true;
          mLocation.mPositionSourceProvider = EXTERNAL;

          // convert the network position source received from framework to internal LocationReport
          // positionsource field.
          if (loc->mHasNetworkPositionSource) {
            mLocation.mHasPositionSource = true;
            if (IZAT_NETWORK_POSITION_FROM_CELL == loc->mNetworkPositionSource) {
                mLocation.mPositionSource = CELLID;
            } else if (IZAT_NETWORK_POSITION_FROM_WIFI == loc->mNetworkPositionSource) {
                mLocation.mPositionSource = WIFI;
            }
          }
        }
        virtual void proc() const;
    };

    struct onSetRequestMsg : public LocMsg {
        OsNpProxy* mOsNpProxy;
        LocationRequest mRequest;

        onSetRequestMsg(OsNpProxy* pOsNpProxy, const LocationRequest* request)
        : mOsNpProxy(pOsNpProxy), mRequest(*request) {}
        virtual void proc() const;
    };

    struct subscribeMsg : public LocMsg {
        OsNpProxy* mOsNpProxy;
        ILocationResponse* mLocResponse;

        subscribeMsg(OsNpProxy* pOsNpProxy, ILocationResponse* locRes)
        : mOsNpProxy(pOsNpProxy), mLocResponse(locRes) {}
        virtual void proc() const;
    };

    struct unsubscribeMsg : public LocMsg {
        OsNpProxy* mOsNpProxy;
         ILocationResponse* mLocResponse;

        unsubscribeMsg(OsNpProxy* pOsNpProxy, ILocationResponse* locRes)
        : mOsNpProxy(pOsNpProxy), mLocResponse(locRes) {}
        virtual void proc() const;
    };

    struct enableMsg : public LocMsg {
        OsNpProxy* mOsNpProxy;

        enableMsg(OsNpProxy* pOsNpProxy)
        : mOsNpProxy(pOsNpProxy) {}
        virtual void proc() const;
    };

    struct disableMsg : public LocMsg {
        OsNpProxy* mOsNpProxy;

        disableMsg(OsNpProxy* pOsNpProxy)
        : mOsNpProxy(pOsNpProxy) {}
        virtual void proc() const;
    };


};

} // izat_manager

#endif // #ifndef __IZAT_MANAGER_OSNPPROXY_H__
