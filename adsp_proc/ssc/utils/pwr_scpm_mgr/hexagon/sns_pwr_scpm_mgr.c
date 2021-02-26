/**
 * sns_pwr_scpm_mgr.c
 *
 * The Sensors Power Manager interface to the SCPM
 *
 * Copyright (c) 2017 - 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 *
 **/

#include <stdint.h>
#include <string.h>
#include "sns_assert.h"
#include "sns_isafe_list.h"
#include "sns_island.h"
#include "sns_memmgr.h"
#include "sns_osa_lock.h"
#include "sns_printf_int.h"
#include "sns_profiler.h"
#include "sns_pwr_scpm_mgr.h"
#include "sns_sensor.h"
#include "sns_time.h"
#include "sns_types.h"

#include "DDIChipInfo.h"
#include "mmpm.h"


/**
 * Resolution is in ticks, equivalent to 100msec.
 * Threshold to reset the  max thread utilization updated by
 * a thread, if not updated further within this limit.
 *
 * This avoids unnecessary clock bump-up.
 */
#define SNS_PWR_SCPM_THREAD_UTILIZATION_UPDATE_THRESHOLD ( 1920000 )


/**
 * Private thread utilization list item.
 */
typedef struct sns_pwr_scpm_mgr_thread_item
{
  /* Thread pointer */
  sns_osa_thread  *thread;
  /* This thread's utilization percentage */
  uint32_t  thread_utilization;
  /* Time at which thread utilization was updated. */
  sns_time last_update_ts;
  /* Thread list entry */
  sns_isafe_list_item  list_entry;
}sns_pwr_scpm_thread_item;

/**
 * Private state definition.
 */
struct sns_fw_pwr_scpm_mgr
{
  /* SCPM handle for the SSC Q6 */
  uint32_t ssc_q6_client_id;
  /* Current MCPS vote */
  //Todo: Refactoring: combine the run lengths/timestamps into an array
  int32_t low_run_len;
  /* Run length of high thread utilization */
  int32_t high_run_len;
  /* High run length vote timestamp */
  sns_time high_vote_ts;
  /* Low run length vote timestamp */
  sns_time low_vote_ts;
  /* Current MCPS vote */
  uint32_t curr_mcps;
  /* List of threads and their utilization percentages */
  struct sns_isafe_list thread_list;
  /* Lock to protect power SCPM manager data structure fields */
  sns_osa_lock*   scpm_mgr_lock;
  /* List of scpm clients */
  sns_isafe_list client_list;
  /* Thread utilization mcps */
  uint32_t thread_utilization_mcps;
  /* Chip version */
  DalChipInfoVersionType version;
  /* Chip ID */
  DalChipInfoIdType chip;
} ;

/**
 * Private state of the Power SCPM Manager Utility.
 */
static struct sns_fw_pwr_scpm_mgr sns_pwr_scpm_mgr SNS_SECTION(".data.sns");

/*=============================================================================
  Forward Declarations
  ===========================================================================*/

extern bool sns_pwr_scpm_mgr_check_chip_and_version(DalChipInfoIdType chip, DalChipInfoVersionType version);

/**
 * Evaluate all clients and update mcps vote
 */
SNS_SECTION(".text.sns") static bool
sns_pwr_scpm_mgr_check_clients_for_speedup()
{
  sns_isafe_list_iter iter;
  for (sns_isafe_list_iter_init(&iter, &sns_pwr_scpm_mgr.client_list, true);
    NULL != sns_isafe_list_iter_curr(&iter); sns_isafe_list_iter_advance(&iter))
  {
    sns_scpm_mgr_client *client =
      (sns_scpm_mgr_client *)sns_isafe_list_iter_get_curr_data(&iter);
    if (client->require_speedup)
    {
      return true;
    }
  }
  return false;
}

/**
 * If the current SLPI SSC Q6 MCPS does not match new mcps vote, send a
 * request to the SCPM to update the vote
 *
 * @param[i]  mcps   New MCPS vote
 */
SNS_SECTION(".text.sns") static bool
sns_pwr_scpm_mgr_update_mcps_vote()
{
  MMPM_STATUS req_result = MMPM_STATUS_SUCCESS;
  MmpmRscExtParamType req_rsc_param;
  MMPM_STATUS result_status[1];
  MmpmRscParamType req_param[1];
  MmpmMipsReqType mcps_param;
  bool ret_val = false;
  uint32_t mcps = sns_pwr_scpm_mgr_check_clients_for_speedup()?SNS_Q6_MCPS_TURBO:
    sns_pwr_scpm_mgr.thread_utilization_mcps;

  /* Only allow Nominal/Turbo modes on SDM845 V1 due to HW bug */
  if(sns_pwr_scpm_mgr_check_chip_and_version(sns_pwr_scpm_mgr.chip, sns_pwr_scpm_mgr.version))
  {
    if(mcps < SNS_Q6_MCPS_NOMINAL)
    {
      mcps = SNS_Q6_MCPS_NOMINAL;
    }
  }

  if(mcps != sns_pwr_scpm_mgr.curr_mcps)
  {
    SNS_ISLAND_EXIT();

    sns_pwr_scpm_mgr.curr_mcps = mcps;

    mcps_param.mipsTotal = mcps;
  /* unit is in Million cycles per sec */
    mcps_param.mipsPerThread = mcps / NUM_HW_THREAD;
    mcps_param.codeLocation = MMPM_BW_PORT_ID_ADSP_MASTER;
    mcps_param.reqOperation = MMPM_MIPS_REQUEST_CPU_CLOCK_ONLY;

    req_param[0].rscId = MMPM_RSC_ID_MIPS_EXT;
    req_param[0].rscParam.pMipsExt = &mcps_param;

    req_rsc_param.apiType = MMPM_API_TYPE_ASYNC;
    req_rsc_param.numOfReq = 1;
    req_rsc_param.reqTag = 0;
    req_rsc_param.pStsArray = result_status;
    req_rsc_param.pReqArray = req_param;

    req_result = MMPM_Request_Ext(sns_pwr_scpm_mgr.ssc_q6_client_id, &req_rsc_param);
    ret_val = (MMPM_STATUS_SUCCESS == req_result);
    SNS_PRINTF(HIGH, sns_fw_printf, "Set the SSC MIPS Requirement: %d", sns_pwr_scpm_mgr.curr_mcps);
    SNS_PROFILE(SNS_PWR_SCPM_MGR_SET_Q6_MCPS, 1, mcps);
  }
  return ret_val;
}

/**
 * Check the thread utilizations for every thread and update the mcps vote
 * if required
 *
 * @param[i] max_thread_utilization   Max thread utilization from all worker threads
 */
SNS_SECTION(".text.sns") void
sns_pwr_scpm_mgr_compare_utilization_threshold(uint32_t max_thread_utilization)
{
  /* When thread utilization increases beyond the threshold bump up the MCPS
     request to not drop events. As thread utilization reduces, decrease
     the MCPS request down per step */
  sns_time current_time = sns_get_system_time();
  uint32_t prev_mcps = sns_pwr_scpm_mgr.thread_utilization_mcps;

  if(max_thread_utilization > SNS_HIGH_THREAD_UTILIZATION_THRESHOLD)
  {
    sns_pwr_scpm_mgr.low_run_len = 0;
    
    if((current_time - sns_pwr_scpm_mgr.high_vote_ts)
          > sns_convert_ns_to_ticks(SNS_MCPS_VOTE_TIME_WINDOW))
    {
      /* Reset the high vote run length if the time delta
         between votes is > time window */
      sns_pwr_scpm_mgr.high_run_len = 1;
    }
    else
    {
      sns_pwr_scpm_mgr.high_run_len += 1;

      if((sns_pwr_scpm_mgr.thread_utilization_mcps <= SNS_Q6_MCPS_LOW_SVS) &&
          (sns_pwr_scpm_mgr.high_run_len > SNS_HIGH_THREAD_UTILIZATION_RUN_LEN_THRESH_SVS)) 
      {
        sns_pwr_scpm_mgr.thread_utilization_mcps = SNS_Q6_MCPS_SVS;
      }
      else if((sns_pwr_scpm_mgr.thread_utilization_mcps <= SNS_Q6_MCPS_SVS) &&
             (sns_pwr_scpm_mgr.high_run_len > SNS_HIGH_THREAD_UTILIZATION_RUN_LEN_THRESH_NOM)) 
        {
          sns_pwr_scpm_mgr.thread_utilization_mcps = SNS_Q6_MCPS_NOMINAL;
        }
      else if((sns_pwr_scpm_mgr.thread_utilization_mcps <= SNS_Q6_MCPS_NOMINAL) &&
             (sns_pwr_scpm_mgr.high_run_len > SNS_HIGH_THREAD_UTILIZATION_RUN_LEN_THRESH_TURBO)) 
        {
          sns_pwr_scpm_mgr.thread_utilization_mcps = SNS_Q6_MCPS_TURBO;
        }
      /* Update mcps vote if it has changed */
      if (prev_mcps != sns_pwr_scpm_mgr.thread_utilization_mcps)
        {
        sns_pwr_scpm_mgr_update_mcps_vote();
          sns_pwr_scpm_mgr.high_run_len = 0;
        }
      }
    sns_pwr_scpm_mgr.high_vote_ts = current_time;
  }
  else if(max_thread_utilization < SNS_LOW_THREAD_UTILIZATION_THRESHOLD)
  {
    sns_pwr_scpm_mgr.high_run_len = 0;

    if((current_time - sns_pwr_scpm_mgr.low_vote_ts)
          > sns_convert_ns_to_ticks(SNS_MCPS_VOTE_TIME_WINDOW))
    {
      sns_pwr_scpm_mgr.low_run_len = 1;
    }
    else
    {
      sns_pwr_scpm_mgr.low_run_len += 1;
      if (sns_pwr_scpm_mgr.low_run_len > SNS_LOW_THREAD_UTILIZATION_RUN_LEN_THRESH)
      {
        if(sns_pwr_scpm_mgr.thread_utilization_mcps == SNS_Q6_MCPS_TURBO)
        {
          sns_pwr_scpm_mgr.thread_utilization_mcps = SNS_Q6_MCPS_NOMINAL;
        }
        else if(sns_pwr_scpm_mgr.thread_utilization_mcps == SNS_Q6_MCPS_NOMINAL)
        {
          sns_pwr_scpm_mgr.thread_utilization_mcps = SNS_Q6_MCPS_SVS;
        }
        else if(sns_pwr_scpm_mgr.thread_utilization_mcps == SNS_Q6_MCPS_SVS)
        {
          sns_pwr_scpm_mgr.thread_utilization_mcps = SNS_Q6_MCPS_LOW_SVS;
        }
        if(prev_mcps != sns_pwr_scpm_mgr.thread_utilization_mcps)
        {
          sns_pwr_scpm_mgr_update_mcps_vote();
          sns_pwr_scpm_mgr.low_run_len = 0;
        }
      }
    }
    sns_pwr_scpm_mgr.low_vote_ts = current_time;
  }
}

/**
 * Update the thread utilization for the provided thread
 */
SNS_SECTION(".text.sns") void
sns_pwr_scpm_mgr_update_thread_utilization(
 sns_osa_thread *thread,
 uint32_t thread_utilization
)
{
  sns_isafe_list_iter  iter;
  sns_pwr_scpm_thread_item *thread_item = NULL;
  uint32_t max_thread_utilization = 0;
  bool is_thread_present = false;

  sns_osa_lock_acquire(sns_pwr_scpm_mgr.scpm_mgr_lock);

  sns_isafe_list_iter_init(&iter, &sns_pwr_scpm_mgr.thread_list, true);

  /* Check if this thread is already in the list */
  for(;
      NULL != sns_isafe_list_iter_curr(&iter);
      sns_isafe_list_iter_advance(&iter))
  {
    thread_item = (sns_pwr_scpm_thread_item *)sns_isafe_list_iter_get_curr_data(&iter);

    if(thread == thread_item->thread)
    {
      is_thread_present = true;
      thread_item->thread_utilization = thread_utilization;
      thread_item->last_update_ts = sns_get_system_time();
    }

    if(thread_item->thread_utilization > max_thread_utilization)
    {
      sns_time now = sns_get_system_time();

      //If thread utilization not updates since the threshold reset it
      if ( now - thread_item->last_update_ts > SNS_PWR_SCPM_THREAD_UTILIZATION_UPDATE_THRESHOLD )
      {
        thread_item->thread_utilization = 0;
        thread_item->last_update_ts = sns_get_system_time();
      }
      else
      {
        max_thread_utilization = thread_item->thread_utilization;
      }
    }
  }

  /* Create a new thread item if its not already present on the list */
  if(false == is_thread_present)
  {
    thread_item = sns_malloc(SNS_HEAP_ISLAND, sizeof(*thread_item));
    if(NULL == thread_item)
    {
      SNS_ISLAND_EXIT();
      thread_item = sns_malloc(SNS_HEAP_MAIN, sizeof(*thread_item));
    }
    SNS_ASSERT(thread_item != NULL);

    thread_item->thread = thread;
    thread_item->thread_utilization = thread_utilization;
    thread_item->last_update_ts = sns_get_system_time();

    if(thread_utilization > max_thread_utilization)
    {
      max_thread_utilization = thread_utilization;
    }
    sns_isafe_list_item_init(&thread_item->list_entry, thread_item);
    sns_isafe_list_iter_insert(&iter, &thread_item->list_entry, true);
  }
  
  /* For debug */
  //SNS_PRINTF(HIGH, sns_fw_printf, "Thread: %x, Utilization:%d, Max Thread Utilization:%d", thread, thread_utilization, max_thread_utilization);

  sns_pwr_scpm_mgr_compare_utilization_threshold(max_thread_utilization);

  sns_osa_lock_release(sns_pwr_scpm_mgr.scpm_mgr_lock);
}

/**
 * CB func for MMPM request to SCPM
 *
 * @param[i] cb_param     callback parameters
 *
 * @return
 * MMPM_STATUS_SUCCESS if successful
 * MMPM error code on failed request
 */
uint32_t
sns_pwr_scpm_mgr_mmpm_cb(MmpmCallbackParamType *cb_param)
{
  MmpmCompletionCallbackDataType *cb_data;

  cb_data = (MmpmCompletionCallbackDataType *) cb_param->callbackData;

  if(cb_data->result != MMPM_STATUS_SUCCESS)
  {
    SNS_PRINTF(ERROR, sns_fw_printf, "MMPM Request failed with error: %d", cb_data->result);
  }

  return cb_data->result;
}

/**
 * Register with MMPM to send requests to SCPM
 *
 * @param[i] core_id_type     Core type
 * @param[i] core_name        Core name
 *
 * @return
 * Client id to the specified core in SCPM
 *
 */
static uint32_t
sns_pwr_scpm_mgr_register_core(MmpmCoreIdType core_id_type, char *core_name)
{
   MmpmRegParamType mmpmRegParam;

   mmpmRegParam.rev = MMPM_REVISION;
   mmpmRegParam.coreId = core_id_type;
   mmpmRegParam.instanceId = MMPM_CORE_INSTANCE_0;
   mmpmRegParam.MMPM_Callback = sns_pwr_scpm_mgr_mmpm_cb;
   mmpmRegParam.pClientName = core_name;
   mmpmRegParam.pwrCtrlFlag = PWR_CTRL_PERIODIC_CLIENT;
   mmpmRegParam.callBackFlag = CALLBACK_REQUEST_COMPLETE;
   mmpmRegParam.cbFcnStackSize = 128;

   return MMPM_Register_Ext(&mmpmRegParam);
}

/**
 * Initialize the sensors power scpm manager.
 */
sns_rc
sns_pwr_scpm_mgr_init(void)
{
  sns_osa_lock_attr   lock_attr;
  sns_rc rc = SNS_RC_SUCCESS;

  sns_pwr_scpm_mgr.ssc_q6_client_id = sns_pwr_scpm_mgr_register_core(MMPM_CORE_ID_LPASS_ADSP, "SSC_Q6");
  SNS_ASSERT(0 != sns_pwr_scpm_mgr.ssc_q6_client_id);

  /* Initialize list of threads */
  sns_isafe_list_init(&sns_pwr_scpm_mgr.thread_list);

  /* Initialize power scpm manager lock */
  sns_osa_lock_attr_init(&lock_attr);
  sns_osa_lock_attr_set_memory_partition(&lock_attr, SNS_OSA_MEM_TYPE_ISLAND);
  rc = sns_osa_lock_create(&lock_attr, &sns_pwr_scpm_mgr.scpm_mgr_lock);
  SNS_ASSERT(SNS_RC_SUCCESS == rc);

  sns_pwr_scpm_mgr.high_run_len = 0;
  sns_pwr_scpm_mgr.low_run_len = 0;
  sns_pwr_scpm_mgr.high_vote_ts = 0;
  sns_pwr_scpm_mgr.low_vote_ts = 0;

  // Initialize at Nominal
  sns_pwr_scpm_mgr.thread_utilization_mcps = SNS_Q6_MCPS_NOMINAL;

  /* Read the chip version and chip id */
  sns_pwr_scpm_mgr.version = DalChipInfo_ChipVersion();
  sns_pwr_scpm_mgr.chip = DalChipInfo_ChipId();

  /* Initialize SSC Q6 MCPS vote */
  sns_pwr_scpm_mgr_update_mcps_vote();

  return rc;
}

sns_scpm_mgr_client * sns_pwr_scpm_mgr_create_client()
{
  sns_scpm_mgr_client *client = sns_malloc(SNS_HEAP_ISLAND, sizeof(sns_scpm_mgr_client));
  if (NULL != client)
  {
    sns_isafe_list_iter iter;
    if (sns_pwr_scpm_mgr.client_list.item.list != &sns_pwr_scpm_mgr.client_list)
    {
      sns_isafe_list_init(&sns_pwr_scpm_mgr.client_list);
    }
    sns_isafe_list_item_init(&client->list_entry, client);
    sns_isafe_list_iter_init(&iter, &sns_pwr_scpm_mgr.client_list, true);
    sns_osa_lock_acquire(sns_pwr_scpm_mgr.scpm_mgr_lock);
    sns_isafe_list_iter_insert(&iter, &client->list_entry, false);
    client->require_speedup = false;
    sns_osa_lock_release(sns_pwr_scpm_mgr.scpm_mgr_lock);
  }
  return client;
}

SNS_SECTION(".text.sns") sns_rc
sns_pwr_scpm_mgr_mcps_bump_up(sns_scpm_mgr_client *client)
{
  sns_rc ret_val = SNS_RC_INVALID_VALUE;
  if (NULL != client)
  {
    if (!sns_island_is_island_ptr((intptr_t)client))
    {
      SNS_ISLAND_EXIT();
    }
    sns_osa_lock_acquire(sns_pwr_scpm_mgr.scpm_mgr_lock);
    client->require_speedup = true;
    sns_pwr_scpm_mgr_update_mcps_vote();
    sns_osa_lock_release(sns_pwr_scpm_mgr.scpm_mgr_lock);
  }
  return ret_val;
}

SNS_SECTION(".text.sns") sns_rc
sns_pwr_scpm_mgr_mcps_bump_down(sns_scpm_mgr_client *client)
{
  sns_rc ret_val = SNS_RC_INVALID_VALUE;
  if (NULL != client)
  {
    if (!sns_island_is_island_ptr((intptr_t)client))
    {
      SNS_ISLAND_EXIT();
    }
    sns_osa_lock_acquire(sns_pwr_scpm_mgr.scpm_mgr_lock);
    client->require_speedup = false;
    sns_pwr_scpm_mgr_update_mcps_vote();
    sns_osa_lock_release(sns_pwr_scpm_mgr.scpm_mgr_lock);
  }
  return ret_val;
}

SNS_SECTION(".text.sns") sns_rc
sns_pwr_scpm_mgr_absolute_mcps_vote(
  sns_scpm_mgr_client *client, uint32_t abs_mcps)
{
  sns_rc ret_val = SNS_RC_INVALID_VALUE;
  if (NULL != client)
  {
    if (!sns_island_is_island_ptr((intptr_t)client))
    {
      SNS_ISLAND_EXIT();
    }
    sns_osa_lock_acquire(sns_pwr_scpm_mgr.scpm_mgr_lock);
    client->require_speedup = false;
	sns_pwr_scpm_mgr.high_run_len = 0;
    sns_pwr_scpm_mgr.low_run_len = 0;
	sns_pwr_scpm_mgr.thread_utilization_mcps = abs_mcps;
    sns_pwr_scpm_mgr_update_mcps_vote();
    sns_osa_lock_release(sns_pwr_scpm_mgr.scpm_mgr_lock);
  }
  return ret_val;
}

SNS_SECTION(".text.sns") uint32_t
sns_pwr_scpm_mgr_get_current_mcps_vote()
{
  return sns_pwr_scpm_mgr.curr_mcps;
}

