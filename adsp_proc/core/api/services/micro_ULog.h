/*============================================================================
  micro_ULog.h

micro_ULog API

  Copyright (c) 2016-2017 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
============================================================================*/
#ifndef MICRO_ULOG_H
#define MICRO_ULOG_H

#include <string.h>
#include <stdarg.h>
#include "com_dtypes.h"

/**
 * Target-specific type overrides
 */

#ifdef WINSIM
#define MICRO_ULOG_MUTEX_TYPE   void*

#elif defined( PROC_CNSS )
#include "DALSys.h"
#include "DDITimetick.h"
#define MICRO_ULOG_MUTEX_TYPE   DALSYSSyncHandle

#else
#include "qurt_mutex.h"
#define MICRO_ULOG_MUTEX_TYPE   qurt_mutex_t

#endif /* Target-specific type overrides */

/**
 * microUlog API
 */

typedef struct MICRO_ULOG_STRUCT *micro_ULogHandle;

/* Maximum length of a log name */
#define MICRO_ULOG_MAX_NAME_SIZE 23

/* Printf macros; Users are recommended to use these, rather than the
   RealTimePrintf API to future-proof their code against micro Ulog changes */
#define MICRO_ULOG_RT_PRINTF_0(h, formatStr) \
           micro_ULog_RealTimePrintf(h, 1, formatStr)
#define MICRO_ULOG_RT_PRINTF_1(h, formatStr, p1) \
           micro_ULog_RealTimePrintf(h, 2, formatStr, p1)
#define MICRO_ULOG_RT_PRINTF_2(h, formatStr, p1, p2) \
           micro_ULog_RealTimePrintf(h, 3, formatStr, p1, p2)
#define MICRO_ULOG_RT_PRINTF_3(h, formatStr, p1, p2, p3) \
           micro_ULog_RealTimePrintf(h, 4, formatStr, p1, p2, p3)
#define MICRO_ULOG_RT_PRINTF_4(h, formatStr, p1, p2, p3, p4) \
           micro_ULog_RealTimePrintf(h, 5, formatStr, p1, p2, p3, p4)
#define MICRO_ULOG_RT_PRINTF_5(h, formatStr, p1, p2, p3, p4, p5) \
           micro_ULog_RealTimePrintf(h, 6, formatStr, p1, p2, p3, p4, p5)

/* x MUST be a 4 byte float. It cannot be a literal or an 8 byte double */
#define MICRO_ULOG_RT_FLOAT(x) (*((uint32 *)((void *)&x)))

/* Macros for splitting up a 64 bit value */
#define MICRO_ULOG64_LOWWORD(x)  (uint32)x
#define MICRO_ULOG64_HIGHWORD(x) (uint32)((uint64)x>>32)
#define MICRO_ULOG64_DATA(x)     (uint32)x, (uint32)((uint64)x>>32)

/* Error codes from micro_Ulog routines */
typedef enum
{
  MICRO_ULOG_ERROR = -1,
  MICRO_ULOG_SUCCESS,
  MICRO_ULOG_ERR_BUFFER_TOO_SMALL,
  MICRO_ULOG_ERR_INVALIDNAME,
  MICRO_ULOG_ERR_ALREADYCREATED,
  MICRO_ULOG_ERR_ALREADYENABLED,
  MICRO_ULOG_ERR_INVALIDHANDLE,
  MICRO_ULOG_ERR_INITINCOMPLETE,
  MICRO_ULOG_ERR_INVALIDPARAMETER,
  MICRO_ULOG_ERR_INSUFFICIENT_BUFFER,   // Buffer provided to a read was insufficient
                                        // for even a single item to be copied into
  MICRO_ULOG_ERR_MISC,                  // Errors that don't fit the other categories
  MICRO_ULOG_ERR_OVERRUN,               // Overrun condition detected while reading
} micro_ULogResult;

/* Vtable that allows custom log write routines to be created and used.
   Use with micro_ULog_SetTransport */
typedef struct
{
  micro_ULogResult( *write )(micro_ULogHandle h,
                             uint32 msghdrSize, const char *msghdr,
                             uint32 msgdataSize, const char *msgdata);
  micro_ULogResult( *write_va )(micro_ULogHandle h,
                                uint32 msghdrSize, const char *msghdr,
                                uint32 dataCount, va_list data);
  int (*multipartMsgBegin)( micro_ULogHandle h );
  void (*multipartMsgEnd)( micro_ULogHandle h );
} MICRO_ULOG_CORE_VTABLE;

/* The actual MICRO_ULOG data type; needs to be in this header to allow use
   of the MICRO_ULOG_MEM_NEEDED macro */
typedef struct MICRO_ULOG_STRUCT
{
  struct MICRO_ULOG_STRUCT * next;  /* Link to the next log in the system */

  uint32 version; /* Version of microUlog in use */

  char name[MICRO_ULOG_MAX_NAME_SIZE+1]; /* Name of the log */
  uint32 logStatus;                      /* Log status flags */

  char *buffer;       /* Pointer to the log's message buffer */
  uint32 bufSize;      /* Size of the log's message buffer */

  /* Read index. Maintained by the reader. Points to the index where the 
   * next log message will be read from. Is used by the writer to determine
   * whether an overflow has occurred.
   */
  volatile uint32 read;

  /* Read status flags. The bits are defined in ULOG_READ_FLAGS */
  volatile uint32 readFlags;

  /* Write index. Maintained by the writer. Points to the index where the
     next log message will be written */
  volatile uint32 write;

  /* Trailing index. Maintained by the writer. Points to the first valid
   * message in the log. As messages are added to the log, messages become
   * invalid. This pointer tells the reader where it can start reading.
   */
  volatile uint32 readWriter;

  /* Maintained by the writer, tracks internal log error/exception conditions */
  uint32 usageData;

  /* One word broken up into 4 chars */
  unsigned char transport;      /* RAM, STM or ALT */
  unsigned char protocol_num;       /* STM protocol number 0-127 are used for binary logs, 128-255 are ASCII logs */
  unsigned char feature_flags1; /* Feature flags */
  unsigned char resetCount;     /* Used to count log resets; should be rare */

  uint32 stmPort;

  /* Size index of a multi-part message */
  uint32 multipartSizeIdx;

  MICRO_ULOG_CORE_VTABLE *transportVTable;

  MICRO_ULOG_MUTEX_TYPE lock;
} MICRO_ULOG_TYPE;

/* Returns x rounded to the next word boundary */
#define MICRO_ULOG_PADCOUNT(x) ((x + 3) & 0xFFFFFFFC)

/* Returns the amount of memory needed for a micro_Ulog with a message buffer
 * size of desiredBufSize bytes. desiredBufSize must be a power of 2 and greater
 * than 64
 */
#define MICRO_ULOG_MEM_NEEDED( desiredBufSize ) \
        (MICRO_ULOG_PADCOUNT(sizeof(MICRO_ULOG_TYPE)) + desiredBufSize)

/* Depreciated macro -- used by sleep  */
#define MICRO_ULOG_MEM_NEEDED_LOCK( x ) MICRO_ULOG_MEM_NEEDED( x )

/**
 * <!-- micro_ULog_CreateLog -->
 *
 * @brief Creates a new lockless microUlog in the given memory buffer.
 * Users are required to use the MICRO_ULOG_MEM_NEEDED macro to compute the
 * size of memory buffer to pass in, based on desired message buffer size.
 * 
 * @param hPtr : [OUT] Pass in the address of a micro_UlogHandle; will be set
 * to the newly created microUlog
 * @param logName : name of the log, must be < MICRO_ULOG_MAX_NAME_SIZE
 * @param mem : Pointer to the memory to create the microUlog in
 * @param pFinalMsgBufSize : [OUT] Returns the actual size of the microUlog's
 * message buffer
 * @param memSize : Size of memory pointed to by "mem"
 *
 * @return MICRO_ULOG_SUCCESS on success, MICRO_ULOG_ERR_BUFFER_TOO_SMALL if
 * the provided memory does not allow creation of a microUlog with a 64-byte
 * message buffer size, MICRO_ULOG_ERROR on other errors
 */
micro_ULogResult micro_ULog_CreateLog( micro_ULogHandle *hPtr,
                                               const char * logName,
                                               char * mem,
                                       uint32 *pFinalMsgBufSize,
                                               uint32 memSize );

/**
 * <!-- micro_ULog_CreateLockableLog -->
 *
 * @brief Creates a new lockable microUlog in the given memory buffer.
 * Users are required to use the MICRO_ULOG_MEM_NEEDED macro to compute the
 * size of memory buffer to pass in, based on desired message buffer size.
 *
 * @param hPtr : [OUT] Pass in the address of a micro_UlogHandle; will be set
 * to the newly created microUlog
 * @param logName : name of the log, must be < MICRO_ULOG_MAX_NAME_SIZE
 * @param mem : Pointer to the memory to create the microUlog in
 * @param pFinalMsgBufSize : [OUT] Returns the actual size of the microUlog's
 * message buffer
 * @param memSize : Size of memory pointed to by "mem"
 *
 * @return MICRO_ULOG_SUCCESS on success, MICRO_ULOG_ERR_BUFFER_TOO_SMALL if
 * the provided memory does not allow creation of a microUlog with a 64-byte
 * message buffer size, MICRO_ULOG_ERROR on other errors
 */
micro_ULogResult
micro_ULog_CreateLockableLog( micro_ULogHandle *hPtr,
                                       const char * logName,
                                       char * mem,
                              uint32 *pFinalMsgBufSize,
                                       uint32 memSize );

/**
 * <!-- micro_ULog_Enable -->
 *
 * @brief Enable the log. Logs are enabled on create. You only need to invoke
 * this routine if you've previously disabled the log.
 *
 * @param h : Handle to the microUlog
 *
 * @return MICRO_ULOG_SUCCESS on success, MICRO_ULOG_ERROR on error
 */
micro_ULogResult micro_ULog_Enable( micro_ULogHandle h );

/**
 * <!-- micro_ULog_SetTransportToRAM -->
 *
 * @brief Sets the transport of the given microUlog to RAM
 *
 * @param h : Handle to the microUlog
 *
 * @return MICRO_ULOG_SUCCESS on success, MICRO_ULOG_ERROR on error
 */
micro_ULogResult micro_ULog_SetTransportToRAM( micro_ULogHandle h );

/**
 * <!-- micro_ULog_SetTransportToSTM -->
 * 
 * @brief Sets the transport of the given microUlog to QDSS/STM
 *
 * @param h : Handle to the microUlog
 *
 * @return MICRO_ULOG_SUCCESS on success, MICRO_ULOG_ERROR on error
 */
micro_ULogResult micro_ULog_SetTransportToSTM( micro_ULogHandle h );

/**
 * <!-- micro_ULog_SetTransport -->
 * 
 * @brief Sets the transport of the microUlog to the given handler
 *
 * @param h : Handle to the microUlog
 * @param transportVTable : Pointer to a new vtable defining routines that
 * will handle writes for the transport
 *
 * @return MICRO_ULOG_SUCCESS on success, MICRO_ULOG_ERROR on error
 */
micro_ULogResult
micro_ULog_SetTransport( micro_ULogHandle h,
                         MICRO_ULOG_CORE_VTABLE *transportVTable );

/**
 * <!-- micro_ULog_RealTimePrintf -->
 *
 * @brief Writes printf data to the specified log
 * Example: micro_ULog_RealTimePrintf(handle, 3, "test %d: %s", 1, "complete");
 *
 * @param h : Handle to the microUlog
 * @param dataCount : Number of word-sized parameters in the given printf
 *
 * @return MICRO_ULOG_SUCCESS on success, MICRO_ULOG_ERROR on error
 */
micro_ULogResult
micro_ULog_RealTimePrintf( micro_ULogHandle h,
                           uint32 dataCount, const char *formatStr, ... );

/**
 * <!-- micro_ULog_RealTimeVprintf -->
 *
 * @brief Writes printf data to the specified log
 * Example: micro_ULog_RealTimePrintf(handle, 3, "test %d: %s", 1, "complete");
 *
 * @param h : Handle to the microUlog
 * @param dataCount : Number of word-sized parameters in the given va_list
 * @param ap : A variable list of arguments to the printf
 *
 * @return MICRO_ULOG_SUCCESS on success, MICRO_ULOG_ERROR on error
 */
micro_ULogResult
micro_ULog_RealTimeVprintf( micro_ULogHandle h, uint32 dataCount,
                            const char *formatStr, va_list ap );

/**
 * <!-- micro_ULog_RealTimeData -->
 * 
 * @brief Writes 32-bit data words to the specified log. 
 * 
 * @param h : Handle to the microUlog
 * @param dataCount : Number of word-sized parameters arguments
 *
 * @return MICRO_ULOG_SUCCESS on success, MICRO_ULOG_ERROR on error
 */
micro_ULogResult micro_ULog_RealTimeData( micro_ULogHandle h,
                                          uint32 dataCount, ... );

#ifdef FEATURE_FAST7
/**
 * <!-- micro_ULog_Fast7WordWrite -->
 *
 * @brief A special routines that writes 7 words to the specified log in a
 * format compatible with Raw Ulogs. This function works on a log that ONLY
 * contains Fast7 messages to optimize the way space is freed when the log
 * wraps. It CANNOT be mixed with other types of writes to the same log.
 * Doing so would corrupt messages in that log.
 *
 * @param h : Handle to the microUlog
 * @param message : Array of 7 words
 *
 * @return MICRO_ULOG_SUCCESS on success, MICRO_ULOG_ERROR on error
 */
micro_ULogResult micro_ULog_Fast7WordWrite( micro_ULogHandle h,
                                            uint32 message[7] );
#endif /* FEATURE_FAST7 */

#endif /* MICRO_ULOG_H */

