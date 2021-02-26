
/*==============================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/riva/src/AFERivaDriver.cpp#2 $
$DateTime: 2018/03/05 21:24:21 $
$Author: pwbldsvc $
$Change: 15605306 $
$Revision: #2 $

FILE:     AFERivaDriver.cpp

DESCRIPTION: Main Interface to the AFE Riva driver

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright 2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR
==============================================================================*/
/*============================================================================
EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order. Please
use ISO format for dates.

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/riva/src/AFERivaDriver.cpp#2 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
12/10/10    JQ      Created file

========================================================================== */

/*=====================================================================
 Includes
 ======================================================================*/
#include "AFEDeviceDriver.h"
#include "AFERivaDriver.h"
#include "AFERivaDriver_i.h"
#include "AFEInternal.h"
#include "AFEDevService.h"
#include "AFEInterface.h"
#include "qurt_elite.h"
#include "adsp_afe_service_commands.h"
#include "EliteMsg_AfeCustom.h"
#include "LPASS_ADDRESS_FILE.h"

#define AFE_SYS_MONITOR
#ifdef AFE_SYS_MONITOR
#include "sys_m_messages.h"
#include "rcecb.h"
#endif // AFE_SYS_MONITOR

/*Qurt minimum page size of 4k*/
#define QURT_MEM_MAP_MIN_PAGE_SIZE   0x1000
/*=====================================================================
 Globals
 ======================================================================*/
static aud_stat_afe_riva_t  aud_stat_afe_riva;
static afe_dev_port_t       riva_bt_rx;
static afe_dev_port_t       riva_bt_tx;
static afe_dev_port_t       riva_fm_rx;
static afe_dev_port_t       riva_fm_tx;
static afe_dev_port_t       riva_bt_aud;
afe_dev_riva_port_t         *riva_ports_ptr_arr[AFE_PORT_ID_INTERNAL_BT_FM_RANGE_SIZE];
aud_stat_afe_riva_t         *aud_stat_afe_riva_ptr;
/*=====================================================================
 Functions
 ======================================================================*/
/**
 * Perform 32bit aligned memcpy
 *
 * @param[in] dst_buff_ptr for destination buffer pointer
 * src_buff_ptr for source buffer pointer
 * size for copy size
 *
 * @return  None
 *
 * @dependencies
 * None.
 *
 */
void afe_memcpy(uint8_t *dst_buff_ptr, uint32_t dst_size,
                uint8_t *src_buff_ptr, uint32_t src_size,
                bool_t aligned, uint16_t port_id)
{
   uint32_t    index;
   volatile uint32_t *dst_tmp_ptr = NULL, *src_tmp_ptr = NULL;

   if (aligned)
   {
      if (((uint32_t)dst_buff_ptr & 0x03) || ((uint32_t)src_buff_ptr & 0x03))
      {
         MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Destination 0x%lu or Source 0x%lu pointer is not word aligned, port-id 0x%x ", (uint32_t)dst_buff_ptr, (uint32_t)src_buff_ptr, port_id);
         return;
      }

      if (0 == src_size % 4)
      {
         dst_tmp_ptr = (volatile uint32_t *)dst_buff_ptr;
         src_tmp_ptr = (volatile uint32_t *)src_buff_ptr;
         for (index = 0; index < src_size / 4; index++)
         {
            dst_tmp_ptr[index] = src_tmp_ptr[index];
         }
      }
      else
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "afe_memcpy size %lu is not multiple of 4", src_size);
         return;
      }
   }
   else
   {
      memscpy(dst_buff_ptr, dst_size, src_buff_ptr, src_size);
   }
   return;
}

/**
 * Perform one time initialization of the AFE Riva driver during start up
 *
 * @param[in] None
 *
 * @return  ADSP_EOK on success, an error code on error
 *
 * @dependencies
 * None.
 *
 */
ADSPResult afe_driver_riva_init()
{
   ADSPResult result = ADSP_EFAILED;
   memset(&aud_stat_afe_riva, 0, sizeof(aud_stat_afe_riva_t));
   aud_stat_afe_riva_ptr = &aud_stat_afe_riva;

   if (ADSP_FAILED(result = afe_riva_ipc_init(&aud_stat_afe_riva)))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE Riva: Failed to init IPC");
   }

   // Register RIVA driver for SSR call backs
   afe_drv_riva_ssr_cb_register();

   return result;
}

/**
 * Perform de-initialization of the AFE Riva driver during AFE service destroy
 *
 * @param[in] None
 *
 * @return  ADSP_EOK on success, an error code on error
 *
 * @dependencies
 * None.
 *
 */
ADSPResult afe_driver_riva_deinit(void)
{
   ADSPResult result = ADSP_EOK;

   result = afe_riva_ipc_deinit(&aud_stat_afe_riva);

   // De-Register RIVA driver for SSR call backs
   afe_drv_riva_ssr_cb_deregister();

   aud_stat_afe_riva_ptr = NULL;

   return result;
}


/**
  @brief Perform RIVA port initialization during start up

  @param[in] afe_svc_ptr pointer for AFE static service

  @return  ADSP_EOK on success, an error code on error

 */
ADSPResult afe_riva_port_init(aud_stat_afe_svc_t *afe_svc_ptr)
{
   ADSPResult      result;
   afe_dev_port    *dev_port_ptr = NULL;
   uint16_t        port_id, dir, num_chan;
   uint32_t        sample_rate;
   afe_dev_riva_port_t *riva_dev_ptr;
   afe_dev_port_proc_func afe_dev_process = NULL;

   sample_rate = 48000;
   num_chan = 2;

   for (port_id = AFE_PORT_ID_INTERNAL_BT_FM_RANGE_START; port_id <= AFE_PORT_ID_INTERNAL_BT_FM_RANGE_END; port_id++)
   {
      switch (port_id)
      {
         case AFE_PORT_ID_INTERNAL_BT_SCO_RX:
            dev_port_ptr = &riva_bt_rx;
            sample_rate = 8000;
            num_chan = 1;
            riva_dev_ptr = &aud_stat_afe_riva.riva_port_bt_voc_rx;
            afe_dev_process = (afe_dev_port_proc_func)afe_write_riva;
            break;
         case AFE_PORT_ID_INTERNAL_BT_SCO_TX:
            dev_port_ptr = &riva_bt_tx;
            sample_rate = 8000;
            num_chan = 1;
            riva_dev_ptr = &aud_stat_afe_riva.riva_port_bt_voc_tx;
            afe_dev_process = (afe_dev_port_proc_func)afe_read_riva;
            break;
         case AFE_PORT_ID_INTERNAL_BT_A2DP_RX:
            dev_port_ptr = &riva_bt_aud;
            sample_rate = 48000;
            num_chan = 2;
            riva_dev_ptr = &aud_stat_afe_riva.riva_port_bt_aud;
            afe_dev_process = (afe_dev_port_proc_func)afe_write_bt_aud;
            break;
         case AFE_PORT_ID_INTERNAL_FM_RX:
            dev_port_ptr = &riva_fm_rx;
            sample_rate = 48000;
            num_chan = 2;
            riva_dev_ptr = &aud_stat_afe_riva.riva_port_fm_rx;
            afe_dev_process = (afe_dev_port_proc_func)afe_write_riva;
            break;
         case AFE_PORT_ID_INTERNAL_FM_TX:
            dev_port_ptr = &riva_fm_tx;
            sample_rate = 48000;
            num_chan = 2;
            riva_dev_ptr = &aud_stat_afe_riva.riva_port_fm_tx;
            afe_dev_process = (afe_dev_port_proc_func)afe_read_riva;
            break;
         default:
            dev_port_ptr = NULL;
            riva_dev_ptr = NULL;
      }

      afe_svc_ptr->ports_riva_ptr[GET_RIVA_PORT_INDEX(port_id)] = dev_port_ptr;

      riva_ports_ptr_arr[GET_RIVA_PORT_INDEX(port_id)] = riva_dev_ptr;

      if (NULL != dev_port_ptr)
      {
         dir = AFE_PORT_RX_DIR;
         if (IS_IT_TX_PORT_AFE_ID(port_id))
         {
            dir = AFE_PORT_TX_DIR;
         }
         if (ADSP_FAILED(result = afe_svc_init_port(dev_port_ptr, port_id, dir)))
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Port Init Fails, port-id =%d", port_id);
            return result;
         }
         else
         {
            dev_port_ptr->dev_sample_rate = sample_rate;
            dev_port_ptr->dev_channels = num_chan;

            //if applicable, overwrite the port media type with the device media type.
            //this is applicable for all the use cases where the port media type is not explicitly set by the HLOS client.
            afe_port_copy_media_type_dev_to_port(dev_port_ptr);

            dev_port_ptr->afe_drv = (void *)riva_dev_ptr;
            dev_port_ptr->afe_dev_process = afe_dev_process;
            riva_dev_ptr->hw_timestamp_available = aud_stat_afe_riva.hw_timestamp_available;
         }
      }
      if (NULL != riva_dev_ptr)
      {
         qurt_elite_mutex_init(&(riva_dev_ptr->riva_drv_mutex));
      }
   }
   return ADSP_EOK;
}

/**
  @brief Perform RIVA port de-initialization during AFE service destroy

  @param[in] afe_svc_ptr pointer for AFE static service

  @return  ADSP_EOK on success, an error code on error

 */
ADSPResult afe_riva_port_deinit(aud_stat_afe_svc_t *afe_svc_ptr)
{
   afe_dev_port     *dev_port_ptr = NULL;
   afe_dev_riva_port_t *riva_dev_ptr = NULL;
   uint16_t         port_id;

   for (port_id = AFE_PORT_ID_INTERNAL_BT_FM_RANGE_START; port_id <= AFE_PORT_ID_INTERNAL_BT_FM_RANGE_END; port_id++)
   {
      dev_port_ptr = afe_svc_ptr->ports_riva_ptr[GET_RIVA_PORT_INDEX(port_id)];
      if (NULL != dev_port_ptr)
      {
         afe_svc_deinit_port(dev_port_ptr);
      }
      riva_dev_ptr = riva_ports_ptr_arr[GET_RIVA_PORT_INDEX(port_id)];
      if (NULL != riva_dev_ptr)
      {
         qurt_elite_mutex_destroy(&(riva_dev_ptr->riva_drv_mutex));
      }
   }

   return ADSP_EOK;
}

/**
  @brief Indicates whether RIVA connected to LPASS or not .

  @param[in] afe_port_ptr pointer to corresponding AFE port

  @return  TRUE if connected, FALSE if not connected
   *
 * @dependencies
 * None.
 *
 */
bool_t afe_is_riva_active(afe_dev_port_t *afe_port_ptr)
{
   afe_dev_riva_port_t *p_dev_state = (afe_dev_riva_port_t *)afe_port_ptr->afe_drv;

   if ((NULL != p_dev_state) && (AFE_RIVA_IPC_CONNECT == p_dev_state->riva_connect))
   {
      return TRUE;
   }

   return FALSE;
}

ADSPResult afe_set_int_bt_cfg(afe_dev_port_t *afe_port_ptr, int8_t *p_cfg, uint16_t cfg_payload_size)
{
   afe_dev_riva_port_t *p_dev_state = (afe_dev_riva_port_t *)afe_port_ptr->afe_drv;

   // choose the appropriate config version
   uint32_t cfg_ver = (uint32_t) * (p_cfg);

   if (0 == aud_stat_afe_riva_ptr->hw_revision)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Internal BT, FM Hardware Not Present in this ChipSet, port_id: 0x%x", afe_port_ptr->intf);
      return ADSP_EUNSUPPORTED;
   }

   if (cfg_ver > AFE_API_VERSION_INTERNAL_BT_FM_CONFIG)
   {
      cfg_ver = AFE_API_VERSION_INTERNAL_BT_FM_CONFIG;
   }

   p_dev_state->dev_cfg_minor_ver = cfg_ver;
   switch (cfg_ver)
   {
      case BT_FM_CFG_V1:
      {
         afe_param_id_internal_bt_fm_cfg_t *p_riva_cfg = (afe_param_id_internal_bt_fm_cfg_t *)p_cfg;
         if (cfg_payload_size < sizeof(afe_param_id_internal_bt_fm_cfg_t))
         {
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Internal BT Config Fails, Bad Param");
            return ADSP_EBADPARAM;
         }

         // parse the bit width information
         if (16 != p_riva_cfg->bit_width)
         {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Un-supported bit width: %x, intf: %x", p_riva_cfg->bit_width, afe_port_ptr->intf);
            return ADSP_EUNSUPPORTED;
         }
         p_dev_state->bit_width = p_riva_cfg->bit_width;

         switch (afe_port_ptr->intf)
         {
            case AFE_PORT_ID_INTERNAL_BT_SCO_RX:
            case AFE_PORT_ID_INTERNAL_BT_SCO_TX:
            {
               if (1 != p_riva_cfg->num_channels)
               {
                  MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Un-supported num_channels: %x, intf: %x", p_riva_cfg->num_channels, afe_port_ptr->intf);
                  return ADSP_EUNSUPPORTED;
               }

               // parse the sample rate
               if ((AFE_PORT_SAMPLE_RATE_8K != p_riva_cfg->sample_rate) &&
                   (AFE_PORT_SAMPLE_RATE_16K != p_riva_cfg->sample_rate))
               {
                  MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Un-supported sampling rate: %lu, intf: %x", p_riva_cfg->sample_rate, afe_port_ptr->intf);
                  return ADSP_EUNSUPPORTED;
               }
            }
               break;

            case AFE_PORT_ID_INTERNAL_FM_RX:
            case AFE_PORT_ID_INTERNAL_FM_TX:
            case AFE_PORT_ID_INTERNAL_BT_A2DP_RX:
            {
               if (2 != p_riva_cfg->num_channels)
               {
                  MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Un-supported num_channels: %x, intf: %x", p_riva_cfg->num_channels, afe_port_ptr->intf);
                  return ADSP_EUNSUPPORTED;
               }

               // parse the sample rate
               if (AFE_PORT_SAMPLE_RATE_48K != p_riva_cfg->sample_rate)
               {
                  MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Un-supported sampling rate: %lu, intf: %x", p_riva_cfg->sample_rate, afe_port_ptr->intf);
                  return ADSP_EUNSUPPORTED;
               }
            }
               break;
         }

         afe_port_ptr->dev_channels = p_riva_cfg->num_channels;
         afe_port_ptr->dev_bytes_per_channel = (p_riva_cfg->bit_width > 16) ? 4 : 2;
         afe_port_ptr->dev_bit_width = p_riva_cfg->bit_width;
         // do not take HLOS sample rate for RIVA port. this will be provided by RIVA IPC cmd during connection. Work with default sample rate until that time.
         // afe_port_ptr->sample_rate = p_riva_cfg->sample_rate;

         //if applicable, overwrite the port media type with the device media type.
         //this is applicable for all the use cases where the port media type is not explicitly set by the HLOS client.
         afe_port_copy_media_type_dev_to_port(afe_port_ptr);

         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Num channels %lu, PortId: 0x%x", afe_port_ptr->dev_channels, afe_port_ptr->intf);
         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "bytes_per_channel: %d, sample_rate: %lu", afe_port_ptr->dev_bytes_per_channel, afe_port_ptr->dev_sample_rate);
      }
         break;

      default:
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Internal BT cfg Set Cmd Cfg fail: InCompatible cfg version: %ld, port_id: %d",
               cfg_ver, afe_port_ptr->intf);
         return ADSP_EUNSUPPORTED;
      }
         break;
   }

   return ADSP_EOK;
}

ADSPResult afe_get_int_bt_cfg(afe_dev_port_t *afe_port_ptr, int8_t *param_buf_ptr, uint32_t payload_size, uint16_t *params_buffer_len_req_ptr)
{
   afe_dev_riva_port_t *p_dev_state = (afe_dev_riva_port_t *)afe_port_ptr->afe_drv;
   uint32_t cfg_ver = p_dev_state->dev_cfg_minor_ver;

   if (0 == aud_stat_afe_riva_ptr->hw_revision)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Internal BT, FM Hardware Not Present in this ChipSet, port_id: 0x%x", afe_port_ptr->intf);
      return ADSP_EUNSUPPORTED;
   }

   switch (cfg_ver)
   {
      case BT_FM_CFG_V1:
      {
         *params_buffer_len_req_ptr = sizeof(afe_param_id_internal_bt_fm_cfg_t);
         if (*params_buffer_len_req_ptr > payload_size)
         {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Invalid payload size: RIVA cfg required size = %d, given size = %lu",
                  *params_buffer_len_req_ptr, payload_size);
            return ADSP_ENOMEMORY;
         }
         afe_param_id_internal_bt_fm_cfg_t *p_riva_cfg = (afe_param_id_internal_bt_fm_cfg_t *)param_buf_ptr;

         p_riva_cfg->bt_fm_cfg_minor_version = cfg_ver;
         p_riva_cfg->bit_width = p_dev_state->bit_width;
         p_riva_cfg->num_channels = afe_port_ptr->channels;
         p_riva_cfg->sample_rate = afe_port_ptr->sample_rate;
      }
         break;

      default:
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Internal BT cfg Get Cmd Cfg fail: InCompatible cfg version: %ld, port_id: %d",
               cfg_ver, afe_port_ptr->intf);
         return ADSP_EBADPARAM;
      }
         break;
   }

   return ADSP_EOK;
}

/**
 * Perform RIVA compatible device open. called each time a device is opened.
 *
 * @param[in] afe_port_ptr pointer to AFE device port structure
 *
 * @return  ADSP_EOK on success, an error code on error
 *
 * @dependencies
 * None.
 *
 */
ADSPResult afe_riva_open(afe_dev_port_t *pDevPort)
{
   ADSPResult           result = ADSP_EOK;
   uint8_t              *buf_ptr;
   uint64_t             ll_us;
   qurt_elite_timer_t   *timer_ptr;
   afe_dev_riva_port_t  *riva_port_ptr = (afe_dev_riva_port_t *)pDevPort->afe_drv;

   riva_port_ptr->block_transfer_time = (uint64_t)(pDevPort->int_samples_per_period * NUM_MS_PER_SEC) / pDevPort->sample_rate;
   riva_port_ptr->block_transfer_time_us = (uint64_t)(pDevPort->int_samples_per_period * NUM_US_PER_SEC) / pDevPort->sample_rate;

   if (0 == aud_stat_afe_riva_ptr->hw_revision)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Internal BT, FM Hardware Not Present in this ChipSet, port_id: 0x%x", pDevPort->intf);
      return ADSP_EUNSUPPORTED;
   }

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "afe_riva_open:%x:riva_connect=%lu ", pDevPort->intf, riva_port_ptr->riva_connect);

   if (AFE_PORT_ID_INTERNAL_BT_A2DP_RX == pDevPort->intf)
   {
      // Primary task on the threadpool:- timer triggerred
      if (NULL == (timer_ptr = (qurt_elite_timer_t *)qurt_elite_memory_malloc(sizeof(qurt_elite_timer_t), QURT_ELITE_HEAP_DEFAULT)))
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to create riva port primary task timer object");
         return ADSP_ENOMEMORY;
      }

      if (ADSP_EOK != (result = afe_time_srvr_timer_create(timer_ptr, QURT_ELITE_TIMER_PERIODIC, &(pDevPort->dma_signal))))
      {
         qurt_elite_memory_free(timer_ptr);
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Failed to start RIVA primary task timer");
         return result;
      }
      pDevPort->timer_ptr = timer_ptr;

      ll_us = riva_port_ptr->block_transfer_time_us;
      if (ADSP_EOK != (result = afe_time_srvr_timer_periodic_start(pDevPort->timer_ptr, ll_us)))
      {
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "afe_sink_start: starting Bt Audio periodic timer  failed\n");
         return result;
      }
      ll_us = ll_us + afe_time_srvr_timer_get_time((avtimer_drv_handle_t)pDevPort->avt_drv);
      ((afe_dev_riva_port_t *)(pDevPort->afe_drv))->riva_clock = ll_us;
   }
   else
   {
      /**
       * For riva BT voice port, setup 6ms circular buffer for 3.75ms period riva interrupt.
       * setup 10ms circular buffer 7.5 ms period riva interrupt.
       */
      if ((AFE_PORT_ID_INTERNAL_BT_SCO_RX == pDevPort->intf) || (AFE_PORT_ID_INTERNAL_BT_SCO_TX == pDevPort->intf))
      {
         /*allocate in DDR for BEAR FAMILY Target*/
         if ((uint32_t)LPASS_HW_VER_3_3_0 == aud_stat_afe_riva_ptr->hw_revision)
         {
            buf_ptr = (uint8_t *)qurt_elite_memory_malloc(AFE_BTVOC_CIRC_BUF_SIZE, QURT_ELITE_HEAP_DEFAULT);
         }
         else
         {
            buf_ptr = (uint8_t *)afe_dev_lpa_aligned_malloc(AFE_BTVOC_CIRC_BUF_SIZE, 32);
         }
      }
      else
      {
         buf_ptr = (uint8_t *)qurt_elite_memory_malloc(AFE_FM_CIRC_BUF_SIZE, QURT_ELITE_HEAP_DEFAULT);
      }

      if (!buf_ptr)
      {
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "afe riva port start, malloc failed\n");
         return ADSP_ENOMEMORY;
      }

      riva_port_ptr->circ_buff.top_addr = buf_ptr;
      riva_port_ptr->riva_lpass_drift_us = 0;
      riva_port_ptr->acc_avt_drift_us     = 0;
      if (ADSP_EOK != (result = afe_riva_set_sample_rate_params(riva_port_ptr, pDevPort)))
      {
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Un-supported sampling rate by RIVA port, exit device open");
         return ADSP_ENOMEMORY;
      }
      afe_driver_set_circ_buff(pDevPort);

      if (AFE_PORT_ID_INTERNAL_BT_SCO_TX == pDevPort->intf)
      {
         aud_stat_afe_riva_ptr->drift_correction_us = 0;
      }

      // Primary task on the threadpool:- timer triggerred
      if (NULL == (timer_ptr = (qurt_elite_timer_t *)qurt_elite_memory_malloc(sizeof(qurt_elite_timer_t), QURT_ELITE_HEAP_DEFAULT)))
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to create riva port primary task timer object");
         return ADSP_ENOMEMORY;
      }

      if (ADSP_EOK != (result = afe_time_srvr_timer_create(timer_ptr, QURT_ELITE_TIMER_ONESHOT_ABSOLUTE, &(pDevPort->dma_signal))))
      {
         qurt_elite_memory_free(timer_ptr);
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Failed to start RIVA primary task timer");
         return result;
      }
      pDevPort->timer_ptr = timer_ptr;

      ll_us = afe_time_srvr_timer_get_time((avtimer_drv_handle_t)pDevPort->avt_drv);
      ll_us = ll_us + riva_port_ptr->block_transfer_time_us;
      riva_port_ptr->riva_clock           = ll_us;
      riva_port_ptr->riva_lpass_drift_us  = 0;
      riva_port_ptr->acc_avt_drift_us     = 0;
      if (ADSP_EOK != (result = afe_time_srvr_timer_oneshot_start_absolute(pDevPort->timer_ptr, ll_us)))
      {
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "afe_sink_start: starting BtVoc/Fm one shot timer  failed\n");
      }
      //change the riva_lpass_data_processing state  to RUN
      qurt_elite_mutex_lock(&riva_port_ptr->riva_drv_mutex);
      riva_port_ptr->riva_lpass_data_proc_state = AFE_RIVA_LPASS_DATA_PROCESSING_RUN;
      qurt_elite_mutex_unlock(&riva_port_ptr->riva_drv_mutex);
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "afe riva:%x: started BtVoc/Fm one shot timer at %lu", pDevPort->intf, (uint32_t)ll_us);
   }

   return result;
}

/**
 * Perform RIVA device close every time a dev is to be closed.
 *
 * @param[in] afe_port_ptr pointer to AFE device port structure
 *
 * @return  ADSP_EOK on success, an error code on error
 *
 * @dependencies
 * None.
 *
 */
void afe_riva_close(afe_dev_port_t *pDevPort)
{
   ADSPResult result = ADSP_EOK;
   afe_dev_riva_port_t  *riva_port_ptr = (afe_dev_riva_port_t *)pDevPort->afe_drv;

   if (0 == aud_stat_afe_riva_ptr->hw_revision)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Internal BT, FM Hardware Not Present in this ChipSet, port_id: 0x%x", pDevPort->intf);
      return;
   }

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "afe_riva_close:%x:riva_connect=%lu ", pDevPort->intf, riva_port_ptr->riva_connect);

   //Stop the timer.
   if (pDevPort->timer_ptr != NULL)
   {
      if (ADSP_EOK != (result = afe_time_srvr_timer_delete(pDevPort->timer_ptr)))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "afe_riva_close: Failed in afe_time_srvr_timer_delete() with return code: %d", result);
      }
      qurt_elite_memory_free(pDevPort->timer_ptr);
      pDevPort->timer_ptr = NULL;
   }

   if (NULL != riva_port_ptr->circ_buff.top_addr)
   {
      //change the riva_lpass_data_processing state  to STOP
      qurt_elite_mutex_lock(&riva_port_ptr->riva_drv_mutex);
      riva_port_ptr->riva_lpass_data_proc_state = AFE_RIVA_LPASS_DATA_PROCESSING_STOP;
      qurt_elite_mutex_unlock(&riva_port_ptr->riva_drv_mutex);
      if ((AFE_PORT_ID_INTERNAL_BT_SCO_RX == pDevPort->intf) || (AFE_PORT_ID_INTERNAL_BT_SCO_TX == pDevPort->intf))
      {
         if ((uint32_t)LPASS_HW_VER_3_3_0 == aud_stat_afe_riva_ptr->hw_revision)
         {
            qurt_elite_memory_free(riva_port_ptr->circ_buff.top_addr);
         }
         else
         {
            afe_dev_lpa_aligned_free(riva_port_ptr->circ_buff.top_addr);
         }
      }
      else
      {
         qurt_elite_memory_free(riva_port_ptr->circ_buff.top_addr);
      }
      riva_port_ptr->circ_buff.top_addr = NULL;
   }

   return;
}

/**
 * Get the current time using Riva port data transfer
 * rate
 *
 * @param[in] afe_port_ptr pointer to AFE device port structure
 *
 * @param[out] curr_time - time stamp in sample ticks
 *
 * @return  ADSP_EOK on success, an error code on error
 *
 * @dependencies
 * None.
 *
 */
ADSPResult afe_get_time_for_riva_port(afe_dev_port_t *dev_port, uint64 *curr_time)
{
   if (NULL == dev_port)
   {
      return ADSP_EFAILED;
   }

   *curr_time = ((afe_dev_riva_port_t *)(dev_port->afe_drv))->riva_clock;

   return ADSP_EOK;
}

/**
 * This is a AV-Timer ISR routine to read samples from port circular buffer
 *
 * @param[in] dev_port_ptr pointer to port instance
 * @param[in] dst_ptr pointer to destination buffer
 * @param[in] num_samples number of samples to read from port circular buffer
 */
ADSPResult afe_read_riva(afe_dev_port_t *dev_port_ptr, int16_t *dst_ptr, uint32_t num_samples)
{
   uint64 ll_us;
   uint32_t length;
   afe_dev_riva_port_t *riva_port_ptr;
   int32_t  avt_adj = 0;

   riva_port_ptr = (afe_dev_riva_port_t *)(dev_port_ptr->afe_drv);

   length = num_samples * dev_port_ptr->channels * dev_port_ptr->bytes_per_channel;

   qurt_elite_mutex_lock(&(riva_port_ptr->riva_drv_mutex));

   afe_driver_circ_to_linear(dev_port_ptr->intf, (uint8_t *)dst_ptr, &(riva_port_ptr->circ_buff),  length, FALSE);

   //update the clock.
   //add (1ms +/- delta), depending on the clock drift between riva and av-timer(lpass)
   afe_riva_calc_avt_adj(dev_port_ptr->intf,\
                            &riva_port_ptr->riva_lpass_drift_us,\
                            riva_port_ptr->block_transfer_time, &avt_adj);

   //accumulate the drift for reporting to the afe clients
   riva_port_ptr->acc_avt_drift_us += avt_adj;  //avt_adj +ve => riva is fast.

   ll_us = riva_port_ptr->block_transfer_time_us - avt_adj;

   qurt_elite_mutex_unlock(&(riva_port_ptr->riva_drv_mutex));

   //MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "afe_read:%x: %d",dev_port_ptr->intf,(uint32_t) ll_us);

   ll_us = ll_us + riva_port_ptr->riva_clock;
   if (ll_us < (uint64_t)afe_time_srvr_timer_get_time((avtimer_drv_handle_t)dev_port_ptr->avt_drv))
   {
      /**< Next interrupt time to be programmed is less than current time,
       *  which indicates signal miss. Set is_signal_miss flag and by checking this flag
       *  dynamic thread work loop will be able to detect timer interrupt driven thread's
       *  signal miss
       */
      dev_port_ptr->dynamic_thread_ptr->is_signal_miss = TRUE;
      return ADSP_EFAILED;
   }
   riva_port_ptr->riva_clock = ll_us;
   if (ADSP_EOK != afe_time_srvr_timer_oneshot_start_absolute(dev_port_ptr->timer_ptr, ll_us))
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "afe riva: starting BtVoc/Fm one shot timer  failed\n");
   }
   return ADSP_EOK;
}

/**
 * This is a AV-Timer 1ms ISR routine to write samples to port circular buffer
 *
 * @param[in] riva_port_ptr pointer to port instance
 * @param[in] src_ptr pointer to source buffer
 * @param[in] num_samples number of samples to read from port circular buffer
 */

ADSPResult afe_write_riva(afe_dev_port_t *dev_port_ptr, int16_t *src_ptr, uint32_t num_samples)
{
   uint64 ll_us;
   uint32_t length, temp;
   afe_dev_riva_port_t *riva_port_ptr;
   int32_t  avt_adj = 0;

   riva_port_ptr = (afe_dev_riva_port_t *)(dev_port_ptr->afe_drv);

   temp = dev_port_ptr->channels * dev_port_ptr->bytes_per_channel;
   length = num_samples * temp;
   // The head pointer adjustment should be done to the word aligned location in the case of circular buffer underrun
   temp = temp << 1;

   qurt_elite_mutex_lock(&(riva_port_ptr->riva_drv_mutex));

   afe_driver_linear_to_circ(dev_port_ptr->intf, &(riva_port_ptr->circ_buff), (uint8_t *)src_ptr,  length, temp, FALSE);

   //update the clock.
   //add (1ms +/- delta), depending on the clock drift between riva and av-timer
   afe_riva_calc_avt_adj(dev_port_ptr->intf,\
                            &riva_port_ptr->riva_lpass_drift_us,\
                            riva_port_ptr->block_transfer_time, &avt_adj);
   //accumulate the drift for reporting to the afe clients
   riva_port_ptr->acc_avt_drift_us += avt_adj;  //avt_adj +ve => riva is fast.

   ll_us = riva_port_ptr->block_transfer_time_us - avt_adj;

   qurt_elite_mutex_unlock(&(riva_port_ptr->riva_drv_mutex));

   //MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "afe_write:%x: %d",dev_port_ptr->intf,(uint32_t) ll_us);

   ll_us = ll_us + riva_port_ptr->riva_clock;
   if (ll_us < (uint64_t)afe_time_srvr_timer_get_time((avtimer_drv_handle_t)dev_port_ptr->avt_drv))
   {
      /**< Next interrupt time to be programmed is less than current time,
       *  which indicates signal miss. Set is_signal_miss flag and by checking this flag,
       *  dynamic thread work loop will be able to detect timer interrupt driven thread's
       *  signal miss
       */
      dev_port_ptr->dynamic_thread_ptr->is_signal_miss = TRUE;
      return ADSP_EFAILED;
   }
   riva_port_ptr->riva_clock = ll_us;
   if (ADSP_EOK != afe_time_srvr_timer_oneshot_start_absolute(dev_port_ptr->timer_ptr, ll_us))
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "afe riva: starting BtVoc/Fm one shot timer  failed\n");
   }
   return ADSP_EOK;
}

/**
 * This is a AV-Timer ISR routine to write samples to cmem circular buffer
 *
 * @param[in] dev_port_ptr pointer to port instance
 * @param[in] src_ptr pointer to source buffer
 * @param[in] num_samples number of samples to write to port circular buffer
 */
ADSPResult afe_write_bt_aud(afe_dev_port_t *dev_port_ptr, int16_t *src_ptr, uint32_t num_samples)
{
   int32_t tail_head_gap;
   uint32_t temp, head_va;
   afe_dev_riva_port_t *riva_port_ptr;
   afe_circ_buff_t *cmem_circ_buff_ptr;
   qurt_mem_cache_mode_t riva_cache_attribs = QURT_MEM_CACHE_NONE;
   qurt_perm_t riva_mem_perm = (qurt_perm_t)(QURT_PERM_READ | QURT_PERM_WRITE);
   ADSPResult result = ADSP_EOK;
   uint64_t tail_phy_addr;

   riva_port_ptr = (afe_dev_riva_port_t *)(dev_port_ptr->afe_drv);
   cmem_circ_buff_ptr = &(riva_port_ptr->cmem_circ_buff);

   qurt_elite_mutex_lock(&(riva_port_ptr->riva_drv_mutex));

   //if lpass <-> riva was disconnected or if riva is not active, do nothing.
   if ((TRUE == aud_stat_afe_riva_ptr->is_riva_active) &&\
          (AFE_RIVA_IPC_CONNECT  == riva_port_ptr->riva_connect))
   {
      temp = dev_port_ptr->bytes_per_channel * dev_port_ptr->channels;

      //Check the tail/head circular distance, if
      // 1. enough space for num_samples, write the samples.
      // 2. otherwise, drop the samples.

      /* Not calling HwdDevCfg_GetVirtAddress_Statically_Mapped as it will print debug message for every succesful
           call. This will be flooding of messages for every 1 msec */

      if (ADSP_EOK != (result = qurt_mem_map_static_query((qurt_addr_t *)&head_va,
                                                          (uint32)(*(riva_port_ptr->head_reg_ptr)),
                                                          QURT_MEM_MAP_MIN_PAGE_SIZE,
                                                          riva_cache_attribs,
                                                          riva_mem_perm)))
      {
         /*this condition will never hit, as IPC start command itself will fail if memsection not found  */
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "This memsection corresponding to phys addr = 0x%x is not statically mapped", (uint32)(*(riva_port_ptr->head_reg_ptr)));
         goto bt_aud_write_bail_out;
      }

      cmem_circ_buff_ptr->head_addr = (uint8_t *)head_va;
      tail_head_gap = (int32_t)(cmem_circ_buff_ptr->tail_addr - cmem_circ_buff_ptr->head_addr);

      //number of bytes to move
      num_samples = num_samples * temp;
      // The head pointer adjustment should be done to the word aligned location in the case of circular buffer underrun
      temp = temp << 1;
      if (tail_head_gap < 0)
      {
         tail_head_gap += cmem_circ_buff_ptr->buffer_size;
      }

      if (tail_head_gap < (int32_t)(cmem_circ_buff_ptr->buffer_size - num_samples))
      {
         //Write samples to circular cmem from pSrc.
         afe_driver_linear_to_circ(dev_port_ptr->intf, cmem_circ_buff_ptr, (uint8_t *)(src_ptr), num_samples, temp, TRUE);
         /*get  phy address of CMEM and store in tail ptr.*/
         tail_phy_addr = qurt_elite_memorymap_get_physical_addr((uint32_t)(cmem_circ_buff_ptr->tail_addr));
         if (0 == tail_phy_addr)
         {
            /*this condition will never hit, as IPC start command itself will fail if memsection not found  */
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error in retrieving PA for VA:0x%x ", (uint32)(cmem_circ_buff_ptr->tail_addr));
            result = ADSP_EFAILED;
            goto bt_aud_write_bail_out;
         }
         *(riva_port_ptr->tail_reg_ptr) = (uint32_t)(tail_phy_addr);
      }
      else
      {
         //print under-run message;
         //MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE BT Audio port under-run");
      }
   }
bt_aud_write_bail_out:
   //update the clock
   riva_port_ptr->riva_clock += riva_port_ptr->block_transfer_time_us;
   qurt_elite_mutex_unlock(&(riva_port_ptr->riva_drv_mutex));
   return result;
}

/**
 * This is to read a circular buffer and write to linear buffer
 *
 * LPASS (RIVA terminology is other way round):
 * tail = write ptr
 * head = read ptr
 * tail_head_gap -> filled area.
 *
 * @param[in] dst_linear_buff_ptr pointer to destination linear buffer
 * @param[in] src_circ_buff_ptr source circular buffer to read from
 * @param[in] length length to read
 */
void afe_driver_circ_to_linear(uint16_t port_id, uint8_t *dst_linear_buff_ptr, afe_circ_buff_t *src_circ_buff_ptr, uint32_t length, bool_t aligned)
{

   uint32_t temp;
   int32_t tail_head_gap;

   if (src_circ_buff_ptr->buffer_size < length)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE Riva:%x: source circ buffer size is too short", port_id);
      return;
   }
   /*
        Check the tail/head circular distance, if distance
        1. larger than length, read the samples.
        2. less or equal length, set the linear buffer as zeros and return;
    */

   tail_head_gap = (int32_t)(src_circ_buff_ptr->tail_addr - src_circ_buff_ptr->head_addr);
   if (tail_head_gap < 0)
   {
      tail_head_gap += src_circ_buff_ptr->buffer_size;
      if (tail_head_gap < 0)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE Riva:%x: source circ buffer tail/head corrupted!", port_id);
         return;
      }
   }
   if (tail_head_gap > (int32_t)src_circ_buff_ptr->buffer_size)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE Riva:%x: source circ buffer tail/head corrupted!", port_id);
      return;
   }

   if (tail_head_gap < (int32_t)length)
   {
      memset(dst_linear_buff_ptr, 0, length);
   }
   else
   {
      /*
            The following computing based on:
            1. src_circ_buff->top_addr <= src_circ_buff_ptr->head_addr, src_circ_buff_ptr->tail_addr
               <= src_circ_buff_ptr->bottom_addr
            2. length <= src_circ_buff_ptr->buffer_size
       */

      temp = src_circ_buff_ptr->bottom_addr - src_circ_buff_ptr->head_addr;

      if (temp >= length)
      {
         // if head to bottom has enough space to read
         afe_memcpy(dst_linear_buff_ptr, length, src_circ_buff_ptr->head_addr, length, aligned, port_id);

         src_circ_buff_ptr->head_addr = src_circ_buff_ptr->head_addr + length;

         if (temp == length)
         {
            src_circ_buff_ptr->head_addr = src_circ_buff_ptr->top_addr;
         }
      }
      else
      {
         // if head to bottom does not have enough space to read
         afe_memcpy(dst_linear_buff_ptr, length, src_circ_buff_ptr->head_addr,  temp, aligned, port_id);
         afe_memcpy(dst_linear_buff_ptr + temp, (length - temp), src_circ_buff_ptr->top_addr,  (length - temp), aligned, port_id);

         src_circ_buff_ptr->head_addr = src_circ_buff_ptr->top_addr + (length - temp);
      }
      if ((src_circ_buff_ptr->head_addr) >= (src_circ_buff_ptr->bottom_addr))
      {
         src_circ_buff_ptr->head_addr = src_circ_buff_ptr->top_addr;
      }
   }
   return;
}

/**
 * This is to read a linear buffer and write to circular buffer
 *
 * @param[in] dst_circ_buff_ptr destination circular buffer to write
 * @param[in] src_linear_buff_ptr source linear buffer address
 * @param[in] length length to write
 * @param[in] num_bytes numBytes gap between head and tail, if dst under-run.
 */
void afe_driver_linear_to_circ(uint16_t port_id, afe_circ_buff_t *dst_circ_buff_ptr, uint8_t *src_linear_buff_ptr, uint32_t length, uint32_t num_bytes, bool_t aligned)
{
   int32_t tail_head_gap, temp;

   if (dst_circ_buff_ptr->buffer_size < length)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE Riva:%x source circ buffer size is too short", port_id);
      return;
   }

   /*
        Check the destination tail/head circular distance, if
        1. there is not enough space for writing, update the head address in the ead
        2. otherwise, normal
    */
   tail_head_gap = (int32_t)(dst_circ_buff_ptr->tail_addr - dst_circ_buff_ptr->head_addr);

   if (tail_head_gap < 0)
   {
      tail_head_gap += dst_circ_buff_ptr->buffer_size;
      if (tail_head_gap < 0)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE Riva:%x dst circ buffer tail/head corrupted!", port_id);
         return;
      }
   }
   if (tail_head_gap > (int32_t)dst_circ_buff_ptr->buffer_size)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE Riva:%x dst circ buffer tail/head corrupted!", port_id);
      return;
   }
   /*
       The following computing based on:
       1. dstCircBuf.top_addr <= dstCircBuf.head_addr, dstCircBuf.tail_addr
         <= dstCircBuf.bottom_addr
       2. length <= dstCircBuf.buffer_size
    */

   temp = (int32_t)(dst_circ_buff_ptr->bottom_addr - dst_circ_buff_ptr->tail_addr);

   if (temp >= (int32_t)length)
   {
      // if tail to bottom has enough space to store

      if (src_linear_buff_ptr != NULL)
      {
         afe_memcpy(dst_circ_buff_ptr->tail_addr, temp, src_linear_buff_ptr, length, aligned, port_id);
      }
      else
      {
         memset(dst_circ_buff_ptr->tail_addr, 0, length);
      }
      dst_circ_buff_ptr->tail_addr = dst_circ_buff_ptr->tail_addr + length;
      if (temp == (int32_t)length)
      {
         dst_circ_buff_ptr->tail_addr = dst_circ_buff_ptr->top_addr;
      }
   }
   else
   {
      // if tail to bottom does not have enough space to store
      if (src_linear_buff_ptr != NULL)
      {
         afe_memcpy(dst_circ_buff_ptr->tail_addr, temp, src_linear_buff_ptr, temp, aligned, port_id);
         afe_memcpy(dst_circ_buff_ptr->top_addr,
                    (dst_circ_buff_ptr->bottom_addr - dst_circ_buff_ptr->top_addr),
                    (src_linear_buff_ptr + temp),
                    (length - temp),
                    aligned, port_id);
      }
      else
      {
         memset(dst_circ_buff_ptr->tail_addr, 0, temp);
         memset(dst_circ_buff_ptr->top_addr, 0, (length - temp));
      }
      dst_circ_buff_ptr->tail_addr = dst_circ_buff_ptr->top_addr + (length - temp);
   }
   if ((dst_circ_buff_ptr->tail_addr) >= (dst_circ_buff_ptr->bottom_addr))
   {
      dst_circ_buff_ptr->tail_addr = dst_circ_buff_ptr->top_addr;
   }

   // if dst circular buffer under-run, overwrite the head address.
   if (tail_head_gap >= (int32_t)(dst_circ_buff_ptr->buffer_size - length))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE Riva:%x dst circ buf underrun", port_id);
      dst_circ_buff_ptr->head_addr = dst_circ_buff_ptr->tail_addr + num_bytes;
      temp = dst_circ_buff_ptr->head_addr - dst_circ_buff_ptr->bottom_addr;
      if (temp >= 0)
      {
         dst_circ_buff_ptr->head_addr = dst_circ_buff_ptr->top_addr + temp;
      }
   }
}

/**
 * This is to copy from source circular buffer to destination circular buffer.
 *
 * @param[in] dst_circ_buff_ptr destination circular buffer to write
 * @param[in] src_circ_buff_ptr source circular buffer to read
 * @param[in] length length in bytes to copy
 * @param[in] num_bytes numBytes gap between head and tail, if dst under-run.
 */
void afe_driver_circ_to_circ(uint16_t port_id, afe_circ_buff_t *dst_circ_buff_ptr, afe_circ_buff_t *src_circ_buff_ptr, uint32_t length, uint32_t num_bytes, bool_t aligned)
{
   uint32_t temp_src, temp_dst, dst_circ_buff_total_len;
   int32_t tail_head_gap, temp;

   if ((dst_circ_buff_ptr->buffer_size < length) || (src_circ_buff_ptr->buffer_size < length))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE Riva:%x source/dst circ buffer size is too short", port_id);
      return;
   }

   /*
        Check the source tail/head circular distance, if distance
        1. larger than length, read the samples.
        2. less or equal length, set the linear buffer as zeros and return;
    */

   tail_head_gap = (int32_t)(src_circ_buff_ptr->tail_addr - src_circ_buff_ptr->head_addr);
   if (tail_head_gap < 0)
   {
      tail_head_gap += src_circ_buff_ptr->buffer_size;
      if (tail_head_gap < 0)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE Riva:%x: src circ buffer tail/head corrupted!", port_id);
         return;
      }
   }
   if (tail_head_gap > (int32_t)src_circ_buff_ptr->buffer_size)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE Riva:%x src circ buffer tail/head corrupted!", port_id);
      return;
   }

   if (tail_head_gap < (int32_t)length)
   {
      //if src circular buffer has not enough samples, return or
      //zeros the dst circular buffer?
      MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE Riva:%x source circ buffer has not enough data %ld,%lu", port_id, tail_head_gap, length);
      return;
   }

   /*
        Check the destination tail/head circular distance, if
        1. there is not enough space for writing, update the head address in the ead
        2. otherwise, normal
    */
   tail_head_gap = (int32_t)(dst_circ_buff_ptr->tail_addr - dst_circ_buff_ptr->head_addr);

   if (tail_head_gap < 0)
   {
      tail_head_gap += dst_circ_buff_ptr->buffer_size;
      if (tail_head_gap < 0)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE Riva:%x dst circ buffer tail/head corrupted!", port_id);
         return;
      }
   }
   if (tail_head_gap > (int32_t)dst_circ_buff_ptr->buffer_size)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE Riva:%x dst circ buffer tail/head corrupted!", port_id);
      return;
   }

   /*
       The following computing based on:
       1. dstCircBuf.top_addr <= dstCircBuf.head_addr, dstCircBuf.tail_addr
         <= dstCircBuf.bottom_addr
       2. length <= dstCircBuf.buffer_size
    */
   temp_dst = dst_circ_buff_ptr->bottom_addr - dst_circ_buff_ptr->tail_addr;
   temp_src = src_circ_buff_ptr->bottom_addr - src_circ_buff_ptr->head_addr;

   dst_circ_buff_total_len = (dst_circ_buff_ptr->bottom_addr - dst_circ_buff_ptr->top_addr);

   if (temp_src >= length)
   {
      //src head to bottom has enough space to load
      if (temp_dst >= length)
      {
         //dst tail to bottom has enough space to store
         afe_memcpy(dst_circ_buff_ptr->tail_addr, temp_dst, src_circ_buff_ptr->head_addr, length, aligned, port_id);

         //update the tail_addr of dst circular buffer
         dst_circ_buff_ptr->tail_addr = dst_circ_buff_ptr->tail_addr + length;
      }
      else
      {
         //dst tail to bottom has not enough space to store,copy twice
         afe_memcpy(dst_circ_buff_ptr->tail_addr, temp_dst, src_circ_buff_ptr->head_addr, temp_dst, aligned, port_id);

         afe_memcpy(dst_circ_buff_ptr->top_addr,
                    dst_circ_buff_total_len,
                    src_circ_buff_ptr->head_addr + temp_dst,
                    (length - temp_dst),
                    aligned, port_id);

         //update the tail_addr of dst circular buffer
         dst_circ_buff_ptr->tail_addr = dst_circ_buff_ptr->top_addr + length - temp_dst;
      }
      //update the head_addr of src circular buffer
      src_circ_buff_ptr->head_addr = src_circ_buff_ptr->head_addr + length;
   }
   else
   {
      //src head to bottom has not enough space to load
      if (temp_dst >= length)
      {
         //dst tail to bottom has enough space to store, copy twice
         afe_memcpy(dst_circ_buff_ptr->tail_addr, temp_dst, src_circ_buff_ptr->head_addr, temp_src, aligned, port_id);

         afe_memcpy(dst_circ_buff_ptr->tail_addr + temp_src,
                    (temp_dst - temp_src),
                    src_circ_buff_ptr->top_addr,
                    (length - temp_src),
                    aligned, port_id);

         //update the tail_addr/head_addr of dst/src circular buffer
         dst_circ_buff_ptr->tail_addr = dst_circ_buff_ptr->tail_addr + length;
      }
      else
      {
         //dst tail to bottom has not enough space to store, copy three times
         if (temp_dst > temp_src)
         {
            afe_memcpy(dst_circ_buff_ptr->tail_addr, temp_dst, src_circ_buff_ptr->head_addr, temp_src, aligned, port_id);

            afe_memcpy(dst_circ_buff_ptr->tail_addr + temp_src,
                       (temp_dst - temp_src),
                       src_circ_buff_ptr->top_addr,
                       (temp_dst - temp_src),
                       aligned, port_id);

            afe_memcpy(dst_circ_buff_ptr->top_addr,
                       dst_circ_buff_total_len,
                       src_circ_buff_ptr->top_addr + (temp_dst - temp_src),
                       (length - temp_dst),
                       aligned, port_id);
         }
         else // temp_dst <= temp_src
         {
            afe_memcpy(dst_circ_buff_ptr->tail_addr, temp_dst, src_circ_buff_ptr->head_addr, temp_dst, aligned, port_id);

            if (temp_dst < temp_src)
            {
               afe_memcpy(dst_circ_buff_ptr->top_addr,
                          dst_circ_buff_total_len,
                          src_circ_buff_ptr->head_addr + temp_dst,
                          (temp_src - temp_dst),
                          aligned, port_id);
            }

            afe_memcpy(dst_circ_buff_ptr->top_addr + (temp_src - temp_dst),
                       dst_circ_buff_total_len - (temp_src - temp_dst),
                       src_circ_buff_ptr->top_addr,
                       (length - temp_src),
                       aligned, port_id);
         }

         //update the tail_addr/head_addr of dst/src circular buffer
         dst_circ_buff_ptr->tail_addr = dst_circ_buff_ptr->top_addr + length - temp_dst;
      }
      //update the head_addr of src circular buffer
      src_circ_buff_ptr->head_addr = src_circ_buff_ptr->top_addr + (length - temp_src);
   }

   // Round up to the top if head/tail pointer reach circular bottom.
   if ((dst_circ_buff_ptr->tail_addr) >= (dst_circ_buff_ptr->bottom_addr))
   {
      dst_circ_buff_ptr->tail_addr = dst_circ_buff_ptr->top_addr;
   }

   if ((src_circ_buff_ptr->head_addr) >= (src_circ_buff_ptr->bottom_addr))
   {
      src_circ_buff_ptr->head_addr = src_circ_buff_ptr->top_addr;
   }

   // if dst circular buffer under-run, overwrite the head address.
   if (tail_head_gap > (int32_t)(dst_circ_buff_ptr->buffer_size - length))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE Riva:%x dst circ buf underrun", port_id);
      dst_circ_buff_ptr->head_addr = dst_circ_buff_ptr->tail_addr + num_bytes;
      temp = dst_circ_buff_ptr->head_addr - dst_circ_buff_ptr->bottom_addr;
      if (temp >= 0)
      {
         dst_circ_buff_ptr->head_addr = dst_circ_buff_ptr->top_addr + temp;
      }
   }

   return;
}

/**
 * This is to get number of samples per interrupt, time per sample, samples per ms that depend on sampling rate
 *
 * @param[in] dev_port_ptr pointer to port instance, return Interrupt Samples Number Per Period
 * @param[in] sampleRate sample rate of the port
 */

ADSPResult afe_riva_set_sample_rate_params(afe_dev_riva_port_t  *riva_port_ptr, afe_dev_port_t *afe_dev_port_ptr)
{
   ADSPResult result = ADSP_EOK;

   switch (afe_dev_port_ptr->sample_rate)
   {
      case 8000:
      {
         riva_port_ptr->samples_per_ms            = 8;
         riva_port_ptr->us_per_sample             = 4194304000U;
         break;
      }
      case 16000:
      {
         riva_port_ptr->samples_per_ms            = 16;
         riva_port_ptr->us_per_sample             = 2097152000U;
         break;
      }
      case 32000:
      {
         riva_port_ptr->samples_per_ms            = 32;
         riva_port_ptr->us_per_sample             = 1048576000U;
         break;
      }
      case 48000:
      {
         riva_port_ptr->samples_per_ms            = 48;
         riva_port_ptr->us_per_sample             = 699050667U;
         break;
      }
      default:
      {
         if ((AFE_PORT_ID_INTERNAL_BT_SCO_RX == afe_dev_port_ptr->intf) || (AFE_PORT_ID_INTERNAL_BT_SCO_TX == afe_dev_port_ptr->intf))
         {
            riva_port_ptr->samples_per_ms            = 8;
            riva_port_ptr->us_per_sample             = 4194304000U;
         }
         else
         {
            riva_port_ptr->samples_per_ms            = 48;
            riva_port_ptr->us_per_sample             = 699050667U;
         }
         break;
      }
   }

   result = afe_port_get_frame_size(afe_dev_port_ptr->latency_mode,
                                    afe_dev_port_ptr->sample_rate,
                                    &afe_dev_port_ptr->int_samples_per_period);
   if (ADSP_FAILED(result))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " AFERivaDriver : Invalid sampling rate detected");
      return result;
   }
   return result;
}

/**
 * This is to define the riva port circular buffer
 *
 * @param[in] dev_port_ptr pointer to port instance
 */

void afe_driver_set_circ_buff(afe_dev_port_t *dev_port_ptr)
{
   uint32_t temp = 0;
   afe_circ_buff_t *circ_buff_ptr;
   afe_dev_riva_port_t *riva_port_ptr;
   riva_port_ptr = (afe_dev_riva_port_t *)(dev_port_ptr->afe_drv);

   if ((AFE_PORT_ID_INTERNAL_BT_SCO_RX == dev_port_ptr->intf) || (AFE_PORT_ID_INTERNAL_BT_SCO_TX == dev_port_ptr->intf))
   {
      switch (riva_port_ptr->riva_intr_period)
      {
         case 1250:
         case 2500:
         case 3750:
         {
            /* 6ms circular buffer for 1.25 /2.5 / 3.75 ms case */
            temp = dev_port_ptr->int_samples_per_period * 6 + 2;
            temp = temp * dev_port_ptr->channels * dev_port_ptr->bytes_per_channel;
            break;
         }
         case 7500:
         {
            /* 10ms circular buffer for 7.5ms case */
            temp = dev_port_ptr->int_samples_per_period * 10 + 2;
            temp = temp * dev_port_ptr->channels * dev_port_ptr->bytes_per_channel;
            break;
         }
      }
   }

   if ((AFE_PORT_ID_INTERNAL_FM_RX == dev_port_ptr->intf) || (AFE_PORT_ID_INTERNAL_FM_TX == dev_port_ptr->intf))
   {
      //       temp = riva_port_ptr->data_size +  (((riva_port_ptr->data_size)>>3)<<2); //1.5X size of num per riva interrupt.
      temp = (riva_port_ptr->data_size) << 1;   //2X size of num per riva interrupt.
   }

   circ_buff_ptr = &(riva_port_ptr->circ_buff);

   circ_buff_ptr->bottom_addr = (uint8_t *)circ_buff_ptr->top_addr + temp;
   circ_buff_ptr->head_addr = (uint8_t *)circ_buff_ptr->top_addr;
   circ_buff_ptr->tail_addr = (uint8_t *)circ_buff_ptr->top_addr;
   circ_buff_ptr->buffer_size = temp;
   //flag to adjust for first time both riva is connected and port is in run-state.
   riva_port_ptr->is_adjust = 1;
}

void afe_drv_riva_ssr_cb_register()
{
#ifdef AFE_SYS_MONITOR
   RCECB_HANDLE rc = NULL;

   if (RCECB_NULL == (rc = rcecb_register_context_name(SYS_M_SSR_WCNSS_BEFORE_SHUTDOWN, (void *)afe_riva_crash_handler)))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE Error registering with system monintor for riva shutdown callback.");
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE registered with system monitor for riva shutdown callback. handle=%x", (unsigned int)rc);
      aud_stat_afe_riva.is_crash_handler_registered = TRUE;
   }

   if (RCECB_NULL == (rc = rcecb_register_context_name(SYS_M_SSR_WCNSS_AFTER_POWERUP, (void *)afe_riva_recovery_handler)))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE Error registering with system monintor for riva powerup callback.");
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE registered with system monitor for riva powerup callback. handle=%x", (unsigned int)rc);
      aud_stat_afe_riva.is_recovery_handler_registered = TRUE;
   }
#endif // AFE_SYS_MONITOR
}

void afe_drv_riva_ssr_cb_deregister()
{
#ifdef AFE_SYS_MONITOR
   RCECB_HANDLE rc = NULL;

   if (aud_stat_afe_riva.is_crash_handler_registered)
   {
      if (RCECB_NULL == (rc = rcecb_unregister_context_name(SYS_M_SSR_WCNSS_BEFORE_SHUTDOWN, (void *)afe_riva_crash_handler)))
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE Error registering with system monintor for riva shutdown callback.");
      }
      else
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE registered with system monitor for riva shutdown callback. handle=%x", (unsigned int)rc);
         aud_stat_afe_riva.is_crash_handler_registered = FALSE;
      }
   }

   if (aud_stat_afe_riva.is_recovery_handler_registered)
   {
      if (RCECB_NULL == (rc = rcecb_unregister_context_name(SYS_M_SSR_WCNSS_AFTER_POWERUP, (void *)afe_riva_recovery_handler)))
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE Error registering with system monintor for riva powerup callback.");
      }
      else
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE registered with system monitor for riva powerup callback. handle=%x", (unsigned int)rc);
         aud_stat_afe_riva.is_recovery_handler_registered = FALSE;
      }
   }
#endif // AFE_SYS_MONITOR
}
/**
 * For providing the delay introduced in RIVA port circular
 * buffer
 *
 * @param[in] port_id port id of the rt_proxy port
 *@param[out] circ_buf_delay_us is the delay introduced due to  
 *  	circular buffer

 * @return  ADSP_EOK on success, an error code on error
 *
 * @dependencies
 * None.
 *
 */
ADSPResult afe_driver_riva_circ_buf_delay(afe_dev_port_t *pDevPort, uint64_t *circ_buf_delay_us)
{
   afe_dev_riva_port_t 		*riva_port_ptr = NULL;

   // Replicating the logic for the distance between head and tail pointers from Riva driver
   riva_port_ptr = (afe_dev_riva_port_t *)(pDevPort->afe_drv);
   *circ_buf_delay_us = (((((riva_port_ptr->circ_buff.buffer_size) >> 4) << 2) / (pDevPort->channels * pDevPort->bytes_per_channel)) * 1000) / pDevPort->int_samples_per_period;

   return ADSP_EOK;
}


