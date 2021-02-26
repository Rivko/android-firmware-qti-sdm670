/**========================================================================
 @file vpm_svc_vp3_utils.cpp
 
 @brief This file contains utilites for VP3 handling
 
 Copyright (c) 2016 Qualcomm Technologies, Inc.
 All rights reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 ====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/vsd/common/cvd/cvp/src/vpm_svc_vp3_utils.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------

========================================================================== */


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "qurt_elite.h"
#include "Elite.h"

#include "vpm_svc_vp3_utils.h"
#include "vpm_session_dyn_svc_cfg_utils.h"
#include "vpm_response_handler.h"
#include "vpm_session_utils.h"
#include "vpm_session_cal_utils.h"

/* -----------------------------------------------------------------------
** Global definitions
** ----------------------------------------------------------------------- */

static vpm_vp3_info_t   *p_global_vp3_info;


ADSPResult vpm_svc_vp3_init(vpm_vp3_info_t  *p_vp3_info)
{
   ADSPResult           result = ADSP_EOK;
   void                 *mem_ptr = NULL;
   uint32_t             mem_size;
   uint32_t             dev_cntr;

   /* Initialize the global VP3 management. */

   /* Calculate the mem size required for all the device pairs */
   mem_size = sizeof(cvp_per_device_pair_vp3_t) * VPM_MAX_NUM_DEVICE_PAIRS_OF_VP3_STORED;

   /* Allocate global VP3 heap. */
   if (NULL == (mem_ptr = qurt_elite_memory_malloc(mem_size, QURT_ELITE_HEAP_DEFAULT)))
   {
      MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
          "vpm_svc_vp3_init(): Failed to allocate cvp_global_vp3_heap");

      return ADSP_ENOMEMORY;
   }

   p_vp3_info->p_sound_dev_pair_info = (cvp_per_device_pair_vp3_t *)mem_ptr;

   /* Update the VP3 address info in static variable */
   p_global_vp3_info = p_vp3_info;

   /* Memset(0) the entire memory */
   memset(mem_ptr, 0, mem_size);

   /* Populate the free Q for VP3 tracking structures. */
   (void)apr_list_init_v2(&p_vp3_info->list_free_q, NULL, NULL);

   /* Init the item pool with allocated memory */
   for (dev_cntr = 0; dev_cntr < VPM_MAX_NUM_DEVICE_PAIRS_OF_VP3_STORED; dev_cntr++)
   {
      p_vp3_info->list_item_pool[dev_cntr].vp3 = (&p_vp3_info->p_sound_dev_pair_info[dev_cntr]);

      /* Init the VP3 node */
      (void)apr_list_init_node((apr_list_node_t *)&p_vp3_info->list_item_pool[dev_cntr]);

      (void)apr_list_add_tail(&p_vp3_info->list_free_q,
                              ((apr_list_node_t *)&p_vp3_info->list_item_pool[dev_cntr]));
   }

   (void)apr_list_init_v2(&p_vp3_info->list_used_q, NULL, NULL);

   return result;
}

ADSPResult vpm_svc_vp3_deinit(vpm_vp3_info_t *p_vp3_info)
{
   if (!p_vp3_info)
   {
      MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
          "vpm_svc_vp3_deinit(): VP3 info pointer is NULL");

      return ADSP_EFAILED;
   }

   /* Release the VP3 management. */
   (void)apr_list_destroy(&p_vp3_info->list_free_q);
   (void)apr_list_destroy(&p_vp3_info->list_used_q);

   if (p_vp3_info->p_sound_dev_pair_info)
   {
      qurt_elite_memory_free(p_vp3_info->p_sound_dev_pair_info);
      p_vp3_info->p_sound_dev_pair_info = NULL;
   }

   return ADSP_EOK;
}

static ADSPResult vpm_find_vp3_item_in_global_db(vpm_session_object_t *session_obj,
                                                 vpm_vp3_list_item_t **ret_vp3_list_item)
{
   ADSPResult                    result = ADSP_EFAILED;
   bool_t                        is_item_found = FALSE;
   vpm_vp3_list_item_t           *vp3_list_item;
   int32_t                       rc;

   if ((!session_obj) || (!ret_vp3_list_item))
   {
      MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
            "vpm_find_vp3_item_in_global_db(): session_obj(0x%lx) and/or vp3 item(0x%lx) return ptr is NULL",
            session_obj,  ret_vp3_list_item);

      return ADSP_EBADPARAM;
   }

   vp3_list_item = ((vpm_vp3_list_item_t *)&p_global_vp3_info->list_used_q.dummy);

   for (;;)
   {
      if (APR_EOK != (rc = apr_list_get_next(&p_global_vp3_info->list_used_q,
                                             ((apr_list_node_t *)vp3_list_item),
                                             ((apr_list_node_t **)&vp3_list_item))))
      {
         return ADSP_EFAILED;
      }

      if (session_obj->sound_device_info.device_pair.direction ==
             vp3_list_item->vp3->sound_device_pair.direction)
      {
         switch (session_obj->sound_device_info.device_pair.direction)
         {
            case VPM_SOUND_DEVICE_DIRECTION_RX:
            {
               if (session_obj->sound_device_info.device_pair.rx_device_id ==
                      vp3_list_item->vp3->sound_device_pair.rx_device_id)
               {
                  *ret_vp3_list_item = vp3_list_item;
                  is_item_found = TRUE;
               }
               break;
            }
            case VPM_SOUND_DEVICE_DIRECTION_TX:
            {
               if (session_obj->sound_device_info.device_pair.tx_device_id ==
                      vp3_list_item->vp3->sound_device_pair.tx_device_id)
               {
                  *ret_vp3_list_item = vp3_list_item;
                  is_item_found = TRUE;
               }
               break;
            }
            case VPM_SOUND_DEVICE_DIRECTION_RX_TX:
            {
               if ((session_obj->sound_device_info.device_pair.rx_device_id ==
                       vp3_list_item->vp3->sound_device_pair.rx_device_id) &&
                   (session_obj->sound_device_info.device_pair.tx_device_id ==
                       vp3_list_item->vp3->sound_device_pair.tx_device_id))
               {
                  *ret_vp3_list_item = vp3_list_item;
                  is_item_found = TRUE;
               }
               break;
            }
            default:
            {
               MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                     "vpm_find_vp3_item_in_global_db(): Invalid device pair direction: 0x%lx",
                     session_obj->sound_device_info.device_pair.direction);

               result = ADSP_EFAILED;
               break;
            }
         }
      }

      if (TRUE == is_item_found)
      {
         MSG_1(MSG_SSID_DFLT, DBG_HIGH_PRIO,
               "vpm_find_vp3_item_in_global_db(): VP3 Node found: 0x%lx", vp3_list_item);
         result = ADSP_EOK;

         break;
      }
   }

   return result;
}

ADSPResult vpm_update_global_vp3_db(vpm_session_object_t *session_obj)
{
   ADSPResult           result = ADSP_EOK;
   int32_t              rc = APR_EOK;
   vpm_vp3_list_item_t  *vp3_list_item;

   if (NULL == session_obj)
   {
      MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
          "vpm_update_global_vp3_db(): session_obj ptr is NULL");

      return ADSP_EBADPARAM;
   }

   for (;;)
   {

      /* If the VP3 data from the global VP3 heap is already stored for the
       * current sound device pair. Update the VP3 data and update the global VP3
       * tracking list to reflect that the current sound device pair is the most
       * recently (i.e. move it to the head of the list) used sound device pair.
       */
      if (ADSP_EOK == (result = vpm_find_vp3_item_in_global_db(session_obj,
                                                               &vp3_list_item)))
      {
         memscpy(vp3_list_item->vp3->data,
                 VPM_MAX_VP3_DATA_LEN,
                 session_obj->shared_heap.vp3_cache.data,
                 session_obj->shared_heap.vp3_cache.data_len);

         vp3_list_item->vp3->data_len = session_obj->shared_heap.vp3_cache.data_len;

         /* Move the item to the head (most recently used device pair) of the
          * VP3 tracking list. */
         (void)apr_list_delete(&p_global_vp3_info->list_used_q, &vp3_list_item->link);
         (void)apr_list_add_head(&p_global_vp3_info->list_used_q, &vp3_list_item->link);

         break;
      }

      /* If VP3 data for the current device pair is not found in the global heap,
       * add the VP3 data for the current device into the global heap and update
       * the global VP3 tracking list to reflect that the current sound device
       * pair is the most recently used sound device pair.
       */

      rc = apr_list_remove_head(&p_global_vp3_info->list_free_q,
                                ((apr_list_node_t **)&vp3_list_item));
      if (rc)
      {
         /* No free VP3 tracking list item is available. Remove the oldest
          * device pair tracking item from the global VP3 heap which will be
          * used to track the new device pair.
          */
         MSG(MSG_SSID_DFLT, DBG_MED_PRIO,
             "vpm_update_global_vp3_db(): Free VP3 list item not available, remove the oldest"
             "from database");

         rc = apr_list_remove_tail(&p_global_vp3_info->list_used_q,
                                   ((apr_list_node_t **)&vp3_list_item));

         if (rc)
         {
            MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                "vpm_update_global_vp3_db(): Failed to remove oldest device pair from VP3 db");
            break;
         }
      }

      memscpy(vp3_list_item->vp3->data,
              VPM_MAX_VP3_DATA_LEN,
              session_obj->shared_heap.vp3_cache.data,
              session_obj->shared_heap.vp3_cache.data_len);

      vp3_list_item->vp3->data_len = session_obj->shared_heap.vp3_cache.data_len;

      vp3_list_item->vp3->sound_device_pair.direction =\
         session_obj->sound_device_info.device_pair.direction;

      vp3_list_item->vp3->sound_device_pair.rx_device_id =\
         session_obj->sound_device_info.device_pair.rx_device_id;

      vp3_list_item->vp3->sound_device_pair.tx_device_id =\
         session_obj->sound_device_info.device_pair.tx_device_id;

      (void)apr_list_add_head(&p_global_vp3_info->list_used_q, &vp3_list_item->link);

      MSG_3(MSG_SSID_DFLT, DBG_MED_PRIO,
            "vpm_update_global_vp3_db(): Updated VP3 data, direction: %lu,"
            "rx_dev_id: 0x%lx, tx_dev_id: 0x%lx",
            session_obj->sound_device_info.device_pair.direction,
            session_obj->sound_device_info.device_pair.rx_device_id,
            session_obj->sound_device_info.device_pair.tx_device_id);

      break;
   } /* End of for (;;) */

   return rc;
}

ADSPResult vpm_action_set_vp3(vpm_session_object_t *session_obj)
{
   ADSPResult              result = ADSP_EOK;
   int32_t                 rc;
   bool_t                  is_vp3_to_be_set = FALSE;
   vpm_vp3_list_item_t     *vp3_list_item;

   cvd_cal_log_vp3_data_header_t log_info_vp3;
   cvd_cal_log_commit_info_t log_info;

   if (!session_obj)
   {
      MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
          "vpm_action_set_vp3(): session_obj ptr is NULL");

      return ADSP_EBADPARAM;
   }

   if (FALSE == session_obj->is_client_set_vp3_data)
   {
      if (TRUE == session_obj->sound_device_info.is_available)
      {
         if (ADSP_EOK == (result = vpm_find_vp3_item_in_global_db(session_obj, &vp3_list_item)))
         {
            if (vp3_list_item->vp3->data_len <= VPM_MAX_VP3_DATA_LEN)
            {
               memscpy(session_obj->shared_heap.vp3_cache.data,
                       VPM_MAX_VP3_DATA_LEN,
                       vp3_list_item->vp3->data,
                       vp3_list_item->vp3->data_len);

               session_obj->shared_heap.vp3_cache.data_len = vp3_list_item->vp3->data_len;

               is_vp3_to_be_set = TRUE;

               MSG_2(MSG_SSID_DFLT, DBG_HIGH_PRIO,
                     "vpm_action_set_vp3(): Found VP3 Device Pair Cal. Rx: 0x%08X, Tx: 0x%08X",
                     vp3_list_item->vp3->sound_device_pair.rx_device_id,
                     vp3_list_item->vp3->sound_device_pair.tx_device_id);
            }
         }
      }
   }

   /* Check if we have vp3 data in the cache. */
   if ((FALSE == is_vp3_to_be_set) &&
       (FALSE == session_obj->is_client_set_vp3_data))
   {
      MSG(MSG_SSID_DFLT, DBG_HIGH_PRIO,
          "vpm_action_set_vp3(): No vp3 data in cache ! Not setting vp3");

      /* Need to return error code to indicate no set params have been issued */
      return ADSP_EFAILED;
   }

   session_obj->is_client_set_vp3_data = FALSE;

   /* Log VP3 Data */
   log_info_vp3.direction = session_obj->sound_device_info.device_pair.direction;
   log_info_vp3.rx_device_id = session_obj->sound_device_info.device_pair.rx_device_id;
   log_info_vp3.tx_device_id = session_obj->sound_device_info.device_pair.tx_device_id;

   log_info.instance = ((session_obj->attached_mvm_handle << 16) |
                           (session_obj->self_apr_port));
   log_info.call_num = session_obj->target_set.system_config.call_num;
   log_info.data_container_id = CVD_CAL_LOG_DATA_CONTAINER_RAW_VP3;
   log_info.data_container_header_size = sizeof(log_info_vp3);
   log_info.data_container_header = &log_info_vp3;
   log_info.payload_size = session_obj->shared_heap.vp3_cache.data_len;
   log_info.payload_buf = session_obj->shared_heap.vp3_cache.data;

   cvd_cal_log_data(LOG_ADSP_CVD_VP3_C, CVD_CAL_LOG_RAW_VP3_DATA_INPUT_TAP_POINT,
                          (void *)&log_info, sizeof(log_info));

   /* Set the cmd rsp pending status */
   vpm_set_cmd_rsp_pending_status(session_obj, TRUE);

   /* Set the function pointer for response handler */
   session_obj->session_rsp_ctrl.p_rsp_hndlr_fn = vpm_set_vp3_param_transition_rsp_fn;

   if (ADSP_EOK != (result = vpm_set_get_param_to_dyn_svc(session_obj,
                                                             session_obj->shared_heap.vp3_cache.data,
                                                             session_obj->shared_heap.vp3_cache.data_len,
                                                             ELITE_CMD_SET_PARAM,
                                                             CVD_CAL_PARAM_MINOR_VERSION_0)))
   {
      MSG_1(MSG_SSID_DFLT, DBG_HIGH_PRIO,
            "vpm_action_set_vp3(): Failed to send set param msg to dyn svcs, result: 0x%lx",
            result);

      /* Clear the cmd rsp pending status */
      vpm_set_cmd_rsp_pending_status(session_obj, FALSE);

      return result;
   }

   MSG_2(MSG_SSID_DFLT, DBG_MED_PRIO,
         "vpm_action_set_vp3(): Sent set param to dyn svcs, addr: 0x%lx, payload_size: %d",
         session_obj->shared_heap.vp3_cache.data,
         session_obj->shared_heap.vp3_cache.data_len);

   /* return and wait for set param responses */
   return result;
}

ADSPResult vpm_action_get_vp3(vpm_session_object_t *session_obj)
{
   ADSPResult           result = ADSP_EOK;
   voice_param_data_t   *p_voice_param_data;

   if (NULL == session_obj)
   {
      MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
          "vpm_action_get_vp3(): Session obj pointer is NULL");

      return ADSP_EFAILED;
   }

   memset(&session_obj->shared_heap.vp3_cache, 0,
          sizeof(session_obj->shared_heap.vp3_cache));

   /* Set the command pending status */
   vpm_set_cmd_rsp_pending_status(session_obj, TRUE);

   /* Init the response handle for this get param */
   session_obj->session_rsp_ctrl.p_rsp_hndlr_fn = vpm_get_vp3_param_transition_rsp_fn;

   p_voice_param_data = (voice_param_data_t *)session_obj->shared_heap.vp3_cache.data;

   /* Populate the required payload for get param */
   p_voice_param_data->module_id = VPM_VP3_MOD_ID;
   p_voice_param_data->param_id = VPM_VP3_PARAM_ID;
   p_voice_param_data->param_size = VPM_MAX_VP3_DATA_LEN;

   if (ADSP_EOK != (result = vpm_set_get_param_to_dyn_svc(session_obj,
                                                             session_obj->shared_heap.vp3_cache.data,
                                                             VPM_MAX_VP3_DATA_LEN,
                                                             ELITE_CMD_GET_PARAM,
                                                             CVD_CAL_PARAM_MINOR_VERSION_0)))
   {
      MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
          "vpm_action_get_vp3(): Failed to send get param cmd to dyn svcs");

      /* Clear the command pending status */
      vpm_set_cmd_rsp_pending_status(session_obj, FALSE);

      return ADSP_EFAILED;
   }

   MSG_2(MSG_SSID_DFLT, DBG_MED_PRIO,
         "vpm_action_get_vp3(): Sent get param to dyn svcs, addr: 0x%lx, payload_size: %lu",
         session_obj->shared_heap.vp3_cache.data,
         VPM_MAX_VP3_DATA_LEN);

   return result;
}

ADSPResult vpm_vp3_get_data_cmd_ctrl(vpm_session_object_t *session_obj)
{
   ADSPResult           result = ADSP_EOK;
   elite_apr_packet_t   *p_apr_pkt;
   int32_t              rc;
   uint32_t             payload_size;
   void                 *payload_address;
   cvd_virt_addr_t      param_data_virt_addr;
   voice_param_data_t   *p_voice_param_data;

   vss_ivp3_cmd_get_data_t *payload;

   p_apr_pkt = (elite_apr_packet_t *)session_obj->session_cmd_ctrl.msg_pkt.pPayload;

   for (;;)
   {
      if (FALSE == session_obj->session_cmd_ctrl.is_prev_cmd_pending)
      {
         payload = (vss_ivp3_cmd_get_data_t *)elite_apr_if_get_payload_ptr(p_apr_pkt);
         payload_size = elite_apr_if_get_payload_size(p_apr_pkt);

         /* Validate in-bound payload size */
         if (payload_size < sizeof(vss_ivp3_cmd_get_data_t))
         {
            MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_vp3_get_data_cmd_ctrl(): Unexpected payload size, %lu < expected: %lu",
                  payload_size, sizeof(vss_ivp3_cmd_get_data_t));

            result = ADSP_EBADPARAM;
            break;
         }

         /* Validate the payload size */
         if (payload->mem_size & 0x3)
         {
            MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_vp3_get_data_cmd_ctrl(): Invalid param data size, %lu",
                  payload->mem_size);

            result = ADSP_EBADPARAM;
            break;
         }

         if (ADSP_EOK != (result = vpm_validate_calib_payload(payload->mem_handle,
                                                              payload->mem_size,
                                                              payload->mem_address)))
         {
            MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_vp3_get_data_cmd_ctrl(): Calib payload validation failed, result: 0x%08X",
                  result);

            /* End the APR command */
            break;
         }

         rc = cvd_mem_mapper_get_virtual_addr_v2(payload->mem_handle,
                                                 payload->mem_address,
                                                 &param_data_virt_addr);
         if (rc)
         {
            MSG_3(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_vp3_get_data_cmd_ctrl(): Cannot get virtual address,"
                  " mem_handle: 0x%08X, addr: lsw: 0x%08X, msw: 0x%08X", payload->mem_handle,
                  (uint32_t)payload->mem_address, (uint32_t)(payload->mem_address >> 32));

            result = ADSP_EFAILED;
            break;
         }

         /* Check if the VP3 data is already available */
         if (session_obj->shared_heap.vp3_cache.data_len != 0)
         {
            if (payload->mem_size < session_obj->shared_heap.vp3_cache.data_len)
            {
               MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                     "vpm_vp3_get_data_cmd_ctrl(): size of VP3 buffer provided by"
                     " client %lu bytes is less than VP3 cache buffer size %lu bytes",
                     payload->mem_size, session_obj->shared_heap.vp3_cache.data_len);

               result = ADSP_EFAILED;
            }
            else
            {
               /* Copy the data from VP3 cache to shared mem location for get param */
               memscpy(param_data_virt_addr.ptr,
                       payload->mem_size,
                       session_obj->shared_heap.vp3_cache.data,
                       session_obj->shared_heap.vp3_cache.data_len);

               rc = cvd_mem_mapper_cache_flush_v2(&param_data_virt_addr, payload->mem_size);
            }

            /* End the APR command */
            break;
         }
         else
         {
            /* Need to get VP3 data from dynamic services */

            /* Set the command status to pending */
            vpm_set_session_cmd_pending_status(session_obj->session_id, TRUE);

            /* Set the command response pending status to TRUE  */
            vpm_set_cmd_rsp_pending_status(session_obj, TRUE);

            payload_address = param_data_virt_addr.ptr;
            payload_size = payload->mem_size;

            p_voice_param_data = (voice_param_data_t *)payload_address;

            /* Populate the get param header */
            p_voice_param_data->module_id = VPM_VP3_MOD_ID;
            p_voice_param_data->param_id = VPM_VP3_PARAM_ID;
            p_voice_param_data->param_size = payload->mem_size;
            p_voice_param_data->reserved = 0;

            /* Set the response handler pointer */
            session_obj->session_rsp_ctrl.p_rsp_hndlr_fn = vpm_vp3_get_data_cmd_rsp_fn;

            /* Send the get param msg to vpRx/Tx dyn svcs */
            if (ADSP_EOK != (result = vpm_set_get_param_to_dyn_svc(session_obj,
                                                                      payload_address,
                                                                      payload_size,
                                                                      ELITE_CMD_GET_PARAM,
                                                                      CVD_CAL_PARAM_MINOR_VERSION_0)))
            {
               MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                     "vpm_vp3_get_data_cmd_ctrl(): Failed to send get param cmd to dyn svcs, result: 0x%lx",
                     result);

               /* Clear  the command pending status */
               vpm_set_cmd_rsp_pending_status(session_obj, FALSE);

               /* Set the command status to pending */
               vpm_set_session_cmd_pending_status(session_obj->session_id, FALSE);

               /* End the APR command processing */
               break;
            }

            /* Return and wait for get param respones from dyn services */
            return ADSP_EOK;
         }
      } /* if (FALSE == session_obj->session_cmd_ctrl.is_prev_cmd_pending) */

      /* Control will reach here after getting responses for all the
         get param commands */

      /* Clear the command pending status */
      vpm_set_session_cmd_pending_status(session_obj->session_id, FALSE);

      /* Free the current command APR packet */
      elite_apr_if_free(session_obj->self_apr_handle, p_apr_pkt);

      return result;

   } /* for (;;) */

   /* End the APR command if any error occurs or no get param commands are sent */
   elite_apr_if_end_cmd(session_obj->self_apr_handle, p_apr_pkt, result);

   return result;
}

ADSPResult vpm_vp3_set_data_cmd_ctrl(vpm_session_object_t *session_obj)
{
   ADSPResult                 result = ADSP_EOK;
   int32_t                    rc;
   vss_ivp3_cmd_set_data_t    *payload;
   cvd_virt_addr_t            param_data_virt_addr;
   voice_param_data_t         *vp3_data;
   elite_apr_packet_t         *p_apr_pkt;
   uint32_t                   payload_len;

   p_apr_pkt = (elite_apr_packet_t *)session_obj->session_cmd_ctrl.msg_pkt.pPayload;

   for (;;)
   {
      if (VPM_STATE_ENUM_RUN == session_obj->state_ctrl.state)
      {
         MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
             "vpm_vp3_set_data_cmd_ctrl(): FAILED, VPM already in RUN state");

         result = ADSP_EFAILED;
         break;
      }

      payload = (vss_ivp3_cmd_set_data_t *)elite_apr_if_get_payload_ptr(p_apr_pkt);
      payload_len = elite_apr_if_get_payload_size(p_apr_pkt);

      /* Validate in-bound payload size */
      if (payload_len != sizeof(vss_ivp3_cmd_set_data_t))
      {
         MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_vp3_set_data_cmd_ctrl(): Unexpected payload size, %lu != expected: %lu",
               payload_len, sizeof(vss_ivp3_cmd_set_data_t));

         result = ADSP_EBADPARAM;
         break;
      }

      if (payload->mem_size & 0x3)
      {
         MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_vp3_set_data_cmd_ctrl(): Invalid param data size, %d",
               payload->mem_size);

         result = ADSP_EBADPARAM;
         break;
      }

      if (payload->mem_size > VPM_MAX_VP3_DATA_LEN)
      {
         MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_vp3_set_data_cmd_ctrl(): Client provided VP3 size 0x%X is"
               " greater than maximum supported of 1024 bytes",
               payload->mem_size);

         /* End the APR command */
         result = ADSP_EUNSUPPORTED;
         break;
      }

      if (ADSP_EOK != (result = vpm_validate_calib_payload(payload->mem_handle,
                                                           payload->mem_size,
                                                           payload->mem_address)))
      {
         MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_vp3_set_data_cmd_ctrl(): Failed to validate calib payload: result: 0x%lx",
               result);

         /* End the APR command */
         break;
      }

      rc = cvd_mem_mapper_get_virtual_addr_v2(payload->mem_handle, payload->mem_address,
                                              &param_data_virt_addr);
      if (rc)
      {
         MSG_3(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_vp3_set_data_cmd_ctrl(): Cannot get virtual address,"
               " mem_handle: 0x%08X, addr: lsw: 0x%08X, msw: 0x%08X", payload->mem_handle,
               (uint32_t)payload->mem_address, (uint32_t)(payload->mem_address >> 32));

         result = APR_EFAILED;
         break;
      }

      /* Invalidate the cache before reading the VP3 data from shared mem */
      cvd_mem_mapper_cache_invalidate_v2(&param_data_virt_addr, payload->mem_size);

      /* Copy the data in session object VP3 cache */
      memscpy(session_obj->shared_heap.vp3_cache.data,
              VPM_MAX_VP3_DATA_LEN,
              param_data_virt_addr.ptr,
              payload->mem_size);

      vp3_data = ((voice_param_data_t *)session_obj->shared_heap.vp3_cache.data);

      session_obj->shared_heap.vp3_cache.data_len = (sizeof(voice_param_data_t) +
                                                     vp3_data->param_size); /* header + data */

      session_obj->is_client_set_vp3_data = TRUE;

      result = ADSP_EOK;
      break;
   } /* End of for (;;) */

   /* End the APR command */
   elite_apr_if_end_cmd(session_obj->self_apr_addr, p_apr_pkt, result);

   return result;
}

ADSPResult vpm_vp3_get_size_cmd_ctrl(vpm_session_object_t *session_obj)
{
   ADSPResult                 result = ADSP_EOK;
   elite_apr_packet_t         *p_apr_pkt;
   void                       *payload_address = NULL;
   uint32_t                   payload_size;
   voice_param_data_t         *p_voice_param;
   vss_ivp3_rsp_get_size_t    *p_vp3_get_size_rsp;

   p_apr_pkt = (elite_apr_packet_t *)session_obj->session_cmd_ctrl.msg_pkt.pPayload;

   if (FALSE == session_obj->session_cmd_ctrl.is_prev_cmd_pending)
   {
      /* Calculate in-band get param rsp payload size.
         Payload size = Param data header + VP3 get size response payload */
      payload_size = (sizeof(voice_param_data_t) +\
                         sizeof(vss_ivp3_rsp_get_size_t));

      /* Allocate payload for in-band get param response */
      if (NULL == (payload_address = qurt_elite_memory_malloc(payload_size,
                                                              QURT_ELITE_HEAP_DEFAULT)))
      {
         MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
             "vpm_vp3_get_size_cmd_ctrl(): Failed to allocate in-band rsp payload");

         result = ADSP_ENOMEMORY;
         elite_apr_if_end_cmd(session_obj->self_apr_handle, p_apr_pkt, ADSP_ENOMEMORY);

         return result;
      }

      p_voice_param = (voice_param_data_t *)payload_address;

      /* Populate the payload */
      p_voice_param->module_id = VPM_VP3_MOD_ID;
      p_voice_param->param_id = VPM_VP3_SIZE_PARAM_ID;
      p_voice_param->param_size = sizeof(vss_ivp3_rsp_get_size_t);
      p_voice_param->reserved = 0;

      /* Set the command status to pending */
      vpm_set_session_cmd_pending_status(session_obj->session_id, TRUE);

      /* Set the command response pending status to TRUE  */
      vpm_set_cmd_rsp_pending_status(session_obj, TRUE);

      /* Set the ack payload pointer */
      session_obj->session_rsp_ctrl.p_ack_payload = payload_address;

      /* Set the response handler pointer */
      session_obj->session_rsp_ctrl.p_rsp_hndlr_fn = vpm_vp3_get_size_cmd_ctrl_rsp_fn;

      /* Send get param cmd to both dynamic services */
      if (ADSP_EOK != (result = vpm_set_get_param_to_dyn_svc(session_obj,
                                                                payload_address,
                                                                payload_size,
                                                                ELITE_CMD_GET_PARAM,
                                                                CVD_CAL_PARAM_MINOR_VERSION_0)))
      {
         MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_vp3_get_size_cmd_ctrl(): Failed to send get param cmd to dyn svcs, result: 0x%lx", result);

         /* Clear the command pending status */
         vpm_set_session_cmd_pending_status(session_obj->session_id, FALSE);

         /* Clear the command response pending TRUE  */
         vpm_set_cmd_rsp_pending_status(session_obj, FALSE);

         if (payload_address)
         {
            /* Free up the allocated payload for in-band get param */
            qurt_elite_memory_free(payload_address);
         }

         /* End the APR command */
         elite_apr_if_end_cmd(session_obj->self_apr_handle, p_apr_pkt, result);
      }

      /* Return and wait for get param responses from dynamic service */
      return ADSP_EOK;
   }

   /* Control will reach here after getting responses for all get param
      commands */

   /* Clear the command pending status */
   vpm_set_session_cmd_pending_status(session_obj->session_id, FALSE);

   /* Free up the current command APR packet */
   elite_apr_if_free(session_obj->self_apr_handle, p_apr_pkt);

   return result;
}
