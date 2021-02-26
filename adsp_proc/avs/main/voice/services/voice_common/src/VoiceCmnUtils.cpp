/*========================================================================

*//** @file VoiceCmnUtils.cpp
This file abstracts out common utilities for voice.

Copyright (c) 2010-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/voice/services/voice_common/src/VoiceCmnUtils.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
05/26/10   pg    Initial version
03/28/11   AZ      Support virtual contiguous memory.

========================================================================== */


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "qurt_elite.h"
#include "Elite.h"
#include "EliteMsg_Util.h"
#include "VoiceCmnUtils.h"
#include "adsp_vsm_api.h"
#include "adsp_vcmn_api.h"
/*--------------------------------------------------------------*/
/* Macro definitions                                            */
/* -------------------------------------------------------------*/

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Function prototypes
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Message handler
** ----------------------------------------------------------------------- */

/* =======================================================================
**                          Function Definitions
** ======================================================================= */


void voice_cmn_time_profile_init(voice_cmn_time_profile_t *time_profile_ptr)
{

   time_profile_ptr->num_samples = 0;
   time_profile_ptr->max_time    = 0;
   time_profile_ptr->min_time    = 0xFFFFFFFF;
   time_profile_ptr->max_cycles  = 0;
   time_profile_ptr->min_cycles  = 0xFFFFFFFF;
   time_profile_ptr->total_cycles = 0;
   time_profile_ptr->total_time  = 0;
}


void voice_cmn_time_profile_add_data(voice_cmn_time_profile_t *time_profile_ptr, uint32_t time, uint32_t cycles)
{
   /* increment number of samples, compute new total time and cycles, and adjust max/min if necessary */
   time_profile_ptr->num_samples++;

   time_profile_ptr->total_time += time;

   if( time < time_profile_ptr->min_time)
   {
      time_profile_ptr->min_time = time;
   }

   if( time > time_profile_ptr->max_time)
   {
      time_profile_ptr->max_time = time;
   }

   time_profile_ptr->total_cycles += cycles;

   if( cycles < time_profile_ptr->min_cycles)
   {
      time_profile_ptr->min_cycles = cycles;
   }

   if( cycles > time_profile_ptr->max_cycles)
   {
      time_profile_ptr->max_cycles = cycles;
   }

}
void voice_calc_inst_drift(voice_afe_drift_info_t *voice_cmn_drift_info_ptr,void *afe_drift_ptr,
      uint8_t vfr_mode, voice_cmn_timer_clients_t client, uint16_t mode,
      uint32_t session, uint8_t timing_ver, uint32_t vsid, int32_t *inst_drift_us)
{
    voice_afe_drift_info_t *prev_info = voice_cmn_drift_info_ptr;
    voice_afe_drift_info_t curr_info = {{0}};
    int32_t frame_counter_diff = 0;

    if ((NULL == voice_cmn_drift_info_ptr)  || (NULL == afe_drift_ptr))
    {
       MSG_2(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: NULL ptrs to voice_cmn_accu_avt_drift client(%#x):session(%#lx)", \
             client, session);
       return;
    }
    if(VFR_NONE == mode)
    {
       afe_port_read_avt_drift((uint64_t*)afe_drift_ptr, &curr_info.avt_drift_info); // read afe memory
       frame_counter_diff = (int32_t)(curr_info.avt_drift_info.frame_counter - prev_info->avt_drift_info.frame_counter);
       *inst_drift_us = (int32_t)curr_info.avt_drift_info.acc_avt_drift_us - prev_info->avt_drift_info.acc_avt_drift_us;

    }

    else
    {
       uint32_t vfr_source = (uint32_t) vfr_mode;
       if (VFR_CLIENT_INFO_VER_2 == timing_ver)
       {
          vfr_source = vsid;
       }
       vfr_drv_read_vfr_drift((afe_drift_info*)afe_drift_ptr, &curr_info.vfr_drift_info, timing_ver, vfr_source);
       frame_counter_diff = (int32_t)(curr_info.vfr_drift_info.frame_counter - prev_info->vfr_drift_info.frame_counter);
       *inst_drift_us = (int32_t)curr_info.vfr_drift_info.acc_vfr_drift_us - prev_info->vfr_drift_info.acc_vfr_drift_us;
    }

    if ( (frame_counter_diff < 0) || (frame_counter_diff > AFE_AV_TIMER_DRIFT_FRM_CNTR_HIGH_THRESH) )
    {
       *inst_drift_us = 0;
        MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Voice Cmn utility: Device Reset/Resync detected: frame_counter_diff (%ld) client (%d), session(%#lx)",frame_counter_diff,(int)client,session);

    }
    *prev_info = curr_info;
}
void voice_cmn_accu_drift(int32_t *ss_info_counter_ptr, voice_cmn_drift_info_t *voice_cmn_drift_info_ptr, void *afe_drift_ptr, uint8_t vfr_mode, voice_cmn_timer_clients_t client, uint16_t mode, uint32_t session, uint8_t timing_ver, uint32_t vsid)
{
   voice_cmn_drift_info_t *prev_info = voice_cmn_drift_info_ptr;
   voice_cmn_drift_info_t curr_info = {0};
   int32_t frame_counter_diff = 0;
   int32_t inst_drift_us = 0;      // instantaneous drift in micro secs
   int64_t nb_fact_drift = 0;      // afe_drift_us*nb_sampling_factor

   if (     (NULL == ss_info_counter_ptr)
         || (NULL == voice_cmn_drift_info_ptr)
         || (NULL == afe_drift_ptr)
      )
   {
      MSG_2(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: NULL ptrs to voice_cmn_accu_avt_drift client(%#x):session(%#lx)", \
            client, session);
      return;
   }

   //internal voice_drift_counter is increamented in multiple of 125 microseconds for each sample drift detected irrespecive of sampling rate.
   //AFE drift in microseconds is multiplied by nb_sampling_factor.
   if(VFR_NONE == mode)
   {
      afe_port_read_avt_drift((uint64_t*)afe_drift_ptr, &curr_info.afe_drift_info.avt_drift_info); // read afe memory
      frame_counter_diff = (int32_t)(curr_info.afe_drift_info.avt_drift_info.frame_counter - prev_info->afe_drift_info.avt_drift_info.frame_counter);
      nb_fact_drift = curr_info.afe_drift_info.avt_drift_info.acc_avt_drift_us * voice_cmn_drift_info_ptr->nb_sampling_rate_factor;         // calculate nb_fact_drift drift.

   }
   else
   {
      //afe_port_read_vfr_drift((afe_drift_info*)afe_drift_ptr, &curr_info.afe_drift_info.vfr_drift_info, vfr_mode); // read afe memory
      uint32_t vfr_source = (uint32_t) vfr_mode;
      if (VFR_CLIENT_INFO_VER_2 == timing_ver)
      {
         vfr_source = vsid;
      }
      vfr_drv_read_vfr_drift((afe_drift_info*)afe_drift_ptr, &curr_info.afe_drift_info.vfr_drift_info, timing_ver, vfr_source);
      frame_counter_diff = (int32_t)(curr_info.afe_drift_info.vfr_drift_info.frame_counter - prev_info->afe_drift_info.vfr_drift_info.frame_counter);
      nb_fact_drift = curr_info.afe_drift_info.vfr_drift_info.acc_vfr_drift_us * voice_cmn_drift_info_ptr->nb_sampling_rate_factor;         // calculate nb_fact_drift drift.
   }

   inst_drift_us = (int32_t)(nb_fact_drift - prev_info->voice_drift_counter);           // calculate instantaneous drift, difference should be int32_t
   //dbg:   if (inst_drift_us)
   //dbg:   {
   //dbg:      MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: client(%#x) inst_drift_us(%d),nb_fact_drift(%ld)",client,inst_drift_us,nb_fact_drift);
   //dbg:   }

   //dbg: MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Voice Cmn utility: is_resync_pending(%d) for client(%d)", prev_info->is_resync_pending, (int)client);


   if( (FALSE == prev_info->is_resync_pending) &&
            ( (frame_counter_diff >= AFE_AV_TIMER_DRIFT_FRM_CNTR_LOW_THRESH) &&
			  (frame_counter_diff <= AFE_AV_TIMER_DRIFT_FRM_CNTR_HIGH_THRESH) )
			  )
    {  // if the framcounter increment is valid (increment is usually 1, but can be 2 if this function is called at av timer rate)
      int32_t num_drift_samples = 0; // number of instantaneous drift samples in 8k sampling frequency,
                                     // i.e. 1 sample in 8k = 125 micro secs

      num_drift_samples = (int32_t) voice_cmn_int_div((int32_t)inst_drift_us, (int32_t)VOICE_8K_SAMPLE_MICRO_SEC);    // convert micro secs dirft to number of 8k sample drifts, -ve DMA is faster than AV Timer
      *ss_info_counter_ptr += num_drift_samples;                                                 // accumulate the sample slip counter

      // Update the previous info location with the latest information
      prev_info->voice_drift_counter += (int64_t)(num_drift_samples * VOICE_8K_SAMPLE_MICRO_SEC);           // accumulate only multiples of 125 micro secs
      //acc_avt_drift_us is no longer used. need to update? if yes, then with what?
      if(VFR_NONE == mode)
      {
         //prev_info->afe_drift_info.avt_drift_info.acc_avt_drift_us += (num_drift_samples * VOICE_8K_SAMPLE_MICRO_SEC);   // accumulate only multiples of 125 micro secs
         prev_info->afe_drift_info.avt_drift_info.frame_counter = curr_info.afe_drift_info.avt_drift_info.frame_counter;   // store current info to previous info for next function call
      }
      else
      {
         //prev_info->afe_drift_info.vfr_drift_info.acc_vfr_drift_us += (num_drift_samples * VOICE_8K_SAMPLE_MICRO_SEC);   // accumulate only multiples of 125 micro secs
         prev_info->afe_drift_info.vfr_drift_info.frame_counter = curr_info.afe_drift_info.vfr_drift_info.frame_counter;   // store current info to previous info for next function call
      }

      if (num_drift_samples)
      {
         MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Voice Cmn utility: Drift in samples detected. client(%#x), samples(%d),voice_drift_counter(%ld),inst_drift_us=(%d),session(%#lx)", \
               (int)client, (int)num_drift_samples, (long int)prev_info->voice_drift_counter,(int)inst_drift_us,session);
      }
   }
   else if ( (prev_info->is_resync_pending) || (frame_counter_diff != 0) ) // i.e. if frame_counter_diff is < 0 or > AFE_AV_TIMER_DRIFT_FRM_CNTR_HIGH_THRESH or resync is pending
   {
      //update voice_drift_counter and drift info structure
      if(VFR_NONE == mode)
      {
         prev_info->voice_drift_counter = curr_info.afe_drift_info.avt_drift_info.acc_avt_drift_us * voice_cmn_drift_info_ptr->nb_sampling_rate_factor;
         prev_info->afe_drift_info.avt_drift_info = curr_info.afe_drift_info.avt_drift_info;
      }
      else
      {
         prev_info->voice_drift_counter = curr_info.afe_drift_info.vfr_drift_info.acc_vfr_drift_us * voice_cmn_drift_info_ptr->nb_sampling_rate_factor;
         prev_info->afe_drift_info.vfr_drift_info = curr_info.afe_drift_info.vfr_drift_info;
      }
      MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: Voice Cmn utility: Device Reset/Resync detected: client (%d), voice_drift_counter=(%ld), frame_counter=(%d), is_resync_pending(%ld), session(%#lx)",
               (int)client,(long int)prev_info->voice_drift_counter,(int)prev_info->afe_drift_info.vfr_drift_info.frame_counter,prev_info->is_resync_pending, session);

      prev_info->is_resync_pending = FALSE;  //make it false by default
   }
   else
   {
      // frame_counter_diff == 0, do nothing
   }

}

// integer divide
int32_t voice_cmn_int_div(int32_t a, int32_t b)
{
   if (0 == b)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: voice_cmn_int_div: num (%ld), den (%ld)",a,b);
      return (0); // dont want a high number here, hence returning zero, will avoid possible mem corruption
   }
   else
   {
      return (a/b);
   }
}


/***************************** start of circ buf *****************************/
circbuf_status_enum voice_circbuf_init(circbuf_struct *struct_ptr,
      int8_t *buf_ptr, int32_t max_samples, num_channels_enum num_channels,
      int32_t bits_per_channel)  //PG: pass handle and print
{
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: voice_circbuf_init: max_samples(%ld)",max_samples);
   int32_t sample_size = num_channels*(bits_per_channel >> 3);
   uint32_t buf_size = 0;

   buf_size = sample_size * max_samples;

   // max size is (128k-1) bytes
   if ((buf_size == 0) || (buf_size >= 0x00020000uL))
   {
      return CIRCBUF_FAIL;
   }

   struct_ptr->read_ptr = buf_ptr;
   struct_ptr->write_ptr = buf_ptr;
   struct_ptr->unread_samples = 0;
   struct_ptr->sample_size = sample_size;
   struct_ptr->max_samples = max_samples;
   struct_ptr->buf_ptr = buf_ptr;
   struct_ptr->end_ptr = buf_ptr + buf_size;
   return CIRCBUF_SUCCESS;
}

circbuf_status_enum voice_circbuf_read_request(circbuf_struct *struct_ptr,
       uint32_t num_samples, uint32_t *available_samples_ptr)
{
   *available_samples_ptr = struct_ptr->unread_samples;
   if (struct_ptr->unread_samples < num_samples)
   {
       return CIRCBUF_UNDERFLOW;
   }
   return CIRCBUF_SUCCESS;
}

circbuf_status_enum voice_circbuf_write_request(circbuf_struct *struct_ptr,
       uint32_t num_samples, uint32_t *available_samples_ptr)
{
   *available_samples_ptr = struct_ptr->max_samples - struct_ptr->unread_samples;
   if (*available_samples_ptr < num_samples)
   {
       return CIRCBUF_OVERFLOW;
   }
   return CIRCBUF_SUCCESS;
}

circbuf_status_enum voice_circbuf_read(circbuf_struct *struct_ptr,
      int8_t *toPtr, uint32_t num_samples, uint32_t to_buff_size)
{
   int32_t start_bytes, end_bytes;
   int8_t *read_ptr = struct_ptr->read_ptr;
   int32_t num_of_bytes = struct_ptr->sample_size * num_samples;
   int8_t *end_ptr = struct_ptr->end_ptr;
   int8_t *buf_ptr = struct_ptr->buf_ptr;

   if (struct_ptr->unread_samples < num_samples)
   {
      return CIRCBUF_UNDERFLOW;
   }

   struct_ptr->unread_samples -= num_samples;


   if ((read_ptr + num_of_bytes) > end_ptr)
   {
      end_bytes = end_ptr - read_ptr;
      start_bytes = num_of_bytes - end_bytes;

      // as the circ buf rolls over we have to use two copies
      if (NULL != toPtr)
      {
         memscpy(toPtr, to_buff_size, read_ptr, end_bytes);
         memscpy(toPtr + end_bytes, to_buff_size - end_bytes, buf_ptr, start_bytes);
      }
      struct_ptr->read_ptr = buf_ptr + start_bytes;
   }
   else
   {
      if (NULL != toPtr)
      {
         memscpy(toPtr, to_buff_size, read_ptr, num_of_bytes);
      }
      struct_ptr->read_ptr += num_of_bytes;
   }

   return CIRCBUF_SUCCESS;
}

circbuf_status_enum voice_circbuf_write(circbuf_struct *struct_ptr,
      int8_t *fromPtr, uint32_t num_samples)
{
   int32_t start_bytes, end_bytes;
   int8_t *write_ptr = struct_ptr->write_ptr;
   int32_t num_of_bytes = struct_ptr->sample_size * num_samples;
   int8_t *end_ptr = struct_ptr->end_ptr;
   int8_t *buf_ptr = struct_ptr->buf_ptr;
   circbuf_status_enum result = CIRCBUF_SUCCESS;

   if (struct_ptr->max_samples < (num_samples))
   {
      return CIRCBUF_FAIL;
   }

   if (struct_ptr->max_samples < (num_samples +
            struct_ptr->unread_samples))
   {
      uint32_t extra_samples = (num_samples + struct_ptr->unread_samples
            - struct_ptr->max_samples);
      // dummy read to drop extra samples in the circ buffer
      voice_circbuf_read(struct_ptr, NULL, extra_samples,0);
      result = CIRCBUF_OVERFLOW;
   }

   struct_ptr->unread_samples += num_samples;

   if ((write_ptr + num_of_bytes) > end_ptr)
   {
      end_bytes = end_ptr - write_ptr;
      start_bytes = num_of_bytes - end_bytes;

      // as the circ buf rolls over we have to use two copies
      memscpy(write_ptr, end_bytes, fromPtr, end_bytes);           // here dest and src size would be same as from write pntr size of circ buf left is end_bytes
      memscpy(buf_ptr, (int32_t)(struct_ptr->read_ptr - buf_ptr), fromPtr + end_bytes, start_bytes);
      struct_ptr->write_ptr = buf_ptr + start_bytes;
   }
   else
   {
      memscpy(write_ptr,(int32_t)(end_ptr - write_ptr), fromPtr, num_of_bytes);
      struct_ptr->write_ptr += num_of_bytes;
   }
   return result;
}

circbuf_status_enum voice_circbuf_reset(circbuf_struct *struct_ptr)
{
   struct_ptr->read_ptr = struct_ptr->write_ptr;
   struct_ptr->unread_samples = 0;

   return CIRCBUF_SUCCESS;
}
/***************************** end of circ buf *****************************/

/***************************** start of msg utils *****************************/
ADSPResult voice_custom_msg_send(qurt_elite_queue_t *respQ, qurt_elite_queue_t *destQ, uint32_t sec_opcode,
      uint32_t client_token, bool_t wait_for_ack)
{

   ADSPResult result=ADSP_EOK;
   uint32_t size = 0;
   elite_msg_any_t msg;
   elite_msg_custom_header_t *payload_ptr;

   size = sizeof(elite_msg_custom_header_t);
   result = elite_msg_create_msg( &msg,
                              &size,
                              ELITE_CUSTOM_MSG,
                              ((TRUE == wait_for_ack) ? respQ : NULL),
                              client_token,
                              ADSP_EOK);
   if( ADSP_FAILED( result))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: FAILED to create EliteMsg!! Not Sending Msg!!\n");
      // added return code to avoid crash incase of buffer run-out
      return result;
   }
   payload_ptr = (elite_msg_custom_header_t *) msg.pPayload;
   payload_ptr->unSecOpCode = sec_opcode;

   result = qurt_elite_queue_push_back(destQ, (uint64*)&msg);
   if (ADSP_FAILED(result))
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: FAILED to connect services!!\n");
      return result;
   }

   if (TRUE == wait_for_ack)
   {
      result = elite_svc_wait_for_ack(&msg);
      elite_msg_return_payload_buffer( &msg);
   }


   return result;
}

/*
 Helper function to send a custom msg to voice timer for subscribe and unsubscribe
 */
ADSPResult voice_custom_vt_sub_unsub_msg_send(qurt_elite_queue_t *respQ, qurt_elite_queue_t *destQ, uint32_t sec_opcode,
      uint32_t client_token, bool_t wait_for_ack, Vtm_SubUnsubMsg_t *data_ptr)
{

   ADSPResult result=ADSP_EOK;
   uint32_t size = 0;
   elite_msg_any_t msg;
   EliteMsg_CustomVtSubUnsubHeaderType *payload_ptr;

   size = sizeof(EliteMsg_CustomVtSubUnsubHeaderType);
   result = elite_msg_create_msg( &msg,
                              &size,
                              ELITE_CUSTOM_MSG,
                              respQ,
                              client_token,
                              ADSP_EOK);
   if( ADSP_FAILED( result))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: FAILED to create EliteMsg!! Not Sending Msg!!\n");
      // added return code to avoid crash incase of buffer run-out
      return result;
   }
   payload_ptr = (EliteMsg_CustomVtSubUnsubHeaderType *) msg.pPayload;
   payload_ptr->unSecOpCode = sec_opcode;
   payload_ptr->vtm_sub_unsub_payload_ptr = data_ptr;

   result = qurt_elite_queue_push_back(destQ, (uint64*)&msg);
   if (ADSP_FAILED(result))
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: FAILED to connect services!!\n");
      return result;
   }

   if (wait_for_ack)
   {
      result = elite_svc_wait_for_ack(&msg);
   }

   //elite_msg_return_payload_buffer( &msg);

   return result;
}
/***************************** end of msg utils *****************************/

/***************************** end of msg utils *****************************/

ADSPResult voice_init_async_control( async_struct_t *async_struct_ptr, elite_apr_packet_t *apr_packet_ptr, uint32_t *free_index)
{
    /* if array is full, tracking index will be all 1...complement will be all 0 */
    if( async_struct_ptr == NULL ||
        apr_packet_ptr  == NULL  ||
        free_index  == NULL  ||
        ~(async_struct_ptr->tracking_mask) == 0)
    {
       return ADSP_EFAILED;
    }

    *free_index = voice_get_free_pos(async_struct_ptr->tracking_mask);  //TODO: check if this can directly figure out if no free pos
    // TODO: make sure free_index valid?
    async_struct_ptr->tracking_mask |= (uint32_t)(1 << *free_index);
    async_struct_ptr->async_status[*free_index].state = 0;
    async_struct_ptr->async_status[*free_index].apr_packet_ptr = apr_packet_ptr;
    async_struct_ptr->async_status[*free_index].result     = ADSP_EOK;

    return ADSP_EOK;
}

ADSPResult voice_end_async_control( async_struct_t *async_struct_ptr, uint32_t free_index)
{

   async_struct_ptr->tracking_mask &= (~(uint32_t)(1 << free_index));
   async_struct_ptr->async_status[free_index].apr_packet_ptr = NULL;
   async_struct_ptr->async_status[free_index].state     = 0;
   async_struct_ptr->async_status[free_index].result    = ADSP_EOK;  //reset for next use

   return ADSP_EOK;
}

ADSPResult voice_init_async_control_rx_tx( async_struct_rxtx_t *async_struct_ptr, elite_apr_packet_t *apr_packet_ptr, uint32_t *free_index)
{
    /* if array is full, tracking index will be all 1...complement will be all 0 */
    if( async_struct_ptr == NULL ||
        apr_packet_ptr  == NULL  ||
        free_index  == NULL  ||
        ~(async_struct_ptr->tracking_mask) == 0)
    {
       return ADSP_EFAILED;
    }

    *free_index = voice_get_free_pos(async_struct_ptr->tracking_mask);  //TODO: check if this can directly figure out if no free pos
    // TODO: make sure free_index valid?
    async_struct_ptr->tracking_mask |= (uint32_t)(1 << *free_index);
    async_struct_ptr->async_status[*free_index].state_rx = 0;
    async_struct_ptr->async_status[*free_index].state_tx = 0;
    async_struct_ptr->async_status[*free_index].apr_packet_ptr = apr_packet_ptr;
    async_struct_ptr->async_status[*free_index].result_rx     = ADSP_EOK;
    async_struct_ptr->async_status[*free_index].result_tx     = ADSP_EOK;

    return ADSP_EOK;
}

ADSPResult voice_end_async_control_rx_tx( async_struct_rxtx_t *async_struct_ptr, uint32_t free_index)
{

   async_struct_ptr->tracking_mask &= (~(uint32_t)(1 << free_index));
   async_struct_ptr->async_status[free_index].apr_packet_ptr = NULL;
   async_struct_ptr->async_status[free_index].state_rx = 0;
   async_struct_ptr->async_status[free_index].state_tx = 0;
   async_struct_ptr->async_status[free_index].result_rx    = ADSP_EOK;  //reset for next use
   async_struct_ptr->async_status[free_index].result_tx    = ADSP_EOK;  //reset for next use
   return ADSP_EOK;
}

/* utility function to get sampling rate from voc media type */

uint16_t voice_get_sampling_rate( uint32_t media_type_format,int16_t BeAMR_flag, uint16_t evs_sampling_rate )
{
   uint16_t samplingRate = 0;
   switch( media_type_format)
   {

      case VSM_MEDIA_TYPE_NONE:
         samplingRate = 0;
         break;

      /* narrowband */
      case VSM_MEDIA_TYPE_13K_MODEM:
      case VSM_MEDIA_TYPE_EVRC_MODEM:
      case VSM_MEDIA_TYPE_4GV_NB_MODEM:
      case VSM_MEDIA_TYPE_EFR_MODEM:
      case VSM_MEDIA_TYPE_FR_MODEM:
      case VSM_MEDIA_TYPE_HR_MODEM:
      case VSM_MEDIA_TYPE_G711_ALAW:
      case VSM_MEDIA_TYPE_G711_MLAW:
      case VSM_MEDIA_TYPE_G711_ALAW_V2:
      case VSM_MEDIA_TYPE_G711_MLAW_V2:
      case VSM_MEDIA_TYPE_PCM_8_KHZ:
      case VSM_MEDIA_TYPE_G729AB:

         samplingRate = VOICE_NB_SAMPLING_RATE;
         break;

      case VSM_MEDIA_TYPE_AMR_NB_MODEM:
      case VSM_MEDIA_TYPE_AMR_NB_IF2:
         {
            samplingRate = VOICE_NB_SAMPLING_RATE;
            if(BeAMR_flag)
            {
               samplingRate = VOICE_WB_SAMPLING_RATE;
            }
         }
         break;
         /* wideband */
      case VSM_MEDIA_TYPE_4GV_WB_MODEM:
      case VSM_MEDIA_TYPE_4GV_NW_MODEM:
      case VSM_MEDIA_TYPE_4GV_NW:
      case VSM_MEDIA_TYPE_AMR_WB_MODEM:
      case VSM_MEDIA_TYPE_PCM_16_KHZ:
      case VSM_MEDIA_TYPE_G722:
      case VSM_MEDIA_TYPE_EAMR:
      case VSM_MEDIA_TYPE_EVRC_NW_2K:

         samplingRate = VOICE_WB_SAMPLING_RATE;
         break;
      case VSM_MEDIA_TYPE_PCM_32_KHZ:

         samplingRate = VOICE_SWB_SAMPLING_RATE;
         break;
      case VSM_MEDIA_TYPE_PCM_48_KHZ:

         samplingRate = VOICE_FB_SAMPLING_RATE;
         break;
      case VSM_MEDIA_TYPE_EVS:
          samplingRate = evs_sampling_rate;
          break;

      default:
         MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Invalid Vocoder(0x%lx)!", media_type_format);
         break;
   }

   return samplingRate;
}

/* utility function to get logging ids for various sampling rate in VPM and VSM */
/* 00 - NB, 01 - WB, 10 -FB  */

uint32_t voice_get_sampling_rate_log_id( uint16_t sampling_rate)
{
    uint32_t sampling_rate_log_id=0;

    switch(sampling_rate)
    {
      case VOICE_NB_SAMPLING_RATE:

        sampling_rate_log_id = VOICE_NB_LOG_ID;
        break;

      case VOICE_WB_SAMPLING_RATE:

        sampling_rate_log_id = VOICE_WB_LOG_ID;
        break;

      case VOICE_SWB_SAMPLING_RATE:

        sampling_rate_log_id = VOICE_SWB_LOG_ID;
        break;

      case VOICE_FB_SAMPLING_RATE:

        sampling_rate_log_id = VOICE_FB_LOG_ID;
        break;

      default:
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Invalid sampling rate(%d)!", sampling_rate);
        break;
     }

     return sampling_rate_log_id;
}



ADSPResult voice_shared_memory_map_regions_process(uint32_t mem_map_client,
                                      elite_apr_handle_t apr_handle,
                                      elite_apr_packet_t *apr_pkt_ptr)
{
    ADSPResult rc = ADSP_EOK;
    void * pPayload,*pDummy;
    voice_cmd_shared_mem_map_regions_t *mem_map_regions_payload_ptr;
    uint32_t mem_map_handle=0;
    elite_apr_packet_t *pRespPkt=NULL;
    QURT_ELITE_MEMORYPOOLTYPE eMemPool;
    voice_shared_map_region_payload_t **ppRegion;

    elite_apr_if_get_payload( (void**) &pPayload, apr_pkt_ptr );

    mem_map_regions_payload_ptr = (voice_cmd_shared_mem_map_regions_t*)pPayload;

    MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: voice map mem util: Received mem_pool_id(%#x), property flag(%#x), num_regions(%#x) ",
          (unsigned int)mem_map_regions_payload_ptr->mem_pool_id,
          (unsigned int)mem_map_regions_payload_ptr->property_flag,
          (unsigned int)mem_map_regions_payload_ptr->num_regions);

    switch (mem_map_regions_payload_ptr->mem_pool_id)
    {
       case VOICE_MAP_MEMORY_SHMEM8_4K_POOL:
          {
             eMemPool = QURT_ELITE_MEMORYMAP_SHMEM8_4K_POOL;
             break;
          }
       default:
          {
             rc = ADSP_EBADPARAM;
             MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: voice map mem util: MemMapRegCmdHandler: Received Invalid PoolID: %d", mem_map_regions_payload_ptr->mem_pool_id);
             goto _voice_mem_shared_memory_map_regions_cmd_handler_bail_1;
          }
    }

     /* Allocate the response packet */
    if (APR_EOK != (rc = elite_apr_if_alloc_cmd_rsp(
         apr_handle,
         elite_apr_if_get_dst_addr( apr_pkt_ptr),
         elite_apr_if_get_dst_port( apr_pkt_ptr),
         elite_apr_if_get_src_addr( apr_pkt_ptr),
         elite_apr_if_get_src_port( apr_pkt_ptr),
         elite_apr_if_get_client_token( apr_pkt_ptr),
         VOICE_RSP_SHARED_MEM_MAP_REGIONS,
         sizeof(voice_rsp_shared_mem_map_regions_t),
         &pRespPkt ))
        )
    {
        rc = ADSP_ENOMEMORY;
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: voice map mem util: MemMapRegCmdHandler: memory map response packet allocation failed with error code  = %d!", rc);
        goto _voice_mem_shared_memory_map_regions_cmd_handler_bail_1;
    }

    if (NULL == pRespPkt)
    {
       rc = ADSP_ENOMEMORY;
       MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: voice map mem util: MemMapRegCmdHandler: memory map response packet allocation failed with null return pointer!");
       goto _voice_mem_shared_memory_map_regions_cmd_handler_bail_1;
    }

    /* prepare the Response payload pointer */
    voice_rsp_shared_mem_map_regions_t *pRespPayload;
    elite_apr_if_get_payload((&pDummy), pRespPkt);
    pRespPayload = (voice_rsp_shared_mem_map_regions_t *)pDummy;


    pDummy = ((uint8_t *)pPayload + sizeof(voice_cmd_shared_mem_map_regions_t));
    ppRegion = (voice_shared_map_region_payload_t **)&pDummy;

    // allocate qurt_elite_memorymap_shm_region_t, why not direct cast? because want to be free from
    // api data structure change or any compiler pack
    qurt_elite_memorymap_shm_region_t* phy_regions;
    if (NULL == (phy_regions = (qurt_elite_memorymap_shm_region_t*) qurt_elite_memory_malloc(
        sizeof(qurt_elite_memorymap_shm_region_t) * mem_map_regions_payload_ptr->num_regions,
        QURT_ELITE_HEAP_DEFAULT)))
    {
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: voice map mem util: Failed to allocate memory for qurt_elite_memorymap_shm_region_t");
        rc = ADSP_ENOMEMORY;
        goto _voice_mem_shared_memory_map_regions_cmd_handler_bail_1;
    }
    for (int i=0; i<mem_map_regions_payload_ptr->num_regions; ++i)
    {
        phy_regions[i].shm_addr_lsw = (*ppRegion+i)->shm_addr_lsw;
        phy_regions[i].shm_addr_msw = (*ppRegion+i)->shm_addr_msw;
        phy_regions[i].mem_size = (*ppRegion+i)->mem_size_bytes;
        MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: voice map mem util: region(%d): shm_addr_lsw(%#x), shm_addr_msw(%#x), mem_size(%#x) ", i, (unsigned int)phy_regions[i].shm_addr_lsw, (unsigned int)phy_regions[i].shm_addr_msw, (unsigned int)phy_regions[i].mem_size);
    }
    if (VOICE_PHYSICAL_SHARED_MEM_ADDR == mem_map_regions_payload_ptr->property_flag)
    {
        //physical mapping
        if (ADSP_FAILED(rc = qurt_elite_memorymap_shm_mem_map(mem_map_client,phy_regions,
                                                    mem_map_regions_payload_ptr->num_regions,
                                                    HMEM_CACHE_WRITEBACK_L2CACHEABLE,
                                                    eMemPool,
                                                    &mem_map_handle)))
        {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: voice map mem util: Failed to map the physical memory, error code is 0x%x",rc );
            goto _voice_mem_shared_memory_map_regions_cmd_handler_bail_2;
        }
    }
    else if (VOICE_VIRTUAL_SHARED_MEM_ADDR ==  mem_map_regions_payload_ptr->property_flag)
    {
        //virtual mapping
        if (ADSP_FAILED(rc = qurt_elite_memorymap_virtaddr_mem_map(mem_map_client,phy_regions,
                                                    mem_map_regions_payload_ptr->num_regions,
                                                    HMEM_CACHE_WRITEBACK_L2CACHEABLE,
                                                    eMemPool,
                                                    &mem_map_handle)))
        {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: voice map mem util: Failed to map the virual memory, error code is 0x%x",rc );
            goto _voice_mem_shared_memory_map_regions_cmd_handler_bail_2;
        }

    }
    else
    {
        rc = ADSP_EBADPARAM;
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: voice map mem util: invalid property flag received in the payload, error code is 0x%x",rc );
        goto _voice_mem_shared_memory_map_regions_cmd_handler_bail_2;
    }

    //store the memory map handle in the response payload
    pRespPayload->mem_map_handle         = mem_map_handle;

    /* send response */
    if (APR_EOK != (rc = elite_apr_if_async_send(apr_handle, pRespPkt)))
    {
        rc = ADSP_EFAILED;
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: voice map mem util: failed to send the memory map response command, error code is  = 0x%x", rc);
        (void)elite_apr_if_free(apr_handle,pRespPkt);
        goto _voice_mem_shared_memory_map_regions_cmd_handler_bail_2;
    }

    qurt_elite_memory_free(phy_regions);
    elite_apr_if_free( apr_handle,  apr_pkt_ptr);
    return rc;

_voice_mem_shared_memory_map_regions_cmd_handler_bail_2:
    qurt_elite_memory_free(phy_regions);
_voice_mem_shared_memory_map_regions_cmd_handler_bail_1:
    elite_apr_if_end_cmd(apr_handle, apr_pkt_ptr, rc);
    return rc;

}

ADSPResult voice_shared_memory_un_map_regions_process(uint32_t mem_map_client,
                                        elite_apr_handle_t apr_handle,
                                        elite_apr_packet_t *apr_pkt_ptr)
{
    ADSPResult rc = ADSP_EOK;
    void * pPayload;
    voice_cmd_shared_mem_unmap_regions_t *pMemUnMapRegionsPayload;

    elite_apr_if_get_payload( (void**) &pPayload, apr_pkt_ptr );

    pMemUnMapRegionsPayload = (voice_cmd_shared_mem_unmap_regions_t *)pPayload;

    if (0 == pMemUnMapRegionsPayload->mem_map_handle)
    {
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: voice unmap util: null memory map handle received in the unmap cmd payload");
        rc = ADSP_EBADPARAM;
        goto _voice_mem_shared_memory_un_map_regions_cmd_handler_bail_1;
    }

    if (ADSP_FAILED(rc = qurt_elite_memorymap_shm_mem_unmap(mem_map_client,pMemUnMapRegionsPayload->mem_map_handle)))
    {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: voice unmap util: Failed to unmap the phyiscal memory, error code is 0x%x",rc );
    }

_voice_mem_shared_memory_un_map_regions_cmd_handler_bail_1:
    elite_apr_if_end_cmd(apr_handle, apr_pkt_ptr, rc);
    return rc;

}

ADSPResult voice_cmn_check_align_size(
            uint32_t payload_address_lsw,
            uint32_t payload_address_msw,
            uint32_t payload_size,
            uint32_t alignment_check
      )
{
   ADSPResult result = ADSP_EOK;
   uint32_t alignment_check_mask = (alignment_check-1);
   // Donot allow NULL address
   if (((NULL == payload_address_lsw) && (NULL == payload_address_msw)) || (payload_address_lsw & alignment_check_mask) || (payload_size & alignment_check_mask)) // check addr alignment
   {
      MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: voice cmn util: Address NULL or not aligned or size is not multiple of cache width bytes, reqd alignment(%d) in bytes lsw(%#lx) msw(%#lx) size(%#lx)",(int)alignment_check,payload_address_lsw, payload_address_msw, payload_size);
      result = ADSP_EBADPARAM;
   }
   return result;
}


ADSPResult voice_mmpm_register(  voice_commom_mmpm_t *mmpm,
                                 const char *clientName,
                                 MmpmCoreIdType coreId)
{

   ADSPResult result = ADSP_EOK;

#if (ADSPPM_INTEGRATION==1)

   mmpm->status = VOICE_COMMON_MMPM_STATUS_INIT;

   mmpm->reg_param.rev            = MMPM_REVISION;
   mmpm->reg_param.coreId         = coreId;
   mmpm->reg_param.instanceId     = MMPM_CORE_INSTANCE_0;
   mmpm->reg_param.MMPM_Callback  = NULL;                    // voice API supports sync only, no callback
   mmpm->reg_param.pClientName    = (char *) clientName;
   mmpm->reg_param.pwrCtrlFlag    = PWR_CTRL_NONE;
   mmpm->reg_param.callBackFlag   = CALLBACK_NONE;
   mmpm->reg_param.cbFcnStackSize = 4096;

   mmpm->client_id =  MMPM_Register_Ext( &mmpm->reg_param);

   // client_id should be non-zero
   if(0 == mmpm->client_id)
   {
      return ADSP_EFAILED;
   }
   else
   {
      mmpm->status = VOICE_COMMON_MMPM_STATUS_REGISTERED;
   }

#endif // ADSPPM_INTEGRATION

   return result;
}

ADSPResult voice_mmpm_config(  voice_commom_mmpm_t *mmpm,
                               uint32_t uTotalThreadMips,
                               uint32_t uFloorThreadMips,
                               uint32_t uAxiIBw,
                               uint32_t uAxiUsage,
                               uint32_t uSleepLatency)
{

   ADSPResult          result = ADSP_EOK;

#if (ADSPPM_INTEGRATION==1)
   MMPM_STATUS         mmpmStatusResult[3] = {MMPM_STATUS_SUCCESS, MMPM_STATUS_SUCCESS, MMPM_STATUS_SUCCESS};    // bundled request, one for MIPS, one for BW, one for latency

   MmpmRscParamType    rscParam[3];                  // bundled request, one for MIPS, one for BW, one for latency
   MMPM_STATUS         mmpmApiResult;
   MmpmRscExtParamType rscExt;
   MmpmGenBwReqType    bwReq;

   if( VOICE_COMMON_MMPM_STATUS_RESOURCES_REQUESTED != mmpm->status &&
       VOICE_COMMON_MMPM_STATUS_REGISTERED          != mmpm->status)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: MMPM_Request_Ext for Voice client_id(%lu) improper state (%u)", mmpm->client_id, mmpm->status);
   }

   // fill out bandwidth request

   mmpm->bw_val.busRoute.masterPort                = MMPM_BW_PORT_ID_ADSP_MASTER;
   mmpm->bw_val.busRoute.slavePort                 = MMPM_BW_PORT_ID_DDR_SLAVE;
   mmpm->bw_val.bwValue.busBwValue.usageType       = MMPM_BW_USAGE_LPASS_DSP;
   mmpm->bw_val.bwValue.busBwValue.bwBytePerSec    = uAxiIBw;     // peak value
   mmpm->bw_val.bwValue.busBwValue.usagePercentage = uAxiUsage;

   bwReq.numOfBw         = 1;
   bwReq.pBandWidthArray = &mmpm->bw_val;

   // fill out MIPS request
   // codeLocation NOT needed for Clock only request:  mmpm->mips_req.codeLocation  = MMPM_BW_PORT_ID_DDR_SLAVE;

   mmpm->mips_req.reqOperation  = MMPM_MIPS_REQUEST_CPU_CLOCK_ONLY;
   mmpm->mips_req.mipsPerThread = uFloorThreadMips;
   mmpm->mips_req.mipsTotal     = uTotalThreadMips;

   // fill out latency request

   mmpm->sleep_microsec = uSleepLatency;

   // fill out array of BW and MIPS

   rscParam[0].rscId                  = MMPM_RSC_ID_GENERIC_BW;
   rscParam[0].rscParam.pGenBwReq     = &bwReq;

   rscParam[1].rscId                  = MMPM_RSC_ID_MIPS_EXT;
   rscParam[1].rscParam.pMipsExt      = &mmpm->mips_req;

   rscParam[2].rscId                  = MMPM_RSC_ID_SLEEP_LATENCY;
   rscParam[2].rscParam.sleepMicroSec = mmpm->sleep_microsec;

   // fill out bundled request using request array.

   rscExt.apiType               = MMPM_API_TYPE_SYNC;                // synchronous call
   rscExt.numOfReq              = 3;
   rscExt.pReqArray             = rscParam;
   rscExt.pStsArray             = mmpmStatusResult;

   // make the request
   mmpmApiResult = MMPM_Request_Ext( mmpm->client_id, &rscExt);

   // check API call result and status result for bundled request
   if( MMPM_STATUS_SUCCESS != mmpmApiResult ||
       MMPM_STATUS_SUCCESS != rscExt.pStsArray[0] ||
       MMPM_STATUS_SUCCESS != rscExt.pStsArray[1] ||
       MMPM_STATUS_SUCCESS != rscExt.pStsArray[2])
   {

      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: MMPM_Request_Ext for Voice client_id(%lu) failed", mmpm->client_id);
      MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: mmpmApiResult(%lu) bwResult(%lu) mipsResult(%lu) latencyResult(%lu)",
                                           mmpmApiResult, rscExt.pStsArray[0], rscExt.pStsArray[1], rscExt.pStsArray[2] );

      result = ADSP_EFAILED;
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: MMPM_Request_Ext for Voice client_id(%lu) succeeded", mmpm->client_id);
   }

   // if ANY of the requests succeeded, indicate we have requested resources, so they can later be released
   if( MMPM_STATUS_SUCCESS == rscExt.pStsArray[0] ||
       MMPM_STATUS_SUCCESS == rscExt.pStsArray[1] ||
       MMPM_STATUS_SUCCESS == rscExt.pStsArray[2])
   {
      mmpm->status = VOICE_COMMON_MMPM_STATUS_RESOURCES_REQUESTED;
   }

#endif // ADSPPM_INTEGRATION
   return (result);
}

ADSPResult voice_mmpm_release(
      voice_commom_mmpm_t *mmpm
)
{
   ADSPResult          result = ADSP_EOK;

#if (ADSPPM_INTEGRATION==1)
   MMPM_STATUS         mmpmStatusResult[3] = {MMPM_STATUS_SUCCESS, MMPM_STATUS_SUCCESS, MMPM_STATUS_SUCCESS};    // bundled request, one for MIPS, one for BW
   MmpmRscParamType    rscParam[3];            // bundled request, one for MIPS, one for BW
   MMPM_STATUS         mmpmApiResult;
   MmpmRscExtParamType rscExt;
   MmpmGenBwReqType    bwReq;

   if( VOICE_COMMON_MMPM_STATUS_RESOURCES_REQUESTED != mmpm->status)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Voice client_id(%lu) cannot release resources it never requested", mmpm->client_id);
      return ADSP_EFAILED;
   }

   // fill out bandwidth request

   bwReq.numOfBw         = 1;
   bwReq.pBandWidthArray = &mmpm->bw_val;                     // not sure if API needs this or keeps a record

   rscParam[0].rscId              = MMPM_RSC_ID_GENERIC_BW;
   rscParam[0].rscParam.pGenBwReq = &bwReq;                   // not sure if API needs this or keeps a record

   rscParam[1].rscId              = MMPM_RSC_ID_MIPS_EXT;
   rscParam[1].rscParam.pMipsExt  = &mmpm->mips_req;          // not sure if API needs this or keeps a record

   rscParam[2].rscId                  = MMPM_RSC_ID_SLEEP_LATENCY;
   rscParam[2].rscParam.sleepMicroSec = mmpm->sleep_microsec; // not sure if API needs this or keeps a record

   // fill out bundled request using request array.

   rscExt.apiType               = MMPM_API_TYPE_SYNC;         // synchronous call
   rscExt.numOfReq              = 3;
   rscExt.pReqArray             = rscParam;
   rscExt.pStsArray             = mmpmStatusResult;

   // make the release request
   mmpmApiResult = MMPM_Release_Ext( mmpm->client_id, &rscExt);

   // check API call result and status result for bundled request. Only change status
   // to resources released if all release requests succeeded.
   if( MMPM_STATUS_SUCCESS != mmpmApiResult ||
       MMPM_STATUS_SUCCESS != rscExt.pStsArray[0] ||
       MMPM_STATUS_SUCCESS != rscExt.pStsArray[1] ||
       MMPM_STATUS_SUCCESS != rscExt.pStsArray[2])
   {

      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: MMPM_Release_Ext for Voice client_id(%lu) failed", mmpm->client_id);
      MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: mmpmApiResult(%lu) bwResult(%lu) mipsResult(%lu) latencyResult(%lu)",
                                           mmpmApiResult, rscExt.pStsArray[0], rscExt.pStsArray[1], rscExt.pStsArray[2] );
      result = ADSP_EFAILED;
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: MMPM_Release_Ext for Voice client_id(%lu) succeeded", mmpm->client_id);
      mmpm->status = VOICE_COMMON_MMPM_STATUS_RESOURCES_RELEASED;
   }

#endif // ADSPPM_INTEGRATION
   return result;

}


ADSPResult vcmn_calc_processing_time(uint32_t dsp_clock_mhz,uint32_t kpps,uint32_t frame_size_ms, uint32_t* processing_time)
{
   if( 0 == dsp_clock_mhz )
   {
       MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: voice cmn: dsp_clock_mhz = 0");
       return ADSP_EBADPARAM;
   }
   if (NULL != processing_time)
   {
      *processing_time = (uint32_t)(((uint64_t)VOICE_CACHE_MISS_FACTOR_OVER_1MHZ * kpps * frame_size_ms) / (dsp_clock_mhz*1000));
      //dbg: MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: voice cmn: the clock info num(%lld) den(%lld) num/den(%lld)", (uint64_t)VOICE_CACHE_MISS_FACTOR_OVER_1MHZ * (uint64_t)kpps, ((uint64_t)dsp_clock * (uint64_t)(1000/frame_size_ms)), (*processing_time));
      /* mpps*1.5/(clock freq per thread*no. of processing frames per sec)*/
      return ADSP_EOK;
   }
   else
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: voice cmn: NULL pointer for processing timer");
      return ADSP_EBADPARAM;
   }
}

ADSPResult vcmn_find_vockpps_table(const vcmn_mediatype_kpps_t *voc_kpps_table, uint32_t media_type, uint32_t *kpps)
{
   uint16_t i = 0;
   if ((NULL == voc_kpps_table) || (NULL == kpps))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: voice cmn: bad pointers");
      return ADSP_EBADPARAM;
   }

   *kpps = 0;

   while(VOICE_INVALID_MEDIATYPE!= voc_kpps_table[i].media_type)
   {
      if (voc_kpps_table[i].media_type == media_type)
      {
         *kpps = voc_kpps_table[i].kpps;
         break;
      }
      i++;
   }


   if(VOICE_INVALID_MEDIATYPE == voc_kpps_table[i].media_type)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: voice cmn: couldn't find kpps info for vocoder (%ld)", media_type);
      return ADSP_EBADPARAM;
   }
   return ADSP_EOK;
}

ADSPResult vcmn_find_kpps_table(const vcmn_sampling_kpps_t *sampling_kpps_table, uint32_t sampling_rate, uint32_t *kpps)
{
   uint16_t i = 0;
   if ((NULL == sampling_kpps_table) || (NULL == kpps))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: voice cmn: bad pointer sampling_kpps_table(%x) kpps(%x)", (unsigned int)sampling_kpps_table, (unsigned int)kpps);
      return ADSP_EBADPARAM;
   }

   *kpps = 0;
   while(VOICE_INVALID_SAMPLING_RATE != sampling_kpps_table[i].sampling_rate)
   {
      //dbg: MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: voice cmn: sampling_kpps_table(%lx) sampling rate(%d)", (unsigned int)sampling_kpps_table, (unsigned int)sampling_kpps_table[i].sampling_rate); // for debug
      if (sampling_kpps_table[i].sampling_rate == sampling_rate)
      {
         *kpps = sampling_kpps_table[i].kpps;
         break;
      }
      i++;
   }

   if(VOICE_INVALID_SAMPLING_RATE == sampling_kpps_table[i].sampling_rate)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: voice cmn: couldn't find kpps info for sampling rate(%ld) sampling_kpps_table(%x)", sampling_rate, (unsigned int)sampling_kpps_table);
      return ADSP_EBADPARAM;
   }
   return ADSP_EOK;
}

ADSPResult voice_init_guard_band(int8_t *guard_band_ptrs[], uint32_t *module_size_ptr, int16_t no_of_modules, int8_t *mem_pointer)
{
   int8_t i;
   if((NULL != mem_pointer) && (NULL != module_size_ptr ) && (NULL != guard_band_ptrs))
   {
      for(i = 0 ; i < no_of_modules ; i++)
      {
         // Populate Guard band memory locations allocated after each module and update in guard band pointers array.
         mem_pointer           +=  *module_size_ptr;
         guard_band_ptrs[i]     =  mem_pointer;              // Update Guard band pointer array
         mem_pointer           +=  VOICE_GUARD_BAND_SIZE;    // Increament mem_pointer with guard band size

         module_size_ptr++;   // Increment module_size pointer to point to next module_size

         // Initialize guard band memory with known value.
         *(uint64_t *)guard_band_ptrs[i] = (uint64_t)VOICE_GUARD_BAND_CONTENTS;
      }
   }
   else
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: In init guard band function received NULL pointer");
      return ADSP_ENOMEMORY; // returning ADSP_ENOMEMORY error code because guard band memory pointer is null
   }
   return ADSP_EOK;
}

ADSPResult voice_check_guard_band(int8_t *guard_band_ptrs[], int16_t no_of_modules, voice_path tx_rx_flag, uint32_t session_no)
{
   int8_t i;

   if (NULL == guard_band_ptrs)
   {
	   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: guard_band_ptrs is NULL tx_rx_flag=%d, Session No=%lx ", tx_rx_flag, session_no);
	   return ADSP_ENOMEMORY;
   }
   for(i = 0 ; i < no_of_modules ; i++)
   {
      if(NULL == guard_band_ptrs[i])
      {
         MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: In check_guard_band function, Guard band pointer with index = %d is NULL, tx_rx_flag=%d, Session No=%lx ", i, tx_rx_flag, session_no);
         return ADSP_ENOMEMORY; // returning ADSP_ENOMEMORY error code because guard band memory pointer is null
      }
      void* temp_ptr = guard_band_ptrs[i];
      uint64_t guard_band_temp_content  =  *(uint64_t *)temp_ptr;
      if (VOICE_GUARD_BAND_CONTENTS != guard_band_temp_content)
      {
         if (RX == tx_rx_flag)
         {
             MSG_3(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "VCP: Guard band Mem corruption is detected in RX voice modules. module index=%d, mem contents=%llx, Session No=%lx",
               i,guard_band_temp_content, session_no);
            return ADSP_ENOMEMORY; // returning ADSP_ENOMEMORY error code because guard band memory is corrupted
         }
         else
         {
            MSG_3(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "VCP: Guard band Mem corruption is detected in TX voice modules. module index=%d, mem contents=%llx, Session No=%lx",
              i,guard_band_temp_content, session_no);
            return ADSP_ENOMEMORY; // returning ADSP_ENOMEMORY error code because guard band memory is corrupted
         }
      }
   }
   return ADSP_EOK;
}

int32_t voice_add_guard_band(uint32_t input_size)
{
   uint32_t output_size = input_size + VOICE_GUARD_BAND_SIZE;
   return (output_size);
}

int16_t voice_get_dec_process_time(uint16_t samp_rate)
{
   int16_t process_time = 0;
   switch (samp_rate)
   {
      case VOICE_NB_SAMPLING_RATE:
         {
            process_time = VOICE_NB_DEC_NOVFR_MIDCLK_PROCTIME_US;
            break;
         }
      case VOICE_WB_SAMPLING_RATE:
         {
            process_time = VOICE_WB_DEC_NOVFR_MIDCLK_PROCTIME_US;
            break;
         }
      case VOICE_FB_SAMPLING_RATE:
         {
            process_time = VOICE_FB_DEC_NOVFR_MIDCLK_PROCTIME_US;
            break;
         }
      default:
         {
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: voice_get_dec_process_time: invalid sampling rate");
         }
   }

   // Apply cache factor
   process_time = process_time * VOICE_CACHE_MISS_FACTOR_OVER_1MHZ/1000;

   return process_time;
}

ADSPResult voice_cmn_send_vds_elite_request(uint32_t vds_client_id, uint32_t vds_client_token, uint32_t buffer_id, qurt_elite_queue_t* peer_ptr,
                                            elite_svc_handle_t* vds_handle_ptr, elite_msg_any_t* msg_ptr, uint32_t session_num)
{
   ADSPResult result;
   elite_msg_any_t msg;
   uint32_t msg_size = sizeof(elite_msg_custom_vds_deliver_elite_type);
   result = elite_msg_create_msg(&msg, &msg_size, ELITE_CUSTOM_MSG, NULL, NULL, ADSP_EOK);
   if(ADSP_FAILED(result))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Client failed to create message, result %d, session(%x)", result, (int)session_num);
      return result;
   }
   elite_msg_custom_vds_deliver_elite_type* payload = (elite_msg_custom_vds_deliver_elite_type*)msg.pPayload;
   payload->unSecOpCode = VDS_DELIVER_ELITE_BUFFER;
   vds_deliver_elite_msg_t* delivery_params = &payload->delivery_request;
   delivery_params->client_id = vds_client_id;
   delivery_params->client_token = vds_client_token;
   delivery_params->buffer_id = buffer_id;
   delivery_params->peer_ptr = peer_ptr;
   delivery_params->msg = *msg_ptr;
   result = qurt_elite_queue_push_back(vds_handle_ptr->dataQ, (uint64_t*)&msg);
   if(ADSP_FAILED(result))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Client failed to push to vds, result %d, session(%x)", result, (int)session_num);
      return result;
   }
   //dbg: MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: Client queued request to VDS, session(%x)", (int)session_num);
   return ADSP_EOK;
}

ADSPResult voice_cmn_send_vds_apr_request(elite_svc_handle_t* vds_handle_ptr, vds_deliver_apr_packet_t* vds_packet_ptr, uint32_t session_num)
{
   ADSPResult result;
   elite_msg_any_t msg;
   uint32_t msg_size = sizeof(elite_msg_custom_vds_deliver_apr_type);
   result = elite_msg_create_msg(&msg, &msg_size, ELITE_CUSTOM_MSG, NULL, NULL, ADSP_EOK);
   if(ADSP_FAILED(result))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Client failed to create message, result %d, session(%x)", result, (int)session_num);
      return result;
   }
   elite_msg_custom_vds_deliver_apr_type* payload = (elite_msg_custom_vds_deliver_apr_type*)msg.pPayload;
   payload->unSecOpCode = VDS_DELIVER_APR_PACKET;
   vds_deliver_apr_packet_t* delivery_params = &payload->delivery_request;
   memscpy(delivery_params, sizeof(vds_deliver_apr_packet_t), vds_packet_ptr, sizeof(vds_deliver_apr_packet_t));

   // MSG_5(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: Client queued request to VDS, session(%x), client token(%x), Client Id(%d), apr handle ptr(%x), apr pkt ptr(%x)", (int)session_num,(int)delivery_params->client_token, (int)delivery_params->client_id, (int)delivery_params->apr_handle_ptr, (int)delivery_params->apr_packet_ptr);
   result = qurt_elite_queue_push_back(vds_handle_ptr->dataQ,(uint64_t*)&msg);
   if(ADSP_FAILED(result))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Client failed to push to vds, result %d, session(%x)", result, (int)session_num);
      elite_msg_return_payload_buffer(&msg);
      return result;
   }
   //dbg: MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: Client queued request to VDS, session(%x)", (int)session_num);
   return ADSP_EOK;
}


ADSPResult voice_cmn_send_vds_command(uint32_t* vds_client_id, uint32_t vds_client_token, qurt_elite_queue_t* respq_ptr, uint32_t sec_opcode,
                                      qurt_elite_signal_t** signal_ptr, qurt_elite_signal_t* error_signal_ptr, elite_svc_handle_t* vds_handle_ptr,
                                      uint32_t session_num, bool_t is_delivery_tick_reqd, qurt_elite_signal_t* resync_response_signal_ptr)
{
   ADSPResult result;
   elite_msg_any_t msg;
   vds_sub_t sub_params;
   uint32_t msg_size = sizeof(elite_msg_custom_vds_sub_type);
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Client VDS (un)subscribe begin, session(%x)", (int)session_num);
   result = elite_msg_create_msg(&msg,
         &msg_size,
         ELITE_CUSTOM_MSG,
         respq_ptr,
         NULL,
         ADSP_EOK);
   if(ADSP_FAILED(result))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Client failed to create message, result %d, session(%x)", result, (int)session_num);
      return result;
   }
   elite_msg_custom_vds_sub_type *vds_sub_msg = (elite_msg_custom_vds_sub_type*)msg.pPayload;
   vds_sub_msg->unSecOpCode = sec_opcode;
   sub_params.client_token = vds_client_token;
   // Populate this value regardless of subscribe/unsubscribe. Subscribe will just ignore it anyway
   sub_params.client_id = *vds_client_id;
   // No need to propagate delivery error
   sub_params.error_signal_ptr = error_signal_ptr;
   // Initializing signal_ptr for safety
   sub_params.signal_ptr= NULL;
   // set whether client needs to wait for delivery tick or not
   sub_params.is_delivery_tick_reqd = is_delivery_tick_reqd;
   sub_params.resync_response_signal_ptr = resync_response_signal_ptr;
   vds_sub_msg->sub_req_ptr = &sub_params;
   // Issue command
   result = qurt_elite_queue_push_back(vds_handle_ptr->cmdQ, (uint64_t*)&msg);
   if(ADSP_FAILED(result))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Client failed to deliver command, result %d, session(%x)", result, (int)session_num);
      elite_msg_return_payload_buffer(&msg);
      return result;
   }
   // Wait for ack
   result = elite_svc_wait_for_ack(&msg);
   if(ADSP_FAILED(result) || ADSP_FAILED(vds_sub_msg->unResponseResult))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Client wait failed, result %d, session(%x)", result, (int)session_num);
      elite_msg_return_payload_buffer(&msg);
      return ADSP_EFAILED;
   }
   if(VDS_SUBSCRIBE == sec_opcode)
   {
      //store client id and signal
      *signal_ptr = sub_params.signal_ptr;
      *vds_client_id = sub_params.client_id;
   }
   else
   {
      //Clear all info
      *signal_ptr = NULL;
      *vds_client_id = 0;
   }

   // Return buffer
   elite_msg_return_payload_buffer(&msg);

   MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Client VDS (un)subscribe end, client id(%d), signal %x,session(%x)",
         (int)(*vds_client_id), (int)*signal_ptr, (int)session_num);
   return result;
}
