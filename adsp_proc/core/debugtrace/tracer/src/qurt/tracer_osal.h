#ifndef TRACER_OSAL_H
#define TRACER_OSAL_H
/*===========================================================================
  @file tracer_osal.h

  Target system specific information needed by TDS.

               Copyright (c) 2011-2012 Qualcomm Technologies Incorporated.
               All rights reserved.
               Qualcomm Confidential and Proprietary
===========================================================================*/
/* $Header: //components/rel/core.qdsp6/2.1/debugtrace/tracer/src/qurt/tracer_osal.h#1 $ */

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/
#include <qurt_thread.h>
#include "tracer.h"

/*---------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------
 * Type Declarations
 * ------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/**
  @brief Get an identifier for the CPU
  @return CPU identifier
 */
static __inline tracer_cpu_id_t tracer_osal_cpu_id(void)
{
   return (0);
};

/**
  @brief Get an identifier for the process
  @return Process identifier
 */
static __inline tracer_process_id_t tracer_osal_process_id(void)
{
   return (0);
};

/**
  @brief Get an identifier for the thread
  @return Thread identifier
 */
static __inline tracer_thread_id_t tracer_osal_thread_id(void)
{
   return ((tracer_thread_id_t) qurt_thread_get_id());
};

#endif /* #ifndef TRACER_OSAL_H */

