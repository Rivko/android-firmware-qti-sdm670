#ifndef MICRO_DIAGBUFFER_I_H
#define MICRO_DIAGBUFFER_I_H
/*============================================================================
  micro_diagbuffer_i.h

  micro diagbuffer internal header file

  Copyright (c) 2017 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
============================================================================*/

/* $Header: //components/rel/core.qdsp6/2.1/services/diag/micro_diagbuffer/src/micro_diagbuffer_i.h#2 $ */

/*==========================================================================
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
10/31/17   nk      Qshrink 2 support for uimage diag messages
06/09/17   sp     Created new file for micro diagbuffer macros, enums

===========================================================================*/
#include "qurt.h"       /* For qurt API's */
#include "micro_ULog.h"
#include "osal.h"

/**
 * Macros
 */
#if defined(__cplusplus)
#define MICRO_ULOG_INLINE inline
#else
#define MICRO_ULOG_INLINE __inline
#endif

#define MICRO_ULOG_TYPE_LOG_HEADER_VERSION   0x00001000

/* Log status flags */
#define MICRO_ULOG_STATUS_DEFINED      (0x01 << 0)
#define MICRO_ULOG_STATUS_ENABLED      (0x01 << 1)
#define MICRO_ULOG_STATUS_LOG_SW_READ_STARTED  (0x1 << 2)    // Set the first time the log is read
#define MICRO_ULOG_STATUS_LAST_MSG_WAS_OVERRUN (0x1 << 3)    // Set when an overrun occurs during reading

/* Log transports */
#define MICRO_ULOG_TRANSPORT_RAM  0
#define MICRO_ULOG_TRANSPORT_STM  1
#define MICRO_ULOG_TRANSPORT_ALT 2

/* Micro Ulog timestamps are 64-bit */
#define MICRO_ULOG_TIMESTAMP_SIZE  8

/* Feature flags */
#define MICRO_ULOG_FF1_64_BIT_TS 0x1
#define MICRO_ULOG_FF1_WLOCK     0x2

/* Length is the top 16 bits of the first word in a message */
#define MICRO_ULOG_LENGTH_MASK  0xFFFF0000
#define MICRO_ULOG_LENGTH_SHIFT 16

/* The minimum size of a micro Ulog buffer size */
#define MICRO_ULOG_MIN_BUF_SIZE 64

/* To speed writes, the log buffer is always powers of two in length.
 * bufSizeMask is bufSize-1, allowing us to simply mask it with the read and
 * write indices to yield the actual memory pointer.
 */
#define MICRO_ULOG_BUFSIZE_MASK(log) (log->bufSize-1)  

/* Include the header for QDSS API */
//#include "QDSSLite.h"

#define MICRO_ULOG_QDSS_SUCCESS   0
#define MICRO_ULOG_QDSS_FAIL     -1

/* Alloc the QDSS port. Return MICRO_ULOG_QDSS_SUCCESS/FAIL */
#define MICRO_ULOG_QDSS_ALLOC( x )  1/* QDSSAllocPort(x) */

/* Free the QDSS port, if necessary */
#define MICRO_ULOG_QDSS_FREE( x )   /* TBD */

/* QDSS Write routines */
#define MICRO_ULOG_QDSS_TS_WRITE( log, firstContent, firstCount )   /* QDSSWriteOSTPacket(log->stmPort, QDSSOSTHeader(TRACER_ENTITY_ULOG, log->protocol_num), firstContent, firstCount);*/
#define MICRO_ULOG_QDSS_NO_TS_WRITE( log, secondContent, secondCount )  /* QDSSWriteOSTPacket(log->stmPort, QDSSOSTHeader(TRACER_ENTITY_ULOG, log->protocol_num), secondContent, secondCount);*/

/**
 * Target-specific overrides
 */

#ifdef WINSIM

/* TODO: Need to update with appropriate PC API */
#define MICRO_ULOG_INIT_TIMESTAMP()
#define MICRO_ULOG_GETTIME64()                ( 0 )
#define MICRO_ULOG_TARGET_LOCK_INIT( x )      ( x = 1 )
#define MICRO_ULOG_LOCK_GET( x )              ( 0 )
#define MICRO_ULOG_LOCK_RELEASE( x )          ( 0 )

#else //defined(ADSP_8994) Working with Chris/Spencer to define a global flag for use in code. Hard coded for now
#include "uTimetick.h"
#include "CoreTime.h"

#define MICRO_ULOG_INIT_TIMESTAMP()
#define MICRO_ULOG_GETTIME64()                  (uint64)CoreTimetick_Get64() //(uint64)uTimetick_Get()  /*uTimetick_Get() only works in island mode.*/
#define MICRO_ULOG_TARGET_LOCK_INIT( x )        qurt_mutex_init( &x )
#define MICRO_ULOG_TARGET_LOCK_GET( x )         qurt_mutex_lock( &x )
#define MICRO_ULOG_TARGET_LOCK_RELEASE( x )     qurt_mutex_unlock( &x )

#endif 

#if 0// defined(FEATURE_REX) || defined(FEATURE_RIVAIMG) /* WCNSS */

static DalDeviceHandle * micro_ULogTimerHandle;

/**
 * <!-- micro_ULog_InitTimeStampHandle -->
 */
static void MICRO_ULOG_INLINE micro_ULog_InitTimeStampHandle( void )
{
  CORE_VERIFY( micro_ULogTimerHandle == NULL );
  DalTimetick_Attach( "SystemTimer", &micro_ULogTimerHandle );
}

/**
 * <!-- micro_ULog_GetDalsysTimeStamp -->
 */
static uint64 MICRO_ULOG_INLINE micro_ULog_GetDalsysTimeStamp( void )
{
  uint64 timestamp;
  CORE_VERIFY_PTR( micro_ULogTimerHandle );

  DalTimetick_GetTimetick64( micro_ULogTimerHandle, &timestamp );
  return timestamp;
}

#define MICRO_ULOG_GETTIME64()                 micro_ULog_GetDalsysTimeStamp()
#define MICRO_ULOG_INIT_TIMESTAMP()            micro_ULog_InitTimeStampHandle()
#define MICRO_ULOG_TARGET_LOCK_INIT( x )       DALSYS_SyncCreate( DALSYS_SYNC_ATTR_RESOURCE, (MICRO_ULOG_MUTEX_TYPE*)&x, NULL )
#define MICRO_ULOG_TARGET_LOCK_GET( x )        DALSYS_SyncEnter( x )
#define MICRO_ULOG_TARGET_LOCK_RELEASE( x )    DALSYS_SyncLeave( x )

#endif

#define MICRO_ULOG_LOCK_LOG( log ) \
do {\
  if ( log->feature_flags1 & MICRO_ULOG_FF1_WLOCK )\
  {\
    MICRO_ULOG_TARGET_LOCK_GET( log->lock );\
  }\
} while(0)

#define MICRO_ULOG_UNLOCK_LOG( log ) \
do {\
  if ( log->feature_flags1 & MICRO_ULOG_FF1_WLOCK )\
  {\
    MICRO_ULOG_TARGET_LOCK_RELEASE( log->lock );\
  }\
} while(0)

/**
 * Compiler or build env related overrides
 */
#if defined (WINSIM) || defined (WIN_DEBUGGER_EXTENSION)
#include "stdio.h"
#define MICRO_ULOG_STRLCPY(a, b, c) strcpy_s(a, c, b)

#elif defined (__GNUC__)
#include "stdio.h"
#include <stringl/stringl.h>
#define MICRO_ULOG_STRLCPY strlcpy

#else
#include <string.h>
#define MICRO_ULOG_STRLCPY strlcpy

#endif

/* Individual micro Ulog messages are limited to 128 bytes */
#define MICRO_ULOG_MAX_MSG_SIZE 128
#define MICRO_ULOG_MIN_MSG_SIZE 8

#define MIN_MSG_LEN 5

// Values for the outputFormat
#define MICROULOG_DIAG_FORMAT_ASCII    0
#define MICROULOG_DIAG_FORMAT_BINARY   1
#define MICROULOG_DIAG_FORMAT_ASCII_OVERRUN_DETECTED     2
#define MICROULOG_DIAG_FORMAT_BINARY_OVERRUN_DETECTED    3

#define MICRO_ULOG_TIMESTAMP_SIZE 8

/* -------------------------------------------------------------------------
** microUlog Task Signal Definitions
** ------------------------------------------------------------------------- */

#define DIAG_DRAIN_CLIENT_TASK_RPT_SIG           0x00000001    /* This signal is set when the
                                                                  dog timer expires */
#define DIAG_DRAIN_CLIENT_TASK_TIMEOUT_SIG       0x00000002
#define DIAG_DRAIN_CLIENT_TASK_SET_PARAM_DONE_SIG            0x00000004

/**
 * Enums
 */

/* Exception/error conditions are indicated by one/more of the below bits
   set in MICRO_ULOG_TYPE.usageData */
typedef enum
  {
  /* Not an error condition; indicates that the log has wrapped atleast once */
  MICRO_ULOG_LOG_WRAPPED = 0x0001,

  /* There was an attempt to write a message too large for the buffer.
     The message was dropped */
  MICRO_ULOG_ERR_LARGEMSG = 0x0002,

  MICRO_ULOG_ERR_LARGEMSGOUT = 0x0004, // A message too large for the output
                                       // buffer was dropped because it could
                                       // not fit.

  /* The log has been reset atleast once */
  MICRO_ULOG_ERR_RESET = 0x0008,

  /* Force enum to 32-bits */
  MICRO_ULOG_ERR_UINT32 = 0x7FFFFFFF
} MICRO_ULOG_STATUS_TYPE;

/* Data subtypes stored in log message headers */
typedef enum
{
  MICRO_ULOG_SUBTYPE_RESERVED_FOR_RAW = 0,
  MICRO_ULOG_SUBTYPE_REALTIME_PRINTF,
  MICRO_ULOG_SUBTYPE_REALTIME_BYTEDATA,
  MICRO_ULOG_SUBTYPE_REALTIME_STRINGDATA,
  MICRO_ULOG_SUBTYPE_REALTIME_WORDDATA,
  MICRO_ULOG_SUBTYPE_REALTIME_CSVDATA,
  MICRO_ULOG_SUBTYPE_REALTIME_VECTOR,
  MICRO_ULOG_SUBTYPE_REALTIME_MULTIPART,
  MICRO_ULOG_SUBTYPE_REALTIME_MULTIPART_STREAM_END,
  MICRO_ULOG_SUBTYPE_REALTIME_TOKENIZED_STRING,
  MICRO_ULOG_SUBTYPE_RESERVED1,
  MICRO_ULOG_SUBTYPE_RESERVED2,
  MICRO_ULOG_SUBTYPE_RESERVED3,
  MICRO_ULOG_SUBTYPE_RAW8,
  MICRO_ULOG_SUBTYPE_RAW16,
  MICRO_ULOG_SUBTYPE_RAW32,
  MICRO_ULOG_SUBTYPE_DIAG_MSG,
  MICRO_ULOG_SUBTYPE_DIAG_MSG_EXPANDED,  /* For non-qshrinked msgs on USER PD  */
  MICRO_ULOG_SUBTYPE_DIAG_MSG_QSR2,     /* For qshrinked msgs on both root and user PD */
  MICRO_ULOG_SUBTYPE_DIAG_LOG,
  MICRO_ULOG_SUBTYPE_DIAG_EVENT,
  MICRO_ULOG_SUBTYPE_DIAG_EVENT_PAYLOAD,

  /* Force enum to 32-bits */
  MICRO_ULOG_SUBTYPE_REALTIME_UINT32 = 0x7FFFFFFF
} MICRO_ULOG_REALTIME_SUBTYPES;

/* Linked list of all logs in the system */
struct
{
  MICRO_ULOG_MUTEX_TYPE gLock;
  MICRO_ULOG_TYPE *logHead;
} micro_ulogContext;

/**
 * <!-- micro_ULogInternal_UsesFastWrite -->
 *
 * @brief Check to see if fast writes can be used, or whether Vtable
 * writes must be used.
 */

boolean micro_ULogInternal_UsesFastWrite( MICRO_ULOG_TYPE *log );

micro_ULogResult
micro_ULogInternal_FastFmtSizeTS( MICRO_ULOG_TYPE *log,
                                  uint32 size, uint32 msg_format,
                                  uint32 *pStartIdx );

#endif /* MICRO_DIAGBUFFER_I_H */