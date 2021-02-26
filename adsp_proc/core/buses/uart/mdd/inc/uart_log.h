#ifndef UART_LOG_H
#define UART_LOG_H //{
/*============================================================================
 
GENERAL DESCRIPTION

  UART logging macros.  The two forms of the macro are:

    UART_LOG_1( level, format, a1 ); 

    UART_BUFFER_LOG_1( buffer, size, format, a1 )          

  The first form logs a message.  The second form logs a message followed
  by "size" number of characters from location "buffer".  The _1 suffix 
  refers to the number of arguments.  Up to 3 arguments are available.  

  UART_LOG( level, format ) is equivalent to UART_LOG_0( level, format )

  The format string is a standard printf style format string.  This is passed
  to the MSG macros so it has the same restrictions on the format string 
  that the MSG macros do.  (Whatever those restrictions may be.)

  The available log levels are:

    TRACE   -> logs to static buffer only
    LOW     -> MSG_LOW
    MEDIUM  -> MSG_MED
    HIGH    -> MSG_HIGH
    ERROR   -> ERR
    FATAL   -> ERR_FATAL

  The TRACE macros should be very effecient.

  The message is logged to the diag message system via the standard macros 
  as noted above.  If buffer logging is enabled, data is also written to the 
  log buffer that can be turned into a diag-like log via the post processing
  perl script uartlog.pl.  

EXTERNALIZED FUNCTIONS
    
INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 1999-2000, 2007-2008, 2012 Qualcomm Technologies Incorporated.  All Rights Reserved.
Qualcomm Confidential and Proprietary
============================================================================*/

/*============================================================================

EDIT HISTORY FOR MODULE

$Header: //components/rel/core.qdsp6/2.1/buses/uart/mdd/inc/uart_log.h#1 $

when          who        what, where, why
-----------   --------   -----------------------------------------------------
07-Oct-2009   cmihalik   Initial revision.

============================================================================*/

#include "target.h"
#include "comdef.h"
#include "msg.h"
#include "err.h"
#include "DALSys.h"

//----------------------------------------------------------------------------
//  Trace buffer logging functions.
//----------------------------------------------------------------------------

#ifndef UART_NO_TRACE_BUFFER //{

void      uart_buf_log_0( uint16 fileId, uint16 line );
void      uart_buf_log_1( uint16 fileId, uint16 line, uint32 a1 );
void      uart_buf_log_2( uint16 fileId, uint16 line, uint32 a1, uint32 a2 );
void      uart_buf_log_3( uint16 fileId, uint16 line, uint32 a1, uint32 a2, uint32 a3 );

void      uart_buf_log_buffer_0( uint16 fileId, uint16 line, uint8* buffer, uint32 size );
void      uart_buf_log_buffer_1( uint16 fileId, uint16 line, uint8* buffer, uint32 size, uint32 a1 );
void      uart_buf_log_buffer_2( uint16 fileId, uint16 line, uint8* buffer, uint32 size, uint32 a1, uint32 a2 );
void      uart_buf_log_buffer_3( uint16 fileId, uint16 line, uint8* buffer, uint32 size, uint32 a1, uint32 a2, uint32 a3 );

void      uart_log_dal_msg_0( uint16 fileId, uint16 line, const char* dalMsg );
void      uart_log_dal_msg_1( uint16 fileId, uint16 line, const char* dalMsg, uint32 a1 );
void      uart_log_dal_msg_2( uint16 fileId, uint16 line, const char* dalMsg, uint32 a1, uint32 a2 );
void      uart_log_dal_msg_3( uint16 fileId, uint16 line, const char* dalMsg, uint32 a1, uint32 a2, uint32 a3 );

//----------------------------------------------------------------------------
//  UART buf log macros.
//----------------------------------------------------------------------------

#define UART_BUFLOG_0( format )                                    uart_buf_log_0((uint16) UART_TRACE_BUFFER_FILE_ID, (uint16) __LINE__ )
#define UART_BUFLOG_1( format, a1 )                                uart_buf_log_1((uint16) UART_TRACE_BUFFER_FILE_ID, (uint16) __LINE__, a1 )
#define UART_BUFLOG_2( format, a1, a2 )                            uart_buf_log_2((uint16) UART_TRACE_BUFFER_FILE_ID, (uint16) __LINE__, a1, a2 )
#define UART_BUFLOG_3( format, a1, a2, a3 )                        uart_buf_log_3((uint16) UART_TRACE_BUFFER_FILE_ID, (uint16) __LINE__, a1, a2, a3 )

#define UART_BUFLOG_BUFFER_0( buffer, size, format )               uart_buf_log_buffer_0((uint16) UART_TRACE_BUFFER_FILE_ID, (uint16) __LINE__, buffer, size )
#define UART_BUFLOG_BUFFER_1( buffer, size, format, a1 )           uart_buf_log_buffer_1((uint16) UART_TRACE_BUFFER_FILE_ID, (uint16) __LINE__, buffer, size, a1 )
#define UART_BUFLOG_BUFFER_2( buffer, size, format, a1, a2 )       uart_buf_log_buffer_2((uint16) UART_TRACE_BUFFER_FILE_ID, (uint16) __LINE__, buffer, size, a1, a2 )
#define UART_BUFLOG_BUFFER_3( buffer, size, format, a1, a2, a3 )   uart_buf_log_buffer_3((uint16) UART_TRACE_BUFFER_FILE_ID, (uint16) __LINE__, buffer, size, a1, a2, a3 )

#define UART_LOG_DAL_MSG_0( dalMsg, format )                       uart_log_dal_msg_0((uint16) UART_TRACE_BUFFER_FILE_ID, (uint16) __LINE__, dalMsg )
#define UART_LOG_DAL_MSG_1( dalMsg, format, a1 )                   uart_log_dal_msg_1((uint16) UART_TRACE_BUFFER_FILE_ID, (uint16) __LINE__, dalMsg, a1 )
#define UART_LOG_DAL_MSG_2( dalMsg, format, a1, a2 )               uart_log_dal_msg_2((uint16) UART_TRACE_BUFFER_FILE_ID, (uint16) __LINE__, dalMsg, a1, a2 )
#define UART_LOG_DAL_MSG_3( dalMsg, format, a1, a2, a3 )           uart_log_dal_msg_3((uint16) UART_TRACE_BUFFER_FILE_ID, (uint16) __LINE__, dalMsg, a1, a2, a3 )

//----------------------------------------------------------------------------
//  Debug locking.
//----------------------------------------------------------------------------

#if 0 //{

DALResult      uart_sync_create( uint32 dwAttribs, DALSYSSyncHandle *phSync, DALSYSSyncObj *pSyncObj );
void           uart_sync_enter( DALSYSSyncHandle handle );
void           uart_sync_leave( DALSYSSyncHandle handle );

#define DALSYS_SyncCreate( dwAttribs, phSync, pSyncObj )     uart_sync_create( dwAttribs, phSync, pSyncObj )
#define DALSYS_SyncEnter( handle )                           uart_sync_enter( handle )
#define DALSYS_SyncLeave( handle )                           uart_sync_leave( handle )

#endif //}

#else //}{

#define UART_BUFLOG_0( format )                    
#define UART_BUFLOG_1( format, a1 )                   
#define UART_BUFLOG_2( format, a1, a2 )                  
#define UART_BUFLOG_3( format, a1, a2, a3 )                 

#define UART_BUFLOG_BUFFER_0( buffer, size, format )
#define UART_BUFLOG_BUFFER_1( buffer, size, format, a1 )
#define UART_BUFLOG_BUFFER_2( buffer, size, format, a1, a2 )
#define UART_BUFLOG_BUFFER_3( buffer, size, format, a1, a2, a3 )

#define UART_LOG_DAL_MSG_0( dalMsg, format )
#define UART_LOG_DAL_MSG_1( dalMsg, format, a1 )
#define UART_LOG_DAL_MSG_2( dalMsg, format, a1, a2 )
#define UART_LOG_DAL_MSG_3( dalMsg, format, a1, a2, a3 )

#endif //}

//----------------------------------------------------------------------------
//  Map the UART_MSG macros to the standard MSG_ macros.  Want to map 
//  UART_MSG_ERROR to ERR instead of MSG_ERROR and UART_MSG_FATAL to
//  ERR_FATAL instead of MSG_FATAL.  ERR_FATAL does more than MSG_FATAL.
//  Otherwise UART_MSG_XXX maps to MSG_XXX, except for UART_MSG_TRACE which
//  maps to null.
//----------------------------------------------------------------------------

#define UART_MSG_TRACE( format, a1, a2, a3 )                
#define UART_MSG_LOW( format, a1, a2, a3 )          MSG_LOW( format, a1, a2, a3 )
#define UART_MSG_MEDIUM( format, a1, a2, a3 )       MSG_MED( format, a1, a2, a3 )
#define UART_MSG_HIGH( format, a1, a2, a3 )         MSG_HIGH( format, a1, a2, a3 )
#define UART_MSG_ERROR( format, a1, a2, a3 )        ERR( format, a1, a2, a3 )
#define UART_MSG_FATAL( format, a1, a2, a3 )        ERR_FATAL( format, a1, a2, a3 )

//----------------------------------------------------------------------------
//  Use the standard MSG macros.  Log to both diag and the trace buffer.
//  TRACE level messages are logged only to the trace buffer.
//----------------------------------------------------------------------------

#define UART_LOG( level, format )           UART_LOG_0( level, format )
#define UART_LOG_0( level, format )                               do { UART_BUFLOG_0( format );             UART_MSG_##level( format,  0,  0,  0 ); } while(0)
#define UART_LOG_1( level, format, a1 )                           do { UART_BUFLOG_1( format, a1 );         UART_MSG_##level( format, a1,  0,  0 ); } while(0)
#define UART_LOG_2( level, format, a1, a2 )                       do { UART_BUFLOG_2( format, a1, a2 );     UART_MSG_##level( format, a1, a2,  0 ); } while(0)
#define UART_LOG_3( level, format, a1, a2, a3 )                   do { UART_BUFLOG_3( format, a1, a2, a3 ); UART_MSG_##level( format, a1, a2, a3 ); } while(0)

//  Debug level for the UART_BUFFER_LOG macros is always TRACE.

#define UART_BUFFER_LOG( buffer, size, format )                   UART_BUFFER_LOG_0( buffer, size, format )
#define UART_BUFFER_LOG_0( buffer, size, format )                 UART_BUFLOG_BUFFER_0( buffer, size, format )
#define UART_BUFFER_LOG_1( buffer, size, format, a1 )             UART_BUFLOG_BUFFER_1( buffer, size, format, a1 )
#define UART_BUFFER_LOG_2( buffer, size, format, a1, a2 )         UART_BUFLOG_BUFFER_2( buffer, size, format, a1, a2 )
#define UART_BUFFER_LOG_3( buffer, size, format, a1, a2, a3 )     UART_BUFLOG_BUFFER_3( buffer, size, format, a1, a2, a3 )

//  Debug level for the UART_DAL_MSG_LOG macros is always TRACE.

#define UART_DAL_MSG_LOG( dalMsg, format ) UART_DAL_MSG_LOG_0( dalMsg, format )
#define UART_DAL_MSG_LOG_0( dalMsg, format )                      UART_LOG_DAL_MSG_0( dalMsg, format )
#define UART_DAL_MSG_LOG_1( dalMsg, format, a1 )                  UART_LOG_DAL_MSG_1( dalMsg, format, a1 )
#define UART_DAL_MSG_LOG_2( dalMsg, format, a1, a2 )              UART_LOG_DAL_MSG_2( dalMsg, format, a1, a2 )
#define UART_DAL_MSG_LOG_3( dalMsg, format, a1, a2, a3 )          UART_LOG_DAL_MSG_3( dalMsg, format, a1, a2, a3 )

#ifdef UART_LOG_UNIT_TEST  //{

void    uart_log_buffer_dump_trace32( void );
void    uart_log_buffer_dump_trace32_2( void );

#endif //}

#endif //}

