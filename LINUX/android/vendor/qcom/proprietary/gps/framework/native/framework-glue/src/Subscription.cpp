/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  loc service module

  Copyright  (c) 2015-2017 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/


#define LOG_TAG "Subscription"
#define LOG_NDEBUG 0
#include <loc_pla.h>
#include <log_util.h>
#include "Subscription.h"
#include "IzatDefines.h"
#include <list>

Subscription* Subscription::mSubscriptionObj = NULL;
#ifdef USE_QCMAP
LocNetIface* Subscription::mLocNetIfaceObj = NULL;
#endif
IDataItemObserver* Subscription::mObserverObj = NULL;
SubscriptionCallbackIface* Subscription::sSubscriptionCb = NULL;

using namespace std;

// Subscription class implementation
Subscription* Subscription::getSubscriptionObj()
{
    int result = 0;

    ENTRY_LOG();
    do {
          // already initialized
          BREAK_IF_NON_ZERO(0, mSubscriptionObj);

          mSubscriptionObj = new (std::nothrow) Subscription();
          BREAK_IF_ZERO(1, mSubscriptionObj);
#ifdef USE_QCMAP
          mLocNetIfaceObj = new (std::nothrow) LocNetIface();
          BREAK_IF_ZERO(2, mLocNetIfaceObj);
          mLocNetIfaceObj->registerDataItemNotifyCallback(
                  Subscription::locNetIfaceCallback, NULL);
#endif
          result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
    return mSubscriptionObj;
}

void Subscription::destroyInstance()
{
    ENTRY_LOG();

    delete mSubscriptionObj;
    mSubscriptionObj = NULL;

    EXIT_LOG_WITH_ERROR("%d", 0);
}

//IDataItemSubscription overrides
void Subscription::subscribe(const std::list<DataItemId> & l, IDataItemObserver * observerObj)
{
    // Assign the observer object if required
    if ((Subscription::mObserverObj == NULL) && (observerObj)) {
        Subscription::mObserverObj = observerObj;
    }

#if defined(USE_QCMAP)
    mLocNetIfaceObj->subscribe(l);
#else
    if (sSubscriptionCb == NULL) {
        LOC_LOGE("sSubscriptionCb NULL !");
        return;
    }
    sSubscriptionCb->updateSubscribe(l, true);
#endif
}

void Subscription::updateSubscription(const std::list<DataItemId> & /*l*/, IDataItemObserver * /*observerObj*/)
{
}

void Subscription::requestData(const std::list<DataItemId> & l, IDataItemObserver * observerObj)
{
    // Assign the observer object if required
    if ((Subscription::mObserverObj == NULL) && (observerObj)) {
        Subscription::mObserverObj = observerObj;
    }

#if defined(USE_QCMAP)
    mLocNetIfaceObj->requestData(l);
#else
    if (sSubscriptionCb == NULL) {
        LOC_LOGE("sSubscriptionCb NULL !");
        return;
    }
    sSubscriptionCb->requestData(l);
#endif
}

void Subscription::unsubscribe(const std::list<DataItemId> & l, IDataItemObserver * observerObj)
{
     // Assign the observer object if required
    if ((Subscription::mObserverObj == NULL) && (observerObj)) {
        Subscription::mObserverObj = observerObj;
    }

#if defined(USE_QCMAP)
    mLocNetIfaceObj->unsubscribe(l);
#else
    if (sSubscriptionCb == NULL) {
        LOC_LOGE("sSubscriptionCb NULL !");
        return;
    }
    sSubscriptionCb->updateSubscribe(l, false);
#endif
}

void Subscription::unsubscribeAll(IDataItemObserver * observerObj)
{
    // Assign the observer object if required
    if ((Subscription::mObserverObj == NULL) && (observerObj)) {
        Subscription::mObserverObj = observerObj;
    }

#if defined(USE_QCMAP)
    mLocNetIfaceObj->unsubscribeAll();
#else
    if (sSubscriptionCb == NULL) {
        LOC_LOGE("sSubscriptionCb NULL !");
        return;
    }
    sSubscriptionCb->unsubscribeAll();
#endif
}

void Subscription::setSubscriptionCallback(SubscriptionCallbackIface* cb) {

    ENTRY_LOG();
    sSubscriptionCb = cb;
}

SubscriptionCallbackIface* Subscription::getSubscriptionCallback() {

    return sSubscriptionCb;
}

#ifdef USE_QCMAP
void Subscription::locNetIfaceCallback(void* userDataPtr, std::list<IDataItemCore*>& itemList)
{
    LOC_LOGV("Subscription::locNetIfaceCallback");
    if (Subscription::mObserverObj == NULL) {
        LOC_LOGE("NULL observer object");
        return;
    }
    Subscription::mObserverObj->notify(itemList);
}
#endif

