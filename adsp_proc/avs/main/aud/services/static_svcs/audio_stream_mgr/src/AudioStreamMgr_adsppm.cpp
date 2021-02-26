/**
@file AudioStreamMgr_adsppm.cpp***********************

@brief This file contains the implementation for ADSPPM REQUESTS (ADSP power manager)**
 *      in AudioStreamMgr_adsppm.cpp.**************************

 */

/*========================================================================
$Header $

Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
07/18/2012 RB      Created file.
==========================================================================*/

/*-----------------------------------------------------------------------
Copyright (c) 2012-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
-----------------------------------------------------------------------*/


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */

#include "qurt_elite.h"
#include "AudioStreamMgr_adsppm.h"
#include "AudioStreamMgr_Type.h"
#include "AudioStreamMgr_Session.h"
#include "AudioStreamMgr_Stream.h"
#include "adsp_media_fmt.h"
#include "AudioDecSvc.h"
#include "hwd_devcfg.h"
#include "EliteMsg.h"
#include "AudioStreamMgr_AprIf.h"
#include "AdspCoreSvc.h"

#if (ADSPPM_INTEGRATION==1)
#include "mmpm.h"
#endif //ADSPPM_INTEGRATION==1

//16 as per CR 518542
#define BW_ADSP_DDR__ADSP_LPM__SCALE_FACTOR 16
/* =======================================================================
 **                          Function Definitions
 ** ======================================================================= */

#ifdef ASM_ADSPPM_DEBUG
static MMPM_STATUS dereg(uint32_t clientId)
{
   printf("deregister mmpm for client id %u\n", (unsigned int)clientId);
   return MMPM_STATUS_SUCCESS;
}
static uint32_t reg(MmpmRegParamType *regParam)
{
   printf("register mmpm for core id %d\n", regParam->coreId);
   return regParam->coreId;
}
static MMPM_STATUS     req(uint32 clientId,
      MmpmRscExtParamType *pRscExtParam)
{
   printf("MMPM request from clientId %u\n", (unsigned int)clientId);
   return MMPM_STATUS_SUCCESS;
}
static MMPM_STATUS     rel(uint32 clientId,
      MmpmRscExtParamType *pRscExtParam)
{
   printf("MMPM release from clientId %u\n", (unsigned int)clientId);
   return MMPM_STATUS_SUCCESS;
}

#define MMPM_Deregister_Ext dereg
#define MMPM_Register_Ext reg
#define MMPM_Request_Ext req
#define MMPM_Release_Ext rel
#endif //STUB


#if (ADSPPM_INTEGRATION==1)

/**
 * gives the number of open sessions.
 */
static uint32_t ASM_GetNumSessionsActive(AudioStreamMgr_t *pMe);

/**
 * core id is not necessary for the latency request.
 */
static ADSPResult ASM_RequestReleaseLatency(AudioStreamMgr_t *pMe, uint32_t coreId, bool_t isRelease);

#endif //ADSPPM_INTEGRATION

#define ADSPPM_NULL_HANDLE 0

ADSPResult AudioStreamMgr_RegisterPm(AudioStreamMgr_t *pMe)
{
#if (ADSPPM_INTEGRATION==1)
   ADSPResult result = ADSP_EOK;
   MmpmRegParamType regParam;
   ASM_adsppm_t *asm_adsppm = &pMe->adsppm;

   regParam.rev             = MMPM_REVISION;
   regParam.instanceId      = MMPM_CORE_INSTANCE_0;
   regParam.pwrCtrlFlag     = PWR_CTRL_NONE; //PWR_CTRL_STATIC_DISPLAY, PWR_CTRL_THERMAL
   regParam.callBackFlag    = CALLBACK_NONE; //CALLBACK_STATIC_DISPLAY, CALLBACK_THERMAL, CALLBACK_REQUEST_COMPLETE
   regParam.MMPM_Callback   = NULL;
   regParam.cbFcnStackSize  = 0;

   regParam.coreId          = MMPM_CORE_ID_LPASS_ADSP; //no need to request power, register access.
   regParam.pClientName     = (char *)"ASM ADSP";

   result = qurt_elite_adsppm_wrapper_register(&regParam, &asm_adsppm->adsp_core_id, &asm_adsppm->adsppm_wrapper);
   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM register by ASM. Result %lu. Client id %lu", result, asm_adsppm->adsp_core_id);

   return result;
#else
   return ADSP_EOK;
#endif
}

ADSPResult AudioStreamMgr_UnRegisterPm(AudioStreamMgr_t *pMe)
{
   uint32_t client_id = pMe->adsppm.adsp_core_id;
   if ( !qurt_elite_adsppm_wrapper_is_registered(pMe->adsppm.adsp_core_id) )
   {
       return ADSP_EOK;
   }
   ADSPResult result = qurt_elite_adsppm_wrapper_deregister(&pMe->adsppm.adsp_core_id, &pMe->adsppm.adsppm_wrapper);

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM deregister by ASM. Result %lu. Client id %lu", result, client_id);

   return result;
}

ADSPResult AudioStreamMgr_RequestReleasePm(AudioStreamMgr_t *pMe)
{
#if (ADSPPM_INTEGRATION==1)

   if ( !qurt_elite_adsppm_wrapper_is_registered(pMe->adsppm.adsp_core_id) )
   {
       return ADSP_EOK;
   }

   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioStreamMgr_adsppm : Requesting/releasing resources from ADSP PM!");
   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"AudioStreamMgr_adsppm : CoreId for ADSP: %lu",pMe->adsppm.adsp_core_id);

   uint32_t numSessionsActive = ASM_GetNumSessionsActive(pMe);
   ADSPResult res = ADSP_EOK;

   if (numSessionsActive > 0)
   {
      res |= ASM_RequestReleaseLatency(pMe, pMe->adsppm.adsp_core_id, REQUEST_RESOURCE);
   }
   else
   {
      res |= ASM_RequestReleaseLatency(pMe, pMe->adsppm.adsp_core_id, RELEASE_RESOURCE);
   }

   if (ADSP_FAILED(res))
   {
      return ADSP_EFAILED;
   }
   return ADSP_EOK;
#else
   return ADSP_EOK;
#endif //#if (ADSPPM_INTEGRATION==1)
}

#if (ADSPPM_INTEGRATION==1)

/**
 * this is only for interrupts.
 *
 * it's advised that local LPASS clients are passing their latency requirements to sleep driver directly instead of using ADSPPM.
 * However, for now let's implement using ADSPPM. TODO: to use sleep driver.
 */
static ADSPResult ASM_RequestReleaseLatency(AudioStreamMgr_t *pMe, uint32_t coreId, bool_t isRelease)
{
   ADSPResult result = ADSP_EOK;
   if (ADSPPM_NULL_HANDLE == coreId)
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioStreamMgr_adsppm : voting for latency for an invalid core");
      return ADSP_EFAILED;
   }

   #define NUM_REQUEST 1
   MmpmRscParamType rscParam[NUM_REQUEST];
   MMPM_STATUS      retStats[NUM_REQUEST];
   rscParam[0].rscId                        = MMPM_RSC_ID_SLEEP_LATENCY;
   //maximum interrupt latency that a client can endure and still function correctly.
   if (coreId == pMe->adsppm.adsp_core_id)
   {
      rscParam[0].rscParam.sleepMicroSec       = SLEEP_LATENCY_US;
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM_Request invalid core id for latency %lu", coreId);
      return ADSP_EOK;
   }

   MmpmRscExtParamType reqParam;
   reqParam.apiType                    = MMPM_API_TYPE_SYNC;
   reqParam.numOfReq                   = NUM_REQUEST;
   reqParam.pExt                       = NULL;     //for future
   reqParam.pReqArray                  = rscParam;
   reqParam.pStsArray                  = retStats; //for most cases mmpmRes is good enough, need not check this array.
   reqParam.reqTag                     = 0;     //for async only

   bool_t req_or_rel_made = true;
   if (isRelease)
   {
      result = qurt_elite_adsppm_wrapper_release(coreId, &pMe->adsppm.adsppm_wrapper, &reqParam);
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM release latency by ASM (%lu). Result %lu", pMe->adsppm.adsp_core_id, result);
      pMe->adsppm.prev_latency_req_rel = FALSE;
   }
   else
   {
      if (pMe->adsppm.prev_latency_vote != rscParam[0].rscParam.sleepMicroSec)
      {
         result = qurt_elite_adsppm_wrapper_request(coreId, &pMe->adsppm.adsppm_wrapper, &reqParam);
         MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM request latency %lu us by ASM (%lu). Result %lu", rscParam[0].rscParam.sleepMicroSec, pMe->adsppm.adsp_core_id, result);
         pMe->adsppm.prev_latency_req_rel = TRUE;
      }
      else
      {
         req_or_rel_made = false;
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ASM Latency vote didn't change");
      }
   }

   if(ADSP_EOK != result)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADSPPM_Request for latency failed");
   }
   else
   {
      if (req_or_rel_made)
      {
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM_Request for latency success");

         pMe->adsppm.prev_latency_vote = isRelease?0:rscParam[0].rscParam.sleepMicroSec;

      }
   }

   return result;
}


static uint32_t ASM_GetNumSessionsActive(AudioStreamMgr_t *pMe)
{
   uint32_t unNumActiveSessions = 0;

   for (int i = 0; i < ASM_MAX_SUPPORTED_SESSION_ID; i++)
   {
      if ( (NULL != pMe->pSessions[i]) && ( ( AUDIO_STREAM_MGR_SESSION_CLOSE != (pMe->pSessions[i])->unState ) &&
         ( AUDIO_STREAM_MGR_SESSION_SUSPEND != (pMe->pSessions[i])->unState ) ) )
      {
         unNumActiveSessions ++;
      }
   }

   return unNumActiveSessions;
}
#endif //ADSPPM_INTEGRATION

