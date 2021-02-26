/*========================================================================

*//** @file SwitchSvc.cpp
This file contains functions for Gapless Switch Service.

Copyright (c) 2010-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/dynamic_svcs/switch_svc/src/SwitchSvc.cpp#1 $


when       who     what, where, why
--------   ---     -------------------------------------------------------
11/10/10   RP      Created file.

========================================================================== */


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */

#include "qurt_elite.h"
#include "Elite.h"
#include "EliteMsg_Custom.h"
#include "AudioStreamMgr_AprIf.h"
#include "SwitchSvc.h"
#include "adsp_asm_api.h"
#include "AudioStreamMgr_GetSetBits.h"
#include "AudioStreamMgr.h"
#include "SwitchSvc_Structs.h"
#include "audio_basic_op_ext.h"
#include "stringl.h"

/*--------------------------------------------------------------*/
/* Macro definitions                                            */
/* -------------------------------------------------------------*/

/*----------------------------------------------------------------------------
* Global Data Definitions
* -------------------------------------------------------------------------*/
static char THREAD_NAME[] = "Swit";
static char SWITCH_SVC_DATA_Q0_NAME[] = "Swid0";
static char SWITCH_SVC_DATA_Q1_NAME[] = "Swid1";
static char SWITCH_SVC_BUF_Q_NAME[] = "Swib";
static char SWITCH_SVC_CMD_Q_NAME[] = "Swic";

/* -----------------------------------------------------------------------
** Function prototypes
** ----------------------------------------------------------------------- */
// destructor
static void switch_svc_destroy (switch_svc_t* pMe);

// Main work loop for service thread. Pulls msgs off of queues and processes them.
static int switch_svc_workloop(void* pInstance);

// message handler functions
static int switch_svc_destroy_yourself(switch_svc_t* pMe);
static ADSPResult switch_svc_handle_custom_command(switch_svc_t* pMe);
static ADSPResult switch_svc_connect_to_service(switch_svc_t* pMe);
static ADSPResult switch_svc_disconnect_from_service(switch_svc_t* pMe);
static ADSPResult switch_svc_return_unsupported (switch_svc_t* pMe);
static void switch_svc_destroy_input_data_q(switch_svc_t* pMe);
static ADSPResult switch_svc_flush_port(switch_svc_t* pMe);
static ADSPResult switch_svc_flush_input_data_q(switch_svc_t* pMe, uint8_t port);
static ADSPResult switch_svc_release_input_msg(switch_svc_t *pMe);
static ADSPResult switch_svc_create_initial_output_buffers(switch_svc_t *pMe, uint32_t num_out_bufs);
static ADSPResult switch_svc_check_recreate_output_buffers(switch_svc_t *pMe, elite_msg_any_t* out_msg_ptr);
static ADSPResult switch_svc_discard_mark_buffer(switch_svc_t* pMe,elite_msg_any_t *pDataQMsg);

/* -----------------------------------------------------------------------
** Queue handler f
** ----------------------------------------------------------------------- */
typedef ADSPResult (*switch_queue_handler_f) (switch_svc_t* pMe , uint32_t port_id);
typedef ADSPResult (*switch_command_handler_f) (switch_svc_t* pMe);

static ADSPResult switch_svc_process_cmd_queue(switch_svc_t* pMe, uint32_t port_id);
static ADSPResult switch_svc_process_input_data_q(switch_svc_t* pMe, uint32_t port_id);
static ADSPResult switch_svc_process_output_buf_q(switch_svc_t* pMe, uint32_t port_id);

static switch_queue_handler_f q_handler_ptr[4]=
{
   switch_svc_process_cmd_queue,
   switch_svc_process_input_data_q,
   switch_svc_process_input_data_q,
   switch_svc_process_output_buf_q
};


/* -----------------------------------------------------------------------
** Message handler f
** ----------------------------------------------------------------------- */
// Build the function pointer table for message handling. Some common handlers
// like ReturnSuccess() which ignores the command and returns success, and
// Unsupported() which returns an error and outputs an error message are useful
// for multiple functions that this service does not support.
// In this example, this service has no concept of state. so just return success
// when RUN or STOP is commanded.
static switch_command_handler_f asm_to_switch_cmd_handler_ptr[] =
{

   switch_svc_handle_custom_command,  // 0 - ELITE_CUSTOM_MSG
   switch_svc_return_unsupported,     // 1 - ELITE_CMD_START_SERVICE
   switch_svc_destroy_yourself,       // 2 - ELITE_CMD_DESTROY_SERVICE
   switch_svc_connect_to_service,     // 3 - ELITE_CMD_CONNECT
   switch_svc_disconnect_from_service,// 4 - ELITE_CMD_DISCONNECT
};

/* =======================================================================
**                          Function Definitions
** ======================================================================= */

/*======================================================================

FUNCTION      switch_svc_create

DESCRIPTION   1) creates the switch state structure
2) creates the queues (2 input data, 1 command)
3) creates the channel and adds queues to it
4) Launches the thread

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/

ADSPResult switch_svc_create
(
 uint32_t init_params,
 void **handle
 )
{
   switch_svc_init_params *switch_params = (switch_svc_init_params *)init_params;

   if(NULL == switch_params || NULL == handle || NULL == switch_params->pCbData)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Switch service creation received NULL params");
      return ADSP_EBADPARAM;
   }

   // static variable counter to make queue and thread name strings unique
   // limit to 16 bits so it will roll over and and only cost 4 characters in hexadecimal.
   // Queues in existence likely to have unique names, but not required...
   uint32_t count;
   uint32_t result;
   char a_data_q0_name[QURT_ELITE_DEFAULT_NAME_LEN];    //data queue name for first decoder
   char a_data_q1_name[QURT_ELITE_DEFAULT_NAME_LEN];    //data queue name for second decoder
   char a_buf_q_name[QURT_ELITE_DEFAULT_NAME_LEN];      //buffer queue name for downstream svc
   char a_cmd_q_name[QURT_ELITE_DEFAULT_NAME_LEN];     //command queue name

   *handle = NULL; // default value

   // allocate instance struct
   switch_svc_t *pMe = (switch_svc_t*) qurt_elite_memory_malloc( sizeof(switch_svc_t),
      QURT_ELITE_HEAP_DEFAULT);
   if (!pMe)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Insufficient memory to allocate to switch service struct instance.It requires %lu bytes",sizeof(switch_svc_t));
      return ADSP_ENOMEMORY;
   }

   // zero out all the fields.
   memset(pMe, 0, sizeof(*pMe));
   //Set up ASM callBack
   pMe->CbData = *(AudioStreamMgr_CallBackHandleType *)switch_params->pCbData;
   pMe->delay_ptr = switch_params->delay_ptr;
   *pMe->delay_ptr = 0;

   // set up channel
   qurt_elite_channel_init(&pMe->channel);

   count = qurt_elite_atomic_get(&qurt_elite_globalstate.nMsgQs) & 0x000FFFFFL;
   qurt_elite_atomic_increment(&qurt_elite_globalstate.nMsgQs);

   // name the queues. Name is concatenated with 5 items:
   // - unique 3 letter code for service
   // - letter abbreviating which queue, e.g. 'd' 'c' 'b' for data, cmd, buf.
   // - number in case there are multiple queues of the same type, e.g. 2 buf q's
   // - count (up to 5 digits)
   // e.g. the first instance of this service would name its dataQ "/EPLd00".and cmd q "/EPLc00"
   snprintf(a_data_q0_name, QURT_ELITE_DEFAULT_NAME_LEN,"%s%x",SWITCH_SVC_DATA_Q0_NAME,(int)count);
   snprintf(a_data_q1_name, QURT_ELITE_DEFAULT_NAME_LEN,"%s%x", SWITCH_SVC_DATA_Q1_NAME,(int)count);
   snprintf(a_buf_q_name, QURT_ELITE_DEFAULT_NAME_LEN,"%s%x", SWITCH_SVC_BUF_Q_NAME,(int)count);
   snprintf(a_cmd_q_name, QURT_ELITE_DEFAULT_NAME_LEN,"%s%x", SWITCH_SVC_CMD_Q_NAME,(int)count);

   if (ADSP_FAILED(result = qurt_elite_queue_create(a_data_q0_name,
      MAX_INP_DATA_Q_ELEMENTS, &(pMe->service_handle[0].dataQ)))

      || ADSP_FAILED(result = qurt_elite_queue_create(a_data_q1_name,
      MAX_INP_DATA_Q_ELEMENTS, &(pMe->service_handle[1].dataQ)))

       || ADSP_FAILED(result = qurt_elite_queue_create(a_buf_q_name,
      MAX_SWITCH_SVC_BUF_Q_ELEMENTS, &(pMe->bufQ)))

      || ADSP_FAILED(result = qurt_elite_queue_create(a_cmd_q_name,
      MAX_CMD_Q_ELEMENTS, &(pMe->service_handle[0].cmdQ))))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
         "SwitchSvc Failed to create Decoder service message queues!! \n");
      switch_svc_destroy(pMe);
      return result;
   }

   //Both handles should correspond to same command queue
   pMe->service_handle[1].cmdQ = pMe->service_handle[0].cmdQ;
   //gpQ for both should be NULL
   pMe->service_handle[0].gpQ = pMe->service_handle[1].gpQ = NULL;


   if (ADSP_FAILED(result = qurt_elite_channel_addq(&pMe->channel, pMe->service_handle[0].cmdQ,SWITCH_CMD_SIG))
      || ADSP_FAILED(result = qurt_elite_channel_addq(&pMe->channel, pMe->service_handle[0].dataQ,SWITCH_DATA0_SIG))
      || ADSP_FAILED(result = qurt_elite_channel_addq(&pMe->channel, pMe->service_handle[1].dataQ,SWITCH_DATA1_SIG))
      || ADSP_FAILED(result = qurt_elite_channel_addq(&(pMe->channel), pMe->bufQ, SWITCH_BUF_SIG))
       )
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
         "SwitchSvc ADSP_FAILED to add mqs to channel result = %d!!\n", (int)result);
      switch_svc_destroy(pMe);
      return(result);
   }

   // create output buffers with default size
   result = switch_svc_create_initial_output_buffers(pMe, switch_params->max_out_bufs);
   if (ADSP_FAILED(result))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to create Switch service output buffers!");
      switch_svc_destroy(pMe);
      return result;
   }

   // populate me
   pMe->service_handle[0].unSvcId = pMe->service_handle[1].unSvcId = ELITE_DYN_SWITCH_SVCID;

   // Launch the thread
   if (ADSP_FAILED(result = qurt_elite_thread_launch(&(pMe->service_handle[0].threadId), THREAD_NAME, NULL,
      SWITCH_THREAD_STACK_SIZE, elite_get_thrd_prio(ELITETHREAD_DYNA_SWITCH_SVC_PRIO_ID), switch_svc_workloop, (void*)pMe,
      QURT_ELITE_HEAP_DEFAULT)))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to launch Switch service thread!! \n");
      switch_svc_destroy(pMe);
      return result;
   }

   //assign the same thread ID for service handle 1
   pMe->service_handle[1].threadId = pMe->service_handle[0].threadId;

   //At the time of creation we always use port 0. So give out the
   //corresponding handle
   *handle = &(pMe->service_handle[0]);
   pMe->port_state[0] = ACTIVE;

   return ADSP_EOK;
}

/*======================================================================

FUNCTION      switch_svc_destroy

DESCRIPTION   1) destroys input data queue
2) destroys command queue
3) destroys channel
4) frees thread stack memory
4) frees state structure memory

DEPENDENCIES  none

RETURN VALUE  none

SIDE EFFECTS  none

======================================================================*/
static void switch_svc_destroy
(
 switch_svc_t* pMe
 )
{
   if (pMe)
   {

      // call utility function to destroy data Q
      switch_svc_destroy_input_data_q(pMe);

      // call common utility function to destroy buf Q
      elite_svc_destroy_buf_queue(pMe->bufQ, pMe->out_bufs_allocated);

      // call utility function to destroy cmd Q
      elite_svc_destroy_cmd_queue(pMe->service_handle[0].cmdQ);

      //destroy channel
      qurt_elite_channel_destroy(&pMe->channel);

      //free self state structure
      qurt_elite_memory_free (pMe);
   }
}


/*======================================================================

FUNCTION      switch_svc_destroy_yourself

DESCRIPTION   1) Releases any command message. This includes
sending the Ack and returning the msg to the
return queue.
2) Then calls the destroy function for the service

DEPENDENCIES  none

RETURN VALUE  error code ADSP_ETERMINATED

SIDE EFFECTS  none

======================================================================*/
static ADSPResult switch_svc_destroy_yourself
(
 switch_svc_t* pMe
 )
{
   elite_msg_any_t temp_cmd_msg = pMe->cmd_msg;

   switch_svc_destroy(pMe);

   elite_msg_finish_msg( &temp_cmd_msg, ADSP_EOK );

   // send ADSP_ETERMINATED so calling routine knows the destroyer has been invoked.
   return ADSP_ETERMINATED;
}

/*======================================================================

FUNCTION      switch_svc_destroy_input_data_q

DESCRIPTION   Flushes each of the two input data queues and destroys them

DEPENDENCIES  none

RETURN VALUE  none

SIDE EFFECTS  none

======================================================================*/
static void switch_svc_destroy_input_data_q
(
 switch_svc_t* pMe
 )
{
   uint32_t i;

   for(i=0; i<NUM_PORTS_SWITCH; i++)
   {
      if (pMe->service_handle[i].dataQ)
      {
         switch_svc_flush_input_data_q(pMe, i);
         qurt_elite_queue_destroy(pMe->service_handle[i].dataQ);
      }
   }
}

/*======================================================================

FUNCTION      switch_svc_flush_input_data_q

DESCRIPTION   1) Frees any input data commands held on to
2) recursively pops and frees any command in the
input data queue

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/
static ADSPResult switch_svc_flush_input_data_q
(
 switch_svc_t* pMe,
 uint8_t port
 )
{

   elite_msg_any_t msg;
   // Drain any queued buffers
   while(ADSP_EOK == qurt_elite_queue_pop_front(pMe->service_handle[port].dataQ, (uint64_t*)&msg))
   {
      if(msg.unOpCode == ELITE_DATA_MARK_BUFFER)
      {
         switch_svc_discard_mark_buffer(pMe,&msg);
      }
      else
      {
      elite_msg_finish_msg(&msg, ADSP_EOK);
   }
   }

   //if the port id matches the current data bit, this means it is possible we have
   //already de-queued an input data msg (cached). we need to return this msg too.
   if (  ( (0==port)&&(SWITCH_DATA0_SIG==pMe->current_data_bit) )
       ||( (1==port)&&(SWITCH_DATA1_SIG==pMe->current_data_bit) )
       )
   {
      //return the already cached msg if there is any
      if (NULL!=pMe->inp_data_msg.pPayload)
      {
         (void) switch_svc_release_input_msg(pMe);
      }
   }

   return ADSP_EOK;
}

/*======================================================================

FUNCTION      switch_svc_workloop

DESCRIPTION   This function is the main work loop for the service. Commands
are handled with the highest priority. Data processing is handled only
when command queue is empty. This should be typical of most/all services.

The way data is handled in this example is simple. The thread listens for
command and data queues simultaneously (commands always prioritized).
When a data buffer is received, it is stored, and the thread switches to listening
for command queue and output buffer queue. As soon as an output buffer is received,
the processing function is called with the data buffer and output buffer, for
one-to-one transform. Many services may work in this way, but variations are also
possible. For example, decoders probably are ready to run as soon as there is any
input data. Only after decoder returns is it known whether an output
buffer is needed. So the work loop for a decoder service may look different.

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/
static int switch_svc_workloop
(
 void* pInstance
 )
{
   switch_svc_t *pMe = (switch_svc_t*)pInstance;
   uint32_t unSigBit;
   ADSPResult result = ADSP_EFAILED;

   // set up mask for listening to the msg queues.
   // save for mask bitfields
   pMe->current_bitfield = SWITCH_CMD_SIG;

   // Enter forever loop
   for(;;)
   {
      // block on any one or more of selected queues to get a msg
      (void) qurt_elite_channel_wait(&(pMe->channel), pMe->current_bitfield);

      for (;;)
      {
         // Check for signals
         pMe->channel_status = qurt_elite_channel_poll(&(pMe->channel), pMe->current_bitfield);

         if( pMe->channel_status == 0 ) break;

         unSigBit = s32_cl0_s32 ( pMe->channel_status);

         result = q_handler_ptr[unSigBit](pMe, unSigBit - 1);

         if(result == ADSP_ETERMINATED) return ADSP_EOK;

      }
   }

   return result;
}

/*======================================================================

FUNCTION      switch_svc_process_cmd_queue

DESCRIPTION   pops a command from the command queue and
calls the appropriate function

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/
static ADSPResult switch_svc_process_cmd_queue
(
 switch_svc_t* pMe,
 uint32_t
 )
{

   ADSPResult result=ADSP_EOK;

   // Take next msg off the q
   result = qurt_elite_queue_pop_front(pMe->service_handle[0].cmdQ, (uint64_t*)&(pMe->cmd_msg) );

   // Process the msg
   if (ADSP_EOK == result)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_LOW_PRIO, "Message ID 0x%8x!!", (int)pMe->cmd_msg.unOpCode);
      /* Sanity check */
      const uint32_t cmd_table_size = sizeof(asm_to_switch_cmd_handler_ptr)/sizeof(asm_to_switch_cmd_handler_ptr[0]);
      if ( pMe->cmd_msg.unOpCode >= cmd_table_size )
      {
         return switch_svc_return_unsupported(pMe);
      }
      else
      {
         // table lookup to call handling function,
         result =  asm_to_switch_cmd_handler_ptr[pMe->cmd_msg.unOpCode](pMe);
      }
   }

   return result;
}

/*======================================================================

FUNCTION      switch_svc_process_input_data_q

DESCRIPTION   pops a command from the input data queue and takes the
appropriate action based on the opcode

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/
static ADSPResult switch_svc_process_input_data_q
(
 switch_svc_t *pMe,
 uint32_t port_id
 )
{
   ADSPResult nResult = ADSP_EOK;
   uint32_t   op_code;

   if (pMe->inp_data_msg.pPayload)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Already holding on to an input data buffer, cannot process another buffer until this is freed");
      return ADSP_EUNEXPECTED;
   }

   // Take next msg off the q
   nResult = qurt_elite_queue_pop_front(pMe->service_handle[port_id].dataQ,
                                        (uint64_t*)&(pMe->inp_data_msg));

   if(ADSP_EOK != nResult)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed get input data msg");
      return ADSP_EFAILED;
   }

   op_code = pMe->inp_data_msg.unOpCode;

   switch (op_code)
   {
      //data buffer from decoder
   case ELITE_DATA_BUFFER:
   {
      //switch to listen to cmd and output bufQ
      pMe->current_data_bit = (pMe->current_bitfield & PORT_SWITCH_MASK);
      pMe->current_bitfield = SWITCH_CMD_SIG | SWITCH_BUF_SIG;
      break;
   }
      case ELITE_DATA_MARK_BUFFER:
      {
         //for data buffer message and media type command just push it into input data Q of POPP .
         elite_msg_any_t mark_buffer_msg;

         uint32_t payload_size = sizeof(elite_msg_data_mark_buffer_t);
         nResult = elite_msg_create_msg(&mark_buffer_msg,&payload_size,ELITE_DATA_MARK_BUFFER,NULL,0,0);
         if (ADSP_FAILED(nResult))
         {
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Fail to create mark buffer message");
            switch_svc_discard_mark_buffer(pMe,&pMe->inp_data_msg);
            (void) switch_svc_release_input_msg(pMe);
            return nResult;
         }

         elite_msg_data_mark_buffer_t *inp_mark_buffer_ptr = (elite_msg_data_mark_buffer_t *)(pMe->inp_data_msg.pPayload);
         elite_msg_data_mark_buffer_t *out_mark_buffer_ptr = (elite_msg_data_mark_buffer_t *)(mark_buffer_msg.pPayload);


         //copy the input to the output msg
         out_mark_buffer_ptr ->token_lsw = inp_mark_buffer_ptr->token_lsw;
         out_mark_buffer_ptr ->token_msw = inp_mark_buffer_ptr->token_msw;

         nResult = qurt_elite_queue_push_back(pMe->downstream_svc_ptr->dataQ, (uint64_t*)&(mark_buffer_msg));
         if(ADSP_FAILED(nResult))
         {
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to deliver buffer dowstream. Dropping!\n");
            switch_svc_discard_mark_buffer(pMe,&pMe->inp_data_msg);
         }

         nResult = switch_svc_release_input_msg(pMe);

         break;
      }

   case ELITE_DATA_MEDIA_TYPE:
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "received media type message");

      elite_msg_any_t media_type_msg;

      uint32_t payload_size = sizeof(elite_msg_data_media_type_apr_t) + sizeof(elite_multi_channel_pcm_fmt_blk_t);

      nResult = elite_msg_create_msg(&media_type_msg,&payload_size,ELITE_DATA_MEDIA_TYPE,NULL, 0,0);
      if (ADSP_FAILED(nResult))
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Fail to create media type message");
         (void) switch_svc_release_input_msg(pMe);
         return nResult;
      }

      elite_msg_data_media_type_apr_t *inp_media_type_ptr = (elite_msg_data_media_type_apr_t *)(pMe->inp_data_msg.pPayload);
      elite_msg_data_media_type_apr_t *out_media_type_ptr = (elite_msg_data_media_type_apr_t *)(media_type_msg.pPayload);

      if (ELITEMSG_MEDIA_FMT_MULTI_CHANNEL_PCM != inp_media_type_ptr->unMediaFormatID)
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "input media type message is not multichannel PCM, returning the input message");
         (void) switch_svc_release_input_msg(pMe);
         return ADSP_EFAILED;
      }

      out_media_type_ptr->unMediaTypeFormat = inp_media_type_ptr->unMediaTypeFormat;
      out_media_type_ptr->unMediaFormatID   = inp_media_type_ptr->unMediaFormatID;

      elite_multi_channel_pcm_fmt_blk_t *inp_fmt_blk_ptr =
         (elite_multi_channel_pcm_fmt_blk_t*) ((uint8_t*)(inp_media_type_ptr)+sizeof(elite_msg_data_media_type_apr_t));

      elite_multi_channel_pcm_fmt_blk_t *out_fmt_blk_ptr =
         (elite_multi_channel_pcm_fmt_blk_t*) ((uint8_t*)(out_media_type_ptr)+sizeof(elite_msg_data_media_type_apr_t));

      pMe->sample_rate = inp_fmt_blk_ptr->sample_rate;
      pMe->num_channels = inp_fmt_blk_ptr->num_channels;
      pMe->bits_per_sample = inp_fmt_blk_ptr->bits_per_sample;

      //copy the input to the output msg
      *out_fmt_blk_ptr = *inp_fmt_blk_ptr;

      //push the media type command into input data Q of POPP .
      nResult = qurt_elite_queue_push_back(pMe->downstream_svc_ptr->dataQ, (uint64_t*)&media_type_msg);
      if(ADSP_FAILED(nResult))
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to deliver media type message dowstream.\n");
         (void) switch_svc_release_input_msg(pMe);
         return nResult;
      }

      nResult = switch_svc_release_input_msg(pMe);

      break;
   }

      //EOS data command from decoder
   case ELITE_DATA_EOS:
   {
      MSG(MSG_SSID_QDSP6, DBG_MED_PRIO, "Switch svc received EOS cmd!!!!");

      elite_msg_any_t eos_msg;
      uint32_t eos_payload_size = sizeof( elite_msg_data_eos_apr_t );

      nResult = elite_msg_create_msg(&eos_msg, &eos_payload_size, ELITE_DATA_EOS, NULL,0,0);
      if (ADSP_FAILED(nResult))
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"ERROR sending EOS downstream, out of buffer. Downstream Svc will not receive EOS! ");
         //free input data cmd
         (void) switch_svc_release_input_msg(pMe);
         return nResult;
      }

      elite_msg_data_eos_apr_t *inp_eos_ptr = (elite_msg_data_eos_apr_t*)(pMe->inp_data_msg.pPayload);
      elite_msg_data_eos_apr_t *out_eos_ptr = (elite_msg_data_eos_apr_t*)(eos_msg.pPayload);

      QURT_ELITE_ASSERT(ELITEMSG_DATA_EOS_APR==inp_eos_ptr->unEosFormat);

      out_eos_ptr->unEosFormat = inp_eos_ptr->unEosFormat;

      memscpy(&out_eos_ptr->eosInfo, sizeof(out_eos_ptr->eosInfo), &inp_eos_ptr->eosInfo, sizeof(inp_eos_ptr->eosInfo));

      if((ACTIVE == pMe->port_state[0]) && (ACTIVE == pMe->port_state[1]))
      {
         //if both ports are connected then this EOS should reset session clk
         out_eos_ptr->unEosFormat = ELITEMSG_DATA_RESET_SESSION_CLK;
      }
      else
      {
         //if one port is connected to an active stream in gapless playback mode
         out_eos_ptr->unEosFormat = ELITEMSG_DATA_GAPLESS_TERMINATION_EOS;
      }

      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "SwitchSvc: Propagating EOS downstream");
      nResult = qurt_elite_queue_push_back(pMe->downstream_svc_ptr->dataQ, (uint64_t*)&eos_msg);

      if(ADSP_FAILED(nResult))
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to deliver EOS dowstream. Downstream Svc will not receive EOS!\n");
         (void) switch_svc_release_input_msg(pMe);
         return nResult;
      }

      //switch to listening to other data port upon processing EOS
      pMe->current_bitfield ^= PORT_SWITCH_MASK;
      //update the current data bit
      pMe->current_data_bit = (pMe->current_bitfield & PORT_SWITCH_MASK);

      nResult = switch_svc_release_input_msg(pMe);

      break;
   }

   default:
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "SwitchSvc: Unexpected opCode for data command 0x%x", (int)op_code );
         (void) switch_svc_release_input_msg(pMe);
         return ADSP_EBADPARAM;
      }
   } /* switch (uAprOpCode) */

   return nResult;
}

/*======================================================================
FUNCTION      switch_svc_process_output_buf_q

DESCRIPTION   pops a buffer from the output buffer queue and takes the
appropriate action based on the opcode of input data buffer

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/
static ADSPResult switch_svc_process_output_buf_q(switch_svc_t *pMe, uint32_t)
{
   ADSPResult nResult = ADSP_EOK;

   qurt_elite_bufmgr_node_t  out_buffer_node;
   uint32_t                  op_code;

   if (NULL==pMe->inp_data_msg.pPayload)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Input data message is empty, cannot process output buffers");
      return ADSP_EUNEXPECTED;
   }

   // Take next msg off the q
   nResult = qurt_elite_queue_pop_front(pMe->bufQ, (uint64_t*)&out_buffer_node);

   if(ADSP_EOK != nResult)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed get output buffers");
      return ADSP_EFAILED;
   }

   op_code = pMe->inp_data_msg.unOpCode;

   switch (op_code)
   {
   //data buffer from decoder
   case ELITE_DATA_BUFFER:
   {
      uint32_t old_buf_delay = switch_svc_get_buf_delay_from_client_token(pMe,
            &(((elite_msg_any_payload_t*)out_buffer_node.pBuffer)->unClientToken));

      elite_msg_any_t*  out_msg_ptr = elite_msg_convt_buf_node_to_msg(&out_buffer_node,
                                                                      ELITE_DATA_BUFFER,
                                                                      NULL, /* resp_q_ptr */
                                                                      0,    /* client_token*/
                                                                      0);   /* resp_result */
      //copy input msg to output msg
      elite_msg_data_buffer_t* inp_data_buf_ptr = (elite_msg_data_buffer_t*)(pMe->inp_data_msg.pPayload);
      elite_msg_data_buffer_t* out_data_buf_ptr = (elite_msg_data_buffer_t*)(out_msg_ptr->pPayload);

      if (out_data_buf_ptr->nMaxSize < inp_data_buf_ptr->nMaxSize)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Input data size too large, reallocating output buffers %lu %lu", (uint32_t) (inp_data_buf_ptr->nActualSize+inp_data_buf_ptr->nOffset), (uint32_t) (inp_data_buf_ptr->nMaxSize) );

         //put current output buffer back to Q so that it can be released.
         nResult = switch_svc_check_recreate_output_buffers(pMe, out_msg_ptr);
         if (ADSP_FAILED(nResult))
         {
            switch_svc_subtract_old_buf_delay(pMe, old_buf_delay);
            return nResult;
         }

         //re-assign the data buffer pointer with reallocated buffer address
         out_data_buf_ptr = (elite_msg_data_buffer_t*)(out_msg_ptr->pPayload);
      }

      out_data_buf_ptr->ullTimeStamp = inp_data_buf_ptr->ullTimeStamp;
      out_data_buf_ptr->nFlag        = inp_data_buf_ptr->nFlag;
      out_data_buf_ptr->nOffset      = inp_data_buf_ptr->nOffset;
      out_data_buf_ptr->nActualSize  = inp_data_buf_ptr->nActualSize;

      uint32_t delay = switch_svc_adjust_buf_delay_and_get_client_token(pMe, inp_data_buf_ptr->nActualSize, old_buf_delay);
      out_data_buf_ptr->unClientToken = delay;

      memscpy((void*)&(out_data_buf_ptr->nDataBuf),
              out_data_buf_ptr->nMaxSize,
              (void*)&(inp_data_buf_ptr->nDataBuf),
              (inp_data_buf_ptr->nActualSize+inp_data_buf_ptr->nOffset));

      //push output buffer into input data Q of POPP .
      nResult = qurt_elite_queue_push_back(pMe->downstream_svc_ptr->dataQ, (uint64_t*)out_msg_ptr);
      if(ADSP_FAILED(nResult))
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to deliver buffer dowstream. Dropping!\n");
         elite_msg_release_msg(out_msg_ptr);
      }

      //release input buffer to decoder svc
      nResult = switch_svc_release_input_msg(pMe);
      if(ADSP_FAILED(nResult))
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to release input data msg!\n");
         return nResult;
      }

      break;
   }

   case ELITE_DATA_MEDIA_TYPE:
   case ELITE_DATA_EOS:
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Media Type and/or EOS msg should be processed immediately on inputQ!");
      return ADSP_EUNEXPECTED;
   }

   default:
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "SwitchSvc: Unexpected opCode for data command 0x%x", (int)op_code );
         (void) switch_svc_release_input_msg(pMe);
         return ADSP_EBADPARAM;
      }
   } /* switch (uAprOpCode) */


   return ADSP_EOK;
}

/*======================================================================

FUNCTION      switch_svc_connect_to_service

DESCRIPTION   stores the service handle of the downstream module (POPP)

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/
static ADSPResult switch_svc_connect_to_service
(
 switch_svc_t* pMe
 )
{
   ADSPResult result = ADSP_EOK;
   elite_msg_cmd_connect_t* p_cmd_msg_payload = NULL;

   p_cmd_msg_payload = (elite_msg_cmd_connect_t*) ((pMe->cmd_msg).pPayload);

   elite_svc_handle_t *svc_handle_ptr = (elite_svc_handle_t *)p_cmd_msg_payload->pSvcHandle;

   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "switch service connecting to Post-Proc service \n");

   if (NULL != pMe->downstream_svc_ptr)
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
         "Connection FAILED, Post-Proc service is already connected.\n");
      result = ADSP_EALREADY;
   }
   //else accept the connection
   else
   {
      MSG(MSG_SSID_QDSP6, DBG_LOW_PRIO, "Connection Succeeded.\n");
      pMe->downstream_svc_ptr = svc_handle_ptr;
   }

   //at the time of connection port0 would be active port. so listen to
   //data buffers on this port
   pMe->current_bitfield = SWITCH_CMD_SIG | SWITCH_DATA0_SIG;
   pMe->current_data_bit = SWITCH_DATA0_SIG;

   result = ADSP_EOK;
   elite_svc_send_ack( &(pMe->cmd_msg), result);

   return result;
}

/*======================================================================

FUNCTION      switch_svc_disconnect_from_service

DESCRIPTION   resets the service handle for downstream module

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/
static ADSPResult switch_svc_disconnect_from_service
(
 switch_svc_t* pMe
 )
{
   ADSPResult result = ADSP_EOK;
   elite_msg_cmd_connect_t* p_cmd_msg_payload = NULL;
   elite_svc_handle_t *svc_handle_ptr = NULL;

   p_cmd_msg_payload = (elite_msg_cmd_connect_t*)(pMe->cmd_msg).pPayload;
   svc_handle_ptr = (elite_svc_handle_t* )p_cmd_msg_payload->pSvcHandle;


   MSG(MSG_SSID_QDSP6, DBG_LOW_PRIO, "Switch Service is disconnecting from downstream service...");


   if (svc_handle_ptr != pMe->downstream_svc_ptr)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Disonnection FAILED: connected to service with handle = 0x%x, but requested to be disconnected from service with handle = 0x%x",
                                            (int)pMe->downstream_svc_ptr, (int)svc_handle_ptr );
      result = ADSP_EBADPARAM;
   }
   //else accept the disconnection
   else
   {
      MSG(MSG_SSID_QDSP6, DBG_LOW_PRIO, "Disonnection Succeeded.\n");
      pMe->downstream_svc_ptr = NULL;
   }

   if (NULL != pMe->inp_data_msg.pPayload)
   {
      result = switch_svc_release_input_msg(pMe);
      if (ADSP_EOK==result)
      {
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "returned input data msg when disconnecting from downstream service");

         pMe->inp_data_msg.unOpCode = 0x0000FFFFL; /* invalid opCode */
         pMe->inp_data_msg.pPayload = NULL;
      }
   }

   elite_svc_send_ack( &(pMe->cmd_msg), result);

   /*
   since we are disconnected, we can no longer process input or output buffers.
   so just listen to command queue
   */
   if(ADSP_EOK == result)
   {
      pMe->current_bitfield = SWITCH_CMD_SIG;
      pMe->current_data_bit = 0;
   }

   return ADSP_EOK;
}

/*======================================================================

FUNCTION      switch_svc_get_svc_handle

DESCRIPTION   returns the svc handle for a vacant port

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/
static ADSPResult switch_svc_get_svc_handle
(
 switch_svc_t* pMe
 )
{

   ADSPResult result = ADSP_EOK;
   elite_msg_custom_switch_get_svc_handle* p_cmd_msg_payload = NULL;

   p_cmd_msg_payload = (elite_msg_custom_switch_get_svc_handle*) ((pMe->cmd_msg).pPayload);

   //if port 0 is vacant then give out its service handle
   if(INACTIVE == pMe->port_state[0])
   {
      p_cmd_msg_payload->pSvcHandle = &(pMe->service_handle[0]);
      pMe->port_state[0] = ACTIVE; //port 0 is going to be occupied
   }
   //else if port 1 is vacant then give out its service handle
   else if(INACTIVE == pMe->port_state[1])
   {
      p_cmd_msg_payload->pSvcHandle = &(pMe->service_handle[1]);
      pMe->port_state[1] = ACTIVE; //port 1 is going to be occupied
   }
   //both ports are occupied, raise unsupported response
   else
   {
      p_cmd_msg_payload->pSvcHandle = NULL;
      result = ADSP_EUNSUPPORTED;
   }
   //send the response
   elite_msg_finish_msg(&(pMe->cmd_msg), result);

   return ADSP_EOK;
}


/*======================================================================

FUNCTION      switch_svc_vacate_port

DESCRIPTION   compares the svc handle with the internal sev handles to
decide which port needs to be vacated. Also flushes the
input buffer for that port, since this will be followed by
a disconnect

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/
static ADSPResult switch_svc_vacate_port
(
 switch_svc_t* pMe
 )
{
   ADSPResult result = ADSP_EOK;
   elite_msg_custom_switch_vacate_port* p_cmd_msg_payload = NULL;

   p_cmd_msg_payload = (elite_msg_custom_switch_vacate_port *) ((pMe->cmd_msg).pPayload);

   uint32_t port = NUM_PORTS_SWITCH+1;
   for (uint32_t i = 0; i < NUM_PORTS_SWITCH; i++)
   {
      if (p_cmd_msg_payload->pSvcHandle == &pMe->service_handle[i])
      {
         port = i;
      }

   }
   //if this is the service handle for port 0, set this as vacant
   if(port == 0)
   {
      //flush the input queue
      result = switch_svc_flush_input_data_q(pMe, 0);
      pMe->port_state[0] = INACTIVE;
   }
   //else if this is the servcie handle for por 1, set port 1 as vacant
   else if(port == 1)
   {
      //flush the input queue
      result = switch_svc_flush_input_data_q(pMe, 1);
      pMe->port_state[1] = INACTIVE;
   }
   //this is an invalid port ID
   else
   {
      result = ADSP_EBADPARAM;
   }
   //send the response
   elite_msg_finish_msg(&(pMe->cmd_msg), result);

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Vacating port %lu. result %d",port,result);

   return ADSP_EOK;
}

/*======================================================================

FUNCTION      switch_svc_flush_port

DESCRIPTION   processes the custom flush command from ASM.

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/
static ADSPResult switch_svc_flush_port
(
 switch_svc_t* pMe
 )
{
   ADSPResult result = ADSP_EOK;
   elite_msg_custom_switch_flush* p_cmd_msg_payload = NULL;

   p_cmd_msg_payload = (elite_msg_custom_switch_flush *) ((pMe->cmd_msg).pPayload);

   uint32_t port = NUM_PORTS_SWITCH+1;
   for (uint32_t i = 0; i < NUM_PORTS_SWITCH; i++)
   {
      if (p_cmd_msg_payload->pSvcHandle == &pMe->service_handle[i])
      {
         port = i;
      }

   }
   if (port < NUM_PORTS_SWITCH)
   {
   //flush the input queue
      result = switch_svc_flush_input_data_q(pMe,  port);
   }
   else
   {
      result = ADSP_EBADPARAM;
   }

   //send the response
   elite_msg_finish_msg(&(pMe->cmd_msg), result);

   return ADSP_EOK;
}


/*======================================================================

FUNCTION      switch_svc_handle_custom_command

DESCRIPTION   Handles the custom command in the appropriate manner

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/
static ADSPResult switch_svc_handle_custom_command
(
 switch_svc_t* pMe
 )
{
   ADSPResult result = ADSP_EOK;

   elite_msg_custom_header_t *pPayload = (elite_msg_custom_header_t*) (pMe->cmd_msg.pPayload);

   switch ( pPayload->unSecOpCode)
   {
   case ELITEMSG_CUSTOM_SWITCH_GET_SVC_HANDLE:
      {
         result = switch_svc_get_svc_handle(pMe);
         break;
      }
   case ELITEMSG_CUSTOM_SWITCH_VACATE_PORT:
      {
         result = switch_svc_vacate_port(pMe);
         break;
      }
   case ELITEMSG_CUSTOM_SWITCH_FLUSH:
      {
         result = switch_svc_flush_port(pMe);
         break;
      }
   default:
      {
         //send the response
         elite_msg_finish_msg(&(pMe->cmd_msg), ADSP_EUNSUPPORTED);
         return ADSP_EFAILED;
      }
   }
   return result;
}


/*======================================================================

FUNCTION      switch_svc_return_unsupported

DESCRIPTION   Unsupported message handler. Returns the message to the
appropriate queue and raises an ADSP_EUNSUPPORTED ack

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/

static ADSPResult switch_svc_return_unsupported
(
 switch_svc_t* pMe
 )
{
   //unsupported command..finish msg with error code
   MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Unsupported command with opcode = 0x%x", (int)pMe->cmd_msg.unOpCode);
   return elite_msg_finish_msg(&(pMe->cmd_msg), ADSP_EUNSUPPORTED);
}

/*======================================================================
FUNCTION      switch_svc_create_initial_output_buffers

DESCRIPTION   create output buffers and push them onto bufQ

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none
======================================================================*/
static ADSPResult switch_svc_create_initial_output_buffers(switch_svc_t *pMe, uint32_t num_out_bufs)
{
   uint32_t req_size = GET_ELITEMSG_DATABUF_REQ_SIZE(SWITCH_SVC_DEFAULT_OUT_BUF_SIZE);

   pMe->out_bufs_allocated = 0;

   for (uint32_t i = 0; i < num_out_bufs; i++)
   {
      void *out_buf_ptr =  qurt_elite_memory_malloc(req_size, ADEC_SVC_OUT_BUF);

      if (NULL==out_buf_ptr)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Insufficient memory to create initial output buffers for Switch Svc.It requires %lu bytes",req_size);
         return ADSP_ENOMEMORY;
      }

      memset(out_buf_ptr, 0, req_size);

      //initialize the parameters in the output  data buffer
      elite_msg_data_buffer_t *out_data_buf_ptr  = (elite_msg_data_buffer_t *)out_buf_ptr;

      out_data_buf_ptr->nFlag                = 0;
      out_data_buf_ptr->ullTimeStamp         = 0;
      out_data_buf_ptr->nOffset              = 0;
      out_data_buf_ptr->nMaxSize             = SWITCH_SVC_DEFAULT_OUT_BUF_SIZE;
      out_data_buf_ptr->nActualSize          = 0;
      out_data_buf_ptr->unClientToken        = 0;
      out_data_buf_ptr->pBufferReturnQ       = pMe->bufQ;
      out_data_buf_ptr->pResponseQ           = NULL;
      out_data_buf_ptr->unResponseResult     = 0;


      //push the buffer into output buffer queue
      if (ADSP_FAILED(elite_msg_push_payload_to_returnq(pMe->bufQ, (elite_msg_any_payload_t*) out_buf_ptr)))
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to push output buffers to queue!! \n");
         //free the buffer
         qurt_elite_memory_free(out_buf_ptr);
         return ADSP_EFAILED;
      }

      pMe->out_bufs_allocated++;
   }

   return ADSP_EOK;
}


/*======================================================================
FUNCTION      switch_svc_check_recreate_output_buffers

DESCRIPTION   free current output buffers and reallocate new output buffers
              with larger size and push them onto bufQ

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none
======================================================================*/
static ADSPResult switch_svc_check_recreate_output_buffers(switch_svc_t *pMe, elite_msg_any_t* out_msg_ptr)
{
   if (NULL==pMe->inp_data_msg.pPayload)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Input data msg empty, cannot check and recreate output buffers");
      return ADSP_EFAILED;
   }

   uint32_t op_code = pMe->inp_data_msg.unOpCode;

   if (ELITE_DATA_BUFFER != op_code)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Input data msg is not data buffer, no need to check and recreate output buffers");
      return ADSP_EUNEXPECTED;
   }

   elite_msg_data_buffer_t* inp_data_buf_ptr = (elite_msg_data_buffer_t*)(pMe->inp_data_msg.pPayload);
   elite_msg_data_buffer_t* out_data_buf_ptr = (elite_msg_data_buffer_t*)(out_msg_ptr->pPayload);

   uint32_t current_buf_size = out_data_buf_ptr->nMaxSize;

   if (current_buf_size > (uint32_t)(inp_data_buf_ptr->nMaxSize))
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "current output buffers are large enough, no need to reallocate output buffers");
      return ADSP_EOK;
   }

   current_buf_size = (uint32_t)(inp_data_buf_ptr->nMaxSize);

   uint32_t out_buf_size = GET_ELITEMSG_DATABUF_REQ_SIZE(current_buf_size);

   //first free the current buffer
   qurt_elite_memory_free(out_msg_ptr->pPayload);
   pMe->out_bufs_allocated--;

   void *out_buf_ptr =  qurt_elite_memory_malloc(out_buf_size, ADEC_SVC_OUT_BUF);
   if (NULL==out_buf_ptr)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Out of Memory, failed reallocate output buffers");
      return ADSP_ENOMEMORY;
   }

   memset(out_buf_ptr, 0, out_buf_size);
   pMe->out_bufs_allocated++;

   //initialize the parameters in the output data buffer
   elite_msg_data_buffer_t *new_out_data_buf_ptr  = (elite_msg_data_buffer_t *)out_buf_ptr;

   new_out_data_buf_ptr->nFlag                = 0;
   new_out_data_buf_ptr->ullTimeStamp         = 0;
   new_out_data_buf_ptr->nOffset              = 0;
   new_out_data_buf_ptr->nMaxSize             = current_buf_size;
   new_out_data_buf_ptr->nActualSize          = 0;
   new_out_data_buf_ptr->unClientToken        = 0;
   new_out_data_buf_ptr->pBufferReturnQ       = pMe->bufQ;
   new_out_data_buf_ptr->pResponseQ           = NULL;
   new_out_data_buf_ptr->unResponseResult     = 0;

   //now update the out msg payload ptr
   out_msg_ptr->pPayload = out_buf_ptr;

   return ADSP_EOK;
}

/*======================================================================
FUNCTION      switch_svc_release_input_msg

DESCRIPTION   release input data msg to its returnQ (decoder svc bufQ)

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none
======================================================================*/
static ADSPResult switch_svc_release_input_msg(switch_svc_t *pMe)
{
   ADSPResult result = ADSP_EOK;

   result = elite_msg_release_msg(&(pMe->inp_data_msg));
   if (ADSP_FAILED(result))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "failed return input msg");
      return ADSP_EFAILED;
   }

   pMe->inp_data_msg.unOpCode = 0x0000FFFFL; /* invalid opCode */
   pMe->inp_data_msg.pPayload = NULL;

   //switch back listening to cmd and input data
   pMe->current_bitfield = SWITCH_CMD_SIG | (pMe->current_data_bit);

   return ADSP_EOK;
}


/*======================================================================

FUNCTION      switch_svc_discard_mark_buffer

DESCRIPTION   Raise mark buffer discarded event without releasing mark buffer

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/

static ADSPResult switch_svc_discard_mark_buffer(switch_svc_t* pMe, elite_msg_any_t *pDataQMsg)
{
   ADSPResult result = ADSP_EOK;
   if (pDataQMsg->unOpCode != ELITE_DATA_MARK_BUFFER)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ERROR: Expecting mark buffer only");
      return ADSP_EFAILED;
   }
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Switch svc Discarding mark buffers");
   elite_msg_data_mark_buffer_t *pMarkBufferPayload = (elite_msg_data_mark_buffer_t *)pDataQMsg->pPayload;
   asm_data_event_mark_buffer_v2_t eventPayload;
   eventPayload.token_lsw = pMarkBufferPayload->token_lsw;
   eventPayload.token_msw = pMarkBufferPayload->token_msw;
   eventPayload.result = ASM_DATA_EVENT_MARK_BUFFER_DISCARDED;
   AudioStreamMgr_CallBackHandleType *pEliteCb = &pMe->CbData;
   result = AudioStreamMgr_GenerateClientEventFromCb(pEliteCb,
         ASM_DATA_EVENT_MARK_BUFFER_V2,
         0, &eventPayload, sizeof(eventPayload));
   return result;
}
