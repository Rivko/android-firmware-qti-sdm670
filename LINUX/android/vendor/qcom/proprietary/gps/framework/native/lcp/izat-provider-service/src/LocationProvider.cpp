/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  loc service module

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Confidential and Proprietary.
=============================================================================*/

#define LOG_NDEBUG 0
#define LOG_TAG "IzatSvc_LocationProvider"

#include "LocationProvider.h"
#include "IOSFramework.h"
#include "Utils.h"
#include "loc_cfg.h"
#include <algorithm>

namespace izat_manager
{
const std::string LocationProvider::mIzatConfFile(LOC_PATH_IZAT_CONF);

LocationProvider::~LocationProvider()
{
    ENTRY_LOG();

    // clear the list of locRespObjs if any
    mLocRespObjs.clear();

    EXIT_LOG_WITH_ERROR("%d", 0);
}

void LocationProvider::broadcastLocation(const LocationReport *locReport)
{
    int result = 0;
    ENTRY_LOG();

    do {
        BREAK_IF_ZERO(1, mLocRespObjs.size());

        list <const ILocationResponse *> :: iterator it = mLocRespObjs.begin();
        for (; it != mLocRespObjs.end(); it++)
        {
            ILocationResponse *respObj = (ILocationResponse *)(*it);
            respObj->reportLocation(locReport, this);
        }
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

void LocationProvider::broadcastError(const LocationError *locError)
{
    int result = 0;
    ENTRY_LOG();

    do {
        BREAK_IF_ZERO(1, mLocRespObjs.size());

        list <const ILocationResponse *> :: iterator it = mLocRespObjs.begin();
        for (; it != mLocRespObjs.end(); it++)
        {
            ILocationResponse *respObj = (ILocationResponse *)(*it);
            respObj->reportError(locError, this);
        }
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

// ILocationProvider overrides
void LocationProvider::subscribe (const ILocationResponse * locRespObj)
{
    int result = 0;
    ENTRY_LOG ();
    do {
        BREAK_IF_ZERO (1, locRespObj);
        mIzatContext->mMsgTask->sendMsg (new (nothrow) SubscribeLocMsg (this, locRespObj));
    } while (0);
    EXIT_LOG_WITH_ERROR ("%d", result);
}

void LocationProvider::unsubscribe (const ILocationResponse * locRespObj)
{
    int result = 0;
    ENTRY_LOG ();
    do {
        BREAK_IF_ZERO (1, locRespObj);
        mIzatContext->mMsgTask->sendMsg (new (nothrow) UnsubscribeLocMsg (this, locRespObj));
    } while (0);
    EXIT_LOG_WITH_ERROR ("%d", result);
}

void LocationProvider :: SubscribeLocMsg :: proc () const {
    ENTRY_LOG ();
    if (mProvider->mLocRespObjs.end() ==
         find (mProvider->mLocRespObjs.begin(), mProvider->mLocRespObjs.end(),
             mLocRespObj)) {

        mProvider->mLocRespObjs.push_back (mLocRespObj);
    }
    EXIT_LOG_WITH_ERROR ("%d", 0);
}

void LocationProvider :: UnsubscribeLocMsg :: proc () const {

    ENTRY_LOG ();
    list <const ILocationResponse *> :: iterator it =
        find (mProvider->mLocRespObjs.begin(), mProvider->mLocRespObjs.end(),
            mLocRespObj);

    if (it != mProvider->mLocRespObjs.end ()) {
        mProvider->mLocRespObjs.erase (it);
    }
    EXIT_LOG_WITH_ERROR ("%d", 0);
}

}// namespace izat_manager
