/*============================================================================
@file CorePRSMList.c

Defines an implementation of a Parallel Read, Serial Modify singly-linked list

Copyright (c) 2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

============================================================================*/
#include "CorePool.h"
#include "CoreVerify.h"
#include "CoreMutex.h"
#include "CoreAtomicOps.h"
#include "CorePRSMList.h"

#ifdef __cplusplus
extern "C" {
#endif

static CorePoolType prsm_list_elem_pool;
static CorePoolType prsm_list_gcb_pool;

/* GC barrier for PRSM lock */
typedef struct prsm_gc_barrier
{
  struct prsm_gc_barrier *next;

  /* Number of threads referring to this state of container */
  unsigned int nref;

  /* Element that will be freed when nref becomes 0 */
  struct CorePRSMListElemType *elem;
  
  /* User-provided cleanup function that will be called when elem is freed */
  void ( *clean_up )( void *data );
} prsm_gc_barrier;

/* PRSM lock */
typedef struct prsm_lock
{
  /* The modify lock; an OS mutex */
  CoreMutex *os_lock;
  
  /* GC barrier list */
  prsm_gc_barrier *gcb_list;
  
  /* Current GC barrier */
  prsm_gc_barrier *cur_gcb;
} prsm_lock;

struct CorePRSMListType
{
  CorePRSMListElemType *head;
  prsm_lock lock;
};

/**
  @brief prsm_free_gc_barriers - Traverse through list of gc barriers for a given
  lock and free those that have a zero refcount i.e those that are inactive.

  Must be invoked from an OS-locked context.

  @param lock - PRSM Lock from which we want to free inactive gc barriers.
*/
static void prsm_free_gc_barriers(prsm_lock *lock)
{
  prsm_gc_barrier *gcb = NULL, *prev_gcb = NULL;

  /* Iterate through the lock's GCB list */
  for ( gcb = lock->gcb_list; gcb != lock->cur_gcb; )
  {
    if ( gcb->nref == 0 )
    {
      /* No thread is referring to this state of the container */
      CorePRSMListElemType *elem = gcb->elem;
      prsm_gc_barrier *free_gcb = gcb;
      
      /* Call clean up func, if any */
      if ( gcb->clean_up )
      {
        gcb->clean_up( elem->data );
      }
      
      gcb = gcb->next;
      /* Remove gcb from list */
      if ( prev_gcb == NULL )
      {
        lock->gcb_list = gcb;
      }
      else
      {
        prev_gcb->next = gcb;
      }
      
      /* Release element */
      Core_PoolFree( &prsm_list_elem_pool, elem );
      
      /* Release GCB */
      Core_PoolFree( &prsm_list_gcb_pool, free_gcb );
    }
    else
    {
      prev_gcb = gcb;
      gcb = gcb->next;
    }
  }
}

/**
  @brief prsm_list_on_remove - Adds the just removed element to the current
  GC barrier, adds a new GC barrier and sets it as current
 
  @param lock: The PRSM lock of the list
  @param elem: The just removed element
  @param clean_up: Optional cleanup function to be invoked when elem is really freed.
*/
static void prsm_list_on_remove( prsm_lock *lock,
                                 CorePRSMListElemType *elem,
                                 void ( *clean_up )( void* ) )
{
  prsm_gc_barrier *new_gcb;
  
  lock->cur_gcb->elem = elem;
  lock->cur_gcb->clean_up = clean_up;
  
  CORE_VERIFY_PTR( 
       new_gcb = (prsm_gc_barrier *)Core_PoolAlloc( &prsm_list_gcb_pool ) );
  memset( new_gcb, 0, sizeof(prsm_gc_barrier) );
  
  lock->cur_gcb->next = new_gcb;
  lock->cur_gcb = new_gcb;
}

/**
  @brief Core_PRSMList_ReadLock - Acquires a read lock on the PRSM list

  A client must acquire a read lock before iterating through or accessing an
  element of the list.

  @param list: The PRSM list
  @return An opaque lock handle
*/
void* Core_PRSMList_ReadLock( CorePRSMListType *list )
{
  prsm_gc_barrier *gcb;
  CORE_VERIFY_PTR( list );

  gcb = list->lock.cur_gcb;

  Core_AtomicInc( (uint32_t*)&gcb->nref );

  return (void *)gcb;
}

/**
  @brief Core_PRSMList_ReadUnlock - Releases a read lock on the PRSM list

  A client must release a read lock after iteration or access

  @param list: The PRSM list
  @param handle: The opaque lock handle returned from Core_PRSMList_ReadLock
*/
void Core_PRSMList_ReadUnlock( CorePRSMListType *list, void *handle )
{
  prsm_gc_barrier *gcb = (prsm_gc_barrier *)handle;
  CORE_VERIFY_PTR( gcb );

  Core_AtomicDec( (uint32_t*)&gcb->nref );
}

/**
  @brief Core_PRSMList_ModifyLock - Acquires a write/modify lock on the PRSM list

  In general, users will not need to call this function explicitly. List modify
  routines in this module internally acquire and release the write lock.
  
  @param list: The PRSM list
  @return An opaque lock handle
*/
void* Core_PRSMList_ModifyLock( CorePRSMListType *list )
{
  prsm_lock *lock;
  CORE_VERIFY_PTR( list );

  lock = &list->lock;
  
  /* Acquire OS lock */
  Core_MutexLock( lock->os_lock );
  
  /* Release inactive GC barriers */
  prsm_free_gc_barriers( lock );
  
  /* Obtain and return a readlock */
  return Core_PRSMList_ReadLock( list );
}

/**
  @brief Core_PRSMList_ModifyUnlock - Releases a write/modify lock on the PRSM list

  In general, users will not need to call this function explicitly. List modify
  routines in this module internally acquire and release the write lock.
  
  @param list: The PRSM list
  @param handle: The opaque lock handle returned from Core_PRSMList_ModifyLock
*/
void Core_PRSMList_ModifyUnlock( CorePRSMListType *list, void *handle )
{
  prsm_gc_barrier *gcb = (prsm_gc_barrier *)handle;
  prsm_lock *lock;

  CORE_VERIFY_PTR( gcb );
  CORE_VERIFY_PTR( list );
  
  lock = &list->lock;
  
  Core_AtomicDec( (uint32_t*)&gcb->nref );
  Core_MutexUnlock( lock->os_lock );
}

/**
  @brief Core_PRSMList_PushFront - Adds an element to the head of the list
*/  
void Core_PRSMList_PushFront( CorePRSMListType *list, void *data )
{
  CorePRSMListElemType *elem;
  void *lock;
  
  CORE_VERIFY_PTR( list );
  
  /* Prepare list element */
  elem = (CorePRSMListElemType *)Core_PoolAlloc( &prsm_list_elem_pool );
  CORE_VERIFY_PTR( elem );
  
  lock = Core_PRSMList_ModifyLock( list );
  
  elem->data = data;
  elem->next = list->head;
  list->head = elem;

  Core_PRSMList_ModifyUnlock( list, lock );  
}

/**
  @brief Core_PRSMList_PopFront - Removes and returns the element at the
  head of the list
*/  
CorePRSMListElemType* Core_PRSMList_PopFront( CorePRSMListType *list,
                                              void ( *clean_up )( void* ) )   
{
  CorePRSMListElemType *elem;
  void *lock;
  
  CORE_VERIFY_PTR( list );
  
  lock = Core_PRSMList_ModifyLock( list );
  elem = list->head;

  if ( elem )
  {
    list->head = elem->next;
    prsm_list_on_remove( &list->lock, elem, clean_up );
  }
  
  Core_PRSMList_ModifyUnlock( list, lock );
  return elem;
}

/**
  @brief Core_PRSMList_PeekFront - Returns the element at head of list, without
  removing it
  
  MUST be invoked after calling Core_PRSMList_ReadLock()
*/
CorePRSMListElemType* Core_PRSMList_PeekFront( CorePRSMListType *list )
{
  CORE_VERIFY_PTR( list );
  return list->head;
}

/**
  @brief Core_PRSMList_PoolInit - Initializes the pool of PRSM list elements.
  Uses a static initializer flag. Revisit if reentrancy becomes a problem.
*/
static void Core_PRSMList_PoolInit( void )
{
  static unsigned int prsm_list_pool_init = 0;
  
  if ( !prsm_list_pool_init )
  {
    CorePoolConfigType prsm_list_elem_config = 
    {
      sizeof(CorePRSMListElemType),
      64,
      16
    };
    
    CorePoolConfigType prsm_list_gcb_config =
    {
      sizeof(prsm_gc_barrier),
      16,
      16
    };
    
    Core_PoolConfig( &prsm_list_elem_pool, &prsm_list_elem_config );
    Core_PoolConfig( &prsm_list_gcb_pool, &prsm_list_gcb_config );
    
    prsm_list_pool_init = 1;
  }
}

/**
  @brief Core_PRSMList_Create - Create a PRSM list. Initializes the PRSM
  element pool on first call.
*/
CorePRSMListType* Core_PRSMList_Create( void )
{
  CorePRSMListType *list = NULL;
  prsm_gc_barrier *gcb = NULL;
  
  Core_PRSMList_PoolInit();
  
  CORE_DAL_VERIFY( DALSYS_Malloc( sizeof(CorePRSMListType),
                                  (void **)&list ) );
  
  list->head = NULL;
  list->lock.os_lock = Core_MutexCreate( CORE_MUTEXATTR_DEFAULT );
  
  CORE_VERIFY_PTR( 
       gcb = (prsm_gc_barrier *)Core_PoolAlloc( &prsm_list_gcb_pool ) );
  memset( gcb, 0, sizeof(prsm_gc_barrier) );

  list->lock.cur_gcb = list->lock.gcb_list = gcb;
  return list;
}

#ifdef __cplusplus
}
#endif
