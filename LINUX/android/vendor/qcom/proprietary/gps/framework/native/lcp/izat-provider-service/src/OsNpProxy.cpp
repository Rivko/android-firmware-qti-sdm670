/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  OS Network Provider Proxy module

  Copyright  (c) 2015 - 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Confidential and Proprietary.
=============================================================================*/

#define LOG_NDEBUG 0
#define LOG_TAG "IzatSvc_OsNpProxy"


#include "OsNpProxy.h"
#include "IzatDefines.h"
#include "LocationReport.h"
#include "IzatRequest.h"
#include "IOSLocationProvider.h"
#include "IOSFramework.h"
#include <log_util.h>

using namespace izat_manager;


OsNpProxy* OsNpProxy::createInstance(const struct s_IzatContext* ctx) {
    ENTRY_LOG();

    int result = -1;
    OsNpProxy* pOsNpProxy = NULL;

    do {
        BREAK_IF_ZERO(1, ctx);
        BREAK_IF_ZERO(2, ctx->mMsgTask);
        BREAK_IF_ZERO(3, ctx->mOSFrameworkObj);

        IOSLocationProvider *pOSNPProvider = ctx->mOSFrameworkObj->getOSNP();
        BREAK_IF_ZERO(4, pOSNPProvider);

        pOsNpProxy = new (std::nothrow) OsNpProxy(ctx, pOSNPProvider);
        BREAK_IF_ZERO(5, pOsNpProxy);
        result = 0;
    } while(0);

    if (result != 0) {
        delete pOsNpProxy;
        pOsNpProxy = NULL;
    }

    EXIT_LOG_WITH_ERROR("%d", result);
    return pOsNpProxy;
}

OsNpProxy::OsNpProxy(const struct s_IzatContext* ctx, IOSLocationProvider *pOSNPProvider)
: mIzatContext(ctx)
, mClient(NULL)
, mOSNPProvider(pOSNPProvider)
, mEnabled(false) {
};

OsNpProxy::~OsNpProxy() {
    if (mEnabled)
    {
        mOSNPProvider->stopRequest();
        mOSNPProvider->unsubscribe(this);
        mEnabled = false;
    }
}

// IIzatListener overrides
void OsNpProxy::onLocationChanged(const IzatLocation* location, const IzatLocationStatus status) {
    ENTRY_LOG();

    int result = -1;
    do {
        BREAK_IF_ZERO(1, status == IZAT_LOCATION_STATUS_FINAL)
        BREAK_IF_ZERO(2, location);

        onLocationChangedMsg *msg = new
            (nothrow) onLocationChangedMsg(this, location);
        BREAK_IF_ZERO(3, msg);
        mIzatContext->mMsgTask->sendMsg(msg);

        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

void OsNpProxy::onStatusChanged(IzatProviderStatus status) {
    ENTRY_LOG();
    LOC_LOGD("OsNpProxy status: %d", status);
    EXIT_LOG_WITH_ERROR("%d", 0);
}

// ILocationProvider overrides
int OsNpProxy::setRequest(const LocationRequest* request) {
    ENTRY_LOG();

    int result = -1;
    do {
        BREAK_IF_ZERO(1, request);

        onSetRequestMsg *msg = new (nothrow) onSetRequestMsg(this, request);
        BREAK_IF_ZERO(2, msg);
        mIzatContext->mMsgTask->sendMsg(msg);

        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}

void OsNpProxy::subscribe(const ILocationResponse* respObject) {
    ENTRY_LOG();

    int result = -1;
    do {
        BREAK_IF_ZERO(1, respObject);
        subscribeMsg *msg = new (nothrow) subscribeMsg(this,
            const_cast<ILocationResponse*>(respObject));
        BREAK_IF_ZERO(2, msg);
        mIzatContext->mMsgTask->sendMsg(msg);
        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

void OsNpProxy::unsubscribe(const ILocationResponse* respObject) {
    ENTRY_LOG();

    int result = -1;
    do {
        BREAK_IF_ZERO(1, respObject);
        unsubscribeMsg *msg = new (nothrow) unsubscribeMsg(this,
            const_cast<ILocationResponse*>(respObject));

        BREAK_IF_ZERO(2, msg);
        mIzatContext->mMsgTask->sendMsg(msg);
        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

void OsNpProxy::enable() {
    ENTRY_LOG();
    mIzatContext->mMsgTask->sendMsg(new (nothrow) enableMsg(this));
    EXIT_LOG_WITH_ERROR("%d", 0);
}

void OsNpProxy::disable() {
    ENTRY_LOG();
    mIzatContext->mMsgTask->sendMsg(new (nothrow) disableMsg(this));
    EXIT_LOG_WITH_ERROR("%d", 0);
}

// Async messages
void OsNpProxy::onLocationChangedMsg::proc() const {
    ENTRY_LOG();

    int result = -1;
    do {
        BREAK_IF_ZERO(1, mOsNpProxy->mEnabled); // Disabled
        BREAK_IF_ZERO(2, mOsNpProxy->mClient); // No subscription active

        mOsNpProxy->mClient->reportLocation(&mLocation, mOsNpProxy);

        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
};

void OsNpProxy::onSetRequestMsg::proc() const {
    ENTRY_LOG();

    int result = -1;
    IzatRequest *izatReq = NULL;
    do {
        BREAK_IF_ZERO(1, mOsNpProxy->mEnabled); // Disabled

        LOC_LOGD ("LocTech-Label :: OsNpProxy :: Position Request In");
        LOC_LOGD ("LocTech-Value :: Action (Single Shot = 0, Start = 1, Stop = 2): %d", mRequest.action);
        LOC_LOGD ("LocTech-Value :: Interval In milliseconds: %u", mRequest.intervalInMsec);

        if ((LocationRequestAction_Start == mRequest.action) ||
            (LocationRequestAction_SingleShot == mRequest.action)) {
            izatReq = IzatRequest::createProviderRequest(
                        IZAT_STREAM_NETWORK,
                        mRequest.intervalInMsec, // timeBetweenFixes
                        0.0f, // distanceBetweenFixes
                        (LocationRequestAction_SingleShot == mRequest.action) // singleShot
                        );
            BREAK_IF_ZERO(2, izatReq);
            BREAK_IF_NON_ZERO(3, mOsNpProxy->mOSNPProvider->setRequest(izatReq));
        } else if (LocationRequestAction_Stop == mRequest.action) {
            mOsNpProxy->mOSNPProvider->stopRequest();
        } else {
            LOC_LOGW("Unexpected request action: %d", mRequest.action);
        }
        result = 0;
    } while(0);

    delete izatReq;
    izatReq = NULL;

    EXIT_LOG_WITH_ERROR("%d", result);
};

void OsNpProxy::subscribeMsg::proc() const {
    ENTRY_LOG();
    mOsNpProxy->mClient = mLocResponse;
    EXIT_LOG_WITH_ERROR("%d", 0);
};

void OsNpProxy::unsubscribeMsg::proc() const {
    ENTRY_LOG();
    mOsNpProxy->mClient = NULL;
    EXIT_LOG_WITH_ERROR("%d", 0);
};

void OsNpProxy::enableMsg::proc() const {
    ENTRY_LOG();

    int result = -1;
    do {
        BREAK_IF_NON_ZERO(1, mOsNpProxy->mEnabled);
        BREAK_IF_NON_ZERO(2, mOsNpProxy->mOSNPProvider->subscribe(mOsNpProxy))
        mOsNpProxy->mEnabled = true;
        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
};

void OsNpProxy::disableMsg::proc() const {
    ENTRY_LOG();

    int result = -1;
    do {
        BREAK_IF_ZERO(1, mOsNpProxy->mEnabled);
        mOsNpProxy->mOSNPProvider->stopRequest();
        BREAK_IF_NON_ZERO(2, mOsNpProxy->mOSNPProvider->unsubscribe(mOsNpProxy))
        mOsNpProxy->mEnabled = false;
        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
};
