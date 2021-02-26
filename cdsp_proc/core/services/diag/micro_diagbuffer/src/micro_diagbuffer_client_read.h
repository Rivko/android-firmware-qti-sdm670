#ifndef MICRO_DIAGBUUFER_CLIENT_READ_H
#define MICRO_DIAGBUUFER_CLIENT_READ_H
/*============================================================================
  micro_diagbuffer_client_read.h

  The read function translates client log buffer to local address
  using mapping function set by the client before reading.
  Additionally it handles overrun notification during read operation.

  Copyright (c) 2017 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
============================================================================*/

/* $Header: //components/rel/core.qdsp6/2.1.c4/services/diag/micro_diagbuffer/src/micro_diagbuffer_client_read.h#1 $ */

/*==========================================================================
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
06/08/17   sp     Created new file to support microULog read functionality.
===========================================================================*/

#ifdef __cplusplus
extern "C"
{
#endif

#include "micro_ULog.h"
#include "micro_diagbuffer_parser.h"

/**
 * <!-- micro_diagbuffer_ReadAndFormat_vaddr -->
 *
 * @param handle : Pointer to microULog handle to read from
 * @param format_req : Format of payload to generate (ASCII, BINARY)
 * @param format_rsp : Format of payload generated
                       (will contain OVERRUN information if needed)
 * @param buf_size : size of payload_buf
 * @param payload_buf : pointer to payload buffer where output will be written
 * @param bytes_written: set to number of bytes written to payload_buf
 *                       upon exitting this function. A value of 0 can
 *                       indicate an empty microULog, or that a rewind has occurred.
 * @param mapping_fptr: mapping function set by client for address translation
 *
 * @return MICRO_ULOG_SUCCESS if successful. (bytes_written value of 0 with MICRO_ULOG_SUCCESS indicates an empty microULog)
 *         Error are definded in the micro_ULogResult enum.
 *         MICRO_ULOG_ERROR in this case represents that the microULog was rewound while trying to read from it.
 */
micro_ULogResult micro_diagbuffer_ReadAndFormat_vaddr(micro_ULogHandle handle,
                                                            int format_req,
                                                            unsigned long int *format_rsp,
                                                            int buf_size,
                                                            char * payload_buf,
                                                            unsigned long int *bytes_written,
                                                            vaddr_fptr mapping_fptr);

/**
 * <!-- micro_diagbuffer_NameGet -->
 *
 * @brief Get the log name by its handle.
 *
 * @param Handle : Handle of a microULog to search for.
 * @param logname : Pointer to copy the log name to
 * @return MICRO_ULOG_SUCCESS if handle is valid
 */
micro_ULogResult micro_diagbuffer_NameGet(micro_ULogHandle handle, char* logname);

#ifdef __cplusplus
}
#endif
#endif              /* MICRO_DIAGBUUFER_CLIENT_READ_H  */
