/*==============================================================================
@file CoreHeap.h

Defines a common interface to heap routines

Copyright (c) 2013 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

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

/**
  @brief Core_Realloc - Reallocate a chunk of memory and preserve the original content
  
  @param in_ptr - Pointer to memory to reallocate; must be previously allocated by
  Core_Malloc. Accepts NULL.

  @param in_size - Size of chunk in bytes
*/
void* Core_Realloc(void* in_ptr,  unsigned int in_size);

#endif /* COREHEAP_H */
