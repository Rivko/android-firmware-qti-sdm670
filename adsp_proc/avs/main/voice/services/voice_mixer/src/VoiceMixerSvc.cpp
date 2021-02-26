/*========================================================================

*//** @file VoiceMixerSvc.cpp

Copyright (c) 2010-2017, 2020 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

Matrix mixer for voice graph. An instance of this service is created statically in tx and rx paths.
The matrix mixer allows routing of arbitrary inputs to arbitrary outputs.


*//*====================================================================== */

/*========================================================================
  Edit History

  $Header: //components/rel/avs.adsp/2.8.5/voice/services/voice_mixer/src/VoiceMixerSvc.cpp#2 $

  when       who     what, where, why
  --------   ---     -------------------------------------------------------
  05/01/10   ksa     Created file

  ========================================================================== */


/* =======================================================================
   INCLUDE FILES FOR MODULE
   ========================================================================== */
#include "qurt_elite.h"
#include "Elite.h"
#include "VoiceMixerSvc.h"
#include "VMtMx.h"
#include "VoiceCmnUtils.h"
#include "adsp_media_fmt.h"
extern "C" {
#include "limiter24_api.h"
}
/*
TODO
proper buf sizes using inPortBufSize
non blocking output port bufq destroy
change sampling rates to 8000/16000
unified mallocs for i/p and o/p ports
*/

#define BITS_PER_SAMPLE 16
#define NO_OF_CHANNELS  1       // MONO. It is used to allocate memory for Limiter buffers.
#define SAMPLING_RATE_48K 48000 // worst case for limiter memory allocation. Because Sampling rate for mixer service is not known at create time.
#define NO_BYPASS 0             // Disable bypass mode in Limiter. ( 0 - disable bypass mode, 1 - enable bypass mode )
/*--------------------------------------------------------------*/
/* Macro definitions                                            */
/* -------------------------------------------------------------*/

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
qurt_elite_atomic_word_t vmx_instance_counter;
void* voice_matrices[2];
/* -----------------------------------------------------------------------
** Function prototypes
** ----------------------------------------------------------------------- */

// Main work loop for service thread. Pulls msgs off of queues and processes them.
static int vmx_workloop(void* instance_ptr);

// message handler functions
static ADSPResult vmx_cfg_input_ports(void *instance_ptr, elite_msg_any_t* pMsg);
static ADSPResult vmx_cfg_output_ports(void *instance_ptr, elite_msg_any_t* pMsg);
static ADSPResult vmx_run(void *instance_ptr, elite_msg_any_t* pMsg);
static ADSPResult vmx_stop(void *instance_ptr, elite_msg_any_t* pMsg);
static ADSPResult vmx_custom_msg_handler(void *instance_ptr, elite_msg_any_t *pMsg);

static ADSPResult vmx_media_type_handler(void *instance_ptr, uint32_t in_port_id, elite_msg_any_t data_q_msg);

static int32_t vmx_get_req_sample_count(vmx_t *me, uint32_t in_port_id);
static ADSPResult vmx_fill_in_port_local_buf(vmx_t *me, uint32_t in_port_id, int32_t num_samples_req);
static ADSPResult vmx_accumulate_samples(vmx_t *me, uint32_t in_port_id);
static ADSPResult vmx_send_media_type_downstream(void* instance_ptr, uint32_t out_port_id);
static void populate_limiter_params(void* instance_ptr);
static uint8_t vmx_choose_resampler(uint32_t in_samp_rate, uint32_t out_samp_rate);
static ADSPResult voice_mixer_resampler_init(vmx_in_port_info_t *vmx_port_ptr,uint32_t in_samp_rate,uint32_t out_samp_rate1,uint32_t out_samp_rate2,uint32_t out_samp_rate3);
static inline void voice_saturate_to_16(int16_t* out, int32_t* in, uint32_t num_samples);
/* -----------------------------------------------------------------------
 ** Message handler f
 ** ----------------------------------------------------------------------- */
// Build the function pointer table for message handling. Some common handlers
// like ReturnSuccess() which ignores the command and returns success, and
// Unsupported() which returns an error and outputs an error message are useful
// for multiple functions that this service does not support.
// In this example, this service has no concept of state. so just return success
// when RUN or STOP is commanded.

static elite_svc_msg_handler_func vmx_cmd_handler[] =
{
   vmx_custom_msg_handler,         //0 - ELITE_CUSTOM_MSG
   elite_svc_unsupported,           //1 -
   elite_svc_unsupported,          //2 - ELITE_CMD_DESTROY_SERVICE
   elite_svc_unsupported,           //3 -
   elite_svc_unsupported,           //4 -
   elite_svc_unsupported,           //5 -
   elite_svc_unsupported,           //6 -
   elite_svc_unsupported,           //7 -
   elite_svc_unsupported,           //8 -
   elite_svc_unsupported,           //9 -
};

ADSPResult CreateVoiceMatrixMixerSvc (uint32_t inputParam, void **handle, VMtMxParams_t* vmx_params_ptr, uint32_t instance_id)
{
   uint32_t in_port_id;
   ADSPResult result;
   char_t thread_name[QURT_ELITE_DEFAULT_NAME_LEN];

   // allocate instance struct
   vmx_t *me = (vmx_t*) qurt_elite_memory_malloc( sizeof(vmx_t), QURT_ELITE_HEAP_DEFAULT);
   if (!me)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Failed to alloc mem for VMX struct !!");
      return ADSP_ENOMEMORY;
   }

   // zero out all the fields.
   memset(me, 0, sizeof(vmx_t));
   me->instance_id = instance_id;

   if (VMX_TX_INSTANCE != me->instance_id
         && VMX_RX_INSTANCE != me->instance_id)
   {
      MSG(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Instance ID is invalid, faulty state!! ");
      //fix for KW warning. proper error handling by deallocating memory.
      qurt_elite_memory_free(me);
      return ADSP_EUNEXPECTED;
   }

   MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: ======= Creating vmx svc instance #%lu 0x%p ========", me->instance_id, me);
   voice_matrices[me->instance_id-1] = (void*)me;
   qurt_elite_atomic_increment(&qurt_elite_globalstate.nMsgQs);

   snprintf(me->cmdQName, QURT_ELITE_DEFAULT_NAME_LEN, "vmx_cmdq0%lx", me->instance_id);
   /* Allocate memory for queue */
   me->service_handle.cmdQ = (qurt_elite_queue_t*) qurt_elite_memory_malloc(QURT_ELITE_QUEUE_GET_REQUIRED_BYTES (MAX_CMD_Q_ELEMENTS) , QURT_ELITE_HEAP_DEFAULT);

   if ( NULL == me->service_handle.cmdQ )
   {
       MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Failed to alloc mem for VMX queue !!");
       return ADSP_ENOMEMORY;
   }
   if (ADSP_FAILED(result = qurt_elite_queue_init(me->cmdQName, MAX_CMD_Q_ELEMENTS, me->service_handle.cmdQ)))
   {
      MSG(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Failed to create queues!! ");
      return result;
   }

   // set up channel
   qurt_elite_channel_init(&me->channel);
   if (ADSP_FAILED(result = qurt_elite_channel_addq(&me->channel, me->service_handle.cmdQ, 1 << CMDQ_BITPOS)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: ADSP_FAILED to add mqs to channel result = %d!!", result);
      return(result);
   }

   // populate me
   me->service_handle.unSvcId = ELITE_VOICE_MX_SVCID;
   me->service_handle.dataQ = NULL;
   me->active_in_ports_mask = 0;

   // set the params
   me->out_ports_frame_size = vmx_params_ptr->outPortsFrameSize;
   me->max_in_ports = vmx_params_ptr->maxInPorts;
   me->max_out_ports = vmx_params_ptr->maxOutPorts;

   // allocate memory for i/p and o/p port params structures
   me->in_port_params_ptr = (vmx_in_port_info_t*) qurt_elite_memory_malloc(sizeof(vmx_in_port_info_t) *
         me->max_in_ports, QURT_ELITE_HEAP_DEFAULT);
   if (!me->in_port_params_ptr)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP:  Couldn't allocate memory for i/p port params!!!");
      result = ADSP_ENOMEMORY;
      return result;
   }

   // reset memory for input ports
   memset( me->in_port_params_ptr,  0, sizeof(vmx_in_port_info_t)  * me->max_in_ports);

   me->out_port_params_ptr = (vmx_out_port_info_t*) qurt_elite_memory_malloc(sizeof(vmx_out_port_info_t) *
         me->max_out_ports, QURT_ELITE_HEAP_DEFAULT);
   if (!me->out_port_params_ptr)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP:  Couldn't allocate memory for o/p port params!!!");
      result = ADSP_ENOMEMORY;
      return result;
   }

   // reset memory for output ports
   memset( me->out_port_params_ptr, 0, sizeof(vmx_out_port_info_t) * me->max_out_ports);

   // Launch the thread
   snprintf(thread_name, QURT_ELITE_DEFAULT_NAME_LEN, "VMX%lu", me->instance_id);

   if (ADSP_FAILED(result = qurt_elite_thread_launch(&(me->service_handle.threadId), thread_name,
               NULL, VMX_THREAD_STACK_SIZE, elite_get_thrd_prio(ELITETHREAD_STAT_VOICE_MIXER_SVC_PRIO_ID), vmx_workloop, (void*)me,QURT_ELITE_HEAP_DEFAULT)))
   {
      MSG(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Failed to create Mixer thread!! ");
      return result;
   }
   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: !!!! thread_id of vmx #%lu is 0x%x !!!!!",
         me->instance_id, me->service_handle.threadId);

   // for now, equate the handles for each of the i/p ports with that of the svc.
   // later, when an i/p port is configured, create the dataQ and update its value
   // in the port handle.
   for (in_port_id = 0; in_port_id < me->max_in_ports; in_port_id++)
   {
      me->in_port_params_ptr[in_port_id].in_port_handle.port_handle = me->service_handle;
   }

   *handle = &(me->service_handle);

   // Initialize limiter interface paramters with the default values.It is done in Matrix svc create
   // because paramter values are same for all the sampling rate.
   populate_limiter_params(me);

   return ADSP_EOK;
}

/**
 * This function is the main work loop for the service.
 * Commands are handled with the highest priority.
 * Data processing is handled only when command queue is empty.
 */

static int vmx_workloop(void* instance_ptr)
{

   int32_t k, result, num_samples_req;
   int16_t* buf_ptr;
   int32_t* buf2_ptr;
   uint32_t i;
   vmx_t *me = (vmx_t*)instance_ptr;       // instance structure
   elite_msg_any_t data_q_msg;
   qurt_elite_bufmgr_node_t out_buf_mgr_node;
   elite_msg_any_t *peer_dataq_msg_ptr;
   uint32_t channel_status;                // bitfield returned from qurt_elite_channel_wait
   // indicating which blocked Q's have msgs.
   uint32_t cmd_bitfield;                  // channel_ptr sub-mask containing cmd Q.
   uint32_t out_port_id;
   uint32_t in_port_id;
   int32_t q_channel_bit = 0;

   for (i = 0; i < me->max_in_ports; i++)
   {
      // input does not go to any output.
      me->in_port_params_ptr[i].in_port_state = PORT_INACTIVE;
      // no output port waiting on this stream.
      me->in_port_params_ptr[i].output_req_pending_mask = 0;
   }

   for (i = 0; i < me->max_out_ports; i++)
   {
      // output port inactive.
      me->out_port_params_ptr[i].out_port_state = PORT_INACTIVE;
   }
   // Acquire handle to the channel so that it can be blocked upon.
   qurt_elite_channel_t *channel_ptr = qurt_elite_queue_get_channel(me->service_handle.cmdQ);

   // save for mask bitfields
   cmd_bitfield = 1 << CMDQ_BITPOS;
   me->current_bit_field = cmd_bitfield;

   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Entering Voice Mixer Service workloop...");

   // Enter forever loop
   for(;;)
   {
      // block on any one or more of selected queues to get a msg
      //MSG(MSG_SSID_QDSP6, DBG_LOW_PRIO,"VCP: vmx_workloop blocked");
      channel_status = qurt_elite_channel_wait(channel_ptr, me->current_bit_field);
      //MSG_1(MSG_SSID_QDSP6, DBG_LOW_PRIO,"VCP: vmx_workloop unblocked, channel_status = %#x ", channel_status);
      // Check if commands came in. Process all commands before
      // falling through and processing input and output ports.
      if (channel_status & cmd_bitfield)
      {
         // call common utility to process command queue in standard way.
         const uint32_t vmx_cmd_table_size = sizeof(vmx_cmd_handler)/sizeof(vmx_cmd_handler[0]);
         result = elite_svc_process_cmd_queue(instance_ptr, &(me->service_handle), vmx_cmd_handler, vmx_cmd_table_size);
         // If service has been destroyed, exit.
         if (ADSP_FAILED( result))
         {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: VMX %lu returned error %ld from cmd handler!", me->instance_id, result);
         }
      }
      /* if a cmd didn't unblock the thread, check if a buf Q or data Q caused the unblocking
         check if the unblocking was due to an input buffer
         at any of the i/p ports. i/p ports given more priority over o/p ports
         so that as soon as a new i/p buf is available, it's samples may be processed.
         If an o/p port, say pull, that was waiting on i/p samples were to be
         processed first, while a new i/p buf is ready, it would send down 0s.
         This is undesirable.
       */
      if ((q_channel_bit =
               (channel_status & me->data_bit_field & me->current_bit_field)))
      {
         /*
            INPUT PORT PROCESSING
          */

         // figure out which input port is unblocked
         vmx_in_port_info_t *curr_in_port_ptr;
         k = 31 - (voice_get_signal_pos( (q_channel_bit) ));
         in_port_id = k - INPORT_BITSTART;
         curr_in_port_ptr = &me->in_port_params_ptr[in_port_id];
         curr_in_port_ptr->num_buf_rcvd++;
#ifdef VOICE_MATRIX_DEBUG
         MSG_3(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: VMX #%lu i/p port %lu rcvd buffer that it was blocked on, buffer # %lu ",
               me->instance_id, in_port_id, curr_in_port_ptr->num_buf_rcvd);
#endif
         //clear this particular dataQ from channel_status
         channel_status ^= 1 << (in_port_id + INPORT_BITSTART);

         // PROCESS THE MSG ONLY IF PORT IS ACTIVE.
         if ((PORT_ACTIVE == curr_in_port_ptr->in_port_state) ||
               (PORT_READY == curr_in_port_ptr->in_port_state))
         {
            // Take next msg off the q
            result = qurt_elite_queue_pop_front(curr_in_port_ptr->in_port_handle.port_handle.dataQ,
                  (uint64_t*)&data_q_msg );
            curr_in_port_ptr->data_q_msg = data_q_msg;

            // DataQ could receive any one of multiple msgs like ELITE_DATA_BUFFER,
            // ELITE_DATA_MEDIA_TYPE. Use the opcode to determine
            // what kind of msg was received and process accordingly.
            switch (data_q_msg.unOpCode)
            {
               case ELITE_DATA_BUFFER:
                  {
                     if (PORT_ACTIVE == curr_in_port_ptr->in_port_state)
                     {
                        // Process buffer only if input is connected to at least one output, unless it's low priority
                        if(curr_in_port_ptr->priority != VMX_INPUT_LOW_PRIO &&
                              curr_in_port_ptr->input_mapping_mask == 0)
                        {
                           //Drop buffer
                           MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: VMX #%lu dropping buffer at unconnected input %lu", me->instance_id, in_port_id);
                           result = elite_msg_return_payload_buffer(&curr_in_port_ptr->data_q_msg);
                           break;
                        }

                        if ( !(curr_in_port_ptr->input_mapping_mask & me->active_out_ports_mask))
                        {
                          //Drop buffer
                          MSG_2( MSG_SSID_QDSP6, DBG_MED_PRIO,
                                 "VCP: VMX #%lu dropping buffer at stopped input %lu",
                                 me->instance_id, in_port_id);
                          result = elite_msg_return_payload_buffer(&curr_in_port_ptr->data_q_msg);
                          break;
                        }

                        if ( (curr_in_port_ptr->priority == VMX_INPUT_LOW_PRIO) && (me->active_in_ports_mask == me->low_priority_inputs_mask))
                        {
                          //Drop buffer
                          MSG_2( MSG_SSID_QDSP6, DBG_MED_PRIO,
                                 "VCP: VMX #%lu dropping buffer at low priority input %lu as high priority port is closed",
                                 me->instance_id, in_port_id);
                          result = elite_msg_return_payload_buffer(&curr_in_port_ptr->data_q_msg);
                          break;
                        }

                        // mark this buffer held
                        curr_in_port_ptr->in_buf_status = INPUT_BUFFER_HELD;
                        curr_in_port_ptr->input_buf_ptr = (elite_msg_data_buffer_t*)data_q_msg.pPayload;
                        // update remaining i/p port params.
                        curr_in_port_ptr->num_remaining_samples = (curr_in_port_ptr->input_buf_ptr->nActualSize) >> 1;
#ifdef VOICE_MATRIX_DEBUG
                        MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: VMX #%lu: Right off the queue, upstr svc o/p buf contains %d samples.",
                              me->instance_id, (int)curr_in_port_ptr->num_remaining_samples);
#endif

                        curr_in_port_ptr->current_sample_ptr = (int16_t*)&(curr_in_port_ptr->input_buf_ptr->nDataBuf);
                        curr_in_port_ptr->in_buf_token = curr_in_port_ptr->input_buf_ptr->unClientToken;
                        curr_in_port_ptr->ull_timestamp = curr_in_port_ptr->input_buf_ptr->ullTimeStamp;
                        num_samples_req = vmx_get_req_sample_count(me, in_port_id);
                        if(curr_in_port_ptr->num_remaining_samples < num_samples_req )
                        {
                           // Based on current assumptions for voice matrix, you should never have fewer samples than input buf size,
                           //Toss partial samples, release buffer and add to wait mask
                           MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: VMX #%lu had an input buffer \
                                 with insufficient samples = %d, dropping", \
                                 me->instance_id, (int)curr_in_port_ptr->num_remaining_samples);

                           result = elite_msg_return_payload_buffer(&curr_in_port_ptr->data_q_msg);
                           curr_in_port_ptr->in_buf_status = INPUT_BUFFER_RELEASED;
                           curr_in_port_ptr->num_remaining_samples = 0;
                           break;

                        }
                        else
                        {
                           vmx_fill_in_port_local_buf(me, in_port_id, num_samples_req);
                        }

                        // update num_remaining_samples to reflect the current # of
                        // new samples remaining in the upstream peer's output buffer.
                        curr_in_port_ptr->num_remaining_samples -= num_samples_req;
                        curr_in_port_ptr->current_sample_ptr += num_samples_req;
                        //if empty, release the buffer
                        if(0 == curr_in_port_ptr->num_remaining_samples)
                        {
                           result = elite_msg_return_payload_buffer(&data_q_msg);
                           curr_in_port_ptr->in_buf_status = INPUT_BUFFER_RELEASED;
                        }

#ifdef VOICE_MATRIX_DEBUG
                        MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: VMX #%lu: After copying samples, upstr o/p buf left with %d samples.",
                              me->instance_id, (int)curr_in_port_ptr->num_remaining_samples);
#endif
                        // no longer block on this i/p port's data Q
                        me->current_bit_field ^= (1 << (in_port_id + INPORT_BITSTART));
                     }
                     else
                     {
                        MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: VMX #%lu i/p port %lu received a buffer \
                              while ready!", me->instance_id, in_port_id);
                        if(ADSP_FAILED(result = elite_msg_return_payload_buffer(&data_q_msg)))
                        {
                           MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: VMX #%lu i/p port %lu buffer return \
                                 failed!!", me->instance_id, in_port_id);
                        }
                     }
                     break;
                  }

               case ELITE_DATA_MEDIA_TYPE:
                  {
                     vmx_media_type_handler(instance_ptr, in_port_id, data_q_msg);
                     curr_in_port_ptr->in_port_state = PORT_ACTIVE;
                     me->active_in_ports_mask |= 1 << in_port_id;
                     // Update output_req_pending_mask. This is required for cases where the sequence is
                     // stop i/p, o/p -> run i/p -> run o/p -> rcv i/p media type
                     uint32_t mask = curr_in_port_ptr->input_mapping_mask
                        & me->out_buffer_available_mask;
                     while (mask)
                     {
                        uint32_t bit_pos = voice_get_high_lsb(mask);
                        mask ^= (1 << bit_pos);
                        if(0 == ((1<<in_port_id) & me->out_port_params_ptr[bit_pos].data_available_mask))
                        {
                           /* This means no data right now, so can mark as pending */
                           curr_in_port_ptr->output_req_pending_mask |= (1 << bit_pos);
                        }
                     }
                     break;
                  }
               default:
                  {
                     //TODO: Shouldn't it be returned though?
                     MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: VMX Svc ignoring input buffer, not a valid dataQ msg!!");
                     break;
                  }
            }
         }
         else
         {
            // Drop all incoming buffers if stopped. Need to wait on this queue to make sure you don't block upstream peer
            if(PORT_STOPPED == curr_in_port_ptr->in_port_state)
            {
               MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: VMX #%lu i/p port %lu received a buffer while stopped!!!!", me->instance_id, in_port_id);
               if(ADSP_FAILED ( result = qurt_elite_queue_pop_front(curr_in_port_ptr->in_port_handle.port_handle.dataQ, (uint64_t*)&data_q_msg)))
               {
                  MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: VMX #%lu i/p port %lu pop front failed!!", me->instance_id, in_port_id);
               }
               // TODO: Remove this later! Throw fatal error on receiving media type, will help catch sequencing issues
               if(ELITE_DATA_MEDIA_TYPE == data_q_msg.unOpCode)
               {
                  MSG_2(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: VMX #%lu i/p port %lu received a mediatype while stopped!!!!", me->instance_id, in_port_id);
               }
               if(ADSP_FAILED(result = elite_msg_return_payload_buffer(&data_q_msg)))
               {
                  MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: VMX #%lu i/p port %lu buffer return failed!!", me->instance_id, in_port_id);
               }
            }
         }
      }
      // check if any output port received an output buffer.
      if ((q_channel_bit = (channel_status & me->buf_bit_field)))
      {
         /*
            OUTPUT PORT PROCESSING
          */

         // figure out which o/p port is unblocked.
         k = 31 - (Q6_R_cl0_R( (q_channel_bit) ));
         out_port_id = k - OUTPORT_BITSTART;
         me->out_port_params_ptr[out_port_id].num_buf_rcvd++;
#ifdef VOICE_MATRIX_DEBUG
         MSG_3(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: VMX #%lu RECEIVED O/P BUFFER #%u AT O/P PORT %lu ",
               me->instance_id, me->out_port_params_ptr[out_port_id].num_buf_rcvd, out_port_id);
#endif
         //clear this particular dataQ from channel_status
         channel_status ^= (1 << (out_port_id + OUTPORT_BITSTART));

         //No longer wait for this buffer
#ifdef VOICE_MATRIX_DEBUG
         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: VMX #%lu Bitfield WAS: %#x",me->instance_id, me->current_bit_field);
#endif
         me->current_bit_field ^= 1 << (out_port_id + OUTPORT_BITSTART);
#ifdef VOICE_MATRIX_DEBUG
         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: VMX #%lu Bitfield IS: %#x",me->instance_id, me->current_bit_field);
#endif
         // Mark this port as having a buffer
         me->out_buffer_available_mask |= 1 << out_port_id;
         // set the corresponding bit in output_req_pending_mask of all the i/p ports feeding to this o/p port.
         uint32_t act_in_ports = me->active_in_ports_mask;
         while(act_in_ports)
         {
            in_port_id = voice_get_high_lsb(act_in_ports);
            act_in_ports ^= 1 << in_port_id;
            me->in_port_params_ptr[in_port_id].output_req_pending_mask |= (me->in_port_params_ptr[in_port_id].input_mapping_mask & (1 << out_port_id));
         }

      }
      /*
         COMMON PROCESSING ANY TIME THE THREAD WAKES UP.
       */

#ifdef VOICE_MATRIX_DEBUG
      MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: VMX #%lu entering Common Processing Portion", me->instance_id);
#endif
      /* in this piece of code, two things have to be checked.
         If any o/p port has a data request pending i/p port(s) need to be processed and mixed
         into the o/p port's accumulation buffer. Then in case after the various i/p ports have been processed,
         check if all the high priority i/p ports feeding to it have completed processing the samples. If yes,
         this o/p port's output buffer can now be sent downstream.
       */
      // traverse through active i/p ports
      uint32_t act_in_ports = me->active_in_ports_mask;
      while(act_in_ports)
      {
         in_port_id = voice_get_high_lsb(act_in_ports);
         act_in_ports ^= 1 << in_port_id;
         /* Check if any output port is waiting for data from this input port */
         if(me->in_port_params_ptr[in_port_id].output_req_pending_mask)
         {
            // if yes, check if any new samples are available at this i/p port.
            if (me->in_port_params_ptr[in_port_id].acc_pending)
            {
               // accumulate samples into AccBufs of all o/p ports that have
               // request pending for data from this i/p port.
               vmx_in_port_info_t *curr_in_port_ptr = &me->in_port_params_ptr[in_port_id];
#ifdef VOICE_MATRIX_DEBUG
               MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: VMX #%lu: Accumulate samples to Accbufs of all pend req o/p ports.",
                     me->instance_id);
#endif

               if (ADSP_FAILED(result = vmx_accumulate_samples(me, in_port_id)))
               {
                  MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Accumulation failed in VMX");
               }

               // clear that i/p port's output_req_pending_mask.
               curr_in_port_ptr->output_req_pending_mask = 0;

               if (curr_in_port_ptr->in_buf_status == INPUT_BUFFER_HELD)
               {
                  // try refilling the local input buffer with new samples
                  // from the upstream peer's output buffer.
#ifdef VOICE_MATRIX_DEBUG
                  MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: VMX #%lu: Trying to refill local i/p buffer with new samples.",
                        me->instance_id);
                  MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: VMX #%lu: # of remaining samples in upstr svc o/p buf = %d.",
                        me->instance_id, curr_in_port_ptr->num_remaining_samples);
#endif
                  if(curr_in_port_ptr->num_remaining_samples < vmx_get_req_sample_count(me, in_port_id) )
                  {
                     // Based on current assumptions for voice matrix, you should never have fewer samples than input buf size,
                     //unless you have none, in which case you should have freed the buffer already
                     //Toss partial samples, release buffer and add to wait mask
                     MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: VMX #%lu had an input buffer \
                           with insufficient samples = %d, dropping", \
                           me->instance_id, (int)curr_in_port_ptr->num_remaining_samples);

                     result = elite_msg_return_payload_buffer(&curr_in_port_ptr->data_q_msg);
                     curr_in_port_ptr->in_buf_status = INPUT_BUFFER_RELEASED;
                     curr_in_port_ptr->num_remaining_samples = 0;
                  }
                  else
                  {
#ifdef VOICE_MATRIX_DEBUG
                     MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: VMX #%lu: upstr svc's o/p buf has sufficient samples for I/P port %lu",
                           me->instance_id, in_port_id);
#endif
                     /*
                        upstream peer's output buffer has sufficient samples. copy them into the local input buffer so that the
                        resampler is ready to process them.
                      */

                     num_samples_req = vmx_get_req_sample_count(me, in_port_id);

                     vmx_fill_in_port_local_buf(me, in_port_id, num_samples_req);

                     // update num_remaining_samples to reflect the current # of
                     // new samples remaining in the upstream peer's output buffer.
                     curr_in_port_ptr->num_remaining_samples -= num_samples_req;
                     curr_in_port_ptr->current_sample_ptr += num_samples_req;

                     if(0 == curr_in_port_ptr->num_remaining_samples)
                     {
                        result = elite_msg_return_payload_buffer(&curr_in_port_ptr->data_q_msg);
                        curr_in_port_ptr->in_buf_status = INPUT_BUFFER_RELEASED;
                     }

#ifdef VOICE_MATRIX_DEBUG
                     MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: VMX #%lu: after copying, # samples remaining in upstr svc o/p buf = %d.",
                           me->instance_id, curr_in_port_ptr->num_remaining_samples);
#endif
                  }
               }
            }
            else
            {
               /* The very first time this condition is checked, this scenario
                  will arise. At this point, add the dataQ to the channel
                  and block on it.
                */
#ifdef VOICE_MATRIX_DEBUG
               MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: VMX #%lu: I/P PORT %lu ADDING DATA Q TO BLOCKING MASK",
                     me->instance_id, in_port_id);
#endif
               me->current_bit_field |= (1 << (in_port_id + INPORT_BITSTART));
            }
         }
         else
         {
            /* If the output port goes to stop state just after accumulating the data,
             * output_req_pending_mask will not get enabled again and hence we need to keep listening to input dataQ
             * so that it can be returned.
             */
#ifdef VOICE_MATRIX_DEBUG
            MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: VMX #%lu: I/P PORT %lu ADDING DATA Q TO BLOCKING MASK",
                  me->instance_id, in_port_id);
#endif
            me->current_bit_field |= (1 << (in_port_id + INPORT_BITSTART));
         }
      } // while(act_in_ports)

      uint32_t act_out_ports = me->active_out_ports_mask;
      while(act_out_ports)
      {
         uint32_t out_port_id = voice_get_high_lsb(act_out_ports);
#ifdef VOICE_MATRIX_DEBUG
         MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: VMX %lu processing o/p port %lu", me->instance_id, out_port_id);
#endif
         act_out_ports ^= (1 << out_port_id);
         if(0 == me->out_port_params_ptr[out_port_id].data_available_mask)
         {
            continue;
         }
#ifdef VOICE_MATRIX_DEBUG

         MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: VMX %lu o/p port %lu ", me->instance_id, out_port_id);
         MSG_4(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: dam %#x omm %#x hpim %#x aipm %#x",
               me->out_port_params_ptr[out_port_id].data_available_mask,
               me->out_port_params_ptr[out_port_id].output_mapping_mask,
               me->high_priority_inputs_mask,
               me->active_in_ports_mask);
#endif

         /* Check if data is available from all high prio inputs
            This requires you to have 3 pieces of information:
            1. The inputs you expect data from
            2. Inputs which are active, since those are the only inputs you want to wait for
            3. Inputs which are high priority, since you must have data from all of them

            The data_available_mask should have atleast the set of bits defined by the above 3 masks set.
            However it is possible for it to also have more bits set if some inputs got detached or stopped
            while processing the current output buffer. Thus, those extra bits can be ignored in general,
            as long as all the expected bits are set.
          */
         uint32_t expected_mask = (me->out_port_params_ptr[out_port_id].output_mapping_mask
               & me->high_priority_inputs_mask
               & me->active_in_ports_mask);
         if(expected_mask == (me->out_port_params_ptr[out_port_id].data_available_mask & expected_mask))
         {
            // take next buffer off the q
#ifdef VOICE_MATRIX_DEBUG
            MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: VMX %lu o/p port %lu ready", me->instance_id, out_port_id);
#endif
            result = qurt_elite_queue_pop_front(me->out_port_params_ptr[out_port_id].bufQ, (uint64_t*)&out_buf_mgr_node);
            if(ADSP_ENEEDMORE == result)
            {
               //Should ideally not happen, but can happen in certain transient states
               MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Mixer PUSH port was ready without an output buffer!");
               continue;
            }
            elite_msg_data_buffer_t* output_buf_ptr = (elite_msg_data_buffer_t*)out_buf_mgr_node.pBuffer;

            // copy samples from o/p port's AccBuf to OutputBuf and clear AccBuf
            buf_ptr = (int16_t*)(&(output_buf_ptr->nDataBuf));
            buf2_ptr = me->out_port_params_ptr[out_port_id].acc_buf_ptr;

            if(VMX_TX_INSTANCE == me->instance_id)
            {
               voice_saturate_to_16(buf_ptr, buf2_ptr, me->out_port_params_ptr[out_port_id].buf_size);
            }
            else
            {
            uint32_t currMask = me->out_port_params_ptr[out_port_id].output_mapping_mask;
            // Test for power of 2. If power of 2, that means output is connected to a single i/p port
            // In this case, saturate to 14/16 bits on tx/rx path rather than call limiter
            if(0 == (currMask & (currMask-1)))
            {
                  voice_saturate_to_16(buf_ptr, buf2_ptr, me->out_port_params_ptr[out_port_id].buf_size);
               }
            else
            {
#ifdef VOICE_MATRIX_DEBUG
               MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Limter process is called");
#endif
               Lim_process((CLimiterLib*)me->out_port_params_ptr[out_port_id].limiter_ptr,(void*)buf_ptr, buf2_ptr,me->out_port_params_ptr[out_port_id].buf_size,0/*No BYPASS*/);

            }
            }

            memset(me->out_port_params_ptr[out_port_id].acc_buf_ptr, 0, DEFAULT_BUF_SIZE << 1);

            //below is new code for setting up output_buf_ptr
            output_buf_ptr->pBufferReturnQ = me->out_port_params_ptr[out_port_id].bufQ;
            output_buf_ptr->nFlag          = output_buf_ptr->nFlag | (0x80000000);
            output_buf_ptr->ullTimeStamp   = me->out_port_params_ptr[out_port_id].ull_timestamp;
            output_buf_ptr->nOffset        = 0;
            output_buf_ptr->nActualSize    = me->out_port_params_ptr[out_port_id].buf_size << 1;
            output_buf_ptr->nMaxSize       = me->out_port_params_ptr[out_port_id].buf_size << 1;
            me->out_port_params_ptr[out_port_id].num_buf_sent++;
            // send OutputBuf downstream and print message only for Rx instance
            if(VMX_RX_INSTANCE == me->instance_id)
            {
               MSG_4(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: VMX #%lu o/p port %lu sending buffer #%lu buf 0x%p downstream",
                     me->instance_id, out_port_id, me->out_port_params_ptr[out_port_id].num_buf_sent, output_buf_ptr);
		    }
            // send OutputBuf downstream.
            peer_dataq_msg_ptr = elite_msg_convt_buf_node_to_msg(
                  &out_buf_mgr_node,
                  ELITE_DATA_BUFFER,
                  NULL, /* do not need response */
                  me->out_port_params_ptr[out_port_id].out_buf_token,    /* token */
                  0     /* do not care response result */
                  );
            result = qurt_elite_queue_push_back(me->out_port_params_ptr[out_port_id].
                  downstream_peer_handle_ptr->dataQ, (uint64_t*)peer_dataq_msg_ptr );
            if (ADSP_FAILED(result))
            {
               MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Failed to deliver buffer dowstream. Dropping!");
               (void) elite_msg_push_payload_to_returnq(
                     me->out_port_params_ptr[out_port_id].bufQ, (elite_msg_any_payload_t*) out_buf_mgr_node.pBuffer);
            }
            // Add to bitfield to wait again on this buffer
#ifdef VOICE_MATRIX_DEBUG
            MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Delivered VMX #%lu Bitfield WAS: %#x",me->instance_id, me->current_bit_field);
#endif
            me->current_bit_field |= 1 << (out_port_id + OUTPORT_BITSTART);
#ifdef VOICE_MATRIX_DEBUG
            MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Delivered VMX #%lu Bitfield IS: %#x",me->instance_id, me->current_bit_field);
#endif
            // Update mask to reflect push ports that still have their requests pending.
            me->out_buffer_available_mask ^= 1 << out_port_id;
            //Remove from samples_available_mask
            me->out_port_params_ptr[out_port_id].data_available_mask = 0;
         }
      }

   } // for(;;)
   return 0;
} // vmx_workloop(void* instance_ptr)

/* =======================================================================
 **                          Message handler functions
 ** ======================================================================= */

static ADSPResult vmx_custom_msg_handler(void* instance_ptr, elite_msg_any_t* pMsg)
{
   ADSPResult result;
   elite_msg_custom_header_t* payload_ptr = (elite_msg_custom_header_t*)pMsg->pPayload;
   switch (payload_ptr->unSecOpCode)
   {
      case ELITEMSG_CUSTOM_VMX_CFG_INPUT_PORTS:
         {
            result = vmx_cfg_input_ports(instance_ptr, pMsg);
            break;
         }
      case ELITEMSG_CUSTOM_VMX_CFG_OUTPUT_PORTS:
         {
            result = vmx_cfg_output_ports(instance_ptr, pMsg);
            break;
         }
      case ELITEMSG_CUSTOM_VMX_RUN:
         {
            result = vmx_run(instance_ptr, pMsg);
            break;
         }
      case ELITEMSG_CUSTOM_VMX_STOP:
         {
            result = vmx_stop(instance_ptr, pMsg);
            break;
         }
      default:
         {
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Custom cmd rcvd by VMX Svc is invalid");
            result = ADSP_EBADPARAM;
            break;
         }

   }
   return result;
}

/**
  This function calculates the number of samples required to fill
  an input port's local buffer so that it may process one output
  frame.

  @param me [in] A pointer to instance struct of the matrix
  @param in_port_id [in] ID of the input port whose input buffer
  is to be filled.
  @return Number of samples required to fill the input port's
  local buffer
 */
static int32_t vmx_get_req_sample_count(vmx_t *me, uint32_t in_port_id)
{
   // This function returns the number of samples that should be copied in order
   // to fill the input port's local buffer.
   int32_t numSamples_t = me->out_ports_frame_size * me->in_port_params_ptr[in_port_id].sample_rate;
#ifdef VOICE_MATRIX_DEBUG
   MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: VMX #%lu: num_samples_req = %d",
         me->instance_id, numSamples_t);
#endif
   return numSamples_t;
}

/**
  This function copies samples from an upstream svc's output
  buffer into a local buffer of the input port that the upstream
  svc is connected to. The copy is such that the local buffer is
  full, ie, contains enough samples to process one output frame.

  @param me [in] A pointer to instance struct of the matrix
  @param in_port_id [in] ID of the input port whose local buffer
  is to be filled.
  @param num_samples_req [in] Number of samples required to fill the
  local buffer
  @return An indication of success or failure
 */

static ADSPResult vmx_fill_in_port_local_buf(vmx_t *me, uint32_t in_port_id, int32_t num_samples_req)
{
   ADSPResult result = ADSP_EOK;
#ifdef VOICE_MATRIX_DEBUG
   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: VMX %lu filling inport %lu", me->instance_id, in_port_id);
#endif
   memscpy((void*) me->in_port_params_ptr[in_port_id].loc_buffer_ptr,  me->in_port_params_ptr[in_port_id].in_port_buf_size, (void*) me->in_port_params_ptr[in_port_id].current_sample_ptr, num_samples_req<<1);
   //Mark as acc pending
   me->in_port_params_ptr[in_port_id].acc_pending = 1;
   return result;
}

/**
  This function accumulates samples from the local buffer of a
  particular input port into accBuf's of output ports that need
  servicing.

  @param me [in] A pointer to instance struct of the matrix
  @param in_port_id [in] ID of the input port whose samples will
  be accumulated.
  @return An indication of success or failure
 */

static ADSPResult vmx_accumulate_samples(vmx_t *me, uint32_t in_port_id)
{
   uint32_t i, j;
   int16_t *buf1_ptr;
   int32_t *buf2_ptr=NULL;
   uint32_t in_samp_rate, out_samp_rate, out_samp_req;
   uint8_t resamp_index;
   ADSPResult result = ADSP_EOK;
   vmx_in_port_info_t *currPort = &me->in_port_params_ptr[in_port_id];
   for (i = 0; i <= me->max_out_portID; i++)
   {
      if (currPort->output_req_pending_mask & (1 << i))
      {
         buf2_ptr = me->out_port_params_ptr[i].acc_buf_ptr;
         in_samp_rate = currPort->sample_rate;
         out_samp_rate = me->out_port_params_ptr[i].sample_rate;
         out_samp_req = out_samp_rate * me->out_ports_frame_size;
         if(in_samp_rate != out_samp_rate)
         {
            resamp_index = vmx_choose_resampler(in_samp_rate, out_samp_rate);

            if( NULL == currPort->resampler[resamp_index].channel_mem_ptr)
            {
                // allocate the resampler for default configuration
                uint32_t out_frame_samples;
                voice_resampler_set_config(&(currPort->resampler[resamp_index].resamp_config),
                        VOICE_SWB_SAMPLING_RATE,VOICE_FB_SAMPLING_RATE,NO_OF_BITS_PER_SAMPLE,VOICE_FRAME_SIZE_SWB,&out_frame_samples);

                currPort->resampler[resamp_index].channel_mem_size = (currPort->resampler[resamp_index].resamp_config).total_channel_mem_size;
                currPort->resampler[resamp_index].channel_mem_ptr = qurt_elite_memory_malloc(currPort->resampler[resamp_index].channel_mem_size, QURT_ELITE_HEAP_DEFAULT);
                if(NULL == currPort->resampler[resamp_index].channel_mem_ptr)
                {
                    MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Failed to allocate memory for resampler in VMX instance #%lu ", me->instance_id);
                    result = ADSP_ENOMEMORY;
                    return result;
                }
                // initialize resampler with right configuration
                uint32 resample_result;
                uint32_t in_frame_samples = (in_samp_rate * 1000)/ 50;
                voice_resampler_set_config(&(currPort->resampler[resamp_index].resamp_config),
                        (in_samp_rate*1000),(out_samp_rate*1000),NO_OF_BITS_PER_SAMPLE,in_frame_samples,&out_frame_samples);
                if( NULL != currPort->resampler[resamp_index].channel_mem_ptr)
                {
                    resample_result = voice_resampler_channel_init(
                            &(currPort->resampler[resamp_index].resamp_config),
                            currPort->resampler[resamp_index].channel_mem_ptr,
                            currPort->resampler[resamp_index].channel_mem_size
                    );
                    if(VOICE_RESAMPLE_SUCCESS != resample_result)
                    {
                        return ADSP_EFAILED;
                    }
                }
                MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: VMX %lu voice resmpler idx (%ld) created and initialized with"
                        "input SR (%ld), output SR(%ld)", me->instance_id,resamp_index, in_samp_rate,out_samp_rate);
            }


#ifdef VOICE_MATRIX_DEBUG
            MSG_3(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: VMX #%lu invoking resampler i/p %lu o/p %lu", me->instance_id, in_port_id, i);
#endif
            //Resample
            if( NULL != currPort->resampler[resamp_index].channel_mem_ptr)
            {
               int32_t resample_result;
               resample_result = voice_resampler_process(
                  &(currPort->resampler[resamp_index].resamp_config),
                  currPort->resampler[resamp_index].channel_mem_ptr,
                  (int8 *)currPort->loc_buffer_ptr,
                  (in_samp_rate * me->out_ports_frame_size),
                  (int8 *)currPort->resamp_buffer_ptr,
                  out_samp_req
                  );
               if(VOICE_RESAMPLE_SUCCESS != resample_result)
               {
                  MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: VMX #%lu resampler process failed",me->instance_id);
               }
            }
            else
            {
               MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: VMX #%lu resampler called with NULL pointer !!!",me->instance_id);
               return ADSP_EFAILED;
            }

#ifdef VOICE_MATRIX_DEBUG
            MSG_3(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: VMX #%lu inSamp %lu outReq %lu ",
                  me->instance_id, in_samp_rate * me->out_ports_frame_size, out_samp_req);
#endif
            //setup pointers
            buf1_ptr = currPort->resamp_buffer_ptr;
         }
         else
         {
#ifdef VOICE_MATRIX_DEBUG
            MSG_3(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: VMX #%lu not invoking resampler i/p %lu o/p %lu", me->instance_id, in_port_id, i);
#endif
            buf1_ptr = me->in_port_params_ptr[in_port_id].loc_buffer_ptr;
         }

         //Can be written in assembly
         for (j = 0; j < out_samp_req; j++)
         {
            *(buf2_ptr+j) = *(buf2_ptr+j) + *(buf1_ptr+j);
         }
         //Mark input samples as used
         me->in_port_params_ptr[in_port_id].acc_pending = 0;
         //Mark output port as having samples if port is not LP
         //This means if data is from only LP port, it will not be pushed out. MP and NP ports will still push out data
         if(VMX_INPUT_LOW_PRIO != me->in_port_params_ptr[in_port_id].priority)
         {
            me->out_port_params_ptr[i].data_available_mask |= 1 << in_port_id;
            //now simply overwrite out_buf_token with latest input port's in_buf_token and update time stamp
            me->out_port_params_ptr[i].out_buf_token = me->in_port_params_ptr[in_port_id].in_buf_token;
            me->out_port_params_ptr[i].ull_timestamp = me->in_port_params_ptr[in_port_id].ull_timestamp;
         }
      }
   }
   return result;
}

/**
  This function handles the Media Type msg received in the data
  queue of an input port.

  @param instance_ptr [in] A pointer to instance struct of the
  matrix
  @param in_port_id [in] ID of the input port that received the
  media type msg.
  @param data_q_msg [in] contents of the dataQ
  @return none
 */

static ADSPResult vmx_media_type_handler(void* instance_ptr, uint32_t in_port_id, elite_msg_any_t data_q_msg)
{

   vmx_t* me = (vmx_t*)instance_ptr;
   int32_t in_samp_rate;
   ADSPResult result = ADSP_EOK;
   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: VMX #%lu received ELITE_DATA_MEDIA_TYPE msg!!!", me->instance_id);
   // TO DO: Check if any existing buffers need to be flushed!!
   elite_msg_data_media_type_apr_t* media_type_payload_ptr =
      (elite_msg_data_media_type_apr_t*)data_q_msg.pPayload;

   switch(media_type_payload_ptr->unMediaTypeFormat)
   {
      case ELITEMSG_DATA_MEDIA_TYPE_APR:
         {
            switch (media_type_payload_ptr->unMediaFormatID)
            {
               case ELITEMSG_MEDIA_FMT_MULTI_CHANNEL_PCM:
                  {

                     elite_multi_channel_pcm_fmt_blk_t *media_format_blk_ptr =
                        (elite_multi_channel_pcm_fmt_blk_t*)elite_msg_get_media_fmt_blk(media_type_payload_ptr);

                     if(media_format_blk_ptr->num_channels != 1)
                     {
                        MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Number of channels not supported by VMX Svc!!!");
                        result = ADSP_EBADPARAM;
                        break;
                     }

                     if(me->in_port_params_ptr[in_port_id].sample_rate * 1000 ==
                           media_format_blk_ptr->sample_rate)
                     {
                        MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: VMX #%lu : No change to sample rate in media type : port SR %ld, MT SR %ld, skipping resampler init!!!",
                                                                         me->instance_id, me->in_port_params_ptr[in_port_id].sample_rate, media_format_blk_ptr->sample_rate);
                        result = ADSP_EOK;
                        break;
                     }

                     if (8000 == media_format_blk_ptr->sample_rate)
                     {
                        me->in_port_params_ptr[in_port_id].sample_rate = 8;
                     }
                     else if (16000 == media_format_blk_ptr->sample_rate)
                     {
                        me->in_port_params_ptr[in_port_id].sample_rate = 16;
                     }
                     else if (32000 == media_format_blk_ptr->sample_rate)
                     {
                        me->in_port_params_ptr[in_port_id].sample_rate = 32;
                     }
                     else if (48000 == media_format_blk_ptr->sample_rate)
                     {
                        me->in_port_params_ptr[in_port_id].sample_rate = 48;
                     }
                     else
                     {
                        MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Sampling rate not supported by VMX Svc!!!");
                        result = ADSP_EBADPARAM;
                        break;
                     }
                     in_samp_rate = media_format_blk_ptr->sample_rate;

                     //Initialize the resamplers. convention is that first pointer is for the lower sampling rate of the two
                     switch(in_samp_rate)
                     {
                        case 8000:
                           {
                                result = voice_mixer_resampler_init(&(me->in_port_params_ptr[in_port_id]),in_samp_rate,16000,32000,48000);
                              break;
                           }
                        case 16000:
                           {
                                result = voice_mixer_resampler_init(&(me->in_port_params_ptr[in_port_id]),in_samp_rate,8000,32000,48000);
                                break;
                             }
                          case 32000:
                             {
                                result = voice_mixer_resampler_init(&(me->in_port_params_ptr[in_port_id]),in_samp_rate,8000,16000,48000);
                              break;
                           }
                        case 48000:
                           {
                                result = voice_mixer_resampler_init(&(me->in_port_params_ptr[in_port_id]),in_samp_rate,8000,16000,32000);
                              break;
                           }
                        default:
                           {
                              MSG_2(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Error! Vmx #%ld unsupported input sampling rate %ld!", me->instance_id, in_samp_rate);
                           }
                     }
                     if (ADSP_FAILED(result))
                     {
                         MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Error! Vmx #%ld resampler init failed !", me->instance_id);
                     }
                     break;
                  }
               default:
                  {
                     MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: VMX #%lu received invalid format in ELITE_DATA_MEDIA_TYPE msg.", me->instance_id);
                     result = ADSP_EBADPARAM;
                     break;
                  }
            }
            break;
         }
      default:
         {
            MSG_2(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: VMX #%lu Svc rcvd invalid media format %#lx in ELITE_DATA_MEDIA_TYPE msg.", me->instance_id,media_type_payload_ptr->unMediaTypeFormat);
            result = ADSP_EBADPARAM;
            break;
         }
   }
   // return buffer to its returnQ
   elite_msg_push_payload_to_returnq(media_type_payload_ptr->pBufferReturnQ,
         (elite_msg_any_payload_t*)media_type_payload_ptr);
   MSG(MSG_SSID_QDSP6, DBG_LOW_PRIO,"VCP: !!! VMX Svc returned payload buffer of ELITE_DATA_MEDIA_TYPE msg to its returnQ!!!");

   return result;
}

/**
  This function is the handler for the ELITEMSG_CUSTOM_VMX_RUN cmd,
  which is a per-stream (or per-input-port) cmd. It changes the
  state of that input port to active and then acks back.

  @param instance_ptr [in] A pointer to instance struct of the
  matrix
  @param pMsg [in] The message that needs handling
  @return An indication of success or failure
 */

static ADSPResult vmx_run(void* instance_ptr, elite_msg_any_t* pMsg)
{
   ADSPResult result = ADSP_EOK;
   vmx_t* me = (vmx_t*)instance_ptr;
   uint32_t port_id;

   elite_msg_custom_vmx_run_t* payload = (elite_msg_custom_vmx_run_t*)pMsg->pPayload;
   MSG_3(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: VMX #%lu received ELITEMSG_CUSTOM_VMX_RUN, port %lu dir %lu ",
         me->instance_id, payload->unPortID, payload->unDirection);
   port_id = payload->unPortID;
   if(VMX_INPUT_DIRECTION == payload->unDirection)
   {
      if (PORT_INACTIVE == me->in_port_params_ptr[port_id].in_port_state ||
            me->max_in_portID < port_id)
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: VMX Svc rcvd invalid i/p port ID for ELITEMSG_CUSTOM_VMX_RUN");
         result = ADSP_EBADPARAM;
      }
      //If already active, just return success
      else if(PORT_ACTIVE == me->in_port_params_ptr[port_id].in_port_state)
      {
         result = ADSP_EOK;
      }
      else
      {
         /* Need to add to output req pending mask based on two criteria:
            1. Output port has a buffer available
            2. Output port does not already have accumulated data from this port
          */
         uint32_t mask = me->in_port_params_ptr[port_id].input_mapping_mask
            & me->out_buffer_available_mask;
         while (mask)
         {
            uint32_t bit_pos = voice_get_high_lsb(mask);
            mask ^= (1 << bit_pos);
            if(0 == ((1<<port_id) & me->out_port_params_ptr[bit_pos].data_available_mask))
            {
               /* This means no data right now, so can mark as pending */
               me->in_port_params_ptr[port_id].output_req_pending_mask |= (1 << bit_pos);
            }
         }
         me->in_port_params_ptr[port_id].in_port_state = PORT_READY;
      }
   }
   else if(VMX_OUTPUT_DIRECTION == payload->unDirection)
   {
      if (me->max_out_portID < port_id || PORT_INACTIVE == me->out_port_params_ptr[port_id].out_port_state)
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: VMX Svc rcvd invalid o/p port ID for ELITEMSG_CUSTOM_VMX_RUN");
         result = ADSP_EBADPARAM;
      }
      else
      {
         vmx_out_port_info_t *curr_out_port_ptr = &me->out_port_params_ptr[port_id];
         curr_out_port_ptr->out_port_state = PORT_ACTIVE;
         me->current_bit_field |= 1 << (port_id + OUTPORT_BITSTART);
         me->buf_bit_field |= 1 << (port_id + OUTPORT_BITSTART);
         /* Mark as active */
         me->active_out_ports_mask |= (1 << port_id);
         // Clear accbuf and data available mask, it is possible some input may have accumulated data to this buffer
         memset(curr_out_port_ptr->acc_buf_ptr, 0, DEFAULT_BUF_SIZE << 1);
         curr_out_port_ptr->data_available_mask = 0;
      }
   }
   else
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Error! VMX #%lu invalid direction %lu", me->instance_id, payload->unDirection);
      result = ADSP_EBADPARAM;
   }
   elite_svc_send_ack(pMsg, result);
   return result;
}

/**
  This function is the handler for the ELITEMSG_CUSTOM_VMX_STOP cmd.

  @param instance_ptr [in] A pointer to instance struct of the
  matrix
  @param pMsg [in] The message that needs handling
  @return An indication of success or failure
 */
/*
   This function needs to do 4 things:
   1. mark port as stopped
   2. drop all buffers it has and any that come in
   3. make sure any push output port that was waiting for this data doesnt do so any more
   4. remove port from active ports mask
 */
static ADSPResult vmx_stop(void* instance_ptr, elite_msg_any_t* pMsg)
{
   ADSPResult result = ADSP_EOK;
   vmx_t* me = (vmx_t*)instance_ptr;
   uint32_t port_id;

   elite_msg_custom_stop_t* payload = (elite_msg_custom_stop_t*)pMsg->pPayload;
   MSG_3(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: VMX #%lu received ELITEMSG_CUSTOM_VMX_STOP, port %lu dir %lu ",
         me->instance_id, payload->unPortID, payload->unDirection);
   port_id = payload->unPortID;
   if(VMX_INPUT_DIRECTION == payload->unDirection)
   {
      if (me->max_in_portID < port_id || PORT_INACTIVE == me->in_port_params_ptr[port_id].in_port_state)
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: VMX Svc rcvd invalid i/p port ID for ELITEMSG_CUSTOM_VMX_STOP");
         result = ADSP_EBADPARAM;
      }
      else
      {
         me->in_port_params_ptr[port_id].in_port_state = PORT_STOPPED;
         me->active_in_ports_mask &= ~(1<<port_id);
         me->in_port_params_ptr[port_id].acc_pending = 0;
         me->in_port_params_ptr[port_id].output_req_pending_mask = 0;

         // Release input buffer if held
         if (INPUT_BUFFER_HELD == me->in_port_params_ptr[port_id].in_buf_status)
         {
            elite_msg_any_t data_q_msg = me->in_port_params_ptr[port_id].data_q_msg;
            if (ADSP_FAILED(elite_msg_return_payload_buffer(&data_q_msg)))
            {
               MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Failed to return buffer i/p port %lu", port_id);
            }
            me->in_port_params_ptr[port_id].in_buf_status = INPUT_BUFFER_RELEASED;
         }
         // Add this port to wait mask, so you can keep dropping any input buffers that come in
         me->data_bit_field |= 1<<(INPORT_BITSTART + port_id);
         me->current_bit_field |= 1<<(INPORT_BITSTART + port_id);

          // freeing up re-sampler
          vmx_in_port_info_t *curr_in_port_ptr = &me->in_port_params_ptr[port_id];;

          for(int i = 0; i < VMX_NUM_SAMPLE_RATES_SUPPORTED - 1; i++)
          {
              if(NULL != curr_in_port_ptr->resampler[i].channel_mem_ptr)
              {
                  qurt_elite_memory_free(curr_in_port_ptr->resampler[i].channel_mem_ptr);
                  curr_in_port_ptr->resampler[i].channel_mem_ptr = NULL;
                  MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: VMX#%lu freeing resmapler with index (%ld) in vmx_stop ",me->instance_id,i);

              }
          }
      }
   }
   /* Following things need to be done:
      1. Change state to stopped
      2. Remove this port from output_req_pending_mask for inputs (mostly not reqd)
      3. Take care of global masks
      4. Clear out accBuf
      5. Remove from waitmask (not sure if need to wait for all o/p buffers to be returned
      , most probably not)
    */
   else if(VMX_OUTPUT_DIRECTION == payload->unDirection)
   {
      if (me->max_out_portID < port_id || PORT_INACTIVE == me->out_port_params_ptr[port_id].out_port_state)
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: VMX Svc rcvd invalid o/p port ID for ELITEMSG_CUSTOM_VMX_STOP");
         result = ADSP_EBADPARAM;
      }
      else
      {
         vmx_out_port_info_t *curr_out_port_ptr = &me->out_port_params_ptr[port_id];
         curr_out_port_ptr->out_port_state = PORT_STOPPED;
         /* Not sure if necessary
         // Remove this port from output_req_pending_mask for all inputs
         uint32_t mask2 = curr_out_port_ptr->output_mapping_mask;
         int32_t bitpos;
         while(mask2)
         {
         bitpos = voice_get_high_lsb(mask2);
         mask2 ^= 1 << bitpos;
         // Have to make sure the port still exists
         if(me->in_port_params_ptr[bitpos].in_port_state != PORT_INACTIVE)
         {
         me->in_port_params_ptr[bitpos].output_req_pending_mask &= ~(1 << port_id);
         }
         }
          */
         // Take care of out_buffer_available_mask and samples_available_mask
         me->out_buffer_available_mask &= ~(1 << port_id);
         curr_out_port_ptr->data_available_mask = 0;
         me->active_out_ports_mask &= ~(1 << port_id);
         // Remove from waitmasks
         me->current_bit_field &= ~(1 << (port_id + OUTPORT_BITSTART));
         me->buf_bit_field &= ~(1 << (port_id + OUTPORT_BITSTART));
      }
   }
   else
   {
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Error! VMX #%lu invalid direction %lu", me->instance_id, payload->unDirection);
      result = ADSP_EBADPARAM;
   }
   elite_svc_send_ack(pMsg, result);
   return result;
}

/**
  This function is the handler for the
  ELITEMSG_CUSTOM_VMX_CFG_INPUT_PORTS cmd. It configures input ports on
  the matrix mixer.

  @param instance_ptr [in] A pointer to instance struct of the
  matrix
  @param pMsg [in] The message that needs handling
  @return An indication of success or failure
 */

static ADSPResult vmx_cfg_input_ports(void* instance_ptr, elite_msg_any_t* pMsg)
{
   vmx_t* me = (vmx_t*)instance_ptr;
   int32_t k, index;
   ADSPResult result = ADSP_EOK;
   uint32_t in_port_id;
   //assert(pMsg);
   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP:  VMX #%lu received ELITEMSG_CUSTOM_VMX_CFG_INPUT_PORTS ",
         me->instance_id);
   elite_msg_custom_vmx_cfg_inports_t* payload_ptr = (elite_msg_custom_vmx_cfg_inports_t*)pMsg->pPayload;
   vmx_inport_cfg_params_t payload = payload_ptr->cfgInPortsPayload;
   vmx_inport_handle_t** responsePtr = payload.ack_ptr;
   vmx_in_port_info_t *curr_in_port_ptr = NULL;

   index = payload.index;
   in_port_id = payload.index;
   if(payload.configuration != VMX_NEW_INPUT)
   {
      curr_in_port_ptr = &me->in_port_params_ptr[in_port_id];
      if(PORT_INACTIVE == curr_in_port_ptr->in_port_state)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Error! VMX #%lu received command on inactive i/p %lu", me->instance_id, in_port_id);
         result = ADSP_EBADPARAM;
         elite_svc_send_ack(pMsg, result);
         return result;
      }
   }

   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: I/P PORT ID = %d ", (int)index);
   switch(payload.configuration)
   {
      case VMX_NEW_INPUT:
         {
            in_port_id = index;
            MSG(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: I/P PORT CONFIGURATION = VMX_NEW_INPUT ");
            if (index < 0)
            {
               in_port_id = 0;
               while (in_port_id <= me->max_in_ports &&
                     (PORT_INACTIVE != me->in_port_params_ptr[in_port_id].in_port_state))
               {
                  in_port_id++;
               }
            }
            if ((in_port_id >= me->max_in_ports) ||
                  (PORT_INACTIVE != me->in_port_params_ptr[in_port_id].in_port_state))
            {
               MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Error: Either invalid index sent or no more input ports available.");
               result = ADSP_EFAILED;
            }
            else if(VMX_INPUT_HIGH_PRIO < payload.priority)
            {
               MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Invalid priority %lu!!", payload.priority);
               result = ADSP_EBADPARAM;
            }
            else
            {
               //TODO: Check create params
               curr_in_port_ptr = &me->in_port_params_ptr[in_port_id];
               curr_in_port_ptr->in_port_state = PORT_STOPPED;
               curr_in_port_ptr->num_buf_rcvd = 0;
               curr_in_port_ptr->loc_buffer_ptr = 0;
               curr_in_port_ptr->resamp_buffer_ptr = 0;
               curr_in_port_ptr->in_buf_status = INPUT_BUFFER_RELEASED;
               curr_in_port_ptr->input_mapping_mask = payload.input_mapping_mask;
               curr_in_port_ptr->priority = payload.priority;

               // create the input port's data Q and add it to the matrix's channel.
               curr_in_port_ptr->in_port_handle.port_handle.dataQ = (qurt_elite_queue_t*) qurt_elite_memory_malloc(
                     QURT_ELITE_QUEUE_GET_REQUIRED_BYTES (MAX_DATA_Q_ELEMENTS) , QURT_ELITE_HEAP_DEFAULT);
               if(NULL == curr_in_port_ptr->in_port_handle.port_handle.dataQ)
               {
                  MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Failed to allocate memory for voice matrix #%lu data queues", me->instance_id);
                  result = ADSP_ENOMEMORY;
                  elite_svc_send_ack(pMsg, result);
                  return result;
               }
               if (ADSP_FAILED(result = qurt_elite_queue_init(curr_in_port_ptr->data_q_name,
                           MAX_DATA_Q_ELEMENTS,
                           curr_in_port_ptr->in_port_handle.
                           port_handle.dataQ)))
               {
                  MSG(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Failed to create voice matrix message queues!! ");
                  qurt_elite_memory_free(curr_in_port_ptr->in_port_handle.port_handle.dataQ);
                  elite_svc_send_ack(pMsg, result);
                  return result;
               }
               if (ADSP_FAILED(result = qurt_elite_channel_addq(&me->channel,
                           curr_in_port_ptr->in_port_handle.port_handle.dataQ,
                           1 << (INPORT_BITSTART + in_port_id))))
               {
                  MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: ADSP_FAILED to add mqs to channel result = %d!!", result);
                  qurt_elite_queue_deinit(curr_in_port_ptr->in_port_handle.port_handle.dataQ);
                  qurt_elite_memory_free(curr_in_port_ptr->in_port_handle.port_handle.dataQ);
                  elite_svc_send_ack(pMsg, result);
                  return result;
               }

               if(VMX_INPUT_HIGH_PRIO == curr_in_port_ptr->priority)
               {
                  me->high_priority_inputs_mask |= (1 << in_port_id);
               }
               else if(VMX_INPUT_MED_PRIO == curr_in_port_ptr->priority)
               {
                  me->medium_priority_inputs_mask |= (1 << in_port_id);
               }
               else
               {
                  me->low_priority_inputs_mask |= (1 << in_port_id);
               }

               // Do this only if input_mapping_mask != 0
               if (0 != curr_in_port_ptr->input_mapping_mask)
               {
                  uint32_t mask2 = curr_in_port_ptr->input_mapping_mask;
                  int32_t bitpos;
                  //update output_mapping_mask everywhere
                  while(mask2)
                  {
                     bitpos = voice_get_high_lsb( mask2 );
                     mask2 ^= 1<<bitpos;
                     me->out_port_params_ptr[bitpos].output_mapping_mask |= 1<<in_port_id;
                  }

               }

               curr_in_port_ptr->output_req_pending_mask = payload.input_mapping_mask & me->out_buffer_available_mask;
               if (me->max_in_portID < in_port_id)
               {
                  me->max_in_portID = in_port_id;
               }

               qurt_elite_atomic_increment(&qurt_elite_globalstate.nMsgQs);

               // name the input port's data Q.
               snprintf(curr_in_port_ptr->data_q_name, QURT_ELITE_DEFAULT_NAME_LEN, "VMXDataQ0%lx", in_port_id);

               k = 1 << (in_port_id + INPORT_BITSTART);
               // update the mask that contains bit positions of all data Qs.
               me->data_bit_field = me->data_bit_field | k;
               me->current_bit_field = me->current_bit_field | k;

               curr_in_port_ptr->acc_pending = 0;

               // Allocate local buffers
               curr_in_port_ptr->in_port_buf_size = DEFAULT_BUF_SIZE;
               curr_in_port_ptr->loc_buffer_ptr =
                  (int16_t*)qurt_elite_memory_malloc(DEFAULT_BUF_SIZE, QURT_ELITE_HEAP_DEFAULT);
               curr_in_port_ptr->resamp_buffer_ptr =
                  (int16_t*)qurt_elite_memory_malloc(DEFAULT_BUF_SIZE, QURT_ELITE_HEAP_DEFAULT);

               if(( NULL ==  curr_in_port_ptr->loc_buffer_ptr ) || ( NULL == curr_in_port_ptr->resamp_buffer_ptr ))
               {
                   MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Failed to allocate memory for local/resampler buffers in VMX instance #%lu ", me->instance_id);
                   return ADSP_ENOMEMORY;
               }

               // Create resampler instances. Since three sampling rates are supported, we need two resamplers on each input port
               // Allocate for max resampler channel mem size for seamless reinit without de-alloc
              /* {
                  uint32_t out_frame_samples;
                  voice_resampler_set_config(&(curr_in_port_ptr->resampler[0].resamp_config),
                       VOICE_SWB_SAMPLING_RATE,VOICE_FB_SAMPLING_RATE,NO_OF_BITS_PER_SAMPLE,VOICE_FRAME_SIZE_SWB,&out_frame_samples);
               }
               for(int i = 0; i < VMX_NUM_SAMPLE_RATES_SUPPORTED - 1; i++)
               {
                  curr_in_port_ptr->resampler[i].channel_mem_size = (curr_in_port_ptr->resampler[0].resamp_config).total_channel_mem_size;
                  curr_in_port_ptr->resampler[i].channel_mem_ptr = qurt_elite_memory_malloc(curr_in_port_ptr->resampler[i].channel_mem_size, QURT_ELITE_HEAP_DEFAULT);
                  if(NULL == curr_in_port_ptr->resampler[i].channel_mem_ptr)
                  {
                     MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Failed to allocate memory for resampler in VMX instance #%lu ", me->instance_id);
                     result = ADSP_ENOMEMORY;
                     elite_svc_send_ack(pMsg, result);
                     return result;
                  }
               }*/

               // Send an ack with port ID and port handle to calling process.
               curr_in_port_ptr->in_port_handle.inport_id = in_port_id;
               *responsePtr = &(curr_in_port_ptr->in_port_handle);
            }
            break;
         }
      case VMX_CLOSE_INPUT:
         {
            elite_msg_any_t data_q_msg = curr_in_port_ptr->data_q_msg;
            MSG(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: I/P PORT CONFIGURATION = VMX_CLOSE_INPUT ");

            //if a buffer is currently held, release the buffer before closing the input port.
            if (INPUT_BUFFER_HELD == curr_in_port_ptr->in_buf_status)
            {
               MSG_1(MSG_SSID_QDSP6, DBG_LOW_PRIO,"VCP: VMX i/p port %lu releasing an input buffer that it was holding on to", in_port_id);
               if (ADSP_FAILED(elite_msg_return_payload_buffer(&data_q_msg)))
               {
                  MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Failed to return buffer while closing i/p port %lu", in_port_id);
               }
               curr_in_port_ptr->in_buf_status = INPUT_BUFFER_RELEASED;
            }
            /*
               Dequeue the dataQ to check if any buffers are present.
               If yes, release those to the upstream svc's bufQ.
             */
            if(NULL != curr_in_port_ptr->in_port_handle.port_handle.dataQ)
            {
               while (ADSP_EOK == (qurt_elite_queue_pop_front(curr_in_port_ptr->in_port_handle.port_handle.dataQ,
                           (uint64_t*)&data_q_msg )))
               {
                  MSG_1(MSG_SSID_QDSP6, DBG_LOW_PRIO,"VCP: VMX i/p port %lu dequeued an input buffer from dataQ. Releasing it now.", in_port_id);
                  if (ADSP_FAILED(elite_msg_return_payload_buffer(&data_q_msg)))
                  {
                     MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Failed to return buffer while closing i/p port %lu", in_port_id);
                  }
               }
               // Free up memory that was allocated to this port's resampler buffer
               MSG_1(MSG_SSID_QDSP6, DBG_LOW_PRIO,"VCP: VMX i/p port %lu de-allocating mem for its resampler buffer", in_port_id);
               if(curr_in_port_ptr->loc_buffer_ptr)
               {
                  qurt_elite_memory_free(curr_in_port_ptr->loc_buffer_ptr);
                  curr_in_port_ptr->loc_buffer_ptr = NULL;
               }
               if(curr_in_port_ptr->resamp_buffer_ptr)
               {
                  qurt_elite_memory_free(curr_in_port_ptr->resamp_buffer_ptr);
                  curr_in_port_ptr->resamp_buffer_ptr = NULL;
               }
               //Remove this port's data Q from the overall wait_mask and the data bitfield.
               me->current_bit_field &= (~(1 << (in_port_id + INPORT_BITSTART)));
               me->data_bit_field &= (~(1 << (in_port_id + INPORT_BITSTART)));

               // Destroy the data Q. This would also remove it from the channel.
               MSG_1(MSG_SSID_QDSP6, DBG_LOW_PRIO,"VCP: VMX i/p port %lu destroying its dataQ", in_port_id);
               qurt_elite_queue_deinit(curr_in_port_ptr->in_port_handle.port_handle.dataQ);
               qurt_elite_memory_free(curr_in_port_ptr->in_port_handle.port_handle.dataQ);
               curr_in_port_ptr->in_port_handle.port_handle.dataQ = NULL;
            }

            //Delete the resamplers

            for(int i = 0; i < VMX_NUM_SAMPLE_RATES_SUPPORTED - 1; i++)
            {
               if(NULL != curr_in_port_ptr->resampler[i].channel_mem_ptr)
               {
                  qurt_elite_memory_free(curr_in_port_ptr->resampler[i].channel_mem_ptr);
                  curr_in_port_ptr->resampler[i].channel_mem_ptr = NULL;
                  MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: VMX#%lu freeing resmapler with index (%ld) in vmx_close ",me->instance_id,i);

               }
            }

            // CHANGE THE STATUS OF THIS PORT TO PORT_INACTIVE
            curr_in_port_ptr->in_port_state = PORT_INACTIVE;

            // Reset some of the parameters that you dont wan't accidentally reused when the port is recycled
            //TODO: Look into using memset
            curr_in_port_ptr->sample_rate = 0;

            me->active_in_ports_mask &= ~(1 << in_port_id);
            // Reset bit in various masks
            me->high_priority_inputs_mask &= ~(1 << in_port_id);
            me->medium_priority_inputs_mask &= ~(1 << in_port_id);
            me->low_priority_inputs_mask &= ~(1 << in_port_id);
            MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: VMX #%lu completed closing output port %d",
                  me->instance_id, (int)in_port_id);
            break;
         }
      case VMX_REASSIGN_INPUT:
         {
            MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: I/P PORT CONFIGURATION = VMX_REASSIGN_INPUT, state %lu ", curr_in_port_ptr->in_port_state);
#ifdef VOICE_MATRIX_DEBUG
            MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: VMX #%lu old mask %#x old output_req_pending_mask %#x new mask %#x",
                  me->instance_id, curr_in_port_ptr->input_mapping_mask, curr_in_port_ptr->output_req_pending_mask, payload.input_mapping_mask);
#endif
            uint32_t attached_mask = (~curr_in_port_ptr->input_mapping_mask) & payload.input_mapping_mask;
            uint32_t detached_mask = curr_in_port_ptr->input_mapping_mask & (~payload.input_mapping_mask);

            curr_in_port_ptr->input_mapping_mask = payload.input_mapping_mask;

            /* Handle other masks for input/output ports */
            uint32_t mask = attached_mask;
            int32_t bitpos;

            /* Take care of attached outputs
               Need to handle following:
               1. Add to output mapping mask of each output port being attached
               2. Add to request pending mask of input, provided an output buffer is
               available with that o/p port
             */
            while(mask)
            {
               bitpos = voice_get_high_lsb( mask );
#ifdef VOICE_MATRIX_DEBUG
               MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: VMX #%lu outport %d old inportsmask %#x updating bit %#x",
                     me->instance_id, bitpos, me->out_port_params_ptr[bitpos].output_mapping_mask, 1<<in_port_id);
#endif
               mask ^= (1<<bitpos);
               me->out_port_params_ptr[bitpos].output_mapping_mask |= 1<<in_port_id;
               curr_in_port_ptr->output_req_pending_mask |= ((1 << bitpos)
                     & me->out_buffer_available_mask
                     & (~me->out_port_params_ptr[bitpos].data_available_mask));
            }

            /* Now handle detached outputs
               This is a simpler case:
               1. Remove from output mapping mask of each o/p port being detached
               2. Remove pending request from i/p port
             */
            mask = detached_mask;
            while(mask)
            {
               bitpos = voice_get_high_lsb( mask );
#ifdef VOICE_MATRIX_DEBUG
               MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: VMX #%lu outport %d old inportsmask %#x updating bit %#x",
                     me->instance_id, bitpos, me->out_port_params_ptr[bitpos].output_mapping_mask, 1<<in_port_id);
#endif
               mask ^= 1<<bitpos;
               me->out_port_params_ptr[bitpos].output_mapping_mask &= ~(1<<in_port_id);
               curr_in_port_ptr->output_req_pending_mask &= ~(1 << bitpos);
            }
            // TODO: Consider releasing input buffer if input_mapping_mask has been made zero
            break;
         }
         //Change input port priority
      case VMX_RECONFIGURE_INPUT:
         {
            MSG(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: I/P PORT CONFIGURATION = VMX_RECONFIGURE_INPUT ");
            if(PORT_STOPPED != curr_in_port_ptr->in_port_state)
            {
               MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Input reconfiguration only allowed in stop state! Current state %lu",
                     curr_in_port_ptr->in_port_state);
               result = ADSP_ENOTREADY;
               break;
            }
            if(curr_in_port_ptr->priority == payload.priority)
            {
               result = ADSP_EOK;
               break;
            }
            if(VMX_INPUT_HIGH_PRIO >= payload.priority)
            {
               MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: VMX #%lu i/p %lu, prio was %lu, changed to %lu",
                     me->instance_id, in_port_id, curr_in_port_ptr->priority, payload.priority);
               switch(curr_in_port_ptr->priority)
               {
                  case VMX_INPUT_LOW_PRIO:
                     {
                        me->low_priority_inputs_mask &= ~(1 << in_port_id);
                        break;
                     }
                  case VMX_INPUT_MED_PRIO:
                     {
                        me->medium_priority_inputs_mask &= ~(1 << in_port_id);
                        break;
                     }
                  case VMX_INPUT_HIGH_PRIO:
                     {
                        me->high_priority_inputs_mask &= ~(1 << in_port_id);
                        break;
                     }
               }
               switch(payload.priority)
               {
                  case VMX_INPUT_LOW_PRIO:
                     {
                        me->low_priority_inputs_mask |= (1 << in_port_id);
                        break;
                     }
                  case VMX_INPUT_MED_PRIO:
                     {
                        me->medium_priority_inputs_mask |= (1 << in_port_id);
                        break;
                     }
                  case VMX_INPUT_HIGH_PRIO:
                     {
                        me->high_priority_inputs_mask |= (1 << in_port_id);
                        break;
                     }
               }
               curr_in_port_ptr->priority = payload.priority;
               result = ADSP_EOK;
            }
            else
            {
               MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Invalid priority %lu!!", payload.priority);
               result = ADSP_EBADPARAM;
            }
            break;
         }

      default:
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Incorrect command type %#lx!!", payload.configuration);
            result =  ADSP_EBADPARAM;
            break;
         }
   }
   MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: VMX #%lu sending ack %d for cfg_input_port cmd",me->instance_id, result);
   elite_svc_send_ack(pMsg, result);
   return result;
}

/**
  This function is the handler for the
  ELITEMSG_CUSTOM_VMX_CFG_OUTPUT_PORTS cmd. It configures output ports
  on the matrix mixer.

  @param instance_ptr [in] A pointer to instance struct of the
  matrix
  @param pMsg [in] The message that needs handling
  @return An indication of success or failure
 */

static ADSPResult vmx_cfg_output_ports(void *instance_ptr, elite_msg_any_t* pMsg)
{
   vmx_t* me = (vmx_t*)instance_ptr;
   ADSPResult result = ADSP_EOK;
   int32_t k, ReqSize, port_id;
   uint32_t j;
   elite_msg_data_buffer_t*  pDataPayload;
   vmx_out_port_info_t   *curr_out_port_ptr = NULL;

   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: VMX #%lu received ELITEMSG_CUSTOM_VMX_CFG_OUTPUT_PORTS ",
         me->instance_id);
   elite_msg_custom_vmx_cfg_outports_t* payload_ptr = (elite_msg_custom_vmx_cfg_outports_t*)pMsg->pPayload;
   vmx_outport_cfg_params_t payload = payload_ptr->cfgOutPortsPayload;
   vmx_outport_handle_t** responsePtr = payload.ack_ptr;

   port_id = payload.index;
   if(payload.configuration != VMX_NEW_OUTPUT)
   {
      curr_out_port_ptr = &me->out_port_params_ptr[port_id];
      if(PORT_INACTIVE == curr_out_port_ptr->out_port_state)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Error! VMX #%lu received command\
               on inactive o/p %d", me->instance_id, (int)port_id);
         result = ADSP_EBADPARAM;
         elite_svc_send_ack(pMsg, result);
         return result;
      }
   }
   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: O/P PORT ID = %d ", (int)port_id);
   switch(payload.configuration)
   {
      case VMX_NEW_OUTPUT:
         {
            MSG(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: O/P PORT CONFIGURATION = VMX_NEW_OUTPUT ");
            if (port_id < 0)
            {
               {
                  port_id = 0;
                  // check if an ID can be assigned to the o/p port.
                  while ((uint32_t)port_id <= (me->max_out_ports) &&
                        (PORT_INACTIVE != me->out_port_params_ptr[port_id].out_port_state))
                  {
                     port_id++;
                  }
                  if (me->max_out_ports == (uint32_t)port_id)
                  {
                     MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Error: No more output ports available.");
                     result = ADSP_EFAILED;
                     elite_svc_send_ack(pMsg, result);
                     return result;
                  }
               }
            }
            else
            {
               MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Error: Invalid port ID sent.");
               result = ADSP_EBADPARAM;
               elite_svc_send_ack(pMsg, result);
               return result;
            }
            curr_out_port_ptr = &me->out_port_params_ptr[port_id];
            curr_out_port_ptr->num_buf_sent = 0;
            curr_out_port_ptr->data_available_mask = 0;
            curr_out_port_ptr->out_port_state = PORT_ACTIVE;
            curr_out_port_ptr->num_buf_rcvd = 0;
            curr_out_port_ptr->num_output_bufs = payload.num_output_bufs;
            curr_out_port_ptr->max_buf_q_capacity = payload.max_bufq_capacity;
            curr_out_port_ptr->downstream_peer_handle_ptr = payload.svc_handle_ptr;
            //sampling rates
            curr_out_port_ptr->sample_rate = payload.out_sample_rate;
            curr_out_port_ptr->output_mapping_mask = payload.output_mapping_mask;

            // create the output port's buf Q and add it to the matrix's channel.
            curr_out_port_ptr->bufQ = (qurt_elite_queue_t*) qurt_elite_memory_malloc(
                  QURT_ELITE_QUEUE_GET_REQUIRED_BYTES (curr_out_port_ptr->max_buf_q_capacity) , QURT_ELITE_HEAP_DEFAULT);
            if(NULL == curr_out_port_ptr->bufQ)
            {
               MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Failed to allocate memory for voice matrix #%lu buf queues", me->instance_id);
               result = ADSP_ENOMEMORY;
               elite_svc_send_ack(pMsg, result);
               return result;
            }
            if (ADSP_FAILED(result = qurt_elite_queue_init(curr_out_port_ptr->buf_q_name,
                        curr_out_port_ptr->max_buf_q_capacity,
                        curr_out_port_ptr->bufQ)))
            {
               MSG(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Failed to create output port buffer queue!! ");
               qurt_elite_memory_free(curr_out_port_ptr->bufQ);
               elite_svc_send_ack(pMsg, result);
               return result;
            }
            if (ADSP_FAILED(result = qurt_elite_channel_addq(&me->channel,
                        curr_out_port_ptr->bufQ, 1 << (OUTPORT_BITSTART + port_id))))
            {
               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: ADSP_FAILED to add buf Q to channel result = %d!!", result);
               elite_svc_deinit_buf_queue(curr_out_port_ptr->bufQ, curr_out_port_ptr->num_output_bufs);
               qurt_elite_memory_free(curr_out_port_ptr->bufQ);
               elite_svc_send_ack(pMsg, result);
               return result;
            }

            // check if this is the new max_out_portID
            if (me->max_out_portID < (uint32_t)port_id)
            {
               me->max_out_portID = port_id;
            }

            MSG_2(MSG_SSID_QDSP6, DBG_LOW_PRIO,"VCP: O/P PORT %d is CONNECTED TO DOWNSTR SVC ID %#lx ",
                  (int)port_id, curr_out_port_ptr->downstream_peer_handle_ptr->unSvcId);

            // check if an output_mapping_mask is provided as a parameter. if yes,
            // update the input_mapping_mask of all affected i/p ports.
            uint32_t mask2 = curr_out_port_ptr->output_mapping_mask;
            // Update input port mapping masks
            while(mask2)
            {
               int32_t bitpos = voice_get_high_lsb(mask2);
               mask2 ^= 1 << bitpos;
               me->in_port_params_ptr[bitpos].input_mapping_mask |= (1 << port_id);
               // Update reqPendingMask. This is equivalent to doing input_mapping_mask = payLoad.input_mapping_mask for VMX_NEW_INPUT
               // The output port has just been created and has to have a buffer available, so mark as pending
               me->in_port_params_ptr[bitpos].output_req_pending_mask |= (1 << port_id);
            }

            // Send media type downstream
            // TO DO: For now, send when o/p port is created. Later, send when i/p port receives it.
            vmx_send_media_type_downstream(me, port_id);

            qurt_elite_atomic_increment(&qurt_elite_globalstate.nMsgQs);
            // name the output port's buf Q.
            snprintf(curr_out_port_ptr->buf_q_name, QURT_ELITE_DEFAULT_NAME_LEN, "VMXBufQ0%x", (unsigned int)port_id);

            // add this port's buf Q to the matrix's pchannel wait mask
            //MSG_2(MSG_SSID_QDSP6, DBG_LOW_PRIO,"VCP: Old me->current_bit_field = %d", me->current_bit_field );
            k = 1 << (port_id + OUTPORT_BITSTART);
            me->current_bit_field = me->current_bit_field | k;
            //MSG_2(MSG_SSID_QDSP6, DBG_LOW_PRIO,"VCP: New me->current_bit_field = %d", me->current_bit_field );
            // update the mask that contains bit positions of all buf Qs.
            me->buf_bit_field = me->buf_bit_field | k;
            /**** Allocate and queue output buffers. *****/
            curr_out_port_ptr->buf_size = me->out_ports_frame_size * curr_out_port_ptr->sample_rate;
            /* allocate for WB case */
            ReqSize = GET_ELITEMSG_DATABUF_REQ_SIZE( DEFAULT_BUF_SIZE );
            MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Allocating and queuing output buffers,\
                  buffer size in int32_t = %d", (int)ReqSize);
            for (j = 0; j < curr_out_port_ptr->num_output_bufs; j++)
            {
               // buffer size = frame size (msec) * sample rate (samples/channel/msec)
               pDataPayload = (elite_msg_data_buffer_t*)qurt_elite_memory_malloc(ReqSize, QURT_ELITE_HEAP_DEFAULT);
               if (!pDataPayload)
               {
                  MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Failed to allocate output buffer queue!!");
                  result = ADSP_ENOMEMORY;
                  return result;
               }
               curr_out_port_ptr->out_bufs_allocated++;

               //MSG(MSG_SSID_QDSP6, DBG_LOW_PRIO,"VCP: Enqueue the pointer to this output buffer to the BufQ");
               if (ADSP_FAILED(result = elite_msg_push_payload_to_returnq(curr_out_port_ptr->bufQ,
                           (elite_msg_any_payload_t*)pDataPayload )))
               {
                  qurt_elite_memory_free(pDataPayload);
                  MSG(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Failed to fill buffer queue!! ");
                  return result;
               }
            }

            // Allocate this o/p port's accBuf.
            MSG(MSG_SSID_QDSP6, DBG_LOW_PRIO,"VCP: Allocate this port's accBuf");
            MSG_1(MSG_SSID_QDSP6, DBG_LOW_PRIO,"VCP: accBuf size in int32_t = %lu",
                  me->out_ports_frame_size * curr_out_port_ptr->sample_rate);
            curr_out_port_ptr->acc_buf_ptr =
               (int32_t*)qurt_elite_memory_malloc (DEFAULT_BUF_SIZE << 1, QURT_ELITE_HEAP_DEFAULT);

            if (!(curr_out_port_ptr->acc_buf_ptr))
            {
               MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Failed to allocate accumulation buffer!!");
               result = ADSP_ENOMEMORY;
               //TODO:Cleanup memory allocated above/unify all allocations
               break;
            }
            memset(curr_out_port_ptr->acc_buf_ptr, 0, DEFAULT_BUF_SIZE << 1);

#ifdef VOICE_MATRIX_DEBUG
            MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Create limiter instance");
#endif
            /* limiter parameters are initialized at the matrix service create time.because paramter values are same for all the sampling rates.*/

            /* Lim_get_lib_size fun returns Limiter lib size.Don't change SAMPLING_RATE.It is set for worst case*/
            uint32_t lim_lib_size = Lim_get_lib_size(me->limiter_params[LIM_DELAY],SAMPLING_RATE_48K,NO_OF_CHANNELS);
            lim_lib_size = VOICE_ROUNDTO8(lim_lib_size);

            int8_t* ptrme =  (int8_t*)qurt_elite_memory_malloc(lim_lib_size, QURT_ELITE_HEAP_DEFAULT);
            if ( NULL == ptrme )
            {
               MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Failed to allocate memory for VMX limiter!!");
               result = ADSP_ENOMEMORY;
               break;
            }
            curr_out_port_ptr->limiter_ptr = (CLimiterLib*)ptrme;
            Lim_set_lib_memory(ptrme,(CLimiterLib**)(&(curr_out_port_ptr->limiter_ptr)),me->limiter_params[LIM_DELAY],SAMPLING_RATE_48K,0/*channel index*/);

            /* 48K sampling rate is passed as the i/p parameter in below function because sampling rate for matrix o/p port is not known at create time.*/
            result = Lim_init((CLimiterLib*)curr_out_port_ptr->limiter_ptr,me->limiter_params,0/*channel index*/,SAMPLING_RATE_48K,BITS_PER_SAMPLE);
            if(ADSP_FAILED(result))
            {
               MSG_3(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "VCP: VMX #%lu SR %lu limiter error %d!!!",
                     me->instance_id, curr_out_port_ptr->sample_rate, result);
            }

            /* Mark as active */
            me->active_out_ports_mask |= (1 << port_id);

            /*
               Send an ack with port ID to calling process. Calling process may not
               have specified the input port ID.
             */
            curr_out_port_ptr->outPortHandle.outport_id = port_id;
            *responsePtr = &(curr_out_port_ptr->outPortHandle);
            break;
         }

      case VMX_CLOSE_OUTPUT:
         {
            MSG(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: O/P PORT CONFIGURATION = VMX_CLOSE_OUTPUT ");
            qurt_elite_memory_free(curr_out_port_ptr->acc_buf_ptr);
            curr_out_port_ptr->acc_buf_ptr = NULL;
            me->current_bit_field &= ~(1 << (port_id + OUTPORT_BITSTART));
            me->buf_bit_field &= ~(1 << (port_id + OUTPORT_BITSTART));
            me->out_buffer_available_mask &= ~(1 << port_id);
            MSG(MSG_SSID_QDSP6, DBG_LOW_PRIO,"VCP: Freed memory for accBuf, removed bufQ from the currentBitfield and BufBitfield");

            // update the input_mapping_mask of all affected i/p ports.
            uint32_t mask2 = curr_out_port_ptr->output_mapping_mask;
            int32_t bitpos;
            while(mask2)
            {
               bitpos = voice_get_high_lsb(mask2);
               mask2 ^= 1 << bitpos;
               // Have to make sure the port still exists
               if(me->in_port_params_ptr[bitpos].in_port_state != PORT_INACTIVE)
               {
                  me->in_port_params_ptr[bitpos].input_mapping_mask &= ~(1 << port_id);
                  me->in_port_params_ptr[bitpos].output_req_pending_mask &= me->in_port_params_ptr[bitpos].input_mapping_mask;
               }
            }

            //Delete the Limiter
            CLimiterLib* limiter_ptr = (CLimiterLib*)curr_out_port_ptr->limiter_ptr;
            if (limiter_ptr)
            {
               qurt_elite_memory_free(limiter_ptr);
               curr_out_port_ptr->limiter_ptr = NULL;
            }

            MSG(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: Going to destroy bufQ and associated buffers");
            if(curr_out_port_ptr->bufQ)
            {
               elite_svc_deinit_buf_queue(curr_out_port_ptr->bufQ, curr_out_port_ptr->num_output_bufs);
               qurt_elite_memory_free(curr_out_port_ptr->bufQ);
               curr_out_port_ptr->bufQ = NULL;
            }
            curr_out_port_ptr->out_port_state = PORT_INACTIVE;
            curr_out_port_ptr->out_bufs_allocated = 0;
            curr_out_port_ptr->output_mapping_mask = 0;
            me->active_out_ports_mask &= ~(1 << port_id);

            MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: VMX #%lu completed closing output port %d",
                  me->instance_id, (int)port_id);
            break;
         }
      case VMX_RECONFIGURE_OUTPUT:
         {
            // This is needed for wideband handovers, where output of matrix needs to change to 16k
            // Only uses the fields index and out_sample_rate
            /*
               Things to do
               1. Clear out accBuf
               2. mark sample rate as the new rate
               3. Reinitialize limiter
               4. send media type message
             */
            MSG(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: O/P PORT CONFIGURATION = VMX_RECONFIGURE_OUTPUT ");
            uint32_t newSampleRate = payload.out_sample_rate;
            if(newSampleRate != 8 && newSampleRate!= 16 && newSampleRate!= 48 && newSampleRate!= 32 )
            {
               MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: VMX #%lu o/p port %d sent invalid sampling rate %lu!!!",
                     me->instance_id, (int)port_id, newSampleRate);
               result = ADSP_EBADPARAM;
               break;
            }
            if(curr_out_port_ptr->sample_rate != newSampleRate)
            {
               curr_out_port_ptr->sample_rate = newSampleRate;
               curr_out_port_ptr->buf_size = me->out_ports_frame_size * curr_out_port_ptr->sample_rate;
               memset(curr_out_port_ptr->acc_buf_ptr, 0, DEFAULT_BUF_SIZE << 1);
#ifdef VOICE_MATRIX_DEBUG
               MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Reconfigure limiter");
#endif
               result = Lim_init((CLimiterLib*)curr_out_port_ptr->limiter_ptr,me->limiter_params,0/*channel index*/,curr_out_port_ptr->sample_rate,BITS_PER_SAMPLE);
               if(ADSP_FAILED(result))
               {
                  MSG_3(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "VCP: VMX #%lu SR %lu limiter error %d!!!",
                        me->instance_id, curr_out_port_ptr->sample_rate, result);
               }
            }

            vmx_send_media_type_downstream(me, port_id);

            break;
         }
         //Convert this output_mapping_mask to a input_mapping_mask. Should be otherwise identical to reassign_input
      case VMX_REASSIGN_OUTPUT:
         {
            MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: O/P PORT CONFIGURATION = VMX_REASSIGN_OUTPUT, state %lu ", curr_out_port_ptr->out_port_state);
#ifdef VOICE_MATRIX_DEBUG
            MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: VMX #%lu old mask %#x new mask %#x",
                  me->instance_id, curr_out_port_ptr->output_mapping_mask,payload.output_mapping_mask);
#endif
            //Need to find out which port assignments have changed. These need to be used to update input port mapping masks
            uint32_t attached_mask = (~curr_out_port_ptr->output_mapping_mask) & payload.output_mapping_mask;
            uint32_t detached_mask = curr_out_port_ptr->output_mapping_mask & (~payload.output_mapping_mask);
            curr_out_port_ptr->output_mapping_mask = payload.output_mapping_mask;

            // update the input_mapping_mask of all affected i/p ports.
            uint32_t mask = attached_mask;
            int32_t bitpos;

            // Update input port mapping masks
            while(mask)
            {
               bitpos = voice_get_high_lsb(mask);
               mask ^= 1 << bitpos;
#ifdef VOICE_MATRIX_DEBUG
               MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: VMX #%lu inport %d old imm %#x old output_req_pending_mask %#x updating bit %#x",
                     me->instance_id, bitpos, me->in_port_params_ptr[bitpos].input_mapping_mask,
                     me->in_port_params_ptr[bitpos].output_req_pending_mask, 1<<port_id);
#endif
               me->in_port_params_ptr[bitpos].input_mapping_mask |= (1 << port_id);
               // Now need to update the output_req_pending_mask
               me->in_port_params_ptr[bitpos].output_req_pending_mask |=
                  ((1 << port_id) & me->out_buffer_available_mask & (~curr_out_port_ptr->data_available_mask));
            }
            mask = detached_mask;
            while(mask)
            {
               bitpos = voice_get_high_lsb( mask );
#ifdef VOICE_MATRIX_DEBUG
               MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: VMX #%lu outport %d old inportsmask %#x updating bit %#x",
                     me->instance_id, bitpos, me->out_port_params_ptr[bitpos].output_mapping_mask, 1<<in_port_id);
#endif
               mask ^= 1<<bitpos;
               me->in_port_params_ptr[bitpos].input_mapping_mask &= ~(1 << port_id);
               me->in_port_params_ptr[bitpos].output_req_pending_mask &= ~(1 << port_id);
            }

            // TODO: Consider releasing input buffer if input_mapping_mask has been made zero
            break;
         }
      default:
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Incorrect command type %#lx!!", payload.configuration);
            result =  ADSP_EBADPARAM;
            break;
         }
   }
   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: !!! VMX #%lu sending ack for cfg_output_port cmd !!!",me->instance_id);
   elite_svc_send_ack(pMsg, result);
   return result;
}

static ADSPResult vmx_send_media_type_downstream(void* instance_ptr, uint32_t out_port_id)
{
   vmx_t*            me = (vmx_t*)instance_ptr;
   ADSPResult result = ADSP_EOK;
   qurt_elite_bufmgr_node_t buf_mgr_node;
   int32_t nActualSize;
   elite_msg_any_t media_type_msg;

   if ( ADSP_FAILED(elite_mem_get_buffer(sizeof(elite_msg_data_media_type_apr_t) + sizeof( elite_multi_channel_pcm_fmt_blk_t), &buf_mgr_node, (int *)&nActualSize)))
   {
      return ADSP_ENOMEMORY;
   }

   elite_msg_data_media_type_apr_t* media_type_payload_ptr =
      (elite_msg_data_media_type_apr_t*)buf_mgr_node.pBuffer;

   media_type_payload_ptr->pBufferReturnQ = buf_mgr_node.pReturnQ;
   media_type_payload_ptr->pResponseQ = NULL;
   media_type_payload_ptr->unClientToken = NULL;

   media_type_payload_ptr->unMediaTypeFormat = ELITEMSG_DATA_MEDIA_TYPE_APR;
   media_type_payload_ptr->unMediaFormatID   = ELITEMSG_MEDIA_FMT_MULTI_CHANNEL_PCM;

   elite_multi_channel_pcm_fmt_blk_t *media_format_blk_ptr =
      (elite_multi_channel_pcm_fmt_blk_t*)elite_msg_get_media_fmt_blk(media_type_payload_ptr);
   memset(media_format_blk_ptr,0,sizeof(elite_multi_channel_pcm_fmt_blk_t));

   media_format_blk_ptr->num_channels        = 1;
   media_format_blk_ptr->bits_per_sample     = 16;
   media_format_blk_ptr->sample_rate         = (1000 * me->out_port_params_ptr[out_port_id].sample_rate);
   media_format_blk_ptr->is_signed           = TRUE;
   media_format_blk_ptr->is_interleaved      = FALSE;
   media_format_blk_ptr->channel_mapping[0]  = PCM_CHANNEL_C;

   media_type_msg.unOpCode = ELITE_DATA_MEDIA_TYPE;
   media_type_msg.pPayload = (void*) media_type_payload_ptr;

   if(ADSP_FAILED(result = qurt_elite_queue_push_back(
               me->out_port_params_ptr[out_port_id].downstream_peer_handle_ptr->dataQ, (uint64_t*)&media_type_msg )))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: VMX failed to send ELITE_DATA_MEDIA_TYPE msg");
      return result;
   }

   return result;
}

static void populate_limiter_params(void* instance_ptr)
{
   vmx_t*            me = (vmx_t*)instance_ptr;

   me->limiter_params[LIM_ENABLE]      = 0x1;      // Q0 Limiter enable word
   me->limiter_params[LIM_MODE]        = 0x1;      // Q0 Limiter mode word
   me->limiter_params[LIM_MAKEUP_GAIN] = 0x100;    // Q7.8 Limiter make-up gain
   me->limiter_params[LIM_GC]          = 0x7EB8;   // Q15 Limiter gain recovery coefficient
   me->limiter_params[LIM_MAX_WAIT]    = 0x147;    // Q15 Limiter delay in seconds
   me->limiter_params[LIM_DELAY]       = 0x0;      // Q15 Limiter waiting time in seconds 1ms
   if(VMX_TX_INSTANCE == me->instance_id)
   {
      me->limiter_params[LIM_THRESH]   = 0x1FF8;   // -12 dB in Q1.15 for limiting to 14 bits on tx path
   }
   else
   {
      me->limiter_params[LIM_THRESH]   = 0x7FFF;   // 0 dB in Q1.15 for limiting to 16 bits on rx path
   }
}

static inline void voice_saturate_to_16(int16_t* out, int32_t* in, uint32_t num_samples)
{
   uint32_t i;
   for(i = 0; i < num_samples; i++)
   {
      *out++ = (int16)(*in++);
   }
   return;
}

static uint8_t vmx_choose_resampler(uint32_t in_samp_rate, uint32_t out_samp_rate)
{
   uint8_t resamp_index = 2;
   if((8 == in_samp_rate && 16 == out_samp_rate) ||
         (8 == out_samp_rate))
   {
      //Lower of two possible output sampling rates, so use index[0]
      resamp_index = 0;
   }
   else if((((8 == in_samp_rate)||(16 == in_samp_rate))&&(32 == out_samp_rate))||(16 == out_samp_rate))
   {
      resamp_index = 1;
    }
   return resamp_index;
}

static ADSPResult voice_mixer_resampler_init(vmx_in_port_info_t *vmx_port_ptr,uint32_t in_samp_rate,uint32_t out_samp_rate1,uint32_t out_samp_rate2,uint32_t out_samp_rate3)
{
    ADSPResult result = ADSP_EOK;
    uint32_t out_frame_samples,resample_result,out_samp_rate[3];
    uint32_t in_frame_samples = in_samp_rate / 50;
    out_samp_rate[0] = out_samp_rate1;
    out_samp_rate[1] = out_samp_rate2;
    out_samp_rate[2] = out_samp_rate3;

    for (int i=0; i < 3; i++)
    {
	    // only if resampler is already created reconfigure the resampler, else it will be taken care during creation of re-sampler
       if( NULL != vmx_port_ptr->resampler[i].channel_mem_ptr)
       {
	        voice_resampler_set_config(&(vmx_port_ptr->resampler[i].resamp_config),
	                in_samp_rate,out_samp_rate[i],NO_OF_BITS_PER_SAMPLE,in_frame_samples,&out_frame_samples);


           resample_result = voice_resampler_channel_init(
                                    &(vmx_port_ptr->resampler[i].resamp_config),
                                    vmx_port_ptr->resampler[i].channel_mem_ptr,
                                    vmx_port_ptr->resampler[i].channel_mem_size
                                    );
           if(VOICE_RESAMPLE_SUCCESS != resample_result)
           {
               return ADSP_EFAILED;
           }
	        MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: reconfiguring re-sampler idx(%ld) for input SR (%ld) output SR (%ld)",i,in_samp_rate,out_samp_rate[i]);
       }
       else
       {
	        return ADSP_EOK;
       }
    }

return result;
}
