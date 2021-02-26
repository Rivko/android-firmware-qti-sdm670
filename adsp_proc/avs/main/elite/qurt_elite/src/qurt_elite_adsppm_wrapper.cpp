/*========================================================================
qurt_elite_adsppm_wrapper.h

This file contains profiling utilities.

   Copyright (c) 2015, 2017 QUALCOMM Technologies, Incorporated.
   All Rights Reserved.
   QUALCOMM Technologies Proprietary.

Edit History

$Header: //components/rel/avs.adsp/2.8.5/elite/qurt_elite/src/qurt_elite_adsppm_wrapper.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
03/12/15   rbhatnk     Created file.

========================================================================== */


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "qurt_elite.h"
#include "qurt_elite_adsppm_wrapper.h"
#include "AdspCoreSvc.h"
#include "qurt_elite_mutex_v2.h"


/*--------------------------------------------------------------*/
/* Macro definitions                                            */
/* -------------------------------------------------------------*/

//#define PRINT_VOTES

#define QURT_ELITE_ADSPPM_MAX_NUM_CLIENTS QURT_ELITE_MAX_SW_THREADS
#define QURT_ELITE_ADSPPM_INVALID_CLIENT_ID ((uint32_t)0)
#define QURT_ELITE_ADSPPM_NULL_HANDLE 0


#ifdef QURT_ELITE_ADSPPM_DEBUG
static qurt_elite_atomic_word_t qurt_elite_wrapper_count;

static MMPM_STATUS dereg(uint32_t clientId)
{
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "deregister mmpm for client id %lu", clientId);
   return MMPM_STATUS_SUCCESS;
}
static uint32_t reg(MmpmRegParamType *regParam)
{
   int id = qurt_elite_atomic_increment(&qurt_elite_wrapper_count);
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"register mmpm for core id %lu", (unsigned int)regParam->coreId);
   return id;
}
static MMPM_STATUS     req(uint32_t clientId,
      MmpmRscExtParamType *pRscExtParam)
{
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"MMPM request from clientId %lu", clientId);
   return MMPM_STATUS_SUCCESS;
}
static MMPM_STATUS     rel(uint32_t clientId,
      MmpmRscExtParamType *pRscExtParam)
{
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"MMPM release from clientId %lu", clientId);
   return MMPM_STATUS_SUCCESS;
}

#define MMPM_Deregister_Ext dereg
#define MMPM_Register_Ext reg
#define MMPM_Request_Ext req
#define MMPM_Release_Ext rel
#endif //STUB


/* -----------------------------------------------------------------------
 ** Constant / Define Declarations
 ** ----------------------------------------------------------------------- */


typedef struct qurt_elite_adsppm_client_struct_t
{
   qurt_thread_t thread_id;

   uint16_t floor_mhz;
   uint16_t total_mips;

   uint64_t adsp_ddr_Bps;
   uint8_t  usage_perc;

} qurt_elite_adsppm_client_struct_t;

struct mips_bw_t
{
   bool_t mips_updated;
   uint16_t floor_mhz;
   uint16_t total_mips;

   bool_t bw_updated;
   uint64_t adsp_ddr_Bps;
   uint8_t  usage_perc;

   bool_t something_else_updated;
};

/** Ptr to array of client ptrs */
qurt_elite_adsppm_client_struct_t **qurt_elite_adsppm_wrapper_clients_ptr_array_ptr=NULL;
qurt_elite_mutex_t *qurt_elite_adsppm_wrapper_mutex_ptr=NULL;
uint32_t qurt_elite_adsppm_wrapper_profiling_ref_count=0;
//client number shouldn't repeat across profiling. this offset helps in creating new client range after begin-end profiling.
uint32_t qurt_elite_adsppm_wrapper_client_offset=0;

#if (ADSPPM_INTEGRATION==1)
/* =======================================================================
 **                          Function Definitions
 ** ======================================================================= */
static qurt_elite_adsppm_client_t * get_client_from_index(uint32_t index)
{
   return (qurt_elite_adsppm_client_t*)(index+1+qurt_elite_adsppm_wrapper_client_offset); //since invalid id is zero, we put one more here
}

static uint32_t get_index_from_client(qurt_elite_adsppm_client_t *client_ptr)
{
   return ((uint32_t)client_ptr)-1-qurt_elite_adsppm_wrapper_client_offset;
}


static void get_mips_bw(MmpmRscExtParamType *pRscExtParam, bool_t is_req, mips_bw_t *mips_bw)
{
   mips_bw->mips_updated = false;
   mips_bw->bw_updated = false;
   mips_bw->something_else_updated = false;

   for (uint32_t i=0; i<pRscExtParam->numOfReq; i++)
   {
      switch(pRscExtParam->pReqArray[i].rscId)
      {
      case MMPM_RSC_ID_MPPS:
      {
         mips_bw->floor_mhz = pRscExtParam->pReqArray[i].rscParam.pMppsReq->adspFloorClock;
         mips_bw->total_mips = pRscExtParam->pReqArray[i].rscParam.pMppsReq->mppsTotal;
         mips_bw->mips_updated = true;
         break;
      }
      case MMPM_RSC_ID_GENERIC_BW_EXT:
      {
         MmpmGenBwValType *bwArr = pRscExtParam->pReqArray[i].rscParam.pGenBwReq->pBandWidthArray;

         for (uint32_t j=0;j<pRscExtParam->pReqArray[i].rscParam.pGenBwReq->numOfBw; j++)
         {
            if ( (bwArr[j].busRoute.masterPort == MMPM_BW_PORT_ID_ADSP_MASTER) &&
                  (bwArr[j].busRoute.slavePort == MMPM_BW_PORT_ID_DDR_SLAVE))
            {
               if (is_req)
               {
                  mips_bw->adsp_ddr_Bps = bwArr[j].bwValue.busBwValue.bwBytePerSec;
                  mips_bw->usage_perc = bwArr[j].bwValue.busBwValue.usagePercentage;
               }
               else
               {
                  mips_bw->adsp_ddr_Bps = 0;
                  mips_bw->usage_perc = 0;
               }
               mips_bw->bw_updated = true;
            }
         }
         break;
      }
      default:
      {
         mips_bw->something_else_updated = true;
      }
      }
   }
}

static void process_req_rel(uint32_t client_id, qurt_elite_adsppm_client_t *client_ptr, MmpmRscExtParamType *pRscExtParam, bool_t is_req)
{
   mips_bw_t mips_bw;
   get_mips_bw(pRscExtParam, is_req, &mips_bw);

   qurt_elite_mutex_lock(qurt_elite_adsppm_wrapper_mutex_ptr);

   if (qurt_elite_adsppm_wrapper_clients_ptr_array_ptr == NULL)
   {
      qurt_elite_mutex_unlock(qurt_elite_adsppm_wrapper_mutex_ptr);
      return;
   }
   qurt_elite_adsppm_client_struct_t *ptr = qurt_elite_adsppm_wrapper_clients_ptr_array_ptr[get_index_from_client(client_ptr)]; //since invalid id is zero

   if (ptr == NULL)
   {
      qurt_elite_mutex_unlock(qurt_elite_adsppm_wrapper_mutex_ptr);
      return;
   }

   if (mips_bw.mips_updated)
   {
      ptr->floor_mhz = mips_bw.floor_mhz;
      ptr->total_mips = mips_bw.total_mips;
   }
   if (mips_bw.bw_updated)
   {
      ptr->adsp_ddr_Bps = mips_bw.adsp_ddr_Bps;
      ptr->usage_perc = mips_bw.usage_perc;
   }

   qurt_elite_mutex_unlock(qurt_elite_adsppm_wrapper_mutex_ptr);
}

static void print_votes(uint32_t client_id, MmpmRscExtParamType *pRscExtParam, bool_t is_req)
{
#ifdef PRINT_VOTES
   mips_bw_t mips_bw;
   get_mips_bw(pRscExtParam, is_req, &mips_bw);

   if (is_req)
   {
      if (mips_bw.mips_updated && mips_bw.bw_updated)
      {
         MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM_Request by client %2lu, mpps_per_thread=%2lu, Total MPPS=%2lu, BW=%4lu KBps, UF=%2lu%%",
               client_id, mips_bw.floor_mhz, mips_bw.total_mips, (uint32_t)(mips_bw.adsp_ddr_Bps/1024), mips_bw.usage_perc);
      }
      else if (mips_bw.mips_updated)
      {
         MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM_Request by client %2lu, mpps_per_thread=%2lu, Total MPPS=%2lu",
               client_id, mips_bw.floor_mhz, mips_bw.total_mips);
      }
      else if (mips_bw.bw_updated)
      {
         MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM_Request by client %2lu, BW=%4lu KBps, UF=%2lu%%",
               client_id, (uint32_t)(mips_bw.adsp_ddr_Bps/1024), mips_bw.usage_perc);
      }
      else if (!mips_bw.something_else_updated)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM_Request by client %2lu, nothing changed",
               client_id);
      }
   }
   else
   {
      if (mips_bw.mips_updated && mips_bw.bw_updated)
      {
         MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM_Release by client %2lu, mpps_per_thread=%2lu, Total MPPS=%2lu, BW=%4lu KBps, UF=%2lu%%",
               client_id, mips_bw.floor_mhz, mips_bw.total_mips, (uint32_t)(mips_bw.adsp_ddr_Bps/1024), mips_bw.usage_perc);
      }
      else if (mips_bw.mips_updated)
      {
         MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM_Release by client %2lu, mpps_per_thread=%2lu, Total MPPS=%2lu",
               client_id, mips_bw.floor_mhz, mips_bw.total_mips);
      }
      else if (mips_bw.bw_updated)
      {
         MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM_Release by client %2lu, BW=%4lu KBps, UF=%2lu%%",
               client_id, (uint32_t)(mips_bw.adsp_ddr_Bps/1024), mips_bw.usage_perc);
      }
      else if (!mips_bw.something_else_updated)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM_Release by client %2lu, nothing changed",
               client_id);
      }
   }
#endif
}

static bool_t client_exists(qurt_elite_adsppm_client_t *client_ptr)
{
   bool_t rc=false;

   qurt_elite_mutex_lock(qurt_elite_adsppm_wrapper_mutex_ptr);

   //client exists only if it is > qurt_elite_adsppm_wrapper_client_offset
   // static svc like ASM/ADM can create_client during request call.
   // when end_profiling is called such clients are freed (this is to make sure tests dont fail with mem leak).
   // if profiling is enabled again, those clients supply old id
   // qurt_elite_adsppm_wrapper_client_offset is updated after end call.
   // when profiling is again enabled and such clients show old id, they need to be recreated.
   //  client_ptr ranges from (1 to QURT_ELITE_ADSPPM_MAX_NUM_CLIENTS)+qurt_elite_adsppm_wrapper_client_offset.
   if ( (uint32_t)client_ptr <= qurt_elite_adsppm_wrapper_client_offset)
   {
      qurt_elite_mutex_unlock(qurt_elite_adsppm_wrapper_mutex_ptr);
      return false;
   }

   if (NULL != qurt_elite_adsppm_wrapper_clients_ptr_array_ptr)
   {
      if (NULL != qurt_elite_adsppm_wrapper_clients_ptr_array_ptr[get_index_from_client(client_ptr)])
      {
         rc = true;
      }
   }
   qurt_elite_mutex_unlock(qurt_elite_adsppm_wrapper_mutex_ptr);

   return rc;
}

/**
 * qurt_elite_adsppm_wrapper_end_profiling would destroy all clients
 *
 * so even if client ptr is valid, it may be the one already destroyed.
 *
 */
static void destroy_client(qurt_elite_adsppm_client_t **client_ptr_ptr)
{
   qurt_elite_mutex_lock(qurt_elite_adsppm_wrapper_mutex_ptr);

   if (!client_exists(*client_ptr_ptr))
   {
      qurt_elite_mutex_unlock(qurt_elite_adsppm_wrapper_mutex_ptr);
      return;
   }

   uint32_t ind=get_index_from_client(*client_ptr_ptr);
   if (NULL != qurt_elite_adsppm_wrapper_clients_ptr_array_ptr[ind])
   {
      qurt_elite_memory_free(qurt_elite_adsppm_wrapper_clients_ptr_array_ptr[ind]);
      qurt_elite_adsppm_wrapper_clients_ptr_array_ptr[ind]=NULL;
      *((uint32_t*)client_ptr_ptr) = QURT_ELITE_ADSPPM_INVALID_CLIENT_ID;
   }

   qurt_elite_mutex_unlock(qurt_elite_adsppm_wrapper_mutex_ptr);
}


//return ADSP_EOK if created
static ADSPResult check_create_client(qurt_elite_adsppm_client_t **client_ptr_ptr)
{
   qurt_elite_adsppm_client_struct_t *client_struct_ptr=NULL;

   //profiling enabled?
   if (NULL == qurt_elite_adsppm_wrapper_clients_ptr_array_ptr)
   {
      return ADSP_EFAILED;
   }

   qurt_elite_mutex_lock(qurt_elite_adsppm_wrapper_mutex_ptr);

   //create if not already created.
   if (client_exists(*client_ptr_ptr)) //even if client_ptr_ptr has valid client id, it may not be available as end_prof might have been called.
   {
      qurt_elite_mutex_unlock(qurt_elite_adsppm_wrapper_mutex_ptr);
      return ADSP_EOK;
   }

   uint32_t i=0;
   //find first null place.
   for (i=0; i<QURT_ELITE_ADSPPM_MAX_NUM_CLIENTS;i++)
   {
      if (NULL == qurt_elite_adsppm_wrapper_clients_ptr_array_ptr[i])
      {
         break;
      }
   }

   if (i==QURT_ELITE_ADSPPM_MAX_NUM_CLIENTS)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Profiling: ADSPPM_Registering failed as there's no place to hold ptr. num space holders %d", QURT_ELITE_ADSPPM_MAX_NUM_CLIENTS);
      *((uint32_t*)client_ptr_ptr) = QURT_ELITE_ADSPPM_INVALID_CLIENT_ID;
   }

   {
      void *ptr = qurt_elite_memory_malloc(sizeof(qurt_elite_adsppm_client_struct_t), QURT_ELITE_HEAP_DEFAULT);
      if (ptr == NULL)
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Profiling: ADSPPM_Registering - failed to allocate client struct.");
         //no need to fail this function
         qurt_elite_mutex_unlock(qurt_elite_adsppm_wrapper_mutex_ptr);
         return ADSP_EFAILED;
      }
      memset(ptr, 0, sizeof(qurt_elite_adsppm_client_struct_t));
      client_struct_ptr = (qurt_elite_adsppm_client_struct_t*)ptr;
   }

   qurt_thread_t thread_id = qurt_thread_get_id();
   (client_struct_ptr)->thread_id = thread_id;

   qurt_elite_adsppm_wrapper_clients_ptr_array_ptr[i] = client_struct_ptr;

   *client_ptr_ptr = get_client_from_index(i);

   qurt_elite_mutex_unlock(qurt_elite_adsppm_wrapper_mutex_ptr);

   return ADSP_EOK;
}

/** @ingroup qurt_elite_adsppm_wrapper
  Registers with ADSPPM

  @return
  returns error code.

   Note about qurt_elite_adsppm_client_t** -> it is actually uint32_t*

  @dependencies
  None.
 */
ADSPResult qurt_elite_adsppm_wrapper_register(MmpmRegParamType *regParam, uint32_t *client_id_ptr, qurt_elite_adsppm_client_t **client_ptr_ptr)
{
   ADSPResult result = ADSP_EOK;
   *((uint32_t*)client_ptr_ptr) = QURT_ELITE_ADSPPM_INVALID_CLIENT_ID;
   *client_id_ptr = NULL;

   if (QURT_ELITE_ADSPPM_NULL_HANDLE == (*client_id_ptr = MMPM_Register_Ext(regParam)))
   {
#if defined(SIM)
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADSPPM_Register_Ext failed %s", regParam->pClientName);
#else
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADSPPM_Register_Ext failed %p", regParam->pClientName);
#endif
      result = ADSP_EFAILED;
   }
   else
   {
#if defined(SIM)
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM_Register_Ext client id %lu. Name %s", *client_id_ptr, regParam->pClientName);
#else
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM_Register_Ext client id %lu. Name %p", *client_id_ptr, regParam->pClientName);
#endif

      //Register for client class. For both Audio and AFE this works.
      MmpmClientClassType class_type = MMPM_AUDIO_CLIENT_CLASS;
      MmpmParameterConfigType param_cfg;
      param_cfg.pParamConfig = (void*)&class_type;
      param_cfg.paramId = MMPM_PARAM_ID_CLIENT_CLASS;
      MMPM_STATUS status = MMPM_SetParameter(*client_id_ptr, &param_cfg);
      if (status != MMPM_STATUS_SUCCESS)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MMPM_SetParameter class type failed for %lu with status %lu ", *client_id_ptr, (uint32_t) status);
      }
      else
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MMPM_SetParameter class type success for %lu", *client_id_ptr);
      }

      //Register for DCVS up only
      MmpmDcvsParticipationType dcvsParticipation;
      dcvsParticipation.enable = TRUE;
      dcvsParticipation.enableOpt = MMPM_DCVS_ADJUST_ONLY_UP;
      param_cfg.pParamConfig = (void*)&dcvsParticipation;
      param_cfg.paramId = MMPM_PARAM_ID_DCVS_PARTICIPATION;
      status = MMPM_SetParameter(*client_id_ptr, &param_cfg);
      if (status != MMPM_STATUS_SUCCESS)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MMPM_SetParameter DCVS participation failed for %lu with status %lu ", *client_id_ptr, (uint32_t) status);
      }
      else
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MMPM_SetParameter DCVS participation success for %lu", *client_id_ptr);
      }
   }

   (void)check_create_client(client_ptr_ptr);

   return result;
}


/** @ingroup qurt_elite_adsppm_wrapper
  Sends request to ADSPPM

  @return
  returns error code.

  @dependencies
  None.
 */
ADSPResult qurt_elite_adsppm_wrapper_request(uint32_t client_id, qurt_elite_adsppm_client_t **client_ptr_ptr, MmpmRscExtParamType *pRscExtParam)
{
   ADSPResult result = ADSP_EOK;

   if (QURT_ELITE_ADSPPM_NULL_HANDLE != client_id)
   {
      MMPM_STATUS mmpmRes;
      mmpmRes = MMPM_Request_Ext(client_id, pRscExtParam);

      if(MMPM_STATUS_SUCCESS != mmpmRes)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADSPPM_Request from client id %lu failed", client_id);
         result = ADSP_EFAILED;
      }
      else
      {
         MSG_1(MSG_SSID_QDSP6, DBG_LOW_PRIO, "ADSPPM_Request from client id %lu success", client_id);
      }

      print_votes(client_id, pRscExtParam, TRUE);

      if (ADSP_SUCCEEDED(check_create_client(client_ptr_ptr)))
      {
         process_req_rel(client_id, *client_ptr_ptr, pRscExtParam, TRUE);
      }
   }
   return result;
}

/** @ingroup qurt_elite_adsppm_wrapper
  Sends release to ADSPPM

  @return
  returns error code.

  @dependencies
  None.
 */
ADSPResult qurt_elite_adsppm_wrapper_release(uint32_t client_id, qurt_elite_adsppm_client_t **client_ptr_ptr, MmpmRscExtParamType *pRscExtParam)
{
   ADSPResult result = ADSP_EOK;
   if (QURT_ELITE_ADSPPM_NULL_HANDLE != client_id)
   {
      MMPM_STATUS mmpmRes;
      mmpmRes = MMPM_Release_Ext(client_id, pRscExtParam);

      if(MMPM_STATUS_SUCCESS != mmpmRes)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADSPPM_Release from client id %lu failed", client_id);
         result = ADSP_EFAILED;
      }
      else
      {
         MSG_1(MSG_SSID_QDSP6, DBG_LOW_PRIO, "ADSPPM_Release from client id %lu success", client_id);
      }
      print_votes(client_id, pRscExtParam, FALSE);

      if (ADSP_SUCCEEDED(check_create_client(client_ptr_ptr)))
      {
         process_req_rel(client_id, *client_ptr_ptr, pRscExtParam, FALSE);
      }
   }

   return result;
}
#endif //ADSPPM_INTEGRATION

/** @ingroup qurt_elite_adsppm_wrapper
  Deregisters with ADSPPM

  @return
  returns error code.

  @dependencies
  None.
 */
ADSPResult qurt_elite_adsppm_wrapper_deregister(uint32_t *client_id_ptr, qurt_elite_adsppm_client_t **client_ptr_ptr)
{
   ADSPResult result = ADSP_EOK;
#if (ADSPPM_INTEGRATION==1)
   MMPM_STATUS     status;

   if (QURT_ELITE_ADSPPM_NULL_HANDLE != *client_id_ptr)
   {
      if (MMPM_STATUS_SUCCESS != (status = MMPM_Deregister_Ext(*client_id_ptr)))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADSPPM_Deregister_Ext failed. client id %lu", *client_id_ptr);
         result = ADSP_EFAILED;
      }
      else
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM_Deregister_Ext success. client id %lu", *client_id_ptr);
      }

      destroy_client(client_ptr_ptr);

      *client_id_ptr = QURT_ELITE_ADSPPM_NULL_HANDLE;
   }
#endif
   return result;
}

ADSPResult qurt_elite_adsppm_wrapper_create()
{
   ADSPResult result = ADSP_EOK;

   qurt_elite_adsppm_wrapper_mutex_ptr = NULL;

   result = qurt_elite_mutex_create(&qurt_elite_adsppm_wrapper_mutex_ptr);
   if (ADSP_FAILED(result))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADSPPM wrapper: mutex alloc failed.");
      return result;
   }
   qurt_elite_adsppm_wrapper_profiling_ref_count = 0;

   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"qurt_elite_adsppm_wrapper_create");

   return result;
}

ADSPResult qurt_elite_adsppm_wrapper_destroy()
{
   ADSPResult result = ADSP_EOK;

   qurt_elite_mutex_destroy(qurt_elite_adsppm_wrapper_mutex_ptr);
   qurt_elite_adsppm_wrapper_mutex_ptr = NULL;

   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"qurt_elite_adsppm_wrapper_destroy");

   return result;
}

/** @ingroup qurt_elite_adsppm_wrapper
  Creates qurt_elite_adsppm_wrapper

  @return
  returns error code.

  @dependencies
  None.
 */
ADSPResult qurt_elite_adsppm_wrapper_begin_profiling()
{
   ADSPResult result = ADSP_EOK;

   qurt_elite_mutex_lock(qurt_elite_adsppm_wrapper_mutex_ptr);
   if (0 == qurt_elite_adsppm_wrapper_profiling_ref_count)
   {
      qurt_elite_adsppm_wrapper_clients_ptr_array_ptr = NULL;

      uint32_t sz=sizeof(qurt_elite_adsppm_client_struct_t*)*QURT_ELITE_ADSPPM_MAX_NUM_CLIENTS;
      qurt_elite_adsppm_client_struct_t **temp = (qurt_elite_adsppm_client_struct_t **)qurt_elite_memory_malloc(sz, QURT_ELITE_HEAP_DEFAULT);
      if (temp == NULL)
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Profiling: ADSPPM wrapper: client ptr array allocation failed");
         qurt_elite_mutex_unlock(qurt_elite_adsppm_wrapper_mutex_ptr);
         return ADSP_ENOMEMORY;
      }
      memset(temp, 0, sz);
      qurt_elite_adsppm_wrapper_clients_ptr_array_ptr=temp;
   }
   qurt_elite_adsppm_wrapper_profiling_ref_count++;
   qurt_elite_mutex_unlock(qurt_elite_adsppm_wrapper_mutex_ptr);

   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"qurt_elite_adsppm_wrapper_begin_profiling");

   return result;
}

/** @ingroup qurt_elite_adsppm_wrapper
  Destroys qurt_elite_adsppm_wrapper

  @return
  returns error code.

  @dependencies
  None.
 */
ADSPResult qurt_elite_adsppm_wrapper_end_profiling()
{
   ADSPResult result = ADSP_EOK;
   qurt_elite_mutex_lock(qurt_elite_adsppm_wrapper_mutex_ptr);
   qurt_elite_adsppm_wrapper_profiling_ref_count--;

   if (0 == qurt_elite_adsppm_wrapper_profiling_ref_count)
   {
      qurt_elite_adsppm_client_struct_t **temp = qurt_elite_adsppm_wrapper_clients_ptr_array_ptr;
      qurt_elite_adsppm_wrapper_clients_ptr_array_ptr=NULL;

      for (uint32_t i=0; i<QURT_ELITE_ADSPPM_MAX_NUM_CLIENTS;i++)
      {
         if (NULL != temp[i])
         {
            qurt_elite_memory_free(temp[i]);
            temp[i] = NULL;
         }
      }
      qurt_elite_memory_free(temp);
      temp = NULL;
      qurt_elite_adsppm_wrapper_client_offset += QURT_ELITE_ADSPPM_MAX_NUM_CLIENTS;
      if ( ((uint64_t)qurt_elite_adsppm_wrapper_client_offset)+QURT_ELITE_ADSPPM_MAX_NUM_CLIENTS > 0xFFFFFFFF)
      {
         qurt_elite_adsppm_wrapper_client_offset=0;
      }
   }
   qurt_elite_mutex_unlock(qurt_elite_adsppm_wrapper_mutex_ptr);

   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"qurt_elite_adsppm_wrapper_end_profiling");

   return result;
}


void qurt_elite_adsppm_wrapper_aggregate(avcs_component_adsppm_voting_info_t *info_ptr, avcs_component_adsppm_voting_info_t *voc_client_ptr, uint32_t *aggr_bw, uint32_t *aggr_mpps)
{
   //MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"qurt_elite_adsppm_wrapper_aggregate");

   uint16_t total_mips=0;
   uint16_t floor_mips=0;
   uint64_t ib_bw=0;
   uint64_t ab_bw=0;
   if (NULL != voc_client_ptr)
   {
      total_mips=voc_client_ptr->total_mips;
      floor_mips=voc_client_ptr->mips_per_thread;
      ib_bw=(((uint64_t)voc_client_ptr->ib_adsp_ddr_Bps_msw)<<32)|((uint64_t)voc_client_ptr->ib_adsp_ddr_Bps_lsw);
      ab_bw=(((uint64_t)voc_client_ptr->ab_adsp_ddr_Bps_msw)<<32)|((uint64_t)voc_client_ptr->ab_adsp_ddr_Bps_lsw);
   }

   qurt_elite_mutex_lock(qurt_elite_adsppm_wrapper_mutex_ptr);
   for (uint32_t i=1;i<QURT_ELITE_ADSPPM_MAX_NUM_CLIENTS;i++) //0 is invalid
   {
      if (NULL != qurt_elite_adsppm_wrapper_clients_ptr_array_ptr[i])
      {
         total_mips += qurt_elite_adsppm_wrapper_clients_ptr_array_ptr[i]->total_mips;

         if (qurt_elite_adsppm_wrapper_clients_ptr_array_ptr[i]->floor_mhz > floor_mips)
         {
            floor_mips = qurt_elite_adsppm_wrapper_clients_ptr_array_ptr[i]->floor_mhz;
         }
         if (qurt_elite_adsppm_wrapper_clients_ptr_array_ptr[i]->adsp_ddr_Bps > ib_bw)
         {
            ib_bw = qurt_elite_adsppm_wrapper_clients_ptr_array_ptr[i]->adsp_ddr_Bps;
         }
         ab_bw += (qurt_elite_adsppm_wrapper_clients_ptr_array_ptr[i]->adsp_ddr_Bps*qurt_elite_adsppm_wrapper_clients_ptr_array_ptr[i]->usage_perc/100);

#if 0
         MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"qurt_elite_adsppm_wrapper MPPS %lu, contribution[%lu] = %lu, ib=%llu, contri=%llu", total_mips, i,
               qurt_elite_adsppm_wrapper_clients_ptr_array_ptr[i]->total_mips, ib_bw, qurt_elite_adsppm_wrapper_clients_ptr_array_ptr[i]->adsp_ddr_Bps );
#endif

      }
   }
   qurt_elite_mutex_unlock(qurt_elite_adsppm_wrapper_mutex_ptr);

   info_ptr->mask = AVCS_COMPONENT_ADSPPM_VOTING_MASK__MIPS_CHANGED_MASK|AVCS_COMPONENT_ADSPPM_VOTING_MASK__ADSP_DDR_BW_CHANGED_MASK;
   info_ptr->mips_per_thread = floor_mips;
   info_ptr->total_mips = total_mips;
   info_ptr->ab_adsp_ddr_Bps_lsw = (uint32_t)ab_bw;
   info_ptr->ab_adsp_ddr_Bps_msw = (uint32_t)(ab_bw>>32);
   info_ptr->ib_adsp_ddr_Bps_lsw = (uint32_t)ib_bw;
   info_ptr->ib_adsp_ddr_Bps_msw = (uint32_t)(ib_bw>>32);

#if (ADSPPM_INTEGRATION==1)
   //GetInfo API on MMPM

   if (NULL != aggr_bw)
   {
      *aggr_bw = 0;
      //aggregated BW
      MmpmInfoBwExtType bw_info;
      MmpmInfoDataType info_data;
      info_data.infoId = MMPM_INFO_ID_BW_EXT;
      info_data.coreId = MMPM_CORE_ID_LPASS_ADSP;
      info_data.instanceId = MMPM_CORE_INSTANCE_0;
      bw_info.busRoute.masterPort = MMPM_BW_PORT_ID_ADSP_MASTER;
      bw_info.busRoute.slavePort = MMPM_BW_PORT_ID_DDR_SLAVE;
      bw_info.totalBw = 0; // output
      info_data.info.pBwExtInfo = &bw_info;
      MMPM_STATUS status = MMPM_GetInfo(&info_data);
      if (status == MMPM_STATUS_SUCCESS)
      {
         *aggr_bw=(uint32_t)(bw_info.totalBw/(1024*1024)); //MBps
      }
   }

   if (NULL != aggr_mpps)
   {
      *aggr_mpps = 0;
      //aggregated MPPS
      MmpmInfoMppsType mpps_info;
      MmpmInfoDataType info_data;
      info_data.infoId = MMPM_INFO_ID_MPPS;
      info_data.coreId = MMPM_CORE_ID_LPASS_ADSP;
      info_data.instanceId = MMPM_CORE_INSTANCE_0;
      mpps_info.aggregateMpps = 0;//output
      mpps_info.clientClasses = (uint32_t) (-1); //all clients
      info_data.info.pMppsInfo = &mpps_info;
      MMPM_STATUS status = MMPM_GetInfo(&info_data);
      if (status == MMPM_STATUS_SUCCESS)
      {
         *aggr_mpps=mpps_info.aggregateMpps;
      }
   }
#endif //(ADSPPM_INTEGRATION==1)

}

bool_t qurt_elite_adsppm_wrapper_is_registered(uint32_t client_id)
{
   if (client_id == QURT_ELITE_ADSPPM_NULL_HANDLE)
   {
      return false;
   }
   return true;
}

ADSPResult qurt_elite_adsppm_wrapper_max_out_pm(uint32_t client_id, qurt_elite_adsppm_client_t **client_ptr_ptr, bool_t is_max_out)
{
   ADSPResult result = ADSP_EOK;

#if (ADSPPM_INTEGRATION==1)

   if (!qurt_elite_adsppm_wrapper_is_registered(client_id))
   {
      return result;
   }

   static const uint8_t NUM_REQUEST=2;
   MmpmRscParamType rscParam[NUM_REQUEST];
   MMPM_STATUS      retStats[NUM_REQUEST];
   MmpmRscExtParamType reqParam;
   uint8_t req_num=0;
   MmpmMppsReqType mmpmMppsParam;
   MmpmGenBwValType bwReqVal;
   MmpmGenBwReqType bwReq;

   reqParam.apiType                    = MMPM_API_TYPE_SYNC;
   reqParam.pExt                       = NULL;       //for future
   reqParam.pReqArray                  = rscParam;
   reqParam.pStsArray                  = retStats;   //for most cases mmpmRes is good enough, need not check this array.
   reqParam.reqTag                     = 0;          //for async only
   uint32_t mpps, total;
   uint64_t newBw;
   {
      mpps =  500; //MPPS
      total = 10;
      if (!is_max_out)
      {
         mpps = 0;
         total = 0;
      }

      mmpmMppsParam.mppsTotal                  = total;
      mmpmMppsParam.adspFloorClock             = mpps;
      rscParam[req_num].rscId                   = MMPM_RSC_ID_MPPS;
      rscParam[req_num].rscParam.pMppsReq       = &mmpmMppsParam;

      req_num++;

   }

   {
      newBw = 400*1024*1024; //Bps
      if (!is_max_out) newBw = 0;

      bwReqVal.busRoute.masterPort                 = MMPM_BW_PORT_ID_ADSP_MASTER;
      bwReqVal.busRoute.slavePort                  = MMPM_BW_PORT_ID_DDR_SLAVE;
      bwReqVal.bwValue.busBwValue.bwBytePerSec     = newBw;
      bwReqVal.bwValue.busBwValue.usagePercentage  = 100;
      bwReqVal.bwValue.busBwValue.usageType        = MMPM_BW_USAGE_LPASS_DSP;

      bwReq.numOfBw            = 1;
      bwReq.pBandWidthArray    = &bwReqVal;

      rscParam[req_num].rscId                   = MMPM_RSC_ID_GENERIC_BW_EXT;
      rscParam[req_num].rscParam.pGenBwReq      = &bwReq;
      req_num++;
   }

   if (req_num )
   {
      reqParam.numOfReq                   = req_num;

      if ( is_max_out )
      {
         result = qurt_elite_adsppm_wrapper_request(client_id, client_ptr_ptr, &reqParam);
         MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "qurt_elite_adsppm_wrapper: ADSPPM request MPPS %lu and BW %lu KBPS by AMDB (%lu). Result %d", mpps, (uint32_t)(newBw/1024), client_id, result);
      }
      else
      {
         result = qurt_elite_adsppm_wrapper_release(client_id, client_ptr_ptr, &reqParam);
         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "qurt_elite_adsppm_wrapper: ADSPPM release BW and MPPS by AMDB (%lu). Result %d", client_id, result);
      }
   }

#endif //#if (ADSPPM_INTEGRATION==1)

   return result;
}
