#ifndef MICRO_DIAGBUFFER_READ_H
#define MICRO_DIAGBUFFER_READ_H

/*============================================================================
  micro_diagbuffer_drain.h

  micro_diagbuffer read function prototype, accosiated functions and types.

  Copyright (c) 2015 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
============================================================================*/

/* $Header: //components/rel/core.qdsp6/2.1/services/diag/micro_diagbuffer/src/micro_diagbuffer_read.h#1 $ */

/*==========================================================================
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
08/21/15   vk      file created for micro diagbuffer read implementation

===========================================================================*/
#include <stringl/stringl.h>        /* For memscpy */

#include "micro_diagbuffer.h"       /* For MICRO_DIAGBUFFER_TYPE */

// Defines for the logStatus word in the header (MICRO_DIAGBUFER_STRUCT) defined below.
#define MICRO_DIAGBUFFER_STATUS_DEFINED              (0x1 << 0)    /* Set if the log is defined */
#define MICRO_DIAGBUFFER_STATUS_ENABLED              (0x1 << 1)    /* Set if the log is enabled */

#define micro_diagbuffer_internal_memcpy(dest, dest_len, src, src_len) memscpy(dest, dest_len, src, src_len)

/**
 * <!-- micro_diagbuffer_read -->
 *
 * @brief Read data from the micro diagbuffer
 *
 * Read data from the micro diagbuffer. This data is unformatted. 
 * Call ULogCore_MsgFormat to format each message. Only full 
 * messages are placed in the output buffer, thus it is incorrect 
 * to use outputCount != outputSize as an indicator that the log 
 * buffer is empty.
 *
 *
 * @param h : A micro diagbuffer handle of the buffer read.
 * @param outputSize : Size of the output buffer.
 * @param outputMem : A pointer to the output buffer.
 * @param outputCount : The amount of data placed in the output buffer.
 * @param outputMessageLimit : The maximum number of messages to read from
 *                     the buffer.
 *
 * @return MICRO_DIAGBUFFER_SUCCESS if the read completed.
 */
micro_diagbuffer_result micro_diagbuffer_read( micro_diagbuffer_handle h,
                                               uint32 outputSize,
                                               uint8 *outputMem,
                                               uint32 *outputCount,
                                               uint32 outputMessageLimit );
											   
#endif /* #ifndef MICRO_DIAGBUFFER_READ_H */
