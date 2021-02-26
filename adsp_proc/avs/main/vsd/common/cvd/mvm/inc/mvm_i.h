#ifndef __MVM_I_H__
#define __MVM_I_H__

/*
   Copyright (c) 2013-2014,2017-2018 Qualcomm Technologies, Inc.
   All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Header: //components/rel/avs.adsp/2.8.5/vsd/common/cvd/mvm/inc/mvm_i.h#2 $
  $Author: pwbldsvc $
*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

/****************************************************************************
 * DEFINES                                                                  *
 ****************************************************************************/

#define MVM_HEAP_SIZE_V ( 10240 )

#define MVM_NUM_COMMANDS_V ( 30 )

#define MVM_HANDLE_TOTAL_BITS_V ( 16 )
#define MVM_HANDLE_INDEX_BITS_V ( 5 ) /* 5 bits = 32 handles. */

#define MVM_MAX_OBJECTS_V ( 1 << MVM_HANDLE_INDEX_BITS_V )

#define MVM_SSR_MAX_TRACKED_MEM_HANDLES ( 32 )

#define MVM_PANIC_ON_ERROR( rc ) \
  { if ( rc ) { ERR_FATAL( "MVM: Error[0x%08x]", rc, 0, 0 ); } }

#define MVM_REPORT_FATAL_ON_ERROR( rc ) \
  { if ( rc ) { MSG_1( MSG_SSID_DFLT, MSG_LEGACY_FATAL,\
                       "MVM: Error[0x%08x]", rc ); } }

/* This macro is to be used to inspect the APR return code whenever MVM
 * calls APR to send a packet.
 *
 * When MVM encounters an APR communication error for sending a packet to an
 * APR service who resides in a different domain than MVM, an error message
 * will be printed and the error is ignored. This is the phase one SSR
 * implementation for handling APR communication errors due to that MVM's
 * client's subsystem encounters SSR. In the future, MVM may need to use a
 * timer based retry mechanism to keep on trying to communicate with the remote
 * client until MVM knows that the remote client is no longer available.
 *
 * When MVM encounters an APR communication error for sending a packet to an
 * APR service who resides in the same domain as MVM, MVM_PANIC_ON_ERROR will
 * be called.
 */
#define MVM_COMM_ERROR( rc, dst_addr ) \
   { if ( ( rc ) && \
          ( APR_GET_FIELD( APRV2_PKT_DOMAIN_ID, dst_addr ) != \
            APR_GET_FIELD( APRV2_PKT_DOMAIN_ID, mvm_my_addr ) ) ) \
     { MSG_2( MSG_SSID_DFLT, MSG_LEGACY_FATAL,"MVM APR comm error 0x%08X, dst_addr = 0x%04X", rc, dst_addr ); } \
     else \
     { MVM_PANIC_ON_ERROR( rc ); } \
   }


#define MVM_MAX_GENERIC_OBJECTS_PER_SESSION_V ( 32 )

#define MVM_MAX_SESSION_NAME_SIZE ( 31 )

#define MVM_MAX_SESSIONS ( 5 )
  /**< Maximum number of full control MVM sessions that can be created. */

#define MVM_NB_SR ( 8000 )
  /**< Narrowband stream/vocproc sample rate. */

#define MVM_WB_SR ( 16000 )
  /**< Wide-band stream/vocproc sample rate. */

#define MVM_SWB_SR ( 32000 )
  /**< Super wide-band stream/vocproc sample rate. */

#define MVM_DEFAULT_DEC_SR ( 8000 ) /* Default decoder sample rate. */
#define MVM_DEFAULT_ENC_SR ( 8000 ) /* Default encoder sample rate. */

#define MVM_DEFAULT_VAR_VOC_DEC_SAMPLING_RATE ( 48000 ) /* Default decoder sample rate for variable vocoders. */
#define MVM_DEFAULT_VAR_VOC_ENC_SAMPLING_RATE ( 48000 ) /* Default encoder sample rate variable vocoders. */

#define MVM_DEFAULT_RX_PP_SR ( 8000 ) /* Default post-proc sample rate. */
#define MVM_DEFAULT_TX_PP_SR ( 8000 ) /* Default pre-proc sample rate. */

#define MVM_VOICE_FRAME_SIZE_US ( 20000 )
  /**<
   * The frame size in microseconds that each voice processing threads (vptx,
   * vprx, encoder, decoder, decoder pp) operates on.
   */

#define MVM_DEFAULT_VFR_CYCLE_DURATION_US ( 20000 )
  /**<
   * The Vocoder frame reference in microseconds. Voice processing chain runs periodically with
   * with this duration.
   */

#define MVM_DEFAULT_ENC_OFFSET_MARGIN_US ( 1000 )
#define MVM_DEFAULT_DEC_REQ_OFFSET_MARGIN_US ( 1000 )
#define MVM_DEFAULT_DEC_OFFSET_MARGIN_US ( 1000 )
  /**<
   * Margin that are added to voice timing reference during vocoder packet exchange 
   * as a safeguard to read/write vocoder packet.
   */

#define MVM_MAX_VOICE_TIMING_OFFSET_MARGIN_US ( 500 )
#define MVM_MAX_VOICE_TIMING_OFFSET_US ( 19500 )
  /**<
   * The maximum voice timing offset in microseconds that can be set on VCP for
   * each of the timing offsets (vp_tx_start_offset, vp_tx_delivery_offset,
   * enc_offset, dec_req_offset, dec_offset, dec_pp_start_offset,
   * vp_rx_delivery_offset).
   *
   * This is used by MVM to solve the following problem:
   * On each VFR interrupt (every 20ms), VCP sets up a number of one-shot
   * relative timers corresponding to each of the voice timing offsets set by
   * CVD. If a particular voice timing offset is high such that it is close to
   * 20ms, the corresponding timer may not fire before the next VFR interrupt
   * occurs, due to system jitter (i.e. VFR interrupt doesn't occur at exact
   * 20ms boundaries). This will cause the timer to be reset before it can
   * fire, which results in that the voice processing chain not operating as
   * expected, and leads to problems such as delivery misses for vocoder
   * packets.
   *
   * The current solution to solve the problem is to ensure a safety margin
   * of 500us for each voice timing offset that CVD set on VCP, i.e. each
   * voice timing offset can not exceed MVM_MAX_VOICE_TIMING_OFFSET_US. More
   * specifically:
   * On the uplink, offsets are moved backward if they exceed
   * MVM_MAX_VOICE_TIMING_OFFSET_US:
   * 1. If enc_offset is greater than MVM_MAX_VOICE_TIMING_OFFSET_US, it will
   *    be adjusted to MVM_MAX_VOICE_TIMING_OFFSET_US. The adjusted enc_offset
   *    will be used to derive vp_tx_delivery_offset.
   * 2. If vp_tx_delivery_offset is greater than
   *    MVM_MAX_VOICE_TIMING_OFFSET_US, it will be adjusted to
   *    MVM_MAX_VOICE_TIMING_OFFSET_US. The adjusted vp_tx_delivery_offset will
   *    be used to derive vp_tx_start_offset.
   * 3. If vp_tx_start_offset is greater than MVM_MAX_VOICE_TIMING_OFFSET_US,
   *    it will be adjusted to MVM_MAX_VOICE_TIMING_OFFSET_US.
   * On the downlink, offsets are moved forward if they exceed
   * MVM_MAX_VOICE_TIMING_OFFSET_US:
   * 1. If dec_req_offset is greater than MVM_MAX_VOICE_TIMING_OFFSET_US, it
   *    will be adjusted to 0us. The amount of adjustment made to
   *    dec_req_offset will be added to dec_offset.
   * 2. If dec_offset is greater than MVM_MAX_VOICE_TIMING_OFFSET_US, it will
   *    be adjusted to 0us. The adjusted dec_offset will be used to derive
   *    dec_pp_start_offset.
   * 3. If dec_pp_start_offset is greater than MVM_MAX_VOICE_TIMING_OFFSET_US,
   *    it will be adjusted to 0us. The adjusted dec_pp_start_offset will be
   *    used to derive vp_rx_delivery_offset.
   * 4. If vp_rx_delivery_offset is greater than
   *    MVM_MAX_VOICE_TIMING_OFFSET_US, it will be adjusted to 0us.
   *
   * TODO: VCP team will discuss if there are any better alternative solutions
   * to the problem, for future PLs. VSD team will update the code accordingly.
   */
#define MVM_VOCPROC_PROCESSING_TIME_SAFETY_MARGIN_US ( 1000 )
  /**<
   * Additional 1ms safety margin in the VpTx and VpRx timing offset that CVD
   * sets on VCP. This is needed to compensate for sample slipping/stuffing,
   * where VpTx/VpRx processing start time can be delayed by 1ms.
   */

#define MVM_TX_PROCESSING_TIME_SAFETY_MARGIN_US ( 500 )
  /**<
   * Additional 0.5ms safety margin in the VpTx and Venc timing offset that CVD
   * sets on VCP. This is needed to take care of thread preemption delay.
   */

#define MVM_HDVOICE_UI_DISABLED (0) /* VSS_IHDVOICE_CMD_DISABLE. */
#define MVM_HDVOICE_UI_ENABLED (1)  /* VSS_IHDVOICE_CMD_ENABLE. */
#define MVM_HDVOICE_UI_UNUSED (2)   /* VSS_IHDVOICE_CMD not used in the session. */
  /**< Controls the state of the HD Voice UI enablement for MVM. */
  
  
#define MVM_FRACT_RESAMP_BUFFER_TIME_US   ( 1000 )
/**< buffer time of 1000us(1ms) to accommodate jitter that results due to data arrival rate in 
   fractional resampling scenario. vocproc operates in integer rate and AFE is set to
   fractional sampling rate. */

#define MVM_SESSION_START_MPPS ( 90 )
/**< Max MPPS required for VPM session creation */

/*****************************************************************************
 * DEFINITIONS                                                               *
 ****************************************************************************/

typedef enum mvm_thread_state_enum_t
{
  MVM_THREAD_STATE_ENUM_INIT,
  MVM_THREAD_STATE_ENUM_READY,
  MVM_THREAD_STATE_ENUM_EXIT
}
  mvm_thread_state_enum_t;

typedef enum mvm_mem_mapping_mode_t
{
  MVM_MEM_MAPPING_MODE_ENUM_VIRTUAL,
  MVM_MEM_MAPPING_MODE_ENUM_PHYSICAL
}
  mvm_mem_mapping_mode_t;

typedef struct mvm_stream_vocproc_processing_time_us_t
{
  uint16_t enc;
  uint16_t dec;
  uint16_t dec_pp;
  uint16_t vp_tx;
  uint16_t vp_rx;
}
  mvm_stream_vocproc_processing_time_us_t;

/****************************************************************************
 * WORK QUEUE DEFINITIONS                                                   *
 ****************************************************************************/

typedef struct mvm_work_item_t
{
  apr_list_node_t link;
  aprv2_packet_t* packet;
}
  mvm_work_item_t;

/****************************************************************************
 * COMMAND RESPONSE FUNCTION TABLE                                          *
 ****************************************************************************/

typedef void ( *mvm_event_handler_fn_t ) ( aprv2_packet_t* packet );

typedef enum mvm_response_fn_enum_t
{
  MVM_RESPONSE_FN_ENUM_ACCEPTED,
  MVM_RESPONSE_FN_ENUM_RESULT,
  MVM_RESPONSE_FN_ENUM_MAP_MEMORY,
  MVM_RESPONSE_FN_ENUM_SET_SYSTEM_CONFIG,
  MVM_RESPONSE_FN_ENUM_GET_AVSYNC_DELAYS,
  MVM_RESPONSE_FN_ENUM_GET_PKTEXG_MODE,
  MVM_RESPONSE_FN_ENUM_GET_MAILBOX_PKTEXG_TIME_REFERENCE,
  MVM_RESPONSE_FN_ENUM_GET_HDVOICE_CONFIG,
  MVM_RESPONSE_FN_ENUM_OPEN_SOFT_VFR,
  MVM_RESPONSE_FN_ENUM_INVALID,
  MVM_RESPONSE_FN_ENUM_MAX = MVM_RESPONSE_FN_ENUM_INVALID
}
  mvm_response_fn_enum_t;

/**
 * Pending commands may load different sets of response and event handlers to
 * complete each job. The response function table is equivalent to the state
 * design pattern. The state context is stored in the pending command control.
 * Pending commands can be as simple or as complex as required.
 */
typedef mvm_event_handler_fn_t mvm_response_fn_table_t[ MVM_RESPONSE_FN_ENUM_MAX ];

/****************************************************************************
 * SESSION CONTROL DEFINITIONS                                              *
 ****************************************************************************/

typedef enum mvm_state_enum_t
{
  MVM_STATE_ENUM_UNINITIALIZED,
    /**< Reserved. */
  MVM_STATE_ENUM_RESET_ENTRY,
    /**< Move into or out of reset. */
  MVM_STATE_ENUM_RESET,
    /**< The session resource is not acquired. */
  MVM_STATE_ENUM_INIT_ENTRY,
    /**< Move into or out of init. */
  MVM_STATE_ENUM_INIT,
    /**< The session resource is not brought up. */
  MVM_STATE_ENUM_VOCPROC_RUN_ENTRY,
    /**< Move into or out of vocproc_run. */
  MVM_STATE_ENUM_VOCPROC_RUN,
    /**< The vocproc session resource is running and stream session resource
      *  is ready to run. */
  MVM_STATE_ENUM_STREAM_RUN_ENTRY,
    /**< Move into or out of stream_run. */
  MVM_STATE_ENUM_STREAM_RUN,
   /**< The stream session resource is running and vocproc session resource
     *  is ready to run. */
  MVM_STATE_ENUM_RUN_ENTRY,
   /**< Move into or out of run. */
  MVM_STATE_ENUM_RUN,
   /**< The session resource are running. */
  MVM_STATE_ENUM_INVALID,
  MVM_STATE_ENUM_MAX
    /**< Reserved. */
}
  mvm_state_enum_t;

typedef enum mvm_goal_enum_t
{
  MVM_GOAL_ENUM_UNINITIALIZED,
  MVM_GOAL_ENUM_NONE,
  MVM_GOAL_ENUM_CREATE,
  MVM_GOAL_ENUM_DESTROY,
  MVM_GOAL_ENUM_VOCPROC_DISABLE,
  MVM_GOAL_ENUM_VOCPROC_RECONFIG,
  MVM_GOAL_ENUM_VOCPROC_ENABLE,
  MVM_GOAL_ENUM_STREAM_DISABLE,
  MVM_GOAL_ENUM_STREAM_RECONFIG,
  MVM_GOAL_ENUM_STREAM_ENABLE,
  MVM_GOAL_ENUM_INVALID,
  MVM_GOAL_ENUM_MAX
}
  mvm_goal_enum_t;

typedef enum mvm_action_enum_t
{
  MVM_ACTION_ENUM_UNINITIALIZED,
    /* Common actions. */
  MVM_ACTION_ENUM_NONE,
    /**< The first action has not started for a goal from any state. */
  MVM_ACTION_ENUM_COMPLETE,
    /**<
     * Reached the last action for a goal from a state. A multi-action goal
     * that starts from and ends in the same state may require a COMPLETE
     * action to properly differentiate a terminate signal.
     */
  MVM_ACTION_ENUM_CONTINUE,
    /**<
     * For multi-state goals, the last action from each state should set to
     * CONINTUE. This indicates to the next state that a goal is continuing
     * its operation from a previous state. Usually the previous state is
     * known given the current state and the continued goal. New actions can
     * be created to help discriminate the direction from where goals come
     * from as required.
     */

   /* VOCPROC actions. */
   MVM_ACTION_ENUM_VOCPROC_REINIT,
   MVM_ACTION_ENUM_VOCPROC_LOAD_COUNT_RESET,
   MVM_ACTION_ENUM_VOCPROC_SET_SYSTEM_CONFIG,
   MVM_ACTION_ENUM_VOCPROC_SET_TIMING_OFFSET,
   MVM_ACTION_ENUM_VOCPROC_ENABLE,
   MVM_ACTION_ENUM_VOCPROC_DISABLE,
   MVM_ACTION_ENUM_VOCPROC_GET_HDVOICE,
   MVM_ACTION_ENUM_VOCPROC_SET_HDVOICE,
   MVM_ACTION_ENUM_VOCPROC_GET_AVSYNC_DELAYS,

   /* STREAM actions. */
   MVM_ACTION_ENUM_STREAM_REINIT,
   MVM_ACTION_ENUM_STREAM_LOAD_COUNT_RESET,
   MVM_ACTION_ENUM_STREAM_SET_SYSTEM_CONFIG,
   MVM_ACTION_ENUM_STREAM_SET_TIMING_OFFSET,
   MVM_ACTION_ENUM_STREAM_SET_AVSYNC_DELAYS,
   MVM_ACTION_ENUM_STREAM_DISABLE,
   MVM_ACTION_ENUM_STREAM_ENABLE,
   MVM_ACTION_ENUM_STREAM_SET_HDVOICE,
   MVM_ACTION_ENUM_STREAM_SET_TTY_MODE,
   MVM_ACTION_ENUM_STREAM_ENCODER_RESET,

  /* COMMON actions. */
  MVM_ACTION_ENUM_EVALUATE_TIMING_PARAMS,
  MVM_ACTION_ENUM_CALCULATE_TIMING_OFFSETS,
  MVM_ACTION_ENUM_CALCULATE_AVSYNC_DELAYS,
  MVM_ACTION_ENUM_DETACH_VOCPROCS_FROM_STREAMS,
  MVM_ACTION_ENUM_CCM_KPPS_UPDATE,
  MVM_ACTION_ENUM_CSM_CONFIG_UPDATE,

  MVM_ACTION_ENUM_INVALID,
  MVM_ACTION_ENUM_MAX
}
  mvm_action_enum_t;

typedef struct mvm_control_t
{
  uint32_t statejob_handle;

  mvm_state_enum_t state;
  mvm_goal_enum_t goal;
  mvm_action_enum_t action;
  uint32_t status;
}
  mvm_control_t;

/****************************************************************************
 * MVM OBJECT DEFINITIONS                                                   *
 ****************************************************************************/

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * THE COMMON OBJECT DEFINITIONS                                           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

typedef enum mvm_object_type_enum_t
{
  MVM_OBJECT_TYPE_ENUM_UNINITIALIZED,
  MVM_OBJECT_TYPE_ENUM_SESSION,
  MVM_OBJECT_TYPE_ENUM_INDIRECTION,
  MVM_OBJECT_TYPE_ENUM_SIMPLE_JOB,
  MVM_OBJECT_TYPE_ENUM_SEQUENCER_JOB,
  MVM_OBJECT_TYPE_ENUM_INVALID
}
  mvm_object_type_enum_t;

typedef struct mvm_object_header_t
{
  uint32_t handle;
    /**< The handle to the associated apr_objmgr_object_t instance. */
  mvm_object_type_enum_t type;
    /**<
     * The object type defines the actual derived object.
     *
     * The derived object can be any custom object type. A session or a
     * command are two such custom object types. A free object entry is set
     * to MVM_OBJECT_TYPE_ENUM_FREE.
     */
}
  mvm_object_header_t;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * THE SESSION OBJECT                                                      *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
 * mvm_generic_item_t is a generic linked list node data structure whose
 * intent is to maximize recycling of linked list node storage space.
 * mvm_generic_item_t linked list nodes contain a handle. The handle is
 * dynamically re-purposed based on the use case.
 *
 * An example of this is in the session object where there is a free_items_q
 * of generic items. The free_item_q are dequeued and enqueued into the
 * indirection_q (storing MVM indirection objects) or the vocproc_q (storing
 * CVP handles). Once the item storage is no longer needed in the respective
 * list, the free item is moved back into the free_item_q.
 */
typedef struct mvm_generic_item_t
{
  apr_list_node_t link;

  uint32_t handle;
    /**< Any custom handle. */
}
  mvm_generic_item_t;

typedef struct mvm_voice_processing_delays_t
{
  uint16_t vptx_proc;
  uint16_t vprx_proc;
  uint16_t venc_proc;
  uint16_t vdec_proc;
}
  mvm_voice_processing_delays_t;

typedef struct mvm_voice_timing_t
{
  bool_t is_timing_ref_available;
  uint64_t tx_ref_timestamp_us;
  uint64_t rx_ref_timestamp_us;

  uint32_t enc_offset;
  uint32_t dec_req_offset;
  uint32_t dec_offset;
  uint32_t vprx_delivery_offset;
  uint32_t vptx_buf_start_offset;
  uint32_t vptx_proc_start_offset;
  uint32_t vptx_delivery_offset;

  uint16_t vptx_preemption_safety_margin;
  uint16_t vprx_preemption_safety_margin;
  uint16_t venc_preemption_safety_margin;
  uint16_t vdec_preemption_safety_margin;
}
  mvm_voice_timing_t;

typedef struct mvm_vfr_subscription_info_t
{
  uint32_t vsid;
  uint16_t vfr_mode;
  uint32_t vfr_cycle_duration_us;
  bool_t is_subscribed;
  uint64_t vfr_start_reference_us;
}
  mvm_vfr_subscription_info_t;

typedef struct mvm_clock_and_concurrency_config_t
{
  bool_t is_multi_session;
    /**<
     * Indicates whether the voice system contains more than one voice session
     * or more than one stream or more than one vocproc active.
     */
  uint32_t total_core_kpps;
    /**< VDSP current core KPPS requested for voice use cases. */
}
  mvm_clock_and_concurrency_config_t;

typedef struct mvm_stream_vocproc_load_t
{
  uint16_t num_nb_streams;
    /**< Number of narrow-band (8 KHz) streams attached to an MVM session. */
  uint16_t num_wb_streams;
    /**< Number of wide-band (16 KHz) streams attached to an MVM session. */
  uint16_t num_swb_streams;
    /**< Number of super wide-band (32 KHz) streams attached to an MVM session. */
  uint16_t num_fb_plus_streams;
    /**< Number of full-band (48 KHz) or higher streams attached to an MVM session. */
  uint16_t num_nb_vocprocs;
    /**<  Number of narrow-band (8 KHz) vocprocs attached to an MVM session. */
  uint16_t num_wb_vocprocs;
    /**< Number of wide-band (16 Khz) vocprocs attached to an MVM session. */
  uint16_t num_swb_vocprocs;
    /**< Number of super wide-band (32 Khz) vocprocs attached to an MVM session. */
  uint16_t num_fb_plus_vocprocs;
    /**< Number of full-band (48 Khz) or higher vocprocs attached to an MVM session. */
  uint32_t total_kpps;
    /**< Total KPPS for all the streams and vocprocs attached to an MVM session. */
  uint32_t total_stream_kpps;
    /**< Total KPPS for all the streams attached to an MVM session. */
  uint32_t total_vocproc_kpps;
    /**< Total KPPS for all the vocprocs attached to an MVM session. */
  uint32_t vocproc_rx_topology_id;
    /**< Vocproc Rx path topology ID. Applicable only if there is a single vocproc
         attached to this MVM session. It is ignored otherwise. */
  uint32_t vocproc_tx_topology_id;
    /**< Vocproc Tx path topology ID. Applicable only if there is a single vocproc
         attached to this MVM session. It is ignored otherwise. */
  uint32_t stream_media_id;
    /**< Stream Media ID.  Applicable only if there is a single stream
         attached to this MVM session. It is ignored otherwise. */
  uint32_t enc_kpps;
    /**< Encoder KPPS requirements. Applicable only if there is a single stream
         attached to this MVM session. It is ignored otherwise. */
  uint32_t dec_kpps;
    /**< Decoder KPPS requirements. Applicable only if there is a single stream
         attached to this MVM session. It is ignored otherwise. */
  uint32_t dec_pp_kpps;
    /**< Stream RX post-processing block KPPS requirements. Applicable only if
         there is a single stream attached to this MVM session. It is ignored
         otherwise. */
  uint32_t vp_rx_kpps;
    /**< Vocproc RX KPPS requirements. Applicable only if there is a single vocproc
         attached to this MVM session. It is ignored otherwise. */
  uint32_t vp_tx_kpps;
    /**< Vocproc TX KPPS requirements. Applicable only if there is a single vocproc
         attached to this MVM session. It is ignored otherwise. */
  uint32_t tx_num_channels;
    /**< Tx Number of channels. Applicable only if there is a single active
         voice session which has a single vocproc attached to it. It is ignored
         otherwise. */
  uint32_t tx_mpps_scale_factor;
    /** Tx MPPS scale factor. */
  uint32_t tx_bw_scale_factor;
    /** Tx BW scale factor. */
  uint32_t rx_mpps_scale_factor;
    /** Rx MPPS scale factor. */
  uint32_t rx_bw_scale_factor;
    /** Rx BW scale factor. */
  uint32_t tx_afe_port_sr;
    /** Tx AFE port SR. */
  uint32_t rx_afe_port_sr;
    /** Rx AFE port SR. */
  uint32_t ec_ref_afe_port_sr;
    /** EC Ref AFE port SR. */
  uint32_t vptx_frame_size_us;
    /** VpTx frame size in us. */
}
  mvm_stream_vocproc_load_t;

typedef struct mvm_avsync_vocproc_delays_info_t
{
  uint32_t max_vp_rx_algorithmic_delay;
    /**< Max vocproc rx path algorithmic delay. */
  uint32_t max_vp_tx_algorithmic_delay;
    /**< Max vocproc tx path algorithmic delay. */
  uint32_t vp_rx_normalized_total_delay;
    /**< vocproc rx path algorithmic + processing delay. */
  uint32_t vp_tx_normalized_total_delay;
    /**< vocproc tx path algorithmic + processing delay. */
}
  mvm_avsync_vocproc_delays_info_t;

typedef enum mvm_client_state_enum_t
{
  MVM_CLIENT_STATE_ENUM_STOP,
  MVM_CLIENT_STATE_ENUM_STANDBY,
  MVM_CLIENT_STATE_ENUM_PAUSE,
  MVM_CLIENT_STATE_ENUM_START
}
  mvm_client_state_enum_t;

typedef struct mvm_session_object_t
{
  mvm_object_header_t header;

  /* Private housekeeping variables. */
  mvm_generic_item_t generic_pool[ MVM_MAX_GENERIC_OBJECTS_PER_SESSION_V ];
  apr_list_t free_item_q;
    /**< A list of free mvm_generic_item_t entries. The handle is undefined. */

  /* Internal variables. */
  char_t session_name[ MVM_MAX_SESSION_NAME_SIZE ];

  uint16_t self_addr;

  bool_t is_dual_control;
    /* Indicates that both the evaluation of the apps and modem state are needed. */

  /* Full control client. */
  uint16_t master_addr;
  uint16_t master_port;

  /* Indirection queue. */
  apr_list_t indirection_q;
    /**<
     * A list of mvm_generic_item_t entries. The handle is an MVM indirection
     * object handle.
     */

  /* Stream management. */
  apr_list_t stream_q;
    /**< A list of mvm_generic_item_t entries. The handle is a CVS handle. */
  uint32_t stream_rsp_cnt;
    /**< CVS group operation response counter. */

  bool_t is_stream_reinit_required;
    /**< Indicates whether a stream re-init is required. */
  bool_t is_stream_reconfig_required;
    /**< Indicates whether a stream re-configuration is required. */
  uint32_t requested_var_voc_rx_sampling_rate;
  uint32_t requested_var_voc_tx_sampling_rate;
    /**<
     * Variable vocoder sampling rates requested by client. Upon
     * VSS_IMVM_CMD_SET_MAX_VAR_VOC_SAMPLING_RATE we cache them here. On
     * every VSS_IMVM_CMD_SET_MAX_VAR_VOC_SAMPLING_RATE or
     * VSS_IMVM_CMD_SET_CAL_MEDIA_TYPE, we determine the target_set enc,
     * dec and pp sample rates based on the media type and the requested
     * sample rates.
     */

  /* Vocproc management. */
  apr_list_t vocproc_q;
    /**< A list of mvm_generic_item_t entries. The handle is a CVP handle. */
  uint32_t vocproc_rsp_cnt;
    /**< CVP group operation response counter. */
  bool_t is_vocproc_reinit_required;
    /**< Indicates whether a vocproc re-init is required. */
  bool_t is_vocproc_reconfig_required;
    /**< Indicates whether a vocproc re-configuration is required. */

  uint32_t stream_attach_detach_vocproc_rsp_cnt;
      /**< CVS multiple attach/detach vocproc commands response counter. */
  uint32_t detach_all_rsp_cnt;
    /**< Detach all vocprocs from all streams response counter. */

  /* Policy management. */
  mvm_control_t session_ctrl;

  uint16_t is_hdvoice_ui_enabled;
    /**< TRUE: client has enabled HD Voice on UI via VSS_IHDVOICE_CMD_ENABLE */

  bool_t is_wv2_enabled;
    /**< FALSE: client has disabled HD Voice on UI via VSS_IHDVOICE_CMD_WV2_DISABLE */

  bool_t is_beamr_enabled;
    /**< FALSE: client has disabled HD Voice on UI via VSS_IHDVOICE_CMD_BEAMR_DISABLE */

  bool_t is_avtimer_handle_open;
    /**< Facilitate per Session reference count for avtimer_drv_handle
     * in vocsvc_avtimer.c.
     */
  mvm_client_state_enum_t modem_state;
    /**< The valid values are: STOP, STANDBY, and START. */
  mvm_client_state_enum_t apps_state;
    /**< The valid values are: STOP, PAUSE, STANDBY, and START. */

  mvm_avsync_vocproc_delays_info_t avsync_vocproc_delay;
  vss_icommon_cmd_set_system_config_t system_config;
  mvm_vfr_subscription_info_t vfr_info; 
  mvm_voice_timing_t voice_timing;
  mvm_voice_processing_delays_t voice_proc_delays;
  mvm_clock_and_concurrency_config_t clock_and_concurrency_config;
  mvm_stream_vocproc_load_t stream_vocproc_load;
  uint32_t tty_mode;
  uint32_t widevoice_enable; /* BACKWARD COMPATIBILITY */

  apr_lock_t lock;
    /* Per session object lock. To guard multi-thread access of session object. */
}
  mvm_session_object_t;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * THE SESSION INDIRECTION OBJECT                                          *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#define MVM_INDIRECTION_ACCESS_FULL_CONTROL_MASK ( 0x00000001 )
#define MVM_INDIRECTION_ACCESS_FULL_CONTROL_SHFT ( 0 )

typedef struct mvm_indirection_object_t
{
  mvm_object_header_t header;

  uint16_t client_addr;
    /**<
     * The address of the MVM client that this indirection object is created
     * for. When MVM client creates either a full or passive session control,
     * MVM creates a session indirection object whose handle is provided
     * to the client for controling the session.
     */

  uint32_t session_handle;
    /**< The handle to the named session mvm_session_object_t. */

  uint32_t access_bits;
    /**<
     * The indirection access bit-flags:
     *
     * Bit 0: 0: Passive control.
     *        1: Full control.
     */
}
  mvm_indirection_object_t;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * THE SIMPLE JOB OBJECT                                                   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

typedef struct mvm_simple_job_object_t
{
  mvm_object_header_t header;

  uint32_t context_handle;
    /**<
     * The context handle provides additional information or storage for the
     * simple job to complete.
     *
     * The context can be anything; a session, a parent-job, etc. The
     * context is especially useful to applications that override the
     * default simple job response function table to perform additional
     * processing such as state transitions.
     *
     * Set this value to -1 when it is unused or when there is no parent.
     */
  mvm_response_fn_table_t fn_table;
    /**<
     * This is the response function v-table. The response table can store
     * custom response routines for all possible responses directed to this
     * specific job.
     */
  bool_t is_accepted;
    /**< The command accepted response flag. 0 is false and 1 is true. */
  bool_t is_completed;
    /**< The command completed response flag. 0 is false and 1 is true. */
  uint32_t status;
    /**< The status returned by the command completion. */
}
  mvm_simple_job_object_t;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * THE SEQUENCER JOB OBJECT                                                *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

typedef struct mvm_sequencer_attach_stream_struct_t
{
  uint16_t cvs_handle;
}
  mvm_sequencer_attach_stream_struct_t;

typedef struct mvm_sequencer_detach_stream_struct_t
{
  uint16_t cvs_handle;
}
  mvm_sequencer_detach_stream_struct_t;

typedef struct mvm_sequencer_attach_vocproc_struct_t
{
  uint16_t cvp_handle;
}
  mvm_sequencer_attach_vocproc_struct_t;

typedef struct mvm_sequencer_detach_vocproc_struct_t
{
  uint16_t cvp_handle;
}
  mvm_sequencer_detach_vocproc_struct_t;

typedef struct mvm_sequencer_map_unmap_memory_struct_t
{
  uint32_t mem_handle;
  vss_imemory_cmd_map_virtual_t* map_virtual_arg;
  vss_imemory_cmd_map_physical_t* map_physical_arg;
}
  mvm_sequencer_map_unmap_memory_struct_t;

typedef struct mvm_sequencer_ssr_cleanup_struct_t
{
  uint8_t domain_id;
}
  mvm_sequencer_ssr_cleanup_struct_t;

typedef union mvm_sequencer_use_case_union_t
{
  mvm_sequencer_attach_stream_struct_t attach_stream;
  mvm_sequencer_detach_stream_struct_t detach_stream;
  mvm_sequencer_attach_vocproc_struct_t attach_vocproc;
  mvm_sequencer_detach_vocproc_struct_t detach_vocproc;
  mvm_sequencer_map_unmap_memory_struct_t map_unmap_memory;
  mvm_sequencer_ssr_cleanup_struct_t ssr_cleanup;
}
  mvm_sequencer_use_case_union_t;

typedef struct mvm_sequencer_job_object_t
{
  mvm_object_header_t header;

  uint32_t state;
    /**< The generic state variable. */
  union mvm_object_t* subjob_obj;
    /**< The current sub-job object. */
  uint32_t status;
    /**< A status value. */

  mvm_sequencer_use_case_union_t use_case;
}
  mvm_sequencer_job_object_t;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * THE GENERIC CVD OBJECT                                                  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

typedef union mvm_object_t
{
  mvm_object_header_t header;
  mvm_session_object_t session;
  mvm_indirection_object_t indirection;
  mvm_simple_job_object_t simple_job;
  mvm_sequencer_job_object_t sequencer_job;
}
  mvm_object_t;

/****************************************************************************
 * PENDING COMMAND CONTROL DEFINITIONS                                      *
 ****************************************************************************/

typedef enum mvm_pending_cmd_state_enum_t
{
  MVM_PENDING_CMD_STATE_ENUM_FETCH,
    /**< Fetch the next pending command to execute. */
  MVM_PENDING_CMD_STATE_ENUM_EXECUTE,
    /**< Execute the current pending command for the first time. */
  MVM_PENDING_CMD_STATE_ENUM_CONTINUE
    /**< Continue executing the current pending command. */
}
  mvm_pending_cmd_state_enum_t;

/**
 * The pending command control structure stores information used to process
 * pending commands serially.
 *
 * Pending commands have state and are executed one at a time until
 * completion. A gated pending command gates all the pending commands that
 * follows it. A gated pending command should employ abort timers to allow the
 * system to make progress.
 *
 * Aborting critical pending commands can leave the system in a bad state. It
 * is recommended that (at the very least) the configurations of the aborted
 * critical pending commands be saved so that on error recovery the proper
 * configurations could be restored.
 */
typedef struct mvm_pending_control_t
{
  apr_list_t cmd_q;
    /**< The pending (mvm_work_item_t) command queue. */
  mvm_pending_cmd_state_enum_t state;
    /**<
     * The current state of the pending command control.
     *
     * This variable is managed by the pending command processor. The
     * individual pending command controls indicates to the pending command
     * processor to complete or to delay the completion of the current
     * pending command.
     */
  aprv2_packet_t* packet;
    /**<
     * The current (command) packet being processed.
     */
  mvm_object_t* pendjob_obj;
    /**<
     * The pendjob_obj is a temporary storage for the current pending
     * command.
     */
}
  mvm_pending_control_t;

/****************************************************************************
 * SUBSYSTEM RESTART (SSR) RELATED TRACKING                                 *
 ****************************************************************************/

typedef struct mvm_ssr_cleanup_cmd_tracking_t
{
  uint32_t num_cmd_issued;
    /**<
     * Number of the commands issued by MVM to itself in each of the cleanup
     * sequencer state, when doing cleanup due to the subsystem where MVM
     * clients reside is being restarted.
     */
  uint32_t rsp_cnt;
    /**<
     * Response counter for the commands issued by MVM to itself in each of the
     * cleanup sequencer state, when doing cleanup due to the subsystem where
     * MVM clients reside is being restarted.
     */
}
  mvm_ssr_cleanup_cmd_tracking_t;

typedef struct mvm_ssr_mem_handle_tracking_item_t
{
  apr_list_node_t link;

  uint16_t client_addr;
    /**< The address of the client who mapped the memory. */
  uint32_t mem_handle;
    /**< The handle to the mapped memory. */
}
  mvm_ssr_mem_handle_tracking_item_t;

typedef struct mvm_ssr_mem_handle_tracking_t
{
  mvm_ssr_mem_handle_tracking_item_t pool[ MVM_SSR_MAX_TRACKED_MEM_HANDLES ];
  apr_list_t free_q;
  apr_list_t used_q;
}
  mvm_ssr_mem_handle_tracking_t;

typedef struct mvm_voice_activity_client_info_t
{
  bool_t is_enabled;
    /**< Indicatea whether voice acitvity update notification is enabled. */

  uint16_t client_addr;
    /**< Address of the client who listens to the voice activty update 
         notification. */

  uint16_t client_port;
    /**< Port of the client who listens to the voice activty update 
         notification. */
}
  mvm_voice_activity_client_info_t;


/****************************************************************************
 * PROTOTYPES                                                               *
 ****************************************************************************/

#endif /* __MVM_I_H__ */

