/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  IPCClient implementation

  Copyright  (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

#define LOG_NDEBUG 0
#define LOG_TAG "IPCClient"

#include <algorithm>
#include <iterator>
#include <string>
#include <log_util.h>
#include <MsgTask.h>
#include <IzatDefines.h>
#include <DataItemId.h>
#include <DataItemsFactory.h>
#include <postcard.h>
#include <IPCHandler.h>
#include <IPCClient.h>

using namespace std;
using namespace izat_manager;
using namespace qc_loc_fw;

// ctors
IPCClient :: IPCClient (const string &clientName,
                     MsgTask * msgTask, IPCHandler *ipcHandler)
:
mClientName(clientName),
mMsgTask (msgTask),
mIpcHandler(ipcHandler)
{}

IPCClient :: LocMsgBase :: LocMsgBase (IPCClient * parent)
: mParent (parent) {}

IPCClient :: NotifyLocMsg :: NotifyLocMsg
 (IPCClient * parent, list <IDataItemSerialization *> dlist)
:
LocMsgBase (parent), mDList (dlist)
{}


// dtors
IPCClient :: ~IPCClient () {
}

IPCClient :: LocMsgBase :: ~LocMsgBase () {}

IPCClient :: NotifyLocMsg :: ~NotifyLocMsg () {
    for (auto di : mDList) {
        if (nullptr != di) {
            delete di;
            di = NULL;
        }
    }
}

void IPCClient :: NotifyLocMsg :: proc () const {

    ENTRY_LOG ();
    int result = 0;
    OutPostcard * outcard = NULL;

    do {
        if (mDList.empty ()) {
            result = 1;
            break;
        }

        outcard = OutPostcard :: createInstance ();
        BREAK_IF_ZERO (2, outcard);
        BREAK_IF_NON_ZERO (3, outcard->init ());
        BREAK_IF_NON_ZERO (4, outcard->addString ("FROM", "OS-Agent"));
        BREAK_IF_NON_ZERO (5, outcard->addString ("TO", this->mParent->mClientName.c_str ()));
        BREAK_IF_NON_ZERO (8, outcard->addString ("INFO", "OS-STATUS-UPDATE"));

        for (auto di : mDList) {
            if (nullptr != di) {
                di->serialize(outcard);
            }
        }

        BREAK_IF_NON_ZERO (6, outcard->finalize ());
        BREAK_IF_NON_ZERO (7, this->mParent->mIpcHandler->send (outcard,
            this->mParent->mClientName.c_str ()));
    } while (0);

    delete outcard;
    EXIT_LOG_WITH_ERROR ("%d", result);
}

void IPCClient :: getName (string & name) {
    name = mClientName;
}

void IPCClient :: notify (const list <IDataItemCore *> & dlist) {
    int result = 0;
    ENTRY_LOG ();
    do {
        if (dlist.empty ()) {
            result = 0;
            break;
        }

        list <IDataItemSerialization *> dISerializerList;
        for (auto dItem : dlist) {
            if (nullptr != dItem) {
                IDataItemSerialization *diSerializer =
                        DataItemsFactory :: cloneDataItem (dItem);
                BREAK_IF_ZERO (2, diSerializer);
                LOC_LOGD("DataItem serialized %d", dItem->getId());
                dISerializerList.push_back (diSerializer);
            }
        }
        LOC_LOGD("OSObserver Notification received for the following data items at IPCClient %s",
                mClientName.c_str());
        mMsgTask->sendMsg (new  (nothrow) NotifyLocMsg (this, dISerializerList));
    } while  (0);
    EXIT_LOG_WITH_ERROR ("%d", result);
}
