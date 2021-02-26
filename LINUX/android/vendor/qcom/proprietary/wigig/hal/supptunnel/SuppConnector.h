/*********************************************************************
 Copyright (c) 2017 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
**********************************************************************/
#ifndef _SuppConnector_h
#define _SuppConnector_h

#include <utils/RefBase.h>

using namespace android;

#define SUPP_REPLY_BUF_SIZE 4096 // wpa_supplicant's maximum size.

/**
 * An interface for receiving callbacks from supplicant
 * connector
 */
class ISuppConnectorCallback {
public:
    virtual ~ISuppConnectorCallback() {}
    virtual void onEvent(char *buf, size_t buflen) = 0;
};

/**
 * A generic interface for connecting to wpa_supplicant/hostapd
 * instance
 */
class SuppConnector : public RefBase {
public:
    virtual ~SuppConnector() {}
    virtual int startSupplicant() = 0;
    virtual int killSupplicant() = 0;
    virtual int sendCommand(const char *cmd, char *reply, size_t *replylen) = 0;
    virtual int registerCallback(ISuppConnectorCallback *callback) = 0;
    virtual int setConfigOption(const char *name, const char *value) = 0;
};

#endif /* _SuppConnector_h */
