/*
  Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Header: //components/rel/avs.adsp/2.8.5/vsd/common/cvd/cvs/src/vsm_svc.cpp#3 $
  $Author: pwbldsvc $
*/

#include "Elite.h"
#include "qurt_elite.h"
#include "mmstd.h"
#include "adsp_vsm_api.h"
#include "adsp_vpm_api.h"
#include "adsp_vparams_api.h"
#include "vss_public_if.h"
#include "vsm_apr_router.h"
#include "VoiceStreamMgr.h"
#include "audio_basic_op_ext.h"
#include "VoiceCmnUtils.h"
#include "vsm_header_i.h"
#include "vsm_common_i.h"
#include "cvd_log_i.h"
#include "vocsvc_avtimer_api.h"
#include "vccm_api.h"
#include "mvm_api_i.h"
/************************Globals and definitions***************************
 **************************************************************************/

static char_t vsm_mvm_dns[] = "qcom.audio.mvm";
uint16_t vsm_mvm_addr;

static char_t vsm_cvp_dns[] = "qcom.audio.cvp";
uint16_t vsm_cvp_addr;

 static char VOICE_THREAD_NAME[]="VSM";
 static const uint32_t THREAD_STACK_SIZE = 16384;
 
 /* public queue name. */
 char VOICE_STREAM_MGR_SVC_CMD_Q_NAME[] = "VSMsysC" ;
 static const uint32_t unSysCmdQMask = 0x00000001L;
 
 /* event queue name and mask. */
 char VOICE_STREAM_MGR_SVC_RESP_Q_NAME[] = "VSMsysE" ;
 static const uint32_t unSysEvtQMask = 0x00000002L;

/* Only support three msgs currently in system cmdQ */
#define VOICE_STREAM_MGR_MAX_OP_CODE_SYS_CMD_Q 16

/*Max number of channel bits.*/
#define VOICE_STREAM_MGR_MAX_NUM_CHANNEL_BITS      32

/* Maximum number of commands expected ever in command queue.*/
static const uint32_t VOICE_STREAM_MGR_MAX_CMD_Q_ELEMENTS = 16;
static const uint32_t VOICE_STREAM_MGR_MAX_RESP_Q_ELEMENTS = 32;
/* Considering Max supported Sessions to be 4, the mask is calculated.
 * bit 0 and 1 are reserved for service commandQ and responseQ.
 * bit 2, 4, 6 and 8 are reserved for session command Queues.
 * bit 3, 5, 7 and 9 are reserved for session response Queues.
 */
static const uint32_t session_cmdQ_mask = 0x00000154;
static const uint32_t session_respQ_mask = 0x000002A8;

/* This mask is used to generate portID by encrypting the session index. */
#define VSM_MAP_SESSION_ID ( 0x0100 )

/**< Queue buffers for maximumm number of sessions. */
static char vsm_session_cmdQ[VSM_MAX_SUPPORTED_SESSION_ID][QURT_ELITE_QUEUE_GET_REQUIRED_BYTES(VOICE_STREAM_MGR_MAX_CMD_Q_ELEMENTS)];
static char vsm_session_respQ[VSM_MAX_SUPPORTED_SESSION_ID][QURT_ELITE_QUEUE_GET_REQUIRED_BYTES(VOICE_STREAM_MGR_MAX_RESP_Q_ELEMENTS)];

elite_apr_handle_t vsm_apr_handle = 0;

//TBD: Make this a VSM struct member
uint32_t vsm_memory_map_client = 0;

// TBD: This actually needs to be a static global. Need to fix this.
voice_stream_mgr_t voice_stream_manager;

/*******************************************************************
 ****************** Function definitions****************************
********************************************************************/

static ADSPResult vsm_create_static_session_state(
  voice_stream_mgr_t *pMe,
  uint8_t session_index
);

int32_t VSM_process_svc_cmdQ( voice_stream_mgr_t* vsmgr );

int32_t VSM_process_sys_respQ( voice_stream_mgr_t* vsmgr );

int32_t VSM_process_session_cmdQ( vsm_session_object_t* session_obj );

int32_t VSM_process_session_respQ( vsm_session_object_t* session_obj );

int32_t vsm_apr_callback_fn( elite_apr_packet_t* packet, void* dispatch_data );

/* Message Handler for unsupported ELITE messages in sys Q */
static ADSPResult VSM_svc_unsupported_handler( voice_stream_mgr_t *pMe );

/* Message Handler to ELITE_CMD_START_SERVICE in sys Q */
static ADSPResult  VSM_svc_start_svc_handler( voice_stream_mgr_t *pMe );

/* Message Handler to ELITE_CMD_DESTROY_SERVICE in sys Q */
static ADSPResult VSM_svc_destroy_svc_handler(voice_stream_mgr_t* pMe);

/* Message Handler to ELITE_APR_PACKET in sys command Q */
static ADSPResult VSM_svc_aprpkt_handler( voice_stream_mgr_t *pMe );

/* Message Handler to ELITE_APR_PACKET in sys resp Q */
static ADSPResult VSM_svc_respQ_aprpkt_handler( voice_stream_mgr_t *pMe);

/* Default Response Handler for ELITE_APR_PACKET in sys resp Q */
static ADSPResult vsm_svc_utility_default_apr_response_handler (
  voice_stream_mgr_t *pMe
);

/*******************************************************************
 ****************** CMD handler tables  ****************************
********************************************************************/

/* Msg handler for system cmdQ. Only support 3 messages currently. */
static VSM_msg_handler_fn VSM_svc_cmdQ_msg_handler_table[ VOICE_STREAM_MGR_MAX_OP_CODE_SYS_CMD_Q ] =
{
   VSM_svc_unsupported_handler,
   VSM_svc_start_svc_handler,      // ELITE_CMD_START_SERVICE
   VSM_svc_destroy_svc_handler,    // ELITE_CMD_DESTROY_SERVICE
   VSM_svc_unsupported_handler,
   VSM_svc_unsupported_handler,
   VSM_svc_unsupported_handler,
   VSM_svc_unsupported_handler,
   VSM_svc_unsupported_handler,
   VSM_svc_unsupported_handler,
   VSM_svc_unsupported_handler,
   VSM_svc_unsupported_handler,
   VSM_svc_unsupported_handler,
   VSM_svc_unsupported_handler,
   VSM_svc_unsupported_handler,
   VSM_svc_unsupported_handler,
   VSM_svc_aprpkt_handler          ///ELITE_APR_PACKET
};

/* Msg handler for system respQ. */
static VSM_msg_handler_fn VSM_svc_respQ_msg_handler_table[ VOICE_STREAM_MGR_MAX_OP_CODE_SYS_CMD_Q ] =
{
   VSM_svc_unsupported_handler,
   VSM_svc_unsupported_handler,
   VSM_svc_unsupported_handler,
   VSM_svc_unsupported_handler,
   VSM_svc_unsupported_handler,
   VSM_svc_unsupported_handler,
   VSM_svc_unsupported_handler,
   VSM_svc_unsupported_handler,
   VSM_svc_unsupported_handler,
   VSM_svc_unsupported_handler,
   VSM_svc_unsupported_handler,
   VSM_svc_unsupported_handler,
   VSM_svc_unsupported_handler,
   VSM_svc_unsupported_handler,
   VSM_svc_unsupported_handler,
   VSM_svc_respQ_aprpkt_handler          ///ELITE_APR_PACKET
};

/* Msg handler for session command Q*/
static VSM_session_cmdQ_handler_fn VSM_session_cmdQ_handler_table[ VOICE_STREAM_MGR_MAX_OP_CODE_SYS_CMD_Q ] =
{
   VSM_session_cmdQ_custom_msg_handler,       ///0 - ELITE_CUSTOM_MSG
   VSM_session_cmdQ_unsupported_msg_handler,  ///1 - UNSUPPORTED CMD
   VSM_session_cmdQ_unsupported_msg_handler,  ///2 - UNSUPPORTED CMD
   VSM_session_cmdQ_unsupported_msg_handler,  ///3 - UNSUPPORTED CMD
   VSM_session_cmdQ_unsupported_msg_handler,  ///4 - UNSUPPORTED CMD
   VSM_session_cmdQ_unsupported_msg_handler,  ///5 - UNSUPPORTED CMD
   VSM_session_cmdQ_unsupported_msg_handler,  ///6 - UNSUPPORTED CMD
   VSM_session_cmdQ_unsupported_msg_handler,  ///7 - UNSUPPORTED CMD
   VSM_session_cmdQ_unsupported_msg_handler,  ///8 - UNSUPPORTED CMD
   VSM_session_cmdQ_unsupported_msg_handler,  ///9 - UNSUPPORTED CMD
   VSM_session_cmdQ_unsupported_msg_handler,  ///A - UNSUPPORTED CMD
   VSM_session_cmdQ_unsupported_msg_handler,  ///B - UNSUPPORTED CMD
   VSM_session_cmdQ_unsupported_msg_handler,  ///C - UNSUPPORTED CMD
   VSM_session_cmdQ_unsupported_msg_handler,  ///D - UNSUPPORTED CMD
   VSM_session_cmdQ_unsupported_msg_handler,  ///E - UNSUPPORTED CMD
   VSM_session_cmdQ_aprpkt_handler            ///F - ELITE_APR_PACKET
};

/* Msg handler for session resp Q*/
static VSM_session_respQ_handler_fn VSM_session_respQ_handler_table[] =
{
  VSM_session_respQ_custom_msg_handler,       ///ELITE_CUSTOM_MSG
  VSM_session_respQ_unsupported_msg_handler,
  VSM_session_respQ_unsupported_msg_handler,
  VSM_session_respQ_default_msg_handler,      ///ELITE_CMD_CONNECT
  VSM_session_respQ_unsupported_msg_handler,
  VSM_session_respQ_default_msg_handler,      ///ELITE_CMD_PAUSE
  VSM_session_respQ_default_msg_handler,      ///ELITE_CMD_RUN
  VSM_session_respQ_unsupported_msg_handler,
  VSM_session_respQ_set_param_msg_handler,    ///ELITE_CMD_SET_PARAM
  VSM_session_respQ_get_param_msg_handler,   ///ELITE_CMD_GET_PARAM
  VSM_session_respQ_unsupported_msg_handler,
  VSM_session_respQ_default_msg_handler,      ///ELITE_DATA_MEDIA_TYPE
  VSM_session_respQ_unsupported_msg_handler,
  VSM_session_respQ_unsupported_msg_handler,
  VSM_session_respQ_unsupported_msg_handler,
  VSM_session_respQ_aprpkt_handler            ///ELITE_APR_PACKET
};

/******************************Utilities **************
*******************************************************/

/** Provides index to handle mapping.

    Forces index to have non-zero port to make valid APR port/handle
    @param [in] sessionIndex Index to convert
    @return Generated APR handle
*/
static inline uint16_t vsm_utility_map_session_index_to_handle( uint16_t session_index )
{
  return ( VSM_MAP_SESSION_ID | session_index );
}

/** Provides handle to index mapping.

    @param [in] handle Index to convert
    @return Generated session index
*/
static inline uint16_t vsm_utility_map_handle_to_session_index( uint16_t handle )
{
  return ( handle & 0x7 );
}

static ADSPResult vsm_utility_verify_handle( elite_apr_port_t port )
{
  /// Check whether the handle is valid.
  uint32_t session_index = 0xFFFF;

  if ( ( port & VSM_MAP_SESSION_ID ) != VSM_MAP_SESSION_ID )
  {
    return ADSP_EHANDLE;
  }

  session_index = vsm_utility_map_handle_to_session_index( port );

  if ( session_index >= VSM_MAX_SUPPORTED_SESSION_ID )
  {
    return ADSP_EHANDLE;
  }
  else if ( NULL == voice_stream_manager.session_table[ session_index ] )
  {
    return ADSP_EHANDLE;    
  }

  return ADSP_EOK;
}

static ADSPResult vsm_utility_set_apr_handle( elite_apr_handle_t handle )
{
   if ( 0 != vsm_apr_handle )
   {
     MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "vsm_utility_set_apr_handle: AprHandle Already Set"
            " vsm_apr_handle: 0x%lx, handle 0x%lx",
           ( uint32_t ) vsm_apr_handle, ( uint32_t ) handle );
     return ADSP_EFAILED;
   }
   vsm_apr_handle = handle;
   return ADSP_EOK;
}

static int32_t vsm_utility_forward_oob_pkt_exchange_event(
  vsm_session_object_t* session_object,
  elite_apr_packet_t *apr_pkt
)
{
  int32_t rc = ADSP_EOK;
  elite_msg_any_t msg;
  qurt_elite_queue_t *svc_cmdQ = NULL;

  if( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  msg.unOpCode = ELITE_APR_PACKET;
  msg.pPayload = ( void* )apr_pkt;

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "vsm_utility_forward_oob_pkt_exchange_event(): "
         " session_portID = 0x%08X, opcode = 0x%08X", apr_pkt->dst_port, apr_pkt->opcode );

  if ( ( session_object->apr_info.client_addr != apr_pkt->src_addr ) ||
       ( session_object->apr_info.client_port != apr_pkt->src_port ) )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "vsm_utility_forward_oob_pkt_exchange_event(): "
           "Client is passive, src_port: 0x%04X.", apr_pkt->src_port );
    rc = elite_apr_if_free( vsm_apr_handle, apr_pkt );
    return ADSP_EOK;
  }

  if ( VSM_STATE_ENUM_RUN != session_object->state_control.state )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "vsm_utility_forward_oob_pkt_exchange_event(): "
           "Vocoder not in RUN state. state:0x%08X.", session_object->state_control.state );
    rc = elite_apr_if_free( vsm_apr_handle, apr_pkt );
    return ADSP_EOK;
  }

  switch ( apr_pkt->opcode )
  {
    case VSS_ITTYOOB_CMD_SEND_RX_CHAR:
    {
      if ( ADSP_FAILED( rc = qurt_elite_queue_push_back( session_object->vdec->cmdQ, ( uint64_t* )&msg ) ) )
      {
        rc = elite_apr_if_free( vsm_apr_handle, apr_pkt );
        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_FATAL,
               "vsm_utility_forward_oob_pkt_exchange_event error = %d, opcode: 0x%08X!!\n",
               rc, apr_pkt->opcode );
      }
    }
    break;

    default:
      rc = elite_apr_if_free( vsm_apr_handle, apr_pkt );
      break;
  }

  return rc;
}

static int32_t vsm_svc_dec_packet_request_event_handler (
  vsm_session_object_t* session_object,
  elite_apr_packet_t* packet
)
{
  uint32_t rc = ADSP_EOK;

  if( NULL == session_object )
  {
    VSM_REPORT_FATAL_ON_ERROR( ADSP_EUNEXPECTED );
    return ADSP_EUNEXPECTED;
  }

  if ( VSS_IPKTEXG_MODE_IN_BAND == session_object->packet_exchange_info.mode )
  {
    packet->dst_addr = session_object->apr_info.client_addr;
    packet->dst_port = session_object->apr_info.client_port;

    rc = __aprv2_cmd_forward( vsm_apr_handle, packet );
    VSM_COMM_ERROR( rc, session_object->apr_info.client_addr );
  }
  else if ( VSS_IPKTEXG_MODE_OUT_OF_BAND == session_object->packet_exchange_info.mode )
  {
    packet->opcode = VSS_IPKTEXG_EVT_OOB_NOTIFY_DEC_BUFFER_REQUEST;
    packet->dst_addr = session_object->apr_info.client_addr;
    packet->dst_port = session_object->apr_info.client_port;

    rc = __aprv2_cmd_forward( vsm_apr_handle, packet );
    VSM_COMM_ERROR( rc, session_object->apr_info.client_addr );
  }
  else /* VSS_IPKTEXG_MODE_MAILBOX == session_object->packet_exchange_info.mode */
  {
    ( void ) vsm_action_rx_mailbox_pktexg( session_object, packet );
    ( void ) elite_apr_if_free( vsm_apr_handle, packet );
  }

  return ADSP_EOK;
}

static int32_t vsm_svc_dec_packet_ready_event_handler (
  vsm_session_object_t* session_object,
  elite_apr_packet_t* packet
)
{
  uint32_t rc = ADSP_EOK;

  if( NULL == session_object )
  {
    VSM_REPORT_FATAL_ON_ERROR( ADSP_EUNEXPECTED );
    return ADSP_EUNEXPECTED;
  }

  if ( VSS_IPKTEXG_MODE_IN_BAND == session_object->packet_exchange_info.mode )
  {
    ( void ) vsm_action_rx_inband_pktexg( session_object, packet );
  }
  else if ( VSS_IPKTEXG_MODE_OUT_OF_BAND == session_object->packet_exchange_info.mode )
  {
    ( void ) vsm_action_rx_oob_pktexg( session_object, packet );
    ( void ) elite_apr_if_free( vsm_apr_handle, packet );
  }

  return ADSP_EOK;
}

static int32_t vsm_svc_enc_packet_ready_event_handler (
  vsm_session_object_t* session_object,
  elite_apr_packet_t* packet
)
{
  int32_t rc = ADSP_EOK;
  vss_imvm_csm_cmd_request_vote_t params;

  if( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  if ( APRV2_PKT_GET_PAYLOAD_BYTE_SIZE( packet->header ) < sizeof( vsm_send_enc_packet_v2_t ) )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "VSM: ENC_PACKET_READY_EVENT: Invalid payload size: %d < %d",
           APRV2_PKT_GET_PAYLOAD_BYTE_SIZE(packet->header),
           sizeof( vsm_send_enc_packet_v2_t ) );
    ( void ) elite_apr_if_free( vsm_apr_handle, packet );
    return APR_EOK;
  }

  if ( VSS_IPKTEXG_MODE_IN_BAND == session_object->packet_exchange_info.mode )
  {
    ( void ) vsm_action_tx_inband_pktexg( session_object, packet );
    ( void ) elite_apr_if_free( vsm_apr_handle, packet );
  }
  else if( VSS_IPKTEXG_MODE_OUT_OF_BAND == session_object->packet_exchange_info.mode )
  {
    ( void ) vsm_action_tx_oob_pktexg( session_object, packet );
    ( void ) elite_apr_if_free( vsm_apr_handle, packet );
  }
  else if ( VSS_IPKTEXG_MODE_MAILBOX == session_object->packet_exchange_info.mode ) 
  {
    ( void ) vsm_action_tx_mailbox_pktexg( session_object, packet );
    ( void ) elite_apr_if_free( vsm_apr_handle, packet );
  }

  params.client = MVM_CSM_CLIENT_ENUM_VENC;
  params.vote = FALSE;
  mvm_call(VSS_IMVM_CSM_CMD_REQUEST_VOTE, &params, sizeof( params ) );

  return ADSP_EOK;
}

int32_t vsm_apr_callback_fn( 
  elite_apr_packet_t* packet,
  void* dispatch_data
)
{
  uint32_t opcode = elite_apr_if_get_opcode( packet );
  elite_apr_port_t port = elite_apr_if_get_dst_port( packet );
  elite_msg_any_t msg;
  uint8_t session_index;
  int32_t result = APR_EOK;
  qurt_elite_queue_t *destQ = NULL;

  msg.unOpCode = ELITE_APR_PACKET;
  msg.pPayload = ( void* ) packet;

  session_index = vsm_utility_map_handle_to_session_index( port );

  if ( session_index >= VSM_MAX_SUPPORTED_SESSION_ID )
  {
    ( void )elite_apr_if_end_cmd( vsm_apr_handle, packet, ADSP_EHANDLE );

    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "vsm_apr_callback_fn: failed"
           " session_index = %d.  port=0x%lX", session_index, ( uint32_t )port );
    return ADSP_EOK;
  }

  /* put different msgs into different queue based on opcode */
  switch ( opcode )
  {
    case VSS_ISTREAM_CMD_CREATE_FULL_CONTROL_SESSION:
    case VSS_ISTREAM_CMD_CREATE_PASSIVE_CONTROL_SESSION:
    case VOICE_CMD_SHARED_MEM_UNMAP_REGIONS:
    case VOICE_CMD_SHARED_MEM_MAP_REGIONS:
    case VSS_ISSR_CMD_CLEANUP:
    {
      if ( ( ADSP_FAILED( result = VSM_apr_router_get_svc_cmdQ( &destQ ) ) ) )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "vsm_apr_callback_fn: fail to get command queue 0x%lx", ( uint32_t ) port );
      }
    }
    break;

    //TBD: Need to change the dynamic services to send this event as Elite message.
    case VSM_EVT_VOC_OPERATING_MODE_UPDATE:
    case VSS_IVOCPROC_RSP_ATTACH_STREAM:
    case VSS_IVOCPROC_RSP_DETACH_STREAM:
    case VSS_ISTREAM_EVT_RX_DTMF_DETECTED:
    case VSS_IVOCPROC_EVT_AFE_PORT_INFO:
    case APRV2_IBASIC_RSP_RESULT:
    case VSS_ICOMMON_RSP_SET_SYSTEM_CONFIG:
    {
      if (port == 0)
      {
        if ( ( ADSP_FAILED( result = VSM_apr_router_get_svc_respQ( &destQ ) ) ) )
        {
          MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
                 "vsm_apr_callback_fn: fail to get response queue 0x%lx", ( uint32_t ) port );
        }
      }
      else if ( ( ADSP_FAILED( result = vsm_utility_verify_handle( port ) ) ) ||
                ( ADSP_FAILED( result = VSM_apr_router_get_respQ( session_index, &destQ ) ) ) )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "vsm_apr_callback_fn: fail to get command queue 0x%lx", ( uint32_t ) port );
      }
    }
    break;

    case VSS_ITTYOOB_CMD_SEND_RX_CHAR:
    {
      if ( ADSP_FAILED(  vsm_utility_verify_handle( port ) ) )
      {
        ( void )elite_apr_if_free( vsm_apr_handle, packet );
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "vsm_apr_callback_fn: invalid port ID 0x%X", port );
      }
      else
      {
        vsm_session_object_t* session_object = voice_stream_manager.session_table[ session_index ];
        ( void ) vsm_utility_forward_oob_pkt_exchange_event( session_object, packet );
      }
      return ADSP_EOK;
    }
    break;

    /** Queue these event to VSM service response queue.*/
    case VSS_IPKTEXG_EVT_OOB_NOTIFY_ENC_BUFFER_CONSUMED:
    case VSS_IPKTEXG_EVT_OOB_NOTIFY_DEC_BUFFER_READY:
    case VSS_IPKTEXG_EVT_IN_BAND_SEND_DEC_BUFFER:
    case VSM_EVT_REQ_DEC_PACKET:
    case VSM_EVT_SEND_ENC_PACKET:
    {
      if ( ADSP_FAILED( result = VSM_apr_router_get_svc_respQ( &destQ ) ) )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "VSM: apr_callback_fn: fail to get response queue rc = %d", result );
      }
    }
    break;

    default:
    {
      if ( ( ADSP_FAILED( result = vsm_utility_verify_handle( port ) ) ) ||
           ( ADSP_FAILED( result = VSM_apr_router_get_cmdQ( session_index, &destQ ) ) ) )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "vsm_apr_callback_fn: fail to get command queue 0x%lx", ( uint32_t ) port );
      }

    }
    break;
  }

  if ( ADSP_FAILED( result ) ) 
  {
    /* Send error message to client: this also frees the packet. */
    uint32_t rc = elite_apr_if_end_cmd( vsm_apr_handle, packet, result );
    if ( ADSP_FAILED( rc ) )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "vsm_apr_callback_fn: "
             "failed at elite_apr_if_end_cmd 0x%lx. Ack not sent. port=0x%lx", rc, ( uint32_t )port );
    }
    /* Returning fail causes another free in APR. therefore return success. */
    return APR_EOK;
  }
 
  if ( ADSP_FAILED( result = qurt_elite_queue_push_back( destQ, ( uint64_t* )( &msg ) ) ) )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "vsm_apr_callback_fn: fail"
           " to push to queue 0x%p, port=0x%lx", destQ, ( uint32_t ) port );
  
    /* Send error message to client: this also frees the packet. */
    uint32_t rc = elite_apr_if_end_cmd( vsm_apr_handle, packet, result );
    if ( ADSP_FAILED( rc ) )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "vsm_apr_callback_fn: failed"
             " at elite_apr_if_end_cmd 0x%lx. Ack not sent. port=0x%lx", rc, ( uint32_t ) port );
    }

    /* Returning fail causes another free in APR. therefore return success. */
    result = APR_EOK;
  }

  return ADSP_EOK;
}

static ADSPResult vsm_utility_get_port_if_session_already_exists(
  voice_stream_mgr_t* vsmgr,
  elite_apr_packet_t* apr_pkt,
  vsm_session_object_t** session_object_ptr,
  bool_t* is_already
)
{
  vss_istream_cmd_create_full_control_session_t* payload = NULL;
  uint32_t payload_size;
  char_t* req_session_name;
  uint32_t req_session_name_size;
  int32_t i;
  int32_t rc = ADSP_EOK;

  *is_already = FALSE;

  if ( VSS_ISTREAM_CMD_CREATE_FULL_CONTROL_SESSION == apr_pkt->opcode )
  {
    payload = APRV2_PKT_GET_PAYLOAD( vss_istream_cmd_create_full_control_session_t, apr_pkt );
    payload_size = APRV2_PKT_GET_PAYLOAD_BYTE_SIZE( apr_pkt->header );

    if ( payload_size < sizeof( vss_istream_cmd_create_full_control_session_t ) )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "vsm_utility_get_port_if_session_already_exists():"
             " Unexpected create full control payload size, %d < %d", payload_size,
             sizeof( vss_istream_cmd_create_full_control_session_t ) );

      return ADSP_EBADPARAM;
    }

    switch ( payload->direction )
    {
      case VSM_DIRECTION_RX:
      case VSM_DIRECTION_TX:
      break;

      case VSM_DIRECTION_RX_TX:
      {
        if ( payload->enc_media_type != payload->dec_media_type )
        {
          rc = ADSP_EBADPARAM;
          break;
        }
      }
      break;

      default:
        rc = ADSP_EBADPARAM;
    }

    if ( ADSP_EBADPARAM == rc )
    {
      MSG_3( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "vsm_utility_get_port_if_session_already_exists:"
             " Invalid params, direction 0x%X, dec_media_id.0x%X, enc_media_id.0x%X..",
             payload->direction, payload->dec_media_type, payload->enc_media_type );
      return rc;
    }

    req_session_name = ( ( ( char_t* ) payload ) +
                         sizeof( vss_istream_cmd_create_full_control_session_t ) );
    req_session_name_size = ( payload_size -
                            sizeof( vss_istream_cmd_create_full_control_session_t ) );
  }
  else
  {
    req_session_name = APRV2_PKT_GET_PAYLOAD( char_t, apr_pkt );;
    req_session_name_size = APRV2_PKT_GET_PAYLOAD_BYTE_SIZE( apr_pkt->header );
  }

  if ( req_session_name_size > VSM_MAX_SESSION_NAME_SIZE )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "vsm_utility_get_port_if_session_already_exists:"
           " Invalid params, req_session_name_size %d", req_session_name_size );
    rc = ADSP_EBADPARAM;
    return rc;
  }

  for( i = 0; i < VSM_MAX_SUPPORTED_SESSION_ID; i++ )
  {
    if ( NULL != vsmgr->session_table[i] )
    {
      if ( mmstd_strncmp( vsmgr->session_table[i]->session_name,
                          sizeof( vsmgr->session_table[i]->session_name ),
                          req_session_name,
                          req_session_name_size )
             == 0 )
      {
        *is_already = TRUE;
        break;
      }
    }
  }

  if ( TRUE == *is_already )
  {
    *session_object_ptr = vsmgr->session_table[i];
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "vsm_utility_get_port_if_session_already_exists:.TRUE..port: 0x%X", (*session_object_ptr)->apr_info.self_port );
  }

  return ADSP_EOK;
}

static uint32_t vsm_utility_get_free_session_index( voice_stream_mgr_t* vsmgr )
{
  uint32_t session_index;

  for ( session_index = 0 ; session_index < VSM_MAX_SUPPORTED_SESSION_ID ; session_index++ )
  {
    if ( NULL == vsmgr->session_table[ session_index ] )
    {
      break;
    }
  }

  return session_index;

}

/*************************Service level Command and Response handlers **************************
***********************************************************************************************/

static int32_t VSM_svc_shared_mem_map_regions_cmd_control( elite_apr_packet_t* apr_pkt )
{
  ADSPResult rc = ADSP_EOK;

  if( NULL == apr_pkt )
  {
    return ADSP_EBADPARAM;
  }
  MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,
       "VSM_svc_shared_mem_map_regions_cmd_control() received" );

  rc = voice_shared_memory_map_regions_process( vsm_memory_map_client, vsm_apr_handle, apr_pkt);

  return rc;
}

static int32_t VSM_svc_shared_mem_unmap_regions_cmd_control( elite_apr_packet_t* apr_pkt )
{
  ADSPResult rc = ADSP_EOK;

  if( NULL == apr_pkt )
  {
    return ADSP_EBADPARAM;
  }
  MSG( MSG_SSID_DFLT, MSG_LEGACY_MED, 
       "VSM_svc_shared_mem_unmap_regions_cmd_control() received" );

  rc = voice_shared_memory_un_map_regions_process( vsm_memory_map_client, vsm_apr_handle, apr_pkt);

  return rc;
}

static int32_t VSM_svc_ssr_cmd_control(
  voice_stream_mgr_t *pMe,
  elite_apr_packet_t* apr_pkt
)
{
  ADSPResult rc = ADSP_EOK;
  int32_t session_index =0;
  int32_t payload_size = 0;
  vsm_session_object_t   *session_obj = NULL;
  vss_issr_cmd_cleanup_t *in_args;
  elite_apr_packet_t     *p_dst_apr_packet;
  elite_msg_any_t         msg_pkt;
  void *apr_payload = NULL;

  //command control
  pMe->cmd_control.issued_count = 0;
  pMe->cmd_control.resp_count = 0;
  pMe->cmd_control.resp_result = 0xFFFFFFFF;

  /* Get payload size */
  payload_size = elite_apr_if_get_payload_size(apr_pkt);

  if ( payload_size != sizeof ( vss_issr_cmd_cleanup_t ) )
  {
    MSG_2( MSG_SSID_DFLT, DBG_ERROR_PRIO,
           "VSM_svc_ssr_cmd_control(): Unexpected payload size for VSS_ISSR_CMD_CLEANUP, "
           "%d != expected: %d", payload_size,
           sizeof( vss_issr_cmd_cleanup_t ) );
    /* End APR command */
    rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EBADPARAM );
    return ADSP_EOK;
  }

  in_args = (vss_issr_cmd_cleanup_t *)elite_apr_if_get_payload_ptr( apr_pkt );

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED, "VSM_svc_ssr_cmd_control(): SSR Domin ID = 0x%08X", 
         in_args->domain_id );

  while ( VSM_MAX_SUPPORTED_SESSION_ID > session_index )
  {
    //Find the Session which needs to be cleaned
    session_obj = pMe->session_table[session_index];
    if ( NULL == session_obj )
    {
      session_index++;
      continue;
    }

    if ( ( APR_GET_FIELD( APRV2_PKT_DOMAIN_ID, session_obj->apr_info.client_addr )  == in_args->domain_id ) || 
         ( VSM_utility_ssr_matches_passive_clients_domain ( in_args->domain_id, session_obj ) ) )
    {
      if ( APR_EOK != ( rc = elite_apr_if_alloc_cmd( vsm_apr_handle,
                               session_obj->apr_info.self_addr,
                               0, session_obj->apr_info.self_addr,
                               session_obj->apr_info.self_port,
                               session_index, VSS_ISSR_CMD_CLEANUP,
                               sizeof( vss_issr_cmd_cleanup_t ),
                               &p_dst_apr_packet ) ) )
      {
        MSG_1( MSG_SSID_DFLT, DBG_ERROR_PRIO,
                "VSM_svc_ssr_cmd_control(): Failed to allocate APR packet result; 0x%lx", rc );
                /* Move onto next session */
                session_index++;
        continue;
      }

      /* Populate elite msg payload */
      msg_pkt.unOpCode = ELITE_APR_PACKET;
      msg_pkt.pPayload = (void *)p_dst_apr_packet;
      apr_payload = ( void* )( elite_apr_if_get_payload_ptr( p_dst_apr_packet ) );
      memscpy( apr_payload, sizeof( vss_issr_cmd_cleanup_t ), in_args, sizeof( vss_issr_cmd_cleanup_t ) );

      if ( ADSP_FAILED( rc = qurt_elite_queue_push_back( ( session_obj->cmd_control.cmdQ ), ( uint64_t* )( &msg_pkt ) ) ) )
      {
        MSG_1( MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "VSM_svc_ssr_cmd_control():Failed to push to session cmdQ, session_id: 0x%lx", 
               session_obj->apr_info.self_port );
        elite_apr_if_free(vsm_apr_handle, p_dst_apr_packet );
        /* Move onto next session */
        session_index++;
        continue;
      }
      pMe->cmd_control.issued_count++;
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW, "VSM_svc_ssr_cmd_control(): command issued count: "
             "0x%d, session_index = %d", pMe->cmd_control.issued_count, session_index );
    }
    session_index++;
  }
  if ( 0 != pMe->cmd_control.issued_count )
  {
    pMe->cmd_control.is_resp_pending = TRUE;
  }
  else
  {
    rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EOK );
  }

  return rc;
}

/* This is to process SVC command. */
ADSPResult VSM_svc_aprpkt_handler( voice_stream_mgr_t *pMe )
{

  ADSPResult rc = ADSP_EOK;
  elite_apr_packet_t *apr_pkt = ( elite_apr_packet_t * )( pMe->msg.pPayload );

  uint32_t opcode = elite_apr_if_get_opcode( apr_pkt );

  switch ( opcode )
  {
    case VSS_ISTREAM_CMD_CREATE_FULL_CONTROL_SESSION:
    case VSS_ISTREAM_CMD_CREATE_PASSIVE_CONTROL_SESSION:
    {
      bool_t is_already;
      uint32_t session_index;
      vsm_session_object_t* session_object;

      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED, "VSM_svc_aprpkt_handler:"
             "received CREATE session command. opcode: 0x%08X", opcode );

      rc = vsm_utility_get_port_if_session_already_exists( pMe, apr_pkt , &session_object, &is_already );

      if ( ADSP_FAILED( rc ) )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_svc_aprpkt_handler: failed with error: 0x%X", rc );
        rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, rc );
        break;
      }

      /* Return if already existing. */
      if ( is_already )
      {
        if ( VSS_ISTREAM_CMD_CREATE_PASSIVE_CONTROL_SESSION == opcode )
        {
          if ( TRUE == VSM_utility_if_passive_client_already_exists( apr_pkt, session_object ) )
          {
            rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EALREADY );
          }
          else
          {
            VSM_utility_update_passive_client_info( apr_pkt, session_object );
            session_object->ref_count++;
            apr_pkt->dst_port = session_object->apr_info.self_port;
            rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EOK );
          }
          break;
        }
        else if ( 0 != session_object->apr_info.client_addr )
        {
          rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EALREADY );
          break;
        }
      }
      else
      {
        /* Check if resources available. */
        session_index = vsm_utility_get_free_session_index( pMe );

        if ( session_index >= VSM_MAX_SUPPORTED_SESSION_ID )
        {
          MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_svc_aprpkt_handler: reached max num sesions.." );
          rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_ENORESOURCE );
          break;
        }

        /* if avaible, allocate memory and.push to the session cmd queue.*/
        session_object = ( vsm_session_object_t* )qurt_elite_memory_malloc(
                                                sizeof( vsm_session_object_t ), QURT_ELITE_HEAP_DEFAULT );

        if ( NULL == session_object )
        {
          MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
                 "VSM_svc_aprpkt_handler: failed to allocate memory for vsm session..." );
          rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_ENORESOURCE );
          break;
        }

        /* Initiate memory. */
        memset( session_object,0xBC,sizeof( vsm_session_object_t ) );

        session_object->ref_count = 0;
        session_object->apr_info.apr_handle = vsm_apr_handle;
        session_object->apr_info.self_port = vsm_utility_map_session_index_to_handle( session_index );
        session_object->apr_info.self_addr = apr_pkt->dst_addr;
        session_object->cmd_control.cmdQ = pMe->static_info[session_index].cmdQ;
        session_object->cmd_control.respQ = pMe->static_info[session_index].respQ;

        pMe->session_table[ session_index ] = session_object;
      }
      if ( ADSP_FAILED( rc = qurt_elite_queue_push_back( ( session_object->cmd_control.cmdQ ), ( uint64_t* )( &pMe->msg ) ) ) )
      {
        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_svc_aprpkt_handler:"
               " fail to push to queue 0x%p result 0x%X", session_object->cmd_control.cmdQ, rc );

        /* send error message to client: this also frees the packet.*/
        rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, rc );
        break;
      }
    }
    break; 

    case VOICE_CMD_SHARED_MEM_MAP_REGIONS:
    {
      rc = VSM_svc_shared_mem_map_regions_cmd_control( apr_pkt );
    }
    break;

    case VOICE_CMD_SHARED_MEM_UNMAP_REGIONS:
    {
      rc = VSM_svc_shared_mem_unmap_regions_cmd_control( apr_pkt );
    }
    break;

    case VSS_ISSR_CMD_CLEANUP:
    {
      rc = VSM_svc_ssr_cmd_control( pMe, apr_pkt );
    }
    break;

    default:
    {
      /* send error message to client: this also frees the packet. */
      rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EUNSUPPORTED );
    }
    break;

  }

  if ( ADSP_FAILED( rc ) )
  {
    MSG_3( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_svc_aprpkt_handler: failed with error:"
           " 0x%X  opcode 0x%X. port=0x%X", rc, opcode, apr_pkt->dst_port );
  }

  return ADSP_EOK;
}

static ADSPResult vsm_svc_utility_default_apr_response_handler (
  voice_stream_mgr_t *pMe
)
{
  ADSPResult rc = ADSP_EOK;

  if ( NULL == pMe) return ADSP_EBADPARAM;

  elite_apr_packet_t *apr_pkt = ( elite_apr_packet_t * )( pMe->cmd_control.resp_msg.pPayload );

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "vsm_svc_utility_default_apr_response_handler: received response token 0x%X  result 0x%X",
         apr_pkt->token, APRV2_PKT_GET_PAYLOAD( aprv2_ibasic_rsp_result_t, apr_pkt )->status );

  if ( TRUE == pMe->cmd_control.is_resp_pending )
  {
    pMe->cmd_control.resp_count++;
  }

  if ( pMe->cmd_control.resp_result == 0xFFFFFFFF )
  {
    pMe->cmd_control.resp_result = APRV2_PKT_GET_PAYLOAD( aprv2_ibasic_rsp_result_t, apr_pkt )->status;
  }

  if ( ADSP_FAILED( APRV2_PKT_GET_PAYLOAD( aprv2_ibasic_rsp_result_t, apr_pkt )->status ) )
  {
    pMe->cmd_control.resp_result = APRV2_PKT_GET_PAYLOAD( aprv2_ibasic_rsp_result_t, apr_pkt )->status;
  }

  if ( pMe->cmd_control.resp_count == pMe->cmd_control.issued_count )
  {
    pMe->cmd_control.is_resp_pending = FALSE;
    //Send the response back to the APR command received
    elite_apr_if_end_cmd(vsm_apr_handle, ( elite_apr_packet_t * )pMe->msg.pPayload, pMe->cmd_control.resp_result);
  }
  elite_apr_if_free( vsm_apr_handle, apr_pkt );

  return rc;
}

ADSPResult VSM_svc_respQ_aprpkt_handler( voice_stream_mgr_t *pMe )
{
  ADSPResult rc = ADSP_EOK;
  uint8_t session_index;

  if ( NULL == pMe )
  {
    return ADSP_EBADPARAM;
  }

  elite_apr_packet_t *apr_pkt = ( elite_apr_packet_t * )( pMe->cmd_control.resp_msg.pPayload );

  uint32_t opcode = elite_apr_if_get_opcode( apr_pkt );

  switch ( opcode )
  {
    case APRV2_IBASIC_RSP_RESULT:
    {
      rc = vsm_svc_utility_default_apr_response_handler( pMe );
    }
    break;

    case VSM_EVT_REQ_DEC_PACKET:
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_LOW, "VSM: VSM_EVT_REQ_DEC_PACKET" );
      elite_apr_port_t port = elite_apr_if_get_dst_port( apr_pkt );
      rc = vsm_utility_verify_handle( port );
      if ( ADSP_EOK == rc )
      {
        uint8_t session_index = vsm_utility_map_handle_to_session_index( port );
        vsm_session_object_t* session_object = voice_stream_manager.session_table[ session_index ];
        ( void ) vsm_svc_dec_packet_request_event_handler( session_object, apr_pkt );
      }
    }
    break;

    case VSS_IPKTEXG_EVT_OOB_NOTIFY_DEC_BUFFER_READY:
    case VSS_IPKTEXG_EVT_IN_BAND_SEND_DEC_BUFFER:
    {
      elite_apr_port_t port = elite_apr_if_get_dst_port( apr_pkt );
      rc = vsm_utility_verify_handle( port );
      if ( ADSP_EOK == rc )
      {
        uint8_t session_index = vsm_utility_map_handle_to_session_index( port );
        vsm_session_object_t* session_object = voice_stream_manager.session_table[ session_index ];
        ( void ) vsm_svc_dec_packet_ready_event_handler( session_object, apr_pkt );
      }
    }
    break;

    case VSS_IPKTEXG_EVT_OOB_NOTIFY_ENC_BUFFER_CONSUMED:
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,
             "VSM: IPKTEXG_EVT_OOB_ENC_BUFFER_CONSUMED port=0x%X", apr_pkt->dst_port );
      elite_apr_port_t port = elite_apr_if_get_dst_port( apr_pkt );
      elite_apr_if_free( vsm_apr_handle, apr_pkt ); 
      rc = vsm_utility_verify_handle( port );
      if ( ADSP_EOK == rc )
      {
        uint8_t session_index = vsm_utility_map_handle_to_session_index( port );
        vsm_session_object_t* session_object = voice_stream_manager.session_table[ session_index ];
        session_object->packet_exchange_info.oob_info.is_enc_buf_consumed = TRUE;
      }
    }
    break;

    case VSM_EVT_SEND_ENC_PACKET:
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_LOW, "VSM: VSM_EVT_SEND_ENC_PACKET" );
      elite_apr_port_t port = elite_apr_if_get_dst_port( apr_pkt );
      rc = vsm_utility_verify_handle( port );
      if ( ADSP_EOK == rc )
      {
        uint8_t session_index = vsm_utility_map_handle_to_session_index( port );
        vsm_session_object_t* session_object = voice_stream_manager.session_table[ session_index ];
        vsm_svc_enc_packet_ready_event_handler( session_object, apr_pkt );
      }
    }
    break;

    default:
    {
      rc = ADSP_EUNSUPPORTED;
    }
    break;

  }

  if ( ADSP_FAILED( rc ) )
  {
    MSG_3( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "VSM: svc_respQ_aprpkt_handler: Command failed opcode 0x%8X, "
           "port 0x%8X, error %d", opcode, apr_pkt->dst_port, rc );
    elite_apr_if_free( vsm_apr_handle, apr_pkt ); 
  }

  return ADSP_EOK;
}

/************************Worker thread and High level MSG handlers*****************************
 *********************************************************************************************/

int32_t VSM_process_session_respQ( vsm_session_object_t* session_object )
{
  ADSPResult result = ADSP_EOK;
  elite_msg_any_t msg;

  if ( session_object == NULL )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_process_session_respQ: NULL Session pointer..." );
    return ADSP_EBADPARAM;
  }

  result = qurt_elite_queue_pop_front( session_object->cmd_control.respQ, (uint64_t* )&msg );
  if( ADSP_EOK != result )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_process_session_respQ: popQ failed..%d", result );
    return result;
  }

  session_object->cmd_control.resp_msg = msg;

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
         "VSM:(0x%X) received response/event opcode:(0x%lx)",
         session_object->apr_info.self_port, msg.unOpCode );

  const uint32_t sessionRespTableSize = sizeof( VSM_session_respQ_handler_table )/sizeof( VSM_session_respQ_handler_table[0] );
  if ( msg.unOpCode >= sessionRespTableSize )
  {
    return VSM_session_respQ_default_msg_handler( session_object );
  }

  return VSM_session_respQ_handler_table[ msg.unOpCode ]( session_object );

}

int32_t VSM_process_session_cmdQ( vsm_session_object_t* session_object )
{
   uint32_t result = ADSP_EOK;
   elite_apr_packet_t* apr_pkt;

  if ( session_object == NULL )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_process_session_cmdQ: NULL Session pointer..." );
    return ADSP_EBADPARAM;
  }

  if( TRUE != session_object->cmd_control.is_cmd_pending )
  {
    result = qurt_elite_queue_pop_front( session_object->cmd_control.cmdQ,
                                         ( uint64_t* )&( session_object->cmd_control.cmd_msg ) );
    if( ADSP_EOK != result )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_process_session_cmdQ: popQ failed..%d", result );
      return result;
    }
 
    if ( session_object->cmd_control.cmd_msg.unOpCode >= VOICE_STREAM_MGR_MAX_OP_CODE_SYS_CMD_Q )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_process_session_cmdQ:"
             " unsupported Elite msg..0x%X", session_object->cmd_control.cmd_msg.unOpCode );
      elite_msg_finish_msg( &session_object->cmd_control.cmd_msg, ADSP_EUNSUPPORTED );
      return ADSP_EUNSUPPORTED;
    }
    session_object->cmd_control.resp_result = ADSP_EOK;
    session_object->cmd_control.current_action = VSM_ACTION_UNINITIALIZED;
  }
  else if ( TRUE == session_object->cmd_control.is_resp_pending )
  {
    return ADSP_EOK;
  }

  VSM_session_cmdQ_handler_table[session_object->cmd_control.cmd_msg.unOpCode]( session_object ) ;

  return ADSP_EOK;
}

static ADSPResult VSM_svc_unsupported_handler( voice_stream_mgr_t *pMe )
{
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,"VSM_svc_unsupported_handler: unsupported msg 0x%X", pMe->msg.unOpCode );
  elite_msg_finish_msg( & ( pMe->msg ), ADSP_EUNSUPPORTED );
  return ADSP_EOK;
}

int32_t VSM_process_sys_respQ( voice_stream_mgr_t* vsmgr )
{
  ADSPResult result = ADSP_EOK;
  
  if ( vsmgr == NULL )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_process_sys_respQ: NULL vsmgr pointer..." );
    return ADSP_EBADPARAM;
  }

  result = qurt_elite_queue_pop_front( vsmgr->respQ, ( uint64_t* )&( vsmgr->cmd_control.resp_msg ) );

  if( ADSP_EOK != result )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VoiceStreamMgr_ProcessSvcRespQ: failed with error: 0x%X ", result );
    return result;
  }

  if ( vsmgr->cmd_control.resp_msg.unOpCode >= VOICE_STREAM_MGR_MAX_OP_CODE_SYS_CMD_Q )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_process_sys_respQ: unsupported msgID: 0x%X ", vsmgr->cmd_control.resp_msg.unOpCode );
    elite_msg_finish_msg( &vsmgr->msg, ADSP_EUNSUPPORTED );
    return ADSP_EUNSUPPORTED;
  }

  return VSM_svc_respQ_msg_handler_table[vsmgr->cmd_control.resp_msg.unOpCode]( vsmgr ) ;
}

int32_t VSM_process_svc_cmdQ( voice_stream_mgr_t* vsmgr )
{
  ADSPResult result = ADSP_EOK;
  uint32_t    session_index;

  for( session_index = 0; session_index < VSM_MAX_SUPPORTED_SESSION_ID; session_index++ )
  {
    if ( NULL == vsmgr->session_table[session_index] )
       continue;
   
    if( TRUE == vsmgr->session_table[session_index]->cmd_control.is_destroy_cmd_pending )
       return ADSP_EOK;
  }

  if( TRUE != vsmgr->cmd_control.is_cmd_pending )
  {
    result = qurt_elite_queue_pop_front( vsmgr->service_handle.cmdQ, ( uint64_t* )&( vsmgr->msg ) );
    if( ADSP_EOK != result )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_process_svc_cmdQ: failed with error: 0x%X ", result );
      return result;
    }

    if ( vsmgr->msg.unOpCode >= VOICE_STREAM_MGR_MAX_OP_CODE_SYS_CMD_Q )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_process_svc_cmdQ: unsupported msgID: 0x%X ", vsmgr->msg.unOpCode );
      elite_msg_finish_msg( &vsmgr->msg, ADSP_EUNSUPPORTED );
      return ADSP_EUNSUPPORTED;
    }
  }
  else if ( TRUE == vsmgr->cmd_control.is_resp_pending )
  {
    return ADSP_EOK;
  }

  return VSM_svc_cmdQ_msg_handler_table[vsmgr->msg.unOpCode]( vsmgr ) ;
}

static void vsm_evaluate_sleep_possibility( voice_stream_mgr_t *pMe )
{
   uint32_t                session_idx;
   vsm_session_object_t    *p_session = NULL;
   vss_imvm_csm_cmd_request_vote_t params;
   bool_t sleep = TRUE;

   for ( session_idx = 0; session_idx < VSM_MAX_SUPPORTED_SESSION_ID; session_idx++ ) 
   {
      p_session = pMe->session_table[session_idx];
      if ( NULL == p_session )
      {
         continue;
      }
      else if ( p_session->cmd_control.is_cmd_pending || p_session->cmd_control.is_resp_pending )
      {
         sleep = FALSE;
      }
   }
   if ( ( TRUE == pMe->vsm_csm_vote )  && ( TRUE == sleep ) )
   {
     params.client = MVM_CSM_CLIENT_ENUM_VSM;
     params.vote = FALSE;
     mvm_call(VSS_IMVM_CSM_CMD_REQUEST_VOTE, &params, sizeof( params ) );
     pMe->vsm_csm_vote = FALSE;
   }
}

static ADSPResult vsm_process_channels( voice_stream_mgr_t *pMe, uint32_t channel_status )
{
   int32_t end_bit, mask;
   ADSPResult result=ADSP_EOK;

   if( !channel_status )
   {
     return ADSP_EOK;
   }

   /* Process each ready channel once. */
   end_bit   = VOICE_STREAM_MGR_MAX_NUM_CHANNEL_BITS - s32_cl0_s32( channel_status );
   mask = 0x1;

   if ( channel_status & unSysCmdQMask )
   {
     result = VSM_process_svc_cmdQ( pMe );
     if ( ADSP_ETERMINATED == result )
     {
       return result;
     }
   }
   else if ( channel_status & unSysEvtQMask )
   {
     result = VSM_process_sys_respQ( pMe );
   }

   for ( int32_t i = 3; i < end_bit; i += 2 )
   {
     uint8_t session_index = ( ( i / 2 ) -1 );

     if ( session_index >= VSM_MAX_SUPPORTED_SESSION_ID )
     {
       break;
     }

     mask = 0x1 << i;

     if ( channel_status & mask )
     {
       VSM_process_session_respQ( pMe->session_table[session_index] );
     }
   }

   for ( int32_t i = 2; i < end_bit; i += 2 )
   {
     uint8_t session_index = ( ( i / 2 ) -1 );

     if ( session_index >= VSM_MAX_SUPPORTED_SESSION_ID )
     {
       break;
     }

     mask = 0x1 << i;

     if ( channel_status & mask )
     {
       VSM_process_session_cmdQ( pMe->session_table[session_index] );
       if ( NULL != pMe->session_table[session_index] )
       {
         if ( TRUE == pMe->session_table[session_index]->cmd_control.is_cmd_pending )
         {
           /* Remember that a command is pending for this session. */
           pMe->pending_mask |= mask;
         }
         else
         {
           /* No pending command for this session. */
           pMe->pending_mask &= ~mask;
         }
       }
     }
   }

   return result;
}
  
 
 /* This function is the main work loop for the service.
 */
static int vsm_workloop(void* pInstance)
{
   voice_stream_mgr_t *pMe = ( voice_stream_mgr_t* )pInstance; // instance structure
   ADSPResult result = ADSP_EOK;
   vss_imvm_csm_cmd_request_vote_t params;
   /* set up mask for listening to the msg queues. */

   pMe->pending_mask = 0;

   pMe->wait_mask |= unSysCmdQMask ;
   pMe->wait_mask |= unSysEvtQMask ;

   for( uint32_t i = 0; i <= VSM_MAX_SUPPORTED_SESSION_ID; i++ )
   {
     pMe->wait_mask |= ( 1 << ( (2 * i) + 2 ) );  // for session cmdQ
     pMe->wait_mask |= ( 1 << ( (2 * i) + 3 ) );     //for session respQ
   }

   MSG(MSG_SSID_DFLT, MSG_LEGACY_HIGH, "Entering VSM workloop...");

   for( ; ; )
   {
     pMe->recvd_mask = qurt_elite_channel_wait( &(pMe->channel),
                                                ( pMe->wait_mask & ( ~pMe->pending_mask ) ) );


     if ( FALSE == pMe->vsm_csm_vote )
     {
       params.client = MVM_CSM_CLIENT_ENUM_VSM;
       params.vote = TRUE;
       mvm_call(VSS_IMVM_CSM_CMD_REQUEST_VOTE, &params, sizeof( params ) );
       pMe->vsm_csm_vote = TRUE;
     }
     /*. Exhaust SVC command queue and all response queues first. */
     result = vsm_process_channels( pMe, ( pMe->recvd_mask & 0xAAAB ) );
     if( ADSP_ETERMINATED == result )
     {
       return ADSP_EOK;
     }

     //process command queues now
     pMe->recvd_mask = qurt_elite_channel_poll( &( pMe->channel ), 0x5555 );

     result = vsm_process_channels( pMe, ( pMe->recvd_mask | pMe->pending_mask ) );
     vsm_evaluate_sleep_possibility( pMe );
   }

   return result;
}

/************************Create, Start and Destroy Service handlers***************************
 *********************************************************************************************/

static void vsm_destroy( voice_stream_mgr_t* pMe )
{
  int i;

  ( void )elite_apr_if_deregister( vsm_apr_handle );
  
  ( void )vccm_mmpm_deregister( VCCM_CLIENT_ID_CVS );

  vsm_apr_handle = 0;

  VoiceStreamMgr_AprRouterDeinit();

  for (i = 0; i < VSM_MAX_SUPPORTED_SESSION_ID ; i ++ )
  {
/*  Shall we free this memory allocated for sessions?
    Clients actually shall free this.
    If clients wont do this, retaining the pointers may help debugging?

    if ( NULL != pMe->session_table[i] )
    {
      qurt_elite_memory_free( ( void* )pMe->session_table[i] );
    }
*/
    elite_svc_deinit_cmd_queue( pMe->static_info[ i ].cmdQ );
    elite_svc_deinit_cmd_queue( pMe->static_info[ i ].respQ );
  }
  elite_svc_deinit_cmd_queue ( pMe->service_handle.cmdQ );
  elite_svc_deinit_cmd_queue ( pMe->respQ );

  qurt_elite_channel_destroy(&pMe->channel);
}

/* Message handler ELITE_CMD_DESTROY_SERVICE */
static ADSPResult VSM_svc_destroy_svc_handler(voice_stream_mgr_t* pMe)
{
   MSG( MSG_SSID_DFLT, MSG_LEGACY_MED, "VoiceStreamMgr: DestroySvc  Enter" );

   /**Make a copy of msg because on destroy, pMe is freed */
   elite_msg_any_t msg = pMe->msg;

   vsm_destroy( pMe );

   elite_msg_finish_msg( &msg, ADSP_EOK );

   MSG( MSG_SSID_DFLT, MSG_LEGACY_MED, "VoiceStreamMgr: DestroySvc  Leave" );

   /* Send ADSP_ETERMINATED so calling routine knows the destroyer has been invoked. */
   return ADSP_ETERMINATED;
}

/* Message Handler to ELITE_CMD_START_SERVICE in sys Q */
static ADSPResult  VSM_svc_start_svc_handler( voice_stream_mgr_t* pMe )
{
  ADSPResult result = ADSP_EOK;
  elite_apr_handle_t handle = 0;
  char vsm_name[] = "qcom.audio.cvs";
  uint32_t vsm_name_size = strlen( vsm_name );
  uint16_t ret_addr = 0;

  MSG( MSG_SSID_DFLT, MSG_LEGACY_MED, "VSM_svc_start_svc_handler: StartSvc  Enter" );

  result = elite_apr_if_register_by_name( &handle, &ret_addr, vsm_name, vsm_name_size, vsm_apr_callback_fn, NULL );
  if ( ADSP_FAILED( result ) ) 
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED, "VSM_svc_start_svc_handler: Failed to register VSM  0x%8X", result );
    return result;
  }

  /* Save APR handle */
  if ( ADSP_FAILED( result = vsm_utility_set_apr_handle( handle ) ) )
  {
    MSG_1(MSG_SSID_DFLT, MSG_LEGACY_MED, "VSM_svc_start_svc_handler: Fail to set handle 0x%8x", result );
  }

  elite_msg_finish_msg( &( pMe->msg ), ADSP_EOK );

  MSG( MSG_SSID_DFLT, MSG_LEGACY_MED, "VSM_svc_start_svc_handler: StartSvc  Leave" );

  return result;
}

/**
 * \brief This function creates the necessary session state
 * structure given the internal session ID that it needs to
 * create. Currently only supports up to 4 sessions.
 *
 */
static ADSPResult vsm_create_static_session_state(voice_stream_mgr_t *pMe, uint8_t session_index )
{
  ADSPResult result = ADSP_EOK;
  vsm_session_static_info_t *session_info = &( pMe->static_info[ session_index ] );
  int start_bit; 
  qurt_elite_queue_t *temp_cmdQ, *temp_respQ;

  QURT_ELITE_ASSERT( session_index < VSM_MAX_SUPPORTED_SESSION_ID );

  /* Create cmd  and resp queue */
  char cmdQ_name[QURT_ELITE_DEFAULT_NAME_LEN], respQ_name[QURT_ELITE_DEFAULT_NAME_LEN];
  snprintf( cmdQ_name, QURT_ELITE_DEFAULT_NAME_LEN, "VSMsc%d", session_index );
  snprintf( respQ_name, QURT_ELITE_DEFAULT_NAME_LEN, "VSMsr%d", session_index );

  temp_cmdQ = ( qurt_elite_queue_t* )&vsm_session_cmdQ[ session_index ];
  temp_respQ = ( qurt_elite_queue_t* )&vsm_session_respQ[ session_index ];

  /* Cmd and resp queue init for each VSM session. */

  if ( ADSP_FAILED( result = qurt_elite_queue_init( cmdQ_name, VOICE_STREAM_MGR_MAX_CMD_Q_ELEMENTS, temp_cmdQ ) ) ||
       ADSP_FAILED( result = qurt_elite_queue_init( respQ_name, VOICE_STREAM_MGR_MAX_RESP_Q_ELEMENTS, temp_respQ ) ) )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_FATAL, "Failed to create VoiceStreamMgr session cmd and resp queues!" );
    return result;
  }

  session_info->cmdQ = temp_cmdQ;
  session_info->respQ = temp_respQ;

  /* Calcuate the bit mask for this session's command queue.
  The system reserves bit 0 and 1 of the channel. Session X
  cmd queue shall take bit 2*x+2 and rep queue takes bit
  2*x+3 */
  start_bit = ( 2 * session_index ) + 2;
  session_info->session_cmdQ_mask   = 1 << start_bit;

  /* Add queue to channel */
  if ( ADSP_FAILED( result = qurt_elite_channel_addq( &pMe->channel, session_info->cmdQ,
         session_info->session_cmdQ_mask ) )||
       ADSP_FAILED( result = qurt_elite_channel_addq(&pMe->channel, session_info->respQ,
         session_info->session_cmdQ_mask << 1 ) )
  )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_FATAL, "ADSP_FAILED to add mqs to channel result = %d!", result );
    return result;
  }

  /* Immediately switch on channel */
  pMe->wait_mask |= session_info->session_cmdQ_mask;

  pMe->session_table[ session_index ] = NULL;

  return ADSP_EOK;
}


/* This function creates all sessions state machine here. */
ADSPResult vsm_create_all_session_states( voice_stream_mgr_t *pMe )
{
  int i;

  for (i = 0; i < VSM_MAX_SUPPORTED_SESSION_ID ; i ++ )
  {
    if ( ADSP_FAILED( vsm_create_static_session_state( pMe, i ) ))
    {         
      MSG_1(MSG_SSID_DFLT, MSG_LEGACY_FATAL, "Failed to create VoiceStreamMgr session %d!", i);
      return ADSP_EFAILED; 
    }
  }

  return ADSP_EOK; 
}

ADSPResult VoiceStreamMgr_Create ( uint32_t inputParam, void **handle )
{
   int result = ADSP_EOK;
   qurt_elite_queue_t  *pQTemp;
   voice_stream_mgr_t *pMe;

   MSG( MSG_SSID_DFLT, MSG_LEGACY_MED, "VoiceStreamMgr: Creating" );

   memset( &voice_stream_manager,0,sizeof( voice_stream_mgr_t ) );

   pMe = ( voice_stream_mgr_t* )&voice_stream_manager;

   /* Create global cmd queue. */
   pQTemp = ( qurt_elite_queue_t* )pMe->cmdQ_buf;

   if ( ADSP_FAILED( result = qurt_elite_queue_init( VOICE_STREAM_MGR_SVC_CMD_Q_NAME,
                      VOICE_STREAM_MGR_MAX_SVC_CMD_Q_ELEMENTS, pQTemp ) ) )
   {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_FATAL, "Failed to create VoiceStreamMgr service message queues!" );
      return result;
   }
   pMe->service_handle.cmdQ = pQTemp;

   /* Initialize event queue. */
   pQTemp = ( qurt_elite_queue_t* )pMe->respQ_buf;

   if ( ADSP_FAILED( result = qurt_elite_queue_init( VOICE_STREAM_MGR_SVC_RESP_Q_NAME,
                      VOICE_STREAM_MGR_MAX_SVC_RESP_Q_ELEMENTS, pQTemp ) ) )
   {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "Failed to initialize VoiceStreamMgr event queue!" );
      return result;
   }

   pMe->respQ = pQTemp;

   /* Set up channel. */
   qurt_elite_channel_init( &pMe->channel );

   /* Add system command queue. */
   if ( ADSP_FAILED( result = qurt_elite_channel_addq( &pMe->channel, pMe->service_handle.cmdQ, unSysCmdQMask ) ) )
   {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "VoiceStreamMgr: fail to add mqs to channel result = %d!", result );
      return( result );
   }

   /* Add system event queue. */
   if ( ADSP_FAILED( result = qurt_elite_channel_addq(&pMe->channel, pMe->respQ, unSysEvtQMask ) ) )
   {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
            "VoiceStreamMgr: failed to add event queue to channel with result = %d!", result );
      return( result );
   }

    /* Populate Service. */
   pMe->service_handle.unSvcId = ELITE_VOICE_STREAM_SVCID;

   /* Create all session states here. */
   vsm_create_all_session_states( pMe );

   /* Once session is created, initialize APR router data base. */
   if ( ADSP_FAILED( result = VoiceStreamMgr_AprRouterInit( pMe ) ) )
   {
     MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "VoiceStreamMgr: fail to init router result = %d!", result );
     return(result);
   }

   /* Launch the thread. */
   if ( ADSP_FAILED( result = qurt_elite_thread_launch( &( pMe->service_handle.threadId ),
                     VOICE_THREAD_NAME, NULL, THREAD_STACK_SIZE, elite_get_thrd_prio(ELITETHREAD_STAT_VOICE_STREAM_SVC_PRIO_ID),
                     vsm_workloop, ( void* )pMe, QURT_ELITE_HEAP_DEFAULT ) ) )
   {
      MSG(MSG_SSID_DFLT, MSG_LEGACY_FATAL, "Failed to create VoiceStreamMgr Thread!");
      //vsm_destroy(pMe);
      return result;
   }

   *handle = &( pMe->service_handle );

   /* Register with qurt_elite memory map. */
   qurt_elite_memorymap_register( &vsm_memory_map_client );

    ( void ) __aprv2_cmd_local_dns_lookup(
               vsm_mvm_dns, sizeof( vsm_mvm_dns ), &vsm_mvm_addr );

    ( void ) __aprv2_cmd_local_dns_lookup(
               vsm_cvp_dns, sizeof( vsm_cvp_dns ), &vsm_cvp_addr );

    ( void ) vccm_mmpm_register( VCCM_CLIENT_ID_CVS );

   MSG( MSG_SSID_DFLT, MSG_LEGACY_MED, "VoiceStreamMgr: Created" );

   return ADSP_EOK;
}

