/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  loc service module

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Confidential and Proprietary.
=============================================================================*/

#ifndef __IZAT_MANAGER_LOCATIONPROVIDER_H__
#define __IZAT_MANAGER_LOCATIONPROVIDER_H__

#include "ILocationProvider.h"
#include "IzatContext.h"
#include "IzatDefines.h"
#include <mq_client/IPCMessagingProxy.h>
#include "Utils.h"
#include <mq_client/IPCMessagingProxy.h>
#include "LocationReport.h"
#include "LocationError.h"
#include <string>
#include "IDataItemObserver.h"
#include "IDataItemCore.h"

using loc_core::IDataItemCore;
using loc_core::IDataItemObserver;

namespace izat_manager
{

using namespace qc_loc_fw;
using namespace std;

class IOSFramework;

class LocationProvider : public ILocationProvider,
    public ILocationResponse,
    public IIPCMessagingResponse,
    public IDataItemObserver {
public:

     // ILocationProvider overrides
    virtual int setRequest(const LocationRequest * /*request*/) {
        return 0;
    }
    virtual void subscribe(const ILocationResponse * locRespObj);
    virtual void unsubscribe(const ILocationResponse * locRespObj);
    virtual void enable() {}
    virtual void disable() {}

    // ILocationResponse overrides
    virtual void reportLocation(const LocationReport * /*locReport*/,
        const ILocationProvider* /*providerSrc*/) {}
    virtual void reportError(const LocationError * /*errorReport*/,
        const ILocationProvider* /*providerSrc*/) {}

    // IIPCMessagingResponse overrides
    virtual void handleMsg(InPostcard * const /*in_card*/) {};

    // IDataItemObserver overrides
    virtual void getName (std::string & name) { name = "LocationProvider";}
    virtual void notify(const std::list<IDataItemCore *> & /*dlist*/) {}

    virtual ~LocationProvider();

protected:
    // static data members
    const static std::string mIzatConfFile;

    const struct s_IzatContext *mIzatContext;
    list <const ILocationResponse *> mLocRespObjs;

    LocationProvider(const struct s_IzatContext *izatContext) :
        mIzatContext(izatContext) {}
    void broadcastLocation(const LocationReport *locReport);
    void broadcastError(const LocationError *locError);

    struct SubscribeLocMsg : public LocMsg {
        LocationProvider *mProvider;
        const ILocationResponse * mLocRespObj;

        inline SubscribeLocMsg (LocationProvider * provider,
                         const ILocationResponse *locRespObj) :
            mProvider(provider), mLocRespObj(locRespObj){
        }
        virtual ~SubscribeLocMsg () {}
        void proc () const;
    };

    struct UnsubscribeLocMsg : public LocMsg {
        LocationProvider *mProvider;
        const ILocationResponse * mLocRespObj;

        inline UnsubscribeLocMsg (LocationProvider * provider,
                         const ILocationResponse *locRespObj) :
            mProvider(provider), mLocRespObj(locRespObj){
        }

        virtual ~UnsubscribeLocMsg () {}
        void proc () const;
    };

private:
};

} // namespace izat_manager

#endif // #ifndef __IZAT_MANAGER_LOCATIONPROVIDER_H__
