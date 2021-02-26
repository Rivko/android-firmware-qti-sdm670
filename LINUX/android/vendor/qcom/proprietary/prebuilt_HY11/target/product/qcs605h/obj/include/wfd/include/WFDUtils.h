#ifndef _WFD_UTILS_H_
#define _WFD_UTILS_H_

/*==============================================================================
*       WFDUtils.h
*
*  DESCRIPTION:
*       Header file for WFDUtils
*
*
*  Copyright (c) 2014-2018 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
*===============================================================================
*/
/*==============================================================================
                             Edit History
================================================================================
   When            Who           Why
-----------------  ------------  -----------------------------------------------
02/19/2014                    InitialDraft
================================================================================
*/

/*==============================================================================
**               Includes and Public Data Declarations
**==============================================================================
*/

/* =============================================================================

                     INCLUDE FILES FOR MODULE

================================================================================
*/
#include "AEEstd.h"
#include "OMX_Core.h"

#define UNUSED_VAR(x) (void)x


#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------------
  Macro definitions
--------------------------------------------------------------------------------
*/

#ifdef WFD_LIKELY
#undef WFD_LIKELY
#endif

#define WFD_LIKELY( exp )    (__builtin_expect( !!(exp), 1 ))

#ifdef WFD_UNLIKELY
#undef WFD_UNLIKELY
#endif

#define WFD_UNLIKELY( exp )  (__builtin_expect( !!(exp), 0 ))

/*----------------------------------------------------------------------
 A structure for storing custom information in an OMX_BUFFERHEADERTYPE
------------------------------------------------------------------------
*/
#define UNUSED_VAR(x) (void)x
typedef enum operatonMode{
    WFD_OPERATION_MIRRORING = 1,
    WFD_OPERATION_STREAMING,
}OperationType;


struct buff_hdr_extra_info
{
    OMX_TICKS   nCaptureTime; // used by mux (must be 1st element always).
    OMX_TICKS   nEncDelTime;
    OMX_TICKS   nEncRcvTime;
    OMX_TICKS   nEncryptTime;
    OMX_TICKS   nMuxDelTime;
    OMX_TICKS   nMuxRcvTime;
    OMX_S64     nFrameNo;
    OMX_BOOL    bPushed;
    OMX_BOOL    bBlackFrame;
    OperationType eMode;
    OMX_BOOL      bEncryption;
    OMX_BOOL      bSecure;
    OMX_BOOL      bFd;
    OMX_S64       nFd;
    char*         pVirtAddr;
    void*         pData;
};

struct buff_hdr_extra_info_sink
{
    uint64   nMediaSrcRcvTime;
    uint64   nDecryptTime;
    uint64   nDecIPTime;
    uint64   nDecOPTime;
    uint64   nSchedDelayTime;
    uint64   nRenderSchedTime;//when we call queue()
    uint64   nRenderQTime;//queue()'s latency
    uint64   nRenderTime;
    uint64   nFrameNo;
};

void GetCurTime(OMX_TICKS& lTtime);

int SleepUs(int timeMs);

int setThreadName(const char* name);

void getThreadName(char* nameBuff, int len);

void setThreadPriority(int prio);

const char* getSchedPolicyName();

int setSchedPolicy(int policy);

unsigned long getTimerSlackValue();

unsigned long setTimerSlackValue(unsigned long slackVal);

int getMaxBitSet(uint64 word);

const char* getWFDDebugDumpLocation();

bool queryUBWCSupport();

#ifdef __cplusplus
}
#endif

#endif //_WFD_UTILS_H_
