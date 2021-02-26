/*==============================================================================
  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All rights reserved. Qualcomm Proprietary and Confidential.
==============================================================================*/

#include "adsp_amdb_resource_voter.h"
#ifndef ADSP_AMDB_TEST
#include "qurt_elite_memory.h"
#include "qurt_elite_mutex.h"
#include "qurt_elite_adsppm_wrapper.h"
#else
#include "test_main.h"
#endif

#include "qurt_elite_diag.h"

struct amdb_voter_t
{
   qurt_elite_mutex_t mutex;
   uint32_t refs;
   uint32_t adsppm_client_id;
   qurt_elite_adsppm_client_t *qurt_elite_adsppm_client_id;

};

static void amdb_voter_register_mmpm(amdb_voter_t *obj_ptr);
static void amdb_voter_deregister_mmpm(amdb_voter_t *obj_ptr);

amdb_voter_t *amdb_voter_create(void)
{
   amdb_voter_t *obj_ptr = (amdb_voter_t*)qurt_elite_memory_malloc(sizeof(amdb_voter_t), QURT_ELITE_HEAP_DEFAULT);
   if (NULL == obj_ptr)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "amdb: Failed to allocate memory for the ADSPPM voter.");
      return NULL;
   }

   qurt_elite_mutex_init(&obj_ptr->mutex);
   obj_ptr->refs = 0;
   obj_ptr->adsppm_client_id = 0;

   amdb_voter_register_mmpm(obj_ptr);

   return obj_ptr;
}

void amdb_voter_vote(amdb_voter_t *obj_ptr)
{
   qurt_elite_mutex_lock(&obj_ptr->mutex);
   if (0 == obj_ptr->refs)
   {
      qurt_elite_adsppm_wrapper_max_out_pm(obj_ptr->adsppm_client_id, &obj_ptr->qurt_elite_adsppm_client_id, TRUE);
   }

   obj_ptr->refs++;
   qurt_elite_mutex_unlock(&obj_ptr->mutex);
}

void amdb_voter_release(amdb_voter_t *obj_ptr)
{
   qurt_elite_mutex_lock(&obj_ptr->mutex);
   obj_ptr->refs--;

   if (0 == obj_ptr->refs)
   {
      qurt_elite_adsppm_wrapper_max_out_pm(obj_ptr->adsppm_client_id, &obj_ptr->qurt_elite_adsppm_client_id, FALSE);
   }

   qurt_elite_mutex_unlock(&obj_ptr->mutex);
}

void amdb_voter_destroy(amdb_voter_t *obj_ptr)
{
   if (NULL != obj_ptr)
   {
      // Deregister
      amdb_voter_deregister_mmpm(obj_ptr);
      qurt_elite_mutex_destroy(&obj_ptr->mutex);
      qurt_elite_memory_free(obj_ptr);
   }
}

static void amdb_voter_register_mmpm(amdb_voter_t *obj_ptr)
{
#if (ADSPPM_INTEGRATION==1) || defined(ADSP_AMDB_TEST)
   MmpmRegParamType regParam;

   regParam.rev             = MMPM_REVISION;
   regParam.instanceId      = MMPM_CORE_INSTANCE_0;
   regParam.pwrCtrlFlag     = PWR_CTRL_NONE; //PWR_CTRL_STATIC_DISPLAY, PWR_CTRL_THERMAL
   regParam.callBackFlag    = CALLBACK_NONE; //CALLBACK_STATIC_DISPLAY, CALLBACK_THERMAL, CALLBACK_REQUEST_COMPLETE
   regParam.MMPM_Callback   = NULL;
   regParam.cbFcnStackSize  = 0;

   regParam.coreId          = MMPM_CORE_ID_LPASS_ADSP; //no need to request power, register access.
   char adsppmClientName[] = "ADSP AMDB";
   regParam.pClientName     = adsppmClientName;

   ADSPResult result = qurt_elite_adsppm_wrapper_register(&regParam, &obj_ptr->adsppm_client_id, &obj_ptr->qurt_elite_adsppm_client_id);
   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "amdb: ADSPPM register by AMDB. Result %d. Client id %lu", result, obj_ptr->adsppm_client_id);
#endif
}

static void amdb_voter_deregister_mmpm(amdb_voter_t *obj_ptr)
{
   uint32_t client_id = obj_ptr->adsppm_client_id;
   if ( !qurt_elite_adsppm_wrapper_is_registered(client_id) )
   {
       return;
   }
   ADSPResult result = qurt_elite_adsppm_wrapper_deregister(&obj_ptr->adsppm_client_id, &obj_ptr->qurt_elite_adsppm_client_id);

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "amdb: ADSPPM deregister by AMDB. Result %d. Client id %lu", result, client_id);
}

