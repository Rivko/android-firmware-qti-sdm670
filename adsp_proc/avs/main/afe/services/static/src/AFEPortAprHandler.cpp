/*========================================================================
   This file contains AFE APR cmd handler implementations

  Copyright (c) 2009-2012, 2017 Qualcomm Technologies, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

  $Header: //components/rel/avs.adsp/2.8.5/afe/services/static/src/AFEPortAprHandler.cpp#4 $
 ====================================================================== */

/*==========================================================================
  Include files
========================================================================== */
#include "AFEPortManagement.h"
#include "AFECodecService.h"
#include "AFEMmpm.h"
#include "AFESwMad.h"
#include "AFE_private_defs_i.h"
#include "AFEFeedback.h"
#include "adsp_audproc_api.h"
#include "AFESvc_topo_db.h"
#include "AFEDutyCycleDriver.h"
#include "AFESvc_topo.h"
#include "topo_utils.h"
#include "AFESidetoneIIR.h"
#include "AFEDtmf.h"
#include "AFEDsatDriver.h"
#include "AFELimiter.h"
#include "AFEAanc.h"
#include "AFEMmpm_i.h"
#include "AFETdmSlot.h"
#include "AFEInternal.h"
#include "AFEErrorHandler.h"
#include "AFEEncoder_If.h"
#include "AFEEncDecCmn_If.h"
#include "AFEDecoder_If.h"
#include "adsp_afe_service_modules_params.h"
#include "AFEHdmiOutputDrv.h"

#define IS_ALIGN_4_BYTE(a) (!(a & 0x3))

/*==========================================================================
  Functions
========================================================================== */
/**
 * This function is command handler for afe_svc_apr_msg_handler
 *
 * @param[in] pAudStatAfeSvc, pointer to AfeSvc instance.
 * @param[in] pMsg, pointer to message.
 * @return the ADSP_EOK error code
 */
ADSPResult afe_svc_apr_msg_handler (void* pAudStatAfeSvc, elite_msg_any_t* pMsg)
{
  ADSPResult          result = ADSP_EOK;
  elite_apr_packet_t  *pPkt;
  afe_dev_port_t      *pDevPort;
  uint16_t            unPortId;

  pPkt = (elite_apr_packet_t *)(pMsg->pPayload);

  /* except for DTMF command, ACK for the command is sent
      from the respective handler functions.For DTMF, it's handled here.*/
  MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFESvc: Executing APR cmd with opcode: %8lx",  pPkt->opcode);

  switch (pPkt->opcode)
  {
    case AFE_PORT_CMD_DEVICE_START:
    case AFE_PORT_CMD_DEVICE_STOP:
    case AFE_PORT_CMD_SET_PARAM_V2:
    case AFE_PORT_CMD_SET_PARAM_V3:
    case AFE_PORT_CMD_GET_PARAM_V2:
    case AFE_PORT_CMD_GET_PARAM_V3:
    {
      unPortId = *((uint16 *) elite_apr_if_get_payload_ptr(pPkt));

      //find the corresponding port and forward it.
      if (ADSP_EOK == (result = afe_svc_get_port(unPortId, &pDevPort)))
      {

        MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
              "AFESvc: Executing APR cmd opcode: 0x%lX, port_id: 0x%X",  pPkt->opcode, unPortId);

        /* The command handler should effectively place the mutex */
        result = afe_port_apr_msg_handler(pDevPort, pMsg);
      }
      else
      {
        MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
              "AFESvc: Can't find port 0x%x for the Apr Cmd. Res = %d!!", unPortId, result);
        elite_apr_if_end_cmd( ((aud_stat_afe_svc_t*)pAudStatAfeSvc)->apr_handle, pPkt, result);
        goto _bail_out;
      }
    }
    break;
    case AFE_PORT_CMD_DEVICES_RESTART:
    {
      result = afe_ports_apr_dma_restart(&(((aud_stat_afe_svc_t*)pAudStatAfeSvc)->apr_handle), pPkt);
    }
    break;
    case AFE_PORTS_CMD_DTMF_CTL:
    {
      result = afe_svc_set_dtmf_config(pMsg);

      /* Return the APR MSG */
      if(ADSP_FAILED(result))
      {
        elite_apr_if_end_cmd(((aud_stat_afe_svc_t*)pAudStatAfeSvc)->apr_handle, pPkt, APR_EFAILED);
      }
      else
      {
        elite_apr_if_end_cmd(((aud_stat_afe_svc_t*)pAudStatAfeSvc)->apr_handle, pPkt, APR_EOK);
      }
      break;
    }
    case AFE_SERVICE_CMD_SHARED_MEM_MAP_REGIONS:
    {
      result = elite_mem_shared_memory_map_regions_cmd_handler(afe_svc_global_ptr->afe_memory_map_client,
                                                               afe_svc_get_apr_handle(),pPkt,
                                                               AFE_SERVICE_CMDRSP_SHARED_MEM_MAP_REGIONS);
    }
    break;
    case AFE_SERVICE_CMD_SHARED_MEM_UNMAP_REGIONS:
    {
      result = elite_mem_shared_memory_un_map_regions_cmd_handler(afe_svc_global_ptr->afe_memory_map_client,
                                                                  afe_svc_get_apr_handle(), pPkt);
    }
    break;
    case AFE_SVC_CMD_SET_PARAM:
    case AFE_SVC_CMD_SET_PARAM_V2:
    {
      result = afe_svc_apr_set_param_handler(pPkt);
    }
    break;
    case AFE_CMD_REMOTE_LPASS_CORE_HW_VOTE_REQUEST:
    {
      result = afe_remote_lpass_core_hw_vote_reqeust((aud_stat_afe_svc_t*)pAudStatAfeSvc, pPkt);
    }
    break;
    case AFE_CMD_REMOTE_LPASS_CORE_HW_DEVOTE_REQUEST:
    {
      result = afe_remote_lpass_core_hw_devote_reqeust((aud_stat_afe_svc_t*)pAudStatAfeSvc, pPkt);
    }
    break;
    case AFE_CMD_ADD_TOPOLOGIES:
    {
      result = afe_add_topologies_handler(pPkt);
      break;
    }
    case AFE_CMD_REQUEST_LPASS_RESOURCES:
    {
      result = afe_svc_cmd_request_lpass_resources((aud_stat_afe_svc_t*)pAudStatAfeSvc, pPkt);
    }
    break;
    case AFE_CMD_RELEASE_LPASS_RESOURCES:
    {
      result = afe_svc_cmd_release_lpass_resources((aud_stat_afe_svc_t*)pAudStatAfeSvc, pPkt);
    }
    break;

    default:
    {
      result = ADSP_EUNSUPPORTED;
      elite_apr_if_end_cmd( ((aud_stat_afe_svc_t*)pAudStatAfeSvc)->apr_handle, pPkt, result);
    }
    break;
  }

  _bail_out:
  afe_svc_print_status_message(result);
  return result;
}

/**
 * This function is to de-allocate port related resources
 *
 * @param[in] pDevPort,  pointer to port instance
 * @return the ADSP_EOK error code
 */
ADSPResult afe_port_dealloc_resources(afe_dev_port_t *pDevPort)
{
  ADSPResult result = ADSP_EOK, rc = ADSP_EOK;

  if(!pDevPort)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Null afe port pointer");
    return ADSP_EFAILED;
  }

  //Destroy enc/dec svc and all the associated Qs etc
  afe_destroy_enc_dec(pDevPort);

  /* De-allocate the device interface resources */
  result = afe_dev_dealloc_resources(pDevPort);

  if (NULL != pDevPort->sw_duty_cycle_data_ptr)
  {
    // Here no need to acquire mutex, at this point no timer interrupt comes as we cancel
    // active configurations above. If any had come before cancelling that will also get served
    // after reelasing mutex above.DC thread just returns from that thread as
    // init flag set to false. So this thread only will access duty cycle structures.
    result |=afe_port_sw_duty_cycle_dealloc_resources(pDevPort);
  }

  if (NULL != pDevPort->hw_mad_ptr)
  {
    afe_port_mutex_lock(pDevPort);
    result |= afe_port_hw_mad_dealloc_resources((void *)pDevPort);
    afe_port_mutex_unlock(pDevPort);
  }

  //dealloc the SW MAD back ground handler
  if (NULL != pDevPort->sw_mad_ptr)
  {
    // SW MAD dealloc resources should be here only and not part of afe_port_de_init_at_port_stop() due to optimizations
    result |= afe_port_sw_mad_dealloc_resources(pDevPort);
  }

  if(ADSP_EOK != ( rc = afe_mmpm_voting(pDevPort, AFE_MMPM_DEFAULT_TOPOLOGY, AFE_MMPM_DEVOTE,TRUE)))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE MMPM port deregistering with MMPM is failed, port-id: 0x%x", pDevPort->intf);
    result |= rc;
  }

  MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE DMA STOP CMD Response: port id: 0x%X, result: 0x%x", pDevPort->intf, result);

  if(ADSP_EOK != result)
  {
    result = ADSP_EFAILED;
  }
  return result;
}

/**
 * This function is to allocate port related resources
 *
 * @param[in] pDevPort,  pointer to port instance
 * @return the ADSP_EOK error code
 */
ADSPResult afe_port_alloc_resources(afe_dev_port_t  *pDevPort)
{
  ADSPResult result = ADSP_EOK;

  if(!pDevPort)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Null afe port pointer");
    return ADSP_EFAILED;
  }

  /* Set the samples, only updates the port data structure */
  result = afe_port_get_frame_size(pDevPort->latency_mode, pDevPort->sample_rate, &pDevPort->int_samples_per_period);

  if(result != ADSP_EOK)
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " AFEPortAprHandler : Invalid sampling rate detected,Portid::0x%x",pDevPort->intf);
    return result;
  }

  if (ADSP_EOK != (result = afe_mmpm_voting(pDevPort, AFE_MMPM_DEFAULT_TOPOLOGY,
                                            AFE_MMPM_VOTE, TRUE)))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE MMPM request failed and port is deregistering with MMPM, port-id: 0x%x", pDevPort->intf);

    return ADSP_EFAILED;
  }

  /* Allocate/init SW MAD resources if this configured by HLOS */
  if(NULL != pDevPort->sw_mad_ptr)
  {
    /* SW MAD alloc resources should be called explicitly here only and not
         inside afe_port_init_at_port_start() due to optimization */
    if (ADSP_EOK != (result = afe_port_sw_mad_alloc_resources(pDevPort)))
    {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Fail to initialize SW MAD module, port_id : 0x%X", pDevPort->intf);

      return ADSP_EFAILED;
    }
  }

  /* Allocate SWDC resources if this configured by HLOS */
  if (NULL != pDevPort->sw_duty_cycle_data_ptr)
  {
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "SW DC is enabled and skipping the device START cmd processing, port_id : 0x%X", pDevPort->intf);

    if (ADSP_EOK != (result = afe_port_sw_duty_cycle_alloc_resources(pDevPort)))
    {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Failed to allocate sw duty cycle module resources, port_id : 0x%X", pDevPort->intf);

      return ADSP_EFAILED;
    }
  }

  /* Allocate HW MAD resources if this configured by HLOS */
  if (NULL != pDevPort->hw_mad_ptr)
  {
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "HW MAD is enabled and skipping the device START cmd processing, port_id : 0x%X",\
          pDevPort->intf);

    if (ADSP_EOK != (result = afe_port_hw_mad_alloc_resources(pDevPort)))
    {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Fail to allocated HW MAD module resources, port_id : 0x%X",\
            pDevPort->intf);
      return ADSP_EFAILED;
    }
  }

  /* ALlocate resources for the port, dynamic thread, av timer */
  if (ADSP_EOK != (result = afe_dev_alloc_resources(pDevPort)))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Failed to allocate dev interface resources, port_id : 0x%X",\
          pDevPort->intf);

    return result;
  }

  //At this point, setup enc/dec (if applicable)
  result = afe_create_enc_dec(pDevPort);
  if(ADSP_EOK != result)
  {
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
          "AFECmdDmaStart: port_id: 0x%x, error in afe_create_enc_dec, error: 0x%x", pDevPort->intf, result);
    return result;
  }

  return result;
}

/**
 * This function is to initialize modules and to start the device
 *
 * @param[in] pDevPort,  pointer to port instance
 * @return    ADSP_EOK error code
 */
ADSPResult afe_port_modules_init_and_dev_start(afe_dev_port_t  *pDevPort)
{
  ADSPResult  result = ADSP_EOK;

  if (!pDevPort)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Null afe port pointer");
    return ADSP_EFAILED;
  }

  if (NULL != pDevPort->sw_duty_cycle_data_ptr)
  {
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "SW DC is enabled and skipping the device START cmd processing, port_id : 0x%X",
          pDevPort->intf);

    if (ADSP_EOK != (result = afe_port_sw_duty_cycle_init(pDevPort)))
    {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Fail to initialize sw duty cycle module, port_id : 0x%X", pDevPort->intf);
      return ADSP_EFAILED;
    }
  }

  if(NULL != pDevPort->hw_mad_ptr)
  {
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "HW MAD is enabled and skipping the device START cmd processing, port_id : 0x%X", pDevPort->intf);
    if (ADSP_EOK != (result = afe_port_hw_mad_init(pDevPort)))
    {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Fail to initialize HW MAD module, port_id : 0x%X", pDevPort->intf);
      return ADSP_EFAILED;
    }
  }

  /* If hwmad or sw duty cycle enabled, skip starting the dev interface */
  if((NULL != pDevPort->hw_mad_ptr) || (NULL != pDevPort->sw_duty_cycle_data_ptr))
  {
    return ADSP_ECONTINUE;
  }

  if (ADSP_EOK == (result = afe_port_init_at_port_start(pDevPort)))
  {
    if (ADSP_EOK == (result = afe_dev_start(pDevPort)))
    {
      // Update the group delays for the connected clients
      afe_port_set_group_delay(pDevPort);

      /* If all the init/start success, update the port state to RUN */
      pDevPort->port_state = AFE_PORT_STATE_RUN;
    }
  }
  else
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "START CMD FAIL, failed to init port modules, port_id: 0x%X", pDevPort->intf);
  }

  return result;
}

/**
 * This function is to process DMA START command
 *
 * @param[in] pDevPort,  pointer to port instance
 * @param[in] pkt_ptr, msg to the port cmdQ
 * @return the ADSP_EOK error code
 */
ADSPResult afe_port_apr_dma_start (afe_dev_port_t* pDevPort, elite_apr_packet_t* pkt_ptr)
{
  ADSPResult result = ADSP_EOK;

  if (!pDevPort)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFECmdDmaStart: null AFE port pointer");
    return ADSP_EFAILED;
  }

  if (pkt_ptr)
  {
    if (elite_apr_if_get_payload_size(pkt_ptr) != sizeof(afe_port_cmd_device_start_t) )
    {
      result = APR_EBADPARAM;
      goto _dma_start_bail_out;
    }
  }

  if (AFE_PORT_STATE_CONFIG != pDevPort->port_state)
  {
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFECmdDmaStart: already started or config not done, port_state: %ud, port_id: 0x%x",
          pDevPort->port_state, pDevPort->intf);
    result = ADSP_EALREADY;
    goto _dma_start_bail_out;
  }

  MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFECmdDmaStart: port_id: 0x%x, port [fs, ch, bw, Bps]:[%lu, %lu, %u, %u]",
        pDevPort->intf, pDevPort->sample_rate,  pDevPort->channels, pDevPort->bit_width, pDevPort->bytes_per_channel);
  MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFECmdDmaStart: port_id: 0x%x, dev [fs, ch, bw, Bps]:[%lu, %lu, %u, %u]",
        pDevPort->intf, pDevPort->dev_sample_rate,  pDevPort->dev_channels, pDevPort->dev_bit_width, pDevPort->dev_bytes_per_channel);

  /* Following are the conditions to check,if port media type is set.
       1. In case of Tx direction,  both device and port sampling rates must be same. Currently there is no requirement.
       2. In case of Rx direction,  device and  port sampling rates must be integer multiples*/
  if(TRUE == pDevPort->is_port_media_type_set)
  {
    if (TX_DIR == pDevPort->dir) 
    {
      if (pDevPort->sample_rate != pDevPort->dev_sample_rate) 
      {
        MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "For Tx port 0x%x, port fs %lu and  dev fs  %lu must be equal",
              pDevPort->intf, pDevPort->sample_rate, pDevPort->dev_sample_rate);
        goto _dma_start_bail_out;
      }
    }
    /* here we need to allow the port and device sampling rates of same family, other combination  is not supported*/
    else if ((afe_is_sample_rate_fract(pDevPort->sample_rate) ^ afe_is_sample_rate_fract(pDevPort->dev_sample_rate)))
    {
      MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "port fs %lu and  dev fs  %lu are not integer multiples on port 0x%x",
            pDevPort->sample_rate, pDevPort->dev_sample_rate, pDevPort->intf);
      result = ADSP_EBADPARAM;
      goto _dma_start_bail_out;
    }
  }

  /* If resource allocation fails, abort the start processing */
  if(ADSP_EOK != (result = afe_port_alloc_resources(pDevPort)))
  {
    goto _dma_start_bail_out;
  }

  afe_port_mutex_lock(pDevPort);
  //Attempt to open dangergen handle and ignore all failures
  afe_port_dangergen_open(pDevPort);

  /* ADSP_ECONTINUE will be for h/w mad or s/w duty enabled case */
  if(ADSP_ECONTINUE == (result = afe_port_modules_init_and_dev_start(pDevPort)))
  {
    result = ADSP_EOK;
    afe_port_mutex_unlock(pDevPort);
    goto _dma_start_bail_out;
  }

  /* Execution will fall through if dev start failed or success */
  afe_port_mutex_unlock(pDevPort);

  _dma_start_bail_out:
  /* Either the port module init OR device start failed. */
  if (ADSP_EOK != result)
  {
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "START CMD FAIL, closing the device, port_id: 0x%X, result: 0x%x",
          pDevPort->intf, result);

    /* execute stop cmd sequence */
    afe_port_apr_dma_stop(pDevPort, NULL);
  }

  return(result);
}

/**
 * This function is to de-initialize modules and to stop the device
 *
 * @param[in]  pDevPort,  pointer to port instance
 * @return     ADSP_EOK error code
 */
ADSPResult afe_port_modules_deinit_and_dev_stop(afe_dev_port_t *pDevPort)
{
  ADSPResult result = ADSP_EOK;

  if (!pDevPort)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Null afe port pointer");
    return ADSP_EFAILED;
  }

  afe_port_mutex_lock(pDevPort);

  /**< In order to avoid race condition with duty cycle thread set dc enable= false and cancel active config.
   *   case 1: if duty cycle thread doing setup channels and stop cmd sent by HLOS. AFES will be
   *           wait until mutex released and close the device from here.
   *   case 2: First Stop cmd sent by HLOS and while executing this stop if timer interrupt arrives.
   *           DC thread waits on mutex till AFES moves to STOP state.Once mutex released by AFES thread
   *           DC thread will execute and come out of function as dc enable flag = false.
   */
  if (NULL != pDevPort->sw_duty_cycle_data_ptr)
  {
    afe_port_disable_sw_duty_cycle(pDevPort);
  }

  if ((AFE_PORT_STATE_STOP == pDevPort->port_state))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFECmdDmaStop ERROR, already STOP, port_id: 0x%x", pDevPort->intf);
    result = ADSP_EOK;
    afe_port_mutex_unlock(pDevPort);

    /**<port state as AFE_PORT_STATE_STOP, does not mean that all the resources were
     * de-allocated and modules were de-initialized.
     *
     * For example, if, afe_port_sw_duty_cycle_reset_handler(), changes port state
     * from RUN to STOP, and it won't take care of deallocating of port
     * resources or de-initializing of modules. Actual apr dma stop, should need to
     * deallocate the resources and de-init the modules.*/
    goto _bail_out_module_deinit;
  }

  /* Set the port state to STOP */
  pDevPort->port_state = AFE_PORT_STATE_STOP;

  afe_port_mutex_unlock(pDevPort);

  /* Stop the device interface */
  result = afe_dev_stop(pDevPort);

  /* De-init modules */
  result |= afe_port_de_init_at_port_stop(pDevPort);

  _bail_out_module_deinit:

  if(NULL != pDevPort->sw_duty_cycle_data_ptr)
  {
    /**< Here no need to acquire mutex, at this point no timer interrupt comes as we cancel
     *   active configurations above. If any had come before cancelling that will also get served
     *   after reelasing mutex above.DC thread just returns from that thread as
     *   init flag set to false. So this thread only will access duty cycle structures.*/

    afe_port_sw_duty_cycle_deinit(pDevPort);
  }

  if(NULL != pDevPort->hw_mad_ptr)
  {
    afe_port_mutex_lock(pDevPort);
    result |= afe_port_hw_mad_deinit((void *)pDevPort);
    afe_port_mutex_unlock(pDevPort);
  }

  return result;
}

/**
 * This function is to process DMA STOP command
 *
 * @param[in] pDevPort,  pointer to port instance
 * @param[in] pkt_ptr, msg to the port cmdQ
 * @return the ADSP_EOK error code
 */
ADSPResult afe_port_apr_dma_stop(afe_dev_port_t* pDevPort,elite_apr_packet_t* pkt_ptr)
{
  ADSPResult result = ADSP_EOK;

  if (!pDevPort)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Null afe port pointer");
    return ADSP_EFAILED;
  }

  if (pkt_ptr)
  {
    if (elite_apr_if_get_payload_size(pkt_ptr) != sizeof(afe_port_cmd_device_stop_t))
    {
      result = APR_EBADPARAM;
      goto _dma_stop_bail_out;
    }
  }

  /* Stop the port and de-init modules */
  result |= afe_port_modules_deinit_and_dev_stop(pDevPort);

  afe_port_dangergen_close(pDevPort);

  /* De allocate port resources */
  result |= afe_port_dealloc_resources(pDevPort);

  /* resetting the media type to default */
  afe_port_reset_media_type(pDevPort);

  _dma_stop_bail_out:
  return result;
}

/**
 * This function is to process DMA RESTART command
 *
 * @param[in] pDevRxPort,  pointer to rx port instance
 * @param[in] pDevTxPort,  pointer to tx port instance
 * @param[in] pkt_ptr, msg to the port cmdQ
 * @return the ADSP_EOK error code
 * @NOTE: This is only for i2s port and restart the interface with new sampling rate. And also it does not turn off clk and ws signal output.
 */
ADSPResult afe_ports_apr_dma_restart (elite_apr_handle_t *apr_handle_ptr, elite_apr_packet_t *pkt_ptr)
{
  ADSPResult                     result = ADSP_EOK;
  afe_port_cmd_devices_restart_t         *pPortsReStartCmd;
  int32_t                        rc;
  bool_t rx_port_mutex_locked = FALSE;
  bool_t tx_port_mutex_locked = FALSE;
  afe_dev_port_t* pDevRxPort = NULL;
  afe_dev_port_t* pDevTxPort = NULL;

  MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "afe_ports_apr_dma_restart:");

  if(NULL == pkt_ptr)
  {
    result = ADSP_EBADPARAM;
    goto _dma_restart_bail_out;
  }

  /* Validate the inbound payload size. */
  if (elite_apr_if_get_payload_size(pkt_ptr) < sizeof(afe_port_cmd_devices_restart_t) )
  {
    result = ADSP_EBADPARAM;
    goto _dma_restart_bail_out;
  }

  pPortsReStartCmd = (afe_port_cmd_devices_restart_t *) elite_apr_if_get_payload_ptr(pkt_ptr);

  if(IS_I2S_PORT_AFE_ID(pPortsReStartCmd->rx_port_id))
  {
    //find the corresponding port and forward it.
    if (ADSP_EOK != (result = afe_svc_get_port(pPortsReStartCmd->rx_port_id, &pDevRxPort)))
    {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFECmdDmaReStart: Can't find port 0x%x for the Apr Cmd. Res = %d!!\n", pPortsReStartCmd->rx_port_id, result);
      result = ADSP_EBADPARAM;
      goto _dma_restart_bail_out;

    }
    else if(RX_DIR != pDevRxPort->dir)
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFECmdDmaReStart: ERROR, invalid rx port_id: 0x%X", pDevRxPort->intf);
      result = ADSP_EBADPARAM;
      goto _dma_restart_bail_out;
    }
  }

  if(IS_I2S_PORT_AFE_ID(pPortsReStartCmd->tx_port_id))
  {
    //find the corresponding port and forward it.
    if (ADSP_EOK != (result = afe_svc_get_port(pPortsReStartCmd->tx_port_id, &pDevTxPort)))
    {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFECmdDmaReStart: Can't find port 0x%x for the Apr Cmd. Res = %d!!\n", pPortsReStartCmd->tx_port_id, result);
      result = ADSP_EBADPARAM;
      goto _dma_restart_bail_out;

    }
    else if(TX_DIR != pDevTxPort->dir)
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFECmdDmaReStart: ERROR, invalid tx port_id: 0x%X", pDevTxPort->intf);
      result = ADSP_EBADPARAM;
      goto _dma_restart_bail_out;
    }
  }

  if((NULL == pDevRxPort) && (NULL == pDevTxPort))
  {
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFECmdDmaReStart: ERROR, invalid rx[0x%X] and tx port_id[0x%X]", pPortsReStartCmd->rx_port_id, pPortsReStartCmd->tx_port_id);
    result = ADSP_EBADPARAM;
    goto _dma_restart_bail_out;
  }

  if(NULL != pDevRxPort)
  {
    if (AFE_PORT_STATE_RUN != pDevRxPort->port_state)
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFECmdDmaReStart ERROR, not running state, port_id: 0x%X", pDevRxPort->intf);
      result = ADSP_ENOTREADY;
      goto _dma_restart_bail_out;
    }

    if(pDevRxPort->sample_rate == pPortsReStartCmd->sample_rate)
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFECmdDmaReStart ERROR, no sampling rate change, port_id: 0x%X", pDevRxPort->intf);
      result = ADSP_EALREADY;
      goto _dma_restart_bail_out;
    }
  }

  if(NULL != pDevTxPort)
  {
    if (AFE_PORT_STATE_RUN != pDevTxPort->port_state)
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFECmdDmaReStart ERROR, not running state, port_id: 0x%X", pDevTxPort->intf);
      result = ADSP_ENOTREADY;
      goto _dma_restart_bail_out;
    }

    if(pDevTxPort->sample_rate == pPortsReStartCmd->sample_rate)
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFECmdDmaReStart ERROR, no sampling rate change, port_id: 0x%X", pDevTxPort->intf);
      result = ADSP_EALREADY;
      goto _dma_restart_bail_out;
    }
  }

  if(NULL != pDevRxPort)
  {
    afe_port_mutex_lock(pDevRxPort);
    rx_port_mutex_locked = TRUE;

    pDevRxPort->dev_proc_seq = AFE_DEV_RESTART;

    if(ADSP_EOK != (result = afe_port_modules_deinit_and_dev_stop(pDevRxPort)))
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ReSTART CMD FAIL, failed to stop the device, port 0x%X", pDevRxPort->intf);
      goto _dma_restart_bail_out;
    }

    pDevRxPort->sample_rate = pPortsReStartCmd->sample_rate;
  }

  if(NULL != pDevTxPort)
  {
    afe_port_mutex_lock(pDevTxPort);
    tx_port_mutex_locked = TRUE;

    pDevTxPort->dev_proc_seq = AFE_DEV_RESTART;

    if(ADSP_EOK != (result = afe_port_modules_deinit_and_dev_stop(pDevTxPort)))
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ReSTART CMD FAIL, failed to stop the device, port 0x%X", pDevTxPort->intf);
      goto _dma_restart_bail_out;
    }

    pDevTxPort->sample_rate = pPortsReStartCmd->sample_rate;
  }

  MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "new sampling rate: %lu",   pPortsReStartCmd->sample_rate);


  if(ADSP_EOK != (result = afe_reconfig_clocks_for_restart(pDevRxPort, pDevTxPort)))
  {
    goto _dma_restart_bail_out;
  }

  if(NULL != pDevTxPort)
  {
    /* ADSP_ECONTINUE will be for h/w mad or s/w duty enabled case */
    if(ADSP_ECONTINUE == (result = afe_port_modules_init_and_dev_start(pDevTxPort)))
    {
      result = ADSP_EOK;
      goto _dma_restart_bail_out;
    }

    if(ADSP_EOK != result)
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ReSTART CMD FAIL, failed to device restart modules port 0x%X", pDevTxPort->intf);

      /* Set the port state to CONFIG. This is needed to execute stop cmd sequence completely.
       *
       * Assume, afe_port_modules_deinit_and_dev_stop() succeeds (so port state moves to STOP),
       * and also assume afe_port_modules_init_and_dev_start() fails (this means port state
       *  still in STOP state). If we try to execute afe_port_apr_dma_stop(), now stop cmd will
       *  think that port is already stopped (due to port state check). Therefore, resources
       * would not be deallocated and modules not be de-initialized */

      pDevTxPort->dev_proc_seq = AFE_NORMAL_SEQ;

      pDevTxPort->port_state = AFE_PORT_STATE_CONFIG;

      /* execute stop cmd sequence */
      afe_port_apr_dma_stop(pDevTxPort, NULL);
    }
  }

  if(NULL != pDevRxPort)
  {
    /* ADSP_ECONTINUE will be for h/w mad or s/w duty enabled case */
    if(ADSP_ECONTINUE == (result = afe_port_modules_init_and_dev_start(pDevRxPort)))
    {
      result = ADSP_EOK;
      goto _dma_restart_bail_out;
    }

    if(ADSP_EOK != result)
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ReSTART CMD FAIL, failed to device restart modules port 0x%X", pDevRxPort->intf);

      /* Set the port state to CONFIG. This is needed to execute stop cmd sequence completely.
       *
       * Assume, afe_port_modules_deinit_and_dev_stop() succeeds (so port state moves to STOP),
       * and also assume afe_port_modules_init_and_dev_start() fails (this means port state
       *  still in STOP state). If we try to execute afe_port_apr_dma_stop(), now stop cmd will
       *  think that port is already stopped (due to port state check). Therefore, resources
       * would not be deallocated and modules not be de-initialized */

      pDevRxPort->dev_proc_seq = AFE_NORMAL_SEQ;

      pDevRxPort->port_state = AFE_PORT_STATE_CONFIG;

      /* execute stop cmd sequence */
      afe_port_apr_dma_stop(pDevRxPort, NULL);
    }
  }

  _dma_restart_bail_out:

  if(rx_port_mutex_locked)
  {
    pDevRxPort->dev_proc_seq = AFE_NORMAL_SEQ;
    afe_port_mutex_unlock(pDevRxPort);
  }
  if(tx_port_mutex_locked)
  {
    pDevTxPort->dev_proc_seq = AFE_NORMAL_SEQ;
    afe_port_mutex_unlock(pDevTxPort);
  }

  if (pkt_ptr && (NULL != apr_handle_ptr))
  {
    if(APR_EOK != (rc = elite_apr_if_end_cmd(*(apr_handle_ptr), pkt_ptr, result)))
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "afe_ports_apr_dma_restart: Failed in elite_apr_if_end_cmd() with return code: %ld", rc);
    }
  }

  return result;
}


ADSPResult afe_port_apr_get_param_handler(afe_dev_port_t* pDevPort, elite_apr_packet_t* pkt_ptr)
{
  elite_apr_packet_t *ack_apr_pkt_ptr = NULL;
  uint32_t payload_address;
  int8_t *calibration_data_pay_load_ptr;
  ADSPResult result = ADSP_EOK;
  ADSPResult param_result = ADSP_EOK;

  uint32_t param_size_max=0, param_headersize=0, module_id=0, param_id=0;
  uint16_t param_size=0, instance_id=0;

  uint32_t opcode = elite_apr_if_get_opcode( pkt_ptr);

  if(AFE_PORT_CMD_GET_PARAM_V3 == opcode)
  {
    //Get APR payload
    afe_port_cmd_get_param_v3_t *apr_payload_ptr = (afe_port_cmd_get_param_v3_t *) ( elite_apr_if_get_payload_ptr(pkt_ptr));
    param_headersize = sizeof(afe_port_param_data_v3_t);
    param_size_max = apr_payload_ptr->payload_size;
    module_id = apr_payload_ptr->module_id;
    instance_id = apr_payload_ptr->instance_id;
    param_id = apr_payload_ptr->param_id;
  }
  else  //AFE_PORT_CMD_GET_PARAM_V2
  {
    //Get APR payload
    afe_port_cmd_get_param_v2_t *apr_payload_ptr = (afe_port_cmd_get_param_v2_t *) ( elite_apr_if_get_payload_ptr(pkt_ptr));
    param_headersize = sizeof(afe_port_param_data_v2_t);
    param_size_max = apr_payload_ptr->payload_size;
    module_id = apr_payload_ptr->module_id;
    instance_id = 0;
    param_id = apr_payload_ptr->param_id;
  }

  if(AFE_MODULE_AANC == module_id)
  {
    /* If requested module contains AANC send the packet to AANC thread and all processing will be done in AANC thread
     * In this case simply return after sending the buffer
     * apr pkt will be free in AANC thread just like other get params
     */
    result = afe_svc_aanc_send_msg(afe_svc_global_ptr->aanc_cmdq_ptr,
                                   ELITEMSG_CUSTOM_GET_AANC_APR_PKT,
                                   pDevPort, pkt_ptr, param_size_max,
                                   0/* NA in get param case*/);
    if(ADSP_FAILED(result))
    {
      goto __bailout;
    }

    return ADSP_EOK;
  }

  if(ADSP_FAILED(result = afe_port_get_calibdata_address(pkt_ptr,&payload_address, &ack_apr_pkt_ptr)))
  {
    goto __bailout;
  }


  calibration_data_pay_load_ptr = (int8_t *)payload_address + param_headersize;


  switch(module_id)
  {
    case AFE_MODULE_SIDETONE_IIR_FILTER:
    {
      if(0 == instance_id) //supports only single instance with ZERO value.
      {
        param_result = afe_port_get_sidetone_iir_filter_module_params(pDevPort, param_id,calibration_data_pay_load_ptr,
                                                                      param_size_max, &param_size);
      }
      else
      {
        param_result = ADSP_EUNSUPPORTED;
      }
    }
    break;
    case AFE_MODULE_LOOPBACK:
    {
      if(0 == instance_id) //supports only single instance with ZERO value.
      {
        param_result = afe_port_get_loopback_module_params(pDevPort, param_id,calibration_data_pay_load_ptr,
                                                           param_size_max, &param_size);
      }
      else
      {
        param_result = ADSP_EUNSUPPORTED;
      }
    }
    break;
    case AFE_MODULE_AUDIO_DEV_INTERFACE:
    {
      if(0 == instance_id) //supports only single instance with ZERO value.
      {
        param_result = afe_port_get_device_module_params(pDevPort, param_id,calibration_data_pay_load_ptr,
                                                         param_size_max, &param_size);
      }
      else
      {
        param_result = ADSP_EUNSUPPORTED;
      }
    }
    break;
    case AFE_PRV_MODULE_AVSYNC_TEST:
    {
      if(0 == instance_id) //supports only single instance with ZERO value.
      {
        param_result = afe_port_get_avsync_test_prv_module_params(pDevPort, param_id,calibration_data_pay_load_ptr,
                                                                  param_size_max, &param_size);
      }
      else
      {
        param_result = ADSP_EUNSUPPORTED;
      }
    }
    break;
    case AFE_MODULE_ID_DRIFT:
    {
      if(0 == instance_id) //supports only single instance with ZERO value.
      {
        param_result = afe_port_get_drift_params(pDevPort, param_id,calibration_data_pay_load_ptr,
                                                 param_size_max, &param_size);
      }
      else
      {
        param_result = ADSP_EUNSUPPORTED;
      }
    }
    break;
    case AFE_MODULE_ID_SW_MAD:
    {
      if(0 == instance_id) //supports only single instance with ZERO value.
      {
        param_result = afe_port_get_swmad_module_params((void*)pDevPort, param_id,calibration_data_pay_load_ptr,
                                                        param_size_max, &param_size);
      }
      else
      {
        param_result = ADSP_EUNSUPPORTED;
      }
    }
    break;
    default:
    {
      uint32_t actual_param_len = 0;
      afe_port_mutex_lock(pDevPort);
      param_result = topo_get_param(pDevPort->port_topo_ptr, module_id, instance_id, param_id,  param_size_max, calibration_data_pay_load_ptr, &actual_param_len,NULL);
      param_size = (actual_param_len>=UINT16_MAX) ?
          UINT16_MAX : (uint16_t)actual_param_len;
      afe_port_mutex_unlock(pDevPort);

    }
  }

  if(AFE_PORT_CMD_GET_PARAM_V3 == opcode)
  {
    afe_port_param_data_v3_t *afe_data_param_ptr = (afe_port_param_data_v3_t *)payload_address;
    afe_data_param_ptr->module_id =module_id;
    afe_data_param_ptr->instance_id =instance_id;
    afe_data_param_ptr->reserved =0;
    afe_data_param_ptr->param_id =param_id;
    afe_data_param_ptr->param_size = param_size;
  }
  else //AFE_PORT_CMD_GET_PARAM_V2
  {
    afe_port_param_data_v2_t *afe_data_param_ptr = (afe_port_param_data_v2_t *)payload_address;
    afe_data_param_ptr->module_id =module_id;
    afe_data_param_ptr->param_id =param_id;
    afe_data_param_ptr->param_size = param_size;
  }

  if (ADSP_EUNSUPPORTED == param_result)
  {

    MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error:Get-Param rcvd unsupported module-instance IDs:0x%lx-0x%hx for port 0x%hx", module_id, instance_id, pDevPort->intf);
  }

  result =  afe_port_send_calibdata_to_client(pkt_ptr,param_result,ack_apr_pkt_ptr);

  return result;

  __bailout:
  if (ack_apr_pkt_ptr)
  {
    elite_apr_if_free( afe_svc_get_apr_handle(),  ack_apr_pkt_ptr);
    ack_apr_pkt_ptr = NULL;
  }
  result = ADSP_ENOTREADY;
  elite_apr_if_end_cmd(afe_svc_global_ptr->apr_handle, pkt_ptr, result );

  return result;
}

ADSPResult afe_port_set_calib_data(afe_dev_port_t *pDevPort, uint32_t payload_address,
                                   uint32_t *nPayloadSize, bool_t *contains_aanc_params_ptr, elite_apr_packet_t* pkt_ptr, uint32_t opcode)
{
  ADSPResult               nLocalResult                    = ADSP_EOK;
  ADSPResult               nAggregateResult                = ADSP_EOK;
  uint16_t                 byte_size_counter               = 0;
  int8_t *calibration_data_pay_load_ptr;
  uint32_t payload_address_start;
  uint32_t module_id=0, param_id=0, param_size=0, payload_headersize=0;
  uint16_t instance_id=0;

  //keep a copy of the start address. The process need not be repeated for AANC.
  payload_address_start = payload_address;
  // Iterate through the entire payload size and copy all updated parameters
  do
  {
    /* For every iteration if the setparam is failing with EUNSUPPORTED we are updating the
     * afe_data_param_ptr and moving to the next setparam. In this way there could be possibility that
     * setparam has received some junk data and because of this its failing with EUNSUPPORTED
     * and afe_data_param_ptr is updated with unalligned address which leads to crash. To circumvent this the
     * below check is added */
    uint8_t *pPayload_address = (uint8_t *)payload_address;
    if(!IS_ALIGN_4_BYTE((uint32_t)pPayload_address))
    {
      MSG_1(MSG_SSID_QDSP6,DBG_ERROR_PRIO,
            "param data pointer is not four byte alligned 0x%lx", pPayload_address);
      return  ADSP_EBADPARAM;
    }

    if(AFE_PORT_CMD_SET_PARAM_V3 == opcode)
    {
      afe_port_param_data_v3_t *pDataPayload = (afe_port_param_data_v3_t *)(payload_address);
      module_id = pDataPayload->module_id;
      instance_id = pDataPayload->instance_id;
      param_size = pDataPayload->param_size;
      param_id = pDataPayload->param_id;
      payload_headersize = sizeof(afe_port_param_data_v3_t);
    }
    else //AFE_PORT_CMD_SET_PARAM_V2
    {
      afe_port_param_data_v2_t *pDataPayload = (afe_port_param_data_v2_t *)(payload_address);
      module_id = pDataPayload->module_id;
      instance_id = 0;
      param_size = pDataPayload->param_size;
      param_id = pDataPayload->param_id;
      payload_headersize = sizeof(afe_port_param_data_v2_t);
    }

    calibration_data_pay_load_ptr = (int8_t*)payload_address + payload_headersize;

    //this condition checks whether individual param id size is valid or not.
    //if it is greater than the sum of all param ids , it will lead to crash if this condition wont present here.
    if (param_size >= (*nPayloadSize-byte_size_counter))
    {
      MSG_1(MSG_SSID_QDSP6,DBG_ERROR_PRIO,
            "param size is invalid %d", param_size);
      return  ADSP_EBADPARAM;
    }
    switch (module_id)
    {
      case AFE_MODULE_SIDETONE_IIR_FILTER:
      {
        if(0 == instance_id) //supports only single instance with ZERO value.
        {
          nLocalResult = afe_port_set_sidetone_iir_filter_module_params(pDevPort,
                                                                        param_id,
                                                                        calibration_data_pay_load_ptr,
                                                                        param_size);
        }
        else
        {
          nLocalResult = ADSP_EUNSUPPORTED;
          MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error:SIDETONE_IIR_FILTER module Set-Param rcvd unsupported instance_id:%hu ", instance_id);
        }
      }
      break;
      case AFE_MODULE_LOOPBACK:
      {
        if(0 == instance_id) //supports only single instance with ZERO value.
        {
          nLocalResult = afe_port_set_loopback_module_params(pDevPort, param_id,
                                                             calibration_data_pay_load_ptr, param_size);
        }
        else
        {
          nLocalResult = ADSP_EUNSUPPORTED;
          MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error:AFE_MODULE_LOOPBACK Set-Param rcvd unsupported instance_id:%hu ", instance_id);
        }
      }
      break;
      case AFE_MODULE_AUDIO_DEV_INTERFACE:
      {
        if(0 == instance_id) //supports only single instance with ZERO value.
        {
          nLocalResult = afe_port_set_device_module_params(pDevPort, param_id,
                                                           calibration_data_pay_load_ptr, param_size);
        }
        else
        {
          nLocalResult = ADSP_EUNSUPPORTED;
          MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error:AUDIO_DEV_INTERFACE Set-Param rcvd unsupported instance_id:%hu ", instance_id);
        }
      }
      break;
      case AFE_MODULE_CUSTOM_EVENTS:
      {
        if(0 == instance_id) //supports only single instance with ZERO value.
        {
          afe_port_event_addr_info_t event_addr_info;
          event_addr_info.apr_client_addr =  elite_apr_if_get_src_addr(pkt_ptr);
          event_addr_info.apr_client_port = elite_apr_if_get_src_port(pkt_ptr);
          event_addr_info.apr_self_addr = elite_apr_if_get_dst_port(pkt_ptr);
          event_addr_info.apr_self_port = elite_apr_if_get_dst_port(pkt_ptr);

          afe_port_mutex_lock(pDevPort);
          nLocalResult = afe_port_set_custom_events_params(pDevPort, param_id,
                                                           calibration_data_pay_load_ptr,
                                                           param_size, &event_addr_info);
          afe_port_mutex_unlock(pDevPort);
        }
        else
        {
          nLocalResult = ADSP_EUNSUPPORTED;
          MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error:AFE_MODULE_CUSTOM_EVENTS Set-Param rcvd unsupported instance_id:%hu ", instance_id);
        }
      }
      break;
      case AFE_MODULE_AANC:
      {
        // Just mark the presence of a AANC parameter. The actual handling
        // is done at the end of the parsing while loop, which is delegating
        // the handling to a dedicated AANC thread
        *contains_aanc_params_ptr = TRUE;
      }
      break;
      case AFE_PRV_MODULE_AVSYNC_TEST:
      {
        if(0 == instance_id) //supports only single instance with ZERO value.
        {
          afe_port_mutex_lock(pDevPort);
          nLocalResult = afe_port_set_avsync_test_prv_module_params(pDevPort, param_id,
                                                                    calibration_data_pay_load_ptr,
                                                                    param_size);
          afe_port_mutex_unlock(pDevPort);
        }
        else
        {
          nLocalResult = ADSP_EUNSUPPORTED;
          MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error:AFE_PRV_MODULE_AVSYNC_TEST Set-Param rcvd unsupported instance_id:%hu ", instance_id);
        }
        break;
      }
      case AFE_MODULE_ID_SW_MAD:
      {
        if(0 == instance_id) //supports only single instance with ZERO value.
        {
          nLocalResult = afe_port_set_swmad_module_params(pDevPort, param_id,
                                                          calibration_data_pay_load_ptr, param_size);
        }
        else
        {
          nLocalResult = ADSP_EUNSUPPORTED;
          MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error:AFE_MODULE_ID_SW_MAD Set-Param rcvd unsupported instance_id:%hu ", instance_id);
        }
      }
      break;
      case AFE_MODULE_HW_MAD:
      {
        if(0 == instance_id) //supports only single instance with ZERO value.
        {
          afe_port_mutex_lock(pDevPort);
          nLocalResult = afe_port_hw_mad_set_param_handler(pDevPort, param_id,
                                                           calibration_data_pay_load_ptr,
                                                           param_size);
          afe_port_mutex_unlock(pDevPort);
        }
        else
        {
          nLocalResult = ADSP_EUNSUPPORTED;
          MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error:AFE_MODULE_HW_MAD Set-Param rcvd unsupported instance_id:%hu ", instance_id);
        }
      }
      break;
      case AFE_MODULE_FEEDBACK:
      {
        if(0 == instance_id) //supports only single instance with ZERO value.
        {
          nLocalResult = afe_port_set_feedback_path_params((void*)pDevPort, param_id,
                                                           calibration_data_pay_load_ptr,
                                                           param_size);
        }
        else
        {
          nLocalResult = ADSP_EUNSUPPORTED;
          MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error:AFE_MODULE_FEEDBACK Set-Param rcvd unsupported instance_id:%hu ", instance_id);
        }
      }
      break;
      case AFE_MODULE_SW_DUTY_CYCLE:
      {
        if(0 == instance_id) //supports only single instance with ZERO value.
        {
          nLocalResult = afe_port_sw_duty_cycle_set_param_handler(pDevPort, param_id,
                                                                  calibration_data_pay_load_ptr,
                                                                  param_size);
        }
        else
        {
          nLocalResult = ADSP_EUNSUPPORTED;
          MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error:AFE_MODULE_SW_DUTY_CYCLE Set-Param rcvd unsupported instance_id:%hu ", instance_id);
        }
      }
      break;
      case AFE_MODULE_TDM:
      {
        if(0 == instance_id) //supports only single instance with ZERO value.
        {
          nLocalResult = afe_port_set_tdm_module_params(pDevPort,param_id,calibration_data_pay_load_ptr,
                                                        param_size);
        }
        else
        {
          nLocalResult = ADSP_EUNSUPPORTED;
          MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error:AFE_MODULE_SW_DUTY_CYCLE Set-Param rcvd unsupported instance_id:%hu ", instance_id);
        }
      }
      break;
      case AFE_MODULE_PORT:
      {
        if(0 == instance_id) //supports only single instance with ZERO value.
        {
          nLocalResult = afe_port_set_port_module_params(pDevPort, param_id, calibration_data_pay_load_ptr, param_size);
          nAggregateResult |= nLocalResult; //need to update aggregated result here because use case will not proceed if the set params failed due to any reason
        }
        else
        {
          nLocalResult = ADSP_EUNSUPPORTED;
          nAggregateResult |= nLocalResult; //need to update aggregated result here because use case will not proceed if the set params failed due to any reason
          MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error:AFE_MODULE_PORT Set-Param rcvd unsupported instance_id:%hu ", instance_id);
        }
      }
      break;
      case AVS_MODULE_ID_ENCODER:
      case AVS_MODULE_ID_DECODER:
      {
        if(0 == instance_id) //supports only single instance with ZERO value.
        {
          nLocalResult = afe_set_enc_dec_params(pDevPort, module_id, param_id, calibration_data_pay_load_ptr, param_size);
          nAggregateResult |= nLocalResult; //need to update aggregated result here because use case will not proceed if the set params failed due to any reason
        }
        else
        {
          nLocalResult = ADSP_EUNSUPPORTED;
          nAggregateResult |= nLocalResult; //need to update aggregated result here because use case will not proceed if the set params failed due to any reason
          MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: module_id 0x%x Set-Param rcvd unsupported instance_id:%hu ", module_id, instance_id);
        }
      }
      break;
      case AFE_MODULE_LIMITER:
        if (0 == instance_id) //supports only single instance with ZERO value.
        {
          {
            nLocalResult = afe_port_set_param_limiter(pDevPort, param_id, calibration_data_pay_load_ptr, param_size);
            nAggregateResult |= nLocalResult; //need to update aggregated result here because use case will not proceed if the set params failed due to any reason
          }
        }
        break;
      default:
      {
        // No need to honor the disable setparam when port is in stop state and AFE graph is not created
        if (AFE_PARAM_ID_ENABLE == param_id)
        {
          afe_mod_enable_param_t *param_ptr = (afe_mod_enable_param_t *)calibration_data_pay_load_ptr;

          if((0 == param_ptr->enable) && (AFE_PORT_STATE_STOP == pDevPort->port_state)
              && (NULL == pDevPort->port_topo_ptr))
          {
            MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
                  "Not honoring the disable command when port 0x%x is in stop state", pDevPort->intf);
            break;
          }
        }
        nLocalResult = afe_topo_setparam((void*)pDevPort, module_id, instance_id,
                                         param_id, param_size,
                                         calibration_data_pay_load_ptr);
        if (ADSP_EUNSUPPORTED == nLocalResult)
        {
          MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                "afe_port_apr_set_param_handler: Un-Supported Module ID: 0x%lx for port 0x%x",
                module_id, pDevPort->intf);
        }
      }
      break;
    }//switch

    // Return ADSP_EBADPARAM by wrappers only when paramsize is less than the required size.
    //Only in this case we are bailing out, for other errors we will continue parsing
    if (ADSP_EBADPARAM == nLocalResult)
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE SetParam bailing out for port ::0x%x!!!", pDevPort->intf);
      nAggregateResult = ADSP_EBADPARAM;
      break; // in case of any error dont go forward with parsing
    }

    byte_size_counter += (payload_headersize + param_size);
    payload_address += (payload_headersize + param_size);
  }while(byte_size_counter < *nPayloadSize);

  if ((ADSP_EBADPARAM != nAggregateResult)&& (TRUE == *contains_aanc_params_ptr))
  {
    nLocalResult = afe_svc_aanc_send_msg(afe_svc_global_ptr->aanc_cmdq_ptr,
                                         ELITEMSG_CUSTOM_SET_AANC_APR_PKT,
                                         pDevPort, pkt_ptr, *nPayloadSize, payload_address_start);
    if(ADSP_FAILED(nLocalResult))
    {
      nAggregateResult = ADSP_EFAILED;
    }
  }

  return nAggregateResult;
}

ADSPResult afe_port_apr_set_param_handler(afe_dev_port_t* pDevPort, elite_apr_packet_t* pkt_ptr)
{
  ADSPResult result = ADSP_EOK;
  int32_t rc;
  bool_t contains_aanc_params = FALSE;
  uint32_t payload_address ;
  uint32_t mem_map_handle, cmd_headersize, addr_lsw, addr_msw, payload_size;

  uint32_t ulOpCode = elite_apr_if_get_opcode( pkt_ptr);


  if(AFE_PORT_CMD_SET_PARAM_V3 == ulOpCode)
  {
    //Get APR payload
    afe_port_cmd_set_param_v3_t *apr_payload_ptr = (afe_port_cmd_set_param_v3_t *) ( elite_apr_if_get_payload_ptr(pkt_ptr));
    cmd_headersize = sizeof(afe_port_cmd_set_param_v3_t);
    mem_map_handle = apr_payload_ptr->mem_map_handle;
    addr_lsw = apr_payload_ptr->payload_address_lsw;
    addr_msw = apr_payload_ptr->payload_address_msw;
    payload_size = apr_payload_ptr->payload_size;
  }
  else //AFE_PORT_CMD_SET_PARAM_V2
  {
    //Get APR payload
    afe_port_cmd_set_param_v2_t *apr_payload_ptr = (afe_port_cmd_set_param_v2_t *) ( elite_apr_if_get_payload_ptr(pkt_ptr));
    cmd_headersize = sizeof(afe_port_cmd_set_param_v2_t);
    mem_map_handle = apr_payload_ptr->mem_map_handle;
    addr_lsw = apr_payload_ptr->payload_address_lsw;
    addr_msw = apr_payload_ptr->payload_address_msw;
    payload_size = apr_payload_ptr->payload_size;
  }


  if (0 == mem_map_handle) //in-band
  {
    payload_address = (uint32_t)((int8_t*)elite_apr_if_get_payload_ptr(pkt_ptr) + cmd_headersize);
    MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO, "AFE Port Received in-band parameters of size %lu to port ::0x%x!!",payload_size,pDevPort->intf);
  }
  else //out-of-band
  {
    if (0 == afe_svc_global_ptr->afe_memory_map_client)
    {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ERROR, Client Null, cannot get virtual address");
      result = ADSP_EFAILED;
      goto _set_param_bail_out;
    }

    result = qurt_elite_memorymap_get_virtual_addr_from_shmm_handle(afe_svc_global_ptr->afe_memory_map_client, mem_map_handle, addr_lsw, addr_msw, &(payload_address));

    MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE Memory Map PhyLSW(%lx) PhyMSW(%lx) virtual address(%lx)",addr_lsw, addr_msw, payload_address);

    if( ADSP_SUCCEEDED(result))
    {
      result = qurt_elite_memorymap_cache_invalidate((payload_address), payload_size);
    }
    else
    {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ERROR, get virtual addr or Invalidate Cache failed");
      result =  ADSP_EFAILED;
      goto _set_param_bail_out;
    }

    MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO, "AFE Port Received out-of-band parameters of size %lu to port ::0x%x!!",payload_size,pDevPort->intf);
  }

  result = afe_port_set_calib_data(pDevPort, payload_address, &payload_size, &contains_aanc_params, pkt_ptr, ulOpCode);

  // Dont need to send apr_if_end_cmd if AANC module is present and command has been successfully sent to AANC
  if ((TRUE == contains_aanc_params) && ADSP_SUCCEEDED(result))
  {
    goto _skip_apr_if_end;
  }

  _set_param_bail_out:
  if(APR_EOK != (rc = elite_apr_if_end_cmd(afe_svc_global_ptr->apr_handle, pkt_ptr, result)))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "afe_port_apr_set_param_handler: Failed in elite_apr_if_end_cmd() with return code: %ld", rc);
  }
  _skip_apr_if_end:
  return(result);
}

ADSPResult afe_svc_set_dev_cfg_params(uint32_t param_id,
                                      int8_t *param_buffer_ptr,
                                      uint32_t param_size)
{
  ADSPResult                          result = ADSP_EOK;

  switch(param_id)
  {
    case AFE_PARAM_ID_DEVICE_RECOVERY_CONFIG:
    {
      result = afe_err_handler_set_dev_recovery_cfg(param_buffer_ptr, param_size);
    }
    break;
    default:
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Un-Supported param ID: 0x%lx", param_id);
      result = ADSP_EUNSUPPORTED;
      break;
  }

  return result;
}

ADSPResult afe_svc_set_calib_data(uint32_t payload_address, uint32_t *nPayloadSize, elite_apr_packet_t* pkt_ptr)
{
  int8_t *calibration_data_pay_load_ptr;
  uint16_t byte_size_counter =0;
  ADSPResult nLocalResult = ADSP_EOK, nAggregateResult = ADSP_EOK;

  uint32_t opcode = pkt_ptr->opcode;

  // Iterate through the entire payload size and copy all updated parameters
  do
  {
    uint32_t module_id, param_id, param_size, payload_headersize;
    uint16_t instance_id;

    if(AFE_SVC_CMD_SET_PARAM_V2 == opcode)
    {
      afe_svc_param_data_v2_t *afe_data_param_ptr = (afe_svc_param_data_v2_t *)(payload_address);
      module_id = afe_data_param_ptr->module_id;
      instance_id = afe_data_param_ptr->instance_id;
      param_size = afe_data_param_ptr->param_size;
      param_id = afe_data_param_ptr->param_id;
      payload_headersize = sizeof(afe_svc_param_data_v2_t);
    }
    else //AFE_SVC_CMD_SET_PARAM
    {
      afe_svc_param_data_t *afe_data_param_ptr = (afe_svc_param_data_t *)(payload_address);
      module_id = afe_data_param_ptr->module_id;
      instance_id = 0;
      param_size = afe_data_param_ptr->param_size;
      param_id = afe_data_param_ptr->param_id;
      payload_headersize = sizeof(afe_svc_param_data_t);
    }

    calibration_data_pay_load_ptr = (int8_t*)payload_address + payload_headersize;

    //this condition checks whetehr individual param id size is valid or not.
    //if it is greater than the sum of all param ids , it will lead to crash if this condition wont present here.
    if (param_size >= (*nPayloadSize-byte_size_counter))
    {
      MSG_1(MSG_SSID_QDSP6,DBG_ERROR_PRIO, "param size is invalid %d",param_size);
      return  ADSP_EBADPARAM;
    }

    if(instance_id !=0)
    {
      MSG_1(MSG_SSID_QDSP6,DBG_ERROR_PRIO, "Error: Set param received invalid InstanceID:%lu, supported only zero value",instance_id);
      nLocalResult =  ADSP_EUNSUPPORTED;
    }

    switch (module_id)
    {
      case AFE_MODULE_CLOCK_SET:
      {
        nLocalResult = afe_clock_manager_set_clk(param_id, calibration_data_pay_load_ptr, param_size);
      }
      break;
      case AFE_MODULE_CDC_DEV_CFG:
      {
        nLocalResult = afe_svc_set_cdc_dev_cfg(param_id, calibration_data_pay_load_ptr, param_size);
      }
      break;
      case AFE_MODULE_GROUP_DEVICE:
      {
        nLocalResult = afe_svc_set_group_device_module_params(param_id, calibration_data_pay_load_ptr,
                                                              param_size);
      }
      break;
      case AFE_MODULE_DEV_CONFIG:
      {
        nLocalResult = afe_svc_set_dev_cfg_params(param_id, calibration_data_pay_load_ptr, param_size);
      }
      break;
      default:
      {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Un-Supported Module ID: 0x%lx", module_id);
        nLocalResult = ADSP_EUNSUPPORTED;
      }
      break;
    }//switch

    MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "module-id: 0x%lx, Param-id: 0x%lx, result: 0x%x", module_id, param_id, nLocalResult);

    // Return ADSP_EBADPARAM by wrappers only when paramsize is less than the required size.
    //Only in this case we are bailing out, for other errors we will continue parsing
    if (ADSP_EBADPARAM == nLocalResult)
    {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE Svc SetParam bailing out");
      nAggregateResult = ADSP_EBADPARAM;
      break; // in case of any error dont go forward with parsing
    }
    byte_size_counter += (payload_headersize+ param_size);
    payload_address += (payload_headersize + param_size);
  }while(byte_size_counter < *nPayloadSize);

  return nAggregateResult;
}

ADSPResult afe_svc_apr_set_param_handler(elite_apr_packet_t* pkt_ptr)
{
  ADSPResult result = ADSP_EOK;
  int32_t rc;
  if(!pkt_ptr)
  {
    return ADSP_EBADPARAM;
  }
  uint32_t mem_map_handle, cmd_headersize, addr_lsw, addr_msw, payload_size, payload_address;
  uint32_t opcode = elite_apr_if_get_opcode( pkt_ptr);

  if(AFE_SVC_CMD_SET_PARAM_V2 == opcode)
  {
    //Get APR payload
    afe_svc_cmd_set_param_v2_t *apr_payload_ptr = (afe_svc_cmd_set_param_v2_t *) ( elite_apr_if_get_payload_ptr(pkt_ptr));
    cmd_headersize = sizeof(afe_svc_cmd_set_param_v2_t);
    mem_map_handle = apr_payload_ptr->mem_map_handle;
    addr_lsw = apr_payload_ptr->payload_address_lsw;
    addr_msw = apr_payload_ptr->payload_address_msw;
    payload_size = apr_payload_ptr->payload_size;
  }
  else //AFE_SVC_CMD_SET_PARAM
  {
    //Get APR payload
    afe_svc_cmd_set_param_t *apr_payload_ptr = (afe_svc_cmd_set_param_t *) ( elite_apr_if_get_payload_ptr(pkt_ptr));
    cmd_headersize = sizeof(afe_svc_cmd_set_param_t);
    mem_map_handle = apr_payload_ptr->mem_map_handle;
    addr_lsw = apr_payload_ptr->payload_address_lsw;
    addr_msw = apr_payload_ptr->payload_address_msw;
    payload_size = apr_payload_ptr->payload_size;
  }

  if (0 == mem_map_handle)
  {
    payload_address = (int32_t)((int8_t*)elite_apr_if_get_payload_ptr(pkt_ptr) + cmd_headersize);
    MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO, "AFE Port Received in-band parameters of size %lu to AFE Service",payload_size);
  }
  else
  {
    if (0 == afe_svc_global_ptr->afe_memory_map_client)
    {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ERROR, Client Null, cannot get virtual address");
      result = ADSP_EFAILED;
      goto _set_param_bail_out;
    }
    if (ADSP_EOK == (result = qurt_elite_memorymap_get_virtual_addr_from_shmm_handle(afe_svc_global_ptr->afe_memory_map_client, mem_map_handle, addr_lsw, addr_msw, &(payload_address))))
    {
      MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE Memory Map PhyLSW(%lx) PhyMSW(%lx) virtual address(%lx)", addr_lsw, addr_msw, payload_address);
      result = qurt_elite_memorymap_cache_invalidate((payload_address), payload_size);
    }
    else
    {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ERROR, get virtual addr or Invalidate Cache failed");
      result =  ADSP_EFAILED;
      goto _set_param_bail_out;
    }
    MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO, "AFE Port Received out-of-band parameters of size %lu to AFE Svc", payload_size);
  }

  result = afe_svc_set_calib_data(payload_address, &payload_size, pkt_ptr);

  _set_param_bail_out:
  if(APR_EOK != (rc = elite_apr_if_end_cmd(afe_svc_global_ptr->apr_handle, pkt_ptr, result)))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "afe_svc_apr_set_param_handler: Failed in elite_apr_if_end_cmd() with return code: %ld", rc);
  }

  return(result);
}

ADSPResult afe_port_init_at_port_start(afe_dev_port_t* pDevPort)
{
  ADSPResult result = ADSP_EOK;

  /* Set the samples, only updates the port data structure */
  result = afe_port_get_frame_size(pDevPort->latency_mode, pDevPort->sample_rate, &pDevPort->int_samples_per_period);
  if(result != ADSP_EOK)
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " AFEPortAprHandler : Invalid sampling rate detected,Portid::0x%x",pDevPort->intf);
    return result;
  }

  if (ADSP_EOK != (result = afe_port_re_init_sample_rate_dependent_modules(pDevPort)))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Fail to re-initialize sample rate dependent modules, port_id : 0x%X", pDevPort->intf);
    return result;
  }

  /* Send media format to all the REGULAR Tx clients connected to this port

      Media type should be send only after init of sample rate dependent modules
      Media type for loopback client will be sent in the below function.
   */
  result = afe_port_send_media_type_to_tx_clients(pDevPort);

  if(ADSP_EOK != result)
  {
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Failed to send media type to all the Tx clients, port_id: 0x%X", pDevPort->intf);
  }

  /* Initing buffer latency measurement */
  result |= afe_cont_buf_latency_init(pDevPort);

  return result;
}

ADSPResult afe_port_re_init_sample_rate_dependent_modules(afe_dev_port_t* pDevPort)
{
  ADSPResult result = ADSP_EOK;
  uint32_t   rc;

  if (RX_DIR == pDevPort->dir)
  {
    if (ADSP_EOK != (result = afe_port_init_limiter(pDevPort)))
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to initialize the limiter, 0x%x", pDevPort->intf);
      return result;
    }
  }

  // If port loopback is in the run state, reconfig the tx-port client
  if (NULL != pDevPort->loopback_info_ptr)
  {
    if (AFE_PORT_LOOPBACK_STATE_RUN == pDevPort->loopback_info_ptr->afe_loopback_state)
    {
      result = afe_port_reconfig_loopback(pDevPort);
    }
  }

  // Initialize the resampler & other stuff for each client in this port.
  if (AFE_UNCOMPRESSED_DATA_PORT == pDevPort->port_data_type)
  {
    /*voting for uncompressed clients that are connected before port start were voted under this function*/
    result |= afe_port_client_reinit_at_port_start(pDevPort);
  }
  else
  {
    /*voting for compressed clients that are connected before port start were voted here*/
    result |= afe_port_compressed_client_voting_at_port_start(pDevPort);
  }

  // AFE PCM logging
  afe_port_data_logging_init (pDevPort);

  /**< Below steps are skipped for device recovery
   * (since, there would be no change in sampling rate),
   *  required only for normal start and device restart (change in sampling rate).*/
  if(AFE_DEV_RECOVERY != pDevPort->dev_proc_seq)
  {
    /* AFE topology Start */
    if (NULL != pDevPort->port_topo_ptr)
    {
      if (ADSP_EOK != (rc = afe_topo_start((void *)pDevPort)))
      {
        MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE topology start failed, bypassing AFE topology modules, 0x%x",
              pDevPort->intf);

        /* Stop the topo */
        afe_topo_stop((void *)pDevPort);

        /* Destroy the topo */
        afe_topo_destroy((void *)pDevPort);

        /* Set topo pointer to NULL */
        pDevPort->port_topo_ptr = NULL;

        result |= rc;
      }
    }

    // calculate common group delay
    afe_port_calc_cmn_group_delay(pDevPort);

    if (NULL != pDevPort->port_topo_ptr)
    {
      pDevPort->port_cmn_group_delay_us += ((topo_t *)pDevPort->port_topo_ptr)->delay_in_us;
    }

    // Update the group delays for the connected clients
    result |= afe_port_set_group_delay(pDevPort);
  }

  return result;
}

ADSPResult afe_port_de_init_at_port_stop(afe_dev_port_t* pDevPort)
{

  /* De-init sample rate dependent params */
  afe_port_de_init_sample_rate_dependent_modules(pDevPort);


  /* Send EoS to all the Tx clients connected to this port */
  if(!IS_PSEUDO_PORT_AFE_ID(pDevPort->intf))
  {
    afe_port_send_eos_to_tx_clients(pDevPort);
  }

  afe_port_reset_client_state(pDevPort);

  /* Deiniting buffer latency measurement */
  afe_cont_buf_latency_deinit(pDevPort);

  return ADSP_EOK;
}

ADSPResult afe_port_de_init_sample_rate_dependent_modules(afe_dev_port_t* pDevPort)
{
  ADSPResult result = ADSP_EOK;

  afe_mmpm_info_t *mmpm_info_ptr = (afe_mmpm_info_t *)pDevPort->mmpm_info_ptr;

  /* For AFE PCM logging */
  afe_port_data_logging_deinit(pDevPort);

  if (RX_DIR == pDevPort->dir)
  {
    afe_port_deinit_limiter(pDevPort);
  }

  /**< Below steps are skipped for device recovery
   * (since, there would be no change in sampling rate),
   *  required only for normal start and device restart (change in sampling rate).*/
  if(AFE_DEV_RECOVERY != pDevPort->dev_proc_seq)
  {
    /**<Stop the topology, for normal stop/device restart.
     * In case of device restart, HLOS would not set the
     * topo again, need to preserve the topo details.*/
    if (NULL != pDevPort->port_topo_ptr)
    {
      afe_topo_stop((void *)pDevPort);
    }

    //Deinit DTMF as it depends on the sampling rate
    afe_port_dtmf_deinit(pDevPort);

    /**< Below steps are done only for normal port stop .*/
    if(AFE_NORMAL_SEQ == pDevPort->dev_proc_seq)
    {
      /**<Destroy the topology only for normal stop.*/
      if(NULL != pDevPort->port_topo_ptr)
      {
        afe_topo_destroy((void *)pDevPort);
        pDevPort->port_topo_ptr = NULL;
      }

      /**< This topo received flag should be set to FALSE, irrespective of whether topo_ptr is NULL or non-NULL.
       * For example, in case of AANC, we are not setting any topology (topo_ptr = NULL), but we are setting
       * topo_rcvd_flag to be TRUE. To avoid set topo fails, this flag need to be cleared.*/
      pDevPort->topo_rcvd_flag = FALSE;

      /**< Free the feedback info memory , only for normal port stop.
       * No need to free, for device recovery and device restart,
       * since this feedback info memory is allocated during set param,
       * hence, this memory should be freed only for normal port stop.*/

      /* Disabling feedback from Tx port to Rx port */
      if(NULL != pDevPort->feedback_path_info_ptr)
      {
        qurt_elite_memory_free(pDevPort->feedback_path_info_ptr);
        pDevPort->feedback_path_info_ptr = NULL;
      }

      /*Since, vote_now is FALSE for the following topologies, this voting will just make these topologies as inactive, but it won't devote immediately.
       * If anyone of the topologies was already inactive, then also no problem, just making it again inactive. Actual devoting will be done during
       *  DEFAULT_TOPOLOGY devoting*/

      /* Note : DTMF is special case, where devoting of DTMF_TOPOLOGY is done only during port stop*/
      afe_mmpm_voting(pDevPort, AFE_MMPM_DTMF_TOPOLOGY, AFE_MMPM_DEVOTE,FALSE);

      afe_mmpm_voting(pDevPort, AFE_MMPM_LOOPBACK_TOPOLOGY, AFE_MMPM_DEVOTE, FALSE);

      afe_mmpm_voting(pDevPort, AFE_MMPM_SIDETONE_IIR_TOPOLOGY, AFE_MMPM_DEVOTE, FALSE);

      afe_mmpm_voting(pDevPort, AFE_MMPM_CLIENT_TOPOLOGY, AFE_MMPM_DEVOTE, FALSE);

      /*Since, CLIENT_TOPOLOGY is moved to inactive, set (client aggregated kpps and BW)and (previous limiter and audio voice mixer kpps)
       * values to zero*/
      if(NULL != mmpm_info_ptr)
      {
        memset(&mmpm_info_ptr->dyn_vot_info, 0, sizeof(afe_client_dynamic_voting_info_t));
      }
    }
  }

  return result;
}

/**
 * This function is about apr msg process
 *
 * @param[in] pDevPort, pointer instance of the port
 * @param[in] pMsg, pointer to message
 * @return the ADSP_EOK error code
 */
ADSPResult afe_port_apr_msg_handler(void* vpDevPort, elite_msg_any_t* pMsg)
{
  ADSPResult                      result = ADSP_EOK;
  afe_dev_port_t                 *pDevPort;
  pDevPort = (afe_dev_port_t *)vpDevPort;
  elite_apr_packet_t              *pkt_ptr;

  int32_t rc;

  pkt_ptr = (elite_apr_packet_t *)(pMsg->pPayload);

  if(NULL == pkt_ptr)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "afe_port_apr_msg_handler: elite apr pkt_ptr is NULL");
    return ADSP_EFAILED;
  }

  switch ( pkt_ptr->opcode )
  {
    case AFE_PORT_CMD_DEVICE_START: // Start the port
    {
      //for HDMI over DP case, afe_port_apr_dma_start() will not return pkt_ptr to caller
      //it will be returned after UNMUTE done
      result = afe_port_apr_dma_start(pDevPort,pkt_ptr);

      if(IS_HDMI_OVER_DP_PORT_AFE_ID(pDevPort->intf) && (ADSP_EOK == result))
      {
        //issue UNMUTE
        result = afe_dev_unmute(pDevPort,(void*) pkt_ptr);

        if((ADSP_EOK != result) && pkt_ptr)
        {
          MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "afe_dev_unmute: Failed with return code: %ld", result);
          //if afe_dev_unmute() is failed, notify to caller immediately.
          if(APR_EOK != (rc = elite_apr_if_end_cmd(afe_svc_global_ptr->apr_handle, pkt_ptr, result)))
          {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "afe_port_apr_dma_start: Failed in elite_apr_if_end_cmd() with return code: %ld", rc);
          }
        }
      }
      else
      {
        if (pkt_ptr)
        {
          if(APR_EOK != (rc = elite_apr_if_end_cmd(afe_svc_global_ptr->apr_handle, pkt_ptr, result)))
          {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "afe_port_apr_dma_start: Failed in elite_apr_if_end_cmd() with return code: %ld", rc);
          }
        }
      }
    }
    break;

    case AFE_PORT_CMD_DEVICE_STOP:
    {
      //Special Handle for HDMI over DP
      if(IS_HDMI_OVER_DP_PORT_AFE_ID(pDevPort->intf))
      {
        /* As per sequence MUTE_DONE will happen, after HLOS issue port stop command */
        /* In some cases if display driver disbles display port (in this case no port stop command received by DSP), */
        /* DMA_TYPE0_EVENT_DP_AUD_DIS_ERROR will trigger and start the teardown. This will make port state to stop. */
        /* In this scenario, no need to do mute metadata request again as part of port stop command*/
        if (!(AFE_PORT_STATE_STOP == pDevPort->port_state))
        {
          //issue MUTE and return
          result = afe_dev_mute(pDevPort,(void*) pkt_ptr);

          //for HDMI over DP case, afe_port_apr_dma_stop() will be called once MUTE done
          if((ADSP_EOK != result) && pkt_ptr)
          {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "afe_dev_mute, Failed with return code: %ld", result);
            //if afe_dev_mute() is failed, notify to caller immediately.
            if(APR_EOK != (rc = elite_apr_if_end_cmd(afe_svc_global_ptr->apr_handle, pkt_ptr, result)))
            {
              MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "afe_port_apr_dma_stop: Failed in elite_apr_if_end_cmd() with return code: %ld", rc);
            }
          }
        }
        else
        {
          MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Port in stop stop already, as part of SDE disabe interrupt handling 0x%x",pDevPort->intf);

          if(APR_EOK != (rc = elite_apr_if_end_cmd(afe_svc_global_ptr->apr_handle, pkt_ptr, result)))
          {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "afe_port_apr_dma_stop: Failed in elite_apr_if_end_cmd() with return code: %ld", rc);
          }
        }
      }
      else
      {
        result = afe_port_apr_dma_stop(pDevPort,pkt_ptr);

        if (pkt_ptr)
        {
          if(APR_EOK != (rc = elite_apr_if_end_cmd(afe_svc_global_ptr->apr_handle, pkt_ptr, result)))
          {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "afe_port_apr_dma_stop: Failed in elite_apr_if_end_cmd() with return code: %ld", rc);
          }
        }
      }
    }
    break;

    case AFE_PORT_CMD_GET_PARAM_V2:
    case AFE_PORT_CMD_GET_PARAM_V3:
    {
      result = afe_port_apr_get_param_handler(pDevPort, pkt_ptr);
    }
    break;
    case AFE_PORT_CMD_SET_PARAM_V2:
    case AFE_PORT_CMD_SET_PARAM_V3:
    {
      result = afe_port_apr_set_param_handler(pDevPort, pkt_ptr);
    }
    break;
    default:
    {
      result = ADSP_EUNSUPPORTED;
      MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO, "Received unsupported cmd=%d ",result);
    }
    break;
  }

  return result;
}


ADSPResult afe_port_get_calibdata_address(elite_apr_packet_t *apr_pkt_ptr, uint32_t *payload_address,\
                                          elite_apr_packet_t **ack_apr_pkt_ptr)
{ 
  ADSPResult result = ADSP_EOK;
  int32 *get_param_ptr=NULL;

  uint32_t mem_map_handle, addr_lsw, addr_msw, param_payload_size;
  uint32_t resp_opcode, resp_payload_size, param_data_size;

  uint32_t opcode = elite_apr_if_get_opcode( apr_pkt_ptr);

  if(AFE_PORT_CMD_GET_PARAM_V3 == opcode)
  {
    //Get APR payload
    afe_port_cmd_get_param_v3_t *apr_payload_ptr = (afe_port_cmd_get_param_v3_t *) ( elite_apr_if_get_payload_ptr(apr_pkt_ptr));
    mem_map_handle = apr_payload_ptr->mem_map_handle;
    addr_lsw = apr_payload_ptr->payload_address_lsw;
    addr_msw = apr_payload_ptr->payload_address_msw;
    param_payload_size = apr_payload_ptr->payload_size;
    resp_opcode = AFE_PORT_CMDRSP_GET_PARAM_V3;
    resp_payload_size = sizeof(afe_port_cmdrsp_get_param_v3_t);
    param_data_size = sizeof(afe_port_param_data_v3_t);
  }
  else //AFE_PORT_CMD_GET_PARAM_V2
  {
    //Get APR payload
    afe_port_cmd_get_param_v2_t *apr_payload_ptr = (afe_port_cmd_get_param_v2_t *) ( elite_apr_if_get_payload_ptr(apr_pkt_ptr));
    mem_map_handle = apr_payload_ptr->mem_map_handle;
    addr_lsw = apr_payload_ptr->payload_address_lsw;
    addr_msw = apr_payload_ptr->payload_address_msw;
    param_payload_size = apr_payload_ptr->payload_size;
    resp_opcode = AFE_PORT_CMDRSP_GET_PARAM_V2;
    resp_payload_size = sizeof(afe_port_cmdrsp_get_param_v2_t);
    param_data_size = sizeof(afe_port_param_data_v2_t);
  }

  // Now the message content depends on whether in-band or
  // out-of-band.
  // For in-band, Port manager needs to allocate large enough
  // APR packet
  if ( !(mem_map_handle) )
  {
    // The ack payload is status + mod Id + param Id + param size
    //                            + param values
    uint32_t ackPyloadSize = resp_payload_size + param_data_size + param_payload_size;

    // Allocate an APR packet
    result = elite_apr_if_alloc_cmd_rsp( afe_svc_get_apr_handle(),
                                         elite_apr_if_get_dst_addr( apr_pkt_ptr),
                                         elite_apr_if_get_dst_port( apr_pkt_ptr),
                                         elite_apr_if_get_src_addr( apr_pkt_ptr),
                                         elite_apr_if_get_src_port( apr_pkt_ptr),
                                         elite_apr_if_get_client_token( apr_pkt_ptr),
                                         resp_opcode,
                                         ackPyloadSize,
                                         ack_apr_pkt_ptr);

    if ( ADSP_FAILED( result ) || (NULL == *ack_apr_pkt_ptr))
    {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE : fail to allocate apr packet [0x%lx, S%lu]",
            (uint32_t) result, ackPyloadSize );
      return result;
    }

    get_param_ptr = (int32_t*)elite_apr_if_get_payload_ptr(*ack_apr_pkt_ptr);

    //offset to add where the actual payload will write
    *payload_address =(uint32_t) get_param_ptr + resp_payload_size;

    memset(((uint8_t *)(*payload_address)),0,param_data_size+param_payload_size);

    // free up the incoming apr packet since we create another one for ack already.
    elite_apr_if_free( afe_svc_get_apr_handle(),  apr_pkt_ptr);
  }
  else
  {
    // out of band processing
    if (0 == afe_svc_global_ptr->afe_memory_map_client)
    {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ERROR, Client Null, cannot get virtual address,\n");
      return result;
    }

    result = qurt_elite_memorymap_get_virtual_addr_from_shmm_handle(afe_svc_global_ptr->afe_memory_map_client, mem_map_handle, addr_lsw,addr_msw, (payload_address));

    MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, " Memory Map PhyLsw(%lx) PhyMsw(%lx) virtual address(%lx)", addr_lsw, addr_msw, *payload_address);

    if (ADSP_FAILED(result))
    {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ERROR, get virtual addr or Invalidate Cache failed, \n");
      return result;
    }
  }
  return  result;
}

ADSPResult afe_port_send_calibdata_to_client(elite_apr_packet_t *apr_pkt_ptr,uint32_t param_result,\
                                             elite_apr_packet_t *ack_apr_pkt_ptr)
{
  ADSPResult  result = ADSP_EOK;
  int32 *get_param_ptr=NULL;
  uint32_t payload_address;

  uint32_t mem_map_handle, addr_lsw, addr_msw, payload_size_max, param_headersize;

  uint32_t opcode = elite_apr_if_get_opcode( apr_pkt_ptr);

  if(AFE_PORT_CMD_GET_PARAM_V3 == opcode)
  {
    afe_port_cmd_get_param_v3_t *apr_payload_ptr = (afe_port_cmd_get_param_v3_t *) ( elite_apr_if_get_payload_ptr(apr_pkt_ptr));
    param_headersize = sizeof(afe_port_param_data_v3_t);
    mem_map_handle = apr_payload_ptr->mem_map_handle;
    addr_lsw = apr_payload_ptr->payload_address_lsw;
    addr_msw = apr_payload_ptr->payload_address_msw;
    payload_size_max = apr_payload_ptr->payload_size;
  }
  else //AFE_PORT_CMD_GET_PARAM_V2
  {
    afe_port_cmd_get_param_v2_t *apr_payload_ptr = (afe_port_cmd_get_param_v2_t *) ( elite_apr_if_get_payload_ptr(apr_pkt_ptr));
    param_headersize = sizeof(afe_port_param_data_v2_t);
    mem_map_handle = apr_payload_ptr->mem_map_handle;
    addr_lsw = apr_payload_ptr->payload_address_lsw;
    addr_msw = apr_payload_ptr->payload_address_msw;
    payload_size_max = apr_payload_ptr->payload_size;
  }

  //send the acknowledgement to client
  if(0 != mem_map_handle )
  {
    // Out of band data, so flush cache
    if (0 != afe_svc_global_ptr->afe_memory_map_client)
    {
      result = qurt_elite_memorymap_get_virtual_addr_from_shmm_handle(afe_svc_global_ptr->afe_memory_map_client,
                                                                      mem_map_handle, addr_lsw,
                                                                      addr_msw, &(payload_address));
      if( ADSP_SUCCEEDED(result))
      {
        MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE Memory Map Phy(%x) virtual address(%x)",
              (int)addr_lsw,(int) payload_address);
        result = qurt_elite_memorymap_cache_flush((payload_address), payload_size_max);
      }
    }
    else
    {
      result = ADSP_EFAILED;
    }
    if (AFE_PORT_CMD_GET_PARAM_V2 == opcode)
    {
      // sending out of band result
      afe_port_cmdrsp_get_param_v2_t get_param_ack;
      if (ADSP_EOK == param_result)
      {
        get_param_ack.status = APR_EOK;
      }
      else
      {
        get_param_ack.status = APR_EBADPARAM;
      }

      result = afe_svc_generate_ack(apr_pkt_ptr, param_result,
                                    (void *) &get_param_ack,      /// payload that is required in ACK, specified by caller
                                    sizeof(afe_port_cmdrsp_get_param_v2_t), /// payload size.
                                    AFE_PORT_CMDRSP_GET_PARAM_V2);
    }
    else //AFE_PORT_CMD_GET_PARAM_V3
    {
      // sending out of band result
      afe_port_cmdrsp_get_param_v3_t get_param_ack;
      if (ADSP_EOK == param_result)
      {
        get_param_ack.status = APR_EOK;
      }
      else
      {
        get_param_ack.status = APR_EBADPARAM;
      }

      result = afe_svc_generate_ack(apr_pkt_ptr, param_result,
                                    (void *) &get_param_ack,      /// payload that is required in ACK, specified by caller
                                    sizeof(afe_port_cmdrsp_get_param_v3_t), /// payload size.
                                    AFE_PORT_CMDRSP_GET_PARAM_V3);
    }

    if( ADSP_FAILED(result))
    {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE Get Param get virtual addr or flush error");
    }
    MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO, "Get Param ack apr_packet_ptr(0x%lx)",(uint32_t)apr_pkt_ptr);
  }
  else
  {
    if (NULL != ack_apr_pkt_ptr)
    {
      get_param_ptr = (int32_t*)elite_apr_if_get_payload_ptr(ack_apr_pkt_ptr);
      // in -band params

      if (AFE_PORT_CMD_GET_PARAM_V2 == opcode)
      {
        afe_port_cmdrsp_get_param_v2_t* get_param_ack_ptr = (afe_port_cmdrsp_get_param_v2_t*)get_param_ptr ;

        // fill the response status
        if (ADSP_EOK == param_result)
        {
          get_param_ack_ptr->status = APR_EOK;
        }
        else
        {
          get_param_ack_ptr->status = APR_EBADPARAM;
        }
      }
      else //AFE_PORT_CMD_GET_PARAM_V3
      {
        afe_port_cmdrsp_get_param_v3_t* get_param_ack_ptr = (afe_port_cmdrsp_get_param_v3_t*)get_param_ptr ;

        // fill the response status
        if (ADSP_EOK == param_result)
        {
          get_param_ack_ptr->status = APR_EOK;
        }
        else
        {
          get_param_ack_ptr->status = APR_EBADPARAM;
        }
      }
      // For in-band get param in which AFE Port already allocate APR packet with all appropriate
      // fields
      elite_apr_if_async_send( afe_svc_get_apr_handle(), ack_apr_pkt_ptr);

      MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO, "AFE  Get Param In-Band end ack packet(0x%lx)",(uint32_t)ack_apr_pkt_ptr);
    }
    else
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE  Get Param In-Band end ack packet(0x%lx) is NULL",(uint32_t)ack_apr_pkt_ptr);
    }
  }
  return result;
}
