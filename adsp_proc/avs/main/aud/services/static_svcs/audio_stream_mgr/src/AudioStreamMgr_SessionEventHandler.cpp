/**
@file AudStreamMgr_SessionRespHandler.cpp

@brief This file contains the implementation for session response handler functions
for AudioStreamMgr.

 */

/*========================================================================
$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/audio_stream_mgr/src/AudioStreamMgr_SessionEventHandler.cpp#1 $

Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
01/22/16   rb      Created file.
==========================================================================*/

/*-----------------------------------------------------------------------
Copyright (c) 2013-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
-----------------------------------------------------------------------*/




/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "AudioStreamMgr_Type.h"
#include "AudioStreamMgr_Session.h"
#include "AudioStreamMgr_DevIF.h"
#include "AudioStreamMgr_Util.h"
#include "AudioStreamMgr_StreamUtil.h"
#include "AudioStreamMgr_AprIf.h"
#include "AudioStreamMgr_AprDataBase.h"
#include "AudioStreamMgr_Session.h"
#include "AudioStreamMgr_SessionRespHandler.h"
#include "AudioStreamMgr_StreamRespHandler.h"
#include "AudioStreamMgr_EliteMsg_Custom.h"
#include "adsp_media_fmt.h"

#include "EliteMsg_Custom.h"
#include "EliteMsg_AfeCustom.h"
#include "EliteMsg_AdmCustom.h"

#include "AudDynaPPSvc.h"
#include "AudioDecSvc.h"
#include "AudioEncSvc.h"
#include "SwitchSvc.h"
#include "StreamRouterSvc.h"

#include "MixerSvc_Interface.h"
#include "AudioStreamMgr_adsppm.h"

/*--------------------------------------------------------------*/
/* Macro definitions                                            */
/* -------------------------------------------------------------*/

/* -----------------------------------------------------------------------
 ** Constant / Define Declarations
 ** ----------------------------------------------------------------------- */

/* =======================================================================
 **                          Function Definitions
 ** ======================================================================= */

ADSPResult AudioStreamMgr_SessionRespQListenToEoSAck(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession )
{
   EliteEvent_CustomASMListenToEoSAck *pPayload = (EliteEvent_CustomASMListenToEoSAck *) pSession->msg.pPayload;

   if(AUDIO_STREAM_MGR_SESSION_CLOSE != pSession->unState)
   {
      uint8_t stream_id = ASM_GetStreamIdFromClientToken(pPayload->unClientToken);
      ASM_GraphNodeIndex node_ind = ASM_GetNodeIndexFromClientToken(pPayload->unClientToken);

      AudioStreamMgr_StreamStateType *pStream = ASM_GetStreamInstance(pSession, stream_id);
      if(NULL == pStream)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
               "AudioStreamMgr [%u, %u] received listen to EoS Ack request on invalid stream!",
               pSession->ucInternalSessionId,stream_id);
         return ADSP_EFAILED;
      }

      MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
            "AudioStreamMgr [%u,%u]: Received a listen to EoS Ack event from node index %d. stream state %u",
            pSession->ucInternalSessionId, pStream->ucStreamId, node_ind, pStream->state);

      //accept this only in non-transitional/non-close states.
      if ( (pStream->state == ASM_STREAM_RUN) || (pStream->state == ASM_STREAM_PAUSE) || (pStream->state == ASM_STREAM_SUSPEND) )
      {
         pStream->wait_for_eos_v2_ack = TRUE;
      }

   }
   else
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "AudioStreamMgr : Invalid state to receive a listen to EoS Ack event. Ignoring it!");
   }

   return elite_msg_release_msg(&pSession->msg);
}



