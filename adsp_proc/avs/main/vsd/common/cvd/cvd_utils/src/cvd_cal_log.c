/*
  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Header: //components/rel/avs.adsp/2.8.5/vsd/common/cvd/cvd_utils/src/cvd_cal_log.c#1 $
  $Author: pwbldsvc $
*/

#include <stdlib.h>
#include "stdio.h"
#include "msg.h"
#include "err.h"
#include "mmstd.h"

#include "apr_errcodes.h"
#include "apr_lock.h"
#include "apr_event.h"
#include "apr_thread.h"
#include "aprv2_api_inline.h"
#include "apr_misc.h"

#include "vss_ilog_private_if.h"
#include "cvd_cal_private_i.h"
#include "cvd_cal_common_i.h"
#include "cvd_cal_linear_search_i.h"
#include "cvd_mem_mapper_i.h"
#include "cvd_task.h"

/* ideally DIAG_MAX_TX_PKT_SIZ from diagpkt.h should be used 
   for limiting the max size of diag buf allocation request.
*/
//#include "diagpkt.h"

#if defined( USE_QURT_MEM_MAPPER )
  #include "qurt.h"
#endif

/****************************************************************************
 * GLOBALS                                                                  *
 ****************************************************************************/

/****************************************************************************
 * DEFINES                                                                  *
 ****************************************************************************/

#define CVD_CAL_PANIC_ON_ERROR( rc ) \
  { if ( rc ) { ERR_FATAL( "Error[%d]", rc, 0, 0 ); } }

#define CVD_CAL_LOG_COMM_ERROR( rc, dst_addr ) \
  { if ( ( rc ) && \
         ( APR_GET_FIELD( APRV2_PKT_DOMAIN_ID, dst_addr ) != \
           APR_GET_FIELD( APRV2_PKT_DOMAIN_ID, cvd_cal_log_my_addr ) ) ) \
    { MSG_2( MSG_SSID_DFLT, MSG_LEGACY_FATAL, "CVD CAL LOG APR comm error 0x%08X, \
                                    dst_addr = 0x%04X", rc, dst_addr ); } \
    else \
    { CVD_CAL_PANIC_ON_ERROR( rc ); } \
  }

/* The actual limit of the packet at diag is DIAG_MAX_TX_PKT_SIZ.
 * however the log_alloc is failing when there are consecutive calls 
 * with max size.
 *
 * Reduced packet size of 0x00000400 is relatively workign better.
*/
//#define CVD_CAL_LOG_MAX_PKT_SIZE ( DIAG_MAX_TX_PKT_SIZ )

#define CVD_CAL_LOG_MAX_PKT_SIZE ( 0x00000400 )

/* Threshold on system heap 150KB */
#define CVD_CAL_LOG_MAX_HEAP_SIZE ( 0x00025800 )

#define CVD_CAL_LOG_NUM_COMMANDS_V ( 60 )

/*****************************************************************************
 * DEFINITIONS                                                               *
 ****************************************************************************/

typedef enum cvd_cal_log_thread_state_enum_t
{
  CVD_CAL_LOG_THREAD_STATE_ENUM_INIT,
  CVD_CAL_LOG_THREAD_STATE_ENUM_READY,
  CVD_CAL_LOG_THREAD_STATE_ENUM_EXIT
}
  cvd_cal_log_thread_state_enum_t;

/****************************************************************************
 * WORK QUEUE DEFINITIONS                                                   *
 ****************************************************************************/

typedef struct cvd_cal_log_work_item_t cvd_cal_log_work_item_t;

struct cvd_cal_log_work_item_t
{
  apr_list_node_t link;
  aprv2_packet_t* packet;
};

/****************************************************************************
 * VARIABLE DECLARATIONS                                                    *
 ****************************************************************************/

/* Lock Management */
static apr_lock_t cvd_cal_log_thread_lock;
static apr_lock_t cvd_cal_log_int_lock;

/* APR Resource */
static uint32_t cvd_cal_log_apr_handle;
static uint16_t cvd_cal_log_my_addr;

/* Task Management */
static apr_event_t cvd_cal_log_task_event;
static apr_event_t cvd_cal_log_thread_event;
static apr_thread_t cvd_cal_log_task_handle;
static cvd_cal_log_thread_state_enum_t cvd_cal_log_task_state = CVD_CAL_LOG_THREAD_STATE_ENUM_INIT;
static uint8_t cvd_cal_logger_task_stack[ CVD_CAL_LOGGER_TASK_STACK_SIZE ];

/* Command Queue Management */
static cvd_cal_log_work_item_t cvd_cal_log_cmd_pool[ CVD_CAL_LOG_NUM_COMMANDS_V ];
static apr_list_t cvd_cal_log_free_cmd_q;
static apr_list_t cvd_cal_log_incoming_cmd_q;

static bool_t cvd_cal_logger_initialized = FALSE;
static uint32_t cvd_cal_log_heap_allocated = 0;

/****************************************************************************
 * COMMON INTERNAL ROUTINES                                                 *
 ****************************************************************************/

static void cvd_cal_log_int_lock_fn ( void )
{
  ( void ) apr_lock_enter( cvd_cal_log_int_lock );
}

static void cvd_cal_log_int_unlock_fn ( void )
{
  ( void ) apr_lock_leave( cvd_cal_log_int_lock );
}

static uint32_t cvd_cal_log_get_sequence_number( void )
{
  static uint32_t cvd_cal_log_sequence_number =0;

  return cvd_cal_log_sequence_number++;
}

static int32_t cvd_cal_log_validate_data_container_header(
  cvd_cal_log_commit_info_t* log_info
)
{
  int32_t rc = APR_EOK;
  uint32_t size = 0;

  if ( log_info->data_container_header == NULL )
  {
    return APR_EBADPARAM;
  }

  switch( log_info->data_container_id )
  {
  case CVD_CAL_LOG_DATA_CONTAINER_TABLE:
    size = sizeof( cvd_cal_log_table_header_t );
    break;

  case CVD_CAL_LOG_DATA_CONTAINER_SYSTEM_CONFIG:
    size = sizeof( cvd_cal_log_system_config_header_t );
    break;

  case CVD_CAL_LOG_DATA_CONTAINER_RAW_CAL_OUTPUT:
    size = sizeof( cvd_cal_log_cal_data_header_t );
    break;
	
  case CVD_CAL_LOG_DATA_CONTAINER_RAW_VP3:
    size = sizeof( cvd_cal_log_vp3_data_header_t );
    break;	

  default:
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED, "cvd_cal_log_validate_data_container_header():"
         " Invalid data_container ID: 0x08%x", ( log_info->data_container_id ) );
    rc = APR_EBADPARAM;
    break;
  }

  if ( size != log_info->data_container_header_size )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "cvd_cal_log_validate_data_container_header():"
           " Invalid data_container size %d for Container ID: 0x08%x",
           log_info->data_container_header_size, ( log_info->data_container_id ) );
    rc = APR_EBADPARAM;
  }

  return rc;
}

#ifdef SIM_DEFINED

void cvd_cal_log_sim_query_call_back (
  void* cal_param1,
  void* cb_data
)
{
  uint32_t size = 0;
  
  FILE* fp = ( FILE* )cb_data;

  if ( CVD_CAL_PARAM_MINOR_VERSION_0 == *(uint32_t*)cal_param1 )
  {
    cvd_cal_param_t* cal_param;
    cal_param = (cvd_cal_param_t*) cal_param1;
    size = sizeof(cvd_cal_param_t);
    fwrite( ( void* )(&(cal_param->module_id)), sizeof( uint8_t ), sizeof( uint32_t ),fp );
    fwrite( ( void* )(&(cal_param->param_id)), sizeof( uint8_t ), sizeof( uint32_t ),fp );
    fwrite( ( void* )cal_param->param_data, sizeof( uint8_t ), cal_param->param_data_size, fp );

    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED, "cvp_cal_log_query_cb_fn(): Number of bytes"
           " written to File: %d", ( cal_param->param_data_size + size ) );
  }
  else
  {
    cvd_cal_param_v1_t* cal_param;
    cal_param = (cvd_cal_param_v1_t*) cal_param1;
    size = sizeof(cvd_cal_param_v1_t);
    fwrite( ( void* )(&(cal_param->module_id)), sizeof( uint8_t ), sizeof( uint32_t ),fp );
    fwrite( ( void* )(&(cal_param->instance_id)), sizeof( uint8_t ), sizeof( uint16_t ),fp );
    fwrite( ( void* )(&(cal_param->reserved)), sizeof( uint8_t ), sizeof( uint16_t ),fp );
    fwrite( ( void* )(&(cal_param->param_id)), sizeof( uint8_t ), sizeof( uint32_t ),fp );
    fwrite( ( void* )cal_param->param_data, sizeof( uint8_t ), cal_param->param_data_size, fp );

    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED, "cvp_cal_log_query_cb_fn(): Number of bytes"
           " written to File: %d", ( cal_param->param_data_size + size ) );
  }
}

static int32_t cvd_cal_data_writetofile ( 
  uint32_t usecase_id,
  void* params
)
{
  FILE *fp = NULL;
  char filename[100];
  uint32_t ret_size = 0;
  cvd_cal_log_commit_info_t* log_info = ( cvd_cal_log_commit_info_t* )params;

  switch( usecase_id )
  {
  case CVD_CAL_LOG_STREAM_STATIC_TABLE:
    snprintf( filename, 100, "aDSPTest/TestOutput/stream_static_table.bin" );
    break;

  case CVD_CAL_LOG_VOCPROC_STATIC_TABLE:
    snprintf( filename, 100, "aDSPTest/TestOutput/vocproc_static_table.bin" );
    break;

   case CVD_CAL_LOG_VOCPROC_DYNAMIC_TABLE:
    snprintf( filename, 100, "aDSPTest/TestOutput/vocproc_dynamic_table.bin" );
    break;

  case CVD_CAL_LOG_DEVICE_CONFIG_TABLE:
    snprintf( filename, 100, "aDSPTest/TestOutput/device_config_table.bin" );
    break;

  case CVD_CAL_LOG_SYSTEM_CONFIG:
    snprintf( filename, 100, "aDSPTest/TestOutput/system_config_info.bin" );
    break;

  case CVD_CAL_LOG_SOUNDFOCUS_ACTIVITY_DATA:
    snprintf( filename, 100, "aDSPTest/TestOutput/sourcetrack_activity.bin" );
    break;

  case CVD_CAL_LOG_RAW_VP3_DATA_INPUT_TAP_POINT:
	  snprintf( filename, 100, "aDSPTest/TestOutput/vp3_data_input_log.bin" );
    break;

  case CVD_CAL_LOG_RAW_VP3_DATA_OUTPUT_TAP_POINT:
	  snprintf( filename, 100, "aDSPTest/TestOutput/vp3_data_output_log.bin" );
    break;	
	
  default:
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_data_writetofile():"
           " Unknown usecase ID 0x%08x", usecase_id );
    break;
  }

  fp = fopen( filename,"wb" );
  if ( fp == NULL )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_data_writetofile():"
           " Failed to open file to write for usecase_id 0x%16x", usecase_id );
    return APR_EFAILED;
  }

  ret_size = fwrite(( void* )log_info->payload_buf, sizeof( uint8_t ),
                    log_info->payload_size, fp );

  MSG_LOW("Total size of the ouput file is : %d\n", ret_size, 0, 0);

  fclose( fp );

  return APR_EOK;
}
#endif /* #ifdef SIM_DEFINED */

static int32_t cvd_cal_log_commit (
  log_code_type log_code,
  uint32_t payload_size,
  uint8_t* payload,
  cvd_cal_log_cal_container_t cal_container
)
{
#ifndef SIM_DEFINED 
  cvd_cal_log_voice_t* log_ptr = NULL;
  uint8_t* dst_data_ptr;

  if( ( payload == NULL ) || ( payload_size == 0 ) )
  {
    return APR_EBADPARAM;
  }

  log_ptr = ( ( cvd_cal_log_voice_t* )log_alloc( log_code,
                  payload_size + sizeof( cvd_cal_log_voice_t ) ) );

  if( log_ptr != NULL )
  {
    dst_data_ptr = ( ( uint8_t* )log_ptr ) + sizeof( cvd_cal_log_voice_t );
    log_ptr->cal_container = cal_container;

    ( void ) mmstd_memcpy( ( void* )dst_data_ptr, payload_size, 
                           ( void* ) payload, payload_size );

    log_commit( log_ptr );
  }
  else
  {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED, "cvd_cal_log_commit():"
            " log_alloc failed, payload size = %d", payload_size );
      return APR_EFAILED;
  }
#endif /* #ifndef SIM_DEFINED */
  return APR_EOK;
}

static int32_t cvd_cal_log_data_fragment (
  log_code_type log_code,
  uint32_t usecase_id,
  void* params
)
{

#ifndef SIM_DEFINED
  uint64_t timestamp = 0;
  cvd_cal_log_cal_container_t cal_container;
  uint32_t payload_remaining = 0;
  uint32_t fragment_size = 0;
  uint32_t payload_offset = 0;
  uint32_t fragment_offset = 0;
  uint8_t* payload_start = NULL;
  int32_t rc = APR_EOK;
  cvd_cal_log_async_log_t async_payload;
  uint8_t* log_buf = NULL;
  uint32_t log_header_size = 0;
  cvd_cal_log_voice_t* log_ptr = NULL;
  uint8_t* dst_data_ptr;
  cvd_cal_log_commit_info_t* log_info = ( cvd_cal_log_commit_info_t* )params;

  rc = cvd_cal_log_validate_data_container_header( log_info );

  if ( rc )
  {
    return rc;
  }

  if ( ( log_info->payload_buf == NULL ) || ( log_info->payload_size == 0 ) )
  {
    return APR_EBADPARAM;
  }

  /* System Tick is one cycle of the Hexagon Processor's 
   * 19.2MHz QTIMER Clock 
   *
   * qurt_sysclock_get_hw_ticks() gets the Hardware 
   * tick count. Returns the current value of 32 bit
   * Hardware Counter.
   *
   * Timestamp is logged in nano seconds.
   */
  #if defined( USE_QURT_MEM_MAPPER )
    timestamp = ( ( qurt_sysclock_get_hw_ticks() * 1000000000 ) / 
                  ( 19.2 * 1024 * 1024 ) );
  #endif

  /* Fill container info. */
  {
    cal_container.container_id = CVD_LOG_VOICE_CONTAINER_ID;
    cal_container.minor_version = 0;
  
    cal_container.segment_header.container_size = sizeof( cvd_cal_log_segment_header_t );
    cal_container.segment_header.container_id = CVD_LOG_SEGMENT_HEADER_CONTAINER_ID;
    cal_container.segment_header.minor_version = 0;
    cal_container.segment_header.total_size = 
      log_info->payload_size + ( log_info->data_container_header_size );
  
    cal_container.segment_header.sequence_num = cvd_cal_log_get_sequence_number();
  
    cal_container.aux_info.container_size = sizeof( cvd_cal_log_aux_info_header_t );
    cal_container.aux_info.container_id = CVD_LOG_AUX_INFO_CONTAINER_ID;
    cal_container.aux_info.minor_version = 0;
    cal_container.aux_info.instance = log_info->instance;
    cal_container.aux_info.call_num = log_info->call_num;
    cal_container.aux_info.timestamp = timestamp;
    cal_container.aux_info.usecase_id = usecase_id;
  
    cal_container.data.container_id = log_info->data_container_id;
    cal_container.data.minor_version = 0;
  }

  log_header_size = sizeof( cvd_cal_log_voice_t );
  payload_start = ( uint8_t* ) log_info->payload_buf;
  payload_remaining = ( log_info->payload_size ) + ( log_info->data_container_header_size );

  /* Fragment the payload if required */
  while ( payload_remaining != 0 )
  {
    if ( ( payload_remaining + log_header_size ) > CVD_CAL_LOG_MAX_PKT_SIZE )
    {
      fragment_size = CVD_CAL_LOG_MAX_PKT_SIZE - log_header_size;
    } else {
      fragment_size = payload_remaining;
    }

    log_ptr = ( cvd_cal_log_voice_t* )log_alloc(
                                        ( log_code_type ) log_code,
                                        ( fragment_size + log_header_size )
                                      );

    if( log_ptr != NULL )
    {
      dst_data_ptr = ( ( uint8_t* )log_ptr ) + sizeof( cvd_cal_log_voice_t );

      cal_container.cointainer_size = fragment_size + sizeof( cvd_cal_log_cal_container_t );
      cal_container.segment_header.offset = fragment_offset;
      cal_container.data.container_size = fragment_size + sizeof( cvd_cal_log_data_container_t );

      log_ptr->cal_container = cal_container;

      if ( fragment_offset == 0 )
      {
        /* Copy the data container header information to the diag buffer. */
        ( void ) mmstd_memcpy( ( void* )dst_data_ptr,
                               log_info->data_container_header_size, 
                               ( void* )( log_info->data_container_header ),
                               log_info->data_container_header_size
                             );

        dst_data_ptr += log_info->data_container_header_size;

        ( void ) mmstd_memcpy( ( void* )dst_data_ptr,
                               ( fragment_size - log_info->data_container_header_size ),
                               ( void* )payload_start,
                               ( fragment_size - log_info->data_container_header_size )
                             );
        payload_offset = ( fragment_size - log_info->data_container_header_size );

      }
      else 
      {
        ( void ) mmstd_memcpy( ( void* )dst_data_ptr, fragment_size, 
                               ( void* )( payload_start + payload_offset ), fragment_size );
        payload_offset += fragment_size;
      }
 
      log_commit( log_ptr );
    }
    else
    {
      if ( log_status( (log_code_type) log_code ) == FALSE )
      {
        return APR_EOK;
      }

      if ( ( cvd_cal_log_heap_allocated + payload_remaining ) >= 
             CVD_CAL_LOG_MAX_HEAP_SIZE )
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "cvd_cal_log_data_fragment(): Throttling error " );
        return APR_EFAILED;
      }

      cal_container.cointainer_size = payload_remaining + sizeof( cvd_cal_log_cal_container_t );
      cal_container.segment_header.offset = fragment_offset;
      cal_container.data.container_size = payload_remaining + sizeof( cvd_cal_log_data_container_t );

      async_payload.log_code = ( uint16_t ) log_code;
      async_payload.log_size = payload_remaining;
      async_payload.container = cal_container;

      log_buf = ( uint8_t* )malloc( payload_remaining );

      if ( log_buf == NULL)
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_log_data_fragment():"
             "memory allocation failure for asynchronous logging  %d", payload_remaining );
        return APR_EFAILED;
      }
	  apr_lock_enter(cvd_cal_log_int_lock);
      cvd_cal_log_heap_allocated += payload_remaining;
	  apr_lock_leave(cvd_cal_log_int_lock);

      if ( fragment_offset == 0 )
      {
        ( void ) mmstd_memcpy( ( void* )log_buf, log_info->data_container_header_size,
                               ( void* )( log_info->data_container_header ),
                               log_info->data_container_header_size );

        ( void ) mmstd_memcpy( ( void* )( log_buf + log_info->data_container_header_size ),
                               ( payload_remaining - log_info->data_container_header_size ),
                               ( void* )payload_start, 
                               ( payload_remaining - log_info->data_container_header_size )
                             );
      } 
      else 
      {
      ( void ) mmstd_memcpy( ( void* )log_buf, payload_remaining, 
                             ( void* )( payload_start + payload_offset ),
                             payload_remaining );
      }

      async_payload.log_payload = ( void* )log_buf;

      rc = __aprv2_cmd_alloc_send(
           cvd_cal_log_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
           cvd_cal_log_my_addr, APR_NULL_V,
           cvd_cal_log_my_addr, APR_NULL_V,
           APR_NULL_V, CVD_CAL_LOG_CMD_LOG_ASYNC,
           &async_payload, sizeof( cvd_cal_log_async_log_t ) );
      if ( rc )
      {
        free( log_buf );
		apr_lock_enter(cvd_cal_log_int_lock);
        cvd_cal_log_heap_allocated -= payload_remaining;
		apr_lock_leave(cvd_cal_log_int_lock);
      }

      return rc;
    }

    payload_remaining -= fragment_size;
    fragment_offset += fragment_size;

    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "cvd_cal_log_data_fragment():"
           "fragment_size = 0x%08X, payload_remaining = 0x%08X",
           fragment_size, payload_remaining );
  }
#else

  cvd_cal_data_writetofile ( usecase_id, params );

#endif /* #ifndef SIM_DEFINED */

  return APR_EOK;
}


void cvd_cal_log_query_call_back(
  void* cal_params1,
  void* client_data 
)
{
  static uint32_t table_handle = 0;
  static uint32_t cal_query_handle = 0;
  uint8_t* dst_data_ptr = NULL;
  uint32_t payload_remaining = 0;
  uint32_t fragment_size = 0;
  uint8_t* cal_data_start;
  uint32_t payload_offset = 0;
  uint32_t cal_data_offset = 0;
  uint32_t payload_size = 0;
  uint64_t timestamp = 0;
  cvd_cal_log_cal_container_t cal_container;
  cvd_cal_log_voice_t* log_ptr = NULL;
  cvd_cal_log_async_log_t async_payload;
  uint8_t* log_buf = NULL;
  int32_t rc = APR_EOK;
  uint32_t log_header_size = 0;
  uint16_t temp_offset = 0;
  uint32_t size = 0;
  cvd_cal_param_t* cal_param;
  cvd_cal_param_v1_t* cal_param_v1;
  cvd_cal_log_cal_data_header_t* cal_data_header;
  cvd_cal_log_commit_info_t* log_info = ( cvd_cal_log_commit_info_t* )( client_data );
  uint32_t minor_version = 0;
  void* cal_param_temp = NULL;
 

  rc = cvd_cal_log_validate_data_container_header( log_info );

  if ( rc )
  {
    return;
  }
  
  minor_version = *(uint32_t*)cal_params1;
  if ( CVD_CAL_PARAM_MINOR_VERSION_0 == minor_version )
  {
    cal_param = (cvd_cal_param_t*) cal_params1;
    size = sizeof(cvd_cal_param_t);
    payload_size = size + cal_param->param_data_size;
    cal_data_start = ( uint8_t* )cal_param->param_data;
    cal_param_temp = (void*)cal_param;
  }
  else
  {
    cal_param_v1 = (cvd_cal_param_v1_t*) cal_params1;
    size = sizeof(cvd_cal_param_v1_t);
    payload_size = size + cal_param_v1->param_data_size;
    cal_data_start = ( uint8_t* )cal_param_v1->param_data;
    cal_param_temp = (void*)cal_param_v1;
  }
  
  cal_data_header = ( cvd_cal_log_cal_data_header_t* )( log_info->data_container_header );

  /* System Tick is one cycle of the Hexagon Processor's 
   * 19.2MHz QTIMER Clock 
   *
   * qurt_sysclock_get_hw_ticks() gets the Hardware 
   * tick count. Returns the current value of 32 bit
   * Hardware Counter.
   *
   * Timestamp is logged in nano seconds.
   */
  #if defined( USE_QURT_MEM_MAPPER )
    timestamp = ( ( qurt_sysclock_get_hw_ticks() * 1000000000 ) / 
                  ( 19.2 * 1024 * 1024 ) );
  #endif

  if ( ( table_handle != cal_data_header->table_handle ) ||
       ( cal_query_handle != cal_data_header->cal_query_handle ) )
  {
    table_handle = cal_data_header->table_handle;
    cal_query_handle = cal_data_header->cal_query_handle;
    cal_data_header->data_seq_num = 0;
  }
  else
  {
    cal_data_header->data_seq_num++;
  }
  payload_remaining = payload_size + log_info->data_container_header_size;
  log_header_size = sizeof( cvd_cal_log_voice_t );

  /* Fill container info. */
  {
    cal_container.container_id = CVD_LOG_VOICE_CONTAINER_ID;
    cal_container.minor_version = 0;

    cal_container.segment_header.container_size = sizeof( cvd_cal_log_segment_header_t );
    cal_container.segment_header.container_id = CVD_LOG_SEGMENT_HEADER_CONTAINER_ID;
    cal_container.segment_header.minor_version = 0;
    cal_container.segment_header.total_size = payload_remaining;
    cal_container.segment_header.sequence_num = cvd_cal_log_get_sequence_number();

    cal_container.aux_info.container_size = sizeof( cvd_cal_log_aux_info_header_t );
    cal_container.aux_info.container_id = CVD_LOG_AUX_INFO_CONTAINER_ID;
    cal_container.aux_info.minor_version = 0;
    cal_container.aux_info.instance = log_info->instance;
    cal_container.aux_info.call_num = log_info->call_num;
    cal_container.aux_info.timestamp = timestamp;
    cal_container.aux_info.usecase_id = CVD_CAL_LOG_RAW_CAL_DATA_OUTPUT;

    cal_container.data.container_id = log_info->data_container_id;
    cal_container.data.minor_version = 0;
  }

  while ( payload_remaining != 0 )
  {
    if ( ( payload_remaining + log_header_size ) > CVD_CAL_LOG_MAX_PKT_SIZE )
    {
      fragment_size = CVD_CAL_LOG_MAX_PKT_SIZE - log_header_size;
    } else {
      fragment_size = payload_remaining;
    }

    log_ptr = ( cvd_cal_log_voice_t* )log_alloc( 
                                        ( log_code_type ) LOG_ADSP_CVD_CAL_DATA_C,
                                        fragment_size + log_header_size
                                      );

    if( log_ptr != NULL )
    {
      dst_data_ptr = ( ( uint8_t* )log_ptr ) + log_header_size;

      cal_container.cointainer_size = fragment_size + sizeof( cvd_cal_log_cal_container_t );
      cal_container.segment_header.offset = payload_offset;
      cal_container.data.container_size = fragment_size + sizeof( cvd_cal_log_data_container_t );

      log_ptr->cal_container = cal_container;

      if ( payload_offset == 0 )
      {
        /* Copy the data container header information to the diag buffer. */
        ( void ) mmstd_memcpy( ( void* )dst_data_ptr,
                               log_info->data_container_header_size, 
                               ( void* )( log_info->data_container_header ),
                               log_info->data_container_header_size
                             );
        temp_offset = log_info->data_container_header_size;

        ( void ) mmstd_memcpy( ( void* )( dst_data_ptr + temp_offset ),
                                  size, 
                                 ( void* )cal_param_temp, size );
        
        temp_offset += size;

        ( void ) mmstd_memcpy( ( void* )( dst_data_ptr + temp_offset ),
                               ( fragment_size - temp_offset ),
                               ( void* )cal_data_start, ( fragment_size - temp_offset )
                             );
        cal_data_offset = ( fragment_size - temp_offset);

      }
      else 
      {
        ( void ) mmstd_memcpy( ( void* )dst_data_ptr, fragment_size, 
                               ( void* )( cal_data_start + cal_data_offset ), fragment_size );
        cal_data_offset += fragment_size;
      }

      log_commit( log_ptr );
    }
    else
    {
      if ( log_status( (log_code_type) LOG_ADSP_CVD_CAL_DATA_C ) == FALSE )
      {
        return;
      }

      if ( ( cvd_cal_log_heap_allocated + payload_remaining ) >= 
             CVD_CAL_LOG_MAX_HEAP_SIZE )
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "cvd_cal_log_data_fragment(): Throttling error " );
        return;
      }
      cal_container.cointainer_size = payload_remaining + sizeof( cvd_cal_log_cal_container_t );
      cal_container.segment_header.offset = payload_offset;
      cal_container.data.container_size = payload_remaining +sizeof( cvd_cal_log_data_container_t );

      async_payload.log_code = ( uint16_t ) LOG_ADSP_CVD_CAL_DATA_C;
      async_payload.log_size = payload_remaining;
      async_payload.container = cal_container;

      log_buf = ( uint8_t* )malloc( payload_remaining );

      if ( log_buf == NULL)
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_log_query_call_back():"
             "memory allocation failure for asynchronous logging  %d", payload_remaining );
        return;
      }
	  apr_lock_enter(cvd_cal_log_int_lock);
      cvd_cal_log_heap_allocated += payload_remaining;
	  apr_lock_leave(cvd_cal_log_int_lock);
      if ( payload_offset == 0 )
      {
        /* Copy the data container header information to the diag buffer. */
        ( void ) mmstd_memcpy( ( void* )log_buf,
                               log_info->data_container_header_size, 
                               ( void* )( log_info->data_container_header ),
                               log_info->data_container_header_size
                             );
        temp_offset = log_info->data_container_header_size;

        ( void ) mmstd_memcpy( ( void* )( log_buf + temp_offset ),
                               size,
                               ( void* )cal_param_temp, size );
        temp_offset += size;

        ( void ) mmstd_memcpy( ( void* )( log_buf + temp_offset ),
                               ( payload_remaining - temp_offset ),
                               ( void* )cal_data_start, 
                               ( payload_remaining - temp_offset )
                             );
      } 
      else 
      {
        ( void ) mmstd_memcpy( ( void* )log_buf, payload_remaining, 
                               ( void* )( cal_data_start + cal_data_offset ), 
                               payload_remaining );
      }

      async_payload.log_payload = ( void* )log_buf;

      rc = __aprv2_cmd_alloc_send(
           cvd_cal_log_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
           cvd_cal_log_my_addr, APR_NULL_V,
           cvd_cal_log_my_addr, APR_NULL_V,
           APR_NULL_V, CVD_CAL_LOG_CMD_LOG_ASYNC,
           &async_payload, sizeof( cvd_cal_log_async_log_t ) );

      if ( rc )
      {
        free( log_buf );
		apr_lock_enter(cvd_cal_log_int_lock);
        cvd_cal_log_heap_allocated -= payload_remaining;
		apr_lock_leave(cvd_cal_log_int_lock);
      }

      return;
    }

    payload_remaining -= fragment_size;
    payload_offset += fragment_size;
  }

}

static int32_t cvd_cal_log_query_output( void* params, uint32_t usecase_id )
{

  int32_t rc = APR_EOK;
  cvd_cal_log_cal_data_header_t* cal_data_header;
  cvd_cal_log_commit_info_t* log_info = ( cvd_cal_log_commit_info_t* )params;


  rc = cvd_cal_log_validate_data_container_header( log_info );
  if ( rc )
  {
    return APR_EFAILED;
  }

  cal_data_header = ( cvd_cal_log_cal_data_header_t* )log_info->data_container_header;

#ifdef SIM_DEFINED
  static FILE* fp = NULL;
  char filename[100];

  switch ( usecase_id )
  {
  case  CVD_CAL_LOG_VOCPROC_STATIC_OUTPUT:
    snprintf( filename, 100, "aDSPTest/TestOutput/cal_data_cvp_static.bin" );
    break;

  case CVD_CAL_LOG_VOCPROC_DYNAMIC_OUTPUT:
    snprintf( filename, 100, "aDSPTest/TestOutput/cal_data_cvp_dynamic.bin" );
    break;  

  case 	CVD_CAL_LOG_STREAM_STATIC_OUTPUT:
    snprintf( filename, 100, "aDSPTest/TestOutput/cal_data_cvs_static.bin" );
    break;

  default:
    return APR_EOK;
  }	

  fp = fopen( filename,"wb" );
  if ( fp == NULL )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_data_writetofile():"
           " Failed to open file to write for usecase_id 0x%08x",
           CVD_CAL_LOG_RAW_CAL_DATA_OUTPUT );
    return APR_EFAILED;
  }

  ( void )cvd_cal_query ( cal_data_header->cal_query_handle, 
                          cvd_cal_log_sim_query_call_back, ( void* )fp );

  fclose( fp );
#else

  

  ( void )cvd_cal_query ( cal_data_header->cal_query_handle, 
                          cvd_cal_log_query_call_back, params );
#endif

  return APR_EOK;
}

APR_INTERNAL void cvd_cal_log_data (
  log_code_type log_code,
  uint32_t usecase_id,
  void* params,
  uint32_t size
)
{
  int32_t rc = APR_EOK;

  if ( ( params == NULL ) || ( size == 0 ) )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
         "cvd_cal_log_data(): NULL Parameters." );
    return;
  }

  switch ( usecase_id )
  {
    case CVD_CAL_LOG_VOCPROC_DYNAMIC_TABLE:
    case CVD_CAL_LOG_STREAM_STATIC_TABLE:
    case CVD_CAL_LOG_VOCPROC_STATIC_TABLE:
    case CVD_CAL_LOG_DEVICE_CONFIG_TABLE:
    case CVD_CAL_LOG_SYSTEM_CONFIG:
    case CVD_CAL_LOG_SOUNDFOCUS_ACTIVITY_DATA:
    case CVD_CAL_LOG_RAW_VP3_DATA_INPUT_TAP_POINT:
    case CVD_CAL_LOG_RAW_VP3_DATA_OUTPUT_TAP_POINT:	
      {
        rc = cvd_cal_log_data_fragment( log_code, usecase_id, params );
      }
      break;

    case CVD_CAL_LOG_STREAM_STATIC_OUTPUT:
    case CVD_CAL_LOG_VOCPROC_STATIC_OUTPUT:
    case CVD_CAL_LOG_VOCPROC_DYNAMIC_OUTPUT:
      {
        cvd_cal_log_query_output( params, usecase_id );
      }
      break;

    default:
      rc = APR_EUNSUPPORTED;
      break;
  }

  if ( rc )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "cvd_cal_log_data(): Failed with error=%d", rc );
  }

  return;
}

static int32_t cvd_cal_log_aync_cmd_processing (
  aprv2_packet_t* packet
)
{
  int32_t rc;
  cvd_cal_log_async_log_t* payload;
  uint32_t payload_size;
  uint32_t payload_remaining;
  uint32_t fragment_size;
  uint8_t* payload_start;
  uint32_t payload_offset = 0;
  uint32_t container_offset = 0;
  cvd_cal_log_cal_container_t cal_container;

  payload = APRV2_PKT_GET_PAYLOAD( cvd_cal_log_async_log_t, packet );
  payload_size = APRV2_PKT_GET_PAYLOAD_BYTE_SIZE( packet->header );

  if ( payload_size < sizeof( cvd_cal_log_async_log_t ) )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "cvd_cal_log_aync_cmd_processing():"
           "Unexpected payload size, %d < %d", payload_size, 
           sizeof( cvd_cal_log_async_log_t ) );
    rc = __aprv2_cmd_end_command( cvd_cal_log_apr_handle, packet, APR_EBADPARAM );
    CVD_CAL_LOG_COMM_ERROR( rc, packet->src_addr );
    return rc;
  }

  cal_container = payload->container;

  payload_remaining = payload->log_size;
  payload_start = ( uint8_t* )( payload->log_payload );
  container_offset = cal_container.segment_header.offset;

  /* Fragment the payload if required */
  while ( payload_remaining != 0 )
  {
    if ( ( payload_remaining + sizeof( cvd_cal_log_voice_t ) ) > CVD_CAL_LOG_MAX_PKT_SIZE )
    {
      fragment_size = CVD_CAL_LOG_MAX_PKT_SIZE - sizeof ( cvd_cal_log_voice_t );
    } else {
      fragment_size = payload_remaining;
    }

    cal_container.cointainer_size = ( uint16_t ) fragment_size + sizeof( cvd_cal_log_cal_container_t );
    cal_container.segment_header.offset = container_offset;
    cal_container.data.container_size = ( uint16_t ) fragment_size + sizeof( cvd_cal_log_data_container_t );

    rc = cvd_cal_log_commit ( payload->log_code, fragment_size, 
                                 ( payload_start + payload_offset ), cal_container );
 
    if ( rc )
    {
      ( void ) apr_misc_sleep( 3000000 ); /* Sleep for 3ms. */
      continue;
    }
    payload_remaining -= fragment_size;
    payload_offset += fragment_size;
    container_offset += fragment_size;

    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "cvd_cal_log_aync_cmd_processing():"
           "fragment_size = 0x%08X, payload_remaining = 0x%08X",
           fragment_size, payload_remaining );
  }

  free( payload->log_payload );
  apr_lock_enter(cvd_cal_log_int_lock);
  cvd_cal_log_heap_allocated -= payload->log_size;
  apr_lock_leave(cvd_cal_log_int_lock);

  return APR_EOK;
}

static void cvd_cal_log_thread_process_commands ( void )
{
  int32_t rc;
  cvd_cal_log_work_item_t* work_item;
  aprv2_packet_t* packet;

  while ( apr_list_remove_head( &cvd_cal_log_incoming_cmd_q,
                                ( ( apr_list_node_t** ) &work_item ) )
          == APR_EOK )
  {
    packet = ( ( aprv2_packet_t* ) work_item->packet );
    ( void ) apr_list_add_tail( &cvd_cal_log_free_cmd_q, &work_item->link );

    switch ( packet->opcode )
    {
    case CVD_CAL_LOG_CMD_LOG_ASYNC:
      ( void ) cvd_cal_log_aync_cmd_processing( packet );
      rc = __aprv2_cmd_free( cvd_cal_log_apr_handle, packet );
      CVD_CAL_LOG_COMM_ERROR( rc, packet->src_addr );
      break;

    default:
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "cvd_cal_log_thread_process_commands():"
           "Unsupported command 0x08%x", packet->opcode );
      rc = __aprv2_cmd_free( cvd_cal_log_apr_handle, packet );
      CVD_CAL_LOG_COMM_ERROR( rc, packet->src_addr );
      break;
    }
  }

}

static void cvd_cal_log_incoming_cmd_q_free_payload ( void )
{
  cvd_cal_log_work_item_t* work_item = NULL;
  aprv2_packet_t* packet = NULL;
  cvd_cal_log_async_log_t* payload = NULL;
  
  while( ( apr_list_remove_head( &cvd_cal_log_incoming_cmd_q,
        ( ( apr_list_node_t** ) &work_item ) )== APR_EOK ) && (cvd_cal_log_heap_allocated > 0))
  {
    packet = ( ( aprv2_packet_t* ) work_item->packet );
    payload = APRV2_PKT_GET_PAYLOAD( cvd_cal_log_async_log_t, packet );

    if ( NULL != payload->log_payload ) 
    {
      free( payload->log_payload );
      
      apr_lock_enter(cvd_cal_log_int_lock);
      cvd_cal_log_heap_allocated -= payload->log_size;
      apr_lock_leave(cvd_cal_log_int_lock);
      
    }
    else
    {
      MSG ( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
            "cvd_cal_log_incoming_cmd_q_free_payload(): "
            "cvd_cal_log_incoming_cmd_q Payload is NULL " );
    }
    
    ( void ) apr_list_add_tail( &cvd_cal_log_free_cmd_q, &work_item->link );
  }

}

static void cvd_cal_log_signal_run ( void )
{
#ifdef USE_SINGLE_AUDSVC_THREAD
  APR_INTERNAL void vocsvc_signal_task ( void );

  vocsvc_signal_task( );
#else
  apr_event_signal( cvd_cal_log_task_event );
#endif /* USE_SINGLE_AUDSVC_THREAD */
}

static int32_t cvd_cal_log_queue_incoming_packet (
  apr_list_t* incoming_cmd_q,
  aprv2_packet_t* packet
)
{
  int32_t rc = APR_EFAILED;
  cvd_cal_log_work_item_t* work_item;
  int do_once = 0;

  if ( incoming_cmd_q == NULL )
  {
    return APR_EBADPARAM;
  }

  if ( packet == NULL )
  { /* We should assert that the packet can't be NULL. */
    return APR_EBADPARAM;
  }

  do
  {
    { /* Get a free command structure. */
      rc = apr_list_remove_head( &cvd_cal_log_free_cmd_q,
                                 ( ( apr_list_node_t** ) &work_item ) );
      if ( rc )
      { /* No free command structure is available. */
        MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "cvd_cal_log_queue_incoming_packet(): Free command queue is full" );
        rc = APR_EBUSY;
        break;
      }
    }

    { /* Queue the new command to the incoming command queue and signal do
       * work.
       */
      work_item->packet = packet;
      ( void ) apr_list_add_tail( incoming_cmd_q, &work_item->link );
      cvd_cal_log_signal_run( );
    }

    return APR_EOK;
  }
   while ( do_once );

  return rc;
}

/* Dispatch the incoming packet in-context (assume in ISR). */
static int32_t cvd_cal_log_dispatch_packet (
  aprv2_packet_t* packet
)
{
  int32_t rc = APR_EOK;
  /* Any ISR context commands should be processed here.
   * Queue the rest of the commands to be processed in thread context. */
  rc = cvd_cal_log_queue_incoming_packet( &cvd_cal_log_incoming_cmd_q, packet );

  return rc;
}

static int32_t cvd_cal_log_isr_dispatch_fn (
  aprv2_packet_t* packet,
  void* dispatch_data
)
{
  int32_t rc = APR_EOK;

  rc = cvd_cal_log_dispatch_packet( packet );

  return rc;
}

static int32_t cvd_cal_logger_run ( void )
{
  if ( cvd_cal_logger_initialized )
  {
    cvd_cal_log_thread_process_commands( );
  }

  return APR_EOK;
}

static int32_t cvd_cal_log_task ( void* param )
{
  int32_t rc;

  rc = apr_event_create( &cvd_cal_log_task_event );
  CVD_CAL_PANIC_ON_ERROR( rc );

  cvd_cal_log_task_state = CVD_CAL_LOG_THREAD_STATE_ENUM_READY;
  apr_event_signal( cvd_cal_log_thread_event );

  do
  {
    rc = apr_event_wait( cvd_cal_log_task_event );
#ifndef USE_SINGLE_AUDSVC_THREAD
   ( void ) cvd_cal_logger_run( );
#endif /* !USE_SINGLE_AUDSVC_THREAD */
  }
  while ( rc == APR_EOK );

  rc = apr_event_destroy( cvd_cal_log_task_event );
  CVD_CAL_PANIC_ON_ERROR( rc );

  cvd_cal_log_task_state = CVD_CAL_LOG_THREAD_STATE_ENUM_EXIT;

  return APR_EOK;
}

int32_t cvd_cal_log_init ( void )
{
  int32_t rc;

  MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "====== cvd_cal_log_init()======" );

  { /* Initialize the locks. */
    ( void ) apr_lock_create( APR_LOCK_TYPE_MUTEX, &cvd_cal_log_thread_lock );
	( void ) apr_lock_create( APR_LOCK_TYPE_MUTEX, &cvd_cal_log_int_lock );
  }

  { /* Initialize the command queue management. */
    uint32_t i;

    { /* Populate the free command structures. */
      ( void ) apr_list_init_v2( &cvd_cal_log_free_cmd_q,
                                 cvd_cal_log_int_lock_fn, cvd_cal_log_int_unlock_fn );
      for ( i = 0; i < CVD_CAL_LOG_NUM_COMMANDS_V; ++i )
      {
        ( void ) apr_list_init_node( ( apr_list_node_t* ) &cvd_cal_log_cmd_pool[ i ] );
        ( void ) apr_list_add_tail( &cvd_cal_log_free_cmd_q,
                                    ( ( apr_list_node_t* ) &cvd_cal_log_cmd_pool[ i ] ) );
      }
    }
    ( void ) apr_list_init_v2( &cvd_cal_log_incoming_cmd_q,
                               cvd_cal_log_int_lock_fn, cvd_cal_log_int_unlock_fn );
  }
  { /* Create the CVD_CAL_LOGGER task. */
     rc = apr_event_create( &cvd_cal_log_thread_event );
     CVD_CAL_PANIC_ON_ERROR( rc );

    ( void ) apr_thread_create( &cvd_cal_log_task_handle, CVD_CAL_LOGGER_TASK_NAME,
                                CVD_CAL_LOGGER_TASK_PRIORITY, 
                                cvd_cal_logger_task_stack, CVD_CAL_LOGGER_TASK_STACK_SIZE, 
                                cvd_cal_log_task, NULL );

    /* Wait for service thread to be created. */
    ( void ) apr_event_wait( cvd_cal_log_thread_event );

    rc = apr_event_destroy( cvd_cal_log_thread_event );
    CVD_CAL_PANIC_ON_ERROR( rc );
  }

  { /* Initialize the APR resource. */
    rc = __aprv2_cmd_register2(
           &cvd_cal_log_apr_handle, "qcom.audio.cvd_cal_logger",
           sizeof( "qcom.audio.cvd_cal_logger" ), 0,
           cvd_cal_log_isr_dispatch_fn, NULL, &cvd_cal_log_my_addr );

    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
           "cvd_cal_log_init(): registered with APR rc=0x%08X",
                                           rc );
  }

  cvd_cal_logger_initialized = TRUE;
  return APR_EOK;
}

int32_t cvd_cal_log_deinit ( void )
{
  int32_t rc;

  MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "====== cvd_cal_log_deinit()======" );

  cvd_cal_logger_initialized = FALSE;

  { /* De Register the APR resource. */
    rc = __aprv2_cmd_deregister(cvd_cal_log_apr_handle);

    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
      "cvd_cal_log_deinit(): de-registered with APR rc=0x%08X", rc );
  }
  /* destroy the cvd_cal_log_task thread and cvd_cal_log_incoming_cmd_q*/

  cvd_cal_log_incoming_cmd_q_free_payload();
  apr_list_destroy ( &cvd_cal_log_incoming_cmd_q );
  {
    ( void ) apr_event_signal_abortall( cvd_cal_log_task_event );

    while (cvd_cal_log_task_state != CVD_CAL_LOG_THREAD_STATE_ENUM_EXIT)
    {
      ( void ) apr_misc_sleep( 1000000 );
    }
    ( void ) apr_thread_destroy( cvd_cal_log_task_handle );
  }

  /* destroy the cvd_cal_log_free_cmd_q */
  apr_list_clear(&cvd_cal_log_free_cmd_q);
  apr_list_destroy ( &cvd_cal_log_free_cmd_q );

  /* Release the locks. */
  { 
    ( void ) apr_lock_destroy(cvd_cal_log_int_lock );
    ( void ) apr_lock_destroy(cvd_cal_log_thread_lock );
  }

return rc;
}
