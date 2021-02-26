/*========================================================================

*//** @file VMtMx.h

Copyright (c) 2010-2015, 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

This file provides interfaces to create and control Matrix mixer for voice graph. An instance of this service is created statically in tx and rx paths.
The matrix mixer allows routing of arbitrary inputs to arbitrary outputs.


*//*====================================================================== */

/*========================================================================
  Edit History

  $Header: //components/rel/avs.adsp/2.8.5/voice/services/voice_mixer/src/VMtMx.h#1 $

  when       who     what, where, why
  --------   ---     -------------------------------------------------------
  05/01/10   ksa     Created file

  ========================================================================== */

#define LIMITER_PARAM_SIZE 8   // Limter configuration parameter size

#ifdef __cplusplus
extern "C" {

#include "voice_resampler.h"

/** Indicates a port is currently unused */
#define PORT_INACTIVE            0
/** Indicates a port is currently used and consuming/delivering data */
#define PORT_ACTIVE              1
/** Indicates that an input port is ready, but not consuming any data till
    a media type is sent
 */
#define PORT_READY               2
/** Indicates that a port is currently stopped and therefore not
    consuming/delivering data
 */
#define PORT_STOPPED             3

/** Indicates that an input buffer at the input port has been released since
    all samples have been consumed
 */
#define INPUT_BUFFER_RELEASED 0
/** Indicates that an input buffer at the input port has not been released
    since there are unused samples available
 */
#define INPUT_BUFFER_HELD 1

/** Indicates rx path instance of voice matrix mixer
*/
#define VMX_RX_INSTANCE 1

/** Indicates tx path instance of voice matrix mixer
*/
#define VMX_TX_INSTANCE 2

/** Supported samping rates, to keep a count
 */
enum
{
VMX_SAMPLE_RATE_8K = 0,
VMX_SAMPLE_RATE_16K,
VMX_SAMPLE_RATE_32K,
VMX_SAMPLE_RATE_48K,
VMX_NUM_SAMPLE_RATES_SUPPORTED
};

typedef struct voice_mixer_resampler_t
{
   voice_resampler_config_struct_t  resamp_config;
   uint32_t         channel_mem_size;
   void* channel_mem_ptr;
} voice_mixer_resampler_t;

typedef struct vmx_in_port_info_t vmx_in_port_info_t;
typedef struct vmx_out_port_info_t vmx_out_port_info_t;

struct vmx_in_port_info_t
{
   vmx_inport_handle_t         in_port_handle;           // handle to the input port
                                                      // ------------------------------------
   char_t                       data_q_name[QURT_ELITE_DEFAULT_NAME_LEN];    // data queue name
                                                      // ------------------------------------
   elite_msg_any_t             data_q_msg;
   elite_msg_data_buffer_t*     input_buf_ptr;              // Input PCM buffer TO DO: Remove if not req
                                                      // ------------------------------------
   uint32_t                     in_port_state;            // holds the current state of the i/p
                                                      // port - active, inactive, pause, etc.
                                                      // ------------------------------------
   uint32_t                     input_mapping_mask;                // mask that shows the outputs that the
                                                      // current input feeds
                                                      // ------------------------------------
   uint32_t                     output_req_pending_mask;   // mask that shows the outputs currently
                                                      // waiting for samples from this input.
                                                      // ------------------------------------
   uint32_t                     in_port_buf_size;   // input port local buffer size (per channel)
                                                      // ------------------------------------
   int16_t*                     current_sample_ptr;         // points to the current sample being
                                                      // used, in the upstream peer svc's
                                                      // output buffer
                                                      // ------------------------------------
   int32_t                        num_remaining_samples;   // number of samples remaining in the
                                                      // upstream peer svc's output buffer
                                                      // ------------------------------------
   uint32_t                     sample_rate;           // Sample Rate
                                                      // ------------------------------------
   bool_t                    in_buf_status;           // flag to indicate of a request
                                                      // for new input buffer has been sent
                                                      // ------------------------------------
   int16_t*                    loc_buffer_ptr;             // Local buffer for input port

   uint32_t                    num_buf_rcvd;              // counter keeping track of # of i/p bufs
                                                      // rcvd by this i/p port so far.
                                                      // ------------------------------------
   voice_mixer_resampler_t     resampler[VMX_NUM_SAMPLE_RATES_SUPPORTED-1];

   int16_t*                    resamp_buffer_ptr;

   bool_t                  acc_pending;              // means current samples in input buffer have not been accumulated yet

   uint32_t                    priority;

   uint32_t                    in_buf_token;         // use this token to pass input buffer related info to outport

   uint64_t                    ull_timestamp;        // time stamp from input buf for propagating downstream
};

struct vmx_out_port_info_t
{
   vmx_outport_handle_t       outPortHandle;           // handle to the output port

   qurt_elite_queue_t*           bufQ;                   // handle to the buffer Q of the port
                                                      // ------------------------------------
   char_t                       buf_q_name[QURT_ELITE_DEFAULT_NAME_LEN];     // buffer Q name
                                                      // ------------------------------------
   uint32_t                     num_output_bufs;        // number of o/p buffers to be allocated
                                                      // for this output port.
                                                      // ------------------------------------
   uint32_t                     max_buf_q_capacity;      // max number of buffers the buf Q can
                                                      // hold
                                                      // ------------------------------------
   uint32_t                     out_bufs_allocated;         // keep track of number of output bufs
                                                      // in circulation
                                                      // ------------------------------------
   elite_svc_handle_t*         downstream_peer_handle_ptr;  // ptr to downstream peer's handle
                                                      // ------------------------------------
   elite_msg_data_buffer_t*     output_buf_ptr;             // ptr to port's output buffer
                                                      // ------------------------------------
   uint32_t                     sample_rate;           // sampling Frequency
                                                      // ------------------------------------
   uint32_t                      buf_size;              // output port's buffer size
                                                       //in terms of number of samples
                                                      // ------------------------------------
   uint32_t                      out_port_state;           // holds the current state of the o/p
                                                      // port - active, inactive, pause, etc.
                                                      // ------------------------------------
   uint32_t                     output_mapping_mask;            // mask that indicates which input ports
                                                      // feed to this output port.
                                                      // ------------------------------------
   int32_t                        output_mode;            // mode in which output port is operating
                                                      // push mode or pull mode
                                                      // ------------------------------------
   int32_t                        num_remaining_samples;   // number of remaining samples
                                                      // TO DO: Remove if not required.
                                                      // ------------------------------------
   uint32_t                      num_buf_rcvd;             // number of o/p buffeers received
                                                      // ------------------------------------
   uint32_t                       num_buf_sent;             // number of o/p buffers sent downstream
                                                      // ------------------------------------
   int32_t*                     acc_buf_ptr;                // pointer to output port's accumulator
                                                      // buffer
                                                      // ------------------------------------
   void*                     limiter_ptr;

   uint32_t data_available_mask;

   uint32_t                   out_buf_token;           // This is the token for last input buffer accumulated.

   uint64_t                   ull_timestamp;           // Timestamp to propagate to output buffer.
};

//maximum input ports in a matrix mixer.
static const uint32_t MAX_INPUT_PORTS = 13;

//maximum output ports in a matrix mixer.
static const uint32_t MAX_OUTPUT_PORTS = 13;

// maximum data messages in data queue. Must consider data buffers + EOS messages.
static const uint32_t MAX_DATA_Q_ELEMENTS = 8;

// maximum number of commands expected ever in command queue.
static const uint32_t MAX_CMD_Q_ELEMENTS = 8;

// thread settings.
static const uint32_t VMX_THREAD_STACK_SIZE = 4096;

// Bit positions for various queues
static const uint32_t CMDQ_BITPOS = 31;
static const uint32_t OUTPORT_BITSTART = 0;
static const uint32_t INPORT_BITSTART = 13;

// Use default 20 ms frame size @ 16 kHz while allocating local buffers, will save a lot of headaches during handovers etc
static const uint32_t DEFAULT_SAMPLING_RATE = 48000;
static const uint32_t DEFAULT_FRAME_SIZE = 20;
static const uint32_t DEFAULT_BYTES_PER_SAMPLE = 2;
static const uint32_t DEFAULT_BUF_SIZE = DEFAULT_FRAME_SIZE * (DEFAULT_SAMPLING_RATE/1000) * DEFAULT_BYTES_PER_SAMPLE;



// This contains all the required data for a service instance. Like the data members of a class...
typedef struct
{
   uint32_t                  instance_id;                    // Mt Mx instance identifier
   elite_svc_handle_t       service_handle;             // handle to give out to other services
   qurt_elite_channel_t       channel;                   // mask for MQ's owned by this obj
   vmx_in_port_info_t*   in_port_params_ptr;             // ptr to an array of structures that hold info
                                                      // of all input ports.
   vmx_out_port_info_t*  out_port_params_ptr;            // ptr to an array of structures that hold info
                                                      // of all output ports.
   uint32_t                  out_ports_frame_size;       // common frame size across all output ports.
   uint32_t                  max_in_ports;              // maximum possible input ports
   uint32_t                  max_out_ports;             // maximum possible output ports
   uint32_t                  max_in_portID;               // max input port ID
   uint32_t                  max_out_portID;              // max output port ID
   uint32_t                  data_bit_field;            // pChannel sub-mask containing data Qs.
   uint32_t                  current_bit_field;         // matrix's overall pChannel wait mask.
   uint32_t                  buf_bit_field;             // pChannel sub-mask containing buf Qs.
   uint32_t                  active_in_ports_mask;
   uint32_t                  active_out_ports_mask;
   char_t                    cmdQName[QURT_ELITE_DEFAULT_NAME_LEN];// command queue name
   uint32_t                  out_buffer_available_mask;
   uint32_t                  samples_available_mask;
   uint32_t                  high_priority_inputs_mask;
   uint32_t                  medium_priority_inputs_mask;
   uint32_t                  low_priority_inputs_mask;
   int32_t                   limiter_params[LIMITER_PARAM_SIZE];
} vmx_t;

}
#endif
