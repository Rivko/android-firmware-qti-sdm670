/*==============================================================================
*  wfd_cfg_utils.cpp
*
*  DESCRIPTION:
*       Helper module to fetch Configurable items for wfd
*
*
*  Copyright (c) 2017 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
*===============================================================================
*/

/* =============================================================================
                     INCLUDE FILES FOR MODULE
================================================================================
*/

#include "wfd_cfg_utils.h"
#include "WFDMMLogs.h"
#include "AEEStdDef.h"

#ifdef __cplusplus
extern "C" {
#endif

void getParameter(const char* key, void* value)
{
    int result =0;

    if (0 == strcmp("AudioThreadPriority",key)) {
        int *val = (int*) value;
        *val = result = AUDIO_SOURCE_THREAD_PRIORITY;
    }
    else if (0 == strcmp("MuxThreadPriority",key)) {
        int *val = (int*) value;
        *val = result = MUX_THREAD_PRIORITY;
    }
    else if (0 == strcmp("RTPXmitIPTTL",key)) {
        uint8 *val = (uint8*) value;
        result = *val = RTP_PACKET_XMIT_IP_TTL;
    }
    else if (0 == strcmp("RTPXmitUDPBuffSize",key)) {
        int *val = (int*) value;
        *val = result = RTP_PACKET_XMIT_UDP_BUFFERSIZE;
    }
    else if (0 == strcmp("RTPXmitTCPBuffSize",key)) {
        int *val = (int*) value;
        *val = result = RTP_PACKET_XMIT_TCP_BUFFERSIZE;
    }
    else if (0 == strcmp("RTPXmitTID",key)) {
        int *val = (int*) value;
        *val = result = RTP_PACKET_XMIT_TID;
    }
    else {
        WFDMMLOGH("getParameter: Invalid key !!");
    }

    WFDMMLOGH2("getParameter:: key = %s value  = %d", key, result);
}

#ifdef __cplusplus
}
#endif


