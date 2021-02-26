
/*========================================================================
 *//** @file StreamRouterSvc.cpp
This file contains functions for Stream Router Service.

Copyright (c) 2012-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
  *//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/stream_router_svc/core/lib/src/StreamRouterSvc_Utils.cpp#1 $


when       who     what, where, why
--------   ---     -------------------------------------------------------
5/19/11    RP      Created file.
1/19/12    RP      Conversion of stream router to static service.
4/10/14    YW      AvSync Changes and 61937 Modules Unification
========================================================================== */


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */

#include "qurt_elite.h"
#include "Elite.h"
#include "EliteMsg_Custom.h"
#include "AudioStreamMgr_AprIf.h"
#include "StreamRouterSvc.h"
#include "adsp_asm_api.h"
#include "adsp_media_fmt.h"
#include "adsp_media_fmt_prop.h"
#include "adsp_asm_session_commands.h"
#include "AudioStreamMgr_GetSetBits.h"
#include "StreamRouterSvcStructs.h"
#include "AudioStreamMgr_PrivateDefs.h"
#include "EliteLoggingUtils_i.h"
#include "AudDevMgr_AprIf.h"
#include "avsync_lib.h"
#include "avsync_stats_lib.h"
#include "IEC61937.h"
#include "dsdUtil.h"
#include <audio_basic_op_ext.h>

#define DBG_MEDIA_FMT_SNIFF


static char RX_STREAM_ROUTER_SVC_INPUT_DATA_Q_NAME[] = "RxSd";
static char TX_STREAM_ROUTER_SVC_INPUT_DATA_Q_NAME[] = "TxSd";
static char RX_STREAM_ROUTER_SVC_OUTPUT_DATA_Q_NAME[] = "RxSb";
static char TX_STREAM_ROUTER_SVC_OUTPUT_DATA_Q_NAME[] = "TxSb";
static const uint32_t ZERO = 0;

/* =======================================================================
 **                          Function Definitions
 ** ======================================================================= */
/*======================================================================

FUNCTION      stream_router_service_create_output_buffers

DESCRIPTION   creates the output data buffers for this service and
adds them to the buffer queue. This is done at the time of connecting
to stream side

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/
ADSPResult stream_router_svc_create_output_buffers(stream_router_svc_t *pMe, uint32_t port_id, uint32_t connected_input_port, uint32_t num_out_buf_to_create)
{
   ADSPResult result = ADSP_EOK;

   if(RX_STRTR_ID == pMe->router_id)
   {
      result = stream_router_svc_get_output_buffer_size(pMe, connected_input_port, port_id, &pMe->out_port[port_id].output_buffer_size);
      if(ADSP_EOK != result)
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Illegal format ID");
         return result;
      }
   }
   else
   {
      pMe->out_port[port_id].output_buffer_size = TX_ROUTER_OUT_BUF_SIZE;
   }

   MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, " out port %lu, in port %lu. creating %lu buffers of size %lu bytes",
         port_id, connected_input_port,
         num_out_buf_to_create, pMe->out_port[port_id].output_buffer_size);

   uint32_t req_size = GET_ELITEMSG_DATABUF_REQ_SIZE(pMe->out_port[port_id].output_buffer_size);
   if (0 == req_size)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Buffer size 0. Continuing");
      return result;

   }

   for (uint32_t i = 0; i < num_out_buf_to_create; i++)
   {
      void *out_buf_ptr =  qurt_elite_memory_malloc(req_size, ADEC_SVC_OUT_BUF);

      if (!out_buf_ptr)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Insufficient memory to create output buffers of size %lu bytes for stream router service",req_size);
         return ADSP_ENOMEMORY;
      }

      memset(out_buf_ptr, 0, pMe->out_port[port_id].output_buffer_size);

      //initialize the parameters in the output buffer
      elite_msg_data_buffer_t *output_data_buf_ptr  = (elite_msg_data_buffer_t *)out_buf_ptr;
      stream_router_svc_init_buf_params(output_data_buf_ptr, pMe, port_id);

      //push the buffer into output buffer queue of router
      if (ADSP_FAILED(result = elite_msg_push_payload_to_returnq(pMe->out_port[port_id].service_handle.gpQ, (elite_msg_any_payload_t*) out_buf_ptr)))
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to fill Stream Router Svc buffer queue");
         //free the buffer
         qurt_elite_memory_free(out_buf_ptr);
         return result;
      }

      pMe->out_port[port_id].num_bufs_allocated++;
   }

   return result;
}

/*======================================================================

FUNCTION      stream_router_svc_check_recreate_out_buf

DESCRIPTION   this function frees the old buffer and creates a new one if required.

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/
ADSPResult stream_router_svc_check_recreate_out_buf(stream_router_svc_t *pMe, uint32_t output_port_id ,
      elite_msg_data_buffer_t **out_buf_ptr_ptr)
{
   stream_router_output_port_t *out_port_ptr = &pMe->out_port[output_port_id];
   elite_msg_data_buffer_t *out_buf_ptr = *out_buf_ptr_ptr;

   if ((uint32_t)out_buf_ptr->nMaxSize != out_port_ptr->output_buffer_size)
   {
      qurt_elite_memory_free(out_buf_ptr);
      out_port_ptr->num_bufs_allocated -= 1;

      //if output buffer size is nonzero, create one.
      if (out_port_ptr->output_buffer_size)
      {
         uint32_t req_size = GET_ELITEMSG_DATABUF_REQ_SIZE(out_port_ptr->output_buffer_size);

         void *buf_ptr =  qurt_elite_memory_malloc(req_size, ADEC_SVC_OUT_BUF);

         if (!buf_ptr)
         {
            MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Insufficient memory to recreate output buffers of size %lu bytes for stream router service", req_size);
            return ADSP_ENOMEMORY;
         }

         memset(buf_ptr, 0, out_port_ptr->output_buffer_size);

         //initialize the parameters in the output buffer
         out_buf_ptr  = (elite_msg_data_buffer_t *)buf_ptr;
         stream_router_svc_init_buf_params(out_buf_ptr, pMe, output_port_id);

         out_port_ptr->num_bufs_allocated += 1;

         *out_buf_ptr_ptr = out_buf_ptr;
         
         if(TX_STRTR_ID == pMe->router_id)
         {
           stream_router_svc_calculate_delay(pMe,output_port_id);
         }         
      }
      else
      {
         *out_buf_ptr_ptr = NULL;
         if ( (0 == pMe->out_port[output_port_id].num_bufs_allocated))
         {
            //stop listening to output port once all buffers are returned.
            pMe->current_bitfield &= ~(stream_router_svc_output_port_to_signal_mask(output_port_id));
            qurt_elite_queue_destroy(pMe->out_port[output_port_id].service_handle.gpQ);
            pMe->out_port[output_port_id].service_handle.gpQ = NULL;
         }

         return ADSP_EOK;
      }
   }
   return ADSP_EOK;
}

/*======================================================================

FUNCTION      stream_router_svc_destroy_out_bufs

DESCRIPTION   destroys output buffers

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/
void stream_router_svc_destroy_out_bufs(stream_router_svc_t* pMe, uint32_t port_id, bool_t need_poll)
{
   // Wait for all buffers to be returned. This has to be a blocking operations
   qurt_elite_bufmgr_node_t bufNode;

   //if already holding on to an output buffer destroy it first
   if(pMe->out_port[port_id].data_msg.pPayload)
   {
      qurt_elite_memory_free(pMe->out_port[port_id].data_msg.pPayload);
      pMe->out_port[port_id].data_msg.pPayload = NULL;
      pMe->out_port[port_id].num_bufs_allocated -= 1;
   }

   uint32_t (*queue_fn) (qurt_elite_channel_t* , uint32_t ) ;
   if (need_poll)
   {
      queue_fn = qurt_elite_channel_poll;
   }
   else
   {
      queue_fn = qurt_elite_channel_wait;
   }

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, " out port %lu, trying to destroy %lu buffers",
         port_id, pMe->out_port[port_id].num_bufs_allocated);

   // Drain the buffers
   while (pMe->out_port[port_id].num_bufs_allocated > 0)
   {
      // wait/poll for buffers
      uint32_t res = queue_fn(qurt_elite_queue_get_channel(pMe->out_port[port_id].service_handle.gpQ),
            qurt_elite_queue_get_channel_bit(pMe->out_port[port_id].service_handle.gpQ));

      //if queue poll/wait doesn't return anything, then break.
      if (0 == res)
      {
         break;
      }
      // retrieve the buffer
      qurt_elite_queue_pop_front(pMe->out_port[port_id].service_handle.gpQ, (uint64_t*)&bufNode );

      // Free the buffer
      qurt_elite_memory_free(bufNode.pBuffer);
      pMe->out_port[port_id].num_bufs_allocated -= 1;
   }
}

//#define STRTR_TS_DEBUG 1
/*======================================================================

FUNCTION      stream_router_svc_copy_from_input_to_intermediate_buffer

DESCRIPTION   copies data from input buffer to intermediate output buffer

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/
void stream_router_svc_copy_from_input_to_intermediate_buffer(stream_router_input_port_t *in_port_ptr, stream_router_output_port_t *out_port_ptr)
{
   intermediate_buffer_t *intmermediate_buff_ptr = &out_port_ptr->intermediate_buff;
   elite_msg_data_buffer_t *data_buf_ptr = (elite_msg_data_buffer_t *)in_port_ptr->data_msg.pPayload;

   if(NULL == data_buf_ptr)
   {
      return;
   }

   //if we are beginning the copy into this intermediate buffer, then need to update the output timestamp
   if(0 == intmermediate_buff_ptr->actual_size)
   {
#ifdef STRTR_TS_DEBUG
      MSG_2(MSG_SSID_QDSP6,  DBG_HIGH_PRIO, "Stream router incoming timestamp %lu %lu\n", (uint32_t)(data_buf_ptr->ullTimeStamp>>32),  (uint32_t)data_buf_ptr->ullTimeStamp);
#endif

      out_port_ptr->ts_state.output_buffer_ts = data_buf_ptr->ullTimeStamp +
            stream_router_svc_buffer_size_to_duration(in_port_ptr, intmermediate_buff_ptr->input_buf_read_pos, out_port_ptr->sampling_rate, out_port_ptr->num_channels,out_port_ptr->bits_per_sample);
      out_port_ptr->ts_state.ts_valid = (bool_t)asm_get_timestamp_valid_flag(data_buf_ptr->nFlag);
   }

   uint32_t bytes_to_copy = intmermediate_buff_ptr->max_size - intmermediate_buff_ptr->actual_size;
   uint32_t rem_bytes_in_input_buffer = data_buf_ptr->nActualSize - intmermediate_buff_ptr->input_buf_read_pos;
   bytes_to_copy = (bytes_to_copy > rem_bytes_in_input_buffer) ? rem_bytes_in_input_buffer : bytes_to_copy;

   uint8_t *src_ptr = (uint8_t *)(&data_buf_ptr->nDataBuf) + intmermediate_buff_ptr->input_buf_read_pos;
   uint8_t *dst_ptr = (uint8_t *)(intmermediate_buff_ptr->address_ptr + intmermediate_buff_ptr->actual_size);

   memscpy(dst_ptr,bytes_to_copy, src_ptr, bytes_to_copy);

   intmermediate_buff_ptr->input_buf_read_pos += bytes_to_copy;
   intmermediate_buff_ptr->actual_size += bytes_to_copy;

   return;
}

/*======================================================================

FUNCTION      stream_router_svc_push_output_buffer_downstream

DESCRIPTION   pushes the output buffer downstream after setting the timestamp
and then updates the timestamp for next output buffer

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/
ADSPResult stream_router_svc_push_output_buffer_downstream(stream_router_input_port_t *in_port_ptr, stream_router_output_port_t *out_port_ptr)
{
   ADSPResult nResult = ADSP_EOK;
   elite_msg_data_buffer_t *data_buf_ptr = (elite_msg_data_buffer_t *)out_port_ptr->data_msg.pPayload;

   asm_set_timestamp_valid_flag(&data_buf_ptr->nFlag, out_port_ptr->ts_state.ts_valid);
   data_buf_ptr->ullTimeStamp = out_port_ptr->ts_state.output_buffer_ts;

#ifdef STRTR_TS_DEBUG
   MSG_2(MSG_SSID_QDSP6,  DBG_HIGH_PRIO, "Stream router output timestamp %lu %lu\n", (uint32_t)(data_buf_ptr->ullTimeStamp>>32),  (uint32_t)data_buf_ptr->ullTimeStamp);
#endif

   //now push this into downstream service input data queue
   out_port_ptr->data_msg.unOpCode = ELITE_DATA_BUFFER;
   nResult= qurt_elite_queue_push_back(out_port_ptr->downstream_svc_handle->dataQ, (uint64_t*)&out_port_ptr->data_msg);
   if(ADSP_FAILED(nResult))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to deliver buffer dowstream");
      (void) elite_msg_push_payload_to_returnq(out_port_ptr->service_handle.gpQ, (elite_msg_any_payload_t*) data_buf_ptr);
      return nResult;
   }

   out_port_ptr->data_msg.pPayload = NULL;

   return nResult;
}

uint32_t stream_router_svc_get_pause_duration(stream_router_input_port_t *in_port_ptr, stream_router_output_port_t *out_port_ptr)
{
   switch (in_port_ptr->operation_mode)
   {
   case IEC_61937:
      return stream_router_svc_60958_frames_to_duration(IEC61937_get_pause_rep_period(in_port_ptr->format_id),
            out_port_ptr->sampling_rate, out_port_ptr->num_channels);
      break;
   case DSD_DOP:
   case GENERIC_COMPRESSED_MODE:
      return DSD_DEFAULT_PAUSE_DURATION_US;
      break;
   default:
      return 0;
      break;
   }
   return 0;
}

/*======================================================================

FUNCTION      stream_router_svc_fill_pause_burst

DESCRIPTION   Fills the given output data buffer with the required number
of pause data bursts.

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/
void stream_router_svc_fill_pause_burst(stream_router_svc_t* pMe,
      elite_msg_data_buffer_t *buf_msg_ptr,
      uint32_t pause_duration_us,
      uint32_t input_port_id,
      uint32_t output_port_id)
{
   if ( (input_port_id >= STRTR_MAX_PORTS) || (output_port_id >= STRTR_MAX_PORTS) )
   {
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Stream router: invalid port ID %lu,%lu",input_port_id, output_port_id);
      return;
   }
   stream_router_input_port_t *in_port_ptr = &(pMe->in_port[input_port_id]);
   stream_router_output_port_t *out_port_ptr = &(pMe->out_port[output_port_id]);
   uint8_t *data_buf_ptr = (uint8_t *)&buf_msg_ptr->nDataBuf;

   if(IEC_61937 == in_port_ptr->operation_mode)
   {
      uint32_t num_pause_frames = stream_router_svc_duration_to_60958_frames(pause_duration_us, out_port_ptr->sampling_rate,out_port_ptr->num_channels);

      IEC61937_fill_pause_bursts_from_IEC60958_frames(pMe->in_port[input_port_id].format_id, data_buf_ptr, &num_pause_frames);

      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "filling %ld IEC 61937 pause bursts", num_pause_frames);

      buf_msg_ptr->nActualSize = (num_pause_frames) * IEC61937_BYTES_PER_IEC60958_FRAME;
   }
   else if (DSD_DOP == in_port_ptr->operation_mode)
   {
      uint32_t num_bytes_to_fill = buf_msg_ptr->nMaxSize;
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "output sampling rate %ld, num_channels %ld",  out_port_ptr->sampling_rate, out_port_ptr->num_channels);
      DSD_fill_pause_bursts(out_port_ptr->sampling_rate, data_buf_ptr, &num_bytes_to_fill, pause_duration_us, out_port_ptr->num_channels);

      buf_msg_ptr->nActualSize = num_bytes_to_fill;
   }
   else if(GENERIC_COMPRESSED_MODE == in_port_ptr->operation_mode)
   {
      uint32_t frame_size = 0;
      if(STRTR_INVALID_PORT_ID != in_port_ptr->primary_output_port)
      {
         elite_svc_get_frame_size(out_port_ptr->sampling_rate, &frame_size);

         uint32_t num_bytes_to_fill =  (pause_duration_us * frame_size * out_port_ptr->num_channels * out_port_ptr->bits_per_sample)/(1000 * BITS_PER_BYTE);

         if (num_bytes_to_fill == 0) //before media fmt comes.
         {
            num_bytes_to_fill = (uint32_t)buf_msg_ptr->nMaxSize;
         }
         else
         {
            if (num_bytes_to_fill > (uint32_t)buf_msg_ptr->nMaxSize)
            {
               num_bytes_to_fill = (uint32_t)buf_msg_ptr->nMaxSize;
            }
         }

         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "filling %lu generic compr pause bursts, %lu allowed",  num_bytes_to_fill, buf_msg_ptr->nMaxSize);

         memset(data_buf_ptr, 0, num_bytes_to_fill);
         buf_msg_ptr->nActualSize = num_bytes_to_fill;
      }
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6,  DBG_HIGH_PRIO, "ERROR: Stream router service received invalid operation mode %ld",in_port_ptr->operation_mode);
      return ;
   }
   return;
}


uint64_t stream_router_svc_buffer_size_to_duration(stream_router_input_port_t *in_port_ptr, uint32_t buffer_size, uint32_t sampling_rate, uint16_t num_channels, uint16_t bits_per_sample)
{
   if(IEC_61937 == in_port_ptr->operation_mode)
   {

      return stream_router_svc_60958_frames_to_duration(buffer_size/IEC61937_BYTES_PER_IEC60958_FRAME, sampling_rate,num_channels);
   }
   else if(DSD_DOP == in_port_ptr->operation_mode)
   {
      return ((uint64_t)buffer_size * NUM_US_IN_SECOND/(sampling_rate * num_channels * DSD_DOP_BYTES_PER_SAMPLE));
   }
   else if (GENERIC_COMPRESSED_MODE == in_port_ptr->operation_mode)
   {
      return ((uint64_t)buffer_size * NUM_US_IN_SECOND/(sampling_rate * num_channels * (bits_per_sample / BITS_PER_BYTE)));
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6,  DBG_HIGH_PRIO, "ERROR: Stream router service received invalid operation mode %ld",in_port_ptr->operation_mode);
      return 0;
   }
};
uint64_t stream_router_svc_remainder_to_duration(stream_router_input_port_t *in_port_ptr, uint32_t remainder_size, uint32_t sampling_rate, uint16_t num_channels, uint16_t bits_per_sample)
{
   if(IEC_61937 == in_port_ptr->operation_mode)
   {

      //IEC 61937 will ensure integer duration, therefore, just return 0 without calculation;
      return 0;
   }
   else if (DSD_DOP == in_port_ptr->operation_mode)
   {
      return ((uint64_t)remainder_size/(sampling_rate * num_channels * DSD_DOP_BYTES_PER_SAMPLE));
   }
   else if (GENERIC_COMPRESSED_MODE == in_port_ptr->operation_mode)
   {
      return ((uint64_t)remainder_size/(sampling_rate * num_channels * (bits_per_sample / BITS_PER_BYTE)));
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6,  DBG_HIGH_PRIO, "ERROR: Stream router service received invalid operation mode %ld",in_port_ptr->operation_mode);
      return 0;
   }
};

uint64_t stream_router_svc_duration_to_buffer_size(stream_router_input_port_t *in_port_ptr, uint64_t duration, uint32_t sampling_rate, uint16_t num_channels, uint16_t bits_per_sample)
{
   if(IEC_61937 == in_port_ptr->operation_mode)
   {
      return stream_router_svc_duration_to_60958_frames(duration, sampling_rate,num_channels);
   }
   else if(DSD_DOP == in_port_ptr->operation_mode)
   {
      return (duration * (sampling_rate * num_channels * DSD_DOP_BYTES_PER_SAMPLE)) / NUM_US_IN_SECOND;
   }
   else if (GENERIC_COMPRESSED_MODE == in_port_ptr->operation_mode)
   {
      uint64_t buffer_size = (duration *(sampling_rate * num_channels * (bits_per_sample / BITS_PER_BYTE)) / NUM_US_IN_SECOND);
      return buffer_size;
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6,  DBG_HIGH_PRIO, "ERROR: Stream router service received invalid operation mode %ld",in_port_ptr->operation_mode);
      return 0;
   }
}

uint64_t stream_router_svc_duration_to_remainder_size(stream_router_input_port_t *in_port_ptr, uint64_t duration, uint32_t sampling_rate, uint16_t num_channels, uint16_t bits_per_sample)
{
   if(IEC_61937 == in_port_ptr->operation_mode)
   {
      //IEC 61937 will ensure integer duration, therefore, just return 0 without calculation;
      return 0;
   }
   else if(DSD_DOP == in_port_ptr->operation_mode)
   {
      return (duration * (sampling_rate * num_channels * DSD_DOP_BYTES_PER_SAMPLE));
   }
   else if (GENERIC_COMPRESSED_MODE == in_port_ptr->operation_mode)
   {
      uint64_t buffer_size = (duration *(sampling_rate * num_channels * (bits_per_sample / BITS_PER_BYTE)) );
      return buffer_size;
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6,  DBG_HIGH_PRIO, "ERROR: Stream router service received invalid operation mode %ld",in_port_ptr->operation_mode);
      return 0;
   }
}
/*======================================================================

FUNCTION      stream_router_svc_init_buf_params

DESCRIPTION   resets the service handle for downstream module

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/
void stream_router_svc_init_buf_params
(
      elite_msg_data_buffer_t *buf_ptr,
      stream_router_svc_t *pMe,
      uint32_t port_id
)
{
   if(!buf_ptr)
   {
      return;
   }

   buf_ptr->nFlag                = 0;
   buf_ptr->ullTimeStamp         = 0;
   buf_ptr->nOffset              = 0;
   buf_ptr->nMaxSize             = pMe->out_port[port_id].output_buffer_size;
   buf_ptr->nActualSize          = 0;
   buf_ptr->unClientToken        = pMe->service_handle.unSvcId;
   buf_ptr->pBufferReturnQ       = pMe->out_port[port_id].service_handle.gpQ;
   buf_ptr->pResponseQ           = NULL;
   buf_ptr->unResponseResult     = 0;
}


/*======================================================================

FUNCTION      stream_router_svc_init_in_port

DESCRIPTION   Initializes input port parameters

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/
ADSPResult stream_router_svc_init_in_port(stream_router_svc_t *pMe, EliteMsg_CustomStreamRouterConnect* p_cmd_msg_payload, uint32_t port_id)
{

   ADSPResult result = ADSP_EOK;

   pMe->in_port[port_id].service_handle.cmdQ = pMe->service_handle.cmdQ;
   pMe->in_port[port_id].data_msg.pPayload = NULL;
   pMe->in_port[port_id].data_burst_duration = 0;
   pMe->in_port[port_id].is_first_buffer_received = FALSE;
   pMe->in_port[port_id].ts_state.input_buffer_ts = 0;
   pMe->in_port[port_id].ts_state.initial_session_clock = 0;
   pMe->in_port[port_id].ts_state.duration_rendered = 0;
   pMe->in_port[port_id].ts_state.is_asynchronous_mode = FALSE;
   pMe->in_port[port_id].ts_state.sync_to_next_ts = FALSE;
   pMe->in_port[port_id].ts_state.skip_update = FALSE;
   pMe->in_port[port_id].input_buffer_remainder_unit = 0;
   pMe->in_port[port_id].input_buffer_remainder_total = 0;
   pMe->in_port[port_id].format_id = p_cmd_msg_payload->format_id; //only useful for Rx
   pMe->in_port[port_id].is_connected = TRUE;
   //Assign operation mode to the input port according to the media fmt
   pMe->in_port[port_id].operation_mode = stream_router_get_operation_mode(pMe->in_port[port_id].format_id);

   if(RX_STRTR_ID == pMe->router_id)
   {
      result = stream_router_init_input_avsync(pMe, port_id);
   }

   return result;
}


/*======================================================================

FUNCTION      stream_router_svc_init_in_buffer_queue

DESCRIPTION   Initializes input buffer queue

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/
ADSPResult stream_router_svc_init_in_buffer_queue(stream_router_svc_t *pMe, uint32_t port_id)
{
   ADSPResult result = ADSP_EOK;
   char inp_buf_q_name[QURT_ELITE_DEFAULT_NAME_LEN];

   if(RX_STRTR_ID == pMe->router_id)
   {
      snprintf(inp_buf_q_name, QURT_ELITE_DEFAULT_NAME_LEN,"%s%x",RX_STREAM_ROUTER_SVC_INPUT_DATA_Q_NAME,(int)port_id);
   }
   else
   {
      snprintf(inp_buf_q_name, QURT_ELITE_DEFAULT_NAME_LEN,"%s%x",TX_STREAM_ROUTER_SVC_INPUT_DATA_Q_NAME,(int)port_id);
   }

   if (ADSP_FAILED(result = qurt_elite_queue_create(inp_buf_q_name, MAX_INP_DATA_Q_ELEMENTS, &(pMe->in_port[port_id].service_handle.dataQ))))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Failed to create input data queue \n");
      return result;
   }

   if (ADSP_FAILED(result = qurt_elite_channel_addq(&pMe->channel, pMe->in_port[port_id].service_handle.dataQ,
         stream_router_svc_input_port_to_signal_mask(port_id))))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Failed to add input data q to channel, result = %d", result);
      qurt_elite_queue_destroy(pMe->in_port[port_id].service_handle.dataQ);
      return result;
   }
   return result;
}


/*======================================================================

FUNCTION      stream_router_svc_init_out_port

DESCRIPTION   Initialize output port parameters

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/
ADSPResult stream_router_svc_init_out_port(stream_router_svc_t *pMe, EliteMsg_CustomStreamRouterConnect* p_cmd_msg_payload, uint32_t port_id)
{
   pMe->out_port[port_id].service_handle.cmdQ = pMe->service_handle.cmdQ;
   pMe->out_port[port_id].data_msg.pPayload = NULL;
   pMe->out_port[port_id].pause_buffer_duration = 0;
   pMe->out_port[port_id].downstream_buffer_duration = 0;
   pMe->out_port[port_id].downstream_svc_handle = p_cmd_msg_payload->svc_handle_ptr;
   pMe->out_port[port_id].ts_state.output_buffer_ts = 0;
   pMe->out_port[port_id].ts_state.ts_valid = FALSE;
   pMe->out_port[port_id].is_connected = TRUE;
   pMe->out_port[port_id].afe_delay_ptr = p_cmd_msg_payload->punAFEDelay;
   pMe->out_port[port_id].copp_buf_delay_ptr = p_cmd_msg_payload->punCoppBufDelay ? p_cmd_msg_payload->punCoppBufDelay : (uint32_t*)&ZERO;
   pMe->out_port[port_id].copp_alg_delay_ptr = p_cmd_msg_payload->punCoppAlgDelay ? p_cmd_msg_payload->punCoppAlgDelay : (uint32_t*)&ZERO;
   pMe->out_port[port_id].str_rtr_delay_ptr = p_cmd_msg_payload->punStrRtrDelay;
   pMe->out_port[port_id].afe_drift_ptr = p_cmd_msg_payload->pAfeDriftPtr;
   return ADSP_EOK;
}


/*======================================================================

FUNCTION      stream_router_svc_init_out_buffer_queue

DESCRIPTION   Initialize output buffer queue
   if queue is already created then max_num_bufs is not used.

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/
ADSPResult stream_router_svc_init_out_buffer_queue(stream_router_svc_t *pMe, uint32_t port_id, uint32_t connected_input_port, uint32_t max_num_bufs)
{

   ADSPResult result = ADSP_EOK;
   char out_buf_q_name[QURT_ELITE_DEFAULT_NAME_LEN];

   /** if no queue was created, then create the queue*/
   if (NULL == pMe->out_port[port_id].service_handle.gpQ)
   {
      pMe->out_port[port_id].max_num_bufs = max_num_bufs;

      if(RX_STRTR_ID == pMe->router_id)
      {
         snprintf(out_buf_q_name, QURT_ELITE_DEFAULT_NAME_LEN,"%s%x",RX_STREAM_ROUTER_SVC_OUTPUT_DATA_Q_NAME,(int)port_id);
      }
      else
      {
         snprintf(out_buf_q_name, QURT_ELITE_DEFAULT_NAME_LEN,"%s%x",TX_STREAM_ROUTER_SVC_OUTPUT_DATA_Q_NAME,(int)port_id);
      }

      if (ADSP_FAILED(result = qurt_elite_queue_create(out_buf_q_name, pMe->out_port[port_id].max_num_bufs, &(pMe->out_port[port_id].service_handle.gpQ))))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Failed to create output data queue %ld \n",pMe->out_port[port_id].max_num_bufs);
         return result;
      }

      if (ADSP_FAILED(result = qurt_elite_channel_addq(&pMe->channel, pMe->out_port[port_id].service_handle.gpQ,
            stream_router_svc_output_port_to_signal_mask(port_id))))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Failed to add output data q to channel, result = %d", result);
         qurt_elite_queue_destroy(pMe->out_port[port_id].service_handle.gpQ);
         return result;
      }
   }

   //never allocate more than what queue can hold.
   if (pMe->out_port[port_id].num_bufs_allocated < pMe->out_port[port_id].max_num_bufs)
   {
      result = stream_router_svc_create_output_buffers(pMe, port_id, connected_input_port,
            (pMe->out_port[port_id].max_num_bufs - pMe->out_port[port_id].num_bufs_allocated));

      if(ADSP_EOK != result)
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Failed to create output buffers\n");
         //destroy any output buffers that may have been created
         stream_router_svc_destroy_out_bufs(pMe, port_id, NO_POLLING_NEEDED);
         qurt_elite_queue_destroy(pMe->out_port[port_id].service_handle.gpQ);
         pMe->out_port[port_id].service_handle.gpQ = NULL;
         return result;
      }
   }

   //create intermediate output buffer of the same size as output buffer
   if (NULL == pMe->out_port[port_id].intermediate_buff.address_ptr)
   {
      pMe->out_port[port_id].intermediate_buff.address_ptr = (uint8_t *)qurt_elite_memory_malloc(pMe->out_port[port_id].output_buffer_size, ADEC_SVC_OUT_BUF);

      if(!pMe->out_port[port_id].intermediate_buff.address_ptr)
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Failed to create intermediate output buffers\n");
         //destroy any output buffers that may have been created
         stream_router_svc_destroy_out_bufs(pMe, port_id, NO_POLLING_NEEDED);
         qurt_elite_queue_destroy(pMe->out_port[port_id].service_handle.gpQ);
         pMe->out_port[port_id].service_handle.gpQ = NULL;
         return ADSP_ENORESOURCE;
      }

      pMe->out_port[port_id].intermediate_buff.actual_size = 0;
      pMe->out_port[port_id].intermediate_buff.is_updated = FALSE;
      pMe->out_port[port_id].intermediate_buff.max_size = pMe->out_port[port_id].output_buffer_size;
      pMe->out_port[port_id].intermediate_buff.input_buf_read_pos = 0;
   }

   return result;
}



/*======================================================================

FUNCTION      stream_router_svc_deinit_in_port

DESCRIPTION   De-init input port parameters

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/

ADSPResult stream_router_svc_deinit_in_port(stream_router_svc_t *pMe, uint32_t port_id)
{

   ADSPResult result = ADSP_EOK;

   pMe->in_port[port_id].format_id = 0;
   pMe->in_port[port_id].is_connected = FALSE;
   pMe->in_port[port_id].connected_output_ports = 0;
   pMe->in_port[port_id].connected_active_output_ports = 0;
   pMe->in_port[port_id].sampling_rate = 0;
   pMe->in_port[port_id].num_channels = 0;
   pMe->in_port[port_id].bits_per_sample = 0;

   if(RX_STRTR_ID == pMe->router_id)
   {
      result = stream_router_deinit_input_avsync(pMe, port_id);
   }

   return result;
}



/*======================================================================

FUNCTION      stream_router_svc_destroy_in_buffer_queue

DESCRIPTION   destroy input buffer queue

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/
void stream_router_svc_destroy_in_buffer_queue(stream_router_svc_t *pMe, uint32_t port_id)
{
   //first return any buffer being held on to
   if(pMe->in_port[port_id].data_msg.pPayload != NULL)
   {
      elite_msg_release_msg(&pMe->in_port[port_id].data_msg);
      pMe->in_port[port_id].data_msg.pPayload = NULL;
   }

   elite_svc_destroy_data_queue(pMe->in_port[port_id].service_handle.dataQ);

   return;
}

/*======================================================================

FUNCTION      stream_router_svc_deinit_out_port

DESCRIPTION   De-init output port parameters

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/
void stream_router_svc_deinit_out_port(stream_router_svc_t *pMe, uint32_t port_id)
{
   pMe->out_port[port_id].downstream_svc_handle = NULL;
   pMe->out_port[port_id].is_connected = FALSE;
   pMe->out_port[port_id].afe_delay_ptr = NULL;
   pMe->out_port[port_id].copp_buf_delay_ptr = NULL;
   pMe->out_port[port_id].copp_alg_delay_ptr = NULL;
   pMe->out_port[port_id].str_rtr_delay_ptr = NULL;
   pMe->out_port[port_id].sampling_rate = 0;
   pMe->out_port[port_id].num_channels = 0;
   pMe->out_port[port_id].bits_per_sample = 0;
   pMe->out_port[port_id].data_msg.pPayload = NULL;
   pMe->out_port[port_id].connected_input_port = STRTR_INVALID_PORT_ID;

   return;
}

/*======================================================================

FUNCTION      stream_router_svc_destroy_out_buffer_queue

DESCRIPTION   destroy output buffer queue

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/
void stream_router_svc_destroy_out_buffer_queue(stream_router_svc_t *pMe, uint32_t port_id, bool_t need_poll)
{
   //destroy the output buffers
   stream_router_svc_destroy_out_bufs(pMe, port_id, need_poll);

   //if all the bufs are freed, then destroy queue.
   if ( pMe->out_port[port_id].num_bufs_allocated == 0 )
   {
      qurt_elite_queue_destroy(pMe->out_port[port_id].service_handle.gpQ);
      pMe->out_port[port_id].service_handle.gpQ = NULL;
   }

   //destroy the intermediate output buffer
   if(NULL != pMe->out_port[port_id].intermediate_buff.address_ptr)
   {
      qurt_elite_memory_free(pMe->out_port[port_id].intermediate_buff.address_ptr);
      pMe->out_port[port_id].intermediate_buff.address_ptr = NULL;
   }

   return;
}

/*======================================================================

FUNCTION      stream_router_svc_connect_input_to_output

DESCRIPTION   Connect input port to output port

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/
ADSPResult stream_router_svc_connect_input_to_output(stream_router_svc_t *pMe, uint32_t input_port_id, uint32_t output_port_id)
{
   pMe->in_port[input_port_id].connected_output_ports |= (1UL << output_port_id);
   pMe->out_port[output_port_id].connected_input_port = input_port_id;
   return ADSP_EOK;
}


/*======================================================================

FUNCTION      stream_router_svc_disconnect_input_from_output

DESCRIPTION   Disconnect the routing from given input port to given output port

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/
ADSPResult stream_router_svc_disconnect_input_from_output(stream_router_svc_t *pMe, uint32_t input_port_id, uint32_t output_port_id)
{
   if(pMe->out_port[output_port_id].connected_input_port != input_port_id)
   {
      //given input port is not connected to given output port
      return ADSP_EFAILED;
   }

   pMe->in_port[input_port_id].connected_output_ports &= ~(1UL << output_port_id);
   pMe->out_port[output_port_id].connected_input_port = STRTR_INVALID_PORT_ID;
   return ADSP_EOK;
}


/*======================================================================

FUNCTION      stream_router_svc_rx_mapping

DESCRIPTION   For Rx stream router set up the connection between stream
and device sides

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/
void stream_router_svc_rx_mapping(stream_router_svc_t *pMe, uint32_t input_port_id, uint32_t output_port_id)
{
   stream_router_output_port_t *out_port_ptr = &pMe->out_port[output_port_id];
   stream_router_input_port_t *in_port_ptr = &pMe->in_port[input_port_id];

   //start listening to output queue. if stream is not in RUN, start pumping pause bursts
   pMe->current_bitfield |= stream_router_svc_output_port_to_signal_mask(output_port_id);
   //if input port has already received a media type, then propagate the values to output port
   if((0 != in_port_ptr->sampling_rate) && (0 != in_port_ptr->num_channels) && (0 != in_port_ptr->bits_per_sample))
   {
      if((out_port_ptr->sampling_rate != in_port_ptr->sampling_rate)
            || (out_port_ptr->num_channels != in_port_ptr->num_channels)
            || (out_port_ptr->bits_per_sample != in_port_ptr->bits_per_sample))
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Sampling rate on stream and device side do not match ");
         //set it to the same as input side for consistency
         out_port_ptr->sampling_rate = in_port_ptr->sampling_rate;
         out_port_ptr->num_channels = in_port_ptr->num_channels;
         out_port_ptr->bits_per_sample =  in_port_ptr->bits_per_sample;
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO," bits per sample set as %lu",in_port_ptr->bits_per_sample);
      }
   }

   //set the pause burst duration at the output to preset duration*/
   pMe->out_port[output_port_id].pause_buffer_duration = PAUSE_BURST_DURATION_IN_US;
   stream_router_svc_connect_input_to_output(pMe, input_port_id, output_port_id);

   return;
}

/*======================================================================

FUNCTION      stream_router_svc_create_and_send_media_type

DESCRIPTION   Create and send media type message downstream

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/
ADSPResult stream_router_svc_create_and_send_media_type(stream_router_svc_t* pMe, uint32_t output_port_id,
      uint32_t sampling_rate, uint16_t num_channels, uint16_t bits_per_sample, uint32_t format_id)
{
   ADSPResult result = ADSP_EOK;
   elite_msg_any_t media_type_msg;

   stream_router_output_port_t *out_port_ptr = &pMe->out_port[output_port_id];

   uint32_t payload_size = sizeof(elite_msg_data_media_type_apr_t);

   if ( format_id == ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V2)
   {
      payload_size += sizeof(elite_multi_channel_pcm_fmt_blk_t);
   }
   else
   {
      payload_size += sizeof(elite_compressed_fmt_blk_t);
   }

   result = elite_msg_create_msg( &media_type_msg, &payload_size, ELITE_DATA_MEDIA_TYPE, NULL, 0,0  );
   if(ADSP_EOK != result)
   {
      return ADSP_ENORESOURCE;
   }

   elite_msg_data_media_type_apr_t *media_type_ptr = (elite_msg_data_media_type_apr_t *)media_type_msg.pPayload;
   media_type_ptr->unMediaTypeFormat = ELITEMSG_DATA_MEDIA_TYPE_APR;

   if (format_id == ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V2)
   {
      media_type_ptr->unMediaFormatID   = ELITEMSG_MEDIA_FMT_MULTI_CHANNEL_PCM;
      elite_multi_channel_pcm_fmt_blk_t *pcm_format_blk_ptr =
            (elite_multi_channel_pcm_fmt_blk_t*) ((uint8_t*)(media_type_ptr) + sizeof(elite_msg_data_media_type_apr_t));
      pcm_format_blk_ptr->num_channels = num_channels;
      pcm_format_blk_ptr->sample_rate = sampling_rate;
      pcm_format_blk_ptr->bits_per_sample = bits_per_sample;
      pcm_format_blk_ptr->is_signed = 1;
      pcm_format_blk_ptr->is_interleaved = 1;
      //for compressed channel mapping is immaterial
      memset(&pcm_format_blk_ptr->channel_mapping[0], 0, sizeof(pcm_format_blk_ptr->channel_mapping));
   }
   else
   {
      media_type_ptr->unMediaFormatID   = ELITEMSG_MEDIA_FMT_COMPRESSED;
      elite_compressed_fmt_blk_t *format_blk_ptr =
            (elite_compressed_fmt_blk_t*) ((uint8_t*)(media_type_ptr) + sizeof(elite_msg_data_media_type_apr_t));
      format_blk_ptr->num_channels = num_channels;
      format_blk_ptr->sample_rate = sampling_rate;
      format_blk_ptr->bits_per_sample = bits_per_sample;
      format_blk_ptr->media_format = format_id;
   }

   if(NULL == out_port_ptr->downstream_svc_handle)
   {
      elite_msg_release_msg(&media_type_msg);
      return ADSP_ENOTREADY;
   }

   result = qurt_elite_queue_push_back(out_port_ptr->downstream_svc_handle->dataQ, (uint64_t*)&media_type_msg);
   if(ADSP_EOK != result)
   {
      elite_msg_release_msg(&media_type_msg);
   }

   MSG_6(MSG_SSID_QDSP6, DBG_HIGH_PRIO, " Sent media format from output port %lu, with result %d,num_channels %u, sample_rate %lu,bits_per_sample %lu, format_id 0x%lx ",
         output_port_id, result, num_channels,sampling_rate,bits_per_sample,format_id );
   return result;
}

/*======================================================================

FUNCTION      is_top_prio_output_port

DESCRIPTION   Check if the given output port is the top priority output port.
It is the one that is being set from ADM.

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/
bool_t is_top_prio_output_port(stream_router_svc_t *pMe, uint32_t port_id)
{

   uint32_t input_port_id = pMe->out_port[port_id].connected_input_port;
   return (pMe->in_port[input_port_id].primary_output_port == port_id);
}


uint32_t stream_router_svc_sniff_media_format(stream_router_svc_t *pMe, uint32_t port_id, uint8_t *data_buf, uint32_t data_length)
{
   ADSPResult result = ADSP_EOK;
   stream_router_input_port_t *in_port_ptr = &(pMe->in_port[port_id]);
   uint16_t *bitstream_pointer = (uint16_t *)data_buf;
   uint32_t curr_position = 0;
   uint32_t media_format = in_port_ptr->format_id;
   //First tell if need to sniff in this buffer at all.

   //Search for sync
   while(curr_position < data_length)
   {
      //If buffer is smaller than compr frame, update and return
      if((data_length-curr_position) < in_port_ptr->bytes_to_next_compr_frame)
      {
#ifdef DBG_MEDIA_FMT_SNIFF
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"frame length less than data length, returning");
#endif
         in_port_ptr->bytes_to_next_compr_frame -= (data_length-curr_position);
         return ADSP_EOK;
      }
      //Update position to the expected position of next sync
      curr_position += in_port_ptr->bytes_to_next_compr_frame;
      in_port_ptr->bytes_to_next_compr_frame = 0;

      result = IEC61937_get_next_syncword_locn_and_update(bitstream_pointer+(curr_position >> 1),data_length,
            &curr_position);
      if(curr_position >=data_length - 4)
      {
         goto __bailout;
      }

      //Decode Preamble
#ifdef DBG_MEDIA_FMT_SNIFF
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"manual check sync word: %lx",
            *(uint32_t *)(bitstream_pointer+(curr_position >> 1) - 2));
#endif
      result =IEC61937_parse_media_info(bitstream_pointer[curr_position >> 1],&in_port_ptr->bytes_to_next_compr_frame, &media_format);
#ifdef DBG_MEDIA_FMT_SNIFF
      MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"Stream router sniffed media format as %lx, frame length %lx, data length %lx",
            media_format,in_port_ptr->bytes_to_next_compr_frame,(data_length-curr_position));
#endif
      if(ADSP_EOK != result)
      {
         goto __bailout;
      }
      //Detect change and raise event for changes.
      if(media_format != in_port_ptr->format_id)
      {//Media format has changed.
         MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
               "Stream router sniffed media format change: current: %lx, incoming: %lx, port_id %lx",
               in_port_ptr->format_id, media_format, port_id);
         if(pMe->in_port[port_id].media_fmt_cb.registered)
         {
            adm_iec_61937_media_fmt_event_t mediaFmtEvent;
            //Set event payload
            mediaFmtEvent.new_format_id = media_format;
            mediaFmtEvent.device_id = in_port_ptr->media_fmt_cb.device_id;

            //APR alloc memory and send the cmdrsp to client processor.
            result = AudDevMgr_GenerateClientCb(&in_port_ptr->media_fmt_cb,
                  ADM_IEC_61937_MEDIA_FMT_EVENT, ///opcode
                  &(mediaFmtEvent),
                  sizeof(mediaFmtEvent));
            MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudDevMgr: IEC61937 media fmt update event!! res 0x%lx",(uint32_t)result);

         }
      }
      in_port_ptr->format_id = media_format;
      pMe->in_port[port_id].operation_mode = stream_router_get_operation_mode(pMe->in_port[port_id].format_id);
      curr_position+=4;

   }
   return ADSP_EOK;
   __bailout:
   return ADSP_EFAILED;
}

/*======================================================================

FUNCTION      stream_router_svc_push_eos_downstream

DESCRIPTION   For Rx stream router set up the connection between stream
and device sides

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/
ADSPResult stream_router_svc_push_eos_downstream(stream_router_svc_t* pMe, uint32_t output_port_id)
{
   stream_router_output_port_t *out_port_ptr = &pMe->out_port[output_port_id];
   uint32_t input_port_id = out_port_ptr->connected_input_port;

   if(out_port_ptr->eos_msg.pPayload == NULL)
   {
      return ADSP_EOK;
   }

   ADSPResult result = qurt_elite_queue_push_back(out_port_ptr->downstream_svc_handle->dataQ, (uint64_t*)&out_port_ptr->eos_msg);

   if(ADSP_EOK != result)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO," Falied to push EOS message downstream (IP Port:%lu, OP Port:%lu)",
            input_port_id, output_port_id);
      elite_msg_release_msg(&out_port_ptr->eos_msg);
   }
   else
   {
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO," Pushed EOS message downstream (IP Port:%lu, OP Port:%lu)",
            input_port_id, output_port_id);
   }
   out_port_ptr->eos_msg.pPayload= NULL;

   /** for Tx stream router, AFE/COPP would be sending proper EoSes hence below protection is not necessary.
    * In addition, inport must be valid */
   if ( (STRTR_INVALID_PORT_ID != input_port_id) && (pMe->router_id == RX_STRTR_ID))
   {
      stream_router_input_port_t *in_port_ptr = &pMe->in_port[input_port_id];

      //check if other connected output ports have EoS pending. if none have an EoS pending, then change listen to input again.
      //this makes sure that EoS is sent to all output ports & client receives rendered event (in Rx) or Eos (in Tx)
      uint32_t out_bitfield = stream_router_svc_get_connected_active_out_ports(pMe, in_port_ptr);
      uint32_t output_port_id = 0;
      bool_t eos_pending = false;
      while(out_bitfield != 0)
      {
         if(out_bitfield & 1)
         {
            stream_router_output_port_t *temp_out_port_ptr = &pMe->out_port[output_port_id];
            if (temp_out_port_ptr->eos_msg.pPayload != NULL )
            {
               eos_pending = true;
               break;
            }
         }
         out_bitfield >>= 1;
         output_port_id++;
      }

      //if EoS is pending then stop listen (although stop listen was done right after EoS was received, it could be
      //ORed back in some cases in stream_router_svc_process_output_data_q_rx_mode) to the input queue otherwise listen.
      if (eos_pending)
      {
         pMe->current_bitfield &= ~(stream_router_svc_input_port_to_signal_mask(input_port_id));
      }
      else
      {
         pMe->current_bitfield |= stream_router_svc_input_port_to_signal_mask(input_port_id);
      }
   }
   return result;
}

/*======================================================================

FUNCTION      stream_router_svc_get_output_buffer_size

DESCRIPTION   get the output buffer size for the given format Id

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/
ADSPResult stream_router_svc_get_output_buffer_size(stream_router_svc_t *pMe,
      uint32_t in_port_id,  uint32_t out_port_id, uint32_t *buffer_size)
{
   ADSPResult result = ADSP_EOK;

   stream_router_input_port_t *in_port_ptr = &pMe->in_port[in_port_id];
   stream_router_output_port_t *out_port_ptr = &pMe->out_port[out_port_id];

   /*          Calculation of output buffer size for non-IEC-61937 data       */
   //   For non-IEC-61937 data, we need to make sure that we can both hold pause
   // duration which is 10ms, and also the decoder output buffer size, which is
   // the threshold value of each capi. We will accept the maximum value of the two.
   if (ASM_MEDIA_FMT_DSD == in_port_ptr->format_id)
   {
      uint32_t samples_per_channel;
      elite_svc_get_frame_size(out_port_ptr->sampling_rate, &samples_per_channel);
      //For DSD, the buffer size will be 8192, we take the maximum value of that versus the pause buffer duration
      uint32_t pause_buffer_size  = out_port_ptr->num_channels * samples_per_channel * DSD_DOP_BYTES_PER_SAMPLE*PAUSE_BURST_DURATION_IN_MS;

      uint32_t decoder_output_buffer_size = DSD_DOP_MAX_BUFFER_SIZE;
      *buffer_size = (pause_buffer_size > decoder_output_buffer_size ) ? pause_buffer_size : decoder_output_buffer_size;
   }
   else if (ASM_MEDIA_FMT_GENERIC_COMPRESSED == in_port_ptr->format_id)
   {
      uint32_t samples_per_channel;
      elite_svc_get_frame_size(out_port_ptr->sampling_rate, &samples_per_channel);
      //For compressed passthru, there is no hard constraint on the decoder output buffer size.
      // for simplicity, I am ensuring the same threshold value as in DSD use case.
      // and that is why I am using dsd max buffer size;
      uint32_t pause_buffer_size = out_port_ptr->num_channels * samples_per_channel * (out_port_ptr->bits_per_sample/BITS_PER_BYTE)*PAUSE_BURST_DURATION_IN_MS;
      uint32_t decoder_output_buffer_size = DSD_DOP_MAX_BUFFER_SIZE;
      *buffer_size = (pause_buffer_size > decoder_output_buffer_size ) ? pause_buffer_size : decoder_output_buffer_size;

   }
   else if (in_port_ptr->format_id == 0)
   {
      //in 61937 passthrough use case, DSP autodetects formats when client opens with fmt_id=0.
      //so until fmt is auto detected, use a random output buf size. it will be useful for transmitting initial pause bursts in client given data.
      *buffer_size = 92160; //this is the size used in passthru CAPI.
   }
   else
   {

      result = IEC61937_get_buffer_size( in_port_ptr->format_id, buffer_size);
   }

   MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Stream router: output buffer size of %lu (previous %lu) for fmt-id 0x%lx", *buffer_size, out_port_ptr->output_buffer_size, in_port_ptr->format_id);

   return result;
}

ADSPResult stream_router_svc_send_media_type_to_connected_output_ports(stream_router_svc_t *pMe,
      uint32_t input_port_id, uint32_t specific_out_port_id)
{
   ADSPResult result = ADSP_EOK;

   if(input_port_id >= STRTR_MAX_PORTS)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Stream router: invalid session ID %lu",input_port_id);
      return ADSP_EFAILED;
   }
   stream_router_input_port_t *in_port_ptr = &pMe->in_port[input_port_id];

   if ((in_port_ptr->sampling_rate == 0) || (in_port_ptr->num_channels == 0))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, " media type not set on input yet on %lu",input_port_id);
      return ADSP_EOK;
   }

   //send the media type downstream on each of the connected output ports
   uint32_t temp_bitfield = stream_router_svc_get_connected_active_out_ports(pMe, in_port_ptr);
   uint32_t output_port_id = 0;
   while(temp_bitfield != 0)
   {
      if(temp_bitfield & 1)
      {
         //in case we have a specific out port, then send to it only & skip others.
         //else send to all
         if ( (STRTR_INVALID_PORT_ID == specific_out_port_id) ||
               ((STRTR_INVALID_PORT_ID != specific_out_port_id) && (specific_out_port_id == output_port_id)))
         {

            stream_router_output_port_t *out_port_ptr = &pMe->out_port[output_port_id];

            if((0 != out_port_ptr->sampling_rate) && (0 != out_port_ptr->num_channels))
            {
               //just print the error and continue processing
               if((out_port_ptr->sampling_rate != in_port_ptr->sampling_rate) ||
                     (out_port_ptr->num_channels != in_port_ptr->num_channels)||
                     (out_port_ptr->bits_per_sample != in_port_ptr->bits_per_sample))
               {
                  MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Media type params don't match params from AFE connect");
                  MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO," Media type - Sampling rate = %lu, num channels =%u, bits_per_sample = %lu",
                        in_port_ptr->sampling_rate, in_port_ptr->num_channels, in_port_ptr->bits_per_sample);
                  MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO," AFE Connect - Sampling rate = %lu, num channels =%u, bits_per_sample = %lu",
                        out_port_ptr->sampling_rate, out_port_ptr->num_channels, in_port_ptr->bits_per_sample);
               }
            }

            uint32_t format_id;
            if (RX_STRTR_ID == pMe->router_id)
            {
               format_id = in_port_ptr->format_id;
            }
            else
            {
               format_id = ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V2;
            }

            //Set output port values to input values for consistency
            out_port_ptr->sampling_rate = in_port_ptr->sampling_rate;
            out_port_ptr->num_channels = in_port_ptr->num_channels;
            out_port_ptr->bits_per_sample = in_port_ptr->bits_per_sample;

            //now make a copy and send this to the downstream service output port is connected to
            //for compressed use cases the media type will not change during a session

            result |= stream_router_svc_create_and_send_media_type(pMe, output_port_id,
                  in_port_ptr->sampling_rate, in_port_ptr->num_channels, in_port_ptr->bits_per_sample, format_id);

            //in Rx stream router, incoming fmt_id may change from 0 to valid etc. buf needs to be recreated.
            if (RX_STRTR_ID == pMe->router_id)
            {
               //first check if the output buffers are already created. if already created then check if they are
               //same size as required for new stream (output buffer size is same as the data burst size for stream).
               //If so nothing more to be done. Otherwise destroy and recreate with new size
               if(out_port_ptr->num_bufs_allocated > 0)
               {
                  uint32_t buff_size = 0;
                  stream_router_svc_get_output_buffer_size(pMe, out_port_ptr->connected_input_port, output_port_id, &buff_size);

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
            }
            
            if(TX_STRTR_ID == pMe->router_id)
            {
              //update the tx stream router delay as media fmt got updated
              stream_router_svc_calculate_delay(pMe,output_port_id);
            }
            
            out_port_ptr->bw_vote = out_port_ptr->sampling_rate * out_port_ptr->num_channels * (out_port_ptr->bits_per_sample / BITS_PER_BYTE);
         }
      }
      temp_bitfield >>= 1;
      output_port_id++;
   }

   if (ADSP_FAILED(result)) result = ADSP_EFAILED;

   return result;
}

/*======================================================================

FUNCTION      stream_router_svc_process_media_type

DESCRIPTION   process the media type message at the input port

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/
ADSPResult stream_router_svc_process_media_type(stream_router_svc_t *pMe, uint32_t input_port_id, elite_msg_data_media_type_apr_t *media_type_ptr)
{
   ADSPResult result = ADSP_EOK;
   stream_router_input_port_t *in_port_ptr = &pMe->in_port[input_port_id];

   if (ELITEMSG_MEDIA_FMT_COMPRESSED == media_type_ptr->unMediaFormatID)
   {
      elite_compressed_fmt_blk_t *format_blk_ptr;

      //store the sampling rate/num channels from the media type
      format_blk_ptr = (elite_compressed_fmt_blk_t *)elite_msg_get_media_fmt_blk(media_type_ptr);
      MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO," Media Type Message: sr %ld, num_ch %ld, bits_per_sample %ld, format 0x%lx",
            format_blk_ptr->sample_rate,
            format_blk_ptr->num_channels,
            format_blk_ptr->bits_per_sample,
            format_blk_ptr->media_format);

      in_port_ptr->format_id = format_blk_ptr->media_format;
      //copy to input port now & when output port is connected, it can be copied.
      in_port_ptr->sampling_rate = format_blk_ptr->sample_rate;
      in_port_ptr->num_channels = format_blk_ptr->num_channels;
      in_port_ptr->bits_per_sample = format_blk_ptr->bits_per_sample;
   }
   else
   {
      elite_multi_channel_pcm_fmt_blk_t *format_blk_ptr;

      //store the sampling rate/num channels from the media type
      format_blk_ptr = (elite_multi_channel_pcm_fmt_blk_t *)elite_msg_get_media_fmt_blk(media_type_ptr);
      MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO," Media Type Message: sr %ld, num_ch %ld, bits_per_sample %ld",
            format_blk_ptr->sample_rate,
            format_blk_ptr->num_channels,
            format_blk_ptr->bits_per_sample);

      //copy to input port now & when output port is connected, it can be copied.
      in_port_ptr->sampling_rate = format_blk_ptr->sample_rate;
      in_port_ptr->num_channels = format_blk_ptr->num_channels;
      in_port_ptr->bits_per_sample = format_blk_ptr->bits_per_sample;

   }

   result = stream_router_svc_send_media_type_to_connected_output_ports(pMe, input_port_id, STRTR_INVALID_PORT_ID);

   in_port_ptr->bw_vote = in_port_ptr->sampling_rate * in_port_ptr->num_channels * (in_port_ptr->bits_per_sample / BITS_PER_BYTE);
   stream_router_svc_process_kpps_bw(pMe, FALSE, FALSE);

   return result;
}



/*======================================================================

FUNCTION      stream_router_svc_process_eos_message

DESCRIPTION   process the EOS message at the input port

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/
ADSPResult stream_router_svc_process_eos_msg(stream_router_svc_t *pMe, elite_msg_data_eos_apr_t *eos_msg_ptr, uint32_t output_port_bitfield, uint32_t input_port_id)
{
   ADSPResult result = ADSP_EOK;
   uint32_t output_port_id = 0;

   pMe->in_port[input_port_id].eos_pending_ack_mask = output_port_bitfield;
   pMe->in_port[input_port_id].eos_info = eos_msg_ptr->eosInfo;

   //copy the EOS message to each of the connected output ports
   while(output_port_bitfield != 0)
   {
      if(output_port_bitfield & 1)
      {
         stream_router_output_port_t *out_port_ptr = &pMe->out_port[output_port_id];
         QURT_ELITE_ASSERT(out_port_ptr->downstream_svc_handle);

         //copy the EOS message to each of the output buffers
         uint32_t eos_payload_size = sizeof( elite_msg_data_eos_apr_t );

         /** In EOS V2, matrix sends rendered_eos event to the client after all/any connected devices render EOS.*/
         qurt_elite_queue_t *respQPtr = NULL;
         if (eos_msg_ptr->eosInfo.event_opcode == ASM_DATA_EVENT_RENDERED_EOS_V2)
         {
            respQPtr = pMe->service_handle.cmdQ;
         }

         uint32_t clientToken = stream_router_svc_get_client_token_from_in_out_port_id(input_port_id, output_port_id);

         result = elite_msg_create_msg(&out_port_ptr->eos_msg, &eos_payload_size, ELITE_DATA_EOS, respQPtr, clientToken, 0 );
         if(ADSP_EOK != result)
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO," Error creating EOS message for downstream service(OP Port:%lu)", output_port_id);
         }
         else
         {
            elite_msg_data_eos_apr_t *pDst = (elite_msg_data_eos_apr_t*)out_port_ptr->eos_msg.pPayload;

            pDst->eosInfo = eos_msg_ptr->eosInfo;
            pDst->unEosFormat = eos_msg_ptr->unEosFormat;
         }
      }
      output_port_bitfield >>= 1;
      output_port_id++;
   }

   if (0 == pMe->in_port[input_port_id].eos_pending_ack_mask)
   {
      stream_router_svc_check_raise_rendered_eos_v2(pMe, NULL, input_port_id, 0, TRUE);
   }
   return result;
}

/*======================================================================

FUNCTION      stream_router_svc_log_data

DESCRIPTION   Log the data at the input of stream router

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/
ADSPResult stream_router_svc_log_data(stream_router_svc_t *pMe, int8_t *buf_addr, uint32_t buf_size, stream_router_input_port_t *in_port_ptr)
{
   ADSPResult result = ADSP_EOK;
   elite_log_info log_info_obj;

   /* Populate the packet to be sent to logging utility */
   if(RX_STRTR_ID == pMe->router_id)
   {
      log_info_obj.qxdm_log_code = QXDM_LOG_CODE_AUD_MTMX_RX_IN;
      log_info_obj.data_info.media_fmt_id = in_port_ptr->format_id;
   }
   else
   {
      log_info_obj.qxdm_log_code = QXDM_LOG_CODE_AUD_MTMX_TX_IN;
      log_info_obj.data_info.media_fmt_id = 0xFFFFF; /*For TX  format is not known*/
   }
   log_info_obj.buf_ptr = (uint8_t *)buf_addr;
   log_info_obj.buf_size = buf_size;
   log_info_obj.session_id = in_port_ptr->data_log_id;
   log_info_obj.log_tap_id = AUDIOLOG_STRTR_IN_TAP_ID;
   log_info_obj.log_time_stamp = qurt_elite_timer_get_time();
   log_info_obj.data_fmt = ELITE_LOG_DATA_FMT_BITSTREAM;

   /* Allocate the log buffer and log the packet
      If log code is disabled, log buffer allocation returns NULL
    */
   result = elite_allocbuf_and_log_data_pkt(&log_info_obj);

   return result;
}

ADSPResult stream_router_svc_process_mark_buffer(stream_router_svc_t *pMe, uint32_t input_port_id, uint16_t status)
{
   ADSPResult nResult = ADSP_EOK;
   stream_router_input_port_t *in_port_ptr = &pMe->in_port[input_port_id];

   //Raise the Mark Buffer Consumption Event
   elite_msg_data_mark_buffer_t *pMarkBufferPayload = (elite_msg_data_mark_buffer_t *)in_port_ptr->data_msg.pPayload;
   asm_data_event_mark_buffer_v2_t eventPayload;
   eventPayload.token_lsw = pMarkBufferPayload->token_lsw;
   eventPayload.token_msw = pMarkBufferPayload->token_msw;
   eventPayload.result = status;
   nResult = AudioStreamMgr_GenerateClientEventFromCb(in_port_ptr->asm_cb_handle.pCbHandle,
         ASM_DATA_EVENT_MARK_BUFFER_V2,
         0, &eventPayload, sizeof(eventPayload));
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"StreamRouter: Raised mark buffer event");
   stream_router_svc_release_buffer(&in_port_ptr->data_msg);
   return nResult;
}

/**
 * force_vote doesn't matter if is_release=TRUE
 *
 * is_release helps in releasing BW even when aggregated BW is nonzero, useful for suspend.
 *
 * force_vote helps in voting BW due to changes in svc & not due to CAPI V2 events.
 */
ADSPResult stream_router_svc_process_kpps_bw(stream_router_svc_t *pMe, bool_t is_release, bool_t force_vote)
{
   ADSPResult result = ADSP_EOK;

   uint32_t bw = 0;
   uint32_t kpps = 0;
   for (uint8_t i = 0; i < STRTR_MAX_PORTS; i++)
   {
      bw += pMe->in_port[i].bw_vote + pMe->out_port[i].bw_vote;
      kpps += pMe->in_port[i].kpps_vote + pMe->out_port[i].kpps_vote;
   }

   //If there was no event or no release-call, or no foce vote or there was no change, return.
   if (!(is_release || (pMe->kpps_vote != kpps) || (pMe->bw_vote != bw)))
   {
      return ADSP_EOK;
   }

#if (ADSPPM_INTEGRATION==1)
   static const uint8_t NUM_REQUEST=2;
   MmpmRscParamType rscParam[NUM_REQUEST];
   MMPM_STATUS      retStats[NUM_REQUEST];
   MmpmRscExtParamType reqParam;
   uint8_t req_num=0;
   MmpmMppsReqType mmpmMppsParam;
   MmpmGenBwValType bwReqVal;
   MmpmGenBwReqType bwReq;
   uint32_t mpps=0;

   reqParam.apiType                    = MMPM_API_TYPE_SYNC;
   reqParam.pExt                       = NULL;       //for future
   reqParam.pReqArray                  = rscParam;
   reqParam.pStsArray                  = retStats;   //for most cases mmpmRes is good enough, need not check this array.
   reqParam.reqTag                     = 0;          //for async only

   /** check if mips and bw are both releases or requests. both_diff => each != request or release. */
   bool_t both_diff = ( (kpps > 0) ^ (bw > 0) );

   //whether mips req is made
   bool_t is_mips_req = (pMe->kpps_vote != kpps) || is_release ;
   //whether bw req is made
   bool_t is_bw_req = ( (pMe->bw_vote != bw) || is_release );

   bool_t req_done = false;

   if ( is_mips_req )
   {
      pMe->kpps_vote = kpps;

      mpps = (kpps+999)/1000;

      mmpmMppsParam.mppsTotal                  = mpps;
      mmpmMppsParam.adspFloorClock             = 0;

      rscParam[req_num].rscId                   = MMPM_RSC_ID_MPPS;
      rscParam[req_num].rscParam.pMppsReq       = &mmpmMppsParam;

      req_num++;
      if (both_diff || !is_bw_req) //request separately if either bw and mips are differet types (req/rel), or if there's no BW req.
      {
         reqParam.numOfReq                   = req_num;

         if (mpps == 0)
         {
            result = qurt_elite_adsppm_wrapper_release(pMe->ulAdsppmClientId, &pMe->adsppmClientPtr, &reqParam);
            if(RX_STRTR_ID == pMe->router_id)
            {
               MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM release MPPS by RXSR (%lu). Result %lu",pMe->ulAdsppmClientId, result);
            }
            else
            {
               MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM release MPPS by TXSR (%lu). Result %lu",pMe->ulAdsppmClientId, result);
            }
         }
         else
         {
            result = qurt_elite_adsppm_wrapper_request(pMe->ulAdsppmClientId, &pMe->adsppmClientPtr, &reqParam);
            if(RX_STRTR_ID == pMe->router_id)
            {
               MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM request MPPS %lu by RXSR (%lu). Result %lu", mpps, pMe->ulAdsppmClientId,result);
            }
            else
            {
               MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM request MPPS %lu by TXSR (%lu). Result %lu", mpps, pMe->ulAdsppmClientId,result);
            }
         }
         req_done = true;
         req_num = 0; //reset req_num as req is already made.
      }
   }

   if ( is_bw_req )
   {
      pMe->bw_vote = bw;

      bwReqVal.busRoute.masterPort                 = MMPM_BW_PORT_ID_ADSP_MASTER;
      bwReqVal.busRoute.slavePort                  = MMPM_BW_PORT_ID_DDR_SLAVE;
      bwReqVal.bwValue.busBwValue.bwBytePerSec     = bw;
      bwReqVal.bwValue.busBwValue.usagePercentage  = 100;
      bwReqVal.bwValue.busBwValue.usageType        = MMPM_BW_USAGE_LPASS_DSP;

      bwReq.numOfBw            = 1;
      bwReq.pBandWidthArray    = &bwReqVal;

      rscParam[req_num].rscId                   = MMPM_RSC_ID_GENERIC_BW_EXT;
      rscParam[req_num].rscParam.pGenBwReq      = &bwReq;

      req_num++;
      if (both_diff || !is_mips_req) //request separately if either bw and mips are differet types (req/rel), or if there's no mips req.
      {
         reqParam.numOfReq                   = req_num;

         if (bw==0)
         {
            result = qurt_elite_adsppm_wrapper_release(pMe->ulAdsppmClientId, &pMe->adsppmClientPtr, &reqParam);
            if(RX_STRTR_ID == pMe->router_id)
            {
               MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM release BW by RXSR (%lu). Result %lu", pMe->ulAdsppmClientId, result);
            }
            else
            {
               MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM release BW by TXSR (%lu). Result %lu", pMe->ulAdsppmClientId, result);
            }
         }
         else
         {
            result = qurt_elite_adsppm_wrapper_request(pMe->ulAdsppmClientId, &pMe->adsppmClientPtr, &reqParam);
            if(RX_STRTR_ID == pMe->router_id)
            {
               MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM request BW %lu KBPS by RXSR (%lu). Result %lu", bw/1024, pMe->ulAdsppmClientId, result);
            }
            else
            {
               MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM request BW %lu KBPS by TXSR (%lu). Result %lu",  bw/1024, pMe->ulAdsppmClientId, result);
            }
         }

         req_done = true;
         req_num = 0; //reset req_num as req is already made.
      }
   }

   if (req_num && !req_done)
   {
      reqParam.numOfReq                   = req_num;

      if ( (kpps == 0) && (bw == 0) )
      {
         result = qurt_elite_adsppm_wrapper_release(pMe->ulAdsppmClientId, &pMe->adsppmClientPtr, &reqParam);
         if(RX_STRTR_ID == pMe->router_id)
         {
            MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM release BW and MPPS by RXSR (%lu). Result %lu", pMe->ulAdsppmClientId,result);
         }
         else
         {
            MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM release BW and MPPS by TXSR (%lu). Result %lu", pMe->ulAdsppmClientId,result);
         }
      }
      else
      {
         result = qurt_elite_adsppm_wrapper_request(pMe->ulAdsppmClientId, &pMe->adsppmClientPtr, &reqParam);
         if(RX_STRTR_ID == pMe->router_id)
         {
            MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM request MPPS %lu and BW %lu KBPS by RXSR (%lu). Result %lu", mpps, bw/1024, pMe->ulAdsppmClientId, result);
         }
         else
         {
            MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM request MPPS %lu and BW %lu KBPS by TXSR (%lu). Result %lu", mpps, bw/1024, pMe->ulAdsppmClientId, result);
         }
      }
   }

#endif //#if (ADSPPM_INTEGRATION==1)
   return result;
}

ADSPResult stream_router_svc_register_with_adsppm(stream_router_svc_t* pMe)
{
   ADSPResult result = ADSP_EOK;
#if (ADSPPM_INTEGRATION==1)

   MmpmRegParamType regParam;
   char threadname[16];
   qurt_thread_get_name(threadname, 16-1);

   regParam.rev             = MMPM_REVISION;
   regParam.instanceId      = MMPM_CORE_INSTANCE_0;
   regParam.pwrCtrlFlag     = PWR_CTRL_NONE; //PWR_CTRL_STATIC_DISPLAY, PWR_CTRL_THERMAL
   regParam.callBackFlag    = CALLBACK_NONE; //CALLBACK_STATIC_DISPLAY, CALLBACK_THERMAL, CALLBACK_REQUEST_COMPLETE
   regParam.MMPM_Callback   = NULL;
   regParam.cbFcnStackSize  = 0;

   regParam.coreId          = MMPM_CORE_ID_LPASS_ADSP; //no need to request power, register access.
   regParam.pClientName     = threadname;

   result = qurt_elite_adsppm_wrapper_register(&regParam, &pMe->ulAdsppmClientId, &pMe->adsppmClientPtr);

   if(RX_STRTR_ID == pMe->router_id)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM register by RXSR. Result %lu. Client id %lu", result, pMe->ulAdsppmClientId);
   }
   else
   {
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM register by TXSR. Result %lu. Client id %lu", result, pMe->ulAdsppmClientId);
   }

#endif
   return result;
}

ADSPResult stream_router_svc_deregister_with_adsppm(stream_router_svc_t* pMe)
{
   ADSPResult result = ADSP_EOK;
   if ( !qurt_elite_adsppm_wrapper_is_registered(pMe->ulAdsppmClientId) )
   {
      return ADSP_EOK;
   }
   uint32_t client_id=pMe->ulAdsppmClientId;
   result = qurt_elite_adsppm_wrapper_deregister(&pMe->ulAdsppmClientId, &pMe->adsppmClientPtr);

   if(RX_STRTR_ID == pMe->router_id)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM deregister by RXSR. Result %lu. Client id %lu", result, client_id);
   }
   else
   {
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM deregister by TXSR. Result %lu. Client id %lu", result, client_id);
   }
   return result;
}

/**
 * if respMsg == NULL, then  unInPortID & unOutPortID sent from caller will be used.
 *    if forceRaise, then unOutPortID is immaterial.
 * else, port-id in client token of respMsg will be used.
 */
ADSPResult stream_router_svc_check_raise_rendered_eos_v2(stream_router_svc_t* pMe, elite_msg_any_t *respMsg, uint32_t input_port_id, uint32_t output_port_id, bool_t forceRaise)
{
   elite_msg_data_eos_apr_t *eos_payload_ptr = NULL;
   uint32_t out_port_mask = 0;

   if (respMsg)
   {
      eos_payload_ptr = (elite_msg_data_eos_apr_t *)respMsg->pPayload;
      stream_router_svc_get_in_out_port_id_from_client_token(eos_payload_ptr->unClientToken, &input_port_id, &output_port_id);
   }

   out_port_mask = 1 << output_port_id;

   stream_router_input_port_t *in_port_ptr = &pMe->in_port[input_port_id];

   if (in_port_ptr && (in_port_ptr->eos_info.event_opcode == ASM_DATA_EVENT_RENDERED_EOS_V2))
   {
      //if we are waiting for eos-ack for this out port.
      if (forceRaise || (in_port_ptr->eos_pending_ack_mask & out_port_mask))
      {
         if ( forceRaise || ((in_port_ptr->eos_info.mask & ASM_BIT_MASK_EOS_V2_RAISE_IF_ANY_OR_ALL) == ASM_BIT_EOS_V2_RAISE_IF_ANY ))
         {
            in_port_ptr->eos_pending_ack_mask = 0;
         }
         else
         {
            in_port_ptr->eos_pending_ack_mask &= ~out_port_mask;
         }

         //raise event
         if (in_port_ptr->eos_pending_ack_mask == 0)
         {
            MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"stream router i/p port %lu : raising rendered EoS event. mask 0x%lx", input_port_id, in_port_ptr->eos_info.mask);
            elite_svc_raise_rendered_eos_v2_event( &(in_port_ptr->eos_info) );
            in_port_ptr->eos_info.event_opcode = 0;
         }
      }
   }

   if (respMsg)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"stream router i/p port %lu : eos release", input_port_id);
      elite_msg_release_msg(respMsg);
   }

   return ADSP_EOK;
}

router_operation_mode_t stream_router_get_operation_mode(uint32_t fmt_id)
{
   if(ASM_MEDIA_FMT_DSD == fmt_id)
   {
      return DSD_DOP;
   }
   else if(ASM_MEDIA_FMT_GENERIC_COMPRESSED == fmt_id)
   {
      return GENERIC_COMPRESSED_MODE;
   }
   else
   {
      return IEC_61937;
   }
}

void stream_router_svc_calculate_delay(stream_router_svc_t *pMe, uint32_t port_id)
{
  if(TX_STRTR_ID == pMe->router_id)
  {
    //update the Tx stream router delay, the delay is equal to the max buffer size, which is one frame size here.
    stream_router_output_port_t *out_port_ptr = &pMe->out_port[port_id];    

    if( (NULL != out_port_ptr->str_rtr_delay_ptr) && 
               (0 != out_port_ptr->sampling_rate) &&
               (0 != out_port_ptr->num_channels) &&
               (0 != out_port_ptr->bits_per_sample) )
    {
      uint64_t out_buf_duration = (uint64_t)out_port_ptr->output_buffer_size * NUM_US_IN_SECOND/(out_port_ptr->sampling_rate * out_port_ptr->num_channels * (out_port_ptr->bits_per_sample / BITS_PER_BYTE));
      *out_port_ptr->str_rtr_delay_ptr = (uint32_t)out_buf_duration;
      MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "stream_router#%lu, o/p port %lu,Delay updated:(micro-sec) %lu", pMe->router_id, port_id, *(out_port_ptr->str_rtr_delay_ptr));      
    }
  }
}
