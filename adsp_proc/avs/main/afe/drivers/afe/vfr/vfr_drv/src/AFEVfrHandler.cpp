/*========================================================================
   This file contains AFE VFR handler implementation

 Copyright (c) 2009-2016 QUALCOMM Technologies, Inc.  All Rights Reserved.
 QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
 by the U.S. Government, Diversion contrary to U.S. law prohibited.

  $Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/vfr/vfr_drv/src/AFEVfrHandler.cpp#1 $
 ====================================================================== */

/*==========================================================================
  Include files
  ========================================================================== */

#include "AFEInternal.h"
#include "Elite.h"
#include "EliteMsg_Custom.h"
#include "EliteMsg_AfeCustom.h"
#include "EliteSvc.h"
#include "qurt_elite.h"
#include "AFEDeviceDriver.h"
#include "AFERtProxyDriver.h"

#include "VFRDriver_i.h"
#include "VFRIsrHandler_i.h"

/*----------------------------------------------------------------------------
 * Functions
 * -------------------------------------------------------------------------*/
static ADSPResult vfr_svc_cmd_handler(void* dummy, elite_msg_any_t* pMsg);

/*----------------------------------------------------------------------------
 * Globals
 * -------------------------------------------------------------------------*/

/* VFR svc thread name */
static char VFR_THREAD_NAME[] =  "VfrD";

/**
 * max number of elements in the cmd or ack queue.
 */
const uint32_t VFR_MAX_CMD_Q_ELEMENTS = 16;
/**
 * AFE thread stack size for static service thread and pseudoport threads
 */
const uint32 VFR_THREAD_STACK_SIZE = 4096;

/* VFR driver state global object */
vfr_drv_state_t vfr_drv_global_stat;

/* VFR svc cmd Queue */
QURT_ELITE_ALIGN(char vfrCmdQBuf[QURT_ELITE_QUEUE_GET_REQUIRED_BYTES(VFR_MAX_CMD_Q_ELEMENTS)], 8);

// VFR svc cmd handler
static   elite_svc_msg_handler_func vfr_cmd_handler[] =
{
    vfr_svc_cmd_handler
};
/*----------------------------------------------------------------------------*/

/**
 * This function Sends a vfr message update to voice
 *
 * @param[in] clientQ over which the message needs to be send:
 *       global vfr_status structure
 * @return None
 */
void vfr_drift_update(vfr_src_state_t *vfr_state_ptr, vfr_hw_info_t *vfr_hw_info)
{
  ADSPResult                    result;
  int                           nActualSize;
  qurt_elite_bufmgr_node_t      bufMgrNode;
  elite_msg_custom_vfr_info_t   *pVfrMsg;
  elite_msg_any_t               eliteMsg;
  uint8_t                       resync_status = FALSE;

  /* read hardware registers for p_vfr_src_state->cur_av_time_stamp*/
  vfr_state_ptr->cur_av_time_stamp = afe_device_read_timestamp_register(vfr_state_ptr->mux_id);


  vfr_driver_calc_drift_updates(afe_svc_global_ptr, vfr_state_ptr, &resync_status);


  /* Below message needs to be sent every VFR interrupt or else the voice timer service
      will be stuck
   */
  if (NULL != vfr_state_ptr->vfr_client_info.vfr_clientq)
  {
    /* Get the buffer and fill the drift values. */
    if ( ADSP_FAILED(elite_mem_get_buffer(sizeof(elite_msg_custom_vfr_info_t), &bufMgrNode, &nActualSize)))
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VFR Handler: FAILED to get message buffer to send VFR msg, vfr_src_idx: %d", vfr_state_ptr->vfr_src_index);
      return;
    }

    pVfrMsg = (elite_msg_custom_vfr_info_t *) bufMgrNode.pBuffer;

    pVfrMsg->buffer_returnq_ptr = bufMgrNode.pReturnQ;
    pVfrMsg->responseq_ptr = NULL;
    pVfrMsg->sec_op_code = vfr_state_ptr->vfr_client_info.client_msg_id;

    pVfrMsg->vfr_info.vfr_client_info_ver = vfr_state_ptr->vfr_client_info.vfr_client_info_ver;
    pVfrMsg->vfr_info.resync_status = resync_status;
    pVfrMsg->vfr_info.vfr_source = vfr_state_ptr->vfr_client_info.vfr_source;
    pVfrMsg->vfr_info.vfr_av_timestamp_us = vfr_state_ptr->cur_av_time_stamp;

    eliteMsg.unOpCode = ELITE_CUSTOM_MSG;
    eliteMsg.pPayload = pVfrMsg;
    if (ADSP_EOK != (result = qurt_elite_queue_push_back(vfr_state_ptr->vfr_client_info.vfr_clientq, (uint64_t*)&eliteMsg)))
    {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "FAILED to send VFR CFG msg, vfr_src_idx: %d", vfr_state_ptr->vfr_src_index);
      elite_msg_release_msg(&eliteMsg);
    }
  }

  return;
}

/**
 * service loop to handle VFR
 *
 * @param[in] vpTerminateSignal, pointer to terminate signal.
 * @return the ADSP_EOK error code
 */
int vfr_svc_work_loop (void* vpCmdqPtr)
{
  uint32_t                unChannelStatus, waitMask, index=0;
  ADSPResult              result;
  qurt_elite_channel_t    vfrChannel;
  qurt_elite_queue_t      *vfr_cmdq_ptr = (qurt_elite_queue_t *) vpCmdqPtr;
  elite_svc_handle_t      vfrSvcHandle;
  vfr_src_state_t         *vfr_status_ptr = NULL;

  MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Entering VFR Service workloop...");

  /* Assign cmdQ ptr for VFR Svc handle */
  vfrSvcHandle.cmdQ = vfr_cmdq_ptr;

  /* Initialize the channel */
  qurt_elite_channel_init(&(vfrChannel));

  /* Add VFR svc cmdQ to the channel & update wait mask */
  qurt_elite_channel_addq(&vfrChannel, vfr_cmdq_ptr, (uint32_t) (1<<VFR_SVC_CMD_TASK_INDEX));
  waitMask = (uint32_t) (1 << VFR_SVC_CMD_TASK_INDEX);


  /* Initialize VFR driver */
  if (ADSP_EOK != (result = vfr_driver_init(&vfr_drv_global_stat)))
  {
    result = ADSP_EFAILED;
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE VFR Driver Init failed");
  }

  /* Initialize VFR signals for each of the sources and add it to the wait mask */
  for (index=0; index < NUM_MAX_VFR_SRC; index++)
  {
    vfr_status_ptr = &vfr_drv_global_stat.vfr_state_info[index];

    if (TRUE == vfr_status_ptr->is_initialized)
    {
      qurt_elite_signal_init(&(vfr_status_ptr->vfr_signal));
      qurt_elite_channel_add_signal(&vfrChannel, &vfr_status_ptr->vfr_signal, (uint32_t)(1<<index));
      waitMask |= (uint32_t)(1<<index);
    }

    vfr_status_ptr->vfr_client_info.vfr_clientq = NULL;
  }

  for(;;)
  {
    /* Block on any one or more of selected queues to get a msg */
    unChannelStatus = qurt_elite_channel_wait(&vfrChannel, waitMask) ;

    /* Get the signal index */
    /* Gets Least significant bit '1' position (i.e. count trailing 0's) */
    index = s32_get_lsb_s32(unChannelStatus);

    if (VFR_SVC_CMD_TASK_INDEX == index)
    {
      const uint32_t vfr_cmd_table_size = sizeof(vfr_cmd_handler)/sizeof(vfr_cmd_handler[0]);
      result = elite_svc_process_cmd_queue(NULL, &vfrSvcHandle, vfr_cmd_handler, vfr_cmd_table_size);

      if (ADSP_ETERMINATED == result)
      {
        MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VFR Svc: Breaking VFR thread.");
        break;
      }
    }
    else if (NUM_MAX_VFR_SRC > index)
    {
      vfr_drift_update(&(vfr_drv_global_stat.vfr_state_info[index]), &vfr_drv_global_stat.hw_info);
      qurt_elite_signal_clear(&(vfr_drv_global_stat.vfr_state_info[index].vfr_signal));
    }
    else
    {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Ignore Un-handled VFR signal: %lu", index);
    }
  }

  qurt_elite_channel_destroy(&vfrChannel);
  return ADSP_EOK;
}

ADSPResult vfr_svc_get_src_instance(uint8_t vfr_client_info_ver,
                                    uint32_t vfr_source,
                                    vfr_src_state_t **p_vfr_src_state)
{
  ADSPResult           result = ADSP_EOK;
  vfr_drv_state_t      *p_vfr_drv;
  uint32_t             vfr_src_idx, mux_ctl_sel;

  p_vfr_drv = &vfr_drv_global_stat;

  /* First check if the VFR client info version is valid
      If its valid, then check if the VFR source index is within range
      for that version
   */
  switch (vfr_client_info_ver)
  {
    case VFR_CLIENT_INFO_VER_1:
    {
      vfr_src_idx = vfr_source;

      if (vfr_src_idx > AFE_VFR_SOURCE_EXTERNAL)
      {
        MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Out of Range VFR source index: %ld, vfr_client_info_ver: %u",
              vfr_src_idx, vfr_client_info_ver);
        return ADSP_EBADPARAM;
      }
    }
    break;
    case VFR_CLIENT_INFO_VER_2:
    {
      mux_ctl_sel =  ((vfr_source & VSID_VFR_BIT_MASK) >> VSID_VFR_ID_MASK_BIT_POS);

      if (mux_ctl_sel > VFR_IRQ_SRC_END_IDX)
      {
        MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Out of Range VFR source index: %ld, vfr_client_info_ver: %u",
              mux_ctl_sel, vfr_client_info_ver);
        return ADSP_EBADPARAM;
      }

      if( VFR_IRQ_SRC_INVALID == (vfr_src_idx = p_vfr_drv->vfr_irq_src_map[mux_ctl_sel]) )
      {
        MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Un-supported VFR source index: %ld, vfr_client_info_ver: %u",
              vfr_src_idx, vfr_client_info_ver);

        return ADSP_EUNSUPPORTED;
      }
    }
    break;
    default:
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Un-supported VFR client info ver: %u", vfr_client_info_ver);
      return ADSP_EBADPARAM;
    }
    break;
  }

  /* Get the VFR instance pointer */
  *p_vfr_src_state = &p_vfr_drv->vfr_state_info[vfr_src_idx];

  /* Check if this VFR source is hw supported or not */
  if (FALSE == (*p_vfr_src_state)->is_initialized)
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Un-supported VFR source ID: %ld", vfr_src_idx);

    p_vfr_src_state = NULL;

    result = ADSP_EFAILED;
  }

  return result;
}

/**
 * This function enables VFR on the driver, specifies VFR source
 * & voice timer's queue Note: this is not per port request. by 
 * design VFR is enabled on all ports when they are started. 
 */
ADSPResult vfr_svc_enable_vfr_src(elite_msg_any_t* pMsg)
{
  ADSPResult                 result = ADSP_EOK;
  vfr_src_state_t            *p_vfr_src_state = NULL;
  vfr_client_info_t          vfr_client_info_payload;

  vfr_client_info_payload = ((elite_msg_custom_afe_vfr_config_info_t *) (pMsg->pPayload))->client_vfr_info;

  if ( ADSP_EOK == (result = vfr_svc_get_src_instance(vfr_client_info_payload.vfr_client_info_ver,
                                                      vfr_client_info_payload.vfr_source,
                                                      &p_vfr_src_state)) )
  {
    if(TRUE == p_vfr_src_state->is_vfr_enabled)
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VFR src already enabled, vfr_src_index: %d", p_vfr_src_state->vfr_src_index);
      result = ADSP_EALREADY;
    }
    else /* VFR source not already enabled */
    {
      /* Copy the client information */
      p_vfr_src_state->vfr_client_info = vfr_client_info_payload;

      /* Enable the VFR driver for the requested vfr src */
      if( ADSP_EOK != (result = vfr_enable_src_driver(&vfr_drv_global_stat, p_vfr_src_state)) )
      {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error occured in enabling VFR src index: %d", p_vfr_src_state->vfr_src_index);
        result = ADSP_EFAILED;
      }
    }
  }

  /* Finish message will ACK back if response Q is present,
   else return the buffer to buffer manager */
  elite_msg_finish_msg(pMsg, result);

  return result;
}

/**
 * This function disables VFR on the driver
 */
ADSPResult vfr_svc_disable_vfr_src(elite_msg_any_t* pMsg)
{
  ADSPResult                    result = ADSP_EOK;
  vfr_src_state_t               *p_vfr_src_state = NULL;
  vfr_drv_state_t               *p_vfr_drv;
  vfr_client_info_t             vfr_client_info_payload;

  vfr_client_info_payload = ((elite_msg_custom_afe_vfr_config_info_t *) (pMsg->pPayload))->client_vfr_info;

  if ( ADSP_EOK == (result = vfr_svc_get_src_instance(vfr_client_info_payload.vfr_client_info_ver,
                                                      vfr_client_info_payload.vfr_source,
                                                      &p_vfr_src_state)) )
  {
    p_vfr_drv = &vfr_drv_global_stat;

    if (TRUE == p_vfr_src_state->is_vfr_enabled)
    {
      /* Disable the VFR driver for the requested vfr src */
      vfr_disable_src_driver(p_vfr_drv, p_vfr_src_state);
    }
    else /* If the VFR source is already disabled */
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VFR src already disabled, vfr_src_index: %d", p_vfr_src_state->vfr_src_index);
      result = ADSP_EALREADY;
    }
  }

  /* Finish message will ACK back if response Q is present,
   else return the buffer to buffer manager */
  elite_msg_finish_msg(pMsg, result);

  return ADSP_EOK;
}

/**
 * This function enables single VFR source on a particular 
 * port. Note that call to the port driver functions is just to 
 * init the software counters. 
 * Actual VFR is enabled only when audio_if function is called. 
 */
ADSPResult afe_drv_add_to_vfr_sync(afe_drift_info_t *p_drift_info, 
                                   uint32_t sample_rate,
                                   uint16_t port_id)
{
  uint8_t                    src_idx;
  vfr_src_state_t            *p_vfr_src_state;

  for (src_idx = 0; src_idx < NUM_MAX_VFR_SRC; src_idx++)
  {
    p_vfr_src_state = &vfr_drv_global_stat.vfr_state_info[src_idx];

    if(TRUE == p_vfr_src_state->is_initialized)
    {
      p_drift_info[src_idx].is_first_vfr = TRUE;
      p_drift_info[src_idx].running_vfr_int_counter = 0;
      p_drift_info[src_idx].vfr_drift_info = 0;
      p_drift_info[src_idx].dev_latched_counter_curr = 0;
      p_drift_info[src_idx].dev_latched_counter_prev = 0;
      p_drift_info[src_idx].inst_sample_drift = 0;
      p_drift_info[src_idx].vfr_acc_sample_drift_to_update = 0;
      p_drift_info[src_idx].prev_avt_drift_info = 0;
      p_drift_info[src_idx].running_dev_int_counter = 0;
      p_drift_info[src_idx].init_av_timestamp = 0;
      p_drift_info[src_idx].latest_dev_av_timestamp = 0;


      if (IS_RT_PROXY_PORT_AFE_ID(port_id))
      {
        afe_rt_proxy_add_to_vfr_sync(port_id, src_idx);
      }

      /* Number of samples per VFR frame (20 ms equivalnet)
            as per port's sampling rate */
      p_drift_info[src_idx].samples_in_vfr_frame = (int16_t) divide_int32(((int32_t)VFR_FRAME_SIZE_IN_US*(sample_rate)), (int32_t) NUM_MICROSEC_PER_SEC);

      qurt_elite_mutex_lock(&p_drift_info[src_idx].port_vfr_mutex);

      p_drift_info[src_idx].vfr_enabled = TRUE;

      qurt_elite_mutex_unlock(&p_drift_info[src_idx].port_vfr_mutex);


#ifdef VFR_DEBUG_MSG
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE VFR sync enable, samples_in_vfr_frame: %ld, VFR source index: %u",
            p_drift_info[src_idx].samples_in_vfr_frame, src_idx);
#endif
    }
    else /* Un-supported VFR source */
    {
      p_drift_info[src_idx].vfr_enabled  = FALSE;
    }
  } /* For loop */

  return ADSP_EOK;
}

void afe_drv_resync_port_vfr_drift_info(afe_drift_info_t *p_drift_info)
{
  p_drift_info->vfr_enabled  = TRUE;
  p_drift_info->vfr_drift_info = 0;
  p_drift_info->dev_latched_counter_prev = 0;
  p_drift_info->inst_sample_drift = 0;
  p_drift_info->vfr_acc_sample_drift_to_update = 0;
  p_drift_info->prev_avt_drift_info = 0;
  p_drift_info->running_dev_int_counter = 0;
  p_drift_info->running_vfr_int_counter = 0;

  /* Reset the init Device AV TS and fifo count with latest values */
  p_drift_info->init_av_timestamp = p_drift_info->latest_dev_av_timestamp;
  p_drift_info->init_fifo_samples = p_drift_info->latest_fifo_samples;

  return;
}

/**
 * This function disables VFR source for a particular port
 */
ADSPResult afe_drv_remove_from_vfr_sync(afe_drift_info_t *p_drift_info)
{
  uint8_t src_idx;
  vfr_src_state_t *p_vfr_src_state;

  for (src_idx = 0; src_idx < NUM_MAX_VFR_SRC; src_idx++)
  {
    p_vfr_src_state = &vfr_drv_global_stat.vfr_state_info[src_idx];
    if(TRUE == p_vfr_src_state->is_initialized)
    {
      qurt_elite_mutex_lock(&p_drift_info[src_idx].port_vfr_mutex);

      p_drift_info[src_idx].vfr_enabled = FALSE;

      qurt_elite_mutex_unlock(&p_drift_info[src_idx].port_vfr_mutex);

      p_drift_info[src_idx].is_first_vfr = FALSE;
      p_drift_info[src_idx].samples_in_vfr_frame = 0;
      p_drift_info[src_idx].dev_latched_counter_curr = 0;
      p_drift_info[src_idx].dev_latched_counter_prev = 0;
      p_drift_info[src_idx].inst_sample_drift = 0;
      p_drift_info[src_idx].vfr_acc_sample_drift_to_update = 0;
      p_drift_info[src_idx].prev_avt_drift_info = 0;
    }
  }

  // Note: don't reset vfr drift info during port stop. this is design expectation.
  //pDevPort->drift_info.vfr_drift_info = 0;
  return ADSP_EOK;
}

/**
 * This function is command handler for general ELITE_CUSTOM_MSG.
 *
 * @param[in] pAudStatAfeSvc, pointer to AfeSvc instance.
 * @param[in] pMsg, pointer to message.
 * @return the ADSP_EOK error code
 */
static ADSPResult vfr_svc_cmd_handler(void* dummy, elite_msg_any_t* pMsg)
{
  ADSPResult     result;
  uint32_t       cmd;

  cmd = ((elite_msg_custom_header_t *)pMsg->pPayload)->unSecOpCode;

  switch (cmd)
  {
    case ELITEMSG_CUSTOM_VFR_SYNC:
    {
      result = vfr_svc_enable_vfr_src(pMsg);
    }
    break;
    case ELITEMSG_CUSTOM_VFR_DISABLE:
    {
      result = vfr_svc_disable_vfr_src(pMsg);
    }
    break;
    case ELITEMSG_CUSTOM_TERMINATE_THREAD: //terminates the thread
    {
      result = ADSP_ETERMINATED;
      elite_msg_return_payload_buffer(pMsg); //no ack
    }
    break;
    default:
    {
      result = ADSP_EBADPARAM;
      elite_msg_ack_msg(pMsg, ADSP_EBADPARAM);
    }
  }

  return result;
}

/**
 * function that sends a message to kill VFR thread. This function is called from AFE static service thread.
 */
ADSPResult afe_svc_vfr_send_kill_msg(qurt_elite_queue_t *vfr_cmdq_ptr)
{
  elite_msg_any_t                 cmd;
  qurt_elite_bufmgr_node_t        bufMgrNode;
  int                             nActualSize;
  elite_msg_custom_afe_header_t   *pPayload;
  ADSPResult                      result = ADSP_EOK;

  if ( ADSP_SUCCEEDED(result = elite_mem_get_buffer(sizeof(elite_msg_custom_afe_header_t), &bufMgrNode, &nActualSize)) )
  {
    pPayload = (elite_msg_custom_afe_header_t*) bufMgrNode.pBuffer;
    pPayload->buffer_returnq_ptr = bufMgrNode.pReturnQ;
    pPayload->afe_id = 0; //doesn't matter
    pPayload->client_token = 0;
    pPayload->response_result = 0;
    pPayload->sec_op_code = ELITEMSG_CUSTOM_TERMINATE_THREAD;
    pPayload->responseq_ptr = NULL;

    cmd.unOpCode = ELITE_CUSTOM_MSG;
    cmd.pPayload = (void*) pPayload;

    result = qurt_elite_queue_push_back(vfr_cmdq_ptr, (uint64_t*) &cmd);
    if (ADSP_FAILED(result))
    {
      elite_msg_return_payload_buffer(&cmd);
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "failed to send cmd to kill vfr thread !!");
    }
  }
  else
  {
    if (ADSP_FAILED(result))
    {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "failed to create cmd to kill vfr thread !!");
    }
  }

  return result;
}

void vfr_handler_deinit(void)
{
  ADSPResult                 result;
  int                        vfr_thread_exit_status;

  /* De-init the VFR driver */
  vfr_driver_deinit(&vfr_drv_global_stat);

  /* Send command to kill VFR thread */
  result = afe_svc_vfr_send_kill_msg(vfr_drv_global_stat.vfr_cmdq_ptr);

  if (ADSP_SUCCEEDED(result))
  {
    qurt_elite_thread_join(vfr_drv_global_stat.vfr_thread_id, &vfr_thread_exit_status);

    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VFR thread joined: status = %d", vfr_thread_exit_status);

    if (ADSP_FAILED(vfr_thread_exit_status))
    {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VFR Driver: Failed while joining VFR thread: %d", vfr_drv_global_stat.vfr_thread_id);
      result = ADSP_EFAILED;
    }
  }

  /* De-init the VFR service command queue */
  qurt_elite_queue_deinit(vfr_drv_global_stat.vfr_cmdq_ptr);

  return;
}

ADSPResult vfr_handler_init(void)
{
  ADSPResult                 result = ADSP_EOK;
  qurt_elite_queue_t         *vfr_cmdq_ptr;
  vfr_drv_state_t            *p_vfr_driver = &vfr_drv_global_stat;

  /* Clear the VFR driver structure */
  memset(p_vfr_driver, 0, sizeof(vfr_drv_state_t));

  if(ADSP_EOK != vfr_device_cfg_init(p_vfr_driver))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VFR Device Cfg Failed");
    return result;
  }

  vfr_cmdq_ptr  = (qurt_elite_queue_t *)&vfrCmdQBuf;
  result        = qurt_elite_queue_init((char*)"VfrCmdQ", VFR_MAX_CMD_Q_ELEMENTS, vfr_cmdq_ptr);

  vfr_drv_global_stat.vfr_cmdq_ptr = vfr_cmdq_ptr;

  /* Launch VFR thread */
  if (ADSP_FAILED (result = qurt_elite_thread_launch(&(vfr_drv_global_stat.vfr_thread_id), VFR_THREAD_NAME, \
                                                     NULL, VFR_THREAD_STACK_SIZE,
                                                     elite_get_thrd_prio(ELITETHREAD_STAT_AFE_VFR_PRIO_ID), \
                                                     vfr_svc_work_loop, (void*) vfr_cmdq_ptr, QURT_ELITE_HEAP_DEFAULT)))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE VFR service: Failed to create VFR thread");

    return result;
  }

  return result;
}

void vfr_drv_update_acc_sample_drift(afe_drift_info_t *p_drift_info, uint32_t num_vfr_sample)
{
  int16_t inst_drift_in_sample;

  /* Instantaneous drift in samples */
  inst_drift_in_sample = (p_drift_info->samples_in_vfr_frame - num_vfr_sample);

  /* Accumulate the instantaneous drift for thresholding */
  p_drift_info->inst_sample_drift += inst_drift_in_sample;

  /* If instantaneous drift is above +/- threshold, increment the acc drift value */
  if ( (p_drift_info->inst_sample_drift <= VFR_SW_LATCH_DRIFT_NEG_THRESH) ||
      (p_drift_info->inst_sample_drift >= VFR_SW_LATCH_DRIFT_POS_THRESH))
  {
    /* If drift is in +tive direction */
    if (p_drift_info->inst_sample_drift > 0)
    {
      /* Increment sample drift by 1 sample */
      p_drift_info->vfr_acc_sample_drift_to_update += 1;

      /* Decrement the drift acounted for */
      p_drift_info->inst_sample_drift -= 1;
    }
    else if(p_drift_info->inst_sample_drift < 0)/* If drift is in -tive direction */
    {
      /* Increment sample drift by 1 sample */
      p_drift_info->vfr_acc_sample_drift_to_update -= 1;

      /* Decrement the drift acounted for */
      p_drift_info->inst_sample_drift += 1;
    }
  }

  return;
}


void vfr_drv_update_btfm_acc_sample_drift(afe_drift_info_t *p_drift_info,
                                          uint32_t sample_rate,
                                          int32_t *acc_drift_in_us,
                                          uint16_t vfr_src_idx,
                                          int32_t port_id,
                                          int32_t *num_vfr_sample)
{
  int32_t                 curr_avt_drift_in_us, prev_avt_drift_in_us, inst_drift_in_us;
  uint64_t                curr_avt_drift_info;
  int32_t                 acc_drift_in_sample;
  int16_t                 inst_drift_in_sample;
  uint32_t                frame_counter_diff;

  *num_vfr_sample = 0;

  /* Read AV timer drift info for this port */
  curr_avt_drift_info = p_drift_info->avt_drift_info;

  frame_counter_diff = ( (uint32_t)(curr_avt_drift_info) - (uint32_t)(p_drift_info->prev_avt_drift_info) );

  inst_drift_in_us = 0;

  if ( (frame_counter_diff < AFE_AV_TIMER_DRIFT_FRM_CNTR_LOW_THRESH) ||
      (frame_counter_diff > AFE_AV_TIMER_DRIFT_FRM_CNTR_HIGH_THRESH) )
  {
    p_drift_info->prev_avt_drift_info = curr_avt_drift_info;
  }
  else /* Valid frame counter diff to read the AVT drift */
  {
    /* Read the AV timer drift for this port */
    curr_avt_drift_in_us = (int32_t)(curr_avt_drift_info >> AFE_ACC_AVT_DRIFT_US_SHIFT);

    prev_avt_drift_in_us = (int32_t)(p_drift_info->prev_avt_drift_info >> AFE_ACC_AVT_DRIFT_US_SHIFT);

    /* Calculate instantaneous drift in us */
    inst_drift_in_us = (curr_avt_drift_in_us - prev_avt_drift_in_us);

    /* Update the previous drift info */
    p_drift_info->prev_avt_drift_info = curr_avt_drift_info;
  }

  /* Derive the current AV timer time stamp for the device */
  p_drift_info->curr_vfr_intr_dev_av_timestamp += (VFR_FRAME_SIZE_IN_US + inst_drift_in_us);

  /* Calulcate accumulated drift between VFR and device */
  *acc_drift_in_us = (p_drift_info->curr_vfr_intr_dev_av_timestamp - p_drift_info->dev_latched_counter_curr);

  /* Convert drift in time to samples */
  acc_drift_in_sample = divide_int32( ( (*acc_drift_in_us) * (sample_rate/NUM_MILLISEC_PER_SEC) ), NUM_MILLISEC_PER_SEC );

  /* Calculate instantenous drift */
  inst_drift_in_sample = (acc_drift_in_sample - p_drift_info->vfr_acc_sample_drift_to_update);

#ifdef VFR_BTFM_DEBUG_MSG
  MSG_6(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "port-id: 0x%x, vfr_src_idx: %u, VFR_AV_TS: %ld, BTFM_AV_TS: %ld, BTFM_AVT_INST_DRIFT: %ld, acc_drift_in_us: %ld",
        port_id, vfr_src_idx, (uint32_t)p_drift_info->dev_latched_counter_curr,\
        (uint32_t)p_drift_info->curr_vfr_intr_dev_av_timestamp, inst_drift_in_us, *acc_drift_in_us);
#endif

#ifdef VFR_DEBUG_MSG
  *num_vfr_sample = p_drift_info->samples_in_vfr_frame;
#endif      

  if ( (inst_drift_in_sample <= VFR_SW_LATCH_DRIFT_NEG_THRESH) ||
      (inst_drift_in_sample >= VFR_SW_LATCH_DRIFT_POS_THRESH))
  {
    /* If drift is in +tive direction */
    if (inst_drift_in_sample > 0)
    {
      /* Increment sample drift by 1 sample */
      p_drift_info->vfr_acc_sample_drift_to_update += 1;

#ifdef VFR_DEBUG_MSG
      *num_vfr_sample = p_drift_info->samples_in_vfr_frame - 1;
#endif   
    }
    else if(inst_drift_in_sample < 0)/* If drift is in -tive direction */
    {
      /* Increment sample drift by 1 sample */
      p_drift_info->vfr_acc_sample_drift_to_update -= 1;

#ifdef VFR_DEBUG_MSG
      *num_vfr_sample = p_drift_info->samples_in_vfr_frame + 1;
#endif      
    }
  } /* If inst drift above threholds */

  return;
}

