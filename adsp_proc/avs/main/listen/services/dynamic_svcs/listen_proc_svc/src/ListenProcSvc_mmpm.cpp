/**
  @file ListenProcSvc_mmpm.cpp
  @brief LSM power manager : Manages power related activities for LSM service
*/

/*==============================================================================
  Edit History

  $Header:

  when       who     what, where, why
  --------   ---     -------------------------------------------------------
  12/12/14   unni    Moving LSM MMPM from static to dynamic service

==============================================================================*/

/*==============================================================================
  Copyright (c) 2012-2016 QUALCOMM Technologies, Inc.  All Rights Reserved.
  QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

==============================================================================*/

/*------------------------------------------------------------------------------
 * Include Files
 *----------------------------------------------------------------------------*/
#include "qurt_elite.h"
#include "ListenProcSvc_mmpm.h"

/*------------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *----------------------------------------------------------------------------*/
#define LSM_NUM_REQUEST_MAX 1
#define ADSPPM_NULL_HANDLE  0
#define CLIENT_NAME_SESSION_ID_OFFSET  4
/* Marco for ceil div, alter. (x + y - 1)/y includes overflow*/
#define ceil_div(x, y) ((x)==0 ? 0 : 1 + (((x) - 1) / (y)))

#if (ADSPPM_INTEGRATION==1)
#include "mmpm.h"

/**
 * Registers a lsm session with mmpm and populate mmpm client info
 * @param mmpm_info_ptr - lsm session mmpm state variables
 * @param session_id - lsm session id
 *
 * @return adsp result status
 */
ADSPResult lsm_mmpm_register(lsm_mmpm_info_t *mmpm_info_ptr,
                             uint16_t session_id)
{
  char client_name[]  = "LSM[0]";
  MmpmRegParamType regParam;
  ADSPResult result = ADSP_EOK;

  if(NULL == mmpm_info_ptr)
  {
#ifdef LSM_DEBUG
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "lsm mmpm reg: NULL arg");
#endif
    return ADSP_EBADPARAM;
  }

  /* Unique name for each session */
  client_name[CLIENT_NAME_SESSION_ID_OFFSET] += session_id;

  regParam.rev             = MMPM_REVISION;
  regParam.instanceId      = MMPM_CORE_INSTANCE_0;
  regParam.pwrCtrlFlag     = PWR_CTRL_NONE;
  regParam.callBackFlag    = CALLBACK_NONE;
  regParam.MMPM_Callback   = NULL;
  regParam.cbFcnStackSize  = 0;

  regParam.coreId          = MMPM_CORE_ID_LPASS_ADSP;
  regParam.pClientName     = client_name;

    
  if(ADSP_EOK != (result = qurt_elite_adsppm_wrapper_register(&regParam,&mmpm_info_ptr->client_id,\
                                                              &mmpm_info_ptr->mmpm_client_ptr)))
  {

#ifdef LSM_DEBUG
     MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
           "MMPM registration failed  for LSM ADSP session id [%u] ",
           session_id);
#endif
  }
  else
  {

#ifdef LSM_DEBUG
    MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
          "MMPM registration success for LSM ADSP session id [%u] is [%u]",
          session_id, mmpm_info_ptr->client_id);
#endif
  }

  return result;
}

/**
 * De-registers from mmpm using mmpm client info
 * @param mmpm_info_ptr - lsm session mmpm state variables
 *
 * @return adsp result status
 */
ADSPResult lsm_mmpm_deregister(lsm_mmpm_info_t *mmpm_info_ptr)
{
  ADSPResult result = ADSP_EOK;

  if(NULL == mmpm_info_ptr)
  {
#ifdef LSM_DEBUG
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "lsm mmpm dereg: NULL arg");
#endif
    return ADSP_EBADPARAM;
  }

  if(ADSP_EOK != (result = qurt_elite_adsppm_wrapper_deregister(&mmpm_info_ptr->client_id,\
                                                                &mmpm_info_ptr->mmpm_client_ptr)))
    {
#ifdef LSM_DEBUG
     MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"MMPM deregistration failed ");
#endif
    }
    else
    {
#ifdef LSM_DEBUG
    MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"MMPM deregistration success ");
#endif          
    }
  return result;
}

/**
 * Vote for KPPS
 * @param mmpm_info_ptr - lsm session mmpm state variables
 * @param topo_ptr - lsm session topology state variables.
 *
 * @return adsp result status
 */
static ADSPResult lsm_mmpm_mpps_vote(lsm_mmpm_info_t* mmpm_info_ptr,
                                     uint32_t mpps)
{
  ADSPResult result = ADSP_EOK;

  
  MmpmRscExtParamType reqParam;
  reqParam.apiType = MMPM_API_TYPE_SYNC;

  MmpmMppsReqType mppsReq;
  mppsReq.mppsTotal = mpps;
  mppsReq.adspFloorClock = 0;

  MmpmRscParamType rscParam[LSM_NUM_REQUEST_MAX];
  MMPM_STATUS      retStats[LSM_NUM_REQUEST_MAX];
  rscParam[0].rscId                   = MMPM_RSC_ID_MPPS;
  rscParam[0].rscParam.pMppsReq       = &mppsReq;

  reqParam.numOfReq                   = LSM_NUM_REQUEST_MAX;
  reqParam.pExt                       = NULL;     //for future
  reqParam.pReqArray                  = rscParam;
  reqParam.pStsArray                  = retStats; //for most cases mmpmRes is good enough, need not check this array.
  reqParam.reqTag                     = 0;     //for async only


  if (mpps > 0)
  {
    result = qurt_elite_adsppm_wrapper_request(mmpm_info_ptr->client_id,\
                                               &mmpm_info_ptr->mmpm_client_ptr,\
                                               &reqParam);
  }
  else
  {
    result = qurt_elite_adsppm_wrapper_release(mmpm_info_ptr->client_id,\
                                               &mmpm_info_ptr->mmpm_client_ptr,\
                                               &reqParam);
  }

  /* Important message to track voting */
  MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
        "LSM client [%u] requesting ADSPPM "
        "mppsTotal = %lu, adspFloorClock = %lu",
        mmpm_info_ptr->client_id, mppsReq.mppsTotal, mppsReq.adspFloorClock);
   if(ADSP_EOK != result)
  {
#ifdef LSM_DEBUG
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, \
        "MMPM_Request for MIPS failed ");
#endif
  }
  else
  {
#ifdef LSM_DEBUG
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,\
       " MMPM_Request for MIPS success");
#endif
    /* Update debug variables */
    mmpm_info_ptr->dbg_current_mpps_per_thread = mpps;
  }

  return result;
}

#if defined LSM_MMPM_DISABLE_KBPS_VOTING
static inline ADSPResult lsm_mmpm_kbps_vote(lsm_mmpm_info_t *mmpm_info_ptr,
                                            uint32_t kbps)
{
  return ADSP_EOK;
}
#else
/**
 * Vote for BPS
 * @param mmpm_info_ptr - lsm session mmpm state variables
 * @param topo_ptr - lsm session topology state variables.
 *
 * @return adsp result status
 */
static ADSPResult lsm_mmpm_kbps_vote(lsm_mmpm_info_t *mmpm_info_ptr,
                                     uint32_t kbps)
{
  ADSPResult result = ADSP_EOK;
  MmpmRscExtParamType reqParam;
  reqParam.apiType = MMPM_API_TYPE_SYNC;

  uint32_t numBw = 0, adsp_ddr_ind;
  MmpmGenBwValType bwArr[LSM_NUM_REQUEST_MAX];

  //previously MMPM_AUD_BW_TYPE_CPU , MMPM_AUD_BW_PORT_EXT_EBI1 ,MMPM_AUD_BW_PORT_QDSP6TCM
  adsp_ddr_ind = numBw;
  bwArr[numBw].busRoute.masterPort                 = MMPM_BW_PORT_ID_ADSP_MASTER;
  bwArr[numBw].busRoute.slavePort                  = MMPM_BW_PORT_ID_DDR_SLAVE;
  bwArr[numBw].bwValue.busBwValue.bwBytePerSec     = kbps*128; //(*1024/8)
  bwArr[numBw].bwValue.busBwValue.usagePercentage  = 100;
  bwArr[numBw].bwValue.busBwValue.usageType        = MMPM_BW_USAGE_LPASS_DSP;
  numBw++; //1

  MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
        "LSM  client [%u] requesting MMPM "
        "(BW bytesPerSec, Usage perc): ADSP DDR (0x%lx%lx), Usage ( %ld)",
        mmpm_info_ptr->client_id,
        (uint32_t)(bwArr[adsp_ddr_ind].bwValue.busBwValue.bwBytePerSec>>32),
        (uint32_t)bwArr[adsp_ddr_ind].bwValue.busBwValue.bwBytePerSec,
        (uint32_t)bwArr[adsp_ddr_ind].bwValue.busBwValue.usagePercentage);


  MmpmGenBwReqType bwReq;
  bwReq.numOfBw            = numBw;
  bwReq.pBandWidthArray    = bwArr;

  MmpmRscParamType rscParam[LSM_NUM_REQUEST_MAX];
  MMPM_STATUS      retStats[LSM_NUM_REQUEST_MAX];
  rscParam[0].rscId                   = MMPM_RSC_ID_GENERIC_BW_EXT;
  rscParam[0].rscParam.pGenBwReq      = &bwReq;

  reqParam.numOfReq                   = LSM_NUM_REQUEST_MAX;
  reqParam.pExt                       = NULL;     //for future
  reqParam.pReqArray                  = rscParam;
  reqParam.pStsArray                  = retStats; //for most cases mmpmRes is good enough, need not check this array.
  reqParam.reqTag                     = 0;     //for async only

  //for BW any client Id is fine (independent if DML, or some other). Actually client id is specific only for power, register access.
  if(kbps > 0)
  {
    result = qurt_elite_adsppm_wrapper_request(mmpm_info_ptr->client_id,\
                                               &mmpm_info_ptr->mmpm_client_ptr,\
                                               &reqParam);
  }
  else
  {
     result = qurt_elite_adsppm_wrapper_release(mmpm_info_ptr->client_id,\
                                                &mmpm_info_ptr->mmpm_client_ptr,\
                                                &reqParam);
  }

  
   if(ADSP_EOK != result)
  {
#ifdef LSM_DEBUG
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
        "MMPM_Request for BW failed ");
#endif
  }
  else
  {
#ifdef LSM_DEBUG
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
       "MMPM_Request for BW success");
#endif
    /* Update debug variables */
    mmpm_info_ptr->dbg_current_bw_bytes_per_sec = kbps*128;
  }
  return result;
}
#endif // LSM_MMPM_DISABLE_KBPS_VOTING

ADSPResult lsm_mmpm_config(lsm_mmpm_info_t* mmpm_info_ptr,
                           topo_t* topo_ptr,
                           lsm_mmpm_vote_t vote)
{
  ADSPResult result = ADSP_EOK;
  uint32_t mpps = 0;
  uint32_t kbps = 0;

  if((NULL == mmpm_info_ptr)                         ||
     (NULL == topo_ptr && LSM_MMPM_VOTE_NONE != vote)  )
  {
#ifdef LSM_DEBUG
    MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
          "lsm mmpm kpps vote null param mmpm_info %p, topo %p, vote %d",
          mmpm_info_ptr, topo_ptr, vote);
#endif
    return ADSP_EBADPARAM;
  }

  switch(vote)
  {
    case LSM_MMPM_VOTE_NONE:
    {
      result = lsm_mmpm_mpps_vote(mmpm_info_ptr,0);
      result |= lsm_mmpm_kbps_vote(mmpm_info_ptr,0);
      break;
    }
    case LSM_MMPM_VOTE_UPDATE_BPS_ONLY:
    {
      kbps = ceil_div(topo_ptr->bps,1000);
      result = lsm_mmpm_kbps_vote(mmpm_info_ptr,kbps);
      break;
    }
    case LSM_MMPM_VOTE_UPDATE_KPPS_ONLY:
    {
      mpps = ceil_div(topo_ptr->kpps,1000);
      result = lsm_mmpm_mpps_vote(mmpm_info_ptr,mpps);
      break;
    }
    case LSM_MMPM_VOTE_ALL:
    {
      mpps = ceil_div(topo_ptr->kpps,1000);
      kbps = ceil_div(topo_ptr->bps,1000);
      result = lsm_mmpm_mpps_vote(mmpm_info_ptr,mpps);
      result |= lsm_mmpm_kbps_vote(mmpm_info_ptr,kbps);
      break;
    }
    default:
    {
#ifdef LSM_DEBUG
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "Unsupported vote %d by lsm client [%u]",
            vote, mmpm_info_ptr->client_id);
#endif
      result = ADSP_EUNSUPPORTED;
      break;
    }
  }
  return result;
}

#else

/** Stub Functions */
ADSPResult lsm_mmpm_register(lsm_mmpm_info_t *mmpm_info_ptr,
                             uint16_t session_id)
{
  return ADSP_EOK;
}

/** Stub Functions */
ADSPResult lsm_mmpm_deregister(lsm_mmpm_info_t *mmpm_info_ptr)
{
  return ADSP_EOK;
}

/** Stub Functions */
ADSPResult lsm_mmpm_config(lsm_mmpm_info_t* mmpm_info_ptr,
                           topo_t* topo_ptr,
                           lsm_mmpm_vote_t vote)
{
  return ADSP_EOK;
}

#endif //ADSPPM_INTEGRATION

