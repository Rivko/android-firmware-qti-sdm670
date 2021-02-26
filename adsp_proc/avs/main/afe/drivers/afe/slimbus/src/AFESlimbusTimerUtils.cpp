/*==============================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/slimbus/src/AFESlimbusTimerUtils.cpp#1 $
$DateTime: 2017/11/07 23:00:25 $
$Author: pwbldsvc $
$Change: 14795016 $
$Revision: #1 $

FILE:     AFESlimbusTimerUtils.cpp

DESCRIPTION: AFE Slimbus driver utility functions

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

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/slimbus/src/AFESlimbusTimerUtils.cpp#1 $ $DateTime: 2017/11/07 23:00:25 $ $Author: pwbldsvc $

when        who  what, where, why
----------  ---  ----------------------------------------------------------
08-08-2010  mspk  Initial Draft


============================================================================*/
#include "AFESlimbusTimerUtils_i.h"
#include "audio_basic_op.h"
#include "AFEDeviceDriver.h"

#define AFE_NUM_SAMPLING_RATES 16  
static sample_rate_info_t sample_rate_info[AFE_NUM_SAMPLING_RATES] = {
                            {8000, 4194304000U,8, 1},
                            {11025,3043485895U,11, 2},
                            {12000,2796202667U,12, 2},
                            {16000,2097152000U,16, 2},
                            {22050,1521742948U,22, 3},
                            {24000,1398101333U,24, 3},
                            {32000,1048576000U,32, 4},
                            {44100,760871474U,44, 5},
                            {48000,699050667U,48, 6},
                            {88200,380435737U,88, 12}, //for HDMI only
                            {96000,349525333U,96, 12}, //for HDMI only
                            {144000,233016889U,144, 18}, //for HDMI only
                            {176400,190217868U,176, 23}, //for HDMI only
                            {192000,174762667U,192, 24}, //for HDMI only
                            {352800,951089342U,352, 24}, 
                            {384000,873813333U,384, 24}, 
                          };

/*=====================================================================
 Functions
 ===================================================================== */

/*=====================================================================
 Functions
 ======================================================================*/
ADSPResult afe_sb_alloc_progress_counter(afe_slimbus_state_struct_t *p_dev_state)
{
    ADSPResult result = ADSP_EOK;
    DalDeviceHandle *p_core_driver=p_dev_state->p_sb_driver->p_core_driver;
    SlimBusResourceHandle h_progress_ctr;
    SlimBusResourceHandle h_master_port =
        p_dev_state->a_master_port_info[p_dev_state->afe_sb_cfg.num_master_ports - 1].h_master_port;
    
    if (0 != p_dev_state->h_progress_ctr)
    {
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Progress Counter is already created");
        return ADSP_EALREADY;
    }

    result = DalSlimBus_AllocProgressCounter(p_core_driver, h_master_port, SLIMBUS_BAM_DEFAULT, &h_progress_ctr);
    if (DAL_SUCCESS != result)
    {
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to allocate the progress counter");
        return ADSP_EFAILED;
    }

    result = DalSlimBus_InitProgressCounter(p_core_driver, h_progress_ctr, 0);
    if (DAL_SUCCESS != result)
    {
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to initialize the progress counter");
        DalSlimBus_DeallocProgressCounter(p_core_driver, h_progress_ctr);
        return ADSP_EFAILED;
    }

    // cache it in dev state struct 
    p_dev_state->h_progress_ctr = h_progress_ctr;
    qurt_elite_mutex_init(&p_dev_state->prog_count_mutex);

    return ADSP_EOK;   
}

ADSPResult afe_sb_dealloc_progress_counter(afe_slimbus_state_struct_t *p_dev_state)
{
    ADSPResult result = ADSP_EOK;
    DalDeviceHandle *p_core_driver=p_dev_state->p_sb_driver->p_core_driver;

    if (0 == p_dev_state->h_progress_ctr)
    {
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Progress Counter is already destroyed");
        return ADSP_EALREADY;
    }

    result = DalSlimBus_DeallocProgressCounter(p_core_driver, p_dev_state->h_progress_ctr);
    if (DAL_SUCCESS != result)
    {
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to allocate the progress counter");
    }

    p_dev_state->h_progress_ctr = 0;

    qurt_elite_mutex_destroy(&p_dev_state->prog_count_mutex);

    return ADSP_EOK;   
}

ADSPResult afe_sb_read_progress_counter(afe_slimbus_state_struct_t *p_dev_state, uint64_t *samp_cnt, 
                                        SlimBusCounterType type, uint16_t vfr_src_idx)
{
    ADSPResult result = ADSP_EOK;
    DalDeviceHandle *p_core_driver = NULL;
    uint32 double_word_data = 0, fifo_samples = 0;
    int64_t diff;
    uint64_t acc_dw_count = 0;
    vfr_drift_detect_t     *p_vfr_info;

    if (0 == p_dev_state->h_progress_ctr)
    {
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Progress Counter is not allocated");
        return ADSP_EBADPARAM;
    }    

    p_core_driver = p_dev_state->p_sb_driver->p_core_driver;
    result = DalSlimBus_ReadProgressCounter(p_core_driver, p_dev_state->h_progress_ctr, type, &double_word_data, &fifo_samples);
    if (DAL_SUCCESS != result)
    {
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Progress Counter Read is failing");
        return ADSP_EBADPARAM;    
    }
    
    switch (type)
    {
       case SLIMBUS_COUNTER_FREE_RUN:
       {
          //use mutex, since call may be from many threads.
          qurt_elite_mutex_lock(&p_dev_state->prog_count_mutex);

          //handle 24 bit overflow of the progress counters
          diff =  (int64_t) (double_word_data - p_dev_state->prev_dw_prog_count);

          if (diff < 0)
          {
             diff += MAX_DMA_WORD_TRANSFERED;
          }
          p_dev_state->acc_dw_prog_counter     += (((uint64_t)diff) & MAX_DMA_WORD_TRANSFERED); //diff is always +ve
          p_dev_state->prev_dw_prog_count      =  double_word_data;
          acc_dw_count                         =  p_dev_state->acc_dw_prog_counter;

          qurt_elite_mutex_unlock(&p_dev_state->prog_count_mutex);
       }
       break;

       case SLIMBUS_COUNTER_VFR:
       {
          /* Handle 24 bit overflow of the progress counters */

          p_vfr_info = &p_dev_state->vfr[vfr_src_idx]; 

          diff =  (int64_t) (double_word_data - p_vfr_info->prev_dw_prog_count);

          if (diff < 0)
          {
             diff += MAX_DMA_WORD_TRANSFERED;
          }
          p_vfr_info->acc_dw_prog_counter     += (((uint64_t)diff) & MAX_DMA_WORD_TRANSFERED); //diff is always +ve
          p_vfr_info->prev_dw_prog_count      =  double_word_data;
          acc_dw_count                        =  p_vfr_info->acc_dw_prog_counter;
       }
       break;

       default:
       {
          acc_dw_count                         = 0;
       }
    }

    if (SLIMBUS_PORT_SINK_FLOW == p_dev_state->direction)
    {
        *samp_cnt = (acc_dw_count) * (p_dev_state->samples_per_dword) + fifo_samples; //assumed that acc_dw_prog_counter overflow never occurs.
    }
    else
    {
        *samp_cnt = (acc_dw_count) * (p_dev_state->samples_per_dword) - fifo_samples;
    }

#ifdef SLIMBUS_DBG_ENABLE
		MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "SLIMbus:%x, type=%d, progress counter samp count: 0x%x%x,double_word_data=%d", p_dev_state->afe_port_ptr->intf, (int)type, (unsigned int)((*samp_cnt)>>32), (unsigned int)(*samp_cnt), (unsigned int)double_word_data);
#endif //SLIMBUS_DBG_ENABLE

    return ADSP_EOK;
}

void afe_get_sample_rate(int16_t index, uint32_t *pRate)
{
   *pRate = sample_rate_info[index].sample_rate;
}

void afe_get_samples_per_ms(int16_t index, uint16_t *pSamples)
{
   *pSamples = sample_rate_info[index].samples_per_ms;
}

void afe_get_time_per_sample(int16_t index, uint32_t *pTime, uint16_t *pFormat)
{
   *pTime = sample_rate_info[index].time_per_sample;
   *pFormat = 25;
}

void afe_get_sample_rate_index(uint32_t samplingRate, int16_t *pIndex)
{
   int16_t i;
      
   *pIndex = -1;
   for(i = 0 ; i < AFE_NUM_SAMPLING_RATES; i++ )
   {
       if (sample_rate_info[i].sample_rate == samplingRate )
       {
           *pIndex = i;
           break;
       }
   }
}

/**
 * to read AV timer and SB counters atomically and convert to time
 */
void afe_read_avt_and_sb_counters_atomically(afe_dev_port_t *dev_port_ptr, uint64_t *sb_time, uint64_t *av_time)
{

   //use an assembly function with spinlock.
#if ( __QDSP6_ARCH__ >= 4 )
   afe_sb_read_avt_sb_time_asm(dev_port_ptr, sb_time , av_time);

#else //non-spinlock version as a fall-back option.

   /** below 2 instructions should execute without varying delay between them: another high prio thread shouldn't swap them. */
   *av_time  = qurt_elite_timer_get_time();
   afe_sb_get_time_for_avt_afe(dev_port_ptr, sb_time);

#endif
}

/**
 * unrolling the afe_sb_get_time function.
 * cannot print log messages or any others that use mutex, since this runs in spinlock.
 */
void afe_sb_get_time_for_avt_afe(afe_dev_port_t *afe_port_ptr, uint64_t *sb_time)
{
   ADSPResult result;
   afe_slimbus_state_struct_t *p_dev_state = NULL;
   uint32 double_word_data = 0, fifo_samples = 0, time_per_sample;
   uint64_t temp1, temp2, samp_cnt=0, acc_dw_count = 0;
   uint16_t q_format;
   int64_t diff;

   *sb_time = 0;

   if (NULL == (p_dev_state = (afe_slimbus_state_struct_t *)afe_port_ptr->afe_drv))
   {
      //MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Device State Struct ptr is not valid");
      return ;
   }
   if (0 == p_dev_state->h_progress_ctr)
   {
      //MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Progress Counter is not allocated");
      return ;
   }

   DalDeviceHandle *p_core_driver = p_dev_state->p_sb_driver->p_core_driver;
   result = DalSlimBus_ReadProgressCounter(p_core_driver, p_dev_state->h_progress_ctr,
            SLIMBUS_COUNTER_FREE_RUN, &double_word_data, &fifo_samples);
   if (DAL_SUCCESS != result)
   {
      //MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Progress Counter Read is failing");
      return ;
   }

   //handle 24 bit overflow of the progress counters
   diff =  (int64_t) (double_word_data - p_dev_state->avt_afe_prev_dw_prog_count);

   if (diff < 0)
   {
      diff += MAX_DMA_WORD_TRANSFERED;
   }
   p_dev_state->avt_afe_acc_dw_prog_counter     += (((uint64_t)diff) & MAX_DMA_WORD_TRANSFERED); //diff is always +ve, TODO: may need to remove MAX_DMA_WORD_TRANSFERED
   p_dev_state->avt_afe_prev_dw_prog_count      =  double_word_data;
   acc_dw_count                                 =  p_dev_state->avt_afe_acc_dw_prog_counter;

   if (SLIMBUS_PORT_SINK_FLOW == p_dev_state->direction)
   {
      samp_cnt = (acc_dw_count) * (p_dev_state->samples_per_dword) + fifo_samples;
   }
   else
   {
      samp_cnt = (acc_dw_count) * (p_dev_state->samples_per_dword) - fifo_samples;
   }

   afe_get_time_per_sample(p_dev_state->sample_rate_index, &time_per_sample, &q_format);

   temp2         = ((uint64_t)(0x01)<<(q_format-1));

   //assumption: 64 bit cur_pg_samp_cnt generally doesn't go beyond 32 bits.
   temp1         = (uint64_t) ((uint64_t) (samp_cnt) * (uint64_t)time_per_sample);

   /*//doing below, we loose precision.
   upper = (uint64_t)((cur_pg_samp_cnt>>32)&0xFFFFFFFF);
   upper = upper * (uint64_t)time_per_sample;

   lower = (uint64_t)((cur_pg_samp_cnt)&0xFFFFFFFF);
   lower = lower * (uint64_t)time_per_sample;

   temp1 = (upper) + (lower>>32);
   */
   *sb_time   = (temp1 + temp2) >> q_format;
}

ADSPResult afe_sb_get_timestamp_and_fifo_count(afe_dev_port_t *afe_port_ptr)
{
	ADSPResult                   result = ADSP_EOK;	
	afe_slimbus_state_struct_t *p_dev_state = NULL;
   DalDeviceHandle               *p_core_driver;
   uint32_t                      dma_count = 0, fifo_samples = 0;
   uint64_t                      timestamp = 0;
   DALBOOL                       sample_missed = FALSE;

	if (NULL == (p_dev_state = (afe_slimbus_state_struct_t *)afe_port_ptr->afe_drv))
	{
		return ADSP_EFAILED;
	}

	if (0 == p_dev_state->h_progress_ctr)
	{
	    return ADSP_EFAILED;
	}

	afe_avt_drift_params_t *avt_drift_params_ptr = (afe_avt_drift_params_t *)&afe_port_ptr->avt_drift_params;

   /* For s/w latched AVT, timestamp is latched in BAM isr context.
      Here  update only the h/w latched timestamp */
	if(TIMESTAMP_LATCH_TYPE_HARDWARE == avt_drift_params_ptr->avtimer_latch_type)
	{
      p_core_driver = p_dev_state->p_sb_driver->p_core_driver;

		if(DAL_SUCCESS != (result = DalSlimBus_ReadProgressCounterTimestamp(p_core_driver, p_dev_state->h_progress_ctr,
				&dma_count, &fifo_samples, &timestamp, &sample_missed)))
		{
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Device State Struct ptr is not valid, port_id: 0x%x", afe_port_ptr->intf);
			return ADSP_EFAILED;
		}

		afe_port_ptr->port_av_timestamp = avtimer_drv_convert_hw_tick_to_time(timestamp);

		avt_drift_params_ptr->fifo_samples = fifo_samples;
	}

	return ADSP_EOK;
}

