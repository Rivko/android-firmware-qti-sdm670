/** @file AFESvc_Appi.cpp

 @brief This file contains AFE Service topology utility
 functions

 */

/*========================================================================
 $Header: //components/rel/avs.adsp/2.8.5/afe/services/static/src/AFESvc_topo.cpp#2 $

 Edit History

 when       who     what, where, why
 --------   ---     -------------------------------------------------------
 04/09/2014 mangeshk   Initial version
 ==========================================================================*/


/*-----------------------------------------------------------------------
   Copyright (c) 2012-2014 Qualcomm  Technologies, Inc.  All rights reserved.
   Qualcomm Technologies Proprietary and Confidential.
 -----------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "AFESvc_topo.h"
#include "topo_utils.h"
#include "bgt_handler_fwk.h"
#include "AFEInternal.h"
#include "AFESvc_topo_db.h"
#include "adsp_media_fmt.h"
#include "Elite_fwk_extns_feedback.h"
#include "Elite_fwk_extns_codec_interrupt.h"
#include "feedback_module.h"
#include "EliteCmnTopology_db_if.h"
#include "adsp_core_api.h"
#include "adsp_afe_service_modules_params.h"

/*
  Callback function for modules in topology

  @param[in]     context_ptr: Context pointer
  @param[in]     id: event id
  @param[in]     event_ptr: pointer to the event

  @return
  capi_v2_err_t

  @dependencies
  None.
*/
capi_v2_err_t afe_topo_event_callback(void *context_ptr, capi_v2_event_id_t id, capi_v2_event_info_t *event_ptr)
{ 
  if(!(context_ptr && event_ptr))
  {
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
          "AFE svc: Null ptr context_ptr(%p), event_ptr(%p)",
          context_ptr, event_ptr);
    return CAPI_V2_EFAILED;
  }

  topo_t *topo_ptr = topo_event_get_struct_ptr(context_ptr);
  uint32_t module_index = topo_event_get_module_index(context_ptr);

  if(module_index >= topo_ptr->num_modules)
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
          "AFE svc: Module with id 0x%lx array out of bound",
          module_index);
    return CAPI_V2_EBADPARAM;
  }

  if(id != CAPI_V2_EVENT_DATA_TO_DSP_SERVICE)
  {
    MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE Svc: Module with id 0x%lX raised event id %lu.",
          topo_ptr->module[module_index].module_id, (uint32_t)(id));
  }

  switch(id)
  {
     case CAPI_V2_EVENT_KPPS:
     {
       if(event_ptr->payload.actual_data_len < sizeof(capi_v2_event_KPPS_t))
       {
         MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE_svc: Module with id 0x%lX provided insufficient size for event id %lu. Required: %d bytes, provided: %lu bytes.",
               topo_ptr->module[module_index].module_id, (uint32_t)(id), sizeof(capi_v2_event_KPPS_t), event_ptr->payload.actual_data_len);
         return CAPI_V2_ENEEDMORE;
       }

       uint32_t old_kpps = topo_ptr->module[module_index].kpps;
       uint32_t new_kpps = ((capi_v2_event_KPPS_t *)(event_ptr->payload.data_ptr))->KPPS;

       uint32_t new_total_kpps = topo_ptr->kpps;

       new_total_kpps -= old_kpps;
       new_total_kpps += new_kpps;

       topo_ptr->module[module_index].kpps = new_kpps;
       topo_ptr->kpps = new_total_kpps;


       break;

     }

     case CAPI_V2_EVENT_BANDWIDTH:
     {
       if(event_ptr->payload.actual_data_len < sizeof(capi_v2_event_bandwidth_t))
       {
         MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
               "AFE_svc: Module with id 0x%lX provided insufficient size for event id %lu. Required: %d bytes, provided: %lu bytes.",
               topo_ptr->module[module_index].module_id, (uint32_t)(id), sizeof(capi_v2_event_bandwidth_t), event_ptr->payload.actual_data_len);
         return CAPI_V2_ENEEDMORE;
       }

       uint32_t old_bps = topo_ptr->module[module_index].bps;
       uint32_t new_code_bandwidth = ((capi_v2_event_bandwidth_t *)(event_ptr->payload.data_ptr))->code_bandwidth;
       uint32_t new_data_bandwidth = ((capi_v2_event_bandwidth_t *)(event_ptr->payload.data_ptr))->data_bandwidth;

       uint32_t new_bps = new_code_bandwidth + new_data_bandwidth; // Currently assuming AFE will not have special handling for keeping code in spl memory such as OCMEM

       uint32_t new_total_bps = topo_ptr->bps;

       new_total_bps -= old_bps;
       new_total_bps += new_bps;

       topo_ptr->module[module_index].bps = new_bps;
       topo_ptr->bps = new_total_bps;

       break;
     }

     case CAPI_V2_EVENT_ALGORITHMIC_DELAY:
     {
       if(event_ptr->payload.actual_data_len < sizeof(capi_v2_event_algorithmic_delay_t))
       {
         MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE_svc: Module with id 0x%lX provided insufficient size for event id %lu. Required: %d bytes, provided: %lu bytes.",
               topo_ptr->module[module_index].module_id, static_cast<uint32_t>(id), sizeof(capi_v2_event_algorithmic_delay_t), event_ptr->payload.actual_data_len);
         return CAPI_V2_ENEEDMORE;
       }

       uint32_t old_delay = topo_ptr->module[module_index].delay_in_us;
       uint32_t new_delay = reinterpret_cast<capi_v2_event_algorithmic_delay_t *>(event_ptr->payload.data_ptr)->delay_in_us;
       uint32_t old_total_delay = topo_ptr->delay_in_us;
       uint32_t new_total_delay = old_total_delay;

       new_total_delay -= old_delay;
       new_total_delay += new_delay;

       topo_ptr->module[module_index].delay_in_us = new_delay;
       topo_ptr->delay_in_us = new_total_delay;
       MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE_svc: Module with id 0x%lX changed delay from %lu us to %lu us. PP total delay changed from %lu us to %lu us.",
             topo_ptr->module[module_index].module_id, old_delay, new_delay, old_total_delay, new_total_delay);
       break;
     }

     case CAPI_V2_EVENT_DATA_TO_DSP_SERVICE:
     {
       if(event_ptr->payload.actual_data_len < sizeof(capi_v2_event_data_to_dsp_service_t))
       {
         MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
               "AFESvc : Module with id 0x%lx provided insufficient size for "
               "event id 0x%lx. Required: %lu bytes, provided: %lu bytes.",
               topo_ptr->module[module_index].module_id,
               (uint32_t)(id),
               sizeof(capi_v2_event_data_to_dsp_service_t),
               event_ptr->payload.actual_data_len);
         return CAPI_V2_ENEEDMORE;
       }
       capi_v2_event_data_to_dsp_service_t *data_ptr =
                                                       (capi_v2_event_data_to_dsp_service_t *)event_ptr->payload.data_ptr;

       if((NULL == data_ptr) || (NULL == data_ptr->payload.data_ptr))
       {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Null payload pointer 0x%lx", data_ptr);
         return CAPI_V2_EFAILED;
       }

       /* Routing Supporting Parameters */
       switch(data_ptr->param_id)
       {
          case CAPI_V2_PARAM_ID_FB_SRC_PUSH_BUF:
          {
            if(data_ptr->payload.actual_data_len < sizeof(capi_v2_fb_src_push_buf_t))
            {
              MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                    "AFESvc: Module with id 0x%lX provided insufficient size for "
                    "push buffer. Required: %d bytes, provided: %lu bytes.",
                    topo_ptr->module[module_index].module_id,
                    sizeof(capi_v2_fb_src_push_buf_t),
                    data_ptr->payload.actual_data_len);
              return CAPI_V2_ENEEDMORE;
            }

            capi_v2_fb_src_push_buf_t *push_buf_ptr = (capi_v2_fb_src_push_buf_t *)data_ptr->payload.data_ptr;
            feedback_send_buf_to_data_q((qurt_elite_queue_t *)push_buf_ptr->queue_ptr, push_buf_ptr->buf_ptr);

            break;
          }

          case CAPI_V2_PARAM_ID_FB_SRC_POP_BUF:
          {
            if(data_ptr->payload.actual_data_len < sizeof(capi_v2_fb_src_pop_buf_t))
            {
              MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                    "AFESvc: Module with id 0x%lX provided insufficient size for "
                    "push buffer. Required: %d bytes, provided: %lu bytes.",
                    topo_ptr->module[module_index].module_id,
                    sizeof(capi_v2_fb_src_pop_buf_t),
                    data_ptr->payload.actual_data_len);
              return CAPI_V2_ENEEDMORE;
            }

            capi_v2_fb_src_pop_buf_t *pop_buf_ptr = (capi_v2_fb_src_pop_buf_t *)data_ptr->payload.data_ptr;
            feedback_pop_buf_from_buf_q((qurt_elite_queue_t *)pop_buf_ptr->queue_ptr, pop_buf_ptr->buf_ptr_ptr);

            break;
          }

          case CAPI_V2_PARAM_ID_FB_DST_RTN_BUF_TO_SRC:
          {
            if(data_ptr->payload.actual_data_len < sizeof(capi_v2_fb_dst_rtn_buf_to_src_t))
            {
              MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                    "AFESvc: Module with id 0x%lX provided insufficient size for "
                    "push buffer. Required: %d bytes, provided: %lu bytes.",
                    topo_ptr->module[module_index].module_id,
                    sizeof(capi_v2_fb_dst_rtn_buf_to_src_t),
                    data_ptr->payload.actual_data_len);
              return CAPI_V2_ENEEDMORE;
            }

            capi_v2_fb_dst_rtn_buf_to_src_t *push_buf_ptr = (capi_v2_fb_dst_rtn_buf_to_src_t *)data_ptr->payload.data_ptr;
            feedback_push_buf_to_buf_q(push_buf_ptr->client_ptr, push_buf_ptr->buf_ptr);

            break;
          }

          case CAPI_V2_PARAM_ID_FB_DST_GET_BUF_FROM_SRC:
          {
            if(data_ptr->payload.actual_data_len < sizeof(capi_v2_fb_dst_get_buf_from_src_t))
            {
              MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                    "AFESvc: Module with id 0x%lX provided insufficient size for "
                    "push buffer. Required: %d bytes, provided: %lu bytes.",
                    topo_ptr->module[module_index].module_id,
                    sizeof(capi_v2_fb_dst_get_buf_from_src_t),
                    data_ptr->payload.actual_data_len);
              return CAPI_V2_ENEEDMORE;
            }

            capi_v2_fb_dst_get_buf_from_src_t *pop_buf_ptr = (capi_v2_fb_dst_get_buf_from_src_t *)data_ptr->payload.data_ptr;
            feedback_pop_buf_from_data_q(pop_buf_ptr->client_ptr, pop_buf_ptr->buf_ptr_ptr);
            break;
          }

          default:
          {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                  "AFESvc : Event-Param id 0x%lx raised by module 0x%lx is "
                  "not supported.",
                  data_ptr->param_id,
                  topo_ptr->module[module_index].module_id);

            return CAPI_V2_EUNSUPPORTED;
          }
       }
       break;
     }

     default:
       MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE_svc: Event id %lu raised by module 0x%lx is not supported.",
             (uint32_t)(id), topo_ptr->module[module_index].module_id);
       return CAPI_V2_EUNSUPPORTED;
  }

  return CAPI_V2_EOK;
}


/*
  This function is to set topology ID

  @param[out]     dev_port_ptr: pointer to AFE dev port
  @param[in]     cfg_ptr: pointer to configuration payload
  @param[in]     payload_size: size of the configuration payload

  @return
  ADSPResult

  @dependencies
  None.
*/
ADSPResult afe_set_topo_id(void *dev_port_ptr, int8_t *cfg_ptr, uint32_t payload_size)
{
  ADSPResult result = ADSP_EOK;
  afe_topo_id_v1_t *topo_def_ptr = (afe_topo_id_v1_t *)cfg_ptr;
  afe_dev_port_t *pDevPort = (afe_dev_port_t *)dev_port_ptr;
  topo_callback_info_t cb_info;

  cb_info.cb_f = (topo_cb_f)afe_topo_create_cb;
  cb_info.cb_arg = dev_port_ptr;

  MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE Svc: set topo_id for port:0x%x", pDevPort->intf);

  // Checking if topology is already created
  if((NULL != pDevPort->port_topo_ptr) || (TRUE == pDevPort->topo_rcvd_flag))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE Svc: Topology is already created or received for port 0x%x. Rejecting afe set topo id set param",
          pDevPort->intf);
    return ADSP_EBADPARAM;
  }

  if(payload_size < sizeof(afe_topo_id_v1_t))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Size of topo def config payload too small");
    return ADSP_EFAILED;
  }

  if(topo_def_ptr->topo_def_minor_version > AFE_API_VERSION_TOPO_ID)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Unsupported minor version");
    return ADSP_EUNSUPPORTED;
  }

  // Ignoring the topologies with special handling
  if((AFE_TX_TOPOLOGY_ID_AANC == topo_def_ptr->topology_id) || (AFE_TX_TOPOLOGY_ID_SWMAD == topo_def_ptr->topology_id) || (AFE_TX_TOPOLOGY_ID_SWMAD_HWMAD == topo_def_ptr->topology_id)
      || (AFE_RX_TOPOLOGY_ID_NONE == topo_def_ptr->topology_id) || (AFE_TX_TOPOLOGY_ID_NONE == topo_def_ptr->topology_id) || (AFE_TX_TOPOLOGY_ID_SW_DUTY_CYCLE_MAD == topo_def_ptr->topology_id))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE Svc: No need to create topology for Topology 0x%lX ", topo_def_ptr->topology_id);
    return ADSP_EOK;
  }

  elite_cmn_topo_db_entry_t *topology_ptr = NULL;
  uint32_t actual_size = 0;
  uint32_t handle = ELITE_CMN_TOPO_DB_INVALID_HANDLE;

  // Search the topology definition in common data base, if not, search in old data base
  result = elite_cmn_topo_db_get_topology(1 << AVCS_TOPO_CFG_AREA_AFE_BIT, topo_def_ptr->topology_id, &topology_ptr, &actual_size, &handle);
  if(ADSP_FAILED(result) || 0 == actual_size || 0 == handle || ELITE_CMN_TOPO_DB_INVALID_HANDLE == handle || NULL == topology_ptr)
  {
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE Svc: Topology 0x%lx is not present in common database, searching in old database", topo_def_ptr->topology_id);

    // get topo definition from Data base
    afe_topology_definition_t *topology_definition_ptr;
    result = topo_db_get_topology(TOPO_DB_CLIENT_TYPE_AFE, topo_def_ptr->topology_id, (const audproc_topology_definition_t **)&topology_definition_ptr);
    if(ADSP_FAILED(result))
    {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "AFE Svc: Topology 0x%lX not built-in and not in custom topologies.", topo_def_ptr->topology_id);
      return ADSP_EUNSUPPORTED;
    }

    // Ignoring the topologies with zero modules

    if(0 == topology_definition_ptr->num_modules)
    {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE Svc: Topology 0x%lX is having zero module, no need to create topology.", topo_def_ptr->topology_id);
      return ADSP_EOK;
    }

    // create the topology

    if(ADSP_EOK != (result = topo_create(&pDevPort->port_topo_ptr, topology_definition_ptr, afe_topo_event_callback, dev_port_ptr, &cb_info)))
    {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to create the topology for topology id 0x%lx :%d", topology_definition_ptr->topology_id, result);
      afe_topo_destroy((void *)pDevPort);
      pDevPort->port_topo_ptr = NULL;

      pDevPort->topo_rcvd_flag = FALSE;
      result = ADSP_EBADPARAM;
      return result;
    }
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE Svc: topology creation for Topology id 0x%lX is done successfully ", topo_def_ptr->topology_id);
  }
  else
  { 
    //Cross check whether actual_size returned from common database is valid
    uint32_t num_modules = topology_ptr->topo_def.num_modules;
    uint32_t size_req = (sizeof(elite_cmn_topo_db_entry_t) - sizeof(avcs_module_instance_info_t)) + (num_modules  * sizeof(avcs_module_instance_info_t));
    if(size_req != actual_size)
    {
      MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE Svc: Topology 0x%lx is present in common database. However, actual_size %lu does not match size_req %lu",
            topo_def_ptr->topology_id, actual_size, size_req);

      result = ADSP_EBADPARAM;
      goto common_db_topo_create_bail_out;
    }

    // Ignoring the topologies with zero modules
    if(0 == num_modules)
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE Svc: Topology 0x%lX is having zero module, no need to create topology.", topo_def_ptr->topology_id);
      result = ADSP_EOK;
      goto common_db_topo_create_bail_out;
    }

    // create the topology
    if(ADSP_EOK != (result = topo_create_common_db(&pDevPort->port_topo_ptr, topology_ptr, afe_topo_event_callback, dev_port_ptr, &cb_info)))
    {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to create the topology for topology id 0x%lx :%d", topo_def_ptr->topology_id, result);
      afe_topo_destroy((void *)pDevPort);
      pDevPort->port_topo_ptr = NULL;

      pDevPort->topo_rcvd_flag = FALSE;
      result = ADSP_EBADPARAM;
      goto common_db_topo_create_bail_out;
    }

    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE Svc: topology creation for Topology id 0x%lX is done successfully ", topo_def_ptr->topology_id);

common_db_topo_create_bail_out:
    (void)elite_cmn_topo_db_release_topology(handle);

    return result;
  }

  return result;
}

/*
  This function is to start the topology

  @param[out]     dev_port_ptr: pointer to AFE dev port
  
  @return
  ADSPResult

  @dependencies
  None.
*/
ADSPResult afe_topo_start(void *dev_port_ptr)
{
  ADSPResult result = ADSP_EOK;
  afe_dev_port_t *pDevPort = NULL;
  topo_t *topo_ptr;
  topo_media_fmt_t media_fmt;

  if(NULL == dev_port_ptr)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Null port pointer");
    return ADSP_EFAILED;
  }

  pDevPort = (afe_dev_port_t *)dev_port_ptr;

  topo_callback_info_t cb_info;

  topo_ptr = (topo_t *)pDevPort->port_topo_ptr;

  if(NULL == topo_ptr)
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Null topology pointer port id 0x%x", pDevPort->intf);
    return ADSP_EBADPARAM;
  }

  cb_info.cb_f = (topo_cb_f)afe_topo_start_cb;
  cb_info.cb_arg = (void *)pDevPort;

  memset(&media_fmt, 0, sizeof(topo_media_fmt_t));

  // Populating only needed parameters
  media_fmt.f.bits_per_sample = pDevPort->bit_width;
  media_fmt.f.num_channels = pDevPort->channels;
  media_fmt.f.sampling_rate = pDevPort->sample_rate;
  media_fmt.h.data_format = CAPI_V2_FIXED_POINT;
  media_fmt.f.bitstream_format = ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V2;
  media_fmt.f.q_factor = elite_data_get_q_format_factor(media_fmt.f.bits_per_sample);

  result = topo_start(pDevPort->port_topo_ptr, &media_fmt, pDevPort->int_samples_per_period, &cb_info);

  if(result != ADSP_EOK)
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Topo start failed for  port id 0x%x", pDevPort->intf);
    return result;
  }

  if(ADSP_FAILED(result = afe_mmpm_voting(pDevPort, AFE_MMPM_CUST_TOPOLOGY, AFE_MMPM_VOTE, TRUE)))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE topology mmpm voting failed for port 0x%x", pDevPort->intf);
  }

  return result;
}

/*
  This function is to stop the topology

  @param[out]     dev_port_ptr: pointer to AFE dev port
  
  @return
  ADSPResult

  @dependencies
  None.
*/
ADSPResult afe_topo_stop(void *dev_port_ptr)
{
  ADSPResult result = ADSP_EOK;
  afe_dev_port_t *pDevPort = NULL;
  topo_callback_info_t cb_info;

  if(NULL == dev_port_ptr)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Null port pointer");
    return ADSP_EFAILED;
  }

  pDevPort = (afe_dev_port_t *)dev_port_ptr;

  cb_info.cb_f = (topo_cb_f)afe_topo_stop_cb;
  cb_info.cb_arg = (void *)pDevPort;

  result = topo_stop(pDevPort->port_topo_ptr, &cb_info);

  if(result != ADSP_EOK)
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Topo stop failed for  port id 0x%x", pDevPort->intf);
  }

  result = afe_mmpm_voting(pDevPort, AFE_MMPM_CUST_TOPOLOGY, AFE_MMPM_DEVOTE, TRUE);
  if(result != ADSP_EOK)
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE topology mmpm voting failed for port 0x%x", pDevPort->intf);
  }

  return result;
}

/*
  This function is to destroy the topology

  @param[out]     dev_port_ptr: pointer to AFE dev port
  
  @return
  ADSPResult

  @dependencies
  None.
*/
ADSPResult afe_topo_destroy(void *dev_port_ptr)
{
  ADSPResult result = ADSP_EOK;
  afe_dev_port_t *pDevPort = NULL;
  topo_callback_info_t cb_info;

  if(NULL == dev_port_ptr)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Null port pointer");
    return ADSP_EFAILED;
  }

  pDevPort = (afe_dev_port_t *)dev_port_ptr;

  cb_info.cb_f = (topo_cb_f)afe_topo_destroy_cb;
  cb_info.cb_arg = dev_port_ptr;
  result = topo_destroy(pDevPort->port_topo_ptr, &cb_info);

  if(result != ADSP_EOK)
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Topo destroy failed for  port id 0x%x", pDevPort->intf);
  }

  return result;
}

/*
  This function is to set the parameters to the topology

  @param[in]     topo_ptr: pointer to topology structure
  @param[in]     module_id: module id
  @param[in]     param_id: param id
  @param[in]     param_size: size of the parameter
  @param[in]     calibration_data_pay_load_ptr: pointer to the parameter buffer
  
  @return
  ADSPResult

  @dependencies
  None.
*/
ADSPResult afe_topo_setparam(void *dev_port_ptr, uint32_t module_id, uint16_t instance_id, uint32_t param_id, uint16_t param_size,
                             int8_t *calibration_data_pay_load_ptr)
{
  ADSPResult result = ADSP_EOK;
  afe_dev_port_t *pDevPort = (afe_dev_port *)dev_port_ptr;

  if(NULL == dev_port_ptr)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Null port pointer");
    return ADSP_EFAILED;
  }

  topo_t **topo_ptr = (topo_t **)&pDevPort->port_topo_ptr;
  topo_callback_info_t cb_info;

  cb_info.cb_f = (topo_cb_f)afe_topo_create_cb;
  cb_info.cb_arg = dev_port_ptr;

  if((NULL == pDevPort->port_topo_ptr) &&(FALSE == pDevPort->topo_rcvd_flag)) // To handle the backward compatibility
  {
    if(AFE_PORT_STATE_RUN != pDevPort->port_state) //Topo creation is not supported when port is in run state
    {
      afe_topology_definition_t topo_def;
      memset(&topo_def, 0, sizeof(topo_def));

      /* For speaker protection V2, two modules should be present in VI path */
      if((AFE_MODULE_SPEAKER_PROTECTION_TH_VI == module_id) ||
         (AFE_MODULE_SPEAKER_PROTECTION_EX_VI == module_id))
      {
        topo_def.module_info[0].module_id = AFE_MODULE_SPEAKER_PROTECTION_TH_VI;
        topo_def.module_info[1].module_id = AFE_MODULE_SPEAKER_PROTECTION_EX_VI;
        topo_def.num_modules = 2;
      }
      else
      {
        topo_def.module_info[0].module_id = module_id;
        topo_def.num_modules = 1; // assumption that all algos currently AFE has, are mutually exclusive
      }

      if(ADSP_EOK != (result = topo_create((void **)topo_ptr, &topo_def, afe_topo_event_callback, NULL, &cb_info))) //TBD NULL
      {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to create the topo :%d", result);
        cb_info.cb_f = (topo_cb_f)afe_topo_destroy_cb;
        cb_info.cb_arg = dev_port_ptr;

        topo_destroy((void *)*topo_ptr, &cb_info);
        *topo_ptr = NULL;
        result = ADSP_EBADPARAM;
        return result;
      }
    }
    else
    {
      MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "topo set param for module id 0x%lx and param id 0x%lx is not supported "
          "when no topology created and port 0x%x is run state", module_id, param_id, pDevPort->intf);
      return ADSP_EUNSUPPORTED;
    }
  }

  if(NULL != pDevPort->port_topo_ptr)
  {

    if(ADSP_EOK != (result = topo_set_param((void *)*topo_ptr, module_id, instance_id, param_id, param_size,
                                            calibration_data_pay_load_ptr, NULL)))
    {
      MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "topo set param for module id 0x%lx and param id 0x%lx is failed with "
            "result %d", module_id, param_id, result);
    }
  }
  else
  { 
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Topology is not set for module id  0x%lx", module_id);
    return ADSP_EBADPARAM;
  }

  return result;
}

/*
  This function is the call back for creating the topology

  @param[in]     handle: handle to topology pointer
  @param[in/out]     dev_port_ptr: pointer to AFE dev port
  @param[in]     cb_type: call back type
  
  @return
  ADSPResult

  @dependencies
  None.
*/
ADSPResult afe_topo_create_cb(void *handle, void *dev_port_ptr, uint32_t cb_type)
{
  ADSPResult result = ADSP_EOK;
  topo_node_t *topo_node_ptr = NULL;
  capi_v2_t *module_ptr = NULL;
  capi_v2_buf_t cfg_buf;
  capi_v2_port_info_t port_info;

  topo_node_ptr = (topo_node_t *)handle;
  port_info.is_valid = FALSE;

  if(NULL == dev_port_ptr)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Null port pointer");
    return ADSP_EFAILED;
  }

  switch((topo_cb_type)cb_type)
  {
     case TOPO_CB_TYPE_PRE:
     {

     }

       break;
     case TOPO_CB_TYPE_POST:
     {
       if(NULL == topo_node_ptr)
       {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Null handle");
         return ADSP_EFAILED;
       }

       for(uint32_t num_extn = 0; num_extn < topo_node_ptr->num_extensions.num_extensions; num_extn++)
       {
         module_ptr = topo_node_ptr->module_ptr;

         switch(topo_node_ptr->fwk_extn_ptr[num_extn].id)
         {
            case FWK_EXTN_BGT_PROC:
            {
              capi_v2_err_t result;

              capi_v2_bgt_module_info_t bgt_module_info;
              memset(&bgt_module_info, 0, sizeof(capi_v2_bgt_module_info_t));

              cfg_buf.data_ptr = (int8_t *)&bgt_module_info;
              cfg_buf.max_data_len = sizeof(capi_v2_bgt_module_info_t);
              cfg_buf.actual_data_len = sizeof(capi_v2_bgt_module_info_t);
              module_ptr->vtbl_ptr->get_param(module_ptr, CAPI_V2_PARAM_ID_BGT_MODULE_INFO, &port_info,
                                              &cfg_buf);

              /* Create BGT */
              bgt_create_param_t bgt_create_param_t;

              snprintf(bgt_create_param_t.thread_name, sizeof(bgt_create_param_t.thread_name),
                       "AfeBgt_%2lx", topo_node_ptr->module_id);
              bgt_create_param_t.bgt_module_info_ptr = &bgt_module_info;
              bgt_create_param_t.svc_cmd_q_ptr = qurt_elite_globalstate.pAfeStatSvcCmdQ;
              bgt_create_param_t.svc_bgt_destroy_opcode = ELITEMSG_CUSTOM_BGT_DESTROY;

              result = bgt_create(&topo_node_ptr->bgt_info_ptr, &bgt_create_param_t);
              if(ADSP_FAILED(result))
              {
                MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Cannot create BGT for module id :0x%lx", topo_node_ptr->module_id);
                bgt_destroy(topo_node_ptr->bgt_info_ptr);
                return ADSP_EFAILED;
              }
              MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "afe_topo_create_cb: topo create post-CB for framework extension "
                  ":0x%lx done!", topo_node_ptr->fwk_extn_ptr[num_extn].id);
              break;
            }
         }
       }
     }
       break;
     default:
       MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "afe_topo_create_cb: Unsupported call back type :%ld", cb_type);
       return ADSP_EUNSUPPORTED;
  }

  return result;
}

/*
  This function is the call back for starting the topology

  @param[in]     handle: handle to topology pointer
  @param[in/out]     dev_port_ptr: pointer to AFE dev port
  @param[in]     cb_type: call back type
  
  @return
  ADSPResult

  @dependencies
  None.
*/
ADSPResult afe_topo_start_cb(void *handle, void *dev_port_ptr, uint32_t cb_type)
{
  ADSPResult result = ADSP_EOK;
  afe_dev_port_t *pDevPort = NULL;
  topo_node_t *topo_node_ptr = NULL;
  capi_v2_t *module_ptr = NULL;

  capi_v2_buf_t cfg_buf;
  capi_v2_port_info_t port_info;

  if((NULL == handle) || (NULL == dev_port_ptr))
  {
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Null handle 0x%lx or port pointer 0x%lx", handle, dev_port_ptr);
    return ADSP_EFAILED;
  }

  topo_node_ptr = (topo_node_t *)handle;

  pDevPort = (afe_dev_port_t *)dev_port_ptr;

  switch((topo_cb_type)cb_type)
  {
     case TOPO_CB_TYPE_PRE:
     {
       port_info.is_valid = FALSE;
       module_ptr = topo_node_ptr->module_ptr;

       for(uint32_t num_extn = 0; num_extn < topo_node_ptr->num_extensions.num_extensions; num_extn++)
       {
         switch(topo_node_ptr->fwk_extn_ptr[num_extn].id)
         {
            case FWK_EXTN_FEEDBACK:
            {
              // Enabling the feedback for the tx path modules
              if(AFE_PORT_TX_DIR == pDevPort->dir)
              {
                bool_t is_bgt_fb = FALSE;
                for(uint32_t i = 0; i < topo_node_ptr->num_extensions.num_extensions; i++)
                {
                  if(FWK_EXTN_BGT_PROC == topo_node_ptr->fwk_extn_ptr[i].id)
                  {
                    is_bgt_fb = TRUE;
                    break;
                  }
                }

                feedback_info_t feedback_path_info_v2;
                afe_feedback_path_t *feedback_path_ptr;

                // Enabling feedback path
                if(NULL != pDevPort->feedback_path_info_ptr)
                {
                  if(ADSP_EOK
                     != afe_port_enable_feedback_path(pDevPort->intf,
                                                      &pDevPort->feedback_path_info_ptr->feedback_cfg,
                                                      &topo_node_ptr->feedback_handle))
                  {
                    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Feedback path Enable is failed port_id: 0x%x", pDevPort->intf);
                    return ADSP_EFAILED;
                  }
                  MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Feedback enabled success for port_id: 0x%x, fb handle: 0x%lx",
                        pDevPort->intf, topo_node_ptr->feedback_handle);

                  feedback_path_info_v2.feedback_cfg.num_channels = pDevPort->feedback_path_info_ptr->feedback_cfg.num_channels;
                  memscpy(feedback_path_info_v2.feedback_cfg.chan_info,
                          sizeof(feedback_path_info_v2.feedback_cfg.chan_info),
                          pDevPort->feedback_path_info_ptr->feedback_cfg.chan_info,
                          sizeof(pDevPort->feedback_path_info_ptr->feedback_cfg.chan_info));

                  feedback_path_ptr = (afe_feedback_path_t *)topo_node_ptr->feedback_handle;

                  feedback_path_info_v2.bufQ_ptr =
                                                   (void *)(((afe_custom_client_info_t *)feedback_path_ptr->src_port_client_ptr)->bufQ);
                  feedback_path_info_v2.dataQ_ptr =
                                                    (void *)(((afe_custom_client_info_t *)feedback_path_ptr->src_port_client_ptr)->afe_custom_client.client_handle->dataQ);
                  feedback_path_info_v2.num_buffers_ptr =
                                                          &(((afe_custom_client_info_t *)feedback_path_ptr->src_port_client_ptr)->afe_custom_client.num_buffers);

                  /* It is BGT FB so Cache a copy of src and dest to disable FB later from BGT*/
                  if(is_bgt_fb)
                  {
                    /* We expect in such scenario BGT have been created */
                    if(topo_node_ptr->bgt_info_ptr)
                    {
                      topo_node_ptr->bgt_info_ptr->src_id = pDevPort->intf;
                      topo_node_ptr->bgt_info_ptr->dest_id = pDevPort->feedback_path_info_ptr->feedback_cfg.dst_portid;
                      topo_node_ptr->bgt_info_ptr->feedback_handle = topo_node_ptr->feedback_handle;
                      topo_node_ptr->feedback_handle = 0;
                    }
                    else
                    {
                      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed BGT not yet created for FB path port 0x%x", pDevPort->intf);
                      return ADSP_EFAILED;
                    }
                  }
                }
                else
                {
                  MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed Feedback config not yet rcvd for port 0x%x", pDevPort->intf);
                  return ADSP_EFAILED;
                }

                cfg_buf.data_ptr = (int8_t *)&feedback_path_info_v2;
                cfg_buf.max_data_len = sizeof(feedback_info_t);
                cfg_buf.actual_data_len = sizeof(feedback_info_t);
                module_ptr->vtbl_ptr->set_param(module_ptr, CAPI_V2_PARAM_ID_FB_PATH_INFO, &port_info,
                                                &cfg_buf);
              }
              else //sending client info to Rx module
              {
                cfg_buf.data_ptr = (int8_t *)&pDevPort->afe_cust_client_list_ptr;
                cfg_buf.max_data_len = sizeof(bufQList *);
                cfg_buf.actual_data_len = sizeof(bufQList *);
                module_ptr->vtbl_ptr->set_param(module_ptr, CAPI_V2_PARAM_ID_CLIENT_INFO, &port_info,
                                                &cfg_buf);
              }

              MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "afe_topo_start_cb: topo start pre-CB for framework extension :0x%lx done!", topo_node_ptr->fwk_extn_ptr[num_extn].id);
              break;
            }
         }
       }
     }
       break;

     case TOPO_CB_TYPE_POST:
     {
       for(uint32_t num_extn = 0; num_extn < topo_node_ptr->num_extensions.num_extensions; num_extn++)
       {
         port_info.is_valid = FALSE;
         module_ptr = topo_node_ptr->module_ptr;

         switch(topo_node_ptr->fwk_extn_ptr[num_extn].id)
         {
            case FWK_EXTN_BGT_PROC:
            {
              /* Pass handle of the BGT to the module */
              capi_v2_bgt_info_t capi_v2_bgt_info;

              capi_v2_bgt_info.bgt_info_ptr = topo_node_ptr->bgt_info_ptr;

              cfg_buf.data_ptr = (int8_t *)&capi_v2_bgt_info;
              cfg_buf.max_data_len = sizeof(capi_v2_bgt_info_t);
              cfg_buf.actual_data_len = sizeof(capi_v2_bgt_info_t);
              module_ptr->vtbl_ptr->set_param(module_ptr, CAPI_V2_PARAM_ID_BGT_INFO, &port_info,
                                              &cfg_buf);

              /* Call the BGT init function from here */
              result = (*(topo_node_ptr->bgt_info_ptr->bgt_module_info.init))(topo_node_ptr->bgt_info_ptr->bgt_module_info.fgt_data_ptr,
                                                                              &topo_node_ptr->bgt_info_ptr->bgt_data_ptr);
              if(CAPI_V2_FAILED(result))
              {
                MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to init the BGT for module :0x%lx", topo_node_ptr->module_id);
                return ADSP_EFAILED;
              }

              MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "afe_topo_start_cb: topo start post-CB for framework extension :0x%lx done!", topo_node_ptr->fwk_extn_ptr[num_extn].id);
              break;
            }

            case FWK_EXTN_CDC_INTERRUPT:
            {

              /* ADSP_EUNSUPPORTED error will come for internal codec and for stub implementation
                 ADSP_EOK will come when WCDXXXX attached to MSM */

              if(ADSP_EOK != afe_svc_check_codec_interrupts_supported())
              {
                MSG(MSG_SSID_QDSP6,DBG_HIGH_PRIO, "skipping interrupt registrations");
                break;
              }
              capi_v2_buf_t buf;
              topo_cdc_int_info_t *cdc_int_info_ptr;
              port_info.is_valid = false;
              module_ptr = topo_node_ptr->module_ptr;
              cdc_int_info_ptr = &topo_node_ptr->cdc_int_info;

              buf.data_ptr = (int8_t *)&cdc_int_info_ptr->num_cdc_int.num_cdc_int;
              buf.max_data_len = sizeof(capi_v2_num_needed_cdc_int_t);

              result = module_ptr->vtbl_ptr->get_param(module_ptr, CAPI_V2_PARAM_ID_NUM_NEEDED_CDC_INT, &port_info, &buf);

              MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Get param for module id 0x%lx with param id 0x%x result %d", topo_node_ptr->module_id, CAPI_V2_PARAM_ID_NUM_NEEDED_CDC_INT, result);
              if (ADSP_EOK != result)
              {
                return result;
              }

              if (0 != cdc_int_info_ptr->num_cdc_int.num_cdc_int)
              {
                if (NULL ==(cdc_int_info_ptr->cdc_int_ptr = (capi_v2_cdc_int_t *)qurt_elite_memory_malloc(sizeof(capi_v2_cdc_int_t)* cdc_int_info_ptr->num_cdc_int.num_cdc_int, topo_node_ptr->heap_id)))
                {
                  MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Out of memory for cdc interrupt structure for Module with ID 0x%lX", topo_node_ptr->module_id);
                  return ADSP_ENOMEMORY;
                }

                topo_cdc_client_t *cdc_client_ptr = NULL;

                if (NULL ==(cdc_client_ptr = (topo_cdc_client_t *)qurt_elite_memory_malloc(sizeof(topo_cdc_client_t)* cdc_int_info_ptr->num_cdc_int.num_cdc_int, topo_node_ptr->heap_id)))
                {
                  MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Out of memory for cdc int client for Module with ID 0x%lX", topo_node_ptr->module_id);
                  return ADSP_ENOMEMORY;
                }

                topo_node_ptr->cdc_client_ptr = cdc_client_ptr;

                buf.data_ptr = (int8_t *)cdc_int_info_ptr->cdc_int_ptr;
                buf.max_data_len = sizeof(capi_v2_cdc_int_t)* cdc_int_info_ptr->num_cdc_int.num_cdc_int;
                result = module_ptr->vtbl_ptr->get_param(module_ptr, CAPI_V2_PARAM_ID_CDC_INT_LIST, &port_info, &buf);
                MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Get param for module id 0x%lx with param id 0x%x result %d", topo_node_ptr->module_id, CAPI_V2_PARAM_ID_CDC_INT_LIST, result);

                if (ADSP_EOK != result)
                {
                  return result;
                }

                for (uint32_t int_idx = 0; int_idx < cdc_int_info_ptr->num_cdc_int.num_cdc_int; int_idx++)
                {
                  /* Initialize the client structs for interrupt handling */
                  topo_cdc_client_t *cdc_client_ptr = &topo_node_ptr->cdc_client_ptr[int_idx];

                  /* Clear the client struct to codec service for interrupt handling */
                  memset(cdc_client_ptr, 0, sizeof(topo_cdc_client_t));

                  /* Populate the client struct with default values */
                  cdc_client_ptr->is_pending_ack = FALSE;                                             /* ACK pending to codec service*/
                  cdc_client_ptr->client_info.client_id = pDevPort->intf;                              /* Port ID */
                  cdc_client_ptr->client_info.client_q_ptr = afe_svc_global_ptr->static_svc_cmdq_ptr; /* Response Q */
                  cdc_client_ptr->client_info.enable_int_ack_ctrl = TRUE;                             /* Receive ACK from codec service */

                  if(ADSP_EOK != afe_topo_cdc_get_int_index(cdc_int_info_ptr->cdc_int_ptr[int_idx].cdc_int, &cdc_client_ptr->client_info.int_index))
                  {
                    return result;
                  }

                  //Registering for the interrupt
                  result = afe_cdc_intr_reg_handler(&cdc_client_ptr->client_info, TRUE);
                  if (ADSP_EOK != result)
                  {
                    MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                          "Registering for CDC intr for module id 0x%lx with param id 0x%x result %d",
                          topo_node_ptr->module_id, CAPI_V2_PARAM_ID_CDC_INT_LIST, result);
                    return result;
                  }
                }
              }

              MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "afe_topo_start_cb: topo start post-CB for framework extension :0x%lx done!",
                    topo_node_ptr->fwk_extn_ptr[num_extn].id);
            }
              break;
         }
       }
     }
       break;

     default:
       MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "afe_topo_start_cb: Unsupported call back type :%ld", cb_type);
       return ADSP_EUNSUPPORTED;

  }

  return result;
}

/*
  This function is the call back for stoping the topology

  @param[in]     handle: handle to topology pointer
  @param[in/out]     dev_port_ptr: pointer to AFE dev port
  @param[in]     cb_type: call back type
  
  @return
  ADSPResult

  @dependencies
  None.
*/
ADSPResult afe_topo_stop_cb(void *handle, void *dev_port_ptr, uint32_t cb_type)
{
  ADSPResult result = ADSP_EOK;
  afe_dev_port_t *pDevPort = NULL;
  topo_node_t *topo_node_ptr = NULL;

  if((NULL == handle) || (NULL == dev_port_ptr))
  {
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Null handle 0x%lx or port pointer 0x%lx", handle, dev_port_ptr);
    return ADSP_EFAILED;
  }

  topo_node_ptr = (topo_node_t *)handle;

  pDevPort = (afe_dev_port_t *)dev_port_ptr;

  switch((topo_cb_type)cb_type)
  {
     case TOPO_CB_TYPE_PRE:
     {
       for(uint32_t num_extn = 0; num_extn < topo_node_ptr->num_extensions.num_extensions; num_extn++)
       {
         switch(topo_node_ptr->fwk_extn_ptr[num_extn].id)
         {
            case FWK_EXTN_FEEDBACK:
            {
              // Disabling the feedback for the tx path modules
              if(AFE_PORT_TX_DIR == pDevPort->dir)
              {
                // Disabling feedback path
                if(NULL != pDevPort->feedback_path_info_ptr)
                {
                  /* Disable only for non BGT FB modules */
                  if(topo_node_ptr->feedback_handle)
                  {
                    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE: feedback disable handle: 0x%lx",
                          topo_node_ptr->feedback_handle);
                    if(ADSP_EOK
                       != afe_port_disable_feedback_path(
                                                         pDevPort->intf, pDevPort->feedback_path_info_ptr->feedback_cfg.dst_portid,
                                                         topo_node_ptr->feedback_handle))
                    {
                      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                            "Disable feedback path is failed for port id: 0x%x, fb handle: 0x%lx",
                            pDevPort->intf, topo_node_ptr->feedback_handle);
                    }

                    topo_node_ptr->feedback_handle = 0;
                  }
                }
              }
              MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
                    "afe_topo_stop_cb: topo stop pre-CB for framework extension :0x%lx done!",
                    topo_node_ptr->fwk_extn_ptr[num_extn].id);
              break;
            }

            case FWK_EXTN_BGT_PROC:
            {
              bgt_send_cmd(topo_node_ptr->bgt_info_ptr, BGT_HANDLER_CMD_DEINIT);
              MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "afe_topo_stop_cb: topo stop post-CB for framework extension :0x%lx done!",
                    topo_node_ptr->fwk_extn_ptr[num_extn].id);
              break;
            }

            case FWK_EXTN_CDC_INTERRUPT:
            {
              /* ADSP_EUNSUPPORTED error will come for internal codec and for stub implementation
                 ADSP_EOK will come when WCDXXXX attached to MSM */
              if(ADSP_EOK != afe_svc_check_codec_interrupts_supported())
              {
                MSG(MSG_SSID_QDSP6,DBG_HIGH_PRIO, "skipping interrupt deregistrations");
                break;
              }

              topo_cdc_int_info_t *cdc_int_info_ptr;
              //module_ptr = topo_node_ptr->module_ptr;
              cdc_int_info_ptr = &topo_node_ptr->cdc_int_info;

              MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "cdc_client_ptr 0x%lx", (uint32_t)topo_node_ptr->cdc_client_ptr);

              for(uint32_t int_idx = 0; int_idx < cdc_int_info_ptr->num_cdc_int.num_cdc_int; int_idx++)
              {
                //Registering for the interrupt
                result = afe_cdc_intr_reg_handler(&(topo_node_ptr->cdc_client_ptr[int_idx].client_info), FALSE);

                if(ADSP_EOK != result)
                {
                  MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "deregistration module id 0x%lx with param id 0x%x result %d", topo_node_ptr->module_id, CAPI_V2_PARAM_ID_CDC_INT_LIST, result);
                }
              }

              if(NULL != topo_node_ptr->cdc_client_ptr)
              {
                qurt_elite_memory_free(topo_node_ptr->cdc_client_ptr);
                topo_node_ptr->cdc_client_ptr = NULL;
              }
              if(NULL != topo_node_ptr->cdc_int_info.cdc_int_ptr)
              {
                qurt_elite_memory_free(topo_node_ptr->cdc_int_info.cdc_int_ptr);
                topo_node_ptr->cdc_int_info.cdc_int_ptr = NULL;
              }

              MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "afe_topo_stop_cb: topo stop pre-CB for framework extension :0x%lx done!",
                    topo_node_ptr->fwk_extn_ptr[num_extn].id);
            }
         }
       }
     }

       break;
     case TOPO_CB_TYPE_POST:
     {

     }
       break;
     default:
       MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "afe_topo_stop_cb: Unsupported call back type :%ld", cb_type);
       return ADSP_EUNSUPPORTED;
  }

  return result;
}

/*
  This function is the call back for destroying the topology

  @param[in]     handle: handle to topology pointer
  @param[in/out]     dev_port_ptr: pointer to AFE dev port
  @param[in]     cb_type: call back type
  
  @return
  ADSPResult

  @dependencies
  None.
*/
ADSPResult afe_topo_destroy_cb(void *handle, void *dev_port_ptr, uint32_t cb_type)
{
  ADSPResult result = ADSP_EOK;
  topo_node_t *topo_node_ptr = NULL;

  if((NULL == handle) || (NULL == dev_port_ptr))
  {
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Null handle 0x%lx or port pointer 0x%lx", handle, dev_port_ptr);
    return ADSP_EFAILED;
  }

  topo_node_ptr = (topo_node_t *)handle;

  switch((topo_cb_type)cb_type)
  {
     case TOPO_CB_TYPE_PRE:
     {

     }

       break;
     case TOPO_CB_TYPE_POST:
     {
       for(uint32_t num_extn = 0; num_extn < topo_node_ptr->num_extensions.num_extensions; num_extn++)
       {
         switch(topo_node_ptr->fwk_extn_ptr[num_extn].id)
         {
            case FWK_EXTN_BGT_PROC:
            {
              bgt_send_cmd(topo_node_ptr->bgt_info_ptr, BGT_HANDLER_CMD_DESTROY);
              topo_node_ptr->bgt_info_ptr = NULL;

              MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "afe_topo_destroy_cb: topo destroy post-CB for framework extension :0x%lx done!",
                    topo_node_ptr->fwk_extn_ptr[num_extn].id);
              break;
            }
         }
       }

       break;
     }

     default:
       MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "afe_topo_destroy_cb: Unsupported call back type :%ld", cb_type);
       return ADSP_EUNSUPPORTED;
  }

  return result;
}


/*
  This function is to acknowledge the codec interrupt

  @param[in]     client_info_ptr: pointer to codec information structure
  
  @return
  ADSPResult

  @dependencies
  None.
*/
ADSPResult afe_topo_ack_cdc_intr(void *client_info_ptr)
{
  ADSPResult result = ADSP_EOK;
  uint32_t payload_size;
  elite_msg_any_t msg;
  elite_msg_custom_afe_cdc_int_info_t *payload_ptr = NULL;
  elite_svc_handle_t *cdc_svc_hptr = &(afe_svc_global_ptr->cdc_service_handle);

  if(NULL == client_info_ptr)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "NULL client info pointer");
	return ADSP_EFAILED;
  }

  payload_size = sizeof(elite_msg_custom_afe_cdc_int_info_t);

  if(ADSP_EOK != (result = elite_msg_create_msg(&msg, &payload_size, ELITE_CUSTOM_MSG, NULL, 0, ADSP_EOK)))
  {
    MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "FBSP: FAILED to create EliteMsg for intr handler ACK");
    return result;
  }

  payload_ptr = (elite_msg_custom_afe_cdc_int_info_t *)msg.pPayload;

  memscpy(&payload_ptr->client_info, sizeof(cdc_client_info_t), client_info_ptr, sizeof(cdc_client_info_t));

  payload_ptr->sec_op_code = ELITEMSG_CUSTOM_CDC_INT_CLIENT_ACK;

  if(ADSP_FAILED(result = qurt_elite_queue_push_back(cdc_svc_hptr->cmdQ, (uint64_t *)&msg)))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "FBSP: Unable to send interrupt ack msg, result: 0x%x", result);
    elite_msg_return_payload_buffer(&msg);
  }

  return result;
}

/*
  This function is to get the interrupt index used in codec handler

  @param[in]     int_id: interrupt GUID
  @param[out]    cdc_int_id: interrupt index used in codec handler
  
  @return
  ADSPResult

  @dependencies
  None.
*/
ADSPResult afe_topo_cdc_get_int_index(uint32_t int_id, uint16_t *cdc_int_id)
{ 
  ADSPResult result = ADSP_EOK;

  switch(int_id)
  {
     case CAPI_V2_PARAM_ID_CDC_INT_MAD_AUDIO:
       *cdc_int_id = AFE_CDC_INT_MAD_AUDIO;
       break;

     case CAPI_V2_PARAM_ID_CDC_INT_MAD_BEACON:
       *cdc_int_id = AFE_CDC_INT_MAD_BEACON;
       break;

     case CAPI_V2_PARAM_ID_CDC_INT_MAD_ULT:
       *cdc_int_id = AFE_CDC_INT_MAD_ULT;
       break;

     case CAPI_V2_PARAM_ID_CDC_INT_SPKR_AUDIO_CLIP:
       *cdc_int_id = AFE_CDC_INT_SPKR_AUDIO_CLIP;
       break;

     case CAPI_V2_PARAM_ID_CDC_INT_VBAT_ATTACK:
       *cdc_int_id = AFE_CDC_INT_VBAT_ATTACK;
       break;

     case CAPI_V2_PARAM_ID_CDC_INT_VBAT_RELEASE:
       *cdc_int_id = AFE_CDC_INT_VBAT_RELEASE;
       break;

     case CAPI_V2_PARAM_ID_CDC_INT_SPKR2_AUDIO_CLIP:
       *cdc_int_id = AFE_CDC_INT_SPKR2_AUDIO_CLIP;
       break;

     default:
       MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Unsupported interrupt type 0x%lx", int_id);
       return ADSP_EUNSUPPORTED;
  }

  return result;
}

/*
  This function is to get the interrupt GUID

  @param[in]    cdc_int_id: interrupt index used in codec handler
  @param[out]     int_id: interrupt GUID
  
  @return
  ADSPResult

  @dependencies
  None.
*/
ADSPResult afe_topo_cdc_get_int_id(uint16_t cdc_int_id, uint32_t *int_id)
{
  ADSPResult result = ADSP_EOK;

  switch(cdc_int_id)
  {
     case AFE_CDC_INT_MAD_AUDIO:
       *int_id = CAPI_V2_PARAM_ID_CDC_INT_MAD_AUDIO;
       break;

     case AFE_CDC_INT_MAD_BEACON:
       *int_id = CAPI_V2_PARAM_ID_CDC_INT_MAD_BEACON;
       break;

     case AFE_CDC_INT_MAD_ULT:
       *int_id = CAPI_V2_PARAM_ID_CDC_INT_MAD_ULT;
       break;

     case AFE_CDC_INT_SPKR_AUDIO_CLIP:
       *int_id = CAPI_V2_PARAM_ID_CDC_INT_SPKR_AUDIO_CLIP;
       break;

     case AFE_CDC_INT_VBAT_ATTACK:
       *int_id = CAPI_V2_PARAM_ID_CDC_INT_VBAT_ATTACK;
       break;

     case AFE_CDC_INT_VBAT_RELEASE:
       *int_id = CAPI_V2_PARAM_ID_CDC_INT_VBAT_RELEASE;
       break;

     case AFE_CDC_INT_SPKR2_AUDIO_CLIP:
       *int_id = CAPI_V2_PARAM_ID_CDC_INT_SPKR2_AUDIO_CLIP;
       break;

     default:
       MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Unsupported interrupt type %d", cdc_int_id);
       return ADSP_EUNSUPPORTED;
  }

  return result;
}


