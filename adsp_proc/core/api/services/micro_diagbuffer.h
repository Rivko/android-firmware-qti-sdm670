/*============================================================================
@file Micro_DiagBuffer.h

Diag shim for micro ULog.

Copyright (c) 2015, 2017 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
============================================================================*/

/*===========================================================================
                              Edit History

 $Header: //components/rel/core.qdsp6/2.1/api/services/micro_diagbuffer.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/12/17   sp      Add support for microULog debug capability 
08/16/15   vk      Added support to drain from micro diagbuffer
07/21/15   xy      Fixed compilation warnings.  
06/12/15   is      Add support for Diag types
06/08/15   vk      Corrected typo, indentation etc.
04/22/15   vk      Created file.
===========================================================================*/

#ifndef MICRO_DIAGBUFFER_H
#define MICRO_DIAGBUFFER_H

#include "micro_ULog.h"

#include "msgcfg.h"
#include "micro_msg_diag_service.h"    /*for XX_MSG_CONST*/
#include "event.h"                     /*for event_id_enum_type*/
#include "log.h"                       /*for log_code_type*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Diag buffer Micro ULog Constants/Macros
 */

/**
 * Target-specific type overrides
 */

#ifdef WINSIM
#define MICRO_DIAGBUFFER_MUTEX_TYPE   void*

#elif defined( PROC_CNSS )
#include "DALSys.h"
#include "DDITimetick.h"
#define MICRO_DIAGBUFFER_MUTEX_TYPE   DALSYSSyncHandle

#else
#include "qurt_mutex.h"
#define MICRO_DIAGBUFFER_MUTEX_TYPE   qurt_mutex_t

#endif /* Target-specific type overrides */


/* shim for micro_ULog.h */
/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
 
/**
 * Macros
 */
#if defined(__cplusplus)
#define MICRO_DIAGBUFFER_INLINE inline
#else
#define MICRO_DIAGBUFFER_INLINE __inline
#endif

typedef struct MICRO_DIAGBUFFER_STRUCT *micro_diagbuffer_handle;

/* Maximum length of a log name */
#define MICRO_DIAGBUFFER_MAX_NAME_SIZE 23

/* To speed writes, the log buffer is always powers of two in length.
 * bufSizeMask is bufSize-1, allowing us to simply mask it with the read and
 * write indices to yield the actual memory pointer.
 */
#define MICRO_DIAGBUFFER_BUFSIZE_MASK(log) (log->bufSize-1)

/* x MUST be a 4 byte float. It cannot be a literal or an 8 byte double */
#define MICRO_DIAGBUFFER_RT_FLOAT(x) (*((uint32 *)((void *)&x)))

/* Macros for splitting up a 64 bit value */
#define MICRO_DIAGBUFFER64_LOWWORD(x)  (uint32)x
#define MICRO_DIAGBUFFER64_HIGHWORD(x) (uint32)((uint64)x>>32)
#define MICRO_DIAGBUFFER64_DATA(x)     (uint32)x, (uint32)((uint64)x>>32)

#define MICRO_DIAGBUFFER_GET_RESULT(x) (x == MICRO_DIAGBUFFER_SUCCESS)?TRUE:FALSE

/* Error codes from micro_diagbuffer routines
* These Error codes are return values from micro diagbuffer functions.
* micro diagbuffer functions additionally use the standard DAL return codes.
* MICRO_DIAGBUFFER_SUCCESS is the return value to test for success in most 
* operations.
* MICRO_DIAGBUFFER_ERROR and the values below are returned by functions.
*/
typedef enum
{
  MICRO_DIAGBUFFER_ERROR = -1,
  MICRO_DIAGBUFFER_SUCCESS,
  MICRO_DIAGBUFFER_ERR_BUFFER_TOO_SMALL,
  MICRO_DIAGBUFFER_ERR_INVALIDNAME,           // The name provided is invalid.
  MICRO_DIAGBUFFER_ERR_ALREADYCREATED,        // The log has already been created.
  MICRO_DIAGBUFFER_ERR_ALREADYENABLED,
  MICRO_DIAGBUFFER_ERR_INVALIDHANDLE,
  MICRO_DIAGBUFFER_ERR_INITINCOMPLETE,
  MICRO_DIAGBUFFER_ERR_INVALIDPARAMETER,
  MICRO_DIAGBUFFER_ERR_INSUFFICIENT_BUFFER,   // Buffer provided to a read was insufficient
                                        // for even a single item to be copied into
  MICRO_DIAGBUFFER_ERR_MISC,                  // Errors that don't fit the other categories
  MICRO_DIAGBUFFER_ERR_OVERRUN,               // Overrun condition detected while reading
} micro_diagbuffer_result;

/* Vtable that allows custom log write routines to be created and used.
   Use with micro_diagbuffer_set_transport */
typedef struct
{
  micro_diagbuffer_result( *write )(micro_diagbuffer_handle h,
                             uint32 msghdrSize, const char *msghdr,
                             uint32 msgdataSize, const char *msgdata);
  micro_diagbuffer_result( *write_va )(micro_diagbuffer_handle h,
                             uint32 msghdrSize, const char *msghdr,
                             uint32 dataCount, va_list data);
  int (*multipartMsgBegin)( micro_diagbuffer_handle h );
  void (*multipartMsgEnd) ( micro_diagbuffer_handle h );
} MICRO_DIAGBUFFER_CORE_VTABLE;

/* The actual MICRO_DIAGBUFFER data type; needs to be in this header to allow use
   of the MICRO_DIAGBUFFER_MEM_NEEDED macro */
typedef struct MICRO_DIAGBUFFER_STRUCT
{
  struct MICRO_DIAGBUFFER_STRUCT * next;       /* Link to the next log in the system */

  uint32 version;                              /* Version of micro_diagbuffer in use */

  char name[MICRO_DIAGBUFFER_MAX_NAME_SIZE+1]; /* Name of the log */
  uint32 logStatus;                            /* Log status flags */

  char *buffer;                                /* Pointer to the log's message buffer */
  uint32 bufSize;                              /* Size of the log's message buffer */

  /* Read index. Maintained by the reader. Points to the index where the 
   * next log message will be read from. Is used by the writer to determine
   * whether an overflow has occurred.
   */
  volatile uint32 read;

  /* Read status flags. The bits are defined in DIAGBUFFER_READ_FLAGS */ /* <<--- ToDo Vinay */
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
  unsigned char transport;          /* RAM, STM or ALT */  /* <<--- ToDo Vinay micro diagbuffer over STM? */
  unsigned char protocol_num;       /* STM protocol number 0-127 are used for binary logs, 128-255 are ASCII logs */ /* <<--- ToDo Vinay micro diagbuffer over STM? */
  unsigned char feature_flags1;     /* Feature flags */
  unsigned char resetCount;         /* Used to count log resets; should be rare */

  uint32 stmPort; /* <<--- ToDo Vinay micro diagbuffer over STM? */

  /* Size index of a multi-part message */
  uint32 multipartSizeIdx;   

  MICRO_DIAGBUFFER_CORE_VTABLE *transportVTable;

  MICRO_DIAGBUFFER_MUTEX_TYPE lock;
} MICRO_DIAGBUFFER_TYPE;

/* Returns x rounded to the next word boundary */
#define MICRO_DIAGBUFFER_PADCOUNT(x) ((x + 3) & 0xFFFFFFFC)

/* Returns the amount of memory needed for a micro_diagbuffer with a message buffer
 * size of desiredBufSize bytes. desiredBufSize must be a power of 2 and greater
 * than 64
 */
#define MICRO_DIAGBUFFER_MEM_NEEDED( desiredBufSize ) \
        (MICRO_DIAGBUFFER_PADCOUNT(sizeof(MICRO_DIAGBUFFER_TYPE)) + desiredBufSize)

/* Depreciated macro -- used by sleep  */ /* <<--- ToDo Vinay Do we keep it? */
#define MICRO_DIAGBUFFER_MEM_NEEDED_LOCK( x ) MICRO_DIAGBUFFER_MEM_NEEDED( x )

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
 * <!-- micro_diagbuffer_create -->
 *
 * @brief Creates a new lockless micro diagbuffer in the given memory buffer.
 * Users are required to use the MICRO_DIAGBUFFER_MEM_NEEDED macro to compute the
 * size of memory buffer to pass in, based on desired message buffer size.
 *
 * This API replaces micro_diagbuffer_CreateLockableLog/micro_diagbuffer_CreateLog of 
 * micro_diagbuffer
 * 
 * @param hPtr : [OUT] Pass in the address of a micro_diagbuffer_handle; will be set
 * to the newly created micro diagbuffer
 * @param logName : name of the log, must be < MICRO_DIAGBUFFER_MAX_NAME_SIZE
 * @param mem : Pointer to the memory to create the micro diagbuffer in
 * @param pFinalMsgBufSize : [OUT] Returns the actual size of the micro diagbuffer's
 * message buffer
 * @param memSize : Size of memory pointed to by "mem"
 * @param bufType : lockable or lockless micro diagbuffer, see MICRO_DIAGBUFFER_TYPE
 *
 * @return MICRO_DIAGBUFFER_SUCCESS on success, MICRO_DIAGBUFFER_ERR_BUFFER_TOO_SMALL if
 * the provided memory does not allow creation of a micro diagbuffer with a 64-byte
 * message buffer size, MICRO_DIAGBUFFER_ERROR on other errors
 */
typedef enum
{
   MICRO_DIAGBUFFER_LOCKABLE = 0,
   MICRO_DIAGBUFFER_LOCKLESS = 1
}MICRO_DIAGBUFFER_LOCK_TYPE;

#define micro_diagbuffer_create( hptr, log_name, mem, pfinal_msg_buf_size, mem_size, buf_type ) ({\
\
  micro_diagbuffer_result result = MICRO_DIAGBUFFER_SUCCESS; \
  \
  if (buf_type == MICRO_DIAGBUFFER_LOCKABLE) \
  {\
    result = (micro_diagbuffer_result)micro_ULog_CreateLockableLog( hptr, log_name, mem, pfinal_msg_buf_size, mem_size ); \
  }\
  else if (buf_type == MICRO_DIAGBUFFER_LOCKLESS)\
  {\
    result = (micro_diagbuffer_result)micro_ULog_CreateLog( hptr, log_name, mem, pfinal_msg_buf_size, mem_size ); \
  }\
  (result);\
})

/**
 * <!-- micro_diagbuffer_enable -->
 *
 * @brief Enable the micro diagbuffer. micro diagbuffers are enabled on create. 
 * You only need to invoke this routine if you've previously disabled the 
 * micro diagbuffer.
 *
 * This API replaces micro_ULog_Enable() of microULog.
 *
 * @param h : Handle to the micro diagbuffer
 *
 * @return MICRO_DIAGBUFFER_SUCCESS : if successful
 *         MICRO_DIAGBUFFER_ERROR   : otherwise 
 */
#define micro_diagbuffer_enable(h) \
              micro_ULog_Enable(h)

/**
 * <!-- micro_diagbuffer_set_transport_to_ram -->
 *
 * @brief Sets the transport of the given micro diagbuffer to 
 *        RAM
 * This API replaces micro_ULog_SetTransportToRAM of microULog.
 *
 * @param h : Handle to the micro diagbuffer
 *
 * @return MICRO_DIAGBUFFER_SUCCESS on success, 
 *         MICRO_DIAGBUFFER_ERROR   on error
 */
#define micro_diagbuffer_set_transport_to_ram(h) \
              micro_ULog_SetTransportToRAM(h)

/**
 * <!-- micro_diagbuffer_set_transport_to_stm -->
 * 
 * @brief Sets the transport of the given micro diagbuffer to 
 *        QDSS/STM
 *
 * This API replaces micro_ULog_SetTransportToSTM of microULog.
 *
 * @param h : Handle to the micro diagbuffer
 *
 * @return MICRO_DIAGBUFFER_SUCCESS on success, 
 *         MICRO_DIAGBUFFER_ERROR   on error
 */
#define micro_diagbuffer_set_transport_to_stm(h) \
              micro_ULog_SetTransportToSTM(h)

/**
 * <!-- micro_diagbuffer_set_transport_to_alt -->
 * 
 * @brief Sets the transport of the micro diagbuffer to the given 
 *        handler
 *
 * This API replaces micro_ULog_SetTransport of microULog.
 *
 * @param h : Handle to the micro diagbuffer
 * @param newTransportVTable : Pointer to a new vtable defining routines that
 *                             will handle writes for the transport
 *
 * @return MICRO_DIAGBUFFER_SUCCESS on success, 
 *         MICRO_DIAGBUFFER_ERROR   on error
 */
#define micro_diagbuffer_set_transport_to_alt(h, newTransportVTable) \
              micro_ULog_SetTransport(h, newTransportVTable)

/**
 * <!-- _micro_diagbuffer_printf -->
 *
 * Do not use this API. Use the MICRO_DIAGBUFFER_PRINTF_n macros.
 * This API replaces micro_ULogFront_RealTimePrintf()() of ULog.
 */ 
#define _micro_diagbuffer_printf(h, dataCount, formatStr, ... ) \
               micro_ULog_RealTimePrintf(h, dataCount, formatStr, __VA_ARGS__ )


/* Printf macros; Users are recommended to use these, rather than the
   RealTimePrintf API to future-proof their code against micro diagbuffer changes */
#define MICRO_DIAGBUFFER_RT_PRINTF_0(h, formatStr) \
             _micro_diagbuffer_printf(h, 0, formatStr)
#define MICRO_DIAGBUFFER_RT_PRINTF_1(h, formatStr, p1) \
             _micro_diagbuffer_printf(h, 1, formatStr, p1)
#define MICRO_DIAGBUFFER_RT_PRINTF_2(h, formatStr, p1, p2) \
             _micro_diagbuffer_printf(h, 2, formatStr, p1, p2)
#define MICRO_DIAGBUFFER_RT_PRINTF_3(h, formatStr, p1, p2, p3) \
             _micro_diagbuffer_printf(h, 3, formatStr, p1, p2, p3)
#define MICRO_DIAGBUFFER_RT_PRINTF_4(h, formatStr, p1, p2, p3, p4) \
             _micro_diagbuffer_printf(h, 4, formatStr, p1, p2, p3, p4)
#define MICRO_DIAGBUFFER_RT_PRINTF_5(h, formatStr, p1, p2, p3, p4, p5) \
             _micro_diagbuffer_printf(h, 5, formatStr, p1, p2, p3, p4, p5)

/**
 * <!-- micro_diagbuffer_vprintf -->
 *
 * @brief Writes printf data to the specified log
 * Example: micro_diagbuffer_vprintf(handle, 3, "test %d: %s", 1, "complete");
 *
 * This API replaces micro_ULog_RealTimeVprintf() of microULog.
 *
 * @param h : Handle to the micro diagbuffer
 * @param dataCount : Number of word-sized parameters in the given va_list
 * @param ap : A variable list of arguments to the printf
 *
 * @return MICRO_DIAGBUFFER_SUCCESS on success, 
 *         MICRO_DIAGBUFFER_ERROR   on error
 */
#define micro_diagbuffer_vprintf(h, dataCount, formatStr, ap) \
              micro_ULog_RealTimeVprintf(h, dataCount, formatStr, ap )

/**
 * <!-- _micro_diagbuffer_write_data -->
 * 
 * Do not use this API. Use the MICRO_DIAGBUFFER_WRITE_DATA_n macros.
 *
 * This API replaces micro_ULog_RealTimeData() of ULog.
 *
 */
#define _micro_diagbuffer_write_data(h, dataCount, ...) \
               micro_ULog_RealTimeData(h, dataCount, __VA_ARGS__ )
  
/**
 * <!-- MICRO_DIAGBUFFER_WRITE_DATA_N -->
 *
 * @brief Macros to provide some parameter count safety to the 
 *        _micro_diagbuffer_write_data function.
 *        These should be used instead of calling the function directly.
 *
 * @param h : A handle indicating where to log.
 * @param p1 - pN : The data to write. These must be (u)int32 in length. 
 *                  Smaller data types will be automatically promoted.
 */
#define MICRO_DIAGBUFFER_WRITE_DATA_1(h, p1)    \
             _micro_diagbuffer_write_data(h, 1, p1)
#define MICRO_DIAGBUFFER_WRITE_DATA_2(h, p1, p2)    \
             _micro_diagbuffer_write_data(h, 2, p1, p2)
#define MICRO_DIAGBUFFER_WRITE_DATA_3(h, p1, p2, p3)    \
             _micro_diagbuffer_write_data(h, 3, p1, p2, p3)
#define MICRO_DIAGBUFFER_WRITE_DATA_4(h, p1, p2, p3, p4)    \
             _micro_diagbuffer_write_data(h, 4, p1, p2, p3, p4)
#define MICRO_DIAGBUFFER_WRITE_DATA_5(h, p1, p2, p3, p4, p5)    \
             _micro_diagbuffer_write_data(h, 5, p1, p2, p3, p4, p5)
#define MICRO_DIAGBUFFER_WRITE_DATA_6(h, p1, p2, p3, p4, p5, p6)    \
             _micro_diagbuffer_write_data(h, 6, p1, p2, p3, p4, p5, p6)
#define MICRO_DIAGBUFFER_WRITE_DATA_7(h, p1, p2, p3, p4, p5, p6, p7)    \
             _micro_diagbuffer_write_data(h, 7, p1, p2, p3, p4, p5, p6, p7)
#define MICRO_DIAGBUFFER_WRITE_DATA_8(h, p1, p2, p3, p4, p5, p6, p7, p8)    \
             _micro_diagbuffer_write_data(h, 8, p1, p2, p3, p4, p5, p6, p7, p8)
#define MICRO_DIAGBUFFER_WRITE_DATA_9(h, p1, p2, p3, p4, p5, p6, p7, p8, p9)    \
             _micro_diagbuffer_write_data(h, 9, p1, p2, p3, p4, p5, p6, p7, p8, p9)
#define MICRO_DIAGBUFFER_WRITE_DATA_10(h, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10)    \
             _micro_diagbuffer_write_data(h, 10, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10)

/**
 * <!-- _micro_diagbuffer_debug_msg -->
 *
 * @brief Writes a Diag F3 msg packet to the specified log
 * Example: _micro_diagbuffer_debug_msg(handle, 3, "test %d: 
 * %s", 1, "complete"); 
 *
 * @param log : Handle to the micro_diagbuffer 
 * @param const_type: constant pointer to the string of the msg
 * @param num_args : Number of word-sized parameters in the given va_list
 * @param version : Version number to interpret data type stored in 'param'
 * @param param : If version == 0, param is ignored.
 *                If version == 1, param is a pointer to a uint32 ID.
 * @param ap : A variable list of arguments to the printf
 *
 * @return MICRO_DIAGBUFFER_SUCCESS on success, MICRO_DIAGBUFFER_ERROR on error
 */
micro_diagbuffer_result _micro_diagbuffer_debug_msg( micro_diagbuffer_handle log,
                                             const msg_const_type* const_type,
                                             uint8 num_args,
                                             uint8 version,
                                             const void* param,
                                             ... );

/* MICRO_DIAGBUFFER_MSG macros; Users are recommended to use these, rather than the
   micro_diagbuffer_debug_msg API to future-proof their code against micro diag buffer changes */
#define MICRO_DIAGBUFFER_MSG(h, xx_ss_id, xx_ss_mask, version, param, xx_fmt)\
          do {\
              XX_MICRO_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
             _micro_diagbuffer_debug_msg(h, &xx_msg_const, 0, version, param);\
          }while(0)\

#define MICRO_DIAGBUFFER_MSG_1(h, xx_ss_id, xx_ss_mask, version, param, xx_fmt, p1) \
          do {\
              XX_MICRO_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
             _micro_diagbuffer_debug_msg(h, &xx_msg_const, 1, version, param, p1);\
          }while(0)\

#define MICRO_DIAGBUFFER_MSG_2(h, xx_ss_id, xx_ss_mask, version, param, xx_fmt, p1, p2) \
          do {\
              XX_MICRO_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
             _micro_diagbuffer_debug_msg(h, &xx_msg_const, 2, version, param, p1, p2);\
          }while(0)

#define MICRO_DIAGBUFFER_MSG_3(h, xx_ss_id, xx_ss_mask, version, param, xx_fmt, p1, p2, p3) \
          do {\
              XX_MICRO_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
             _micro_diagbuffer_debug_msg(h, &xx_msg_const, 3, version, param, p1, p2, p3);\
          }while(0)\

#define MICRO_DIAGBUFFER_MSG_4(h, xx_ss_id, xx_ss_mask, version, param, xx_fmt, p1, p2, p3, p4) \
          do {\
              XX_MICRO_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
             _micro_diagbuffer_debug_msg(h, &xx_msg_const, 4, version, param, p1, p2, p3, p4);\
          }while(0)\

#define MICRO_DIAGBUFFER_MSG_5(h, xx_ss_id, xx_ss_mask, version, param, xx_fmt, p1, p2, p3, p4, p5) \
          do {\
              XX_MICRO_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
             _micro_diagbuffer_debug_msg(h, &xx_msg_const, 5, version, param, p1, p2, p3, p4, p5);\
          }while(0)\

#define MICRO_DIAGBUFFER_MSG_6(h, xx_ss_id, xx_ss_mask, version, param, xx_fmt, p1, p2, p3, p4, p5, p6) \
          do {\
              XX_MICRO_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
             _micro_diagbuffer_debug_msg(h, &xx_msg_const, 6, version, param, p1, p2, p3, p4, p5, p6);\
          }while(0)\

#define MICRO_DIAGBUFFER_MSG_7(h, xx_ss_id, xx_ss_mask, version, param, xx_fmt, p1, p2, p3, p4, p5, p6, p7) \
          do {\
              XX_MICRO_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
             _micro_diagbuffer_debug_msg(h, &xx_msg_const, 7, version, param, p1, p2, p3, p4, p5, p6, p7);\
          }while(0)\

#define MICRO_DIAGBUFFER_MSG_8(h, xx_ss_id, xx_ss_mask, version, param, xx_fmt, p1, p2, p3, p4, p5, p6, p7, p8) \
          do {\
              XX_MICRO_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
             _micro_diagbuffer_debug_msg(h, &xx_msg_const, 8, version, param, p1, p2, p3, p4, p5, p6, p7, p8);\
          }while(0)\

#define MICRO_DIAGBUFFER_MSG_9(h, xx_ss_id, xx_ss_mask, version, param, xx_fmt, p1, p2, p3, p4, p5, p6, p7, p8, p9) \
          do {\
              XX_MICRO_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
             _micro_diagbuffer_debug_msg(h, &xx_msg_const, 9, version, param, p1, p2, p3, p4, p5, p6, p7, p8, p9);\
          }while(0)\

/**
 * <!-- _micro_diagbuffer_event -->
 * 
 * @brief Writes a Diag event packet to the specified log
 *
 * @param log : Handle to the micro_diagbuffer
 * @param event_id : Event ID to log, as defined in event_id_enum_type
 * @param version : Version number to interpret data type stored in 'param'
 * @param param : If version == 0, param is ignored.
 *                If version == 1, param is a pointer to a uint32 ID.
 *
 * @return MICRO_DIAGBUFFER_SUCCESS on success, MICRO_DIAGBUFFER_ERROR on error
 */
micro_diagbuffer_result _micro_diagbuffer_event( micro_diagbuffer_handle log,
                                                event_id_enum_type event_id,
                                                uint8 version,
                                                const void* param );

/* MICRO_DIAGBUFFER_EVENT macros; Users are recommended to use these, rather than the
   micro_diagbuffer_event API to future-proof their code against micro diag buffer changes */
#define MICRO_DIAGBUFFER_EVENT(h, event_id, version, param) \
             _micro_diagbuffer_event(h, event_id, version, param)

/**
 * <!-- _micro_diagbuffer_event_payload -->
 * 
 * @brief Writes a Diag event with payload packet to the specified log
 *
 * @param log : Handle to the micro_diagbuffer.
 * @param event_id : Event ID to log, as defined in event_id_enum_type.
 * @param version : Version number to interpret data type stored in 'param'
 * @param param : If version == 0, param is ignored.
 *                If version == 1, param is a pointer to a uint32 ID.
 * @param length : Length of the associated  payload.
 * @param payload : Pointer to the payload structure. Payload should be published.
                    Please refer to 80-V6196-1 for details.
 *
 * @return MICRO_DIAGBUFFER_SUCCESS on success, MICRO_DIAGBUFFER_ERROR on error
 */
micro_diagbuffer_result _micro_diagbuffer_event_payload( micro_diagbuffer_handle log,
                                                event_id_enum_type event_id,
                                                uint8 version,
                                                const void* param,
                                                uint8 length,
                                                const void* payload );
          
#define MICRO_DIAGBUFFER_EVENT_PAYLOAD(h, event_id, version, param, length, payload) \
             _micro_diagbuffer_event_payload(h, event_id, version, param, length, payload)

/**
 * <!-- _micro_diagbuffer_log_submit -->
 * 
 * @brief Writes a Diag log packet to the specified log
 *
 * @param log : Handle to the micro_diagbuffer.
 * @param log_code : Log code for the Diag log packet.
 * @param version : Version number to interpret data type stored in 'param'
 * @param param : If version == 0, param is ignored.
 *                If version == 1, param is a pointer to a uint32 ID.
 * @param length : Length of the associated  payload.
 * @param log_data : Pointer to the log packet data.
 *
 * @return MICRO_DIAGBUFFER_SUCCESS on success, MICRO_DIAGBUFFER_ERROR on error
 */
micro_diagbuffer_result _micro_diagbuffer_log_submit( micro_diagbuffer_handle log,
                                                log_code_type log_code,
                                                uint8 version,
                                                const void* param,
                                                uint8 length,
                                                const void* log_data );
/* MICRO_DIAGBUFFER_LOG macro; Users are recommended to use these, rather than the
   micro_diagbuffer_log_submit API to future-proof their code against micro diag buffer changes */
#define MICRO_DIAGBUFFER_LOG(h, log_code, version, param, length, log_data) \
             _micro_diagbuffer_log_submit(h, log_code, version, param, length, log_data)
                                                
#ifdef FEATURE_FAST7
/**
 * <!-- micro_diagbuffer_write_7_words -->
 *
 * @brief A special routines that writes 7 words to the specified log in a
 *        format compatible with Raw diagbuffer. This function works on a 
 *        log that ONLY contains Fast7 messages to optimize the way space is 
 *        freed when the log wraps. It CANNOT be mixed with other types of 
 *        writes to the same log. Doing so would corrupt messages in that log.
 *
 * @param h : Handle to the micro diagbuffer
 * @param message : Array of 7 words
 *
 * @return MICRO_DIAGBUFFER_SUCCESS on success, 
 *         MICRO_DIAGBUFFER_ERROR   on error
 */
#define micro_diagbuffer_write_7_words(h, message) \
              micro_ULog_Fast7WordWrite(h, message)
  
#endif /* FEATURE_FAST7 */

#ifdef __cplusplus
}
#endif

#endif /* MICRO_DIAGBUFFER_H */
