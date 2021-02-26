/**
@file AudDevMgr_mmpm.cpp
@brief This file contains the implementation for MMPM REQUESTS  in AudDevMgr_mmpm.cpp.
 */
/*========================================================================
Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
02/27/12   KR      Non-backward compatible API changes for Badger.  
==========================================================================*/
/*-----------------------------------------------------------------------
Copyright (c) 2011-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
-----------------------------------------------------------------------*/
/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "qurt_elite.h"
#include "AudDevMgr_mmpm.h"
#include "adsp_vpm_api.h"
#include "adsp_media_fmt.h"
#include "AFEInterface.h"
#include "EliteTopology_db_if.h"
#include "EliteCmnTopology_db.h"
#include "AdspCoreSvc.h"

#if (ADSPPM_INTEGRATION==1)
#include "mmpm.h"
#endif

/* =======================================================================
 **                          Function Definitions
 ** ======================================================================= */

//Assuming steady state cache miss  15*1024 kBps = 15MBps per voice COPP
#define ONE_VOICECOPP_CPU_BW         (15360)

//72*1000*1.8 KIPS = 131 MCPS
#define SINGLE_VOICECOPP_NB_WB_KIPS_TOTAL            (72000)  

//95*1000*1.8 KIPS = 171 MCPS
#define SINGLE_VOICECOPP_FB_KIPS_TOTAL                   (95000)  

ADSPResult Adm_RegisterAdsppm(AdmStatSvc_InfoType *pAdmStatSvc)
{
   ADSPResult result = ADSP_EOK;
#if (ADSPPM_INTEGRATION==1)
   {
      MmpmRegParamType regParam;
      char threadname[16];
      qurt_thread_get_name(threadname, 16-1);

      regParam.rev             = MMPM_REVISION;
      regParam.instanceId      = MMPM_CORE_INSTANCE_0;
      regParam.pwrCtrlFlag     = PWR_CTRL_NONE; //PWR_CTRL_STATIC_DISPLAY, PWR_CTRL_THERMAL
      regParam.callBackFlag    = CALLBACK_NONE; //CALLBACK_STATIC_DISPLAY, CALLBACK_THERMAL, CALLBACK_REQUEST_COMPLETE
      regParam.MMPM_Callback   = NULL;
      regParam.cbFcnStackSize  = 0;

      regParam.coreId          = MMPM_CORE_ID_LPASS_ADSP; //no need to request power, register access.
      regParam.pClientName     = threadname;

      result = qurt_elite_adsppm_wrapper_register(&regParam, &pAdmStatSvc->ulAdsppmClientId, &pAdmStatSvc->adsppmClientPtr);

      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM register by ADM. Result %lu. Client id %lu", result, pAdmStatSvc->ulAdsppmClientId);
   }
#endif
   return result;
}

ADSPResult Adm_DeregisterAdsppm(AdmStatSvc_InfoType *pAdmStatSvc)
{
   ADSPResult result = ADSP_EOK;
   if ( !qurt_elite_adsppm_wrapper_is_registered(pAdmStatSvc->ulAdsppmClientId) )
   {
      return ADSP_EOK;
   }
   uint32_t client_id=pAdmStatSvc->ulAdsppmClientId;
   result = qurt_elite_adsppm_wrapper_deregister(&pAdmStatSvc->ulAdsppmClientId, &pAdmStatSvc->adsppmClientPtr);
   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM deregister by ADM. Result %lu. Client id %lu", result, client_id);
   return ADSP_EOK;
}

ADSPResult AudDevMgr_RequestHwResources(AdmStatSvc_InfoType *pAudStatAdmSvc)
{
#if (ADSPPM_INTEGRATION==1)
   ADSPResult                result = ADSP_EOK;
   AdmCalPort_Type         	*phCalPort;
   uint16_t                  unCoppID;
   MMPM_STATUS            	 mmpmReqResult[2];

   MmpmMppsReqType mmpmMppsParam;
   MmpmGenBwReqType BwReq;
   MmpmGenBwValType BwVal;
   MmpmRscParamType AdmRscParams[2];
   MmpmRscExtParamType AdmAdsppmReq;

   uint32_t unTotalKipsReq = 0, unAvgBusBWtReq = 0;
   uint32_t unTotalMipsReq =0; 
   
   if ( !qurt_elite_adsppm_wrapper_is_registered(pAudStatAdmSvc->ulAdsppmClientId) )
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM MMPM_Request ignored because ADM is not registered with ADSPPM or failed\n");
      return ADSP_EFAILED;
   }

   //Determine the Total KIPS and Floor KIPS
   for (unCoppID = 0; unCoppID < ADM_MAX_COPPS; unCoppID++)
   {
	   phCalPort = &pAudStatAdmSvc->deviceCalPort[unCoppID];
	   if (ADM_COPP_STATUS_ACTIVE == phCalPort->unCoppStatus &&
			   ((ADM_ULTRA_LOW_LATENCY_DEVICE_SESSION != phCalPort->unDevicePerfMode)
					   && (ADM_LOW_LATENCY_NO_PROC_DEVICE_SESSION != phCalPort->unDevicePerfMode)))
	   {

		   if(ADSP_EOK == elite_cmn_topo_db_is_active_topology_present(1 << AVCS_TOPO_CFG_AREA_VOICE_BIT, phCalPort->unCoppTopologyID))
		   {
			   if(ADM_SAMPLING_RATE_16K >= phCalPort->unEID1.unSampleRate)
			   {
				   //NB and WB case
				   unTotalKipsReq += SINGLE_VOICECOPP_NB_WB_KIPS_TOTAL;
			   }
			   else
			   {
				   //FB case
				   unTotalKipsReq += SINGLE_VOICECOPP_FB_KIPS_TOTAL;
			   }

			   unAvgBusBWtReq += ONE_VOICECOPP_CPU_BW;
		   }
	   }
   }

   if ( (pAudStatAdmSvc->prev_kpps_vote != unTotalKipsReq) ||
         (pAudStatAdmSvc->prev_bw_vote != unAvgBusBWtReq) )
   {
      pAudStatAdmSvc->prev_kpps_vote = unTotalKipsReq;
      pAudStatAdmSvc->prev_bw_vote = unAvgBusBWtReq;
      /* *
       * The voting calculation is completed in units of KIPS, however, the vote is done in
       * units of MIPS. Since all math is integer math we will lose fractional values when
       * dividing by 1000. Therefore, care must be taken to round up to the nearest multiple
       * of 1000 before casting the vote in MIPS.
       * */
      unTotalMipsReq = (unTotalKipsReq+999)/1000;

      //Configure the ADM MIPS Request
      mmpmMppsParam.mppsTotal                  = unTotalMipsReq;
      mmpmMppsParam.adspFloorClock             = 0;

      //Configure the ADM BW Request
      BwVal.busRoute.masterPort =  MMPM_BW_PORT_ID_ADSP_MASTER;
      BwVal.busRoute.slavePort =  MMPM_BW_PORT_ID_DDR_SLAVE;
      BwVal.bwValue.busBwValue.bwBytePerSec = (uint64_t)(unAvgBusBWtReq*1024);   //converting from KBps to Bps
      BwVal.bwValue.busBwValue.usagePercentage = 100;               //ToDO profile it to see if we really need 100%
      BwVal.bwValue.busBwValue.usageType = MMPM_BW_USAGE_LPASS_DSP;

      BwReq.numOfBw = 1;
      BwReq.pBandWidthArray = &BwVal;


      //Bundle the MIPS and BW request in a single request
      AdmRscParams[0].rscId = MMPM_RSC_ID_MPPS;
      AdmRscParams[0].rscParam.pMppsReq = &mmpmMppsParam;

      AdmRscParams[1].rscId = MMPM_RSC_ID_GENERIC_BW_EXT;
      AdmRscParams[1].rscParam.pGenBwReq = &BwReq;

      AdmAdsppmReq.apiType = MMPM_API_TYPE_SYNC; //ToDo explore ASync mode later
      AdmAdsppmReq.numOfReq = sizeof(AdmRscParams)/sizeof(AdmRscParams[0]);
      AdmAdsppmReq.pReqArray = AdmRscParams;
      AdmAdsppmReq.pStsArray = mmpmReqResult;
      AdmAdsppmReq.reqTag = 0; //ignored for sync operation
      AdmAdsppmReq.pExt = NULL; // reserved for future use by ADSPPM

      if((0 == mmpmMppsParam.mppsTotal) && (0 != BwVal.bwValue.busBwValue.bwBytePerSec))
      {
         AdmAdsppmReq.numOfReq = 1;
         AdmAdsppmReq.pReqArray = &AdmRscParams[0];
         result = qurt_elite_adsppm_wrapper_release(pAudStatAdmSvc->ulAdsppmClientId, &pAudStatAdmSvc->adsppmClientPtr, &AdmAdsppmReq);
         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM release MPPS by ADM (%lu). Result %lu", pAudStatAdmSvc->ulAdsppmClientId,result);

         AdmAdsppmReq.pReqArray = &AdmRscParams[1];
         result = qurt_elite_adsppm_wrapper_request(pAudStatAdmSvc->ulAdsppmClientId, &pAudStatAdmSvc->adsppmClientPtr, &AdmAdsppmReq);
         MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM request BW %lu KBPS by ADM (%lu). Result %lu", unAvgBusBWtReq, pAudStatAdmSvc->ulAdsppmClientId, result);
      }
      else if ((0 != mmpmMppsParam.mppsTotal) && (0 == BwVal.bwValue.busBwValue.bwBytePerSec))
      {
         AdmAdsppmReq.numOfReq = 1;
         AdmAdsppmReq.pReqArray = &AdmRscParams[0];
         result = qurt_elite_adsppm_wrapper_request(pAudStatAdmSvc->ulAdsppmClientId, &pAudStatAdmSvc->adsppmClientPtr, &AdmAdsppmReq);
         MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM request MPPS %lu by ADM (%lu). Result %lu", mmpmMppsParam.mppsTotal, pAudStatAdmSvc->ulAdsppmClientId, result);

         AdmAdsppmReq.pReqArray = &AdmRscParams[1];
         result = qurt_elite_adsppm_wrapper_release(pAudStatAdmSvc->ulAdsppmClientId, &pAudStatAdmSvc->adsppmClientPtr, &AdmAdsppmReq);
         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM release BW by ADM (%lu). Result %lu", pAudStatAdmSvc->ulAdsppmClientId,result);
      }
      else if((0 != mmpmMppsParam.mppsTotal) && (0 != BwVal.bwValue.busBwValue.bwBytePerSec))
      {
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Sending ADM MMPM_Request_Ext");
         result = qurt_elite_adsppm_wrapper_request(pAudStatAdmSvc->ulAdsppmClientId, &pAudStatAdmSvc->adsppmClientPtr, &AdmAdsppmReq);
         MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM request MPPS %lu and BW %lu KBPS by ADM (%lu). Result %lu", mmpmMppsParam.mppsTotal, unAvgBusBWtReq, pAudStatAdmSvc->ulAdsppmClientId, result);
      }
      else
      {
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Sending ADM MMPM_Release_Ext");
         result = qurt_elite_adsppm_wrapper_release(pAudStatAdmSvc->ulAdsppmClientId, &pAudStatAdmSvc->adsppmClientPtr, &AdmAdsppmReq);
         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM release BW and MPPS by ADM (%lu). Result %lu", pAudStatAdmSvc->ulAdsppmClientId,result);
      }
   }
   else
   {
      MSG(MSG_SSID_QDSP6, DBG_LOW_PRIO, "ADM MPPS & BW didnt change");
   }

   return result;
#else
   return ADSP_EOK;
#endif
}

