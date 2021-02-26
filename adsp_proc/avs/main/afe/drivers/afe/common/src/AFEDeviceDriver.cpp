/*==============================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/common/src/AFEDeviceDriver.cpp#3 $
$DateTime: 2018/03/05 21:24:21 $
$Author: pwbldsvc $
$Change: 15605306 $
$Revision: #3 $

FILE:     AFEDeviceDriver.cpp

DESCRIPTION: Main Interface to the AFE Device driver

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright (c) 2013-2017 QUALCOMM Technologies, Inc.  All Rights Reserved.
QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
==============================================================================*/
/*============================================================================
EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order. Please
use ISO format for dates.

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/common/src/AFEDeviceDriver.cpp#3 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
11/06/07    RA      Created file

============================================================================*/

/*=====================================================================
 Includes
 ======================================================================*/
#include "AFEDeviceDriver.h"
#include "AFEHdmiOutputDrv.h"
#include "AFEDeviceDriver_i.h"
#include "AFERivaDriver.h"
#include "AFESlimbusDriver.h"
#include "AFERtProxyDriver.h"
#include "AFEPseudoPort.h"
#include "adsp_afe_service_commands.h"
#include "EliteMsg_AfeCustom.h"
#include "AFEVfrHandler.h"
#include "AFESpdifDriver.h"
#include "AfeTimeStampDriver.h"
#include "hwd_devcfg.h"
#include "AFESvc_topo.h"
#include "clock_manager.h"
#include "AFEI2sDriver.h"
#include "AFEPcmDriver.h"
#include "AFETdmDriver.h"
#include "AFEGroupDeviceDriver.h"
#include "AFEHdmi14Driver.h"
#include "AFEDmaManager.h"
#include "AFEDeviceDriverInterface.h"
#include "AFEAudioIfCommon.h"
#include "AudioIfClkDriver.h"
#include "afe_lpass_core_clk.h"
#include "AFESsrDrv.h"
#include "AFEErrorHandler.h"
#include "AFEUSBAudioDriver.h"
#include "AFEEncDecCmn_If.h"
#include "AFEDevCommon.h"
#include "afe_lpm.h"


/*=====================================================================
 Globals
 ======================================================================*/

#define NUM_SAMPLING_RATES 14

uint32 afe_device_sampling_rate_info[NUM_SAMPLING_RATES][2] = {
   { 8000, 4194304000U }, // 1000000 / 8000 = 125 Q25 =xxx xxxx. xxxxxxxxxxxxx => 0xfa00 0000 = 4194304000
   { 11025, 3043485895U }, // 7bit integer 125.00000 (25 bits fraction.) // to get fraction part. divide by 2 and then take out the integer part till the end.
   { 12000, 2796202667U },
   { 16000, 2097152000U },
   { 22050, 1521742948U },
   { 24000, 1398101333U },
   { 32000, 1048576000U },
   { 44100, 760871474U },
   { 48000, 699050667U },  //20.83333333..... = 20.83333333            8333333 / 2 => 4
   { 88200, 380435737U },  //for HDMI only
   { 96000, 349525333U },  //for HDMI only
   { 144000, 233016889U }, //for HDMI only
   { 176400, 190217868U }, //for HDMI only
   { 192000, 174762667U }, //for HDMI only
};

/*=====================================================================
 Functions
 ======================================================================*/
/**
  @brief Callback routine for the device driver

  @param[in] ctxt_ptr pointer to the 

  @return  none

 */
void afe_dev_driver_callback(void *ctxt_ptr)
{
   afe_dev_port_t *afe_port_ptr = (afe_dev_port_t *)ctxt_ptr;
   qurt_elite_signal_t      *p_dma_signal = NULL;
   afe_dynamic_thread_t     *dynamic_thread_ptr = NULL;

   p_dma_signal = &(afe_port_ptr->dma_signal);
   dynamic_thread_ptr = afe_port_ptr->dynamic_thread_ptr;

   if (AFE_PORT_STATE_RUN == afe_port_ptr->port_state)
   {
      dynamic_thread_ptr->isr_context_counter++;
      qurt_elite_signal_send(p_dma_signal);
   }
}

/**
  @brief Callback routine for the group device use case

  @param[in] ctxt_ptr pointer to the 

  @return  none

 */
void afe_dev_group_callback(void *ctxt_ptr)
{
   afe_group_device_state_struct_t *p_group_device_state  = (afe_group_device_state_struct_t *)ctxt_ptr;
   qurt_elite_signal_t      *p_dma_signal = NULL;
   afe_dynamic_thread_t     *dynamic_thread_ptr = NULL;

   p_dma_signal  = &(p_group_device_state->dma_signal);
   dynamic_thread_ptr = p_group_device_state->dynamic_thread_ptr;

   if (AFE_GROUP_DEVICE_STATE_RUN == p_group_device_state->group_state)
   {
      dynamic_thread_ptr->isr_context_counter++;
      qurt_elite_signal_send(p_dma_signal);
   }
}


/**
  @brief Remote API to AFE to vote LPASS Core HW Module by remote clients

  @param[in] pMe, pointer to AFE static service
  @param[in] pAprPacket, pointer to APR packet

  @return  ADSP_EOK on success, an error code on error

 */
ADSPResult afe_remote_lpass_core_hw_vote_reqeust(aud_stat_afe_svc_t *pMe, elite_apr_packet_t *pAprPacket)
{
   elite_apr_packet_t *pPacket = NULL;
   int32_t rc = APR_EOK;

   if (!pAprPacket)
   {
      return ADSP_EBADPARAM;
   }

   if (elite_apr_if_get_payload_size(pAprPacket) < sizeof(afe_cmd_remote_lpass_core_hw_vote_request))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Apr payload of insufficient size received for LPASS core HW vote request.");
      rc = ADSP_EUNEXPECTED;
      goto afe_remote_lpass_core_hw_vote_reqeust_error_1;
   }

   /* Allocate the return packet */
   if (APR_EOK != (rc = elite_apr_if_alloc_cmd_rsp(
                        pMe->apr_handle,
                        elite_apr_if_get_dst_addr(pAprPacket),
                        elite_apr_if_get_dst_port(pAprPacket),
                        elite_apr_if_get_src_addr(pAprPacket),
                        elite_apr_if_get_src_port(pAprPacket),
                        elite_apr_if_get_client_token(pAprPacket),
                        AFE_CMD_RSP_REMOTE_LPASS_CORE_HW_VOTE_REQUEST,
                        sizeof(afe_cmd_rsp_remote_lpass_core_hw_vote_request), &pPacket)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "afe_remote_lpass_core_hw_vote_reqeust Response packet allocation failed  = %ld!", rc);
      rc = ADSP_ENOMEMORY;
      goto afe_remote_lpass_core_hw_vote_reqeust_error_1;
   }
   if (NULL == pPacket)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "afe_remote_lpass_core_hw_vote_reqeust Response packet allocation failed. pPacket is NULL");
      rc = ADSP_ENOMEMORY;
      goto afe_remote_lpass_core_hw_vote_reqeust_error_1;
   }

   void *pPayload;

   /* Get the response payload */
   elite_apr_if_get_payload((void **)(&pPayload), pPacket);
   afe_cmd_rsp_remote_lpass_core_hw_vote_request *pRspPayload;
   pRspPayload = (afe_cmd_rsp_remote_lpass_core_hw_vote_request *)pPayload;

   /* parse command payload */
   afe_cmd_remote_lpass_core_hw_vote_request *pCmd;
   pCmd = (afe_cmd_remote_lpass_core_hw_vote_request *)(elite_apr_if_get_payload_ptr(pAprPacket));

   if (pCmd)
   {
      switch (pCmd->hw_block_id)
      {
         case AFE_LPASS_CORE_HW_BLOCK_ID_AVTIMER:
         {
            avtimer_open_param_t avtimer_open_param;

            avtimer_open_param.client_name = &(pCmd->client_name[0]);
            avtimer_open_param.flag = 0; //not used in api

            if (ADSP_FAILED(rc = avtimer_drv_hw_open((void **)(&(pRspPayload->client_handle)), &avtimer_open_param)))
            {
               MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "avtimer open request failed  = %ld!", rc);
               rc = ADSP_EFAILED;
               goto afe_remote_lpass_core_hw_vote_reqeust_error_2;
            }

            MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "avtimer client_handle = 0x%lx, name = %s!",
                  (pRspPayload->client_handle), avtimer_open_param.client_name);
         }
            break;
         default:
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Unsupported hw block id = %lX!", pCmd->hw_block_id);
            rc = ADSP_EUNSUPPORTED;
            goto afe_remote_lpass_core_hw_vote_reqeust_error_2;
         }
      }
   }
   else
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "afe_remote_lpass_core_hw_vote_reqeust ERROR payload pointer is NULL!");
   }
   /* Send response packet out */
   if (APR_EOK != (rc = elite_apr_if_async_send(pMe->apr_handle, pPacket)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "afe_remote_lpass_core_hw_vote_reqeust Response send failed  = %ld!", rc);
      rc = ADSP_EFAILED;
      goto afe_remote_lpass_core_hw_vote_reqeust_error_2;
   }

   /* free the original packet */
   (void)elite_apr_if_free(pMe->apr_handle, pAprPacket);

   return ADSP_EOK;

afe_remote_lpass_core_hw_vote_reqeust_error_2:
   //free response packet
   (void)elite_apr_if_free(pMe->apr_handle, pPacket);
afe_remote_lpass_core_hw_vote_reqeust_error_1:
   //ack back the apr packet
   (void)elite_apr_if_end_cmd(pMe->apr_handle, pAprPacket, rc);
   return rc;
}

/**
  @brief Remote API to AFE to devote LPASS Core HW Module by remote clients

  @param[in] pMe, pointer to AFE static service
  @param[in] pAprPacket, pointer to APR packet

  @return  ADSP_EOK on success, an error code on error

 */
ADSPResult afe_remote_lpass_core_hw_devote_reqeust(aud_stat_afe_svc_t *pMe, elite_apr_packet_t *pAprPacket)
{
   ADSPResult result = ADSP_EOK;

   if (!pAprPacket)
   {
      return ADSP_EBADPARAM;
   }

   if (elite_apr_if_get_payload_size(pAprPacket) < sizeof(afe_cmd_remote_lpass_core_hw_devote_request))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Apr payload of insufficient size received for LPASS core HW vote request.");
      (void)elite_apr_if_end_cmd(pMe->apr_handle, pAprPacket, ADSP_EFAILED);
      return ADSP_EUNEXPECTED;
   }

   /* parse command payload */
   afe_cmd_remote_lpass_core_hw_devote_request *pCmd = (afe_cmd_remote_lpass_core_hw_devote_request *)(elite_apr_if_get_payload_ptr(pAprPacket));

   if (pCmd)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "client_handle = 0x%lx", (pCmd->client_handle));

      switch (pCmd->hw_block_id)
      {
         case AFE_LPASS_CORE_HW_BLOCK_ID_AVTIMER:
         {
            if (ADSP_FAILED(result = avtimer_drv_hw_close((void *)(pCmd->client_handle))))
            {
               MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "avtimer release request failed  = %d!", result);
               result = ADSP_EFAILED;
            }
         }
            break;
         default:
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Unsupported hw block id = %lX!", pCmd->hw_block_id);
            result = ADSP_EUNSUPPORTED;
         }

      }
   }
   else
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "afe_remote_lpass_core_hw_devote_reqeust ERROR payload pointer is NULL!");
   }
   /* end command */
   (void)elite_apr_if_end_cmd(pMe->apr_handle, pAprPacket, result);

   return result;
}

/**
  @brief Perform one time initialization of the AFE Device
         driver

  @param[in] none

  @return  ADSP_EOK on success, an error code on error

 */
ADSPResult afe_device_driver_init()
{
   ADSPResult result = ADSP_EFAILED;

   if (ADSP_EOK != (result = afe_dev_init_lpass_hw_info()))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE LPASS h/w info init failed");
   }

   if (ADSP_EOK != (result = afe_i2s_driver_init()))
   {
      if (ADSP_EUNSUPPORTED == result)
      {
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Warning: AFE I2S Driver Not Supported");
      }
      else
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE I2S Driver Init failed");
      }
   }

   if (ADSP_EOK != (result = afe_pcm_driver_init()))
   {
      if (ADSP_EUNSUPPORTED == result)
      {
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Warning: AFE PCM Driver Not Supported");
      }
      else
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE PCM Driver Init failed");
      }
   }

   if (ADSP_EOK != (result = afe_tdm_driver_init()))
   {
      if (ADSP_EUNSUPPORTED == result)
      {
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Warning: AFE TDM Driver Not Supported");
      }
      else
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE TDM Driver Init failed");
      }
   }

   if (ADSP_EOK != (result = afe_hdmi14_driver_init()))
   {
      if (ADSP_EUNSUPPORTED == result)
      {
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Warning: AFE HDMI1.4 Driver Not Supported");;
      }
      else
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE HDMI1.4 Driver Init failed");
      }
   }

   if (ADSP_EOK != (result = afe_dma_mgr_init()))
   {
      if (ADSP_EUNSUPPORTED == result)
      {
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Warning: AFE dma manager driver Not Supported");
      }
      else
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE dma manager driver init failed");
      }
   }

   if (ADSP_EOK != (result = afe_audioif_mux_init()))
   {
      if (ADSP_EUNSUPPORTED == result)
      {
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Warning: AFE AUDIOIF mux Not Supported");
      }
      else
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE AUDIOIF mux init failed");
      }
   }

#ifndef SIM
   if (ADSP_EOK != (result = afe_slimbus_driver_init()))
   {
      if (ADSP_EUNSUPPORTED == result)
      {
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Warning: SLIMBUS interface is not supported");
      }
      else
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE SLIMBUS Driver Init failed");
      }
   }
#endif // !SIM

   if (ADSP_EOK != (result = afe_usba_driver_init()))
   {
      if (ADSP_EUNSUPPORTED == result)
      {
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Warning: USB Audio interface is not supported");
      }
      else
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE USB Audio Driver Init failed");
      }
   }

   if (ADSP_EOK != (result = afe_driver_riva_init()))
   {
      if (ADSP_EUNSUPPORTED == result)
      {
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Warning: RIVA interface is not supported");
      }
      else
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE Riva Driver Init failed");
      }
   }

   if (ADSP_EOK != (result =   afe_spdif_driver_init()))
   {
      if (ADSP_EUNSUPPORTED == result)
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Warning: SPDIF interface is not supported");
      }
      else
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE SPDIF Driver Init failed");
      }
   }

   if (ADSP_EOK != (result =   afe_hdmi_output_driver_init()))
   {
      if (ADSP_EUNSUPPORTED == result)
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Warning: HDMI Output interface is not supported");
      }
      else
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE HDMI Output Driver Init failed");
      }
   }

   if (ADSP_EOK != (result = afe_driver_rt_proxy_init()))
   {
      result = ADSP_EFAILED;
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE RT proxy port Driver Init failed");
   }


   /* Initialize codec driver */
   result = afe_svc_cdc_handler_init();
   if ((ADSP_EOK != result) && (ADSP_ENOTIMPL != result))
   {
      result = ADSP_EFAILED;
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE Codec handler Init failed");
   }
   else
   {
      if (ADSP_ENOTIMPL == result)
      {
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE Codec handler stubbed out");
      }
      result = ADSP_EOK;
   }

   /* Initialize clock driver */
   result = afe_clock_manager_init();
   if ((ADSP_EOK != result) && (ADSP_ENOTIMPL != result))
   {
      result = ADSP_EFAILED;
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE Clock handler Init failed");
   }
   else
   {
      if (ADSP_ENOTIMPL == result)
      {
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE Clock handler stubbed out");
      }
      result = ADSP_EOK;
   }

   if (ADSP_EOK != (result = vfr_handler_init()))
   {
      if (ADSP_EUNSUPPORTED == result)
      {
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Warning: VFR hw module not supported");
      }
      else
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VFR handler Initialization failed");
      }
   }

   if (ADSP_EOK != (result = afe_ssr_handler_init()))
   {
      if (ADSP_EUNSUPPORTED == result)
      {
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Warning: SSR handling not supported");
      }
      else
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "SSR handler Initialization failed");
      }
   }

   if (ADSP_EOK != (result = afe_err_handler_init()))
   {
      if (ADSP_EUNSUPPORTED == result)
      {
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Warning: Error handling not supported");
      }
      else
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error handler Initialization failed");
      }
   }

   return ADSP_EOK;
}

/**
  @brief Perform de-initialization of the AFE Device driver

  @param[in] None

  @return  ADSP_EOK on success, an error code on error

 */
ADSPResult afe_device_driver_deinit()
{
   afe_i2s_driver_deinit();

   afe_pcm_driver_deinit();

   afe_tdm_driver_deinit();

   afe_hdmi14_driver_deinit();

   afe_dma_mgr_deinit();

   afe_audioif_mux_deinit();

#ifndef SIM
   afe_slimbus_driver_deinit();
#endif // !SIM

   afe_usba_driver_deinit();

   afe_driver_riva_deinit();

   afe_spdif_driver_deinit();

   afe_hdmi_output_driver_deinit();

   afe_driver_rt_proxy_deinit();

   vfr_handler_deinit();

   // De-initialize codec driver
   afe_svc_cdc_handler_deinit();

   // De-initialize clock driver
   afe_clock_manager_deinit();

   afe_err_handler_deinit();

   return ADSP_EOK;
}

/**
  @brief Perform Device port initialization

  @param[in] afe_svc_ptr pointer for AFE static service

  @return  ADSP_EOK on success, an error code on error

 */
ADSPResult afe_dev_port_init(aud_stat_afe_svc_t *pAfeSvc)
{
   afe_i2s_port_init(pAfeSvc);

   afe_pcm_port_init(pAfeSvc);

   afe_tdm_port_init(pAfeSvc);

   afe_hdmi14_port_init(pAfeSvc);

   afe_slimbus_port_init(pAfeSvc);

   afe_usba_port_init(pAfeSvc);

   afe_riva_port_init(pAfeSvc);

   afe_spdif_port_init(pAfeSvc);

   afe_hdmi_output_port_init(pAfeSvc);

   afe_rt_proxy_port_init(pAfeSvc);

   afe_pseudo_port_init(pAfeSvc);

   return ADSP_EOK;
}

/**
  @brief Perform Device port de-initialization

  @param[in] afe_svc_ptr pointer for AFE static service

  @return  ADSP_EOK on success, an error code on error

 */
ADSPResult afe_dev_port_deinit(aud_stat_afe_svc_t *pAfeSvc)
{
   afe_i2s_port_deinit(pAfeSvc);

   afe_pcm_port_deinit(pAfeSvc);

   afe_tdm_port_deinit(pAfeSvc);

   afe_hdmi14_port_deinit(pAfeSvc);

   afe_slimbus_port_deinit(pAfeSvc);

   afe_usba_port_deinit(pAfeSvc);

   afe_riva_port_deinit(pAfeSvc);

   afe_spdif_port_deinit(pAfeSvc);

   afe_hdmi_output_port_deinit(pAfeSvc);

   afe_rt_proxy_port_deinit(pAfeSvc);

   afe_pseudo_port_deinit(pAfeSvc);

   return ADSP_EOK;
}


/**
  @brief Perform specific cmd processing for the given device

  @param[in] opcode - cmd opcode
  @param[in] obj_ptr - object pointer used for serving the given
        cmd
  @param[in] cfg_ptr - configuration info if any

  @return  ADSP_EOK on success, an error code on error

 */
ADSPResult afe_device_generic_cmd(uint32_t opcode, void *obj_ptr, void *cfg_ptr)
{
   ADSPResult result = ADSP_EOK;

   switch (opcode)
   {
      case ELITEMSG_CUSTOM_RIVA_IPC_COMMAND:
         result = afe_driver_ipc_cmd_proc((aud_stat_afe_svc_t *)obj_ptr);
         break;
      default:
         result = ADSP_EFAILED;
         break;
   }

   return result;
}

/**
 * This function provides the time since the dma is started
 *
 * @param[in] port_id
 * @return Flag indicating successful operation and current time
 *         via pointer.
 */
ADSPResult afe_drv_get_time_for_port(uint16_t port_id, uint64 *curr_time)
{
   afe_dev_port_t *dev_port = NULL;

   *curr_time = 0;


   afe_svc_get_port(port_id, &dev_port);
   if (NULL != dev_port)
   {
      if (IS_RIVA_PORT_AFE_ID(port_id))
      {
         afe_get_time_for_riva_port(dev_port, curr_time);
      }
      else if (IS_RT_PROXY_PORT_AFE_ID(port_id))
      {
         afe_get_time_for_rt_proxy_port(dev_port, curr_time);
      }
   }

   return ADSP_EOK;
}

/**
 * This function is to de-allocate the device interface related resources
 *
 * @param[in] pDevPort,  pointer to port instance
 * @return the ADSP_EOK error code
 */
ADSPResult afe_dev_dealloc_resources(afe_dev_port_t *pDevPort)
{
   ADSPResult result = ADSP_EOK;
   bool_t                 stop_device = TRUE;
   afe_dynamic_thread_t   **dynamic_thread_pptr;
   avtimer_drv_handle_t   h_avtimer_drv;
   afe_group_device_state_struct_t    *p_group_device = NULL;
   afe_err_handler_drv_handle_t       *h_err_handler_drv;

   if (!pDevPort)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Null afe port pointer");
      return ADSP_EFAILED;
   }

   p_group_device      = pDevPort->p_group_device_state;
   h_avtimer_drv       = pDevPort->avt_drv;
   h_err_handler_drv   = &pDevPort->err_handler_drv;
   dynamic_thread_pptr = &pDevPort->dynamic_thread_ptr;

   if (NULL != p_group_device)
   {
      /* If one of the port is still running, don't deallocate the resources */
      if (AFE_GROUP_DEVICE_STATE_STOP != p_group_device->group_state)
      {
         stop_device = FALSE;
      }
      else /* Group device is stopped */
      {
         dynamic_thread_pptr = &pDevPort->p_group_device_state->dynamic_thread_ptr;
         h_avtimer_drv       = pDevPort->p_group_device_state->avt_drv;
         h_err_handler_drv   = &pDevPort->p_group_device_state->err_handler_drv;
      }
   }

   /**< Destroy the dynamic thread .*/
   if ((NULL != *dynamic_thread_pptr) && (TRUE == stop_device))
   {
      if (ADSP_FAILED(result = afe_dynamic_thread_destroy(dynamic_thread_pptr)))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to destroy thread for port :0x%x", pDevPort->intf);
      }
   }

   /**< Close AV timer driver .*/
   if (NULL != h_avtimer_drv)
   {
      avtimer_drv_hw_close(h_avtimer_drv);
      pDevPort->avt_drv = NULL;

      if ((TRUE == stop_device) &&
          (NULL != pDevPort->p_group_device_state))
      {
         pDevPort->p_group_device_state->avt_drv = NULL;
      }
   }

   /**< De-register afe error handler driver.*/
   if ((NULL != *h_err_handler_drv) && (TRUE == stop_device))
   {
      if (ADSP_FAILED(result = afe_err_handler_driver_deregister(h_err_handler_drv)))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to de-register error handler driver for the port :0x%x", pDevPort->intf);
      }
      else
      {
         MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO, "AFE error handler de-registration successful, port id 0x%x ", pDevPort->intf);
      }

      /*If configured for signal miss, then need to free up
       * the stored values in signal miss information pool*/
      if (NULL != signal_miss_pool_global_ptr)
      {
         afe_err_handler_free_port_signal_miss_cfg(pDevPort);
      }
   }

	/* Reset AFE port latency mode to DEFAULT latency mode*/
	pDevPort->latency_mode = AFE_PORT_DEFAULT_LATENCY_MODE;

   return result;
}

/**
 * This function is to stop the dma for sink\source port
 *
 * @param[in] pDevPort, ptr to the port instance.
 * @return the ADSP_EOK error code
 */
ADSPResult afe_dev_stop(afe_dev_port_t *pDevPort)
{
   ADSPResult result = ADSP_EOK;
   uint16_t               port_idx;
   bool_t                 stop_device = TRUE;
   uint32_t               group_dev_state = AFE_GROUP_DEVICE_STATE_STOP;
   qurt_elite_signal_t    *p_dma_signal;
   afe_dev_port_t         *p_dev_port = NULL;
   afe_group_device_state_struct_t    *p_group_device = NULL;

   if (!pDevPort)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Null afe port pointer");
      return ADSP_EFAILED;
   }

   p_group_device = pDevPort->p_group_device_state;
   p_dma_signal = &pDevPort->dma_signal;

   if (NULL != p_group_device)
   {
      /* The port in context would have been set to STOP state before call to close the device
              Check if any other member port is running */
      port_idx = 0;

      while (port_idx < AFE_GROUP_DEVICE_NUM_PORTS)
      {
         if (NULL != (p_dev_port = p_group_device->p_dev_port[port_idx++]))
         {
            group_dev_state |= p_dev_port->port_state;
         }
      }

      /* If one of the port is still running, don't close the physical interface */
      if (AFE_GROUP_DEVICE_STATE_STOP != group_dev_state)
      {
         stop_device = FALSE;
      }
      else /* Set the group state to STOP */
      {
         pDevPort->p_group_device_state->group_state = AFE_GROUP_DEVICE_STATE_STOP;
         p_dma_signal = &(pDevPort->p_group_device_state->dma_signal);
      }
   }

   /* Remove port from the VFR sync */
   result = afe_drv_remove_from_vfr_sync(pDevPort->drift_info);


   // Close the port
   if (IS_SLIMBUS_PORT_AFE_ID(pDevPort->intf))
   {
      afe_slimbus_close(pDevPort);
   }
   else if (IS_RIVA_PORT_AFE_ID(pDevPort->intf))
   {
      afe_riva_close(pDevPort);
   }
   else if (IS_USBA_PORT_AFE_ID(pDevPort->intf))
   {
      afe_usba_close(pDevPort);
   }
   else if (IS_RT_PROXY_PORT_AFE_ID(pDevPort->intf))
   {
      afe_rt_proxy_port_close(pDevPort);
   }
   else if ((IS_AUDIO_IF_PORT_AFE_ID(pDevPort->intf) || IS_AUDIO_IF_TDM_PORT_AFE_ID(pDevPort->intf)) && (TRUE == stop_device))
   {
      if (AFE_DEV_RESTART == pDevPort->dev_proc_seq)
      {
         uint32_t prop_id = AFE_DEVICE_RESTART_PROP;
         uint32_t prop_size = sizeof(afe_dev_restart_info_t);
         afe_dev_restart_info_t dev_restart_params;
         memset(&dev_restart_params, 0, sizeof(afe_dev_restart_info_t));
         dev_restart_params.sample_rate = 0; // there is no new sampling rate info yet and it is not required during the stop...

         if ((NULL != pDevPort->afe_drv_ftbl) && (NULL != pDevPort->afe_drv_ftbl->set_properties))
         {
            if (ADSP_EOK != pDevPort->afe_drv_ftbl->set_properties(pDevPort->afe_drv, prop_id, (int8_t *)&dev_restart_params, prop_size))
            {
               return ADSP_EFAILED;
            }
         }
      }

      if ((NULL != pDevPort->afe_drv_ftbl) && (NULL != pDevPort->afe_drv_ftbl->stop))
      {
         pDevPort->afe_drv_ftbl->stop(pDevPort->afe_drv);
      }
      else
      {
         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "driver function table is null: dir: %d, port id: 0x%x", pDevPort->dir, pDevPort->intf);
      }

   }
   else if (IS_SPDIF_PORT_AFE_ID(pDevPort->intf))
   {
      afe_spdif_close(pDevPort);
   }
   else if (IS_HDMI_OUTPUT_PORT_AFE_ID(pDevPort->intf) || IS_HDMI_OVER_DP_PORT_AFE_ID(pDevPort->intf))
   {
      result |= afe_hdmi_output_driver_close(pDevPort);
   }
   else if (IS_PSEUDO_PORT_AFE_ID(pDevPort->intf))
   {
      afe_pseudo_port_close(pDevPort);
   }

   /**< In order to reduce latency for device restart and device recovery,
    * clear only dma signal instead of destroying the thread. Dynamic thread will
    * be destroyed during actual port stop */
   if ((TRUE == stop_device) && (NULL != p_dma_signal->pChannel))
   {
      qurt_elite_signal_clear(p_dma_signal);
   }

   /**< In case of device recovery, there will be no change in sampling rate,
    * so no need to free working buffer and reallocate again */
   if (AFE_DEV_RECOVERY != pDevPort->dev_proc_seq)
   {
      /* Release the copy buffers
          using aligned free routine, since these buffers had been allocated
          using aligned malloc
       */
      if (NULL != pDevPort->working_buf_1)
      {
         qurt_elite_memory_aligned_free(pDevPort->working_buf_1);
         pDevPort->working_buf_1 = NULL;
      }
      if (NULL != pDevPort->scratch_buf)
      {
         qurt_elite_memory_aligned_free(pDevPort->scratch_buf);
         pDevPort->scratch_buf = NULL;
         pDevPort->scratch_buf_size = 0;
      }
      if (NULL != pDevPort->working_buf_2)
      {
         qurt_elite_memory_aligned_free(pDevPort->working_buf_2);
         pDevPort->working_buf_2 = NULL;
      }

      /* To process a stop on group, all member ports are stopped already */
      if ((TRUE == stop_device) &&
          (NULL != p_group_device))
      {
         if (NULL != p_group_device->p_group_io_buf)
         {
            qurt_elite_memory_aligned_free(p_group_device->p_group_io_buf);
            p_group_device->p_group_io_buf = NULL;
            p_group_device->group_io_buf_size = 0;
         }
         if (NULL != p_group_device->default_header_backup.p_header)
         {
            qurt_elite_memory_aligned_free(p_group_device->default_header_backup.p_header);
            p_group_device->default_header_backup.p_header = NULL;
         }
         /**<Need to devote only for actual stop, not needed for device restart/recovery.**/
         if (AFE_NORMAL_SEQ == pDevPort->dev_proc_seq)
         {
            if (NULL != p_group_device->p_first_active_port)
            {
               result = afe_mmpm_voting(p_group_device->p_first_active_port, AFE_MMPM_GROUP_TOPOLOGY, AFE_MMPM_DEVOTE, TRUE);
               p_group_device->p_first_active_port = NULL;
            }
         }
      }

      /* Release the port associated custom header memory as necessary */
      if (NULL != pDevPort->custom_header_info.p_header)
      {
         qurt_elite_memory_aligned_free(pDevPort->custom_header_info.p_header);
         pDevPort->custom_header_info.p_header = NULL;
      }
   }

   return result;
}



static ADSPResult afe_dev_i2s_check_sampling_rate(afe_dev_port_t *pDevPort)
{
   ADSPResult result = ADSP_EOK;
   afe_dev_port_t *pPairDevPort = NULL;
   //This is getting the pair intf port id for i2s. If it is rx, then add 1 to figure out the tx port id, if not, opposite way.
   uint16_t pair_intf = (RX_DIR == pDevPort->dir) ? (pDevPort->intf + TX_DIR) : (pDevPort->intf - TX_DIR);

   if (ADSP_EOK == (result = afe_svc_get_port(pair_intf, &pPairDevPort)))
   {
      if (AFE_PORT_STATE_RUN == pPairDevPort->port_state)
      {
         if (pDevPort->sample_rate != pPairDevPort->sample_rate)
         {
            MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "afe_dev_i2s_check_sampling_rate mismatch intf[%x]sr[%d] intf[%x] sr[%d]",
                  pDevPort->intf, pDevPort->sample_rate, pPairDevPort->intf, pPairDevPort->sample_rate);
            result = ADSP_EFAILED;
         }
      }
   }
   else
   {
      // there is no pair interface to be compared
      result = ADSP_EOK;
   }

   return result;
}

static ADSPResult afe_dev_check_group_start_info(afe_dev_port_t *pDevPort)
{
   afe_group_device_state_struct_t       *p_group_device = NULL;
   uint16_t frame_size = pDevPort->p_group_device_state->frame_size;
   uint16_t i;

   p_group_device = pDevPort->p_group_device_state;

   //1. Check group slot mapping offset against frame_size..
   for (i = 0; i < pDevPort->slot_mapping_info.slot_num_channel; i++)
   {
      if (frame_size < (pDevPort->slot_mapping_info.offset_info[i].offset_input + pDevPort->slot_mapping_info.slot_bytes_per_channel))
      {
         MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE Port group slot mapping is failed: offset is out of frame boundary: port_id: 0x%x offset: %d frame_size: %d", pDevPort->intf, pDevPort->slot_mapping_info.offset_info[i].offset_input, frame_size);
         return ADSP_EFAILED;
      }
   }

   //2. Check custom header offset against frame_size..
   //   NOTE: This is special case to set header..
   if (pDevPort->custom_header_info.is_cfg_received)
   {
      if ((frame_size < (pDevPort->custom_header_info.header_offset.offset_input + pDevPort->custom_header_info.header_width_per_frame)))
      {
         if (pDevPort->custom_header_info.is_default_header_backup)
         {
            if (p_group_device->default_header_backup.p_header)
            {
               qurt_elite_memory_aligned_free(p_group_device->default_header_backup.p_header);
            }

            memset((void *)&p_group_device->default_header_backup, 0, sizeof(afe_custom_header_info_t));
         }

         MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE Port CUSTOM HEADER Cfg is failed: offset is out of frame boundary: port_id: 0x%x offset: %d frame_size: %d", pDevPort->intf, pDevPort->custom_header_info.header_offset.offset_input, frame_size);
         return ADSP_EFAILED;
      }
   }
   return ADSP_EOK;
}

ADSPResult afe_dev_alloc_resources(afe_dev_port_t *pDevPort)
{
   ADSPResult                result      = ADSP_EOK;
   afe_dynamic_thread_task_t task;
   afe_group_device_state_struct_t       *p_group_device = NULL;
   bool_t                                is_group_device = FALSE; /*Default FALSE */
   bool_t                                start_device = TRUE; /*Default TRUE */
   avtimer_drv_handle_t                  *h_avtimer_drv = NULL;
   afe_dynamic_thread_t                  **dynamic_thread_pptr;
   afe_err_handler_drv_handle_t          *h_err_handler_drv = NULL;

   if (NULL == pDevPort)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Null afe port pointer");
      return ADSP_EFAILED;
   }

   p_group_device = pDevPort->p_group_device_state;

   if (NULL != p_group_device)
   {
      is_group_device = TRUE;

      MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE Port %x Group frame_size: %d num_frame_in_group %d",
            pDevPort->intf, pDevPort->p_group_device_state->frame_size, pDevPort->p_group_device_state->num_frame_in_group);

      result = afe_dev_check_group_start_info(pDevPort);
      if (ADSP_EOK != result)
      {
         return result;
      }

      /* Check if group is already running */
      if (AFE_GROUP_DEVICE_STATE_RUN == p_group_device->group_state)
      {
         start_device = FALSE;
      }
      else /* First member port start */
      {
         pDevPort->p_group_device_state->avt_drv         = NULL;
         pDevPort->p_group_device_state->err_handler_drv = NULL;
         h_avtimer_drv     = (avtimer_drv_handle_t *)&(pDevPort->p_group_device_state->avt_drv);
         h_err_handler_drv = (afe_err_handler_drv_handle_t *)&(pDevPort->p_group_device_state->err_handler_drv);

         result = afe_mmpm_voting(pDevPort, AFE_MMPM_GROUP_TOPOLOGY, AFE_MMPM_VOTE, TRUE);
         p_group_device->p_first_active_port = pDevPort;
      }
   }
   else /* Not group device */
   {
      pDevPort->avt_drv         = NULL;
      pDevPort->err_handler_drv = NULL;
      h_avtimer_drv     = (avtimer_drv_handle_t *)&(pDevPort->avt_drv);
      h_err_handler_drv = (afe_err_handler_drv_handle_t *)&(pDevPort->err_handler_drv);
   }

   /* Open AV timer and register for AFE error handler
    *
    * HW MAD doesn't use AV timer and thereby not registering to AFE error handler.
    * (Actually AFE error handler requires avtimer to get time stamp during signal miss
    * and decides whether to crash or recover.) */
   if ((NULL == pDevPort->hw_mad_ptr) && (TRUE == start_device))
   {
      avtimer_open_param_t open_param;
      open_param.client_name = (char *)"AfeSvc";
      open_param.flag = 0;
      if (ADSP_EOK != (result = avtimer_drv_hw_open(h_avtimer_drv, &open_param)))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to Open AVTimer Driver for port id 0x%x,", pDevPort->intf);
         return ADSP_EFAILED;
      }

      MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO, "AV timer hw open success with handle: 0x%lx for port id 0x%x ",
            *h_avtimer_drv, pDevPort->intf);

      // register afe error handler driver
      if (ADSP_EOK != (result = afe_err_handler_driver_register(h_err_handler_drv)))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to register afe error handler Driver: 0x%lx,", pDevPort->intf);
         return ADSP_EFAILED;
      }

      MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO, "Successfully registered to AFE error handler, port id 0x%x ", pDevPort->intf);
   }

   /* Assign the port process function pointers based on direction and data type */
   if (pDevPort->dir == AFE_PORT_RX_DIR)
   {
      if (AFE_UNCOMPRESSED_DATA_PORT == pDevPort->port_data_type)
      {
         if (FALSE == is_group_device)
         {
            //non-grouped device
            //PCM in (port) - PCM out (device) OR
            //PCM in (port) - Compressed out (device) (encoding use case)
            task.pfn_task_func = afe_port_render_pcm_samples_thread_safe;
         }
         else
         {
            //grouped device: only PCM in (device) - PCM out (port) is supported
            if (AFE_UNCOMPRESSED_DATA_PORT != pDevPort->dev_data_type)
            {
               MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "encoding in afe is not supported for group device on port 0x%x", pDevPort->intf);
               return ADSP_EFAILED;
            }

            task.pfn_task_func = afe_group_device_render_pcm_samples_thread_safe;
         }
      }
      else
      {
         //Compressed in - compressed out
         task.pfn_task_func = afe_port_render_compressed_data_thread_safe;
      }
   }
   else /* Tx direction */
   {
      if (AFE_UNCOMPRESSED_DATA_PORT == pDevPort->port_data_type)
      {
         if (FALSE == is_group_device)
         {
            //non-grouped device
            //PCM in (device)               - PCM out (port) OR
            //Compressed in (device) - PCM out (port) (decoding use case)
            task.pfn_task_func = afe_port_forward_pcm_samples_thread_safe;
         }
         else
         {
            //grouped device: only PCM in (device) - PCM out (port) is supported
            if (AFE_UNCOMPRESSED_DATA_PORT != pDevPort->dev_data_type)
            {
               MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "decoding in afe is not supported for group device on port 0x%x", pDevPort->intf);
               return ADSP_EFAILED;
            }

            task.pfn_task_func = afe_group_device_forward_pcm_samples_thread_safe;
         }
      }
      else
      {
         //Compressed in - compressed out
         task.pfn_task_func = afe_port_forward_compressed_data_thread_safe;
      }
   }

   if (TRUE == start_device)
   {
      if (FALSE == is_group_device)
      {
         task.task_signal_ptr = &pDevPort->dma_signal;
         task.vp_task_arg = (void *)pDevPort;
         dynamic_thread_pptr = &(pDevPort->dynamic_thread_ptr);
      }
      else /* Group device */
      {
         task.vp_task_arg = (void *)p_group_device;
         task.task_signal_ptr = &p_group_device->dma_signal;
         dynamic_thread_pptr = &(p_group_device->dynamic_thread_ptr);
      }

      if (ADSP_FAILED(result = afe_dynamic_thread_launch(dynamic_thread_pptr,
                                                         &task,
                                                         elite_get_thrd_prio(ELITETHREAD_STAT_AFE_PORT_MANAGER_PRIO_ID),
                                                         AFE_THREAD_STACK_SIZE,
                                                         (uint32)pDevPort->intf)))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to launch dynamic thread for port: 0x%x", pDevPort->intf);
         return result;
      }
   }

   return result;
}

/**
 * This function is to start the dma for sink\source port
 *
 * @param[in] pDevPort, ptr to the port instance.
 * @return the ADSP_EOK error code
 */
ADSPResult afe_dev_start(afe_dev_port_t *pDevPort)
{
   ADSPResult                         result = ADSP_EOK;
   afe_group_device_state_struct_t    *p_group_device = NULL;
   bool_t                             is_group_device = FALSE; /*Default FALSE */
   bool_t                             start_device = TRUE;     /*Default TRUE */

   if (NULL == pDevPort)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Null afe port pointer");
      return ADSP_EFAILED;
   }

   p_group_device = pDevPort->p_group_device_state;

   if (NULL != p_group_device)
   {
      is_group_device = TRUE;

      /* Check if group is already running */
      if (AFE_GROUP_DEVICE_STATE_RUN == p_group_device->group_state)
      {
         start_device = FALSE;
      }
   }
   result = afe_port_get_frame_size(pDevPort->latency_mode, pDevPort->sample_rate, &pDevPort->int_samples_per_period);
   result = afe_port_get_frame_size(pDevPort->latency_mode, pDevPort->dev_sample_rate, &pDevPort->dev_int_samples_per_period);
   if (result != ADSP_EOK)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " AFEDeviceDriver : Invalid sampling rate detected,Portid::0x%x", pDevPort->intf);
      return result;
   }

   /**< In case of device recovery, there will be no change in sampling rate and thereby, no need to re-allocate working/device buffers*/
   if (AFE_DEV_RECOVERY != pDevPort->dev_proc_seq)
   {
      //allocate port working buffers
      if (ADSP_EOK != (result = afe_dev_create_working_buffers(pDevPort)))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to allocate memory for working afe_dev_create_working_buffers buffers, port id : 0x%x ", pDevPort->intf);
         return result;
      }

      //allocate device working buffers (for enc/dec)
      if (ADSP_EOK != (result = afe_create_enc_dec_dev_buffers(pDevPort)))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to allocate memory for afe_create_enc_dec_dev_buffers buffers, port id : 0x%x ", pDevPort->intf);
         return result;
      }
   }

   if (IS_SLIMBUS_PORT_AFE_ID(pDevPort->intf))
   {
      if (ADSP_EOK !=  afe_slimbus_open(pDevPort))
      {
         return ADSP_EFAILED;
      }
   }
   else if (IS_RIVA_PORT_AFE_ID(pDevPort->intf))
   {
      if (ADSP_EOK !=  afe_riva_open(pDevPort))
      {
         return ADSP_EFAILED;
      }
   }
   else if (IS_USBA_PORT_AFE_ID(pDevPort->intf))
   {
      if (ADSP_EOK !=  afe_usba_open(pDevPort))
      {
         return ADSP_EFAILED;
      }
   }
   else if (IS_RT_PROXY_PORT_AFE_ID(pDevPort->intf))
   {
      if (ADSP_EOK !=  afe_rt_proxy_port_open(pDevPort))
      {
         return ADSP_EFAILED;
      }
   }
   else if ((IS_AUDIO_IF_PORT_AFE_ID(pDevPort->intf) || ((IS_AUDIO_IF_TDM_PORT_AFE_ID(pDevPort->intf)))) && (TRUE == start_device))
   {
      //error check for the sampling rate on the paired i2s interfaces...
      if (IS_I2S_PORT_AFE_ID(pDevPort->intf))
      {
         if (ADSP_EOK != afe_dev_i2s_check_sampling_rate(pDevPort))
         {
            return ADSP_EFAILED;
         }
      }

      /* Audio IF will be opened only ONCE for the first member port,  PORT is in STOP state at this point */
      if (AFE_DEV_RESTART == pDevPort->dev_proc_seq)
      {
         uint32_t prop_id = AFE_DEVICE_RESTART_PROP;
         uint32_t prop_size = sizeof(afe_dev_restart_info_t);
         afe_dev_restart_info_t dev_restart_params;
         memset(&dev_restart_params, 0, sizeof(afe_dev_restart_info_t));
         dev_restart_params.sample_rate = pDevPort->sample_rate;

         if ((NULL != pDevPort->afe_drv_ftbl) && (NULL != pDevPort->afe_drv_ftbl->set_properties))
         {
            if (ADSP_EOK != pDevPort->afe_drv_ftbl->set_properties(pDevPort->afe_drv, prop_id, (int8_t *)&dev_restart_params, prop_size))
            {
               return ADSP_EFAILED;
            }
         }
         else
         {
            MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "driver function table is null: dir: %d, port id: 0x%x", pDevPort->dir, pDevPort->intf);
         }

      }

      dev_start_params_t dev_start_params;
      memset(&dev_start_params, 0, sizeof(dev_start_params_t));
      dev_start_params.int_samples_per_period = pDevPort->dev_int_samples_per_period;
      if (TRUE == is_group_device)
      {
         dev_start_params.cb_info.afe_dev_int_cb = afe_dev_group_callback;
         dev_start_params.cb_info.cb_arg = (void *)p_group_device;
      }
      else
      {
         dev_start_params.cb_info.afe_dev_int_cb = afe_dev_driver_callback;
         dev_start_params.cb_info.cb_arg = (void *)pDevPort;
      }
      if ((NULL != pDevPort->afe_drv_ftbl) && (NULL != pDevPort->afe_drv_ftbl->start))
      {
         if (ADSP_EOK != pDevPort->afe_drv_ftbl->start(pDevPort->afe_drv, &dev_start_params))
         {
            return ADSP_EFAILED;
         }
      }
      else
      {
         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "driver function table is null: dir: %d, port id: 0x%x", pDevPort->dir, pDevPort->intf);
      }
   }
   else if (IS_SPDIF_PORT_AFE_ID(pDevPort->intf))
   {
      if (ADSP_EOK !=  afe_spdif_open(pDevPort))
      {
         return ADSP_EFAILED;
      }
   }
   else if (IS_HDMI_OUTPUT_PORT_AFE_ID(pDevPort->intf) || (IS_HDMI_OVER_DP_PORT_AFE_ID(pDevPort->intf)))
   {
      if (ADSP_EOK != afe_hdmi_output_driver_open(pDevPort))
      {
         return ADSP_EFAILED;
      }
   }
   else if (IS_PSEUDO_PORT_AFE_ID(pDevPort->intf))
   {
      if (ADSP_EOK != afe_pseudo_port_open(pDevPort))
      {
         return ADSP_EFAILED;
      }
   }

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFECmdDmaStart: Dma Start Success: dir: %d, intf: 0x%x",
         pDevPort->dir, pDevPort->intf);


   /* Add port to VFR sync */
   result = afe_drv_add_to_vfr_sync(pDevPort->drift_info, pDevPort->sample_rate, pDevPort->intf);


   pDevPort->periodic_int_counter            = 0;
   pDevPort->running_int_counter             = 0;
   pDevPort->is_first_dma                    = TRUE;

   /* Change the group state to RUN if one of the port in group is started */
   if (NULL != p_group_device)
   {
      p_group_device->group_state = AFE_GROUP_DEVICE_STATE_RUN;
   }

   return result;
}

/**
 * This function moves port state from run to Suspend state
 *
 * @param[in] port_id, id of the port
 * @return the ADSP_EOK error code
 */
ADSPResult afe_device_move_to_suspend_state(uint32_t port_id)
{
   ADSPResult        result = ADSP_EOK;
   afe_dev_port_t    *p_dev_port = NULL;
   uint16_t          port_idx;
   afe_dev_port_t    *p_dev_port_l = NULL;

   /* Fetch the pointer to the port structure */
   if (ADSP_EOK != (result = afe_svc_get_port(port_id, &p_dev_port)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Failed to get pointer to port obj, port_id: 0x%lx", port_id);
      return result;
   }

   /* Check if this port is member of group device */
   if (NULL != p_dev_port->p_group_device_state)
   {
      for (port_idx = 0; port_idx < AFE_GROUP_DEVICE_NUM_PORTS; port_idx++)
      {
         p_dev_port_l = p_dev_port->p_group_device_state->p_dev_port[port_idx];

         if (NULL != p_dev_port_l)
         {
            qurt_elite_mutex_lock(&p_dev_port_l->port_mutex);

            /* Change the port state from RUN to SUSPEND. The member ports in a group which were
             *  running earlier, only be restarted (ex: not the ports already in CONFIG state).*/
            if (AFE_PORT_STATE_RUN == p_dev_port_l->port_state)
            {
               p_dev_port_l->port_state = AFE_PORT_STATE_SUSPEND;

               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Moving port state from RUN to SUSPEND state, port_id: 0x%lx", p_dev_port_l->intf);
            }

            qurt_elite_mutex_unlock(&p_dev_port_l->port_mutex);
         }
      }
   }
   else /* Standalone port */
   {
      qurt_elite_mutex_lock(&p_dev_port->port_mutex);

      if (AFE_PORT_STATE_RUN == p_dev_port->port_state)
      {
         p_dev_port->port_state = AFE_PORT_STATE_SUSPEND;

         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Moving port state from RUN to SUSPEND state, port_id: 0x%lx", port_id);
      }
      else
      {
         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "FAILED to move port state to SUSPEND, port_id: 0x%lx, port_state: %d",\
                  port_id, p_dev_port->port_state);
         result = ADSP_EFAILED;
      }

      qurt_elite_mutex_unlock(&p_dev_port->port_mutex);
   }

   return result;
}

/**
 * This function is to restore device configuration parameters
 *
 * @param[in] p_dev_port, ptr to the port instance.
 * @return the ADSP_EOK error code
 */
ADSPResult afe_restore_dev_config_param(afe_dev_port_t *p_dev_port)
{
   ADSPResult  result = ADSP_EOK;

   /***< Restoring of configuration is needed only for SLIMbus interface for now.
    * TODO : Need to explore about the requirement of restoring*/
   if (IS_SLIMBUS_PORT_AFE_ID(p_dev_port->intf))
   {
      result = afe_restore_slimbus_cfg(p_dev_port);
   }
   else if (IS_USBA_PORT_AFE_ID(p_dev_port->intf))
   {
      result = afe_restore_usba_cfg(p_dev_port);
   }

   return result;
}

ADSPResult afe_device_i2sclk_reconfig(afe_dev_port_t *pDevPort)
{
   ADSPResult result = ADSP_EOK;
   uint32_t prop_id = AFE_DEVICE_RESTART_PROP;
   uint32_t prop_size = sizeof(afe_dev_restart_info_t);
   afe_dev_restart_info_t dev_restart_param;

   memset(&dev_restart_param, 0, sizeof(afe_dev_restart_info_t));

   if ((NULL != pDevPort->afe_drv_ftbl) && (NULL != pDevPort->afe_drv_ftbl->get_properties))
   {
      if (ADSP_EOK != pDevPort->afe_drv_ftbl->get_properties(pDevPort->afe_drv, prop_id, (int8_t *)&dev_restart_param, prop_size))
      {
         return ADSP_EFAILED;
      }
   }

   if (HwdDevCfg_HWSupport(HWD_MODULE_TYPE_GENERICCLK))
   {
      uint32 clkid = 0;

      switch (pDevPort->intf)
      {
         case  AUDIO_PORT_ID_I2S_RX:
            clkid = AFE_CLOCK_SET_CLOCK_ID_SPEAKER_I2S_IBIT;
            break;
         case AFE_PORT_ID_PRIMARY_MI2S_RX:
         case AFE_PORT_ID_PRIMARY_MI2S_TX:
            clkid = AFE_CLOCK_SET_CLOCK_ID_PRI_MI2S_IBIT;
            break;
         case AFE_PORT_ID_SECONDARY_MI2S_RX:
         case AFE_PORT_ID_SECONDARY_MI2S_TX:
            clkid = AFE_CLOCK_SET_CLOCK_ID_SEC_MI2S_IBIT;
            break;
         case AFE_PORT_ID_TERTIARY_MI2S_RX:
         case AFE_PORT_ID_TERTIARY_MI2S_TX:
            clkid = AFE_CLOCK_SET_CLOCK_ID_TER_MI2S_IBIT;
            break;
         case AFE_PORT_ID_QUATERNARY_MI2S_RX:
         case AFE_PORT_ID_QUATERNARY_MI2S_TX:
            clkid = AFE_CLOCK_SET_CLOCK_ID_QUAD_MI2S_IBIT;
            break;
         default:
            clkid = 0;
            break;
      }
      if (0 == clkid)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
               "Failed to lookup inf : %x", pDevPort->intf);
         result = ADSP_EBADPARAM;
      }
      else
      {
         MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
               "Generic CLK reconfig inf  0x%x new sampling rate: %lu original sampling reate: %lu",
               pDevPort->intf, pDevPort->sample_rate, dev_restart_param.sample_rate);
         result = afe_clock_manager_reconfig_i2s_clk(clkid, pDevPort->sample_rate, dev_restart_param.sample_rate);
      }
   }
   else
   {

      MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
            "Generic CLK reconfig inf  0x%x new sampling rate: %lu original sampling reate: %lu",
            pDevPort->intf, pDevPort->sample_rate, dev_restart_param.sample_rate);
      result = afe_set_audioif_reconfig_i2s_clk(pDevPort->intf, pDevPort->sample_rate, dev_restart_param.sample_rate);

   }

   return result;
}

/**
 * This function is to reconfigure the clocks for sink\source port
 *
 * @param[in] pDevRxPort, ptr to the rx port instance.
 * @param[in] pDevTxPort, ptr to the tx port instance.
 * @return the ADSP_EOK error code
 */
ADSPResult afe_reconfig_clocks_for_restart(afe_dev_port_t *pDevRxPort, afe_dev_port_t *pDevTxPort)
{
   ADSPResult  result = ADSP_EOK;

   /* reconfigure new clock based on the sampling rate */
   if ((NULL != pDevRxPort) && (NULL != pDevTxPort))
   {
      if (ADSP_EOK != (result = afe_device_i2sclk_reconfig(pDevRxPort)))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to recofig i2s clocks: %x", pDevRxPort->intf);
         return ADSP_EFAILED;
      }

      if (pDevTxPort->intf != (pDevRxPort->intf | TX_DIR))
      {
         if (ADSP_EOK != (result = afe_device_i2sclk_reconfig(pDevTxPort)))
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to recofig i2s clocks: %x", pDevTxPort->intf);
            return ADSP_EFAILED;
         }
      }
   }
   else if (NULL != pDevRxPort)
   {
      if (ADSP_EOK != (result = afe_device_i2sclk_reconfig(pDevRxPort)))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to recofig i2s clocks: %x", pDevRxPort->intf);
         return ADSP_EFAILED;
      }

   }
   else if (NULL != pDevTxPort)
   {
      if (ADSP_EOK != (result = afe_device_i2sclk_reconfig(pDevTxPort)))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to recofig i2s clocks: %x", pDevTxPort->intf);
         return ADSP_EFAILED;
      }
   }

   return result;
}

/**
Allocate the memory for low power memories to achevie low power
audio solutions

@param[in] num_bytes Number of bytes need to be allocated
@param[in] align_size - alignment size in bytes

@return
Valid memory pointer in case of success, otherwise NULL ptr is
returned
 */
void* afe_dev_lpa_aligned_malloc(uint32_t num_bytes, uint32_t align_size)
{

#ifdef LPM_ALLOC_DEBUG
   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "lpa_align_malloc:num bytes %lu: align size: %lu", num_bytes, align_size);
#endif

   ADSPResult result = ADSP_EOK;
   if (ADSP_EOK != (result = afe_lpm_client_register()))
   {
	   MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "LPM client registration failed. Deregistering.");
	   afe_lpm_client_deregister();
   }

   // only allow alignment to 4 or more bytes.
  if (align_size <= 4) align_size = 4;

   // keep out the crazy stuff.
  if (align_size > (1 << 30)) return NULL;

   char *ptr, *ptr2, *aligned_ptr;
   uint32_t temp_addr;
   uint32_t align_mask = ~(align_size - 1);

   // allocate enough for requested bytes + alignment wasteage + 1 word for storing offset
   // (which will be just before the aligned ptr)
   ptr = (char *)qurt_elite_memory_heapmgr_malloc(num_bytes + align_size + sizeof(int), elite_mem_afe_dma_buf_heap_id);
  if(ptr == NULL)
   {
	   afe_lpm_client_deregister();
       return (NULL);
   }

   //allocate enough for requested bytes + alignment wasteage + 1 word for storing offset
   temp_addr =  (uint32)(ptr + sizeof(int));
   aligned_ptr = (char *)((uint32_t)(temp_addr - 1) & align_mask) + align_size;

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "lpa_align_malloc::aligned_ptr 0x%lx: ptr: 0x%lx", (uint32)aligned_ptr, (uint32)ptr);

   // save offset to raw pointer from aligned pointer
   ptr2 = aligned_ptr - sizeof(int);
   *((int *)ptr2) = (int)(aligned_ptr - ptr);

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "lpa_align_malloc::offset ptr 0x%lx: offset value: 0x%lx", (uint32)ptr2, (uint32) * ((int *)ptr2));

   return (aligned_ptr);
}

/**
De-Allocate the low power memory

@param[in] ptr Pointer to be freed

@return
None
 */
void afe_dev_lpa_aligned_free(void *ptr)
{
   ADSPResult result = ADSP_EOK;
   uint32_t *temp_ptr = (uint32_t *)ptr;
   uint32_t *ptr2 = temp_ptr - 1;

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "lpa_align_free:addr in 0x%lx: add for offset: 0x%lx", (uint32)ptr, (uint32)ptr2);

   if (NULL == ptr)
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Warning: NULL reference to DMA buf pointer");
      return;
   }
   temp_ptr -= *ptr2 / sizeof(uint32_t);
   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "lpa_align_free: free addr %lx: offset: %lx", (uint32)temp_ptr, *ptr2);
   qurt_elite_memory_heapmgr_free(temp_ptr, elite_mem_afe_dma_buf_heap_id);

   if (ADSP_EOK != (result = afe_lpm_client_deregister()))
   {
	   MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "LPM client deregistration failed.");
   }
}

ADSPResult afe_dev_create_working_buffers(afe_dev_port_t *pDevPort)
{
   uint32_t scratch_buf_size = 0, working_buf_size = 0;
   uint32_t group_dev_working_buf_size = 0;
   afe_group_device_state_struct_t   *p_group_device = NULL;

   if ((AFE_UNCOMPRESSED_DATA_PORT == pDevPort->port_data_type) && (AFE_UNCOMPRESSED_DATA_PORT == pDevPort->dev_data_type))
   {
      //PCM in - PCM out
      /* Allocate the memory for working and scratch buffers in the port. */
      /* If Rx port, working buffers need to be allocated for 64bit to handle client accumulations */
      if (TX_DIR == pDevPort->dir)
      {
         working_buf_size = pDevPort->int_samples_per_period * pDevPort->channels * pDevPort->bytes_per_channel;
      }
      else
      {
         working_buf_size = pDevPort->int_samples_per_period * pDevPort->channels * sizeof(uint64_t);
      }

      pDevPort->working_buf_size = working_buf_size;

      /* Update the working buffer size to twice of it for mono case, to handle buffer needs of interfaces such as I2S */
      if (1 == pDevPort->channels)
      {
         working_buf_size *= 2;
      }

      /* Allocate the port buffers, using aligned malloc, so that it can be used for vector operations
               8 Byte alignment needed for 64-bit vector operations on these buffers
       */
      if (NULL == (pDevPort->working_buf_1 = (int8_t *)qurt_elite_memory_aligned_malloc(working_buf_size, 8, AFE_SINK_ADD_BUF)))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Malloc failure while allocating working buffer 1 for port 0x%x", pDevPort->intf);
         return ADSP_ENOMEMORY;
      }

      if (NULL == (pDevPort->working_buf_2 = (int8_t *)qurt_elite_memory_aligned_malloc(working_buf_size, 8, AFE_SINK_ADD32_BUF)))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Malloc failure while allocating working buffer 2 for port 0x%x", pDevPort->intf);
         return ADSP_ENOMEMORY;
      }

      /* size of scratch buffer is determined by max of max client size or port working buffer size
          if clients are connected before port start, scratch buf size is non zero in port structure
       */

      /* updating the scratch buf size if port working buf size is greater and allocate the scratch buffer */
      scratch_buf_size = pDevPort->scratch_buf_size;
      if (working_buf_size > scratch_buf_size)
      {
         scratch_buf_size = working_buf_size;
         pDevPort->scratch_buf_size = scratch_buf_size;
      }

      if (0 != scratch_buf_size)
      {
         if (NULL == (pDevPort->scratch_buf = (int8_t *)qurt_elite_memory_aligned_malloc(scratch_buf_size, 8, AFE_SINK_COPY_BUF)))
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Malloc failure while allocating scratch buffer for port 0x%x", pDevPort->intf);
            return ADSP_ENOMEMORY;
         }
         else
         {
            MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "scratch buffer is allocated for port:0x%x of size %ld ", pDevPort->intf, scratch_buf_size);
         }
      }

      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "scratch_buf_size %ld working_buf_size %ld", scratch_buf_size, working_buf_size);

      /* Allocate the port buffers, using aligned malloc, so that it can be used for vector operations
               8 Byte alignment needed for 64-bit vector operations on these buffers
       */

      /* If port part of group device group AND group output buffer is not allocated */
      if ((NULL != pDevPort->p_group_device_state) &&
          (NULL == pDevPort->p_group_device_state->p_group_io_buf))
      {
         p_group_device = pDevPort->p_group_device_state;
         group_dev_working_buf_size = p_group_device->num_channels
                                      * p_group_device->int_samples_per_period
                                      * p_group_device->bytes_per_channel;
         if (NULL == (p_group_device->p_group_io_buf = (int8_t *)qurt_elite_memory_aligned_malloc(group_dev_working_buf_size, 8, AFE_SINK_ADD32_BUF)))
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Malloc failure while allocating group device output buffer for port 0x%x", pDevPort->intf);
            return ADSP_ENOMEMORY;
         }
         p_group_device->group_io_buf_size = group_dev_working_buf_size;
      }

#ifdef DBG_BUFFER_ADDRESSES
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Working Buffer 1: 0x%8x, size %d", pDevPort->working_buf_1, working_buf_size);
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Scratch Buffer: 0x%8x, size %d", pDevPort->scratch_buf, scratch_buf_size);
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Working Buffer 1: 0x%8x, size %d", pDevPort->working_buf_2, working_buf_size);
#endif /* DBG_BUFFER_ADDRESSES */
   }
   else if ((AFE_UNCOMPRESSED_DATA_PORT == pDevPort->port_data_type) && (AFE_GENERIC_COMPRESSED_DATA_PORT == pDevPort->dev_data_type))
   {
      //PCM in - Compressed out (encoding use case) in RX direction
      //Compressed in - PCM out (decoding use case) in TX direction
      ADSPResult result = ADSP_EOK;
      uint32_t working_buf_size_port = 0;

      //allocate sizes
      if (TX_DIR == pDevPort->dir)
      {
         working_buf_size_port = pDevPort->int_samples_per_period * pDevPort->channels * pDevPort->bytes_per_channel;
      }
      else
      {
         working_buf_size_port     = pDevPort->int_samples_per_period * pDevPort->channels * sizeof(uint64_t);
      }

      //allocate the working_buf_1, working_buf_2, scratch_buffer
      if (NULL == (pDevPort->working_buf_1 = (int8_t *)qurt_elite_memory_aligned_malloc(working_buf_size_port, 8, QURT_ELITE_HEAP_DEFAULT)))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Malloc failure while allocating working_buf_1 for port 0x%x", pDevPort->intf);
         result = ADSP_ENOMEMORY;
         return result;
      }

      if (NULL == (pDevPort->working_buf_2 = (int8_t *)qurt_elite_memory_aligned_malloc(working_buf_size_port, 8, QURT_ELITE_HEAP_DEFAULT)))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Malloc failure while allocating working_buf_2 for port 0x%x", pDevPort->intf);
         result = ADSP_ENOMEMORY;
         return result;
      }

      //size of scratch buffer is determined by max of max client size or port working buffer size
      scratch_buf_size = pDevPort->scratch_buf_size;
      if (working_buf_size_port > scratch_buf_size)
      {
         scratch_buf_size = working_buf_size_port;
         pDevPort->scratch_buf_size = scratch_buf_size;
      }
      if (0 != scratch_buf_size)
      {
         if (NULL != pDevPort->scratch_buf)
         {
            qurt_elite_memory_aligned_free(pDevPort->scratch_buf);
            pDevPort->scratch_buf = NULL;
         }
         if (NULL == (pDevPort->scratch_buf = (int8_t *)qurt_elite_memory_aligned_malloc(scratch_buf_size, 8, QURT_ELITE_HEAP_DEFAULT)))
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Malloc failure while allocating scratch_buf for port 0x%x", pDevPort->intf);
            result = ADSP_ENOMEMORY;
            return result;
         }
      }

      //success case: assign state variables
      pDevPort->working_buf_size = working_buf_size_port;

      MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Port 0x%x [scratch_buf_size, working_buf_size_port] [%lu, %lu]",
            pDevPort->intf, pDevPort->scratch_buf_size, pDevPort->working_buf_size);

      MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Port 0x%x [scratch_buf, working_buf_1, working_buf_2] [0x%p, 0x%p, 0x%p]",
            pDevPort->intf, pDevPort->scratch_buf, pDevPort->working_buf_1, pDevPort->working_buf_2);

      return result;
   }
   else
   {
      //Compressed use case
      working_buf_size = (pDevPort->int_samples_per_period) * (pDevPort->channels) * (pDevPort->bytes_per_channel);
      if (NULL == (pDevPort->working_buf_1 = (int8_t *)qurt_elite_memory_aligned_malloc(working_buf_size, 8, AFE_SINK_ADD_BUF)))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Malloc failure while allocating working_buf_1 for port 0x%x", pDevPort->intf);
         return ADSP_ENOMEMORY;
      }
      if (NULL == (pDevPort->working_buf_2 = (int8_t *)qurt_elite_memory_aligned_malloc(working_buf_size, 8, AFE_SINK_ADD_BUF)))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Malloc failure while allocating working_buf_2 for port 0x%x", pDevPort->intf);
         return ADSP_ENOMEMORY;
      }
   }

   return ADSP_EOK;
}

/**
 * updates the avt drift per port for riva and rt proxy ports. for audio if and slimbus, the ISR itself updates.
 */
void afe_drv_update_avt_drift(afe_dev_port_t *dev_port_ptr)
{
   uint16_t port_id = dev_port_ptr->intf;

   //for slimbus and audio if drift variable is directly updated in calc function.
   if (IS_SLIMBUS_PORT_AFE_ID(port_id))
   {
      afe_device_update_avt_drift(dev_port_ptr);
   }
   else if (IS_AUDIO_IF_PORT_AFE_ID(port_id))
   {
      afe_device_update_avt_drift(dev_port_ptr);
   }
   else if (IS_AUDIO_IF_TDM_PORT_AFE_ID(port_id))
   {
      afe_device_update_avt_drift(dev_port_ptr);
   }
   else if (IS_RIVA_PORT_AFE_ID(port_id))
   {
      afe_update_avt_drift_for_riva_port(dev_port_ptr);
   }
   else if (IS_RT_PROXY_PORT_AFE_ID(port_id))
   {
      afe_update_avt_drift_for_rt_proxy_port(dev_port_ptr);
   }
   else if (IS_USBA_PORT_AFE_ID(port_id))
   {
      afe_update_avt_drift_for_usba_port(dev_port_ptr);
   }
   else if (IS_SPDIF_PORT_AFE_ID(port_id))
   {
      afe_update_avt_drift_for_spdif_port(dev_port_ptr);
   }
   else if (IS_HDMI_OUTPUT_PORT_AFE_ID(port_id) || IS_HDMI_OVER_DP_PORT_AFE_ID(port_id))
   {
      afe_device_update_avt_drift(dev_port_ptr);
   }
#ifdef AVT_BASED_AFE_DRIFT_DEBUG
   if (IS_AUDIO_IF_PORT_AFE_ID(port_id) || (IS_AUDIO_IF_TDM_PORT_AFE_ID(port_id)))
   {
      afe_dev_avt_drift_t *avt_drift_ptr = dev_port_ptr->avt_drift_params.avt_drift_struct_ptr;
      MSG_9(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "port_id %x, av_time = 0x%x%x, dev_time = 0x%x%x, init:av_time = 0x%x%x, dev_time = 0x%x%x ", dev_port_ptr->intf,\
               (uint32_t)(avt_drift_ptr->avtimer_timestamp >> 32), (uint32_t)(avt_drift_ptr->avtimer_timestamp),\
               (uint32_t)(avt_drift_ptr->dev_time >> 32), (uint32_t)(avt_drift_ptr->dev_time),\
               (uint32_t)(avt_drift_ptr->init_avtimer_timestamp >> 32), (uint32_t)(avt_drift_ptr->init_avtimer_timestamp),\
               (uint32_t)(avt_drift_ptr->init_dev_time >> 32), (uint32_t)(avt_drift_ptr->init_dev_time));
   }
#endif

}


ADSPResult afe_port_set_custom_events_params(afe_dev_port_t *pDevPort, uint32_t param_id,
                                             int8_t *param_buffer_ptr, uint32_t param_size, afe_port_event_addr_info_t *pEvent_addr_info)
{
   ADSPResult result = ADSP_EOK;

   //Accept event register/unregister command only in not RUN state.
   if (AFE_PORT_STATE_RUN == pDevPort->port_state)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE Port Event Cfg ERROR, PORT is in RUN state already: %d !", pDevPort->port_state);
      return APR_ENOTREADY;
   }

   switch (param_id)
   {
      default:
         result = ADSP_EUNSUPPORTED;
         break;
   }

   return result;
}

ADSPResult afe_set_port_operating_latency_mode(afe_dev_port_t *pDevPort, int8_t *cfg, uint32_t payload_size)
{
  ADSPResult result = ADSP_EOK;
  afe_param_id_port_lat_mode_cfg_t *config_ptr    = (afe_param_id_port_lat_mode_cfg_t *)cfg;

  if(NULL == config_ptr)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE port latency mode payload config pointer is NULL");
    return ADSP_EBADPARAM;
  }

  if(payload_size < sizeof(afe_param_id_port_lat_mode_cfg_t))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Size of AFE port latency mode config payload too small");
    return ADSP_EFAILED;
  }

  if(config_ptr->minor_version > AFE_API_VERSION_PORT_LATENCY_MODE)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Unsupported minor version");
    return ADSP_EUNSUPPORTED;
  }

  pDevPort->latency_mode = config_ptr->mode;


  MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE port operating latency mode set to (DEFAULT-0 /LOW_LATENCY-1): %ld, port id : 0x%x",\
        pDevPort->latency_mode, pDevPort->intf);

  return result;
}


ADSPResult afe_port_set_device_module_params(afe_dev_port_t *pDevPort, uint32_t param_id,
                                             int8_t *param_buffer_ptr, uint32_t param_size)
{
   ADSPResult result = ADSP_EOK;

   uint16_t port_id = pDevPort->intf;
   boolean is_it_port_cfg_param = FALSE;


   //Accept Config command only in not RUN state.
   if ((AFE_PORT_STATE_RUN == pDevPort->port_state) &&
       (AFE_PARAM_ID_CH_STATUS_CONFIG != param_id))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFEAudioCfg ERROR,PORT is in RUN state already: %d !", pDevPort->port_state);
      return APR_ENOTREADY;
   }

   switch (param_id)
   {
      case AFE_PARAM_ID_I2S_CONFIG:
      case AFE_PARAM_ID_PCM_CONFIG:
      case AFE_PARAM_ID_TDM_CONFIG:
      case AFE_PARAM_ID_HDMI_CONFIG:
         if (IS_HDMI_OUTPUT_PORT_AFE_ID(port_id) || IS_HDMI_OVER_DP_PORT_AFE_ID(port_id))
         {
            result = afe_set_hdmi_output_cfg(pDevPort, param_buffer_ptr, param_size);
         }
         else
         {
            afe_group_device_state_struct_t *p_group_device = pDevPort->p_group_device_state;
            uint32_t prop_id;
            uint32_t prop_size;

            if (NULL != p_group_device)
            {
               /* This get properties will modify the i2s param and provide required port property so that i2s driver do not need to be aware of group device use case
                *  Group device use case :
                *  - i2s device config will not be same as i2s config param from port.
                *          data liine sd0 from port i2s config has to be converted to quad0 if the number of group channels is 4.
                *  - Port layer need to know the dma offset and number channels based on the all i2s config and group device config information.
                *  - This helper function does not save any state in i2s driver layer and only convert i2s config param accouting for i2s config and group device config.
                *     And also it give all the required information for the port such number of channels, bit width, ...
                *  - After this function, i2s config param is converted to match to gorup device config so that it can be used to set param for AFE_PARAM_ID_I2S_CONFIG.
                */

               prop_id = AFE_DEVICE_GROUP_PROP;
               prop_size = sizeof(afe_dev_grp_config_info_t);
               afe_dev_grp_config_info_t dev_grp_config_params;
               memset(&dev_grp_config_params, 0, sizeof(afe_dev_grp_config_info_t));
               dev_grp_config_params.param_buffer_ptr = param_buffer_ptr;
               dev_grp_config_params.param_size = param_size;
               dev_grp_config_params.group_id = p_group_device->group_id;

               if ((NULL != pDevPort->afe_drv_ftbl) && (NULL != pDevPort->afe_drv_ftbl->get_properties))
               {
                  if (ADSP_EOK != pDevPort->afe_drv_ftbl->get_properties(pDevPort->afe_drv, prop_id, (int8_t *)&dev_grp_config_params, prop_size))
                  {
                     return ADSP_EFAILED;
                  }
                  else
                  {
                     // deploy all the required port property including the dma offset.
                     pDevPort->q_format_shift_factor = dev_grp_config_params.port_prop.q_format_shift_factor;

                     pDevPort->dev_data_type = dev_grp_config_params.port_prop.port_data_type;
                     pDevPort->dev_bytes_per_channel = dev_grp_config_params.port_prop.bytes_per_channel;
                     pDevPort->dev_bit_width = dev_grp_config_params.port_prop.bit_width;
                     pDevPort->dev_channels = dev_grp_config_params.port_prop.channels;
                     pDevPort->dev_sample_rate = dev_grp_config_params.port_prop.sample_rate;

                     //if applicable, overwrite the port media type with the device media type.
                     //this is applicable for all the use cases where the port media type is not explicitly set by the HLOS client.
                     afe_port_copy_media_type_dev_to_port(pDevPort);

                     pDevPort->is_interleaved = dev_grp_config_params.port_prop.is_interleaved;
                     pDevPort->frame_idx = dev_grp_config_params.port_prop.frame_idx;
                  }
               }
               else
               {
                  MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "driver function table is null: dir: %d, port id: 0x%x", pDevPort->dir, pDevPort->intf);
               }

               /* Check if group is already running, then skip it */
               if (AFE_GROUP_DEVICE_STATE_RUN != p_group_device->group_state)
               {
                  if ((NULL != pDevPort->afe_drv_ftbl) && (NULL != pDevPort->afe_drv_ftbl->set_param))
                  {
                     result = pDevPort->afe_drv_ftbl->set_param(pDevPort->afe_drv, param_id, param_buffer_ptr, param_size);
                  }
                  else
                  {
                     MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "driver function table is null: dir: %d, port id: 0x%x", pDevPort->dir, pDevPort->intf);
                  }
               }
            }
            else
            {
               if ((NULL != pDevPort->afe_drv_ftbl) && (NULL != pDevPort->afe_drv_ftbl->set_param) && (NULL != pDevPort->afe_drv_ftbl->get_properties))
               {
                  if (ADSP_FAILED(result = pDevPort->afe_drv_ftbl->set_param(pDevPort->afe_drv, param_id, param_buffer_ptr, param_size)))
                  {
                     MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "driver failed to set_param: param_id: %d, port id: 0x%x", param_id, pDevPort->intf);
                     return result;
                  }
                  else
                  {

                     prop_id = AFE_DEVICE_PORT_PROP;
                     prop_size = sizeof(afe_dev_port_config_t);
                     afe_dev_port_config_t port_cfg_data;
                     memset(&port_cfg_data, 0, sizeof(afe_dev_port_config_t));

                     if (ADSP_FAILED(result = pDevPort->afe_drv_ftbl->get_properties(pDevPort->afe_drv, prop_id, (int8_t *)&port_cfg_data, prop_size)))
                     {
                        MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "driver failed to get port property: prop_id: %d, port id: 0x%x", prop_id, pDevPort->intf);
                        return result;
                     }
                     else
                     {
                /* this channel check not keeping in driver, to simplify the code for supporting 16Ch in group
                   device case. This will hit only in non group device case.*/
                /* This condition to remove once we add 16 ch support for TDM */
                if ((AFE_PARAM_ID_TDM_CONFIG == param_id) && (port_cfg_data.channels > 8) ) 
                {
                   MSG_2(MSG_SSID_QDSP6,DBG_ERROR_PRIO, "Invalid number of channels :%u  port:0x%x", port_cfg_data.channels,pDevPort->intf);
                   return ADSP_EFAILED;
                }
                        pDevPort->q_format_shift_factor = port_cfg_data.q_format_shift_factor;
                        pDevPort->dev_data_type = port_cfg_data.port_data_type;
                        pDevPort->dev_bytes_per_channel = port_cfg_data.bytes_per_channel;
                        pDevPort->dev_bit_width = port_cfg_data.bit_width;
                        pDevPort->dev_channels = port_cfg_data.channels;
                        pDevPort->dev_sample_rate = port_cfg_data.sample_rate;

                        //if applicable, overwrite the port media type with the device media type.
                        //this is applicable for all the use cases where the port media type is not explicitly set by the HLOS client.
                        afe_port_copy_media_type_dev_to_port(pDevPort);

                        pDevPort->is_interleaved = port_cfg_data.is_interleaved;
                     }
                  }
               }
               else
               {
                  MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "driver function table is null: dir: %d, port id: 0x%x", pDevPort->dir, pDevPort->intf);
               }
            }
         }
         is_it_port_cfg_param = TRUE;
         break;
      case AFE_PARAM_ID_RT_PROXY_CONFIG:
         result = afe_set_rt_proxy_cfg(pDevPort, param_buffer_ptr, param_size);
         is_it_port_cfg_param = TRUE;
         break;
      case AFE_PARAM_ID_SLIMBUS_CONFIG:
         result = afe_set_slimbus_cfg(pDevPort, param_buffer_ptr, param_size, TRUE, 0);
         is_it_port_cfg_param = TRUE;
         break;
      case AFE_PARAM_ID_INTERNAL_BT_FM_CONFIG:
         result = afe_set_int_bt_cfg(pDevPort, param_buffer_ptr, param_size);
         is_it_port_cfg_param = TRUE;
         break;
      case AFE_PARAM_ID_PSEUDO_PORT_CONFIG:
         result = afe_set_int_pseudo_port_cfg(pDevPort, param_buffer_ptr, param_size);
         is_it_port_cfg_param = TRUE;
         break;
      case AFE_PARAM_ID_SPDIF_CONFIG:
         result = afe_spdif_set_cfg(pDevPort, param_buffer_ptr, param_size);
         is_it_port_cfg_param = TRUE;
         break;
      case AFE_PARAM_ID_USB_AUDIO_CONFIG:
         result = afe_set_usba_cfg(pDevPort, param_buffer_ptr, param_size, param_id);
         is_it_port_cfg_param = TRUE;
         break;
      case AFE_PARAM_ID_USB_AUDIO_DEV_PARAMS:
      case AFE_PARAM_ID_USB_AUDIO_DEV_LPCM_FMT:
         result = afe_set_usba_cfg(pDevPort, param_buffer_ptr, param_size, param_id);
         is_it_port_cfg_param = FALSE;
         break;
      case AFE_PARAM_ID_PORT_SIGNAL_MISS_CONFIG:
         result = afe_err_handler_set_port_signal_miss_cfg(pDevPort, param_buffer_ptr, param_size, param_id);
         is_it_port_cfg_param = FALSE;
         break;
      case AFE_PARAM_ID_CH_STATUS_CONFIG:
         if (IS_SPDIF_PORT_AFE_ID(port_id))
         {
            afe_port_mutex_lock(pDevPort);
            result = afe_spdif_set_chstatus_cfg(pDevPort, param_buffer_ptr, param_size);
            afe_port_mutex_unlock(pDevPort);
         }
         else if (IS_HDMI_OUTPUT_PORT_AFE_ID(port_id) || IS_HDMI_OVER_DP_PORT_AFE_ID(port_id))
         {
            afe_port_mutex_lock(pDevPort);
            result = afe_hdmi_output_set_chstatus_cfg(pDevPort, param_buffer_ptr, param_size);
            afe_port_mutex_unlock(pDevPort);
         }
         else
         {
            return ADSP_EUNSUPPORTED;
         }
         break;
      case AFE_PARAM_ID_LPAIF_CLK_CONFIG:
         result = afe_set_audioif_clk_cfg(pDevPort->intf, param_buffer_ptr, param_size);
         break;
      case AFE_PARAM_ID_INTERNAL_DIGIATL_CDC_CLK_CONFIG:
      {
         result = afe_set_digital_codec_coreclk_cfg(pDevPort->intf, param_buffer_ptr, param_size);
         is_it_port_cfg_param = FALSE;
      }
         break;
      case AFE_PARAM_ID_SPDIF_CLK_CONFIG:
         if (IS_SPDIF_PORT_AFE_ID(port_id))
         {
            result = afe_spdif_set_clk_cfg(pDevPort, param_buffer_ptr, param_size);
         }
         else
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFEAudioCfg ERROR, invalid clock config for port: 0x%x !", (unsigned int)port_id);
            result = ADSP_EBADPARAM;
         }
         break;
      case AFE_PARAM_ID_LPASS_CORE_SHARED_CLOCK_CONFIG:
         result = afe_lpass_core_clk_set(param_buffer_ptr, param_size);
         break;
      case AFE_PARAM_ID_DEVICE_HW_DELAY:
         result = afe_set_dev_hw_delay_cfg(pDevPort, param_buffer_ptr, param_size);
         break;
      case AFE_PARAM_ID_TOPO_ID:
      {
         result = afe_set_topo_id(pDevPort, param_buffer_ptr, param_size);
         if (ADSP_EOK == result)
         {
            pDevPort->topo_rcvd_flag = TRUE;
         }
      }
         break;
	case AFE_PARAM_ID_PORT_LATENCY_MODE_CONFIG:
	{
	  result = afe_set_port_operating_latency_mode(pDevPort, param_buffer_ptr, param_size);
	}
	break;
      default:
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Unsupported param id 0x%lx for AFE_MODULE_AUDIO_DEV_INTERFACE in port:0x%x",
               param_id, (unsigned int)pDevPort->intf);
         result = ADSP_EUNSUPPORTED;
         break;
   }

   if ((ADSP_EOK == result) && is_it_port_cfg_param)
   {
      /* Change the port state to CONFIG */
      pDevPort->port_state =  AFE_PORT_STATE_CONFIG;

      /* If this is first config, change the group state to config as well */
      if ((NULL != pDevPort->p_group_device_state) &&
          (AFE_GROUP_DEVICE_STATE_STOP == pDevPort->p_group_device_state->group_state))
      {
         pDevPort->p_group_device_state->group_state = AFE_GROUP_DEVICE_STATE_CONFIG;
      }
   }

   return result;
}

ADSPResult afe_port_get_device_module_params(afe_dev_port_t *pDevPort, uint32_t param_id,\
                                                int8_t *param_buffer_ptr, int32_t param_buf_len, uint16_t *params_buffer_len_req_ptr)
{
   ADSPResult result = ADSP_EOK;

   switch (param_id)
   {
      case AFE_PARAM_ID_I2S_CONFIG:
      case AFE_PARAM_ID_PCM_CONFIG:
      case AFE_PARAM_ID_TDM_CONFIG:
      case AFE_PARAM_ID_HDMI_CONFIG:
         if (IS_HDMI_OUTPUT_PORT_AFE_ID(pDevPort->intf) || IS_HDMI_OVER_DP_PORT_AFE_ID(pDevPort->intf))
         {
            result = afe_hdmi_output_get_cfg(pDevPort, param_buffer_ptr, param_buf_len, params_buffer_len_req_ptr);
         }
         else
         {
            if ((NULL != pDevPort->afe_drv_ftbl) && (NULL != pDevPort->afe_drv_ftbl->get_param))
            {
               result = pDevPort->afe_drv_ftbl->get_param(pDevPort->afe_drv, param_id, param_buffer_ptr, param_buf_len, params_buffer_len_req_ptr);
            }
            else
            {
               MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "driver function table is null: dir: %d, port id: 0x%x", pDevPort->dir, pDevPort->intf);
            }
         }
         break;
      case AFE_PARAM_ID_RT_PROXY_CONFIG:
         result = afe_get_rt_proxy_cfg(pDevPort, param_buffer_ptr, param_buf_len, params_buffer_len_req_ptr);
         break;
      case AFE_PARAM_ID_SLIMBUS_CONFIG:
         result = afe_get_slimbus_cfg(pDevPort, param_buffer_ptr, param_buf_len, params_buffer_len_req_ptr);
         break;
      case AFE_PARAM_ID_USB_AUDIO_CONFIG:
      case AFE_PARAM_ID_USB_AUDIO_DEV_PARAMS:
      case AFE_PARAM_ID_USB_AUDIO_DEV_LPCM_FMT:
         result = afe_get_usba_cfg(pDevPort, param_buffer_ptr, param_buf_len, params_buffer_len_req_ptr, param_id);
         break;
      case AFE_PARAM_ID_INTERNAL_BT_FM_CONFIG:
         result = afe_get_int_bt_cfg(pDevPort, param_buffer_ptr, param_buf_len, params_buffer_len_req_ptr);
         break;
      case AFE_PARAM_ID_SPDIF_CONFIG:
         result =  afe_spdif_get_cfg(pDevPort, param_buffer_ptr, param_buf_len, params_buffer_len_req_ptr);
         break;
      case AFE_PARAM_ID_CH_STATUS_CONFIG:
         if (IS_SPDIF_PORT_AFE_ID(pDevPort->intf))
         {
            result =  afe_spdif_get_chstatus_cfg(pDevPort, param_buffer_ptr, param_buf_len, params_buffer_len_req_ptr);
         }
         else
         {
            return ADSP_EUNSUPPORTED;
         }
         break;
      case AFE_PARAM_ID_DEV_TIMING_STATS:
         result = afe_get_timing_stats(pDevPort, param_buffer_ptr, param_buf_len, params_buffer_len_req_ptr);
         break;
      default:
         result = ADSP_EUNSUPPORTED;
         break;
   }

   return result;
}

ADSPResult afe_set_dev_hw_delay_cfg(afe_dev_port_t *pDevPort, int8_t *cfg, uint32_t payload_size)
{
   ADSPResult result = ADSP_EOK;

   // choose the appropriate config version
   uint32_t dev_hw_delay_cfg_version = (uint32_t) * (cfg);
   if (dev_hw_delay_cfg_version > AFE_API_VERSION_DEVICE_HW_DELAY)
   {
      dev_hw_delay_cfg_version = AFE_API_VERSION_DEVICE_HW_DELAY;
   }

   if (0x1 == dev_hw_delay_cfg_version)
   {
      if (payload_size < sizeof(afe_param_id_device_hw_delay_t))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Device hw delay cfg fail: Invalid payload size: %ld, port_id: 0x%x", payload_size, pDevPort->intf);
         result = ADSP_EBADPARAM;
      }
      else
      {
         afe_param_id_device_hw_delay_t	*dev_hw_delay_ptr = (afe_param_id_device_hw_delay_t *)cfg;
         pDevPort->dev_hw_delay_us = dev_hw_delay_ptr->delay_in_us;
         MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO, "Device HW delay %ld, port_id: 0x%x", pDevPort->dev_hw_delay_us, pDevPort->intf);
      }
   }
   return result;
}

void afe_device_gettime_per_sample(uint32 sample_rate, uint32 *p_time, int32 *p_format)
{
   int32 i;
   for (i = 0; i < NUM_SAMPLING_RATES; i++)
   {
      if (sample_rate == afe_device_sampling_rate_info[i][0])
      {
         *p_time = afe_device_sampling_rate_info[i][1];
         *p_format = 25;
      }
   }
}

ADSPResult afe_port_delete_timer(afe_dev_port_t *pDevPort)
{
   ADSPResult  result = ADSP_EOK;

   /* Delete the timer object */
   if (pDevPort->timer_ptr != NULL)
   {
      if (ADSP_EOK != (result = afe_time_srvr_timer_delete(pDevPort->timer_ptr)))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to de-allocate timer object, port_id: 0x%x, result: %d",
               pDevPort->intf, result);
      }

      /* Free up the timer memory */
      qurt_elite_memory_free(pDevPort->timer_ptr);
      pDevPort->timer_ptr = NULL;
   }
   return result;
}

ADSPResult afe_port_create_and_start_timer(afe_dev_port_t *pDevPort,
                                           qurt_elite_timer_duration_t timer_type,
                                           uint64_t duration)
{
   ADSPResult                 result = ADSP_EOK;
   qurt_elite_timer_t         *timer_ptr;

   /* Allocate memory for the timer object */
   if (NULL == (timer_ptr = (qurt_elite_timer_t *)qurt_elite_memory_malloc(sizeof(qurt_elite_timer_t), QURT_ELITE_HEAP_DEFAULT)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to allocate timer object, port_id: 0x%x", pDevPort->intf);
      return ADSP_ENOMEMORY;
   }

   /* Create timer as per input type and duration */
   if (ADSP_SUCCEEDED(result = afe_time_srvr_timer_create(timer_ptr, timer_type, &(pDevPort->dma_signal))))
   {
      pDevPort->timer_ptr = timer_ptr;

      if (QURT_ELITE_TIMER_PERIODIC == timer_type)
      {
         if (ADSP_EOK != (result = afe_time_srvr_timer_periodic_start(timer_ptr, duration)))
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to start the periodic timer, port_id: 0x%x", pDevPort->intf);
         }
         pDevPort->init_port_clock = 0;
      }
      else if (QURT_ELITE_TIMER_ONESHOT_ABSOLUTE == timer_type)
      {
         pDevPort->init_port_clock = afe_time_srvr_timer_get_time((avtimer_drv_handle_t)timer_ptr);
         duration += pDevPort->init_port_clock;
         if (ADSP_EOK != (result = afe_time_srvr_timer_oneshot_start_absolute(timer_ptr, duration)))
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to start the one-short timer, port_id: 0x%x", pDevPort->intf);
         }
      }

      /* Delete timer object if any error occurs in starting the timer */
      if (ADSP_EOK != result)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Deleting timer obj after start failure, port_id: 0x%x", pDevPort->intf);
         afe_port_delete_timer(pDevPort);
      }
      else
      {
         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Timer create and start success, timer_type: %lu, port_id: 0x%x",
               timer_type, pDevPort->intf);
      }
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to create timer object, port_id :0x%x", pDevPort->intf);
      qurt_elite_memory_free(timer_ptr);
   }

   return result;
}

ADSPResult afe_dev_unmute(afe_dev_port_t *pDevPort, void *p_ctx)
{
   ADSPResult result = ADSP_EOK;

   if (IS_HDMI_OVER_DP_PORT_AFE_ID(pDevPort->intf))
   {
      result = afe_hdmi_over_dp_unmute(pDevPort, p_ctx);
   }

   return result;
}


ADSPResult afe_dev_mute(afe_dev_port_t *pDevPort, void *p_ctx)
{
   ADSPResult result = ADSP_EOK;

   if (IS_HDMI_OVER_DP_PORT_AFE_ID(pDevPort->intf))
   {
      result = afe_hdmi_over_dp_mute(pDevPort, p_ctx);
   }

   return result;
}



ADSPResult afe_svc_cmd_request_lpass_resources(aud_stat_afe_svc_t *pMe, elite_apr_packet_t *apr_pkt_ptr)
{
  ADSPResult result= ADSP_EOK;

  if (!apr_pkt_ptr)
  {
    return ADSP_EBADPARAM;
  }
   /*Only if the basic APR packet validations are failing, then this function
   sends the response to apr. In rest of the cases respective resource handler will send the
   response and free the incoming apr packet*/
  if (elite_apr_if_get_payload_size(apr_pkt_ptr) < sizeof(afe_cmd_request_lpass_resources))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Apr payload of insufficient size received for AFE_SVC_CMD_REQUEST_LPASS_RESOURCES");
    result = ADSP_EUNEXPECTED;
    goto afe_request_lpass_resources_error_1;
  }

  /* parse command payload */
  afe_cmd_request_lpass_resources *cmd_payload_ptr;
  cmd_payload_ptr = (afe_cmd_request_lpass_resources *)(elite_apr_if_get_payload_ptr(apr_pkt_ptr));

  if (NULL ==cmd_payload_ptr) 
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "apr payloadptr is NULL");
    result = ADSP_EUNEXPECTED;
    goto afe_request_lpass_resources_error_1;
  }

  switch (cmd_payload_ptr->resource_id) 
  {
    case AFE_LPAIF_DMA_RESOURCE_ID:
    {
      result = afe_request_lpass_dma_resources_by_remote_client_handler((void *)apr_pkt_ptr);
      break;
    }
    default:
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Invalid Resource id 0x%x",cmd_payload_ptr->resource_id);
	  result= ADSP_EFAILED;
      goto afe_request_lpass_resources_error_1;
      break;
    }
  }
  if(ADSP_FAILED(result))
  {
    MSG_1(MSG_SSID_QDSP6,DBG_ERROR_PRIO, "Resouce allocation failed for resource id:0x%x",cmd_payload_ptr->resource_id);
  }
  return result;

  /* Only if the basic APR packet validations are failing, then this function
   sends the response to apr. In rest of the cases respective resource handler will send the
   response and free the incoming apr packet*/
  afe_request_lpass_resources_error_1:
  //ack back the apr packet
  (void)elite_apr_if_end_cmd(pMe->apr_handle, apr_pkt_ptr, result);
  return result;
}




ADSPResult afe_svc_cmd_release_lpass_resources(aud_stat_afe_svc_t *pMe, elite_apr_packet_t *apr_pkt_ptr)
{
  ADSPResult result= ADSP_EOK;

  if (!apr_pkt_ptr)
  {
    return ADSP_EBADPARAM;
  }

  if (elite_apr_if_get_payload_size(apr_pkt_ptr) < sizeof(afe_cmd_release_lpass_resources))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Apr payload of insufficient size received for AFE_SVC_CMD_RELEASE_LPASS_RESOURCES");
    result = ADSP_EUNEXPECTED;
    goto afe_release_lpass_resources_error_1;
  }

  /* parse command payload */
  afe_cmd_release_lpass_resources *cmd_payload_ptr;
  cmd_payload_ptr = (afe_cmd_release_lpass_resources *)(elite_apr_if_get_payload_ptr(apr_pkt_ptr));

  if (NULL ==cmd_payload_ptr) 
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "apr payloadptr is NULL");
    result = ADSP_EUNEXPECTED;
    goto afe_release_lpass_resources_error_1;
  }

  switch (cmd_payload_ptr->resource_id) 
  {
    case AFE_LPAIF_DMA_RESOURCE_ID:
    {
      result = afe_release_lpass_dma_resources_by_remote_client_handler((void *)apr_pkt_ptr);
      break;
    }
    default:
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Invalid Resource id 0x%x",cmd_payload_ptr->resource_id);
	  result= ADSP_EFAILED;
      goto afe_release_lpass_resources_error_1;
      break;
    }
  }
  if(ADSP_FAILED(result))
  {
    MSG_1(MSG_SSID_QDSP6,DBG_ERROR_PRIO, "Resouce release failed for resource id:0x%x",cmd_payload_ptr->resource_id);
  }
  return result;

  /* Only if the basic APR packet validations are failing, then this function
   sends the response to apr. In rest of the cases respective resource handler will send the
   response and free the incoming apr packet*/
  afe_release_lpass_resources_error_1:
  //ack back the apr packet
  (void)elite_apr_if_end_cmd(pMe->apr_handle, apr_pkt_ptr, result);
  return result;
}
