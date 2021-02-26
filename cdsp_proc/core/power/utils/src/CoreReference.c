/*============================================================================
@file CoreReference.c

Implementation for a referenced tracked structures

Copyright (c) 2010-2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

$Header: //components/rel/core.qdsp6/2.1.c4/power/utils/src/CoreReference.c#1 $
============================================================================*/
#include "CoreReference.h"
#include "CoreVerify.h"

/* Protected functions from CorePool.c */
extern void Core_PoolLock  ( CorePoolType *pPool );
extern void Core_PoolUnlock( CorePoolType *pPool );

/* Recurisvely walk the list.  The reference list is expected to be
 * small (max of ~3?), so the recursive walk isn't expected to be
 * probelmatic*/
static CoreReferenceHeader*
CoreReference_RemoveReference( CoreReferenceHeader *list, 
                               CoreReferenceHeader *reference )
{
  if ( NULL == list ) { return NULL; }
  if ( reference == list )
  {
    return reference->next;
  }
  list->next = CoreReference_RemoveReference( list->next, reference );
  return list;
}

void *CoreReferenceBuffer_Data( CoreReferenceBuffer *buffer )
{
  return &buffer->data;
}

/* Add a reference to the buffer */
void CoreReferenceBuffer_AddReference( CoreReferenceBuffer *buffer, 
                                       CoreReferenceHeader *reference )
{
  reference->next    = buffer->references;
  buffer->references = reference;
}

CoreReferenceBuffer_FinalizeFcn CoreReferenceBuffer_SetFinalizeFcn(
  CoreReferenceBuffer             *buffer, 
  CoreReferenceBuffer_FinalizeFcn  finalize_fcn )
{
  CoreReferenceBuffer_FinalizeFcn old_finalize_fcn;
  CORE_VERIFY_PTR( buffer );
  old_finalize_fcn = buffer->finalize_fcn;
  buffer->finalize_fcn = finalize_fcn;
  return old_finalize_fcn;
}

/* Helper function to remove the element from the list */
static void CoreReferenceBuffer_RemoveReference( CoreReferenceBuffer *buffer, 
                                                 CoreReferenceHeader *reference )
{
  buffer->references = CoreReference_RemoveReference( buffer->references, reference );
}

/* Private functions inside CoreReferencePool */
static CoreReferenceBuffer *CoreReferencePool_RemoveReferencedBuffer(
  CoreReferenceBuffer *list, 
  CoreReferenceBuffer *buffer )
{
  if ( NULL == list ) { return NULL; }
  if ( buffer == list )
  {
    return buffer->next;
  }
  list->next = CoreReferencePool_RemoveReferencedBuffer( list->next, buffer );
  return list;
}

/* Garbage collect the buffer.  Check every active reference for
 * validity.  Remove any invalidated references and free the buffer if
 * there are no longer any valid external references to this buffer */
static void CoreReferencePool_GarbageCollectBuffer( 
  CoreReferencePool *pool,
  CoreReferenceBuffer *buffer )
{
  CoreReferenceHeader *reference = buffer->references;
  while( NULL != reference )
  {
    if ( !reference->vtable->VerifyReference( reference ) )
    {
      CoreReferenceBuffer_RemoveReference( buffer, reference );
      reference->vtable->FreeReference( reference );
    }
    reference = reference->next;
  }

  if ( NULL == buffer->references ) 
  {
    if ( NULL != buffer->finalize_fcn ) 
    {
      buffer->finalize_fcn( buffer );
    }
    pool->referenced_list = CoreReferencePool_RemoveReferencedBuffer
      ( pool->referenced_list, buffer );
    Core_PoolFree( &pool->pool, buffer );
  }
}

/* Garbage collect the pool - check everything on the referenced list
 * for active references and place unreferenced items on the free list */
static void CoreReferencePool_GarbageCollectPool( CoreReferencePool *pool )
{
  CoreReferenceBuffer *garbage;

  Core_PoolLock( &pool->pool );

  garbage = pool->referenced_list;

  while ( NULL != garbage )
  {
    /* Create a temp pointer and dereference it first in case it gets
     * reclaimed by the garbage collection */
    CoreReferenceBuffer *buffer = garbage;
    garbage = buffer->next;

    CoreReferencePool_GarbageCollectBuffer( pool, buffer );
  }
  Core_PoolUnlock( &pool->pool );
}


void CoreReferencePool_Config(       CoreReferencePool  *pool,
                               const CorePoolConfigType *config )
{
  /* Need to modify the size of the element size to include the
   * reference tracking, but want the config structure to be const so
   * it can potentially be in ROM */
  CorePoolConfigType my_config;

  CORE_VERIFY_PTR( pool );
  CORE_VERIFY_PTR( config );

  my_config = *config;
  my_config.size += sizeof(CoreReferenceBuffer);

  Core_PoolConfig( &pool->pool, &my_config );
  pool->referenced_list = NULL;
}



/* Build a reference tracked structure on top of the CorePool  */

/* Alloc from the pool of CoreReferencedBuffers.  If the free_list is
 * empty, run a garbage collection cycle  */
CoreReferenceBuffer *CoreReferencePool_Alloc( CoreReferencePool *pool )
{
  CoreReferenceBuffer *buffer;
  
  CORE_VERIFY_PTR( pool );

  /* If the free list is empty, do a garbage collection cycle */
  if ( NULL == pool->pool.pFreeList )
  {
    CoreReferencePool_GarbageCollectPool( pool );
  }
  buffer = (CoreReferenceBuffer *)Core_PoolAlloc( &pool->pool );
  if (NULL != buffer)
  {
    memset( buffer, 0, pool->pool.uElemSize );
  }
  return(buffer);
}

/* Free the buffer. If it has no active references add it to the free
 * list, otherwise add it to the referenced_list  */
void CoreReferencePool_Free( CoreReferencePool   *pool,
                             CoreReferenceBuffer *buffer )
{
  CORE_VERIFY_PTR( pool );
  CORE_VERIFY_PTR( buffer );

  if ( NULL == buffer->references )
  {
    CoreReferencePool_GarbageCollectBuffer( pool, buffer );
  }
  else
  {
    CoreReferenceHeader *reference = buffer->references;
    while( NULL != reference )
    {
      reference->vtable->FinalizeReference( reference );
      reference = reference->next;
    }
    Core_PoolLock( &pool->pool );
    buffer->next          = pool->referenced_list;
    pool->referenced_list = buffer;
    Core_PoolUnlock( &pool->pool );
  }
}
