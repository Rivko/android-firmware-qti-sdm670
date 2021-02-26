/*============================================================================
@file npa_internal.h

NPA data structures/functions shared internally. 

Copyright (c) 2010-2015, Qualcomm Technologies, Inc.
All Rights Reserved.

============================================================================*/
#ifndef NPA_INTERNAL_H
#define NPA_INTERNAL_H

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "npa.h"
#include "npa_resource.h"
#include "npa_extension.h"
#include "npa_transaction.h"
#include "npa_config.h"

#include "ULogFront.h"

#include "DALSys.h"

#include "CoreVerify.h"
#include "CorePool.h"
#include "CoreWorkContainer.h"
#include "CoreWorkQueue.h"
#include "CoreAtomicOps.h"
#include "CoreMutex.h"
#include "CoreTime.h"
#include "CoreHeap.h"
#include "CoreString.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

#define NPA_SEQUENCE_INCREMENT 0x100

/* Bit markers in resource->sequence and client->sequence */
#define NPA_REDUNDANT_REQUEST  0x1
#define NPA_RESOURCE_DISABLED  0x2
#define NPA_RESOURCE_FORKED    0x4

#ifndef MAX
#define MAX( a, b ) ((a)>(b)) ? (a) : (b)
#endif

#ifndef MIN
#define MIN( a, b ) ((a)<(b)) ? (a) : (b)
#endif

#ifndef INT64_MAX
#define INT64_MAX (9223372036854775807ll)
#endif

#ifndef UINT64_MAX
#define UINT64_MAX (18446744073709551615ull)
#endif

#if defined(__cplusplus)
  #define NPA_INLINE inline
  extern "C" {
#else
  #ifdef __clang__ 
      #define NPA_INLINE  
  #else
      #define NPA_INLINE __inline
  #endif
#endif

/* NPA internal event trigger type */
#define NPA_TRIGGER_RESOURCE_AVAILABLE  NPA_TRIGGER_RESERVED_EVENT_0
#define NPA_TRIGGER_SYSTEM_EVENT        NPA_TRIGGER_RESERVED_EVENT_1
#define NPA_TRIGGER_SYNC_EVENT          NPA_TRIGGER_RESERVED_EVENT_2

/* NPA internal event trigger bit flag */
#define NPA_TRIGGER_DISABLED_FLAG       NPA_TRIGGER_RESERVED_BIT_30
#define NPA_TRIGGER_DESTROY_FLAG        NPA_TRIGGER_RESERVED_BIT_31

/* NPA internal event type */
#define NPA_EVENT_SYSTEM                NPA_EVENT_RESERVED_1
#define NPA_EVENT_SYNC                  NPA_EVENT_RESERVED_2

/* NPA default stack size for Core/utils workloops */
#define NPA_DEFAULT_WL_STACKSIZE 0x2000  /* 8k stack size */

/* This is a node that is never defined but used to mark a resource as an
 * alias resource.  If resource->node is equal to this, then it is an
 * alias.
 */
extern npa_node_definition npa_aliased_node_def;
#define npa_aliased_node &npa_aliased_node_def

/*----------------------------------------------------------------------------
 * Enums
 * -------------------------------------------------------------------------*/
typedef enum
{
  NPA_UNDEFINED_RESOURCE,
  NPA_PLACEHOLDER_RESOURCE,
  NPA_DEFINED_RESOURCE,
  NPA_ALIAS_DEFINITION,
  NPA_INSTANTIATED_RESOURCE
} npa_resource_type;


/*----------------------------------------------------------------------------
 * Types/Typedefs
 * -------------------------------------------------------------------------*/

/* TODO: The list types should be in utils */
typedef struct npa_list_elem
{
  struct npa_list_elem *next;
  void                 *data;
} npa_list_elem;

typedef struct npa_list_header
{
  npa_list_elem *head;
  npa_list_elem *tail;
} npa_list_header;


typedef struct npa_callback_link
{
  struct npa_callback_link *next, *prev;
  npa_callback_event        cb_event;
} npa_callback_link;

typedef struct npa_struct
{
  /* memory pools */
  CorePoolType resource_pool;
  CorePoolType internal_pool;
  CorePoolType client_pool;
  CorePoolType event_pool;
  CorePoolType link_pool;
  CorePoolType list_pool;

/* Global list of registered resources
 * 
 * TODO: Perhaps this should be a hash table indexed on resource name?
 * Depends on the number of resources and lookups that happen... */
  npa_link      *resources;
  npa_link      *placeholders;

  CoreMutex     *graph_lock;
  CoreMutex     *event_lock;

  ULogHandle     log;

  /* Workloop for async requests */
  DALSYSWorkLoopHandle async_request_handle;

  /* Workloop for npa event processing */
  CoreContainerWorkLoopHandle npa_event_handle;

  /* Queue for async npa event processing */
  CoreWorkContainerHandle npa_event_queue_handle;

  /* Global list of wildcard resource available callbacks, matching any linked
     resources. This should be updated with the npa graph lock */
  npa_callback_link *wildcard_resource_available_callbacks;

  /* Handle to the "/npa/in_stm" resource; used to check if we are in STM */
  npa_resource *in_stm_resource;
} npa_struct;

extern npa_struct npa;
extern npa_resource marker_resource;
extern npa_resource global_marker_resource;

#define NPA_IS_RESOURCE_MARKER(resource) \
  ((resource == &marker_resource) || (resource == &global_marker_resource))

typedef struct npa_continuation
{
  npa_callback_event callback;

  union
  {
    struct npa_alias_continuation
    {
      const char        *resource_name;
      const char        *alias_name;
    } alias;

    struct npa_node_continuation
    {
      npa_node_definition *definition;
      unsigned int         dependency;
    } node;

    struct npa_resources_available_continuation
    {
      const char  **resources;
      unsigned int  num_resources;
      unsigned int  index;
    } resources;

    struct npa_client_request_continuation
    {
      DALSYSEventHandle    enqueue_event; /* DAL Event Handle for enquing
                                           * async requests */
      npa_resource_state  *vector;
    } client;

  } data;

} npa_continuation;

typedef struct npa_event_header
{
  npa_event             *event;      /* Event list */
  npa_event             *sys_event;  /* System event list */
  volatile unsigned int  reg_count;  /* Count of change/watermark/threshold events */
  volatile unsigned int  sys_count;  /* Count of system events */
  volatile unsigned int  sync_count; /* Count of sync events */
} npa_event_header;

/* Structs defining data associated with event triggers of various types */

typedef struct npa_set_watermark
{
  npa_event                *event;
  npa_resource_state_delta  hi;
  npa_resource_state_delta  lo;
  npa_resource_state        state;
  npa_resource_state_delta  headroom;
} npa_set_watermark;

typedef struct npa_set_threshold
{
  npa_event          *event;
  npa_resource_state  hi;
  npa_resource_state  lo;
  npa_resource_state  state;
  npa_resource_state_delta  headroom;
} npa_set_threshold;

typedef struct npa_event_trigger
{
  npa_resource             *resource;
  npa_resource_state        old_state;
  npa_resource_state        new_state;
  npa_resource_state_delta  old_headroom;
  npa_resource_state_delta  new_headroom;
} npa_event_trigger;

typedef struct npa_sync_event_trigger
{
  npa_event          *event;
  int                 valid; /* If the state field is valid or not */
  npa_resource_state  state;
} npa_sync_event_trigger;

typedef struct npa_custom_event_data
{
  npa_custom_event *event;
  void             *context;
} npa_custom_event_data;

/* Event action */
typedef struct npa_event_action
{
  /* function pointer for processing this event action */
  DALResult (*function) ( struct npa_event_action *action );
  union
  {
    npa_event             *event;
    npa_custom_event_data  custom;
    npa_set_watermark      watermark;
    npa_set_threshold      threshold;
    npa_event_trigger      trigger;
    npa_sync_event_trigger sync_trigger;
  } state;
} npa_event_action;

/* Structure to hold time statistics */
typedef struct
{
  uint64 max;
  uint64 count;
  uint64 total;
} npa_duration;

/* Anonymous structs are not portable so keep outside of
 * npa_resource_internal_data
 */
typedef struct npa_resource_internal_data_stats
{
  npa_duration driver_duration;
} npa_resource_internal_data_stats;

typedef struct npa_resource_internal_data
{
  /* Active resource attributes */
  unsigned int active_attr;

  /* time spent calling the resource driver */
  npa_resource_internal_data_stats stats;

} npa_resource_internal_data;

typedef struct npa_transaction_extension
{
  npa_extension_header header;

  /* This function is the destructor for the transaction
   * extension. When the user calls end_transaction, this function
   * will be invoked for every transaction extension add to the
   * transaction handle.  The transaction structure will not be
   * dereferenced after this function is called, so it can free */
  void (*end_transaction)( struct npa_transaction *transaction,
                           struct npa_transaction_extension *extension );
} npa_transaction_extension;

/* NPA Transaction */
typedef struct npa_transaction
{
  const char                *name;
  /* Attributes of this transaction */
  unsigned int               attributes;
  /* The aggregated fire-and-forget "state" of requests issued in a
   * transaction. Will be 1, at end_transaction, only if all requests
   * in the transaction were issued as fire-and-forget.
   */
  unsigned int               fire_and_forget;
  /* Prevents multiple instances from being run concurrently */
  CoreMutex                 *lock;
  /* Number of transactions nested under this one */
  unsigned int               depth;
  /* Each extension defines a 'flush' function invoked at end_transaction */
  npa_transaction_extension *extensions;
  /* List of resources provided when defining transaction */
  npa_list_header            resource_list;
  /* Stack of resources in the current request path in the transaction */
  npa_list_header            active_resources;
  /* Deferred unlock list of resources; unlocked at end_transaction */
  npa_list_header            locked_resources;
  /* Clients whose requests are pending until end_transaction */
  npa_list_header            pending_clients;
  /* Hook to hang data off a transaction; for when an extension is overkill */
  union
  {
    unsigned int  u;
    void         *p;
  } data;
} npa_transaction;

/*----------------------------------------------------------------------------
 * More defines (these cannot precede the type decls above)
 * -------------------------------------------------------------------------*/

/* alias some of the pools */
#define continuation_pool event_pool
CORE_STATIC_VERIFY( sizeof( npa_continuation ) <= sizeof( npa_event ) );

#define callback_pool link_pool
CORE_STATIC_VERIFY( sizeof( npa_callback_event ) <= sizeof( npa_link ) );
CORE_STATIC_VERIFY( sizeof( npa_callback_link ) <= sizeof( npa_link ) );
CORE_STATIC_VERIFY( sizeof( npa_event_header ) <= sizeof( npa_event ) );

#define latency_pool link_pool
CORE_STATIC_VERIFY( sizeof( npa_resource_latency ) <= sizeof( npa_link ) );

/* For naming consistency, while preserving BC */
#define npa_lock_resource( resource )     npa_resource_lock( resource )
#define npa_unlock_resource( resource )   npa_resource_unlock( resource )

/*----------------------------------------------------------------------------
 * Functions
 * -------------------------------------------------------------------------*/

/*
  npa_graph.c functions
*/

/**
 * <!-- npa_find_resource_link -->
 *
 */
npa_link *npa_find_resource_link( const char *resource_name );


/**
 * <!-- npa_find_resource -->
 *
 */
npa_resource *npa_find_resource( const char *resource_name );


/**
 * <!-- npa_define_node_continuation -->
 *
 * @brief Process the node creation continuation.
 * 
 * Create the node if all the dependencies exist and the node is not disabled. 
 * Otherwise re-queue processing of this continuation by passing this function
 * and the continuation to npa_resource_available_cb for a resource that is
 * preventing the creation. 
 *
 * @param user_data : A npa_continuation to evaluate
 * @param cb_event_type : Not used
 * @param cb_data : Not used
 * @param size : Not used
 */
void npa_define_node_continuation( void         *user_data, 
                                   unsigned int  cb_event_type,
                                   void         *cb_data, 
                                   unsigned int  size );


/**
 * <!-- npa_alias_resource_continuation -->
 *
 * @brief Process an alias resource creation continuation.
 *
 * @param user_data : A npa_continuation to process
 * @param cb_event_type : Not used
 * @param cb_data : The resource name
 * @param size : Not used
 */
void npa_alias_resource_continuation( void         *user_data, 
                                      unsigned int  cb_event_type,
                                      void         *cb_data, 
                                      unsigned int  size );


/**
 * <!-- npa_resources_available_continuation -->
 *
 * @brief Process a resource_available continuation 
 *
 * @param user_data : A npa_continuation to potentially process
 * @param cb_event_type : 
 * @param cb_data : Not used
 * @param size : Not used
 */
void npa_resources_available_continuation( void         *user_data, 
                                           unsigned int  cb_event_type,
                                           void         *cb_data, 
                                           unsigned int  size );


/**
 * <!-- npa_destroy_resource -->
 *
 */
void npa_destroy_resource( npa_resource *resource );


/**
 * <!-- npa_any_resource_available_cb -->
 *
 *  @brief Notify when any new resource is available.
 *
 *  Register an callback to be triggered (once) when any resource is 
 *  available and ready for use. The callback will be invoked once for 
 *  every existing resource during this function's execution.
 *
 *  When invoked, the callback arguments will be
 *  (context, 0, resource_name, strlen( resource_name ) );
 *
 *  @param callback: Callback funtion
 *  @param context: User specified callback context data
 */
void npa_any_resource_available_cb( npa_callback    callback,
                                    void           *context );

/*
  npa_resource.c
*/

void npa_issue_sync_request( npa_client_handle client, int new_request );

DALResult npa_issue_async_request( DALSYSEventHandle event, void *arg );

void npa_enqueue_async_request( npa_client_handle, int );

void npa_terminate_request( npa_client_handle client );

void npa_update_resource_state( npa_resource *resource,
                                npa_resource_state new_state );

void npa_request_serviced( npa_client *client );

void npa_issue_sync_request_unconditional( npa_client_handle client,
                                           int new_request );

void npa_enqueue_async_request_unconditional( npa_client_handle client,
                                              int new_request );

/*
  npa.c
*/

/* Vector logging limit */
extern unsigned int npa_vector_logging_limit;


/**
 * <!-- npa_new_continuation -->
 *
 * @brief Get an new continuation from the pool, and initialize it. 
 *
 * @param callback: Function to call when the continuation is processed.
 * @param context: Data passed to the callback. 
 */
npa_continuation *npa_new_continuation( npa_callback  callback, 
                                        npa_user_data context );


/**
 * <!-- npa_destroy_continuation -->
 *
 * @brief return the passed in continuation to the pool.
 *
 * @param continuation: Pointer to the continuation that is no longer in use. 
 */
void npa_destroy_continuation( npa_continuation *continuation );


/**
 * <!-- npa_init_pool -->
 *
 * @brief Used to setup pools for things like resources, clients, internal data, events, and links
 *
 * @param pool: Pointer where the completed pool will be placed.
 * @param npa_pool_config: Initial and subsequent number of elements in pool.
 * @param element_size: Size in bytes of the elements. 
 */
void npa_init_pool( CorePoolType          *pool, 
                    const npa_pool_config *config,
                    unsigned int           element_size );


/**
 * <!-- npa_calc_duration -->
 *
 * @brief Returns the unsigned time difference between two times. Optionally, updates stats.
 *
 * @param start, end: The two time stamps
 * @param dur_ptr: Pointer to stats-gathering npa_duration structure, or NULL
 * 
 * @return: end time - start time
 */
uint64 npa_calc_duration( uint64 start, uint64 end, npa_duration *dur_ptr );


/*
  npa_event.c functions
*/

/**
 * <!-- npa_new_event -->
 *
 * @brief Creates a new event.
 *
 * This is an internal function used to create and link an event for a 
 * resource.  If the event type is NPA_TRIGGER_RESOURCE_AVAILABLE it 
 * will create a placeholder to that resource.
 *
 * @param resource_name : Resource to create the event for.
 * @param event_name : Name of event to create.
 * @param trigger_type : The type of event that triggers the event.
 * @param callback : Function to call when the event is triggered.
 * @param context : Data pointer to pass to the function when it is called.
 *
 * @return The new event, else NULL
 */
npa_event *npa_new_event( const char      *resource_name,
                          const char      *event_name,
                          npa_event_trigger_type trigger_type,
                          npa_callback     callback,
                          void            *context );


/**
 * <!-- npa_trigger_callback_event -->
 *
 * @brief Call the callback with it's context, the event_type, data, 
 * and data_size as arguments. 
 *
 * @param cb_event : Pointer to a npa_callback_event
 * @param event_type : An npa_event_type value.
 * @param data : Pointer to data appropriate to be passed to the callback
 * @param data_size : Size of data in bytes.
 */
void npa_trigger_callback_event( npa_callback_event *cb_event, 
                                 unsigned int        event_type, 
                                 void               *data,       
                                 unsigned int        data_size );


/* Potential public API. Internal pending further review */

/**
 * <!-- npa_create_custom_event -->
 *
 * @brief  Create a custom event, a (callback, context) tuple that may be
 * arbitrarily posted by resource authors and dispatched in the event workloop
 *
 * @param name: Name of the custom event
 * @param callback: Callback to be invoked on dispatch, with context supplied
 * during post
 * @param data: data to associate with event at creation; will be 
 * accessible via event->data in callback
 *
 * @return The new event
 */
npa_custom_event* npa_create_custom_event( const char *name,
                                           npa_callback  callback,
                                           void         *data );

/**
 * <!-- npa_post_custom_event -->
 *
 * @brief Post a custom event to npa's npa_event_queue_handle
 *
 * @param event: The custom event
 * @param context: The context to post the event with
 */
void npa_post_custom_event( npa_custom_event *event, void *context );


/**
 * <!-- npa_post_custom_event_nodups -->
 *
 * @brief Post a custom event or if one is queued for dispatch already, simply
 * update the event in the dispatch queue
 *
 * @param event: The custom event to either post afresh or update in the
 * dispatch queue
 * @param context: The context to post or update the event with.
 * @param p_action: An opaque handle, representing an event action, that
 * is created when this function is first called and used in all subsequent
 * calls. Must be used as below.
 *
 * npa_event_action *action = NULL; // init to NULL is important
 * npa_post_custom_event_nodups( event, &action );
 * ...
 * npa_post_custom_event_nodups( event2, &action );
 */
void npa_post_custom_event_nodups( npa_custom_event  *event,
                                   void              *context,
                                   npa_event_action **p_action );

/**
 * <!-- npa_destroy_custom_event -->
 *
 * @brief Put a npa_custom_event_action_destroy on the npa event queue to destroy
 * the event asyncronously.
 *
 * npa_custom_event_action_destroy will free the event when it processes the
 * action.  The action allocated by Core_WorkQueueAlloc will be freed by the
 * Core_WorkElementProcess function. 
 *
 * @param event : The npa_custom_event that is no longer needed. 
 */
void npa_destroy_custom_event( npa_custom_event *event );


/**
 * <!-- npa_event_action_trigger -->
 *
 * @brief Trigger an event action  
 *
 * Internal function called by _npa_update_resource_state whenever
 * there are events for a given resource and the overall aggregation
 * or headroom changes.  This function goes through all of the events
 * on a resource and will post the event's callback to the event WL if
 * the event should be triggered.
 *
 * @param action: Action to post
 *
 * @return DAL_SUCCESS if successful.
 */
DALResult npa_event_action_trigger( struct npa_event_action *action );


/** 
 * <!-- npa_post_sync_event -->
 *
 * @brief Post a sync event to event work queue  
 *
 * @param event: Handle to event 
 * @param valid: If the passed-in resource state is valid or not
 * @param state: New resource state
 */
void npa_post_sync_event( npa_event *event, int valid, npa_resource_state state );


/*
  npa_client.c functions
*/

/**
 * <!-- npa_client_type_name -->
 *
 * @brief Get an ascii name of the client type. 
 *
 * @returns a pointer to a text literal for the npa_client_type 
 */
const char *npa_client_type_name( npa_client_type client_type );


/*
  npa_transaction.c functions
*/

/**
 * <!-- npa_transaction_init -->
 *
 * @brief Initialize support for NPA transactions. 
 */
void npa_transaction_init( void );


/**
 * <!-- npa_request_enter_transaction -->
 *
 * @brief  Add client->resource to the stack of resources in the current
 * transaction and mark client as 'in a transaction'.
 */
void npa_request_enter_transaction( npa_transaction_handle transaction,
                                    npa_client_handle      client );


/**
 * <!-- npa_request_exit_transaction -->
 *
 * @brief  Remove client->resource from the stack of resources in the current transaction.
 *
 * If the resource is 'forked', leave the resource locked and mark the next
 * resource on the stack as 'forked'. Else, unlock the resource. 
 */
void npa_request_exit_transaction( npa_transaction_handle transaction,
                                   npa_client_handle      client );


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
                               unsigned int extension_id );


/**
 * <!-- npa_add_transaction_extension -->
 *
 * @brief add the extension to the start of the transaction linked list.
 *
 * @param transaction : The transaction to add the extension to.
 * @param extension : The extension to be added.
 */
void npa_add_transaction_extension( npa_transaction_handle transaction,
                                    npa_transaction_extension *extension );


/* 
  npa_list.c functions
*/

/**
 * <!-- npa_list_Init -->
 *
 * @brief Initialize list support 
 */
void npa_list_Init( const npa_pool_config *config );


/**
 * <!-- npa_list_PushFront -->
 *
 * @brief Insert an element at the head of the list 
 */
void npa_list_PushFront( npa_list_header *list, void *data );


/**
 * <!-- npa_list_PopFront -->
 *
 * @brief Pop an element off the head of the list 
 */
void *npa_list_PopFront( npa_list_header *list );


/**
 * <!-- npa_list_PushBack -->
 *
 * @brief Append an element to the tail of the list 
 */
void npa_list_PushBack( npa_list_header *list, void *data );


/**
 * <!-- npa_list_Find -->
 *
 * @brief Find an element 'data' in the list 
 */
void* npa_list_Find( npa_list_header *list, void *data );


/**
 * <!-- npa_list_Remove -->
 *
 * @brief Find and delete an element 'data' in the list 
 */
void npa_list_Remove( npa_list_header *list, void *data );


/**
 * <!-- npa_list_Apply -->
 *
 * @brief Invoke function 'fcn( element )' on each element in the list 
 */
void npa_list_Apply( npa_list_header *list, void (*fcn)( void *data) );


/**
 * <!-- npa_list_Apply2 -->
 *
 * @brief Invoke function 'fcn( element, data2 )' on each element in the list 
 */
void npa_list_Apply2( npa_list_header *list, 
                      void (*fcn)(void *data, void *data2),
                      void *data2 );


/**
 * <!-- npa_list_Clear -->
 *
 * @brief Clear the list; delete all elements 
 */
void npa_list_Clear( npa_list_header *list );


/**
 * <!-- npa_list_ApplyAndClear -->
 *
 * @brief Clear the list after invoking function 'fcn( element )' on each element in the list 
 */
void npa_list_ApplyAndClear( npa_list_header *list, void (*fcn)(void *data) );


/**
 * <!-- npa_list_SwapLists -->
 *
 * @brief Swap headers of two lists, such that list1 becomes list2 and vice versa 
 */
void npa_list_SwapLists( npa_list_header *list1, npa_list_header *list2 );


/**
 * <!-- npa_list_IsEmpty -->
 *
 * @return True if list is empty 
 */
unsigned int npa_list_IsEmpty( npa_list_header *list );


/**
 * <!-- npa_list_PeekFront -->
 *
 * @brief Get the head of the list without removing it. 
 *
 * @return element at the head of the list, without popping it off the list 
 */
void *npa_list_PeekFront( npa_list_header *list );

/*
  npa_query.c
*/

/**
 * @brief npa_query_resource - Query a resource via the resource handle
 *
 * Note that due to concurrency, the resource may no longer be in the
 * returned state.

 * @param resource : resource being queried
 * @param id : type of query being issued
 * @param result : result from the query
 *
 * @return success/failure of query (npa_query_status)
 *
 */
npa_query_status npa_query_resource( npa_resource    *resource, 
                                     uint32           id, 
                                     npa_query_type  *result );


/*
  npa_log.c functions
*/

/**
 * <!-- npa_add_client_log_by_handle -->
 *
 * @brief Copies the npa logging messages associated with an npa client to a separate log
 *
 * @param ulog_name: Name of the log to which the messages should go, the name
 * must follow the ulog naming restrictions
 * @param client: handle to the client that is of interest
 *
 * @return success/failure (npa_status)
 */
npa_status npa_add_client_log_by_handle( char *log_name, 
                                         npa_client_handle client );


/**
 * <!-- npa_remove_client_log_by_handle -->
 *
 * @brief Stops copying messages associated with an npa client to the named log
 *
 * Note: Client log is a superset of its resoruce log. It is not allowed to 
 * remove a resource log by calling this funtion.
 *
 * @param ulog_name: Name of the log, which must follow the ulog naming
 * restrictions
 * @param client: Handle to the client that is of interested
 *
 * @return success/failure (npa_status)
 */
npa_status npa_remove_client_log_by_handle( char *log_name, 
                                            npa_client_handle client );

/**
 * <!-- npaScheduler task -->
 * 
 * @brief Wait on the npa_scheduler.events_array and process npa_scheduler_action_types
 *
 * @param unused_param: Parameter received from Main Control task - ignored
 */
void npaScheduler_task(unsigned long int unused_param);

#ifdef __cplusplus
}
#endif

#endif /* NPA_INTERNAL_H */
