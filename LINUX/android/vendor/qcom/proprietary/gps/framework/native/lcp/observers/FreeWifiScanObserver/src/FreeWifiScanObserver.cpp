/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  FreeWifiScanObserver Implementation

  Copyright (c) 2015 - 2016 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

#define LOG_NDEBUG 0
#define LOG_TAG "IzatSvc_FreeWifiScanObserver"

#include <list>
#include <new>
#include <algorithm>
#include <lowi_request.h>
#include <lowi_response.h>
#include <lowi_client.h>
#include <IFreeWifiScanSubscription.h>
#include <mq_client/IPCMessagingProxy.h>
#include <IFreeWifiScanObserver.h>
#include <FreeWifiScanObserver.h>
#include <log_util.h>
#include <IzatDefines.h>
#include <mq_client/IPCMessagingProxy.h>
#include <postcard.h>

using namespace std;
using namespace qc_loc_fw;
using namespace izat_manager;

//TODO: Below LOWI parameters are tentative
const int MEAS_FILTER_PERIOD = 10; //10 seconds
const int FALLBACK_TOLERANCE_PERIOD = 3; //3 seconds

FreeWifiScanObserver * FreeWifiScanObserver :: mInstance = NULL;

FreeWifiScanObserver* FreeWifiScanObserver :: getInstance
(IIPCMessagingRequest * ipcIface, MsgTask * msgTask) {

    if (!mInstance) {
        mInstance = new (nothrow) FreeWifiScanObserver (ipcIface, msgTask);
        if (mInstance) {
            int result = mInstance->mIpcIface->registerResponseObj
                             ( mInstance->mAddress.c_str (), mInstance);
            if (result != 0) {
                return NULL;
            }
        }
    }
    return mInstance;
}

void FreeWifiScanObserver :: destroyInstance () {
    delete mInstance;
    mInstance = NULL;
}

void FreeWifiScanObserver :: Timer:: timeOutCallback() {
    mClient->mMsgTask->sendMsg (new (nothrow) ReqExpiryMsg(mClient));
}

// ctors
inline FreeWifiScanObserver :: FreeWifiScanObserver
(IIPCMessagingRequest * ipcIface, MsgTask * msgTask)
:
mIpcIface (ipcIface),
mMsgTask (msgTask),
mAddress ("FreeWifiScanObserver"),
mRegisteredForFreeWifiScan(false),
mRequestId(0),
mTimer(this)
{}

inline FreeWifiScanObserver :: LocMsgBase :: LocMsgBase
(FreeWifiScanObserver * parent)
: mParent (parent) {}


inline FreeWifiScanObserver :: InitiateWifiScanLocMsg :: InitiateWifiScanLocMsg
(
FreeWifiScanObserver * parent,
unsigned long int measurementAgeFilterInSecs,
unsigned long int fallbackToleranceSec,
long long int timeoutTimestamp,
bool isEmergencyRequest
)
:
LocMsgBase (parent),
mMeasurementAgeFilterInSecs (measurementAgeFilterInSecs),
mFallbackToleranceSec (fallbackToleranceSec),
mTimeoutTimestamp (timeoutTimestamp),
mIsEmergencyRequest(isEmergencyRequest)
{}


inline FreeWifiScanObserver :: SubscribeLocMsg :: SubscribeLocMsg
(FreeWifiScanObserver * parent,
 const IFreeWifiScanObserver * observer)
: LocMsgBase (parent), mObserver (observer) {}

inline FreeWifiScanObserver :: UnsubscribeLocMsg :: UnsubscribeLocMsg
(FreeWifiScanObserver * parent,
 const IFreeWifiScanObserver * observer)
: LocMsgBase (parent), mObserver (observer) {}

inline FreeWifiScanObserver :: NotifyLocMsg :: NotifyLocMsg
(FreeWifiScanObserver * parent,
 const LOWIResponse * response)
: LocMsgBase (parent), mResponse (response) {}

inline FreeWifiScanObserver :: ReqExpiryMsg :: ReqExpiryMsg
(FreeWifiScanObserver * parent)
: LocMsgBase (parent)
{}


inline FreeWifiScanObserver :: LocMsgBase :: ~LocMsgBase () {}
inline FreeWifiScanObserver :: InitiateWifiScanLocMsg :: ~InitiateWifiScanLocMsg
() {}
inline FreeWifiScanObserver :: SubscribeLocMsg :: ~SubscribeLocMsg () {}
inline FreeWifiScanObserver :: UnsubscribeLocMsg :: ~UnsubscribeLocMsg () {}
FreeWifiScanObserver :: NotifyLocMsg :: ~NotifyLocMsg () {
    if (mResponse) {
        delete mResponse;
        mResponse = NULL;
    }
}
inline FreeWifiScanObserver :: ReqExpiryMsg :: ~ReqExpiryMsg () {}

// Helpers
void FreeWifiScanObserver :: sendFreeWifiScanRequest
(
    unsigned long int validTime
)
{
    int result = 0;
    OutPostcard * out = NULL;
    ENTRY_LOG ();
    do {
        mTimer.stop();

        LOWIAsyncDiscoveryScanResultRequest request (mRequestId, validTime);

        out = LOWIClient :: composePostCard (&request, mAddress.c_str());

        BREAK_IF_ZERO (1, out);

        BREAK_IF_NON_ZERO (2, mIpcIface->sendMsg (out, qc_loc_fw::SERVER_NAME));

        mRequestId++;

        mTimer.start(validTime * 1000, false);

    } while (0);

    if (out) {
        delete out;
    }

    LOC_LOGD ("Free Wifi-scan request validity period (secs) : %lu", validTime);

    EXIT_LOG_WITH_ERROR ("%d", result);
}

void FreeWifiScanObserver :: refreshFreeWifiScanRequest () {
    ENTRY_LOG ();
    sendFreeWifiScanRequest ();
    EXIT_LOG_WITH_ERROR ("%d", 0);
}

void FreeWifiScanObserver :: cancelFreeWifiScanRequest () {
    ENTRY_LOG ();
    sendFreeWifiScanRequest (0);
    EXIT_LOG_WITH_ERROR ("%d", 0);
}

// proc
/*
  We trigger a Fresh Scan Request for Emergency mode request from modem
  Cache fallback request otwerwise
 */
void FreeWifiScanObserver :: InitiateWifiScanLocMsg :: proc () const {

    int result = 0;
    LOWIDiscoveryScanRequest * request = 0;
    OutPostcard * out = 0;
    ENTRY_LOG ();
    do {
        if(mIsEmergencyRequest) {
            LOC_LOGV ("Send emergency on demand scan request");
            request =
                LOWIDiscoveryScanRequest::createFreshScanRequest
                (
                    this->mParent->mRequestId,
                    LOWIDiscoveryScanRequest::BAND_ALL,
                    LOWIDiscoveryScanRequest::ACTIVE_SCAN,
                    MEAS_FILTER_PERIOD,
                    mTimeoutTimestamp,
                    LOWIDiscoveryScanRequest::FORCED_FRESH
                    );
        } else {
            LOC_LOGV ("Send on demand scan request");
            request =
                LOWIDiscoveryScanRequest::createCacheFallbackRequest
                (
                    this->mParent->mRequestId,
                    LOWIDiscoveryScanRequest::TWO_POINT_FOUR_GHZ,
                    LOWIDiscoveryScanRequest::ACTIVE_SCAN,
                    MEAS_FILTER_PERIOD,
                    FALLBACK_TOLERANCE_PERIOD,
                    mTimeoutTimestamp,
                    false
                    );
        }

        BREAK_IF_ZERO (1, request);

        out = LOWIClient :: composePostCard (request,
                                             this->mParent->mAddress.c_str());

        BREAK_IF_ZERO (2, out);

        BREAK_IF_NON_ZERO (3, this->mParent->mIpcIface->sendMsg
                           (out,
                            qc_loc_fw::SERVER_NAME));

        this->mParent->mRequestId++;

    } while (0);

    if (out) {
        delete out;
    }

    EXIT_LOG_WITH_ERROR ("%d", result);
}

void FreeWifiScanObserver :: SubscribeLocMsg :: proc () const {
    ENTRY_LOG ();
    if ( this->mParent->mClients.end () ==
         find (this->mParent->mClients.begin (),
               this->mParent->mClients.end (),
               this->mObserver) ) {

        this->mParent->mClients.push_back (mObserver);

        if (!this->mParent->mRegisteredForFreeWifiScan) {
            this->mParent->refreshFreeWifiScanRequest ();
            this->mParent->mRegisteredForFreeWifiScan = true;
        }
    }
    EXIT_LOG_WITH_ERROR ("%d", 0);
}

void FreeWifiScanObserver :: UnsubscribeLocMsg :: proc () const {

    ENTRY_LOG ();
    list <const IFreeWifiScanObserver *> :: iterator it =
        find (this->mParent->mClients.begin (),
              this->mParent->mClients.end (),
              this->mObserver);

    if (it != this->mParent->mClients.end ()) {
        this->mParent->mClients.erase (it);
        if ( this->mParent->mClients.empty () &&
             this->mParent->mRegisteredForFreeWifiScan ) {
            this->mParent->cancelFreeWifiScanRequest ();
            this->mParent->mRegisteredForFreeWifiScan = false;
        }
    }
    EXIT_LOG_WITH_ERROR ("%d", 0);
}

void FreeWifiScanObserver :: NotifyLocMsg :: proc () const {
    int result = 0;
    ENTRY_LOG ();
    do {
        BREAK_IF_ZERO (1, mResponse);
        list <const IFreeWifiScanObserver *> :: iterator it =
            this->mParent->mClients.begin ();
        for ( ; it != this->mParent->mClients.end (); ++it) {
           (const_cast <IFreeWifiScanObserver *>(*it))->notify (mResponse);
        }
    } while (0);
    EXIT_LOG_WITH_ERROR ("%d", result);
}

void FreeWifiScanObserver :: ReqExpiryMsg :: proc () const {
    ENTRY_LOG ();

    if ( this->mParent->mClients.empty () != 0) {
        this->mParent->refreshFreeWifiScanRequest ();
    }
    EXIT_LOG_WITH_ERROR ("%d", 0);
}

void FreeWifiScanObserver :: initiateWifiScan
(
unsigned long int measurementAgeFilterInSecs,
unsigned long int fallbackToleranceSec,
long long int timeoutTimestamp,
bool isEmergencyRequest
) {
    ENTRY_LOG ();
    mMsgTask->sendMsg (new (nothrow) InitiateWifiScanLocMsg
                                         (this,
                                          measurementAgeFilterInSecs,
                                          fallbackToleranceSec,
                                          timeoutTimestamp,
                                          isEmergencyRequest) );
    EXIT_LOG_WITH_ERROR ("%d", 0);
}

void FreeWifiScanObserver :: subscribe
(
    const IFreeWifiScanObserver * observer
)
{
    int result = 0;
    ENTRY_LOG ();
    do {
        BREAK_IF_ZERO (1, observer);
        mMsgTask->sendMsg (new (nothrow) SubscribeLocMsg (this, observer));
    } while (0);
    EXIT_LOG_WITH_ERROR ("%d", result);
}

void FreeWifiScanObserver :: unsubscribe
(
    const IFreeWifiScanObserver * observer
)
{
    int result = 0;
    ENTRY_LOG ();
    do {
        BREAK_IF_ZERO (1, observer);
        mMsgTask->sendMsg (new (nothrow) UnsubscribeLocMsg (this, observer));
    } while (0);
    EXIT_LOG_WITH_ERROR ("%d", result);
}

void FreeWifiScanObserver :: handleMsg (InPostcard * const inCard) {
    int result = 0;
    ENTRY_LOG ();
    do {
        const char* info = NULL;
        const char* resp = NULL;

        if (inCard->getString("INFO", &info) == 0){
            if (strcmp(info, "REGISTER-EVENT") == 0) {
                const char *client_name = NULL;
                if (inCard->getString("CLIENT", &client_name) == 0) {
                   if (0 == strcmp(client_name, "LOWI-SERVER")) {
                       mMsgTask->sendMsg (new (nothrow) ReqExpiryMsg (this));
                   }
               }
            }
        } else if (inCard->getString("RESP", &resp) == 0) {

             if (strcmp(resp, "LOWI_ASYNC_DISCOVERY_SCAN_RESULTS") == 0) {
                LOWIAsyncDiscoveryScanResultResponse * response =
                  static_cast <LOWIAsyncDiscoveryScanResultResponse *>
                  (
                      LOWIClient :: parsePostCard (inCard)
                  );
                BREAK_IF_ZERO (1, response);
                LOC_LOGD ("LOWI ASYNC DISCOVERY response received with status:%d",
                          response->scanStatus);
                mMsgTask->sendMsg (new (nothrow) NotifyLocMsg (this, response));
             }

             if (strcmp(resp, "LOWI_DISCOVERY_SCAN") == 0) {
                LOWIDiscoveryScanResponse * response =
                  static_cast <LOWIDiscoveryScanResponse *>
                  (
                      LOWIClient :: parsePostCard (inCard)
                  );
                BREAK_IF_ZERO (2, response);
                LOC_LOGD ("LOWI DISCOVERY response received with status:%d",
                          response->scanStatus);
                mMsgTask->sendMsg (new (nothrow) NotifyLocMsg (this, response));
            }
        }
    } while (0);
    EXIT_LOG_WITH_ERROR ("%d", result);
}
