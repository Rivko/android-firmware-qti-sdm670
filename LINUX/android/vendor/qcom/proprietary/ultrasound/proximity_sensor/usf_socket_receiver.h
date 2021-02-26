/*===========================================================================
                           usf_socket_receiver.h

DESCRIPTION: Class to wrap socket receive calls.

Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/
#ifndef __USF_SOCKET_RECEIVER_H__
#define __USF_SOCKET_RECEIVER_H__

/*-----------------------------------------------------------------------------
 Includes
 -----------------------------------------------------------------------------*/
#include <sys/un.h>
#include <unistd.h>

/*-----------------------------------------------------------------------------
  Classes
 -----------------------------------------------------------------------------*/
/**
  Unix socket receiver:
  Connects to a socket as a listener, receives packets of int32_t data.
*/
class UnSocketReceiver {
public:
    UnSocketReceiver();
    virtual ~UnSocketReceiver();
    virtual int open(const char *socketPath);
    virtual int connect();
    virtual int receivePacket(int32_t payload[], int payloadSize);
    virtual int getFd();
    virtual int close();
private:
    int sockFd;
    sockaddr_un sockAddr;
};

#endif // __USF_SOCKET_RECEIVER_H__
