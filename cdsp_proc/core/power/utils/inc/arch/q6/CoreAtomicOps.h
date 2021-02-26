/*==============================================================================
@file CoreAtomicOps.h

Implements an atomic increment or decrement

Copyright (c) 2010-2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

$Header: //components/rel/core.qdsp6/2.1.c4/power/utils/inc/arch/q6/CoreAtomicOps.h#1 $
==============================================================================*/
#ifndef Q6_COREATOMICOPS_H
#define Q6_COREATOMICOPS_H

/* This is an inline implementation of CoreAtomicOps.h for the Q6 */
static inline unsigned Core_AtomicInc( volatile uint32_t * pCount )
{
  unsigned retval;
  __asm__ __volatile (
    "0:   %0 = memw_locked(%1);\n"
    "     %0 = add(%0, #1);\n"
    "     memw_locked(%1,p0) = %0;\n"
    "     if (!p0) jump 0b;\n"
    : "=&r" (retval)
    : "r" (pCount)
    : "p0"
    );
  return retval;
}

static inline unsigned Core_AtomicDec( volatile uint32_t * pCount )
{
  unsigned retval;
  __asm__ __volatile (
    "0:   %0 = memw_locked(%1);\n"
    "     %0 = add(%0, #-1);\n"
    "     memw_locked(%1,p0) = %0;\n"
    "     if (!p0) jump 0b;\n"
    : "=&r" (retval)
    : "r" (pCount)
    : "p0"
    );
  return retval;
}

/* Ideally AtomicInc and AtomicDec would be expressed as constant
   increments to this function, but I can't figure out how to make the
   compiler properly handle constant increments.

   This may want to be added to the public AtomicOps API.
*/
static inline unsigned Core_AtomicAdd( volatile uint32_t * pCount, int32_t increment )
{
  unsigned retval;
  __asm__ __volatile (
    "0:   %0 = memw_locked(%1);\n"
    "     %0 = add(%0, %2);\n"
    "     memw_locked(%1,p0) = %0;\n"
    "     if (!p0) jump 0b;\n"
    : "=&r" (retval)
    : "r" (pCount), "r" (increment)
    : "p0"
    );
  return retval;
}

#endif /* Q6_COREATOMICOPS_H */
