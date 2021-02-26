/**========================================================================
 @file vpm_session_dyn_svc_cfg_utils.cpp
 
 @brief This file contains configuration utilites for voice proc Tx and Rx
 dynamic service
 
 Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 All rights reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 ====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/vsd/common/cvd/cvp/src/vpm_session_dyn_svc_cfg_utils.cpp#2 $

when       who     what, where, why
--------   ---     -------------------------------------------------------

========================================================================== */

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "mmstd.h"
#include "qurt_elite.h"
#include "Elite.h"
#include "adsp_afe_service_commands.h"
#include "adsp_vpm_api.h"
#include "VoiceProcTx.h"
#include "VoiceProcRx.h"
#include "VoiceMixerSvc.h"
#include "voice_proc_mgr.h"
#include "EliteMsg_AfeCustom.h"
#include "AFEInterface.h"

#include "vss_params_public_if.h"
#include "vpm_svc_apr_if.h"
#include "vpm_session_dyn_svc_cfg_utils.h"

#include "avtimer_drv_api.h"
#include "vpm_response_handler.h"
#include "vpm_session_utils.h"
#include "VoiceLoggingUtils.h"

/* -----------------------------------------------------------------------
** Struct Definitions
** ----------------------------------------------------------------------- */

extern elite_svc_handle_t *txVoiceMatrix;
extern elite_svc_handle_t *rxVoiceMatrix;

/* -----------------------------------------------------------------------
** Function Definitions
** ----------------------------------------------------------------------- */

/* provide proper far reference port for Vptx considering the topology id and external mixing mode */
uint16_t vpm_get_ec_ref_port(vpm_session_object_t *p_session)
{

   uint16_t ec_ref_port = VSS_IVOCPROC_PORT_ID_NONE;

   /* External EC mode */
   if (VPM_TX_EXT_MIX_MODE == p_session->target_set.vocproc_mode)
   {
      if (VSS_IVOCPROC_PORT_ID_NONE != p_session->target_set.ec_ref_port_id)
      {
         ec_ref_port = p_session->target_set.ec_ref_port_id;
      }
   }
   else /* Internal EC mode */
   {
      if (VSS_IVOCPROC_PORT_ID_NONE != p_session->target_set.rx_port_id)
      {
         ec_ref_port = p_session->target_set.rx_port_id;
      }
   }

   if (VSS_IVOCPROC_TOPOLOGY_ID_NONE == p_session->target_set.tx_topology_id)
   {
      ec_ref_port = VSS_IVOCPROC_PORT_ID_NONE;
   }

   /* Update the target EC ref port ID */
   p_session->target_set.ec_ref_port_id = ec_ref_port;

   return ec_ref_port;
}

static ADSPResult vpm_update_additional_create_params(vpm_session_object_t *p_session,
                                                      void *create_param,
                                                      uint8_t tx_rx_flag)
{
   ADSPResult  result = ADSP_EOK;
   uint32_t    size = 0;
   uint32_t    total_param_size = p_session->topo_param.vpm_param_size;
   uint32      payload_address = (uint32_t)p_session->topo_param.vpm_param;
   vptx_create_params_v2_t *create_param_tx = NULL;
   vprx_create_params_t *create_param_rx = NULL;

   if ( VSS_IVOCPROC_DIRECTION_RX == tx_rx_flag )
   {
     create_param_rx = ( vprx_create_params_t* ) create_param;

     /* Populate create param payload */
     create_param_rx->afe_port_id   = p_session->target_set.rx_port_id;
     create_param_rx->sampling_rate = p_session->target_set.system_config.rx_pp_sr;
     create_param_rx->topology_id   = p_session->target_set.rx_topology_id;

     create_param_rx->session_num   =  p_session->self_apr_port;
     create_param_rx->shared_mem_client = vpm_svc_get_mem_map_client();
     create_param_rx->num_channels  = 1;

     /* Check if device channel info is registered. */
     if ( p_session->target_set.dev_chan_info.rx.num_channels > 0 )  
     {
        create_param_rx->num_channels  = p_session->target_set.dev_chan_info.rx.num_channels;
        
        MSG_1(MSG_SSID_DFLT, DBG_HIGH_PRIO,
                 "vpm_update_additional_create_params(): Num Rx channels (%d)",
                 create_param_rx->num_channels );

        mmstd_memcpy( &create_param_rx->channel_map[0], sizeof(create_param_rx->channel_map), 
                      &p_session->target_set.dev_chan_info.rx.channel_mapping[0],  
                      sizeof(p_session->target_set.dev_chan_info.rx.channel_mapping) );
     }
   }
   else if ( VSS_IVOCPROC_DIRECTION_TX == tx_rx_flag )
   {
     create_param_tx = ( vptx_create_params_v2_t* ) create_param;

     /* Populate the create param payload with default values. */
     create_param_tx->num_mics = 0;
     create_param_tx->near_port_id = p_session->target_set.tx_port_id;;
     create_param_tx->far_port_id = vpm_get_ec_ref_port(p_session);
     create_param_tx->topology_id = p_session->target_set.tx_topology_id;
     create_param_tx->sampling_rate = p_session->target_set.system_config.tx_pp_sr;
     create_param_tx->session_num = p_session->self_apr_port;
     create_param_tx->shared_mem_client = vpm_svc_get_mem_map_client();
     create_param_tx->static_svc_apr_handle = voice_proc_mgr_get_apr_handle();
     create_param_tx->num_ref_channels = 1;

     /* Check if device channel info is registered. */
     if (  p_session->target_set.dev_chan_info.ec_ref.num_channels > 0 ) 
     {
        create_param_tx->num_ref_channels = p_session->target_set.dev_chan_info.ec_ref.num_channels;

        mmstd_memcpy( &create_param_tx->channel_map_far[0], sizeof(create_param_tx->channel_map_far), 
                      &p_session->target_set.dev_chan_info.ec_ref.channel_mapping[0],  
                      sizeof(p_session->target_set.dev_chan_info.ec_ref.channel_mapping) );
        MSG_1(MSG_SSID_DFLT, DBG_HIGH_PRIO,
                 "vpm_update_additional_create_params(): Num EC ref channels (%d)",
                 create_param_tx->num_ref_channels);
     }
     if ( p_session->target_set.dev_chan_info.tx.num_channels > 0 )  
     {
        create_param_tx->num_mics = p_session->target_set.dev_chan_info.tx.num_channels;
        
        mmstd_memcpy( &create_param_tx->channel_map_near[0], sizeof(create_param_tx->channel_map_near), 
                      &p_session->target_set.dev_chan_info.tx.channel_mapping[0],  
                      sizeof(p_session->target_set.dev_chan_info.tx.channel_mapping) );
        MSG_2(MSG_SSID_DFLT, DBG_HIGH_PRIO,
                 "vpm_update_additional_create_params(): Num channels for Tx topology(0x%lx): (%lu)",
                 create_param_tx->topology_id, create_param_tx->num_mics);
        return ADSP_EOK;
     }
    
     /* Check if the topology is known, if yes, then set the number of channels
      * else read the number of channels from virtual address */
     for (uint32_t i = 0, j = 0; i < sizeof(topo_vptx); i += sizeof(topo_vptx[0]), j++)
     {
        if (create_param_tx->topology_id == topo_vptx[j].topology_id)
        {
           create_param_tx->num_mics = topo_vptx[j].num_in_channels;

           MSG_2(MSG_SSID_DFLT, DBG_HIGH_PRIO,
                 "vpm_update_additional_create_params(): Num channels for Tx topology(0x%lx): (%lu)",
                 create_param_tx->topology_id, create_param_tx->num_mics);

           return ADSP_EOK;
        }
     }
     
     MSG_1(MSG_SSID_DFLT, DBG_HIGH_PRIO,
           "vpm_update_additional_create_params(): VPTx topology(0x%lx) is a custom topo."
           " Reading number of channels from virtual memory",
           create_param_tx->topology_id);

     if (NULL == payload_address)
     {
        MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
              "vpm_update_additional_create_params(): Virtual address is null for custom topology(0x%lx)",
              create_param_tx->topology_id);

        return ADSP_EBADPARAM;
     }

     create_param_tx->num_mics = 0;

     do
     {
        vpm_create_param_t *additional_create_params = (vpm_create_param_t *)payload_address;

        switch (additional_create_params->param_id)
        {
           case VSS_PARAM_NUM_DEV_CHANNELS:
           {
              vss_param_num_dev_channels_t *channel_info_ptr =\
                 (vss_param_num_dev_channels_t *)additional_create_params->param_virt_addr;

              if ((NULL == channel_info_ptr) ||
                  (additional_create_params->param_size != sizeof(vss_param_num_dev_channels_t)))
              {
                 MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                       " Incorrect virtual address and/orsize. Required size (%ld) Received size (%ld)",
                       sizeof(vss_param_num_dev_channels_t),
                       additional_create_params->param_size);

                 return ADSP_EBADPARAM;
              }

              create_param_tx->num_mics = channel_info_ptr->tx_num_channels;

              MSG_2(MSG_SSID_DFLT, DBG_HIGH_PRIO,
                    "vpm_update_additional_create_params(), Num channels for Custom Tx topology(0x%lx) = (%ld)",
                    create_param_tx->topology_id, create_param_tx->num_mics);

              break;
           }
           default:
           {
              MSG_1(MSG_SSID_DFLT, DBG_HIGH_PRIO,
                    "vpm_update_additional_create_params(), Unrecognized param id (0x%lx)"
                    " during vocproc creation/reconfig ",
                    additional_create_params->param_id);
              break;
           }
        }

        size += sizeof(vpm_create_param_t);
        payload_address += sizeof(vpm_create_param_t);

     } while (size < total_param_size);

     if (0 == create_param_tx->num_mics)
     {
        MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
              "vpm_update_additional_create_params(), num_channel is not received for custom topo(0x%lx)",
              create_param_tx->topology_id);

        return ADSP_EFAILED;
     }
   }

   return ADSP_EOK;
}

ADSPResult vpm_configure_rx_mixer_output(vpm_session_object_t *p_session,
                                         uint32_t vmx_outport_config,
                                         uint16_t sampling_rate)
{
   ADSPResult           result = ADSP_EOK;
   elite_msg_any_t      matrix_connect_msg;
   uint32_t             nSize = sizeof(elite_msg_custom_vmx_cfg_outports_t);
   vpm_dyn_svc_info_t   *p_dyn_svc = &p_session->dyn_svc_info;
   uint32_t             session_id;

   session_id = p_session->self_apr_port;

   if (ADSP_EOK != (result = elite_msg_create_msg(&matrix_connect_msg,
                                                  &nSize,
                                                  ELITE_CUSTOM_MSG,
                                                  p_session->session_rsp_ctrl.p_rsp_queue,
                                                  NULL,
                                                  ADSP_EOK)))
   {
      MSG_1(MSG_SSID_DFLT, DBG_HIGH_PRIO,
            "vpm_configure_rx_mixer_output(), FAILED to create EliteMsg, session_id: 0x%lx",
            session_id);

      return result;
   }

   elite_msg_custom_vmx_cfg_outports_t *outportspayload =\
      (elite_msg_custom_vmx_cfg_outports_t *)matrix_connect_msg.pPayload;

   outportspayload->unSecOpCode = ELITEMSG_CUSTOM_VMX_CFG_OUTPUT_PORTS;
   outportspayload->cfgOutPortsPayload.ack_ptr = &p_dyn_svc->mixer_out_port; //&me->session[nSessionIndex]->mixer_out_port;
   outportspayload->cfgOutPortsPayload.configuration = vmx_outport_config;

   if (VMX_NEW_OUTPUT == vmx_outport_config)
   {
      outportspayload->cfgOutPortsPayload.index = -1;
      outportspayload->cfgOutPortsPayload.num_output_bufs = 2;
      outportspayload->cfgOutPortsPayload.max_bufq_capacity = 4;
      outportspayload->cfgOutPortsPayload.output_mapping_mask = p_dyn_svc->output_mapping_mask = 0;
      //me->session[nSessionIndex]->output_mapping_mask = 0;
   }
   else /* VMX_RECONFIGURE_OUTPUT */
   {
      outportspayload->cfgOutPortsPayload.index = p_dyn_svc->mixer_out_port->outport_id;
   }

   outportspayload->cfgOutPortsPayload.svc_handle_ptr = p_dyn_svc->rx;
   outportspayload->cfgOutPortsPayload.out_sample_rate = sampling_rate / 1000;

   if (ADSP_FAILED(result = qurt_elite_queue_push_back(rxVoiceMatrix->cmdQ,
                                                       (uint64_t *)&matrix_connect_msg)))
   {
      MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO, "Unable to push to Rx matrix cmdQ, result: 0x%lx", result);

      elite_msg_return_payload_buffer(&matrix_connect_msg);

      return result;
   }

   /* Blocking wait for the response */
   result = elite_svc_wait_for_ack(&matrix_connect_msg);

   result = outportspayload->unResponseResult;

   /* free the msg paload buffer */
   elite_msg_return_payload_buffer(&matrix_connect_msg);

   /* Check if the failed response */
   if (ADSP_FAILED(result))
   {
      MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
            "vpm_configure_rx_mixer_output(): "
            " Failed to connect to rx matrix, session_id: 0x%lx, result: 0x%lx", session_id, result);
   }
   else
   {
      MSG_1(MSG_SSID_DFLT, DBG_HIGH_PRIO,
            "vpm_create_rx_mixer_output() SUCCESS, session_id: 0x%lx",
            session_id);
   }

   return result;
}

ADSPResult vpm_configure_tx_mixer_input(vpm_session_object_t *p_session,
                                        uint32_t vmx_inport_config)
{
   ADSPResult           result = ADSP_EOK;
   elite_msg_any_t      matrix_connect_msg;
   uint32_t             nSize = sizeof(elite_msg_custom_vmx_cfg_inports_t);
   vpm_dyn_svc_info_t   *p_dyn_svc = &p_session->dyn_svc_info;
   uint32_t             vmx_inp_port_prio;
   uint32_t             session_id;

   session_id = p_session->self_apr_port;

   if (!is_pseudo_port_id(p_session->target_set.tx_port_id))
   {
      vmx_inp_port_prio = VMX_INPUT_HIGH_PRIO;
   }
   else
   {
      vmx_inp_port_prio = VMX_INPUT_LOW_PRIO;
   }

   if (ADSP_EOK != (result = elite_msg_create_msg(&matrix_connect_msg,
                                                  &nSize,
                                                  ELITE_CUSTOM_MSG,
                                                  p_session->session_rsp_ctrl.p_rsp_queue,
                                                  NULL,
                                                  ADSP_EOK)))
   {
      MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
          "vpm_configure_tx_mixer_input(): FAILED to create EliteMsg");
      return result;
   }

   elite_msg_custom_vmx_cfg_inports_t *inportspayload =\
      (elite_msg_custom_vmx_cfg_inports_t *)matrix_connect_msg.pPayload;

   inportspayload->unSecOpCode = ELITEMSG_CUSTOM_VMX_CFG_INPUT_PORTS;
   inportspayload->cfgInPortsPayload.ack_ptr = &p_dyn_svc->mixer_in_port;
   inportspayload->cfgInPortsPayload.configuration = vmx_inport_config;
   inportspayload->cfgInPortsPayload.input_mapping_mask = p_dyn_svc->input_mapping_mask = 0;
   inportspayload->cfgInPortsPayload.priority = vmx_inp_port_prio;

   if (VMX_NEW_INPUT == vmx_inport_config)
   {
      inportspayload->cfgInPortsPayload.index = -1;
   }
   else /* VMX_RECONFIGURE_INPUT */
   {
      inportspayload->cfgInPortsPayload.index = p_dyn_svc->mixer_in_port->inport_id;
   }

   if (ADSP_FAILED(result = qurt_elite_queue_push_back(txVoiceMatrix->cmdQ,
                                                       (uint64_t *)&matrix_connect_msg)))
   {
      MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
            "Failed to push msg to tx matrix cmdQ, result: 0x%lx", result);

      elite_msg_return_payload_buffer(&matrix_connect_msg)
      ;
      return result;
   }

   result = elite_svc_wait_for_ack(&matrix_connect_msg);

   result = inportspayload->unResponseResult;

   elite_msg_return_payload_buffer(&matrix_connect_msg);

   if (ADSP_FAILED(result))
   {
      MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
            "vpm_configure_tx_mixer_input(), Failed to connect to tx matrix,"
            " session_id: 0x%lx, result: 0x%lx",
            session_id, result);
   }
   else
   {
      MSG_1(MSG_SSID_DFLT, DBG_HIGH_PRIO,
            "vpm_create_tx_mixer_input() SUCCESS, session_id: 0x%lx",
            session_id);
   }

   return result;
}

ADSPResult vpm_connect_peer_svc(uint32_t sec_opcode, qurt_elite_queue_t *p_peer_svc_cmd_q,
                                qurt_elite_queue_t *p_vpm_svc_rsp_q, elite_svc_handle_t *h_src_svc)
{
   ADSPResult        result = ADSP_EOK;
   elite_msg_any_t   msg;
   uint32_t          nSize;

   elite_msg_custom_voc_svc_connect_type *p_vpm_connect_payload;

   nSize = sizeof(elite_msg_custom_voc_svc_connect_type);

   if (ADSP_EOK != (result = elite_msg_create_msg(&msg,
                                                  &nSize,
                                                  ELITE_CUSTOM_MSG,
                                                  p_vpm_svc_rsp_q,
                                                  NULL,
                                                  ADSP_EOK)))
   {
      MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
            "vpm_connect_peer_svc(): Failed to create EliteMsg for peer (dis)connect, sec_opcode: 0x%lx",
            sec_opcode);

      return result;
   }

   p_vpm_connect_payload = (elite_msg_custom_voc_svc_connect_type *)msg.pPayload;

   p_vpm_connect_payload->sec_opcode = sec_opcode;
   p_vpm_connect_payload->svc_handle_ptr  = h_src_svc;

   if (ADSP_EOK != (result = qurt_elite_queue_push_back(p_peer_svc_cmd_q, (uint64_t *)&msg)))
   {
      MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO, "Failed to push (dis)connect msg to peer services!!");

      elite_msg_return_payload_buffer(&msg);

      return result;
   }

   /* Blocking wait for the ACK */
   elite_svc_wait_for_ack(&msg);

   /* Get the result */
   result = p_vpm_connect_payload->unResponseResult;

   /* Return the payload buffer to return queue */
   elite_msg_return_payload_buffer(&msg);

   /* Check if the connect msg failed */
   if (ADSP_FAILED(result))
   {
      MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
            "Failed to connect(8)/disconnet(9) vpTx/Rx, sec_opcode: %lu, result: 0x%lx",
            sec_opcode, result);
   }
   else
   {
      MSG_1(MSG_SSID_DFLT, DBG_HIGH_PRIO,
            "Successfully connected(8)/disconneted(9) vpTx/Rx, sec_opcode: 0x%lx", sec_opcode);
   }

   return result;
}

ADSPResult vpm_connect_vptx_to_vmx_inp_port(qurt_elite_queue_t *p_vptx_cmd_q,
                                            qurt_elite_queue_t *p_vpm_svc_rsp_q,
                                            elite_svc_handle_t *h_vmx_inp_port)
{
   ADSPResult                 result = ADSP_EOK;
   uint32_t                   payload_size;
   elite_msg_any_t            tx_connect_msg;
   elite_msg_cmd_connect_t    *p_tx_connect_params;

   payload_size = sizeof(elite_msg_cmd_connect_t);

   if (ADSP_EOK != (result = elite_msg_create_msg(&tx_connect_msg,
                                                  &payload_size,
                                                  ELITE_CMD_CONNECT,
                                                  p_vpm_svc_rsp_q,
                                                  NULL,
                                                  ADSP_EOK)))
   {
      MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
            "vpm_connect_vptx_to_vmx_inp_port(), Failed to create EliteMsg"
            " for peer connect, opcode: 0x%lx",
            ELITE_CMD_CONNECT);

      return result;
   }

   p_tx_connect_params = (elite_msg_cmd_connect_t *)tx_connect_msg.pPayload;

   /* Populate the mixer service handle */
   p_tx_connect_params->pSvcHandle = h_vmx_inp_port;

   /* Connect VPTx to acquired VMX input port */
   if (ADSP_EOK != (result = qurt_elite_queue_push_back(p_vptx_cmd_q,
                                                        (uint64_t *)&tx_connect_msg)))
   {
      MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
            "Tx session connect to mixer cmd push back error result(0x%lx)",
            result);

      elite_msg_return_payload_buffer(&tx_connect_msg);

      return result;
   }

   elite_svc_wait_for_ack(&tx_connect_msg);

   result = p_tx_connect_params->unResponseResult;

   elite_msg_return_payload_buffer(&tx_connect_msg);

   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
            "vpm_connect_vptx_to_vmx_inp_port(), Failed to attach vpTx tp VMX inp port, result: 0x%lx",
            result);
   }
   else
   {
      MSG(MSG_SSID_DFLT, DBG_HIGH_PRIO,
          "vpm_connect_vptx_to_vmx_inp_port(), vpTx attach to VMX input port SUCCESS");
   }

   return result;
}


ADSPResult vpm_close_tx_mixer_input(vpm_session_object_t *p_session)
{
   ADSPResult           result = ADSP_EOK;
   uint32_t             nSize;
   vpm_dyn_svc_info_t   *p_dyn_svc = &p_session->dyn_svc_info;
   elite_msg_any_t      matrix_connect_msg;
   uint32_t             session_id;

   session_id = p_session->self_apr_port;

   if (!p_dyn_svc->mixer_in_port)
   {
      MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_close_tx_mixer_input(), vmx in port handle is NULL");
      return ADSP_EFAILED;
   }

   /* Size of the payload */
   nSize = sizeof(elite_msg_custom_vmx_cfg_inports_t);

   if (ADSP_EOK != (result = elite_msg_create_msg(&matrix_connect_msg,
                                                  &nSize,
                                                  ELITE_CUSTOM_MSG,
                                                  p_session->session_rsp_ctrl.p_rsp_queue,
                                                  NULL,
                                                  ADSP_EOK)))
   {
      MSG(MSG_SSID_DFLT, DBG_HIGH_PRIO, "Failed to create EliteMsg for tx mixer close");
      return result;
   }

   elite_msg_custom_vmx_cfg_inports_t *in_ports_payload =\
      (elite_msg_custom_vmx_cfg_inports_t *)matrix_connect_msg.pPayload;

   in_ports_payload->unSecOpCode = ELITEMSG_CUSTOM_VMX_CFG_INPUT_PORTS;
   in_ports_payload->cfgInPortsPayload.ack_ptr = &p_dyn_svc->mixer_in_port;
   in_ports_payload->cfgInPortsPayload.index = p_dyn_svc->mixer_in_port->inport_id;
   in_ports_payload->cfgInPortsPayload.configuration = VMX_CLOSE_INPUT;

   if (ADSP_FAILED(result = qurt_elite_queue_push_back(txVoiceMatrix->cmdQ,
                                                       (uint64_t *)&matrix_connect_msg)))
   {
      MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
            "Failed to push msg buf to Tx matrix cmdQ, result: 0x%lx", result);

      elite_msg_return_payload_buffer(&matrix_connect_msg);

      return result;
   }

   result = elite_svc_wait_for_ack(&matrix_connect_msg);

   result = in_ports_payload->unResponseResult;

   elite_msg_return_payload_buffer(&matrix_connect_msg);

   if (ADSP_FAILED(result))
   {
      MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
            "vpm_close_tx_mixer_input(), Failed to destroy tx matrix port,"
            " session_id: 0x%lx, result: 0x%lx",
            session_id, result);
   }
   else
   {
      MSG_1(MSG_SSID_DFLT, DBG_HIGH_PRIO,
            "vpm_close_tx_mixer_input() SUCCESS, session_id: 0x%lx",
            session_id);
   }

   return result;
}

ADSPResult vpm_close_rx_mixer_output(vpm_session_object_t *p_session)
{
   ADSPResult           result = ADSP_EOK;
   uint32_t             nSize;
   vpm_dyn_svc_info_t   *p_dyn_svc = &p_session->dyn_svc_info;
   elite_msg_any_t      matrix_connect_msg;
   uint32_t             session_id;

   session_id = p_session->self_apr_port;

   if (!p_dyn_svc->mixer_out_port)
   {
      MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
            "vpm_close_rx_mixer_output(), VMX out port handle is null, session_id: 0x%lx",
            session_id);
      return ADSP_EFAILED;
   }

   /* Size of the payload */
   nSize = sizeof(elite_msg_custom_vmx_cfg_outports_t);

   if (ADSP_EOK != (result = elite_msg_create_msg(&matrix_connect_msg,
                                                  &nSize,
                                                  ELITE_CUSTOM_MSG,
                                                  p_session->session_rsp_ctrl.p_rsp_queue,
                                                  NULL,
                                                  ADSP_EOK)))
   {
      MSG(MSG_SSID_DFLT, DBG_HIGH_PRIO, "FAILED to create EliteMsg for rx mixer close");

      return result;
   }

   elite_msg_custom_vmx_cfg_outports_t *out_ports_payload =\
      (elite_msg_custom_vmx_cfg_outports_t *)matrix_connect_msg.pPayload;

   out_ports_payload->unSecOpCode = ELITEMSG_CUSTOM_VMX_CFG_OUTPUT_PORTS;
   out_ports_payload->cfgOutPortsPayload.ack_ptr = &p_dyn_svc->mixer_out_port;
   out_ports_payload->cfgOutPortsPayload.index = p_dyn_svc->mixer_out_port->outport_id;
   out_ports_payload->cfgOutPortsPayload.configuration = VMX_CLOSE_OUTPUT;


   if (ADSP_FAILED(result = qurt_elite_queue_push_back(rxVoiceMatrix->cmdQ,
                                                       (uint64_t *)&matrix_connect_msg)))
   {
      MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
            "Failed to push to msg buf to Rx matrix cmdQ, result: %lu", result);

      elite_msg_return_payload_buffer(&matrix_connect_msg);
      return result;
   }

   result = elite_svc_wait_for_ack(&matrix_connect_msg);

   result = out_ports_payload->unResponseResult;

   elite_msg_return_payload_buffer(&matrix_connect_msg);

   if (ADSP_FAILED(result))
   {
      MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
            "vpm_close_rx_mixer_output(), Failed to connect to rx matrix,"
            " session_id: 0x%l, result: 0x%lx",
            session_id, result);
   }
   else
   {
      MSG_1(MSG_SSID_DFLT, DBG_HIGH_PRIO,
            "vpm_close_rx_mixer_output(), SUCCESS, session_id: 0x%lx",
            session_id);
   }

   return result;
}

ADSPResult vpm_destroy_dyn_svcs(elite_svc_handle_t *h_dyn_svc,
                                qurt_elite_queue_t *p_vpm_svc_rsp_q)
{
   ADSPResult           result = APR_EOK;
   qurt_elite_thread_t  thread_id = h_dyn_svc->threadId;

   /* The naming of this function is now a little inconsistent for Tx.  Tx has 2 step
    * process...first is to send ELITEMSG_CUSTOM_AFECLIENTDISABLE, which is done in
    * disconnect_afe_tx function.  After the VoiceProcTx is destroyed,
    * we can later finish second step of Disconnect, since we know the buffers are returend to
    * AFE Tx. In Rx, we can directly send ELITEMSG_CUSTOM_AFEDISCONNECT_REQ
    */

   MSG(MSG_SSID_DFLT, DBG_HIGH_PRIO, "vpm_destroy_dyn_svcs(), Begin");

   /* Send disconnect cmd to vpTx for disconnecting with RX */

   (void)voice_custom_msg_send(p_vpm_svc_rsp_q, /* response Q*/
                               h_dyn_svc->cmdQ, /* dstn svc cmd Q */
                               VOICEPROC_DESTROY_YOURSELF_CMD,
                               NULL, /* client token */
                               FALSE); /* wait for ACK */

   /* Wait for the threads to finish */
   qurt_elite_thread_join(thread_id, &result);

   MSG_1(MSG_SSID_DFLT, DBG_HIGH_PRIO, "vpm_destroy_dyn_svcs(), End, result: 0x%lx", result);

   return result;
}

ADSPResult vpm_create_tx_dyn_svc(vpm_session_object_t *p_session)
{
   ADSPResult                 result = ADSP_EOK;
   uint16_t                   rx_port_for_vptx;
   vptx_create_params_v2_t    create_param_tx;
   uint32_t                   session_id;
   vptx_vpm_apr_info_t        apr_info;

   /* Use unique APR port as session ID for this session */
   session_id = p_session->self_apr_port;

   /* Get the EC reference port ID based on EC mode (int/ext)*/
   rx_port_for_vptx = vpm_get_ec_ref_port(p_session);

   /* Clear the create param struct */
   memset(&create_param_tx, 0, sizeof(create_param_tx));

  /* Call this function to get the number of channels for V2 command as well.
      All the static topologies will be provided with prop num of channels */
   if (ADSP_FAILED(result = vpm_update_additional_create_params(p_session,
                                                                (void *)&create_param_tx,
                                                                VSS_IVOCPROC_DIRECTION_TX)))
   {
      MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
            "vpm_create_tx_dyn_svc(), Failed to get additional params for vpTx"
            " dyn svc creation, topo_id: 0x%lx",
            p_session->target_set.tx_topology_id);

      return ADSP_EFAILED;
   }

   apr_info.apr_handle = p_session->self_apr_handle;
   apr_info.self_addr = p_session->self_apr_addr;
   apr_info.self_port = p_session->self_apr_port;
   apr_info.client_addr = p_session->self_apr_addr;
   apr_info.client_port = p_session->self_apr_port;

   if (ADSP_FAILED(result = vptx_create((void **)&(p_session->dyn_svc_info.tx),
                                        (void **)&(p_session->dyn_svc_info.tx_far),
                                        &create_param_tx,
                                        &apr_info ) ) )
   {
      MSG_1(MSG_SSID_DFLT, DBG_FATAL_PRIO,
            "vpm_create_tx_dyn_svc(), VoiceProc Tx Creation Failed, topo_id :0x%lx",
            p_session->target_set.tx_topology_id);

      return ADSP_EFAILED;
   }
   else
   {
      MSG_2(MSG_SSID_DFLT, DBG_MED_PRIO,
            "vpm_create_tx_dyn_svc(), SUCCESS, session_id: (0x%lx), Handle(0x%lx)",
            session_id,
            p_session->dyn_svc_info.tx);

      /* Update number of channels */
      p_session->dyn_svc_info.tx_num_channels = create_param_tx.num_mics;
      p_session->dyn_svc_info.ec_ref_num_channels = create_param_tx.num_ref_channels;
   }
   return result;
}

ADSPResult vpm_create_rx_dyn_svc(vpm_session_object_t *p_session)
{
   ADSPResult              result = ADSP_EOK;
   vprx_create_params_t    create_param = { 0 };
   vpm_dyn_svc_info_t      *p_dyn_svc = &p_session->dyn_svc_info;
   uint32_t                session_id;

   /* Use unique APR port as session ID for this session */
   session_id = p_session->self_apr_port;

   memset(&create_param, 0, sizeof(create_param));
   if (ADSP_FAILED(result = vpm_update_additional_create_params(p_session,
                                                                (void *)&create_param,
                                                                VSS_IVOCPROC_DIRECTION_RX)))
   {
     MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
           "vpm_send_reconfig_msg_to_dyn_svc(), Failed to get additional params for VpRx"
           " dyn svc creation, topo_id: 0x%lx",
           p_session->target_set.rx_topology_id);

     return ADSP_EFAILED;
   }
   if (ADSP_FAILED(result = vprx_create((void **)&(p_dyn_svc->rx),
                                        &create_param)))
   {
      MSG_1(MSG_SSID_DFLT, DBG_FATAL_PRIO,
            "vpm_create_rx_dyn_svc(), Failed to create vpRx dyn service, topo_id: 0x%lx",
            p_session->target_set.rx_port_id);

      return ADSP_EFAILED;
   }
   else
   {
      MSG_2(MSG_SSID_DFLT, DBG_MED_PRIO,
            "vpm_create_rx_dyn_svc(): SUCCESS, session_id: (0x%lx), Handle(0x%lx)",
            session_id,
            p_dyn_svc->rx);

      /* Update number of channels */
      p_dyn_svc->rx_num_channels = create_param.num_channels;
   }

   return result;
}

uint32_t vpm_get_vmx_out_port_mask(uint32_t session_idx)
{
   uint32_t result = 0;

   if (session_idx >= VPM_MAX_NUM_SESSIONS)
   {
      MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
            "vpm_get_vmx_out_port_mask() session_idx(0x%lx) out of range", session_idx);

      return result;
   }

   vpm_session_object_t *p_session = p_vpm_svc_global_obj->sessions[session_idx];

   if (NULL == p_session)
   {
      MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
            "vpm_get_vmx_out_port_mask() NULL session 0x%lx, ignoring!", session_idx);

      return result;
   }

   if (p_session->dyn_svc_info.mixer_out_port)
   {
      //result |= 1 << (voiceProcMgr->session[index]->mixer_out_port->outport_id);
      result |= 1 << (p_session->dyn_svc_info.mixer_out_port->outport_id);
   }
   else
   {
      MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
            "vpm_get_vmx_out_port_mask(), NULL output port, session_idx: %lu",
            session_idx);
   }

   return result;
}

uint32_t vpm_get_vmx_in_port_mask(uint32_t session_idx)
{
   uint32_t result = 0;

   if (session_idx >= VPM_MAX_NUM_SESSIONS)
   {
      MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
            "vpm_get_vmx_in_port_mask() session_idx(%lu) out of range", session_idx);

      return result;
   }

   vpm_session_object_t *p_session = p_vpm_svc_global_obj->sessions[session_idx];

   if (NULL == p_session)
   {
      MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
            "vpm_get_vmx_in_port_mask() NULL session %lu, ignoring!", session_idx);

      return result;
   }

   if (p_session->dyn_svc_info.mixer_in_port)
   {
      //result |= 1<<(voiceProcMgr->session[index]->mixer_in_port->inport_id);
      result |= 1 << (p_session->dyn_svc_info.mixer_in_port->inport_id);
   }
   else
   {
      MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
            "vpm_get_vmx_in_port_mask(), NULL input port, session_idx: %lu",
            session_idx);
   }

   return result;
}

ADSPResult vpm_send_vfr_mode_to_dyn_svc(void *p_timing_param,
                                        qurt_elite_queue_t *p_session_rsp_q,
                                        qurt_elite_queue_t *p_dstn_svc_cmd_q,
                                        uint32_t sec_opcode)
{
   ADSPResult        result = ADSP_EOK;
   elite_msg_any_t   msg_pkt;
   uint32_t          payload_size;
   elite_msg_custom_voc_timing_param_type *pPayload = NULL;

   payload_size = sizeof(elite_msg_custom_voc_timing_param_type);

   if (ADSP_EOK != (result = elite_msg_create_msg(&msg_pkt,
                                                  &payload_size,
                                                  ELITE_CUSTOM_MSG,
                                                  p_session_rsp_q,
                                                  0,
                                                  ADSP_EOK)))
   {
      MSG(MSG_SSID_DFLT, DBG_HIGH_PRIO,
          "vpm_send_vfr_mode_to_dyn_svc(): FAILED to create EliteMsg!!");
      return result;
   }

   pPayload = (elite_msg_custom_voc_timing_param_type *)msg_pkt.pPayload;
   pPayload->param_data_ptr = (int32_t *)p_timing_param;
   pPayload->sec_opcode = sec_opcode;

   if (ADSP_EOK != (result = qurt_elite_queue_push_back(p_dstn_svc_cmd_q, (uint64_t *)&msg_pkt)))
   {
      MSG(MSG_SSID_DFLT, DBG_HIGH_PRIO,
          "vpm_send_vfr_mode_to_dyn_svc(): FAILED to set push buffer to dyn svc cmdQ");

      elite_msg_return_payload_buffer(&msg_pkt);
   }

   return result;
}

ADSPResult vpm_send_mute_cmd_to_dyn_svc(voice_set_soft_mute_v2_t *p_cmd_payload,
                                        qurt_elite_queue_t *p_session_rsp_q,
                                        qurt_elite_queue_t *p_dstn_svc_cmd_q)
{
   ADSPResult        result;
   elite_msg_any_t   msg_pkt;
   uint32_t          payload_size;

   elite_msg_custom_voc_set_soft_mute_type *pPayload = NULL;

   payload_size = sizeof(elite_msg_custom_voc_set_soft_mute_type);

   if (ADSP_EOK != (result = elite_msg_create_msg(&msg_pkt,
                                                  &payload_size,
                                                  ELITE_CUSTOM_MSG,
                                                  p_session_rsp_q,
                                                  0,
                                                  ADSP_EOK)))
   {
      MSG(MSG_SSID_DFLT, DBG_HIGH_PRIO, "VCP: FAILED to create EliteMsg!!");
      return result;
   }

   pPayload                     = (elite_msg_custom_voc_set_soft_mute_type *)msg_pkt.pPayload;
   pPayload->sec_opcode         = VOICEPROC_SET_MUTE_CMD;
   pPayload->mute               = p_cmd_payload->mute;
   pPayload->ramp_duration      = p_cmd_payload->ramp_duration;

   if (ADSP_EOK != (result = qurt_elite_queue_push_back(p_dstn_svc_cmd_q, (uint64_t *)&msg_pkt)))
   {
      MSG(MSG_SSID_DFLT, DBG_HIGH_PRIO,
          "vpm_send_mute_cmd_to_dyn_svc(): FAILED to set push buffer to dyn svc cmdQ");

      elite_msg_return_payload_buffer(&msg_pkt);

   }

   return result;
}

ADSPResult vpm_send_get_delay_cmd_to_dyn_svc(qurt_elite_queue_t *p_session_rsp_q,
                                             qurt_elite_queue_t *p_dstn_svc_cmd_q,
                                             void *cust_rsp_payload,
                                             uint32_t client_token)
{
   ADSPResult        result = ADSP_EOK;
   elite_msg_any_t   msg_pkt;
   uint32_t          payload_size;
   void              *rsp_payload;
   elite_msg_custom_delay_type  *p_msg_payload = NULL;

   payload_size = sizeof(elite_msg_custom_delay_type);

   if (ADSP_EOK != (result = elite_msg_create_msg(&msg_pkt,
                                                  &payload_size,
                                                  ELITE_CUSTOM_MSG,
                                                  p_session_rsp_q,
                                                  client_token,
                                                  ADSP_EOK)))
   {
      MSG(MSG_SSID_DFLT, DBG_HIGH_PRIO, "VCP: FAILED to create EliteMsg!!");
      return result;
   }

   p_msg_payload = (elite_msg_custom_delay_type *)msg_pkt.pPayload;

   p_msg_payload->delay_ptr = cust_rsp_payload;
   p_msg_payload->sec_opcode = VOICEPROC_GET_DELAY_CMD;

   if (ADSP_EOK != (result = qurt_elite_queue_push_back(p_dstn_svc_cmd_q, (uint64_t *)&msg_pkt)))
   {
      MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO, "VCP: FAILED to set push buffer to dyn svc cmdQ");

      elite_msg_return_payload_buffer(&msg_pkt);

   }

   return result;
}

ADSPResult vpm_send_get_kpps_cmd_to_dyn_svc(vpm_session_object_t *session_obj)
{
   ADSPResult        result = ADSP_EOK;
   ADSPResult        result_tx = ADSP_EOK, result_rx = ADSP_EOK;
   elite_msg_any_t   msg_pkt_tx, msg_pkt_rx;
   uint32_t          payload_size;
   void              *rsp_payload;
   uint32_t          session_id;

   elite_msg_custom_kpps_type  *p_msg_payload = NULL;

   session_id = session_obj->self_apr_port;

   /* Allocate the get kpps response payload */
   if (NULL == (rsp_payload = qurt_elite_memory_malloc(sizeof(vpm_get_kpps_ack_t),
                                                       QURT_ELITE_HEAP_DEFAULT)))
   {
      MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
          "vpm_send_get_kpps_cmd_to_dyn_svc(), FAILED to allocate get KPPS cmd rsp payload");

      return ADSP_EFAILED;
   }
   ( void ) mmstd_memset( rsp_payload, 0x0, sizeof(vpm_get_kpps_ack_t) );

   /* Set the cmd rsp pending status */
   vpm_set_cmd_rsp_pending_status(session_obj, TRUE);

   /* Set the ack payload */
   session_obj->session_rsp_ctrl.p_ack_payload = rsp_payload;

   /* Pointer to response handler function */
   session_obj->session_rsp_ctrl.p_rsp_hndlr_fn =  vpm_get_kpps_cmd_rsp_handler;

   payload_size = sizeof(elite_msg_custom_kpps_type);

   /* If Tx is present */
   if (session_obj->dyn_svc_info.tx)
   {
      if (ADSP_EOK == (result_tx = elite_msg_create_msg(&msg_pkt_tx,
                                                        &payload_size,
                                                        ELITE_CUSTOM_MSG,
                                                        session_obj->session_rsp_ctrl.p_rsp_queue,
                                                        VSS_IVOCPROC_DIRECTION_TX,
                                                        ADSP_EOK)))
      {
         p_msg_payload = (elite_msg_custom_kpps_type *)msg_pkt_tx.pPayload;

         p_msg_payload->pnKpps = rsp_payload;
         p_msg_payload->sec_opcode = VOICEPROC_GET_KPPS_CMD;

         if (ADSP_EOK == (result_tx = qurt_elite_queue_push_back(session_obj->dyn_svc_info.tx->cmdQ,
                                                                 (uint64_t *)&msg_pkt_tx)))
         {
            MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO,
                  "vpm_send_get_kpps_cmd_to_dyn_svc(): Successufully send to vpTx, session_id: 0x%lx",
                  session_id);

            /* Increment the number of cmd issued */
            session_obj->session_rsp_ctrl.num_cmd_issued++;
         }
         else
         {
            MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                "vpm_send_get_kpps_cmd_to_dyn_svc(): FAILED to set push buffer to vpTx dyn svc cmdQ");

            elite_msg_return_payload_buffer(&msg_pkt_tx);
         }
      }
      else
      {
         MSG(MSG_SSID_DFLT, DBG_HIGH_PRIO,
             "vpm_send_get_kpps_cmd_to_dyn_svc(): FAILED to create EliteMsg for vpTx get KPPS cmd !!");
      }
   } /* if (session_obj->dyn_svc_info.tx) */

   /* If Rx is present */
   if (session_obj->dyn_svc_info.rx)
   {
      if (ADSP_EOK == (result_rx = elite_msg_create_msg(&msg_pkt_rx,
                                                        &payload_size,
                                                        ELITE_CUSTOM_MSG,
                                                        session_obj->session_rsp_ctrl.p_rsp_queue,
                                                        VSS_IVOCPROC_DIRECTION_RX,
                                                        ADSP_EOK)))
      {
         p_msg_payload = (elite_msg_custom_kpps_type *)msg_pkt_rx.pPayload;

         p_msg_payload->pnKpps = rsp_payload;
         p_msg_payload->sec_opcode = VOICEPROC_GET_KPPS_CMD;

         if (ADSP_EOK == (result_rx = qurt_elite_queue_push_back(session_obj->dyn_svc_info.rx->cmdQ,
                                                                 (uint64_t *)&msg_pkt_rx)))
         {
            MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO,
                  "vpm_send_get_kpps_cmd_to_dyn_svc(): Successufully send to vpRx, session_id: 0x%lx",
                  session_id);

            /* Increment the number of cmd issued */
            session_obj->session_rsp_ctrl.num_cmd_issued++;
         }
         else
         {
            MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                "vpm_send_get_kpps_cmd_to_dyn_svc(), FAILED to push buffer to vpRx cmdQ");

            elite_msg_return_payload_buffer(&msg_pkt_rx);
         }
      }
      else
      {
         MSG(MSG_SSID_DFLT, DBG_HIGH_PRIO,
             "vpm_send_get_kpps_cmd_to_dyn_svc(),"
             " FAILED to create EliteMsg for vpRx get KPPS cmd !!");
      }
   } /* if (session_obj->dyn_svc_info.rx) */

   /* If both set params failed */
   if (!session_obj->session_rsp_ctrl.num_cmd_issued)
   {
      qurt_elite_memory_free(rsp_payload);

      /* Clear the cmd rsp pending status */
      vpm_set_cmd_rsp_pending_status(session_obj, FALSE);

      result = ADSP_EFAILED;
   }

   return result;
}

ADSPResult vpm_connect_afe_tx(vpm_session_object_t *p_session)
{
   ADSPResult           result = ADSP_EOK;
   afe_client_t         tx_afe_client;
   elite_msg_any_t      msg_pkt;
   uint32_t             payload_size;
   vpm_dyn_svc_info_t   *p_dyn_svc = &p_session->dyn_svc_info;
   uint32_t             session_id;

   session_id = p_session->self_apr_port;

   elite_msg_custom_afe_connect_t   *pPayload;

   MSG_4(MSG_SSID_DFLT, DBG_HIGH_PRIO,
         "VCP: Connecting to AFE Tx, SR(%d),txPort(0x%x),"
         " num_channel(%lu), session_id(0x%x)",
         p_session->active_set.system_config.tx_pp_sr,
         p_session->active_set.tx_port_id,
         p_session->dyn_svc_info.tx_num_channels,
         session_id);

   /***************************************************************************
     Connecting to afe tx port
    ***************************************************************************/

   /* Populate the tx client parameters */

   /* Initialize AFE client to all zeros (default values) */
   memset(&tx_afe_client, 0, sizeof(tx_afe_client));

   tx_afe_client.sample_rate      = p_session->active_set.system_config.tx_pp_sr;
   tx_afe_client.data_path        = AFE_PP_OUT;

   /* Get the number of input channels for corresponding Tx topology. */
   tx_afe_client.channel = p_session->dyn_svc_info.tx_num_channels;

   tx_afe_client.bytes_per_channel     = 2;
   tx_afe_client.bit_width             = 16;
   tx_afe_client.afe_dp_delay_ptr      = &p_session->dyn_svc_info.tx_afe_delay;
   tx_afe_client.afe_dev_hw_delay_ptr  = &p_session->dyn_svc_info.tx_afe_hw_delay;
   tx_afe_client.buf_size              = VPTX_DMA_BLOCK(p_session->active_set.system_config.tx_pp_sr);

   /* 21 buffers for 20ms frame and 1ms to handler jitter.
      this number should be greater that the maximum processing time of vptx */
   tx_afe_client.num_buffers                          = 21;
   tx_afe_client.interleaved                          = FALSE;
   tx_afe_client.cust_proc.resampler_type             = IIR_BASED_SRC;
   tx_afe_client.afe_client_handle                    = p_session->dyn_svc_info.tx;
   tx_afe_client.cust_proc.subscribe_to_avt_drift     = FALSE;
   tx_afe_client.cust_proc.is_pop_suppressor_required = FALSE;

   payload_size = sizeof(elite_msg_custom_afe_connect_t);

   if (ADSP_EOK != (result = elite_msg_create_msg(&msg_pkt,
                                                  &payload_size,
                                                  ELITE_CUSTOM_MSG,
                                                  p_session->session_rsp_ctrl.p_rsp_queue,
                                                  NULL,
                                                  ADSP_EOK)))
   {
      MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
          "vpm_connect_afe_tx(), FAILED to create EliteMsg for Tx afe connect !!");
      return result;
   }

   pPayload = (elite_msg_custom_afe_connect_t *)msg_pkt.pPayload;

   pPayload->sec_op_code   = ELITEMSG_CUSTOM_AFECONNECT_REQ;
   pPayload->afe_id        = p_session->active_set.tx_port_id;
   pPayload->afe_client    = tx_afe_client;

   /* Send the msg to device static service cmdQ */
   if (ADSP_FAILED(result = qurt_elite_queue_push_back(p_vpm_svc_global_obj->p_afe_svc_cmd_q,
                                                       (uint64_t *)&msg_pkt)))
   {
      MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
            "vpm_connect_afe_tx(), Failed to push msg buf to AFE cmd Q, result: 0x%lx", result);

      elite_msg_return_payload_buffer(&msg_pkt);
      return result;
   }

   /* Blocking wait for the response */
   elite_svc_wait_for_ack(&msg_pkt);

   pPayload = (elite_msg_custom_afe_connect_t *)msg_pkt.pPayload;

   if (ADSP_EOK == pPayload->response_result)
   {
      /* Save the AFE service handle */
      p_dyn_svc->tx_afe_handle = (elite_svc_handle_t *)pPayload->svc_handle_ptr;

      /* Save ptr to drift struct for tx port */
      p_dyn_svc->tx_afe_drift_ptr = (void *)pPayload->afe_drift_ptr;

      MSG_3(MSG_SSID_DFLT, DBG_HIGH_PRIO,
            "vpm_connect_afe_tx(): AFE Near Tx Connected, session_id(0x%lx),"
            " pHandle(0x%lx), pDrift(0x%lx)",
            session_id, p_dyn_svc->tx_afe_handle,
            p_dyn_svc->tx_afe_drift_ptr);
   }
   else
   {
      p_dyn_svc->tx_afe_handle = NULL;

      MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
            "vpm_connect_afe_tx(), TxAfe Near Connect ack error: 0x%lx", pPayload->response_result);
   }

   elite_msg_return_payload_buffer(&msg_pkt);

   MSG_1(MSG_SSID_DFLT, DBG_HIGH_PRIO,
         "vpm_connect_afe_tx(): Done connecting AFE Tx , sessionIndex(0x%lx)", session_id);

   return result;
}

ADSPResult vpm_connect_afe_far(vpm_session_object_t *p_session)
{
   ADSPResult           result = ADSP_EOK;

#if FARSRC
   afe_client_t         far_afe_client;
   elite_msg_any_t      msg_pkt;
   uint32_t             payload_size;
   vpm_dyn_svc_info_t   *p_dyn_svc = &p_session->dyn_svc_info;
   uint32_t             tx_sample_rate = p_session->active_set.system_config.tx_pp_sr;
   uint32_t             session_id;

   elite_msg_custom_afe_connect_t   *pPayload;

   session_id = p_session->self_apr_port;

   if ((VSS_IVOCPROC_PORT_ID_NONE == p_session->active_set.ec_ref_port_id) ||
       (VPM_TX_NONE == p_session->active_set.tx_topology_id))
   {
      MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
            "vpm_connect_afe_far(), No far port connected. sessionIndex(0x%x)",
            session_id);

      return ADSP_EOK;
   }

   MSG_3(MSG_SSID_DFLT, DBG_HIGH_PRIO,
         "vpm_connect_afe_far(): Sample Rate(%lu), farPort(0x%x), sessionIndex(0x%x)",
         tx_sample_rate, p_session->active_set.ec_ref_port_id, session_id);

   /***************************************************************************
    Connecting to afe far tx port
   ***************************************************************************/

   /* Zero init the client connect payload */
   memset(&far_afe_client, 0, sizeof(far_afe_client));

   far_afe_client.sample_rate       = tx_sample_rate;

   far_afe_client.data_path         = AFE_PP_OUT;
   far_afe_client.channel           = p_session->dyn_svc_info.ec_ref_num_channels;
   far_afe_client.bytes_per_channel = 2;
   far_afe_client.bit_width         = 16;
   far_afe_client.buf_size          = VPTX_DMA_BLOCK(tx_sample_rate); // 1ms

   /* 21 buffers for 20ms frame and 1ms to handler jitter. 
      This number should be greater that the maximum processing time of vptx */
   far_afe_client.num_buffers       = 21;
   far_afe_client.interleaved       = FALSE;

   far_afe_client.afe_client_handle                      = p_dyn_svc->tx_far;
   far_afe_client.cust_proc.resampler_type               = IIR_BASED_SRC;
   far_afe_client.cust_proc.subscribe_to_avt_drift       = FALSE;
   far_afe_client.cust_proc.is_pop_suppressor_required   = FALSE;

   payload_size = sizeof(elite_msg_custom_afe_connect_t);

   if (ADSP_EOK != (result = elite_msg_create_msg(&msg_pkt,
                                                  &payload_size,
                                                  ELITE_CUSTOM_MSG,
                                                  p_session->session_rsp_ctrl.p_rsp_queue,
                                                  NULL,
                                                  ADSP_EOK)))
   {
      MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
          "vpm_connect_afe_far(): FAILED to create EliteMsg for afe far connect !!");
      return result;
   }

   pPayload = (elite_msg_custom_afe_connect_t *)msg_pkt.pPayload;

   pPayload->sec_op_code   = ELITEMSG_CUSTOM_AFECONNECT_REQ;
   pPayload->afe_id        = p_session->active_set.ec_ref_port_id;
   pPayload->afe_client    = far_afe_client;

   /* Send the msg to device static service cmdQ */
   if (ADSP_FAILED(result = qurt_elite_queue_push_back(p_vpm_svc_global_obj->p_afe_svc_cmd_q,
                                                       (uint64_t *)&msg_pkt)))
   {
      MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
            "vpm_connect_afe_far(): Failed to push msg to afe svc cmd Q, result: 0x%lx", result);

      elite_msg_return_payload_buffer(&msg_pkt);
      return (result);
   }

   /* Blocking wait for the response */
   elite_svc_wait_for_ack(&msg_pkt);

   pPayload = (elite_msg_custom_afe_connect_t *)msg_pkt.pPayload;

   if (ADSP_EOK == pPayload->response_result)
   {
      /* Save handle to AFE service */
      p_dyn_svc->far_afe_handle = (elite_svc_handle_t *)pPayload->svc_handle_ptr;

      /* Save ptr to drift struct for tx port */
      p_dyn_svc->far_src_drift_ptr = (void *)pPayload->afe_drift_ptr;

      MSG_2(MSG_SSID_DFLT, DBG_HIGH_PRIO,
            "vpm_connect_afe_far(): AFE Far Tx Connected, sessionIndex(0x%lx), Handle(0x%lx)",
            session_id, p_dyn_svc->far_afe_handle);
   }
   else
   {
      p_dyn_svc->far_afe_handle = NULL;
      p_dyn_svc->far_src_drift_ptr = NULL;

      MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_connect_afe_far(): TxAfe Connect Ack error: 0x%lx",
            pPayload->response_result);
   }

   elite_msg_return_payload_buffer(&msg_pkt);

#endif /* #if FARSRC */


   MSG_1(MSG_SSID_DFLT, DBG_HIGH_PRIO,
         "vpm_connect_afe_far(): Done connecting AFE Far, sessionIndex(0x%lx)", session_id);

   return result;
}

ADSPResult vpm_connect_afe_rx(vpm_session_object_t *p_session)
{
   ADSPResult           result = ADSP_EOK;
   afe_client_t         rx_afe_client;
   elite_msg_any_t      msg_pkt;
   uint32_t             payload_size;
   vpm_dyn_svc_info_t   *p_dyn_svc = &p_session->dyn_svc_info;
   uint32_t             rx_sample_rate = p_session->active_set.system_config.rx_pp_sr;
   uint16_t             afe_rx_port_id = p_session->active_set.rx_port_id;
   uint32_t             session_id;

   elite_msg_custom_afe_connect_t   *pPayload;

   session_id = p_session->self_apr_port;

   MSG_3(MSG_SSID_DFLT, DBG_HIGH_PRIO,
         "vpm_connect_afe_rx(), Connecting to AFE Rx, sampRate(%lu), rxPort(0x%x), sessionIndex(0x%x)",
         rx_sample_rate, afe_rx_port_id, session_id);

   memset(&rx_afe_client, 0, sizeof(rx_afe_client));

   rx_afe_client.sample_rate        = rx_sample_rate;
   rx_afe_client.data_path          = AFE_RX_VOC_MIXER_IN;
   rx_afe_client.channel            = p_session->dyn_svc_info.rx_num_channels;;
   rx_afe_client.bytes_per_channel  = 2;
   rx_afe_client.bit_width          = 16;

   /* (samplingRate / 1000)*(20 + RX_PRE_BUF); // 21 ms */
   rx_afe_client.buf_size         = 200 * (rx_sample_rate / VOICE_NB_SAMPLING_RATE);
   rx_afe_client.num_buffers      = 5;
   rx_afe_client.interleaved      = FALSE;

   rx_afe_client.cust_proc.resampler_type               = IIR_BASED_SRC;
   rx_afe_client.cust_proc.subscribe_to_avt_drift       = FALSE;
   rx_afe_client.cust_proc.is_pop_suppressor_required   = TRUE;

   rx_afe_client.afe_dp_delay_ptr = &p_dyn_svc->rx_afe_delay;
   rx_afe_client.afe_dev_hw_delay_ptr = &p_dyn_svc->rx_afe_hw_delay;

   payload_size = sizeof(elite_msg_custom_afe_connect_t);

   if (ADSP_EOK != (result = elite_msg_create_msg(&msg_pkt,
                                                  &payload_size,
                                                  ELITE_CUSTOM_MSG,
                                                  p_session->session_rsp_ctrl.p_rsp_queue,
                                                  NULL,
                                                  ADSP_EOK)))
   {
      MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_connect_afe_rx(), FAILED to create EliteMsg for afe far connect !!");
      return result;
   }

   pPayload = (elite_msg_custom_afe_connect_t *)msg_pkt.pPayload;
   pPayload->sec_op_code   = ELITEMSG_CUSTOM_AFECONNECT_REQ;
   pPayload->afe_id        = afe_rx_port_id;
   pPayload->afe_client    = rx_afe_client;

   /* Send the msg to device static service cmdQ */
   if (ADSP_FAILED(result = qurt_elite_queue_push_back(p_vpm_svc_global_obj->p_afe_svc_cmd_q,
                                                       (uint64_t *)&msg_pkt)))
   {
      MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
            "vpm_connect_afe_rx(), Failed to push msg buf to AFE cmd Q, result: 0x%lx", result);

      elite_msg_return_payload_buffer(&msg_pkt);
      return (result);
   }

   /* Blocking wait for the response */
   elite_svc_wait_for_ack(&msg_pkt);

   pPayload = (elite_msg_custom_afe_connect_t *)msg_pkt.pPayload;

   if (ADSP_EOK == (result = pPayload->response_result))
   {
      /* Read rx_afe_handle for the dataQ handle */
      p_dyn_svc->rx_afe_handle = (elite_svc_handle_t *)pPayload->svc_handle_ptr;

      /* Save ptr to drift struct for rx port */
      p_dyn_svc->rx_afe_drift_ptr = (void *)pPayload->afe_drift_ptr;

      MSG_3(MSG_SSID_DFLT, DBG_HIGH_PRIO,
            "vpm_connect_afe_rx(): AFE Rx Connected, sessionIndex(0x%lx), pHandle(0x%x), pDrift(0x%x)",
            session_id, p_dyn_svc->rx_afe_handle,
            p_dyn_svc->rx_afe_drift_ptr);
   }
   else /* Failed */
   {
      p_dyn_svc->rx_afe_handle = NULL;

      MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_connect_afe_rx(): RxAfe Connect Ack result(0x%lx), sessionIndex(0x%lx)",
            pPayload->response_result, session_id);

      result = ADSP_EFAILED;
   }

   elite_msg_return_payload_buffer(&msg_pkt);

   return result;
}

ADSPResult vpm_connect_aferx_to_vprx(vpm_session_object_t *p_session,
                                     uint32_t sec_opcode)
{
   ADSPResult           result = ADSP_EOK;
   elite_msg_any_t      msg_pkt;
   uint32_t             payload_size;
   vpm_dyn_svc_info_t   *p_dyn_svc = &p_session->dyn_svc_info;
   uint32_t             session_id;

   elite_msg_custom_voc_svc_connect_type *p_payload_connect_cmd = NULL;

   session_id = p_session->self_apr_port;

   payload_size = sizeof(elite_msg_custom_voc_svc_connect_type);

   if (ADSP_EOK != (result = elite_msg_create_msg(&msg_pkt,
                                                  &payload_size,
                                                  ELITE_CUSTOM_MSG,
                                                  p_session->session_rsp_ctrl.p_rsp_queue,
                                                  NULL,
                                                  ADSP_EOK)))
   {
      MSG(MSG_SSID_DFLT, DBG_HIGH_PRIO,
          "vpm_connect_aferx_to_vprx(): FAILED to create EliteMsg");
      return result;
   }

   p_payload_connect_cmd                  = (elite_msg_custom_voc_svc_connect_type *)msg_pkt.pPayload;
   p_payload_connect_cmd->sec_opcode      = sec_opcode;
   p_payload_connect_cmd->svc_handle_ptr  = p_dyn_svc->rx_afe_handle;

   if (ADSP_EOK != (result = qurt_elite_queue_push_back(p_dyn_svc->rx->cmdQ,
                                                        (uint64_t *)&msg_pkt)))
   {
      MSG(MSG_SSID_DFLT, DBG_HIGH_PRIO,
          "vpm_connect_aferx_to_vprx(): FAILED to push connect msg to vpRx");

      elite_msg_return_payload_buffer(&msg_pkt);
      return result;
   }

   elite_svc_wait_for_ack(&msg_pkt);

   if (ADSP_EOK == (result = p_payload_connect_cmd->unResponseResult))
   {
      MSG_2(MSG_SSID_DFLT, DBG_HIGH_PRIO,
            "vpm_connect_aferx_to_vprx(): AFE Rx to vpRx connect(0)/disconnect(1) status(0x%lx), sessionIndex(0x%lx)",
            sec_opcode, session_id);
   }
   else
   {
      MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
            "vpm_connect_aferx_to_vprx(): AFE Rx to vpRx connect(0)/disconnect(1) status(0x%lx) failed, sessionIndex(0x%lx)",
            sec_opcode, session_id);
   }

   elite_msg_return_payload_buffer(&msg_pkt);

   return result;
}

ADSPResult vpm_control_mixer_ports(vpm_session_object_t *p_session,
                                   uint32_t vmx_port_id,
                                   uint32_t vmx_direction,
                                   uint32_t sec_opcode,
                                   qurt_elite_queue_t *p_vmx_cmd_q)
{
   ADSPResult           result = ADSP_EOK;
   vpm_dyn_svc_info_t   *p_dyn_svc = &p_session->dyn_svc_info;
   elite_msg_any_t      mixer_control_msg;
   uint32_t             payload_size;
   uint32_t             session_id;

   session_id = p_session->self_apr_port;

   payload_size = sizeof(elite_msg_custom_vmx_stop_run_t);

   if (ADSP_EOK != (result = elite_msg_create_msg(&mixer_control_msg,
                                                  &payload_size,
                                                  ELITE_CUSTOM_MSG,
                                                  p_session->session_rsp_ctrl.p_rsp_queue,
                                                  NULL,
                                                  ADSP_EOK)))
   {
      MSG(MSG_SSID_DFLT, DBG_HIGH_PRIO,
          "vpm_control_mixer_ports(): FAILED to create EliteMsg");
      return result;
   }

   elite_msg_custom_stop_t *payload = (elite_msg_custom_stop_t *)mixer_control_msg.pPayload;

   payload->unSecOpCode = sec_opcode;
   payload->unPortID = vmx_port_id;
   payload->unDirection = vmx_direction;

   MSG_4(MSG_SSID_DFLT, DBG_MED_PRIO,
         "vpm_control_mixer_ports(): VPM session (0x%lx) run/stop opcode(0x%x), mixer input port: %lu, direction: %lu",
         session_id, sec_opcode,
         payload->unPortID, vmx_direction);

   if (ADSP_FAILED(result = qurt_elite_queue_push_back(p_vmx_cmd_q,
                                                       (uint64_t *)&mixer_control_msg)))
   {
      MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO, "VCP: Failed to push msg buf to VMX cmdQ, result: %lu", result);

      elite_msg_return_payload_buffer(&mixer_control_msg);
      return result;
   }

   elite_svc_wait_for_ack(&mixer_control_msg);

   if (ADSP_EOK != (result = payload->unResponseResult))
   {
      MSG_3(MSG_SSID_DFLT, DBG_ERROR_PRIO,
            "vpm_control_mixer_ports(), Failed to run/stop opcode(0x%x), VMX port_id: %lu, direction: %lu",
            sec_opcode, payload->unPortID, vmx_direction);
   }
   else
   {
      MSG_4(MSG_SSID_DFLT, DBG_HIGH_PRIO,
            "vpm_control_mixer_ports(), Successfully run/stop opcode(0x%x), VMX port_id: %lu,"
            " direction: %lu, session_id: 0x%lx",
            sec_opcode, payload->unPortID, vmx_direction, session_id);
   }

   elite_msg_return_payload_buffer(&mixer_control_msg);

   return result;
}


ADSPResult vpm_send_run_msg_to_dyn_svc(vpm_session_object_t *p_session,
                                       uint32_t vpm_direction)
{
   ADSPResult           result = ADSP_EOK;
   uint32_t             payload_size = 0;
   elite_msg_any_t      msg_pkt;
   qurt_elite_queue_t   *p_dstn_q;
   uint32_t             session_id;

   session_id = p_session->self_apr_port;

   elite_msg_custom_voiceproc_run_payload_t *payload_ptr;

   payload_size = sizeof(elite_msg_custom_voiceproc_run_payload_t);

   if (ADSP_EOK != (result = elite_msg_create_msg(&msg_pkt,
                                                  &payload_size,
                                                  ELITE_CUSTOM_MSG,
                                                  p_session->session_rsp_ctrl.p_rsp_queue,
                                                  NULL,
                                                  ADSP_EOK)))
   {
      MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
            "vpm_send_run_msg_to_dyn_svc(): FAILED to create EliteMsg, result: 0x%lx", result);

      return result;
   }

   payload_ptr = (elite_msg_custom_voiceproc_run_payload_t *)msg_pkt.pPayload;
   payload_ptr->unSecOpCode = VOICEPROC_RUN_CMD;

   if (VSS_IVOCPROC_DIRECTION_TX == vpm_direction)
   {
      payload_ptr->afe_drift_ptr = p_session->dyn_svc_info.tx_afe_drift_ptr;
      payload_ptr->far_src_drift_ptr = p_session->dyn_svc_info.far_src_drift_ptr;

      p_dstn_q = p_session->dyn_svc_info.tx->cmdQ;
   }
   else /* VSS_IVOCPROC_DIRECTION_RX */
   {
      payload_ptr->afe_drift_ptr = p_session->dyn_svc_info.rx_afe_drift_ptr;
      payload_ptr->far_src_drift_ptr = NULL;

      p_dstn_q = p_session->dyn_svc_info.rx->cmdQ;
   }

   payload_ptr->session_thread_clock_mhz = 0;

   if (ADSP_EOK != (result = qurt_elite_queue_push_back(p_dstn_q,
                                                        (uint64_t *)&msg_pkt)))
   {
      MSG_2(MSG_SSID_DFLT, DBG_HIGH_PRIO,
            "vpm_send_run_msg_to_dyn_svc(), FAILED to push msg buf to dyn svc cmdQ,"
            " direction: %lu, result: 0x%lx",
            vpm_direction, result);

      elite_msg_return_payload_buffer(&msg_pkt);

      return result;
   }

   elite_svc_wait_for_ack(&msg_pkt);

   if (ADSP_EOK != (result = payload_ptr->unResponseResult))
   {
      MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
            "vpm_send_run_msg_to_dyn_svc(), FAILED to send run cmd to dynamic svc,"
            " direction: %lu, result: 0x%lx",
            vpm_direction, result);
   }
   else
   {
      MSG_3(MSG_SSID_DFLT, DBG_HIGH_PRIO,
            "vpm_send_run_msg_to_dyn_svc(): Successfully sent run cmd to dynamic svc,"
            " direction: %lu, session_id, 0x%lx, result: 0x%lx",
            vpm_direction, session_id, result);
   }

   elite_msg_return_payload_buffer(&msg_pkt);

   return result;
}

ADSPResult vpm_send_reconfig_msg_to_dyn_svc(vpm_session_object_t *p_session,
                                            uint32_t vpm_direction)
{
   ADSPResult                 result = ADSP_EOK;
   elite_msg_any_t            msg_pkt;
   qurt_elite_queue_t         *p_dstn_svc_cmd_q;
   vptx_create_params_v2_t    create_param_tx;
   vprx_create_params_t       create_param_rx;
   uint32_t                   session_id;
   uint16_t                   rx_port_for_vptx;

   elite_msg_voice_re_config_type *p_reinit_msg_payload;

   session_id = p_session->self_apr_port;

   uint32_t payload_size = sizeof(elite_msg_voice_re_config_type);

   if (ADSP_EOK != (result = elite_msg_create_msg(&msg_pkt,
                                                  &payload_size,
                                                  ELITE_CUSTOM_MSG,
                                                  p_session->session_rsp_ctrl.p_rsp_queue,
                                                  NULL,
                                                  ADSP_EOK)))
   {
      MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
          "vpm_send_reconfig_msg_to_dyn_svc(): FAILED to create EliteMsg");
      return result;
   }

   p_reinit_msg_payload = (elite_msg_voice_re_config_type *)msg_pkt.pPayload;
   p_reinit_msg_payload->sec_opcode    = VOICEPROC_RECONFIG_CMD;

   if (VSS_IVOCPROC_DIRECTION_RX == vpm_direction)
   {
      p_reinit_msg_payload->topology_id   = p_session->target_set.rx_topology_id;
      p_reinit_msg_payload->sampling_rate = p_session->target_set.rx_in_sample_rate;
      p_reinit_msg_payload->port_id_rx    = p_session->target_set.rx_port_id;
      memset(&create_param_rx, 0, sizeof(create_param_rx));

      if (ADSP_FAILED(result = vpm_update_additional_create_params(p_session,
                                                                  (void *)&create_param_rx,
                                                                  VSS_IVOCPROC_DIRECTION_RX)))
      {
        MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
              "vpm_send_reconfig_msg_to_dyn_svc(), Failed to get additional params for VpRx"
              " dyn svc creation, topo_id: 0x%lx",
              p_session->target_set.tx_topology_id);
        elite_msg_return_payload_buffer(&msg_pkt);
        return ADSP_EFAILED;
      }
      p_dstn_svc_cmd_q = p_session->dyn_svc_info.rx->cmdQ;
      p_reinit_msg_payload->num_channels = create_param_rx.num_channels;
      
      mmstd_memcpy( &p_reinit_msg_payload->channel_map_near[0], sizeof(p_reinit_msg_payload->channel_map_near), 
                    &create_param_rx.channel_map[0],  sizeof(create_param_rx.channel_map) );

      if (ADSP_FAILED(result = qurt_elite_queue_push_back(p_dstn_svc_cmd_q,
                                                          (uint64_t *)&msg_pkt)))
      {
         MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_send_reconfig_msg_to_dyn_svc(), Failed to push msg buf to Rx dyn svc cmdQ, result: 0x%lx", result);

         elite_msg_return_payload_buffer(&msg_pkt);
         return (result);
      }
   }
   else if( VSS_IVOCPROC_DIRECTION_TX == vpm_direction )
   {
      rx_port_for_vptx = vpm_get_ec_ref_port(p_session);

      p_reinit_msg_payload->topology_id      = p_session->target_set.tx_topology_id;
      p_reinit_msg_payload->sampling_rate    = p_session->target_set.system_config.tx_pp_sr;
      p_reinit_msg_payload->port_id_tx       = p_session->target_set.tx_port_id;
      p_reinit_msg_payload->port_id_rx       = rx_port_for_vptx;
      p_reinit_msg_payload->num_ref_channels = 1;
      p_dstn_svc_cmd_q = p_session->dyn_svc_info.tx->cmdQ;
      memset(&create_param_tx, 0, sizeof(create_param_tx));

      if (ADSP_EOK != (result = vpm_update_additional_create_params(p_session,
                                                                    (void *)&create_param_tx,
                                                                    vpm_direction)))
      {
         MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_send_reconfig_msg_to_dyn_svc(), Failed to update additional create params, topo:0x%lx",
               p_session->target_set.tx_topology_id);

         elite_msg_return_payload_buffer(&msg_pkt);
         return ADSP_EFAILED;
      }

      p_reinit_msg_payload->num_channels = create_param_tx.num_mics;
      p_reinit_msg_payload->num_ref_channels = create_param_tx.num_ref_channels;
      mmstd_memcpy( &p_reinit_msg_payload->channel_map_far[0], sizeof(p_reinit_msg_payload->channel_map_far), 
                    &create_param_tx.channel_map_far[0],  sizeof(create_param_tx.channel_map_far) );

      mmstd_memcpy( &p_reinit_msg_payload->channel_map_near[0], sizeof(p_reinit_msg_payload->channel_map_near), 
                    &create_param_tx.channel_map_near[0],  sizeof(create_param_tx.channel_map_near) );

      if (ADSP_FAILED(result = qurt_elite_queue_push_back(p_dstn_svc_cmd_q,
                                                          (uint64_t *)&msg_pkt)))
      {
         MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_send_reconfig_msg_to_dyn_svc(), Failed to push msg buf to Tx dyn svc cmdQ, result: 0x%lx", result);

         elite_msg_return_payload_buffer(&msg_pkt);
         return (result);
      }

   }

   (void)elite_svc_wait_for_ack(&msg_pkt);

   if (ADSP_EOK != (result = p_reinit_msg_payload->unResponseResult))
   {
      MSG_3(MSG_SSID_DFLT, DBG_HIGH_PRIO,
            "vpm_send_reconfig_msg_to_dyn_svc(), "
            "dyn svc (direction: %lu) reconfigure failed with ack result: 0x%lx "
            "session_id: 0x%lx",
            vpm_direction, result, session_id);
   }
   else
   {
      if( VSS_IVOCPROC_DIRECTION_RX == vpm_direction )
      {
         MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO,
               "vpm_send_reconfig_msg_to_dyn_svc(), VpRx reconfigure success, session_id: 0x%lx",
               session_id);

         p_session->dyn_svc_info.rx_num_channels = p_reinit_msg_payload->num_channels;
      }
      else if( VSS_IVOCPROC_DIRECTION_TX == vpm_direction )
      {
         MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO,
               "vpm_send_reconfig_msg_to_dyn_svc(), VpTx reconfigure success, session_id: 0x%lx",
               session_id);

         p_session->dyn_svc_info.tx_num_channels = p_reinit_msg_payload->num_channels;
         p_session->dyn_svc_info.ec_ref_num_channels = create_param_tx.num_ref_channels;
      }
   }

   elite_msg_return_payload_buffer(&msg_pkt);

   return result;
}

ADSPResult vpm_set_get_param_to_dyn_svc(vpm_session_object_t *session_obj,
                                           void *payload_address,
                                           uint32_t payload_size,
                                           uint32_t opcode,
                                           uint32_t version )
{
   ADSPResult              result = ADSP_EOK;
   ADSPResult              result_tx = ADSP_EOK, result_rx = ADSP_EOK;
   uint32_t                msg_payload_size;
   elite_msg_any_t         msg_pkt_tx, msg_pkt_rx;
   elite_msg_param_cal_t   *p_msg_payload;
   uint32_t                session_id;
   uint32_t log_channel = 0;

   msg_payload_size = sizeof(elite_msg_param_cal_t);

   session_id = session_obj->self_apr_port;

   if (session_obj->dyn_svc_info.tx)
   {
      if (ADSP_EOK == (result_tx = elite_msg_create_msg(&msg_pkt_tx,
                                                        &msg_payload_size,
                                                        opcode,
                                                        session_obj->session_rsp_ctrl.p_rsp_queue,
                                                        0,
                                                        ADSP_EOK)))
      {
         p_msg_payload = (elite_msg_param_cal_t *)msg_pkt_tx.pPayload;

         p_msg_payload->pnParamData = (int32_t *)payload_address;
         p_msg_payload->unSize = payload_size;
         if ( 0 == version )
         {
            log_channel = VOICE_LOG_CHAN_VPM_SET_PARAM;
            p_msg_payload->unParamId = ELITEMSG_PARAM_ID_CAL;

         }
         else
         {
            log_channel = VOICE_LOG_CHAN_VPM_SET_PARAM_V2;
            p_msg_payload->unParamId = ELITEMSG_PARAM_ID_CAL_V2;
         }

         // MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO,"VCP: Dispatch SetParams to Tx/Rx, Apr packet addr = 0x%x!!\n",(uint32_t)pAprPacket);

         if (ADSP_EOK != (result_tx = qurt_elite_queue_push_back(session_obj->dyn_svc_info.tx->cmdQ,
                                                                 (uint64_t *)&msg_pkt_tx)))
         {
            MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_set_get_param_v2_to_dyn_svc(): FAILED to push msg buf to vpTx cmdQ, result: 0x%lx",
                  result_tx);

            elite_msg_return_payload_buffer(&msg_pkt_tx);
         }
         else
         {
            MSG_2(MSG_SSID_DFLT, DBG_MED_PRIO,
                  "vpm_set_get_param_v2_to_dyn_svc(): Successfully sent to vpTx, opcode: 0x%lx, session_id: 0x%lx",
                  opcode, session_id);

            session_obj->session_rsp_ctrl.num_cmd_issued++;
         }
      }
      else
      {
         MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO, "VCP: FAILED to create EliteMsg for vpTx dyn svc setparam cmd!!");
      }
   }

   if (session_obj->dyn_svc_info.rx)
   {
      if (ADSP_EOK == (result_rx = elite_msg_create_msg(&msg_pkt_rx,
                                                        &msg_payload_size,
                                                        opcode,
                                                        session_obj->session_rsp_ctrl.p_rsp_queue,
                                                        0,
                                                        ADSP_EOK)))
      {
         p_msg_payload = (elite_msg_param_cal_t *)msg_pkt_rx.pPayload;

         p_msg_payload->pnParamData = (int32_t *)payload_address;
         p_msg_payload->unSize = payload_size;
         if ( 0 == version )
         {
            p_msg_payload->unParamId = ELITEMSG_PARAM_ID_CAL;
         }
         else
         {
            p_msg_payload->unParamId = ELITEMSG_PARAM_ID_CAL_V2;
         }

         // MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO,"VCP: Dispatch SetParams to Tx/Rx, Apr packet addr = 0x%x!!\n",(uint32_t)pAprPacket);

         if (ADSP_EOK != (result_rx = qurt_elite_queue_push_back(session_obj->dyn_svc_info.rx->cmdQ,
                                                                 (uint64_t *)&msg_pkt_rx)))
         {
            MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                "vpm_set_get_param_v2_to_dyn_svc(): FAILED to push msg buf to vpRx cmdQ");

            elite_msg_return_payload_buffer(&msg_pkt_rx);
         }
         else
         {
            MSG_2(MSG_SSID_DFLT, DBG_MED_PRIO,
                  "vpm_set_get_param_v2_to_dyn_svc(): Successfully sent to vpRx, opcode: 0x%lx, session_id: 0x%lx",
                  opcode, session_id);

            session_obj->session_rsp_ctrl.num_cmd_issued++;
         }
      }
      else
      {
         MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
             "vpm_set_get_param_v2_to_dyn_svc(), FAILED to create EliteMsg for vpRx dyn svc setparam cmd!!");
      }
   }

   /* If both the set param failed */
   if (result_rx && result_tx)
   {
      result = ADSP_EFAILED;
   }
   else if (ELITE_CMD_SET_PARAM == opcode)
   {
#if defined(__qdsp6__) && !defined(SIM)

      int8_t *bufptr[4] = { (int8_t *)payload_address, NULL, NULL, NULL };

      voice_log_buffer(bufptr,
                       log_channel,
                       session_obj->self_apr_port /* Session Id */,
                       qurt_elite_timer_get_time(),
                       VOICE_LOG_DATA_FORMAT_PCM_MONO,
                       1,   /* dummy value */
                       payload_size,
                       NULL);
#endif
   }

   return result;
}

ADSPResult vpm_disconnect_afe(vpm_session_object_t *p_session,
                              uint32_t afe_port_id,
                              elite_svc_handle_t *h_afe,
                              uint32_t sec_opcode)
{

   ADSPResult           result = ADSP_EOK;
   elite_msg_any_t      msg_pkt;
   uint32_t             payload_size;
   uint32_t             session_id;

   elite_msg_custom_afe_connect_t   *p_msg_payload;

   session_id = p_session->self_apr_port;

   MSG_2(MSG_SSID_DFLT, DBG_HIGH_PRIO,
         "vpm_disconnect_afe(): Disconnecting to AFE port_id (0x%lx), sessionIndex(0x%lx)",
         afe_port_id, session_id);

   payload_size = sizeof(elite_msg_custom_afe_connect_t);

   if (ADSP_EOK != (result = elite_msg_create_msg(&msg_pkt,
                                                  &payload_size,
                                                  ELITE_CUSTOM_MSG,
                                                  p_session->session_rsp_ctrl.p_rsp_queue,
                                                  NULL,
                                                  ADSP_EOK)))
   {
      MSG_1(MSG_SSID_DFLT, DBG_HIGH_PRIO,
            "vpm_disconnect_afe(): FAILED to create EliteMsg for afe disconnect, result: 0x%lx", result);

      return result;
   }

   p_msg_payload = (elite_msg_custom_afe_connect_t *)msg_pkt.pPayload;

   /* First disable the client */
   p_msg_payload->sec_op_code     = sec_opcode;
   p_msg_payload->afe_id          = afe_port_id;
   p_msg_payload->svc_handle_ptr  = h_afe;

   /* Send the msg to device static service cmdQ. */
   if (ADSP_FAILED(result = qurt_elite_queue_push_back(p_vpm_svc_global_obj->p_afe_svc_cmd_q,
                                                       (uint64_t *)&msg_pkt)))
   {
      MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
            "vpm_disconnect_afe(): Failed to push disconnect msg buf to AFE svc cmd Q, result: 0x%lx", result);

      elite_msg_return_payload_buffer(&msg_pkt);

      return result;
   }

   /* Wait for ACK */
   elite_svc_wait_for_ack(&msg_pkt);

   p_msg_payload = (elite_msg_custom_afe_connect_t *)msg_pkt.pPayload;

   if (ADSP_EOK != p_msg_payload->response_result)
   {
      MSG_4(MSG_SSID_DFLT, DBG_ERROR_PRIO,
            "vpm_disconnect_afe(): AFE disconnect/disable (0x%x) FAILED,"
            " afe_port_id: 0x%x, result: %lu, sessionIndex: 0x%x",
            sec_opcode, afe_port_id,
            p_msg_payload->response_result, session_id);
   }
   else
   {
      MSG_3(MSG_SSID_DFLT, DBG_HIGH_PRIO,
            "vpm_disconnect_afe(), AFE disconnect/disable (0x%x) SUCCESS, afe_port_id: 0x%x, session_id: 0x%lx",
            sec_opcode, afe_port_id, session_id);
   }

   elite_msg_return_payload_buffer(&msg_pkt);

   return result;
}

ADSPResult vpm_send_disconnect_marker_to_afe(vpm_session_object_t *session_obj)
{
   ADSPResult        result = ADSP_EOK;
   elite_msg_any_t   msg_pkt;
   uint32_t          payload_size;
   uint16_t          afe_port_id;
   uint32_t session_id;

   elite_msg_custom_afeclient_disconnect_marker_t   *p_msg_payload;

   session_id = session_obj->self_apr_port;

   afe_port_id = session_obj->active_set.rx_port_id;

   MSG_2(MSG_SSID_DFLT, DBG_HIGH_PRIO,
         "Sending disconnect marker to AFE Rx rxPort(0x%lx),sessionIndex(0x%lx)",
         afe_port_id, session_id);

   payload_size = sizeof(elite_msg_custom_afeclient_disconnect_marker_t);

   if (ADSP_EOK != (result = elite_msg_create_msg(&msg_pkt,
                                                  &payload_size,
                                                  ELITE_CUSTOM_MSG,
                                                  session_obj->session_rsp_ctrl.p_rsp_queue,
                                                  0,
                                                  ADSP_EOK)))
   {
      MSG(MSG_SSID_DFLT, DBG_HIGH_PRIO,
          "vpm_send_disconnect_marker_to_afe(), FAILED to create EliteMsg");
      return result;
   }

   p_msg_payload = (elite_msg_custom_afeclient_disconnect_marker_t *)msg_pkt.pPayload;
   p_msg_payload->sec_op_code      = ELITEMSG_CUSTOM_AFECLIENT_DISCONNECT_MARKER;
   p_msg_payload->afe_id           = afe_port_id;
   p_msg_payload->svc_handle_ptr   = session_obj->dyn_svc_info.rx_afe_handle;

   /* Send the msg to device static service cmdQ */
   if (ADSP_FAILED(result = qurt_elite_queue_push_back(p_vpm_svc_global_obj->p_afe_svc_cmd_q,
                                                       (uint64_t *)&msg_pkt)))
   {
      MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
            "vpm_send_disconnect_marker_to_afe(): Failed to push msg buf to AFE svc cmd Q, result: 0x%lx",
            result);

      elite_msg_return_payload_buffer(&msg_pkt);

      return result;
   }

   /* Wait for ACK */
   elite_svc_wait_for_ack(&msg_pkt);

   p_msg_payload = (elite_msg_custom_afeclient_disconnect_marker_t *)msg_pkt.pPayload;

   if (ADSP_EOK != (result = p_msg_payload->response_result))
   {
      MSG_3(MSG_SSID_DFLT, DBG_ERROR_PRIO,
            "vpm_send_disconnect_marker_to_afe(): AFE Ack Failed"
            " rxPort(0x%lx),sessionIndex(0x%lx), result(0x%lx)",
            afe_port_id, session_id, result);
   }

   /* Return msg buffer */
   elite_msg_return_payload_buffer(&msg_pkt);

   return result;
}

ADSPResult vpm_action_disable_vocproc(vpm_session_object_t *session_obj)
{

   ADSPResult           result = ADSP_EOK;
   vpm_dyn_svc_info_t   *p_dyn_svc = &session_obj->dyn_svc_info;

   /* Send client disable to AFE Tx port */
   if (p_dyn_svc->tx)
   {
      result = vpm_disconnect_afe(session_obj,
                                  session_obj->active_set.tx_port_id,
                                  session_obj->dyn_svc_info.tx_afe_handle,
                                  ELITEMSG_CUSTOM_AFECLIENTDISABLE);

      /* Reinitialize delay to zero */
      p_dyn_svc->tx_afe_delay = 0;
      p_dyn_svc->tx_afe_hw_delay = 0;
   }

   /* Send client disable to AFE Far port */
   if ((VSS_IVOCPROC_PORT_ID_NONE != session_obj->active_set.ec_ref_port_id) &&
       (VPM_TX_NONE != session_obj->active_set.tx_topology_id))
   {
      result |= vpm_disconnect_afe(session_obj,
                                   session_obj->active_set.ec_ref_port_id,
                                   session_obj->dyn_svc_info.far_afe_handle,
                                   ELITEMSG_CUSTOM_AFECLIENTDISABLE);
   }

   /* Send message to stop the vpTx/Rx dyn svc */
   if (p_dyn_svc->tx)
   {
      result |= voice_custom_msg_send(session_obj->session_rsp_ctrl.p_rsp_queue,
                                      p_dyn_svc->tx->cmdQ,
                                      VOICEPROC_STOP_CMD,
                                      NULL, TRUE);
   }
   if (p_dyn_svc->rx)
   {
      result |= voice_custom_msg_send(session_obj->session_rsp_ctrl.p_rsp_queue,
                                      p_dyn_svc->rx->cmdQ,
                                      VOICEPROC_STOP_CMD,
                                      NULL, TRUE);
   }

   /* Send stop command to VMX input port */
   if (session_obj->dyn_svc_info.mixer_in_port)
   {
      if (ADSP_EOK != (result |= vpm_control_mixer_ports(session_obj,
                                                         session_obj->dyn_svc_info.mixer_in_port->inport_id,
                                                         VMX_INPUT_DIRECTION,
                                                         ELITEMSG_CUSTOM_VMX_STOP,
                                                         txVoiceMatrix->cmdQ)))
      {
         MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_action_disable_vocproc(): Failed to run VMX inport_id: %lu, result: 0x%lx",
               session_obj->dyn_svc_info.mixer_in_port, result);

         return result;
      }
   }

   /* Send stop command to VMX output port */
   if (session_obj->dyn_svc_info.mixer_out_port)
   {
      if (ADSP_EOK != (result |= vpm_control_mixer_ports(session_obj,
                                                         session_obj->dyn_svc_info.mixer_out_port->outport_id,
                                                         VMX_OUTPUT_DIRECTION,
                                                         ELITEMSG_CUSTOM_VMX_STOP,
                                                         rxVoiceMatrix->cmdQ)))
      {
         MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_action_disable_vocproc(): Failed to run VMX outport_id: %lu, result: 0x%lx",
               session_obj->dyn_svc_info.mixer_out_port, result);

         return result;
      }
   }

   if (p_dyn_svc->rx)
   {
      /*
       * the following code is to inform AFE that voice is about to disconnect. With this information
       * AFE would ramp down the voice data before pumping out. voice would sleep for 2 ms
       * to allow the rampdown process to complete
       */
      avtimer_open_param_t       open_param;
      avtimer_drv_handle_t       avt_drv;
      open_param.client_name = (char *)"VPM";
      open_param.flag = 0;

      if (ADSP_EOK != (result = avtimer_drv_hw_open(&avt_drv, &open_param)))
      {
         MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
             "vpm_action_disable_vocproc(): Failed to Open AVTimer Driver for VPM_SLEEP");
      }
      else
      {
         result |= vpm_send_disconnect_marker_to_afe(session_obj);

         MSG(MSG_SSID_DFLT, DBG_HIGH_PRIO,
             "vpm_action_disable_vocproc(): VPM going to sleep for 2ms");

         avtimer_drv_sleep(2000);

         MSG(MSG_SSID_DFLT, DBG_HIGH_PRIO,
             "vpm_action_disable_vocproc(): VPM coming out of sleep");

         if (ADSP_EOK != (result = avtimer_drv_hw_close(avt_drv)))
         {
            MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                "vpm_action_disable_vocproc(): Failed to close AVTimer Driver for VPM_SLEEP");
         }
      }

      /* Rx is stopped so we can disconnect downstream */
      result |= vpm_disconnect_afe(session_obj,
                                   session_obj->active_set.rx_port_id,
                                   session_obj->dyn_svc_info.rx_afe_handle,
                                   ELITEMSG_CUSTOM_AFEDISCONNECT_REQ);

      /* Disconnect AFE Rx and vpRx */
      result |= vpm_connect_aferx_to_vprx(session_obj, VOICEPROC_DISCONNECT_DWN_STREAM_CMD);

      /* Reinitialize delay to zero */
      p_dyn_svc->rx_afe_delay = 0;
      p_dyn_svc->rx_afe_hw_delay = 0;

      MSG_2(MSG_SSID_DFLT, DBG_HIGH_PRIO,
            "vpm_action_disable_vocproc(): AFE Rx Disconnected, sessionIndex(0x%lx), pHandle(0x%x)",
            session_obj->self_apr_port, p_dyn_svc->rx_afe_handle);
   }

   /* Now it's safe to send ELITEMSG_CUSTOM_AFEDISCONNECT_REQ message */
   if (p_dyn_svc->tx)
   {
      /* Send client disable to AFE Tx port */

      result |= vpm_disconnect_afe(session_obj,
                                   session_obj->active_set.tx_port_id,
                                   session_obj->dyn_svc_info.tx_afe_handle,
                                   ELITEMSG_CUSTOM_AFEDISCONNECT_REQ);

      /* Reinitialize delay to zero */
      p_dyn_svc->tx_afe_delay = 0;
      p_dyn_svc->tx_afe_hw_delay = 0;


      /* Send client disable to AFE Far port */
      if ((VSS_IVOCPROC_PORT_ID_NONE != session_obj->active_set.ec_ref_port_id) &&
          (VPM_TX_NONE != session_obj->active_set.tx_topology_id))
      {
         result |= vpm_disconnect_afe(session_obj,
                                      session_obj->active_set.ec_ref_port_id,
                                      session_obj->dyn_svc_info.far_afe_handle,
                                      ELITEMSG_CUSTOM_AFEDISCONNECT_REQ);
      }
   }

   MSG_1(MSG_SSID_DFLT, DBG_HIGH_PRIO,
         "vpm_action_disable_vocproc(): Completed, result: 0x%lx", result);

   return result;
}

ADSPResult vpm_set_param_v3_to_dyn_svc(vpm_session_object_t *session_obj,
                                           uint32_t cal_handle)
{
   ADSPResult              result = ADSP_EOK;
   ADSPResult              result_tx = ADSP_EOK, result_rx = ADSP_EOK;
   uint32_t                msg_payload_size;
   elite_msg_any_t         msg_pkt_tx, msg_pkt_rx;
   uint32_t                session_id;

   elite_msg_custom_set_param_v3_type   *p_msg_payload;

   session_id = session_obj->self_apr_port;

   msg_payload_size = sizeof(elite_msg_custom_set_param_v3_type);

   if (session_obj->dyn_svc_info.tx)
   {
      if (ADSP_EOK == (result_tx = elite_msg_create_msg(&msg_pkt_tx,
                                                        &msg_payload_size,
                                                        ELITE_CUSTOM_MSG,
                                                        session_obj->session_rsp_ctrl.p_rsp_queue,
                                                        0,
                                                        ADSP_EOK)))
      {
         p_msg_payload = (elite_msg_custom_set_param_v3_type *)msg_pkt_tx.pPayload;

         p_msg_payload->sec_opcode = VOICEPROC_SET_PARAM_V3_CMD;
         p_msg_payload->cal_handle = cal_handle;

         // MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO,"VCP: Dispatch SetParams to Tx/Rx, Apr packet addr = 0x%x!!\n",(uint32_t)pAprPacket);

         if (ADSP_EOK != (result_tx = qurt_elite_queue_push_back(session_obj->dyn_svc_info.tx->cmdQ,
                                                                 (uint64_t *)&msg_pkt_tx)))
         {
            MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_set_param_v3_to_dyn_svc(), FAILED to push msg buf to vpTx cmdQ, result: 0x%lx",
                  result_tx);

            elite_msg_return_payload_buffer(&msg_pkt_tx);
         }
         else
         {
            MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO,
                  "vpm_set_param_v3_to_dyn_svc(), Send set param cmd to vpTx cmdQ, session_id: 0x%lx",
                  session_id);

            session_obj->session_rsp_ctrl.num_cmd_issued++;
         }
      }
      else
      {
         MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
             "vpm_set_param_v3_to_dyn_svc(), FAILED to create EliteMsg for vpTx cmdQ");
      }
   }

   if (session_obj->dyn_svc_info.rx)
   {
      if (ADSP_EOK == (result_rx = elite_msg_create_msg(&msg_pkt_rx,
                                                        &msg_payload_size,
                                                        ELITE_CUSTOM_MSG,
                                                        session_obj->session_rsp_ctrl.p_rsp_queue,
                                                        0,
                                                        ADSP_EOK)))
      {
         p_msg_payload = (elite_msg_custom_set_param_v3_type *)msg_pkt_rx.pPayload;

         p_msg_payload->sec_opcode = VOICEPROC_SET_PARAM_V3_CMD;
         p_msg_payload->cal_handle = cal_handle;

         // MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO,"VCP: Dispatch SetParams to Tx/Rx, Apr packet addr = 0x%x!!\n",(uint32_t)pAprPacket);

         if (ADSP_EOK != (result_rx = qurt_elite_queue_push_back(session_obj->dyn_svc_info.rx->cmdQ,
                                                                 (uint64_t *)&msg_pkt_rx)))
         {
            MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                "vpm_set_param_v3_to_dyn_svc(): FAILED to push msg buf to vpRx cmdQ");

            elite_msg_return_payload_buffer(&msg_pkt_rx);
         }
         else
         {
            MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO,
                  "vpm_set_param_v3_to_dyn_svc(), Send set param cmd to vpRx cmdQ, session_id: 0x%lx",
                  session_id);

            session_obj->session_rsp_ctrl.num_cmd_issued++;
         }
      }
      else
      {
         MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
             "vpm_set_param_v3_to_dyn_svc(): FAILED to create EliteMsg");
      }
   }

#if defined(__qdsp6__) && !defined(SIM)
   /* Log the cal data */
   voice_log_cal_data(cal_handle, VOICE_LOG_CHAN_VPM_SET_PARAM_V2, session_obj->self_apr_port);
#endif

   /* If both the set param failed */
   if (result_rx && result_tx)
   {
      result = ADSP_EFAILED;
   }

   return result;
}


ADSPResult vpm_send_vcpm_cfg_msg_to_dyn_svc(vpm_session_object_t *session_obj,
                                            uint32_t vpm_mem_handle,
                                            elite_apr_packet_t *p_apr_pkt)
{
   ADSPResult        result = ADSP_EOK;
   elite_msg_any_t   msg_pkt_tx, msg_pkt_rx;
   uint32_t          payload_size;

   elite_msg_custom_voc_config_host_pcm_type    *p_msg_payload = NULL;

   payload_size = sizeof(elite_msg_custom_voc_config_host_pcm_type);

   /* If vpTx present */
   if (session_obj->dyn_svc_info.tx)
   {
      if (ADSP_EOK == (result = elite_msg_create_msg(&msg_pkt_tx,
                                                     &payload_size,
                                                     ELITE_CUSTOM_MSG,
                                                     session_obj->session_rsp_ctrl.p_rsp_queue,
                                                     NULL,
                                                     ADSP_EOK)))
      {
         p_msg_payload = (elite_msg_custom_voc_config_host_pcm_type *)msg_pkt_tx.pPayload;

         p_msg_payload->sec_opcode        = VOICEPROC_CONFIG_HOST_PCM;
         p_msg_payload->apr_handle        = vpm_mem_handle;
         p_msg_payload->apr_packet_ptr    = p_apr_pkt;

         if (ADSP_EOK == (result = qurt_elite_queue_push_back(session_obj->dyn_svc_info.tx->cmdQ,
                                                              (uint64_t *)&msg_pkt_tx)))
         {
            MSG(MSG_SSID_DFLT, DBG_MED_PRIO,
                "vpm_send_vcpm_cfg_msg_to_dyn_svc(): Sent config Host PCM cmd to vpTx");

            session_obj->session_rsp_ctrl.num_cmd_issued++;
         }
         else
         {
            MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                "vpm_send_vcpm_cfg_msg_to_dyn_svc(): FAILED to config Host PCM on vpTx");

            elite_msg_return_payload_buffer(&msg_pkt_tx);
         }

      }
      else
      {
         MSG(MSG_SSID_DFLT, DBG_HIGH_PRIO,
             "vpm_send_vcpm_cfg_msg_to_dyn_svc(): FAILED to create EliteMsg for vpTx");
      }
   }

   /* If vpRx present */
   if (session_obj->dyn_svc_info.rx)
   {
      if (ADSP_EOK == (result = elite_msg_create_msg(&msg_pkt_rx,
                                                     &payload_size,
                                                     ELITE_CUSTOM_MSG,
                                                     session_obj->session_rsp_ctrl.p_rsp_queue,
                                                     NULL,
                                                     ADSP_EOK)))
      {
         p_msg_payload = (elite_msg_custom_voc_config_host_pcm_type *)msg_pkt_rx.pPayload;

         p_msg_payload->sec_opcode        = VOICEPROC_CONFIG_HOST_PCM;
         p_msg_payload->apr_handle        = vpm_mem_handle;
         p_msg_payload->apr_packet_ptr    = p_apr_pkt;

         if (ADSP_EOK == (result = qurt_elite_queue_push_back(session_obj->dyn_svc_info.rx->cmdQ,
                                                              (uint64_t *)&msg_pkt_rx)))
         {
            MSG(MSG_SSID_DFLT, DBG_MED_PRIO,
                "vpm_send_vcpm_cfg_msg_to_dyn_svc(): Sent config Host PCM cmd to vpRx");

            session_obj->session_rsp_ctrl.num_cmd_issued++;
         }
         else
         {
            MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                "vpm_send_vcpm_cfg_msg_to_dyn_svc(): FAILED to config Host PCM on vpRx");

            elite_msg_return_payload_buffer(&msg_pkt_rx);
         }

      }
      else
      {
         MSG(MSG_SSID_DFLT, DBG_HIGH_PRIO,
             "vpm_send_vcpm_cfg_msg_to_dyn_svc(): FAILED to create EliteMsg for vpRx");
      }
   }

   /* Check if at least 1 command was issued */
   if (!session_obj->session_rsp_ctrl.num_cmd_issued)
   {
      result = ADSP_EFAILED;
   }

   return result;
}

ADSPResult vpm_action_set_mute(vpm_session_object_t *session_obj,
                               uint32_t mute_ramp_duration,
                               uint32_t vpm_direction)
{
   ADSPResult                 result = ADSP_EOK;
   voice_set_soft_mute_v2_t   set_soft_mute_args;
   uint32_t                   session_id;

   session_id = session_obj->self_apr_port;

   /* Initialize the common arguments between Tx and Rx */
   set_soft_mute_args.ramp_duration = (int16_t)mute_ramp_duration;
   set_soft_mute_args.reserved = 0;

   /* Init the response control params */
   vpm_set_cmd_rsp_pending_status(session_obj, TRUE);

   /* If Tx dynamic service present */
   if ((session_obj->dyn_svc_info.tx) &&
       ((VOICE_SET_MUTE_TX_ONLY == vpm_direction) ||
        (VOICE_SET_MUTE_TX_AND_RX == vpm_direction)))
   {
      /* Direction is don't care */
      set_soft_mute_args.direction = VOICE_SET_MUTE_TX_ONLY;

      set_soft_mute_args.mute = session_obj->target_set.client_tx_mute;

      if (ADSP_EOK != (result = vpm_send_mute_cmd_to_dyn_svc(&set_soft_mute_args,
                                                             session_obj->session_rsp_ctrl.p_rsp_queue,
                                                             session_obj->dyn_svc_info.tx->cmdQ)))
      {
         MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
             "vpm_action_set_mute(): Failed to send sent mute cmd to VpTx");
      }
      else
      {
         MSG_3(MSG_SSID_DFLT, DBG_MED_PRIO,
               "vpm_action_set_mute(): Sucessfully sent mute cmd to VpTx, "
               "mute(on/off): %u, ramp_duration: %u, session_id: 0x%lx",
               set_soft_mute_args.mute,
               set_soft_mute_args.ramp_duration,
               session_id);

         /* Increment the number of cmd issued counter */
         session_obj->session_rsp_ctrl.num_cmd_issued++;
      }
   }

   /* If Rx dynamic service present */
   if ((session_obj->dyn_svc_info.rx) &&
       ((VOICE_SET_MUTE_RX_ONLY == vpm_direction) ||
        (VOICE_SET_MUTE_TX_AND_RX == vpm_direction)))
   {
      /* Direction is don't care */
      set_soft_mute_args.direction = VOICE_SET_MUTE_RX_ONLY;

      set_soft_mute_args.mute = session_obj->target_set.client_rx_mute;

      if (ADSP_EOK != (result = vpm_send_mute_cmd_to_dyn_svc(&set_soft_mute_args,
                                                             session_obj->session_rsp_ctrl.p_rsp_queue,
                                                             session_obj->dyn_svc_info.rx->cmdQ)))
      {
         MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
             "vpm_action_set_mute(): Failed to send mute cmd to VpRx");
      }
      else
      {
         MSG_3(MSG_SSID_DFLT, DBG_MED_PRIO,
               "vpm_action_set_mute(): Sucessfully sent mute cmd to VpRx, "
               "mute(on/off): %u, ramp_duration: %u, session_id: 0x%lx",
               set_soft_mute_args.mute,
               set_soft_mute_args.ramp_duration,
               session_id);

         /* Increment the number of cmd issued counter */
         session_obj->session_rsp_ctrl.num_cmd_issued++;
      }
   }

   /* Check if at least 1 command was issued */
   if (!session_obj->session_rsp_ctrl.num_cmd_issued)
   {
      /* Clear the response pending status  */
      vpm_set_cmd_rsp_pending_status(session_obj, FALSE);

      result = ADSP_EFAILED;
   }
   else
   {
      result = ADSP_EOK;
   }

   return result;
}

ADSPResult vpm_send_hdvoice_enable_disable_msg(vpm_session_object_t *session_object,
                                               bool_t is_enable)
{
   ADSPResult                    result = ADSP_EOK;
   uint32_t                      payload_size = 0;
   uint32_t                      feature_id;
   uint32_t                      session_id;
   elite_msg_any_t               msg_rx_t;
   elite_msg_param_cal_t         *payload_ptr = NULL;

   vpm_hdvoice_enable_param_info_t    *p_hdvoice_enable_param;

   if (!session_object ||
       !session_object->dyn_svc_info.rx)
   {
      MSG_1(MSG_SSID_DFLT, MSG_LEGACY_ERROR,
            "vpm_send_hdvoice_enable_disable_msg: Session object (0x%x) or vpRx info pointer is NULL",
            session_object);

      return ADSP_EBADPARAM;
   }

   session_id = session_object->self_apr_port;

   /* Allocate the payload for ELITE_SET_PARAM */
   p_hdvoice_enable_param = (vpm_hdvoice_enable_param_info_t *)qurt_elite_memory_malloc(sizeof(vpm_hdvoice_enable_param_info_t),
                                                                                        QURT_ELITE_HEAP_DEFAULT);
   if (NULL == p_hdvoice_enable_param)
   {
      MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR,
          "vpm_send_hdvoice_enable_disable_msg: Failed to allocate memory for set_param");

      return ADSP_ENOMEMORY;
   }

   /* Select from Target setting for enable. For disable, select from Active setting.*/
   if (TRUE == is_enable)
   {
      feature_id = session_object->target_set.system_config.feature;
   }
   else /* Disable */
   {
      feature_id = session_object->active_set.system_config.feature;
   }

   /* Get ModuleID corresponding to the feature. */
   switch (feature_id)
   {
      case VSS_ICOMMON_CAL_FEATURE_WIDEVOICE:
      {
         p_hdvoice_enable_param->param_hdr.module_id = VOICE_MODULE_WV;
         break;
      }
      case VSS_ICOMMON_CAL_FEATURE_WIDEVOICE2:
      {
         p_hdvoice_enable_param->param_hdr.module_id = VOICE_MODULE_WV_V2;
         break;
      }
      default:
      {
         MSG_2(MSG_SSID_DFLT, MSG_LEGACY_HIGH,
               "vpm_send_hdvoice_enable_disable_msg(): Unsupported "
               "Feature ID: 0x%08X, enable(1)/disable(0): is_enable: %u",
               feature_id, is_enable);

         /* bail out */
         result = ADSP_EUNSUPPORTED;
         goto __bailout_wv_enable_disable;
      }
   }

   p_hdvoice_enable_param->param_hdr.param_id = VOICE_PARAM_MOD_ENABLE;
   p_hdvoice_enable_param->param_hdr.param_size = sizeof(uint32_t);
   p_hdvoice_enable_param->param_hdr.reserved = 0;
   p_hdvoice_enable_param->enable = is_enable;
   p_hdvoice_enable_param->reserved = 0;

   MSG_2(MSG_SSID_DFLT, MSG_LEGACY_MED, "vpm_send_hdvoice_enable_disable_msg:"
         " Feature ID: 0x%08X, enable(1)/disable(0): %u", feature_id, is_enable);

   payload_size = sizeof(elite_msg_param_cal_t);

   if (ADSP_EOK != (result = elite_msg_create_msg(&msg_rx_t,
                                                  &payload_size,
                                                  ELITE_CMD_SET_PARAM,
                                                  session_object->session_rsp_ctrl.p_rsp_queue,
                                                  VSS_IVOCPROC_DIRECTION_RX,
                                                  ADSP_EOK)))
   {
      MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR,
          "vpm_send_hdvoice_enable_disable_msg: FAILED to create EliteMsg");

      goto __bailout_wv_enable_disable;
   }

   payload_ptr = (elite_msg_param_cal_t *)msg_rx_t.pPayload;
   payload_ptr->unSize = sizeof(vpm_hdvoice_enable_param_info_t);
   payload_ptr->pnParamData = (int32_t *)p_hdvoice_enable_param;
   payload_ptr->unParamId = ELITEMSG_PARAM_ID_CAL;

   /* Push the msg buf to vpRx command Q */
   if (ADSP_EOK != (result = qurt_elite_queue_push_back(session_object->dyn_svc_info.rx->cmdQ,
                                                        (uint64_t *)&msg_rx_t)))
   {
      MSG_1(MSG_SSID_DFLT, MSG_LEGACY_HIGH,
            "vpm_send_hdvoice_enable_disable_msg: FAILED to push buf to vpRx cmdQ, result: 0x%08X", result);

      /* Return the allocated msg pkt buf */
      elite_msg_return_payload_buffer(&msg_rx_t);

      goto __bailout_wv_enable_disable;
   }

   /* Set the response pending status */
   vpm_set_cmd_rsp_pending_status(session_object, TRUE);

   /* Update the payload pointer in response handler */
   session_object->session_rsp_ctrl.p_ack_payload = p_hdvoice_enable_param;

   /* Increment the command issued counter */
   session_object->session_rsp_ctrl.num_cmd_issued++;

   MSG_3(MSG_SSID_DFLT, MSG_LEGACY_MED,
         "vpm_send_hdvoice_enable_disable_msg: Successfully sent msg to vpRx "
         "Feature ID: 0x%08X, enable(1)/disable(0): %u, session_id: 0x%lX",
         feature_id, is_enable, session_id);

   return result;

__bailout_wv_enable_disable:

   /* Free up the allocated set param payload */
   if (p_hdvoice_enable_param)
   {
      qurt_elite_memory_free((void *)p_hdvoice_enable_param);
   }

   return result;
}

/* Sends reinit command to dyn svcs */
ADSPResult vpm_reinit_dyn_svcs(
  vpm_session_object_t *session_obj,
  bool_t is_forced_reinit
)
{
   ADSPResult                 result = ADSP_EOK;
   vpm_active_settings_t   *active_set = &(session_obj->active_set);
   vpm_target_settings_t   *target_set = &(session_obj->target_set);

   MSG_5( MSG_SSID_DFLT, DBG_MED_PRIO,
         "vpm_reinit_dyn_svcs(): tx_port: 0x%04X, tx_top_id: 0x%08X, " \
         "tx_pp_sr: %lu, ec_ref_port: 0x%04X, ec_mode: 0x%08X",
         session_obj->target_set.tx_port_id, session_obj->target_set.tx_topology_id,
         session_obj->target_set.system_config.tx_pp_sr,
         session_obj->target_set.ec_ref_port_id, session_obj->target_set.vocproc_mode );

   MSG_5(MSG_SSID_DFLT, DBG_MED_PRIO,
         "vpm_reinit_dyn_svcs(): rx_port: 0x%04X, rx_topology_id: 0x%08X, " \
         "rx_pp_sampling_rate: %lu, rx_ip_sampling_rate: %lu," \
         "is_forced_reinit(0/1): %u", session_obj->target_set.rx_port_id,
         session_obj->target_set.rx_topology_id,
         session_obj->target_set.system_config.rx_pp_sr,
         session_obj->target_set.rx_in_sample_rate,
         is_forced_reinit);

    /* Re-init vpRx if required */
   if ( ( ( TRUE == is_forced_reinit ) ||
          (active_set->rx_topology_id != target_set->rx_topology_id) ||
          ( active_set->rx_in_sample_rate != target_set->rx_in_sample_rate ) ) &&          
        ( NULL != session_obj->dyn_svc_info.rx ) )
   {
     result = vpm_send_reconfig_msg_to_dyn_svc( session_obj,
                                                VSS_IVOCPROC_DIRECTION_RX );
     if ( ADSP_EOK != result )
      {
         MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_reinit_dyn_svcs(): Failed to reconfigure Rx dyn svc, "
               "session_id: 0x%lx, result: 0x%lx",
               session_obj->self_apr_port, result);
         return ADSP_EFAILED;
      }
      /* Clear the feature ID, as vpRx re-init will result in topo destroy and reconfig.
         If after re-init, target feature ID is non-none, it will be re-enabled */
      session_obj->active_set.system_config.feature = VSS_ICOMMON_CAL_FEATURE_NONE;
   }

   /* Re-init vpTx if required */
   if ( ( ( TRUE == is_forced_reinit ) ||
          ( active_set->system_config.tx_pp_sr != target_set->system_config.tx_pp_sr ) ||
          ( active_set->tx_topology_id != target_set->tx_topology_id ) ||
          ( active_set->vocproc_mode != target_set->vocproc_mode ) ||
          vpm_is_dev_channel_info_modified ( session_obj ) ) &&
        ( NULL != session_obj->dyn_svc_info.tx ) )
   {
     result = vpm_send_reconfig_msg_to_dyn_svc( session_obj,
                                                VSS_IVOCPROC_DIRECTION_TX );
     if ( ADSP_EOK != result )
      {
         MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_reinit_dyn_svcs(): Failed to reconfigure Tx dyn svc, "
               "session_id: 0x%lx, result: 0x%lx",
               session_obj->self_apr_port, result);
       return ADSP_EFAILED;
     }
   }
   else if ( ( active_set->tx_port_id != target_set->tx_port_id ) &&
             ( NULL != session_obj->dyn_svc_info.tx ) )
   {
     /* Update EndPoint Port ID if only port is changed and re-init not required.*/
     elite_msg_any_t msg_tx_t;
     uint32_t size = sizeof( elite_msg_custom_set_ep_id );
     elite_msg_custom_set_ep_id* payload_ptr;

     result = elite_msg_create_msg( &msg_tx_t,
               &size,
               ELITE_CUSTOM_MSG,
               session_obj->session_rsp_ctrl.p_rsp_queue,
               0,
               ADSP_EOK );

     if( ADSP_FAILED( result ) )
     {
       MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "vpm_reinit_dyn_svcs: FAILED to create EliteMsg!!\n" );
       return result;
     }

     payload_ptr = ( elite_msg_custom_set_ep_id* ) msg_tx_t.pPayload;
     payload_ptr->sec_opcode = VOICEPROC_SET_EP_ID;
     payload_ptr->ep_id[ 0 ] = target_set->tx_port_id;
     payload_ptr->ep_id[ 1 ] = target_set->ec_ref_port_id;

     result = qurt_elite_queue_push_back( session_obj->dyn_svc_info.tx->cmdQ, ( uint64_t* )&msg_tx_t );

     if ( ADSP_FAILED( result ) )
     {
       MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "vpm_reinit_dyn_svcs: FAILED to push to vptx cmdQ. error %d!!\n", result );
       ( void )elite_msg_return_payload_buffer( &msg_tx_t );
       return result;
     }

     /* Wait for Acknowledge*/
     (void)elite_svc_wait_for_ack(&msg_tx_t);

     if ( ADSP_EOK != ( result = payload_ptr->unResponseResult ) )
     {
       MSG_2( MSG_SSID_DFLT, DBG_ERROR_PRIO,
             "vpm_reinit_dyn_svcs(), "
             "set_ep_id for Tx failed with ack result: 0x%lx "
             "session_id: 0x%lx",
              result, session_obj->self_apr_port );
     }

     ( void )elite_msg_return_payload_buffer( &msg_tx_t );
   }

   /* If change in Rx sample rate and WV feature has been requested */
   if ( ( active_set->rx_in_sample_rate != target_set->rx_in_sample_rate  ) &&
        ( NULL != session_obj->dyn_svc_info.rx ) )
   {
     result = vpm_configure_rx_mixer_output( session_obj, VMX_RECONFIGURE_OUTPUT,
                                             target_set->rx_in_sample_rate );
     if ( ADSP_EOK != result )
     {
       MSG( MSG_SSID_DFLT, DBG_ERROR_PRIO,
            "vpm_action_reinit_vocproc(): Failed to reconfigure Rx matrix "
            "output ports");
         return ADSP_EFAILED;
      }
   }

   if ( ( active_set->tx_port_id != target_set->tx_port_id ) &&
        ( NULL != session_obj->dyn_svc_info.tx ) )
   {
     if ( is_pseudo_port_id( active_set->tx_port_id ) !=
          is_pseudo_port_id( target_set->tx_port_id ) ) 
      {
       result = vpm_configure_tx_mixer_input(session_obj, VMX_RECONFIGURE_INPUT );
       if ( ADSP_EOK !=  result )
         {
            MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                "vpm_reinit_dyn_svcs(): Failed to reconfigure Rx matrix output ports");
            return result;
         }
      }
   }

   vpm_update_device_active_set(session_obj);

   MSG_1( MSG_SSID_DFLT, DBG_HIGH_PRIO,
          "vpm_reinit_dyn_svcs(): VpTx/Rx re-init completed successfully, "
          "session_id: 0x%lx", session_obj->self_apr_port );

   return result;
}

ADSPResult vpm_action_register_event(
  vpm_session_object_t *session_obj,
  uint32_t event_id
)
{
  ADSPResult result = ADSP_EOK;
  elite_msg_custom_event_reg_type *payload_ptr = NULL;
  elite_msg_any_t msg_tx;
  uint32_t size = sizeof( elite_msg_custom_event_reg_type );

  /* If Tx dynamic service present */
  while ( session_obj->dyn_svc_info.tx )
  {
    result = elite_msg_create_msg( &msg_tx,
               &size,
               ELITE_CUSTOM_MSG,
               NULL,//no response required
               0,
               ADSP_EOK );
    if( ADSP_FAILED( result ) )
    {
      MSG( MSG_SSID_QDSP6, DBG_ERROR_PRIO,"vpm_action_register_event:"
           " FAILED to create EliteMsg" );
      break;
    }
    payload_ptr = (elite_msg_custom_event_reg_type*) msg_tx.pPayload;
    payload_ptr->sec_opcode = VOICEPROC_REGISTER_EVENT;
    payload_ptr->event_id = event_id;

    result = qurt_elite_queue_push_back( session_obj->dyn_svc_info.tx->cmdQ, ( uint64_t* )&msg_tx );
    if ( ADSP_FAILED( result ) )
    {
      MSG( MSG_SSID_DFLT, DBG_HIGH_PRIO,
           "vpm_action_register_event(): FAILED to set push buffer to dyn svc cmdQ" );
      elite_msg_return_payload_buffer( &msg_tx );
    }
    else
    {
      MSG_2( MSG_SSID_DFLT, DBG_MED_PRIO, "vpm_action_register_event(): Sucessfully sent"
             " register event cmd to VpTx, session_id 0x%X, event ID: 0x%08X",
             session_obj->self_apr_port, event_id );
    }
    break;
  }

  return result;
}
