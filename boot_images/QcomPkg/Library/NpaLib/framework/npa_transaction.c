/*============================================================================
@file npa_transaction.c

NPA Transaction API

Copyright (c) 2010-2014 Qualcomm Technologies Inc.
All Rights Reserved.
Qualcomm Confidential and Proprietary

==========================================================================*/

#include "npa_internal.h"
#include "npa_inlines.h"
#include "CoreThread.h"

#define NPA_TRANSACTION_SHARED_LOCK 0x2

#ifdef __cplusplus
extern "C" {
#endif

/* TODO: Determine the optimal pool */
#define transaction_pool resource_pool
CORE_STATIC_VERIFY( sizeof( npa_transaction ) <= sizeof( npa_resource ) );

/* NPA notionally allows one active transaction per thread. However, 
   in practice, we did not support this. Given the lack of portable 
   thread-local-storage support across targets, we used a global transaction
   structure and lock to track transactions and thereby, restricted use of
   transactions to one active per system. In the 2012 branch, we remove
   this restriction, by using available TLS interfaces to support one active
   transaction per thread.

   In npa_transaction_init, we allocate a TLS key for use of transactions.
   Then in each thread, as a new transaction is begun, we save the transaction
   handle against this key. get_current_transaction retrieves the handle
   against this key from the TLS of the current thread. end_transaction sets
   this key in the TLS of the current thread to NULL.
*/

/** TLS key used to store transaction data */
unsigned int npa_transaction_key;

/** Lock used to serialize transaction definitions */
static CoreMutex *npa_transaction_lock;

/** List of all defined transactions */
static npa_list_header npa_transaction_list;


/**  
 * npa_transaction_init
 *
 * @brief Setup the global TLS key and transaction lock.
 */
void npa_transaction_init( void )
{
  npa_transaction_key = Core_TlsAlloc();
  CORE_VERIFY_PTR( 
       npa_transaction_lock = Core_MutexCreate( CORE_MUTEXATTR_DEFAULT ) );
}


/**  
 * npa_transaction_find_resource_overlap
 *
 * @brief Check for resources in common. 
 *
 * Iterates through the resource lists of two transactions passed in as
 * data1 and data2, checking to see if they share any resources. If they
 * do, the transaction in data2 is set to use the lock of the earlier
 * defined transaction in data1. This ensures that the two transactions
 * never execute concurrently
 *
 * @param data1: transaction to compare
 * @param data2: transaction to compare
 */
static void npa_transaction_find_resource_overlap( void *data1, void *data2 )
{
  npa_transaction_handle t1 = (npa_transaction_handle)data1;
  npa_transaction_handle t2 = (npa_transaction_handle)data2;
  npa_list_elem *it1, *it2;

  CORE_VERIFY_PTR( t1 );
  CORE_VERIFY_PTR( t2 );

  it1 = t1->resource_list.head;
  it2 = t2->resource_list.head;
  
  while ( it2 )
  {
    while ( it1 )
    {
      if ( it2->data == it1->data )
      {
        t2->lock = t1->lock;
        t2->attributes |= NPA_TRANSACTION_SHARED_LOCK;
        t1->attributes |= NPA_TRANSACTION_SHARED_LOCK;
        return;
      }
      it1 = it1->next;
    }
    it2 = it2->next;
  }
}


/**
 * npa_transaction_resources_available
 *
 * @brief Used as the callback function to npa_resources_available_cb in npa_define_transaction.
 *
 * The specified resources may not be available when the transaction is defined.
 * This function is called when everything is available. 
 */
static void npa_transaction_resources_available( void *arg1, unsigned arg2,
                                                 void *arg3, unsigned arg4 )
{
  npa_transaction_handle transaction = (npa_transaction_handle)arg1;
  const char **resources = (const char **)arg3;
  unsigned int num_resources = arg4, i;
  npa_list_elem *it1, *it2;
  npa_list_header dep_list;
  void *ptr;

  /* For klocworks */
  CORE_VERIFY_PTR( resources );
  CORE_VERIFY( num_resources > 0 );

  memset( &dep_list, 0, sizeof(npa_list_header) );
  
  /* Serialize transaction definitions */
  Core_MutexLock( npa_transaction_lock );

  /* Topsort the resource list */
  /* First, push given resources into the list */
  for ( i = 0; i < num_resources; ++i )
  {
    npa_resource *resource;
    CORE_VERIFY_PTR( resource = npa_find_resource( resources[i] ) );

    /* Assume nobody has listed the same resource twice */
    npa_list_PushBack( &transaction->resource_list, resource ); 
  }

  /* Iterate through the list, adding dependencies of each resource in
     the list, to the list if not already present. If already present,
     there is a diamond in the transaction resource graph. Mark the
     transaction as requiring PESSIMISTIC locking, remove the
     dependency from its position in the list and add it to the very
     back. By the end of the outer loop, the list will be sorted in top-order
   */
  it1 = transaction->resource_list.head;
  while ( it1 )
  {
    npa_resource *resource = (npa_resource *)it1->data;
    
    /* Create a list of dependencies. Need the loop to trim out
       duplicate references. The same dependency may be listed
       twice - once as REQUIRED, once as SUPPRESSIBLE */
    for ( i = 0; i < resource->node->dependency_count; ++i )
    {
      npa_node_dependency *dependency = &resource->node->dependencies[i];
      if ( dependency->handle &&
           npa_list_Find( &dep_list, dependency->handle->resource ) == NULL )
      {
        npa_list_PushBack( &dep_list, dependency->handle->resource );
      }
    }
    
    ptr = npa_list_PopFront( &dep_list );
    while ( ptr )
    {
      /* For each dependency, look to see if it is already in resource_list */
      it2 = transaction->resource_list.head;
      do
      {
        if ( it2->data == ptr )
        {
          /* If already in list, we have a diamond */
          transaction->attributes |= NPA_TRANSACTION_PESSIMISTIC_LOCKING;
          /* Do a shuffle-delete. Copy the data/next pointers of the next
             element into this one and delete the next element. If there is
             no next element, there is nothing to do.
           */
          if ( it2->next != NULL )
          {
            npa_list_elem *next = it2->next;
            /* Move resource to the back of list. First, add at tail. Then,
               remove this copy */
            npa_list_PushBack( &transaction->resource_list, ptr );

            /* Shuffle */
            it2->data = next->data;
            it2->next = next->next;
              
            /* If the outer iterator it1 was pointing at 'next', we need
               to reset it to point to it2, since we moved 'next' to it2 */
            if ( it1 == next ) it1 = it2;
              
            /* Delete */
            Core_PoolFree( &npa.list_pool, next );
            break;
          }
        }
        it2 = it2->next;
      } while ( it2 );

      /* If not already in resource_list, add it */
      if ( it2 == NULL )
      {
        npa_list_PushBack( &transaction->resource_list, ptr );
      }

      ptr = npa_list_PopFront( &dep_list );
    }

    it1 = it1->next;
  }

  /* Determine if this transaction shares resources with previously
     defined transactions */
  npa_list_Apply2( &npa_transaction_list, 
                   npa_transaction_find_resource_overlap, transaction );

  /* If find_resource_overlap has not set transaction->lock, create one */
  if ( transaction->lock == NULL )
  {
    CORE_VERIFY_PTR( 
         transaction->lock = Core_MutexCreate( CORE_MUTEXATTR_DEFAULT ) );
  }
  
  /* Add this transaction to the global list */
  npa_list_PushFront( &npa_transaction_list, transaction );

  Core_MutexUnlock( npa_transaction_lock );

  ULOG_RT_PRINTF_3( npa.log, "\t end npa_define_transaction "
                    "(handle: 0x%p) (transaction_name: \"%s\") "
                    "(attributes: %d)",
                    transaction, transaction->name, transaction->attributes );
}


/**
 * <!-- npa_define_transaction -->
 *
 * @brief Define a transaction with the list of resources that will be used in the transaction.
 * 
 * Creation of the transaction handle may be expensive. A transaction
 * handle can be reused for multiple transactions, although any single
 * transaction must be completed before another transaction can be
 * started on the same handle.  If the transaction is created and used
 * within the driver function of a (locked) resource, transaction
 * atomicity will be implicit.
 *
 * The resource array passed into this function must remain valid for
 * the life of the transaction structure.
 *
 * If defined with the DEFAULT or LAZY_LOCKING attribute, it is the
 * responsibility of the person defining the transaction to understand
 * the implications of leaving resources locked beyond the duration of
 * the resource request.  This typically means understanding the
 * dependencies associated with your requests and what things will remain
 * locked, and in particular avoid having a circular path to the same
 * underlying resource, as this introduces an opportunity for deadlock.
 * 
 * If a request is issued to a resource that was defined without
 * locking, the resource will not be locked, although this should not
 * be a problem as lockless resources are either side-effect free or
 * not intended to be generally accessed.
 *
 * Note: the list of resources are all the resources that may be part
 * of this transaction.  Any particular transaction does not
 * necessarily have to access all of the resources that are defined as
 * part of the transaction, but any resources that are not defined as
 * part of the transaction will not be considered part of the
 * transaction.
 *
 * @param transaction_name : name of the transaction, used for logging purposes
 * @param attributes : One or more from npa_transaction_attribute ORd together
 * @param num_resources : number of resources that in the transaction array
 * @param resources : array of resources that are part of the transaction
 *
 * @return the transaction handle for this transaction
 */
npa_transaction_handle npa_define_transaction( const char *transaction_name,
                                               unsigned int attributes,
                                               unsigned int num_resources, 
                                               const char *resources[] )
{
  npa_transaction_handle transaction;

  CORE_VERIFY_PTR( transaction_name );
  CORE_VERIFY( num_resources > 0 );
  CORE_VERIFY_PTR( resources );

  ULOG_RT_PRINTF_3( npa.log, "npa_define_transaction "
                    "(transaction_name: \"%s\") (attributes: 0x%08x) "
                    "(num_resources: %d)",
                    transaction_name, attributes, num_resources );

  CORE_VERIFY_PTR( 
    transaction = (npa_transaction *)Core_PoolAlloc( &npa.transaction_pool ) );
  memset( transaction, 0, sizeof( npa_transaction ) );

  transaction->name = transaction_name;
  transaction->attributes = attributes;

  /* Transactions may be defined before their resources are. Setup a callback
     to complete definition when resources become available */
  npa_resources_available_cb( num_resources, resources,
                              npa_transaction_resources_available,
                              (void *)transaction );

  return transaction;
}


/**
 * <!-- npa_destroy_transaction -->
 *
 * @brief  Destroy the transaction handle and free any internal data structures
 *         allocated for it.
 *
 * It is expected that this will not be a common operation, and that
 * typically transactions will be created when the resource is
 * initialized and are never destroyed.
 *
 * @param transaction : The transaction to be destroyed
 */
void npa_destroy_transaction( npa_transaction_handle transaction )
{
  CORE_VERIFY_PTR( transaction );
  CORE_VERIFY( npa_list_IsEmpty( &transaction->active_resources ) );
  CORE_VERIFY( npa_list_IsEmpty( &transaction->locked_resources ) );

  ULOG_RT_PRINTF_2( npa.log, "npa_destroy_transaction "
                    "(handle: 0x%p) (transaction_name: \"%s\") ",
                    transaction, 
                    transaction->name );

  npa_list_Clear( &transaction->resource_list );

  /* Lock is not reference-counting. For now, only destroy if not shared */
  if ( transaction->attributes & NPA_TRANSACTION_SHARED_LOCK )
  {
    Core_MutexDestroy( transaction->lock );
  }

  Core_PoolFree( &npa.transaction_pool, transaction );
}


/**
 * <!-- npa_transaction_lock_resource -->
 *
 * @brief Lock the resource, mark it forked, and add it to the transaction's locked resources list. 
 *
 * @param ptr1 : The npa_resource to lock
 * @param ptr2 : The transaction with the resource in it. 
 */
static void npa_transaction_lock_resource( void *ptr1, void *ptr2 )
{
  npa_resource *resource = (npa_resource *)ptr1;
  npa_transaction *transaction = (npa_transaction *)ptr2;

  Core_MutexLock( resource->node_lock );
  /* Add to the locked_resources list so that we unlock the right
     number of times at end_transaction */
  npa_list_PushFront( &transaction->locked_resources, resource );
}


/**
 * <!-- npa_begin_transaction -->
 *
 * @brief Begin a transaction on the transaction handle defined by npa_define_transaction
 * 
 * Begin a transaction on the given handle.  All resources in the
 * transaction that have an request issued on them will remain locked
 * until npa_end_transaction() is issued.
 *
 * If a remote resource is part of the transaction and the remote
 * protocol supports transactions, the resource request will be added
 * to the remote transaction.  The remote transaction is not guaranteed
 * to be issued until npa_end_transaction. Remote transaction behavior,
 * including ordering of remote requests, is protocol specific.
 *
 * If a request is issued to a resource that was not defined as part of
 * the transaction, the resource behavior is undefined (i.e., it may or
 * may not remain locked until the end of the transaction and it may or
 * may not be included in any remote transaction).
 *
 * Only one transaction may be outstanding on a given transaction
 * handle at any particular point in time.  It is an error to attempt
 * to begin a second transaction on a handle that has an outstanding
 * transaction in process.
 *
 * @param transaction : The transaction that is beginning
 */
void npa_begin_transaction( npa_transaction_handle transaction )
{
  npa_transaction_handle current;
  CORE_VERIFY_PTR( transaction );
  /* Ensure that transaction definition was complete before use */
  CORE_VERIFY_PTR( transaction->lock );

  current = npa_get_current_transaction();
  if ( current )
  {
    /* Disallow recursive begins of the same transaction */
    CORE_VERIFY( current != transaction );
    /* If nested transactions share resources with other transactions,
       a deadlock may result. TODO: Investigate try-locks */
    CORE_VERIFY( 
         ( transaction->attributes & NPA_TRANSACTION_SHARED_LOCK ) == 0 );
    
    ++current->depth;
  }
  else
  {
    Core_MutexLock( transaction->lock );
    Core_TlsSet( npa_transaction_key, transaction );

    /* Initialize fire_and_forget to 1; We "AND" this field with the result of
     * npa_has_request_attribute( NPA_REQUEST_FIRE_AND_FORGET ) for each
     * request in the transaction. At end_transaction, the field will be 1 
     * only if ALL requests in the transaction were fire-and-forget
     */
    transaction->fire_and_forget = 1;

    /* If we need to lock pessimistically, lock all resources in
       transaction->resource_list, in topsort order */
    if ( transaction->attributes & NPA_TRANSACTION_PESSIMISTIC_LOCKING )
    {
      npa_list_Apply2( &transaction->resource_list,
                       npa_transaction_lock_resource, transaction );
    }
  }

  ULOG_RT_PRINTF_4( npa.log, "npa_begin_transaction "
                    "(handle: 0x%p) (transaction_name: \"%s\") "
                    "(attributes: %d) (current: 0x%p)",
                    transaction, transaction->name,
                    transaction->attributes, current );
}


/**
 * <!-- npa_mark_resource_joined -->
 *
 * @brief Clear the NPA_RESOURCE_FORKED flag in the sequence. 
 *
 * @param ptr : The resource to mark. 
 */
static NPA_INLINE void npa_mark_resource_joined( npa_resource *resource )
{
  resource->sequence &= ~NPA_RESOURCE_FORKED;
}


/**
 * <!-- npa_transaction_unlock_resource -->
 *
 * @brief Unlock all resources that were held locked until end_transaction 
 *
 * @param ptr : The resource to unlock. 
 */
static void npa_transaction_unlock_resource( void *ptr )
{
  npa_resource *resource = (npa_resource *)ptr;

  resource->transaction = NULL;
  npa_mark_resource_joined( resource );
  Core_MutexUnlock( resource->node_lock );
}


/**
 * <!-- npa_transaction_request_serviced -->
 *
 * @brief Log request completes and post async completion events for all
 *        clients whose requests were pending until end_transaction 
 */
static void npa_transaction_request_serviced( void *ptr )
{
  npa_client *client = (npa_client *)ptr;
  client->sequence &= ~NPA_RESOURCE_FORKED;
  npa_request_serviced( client );
}


/**
 * <!-- npa_end_transaction -->
 *
 * @brief End a transaction on the transaction handle defined by npa_define_transaction
 * 
 * Complete the transaction that was issued. Completing a transaction
 * will issue any remote transactions that had been initiated on the
 * transaction handle and upon completing free all resource locks that
 * were acquired by the transaction.
 *
 * When this function returns, the transaction has completed and all
 * the underlying locks acquried by the transaction will have been
 * released.
 *
 * @param transaction : The transaction that is ending
 */
void npa_end_transaction( npa_transaction_handle transaction )
{
  npa_transaction_handle current;
  npa_list_header pending_clients = { 0 };

  CORE_VERIFY_PTR( transaction );
  CORE_VERIFY_PTR( current = npa_get_current_transaction() );
  if (current ) //KW fix
  {
      if ( current != transaction )
      {
        --current->depth;
      }
      else
      {
        npa_transaction_extension *ext;

        /* Verify that there are no nested transactions under current */
        CORE_VERIFY( transaction->depth == 0 );

        for (ext = transaction->extensions; 
             ext != NULL; 
             ext = (npa_transaction_extension *)ext->header.next_extension)
        {
          ext->end_transaction( transaction, ext );
        }
        transaction->extensions = NULL;

        npa_list_ApplyAndClear( &transaction->locked_resources,
                                npa_transaction_unlock_resource );

        npa_list_SwapLists( &transaction->pending_clients, &pending_clients );

        Core_TlsSet( npa_transaction_key, NULL );
        Core_MutexUnlock( transaction->lock );
      }

      ULOG_RT_PRINTF_4( npa.log, "npa_end_transaction "
                        "(handle: 0x%p) (transaction_name: \"%s\") "
                        "(current: 0x%p) (fire_and_forget: %d)",
                        transaction, transaction->name, current,
                        transaction->fire_and_forget );

      npa_list_ApplyAndClear( &pending_clients, 
                              npa_transaction_request_serviced );
  }
}


/**
 * <!-- npa_mark_resource_forked -->
 *
 * @brief Marks the resource as being in a pending state, because the current
 * request to it is still outstanding.
 *
 * Resource authors must call this function if their driver functions take
 * advantage of transactions to batch requests.
 *
 * @param resource : The resource in question
 */
void npa_mark_resource_forked( npa_resource *resource )
{
  CORE_VERIFY_PTR( resource );
  resource->sequence |= NPA_RESOURCE_FORKED;
}


/**
 * <!-- npa_is_resource_forked -->
 */ 
static NPA_INLINE
unsigned int npa_is_resource_forked( npa_resource *resource )
{
  return ( resource->sequence & NPA_RESOURCE_FORKED );
}


/**
 * <!-- npa_request_enter_transaction -->
 *
 * @brief  Add client->resource to the stack of resources in the current
 * transaction.
 */
void npa_request_enter_transaction( npa_transaction_handle transaction,
                                    npa_client_handle      client )
{
  npa_resource *resource = client->resource;

  /* Aggregate the fire-and-forget state */
  transaction->fire_and_forget &= \
    npa_request_has_attribute( client, NPA_REQUEST_FIRE_AND_FORGET );

    Core_MutexLock( resource->node_lock );

    resource->transaction = transaction;

  npa_list_PushFront( &transaction->active_resources, resource );
}


/**
 * <!-- npa_get_transaction_extension -->
 *
 * @brief Get the extension associated with the current transaction, if present.
 * 
 * Extensions define how the transaction is actually implemented.
 * 
 * @return return a pointer to the extension, or NULL
 */
npa_transaction_extension *
npa_get_transaction_extension( npa_transaction_handle transaction,
                               unsigned int           extension_id )
{
  return (npa_transaction_extension *) 
    npa_find_extension( &transaction->extensions->header, extension_id );
}


/**
 * <!-- npa_add_transaction_extension -->
 *
 * @brief add the extension to the start of the transaction linked list.
 *
 * @param transaction : The transaction to add the extension to.
 * @param extension : The extension to be added.
 */
void npa_add_transaction_extension( npa_transaction_handle     transaction,
                                    npa_transaction_extension *extension )     
{     
  extension->header.next_extension = transaction->extensions;     
  transaction->extensions = extension;     
} 


/**
 * <!-- npa_request_exit_transaction -->
 *
 * @brief  Remove client->resource from the stack of resources in the current transaction.
 *
 * If the resource is 'forked', leave the resource locked and mark the next
 * resource on the stack as 'forked'. Else, unlock the resource. 
 */
void npa_request_exit_transaction( npa_transaction_handle transaction,
                                   npa_client_handle      client )
{
  npa_resource *resource = client->resource;

    npa_resource *top = 
      (npa_resource *)npa_list_PopFront( &transaction->active_resources );

    CORE_VERIFY( top == resource );

    if ( npa_is_resource_forked( resource ) )
    {
      /* Mark the calling resource (the new top of the stack) as forked */
    top = (npa_resource *)npa_list_PeekFront( &transaction->active_resources );
      if ( top )
      {
        npa_mark_resource_forked( top );
      }
  
    /* We're leaving the resource locked; add to locked_resources list */
    npa_list_PushBack( &transaction->locked_resources, resource );

    npa_list_PushBack( &transaction->pending_clients, client );
    }
    else
    {
      resource->transaction = NULL;
      Core_MutexUnlock( resource->node_lock );
      npa_request_serviced( client );
    }
  }


/**
 * <!-- npa_is_transaction_nested -->
 *
 * @brief Returns TRUE if transaction is currently nested under another 
 *
 * @param transaction: The transaction to check
 */
unsigned int
npa_is_transaction_nested( npa_transaction_handle transaction )
{
  npa_transaction_handle current = npa_get_current_transaction();
  return ( current != NULL && transaction != current );
}

#ifdef __cplusplus
}
#endif
