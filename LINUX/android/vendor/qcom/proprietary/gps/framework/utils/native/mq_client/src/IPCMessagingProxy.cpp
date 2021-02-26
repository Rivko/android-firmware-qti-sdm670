/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  IPCMessagingProxy class serves as a hub for sending /receiving inter-process messages
  via location-mq. This class will have the connection to location-mq and will
  send/receive messages on behalf of other clients.

  Copyright (c) 2015 - 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Confidential and Proprietary.
=============================================================================*/

#define LOG_NDEBUG 0
#define LOG_TAG "IPCMessagingProxy"

#include <sys/types.h>
#include <unistd.h>
#include <mq_client/IPCMessagingProxy.h>
#include <log_util.h>
#include <mq_client/mq_client.h>

#define UNUSED(x) (void)(x)
#define BREAK_IF_ZERO(ERR,X) if(0==(X)) {result = (ERR); break;}
#define BREAK_IF_NON_ZERO(ERR,X) if(0!=(X)) {result = (ERR); break;}

using namespace std;
using namespace qc_loc_fw;

IPCMessagingProxy* IPCMessagingProxy::mInstance = NULL;

const string IPCMessagingProxy::mSelfName("IPCMessagingProxy-"+to_string(getpid()));
const string IPCMessagingProxy::mConnectionSocketName(MQ_SERVER_SOCKET_NAME);

IPCMessagingProxy* IPCMessagingProxy::getInstance()
{
    ENTRY_LOG();
    if(mInstance == NULL) {
        mInstance = new (nothrow) IPCMessagingProxy();
    }
    return mInstance;
}

void IPCMessagingProxy::destroyInstance()
{
    delete mInstance;
    mInstance = NULL;
}

IPCMessagingProxy::IPCMessagingProxy() :
    MqClientControllerBase(LOG_TAG, mSelfName.c_str(), mConnectionSocketName.c_str(), NULL),
    mInitialized(false), mMsgResponseMapMutex(0)
{
    int result = -1;
    ENTRY_LOG();
    do {

        int baseInitError = init();
        if (baseInitError != 0) {
            result = 1;
            LOC_LOGE("MQClientControllerBase::init failed : %d", baseInitError);
            break;
        }

        int baselaunchError = launch();
        if (baselaunchError != 0) {
            result = 2;
            LOC_LOGE("MQClientControllerBase::launch failed : %d", baselaunchError);
            break;
        }

        mMsgResponseMapMutex = Mutex::createInstance("IPCMsgProxyMsgRespMapMutex",false);
        if(0 == mMsgResponseMapMutex) {
            result = 3;
            LOC_LOGE("Cannot allocate mutex for mMsgResponseMapMutex");
            break;
        }

       result = 0;
       mInitialized = true;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

IPCMessagingProxy::~IPCMessagingProxy()
{
    int result = -1;
    ENTRY_LOG();

    do {

        if (mInitialized) {

            // Call baseClass kill that will insert a KILL message into the IPCHandler thread of this client
            // causing it to exit.
            kill();

            LOC_LOGI("Entering join of MqClientControllerBase handler thread");
            (void) join();
            LOC_LOGI("Exited join of MqClientControllerBase handler thread");
        }

        mMsgResponseMap.clear();
        mInitialized = false;

        delete mMsgResponseMapMutex; mMsgResponseMapMutex = 0;

        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

// MqClientControllerBase overrides
int IPCMessagingProxy::_init()
{
    int result = 0;

    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}

void IPCMessagingProxy::_process(qc_loc_fw::InPostcard * const in_card)
{
    int result = -1;

    do {
        BREAK_IF_ZERO(1, in_card);

        // read who the message is for and direct it
        const char *msgTo;
        BREAK_IF_NON_ZERO(2, in_card->getString("TO", &msgTo));

        AutoLock autolock(mMsgResponseMapMutex);
        MessageResponseMap::iterator it = mMsgResponseMap.find(msgTo);
        if (it != mMsgResponseMap.end()) {
            IIPCMessagingResponse *responseObj = it->second;
            responseObj->handleMsg(in_card);
        } else {
            // check if the message is for the proxy itself, it could be a message to broadcast
            if (0 == strcmp(msgTo, mSelfName.c_str())) {
                // check if the msg is a REGISTER_EVENT message
                // If it is then just broadcast it all its clients.
                const char *infoMsg;
                BREAK_IF_NON_ZERO(3, in_card->getString("INFO", &infoMsg));
                if (0 == strcmp(infoMsg, "REGISTER-EVENT")) {
                    broadcastToAllClients(in_card);
                }
            } else {
                result = 3;
                break;
            }
        }
        result = 0;
    } while(0);

    delete in_card;

    EXIT_LOG_WITH_ERROR("%d", result);
}

void IPCMessagingProxy::_shutdown()
{
    EXIT_LOG_WITH_ERROR("%d", 0);
}

void IPCMessagingProxy::handleKill(qc_loc_fw::InPostcard * const in_msg)
{
    UNUSED(in_msg);
    EXIT_LOG_WITH_ERROR("%d", 0);
}

IIPCMessagingResponse* IPCMessagingProxy::getResponseObj(const char* listener_name)
{
    IIPCMessagingResponse* obj = nullptr;

    AutoLock autolock(mMsgResponseMapMutex);
    MessageResponseMap::iterator it = mMsgResponseMap.find(listener_name);
    if (it != mMsgResponseMap.end()) {
        obj = it->second;
    }
    return obj;
}

IIPCMessagingResponse* IPCMessagingProxy::unregisterResponseObj(const char* listener_name)
{
    IIPCMessagingResponse* obj = nullptr;

    AutoLock autolock(mMsgResponseMapMutex);
    MessageResponseMap::iterator it = mMsgResponseMap.find(listener_name);
    if (it != mMsgResponseMap.end()) {
        obj = it->second;
        mMsgResponseMap.erase(it);
    }
    return obj;
}

// IIPCMessagingRequest overrides
int IPCMessagingProxy::registerResponseObj(const char* listener_name, IIPCMessagingResponse* respObj)
{
    int result = -1;

    ENTRY_LOG();

    do {
        AutoLock autolock(mMsgResponseMapMutex);
        if (mMsgResponseMap.find(listener_name) != mMsgResponseMap.end()){
            LOC_LOGD("Listener already registerd: %s", listener_name);
            break;
        }

        string listenerKeyName(listener_name);

        if (registerWithMessagingServer(listenerKeyName) == 0 ) {
            // insert into map
            mMsgResponseMap.insert(make_pair(listenerKeyName, respObj));
        } else {
            result = 3;
            break;
        }

        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}

int IPCMessagingProxy::sendMsg(OutPostcard * const out_card, const char* msgTo)
{
    int result = -1;
    do {

        BREAK_IF_ZERO(1, out_card);
        BREAK_IF_ZERO(2, msgTo);

        AutoLock autolock(mMsgResponseMapMutex);
        MessageResponseMap::iterator it = mMsgResponseMap.find(msgTo);
        if (it != mMsgResponseMap.end()) {
            // it is a local message
            sendLocalMessage(out_card);
        } else {
            // it is cross process message
            sendIpcMessage(out_card->getEncodedBuffer());
        }

        result = 0;
    } while(0);

    if (result != 0) {
        EXIT_LOG_WITH_ERROR("%d", result);
    }

    return result;
}

int IPCMessagingProxy::registerWithMessagingServer(string & listener_name)
{
    int result = -1;
    OutPostcard *card = NULL;

    ENTRY_LOG();

    do {
        if (mInitialized == false) {
            result = 0;
            break;
        }

        card = OutPostcard::createInstance();
        BREAK_IF_ZERO(102, card);
        BREAK_IF_NON_ZERO(103, card->init());
        BREAK_IF_NON_ZERO(104, card->addString("TO", "SERVER"));
        BREAK_IF_NON_ZERO(105, card->addString("FROM", mSelfName.c_str()));
        BREAK_IF_NON_ZERO(106, card->addString("REQ", "ADD-ALIAS"));
        BREAK_IF_NON_ZERO(107, card->addString("ALIAS", listener_name.c_str()));
        BREAK_IF_NON_ZERO(108, card->finalize());
        BREAK_IF_NON_ZERO(109, sendIpcMessage(card->getEncodedBuffer()));

        result = 0;
    } while (0);

    delete card;
    card = NULL;

    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}

int IPCMessagingProxy::broadcastToAllClients(qc_loc_fw::InPostcard * const in_card)
{
    int result = -1;
    ENTRY_LOG();

    do {
        BREAK_IF_ZERO(1, in_card);

        MessageResponseMap::iterator it = mMsgResponseMap.begin();
        for(; it != mMsgResponseMap.end(); it++) {
            IIPCMessagingResponse *responseObj = it->second;
            responseObj->handleMsg(in_card);
        }

        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}

