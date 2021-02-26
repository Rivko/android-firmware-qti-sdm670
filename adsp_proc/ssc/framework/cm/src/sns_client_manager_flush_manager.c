/*=============================================================================
  @file sns_client_manager_flush_manager.c

  This module integrates all the functionality in CM, which deals with flush requests
  and events.

  Copyright (c) 2017 - 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include "sns_assert.h"
#include "sns_client.pb.h"
#include "sns_client_manager.h"
#include "sns_client_manager_flush_manager.h"
#include "sns_client_manager_instance.h"
#include "sns_event_service.h"
#include "sns_fw_attribute_service.h"
#include "sns_fw_data_stream.h"
#include "sns_fw_sensor_instance.h"
#include "sns_isafe_list.h"
#include "sns_island_util.h"
#include "sns_mem_util.h"
#include "sns_memmgr.h"
#include "sns_osa_lock.h"
#include "sns_osa_thread.h"
#include "sns_printf_int.h"
#include "sns_pwr_scpm_mgr.h"
#include "sns_sensor_instance.h"
#include "sns_sensor_uid.h"
#include "sns_service_manager.h"
#include "sns_types.h"

/*=============================================================================
   Public Function Definitions
  ===========================================================================*/

sns_rc sns_cm_handle_client_flush_request(
   sns_sensor_instance *cm_instance,
   sns_cm_request      *cm_request,
   sns_data_stream     *data_stream)
{
  //Add to active flush list
  sns_rc rc = SNS_RC_FAILED;
  sns_cm_inst_state *cm_inst_state = (sns_cm_inst_state*)cm_instance->state->state;
  cm_request->active_flush_req_count++;
  SNS_INST_PRINTF(HIGH, cm_instance, "Flush being requested for req %0x actv clnt flsh cnt %d",
                  cm_request, cm_request->active_flush_req_count);
  //1.Initiate the actual flush
  cm_request->flush_type = SNS_CM_CLIENT_FLUSH;
  rc = data_stream->api->initiate_flush(data_stream);
  if(SNS_RC_SUCCESS != rc)
  {
    SNS_INST_PRINTF(ERROR, cm_instance,
        "Flush failed for client_id %d proc_type %d",
        cm_inst_state->client_id,
        cm_request->req_msg.susp_config.client_proc_type);
    SNS_INST_PRINTF(LOW, cm_instance,
        "Flush failed for suid_high[low] %x suid_high[high] %x suid_low[low] %x suid_low[high] %x",
        SNS_CM_FETCH_LOW_32(cm_request->req_msg.suid.suid_high),
        SNS_CM_FETCH_HI_32(cm_request->req_msg.suid.suid_high),
        SNS_CM_FETCH_LOW_32(cm_request->req_msg.suid.suid_low),
        SNS_CM_FETCH_HI_32(cm_request->req_msg.suid.suid_low));
  }
  else
  {
    sns_cm_bump_up_on_flush_initiate(cm_instance);
#if ENABLE_DEBUG_PRINT	
    SNS_INST_PRINTF(HIGH, cm_instance,
      "Flush initiated for client_id %d proc_type %d active clnt flushes %d",
      cm_inst_state->client_id,
      cm_request->req_msg.susp_config.client_proc_type,
      cm_request->active_flush_req_count);
    SNS_INST_PRINTF(HIGH, cm_instance,
      "Flush initiated  for suid_high[low] %x suid_high[high] %x suid_low[low] %x suid_low[high] %x",
      SNS_CM_FETCH_LOW_32(cm_request->req_msg.suid.suid_high),
      SNS_CM_FETCH_HI_32(cm_request->req_msg.suid.suid_high),
      SNS_CM_FETCH_LOW_32(cm_request->req_msg.suid.suid_low),
      SNS_CM_FETCH_HI_32(cm_request->req_msg.suid.suid_low));
#endif	  
  }
  return rc;
}

void sns_cm_send_flush(
   sns_sensor_instance *instance,
   sns_std_client_processor proc_type)
{
  UNUSED_VAR(proc_type);
  sns_isafe_list_iter iter;
  sns_cm_inst_state *client = (sns_cm_inst_state*)instance->state->state;
  sns_rc rc;
  bool is_flush_initiated = false;
  for(sns_isafe_list_iter_init(&iter, &client->req_list, true);
      NULL != sns_isafe_list_iter_curr(&iter);
      sns_isafe_list_iter_advance(&iter))
  {
    sns_cm_request *cm_request = (sns_cm_request*)
      sns_isafe_list_iter_get_curr_data(&iter);

    if(proc_type != cm_request->req_msg.susp_config.client_proc_type)
    {
      continue;
    }
    //If Flush already active do not trigger another flush
    if(SNS_CM_IS_FLUSH_REQ(cm_request))
    {
       rc = cm_request->data_stream->api->initiate_flush(cm_request->data_stream);
       if(SNS_RC_SUCCESS == rc)
       {
          is_flush_initiated = true;
          cm_request->flush_type = SNS_CM_BATCH_FLUSH;
       }
    }
  }
  if(is_flush_initiated)
  {
    sns_cm_bump_up_on_flush_initiate(instance);
  }
}
