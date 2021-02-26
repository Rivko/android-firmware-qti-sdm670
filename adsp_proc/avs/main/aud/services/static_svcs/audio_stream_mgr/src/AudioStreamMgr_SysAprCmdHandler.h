/**
@file AudioStreamMgr_SysAprCmdHandler.h***************
@brief This file declares system cmd handler functions for******
*      AudioStreamMgr.

*/

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/audio_stream_mgr/src/AudioStreamMgr_SysAprCmdHandler.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
07/06/2010 KK      Created file.
==========================================================================*/

/*-----------------------------------------------------------------------
Copyright (c) 2010-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
-----------------------------------------------------------------------*/

#ifndef AUDIO_STREAM_MGR_SYS_APR_CMD_HANDLER_H
#define AUDIO_STREAM_MGR_SYS_APR_CMD_HANDLER_H

/*-------------------------------------------------------------------------
Include Files
-------------------------------------------------------------------------*/

/// System
#include "qurt_elite.h"

/// Audio
#include "Elite.h"
#include "AudioStreamMgr_Type.h"


#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/*-------------------------------------------------------------------------
Preprocessor Definitions and Constants
-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
Type Declarations
-------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
Class Definitions
----------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
Function Declarations and Documentation
----------------------------------------------------------------------------*/

/* System cmd queue: Process all APR commands */

/**
* This function process the Scorpion command that is sent to the system command Queue.
* Any command destined to session 0 and stream 0 is a system command
* @param[out] pMe, This points to the instance of AudioStreamMgr.
* @return ADSPResult error codes.
*/
ADSPResult AudioStreamMgr_SysQAprPktHandler( AudioStreamMgr_t *pMe);


/**
* This function process the Scorpion ASM_CMD_ADD_TOPOLOGIES command
* that is sent to the system command Queue. Any command destined
* to session 0 and stream 0 is a system command
* @param[out] pMe, This points to the instance of AudioStreamMgr.
* @return ADSPResult error codes.
*/
ADSPResult AudioStreamMgr_AddTopologiesHandler(AudioStreamMgr_t *pMe);

ADSPResult AudioStreamMgr_RegisterForAdsppmVotes(AudioStreamMgr_t *pMe);
ADSPResult AudioStreamMgr_DeregisterFromAdsppmVotes(AudioStreamMgr_t *pMe);
ADSPResult AudioStreamMgr_GetAdsppmVotes(AudioStreamMgr_t *pMe);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef AUDIO_STREAM_MGR_SYS_APR_CMD_HANDLER_H

