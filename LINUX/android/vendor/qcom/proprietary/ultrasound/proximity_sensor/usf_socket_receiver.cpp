/*===========================================================================
                           usf_socket_receiver.cpp

 DESCRIPTION: Implements Unix socket receiver

 Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
 Qualcomm Technologies Proprietary and Confidential.
 =============================================================================*/
#define LOG_TAG "usf_socket_receiver"

/*-----------------------------------------------------------------------------
 Includes
 -----------------------------------------------------------------------------*/
#include "usf_socket_receiver.h"
#include "check_call.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <log/log.h>

/*-----------------------------------------------------------------------------
 Functions
 -----------------------------------------------------------------------------*/
/*============================================================================
 FUNCTION:  UnSocketReceiver::UnSocketReceiver
 ============================================================================*/
/**
 C'tor for socket receiver class
 */
UnSocketReceiver::UnSocketReceiver() :
        sockFd(-1) {
}

/*============================================================================
 FUNCTION:  UnSocketReceiver::~UnSocketReceiver
 ============================================================================*/
/**
 D'tor for socket receiver class, empty but necessary for polymorphism.
 */
UnSocketReceiver::~UnSocketReceiver() {

}

/*============================================================================
 FUNCTION:  UnSocketReceiver::open
 ============================================================================*/
/**
 Opens the path supplied by the argument, initializes the socketPath member.
 */
int UnSocketReceiver::open(const char *socketPath) {
    CHECK_CALL(sockFd = socket(AF_UNIX, SOCK_STREAM, 0));
    if (sockFd < 0) {
        return sockFd; // error
    }

    sockAddr.sun_family = AF_UNIX;
    strlcpy(sockAddr.sun_path, socketPath, sizeof(sockAddr.sun_path));
    return 0;
}

/*============================================================================
 FUNCTION:  UnSocketReceiver::connect
 ============================================================================*/
/**
 Attempts to connect to the opened socket.
 */
int UnSocketReceiver::connect() {
    return ::connect(sockFd, (sockaddr*) (&sockAddr), sizeof(sockAddr));
}

/*============================================================================
 FUNCTION:  UnSocketReceiver::receivePacket
 ============================================================================*/
/**
 Non-blocking receive; 0 if nothing to read.
 */
int UnSocketReceiver::receivePacket(int32_t payload[], int payloadSize) {
    int numRead = recv(sockFd, payload, payloadSize, MSG_DONTWAIT);
    if (numRead == EAGAIN || numRead == EWOULDBLOCK) {
        return 0;
    }
    return numRead;
}

/*============================================================================
 FUNCTION:  UnSocketReceiver::getFd
 ============================================================================*/
/**
 Returns socket FD, e.g., for pselect. Should not be used to close, receive
 data, etc.
 */
int UnSocketReceiver::getFd() {
    return sockFd;
}

/*============================================================================
 FUNCTION:  UnSocketReceiver::close
 ============================================================================*/
/**
 Closes the socket.
 */
int UnSocketReceiver::close() {
    return ::close(sockFd);
}
