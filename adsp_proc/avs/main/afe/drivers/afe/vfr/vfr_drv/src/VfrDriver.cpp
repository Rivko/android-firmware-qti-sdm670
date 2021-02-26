/*==============================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/vfr/vfr_drv/src/VfrDriver.cpp#1 $
$DateTime: 2017/11/07 23:00:25 $
$Author: pwbldsvc $
$Change: 14795016 $
$Revision: #1 $

FILE:     AFEDeviceDriver.cpp

DESCRIPTION: Main Interface to the AFE Device driver

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

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/vfr/vfr_drv/src/VfrDriver.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
11/06/07    RA      Created file

============================================================================*/

/*=====================================================================
 Includes
 ======================================================================*/
#include "AFEDeviceDriver.h"
#include "AFERivaDriver.h"
#include "AFESlimbusDriver.h"
#include "AFERtProxyDriver.h"
#include "AFEPseudoPort.h"
#include "common.h"
#include "VFRDriver_i.h"
#include "VFRIsrHandler_i.h"
#include "AfeTimeStampDriver.h"
#include "AFEInternal.h"
#include "hwd_devcfg.h"

/*=====================================================================
 Globals
 ======================================================================*/

static vfr_isr_handler_t vfr_isr_handler_ptr[] =
{
   vfr_src_idx_0_isr_handler,        //0
   vfr_src_idx_1_isr_handler         //1
};

uint32_t *vfr_irq_src_map;

/*=====================================================================
 Functions
 ======================================================================*/
ADSPResult vfr_device_cfg_init(vfr_drv_state_t *p_vfr_driver)
{
   DALSYSPropertyVar                prop_var;
   lpasshwio_vfr_prop_t             *p_vfr_prop_dev_cfg = NULL;
   uint16_t                         index;
   vfr_src_state_t                  *vfr_state_ptr;
   vfr_src_prop_t                   *p_vfr_src_prop = NULL,*p_vfr_src_dev_cfg = NULL;
   const char                       *devId = "VFR";

   DALSYS_PROPERTY_HANDLE_DECLARE(pHandle);

   /* Get device handle */
   if (DAL_SUCCESS != DALSYS_GetDALPropertyHandleStr(devId, pHandle))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VFR DevCfg: Error getting VFR device handle");
      return ADSP_EFAILED;
   }
   else
   {
      /* Read property ptr */
      if (DAL_SUCCESS != DALSYS_GetPropertyValue(pHandle, "VfrPropStructPtr", 0, &prop_var))
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VFR DevCfg: Error getting VFR Property Struct Ptr");
         return ADSP_EFAILED;
      }
      else
      {
         p_vfr_prop_dev_cfg = (lpasshwio_vfr_prop_t *)prop_var.Val.pStruct;

         /* VFR hw revision */
         if (0 == p_vfr_prop_dev_cfg->hw_revision)
         {
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VFR DevCfg: Not HW supported");
            return ADSP_EUNSUPPORTED;
         }

         p_vfr_driver->hw_info.hw_revision = p_vfr_prop_dev_cfg->hw_revision;

         /* VFR register base address */
         p_vfr_driver->hw_info.reg_base_addr = p_vfr_prop_dev_cfg->reg_base_addr;

         /* VFR register page size */
         p_vfr_driver->hw_info.reg_size = p_vfr_prop_dev_cfg->reg_size;

         /* VFR Interrupt type */
         p_vfr_driver->hw_info.vfr_irq_type = p_vfr_prop_dev_cfg->vfr_irq_type;
         
         /* VFR register vfr_irq_mux_offset1 */
         p_vfr_driver->hw_info.vfr_ts_mux_offset1 = p_vfr_prop_dev_cfg->vfr_ts_mux_offset1_addr;

         /* VFR registergp_irq_ts_mux_offset0*/
         p_vfr_driver->hw_info.gp_ts_mux_offset0 = p_vfr_prop_dev_cfg->gp_ts_mux_offset0_addr;

         /* VFR register gp_irq_ts_mux_offset1 */
         p_vfr_driver->hw_info.gp_ts_mux_offset1 = p_vfr_prop_dev_cfg->gp_ts_mux_offset1_addr;

         /* Initialize the number of h/w timestamped VFR sources available */
         p_vfr_driver->hw_info.num_vfr_ts_mux = p_vfr_prop_dev_cfg->num_vfr_ts_mux;

         /* Initialize the number of general purpose TS h/w available */
         p_vfr_driver->hw_info.num_gp_ts_mux = p_vfr_prop_dev_cfg->num_gp_ts_mux;

         /* HW latching version for VFR. This is the same for all VFR soruces
          * It can be either progress counter based on AV-timer based*/
         p_vfr_driver->hw_latch_ver = p_vfr_prop_dev_cfg->hw_latch_ver;

         /* Configure the individual VFR src hw properties */
         for (index = 0; index < NUM_MAX_VFR_SRC; index++)
         {
            /* Store the available l2vic # */
            p_vfr_driver->hw_info.available_l2vic_num[index] = p_vfr_prop_dev_cfg->l2vic_num[index];

            vfr_state_ptr = &p_vfr_driver->vfr_state_info[index];

            p_vfr_src_prop = &(vfr_state_ptr->vfr_prop);
            p_vfr_src_dev_cfg = &(p_vfr_prop_dev_cfg->vfr_src_prop[index]);

            /* VFR IRQ Mux Control select */
            p_vfr_src_prop->irq_mux_ctl_sel = p_vfr_src_dev_cfg->irq_mux_ctl_sel;

            /* Flag for VFR source supported or not */
            p_vfr_src_prop->is_supported = p_vfr_src_dev_cfg->is_supported;
         }
      }
   }

   return ADSP_EOK;
}

ADSPResult vfr_device_cfg_deinit(vfr_drv_state_t *p_vfr_driver)
{
   p_vfr_driver->hw_info.hw_revision = 0;

   return ADSP_EOK;
}

ADSPResult vfr_driver_init(vfr_drv_state_t *p_vfr_driver)
{
   ADSPResult                 result = ADSP_EOK;
   vfr_hw_info_t              *p_vfr_hw_info = NULL;
   uint8_t                    vfr_src_idx, mux_ctl_sel;
   vfr_src_state_t            *vfr_state_ptr;

   p_vfr_hw_info = &p_vfr_driver->hw_info;

   if (ADSP_EOK != (result = HwdDevCfg_GetVirtAddress(&(p_vfr_hw_info->vfr_qmem_region),
                                                      p_vfr_hw_info->reg_base_addr,
                                                      p_vfr_hw_info->reg_size,
                                                      &p_vfr_hw_info->reg_virt_base)))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VFR Driver: Error obtaining VFR Register virtual addr");

      return ADSP_EFAILED;
   }

   /* Invalidate all the VFR mux control sel indices */
   for (mux_ctl_sel = 0; mux_ctl_sel < VFR_IRQ_SRC_SIZE; mux_ctl_sel++)
   {
      p_vfr_driver->vfr_irq_src_map[mux_ctl_sel] = VFR_IRQ_SRC_INVALID;
   }

   /* Init the pointer to be used by client for accesing VFR source object array */
   vfr_irq_src_map = p_vfr_driver->vfr_irq_src_map;

   /* Initialize the h/w supported VFR sources */
   for (vfr_src_idx = 0; vfr_src_idx < NUM_MAX_VFR_SRC; vfr_src_idx++)
   {
      vfr_state_ptr = &p_vfr_driver->vfr_state_info[vfr_src_idx];

      /* Check if this VFR source is hw supported */
      if (TRUE == vfr_state_ptr->vfr_prop.is_supported)
      {
         /* Initialize the VFR source index */
         vfr_state_ptr->vfr_src_index = vfr_src_idx;

         /* Set default state to disabled */
         vfr_state_ptr->is_vfr_enabled = FALSE;

         /* Assign the ISR function pointer corresponding to this VFR */
         vfr_state_ptr->vfr_isr_handler = vfr_isr_handler_ptr[vfr_src_idx];

         p_vfr_driver->vfr_irq_src_map[vfr_state_ptr->vfr_prop.irq_mux_ctl_sel] = vfr_src_idx;

         qurt_elite_mutex_init(&vfr_state_ptr->vfr_mutex);

         vfr_state_ptr->is_initialized = TRUE;

         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VFR Driver Init: vfr_src_idx: %u, irq_mux_ctl_sel: %lu",
               vfr_src_idx, vfr_state_ptr->vfr_prop.irq_mux_ctl_sel);
      }
      else /* Not hw supported */
      {
         vfr_state_ptr->is_initialized = FALSE;
      }
   }

   /* Initalize the timestamping h/w state */
   afe_ts_drv_init(&p_vfr_driver->hw_info);

   return result;
}

ADSPResult vfr_driver_deinit(vfr_drv_state_t *p_vfr_driver)
{
   uint32_t          src_idx;

   if (p_vfr_driver->hw_info.vfr_qmem_region)
   {
      HwdDevCfg_DestoryVirtAddress(p_vfr_driver->hw_info.vfr_qmem_region);
      p_vfr_driver->hw_info.vfr_qmem_region = 0;
   }

   for (src_idx = 0; src_idx < NUM_MAX_VFR_SRC; src_idx++)
   {
      if (TRUE == p_vfr_driver->vfr_state_info[src_idx].is_initialized)
      {
         qurt_elite_mutex_destroy(&p_vfr_driver->vfr_state_info[src_idx].vfr_mutex);
      }
   }

   vfr_device_cfg_deinit(p_vfr_driver);

   return ADSP_EOK;
}

/**
 * This function Enables VFR.
 * @param[in] vfr_source - internal or external VFR source
 * @return the ADSP_EOK error code
 */
ADSPResult vfr_enable_src_driver(vfr_drv_state_t *p_vfr_drv, vfr_src_state_t *vfr_state_ptr)
{
   ADSPResult           result = ADSP_EOK;

   /*open av timer*/
   avtimer_open_param_t open_param;
   open_param.client_name = (char *)"vfr";
   open_param.flag = 0;

   if (ADSP_EOK != (result = avtimer_drv_hw_open(&vfr_state_ptr->avt_drv_hdl, &open_param)))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to Open AV Timer Driver for VFR source");
      return ADSP_EFAILED;
   }

   /* Clear pending signals if any */
   qurt_elite_signal_clear(&(vfr_state_ptr->vfr_signal));

   if (ADSP_EOK != (result = vfr_register_callback(p_vfr_drv, vfr_state_ptr, &p_vfr_drv->hw_info)))
   {
      /* Mark that mux as available */
      afe_ts_drv_release_ts_mux(vfr_state_ptr->mux_id);

      vfr_state_ptr->is_hw_latched = FALSE;

      /* Close AVT Driver */
      avtimer_drv_hw_close(vfr_state_ptr->avt_drv_hdl);
      vfr_state_ptr->avt_drv_hdl = NULL;

      return ADSP_EFAILED;
   }

   /* Initialize the VFR driver state */
   vfr_state_ptr->cur_av_time_stamp = 0;
   vfr_state_ptr->prev_av_time_stamp = 0;
   vfr_state_ptr->is_first_vfr = TRUE;
   vfr_state_ptr->is_vfr_enabled = TRUE;
   vfr_state_ptr->vfr_period = VFR_FRAME_SIZE_IN_US;

   MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VFR svc enabled for src: %d, is_hw_latch: %u, irq_mux_ctl_sel: %lu, l2vic_num: %lu",
         vfr_state_ptr->vfr_src_index, vfr_state_ptr->is_hw_latched, vfr_state_ptr->vfr_prop.irq_mux_ctl_sel, vfr_state_ptr->l2vic_num);

   return result;
}

/**
 * This function Disable VFR on AFEDAL.
 * @param[in] vfr_source - internal or external VFR source
 * @return the ADSP_EOK error code
 */
ADSPResult vfr_disable_src_driver(vfr_drv_state_t *p_vfr_drv, vfr_src_state_t *vfr_state_ptr)
{
   /* De-register the call back */
   vfr_deregister_callback(vfr_state_ptr);

   /* Clear pending signals if any */
   qurt_elite_signal_clear(&vfr_state_ptr->vfr_signal);

   /*mark that mux as available*/
   afe_ts_drv_release_ts_mux(vfr_state_ptr->mux_id);

   /* De-initialize the vfr state */
   vfr_state_ptr->cur_av_time_stamp = 0;
   vfr_state_ptr->is_first_vfr = FALSE;
   vfr_state_ptr->is_vfr_enabled = FALSE;
   vfr_state_ptr->prev_av_time_stamp = 0;
   vfr_state_ptr->vfr_client_info.vfr_clientq = NULL;
   vfr_state_ptr->vfr_client_info.vfr_source = 0;
   vfr_state_ptr->is_hw_latched = FALSE;

   //clse AVT Driver
   avtimer_drv_hw_close(vfr_state_ptr->avt_drv_hdl);
   vfr_state_ptr->avt_drv_hdl = NULL;

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VFR svc disabled for src: %d", vfr_state_ptr->vfr_src_index);

   return ADSP_EOK;
}

void vfr_driver_get_vfr_drift_timestamp_based(afe_dev_port_t *afe_port_ptr, vfr_src_state_t *vfr_state_ptr)
{

   afe_drift_info_t     *p_drift_info;
   uint64_t             av_time_vfr = 0, vfr_time = 0, av_time_dev = 0, dev_time = 0;
   int32_t              acc_drift_in_us = 0;
   uint32_t             vfr_frame_counter;
   uint32_t             interrupt_period = 0;
   int32_t              fifo_diff;
   int64_t              fifo_duration;

#ifdef VFR_DEBUG_MSG
   int32_t acc_drift_in_us_chk = 0;
#endif

   /* For RIVA, interrupt period is the RIVA interrupt period, for all other interfaces
      interrupt period is DMA period (block transfer time) */
   if (IS_RIVA_PORT_AFE_ID(afe_port_ptr->intf))
   {
      afe_dev_riva_port_t *riva_port_ptr = (afe_dev_riva_port_t *)(afe_port_ptr->afe_drv);
      interrupt_period = riva_port_ptr->riva_intr_period;
   }

   p_drift_info = &afe_port_ptr->drift_info[vfr_state_ptr->vfr_src_index];

   /* If this is the first VFR for this port from the particular VFR source */
   if ((TRUE == vfr_state_ptr->is_first_vfr) ||
       ((p_drift_info->running_dev_int_counter >= 1) && (TRUE == p_drift_info->is_first_vfr)))
   {
      if ((p_drift_info->running_dev_int_counter >= 1) && (TRUE == p_drift_info->is_first_vfr))
      {
         p_drift_info->is_first_vfr = FALSE;
      }

      afe_drv_resync_port_vfr_drift_info(p_drift_info);

      /* Rest the VFR init TS with latest timestamp value */
      p_drift_info->init_vfr_av_timestamp = vfr_state_ptr->cur_av_time_stamp;
   }

   if ((FALSE == vfr_state_ptr->is_first_vfr) && (FALSE == p_drift_info->is_first_vfr))
   {
      /* Total AV time elapsed since first VFR interrupt for the device */
      av_time_vfr = vfr_state_ptr->cur_av_time_stamp - p_drift_info->init_vfr_av_timestamp;

      /* Total time in VFR domain */
      vfr_time = p_drift_info->running_vfr_int_counter *  vfr_state_ptr->vfr_period;

      /* Device time and device interrupt counter are incremented atomically */
      /* Total AV time elapsed since device started with this VFR */
      av_time_dev = p_drift_info->latest_dev_av_timestamp - p_drift_info->init_av_timestamp;

      /* Total time in dev domain. New VFR timestamping logic assumes hardware timestamping of
         DMA interrupts is also available */
      if (IS_RIVA_PORT_AFE_ID(afe_port_ptr->intf))
      {
         dev_time = p_drift_info->running_dev_int_counter * interrupt_period;

      }
      else
      {
         dev_time = ((p_drift_info->running_dev_int_counter) * (uint64_t)(afe_port_ptr->int_samples_per_period * NUM_US_PER_SEC) / afe_port_ptr->sample_rate);
      }

      /* Need to also account FIFO samples in dev time calculation */
      fifo_diff = p_drift_info->latest_fifo_samples - p_drift_info->init_fifo_samples;
      fifo_duration = ((int64_t)fifo_diff * NUM_US_PER_SEC) / (afe_port_ptr->channels * afe_port_ptr->sample_rate);

      if (IS_IT_TX_PORT_AFE_ID(afe_port_ptr->intf))
      {
         /* For TX port:
            delta = dev_time - init_dev_time = (per_cnt + fifo_count) - (init_per_cnt + init_fifo_count)
                  = (per_cnt - init_per_cnt) + (fifo_count - init_fifo_count) */
         dev_time = dev_time + fifo_duration;
      }
      else
      {
         /* For RX port:
            delta = dev_time - init_dev_time = (per_cnt - fifo_count) - (init_per_cnt - init_fifo_count)
                  = (per_cnt - init_per_cnt) - (fifo_count - init_fifo_count) */
         dev_time = dev_time - fifo_duration;
      }

      acc_drift_in_us = (vfr_time - av_time_vfr) - (dev_time - av_time_dev);

#ifdef VFR_DEBUG_MSG
      acc_drift_in_us_chk = vfr_time - (dev_time * av_time_vfr) / av_time_dev;
#endif
   }


#ifdef VFR_DEBUG_MSG
   MSG_7(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
         "Port-id: 0x%x, vfr_src_idx: %u, VFR Time: %lu, Dev time: %lu, AVT VFR: %lu, AVT Dev: %lu, Acc drift: %d,",
         afe_port_ptr->intf, vfr_state_ptr->vfr_src_index, (uint32_t)vfr_time, (uint32_t)dev_time,
         (uint32_t)av_time_vfr, (uint32_t)av_time_dev, (int)acc_drift_in_us);

   MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
         "Port-id: 0x%x, vfr_src_idx: %u, Acc drift Chk: %d",
         afe_port_ptr->intf, vfr_state_ptr->vfr_src_index, (int)acc_drift_in_us_chk);
#endif

   p_drift_info->running_vfr_int_counter++;

   /* Update vfr frame ctr and acc drift info */
   if (AFE_PORT_STATE_RUN == afe_port_ptr->port_state)
   {
      /* Increment the VFR frame counter */
      vfr_frame_counter = (uint32_t)(p_drift_info->vfr_drift_info) + 1;

      /* Update the VFR drift info */
      p_drift_info->vfr_drift_info = (uint64_t)(((int64_t)acc_drift_in_us << 32) | ((uint64_t)vfr_frame_counter));
   }

   return;
}

ADSPResult vfr_driver_get_afe_port_acc_drift(afe_dev_port_t *afe_port_ptr, vfr_src_state_t *vfr_state_ptr)
{
   uint16_t vfr_src_idx = vfr_state_ptr->vfr_src_index;

   if ((IS_SLIMBUS_PORT_AFE_ID(afe_port_ptr->intf)) ||
       (IS_RIVA_PORT_AFE_ID(afe_port_ptr->intf)) ||
       (IS_AUDIO_IF_PORT_AFE_ID(afe_port_ptr->intf)) ||
       (IS_AUDIO_IF_TDM_PORT_AFE_ID(afe_port_ptr->intf)) ||
       (IS_HDMI_OUTPUT_PORT_AFE_ID(afe_port_ptr->intf)) ||
       (IS_HDMI_OVER_DP_PORT_AFE_ID(afe_port_ptr->intf)) ||
       (IS_USBA_PORT_AFE_ID(afe_port_ptr->intf)))
   {
      vfr_driver_get_vfr_drift_timestamp_based(afe_port_ptr, vfr_state_ptr);
   }
   else if (IS_RT_PROXY_PORT_AFE_ID(afe_port_ptr->intf))
   {
      afe_rt_proxy_get_vfr_drift(afe_port_ptr, vfr_state_ptr);
   }
   else if (IS_PSEUDO_PORT_AFE_ID(afe_port_ptr->intf))
   {
      afe_pseudoport_get_vfr_drift(afe_port_ptr, vfr_state_ptr);
   }
   else
   {
      return ADSP_EFAILED;
   }

#if defined(PRINT_AVTIMER_DRV_DBG_MSG)

   /* Print once every VFR period */
   if (afe_port_ptr->intf & 0x1)
   {
      avtimer_server_dump_dbg_msg();
   }

#endif /* #if defined(PRINT_AVTIMER_DRV_DBG_MSG) */

   MSG_4(MSG_SSID_QDSP6, DBG_MED_PRIO, "Accumulated drift: %ld, VFR int: %ld, intf: 0x%x, vfr_src_idx: %u",
         (int32_t)((afe_port_ptr->drift_info[vfr_src_idx].vfr_drift_info) >> 32),
         (uint32_t)(afe_port_ptr->drift_info[vfr_src_idx].vfr_drift_info),
         afe_port_ptr->intf, vfr_state_ptr->vfr_src_index);

   return ADSP_EOK;
}

void afe_port_pool_vfr_drift_update(vfr_src_state_t *vfr_state_ptr, afe_dev_port_t **afe_port_pool_ptr,
                                    uint16_t port_id_range_size)
{
   uint16_t          port_id_index;
   afe_dev_port_t    *dev_port_ptr = NULL;
   uint16_t          vfr_src_idx = vfr_state_ptr->vfr_src_index;

   for (port_id_index = 0; port_id_index < port_id_range_size; port_id_index++)
   {
      dev_port_ptr = *(afe_port_pool_ptr + port_id_index);

      if (NULL != dev_port_ptr)
      {
         if (TRUE == dev_port_ptr->drift_info[vfr_src_idx].port_vfr_mutex_init)
         {
            qurt_elite_mutex_lock(&dev_port_ptr->drift_info[vfr_src_idx].port_vfr_mutex);

            if (TRUE == dev_port_ptr->drift_info[vfr_src_idx].vfr_enabled)
            {
               vfr_driver_get_afe_port_acc_drift(dev_port_ptr, vfr_state_ptr);
            }

            qurt_elite_mutex_unlock(&dev_port_ptr->drift_info[vfr_src_idx].port_vfr_mutex);
         }
      }
   }

   return;
}

void vfr_driver_calc_drift_updates(aud_stat_afe_svc_t *afe_svc_global_ptr, vfr_src_state_t *vfr_state_ptr,
                                   uint8_t *resync_status)
{
   int64_t vfr_drift;

   /* Update VFR drift info for AUDIO IF ports, if VFR feedback is enabled */
   afe_port_pool_vfr_drift_update(vfr_state_ptr, afe_svc_global_ptr->ports_audioif_ptr,
                                  AFE_PORT_ID_AUDIO_IF_PORT_RANGE_SIZE);

   /* Update VFR drift info for TDM IF ports, if VFR feedback is enabled */
   afe_port_pool_vfr_drift_update(vfr_state_ptr, afe_svc_global_ptr->ports_audioif_tdm_ptr,
                                  afe_svc_global_ptr->num_audioif_tdm_port);

   /* Update VFR drift info for SLIMBUS ports, if VFR feedback is enabled */
   afe_port_pool_vfr_drift_update(vfr_state_ptr, afe_svc_global_ptr->ports_slimbus_ptr,
                                  AFE_PORT_ID_SLIMBUS_RANGE_SIZE);

   /* Update VFR drift info for RIVA ports, if VFR feedback is enabled */
   afe_port_pool_vfr_drift_update(vfr_state_ptr, afe_svc_global_ptr->ports_riva_ptr,
                                  AFE_PORT_ID_INTERNAL_BT_FM_RANGE_SIZE);

   /* Update VFR drift info for RT Proxy ports, if VFR feedback is enabled */
   afe_port_pool_vfr_drift_update(vfr_state_ptr, afe_svc_global_ptr->port_rt_proxy_ptr,
                                  AFE_PORT_ID_RT_PROXY_PORT_RANGE_SIZE);

   /* Update VFR drift info for pseudoports, if VFR feedback is enabled (for pseudoports VFR drift = drift of avtimer wrt VFR) */
   afe_port_pool_vfr_drift_update(vfr_state_ptr, afe_svc_global_ptr->pseudo_ports_ptr,
                                  AFE_PORT_ID_PSEUDOPORT_RANGE_SIZE);

   /* Update VFR drift info for HDMI multi-stream ports, if VFR feedback is enabled */
   afe_port_pool_vfr_drift_update(vfr_state_ptr, afe_svc_global_ptr->port_hdmi_output_ptr,
                                  AFE_PORT_ID_HDMI_MULTISTREAM_RANGE_SIZE / 2);

   /* Update VFR drift info for USB audio ports, if VFR feedback is enabled */
   afe_port_pool_vfr_drift_update(vfr_state_ptr, afe_svc_global_ptr->ports_usba_ptr,
                                  AFE_PORT_ID_USB_AUDIO_RANGE_SIZE);

   /* Calculate vfr drift */
   vfr_drift  = (vfr_state_ptr->cur_av_time_stamp - vfr_state_ptr->prev_av_time_stamp) - VFR_FRAME_SIZE_IN_US;

   if (TRUE == vfr_state_ptr->is_first_vfr)
   {
      vfr_state_ptr->is_first_vfr = FALSE;
      vfr_drift = 0;
   }

   /* Detect the resync situation */

   *resync_status = FALSE;
   if ((vfr_drift > 500) || (vfr_drift < -500))
   {
      *resync_status = TRUE;
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Resync detected, vfr_drift: %ld us", (int32_t)vfr_drift);
   }

   /* Update the prev av timestamp value */
   vfr_state_ptr->prev_av_time_stamp = vfr_state_ptr->cur_av_time_stamp;

   return;
}

