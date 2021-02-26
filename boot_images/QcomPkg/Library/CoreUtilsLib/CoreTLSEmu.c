/*============================================================================
@file CoreTLSEmu.c

Defines emulated TLS routines if not supported by OS

Copyright (c) 2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

============================================================================*/
#include "CoreThread.h"

#ifdef CORE_UTILS_EMULATE_TLS

#include "CorePRSMList.h"
#include "CoreVerify.h"
#include "CorePool.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Counter representing allocated TLS keys */
static unsigned int key_counter;

/* The (tid, data_ptr) data structure */
typedef struct CoreTLSDataType
{
  unsigned int  tid;
  void         *data;
} CoreTLSDataType;

/* The TLS list; we limit ourselves to 10 unique keys */
static struct CoreTLSListType
{
  /* List of (tid, data_ptr) elements per key */
  CorePRSMListType *tid_list;
  /* Pool of (tid, data_ptr) elements per key */
  CorePoolType      pool;
} tls_list[10];

/**
  @note We do not release (tid, data_ptr) tuples back to the pool,
  because we do not know when a thread dies and its TLS structure
  is no longer relevant. We expect to be used on platforms that
  do not create and destroy threads at run-time enough number of
  times for this to be a problem.
  TODO: Revisit
*/

/**
  @brief Core_TlsAlloc - Allocates TLS keys, upto a maximum of 10. 
  Creates a TID list and allocates a pool of (tid, data_ptr) tuples for
  the key before returning.
*/
unsigned int Core_TlsAlloc( void )
{
  CorePoolConfigType tls_pool_config =
  {
    sizeof(CoreTLSDataType),
    16,
    8
  };
  /* Assign a key */
  unsigned int key = key_counter++;
  CORE_VERIFY( key < 10 );
  /* Create the list */
  tls_list[key].tid_list = Core_PRSMList_Create();
  /* Initialize the pool */
  Core_PoolConfig( &tls_list[key].pool, &tls_pool_config );
  return key;
}

/**
  @brief get_tls_data - Shared routine, used by both Core_TlsGet and
  Core_TlsSet. Iterates through the TID list to find the (tid, data_ptr)
  tuple against the given tid, if previously set.
*/
CORE_STATIC_INLINE
CoreTLSDataType* get_tls_data( unsigned int key, unsigned int tid )
{
  CoreTLSDataType *tls_ptr = NULL;
  CorePRSMListElemType *elem;
  void *read_lock;

  read_lock = Core_PRSMList_ReadLock( tls_list[key].tid_list );

  /* Iterate through the list, looking for a tid match */
  elem = Core_PRSMList_PeekFront( tls_list[key].tid_list );
  while ( elem )
  {
    tls_ptr = (CoreTLSDataType *)elem->data;
    if ( tls_ptr->tid == tid )
    {
      break;
    }
    elem = elem->next;
  }
  Core_PRSMList_ReadUnlock( tls_list[key].tid_list, read_lock );

  /* If end_of_list return NULL, else the found tuple */
  return ( elem ? tls_ptr : NULL );
}

/**
  @brief Core_TlsSet - Set given 'data' pointer against key for this thread.
  First check the list to see if a tuple was previously allocated to this
  thread (from a previous TlsSet). If so, just update the data_ptr.
  Else, allocate and add a new tuple in the list.
*/
void Core_TlsSet( unsigned int key, void *data )
{
  CoreTLSDataType *tls_ptr;
  unsigned int tid = Core_ThreadSelf();

  /* See if the current thread has a tuple in the list already */
  tls_ptr = get_tls_data( key, tid );

  if ( tls_ptr )
  { 
    tls_ptr->data = data;
  }
  else
  {
    /* Allocate and add a new (tid, data) tuple to the list */
    CORE_VERIFY_PTR( 
         tls_ptr = (CoreTLSDataType *)Core_PoolAlloc( &tls_list[key].pool ) );
    tls_ptr->tid = tid;
    tls_ptr->data = data;

    Core_PRSMList_PushFront( tls_list[key].tid_list, tls_ptr );
  }
}

/**
  @brief Core_TlsGet - Returns the data pointer corresponding to 'key' for
  the current thread. Scans the list for tid of the current thread and
  returns data_ptr, if available.
*/
void* Core_TlsGet( unsigned int key )
{
  CoreTLSDataType *tls_ptr;
  unsigned int tid = Core_ThreadSelf();

  tls_ptr = get_tls_data( key, tid );

  return ( tls_ptr ? tls_ptr->data : NULL );
}

#ifdef __cplusplus
}
#endif

#endif /* CORE_UTILS_EMULATE_TLS */
