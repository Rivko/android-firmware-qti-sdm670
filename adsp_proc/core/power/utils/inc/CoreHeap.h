/*==============================================================================
@file CoreHeap.h

Defines a common interface to heap routines

Copyright (c) 2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

$Header: //components/rel/core.qdsp6/2.1/power/utils/inc/CoreHeap.h#1 $
==============================================================================*/
#ifndef COREHEAP_H
#define COREHEAP_H

/**
  @brief Core_Malloc - Allocate a chunk of memory of given size
  
  @param size - Size of chunk in bytes
  
  @return - Pointer to allocated memory if successful, NULL if not
*/
void* Core_Malloc( unsigned int size );

/**
  @brief Core_Free - Frees previously allocated memory
  
  @param ptr - Pointer to memory to free; must be previously allocated by
  Core_Malloc. Accepts NULL.
*/
void Core_Free( void *ptr );

#endif /* COREHEAP_H */
