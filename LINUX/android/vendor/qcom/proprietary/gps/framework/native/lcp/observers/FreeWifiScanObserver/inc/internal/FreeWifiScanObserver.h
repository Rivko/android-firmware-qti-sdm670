/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  IIPCMessagingResponse

  Copyright (c) 2015 - 2016 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

#ifndef __IZAT_MANAGER_FREEWIFISCANOBSERVER_H__
#define __IZAT_MANAGER_FREEWIFISCANOBSERVER_H__

#include <list>
#include <string>
#include <lowi_request.h>
#include <mq_client/IPCMessagingProxy.h>
#include <MsgTask.h>
#include <IWifiScanInitiator.h>
#include <IFreeWifiScanSubscription.h>
#include <LocTimer.h>

namespace qc_loc_fw {
class InPostcard;
class LOWIAsyncDiscoveryScanResultRequest;
}

namespace izat_manager
{

using namespace std;
using namespace qc_loc_fw;

class IFreeWifiScanObserver;

class FreeWifiScanObserver :
public IWifiScanInitiator,
public IFreeWifiScanSubscription,
public IIPCMessagingResponse {

public:
    // Singleton methods
    static FreeWifiScanObserver * getInstance (IIPCMessagingRequest * ipcIface,
                                               MsgTask * msgTask);
    static void destroyInstance ();

    // IWifiScanInitiator overrides
    void initiateWifiScan
    (
        unsigned long int measurementAgeFilterInSecs,
        unsigned long int fallbackToleranceSec,
        long long int timeoutTimestamp,
        bool mIsEmergencyRequest
    );

    // IFreeWifiScanSubscription, overrides
    void subscribe (const IFreeWifiScanObserver * observer);
    void unsubscribe (const IFreeWifiScanObserver * observer);

    // IIPCMessagingResponse overrides
    void handleMsg (InPostcard * const inCard);


private:
    FreeWifiScanObserver (IIPCMessagingRequest * ipcIface, MsgTask * msgTask);
    inline ~FreeWifiScanObserver () {}

    // helpers
    void sendFreeWifiScanRequest
    (
        unsigned long int validTime = ASYNC_DISCOVERY_REQ_VALIDITY_PERIOD_SEC_MAX
    );
    void refreshFreeWifiScanRequest ();
    void cancelFreeWifiScanRequest ();

    // Loc messages
    struct LocMsgBase : public LocMsg {
        LocMsgBase (FreeWifiScanObserver * parent);
        virtual ~LocMsgBase ();
        // Data memebers
        FreeWifiScanObserver * mParent;
    };

    struct InitiateWifiScanLocMsg : public LocMsgBase {

        InitiateWifiScanLocMsg (FreeWifiScanObserver * parent,
                                unsigned long int measurementAgeFilterInSecs,
                                unsigned long int fallbackToleranceSec,
                                long long int timeoutTimestamp,
                                bool isEmergencyRequest);
        virtual ~InitiateWifiScanLocMsg ();
        void proc () const;
        // Data members
        const unsigned long int mMeasurementAgeFilterInSecs;
        const unsigned long int mFallbackToleranceSec;
        const int64 mTimeoutTimestamp;
        bool mIsEmergencyRequest;
    };

    struct SubscribeLocMsg : public LocMsgBase {
        SubscribeLocMsg (FreeWifiScanObserver * parent,
                         const IFreeWifiScanObserver * observer);
        virtual ~SubscribeLocMsg ();
        void proc () const;
        // Data members
        const IFreeWifiScanObserver * mObserver;
    };

    struct UnsubscribeLocMsg : public LocMsgBase {
        UnsubscribeLocMsg (FreeWifiScanObserver * parent,
                           const IFreeWifiScanObserver * observer);
        virtual ~UnsubscribeLocMsg ();
        void proc () const;
        // Data members
        const IFreeWifiScanObserver * mObserver;
    };

    struct NotifyLocMsg : public LocMsgBase {
        NotifyLocMsg (FreeWifiScanObserver * parent,
                      const LOWIResponse * response);
        virtual ~NotifyLocMsg ();
        void proc () const;
        // Data members
        const LOWIResponse * mResponse;
    };

    struct ReqExpiryMsg : public LocMsgBase {
        ReqExpiryMsg (FreeWifiScanObserver * parent);
        virtual ~ReqExpiryMsg ();
        void proc () const;
        // Data members
    };

    class Timer : public LocTimer {
        FreeWifiScanObserver* mClient;
    public:
        inline Timer(FreeWifiScanObserver* client) :
            LocTimer(), mClient(client) {}
        virtual void timeOutCallback();
    };

    // Data members
    static FreeWifiScanObserver         * mInstance;
    IIPCMessagingRequest                * mIpcIface;
    MsgTask                             * mMsgTask;
    string                                mAddress;
    bool                                  mRegisteredForFreeWifiScan;
    unsigned long int                     mRequestId;
    Timer                                 mTimer;
    list <const IFreeWifiScanObserver *>  mClients;
    friend class Timer;
};

} // namespace izat_manager

#endif // #ifndef __IZAT_MANAGER_FREEWIFISCANOBSERVER_H__

