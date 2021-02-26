#ifndef MICRO_DIAGBUFFER_DRAIN_H
#define MICRO_DIAGBUFFER_DRAIN_H

/*============================================================================
  micro_diagbuffer_drain.h

  micro_diagbuffer drain and helper functions implementation.

  Copyright (c) 2015 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
============================================================================*/

/* $Header: //components/rel/core.qdsp6/2.1/services/diag/micro_diagbuffer/src/micro_diagbuffer_drain.h#1 $ */

/*==========================================================================
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
08/16/15   vk      file created for micro diagbuffer drain implementation

===========================================================================*/

#include "osal.h"

/* Error codes from micro_diagbuffer_drain routine
 * These Error codes are return values from micro diagbuffer_drain function.
 */
typedef enum
{
  MICRO_DIAGBUFFER_DRAIN_ERROR = -1,
  MICRO_DIAGBUFFER_DRAIN_SUCCESS,
  MICRO_DIAGBUFFER_DRAIN_BUFFER_EMPTY,
  MICRO_DIAGBUFFER_DRAIN_READ_FROM_MDB_FAILED,
  MICRO_DIAGBUFFER_DRAIN_EXIT_DUE_TO_SIGNAL_ON_DIAG,
  MICRO_DIAGBUFFER_DRAIN_EXIT_DUE_TO_UNKOWN_PKT_FORMAT,
} micro_diagbuffer_drain_result;

micro_diagbuffer_drain_result micro_diagbuffer_drain( osal_sigs_t mask ); 

#endif /* #ifndef MICRO_DIAGBUFFER_DRAIN_H */
