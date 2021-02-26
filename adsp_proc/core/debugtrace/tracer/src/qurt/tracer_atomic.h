#ifndef TRACER_ATOMIC_H
#define TRACER_ATOMIC_H
/*===========================================================================
  @file tracer_atomic.h

  Atomic operations.

               Copyright (c) 2012 Qualcomm Technologies Incorporated.
               All rights reserved.
               Qualcomm Confidential and Proprietary
===========================================================================*/
/* $Header: //components/rel/core.qdsp6/2.1/debugtrace/tracer/src/qurt/tracer_atomic.h#1 $ */

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/
#include <atomic_ops_plat.h>

/*---------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ------------------------------------------------------------------------*/
#define TRACER_ATOMIC_INC(_pVal)          atomic_inc(_pVal)
#define TRACER_ATOMIC_DEC(_pVal)          atomic_dec(_pVal)
#define TRACER_ATOMIC_SET(_pVal, _iVal)   atomic_set(_pVal,_iVal)
#define TRACER_ATOMIC_CAS(_pVar, _oVar, _nVar) \
                              atomic_compare_and_set(_pVar, _oVar, _nVar)
/*-------------------------------------------------------------------------*/

#endif /* #ifndef TRACER_ATOMIC_H */

