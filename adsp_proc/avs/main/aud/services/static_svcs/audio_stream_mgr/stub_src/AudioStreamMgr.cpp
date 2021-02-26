/**
@file AudStreamMgr.cpp

@brief This file contains the stubbed implementation for AudioStreamMgr.

*/


/*-----------------------------------------------------------------------
Copyright (c) 2010-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
-----------------------------------------------------------------------*/

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "AudioStreamMgr.h"
#include "AudioStreamMgr_Type.h"
#include "AudioStreamMgr_Util.h"
#include "AudioStreamMgr_DevIF.h"
#include "AudioStreamMgr_SessionCmdHandler.h"
#include "AudioStreamMgr_CustomCmdHandler.h"
#include "AudioStreamMgr_SessionRespHandler.h"
#include "AudioStreamMgr_StreamRespHandler.h"
#include "AudioStreamMgr_Session.h"
#include "AudioStreamMgr_SysAprCmdHandler.h"
#include "AudioStreamMgr_PrivateDefs.h"

#include "EliteMsg_Custom.h"
#include "AFEInterface.h"
#include "audio_basic_op_ext.h"

#include "AudioStreamMgr_AprIf.h"
#include "AudioStreamMgr_AprDataBase.h"
#include "EliteTopology_db_if.h"
#include "AudioStreamMgr_adsppm.h"
#include "aprv2_ids_services.h"

/* =======================================================================
**                          Function Definitions
** ======================================================================= */

//qurt_elite MemoryMap Client
uint32_t asmMemoryMapClient;

/**
 * This function returns the ASM service version
 */
ADSPResult asm_svc_get_api_version(avs_svc_api_info_t *svc_info_ptr)
{
  return ADSP_ENOTIMPL;
}

/* =======================================================================
**         AudioStreamMgr Instance Create/Destroy/Workloop
** ======================================================================= */

ADSPResult AudioStreamMgr_Create (uint32_t inputParam, void **handle)
{
    *handle = NULL;
    return ADSP_EOK;
}

ADSPResult AudioStreamMgr_FreeAprPkt(  elite_apr_packet_t* pPkt)
{
   return ADSP_EOK;
}

ADSPResult AudioStreamMgr_GenerateAck(  elite_apr_packet_t* pPkt,/// This is the received packet that requires ACK.
                                       ADSPResult status,
                                       void *pAckPayload,      /// payload that is required in ACK, specified by caller
                                       uint32_t ulSize, /// payload size.
                                       uint32_t    ulAckOpcode     /// Optonal The opcode for ACK.
                                    )
{
	   return ADSP_EOK;
}

ADSPResult AudioStreamMgr_GenerateClientEventFromCb( void  *pInCb,
                                               uint32_t eventOpcode,
                                               uint32_t token,
                                                void *pEventPayload,
                                                uint32_t ulPayloadSize)
{
		   return ADSP_EOK;
}