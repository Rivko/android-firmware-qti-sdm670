/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  IPCHandler Implementation

  Copyright  (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

#define LOG_NDEBUG 0
#define LOG_TAG "IPCHandler"

#include <map>
#include <log_util.h>
#include <IzatDefines.h>
#include <Utils.h>
#include <MsgTask.h>
#include <postcard.h>
#include <IIPCClientSubscription.h>
#include <mq_client/IPCMessagingProxy.h>
#include <IPCHandler.h>
#include <IPCClient.h>

using namespace izat_manager;
using namespace std;
using namespace qc_loc_fw;

class IPCClient;

IPCHandler* IPCHandler :: mInstance = NULL;

#define BREAK_AND_DELETE_IF_NON_ZERO(ERR,X,OBJ) \
if(0!=(X)) {result = (ERR); delete (OBJ); break;}

IPCHandler * IPCHandler :: getInstance
(
    IIPCMessagingRequest * ipcHandle,
    MsgTask * msgTask
)
{
    if (!mInstance) {
        mInstance = new  (nothrow) IPCHandler (ipcHandle, msgTask);
        LOC_LOGD ("IPCHandler Created");
    }
    return mInstance;
}

void IPCHandler :: destroyInstance () {
    delete mInstance;
    mInstance = NULL;
    LOC_LOGD ("IPCHandler Destroyed");
}

// ctors
IPCHandler :: IPCHandler
(
    IIPCMessagingRequest * ipcHandle,
    MsgTask * msgTask
)
: mIpcHandle (ipcHandle), mMsgTask (msgTask) {}

IPCHandler :: LocMsgBase :: LocMsgBase (IPCHandler * parent)
: mParent (parent) {}

IPCHandler :: RegLocMsg :: RegLocMsg
 (IPCHandler * parent, const string & name, IDataItemSubscription * pOBIface)
: LocMsgBase (parent), mName (name), mOBIface (pOBIface) {}


IPCHandler :: HandleIPCMsg :: HandleIPCMsg (IPCHandler * parent, InPostcard * const inCard)
: LocMsgBase (parent), mIncard (inCard) {}


// dtors
IPCHandler :: ~IPCHandler ()
{
    map <string, IDataItemObserver *> :: iterator citer = mCallBackTable.begin ();
    for (; citer != mCallBackTable.end (); ++citer) {
        if (citer->second != NULL) { delete citer->second; }
    }
    mCallBackTable.clear ();
}

IPCHandler :: LocMsgBase :: ~LocMsgBase () {}
IPCHandler :: RegLocMsg :: ~RegLocMsg () {}
IPCHandler :: HandleIPCMsg :: ~HandleIPCMsg() {
    if (mIncard) { delete mIncard; mIncard = NULL;}
}


// proc
void IPCHandler:: RegLocMsg :: proc () const
{
    int result = -1;
    ENTRY_LOG();

    do {
        // store the IDataItemsubscription pointer
        BREAK_IF_ZERO(1, mOBIface);
        this->mParent->mOBIface = mOBIface;

        // register self with IPCMessagingProxyObj here
        BREAK_IF_NON_ZERO(2,
            this->mParent->mIpcHandle->registerResponseObj(mName.c_str(), this->mParent));
        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

void IPCHandler:: HandleIPCMsg :: proc () const
{
    int result = -1;
    ENTRY_LOG();

    do {
        BREAK_IF_ZERO(1, mIncard);

        const char * tostr;
        const char * fromstr;
        BREAK_IF_NON_ZERO (2, mIncard->getString ("TO", &tostr));
        BREAK_IF_NON_ZERO (3, mIncard->getString ("FROM", &fromstr));
        LOC_LOGD ("%s  <------- %s", tostr, fromstr);

        const char * req;
        if (0 == mIncard->getString ("REQ", &req)) {
            LOC_LOGD ("REQ: %s", req);
            string request (req);
            if (this->mParent->mCallBackTable.end () ==
                this->mParent->mCallBackTable.find (fromstr)) {
                IPCClient *ipcClient = new IPCClient(fromstr,
                    this->mParent->mMsgTask, this->mParent);
                BREAK_IF_ZERO(4, ipcClient);
                this->mParent->mCallBackTable[fromstr] = ipcClient;
            }

            if (0 == request.compare ("SUBSCRIBE")) {
                list <DataItemId> out;
                getListOfDataItems (mIncard, out);
                out.sort ();
                this->mParent->mOBIface->subscribe (out,
                    this->mParent->mCallBackTable[fromstr]);
            } else if (0 == request.compare ("UPDATE-SUBSCRIPTION")) {
                list <DataItemId> out;
                getListOfDataItems (mIncard, out);
                out.sort ();
                this->mParent->mOBIface->updateSubscription (out,
                    this->mParent->mCallBackTable[fromstr]);
            } else if (0 == request.compare ("REQUEST-DATA")) {
                list <DataItemId> out;
                getListOfDataItems (mIncard, out);
                out.sort ();
                this->mParent->mOBIface->requestData (out,
                    this->mParent->mCallBackTable[fromstr]);
            } else if (0 == request.compare ("UNSUBSCRIBE")) {
                list <DataItemId> out;
                getListOfDataItems (mIncard, out);
                out.sort ();
                this->mParent->mOBIface->unsubscribe (out,
                    this->mParent->mCallBackTable[fromstr]);
            } else if (0 == request.compare ("UNSUBSCRIBE-ALL")) {
                this->mParent->mOBIface->unsubscribeAll (this->mParent->mCallBackTable[fromstr]);
            } else {
                result = 5;
                LOC_LOGE ("Cannot Handle this Request message");
                break;
            }
        } else if (0 == mIncard->getString ("INFO", &req)) {
            string request (req);
            LOC_LOGD ("INFO: %s", request.c_str ());
            if (0 == request.compare ("REGISTER-EVENT")) {
                const char * clientstr;
                BREAK_IF_NON_ZERO (6, mIncard->getString ("CLIENT", &clientstr));
                if (this->mParent->mCallBackTable.end () != this->mParent->mCallBackTable.find (clientstr)) {
                    this->mParent->mOBIface->unsubscribeAll (
                        this->mParent->mCallBackTable[clientstr]);
                }
            } else {
                result = 7;
                LOC_LOGE ("Cannot Handle this Info message");
                break;
            }
        } else {
            result = 8;
            LOC_LOGE ("Registered handler cannot handle this message");
            break;
        }
        result = 0;
    } while (0);

    EXIT_LOG_WITH_ERROR("%d", result);
}


void IPCHandler :: HandleIPCMsg :: getListOfDataItems
(
    InPostcard * const inCard,
    list <DataItemId> & out
) const
{
    int result = 0;
    ENTRY_LOG ();
    int numDataItems = 0;
    int * dataItems = NULL;
    do {
        BREAK_IF_ZERO (1, inCard);
        BREAK_IF_NON_ZERO (2, inCard->getArrayInt32 ("DATA-ITEMS", &numDataItems, dataItems));
        BREAK_IF_ZERO (3, numDataItems);

        dataItems = new  (std :: nothrow) int [numDataItems];
        BREAK_IF_ZERO (4, dataItems);
        BREAK_IF_NON_ZERO (5, inCard->getArrayInt32 ("DATA-ITEMS", &numDataItems, dataItems));

        for (int i = 0; i <numDataItems; i++) {
            out.push_back (static_cast <DataItemId> (dataItems [i]));
        }
    } while (0);

    delete  [] dataItems;
    EXIT_LOG_WITH_ERROR ("%d", result);
}

void IPCHandler :: handleMsg (InPostcard * const inCard) {
    int result = 0;
    ENTRY_LOG ();
    do {
        BREAK_IF_ZERO (1, inCard);
        InPostcard * copy = InPostcard::createInstance();
        BREAK_AND_DELETE_IF_NON_ZERO(2, copyInCard(inCard, copy), copy);
        mMsgTask->sendMsg(new (nothrow) HandleIPCMsg(this, copy));

    } while (0);
    EXIT_LOG_WITH_ERROR ("%d", result);
}

int IPCHandler :: send (OutPostcard * const outCard, const string & to) {
    int result = 0;
    ENTRY_LOG ();
    do {
        BREAK_IF_ZERO (1, outCard);
        BREAK_IF_NON_ZERO (2, mIpcHandle->sendMsg (outCard, to.c_str ()));
        LOC_LOGD ("Postcard Message sent to %s", to.c_str ());
    } while (0);
    EXIT_LOG_WITH_ERROR ("%d", result);
    return result;
}

void IPCHandler :: reg(const string & name, IDataItemSubscription * iOBface)
{
     int result = -1;
     ENTRY_LOG ();
     do {
         RegLocMsg * msg = new  (nothrow) RegLocMsg (this, name, iOBface);
         BREAK_IF_ZERO (1, msg);
         mMsgTask->sendMsg (msg);
         result = 0;
     } while (0);
     EXIT_LOG_WITH_ERROR ("%d", result);
}
