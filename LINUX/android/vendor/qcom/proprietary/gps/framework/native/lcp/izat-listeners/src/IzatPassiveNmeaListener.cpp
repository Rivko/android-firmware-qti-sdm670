/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#define LOG_NDEBUG 0
#define LOG_TAG "IzatSvc_PassiveNmeaListener"
#include <IzatDefines.h>
#include <IzatManager.h>
#include <IzatPassiveNmeaListener.h>
#include <Utils.h>

#define LOG_NDEBUG 0
#define LOG_TAG "IzatSvc_PassiveNmeaListener"
#include <log_util.h>

using namespace izat_manager;
using namespace std;



#define NMEA_TAG                   "NMEA-STRING"


IzatPassiveNmeaListener::IzatListenerMapType IzatPassiveNmeaListener::sObjects;

void IzatPassiveNmeaListener :: startPassiveNmeaListener(
    const std::string clientName,
    const IzatListenerMask listensTo,
    IIzatManager *izatManager,
    IPCMessagingProxy *iPCMessagingProxy) {
    int result = -1;
    do {
        if (NULL == sObjects[clientName]) {
            sObjects[clientName] =
                new(nothrow) IzatPassiveNmeaListener(clientName, listensTo, izatManager, iPCMessagingProxy);
            BREAK_IF_ZERO(1, sObjects[clientName]);
            result = 0;
        }
    } while (0);
    EXIT_LOG_WITH_ERROR("%d", result);
}

void IzatPassiveNmeaListener::stopAllPassiveNmeaListers(IIzatManager *izatManager) {
    for (IzatListenerMapType::iterator it = sObjects.begin(); it != sObjects.end(); it++) {
        if(0 != izatManager->unsubscribeListener(it->second)) {
            LOC_LOGI("Failed to unsubcribe nmea listener for %s", it->first.c_str());
        }
        delete it->second;
    }
}


// ctor
IzatPassiveNmeaListener :: IzatPassiveNmeaListener(const std::string clientName,
                                                           IzatListenerMask listensTo,
                                                           IIzatManager *izatManager,
                                                           IPCMessagingProxy *iPCMessagingProxy)
    : mClientName(clientName)
    , mIpcName(clientName + "-NMEA-LISTENER")
    , mListenerMask(listensTo)
    , mIPCMessagingProxy(iPCMessagingProxy) {
    int result = -1;

    do {
        BREAK_IF_ZERO(1, izatManager);
        izatManager->subscribeListener(this);
        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

// dtor
IzatPassiveNmeaListener :: ~IzatPassiveNmeaListener() { }
//overrides
IzatListenerMask IzatPassiveNmeaListener :: listensTo() const {
    return mListenerMask;
}

void IzatPassiveNmeaListener :: onNmeaChanged (const IzatNmea *nmea)
{
    ENTRY_LOG();
    OutPostcard *card = NULL;
    int result = 0;

    do {

        BREAK_IF_ZERO(1, nmea);
        LOC_LOGD("HasNmeaString = %d, NMEA = %s", nmea->isValid(), nmea->getNmea().c_str());
        BREAK_IF_ZERO(2, mIPCMessagingProxy);
        // send out the NMEA update
        card = OutPostcard::createInstance();
        BREAK_IF_ZERO(3, card);
        BREAK_IF_NON_ZERO(4, card->init());
        BREAK_IF_NON_ZERO(5, card->addString("TO", mClientName.c_str()));
        BREAK_IF_NON_ZERO(6, card->addString("FROM", mIpcName.c_str()));
        BREAK_IF_NON_ZERO(7, card->addString("INFO", "NMEA-UPDATE"));
        //NMEA String
        BREAK_IF_NON_ZERO(8, card->addString(NMEA_TAG, nmea->getNmea().c_str()));

        BREAK_IF_NON_ZERO(9, card->finalize());
        BREAK_IF_NON_ZERO(10, mIPCMessagingProxy->sendMsg(card, mClientName.c_str()));
    } while(0);

    delete card;

    EXIT_LOG_WITH_ERROR("%d", result);
}

