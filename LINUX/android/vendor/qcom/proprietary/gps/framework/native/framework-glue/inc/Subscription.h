/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  loc service module

  Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#ifndef __SUBSCRIPTION_H__
#define __SUBSCRIPTION_H__

#include <IDataItemSubscription.h>
#include <IDataItemObserver.h>
#include <DataItemId.h>
#include <loc_pla.h>

#ifdef USE_QCMAP
#include "LocNetIface.h"
#endif

using loc_core::IDataItemObserver;
using loc_core::IDataItemSubscription;

struct SubscriptionCallbackIface {

    virtual ~SubscriptionCallbackIface() = default;
    virtual void updateSubscribe(const std::list<DataItemId> & l, bool subscribe);
    virtual void requestData(const std::list<DataItemId> & l);
    virtual void unsubscribeAll();
    virtual void turnOnModule(DataItemId dit,int timeOut);
    virtual void turnOffModule(DataItemId dit);
};

class Subscription : public IDataItemSubscription {

public:
    static IDataItemObserver *mObserverObj;

    static Subscription* getSubscriptionObj();
    static void destroyInstance();
#ifdef USE_QCMAP
    inline LocNetIface* getLocNetIfaceObj() {
        return mLocNetIfaceObj;
    }
#endif

    // IDataItemSubscription overrides
    void subscribe(const std::list<DataItemId> & l, IDataItemObserver * observerObj = NULL);
    void updateSubscription(const std::list<DataItemId> & l, IDataItemObserver * observerObj = NULL);
    void requestData(const std::list <DataItemId> & l, IDataItemObserver * client = NULL);
    void unsubscribe(const std::list<DataItemId> & l, IDataItemObserver * observerObj = NULL);
    void unsubscribeAll(IDataItemObserver * observerObj = NULL);

    static void setSubscriptionCallback(SubscriptionCallbackIface* cb);
    static SubscriptionCallbackIface* getSubscriptionCallback();

protected:

private:
    static Subscription *mSubscriptionObj;
#ifdef USE_QCMAP
    static LocNetIface* mLocNetIfaceObj;
#endif

    Subscription() {
      mObserverObj = NULL;
    }
    ~Subscription() {}

    static SubscriptionCallbackIface* sSubscriptionCb;

#ifdef USE_QCMAP
    static void locNetIfaceCallback(
            void* userDataPtr, std::list<IDataItemCore*>& itemList);
#endif

};

#endif // #ifndef __SUBSCRIPTION_H__
