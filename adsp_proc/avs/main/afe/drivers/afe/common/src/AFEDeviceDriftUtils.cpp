/*==============================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/common/src/AFEDeviceDriftUtils.cpp#1 $
$DateTime: 2017/11/07 23:00:25 $
$Author: pwbldsvc $
$Change: 14795016 $
$Revision: #1 $
 
FILE:     AFEDeviceDriftUtils.cpp

DESCRIPTION: AFE Device Utilites for VFR/AVT drift calculations 

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright (c) 2011-2016 QUALCOMM Technologies, Inc.  All Rights Reserved.
QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
==============================================================================*/
/*============================================================================
EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order. Please
use ISO format for dates.

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/common/src/AFEDeviceDriftUtils.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
11/06/07    RA      Created file

============================================================================*/

/*=====================================================================
 Includes
 ======================================================================*/
#include "AFEDeviceDriver.h"
#include "AFEDeviceDriver_i.h"
//#include "VfrDriverInternal.h"
#include "AfeTimeStampDriver.h"
#include "AFESpdifDriver.h"
#include "AFESlimbusDriver.h"
#include "AFEHdmiOutputDrv.h"
#include "AFEUSBAudioDriver.h"

/*=====================================================================
 Globals
 ======================================================================*/

/*=====================================================================
 Functions
 ======================================================================*/
 
uint64_t afe_device_read_timestamp_register(uint32_t mux_id)
{
   uint64_t   timestamp;

   afe_ts_drv_get_timestamp(mux_id, &timestamp);

   return timestamp;
}

ADSPResult afe_device_update_avt_drift(afe_dev_port_t *dev_port_ptr)
{
   int32_t                 acc_avt_drift_us;
   int32_t                 fifo_diff;
   int64_t                 diff_dev_av_ts = 0, diff_dev_ts = 0, fifo_duration = 0;
   afe_dev_avt_drift_t     *avt_drift_ptr = dev_port_ptr->avt_drift_params.avt_drift_struct_ptr;
   afe_avt_drift_params_t  *avt_drift_params_ptr = (afe_avt_drift_params_t *)&dev_port_ptr->avt_drift_params;

   if (dev_port_ptr->is_first_dma)
   {
      avt_drift_ptr->init_avtimer_timestamp  = dev_port_ptr->port_av_timestamp;
      avt_drift_ptr->init_dev_time = avt_drift_params_ptr->sw_latched_dev_time;
      avt_drift_ptr->init_fifo_samples = avt_drift_params_ptr->fifo_samples;

      /* If first DMA reset the AVT vs device drift */
      afe_drv_update_avt_drift_info_element(&(dev_port_ptr->drift_info[AVT_DRIFT_INFO_IDX].avt_drift_info), 0, TRUE);
   }
   else /* For subsequent DMA interrupts */
   {
      /* Calculate difference between current and init device AV timer time stamp */
      diff_dev_av_ts = dev_port_ptr->port_av_timestamp - avt_drift_ptr->init_avtimer_timestamp;

      if(TIMESTAMP_LATCH_TYPE_SOFTWARE == avt_drift_params_ptr->avtimer_latch_type)
      {
         /* For SW latching, device time elapsed is difference in the last SW latched progress counter
            to the initial SW latched progress counter on first DMA interrupt */
         diff_dev_ts = avt_drift_params_ptr->sw_latched_dev_time - avt_drift_ptr->init_dev_time;
      }
      else /* H/w latched AVT time stamp */
      {
         /* For HW latching, device time is just the total time in terms of DMA samples transferred */
    	  diff_dev_ts = ((int64_t)(dev_port_ptr->running_int_counter * (int64_t)(dev_port_ptr->int_samples_per_period * NUM_US_PER_SEC))/dev_port_ptr->sample_rate);

         fifo_diff = avt_drift_params_ptr->fifo_samples - avt_drift_ptr->init_fifo_samples;

         /* Convert FIFO sample difference to time in us */
         fifo_duration = ((int64_t)fifo_diff * NUM_US_PER_SEC) /(dev_port_ptr->channels * dev_port_ptr->sample_rate);

         /* Adjust the device time as per latched FIFO samples */
         if(IS_IT_TX_PORT_AFE_ID(dev_port_ptr->intf))
         {
            /* delta = (curr_dev_time - init_dev_time) = (curr_per_cnt + curr_fifo_count) - (init_per_cnt + init_fifo_count)
                     = (curr_per_cnt - init_per_cnt) + (curr_fifo_count - init_fifo_count) */
            diff_dev_ts = diff_dev_ts + fifo_duration;
         }
         else /* Rx port */
         {
            /* delta = (curr_dev_time - init_dev_time) = (curr_per_cnt - curr_fifo_count) - (init_per_cnt - init_fifo_count)
        	            = (per_cnt - init_per_cnt) - (curr_fifo_count - init_fifo_count) */
            diff_dev_ts = diff_dev_ts - fifo_duration;
         }
      }

      acc_avt_drift_us =  (int32_t) (diff_dev_av_ts - diff_dev_ts) ; //+ve -> avtimer faster -> since avtimer counted more.

#ifdef AVT_BASED_AFE_DRIFT_DEBUG
      MSG_9(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
    		  "Port ID:0x%x, FIFO samples = %lu, acc drift = %d, diff_dev_av_ts = %lu, diff_dev_ts = %lu port_timestamp = %lu, init_timestamp = %lu, interrtup counter = %lu, int_samples_per_frame= %lu",
    		  (uint32_t)dev_port_ptr->intf, avt_drift_params_ptr->fifo_samples, (int)acc_avt_drift_us, (uint32_t)diff_dev_av_ts, (uint32_t)diff_dev_ts,
    		  (uint32_t)dev_port_ptr->port_av_timestamp, (uint32_t)avt_drift_ptr->init_avtimer_timestamp, (uint32_t)dev_port_ptr->running_int_counter, (uint32_t)dev_port_ptr->int_samples_per_period);
#endif

      /* note: above is same as: acc_avt_drift_us +=  (curr_dev_time - prev_dev_time) - (curr_avt_timestamp - prev_avt_timestamp)*/
      afe_drv_update_avt_drift_info_element(&(dev_port_ptr->drift_info[AVT_DRIFT_INFO_IDX].avt_drift_info), acc_avt_drift_us, FALSE);

#ifdef AVT_BASED_AFE_DRIFT_DEBUG
      avt_drift_ptr->avtimer_timestamp = dev_port_ptr->port_av_timestamp;
      if(TIMESTAMP_LATCH_TYPE_SOFTWARE == avt_drift_params_ptr->avtimer_latch_type)
      {
         avt_drift_ptr->dev_time = avt_drift_params_ptr->sw_latched_dev_time;
      }
      else
      {
         avt_drift_ptr->dev_time =  ((int64_t)(dev_port_ptr->running_int_counter * (int64_t)(dev_port_ptr->int_samples_per_period * NUM_US_PER_SEC))/dev_port_ptr->sample_rate);
         if(IS_IT_TX_PORT_AFE_ID(dev_port_ptr->intf))
         {
            avt_drift_ptr->dev_time += fifo_duration;
         }
         else
         {
            avt_drift_ptr->dev_time -= fifo_duration;
         }
      }
#endif // AVT_BASED_AFE_DRIFT_DEBUG
   }

   return ADSP_EOK;
}

void afe_device_update_port_vfr_drift_info(void *dev_port_ptr, uint64_t dev_timestamp)
{
   afe_dev_port_t *afe_port_ptr = (afe_dev_port_t *)dev_port_ptr;

   /* Update the VFR related params for each VFR source enabled for this port.
      Need to atomically increment the av-timer timestamp and the running interrupt counter as this
      will be read from VFR thread when calculating VFR drift */

   for(int32_t i=0; i < NUM_MAX_VFR_SRC; i++)
   {
      afe_drift_info_t *drift_info_ptr = &afe_port_ptr->drift_info[i];

      if(TRUE == drift_info_ptr->port_vfr_mutex_init)
      {
         qurt_elite_mutex_lock(&drift_info_ptr->port_vfr_mutex);

        if(drift_info_ptr->vfr_enabled)
        {
         if(0 != dev_timestamp)
         {
            /* This condition is required for BT where TX and RX share the same interrupts for TS
               Counter for TX should not be incremented as long as RX port is not started */
            drift_info_ptr->running_dev_int_counter++;
            drift_info_ptr->latest_dev_av_timestamp = dev_timestamp;
            drift_info_ptr->latest_fifo_samples = afe_port_ptr->avt_drift_params.fifo_samples;
         }
        }

         qurt_elite_mutex_unlock(&drift_info_ptr->port_vfr_mutex);
      }
   }
}

void afe_device_update_acc_sample_drift(afe_drift_info_t *p_drift_info, uint32_t num_vfr_sample)
{
   int16_t inst_drift_in_sample;

   /* Instantaneous drift in samples */
   inst_drift_in_sample = (p_drift_info->samples_in_vfr_frame - num_vfr_sample);

   /* Accumulate the instantaneous drift for thresholding */
   p_drift_info->inst_sample_drift += inst_drift_in_sample;

   /* If instantaneous drift is above +/- threshold, increment the acc drift value */
   if ( (p_drift_info->inst_sample_drift <= VFR_SW_LATCH_DRIFT_NEG_THRESH) ||
        (p_drift_info->inst_sample_drift >= VFR_SW_LATCH_DRIFT_POS_THRESH))
   {
      /* If drift is in +tive direction */
      if (p_drift_info->inst_sample_drift > 0)
      {
         /* Increment sample drift by 1 sample */
         p_drift_info->vfr_acc_sample_drift_to_update += 1;

         /* Decrement the drift acounted for */
         p_drift_info->inst_sample_drift -= 1;
      }
      else if(p_drift_info->inst_sample_drift < 0)/* If drift is in -tive direction */
      {
         /* Increment sample drift by 1 sample */
         p_drift_info->vfr_acc_sample_drift_to_update -= 1;

         /* Decrement the drift acounted for */
         p_drift_info->inst_sample_drift += 1;
      }
   }

   return;
}

/* Update the port AV Time stamp with either s/w or h/w latched time stamp as available */
void afe_port_update_timestamp(afe_dev_port_t *dev_port_ptr)
{
	if ((IS_RIVA_PORT_AFE_ID(dev_port_ptr->intf))||(IS_RT_PROXY_PORT_AFE_ID(dev_port_ptr->intf)))
	{
		afe_drv_get_time_for_port(dev_port_ptr->intf, &(dev_port_ptr->port_av_timestamp));
	}
	else if (IS_AUDIO_IF_PORT_AFE_ID(dev_port_ptr->intf) || (IS_AUDIO_IF_TDM_PORT_AFE_ID(dev_port_ptr->intf) ))
	{
            uint32_t prop_id = AFE_DEVICE_TIMESTAMP_AND_FIFO_PROP;
            uint32_t prop_size = sizeof(afe_dev_timestamp_info_t);
            afe_dev_timestamp_info_t dev_timestamp_info;
            memset(&dev_timestamp_info, 0, sizeof(afe_dev_timestamp_info_t));
            if((NULL != dev_port_ptr->afe_drv_ftbl) && (NULL != dev_port_ptr->afe_drv_ftbl->get_properties))
            {
               dev_port_ptr->afe_drv_ftbl->get_properties(dev_port_ptr->afe_drv, prop_id, (int8_t *) &dev_timestamp_info, prop_size);
               dev_port_ptr->port_av_timestamp =    dev_timestamp_info.dma_int_timestamp;
               dev_port_ptr->avt_drift_params.fifo_samples = dev_timestamp_info.fifo_samples;
            }
            else
            {
               MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "driver function table is null: dir: %d, port id: 0x%x", dev_port_ptr->dir, dev_port_ptr->intf);
            }
	}
	else if (IS_SLIMBUS_PORT_AFE_ID(dev_port_ptr->intf))
	{
		afe_sb_get_timestamp_and_fifo_count(dev_port_ptr);
	}
	else if(AFE_PORT_ID_SPDIF_RX == dev_port_ptr->intf)
	{
		dev_port_ptr->port_av_timestamp = afe_spdif_get_tsdiff_timestamp(dev_port_ptr);
	}
	else if(IS_HDMI_OUTPUT_PORT_AFE_ID(dev_port_ptr->intf) || IS_HDMI_OVER_DP_PORT_AFE_ID(dev_port_ptr->intf))
	{
		afe_hdmi_output_get_dma_timestamp_and_fifo_count(dev_port_ptr);
	}
	else if(IS_USBA_PORT_AFE_ID(dev_port_ptr->intf))
	{
	  afe_usba_get_timestamp(dev_port_ptr);
	}
}

