#ifdef AFE_TEST
#include "qurt_elite.h"
#include "AFEInterface.h"
#include "AFEInternal.h"
#include "Elite.h"
#include "EliteMsg_Custom.h"
#include "EliteMsg_AfeCustom.h"
#include "VfrDriverInternal.h"

#define FEAT_MULT_VFR


#define AFE_TST_DATQ_MAX 16
#define AFE_TST_CMDQ_MAX 16
extern  aud_stat_afe_svc_t  *afe_svc_global_ptr;

#ifdef FEAT_MULT_VFR
#define VFR_CLIENT_INFO_VER      (VFR_CLIENT_INFO_VER_2)      
#define VFR_SRC_1_ID             (VFR_IRQ_MUX_CTL_SEL_INTERNAL << VSID_VFR_ID_MASK_BIT_POS)
#define VFR_SRC_2_ID             (VFR_IRQ_MUX_CTL_SEL_DIR_CONN_5 << VSID_VFR_ID_MASK_BIT_POS)
#else
#define VFR_CLIENT_INFO_VER      (VFR_CLIENT_INFO_VER_1)      
#define VFR_SRC_1_ID             (AFE_VFR_SOURCE_INTERNAL)
#endif /* FEAT_MULT_VFR */

int afe_vfr_test_thread(void *afeCmdQueue)
{
   ADSPResult                    result = ADSP_EOK;
   qurt_elite_channel_t 		   clientChannel;
   uint32_t 						   signalMask, unChannelStatus;
   qurt_elite_bufmgr_node_t      bufMgrNode;
   qurt_elite_queue_t            *vfrTestQ, *srvCmdQ, *respQ;
   elite_msg_any_t               eliteMsg, msg;
   int                           vfrCount, nActualSize;
   uint16_t                      port_id;
   uint16_t                      index;
   uint64_t                      vfr_drift_info;
   uint32_t                      mux_ctl_sel, vfr_src_idx;
   afe_dev_port_t   	            *dev_port_ptr = NULL;

   elite_msg_custom_afe_vfr_config_info_t *pVfrMsg;
   

   QURT_ELITE_ALIGN(char vfrRespQBuf[QURT_ELITE_QUEUE_GET_REQUIRED_BYTES(AFE_MAX_CMD_Q_ELEMENTS)],8);
   respQ = (qurt_elite_queue_t*) &vfrRespQBuf;
   qurt_elite_queue_init((char*)"VfrRespQ", AFE_MAX_CMD_Q_ELEMENTS, respQ);

   QURT_ELITE_ALIGN(char vfrTestQBuf[QURT_ELITE_QUEUE_GET_REQUIRED_BYTES(AFE_MAX_CMD_Q_ELEMENTS)],8);
   vfrTestQ = (qurt_elite_queue_t*) &vfrTestQBuf;
   qurt_elite_queue_init((char*)"VfrTestQ", AFE_MAX_CMD_Q_ELEMENTS, vfrTestQ);

   srvCmdQ = (qurt_elite_queue_t *)afeCmdQueue;

   signalMask = 0x80000000;
   qurt_elite_channel_init(&clientChannel);
   qurt_elite_channel_addq(&clientChannel, vfrTestQ, NULL);
   qurt_elite_channel_addq(&clientChannel, respQ, NULL);

   /* Send vfr enable request to AFE */
   if ( ADSP_FAILED(elite_mem_get_buffer(sizeof(elite_msg_custom_afe_vfr_config_info_t), &bufMgrNode, &nActualSize)))
   {
       return ADSP_ENOMEMORY;
   }

   pVfrMsg = (elite_msg_custom_afe_vfr_config_info_t *) bufMgrNode.pBuffer;
   pVfrMsg->buffer_returnq_ptr = bufMgrNode.pReturnQ;
   pVfrMsg->responseq_ptr = respQ;
   pVfrMsg->sec_op_code = ELITEMSG_CUSTOM_VFR_SYNC;
   eliteMsg.unOpCode = ELITE_CUSTOM_MSG;
   eliteMsg.pPayload = (void *)pVfrMsg;

   pVfrMsg->client_vfr_info.vfr_client_info_ver = VFR_CLIENT_INFO_VER;
   pVfrMsg->client_vfr_info.client_msg_id = 0xdead;
   pVfrMsg->client_vfr_info.vfr_clientq = vfrTestQ;
   pVfrMsg->client_vfr_info.vfr_source = VFR_SRC_1_ID;

   /* Send request for source 1 */
   result = qurt_elite_queue_push_back(srvCmdQ, (uint64_t*)&eliteMsg);

   if (ADSP_FAILED(result))
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "FAILED to send VFR CFG msg for source idx: 0");
      return result;
   }

   result = elite_svc_wait_for_ack(&eliteMsg);

#ifdef FEAT_MULT_VFR
   /* Send request for source 2 */
   pVfrMsg->client_vfr_info.vfr_source = VFR_SRC_2_ID;

   result = qurt_elite_queue_push_back(srvCmdQ, (uint64_t*)&eliteMsg);

   if (ADSP_FAILED(result))
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "FAILED to send VFR CFG msg for source idx: 7");
      return result;
   }

   result = elite_svc_wait_for_ack(&eliteMsg);
#endif /*FEAT_MULT_VFR */

   qurt_elite_queue_push_back(bufMgrNode.pReturnQ, (uint64_t*)&bufMgrNode);

   QURT_ELITE_ASSERT(ADSP_SUCCEEDED(result));

   vfrCount = 0;
   while(vfrCount < 200)
   {
       unChannelStatus = qurt_elite_channel_wait(&clientChannel, qurt_elite_queue_get_channel_bit(vfrTestQ));
       vfrCount++;
       result = qurt_elite_queue_pop_front(vfrTestQ, (uint64_t*) &msg);
       bufMgrNode.pReturnQ = ((elite_msg_custom_vfr_info_t *)(msg.pPayload))->buffer_returnq_ptr;
       bufMgrNode.pBuffer = msg.pPayload;

       elite_msg_custom_vfr_info_t *vfrMsg = (elite_msg_custom_vfr_info_t*) bufMgrNode.pBuffer;

       if (VFR_CLIENT_INFO_VER_2 == vfrMsg->vfr_info.vfr_client_info_ver)
       {
          mux_ctl_sel = (vfrMsg->vfr_info.vfr_source & VSID_VFR_BIT_MASK) >> VSID_VFR_ID_MASK_BIT_POS;
          vfr_src_idx = vfr_drv_global_stat.vfr_irq_src_map[mux_ctl_sel];
       }
       else if (VFR_CLIENT_INFO_VER_1 == vfrMsg->vfr_info.vfr_client_info_ver)
       {
          vfr_src_idx = vfrMsg->vfr_info.vfr_source;
       }
       else /* Un-supported VFR source */
       {
          MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Un-supported VFR intf version: %u", vfrMsg->vfr_info.vfr_client_info_ver);
          continue;
       }

       MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VFR msg received: resync_flag: %d, vfr_src_idx: %lu", vfrMsg->vfr_info.resync_status, vfr_src_idx);

       qurt_elite_queue_push_back(((elite_msg_custom_vfr_info_t *)(msg.pPayload))->buffer_returnq_ptr, (uint64_t *)&bufMgrNode );

       /* Get drift info for Audio IF ports */
       for (port_id = AFE_PORT_ID_AUDIO_IF_PORT_RANGE_START; port_id <= AFE_PORT_ID_AUDIO_IF_PORT_RANGE_END; port_id++)
       {
          dev_port_ptr = afe_svc_global_ptr->ports_audioif_ptr[GET_AUDIOIF_PORT_INDEX(port_id)];

          if ( (NULL != dev_port_ptr) && (TRUE == dev_port_ptr->drift_info[vfr_src_idx].vfr_enabled))
          {
             vfr_drift_info = dev_port_ptr->drift_info[vfr_src_idx].vfr_drift_info;
             MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VFR: Port_id: 0x%x, acc drift = %ld, frame ctr = %lu", port_id, (int32_t)(vfr_drift_info >> 32), (uint32_t)(vfr_drift_info));
          }
       }

       /* Get drift info for AUDIO IF TDM ports, if VFR feedback is enabled */
       for(index = 0; index < afe_svc_global_ptr->num_audioif_tdm_port; index++)
       {
          dev_port_ptr = afe_svc_global_ptr->ports_audioif_tdm_ptr[index];
          if ( (NULL != dev_port_ptr) && (TRUE == dev_port_ptr->drift_info[vfr_src_idx].vfr_enabled))
          {
             vfr_drift_info = dev_port_ptr->drift_info[vfr_src_idx].vfr_drift_info;
             MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VFR: Port_id: 0x%x, acc drift = %ld, frame ctr = %lu", port_id, (int32_t)(vfr_drift_info >> 32), (uint32_t)(vfr_drift_info));
          }
       }

       /* Get drift info for RT-Proxy ports */
       for (port_id = AFE_PORT_ID_RT_PROXY_PORT_RANGE_START; port_id <= AFE_PORT_ID_RT_PROXY_PORT_RANGE_END; port_id++)
       {
          dev_port_ptr = afe_svc_global_ptr->port_rt_proxy_ptr[GET_RT_PROXY_PORT_INDEX(port_id)];

          if ( (NULL != dev_port_ptr) && (TRUE == dev_port_ptr->drift_info[vfr_src_idx].vfr_enabled))
          {
             vfr_drift_info = dev_port_ptr->drift_info[vfr_src_idx].vfr_drift_info;
             MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VFR: Port_id: 0x%x, acc drift = %ld, frame ctr = %lu", port_id, (int32_t)(vfr_drift_info >> 32), (uint32_t)(vfr_drift_info));
          }
       }

       /* Get drift info for BT_FM ports */
       for (port_id = AFE_PORT_ID_INTERNAL_BT_FM_RANGE_START; port_id <= AFE_PORT_ID_INTERNAL_BT_FM_RANGE_END; port_id++)
       {
          dev_port_ptr = afe_svc_global_ptr->ports_riva_ptr[GET_RIVA_PORT_INDEX(port_id)];

          if ( (NULL != dev_port_ptr) && (TRUE == dev_port_ptr->drift_info[vfr_src_idx].vfr_enabled))
          {
             vfr_drift_info = dev_port_ptr->drift_info[vfr_src_idx].vfr_drift_info;
             MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VFR: Port_id: 0x%x, acc drift = %ld, frame ctr = %lu", port_id, (int32_t)(vfr_drift_info >> 32), (uint32_t)(vfr_drift_info));
          }
       }

       /* Get drift info for SLIMbus ports */
       for (port_id = AFE_PORT_ID_SLIMBUS_RANGE_START; port_id <= AFE_PORT_ID_SLIMBUS_RANGE_END; port_id++)
       {
          dev_port_ptr = afe_svc_global_ptr->ports_slimbus_ptr[GET_SLIMBUS_PORT_INDEX(port_id)];

          if ( (NULL != dev_port_ptr) && (TRUE == dev_port_ptr->drift_info[vfr_src_idx].vfr_enabled))
          {
             vfr_drift_info = dev_port_ptr->drift_info[vfr_src_idx].vfr_drift_info;
             MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VFR: Port_id: 0x%x, acc drift = %ld, frame ctr = %lu", port_id, (int32_t)(vfr_drift_info >> 32), (uint32_t)(vfr_drift_info));
          }
       }

       /* Get drift info for Pseudo ports */
       for (port_id = AFE_PORT_ID_PSEUDOPORT_RANGE_START; port_id < AFE_PORT_ID_PSEUDOPORT_RANGE_END; port_id++)
       {
          dev_port_ptr = afe_svc_global_ptr->pseudo_ports_ptr[GET_PSEUDO_PORT_INDEX(port_id)];

          if ( (NULL != dev_port_ptr) && (TRUE == dev_port_ptr->drift_info[vfr_src_idx].vfr_enabled))
          {
             vfr_drift_info = dev_port_ptr->drift_info[vfr_src_idx].vfr_drift_info;
             MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VFR: Port_id: 0x%x, acc drift = %ld, frame ctr = %lu", port_id, (int32_t)(vfr_drift_info >> 32), (uint32_t)(vfr_drift_info));
          }
       }

       /* Get drift info for HDMI OUTPUT ports */
       for(port_id = AFE_PORT_ID_HDMI_MULTISTREAM_RANGE_START; port_id < AFE_PORT_ID_HDMI_MULTISTREAM_RANGE_END; port_id++)
       {
          dev_port_ptr = afe_svc_global_ptr->port_hdmi_output_ptr[GET_HDMI_OUTPUT_PORT_INDEX(port_id)];

          if ( (NULL != dev_port_ptr) && !IS_IT_TX_PORT_AFE_ID(port_id) && (TRUE == dev_port_ptr->drift_info[vfr_src_idx].vfr_enabled))
          {
             vfr_drift_info = dev_port_ptr->drift_info[vfr_src_idx].vfr_drift_info;
             MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VFR: Port_id: 0x%x, acc drift = %ld, frame ctr = %lu", port_id, (int32_t)(vfr_drift_info >> 32), (uint32_t)(vfr_drift_info));
          }
       }

   } /* End of while loop */

   if ( ADSP_FAILED(elite_mem_get_buffer(sizeof(elite_msg_custom_afe_vfr_config_info_t), &bufMgrNode, &nActualSize)))
   {
       return ADSP_ENOMEMORY;
   }

   pVfrMsg = (elite_msg_custom_afe_vfr_config_info_t *) bufMgrNode.pBuffer;
   pVfrMsg->buffer_returnq_ptr = bufMgrNode.pReturnQ;
   pVfrMsg->responseq_ptr = respQ;
   pVfrMsg->sec_op_code = ELITEMSG_CUSTOM_VFR_DISABLE;
   eliteMsg.unOpCode = ELITE_CUSTOM_MSG;;
   eliteMsg.pPayload = (void *)pVfrMsg;

   pVfrMsg->client_vfr_info.vfr_client_info_ver = VFR_CLIENT_INFO_VER;
   pVfrMsg->client_vfr_info.client_msg_id = 0xdead;
   pVfrMsg->client_vfr_info.vfr_clientq = vfrTestQ;
   pVfrMsg->client_vfr_info.vfr_source = VFR_SRC_1_ID;

   result = qurt_elite_queue_push_back(srvCmdQ, (uint64_t*)&eliteMsg);

   if (ADSP_FAILED(result))
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "FAILED to send disable VFR CFG msg for first source");
      return result;
   }

   result = elite_svc_wait_for_ack(&eliteMsg);

#ifdef FEAT_MULT_VFR

   pVfrMsg->client_vfr_info.vfr_source = VFR_SRC_2_ID;

   result = qurt_elite_queue_push_back(srvCmdQ, (uint64_t*)&eliteMsg);

   if (ADSP_FAILED(result))
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "FAILED to send disable VFR CFG msg for second source");
      return result;
   }

   result = elite_svc_wait_for_ack(&eliteMsg);

#endif

   qurt_elite_queue_push_back(bufMgrNode.pReturnQ, (uint64_t*)&bufMgrNode);
   qurt_elite_queue_deinit(respQ);
   qurt_elite_queue_deinit(vfrTestQ);

   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VFR test end");

   return result;
}

#endif //#ifdef AFE_TEST
