#ifndef _WFD_CFG_UTILS_H
#define _WFD_CFG_UTILS_H

/*==============================================================================
*       wfd_cfg_utils.h
*
*  DESCRIPTION:
*       Configurable items for wfd
*
*
*  Copyright (c) 2017 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
*===============================================================================
*/

#define AUDIO_SOURCE_THREAD_PRIORITY -19 //as per current configuration in AudioSource
#define MUX_THREAD_PRIORITY -19 // as per current configuration of Mux
#define RTP_PACKET_XMIT_IP_TTL 255
#define RTP_PACKET_XMIT_UDP_BUFFERSIZE 1024 * 1024
#define RTP_PACKET_XMIT_TCP_BUFFERSIZE 5 * 1024 * 1024
#define RTP_PACKET_XMIT_TID 6 << 5 //Voice TID = 6 & BestEffort TID = 0

#endif

