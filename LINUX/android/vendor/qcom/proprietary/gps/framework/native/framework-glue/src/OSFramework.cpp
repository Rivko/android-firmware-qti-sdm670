/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/


#define LOG_TAG "OSFramework"
#define LOG_NDEBUG 0

#include "OSFramework.h"
#include <loc_pla.h>

#include "Subscription.h"
#include "FrameworkActionRequest.h"
#include "IzatDefines.h"
#include "OSFrameworkCleaner.h"
#include "OsNpGlue.h"

OSFramework* OSFramework::mOSFramework = NULL;

OSFramework::OSFramework()
{
    int result = -1;
    ENTRY_LOG();

    do {
        // Create IzatManager
        IIzatManager *pIzatManager = getIzatManager(this);
        BREAK_IF_ZERO(1, pIzatManager);

        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

OSFramework::~OSFramework()
{
    ENTRY_LOG();

    // Destroy IzatManager
    (void) destroyIzatManager();

    EXIT_LOG_WITH_ERROR("%d", 0);
}

OSFramework* OSFramework::getOSFramework()
{
    int result = -1;

    ENTRY_LOG();
    do {
          // already initialized
          BREAK_IF_NON_ZERO(0, mOSFramework);

          mOSFramework = new (std::nothrow) OSFramework();
          BREAK_IF_ZERO(2, mOSFramework);

          result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
    return mOSFramework;
}

void OSFramework::destroyInstance()
{
    ENTRY_LOG();

    if (mOSFramework) {
        delete mOSFramework;
        mOSFramework = NULL;
    }

    EXIT_LOG_WITH_ERROR("%d", 0);
}

// IOSFramework overrides
IDataItemSubscription* OSFramework::getDataItemSubscriptionObj()
{
    return Subscription::getSubscriptionObj();
}

IFrameworkActionReq* OSFramework::getFrameworkActionReqObj()
{
    return FrameworkActionRequest::getFrameworkActionReqObj();
}

IOSLocationProvider * OSFramework::getOSNP()
{
    return OsNpGlue::getInstance();
}

IOSFrameworkCleaner * OSFramework::getOSFrameworkCleaner()
{
    return OSFrameworkCleaner::getInstance();
}
