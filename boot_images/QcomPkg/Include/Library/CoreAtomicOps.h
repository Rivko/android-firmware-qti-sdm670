/*==============================================================================
@file CoreAtomicOps.h

Atomic increment and decrement

Copyright (c) 2010-2013 , 2018 by Qualcomm Technologies, Inc. All Rights Reserved
All Rights Reserved.
Qualcomm Confidential and Proprietary
      
==============================================================================*/
#ifndef COREATOMICOPS_H
#define COREATOMICOPS_H

#ifdef WIN8EA
#include "stdint.h"
#else
#include <stdint.h>
#endif

/**
   @brief Core_AtomicInc - Atomic increment
 
   Increments by one the value of the specified 32-bit variable
   as an atomic operation. 
  
   @param pCount : pointer to the variable to be incremented
   @return unsigned : the resulting incremented value
 */
unsigned Core_AtomicInc( volatile uint32_t * pCount );

/**
   @brief Core_AtomicDec - Atomic decrement
 
   Decrements by one the value of the specified 32-bit variable 
   as an atomic operation. 
  
   @param pCount : pointer to the variable to be decremented
   @return unsigned : the resulting decremented value
 */
unsigned Core_AtomicDec( volatile uint32_t * pCount );
#endif /* COREATOMICOPS_H */