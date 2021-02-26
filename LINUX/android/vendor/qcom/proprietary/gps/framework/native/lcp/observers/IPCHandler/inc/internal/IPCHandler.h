/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  IPCHandler

  Copyright  (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

#ifndef __IZAT_MANAGER_IPCHANDLER_H__
#define __IZAT_MANAGER_IPCHANDLER_H__


#include <map>
#include <MsgTask.h>
#include <mq_client/IPCMessagingProxy.h>
#include <IIPCHandlerRequest.h>
#include <IDataItemObserver.h>
#include <IIPCHandlerRegistration.h>
#include <IIPCHandlerRequest.h>
#include <IDataItemSubscription.h>

// Forward delcarations
namespace qc_loc_fw {
class InPostcard;
class OutPostcard;
}

using loc_core::IDataItemObserver;
using loc_core::IDataItemSubscription;

namespace izat_manager {

// using declarations
using namespace std;
using namespace qc_loc_fw;

// Forward delcarations
class IIPCHandlerRegistration;
class IIPCHandlerRequest;

// Class involved with sending and receiving
// IPC messages
class IPCHandler :
public IIPCHandlerRegistration,
public IIPCHandlerRequest,
public IIPCMessagingResponse {

public:
    static IPCHandler * getInstance (IIPCMessagingRequest * ipcIface,
                                    MsgTask * msgTask);
    static void destroyInstance ();

    // IIPCHandlerRegistration overrides
    void reg (const string & name , IDataItemSubscription * ipcClient);

    // IIPCMessagingResponse overrides
    void handleMsg (InPostcard * const inCard);

    // IIPCHandlerRequest overrides
    int send (OutPostcard * const outCard, const string & to);

private:
    IPCHandler (IIPCMessagingRequest * ipcHandle,
                MsgTask * msgTask);
    ~IPCHandler ();

    // Nested Types
    // Loc Messages

    struct LocMsgBase : public LocMsg {
        LocMsgBase (IPCHandler * parent);
        virtual ~LocMsgBase ();
        // Data members
        IPCHandler * mParent;
    };

    struct RegLocMsg : public LocMsgBase {
        RegLocMsg (IPCHandler * parent,
                   const string & name,
                   IDataItemSubscription * pOBIface);
        virtual ~RegLocMsg ();
        void proc () const;
        // Data members
        const string mName;
        IDataItemSubscription * mOBIface;
    };

    struct HandleIPCMsg : public LocMsgBase {
        HandleIPCMsg(IPCHandler * parent, InPostcard * const inCard);
        ~HandleIPCMsg();

        void proc() const;

        // Data members
        InPostcard *mIncard;

        // Helpers
        void getListOfDataItems
        (
            InPostcard * const inCard,
            list <DataItemId> & out
        ) const;
    };

    // Helpers

    // Data members
    static IPCHandler                     * mInstance;
    IIPCMessagingRequest                  * mIpcHandle;
    MsgTask                               * mMsgTask;
    IDataItemSubscription                 * mOBIface;
    map <string, IDataItemObserver *>       mCallBackTable;
};

}

#endif // #ifndef __IZAT_MANAGER_IPCHANDLER_H__
