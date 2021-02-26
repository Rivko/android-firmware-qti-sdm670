#ifndef _VSM_HEADER_I_
#define _VSM_HEADER_I_
/*
  Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Header: //components/rel/avs.adsp/2.8.5/vsd/common/cvd/cvs/inc/vsm_header_i.h#5 $
  $Author: pwbldsvc $
*/

/* This file contains the common definitions of types, macros and some high level functions used by VSM.
*/

#include "Elite.h"
#include "qurt_elite.h"
#include "err.h"
#include "apr_comdef.h"
#include "VoiceMixerSvc.h"
#include "adsp_vsm_api.h"
#include "vss_public_if.h"
#include "vss_private_if.h"
#include "cvd_cal_private_i.h"
#include "mvm_api_i.h"

#define VSM_COMM_ERROR( rc, dst_addr ) \
  if ( rc )\
  { MSG_2( MSG_SSID_DFLT, MSG_LEGACY_FATAL,"VSM APR comm error 0x%08X, \
                                dst_addr = 0x%04X", rc, dst_addr );\
  }

#define VSM_PANIC_ON_ERROR( rc ) \
  { if ( rc ) { ERR_FATAL( "VSM: Error[0x%08x]", rc, 0, 0 ); } }

#define VSM_REPORT_FATAL_ON_ERROR( rc ) \
  { if ( rc ) { MSG_1( MSG_SSID_DFLT, MSG_LEGACY_FATAL,\
                       "VSM: Error[0x%08x]", rc ); } }

/* Ideally there is no limit to the number of passive clients supported.
   However, adding a place holder for tracking purpose.
*/
#define VSM_NUM_MAX_PASSIVE_CLIENTS ( 5 )

#define VSM_ACTION_UNINITIALIZED ( 0 )

#define VSM_MAX_SESSION_NAME_SIZE ( 31 )

#define VSM_ASYNC_CMD_TOKEN_TX ( 0x00000001 )
/**< This token is used while communicating to venc. */

#define VSM_ASYNC_CMD_TOKEN_RX ( 0x00000002 )
/**< This token is used while communicating to vdec. */

#define VSM_ASYNC_CMD_TOKEN_VOCPROC ( 0x00000003 )
/**< This token is used while communicating to vocproc. */

#define VSM_ASYNC_CMD_TOKEN_PARAM_INBAND_TX ( 0x00000004 )

#define VSM_ASYNC_CMD_TOKEN_PARAM_OOB_TX ( 0x00000005 )

#define VSM_ASYNC_CMD_TOKEN_PARAM_INBAND_RX ( 0x00000006 )

#define VSM_ASYNC_CMD_TOKEN_PARAM_OOB_RX ( 0x00000007 )

#define VSM_MAX_ATTACHED_VOCPROC_CNT ( 4 )

#define VSM_ATTACHED_VOCPROC_HANDLE_INVALID ( 0xFFFF )

#define VSM_DEFAULT_DEC_SR ( 8000 )
  /**< Default decoder sample rate. */
#define VSM_DEFAULT_ENC_SR ( 8000 )
  /**< Default encoder sample rate. */

#define VSM_DEFAULT_VAR_VOC_DEC_SAMPLING_RATE ( 48000 )
  /**< Default variable vocoder decoder sample rate. */
#define VSM_DEFAULT_VAR_VOC_ENC_SAMPLING_RATE ( 48000 )
  /**< Default variable vocoder encoder sample rate. */

#define VSM_MAX_OOB_VOC_PACKET_BUFFER_SIZE ( 1932 )
  /**< Buffer size in bytes required to hold the maximum size vocoder packet
       used in out-of-band packet exchange. The size includes 12 bytes of
       header (i.e. 4 bytes timestamp, 4 bytes media type, 4 bytes data size)
       plus the actual data size for the maximum vocoder packet, i.e. PCM_48KHZ
       (1920 bytes). */

#define VSM_MAILBOX_DEC_REQUEST_OFFSET_SAFETY_MARGIN ( 2000 )
  /**< Time in microseconds behind of the client's mailbox Rx timeline for
       picking up a decoder packet. The safety margin is to ensure that the
       stream picks up a decoder packet at some time after the client
       provides a packet, to avoid read and write race condition due to
       timing jitters. */

#define VSM_MAILBOX_AVSYNC_RX_DELAY_UPDATE_THRESHOLD_US ( 2000 )
  /**< The threshold in microseconds of change in Rx path AVSync delay detected
       during mailbox vocoder packet exchange operation that triggers sending
       the updated Rx path delay to the client (if the client registered for
       AVSync Rx event class). */

#define VSM_MAILBOX_ENC_OFFSET_SAFETY_MARGIN ( 2000 )
  /**< Time in microseconds ahead of the client's mailbox Tx timeline for
       producing an encoder packet. The safety margin is to ensure that there
       is sufficient time for the stream to process an encoder packet such that
       the client's encoding request is completed on time. */

#define VSM_MAILBOX_DEC_REQUEST_OFFSET_SAFETY_MARGIN ( 2000 )
  /**< Time in microseconds behind of the client's mailbox Rx timeline for
       picking up a decoder packet. The safety margin is to ensure that the
       stream picks up a decoder packet at some time after the client
       provides a packet, to avoid read and write race condition due to
       timing jitters. */

#define VSM_MAILBOX_DEC_OFFSET_SAFETY_MARGIN ( 2000 )
  /**< Time in microseconds between when VSM request for a decoder packet to
       when VSM must start decoding a packet. The safety margin is to ensure
       that there is sufficient time for CVS to process (such as error
       checking) a decoder packet and deliver (such as mem_cpy and sending an
       OOB packet exchange event via APR) a packet to VSM. */

#define VSM_MAILBOX_DEC_EXPIRY_HANDLING_SAFETY_MARGIN ( 1000 )
  /**< Time in microseconds between the expected time of receiving a decoder
       packet request from VSM to when CVS begins dropping expired decoding
       request during call setup time. This is used for dropping expired
       decoding request during call setup time before the decoder becomes
       active. */

#define VSM_MAILBOX_DEC_WINDOW ( VSM_VOC_PACKET_DURATION_US + 1000 )
  /**< The decoding window in microseconds. A decoding request with a timestamp
       of more than a decoding window in the past is considered as an expired
       request and will be dropped. The decoding window is
       VSM_VOC_PACKET_DURATION_US plus a 1ms safety margin. */

#define VSM_MAILBOX_VOC_PACKET_ERROR_STATUS_MASK ( 1 )
  /**< Mailbox vocoder packet error status bit mask. */

#define VSM_DEFAULT_RX_PP_SR ( 8000 )
  /**< Default post-proc sample rate. */
#define VSM_DEFAULT_TX_PP_SR ( 8000 )
  /**< Default pre-proc sample rate. */

  /**< Default values for any stream property as long as 
       the property is not set by the full control client. */
#define VSM_STREAM_PROPERTY_NOT_SET_UINT32 ( 0xFDFDFDFD )

#define VSM_STREAM_PROPERTY_NOT_SET_UINT8 ( 0xFE )
#define VSM_STREAM_PROPERTY_NOT_APPLICABLE_UINT8 ( 0xFF )

#define VSM_STREAM_VOC_BANDWIDTH_EVT_NOT_SET ( 0xFE )
  /**< Vocoder Bandwidth Event sent by VSM_EVT_VOC_OPERATING_MODE_UPDATE
       is not yet received. */

#define VSM_NUM_COMMON_CAL_COLUMNS ( 3 ) /* BACKWARD COMPATIBILITY */
  /**< Number of common cal columns is 3: network ID, Tx PP sampling rate, and
       Rx PP sampling rate. If more columns are added, this value has to be
       increased. */

#define VSM_MAX_NUM_MATCHING_COMMON_CAL_ENTRIES ( 1 << VSM_NUM_COMMON_CAL_COLUMNS ) /* BACKWARD COMPATIBILITY */

#define VSM_NUM_STATIC_CAL_COLUMNS ( 7 )
  /**< Number of static cal columns is 7: network ID, Tx PP sampling rate, Rx
       PP sampling rate, Tx vocoder operating mode, Rx vocoder operating mode,
       media ID, and feature. If more columns are added, this value has to be
       increased. */

#define VSM_MAX_UI_PROP ( 20 )
#define VSM_MAX_UI_PROP_DATA_LEN ( 12 )
  /**<
   * UI prop data can hold only enable/disable params
   * If UI properties intend to hold more params,
   * this size need to be increased
   */

#define VSM_MAX_NUM_OF_STREAM_PARAMS ( 6 ) 
#define VSM_MAX_PARAM_DATA_LEN ( 4 )  /* 4 bytes. */
#define VSM_VOC_PARAM_CACHE_LEN ( VSM_MAX_NUM_OF_STREAM_PARAMS * ( VSM_MAX_PARAM_DATA_LEN + sizeof( vss_istream_voc_param_data_t ) ) )

#define VSM_DIRECTION_TX ( 0 )
#define VSM_DIRECTION_RX ( 1 )
#define VSM_DIRECTION_RX_TX ( 2 )

#define VSM_STREAM_PROPERTY_DTX_MODE_ENABLE ( 1 )
#define VSM_STREAM_PROPERTY_DTX_MODE_DISABLE ( 0 )
  /**< DTX mode set by the clients of VSM */

#define VSM_SESSION_CREATE_MPPS ( 50 )
  /**< Max MPPS required for VSM session creation */

#define VSM_CACHE_LINE_SIZE (128)

/**
 * Event-ID is allocated by the DIAG team.
 *
 * This event is used to logs EVS vocoder parameters like bandwidth,
 * rate information during voice call set-up and steady state.
 *
 * Only purpose of redefinition is to avoid compilation dependency 
 * on DIAG. This macro should be replaced by EVENT_VOICE_EVS_PARAMS 
 * once DIAG changes are avaiable.
 */
#define VSM_EVENT_VOICE_EVS_PARAMS ( 2883 )


/** NO DATA (no transmission) frame for
 * VSS_MEDIA_ID_G711_MULAW_V2 and VSS_MEDIA_ID_G711_ALAW_V2
 */
#define G711_NO_DATA_FRAME_TYPE (2)

/** No DATA frame for VSS_MEDIA_ID_EVS */
#define EVS_NO_DATA_FRAME_TYPE (15)

/** No DATA frame for VSS_MEDIA_ID_AMR_NB and VSS_MEDIA_ID_AMR_WB */
#define AMR_NO_DATA_FRAME_TYPE (7)



typedef struct vsm_session_static_info_t 
{
  qurt_elite_queue_t*   cmdQ;
  /**< Client to session cmd Q */
  qurt_elite_queue_t*   respQ;
  /**< Other svcs to session response Q */
  uint32_t              session_cmdQ_mask;
  /**< Mask for Session Cmd Q */
}
  vsm_session_static_info_t;

typedef enum vsm_voc_property_enum_t
{
  VSM_VOC_PROPERTY_ENUM_RATE_MODULATION,
  VSM_VOC_PROPERTY_ENUM_MINMAX_RATE,
  VSM_VOC_PROPERTY_ENUM_ENC_RATE,
  VSM_VOC_PROPERTY_ENUM_CHANNEL_AWARE_MODE,
  VSM_VOC_PROPERTY_ENUM_ENC_OPERATING_MODE
}
  vsm_voc_property_enum_t;
/**< DTX mode is also a vocoder proerty which is handled seperately because 
  * it can modified by both full control client and passive control clients 
  */

typedef struct vsm_sesion_apr_info_t vsm_sesion_apr_info_t;

struct vsm_sesion_apr_info_t
{
   /*The following is necessary information for direct communication with client
    and for talking with APR*/
   uint32_t    apr_handle;
   /**< This is the apr handle that is required in sending ack. */
   uint16_t    self_addr;
   /**< self's address (16bit) */
   uint16_t    self_port;
   /**< slef's port (16bit) */
   uint16_t    client_addr;
   /**< Client's address (16bit)*/
   uint16_t    client_port;
   /**< Client's port (16bit) */
};

struct passive_client_info_t
{
   uint16_t    client_addr;
   /**< Client's address (16bit) */
   uint16_t    client_port;
   /**< Client's port (16bit) */
};

#define voice_strm_apr_info_t vsm_sesion_apr_info_t

/** @brief This structure is used to maitain the state of GSM-TTY.*/
struct voice_strm_tty_state_t
{
   uint16_t m_etty_mode;
   /**< TTY operation mode - OFF/HCO/VCO/FULL, set by VSM in stop state, read by CTM Rx/Tx (Dec/Enc) >*/
   int32_t m_ctm_from_far_end_detected;
   /**< variable that CTM Rx (Dec) writes, CTM Tx (Enc) reads */
   int32_t m_enquiry_from_far_end_detected;
   /**< variable that CTM Rx (Dec) writes, CTM Tx (Enc) reads/clears */
   int32_t m_rx_tty_detected;
   /**< variable that CTM Rx (Dec) writes, Tx CNG (Enc) reads */
   int32_t m_ctm_character_transmitted;
   /**< variable that CTM Tx (Enc) writes, CTM Rx (Dec) reads */
   int32_t m_sync_recover_rx;
   /**< variable that VSM writes based on RESYNC_CTM, CTM Rx (Dec) reads/resets */
   int32_t m_sync_recover_tx;
   /**< variable that VSM writes based on RESYNC_CTM, CTM Tx (Dec) reads/resets */
   uint16_t m_oobtty_mode;
   /**< VoLTE TTY operation mode - OFF/HCO/VCO/FULL, set by VSM in stop state >*/
   int32_t m_rx_oobtty_detected;
   /**< variable that VoLTE TTY Rx (Dec) writes, Tx VoLTE (Enc) reads */
};

typedef struct vsm_vocproc_table_t
{
  uint16_t tx_device_cnt;
  uint16_t rx_device_cnt;
  uint16_t tx_device_handle[VSM_MAX_ATTACHED_VOCPROC_CNT];
  uint16_t rx_device_handle[VSM_MAX_ATTACHED_VOCPROC_CNT];

  uint16_t vpm_handles[VSM_MAX_ATTACHED_VOCPROC_CNT];
  bool_t   vpm_isready[VSM_MAX_ATTACHED_VOCPROC_CNT];

  uint16_t vpm_ready_cnt;
  uint32_t tx_port_id[VSM_MAX_ATTACHED_VOCPROC_CNT];
  uint32_t rx_port_id[VSM_MAX_ATTACHED_VOCPROC_CNT];
  uint32_t ec_ref_port_id[VSM_MAX_ATTACHED_VOCPROC_CNT];
}
  vsm_session_vocproc_table_t;

typedef struct vsm_dtmf_settings_t
{
  uint16_t direction;
  int16_t  mixing;
  uint16_t tone1;
  uint16_t tone2;
  int32_t  duration;
}
  vsm_session_dtmf_settings_t;

typedef enum vsm_enable_state_enum_t
{
  VSM_DISABLED = 0,
  VSM_ENABLED = 1
}
  vsm_enable_state_enum_t;

typedef enum vsm_record_state_enum_t
{
  VSM_RECORD_STATE_IDLE,
  VSM_RECORD_STATE_WAIT_ON_AFE_CONNECT,
  VSM_RECORD_STATE_AFE_CONNECTED,
  VSM_RECORD_STATE_WAIT_ON_START_RECORDING,
  VSM_RECORD_STATE_RECORDING,
  VSM_RECORD_STATE_WAIT_ON_STOP_RECORDING,
  VSM_RECORD_STATE_RECORDING_STOPPED,
  VSM_RECORD_STATE_WAIT_ON_AFE_DISCONNECT
 //VSM_RECORD_STATE_IDLE 
}
  vsm_record_state_enum_t;
  
typedef struct vsm_recording_settings_t
{
  uint32_t rx_tap_point;
  uint32_t tx_tap_point;
  uint16_t port_id;
  uint32_t mode;
  vsm_enable_state_enum_t enable_flag;
  vsm_record_state_enum_t state;
}
  vsm_recording_settings_t;

typedef enum vsm_playback_state_enum_t
{
  VSM_PLAYBACK_STATE_IDLE,
  VSM_PLAYBACK_STATE_WAIT_ON_VOCPROC_CREATE,
  VSM_PLAYBACK_STATE_VOCPROC_CREATED,
  VSM_PLAYBACK_STATE_WAIT_ON_PLAYBACK_SET_SYSTEM_CONFIG,
  VSM_PLAYBACK_STATE_ATTACH_VOCPROC,
  VSM_PLAYBACK_STATE_WAIT_ON_ATTACH_VOCPROC,
  VSM_PLAYBACK_STATE_VOCPROC_ATTACHED,
  VSM_PLAYBACK_STATE_WAIT_ON_MIXER_RESPONSE,
  VSM_PLAYBACK_STATE_MIXER_CONFIGURED,
  VSM_PLAYBACK_STATE_WAIT_ON_SET_TIMING,
  VSM_PLAYBACK_STATE_VOCPROC_TIMING_SET,
  VSM_PLAYBACK_STATE_WAIT_ON_START_VOCPROC,
  VSM_PLAYBACK_STATE_PLAYING,
  VSM_PLAYBACK_STATE_WAIT_ON_DETACH_VOCPROC,
  VSM_PLAYBACK_STATE_VOCPROC_DETACHED,
  VSM_PLAYBACK_STATE_WAIT_ON_STOP_VOCPROC,
  VSM_PLAYBACK_STATE_VOCPROC_STOPPED,
  VSM_PLAYBACK_STATE_WAIT_ON_VOCPROC_DESTROY
 //VSM_PLAYBACK_STATE_IDLE 
}
  vsm_playback_state_enum_t;

typedef struct vsm_playback_settings_t
{
  uint16_t port_id;
  vsm_enable_state_enum_t  enable_flag;
  vsm_playback_state_enum_t state;
  uint16_t vpm_port_id;
}
  vsm_playback_settings_t;

typedef struct vsm_mute_settings_t
{
  uint16_t tx_mute_flag;
  uint16_t rx_mute_flag;
  uint32_t tx_ramp_duration;
  uint32_t rx_ramp_duration;  
}
  vsm_mute_settings_t;
  
typedef struct vsm_session_settings_t
{
  uint16_t direction;
  uint32_t media_id;
  vss_icommon_cmd_set_system_config_t system_config;
  vsm_vocproc_table_t attach_table;
  vsm_recording_settings_t record;
  vsm_playback_settings_t playback;
  vsm_mute_settings_t mute;
  vss_istream_cmd_set_voice_timing_t voice_timing;
  /* TODO: The following need to be cleaned up along with the clean-up for
   * vocproc attachment handling. The following are used to store the vocproc
   * handle and the stream handle (indirection handle) that are needed to be
   * attached together.
   */
  uint16_t attach_detach_vocproc_handle;
  uint16_t attach_detach_vocproc_direction;
  uint16_t attach_detach_stream_handle;
}
  vsm_session_settings_t;

typedef enum vsm_state_enum_t
{
  VSM_STATE_ENUM_UNINITIALIZED,
    /**< Reserved. */
  VSM_STATE_ENUM_IDLE,
    /**< The session resource is ready to run. */
  VSM_STATE_ENUM_RUN,
    /**< The session resource is running. */
  VSM_STATE_ENUM_ERROR,
    /**< The session resource is unusable and should be destroyed. */
  VSM_STATE_ENUM_INVALID
    /**< Reserved. */
}
  vsm_state_enum_t;

typedef struct vsm_session_state_ctrl_t{

//TBD:  Remove/reduce whichever is not required

//   vsm_goal_enum_t goal;
    /**<
     * The goal field is the current goal for the state machine. Goals are
     * defined to complete one at a time.
     *
     * CAUTION: The goal field is intended for the user of the state machine
     *          to indicate the goal. Any attempt by the state machine to
     *          manipulate the goal will lead to live locking and
     *          unmaintainable code.
     */
  vsm_state_enum_t state;
    /**<
     * The state field is the current state of the state machine.
     *
     * CAUTION: The state field is intended for the state machine to keep
     *          track of its current state. Any attempt to manipulate the
     *          state field from outside the state machine will lead to
     *          live locking and unmaintainable code.
     */
  //vsm_action_enum_t action;
    /**<
     * The action field is the next action to perform by the state machine.
     *
     * CAUTION: The action field is intended by the state machine to keep
     *          track of the series of steps to transition out of the current
     *          state or to complete a goal. Any attempt to manipulate the
     *          action field from outside the state machine will lead to
     *          live locking, system panics due to unhandled actions in the
     *          state, and unmaintainable code.
     */
  uint32_t status;
    /**<
     * The status field reports the status of the last action.
     */

}
  vsm_session_state_ctrl_t;

typedef struct vsm_cmd_ctrl_t
{
  qurt_elite_queue_t  *cmdQ;
  /**< Pointer to the command queue associated with the session. */
  qurt_elite_queue_t  *respQ;
  /**< Pointer to the response queue associated with the session. */
  elite_msg_any_t  cmd_msg;
  uint32_t         cmd_result;
  elite_msg_any_t  resp_msg;
  bool_t           is_resp_pending;
  bool_t           is_cmd_pending;
  uint32_t         current_action;
  uint32_t         issued_count;
  uint32_t         resp_count;
  uint32_t         resp_result;
  bool_t           is_destroy_cmd_pending;
}
  vsm_cmd_ctrl_t;

typedef struct vsm_voc_operating_mode_info_t
{
  bool_t is_rx_mode_received;
    /**<
      * Indicates whether the VSM_EVT_VOC_OPERATING_MODE_UPDATE event
      * containing the initial Rx path vocoder mode has been received from VSM.
      */
  bool_t is_tx_mode_received;
    /**<
      * Indicates whether the VSM_EVT_VOC_OPERATING_MODE_UPDATE event
      * containing the initial Tx path vocoder mode has been received from VSM.
      */
  uint32_t rx_mode;
    /**< VSM reported Rx vocoder operating mode. */
  uint32_t tx_mode;
    /**< VSM reported Tx vocoder operating mode. */
}
  vsm_voc_operating_mode_info_t;

typedef struct vsm_oob_packet_exchange_info_t
{
  bool_t is_configured;
    /**< Flag indicates if OOB packet exchange is properly configured. */
  bool_t is_enc_buf_consumed;
    /**< Flag indicates if OOB encoder packet is consumed by client. */
  vsm_config_packet_exchange_t config;
    /**< Packet exchange configuration set on VSM. */
}
  vsm_oob_packet_exchange_info_t;

typedef struct vsm_mailbox_packet_exchange_config_t
{
  uint32_t cvs_mem_handle;
    /**< Handle to the shared memory used for mailbox packet exchange between
         CVS and its client. */
  vss_ipktexg_mailbox_voc_req_circ_buffer_t* tx_circ_buf;
    /**< Tx path vocoding request circular buffer. */
  uint32_t tx_circ_buf_mem_size;
    /**< Tx path vocoding request circular buffer memory size in bytes. */
  vss_ipktexg_mailbox_voc_req_circ_buffer_t* rx_circ_buf;
    /**< Rx path vocoding request circular buffer. */
  uint32_t rx_circ_buf_mem_size;
    /**< Rx path vocoding request circular buffer memory size in bytes. */
  uint32_t max_enc_pkt_size;
    /**< The maximum possible encoder packet size for the media ID currently
         configured on the stream. */
  uint32_t max_dec_pkt_size;
    /**< The maximum possible decoder packet size for the media ID currently
         configured on the stream. */
}
  vsm_mailbox_packet_exchange_config_t;

/* Mailbox packet exchange statistics for debugging purpose. */
typedef struct vsm_mailbox_packet_exchange_stats_t
{
  uint32_t num_no_rx_req_in_buf;
    /**< Number of times that no decoding request found in the Rx circular
         buffer upon decoder requesting for a packet, since the decoder is
         started. */
  uint32_t num_expired_rx_req_dropped;
    /**< Number of times that expired decoding request found in the Rx circular
         buffer upon decoder requesting for a packet, since the decoder is
         started. */
  uint32_t num_future_rx_req_skipped;
    /**< Number of times that future decoding request found in the Rx circular
         buffer upon decoder requesting for a packet, since the decoder is
         started. */
  uint32_t num_invalid_rx_req_dropped;
    /**< Number of times that invalid decoding request found in the Rx circular
         buffer upon decoder requesting for a packet, since the decoder is
         started. */
  uint32_t num_error_status_rx_req_dropped;
    /**< Number of times that an error status (i.e. error status bit is set)
         decoding request found in the Rx circular buffer upon decoder
         requesting for a packet, since the decoder is started. */
  uint32_t num_no_tx_req_in_buf;
    /**< Number of times that no encoding request found in the Tx circular
         buffer upon encoder generating a packet, since the encoder is
         started. */
  uint32_t num_invalid_tx_req_dropped;
    /**< Number of times that invalid encoding request found in the Tx circular
         buffer upon encoder generating a packet, since the encoder is
         started. */
}
  vsm_mailbox_packet_exchange_stats_t;

typedef struct vsm_mailbox_packet_exchange_info_t
{
  bool_t is_configured;
    /**< Flag indicates if mailbox packet exchange is properly configured. */
  vsm_mailbox_packet_exchange_config_t config;
    /**< Mailbox packet exchange configuration. */
  vsm_mailbox_packet_exchange_stats_t stats;
    /**< Mailbox packet exchange statistics for debugging purpose. */
}
  vsm_mailbox_packet_exchange_info_t;

typedef struct vsm_packet_exchange_info_t
{
  uint32_t mode;
    /**< Vocoder packet exchange mode.
         In band = VSS_IPKTEXG_MODE_IN_BAND.
         OOB = VSS_IPKTEXG_MODE_OUT_OF_BAND.
         Mailbox = VSS_IPKTEXG_MODE_MAILBOX */
  vsm_oob_packet_exchange_info_t oob_info;
    /**< OOB packet exchange info. Applicable only if the mode is set to
         VSS_IPKTEXG_MODE_OUT_OF_BAND. */
  vsm_mailbox_packet_exchange_info_t mailbox_info;
    /**< Mailbox packet exchange info. Applicable only if the mode is set to
         VSS_IPKTEXG_MODE_MAILBOX. */
}
  vsm_packet_exchange_info_t;

typedef struct vsm_tty_client_info_t
{
  uint16_t ittyoob_client_addr;
    /**< Address of cleint who registers for ITTYOOB. */
  uint16_t ittyoob_client_port;
    /**< Port of cleint who registers for ITTYOOB. */
  uint32_t tty_mode;
    /**< Represent TTY mode set for TTY device. */
  bool_t is_ittyoob_registered;
    /**< flag = FALSE is unregistered indicating In band TTY
         and flag = TRUE is registered indicating Out of band TTY. */
}
  vsm_tty_client_info_t;

typedef struct vsm_iavsync_client_info_t
{
  bool_t is_enabled;
    /**< Flag to indicate whether client has registered for
         VSS_IAVSYNC_EVENT_CLASS_RX. */
  uint16_t client_addr;
    /**< Address of the client who listens to the IAVSYNC RX Delay
         notification. */
  uint16_t client_port;
    /**< Port of the client who listens to the IAVSYNC RX Delay
         notification. */
}
  vsm_iavsync_client_info_t;

typedef struct vsm_iavsync_delays_info_t
{
  uint32_t stream_rx_algorithmic_delay;
    /**< Stream algorithmic RX delay. */
  uint32_t stream_tx_algorithmic_delay;
    /**< Stream algorithmic TX delay. */
  uint32_t total_rx_delay;
    /**< Total normalized vocproc + stream rx delay. */
  uint32_t total_tx_delay;
    /**< Total normalized vocproc + stream rx delay. */
}
  vsm_iavsync_delays_info_t;

typedef struct vsm_set_dtmf_gen_settings_t
{
  uint16_t client_addr;
  uint16_t client_port;
  uint16_t vsm_port;
}
  vsm_set_dtmf_gen_settings_t;

typedef struct vsm_set_rx_dtmf_detect_settings_t
{
  uint16_t client_addr;
  uint16_t client_port;
  uint16_t vsm_port;
  vsm_enable_state_enum_t enable_flag;
}
  vsm_set_rx_dtmf_detect_settings_t;

typedef struct vsm_kpps_info_t
{
  uint32_t enc;
  uint32_t dec;
  uint32_t dec_pp;
}
  vsm_kpps_info_t;

typedef struct vsm_voc_property_qcelp13k_t
{
  uint32_t rate;
    /**< Average vocoder rate. */
  uint32_t min_rate;
    /**< Lower boundary of the encoder rate. */
  uint32_t max_rate;
    /**< Upper boundary of the encoder rate. */
  uint32_t reduced_rate_mode;
    /**< Reduced_rate_mode of the encoder rate. */
}
  vsm_voc_property_qcelp13k_t;

typedef struct vsm_voc_property_evrc_t
{
  uint32_t rate;
    /**< Average vocoder rate. */
  uint32_t min_rate;
    /**< Lower boundary of the encoder rate. */
  uint32_t max_rate;
    /**< Upper boundary of the encoder rate. */
  uint32_t reduced_rate_mode;
    /**< Reduced_rate_mode of the encoder rate. */
}
  vsm_voc_property_evrc_t;

typedef struct vsm_voc_property_4gvnb_t
{
  uint32_t rate;
    /**< Average vocoder rate. */
  uint32_t min_rate;
    /**< Lower boundary of the encoder rate. */
  uint32_t max_rate;
    /**< Upper boundary of the encoder rate. */
}
  vsm_voc_property_4gvnb_t;

typedef struct vsm_voc_property_4gvwb_t
{
  uint32_t rate;
    /**< Average vocoder rate. */
  uint32_t min_rate;
    /**< Lower boundary of the encoder rate. */
  uint32_t max_rate;
    /**< Upper boundary of the encoder rate. */
}
  vsm_voc_property_4gvwb_t;

typedef struct vsm_voc_property_4gvnw_t
{
  uint32_t rate;
    /**< Average vocoder rate. */
  uint32_t min_rate;
    /**< Lower boundary of the encoder rate. */
  uint32_t max_rate;
    /**< Upper boundary of the encoder rate. */
  uint32_t dtx_mode;
    /**< DTX mode value. */
}
  vsm_voc_property_4gvnw_t;

typedef struct vsm_voc_property_4gvnw2k_t
{
  uint32_t rate;
    /**< Average vocoder rate. */
  uint32_t min_rate;
    /**< Lower boundary of the encoder rate. */
  uint32_t max_rate;
    /**< Upper boundary of the encoder rate. */
  uint32_t dtx_mode;
    /**< DTX mode value. */
}
  vsm_voc_property_4gvnw2k_t;

typedef struct vsm_voc_property_amr_t
{
  uint32_t rate;
    /**< AMR encoder rate. */
  uint32_t dtx_mode;
    /**< DTX mode value. */
}
  vsm_voc_property_amr_t;

typedef struct vsm_voc_property_amrwb_t
{
  uint32_t rate;
    /**< AMRWB encoder rate. */
  uint32_t dtx_mode;
    /**< DTX mode value. */
}
  vsm_voc_property_amrwb_t;

typedef struct vsm_voc_property_eamr_t
{
  uint32_t rate;
    /**< EAMR encoder rate. */
  uint32_t dtx_mode;
    /**< DTX mode value. */
}
  vsm_voc_property_eamr_t;

typedef struct vsm_voc_property_efr_t
{
  uint32_t dtx_mode;
    /**< DTX mode value. */
}
  vsm_voc_property_efr_t;

typedef struct vsm_voc_property_fr_t
{
  uint32_t dtx_mode;
    /**< DTX mode value. */
}
  vsm_voc_property_fr_t;

typedef struct vsm_voc_property_hr_t
{
  uint32_t dtx_mode;
    /**< DTX mode value. */
}
  vsm_voc_property_hr_t;

typedef struct vsm_voc_property_g711_alaw_t
{
  uint32_t dtx_mode;
    /**< DTX mode value. */
}
  vsm_voc_property_g711_alaw_t;

typedef struct vsm_voc_property_g711_mulaw_t
{
  uint32_t dtx_mode;
    /**< DTX mode value. */
}
  vsm_voc_property_g711_mulaw_t;

typedef struct vsm_voc_property_g711_alaw_v2_t
{
  uint32_t dtx_mode;
    /**< DTX mode value. */
}
  vsm_voc_property_g711_alaw_v2_t;

typedef struct vsm_voc_property_g711_mulaw_v2_t
{
  uint32_t dtx_mode;
    /**< DTX mode value. */
}
  vsm_voc_property_g711_mulaw_v2_t;

typedef struct vsm_voc_property_g711_linear_t
{
  uint32_t dtx_mode;
    /**< DTX mode value. */
}
  vsm_voc_property_g711_linear_t;

typedef struct vsm_voc_property_g729_t
{
  uint32_t dtx_mode;
    /**< DTX mode value. */
}
  vsm_voc_property_g729_t;

typedef struct vsm_voc_property_evs_t
{
  uint8_t mode;
    /**< Operating bit-rate of the vocoder encoder. */
  uint8_t bandwidth;
    /**<  Operating audio bandwidth of the vocoder encoder. */
  uint8_t channel_aware_enabled;
    /**< Inicates whether channel aware mode is enabled.
       1 - Enabled,
       0 - Disabled.
       CVS_STREAM_PROPERTY_NOT_SET_UINT8 - value not set. */
  uint8_t fec_offset;
    /**< Specifies the forward-error correction offset.
         @values 2, 3, 5, 7 */
  uint8_t fer_rate;
    /**< Specifies FER rate threshold to LOW (0) or HIGH (1). */
  uint32_t dtx_mode;
    /**< DTX mode value. */
}
  vsm_voc_property_evs_t;

typedef struct vsm_voc_property_passive_ctrl_t
  {
    uint32_t dtx_mode;
      /**<
        * DTX mode value for passive control clients.
        * eCall is a passive control client for VSM. During eCall MSD transmission the DTX mode 
        * should be disabled. So to facilitate DTX mode setting by a passive control client 
        * this is introduced. The clients can disable/enable the flag using command 
        * VSS_ISTREAM_CMD_SET_ENC_DTX_MODE. 
        *
        * The logic here is DTX enable will be sent to encoder when both full control client 
        * DTX mode and passive control client DTX mode is enabled or one client has enabled it 
        * and the other client has not set DTX mode.
        */
  }
  vsm_voc_property_passive_ctrl_t;

typedef struct vsm_enc_channel_aware_mode_params_t
{
  uint8_t channel_aware_enabled;
    /**< Inicates whether channel aware mode is enabled.
       1 - Enabled,
       0 - Disabled.
       VSM_STREAM_PROPERTY_NOT_SET_UINT8 - value not set. */
  uint8_t fec_offset;
    /**< Specifies the forward-error correction offset.
         @values 2, 3, 5, 7  */
  uint8_t fer_rate;
    /**< Specifies FER rate threshold to LOW (0) or HIGH (1). */
}
  vsm_enc_channel_aware_mode_params_t;

typedef struct vsm_evs_bandwidth_change_notification_info_t
{
  bool_t is_enabled;
    /**< Flag to indicate whether EVS BW change notification is enabled. */
  uint16_t client_addr;
    /**< Address of the client who listens to the EVS bandwidth
         change notification. */
  uint16_t client_port;
    /**< Port of the client who listens to the EVS bandwidth change
         notification. */
  uint8_t last_received_rx_bandwidth;
    /**< Last known EVS Rx bandwidth received from VSM. */
  uint8_t last_sent_rx_bandwidth;
    /**< Last known EVS Rx bandwidth sent to registered client. */    
}
  vsm_evs_bandwidth_change_notification_info_t;

typedef struct vsm_packet_logging_info_t
{
  uint32_t voice_call_num;
  uint32_t rx_packet_seq_num;
  uint32_t tx_packet_seq_num;
}
  vsm_packet_logging_info_t;

typedef struct vsm_eamr_mode_change_notification_info_t
{
  bool_t is_enabled;
    /**< Flag to indicate whether eAMR mode change notification is enabled. */
  uint16_t client_addr;
    /**< Address of the client who listens to the eAMR mode change
         notification. */
  uint16_t client_port;
    /**< Port of the client who listens to the eAMR mode change
         notification. */
  uint32_t mode;
    /**< Last known eAMR mode. */
}
  vsm_eamr_mode_change_notification_info_t;

typedef struct vsm_hdvoice_enable_param_info_t
{
  voice_param_data_t param_hdr;
    /**< VSM reported Tx vocoder operating mode. */
  uint16_t enable;
    /**< Enablement value. Supported values: \n
         - 0 -- Disabled
         - 1 -- Enabled */
  uint16_t reserved;
}
  vsm_hdvoice_enable_param_info_t;

typedef struct vsm_vocoder_reset_param_info_t
{
  voice_param_data_t param_hdr;
    /**< VSM reported Tx vocoder operating mode. */
  uint16_t encoder_reset;
    /**< if set encoder will be reset */
  uint16_t decoder_reset;
   /**< if set decoder will be reset */
}
  vsm_vocoder_reset_param_info_t;

typedef struct vsm_static_cal_info_t
{
  bool_t is_registered;
  bool_t is_calibrate_needed;
    /**<
      * Indicates whether the stream static calibration data need to be
      * applied. Specifically, this flag is set to TRUE under the following
      * circumstances:
      *
      *   - upon a new set of static calibration data being registered with the
      *     stream.
      *
      *   - upon the stream has been re-initialized and there is static
      *     calibration data currently being registered with the stream.
      *
      *   - upon the stream receiving a VSS_ICOMMON_CMD_SET_SYSTEM_CONFIG command
      *     and any of the following parameters is different between the stream
      *     active_set and target_set: network_id, tx_pp_sr, rx_pp_sr,
      *     tx_voc_op_mode, rx_voc_op_mode, media ID, feature, and there is
      *     static calibration data currently registered.
      *
      * This flag is set to FALSE under the following circumstances:
      *
      *   - upon session creation.
      *
      *   - upon VSS_ICOMMON_CMD_SET_SYSTEM_CONFIG has been processed.
      *
      *   - upon static calibration being deregistered.
      */
  uint32_t table_handle;
  uint32_t query_handle;
  uint8_t matching_entries[ CVD_CAL_GET_REQUIRED_MATCHING_ENTRIES_BYTE_SIZE( VSM_NUM_STATIC_CAL_COLUMNS ) ];
    /**<
      * Book-keeping buffer to be provided to CVD CAL utility for maintaining
      * the location (address or offset) of the matching calibration entries in
      * the calibration table; so that multiple calls of cvd_cal_query with the
      * the same query_handle can simply return the calibration values based on
      * the cached matching entries rather than performing a fresh search on
      * every cvd_cal_query call.
      */
  cvd_cal_column_t query_key_columns[ VSM_NUM_STATIC_CAL_COLUMNS ];
}
  vsm_static_cal_info_t;

typedef struct vsm_common_cal_info_t /* BACKWARD COMPATIBILITY. */
{
  bool_t is_registered;
  bool_t is_evaluated;
  bool_t is_calibrate_needed;
    /**<
      * Indicates whether the stream calibration data need to be applied.
      * Specifically, this flag is set to TRUE under the following
      * circumstances:
      *
      *   - upon a new set of calibration data being registered with the stream.
      *
      *   - upon the stream has been re-initialized and there is calibration
      *     data currently being registered with the stream.
      *
      *   - upon the stream receiving a VSS_ICOMMON_CMD_SET_SYSTEM_CONFIG command
      *     and any of the following parameters is different between the stream
      *     active_set and target_set: network_id, tx_pp_sr, rx_pp_sr, and
      *     there is calibration data currently registered.
      *
      * This flag is set to FALSE under the following circumstances:
      *
      *   - upon session creation.
      *
      *   - upon VSS_ICOMMON_CMD_SET_SYSTEM_CONFIG has been processed.
      *
      *   - upon calibration being deregistered.
      */
  cvd_cal_table_handle_t* table_handle;
  uint32_t vsm_mem_handle;
  cvd_cal_entry_t matching_entries[ ( VSM_MAX_NUM_MATCHING_COMMON_CAL_ENTRIES *
                                      sizeof( cvd_cal_entry_t ) ) ];
  uint32_t num_matching_entries;
  uint32_t set_param_rsp_cnt;
  uint32_t set_param_failed_rsp_cnt;
}
  vsm_common_cal_info_t;

typedef struct vsm_ui_prop_cache_t
{
  uint32_t data_len;
  uint32_t version;
  uint32_t num_ui_prop;
  uint32_t cache_fail_count;
  uint8_t data [ ( ( VSM_MAX_UI_PROP_DATA_LEN +
                     sizeof( vss_icommon_param_data_v3_t ) ) *
                   VSM_MAX_UI_PROP ) ];
    /**<
     * The size of the data is
     * ( UI prop data length + header ) * max number UI props.
     */
}
  vsm_ui_prop_cache_t;

typedef struct vsm_stream_param_cache_t
{
  uint32_t data_len;
  uint32_t num_voc_params;
  uint8_t data [ VSM_VOC_PARAM_CACHE_LEN ];
    /**<
     * The size of the data is
     * ( set param data length + header ) * max number parameters.
     */
}
  vsm_stream_param_cache_t;

typedef enum vsm_evs_operating_mode_enum_t
{
  VSM_EVS_OPERATING_MODE_AMRWB_IO_6_60_KBPS = 0,
  VSM_EVS_OPERATING_MODE_AMRWB_IO_8_85_KBPS,
  VSM_EVS_OPERATING_MODE_AMRWB_IO_12_65_KBPS,
  VSM_EVS_OPERATING_MODE_AMRWB_IO_14_25_KBPS,
  VSM_EVS_OPERATING_MODE_AMRWB_IO_15_85_KBPS,
  VSM_EVS_OPERATING_MODE_AMRWB_IO_18_25_KBPS,
  VSM_EVS_OPERATING_MODE_AMRWB_IO_19_85_KBPS,
  VSM_EVS_OPERATING_MODE_AMRWB_IO_23_05_KBPS,
  VSM_EVS_OPERATING_MODE_AMRWB_IO_23_85_KBPS,
  VSM_EVS_OPERATING_MODE_EVS_5_90_KBPS,
  VSM_EVS_OPERATING_MODE_EVS_7_20_KBPS,
  VSM_EVS_OPERATING_MODE_EVS_8_00_KBPS,
  VSM_EVS_OPERATING_MODE_EVS_9_60_KBPS,
  VSM_EVS_OPERATING_MODE_EVS_13_20_KBPS,
  VSM_EVS_OPERATING_MODE_EVS_16_40_KBPS,
  VSM_EVS_OPERATING_MODE_EVS_24_40_KBPS,
  VSM_EVS_OPERATING_MODE_EVS_32_00_KBPS,
  VSM_EVS_OPERATING_MODE_EVS_48_00_KBPS,
  VSM_EVS_OPERATING_MODE_EVS_64_00_KBPS,
  VSM_EVS_OPERATING_MODE_EVS_96_00_KBPS,
  VSM_EVS_OPERATING_MODE_EVS_128_00_KBPS,
  VSM_EVS_OPERATING_MODE_MAX
}
  VSM_evs_operating_mode_enum_t;

typedef enum VSM_evs_fec_offset_enum_t
{
  VSM_EVS_FEC_OFFSET_2 = 2,
  VSM_EVS_FEC_OFFSET_3 = 3,
  VSM_EVS_FEC_OFFSET_5 = 5,
  VSM_EVS_FEC_OFFSET_7 = 7,
  VSM_EVS_FEC_OFFSET_MAX
}
  VSM_evs_fec_offset_enum_t;

typedef enum VSM_evs_fer_rate_enum_t
{
  VSM_EVS_FER_RATE_LOW = 0,
  VSM_EVS_FER_RATE_HIGH,
  VSM_EVS_FER_RATE_MAX
}
  VSM_evs_fer_rate_enum_t;

typedef struct vsm_vpcm_info_t
{
  bool_t is_enabled;
    /**< Flag to indicate whether vpcm is enabled or not. */
  uint16_t client_addr;
    /**< Address of VPCM client. */
  uint16_t client_handle;
    /**< Handle for VPCM client. */
  uint16_t cvs_handle;
    /**< Handle (indirection object) for CVS. */
  uint32_t mem_handle;
    /**< VPCM Client's memory handle. */
}
  vsm_vpcm_info_t;

#include "apr_pack_begin.h"
typedef struct vsm_evs_params_t {

  uint32_t vsid;
  uint8_t encoder_bandwidth;
  uint8_t decoder_bandwidth;
  uint8_t encoder_dtx;
  uint8_t encoder_rate;
  uint8_t channel_awareness_mode;
  uint8_t fec_offset;
  uint8_t fer_rate;

} vsm_evs_params_t 
#include "apr_pack_end.h"
;

typedef struct vsm_afe_dtmf_tone_generate_payload_t {

	int32_t dtmf_duration_in_ms;
	 /**< Duration of the DTMF tone in milliseconds. 
		  Duration includes both ramp up and ramp down.
	
			@values
			- - 1 -- Infinite tone
				- 0 -- Disable or stop infinite tone
				- > 0 -- Finite duration @tablebulletend */
	
	uint16_t dtmf_high_freq;
	/**< DTMF high tone frequency.	
	@values 1209, 1336, 1477, 1633 Hz */
	
	uint16_t dtmf_low_freq;
	/**< DTMF low tone frequency.	
	 @values 697, 770, 852, 941 Hz */
	
	uint16_t dtmf_gain;
	/**< Gain applied to a DTMF tone.	
	 @values Any number (in Q13 format) */
	
	uint16_t	mix_enable;
	/**< flag to enable DTMF tone mixing with
	other data on the same port. */

} vsm_afe_dtmf_tone_generate_payload_t; 

typedef struct vsm_session_object_t {

  elite_svc_handle_t    *venc;
  /**< Voice Enc handle */
  
  elite_svc_handle_t    *vdec;
  /**< Voice Dec handle */

  vsm_sesion_apr_info_t apr_info;

  passive_client_info_t passive_clients[ VSM_NUM_MAX_PASSIVE_CLIENTS ];

  vsm_session_state_ctrl_t  state_control;
  /**< Contains state control information.  */

  vsm_cmd_ctrl_t cmd_control;
  /**< Contains command control information. */

  char_t  session_name[ VSM_MAX_SESSION_NAME_SIZE ];

  uint32_t ref_count;
  /* Reference counter. To safely free session object and associated resources. */

  uint16_t attached_mvm_handle;
    /**<
      * The MVM session handle that this stream is attached to. A handle with
      * a value of APR_NULL_V indicates that this stream is not attached to an
      * MVM session.
      */
  uint16_t direction;

//TBD: Check if this is needed
  bool_t is_stream_config_changed;
    /**<
      * Indicates whether the stream configurations have changed since the
      * last time when VSS_ICOMMON_CMD_SET_SYSTEM_CONFIG has been processed.
      * Specifically, this flag is set to TRUE under the following
      * circumstances:
      *
      *   - upon the stream receiving a VSS_ISTREAM_CMD_SET_MEDIA_TYPE command
      *     with a different media type from that currently configured on the
      *     stream.
      *
      *   - upon the stream receiving a VSS_ISTREAM_CMD_SET_VAR_VOC_SAMPLING_RATE
      *     command such that it results in the stream to have different sample
      *     rates from those currently configured on the stream.
      *
      *   - upon a new set of calibration data being registered with the stream.
      *
      * All of the above events have the effect of potentially changing the
      * current KPPS requirements of the stream, and the stream must be
      * reconfigured.
      *
      * Upon the stream receiving a VSS_ISTREAM_CMD_ENABLE command, if
      * is_stream_config_changed is TRUE, the stream will send a
      * VSS_ISTREAM_EVT_RECONFIG event to MVM in order to notify MVM to
      * reconfigure this stream.
      *
      * This flag is set to FALSE under the following circumstances:
      *
      *   - upon session creation.
      *
      *   - upon VSS_ICOMMON_CMD_SET_SYSTEM_CONFIG has been processed.
      */

  vsm_voc_operating_mode_info_t voc_operating_mode_info;

  vsm_session_settings_t active_set;
  vsm_session_settings_t target_set;

  //TBD: these params need to be part of attach_table structure,
  uint32_t attached_vpm_inport_mapping;
  uint32_t attached_vpm_outport_mapping;
  uint32_t attached_vpm_session_handle;
  uint32_t attached_vpm_session_direction;

  vsm_packet_exchange_info_t packet_exchange_info;

  vsm_tty_client_info_t tty_info;
  vsm_iavsync_client_info_t avsync_client_tx_info;
  vsm_iavsync_client_info_t avsync_client_rx_info;  
  vsm_iavsync_delays_info_t avsync_delay_info;

  bool_t is_var_voc_sr_requested;
  uint32_t requested_var_voc_rx_sampling_rate;
  uint32_t requested_var_voc_tx_sampling_rate;
  uint32_t active_tx_sampling_rate;
  uint32_t active_rx_sampling_rate;

    /**<
     * Variable vocoder sampling rates requested by client. Upon
     * VSS_IMVM_CMD_SET_CAL_VAR_VOC_SAMPLING_RATE we cache them here. On
     * every VSS_IMVM_CMD_SET_CAL_VAR_VOC_SAMPLING_RATE or
     * VSS_IMVM_CMD_SET_CAL_MEDIA_TYPE, we determine the target_set enc,
     * dec and pp sample rates based on the media type and the requested
     * sample rates.
     */

  vsm_ui_prop_cache_t cached_ui_properties;
  vsm_stream_param_cache_t stream_param_cache;

  vsm_common_cal_info_t common_cal; /* BACKWARD COMPATIBILITY. */
    /**< Stream common calibration information. */

  vsm_static_cal_info_t static_cal;
    /**< Stream static calibration information. */

  vsm_set_dtmf_gen_settings_t set_dtmf_gen;
  vsm_set_rx_dtmf_detect_settings_t set_rx_dtmf_detect;

  vsm_vpcm_info_t vpcm_info;

  vsm_kpps_info_t kpps_info;
  bool_t is_kpps_changed;
  
  bool_t is_clk_overridden;
    /**< TRUE (1): Clock / BW / MPPS are voted for session creation.
     *   FALSE (0): Clock / BW / MPPS are not voted or released.
     */

  vsm_eamr_mode_change_notification_info_t eamr_mode_change_notification_info;
    /**<
     * Note: The existing VSM_CMD_SET_EAMR_MODE_CHANGE_DETECTION API is
     * deleted from VSM as part of the introduction of the new
     * VSM_EVT_VOC_OPERATING_MODE_UPDATE event. CVS will utilize the new event
     * to detect eAMR mode change and notify the registered client. The CVS
     * implementation maintains the same behavior as before in terms of when
     * the eAMR mode changed event is sent to the client. Specifically, the
     * behavior is as follows:
     * 1. Upon the client listens to the eAMR mode change event, the initial
     *    default eAMR mode is assumed to be NB.
     * 2. Subsequently, whenever the eAMR mode changes, the eAMR mode changed
     *    event is sent to the client.
     * 3. Upon VSM is re-initialized, the eAMR mode is reset back to default
     *    (i.e. NB).
     *
     * TODO: Current implementation allows only one client to listen to the
     * eAMR mode change notification. When there is a need to support multiple
     * listeners, we must store all the listeners' information and broadcast
     * the eAMR mode change notification event to all listeners.
     */
     
  vsm_evs_bandwidth_change_notification_info_t evs_bandwidth_change_notification_info;
     /**<
     * EVS bandwidth change notification information.  CVS uses the  
     * VSM_EVT_VOC_OPERATING_MODE_UPDATE event to detect EVS bandwidth changes and
     * will notify the registered client.  The VSM_EVT_VOC_OPERATING_MODE_UPDATE
     * event is sent before a call starts after VSM_CMD_SET_MEDIA_TYPE with EVS is 
     * sent to VSM. Also, if EVS is being used, VSM_CMD_SET_STREAM_PP_SAMP_RATE will
     * also trigger a mode update from VSM.  The EVS bandwidth event notification 
     * behavior is as follows:
     * 1. Upon client registration, if CVS is in the RUN state, the EVS bandwidth
     *    change event will be sent.
     * 2. Whenever the EVS bandwidth changes during a call, the EVS bandwidth change
     *    event is sent to the client.
     * 3. During CVS's state transition from IDLE -> RUN, the EVS bandwidth change 
     *    event is sent if the client is registered and if the previously last sent
     *    bandwidth is not the same as the current bandwidth.
     *
     * TODO: Current implementation allows only one client to listen to the
     * EVS bandwidth change notification. When there is a need to support multiple
     * listeners, we must store all the listeners' information and broadcast
     * the EVS bandwidth change notification event to all listeners.
     */

  vsm_packet_logging_info_t packet_logging_info;

  vsm_voc_property_qcelp13k_t cached_voc_properties_qcelp13k;
  vsm_voc_property_evrc_t cached_voc_properties_evrc;
  vsm_voc_property_4gvnb_t cached_voc_properties_4gvnb;
  vsm_voc_property_4gvwb_t cached_voc_properties_4gvwb;
  vsm_voc_property_4gvnw_t cached_voc_properties_4gvnw;
  vsm_voc_property_4gvnw2k_t cached_voc_properties_4gvnw2k;
  vsm_voc_property_amr_t cached_voc_properties_amr;
  vsm_voc_property_amrwb_t cached_voc_properties_amrwb;
  vsm_voc_property_eamr_t cached_voc_properties_eamr;
  vsm_voc_property_efr_t cached_voc_properties_efr;
  vsm_voc_property_fr_t cached_voc_properties_fr;
  vsm_voc_property_hr_t cached_voc_properties_hr;
  vsm_voc_property_g711_alaw_t cached_voc_properties_g711_alaw;
  vsm_voc_property_g711_mulaw_t cached_voc_properties_g711_mulaw;
  vsm_voc_property_g711_alaw_v2_t cached_voc_properties_g711_alaw_v2;
  vsm_voc_property_g711_mulaw_v2_t cached_voc_properties_g711_mulaw_v2;
  vsm_voc_property_g711_linear_t cached_voc_properties_g711_linear;
  vsm_voc_property_g729_t cached_voc_properties_g729;
  vsm_voc_property_evs_t cached_voc_properties_evs;
    /**< Cached vocoder properties required for each media id. 
      * TBD: Check if this caching is needed.*/

  vsm_voc_property_passive_ctrl_t cached_voc_properties_passive_ctrl;
    /**< Cached vocoder properties which are set by passive control clients. */

   // Include all required fileds from voice_strm_session_t from VoiceSvc.h here.
   vmx_inport_handle_t   *mixer_in_port_ptr;
   /**< Input port of mixer for Voice Dec */
   vmx_outport_handle_t  *mixer_out_port_ptr;
   /**< Output port of mixer for Voice Enc */
   uint32_t              input_mapping_mask;
   /**< indicates mapping from the voice decoder modules to vocproc rx modules */
   uint32_t              output_mapping_mask;
   /**< indicates mapping from the vocproc tx modules to encoders */
   int32_t               tx_port;
   /**< port mask corresponding to tx device */
   int32_t               rx_port;
   /**< port mask corresponding to rx device */

   //TBD: club this with CVS tty info
   voice_strm_tty_state_t tty_state;
   /**< TTY state info */

   uint16_t               pb_session;
   /**< Handle to voiceproc session used for in call audio delivery */
   elite_svc_handle_t     *tx_afe_handle_ptr;
   /**< saved handle to send pcm samples to be recorded from VENC to AFE service (returned from AFE connect) */
   elite_svc_handle_t     *rx_afe_handle_ptr;
   /**< saved handle to send pcm samples to be recorded from VDEC to AFE service (returned from AFE connect) */
#if 0
   uint32_t               rx_tap_point;
   /**< Tap point to use on rx path. Values are:
        - ::VSM_TAP_POINT_NONE -- Do not record rx path   
        - ::VSM_TAP_POINT_STREAM_END -- Rx tap point is at the end of the
                                stream*/
   uint32_t               tx_tap_point;
   /**< Tap point to use on tx path. Values are:
        - ::VSM_TAP_POINT_NONE -- Do not record tx path   
        - ::VSM_TAP_POINT_STREAM_END -- Tx tap point is at the end of the
                                stream*/
#endif
   uint32_t               record_mode;
   /**< recording mode that can be choosen by the user. Values are:
      - ::VSM_RECORD_TX_ONLY -- record only tx path   
        - ::VSM_RECORD_TX_ONLY -- record only rx path   
        - ::VSM_RECORD_TX_RX_STEREO -- record tx and rx paths in stereo mode   
      - ::VSM_RECORD_TX_RX_MIXING -- record mixed data of tx and rx paths*/
   uint16_t               aud_ref_port_rx;
   /**< Audio Front End (AFE) port ID for sending the Vdec to AFE.
        This can be real port or psuedo port.*/
   uint16_t               aud_ref_port_tx;
   /**< Audio Front End (AFE) port ID for sending the Venc samples to AFE.
        This can be real port or psuedo port.*/
   uint16_t               record_start_on;
   /**< flag to indicate record start command is on already or not*/

//TBD: Check if the following is redundant
   uint16_t               vfr_mode;
   /**< Sets the stream VFR mode. Values are:
        - 0 -- None
        - 1 -- Hard internal
        - 2 -- Hard external*/

   uint16_t               host_pcm_num_active_tap_points;     
   /**< used to track number of active host pcm tap points per session 
       (can stay enabled across RUN->STOP->RUN state transitions) */
   void                   *tx_afe_drift_ptr;
   /**< saved ptr to drift info struct (returned from Tx near-end AFE connect)  */
   void                   *rx_afe_drift_ptr;
   /**< saved ptr to drift info struct (returned from Rx AFE connect)  */   
//TBD: Check if thsi is needed
   uint16_t               timing_cmd_version;
   /**< keeps track of which version of timing command is used.
        needed for in-call music delivery use case where VSM need to 
        send right version of timing command to VPM as part of setup sequence.
        This is also being extended to identify clock requirements for ADSP Test fwk.
    */
   uint32_t               vsid;
   /**< voice systems id for hard vfr timing*/

   // Remove redundancies in above fields.   
   qurt_elite_queue_t     *vsm_afe_cmd_q_ptr ;
}
  vsm_session_object_t;

/***********************************************************************
 ************************Function Definitions***************************
 ***********************************************************************
*/

ADSPResult VSM_session_cmdQ_unsupported_msg_handler( vsm_session_object_t* session_object );

ADSPResult VSM_session_cmdQ_custom_msg_handler( vsm_session_object_t* session_object );

ADSPResult VSM_session_cmdQ_aprpkt_handler( vsm_session_object_t* session_object );

ADSPResult VSM_session_respQ_default_msg_handler( vsm_session_object_t* session_object );

ADSPResult VSM_session_respQ_unsupported_msg_handler( vsm_session_object_t* session_object );

ADSPResult VSM_session_respQ_aprpkt_handler( vsm_session_object_t* session_object );

ADSPResult VSM_session_respQ_get_param_msg_handler( vsm_session_object_t* session_object );

ADSPResult VSM_session_respQ_set_param_msg_handler( vsm_session_object_t* session_object );

ADSPResult VSM_session_respQ_custom_msg_handler( vsm_session_object_t* session_object );

#endif // _VSM_HEADER_I_
