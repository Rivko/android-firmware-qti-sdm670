/*========================================================================
 This file implements functions for handling AVtimer drift of a port.
 It also addresses loopback rate matching.

 Copyright (c) 2009-2012,2017 QUALCOMM Technologies, Inc.  All Rights Reserved.
 QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
 by the U.S. Government, Diversion contrary to U.S. law prohibited.

 $Header: //components/rel/avs.adsp/2.8.5/afe/services/static/src/AFEAvtDrift.cpp#1 $
 ====================================================================== */

/*==========================================================================
 the Include files
 ========================================================================== */
#include "AFEPortManagement.h"
#include "AFE_private_defs_i.h"

/**
 * General AVT drift update function: --------------------------------------------------
 */

/**
 * updates the drift_info every 100ms (AFE_AV_TIMER_DRIFT_CALC_PERIOD)
 */
void afe_port_update_avt_drift(afe_dev_port_t *dev_port_ptr)
{

   /** no drift calculations for pseudoports */
   if(IS_PSEUDO_PORT_AFE_ID(dev_port_ptr->intf))
   {
      dev_port_ptr->running_int_counter++;
      return;
   }

   //this flag enables, sending drift to registered clients.
   dev_port_ptr->avt_drift_params.is_avt_drift_updated = FALSE;

   /** below condition is true the first interrupt (periodic_int_counter=0) & later every 100 interrupts, */
   if(AFE_AV_TIMER_DRIFT_CALC_PERIOD
         <= (dev_port_ptr->periodic_int_counter * (dev_port_ptr->int_samples_per_period * 1000))
         / dev_port_ptr->sample_rate ||
         TRUE == dev_port_ptr->is_first_dma)
   {
      afe_drv_update_avt_drift(dev_port_ptr);
      if(TRUE == dev_port_ptr->is_first_dma)
      {
        dev_port_ptr->avt_drift_params.resync_flag = TRUE;
      }
      dev_port_ptr->avt_drift_params.avt_timestamp = dev_port_ptr->port_av_timestamp;
      dev_port_ptr->avt_drift_params.is_avt_drift_updated = TRUE;
      dev_port_ptr->periodic_int_counter = 0;
      dev_port_ptr->is_first_dma = FALSE;

#ifdef AVT_BASED_AFE_DRIFT_DEBUG
      afe_client_avt_drift_info_t drift_info_struct;
      afe_port_read_avt_drift(&(dev_port_ptr->drift_info[AVT_DRIFT_INFO_IDX].avt_drift_info), &drift_info_struct);
      MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "port_id %x, ACC_AVT_DRIFT = %d, frame_cntr=%d",dev_port_ptr->intf, drift_info_struct.acc_avt_drift_us, drift_info_struct.frame_counter);
#endif
   }

   dev_port_ptr->periodic_int_counter++;
   dev_port_ptr->running_int_counter++;

   /* Update VFR state information for each VFR source.
    For RIVA this should be done at RIVA interrupt, not here, since STC is latched for RIVA interrupt */
   if(!IS_RIVA_PORT_AFE_ID(dev_port_ptr->intf)
         && (VFR_HW_LATCH_V2 == vfr_drv_global_stat.hw_latch_ver))
   {
      afe_device_update_port_vfr_drift_info(dev_port_ptr, dev_port_ptr->port_av_timestamp);
   }
}

/**
 * sends drif_info to the registered clients
 */
void afe_port_send_avt_drift_update(afe_dev_port_t *dev_port_ptr,
                                    afe_client_info_t *client_info_ptr)
{
   ADSPResult result;
   int actual_size;
   qurt_elite_bufmgr_node_t buf_mgr_node;
   elite_msg_custom_avt_drift_update_t *avt_msg;
   elite_msg_any_t elite_msg;

   /** for audio client that register for drift */
   if( TRUE == client_info_ptr->afe_client.cust_proc.subscribe_to_avt_drift &&
         NULL != client_info_ptr->afe_client.afe_client_handle->cmdQ)
   {
      /** send a message over the cmdQ with port id */
      if(ADSP_FAILED(
            elite_mem_get_buffer(sizeof(elite_msg_custom_vfr_info_t), &buf_mgr_node, &actual_size)))
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "FAILED to get buf for AVT drift update msg");
         return;
      }

      avt_msg = (elite_msg_custom_avt_drift_update_t *)buf_mgr_node.pBuffer;

      avt_msg->buffer_returnq_ptr = buf_mgr_node.pReturnQ;
      avt_msg->responseq_ptr = NULL;
      avt_msg->sec_op_code = ELITEMSG_CUSTOM_AFE_AVT_DRIFT_UPDATE;
      avt_msg->port_id = dev_port_ptr->intf;

      elite_msg.unOpCode = ELITE_CUSTOM_MSG;
      elite_msg.pPayload = avt_msg;

      if(ADSP_FAILED(
            (result = qurt_elite_queue_push_back(
                  client_info_ptr->afe_client.afe_client_handle->cmdQ, (uint64_t* )&elite_msg))))
      {
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "FAILED to send AVT drift update msg");
         elite_msg_release_msg(&elite_msg);
      }
      else
      {
         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "sent drift update msg port_id:%x,client_id: 0x%lX",dev_port_ptr->intf, client_info_ptr->client_id);
      }
   }
}

ADSPResult afe_port_get_drift_params(void *dev_port, uint32_t param_id, int8_t* param_buffer_ptr,
                                     int32_t param_buf_len, uint16_t* params_buffer_len_req_ptr)
{
   uint32_t result = ADSP_EOK;
   afe_dev_port_t *pDevPort = (afe_dev_port_t*)dev_port;

   switch(param_id)
   {
      case AFE_PARAM_ID_AVT_DRFT:
      {

         *params_buffer_len_req_ptr = sizeof(afe_avt_drift_param_t);
         if(*params_buffer_len_req_ptr > param_buf_len)
         {
            MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,
                  "afe_dev_port_t fail, required size = %d, given size = %lu",
                  *params_buffer_len_req_ptr, param_buf_len);
            return ADSP_EBADPARAM;
         }

         afe_avt_drift_param_t *req_cfg_ptr = (afe_avt_drift_param_t *)param_buffer_ptr;

         req_cfg_ptr->frame_counter = (uint32_t)pDevPort->drift_info[0].avt_drift_info;
         req_cfg_ptr->drift_value = (uint32_t)(pDevPort->drift_info[0].avt_drift_info >> 32);

         break;
      }
      case AFE_PARAM_ID_VFR_DRFT:
      {
         *params_buffer_len_req_ptr = sizeof(afe_vfr_drift_param_t);
         if(*params_buffer_len_req_ptr > param_buf_len)
         {
            MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,
                  "afe_dev_port_t fail, required size = %d, given size = %lu",
                  *params_buffer_len_req_ptr, param_buf_len);
            return ADSP_EBADPARAM;
         }

         afe_vfr_drift_param_t *req_cfg_ptr = (afe_vfr_drift_param_t *)param_buffer_ptr;

         for(uint32_t i = 0; i < NUM_MAX_VFR_SRC; i++)
         {
            req_cfg_ptr->frame_counter[i] = (uint32_t)pDevPort->drift_info[i].vfr_drift_info;
            req_cfg_ptr->drift_value[i] = (uint32_t)(pDevPort->drift_info[i].vfr_drift_info >> 32);
         }

         break;
      }

      default:
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
               "AFE_MODULE_DRIFT: Un-Supported PARAM ID: %ld, Port_id: 0x%x",
               param_id, pDevPort->intf);
         result = ADSP_EUNSUPPORTED;
         break;
      }
   }

   return result;

}

ADSPResult afe_get_timing_stats(void *dev_port, int8_t* param_buffer_ptr,
                                     int32_t param_buf_len, uint16_t* params_buffer_len_req_ptr)
{
    uint32_t result = ADSP_EOK;
    afe_dev_port_t *pDevPort = (afe_dev_port_t*)dev_port;

    *params_buffer_len_req_ptr = sizeof(afe_param_id_dev_timing_stats_t);
    if(*params_buffer_len_req_ptr > param_buf_len)
    {
         MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
              "afe_dev_timing_stats fail, port_id:0x%x, required size = %d, given size = %d",
              pDevPort->intf, *params_buffer_len_req_ptr, param_buf_len);
         return ADSP_EBADPARAM;
    }
    if (AFE_PORT_STATE_RUN == pDevPort->port_state)
    {
        afe_param_id_dev_timing_stats_t *req_cfg_ptr = (afe_param_id_dev_timing_stats_t *)param_buffer_ptr;

        req_cfg_ptr->minor_version = AFE_API_VERSION_DEV_TIMING_STATS;
        req_cfg_ptr->device_direction = (pDevPort->intf & AFE_PORT_DIR_MASK ) ? AFE_SOURCE_DEVICE: AFE_SINK_DEVICE;
        req_cfg_ptr->reference_timer = AFE_REF_TIMER_TYPE_AVTIMER;
        
        afe_port_mutex_lock(pDevPort);
        
        req_cfg_ptr->resync_flag = pDevPort->avt_drift_params.resync_flag;
        req_cfg_ptr->acc_drift_value = (int32_t)(pDevPort->drift_info[0].avt_drift_info >> 32);
        req_cfg_ptr->ref_timer_abs_ts_lsw = (uint32_t) pDevPort->avt_drift_params.avt_timestamp;
        req_cfg_ptr->ref_timer_abs_ts_msw = (uint32_t) (pDevPort->avt_drift_params.avt_timestamp >> 32);
        
        afe_port_mutex_unlock(pDevPort);
        
        //resets resync flag after reading
        pDevPort->avt_drift_params.resync_flag = FALSE;

        MSG_7(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
        "afe_dev_timing_stats:port_id:0x%x, direction:%d, reference_timer:%d, resync_flag:%d, acc_drift_value:%d, absoulte_time:[MSW: %lu, LSW: %lu] ",
         pDevPort->intf,req_cfg_ptr->device_direction,req_cfg_ptr->reference_timer,req_cfg_ptr->resync_flag,
         req_cfg_ptr->acc_drift_value,req_cfg_ptr->ref_timer_abs_ts_msw,req_cfg_ptr->ref_timer_abs_ts_lsw );
       
    }
    else
    {
        MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
          "afe_dev_timing_stats fail, device is not active, port_id:0x%x, afe_port_state:%d",
           pDevPort->intf, pDevPort->port_state);
        return ADSP_ENOTREADY;
    }

    return result;
}
