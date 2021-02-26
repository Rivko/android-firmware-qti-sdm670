
/*========================================================================

This file contains structure definitions for Switch Service.

Copyright (c) 2011-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
 *//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/stream_router_svc/core/lib/src/StreamRouterSvcStructs.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
11/04/10   RP      Created file.
11/09/15   YW      Updated to adopt DSD formats
05/06/16   YW      Updated to adopt compressed raw passthru
========================================================================== */
#ifndef _STREAMROUTERSVC_STRUCT_H_
#define _STREAMROUTERSVC_STRUCT_H_

#include "avtimer_drv_api.h"
#include "AudDevMgr_AprIf.h"
#include "IEC61937.h"
#include "dsdUtil.h"
#include "AudioStreamMgr.h"
#include "AFEInterface.h"

#include "qurt_elite_adsppm_wrapper.h"
#include "adsp_asm_data_commands.h"


#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */

static const uint32_t  STREAM_ROUTER_CMD_SIG       = 0x80000000;
static const uint32_t  STREAM_ROUTER_PORT_START_MASK = 0x20000000;

//signals are assigned as follows
//------------------------------------------------------------------------
//bit31     bit30        bit29         bit28          bit27        bit26
//------------------------------------------------------------------------
//command | reserved | port0 input | port0 output| port1 input| port1 output
//-----------------------------------------------------------------------

/* -----------------------------------------------------------------------
 ** Constant / Define Declarations
 ** ----------------------------------------------------------------------- */
#define STREAM_ROUTER_THREAD_STACK_SIZE 4096
#define POLLING_NEEDED     TRUE
#define NO_POLLING_NEEDED  FALSE

// maximum data messages in input data queue. Must consider data buffers + EOS messages.
static const uint32_t MAX_INP_DATA_Q_ELEMENTS = 16;

// maximum data messages in output queue.
static const uint32_t MAX_OUT_DATA_Q_ELEMENTS = 2;

// maximum number of commands expected ever in command queue.
static const uint32_t MAX_CMD_Q_ELEMENTS = 8;

static const uint16_t NUM_NULL_SYNC_FRAMES_PER_CHANNEL = 1;
//
////output buffer parameters
static const uint32_t TX_ROUTER_OUT_BUF_SIZE = 1920;  //TODO: Make it based on input frame size

/*should be at least as large as 1 AFE DMA buffer size (worst case 10ms buffer for 44.1kHz)*/
static const uint64_t PAUSE_BURST_DURATION_IN_US = 10000;
static const uint32_t PAUSE_BURST_DURATION_IN_MS = PAUSE_BURST_DURATION_IN_US / 1000;

//timestamp related parameters
static const uint32_t NUM_US_IN_SECOND = 1000000;
static const int32_t  TIME_DRIFT_TOLERANCE = 2000;//in microseconds
static const uint32_t NUM_CHANNELS = 2;
static const uint32_t BYTES_PER_SAMPLE = 2;
static const uint32_t BITS_PER_BYTE = 8;

//Tapping point Id for Rx and Tx Stream routers
static const uint32_t AUDIOLOG_STRTR_IN_TAP_ID =  0x0001;

static const uint32_t default_kpps = 2000;
static const uint32_t default_bw = 192000; //Bps

// stream router service states
typedef enum
{
	DISCONNECTED = 0,
	CONNECTED,
	WAITING,
	RUN,
	PAUSE
} router_state_t;

typedef enum
{
	IEC_61937 =0,
	DSD_DOP,
	GENERIC_COMPRESSED_MODE
} router_operation_mode_t;
//timestamp handling state for input port
struct inport_timestamp_state_t
{
	uint64_t initial_session_clock;
	uint64_t duration_rendered;
	uint64_t input_buffer_ts;
	bool_t   is_asynchronous_mode;
	bool_t   sync_to_next_ts;
	bool_t   skip_update;
	bool_t   is_resync;
};

//timestamp handling state for output port
struct outport_timestamp_state_t
{
	uint64_t output_buffer_ts; //for Rx st rtr, this stands for last processed TS. we sync this to input TS after rendering/dropping (not before hold).
	bool_t   ts_valid;
};

//intermediate output buffer, to temporarily store data till output buffer arrives
struct intermediate_buffer_t
{
	uint8_t  *address_ptr;
	uint32_t max_size;   //size of the buffer
	uint32_t actual_size;//amount of valid data in the buffer
	uint32_t input_buf_read_pos; //read position of the corresponding input buffer (this much has been copied)
	bool_t   is_updated; //whether the intermediate buffer has been updated from a new input buffer
};

/* -----------------------------------------------------------------------
 ** Global definitions/forward declarations
 ** ----------------------------------------------------------------------- */

struct stream_router_input_port_t
{
	elite_msg_any_t                      data_msg;
	elite_svc_handle_t                   service_handle;     //handle to give out to others
	router_state_t                       stream_state;       //valid only for rx router
	router_operation_mode_t              operation_mode;     //mode will stream router be operating with
	uint64_t                             data_burst_duration;//duration of the data burst in us.
	inport_timestamp_state_t             ts_state;
	bool_t                               is_connected;
	uint32_t                             format_id;                //format ID of the compressed stream. can be zero initially for compr passthru
	uint32_t                             sampling_rate;
	uint16_t                             num_channels;
	bool_t                               is_first_buffer_received;
	uint16_t                             bits_per_sample;                      //bits per sample of the incoming bit stream, passthru only.
	uint32_t                             connected_output_ports;   //bitfield of output ports this port is connected to
	uint32_t                             connected_active_output_ports; //For Tx router connected output ports in RUN state
	uint32_t                             data_log_id;
	AdmEventCbData_t                     media_fmt_cb;
	uint32_t                             primary_output_port;
	uint32_t                             bytes_to_next_compr_frame;
	Elite_CbType                         asm_cb_handle;

	//AvSync Params
	void *avsync_params;     //All avsync-related parameters stored here. For stub, this would be Null.
	uint64_t session_time;     //Stub version will hold a local copy of session time
	uint64_t absolute_time;   //Stub version will hold a local copy of absolute time
	avtimer_drv_handle_t avt_drv_handle;  //Handle to avtimer (If AvSync is stubbed out, this is still used; If AvSync is not stubbed out, AvSyncLib will store handle)
	uint64_t                             input_buffer_remainder_unit;         // the remainder size of the input buffer for each input buffer;
	uint64_t                             input_buffer_remainder_total;         // the total remainder size of the input buffer so far, if has not been added up yet

	uint32_t                             kpps_vote;     //total KPPS
	uint32_t                             bw_vote;       // bw vote

	uint32_t                             eos_pending_ack_mask;             //Mask that indicates which output ports are waiting for ack from AFE for EoS.
	elite_msg_data_eos_info_t            eos_info;
};

struct stream_router_output_port_t
{
	elite_msg_any_t                      data_msg;
	elite_msg_any_t                      eos_msg;                 //used to cache the EOS message till all data has been sent down
	router_operation_mode_t              operation_mode;
	uint32_t                             num_bufs_allocated;
	uint16_t                             max_num_bufs;
	router_state_t                       stream_state;             //valid only for tx router
	uint32_t                             output_buffer_size;       //the size that the output buffers must have (Rx rtr: if not they will be freed)
	uint32_t                             pause_buffer_duration;      //duration of the current output buffer.
	uint64_t                             downstream_buffer_duration; //total duration of buffers downstream to stream router
	elite_svc_handle_t                   service_handle;           //handle to give out to others
	elite_svc_handle_t                   *downstream_svc_handle;   // down stream peer service handle
	outport_timestamp_state_t            ts_state;
	bool_t                               is_connected;
	uint32_t                             sampling_rate;
	uint16_t                             num_channels;
	uint16_t                             bits_per_sample;                      //bit width of hte outgoing bit stream, passthru only.
	uint32_t                             connected_input_port; //input port driving this output port
	intermediate_buffer_t                intermediate_buff;
	volatile uint32_t                    *afe_delay_ptr;    //Address to the downstream AFE delay (AV-Sync).
	volatile uint32_t                    *copp_buf_delay_ptr;    //Address to the downstream compressed COPP buffering delay (AV-Sync).
	volatile uint32_t                    *copp_alg_delay_ptr;    //Address to the downstream compressed COPP algorithmic delay (AV-Sync).
	volatile uint32_t                    *str_rtr_delay_ptr; //Address to the stream router delay (AV-Sync).
	volatile const afe_drift_info_t      *afe_drift_ptr; //AFE drift pointer. Applicable to TX matrix only.
	uint32_t                             kpps_vote;     //total KPPS
	uint32_t                             bw_vote;       // bw vote

};


struct stream_router_svc_t
{
	elite_svc_handle_t                   service_handle;           //handle to give out to others
	uint32_t                             router_id;                //indicates whether Tx or Rx Router
	qurt_elite_channel_t                 channel;                  //hold MQ's owned by this instance
	uint32_t                             current_bitfield;
	uint32_t                             channel_status;
	elite_msg_any_t                      cmd_msg;
	stream_router_input_port_t           in_port[STRTR_MAX_PORTS];
	stream_router_output_port_t          out_port[STRTR_MAX_PORTS];

	uint32_t                             ulAdsppmClientId;
	qurt_elite_adsppm_client_t           *adsppmClientPtr;

	uint32_t                             kpps_vote;     //total KPPS
	uint32_t                             bw_vote;       // bw vote

};

/* -----------------------------------------------------------------------
 ** Function prototypes
 ** ----------------------------------------------------------------------- */

ADSPResult stream_router_svc_create_output_buffers(stream_router_svc_t *pMe, uint32_t port_id, uint32_t connected_input_port, uint32_t num_out_buf_to_create);
ADSPResult stream_router_svc_check_recreate_out_buf(stream_router_svc_t *pMe, uint32_t output_port_id, elite_msg_data_buffer_t **out_buf_ptr_ptr);
void stream_router_svc_destroy_out_bufs(stream_router_svc_t* pMe, uint32_t port_id, bool_t need_poll);
void stream_router_svc_copy_from_input_to_intermediate_buffer(stream_router_input_port_t *in_port_ptr, stream_router_output_port_t *out_port_ptr);
ADSPResult stream_router_svc_push_output_buffer_downstream(stream_router_input_port_t *in_port_ptr, stream_router_output_port_t *out_port_ptr);
ADSPResult stream_router_svc_init_in_port(stream_router_svc_t *pMe, EliteMsg_CustomStreamRouterConnect* p_cmd_msg_payload, uint32_t port_id);
ADSPResult stream_router_svc_init_in_buffer_queue(stream_router_svc_t *pMe, uint32_t port_id);
ADSPResult stream_router_svc_init_out_port(stream_router_svc_t *pMe, EliteMsg_CustomStreamRouterConnect* p_cmd_msg_payload, uint32_t port_id);
ADSPResult stream_router_svc_init_out_buffer_queue(stream_router_svc_t *pMe, uint32_t port_id, uint32_t connected_input_port, uint32_t max_buffering_ms);
ADSPResult stream_router_svc_deinit_in_port(stream_router_svc_t *pMe, uint32_t port_id);
void stream_router_svc_destroy_in_buffer_queue(stream_router_svc_t *pMe, uint32_t port_id);
void stream_router_svc_deinit_out_port(stream_router_svc_t *pMe, uint32_t port_id);
void stream_router_svc_destroy_out_buffer_queue(stream_router_svc_t *pMe, uint32_t port_id, bool_t need_poll);
void stream_router_svc_try_destroy_out_buffers(stream_router_svc_t *pMe, uint32_t port_id);
void stream_router_svc_init_buf_params(elite_msg_data_buffer_t *buf_ptr,stream_router_svc_t *pMe, uint32_t port_id);
uint32_t stream_router_svc_get_pause_duration(stream_router_input_port_t *in_port_ptr, stream_router_output_port_t *out_port_ptr);
void stream_router_svc_fill_pause_burst(stream_router_svc_t* pMe, elite_msg_data_buffer_t *buf_msg_ptr, uint32_t num_pause_samples, uint32_t in_port_id,  uint32_t output_port_id);
ADSPResult stream_router_svc_connect_input_to_output(stream_router_svc_t *pMe, uint32_t input_port_id, uint32_t output_port_id);
ADSPResult stream_router_svc_disconnect_input_from_output(stream_router_svc_t *pMe, uint32_t input_port_id, uint32_t output_port_id);
void stream_router_svc_rx_mapping(stream_router_svc_t *pMe, uint32_t input_port_id, uint32_t output_port_id);
ADSPResult stream_router_svc_create_and_send_media_type(stream_router_svc_t* pMe, uint32_t output_port_id, uint32_t sampling_rate, uint16_t num_channels, uint16_t bits_per_sample,  uint32_t format_id);
bool_t is_top_prio_output_port(stream_router_svc_t *pMe, uint32_t port_id);
ADSPResult stream_router_svc_push_eos_downstream(stream_router_svc_t* pMe, uint32_t output_port_id);
ADSPResult stream_router_svc_get_output_buffer_size(stream_router_svc_t *pMe,uint32_t input_port_id, uint32_t output_port_id, uint32_t *buffer_size);
ADSPResult stream_router_svc_process_media_type(stream_router_svc_t *pMe, uint32_t input_port_id, elite_msg_data_media_type_apr_t *media_type_ptr);
ADSPResult stream_router_svc_send_media_type_to_connected_output_ports(stream_router_svc_t *pMe, uint32_t input_port_id, uint32_t specific_out_port_id);
ADSPResult stream_router_svc_process_eos_msg(stream_router_svc_t *pMe, elite_msg_data_eos_apr_t *eos_msg_ptr, uint32_t output_port_bitfield, uint32_t input_port_id);
ADSPResult stream_router_svc_log_data(stream_router_svc_t *pMe, int8_t *buf_addr, uint32_t buf_size, stream_router_input_port_t *in_port_ptr);
uint32_t stream_router_svc_sniff_media_format(stream_router_svc_t *pMe, uint32_t port_id, uint8_t *data_buf, uint32_t data_length);
ADSPResult stream_router_svc_process_mark_buffer(stream_router_svc_t *pMe, uint32_t input_port_id, uint16_t status);
ADSPResult stream_router_svc_init_avsync_stats(stream_router_svc_t *pMe, uint32_t port_id);
ADSPResult stream_router_svc_custom_set_primary_port(stream_router_svc_t *pMe);

//AV-Sync
ADSPResult stream_router_svc_set_avsync_params(stream_router_svc_t* pMe, uint32_t param_id, void *param_val_ptr, uint16_t param_size, uint32_t port_id);
ADSPResult stream_router_svc_get_avsync_params(stream_router_svc_t* pMe);
ADSPResult stream_router_svc_custom_set_param(stream_router_svc_t* pMe);
ADSPResult stream_router_svc_custom_get_param(stream_router_svc_t* pMe);
ADSPResult stream_router_svc_query_avsync_stats(stream_router_svc_t* pMe, asm_session_param_data_v2_t *param_data_ptr, uint32_t payload_size, uint32_t struct_size, uint32_t port_id);
ADSPResult stream_router_init_input_avsync(stream_router_svc_t *pMe, uint32_t port_id);
ADSPResult stream_router_deinit_input_avsync(stream_router_svc_t *pMe, uint32_t port_id);
ADSPResult stream_router_svc_init_s2d_drift(stream_router_svc_t *pMe, uint32_t port_id);
ADSPResult stream_router_svc_update_session_clock(stream_router_svc_t *pMe, uint32_t input_port_id, bool_t is_pausing);
ADSPResult stream_router_svc_reset_session_clock(stream_router_svc_t *pMe, uint32_t port_id);
ADSPResult stream_router_svc_update_session_clock_2(stream_router_svc_t *pMe, uint32_t port_id, uint64_t time_stamp);
ADSPResult stream_router_svc_set_in_out_drift_ptr(stream_router_svc_t *pMe, uint32_t in_port_id, uint32_t out_port_id);
ADSPResult stream_router_svc_process_output_data_q_rx_mode(stream_router_svc_t* pMe, uint32_t port_id);
ADSPResult stream_router_svc_process_output_data_q_wait_state(stream_router_svc_t *pMe, uint32_t port_id, qurt_elite_bufmgr_node_t *output_buffer_node, uint32_t old_buf_size);
ADSPResult stream_router_svc_update_s2d_drift(stream_router_svc_t *pMe, uint32_t port_id, int32_t input_data_length, uint64_t time_stamp);
ADSPResult stream_router_svc_update_expected_session_clk(stream_router_svc_t *pMe, uint32_t port_id, uint64_t proposed_val);
router_operation_mode_t stream_router_get_operation_mode(uint32_t fmt_id);
void stream_router_svc_calculate_delay(stream_router_svc_t *pMe, uint32_t port_id);
/*======================================================================

FUNCTION      stream_router_svc_get_connected_active_out_ports

DESCRIPTION   returns the mask containing active connected output ports.

DEPENDENCIES  none

RETURN VALUE  duration in usec

SIDE EFFECTS  none

======================================================================*/
static inline uint32_t stream_router_svc_get_connected_active_out_ports(stream_router_svc_t *pMe, stream_router_input_port_t *in_port_ptr)
{
	if (pMe->router_id == RX_STRTR_ID)
	{
		return in_port_ptr->connected_output_ports;
	}
	else
	{
		return in_port_ptr->connected_active_output_ports;
	}
};
/*======================================================================

FUNCTION      stream_router_svc_buffer_size_to_duration

DESCRIPTION   return the duration corresponding to a given buffer size.
Automatically return the correct buffer size with different operation modes.

DEPENDENCIES  none

RETURN VALUE  duration in usec

SIDE EFFECTS  none

======================================================================*/
uint64_t stream_router_svc_buffer_size_to_duration(stream_router_input_port_t *in_port_ptr, uint32_t frames, uint32_t sampling_rate, uint16_t num_channels, uint16_t bits_per_sample);


/*======================================================================

FUNCTION      stream_router_svc_duration_to_buffer_size

DESCRIPTION   return the corresponding buffer size corresponding to a given duration.
Automatically return the correct buffer size with different operation modes.

DEPENDENCIES  input duration  should be in usec

RETURN VALUE  calculated buffer size

SIDE EFFECTS  none

======================================================================*/
uint64_t stream_router_svc_duration_to_buffer_size(stream_router_input_port_t *in_port_ptr, uint64_t duration, uint32_t sampling_rate, uint16_t num_channels, uint16_t bits_per_sample);

/*======================================================================

 FUNCTION      stream_router_svc_remainder_to_duration

 DESCRIPTION   return the duration corresponding to a given remainder size.
 Automatically return the correct buffer size with different operation modes.
 This is a seperate function in that if we use buffer size to duration, we are
 losing precision.

 DEPENDENCIES  remainder has to be un-changed.

 RETURN VALUE  duration in usec

 SIDE EFFECTS  none

 ======================================================================*/
uint64_t stream_router_svc_remainder_to_duration(stream_router_input_port_t *in_port_ptr, uint32_t remainder_size, uint32_t sampling_rate, uint16_t num_channels, uint16_t bits_per_sample);

/*======================================================================

FUNCTION      stream_router_svc_60958_frames_to_duration

DESCRIPTION   return the duration corresponding to a given number of
IEC 60958 frames. Also takes into account the case where
number of channels != 2 (HBR Audio)

DEPENDENCIES  none

RETURN VALUE  duration in usec

SIDE EFFECTS  none

======================================================================*/
static inline uint64_t stream_router_svc_60958_frames_to_duration(uint32_t frames, uint32_t sampling_rate, uint16_t num_channels)
{
	return ((uint64_t)frames * NUM_US_IN_SECOND * IEC61937_NUM_SAMPLES_PER_IEC60958_FRAME)/
			(sampling_rate * num_channels);
};

/*======================================================================

FUNCTION      stream_router_svc_duration_to_60958_frames

DESCRIPTION   return the IEC 60958 frames corresponding to the given duration
in microseconds. Also takes into account the case where
number of channels != 2 (HBR Audio)

DEPENDENCIES  none

RETURN VALUE  number of IEC 60958 frames

SIDE EFFECTS  none

======================================================================*/
static inline uint32_t stream_router_svc_duration_to_60958_frames(uint64_t duration, uint32_t sampling_rate, uint16_t num_channels)
{
	return (duration * sampling_rate * num_channels)/
			(NUM_US_IN_SECOND * IEC61937_NUM_SAMPLES_PER_IEC60958_FRAME);
}
/*======================================================================

   FUNCTION      stream_router_svc_duration_to_remainder_size

   DESCRIPTION   return the corresponding remainder size corresponding to a given duration.
   Automatically return the correct remainder size with different operation modes.
   This is a seperate function in that if we use duration to buffer size, we have a high
   risk of integer overflow.

   DEPENDENCIES  input duration  should be in usec

   RETURN VALUE  calculated remainder size

   SIDE EFFECTS  none

   ======================================================================*/
uint64_t stream_router_svc_duration_to_remainder_size(stream_router_input_port_t *in_port_ptr, uint64_t duration, uint32_t sampling_rate, uint16_t num_channels, uint16_t bits_per_sample);

/*======================================================================

FUNCTION      stream_router_svc_input_port_to_signal_mask

DESCRIPTION   returns signal mask for given input port id

DEPENDENCIES  none

RETURN VALUE  number of IEC 60958 frames

SIDE EFFECTS  none

======================================================================*/
static inline uint32_t stream_router_svc_input_port_to_signal_mask(uint32_t port_id)
{
	return (STREAM_ROUTER_PORT_START_MASK >> (port_id * 2));
}

/*======================================================================

FUNCTION      stream_router_svc_output_port_to_signal_mask

DESCRIPTION   returns signal mask for given output port id

DEPENDENCIES  none

RETURN VALUE  signal mask

SIDE EFFECTS  none

======================================================================*/
static inline uint32_t stream_router_svc_output_port_to_signal_mask(uint32_t port_id)
{
	return (STREAM_ROUTER_PORT_START_MASK >> (port_id * 2 + 1));
}


/*======================================================================

FUNCTION      stream_router_svc_sigbit_to_port_id

DESCRIPTION   convert signal bit to port id

DEPENDENCIES  none

RETURN VALUE  port id

SIDE EFFECTS  none

======================================================================*/
static inline uint32_t stream_router_svc_sigbit_to_port_id(uint32_t sigbit)
{
	return (sigbit/2 - 1);
}


/*======================================================================

FUNCTION      stream_router_svc_release_buffer

DESCRIPTION   releases the buffer to the return Q and sets the payload
              ptr to NULL

DEPENDENCIES  none

RETURN VALUE  none

SIDE EFFECTS  none

======================================================================*/
static inline ADSPResult stream_router_svc_release_buffer(elite_msg_any_t *data_msg_ptr)
{
	ADSPResult result;
	result = elite_msg_release_msg(data_msg_ptr);
	data_msg_ptr->pPayload = NULL;
	return result;
}

ADSPResult stream_router_svc_register_with_adsppm(stream_router_svc_t* pMe);
ADSPResult stream_router_svc_deregister_with_adsppm(stream_router_svc_t* pMe);
ADSPResult stream_router_svc_process_kpps_bw(stream_router_svc_t *pMe, bool_t is_release, bool_t force_vote);

static inline uint32_t stream_router_svc_get_client_token_from_in_out_port_id(uint32_t unInPortID, uint32_t unOutPortID)
{
	return ((unInPortID & 0x0000FFFF)<<16) | (unOutPortID & 0x0000FFFF);
}

static inline void stream_router_svc_get_in_out_port_id_from_client_token(uint32_t clientToken, uint32_t *unInPortID, uint32_t *unOutPortID)
{
	*unInPortID = (clientToken & 0xFFFF0000) >>16;
	*unOutPortID = (clientToken & 0x0000FFFF);
}

ADSPResult stream_router_svc_check_raise_rendered_eos_v2(stream_router_svc_t* pMe, elite_msg_any_t *respMsg, uint32_t input_port_id, uint32_t output_port_id, bool_t forceRaise);
#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef _STREAMROUTERSVC_STRUCT_H_

