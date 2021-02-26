/*
  Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Header: //components/rel/avs.adsp/2.8.5/vsd/common/cvd/cvs/src/vsm_session_cmd_handler.cpp#4 $
  $Author: pwbldsvc $
*/

#include "qurt_elite.h"
#include "mmstd.h"
#include "VoiceStreamMgr.h"
#include "vsm_header_i.h"
#include "vsm_common_i.h"
#include "venc_svc.h"
#include "VoiceDec.h"
#include "VoiceMixerSvc.h"
#include "VoiceCmnUtils.h"
#include "adsp_vparams_api.h"
#include "cvd_log_i.h"
#include "cvd_cal_log_i.h"
#include "AFEInterface.h"
#include "VoiceLoggingUtils.h"
#include "vsm_apr_router.h"
#include "vccm_api.h"

extern elite_apr_handle_t vsm_apr_handle;
extern uint16_t vsm_mvm_addr;
extern uint16_t vsm_cvp_addr;
extern elite_svc_handle_t *txVoiceMatrix;
extern elite_svc_handle_t *rxVoiceMatrix;

//TBD: Make this a VSM struct member
extern uint32_t vsm_memory_map_client;

/************************ Command handlers **********************
*****************************************************************/

static int32_t VSM_vpcm_evt_push_buffer_to_enc_dec_processing (
  vsm_session_object_t* session_object
)
{
  int32_t rc = ADSP_EOK;
  vss_ivpcm_evt_push_buffer_v2_t* payload;
  uint32_t payload_len;
  voice_evt_push_host_pcm_buf_v2_t push_buf_event;
  uint64_t out_buf_virt_addr = ( uint64_t ) NULL;
  uint64_t in_buf_virt_addr = ( uint64_t ) NULL;
  uint32_t cvd_mem_handle = ( uint64_t ) NULL;
  uint16_t client_addr;
  elite_msg_any_t msg;
  void* apr_payload;

  if( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  elite_apr_packet_t *apr_pkt = ( elite_apr_packet_t * )( session_object->cmd_control.cmd_msg.pPayload );
  client_addr = apr_pkt->src_addr;

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED, "VSM_vpcm_evt_push_buffer_to_enc_dec_processing(): "
         " session_portID = 0x%08X", apr_pkt->dst_port );

  for ( ;; )
  {

    if ( FALSE == session_object->vpcm_info.is_enabled )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_vpcm_evt_push_buffer_to_enc_dec_processing(): VPCM not started" );
      break;
    }

    payload = APRV2_PKT_GET_PAYLOAD( vss_ivpcm_evt_push_buffer_v2_t, apr_pkt );
    payload_len = APRV2_PKT_GET_PAYLOAD_BYTE_SIZE( apr_pkt->header );

    if ( payload_len != sizeof( vss_ivpcm_evt_push_buffer_v2_t ) )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_vpcm_evt_push_buffer_to_enc_dec_processing():"
             " Invalid data. Payload len: %d, expected len: %d", payload_len,
             sizeof( vss_ivpcm_evt_push_buffer_v2_t ) );
      break;
    }

    cvd_mem_handle = session_object->vpcm_info.mem_handle;

    if ( payload->push_buf_mask & VSS_IVPCM_PUSH_BUFFER_MASK_OUTPUT_BUFFER )
    {
      rc = cvd_mem_mapper_validate_attributes_align( cvd_mem_handle, payload->out_buf_mem_address );
      if ( rc )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_vpcm_evt_push_buffer_to_enc_dec_processing():"
               " Mis-aligned out_buf_mem_address: 0x%016X.", payload->out_buf_mem_address );
        break;
      }

      rc = cvd_mem_mapper_validate_attributes_align( cvd_mem_handle, payload->out_buf_mem_size );
      if ( rc )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_vpcm_evt_push_buffer_to_enc_dec_processing():"
               " Mis-aligned out_buf_mem_size: %d.", payload->out_buf_mem_size );
        break;
      }

      rc = cvd_mem_mapper_validate_mem_is_in_region( cvd_mem_handle, payload->out_buf_mem_address,
                                                     payload->out_buf_mem_size );
      if ( rc )
      {
        MSG_3( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_vpcm_evt_push_buffer_to_enc_dec_processing():"
               " Memory is not within range, mem_handle: 0x%08X, out_buf_mem_address: 0x%016X, size: %d. ",
               cvd_mem_handle, payload->out_buf_mem_address, payload->out_buf_mem_size );
        rc = ADSP_EBADPARAM;
        break;
      }

      rc = cvd_mem_mapper_get_virtual_addr( cvd_mem_handle,
                                            payload->out_buf_mem_address,
                                            &out_buf_virt_addr );
      if ( rc )
      {
        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_vpcm_evt_push_buffer_to_enc_dec_processing():"
               " Cannot get virtual address, mem_handle: 0x%08X, out_buf_mem_address: 0x%016X, ",
               cvd_mem_handle, payload->out_buf_mem_address );
        rc = ADSP_EFAILED;
        break;
      }
    }

    if ( payload->push_buf_mask & VSS_IVPCM_PUSH_BUFFER_MASK_INPUT_BUFFER )
    {
      rc = cvd_mem_mapper_validate_attributes_align( cvd_mem_handle, payload->in_buf_mem_address );
      if ( rc )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_vpcm_evt_push_buffer_to_enc_dec_processing():"
               " Mis-aligned in_buf_mem_address: 0x%016X.", payload->in_buf_mem_address );
        break;
      }

      rc = cvd_mem_mapper_validate_mem_is_in_region( cvd_mem_handle, payload->in_buf_mem_address,
                                                     payload->in_buf_mem_size );
      if ( rc )
      {
        MSG_3( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_vpcm_evt_push_buffer_to_enc_dec_processing():"
               " Memory is not within range, mem_handle: 0x%08X, in_buf_mem_address: 0x%016X, size: %d.",
               cvd_mem_handle, payload->in_buf_mem_address, payload->in_buf_mem_size );
        rc = ADSP_EBADPARAM;
        break;
      }

      rc = cvd_mem_mapper_get_virtual_addr( cvd_mem_handle,
                                            payload->in_buf_mem_address,
                                            &in_buf_virt_addr );
      if ( rc )
      {
        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_vpcm_evt_push_buffer_to_enc_dec_processing():"
               " Cannot get virtual address, mem_handle: 0x%08X, in_buf_mem_address: 0x%016X.",
               cvd_mem_handle, payload->in_buf_mem_address );
        rc = ADSP_EFAILED;
        break;
      }

      /* Invalidate the cache */
      cvd_mem_mapper_cache_invalidate( in_buf_virt_addr, payload->in_buf_mem_size );
    }

    switch( payload->tap_point )
    {
    case VSS_IVPCM_TAP_POINT_TX_DEFAULT:
      push_buf_event.tap_point = VOICESTREAM_MODULE_TX;
      break;

    case VSS_IVPCM_TAP_POINT_RX_DEFAULT:
      push_buf_event.tap_point = VOICESTREAM_MODULE_RX;
      break;

    default:
      /* Drop and free the push buffer event. */
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_vpcm_evt_push_buffer_to_enc_dec_processing():"
             " Invalid tap point: 0x%08X", payload->tap_point );
      rc = ADSP_EBADPARAM;
      break;
    }

    if ( rc ) break;

    push_buf_event.mask = payload->push_buf_mask;
    push_buf_event.rd_buff_addr_lsw = ( ( uint32_t ) out_buf_virt_addr );
    push_buf_event.rd_buff_addr_msw = ( ( uint32_t ) ( out_buf_virt_addr >> 32 ) );
    push_buf_event.wr_buff_addr_lsw = ( ( uint32_t ) in_buf_virt_addr );
    push_buf_event.wr_buff_addr_msw = ( ( uint32_t ) ( in_buf_virt_addr >> 32 ) );
    push_buf_event.rd_buff_size = payload->out_buf_mem_size;
    push_buf_event.wr_buff_size = payload->in_buf_mem_size;
    push_buf_event.sampling_rate = payload->sampling_rate;
    push_buf_event.wr_num_chan = payload->num_in_channels;

    elite_apr_packet_t* apr_payload_pkt;

    rc = elite_apr_if_alloc_event(
           vsm_apr_handle,
           session_object->apr_info.self_addr, session_object->apr_info.self_port,
           session_object->apr_info.self_addr, session_object->apr_info.self_port,
           VSM_ASYNC_CMD_TOKEN_TX, VOICE_EVT_PUSH_HOST_BUF_V2,
           sizeof( voice_evt_push_host_pcm_buf_v2_t ), &apr_payload_pkt );
    if( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"VSM_vpcm_evt_push_buffer_to_enc_dec_processing:"
             " FAILED to allocate APR packet, session_port0x%X )",
            ( int )session_object->apr_info.self_port );
      rc = ADSP_EFAILED;
      break;
    }
    apr_payload = ( void* )( elite_apr_if_get_payload_ptr( apr_payload_pkt ) );
    memscpy( apr_payload, sizeof( voice_evt_push_host_pcm_buf_v2_t ), &push_buf_event, sizeof( voice_evt_push_host_pcm_buf_v2_t ) );

    msg.unOpCode = ELITE_APR_PACKET;
    msg.pPayload = (void *) apr_payload_pkt;

    if ( ( VOICESTREAM_MODULE_TX == push_buf_event.tap_point ) && ( session_object->venc ) )
    {
      rc = qurt_elite_queue_push_back( session_object->venc->cmdQ, ( uint64_t* )&msg );
      if ( ADSP_FAILED( rc ) )
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,"VSM_vpcm_evt_push_buffer_to_enc_dec_processing: FAILED to set enc operating mode on tx!!\n" );
        elite_msg_return_payload_buffer( &msg );
      }
    }
    else if ( ( VOICESTREAM_MODULE_RX == push_buf_event.tap_point ) && ( session_object->vdec ) )
    {
      rc = qurt_elite_queue_push_back( session_object->vdec->cmdQ, ( uint64_t* )&msg );
      if ( ADSP_FAILED( rc ) )
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,"VSM_vpcm_evt_push_buffer_to_enc_dec_processing: FAILED to set enc operating mode on rx!!\n" );
        elite_msg_return_payload_buffer( &msg );
      }
    }
    break;
  }

  ( void ) elite_apr_if_free( vsm_apr_handle, apr_pkt );
  return ADSP_EOK;
}

static int32_t VSM_vpcm_evt_notify_to_client_processing (
  vsm_session_object_t* session_object
)
{
  int32_t rc = ADSP_EOK;
  vss_ivpcm_evt_notify_v2_t notify_event;
  voice_evt_notify_host_pcm_buf_v2_t* payload;
  uint64_t out_buf_virt_addr;
  uint64_t out_buf_mem_addr;
  uint64_t in_buf_virt_addr;
  uint64_t in_buf_mem_addr;
  uint32_t handle;
  bool_t is_lock_acquired = TRUE;
  uint16_t client_addr;
  elite_msg_any_t msg;

  if( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  msg = session_object->cmd_control.cmd_msg;
  elite_apr_packet_t *apr_pkt = ( elite_apr_packet_t * )( session_object->cmd_control.cmd_msg.pPayload );
  client_addr = apr_pkt->src_addr;

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED, "VSM_vpcm_evt_notify_to_client_processing(): "
         " session_portID = 0x%08X", apr_pkt->dst_port );

  for ( ; ; )
  {
    if ( session_object->vpcm_info.client_addr == APRV2_PKT_INIT_ADDR_V )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_vpcm_evt_notify_to_client_processing():"
             " Invalid params, packet->dst_port: 0x%4X", apr_pkt->dst_port);
      break;
    }

    payload = APRV2_PKT_GET_PAYLOAD( voice_evt_notify_host_pcm_buf_v2_t, apr_pkt );

    switch( payload->tap_point )
    {
    case VOICESTREAM_MODULE_TX:
      notify_event.tap_point = VSS_IVPCM_TAP_POINT_TX_DEFAULT;
      break;

    case VOICESTREAM_MODULE_RX:
      notify_event.tap_point = VSS_IVPCM_TAP_POINT_RX_DEFAULT;
      break;

    default:
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_vpcm_evt_notify_to_client_processing():"
             " Invalid tap point: 0x%8X", payload->tap_point);
      rc = ADSP_EBADPARAM;
      break;
    }

    if ( rc ) break;

    notify_event.notify_mask = payload->mask;
    if ( notify_event.notify_mask & VSS_IVPCM_NOTIFY_MASK_OUTPUT_BUFFER )
    {
      out_buf_virt_addr = payload->rd_buff_addr_msw;
      out_buf_virt_addr = ( ( out_buf_virt_addr << 32 ) | payload->rd_buff_addr_lsw );

      rc = cvd_mem_mapper_get_mem_addr(
             session_object->vpcm_info.mem_handle,
             out_buf_virt_addr,
             &out_buf_mem_addr );
      if ( rc )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_vpcm_evt_notify_to_client_processing():"
               " Unable to get mem_addr for out_buf_virt_addr: 0x%016X", out_buf_virt_addr );
        break;
      }
      notify_event.out_buf_mem_address = out_buf_mem_addr;

      /* Flush the cache */
      cvd_mem_mapper_cache_flush( out_buf_virt_addr, payload->rd_buff_size );
    }

    if ( notify_event.notify_mask & VSS_IVPCM_NOTIFY_MASK_INPUT_BUFFER )
    {
      in_buf_virt_addr = payload->wr_buff_addr_msw;
      in_buf_virt_addr = ( ( in_buf_virt_addr << 32 ) | payload->wr_buff_addr_lsw );

      rc = cvd_mem_mapper_get_mem_addr(
             session_object->vpcm_info.mem_handle,
             in_buf_virt_addr,
             &in_buf_mem_addr );
      if ( rc )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_vpcm_evt_notify_to_client_processing():"
               " Unable to get mem_addr for in_buf_virt_addr: 0x%016X", in_buf_virt_addr );
        break;
      }
      notify_event.in_buf_mem_address = in_buf_mem_addr;
    }

    notify_event.filled_out_size = payload->rd_buff_size;
    notify_event.request_buf_size = payload->wr_buff_size;
    notify_event.sampling_rate = payload->sampling_rate;

    /* For OUTPUT only mode or dual mic OUTPUT-INPUT mode, write buffer size
       and hence request buffer size will be zero and need to be derived from
       read buffer size. */
    if ( notify_event.request_buf_size == 0 )
    {
      /* Derive request buffer size from read buffer size. */
      notify_event.request_buf_size = payload->rd_buff_size;

      /* If the output buffer mask is not set, set the filled out size to zero. */
      if ( ( notify_event.notify_mask & VSS_IVPCM_NOTIFY_MASK_OUTPUT_BUFFER ) == 0 )
      {
        notify_event.filled_out_size = 0;
      }
    }

    /* If the mode is NONE or INPUT only, number of output channels is derived
       from num of write channels from FW. */
    if ( ( notify_event.notify_mask &
           ( VSS_IVPCM_NOTIFY_MASK_OUTPUT_BUFFER | VSS_IVPCM_NOTIFY_MASK_INPUT_BUFFER ) ) == 0
         || ( ( ( notify_event.notify_mask & VSS_IVPCM_NOTIFY_MASK_INPUT_BUFFER ) != 0 )
         && ( ( notify_event.notify_mask & VSS_IVPCM_NOTIFY_MASK_OUTPUT_BUFFER ) == 0 ) ) )
    {
      /* Number of expected input channels. */
      notify_event.num_out_channels = payload->wr_num_chan;
    }
    else /* If OUTPUT only or OUTPUT-INPUT mode. */
    {
      /* Number of output channels.  */
      notify_event.num_out_channels = payload->rd_num_chan;
    }

    rc = elite_apr_if_alloc_send_event(
           vsm_apr_handle,
           session_object->apr_info.self_addr, session_object->vpcm_info.cvs_handle,
           session_object->vpcm_info.client_addr, session_object->vpcm_info.client_handle,
           apr_pkt->token, VSS_IVPCM_EVT_NOTIFY_V2,
           &notify_event, sizeof( notify_event ) );
    VSM_COMM_ERROR( rc, session_object->vpcm_info.client_addr );

    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "VSM_vpcm_evt_notify_to_client_processing(): sent event to client"
           " client addr = 0x%08X, client port = 0x%08X", session_object->vpcm_info.client_addr, session_object->vpcm_info.client_handle );
    break;
  }

  ( void ) elite_apr_if_free( vsm_apr_handle, apr_pkt );
  return ADSP_EOK;
}

static int32_t VSM_set_voc_param_cmd_control (
  vsm_session_object_t* session_object
)
{
  int32_t rc = ADSP_EOK;
  uint16_t client_addr;
  vss_istream_cmd_set_voc_param_t* set_param = NULL;
  vsm_stream_param_cache_t* voc_param_cache = NULL;
  vss_istream_voc_param_data_t* payload = NULL;
  vss_istream_voc_param_data_t* voc_param = NULL;
  uint32_t set_param_payload_size = 0;
  bool_t is_cached = FALSE;
  uint8_t* cached_voc_param = NULL;
  uint8_t* temp_payload = NULL;
  uint32_t num_params = 0;
  uint32_t remaining_data_len = 0;
  cvd_virt_addr_t mem_virt_addr;

  enum
  {
    VSM_ACTION_ENUM_UNINITIALIZED,
    VSM_ACTION_ENUM_SET_VOC_PARAM_1,
    VSM_ACTION_ENUM_WAIT_2,
    VSM_ACTION_ENUM_ERROR_HANDLE,
    VSM_ACTION_ENUM_REPORT_FAILURE,
    VSM_ACTION_ENUM_COMPLETE,
    VSM_ACTION_ENUM_INVALID
  };

  if( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  elite_apr_packet_t *apr_pkt = ( elite_apr_packet_t * )( session_object->cmd_control.cmd_msg.pPayload );
  client_addr = apr_pkt->src_addr;

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "VSM_set_voc_param_cmd_control():  session_portID = 0x%08X,"
         " current_action %d", apr_pkt->dst_port, session_object->cmd_control.current_action );

  session_object->cmd_control.is_cmd_pending = TRUE;
  set_param = APRV2_PKT_GET_PAYLOAD( vss_istream_cmd_set_voc_param_t, apr_pkt );

  while ( VSM_ACTION_ENUM_UNINITIALIZED == session_object->cmd_control.current_action )
  {
    set_param_payload_size = APRV2_PKT_GET_PAYLOAD_BYTE_SIZE( apr_pkt->header );

    if ( set_param_payload_size < sizeof( vss_istream_cmd_set_voc_param_t ) )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_set_voc_param_cmd_control(): Unexpected payload size, %d < %d",
             set_param_payload_size, sizeof( vss_istream_cmd_set_voc_param_t ) );

      session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
      break;
    }

    if ( FALSE == vsm_utility_verify_param_data_size_aligned( set_param->mem_size ) )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_set_voc_param_cmd_control(): Invalid param data size, %d",
                                              set_param->mem_size );
      session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
      break;
    }

    if ( set_param->mem_size < sizeof( vss_istream_voc_param_data_t ) )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_set_voc_param_cmd_control(): Invalid param data length %d < Min length %d",
             set_param->mem_size, sizeof( vss_istream_voc_param_data_t ) );
      session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
      break;

      break;
    }

    if ( set_param->mem_handle == NULL )
    {
      /* Inband mode. */
      if ( set_param_payload_size != ( set_param->mem_size + sizeof( vss_istream_cmd_set_voc_param_t ) ) )
      {
        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_set_voc_param_cmd_control(): Invalid payload size %d != %d",
               set_param_payload_size, ( set_param->mem_size + sizeof( vss_istream_cmd_set_voc_param_t ) ) );
        session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
        session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
        break;
      }
      temp_payload = ( uint8_t* )set_param;
      payload = ( vss_istream_voc_param_data_t* )( temp_payload + sizeof( vss_istream_cmd_set_voc_param_t ) );  
    }
    else
    {
      /* out of band mode. */
      rc = cvd_mem_mapper_validate_handle( set_param->mem_handle );
      if ( rc )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_set_voc_param_cmd_control(): Invalid mem_handle: 0x%08X.",
                                                set_param->mem_handle );
        session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
        session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
        break;
      }

      rc = cvd_mem_mapper_validate_attributes_align( set_param->mem_handle, set_param->mem_address );
      if ( rc )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_set_voc_param_cmd_control(): Mis-aligned mem_address: 0x%016X.",
                                                set_param->mem_address );
      
        session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
        session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
        break;
      }

      rc = cvd_mem_mapper_validate_mem_is_in_region( set_param->mem_handle, set_param->mem_address,
                                                     set_param->mem_size );
      if ( rc )
      {
        MSG_3( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_set_voc_param_cmd_control(): Memory is not within range,"
               " mem_handle: 0x%016X, mem_address: 0x%016X, size: %d.", set_param->mem_handle,
               set_param->mem_address, set_param->mem_size );
        session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
        session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
        break;
      }

      rc = cvd_mem_mapper_get_virtual_addr_v2( set_param->mem_handle, set_param->mem_address, &mem_virt_addr );
      if ( rc )
      {
        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_set_voc_param_cmd_control(): Cannot get virtual address,"
               " mem_handle: 0x%016X, mem_address: 0x%016X.", set_param->mem_handle, set_param->mem_address );
        session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
        session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
        break;
      }
      payload = ( vss_istream_voc_param_data_t* ) mem_virt_addr.ptr;
    }

    if ( payload->param_size > VSM_MAX_PARAM_DATA_LEN )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "VSM_set_voc_param_cmd_control(): param size : %d", payload->param_size );
    }

    voc_param_cache = &session_object->stream_param_cache;
    num_params = voc_param_cache->num_voc_params;
    cached_voc_param = voc_param_cache->data;

    /* Caching the vocoder parameter received. */
    while ( num_params > 0 )
    {
      voc_param = ( vss_istream_voc_param_data_t *) cached_voc_param;

      if ( ( voc_param->param_id == payload->param_id ) &&
           ( voc_param->direction_id == payload->direction_id ) )
      {
        if ( voc_param->param_size != payload->param_size )
        {
          MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_set_voc_param_cmd_control(): change in param size."
                 "Error caching Media id 0x%08X Param id 0x%08X", payload->media_id, payload->param_id );
          rc = ADSP_EBADPARAM;
          break;
        }

        MSG( MSG_SSID_DFLT, MSG_LEGACY_MED, "VSM_set_voc_param_cmd_control(): cached entry found");

        mmstd_memcpy( voc_param + sizeof( vss_istream_voc_param_data_t ), voc_param->param_size, 
                      payload + sizeof( vss_istream_voc_param_data_t ), payload->param_size );

        is_cached = TRUE;
        break;
      }
      else
      {
        cached_voc_param += ( sizeof( vss_istream_voc_param_data_t ) + voc_param->param_size ); 
        num_params--;
      }
    }

    if ( rc )
    {
      session_object->cmd_control.cmd_result = ADSP_EFAILED;
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
      break;
    }

    if ( FALSE == is_cached )
    {
      remaining_data_len = VSM_VOC_PARAM_CACHE_LEN - voc_param_cache->data_len;
      cached_voc_param = voc_param_cache->data;
      cached_voc_param += voc_param_cache->data_len;
      if ( remaining_data_len >= ( payload->param_size + sizeof( vss_istream_voc_param_data_t ) ) )
      {
        mmstd_memcpy( cached_voc_param , payload->param_size + sizeof( vss_istream_voc_param_data_t ), 
                        payload, payload->param_size + sizeof( vss_istream_voc_param_data_t ) );
        
        voc_param_cache->data_len += payload->param_size + sizeof( vss_istream_voc_param_data_t );
        voc_param_cache->num_voc_params++;
      }
      else
      {
        MSG_3( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_set_voc_param_cmd_control(): Maximum cached"
               " entries %d Num_entries %d rem_data_len %d", VSM_MAX_NUM_OF_STREAM_PARAMS, 
               voc_param_cache->num_voc_params, remaining_data_len );

        MSG_3( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_set_voc_param_cmd_control(): Error caching "
               "Media id 0x%08X Param id 0x%08X param_size %d", payload->media_id,
               payload->param_id, payload->param_size );
      }
    }
    session_object->cmd_control.current_action = VSM_ACTION_ENUM_SET_VOC_PARAM_1;
    break;
  }

  for ( ; ; )
  {
    switch ( session_object->cmd_control.current_action )
    {
      case VSM_ACTION_ENUM_SET_VOC_PARAM_1:
      {
        rc = vsm_action_set_voc_param( session_object, payload );
        if ( ADSP_FAILED( rc ) )
        {
          session_object->cmd_control.cmd_result = ADSP_EFAILED;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
          continue;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_WAIT_2;
          return ADSP_EOK;
        }
      }
      break;

      case VSM_ACTION_ENUM_WAIT_2:
      {
        if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
        {
          session_object->cmd_control.cmd_result = session_object->cmd_control.resp_result;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_COMPLETE;
        }
        continue;
      }
      break;

      case VSM_ACTION_ENUM_ERROR_HANDLE:
      {
        if ( TRUE == session_object->cmd_control.is_resp_pending )
        {
          return ADSP_EOK;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
        }
        continue;
      }
      break;

      case VSM_ACTION_ENUM_REPORT_FAILURE:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"VSM_set_voc_param_cmd_control: command FAILED:." );
        elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, session_object->cmd_control.cmd_result );
        session_object->cmd_control.is_cmd_pending = FALSE;
        return ADSP_EOK;
      }
      break;

      case VSM_ACTION_ENUM_COMPLETE:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,"VSM_set_voc_param_cmd_control: command Success:." );
        elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EOK );
        session_object->cmd_control.is_cmd_pending = FALSE;
        return ADSP_EOK;
      }
      break;

      default:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"VSM_set_voc_param_cmd_control: ERROR: Unexpected..." );
      }
      return ADSP_EOK;
    }
  }

  return ADSP_EOK;
}

static ADSPResult VSM_stream_stop_playback_cmd_control(
  vsm_session_object_t* session_object
)
{
  int32_t rc = ADSP_EOK;
  vsm_enable_state_enum_t enable_flag;
  uint16_t client_addr;
  enum
  {
    VSM_ACTION_ENUM_UNINITIALIZED,
    VSM_ACTION_ENUM_STOP_PLAYBACK,
    VSM_ACTION_ENUM_ERROR_HANDLE,
    VSM_ACTION_ENUM_REPORT_FAILURE,
    VSM_ACTION_ENUM_COMPLETE,
    VSM_ACTION_ENUM_INVALID
  };

  if( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  elite_apr_packet_t *apr_pkt = ( elite_apr_packet_t * )( session_object->cmd_control.cmd_msg.pPayload );
  client_addr = apr_pkt->src_addr;

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "VSM_stream_stop_playback_cmd_control():  session_portID = 0x%08X,"
         " current_action %d", apr_pkt->dst_port, session_object->cmd_control.current_action );

  session_object->cmd_control.is_cmd_pending = TRUE;
  while ( VSM_ACTION_ENUM_UNINITIALIZED == session_object->cmd_control.current_action )
  {
    if ( ( VSM_ENABLED == session_object->active_set.playback.enable_flag ) &&
         ( VSM_STATE_ENUM_RUN == session_object->state_control.state )
       )
    {
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_STOP_PLAYBACK;
    }
    else if( VSM_ENABLED == session_object->active_set.playback.enable_flag )
    {
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_COMPLETE;
    }
    else
    {
      session_object->cmd_control.cmd_result = ADSP_EALREADY;
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
      break;
    }
    session_object->active_set.playback.enable_flag = VSM_DISABLED;
    break;
  }

  for ( ; ; )
  {
    switch ( session_object->cmd_control.current_action )
    {
      case VSM_ACTION_ENUM_STOP_PLAYBACK:
      {
        rc = vsm_utility_playback_control_stop( session_object );
        if ( ADSP_EPENDING == rc )
        {
          return ADSP_EOK;
        }
        else if ( ADSP_FAILED( rc ) )
        {
          session_object->cmd_control.cmd_result = ADSP_EFAILED;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
          continue;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_COMPLETE;
          continue;
        }
      }
      break;

      case VSM_ACTION_ENUM_ERROR_HANDLE:
      {
        if ( TRUE == session_object->cmd_control.is_resp_pending )
        {
          return ADSP_EOK;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
          continue;
        }
      }
      break;

      case VSM_ACTION_ENUM_REPORT_FAILURE:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"VSM_stream_stop_playback_cmd_control: command FAILED:." );
        elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, session_object->cmd_control.cmd_result );
        session_object->cmd_control.is_cmd_pending = FALSE;
        return ADSP_EOK;
      }
      break;

      case VSM_ACTION_ENUM_COMPLETE:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,"VSM_stream_stop_playback_cmd_control: command Success:." );
        elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EOK );
        session_object->cmd_control.is_cmd_pending = FALSE;
        vsm_action_toggle_csm( session_object, TRUE );
        return ADSP_EOK;
      }
      break;

      default:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"VSM_stream_stop_playback_cmd_control: ERROR: Unexpected..." );
      }
      return ADSP_EOK;
    }
  }

  return ADSP_EOK;
}

static ADSPResult VSM_stream_start_playback_cmd_control (
  vsm_session_object_t* session_object
)
{
  int32_t rc = ADSP_EOK;
  vsm_start_playback_t playback_param;
  vss_iplayback_cmd_start_t* in_args;
  uint16_t client_addr;
  enum
  {
    VSM_ACTION_ENUM_UNINITIALIZED,
    VSM_ACTION_ENUM_START_PLAYBACK,
    VSM_ACTION_ENUM_ERROR_HANDLE,
    VSM_ACTION_ENUM_REPORT_FAILURE,
    VSM_ACTION_ENUM_COMPLETE,
    VSM_ACTION_ENUM_INVALID
  };

  if( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  elite_apr_packet_t *apr_pkt = ( elite_apr_packet_t * )( session_object->cmd_control.cmd_msg.pPayload );
  client_addr = apr_pkt->src_addr;

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "VSM_stream_start_playback_cmd_control():  session_portID = 0x%08X,"
         " current_action %d", apr_pkt->dst_port, session_object->cmd_control.current_action );

  session_object->cmd_control.is_cmd_pending = TRUE;
  while ( VSM_ACTION_ENUM_UNINITIALIZED == session_object->cmd_control.current_action )
  {
    /* Playback already enabled. */
    if ( VSM_ENABLED == session_object->active_set.playback.enable_flag )
    {
      session_object->cmd_control.cmd_result = ADSP_EALREADY;
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
      break;
    }

    if ( VSS_IPLAYBACK_CMD_START == apr_pkt->opcode )
    {
      if( FALSE == vsm_utility_validate_apr_payload_size( apr_pkt ) )
      {
        session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
        session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
        break;
      }
      in_args = APRV2_PKT_GET_PAYLOAD( vss_iplayback_cmd_start_t, apr_pkt );
      session_object->active_set.playback.port_id = in_args->port_id;
    }
    else
    {
      session_object->active_set.playback.port_id = VSS_IPLAYBACK_PORT_ID_DEFAULT;
    }
    session_object->active_set.playback.enable_flag = VSM_ENABLED;

    /* Not in RUN State, cache and return immediately.
    * Cached value will be applied on transition to RUN state.
    */
    if ( VSM_STATE_ENUM_RUN != session_object->state_control.state )
    {
      session_object->cmd_control.cmd_result = ADSP_EOK;
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_COMPLETE;
      break;
    }
    session_object->cmd_control.current_action = VSM_ACTION_ENUM_START_PLAYBACK;
    break;
  }

  for ( ; ; )
  {
    switch ( session_object->cmd_control.current_action )
    {
      case VSM_ACTION_ENUM_START_PLAYBACK:
      {
        rc = vsm_utility_playback_control_start( session_object );
        if ( ADSP_EPENDING == rc )
        {
          return ADSP_EOK;
        }
        else if ( ADSP_FAILED( rc ) )
        {
          session_object->cmd_control.cmd_result = ADSP_EFAILED;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
          continue;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_COMPLETE;
          continue;
        }
      }
      break;

      case VSM_ACTION_ENUM_ERROR_HANDLE:
      {
        if ( TRUE == session_object->cmd_control.is_resp_pending )
        {
          return ADSP_EOK;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
          continue;
        }
      }
      break;

      case VSM_ACTION_ENUM_REPORT_FAILURE:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"VSM_stream_start_playback_cmd_control: command FAILED:." );
        elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, session_object->cmd_control.cmd_result );
        session_object->cmd_control.is_cmd_pending = FALSE;
        return ADSP_EOK;
      }
      break;

      case VSM_ACTION_ENUM_COMPLETE:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,"VSM_stream_start_playback_cmd_control: command Success:." );
        elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EOK );
        session_object->cmd_control.is_cmd_pending = FALSE;
        vsm_action_toggle_csm( session_object, FALSE );
        return ADSP_EOK;
      }
      break;

      default:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"VSM_stream_start_playback_cmd_control: ERROR: Unexpected..." );
      }
      return ADSP_EOK;
    }
  }

  return ADSP_EOK;
}

static int32_t VSM_stream_stop_record_cmd_control (
  vsm_session_object_t* session_object
)
{
  int32_t rc = ADSP_EOK;
  vsm_enable_state_enum_t enable_flag;
  uint16_t client_addr;
  enum
  {
    VSM_ACTION_ENUM_UNINITIALIZED,
    VSM_ACTION_ENUM_STOP_RECORD,
    VSM_ACTION_ENUM_ERROR_HANDLE,
    VSM_ACTION_ENUM_REPORT_FAILURE,
    VSM_ACTION_ENUM_COMPLETE,
    VSM_ACTION_ENUM_INVALID
  };

  if( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  elite_apr_packet_t *apr_pkt = ( elite_apr_packet_t * )( session_object->cmd_control.cmd_msg.pPayload );
  client_addr = apr_pkt->src_addr;

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "VSM_stream_stop_record_cmd_control():  session_portID = 0x%08X,"
         " current_action %d", apr_pkt->dst_port, session_object->cmd_control.current_action );

  session_object->cmd_control.is_cmd_pending = TRUE;
  while ( VSM_ACTION_ENUM_UNINITIALIZED == session_object->cmd_control.current_action )
  {
    if ( ( VSM_ENABLED == session_object->active_set.record.enable_flag ) &&
         ( VSM_STATE_ENUM_RUN == session_object->state_control.state )
       )
    {
      session_object->active_set.record.enable_flag = VSM_DISABLED;
    }
    else
    {
      session_object->active_set.record.enable_flag = VSM_DISABLED;
      session_object->cmd_control.cmd_result = ADSP_EOK;
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_COMPLETE;
      break;
    }
    session_object->cmd_control.current_action = VSM_ACTION_ENUM_STOP_RECORD;
    break;
  }

  for ( ; ; )
  {
    switch ( session_object->cmd_control.current_action )
    {
      case VSM_ACTION_ENUM_STOP_RECORD:
      {
        rc = vsm_utility_record_control_stop( session_object );
        if ( ADSP_EPENDING == rc )
        {
          return ADSP_EOK;
        }
        else if ( ADSP_FAILED( rc ) )
        {
          session_object->cmd_control.cmd_result = ADSP_EFAILED;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
          continue;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_COMPLETE;
          continue;
        }
      }
      break;

      case VSM_ACTION_ENUM_ERROR_HANDLE:
      {
        if ( TRUE == session_object->cmd_control.is_resp_pending )
        {
          return ADSP_EOK;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
          continue;
        }
      }
      break;

      case VSM_ACTION_ENUM_REPORT_FAILURE:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"VSM_stream_stop_record_cmd_control: command FAILED:." );
        elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, session_object->cmd_control.cmd_result );
        session_object->cmd_control.is_cmd_pending = FALSE;
        return ADSP_EOK;
      }
      break;

      case VSM_ACTION_ENUM_COMPLETE:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,"VSM_stream_stop_record_cmd_control: command Success:." );
        elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EOK );
        session_object->cmd_control.is_cmd_pending = FALSE;
        return ADSP_EOK;
      }
      break;

      default:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"VSM_stream_stop_record_cmd_control: ERROR: Unexpected..." );
      }
      return ADSP_EOK;
    }
  }

  return ADSP_EOK;
}

static int32_t VSM_stream_start_record_cmd_control (
  vsm_session_object_t* session_object
)
{
  int32_t rc = ADSP_EOK;
  vss_istream_cmd_start_record_t* start_record_v1_params;
  vss_irecord_cmd_start_t* start_record_v2_params;
  uint16_t client_addr;
  enum
  {
    VSM_ACTION_ENUM_UNINITIALIZED,
    VSM_ACTION_ENUM_START_RECORD,
    VSM_ACTION_ENUM_ERROR_HANDLE,
    VSM_ACTION_ENUM_REPORT_FAILURE,
    VSM_ACTION_ENUM_COMPLETE,
    VSM_ACTION_ENUM_INVALID
  };

  if( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  elite_apr_packet_t *apr_pkt = ( elite_apr_packet_t * )( session_object->cmd_control.cmd_msg.pPayload );
  client_addr = apr_pkt->src_addr;

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "VSM_stream_start_record_cmd_control():  session_portID = 0x%08X,"
         " current_action %d", apr_pkt->dst_port, session_object->cmd_control.current_action );

  session_object->cmd_control.is_cmd_pending = TRUE;
  while ( VSM_ACTION_ENUM_UNINITIALIZED == session_object->cmd_control.current_action )
  {
    if( FALSE == vsm_utility_validate_apr_payload_size( apr_pkt ) )
    {
      session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
      break;
    }

    /* Recording already enabled. */
    if ( VSM_ENABLED == session_object->active_set.record.enable_flag )
    {
      session_object->cmd_control.cmd_result = ADSP_EALREADY;
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
      break;
    }

    if ( VSS_ISTREAM_CMD_START_RECORD == apr_pkt->opcode )
    {
      start_record_v1_params = APRV2_PKT_GET_PAYLOAD( vss_istream_cmd_start_record_t, apr_pkt );

      session_object->active_set.record.enable_flag = VSM_ENABLED;
      session_object->active_set.record.rx_tap_point = start_record_v1_params->rx_tap_point;
      session_object->active_set.record.tx_tap_point = start_record_v1_params->tx_tap_point;
      session_object->active_set.record.port_id = VSS_IRECORD_PORT_ID_DEFAULT;
      session_object->active_set.record.mode = VSS_IRECORD_MODE_TX_RX_MIXING;
    }
    else
    {
      start_record_v2_params = APRV2_PKT_GET_PAYLOAD( vss_irecord_cmd_start_t, apr_pkt );

      session_object->active_set.record.enable_flag = VSM_ENABLED;
      session_object->active_set.record.rx_tap_point = start_record_v2_params->rx_tap_point;
      session_object->active_set.record.tx_tap_point = start_record_v2_params->tx_tap_point;
      session_object->active_set.record.port_id = start_record_v2_params->port_id;
      session_object->active_set.record.mode = start_record_v2_params->mode;
    }

    if ( AFE_PORT_ID_INVALID == session_object->active_set.record.port_id )
    {
      //ignore the record_mode in case of recording through default ports.
      session_object->active_set.record.mode = 0;
    }

    /*  If not in RUN State, cache and return immediately.
        * Cached value will be applied on transition to RUN state.
       */
    if ( VSM_STATE_ENUM_RUN != session_object->state_control.state )
    {
      session_object->cmd_control.cmd_result = ADSP_EOK;
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_COMPLETE;
      break;
    }

    session_object->cmd_control.current_action = VSM_ACTION_ENUM_START_RECORD;
    break;
  }

  for ( ; ; )
  {
    switch ( session_object->cmd_control.current_action )
    {
      case VSM_ACTION_ENUM_START_RECORD:
      {
        rc = vsm_utility_record_control_start( session_object );
        if ( ADSP_EPENDING == rc )
        {
          return ADSP_EOK;
        }
        else if ( ADSP_FAILED( rc ) )
        {
          session_object->cmd_control.cmd_result = ADSP_EFAILED;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
          continue;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_COMPLETE;
          continue;
        }
      }
      break;

      case VSM_ACTION_ENUM_ERROR_HANDLE:
      {
        if ( TRUE == session_object->cmd_control.is_resp_pending )
        {
          return ADSP_EOK;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
          continue;
        }
      }
      break;

      case VSM_ACTION_ENUM_REPORT_FAILURE:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"VSM_stream_start_record_cmd_control: command FAILED:." );
        elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, session_object->cmd_control.cmd_result );
        session_object->cmd_control.is_cmd_pending = FALSE;
        return ADSP_EOK;
      }
      break;

      case VSM_ACTION_ENUM_COMPLETE:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,"VSM_stream_start_record_cmd_control: command Success:." );
        elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EOK );
        session_object->cmd_control.is_cmd_pending = FALSE;
        return ADSP_EOK;
      }
      break;

      default:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"VSM_stream_start_record_cmd_control: ERROR: Unexpected..." );
      }
      return ADSP_EOK;
    }
  }
  return ADSP_EOK;
}

static int32_t VSM_resync_ctm_cmd_control (
  vsm_session_object_t* session_object
)
{
  int32_t rc = ADSP_EOK;
  uint16_t client_addr;

  if( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  elite_apr_packet_t *apr_pkt = ( elite_apr_packet_t * )( session_object->cmd_control.cmd_msg.pPayload );
  client_addr = apr_pkt->src_addr;

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED, "VSM_resync_ctm_cmd_control(): "
         " session_portID = 0x%08X", apr_pkt->dst_port );

  session_object->tty_state.m_sync_recover_rx = TRUE;
  session_object->tty_state.m_sync_recover_tx = TRUE;

  rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EOK );
  VSM_COMM_ERROR( rc, client_addr );

  MSG( MSG_SSID_DFLT, MSG_LEGACY_MED, "VSM_resync_ctm_cmd_control: command Success:." );
  return ADSP_EOK;
}

static int32_t VSM_volume_mute_v2_cmd_control (
  vsm_session_object_t* session_object
)
{
  int32_t rc = ADSP_EOK;
  vss_ivolume_cmd_mute_v2_t* payload;
  uint16_t client_addr;

  enum
  {
    VSM_ACTION_ENUM_UNINITIALIZED,
    VSM_ACTION_ENUM_SET_MUTE,
    VSM_ACTION_ENUM_WAIT_1,
    VSM_ACTION_ENUM_SEND_MUTE_NOTIFICATION,
    VSM_ACTION_ENUM_ERROR_HANDLE,
    VSM_ACTION_ENUM_REPORT_FAILURE,
    VSM_ACTION_ENUM_COMPLETE,
    VSM_ACTION_ENUM_INVALID
  };

  if( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  elite_apr_packet_t *apr_pkt = ( elite_apr_packet_t * )( session_object->cmd_control.cmd_msg.pPayload );
  client_addr = apr_pkt->src_addr;

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "VSM_volume_mute_v2_cmd_control():  session_portID = 0x%08X,"
         " current_action %d", apr_pkt->dst_port, session_object->cmd_control.current_action );

  session_object->cmd_control.is_cmd_pending = TRUE;
  payload = APRV2_PKT_GET_PAYLOAD( vss_ivolume_cmd_mute_v2_t, apr_pkt );
  while ( VSM_ACTION_ENUM_UNINITIALIZED == session_object->cmd_control.current_action )
  {
    if( FALSE == vsm_utility_validate_apr_payload_size( apr_pkt ) )
    {
      session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
      break;
    }

    /* Validate payload content. */
    if ( ( ( payload->direction != VSS_IVOLUME_DIRECTION_TX ) &&
           ( payload->direction != VSS_IVOLUME_DIRECTION_RX ) ) ||
         ( ( payload->mute_flag != VSS_IVOLUME_MUTE_OFF ) &&
           ( payload->mute_flag != VSS_IVOLUME_MUTE_ON ) ) )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_volume_mute_v2_cmd_control(): " \
             "Unexpected params, direction: %d, mute_flag: %d",
             payload->direction, payload->mute_flag );
      session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
      break;
    }

    /* Read/write access to target_set.mute.
           read access to vsm_stream_handle/session_ctrl.state. */

    switch ( payload->direction )
    {
      case VSM_DIRECTION_TX:
        session_object->target_set.mute.tx_mute_flag = payload->mute_flag ;
        session_object->target_set.mute.tx_ramp_duration = payload->ramp_duration_ms;
        break;

      case VSM_DIRECTION_RX:
        session_object->target_set.mute.rx_mute_flag = payload->mute_flag ;
        session_object->target_set.mute.rx_ramp_duration = payload->ramp_duration_ms;
        break;

      default:
        VSM_COMM_ERROR( ADSP_EUNEXPECTED, client_addr );
        break;
    }
  
    /* If in RUN state, set mute on the VDSP.
        * If in IDLE state, leave settings in the target_set.
        * They will be picked up when we go to RUN. 
       */
    if ( VSM_STATE_ENUM_RUN == session_object->state_control.state )
    {
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_SET_MUTE;
    }
    else
    {
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_COMPLETE;
    }

  }

  for ( ; ; )
  {
    switch ( session_object->cmd_control.current_action )
    {
      case VSM_ACTION_ENUM_SET_MUTE:
      {
        rc = vsm_action_set_tx_rx_mute( session_object, payload->direction,
                                        session_object->target_set.mute.tx_ramp_duration,
                                        session_object->target_set.mute.rx_ramp_duration );
        if ( ADSP_FAILED( rc ) )
        {
          session_object->cmd_control.cmd_result = ADSP_EFAILED;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
          continue;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_WAIT_1;
          return ADSP_EOK;
        }
      }
      break;

      case VSM_ACTION_ENUM_WAIT_1:
      {
        if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
        {
          session_object->cmd_control.cmd_result = session_object->cmd_control.resp_result;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_SEND_MUTE_NOTIFICATION;
        }
        continue;
      }
      break;

      case VSM_ACTION_ENUM_SEND_MUTE_NOTIFICATION:
      {
        ( void ) vsm_action_send_mute_notification( session_object );
        session_object->cmd_control.current_action = VSM_ACTION_ENUM_COMPLETE;
        continue;
      }
      break;

      case VSM_ACTION_ENUM_ERROR_HANDLE:
      {
        if ( TRUE == session_object->cmd_control.is_resp_pending )
        {
          return ADSP_EOK;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
          continue;
        }
      }
      break;

      case VSM_ACTION_ENUM_REPORT_FAILURE:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"VSM_volume_mute_v2_cmd_control: command FAILED:." );
        elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, session_object->cmd_control.cmd_result );
        session_object->cmd_control.is_cmd_pending = FALSE;
        return ADSP_EOK;
      }
      break;

      case VSM_ACTION_ENUM_COMPLETE:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,"VSM_volume_mute_v2_cmd_control: command Success:." );
        session_object->active_set.mute.tx_mute_flag = session_object->target_set.mute.tx_mute_flag ;
        session_object->active_set.mute.rx_mute_flag = session_object->target_set.mute.rx_mute_flag ;
        session_object->active_set.mute.tx_ramp_duration = session_object->target_set.mute.tx_ramp_duration;
        session_object->active_set.mute.rx_ramp_duration = session_object->target_set.mute.rx_ramp_duration;
        elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EOK );
        session_object->cmd_control.is_cmd_pending = FALSE;
        return ADSP_EOK;
      }
      break;

      default:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"VSM_volume_mute_v2_cmd_control: ERROR: Unexpected..." );
      }
      return ADSP_EOK;
    }
  }
  return ADSP_EOK;
}

static int32_t VSM_cancel_event_class_cmd_control (
  vsm_session_object_t* session_object
)
{
  int32_t rc = ADSP_EOK;
  vss_inotify_cmd_cancel_event_class_t* in_args;
  uint16_t client_addr;

  if( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  elite_apr_packet_t *apr_pkt = ( elite_apr_packet_t * )( session_object->cmd_control.cmd_msg.pPayload );
  client_addr = apr_pkt->src_addr;

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED, "VSM_cancel_event_class_cmd_control(): "
         " session_portID = 0x%08X", apr_pkt->dst_port );

  if( FALSE == vsm_utility_validate_apr_payload_size( apr_pkt ) )
  {
    rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EBADPARAM );
    VSM_COMM_ERROR( rc, client_addr );
    return ADSP_EOK;
  }

  in_args = APRV2_PKT_GET_PAYLOAD( vss_inotify_cmd_cancel_event_class_t, apr_pkt );

  switch ( in_args->class_id )
  {
    case VSS_ISTREAM_EVENT_CLASS_EAMR_MODE_CHANGE:
    {
      if ( session_object->eamr_mode_change_notification_info.is_enabled == FALSE )
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_cancel_event_class_cmd_control(): Client has not registered." );
        rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EALREADY );
        VSM_COMM_ERROR( rc, client_addr );
        break;
      }

      /* Write access to eamr_mode_change_notification_info. */
      session_object->eamr_mode_change_notification_info.is_enabled = FALSE;
      session_object->eamr_mode_change_notification_info.client_addr = APRV2_PKT_INIT_ADDR_V;
      session_object->eamr_mode_change_notification_info.client_port = APR_NULL_V;   

      rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EOK );
      VSM_COMM_ERROR( rc, client_addr );
    }
    break;

    case VSS_ISTREAM_EVENT_CLASS_EVS_BANDWIDTH_CHANGE:
    {
      if ( session_object->evs_bandwidth_change_notification_info.is_enabled == FALSE )
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_cancel_event_class_cmd_control(): Client has not registered." );
        rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EALREADY );
        VSM_COMM_ERROR( rc, client_addr );
        break;
      }

      /* Write access to evs_bandwidth_change_notification_info. */
      session_object->evs_bandwidth_change_notification_info.is_enabled = FALSE;
      session_object->evs_bandwidth_change_notification_info.client_addr = APRV2_PKT_INIT_ADDR_V;
      session_object->evs_bandwidth_change_notification_info.client_port = APR_NULL_V;
      session_object->evs_bandwidth_change_notification_info.last_sent_rx_bandwidth = VSM_STREAM_VOC_BANDWIDTH_EVT_NOT_SET;

      rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EOK );
      VSM_COMM_ERROR( rc, client_addr );
    }
    break;
      
    case VSS_IAVSYNC_EVENT_CLASS_RX:
    { /*  Need to store the Cache the client addr/port info. */
      if ( session_object->avsync_client_rx_info.is_enabled == FALSE )
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_cancel_event_class_cmd_control(): Client has not registered to IAVSYNC in RX." );

        rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EALREADY );
        VSM_COMM_ERROR( rc, client_addr );

        break;
      }

      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW, "VSM_cancel_event_class_cmd_control(): IAVSYNC RX Client Addr: 0x%04X",
                                            session_object->avsync_client_rx_info.client_addr );
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW, "VSM_cancel_event_class_cmd_control(): IAVSYNC RX Client Port: 0x%04X",
                                            session_object->avsync_client_rx_info.client_port );

      /* Write access to avsync_client_info. */
      session_object->avsync_client_rx_info.is_enabled = FALSE;
      session_object->avsync_client_rx_info.client_addr = APR_NULL_V;
      session_object->avsync_client_rx_info.client_port = APR_NULL_V;

      MSG( MSG_SSID_DFLT, MSG_LEGACY_LOW, "VSM_cancel_event_class_cmd_control(): Client cancelled IAVSYNC on RX." );

      rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, rc );
      VSM_COMM_ERROR( rc, client_addr );
    }
    break;

    case VSS_IAVSYNC_EVENT_CLASS_TX:
    {
      if ( session_object->avsync_client_tx_info.is_enabled == FALSE )
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_cancel_event_class_cmd_control(): Client has not listened to IAVSYNC in TX." );
        rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EALREADY );
        VSM_COMM_ERROR( rc, client_addr );
        break;
      }

      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW, "VSM_cancel_event_class_cmd_control(): IAVSYNC TX Client Addr: 0x%04X",
                                            session_object->avsync_client_tx_info.client_addr );
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW, "VSM_cancel_event_class_cmd_control(): IAVSYNC TX Client Port: 0x%04X",
                                            session_object->avsync_client_tx_info.client_port );

      /* Write access to avsync_client_tx_info. */
      session_object->avsync_client_tx_info.is_enabled = FALSE;
      session_object->avsync_client_tx_info.client_addr = APR_NULL_V;
      session_object->avsync_client_tx_info.client_port = APR_NULL_V;

      MSG( MSG_SSID_DFLT, MSG_LEGACY_LOW, "VSM_cancel_event_class_cmd_control(): Client cancelled IAVSYNC on TX." );

      rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, rc );
      VSM_COMM_ERROR( rc, client_addr );
    }
    break;

    default:
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_cancel_event_class_cmd_control(): Unsupported event class: 0x%08X",
                                              in_args->class_id );

      rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EUNSUPPORTED );
      VSM_COMM_ERROR( rc, client_addr );
    }
    break;
  }

  MSG( MSG_SSID_DFLT, MSG_LEGACY_MED, "VSM_cancel_event_class_cmd_control: command Success:." );
  return ADSP_EOK;
}

static int32_t VSM_listen_for_event_class_cmd_control (
  vsm_session_object_t* session_object
)
{
  int32_t rc = ADSP_EOK;
  vss_inotify_cmd_listen_for_event_class_t* in_args;
  uint16_t client_addr;

  if( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  elite_apr_packet_t *apr_pkt = ( elite_apr_packet_t * )( session_object->cmd_control.cmd_msg.pPayload );
  client_addr = apr_pkt->src_addr;

  if( FALSE == vsm_utility_validate_apr_payload_size( apr_pkt ) )
  {
    rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EBADPARAM );
    VSM_COMM_ERROR( rc, client_addr );
    return ADSP_EOK;
  }

  in_args = APRV2_PKT_GET_PAYLOAD( vss_inotify_cmd_listen_for_event_class_t, apr_pkt );

  switch ( in_args->class_id )
  {
    case VSS_ISTREAM_EVENT_CLASS_EAMR_MODE_CHANGE:
    {
      if ( session_object->eamr_mode_change_notification_info.is_enabled == TRUE )
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VSM: LISTEN_EAMR_MODE_CHANGE: Client has already registered." );
        rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EALREADY );
        VSM_COMM_ERROR( rc, client_addr );

        break;
      }

      session_object->eamr_mode_change_notification_info.is_enabled = TRUE;
      session_object->eamr_mode_change_notification_info.client_addr = apr_pkt->src_addr;
      session_object->eamr_mode_change_notification_info.client_port = apr_pkt->src_port;

      /** Send eAMR mode change event to registered client if the mode is
       * different from default mode. Refer to the comments under
       * eamr_mode_change_notification_info in vsm_session_object_t for
       * details. */
      if ( ( VSS_MEDIA_ID_EAMR == session_object->active_set.media_id ) &&
           ( VSM_STATE_ENUM_RUN == session_object->state_control.state ) )
      {
        ( void ) vsm_action_send_eamr_mode_event( session_object );
      }

      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "VSM: LISTEN_EAMR_MODE_CHANGE client's Addr(0x%04X) Port(0x%04X)",
             apr_pkt->src_addr, apr_pkt->src_port );

      rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EOK );
      VSM_COMM_ERROR( rc, client_addr );
    }
    break;

    case VSS_ISTREAM_EVENT_CLASS_EVS_BANDWIDTH_CHANGE:
    {
      if ( TRUE == session_object->evs_bandwidth_change_notification_info.is_enabled )
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VSM: LISTEN_EVS_BANDWIDTH_CHANGE: Client has already listened." );
        rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EALREADY );
        VSM_COMM_ERROR( rc, client_addr );
        break;
      }

      session_object->evs_bandwidth_change_notification_info.is_enabled = TRUE;
      session_object->evs_bandwidth_change_notification_info.client_addr = apr_pkt->src_addr;
      session_object->evs_bandwidth_change_notification_info.client_port = apr_pkt->src_port;

      /** Send EVS Rx Bandwidth if registering during a call.
       * Refer to the comments under evs_bandwidth_change_notification_info in
       * vsm_session_object_t for details. */
      if ( ( VSS_MEDIA_ID_EVS == session_object->active_set.media_id ) &&
           ( VSM_STATE_ENUM_RUN == session_object->state_control.state ) )
      {
        ( void ) vsm_action_send_evs_bandwidth_event( session_object );
      }

      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "VSM: LISTEN_EVS_BANDWIDTH_CHANGE client's Addr(0x%04X) Port(0x%04X)",
             apr_pkt->src_addr, apr_pkt->src_port );

      rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EOK );
      VSM_COMM_ERROR( rc, client_addr );
    }
    break;

    case VSS_IAVSYNC_EVENT_CLASS_RX:
    { /*  Need to Cache the client addr/port info . */
      if ( TRUE == session_object->avsync_client_rx_info.is_enabled )
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, 
             "VSM: LISTEN_IAVSYNC_CLASS_TX: Client has already registered" );

        rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EALREADY );
        VSM_COMM_ERROR( rc, client_addr );
        break;
      }

      session_object->avsync_client_rx_info.is_enabled = TRUE;
      session_object->avsync_client_rx_info.client_addr = apr_pkt->src_addr;
      session_object->avsync_client_rx_info.client_port = apr_pkt->src_port;

      if( VSM_STATE_ENUM_RUN == session_object->state_control.state )
      {
        ( void ) vsm_action_send_avsync_rx_delay_event( session_object );
      }

      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "VSM: LISTEN_IAVSYNC_CLASS_RX client's Addr(0x%04X) Port(0x%04X)",
             session_object->avsync_client_rx_info.client_addr,
             session_object->avsync_client_rx_info.client_port );

      rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EOK );
      VSM_COMM_ERROR( rc, client_addr );
    }
    break;

    case VSS_IAVSYNC_EVENT_CLASS_TX:
    { /*  Need to Cache the client addr/port info . */
      if ( TRUE == session_object->avsync_client_tx_info.is_enabled )
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VSM: LISTEN_IAVSYNC_CLASS_TX: Client has already registered." );

        rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EALREADY );
        VSM_COMM_ERROR( rc, client_addr );
        break;
      }

      session_object->avsync_client_tx_info.is_enabled = TRUE;
      session_object->avsync_client_tx_info.client_addr = apr_pkt->src_addr;
      session_object->avsync_client_tx_info.client_port = apr_pkt->src_port;

      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "VSM: LISTEN_IAVSYNC_CLASS_TX: client's Addr(0x%04X) Port(0x%04X)",
             session_object->avsync_client_tx_info.client_addr,
             session_object->avsync_client_tx_info.client_port );

      rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EOK );
      VSM_COMM_ERROR( rc, client_addr );
    }
    break;
      
    default:
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VSM_listen_for_event_class_cmd_ctrl(): Unsupported event class: 0x%08X",
             in_args->class_id );

      rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EUNSUPPORTED );
      VSM_COMM_ERROR( rc, client_addr );
    }
    break;
  }

  return ADSP_EOK;
}

static int32_t VSM_set_rx_dtmf_detection_cmd_control (
  vsm_session_object_t* session_object
)
{
  int32_t rc = ADSP_EOK;
  vss_istream_cmd_set_rx_dtmf_detection_t *p_in_args;
  elite_msg_any_t msg;
  uint32_t in_size;
  uint16_t client_addr;

  enum
  {
    VSM_ACTION_ENUM_UNINITIALIZED,
    VSM_ACTION_ENUM_SET_RX_DTMF_DETECT_CONFIG,
    VSM_ACTION_ENUM_WAIT_1,
    VSM_ACTION_ENUM_ERROR_HANDLE,
    VSM_ACTION_ENUM_REPORT_FAILURE,
    VSM_ACTION_ENUM_COMPLETE,
    VSM_ACTION_ENUM_INVALID
  };

   if( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  msg = session_object->cmd_control.cmd_msg;
  elite_apr_packet_t *apr_pkt = ( elite_apr_packet_t * )( session_object->cmd_control.cmd_msg.pPayload );
  client_addr = apr_pkt->src_addr;

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED, "VSM_set_rx_dtmf_detection_cmd_control(): "
         " session_portID = 0x%08X", apr_pkt->dst_port );

  session_object->cmd_control.is_cmd_pending = TRUE;
  while ( VSM_ACTION_ENUM_UNINITIALIZED == session_object->cmd_control.current_action )
  {
    p_in_args = APRV2_PKT_GET_PAYLOAD( vss_istream_cmd_set_rx_dtmf_detection_t, apr_pkt );
    in_size = APRV2_PKT_GET_PAYLOAD_BYTE_SIZE( apr_pkt->header );

    if( FALSE == vsm_utility_validate_apr_payload_size( apr_pkt ) )
    {
      session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
      break;
    }

    /* If the current rx dtmf detect status is same as the request,
          return already done. */
    if ( session_object->set_rx_dtmf_detect.enable_flag == p_in_args->enable )
    {
      session_object->cmd_control.cmd_result = ADSP_EALREADY;
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
      break;
    }

    /* Store the enable status. */
    session_object->set_rx_dtmf_detect.enable_flag = ( vsm_enable_state_enum_t )p_in_args->enable;

    /* If the request is to enable, save the client addr and port. */
    if ( session_object->set_rx_dtmf_detect.enable_flag )
    {
      session_object->set_rx_dtmf_detect.client_addr = apr_pkt->src_addr;
      session_object->set_rx_dtmf_detect.client_port = apr_pkt->src_port;
      session_object->set_rx_dtmf_detect.vsm_port = apr_pkt->dst_port;
    }
    else
    {
      /* If the request is to disable, check whether the client
         is same as the one that issued enable. */
      if ( session_object->set_rx_dtmf_detect.client_addr != apr_pkt->src_addr ||
           session_object->set_rx_dtmf_detect.client_port != apr_pkt->src_port )
      {
        session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
        session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
        break;
      }
    }

    if ( NULL == session_object->vdec )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_FATAL,"VSM_set_rx_dtmf_detection_cmd_control: vdec is NULL!!\n" );
      session_object->cmd_control.cmd_result = ADSP_ENOTREADY;
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
      break;
    }

    session_object->cmd_control.current_action = VSM_ACTION_ENUM_SET_RX_DTMF_DETECT_CONFIG;
    break;
  }

  for ( ; ; )
  {
    switch ( session_object->cmd_control.current_action )
    {
      case VSM_ACTION_ENUM_SET_RX_DTMF_DETECT_CONFIG:
      {
        rc = vsm_action_set_rx_dtmf_detect_config( session_object );
        if ( ADSP_FAILED( rc ) )
        {
          session_object->cmd_control.cmd_result = ADSP_EFAILED;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
          continue;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_WAIT_1;
          return ADSP_EOK;
        }
      }
      break;

      case VSM_ACTION_ENUM_WAIT_1:
      {
        if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
        {
          session_object->cmd_control.cmd_result = session_object->cmd_control.resp_result;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_COMPLETE;
        }
        continue;
      }
      break;

      case VSM_ACTION_ENUM_ERROR_HANDLE:
      {
        if ( TRUE == session_object->cmd_control.is_resp_pending )
        {
          return ADSP_EOK;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
          continue;
        }
      }
      break;

      case VSM_ACTION_ENUM_REPORT_FAILURE:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"VSM_set_rx_dtmf_detection_cmd_control: command FAILED:." );
        elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, session_object->cmd_control.cmd_result );
        session_object->cmd_control.is_cmd_pending = FALSE;
        return ADSP_EOK;
      }
      break;

      case VSM_ACTION_ENUM_COMPLETE:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,"VSM_set_rx_dtmf_detection_cmd_control: command Success:." );
        elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EOK );
        session_object->cmd_control.is_cmd_pending = FALSE;
        return ADSP_EOK;
      }
      break;

      default:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"VSM_set_rx_dtmf_detection_cmd_control: ERROR: Unexpected..." );
      }
      return ADSP_EOK;
    }
  }

  return ADSP_EOK;
}

static int32_t VSM_set_dtmf_generation_cmd_control (
  vsm_session_object_t* session_object
)
{
  int32_t rc = ADSP_EOK;
  vss_istream_cmd_set_dtmf_generation_t *p_in_args;
  elite_msg_any_t msg;
  uint16_t client_addr;

   if( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  msg = session_object->cmd_control.cmd_msg;
  elite_apr_packet_t *apr_pkt = ( elite_apr_packet_t * )( session_object->cmd_control.cmd_msg.pPayload );
  client_addr = apr_pkt->src_addr;

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED, "VSM_set_dtmf_generation_cmd_control(): "
         " session_portID = 0x%08X", apr_pkt->dst_port );

  p_in_args = APRV2_PKT_GET_PAYLOAD( vss_istream_cmd_set_dtmf_generation_t, apr_pkt );

  if( FALSE == vsm_utility_validate_apr_payload_size( apr_pkt ) )
  {
    rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EBADPARAM );
    VSM_COMM_ERROR( rc, client_addr );
    return ADSP_EOK;
  }

  session_object->set_dtmf_gen.client_addr = apr_pkt->src_addr;
  session_object->set_dtmf_gen.client_port = apr_pkt->src_port;
  session_object->set_dtmf_gen.vsm_port = apr_pkt->dst_port;

  MSG_3( MSG_SSID_DFLT, MSG_LEGACY_MED, "VSM_set_dtmf_generation_cmd_control(): direction: %d, mixing: %d, tone1: %d",
                                        p_in_args->direction,
                                        p_in_args->mix_flag,
                                        p_in_args->tone_1 );
  MSG_3( MSG_SSID_DFLT, MSG_LEGACY_MED, "VSM_set_dtmf_generation_cmd_control(): tone2: %d, gain: %d, duration: %d",
                                        p_in_args->tone_2,
                                        p_in_args->gain,
                                        p_in_args->duration );

  if ( NULL != session_object->venc )
  {
    if ( ADSP_FAILED( rc = qurt_elite_queue_push_back( session_object->venc->cmdQ, ( uint64_t* )&msg ) ) )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_FATAL,"VSM_set_dtmf_generation_cmd_control:"
             " qurt_elite_queue_push_back error = %d!!\n",( int )rc );
      rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EFAILED );
      VSM_COMM_ERROR( rc, client_addr );
    }
  }
  else
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_FATAL,"VSM_set_dtmf_generation_cmd_control: venc is NULL!!\n" );
    rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_ENOTREADY );
    VSM_COMM_ERROR( rc, client_addr );
  }

  MSG( MSG_SSID_DFLT, MSG_LEGACY_MED, "VSM_set_dtmf_generation_cmd_control: command Success:." );
  return ADSP_EOK;
}

static int32_t VSM_set_param_v2_cmd_control (
  vsm_session_object_t* session_object
)
{
  int32_t rc = ADSP_EOK;
  vss_icommon_cmd_set_param_v2_t* payload;
  uint32_t payload_size;
  uint32_t expected_payload_size;
  uint8_t* param_data = NULL;
  cvd_virt_addr_t param_data_virt_addr;
  elite_mem_shared_memory_map_t mem_shared_memory_map = {0};
  uint32_t payload_address = 0;
  voice_set_param_v2_t set_param;
  uint32_t vsm_mem_handle;
  uint16_t client_addr;
  uint32_t param_id;
  uint16_t param_size;
  uint8_t* param_payload_data;
enum
  {
    VSM_ACTION_ENUM_UNINITIALIZED,
    VSM_ACTION_ENUM_SET_PARAM_1,
    VSM_ACTION_ENUM_WAIT_2,
    VSM_ACTION_ENUM_GET_KPPS_3,
    VSM_ACTION_ENUM_WAIT_4,
    VSM_ACTION_ENUM_SEND_RECONFIG_EVENT_5,
    VSM_ACTION_ENUM_ERROR_HANDLE,
    VSM_ACTION_ENUM_REPORT_FAILURE,
    VSM_ACTION_ENUM_COMPLETE,
    VSM_ACTION_ENUM_INVALID
  };

  if( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  elite_apr_packet_t *apr_pkt = ( elite_apr_packet_t * )( session_object->cmd_control.cmd_msg.pPayload );
  client_addr = apr_pkt->src_addr;

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "VSM_set_param_v2_cmd_control():  session_portID = 0x%08X,"
         " current_action %d", apr_pkt->dst_port, session_object->cmd_control.current_action );

  session_object->cmd_control.is_cmd_pending = TRUE;
  payload = APRV2_PKT_GET_PAYLOAD( vss_icommon_cmd_set_param_v2_t, apr_pkt );
  while ( VSM_ACTION_ENUM_UNINITIALIZED == session_object->cmd_control.current_action )
  {
    payload_size = APRV2_PKT_GET_PAYLOAD_BYTE_SIZE( apr_pkt->header );

    if ( payload_size < sizeof( vss_icommon_cmd_set_param_v2_t ) )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_set_param_v2_cmd_control(): Unexpected payload size, %d < %d",
                                              payload_size,
                                              sizeof( vss_icommon_cmd_set_param_v2_t ) );

      session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
      break;
    }

    if ( FALSE == vsm_utility_verify_param_data_size_aligned( payload->mem_size ) )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_set_param_v2_cmd_control(): Invalid param data size, %d",
                                              payload->mem_size );
      session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
      break;
    }

    if ( payload->mem_handle == 0 )
    { /* Parameter data is in-band. */
      expected_payload_size = ( sizeof( vss_icommon_cmd_set_param_v2_t ) + payload->mem_size );
      if ( payload_size != expected_payload_size )
      {
        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_set_param_v2_cmd_control(): Unexpected payload size, %d != %d",
                                                payload_size,
                                                expected_payload_size );
        session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
        session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
        break;
      }

      param_data = ( ( uint8_t* ) payload );
      param_data += sizeof( vss_icommon_cmd_set_param_v2_t );
      vsm_mem_handle = 0;

      param_id = *( uint32_t* ) ( param_data +  sizeof( uint32_t ) );
      param_size = *( uint32_t* ) ( param_data + ( 2 * ( sizeof( uint32_t ) ) ) );
      param_payload_data = ( param_data + ( 3 * ( sizeof( uint32_t ) ) ) );

      payload_address = ( uint32_t )param_data;

      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED, " VSM_set_param_v2_cmd_control(): Param ID[0]=0x%08x",
                                            param_id );

      if ( param_size >= 16 )
      {
        MSG_3( MSG_SSID_DFLT, MSG_LEGACY_MED, " VSM_set_param_v2_cmd_control(): Payload[0-2]=0x%08x, " \
                                              "0x%08x, 0x%08x",
                                              *( uint32_t* ) ( param_payload_data + sizeof( voice_param_data_t ) ), 
                                              *( uint32_t* ) ( param_payload_data + sizeof( voice_param_data_t ) + sizeof( uint32_t ) ), 
                                              *( uint32_t* ) ( param_payload_data + sizeof( voice_param_data_t ) + ( 2 * sizeof( uint32_t ) ) ) );
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED, " VSM_set_param_v2_cmd_control(): Payload[3]=0x%08x",
                                              *( uint32_t* ) ( param_payload_data + sizeof( voice_param_data_t ) + ( 3 * sizeof( uint32_t ) ) ) );
      }
      else if ( param_size >= 4 )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED, " VSM_set_param_v2_cmd_control(): Payload[0]=0x%08x",
                                              *( uint32_t* ) ( param_payload_data + sizeof( voice_param_data_t ) ) );
      }
    }
    else
    { /* Parameter data is out-of-band. */
      rc = cvd_mem_mapper_validate_handle( payload->mem_handle );
      if ( rc )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_set_param_v2_cmd_control(): Invalid mem_handle: 0x%08X.",
                                                payload->mem_handle );
        session_object->cmd_control.cmd_result = ADSP_EHANDLE;
        session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
        break;
      }

      ( void ) cvd_mem_mapper_get_vsm_mem_handle( payload->mem_handle, &vsm_mem_handle );

      rc = cvd_mem_mapper_validate_attributes_align( payload->mem_handle, payload->mem_address );
      if ( rc )
      {
        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_set_param_v2_cmd_control(): Mis-aligned mem address:"
               " lsw: 0x%08X, msw: 0x%08X", ( uint32_t )payload->mem_address, ( uint32_t )( payload->mem_address >> 32 ) );
        session_object->cmd_control.cmd_result = ADSP_EHANDLE;
        session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
        break;
      }

      rc = cvd_mem_mapper_validate_mem_is_in_region( payload->mem_handle, payload->mem_address,
                                                     payload->mem_size );
      if ( rc )
      {
        MSG_4( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_set_param_v2_cmd_control(): Memory is not within range,"
               " mem_handle: 0x%08X, addr: lsw: 0x%08X, msw: 0x%08X, size: %d", payload->mem_handle, ( uint32_t )payload->mem_address,
               ( uint32_t )( payload->mem_address >> 32 ), payload->mem_size );
        session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
        session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
        break;
      }

      rc = cvd_mem_mapper_get_virtual_addr_v2( payload->mem_handle, payload->mem_address,
                                               &param_data_virt_addr );
      if ( rc )
      {
        MSG_3( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_set_param_v2_cmd_control(): Cannot get virtual address,"
               " mem_handle: 0x%08X, addr: lsw: 0x%08X, msw: 0x%08X", payload->mem_handle,
               ( uint32_t )payload->mem_address, ( uint32_t )( payload->mem_address >> 32 ) );
        session_object->cmd_control.cmd_result = ADSP_EFAILED;
        session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
        break;
      }

      param_id = *( uint32_t* ) ( ( uint8_t* ) param_data_virt_addr.ptr +  sizeof( uint32_t ) );
      param_size = *( uint32_t* ) ( ( uint8_t* ) param_data_virt_addr.ptr + ( 2 * ( sizeof( uint32_t ) ) ) );
      param_payload_data = ( ( uint8_t* ) param_data_virt_addr.ptr + ( 3 * ( sizeof( uint32_t ) ) ) );

      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED, " VSM_set_param_v2_cmd_control(): Param ID[0]=0x%08x", param_id );

      if ( param_size >= 16 )
      {
        MSG_3( MSG_SSID_DFLT, MSG_LEGACY_MED, " VSM_set_param_v2_cmd_control(): Payload[0-2]=0x%08x, " \
                                              "0x%08x, 0x%08x",
                                              *( uint32_t* ) ( param_payload_data + sizeof( voice_param_data_t ) ), 
                                              *( uint32_t* ) ( param_payload_data + sizeof( voice_param_data_t ) + sizeof( uint32_t ) ), 
                                              *( uint32_t* ) ( param_payload_data + sizeof( voice_param_data_t ) + ( 2 * sizeof( uint32_t ) ) ) );

        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED, " VSM_set_param_v2_cmd_control(): Payload[3]=0x%08x",
                                              *( uint32_t* ) ( param_payload_data + sizeof( voice_param_data_t ) + ( 3 * sizeof( uint32_t ) ) ) );
      }
      else if ( param_size >= 4 )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED, " VSM_set_param_v2_cmd_control(): Payload[0]=0x%08x",
                                              *( uint32_t* ) ( param_payload_data + sizeof( voice_param_data_t ) ) );
      }
      // TBD: Can remove this utility call.
      cvd_mem_mapper_set_virt_addr_to_uint32( &set_param.payload_address_msw, &set_param.payload_address_lsw,
                                              param_data_virt_addr.ptr );

      if ( NULL == vsm_memory_map_client )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, " VSM_set_param_v2_cmd_control(): memory mapping not done. vsm_memory_map_client: %d",
                                              vsm_memory_map_client );
        session_object->cmd_control.cmd_result = ADSP_EFAILED;
        session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
        break;
      }
      mem_shared_memory_map.unMemMapClient = vsm_memory_map_client;
      mem_shared_memory_map.unMemMapHandle = vsm_mem_handle;
      rc = elite_mem_map_get_shm_attrib( set_param.payload_address_lsw,
                                         set_param.payload_address_msw, 
                                         payload->mem_size, &mem_shared_memory_map);
      payload_address = mem_shared_memory_map.unVirtAddr;

    }

#if defined(__qdsp6__) && !defined(SIM)
    int8_t *bufptr[4] = { (int8_t *) payload_address, NULL, NULL, NULL };
    voice_log_buffer( bufptr,
                      VOICE_LOG_CHAN_VSM_SET_PARAM,
                      session_object->apr_info.self_port,
                      qurt_elite_timer_get_time(),
                      VOICE_LOG_DATA_FORMAT_PCM_MONO,
                      1,   /* dummy value */
                      payload->mem_size,
                      NULL );
#endif

    session_object->cmd_control.current_action = VSM_ACTION_ENUM_SET_PARAM_1;
    break;

  }

  for ( ; ; )
  {
    switch ( session_object->cmd_control.current_action )
    {
      case VSM_ACTION_ENUM_SET_PARAM_1:
      {
        rc = vsm_action_set_param( session_object, ( int32_t* )payload_address, payload->mem_size, CVD_CAL_PARAM_MINOR_VERSION_0 );
        if ( ADSP_FAILED( rc ) )
        {
          session_object->cmd_control.cmd_result = ADSP_EFAILED;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
          continue;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_WAIT_2;
          return ADSP_EOK;
        }
      }
      break;

      case VSM_ACTION_ENUM_WAIT_2:
      {
        if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
        {
          MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_set_param_v2_cmd_control():"
                 " set_param error: %d.", session_object->cmd_control.resp_result );
        }

        if ( ( VSM_STATE_ENUM_RUN == session_object->state_control.state ) &&
             ( APR_NULL_V != session_object->attached_mvm_handle ) )
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_GET_KPPS_3;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_COMPLETE;
        }
        continue;
      }
      break;

      case VSM_ACTION_ENUM_GET_KPPS_3:
      {
        rc = vsm_action_get_kpps( session_object );
        if ( ADSP_FAILED( rc ) )
        {
          MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"VSM_set_param_v2_cmd_control: GET KPPS failed...." );
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
          continue;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_WAIT_4;
          return ADSP_EOK;
        }
      }
      break;

      case VSM_ACTION_ENUM_WAIT_4:
      {
        if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
        {
          MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"VSM_set_param_v2_cmd_control: GET KPPS failed...." );
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
        }
        else
        {
          if ( TRUE == session_object->is_kpps_changed )
          {
            session_object->cmd_control.current_action = VSM_ACTION_ENUM_SEND_RECONFIG_EVENT_5;
          }
          else
          {
            session_object->cmd_control.current_action = VSM_ACTION_ENUM_COMPLETE;
          }
        }
        continue;
      }
      break;

      case VSM_ACTION_ENUM_SEND_RECONFIG_EVENT_5:
      {
        rc = vsm_action_send_reconfig_event( session_object );
        VSM_COMM_ERROR( rc, client_addr );
        session_object->is_kpps_changed = FALSE;
        session_object->cmd_control.current_action = VSM_ACTION_ENUM_COMPLETE;
        continue;
      }
      break;

      case VSM_ACTION_ENUM_ERROR_HANDLE:
      {
        if ( TRUE == session_object->cmd_control.is_resp_pending )
        {
          return ADSP_EOK;
        }
        else
        {
          if ( ADSP_FAILED( session_object->cmd_control.cmd_result ) )
          {
            session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
          }
          else
          {
            session_object->cmd_control.current_action = VSM_ACTION_ENUM_COMPLETE;
          }
          continue;
        }
      }
      break;

      case VSM_ACTION_ENUM_REPORT_FAILURE:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"VSM_set_param_v2_cmd_control: command FAILED:." );
        elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, session_object->cmd_control.cmd_result );
        session_object->cmd_control.is_cmd_pending = FALSE;
        return ADSP_EOK;
      }
      break;

      case VSM_ACTION_ENUM_COMPLETE:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,"VSM_set_param_v2_cmd_control: command Success:." );
        elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EOK );
        session_object->cmd_control.is_cmd_pending = FALSE;
        return ADSP_EOK;
      }
      break;

      default:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"VSM_set_param_v2_cmd_control: ERROR: Unexpected..." );
      }
      return ADSP_EOK;
    }
  }

  return ADSP_EOK;
}


static int32_t VSM_set_param_v3_cmd_control (
  vsm_session_object_t* session_object
)
{
  int32_t rc = ADSP_EOK;
  vss_icommon_cmd_set_param_v3_t* payload;
  uint32_t payload_size;
  uint32_t expected_payload_size;
  uint8_t* param_data = NULL;
  cvd_virt_addr_t param_data_virt_addr;
  elite_mem_shared_memory_map_t mem_shared_memory_map = {0};
  uint32_t payload_address = 0;
  voice_set_param_v2_t set_param;
  uint32_t vsm_mem_handle;
  uint16_t client_addr;
  uint32_t param_id;
  uint16_t param_size;
  uint8_t* param_payload_data;
  uint16_t instance_id;
enum
  {
    VSM_ACTION_ENUM_UNINITIALIZED,
    VSM_ACTION_ENUM_SET_PARAM_1,
    VSM_ACTION_ENUM_WAIT_2,
    VSM_ACTION_ENUM_GET_KPPS_3,
    VSM_ACTION_ENUM_WAIT_4,
    VSM_ACTION_ENUM_SEND_RECONFIG_EVENT_5,
    VSM_ACTION_ENUM_ERROR_HANDLE,
    VSM_ACTION_ENUM_REPORT_FAILURE,
    VSM_ACTION_ENUM_COMPLETE,
    VSM_ACTION_ENUM_INVALID
  };
  #define INSTANCE_ID_OFFSET ( sizeof( uint32_t ) )
  #define PARAM_ID_OFFSET ( 2 * sizeof( uint32_t ) )
  #define PARAM_SIZE_OFFSET ( 3 * sizeof( uint32_t ) )
  #define PARAM_DATA_OFFSET ( 4 * sizeof( uint32_t ) )

  if( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  elite_apr_packet_t *apr_pkt = ( elite_apr_packet_t * )( session_object->cmd_control.cmd_msg.pPayload );
  client_addr = apr_pkt->src_addr;

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "VSM_set_param_v3_cmd_control():  session_portID = 0x%08X,"
         " current_action %d", apr_pkt->dst_port, session_object->cmd_control.current_action );

  session_object->cmd_control.is_cmd_pending = TRUE;
  payload = APRV2_PKT_GET_PAYLOAD( vss_icommon_cmd_set_param_v3_t, apr_pkt );
  while ( VSM_ACTION_ENUM_UNINITIALIZED == session_object->cmd_control.current_action )
  {
    payload_size = APRV2_PKT_GET_PAYLOAD_BYTE_SIZE( apr_pkt->header );

    if ( payload_size < sizeof( vss_icommon_cmd_set_param_v3_t ) )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_set_param_v3_cmd_control(): Unexpected payload size, %d < %d",
                                              payload_size,
                                              sizeof( vss_icommon_cmd_set_param_v3_t ) );

      session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
      break;
    }

    if ( FALSE == vsm_utility_verify_param_data_size_aligned( payload->mem_size ) )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_set_param_v3_cmd_control(): Invalid param data size, %d",
                                              payload->mem_size );
      session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
      break;
    }

    if ( payload->mem_handle == 0 )
    { /* Parameter data is in-band. */
      expected_payload_size = ( sizeof( vss_icommon_cmd_set_param_v3_t ) + payload->mem_size );
      if ( payload_size != expected_payload_size )
      {
        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_set_param_v3_cmd_control(): Unexpected payload size, %d != %d",
                                                payload_size,
                                                expected_payload_size );
        session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
        session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
        break;
      }

      param_data = ( ( uint8_t* ) payload );
      param_data += sizeof( vss_icommon_cmd_set_param_v3_t );
      vsm_mem_handle = 0;

      instance_id = *( uint16_t* ) ( param_data +  INSTANCE_ID_OFFSET );
      param_id = *( uint32_t* ) ( param_data +  PARAM_ID_OFFSET );
      param_size = *( uint32_t* ) ( param_data + PARAM_SIZE_OFFSET );
      param_payload_data = ( param_data + PARAM_DATA_OFFSET );

      payload_address = ( uint32_t )param_data;

      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, " VSM_set_param_v3_cmd_control(): Param ID[0]=0x%08x Instance ID[0]=0x%08x",
                                            param_id, instance_id );

      if ( param_size >= 16 )
      {
        MSG_3( MSG_SSID_DFLT, MSG_LEGACY_MED, " VSM_set_param_v3_cmd_control(): Payload[0-2]=0x%08x, " \
                                              "0x%08x, 0x%08x",
                                              *( uint32_t* ) ( param_payload_data  ), 
                                              *( uint32_t* ) ( ( param_payload_data  ) + sizeof( uint32_t ) ) , 
                                              *( uint32_t* ) ( ( param_payload_data  + ( 2 * sizeof( uint32_t ) ) ) ) );

        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED, " VSM_set_param_v3_cmd_control(): Payload[3]=0x%08x",
                                              *( uint32_t* ) ( param_payload_data + ( 3 * sizeof( uint32_t ) ) ) );
      }
      else if ( param_size >= 4 )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED, " VSM_set_param_v3_cmd_control(): Payload[0]=0x%08x",
                                              *( uint32_t* ) ( param_payload_data + sizeof( voice_param_data_v2_t ) ) );
      }
    }
    else
    { /* Parameter data is out-of-band. */
      rc = cvd_mem_mapper_validate_handle( payload->mem_handle );
      if ( rc )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_set_param_v3_cmd_control(): Invalid mem_handle: 0x%08X.",
                                                payload->mem_handle );
        session_object->cmd_control.cmd_result = ADSP_EHANDLE;
        session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
        break;
      }

      ( void ) cvd_mem_mapper_get_vsm_mem_handle( payload->mem_handle, &vsm_mem_handle );

      rc = cvd_mem_mapper_validate_attributes_align( payload->mem_handle, payload->mem_address );
      if ( rc )
      {
        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_set_param_v3_cmd_control(): Mis-aligned mem address:"
               " lsw: 0x%08X, msw: 0x%08X", ( uint32_t )payload->mem_address, ( uint32_t )( payload->mem_address >> 32 ) );
        session_object->cmd_control.cmd_result = ADSP_EHANDLE;
        session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
        break;
      }

      rc = cvd_mem_mapper_validate_mem_is_in_region( payload->mem_handle, payload->mem_address,
                                                     payload->mem_size );
      if ( rc )
      {
        MSG_4( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_set_param_v3_cmd_control(): Memory is not within range,"
               " mem_handle: 0x%08X, addr: lsw: 0x%08X, msw: 0x%08X, size: %d", payload->mem_handle, ( uint32_t )payload->mem_address,
               ( uint32_t )( payload->mem_address >> 32 ), payload->mem_size );
        session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
        session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
        break;
      }

      rc = cvd_mem_mapper_get_virtual_addr_v2( payload->mem_handle, payload->mem_address,
                                               &param_data_virt_addr );
      if ( rc )
      {
        MSG_3( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_set_param_v3_cmd_control(): Cannot get virtual address,"
               " mem_handle: 0x%08X, addr: lsw: 0x%08X, msw: 0x%08X", payload->mem_handle,
               ( uint32_t )payload->mem_address, ( uint32_t )( payload->mem_address >> 32 ) );
        session_object->cmd_control.cmd_result = ADSP_EFAILED;
        session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
        break;
      }

      instance_id = *( uint16_t* ) ( ( uint8_t* ) param_data_virt_addr.ptr + INSTANCE_ID_OFFSET );
      param_id = *( uint32_t* ) ( ( uint8_t* ) param_data_virt_addr.ptr + PARAM_ID_OFFSET );
      param_size = *( uint32_t* ) ( ( uint8_t* ) param_data_virt_addr.ptr + PARAM_SIZE_OFFSET ) ;
      param_payload_data = ( ( uint8_t* ) param_data_virt_addr.ptr + PARAM_DATA_OFFSET );

      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, " VSM_set_param_v3_cmd_control(): Param ID[0]=0x%08x Instance ID[0]=0x%08x", 
                                             param_id, instance_id );

      if ( param_size >= 16 )
      {
        MSG_3( MSG_SSID_DFLT, MSG_LEGACY_MED, " VSM_set_param_v3_cmd_control(): Payload[0-2]=0x%08x, " \
                                              "0x%08x, 0x%08x",
                                              *( uint32_t* ) ( param_payload_data  ), 
                                              *( uint32_t* ) ( ( param_payload_data  ) + sizeof( uint32_t ) ) , 
                                              *( uint32_t* ) ( ( param_payload_data  + ( 2 * sizeof( uint32_t ) ) ) ) );

        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED, " VSM_set_param_v3_cmd_control(): Payload[3]=0x%08x",
                                              *( uint32_t* ) ( param_payload_data + ( 3 * sizeof( uint32_t ) ) ) );
      }
      else if ( param_size >= 4 )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED, " VSM_set_param_v3_cmd_control(): Payload[0]=0x%08x",
                                              *( uint32_t* ) ( param_payload_data ) );
      }
      // TBD: Can remove this utility call.
      cvd_mem_mapper_set_virt_addr_to_uint32( &set_param.payload_address_msw, &set_param.payload_address_lsw,
                                              param_data_virt_addr.ptr );

      if ( NULL == vsm_memory_map_client )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, " VSM_set_param_v3_cmd_control(): memory mapping not done. vsm_memory_map_client: %d",
                                              vsm_memory_map_client );
        session_object->cmd_control.cmd_result = ADSP_EFAILED;
        session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
        break;
      }
      mem_shared_memory_map.unMemMapClient = vsm_memory_map_client;
      mem_shared_memory_map.unMemMapHandle = vsm_mem_handle;
      rc = elite_mem_map_get_shm_attrib( set_param.payload_address_lsw,
                                         set_param.payload_address_msw, 
                                         payload->mem_size, &mem_shared_memory_map);
      payload_address = mem_shared_memory_map.unVirtAddr;

    }

#if defined(__qdsp6__) && !defined(SIM)
    int8_t *bufptr[4] = { (int8_t *) payload_address, NULL, NULL, NULL };
    voice_log_buffer( bufptr,
                      VOICE_LOG_CHAN_VSM_SET_PARAM_V2,
                      session_object->apr_info.self_port,
                      qurt_elite_timer_get_time(),
                      VOICE_LOG_DATA_FORMAT_PCM_MONO,
                      1,   /* dummy value */
                      payload->mem_size,
                      NULL );
#endif

    session_object->cmd_control.current_action = VSM_ACTION_ENUM_SET_PARAM_1;
    break;

  }

  for ( ; ; )
  {
    switch ( session_object->cmd_control.current_action )
    {
      case VSM_ACTION_ENUM_SET_PARAM_1:
      {
        rc = vsm_action_set_param( session_object, ( int32_t* )payload_address, payload->mem_size, CVD_CAL_PARAM_MINOR_VERSION_1 );
        if ( ADSP_FAILED( rc ) )
        {
          session_object->cmd_control.cmd_result = ADSP_EFAILED;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
          continue;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_WAIT_2;
          return ADSP_EOK;
        }
      }
      break;

      case VSM_ACTION_ENUM_WAIT_2:
      {
        if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
        {
          MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_set_param_v3_cmd_control():"
                 " set_param error: %d.", session_object->cmd_control.resp_result );
        }

        if ( ( VSM_STATE_ENUM_RUN == session_object->state_control.state ) &&
             ( APR_NULL_V != session_object->attached_mvm_handle ) )
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_GET_KPPS_3;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_COMPLETE;
        }
        continue;
      }
      break;

      case VSM_ACTION_ENUM_GET_KPPS_3:
      {
        rc = vsm_action_get_kpps( session_object );
        if ( ADSP_FAILED( rc ) )
        {
          MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"VSM_set_param_v3_cmd_control: GET KPPS failed...." );
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
          continue;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_WAIT_4;
          return ADSP_EOK;
        }
      }
      break;

      case VSM_ACTION_ENUM_WAIT_4:
      {
        if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
        {
          MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"VSM_set_param_v3_cmd_control: GET KPPS failed...." );
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
        }
        else
        {
          if ( TRUE == session_object->is_kpps_changed )
          {
            session_object->cmd_control.current_action = VSM_ACTION_ENUM_SEND_RECONFIG_EVENT_5;
          }
          else
          {
            session_object->cmd_control.current_action = VSM_ACTION_ENUM_COMPLETE;
          }
        }
        continue;
      }
      break;

      case VSM_ACTION_ENUM_SEND_RECONFIG_EVENT_5:
      {
        rc = vsm_action_send_reconfig_event( session_object );
        VSM_COMM_ERROR( rc, client_addr );
        session_object->is_kpps_changed = FALSE;
        session_object->cmd_control.current_action = VSM_ACTION_ENUM_COMPLETE;
        continue;
      }
      break;

      case VSM_ACTION_ENUM_ERROR_HANDLE:
      {
        if ( TRUE == session_object->cmd_control.is_resp_pending )
        {
          return ADSP_EOK;
        }
        else
        {
          if ( ADSP_FAILED( session_object->cmd_control.cmd_result ) )
          {
            session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
          }
          else
          {
            session_object->cmd_control.current_action = VSM_ACTION_ENUM_COMPLETE;
          }
          continue;
        }
      }
      break;

      case VSM_ACTION_ENUM_REPORT_FAILURE:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"VSM_set_param_v3_cmd_control: command FAILED:." );
        elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, session_object->cmd_control.cmd_result );
        session_object->cmd_control.is_cmd_pending = FALSE;
        return ADSP_EOK;
      }
      break;

      case VSM_ACTION_ENUM_COMPLETE:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,"VSM_set_param_v3_cmd_control: command Success:." );
        elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EOK );
        session_object->cmd_control.is_cmd_pending = FALSE;
        return ADSP_EOK;
      }
      break;

      default:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"VSM_set_param_v3_cmd_control: ERROR: Unexpected..." );
      }
      return ADSP_EOK;
    }
  }

  #undef INSTANCE_ID_OFFSET
  #undef PARAM_ID_OFFSET 
  #undef PARAM_SIZE_OFFSET
  #undef PARAM_DATA_OFFSET

  return ADSP_EOK;
}

static int32_t VSM_get_param_v2_cmd_control (
  vsm_session_object_t* session_object
)
{
  int32_t rc = ADSP_EOK;
  vss_icommon_cmd_get_param_v2_t* payload;
  uint32_t payload_size;
  uint64_t param_data_virt_addr = 0;
  voice_get_param_v2_t get_param;
  uint32_t vsm_mem_handle;
  uint16_t client_addr;
  qurt_elite_memorymap_mapping_mode_t mapping_mode;
  uint32_t alignment_check;
  int32_t *get_param_ptr;
  voice_param_data_t* param_data_ptr;
  bool_t is_inband = FALSE;

  enum
  {
    VSM_ACTION_ENUM_UNINITIALIZED,
    VSM_ACTION_ENUM_GET_PARAM,
    VSM_ACTION_ENUM_WAIT_1,
    VSM_ACTION_ENUM_ERROR_HANDLE,
    VSM_ACTION_ENUM_REPORT_FAILURE,
    VSM_ACTION_ENUM_COMPLETE,
    VSM_ACTION_ENUM_INVALID
  };

   if( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  elite_apr_packet_t *apr_pkt = ( elite_apr_packet_t * )( session_object->cmd_control.cmd_msg.pPayload );
  client_addr = apr_pkt->src_addr;

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "VSM_get_param_v2_cmd_control():  session_portID = 0x%08X,"
         " current_action %d", apr_pkt->dst_port, session_object->cmd_control.current_action );

  session_object->cmd_control.is_cmd_pending = TRUE;
  while ( VSM_ACTION_ENUM_UNINITIALIZED == session_object->cmd_control.current_action )
  {
    if( FALSE == vsm_utility_validate_apr_payload_size( apr_pkt ) )
    {
      session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
      break;
    }

    payload = APRV2_PKT_GET_PAYLOAD( vss_icommon_cmd_get_param_v2_t, apr_pkt );
    if ( FALSE == vsm_utility_verify_param_data_size_aligned( payload->mem_size ) )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_get_param_v2_cmd_control(): Invalid param data size, %d",
                                              payload->mem_size );
      session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
      break;
    }

    if ( payload->mem_handle == 0 )
    {
      /* Parameter data is in-band. */
      vsm_mem_handle = 0;
      is_inband = TRUE;

     /* Allocate Memory for in-band operation. */
      get_param_ptr = ( int32_t* )qurt_elite_memory_malloc(
                                     payload->mem_size, QURT_ELITE_HEAP_DEFAULT );
      if ( NULL == get_param_ptr )
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VSM_get_param_v2_cmd_control: failed to allocate memory for get_param payload..." );
        session_object->cmd_control.cmd_result = ADSP_ENORESOURCE;
        session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
        break;
      }
    }
    else
    {
      /* Parameter data is out-of-band. */
      is_inband = FALSE;

      rc = cvd_mem_mapper_validate_handle( payload->mem_handle );
      if ( rc )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_get_param_v2_cmd_control(): Invalid mem_handle: 0x%08X.",
                                                payload->mem_handle );
        session_object->cmd_control.cmd_result = ADSP_EHANDLE;
        session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
        break;
      }

      ( void ) cvd_mem_mapper_get_vsm_mem_handle( payload->mem_handle, &vsm_mem_handle );

      rc = qurt_elite_memorymap_get_mapping_mode(vsm_memory_map_client, vsm_mem_handle, &mapping_mode);
      if( ADSP_FAILED( rc ) )
      {
        session_object->cmd_control.cmd_result = ADSP_EHANDLE;
        session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
        break;
      }

      alignment_check = 32;

      rc = cvd_mem_mapper_validate_attributes_align( payload->mem_handle, payload->mem_address );
      if ( rc )
      {
        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_get_param_v2_cmd_control(): Mis-aligned mem address:"
          " lsw: 0x%08X, msw: 0x%08X", ( uint32_t )payload->mem_address,
          ( uint32_t )( payload->mem_address >> 32 ) );
        session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
        session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
        break;
      }

      rc = cvd_mem_mapper_validate_attributes_align( payload->mem_handle, payload->mem_size );
      if ( rc )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_get_param_v2_cmd_control(): Mis-aligend mem size: 0x%08X",
                                                payload->mem_size );
        session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
        session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
        break;
      }

      rc = cvd_mem_mapper_validate_mem_is_in_region( payload->mem_handle, payload->mem_address,
                                                     payload->mem_size );
      if ( rc )
      {
        MSG_4( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
          "VSM_get_param_v2_cmd_control(): Memory is not within range,"
          " mem_handle: 0x%08X, addr: lsw: 0x%08X, msw: 0x%08X, size: %d",
          payload->mem_handle, ( uint32_t )payload->mem_address,
          ( uint32_t )( payload->mem_address >> 32 ), payload->mem_size );
        session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
        session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
        break;
      }

      rc = cvd_mem_mapper_get_virtual_addr( payload->mem_handle, payload->mem_address,
                                            &param_data_virt_addr );
      if ( rc )
      {
        MSG_3( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
          "VSM_get_param_v2_cmd_control(): Cannot get virtual address,"
          " mem_handle: 0x%08X, addr: lsw: 0x%08X, msw: 0x%08X", payload->mem_handle,
          ( uint32_t )payload->mem_address, ( uint32_t )( payload->mem_address >> 32 ) );
        session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
        session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
        break;
      }

      rc = voice_cmn_check_align_size( ( uint32_t ) param_data_virt_addr, 
              ( uint32_t ) ( param_data_virt_addr >> 32 ),
                payload->mem_size, alignment_check );
      if( ADSP_FAILED( rc ) )
      {
        session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
        session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
        break;
      }

      elite_mem_shared_memory_map_t memmap = {0};
      memmap.unMemMapClient = vsm_memory_map_client;
      memmap.unMemMapHandle = vsm_mem_handle;
      rc = elite_mem_map_get_shm_attrib( ( uint32_t ) param_data_virt_addr,
                                         ( uint32_t ) ( param_data_virt_addr >> 32 ),
                                          payload->mem_size, &memmap );
      if( ADSP_FAILED( rc ) )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"VSM_get_param_v2_cmd_control: Error! Failed to get virtual address with result %d!", rc );
        session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
        session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
        break;
      }
      get_param_ptr = ( int32_t* )memmap.unVirtAddr;
    }

    param_data_ptr = ( voice_param_data_t* )get_param_ptr;
    param_data_ptr->module_id = payload->module_id;
    param_data_ptr->param_id = payload->param_id;
    param_data_ptr->param_size = payload->mem_size;
    param_data_ptr->reserved = 0;

    session_object->cmd_control.current_action = VSM_ACTION_ENUM_GET_PARAM;
    break;

  }

  for ( ; ; )
  {
    switch ( session_object->cmd_control.current_action )
    {
      case VSM_ACTION_ENUM_GET_PARAM:
      {
        rc = vsm_action_get_param( session_object, get_param_ptr, is_inband, CVD_CAL_PARAM_MINOR_VERSION_0 );
        if ( ADSP_FAILED( rc ) )
        {
          session_object->cmd_control.cmd_result = ADSP_EFAILED;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
          continue;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_WAIT_1;
          return ADSP_EOK;
        }
      }
      break;

      case VSM_ACTION_ENUM_WAIT_1:
      {
        if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
        {
          session_object->cmd_control.cmd_result = session_object->cmd_control.resp_result;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_COMPLETE;
        }
        continue;
      }
      break;

      case VSM_ACTION_ENUM_ERROR_HANDLE:
      {
        if ( TRUE == session_object->cmd_control.is_resp_pending )
        {
          return ADSP_EOK;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
          continue;
        }
      }
      break;

      case VSM_ACTION_ENUM_REPORT_FAILURE:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"VSM_get_param_v2_cmd_control: command FAILED:." );
        elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, session_object->cmd_control.cmd_result );
        session_object->cmd_control.is_cmd_pending = FALSE;
        return ADSP_EOK;
      }
      break;

      case VSM_ACTION_ENUM_COMPLETE:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,"VSM_get_param_v2_cmd_control: command Success:." );
        elite_apr_if_free( vsm_apr_handle, apr_pkt );
        session_object->cmd_control.is_cmd_pending = FALSE;
        return ADSP_EOK;
      }
      break;

      default:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"VSM_get_param_v2_cmd_control: ERROR: Unexpected..." );
      }
      return ADSP_EOK;
    }
  }

  return ADSP_EOK;
}


static int32_t VSM_get_param_v3_cmd_control (
  vsm_session_object_t* session_object
)
{
  int32_t rc = ADSP_EOK;
  vss_icommon_cmd_get_param_v3_t* payload;
  uint32_t payload_size;
  uint64_t param_data_virt_addr = 0;
  voice_get_param_v2_t get_param;
  uint32_t vsm_mem_handle;
  uint16_t client_addr;
  qurt_elite_memorymap_mapping_mode_t mapping_mode;
  uint32_t alignment_check;
  int32_t *get_param_ptr = NULL;
  voice_param_data_v2_t* param_data_ptr;
  bool_t is_inband = FALSE;

  enum
  {
    VSM_ACTION_ENUM_UNINITIALIZED,
    VSM_ACTION_ENUM_GET_PARAM,
    VSM_ACTION_ENUM_WAIT_1,
    VSM_ACTION_ENUM_ERROR_HANDLE,
    VSM_ACTION_ENUM_REPORT_FAILURE,
    VSM_ACTION_ENUM_COMPLETE,
    VSM_ACTION_ENUM_INVALID
  };

   if( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  elite_apr_packet_t *apr_pkt = ( elite_apr_packet_t * )( session_object->cmd_control.cmd_msg.pPayload );
  client_addr = apr_pkt->src_addr;

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "VSM_get_param_v3_cmd_control():  session_portID = 0x%08X,"
         " current_action %d", apr_pkt->dst_port, session_object->cmd_control.current_action );

  session_object->cmd_control.is_cmd_pending = TRUE;
  while ( VSM_ACTION_ENUM_UNINITIALIZED == session_object->cmd_control.current_action )
  {
    if( FALSE == vsm_utility_validate_apr_payload_size( apr_pkt ) )
    {
      session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
      break;
    }

    payload = APRV2_PKT_GET_PAYLOAD( vss_icommon_cmd_get_param_v3_t, apr_pkt );
    if ( FALSE == vsm_utility_verify_param_data_size_aligned( payload->mem_size ) )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_get_param_v3_cmd_control(): Invalid param data size, %d",
                                              payload->mem_size );
      session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
      break;
    }

    if ( payload->mem_handle == 0 )
    {
      /* Parameter data is in-band. */
      vsm_mem_handle = 0;
      is_inband = TRUE;

     /* Allocate Memory for in-band operation. */
      get_param_ptr = ( int32_t* )qurt_elite_memory_malloc(
                                     payload->mem_size, QURT_ELITE_HEAP_DEFAULT );
      if ( NULL == get_param_ptr )
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VSM_get_param_v3_cmd_control: failed to allocate memory for get_param payload..." );
        session_object->cmd_control.cmd_result = ADSP_ENORESOURCE;
        session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
        break;
      }
    }
    else
    {
      /* Parameter data is out-of-band. */
      is_inband = FALSE;

      rc = cvd_mem_mapper_validate_handle( payload->mem_handle );
      if ( rc )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_get_param_v3_cmd_control(): Invalid mem_handle: 0x%08X.",
                                                payload->mem_handle );
        session_object->cmd_control.cmd_result = ADSP_EHANDLE;
        session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
        break;
      }

      ( void ) cvd_mem_mapper_get_vsm_mem_handle( payload->mem_handle, &vsm_mem_handle );

      rc = qurt_elite_memorymap_get_mapping_mode( vsm_memory_map_client, vsm_mem_handle, &mapping_mode );
      if( ADSP_FAILED( rc ) )
      {
        session_object->cmd_control.cmd_result = ADSP_EHANDLE;
        session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
        break;
      }

      alignment_check = 32;

      rc = cvd_mem_mapper_validate_attributes_align( payload->mem_handle, payload->mem_address );
      if ( rc )
      {
        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_get_param_v3_cmd_control(): Mis-aligned mem address:"
          " lsw: 0x%08X, msw: 0x%08X", ( uint32_t )payload->mem_address,
          ( uint32_t )( payload->mem_address >> 32 ) );
        session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
        session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
        break;
      }

      rc = cvd_mem_mapper_validate_attributes_align( payload->mem_handle, payload->mem_size );
      if ( rc )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_get_param_v3_cmd_control(): Mis-aligend mem size: 0x%08X",
                                                payload->mem_size );
        session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
        session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
        break;
      }

      rc = cvd_mem_mapper_validate_mem_is_in_region( payload->mem_handle, payload->mem_address,
                                                     payload->mem_size );
      if ( rc )
      {
        MSG_4( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
          "VSM_get_param_v3_cmd_control(): Memory is not within range,"
          " mem_handle: 0x%08X, addr: lsw: 0x%08X, msw: 0x%08X, size: %d",
          payload->mem_handle, ( uint32_t )payload->mem_address,
          ( uint32_t )( payload->mem_address >> 32 ), payload->mem_size );
        session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
        session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
        break;
      }

      rc = cvd_mem_mapper_get_virtual_addr( payload->mem_handle, payload->mem_address,
                                            &param_data_virt_addr );
      if ( rc )
      {
        MSG_3( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
          "VSM_get_param_v3_cmd_control(): Cannot get virtual address,"
          " mem_handle: 0x%08X, addr: lsw: 0x%08X, msw: 0x%08X", payload->mem_handle,
          ( uint32_t )payload->mem_address, ( uint32_t )( payload->mem_address >> 32 ) );
        session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
        session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
        break;
      }

      rc = voice_cmn_check_align_size( ( uint32_t ) param_data_virt_addr, 
              ( uint32_t ) ( param_data_virt_addr >> 32 ),
                payload->mem_size, alignment_check );
      if( ADSP_FAILED( rc ) )
      {
        session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
        session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
        break;
      }

      elite_mem_shared_memory_map_t memmap = {0};
      memmap.unMemMapClient = vsm_memory_map_client;
      memmap.unMemMapHandle = vsm_mem_handle;
      rc = elite_mem_map_get_shm_attrib( ( uint32_t ) param_data_virt_addr,
                                         ( uint32_t ) ( param_data_virt_addr >> 32 ),
                                          payload->mem_size, &memmap );
      if( ADSP_FAILED( rc ) )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"VSM_get_param_v3_cmd_control: Error! Failed to get virtual address with result %d!", rc );
        session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
        session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
        break;
      }
      get_param_ptr = ( int32_t* )memmap.unVirtAddr;
    }

    param_data_ptr = ( voice_param_data_v2_t* )get_param_ptr;
    param_data_ptr->module_id = payload->module_id;
    param_data_ptr->instance_id = payload->instance_id;
    param_data_ptr->param_id = payload->param_id;
    param_data_ptr->param_size = payload->mem_size;
    param_data_ptr->reserved = 0;
    
    session_object->cmd_control.current_action = VSM_ACTION_ENUM_GET_PARAM;
    break;

  }

  for ( ; ; )
  {
    switch ( session_object->cmd_control.current_action )
    {
      case VSM_ACTION_ENUM_GET_PARAM:
      {
        rc = vsm_action_get_param( session_object, get_param_ptr, is_inband, CVD_CAL_PARAM_MINOR_VERSION_1 );
        if ( ADSP_FAILED( rc ) )
        {
          session_object->cmd_control.cmd_result = ADSP_EFAILED;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
          continue;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_WAIT_1;
          return ADSP_EOK;
        }
      }
      break;

      case VSM_ACTION_ENUM_WAIT_1:
      {
        if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
        {
          session_object->cmd_control.cmd_result = session_object->cmd_control.resp_result;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_COMPLETE;
        }
        continue;
      }
      break;

      case VSM_ACTION_ENUM_ERROR_HANDLE:
      {
        if ( TRUE == session_object->cmd_control.is_resp_pending )
        {
          return ADSP_EOK;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
          continue;
        }
      }
      break;

      case VSM_ACTION_ENUM_REPORT_FAILURE:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"VSM_get_param_v3_cmd_control: command FAILED:." );
        elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, session_object->cmd_control.cmd_result );
        session_object->cmd_control.is_cmd_pending = FALSE;
        return ADSP_EOK;
      }
      break;

      case VSM_ACTION_ENUM_COMPLETE:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,"VSM_get_param_v3_cmd_control: command Success:." );
        elite_apr_if_free( vsm_apr_handle, apr_pkt );
        session_object->cmd_control.is_cmd_pending = FALSE;
        return ADSP_EOK;
      }
      break;

      default:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"VSM_get_param_v3_cmd_control: ERROR: Unexpected..." );
      }
      return ADSP_EOK;
    }
  }

  return ADSP_EOK;
}

static int32_t VSM_vpcm_stop_cmd_control(
  vsm_session_object_t* session_object
)
{
  int32_t rc = ADSP_EOK;
  uint16_t client_addr;
  elite_apr_packet_t *apr_payload_pkt;

  enum
  {
    VSM_ACTION_ENUM_UNINITIALIZED,
    VSM_ACTION_VPCM_STOP,
    VSM_ACTION_ENUM_WAIT_1,
    VSM_ACTION_ENUM_ERROR_HANDLE,
    VSM_ACTION_ENUM_REPORT_FAILURE,
    VSM_ACTION_ENUM_COMPLETE,
    VSM_ACTION_ENUM_INVALID
  };

   if( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  elite_apr_packet_t *apr_pkt = ( elite_apr_packet_t * )( session_object->cmd_control.cmd_msg.pPayload );
  client_addr = apr_pkt->src_addr;

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "VSM_vpcm_stop_cmd_control():  session_portID = 0x%08X,"
         " current_action %d", apr_pkt->dst_port, session_object->cmd_control.current_action );

  session_object->cmd_control.is_cmd_pending = TRUE;
  while ( VSM_ACTION_ENUM_UNINITIALIZED == session_object->cmd_control.current_action )
  {
    if ( FALSE == session_object->vpcm_info.is_enabled )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_vpcm_stop_cmd_control(): VPCM already stoped" );
      session_object->cmd_control.cmd_result = ADSP_EALREADY;
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
      break;
    }

    rc = elite_apr_if_alloc_cmd(
             vsm_apr_handle,
             session_object->apr_info.self_addr, session_object->apr_info.self_port,
             session_object->apr_info.self_addr, session_object->apr_info.self_port,
             0, VOICE_CMD_STOP_HOST_PCM,
             NULL, &apr_payload_pkt );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_vpcm_stop_cmd_control(): "
             " Failed to allocate APR packet error %d.", rc );
      session_object->cmd_control.cmd_result = ADSP_EFAILED;
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
      break;
    }
    session_object->vpcm_info.client_addr = APRV2_PKT_INIT_ADDR_V;
    session_object->vpcm_info.client_handle = APR_NULL_V;
    session_object->vpcm_info.cvs_handle = APR_NULL_V;
    session_object->vpcm_info.mem_handle = 0;
    session_object->cmd_control.current_action = VSM_ACTION_VPCM_STOP;
    break;
  }

  for ( ; ; )
  {
    switch ( session_object->cmd_control.current_action )
    {
      case VSM_ACTION_VPCM_STOP:
      {
        rc = vsm_action_send_start_stop_host_pcm( session_object, apr_payload_pkt );
        if ( ADSP_FAILED( rc ) )
        {
          session_object->cmd_control.cmd_result = ADSP_EFAILED;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
          continue;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_WAIT_1;
          return ADSP_EOK;
        }
      }
      break;

      case VSM_ACTION_ENUM_WAIT_1:
      {
        if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
        {
          session_object->cmd_control.cmd_result = session_object->cmd_control.resp_result;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
        }
        else
        {
          session_object->vpcm_info.is_enabled = FALSE;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_COMPLETE;
        }
        continue;
      }
      break;

      case VSM_ACTION_ENUM_ERROR_HANDLE:
      {
        if ( TRUE == session_object->cmd_control.is_resp_pending )
        {
          return ADSP_EOK;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
          continue;
        }
      }
      break;

      case VSM_ACTION_ENUM_REPORT_FAILURE:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"VSM_vpcm_stop_cmd_control: command FAILED:." );
        elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, session_object->cmd_control.cmd_result );
        session_object->cmd_control.is_cmd_pending = FALSE;
        return ADSP_EOK;
      }
      break;

      case VSM_ACTION_ENUM_COMPLETE:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,"VSM_vpcm_stop_cmd_control: command Success:." );
        elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EOK );
        session_object->cmd_control.is_cmd_pending = FALSE;
        return ADSP_EOK;
      }
      break;

      default:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"VSM_vpcm_stop_cmd_control: ERROR: Unexpected..." );
      }
      return ADSP_EOK;
    }
  }

  return ADSP_EOK;
}

static int32_t VSM_vpcm_start_v2_cmd_control(
  vsm_session_object_t* session_object
)
{
  int32_t rc = ADSP_EOK;
  vss_ivpcm_cmd_start_v2_t* payload;
  vss_ivpcm_tap_point_t* tap_points;
  voice_tap_point_v2_t dst_tap_point;
  uint32_t num_tap_points;
  uint32_t payload_len;
  uint32_t expected_payload_len;
  uint32_t tap_point_index;
  uint32_t target_payload_len;
  aprv2_packet_t* dst_packet;
  uint8_t* dst_payload;
  uint32_t dst_payload_left;
  uint32_t cvd_mem_handle;
  uint32_t vsm_mem_handle;
  uint16_t client_addr;

  enum
  {
    VSM_ACTION_ENUM_UNINITIALIZED,
    VSM_ACTION_VPCM_START_V2,
    VSM_ACTION_ENUM_WAIT_1,
    VSM_ACTION_ENUM_ERROR_HANDLE,
    VSM_ACTION_ENUM_REPORT_FAILURE,
    VSM_ACTION_ENUM_COMPLETE,
    VSM_ACTION_ENUM_INVALID
  };

  if( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  elite_apr_packet_t *apr_pkt = ( elite_apr_packet_t * )( session_object->cmd_control.cmd_msg.pPayload );
  client_addr = apr_pkt->src_addr;

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "VSM_vpcm_start_v2_cmd_control():  session_portID = 0x%08X,"
         " current_action %d", apr_pkt->dst_port, session_object->cmd_control.current_action );

  session_object->cmd_control.is_cmd_pending = TRUE;
  while ( VSM_ACTION_ENUM_UNINITIALIZED == session_object->cmd_control.current_action )
  {
    if ( session_object->vpcm_info.is_enabled == TRUE )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_vpcm_start_v2_cmd_control(): VPCM already started" );
      session_object->cmd_control.cmd_result = ADSP_EALREADY;
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
      break;
    }

    payload_len = APRV2_PKT_GET_PAYLOAD_BYTE_SIZE( apr_pkt->header );

    if ( payload_len < sizeof( vss_ivpcm_cmd_start_v2_t ) )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_vpcm_start_v2_cmd_control(): Unexpected payload size, %d < %d",
              payload_len, sizeof( vss_ivpcm_cmd_start_v2_t ) );
      session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
      break;
    }

    payload = APRV2_PKT_GET_PAYLOAD( vss_ivpcm_cmd_start_v2_t, apr_pkt );

    num_tap_points = payload->num_tap_points;
    /* currently can support 1 tx and 1 rx tap point per session. */
    if ( ( 0 == num_tap_points ) || ( num_tap_points > 2 ) )
    {
      session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
      break;
    }

    /* expected_payload_len = ( sizeof( mem_handle ) + sizeof( num_tap_points ) +
                                ( num_tap_points * sizeof( each tap point ) ) ). */
    expected_payload_len = ( sizeof( uint32_t ) + sizeof( uint32_t ) +
                             ( num_tap_points * sizeof( vss_ivpcm_tap_point_t ) ) );

    if ( payload_len != expected_payload_len )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_vpcm_start_v2_cmd_control(): Invalid data."
             " Payload len: %d, expected len: %d", payload_len, expected_payload_len );
      session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
      break;
    }

    cvd_mem_handle = payload->mem_handle;

    rc = cvd_mem_mapper_validate_handle( cvd_mem_handle );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_vpcm_start_v2_cmd_control(): "
             " Invalid mem_handle: 0x%08X.", cvd_mem_handle );
      session_object->cmd_control.cmd_result = ADSP_EHANDLE;
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
      break;
    }

    ( void ) cvd_mem_mapper_get_vsm_mem_handle( cvd_mem_handle, &vsm_mem_handle );

    tap_points = ( ( vss_ivpcm_tap_point_t* )
                    ( ( ( uint8_t* ) payload ) + sizeof( vss_ivpcm_cmd_start_v2_t ) ) );

    /* target_payload_len = ( sizeof( num_tap_points ) +
                              ( num_tap_points * sizeof( each tap point ) ) ). */
    target_payload_len = ( sizeof( uint32_t ) +
                             ( num_tap_points * sizeof( voice_tap_point_v2_t ) ) );

      rc = elite_apr_if_alloc_cmd(
            vsm_apr_handle,
            session_object->apr_info.self_addr, session_object->apr_info.self_port,
            session_object->apr_info.self_addr, session_object->apr_info.self_port,
            0, VOICE_CMD_START_HOST_PCM_V2,
            target_payload_len, &dst_packet );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_vpcm_start_v2_cmd_control(): "
             " Failed to allocate APR packet error %d.", rc );
      session_object->cmd_control.cmd_result = ADSP_EFAILED;
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
      break;
    }

    dst_payload = APRV2_PKT_GET_PAYLOAD( uint8_t, dst_packet );
    dst_payload_left = APRV2_PKT_GET_PAYLOAD_BYTE_SIZE( dst_packet->header );

    /* Copy number of tap points. */
    ( void ) mmstd_memcpy( dst_payload, dst_payload_left, &num_tap_points, sizeof( num_tap_points ) );
    dst_payload_left -= sizeof( num_tap_points );
    dst_payload += sizeof( num_tap_points );

    for ( tap_point_index = 0; tap_point_index < num_tap_points; ++tap_point_index )
    {
      switch( tap_points->tap_point )
      {
        case VSS_IVPCM_TAP_POINT_TX_DEFAULT:
          dst_tap_point.tap_point = VOICESTREAM_MODULE_TX;
          break;

        case VSS_IVPCM_TAP_POINT_RX_DEFAULT:
          dst_tap_point.tap_point = VOICESTREAM_MODULE_RX;
          break;

        default:
          MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvs_vpcm_start_v2_cmd_ctrl(): Invalid tap point: 0x%8X",
                                                  tap_points->tap_point );
          /* Free the allocated packet. */
          ( void ) elite_apr_if_free( vsm_apr_handle, dst_packet );
          rc = ADSP_EBADPARAM;
          break;
      }
      if ( rc ) break;

      switch( tap_points->direction )
      {
        case VSS_IVPCM_TAP_POINT_DIR_OUT:
          dst_tap_point.direction = VOICE_HOST_PCM_READ;
          break;

        case VSS_IVPCM_TAP_POINT_DIR_IN:
          dst_tap_point.direction = VOICE_HOST_PCM_WRITE;
          break;

        case VSS_IVPCM_TAP_POINT_DIR_OUT_IN:
          dst_tap_point.direction = ( VOICE_HOST_PCM_READ | VOICE_HOST_PCM_WRITE );
          break;

        default:
          MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvs_vpcm_start_v2_cmd_ctrl(): Invalid dir: %d",
                                                  tap_points->direction );
          /* Free the allocated packet. */
          ( void ) elite_apr_if_free( vsm_apr_handle, dst_packet );
          rc = ADSP_EBADPARAM;
          break;
      }
      if ( rc ) break;

      dst_tap_point.sampling_rate = tap_points->sampling_rate;
      dst_tap_point.duration_ms = tap_points->duration;
      dst_tap_point.reserved = 0;
      dst_tap_point.mem_map_handle = vsm_mem_handle;

      /* Copy each tap point structure to destination payload. */
      ( void ) mmstd_memcpy( dst_payload, dst_payload_left, &dst_tap_point, sizeof( dst_tap_point ) );
      dst_payload_left -= sizeof( dst_tap_point );
      dst_payload += sizeof( dst_tap_point );
      /* Advance to the next tap point. */
      tap_points += 1;
    }

    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_vpcm_start_v2_cmd_control(): "
             " Failed to allocate APR payload error %d.", rc );
      session_object->cmd_control.cmd_result = ADSP_EFAILED;
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
      break;
    }

    session_object->vpcm_info.client_addr = apr_pkt->src_addr;
    session_object->vpcm_info.client_handle = apr_pkt->src_port;
    session_object->vpcm_info.cvs_handle = session_object->apr_info.self_port;
    session_object->vpcm_info.mem_handle = payload->mem_handle;
    session_object->cmd_control.current_action = VSM_ACTION_VPCM_START_V2;
    break;
  }

  for ( ; ; )
  {
    switch ( session_object->cmd_control.current_action )
    {
      case VSM_ACTION_VPCM_START_V2:
      {
        rc = vsm_action_send_start_stop_host_pcm( session_object, dst_packet );
        if ( ADSP_FAILED( rc ) )
        {
          session_object->cmd_control.cmd_result = ADSP_EFAILED;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
          continue;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_WAIT_1;
          return ADSP_EOK;
        }
      }
      break;

      case VSM_ACTION_ENUM_WAIT_1:
      {
        if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
        {
          session_object->cmd_control.cmd_result = session_object->cmd_control.resp_result;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
        }
        else
        {
          session_object->vpcm_info.is_enabled = TRUE;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_COMPLETE;
        }
        continue;
      }
      break;

      case VSM_ACTION_ENUM_ERROR_HANDLE:
      {
        if ( TRUE == session_object->cmd_control.is_resp_pending )
        {
          return ADSP_EOK;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
          continue;
        }
      }
      break;

      case VSM_ACTION_ENUM_REPORT_FAILURE:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"VSM_vpcm_start_v2_cmd_control: command FAILED:." );
        elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, session_object->cmd_control.cmd_result );
        session_object->cmd_control.is_cmd_pending = FALSE;
        return ADSP_EOK;
      }
      break;

      case VSM_ACTION_ENUM_COMPLETE:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,"VSM_vpcm_start_v2_cmd_control: command Success:." );
        elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EOK );
        session_object->cmd_control.is_cmd_pending = FALSE;
        return ADSP_EOK;
      }
      break;

      default:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"VSM_vpcm_start_v2_cmd_control: ERROR: Unexpected..." );
      }
      return ADSP_EOK;
    }
  }

  return ADSP_EOK;
}

static int32_t VSM_ittyoob_cmd_deregister_cmd_control(
  vsm_session_object_t* session_object
)
{
  int32_t rc = ADSP_EOK;
  uint16_t client_addr;

  if( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  elite_apr_packet_t *apr_pkt = ( elite_apr_packet_t * )( session_object->cmd_control.cmd_msg.pPayload );
  client_addr = apr_pkt->src_addr;

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED, "VSM_ittyoob_cmd_deregister_cmd_control(): "
         " session_portID = 0x%08X, current_action %d", apr_pkt->dst_port );

  /* Allow only the following entities to deregister:
   * 1. Client who registered for OOB TTY.
   * 2. CVS on behalf of its client to deregister, in case of the client's
   *    subsystem is being restarted.
   */
  if ( ( apr_pkt->src_addr != session_object->tty_info.ittyoob_client_addr ) &&
       ( apr_pkt->src_addr != session_object->apr_info.self_addr ) )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_ittyoob_cmd_deregister_cmd_control(): Deregistration by invalid client!!!" );

    rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EFAILED);
    VSM_COMM_ERROR( rc, client_addr );

    return ADSP_EOK;
  }

  if( session_object->tty_info.is_ittyoob_registered == TRUE )
  {
    session_object->is_stream_config_changed = TRUE;
  }

  session_object->tty_info.is_ittyoob_registered = FALSE;
  /* Clearing the Cleint Addr/Port  for routing the OOBTTY EVT for Tx. */
  session_object->tty_info.ittyoob_client_addr = APR_NULL_V;
  session_object->tty_info.ittyoob_client_port = APR_NULL_V;

  MSG( MSG_SSID_DFLT, MSG_LEGACY_LOW, "cvs_ittyoob_cmd_deregister(): ITTYOOB Deregistered" );

  rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EOK);
  VSM_COMM_ERROR( rc, client_addr );

  MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,
       "VSM_ittyoob_cmd_deregister_cmd_control: command Success:." );

  return ADSP_EOK;
}

static int32_t VSM_ittyoob_cmd_register_cmd_control(
  vsm_session_object_t* session_object
)
{
  int32_t rc = ADSP_EOK;
  uint16_t client_addr;

  if( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  elite_apr_packet_t *apr_pkt = ( elite_apr_packet_t * )( session_object->cmd_control.cmd_msg.pPayload );
  client_addr = apr_pkt->src_addr;

  /* Registration supported for Single Client  */
  if( APR_NULL_V != session_object->tty_info.ittyoob_client_addr )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_ittyoob_cmd_register_cmd_control(): ITTYOOB is already Registered!!!" );

    rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EALREADY);
    VSM_COMM_ERROR( rc, client_addr );

    return ADSP_EOK;
  }

  if( session_object->tty_info.is_ittyoob_registered == FALSE )
  {
    session_object->is_stream_config_changed = TRUE;
  }

  session_object->tty_info.is_ittyoob_registered = TRUE;

  /* Caching the Source Addr/Port  for routing the OOBTTY EVT for Tx. */
  session_object->tty_info.ittyoob_client_addr = apr_pkt->src_addr;
  session_object->tty_info.ittyoob_client_port = apr_pkt->src_port;

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "VSM_ittyoob_cmd_register_cmd_control(): Registered:" \
                                        " Client Addr: %04x, Client Port: %04x", 
                                        session_object->tty_info.ittyoob_client_addr,
                                        session_object->tty_info.ittyoob_client_port );

  rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EOK );
  VSM_COMM_ERROR( rc, client_addr );

  return ADSP_EOK;
}

static int32_t VSM_pktexg_oob_set_config_cmd_control(
  vsm_session_object_t* session_object
)
{
  int32_t rc;
  vss_ipktexg_cmd_oob_set_config_t* payload;
  uint64_t enc_buf_virt_addr;
  uint64_t dec_buf_virt_addr;
  uint32_t vsm_mem_handle;

  if( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  elite_apr_packet_t *apr_pkt = ( elite_apr_packet_t * )( session_object->cmd_control.cmd_msg.pPayload );
  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
         "VSM: SET_OOB_CONFIG: session_portID = 0x%08X, current_action %d",
         apr_pkt->dst_port, session_object->cmd_control.current_action );

  session_object->cmd_control.is_cmd_pending = TRUE;

  for( ;; )
  {
    if( FALSE == vsm_utility_validate_apr_payload_size( apr_pkt ) )
    {
      session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
      break;
    }

    if ( ( session_object->apr_info.client_addr != apr_pkt->src_addr ) ||
         ( session_object->apr_info.client_port != apr_pkt->src_port ) )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VSM: SET_OOB_CONFIG: Client is passive, src_port: 0x%04X.", apr_pkt->src_port );
      session_object->cmd_control.cmd_result = ADSP_EHANDLE;
      break;
    }

    if ( VSM_STATE_ENUM_RUN == session_object->state_control.state )
    { /* Stream must not be running when setting the packet exchange mode. */
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "VSM: SET_OOB_CONFIG: Client error: Stream is running." );
      session_object->cmd_control.cmd_result = ADSP_EFAILED;
      break;
    }

    if ( VSS_ISTREAM_PACKET_EXCHANGE_MODE_OUT_OF_BAND != session_object->packet_exchange_info.mode )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "VSM: SET_OOB_CONFIG: Packet exchange mode is not OOB." );
      session_object->cmd_control.cmd_result = ADSP_EFAILED;
      break;
    }

    payload = APRV2_PKT_GET_PAYLOAD( vss_ipktexg_cmd_oob_set_config_t, apr_pkt );

    if ( ( payload->enc_buf_size < VSM_MAX_OOB_VOC_PACKET_BUFFER_SIZE ) ||
         ( payload->dec_buf_size < VSM_MAX_OOB_VOC_PACKET_BUFFER_SIZE ) )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VSM: SET_OOB_CONFIG: Invalid enc (%d) or dec (%d) buffer size.",
             payload->enc_buf_size, payload->dec_buf_size );
      session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
      break;
    }

    rc = cvd_mem_mapper_validate_handle( payload->mem_handle );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VSM: SET_OOB_CONFIG: Invalid mem_handle: 0x%08X.",
             payload->mem_handle );
      session_object->cmd_control.cmd_result = ADSP_EHANDLE;
      break;
    }

    rc = cvd_mem_mapper_validate_attributes_align( payload->mem_handle, payload->enc_buf_addr );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VSM: SET_OOB_CONFIG: Mis-aligned enc_buf_addr: 0x%016X.", payload->enc_buf_addr );
      session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
      break;
    }

    rc = cvd_mem_mapper_validate_attributes_align( payload->mem_handle, payload->dec_buf_addr );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VSM: SET_OOB_CONFIG: Mis-aligned dec_buf_addr: 0x%016X", payload->dec_buf_addr );
      session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
      break;
    }

    rc = cvd_mem_mapper_validate_attributes_align( payload->mem_handle, payload->enc_buf_size );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VSM: SET_OOB_CONFIG: Mis-aligned enc_buf_size: %d.", payload->enc_buf_size );
      session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
      break;
    }

    rc = cvd_mem_mapper_validate_attributes_align( payload->mem_handle, payload->dec_buf_size );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VSM: SET_OOB_CONFIG: Mis-aligned dec_buf_size: %d.", payload->dec_buf_size );
      session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
      break;
    }

    rc = cvd_mem_mapper_validate_mem_is_in_region( payload->mem_handle, payload->enc_buf_addr,
                                                   payload->enc_buf_size );
    if ( rc )
    {
      MSG_3( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VSM: SET_OOB_CONFIG: Memory is not within range, mem_handle: 0x%08X, "
             "enc_buf_addr: 0x%016X, enc_buf_size: %d.", payload->mem_handle,
             payload->enc_buf_addr, payload->enc_buf_size );
      session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
      break;
    }

    rc = cvd_mem_mapper_validate_mem_is_in_region( payload->mem_handle, payload->dec_buf_addr,
                                                   payload->dec_buf_size );
    if ( rc )
    {
      MSG_3( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VSM: SET_OOB_CONFIG: Memory is not within range, mem_handle: 0x%08X, "
             "dec_buf_addr: 0x%016X, dec_buf_size: %d.", payload->mem_handle,
             payload->dec_buf_addr, payload->dec_buf_size );
      session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
      break;
    }

    rc = cvd_mem_mapper_get_virtual_addr( payload->mem_handle, payload->enc_buf_addr, &enc_buf_virt_addr );
    if ( rc )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VSM: SET_OOB_CONFIG: Cannot get virtual address, mem_handle: 0x%08X, "
             "enc_buf_addr: 0x%016X.", payload->mem_handle, payload->enc_buf_addr );
      session_object->cmd_control.cmd_result = ADSP_EFAILED;
      break;
    }

    rc = cvd_mem_mapper_get_virtual_addr( payload->mem_handle, payload->dec_buf_addr, &dec_buf_virt_addr );
    if ( rc )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VSM: SET_OOB_CONFIG: Cannot get virtual address, mem_handle: 0x%08X, "
             "dec_buf_addr: 0x%016X.", payload->mem_handle, payload->dec_buf_addr );
      session_object->cmd_control.cmd_result = ADSP_EFAILED;
      break;
    }

    rc = cvd_mem_mapper_get_vsm_mem_handle( payload->mem_handle, &vsm_mem_handle );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VSM: SET_OOB_CONFIG: Cannot get vsm_mem_handle for cvd_mem_handle: 0x%08X.", payload->mem_handle );
      session_object->cmd_control.cmd_result = ADSP_EFAILED;
      break;
    }

    session_object->packet_exchange_info.oob_info.config.enc_buf_addr_lsw = ( ( uint32_t ) enc_buf_virt_addr );;
    session_object->packet_exchange_info.oob_info.config.enc_buf_addr_msw = ( ( uint32_t ) ( enc_buf_virt_addr >> 32 ) );
    session_object->packet_exchange_info.oob_info.config.enc_buf_size = payload->enc_buf_size;
    session_object->packet_exchange_info.oob_info.config.dec_buf_addr_lsw = ( ( uint32_t ) dec_buf_virt_addr );
    session_object->packet_exchange_info.oob_info.config.dec_buf_addr_msw = ( ( uint32_t ) ( dec_buf_virt_addr >> 32 ) );
    session_object->packet_exchange_info.oob_info.config.dec_buf_size = payload->dec_buf_size;
    session_object->packet_exchange_info.oob_info.config.mem_handle = vsm_mem_handle;
    session_object->packet_exchange_info.oob_info.is_configured = TRUE;

    MSG_3( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "VSM: SET_OOB_CONFIG: enc_buf_addr_lsw: 0x%08X, enc_buf_addr_msw: 0x%08X, enc_buf_size: 0x%08X",
           session_object->packet_exchange_info.oob_info.config.enc_buf_addr_lsw,
           session_object->packet_exchange_info.oob_info.config.enc_buf_addr_msw,
           session_object->packet_exchange_info.oob_info.config.enc_buf_size );
    MSG_3( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "VSM: SET_OOB_CONFIG: dec_buf_addr_lsw: 0x%08X, dec_buf_addr_msw: 0x%08X, dec_buf_size: 0x%08X",
           session_object->packet_exchange_info.oob_info.config.dec_buf_addr_lsw,
           session_object->packet_exchange_info.oob_info.config.dec_buf_addr_msw,
           session_object->packet_exchange_info.oob_info.config.dec_buf_size );
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "VSM: SET_OOB_CONFIG: vsm_mem_handle: 0x%08X.", vsm_mem_handle );

    break;
  }

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
         "VSM: SET_OOB_CONFIG: command result=%d", session_object->cmd_control.cmd_result );
  elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, session_object->cmd_control.cmd_result );
  session_object->cmd_control.is_cmd_pending = FALSE;

  return ADSP_EOK;
}

static int32_t VSM_set_mailbox_pktexg_config_cmd_control (
  vsm_session_object_t* session_object
)
{
  int32_t rc = ADSP_EOK;
  vss_ipktexg_cmd_mailbox_set_config_t* payload;
  cvd_virt_addr_t tx_circ_buf_virt_addr;
  cvd_virt_addr_t rx_circ_buf_virt_addr;
  uint32_t tx_circ_buf_data_size;
  uint32_t rx_circ_buf_data_size;

  if( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  elite_apr_packet_t *apr_pkt = ( elite_apr_packet_t * )( session_object->cmd_control.cmd_msg.pPayload );

  for( ;; )
  {
    if( FALSE == vsm_utility_validate_apr_payload_size( apr_pkt ) )
    {
      session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
      break;
    }

    if ( ( session_object->apr_info.client_addr != apr_pkt->src_addr ) ||
         ( session_object->apr_info.client_port != apr_pkt->src_port ) )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VSM: SET_MAILBOX_CONFIG: Client is passive, src_port: 0x%04X.",
             apr_pkt->src_port );
      session_object->cmd_control.cmd_result = ADSP_EHANDLE;
      break;
    }

    if ( VSM_STATE_ENUM_RUN == session_object->state_control.state )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
           "VSM: SET_MAILBOX_CONFIG: Client error: Stream is running." );
      session_object->cmd_control.cmd_result = ADSP_EFAILED;
      break;
    }

    if ( VSS_IPKTEXG_MODE_MAILBOX != session_object->packet_exchange_info.mode )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "VSM: SET_MAILBOX_CONFIG: Packet exchange mode is not MAILBOX." );
      session_object->cmd_control.cmd_result = ADSP_EFAILED;
      break;
    }

    payload = APRV2_PKT_GET_PAYLOAD( vss_ipktexg_cmd_mailbox_set_config_t, apr_pkt );

    rc = cvd_mem_mapper_validate_handle( payload->mem_handle );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VSM: SET_MAILBOX_CONFIG: Invalid mem_handle: 0x%08X.", payload->mem_handle );
      session_object->cmd_control.cmd_result = ADSP_EHANDLE;
      break;
    }

    if ( ( VSM_DIRECTION_TX == session_object->direction) ||
         ( VSM_DIRECTION_RX_TX == session_object->direction ) )
    {
      rc = cvd_mem_mapper_validate_attributes_align(
             payload->mem_handle, payload->tx_circ_buf_mem_address );
      if ( rc )
      {
        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "VSM: SET_MAILBOX_CONFIG: Mis-aligned tx_circ_buf_mem_address: MSW:(0X%08X) LSW:(0X%08X)",
               (uint32_t)(payload->tx_circ_buf_mem_address>>32),
               (uint32_t)(payload->tx_circ_buf_mem_address) );
        session_object->cmd_control.cmd_result = APR_EBADPARAM;
        break;
      }

      rc = cvd_mem_mapper_validate_mem_is_in_region(
              payload->mem_handle, payload->tx_circ_buf_mem_address, 
              payload->tx_circ_buf_mem_size );
      if ( rc )
      {
        MSG_4( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "VSM: SET_MAILBOX_CONFIG: Memory is not within range, mem_handle: 0x%08X, "
               "tx_circ_buf mem_address: MSW:(0X%08X) LSW:(0X%08X), mem_size:(%d)",
               payload->mem_handle, (uint32_t)(payload->tx_circ_buf_mem_address>>32),
               (uint32_t)(payload->tx_circ_buf_mem_address), payload->tx_circ_buf_mem_size );
        session_object->cmd_control.cmd_result = APR_EBADPARAM;
        break;
      }

      tx_circ_buf_data_size = 
        ( payload->tx_circ_buf_mem_size - sizeof( vss_ipktexg_mailbox_voc_req_circ_buffer_t ) );

      if ( ( tx_circ_buf_data_size % sizeof( vss_ipktexg_mailbox_enc_request_t ) ) != 0 )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "VSM: SET_MAILBOX_CONFIG: Tx circular buffer data size %d is not "
               "a multiple of an encoding request size.", tx_circ_buf_data_size );
        session_object->cmd_control.cmd_result = APR_EBADPARAM;
        break;
      }

      rc = cvd_mem_mapper_get_virtual_addr_v2( 
             payload->mem_handle, payload->tx_circ_buf_mem_address,
             &tx_circ_buf_virt_addr );
      if ( rc )
      {
        MSG_3( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "VSM: SET_MAILBOX_CONFIG: Cannot get virtual address, mem_handle: 0x%08X, "
               "tx_circ_buf mem_address: MSW:(0X%08X) LSW:(0X%08X)", payload->mem_handle,
               (uint32_t)(payload->tx_circ_buf_mem_address>>32),
               (uint32_t)(payload->tx_circ_buf_mem_address) );
        session_object->cmd_control.cmd_result = APR_EFAILED;
        break;
      }

      session_object->packet_exchange_info.mailbox_info.config.tx_circ_buf = 
         (vss_ipktexg_mailbox_voc_req_circ_buffer_t*)tx_circ_buf_virt_addr.ptr;
      session_object->packet_exchange_info.mailbox_info.config.tx_circ_buf_mem_size = payload->tx_circ_buf_mem_size;
    }

    if ( ( VSM_DIRECTION_RX == session_object->direction) ||
         ( VSM_DIRECTION_RX_TX == session_object->direction ) )
    {
      rc = cvd_mem_mapper_validate_attributes_align(
             payload->mem_handle, payload->rx_circ_buf_mem_address );
      if ( rc )
      {
        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "VSM: SET_MAILBOX_CONFIG: Mis-aligned tx_circ_buf_mem_address: MSW:(0X%08X) LSW:(0X%08X)",
               (uint32_t)(payload->rx_circ_buf_mem_address>>32),
               (uint32_t)(payload->rx_circ_buf_mem_address) );
        session_object->cmd_control.cmd_result = APR_EBADPARAM;
        break;
      }

      rc = cvd_mem_mapper_validate_mem_is_in_region(
              payload->mem_handle, payload->rx_circ_buf_mem_address, 
              payload->rx_circ_buf_mem_size );
      if ( rc )
      {
        MSG_4( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "VSM: SET_MAILBOX_CONFIG: Memory is not within range, mem_handle: 0x%08X, "
               "rx_circ_buf mem_address: MSW:(0X%08X) LSW:(0X%08X), mem_size:(%d)",
               payload->mem_handle, (uint32_t)(payload->rx_circ_buf_mem_address>>32),
               (uint32_t)(payload->rx_circ_buf_mem_address), payload->rx_circ_buf_mem_size );
        session_object->cmd_control.cmd_result = APR_EBADPARAM;
        break;
      }

      rx_circ_buf_data_size = 
        ( payload->rx_circ_buf_mem_size - sizeof( vss_ipktexg_mailbox_voc_req_circ_buffer_t ) );

      if ( ( rx_circ_buf_data_size % sizeof( vss_ipktexg_mailbox_dec_request_t ) ) != 0 )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "VSM: SET_MAILBOX_CONFIG: Tx circular buffer data size %d is not "
               "a multiple of an encoding request size.", rx_circ_buf_data_size );
        session_object->cmd_control.cmd_result = APR_EBADPARAM;
        break;
      }

      rc = cvd_mem_mapper_get_virtual_addr_v2( 
             payload->mem_handle, payload->rx_circ_buf_mem_address,
             &rx_circ_buf_virt_addr );
      if ( rc )
      {
        MSG_3( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "VSM: SET_MAILBOX_CONFIG: Cannot get virtual address, mem_handle: 0x%08X, "
               "rx_circ_buf mem_address: MSW:(0X%08X) LSW:(0X%08X)", payload->mem_handle,
               (uint32_t)(payload->rx_circ_buf_mem_address>>32),
               (uint32_t)(payload->rx_circ_buf_mem_address) );
        session_object->cmd_control.cmd_result = APR_EFAILED;
        break;
      }

      session_object->packet_exchange_info.mailbox_info.config.rx_circ_buf = 
         ( vss_ipktexg_mailbox_voc_req_circ_buffer_t* ) rx_circ_buf_virt_addr.ptr;
      session_object->packet_exchange_info.mailbox_info.config.rx_circ_buf_mem_size =  payload->rx_circ_buf_mem_size;
    }

    session_object->packet_exchange_info.mailbox_info.is_configured = TRUE;
    session_object->packet_exchange_info.mailbox_info.config.cvs_mem_handle = payload->mem_handle;

    MSG_5( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "VSM: SET_MAILBOX_CONFIG: mem_handle: 0x%08X, rx_circ_buf mem_address:(0X%08X), "
           "rx_mem_size:(%d), tx_circ_buf mem_address:(0X%08X), tx_mem_size:(%d)",
           session_object->packet_exchange_info.mailbox_info.config.cvs_mem_handle,
           session_object->packet_exchange_info.mailbox_info.config.rx_circ_buf,
           (uint32_t)session_object->packet_exchange_info.mailbox_info.config.rx_circ_buf_mem_size,
           session_object->packet_exchange_info.mailbox_info.config.tx_circ_buf,
           (uint32_t)session_object->packet_exchange_info.mailbox_info.config.tx_circ_buf_mem_size );

    break;
  }

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
         "VSM: SET_MAILBOX_CONFIG: command result=%d", session_object->cmd_control.cmd_result );
  elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, session_object->cmd_control.cmd_result );
  session_object->cmd_control.is_cmd_pending = FALSE;

  return ADSP_EOK;
}

static int32_t VSM_get_mailbox_pktexg_voc_packet_properties_cmd_control (
  vsm_session_object_t* session_object
)
{
  int32_t rc = ADSP_EOK;
  vss_ipktexg_rsp_mailbox_get_voc_packet_properties_t rspPayload;
  uint32_t tx_max_size = 0;
  uint32_t rx_max_size = 0;
  /* Note: Taking address of packed member 'rx_max_size/rx_max_size' of class or 
   structure 'vss_ipktexg_rsp_mailbox_get_voc_packet_properties_t' may result in 
   an unaligned pointer value [-Werror,-Waddress-of-packed-member], hence local
   variables are intruduced to avoid this error. */

  if( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  elite_apr_packet_t *packet = ( elite_apr_packet_t * )( session_object->cmd_control.cmd_msg.pPayload );

  for( ;; )
  {
    if ( FALSE == vsm_utility_media_id_is_valid( session_object->target_set.media_id ) )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
           "VSM: GET_VOC_PROPERTIES: Client error: Media type has not been set yet." );
      session_object->cmd_control.cmd_result = ADSP_EFAILED;
      break;
    }

    rc = vsm_utility_get_max_mailbox_pkt_size ( session_object->target_set.media_id, &tx_max_size, &rx_max_size );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "VSM: GET_VOC_PROPERTIES: Failed to get vocoder packet size, "
             "rc = 0x%08X", rc );
      session_object->cmd_control.cmd_result = ADSP_EFAILED;
      break;
    }

    rspPayload.tx_max_size = tx_max_size;
    rspPayload.rx_max_size = rx_max_size;
    rspPayload.tx_duration_us = 20000;
    rspPayload.rx_duration_us = 20000;

    switch ( session_object->active_set.direction )
    {
      case VSM_DIRECTION_RX:
      {
        rspPayload.tx_duration_us = 0;
        rspPayload.tx_max_size = 0;
      }
      break;

      case VSM_DIRECTION_TX:
      {
        rspPayload.rx_duration_us = 0;
        rspPayload.rx_max_size = 0;
      }
      break;

      default:
        break;
    }

    session_object->cmd_control.cmd_result = ADSP_EOK;
    break;
  }

  session_object->cmd_control.is_cmd_pending = FALSE;
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
         "VSM: GET_VOC_PROPERTIES: command result=%d", session_object->cmd_control.cmd_result );

  if( ADSP_EOK == session_object->cmd_control.cmd_result )
  {
    elite_apr_if_alloc_send_ack( vsm_apr_handle,
      packet->dst_addr, packet->dst_port,
      packet->src_addr, packet->src_port,
      packet->token, VSS_IPKTEXG_RSP_MAILBOX_GET_VOC_PACKET_PROPERTIES,
      ( void* ) &rspPayload, sizeof( rspPayload ) );

    elite_apr_if_free(  vsm_apr_handle, packet );
  }
  else
  {
    elite_apr_if_end_cmd( vsm_apr_handle, packet, session_object->cmd_control.cmd_result );
  }

  return ADSP_EOK;
}

static int32_t VSM_pktexg_set_mode_cmd_control(
  vsm_session_object_t* session_object
)
{
  int32_t rc = ADSP_EOK;
  uint16_t client_addr;
  vss_ipktexg_cmd_set_mode_t* payload;
  vsm_set_pkt_exchange_mode_t set_pkt_exchange_mode;
  voice_cmd_shared_mem_unmap_regions_t voice_unmap_memory;
  enum
  {
    VSM_ACTION_ENUM_UNINITIALIZED,
    VSM_ACTION_ENUM_SET_PACKET_EXCHANGE_MODE,
    VSM_ACTION_ENUM_WAIT_1,
    VSM_ACTION_ENUM_CLEAR_OOB_PKTEXG_CONFIG,
    VSM_ACTION_ENUM_ERROR_HANDLE,
    VSM_ACTION_ENUM_REPORT_FAILURE,
    VSM_ACTION_ENUM_COMPLETE,
    VSM_ACTION_ENUM_INVALID
  };

  if( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  elite_apr_packet_t *apr_pkt = ( elite_apr_packet_t * )( session_object->cmd_control.cmd_msg.pPayload );
  client_addr = apr_pkt->src_addr;

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
         "VSM: IPKTEXG_CMD_SET_MODE: sessionPortID: 0x%08X, current_action %d",
         apr_pkt->dst_port, session_object->cmd_control.current_action );

  session_object->cmd_control.is_cmd_pending = TRUE;

  while ( VSM_ACTION_ENUM_UNINITIALIZED == session_object->cmd_control.current_action )
  {
    if( FALSE == vsm_utility_validate_apr_payload_size( apr_pkt ) )
    {
      session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
      break;
    }

    if ( ( session_object->apr_info.client_addr != apr_pkt->src_addr ) ||
         ( session_object->apr_info.client_port != apr_pkt->src_port ) )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VSM: IPKTEXG_CMD_SET_MODE: Client is passive, src_port: 0x%04X",
             apr_pkt->src_port );
      session_object->cmd_control.cmd_result = ADSP_EHANDLE;
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
      break;
    }

    if ( VSM_STATE_ENUM_RUN == session_object->state_control.state )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "VSM: IPKTEXG_CMD_SET_MODE: Stream is in RUN_STATE." );
      session_object->cmd_control.cmd_result = ADSP_EFAILED;
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
      break;
    }

    payload = APRV2_PKT_GET_PAYLOAD( vss_ipktexg_cmd_set_mode_t, apr_pkt );

    if ( ( payload->mode != VSS_IPKTEXG_MODE_IN_BAND ) &&
         ( payload->mode != VSS_IPKTEXG_MODE_MAILBOX ) &&
         ( payload->mode != VSS_IPKTEXG_MODE_OUT_OF_BAND ) )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VSM: IPKTEXG_CMD_SET_MODE: Invalid PKTEXG_MODE:0x%08X", payload->mode );
      session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
      break;
    }

    session_object->cmd_control.current_action = VSM_ACTION_ENUM_SET_PACKET_EXCHANGE_MODE;
    break;
  }

  payload = APRV2_PKT_GET_PAYLOAD( vss_ipktexg_cmd_set_mode_t, apr_pkt );

  for ( ; ; )
  {
    switch ( session_object->cmd_control.current_action )
    {
      case VSM_ACTION_ENUM_SET_PACKET_EXCHANGE_MODE:
      {
        rc = vsm_action_set_packet_exchange_mode( session_object, payload->mode );
        if( ADSP_FAILED( rc ) )
        {
          session_object->cmd_control.cmd_result = ADSP_EFAILED;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
          continue;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_WAIT_1;
          return ADSP_EOK;
        }
      }
      break;

      case VSM_ACTION_ENUM_WAIT_1:
      {
        if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
        {
          session_object->cmd_control.cmd_result = session_object->cmd_control.resp_result;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
        }
        else
        {
          session_object->cmd_control.cmd_result = ADSP_EOK;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_CLEAR_OOB_PKTEXG_CONFIG;
        }
        continue;
      }
      break;

      case VSM_ACTION_ENUM_CLEAR_OOB_PKTEXG_CONFIG:
      {
        if ( ( VSS_IPKTEXG_MODE_OUT_OF_BAND == session_object->packet_exchange_info.mode ) &&
             ( VSS_IPKTEXG_MODE_OUT_OF_BAND != payload->mode ) )
        {
          vsm_action_clear_oob_configuration( session_object );
        }

        if ( ( VSS_IPKTEXG_MODE_MAILBOX == session_object->packet_exchange_info.mode ) &&
             ( VSS_IPKTEXG_MODE_MAILBOX != payload->mode ) )
        {
          vsm_action_clear_mailbox_configuration( session_object );
        }

        session_object->cmd_control.current_action = VSM_ACTION_ENUM_COMPLETE;
        continue;
      }
      break;

      case VSM_ACTION_ENUM_ERROR_HANDLE:
      {
        if ( TRUE == session_object->cmd_control.is_resp_pending )
        {
          return ADSP_EOK;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
          if ( TRUE == session_object->cmd_control.is_resp_pending )
          {
            return ADSP_EOK;
          }
          continue;
        }
      }
      break;

      case VSM_ACTION_ENUM_REPORT_FAILURE:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VSM: IPKTEXG_CMD_SET_MODE: command FAILED:." );
        elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, session_object->cmd_control.cmd_result );
        session_object->cmd_control.is_cmd_pending = FALSE;
        return ADSP_EOK;
      }
      break;

      case VSM_ACTION_ENUM_COMPLETE:
      {
        elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EOK );
        session_object->cmd_control.is_cmd_pending = FALSE;
        session_object->packet_exchange_info.mode = payload->mode;
        return ADSP_EOK;
      }
      break;

      default:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VSM: IPKTEXG_CMD_SET_MODE: ERROR: Unexpected..." );
      }
      return ADSP_EOK;
    }

    break;
  }

  return rc;
}

static int32_t VSM_set_evs_enc_channel_aware_mode_control(
  vsm_session_object_t* session_object,
  bool_t enable
)
{
  int32_t rc = ADSP_EOK;
  int32_t status;
  uint16_t client_addr;
  vss_istream_cmd_set_evs_enc_channel_aware_mode_enable_t* in_args;
  vsm_enc_channel_aware_mode_params_t vsm_channel_aware_mode_params;

  status = ADSP_EOK;

  if( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  elite_apr_packet_t *apr_pkt = ( elite_apr_packet_t * )( session_object->cmd_control.cmd_msg.pPayload );
  client_addr = apr_pkt->src_addr;

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED, "VSM_set_evs_enc_channel_aware_mode_control():"
         " Session portID = 0x%08X ", session_object->apr_info.self_port );

  if ( ( session_object->apr_info.client_addr != apr_pkt->src_addr ) ||
       ( session_object->apr_info.client_port != apr_pkt->src_port ) )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_set_evs_enc_channel_aware_mode_ctrl(): "
           "Client is passive, src_port: 0x%04X.", apr_pkt->src_port );
    rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EHANDLE );
    VSM_COMM_ERROR( rc, client_addr );
    return ADSP_EOK;
  }

  if ( !vsm_utility_vocoder_is_variable( session_object->active_set.media_id ) )
  {
    client_addr = apr_pkt->src_addr;
    status = ADSP_EFAILED;
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_set_evs_enc_channel_aware_mode_ctrl(): Active Media ID not " \
           "a variable vocoder: 0x%08X", session_object->active_set.media_id );

    rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EFAILED );
    VSM_COMM_ERROR( rc, client_addr );
    return ADSP_EOK;
  }

  if ( enable )
  {
    if( FALSE == vsm_utility_validate_apr_payload_size( apr_pkt ) )
    {
      rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EBADPARAM );
      VSM_COMM_ERROR( rc, client_addr );
      return ADSP_EOK;
    }

    in_args = APRV2_PKT_GET_PAYLOAD( vss_istream_cmd_set_evs_enc_channel_aware_mode_enable_t, apr_pkt );

    /* Check if the command payload is valid */
    if ( !vsm_utility_evs_validate_enc_channel_aware_mode( in_args->fec_offset, in_args->fer_rate ) )
    {
      status = ADSP_EBADPARAM;
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_set_evs_enc_channel_aware_mode_ctrl(): Invalid payload " \
             "FEC Offset=0x%08X, FER Rate=0x%08X", in_args->fec_offset, in_args->fer_rate );
      rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, status );
      VSM_COMM_ERROR( rc, client_addr );
      return ADSP_EOK;
    }

    vsm_channel_aware_mode_params.channel_aware_enabled = TRUE;
    vsm_channel_aware_mode_params.fec_offset = in_args->fec_offset;
    vsm_channel_aware_mode_params.fer_rate = in_args->fer_rate;
    vsm_utility_cache_voc_property( session_object, (void*)&vsm_channel_aware_mode_params, VSM_VOC_PROPERTY_ENUM_CHANNEL_AWARE_MODE );
    rc = vsm_action_forward_command_to_venc( session_object, VSM_CMD_SET_EVS_ENC_CHANNEL_AWARE_MODE_ENABLE );
  }
  else
  {
    vsm_channel_aware_mode_params.channel_aware_enabled = FALSE;
    vsm_channel_aware_mode_params.fec_offset = VSM_STREAM_PROPERTY_NOT_SET_UINT8;
    vsm_channel_aware_mode_params.fer_rate = VSM_STREAM_PROPERTY_NOT_SET_UINT8;
    vsm_utility_cache_voc_property( session_object, (void*)&vsm_channel_aware_mode_params, VSM_VOC_PROPERTY_ENUM_CHANNEL_AWARE_MODE );
    rc = vsm_action_forward_command_to_venc( session_object, VSM_CMD_SET_EVS_ENC_CHANNEL_AWARE_MODE_DISABLE );
  }

  MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,
       "VSM_set_evs_enc_channel_aware_mode_control: command Success:." );

  return rc;
}

static int32_t VSM_set_evs_voc_operating_mode_control(
  vsm_session_object_t* session_object
)
{
  int32_t rc = ADSP_EOK;
  int32_t status = ADSP_EOK;
  uint16_t client_addr;
  vss_istream_cmd_set_evs_voc_enc_operating_mode_t* in_args;

  if( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  elite_apr_packet_t *apr_pkt = ( elite_apr_packet_t * )( session_object->cmd_control.cmd_msg.pPayload );
  client_addr = apr_pkt->src_addr;

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED, "VSM_set_evs_voc_operating_mode_control():"
         " Session portID = 0x%08X ", session_object->apr_info.self_port );

  in_args = APRV2_PKT_GET_PAYLOAD( vss_istream_cmd_set_evs_voc_enc_operating_mode_t, apr_pkt );

  if( FALSE == vsm_utility_validate_apr_payload_size( apr_pkt ) )
  {
    rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EBADPARAM );
    VSM_COMM_ERROR( rc, client_addr );
    return ADSP_EOK;
  }

  if ( ( session_object->apr_info.client_addr != apr_pkt->src_addr ) ||
       ( session_object->apr_info.client_port != apr_pkt->src_port ) )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_set_evs_voc_operating_mode_control(): "
           "Client is passive, src_port: 0x%04X.", apr_pkt->src_port );
    rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EHANDLE );
    VSM_COMM_ERROR( rc, client_addr );
    return ADSP_EOK;
  }

  if ( !vsm_utility_vocoder_is_variable( session_object->active_set.media_id ) )
  {
    status = ADSP_EFAILED;
  }

  if ( !vsm_utility_evs_validate_enc_operating_mode( in_args->mode, in_args->bandwidth, session_object->requested_var_voc_tx_sampling_rate ) )
  {
    status = ADSP_EBADPARAM;
    MSG_3( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_set_evs_voc_operating_mode_control(): Invalid payload " \
           "Mode=0x%08X, Bandwidth=0x%08X, Tx PP Sr=0x%08X", in_args->mode, in_args->bandwidth,
           session_object->requested_var_voc_tx_sampling_rate );
  }

  if ( ADSP_EOK != status )
  {
    rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, status );
    VSM_COMM_ERROR( rc, client_addr );
    return ADSP_EOK;
  }

  /* Cache voc property. */
  vsm_utility_cache_voc_property( session_object, ( void* )in_args, VSM_VOC_PROPERTY_ENUM_ENC_OPERATING_MODE );

  vsm_action_forward_command_to_venc( session_object, VSM_CMD_SET_EVS_ENC_OPERATING_MODE );

  MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,
       "VSM_set_evs_voc_operating_mode_control: command Success:." );

  return ADSP_EOK;
}

static int32_t VSM_delegate_to_venc_cmd_processing (
  vsm_session_object_t* session_object,
  uint32_t opcode
)
{
  int32_t rc = ADSP_EOK;
  uint32_t payload_size;
  uint16_t client_addr;
  uint32_t indirect_handle;
  vss_istream_cmd_cdma_set_enc_rate_modulation_t* rate_modulation;

  if( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  elite_apr_packet_t *apr_pkt = ( elite_apr_packet_t * )( session_object->cmd_control.cmd_msg.pPayload );
  client_addr = apr_pkt->src_addr;

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED, "VSM_delegate_to_venc_cmd_processing():"
         " Session portID = 0x%08X ", session_object->apr_info.self_port );

  if( FALSE == vsm_utility_validate_apr_payload_size( apr_pkt ) )
  {
    rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EBADPARAM );
    VSM_COMM_ERROR( rc, client_addr );
    return ADSP_EOK;
  }

  if ( ( session_object->apr_info.client_addr != apr_pkt->src_addr ) ||
       ( session_object->apr_info.client_port != apr_pkt->src_port ) )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_delegate_to_venc_cmd_processing(): "
           "Client is passive, src_port: 0x%04X.", apr_pkt->src_port );
    rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EHANDLE );
    VSM_COMM_ERROR( rc, client_addr );
    return ADSP_EOK;
  }

  /* Cache voc property. */
  switch ( opcode )
  {
    case VSM_CMD_ENC_SET_RATE_MOD:
    {
      rate_modulation = APRV2_PKT_GET_PAYLOAD( vss_istream_cmd_cdma_set_enc_rate_modulation_t, apr_pkt );
      vsm_utility_cache_voc_property( session_object, ( void* )rate_modulation, VSM_VOC_PROPERTY_ENUM_RATE_MODULATION );

      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED, "VSM_delegate_to_venc_cmd_processing(): "
             "enc rate modulation mode %d", rate_modulation->mode );
    }
    break;
  }

  vsm_action_forward_command_to_venc( session_object, opcode );

  MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,
       "VSM_delegate_to_venc_cmd_processing: command Success:." );

  return ADSP_EOK;
}

static int32_t VSM_stream_set_min_max_rate_cmd_control (
  vsm_session_object_t* session_object
)
{
  int32_t rc = ADSP_EOK;
  vss_istream_cmd_cdma_set_enc_minmax_rate_t* min_max_rate = NULL;
  vsm_enc_set_minmax_rate_t set_minmax_rate;
  uint16_t client_addr;

  enum
  {
    VSM_ACTION_ENUM_UNINITIALIZED,
    VSM_ACTION_ENUM_CACHE_AND_SET_1,
    VSM_ACTION_ENUM_WAIT_2,
    VSM_ACTION_ENUM_ERROR_HANDLE,
    VSM_ACTION_ENUM_REPORT_FAILURE,
    VSM_ACTION_ENUM_COMPLETE,
    VSM_ACTION_ENUM_INVALID
  };
  
  if( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  elite_apr_packet_t *apr_pkt = ( elite_apr_packet_t * )( session_object->cmd_control.cmd_msg.pPayload );
  client_addr = apr_pkt->src_addr;

  if ( ( session_object->apr_info.client_addr != apr_pkt->src_addr ) ||
       ( session_object->apr_info.client_port != apr_pkt->src_port ) )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_stream_set_min_max_rate_cmd_control(): "
           "Client is passive, src_port: 0x%04X.", apr_pkt->src_port );
    rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EHANDLE );
    VSM_COMM_ERROR( rc, client_addr );
    return ADSP_EOK;
  }

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "VSM_stream_set_min_max_rate_cmd_control():"
         " session_portID = 0x%08X, current_action %d", apr_pkt->dst_port, 
           session_object->cmd_control.current_action );
  
  session_object->cmd_control.is_cmd_pending = TRUE;
  
  while ( VSM_ACTION_ENUM_UNINITIALIZED == session_object->cmd_control.current_action )
  {
    if( FALSE == vsm_utility_validate_apr_payload_size( apr_pkt ) )
    {
      session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
      break;
    }
    min_max_rate = APRV2_PKT_GET_PAYLOAD( vss_istream_cmd_cdma_set_enc_minmax_rate_t, apr_pkt );
    MSG_3( MSG_SSID_DFLT, MSG_LEGACY_MED, "VSM_stream_set_min_max_rate_cmd_control(): "
           "media_type: 0x%08X, min rate: %d, max rate: %d",
           session_object->active_set.media_id, min_max_rate->min_rate, min_max_rate->max_rate );
    session_object->cmd_control.current_action = VSM_ACTION_ENUM_CACHE_AND_SET_1;

    break;
  }
  
  for ( ; ; )
  {
    switch ( session_object->cmd_control.current_action )
    {
      case VSM_ACTION_ENUM_CACHE_AND_SET_1:
      {
        vsm_utility_cache_voc_property( session_object, ( void* )min_max_rate,
                                        VSM_VOC_PROPERTY_ENUM_MINMAX_RATE );
        rc = vsm_action_set_cached_minmax_rate( session_object );
        if ( ADSP_FAILED( rc ) )
        {
          session_object->cmd_control.cmd_result = ADSP_EFAILED;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
          continue;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_WAIT_2;
          return ADSP_EOK;
        }
      }
      break;

      case VSM_ACTION_ENUM_WAIT_2:
      {
        if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
        {
          session_object->cmd_control.cmd_result = session_object->cmd_control.resp_result;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_COMPLETE;
        }
        continue;
      }
      break;

      case VSM_ACTION_ENUM_ERROR_HANDLE:
      {
        if ( TRUE == session_object->cmd_control.is_resp_pending )
        {
          return ADSP_EOK;
        }
        else
        {
          if ( ADSP_FAILED( session_object->cmd_control.cmd_result ) )
          {
            session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
          }
          else
          {
            session_object->cmd_control.current_action = VSM_ACTION_ENUM_COMPLETE;
          }
          continue;
        }
      }
      break;

      case VSM_ACTION_ENUM_REPORT_FAILURE:
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_stream_set_min_max_rate_cmd_control():"
               " failed  error = 0x%08X", session_object->cmd_control.cmd_result );
        rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, session_object->cmd_control.cmd_result );
        session_object->cmd_control.is_cmd_pending = FALSE;
        VSM_COMM_ERROR( rc, client_addr );
        return ADSP_EOK;
      }
      break;

      case VSM_ACTION_ENUM_COMPLETE:
      {
        rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EOK );
        VSM_COMM_ERROR( rc, client_addr );
        session_object->cmd_control.is_cmd_pending = FALSE;
        MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,"VSM_stream_set_min_max_rate_cmd_control: command Success:." );
        return ADSP_EOK;
      }
      break;

      default:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"VSM_stream_set_min_max_rate_cmd_control: ERROR: Unexpected..." );
      }
      return ADSP_EOK;
    }
  }
  return ADSP_EOK;
}

static int32_t VSM_stream_set_enc_rate_cmd_control (
  vsm_session_object_t* session_object
)
{
  int32_t rc = ADSP_EOK;
  vss_istream_cmd_voc_amr_set_enc_rate_t* in_args;
  vsm_enc_set_rate_t vsm_enc_rate_param;
  uint16_t client_addr;

  if( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  elite_apr_packet_t *apr_pkt = ( elite_apr_packet_t * )( session_object->cmd_control.cmd_msg.pPayload );
  client_addr = apr_pkt->src_addr;

  in_args = APRV2_PKT_GET_PAYLOAD( vss_istream_cmd_voc_amr_set_enc_rate_t, apr_pkt );

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "VSM_stream_set_enc_rate_cmd_control():"
        "session_portID = 0x%08X, ENC rate received = %d",
         apr_pkt->dst_port, in_args->mode);

  if ( ( sizeof( vss_istream_cmd_voc_amrwb_set_enc_rate_t ) ==
         sizeof( vss_istream_cmd_voc_qcelp13k_set_rate_t ) ) &&
       ( sizeof( vss_istream_cmd_voc_qcelp13k_set_rate_t ) ==
         sizeof( vss_istream_cmd_voc_amr_set_enc_rate_t ) ) &&
       ( sizeof( vss_istream_cmd_voc_amr_set_enc_rate_t ) ==
         sizeof( vss_istream_cmd_voc_4gvnb_set_rate_t ) ) &&
       ( sizeof( vss_istream_cmd_voc_4gvnb_set_rate_t ) ==
         sizeof( vss_istream_cmd_voc_4gvwb_set_rate_t ) ) &&
       ( sizeof( vss_istream_cmd_voc_4gvwb_set_rate_t ) ==
         sizeof( vss_istream_cmd_voc_4gvnw_set_rate_t ) ) &&
       ( sizeof( vss_istream_cmd_voc_4gvnw_set_rate_t ) ==
         sizeof( vss_istream_cmd_voc_4gvnw2k_set_rate_t ) ) )
  {
    //Good to go
  }
  else
  {
    /* All media types must have teh same structure if this is not true anymore then this function needs to accomodate */
    rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EBADPARAM );
    VSM_COMM_ERROR( rc, client_addr );
    return ADSP_EOK;
  }

  if ( ( session_object->apr_info.client_addr != apr_pkt->src_addr ) ||
       ( session_object->apr_info.client_port != apr_pkt->src_port ) )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_stream_set_enc_rate_cmd_control(): " \
           "command allowed for full control client only. client_src_port: 0x%04X, full_control_src_port: 0x%04X", 
           apr_pkt->src_port, session_object->apr_info.client_port );
    rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EHANDLE );
    VSM_COMM_ERROR( rc, client_addr );
    return ADSP_EOK;
  }

  /* Cache voc property. */
  vsm_utility_cache_voc_property( session_object, ( void* )in_args, VSM_VOC_PROPERTY_ENUM_ENC_RATE );

  vsm_action_set_enc_rate( session_object, in_args->mode );

  rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EOK );
  VSM_COMM_ERROR( rc, client_addr );

  return ADSP_EOK;
}

static int32_t VSM_set_enc_dtx_mode_control (
  vsm_session_object_t* session_object
)
{
  int32_t rc = ADSP_EOK;
  vss_istream_cmd_set_enc_dtx_mode_t* apr_payload;
  vsm_enc_set_dtx_mode_t out_args;
  uint16_t client_addr;
  bool_t is_full_ctrl = FALSE;

  if( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  elite_apr_packet_t *apr_pkt = ( elite_apr_packet_t * )( session_object->cmd_control.cmd_msg.pPayload );
  client_addr = apr_pkt->src_addr;

  apr_payload = APRV2_PKT_GET_PAYLOAD( vss_istream_cmd_set_enc_dtx_mode_t, apr_pkt );
  if( FALSE == vsm_utility_validate_apr_payload_size( apr_pkt ) )
  {
    rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EBADPARAM );
    VSM_COMM_ERROR( rc, client_addr );
    return ADSP_EBADPARAM;
  }
  
  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "VSM_set_enc_dtx_mode_control(): "
                 "Session_portID(0x%08X), DTX mode received(%d).", apr_pkt->dst_port, 
                 apr_payload->enable );

  if ( ( session_object->apr_info.client_addr == apr_pkt->src_addr ) &&
       ( session_object->apr_info.client_port == apr_pkt->src_port ) )
  {
    is_full_ctrl = TRUE;
  }

  vsm_utility_cache_dtx_mode( session_object, is_full_ctrl, apr_payload );
  vsm_utility_get_cached_dtx_mode( session_object, &out_args );

  if ( VSM_STREAM_PROPERTY_NOT_SET_UINT32 == out_args.dtx_mode )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "VSM_set_enc_dtx_mode_control(): "
           "DTX mode not set, session_portID(0x%08X), DTX mode received(%d)",
           apr_pkt->dst_port, apr_payload->enable );
    rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EOK );
    VSM_COMM_ERROR( rc, client_addr );
    return ADSP_EOK;
  }
  
  memscpy( apr_payload, sizeof( out_args ), &out_args, sizeof( out_args ) );
  vsm_action_forward_command_to_venc( session_object, VSS_ISTREAM_CMD_SET_ENC_DTX_MODE );
  
  return ADSP_EOK;
}

static int32_t VSM_set_hdvoice_config_cmd_control( vsm_session_object_t* session_object )
{
  int32_t rc = ADSP_EOK;
  vss_ihdvoice_cmd_set_config_t* in_args;
  uint16_t client_addr;
  enum
  {
    VSM_ACTION_ENUM_UNINITIALIZED,
    VSM_ACTION_ENUM_HDVOICE_DISABLE_1,
    VSM_ACTION_ENUM_WAIT_2,
    VSM_ACTION_ENUM_HDVOICE_ENABLE_3,    /** dynamic: doesn't need to calibrate **/
    VSM_ACTION_ENUM_WAIT_4,
    VSM_ACTION_ENUM_GET_KPPS_5,
    VSM_ACTION_ENUM_WAIT_6,
    VSM_ACTION_ENUM_SEND_RECONFIG_EVENT_7,
    VSM_ACTION_ENUM_ERROR_HANDLE,
    VSM_ACTION_ENUM_REPORT_FAILURE,
    VSM_ACTION_ENUM_COMPLETE,
    VSM_ACTION_ENUM_INVALID
  };

  if( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  elite_apr_packet_t *apr_pkt = ( elite_apr_packet_t * )( session_object->cmd_control.cmd_msg.pPayload );
  client_addr = apr_pkt->src_addr;

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "VSM_set_hdvoice_config_cmd_control(): session_portID = 0x%08X"
         " current_action %d", apr_pkt->dst_port, session_object->cmd_control.current_action );

  session_object->cmd_control.is_cmd_pending = TRUE;

  while ( VSM_ACTION_ENUM_UNINITIALIZED == session_object->cmd_control.current_action )
  {
    in_args = APRV2_PKT_GET_PAYLOAD( vss_ihdvoice_cmd_set_config_t, apr_pkt );

    if( FALSE == vsm_utility_validate_apr_payload_size( apr_pkt ) )
    {
      session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
      break;
    }

    session_object->target_set.system_config.feature = in_args->feature_id;

    if ( session_object->target_set.system_config.feature == session_object->active_set.system_config.feature )
    {
      /* No change in KPPS value. Clear the flag and complete the cmd */
      session_object->cmd_control.cmd_result = ADSP_EOK;
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_COMPLETE;
    }
    else
    {
      if ( VSS_ICOMMON_CAL_FEATURE_NONE == session_object->active_set.system_config.feature )
      {
        session_object->cmd_control.current_action = VSM_ACTION_ENUM_HDVOICE_ENABLE_3;
      }
      else
      {
        session_object->cmd_control.current_action = VSM_ACTION_ENUM_HDVOICE_DISABLE_1;
      }
    }
    break;
  }

  for ( ; ; )
  {
    switch ( session_object->cmd_control.current_action )
    {
      case VSM_ACTION_ENUM_HDVOICE_DISABLE_1:
      {
        rc = vsm_action_hdvoice_enable_disable( session_object, FALSE );
        if ( ADSP_FAILED( rc ) )
        {
          session_object->cmd_control.cmd_result = ADSP_EFAILED;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
          continue;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_WAIT_2;
          return ADSP_EOK;
        }
      }
      break;

      case VSM_ACTION_ENUM_WAIT_2:
      {
        if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
        {
          session_object->cmd_control.cmd_result = session_object->cmd_control.resp_result;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
        }
        else
        {
          session_object->active_set.system_config.feature = VSS_ICOMMON_CAL_FEATURE_NONE;

          /* Check if the target feature ID is None, or,
             if it is non-None, then is it supported */
          if ( ( VSS_ICOMMON_CAL_FEATURE_NONE == session_object->target_set.system_config.feature ) ||
               (FALSE == vsm_utility_is_hdvoice_feature_supported( session_object->target_set.system_config.feature ) ) )
          {
            /* No new feature needs to be enabled. Query the KPPS from dyn svcs. */
            session_object->cmd_control.current_action = VSM_ACTION_ENUM_GET_KPPS_5;
          }
          else
          {
            /* Target feature ID is non-None and it is supported, so enable it */
            session_object->cmd_control.current_action = VSM_ACTION_ENUM_HDVOICE_ENABLE_3;
          }
        }
        continue;
      }
      break;

      case VSM_ACTION_ENUM_HDVOICE_ENABLE_3:
      {
        rc = vsm_action_hdvoice_enable_disable( session_object, TRUE );
        if ( ADSP_FAILED( rc ) )
        {
          session_object->cmd_control.cmd_result = ADSP_EFAILED;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
          continue;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_WAIT_4;
          return ADSP_EOK;
        }
      }
      break;

      case VSM_ACTION_ENUM_WAIT_4:
      {
        session_object->cmd_control.cmd_result = session_object->cmd_control.resp_result;
        if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
        }
        else
        {
          session_object->active_set.system_config.feature = session_object->target_set.system_config.feature;

          if ( ( VSM_STATE_ENUM_RUN == session_object->state_control.state ) &&
               ( APR_NULL_V != session_object->attached_mvm_handle ) )
          {
            session_object->cmd_control.current_action = VSM_ACTION_ENUM_GET_KPPS_5;
          }
          else
          {
            session_object->cmd_control.current_action = VSM_ACTION_ENUM_COMPLETE;
          }
        }
        continue;
      }
      break;

      case VSM_ACTION_ENUM_GET_KPPS_5:
      {
        rc = vsm_action_get_kpps( session_object );
        if ( ADSP_FAILED( rc ) )
        {
          MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"VSM_set_hdvoice_config_cmd_control: GET KPPS failed...." );
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
          continue;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_WAIT_6;
          return ADSP_EOK;
        }
      }
      break;

      case VSM_ACTION_ENUM_WAIT_6:
      {
        if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
        {
          MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"VSM_set_hdvoice_config_cmd_control: GET KPPS failed...." );
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
        }
        else
        {
          if ( TRUE == session_object->is_kpps_changed )
          {
            session_object->cmd_control.current_action = VSM_ACTION_ENUM_SEND_RECONFIG_EVENT_7;
          }
          else
          {
            session_object->cmd_control.current_action = VSM_ACTION_ENUM_COMPLETE;
          }
        }
        continue;
      }
      break;

      case VSM_ACTION_ENUM_SEND_RECONFIG_EVENT_7:
      {
        rc = vsm_action_send_reconfig_event( session_object );
        VSM_COMM_ERROR( rc, client_addr );

        session_object->is_kpps_changed = FALSE;
        session_object->cmd_control.current_action = VSM_ACTION_ENUM_COMPLETE;
        continue;
      }
      break;

      case VSM_ACTION_ENUM_ERROR_HANDLE:
      {
        if ( TRUE == session_object->cmd_control.is_resp_pending )
        {
          return ADSP_EOK;
        }
        else
        {
          if ( ADSP_FAILED( session_object->cmd_control.cmd_result ) )
          {
            session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
          }
          else
          {
            session_object->cmd_control.current_action = VSM_ACTION_ENUM_COMPLETE;
          }
          continue;
        }
      }
      break;

      case VSM_ACTION_ENUM_REPORT_FAILURE:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"VSM_set_param_v2_cmd_control: command FAILED:." );
        elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, session_object->cmd_control.cmd_result );
        session_object->cmd_control.is_cmd_pending = FALSE;
        return ADSP_EOK;
      }
      break;

      case VSM_ACTION_ENUM_COMPLETE:
      {
        elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, session_object->cmd_control.cmd_result );

        session_object->cmd_control.is_cmd_pending = FALSE;

        MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,"VSM_set_param_v2_cmd_control: command Success:." );

        return ADSP_EOK;
      }
      break;

      default:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"VSM_set_param_v2_cmd_control: ERROR: Unexpected..." );
      }
      return ADSP_EOK;
    }
  }

  return ADSP_EOK;
}

static int32_t VSM_stream_set_system_config_cmd_control( vsm_session_object_t* session_object )
{
  ADSPResult rc = ADSP_EOK;
  uint32_t payload_size;
  vss_icommon_cmd_set_system_config_t* in_args;

  enum {
    VSM_ACTION_ENUM_UNINITIALIZED,
    VSM_ACTION_ENUM_CALIBRATE_COMMON_1, /* BACKWARD COMPATIBILITY */
    VSM_ACTION_ENUM_WAIT_2, /* BACKWARD COMPATIBILITY */
    VSM_ACTION_ENUM_HDVOICE_DISABLE_3,
    VSM_ACTION_ENUM_WAIT_4,
    VSM_ACTION_ENUM_HDVOICE_ENABLE_5,
    VSM_ACTION_ENUM_WAIT_6,
    VSM_ACTION_ENUM_CALIBRATE_STATIC_7,
    VSM_ACTION_ENUM_WAIT_8,
    VSM_ACTION_ENUM_SET_UI_PROPERTIES_9, /* BACKWARD COMPATIBILITY FOR WIDEVOICE */
    VSM_ACTION_ENUM_WAIT_10,
    VSM_ACTION_ENUM_GET_KPPS_11,
    VSM_ACTION_ENUM_WAIT_12,
    VSM_ACTION_ENUM_GET_AVSYNC_DELAYS_13,
    VSM_ACTION_ENUM_WAIT_14,
    VSM_ACTION_ENUM_SEND_RESPONSE_15,
    VSM_ACTION_ENUM_ERROR_HANDLE,
    VSM_ACTION_ENUM_REPORT_FAILURE,
    VSM_ACTION_ENUM_COMPLETE,
    VSM_ACTION_ENUM_INVALID
  };

  if( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  elite_apr_packet_t *apr_pkt = ( elite_apr_packet_t * )( session_object->cmd_control.cmd_msg.pPayload );

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "VSM_stream_set_system_config_cmd_control(): session_portID = 0x%08X"
         " current_action %d", apr_pkt->dst_port, session_object->cmd_control.current_action );

  session_object->cmd_control.is_cmd_pending = TRUE;

  while ( VSM_ACTION_ENUM_UNINITIALIZED == session_object->cmd_control.current_action )
  {
    if ( VSM_STATE_ENUM_RUN == session_object->state_control.state )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "VSM_stream_set_system_config_cmd_control(): not allowed when stream is running." );
      session_object->cmd_control.cmd_result = ADSP_EFAILED;
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
      break;
    }

    /* Validate input params. */
    payload_size = APRV2_PKT_GET_PAYLOAD_BYTE_SIZE( apr_pkt->header );
    if( payload_size != sizeof( vss_icommon_cmd_set_system_config_t ) )
    {
      session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_stream_set_system_config_cmd_control():"
             "invalid payload size. %d", payload_size );
      break;
    }
    in_args = APRV2_PKT_GET_PAYLOAD( vss_icommon_cmd_set_system_config_t, apr_pkt );

    session_object->target_set.system_config.network_id = in_args->network_id;
    session_object->target_set.system_config.media_id = in_args->media_id;
    session_object->target_set.system_config.rx_voc_op_mode = in_args->rx_voc_op_mode;
    session_object->target_set.system_config.tx_voc_op_mode = in_args->tx_voc_op_mode;
    session_object->target_set.system_config.feature = in_args->feature;
    session_object->target_set.system_config.vsid = in_args->vsid;
    session_object->target_set.system_config.vfr_mode = in_args->vfr_mode;
    session_object->target_set.system_config.call_num = in_args->call_num;

    session_object->target_set.system_config.dec_sr = in_args->dec_sr;
    session_object->target_set.system_config.enc_sr = in_args->enc_sr;
    session_object->target_set.system_config.rx_pp_sr = in_args->rx_pp_sr;
    session_object->target_set.system_config.tx_pp_sr = in_args->tx_pp_sr;

    if ( TRUE == vsm_is_calibration_required( session_object ) )
    {
      /* BACKWARD COMPATIBILITY. */
      session_object->common_cal.is_calibrate_needed = session_object->common_cal.is_registered;

      session_object->static_cal.is_calibrate_needed = session_object->static_cal.is_registered;
    }

    if ( session_object->common_cal.is_calibrate_needed )
    {
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_CALIBRATE_COMMON_1;
    }
    else
    {
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_HDVOICE_DISABLE_3;
    }
  }

  for( ; ; )
  {
    switch ( session_object->cmd_control.current_action )
    {
      case VSM_ACTION_ENUM_CALIBRATE_COMMON_1:
      {
        rc = vsm_action_calibrate_common( session_object );
        if ( ADSP_FAILED( rc ) )
        {
          session_object->cmd_control.cmd_result = ADSP_EFAILED;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
          continue;
        }
        else if ( TRUE != session_object->cmd_control.is_resp_pending )
        {
          /* No matching entries found/ No calibration required.*/
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_SET_UI_PROPERTIES_9;
          continue;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_WAIT_2;
          return ADSP_EOK;
        }
      }
      break;

      case VSM_ACTION_ENUM_WAIT_2:
      {
        if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
        {
          MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_stream_set_system_config_cmd_control():"
                 " common calibration error: %d.", session_object->cmd_control.resp_result );
        }
        session_object->cmd_control.current_action = VSM_ACTION_ENUM_SET_UI_PROPERTIES_9;
        continue;
      }
      break;

      case VSM_ACTION_ENUM_HDVOICE_DISABLE_3:
      {
        rc = vsm_action_hdvoice_enable_disable( session_object, FALSE );

        if ( ADSP_FAILED( rc ) )
        {
          /* Jump to next action */
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_HDVOICE_ENABLE_5;
          continue;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_WAIT_4;
          return ADSP_EOK;
        }
      }
      break;

      case VSM_ACTION_ENUM_WAIT_4:
      {
        if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
        {
           MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
                  "VSM_stream_set_system_config_cmd_control(): hdvoice disable "
                  "failed, dyn svc ack error: 0x%lx",
                  session_object->cmd_control.resp_result );
        }

        session_object->cmd_control.current_action = VSM_ACTION_ENUM_HDVOICE_ENABLE_5;
        continue;
      }
      break;

      case VSM_ACTION_ENUM_HDVOICE_ENABLE_5:
      {
        rc = vsm_action_hdvoice_enable_disable( session_object, TRUE );

        if ( ADSP_FAILED( rc ) )
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_CALIBRATE_STATIC_7;
          continue;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_WAIT_6;
          return ADSP_EOK;
        }
      }
      break;

      case VSM_ACTION_ENUM_WAIT_6:
      {
        session_object->cmd_control.current_action = VSM_ACTION_ENUM_CALIBRATE_STATIC_7;

        if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
        {
          MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_stream_set_system_config_cmd_control():"
                 " hdvoice enable failed, dyn svc ack error: 0x%lx",
                   session_object->cmd_control.resp_result );
        }
        /* Fall through */
      }
      break;

      case VSM_ACTION_ENUM_CALIBRATE_STATIC_7:
      {
        if ( TRUE == session_object->static_cal.is_calibrate_needed )
        {
           rc = vsm_action_calibrate_static( session_object );
        }
        else
        {
           session_object->cmd_control.current_action = VSM_ACTION_ENUM_SET_UI_PROPERTIES_9;
           continue;
        }
        if ( ADSP_FAILED( rc ) )
        {
          session_object->cmd_control.cmd_result = ADSP_EFAILED;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
          continue;
        }
        else if ( TRUE != session_object->cmd_control.is_resp_pending )
        {
          /* No matching entries found/ No calibration required.*/
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_SET_UI_PROPERTIES_9;
          continue;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_WAIT_8;
          return ADSP_EOK;
        }
      }
      break;

      case VSM_ACTION_ENUM_WAIT_8:
      {
        if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
        {
          MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_stream_set_system_config_cmd_control():"
                 " static calibration error: %d.", session_object->cmd_control.resp_result );
        }
        ( void ) cvd_cal_query_deinit( session_object->static_cal.query_handle );
        session_object->static_cal.query_handle = CVD_CAL_INVALID_HANDLE_V;
        session_object->cmd_control.current_action = VSM_ACTION_ENUM_SET_UI_PROPERTIES_9;
        continue;
      }
      break;

      case VSM_ACTION_ENUM_SET_UI_PROPERTIES_9:
      {
        rc = vsm_action_set_ui_properties( session_object );
        if ( ADSP_FAILED( rc ) )
        {
          session_object->cmd_control.cmd_result = ADSP_EFAILED;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
          continue;
        }
        else if ( TRUE != session_object->cmd_control.is_resp_pending )
        {
          /* No matching entries found/ No calibration required.*/
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_GET_KPPS_11;
          continue;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_WAIT_10;
          return ADSP_EOK;
        }
      }
      break;

      case VSM_ACTION_ENUM_WAIT_10:
      {
        if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
        {
          MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_stream_set_system_config_cmd_control():"
                 " set_ui_properties error: %d.", session_object->cmd_control.resp_result );
        }
        session_object->cmd_control.current_action = VSM_ACTION_ENUM_GET_KPPS_11;
        continue;
      }
      break;

      case VSM_ACTION_ENUM_GET_KPPS_11:
      {
        rc = vsm_action_get_kpps( session_object );
        if ( ADSP_FAILED( rc ) )
        {
          session_object->cmd_control.cmd_result = ADSP_EFAILED;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
          continue;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_WAIT_12;
          return ADSP_EOK;
        }
      }
      break;

      case VSM_ACTION_ENUM_WAIT_12:
      {
        if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
        {
          session_object->cmd_control.cmd_result = session_object->cmd_control.resp_result;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
        }
        else
        {
          session_object->is_kpps_changed = FALSE;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_GET_AVSYNC_DELAYS_13;
        }
        continue;
      }
      break;

      case VSM_ACTION_ENUM_GET_AVSYNC_DELAYS_13:
      {
        rc = vsm_action_get_avsync_delays( session_object );
        if ( ADSP_FAILED( rc ) )
        {
          session_object->cmd_control.cmd_result = ADSP_EFAILED;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
          continue;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_WAIT_14;
          return ADSP_EOK;
        }
      }
      break;

      case VSM_ACTION_ENUM_WAIT_14:
      {
        if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
        {
          session_object->cmd_control.cmd_result = session_object->cmd_control.resp_result;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_SEND_RESPONSE_15;
        }
        continue;
      }
      break;

      case VSM_ACTION_ENUM_SEND_RESPONSE_15:
      {
        rc = vsm_action_send_system_config_response( session_object );
        session_object->cmd_control.current_action = VSM_ACTION_ENUM_COMPLETE;
        continue;
      }
      break;

      case VSM_ACTION_ENUM_ERROR_HANDLE:
      {
        if ( TRUE == session_object->cmd_control.is_resp_pending )
        {
          return ADSP_EOK;
        }
        else
        {
          //Do error handling
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
          continue;
        }
      }
      break;

      case VSM_ACTION_ENUM_REPORT_FAILURE:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"VSM_stream_set_system_config_cmd_control: command FAILED:." );
        elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, session_object->cmd_control.cmd_result );
        session_object->cmd_control.is_cmd_pending = FALSE;
        return ADSP_EOK;
      }
      break;

      case VSM_ACTION_ENUM_COMPLETE:
      {
        /* BACKWARD COMPATIBILITY. */
        session_object->common_cal.is_calibrate_needed = FALSE;
        session_object->static_cal.is_calibrate_needed = FALSE;

        MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,"VSM_stream_set_system_config_cmd_control: command Success:." );
       ( void ) elite_apr_if_free( vsm_apr_handle, apr_pkt );
        session_object->cmd_control.is_cmd_pending = FALSE;
        return ADSP_EOK;
      }
      break;

      default:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"VSM_stream_set_system_config_cmd_control: ERROR: Unexpected..." );
      }
      return ADSP_EOK;
    }
  }

}

static int32_t VSM_stream_deregister_static_calibration_data_cmd_ctrl (
  vsm_session_object_t* session_object
)
{
  int32_t rc = ADSP_EOK;
  uint16_t client_addr;

  if( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }
  elite_apr_packet_t *apr_pkt = ( elite_apr_packet_t * )( session_object->cmd_control.cmd_msg.pPayload );

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED, "VSM_stream_deregister_static_calibration_data_cmd_ctrl():"
         "  session_portID = 0x%08X", apr_pkt->dst_port );

  for ( ;; )
  {
    client_addr = apr_pkt->src_addr;

    if ( FALSE == session_object->static_cal.is_registered )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_stream_deregister_static_calibration_data_cmd_ctrl(): Calibration not registered." );
      rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EALREADY );
      VSM_COMM_ERROR( rc, client_addr );
      break;
    }

    rc = cvd_cal_discard_table_v2( session_object->static_cal.table_handle );
    if ( rc )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_stream_deregister_static_calibration_data_cmd_ctrl(): Failed to deregister calibration data." );
      rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EFAILED );
      VSM_COMM_ERROR( rc, client_addr );
      break;
    }
    else
    {
      session_object->static_cal.is_registered = FALSE;
      session_object->static_cal.is_calibrate_needed = FALSE;
      rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EOK );
      VSM_COMM_ERROR( rc, client_addr );
    }

    break;
  }

  MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,
       "VSM_stream_deregister_static_calibration_data_cmd_ctrl: command Success:." );

  return ADSP_EOK;
}

static int32_t VSM_stream_register_static_calibration_data_cmd_ctrl (
  vsm_session_object_t* session_object
)
{
  int32_t rc = ADSP_EOK;
  vss_istream_cmd_register_static_calibration_data_t* payload;
  uint32_t payload_len;
  cvd_virt_addr_t cal_virt_addr;
  cvd_cal_table_descriptor_t cal_table_descriptor;
  uint16_t client_addr;
  cvd_cal_log_commit_info_t log_info;
  cvd_cal_log_table_header_t log_info_table;

  bool_t is_mapped_memory_in_range = FALSE;
  /* Flag to indicate if memory range of mem_map command
   * and cal registration are in sync
   */
  if( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }
  elite_apr_packet_t *apr_pkt = ( elite_apr_packet_t * )( session_object->cmd_control.cmd_msg.pPayload );

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED, "VSM_stream_register_static_calibration_data_cmd_ctrl():"
         " session_portID = 0x%08X", apr_pkt->dst_port );

  for ( ;; )
  {
    client_addr = apr_pkt->src_addr;

    if ( session_object->static_cal.is_registered == TRUE )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_stream_register_static_calibration_data_cmd_ctrl(): Calibration data already registered." );
      rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EALREADY );
      VSM_COMM_ERROR( rc, client_addr );
      break;
    }

    if ( session_object->common_cal.is_registered == TRUE )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_stream_register_static_calibration_data_cmd_ctrl():"
           " Calibration data already registered via deprecated registration command." );
      rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EALREADY );
      VSM_COMM_ERROR( rc, client_addr );
      break;
    }

    payload = APRV2_PKT_GET_PAYLOAD( vss_istream_cmd_register_static_calibration_data_t, apr_pkt );
    payload_len = APRV2_PKT_GET_PAYLOAD_BYTE_SIZE( apr_pkt->header );

    if ( payload_len < sizeof( vss_istream_cmd_register_static_calibration_data_t ) )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_stream_register_static_calibration_data_cmd_ctrl():"
             " Unexpected payload size, %d < %d.", payload_len, sizeof( vss_istream_cmd_register_static_calibration_data_t ) );
      rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EBADPARAM );
      VSM_COMM_ERROR( rc, client_addr );
      break;
    }

    rc = cvd_mem_mapper_validate_handle( payload->cal_mem_handle );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_stream_register_static_calibration_data_cmd_ctrl():"
             " Invalid cal_mem_handle: 0x%08X.", payload->cal_mem_handle );
      rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EHANDLE );
      VSM_COMM_ERROR( rc, client_addr );
      break;
    }

    rc = cvd_mem_mapper_validate_attributes_align( payload->cal_mem_handle, payload->cal_mem_address );
    if ( rc )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
        "VSM_stream_register_static_calibration_data_cmd_ctrl(): Mis-aligned cal_mem_address:"
        " lsw: 0x%08X, msw: 0x%08X", ( uint32_t )payload->cal_mem_address,
        ( uint32_t )( payload->cal_mem_address >> 32 ) );
      rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EBADPARAM );
      VSM_COMM_ERROR( rc, client_addr );
      break;
    }

    rc = cvd_mem_mapper_validate_mem_is_in_region( payload->cal_mem_handle, payload->cal_mem_address,
                                                   payload->cal_mem_size );
    if ( rc )
    {
      MSG_4( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
        "VSM_stream_register_static_calibration_data_cmd_ctrl(): Memory is not within range,"
        " cal_mem_handle: 0x%08X, addr: lsw: 0x%08X, msw: 0x%08X, size: %d",
        payload->cal_mem_handle, ( uint32_t )payload->cal_mem_address,
        ( uint32_t )( payload->cal_mem_address >> 32 ), payload->cal_mem_size );
      rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EBADPARAM );
      VSM_COMM_ERROR( rc, client_addr );
      break;
    }

    rc = cvd_mem_mapper_get_virtual_addr_v2( payload->cal_mem_handle,
                                             payload->cal_mem_address,
                                             &cal_virt_addr );
    if ( rc )
    {
      MSG_3( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
        "VSM_stream_register_static_calibration_data_cmd_ctrl(): Cannot get virtual address,"
        " mem_handle: 0x%08X, addr: lsw: 0x%08X, msw: 0x%08X", payload->cal_mem_handle,
        ( uint32_t )payload->cal_mem_address, ( uint32_t )( payload->cal_mem_address >> 32 ) );
      rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EFAILED );
      VSM_COMM_ERROR( rc, client_addr );
      break;
    }

    is_mapped_memory_in_range = TRUE;

    ( void ) cvd_mem_mapper_cache_invalidate_v2( &cal_virt_addr, payload->cal_mem_size );

    cal_table_descriptor.start_ptr = cal_virt_addr.ptr;
    cal_table_descriptor.size = payload->cal_mem_size;
    cal_table_descriptor.data_mem_handle = payload->cal_mem_handle;
    cal_table_descriptor.indexing_mem_handle = ( uint32_t ) NULL;
    cal_table_descriptor.num_columns = payload->num_columns;
    cal_table_descriptor.columns = ( ( cvd_cal_column_descriptor_t* )
                                     ( ( ( uint8_t* ) payload ) +
                                       sizeof ( vss_istream_cmd_register_static_calibration_data_t ) ) );

    rc = cvd_cal_parse_table_v2( &cal_table_descriptor,
                                 &session_object->static_cal.table_handle );
    if ( rc )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvs_stream_register_static_calibration_data_cmd_ctrl(): Error in parsing the cal table." );
      rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EFAILED );
      VSM_COMM_ERROR( rc, client_addr );
      break;
    }

    session_object->static_cal.is_calibrate_needed = TRUE;
    session_object->static_cal.is_registered = TRUE;

    session_object->is_stream_config_changed = TRUE;

      rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EOK );
    VSM_COMM_ERROR( rc, client_addr );

    break;
  }

  /* Log stream static cal table. */
  if (is_mapped_memory_in_range == TRUE)
  {
    log_info_table.table_handle = session_object->static_cal.table_handle;

    log_info.instance = ( ( session_object->attached_mvm_handle << 16 ) | 
                          (  session_object->apr_info.self_port ) );
    log_info.call_num = session_object->target_set.system_config.call_num;
    log_info.data_container_id = CVD_CAL_LOG_DATA_CONTAINER_TABLE;
    log_info.data_container_header_size = sizeof ( log_info_table );
    log_info.data_container_header = &log_info_table;
    log_info.payload_size = cal_table_descriptor.size;
    log_info.payload_buf = cal_table_descriptor.start_ptr;

    cvd_cal_log_data ( ( log_code_type )LOG_ADSP_CVD_CAL_DATA_C, CVD_CAL_LOG_STREAM_STATIC_TABLE,
                              ( void* )&log_info, sizeof( log_info ) );
  }

  MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,
       "VSM_stream_register_static_calibration_data_cmd_ctrl: command Success:." );

  return ADSP_EOK;
}

/* BACKWARD COMPATIBILITY */
static int32_t VSM_stream_deregister_calibration_data_cmd_ctrl (
  vsm_session_object_t* session_object
)
{
  int32_t rc = ADSP_EOK;
  uint16_t client_addr;

  if( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }
  elite_apr_packet_t *apr_pkt = ( elite_apr_packet_t * )( session_object->cmd_control.cmd_msg.pPayload );

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED, "VSM_stream_deregister_calibration_data_cmd_ctrl():"
         " session_portID = 0x%08X", apr_pkt->dst_port );

  for ( ;; )
  {
    client_addr = apr_pkt->src_addr;

    if ( FALSE == session_object->common_cal.is_registered )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_stream_deregister_calibration_data_cmd_ctrl(): Calibration not registered." );
      rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EALREADY );
      VSM_COMM_ERROR( rc, client_addr );
      break;
    }

    rc = cvd_cal_discard_table( session_object->common_cal.table_handle );
    if ( rc )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_stream_deregister_calibration_data_cmd_ctrl(): Failed to deregister calibration data." );
      rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EFAILED );
      VSM_COMM_ERROR( rc, client_addr );
      break;
    }
    else
    {
      session_object->common_cal.is_registered = FALSE;
      session_object->common_cal.is_evaluated = FALSE;
      session_object->common_cal.is_calibrate_needed = FALSE;
      rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EOK );
      VSM_COMM_ERROR( rc, client_addr );
    }

    break;
  }

  MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,
       "VSM_stream_deregister_calibration_data_cmd_ctrl: command Success:." );

  return ADSP_EOK;
}

static int32_t VSM_stream_register_calibration_data_v2_cmd_control (
  vsm_session_object_t* session_object
)
{
  int32_t rc = ADSP_EOK;
  vss_istream_cmd_register_calibration_data_v2_t* payload;
  uint32_t payload_len;
  cvd_virt_addr_t cal_virt_addr;
  cvd_cal_table_descriptor_t cal_table_descriptor;
  uint32_t client_addr;

  if( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }
  elite_apr_packet_t *apr_pkt = ( elite_apr_packet_t * )( session_object->cmd_control.cmd_msg.pPayload );

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED, "VSM_stream_register_calibration_data_v2_cmd_control():"
         " session_portID = 0x%08X", apr_pkt->dst_port );

  for ( ;; )
  {
    client_addr = apr_pkt->src_addr;

    if ( session_object->common_cal.is_registered == TRUE )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_stream_register_calibration_data_v2_cmd_control(): Calibration already registered." );
      rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EALREADY );
      VSM_COMM_ERROR( rc, client_addr );
      break;
    }

    if ( session_object->static_cal.is_registered == TRUE )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_stream_register_calibration_data_v2_cmd_control():"
           " Calibration already registered via static cal registration command." );
      rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EALREADY );
      VSM_COMM_ERROR( rc, client_addr );
      break;
    }

    payload = APRV2_PKT_GET_PAYLOAD( vss_istream_cmd_register_calibration_data_v2_t, apr_pkt );
    payload_len = APRV2_PKT_GET_PAYLOAD_BYTE_SIZE( apr_pkt->header );

    if ( payload_len < sizeof( vss_istream_cmd_register_calibration_data_v2_t ) )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_stream_register_calibration_data_v2_cmd_control():"
             " Unexpected payload size, %d !< %d.", payload_len, sizeof( vss_istream_cmd_register_calibration_data_v2_t ) );
      rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EBADPARAM );
      VSM_COMM_ERROR( rc, client_addr );
      break;
    }

    rc = cvd_mem_mapper_validate_handle( payload->cal_mem_handle );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_stream_register_calibration_data_v2_cmd_control():"
             " Invalid cal_mem_handle: 0x%08X.", payload->cal_mem_handle );
      rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EHANDLE );
      VSM_COMM_ERROR( rc, client_addr );
      break;
    }

    rc = cvd_mem_mapper_validate_attributes_align( payload->cal_mem_handle, payload->cal_mem_address );
    if ( rc )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
        "VSM_stream_register_calibration_data_v2_cmd_control(): Mis-aligned cal_mem_address:"
        " lsw: 0x%08X, msw: 0x%08X", ( uint32_t )payload->cal_mem_address,
        ( uint32_t )( payload->cal_mem_address >> 32 ) );
      rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EBADPARAM );
      VSM_COMM_ERROR( rc, client_addr );
      break;
    }

    rc = cvd_mem_mapper_validate_mem_is_in_region( payload->cal_mem_handle, payload->cal_mem_address,
                                                   payload->cal_mem_size );
    if ( rc )
    {
      MSG_4( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
        "VSM_stream_register_calibration_data_v2_cmd_control(): Memory is not within range,"
        " cal_mem_handle: 0x%08X, addr: lsw: 0x%08X, msw: 0x%08X, size: %d",
        payload->cal_mem_handle, ( uint32_t )payload->cal_mem_address,
        ( uint32_t )( payload->cal_mem_address >> 32 ), payload->cal_mem_size );
      rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EBADPARAM );
      VSM_COMM_ERROR( rc, client_addr );
      break;
    }

    rc = cvd_mem_mapper_get_virtual_addr_v2( payload->cal_mem_handle,
                                             payload->cal_mem_address,
                                             &cal_virt_addr );
    if ( rc )
    {
      MSG_3( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
        "VSM_stream_register_calibration_data_v2_cmd_control(): Cannot get virtual address,"
        " mem_handle: 0x%08X, addr: lsw: 0x%08X, msw: 0x%08X", payload->cal_mem_handle,
        ( uint32_t )payload->cal_mem_address, ( uint32_t )( payload->cal_mem_address >> 32 ) );
      rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EFAILED );
      VSM_COMM_ERROR( rc, client_addr );
      break;
    }

    ( void ) cvd_mem_mapper_get_vsm_mem_handle( payload->cal_mem_handle,
                                                &session_object->common_cal.vsm_mem_handle );

    ( void ) cvd_mem_mapper_cache_invalidate_v2( &cal_virt_addr, payload->cal_mem_size );

    cal_table_descriptor.start_ptr = cal_virt_addr.ptr;
    cal_table_descriptor.size = payload->cal_mem_size;
    cal_table_descriptor.data_mem_handle = payload->cal_mem_handle;
    cal_table_descriptor.indexing_mem_handle = APR_NULL_V;
    cal_table_descriptor.num_columns = payload->num_columns;
    cal_table_descriptor.columns = ( ( cvd_cal_column_descriptor_t* )
                                     ( ( ( uint8_t* ) payload ) +
                                       sizeof ( vss_istream_cmd_register_calibration_data_v2_t ) ) );

    rc = cvd_cal_parse_table( 0, 0, &cal_table_descriptor,
                              &session_object->common_cal.table_handle );
    if ( rc )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_stream_register_calibration_data_v2_cmd_control():"
           " Error in parsing the cal table." );
      rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EFAILED );
      VSM_COMM_ERROR( rc, client_addr );
      break;
    }

    session_object->common_cal.is_calibrate_needed = TRUE;
    session_object->common_cal.is_registered = TRUE;

    session_object->is_stream_config_changed = TRUE;

    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED, "VSM_stream_register_calibration_data_v2_cmd_control(): Table size reported: %d",
                                          cal_table_descriptor.size );

    rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EOK );
    VSM_COMM_ERROR( rc, client_addr );

  MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,
       "VSM_stream_register_calibration_data_v2_cmd_control: command Success:." );

    break;
  }

  return ADSP_EOK;
}

static int32_t VSM_session_ssr_cleanup_cmd_control ( vsm_session_object_t* session_object )
{

  ADSPResult rc = ADSP_EOK;
  vss_issr_cmd_cleanup_t  *in_args;
  elite_apr_packet_t* apr_pkt;
  elite_apr_packet_t *apr_payload_pkt;

  enum
  {
    VSM_ACTION_ENUM_UNINITIALIZED,
    VSM_ACTION_ENUM_CLEANUP_PASSIVE_CLIENTS_1,
    VSM_ACTION_ENUM_STOP_VPCM_2,
    VSM_ACTION_ENUM_WAIT_3,
    VSM_ACTION_ENUM_RESET_PACKET_EXCHANGE_MODE_TO_INBAND_4,
    VSM_ACTION_ENUM_WAIT_5,
    VSM_ACTION_ENUM_CLEAR_MAILBOX_CONFIGURATION_6,
    VSM_ACTION_ENUM_CLEAR_OOB_CONFIGURATION_7,
    VSM_ACTION_ENUM_CLEAR_EVENT_CLASS_REGISTRATIONS_8,
    VSM_ACTION_ENUM_CLOSE_MIXER_9,
    VSM_ACTION_ENUM_WAIT_10,
    VSM_ACTION_ENUM_ERROR_HANDLE,
    VSM_ACTION_ENUM_REPORT_FAILURE,
    VSM_ACTION_ENUM_COMPLETE,
    VSM_ACTION_ENUM_INVALID
  };

  if( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  apr_pkt = ( elite_apr_packet_t* )session_object->cmd_control.cmd_msg.pPayload;
  in_args = ( vss_issr_cmd_cleanup_t* )elite_apr_if_get_payload_ptr( apr_pkt );
  session_object->cmd_control.is_cmd_pending = TRUE;

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "VSM_session_ssr_cleanup_cmd_control(): Session portID = 0x%08X, current_action %d",
         session_object->apr_info.self_port, session_object->cmd_control.current_action );

  while ( VSM_ACTION_ENUM_UNINITIALIZED == session_object->cmd_control.current_action )
  {
    session_object->cmd_control.current_action = VSM_ACTION_ENUM_CLEANUP_PASSIVE_CLIENTS_1;
    break;
  }

  for ( ; ; )
  {
    switch ( session_object->cmd_control.current_action )
    {
      case VSM_ACTION_ENUM_CLEANUP_PASSIVE_CLIENTS_1:
      {
        vsm_action_cleanup_ssr_domain_passive_clients( in_args->domain_id, session_object );

        if ( ( TRUE == session_object->vpcm_info.is_enabled ) &&
             ( APR_GET_FIELD(APRV2_PKT_DOMAIN_ID, session_object->vpcm_info.client_addr ) == in_args->domain_id ) )
        {
          rc = elite_apr_if_alloc_cmd( vsm_apr_handle,
                     session_object->apr_info.self_addr, session_object->apr_info.self_port,
                     session_object->apr_info.self_addr, session_object->apr_info.self_port,
                     0, VOICE_CMD_STOP_HOST_PCM,
                     NULL, &apr_payload_pkt );
          if( ADSP_FAILED( rc ) )
          {
            session_object->cmd_control.cmd_result = ADSP_EFAILED;
            session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
          }
          else
          {
            session_object->cmd_control.current_action = VSM_ACTION_ENUM_STOP_VPCM_2;
          }
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_WAIT_3;
        }
        continue;
      }
      break;

      case VSM_ACTION_ENUM_STOP_VPCM_2:
      {
        rc = vsm_action_send_start_stop_host_pcm( session_object, apr_payload_pkt );
        session_object->vpcm_info.client_addr = APRV2_PKT_INIT_ADDR_V;
        session_object->vpcm_info.client_handle = APR_NULL_V;
        session_object->vpcm_info.cvs_handle = APR_NULL_V;
        session_object->vpcm_info.mem_handle = 0;

        if ( ADSP_FAILED( rc ) )
        {
          session_object->cmd_control.cmd_result = ADSP_EFAILED;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
          continue;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_WAIT_3;
          return ADSP_EOK;
        }
      }
      break;

      case VSM_ACTION_ENUM_WAIT_3:
      {
        if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
        {
          session_object->cmd_control.cmd_result = session_object->cmd_control.resp_result;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
          continue;
        }

        if ( APR_GET_FIELD(APRV2_PKT_DOMAIN_ID, session_object->apr_info.client_addr ) 
               == in_args->domain_id )
        {
          if ( 1 == session_object->ref_count)
          {
            session_object->cmd_control.current_action = VSM_ACTION_ENUM_CLOSE_MIXER_9;
          }
          else if ( VSS_IPKTEXG_MODE_IN_BAND != session_object->packet_exchange_info.mode )
          {
            session_object->cmd_control.current_action = VSM_ACTION_ENUM_RESET_PACKET_EXCHANGE_MODE_TO_INBAND_4;
          }
          else
          {
            /* No cleaning required for in-band packet exchange configuration.
               Reset Event class registrations, reduce the reference count and end command.
            */
            session_object->cmd_control.current_action = VSM_ACTION_ENUM_CLEAR_EVENT_CLASS_REGISTRATIONS_8;
          }
        }
        else if ( 0 == session_object->ref_count )
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_CLOSE_MIXER_9;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_COMPLETE;
        }
        continue;
      }
      break;

      case VSM_ACTION_ENUM_RESET_PACKET_EXCHANGE_MODE_TO_INBAND_4:
      {
        rc = vsm_action_set_packet_exchange_mode( session_object, VSS_IPKTEXG_MODE_IN_BAND );

        if ( ADSP_FAILED( rc ) )
        {
          session_object->cmd_control.cmd_result = ADSP_EFAILED;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
          continue;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_WAIT_5;
          return ADSP_EOK;
        }
      }
      break;

      case VSM_ACTION_ENUM_WAIT_5:
      {
        if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
        {
          session_object->cmd_control.cmd_result = session_object->cmd_control.resp_result;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
        }
        else
        {
          if ( TRUE == session_object->packet_exchange_info.mailbox_info.is_configured )
          {
           session_object->cmd_control.current_action = VSM_ACTION_ENUM_CLEAR_MAILBOX_CONFIGURATION_6;
          }
          else if ( TRUE == session_object->packet_exchange_info.oob_info.is_configured )
          {
            session_object->cmd_control.current_action = VSM_ACTION_ENUM_CLEAR_OOB_CONFIGURATION_7;
          }
          else
          {
            session_object->cmd_control.current_action = VSM_ACTION_ENUM_CLEAR_EVENT_CLASS_REGISTRATIONS_8;
          }
        }
        continue;
      }
      break;

      case VSM_ACTION_ENUM_CLEAR_MAILBOX_CONFIGURATION_6:
      {
        vsm_action_clear_mailbox_configuration( session_object );
        session_object->cmd_control.current_action = VSM_ACTION_ENUM_CLEAR_EVENT_CLASS_REGISTRATIONS_8;
        continue;
      }
      break;

      case VSM_ACTION_ENUM_CLEAR_OOB_CONFIGURATION_7:
      {
        vsm_action_clear_oob_configuration( session_object );
        session_object->cmd_control.current_action = VSM_ACTION_ENUM_CLEAR_EVENT_CLASS_REGISTRATIONS_8;
        continue;
      }
      break;

      case VSM_ACTION_ENUM_CLEAR_EVENT_CLASS_REGISTRATIONS_8:
      {
        vsm_action_ssr_clear_event_class_registrations_oob_tty( session_object, in_args->domain_id );
        session_object->cmd_control.current_action = VSM_ACTION_ENUM_COMPLETE;
        continue;
      }
      break;

      case VSM_ACTION_ENUM_CLOSE_MIXER_9:
      {
        rc = vsm_action_close_mixer_input_output( session_object );

        if ( ADSP_FAILED( rc ) )
        {
          session_object->cmd_control.cmd_result = ADSP_EFAILED;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
          continue;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_WAIT_10;
          return ADSP_EOK;
        }
      }
      break;

      case VSM_ACTION_ENUM_WAIT_10:
      {
        if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
        {
          session_object->cmd_control.cmd_result = session_object->cmd_control.resp_result;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_COMPLETE;
        }
        continue;
      }
      break;

      case VSM_ACTION_ENUM_ERROR_HANDLE:
      {
        if ( TRUE == session_object->cmd_control.is_resp_pending )
        {
          return ADSP_EOK;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
          continue;
        }
      }
      break;

      case VSM_ACTION_ENUM_REPORT_FAILURE:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"VSM_session_ssr_cleanup_cmd_control: command FAILED:." );
        elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, session_object->cmd_control.cmd_result );
        session_object->cmd_control.is_cmd_pending = FALSE;
        return ADSP_EOK;;
      }
      break;

      case VSM_ACTION_ENUM_COMPLETE:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,"VSM_session_ssr_cleanup_cmd_control: command Success:." );

        if ( ( APR_GET_FIELD(APRV2_PKT_DOMAIN_ID, session_object->apr_info.client_addr ) == in_args->domain_id ) )
        {
          session_object->apr_info.client_addr = 0;
          session_object->apr_info.client_port = 0;
          session_object->ref_count--;
        }

        session_object->cmd_control.cmd_result = ADSP_EOK;
        session_object->cmd_control.is_cmd_pending = FALSE;

        if ( 0 == session_object->ref_count )
        {
          ( void )vsm_action_free_session_resources( session_object );
        }
        //Send response to vsm service
        rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EOK );
        return ADSP_EOK;;
      }
      break;

      default:
        MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,"VSM_session_ssr_cleanup_cmd_control: ERROR: Unexpected:." );
      break;
    }
  }

}

static int32_t VSM_stream_disable_cmd_control( vsm_session_object_t* session_object )
{
  ADSPResult rc = ADSP_EOK;
  uint32_t payload_size;

  enum
  {
    VSM_ACTION_ENUM_UNINITIALIZED,
    VSM_ACTION_ENUM_STOP_RECORD_1,
    VSM_ACTION_ENUM_STOP_PLAYBACK_2,
    VSM_ACTION_ENUM_STOP_SERVICES_3,
    VSM_ACTION_ENUM_WAIT_4,
    VSM_ACTION_ENUM_STOP_PORTS_5,
    VSM_ACTION_ENUM_WAIT_6,
    VSM_ACTION_ENUM_SEND_DISABLE_NOTIFICATIONS_TO_CLIENT_7,
    VSM_ACTION_ENUM_ERROR_HANDLE,
    VSM_ACTION_ENUM_REPORT_FAILURE,
    VSM_ACTION_ENUM_COMPLETE,
    VSM_ACTION_ENUM_INVALID
  };

  if( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  elite_apr_packet_t *apr_pkt = ( elite_apr_packet_t * )( session_object->cmd_control.cmd_msg.pPayload );

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "VSM_stream_disable_cmd_control():  session_portID = 0x%08X,"
         " current_action %d", apr_pkt->dst_port, session_object->cmd_control.current_action );

  session_object->cmd_control.is_cmd_pending = TRUE;

  while ( VSM_ACTION_ENUM_UNINITIALIZED == session_object->cmd_control.current_action )
  {
    if ( VSM_STATE_ENUM_RUN != session_object->state_control.state )
    {
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_COMPLETE;
      break;
    }

    if ( session_object->active_set.record.enable_flag == VSM_ENABLED )
    {
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_STOP_RECORD_1;
    }
    else if ( session_object->active_set.playback.enable_flag == VSM_ENABLED )
    {
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_STOP_PLAYBACK_2;
    }
    else
    {
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_STOP_SERVICES_3;
    }
    break;
  }

  for ( ; ; )
  {
    switch ( session_object->cmd_control.current_action )
    {
      case VSM_ACTION_ENUM_STOP_RECORD_1:
      {
        if ( VSM_ENABLED == session_object->active_set.record.enable_flag )
        {
          rc = vsm_utility_record_control_stop( session_object );
          if ( ADSP_EPENDING == rc )
          {
            return ADSP_EOK;
          }
          else if ( ADSP_FAILED( rc ) )
          {
            session_object->cmd_control.cmd_result = ADSP_EFAILED;
            session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
            continue;
          }
          else
          {
            session_object->cmd_control.current_action = VSM_ACTION_ENUM_STOP_PLAYBACK_2;
            continue;
          }
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_STOP_PLAYBACK_2;
          continue;
        }
      }
      break;

      case VSM_ACTION_ENUM_STOP_PLAYBACK_2:
      {
        if ( VSM_ENABLED == session_object->active_set.playback.enable_flag )
        {
          rc = vsm_utility_playback_control_stop( session_object );
          if ( ADSP_EPENDING == rc )
          {
            return ADSP_EOK;
          }
          else if ( ADSP_FAILED( rc ) )
          {
            session_object->cmd_control.cmd_result = ADSP_EFAILED;
            session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
            continue;
          }
          else
          {
            session_object->cmd_control.current_action = VSM_ACTION_ENUM_STOP_SERVICES_3;
            continue;
          }
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_STOP_SERVICES_3;
          continue;
        }
      }
      break;

      case VSM_ACTION_ENUM_STOP_SERVICES_3:
      {
        rc = vsm_action_run_pause_services( session_object, ELITE_CMD_PAUSE );
        if ( ADSP_FAILED( rc ) )
        {
          session_object->cmd_control.cmd_result = ADSP_EFAILED;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
          continue;
        }
        else
        {
          session_object->state_control.state = VSM_STATE_ENUM_IDLE;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_WAIT_4;
          return ADSP_EOK;
        }
      }
      break;

      case VSM_ACTION_ENUM_WAIT_4:
      {
        if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
        {
          session_object->cmd_control.cmd_result = session_object->cmd_control.resp_result;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_STOP_PORTS_5;
        }
        continue;
      }
      break;

      case VSM_ACTION_ENUM_STOP_PORTS_5:
      {
        rc = vsm_action_run_stop_ports( session_object, ELITEMSG_CUSTOM_VMX_STOP );
        if( ADSP_FAILED( rc ) )
        {
          session_object->cmd_control.cmd_result = ADSP_EFAILED;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
          continue;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_WAIT_6; 
          return ADSP_EOK;
        }
      }
      break;

      case VSM_ACTION_ENUM_WAIT_6:
      {
        if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
        {
          session_object->cmd_control.cmd_result = session_object->cmd_control.resp_result;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
        }
        else
        {
          session_object->active_set.media_id = session_object->target_set.media_id;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_SEND_DISABLE_NOTIFICATIONS_TO_CLIENT_7;
        }
        continue;
      }
      break;

      case VSM_ACTION_ENUM_SEND_DISABLE_NOTIFICATIONS_TO_CLIENT_7:
      {
        ( void )vsm_action_send_disable_notifications_to_clients( session_object );
        session_object->cmd_control.current_action = VSM_ACTION_ENUM_COMPLETE;
        continue;
      }
      break;

      case VSM_ACTION_ENUM_ERROR_HANDLE:
      {
        if ( TRUE == session_object->cmd_control.is_resp_pending )
        {
          return ADSP_EOK;
        }
        else
        {
          //Do error handling
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
          continue;
        }
      }
      break;

      case VSM_ACTION_ENUM_REPORT_FAILURE:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"VSM_stream_disable_cmd_control: command FAILED:." );
        elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, session_object->cmd_control.cmd_result );
        session_object->cmd_control.is_cmd_pending = FALSE;
        return ADSP_EOK;
      }
      break;

      case VSM_ACTION_ENUM_COMPLETE:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,"VSM_stream_disable_cmd_control: command Success:." );
        elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EOK );
        session_object->cmd_control.is_cmd_pending = FALSE;
        return ADSP_EOK;
      }
      break;

      default:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"VSM_stream_disable_cmd_control: ERROR: Unexpected..." );
      }
      return ADSP_EOK;
    }
  }
  return rc;
}

static int32_t VSM_stream_enable_cmd_control( vsm_session_object_t* session_object )
{
  ADSPResult rc = ADSP_EOK;
  uint32_t payload_size;

  enum
  {
    VSM_ACTION_ENUM_UNINITIALIZED,
    VSM_ACTION_ENUM_RECONFIG_MAILBOX_TIMING_1,
    VSM_ACTION_ENUM_WAIT_2,
    VSM_ACTION_ENUM_SET_VOICE_TIMING_3,
    VSM_ACTION_ENUM_WAIT_4,
    VSM_ACTION_ENUM_SET_TX_RX_MUTE_5,
    VSM_ACTION_ENUM_WAIT_6,
    VSM_ACTION_ENUM_SET_UI_PROPERTIES_7,
    VSM_ACTION_ENUM_WAIT_8,
    VSM_ACTION_ENUM_UPDATE_TTY_MODE_9,
    VSM_ACTION_ENUM_WAIT_10,
    VSM_ACTION_ENUM_SEND_ENABLE_NOTIFICATIONS_TO_CLIENT_11,
    VSM_ACTION_ENUM_RUN_PORTS_12,
    VSM_ACTION_ENUM_WAIT_13,
    VSM_ACTION_ENUM_RUN_SERVICES_14,
    VSM_ACTION_ENUM_WAIT_15,
    VSM_ACTION_ENUM_START_RECORD_16,
    VSM_ACTION_ENUM_START_PLAYBACK_17,
    VSM_ACTION_ENUM_ERROR_HANDLE,
    VSM_ACTION_ENUM_REPORT_FAILURE,
    VSM_ACTION_ENUM_COMPLETE,
    VSM_ACTION_ENUM_INVALID
  };

  if( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  elite_apr_packet_t *apr_pkt = ( elite_apr_packet_t * )( session_object->cmd_control.cmd_msg.pPayload );

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "VSM_stream_enable_cmd_control():  session_portID = 0x%08X,"
         " current_action %d", apr_pkt->dst_port, session_object->cmd_control.current_action );

  session_object->cmd_control.is_cmd_pending = TRUE;

  while ( VSM_ACTION_ENUM_UNINITIALIZED == session_object->cmd_control.current_action )
  {
    if ( VSM_STATE_ENUM_RUN == session_object->state_control.state )
    {
      session_object->cmd_control.cmd_result = ADSP_EALREADY;
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
      break;
    }

    if ( FALSE == vsm_utility_media_id_is_valid( session_object->active_set.media_id ) )
    { /* Stream cannot run without valid media ID. */
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_stream_enable_cmd_control: Cannot enable stream, media id has not been set" );
      session_object->cmd_control.cmd_result = ADSP_ENOTREADY;
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
      break;
    }

    if ( TRUE == session_object->is_stream_config_changed )
    {
      /* If the stream configuration has changed since the last time when
         * VSS_ICOMMON_CMD_SET_SYSTEM_CONFIG has been processed, end the
         * current command and send VSS_ISTREAM_EVT_RECONFIG to MVM. Note that
         * the stream can only receive the enable command if it has been attached
         * to an MVM session. Therefore the session_obj->attached_mvm_handle is
         * guaranteed to be valid.
         */
      rc = vsm_action_send_reconfig_event( session_object );
      if ( rc )
      {
        session_object->cmd_control.cmd_result = rc;
        session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
      }
      else
      {
        session_object->cmd_control.current_action = VSM_ACTION_ENUM_COMPLETE;
      }
      break;
    }

    session_object->cmd_control.current_action = VSM_ACTION_ENUM_SET_VOICE_TIMING_3;
    break;
  }

  for ( ; ; )
  {
    switch ( session_object->cmd_control.current_action )
    {
      case VSM_ACTION_ENUM_RECONFIG_MAILBOX_TIMING_1:
      {
        rc = vsm_action_reconfig_mailbox_timing( session_object );
        if ( ADSP_FAILED( rc ) )
        {
          session_object->cmd_control.cmd_result = ADSP_EFAILED;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
          continue;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_WAIT_2;
          return ADSP_EOK;
        }
      }
      break;

      case VSM_ACTION_ENUM_WAIT_2:
      {
        if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
        {
          session_object->cmd_control.cmd_result = session_object->cmd_control.resp_result;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_SET_VOICE_TIMING_3;
        }
        continue;
      }
      break;

      case VSM_ACTION_ENUM_SET_VOICE_TIMING_3:
      {
        rc = vsm_action_set_voice_timing( session_object );
        if( ADSP_FAILED( rc ) )
        {
          session_object->cmd_control.cmd_result = ADSP_EFAILED;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
          continue;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_WAIT_4;
          return ADSP_EOK;
        }
      }
      break;

      case VSM_ACTION_ENUM_WAIT_4:
      {
        if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
        {
          session_object->cmd_control.cmd_result = session_object->cmd_control.resp_result;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
        }
        else
        {
          session_object->active_set.media_id = session_object->target_set.media_id;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_SET_TX_RX_MUTE_5;
        }
        continue;
      }
      break;

      case VSM_ACTION_ENUM_SET_TX_RX_MUTE_5:
      {
        rc = vsm_action_set_tx_rx_mute( session_object, VSM_DIRECTION_RX_TX, 0, 0 );
        if( ADSP_FAILED( rc ) )
        {
          session_object->cmd_control.cmd_result = ADSP_EFAILED;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
          continue;
        }
        else
        {
          if ( TRUE == session_object->cmd_control.is_resp_pending )
          {
            session_object->cmd_control.current_action = VSM_ACTION_ENUM_WAIT_6;
            return ADSP_EOK;
          }
          else
          {
            session_object->cmd_control.current_action = VSM_ACTION_ENUM_SET_UI_PROPERTIES_7;
            continue;
          }
        }
      }
      break;

      case VSM_ACTION_ENUM_WAIT_6:
      {
        if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
        {
          session_object->cmd_control.cmd_result = session_object->cmd_control.resp_result;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
        }
        else
        {
          session_object->active_set.mute.tx_mute_flag = session_object->target_set.mute.tx_mute_flag;
          session_object->active_set.mute.tx_ramp_duration = session_object->target_set.mute.tx_ramp_duration;
          session_object->active_set.mute.rx_mute_flag = session_object->target_set.mute.rx_mute_flag;
          session_object->active_set.mute.rx_ramp_duration = session_object->target_set.mute.rx_ramp_duration;

          if ( 0 != session_object->cached_ui_properties.data_len )
          {
            session_object->cmd_control.current_action = VSM_ACTION_ENUM_SET_UI_PROPERTIES_7;
          }
          else
          {
            session_object->cmd_control.current_action = VSM_ACTION_ENUM_UPDATE_TTY_MODE_9;
          }
        }
        continue;
      }
      break;

      case VSM_ACTION_ENUM_SET_UI_PROPERTIES_7:
      {
        rc = vsm_action_set_ui_properties( session_object );
        if ( ADSP_FAILED( rc ) )
        {
          session_object->cmd_control.cmd_result = ADSP_EFAILED;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
          continue;
        }
        else if ( TRUE != session_object->cmd_control.is_resp_pending )
        {
          /* No matching entries found/ No calibration required.*/
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_UPDATE_TTY_MODE_9;
          continue;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_WAIT_8;
          return ADSP_EOK;
        }
      }
      break;

      case VSM_ACTION_ENUM_WAIT_8:
      {
        if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
        {
          MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_stream_enable_cmd_control():"
                 " set_ui_properties error: %d.", session_object->cmd_control.resp_result );
        }
        session_object->cmd_control.current_action = VSM_ACTION_ENUM_UPDATE_TTY_MODE_9;
        continue;
      }
      break;

      case VSM_ACTION_ENUM_UPDATE_TTY_MODE_9:
      {
        rc = vsm_action_update_tty_mode( session_object );
        if( ADSP_FAILED( rc ) )
        {
          session_object->cmd_control.cmd_result = ADSP_EFAILED;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
        continue;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_WAIT_10;
          return ADSP_EOK;
        }
      }
      break;

      case VSM_ACTION_ENUM_WAIT_10:
      {
        if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
        {
          session_object->cmd_control.cmd_result = session_object->cmd_control.resp_result;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_SEND_ENABLE_NOTIFICATIONS_TO_CLIENT_11;
        }
        continue;
      }
      break;

      case VSM_ACTION_ENUM_SEND_ENABLE_NOTIFICATIONS_TO_CLIENT_11:
      {
        ( void )vsm_action_send_enable_notifications_to_clients( session_object );
        session_object->cmd_control.current_action = VSM_ACTION_ENUM_RUN_PORTS_12;
        continue;
      }
      break;

      case VSM_ACTION_ENUM_RUN_PORTS_12:
      {
        rc = vsm_action_run_stop_ports( session_object, ELITEMSG_CUSTOM_VMX_RUN );
        if( ADSP_FAILED( rc ) )
        {
          session_object->cmd_control.cmd_result = ADSP_EFAILED;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
          continue;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_WAIT_13; 
          return ADSP_EOK;
        }
      }
      break;

      case VSM_ACTION_ENUM_WAIT_13:
      {
        if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
        {
          session_object->cmd_control.cmd_result = session_object->cmd_control.resp_result;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
        }
        else
        {
          session_object->cmd_control.cmd_result = ADSP_EOK;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_RUN_SERVICES_14;
        }
        continue;
      }
      break;

      case VSM_ACTION_ENUM_RUN_SERVICES_14:
      {
        rc = vsm_action_run_pause_services( session_object, ELITE_CMD_RUN );
        if( ADSP_FAILED( rc ) )
        {
          session_object->cmd_control.cmd_result = ADSP_EFAILED;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
          continue;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_WAIT_15; 
          return ADSP_EOK;
        }
      }
      break;

      case VSM_ACTION_ENUM_WAIT_15:
      {
        if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
        {
          session_object->cmd_control.cmd_result = session_object->cmd_control.resp_result;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
        }
        else
        {
          session_object->cmd_control.cmd_result = ADSP_EOK;
          session_object->state_control.state = VSM_STATE_ENUM_RUN;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_START_RECORD_16;
        }
        continue;
      }
      break;

      case VSM_ACTION_ENUM_START_RECORD_16:
      {
        if ( VSM_ENABLED == session_object->active_set.record.enable_flag )
        {
          rc = vsm_utility_record_control_start( session_object );
          if ( ADSP_EPENDING == rc )
          {
            return ADSP_EOK;
          }
          else if ( ADSP_FAILED( rc ) )
          {
            session_object->cmd_control.cmd_result = ADSP_EFAILED;
            session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
            continue;
          }
          else
          {
            session_object->cmd_control.current_action = VSM_ACTION_ENUM_START_PLAYBACK_17;
            continue;
          }
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_START_PLAYBACK_17;
          continue;
        }
      }
      break;

      case VSM_ACTION_ENUM_START_PLAYBACK_17:
      {
        if ( VSM_ENABLED == session_object->active_set.playback.enable_flag )
        {
          rc = vsm_utility_playback_control_start( session_object );
          if ( ADSP_EPENDING == rc )
          {
            return ADSP_EOK;
          }
          else if ( ADSP_FAILED( rc ) )
          {
            session_object->cmd_control.cmd_result = ADSP_EFAILED;
            session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
            continue;
          }
          else
          {
            session_object->cmd_control.current_action = VSM_ACTION_ENUM_COMPLETE;
            continue;
          }
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_COMPLETE;
          continue;
        }
      }
      break;

      case VSM_ACTION_ENUM_ERROR_HANDLE:
      {
        if ( TRUE == session_object->cmd_control.is_resp_pending )
        {
          return ADSP_EOK;
        }
        else
        {
          //Do error handling
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
          continue;
        }
      }
      break;

      case VSM_ACTION_ENUM_REPORT_FAILURE:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"VSM_stream_enable_cmd_control: command FAILED:." );
        elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, session_object->cmd_control.cmd_result );
        session_object->cmd_control.is_cmd_pending = FALSE;
        return ADSP_EOK;
      }
      break;

      case VSM_ACTION_ENUM_COMPLETE:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,"VSM_stream_enable_cmd_control: command Success:." );
        elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EOK );
        session_object->cmd_control.is_cmd_pending = FALSE;
        return ADSP_EOK;
      }
      break;

      default:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"VSM_stream_enable_cmd_control: ERROR: Unexpected..." );
      }
      return ADSP_EOK;
    }
  }
  return rc;
}


static int32_t VSM_stream_encoder_reset_cmd_control ( 
  vsm_session_object_t* session_object
)
{
  int32_t rc = ADSP_EOK;
  elite_apr_packet_t *apr_pkt = ( elite_apr_packet_t * )( session_object->cmd_control.cmd_msg.pPayload );

  /* While loop to be processed only once. */
  while ( FALSE == session_object->cmd_control.is_cmd_pending )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "VSM: handling encoder reset for session_portID = 0x%08X", apr_pkt->dst_port );

    session_object->cmd_control.resp_count = 0;
    session_object->cmd_control.issued_count = 0;
    session_object->cmd_control.resp_result = APR_UNDEFINED_ID_V;

    if ( NULL == session_object->venc )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "VSM: handling encoder reset: failed to set_param on encoder!!" );
      session_object->cmd_control.resp_result = ADSP_EFAILED;
      break;
    }

    ( void ) vsm_action_set_encoder_reset( session_object );

    break;
  }

  if ( FALSE == session_object->cmd_control.is_resp_pending )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,
         "VSM: handling encoder reset: command Success:." );

    session_object->cmd_control.is_cmd_pending = FALSE;
    rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt,
                               session_object->cmd_control.resp_result );
  }

  return ADSP_EOK;
}


static int32_t VSM_stream_reinit_cmd_control( vsm_session_object_t* session_object )
{
  ADSPResult rc = ADSP_EOK;
  uint32_t payload_size;
  enum
  {
    VSM_ACTION_ENUM_UNINITIALIZED,
    VSM_ACTION_ENUM_REINIT_STREAM_1,
    VSM_ACTION_ENUM_WAIT_2,
    VSM_ACTION_ENUM_SET_MEDIA_TYPE_3,
    VSM_ACTION_ENUM_WAIT_4,
    VSM_ACTION_ENUM_SET_PP_SAMPLE_RATE_5,
    VSM_ACTION_ENUM_WAIT_6,
    VSM_ACTION_ENUM_RECONFIG_TX_MIXER_7,
    VSM_ACTION_ENUM_WAIT_8,
    VSM_ACTION_ENUM_SET_CACHED_PROPERTIES_9,
    VSM_ACTION_ENUM_WAIT_10,
    VSM_ACTION_ENUM_SET_CACHED_VOC_PARAMS_11,
    VSM_ACTION_ENUM_WAIT_12,
    VSM_ACTION_ENUM_REGISTER_VOC_OP_MODE_EVENT_13,
    VSM_ACTION_ENUM_WAIT_14,
    VSM_ACTION_ENUM_SET_PACKET_EXCHANGE_MODE_15,
    VSM_ACTION_ENUM_WAIT_16,
    VSM_ACTION_ENUM_SET_OOB_PACKET_EXCHANGE_CONFIG_17,
    VSM_ACTION_ENUM_WAIT_18,
    VSM_ACTION_ENUM_SET_RX_DTMF_DETECT_CONFIG_19,
    VSM_ACTION_ENUM_WAIT_20,
    VSM_ACTION_ENUM_ERROR_HANDLE,
    VSM_ACTION_ENUM_REPORT_FAILURE,
    VSM_ACTION_ENUM_COMPLETE,
    VSM_ACTION_ENUM_INVALID
  };

  if( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  elite_apr_packet_t *apr_pkt = ( elite_apr_packet_t * )( session_object->cmd_control.cmd_msg.pPayload );

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "VSM_stream_reinit_cmd_control():  session_portID = 0x%08X,"
         " current_action %d", apr_pkt->dst_port, session_object->cmd_control.current_action );

  session_object->cmd_control.is_cmd_pending = TRUE;

  while ( VSM_ACTION_ENUM_UNINITIALIZED == session_object->cmd_control.current_action )
  {
    if ( VSM_STATE_ENUM_IDLE != session_object->state_control.state )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_stream_reinit_cmd_control():"
             " Reinit allowed only in IDLE state. current state = 0x%X", session_object->state_control.state );
      session_object->cmd_control.cmd_result = ADSP_EFAILED;
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
    }
    else
    {
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_REINIT_STREAM_1;
    }
    break;
  }

  for ( ; ; )
  {
    switch ( session_object->cmd_control.current_action )
    {
      case VSM_ACTION_ENUM_REINIT_STREAM_1:
      {
        rc = vsm_action_reinit_stream( session_object );
        if ( ADSP_FAILED( rc ) )
        {
          session_object->cmd_control.cmd_result = ADSP_EFAILED;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
          continue;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_WAIT_2;
          return ADSP_EOK;
        }
      }
      break;

      case VSM_ACTION_ENUM_WAIT_2:
      {
        if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
        {
          session_object->cmd_control.cmd_result = session_object->cmd_control.resp_result;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
        }
        else
        {
          /* Assuming the reinit is called only for new call*/
          session_object->packet_logging_info.voice_call_num++;
          session_object->packet_logging_info.rx_packet_seq_num = 0;
          session_object->packet_logging_info.tx_packet_seq_num = 0;

          session_object->cmd_control.current_action = VSM_ACTION_ENUM_SET_MEDIA_TYPE_3;
        }
        continue;
      }
      break;

      case VSM_ACTION_ENUM_SET_MEDIA_TYPE_3:
      {
        rc = vsm_action_set_media_type( session_object );
        if( ADSP_FAILED( rc ) )
        {
          session_object->cmd_control.cmd_result = ADSP_EFAILED;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
          continue;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_WAIT_4;
          return ADSP_EOK;
        }
      }
      break;

      case VSM_ACTION_ENUM_WAIT_4:
      {
        if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
        {
          session_object->cmd_control.cmd_result = session_object->cmd_control.resp_result;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
        }
        else
        {
          if ( vsm_utility_vocoder_is_variable( session_object->target_set.media_id ) )
          {
            session_object->cmd_control.current_action = VSM_ACTION_ENUM_SET_PP_SAMPLE_RATE_5;
          }
          else
          {
            session_object->cmd_control.current_action = VSM_ACTION_ENUM_RECONFIG_TX_MIXER_7;
          }
          session_object->active_set.media_id = session_object->target_set.media_id;
          session_object->target_set.system_config.media_id = session_object->active_set.media_id;
        }
        continue;
      }
      break;

      case VSM_ACTION_ENUM_SET_PP_SAMPLE_RATE_5:
      {
        rc = vsm_action_set_pp_sampling_rate( session_object );
        if ( ADSP_FAILED( rc ) )
        {
          session_object->cmd_control.cmd_result = ADSP_EFAILED;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
          continue;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_WAIT_6;
          return ADSP_EOK;
        }
      }
      break;

      case VSM_ACTION_ENUM_WAIT_6:
      {
        if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
        {
          session_object->cmd_control.cmd_result = session_object->cmd_control.resp_result;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_RECONFIG_TX_MIXER_7;
        }
        continue;
      }
      break;

      case VSM_ACTION_ENUM_RECONFIG_TX_MIXER_7:
      {
        rc = vsm_action_reconfig_tx_mixer( session_object );
        if ( ADSP_FAILED( rc ) )
        {
          session_object->cmd_control.cmd_result = ADSP_EFAILED;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
          continue;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_WAIT_8;
          return ADSP_EOK;
        }
      }
      break;

      case VSM_ACTION_ENUM_WAIT_8:
      {
        if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
        {
          session_object->cmd_control.cmd_result = session_object->cmd_control.resp_result;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_SET_CACHED_PROPERTIES_9;
        }
        continue;
      }
      break;

      case VSM_ACTION_ENUM_SET_CACHED_PROPERTIES_9:
      {
        rc = vsm_action_set_cached_properties( session_object );
        if( ADSP_FAILED( rc ) )
        {
          session_object->cmd_control.cmd_result = ADSP_EFAILED;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
          continue;
        }
        else
        {
          if ( TRUE == session_object->cmd_control.is_resp_pending )
          {
            session_object->cmd_control.current_action = VSM_ACTION_ENUM_WAIT_10;
            return ADSP_EOK;
          }
          else
          {
            session_object->cmd_control.current_action = VSM_ACTION_ENUM_SET_CACHED_VOC_PARAMS_11;
            continue;
          }
        }
      }
      break;

      case VSM_ACTION_ENUM_WAIT_10:
      {
        if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
        {
          session_object->cmd_control.cmd_result = session_object->cmd_control.resp_result;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_SET_CACHED_VOC_PARAMS_11;
        }
        continue;
      }
      break;

      case VSM_ACTION_ENUM_SET_CACHED_VOC_PARAMS_11:
      {
        rc = vsm_action_set_cached_voc_params( session_object );
        if( ADSP_FAILED( rc ) )
        {
          session_object->cmd_control.cmd_result = ADSP_EFAILED;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
          continue;
        }
        else
        {
          if ( TRUE == session_object->cmd_control.is_resp_pending )
          {
            session_object->cmd_control.current_action = VSM_ACTION_ENUM_WAIT_12;
            return ADSP_EOK;
          }
          else
          {
            session_object->cmd_control.current_action = VSM_ACTION_ENUM_REGISTER_VOC_OP_MODE_EVENT_13;
            continue;
          }
        }
      }
      break;

      case VSM_ACTION_ENUM_WAIT_12:
      {
        if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
        {
          session_object->cmd_control.cmd_result = session_object->cmd_control.resp_result;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_REGISTER_VOC_OP_MODE_EVENT_13;
        }
        continue;
      }
      break;

      case VSM_ACTION_ENUM_REGISTER_VOC_OP_MODE_EVENT_13:
      {
        rc = vsm_action_register_event( session_object, VSM_EVT_VOC_OPERATING_MODE_UPDATE );
        if( ADSP_FAILED( rc ) )
        {
          session_object->cmd_control.cmd_result = ADSP_EFAILED;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
          continue;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_WAIT_14;
          return ADSP_EOK;
        }
      }
      break;

      case VSM_ACTION_ENUM_WAIT_14:
      {
        if ( FALSE == vsm_utility_is_voc_op_mode_event_received( session_object ) )
        {
          return ADSP_EOK;
        }
        else
        {
          if ( VSS_IPKTEXG_MODE_IN_BAND == session_object->packet_exchange_info.mode )
          {
            session_object->cmd_control.cmd_result = ADSP_EOK;
            session_object->cmd_control.current_action = VSM_ACTION_ENUM_COMPLETE;
          }
          else
          {
            session_object->cmd_control.current_action = VSM_ACTION_ENUM_SET_PACKET_EXCHANGE_MODE_15;
          }
        }
        continue;
      }
      break;

      case VSM_ACTION_ENUM_SET_PACKET_EXCHANGE_MODE_15:
      {
        rc = vsm_action_set_packet_exchange_mode( session_object, session_object->packet_exchange_info.mode );
        if( ADSP_FAILED( rc ) )
        {
          session_object->cmd_control.cmd_result = ADSP_EFAILED;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
          continue;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_WAIT_16;
          return ADSP_EOK;
        }
      }
      break;

      case VSM_ACTION_ENUM_WAIT_16:
      {
        if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
        {
          session_object->cmd_control.cmd_result = session_object->cmd_control.resp_result;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_SET_RX_DTMF_DETECT_CONFIG_19;
        }
        continue;
      }
      break;

      case VSM_ACTION_ENUM_SET_OOB_PACKET_EXCHANGE_CONFIG_17:
      {
        rc = vsm_action_set_oob_pkt_exchange_config( session_object );
        if( ADSP_FAILED( rc ) )
        {
          session_object->cmd_control.cmd_result = ADSP_EFAILED;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
          continue;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_WAIT_18; 
          return ADSP_EOK;
        }
      }
      break;

      case VSM_ACTION_ENUM_WAIT_18:
      {
        if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
        {
          session_object->cmd_control.cmd_result = session_object->cmd_control.resp_result;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_SET_RX_DTMF_DETECT_CONFIG_19;
        }
        continue;
      }
      break;

      case VSM_ACTION_ENUM_SET_RX_DTMF_DETECT_CONFIG_19:
      {
        rc = vsm_action_set_rx_dtmf_detect_config( session_object );
        if ( ADSP_FAILED( rc ) )
        {
          session_object->cmd_control.cmd_result = ADSP_EFAILED;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
          continue;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_WAIT_20;
          return ADSP_EOK;
        }
      }
      break;

      case VSM_ACTION_ENUM_WAIT_20:
      {
        if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
        {
          session_object->cmd_control.cmd_result = session_object->cmd_control.resp_result;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
        }
        else
        {
          session_object->cmd_control.cmd_result = ADSP_EOK;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_COMPLETE;
        }
        continue;
      }
      break;

      case VSM_ACTION_ENUM_ERROR_HANDLE:
      {
        if ( TRUE == session_object->cmd_control.is_resp_pending )
        {
          return ADSP_EOK;
        }
        else
        {
          //Do error handling
//        ( void )vsm_action_cleanup_create_errors( session_object );
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
          if ( TRUE == session_object->cmd_control.is_resp_pending )
          {
            return ADSP_EOK;
          }
          continue;
        }
      }
      break;

      case VSM_ACTION_ENUM_REPORT_FAILURE:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"VSM_stream_reinit_cmd_control: command FAILED:." );
        elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, session_object->cmd_control.cmd_result );
        session_object->cmd_control.is_cmd_pending = FALSE;
        return ADSP_EOK;
      }
      break;

      case VSM_ACTION_ENUM_COMPLETE:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,"VSM_stream_reinit_cmd_control: command Success:." );
        elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EOK );
        session_object->cmd_control.is_cmd_pending = FALSE;
        return ADSP_EOK;
      }
      break;

      default:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"VSM_stream_reinit_cmd_control: ERROR: Unexpected..." );
      }
      return ADSP_EOK;
    }
  }
  return rc;
}

static int32_t VSM_set_vocproc_avsync_delays_cmd_control ( 
  vsm_session_object_t* session_object
)
{
  int32_t rc = ADSP_EOK;
  uint32_t payload_size;
  vss_istream_cmd_set_vocproc_avsync_delays_t *avsync_vp_delay;

  if( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  elite_apr_packet_t *apr_pkt = ( elite_apr_packet_t * )( session_object->cmd_control.cmd_msg.pPayload );

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
         "VSM: SET_AVSYNC_DELAYS: session_portID = 0x%08X", apr_pkt->dst_port );

  /* Validate input params. */
  payload_size = APRV2_PKT_GET_PAYLOAD_BYTE_SIZE( apr_pkt->header );
  if( payload_size != sizeof( vss_istream_cmd_set_vocproc_avsync_delays_t ) )
  {
    session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
    rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EBADPARAM );
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "VSM: SET_AVSYNC_DELAYS: invalid payload size. %d", payload_size );
    return rc;
  }

  session_object->avsync_delay_info.total_rx_delay = 0;
  session_object->avsync_delay_info.total_tx_delay = 0;

  avsync_vp_delay = APRV2_PKT_GET_PAYLOAD( vss_istream_cmd_set_vocproc_avsync_delays_t, apr_pkt );

  /* Add the normalized< processing + algorithmic > vocproc delay. */
  session_object->avsync_delay_info.total_rx_delay =
    avsync_vp_delay->vp_rx_normalized_total_delay;
  session_object->avsync_delay_info.total_tx_delay =
    avsync_vp_delay->vp_tx_normalized_total_delay;

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
         "VSM: SET_AVSYNC_DELAYS: tx_delay_us = %d us, rx_delay_us = %d us ",
         avsync_vp_delay->vp_tx_normalized_total_delay,
         avsync_vp_delay->vp_rx_normalized_total_delay );

  /* The encoder/decoder processing delays were already reverse calculated
   * during mvm_calculate_avsync_delays() with in MVM. Thus adding of algorithmic
   * delays is only left.*/
  session_object->avsync_delay_info.total_rx_delay +=
    session_object->avsync_delay_info.stream_rx_algorithmic_delay ;
  session_object->avsync_delay_info.total_tx_delay +=
    session_object->avsync_delay_info.stream_tx_algorithmic_delay ;

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
         "VSM: SET_AVSYNC_DELAYS: total_voice_tx_path_delay = %d Us, total_voice_rx_path_delay = %d us",
         session_object->avsync_delay_info.total_tx_delay,
         session_object->avsync_delay_info.total_rx_delay );

  if ( VSM_STATE_ENUM_RUN == session_object->state_control.state )
  {
    ( void ) vsm_action_send_avsync_rx_delay_event( session_object );
  }

  rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EOK );

  return rc;
}


static int32_t VSM_get_ui_property_cmd_control(
 vsm_session_object_t* session_object
)
{
  int32_t rc = ADSP_EOK;
  vss_icommon_cmd_get_ui_property_t* payload;
  uint32_t payload_size;
  bool_t ui_prop_exists = FALSE;
  uint8_t* cur_ui_prop_cache_slot;
  uint32_t remaining_ui_prop_cache_len;
  voice_param_data_t* cached_ui_prop;
  uint32_t get_ui_status;
  aprv2_packet_t* rsp_packet;
  uint8_t* rsp_payload;
  uint32_t rsp_payload_size;
  uint32_t rsp_payload_left;
  uint16_t client_addr;
  int32_t *get_param_ptr;
  voice_param_data_t* param_data_ptr;

  enum
  {
    VSM_ACTION_ENUM_UNINITIALIZED,
    VSM_ACTION_ENUM_GET_PARAM,
    VSM_ACTION_ENUM_WAIT_1,
    VSM_ACTION_ENUM_ERROR_HANDLE,
    VSM_ACTION_ENUM_REPORT_FAILURE,
    VSM_ACTION_ENUM_COMPLETE,
    VSM_ACTION_ENUM_INVALID
  };

  if( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  elite_apr_packet_t *apr_pkt = ( elite_apr_packet_t * )( session_object->cmd_control.cmd_msg.pPayload );
  client_addr = apr_pkt->src_addr;

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED, "VSM_get_ui_property_cmd_control():"
         " session_portID = 0x%08X", apr_pkt->dst_port );

  session_object->cmd_control.is_cmd_pending = TRUE;
  while ( VSM_ACTION_ENUM_UNINITIALIZED == session_object->cmd_control.current_action )
  {
    payload = APRV2_PKT_GET_PAYLOAD( vss_icommon_cmd_get_ui_property_t, apr_pkt );
    payload_size = APRV2_PKT_GET_PAYLOAD_BYTE_SIZE( apr_pkt->header );

    if ( payload_size != sizeof( vss_icommon_cmd_get_ui_property_t ) )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_get_ui_property_cmd_control():"
             " Unexpected payload size, %d != %d", payload_size,
             sizeof( vss_icommon_cmd_get_ui_property_t ) );
      session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
      break;
    }

    if ( FALSE == vsm_utility_verify_param_data_size_aligned( payload->param_size ) )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_get_ui_property_cmd_control():"
             " Invalid param data size, %d", payload->param_size );
      session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
      break;
    }
    rc = vsm_utility_validate_and_update_ui_prop_version
       ( &session_object->cached_ui_properties.version,
         CVD_CAL_PARAM_MINOR_VERSION_0 );
    if ( rc == APR_EOK ) 
    {
       remaining_ui_prop_cache_len = session_object->cached_ui_properties.data_len;
       cur_ui_prop_cache_slot = session_object->cached_ui_properties.data;

       while ( remaining_ui_prop_cache_len != 0 )
       {
         cached_ui_prop = ( ( voice_param_data_t* ) cur_ui_prop_cache_slot );

         if ( ( cached_ui_prop->module_id == payload->module_id ) &&
              ( cached_ui_prop->param_id == payload->param_id ) )
         {
           ui_prop_exists = TRUE;
           get_ui_status = 0;
           break;
         }

         cur_ui_prop_cache_slot += ( cached_ui_prop->param_size + sizeof( voice_param_data_t ) );
         remaining_ui_prop_cache_len -= ( cached_ui_prop->param_size + sizeof( voice_param_data_t ) );
       }
    }

    if ( ui_prop_exists == TRUE )
    {
      rsp_payload_size = ( sizeof( vss_icommon_rsp_get_ui_property_t ) +
                           cached_ui_prop->param_size );

      rc = elite_apr_if_alloc_cmd_rsp(
               vsm_apr_handle, session_object->apr_info.self_addr,
               session_object->apr_info.self_port,
               apr_pkt->src_addr, apr_pkt->src_port,
               apr_pkt->token, VSS_ICOMMON_RSP_GET_UI_PROPERTY,
               rsp_payload_size, &rsp_packet );
      if ( ADSP_FAILED( rc ) )
      {
        session_object->cmd_control.cmd_result = ADSP_EFAILED;
        elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, session_object->cmd_control.cmd_result );
        return ADSP_EOK;
      }

      rsp_payload = APRV2_PKT_GET_PAYLOAD( uint8_t, rsp_packet );
      rsp_payload_left = APRV2_PKT_GET_PAYLOAD_BYTE_SIZE( rsp_packet->header );

      ( void ) mmstd_memcpy( rsp_payload, rsp_payload_left, &get_ui_status, sizeof( get_ui_status ) );
      rsp_payload_left -= sizeof( get_ui_status );
      ( void ) mmstd_memcpy( ( rsp_payload + sizeof( get_ui_status ) ), rsp_payload_left,
                             cached_ui_prop, ( cached_ui_prop->param_size + sizeof( voice_param_data_t ) ) );

      rc = elite_apr_if_async_send( vsm_apr_handle, rsp_packet );
      VSM_COMM_ERROR( rc, client_addr );

      rc = elite_apr_if_free( vsm_apr_handle, apr_pkt );
      VSM_COMM_ERROR( rc, client_addr );

      session_object->cmd_control.is_cmd_pending = FALSE;
      return ADSP_EOK;

    }
    else
    {
      /* If the UI property is not cached, get it from dynamic service. */
      /* Allocate Memory for in-band operation. */
      get_param_ptr = ( int32_t* )qurt_elite_memory_malloc(
                                     payload->param_size + sizeof( voice_param_data_t ), QURT_ELITE_HEAP_DEFAULT );
      if ( NULL == get_param_ptr )
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VSM_get_ui_property_cmd_control: failed to allocate memory for get_param payload..." );
        session_object->cmd_control.cmd_result = ADSP_ENORESOURCE;
        session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
        break;
      }

      param_data_ptr = ( voice_param_data_t* )get_param_ptr;
      param_data_ptr->module_id = payload->module_id;
      param_data_ptr->param_id = payload->param_id;
      param_data_ptr->param_size = ( ( uint16_t ) ( payload->param_size + sizeof( voice_param_data_t ) ) );
      param_data_ptr->reserved = 0;

      session_object->cmd_control.current_action = VSM_ACTION_ENUM_GET_PARAM;
      break;

      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, " VSM_get_ui_property_cmd_control():"
             " module ID= 0x%08X Param ID=0x%08X", payload->module_id, payload->param_id );
    }

  }

  for ( ; ; )
  {
    switch ( session_object->cmd_control.current_action )
    {
      case VSM_ACTION_ENUM_GET_PARAM:
      {
        rc = vsm_action_get_param( session_object, get_param_ptr, TRUE, CVD_CAL_PARAM_MINOR_VERSION_0 );
        if ( ADSP_FAILED( rc ) )
        {
          session_object->cmd_control.cmd_result = ADSP_EFAILED;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
          continue;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_WAIT_1;
          return ADSP_EOK;
        }
      }
      break;

      case VSM_ACTION_ENUM_WAIT_1:
      {
        if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
        {
          session_object->cmd_control.cmd_result = session_object->cmd_control.resp_result;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_COMPLETE;
        }
        continue;
      }
      break;

      case VSM_ACTION_ENUM_ERROR_HANDLE:
      {
        if ( TRUE == session_object->cmd_control.is_resp_pending )
        {
          return ADSP_EOK;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
          continue;
        }
      }
      break;

      case VSM_ACTION_ENUM_REPORT_FAILURE:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"VSM_get_ui_property_cmd_control: command FAILED:." );
        elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, session_object->cmd_control.cmd_result );
        session_object->cmd_control.is_cmd_pending = FALSE;
        return ADSP_EOK;
      }
      break;

      case VSM_ACTION_ENUM_COMPLETE:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,"VSM_get_ui_property_cmd_control: command Success:." );
        elite_apr_if_free( vsm_apr_handle, apr_pkt );
        session_object->cmd_control.is_cmd_pending = FALSE;
        return ADSP_EOK;
      }
      break;

      default:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"VSM_get_ui_property_cmd_control: ERROR: Unexpected..." );
      }
      return ADSP_EOK;
    }
  }

  return ADSP_EOK;
}


static int32_t VSM_get_ui_property_v2_cmd_control(
 vsm_session_object_t* session_object
)
{
  int32_t rc = ADSP_EOK;
  vss_icommon_cmd_get_ui_property_v2_t* payload;
  uint32_t payload_size;
  bool_t ui_prop_exists = FALSE;
  uint8_t* cur_ui_prop_cache_slot;
  uint32_t remaining_ui_prop_cache_len;
  voice_param_data_v2_t* cached_ui_prop;
  uint32_t get_ui_status;
  aprv2_packet_t* rsp_packet = NULL;
  uint8_t* rsp_payload;
  uint32_t rsp_payload_size;
  uint32_t rsp_payload_left;
  uint16_t client_addr;
  int32_t *get_param_ptr = NULL;
  voice_param_data_v2_t* param_data_ptr;

  enum
  {
    VSM_ACTION_ENUM_UNINITIALIZED,
    VSM_ACTION_ENUM_GET_PARAM,
    VSM_ACTION_ENUM_WAIT_1,
    VSM_ACTION_ENUM_ERROR_HANDLE,
    VSM_ACTION_ENUM_REPORT_FAILURE,
    VSM_ACTION_ENUM_COMPLETE,
    VSM_ACTION_ENUM_INVALID
  };

  if( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  elite_apr_packet_t *apr_pkt = ( elite_apr_packet_t * )( session_object->cmd_control.cmd_msg.pPayload );
  client_addr = apr_pkt->src_addr;

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED, "VSM_get_ui_property_v2_cmd_control():"
         " session_portID = 0x%08X", apr_pkt->dst_port );

  session_object->cmd_control.is_cmd_pending = TRUE;
  while ( VSM_ACTION_ENUM_UNINITIALIZED == session_object->cmd_control.current_action )
  {
    payload = APRV2_PKT_GET_PAYLOAD( vss_icommon_cmd_get_ui_property_v2_t, apr_pkt );
    payload_size = APRV2_PKT_GET_PAYLOAD_BYTE_SIZE( apr_pkt->header );

    if ( payload_size != sizeof( vss_icommon_cmd_get_ui_property_v2_t ) )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_get_ui_property_v2_cmd_control():"
             " Unexpected payload size, %d != %d", payload_size,
             sizeof( vss_icommon_cmd_get_ui_property_v2_t ) );
      session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
      break;
    }

    if ( FALSE == vsm_utility_verify_param_data_size_aligned( payload->param_size ) )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_get_ui_property_v2_cmd_control():"
             " Invalid param data size, %d", payload->param_size );
      session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
      break;
    }

    rc = vsm_utility_validate_and_update_ui_prop_version
       ( &session_object->cached_ui_properties.version,
       CVD_CAL_PARAM_MINOR_VERSION_1 );
    if ( rc == APR_EOK ) 
    {
       remaining_ui_prop_cache_len = session_object->cached_ui_properties.data_len;
       cur_ui_prop_cache_slot = session_object->cached_ui_properties.data;

       while ( remaining_ui_prop_cache_len != 0 )
       {
         cached_ui_prop = ( ( voice_param_data_v2_t* ) cur_ui_prop_cache_slot );

         if ( ( cached_ui_prop->module_id == payload->module_id ) &&
              ( cached_ui_prop->param_id == payload->param_id ) &&
              ( cached_ui_prop->instance_id == payload->instance_id ) )
         {
           ui_prop_exists = TRUE;
           get_ui_status = 0;
           break;
         }

         cur_ui_prop_cache_slot += ( cached_ui_prop->param_size + sizeof( voice_param_data_v2_t ) );
         remaining_ui_prop_cache_len -= ( cached_ui_prop->param_size + sizeof( voice_param_data_v2_t ) );
       }
    }

    if ( ui_prop_exists == TRUE )
    {
      rsp_payload_size = ( sizeof( vss_icommon_rsp_get_ui_property_v2_t ) +
                           cached_ui_prop->param_size );
      MSG_3( MSG_SSID_DFLT, MSG_LEGACY_MED, " VSM_get_ui_property_v2_cmd_control():"
             " module ID= 0x%08X Param ID=0x%08X instance ID=0x%08X", payload->module_id, payload->param_id, payload->instance_id );
      rc = elite_apr_if_alloc_cmd_rsp(
               vsm_apr_handle, session_object->apr_info.self_addr,
               session_object->apr_info.self_port,
               apr_pkt->src_addr, apr_pkt->src_port,
               apr_pkt->token, VSS_ICOMMON_RSP_GET_UI_PROPERTY_V2,
               rsp_payload_size, &rsp_packet );
      if ( ADSP_FAILED( rc ) )
      {
        session_object->cmd_control.cmd_result = ADSP_EFAILED;
        elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, session_object->cmd_control.cmd_result );
        return ADSP_EOK;
      }

      rsp_payload = APRV2_PKT_GET_PAYLOAD( uint8_t, rsp_packet );
      rsp_payload_left = APRV2_PKT_GET_PAYLOAD_BYTE_SIZE( rsp_packet->header );

      ( void ) mmstd_memcpy( rsp_payload, rsp_payload_left, &get_ui_status, sizeof( get_ui_status ) );
      rsp_payload_left -= sizeof( get_ui_status );
      ( void ) mmstd_memcpy( ( rsp_payload + sizeof( get_ui_status ) ), rsp_payload_left,
                             cached_ui_prop, ( cached_ui_prop->param_size + sizeof( voice_param_data_v2_t ) ) );

      rc = elite_apr_if_async_send( vsm_apr_handle, rsp_packet );
      VSM_COMM_ERROR( rc, client_addr );

      rc = elite_apr_if_free( vsm_apr_handle, apr_pkt );
      VSM_COMM_ERROR( rc, client_addr );

      session_object->cmd_control.is_cmd_pending = FALSE;
      return ADSP_EOK;

    }
    else
    {
      /* If the UI property is not cached, get it from dynamic service. */
      /* Allocate Memory for in-band operation. */
      get_param_ptr = ( int32_t* )qurt_elite_memory_malloc(
                                     payload->param_size + sizeof( voice_param_data_v2_t ), QURT_ELITE_HEAP_DEFAULT );
      if ( NULL == get_param_ptr )
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VSM_get_ui_property_v2_cmd_control: failed to allocate memory for get_param payload..." );
        session_object->cmd_control.cmd_result = ADSP_ENORESOURCE;
        session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
        break;
      }
      
      param_data_ptr = ( voice_param_data_v2_t* )get_param_ptr;
      param_data_ptr->module_id = payload->module_id;
      param_data_ptr->param_id = payload->param_id;
      param_data_ptr->instance_id = payload->instance_id;
      param_data_ptr->param_size = ( ( uint16_t ) ( payload->param_size + sizeof( voice_param_data_v2_t ) ) );
      param_data_ptr->reserved = 0;

      session_object->cmd_control.current_action = VSM_ACTION_ENUM_GET_PARAM;
      MSG_3( MSG_SSID_DFLT, MSG_LEGACY_MED, " VSM_get_ui_property_v2_cmd_control():"
             " module ID= 0x%08X Param ID=0x%08X instance ID=0x%08X", payload->module_id, payload->param_id, payload->instance_id );
      break;
    }
  }

  for ( ; ; )
  {
    switch ( session_object->cmd_control.current_action )
    {
      case VSM_ACTION_ENUM_GET_PARAM:
      {
        rc = vsm_action_get_param( session_object, get_param_ptr, TRUE, CVD_CAL_PARAM_MINOR_VERSION_1 );
        if ( ADSP_FAILED( rc ) )
        {
          session_object->cmd_control.cmd_result = ADSP_EFAILED;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
          continue;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_WAIT_1;
          return ADSP_EOK;
        }
      }
      break;

      case VSM_ACTION_ENUM_WAIT_1:
      {
        if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
        {
          session_object->cmd_control.cmd_result = session_object->cmd_control.resp_result;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_COMPLETE;
        }
        continue;
      }
      break;

      case VSM_ACTION_ENUM_ERROR_HANDLE:
      {
        if ( TRUE == session_object->cmd_control.is_resp_pending )
        {
          return ADSP_EOK;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
          continue;
        }
      }
      break;

      case VSM_ACTION_ENUM_REPORT_FAILURE:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"VSM_get_ui_property_v2_cmd_control: command FAILED:." );
        elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, session_object->cmd_control.cmd_result );
        session_object->cmd_control.is_cmd_pending = FALSE;
        return ADSP_EOK;
      }
      break;

      case VSM_ACTION_ENUM_COMPLETE:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,"VSM_get_ui_property_v2_cmd_control: command Success:." );
        elite_apr_if_free( vsm_apr_handle, apr_pkt );
        session_object->cmd_control.is_cmd_pending = FALSE;
        return ADSP_EOK;
      }
      break;

      default:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"VSM_get_ui_property_v2_cmd_control: ERROR: Unexpected..." );
      }
      return ADSP_EOK;
    }
  }

  return ADSP_EOK;
}

static int32_t VSM_set_ui_property_cmd_control(
 vsm_session_object_t* session_object
)
{
  int32_t rc = ADSP_EOK;
  vss_icommon_cmd_set_ui_property_t* payload;
  uint32_t payload_size;
  bool_t is_cached = FALSE;
  uint8_t* cur_ui_prop_cache_slot;
  uint32_t remaining_ui_prop_cache_len;
  uint8_t* destination_ui_prop_cache_slot = NULL;
  uint8_t* remaining_ui_prop;
  uint32_t remaining_ui_prop_len;
  voice_param_data_t* cached_ui_prop;
  uint32_t cached_ui_prop_len;
  uint16_t client_addr;
  enum
  {
    VSM_ACTION_ENUM_UNINITIALIZED,
    VSM_ACTION_ENUM_SET_PARAM,
    VSM_ACTION_ENUM_WAIT_1,
    VSM_ACTION_ENUM_ERROR_HANDLE,
    VSM_ACTION_ENUM_REPORT_FAILURE,
    VSM_ACTION_ENUM_COMPLETE,
    VSM_ACTION_ENUM_INVALID
  };

  if( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  elite_apr_packet_t *apr_pkt = ( elite_apr_packet_t * )( session_object->cmd_control.cmd_msg.pPayload );

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED, "VSM_set_ui_property_cmd_control():"
         " session_portID = 0x%08X", apr_pkt->dst_port );

  session_object->cmd_control.is_cmd_pending = TRUE;
  payload = APRV2_PKT_GET_PAYLOAD( vss_icommon_cmd_set_ui_property_t, apr_pkt );
  payload_size = APRV2_PKT_GET_PAYLOAD_BYTE_SIZE( apr_pkt->header );

  while ( VSM_ACTION_ENUM_UNINITIALIZED == session_object->cmd_control.current_action )
  {

    if ( payload_size < sizeof( vss_icommon_cmd_set_ui_property_t ) )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_set_ui_property_cmd_control(): Unexpected payload size, %d < %d",
                                              payload_size,
                                              sizeof( vss_icommon_cmd_set_ui_property_t ) );
      session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
      break;
    }

    if ( FALSE == vsm_utility_verify_param_data_size_aligned( payload->param_size ) )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_set_ui_property_cmd_control():"
             " Invalid param data size, %d", payload->param_size );
      session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
      break;
    }

    if ( payload->param_size > VSM_MAX_UI_PROP_DATA_LEN )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_set_ui_property_cmd_control():"
             " Invalid param data length %d > Max length %d", payload->param_size,
             VSM_MAX_UI_PROP_DATA_LEN );
      session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
      break;
    }

    if ( payload_size != ( payload->param_size + sizeof( voice_param_data_t ) ) )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_set_ui_property_cmd_control():"
             " Invalid payload size %d != %d", payload_size,
             ( payload->param_size + sizeof( voice_param_data_t ) ) );
      session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
      break;
    }

    rc = vsm_utility_validate_and_update_ui_prop_version
       ( &session_object->cached_ui_properties.version,
       CVD_CAL_PARAM_MINOR_VERSION_0 );
    if ( rc ) 
    {
       destination_ui_prop_cache_slot = ( uint8_t* )payload;
       session_object->cmd_control.current_action = VSM_ACTION_ENUM_SET_PARAM;
       session_object->cached_ui_properties.cache_fail_count++;
       MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_set_ui_property_cmd_control():"
             " Invalid version. fail_count %d", session_object->cached_ui_properties.cache_fail_count );
       break;
    }
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "VSM_set_ui_property_cmd_control():"
           " Module Id 0x%08X / Param Id 0x%08X received ",
           payload->module_id, payload->param_id );

    cur_ui_prop_cache_slot = session_object->cached_ui_properties.data;
    remaining_ui_prop_cache_len = sizeof( session_object->cached_ui_properties.data );
    remaining_ui_prop_len = session_object->cached_ui_properties.data_len;

    /* Cache UI properties. */
    while ( remaining_ui_prop_cache_len > 0 )
    {
      cached_ui_prop = ( ( voice_param_data_t* ) cur_ui_prop_cache_slot );
      cached_ui_prop_len = ( cached_ui_prop->param_size + sizeof( voice_param_data_t ) );

      if ( ( cached_ui_prop->module_id == payload->module_id ) &&
           ( cached_ui_prop->param_id == payload->param_id ) )
      {
        /* Value already exists. Cache it. */
        if ( cached_ui_prop_len != payload_size )
        { /* New UI property data size is different than the cached UI property data. */
          /* Shuffle the UI property cache to accommodate the new size. */
          remaining_ui_prop_len -= cached_ui_prop_len;
          remaining_ui_prop = ( cur_ui_prop_cache_slot + cached_ui_prop_len );

          while ( remaining_ui_prop_len >= cached_ui_prop_len )
          {
            ( void ) mmstd_memcpy( cur_ui_prop_cache_slot, remaining_ui_prop_cache_len, remaining_ui_prop, cached_ui_prop_len );
            remaining_ui_prop_len -= cached_ui_prop_len;
            remaining_ui_prop += cached_ui_prop_len;
            cur_ui_prop_cache_slot += cached_ui_prop_len;
            remaining_ui_prop_cache_len -= cached_ui_prop_len;
          }

          if ( remaining_ui_prop_len != 0 )
          {
            ( void ) mmstd_memcpy( cur_ui_prop_cache_slot, remaining_ui_prop_cache_len,
                                   remaining_ui_prop, remaining_ui_prop_len );
            cur_ui_prop_cache_slot += remaining_ui_prop_len;
            remaining_ui_prop_cache_len -= remaining_ui_prop_len;
          }

          ( void ) mmstd_memcpy( cur_ui_prop_cache_slot, remaining_ui_prop_cache_len, payload, payload_size );

          if ( cached_ui_prop_len < payload_size )
          { /* Increase the cached data length due to increased UI prop data. */
            session_object->cached_ui_properties.data_len += ( payload_size - cached_ui_prop_len );
          }
          else
          { /* Decrease the cached data length due to decreased UI prop data. */
            session_object->cached_ui_properties.data_len -= ( cached_ui_prop_len - payload_size );

            /* Set the decreased part of the cache that used to contain data to zero. */
            ( void ) mmstd_memset( ( cur_ui_prop_cache_slot + payload_size ), 0,
                                   ( cached_ui_prop_len - payload_size ) );
          }
        }
        else
        {
          ( void ) mmstd_memcpy( cur_ui_prop_cache_slot, remaining_ui_prop_cache_len, payload, payload_size );
        }

        destination_ui_prop_cache_slot = cur_ui_prop_cache_slot;
        is_cached = TRUE;
        break;
      }

      /* Save the free slot. */
      if ( ( cached_ui_prop->module_id == 0 ) && ( cached_ui_prop->param_id == 0 ) )
      {
        destination_ui_prop_cache_slot = cur_ui_prop_cache_slot;
        break;
      }

      cur_ui_prop_cache_slot += cached_ui_prop_len;
      remaining_ui_prop_cache_len -= cached_ui_prop_len;
      remaining_ui_prop_len -= cached_ui_prop_len;
    }

    /* If not already cached, copy the parameter to the first free slot */
   if( ( is_cached == FALSE ) && ( destination_ui_prop_cache_slot != NULL ) &&
       ( session_object->cached_ui_properties.num_ui_prop != VSM_MAX_UI_PROP ) && 
       ( remaining_ui_prop_cache_len >= payload_size ) )
    {
      ( void ) mmstd_memcpy( destination_ui_prop_cache_slot, remaining_ui_prop_cache_len, payload, payload_size );
      session_object->cached_ui_properties.data_len += payload_size;
      session_object->cached_ui_properties.num_ui_prop++;
      is_cached = TRUE;
    }

    if ( is_cached == FALSE )
    {
      session_object->cached_ui_properties.cache_fail_count++;
       MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_set_ui_property_cmd_control():"
            " Reached Maximum cached entries %d fail_count %d", VSM_MAX_UI_PROP,
              session_object->cached_ui_properties.cache_fail_count );
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_set_ui_property_cmd_control():"
             " Error caching Module id 0x%08X Param id 0x%08X",
             payload->module_id, payload->param_id );
      destination_ui_prop_cache_slot = ( uint8_t* )payload;
    }
    session_object->cmd_control.current_action = VSM_ACTION_ENUM_SET_PARAM;
    break;

  }

  for ( ; ; )
  {
    switch ( session_object->cmd_control.current_action )
    {
      case VSM_ACTION_ENUM_SET_PARAM:
      {
        rc = vsm_action_set_param( session_object, ( int32_t* )destination_ui_prop_cache_slot,
                                   payload_size, CVD_CAL_PARAM_MINOR_VERSION_0 );
        if ( ADSP_FAILED( rc ) )
        {
          session_object->cmd_control.cmd_result = ADSP_EFAILED;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
          continue;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_WAIT_1;
          return ADSP_EOK;
        }
      }
      break;

      case VSM_ACTION_ENUM_WAIT_1:
      {
        if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
        {
          MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_set_ui_property_cmd_control():"
                 " set_param error: %d.", session_object->cmd_control.resp_result );
        }
        session_object->cmd_control.current_action = VSM_ACTION_ENUM_COMPLETE;
        continue;
      }
      break;

      case VSM_ACTION_ENUM_ERROR_HANDLE:
      {
        if ( TRUE == session_object->cmd_control.is_resp_pending )
        {
          return ADSP_EOK;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
          continue;
        }
      }
      break;

      case VSM_ACTION_ENUM_REPORT_FAILURE:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"VSM_set_ui_property_cmd_control: command FAILED:." );
        elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, session_object->cmd_control.cmd_result );
        session_object->cmd_control.is_cmd_pending = FALSE;
        return ADSP_EOK;
      }
      break;

      case VSM_ACTION_ENUM_COMPLETE:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,"VSM_set_ui_property_cmd_control: command Success:." );
        elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EOK );
        session_object->cmd_control.is_cmd_pending = FALSE;
        return ADSP_EOK;
      }
      break;

      default:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"VSM_set_ui_property_cmd_control: ERROR: Unexpected..." );
      }
      return ADSP_EOK;
    }

  }

  return ADSP_EOK;
}

static int32_t VSM_set_ui_property_v2_cmd_control(
 vsm_session_object_t* session_object
)
{
  int32_t rc = ADSP_EOK;
  vss_icommon_cmd_set_ui_property_v2_t* payload;
  uint32_t payload_size;
  bool_t is_cached = FALSE;
  uint8_t* cur_ui_prop_cache_slot;
  uint32_t remaining_ui_prop_cache_len;
  uint8_t* destination_ui_prop_cache_slot = NULL;
  uint8_t* remaining_ui_prop;
  uint32_t remaining_ui_prop_len;
  voice_param_data_v2_t* cached_ui_prop;
  uint32_t cached_ui_prop_len;
  uint16_t client_addr;
  enum
  {
    VSM_ACTION_ENUM_UNINITIALIZED,
    VSM_ACTION_ENUM_SET_PARAM,
    VSM_ACTION_ENUM_WAIT_1,
    VSM_ACTION_ENUM_ERROR_HANDLE,
    VSM_ACTION_ENUM_REPORT_FAILURE,
    VSM_ACTION_ENUM_COMPLETE,
    VSM_ACTION_ENUM_INVALID
  };

  if( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  elite_apr_packet_t *apr_pkt = ( elite_apr_packet_t * )( session_object->cmd_control.cmd_msg.pPayload );

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED, "VSM_set_ui_property_v2_cmd_control():"
         " session_portID = 0x%08X", apr_pkt->dst_port );

  session_object->cmd_control.is_cmd_pending = TRUE;
  payload = APRV2_PKT_GET_PAYLOAD( vss_icommon_cmd_set_ui_property_v2_t, apr_pkt );
  payload_size = APRV2_PKT_GET_PAYLOAD_BYTE_SIZE( apr_pkt->header );

  while ( VSM_ACTION_ENUM_UNINITIALIZED == session_object->cmd_control.current_action )
  {
    if ( payload_size < sizeof( vss_icommon_cmd_set_ui_property_v2_t ) )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_set_ui_property_v2_cmd_control(): Unexpected payload size, %d < %d",
                                              payload_size,
                                              sizeof( vss_icommon_cmd_set_ui_property_v2_t ) );
      session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
      break;
    }

    if ( FALSE == vsm_utility_verify_param_data_size_aligned( payload->param_size ) )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_set_ui_property_v2_cmd_control():"
             " Invalid param data size, %d", payload->param_size );
      session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
      break;
    }

    if ( payload->param_size > VSM_MAX_UI_PROP_DATA_LEN )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_set_ui_property_v2_cmd_control():"
             " Invalid param data length %d > Max length %d", payload->param_size,
             VSM_MAX_UI_PROP_DATA_LEN );
      session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
      break;
    }

    if ( payload_size != ( payload->param_size + sizeof( voice_param_data_v2_t ) ) )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_set_ui_property_v2_cmd_control():"
             " Invalid payload size %d != %d", payload_size,
             ( payload->param_size + sizeof( voice_param_data_v2_t ) ) );
      session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
      break;
    }

    rc = vsm_utility_validate_and_update_ui_prop_version
       ( &session_object->cached_ui_properties.version,
         CVD_CAL_PARAM_MINOR_VERSION_1 );
    if ( rc ) 
    {
       session_object->cmd_control.current_action = VSM_ACTION_ENUM_SET_PARAM;
       session_object->cached_ui_properties.cache_fail_count++;
       MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_set_ui_property_v2_cmd_control():"
             " Invalid version. fail_count=%d", session_object->cached_ui_properties.cache_fail_count );
       destination_ui_prop_cache_slot = ( uint8_t* )payload;
       break;
    }

    MSG_3( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "VSM_set_ui_property_v2_cmd_control():"
           " Module Id 0x%08X / Param Id 0x%08X instance Id 0x%08X received ",
           payload->module_id, payload->param_id, payload->instance_id );

    cur_ui_prop_cache_slot = session_object->cached_ui_properties.data;
    remaining_ui_prop_cache_len = sizeof( session_object->cached_ui_properties.data );
    remaining_ui_prop_len = session_object->cached_ui_properties.data_len;

    /* Cache UI properties. */
    while ( remaining_ui_prop_cache_len > 0 )
    {
      cached_ui_prop = ( ( voice_param_data_v2_t* ) cur_ui_prop_cache_slot );
      cached_ui_prop_len = ( cached_ui_prop->param_size + sizeof( voice_param_data_v2_t ) );

      if ( ( cached_ui_prop->module_id == payload->module_id ) &&
           ( cached_ui_prop->param_id == payload->param_id ) &&
           ( cached_ui_prop->instance_id == payload->instance_id ) )
      {
        /* Value already exists. Cache it. */
        if ( cached_ui_prop_len != payload_size )
        { /* New UI property data size is different than the cached UI property data. */
          /* Shuffle the UI property cache to accommodate the new size. */
          remaining_ui_prop_len -= cached_ui_prop_len;
          remaining_ui_prop = ( cur_ui_prop_cache_slot + cached_ui_prop_len );

          while ( remaining_ui_prop_len >= cached_ui_prop_len )
          {
            ( void ) mmstd_memcpy( cur_ui_prop_cache_slot, remaining_ui_prop_cache_len, remaining_ui_prop, cached_ui_prop_len );
            remaining_ui_prop_len -= cached_ui_prop_len;
            remaining_ui_prop += cached_ui_prop_len;
            cur_ui_prop_cache_slot += cached_ui_prop_len;
            remaining_ui_prop_cache_len -= cached_ui_prop_len;
          }

          if ( remaining_ui_prop_len != 0 )
          {
            ( void ) mmstd_memcpy( cur_ui_prop_cache_slot, remaining_ui_prop_cache_len,
                                   remaining_ui_prop, remaining_ui_prop_len );
            cur_ui_prop_cache_slot += remaining_ui_prop_len;
            remaining_ui_prop_cache_len -= remaining_ui_prop_len;
          }

          ( void ) mmstd_memcpy( cur_ui_prop_cache_slot, remaining_ui_prop_cache_len, payload, payload_size );

          if ( cached_ui_prop_len < payload_size )
          { /* Increase the cached data length due to increased UI prop data. */
            session_object->cached_ui_properties.data_len += ( payload_size - cached_ui_prop_len );
          }
          else
          { /* Decrease the cached data length due to decreased UI prop data. */
            session_object->cached_ui_properties.data_len -= ( cached_ui_prop_len - payload_size );

            /* Set the decreased part of the cache that used to contain data to zero. */
            ( void ) mmstd_memset( ( cur_ui_prop_cache_slot + payload_size ), 0,
                                   ( cached_ui_prop_len - payload_size ) );
          }
        }
        else
        {
          ( void ) mmstd_memcpy( cur_ui_prop_cache_slot, remaining_ui_prop_cache_len, payload, payload_size );
        }

        destination_ui_prop_cache_slot = cur_ui_prop_cache_slot;
        is_cached = TRUE;
        break;
      }

      /* Save the free slot. */
      if ( ( cached_ui_prop->module_id == 0 ) && ( cached_ui_prop->param_id == 0 ) )
      {
        destination_ui_prop_cache_slot = cur_ui_prop_cache_slot;
        break;
      }

      cur_ui_prop_cache_slot += cached_ui_prop_len;
      remaining_ui_prop_cache_len -= cached_ui_prop_len;
      remaining_ui_prop_len -= cached_ui_prop_len;
    }

    /* If not already cached, copy the parameter to the first free slot */
    if( ( is_cached == FALSE ) && ( destination_ui_prop_cache_slot != NULL ) &&
        ( session_object->cached_ui_properties.num_ui_prop != VSM_MAX_UI_PROP ) && 
        ( remaining_ui_prop_cache_len >= payload_size )
        )
    {
      ( void ) mmstd_memcpy( destination_ui_prop_cache_slot, remaining_ui_prop_cache_len, payload, payload_size );
      session_object->cached_ui_properties.data_len += payload_size;
      session_object->cached_ui_properties.num_ui_prop++;
      is_cached = TRUE;
    }

    if ( is_cached == FALSE )
    {
      session_object->cached_ui_properties.cache_fail_count++;
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_set_ui_property_v2_cmd_control():"
           " Reached Maximum cached entries %d fail_count %d", VSM_MAX_UI_PROP,
             session_object->cached_ui_properties.cache_fail_count );
      MSG_3( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_set_ui_property_v2_cmd_control():"
             " Error caching Module id 0x%08X Param id 0x%08X instance id 0x%08X",
             payload->module_id, payload->param_id, payload->instance_id );
      destination_ui_prop_cache_slot = ( uint8_t* )payload;
    }
    session_object->cmd_control.current_action = VSM_ACTION_ENUM_SET_PARAM;
    break;

  }

  for ( ; ; )
  {
    switch ( session_object->cmd_control.current_action )
    {
      case VSM_ACTION_ENUM_SET_PARAM:
      {
        rc = vsm_action_set_param( session_object, ( int32_t* )destination_ui_prop_cache_slot,
                                   payload_size, CVD_CAL_PARAM_MINOR_VERSION_1 );
        if ( ADSP_FAILED( rc ) )
        {
          session_object->cmd_control.cmd_result = ADSP_EFAILED;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
          continue;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_WAIT_1;
          return ADSP_EOK;
        }
      }
      break;

      case VSM_ACTION_ENUM_WAIT_1:
      {
        if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
        {
          MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_set_ui_property_v2_cmd_control():"
                 " set_param error: %d.", session_object->cmd_control.resp_result );
        }
        session_object->cmd_control.current_action = VSM_ACTION_ENUM_COMPLETE;
        continue;
      }
      break;

      case VSM_ACTION_ENUM_ERROR_HANDLE:
      {
        if ( TRUE == session_object->cmd_control.is_resp_pending )
        {
          return ADSP_EOK;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
          continue;
        }
      }
      break;

      case VSM_ACTION_ENUM_REPORT_FAILURE:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"VSM_set_ui_property_v2_cmd_control: command FAILED:." );
        elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, session_object->cmd_control.cmd_result );
        session_object->cmd_control.is_cmd_pending = FALSE;
        return ADSP_EOK;
      }
      break;

      case VSM_ACTION_ENUM_COMPLETE:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,"VSM_set_ui_property_v2_cmd_control: command Success:." );
        elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EOK );
        session_object->cmd_control.is_cmd_pending = FALSE;
        return ADSP_EOK;
      }
      break;

      default:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"VSM_set_ui_property_v2_cmd_control: ERROR: Unexpected..." );
      }
      return ADSP_EOK;
    }

  }

  return ADSP_EOK;
}

static int32_t VSM_set_tty_mode_cmd_control ( vsm_session_object_t* session_object )
{
  int32_t rc = ADSP_EOK;
  uint32_t payload_size;
  vss_itty_cmd_set_tty_mode_t* in_args;

  if( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  elite_apr_packet_t *apr_pkt = ( elite_apr_packet_t * )( session_object->cmd_control.cmd_msg.pPayload );

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED, "VSM_set_tty_mode_cmd_control():"
         " session_portID = 0x%08X", apr_pkt->dst_port );

  /* Validate input params. */
  payload_size = APRV2_PKT_GET_PAYLOAD_BYTE_SIZE( apr_pkt->header );
  if( payload_size != sizeof( vss_itty_cmd_set_tty_mode_t ) )
  {
    session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
    rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EBADPARAM );
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_set_tty_mode_cmd_control():"
           "invalid payload size. %d", payload_size );
    return rc;
  }

  in_args = APRV2_PKT_GET_PAYLOAD( vss_itty_cmd_set_tty_mode_t, apr_pkt );

  if ( session_object->tty_info.tty_mode != in_args->mode )
  {
    if ( VSM_STATE_ENUM_RUN == session_object->state_control.state )
    {
      ( void ) vsm_action_send_reconfig_event( session_object );
    }

    /* Caching the TTY mode from MVM. */
    session_object->tty_info.tty_mode = in_args->mode;
  }

  rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EOK );
  MSG( MSG_SSID_DFLT, MSG_LEGACY_MED, "VSM_set_tty_mode_cmd_control: command Success:." );

  return rc;
}

static int32_t VSM_set_voice_timing_cmd_control ( vsm_session_object_t* session_object )
{
  int32_t rc = ADSP_EOK;
  uint32_t payload_size;
  vss_istream_cmd_set_voice_timing_t* in_args;

  if( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  elite_apr_packet_t *apr_pkt = ( elite_apr_packet_t * )( session_object->cmd_control.cmd_msg.pPayload );

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED, "VSM_set_voice_timing_cmd_control():"
         " session_portID = 0x%08X", apr_pkt->dst_port );

  /* Validate input params. */
  payload_size = APRV2_PKT_GET_PAYLOAD_BYTE_SIZE( apr_pkt->header );
  if( payload_size != sizeof( vss_istream_cmd_set_voice_timing_t ) )
  {
    session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
    rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EBADPARAM );
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_set_voice_timing_cmd_control():"
           "invalid payload size. %d", payload_size );
    return rc;
  }

  if ( session_object->state_control.state == VSM_STATE_ENUM_RUN )
  { /* Setting voice timing when the stream is running is not allowed. */
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_set_voice_timing_cmd_control(): Stream is running." );
    rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EFAILED );
    return rc;
  }

  in_args = APRV2_PKT_GET_PAYLOAD( vss_istream_cmd_set_voice_timing_t, apr_pkt );

  /* Save the timing parameters in target_set to be picked up by before enable. */
  session_object->target_set.voice_timing = *in_args;

  rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EOK );
  MSG( MSG_SSID_DFLT, MSG_LEGACY_MED, "VSM_set_voice_timing_cmd_control: command Success:." );

  return rc;
}

static int32_t VSM_attach_detach_vocproc_cmd_control(
  vsm_session_object_t* session_object,
  bool_t is_attach
)
{
  ADSPResult rc = ADSP_EOK;
  uint32_t payload_size;
  vss_istream_cmd_attach_vocproc_t* in_args;
  enum
  {
    VSM_ACTION_ENUM_UNINITIALIZED,
    VSM_ACTION_ENUM_UPDATE_TO_VOCPROC_1,   //  TBD: replace with VSM_ACTION_ENUM_GET_VPM_MIXER_PORTS
    VSM_ACTION_ENUM_WAIT_2,
    VSM_ACTION_ENUM_UPDATE_MIXER_PORT_MAPPING_3,
    VSM_ACTION_ENUM_REASSIGN_MIXER_INOUT_PORTS_4,
    VSM_ACTION_ENUM_WAIT_5,
    VSM_ACTION_ENUM_ERROR_HANDLE,
    VSM_ACTION_ENUM_REPORT_FAILURE,
    VSM_ACTION_ENUM_COMPLETE,
    VSM_ACTION_ENUM_INVALID
  };

  if( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  elite_apr_packet_t *apr_pkt = ( elite_apr_packet_t * )( session_object->cmd_control.cmd_msg.pPayload );

  MSG_3( MSG_SSID_DFLT, MSG_LEGACY_MED, "VSM_attach_detach_vocproc_cmd_control(): is_attach %d, session_portID = 0x%08X,"
         " current_action %d", is_attach, apr_pkt->dst_port, session_object->cmd_control.current_action );

  session_object->cmd_control.is_cmd_pending = TRUE;

  while ( VSM_ACTION_ENUM_UNINITIALIZED == session_object->cmd_control.current_action )
  {
    /* Validate input params. */
    payload_size = APRV2_PKT_GET_PAYLOAD_BYTE_SIZE( apr_pkt->header );
    if( payload_size != sizeof( vss_istream_cmd_attach_vocproc_t ) )
    {
      session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_attach_detach_vocproc_cmd_control():"
             "invalid payload size. %d", payload_size );
      break;
    }
    in_args = APRV2_PKT_GET_PAYLOAD( vss_istream_cmd_attach_vocproc_t, apr_pkt );

    session_object->target_set.attach_detach_vocproc_handle = in_args->handle;
    session_object->target_set.attach_detach_vocproc_direction = session_object->direction;
    session_object->cmd_control.current_action = VSM_ACTION_ENUM_UPDATE_TO_VOCPROC_1;
    break;

  }

  for ( ; ; )
  {
    switch ( session_object->cmd_control.current_action )
    {
      case VSM_ACTION_ENUM_UPDATE_TO_VOCPROC_1:
      {
        if ( TRUE == is_attach )
        {
          rc = vsm_action_send_attach_to_vocproc( session_object );
        }
        else
        {
          rc = vsm_action_send_detach_to_vocproc( session_object );
        }
        if ( ADSP_FAILED( rc ) )
        {
          session_object->cmd_control.cmd_result = ADSP_EFAILED;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
          continue;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_WAIT_2;
          return ADSP_EOK;
        }
      }
      break;

      case VSM_ACTION_ENUM_WAIT_2:
      {
        if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
        {
          session_object->cmd_control.cmd_result = session_object->cmd_control.resp_result;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_UPDATE_MIXER_PORT_MAPPING_3;
        }
        continue;
      }
      break;

      case VSM_ACTION_ENUM_UPDATE_MIXER_PORT_MAPPING_3:
      {
        rc = vsm_action_update_mixer_port_mapping( session_object );
        if ( ADSP_FAILED( rc ) )
        {
          session_object->cmd_control.cmd_result = ADSP_EFAILED;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_REASSIGN_MIXER_INOUT_PORTS_4;
        }
        continue;
      }
      break;

      case VSM_ACTION_ENUM_REASSIGN_MIXER_INOUT_PORTS_4:
      {
        rc = vsm_action_reassign_mixer_inout_ports( session_object, is_attach );
        if ( ADSP_FAILED( rc ) )
        {
          session_object->cmd_control.cmd_result = ADSP_EFAILED;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
          continue;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_WAIT_5;
          return ADSP_EOK;
        }
      }
      break;

      case VSM_ACTION_ENUM_WAIT_5:
      {
        if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
        {
          session_object->cmd_control.cmd_result = session_object->cmd_control.resp_result;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_COMPLETE;
        }
        continue;
      }
      break;

      case VSM_ACTION_ENUM_ERROR_HANDLE:
      {
        if ( TRUE == session_object->cmd_control.is_resp_pending )
        {
          return ADSP_EOK;
        }
        else
        {
          //Do error handling
//          ( void )vsm_action_cleanup_create_errors( session_object );
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
          if ( TRUE == session_object->cmd_control.is_resp_pending )
          {
            return ADSP_EOK;
          }
          continue;
        }
      }
      break;

      case VSM_ACTION_ENUM_REPORT_FAILURE:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"VSM_attach_detach_vocproc_cmd_control: command FAILED:." );
        elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, session_object->cmd_control.cmd_result );
        session_object->cmd_control.is_cmd_pending = FALSE;
        return ADSP_EOK;
      }
      break;

      case VSM_ACTION_ENUM_COMPLETE:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,"VSM_attach_detach_vocproc_cmd_control: command Success:." );
        elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EOK );
        session_object->cmd_control.is_cmd_pending = FALSE;
        return ADSP_EOK;
      }
      break;

      default:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"VSM_attach_detach_vocproc_cmd_control: ERROR: Unexpected..." );
      }
      return ADSP_EOK;
    }
  }

  return rc;
}

static int32_t VSM_mvm_detach_cmd_control( vsm_session_object_t* session_object )
{
  int32_t rc = ADSP_EOK;
  vss_istream_evt_voc_operating_mode_update_t voc_op_mode_update;

  elite_apr_packet_t *apr_pkt = ( elite_apr_packet_t * )( session_object->cmd_control.cmd_msg.pPayload );

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED, "VSM_mvm_detach_cmd_control(): session_portID = 0x%08X", apr_pkt->dst_port );

  if ( session_object->attached_mvm_handle == APR_NULL_V )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_mvm_detach_cmd_control(): Stream is currently " \
                                            "not attached to any MVM session " );
    rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EHANDLE );
    return rc;
  }

  if ( session_object->attached_mvm_handle != apr_pkt->src_port )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_mvm_detach_cmd_control(): Stream is currently " \
           "attached to the MVM session handle 0x%04X, and the caller MVM session's handle is 0x%04X.",
            session_object->attached_mvm_handle, apr_pkt->src_port );
    rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EFAILED );
    return rc;
  }

  session_object->attached_mvm_handle = APR_NULL_V;

  rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EOK );
  MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,"VSM_mvm_detach_cmd_control: command Success:." );

  return rc;
}

static int32_t VSM_mvm_attach_cmd_control( vsm_session_object_t* session_object )
{
  int32_t rc = ADSP_EOK;
  vss_istream_evt_voc_operating_mode_update_t voc_op_mode_update;

  elite_apr_packet_t *apr_pkt = ( elite_apr_packet_t * )( session_object->cmd_control.cmd_msg.pPayload );

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED, "VSM_mvm_attach_cmd_control(): session_portID = 0x%08X", apr_pkt->dst_port );

  if ( session_object->attached_mvm_handle == apr_pkt->src_port )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_mvm_attach_cmd_control(): Stream has already been " \
                                            "attached to the MVM session handle 0x%04X.",
                                            apr_pkt->src_port );
    rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EALREADY );
    return rc;
  }

  if ( session_object->attached_mvm_handle != APR_NULL_V )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_mvm_attach_cmd_control(): Stream is currently " \
                                            "attached to the MVM session handle 0x%04X, and cannot be " \
                                            "attached to the new MVM session handle 0x%04X",
                                            session_object->attached_mvm_handle,
                                            apr_pkt->src_port );
    rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EFAILED );
    return rc;
  }

  session_object->attached_mvm_handle = apr_pkt->src_port;

  /* Send VSS_ISTREAM_EVT_VOC_OPERATING_MODE_UPDATE event to MVM. */
  voc_op_mode_update.rx_mode = session_object->voc_operating_mode_info.rx_mode;
  voc_op_mode_update.tx_mode = session_object->voc_operating_mode_info.tx_mode;

  rc = elite_apr_if_alloc_send_event(
         vsm_apr_handle, session_object->apr_info.self_addr,
         ( ( uint16_t ) session_object->apr_info.self_port ),
         vsm_mvm_addr, session_object->attached_mvm_handle,
         0, VSS_ISTREAM_EVT_VOC_OPERATING_MODE_UPDATE,
         &voc_op_mode_update, sizeof( voc_op_mode_update ) );

  if ( rc )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED, "VSM_mvm_attach_cmd_control():"
           " failed to send mode update event to MVM error: 0x%X", rc );
  }

  rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EOK );
  MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,"VSM_mvm_attach_cmd_control: command Success:." );

  return rc;
}

static int32_t VSM_set_var_voc_sampling_rate_cmd_control( vsm_session_object_t* session_object )
{
  int32_t rc = ADSP_EOK;
  uint32_t payload_size;
  vss_icommon_cmd_set_var_voc_sampling_rate_t* in_args;
  uint32_t media_id = VSS_MEDIA_ID_NONE;

  enum
  {
    VSM_ACTION_ENUM_UNINITIALIZED,
    VSM_ACTION_ENUM_SET_PP_SAMPLE_RATE_1,
    VSM_ACTION_ENUM_WAIT_2,
    VSM_ACTION_ENUM_RECONFIG_TX_MIXER_3,
    VSM_ACTION_ENUM_WAIT_4,
    VSM_ACTION_ENUM_UPDATE_CACHED_SYSTEM_SAMPLING_RATES_5,
    VSM_ACTION_ENUM_ERROR_HANDLE,
    VSM_ACTION_ENUM_REPORT_FAILURE,
    VSM_ACTION_ENUM_COMPLETE,
    VSM_ACTION_ENUM_INVALID
  };

  if( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  elite_apr_packet_t *apr_pkt = ( elite_apr_packet_t * )( session_object->cmd_control.cmd_msg.pPayload );

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "VSM_set_var_voc_sampling_rate_cmd_control():  session_portID = 0x%08X,"
         " current_action %d", apr_pkt->dst_port, session_object->cmd_control.current_action );

  session_object->cmd_control.is_cmd_pending = TRUE;

  while ( VSM_ACTION_ENUM_UNINITIALIZED == session_object->cmd_control.current_action )
  {
    /* Validate input params. */
    payload_size = APRV2_PKT_GET_PAYLOAD_BYTE_SIZE( apr_pkt->header );
    if( payload_size != sizeof( vss_icommon_cmd_set_var_voc_sampling_rate_t ) )
    {
      session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_set_var_voc_sampling_rate_cmd_control():"
             "invalid payload size. %d", payload_size );
      break;
    }

    in_args = APRV2_PKT_GET_PAYLOAD( vss_icommon_cmd_set_var_voc_sampling_rate_t, apr_pkt );
    session_object->requested_var_voc_tx_sampling_rate = in_args->tx_pp_sr;
    session_object->requested_var_voc_rx_sampling_rate = in_args->rx_pp_sr;

    session_object->cmd_control.current_action = VSM_ACTION_ENUM_SET_PP_SAMPLE_RATE_1;
    break;
  }

  for ( ; ; )
  {
    switch ( session_object->cmd_control.current_action )
    {
      case VSM_ACTION_ENUM_SET_PP_SAMPLE_RATE_1:
      {
        rc = vsm_action_set_pp_sampling_rate( session_object );
        if( ADSP_FAILED( rc ) )
        {
          session_object->cmd_control.cmd_result = ADSP_EFAILED;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
          continue;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_WAIT_2;
          return ADSP_EOK;
        }
      }
      break;

      case VSM_ACTION_ENUM_WAIT_2:
      {
        if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
        {
          session_object->cmd_control.cmd_result = session_object->cmd_control.resp_result;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
        }
        else
        {
          if ( session_object->active_tx_sampling_rate != session_object->requested_var_voc_tx_sampling_rate )
          {
            session_object->cmd_control.current_action = VSM_ACTION_ENUM_RECONFIG_TX_MIXER_3;
          }
          else
          {
            session_object->cmd_control.current_action = VSM_ACTION_ENUM_UPDATE_CACHED_SYSTEM_SAMPLING_RATES_5;
          }
        }
        continue;
      }
      break;

      case VSM_ACTION_ENUM_RECONFIG_TX_MIXER_3:
      {
        rc = vsm_action_reconfig_tx_mixer( session_object );
        if ( ADSP_FAILED( rc ) )
        {
          session_object->cmd_control.cmd_result = ADSP_EFAILED;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
          continue;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_WAIT_4;
          return ADSP_EOK;
        }
      }
      break;

      case VSM_ACTION_ENUM_WAIT_4:
      {
        if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
        {
          session_object->cmd_control.cmd_result = session_object->cmd_control.resp_result;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_UPDATE_CACHED_SYSTEM_SAMPLING_RATES_5;
        }
        continue;
      }
      break;

      case VSM_ACTION_ENUM_UPDATE_CACHED_SYSTEM_SAMPLING_RATES_5:
      {
        ( void )vsm_action_update_cached_system_sampling_rates( session_object );
        session_object->cmd_control.current_action = VSM_ACTION_ENUM_COMPLETE;
        continue;
      }
      break;

      case VSM_ACTION_ENUM_ERROR_HANDLE:
      {
        if ( TRUE == session_object->cmd_control.is_resp_pending )
        {
          return ADSP_EOK;
        }
        else
        {
          //Do error handling if required
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
          continue;
        }
      }
      break;

      case VSM_ACTION_ENUM_REPORT_FAILURE:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"VSM_set_var_voc_sampling_rate_cmd_control: command FAILED:." );
        elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, session_object->cmd_control.cmd_result );
        session_object->cmd_control.is_cmd_pending = FALSE;
        return ADSP_EOK;
      }
      break;

      case VSM_ACTION_ENUM_COMPLETE:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,"VSM_set_var_voc_sampling_rate_cmd_control: command Success:." );
        session_object->cmd_control.cmd_result = ADSP_EOK;
        elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EOK );
        session_object->cmd_control.is_cmd_pending = FALSE;
        return ADSP_EOK;
      }
      break;

      default:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"VSM_set_var_voc_sampling_rate_cmd_control: ERROR: Unexpected..." );
      }
      return ADSP_EOK;
    }

  }
  return rc;
}

static int32_t VSM_set_media_type_cmd_control( vsm_session_object_t* session_object )
{
  int32_t rc = ADSP_EOK;
  uint32_t payload_size;
  vss_istream_cmd_set_media_type_t* in_args;
  uint32_t media_id = VSS_MEDIA_ID_NONE;

  enum
  {
    VSM_ACTION_ENUM_UNINITIALIZED,
    VSM_ACTION_ENUM_SET_MEDIA_TYPE_1,
    VSM_ACTION_ENUM_WAIT_2,
    VSM_ACTION_ENUM_SET_PP_SAMPLE_RATE_3,
    VSM_ACTION_ENUM_WAIT_4,
    VSM_ACTION_ENUM_RECONFIG_TX_MIXER_5,
    VSM_ACTION_ENUM_WAIT_6,
    VSM_ACTION_ENUM_UPDATE_CACHED_SYSTEM_SAMPLING_RATES_7,
    VSM_ACTION_ENUM_ERROR_HANDLE,
    VSM_ACTION_ENUM_REPORT_FAILURE,
    VSM_ACTION_ENUM_COMPLETE,
    VSM_ACTION_ENUM_INVALID
  };

  if( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  elite_apr_packet_t *apr_pkt = ( elite_apr_packet_t * )( session_object->cmd_control.cmd_msg.pPayload );

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW, "VSM_set_media_type_cmd_control():  session_portID = 0x%08X,"
         " current_action %d", apr_pkt->dst_port, session_object->cmd_control.current_action );

  session_object->cmd_control.is_cmd_pending = TRUE;

  while ( VSM_ACTION_ENUM_UNINITIALIZED == session_object->cmd_control.current_action )
  {
    if ( VSM_STATE_ENUM_IDLE != session_object->state_control.state )
    {
      session_object->cmd_control.cmd_result = ADSP_EUNEXPECTED;
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_set_media_type_cmd_control():"
             "session is not in IDLE state.  state: %d", session_object->state_control.state );
      break;
    }

    /* This command is allowed only with full control handle. */
    if ( session_object->apr_info.client_port != apr_pkt->src_port )
    {
      session_object->cmd_control.cmd_result = ADSP_EHANDLE;
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_set_media_type_cmd_control():"
             "This command is allowd only for full control client. src_addr 0x%X src_port 0x%X",
             apr_pkt->src_addr, apr_pkt->src_port );
      break;
    }

    /* Validate input params. */
    payload_size = APRV2_PKT_GET_PAYLOAD_BYTE_SIZE( apr_pkt->header );
    if( payload_size != sizeof( vss_istream_cmd_set_media_type_t ) )
    {
      session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_set_media_type_cmd_control():"
             "invalid payload size. %d", payload_size );
      break;
    }

    in_args = APRV2_PKT_GET_PAYLOAD( vss_istream_cmd_set_media_type_t, apr_pkt );

    switch ( session_object->direction )
    {
    case VSM_DIRECTION_RX:
      media_id = in_args->rx_media_id;
      break;

    case VSM_DIRECTION_TX:
      media_id = in_args->tx_media_id;
      break;

    case VSM_DIRECTION_RX_TX:
      {
        if ( in_args->rx_media_id != in_args->tx_media_id )
        {
          /* VSM currently doesn't support independent Rx and Tx media types. */
          MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_set_media_type_cmd_control():"
                 "Different Tx and Rx media IDs not allowed tx 0x%X  rx 0x%X",
                 in_args->tx_media_id, in_args->rx_media_id );
          rc = ADSP_EBADPARAM;
        }
        else
        {
          media_id = in_args->rx_media_id;
        }
      }
      break;

    default:
      rc = ADSP_EUNEXPECTED;
      break;
    }

    if ( rc )
    {
      session_object->cmd_control.cmd_result = rc;
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
      break;
    }

    if ( FALSE == vsm_utility_media_id_is_valid( media_id ) )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_set_media_type_cmd_control():"
             "Invalid media ID 0x%X", media_id );
      session_object->cmd_control.cmd_result = ADSP_EBADPARAM;
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
      break;
    }

    session_object->target_set.media_id = media_id;
    session_object->cmd_control.current_action = VSM_ACTION_ENUM_SET_MEDIA_TYPE_1;
    break;
  }

  for ( ; ; )
  {
    switch ( session_object->cmd_control.current_action )
    {

      case VSM_ACTION_ENUM_SET_MEDIA_TYPE_1:
      {
        ( void ) vsm_utility_reset_cached_vocoder_properties( session_object );
        rc = vsm_action_set_media_type( session_object );
        if( ADSP_FAILED( rc ) )
        {
          session_object->cmd_control.cmd_result = ADSP_EFAILED;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
          continue;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_WAIT_2;
          return ADSP_EOK;
        }
      }
      break;

      case VSM_ACTION_ENUM_WAIT_2:
      {
        if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
        {
          session_object->cmd_control.cmd_result = session_object->cmd_control.resp_result;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
        }
        else
        {
          if ( session_object->active_set.media_id != session_object->target_set.media_id )
          {
            MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,"VSM_set_media_type_cmd_control: stream_config changed." );
            session_object->is_stream_config_changed = TRUE;
            /* Clearing the cached vocoder properties for the previous media_id. */
            session_object->stream_param_cache.num_voc_params = 0;
            session_object->stream_param_cache.data_len = 0;
          }
          if ( vsm_utility_vocoder_is_variable( session_object->target_set.media_id ) )
          {
            session_object->cmd_control.current_action = VSM_ACTION_ENUM_SET_PP_SAMPLE_RATE_3;
          }
          else if( voice_get_sampling_rate( session_object->target_set.media_id, 0 ,(uint16_t )session_object->requested_var_voc_tx_sampling_rate ) != 
                   session_object->active_tx_sampling_rate
                 )
          {
            session_object->cmd_control.current_action = VSM_ACTION_ENUM_RECONFIG_TX_MIXER_5;
          }
          else
          {
            session_object->cmd_control.current_action = VSM_ACTION_ENUM_COMPLETE;
          }
          session_object->active_set.media_id = session_object->target_set.media_id;
          session_object->target_set.system_config.media_id = session_object->active_set.media_id;
        }
        continue;
      }
      break;

      case VSM_ACTION_ENUM_SET_PP_SAMPLE_RATE_3:
      {
        rc = vsm_action_set_pp_sampling_rate( session_object );
        if ( ADSP_FAILED( rc ) )
        {
          session_object->cmd_control.cmd_result = ADSP_EFAILED;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
          continue;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_WAIT_4;
          return ADSP_EOK;
        }
      }
      break;

      case VSM_ACTION_ENUM_WAIT_4:
      {
        if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
        {
          session_object->cmd_control.cmd_result = session_object->cmd_control.resp_result;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
        }
        else
        {
          if ( voice_get_sampling_rate( session_object->active_set.media_id, 0 , (uint16_t )session_object->requested_var_voc_tx_sampling_rate ) != 
                session_object->active_tx_sampling_rate
             )
          {
            session_object->cmd_control.current_action = VSM_ACTION_ENUM_RECONFIG_TX_MIXER_5;
          }
          else
          {
            session_object->cmd_control.current_action = VSM_ACTION_ENUM_UPDATE_CACHED_SYSTEM_SAMPLING_RATES_7;
          }
        }
        continue;
      }
      break;

      case VSM_ACTION_ENUM_RECONFIG_TX_MIXER_5:
      {
        rc = vsm_action_reconfig_tx_mixer( session_object );
        if ( ADSP_FAILED( rc ) )
        {
          session_object->cmd_control.cmd_result = ADSP_EFAILED;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
          continue;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_WAIT_6;
          return ADSP_EOK;
        }
      }
      break;

      case VSM_ACTION_ENUM_WAIT_6:
      {
        if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
        {
          session_object->cmd_control.cmd_result = session_object->cmd_control.resp_result;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_UPDATE_CACHED_SYSTEM_SAMPLING_RATES_7;
        }
        continue;
      }
      break;

      case VSM_ACTION_ENUM_UPDATE_CACHED_SYSTEM_SAMPLING_RATES_7:
      {
        ( void )vsm_action_update_cached_system_sampling_rates( session_object );
        session_object->cmd_control.current_action = VSM_ACTION_ENUM_COMPLETE;
        continue;
      }
      break;

      case VSM_ACTION_ENUM_ERROR_HANDLE:
      {
        if ( TRUE == session_object->cmd_control.is_resp_pending )
        {
          return ADSP_EOK;
        }
        else
        {
          //Do error handling if required
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
          continue;
        }
      }
      break;

      case VSM_ACTION_ENUM_REPORT_FAILURE:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"VSM_set_media_type_cmd_control: command FAILED:." );
        elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, session_object->cmd_control.cmd_result );
        session_object->cmd_control.is_cmd_pending = FALSE;
        return ADSP_EOK;
      }
      break;

      case VSM_ACTION_ENUM_COMPLETE:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,"VSM_set_media_type_cmd_control: command Success:." );
        session_object->cmd_control.cmd_result = ADSP_EOK;
        elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EOK );
        session_object->cmd_control.is_cmd_pending = FALSE;
        return ADSP_EOK;
      }
      break;

      default:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"VSM_set_media_type_cmd_control: ERROR: Unexpected..." );
      }
      return ADSP_EOK;
    }

  }

  return rc;
}

static int32_t VSM_create_session_cmd_control( vsm_session_object_t* session_object )
{
  ADSPResult rc = ADSP_EOK;
  enum
  {
    VSM_ACTION_ENUM_UNINITIALIZED,
    VSM_ACTION_ENUM_SET_CLOCKS_1,
    VSM_ACTION_ENUM_CREATE_DYNAMIC_SERVICES_2,
    VSM_ACTION_ENUM_CREATE_MIXER_3,
    VSM_ACTION_ENUM_WAIT_4,
    VSM_ACTION_ENUM_CONNECT_DOWN_STREAM_5,
    VSM_ACTION_ENUM_WAIT_6,
    VSM_ACTION_ENUM_REVERT_CLOCKS_7,
    VSM_ACTION_ENUM_SET_MEDIA_TYPE_8,
    VSM_ACTION_ENUM_WAIT_9,
    VSM_ACTION_ENUM_RECONFIG_TX_MIXER_10,
    VSM_ACTION_ENUM_WAIT_11,
    VSM_ACTION_ENUM_REGISTER_VOC_OP_MODE_EVENT_12,
    VSM_ACTION_ENUM_WAIT_13,
    VSM_ACTION_ENUM_ERROR_HANDLE,
    VSM_ACTION_ENUM_REPORT_FAILURE,
    VSM_ACTION_ENUM_COMPLETE,
    VSM_ACTION_ENUM_INVALID
  };

  if( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  elite_apr_packet_t *apr_pkt = ( elite_apr_packet_t * )( session_object->cmd_control.cmd_msg.pPayload );

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "VSM_create_session_cmd_control(): Session portID = 0x%08X, current_action %d",
         session_object->apr_info.self_port, session_object->cmd_control.current_action );

  session_object->cmd_control.is_cmd_pending = TRUE;

  if ( VSM_ACTION_ENUM_UNINITIALIZED == session_object->cmd_control.current_action )
  {
    /* Connect To AFE. */
    if ( qurt_elite_globalstate.pAfeStatSvcCmdQ == NULL )
    {
      session_object->cmd_control.cmd_result = ADSP_EUNEXPECTED;
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
    }
    else
    {
      session_object->vsm_afe_cmd_q_ptr = qurt_elite_globalstate.pAfeStatSvcCmdQ;
      if ( 0 == session_object->ref_count )
      {
        vsm_utility_initialize_session_variables( session_object );
        session_object->cmd_control.current_action = VSM_ACTION_ENUM_SET_CLOCKS_1;
      }
      else
      {
        /* Passive session got created earlier. Call from full control client now.*/
        session_object->apr_info.client_addr = apr_pkt->src_addr;
        session_object->apr_info.client_port = apr_pkt->src_port;
        vsm_utility_initiate_fullcontrol_client_info(
          session_object,
          ( vss_istream_cmd_create_full_control_session_t* )elite_apr_if_get_payload_ptr( apr_pkt ) );

        if ( VSS_MEDIA_ID_NONE == session_object->target_set.media_id )
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_COMPLETE;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_SET_MEDIA_TYPE_8;
        }
      }
    }
  }

  for ( ; ; )
  {
    switch ( session_object->cmd_control.current_action )
    {
      case VSM_ACTION_ENUM_SET_CLOCKS_1:
      {
        rc = vccm_mmpm_request_clocks( VCCM_CLIENT_ID_CVS, VSM_SESSION_CREATE_MPPS );
        if ( ADSP_FAILED( rc ) )
        {
          MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR," VSM_create_session_cmd_control: "
                                " failed to request clocks with rc().",rc );
        }
        session_object->is_clk_overridden = TRUE;
        session_object->cmd_control.current_action = VSM_ACTION_ENUM_CREATE_DYNAMIC_SERVICES_2;
      }//fall through

      case VSM_ACTION_ENUM_CREATE_DYNAMIC_SERVICES_2:
      {
        rc = vsm_action_create_dynamic_services( session_object );
        if ( ADSP_FAILED( rc ) )
        {
          session_object->cmd_control.cmd_result = ADSP_EFAILED;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
          continue;
        }
        session_object->cmd_control.current_action = VSM_ACTION_ENUM_CREATE_MIXER_3;
      }
      //fall through

      case VSM_ACTION_ENUM_CREATE_MIXER_3:
      {
        rc = vsm_action_create_mixer_input_ouput( session_object );
        if ( ADSP_FAILED( rc ) )
        {
          session_object->cmd_control.cmd_result = ADSP_EFAILED;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
          continue;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_WAIT_4;
          return ADSP_EOK;
        }
      }
      break;

      case VSM_ACTION_ENUM_WAIT_4:
      {
        if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
        {
          session_object->cmd_control.cmd_result = session_object->cmd_control.resp_result;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_CONNECT_DOWN_STREAM_5;
        }
        continue;
      }
      break;

      case VSM_ACTION_ENUM_CONNECT_DOWN_STREAM_5:
      {
        rc = vsm_action_connect_rx_downstream( session_object );
        if( ADSP_FAILED( rc ) )
        {
          session_object->cmd_control.cmd_result = ADSP_EFAILED;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
          continue;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_WAIT_6;
          return ADSP_EOK;
        }
      }
      break;

      case VSM_ACTION_ENUM_WAIT_6:
      {
        if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
        {
          session_object->cmd_control.cmd_result = session_object->cmd_control.resp_result;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_REVERT_CLOCKS_7;
        }
        continue;
      }
      break;

      case VSM_ACTION_ENUM_REVERT_CLOCKS_7:
      { 
        if ( TRUE == session_object->is_clk_overridden )
        {
          session_object->is_clk_overridden = FALSE;
          rc = vccm_mmpm_release_clocks( VCCM_CLIENT_ID_CVS );
          if ( ADSP_FAILED( rc ) )
          {
            MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR," VSM_create_session_cmd_control: "
                                  " failed to revert clocks with rc().",rc );
          }
        }
        session_object->cmd_control.current_action = VSM_ACTION_ENUM_SET_MEDIA_TYPE_8;
      }//fall through

      case VSM_ACTION_ENUM_SET_MEDIA_TYPE_8:
      {
        rc = vsm_action_set_media_type( session_object );
        if( ADSP_FAILED( rc ) )
        {
          session_object->cmd_control.cmd_result = ADSP_EFAILED;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
          continue;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_WAIT_9;
          return ADSP_EOK;
        }
      }
      break;

      case VSM_ACTION_ENUM_WAIT_9:
      {
        if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
        {
          session_object->cmd_control.cmd_result = session_object->cmd_control.resp_result;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
        }
        else
        {
          if( VSM_MEDIA_TYPE_NONE != session_object->target_set.media_id )
          {
            session_object->cmd_control.current_action = VSM_ACTION_ENUM_RECONFIG_TX_MIXER_10;
          }
          else
          {
            session_object->cmd_control.current_action = VSM_ACTION_ENUM_REGISTER_VOC_OP_MODE_EVENT_12;
          }
          session_object->active_set.media_id = session_object->target_set.media_id;
          session_object->target_set.system_config.media_id = session_object->active_set.media_id;
        }
        continue;
      }
      break;

      case VSM_ACTION_ENUM_RECONFIG_TX_MIXER_10:
      {
        rc = vsm_action_reconfig_tx_mixer( session_object );
        if ( ADSP_FAILED( rc ) )
        {
          session_object->cmd_control.cmd_result = ADSP_EFAILED;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
          continue;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_WAIT_11;
          return ADSP_EOK;
        }
      }
      break;

      case VSM_ACTION_ENUM_WAIT_11:
      {
        if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
        {
          session_object->cmd_control.cmd_result = session_object->cmd_control.resp_result;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_REGISTER_VOC_OP_MODE_EVENT_12;
        }
        continue;
      }
      break;

      case VSM_ACTION_ENUM_REGISTER_VOC_OP_MODE_EVENT_12:
      {
        rc = vsm_action_register_event( session_object, VSM_EVT_VOC_OPERATING_MODE_UPDATE );
        if( ADSP_FAILED( rc ) )
        {
          session_object->cmd_control.cmd_result = ADSP_EFAILED;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
          continue;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_WAIT_13;
          return ADSP_EOK;
        }
      }
      break;

      case VSM_ACTION_ENUM_WAIT_13:
      {
        if ( FALSE == vsm_utility_is_voc_op_mode_event_received( session_object ) )
        {
          return ADSP_EOK;
        }
        else
        {
          session_object->cmd_control.cmd_result = ADSP_EOK;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_COMPLETE;
        }
        continue;
      }
      break;

      case VSM_ACTION_ENUM_ERROR_HANDLE:
      {
        if ( TRUE == session_object->cmd_control.is_resp_pending )
        {
          return ADSP_EOK;
        }
        else
        {
          //Do error handling
          ( void )vsm_action_cleanup_create_errors( session_object );

          if ( TRUE == session_object->is_clk_overridden )
          {
            session_object->is_clk_overridden = FALSE;
            ( void )vccm_mmpm_release_clocks( VCCM_CLIENT_ID_CVS );
          }
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
          if ( TRUE == session_object->cmd_control.is_resp_pending )
          {
            return ADSP_EOK;
          }
          continue;
        }
      }
      break;

      case VSM_ACTION_ENUM_REPORT_FAILURE:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"VSM_create_session_cmd_control: command FAILED:." );
        elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, session_object->cmd_control.cmd_result );
        ( void )vsm_action_free_session_resources( session_object );
        return ADSP_EOK;
      }
      break;

      case VSM_ACTION_ENUM_COMPLETE:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,"VSM_create_session_cmd_control: command Success:." );
        rc = elite_apr_if_alloc_send_ack_result(
               vsm_apr_handle,
               session_object->apr_info.self_addr,
               session_object->apr_info.self_port,      /* give back new handle if valid */
               elite_apr_if_get_src_addr( apr_pkt ),
               elite_apr_if_get_src_port( apr_pkt ),
               elite_apr_if_get_client_token( apr_pkt ),
               elite_apr_if_get_opcode( apr_pkt ),
               ADSP_EOK );

        /* free the original packet */
        rc = elite_apr_if_free( vsm_apr_handle, apr_pkt );
        session_object->ref_count++;
        session_object->state_control.state = VSM_STATE_ENUM_IDLE;
        session_object->cmd_control.is_cmd_pending = FALSE;
        return ADSP_EOK;
      }
      break;

      default:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"VSM_create_session_cmd_control: ERROR: Unexpected..." );
      }
      return ADSP_EOK;
    }
  }
}

static int32_t VSM_destroy_session_cmd_control( vsm_session_object_t* session_object )
{
  ADSPResult rc = ADSP_EOK;
  elite_apr_packet_t* apr_pkt;
  bool_t is_full_control_client = FALSE;

  enum
  {
    VSM_ACTION_ENUM_UNINITIALIZED,
    VSM_ACTION_ENUM_RESET_PACKET_EXCHANGE_MODE_TO_INBAND_1,
    VSM_ACTION_ENUM_WAIT_2,
    VSM_ACTION_ENUM_CLEAR_MAILBOX_CONFIGURATION_3,
    VSM_ACTION_ENUM_CLEAR_OOB_CONFIGURATION_4,
    VSM_ACTION_ENUM_CLOSE_MIXER_5,
    VSM_ACTION_ENUM_WAIT_6,
    VSM_ACTION_ENUM_ERROR_HANDLE,
    VSM_ACTION_ENUM_REPORT_FAILURE,
    VSM_ACTION_ENUM_COMPLETE,
    VSM_ACTION_ENUM_INVALID
  };

  if ( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }
  apr_pkt = ( elite_apr_packet_t* )session_object->cmd_control.cmd_msg.pPayload;
  session_object->cmd_control.is_cmd_pending = TRUE;
  session_object->cmd_control.is_destroy_cmd_pending = TRUE;

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "VSM_destroy_session_cmd_control(): Session portID = 0x%08X, current_action %d",
         session_object->apr_info.self_port, session_object->cmd_control.current_action );

  if ( VSM_ACTION_ENUM_UNINITIALIZED == session_object->cmd_control.current_action )
  {
    if ( ( session_object->apr_info.client_addr == apr_pkt->src_addr ) && 
         ( session_object->apr_info.client_port == apr_pkt->src_port ) )
    {
      is_full_control_client = TRUE;
    }

    if ( ( VSM_STATE_ENUM_RUN == session_object->state_control.state ) &&
         ( TRUE == is_full_control_client ) )
    {
      session_object->cmd_control.cmd_result = ADSP_EUNEXPECTED;
      session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
    }
    else
    {
      if ( 1 == session_object->ref_count )
      {
        /* There is only one client, go ahead and free all the resources. */
        session_object->cmd_control.current_action = VSM_ACTION_ENUM_CLOSE_MIXER_5;
      }
      else if ( TRUE == is_full_control_client )
      {
        /* FULL Control client destroying the session. some passive control clients still existing.
           Only reset the paceket exchaneg mode and clear mailbox configuration.
        */
        if ( VSS_IPKTEXG_MODE_IN_BAND != session_object->packet_exchange_info.mode )
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_RESET_PACKET_EXCHANGE_MODE_TO_INBAND_1;
        }
        else
        {
          /* No cleaning requried for OOB or mailbox packet exchange configuration.
             Just reduce the reference count and end command.
          */
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_COMPLETE;
        }
        /* Clearing the cached vocoder properties as the FULL control client
           has triggered the Destroy session cmd. */
        session_object->stream_param_cache.num_voc_params = 0;
        session_object->stream_param_cache.data_len = 0;
      }
      else
      {
        /* This is a Passive control client request. Just reduce the reference count and end command. */
        rc = VSM_utility_remove_passive_client_info( apr_pkt->src_addr, apr_pkt->src_port, session_object );
        if ( ADSP_FAILED( rc ) )
        {
          session_object->cmd_control.cmd_result = rc;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_COMPLETE;
        }
      }
    }
  }

  for ( ; ; )
  {
    switch ( session_object->cmd_control.current_action )
    {
      case VSM_ACTION_ENUM_RESET_PACKET_EXCHANGE_MODE_TO_INBAND_1:
      {
        rc = vsm_action_set_packet_exchange_mode( session_object, VSS_IPKTEXG_MODE_IN_BAND );

        if ( ADSP_FAILED( rc ) )
        {
          session_object->cmd_control.cmd_result = ADSP_EFAILED;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
          continue;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_WAIT_2;
          return ADSP_EOK;
        } 
      }
      break;

      case VSM_ACTION_ENUM_WAIT_2:
      {
        if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
        {
          session_object->cmd_control.cmd_result = session_object->cmd_control.resp_result;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
        }
        else
        {
          if ( TRUE == session_object->packet_exchange_info.mailbox_info.is_configured )
          {
            session_object->cmd_control.current_action = VSM_ACTION_ENUM_CLEAR_MAILBOX_CONFIGURATION_3;
          }
          else if ( TRUE == session_object->packet_exchange_info.oob_info.is_configured )
          {
            session_object->cmd_control.current_action = VSM_ACTION_ENUM_CLEAR_OOB_CONFIGURATION_4;
          }
          else
          {
            session_object->cmd_control.current_action = VSM_ACTION_ENUM_COMPLETE;
          }
        }
        continue;
      }
      break;

      case VSM_ACTION_ENUM_CLEAR_MAILBOX_CONFIGURATION_3:
      {
        vsm_action_clear_mailbox_configuration( session_object );
        session_object->cmd_control.current_action = VSM_ACTION_ENUM_COMPLETE;
        continue;
      }
      break;

      case VSM_ACTION_ENUM_CLEAR_OOB_CONFIGURATION_4:
      {
        vsm_action_clear_oob_configuration( session_object );
        session_object->cmd_control.current_action = VSM_ACTION_ENUM_COMPLETE;
        continue;
      }
      break;

      case VSM_ACTION_ENUM_CLOSE_MIXER_5:
      {
        rc = vsm_action_close_mixer_input_output( session_object );

        if ( ADSP_FAILED( rc ) )
        {
          session_object->cmd_control.cmd_result = ADSP_EFAILED;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
          continue;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_WAIT_6;
          return ADSP_EOK;
        }
      }
      break;

      case VSM_ACTION_ENUM_WAIT_6:
      {
        if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
        {
          session_object->cmd_control.cmd_result = session_object->cmd_control.resp_result;
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_ERROR_HANDLE;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_COMPLETE;
        }
          continue;
        }
      break;

      case VSM_ACTION_ENUM_ERROR_HANDLE:
      {
        if ( TRUE == session_object->cmd_control.is_resp_pending )
        {
          return ADSP_EOK;
        }
        else
        {
          session_object->cmd_control.current_action = VSM_ACTION_ENUM_REPORT_FAILURE;
          continue;
        }
      }
      break;

      case VSM_ACTION_ENUM_REPORT_FAILURE:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"VSM_destroy_session_cmd_control: command FAILED:." );
        elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, session_object->cmd_control.cmd_result );
        session_object->cmd_control.is_cmd_pending = FALSE;
        session_object->cmd_control.is_destroy_cmd_pending = FALSE;
        return ADSP_EOK;;
      }
      break;

      case VSM_ACTION_ENUM_COMPLETE:
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,"VSM_destroy_session_cmd_control: command Success:." );
        apr_pkt = ( elite_apr_packet_t* )session_object->cmd_control.cmd_msg.pPayload;
        if ( ( session_object->apr_info.client_addr == apr_pkt->src_addr ) && 
             ( session_object->apr_info.client_port == apr_pkt->src_port ) )
        {
          session_object->apr_info.client_addr = 0;
          session_object->apr_info.client_port = 0;
        }
        session_object->ref_count-- ;
        session_object->cmd_control.cmd_result = ADSP_EOK;
        session_object->cmd_control.is_cmd_pending = FALSE;
        session_object->cmd_control.is_destroy_cmd_pending = FALSE;
        if ( 0 == session_object->ref_count )
        {
          ( void )vsm_action_free_session_resources( session_object );
        }
        elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EOK );
        return ADSP_EOK;;
      }
      break;

      default:
        MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,"VSM_destroy_session_cmd_control: ERROR: Unexpected:." );
      break;
    }
  }
}

/************************ Elite MSG Handlers **********************
*******************************************************************/

ADSPResult VSM_session_cmdQ_aprpkt_handler( vsm_session_object_t* session_object )
{
  ADSPResult rc = ADSP_EOK;

  QURT_ELITE_ASSERT( session_object );

  elite_apr_packet_t *apr_pkt = ( elite_apr_packet_t * )( session_object->cmd_control.cmd_msg.pPayload );

  switch ( apr_pkt->opcode )
  {
    case VSS_ISTREAM_CMD_CREATE_FULL_CONTROL_SESSION:
    case VSS_ISTREAM_CMD_CREATE_PASSIVE_CONTROL_SESSION:
    {
      rc = VSM_create_session_cmd_control( session_object );
    }
    break;

    case APRV2_IBASIC_CMD_DESTROY_SESSION:
    {
      rc = VSM_destroy_session_cmd_control( session_object );
    }
    break;

    case VSS_ISTREAM_CMD_SET_MEDIA_TYPE:
    {
      rc = VSM_set_media_type_cmd_control( session_object );
    }
    break;

    case VSS_ISTREAM_CMD_DETACH_VOCPROC:
    {
      rc = VSM_attach_detach_vocproc_cmd_control( session_object, FALSE );
    }
    break;

    case VSS_ISTREAM_CMD_ATTACH_VOCPROC:
    {
      rc = VSM_attach_detach_vocproc_cmd_control( session_object, TRUE );
    }
    break;

    case VSS_ISTREAM_CMD_MVM_ATTACH:
    {
      rc = VSM_mvm_attach_cmd_control( session_object );
    }
    break;

    case VSS_ISTREAM_CMD_MVM_DETACH:
    {
      rc = VSM_mvm_detach_cmd_control( session_object );
    }
    break;

    case VSS_ISTREAM_CMD_ENCODER_RESET:
    {
      rc = VSM_stream_encoder_reset_cmd_control( session_object );
    }
    break;

    case VSS_ISTREAM_CMD_REINIT:
    {
      rc = VSM_stream_reinit_cmd_control( session_object );
    }
    break;

    case VSS_ICOMMON_CMD_SET_VAR_VOC_SAMPLING_RATE:
    {
      rc = VSM_set_var_voc_sampling_rate_cmd_control( session_object );
    }
    break;

    case VSS_ISTREAM_CMD_REGISTER_CALIBRATION_DATA_V2: /* BACKWARD COMPATIBILITY */
    {
      rc = VSM_stream_register_calibration_data_v2_cmd_control( session_object );
    }
    break;

    case VSS_ISTREAM_CMD_DEREGISTER_CALIBRATION_DATA: /* BACKWARD COMPATIBILITY */
    {
      rc = VSM_stream_deregister_calibration_data_cmd_ctrl( session_object );
    }
    break;

    case VSS_ISTREAM_CMD_REGISTER_STATIC_CALIBRATION_DATA:
    {
      rc = VSM_stream_register_static_calibration_data_cmd_ctrl( session_object );
    }
    break;

    case VSS_ISTREAM_CMD_DEREGISTER_STATIC_CALIBRATION_DATA:
    {
      rc = VSM_stream_deregister_static_calibration_data_cmd_ctrl( session_object );
    }
    break;

    case VSS_ITTY_CMD_SET_TTY_MODE:
    {
      rc =  VSM_set_tty_mode_cmd_control( session_object );
    }
    break;

    case VSS_ICOMMON_CMD_SET_UI_PROPERTY:
    {
      rc = VSM_set_ui_property_cmd_control( session_object );
    }
    break;

    case VSS_ICOMMON_CMD_GET_UI_PROPERTY:
    {
      rc = VSM_get_ui_property_cmd_control( session_object );
    }
    break;

    case VSS_ICOMMON_CMD_SET_UI_PROPERTY_V2:
    {
      rc = VSM_set_ui_property_v2_cmd_control( session_object );
    }
    break;

    case VSS_ICOMMON_CMD_GET_UI_PROPERTY_V2:
    {
      rc = VSM_get_ui_property_v2_cmd_control( session_object );
    }
    break;

    case VSS_IHDVOICE_CMD_SET_CONFIG:
    {
      rc = VSM_set_hdvoice_config_cmd_control ( session_object );
    }
    break;

    case VSS_ICOMMON_CMD_SET_SYSTEM_CONFIG:
    {
      rc = VSM_stream_set_system_config_cmd_control( session_object );
    }
    break;

    case VSS_ISTREAM_CMD_SET_VOICE_TIMING:
    {
      rc = VSM_set_voice_timing_cmd_control( session_object );
    }
    break;

    case VSS_ISTREAM_CMD_SET_VOCPROC_AVSYNC_DELAYS:
    {
      rc = VSM_set_vocproc_avsync_delays_cmd_control( session_object );
    }
    break;

    case VSS_ISTREAM_CMD_ENABLE:
    {
      rc = VSM_stream_enable_cmd_control( session_object );
    }
    break;

    case VSS_ISTREAM_CMD_DISABLE:
    {
      rc = VSM_stream_disable_cmd_control( session_object );
    }
    break;

    case VSS_ISSR_CMD_CLEANUP:
    {
      rc = VSM_session_ssr_cleanup_cmd_control( session_object );
    }
    break;

    case VSS_ISTREAM_CMD_VOC_QCELP13K_SET_RATE:
    case VSS_ISTREAM_CMD_VOC_4GVNB_SET_RATE:
    case VSS_ISTREAM_CMD_VOC_4GVWB_SET_RATE:
    case VSS_ISTREAM_CMD_VOC_4GVNW_SET_RATE:
    case VSS_ISTREAM_CMD_VOC_4GVNW2K_SET_RATE:
    case VSS_ISTREAM_CMD_VOC_AMR_SET_ENC_RATE:
    case VSS_ISTREAM_CMD_VOC_AMRWB_SET_ENC_RATE:
    {
      rc = VSM_stream_set_enc_rate_cmd_control( session_object );
    }
    break;

    case VSS_ISTREAM_CMD_CDMA_SET_ENC_MINMAX_RATE:
    {
      rc = VSM_stream_set_min_max_rate_cmd_control( session_object );
    }
    break;

    case VSS_ISTREAM_CMD_CDMA_SET_ENC_RATE_MODULATION:
    {
      rc = VSM_delegate_to_venc_cmd_processing( session_object, VSM_CMD_ENC_SET_RATE_MOD );
    }
    break;

    case VSS_ISTREAM_CMD_SET_ENC_DTX_MODE:
    {
      rc = VSM_set_enc_dtx_mode_control( session_object );
    }
    break;

    case VSS_ISTREAM_CMD_SET_EVS_VOC_ENC_OPERATING_MODE:
    {
      rc = VSM_set_evs_voc_operating_mode_control( session_object );
    }
    break;

    case VSS_ISTREAM_CMD_SET_EVS_ENC_CHANNEL_AWARE_MODE_ENABLE:
    {
      rc = VSM_set_evs_enc_channel_aware_mode_control( session_object, TRUE );
    }
    break;

    case VSS_ISTREAM_CMD_SET_EVS_ENC_CHANNEL_AWARE_MODE_DISABLE:
    {
      rc = VSM_set_evs_enc_channel_aware_mode_control( session_object, FALSE );
    }
    break;

    case VSS_IPKTEXG_CMD_SET_MODE:
    {
      rc = VSM_pktexg_set_mode_cmd_control( session_object );
    }
    break;

    case VSS_IPKTEXG_CMD_OOB_SET_CONFIG:
    {
      rc = VSM_pktexg_oob_set_config_cmd_control( session_object );
    }
    break;

    case VSS_IPKTEXG_CMD_MAILBOX_SET_CONFIG:
    {
      rc = VSM_set_mailbox_pktexg_config_cmd_control( session_object );
    }
    break;

    case VSS_IPKTEXG_CMD_MAILBOX_GET_VOC_PACKET_PROPERTIES:
    {
      rc = VSM_get_mailbox_pktexg_voc_packet_properties_cmd_control( session_object );
    }
    break;

    case VSS_ITTYOOB_CMD_REGISTER:
    {
      rc = VSM_ittyoob_cmd_register_cmd_control( session_object );
    }
    break;

    case VSS_ITTYOOB_CMD_DEREGISTER:
    {
      rc = VSM_ittyoob_cmd_deregister_cmd_control( session_object );
    }
    break;

    case VSS_IVPCM_CMD_START_V2:
    {
      rc = VSM_vpcm_start_v2_cmd_control( session_object );
    }
    break;

    case VSS_IVPCM_CMD_STOP:
    {
      rc = VSM_vpcm_stop_cmd_control( session_object );
    }
    break;

    case VSS_ICOMMON_CMD_SET_PARAM_V2:
    {
      rc = VSM_set_param_v2_cmd_control( session_object );
    }
    break;

    case VSS_ICOMMON_CMD_GET_PARAM_V2:
    {
      rc = VSM_get_param_v2_cmd_control( session_object );
    }
    break;

    case VSS_ICOMMON_CMD_SET_PARAM_V3:
    {
      rc = VSM_set_param_v3_cmd_control( session_object );
    }
    break;

    case VSS_ICOMMON_CMD_GET_PARAM_V3:
    {
      rc = VSM_get_param_v3_cmd_control( session_object );
    }
    break;

    case VSS_ISTREAM_CMD_SET_DTMF_GENERATION:
    {
      rc = VSM_set_dtmf_generation_cmd_control( session_object );
    }
    break;

    case VSS_ISTREAM_CMD_SET_RX_DTMF_DETECTION:
    {
      rc = VSM_set_rx_dtmf_detection_cmd_control( session_object );
    }
    break;

    case VSS_INOTIFY_CMD_LISTEN_FOR_EVENT_CLASS:
    {
      rc = VSM_listen_for_event_class_cmd_control( session_object );
    }
    break;

    case VSS_INOTIFY_CMD_CANCEL_EVENT_CLASS:
    {
      rc = VSM_cancel_event_class_cmd_control( session_object );
    }
    break;

    case VSS_IVOLUME_CMD_MUTE_V2:
    {
      rc = VSM_volume_mute_v2_cmd_control( session_object );
    }
    break;

    case VSS_ISTREAM_CMD_RESYNC_CTM:
    {
      rc = VSM_resync_ctm_cmd_control( session_object );
    }
    break;

    case VSS_ISTREAM_CMD_START_RECORD:
    case VSS_IRECORD_CMD_START:
    {
      rc = VSM_stream_start_record_cmd_control( session_object );
    }
    break;

    case VSS_IPLAYBACK_CMD_START:
    case VSS_ISTREAM_CMD_START_PLAYBACK:
    {
      /* Handles playback via pseudo port. */
      rc = VSM_stream_start_playback_cmd_control( session_object );
    }
    break;

    /* VSS_ISTREAM_CMD_STOP_PLAYBACK is deprecated and
     * VSS_IPLAYBACK_CMD_STOP is introduced. The two
     * commands have the same GUID. They behave the same
     * and share the same command handling function.
     */
    case VSS_IPLAYBACK_CMD_STOP:
    {
      rc = VSM_stream_stop_playback_cmd_control( session_object );
    }
    break;

    /* VSS_ISTREAM_CMD_STOP_RECORD is deprecated and
     * VSS_IRECORD_CMD_STOP is introduced. The two
     * commands have the same GUID. They behave the same
     * and share the same command handling function.
     */
    case VSS_IRECORD_CMD_STOP:
    {
      rc = VSM_stream_stop_record_cmd_control( session_object );
    }
    break;

    case VOICE_EVT_HOST_BUF_AVAILABLE_V2:
    {
      rc = VSM_vpcm_evt_notify_to_client_processing( session_object );
    }
    break;

    case VSS_IVPCM_EVT_PUSH_BUFFER_V2:
    {
      rc = VSM_vpcm_evt_push_buffer_to_enc_dec_processing( session_object );
    }
    break;

    case VSS_ISTREAM_CMD_SET_VOC_PARAM:
    {
      rc = VSM_set_voc_param_cmd_control( session_object );
    }
    break;

    default:
    {
      // send error message to client: this also frees the packet.
      rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EUNSUPPORTED );
    }
    break;

  }

  if ( ADSP_FAILED( rc ) )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
           "VSM_session_cmdQ_aprpkt_handler: Command failed opcode 0x%8X, port 0x%8X",
           apr_pkt->opcode, apr_pkt->dst_port );
  }

  return ADSP_EOK;
}

ADSPResult VSM_session_cmdQ_custom_msg_handler( vsm_session_object_t* session_object )
{
  /* All commands to VSM are now through APR packet. this handler is only a place holder for now.*/
  return ADSP_EOK;
}

ADSPResult VSM_session_cmdQ_unsupported_msg_handler( vsm_session_object_t* session_object )
{
   MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,"VSM_session_cmdQ_unsupported_msg_handler: unsupporte msg" );

   elite_msg_finish_msg( &session_object->cmd_control.cmd_msg, ADSP_EUNSUPPORTED );
   return ADSP_EFAILED;
}
