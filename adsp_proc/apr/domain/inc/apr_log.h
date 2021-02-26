#ifndef __APR_LOG_H__
#define __APR_LOG_H__

/*
  Copyright (C) 2009-2010, 2012 QUALCOMM Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - QUALCOMM Technologies, Inc.

  $Header: //components/rel/apr.common/2.0/domain/inc/apr_log.h#1 $
  $Author: pwbldsvc $
*/

#include "apr_comdef.h"
#include "aprv2_packet.h"


#define APR_LOG_ERROR_MAX_MSG_SIZE_V ( 64 )

APR_INTERNAL int32_t apr_log_init ( void );
APR_INTERNAL int32_t apr_log_deinit ( void );

APR_INTERNAL void apr_log_printf ( const char * fmt, ... );
APR_INTERNAL void apr_log_panic_printf ( const char * fmt, ... );

APR_INTERNAL int32_t apr_log_pktv2_packet ( 
  uint32_t handle, 
  aprv2_packet_t* 
  packet 
);

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
);

APR_INTERNAL  void apr_log_out_of_memory_error ( 
  uint32_t* handles, 
  uint32_t* memq_items, 
  uint32_t handle_count 
);

#endif /* __APR_LOG_H__ */

