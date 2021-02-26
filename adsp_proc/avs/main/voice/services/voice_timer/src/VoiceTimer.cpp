/*========================================================================

*//** @file VoiceTimer.cpp

Copyright (c) 2010-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

This file contains the code for Voice Timer service - centralized entity which
takes care of all the timing needs required by dynamic services. This
service has one thread and can receive the following command msgs:
1) subscribe msg- use this if you need hard vfr tick or sample slip info
2) unsubscribe msg - use this if you no longer need hard vfr tick and sample slip info
3) hard vfr msg - AFE sends this msg every hard vfr tick it receives and also updates
msg payload with sample slip info for all active devices
*//*====================================================================== */

/*========================================================================
  Edit History

  $Header: //components/rel/avs.adsp/2.8.5/voice/services/voice_timer/src/VoiceTimer.cpp#1 $

  when       who     what, where, why
  --------   ---     -------------------------------------------------------
  06/21/10   pg      1) Added hard vfr / sample slip support
  2) change subscribe/unsubscribe to FADD cust msgs
  3) major cleanup - use function handlers, coding convention
  05/14/10   pg      Spin off VoiceTimer from VoiceDevSvc

  ========================================================================== */


/* =======================================================================
   INCLUDE FILES FOR MODULE
   ========================================================================== */
#include "qurt_elite.h"
#include "Elite.h"
#include "EliteMsg_Util.h"
#include "VoiceTimer.h"
#include "VoiceCmnUtils.h"
#include "AFEDevService.h"
#include "EliteMsg_AfeCustom.h"
#include "adsp_vcmn_api.h"
#include "voice_delivery.h"
/*--------------------------------------------------------------*/
/* Macro definitions                                            */
/* -------------------------------------------------------------*/
static const int32_t VTM_SOFT_VFR_MASK=0x80000000;
static const int32_t VTM_CMD_MASK=0x40000000;
static const int32_t VTM_TEST_SIGNAL_MASK=0x20000000;

#define VTM_ALL_MASKS         (VTM_SOFT_VFR_MASK |\
      VTM_CMD_MASK|VTM_TEST_SIGNAL_MASK)

#if defined(VTM_ONESHOT_DEBUG)
static const int32_t TEST_TIMER_DURATION = 15000;
#endif
/* -----------------------------------------------------------------------
 ** Globals
 ** ----------------------------------------------------------------------- */
// Global handle to VDS instance
elite_svc_handle_t* vds_handle;

/* -----------------------------------------------------------------------
 ** Constant / Define Declarations
 ** ----------------------------------------------------------------------- */
// Maximum number of commands expected ever in command queue.
static const uint32_t MAX_CMD_Q_ELEMENTS = 8;

// Thread name
static char_t VTM_THREAD_NAME[]="VTM";

// Thread stack size
static const uint32_t VTM_THREAD_STACK_SIZE = (1024*2 + 512);

// This contains all the required data for a service instance.
static char_t vtm_cmdq_name[]="VTimerQ";  //todo: change VTimerQ

static char_t vtm_cmd_q_buf[QURT_ELITE_QUEUE_GET_REQUIRED_BYTES(MAX_CMD_Q_ELEMENTS)];

/* -----------------------------------------------------------------------
 ** Function prototypes
 ** ----------------------------------------------------------------------- */

// Main work loop for voice timer thread. Pulls msgs off of queues and processes them.
static ADSPResult vtm_thread(void* instance_ptr); //todo: change int32_t to ADSPResult

// Channel handler functions
static ADSPResult vtm_soft_vfr_handler(void* instance_ptr);
static ADSPResult vtm_cmd_handler(void* instance_ptr);
static ADSPResult vtm_test_signal_handler(void* instance_ptr);

// Cmd msg handler functions
static ADSPResult vtm_subscribe_msg(void *timer_server_ptr, elite_msg_any_t *data_ptr);
static ADSPResult vtm_unsubscribe_msg(void *timer_server_ptr, elite_msg_any_t *data_ptr);
static ADSPResult vtm_hard_vfr_msg(void *timer_server_ptr, elite_msg_any_t *data_ptr);
static ADSPResult vtm_start_oneshot_timer(void *instance_ptr, elite_msg_any_t *data_ptr);
static ADSPResult vtm_custom_msg( void* instance_ptr, elite_msg_any_t* msg_ptr);

// Helper functions
ADSPResult vtm_subscribe_afe(vtm_t *vtm_ptr, uint8_t vfr_mode, uint32_t vsid, uint8_t timing_ver);
ADSPResult vtm_unsubscribe_afe(vtm_t *vtm_ptr, uint8_t vfr_mode, uint32_t vsid, uint8_t timing_ver);
/* -----------------------------------------------------------------------
 ** Message handler
 ** ----------------------------------------------------------------------- */

// Function handler for channel
static ADSPResult (*vtm_handler_ptr[])(void *) =
{
   vtm_soft_vfr_handler,
   vtm_cmd_handler,
   vtm_test_signal_handler
};

// Function handler for vt cmd msgs
static elite_svc_msg_handler_func vtm_cmd_handler_ptr[VOICE_TIMER_MSGS_END] =
{
   vtm_subscribe_msg,
   vtm_unsubscribe_msg,
   vtm_hard_vfr_msg,     // TODO: remove hard vfr msg out of cmd handler so that vfr has higher priority than cmds
   vtm_start_oneshot_timer
};

// Function handler for FADD msgs
static elite_svc_msg_handler_func elite_handler_ptr[] =
{
   vtm_custom_msg,                //0  - ELITE_CUSTOM_MSG
   elite_svc_unsupported,          //1  - ELITE_CMD_START_SERVICE
   elite_svc_unsupported,          //2  - ELITE_CMD_DESTROY_SERVICE
   elite_svc_unsupported,          //3  - ELITE_CMD_CONNECT
   elite_svc_unsupported,          //4  - ELITE_CMD_DISCONNECT
   elite_svc_unsupported,          //5  - ELITE_CMD_PAUSE
   elite_svc_unsupported,          //6  - ELITE_CMD_RESUME
   elite_svc_unsupported,          //7  - ELITE_CMD_FLUSH
   elite_svc_unsupported,          //8  - ELITE_CMD_SET_PARAM
   elite_svc_unsupported,          //9  - ELITE_CMD_GET_PARAM
   elite_svc_unsupported,          //10 - ELITE_DATA_BUFFER
   elite_svc_unsupported,          //11 - ELITE_DATA_MEDIA_TYPE
   elite_svc_unsupported,          //12 - ELITE_DATA_EOS
   elite_svc_unsupported,          //13 - ELITE_DATA_RAW_BUFFER
   elite_svc_unsupported,          //14 - ELITE_CMD_STOP_SERVICE
   elite_svc_unsupported           //15 - ELITE_APR_PACKET_OPCODE
};

/* =======================================================================
 **                          Function Definitions
 ** ======================================================================= */

ADSPResult vtm_create (uint32_t input_param, void **handle)
{
   ADSPResult result;
   qurt_elite_queue_t *temp_ptr;

   // allocate instance struct
   vtm_t *vtm_ptr = (vtm_t*) qurt_elite_memory_malloc( sizeof(vtm_t), QURT_ELITE_HEAP_DEFAULT);
   if (!vtm_ptr)
   {
       MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Failed to allocate memory for VTM struct !!");
       return ADSP_ENOMEMORY;
   }

   // zero out all the fields.
   memset(vtm_ptr, 0, sizeof(vtm_t));

   // initialize instance variables
   vtm_ptr->wait_mask = VTM_ALL_MASKS;

   // initialize channel
   qurt_elite_channel_init(&(vtm_ptr->timer_channel));

   // Create the queues and signals and add to channel
   //qurt_elite_globalstate.pVoiceTimerCmdQ = (qurt_elite_queue_t *) vtm_cmd_q_buf;
   temp_ptr = (qurt_elite_queue_t *) vtm_cmd_q_buf;
   if (ADSP_FAILED(result = qurt_elite_queue_init(vtm_cmdq_name, MAX_CMD_Q_ELEMENTS, temp_ptr)) ||
         ADSP_FAILED(result = qurt_elite_channel_addq(&vtm_ptr->timer_channel, temp_ptr, VTM_CMD_MASK)) ||
         ADSP_FAILED(result = qurt_elite_channel_add_signal(&vtm_ptr->timer_channel, &vtm_ptr->soft_vfr_signal, VTM_SOFT_VFR_MASK)) ||
         ADSP_FAILED(result = qurt_elite_channel_add_signal(&vtm_ptr->timer_channel, &vtm_ptr->test_signal, VTM_TEST_SIGNAL_MASK)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Queue or Signal creation failed with result %d!", (int)result);
   }
   qurt_elite_globalstate.pVoiceTimerCmdQ = temp_ptr;
   vtm_ptr->cmdq_ptr = qurt_elite_globalstate.pVoiceTimerCmdQ;

   // Thread priority setting
   uint32_t VTM_THREAD_PRIO = elite_get_thrd_prio(ELITETHREAD_STAT_VOICE_TIMER_SVC_PRIO_ID);

   // Launch the timer thread
   if (ADSP_FAILED(result = qurt_elite_thread_launch(&(vtm_ptr->timer_thread), VTM_THREAD_NAME,
               NULL, VTM_THREAD_STACK_SIZE, VTM_THREAD_PRIO, vtm_thread, (void*)vtm_ptr,QURT_ELITE_HEAP_DEFAULT)))
   {
      qurt_elite_memory_free (vtm_ptr);
      MSG(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Failed create Vt thread");
      return result;
   }
   //create VDS instance
   result = vds_create(&vds_handle, 1);
   if(ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: VDS creation failed with result %d!", (int)result);
      return result;
   }
#if defined(VTM_ONESHOT_DEBUG)
   if (ADSP_FAILED(result = avtimer_drv_create(&vtm_ptr->test_timer,QURT_ELITE_TIMER_ONESHOT_DURATION, &vtm_ptr->test_signal)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: timer create for one shot re-arming failed, result(%d)",result);
      return result;
   }
#endif

   *handle = vtm_ptr;
   return ADSP_EOK;
}

static ADSPResult vtm_thread(void* instance_ptr)
{
   ADSPResult result = ADSP_EOK;
   vtm_t *vtm_ptr = (vtm_t *)instance_ptr;

   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Entering Voice Timer workloop...");

   // Enter forever loop
   for(;;)
   {
      // block on any one or more of selected queues to get a msg
      vtm_ptr->rcvd_mask = qurt_elite_channel_wait(&vtm_ptr->timer_channel, vtm_ptr->wait_mask);

      while(vtm_ptr->rcvd_mask)
      {
         vtm_ptr->bit_pos = voice_get_signal_pos(vtm_ptr->rcvd_mask);

         // De queue and signal clear done in the handler functions.
         result = vtm_handler_ptr[vtm_ptr->bit_pos](vtm_ptr);

         if(result != ADSP_EOK)
         {
            MSG_2(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Vt thread handler error bit_pos=%ld, res=%d",vtm_ptr->bit_pos, result );
         }

         vtm_ptr->rcvd_mask = qurt_elite_channel_poll(&vtm_ptr->timer_channel, vtm_ptr->wait_mask);
      }
   }

   return 0;
}

static ADSPResult vtm_start_oneshot_timer(void *instance_ptr, elite_msg_any_t *data_ptr)
{
   ADSPResult result = ADSP_EOK;
   vtm_t *vtm_ptr =  (vtm_t *)instance_ptr;
   EliteMsg_CustomVtSubUnsubHeaderType *sub_unsub_data_ptr;
   Vtm_SubUnsubMsg_t *data_sub_ptr;
   uint8_t vfr_id,idx;
   int32_t i;
   uint64_t abs_time;

   sub_unsub_data_ptr = (EliteMsg_CustomVtSubUnsubHeaderType *)(data_ptr->pPayload);
   data_sub_ptr = sub_unsub_data_ptr->vtm_sub_unsub_payload_ptr;
   vfr_id = voice_get_vfrid_from_vsid(data_sub_ptr->vsid); //get vfr_id (0 to 15) from VSID

   MSG_6(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vtm_start_oneshot_timer: client(%ld), vfr_mode(%d), vsid(%lx), vfr_id(%d), version(%d), offset(%d)",
         data_sub_ptr->client_id,(int16_t)data_sub_ptr->vfr_mode,data_sub_ptr->vsid,(int16_t)vfr_id,(int16_t)data_sub_ptr->timing_ver,data_sub_ptr->offset);

   //using timing version and vfr_mode/vsid, derive the index into the array which points to the right vfr source context
   if (((VFR_CLIENT_INFO_VER_1 == data_sub_ptr->timing_ver) && (VFR_MODE_END > data_sub_ptr->vfr_mode)) ||
       (VFR_NONE == data_sub_ptr->vfr_mode)) //legacy timing without VSID OR VFR_NONE case
   {
      idx = data_sub_ptr->vfr_mode; //VFR_NONE, VFR_HARD and VFR_HARD_EXT are supported only
   }
   else if((VFR_CLIENT_INFO_VER_2 == data_sub_ptr->timing_ver) &&
           (VFR_HARD == data_sub_ptr->vfr_mode) &&
           (0 != data_sub_ptr->vsid) &&
           ((NUM_VFR_SOURCES-VFR_MODE_END) > vfr_id)) //timing with VSID
   {
      idx = vfr_id + VFR_MODE_END; //possible vfr sources in VSID are placed after legacy vfr sources in the array
   }
   else
   {
      //return msg buffer
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vtm_start_oneshot_timer: Invalid timing version and/or invalid vfr mode");
      elite_msg_return_payload_buffer(data_ptr);
      return ADSP_EUNSUPPORTED;
   }
   uint32_t mask = vtm_ptr->vfr_source_context[idx].sub_mask;

   Vtm_SubUnsubMsg_t   **client_subscription_ptr = &(vtm_ptr->vfr_source_context[idx].client_subscription_ptr[0]);
   qurt_elite_timer_t  **oneshot_timer_ptr = &(vtm_ptr->vfr_source_context[idx].oneshot_timer_ptr[0]);
   for (i = 0; i < VOICE_MAX_TIMERS_PER_VFR_SOURCE; i++)
   {
      if(voice_tst_bit(mask, i) && (client_subscription_ptr[i] == data_sub_ptr))
      {
         break;
      }
   }
   if ( i != VOICE_MAX_TIMERS_PER_VFR_SOURCE )
   {
	   abs_time = vtm_ptr->vfr_source_context[idx].vfr_time_stamp + (uint64_t)client_subscription_ptr[i]->offset;
	   uint64_t current_time = avtimer_drv_get_time( vtm_ptr->avt_drv );
       while ( abs_time <= current_time)
       abs_time+=20000;
   }
   else
   {
	   MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vtm_start_oneshot_timer: one shot timer index not available");
	   return ADSP_EFAILED;
   }

   if (ADSP_FAILED(result = avtimer_drv_start_absolute(oneshot_timer_ptr[i], abs_time)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vtm_start_oneshot_timer: one shot timer re-arming failed, result(%d)",result);
   }
   // return msg buffer
   elite_msg_return_payload_buffer(data_ptr);

   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vtm_start_oneshot_timer end, result=%d",result);

   return result;

}
// This handler will be called when VFR SOURCE is VFR_NONE only
static ADSPResult vtm_soft_vfr_handler(void *instance_ptr)
{
   vtm_t *vtm_ptr = (vtm_t*)instance_ptr;
   ADSPResult result = ADSP_EOK;
   uint32_t mask = vtm_ptr->vfr_source_context[VFR_NONE].sub_mask; //subscription mask corresponding to VFR_NONE context
   Vtm_SubUnsubMsg_t   **client_subscription_ptr = &(vtm_ptr->vfr_source_context[VFR_NONE].client_subscription_ptr[0]);
   qurt_elite_timer_t  **oneshot_timer_ptr = &(vtm_ptr->vfr_source_context[VFR_NONE].oneshot_timer_ptr[0]);
   //dbg: MSG(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vtm_soft_vfr_handler begin");

   qurt_elite_signal_clear(&vtm_ptr->soft_vfr_signal);

   // store the time of VFR
   vtm_ptr->vfr_source_context[VFR_NONE].vfr_time_stamp = avtimer_drv_get_time( vtm_ptr->avt_drv );

   for (int32_t i = 0; i < VOICE_MAX_TIMERS_PER_VFR_SOURCE; i++)
   {
      if(voice_tst_bit(mask, i))
      {
          if (VFR_NONE == client_subscription_ptr[i]->vfr_mode)
          {
            client_subscription_ptr[i]->avtimer_timestamp_us = vtm_ptr->vfr_source_context[VFR_NONE].vfr_time_stamp;
                //MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vtm_soft_vfr_handler: avtimer_timestamp_us=%llu, client(%d)", (unsigned long long int)client_subscription_ptr[i]->avtimer_timestamp_us,(int)client_subscription_ptr[i]->client_id);

            if (client_subscription_ptr[i]->signal_enable)
            {
                if (ADSP_FAILED(result = avtimer_drv_start_absolute(oneshot_timer_ptr[i], client_subscription_ptr[i]->avtimer_timestamp_us + (uint64_t)client_subscription_ptr[i]->offset)))
                {
                   MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vtm_soft_vfr_handler: one shot timer re-arming failed, result(%d)",result);
                }
            }
         }
         mask = voice_toggle_bit(mask, i);
      }

      if (mask == 0)
      {
         break;
      }
   }
   //dbg: MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vtm_soft_vfr_handler end, result = %d",result);
   return result;
}

static ADSPResult vtm_cmd_handler(void *instance_ptr)
{
   ADSPResult result = ADSP_EOK;
   vtm_t *vtm_ptr = (vtm_t*)instance_ptr;
   elite_msg_any_t msg;

   //dbg: MSG(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vtm_cmd_handler begin");

   // Take next msg off the cmdQ
   result = qurt_elite_queue_pop_front(vtm_ptr->cmdq_ptr, (uint64_t*)&msg );
   // Process the msg if success
   if (ADSP_EOK == result)
   {
      const uint32_t cmd_table_size = sizeof(elite_handler_ptr)/sizeof(elite_handler_ptr[0]);
      if (msg.unOpCode >= cmd_table_size)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Unsupported message ID 0x%8lx!!", msg.unOpCode);
         return ADSP_EUNSUPPORTED;
      }
      //dbg: MSG_1(MSG_SSID_QDSP6, DBG_LOW_PRIO,"VCP: Vt FADD cmd handler, Opcode=%d",msg.unOpCode);
      result = elite_handler_ptr[msg.unOpCode](instance_ptr, &msg);
   }
   //dbg: MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vtm_cmd_handler end, result = %d",result);
   return result;
}

static ADSPResult vtm_custom_msg( void* instance_ptr, elite_msg_any_t* msg_ptr)
{
   ADSPResult result = ADSP_EOK;
   elite_msg_custom_header_t *custom_ptr = (elite_msg_custom_header_t *) msg_ptr->pPayload;

   if( custom_ptr->unSecOpCode >= VOICE_TIMER_MSGS_END)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Unsupported custom message ID 0x%8lx!!", custom_ptr->unSecOpCode);
      return ADSP_EUNSUPPORTED;
   }
   //dbg: MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: Vt custom cmd handler, Opcode=%d",custom_ptr->unSecOpCode );
   result = vtm_cmd_handler_ptr[custom_ptr->unSecOpCode](instance_ptr, msg_ptr);
   return result;
}

static ADSPResult vtm_subscribe_msg(void *instance_ptr, elite_msg_any_t *data_ptr)
{
   ADSPResult result = ADSP_EOK;
   vtm_t *vtm_ptr =  (vtm_t *)instance_ptr;
   EliteMsg_CustomVtSubUnsubHeaderType *sub_unsub_data_ptr;
   Vtm_SubUnsubMsg_t *data_sub_ptr, **client_subscription_ptr;
   int32_t free_index = -1, free_index_oneshot_timer = -1, *client_cnt_ptr;
   uint32_t *mask_ptr, sum_mask = 0;
   int16_t i = 0;
   uint8_t idx;
   qurt_elite_timer_t **oneshot_timer_ptr;
   uint8_t vfr_id;

   // sum all the masks, if this is first subscription by any client then sum will be zero
   // then register for AV timer resources
   for (i=0; i<NUM_VFR_SOURCES; i++)
   {
      sum_mask |= vtm_ptr->vfr_source_context[i].sub_mask;
   }
   if (0 == sum_mask)
   {
      avtimer_open_param_t open_param;
      open_param.client_name = (char*)"VTM";
      open_param.flag = 0;
      if (ADSP_EOK != (result = avtimer_drv_hw_open(&(vtm_ptr->avt_drv), &open_param)))
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Failed to Open AVTimer Driver for VTM");
         return ADSP_EFAILED;
      }
      MSG(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: AV Timer registration done for VTM");
   }

   sub_unsub_data_ptr = (EliteMsg_CustomVtSubUnsubHeaderType *)(data_ptr->pPayload);
   data_sub_ptr = sub_unsub_data_ptr->vtm_sub_unsub_payload_ptr;
   vfr_id = voice_get_vfrid_from_vsid(data_sub_ptr->vsid); //get vfr_id (0 to 15) from VSID

   MSG_7(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vtm_subscribe_msg: client(%ld), vfr_mode(%d), vsid(%lx), vfr_id(%d), version(%d), offset(%d), vfr_cycle(%lx)",
         data_sub_ptr->client_id,(int16_t)data_sub_ptr->vfr_mode,data_sub_ptr->vsid,(int16_t)vfr_id,(int16_t)data_sub_ptr->timing_ver,data_sub_ptr->offset,data_sub_ptr->vfr_cycle);

   //using timing version and vfr_mode/vsid, derive the index into the array which points to the right vfr source context
   if (((VFR_CLIENT_INFO_VER_1 == data_sub_ptr->timing_ver) && (VFR_MODE_END > data_sub_ptr->vfr_mode)) ||
       (VFR_NONE == data_sub_ptr->vfr_mode)) //legacy timing without VSID OR VFR_NONE case
   {
      idx = data_sub_ptr->vfr_mode; //VFR_NONE, VFR_HARD and VFR_HARD_EXT are supported only
   }
   else if((VFR_CLIENT_INFO_VER_2 == data_sub_ptr->timing_ver) &&
           (VFR_HARD == data_sub_ptr->vfr_mode) &&
           (0 != data_sub_ptr->vsid) &&
           ((NUM_VFR_SOURCES-VFR_MODE_END) > vfr_id)) //timing with VSID
   {
      idx = vfr_id + VFR_MODE_END; //possible vfr sources in VSID are placed after legacy vfr sources in the array
   }
   else
   {
      //return msg buffer
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vtm_subscribe_msg: Invalid timing version and/or invalid vfr mode");
      elite_msg_return_payload_buffer(data_ptr);
      return ADSP_EUNSUPPORTED;
   }

   mask_ptr = &(vtm_ptr->vfr_source_context[idx].sub_mask);            // ptr to sub_mask of that vfr source
   client_cnt_ptr = &(vtm_ptr->vfr_source_context[idx].client_cnt);    // ptr to client counter of that vfr source

   // Check for the first zero'th bit
   // voice_tst_bit checks for 1, hence ~vtm_ptr->sub_mask sent it.
   while(voice_tst_bit(*mask_ptr, ++free_index));

   client_subscription_ptr = &(vtm_ptr->vfr_source_context[idx].client_subscription_ptr[free_index]); // ptr to store ptr pointing to client payload struct
   oneshot_timer_ptr = &(vtm_ptr->vfr_source_context[idx].oneshot_timer_ptr[free_index]);             // ptr to store ptr pointing to one-shot timer struct

   // Grab one-shot timer from a common pool
   while(voice_tst_bit(vtm_ptr->oneshot_timer_mask, ++free_index_oneshot_timer));

   if ((VOICE_MAX_TIMERS_TOTAL > free_index_oneshot_timer) && (*client_cnt_ptr < VOICE_MAX_TIMERS_PER_VFR_SOURCE))
   {
      *mask_ptr = voice_set_bit(*mask_ptr, free_index);
      *client_subscription_ptr = data_sub_ptr;
      *client_cnt_ptr += 1; // increment client count for that vfr_source
      *oneshot_timer_ptr = &(vtm_ptr->oneshot_timer_struct[free_index_oneshot_timer]);
      vtm_ptr->oneshot_timer_mask = voice_set_bit(vtm_ptr->oneshot_timer_mask, free_index_oneshot_timer);

      // Create a timer for one shot re-arming to signal clients
      //PGtodo: pre-allocating timer struct for all vfr sources and 32 clients is an over-kill, do dynamic malloc and free
      if (ADSP_FAILED(result = avtimer_drv_create(*oneshot_timer_ptr,QURT_ELITE_TIMER_ONESHOT_ABSOLUTE,
                  data_sub_ptr->signal_ptr)))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: vtm_subscribe_msg: timer create for one shot re-arming failed, result(%d)",result);
      }

      if (1 == *client_cnt_ptr)
      {
         if (VFR_NONE == data_sub_ptr->vfr_mode) //this takes care of VFR_NONE case in both versions of timing
         {
            MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vtm_subscribe_msg: creating timer for soft vfr");
            if (ADSP_FAILED(result = avtimer_drv_create(&vtm_ptr->soft_vfr_timer,QURT_ELITE_TIMER_PERIODIC,
                        &vtm_ptr->soft_vfr_signal)))
            {
               MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: vtm_subscribe_msg: soft vfr periodic timer create failed, result(%d)",result);
            }
            qurt_elite_signal_send(&vtm_ptr->soft_vfr_signal);
            // triggering sof_vfr_handler immediately so that ticks start to receive in the first frame itself

            // Start a periodic timer which triggers vtm_ptr->soft_vfr_signal signal every 20ms
            MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vtm_subscribe_msg: Timer started, ptr %x", (int)&vtm_ptr->soft_vfr_timer);
            if (ADSP_FAILED(result = avtimer_drv_start_periodic(&vtm_ptr->soft_vfr_timer,data_sub_ptr->vfr_cycle)))
            {
               MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: vtm_subscribe_msg: start periodic timer failed, result(%d)",result);
            }

         }
         else // all vfr sources other than VFR_NONE
         {
            if (NULL == vtm_ptr->afe_cmdq_ptr)
            {
               // Obtain AFE cmdQ
               if (qurt_elite_globalstate.pAfeStatSvcCmdQ == NULL)
               {
                  MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vtm_subscribe_msg: Failed to get Afe cmdQ session");
                  elite_msg_return_payload_buffer(data_ptr);
                  return ADSP_EUNSUPPORTED;
               }
               vtm_ptr->afe_cmdq_ptr = qurt_elite_globalstate.pAfeStatSvcCmdQ;
            }

            {
               // subscribe to AFE for hard vfr
               result = vtm_subscribe_afe(vtm_ptr,data_sub_ptr->vfr_mode,data_sub_ptr->vsid,data_sub_ptr->timing_ver);
               if (ADSP_FAILED(result))
               {
                  MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: VoiceTimer failed to subscribe to AFE");
               }
            }
         }
         //Clear VFR jitter structure variables
         memset(&vtm_ptr->vfr_jitter_info, 0, sizeof(vfr_jitter_info_t));
      }
      else
      {
          // this part is valid for both soft and hard vfr
          uint64_t client_subscription_time= avtimer_drv_get_time( vtm_ptr->avt_drv );
          uint64_t vfr_time = vtm_ptr->vfr_source_context[idx].vfr_time_stamp;
          uint64_t absolute_offset_time;

          absolute_offset_time = vfr_time  + (*client_subscription_ptr)->offset;

          if (vfr_time)
          {
              // check if client tick is already elapsed in real time
              if ((*client_subscription_ptr)->signal_enable)
              {
                  if ((absolute_offset_time-100) > client_subscription_time)  // the offset time is not yet elapsed, 100 is guard time
                  {
                      if (ADSP_FAILED(result = avtimer_drv_start_absolute(*oneshot_timer_ptr, absolute_offset_time)))
                      {
                          MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vtm_subscribe_msg: one shot timer re-arming failed, result(%d)",result);
                      }
                  }
                  else
                  {
                      MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vtm_subscribe_msg: the offset for client (%d) elapsed, vfr time (%llu) subscription (%llu), offset_time (%llu),fire tick after next vfr",
                              data_sub_ptr->client_id,vfr_time,client_subscription_time,absolute_offset_time);

                  }
              }
          }
          else
          {
              // vfr time stamp 0 implies VFR has not come yet
              MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: First VFR for source (%ld) has not come yet",idx);
          }


      }
   }
   else
   {
      MSG(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Max Timers exceeded");
      result = ADSP_EFAILED;
   }

   // set signal for client to come out of synchronous wait
   if (VFR_NONE == data_sub_ptr->vfr_mode)
   {
      qurt_elite_signal_send(data_sub_ptr->signal_end_ptr);
   }
   // return msg buffer
   elite_msg_return_payload_buffer(data_ptr);

   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vtm_subscribe_msg end, result=%d",result);

   return result;
}

static ADSPResult vtm_unsubscribe_msg(void *instance_ptr, elite_msg_any_t *data_ptr)
{
   ADSPResult result = ADSP_EOK;
   uint8_t idx;
   uint32_t *mask_ptr,sum_mask=0;
   int32_t *client_cnt_ptr;
   qurt_elite_timer_t **oneshot_timer_ptr;
   EliteMsg_CustomVtSubUnsubHeaderType *sub_unsub_data_ptr;
   Vtm_SubUnsubMsg_t *data_unsub_ptr, **client_subscription_ptr;
   vtm_t *vtm_ptr =  (vtm_t *)instance_ptr;
   uint8_t vfr_id;

   sub_unsub_data_ptr = (EliteMsg_CustomVtSubUnsubHeaderType *)(data_ptr->pPayload);
   data_unsub_ptr = sub_unsub_data_ptr->vtm_sub_unsub_payload_ptr;
   vfr_id = voice_get_vfrid_from_vsid(data_unsub_ptr->vsid);
   MSG_4(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vtm_unsubscribe_msg, client(%ld): vfr_mode(%d), vsid(%lx), version(%d)",
         data_unsub_ptr->client_id,(int16_t)data_unsub_ptr->vfr_mode,data_unsub_ptr->vsid,(int16_t)data_unsub_ptr->timing_ver);

   //using timing version and vfr_mode/vsid, derive the index into the array which points to the right vfr source context
   if (((VFR_CLIENT_INFO_VER_1 == data_unsub_ptr->timing_ver) && (VFR_MODE_END > data_unsub_ptr->vfr_mode)) ||
       (VFR_NONE == data_unsub_ptr->vfr_mode)) //legacy timing without VSID OR VFR_NONE case
   {
      idx = data_unsub_ptr->vfr_mode;  //VFR_NONE, VFR_HARD and VFR_HARD_EXT are supported only
   }
   else if((VFR_CLIENT_INFO_VER_2 == data_unsub_ptr->timing_ver) &&
           (VFR_HARD == data_unsub_ptr->vfr_mode) &&
           (0 != data_unsub_ptr->vsid) &&
           ((NUM_VFR_SOURCES-VFR_MODE_END) > vfr_id)) //timing with VSID
   {
      idx = vfr_id + VFR_MODE_END; //possible vfr sources in VSID are placed after legacy vfr sources in the array
   }
   else
   {
      //return msg buffer
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vtm_unsubscribe_msg: Invalid timing version and/or invalid vfr mode");
      elite_msg_return_payload_buffer(data_ptr);
      return ADSP_EUNSUPPORTED;
   }

   mask_ptr = &(vtm_ptr->vfr_source_context[idx].sub_mask);            // ptr to sub_mask of that vfr source
   client_cnt_ptr = &(vtm_ptr->vfr_source_context[idx].client_cnt);    // ptr to client counter of that vfr source
   client_subscription_ptr = &(vtm_ptr->vfr_source_context[idx].client_subscription_ptr[0]); // ptr to ptr pointing to first client payload struct
   oneshot_timer_ptr = &(vtm_ptr->vfr_source_context[idx].oneshot_timer_ptr[0]);             // ptr to ptr pointing to first client oneshot timer struct



   for (int32_t i = 0; i < VOICE_MAX_TIMERS_PER_VFR_SOURCE; i++)
   {
      if(voice_tst_bit(*mask_ptr, i) && (client_subscription_ptr[i] == data_unsub_ptr))
      {
         /* Send Ack */
         qurt_elite_signal_send(client_subscription_ptr[i]->signal_end_ptr);

         // clear the mask for this timer
         *mask_ptr = voice_toggle_bit(*mask_ptr, i);

         client_subscription_ptr[i] = NULL;
         *client_cnt_ptr -= 1; //decrement counter tracking number of clients

         // Delete one shot timer for re-arming signal to client
         if (ADSP_FAILED(result = avtimer_drv_delete(oneshot_timer_ptr[i])))
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vtm_unsubscribe_msg: one shot timer delete failed, result(%d)",result);
         }

         // Free up the one-shot timer instance for it to be available in the common pool for other clients
         for (int32_t j = 0; j < VOICE_MAX_TIMERS_TOTAL; j++)
         {
            if(voice_tst_bit(vtm_ptr->oneshot_timer_mask, j) && (&(vtm_ptr->oneshot_timer_struct[j]) == oneshot_timer_ptr[i]))
            {
               vtm_ptr->oneshot_timer_mask = voice_toggle_bit(vtm_ptr->oneshot_timer_mask, j);
            }
         }

         if (*client_cnt_ptr == 0)
         {
            if (VFR_NONE == data_unsub_ptr->vfr_mode)
            {
               // Delete periodic timer
               MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vtm_unsubscribe_msg: deleting timer for soft vfr");
               if (ADSP_FAILED(result = avtimer_drv_delete(&vtm_ptr->soft_vfr_timer)))
               {
                  MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vtm_unsubscribe_msg: periodic timer delete failed, result(%d)",result);
               }
            }
            else
            {
               // dbg: MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vtm_unsubscribe_msg: VTM unsubscribing for hard vfr");
               // unsubscribe to AFE for hard vfr
               if (NULL != vtm_ptr->afe_cmdq_ptr)
               {
                  result = vtm_unsubscribe_afe(vtm_ptr, data_unsub_ptr->vfr_mode,data_unsub_ptr->vsid,data_unsub_ptr->timing_ver);
                  if (ADSP_FAILED(result))
                  {
                     MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vtm_unsubscribe_msg: VoiceTimer failed to unsubscribe to AFE");
                  }
               }
               else
               {
                  MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vtm_unsubscribe_msg: AFE cmdQ in NULL");
               }
            }
            //Clear VFR jitter structure variables
            memset(&vtm_ptr->vfr_jitter_info, 0, sizeof(vfr_jitter_info_t));
            vtm_ptr->vfr_source_context[idx].vfr_time_stamp = 0;
         }
         /* Since only one data_unsub_ptr can match, break from loop */
         break;
      }
   }

   //sum all masks
   for (int32_t i=0; i<NUM_VFR_SOURCES; i++)
   {
      sum_mask |= vtm_ptr->vfr_source_context[i].sub_mask;
   }

   if (0 == sum_mask)
   {
      MSG(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vtm_unsubscribe_msg: VTM Deregistering from AV Timer");
      avtimer_drv_hw_close(vtm_ptr->avt_drv);
   }

   //return msg buffer
   elite_msg_return_payload_buffer(data_ptr);

   MSG_1(MSG_SSID_QDSP6, DBG_LOW_PRIO,"VCP: vtm_unsubscribe_msg end, result=%d",result);

   return result;
}


static ADSPResult vtm_hard_vfr_msg(void *instance_ptr, elite_msg_any_t *data_ptr)
{
   ADSPResult result = ADSP_EOK;
   elite_msg_custom_vfr_info_t *vfr_payload_ptr;
   vfr_info_t *vfr_data_ptr;
   vtm_t *vtm_ptr = (vtm_t*)instance_ptr;
   uint8_t timing_ver,idx,vfr_id;
   uint32_t vfr_source;
   int32_t mask;
   uint64_t corrected_absolute_offset = 0;
   //dbg: MSG(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vtm_hard_vfr_msg begin");

   vfr_payload_ptr = (elite_msg_custom_vfr_info_t *)(data_ptr->pPayload);
   vfr_data_ptr = &(vfr_payload_ptr->vfr_info);
   timing_ver = vfr_data_ptr->vfr_client_info_ver;
   vfr_source = vfr_data_ptr->vfr_source;
   vfr_id = voice_get_vfrid_from_vsid(vfr_source);

   //using timing version and vfr_mode/vsid, derive the index into the array which points to the right vfr source context
   if ((VFR_CLIENT_INFO_VER_1 == timing_ver) && ((AFE_VFR_SOURCE_INTERNAL == vfr_source) || (AFE_VFR_SOURCE_EXTERNAL == vfr_source)))
   {
      idx = VFR_HARD;
      if (AFE_VFR_SOURCE_EXTERNAL == vfr_source)
      {
         idx = VFR_HARD_EXT;
      }

   }
   else if ((VFR_CLIENT_INFO_VER_2 == timing_ver) && ((NUM_VFR_SOURCES-VFR_MODE_END) > vfr_id))
   {
      idx = vfr_id + VFR_MODE_END;
   }
   else
   {
      //return msg buffer
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vtm_hard_vfr_msg: Invalid timing version and/or invalid vfr mode");
      elite_msg_return_payload_buffer(data_ptr);
      return ADSP_EUNSUPPORTED;
   }

   mask = vtm_ptr->vfr_source_context[idx].sub_mask;

   Vtm_SubUnsubMsg_t   **client_subscription_ptr = &(vtm_ptr->vfr_source_context[idx].client_subscription_ptr[0]);
   qurt_elite_timer_t  **oneshot_timer_ptr = &(vtm_ptr->vfr_source_context[idx].oneshot_timer_ptr[0]);

#if defined(VTM_ONESHOT_DEBUG)
   if(vtm_ptr->first_hard_vfr_rcvd)
   {
      //check for signal being cleared and set it again
      if(vtm_ptr->test_flag)
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vtm_hard_vfr_msg: Error! VTM hard vfr found flag uncleared!");
      }
      else
      {
         vtm_ptr->test_flag = TRUE;
      }
   }
   else
   {
      vtm_ptr->first_hard_vfr_rcvd = TRUE;
      vtm_ptr->test_flag = TRUE;
   }

   // rearm test timer
   if (ADSP_FAILED(result = avtimer_drv_start_duration(&vtm_ptr->test_timer, TEST_TIMER_DURATION)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vtm_hard_vfr_msg: one shot timer re-arming failed, result(%d)",result);
   }
#endif

   for (int32_t i = 0; i < VOICE_MAX_TIMERS_PER_VFR_SOURCE; i++)
   {
      if(voice_tst_bit(mask, i))
      {
         if (client_subscription_ptr[i]->signal_enable)
         {
            /*if (  ((VFR_CLIENT_INFO_VER_1 == timing_ver) && (vfr_source == client_subscription_ptr[i]->vfr_mode)) ||
                  ((VFR_CLIENT_INFO_VER_2 == timing_ver) && (vfr_source == client_subscription_ptr[i]->vsid)))
               )*/ //this check is not needed because index into context is derived using timing_ver and vfr_source
            {
               // Signal resync
               if(vfr_data_ptr->resync_status && (NULL != client_subscription_ptr[i]->resync_signal_ptr))
               {
                  qurt_elite_signal_send(client_subscription_ptr[i]->resync_signal_ptr);
               }
               // VFR jitter correction
               corrected_absolute_offset = vfr_payload_ptr->vfr_info.vfr_av_timestamp_us + client_subscription_ptr[i]->offset;

               if (ADSP_FAILED(result = avtimer_drv_start_absolute(oneshot_timer_ptr[i], corrected_absolute_offset)))
               {
                  MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vtm_hard_vfr_msg: one shot timer re-arming failed, result(%d)",result);
               }
            }
         }
         mask = voice_toggle_bit(mask, i);
      }

      if (mask == 0)
      {
         break;
      }
   }


   //save vfr time stamp of idx  source
   vtm_ptr->vfr_source_context[idx].vfr_time_stamp = vfr_payload_ptr->vfr_info.vfr_av_timestamp_us;

   //VFR jitter detection
   vtm_ptr->vfr_jitter_info.vfr_afe_av_timestamp_us = vfr_payload_ptr->vfr_info.vfr_av_timestamp_us;
   vtm_ptr->vfr_jitter_info.vfr_vtm_av_timestamp_us = avtimer_drv_get_time( vtm_ptr->avt_drv );

   vtm_ptr->vfr_jitter_info.vfr_curr_jitter = (uint32_t)(vtm_ptr->vfr_jitter_info.vfr_vtm_av_timestamp_us - vtm_ptr->vfr_jitter_info.vfr_afe_av_timestamp_us);

   if (vtm_ptr->vfr_jitter_info.vfr_curr_jitter > vtm_ptr->vfr_jitter_info.vfr_max_jitter)
   {
      vtm_ptr->vfr_jitter_info.vfr_max_jitter = vtm_ptr->vfr_jitter_info.vfr_curr_jitter;
   }

   MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: vtm_hard_vfr_msg: version(%d), vfr_source(%lx), vfr_jitter_us(%ld), max_vfr_jitter(%ld)",
                    timing_ver,vfr_source, vtm_ptr->vfr_jitter_info.vfr_curr_jitter, vtm_ptr->vfr_jitter_info.vfr_max_jitter);

   // return msg buffer
   elite_msg_return_payload_buffer(data_ptr);

   //dbg: MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vtm_hard_vfr_msg end, result=%d",result);

   return result;
}

ADSPResult vtm_subscribe_afe(vtm_t *vtm_ptr, uint8_t vfr_mode, uint32_t vsid, uint8_t timing_ver)
{
   ADSPResult result = ADSP_EOK;
   uint32_t size = 0;
   elite_msg_any_t sMsg;
   elite_msg_custom_afe_vfr_config_info_t *payload_ptr;
   uint32_t vfr_source; // source of hard vfr
   MSG_3(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vtm_subscribe_afe begin, vfr_mode(%d), vsid(%lx), version(%d)",vfr_mode,vsid,(int16_t)timing_ver);

   if (VFR_CLIENT_INFO_VER_1 == timing_ver)
   {
      vfr_source = AFE_VFR_SOURCE_INTERNAL; // default is hard vfr from internal source
      if (VFR_HARD_EXT == vfr_mode)
      {
         vfr_source = AFE_VFR_SOURCE_EXTERNAL;  // hard vfr from external source
      }
   }
   else //VFR_CLIENT_INFO_VER_2
   {
      vfr_source = vsid;
   }
   size = sizeof(elite_msg_custom_afe_vfr_config_info_t);
   result = elite_msg_create_msg( &sMsg,
         &size,
         ELITE_CUSTOM_MSG,
         NULL,
         NULL,
         ADSP_EOK);
   if(ADSP_FAILED(result))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: FAILED to create EliteMsg!! Not Sending Msg!!");
      // added return code to avoid crash incase of buffer run-out
      return result;
   }
   payload_ptr = (elite_msg_custom_afe_vfr_config_info_t *) sMsg.pPayload;
   payload_ptr->sec_op_code = ELITEMSG_CUSTOM_VFR_SYNC;

   /* Default VFR client interface version */
   payload_ptr->client_vfr_info.vfr_client_info_ver = timing_ver;

   payload_ptr->client_vfr_info.vfr_clientq = vtm_ptr->cmdq_ptr; // Vtm cmd Q
   payload_ptr->client_vfr_info.client_msg_id = VOICE_TIMER_HARD_VFR; // Vtm hard vfr msg id
   payload_ptr->client_vfr_info.vfr_source = vfr_source;

   result = qurt_elite_queue_push_back(vtm_ptr->afe_cmdq_ptr, (uint64_t*)&sMsg);//hack: fix this
   if (ADSP_FAILED(result))
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Failed to send subscribe msg to AFE");
      return result;
   }

   MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vtm_subscribe_afe end, vfr_source=%lx, result = %d",vfr_source,result);
   return result;
}

ADSPResult vtm_unsubscribe_afe(vtm_t *vtm_ptr, uint8_t vfr_mode, uint32_t vsid, uint8_t timing_ver)
{
   ADSPResult result = ADSP_EOK;
   uint32_t size = 0;
   elite_msg_any_t sMsg;
   elite_msg_custom_afe_vfr_config_info_t *payload_ptr;
   uint32_t vfr_source; // source of hard vfr
   MSG_3(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vtm_unsubscribe_afe begin, vfr_mode(%d), vsid(%lx), version(%d)",vfr_mode,vsid,(int16_t)timing_ver);

   if (VFR_CLIENT_INFO_VER_1 == timing_ver)
   {
      vfr_source = AFE_VFR_SOURCE_INTERNAL; // default is hard vfr from internal source
      if (VFR_HARD_EXT == vfr_mode)
      {
         vfr_source = AFE_VFR_SOURCE_EXTERNAL;  // hard vfr from external source
      }
   }
   else //VFR_CLIENT_INFO_VER_2
   {
      vfr_source = vsid;
   }
   size = sizeof(elite_msg_custom_afe_vfr_config_info_t);
   result = elite_msg_create_msg( &sMsg,
         &size,
         ELITE_CUSTOM_MSG,
         NULL,
         NULL,
         ADSP_EOK);
   if(ADSP_FAILED(result))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vtm_unsubscribe_afe: FAILED to create EliteMsg!! Not Sending Msg!!");
      // added return code to avoid crash incase of buffer run-out
      return result;
   }
   payload_ptr = (elite_msg_custom_afe_vfr_config_info_t *) sMsg.pPayload;
   payload_ptr->sec_op_code = ELITEMSG_CUSTOM_VFR_DISABLE;

   /* Default VFR client interface version */
   payload_ptr->client_vfr_info.vfr_client_info_ver = timing_ver;

   payload_ptr->client_vfr_info.vfr_clientq = vtm_ptr->cmdq_ptr; // Vtm cmd Q
   payload_ptr->client_vfr_info.client_msg_id = VOICE_TIMER_HARD_VFR; // Vtm hard vfr msg id
   payload_ptr->client_vfr_info.vfr_source = vfr_source;

   result = qurt_elite_queue_push_back(vtm_ptr->afe_cmdq_ptr, (uint64_t*)&sMsg);//hack: fix this
   if (ADSP_FAILED(result))
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vtm_unsubscribe_afe: Failed to send subscribe msg to AFE");
    elite_msg_return_payload_buffer(&sMsg);
      return result;
   }
#if defined(VTM_ONESHOT_DEBUG)
   vtm_ptr->first_hard_vfr_rcvd = FALSE;
   vtm_ptr->test_flag = FALSE;
#endif

   MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vtm_unsubscribe_afe end, vfr_source=%lx, result = %d",vfr_source,result);
   return result;
}

static ADSPResult vtm_test_signal_handler(void* instance_ptr)
{
   vtm_t* vtm_ptr = (vtm_t*)instance_ptr;
   qurt_elite_signal_clear(&vtm_ptr->test_signal);
   if(vtm_ptr->test_flag)
   {
      vtm_ptr->test_flag = FALSE;
   }
   else
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Error! VTM test signal handler found flag cleared!");
   }
   return ADSP_EOK;
}
