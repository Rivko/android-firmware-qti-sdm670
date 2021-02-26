/*========================================================================
   This file contains AFE custiom cmd handler implementations

  Copyright (c) 2009-2012, 2017 Qualcomm Technologies, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.
 
  $Header: //components/rel/avs.adsp/2.8.5/afe/services/static/src/AFEPortCustMsgHandler.cpp#1 $
 ====================================================================== */

/*==========================================================================
  Include files
  ========================================================================== */
#include "AFEPortManagement.h"
#include "AFEInternal.h"
#include "EliteMsg_AfeCustom.h"
#include "AFECodecHandler.h"
#include "topo_utils.h"
#include "AFESvc_topo.h"
#include "AFEErrorHandler.h"
#include "AFEHdmiOutputDrv.h"
#include "AFEDtmf.h"

/*==========================================================================
  Function Definitions
========================================================================== */
/** * This function is about custom msg process
 *
 * @param[in] pDevPort, pointer to instance of the port
 * @param[in] pMsg, pointer to message
 * @return the ADSP_EOK error code
 */
ADSPResult afe_port_custom_msg_handler(void* vpDevPort, elite_msg_any_t* pMsg)
{
   ADSPResult                      result = ADSP_EOK;
   afe_dev_port_t                 *pDevPort;

   pDevPort = (afe_dev_port_t *)vpDevPort;

   switch (((elite_msg_custom_header_t *)pMsg->pPayload)->unSecOpCode)
   {
      case ELITEMSG_CUSTOM_AFECONNECT_REQ:
      {
         result = afe_port_client_connect_handler((void *)pDevPort, pMsg);
      }
      break;
      case ELITEMSG_CUSTOM_AFEDISCONNECT_REQ:
      {
         result = afe_port_client_disconnect_handler((void *)pDevPort, pMsg);
      }
      break;
      case ELITEMSG_CUSTOM_AFECLIENTDISABLE:
      {
         result = afe_port_client_disable_handler(pDevPort, pMsg);
      }
      break;
      case ELITEMSG_CUSTOM_AFECLIENTENABLE:
      {
         result = afe_port_client_enable_handler(pDevPort, pMsg);
      }
      break;
      case ELITEMSG_CUSTOM_AFECLIENT_DC_RESET:
      {
         result = afe_port_client_dc_reset_handler(pDevPort, pMsg);
      }
      break;
      case ELITEMSG_CUSTOM_TERMINATE_THREAD: 
      {
         result = ADSP_ETERMINATED;
         elite_msg_return_payload_buffer(pMsg);
      }
      break;
      case ELITEMSG_CUSTOM_AFE_LOOPBACK_CONNECT_REQ:
      {
         result = afe_port_client_loopback_connect_handler(pDevPort, pMsg);
      }
      break;
      case ELITEMSG_CUSTOM_AFECLIENT_DISCONNECT_MARKER:
      {
         result = afe_port_register_client_disconnect_marker_handler(pDevPort, pMsg);
      }
      break;
      case ELITEMSG_CUSTOM_AFE_CLIENT_CRITERIA_REGISTER:
      {
        result = afe_port_client_criteria_register_handler(pDevPort, pMsg);
      }
      break;
      case ELITEMSG_CUSTOM_AFECLIENT_MEDIA_FMT_UPDATE:
      {
         result = afe_port_update_client_media_fmt(pDevPort, pMsg);
      }
      break;
      case ELITEMSG_CUSTOM_DEV_ERR_RECOVERY:
      {
         result = afe_err_handler_dev_recovery_msg(pDevPort, pMsg);
      }
      break;
      default:
      {
         elite_msg_ack_msg(pMsg, ADSP_EBADPARAM);
         result = ADSP_EOK;
      }
   }

   return result;
}

/**
 * This function is command handler for general ELITE_CUSTOM_MSG.
 *
 * @param[in] pAudStatAfeSvc, pointer to AfeSvc instance.
 * @param[in] pMsg, pointer to message.
 * @return the ADSP_EOK error code
 */
ADSPResult afe_svc_custom_msg_handler(void* pAudStatAfeSvc, elite_msg_any_t* pMsg)
{
   ADSPResult                      result = ADSP_EOK;
   elite_svc_handle_t              *pHDevService;
   uint16_t                        unPortId;
   uint32_t                        cmd;
   afe_dev_port_t                  *pDevPort;

   pHDevService = &(((aud_stat_afe_svc_t *)pAudStatAfeSvc)->afe_service_handle);
   cmd = ((elite_msg_custom_header_t *)pMsg->pPayload)->unSecOpCode;

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "At <addr/instance id 0x%lx>  FADDCMD  <0x%lx> ", (uint32_t)pHDevService,cmd);

   switch (cmd)
   {
      case ELITEMSG_CUSTOM_AFECONNECT_REQ:
      case ELITEMSG_CUSTOM_AFEDISCONNECT_REQ:
      case ELITEMSG_CUSTOM_AFECLIENTENABLE:
      case ELITEMSG_CUSTOM_AFECLIENTDISABLE:
      case ELITEMSG_CUSTOM_AFECLIENT_DC_RESET:
      case ELITEMSG_CUSTOM_AFE_LOOPBACK_CONNECT_REQ:
      case ELITEMSG_CUSTOM_AFECLIENT_DISCONNECT_MARKER:
      case ELITEMSG_CUSTOM_AFE_CLIENT_CRITERIA_REGISTER:
      case ELITEMSG_CUSTOM_AFECLIENT_MEDIA_FMT_UPDATE:
      case ELITEMSG_CUSTOM_DEV_ERR_RECOVERY:
      {
         //find the corresponding port and it's cmdQ, then, forward it.
         elite_msg_custom_afe_header_t   *pCustomAfeMsgPayload;
         pCustomAfeMsgPayload    = (elite_msg_custom_afe_header_t *)pMsg->pPayload;
         unPortId                = pCustomAfeMsgPayload->afe_id;
         if (ADSP_EOK != (result = afe_svc_get_port( unPortId, &pDevPort)))
         {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFESvc: Can't find port 0x%x for the custom Cmd = %d", unPortId, result);
            afe_svc_print_status_message(ADSP_EFAILED);
            elite_msg_ack_msg(pMsg, ADSP_EFAILED);
         }
         else
         {
             //the command handler should effectively place the mutex.
             //it should also ack back to the client.
             MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFESvc: executing %8lx ", cmd);
             result = afe_port_custom_msg_handler(pDevPort, pMsg);
             afe_svc_print_status_message(result);
         }
      }
      break;
      case ELITEMSG_CUSTOM_VFR_SYNC:
      case ELITEMSG_CUSTOM_VFR_DISABLE:
      {
         qurt_elite_queue_t *vfr_cmdq_ptr = vfr_drv_global_stat.vfr_cmdq_ptr;
         if (NULL != vfr_cmdq_ptr)
         {
            MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFESvc: delegating %8lx to VFR ", cmd);
            if (ADSP_FAILED(result = qurt_elite_queue_push_back(vfr_cmdq_ptr, (uint64_t*) pMsg )))
            {
               MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFESvc: Thread encountered VFR qurt_elite_queue_push_back error = %d!!\n", result);
            }
         }
         else
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFESvc: Can't find VFR cmdQ for the Custom Cmd = %d!!\n", result);
         }
         if (ADSP_FAILED(result))
         {
            elite_msg_ack_msg(pMsg, ADSP_EFAILED);
            afe_svc_print_status_message(ADSP_EFAILED);
         }
         break;
      }
      case ELITEMSG_CUSTOM_RIVA_IPC_COMMAND:
      {
         //Process the Ipc command.
         result = afe_device_generic_cmd(cmd, (aud_stat_afe_svc_t*)pAudStatAfeSvc, NULL);
         elite_msg_finish_msg(pMsg, result);
         break;
      }
      case ELITEMSG_CUSTOM_AFEBGT_DESTROY:
      {
    	  elite_msg_custom_afe_header_t *msg_payload_ptr =(elite_msg_custom_afe_header_t *) pMsg->pPayload;
    	  result = afe_bgt_destroy(msg_payload_ptr->payload_ptr);
    	  elite_msg_finish_msg(pMsg, result);
    	  break;
      }
      case ELITEMSG_CUSTOM_BGT_DESTROY:
      {
        elite_msg_custom_bgt_t *msg_payload_ptr =(elite_msg_custom_bgt_t *) pMsg->pPayload;
        module_bgt_info_t *bgt_info_ptr = (module_bgt_info_t *)msg_payload_ptr->payload_ptr;

        //Cleanup the feedback if enabled in BGT
        if(bgt_info_ptr->feedback_handle)
        {
           if(ADSP_EOK
                 != afe_port_disable_feedback_path(bgt_info_ptr->src_id, bgt_info_ptr->dest_id,
                                                   bgt_info_ptr->feedback_handle))

           {
              MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Disable feedback path BGT failed for fb handle: 0x%lu",
                    bgt_info_ptr->feedback_handle);
           }
        }

        result = bgt_destroy(bgt_info_ptr);
        elite_msg_finish_msg(pMsg, result);
        break;
      }
      case ELITEMSG_CUSTOM_CDC_INT_CLIENT_SIG:
      {
         result = afe_port_custom_cdc_int_handling(pMsg);
         elite_msg_finish_msg(pMsg, result);
      }
      break;
      case ELITEMSG_CUSTOM_HDMI_OVER_DP:
      {
         //use afe_svc_custom_msg_handler as this UNMUTE/MUTE is related to VSYNC not indiviual AFE Ports
         //consider multi-stream support in DP case for future
         result = afe_hdmi_over_dp_cust_msg_handler(pMsg->pPayload);
         elite_msg_finish_msg(pMsg, result);
      }
      break;
      case ELITEMSG_CUSTOM_TIMED_DTMF_CONFIG:
      {
         result = afe_svc_set_dtmf_config(pMsg);
         elite_msg_finish_msg(pMsg, result);
         break;
      }
      default:
      {
         result = ADSP_EBADPARAM;
         afe_svc_print_status_message(result);
         elite_msg_ack_msg(pMsg, ADSP_EBADPARAM);
         break;
      }
   }
   return result;
}
