
/*============================================================================
 
GENERAL DESCRIPTION

  Trace logging to static buffer.  

EXTERNALIZED FUNCTIONS
    
INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 1999-2000, 2007-2008, 2012, 2014 Qualcomm Technologies Incorporated.  All Rights Reserved.
Qualcomm Confidential and Proprietary
============================================================================*/

/*============================================================================

EDIT HISTORY FOR MODULE

$Header: //components/rel/core.qdsp6/2.1.c4/buses/uart/mdd/src/uart_log.c#1 $

when          who        what, where, why
-----------   --------   -----------------------------------------------------
07-Oct-2009   cmihalik   Initial revision.

============================================================================*/

#include "DDITimetick.h"
#include "DALSys.h"

#include "uart_log.h"

//-----------------------------------------------------------------------------
//  Prevent infinite recursion by undefining these.  Also removes logging 
//  that would be generated when accessing the trace log lock. 
//-----------------------------------------------------------------------------

#ifdef DALSYS_SyncCreate
#undef DALSYS_SyncCreate
#endif

#ifdef DALSYS_SyncEnter
#undef DALSYS_SyncEnter
#endif

#ifdef DALSYS_SyncLeave
#undef DALSYS_SyncLeave
#endif

//  Must be unique across all files.
#define  UART_TRACE_BUFFER_FILE_ID            5

//-----------------------------------------------------------------------------
//  Types.
//-----------------------------------------------------------------------------

#ifndef UART_NO_TRACE_BUFFER //{

#define BUF_LOG_MAGIC      0xfabdeed1
#define BUF_LOG_MAGIC_2    0xfabdeed2

typedef struct TraceLogMsg
{
  //  This is the fixed header data.  
  
  uint32                   magic;           //  magic number

  uint16                   fileId;          //  file id
  uint16                   lineNumber;      //  line number

  //  Timestamp is a 64 bit value.  Read / write it as 2 32 bit 
  //  values so the alignment restriction is 32 bits (instead of
  //  64 bits).  This leads to less padding and more data.

  uint32                   timestampLsw;    //  time stamp LSW
  uint32                   timestampMsw;    //  time stamp MSW

  uint8                    tsSeq;           //  sequence number for time stamp
  uint8                    numArgs;         //  number of arguments
  uint16                   numBytes;        //  number of bytes (buffer messages only)

  //  Variable size data is appended here.  The variable size
  //  data is 0-3 32 bit arguments followed by any number of bytes
  //  (up to 65K) for a BUFFER type message.  Note that the data 
  //  here must be padded out to a 32 bit boundary so that 
  //  the entire message is an integral number of 32 bits.
  //  Otherwise, the 32 bit data in array postion 1 won't
  //  be on a 32 bit boundary.  

} TraceLogMsg;

#define MESSAGE_HEADER_SIZE       ( 5 * sizeof( uint32 )) 

#define MAX_UART_LOG_ARGS         3

//-----------------------------------------------------------------------------
//  Globals
//-----------------------------------------------------------------------------

#define  MAX_TRACE_BUFFER_SIZE    (20 * 1024) 

#define  BUF_LOG_SENTINEL         0xdeadba11

typedef struct TraceLogInfo
{
  uint8*                   ptr;
  int32                    bytesLeft;
  uint8                    traceBuffer[MAX_TRACE_BUFFER_SIZE];
  uint32                   sentinel;
  int32                    maxBytes;

  uint64                   lastTimetick;
  uint32                   timetickSeq;

  DALSYSSyncHandle         bufferSync;
  DalDeviceHandle*         hTimetick;

} TraceLogInfo;

//  Should be static, but it's not so that it's visible in trace32.  

TraceLogInfo               
s_uartTraceLogInfo =
{
  0,                       // ptr
  0,                       // bytesLeft
  {0},                     // traceBuffer
  0,                       // sentinel
  0,                       // maxBytes

  0,                       // lastTimetick
  0,                       // timetickSeq

  0,                       // bufferSync
  0                        // hTimetick
};

//-----------------------------------------------------------------------------
//  buf_log_init_locking
//-----------------------------------------------------------------------------

static int
buf_log_init_locking( void )
{
  int initialized = TRUE;
  DALResult          dal_result;



  if( s_uartTraceLogInfo.bufferSync )
    return( TRUE );

  dal_result = DALSYS_SyncCreate( DALSYS_SYNC_ATTR_RESOURCE, 
                                  &s_uartTraceLogInfo.bufferSync, 
                                  NULL );

  initialized = ( DAL_SUCCESS == dal_result );

  return( initialized ); 
}

//-----------------------------------------------------------------------------
//  buf_log_lock
//-----------------------------------------------------------------------------

static void
buf_log_lock( void )
{
  DALSYS_SyncEnter( s_uartTraceLogInfo.bufferSync );
}

//-----------------------------------------------------------------------------
//  buf_log_unlock
//-----------------------------------------------------------------------------

static void
buf_log_unlock( void )
{
  DALSYS_SyncLeave( s_uartTraceLogInfo.bufferSync );
}

//-----------------------------------------------------------------------------
//  buf_log_init_trace_buffer
//-----------------------------------------------------------------------------

static void
buf_log_init_trace_buffer( void )
{
  DALResult       ret;



  if( s_uartTraceLogInfo.ptr != 0 )
    return;

  if( !buf_log_init_locking())
    return;

  if( s_uartTraceLogInfo.hTimetick == NULL )
  {
    ret = DalTimetick_Attach("SystemTimer", &s_uartTraceLogInfo.hTimetick );
    if( ret != DAL_SUCCESS )
      return;
  }

  s_uartTraceLogInfo.ptr       = s_uartTraceLogInfo.traceBuffer;
  s_uartTraceLogInfo.maxBytes  = MAX_TRACE_BUFFER_SIZE;
  s_uartTraceLogInfo.bytesLeft = MAX_TRACE_BUFFER_SIZE;
  s_uartTraceLogInfo.sentinel  = BUF_LOG_SENTINEL;
}

//-----------------------------------------------------------------------------
//  uart_buf_log_get_msg
//-----------------------------------------------------------------------------

static TraceLogMsg*
uart_buf_log_get_msg( int* requestSize )
{
  TraceLogInfo*        ti;
  TraceLogMsg*         msg;
  uint64               timestamp;
  uint32               timestampLsw;
  uint32               timestampMsw;
  DALResult            ret;
  int                  size;



  buf_log_init_trace_buffer();

  //  Round size up to a 4 byte boundary.  

  size = *requestSize;
  size = (size+3) & ~0x3;

  //  Lock logging data structures.

  buf_log_lock();

  //  Do we have enough space left?  If not, wrap around to
  //  the beginning of the buffer.

  ti = &s_uartTraceLogInfo;
  if( size > ti->bytesLeft )
  {
    ti->bytesLeft = ti->maxBytes;
    ti->ptr       = ti->traceBuffer;
  }

  //  Sanity check.  If message size is greater than
  //  the entire buffer size, something's wrong.

  if( size > ti->bytesLeft )
  {
     msg = NULL;
     *requestSize = 0;
     goto DONE;
  }

  //  Allocate the message.

  msg = (TraceLogMsg*) ti->ptr;

  ti->ptr += size;
  ti->bytesLeft -= size;

  //  Fill in header data.

  ret = DalTimetick_GetTimetick64( s_uartTraceLogInfo.hTimetick, &timestamp );
  if( ret != DAL_SUCCESS )
  {
    timestamp = 0;
  }

  if( timestamp != ti->lastTimetick )
  {
    ti->lastTimetick = timestamp;
    ti->timetickSeq  = 0;
  }

  timestampLsw = (uint32) timestamp;
  timestampMsw = (uint32) (timestamp >> 32);

  msg->magic        = BUF_LOG_MAGIC;
  msg->timestampLsw = timestampLsw;
  msg->timestampMsw = timestampMsw;
  msg->tsSeq        = ti->timetickSeq++;

DONE:

  buf_log_unlock();

  //  Return the size actually allocated.

  *requestSize = size;

  return( msg );
}

//-----------------------------------------------------------------------------
//  uart_buf_log_write_msg
//-----------------------------------------------------------------------------

static void
uart_buf_log_write_msg( uint16 fileId, uint16 lineNumber, int numArgs, uint32 args[] )
{
  TraceLogMsg*         msg;
  int                  i;
  int                  size;
  int                  actualSize;
  int                  numPadDWords;
  uint32*              pData;
  uint8*               ptr;



  size = MESSAGE_HEADER_SIZE;
  size += (numArgs * sizeof( uint32 ));

  actualSize = size;
  msg = uart_buf_log_get_msg( &actualSize ); 
  if( NULL == msg )
     return;

  msg->fileId     = fileId;
  msg->lineNumber = lineNumber;
  msg->numArgs    = numArgs;
  msg->numBytes   = 0;

  //  Point to start of variable data part of message.

  ptr = (uint8*) msg;
  ptr += MESSAGE_HEADER_SIZE;
  pData = (uint32*) ptr;

  //  Write args.

  for( i = 0; i < numArgs; i++ )
    *pData++ = *args++;

  //  There may be pad data in the message.
  //  If so, overwrite the pad data with 0xFF
  //  to make it easy to spot.
  
  numPadDWords = (actualSize - size) / sizeof( uint32 );
  for( i = 0; i < numPadDWords; i++ )
    *pData = 0xFFFFFFFF;
}

//-----------------------------------------------------------------------------
//  uart_buf_log_0
//-----------------------------------------------------------------------------

void    
uart_buf_log_0( uint16 fileId, uint16 lineNumber )
{
  uint32 args[MAX_UART_LOG_ARGS];

  uart_buf_log_write_msg( fileId, lineNumber, 0, args );
}

//-----------------------------------------------------------------------------
//  uart_buf_log_1
//-----------------------------------------------------------------------------

void    
uart_buf_log_1( uint16 fileId, uint16 lineNumber, uint32 a1 )
{
  uint32 args[MAX_UART_LOG_ARGS];

  args[0] = a1;
  uart_buf_log_write_msg( fileId, lineNumber, 1, args );
}

//-----------------------------------------------------------------------------
//  uart_buf_log_2
//-----------------------------------------------------------------------------

void    
uart_buf_log_2( uint16 fileId, uint16 lineNumber, uint32 a1, uint32 a2 )
{
  uint32 args[MAX_UART_LOG_ARGS];

  args[0] = a1;
  args[1] = a2;
  uart_buf_log_write_msg( fileId, lineNumber, 2, args );
}

//-----------------------------------------------------------------------------
//  uart_buf_log_3
//-----------------------------------------------------------------------------

void    
uart_buf_log_3( uint16 fileId, uint16 lineNumber, uint32 a1, uint32 a2, uint32 a3 )
{
  uint32 args[MAX_UART_LOG_ARGS];

  args[0] = a1;
  args[1] = a2;
  args[2] = a3;

  uart_buf_log_write_msg( fileId, lineNumber, 3, args );
}

//-----------------------------------------------------------------------------
//  uart_buf_log_buffer
//-----------------------------------------------------------------------------

static TraceLogMsg*
uart_buf_log_buffer( uint16 fileId, uint16 lineNumber, uint8* buffer, uint32 numBytes, uint32 numArgs, uint32 args[] )
{
  TraceLogMsg*         msg;
  uint32               i;
  int                  size;
  int                  actualSize;
  uint32*              dest32;
  uint8*               dest8;
  uint8*               ptr;
  int                  numPadBytes;



  size = MESSAGE_HEADER_SIZE;
  size += (numArgs * sizeof( uint32 ));
  size += numBytes;

  actualSize = size;
  msg = uart_buf_log_get_msg( &actualSize ); 
  if( NULL == msg )
     return( NULL );

  msg->fileId     = fileId;
  msg->lineNumber = lineNumber;
  msg->numArgs    = numArgs;
  msg->numBytes   = numBytes;

  //  Copy args.

  ptr = (uint8*) msg;
  ptr += MESSAGE_HEADER_SIZE;
  dest32 = (uint32*) ptr;
  for( i = 0; i < numArgs; i++ )
     *dest32++ = *args++;

  //  Copy data.

  dest8 = (uint8*) dest32;
  for( i = 0; i < numBytes; i++ )
    *dest8++ = *buffer++;

  //  Fill in the pad bytes with something easy to 
  //  recognize.

  numPadBytes = actualSize - size;
  for( i = 0; i < numPadBytes; i++ )
    *dest8++ = 0xFF;

  return( msg );
}

//-----------------------------------------------------------------------------
//  uart_buf_log_buffer_0
//-----------------------------------------------------------------------------

void    
uart_buf_log_buffer_0( uint16 fileId, uint16 lineNumber, uint8* buffer, uint32 numBytes )
{
  uint32 args[MAX_UART_LOG_ARGS];

  uart_buf_log_buffer( fileId, lineNumber, buffer, numBytes, 0, args );
}

//-----------------------------------------------------------------------------
//  uart_buf_log_buffer_1
//-----------------------------------------------------------------------------

void    
uart_buf_log_buffer_1( uint16 fileId, uint16 lineNumber, uint8* buffer, uint32 numBytes, uint32 a1 )
{
  uint32 args[MAX_UART_LOG_ARGS];

  args[0] = a1;
  uart_buf_log_buffer( fileId, lineNumber, buffer, numBytes, 1, args );
}

//-----------------------------------------------------------------------------
//  uart_buf_log_buffer_2
//-----------------------------------------------------------------------------

void    
uart_buf_log_buffer_2( uint16 fileId, uint16 lineNumber, uint8* buffer, uint32 numBytes, uint32 a1, uint32 a2 )
{
  uint32 args[MAX_UART_LOG_ARGS];

  args[0] = a1;
  args[1] = a2;
  uart_buf_log_buffer( fileId, lineNumber, buffer, numBytes, 2, args );
}

//-----------------------------------------------------------------------------
//  uart_buf_log_buffer_3
//-----------------------------------------------------------------------------

void    
uart_buf_log_buffer_3( uint16 fileId, uint16 lineNumber, uint8* buffer, uint32 numBytes, uint32 a1, uint32 a2, uint32 a3 )
{
  uint32 args[MAX_UART_LOG_ARGS];

  args[0] = a1;
  args[1] = a2;
  args[1] = a3;
  uart_buf_log_buffer( fileId, lineNumber, buffer, numBytes, 3, args );
}

//-----------------------------------------------------------------------------
//  uart_log_dal_msg
//-----------------------------------------------------------------------------

static void
uart_log_dal_msg( uint16 fileId, uint16 lineNumber, const char* dalMsg, uint32 numArgs, uint32 args[] )
{
  const char*          pos;
  uint32               len;
  TraceLogMsg*         msg;


  //  The DAL msg string is a format string.  We could examine
  //  the string and do sprintf like processing on it.  We'd also
  //  have to get the arguments.  We're not going to do that.

  pos = dalMsg;
  while( *pos++ );
  pos--;  // pos points one past NULL byte.
  len = pos - dalMsg;

  msg = uart_buf_log_buffer( fileId, lineNumber, (uint8*) dalMsg, len, numArgs, args );

  //  Use this to mark as a DAL msg.

  if( msg != NULL )
    msg->magic = BUF_LOG_MAGIC_2;
}

//-----------------------------------------------------------------------------
//  uart_log_dal_msg_0
//-----------------------------------------------------------------------------

void
uart_log_dal_msg_0( uint16 fileId, uint16 lineNumber, const char* dalMsg )
{
  uint32 args[MAX_UART_LOG_ARGS];

  uart_log_dal_msg( fileId, lineNumber, dalMsg, 0, args );
}

//-----------------------------------------------------------------------------
//  uart_log_dal_msg_1
//-----------------------------------------------------------------------------

void
uart_log_dal_msg_1( uint16 fileId, uint16 lineNumber, const char* dalMsg, uint32 a1 )
{
  uint32 args[MAX_UART_LOG_ARGS];

  args[0] = a1;
  uart_log_dal_msg( fileId, lineNumber, dalMsg, 1, args );
}

//-----------------------------------------------------------------------------
//  uart_log_dal_msg_2
//-----------------------------------------------------------------------------

void
uart_log_dal_msg_2( uint16 fileId, uint16 lineNumber, const char* dalMsg, uint32 a1, uint32 a2 )
{
  uint32 args[MAX_UART_LOG_ARGS];

  args[0] = a1;
  args[1] = a2;
  uart_log_dal_msg( fileId, lineNumber, dalMsg, 2, args );
}

//-----------------------------------------------------------------------------
//  uart_log_dal_msg_3
//-----------------------------------------------------------------------------

void
uart_log_dal_msg_3( uint16 fileId, uint16 lineNumber, const char* dalMsg, uint32 a1, uint32 a2, uint32 a3 )
{
  uint32 args[MAX_UART_LOG_ARGS];

  args[0] = a1;
  args[1] = a2;
  args[2] = a3;
  uart_log_dal_msg( fileId, lineNumber, dalMsg, 3, args );
}

//-----------------------------------------------------------------------------
//  For lock debugging
//-----------------------------------------------------------------------------

DALResult
uart_sync_create( uint32 dwAttribs, DALSYSSyncHandle *phSync, DALSYSSyncObj *pSyncObj )
{
  DALResult            result;


  result = DALSYS_SyncCreate( dwAttribs, phSync, pSyncObj );
  UART_LOG_2( TRACE, "uart_sync_create() = %d, handle = 0x%08x", result, (uint32) *phSync );

  return( result );
}

void
uart_sync_enter( DALSYSSyncHandle handle )
{
  UART_LOG_1( TRACE, "uart_sync_enter( 0x%08x )", (uint32) handle );
  DALSYS_SyncEnter( handle );
}

void
uart_sync_leave( DALSYSSyncHandle handle )
{
  DALSYS_SyncLeave( handle );
  UART_LOG_1( TRACE, "uart_sync_leave( 0x%08x )", (uint32) handle );
}

//-----------------------------------------------------------------------------
//  Unit test stuff
//-----------------------------------------------------------------------------

#ifdef UART_LOG_UNIT_TEST  //{

#include <stdio.h>
#include <ctype.h>

//-----------------------------------------------------------------------------
//  uart_log_buffer_dump_trace32
//-----------------------------------------------------------------------------

void
uart_log_buffer_dump_trace32( void )
{
   int         i;



   printf("   s_uartTraceLogInfo = (\n");
   printf("      ptr = 0x%X,\n", s_uartTraceLogInfo.ptr );
   printf("      bytesLeft = %d,\n", s_uartTraceLogInfo.bytesLeft );
   printf("      traceBuffer = (\n");

   for( i = 0; i < MAX_TRACE_BUFFER_SIZE; i++ )
   {
      printf("         0x%02x", s_uartTraceLogInfo.traceBuffer[i] );

      if( i < ( MAX_TRACE_BUFFER_SIZE - 1 ))
         printf(",");
      else
         printf("),");

      printf("\n");
   }

   printf("      ),\n");
   printf("      sentinel = %d,\n", s_uartTraceLogInfo.sentinel );
   printf("      maxBytes = %d,\n", s_uartTraceLogInfo.maxBytes );
   printf("      lastTimetick = %ld,\n", s_uartTraceLogInfo.lastTimetick );
   printf("      timetickSeq = %d,\n", s_uartTraceLogInfo.timetickSeq );
   printf("      bufferSync = 0x%X,\n", s_uartTraceLogInfo.bufferSync );
   printf("      hTimetick = 0x%X)\n", s_uartTraceLogInfo.hTimetick );
}

#endif //}
#endif //}

