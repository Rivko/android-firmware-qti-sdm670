#ifndef _RTCPRECEIVER_H
#define _RTCPRECEIVER_H
/* =======================================================================
                              RTCPReceiver.h
DESCRIPTION
  Test class definition of IStreamPort based output interface


  Copyright (c) 2011 - 2012, 2017-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
========================================================================== */


/* =======================================================================
                             Edit History

$Header:

========================================================================== */

/* =======================================================================
**               Includes and Public Data Declarations
** ======================================================================= */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
#include <stdio.h>
#include "RTPEncoder.h"
#include "MMDebugMsg.h"
#include "MMMalloc.h"
#include "MMThread.h"

typedef enum RTCPState
{
    RTCP_STATE_RUNNING,
    RTCP_STATE_ERROR,
    RTCP_STATE_STOPPED,
    RTCP_STATE_IDLE
}RTCPStateType;



class CRTCPReceiver
{
public:
    CRTCPReceiver(int nSocket, RTPCallbackType pCallback, void *pUserData,
         bool isMulticast=false, bool isIPv6=false, uint32 multicastIP=0, uint8 multicastIPv6[]={0}, uint32 multicastIPv6Len=0);

    ~CRTCPReceiver();

private:
    static int RTCPThreadEntry(void *);
    int RTCPThread(void);
    int RecvPacket(uint8 *pData, uint32 nLen);

    RTCPStateType   m_eState;
    RTPCallbackType m_pCallback;
    void*           m_pUserData;
    MM_HANDLE       m_pRTCPThread;
    int32           m_nSockfd;
    bool            m_isIPv6;
    bool            m_isMulticast;
    uint32          m_multicastIP;
    uint8           m_multicastIPv6[16];
};

#endif //_RTCPRECEIVER_H
