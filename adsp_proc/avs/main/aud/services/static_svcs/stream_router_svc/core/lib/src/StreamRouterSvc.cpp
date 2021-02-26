/*========================================================================
 *//** @file StreamRouterSvc.cpp
This file contains functions for Stream Router Service.

Copyright (c) 2011-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
  *//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/stream_router_svc/core/lib/src/StreamRouterSvc.cpp#1 $


when       who     what, where, why
--------   ---     -------------------------------------------------------
5/19/11    RP      Created file.
1/19/12    RP      Conversion of stream router to static service.
9/21/12    RP      Added multiport support
4/10/14    YW      AvSync Changes and 61937 Modules Unification

========================================================================== */


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */

#include "qurt_elite.h"
#include "Elite.h"
#include "EliteMsg_Custom.h"
#include "AudioStreamMgr_AprIf.h"
#include "AudDevMgr_AprIf.h"
#include "StreamRouterSvc.h"
#include "adsp_asm_api.h"
#include "adsp_media_fmt.h"
#include "adsp_asm_session_commands.h"
#include "adsp_mtmx_strtr_api.h"
#include "AudioStreamMgr_GetSetBits.h"
#include "AudioStreamMgr.h"
#include "StreamRouterSvcStructs.h"
#include "AudioStreamMgr_PrivateDefs.h"
#include "avsync_lib.h"
#include "avsync_stats_lib.h"
#include <audio_basic_op_ext.h>

/*header file for memscpy function*/
#include "stringl.h"

/*--------------------------------------------------------------*/
/* Macro definitions                                            */
/* -------------------------------------------------------------*/
//Enable when you wanted to debug avsync features.
//#define DBG_STRTR_AVSYNC

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/
static char RX_THREAD_NAME[] = "RXSR";
static char TX_THREAD_NAME[] = "TXSR";
static char RX_STREAM_ROUTER_SVC_CMD_Q_NAME[] = "RxSc";
static char TX_STREAM_ROUTER_SVC_CMD_Q_NAME[] = "TxSc";

//Stream Router self Queues aligned to 8 bytest
QURT_ELITE_ALIGN(static char RxStRtrCmdQbuf[QURT_ELITE_QUEUE_GET_REQUIRED_BYTES(MAX_CMD_Q_ELEMENTS)], 8);
QURT_ELITE_ALIGN(static char TxStRtrCmdQbuf[QURT_ELITE_QUEUE_GET_REQUIRED_BYTES(MAX_CMD_Q_ELEMENTS)], 8);

/* -----------------------------------------------------------------------
 ** Function prototypes
 ** ----------------------------------------------------------------------- */
// destructor
static void stream_router_svc_destroy (stream_router_svc_t* pMe);

// Main work loop for service thread. Pulls msgs off of queues and processes them.
static int stream_router_svc_workloop(void* pInstance);

// message handler functions
ADSPResult stream_router_svc_custom_msg_handler(stream_router_svc_t *pMe);
static ADSPResult stream_router_svc_destroy_yourself(stream_router_svc_t* pMe);
static ADSPResult stream_router_svc_return_unsupported (stream_router_svc_t* pMe);
ADSPResult stream_router_svc_flush_input_data_q(stream_router_svc_t* pMe, uint32_t port_id);


//custom message handlers
ADSPResult stream_router_svc_custom_run(stream_router_svc_t *pMe);
ADSPResult stream_router_svc_custom_run_rx_mode(stream_router_svc_t *pMe);
ADSPResult stream_router_svc_custom_run_tx_mode(stream_router_svc_t *pMe);
ADSPResult stream_router_svc_custom_suspend(stream_router_svc_t *pMe);
ADSPResult stream_router_svc_custom_pause(stream_router_svc_t *pMe);
ADSPResult stream_router_svc_custom_flush(stream_router_svc_t *pMe);
ADSPResult stream_router_svc_custom_connect(stream_router_svc_t* pMe);
ADSPResult stream_router_svc_custom_disconnect(stream_router_svc_t* pMe);
ADSPResult stream_router_svc_disable_port(stream_router_svc_t *pMe);
ADSPResult stream_router_svc_custom_map_routings(stream_router_svc_t* pMe);
ADSPResult stream_router_svc_custom_unmap_routings(stream_router_svc_t* pMe);
ADSPResult stream_router_svc_custom_reg_comp_fmt_chg(stream_router_svc_t* pMe);

/* -----------------------------------------------------------------------
 ** Queue handler f
 ** ----------------------------------------------------------------------- */
ADSPResult stream_router_svc_process_cmd_queue(stream_router_svc_t* pMe);
ADSPResult stream_router_svc_process_input_data_q_tx_mode(stream_router_svc_t* pMe, uint32_t port_id);
ADSPResult stream_router_svc_process_input_data_q_rx_mode(stream_router_svc_t* pMe, uint32_t port_id);
ADSPResult stream_router_svc_process_output_data_q_tx_mode(stream_router_svc_t* pMe, uint32_t port_id);
ADSPResult stream_router_svc_process_output_data_q(stream_router_svc_t* pMe, uint32_t port_id);
ADSPResult stream_router_svc_process_input_data_q(stream_router_svc_t* pMe, uint32_t port_id);

static ADSPResult stream_router_svc_eos_v2_resp_handler(stream_router_svc_t* pMe);

/* -----------------------------------------------------------------------
 ** Message handler f
 ** ----------------------------------------------------------------------- */
typedef ADSPResult (*stream_router_command_handler_f) (stream_router_svc_t* pMe);
// Build the function pointer table for message handling. Some common handlers
// like ReturnSuccess() which ignores the command and returns success, and
// Unsupported() which returns an error and outputs an error message are useful
// for multiple functions that this service does not support.
// In this example, this service has no concept of state. so just return success
// when RUN or STOP is commanded.
static stream_router_command_handler_f stream_router_cmd_handler_ptr[] =
{

      stream_router_svc_custom_msg_handler,     // 0 - ELITE_CUSTOM_MSG
      stream_router_svc_return_unsupported,     // 1 - ELITE_CMD_START_SERVICE
      stream_router_svc_destroy_yourself,       // 2 - ELITE_CMD_DESTROY_SERVICE
      stream_router_svc_return_unsupported,           //3 - UNSUPPORTED CMD
      stream_router_svc_return_unsupported,           //4 - UNSUPPORTED CMD
      stream_router_svc_return_unsupported,           //5 - UNSUPPORTED CMD
      stream_router_svc_return_unsupported,           //6 - UNSUPPORTED CMD
      stream_router_svc_return_unsupported,           //7 - UNSUPPORTED CMD
      stream_router_svc_return_unsupported,           //8 - UNSUPPORTED CMD
      stream_router_svc_return_unsupported,           //9 - UNSUPPORTED CMD
      stream_router_svc_return_unsupported,           //A - UNSUPPORTED CMD
      stream_router_svc_return_unsupported,           //B - UNSUPPORTED CMD
      stream_router_svc_eos_v2_resp_handler,          //C - ELITE_DATA_EOS: handles eos v2 response from rx AFE
      stream_router_svc_return_unsupported,           //D - UNSUPPORTED CMD
      stream_router_svc_return_unsupported,           //E - UNSUPPORTED CMD
      stream_router_svc_return_unsupported,           //F - UNSUPPORTED CMD
};

/* =======================================================================
 **                          Function Definitions
 ** ======================================================================= */

/*======================================================================

FUNCTION      stream_router_svc_create

DESCRIPTION
1) creates the switch state structure
2) Initializes the queues (input data Q, output data Q, command Q)
3) creates the channel and adds queues to it
4) Launches the thread

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/

ADSPResult stream_router_svc_create
(
      streamRouterSvcInitParams_t *inputParam,
      void **handle
)
{
   uint32_t result = ADSP_EOK;

   *handle = NULL; // default value

   // allocate instance struct
   stream_router_svc_t *pMe = (stream_router_svc_t*) qurt_elite_memory_malloc( sizeof(stream_router_svc_t),
         QURT_ELITE_HEAP_DEFAULT);
   if (!pMe)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Insufficient memory to create stream router service.It requires %lu bytes",sizeof(stream_router_svc_t));
      return ADSP_ENOMEMORY;
   }

   // zero out all the fields.
   memset(pMe, 0, sizeof(*pMe));

   // set up channel
   qurt_elite_channel_init(&pMe->channel);
   qurt_elite_atomic_increment(&qurt_elite_globalstate.nMsgQs);

   char *thread_name, *cmd_q_name;
   qurt_elite_queue_t *cmd_q;
   if(RX_STRTR_ID == inputParam->stream_router_id)
   {
      thread_name = RX_THREAD_NAME;
      cmd_q_name = RX_STREAM_ROUTER_SVC_CMD_Q_NAME;
      cmd_q = (qurt_elite_queue_t*)&RxStRtrCmdQbuf;
   }
   else if(TX_STRTR_ID == inputParam->stream_router_id)
   {
      thread_name = TX_THREAD_NAME;
      cmd_q_name = TX_STREAM_ROUTER_SVC_CMD_Q_NAME;
      cmd_q = (qurt_elite_queue_t*)&TxStRtrCmdQbuf;
   }
   else
   {
      stream_router_svc_destroy(pMe);
      return ADSP_EBADPARAM;
   }

   if (ADSP_FAILED(result = qurt_elite_queue_init(cmd_q_name, MAX_CMD_Q_ELEMENTS, cmd_q)))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Failed to init service comamnd queues!! \n");
      stream_router_svc_destroy(pMe);
      return result;
   }

   if (ADSP_FAILED(result = qurt_elite_channel_addq(&pMe->channel, cmd_q,STREAM_ROUTER_CMD_SIG)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " ADSP_FAILED to add mqs to channel result = %d!!\n", (int)result);
      stream_router_svc_destroy(pMe);
      return(result);
   }

   pMe->router_id = inputParam->stream_router_id;
   pMe->service_handle.unSvcId = ELITE_DYN_STREAM_ROUTER_SVCID;
   pMe->service_handle.cmdQ = cmd_q;

   // Launch the thread
   if (ADSP_FAILED(result = qurt_elite_thread_launch(&(pMe->service_handle.threadId), thread_name, NULL,
         STREAM_ROUTER_THREAD_STACK_SIZE, elite_get_thrd_prio(ELITETHREAD_DYNA_STREAM_ROUTER_SVC_PRIO_ID), stream_router_svc_workloop, (void*)pMe,
         QURT_ELITE_HEAP_DEFAULT)))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to launch Stream Router service thread!! \n");
      stream_router_svc_destroy(pMe);
      return result;
   }

   *handle = &(pMe->service_handle);

   for(uint32_t i=0; i<STRTR_MAX_PORTS; i++)
   {
      pMe->in_port[i].stream_state = DISCONNECTED;
      pMe->in_port[i].primary_output_port = STRTR_INVALID_PORT_ID;
      pMe->out_port[i].stream_state = DISCONNECTED;
      pMe->out_port[i].connected_input_port = STRTR_INVALID_PORT_ID;
   }

   return ADSP_EOK;
}

/*======================================================================

FUNCTION      stream_router_svc_destroy

DESCRIPTION
1) Deinit input data queue
2) Deinit command queue
3) Deinit channel
4) frees thread stack memory
4) frees state structure memory

DEPENDENCIES  none

RETURN VALUE  none

SIDE EFFECTS  none

======================================================================*/
static void stream_router_svc_destroy(stream_router_svc_t* pMe)
{
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "StreamRouterSvc received msg ELITE_CMD_DESTROY_SERVICE");

   if (pMe)
   {
      /** release, force_vote doesn't matter.*/
      (void)stream_router_svc_deregister_with_adsppm(pMe);

      for(uint32_t port_id=0; port_id<STRTR_MAX_PORTS; port_id++)
      {
         if(pMe->in_port[port_id].is_connected)
         {
            // call utility function to deinit input data Q
            elite_svc_destroy_data_queue(pMe->in_port[port_id].service_handle.dataQ);
         }
      }

      for(uint32_t port_id=0; port_id<STRTR_MAX_PORTS; port_id++)
      {
         if(pMe->out_port[port_id].is_connected)
         {
            // call utility function to deinit output data Q
            elite_svc_destroy_buf_queue(pMe->out_port[port_id].service_handle.gpQ, pMe->out_port[port_id].num_bufs_allocated);
         }
      }

      // call utility function to deinit cmd Q
      elite_svc_deinit_cmd_queue(pMe->service_handle.cmdQ);

      //destroy channel
      qurt_elite_channel_destroy(&pMe->channel);

      //free self state structure
      qurt_elite_memory_free (pMe);
      pMe = NULL;
   }
}


/*======================================================================

FUNCTION      stream_router_svc_destroy_yourself

DESCRIPTION
Invokes destroy of the stream router state structure
Returns ADSP_ETERMINATED to the caller to indicate that
destruction is done

DEPENDENCIES  none

RETURN VALUE  error code ADSP_ETERMINATED

SIDE EFFECTS  none

======================================================================*/
static ADSPResult stream_router_svc_destroy_yourself(stream_router_svc_t* pMe)
{
   elite_msg_any_t temp_cmd_msg = pMe->cmd_msg;

   stream_router_svc_destroy(pMe);

   elite_msg_finish_msg( &temp_cmd_msg, ADSP_EOK );

   // send ADSP_ETERMINATED so calling routine knows the destroyer has been invoked.
   return ADSP_ETERMINATED;
}

/*======================================================================

FUNCTION      stream_router_svc_workloop

DESCRIPTION   This function is the main work loop for the service. Commands
are handled with the highest priority. Data processing is handled only
when command queue is empty. This should be typical of most/all services.

The thread listens for command and data queues simultaneously (commands always prioritized).
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
int stream_router_svc_workloop(void* pInstance)
{
   stream_router_svc_t *pMe = (stream_router_svc_t*)pInstance;
   uint32_t unSigBit;
   ADSPResult result = ADSP_EFAILED;

   // set up mask for listening to the msg queues.
   // save for mask bitfields
   pMe->current_bitfield = STREAM_ROUTER_CMD_SIG;

   (void)stream_router_svc_register_with_adsppm(pMe);

   // Enter forever loop
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Entering Stream Router Svc workloop...");
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

         if(0 == unSigBit)
         {
            result = stream_router_svc_process_cmd_queue(pMe);
         }
         else if(0 == (unSigBit & 1))
         {
            //even bit positions are for input data queues
            //listen to input data queue only in TX stream router
            result = stream_router_svc_process_input_data_q(pMe, stream_router_svc_sigbit_to_port_id(unSigBit));
         }
         else
         {
            //odd positions greater than 1 are for output data queues
            result = stream_router_svc_process_output_data_q(pMe, stream_router_svc_sigbit_to_port_id(unSigBit));
         }

         if(result == ADSP_ETERMINATED)
         {
            result = ADSP_EOK;
            goto __bailout;
         }
      }
   }

   __bailout:
   return result;
}


/*======================================================================

FUNCTION      stream_router_svc_flush_input_data_q

DESCRIPTION   1) Frees any input data commands held on to
2) recursively pops and frees any msg in the
input data queue

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/
ADSPResult stream_router_svc_flush_input_data_q(stream_router_svc_t* pMe, uint32_t port_id)
{
   //first return any buffer being held on to
   //Handle flush here.
   if(pMe->in_port[port_id].data_msg.unOpCode == ELITE_DATA_MARK_BUFFER)
   {
      stream_router_svc_process_mark_buffer(pMe, port_id, ASM_DATA_EVENT_MARK_BUFFER_DISCARDED);
   }
   else if(pMe->in_port[port_id].data_msg.pPayload != NULL)
   {
      stream_router_svc_release_buffer(&pMe->in_port[port_id].data_msg);
   }

   //return any pending EOS messages, since for eos v2 we will not wait for any more eos acks
   uint32_t output_port_id = 0;
   uint32_t connected_output_ports = pMe->in_port[port_id].connected_output_ports;
   while(connected_output_ports)
   {
      if (connected_output_ports & 0x1)
      {
         if(NULL != pMe->out_port[output_port_id].eos_msg.pPayload)
         {
            stream_router_svc_release_buffer(&pMe->out_port[output_port_id].eos_msg);
         }
      }
      connected_output_ports >>= 1;
      output_port_id++;
   }

   //EoS is dropped after Flush.
   pMe->in_port[port_id].eos_pending_ack_mask = 0;

   elite_msg_any_t msg;
   // Drain any queued buffers
   while(ADSP_EOK == qurt_elite_queue_pop_front(pMe->in_port[port_id].service_handle.dataQ, (uint64_t*)&msg))
   {
      if(msg.unOpCode == ELITE_DATA_MARK_BUFFER)
      {
         //Raise mark buffer event and then release msg;
         pMe->in_port[port_id].data_msg = msg;
         stream_router_svc_process_mark_buffer(pMe, port_id, ASM_DATA_EVENT_MARK_BUFFER_DISCARDED);
      }
      else
      {
         stream_router_svc_release_buffer(&msg);
      }
   }

   return ADSP_EOK;
}

/*======================================================================

FUNCTION      stream_router_svc_process_cmd_queue

DESCRIPTION   pops a command from the command queue and
calls the appropriate function

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/
ADSPResult stream_router_svc_process_cmd_queue(stream_router_svc_t* pMe)
{

   ADSPResult result=ADSP_EOK;

   // Take next msg off the q
   result = qurt_elite_queue_pop_front(pMe->service_handle.cmdQ, (uint64_t*)&(pMe->cmd_msg) );

   // Process the msg
   if (ADSP_EOK == result)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_LOW_PRIO, " Message ID 0x%8x!!", (int)pMe->cmd_msg.unOpCode);
      /* Sanity check */
      const uint32_t cmd_table_size = sizeof(stream_router_cmd_handler_ptr)/sizeof(stream_router_cmd_handler_ptr[0]);
      if ( pMe->cmd_msg.unOpCode >= cmd_table_size )
      {
         return stream_router_svc_return_unsupported(pMe);
      }
      else
      {
         // table lookup to call handling function,
         result =  stream_router_cmd_handler_ptr[pMe->cmd_msg.unOpCode](pMe);
      }
   }

   return result;
}


/*======================================================================

FUNCTION      stream_router_svc_process_input_data_q

DESCRIPTION   call either the Rx or Tx router input data queue handler

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/
ADSPResult stream_router_svc_process_input_data_q(stream_router_svc_t* pMe, uint32_t port_id)
{
   ADSPResult nResult;
   if(RX_STRTR_ID == pMe->router_id)
   {
      nResult = stream_router_svc_process_input_data_q_rx_mode(pMe, port_id);
   }
   else
   {
      nResult = stream_router_svc_process_input_data_q_tx_mode(pMe, port_id);
   }
   return nResult;
}

/*======================================================================

FUNCTION      stream_router_svc_process_output_data_q

DESCRIPTION   call either the Rx or Tx router output data queue handler

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/
ADSPResult stream_router_svc_process_output_data_q(stream_router_svc_t* pMe, uint32_t port_id)
{
   ADSPResult nResult;
   if(RX_STRTR_ID == pMe->router_id)
   {
      nResult = stream_router_svc_process_output_data_q_rx_mode(pMe, port_id);
   }
   else
   {
      nResult = stream_router_svc_process_output_data_q_tx_mode(pMe, port_id);
   }
   return nResult;
}

/*======================================================================

FUNCTION      stream_router_svc_process_input_data_q_rx_mode

DESCRIPTION   pops a command from the input data queue and takes the
appropriate action based on the opcode

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/
ADSPResult stream_router_svc_process_input_data_q_rx_mode(stream_router_svc_t *pMe, uint32_t input_port_id)
{
   ADSPResult nResult = ADSP_EOK;
   uint32_t op_code;

   stream_router_input_port_t *in_port_ptr = &pMe->in_port[input_port_id];

   // Take next msg off the q only if we are not holding on to a data msg
   if(in_port_ptr->data_msg.pPayload == NULL)
   {
      if(ADSP_FAILED(nResult = qurt_elite_queue_pop_front(in_port_ptr->service_handle.dataQ,
            (uint64_t *)&in_port_ptr->data_msg)))
      {
         return nResult;
      }
   }
   else
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, " Already holding on to input buffer when receiving output buffer!\n");
      return ADSP_EOK;
   }

   op_code = in_port_ptr->data_msg.unOpCode;

   switch (op_code)
   {

   case ELITE_DATA_MEDIA_TYPE:
   {
      nResult = stream_router_svc_process_media_type(pMe, input_port_id, (elite_msg_data_media_type_apr_t *)in_port_ptr->data_msg.pPayload);
      //release the original media type message since we have made copies of it
      stream_router_svc_release_buffer(&in_port_ptr->data_msg);
      break;
   }

   case ELITE_DATA_EOS:
   {
      nResult = stream_router_svc_process_eos_msg(pMe, (elite_msg_data_eos_apr_t *)in_port_ptr->data_msg.pPayload,
            stream_router_svc_get_connected_active_out_ports(pMe, in_port_ptr),input_port_id);
      //release the original EOS message
      stream_router_svc_release_buffer(&in_port_ptr->data_msg);
      //stop listening to the input data queue till the EoS is sent to all downstream o/p ports.
      pMe->current_bitfield &= ~(stream_router_svc_input_port_to_signal_mask(input_port_id));
      break;
   }
   case ELITE_DATA_MARK_BUFFER:
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "StreamRouter: Received Mark Buffer from upstream!");
      //Raise the Mark Buffer Consumption Event
      stream_router_svc_process_mark_buffer(pMe, input_port_id, ASM_DATA_EVENT_MARK_BUFFER_PROCESSED);
      break;
   }

   case ELITE_DATA_BUFFER:
   {
      bool_t ts_valid;
      elite_msg_data_buffer_t *data_buf_ptr;
      //Do not process data buffer till media type message is received
      if(0 == in_port_ptr->sampling_rate)
      {
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, " sampling rate not defined");
         stream_router_svc_release_buffer(&in_port_ptr->data_msg);
         break;
      }
      //check if input buffer has valid timestamp
      data_buf_ptr = (elite_msg_data_buffer_t *)in_port_ptr->data_msg.pPayload;

      //log the input buffer data
      stream_router_svc_log_data(pMe, (int8_t *)&data_buf_ptr->nDataBuf, data_buf_ptr->nActualSize, in_port_ptr);

      ts_valid = (bool_t)asm_get_timestamp_valid_flag(data_buf_ptr->nFlag);
      if(!ts_valid)
      {
         in_port_ptr->ts_state.is_asynchronous_mode = TRUE;
      }

      //RUN immediate
      if(RUN == in_port_ptr->stream_state && in_port_ptr->ts_state.sync_to_next_ts)
      {
         if(!in_port_ptr->ts_state.is_asynchronous_mode)
         {
            //For run immediate, TS honoring mode, sync to next incoming buffer TS
            (void)stream_router_svc_update_session_clock_2(pMe, input_port_id, data_buf_ptr->ullTimeStamp);
            //These parameters are used for session time updates
            in_port_ptr->ts_state.initial_session_clock = data_buf_ptr->ullTimeStamp;
            in_port_ptr->ts_state.duration_rendered = 0;
            //Obtain the S2PDDrift pointer and calculate the drift
            int32_t input_data_length = data_buf_ptr->nActualSize * 1000000 / (in_port_ptr->sampling_rate * in_port_ptr->num_channels * (in_port_ptr->bits_per_sample / BITS_PER_BYTE)) ;
            (void)stream_router_svc_update_s2d_drift(pMe, input_port_id, input_data_length, data_buf_ptr->ullTimeStamp);

            /* Update the expected session clock if in resync state
             * This is needed to ensure that the buffer received after
             * resync is gated against the right value of expected session clock.
             * Basically this state is a reset point for the render decision */
            if(in_port_ptr->ts_state.is_resync)
            {
                (void)stream_router_svc_update_expected_session_clk(pMe, input_port_id, data_buf_ptr->ullTimeStamp);
                in_port_ptr->ts_state.is_resync = FALSE;

            }
         }

         in_port_ptr->ts_state.sync_to_next_ts = FALSE;
      }

      //Derive the samples per frame for this format from the first input buffer length
      if(!in_port_ptr->is_first_buffer_received)
      {
         //if data burst size has not been initialized yet
         if(0 == in_port_ptr->data_burst_duration)
         {
            uint64_t actual_data_size = (uint64_t) (data_buf_ptr->nActualSize);
            //there will be null frames (0 frames) at the beginning of the data stream. These are part of extended sync word.
            //there is 1 null frame (4 bytes) per channel. So for Audio sample packet 8 bytes of 0 and for HBR 32 bytes of 0s
            //These should be ignored for calculation of frame duration
            if(IEC_61937 == in_port_ptr->operation_mode)
            {
               actual_data_size -= in_port_ptr->num_channels * NUM_NULL_SYNC_FRAMES_PER_CHANNEL*IEC61937_BYTES_PER_IEC60958_FRAME;
            }
            //We will update both the data duration and also the remainder size;
            uint64_t numerator = actual_data_size * NUM_US_IN_SECOND;
            uint64_t denominator = (in_port_ptr->num_channels * in_port_ptr->sampling_rate * in_port_ptr->bits_per_sample / BITS_PER_BYTE);
            in_port_ptr->data_burst_duration = numerator / denominator;
            in_port_ptr->input_buffer_remainder_unit  = numerator % denominator;
         }
         in_port_ptr->is_first_buffer_received = TRUE;
      }

      //copy into each intermediate output buffer
      uint32_t temp_bitfield = stream_router_svc_get_connected_active_out_ports(pMe, in_port_ptr);
      uint32_t output_port_id = 0;
      while(temp_bitfield != 0)
      {
         if((temp_bitfield & 1) && (output_port_id < STRTR_MAX_PORTS))
         {
            stream_router_output_port_t *out_port_ptr = &pMe->out_port[output_port_id];
            if(NULL != out_port_ptr->intermediate_buff.address_ptr && out_port_ptr->intermediate_buff.max_size >= (uint32_t)data_buf_ptr->nActualSize)
            {
               memscpy(out_port_ptr->intermediate_buff.address_ptr, out_port_ptr->intermediate_buff.max_size, &data_buf_ptr->nDataBuf, data_buf_ptr->nActualSize);
               out_port_ptr->intermediate_buff.actual_size = data_buf_ptr->nActualSize;
               out_port_ptr->intermediate_buff.is_updated = TRUE;
            }
            else
            {
               //for 61937 compressed passthru if fmt is not given by client, the passthru CAPI pushes big buffers.
               MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "intermediate buffer not defined or small. needed (actual, max) (%lu, %lu). max intermediate size %lu.",
                     data_buf_ptr->nActualSize, data_buf_ptr->nMaxSize, out_port_ptr->intermediate_buff.max_size);
            }
         }
         temp_bitfield >>= 1;
         output_port_id++;
      }
      //save the timestamp of the input buffer
      in_port_ptr->ts_state.input_buffer_ts = data_buf_ptr->ullTimeStamp;

      //Obtain the S2PDDrift pointer and calculate the drift
      int32_t input_data_length = data_buf_ptr->nActualSize * 1000000 / (in_port_ptr->sampling_rate * in_port_ptr->num_channels * (in_port_ptr->bits_per_sample / BITS_PER_BYTE)) ;
      (void)stream_router_svc_update_s2d_drift(pMe, input_port_id, input_data_length, data_buf_ptr->ullTimeStamp);
      //return the data buffer to upstream service
      stream_router_svc_release_buffer(&in_port_ptr->data_msg);
      //stop listening to the input data queue till current data is consumed
      pMe->current_bitfield &= ~(stream_router_svc_input_port_to_signal_mask(input_port_id));
      break;
   }

   case ELITEMSG_DATA_RESYNC:
   {
       MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "StreamRouter: Received Resync command from upstream!");
       stream_router_svc_reset_session_clock(pMe,input_port_id);

       in_port_ptr->ts_state.initial_session_clock = 0;
       in_port_ptr->ts_state.duration_rendered = 0;
       in_port_ptr->ts_state.sync_to_next_ts = TRUE;
       in_port_ptr->ts_state.is_resync = TRUE;

       //return the data buffer to upstream service
       stream_router_svc_release_buffer(&in_port_ptr->data_msg);

       break;
   }

   default:
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Unexpected opCode for data command 0x%x", (int)op_code );
      stream_router_svc_release_buffer(&in_port_ptr->data_msg);
      return ADSP_EUNSUPPORTED;
   }

   } /* switch (op_code) */

   return nResult;
}

/*======================================================================

FUNCTION      stream_router_svc_process_input_data_q_tx_mode

DESCRIPTION   pops a msg from the input data queue and takes the
appropriate action based on the opcode

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/
ADSPResult stream_router_svc_process_input_data_q_tx_mode(stream_router_svc_t *pMe, uint32_t input_port_id)
{
   ADSPResult nResult = ADSP_EOK;
   uint32_t op_code;
   elite_msg_any_t data_msg;

   stream_router_input_port_t *in_port_ptr = &pMe->in_port[input_port_id];

   if(ADSP_FAILED(nResult = qurt_elite_queue_pop_front(in_port_ptr->service_handle.dataQ,
         (uint64_t*)&data_msg)))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Failed to pop message from input data Q");
      return ADSP_EFAILED;
   }

   op_code = data_msg.unOpCode;

   switch (op_code)
   {

   case ELITE_DATA_MEDIA_TYPE:
   {
      nResult = stream_router_svc_process_media_type(pMe, input_port_id, (elite_msg_data_media_type_apr_t *)data_msg.pPayload);
      //release the original media type message since we have made copies of it
      stream_router_svc_release_buffer(&data_msg);
      break;
   }

   case ELITE_DATA_EOS:
   {
      nResult = stream_router_svc_process_eos_msg(pMe, (elite_msg_data_eos_apr_t *)data_msg.pPayload,
            stream_router_svc_get_connected_active_out_ports(pMe, in_port_ptr), input_port_id);
      //release the original EOS message
      stream_router_svc_release_buffer(&data_msg);
      break;
   }

   case ELITE_DATA_BUFFER:
   {

      //if there is already and input buffer popped out, return it upstream
      if(NULL != in_port_ptr->data_msg.pPayload)
      {
         stream_router_svc_release_buffer(&in_port_ptr->data_msg);
      }
      in_port_ptr->data_msg = data_msg;

      elite_msg_data_buffer_t *data_buf_ptr = (elite_msg_data_buffer_t *)in_port_ptr->data_msg.pPayload;

      //log the input buffer, format id is unkown, so pass 0xFFFFF
      stream_router_svc_log_data(pMe, (int8_t *)&data_buf_ptr->nDataBuf, data_buf_ptr->nActualSize, in_port_ptr);

      //If registered, execute sniffer (sniffing has to be done even though Tx st rtr output is not connected yet.
      if(in_port_ptr->media_fmt_cb.registered)
      {
         stream_router_svc_sniff_media_format(pMe, input_port_id,(uint8_t *)&data_buf_ptr->nDataBuf, data_buf_ptr->nActualSize);
      }

      //TODO Do not process data buffer till media type message is received
      //no connected output ports, so return the buffer back upstream right away
      if(0 == stream_router_svc_get_connected_active_out_ports(pMe, in_port_ptr))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Tx: dropping input data as no connected active out ports (in port %lu). size %ld", input_port_id, data_buf_ptr->nActualSize);
         stream_router_svc_release_buffer(&in_port_ptr->data_msg);
         return ADSP_EOK;
      }

      uint32_t temp_bitfield = stream_router_svc_get_connected_active_out_ports(pMe, in_port_ptr);
      uint32_t output_port_id = 0;
      while(temp_bitfield != 0)
      {
         if(temp_bitfield & 1)
         {
            stream_router_output_port_t *out_port_ptr = &pMe->out_port[output_port_id];
            QURT_ELITE_ASSERT(out_port_ptr->downstream_svc_handle);

            //this is a new input buffer, so input buffer read positions of all connected active
            //output ports'intermediate buffers should be reset
            out_port_ptr->intermediate_buff.input_buf_read_pos = 0;

            if(out_port_ptr->intermediate_buff.actual_size == out_port_ptr->intermediate_buff.max_size)
            {
               MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Tx: intermediate buf is full. dropping: in port %lu, output %lu", input_port_id, output_port_id );
            }
            //copy the data from input to intermediate output buffer
            stream_router_svc_copy_from_input_to_intermediate_buffer(in_port_ptr, out_port_ptr);

            //if the intermediate buffer is full then listen for the corresponding output buffer
            if(out_port_ptr->intermediate_buff.actual_size == out_port_ptr->intermediate_buff.max_size)
            {
               pMe->current_bitfield |= stream_router_svc_output_port_to_signal_mask(output_port_id);
            }
         }
         temp_bitfield >>= 1;
         output_port_id++;
      }
      break;
   }

   default:
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Unexpected opCode for data command 0x%x", (int)op_code );
      stream_router_svc_release_buffer(&in_port_ptr->data_msg);
      return ADSP_EUNSUPPORTED;

   } /* switch (op_code) */

   return nResult;
}


/*======================================================================

FUNCTION      stream_router_svc_process_output_data_q_tx_mode

DESCRIPTION   pops a data buffer from output queue. checks if there is an
input buffer available. if so, copies whatever data is possible from input
to output buffer. if input buffer is empty returns it upstream. if output
buffer is full pushes it downstream.

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/
ADSPResult stream_router_svc_process_output_data_q_tx_mode(stream_router_svc_t *pMe, uint32_t output_port_id)
{
   ADSPResult nResult = ADSP_EOK;

   stream_router_output_port_t *out_port_ptr = &pMe->out_port[output_port_id];
   uint32_t input_port_id = out_port_ptr->connected_input_port;
   stream_router_input_port_t *in_port_ptr = &pMe->in_port[input_port_id];

   // Take next msg off the q
   nResult = qurt_elite_queue_pop_front(out_port_ptr->service_handle.gpQ, (uint64_t*)&out_port_ptr->data_msg);

   if(ADSP_EOK != nResult)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failure in popping output buffer ");
      return nResult;
   }

   elite_msg_data_buffer_t *out_buf_ptr = (elite_msg_data_buffer_t *)out_port_ptr->data_msg.pPayload;

   /** check if buffer max size matches. If not, free this buffer and create one. */
   nResult = stream_router_svc_check_recreate_out_buf(pMe, output_port_id, &out_buf_ptr);

   out_port_ptr->data_msg.pPayload = out_buf_ptr;

   if (NULL == out_buf_ptr)
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "returning since out buf is null ");
      return nResult;
   }
   if(ADSP_EOK != nResult)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failure in recreating output buffer ");
      return nResult;
   }

   //stop listening for an output buffer
   pMe->current_bitfield &= ~stream_router_svc_output_port_to_signal_mask(output_port_id);

   //copy data from intermediate buffer to output buffer
   memscpy(&out_buf_ptr->nDataBuf, out_buf_ptr->nMaxSize, out_port_ptr->intermediate_buff.address_ptr, out_port_ptr->intermediate_buff.actual_size);
   out_buf_ptr->nActualSize = out_port_ptr->intermediate_buff.actual_size;
   //mark the intermediate buffer as empty
   out_port_ptr->intermediate_buff.actual_size = 0;

   //if output buffer is full push it down
   if(ADSP_FAILED(nResult = stream_router_svc_push_output_buffer_downstream(in_port_ptr, out_port_ptr)))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Failed to deliver buffer dowstream");
      return nResult;
   }

   //copy over new data to intermediate buffer
   stream_router_svc_copy_from_input_to_intermediate_buffer(in_port_ptr, out_port_ptr);

   //if intermediate buffer is full listen for new output buffer
   if(out_port_ptr->intermediate_buff.actual_size == out_port_ptr->intermediate_buff.max_size)
   {
      pMe->current_bitfield |= stream_router_svc_output_port_to_signal_mask(output_port_id);
   }

   //if both intermediate buffer and input buffer are empty, check for EOS
   if(0 == out_port_ptr->intermediate_buff.actual_size)
   {
      //if there are any pending EOS's on this port send them down
      nResult = stream_router_svc_push_eos_downstream(pMe, output_port_id);
   }

   return nResult;
}

/*======================================================================

FUNCTION      stream_router_svc_return_unsupported

DESCRIPTION   Unsupported message handler. Returns the message to the
appropriate queue and raises an ADSP_EUNSUPPORTED ack

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/

static ADSPResult stream_router_svc_return_unsupported(stream_router_svc_t* pMe)
{
   //unsupported command..finish msg with error code
   MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Unsupported command with opcode = 0x%x", (int)pMe->cmd_msg.unOpCode);
   return elite_msg_finish_msg(&(pMe->cmd_msg), ADSP_EUNSUPPORTED);
}

/*======================================================================

FUNCTION      stream_router_svc_custom_msg_handler

DESCRIPTION   handles custom messages to stream router. calls the appropriate
handling function based on secondary opcode

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/
ADSPResult stream_router_svc_custom_msg_handler(stream_router_svc_t *pMe)
{
   ADSPResult result = ADSP_EOK;
   elite_msg_custom_header_t* payload_ptr = (elite_msg_custom_header_t*)((pMe->cmd_msg).pPayload);
   MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"rcvd custom msg 0x%lx, thread ID = %d",
         payload_ptr->unSecOpCode,
         pMe->service_handle.threadId);

   switch(payload_ptr->unSecOpCode)
   {
   case ELITEMSG_CUSTOM_STREAM_ROUTER_CONNECT:
      result = stream_router_svc_custom_connect(pMe);
      break;
   case ELITEMSG_CUSTOM_STREAM_ROUTER_DISCONNECT:
      result = stream_router_svc_custom_disconnect(pMe);
      break;
   case ELITEMSG_CUSTOM_STREAM_ROUTER_DISABLE_PORT:
      result = stream_router_svc_disable_port(pMe);
      break;
   case ELITEMSG_CUSTOM_STREAM_ROUTER_RUN:
      result = stream_router_svc_custom_run(pMe);
      break;
   case ELITEMSG_CUSTOM_STREAM_ROUTER_SET_PARAM:
      result = stream_router_svc_custom_set_param(pMe);
      break;
   case ELITEMSG_CUSTOM_STREAM_ROUTER_GET_PARAM:
      result = stream_router_svc_custom_get_param(pMe);
      break;
   case ELITEMSG_CUSTOM_STREAM_ROUTER_PAUSE:
      result = stream_router_svc_custom_pause(pMe);
      break;
   case ELITEMSG_CUSTOM_STREAM_ROUTER_FLUSH:
      result = stream_router_svc_custom_flush(pMe);
      break;
   case ELITEMSG_CUSTOM_STREAM_ROUTER_SUSPEND:
      result = stream_router_svc_custom_suspend(pMe);
      break;
   case ELITEMSG_CUSTOM_STREAM_ROUTER_MAP_ROUTINGS:
      result = stream_router_svc_custom_map_routings(pMe);
      break;
   case ELITEMSG_CUSTOM_STREAM_ROUTER_UNMAP_ROUTINGS:
      result = stream_router_svc_custom_unmap_routings(pMe);
      break;
   case ELITEMSG_CUSTOM_STREAM_ROUTER_REG_COMP_FMT_CHG:
      result = stream_router_svc_custom_reg_comp_fmt_chg(pMe);
      break;
   case ELITEMSG_CUSTOM_STREAM_ROUTER_SET_PRIMARY_PORT:
      result = stream_router_svc_custom_set_primary_port(pMe);
      break;

   default:
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "rcvd: invalid custom msg");
      result = elite_msg_finish_msg(&pMe->cmd_msg, ADSP_EBADPARAM);
      break;
   }
   return result;
}


/*======================================================================

FUNCTION      stream_router_svc_custom_run

DESCRIPTION   depending on RX or TX Stream Router calls appropriate Run
handler

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/
ADSPResult stream_router_svc_custom_run(stream_router_svc_t *pMe)
{
   if(RX_STRTR_ID == pMe->router_id)
   {
      return stream_router_svc_custom_run_rx_mode(pMe);
   }
   else
   {
      return stream_router_svc_custom_run_tx_mode(pMe);
   }
}

/*======================================================================

FUNCTION      stream_router_svc_custom_run_tx_mode

DESCRIPTION   changes the stream router state to RUN. Starts processing
input data buffers

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/
ADSPResult stream_router_svc_custom_run_tx_mode(stream_router_svc_t *pMe)
{

   EliteMsg_CustomStreamRouterRun *run_msg_ptr =
         (EliteMsg_CustomStreamRouterRun *)pMe->cmd_msg.pPayload;
   uint32_t port_id = run_msg_ptr->port_id;
   stream_router_output_port_t *out_port_ptr = &pMe->out_port[port_id];

   if(DISCONNECTED == out_port_ptr->stream_state)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " received RUN before connection complete");
      //cannot issue run till graph is established
      elite_msg_finish_msg(&pMe->cmd_msg, ADSP_ENOTREADY);
      return ADSP_ENOTREADY;
   }


   if (out_port_ptr->connected_input_port != STRTR_INVALID_PORT_ID)
   {
      uint32_t input_port_id = out_port_ptr->connected_input_port;
      //we will start listening to the output port only when the corresponding intermediate buffer is full

      //the corresponding input port is active (listening to buffers on the device side) as soon as it's connected.

      //add the output port to the active channel mask of the input port
      pMe->in_port[input_port_id].connected_active_output_ports |= (1 << port_id);

      //send media format message down.
      ADSPResult result = stream_router_svc_send_media_type_to_connected_output_ports(pMe, input_port_id, port_id);
      if (ADSP_FAILED(result))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " failed to send media fmt to output ports of input_port %lu",input_port_id);
      }
   }

   switch(run_msg_ptr->unStartFlag)
   {
   case ASM_SESSION_CMD_RUN_START_TIME_RUN_IMMEDIATE:
      out_port_ptr->stream_state = RUN;
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, " Run Immediate");
      break;
   default:
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Does not support this RUN mode %lu", run_msg_ptr->unStartFlag);
      elite_msg_finish_msg(&pMe->cmd_msg, ADSP_EUNSUPPORTED);
      return ADSP_EUNSUPPORTED;
   }

   out_port_ptr->kpps_vote = default_kpps;
   out_port_ptr->bw_vote = out_port_ptr->sampling_rate * out_port_ptr->num_channels * (out_port_ptr->bits_per_sample / BITS_PER_BYTE);
   if (0 == out_port_ptr->bw_vote)
   {
      out_port_ptr->bw_vote = default_bw;
   }
   stream_router_svc_process_kpps_bw(pMe, FALSE, FALSE);

   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, " Processed RUN");

   //ACK run command
   elite_msg_finish_msg(&pMe->cmd_msg, ADSP_EOK);

   return ADSP_EOK;
}

/*======================================================================

FUNCTION      stream_router_svc_custom_suspend

DESCRIPTION   changes the stream router state to suspend.
               releases hardware resources

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/
ADSPResult stream_router_svc_custom_suspend(stream_router_svc_t *pMe)
{
   EliteMsg_CustomStreamRouterSuspend* p_cmd_msg_payload =
         (EliteMsg_CustomStreamRouterSuspend*) ((pMe->cmd_msg).pPayload);
   uint32_t port_id = p_cmd_msg_payload->port_id;

   if(RX_STRTR_ID == pMe->router_id)
   {
      stream_router_input_port_t *in_port_ptr = &pMe->in_port[port_id];
      in_port_ptr->kpps_vote = 0;
      in_port_ptr->bw_vote = 0;
      stream_router_svc_process_kpps_bw(pMe, FALSE, FALSE);
   }
   else
   {
      stream_router_output_port_t *out_port_ptr = &pMe->out_port[port_id];
      out_port_ptr->kpps_vote = 0;
      out_port_ptr->bw_vote = 0;
      stream_router_svc_process_kpps_bw(pMe, FALSE, FALSE);
   }
   stream_router_svc_process_kpps_bw(pMe, TRUE, FALSE);

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, " entering suspend for port %lu\n", port_id);

   //ACK pause command
   elite_msg_finish_msg(&pMe->cmd_msg, ADSP_EOK);

   return ADSP_EOK;
}

/*======================================================================

FUNCTION      stream_router_svc_custom_pause

DESCRIPTION   changes the stream router state to PAUSE. Starts sending
pause data bursts

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/
ADSPResult stream_router_svc_custom_pause(stream_router_svc_t *pMe)
{
   EliteMsg_CustomStreamRouterPauseFlush* p_cmd_msg_payload =
         (EliteMsg_CustomStreamRouterPauseFlush*) ((pMe->cmd_msg).pPayload);
   uint32_t port_id = p_cmd_msg_payload->port_id;

   if(RX_STRTR_ID == pMe->router_id)
   {
      pMe->in_port[port_id].stream_state = PAUSE;
      //stop listening to data on input port
      pMe->current_bitfield &= ~stream_router_svc_input_port_to_signal_mask(port_id);
      pMe->in_port[port_id].is_first_buffer_received = FALSE;
   }
   else //TX STRTR
   {
      pMe->out_port[port_id].stream_state = PAUSE;
      //stop listening to output port
      pMe->current_bitfield &= ~stream_router_svc_output_port_to_signal_mask(port_id);
      //remove this output port from the active channel mask of corresponding input port
      uint32_t input_port_id = pMe->out_port[port_id].connected_input_port;
      pMe->in_port[input_port_id].connected_active_output_ports &= ~(1 << port_id);
   }

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, " entering PAUSE for port %lu\n", port_id);

   //ACK pause command
   elite_msg_finish_msg(&pMe->cmd_msg, ADSP_EOK);

   return ADSP_EOK;
}

/*======================================================================

FUNCTION      stream_router_service_flush

DESCRIPTION   Flush the input data queue

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/
ADSPResult stream_router_svc_custom_flush(stream_router_svc_t* pMe)
{
   ADSPResult result = ADSP_EOK;
   EliteMsg_CustomStreamRouterPauseFlush* p_cmd_msg_payload =
         (EliteMsg_CustomStreamRouterPauseFlush*) ((pMe->cmd_msg).pPayload);
   uint32_t port_id = p_cmd_msg_payload->port_id;

   if(RX_STRTR_ID == pMe->router_id)
   {
      if (pMe->in_port[port_id].service_handle.dataQ)
      {
         stream_router_svc_flush_input_data_q(pMe, port_id);
      }

      (void)stream_router_svc_reset_session_clock(pMe, port_id);
      pMe->in_port[port_id].ts_state.initial_session_clock = 0;
      pMe->in_port[port_id].ts_state.duration_rendered = 0;
   }
   else
   {
      result = ADSP_EUNSUPPORTED;
   }

   /* Generate ACK to Flush cmd */
   return elite_msg_finish_msg(&(pMe->cmd_msg), result);
}

/*======================================================================

FUNCTION      stream_router_svc_custom_connect

DESCRIPTION   processes the custom connect command to connect to either
stream side or AFE side.

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/
ADSPResult stream_router_svc_custom_connect(stream_router_svc_t* pMe)
{
   ADSPResult result = ADSP_EOK;
   EliteMsg_CustomStreamRouterConnect* p_cmd_msg_payload =
         (EliteMsg_CustomStreamRouterConnect*) ((pMe->cmd_msg).pPayload);
   uint32_t port_id = STRTR_INVALID_PORT_ID;
   //before input port is connected (Rx St Rtr), we send pause bursts with rep period of 3 (DEFAULT_PAUSE_REPETITION_PERIOD)
   //sending init pause burst helps in receiver synchronization.
   //with 0 as the media fmt, compressed media type msg is sent downstream.
   uint32_t media_fmt_id = 0;

   if(STRTR_PORT_DIR_INPUT == p_cmd_msg_payload->port_dir)
   {
      for(uint32_t i=0; i<STRTR_MAX_PORTS; i++)
      {
         if(!pMe->in_port[i].is_connected)
         {
            port_id = i;
            break;
         }
      }

      if(STRTR_INVALID_PORT_ID == port_id)
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Connection FAILED, No more vacant input ports.\n");
         result = ADSP_ENORESOURCE;
         goto __bailout;
      }

      //initialize input port parameters
      if(ADSP_EOK != (result = stream_router_svc_init_in_port(pMe, p_cmd_msg_payload, port_id)))
      {
         goto __bailout;
      }

      //create input buffer queue and add to channel
      if(ADSP_EOK != (result = stream_router_svc_init_in_buffer_queue(pMe, port_id)))
      {
         goto __bailout;
      }

      if(RX_STRTR_ID == pMe->router_id)
      {
         //connection established on the stream side
         //initialize avsync parameters
         if(ADSP_EOK != (result = stream_router_svc_init_s2d_drift(pMe, port_id)))
         {
            goto __bailout;
         }
         if(pMe->in_port[port_id].asm_cb_handle.pCbHandle == NULL)
         {
            pMe->in_port[port_id].asm_cb_handle = *(p_cmd_msg_payload->pEliteCb);

         }
         //connection established on the stream side
         pMe->in_port[port_id].stream_state = CONNECTED;
      }

      if(TX_STRTR_ID == pMe->router_id )
      {
         if (p_cmd_msg_payload->is_stream_to_be_connected)
         {
            //Port Id of corresponding output port contained in the message.
            uint32_t output_port_id = p_cmd_msg_payload->port_id;
            stream_router_svc_connect_input_to_output(pMe, port_id, output_port_id);
         }

         //start listening to input port as soon as it is connected, even if run is not issued on stream side.
         //this helps in sniffer logic and also in dropping stale data.
         pMe->current_bitfield |= stream_router_svc_input_port_to_signal_mask(port_id);

         pMe->in_port[port_id].kpps_vote = default_kpps;
         pMe->in_port[port_id].bw_vote = default_bw;
         stream_router_svc_process_kpps_bw(pMe, FALSE, FALSE);
      }

      pMe->in_port[port_id].data_log_id = p_cmd_msg_payload->data_log_id;
      //Need to return the service handle to upstream client
      p_cmd_msg_payload->svc_handle_ptr = &pMe->in_port[port_id].service_handle;
      p_cmd_msg_payload->port_id = port_id;
   }
   else if(STRTR_PORT_DIR_OUTPUT == p_cmd_msg_payload->port_dir)
   {
      for(uint32_t i=0; i<STRTR_MAX_PORTS; i++)
      {
         if(!pMe->out_port[i].is_connected)
         {
            port_id = i;
            break;
         }
      }

      if(STRTR_INVALID_PORT_ID == port_id)
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Connection FAILED, No more vacant output ports.\n");
         result = ADSP_ENORESOURCE;
         goto __bailout;
      }

      //initialize output port parameters
      if(ADSP_EOK != (result = stream_router_svc_init_out_port(pMe, p_cmd_msg_payload, port_id)))
      {
         goto __bailout;
      }

      if(TX_STRTR_ID == pMe->router_id)
      {
         //connection established on the stream side
         pMe->out_port[port_id].stream_state = CONNECTED;
         //-passing 0 for connected input port as input port format does not matter for
         //    tx stream router when allocating output buffers
         //-Tx router needs extra buffering in loopback path -which is identified by ASM and
         //    buffering is indicated through p_cmd_msg_payload->buffering_ms;

         //TODO: to make this sampling rate dependent.
         uint32_t max_num_buffers = p_cmd_msg_payload->buffering_ms/(TX_ROUTER_OUT_BUF_SIZE/(48*2*2)) + 1;
         //at least have MAX_OUT_DATA_Q_ELEMENTS elements
         max_num_buffers = (max_num_buffers>MAX_OUT_DATA_Q_ELEMENTS)?max_num_buffers:MAX_OUT_DATA_Q_ELEMENTS;

         if(ADSP_EOK != (result = stream_router_svc_init_out_buffer_queue(pMe, port_id, 0, max_num_buffers)))
         {
            goto __bailout;
         }
      }

      if(RX_STRTR_ID == pMe->router_id)
      {
         pMe->out_port[port_id].sampling_rate = p_cmd_msg_payload->sampling_rate;
         pMe->out_port[port_id].num_channels = p_cmd_msg_payload->num_channels;
         pMe->out_port[port_id].bits_per_sample = p_cmd_msg_payload->bits_per_sample;

         if(p_cmd_msg_payload->is_stream_to_be_connected)
         {
            //create output buffer queue and buffers only when connection to stream side is required
            //this is because there is a dependence on the stream side for the format information which
            //in turn decides the size of the output buffers
            uint32_t input_port_id = p_cmd_msg_payload->port_id;
            if(ADSP_EOK != (result = stream_router_svc_init_out_buffer_queue(pMe, port_id, input_port_id, MAX_OUT_DATA_Q_ELEMENTS)))
            {
               goto __bailout;
            }
            stream_router_svc_rx_mapping(pMe, input_port_id, port_id);

            media_fmt_id = pMe->in_port[input_port_id].format_id; //this can be zero initially for compressed 61937 passthru
         }

         //send default media format message with PCM as media format (or actual media fmt if inport is known).
         result = stream_router_svc_create_and_send_media_type(pMe, port_id,
               pMe->out_port[port_id].sampling_rate, pMe->out_port[port_id].num_channels,pMe->out_port[port_id].bits_per_sample, media_fmt_id);

         if(ADSP_EOK != result)
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO," Failed to push media type message downstream (OP Port:%lu)",port_id);
         }

         pMe->out_port[port_id].kpps_vote = default_kpps;
         pMe->out_port[port_id].bw_vote = default_bw;
         stream_router_svc_process_kpps_bw(pMe, FALSE, FALSE);
      }
      //respond with the service handle
      p_cmd_msg_payload->svc_handle_ptr = &(pMe->out_port[port_id].service_handle);
      p_cmd_msg_payload->port_id = port_id;
   }
   else
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Incorrect port direction in connect message\n");
      result = ADSP_EBADPARAM;
   }

   __bailout:
   elite_svc_send_ack( &(pMe->cmd_msg), result);

   MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, " connection port_id router Id %lu, dir (in 0/out 1) %lu, port_id %lu, is_stream_to_be_connected=%d, result %d",
         pMe->router_id, p_cmd_msg_payload->port_dir, p_cmd_msg_payload->port_id,p_cmd_msg_payload->is_stream_to_be_connected, result);

   //if error occurred do the necessary cleanup
   if((ADSP_EOK != result) && (STRTR_INVALID_PORT_ID != port_id))
   {
      if(STRTR_PORT_DIR_INPUT == p_cmd_msg_payload->port_dir)
      {
         stream_router_svc_deinit_in_port(pMe, port_id);
         stream_router_svc_destroy_in_buffer_queue(pMe, port_id);
      }
      else
      {
         stream_router_svc_deinit_out_port(pMe, port_id);
         stream_router_svc_destroy_out_buffer_queue(pMe, port_id, NO_POLLING_NEEDED);
      }
   }

   return result;
}

/*======================================================================

FUNCTION      stream_router_svc_custom_disconnect

DESCRIPTION   processes the custom disconnect command to disconnect from either
stream side or AFE side.

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/
ADSPResult stream_router_svc_custom_disconnect(stream_router_svc_t* pMe)
{
   ADSPResult result = ADSP_EOK;
   EliteMsg_CustomStreamRouterConnect* p_cmd_msg_payload =
         (EliteMsg_CustomStreamRouterConnect*) ((pMe->cmd_msg).pPayload);
   uint32_t port_id = p_cmd_msg_payload->port_id;

   if(STRTR_PORT_DIR_INPUT == p_cmd_msg_payload->port_dir)
   {
      if(!pMe->in_port[port_id].is_connected)
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Disconnection FAILED, input port is already disconnected\n");
         result = ADSP_EALREADY;
      }
      else
      {
         pMe->current_bitfield &= ~stream_router_svc_input_port_to_signal_mask(port_id);
         uint32_t output_port_id = 0;
         while(pMe->in_port[port_id].connected_output_ports)
         {
            if(pMe->in_port[port_id].connected_output_ports & 1)
            {
               pMe->out_port[output_port_id].connected_input_port = STRTR_INVALID_PORT_ID;

               //return any pending EOS messages, since for eos v2 we will not wait for any more eos acks
               if(NULL != pMe->out_port[output_port_id].eos_msg.pPayload)
               {
                  stream_router_svc_release_buffer(&pMe->out_port[output_port_id].eos_msg);
               }
            }
            pMe->in_port[port_id].connected_output_ports >>= 1;
            output_port_id++;
         }

         //deinitialize input port parameters
         result |= stream_router_svc_deinit_in_port(pMe, port_id);

         //destroy the input queue
         stream_router_svc_destroy_in_buffer_queue(pMe, port_id);

         if(RX_STRTR_ID == pMe->router_id)
         {
            //disconnected from stream side
            pMe->in_port[port_id].stream_state = DISCONNECTED;
         }
      }

      pMe->in_port[port_id].kpps_vote = 0;
      pMe->in_port[port_id].bw_vote = 0;
      stream_router_svc_process_kpps_bw(pMe, FALSE, FALSE);

      pMe->in_port[port_id].eos_pending_ack_mask = 0;  //rx st rtr : if stream side closes, eos can be dropped.

   }
   else if(STRTR_PORT_DIR_OUTPUT == p_cmd_msg_payload->port_dir)
   {
      if(!pMe->out_port[port_id].is_connected)
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Disconnection FAILED, output port is already disconnected.\n");
         result = ADSP_EALREADY;
      }
      else
      {
         pMe->current_bitfield &= ~stream_router_svc_output_port_to_signal_mask(port_id);

         //reset the corresponding bit in the input port connected_output_ports bitfield
         uint32_t input_port_id = pMe->out_port[port_id].connected_input_port;
         if(STRTR_INVALID_PORT_ID != input_port_id)
         {
            pMe->in_port[input_port_id].connected_output_ports &= ~(1UL << port_id);

            //check raise EoS at device close (rx st router)
            stream_router_svc_check_raise_rendered_eos_v2(pMe, NULL, input_port_id, port_id, FALSE);
         }

         //deinitialize output port parameters
         stream_router_svc_deinit_out_port(pMe, port_id);

         //destroy output buffer queue only if not already destroyed
         if(pMe->out_port[port_id].num_bufs_allocated > 0)
         {
            stream_router_svc_destroy_out_buffer_queue(pMe, port_id, NO_POLLING_NEEDED);
         }

         //return any pending EOS messages
         if(NULL != pMe->out_port[port_id].eos_msg.pPayload)
         {
            stream_router_svc_release_buffer(&pMe->out_port[port_id].eos_msg);
         }

         if(TX_STRTR_ID == pMe->router_id)
         {
            //disconnected from stream side
            pMe->out_port[port_id].stream_state = DISCONNECTED;
         }
      }

      pMe->out_port[port_id].kpps_vote = 0;
      pMe->out_port[port_id].bw_vote = 0;
      stream_router_svc_process_kpps_bw(pMe, FALSE, FALSE);
   }
   else
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Incorrect port direction in disconnect message\n");
      result = ADSP_EBADPARAM;
   }

   elite_svc_send_ack( &(pMe->cmd_msg), result);

   MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, " disconnection port_id router Id %lu, dir (in 0/out 1) %lu, port_id %lu, result %d",
         pMe->router_id, p_cmd_msg_payload->port_dir, p_cmd_msg_payload->port_id, result);

   return ADSP_EOK;
}


/*======================================================================

FUNCTION      stream_router_svc_map_single_connection

DESCRIPTION   establish connection between a given input port and given output port

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/
static ADSPResult stream_router_svc_map_single_connection(stream_router_svc_t* pMe, uint16_t input_port_id, uint16_t output_port_id)
{
   ADSPResult result = ADSP_EOK;
   stream_router_output_port_t *out_port_ptr;
   stream_router_input_port_t *in_port_ptr;
   uint32_t media_fmt_id;
   uint32_t sampling_rate;
   uint16_t num_channels;
   uint16_t bits_per_sample;

   out_port_ptr = (stream_router_output_port_t *)&pMe->out_port[output_port_id];
   in_port_ptr = (stream_router_input_port_t *)&pMe->in_port[input_port_id];

   if(RX_STRTR_ID == pMe->router_id)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
            " Rx: maprouting input port %u, output port %u", input_port_id, output_port_id);

      //make sure output port is not already connected to a valid input port
      if(out_port_ptr->connected_input_port != STRTR_INVALID_PORT_ID)
      {
         //output port cannot accept connections from more than one input port
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
               " Received connection to output port %u which is already connected to an input port\n", output_port_id);
         result =  ADSP_EALREADY;
         return result;

      }

      //first check if the output buffers are already created. if already created then check if they are
      //same size as required for new stream (output buffer size is same as the data burst size for stream).
      //If so nothing more to be done. Otherwise destroy and recreate with new size
      if(out_port_ptr->num_bufs_allocated > 0)
      {
         uint32_t buff_size = 0;
         stream_router_svc_get_output_buffer_size(pMe, input_port_id, output_port_id, &buff_size);

         if(buff_size != out_port_ptr->output_buffer_size)
         {
            stream_router_svc_destroy_out_buffer_queue(pMe, output_port_id, POLLING_NEEDED);
            out_port_ptr->output_buffer_size = buff_size; //assign new buf size, so that if any buffer returned doesn't match this size, we can delete the buffer.
         }
      }

      //create the output buffer queue and buffers, (only if not already created)
      //if queue is already created, then MAX_OUT_DATA_Q_ELEMENTS is not used.
      result = stream_router_svc_init_out_buffer_queue(pMe, output_port_id, input_port_id, MAX_OUT_DATA_Q_ELEMENTS);
      if(ADSP_EOK != result)
      {
         return result;
      }

      //set the connection and also start listening to output port
      stream_router_svc_rx_mapping(pMe, input_port_id, output_port_id);

      //if the input port is already in RUN state then need to listen to input data queue
      if((RUN == in_port_ptr->stream_state) && (STRTR_INVALID_PORT_ID != in_port_ptr->primary_output_port))
      {
         pMe->current_bitfield |= stream_router_svc_input_port_to_signal_mask(input_port_id);
      }
      // for Rx stream router, use compressed format id (useful for the PPs)
      media_fmt_id = in_port_ptr->format_id;

      //send input media type message downstream if media type has already been determined on the input port
      //else send output media fmt.
      if((0 != in_port_ptr->sampling_rate) && (0 != in_port_ptr->num_channels) && (0 != in_port_ptr->bits_per_sample))
      {
         sampling_rate = in_port_ptr->sampling_rate; //must be same as output if set.
         num_channels = in_port_ptr->num_channels;
         bits_per_sample = in_port_ptr->bits_per_sample;
      }
      else
      {
         sampling_rate = out_port_ptr->sampling_rate;
         num_channels = out_port_ptr->num_channels;
         bits_per_sample = out_port_ptr->bits_per_sample;
      }
   }
   else
   {
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
            " Tx: maprouting input port %u, output port %u", input_port_id, output_port_id);

      //make sure output port is not already connected to a valid input port
      if(out_port_ptr->connected_input_port != STRTR_INVALID_PORT_ID)
      {
         //output port cannot accept connections from more than one input port
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
               " Received connection to output port %u which is already connected to an input port\n", output_port_id);
         result =  ADSP_EALREADY;
         return result;
      }

      stream_router_svc_connect_input_to_output(pMe, input_port_id, output_port_id);

      // for Tx stream router, use PCM since format id is not known.
      media_fmt_id = ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V2;

      sampling_rate = in_port_ptr->sampling_rate;
      num_channels = in_port_ptr->num_channels;
      bits_per_sample = in_port_ptr->bits_per_sample;

      if(RUN == out_port_ptr->stream_state)
      {
         //if stream was already run, then add this out port to connected out port mask of input.
         pMe->in_port[input_port_id].connected_active_output_ports |= (1 << output_port_id);
      }

   }

   uint32_t bw = sampling_rate*num_channels*(bits_per_sample / BITS_PER_BYTE);
   bw = (bw==0?default_bw:bw);
   in_port_ptr->kpps_vote = default_kpps;
   in_port_ptr->bw_vote = bw;
   out_port_ptr->kpps_vote = default_kpps;
   out_port_ptr->bw_vote = bw;
   stream_router_svc_process_kpps_bw(pMe, FALSE, FALSE);

   if((0 != sampling_rate) && (0 != num_channels))
   {
      result = stream_router_svc_create_and_send_media_type(pMe, output_port_id, sampling_rate, num_channels, bits_per_sample, media_fmt_id);
      if(ADSP_EOK != result)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO," Failed to push media type message downstream (IP Port:%u, OP Port:%u)",
               input_port_id, output_port_id);
      }
      //update the delay for media fmt update
      stream_router_svc_calculate_delay(pMe,output_port_id);
   }
   return result;
}

/*======================================================================

FUNCTION      stream_router_svc_unmap_single_connection

DESCRIPTION   remove the connection between the given input port and given output port

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/
static ADSPResult stream_router_svc_unmap_single_connection(stream_router_svc_t* pMe, uint16_t input_port_id, uint16_t output_port_id)
{
   ADSPResult result = ADSP_EOK;

   stream_router_output_port_t *out_port_ptr;
   stream_router_input_port_t *in_port_ptr;

   out_port_ptr = (stream_router_output_port_t *)&pMe->out_port[output_port_id];
   in_port_ptr = (stream_router_input_port_t *)&pMe->in_port[input_port_id];

   if(RX_STRTR_ID == pMe->router_id)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
            " Rx: unmaprouting input port %u, output port %u", input_port_id, output_port_id);

      result = stream_router_svc_disconnect_input_from_output(pMe, input_port_id, output_port_id);
      if(ADSP_EOK != result)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Given input port (%u) is not connected to given output port (%u)",
               input_port_id, output_port_id);
         result = ADSP_EFAILED;
         return result;
      }
      //destroy the output buffers
      stream_router_svc_destroy_out_buffer_queue(pMe, output_port_id, POLLING_NEEDED);
      //set output buffer size as a marker to indicate that no output buffer is required.
      pMe->out_port[output_port_id].output_buffer_size = 0;
      //stop listening on output buffer queue
      if (0 == pMe->out_port[output_port_id].num_bufs_allocated)
      {
         pMe->current_bitfield &= ~(stream_router_svc_output_port_to_signal_mask(output_port_id));
      }

      in_port_ptr->kpps_vote = 0;
      in_port_ptr->bw_vote = 0;
      stream_router_svc_process_kpps_bw(pMe, FALSE, FALSE);
   }
   else //TX STRTR
   {
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
            " Tx: unmaprouting input port %u, output port %u", input_port_id, output_port_id);

      result = stream_router_svc_disconnect_input_from_output(pMe, input_port_id, output_port_id);
      if(ADSP_EOK != result)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Given input port (%u) is not connected to given output port (%u)",
               input_port_id, output_port_id);
         result = ADSP_EFAILED;
         return result;
      }

      out_port_ptr->kpps_vote = 0;
      out_port_ptr->bw_vote = 0;
      stream_router_svc_process_kpps_bw(pMe, FALSE, FALSE);
   }

   return result;
}
/*======================================================================

FUNCTION      stream_router_svc_custom_map_routings

DESCRIPTION   establish connection between a given input port and given output port

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/
ADSPResult stream_router_svc_custom_map_routings(stream_router_svc_t* pMe)
{
   ADSPResult result = ADSP_EOK;
   EliteMsg_CustomStreamRouterMapRoutings* p_cmd_msg_payload =
         (EliteMsg_CustomStreamRouterMapRoutings *) ((pMe->cmd_msg).pPayload);

   uint16_t input_port_id, output_port_id;

   uint32_t i, j;
   uint16_t stream_port_id, device_port_id, min_ind, max_ind;
   uint16_t *device_end_point_id_ptr;

   enum {NO_CHANGE=0, CONNECT, DISCONNECT};
   uint16_t desired_conn_status[STRTR_MAX_PORTS]; //index indicates the output port id.

   adm_cmd_stream_device_map_routings_v5_t *map_routings_cmd_ptr =  p_cmd_msg_payload->pRoutingPayload;

   adm_stream_device_map_node_v5_t *map_node_ptr = (adm_stream_device_map_node_v5_t *)(map_routings_cmd_ptr + 1);

   /**
    * 1. connection has to be removed if already connected, but not mentioned in this new cmd.
    * 2. connection has to be established only if not already connected.
    */
   for(i = 0; i < map_routings_cmd_ptr->num_sessions; i++)
   {
      if ( (TX_STRTR_ID == pMe->router_id) && (map_node_ptr->num_device_end_points > 1) )
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Tx stream router supports only one device per stream. given %u",map_node_ptr->num_device_end_points);
         goto __bailout;
      }

      stream_port_id = map_node_ptr->session_id;

      memset(&desired_conn_status, NO_CHANGE, sizeof(desired_conn_status));

      if(RX_STRTR_ID == pMe->router_id)
      {
         //for every session mentioned, if output port is currently connected to this input, it is marked to be disconnected.
         //rx has one input going to multiple outputs.
         input_port_id = stream_port_id;
         for(j = 0; j < STRTR_MAX_PORTS; j++)
         {
            if (pMe->out_port[j].connected_input_port == input_port_id) //stream router out port has only one input (since no mixing).
            {
               desired_conn_status[j] = DISCONNECT;

               //return any pending EOS messages even if this port is going to be connected back below. this because EoS V2 event is raised at maprouting below.
               //the client won't get EoS ack for v1.
               if(NULL != pMe->out_port[j].eos_msg.pPayload)
               {
                  MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
                        " Rx: while unmaprouting input port %u, output port %u, EoS is freed. Client won't receive rendered_eos event for v1.", input_port_id, j);
                  stream_router_svc_release_buffer(&pMe->out_port[j].eos_msg);
               }
            }
         }

         if (pMe->in_port[input_port_id].eos_pending_ack_mask)
         {
            stream_router_svc_check_raise_rendered_eos_v2(pMe, NULL, input_port_id, 0, TRUE);
            pMe->in_port[input_port_id].eos_pending_ack_mask = 0;  //rx st rtr : for map routing, eos is raised.
         }
      }
      else
      {
         //for every session mentioned, if output port is connected to an input, it is marked to be disconnected.
         //tx has only one conn from input to output.
         output_port_id = stream_port_id;
         input_port_id = pMe->out_port[output_port_id].connected_input_port; //just for init
         if (pMe->out_port[output_port_id].connected_input_port != STRTR_INVALID_PORT_ID)
         {
            desired_conn_status[output_port_id] = DISCONNECT;
         }
      }

      //now go through the payload to see which of the connections can be kept, & which new conn has to be created.
      device_end_point_id_ptr = (uint16_t *)(map_node_ptr + 1);
      for(j = 0; j < map_node_ptr->num_device_end_points; j++) //for Tx, num_device_end_points is 1 (num inputs)
      {
         device_port_id = *device_end_point_id_ptr;
         device_end_point_id_ptr++;

         if(RX_STRTR_ID == pMe->router_id)
         {
            input_port_id = stream_port_id;
            output_port_id = device_port_id;
         }
         else
         {
            input_port_id = device_port_id;
            output_port_id = stream_port_id;
         }

         //sanity check on the output_port_id
         if(STRTR_MAX_PORTS <= output_port_id)
         {
             MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                   " Rx strtr maprouting invalid output_port_id %u", output_port_id);
        	 goto __bailout;
         }

         if(pMe->out_port[output_port_id].connected_input_port == input_port_id) //stream router out port has only one input (since no mixing).
         {
            desired_conn_status[output_port_id] = NO_CHANGE;
         }
         else
         {
            desired_conn_status[output_port_id] = CONNECT;
         }
      }

      if (RX_STRTR_ID == pMe->router_id)
      {
         min_ind = 0; max_ind = STRTR_MAX_PORTS-1;
      }
      else
      {
         min_ind = input_port_id; max_ind = input_port_id;
      }

      //establish connection or disconnect or no change.
      for(j = min_ind; j <= max_ind; j++)
      {
         if(RX_STRTR_ID == pMe->router_id)
         {
            input_port_id = stream_port_id;
            output_port_id = j;
         }
         else
         {
            input_port_id = j;
            output_port_id = stream_port_id;
         }

         switch (desired_conn_status[output_port_id])
         {
         case CONNECT:
         {
            if (ADSP_FAILED(result = stream_router_svc_map_single_connection(pMe, input_port_id, output_port_id)))
            {
               goto __bailout;
            }
            break;
         }
         case DISCONNECT:
         {
            if (ADSP_FAILED(result = stream_router_svc_unmap_single_connection(pMe, input_port_id, output_port_id)))
            {
               goto __bailout;
            }
            break;
         }
         default:
            break;
         }
      }

      //calculate the byte offset, if number of devices is odd, force 4 byte alignment
      uint32_t num_devices_rounded_up = (map_node_ptr->num_device_end_points & 1) ? (map_node_ptr->num_device_end_points + 1) :
            map_node_ptr->num_device_end_points;
      uint32_t byte_offset = sizeof(adm_stream_device_map_node_v5_t) + num_devices_rounded_up * sizeof(uint16_t);
      map_node_ptr = (adm_stream_device_map_node_v5_t *)((uint8_t *)map_node_ptr + byte_offset);
   }

   __bailout:
   return elite_msg_finish_msg(&(pMe->cmd_msg), result);
}


/*======================================================================

FUNCTION      stream_router_svc_custom_unmap_routings

DESCRIPTION   remove the connection between input ports and output ports

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/
ADSPResult stream_router_svc_custom_unmap_routings(stream_router_svc_t* pMe)
{
   ADSPResult result = ADSP_EOK;
   EliteMsg_CustomStreamRouterUnMapRoutings* p_cmd_msg_payload =
         (EliteMsg_CustomStreamRouterUnMapRoutings *) ((pMe->cmd_msg).pPayload);

   uint32_t i, j;
   uint16_t stream_port_id, device_port_id;
   uint16_t input_port_id, output_port_id;
   uint16_t *device_end_point_id_ptr;

   adm_cmd_stream_device_unmap_routings_v5_t *unmap_routings_cmd_ptr =  p_cmd_msg_payload->pRoutingPayload;

   adm_stream_device_unmap_node_v5_t *unmap_node_ptr = (adm_stream_device_unmap_node_v5_t *)(unmap_routings_cmd_ptr + 1);

   for(i = 0; i < unmap_routings_cmd_ptr->num_sessions; i++)
   {
      stream_port_id = unmap_node_ptr->session_id; //ADM maps session id to port id.

      device_end_point_id_ptr = (uint16_t *)(unmap_node_ptr + 1);
      for(j = 0; j < unmap_node_ptr->num_device_end_points; j++)
      {
         device_port_id = *device_end_point_id_ptr; //ADM maps device id to port id.

         if(RX_STRTR_ID == pMe->router_id)
         {
            input_port_id = stream_port_id;
            output_port_id = device_port_id;
         }
         else
         {
            input_port_id = device_port_id;
            output_port_id = stream_port_id;
         }

         if (ADSP_FAILED(result = stream_router_svc_unmap_single_connection(pMe, input_port_id, output_port_id)))
         {
            goto __bailout;
         }

         device_end_point_id_ptr++;
      }

      //calculate the byte offset, if number of devices is odd, force 4 byte alignment
      uint32_t num_devices_rounded_up = (unmap_node_ptr->num_device_end_points & 1) ? (unmap_node_ptr->num_device_end_points + 1) :
            unmap_node_ptr->num_device_end_points;
      uint32_t byte_offset = sizeof(adm_stream_device_unmap_node_v5_t) + num_devices_rounded_up * sizeof(uint16_t);
      unmap_node_ptr = (adm_stream_device_unmap_node_v5_t *)((uint8_t *)unmap_node_ptr + byte_offset);
   }


   __bailout:
   return elite_msg_finish_msg(&(pMe->cmd_msg), result);
}

/*======================================================================

FUNCTION      stream_router_svc_custom_reg_comp_fmt_chg

DESCRIPTION   register/deregister compressed media format changes.

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/
ADSPResult stream_router_svc_custom_reg_comp_fmt_chg(stream_router_svc_t* pMe)
{
   ADSPResult result = ADSP_EOK;
   uint16_t port_id = 0;
   EliteMsg_CustomCompressedFmtReg* p_cmd_msg_payload =
         (EliteMsg_CustomCompressedFmtReg *) ((pMe->cmd_msg).pPayload);

   port_id = p_cmd_msg_payload->port_id;

   if(port_id >= STRTR_MAX_PORTS || p_cmd_msg_payload->enable == pMe->in_port[port_id].media_fmt_cb.registered)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "ADM_CMD_REGISTER_IEC_61937_FMT_UPDATE cmd failed, port id 0x%x, registered 0x%x",
            port_id,
            p_cmd_msg_payload->enable);
      result = ADSP_EFAILED;
      goto __bailout;
   }
   pMe->in_port[port_id].media_fmt_cb.registered = p_cmd_msg_payload->enable;
   pMe->in_port[port_id].media_fmt_cb.dest_addr = p_cmd_msg_payload->dest_addr;
   pMe->in_port[port_id].media_fmt_cb.dest_port = p_cmd_msg_payload->dest_port;
   pMe->in_port[port_id].media_fmt_cb.src_addr = p_cmd_msg_payload->src_addr;
   pMe->in_port[port_id].media_fmt_cb.src_port = p_cmd_msg_payload->src_port;
   __bailout:
   elite_svc_send_ack( &(pMe->cmd_msg), result);
   return result;

}

/*======================================================================

FUNCTION      stream_router_svc_disable_port

DESCRIPTION   stop listening to data on output data queue

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/
ADSPResult stream_router_svc_disable_port(stream_router_svc_t *pMe)
{
   EliteMsg_CustomStreamRouterDisablePort *disable_msg_ptr =
         (EliteMsg_CustomStreamRouterDisablePort *)pMe->cmd_msg.pPayload;
   uint32_t port_id = disable_msg_ptr->port_id;

   //stop listening to output data queue
   pMe->current_bitfield &= ~stream_router_svc_output_port_to_signal_mask(port_id);

   //push back any buffer we are holding on to
   if(NULL != pMe->out_port[port_id].data_msg.pPayload)
   {
      elite_msg_push_payload_to_returnq(pMe->out_port[port_id].service_handle.gpQ,
            (elite_msg_any_payload_t*) pMe->out_port[port_id].data_msg.pPayload);
      pMe->out_port[port_id].data_msg.pPayload = NULL;
   }

   //remove the output port from the active connected port list of the corresponding input port
   uint32_t input_port_id = pMe->out_port[port_id].connected_input_port;
   if(input_port_id != STRTR_INVALID_PORT_ID)
   {
      pMe->in_port[input_port_id].connected_active_output_ports &= ~(1UL << port_id);
   }

   //send the response back to ADM
   elite_msg_finish_msg(&pMe->cmd_msg, ADSP_EOK);

   return ADSP_EOK;
}

ADSPResult stream_router_svc_custom_set_primary_port(stream_router_svc_t *pMe)
{
   ADSPResult result = ADSP_EOK;
   EliteMsg_CustomSetStrtrPrimaryPortType *payload_ptr = (EliteMsg_CustomSetStrtrPrimaryPortType*)pMe->cmd_msg.pPayload;
   uint32_t num_sessions = 0;
   uint8_t *scratch_ptr = NULL;
   EliteMsg_CustomComprPrimaryPortMappingInfoType *mapping_payload_ptr = NULL;
   stream_router_input_port_t *in_port_ptr = NULL;

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Stream router: Processing cmd %lu (SetPrimayOutputPort)",
         payload_ptr->unSecOpCode);

   //derive number of sessions
   num_sessions = payload_ptr->unNumSessions;

   //derive mapping information
   scratch_ptr = (uint8_t*)(pMe->cmd_msg.pPayload);
   scratch_ptr = scratch_ptr + sizeof(EliteMsg_CustomSetStrtrPrimaryPortType);
   mapping_payload_ptr = (EliteMsg_CustomComprPrimaryPortMappingInfoType*)scratch_ptr;

   //update primary port information of input ports
   while(num_sessions)
   {
      in_port_ptr =  &pMe->in_port[mapping_payload_ptr->unInputPort];
      in_port_ptr->primary_output_port = mapping_payload_ptr->unPrimaryOutputPort;
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Stream router: Input Port [%u] maps to primary output port [%u]",
            mapping_payload_ptr->unInputPort, mapping_payload_ptr->unPrimaryOutputPort);

      (void)stream_router_svc_set_in_out_drift_ptr(pMe, mapping_payload_ptr->unInputPort, in_port_ptr->primary_output_port);

      uint32_t input_port_id;
      input_port_id = mapping_payload_ptr->unInputPort;

      //if the input port is already in RUN state then need to listen to input data queue
      if(RUN == in_port_ptr->stream_state)
      {
         pMe->current_bitfield |= stream_router_svc_input_port_to_signal_mask(input_port_id);
      }

      mapping_payload_ptr++;
      num_sessions--;
   }
   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "StreamRouter: Leaving cmd [%lu] (SetPrimayOutputPort) handler with status %d", payload_ptr->unSecOpCode, (int)result);
   elite_msg_finish_msg(&pMe->cmd_msg, result);
   return result;
}

ADSPResult stream_router_svc_custom_set_param(stream_router_svc_t* pMe)
{
   ADSPResult result = ADSP_EOK;

   EliteMsg_CustomStreamRouterGetSetParam *param_msg_ptr =
         (EliteMsg_CustomStreamRouterGetSetParam *)pMe->cmd_msg.pPayload;
   uint32_t port_id = param_msg_ptr->unPortID;

   if(ELITEMSG_PARAM_ID_CAL != param_msg_ptr->unParamId)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " SetParam failed. ParamId 0x%lx not supported",
            param_msg_ptr->unParamId);
      return elite_msg_finish_msg(&(pMe->cmd_msg), ADSP_EBADPARAM);
   }

   uint16_t payload_size = param_msg_ptr->unSize;
   uint8_t *payload_ptr = (uint8_t *)param_msg_ptr->pnParamData;

   if(NULL == payload_ptr)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " SetParam failed. Invalid payload address");
      return ADSP_EBADPARAM;
   }

   while(payload_size >= sizeof(asm_session_param_data_v2_t))
   {
      ADSPResult temp_res;
      asm_session_param_data_v2_t * param_data_ptr = (asm_session_param_data_v2_t *)payload_ptr;

      if (payload_size < (sizeof(asm_session_param_data_v2_t) + param_data_ptr->param_size))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Insufficient size, payload size=%u, Param size=%u", payload_size, param_data_ptr->param_size);
         result = ADSP_ENEEDMORE;
         break;
      }

      switch(param_data_ptr->module_id)
      {
      case ASM_SESSION_MTMX_STRTR_MODULE_ID_AVSYNC:
         temp_res = stream_router_svc_set_avsync_params(pMe, param_data_ptr->param_id, (void *)(param_data_ptr + 1), param_data_ptr->param_size, port_id);
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Received avsync set param command, result %d",temp_res);
         break;
      default:
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Module Id 0x%lx not present", param_data_ptr->module_id);
         temp_res = ADSP_EBADPARAM;
      }

      payload_ptr += (sizeof(asm_session_param_data_v2_t) + param_data_ptr->param_size);
      payload_size -= (sizeof(asm_session_param_data_v2_t) + param_data_ptr->param_size);

      if(ADSP_EOK != temp_res)
      {
         result = ADSP_EFAILED;
      }
   }

   elite_svc_send_ack( &(pMe->cmd_msg), result);

   return result;
}

ADSPResult stream_router_svc_custom_get_param(stream_router_svc_t* pMe)
{
   ADSPResult result = ADSP_EOK;

   EliteMsg_CustomStreamRouterGetSetParam *param_msg_ptr =
         (EliteMsg_CustomStreamRouterGetSetParam *)pMe->cmd_msg.pPayload;

   if(ELITEMSG_PARAM_ID_CAL != param_msg_ptr->unParamId)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " GetParam failed. ParamId 0x%lx not supported",
            param_msg_ptr->unParamId);
      return elite_msg_finish_msg(&(pMe->cmd_msg), ADSP_EBADPARAM);
   }

   if(NULL == param_msg_ptr->pnParamData)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " SetParam failed. Invalid payload address");
      return ADSP_EBADPARAM;
   }

   asm_session_param_data_v2_t * param_data_ptr = (asm_session_param_data_v2_t *)param_msg_ptr->pnParamData;

   if (param_msg_ptr->unSize < sizeof(asm_session_param_data_v2_t))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Insufficient size, payload size=%u, Param size=%u",
            param_msg_ptr->unSize, param_data_ptr->param_size);
      return elite_msg_finish_msg(&(pMe->cmd_msg), ADSP_ENEEDMORE);
   }

   switch(param_data_ptr->module_id)
   {
   case ASM_SESSION_MTMX_STRTR_MODULE_ID_AVSYNC:
      result = stream_router_svc_get_avsync_params(pMe);
      param_msg_ptr->unSize = sizeof(asm_session_param_data_v2_t) +  param_data_ptr->param_size;
      break;
   default:
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Module Id 0x%lx not present", param_data_ptr->module_id);
      result = ADSP_EBADPARAM;
   }

   elite_svc_send_ack( &(pMe->cmd_msg), result);

   return result;
}

static ADSPResult stream_router_svc_eos_v2_resp_handler(stream_router_svc_t *pMe)
{
   elite_msg_any_t* pMsg = &pMe->cmd_msg;
   MSG(MSG_SSID_QDSP6, DBG_MED_PRIO,"Stream router : received EoS resp");
   return stream_router_svc_check_raise_rendered_eos_v2(pMe, pMsg, 0, 0, FALSE);
}

