/*==============================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/riva/src/AFERivaAudSync.cpp#1 $
$DateTime: 2017/11/07 23:00:25 $
$Author: pwbldsvc $
$Change: 14795016 $
$Revision: #1 $

FILE:     AFERivaAudSync.cpp

DESCRIPTION: Implements timing calculations required for riva: such as VFR, rate matching etc using aud_sync block in lpass.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright 2011 Qualcomm Technologies Incorporated.All Rights Reserved.
QUALCOMM Proprietary/GTDR
==============================================================================*/
/*============================================================================
EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order. Please
use ISO format for dates.

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/riva/src/AFERivaAudSync.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
06/02/10    rb      Created file

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
#include "avtimer_hal.h"
#include "avtimer_drv_api.h"

//#define IPC_DRIFT_DEBUG
//#define AVT_DRIFT_DEBUG
#define VFR_DRIFT_DEBUG


/*=====================================================================
 Globals
 ======================================================================*/

/*=====================================================================
 Functions
 ======================================================================*/


/**
 * todo: since mute/unmute is typically used for BT hand-over, during this time,
 * complete reset of aud_sync block is not necessary. However, in this implementation,
 * that's we have done it
 */
ADSPResult afe_riva_start_bt_aud_sync(uint32_t sample_rate, afe_dev_riva_port_t *tx_riva_port_ptr, afe_dev_riva_port_t *rx_riva_port_ptr)
{
   ADSPResult result = ADSP_EOK;

   tx_riva_port_ptr->riva_lpass_drift_us  = 0;
   rx_riva_port_ptr->riva_lpass_drift_us  = 0;
   tx_riva_port_ptr->acc_avt_drift_us  = 0;
   rx_riva_port_ptr->acc_avt_drift_us  = 0;
   tx_riva_port_ptr->is_first_riva_lpass_sync = TRUE;
   rx_riva_port_ptr->is_first_riva_lpass_sync = TRUE;
   aud_stat_afe_riva_ptr->drift_correction_us = 0;

   qurt_elite_mutex_lock(&aud_stat_afe_riva_ptr->aud_sync_mutex);

   // Open AV timer driver
   avtimer_open_param_t open_param;
   open_param.client_name = (char *)"AfeSvc";
   open_param.flag = 0;
   tx_riva_port_ptr->avt_drv = NULL;
   rx_riva_port_ptr->avt_drv = NULL;
   if (ADSP_EOK != (result = avtimer_drv_hw_open((avtimer_drv_handle_t *)&(rx_riva_port_ptr->avt_drv), &open_param)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to Open AVTimer driver for IntBT Rx, error: %d", result);
      goto bail_out;
   }
   tx_riva_port_ptr->avt_drv = rx_riva_port_ptr->avt_drv;

   //Enable the clks needed for BT Aud Sync block
   avtimer_drv_ioctl_prop_info_t prop_info;
   prop_info.prop = AVTIMER_DRV_PROPERTY_ENABLE_AVSYNC_BT;
   if (ADSP_EOK != (result = avtimer_drv_ioctl(rx_riva_port_ptr->avt_drv, &prop_info)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to enable BT clks, error: %d", result);
      goto bail_out;
   }

   hal_aud_sync_reset_tracking(HAL_AUD_SYNC_INTF_BT_1);
   hal_aud_sync_unreset_tracking(HAL_AUD_SYNC_INTF_BT_1);
   //load ref inc count: depends on sampling rate * block transfer len b/w RIVA <-> LPASS
   hal_aud_sync_set_ref_cnt_inc(HAL_AUD_SYNC_INTF_BT_1, rx_riva_port_ptr->data_size>>1); //TODO: is 8bit also supported, or only 16bit?
   //set XO DIV val: depends on sampling rate and clock crystal
   hal_aud_sync_set_xo_div_val(HAL_AUD_SYNC_INTF_BT_1, sample_rate);
   //enable XO DIV & tracking
   hal_aud_sync_enable_tracking(HAL_AUD_SYNC_INTF_BT_1);
   aud_stat_afe_riva_ptr->is_bt_aud_sync_enable = TRUE;

bail_out:
   if (ADSP_EOK != result)
   {
      if (NULL != rx_riva_port_ptr->avt_drv)
      {
         avtimer_drv_hw_close((avtimer_drv_handle_t)rx_riva_port_ptr->avt_drv);
         rx_riva_port_ptr->avt_drv = NULL;
         tx_riva_port_ptr->avt_drv = NULL;
      }
   }
   qurt_elite_mutex_unlock(&aud_stat_afe_riva_ptr->aud_sync_mutex);

   return result;
}

void afe_riva_stop_bt_aud_sync(afe_dev_riva_port_t *tx_riva_port_ptr, afe_dev_riva_port_t *rx_riva_port_ptr)
{
   tx_riva_port_ptr->riva_lpass_drift_us  = 0;
   rx_riva_port_ptr->riva_lpass_drift_us  = 0;
   tx_riva_port_ptr->is_first_riva_lpass_sync = TRUE;
   rx_riva_port_ptr->is_first_riva_lpass_sync = TRUE;
   aud_stat_afe_riva_ptr->drift_correction_us = 0;

   qurt_elite_mutex_lock(&aud_stat_afe_riva_ptr->aud_sync_mutex);

   //disable XO DIV and tracking
   hal_aud_sync_disable_tracking(HAL_AUD_SYNC_INTF_BT_1);

   //reset the ref cnt
   hal_aud_sync_set_ref_cnt_inc(HAL_AUD_SYNC_INTF_BT_1, 0);

   //reset bit in ctl
   hal_aud_sync_reset_tracking(HAL_AUD_SYNC_INTF_BT_1);

   // Close the AVT driver on Rx path
   avtimer_drv_ioctl_prop_info_t prop_info;
   if (NULL != rx_riva_port_ptr->avt_drv)
   {
      prop_info.prop = AVTIMER_DRV_PROPERTY_DISABLE_AVSYNC_BT;
      avtimer_drv_ioctl(rx_riva_port_ptr->avt_drv, &prop_info);
      avtimer_drv_hw_close((avtimer_drv_handle_t)rx_riva_port_ptr->avt_drv);
      rx_riva_port_ptr->avt_drv = NULL;
      tx_riva_port_ptr->avt_drv = NULL;
   }
   aud_stat_afe_riva_ptr->is_bt_aud_sync_enable = FALSE;
   qurt_elite_mutex_unlock(&aud_stat_afe_riva_ptr->aud_sync_mutex);
}

ADSPResult afe_riva_start_fm_aud_sync(uint32_t sample_rate, afe_dev_riva_port_t *fm_port_ptr, uint32_t  samples_per_riva_int)
{
   ADSPResult result = ADSP_EOK;

   fm_port_ptr->riva_lpass_drift_us  = 0;
   fm_port_ptr->acc_avt_drift_us     = 0;
   fm_port_ptr->is_first_riva_lpass_sync = TRUE;

   qurt_elite_mutex_lock(&aud_stat_afe_riva_ptr->aud_sync_mutex);

   // Open AV timer driver
   avtimer_open_param_t open_param;
   open_param.client_name = (char *)"AfeSvc";
   open_param.flag = 0;
   fm_port_ptr->avt_drv = NULL;
   if (ADSP_EOK != (result = avtimer_drv_hw_open((avtimer_drv_handle_t *)&(fm_port_ptr->avt_drv), &open_param)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to Open AVTimer driver for Int-FM use case, error: %d", result);
      goto bail_out;
   }
   avtimer_drv_ioctl_prop_info_t prop_info;
   prop_info.prop = AVTIMER_DRV_PROPERTY_ENABLE_AVSYNC_FM;
   if (ADSP_EOK != (result = avtimer_drv_ioctl(fm_port_ptr->avt_drv, &prop_info)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to enable Int-FM clks, error: %d", result);
      goto bail_out;
   }

   hal_aud_sync_reset_tracking(HAL_AUD_SYNC_INTF_FM);
   hal_aud_sync_unreset_tracking(HAL_AUD_SYNC_INTF_FM);

   /* enable BT clocks as well because of the hardware bug:
   The bt_xo_clk is connected to both the BT and FM Phase Trackers. This is incorrect.
   The bt_xo_clk should be connected only to the BT Phase Tracker.
   The FM Phase tracker should be connected to its own clock, fm_xo_clk.*/
   //load ref inc count: depends on sampling rate * block transfer len b/w RIVA <-> LPASS
   hal_aud_sync_set_ref_cnt_inc(HAL_AUD_SYNC_INTF_FM, samples_per_riva_int);
   //set XO DIV val: depends on sampling rate and clock
   hal_aud_sync_set_xo_div_val(HAL_AUD_SYNC_INTF_FM, sample_rate);
   //enable XO DIV & tracking
   hal_aud_sync_enable_tracking(HAL_AUD_SYNC_INTF_FM);
   aud_stat_afe_riva_ptr->is_fm_aud_sync_enable = TRUE;

bail_out:
   if (ADSP_EOK != result)
   {
      if (NULL != fm_port_ptr->avt_drv)
      {
         avtimer_drv_hw_close((avtimer_drv_handle_t)fm_port_ptr->avt_drv);
         fm_port_ptr->avt_drv = NULL;
      }
   }
   qurt_elite_mutex_unlock(&aud_stat_afe_riva_ptr->aud_sync_mutex);

   return result;
}

void afe_riva_stop_fm_aud_sync(afe_dev_riva_port_t *fm_port_ptr)
{
   fm_port_ptr->riva_lpass_drift_us  = 0;
   fm_port_ptr->is_first_riva_lpass_sync = TRUE;

   qurt_elite_mutex_lock(&aud_stat_afe_riva_ptr->aud_sync_mutex);

   //disable XO DIV and tracking
   hal_aud_sync_disable_tracking(HAL_AUD_SYNC_INTF_FM);
   //reset the ref cnt
   hal_aud_sync_set_ref_cnt_inc(HAL_AUD_SYNC_INTF_FM, 0);
   //reset bit in ctl
   hal_aud_sync_reset_tracking(HAL_AUD_SYNC_INTF_FM);
   //disable clocks

   // Close the AVT driver
   avtimer_drv_ioctl_prop_info_t prop_info;
   if (NULL != fm_port_ptr->avt_drv)
   {
      prop_info.prop = AVTIMER_DRV_PROPERTY_DISABLE_AVSYNC_FM;
      avtimer_drv_ioctl(fm_port_ptr->avt_drv, &prop_info);
      avtimer_drv_hw_close((avtimer_drv_handle_t)fm_port_ptr->avt_drv);
      fm_port_ptr->avt_drv = NULL;
   }

   aud_stat_afe_riva_ptr->is_fm_aud_sync_enable = FALSE;
   qurt_elite_mutex_unlock(&aud_stat_afe_riva_ptr->aud_sync_mutex);
}

/**
 * -gets the val of counters: sample counter, reference counter
 * -handles overflow
 * -counts the diff between the 2.
 * -stores it as drift for av timer correction
 * -applies the drift on the hardware so that sample counter is matched to ref counter.
 * -special cases: a. what if |drift| > 1? (h/w supports correction by 1) -> BT out of range? -> slowly correct it
 *
 * this function is called for both BT RX and FM. Fot bt tx next function is called.
 */
void afe_riva_aud_sync(uint16_t port_id, hal_aud_sync_intf_t intf, \
      int64_t *riva_lpass_drift_us, uint32_t us_per_sample, uint8_t *is_first_riva_lpass_sync, int64_t *correction_us)
{
   uint32_t sample_counter = 0, reference_counter = 0, i, mask1=0, field_val1=0 ,mask2=0, field_val2=0;

   int16_t correction_sample_drift = 0, acc_corr_sample_drift = 0;

   int64_t mes_sample_drift = 0;

   int16_t riva_lpass_drift = 0;

#ifdef IPC_DRIFT_DEBUG
   uint8_t is_first_riva_lpass_sync_temp = *is_first_riva_lpass_sync; //same for logging
#endif

   hal_aud_sync_get_smp_cnt(intf, &sample_counter);

   hal_aud_sync_get_ref_cnt(intf, &reference_counter);

   if (*is_first_riva_lpass_sync)
   {
      *is_first_riva_lpass_sync = FALSE;

      //load ref cnt to smp count
      hal_aud_sync_set_smp_cnt(intf, reference_counter);

      *riva_lpass_drift_us = 0;
      *correction_us       = 0;
   }
   else
   {
      mes_sample_drift = reference_counter - sample_counter;

      //handling overflow : if the abs diff is greater than 2^31 then add or subtract by 2^32 and bring it back to range.
      if (mes_sample_drift < 0 && -mes_sample_drift > (int64_t)(1<<31))
      {
         mes_sample_drift +=  ((uint64_t)1<<32) ;
      }
      else if (mes_sample_drift > (int64_t)(1<<31))
      {
         mes_sample_drift -=  ((uint64_t)1<<32);
      }

      //add to existing drift
      riva_lpass_drift = (int16_t) mes_sample_drift;

     qurt_elite_mutex_lock(&aud_stat_afe_riva_ptr->aud_sync_mutex);

      for (i = 0; (i < MAX_SMP_CORR_PER_RIVA_INT) && \
                     (riva_lpass_drift != 0); i++)
      {
         //we can correct by +1 or -1 only at a time
         if (riva_lpass_drift > 0)
         {
            correction_sample_drift = 1;
         }
         else if (riva_lpass_drift < 0)
         {
            correction_sample_drift = -1;
         }
         else
         {
            correction_sample_drift = 0;
         }
         riva_lpass_drift    -= correction_sample_drift; //subtract off the corrected part.
         acc_corr_sample_drift += correction_sample_drift; //acc for the adding to riva_lpass_drift_us
         hal_aud_sync_adj_phase(intf, (hal_aud_sync_phase_adj_t) correction_sample_drift, &mask1,&field_val1); //by repeatedly calling we can correct multiple sample drifts.
      }
      *correction_us       =  afe_convert_sample_to_time (us_per_sample, acc_corr_sample_drift);
      //note that since AV timer and BT/FM interrupts are not processed at the same time below is OK. This functions happens during BT/FM int.
      *riva_lpass_drift_us += *correction_us;
      hal_aud_sync_adj_phase(intf, (hal_aud_sync_phase_adj_t) 0, &mask2,&field_val2); //we should clear otherwise, corrections don't take effect nexttime if NEG & POS bits are both 1.

      qurt_elite_mutex_unlock(&aud_stat_afe_riva_ptr->aud_sync_mutex);
   }
#ifdef IPC_DRIFT_DEBUG
   volatile uint32_t *riva_cnt = (uint32_t*)(0x031A0028);
   MSG_7(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AUD_SYNC:IPC:port_id=%x, sample_counter=%u, reference_counter=%u,riva=%u, correction=%d,mask1=%x,field_val1=%x", \
            port_id, sample_counter, reference_counter,*(riva_cnt),acc_corr_sample_drift,mask1,field_val1);
#endif
}

ADSPResult afe_riva_map_port_id_to_aud_sync(uint16_t port_id, hal_aud_sync_intf_t *aud_sync_id)
{
   //use DPLL1 phase tracker for RIVA BT
   //BT SCO RX and TX both use the same counters.
   switch(port_id)
   {
      case AFE_PORT_ID_INTERNAL_BT_SCO_RX:
      case AFE_PORT_ID_INTERNAL_BT_SCO_TX:
      {
         *aud_sync_id = HAL_AUD_SYNC_INTF_BT_1;
          return ADSP_EOK;
      }
      case AFE_PORT_ID_INTERNAL_FM_RX:
      case AFE_PORT_ID_INTERNAL_FM_TX:
      {
         *aud_sync_id = HAL_AUD_SYNC_INTF_FM;
         return ADSP_EOK;
      }
      default:
      {
         *aud_sync_id = HAL_AUD_SYNC_INTF_INVALID;
         return ADSP_EFAILED;
      }
   }
}

ADSPResult afe_riva_init_aud_sync()
{
   qurt_elite_mutex_init(&aud_stat_afe_riva_ptr->bt_rx_tx_aud_sync_mutex);

   qurt_elite_mutex_init(&aud_stat_afe_riva_ptr->aud_sync_mutex);

   aud_stat_afe_riva_ptr->drift_correction_us = 0;

   aud_stat_afe_riva_ptr->is_aud_sync_mutexes_init = TRUE;

   return ADSP_EOK;
}

ADSPResult afe_riva_deinit_aud_sync()
{

  if(aud_stat_afe_riva_ptr->is_aud_sync_mutexes_init)
  {
   qurt_elite_mutex_destroy(&aud_stat_afe_riva_ptr->bt_rx_tx_aud_sync_mutex);

   qurt_elite_mutex_destroy(&aud_stat_afe_riva_ptr->aud_sync_mutex);

    aud_stat_afe_riva_ptr->is_aud_sync_mutexes_init = FALSE;
  }

   return ADSP_EOK;
}

/**
  @brief specified port is enabled for VFR drifts.

  @param[in] port_id port id of the riva port

  @return  ADSP_EOK on success, an error code on error
*/
ADSPResult afe_riva_add_to_vfr_sync(afe_dev_port_t *afe_port_ptr)
{
   afe_dev_riva_port_t *riva_port_ptr = riva_ports_ptr_arr[GET_RIVA_PORT_INDEX(afe_port_ptr->intf)];

   riva_port_ptr->is_vfr_enabled = TRUE;
   riva_port_ptr->is_first_vfr   = TRUE;

   afe_drv_add_to_vfr_sync(afe_port_ptr->drift_info, afe_port_ptr->sample_rate, afe_port_ptr->intf);

   if (TRUE == aud_stat_afe_riva_ptr->hw_timestamp_available)
   {
      aud_stat_afe_riva_ptr->bt_rx_timestamp = 0;
   }

   return ADSP_EOK;
}

/**
  @brief specified port is disabled for VFR drifts.

  @param[in] port_id port id of the riva port

  @return  ADSP_EOK on success, an error code on error
*/
ADSPResult afe_riva_remove_from_vfr_sync(afe_dev_port_t *afe_port_ptr)
{
   afe_dev_riva_port_t *riva_port_ptr = riva_ports_ptr_arr[GET_RIVA_PORT_INDEX(afe_port_ptr->intf)];

   riva_port_ptr->is_vfr_enabled = FALSE;
   riva_port_ptr->is_first_vfr   = FALSE;

   afe_drv_remove_from_vfr_sync(afe_port_ptr->drift_info);

   return ADSP_EOK;

}

void afe_riva_update_btfm_acc_vfr_sample_drift(afe_drift_info_t *p_drift_info,
                                               uint32_t sample_rate,
                                               int32_t *acc_drift_in_us,
                                               uint16_t vfr_src_idx,
                                               int32_t port_id,
                                               int32_t *num_vfr_sample)
{
   int32_t                 curr_avt_drift_in_us, prev_avt_drift_in_us, inst_drift_in_us;
   uint64_t                curr_avt_drift_info;
   int32_t                 acc_drift_in_sample;
   int16_t                 inst_drift_in_sample;
   uint32_t                frame_counter_diff;

   *num_vfr_sample = 0;

   /* Read AV timer drift info for this port */ 
   curr_avt_drift_info = p_drift_info->avt_drift_info;

   frame_counter_diff = ( (uint32_t)(curr_avt_drift_info) - (uint32_t)(p_drift_info->prev_avt_drift_info) );

   inst_drift_in_us = 0;

   if ( (frame_counter_diff < AFE_AV_TIMER_DRIFT_FRM_CNTR_LOW_THRESH) ||
        (frame_counter_diff > AFE_AV_TIMER_DRIFT_FRM_CNTR_HIGH_THRESH) )
   {
      p_drift_info->prev_avt_drift_info = curr_avt_drift_info;
   }
   else /* Valid frame counter diff to read the AVT drift */
   {
      /* Read the AV timer drift for this port */
      curr_avt_drift_in_us = (int32_t)(curr_avt_drift_info >> AFE_ACC_AVT_DRIFT_US_SHIFT);

      prev_avt_drift_in_us = (int32_t)(p_drift_info->prev_avt_drift_info >> AFE_ACC_AVT_DRIFT_US_SHIFT);

      /* Calculate instantaneous drift in us */
      inst_drift_in_us = (curr_avt_drift_in_us - prev_avt_drift_in_us);

      /* Update the previous drift info */
      p_drift_info->prev_avt_drift_info = curr_avt_drift_info;
   }

   /* Derive the current AV timer time stamp for the device */
   p_drift_info->curr_vfr_intr_dev_av_timestamp += (VFR_FRAME_SIZE_IN_US + inst_drift_in_us);
              
   /* Calulcate accumulated drift between VFR and device */
   *acc_drift_in_us = (p_drift_info->curr_vfr_intr_dev_av_timestamp - p_drift_info->dev_latched_counter_curr);

   /* Convert drift in time to samples */
   acc_drift_in_sample = divide_int32( ( (*acc_drift_in_us) * (sample_rate/NUM_MILLISEC_PER_SEC) ), NUM_MILLISEC_PER_SEC );

   /* Calculate instantenous drift */
   inst_drift_in_sample = (acc_drift_in_sample - p_drift_info->vfr_acc_sample_drift_to_update);

#ifdef VFR_BTFM_DEBUG_MSG
   MSG_6(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "port-id: 0x%x, vfr_src_idx: %u, VFR_AV_TS: %ld, BTFM_AV_TS: %ld, BTFM_AVT_INST_DRIFT: %ld, acc_drift_in_us: %ld",
         port_id, vfr_src_idx, (uint32_t)p_drift_info->dev_latched_counter_curr,\
         (uint32_t)p_drift_info->curr_vfr_intr_dev_av_timestamp, inst_drift_in_us, *acc_drift_in_us);
#endif

#ifdef VFR_DEBUG_MSG
   *num_vfr_sample = p_drift_info->samples_in_vfr_frame;
#endif      

   if ( (inst_drift_in_sample <= VFR_SW_LATCH_DRIFT_NEG_THRESH) ||
        (inst_drift_in_sample >= VFR_SW_LATCH_DRIFT_POS_THRESH))
   {
      /* If drift is in +tive direction */
      if (inst_drift_in_sample > 0)
      {
         /* Increment sample drift by 1 sample */
         p_drift_info->vfr_acc_sample_drift_to_update += 1;
            
#ifdef VFR_DEBUG_MSG
         *num_vfr_sample = p_drift_info->samples_in_vfr_frame - 1;
#endif   
      }
      else if(inst_drift_in_sample < 0)/* If drift is in -tive direction */
      {
         /* Increment sample drift by 1 sample */
         p_drift_info->vfr_acc_sample_drift_to_update -= 1;

#ifdef VFR_DEBUG_MSG
         *num_vfr_sample = p_drift_info->samples_in_vfr_frame + 1; 
#endif      
      }
   } /* If inst drift above threholds */

   return;
}

/**
  @brief get VFR drifts

  @param[in] port_id port id of the riva port
  @param[out] drift ptr to the value of the drift
  @param[out] resync_flag flag to indicate resync

  @return  ADSP_EOK on success, an error code on error

  AUD_SYNC note: since RX and TX BT ports have same hardware for aud_sync, drifts should essentially be same.
  during BT mute, drifts will be zero (is_vfr_enabled is false).
*/
void afe_riva_get_vfr_drift(afe_dev_port_t *afe_port_ptr, vfr_src_state_t *vfr_state_ptr)
{
   uint32_t                vfr_timestamp = 0;
   hal_aud_sync_intf_t     intf = HAL_AUD_SYNC_INTF_INVALID;
   afe_dev_riva_port_t     *riva_port_ptr = riva_ports_ptr_arr[GET_RIVA_PORT_INDEX(afe_port_ptr->intf)];
   int32_t                 num_vfr_sample = 0, acc_drift_in_us = 0;
   uint16_t                vfr_src_idx;
   afe_drift_info_t        *p_drift_info;
   uint32_t                sample_rate, frame_counter;

   vfr_src_idx = vfr_state_ptr->vfr_src_index;

   p_drift_info = &afe_port_ptr->drift_info[vfr_src_idx];

   if (NULL != riva_port_ptr)
   {
      // only when riva driver vfr is enabled, then only aud sync will be enabled
      if (TRUE == riva_port_ptr->is_vfr_enabled)
      {
         afe_riva_map_port_id_to_aud_sync(afe_port_ptr->intf, &intf);
         
         hal_aud_sync_get_vfr_cnt(intf, &vfr_timestamp);
      }

      sample_rate = afe_port_ptr->sample_rate;

      /* If VFR source has been disabled and re-enabled, while the port is still running
         resync the port's drift info structure. */
      
      if (TRUE == vfr_state_ptr->is_first_vfr)
      {
         afe_drv_resync_port_vfr_drift_info(p_drift_info);
      }

      if (TRUE == p_drift_info->is_first_vfr)
      {
         p_drift_info->is_first_vfr = FALSE;

         p_drift_info->curr_vfr_intr_dev_av_timestamp = p_drift_info->dev_latched_counter_curr;
      }
      else if (TRUE == riva_port_ptr->is_first_vfr)
      {
         riva_port_ptr->is_first_vfr = FALSE;

         p_drift_info->curr_vfr_intr_dev_av_timestamp = p_drift_info->dev_latched_counter_curr;
      }
      else if (TRUE == riva_port_ptr->is_vfr_enabled)
      {
         /* Accumulate drift in samples */
         if (TRUE == vfr_state_ptr->is_hw_latched)
         {
             /* Number of samples from last VFR to current. */
            num_vfr_sample = vfr_timestamp - riva_port_ptr->last_vfr_timestamp;

            /* Accumulate drift in sample */
            (p_drift_info->vfr_acc_sample_drift_to_update) += (p_drift_info->samples_in_vfr_frame - num_vfr_sample);
         }
         else /* sw latched VFR */
         {
            afe_riva_update_btfm_acc_vfr_sample_drift(p_drift_info, sample_rate, 
                                                      &acc_drift_in_us, vfr_src_idx,
                                                      afe_port_ptr->intf,
                                                      &num_vfr_sample);
         }

         /* Convert the accumulated drift in samples to time */
         acc_drift_in_us = divide_int32((int32_t)(p_drift_info->vfr_acc_sample_drift_to_update * NUM_MILLISEC_PER_SEC), sample_rate/NUM_MILLISEC_PER_SEC);

#ifdef VFR_DEBUG_MSG
         MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "port-id: 0x%x, vfr_src_idx: %u, acc_drift_in_us: %ld, acc_drift_in_samples: %ld, num_vfr_sample: %ld",
               afe_port_ptr->intf, vfr_src_idx, acc_drift_in_us, p_drift_info->vfr_acc_sample_drift_to_update, num_vfr_sample);
#endif
      }
      riva_port_ptr->last_vfr_timestamp = vfr_timestamp;
   }

   /* Update vfr frame ctr and acc drift info */
   qurt_elite_mutex_lock( &(afe_port_ptr->port_mutex) );

   if (AFE_PORT_STATE_RUN == afe_port_ptr->port_state)
   {
       frame_counter = (uint32_t) (p_drift_info->vfr_drift_info) + 1;
       p_drift_info->vfr_drift_info = (uint64_t)(((int64_t)acc_drift_in_us << 32) | ((uint64_t)frame_counter));
   }

   qurt_elite_mutex_unlock( &(afe_port_ptr->port_mutex) );

   return;
}


/**
 * based lpass-riva drift, avt interrupt has to be adjusted.
 * factors related to amount of correction per interrupt
 * 1. too high a value can reduce the amount of time available for afe processing.
 * 2. too low a value can cause inconsistency: VFR time stamp shows immediate correction, but right amount of samples
 * would have reached (or pulled from) voice.
 * 3. AFE circular buffer size: if corrections are slow then circular buffer will underrun or overrun
 * (since riva rate should be matched to av timer rate, if we are slow, then constant riva rate can cause this).
 * 4. high block transfer time higher the correction since interrupts occur less often.
 * 5. since hardware would have taken considerable time to accumulate 1 sample drift, software seems to have to do the same.
 * But it's not so because (a) we don't know how long h/w took (b) sample counter correction on hw is immediate.
 *
 * @param[out] av timer adjustment in us: amount to be subtracted from 1ms avt tick
 * avt_adj -> +ve =>RIVA is fast
 *
 */
void afe_riva_calc_avt_adj(uint16_t port_id, int64_t *riva_lpass_drift_us, uint32_t block_transfer_time, int32_t *avt_adj)
{
   *avt_adj = 0;
   //int64_t temp=0;

   //no corrections applied if riva_lpass_drift_us == 0

   //+ve => RIVA is fast. so AVT period should shorten to match RIVA rate.
   if (*riva_lpass_drift_us > 0)
   {
      if (*riva_lpass_drift_us >= MAX_US_CORR_PER_MS_IN_AVT_INT)
      {
         //should be +ve
         *avt_adj  =  MAX_US_CORR_PER_MS_IN_AVT_INT*block_transfer_time;
      }
      else
      {
         //should be +ve
         *avt_adj  = (int32_t)*riva_lpass_drift_us;
      }
   }
   //-ve => RIVA is slow. so AVT period should inc to match RIVA rate.
   else if (*riva_lpass_drift_us < 0)
   {
      if (*riva_lpass_drift_us <= -MAX_US_CORR_PER_MS_IN_AVT_INT)
      {
         //should be -ve
         *avt_adj  =  -MAX_US_CORR_PER_MS_IN_AVT_INT*block_transfer_time;
      }
      else
      {
         //should be -ve
         *avt_adj  = (int32_t)*riva_lpass_drift_us;
      }
   }
   *riva_lpass_drift_us -= *avt_adj;    //remainder is taken care

#ifdef AVT_DRIFT_DEBUG
   MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AUD_SYNC:AVT:port_id=%x, avt_adj=%d, riva_lpass_drift_us=%d", port_id, (int32_t)*avt_adj, (int32_t)(*riva_lpass_drift_us));
#endif

   //return avt_adj;
}

/**
 * gets accumulated drift for riva port
 */
void afe_update_avt_drift_for_riva_port(afe_dev_port_t *dev_port_ptr)
{
   afe_dev_riva_port_t  *riva_port_ptr = riva_ports_ptr_arr[GET_RIVA_PORT_INDEX(dev_port_ptr->intf)];
   //make frame counter zero in the first avt interrupt.
   //-ve sign for the drift is because:in riva: +ve drift => riva is faster, but in afe: +ve means av timer is faster.
   afe_drv_update_avt_drift_info_element(&(dev_port_ptr->drift_info[AVT_DRIFT_INFO_IDX].avt_drift_info), -riva_port_ptr->acc_avt_drift_us, dev_port_ptr->is_first_dma);

}


void afe_riva_get_av_timestamp(afe_dev_port_t *afe_port_ptr, uint64_t *riva_av_timestamp)
{
   *riva_av_timestamp = afe_time_srvr_timer_get_time((avtimer_drv_handle_t)afe_port_ptr->avt_drv);

   return;
}
