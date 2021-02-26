/*==============================================================================
@file CoreOpaqueHandle.h

Very opaque handle type

This provides a handle to arbitrary objects that can not be directly
de-referenced.  In order to retrieve the actual pointer, you must
dereference the handle though this table.

The intent for this handle type is when you do not wish to expose raw
pointers directly to clients, typically for security reasons. This
might be used to expose handles across processor boundaries or from
kernel to user space.

Copyright (c) 2009-2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

$Header: //components/rel/core.qdsp6/2.1.c4/power/utils/inc/CoreOpaqueHandle.h#1 $
==============================================================================*/
#ifndef COREOPAQUEHANDLE_H
#define COREOPAQUEHANDLE_H

/* The CoreOpaqueHandle is a void * so it can be used as a pointer. In
 * particular, CoreOpaqueHandles and be compared for equality and
 * checked against NULL */
typedef void *CoreOpaqueHandle;

/**
   @brief CoreOpaqueHandle_Init - Initialzie the module
**/
void CoreOpaqueHandle_Init( void );

/**
   @brief CoreOpaqueHandle_AddHandle - Add a new local handle to the
   module and get back the corresponding CoreOpaqueHandle

   @param handle : pointer to object

   @return : The opaque handle to this object
**/
CoreOpaqueHandle CoreOpaqueHandle_AddHandle( void *handle );

/**
   @brief CoreOpaqueHandle_RetrieveHandle - Retrieve the local pointer
   that corresponds to the given opaque handle. A NULL handle will be
   returned if the opaque handle does not correspond to a valid local
   object

   @param handle : pointer to object

   @return : The actual pointer corresponding to the opaque handle
**/
void *CoreOpaqueHandle_RetrieveHandle( CoreOpaqueHandle handle );


/**
   @brief CoreOpaqueHandle_RemoveHandle - Remove and return the local
   pointer that corresponds to the given opaque handle. A NULL handle
   will be returned if the opaque handle does not correspond to a
   valid local object
   
   After this call, the CoreOpaqueHandle is no longer valid

   @param handle : pointer to object

   @return : The actual pointer that corresponded to the opaque handle
**/
void *CoreOpaqueHandle_RemoveHandle( CoreOpaqueHandle handle );

#endif /* COREOPAQUEHANDLE_H */
