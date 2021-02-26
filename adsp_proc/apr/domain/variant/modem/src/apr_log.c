/*
  Copyright (C) 2009-2013,2016,2018 QUALCOMM Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - QUALCOMM Technologies, Inc.

  $Header: //components/rel/apr.common/2.0/domain/variant/modem/src/apr_log.c#3 $
  $Author: pwbldsvc $
*/

#include <stdarg.h> /* va_list */
#include <stdio.h> /* vsnprintf() */
#include "err.h"
#include "msg.h"
#include "log.h"

#include "mmstd.h"

#include "apr_comdef.h"
#include "apr_errcodes.h"
#include "aprv2_packet.h"
#include "aprv2p1_packet.h"
#include "aprv2_msg_if.h"
#include "apr_lock.h"
#include "apr_misc.h"
#include "apr_dbg_msg.h"
#include "apr_log.h"
#include "aprv2_ids_domains.h"

/*****************************************************************************
 * Defines                                                                   *
 ****************************************************************************/

#define APR_ERROR_LOGGING
  /*
  * Send error messages into a separate log which can be analyzed 
  * from crash dump
  */

/* #define USE_SPIN_ON_PANIC */
  /* Hold the call from progressing on panic. A debugger can be attached and
   * the full thread context can be analyzed and manually resumed.
   */

#define APR_LOG_HISTORY_BYTE_SIZE_V ( 4 * 1024 )
  /* Memory is reduced from 8K to 4K to accommodate separate log history buffer 
   * between ADSP and modem.
   */  

#ifdef modem_proc
    #ifndef LOG_APR_MODEM_C
      #define LOG_APR_MODEM_C ( ( 0x4D0 ) + LOG_1X_BASE_C )
    #endif /* LOG_APR_MODEM_C */
    #define LOG_APRV2_CODE LOG_APR_MODEM_C

#endif

#ifdef adsp_proc
    #ifndef LOG_APR_ADSP_USERPD_C
      #define LOG_APR_ADSP_USERPD_C ((0x996) + LOG_1X_BASE_C)
    #endif /* LOG_APR_ADSP_USERPD_C */
	
	#ifndef LOG_APR_ADSP_C
      #define LOG_APR_ADSP_C ( ( 0x4D2 ) + LOG_1X_BASE_C )
    #endif /* LOG_APR_ADSP_C */
		
	#ifdef APR_USER_PD_FEATURE
	   #define LOG_APRV2_CODE LOG_APR_ADSP_USERPD_C
    #else
	   #define LOG_APRV2_CODE LOG_APR_ADSP_C
    #endif
	   
#endif


#ifdef cdsp_proc
    #ifndef LOG_APR_CDSP_C
      #define LOG_APR_CDSP_C ( ( 0x4D2 ) + LOG_1X_BASE_C )
    #endif /* LOG_APR_CDSP_C */
    #define LOG_APRV2_CODE LOG_APR_CDSP_C
#endif

#ifndef MSG_SSID_APR_MODEM
  #define MSG_SSID_APR_MODEM ( MSG_SSID_AVS )
  #define MSG_BUILD_MASK_MSG_SSID_APR_MODEM ( MSG_BUILD_MASK_MSG_SSID_AVS )
#endif /* MSG_SSID_APR_MODEM */

#ifdef APR_ERROR_LOGGING
  #define APR_LOG_MAX_FILENAME_SIZE_V ( 256 )
  #define APR_LOG_MAX_SHORT_FILENAME_SIZE_V ( 31 )
  #define APR_LOG_ERROR_HISTORY_ENTRIES_V ( 16 )
  #define APR_LOG_ERROR_HISTORY_ENTRY_SIZE_V ( 128 )
  #define APR_LOG_ERROR_HISTORY_BYTE_SIZE_V ( APR_LOG_ERROR_HISTORY_ENTRIES_V * \
                                              APR_LOG_ERROR_HISTORY_ENTRY_SIZE_V )
  #define APR_LOG_MAX_APR_USERS ( 24 )
  #define APR_LOG_NO_MEMORY_ERROR_LOG_SIZE ( APR_LOG_MAX_APR_USERS * 2 )
#endif /* APR_ERROR_LOGGING */

/*****************************************************************************
 * Definitions                                                               *
 ****************************************************************************/

typedef struct apr_log_history_t apr_log_history_t;
struct apr_log_history_t
{
  uint8_t start_marker[ 8 ];
  uint32_t size;
  uint32_t offset;
  uint8_t log[ APR_LOG_HISTORY_BYTE_SIZE_V ];
  uint8_t end_marker[ 8 ];
};

#ifdef APR_ERROR_LOGGING
typedef struct apr_log_error_history_t apr_log_error_history_t;
struct apr_log_error_history_t
{
  uint8_t start_marker[ 8 ];
  uint32_t idx;
  uint32_t total_units;
  uint32_t unit_size;
  uint32_t total_size;
  uint8_t log[ APR_LOG_ERROR_HISTORY_ENTRIES_V ][ APR_LOG_ERROR_HISTORY_ENTRY_SIZE_V ];
  uint32_t out_of_memory_log[ APR_LOG_NO_MEMORY_ERROR_LOG_SIZE ]; 
  bool_t is_first_out_of_memory_failure;
  uint8_t end_marker[ 8 ];
};
#endif /* APR_ERROR_LOGGING */

/*****************************************************************************
 * Variables                                                                 *
 ****************************************************************************/

static apr_lock_t apr_log_printf_mutex;
static char apr_log_printf_buffer[ 256 ];

static apr_lock_t apr_log_history_mutex;
static apr_log_history_t apr_log_history;
static apr_log_history_t apr_log_history_modem;

#ifdef APR_ERROR_LOGGING
static apr_log_error_history_t apr_log_error_history;
static char_t apr_log_scratch_buffer[ MMSTD_MAX( APR_LOG_ERROR_MAX_MSG_SIZE_V, APR_LOG_MAX_SHORT_FILENAME_SIZE_V ) ];

static apr_lock_t apr_log_error_mutex;
#endif /* APR_ERROR_LOGGING */

/*****************************************************************************
 * Routines                                                                  *
 ****************************************************************************/

APR_INTERNAL int32_t apr_log_init ( void )
{
  int32_t rc;
  uint32_t checkpoint = 0;

  for ( ;; )
  {
    rc = apr_lock_create( APR_LOCK_TYPE_INTERRUPT, &apr_log_printf_mutex );
    if ( rc ) break;
    checkpoint = 1;

    rc = apr_lock_create( APR_LOCK_TYPE_INTERRUPT, &apr_log_history_mutex );
    if ( rc ) break;
    checkpoint = 2;

	( void ) mmstd_memcpy( apr_log_history_modem.start_marker, sizeof( apr_log_history_modem.start_marker ), "APRMODM1", 8 );
    apr_log_history_modem.size = APR_LOG_HISTORY_BYTE_SIZE_V;
    ( void ) mmstd_memset( apr_log_history_modem.log, 0xFF, sizeof( apr_log_history_modem.log ) );
    ( void ) mmstd_memcpy( apr_log_history_modem.end_marker, sizeof( apr_log_history_modem.end_marker ), "APRMODM2", 8 );
	
    ( void ) mmstd_memcpy( apr_log_history.start_marker, sizeof( apr_log_history.start_marker ), "APRMODM1", 8 );
    apr_log_history.size = APR_LOG_HISTORY_BYTE_SIZE_V;
    ( void ) mmstd_memset( apr_log_history.log, 0xFF, sizeof( apr_log_history.log ) );
    ( void ) mmstd_memcpy( apr_log_history.end_marker, sizeof( apr_log_history.end_marker ), "APRMODM2", 8 );
    checkpoint = 3;

#ifdef APR_ERROR_LOGGING
    rc = apr_lock_create( APR_LOCK_TYPE_INTERRUPT, &apr_log_error_mutex );
    if ( rc ) break;
    checkpoint = 4;

    ( void ) mmstd_memcpy( apr_log_error_history.start_marker, sizeof( apr_log_error_history.start_marker ), "APRMODM3", 8 );
    apr_log_error_history.idx = 0;
    apr_log_error_history.total_units = APR_LOG_ERROR_HISTORY_ENTRIES_V;
    apr_log_error_history.unit_size = APR_LOG_ERROR_HISTORY_ENTRY_SIZE_V;
    apr_log_error_history.total_size = sizeof( apr_log_error_history );
    ( void ) mmstd_memset( apr_log_error_history.log, 0xFF, sizeof( apr_log_error_history.log ) );
    ( void ) mmstd_memset( apr_log_error_history.out_of_memory_log, 0x00, sizeof(apr_log_error_history.out_of_memory_log ) );
    apr_log_error_history.is_first_out_of_memory_failure = FALSE;
    ( void ) mmstd_memcpy( apr_log_error_history.end_marker, sizeof( apr_log_error_history.end_marker ), "APRMODM4", 8 );
#endif /* APR_ERROR_LOGGING */

    return APR_EOK;
  }

  switch ( checkpoint )
  {
  case 4:
    /*-fallthru */
  case 3:
    /*-fallthru */
  case 2:
    ( void ) apr_lock_destroy( apr_log_history_mutex );
    /*-fallthru */
  case 1:
    ( void ) apr_lock_destroy( apr_log_printf_mutex );
    /*-fallthru */
  default:
    break;
  }

  return APR_EFAILED;
}

APR_INTERNAL int32_t apr_log_deinit ( void )
{
  ( void ) apr_lock_destroy( apr_log_history_mutex );
  ( void ) apr_lock_destroy( apr_log_printf_mutex );

  return APR_EOK;
}

APR_INTERNAL void apr_log_printf ( const char * fmt, ... )
{
  va_list args;

  va_start( args, fmt );
  ( void ) apr_lock_enter( apr_log_printf_mutex );
  vsnprintf( apr_log_printf_buffer, sizeof( apr_log_printf_buffer ), fmt, args );
  MSG_SPRINTF_1( MSG_SSID_APR_MODEM, MSG_LVL_LOW, "%s", apr_log_printf_buffer );
  ( void ) apr_lock_leave( apr_log_printf_mutex );
  va_end( args );
}

APR_INTERNAL void apr_log_panic_printf ( const char * fmt, ... )
{
  va_list args;

  va_start( args, fmt );
  ( void ) apr_lock_enter( apr_log_printf_mutex );
  vsnprintf( apr_log_printf_buffer, sizeof( apr_log_printf_buffer ), fmt, args );
  MSG_SPRINTF_1( MSG_SSID_APR_MODEM, MSG_LVL_FATAL, "%s", apr_log_printf_buffer );
  ( void ) apr_lock_leave( apr_log_printf_mutex );
  va_end( args );

#ifdef USE_SPIN_ON_PANIC
  APR_DBG_0(APR_DBG_FATAL, "PANICKING");

  for ( ;; )
  {
    apr_misc_sleep( 5000000 );
  }
#else
  ERR_FATAL( "PANICKING", 0, 0, 0 );
#endif /* USE_SPIN_ON_PANIC */
}

static void apr_log_shorten_filename (
  char* full_name,
  char* short_name,
  uint32_t short_name_len
)
{
#ifdef APR_ERROR_LOGGING
  uint32_t index;
  uint32_t len;
  char_t character;

  len = mmstd_strnlen( full_name, APR_LOG_MAX_FILENAME_SIZE_V );

  /* Find the last '/' or '\' in the filename
   * in order to be able to remove the path.
   */
  for ( index = len; index > 0; --index )
  {
    character = full_name[ index - 1 ];
    if ( ( character == '/' ) ||
         ( character == '\\' ) )
    { /* Remove the slash. */
      break;
    }
  }

  if ( index == len )
  { /* Handle inproper input case where only path name exist, e.g. path:\ */
    index = 0;
  }

  ( void ) mmstd_strlcpy( short_name, &full_name[ index ], short_name_len );

  /* Ensure NULL termination. */
  if ( ( len - index ) < short_name_len )
  {
    short_name[ len - index ] = '\0';
  }
  else 
  {
    short_name[ short_name_len - 1 ] = '\0';
  }
#endif /* APR_ERROR_LOGGING */
}

static void apr_log_circ_write (
  uint8_t* const buf,
  uint32_t const buf_size,
  uint32_t* offset,
  const uint8_t* const data,
  uint32_t const data_size
)
{
  uint32_t endpos;
  uint32_t segment_size;
  uint32_t remaining_size;

  remaining_size = ( ( data_size > buf_size ) ? buf_size : data_size );
  endpos = ( *offset + remaining_size );
  segment_size = ( buf_size - *offset );

  /* Check whether buffer is full or not */
  if ( endpos < buf_size )
  {
    ( void ) mmstd_memcpy( &buf[ *offset ], segment_size, data, remaining_size );
    *offset = endpos;
  }
  else
  {
    if ( endpos == buf_size )
    {
      /* Reset offset to 0 if end position and buffer size are equals */
      ( void ) mmstd_memcpy( &buf[ *offset ], segment_size, data, remaining_size );
      *offset = 0;
    }
    else
    {
      /* Since current data cannot be fitted into the buffer, write only 
       * data which cab be fitted at he end of buffer and write rest of the data
       * from begining of the buffer
       */       
      ( void ) mmstd_memcpy( &buf[ *offset ], segment_size, data, segment_size );
      remaining_size -= segment_size;
      *offset = remaining_size;
      ( void ) mmstd_memcpy( buf, buf_size, &data[ segment_size ], remaining_size );
    }
  }
}

/* Assume packet is not NULL. */
APR_INTERNAL int32_t apr_log_pktv2_packet (
  uint32_t handle,
  aprv2_packet_t* packet
)
{
  uint8_t *log_ptr;
  uint32_t packet_size;
  uint32_t marker;

  uint16_t dst_domain_id;
  uint16_t src_domain_id;

  src_domain_id = APR_GET_FIELD( APRV2_PKT_DOMAIN_ID, packet->src_addr );
  dst_domain_id = APR_GET_FIELD( APRV2_PKT_DOMAIN_ID, packet->dst_addr );
  
  #ifdef APR_USER_PD_FEATURE
  
  uint32_t temp_packet_size;

  if( src_domain_id != dst_domain_id )
  {
    temp_packet_size = APRV2_PKT_GET_PACKET_BYTE_SIZE( packet->header );
    packet_size = APRV2_PKT_GET_HEADER_BYTE_SIZE( packet->header );
    /* Since we are logging only packet header here, we need to update hearder
     * for correct size to support parser tool
     */
    packet->header &= ~( APRV2_PKT_PACKET_SIZE_MASK );
    packet->header |= APR_SET_FIELD( APRV2_PKT_PACKET_SIZE, packet_size );
  }
  else
#endif /* APR_USER_PD_FEATURE */
  {
    packet_size = APRV2_PKT_GET_PACKET_BYTE_SIZE( packet->header );
  }

  /* Log APR packet to SOS buffer. */
  {
    ( void ) apr_lock_enter( apr_log_history_mutex );

    marker = 0xE91111E9;
	
    if( ( src_domain_id == APRV2_IDS_DOMAIN_ID_MODEM_V ) || 
        ( dst_domain_id == APRV2_IDS_DOMAIN_ID_MODEM_V ) )
    {
      apr_log_circ_write(
        apr_log_history_modem.log, apr_log_history_modem.size, &apr_log_history_modem.offset,
        ( ( uint8_t* ) &marker ), sizeof( marker ) );
        /* Write the sync word. */
      apr_log_circ_write(
        apr_log_history_modem.log, apr_log_history_modem.size, &apr_log_history_modem.offset,
        ( ( uint8_t* ) packet ), packet_size );
        /* Write the packet. */
      apr_log_history_modem.offset = ( ( ( apr_log_history_modem.offset + 3 ) >> 2 ) << 2 );
        /* Align to 32-bits. */
    }
    else
    {
      apr_log_circ_write(
        apr_log_history.log, apr_log_history.size, &apr_log_history.offset,
        ( ( uint8_t* ) &marker ), sizeof( marker ) );
        /* Write the sync word. */
      apr_log_circ_write(
        apr_log_history.log, apr_log_history.size, &apr_log_history.offset,
        ( ( uint8_t* ) packet ), packet_size );
        /* Write the packet. */
      apr_log_history.offset = ( ( ( apr_log_history.offset + 3 ) >> 2 ) << 2 );
        /* Align to 32-bits. */
    }
	
      ( void ) apr_lock_leave( apr_log_history_mutex );
  }

  /* Log APR packet to DIAG when enabled. */
  log_ptr = ( ( uint8_t* ) log_alloc( LOG_APRV2_CODE,
              ( sizeof( log_hdr_type ) + packet_size ) ) );
  if ( log_ptr != NULL )
  {
    ( void ) mmstd_memcpy( ( log_ptr + sizeof( log_hdr_type ) ), packet_size,
                           packet, packet_size );
    log_commit( log_ptr );
  }

#ifdef APR_USER_PD_FEATURE
  if( src_domain_id != dst_domain_id )
  {
    /* Revert actual packet size */
    packet->header &= ~( APRV2_PKT_PACKET_SIZE_MASK );
    packet->header |= APR_SET_FIELD( APRV2_PKT_PACKET_SIZE, temp_packet_size );
  }
#endif /* APR_USER_PD_FEATURE */

  return APR_EOK;
}

/* Assumes all pointers are not NULL and that all sizes are correct. */
static int32_t apr_log_write_segment (
  /* in/out */ void** dst_buffer,
  /* in/out */ uint32_t* dst_left,
  /* in */ void* src_buffer,
  /* in */ uint32_t src_size
)
{
  if ( src_size > *dst_left )
  {
    return APR_EFAILED;
  }

  ( void ) mmstd_memcpy( *dst_buffer, *dst_left, src_buffer, src_size );
  *( ( uint8_t** ) dst_buffer ) += src_size;
  *dst_left -= src_size;

  return APR_EOK;
}

/* Calling this function will log the current error to a circular buffer
** which can be recovered and parsed from a crash dump. The user can pass 
** the offending packet (if one exists) the line number and filename and 
** a short string (up to APR_LOG_ERROR_MAX_MSG_SIZE_V bytes). If the 
** provided string is longer than APR_LOG_ERROR_MAX_MSG_SIZE_V, only the 
** first APR_LOG_ERROR_MAX_MSG_SIZE_V bytes will be logged. 
*/
APR_INTERNAL int32_t apr_log_error (
  uint32_t handle, 
  aprv2_packet_t* packet, 
  uint32_t line_number,
  char* filename,
  char* msg, 
  int32_t param_1,
  int32_t param_2,
  int32_t param_3
)
{
#ifdef APR_ERROR_LOGGING
  int32_t rc;
  uint32_t checkpoint = 0;
  uint32_t log_entry_idx;
  uint8_t* buffer_ptr;
  uint32_t buffer_left;
  uint32_t write_size;

  /* Increment index to ensure safe access to error log buffer. */
  ( void ) apr_lock_enter( apr_log_error_mutex );
  log_entry_idx = apr_log_error_history.idx;
  apr_log_error_history.idx = ( apr_log_error_history.idx + 1 ) % apr_log_error_history.total_units;
  ( void ) apr_lock_leave( apr_log_error_mutex );

  /* Remove path from filename to reduce string size. */
  apr_log_shorten_filename( filename, apr_log_scratch_buffer, sizeof( apr_log_scratch_buffer ) );

  buffer_ptr = &apr_log_error_history.log[ log_entry_idx ][0];
  buffer_left = sizeof( apr_log_error_history.log[ log_entry_idx ] );

  for ( ;; )
  {
    checkpoint = 1;
    if ( packet == NULL )
    {
      break;
    }

    checkpoint = 2;
    write_size = sizeof( aprv2_packet_t );
    rc = apr_log_write_segment( ( void** ) &buffer_ptr, &buffer_left, packet, write_size );
    if ( rc ) break;

    checkpoint = 3;
    write_size = MMSTD_MIN( sizeof( apr_log_scratch_buffer ), APR_LOG_MAX_SHORT_FILENAME_SIZE_V );
    rc = apr_log_write_segment( ( void** ) &buffer_ptr, &buffer_left, apr_log_scratch_buffer, write_size );
    if ( rc ) break;

    checkpoint = 4;
    write_size = sizeof( line_number );
    rc = apr_log_write_segment( ( void** ) &buffer_ptr, &buffer_left, &line_number, write_size );
    if ( rc ) break;

    checkpoint = 5;
    /* Write out the formatted string up to the amount of space left. */
    ( void ) mmstd_snprintf( ( ( char_t* ) buffer_ptr ), buffer_left,
                             msg, param_1, param_2, param_3 );

    return APR_EOK;
  }

  MSG_ERROR( "Error checkpoint (%d)", checkpoint, 0, 0 );

  switch ( checkpoint )
  {
  default:
    /* Nothing to clean up. */
    break;
  }

#endif /* APR_ERROR_LOGGING */

  return APR_EFAILED;
}

APR_INTERNAL  void apr_log_out_of_memory_error ( 
  uint32_t* handles, 
  uint32_t* memq_items, 
  uint32_t handle_count 
)
{
#ifdef APR_ERROR_LOGGING
  uint32_t index;

  if ( apr_log_error_history.is_first_out_of_memory_failure == FALSE )
  {
    if ( handle_count > APR_LOG_MAX_APR_USERS )
    {
      handle_count = APR_LOG_MAX_APR_USERS;
    }
    for ( index = 0; index < handle_count; index += 2 )
    {
      apr_log_error_history.out_of_memory_log[ index ] = handles[ index ];
      apr_log_error_history.out_of_memory_log[ index + 1 ] = memq_items[ index ];
    }
    apr_log_error_history.is_first_out_of_memory_failure = TRUE;
  }
#endif /* APR_ERROR_LOGGING */

  return;
}


