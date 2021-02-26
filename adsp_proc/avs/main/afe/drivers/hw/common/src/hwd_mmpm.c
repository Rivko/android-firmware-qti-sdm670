/*========================================================================
   This file contains AFE MMPM driver wrapper functions

  Copyright (c) 2009-2012 Qualcomm Technologies, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.
 
  $Header: //components/rel/avs.adsp/2.8.5/afe/drivers/hw/common/src/hwd_mmpm.c#1 $
 ====================================================================== */

/*==========================================================================
  Include files
  ========================================================================== */
#include "qurt_elite_types.h"
#include "mmpm.h"
#include "qurt_elite.h"
#include "adsp_error_codes.h"
#include "hwd_mmpm.h"
#include "rs_mmpm.h"
#include "avtimer_mmpm.h"
#include "AFEMmpm.h"

#if defined DEBUG || defined SIM
   #include <assert.h>
   #define HWD_MMPM_ASSERT(x) assert(x)
#else
   #define HWD_MMPM_ASSERT(x)
#endif


#define HWD_MMPM_LOW_LEVEL_DEBUG
#undef HWD_MMPM_LOW_LEVEL_DEBUG

#define HWD_MAX_LEN_CLIENT_NAME  32
#define HWD_MAX_BW_REQUEST       10
#define HWD_MAX_CLK_REQUEST      8

// power        : avt, resampler,
// register access : avt, resampler
// core clocks : avt (3 core clock), resampler (1 core clock)
// bw             : resampler ?
// latency       : resampler

/* client name will be in the format MMPM2_Test_<coreId>_<instanceId> */
static void HwdCreateClientName(char clientName[], MmpmCoreIdType coreId)
{
   switch(coreId)
   {
      case MMPM_CORE_ID_LPASS_ADSP:
      {
         strlcpy(clientName, "HWD_ADSP", HWD_MAX_LEN_CLIENT_NAME);
      }
      break;
      case MMPM_CORE_ID_LPASS_LPM:
      {
         strlcpy(clientName, "HWD_LPM", HWD_MAX_LEN_CLIENT_NAME);
      }
      break;
      case MMPM_CORE_ID_LPASS_DML:
      {
         strlcpy(clientName, "HWD_DML", HWD_MAX_LEN_CLIENT_NAME);
      }
      break;
      case MMPM_CORE_ID_LPASS_AIF:
      {
         strlcpy(clientName, "HWD_AIF", HWD_MAX_LEN_CLIENT_NAME);
      }
      break;
      case MMPM_CORE_ID_LPASS_AVSYNC:
      {
         strlcpy(clientName, "HWD_AVSYNC", HWD_MAX_LEN_CLIENT_NAME);
      }
      break;
      case MMPM_CORE_ID_LPASS_HWRSMP:
      {
         strlcpy(clientName, "HWD_HWRSMP", HWD_MAX_LEN_CLIENT_NAME);
      }
      break;
      case MMPM_CORE_ID_LPASS_SPDIF:
      {
         strlcpy(clientName, "HWD_SPDIF", HWD_MAX_LEN_CLIENT_NAME);
      }
      break;
      default:
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Unsupported coreId : %d", coreId );
         HWD_MMPM_ASSERT(0);
      }
   }
}

/* MMPM2 registration */
uint32 HwdMmpm_Register(MmpmCoreIdType coreId, MmpmCoreInstanceIdType instanceId)
{
   uint32            clientId = 0;
   MmpmRegParamType  mmpmRegParam;
   char              clientName[HWD_MAX_LEN_CLIENT_NAME];

   memset(&mmpmRegParam, 0,  sizeof(MmpmRegParamType));
   memset(clientName, 0, sizeof(clientName));
   mmpmRegParam.rev = MMPM_REVISION;
   mmpmRegParam.coreId = coreId;
   mmpmRegParam.instanceId = instanceId;
   mmpmRegParam.MMPM_Callback = NULL;
   HwdCreateClientName(clientName, coreId);
   mmpmRegParam.pClientName = clientName;
   mmpmRegParam.pwrCtrlFlag = PWR_CTRL_NONE;
   mmpmRegParam.callBackFlag = CALLBACK_NONE;

#if (ADSPPM_INTEGRATION==1)
   clientId = MMPM_Register_Ext(&mmpmRegParam);
#else
   clientId = 99;
#endif

   if (!clientId)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"HWD MMPM register FAILED for coreID %d instanceID %d\n", coreId, instanceId);
   }

   if(ADSP_FAILED(afe_mmpm_register_audio_client_class_and_dcvs_adjust(clientId)))
    {
     /*result is not printing as it has taken care in the above function definition*/
     MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error in registering either DCVS UP or Audio client class" );
    }

   return clientId;
}

/* MMPM2 Deregister */
ADSPResult HwdMmpm_Deregister(uint32 clientId)
{
   ADSPResult         rc = ADSP_EOK; 
   MMPM_STATUS       sts = MMPM_STATUS_FAILED;

   if (clientId)
   {
#if (ADSPPM_INTEGRATION==1)
      sts = MMPM_Deregister_Ext(clientId);
#else
      sts = MMPM_STATUS_SUCCESS;
#endif
      if (MMPM_STATUS_SUCCESS != sts)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"HWD MMPM Deregister FAILED for clientId %ld \n", clientId);
         rc = ADSP_EFAILED;
      }
   }
   else
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "HWD No valid MMPM client ID \n");
      rc = ADSP_EFAILED;
   }

   return rc;
}



/* Request MMPM2 for power Id(GDHS) */
ADSPResult HwdMmpm_RequestPwr(uint32 clientId)
{
   ADSPResult          rc = ADSP_EFAILED; 
   MMPM_STATUS        sts = MMPM_STATUS_FAILED;
   MmpmRscParamType   mmpmRscParam;
   MmpmRscExtParamType mmpmRscExtParam;

   if (0 != clientId)
   {
      memset(&mmpmRscParam, 0,  sizeof(MmpmRscParamType));
      memset(&mmpmRscExtParam, 0,  sizeof(MmpmRscParamType));
      mmpmRscParam.rscId = MMPM_RSC_ID_POWER;
      mmpmRscExtParam.apiType = MMPM_API_TYPE_SYNC;
      mmpmRscExtParam.numOfReq = 1;
      mmpmRscExtParam.pReqArray = &mmpmRscParam;
      mmpmRscExtParam.pStsArray = &sts;

#if (ADSPPM_INTEGRATION==1)
      sts = MMPM_Request_Ext(clientId, &mmpmRscExtParam);
#else
      sts = MMPM_STATUS_SUCCESS;
#endif

      if (MMPM_STATUS_SUCCESS != sts)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "HWD MMPM Power(GDHS) request FAILED for clientId %ld \n", clientId);
      }
      else
      {
         rc = ADSP_EOK;
      }
   } 
   else
   {
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "HWD No valid MMPM client ID \n");
   }

   return rc;
}


/* MMPM2 power (gdhs) release */
ADSPResult HwdMmpm_ReleasePwr(uint32 clientId)
{  
   ADSPResult          rc = ADSP_EFAILED; 
   MMPM_STATUS       sts = MMPM_STATUS_FAILED;
   MmpmRscParamType  mmpmRscParam;
	MmpmRscExtParamType mmpmRscExtParam;
   
   if (0 != clientId)
   {
      memset(&mmpmRscParam, 0,  sizeof(MmpmRscParamType));
      memset(&mmpmRscExtParam, 0,  sizeof(MmpmRscParamType));
      mmpmRscParam.rscId = MMPM_RSC_ID_POWER;
      mmpmRscExtParam.apiType = MMPM_API_TYPE_SYNC;
      mmpmRscExtParam.numOfReq = 1;
      mmpmRscExtParam.pReqArray = &mmpmRscParam;
      mmpmRscExtParam.pStsArray = &sts;

#if (ADSPPM_INTEGRATION==1)
      sts = MMPM_Release_Ext(clientId, &mmpmRscExtParam);
#else
      sts = MMPM_STATUS_SUCCESS;
#endif

      if (MMPM_STATUS_SUCCESS != sts)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "HWD MMPM Power(gdhs) release FAILED for clientId %ld \n", clientId);
      }
      else
      {
         rc = ADSP_EOK;
      }
      
   }
   else
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "HWD No valid MMPM client ID \n");
   }

   return rc;
}


/* MMPM2 Request core clock */
ADSPResult HwdMmpm_RequestClk(uint32 clientId, MmpmClkIdLpassType clkId[], uint32 clkFreq[], uint32 numClk)
{
   ADSPResult               rc = ADSP_EFAILED;
   MMPM_STATUS             sts = MMPM_STATUS_FAILED;
   MmpmRscParamType        mmpmRscParam;
   MmpmClkReqType          mmpmClkReqType;
   MmpmClkValType          mmpmClkValType[HWD_MAX_CLK_REQUEST];
   MmpmClkIdArrayParamType mmpmClkIdArray;
   MmpmRscExtParamType     mmpmRscExtParam;
   int32 i;

   memset(&mmpmRscParam, 0,  sizeof(MmpmRscParamType));
   memset(&mmpmClkValType, 0, sizeof(MmpmClkValType));
   memset(&mmpmClkReqType, 0, sizeof(MmpmClkReqType));
   memset(&mmpmClkIdArray, 0, sizeof(MmpmClkIdArrayParamType));
   memset(&mmpmRscExtParam, 0,  sizeof(MmpmRscParamType));

   if(clientId)
   {
      for(i=0; i < numClk; i++)
      {
         mmpmClkValType[i].clkFreqHz = clkFreq[i];
         mmpmClkValType[i].clkId.clkIdLpass = (MmpmClkIdLpassType)clkId[i];
         mmpmClkValType[i].freqMatch = MMPM_FREQ_AT_LEAST;

#if defined(HWD_MMPM_LOW_LEVEL_DEBUG)
         // Log each clock request as a Info level.

         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"HWD MMPM Clk Request Clock Id=%ld Freq =%ld \n", mmpmClkValType[i].clkId.clkIdLpass, mmpmClkValType[i].clkFreqHz);
#endif

      }

      mmpmClkReqType.numOfClk = numClk; 
      mmpmClkReqType.pClkArray = mmpmClkValType;
      mmpmRscParam.rscId = MMPM_RSC_ID_CORE_CLK;      
      mmpmRscParam.rscParam.pCoreClk = &mmpmClkReqType;
      mmpmRscExtParam.apiType = MMPM_API_TYPE_SYNC;
      mmpmRscExtParam.numOfReq = 1;
      mmpmRscExtParam.pReqArray = &mmpmRscParam;
      mmpmRscExtParam.pStsArray = &sts;

#if (ADSPPM_INTEGRATION==1) 
      sts = MMPM_Request_Ext(clientId, &mmpmRscExtParam);
#else
      sts = MMPM_STATUS_SUCCESS;
#endif 

      if(MMPM_STATUS_SUCCESS != sts)
      {
         // TODO - add details of why it failed.
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "HWD MMPM Clk Request with Freq FAILED for clientId %ld, return %d\n", clientId, sts);
      }
      else 
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"HWD MMPM Clk Request Passed for clientId=%ld\n", clientId);
         rc = ADSP_EOK;
      }
  } 
  else
  {
     MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"HWD No valid MMPM client ID \n");
  }

  return rc;
}


/* MMPM2 core clock release */
ADSPResult HwdMmpm_ReleaseClk(uint32 clientId, MmpmClkIdLpassType clkId[], uint32 numClk)
{
   ADSPResult          rc = ADSP_EFAILED;
   MMPM_STATUS             sts = MMPM_STATUS_FAILED;
   MmpmRscParamType        mmpmRscParam;
   MmpmRscExtParamType     mmpmRscExtParam;
   MmpmClkIdArrayParamType mmpmClkIdArray;
   MmpmCoreClkIdType       coreClkId[HWD_MAX_CLK_REQUEST];
   int32 i;

   memset(&mmpmRscParam, 0,  sizeof(MmpmRscParamType));
   memset(&mmpmClkIdArray, 0, sizeof(MmpmClkIdArrayParamType));
   memset(&mmpmRscExtParam, 0,  sizeof(MmpmRscParamType));

   if(clientId)
   {
      for (i = 0; i < numClk;i++)
      {
         coreClkId[i].clkIdLpass = (MmpmClkIdLpassType)clkId[i];
      }
        
      mmpmClkIdArray.numOfClk = 1;
      mmpmClkIdArray.pClkIdArray = coreClkId;
      mmpmRscParam.rscId = MMPM_RSC_ID_CORE_CLK;
      mmpmRscParam.rscParam.pRelClkIdArray = &mmpmClkIdArray;
      mmpmRscExtParam.apiType = MMPM_API_TYPE_SYNC;
      mmpmRscExtParam.numOfReq = 1;
      mmpmRscExtParam.pReqArray = &mmpmRscParam;
      mmpmRscExtParam.pStsArray = &sts;

#if (ADSPPM_INTEGRATION==1)
      sts = MMPM_Release_Ext(clientId, &mmpmRscExtParam);
#else
      sts = MMPM_STATUS_SUCCESS;
#endif

      if (MMPM_STATUS_SUCCESS != sts)
      {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"HWD MMPM core clk release FAILED for clientId %ld\n", clientId);
      }
      else
      {
         rc = ADSP_EOK;
      }
   } 
   else
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "HWD No valid MMPM client ID \n");
   }
   
   return rc;
}


/* Request MMPM2 for register programming (AHB clk) */
ADSPResult HwdMmpm_RequestRegProg(uint32 clientId, MmpmRegProgMatchType match) /*uint32  match */
{
   ADSPResult          rc = ADSP_EFAILED; 
   MMPM_STATUS        sts = MMPM_STATUS_FAILED;
   MmpmRscParamType   mmpmRscParam;
	MmpmRscExtParamType mmpmRscExtParam;
   
   if(clientId)
   {
      memset(&mmpmRscParam, 0,  sizeof(MmpmRscParamType));
      memset(&mmpmRscExtParam, 0,  sizeof(MmpmRscParamType));
      mmpmRscParam.rscId = MMPM_RSC_ID_REG_PROG;
      mmpmRscParam.rscParam.regProgMatch = match ;
      mmpmRscExtParam.apiType = MMPM_API_TYPE_SYNC;
      mmpmRscExtParam.numOfReq = 1;
      mmpmRscExtParam.pReqArray = &mmpmRscParam;
      mmpmRscExtParam.pStsArray = &sts;

#if (ADSPPM_INTEGRATION==1)
      sts = MMPM_Request_Ext(clientId, &mmpmRscExtParam);
#else
      sts = MMPM_STATUS_SUCCESS;
#endif

      if (MMPM_STATUS_SUCCESS != sts)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "HWD MMPM clk register program request FAILED for clientId %ld\n", clientId);
      }
      else
      {
         rc = ADSP_EOK;
      }
   }
   else
   {
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "HWD No valid MMPM client ID \n");
   }

   return rc;
}


/* MMPM2 register programming release */
ADSPResult HwdMmpm_ReleaseRegProg(uint32 clientId)
{
   ADSPResult          rc = ADSP_EFAILED; 
   MMPM_STATUS        sts = MMPM_STATUS_FAILED;
   MmpmRscParamType   mmpmRscParam;
	MmpmRscExtParamType mmpmRscExtParam;

   if (0 != clientId)
   {
      memset(&mmpmRscParam, 0,  sizeof(MmpmRscParamType));
      memset(&mmpmRscExtParam, 0,  sizeof(MmpmRscParamType));
      mmpmRscParam.rscId = MMPM_RSC_ID_REG_PROG;
      mmpmRscExtParam.apiType = MMPM_API_TYPE_SYNC;
      mmpmRscExtParam.numOfReq = 1;
      mmpmRscExtParam.pReqArray = &mmpmRscParam;
      mmpmRscExtParam.pStsArray = &sts;

#if (ADSPPM_INTEGRATION==1)
      sts = MMPM_Release_Ext(clientId, &mmpmRscExtParam);
#else
      sts = MMPM_STATUS_SUCCESS;
#endif

      if (MMPM_STATUS_SUCCESS != sts)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "HWD MMPM Register Programming FAILED for clientId %ld \n", clientId);
      }
      else
      {
         rc = ADSP_EOK;
      }
   }
   else
   {
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"HWD No valid MMPM client ID \n");
   }

   return rc;
}



/* Request MMPM2 for sleep latency */
ADSPResult HwdMmpm_RequestSleepLatency(uint32 clientId, uint32 microSec)
{
   ADSPResult          rc = ADSP_EFAILED;  
   MMPM_STATUS        sts = MMPM_STATUS_FAILED;
   MmpmRscParamType   mmpmRscParam;
	MmpmRscExtParamType mmpmRscExtParam;

   if (0 != clientId)
   {  
      memset(&mmpmRscParam, 0,  sizeof(MmpmRscParamType));
      memset(&mmpmRscExtParam, 0,  sizeof(MmpmRscParamType));
      mmpmRscParam.rscId = MMPM_RSC_ID_SLEEP_LATENCY;
      mmpmRscParam.rscParam.sleepMicroSec = microSec;
      mmpmRscExtParam.apiType = MMPM_API_TYPE_SYNC;
      mmpmRscExtParam.numOfReq = 1;
      mmpmRscExtParam.pReqArray = &mmpmRscParam;
      mmpmRscExtParam.pStsArray = &sts;

#if (ADSPPM_INTEGRATION==1)
      sts = MMPM_Request_Ext(clientId, &mmpmRscExtParam);
#else
      sts = MMPM_STATUS_SUCCESS;
#endif

      if (MMPM_STATUS_SUCCESS != sts)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MMPM Sleep Latency request FAILED for clientId %ld\n", clientId);
      }
      else
      {
         rc = ADSP_EOK;
      }
   } 
   else
   {
       MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "No valid MMPM client ID \n");
   }

   return rc;
}

/* MMPM2 sleep latency release */
ADSPResult HwdMmpm_ReleaseSleepLatency(uint32 clientId)
{
   ADSPResult          rc = ADSP_EFAILED; 
   MMPM_STATUS        sts = MMPM_STATUS_FAILED;
   MmpmRscParamType   mmpmRscParam;
   MmpmRscExtParamType mmpmRscExtParam;

   if (0 != clientId) 
   {
      memset(&mmpmRscParam, 0,  sizeof(MmpmRscParamType));
      memset(&mmpmRscExtParam, 0,  sizeof(MmpmRscParamType));
      mmpmRscParam.rscId = MMPM_RSC_ID_SLEEP_LATENCY;
      mmpmRscExtParam.apiType = MMPM_API_TYPE_SYNC;
      mmpmRscExtParam.numOfReq = 1;
      mmpmRscExtParam.pReqArray = &mmpmRscParam;
      mmpmRscExtParam.pStsArray = &sts;

#if (ADSPPM_INTEGRATION==1)
      sts = MMPM_Release_Ext(clientId, &mmpmRscExtParam);
#else
      sts = MMPM_STATUS_SUCCESS;
#endif

      if (MMPM_STATUS_SUCCESS != sts)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"HWD MMPM Sleep latency release FAILED for clientId %ld \n", clientId);
      }
      else
      {
         rc = ADSP_EOK;
      }
   }
   else
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "HWD No valid MMPM client ID \n");
   }

   return rc;
}



/*  ADSPPM bandwidth request  */
ADSPResult HwdMmpm_RequestBw(uint32 clientId, HwdBwReqType *bw /* or MmpmGenBwValType*/, uint32 numBw)
{
   ADSPResult          rc = ADSP_EFAILED; 
   MMPM_STATUS   sts = MMPM_STATUS_FAILED;
   MmpmRscParamType    mmpmRscParam;
   MmpmGenBwReqType    mmpmBwReqParam;
   MmpmGenBwValType    bandWidthArray[HWD_MAX_BW_REQUEST];
   uint32 i;
   MmpmRscExtParamType mmpmRscExtParam;
   
   if (0 != clientId)
   {
      memset(&mmpmRscParam, 0,  sizeof(MmpmRscParamType));
      memset(&mmpmBwReqParam, 0,  sizeof(mmpmBwReqParam));
      memset(bandWidthArray, 0,  HWD_MAX_BW_REQUEST * sizeof(MmpmGenBwValType));
      memset(&mmpmRscExtParam, 0,  sizeof(MmpmRscParamType));

      for (i = 0;i < numBw;i++)
      {
         bandWidthArray[i].busRoute.masterPort = bw[i].masterPort;
         bandWidthArray[i].busRoute.slavePort = bw[i].slavePort;
         bandWidthArray[i].bwValue.busBwValue.bwBytePerSec = bw[i].bwVal;
         bandWidthArray[i].bwValue.busBwValue.usagePercentage = bw[i].usagePercent;
         bandWidthArray[i].bwValue.busBwValue.usageType= bw[i].usageType;
      }

      mmpmBwReqParam.numOfBw = numBw;
      mmpmBwReqParam.pBandWidthArray = bandWidthArray;
      mmpmRscParam.rscId = MMPM_RSC_ID_GENERIC_BW;
      mmpmRscParam.rscParam.pGenBwReq = &mmpmBwReqParam;
      mmpmRscExtParam.apiType = MMPM_API_TYPE_SYNC;
      mmpmRscExtParam.numOfReq = 1;
      mmpmRscExtParam.pReqArray = &mmpmRscParam;
      mmpmRscExtParam.pStsArray = &sts;

#if (ADSPPM_INTEGRATION==1)
      sts = MMPM_Request_Ext(clientId, &mmpmRscExtParam);
#else
      sts = MMPM_STATUS_SUCCESS;
#endif

      if (MMPM_STATUS_SUCCESS != sts)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"HWD MMPM bandwidth request FAILED for clientId %ld \n", clientId);
      }
      else
      {
         rc = ADSP_EOK;
      }
   }
   else
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"HWD No valid MMPM client ID \n");
   }

   return rc;
}

/* MMPM2 bandwidth release */
ADSPResult HwdMmpm_ReleaseBw(uint32 clientId)
{
   ADSPResult          rc = ADSP_EFAILED; 
   MMPM_STATUS        sts = MMPM_STATUS_FAILED;
   MmpmRscParamType   mmpmRscParam;
   MmpmRscExtParamType mmpmRscExtParam;

   if (0 != clientId) 
   {
      memset(&mmpmRscParam, 0,  sizeof(MmpmRscParamType));
      memset(&mmpmRscExtParam, 0,  sizeof(MmpmRscParamType));
      mmpmRscParam.rscId = MMPM_RSC_ID_GENERIC_BW;
      mmpmRscExtParam.apiType = MMPM_API_TYPE_SYNC;
      mmpmRscExtParam.numOfReq = 1;
		mmpmRscExtParam.pReqArray = &mmpmRscParam;
		mmpmRscExtParam.pStsArray = &sts;

#if (ADSPPM_INTEGRATION==1)
      sts = MMPM_Release_Ext(clientId, &mmpmRscExtParam);
#else
      sts = MMPM_STATUS_SUCCESS;
#endif

      if (MMPM_STATUS_SUCCESS != sts)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MMPM bandwidth release FAILED for clientId %ld \n", clientId);
      }
      else
      {
         rc = ADSP_EOK;
      }
   } 
   else
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"HWD No valid MMPM client ID \n");
   }

   return rc;
}


/* Request MMPM2 for MIPS */
ADSPResult HwdMmpm_RequestMips(uint32 clientId, uint32 mips, uint32 mipsPerTherad)
{
   ADSPResult         rc = ADSP_EFAILED; 
   MMPM_STATUS        sts = MMPM_STATUS_FAILED;
   MmpmMipsReqType    mmpmMipsParam;
   MmpmRscParamType   mmpmRscParam;
   MmpmRscExtParamType mmpmRscExtParam;

   if (0 != clientId) 
   {
      memset(&mmpmMipsParam, 0,  sizeof(MmpmMipsReqType));
      memset(&mmpmRscParam, 0,  sizeof(MmpmRscParamType));
      memset(&mmpmRscExtParam, 0,  sizeof(MmpmRscParamType));
      mmpmMipsParam.mipsTotal = mips;
      mmpmMipsParam.mipsPerThread = mipsPerTherad;
      mmpmMipsParam.codeLocation = MMPM_BW_PORT_ID_ADSP_MASTER;
      mmpmMipsParam.reqOperation = MMPM_MIPS_REQUEST_CPU_CLOCK_ONLY;

      mmpmRscParam.rscId = MMPM_RSC_ID_MIPS_EXT;
      mmpmRscParam.rscParam.pMipsExt = &mmpmMipsParam;
      mmpmRscExtParam.apiType = MMPM_API_TYPE_SYNC;
      mmpmRscExtParam.numOfReq = 1;
      mmpmRscExtParam.pReqArray = &mmpmRscParam;
      mmpmRscExtParam.pStsArray = &sts;

#if (ADSPPM_INTEGRATION==1)
      sts = MMPM_Request_Ext(clientId, &mmpmRscExtParam);
#else
      sts = MMPM_STATUS_SUCCESS;
#endif

      if (MMPM_STATUS_SUCCESS != sts)
      {
          MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "HWD MMPM MIPS request FAILED for clientId %ld\n", clientId);
      }
      else
      {
         rc = ADSP_EOK;
      }
   }
   else
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "HWD No valid MMPM client ID \n");   
   }

   return rc;
}


/* MMPM2 MIPS release */
ADSPResult HwdMmpm_ReleaseMips(uint32 clientId)
{
   ADSPResult          rc = ADSP_EFAILED; 
   MMPM_STATUS        sts = MMPM_STATUS_FAILED;
   MmpmRscParamType   mmpmRscParam;
   MmpmRscExtParamType mmpmRscExtParam;

   if (0 != clientId) 
   {
      memset(&mmpmRscParam, 0,  sizeof(MmpmRscParamType));
      memset(&mmpmRscExtParam, 0,  sizeof(MmpmRscParamType));
      mmpmRscParam.rscId = MMPM_RSC_ID_MIPS;
      mmpmRscExtParam.apiType = MMPM_API_TYPE_SYNC;
      mmpmRscExtParam.numOfReq = 1;
      mmpmRscExtParam.pReqArray = &mmpmRscParam;
      mmpmRscExtParam.pStsArray = &sts;

#if (ADSPPM_INTEGRATION==1)
      sts = MMPM_Release_Ext(clientId, &mmpmRscExtParam);
#else
      sts = MMPM_STATUS_SUCCESS;
#endif

      if (MMPM_STATUS_SUCCESS != sts)
      {
          MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "HWD MMPM MIPS release FAILED for clientId %ld \n", clientId);
      }
      else
      {
         rc = ADSP_EOK;
      }
   } 
   else
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "HWD No valid MMPM client ID \n");
   }
   
   return rc;
}

/* Set clk source */
//Note: This function does not enable clock
//      This function does not reset clk frequency
ADSPResult HwdMmpm_RequestClkDomain(uint32 clientId, HwdClkDomainReqType clk[], uint32 numClk)
{
    ADSPResult         rc = ADSP_EFAILED;
    MMPM_STATUS             sts = MMPM_STATUS_FAILED;
    MmpmRscParamType        mmpmRscParam;
    MmpmClkDomainReqType    mmpmClkDomainReqType;
    MmpmClkDomainType       mmpmClkDomainValType[HWD_MAX_CLK_REQUEST];
    uint32                  i;
    MmpmRscExtParamType     mmpmRscExtParam;

   
    memset(&mmpmRscParam, 0,  sizeof(MmpmRscParamType));
    memset(&mmpmClkDomainValType, 0, HWD_MAX_CLK_REQUEST *  sizeof(MmpmClkValType));
    memset(&mmpmClkDomainReqType, 0, sizeof(MmpmClkReqType));
    memset(&mmpmRscExtParam, 0,  sizeof(MmpmRscExtParamType));

    if (clientId)
    {
        for (i = 0; i < numClk; i++)
        {
            mmpmClkDomainValType[i].clkId.clkIdLpass = (MmpmClkIdLpassType)clk[i].clkId;
            mmpmClkDomainValType[i].clkFreqHz = clk[i].clkFreqHz;
            mmpmClkDomainValType[i].clkDomainSrc.clkDomainSrcIdLpass = clk[i].clkDomainSrc;

            MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "HWD MMPM Clk Domain request for clientId %ld clockId=%ld, clkFreqHz=%ld Hz, clkDomainSrc = %ld.\n", clientId, clk[i].clkId, clk[i].clkFreqHz, clk[i].clkDomainSrc);
        }
        mmpmClkDomainReqType.numOfClk = numClk; 
        mmpmClkDomainReqType.pClkDomainArray = mmpmClkDomainValType;
        mmpmRscParam.rscId = MMPM_RSC_ID_CORE_CLK_DOMAIN;      
        mmpmRscParam.rscParam.pCoreClkDomain = &mmpmClkDomainReqType;
        mmpmRscExtParam.apiType = MMPM_API_TYPE_SYNC;
        mmpmRscExtParam.numOfReq = 1;
        mmpmRscExtParam.pReqArray = &mmpmRscParam;
        mmpmRscExtParam.pStsArray = &sts;

#if (ADSPPM_INTEGRATION==1)
        sts = MMPM_Request_Ext(clientId, &mmpmRscExtParam);
#else
        sts = MMPM_STATUS_SUCCESS;
#endif

        if(MMPM_STATUS_SUCCESS != sts)
        {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "HWD MMPM Clk Domain request FAILED for clientId %ld\n", clientId);
        } 
        else 
        {
            rc = ADSP_EOK;
        }
    } 
    else
    {
        // This is an error in the test code.
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "HWD No valid MMPM client ID \n");
    }

    return rc;
}

/* Set clk source to default */
ADSPResult HwdMmpm_ReleaseClkDomain(uint32 clientId, uint32 relClkId[], uint32 numClk)
{
    ADSPResult         rc = ADSP_EFAILED; 
    MMPM_STATUS             sts = MMPM_STATUS_FAILED;
    MmpmRscParamType        mmpmRscParam;
    MmpmRscExtParamType     mmpmRscExtParam;
    MmpmClkIdArrayParamType mmpmClkIdArray;
    MmpmCoreClkIdType       coreClkId[HWD_MAX_CLK_REQUEST];
    uint32 i;

    memset(&mmpmRscParam, 0,  sizeof(MmpmRscParamType));
    memset(&mmpmClkIdArray, 0, sizeof(MmpmClkIdArrayParamType));
    memset(&mmpmRscExtParam, 0,  sizeof(MmpmRscExtParamType));

    if (clientId)
    {
        for (i = 0; i < numClk;i++)
        {
            coreClkId[i].clkIdLpass = (MmpmClkIdLpassType)relClkId[i];
        }
        mmpmClkIdArray.numOfClk = numClk ;
        mmpmClkIdArray.pClkIdArray = coreClkId;
        mmpmRscParam.rscId = MMPM_RSC_ID_CORE_CLK_DOMAIN;
        mmpmRscParam.rscParam.pRelClkIdArray = &mmpmClkIdArray;
        mmpmRscExtParam.apiType = MMPM_API_TYPE_SYNC;
        mmpmRscExtParam.numOfReq = 1;
        mmpmRscExtParam.pReqArray = &mmpmRscParam;
        mmpmRscExtParam.pStsArray = &sts;

#if (ADSPPM_INTEGRATION==1)
        sts = MMPM_Release_Ext(clientId, &mmpmRscExtParam);
#else
        sts = MMPM_STATUS_SUCCESS;
#endif

        if (MMPM_STATUS_SUCCESS != sts)
        {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "HWD MMPM Clk Domain release FAILED for clientId %ld\n", clientId);
        }
        else
        {
            rc = ADSP_EOK;
        }
    } 
    else
    {
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "HWD No valid MMPM client ID \n");
    }

    return rc;
}


