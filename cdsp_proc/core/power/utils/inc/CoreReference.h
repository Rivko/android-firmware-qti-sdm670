/*============================================================================
@file CoreReference.h

Abstract Reference Tracked data type

This defines an abstract reference tracking structure.  Any entity
that wants to add reference tracking would "inherit" from the abstract
core reference and provide a constructor of sorts that sets up the
vtable, and adds whatever extra tracking data is required.

This implicitly assumes references will be stored in a list

Copyright (c) 2009-2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

$Header: //components/rel/core.qdsp6/2.1.c4/power/utils/inc/CoreReference.h#1 $
============================================================================*/
#ifndef COREREFERNCE_H
#define COREREFERNCE_H

/* Abstract Reference Header
 *
 * Every Concrete Reference type must begin with this header */
typedef struct CoreReferenceHeader
{
  struct CoreReferenceHeader       *next;   /* Link list of references */
  const struct CoreReferenceVtable *vtable; /* VTable for this type of
                                             * reference */
} CoreReferenceHeader;


/* Virtual Function Table for Abstract reference */
typedef struct CoreReferenceVtable
{
  /* A reference is always valid until it has been finalized. A
   * finalized reference should cache whatever state is necessary to
   * enable the reference to be validated at a later date  */
  void (*FinalizeReference) ( CoreReferenceHeader *reference );

  /* Verify the reference - returns true if the reference is still
   * accessible.  That is, destroying the undelying object will
   * invalidate some external entity. If false, the underlying
   * object is no longer being referenced */
  int  (*VerifyReference)   ( CoreReferenceHeader *reference );

  /* Free the underlying reference.  */
  void (*FreeReference)     ( CoreReferenceHeader *reference );
} CoreReferenceVtable;


/* Define a buffer that mainatins a list of outstanding references -
 * this allows the same data to be multiply referenced (say ulog and
 * thread references)  */
typedef struct CoreReferenceBuffer
{
/* Linked list of buffers - this is used by the pool to track the free'd
   but still referenced buffers */  
  struct CoreReferenceBuffer *next; 

  /*List of outstanding references to this buffer */
  CoreReferenceHeader *references; 

  /* Finalize Fcn - if defined, this function will be invoked on this
   * buffer when it is finally released.  This can be used to perform
   * any any additional desired cleanup */
  void (*finalize_fcn) ( struct CoreReferenceBuffer *buffer );

  /* Actual data in the buffer follows this header */
  unsigned char         data[1];

} CoreReferenceBuffer;

/* Finalize Function invoked when buffer is finally destroyed.  This
 * can be used to perform any any additional desired cleanup */
typedef void (*CoreReferenceBuffer_FinalizeFcn) ( CoreReferenceBuffer *buffer );


/**
   @brief CoreReferenceBuffer_Data - get a pointer to the data field of
   the CoreReferenceBuffer

   This is a utility function
   
   @param buffer : pointer to the buffer being used

   @return The return value is a void *, it's intended to be cast to
   whatever type the buffer is intended to hold
**/
void *CoreReferenceBuffer_Data( CoreReferenceBuffer *buffer );

/**
   @brief CoreReferenceBuffer_AddReference - add a new reference to the buffer

   It is assumed theat this function will not be accessed
   concurrently.  If concurrent accesses to this is required, it is
   the responsibility of the user to synchronize the accesses
   
   @param buffer : pointer to the buffer who's finalize function is being set
   @param reference : the new reference being added
**/
void CoreReferenceBuffer_AddReference(
  CoreReferenceBuffer *buffer, 
  CoreReferenceHeader *reference );


/**
   @brief CoreReferenceBuffer_SetFinalizeFcn - set the finalize
   function for the reference buffer instance

   The finalzie function will be invoked for this buffer when it is
   finally reclaimed.  This is an optional feature.

   @param buffer : pointer to the buffer who's finalize function is being set
   @param finalize_fcn : the finalize fcn to set

   @return : The previous value of the finalize function. If non-NULL,
   it's the responsibility of the person overriding the previous
   finalize function to ensure that the previous functionis handled
   appropriately
**/
CoreReferenceBuffer_FinalizeFcn CoreReferenceBuffer_SetFinalizeFcn(
  CoreReferenceBuffer             *buffer, 
  CoreReferenceBuffer_FinalizeFcn  finalize_fcn );


#include "CorePool.h"

/* Define a CoreReferencePool.  This effectively publically inherits
 * from the CorePool, but as this is C, this is a bit cumbersome. The
 * user has to explictly dereference the CorePoolType pool member in
 * order to invoke the CorePool functions.  */
typedef struct CoreReferencePool
{
  CorePoolType         pool;   
  CoreReferenceBuffer *referenced_list;
} CoreReferencePool;


/**
   @brief CoreReferencePool_Create - Create a memory pool and initialize it

   This function dynamically allocates a pool of memory buffers from
   the default DALSYS heap, initialize the pool, and set a default
   allocator function for subsequent allocations from the same
   heap. The element size is the size of the user data; the reference
   tracking members will be added internally

   The CoreReferencePool only supports dynamic memory management -
   static reference tracked pools are not supported

   See the description of Core_PoolInit() for the alignment issue.

   @param pool : pointer to the pool to be initialized
   @param config : pointer to configuration structure for the pool
*/
void CoreReferencePool_Config(       CoreReferencePool  *pool, 
                               const CorePoolConfigType *config );


/**
   @brief CoreReferencePool_Alloc -  Allocate an element from the pool

   @param pPool : pointer to the pool to allocate from
   @return memory allocated, or NULL if there was no memory available
*/
CoreReferenceBuffer *CoreReferencePool_Alloc( CoreReferencePool *pPool );

/**
   @brief Core_PoolFree - Free an element that was allocated from the pool.

   This only releases the implicit references. The buffer will not get
   recycled until all external references are also released.  The pool does not attempt to check references for validity unless the free pool is empty

   @param pPool: pointer to the pool the element was allocated from
   @param pData: pointer to data being freed
*/
void CoreReferencePool_Free( CoreReferencePool   *pool,
                             CoreReferenceBuffer *buffer );

#endif /* COREREFERNCE_H */
