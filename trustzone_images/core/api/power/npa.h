#ifndef NPA_H
#define NPA_H

/*==============================================================================
  @file npa.h

  NPA client API 
  This is not the API for authoring nodes.

  The interface uses strings and opaque types.  This is done
  intentionally to minimize client dependencies on the implementation.
  The data structures and implementation can change radically without
  requiring recompilation of client code

  Copyright (c) 2009 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //components/rel/core.tz/2.0.2/api/power/npa.h#1 $ */


/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "DALSys.h"

/*----------------------------------------------------------------------------
 * Enumerations
 * -------------------------------------------------------------------------*/

/* Client work models */
typedef enum
{
  NPA_NO_CLIENT          = 0x7fffffff, /* Do not create a client */
  NPA_CLIENT_RESERVED1   = (1 << 0), /* Reserved client type */
  NPA_CLIENT_RESERVED2   = (1 << 1), /* Reserved client type */
  NPA_CLIENT_CUSTOM1     = (1 << 2), /* Custom client types - can be */
  NPA_CLIENT_CUSTOM2     = (1 << 3), /* customized by resource */
  NPA_CLIENT_CUSTOM3     = (1 << 4), 
  NPA_CLIENT_CUSTOM4     = (1 << 5), 
  NPA_CLIENT_REQUIRED    = (1 << 6), /* Request must honor request */
  NPA_CLIENT_ISOCHRONOUS = (1 << 7), /* Placeholder for isochronous work model */
  NPA_CLIENT_IMPULSE     = (1 << 8), /* Poke the resource - node defines
                                      * the impulse response */
  NPA_CLIENT_LIMIT_MAX   = (1 << 9), /* Set maximum value for the resource */
  NPA_CLIENT_VECTOR      = (1 << 10) /* Vector request */
} npa_client_type;


/* Event types */
typedef enum
{
  NPA_EVENT_RESERVED,                 /* Reserved event */
  NPA_EVENT_HI_WATERMARK,       /* Resource passed hi watermark */
  NPA_EVENT_LO_WATERMARK,       /* Resource passed lo watermark */
  NPA_EVENT_CHANGE,             /* Resource changed state */
  NPA_NUM_EVENT_TYPES
} npa_event_type;


/* Resource independent maximum value
 *
 * Note: This will not work for resources with signed state values.
 * This may need to be revisited.  
 */
#define NPA_MAX_STATE 0xffffffff

/* Public Resource Query IDs
 *
 * Note: All the public resource queries return a type of
 * npa_resource_state  */
enum {
  /* Query the current state of the resource */
  NPA_QUERY_CURRENT_STATE,    
  /* Query the active maximum state supported by the resource. The
   * active max may be lower than the resource maximum due to
   * limit_max request */
  NPA_QUERY_ACTIVE_MAX, 
  /* Query the absolute max of the resource.  This may be greater than
   * the resource's active max if there are active limit_max
   * requests  */
  NPA_QUERY_RESOURCE_MAX,

  /* End public resource query space */
  NPA_MAX_PUBLIC_QUERY     = 1023,

  /* Reserved section of query space */
  NPA_QUERY_RESERVED_BEGIN = 1024,
  NPA_QUERY_RESERVED_END   = 4095

  /* Resource-specific query ids beyond this */
};

/* Query Status */
typedef enum {
  NPA_QUERY_SUCCESS = 0,
  NPA_QUERY_UNSUPPORTED_QUERY_ID,
  NPA_QUERY_UNKNOWN_RESOURCE,
} npa_query_status;

/*----------------------------------------------------------------------------
 * Types/Typedefs
 * -------------------------------------------------------------------------*/
/* Work request types */
typedef unsigned int npa_resource_state; 
typedef          int npa_resource_state_delta; 
typedef unsigned int npa_resource_time;

/* Callback Type */
typedef void ( *npa_callback )( void         *context,
                                unsigned int  event_type, 
                                void         *data,       
                                unsigned int  data_size );

/* Handle to the client */
typedef struct npa_client * npa_client_handle;

/* Event structure */
typedef struct npa_event * npa_event_handle;

/* Event data structure - passed to npa_event callback */
typedef struct npa_event_data
{
  const char               *resource_name;
  npa_resource_state        state;
  npa_resource_state_delta  headroom;
} npa_event_data;

/* Query handle to the resource */
typedef struct npa_query * npa_query_handle;

/* Query Type
 *
 * Mulitple types can be passed via the query API. The enum defines
 * which type is in the union */
typedef enum
{
   NPA_QUERY_TYPE_STATE,
   NPA_QUERY_TYPE_VALUE,
   NPA_QUERY_TYPE_NAME,
   NPA_QUERY_TYPE_REFERENCE,

   NPA_QUERY_NUM_TYPES
} npa_query_type_enum;

/* The actual type being passed to the query API */
typedef struct npa_query_type
{
  npa_query_type_enum type;
  union
  {
    npa_resource_state  state;
    unsigned int        value;
    char               *name;
    void               *reference;
  } data;
} npa_query_type;

/* Maximum NPA name string length */
#define NPA_MAX_STR_LEN 63

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * NPA Client API
 * -------------------------------------------------------------------------*/


/**

   @brief npa_create_sync_client_ex - Creates a synchronous client of a
   resource

   Create a synchronous client of the given resource with the given
   work model.  The new client will not be created with have an
   initial request. Work requests made via syncronous clients will be
   fully processed before returning.

   If the named resource does not exist or it exists but doesn't
   support the requested client_type, a NULL handle will be
   returned. Callers are expected to check for NULL handles.

   A client must never attempt to use a NULL handle to update a
   resource.

   There are two resource-optional arguments, client_value (an
   unsigned int) and client_ref( a void *). The meaning of these
   arguments, if any, is defined by the resource. If unnecessary, 0 &
   NULL can be passed as these arguments.  Note that the macro
   npa_create_sync_client can be used for this case.

   The client_name must be < 64 characters in length.

   The client_name string is not copied.  Any string pointer passed to
   the framework must be immutable. It is hoped that future versions
   will relax this restriction.  

   @param resource_name : name of the resource that is to be used
   @param client_name : name of the client that's being created
   @param client_type : the work model that this client will use for
   work requests
   @param client_value : resource-specific value argument
   @param client_ref : resource-specific pointer argument
   @return Handle to new client, NULL if client could not be created
*/
npa_client_handle npa_create_sync_client_ex( const char       *resource_name, 
                                             const char       *client_name,
                                             npa_client_type   client_type,
                                             unsigned int      client_value,
                                             void             *client_ref );

/**

   @brief npa_create_sync_client - Creates a synchronous client of a
   resource

   Create a synchronous client of the given resource with the given
   work model.  The new client will not be created with have an
   initial request. Work requests made via syncronous clients will be
   fully processed before returning.

   If the named resource does not exist or it exists but doesn't
   support the requested client_type, a NULL handle will be
   returned. Callers are expected to check for NULL handles.

   A client must never attempt to use a NULL handle to update a
   resource.

   The client_name must be < 64 characters in length.

   The client_name string is not copied.  Any string pointer passed to
   the framework must be immutable. It is hoped that future versions
   will relax this restriction.  

   @param resource_name : name of the resource that is to be used
   @param client_name : name of the client that's being created
   @param client_type : the work model that this client will use for
   work requests

   @return Handle to new client, NULL if client could not be created
*/
#define npa_create_sync_client( resource_name, client_name, client_type ) \
  npa_create_sync_client_ex( resource_name, client_name, client_type, 0, NULL )

/**
   @brief npa_create_async_client_cb - Creates an asynchronous client of
   a resource

   Create an asynchronous client of the given resource with the given
   work model.  Requests made via an asynchronous client will be
   processed asynchronously. The new client will not be created with an
   initial request.

   If the named resource does not exist or it exists but doesn't
   support the requested client_type, a NULL handle will be
   returned. Callers are expected to check for NULL handles.
   
   A client must never attempt to use a NULL handle to update a
   resource.

   There are two resource-optional arguments, client_value (an
   unsigned int) and client_ref( a void *). The meaning of these
   arguments, if any, is defined by the resource. If unnecessary, 0 &
   NULL can be passed as these arguments.  Note that the macro
   npa_create_sync_client can be used for this case.

   The client_name must be < 64 characters in length.

   The client_name string is not copied.  Any string pointer passed to
   the framework must be immutable. It is hoped that future versions
   will relax this restriction.  

   If non-NULL, the npa_callback will be triggered when the
   asynchronous request has completed.  when invoked, the arguments
   will be (context, 0, client_handle, sizeof(client_handle) );

   Restrictions: 

   In general, the same client can not have multiple simultaneous
   requests issued.  It is the responsibility of the user to make sure
   the previous request has completed before issuing a new request via
   the same client.
   
   Due to current limitations in DALSYS, async client requests can not
   be issued while INTLOCK'd.1

   It is hoped that both these restrictions will be removed in future
   versions.

   @param resource_name : name of the resource that is to be used
   @param client_name : name of the client that's being created 
   @param client_type : the work model that this client will use for
   work requests

   @param callback : callback to be invoked when work
   request is complete
   @param context : context to be passed back in callback invocation

   @param client_value : resource-specific value argument
   @param client_ref : resource-specific pointer argument

   @return Handle to new client, NULL if client could not be created

*/
npa_client_handle 
npa_create_async_client_cb_ex( const char       *resource_name, 
                               const char       *client_name,
                               npa_client_type   client_type,
                               npa_callback      callback,
                               void             *context, 
                               unsigned int      client_value,
                               void             *client_ref );


/**
   @brief npa_create_async_client_cb - Creates an asynchronous client of
   a resource

   Create an asynchronous client of the given resource with the given
   work model.  Requests made via an asynchronous client will be
   processed asynchronously. The new client will not be created with an
   initial request.

   If the named resource does not exist or it exists but doesn't
   support the requested client_type, a NULL handle will be
   returned. Callers are expected to check for NULL handles.
   
   A client must never attempt to use a NULL handle to update a
   resource.

   The client_name must be < 64 characters in length.

   The client_name string is not copied.  Any string pointer passed to
   the framework must be immutable. It is hoped that future versions
   will relax this restriction.  

   If non-NULL, the npa_callback will be triggered when the
   asynchronous request has completed.  when invoked, the arguments
   will be (context, 0, client_handle, sizeof(client_handle) );

   Restrictions: 

   In general, the same client can not have multiple simultaneous
   requests issued.  It is the responsibility of the user to make sure
   the previous request has completed before issuing a new request via
   the same client.
   
   Due to current limitations in DALSYS, async client requests can not
   be issued while INTLOCK'd.

   It is hoped that both these restrictions will be removed in future
   versions.

   @param resource_name : name of the resource that is to be used
   @param client_name : name of the client that's being created 
   @param client_type : the work model that this client will use for
   work requests

   @param callback : callback to be invoked when work
   request is complete
   @param context : context to be passed back in callback invocation

   @return Handle to new client, NULL if client could not be created

*/
#define npa_create_async_client_cb( resource_name, client_name, client_type, callback, context )         \
  npa_create_async_client_cb_ex( resource_name, \
                                 client_name, client_type,      \
                                 callback, context,             \
                                 0, NULL )

/**

   @brief npa_destroy_client - Destroy the client.  

   This will cancel the client request (if any) and free the client
   structure. The client handle can not be used to issue requests after
   being destroyed

   @param client : handle to client being destroyed

*/
void npa_destroy_client( npa_client_handle client );


/**
   @brief npa_issue_required_request - Issue the REQUIRED client work
   request

   Resource state should be updated to account for new request. 
   
   The client must be of REQUIRED work model
  
   @param client : handle to client being used
   @param state : requested resource state
*/
void npa_issue_required_request( npa_client_handle   client, 
                                 npa_resource_state  state );


/**
   @brief npa_modify_required_request - Modify the existing REQUIRED
   client work request

   This is intended to be used in a feedback loop, so the client can
   request incrementally more (or less) resource 

   Resource state should be updated to account for new request. 
   
   The client must be of REQUIRED work model
  
   @param client : handle to client being used
   @param delta : modifier to resource state
*/
void npa_modify_required_request( npa_client_handle         client, 
                                  npa_resource_state_delta  delta );

/**
    @brief npa_issue_impulse_request - Issue an IMPULSE work request 

    The client must be of IMPULSE work model

    The resource defines the impulse response

    Resource state should be updated to account for new request

    @param client : handle to client being used
*/
void npa_issue_impulse_request( npa_client_handle client );

/**
   @brief npa_issue_vector_request - Issue the VECTOR client work
   request

   Resource state should be updated to account for new request.

   The vector pointermust remain valid for the duration of the
   requrest, but not beyond.  If the resource needs the data to
   persist, it is the repsonsibility of the resource to copy the data.

   The interpretation of the vector elements is defined by the
   resource.
   
   The client must be of VECTOR work model
  
   @param client : handle to client being used
   @param num_elems : number of elements in the vector
   @param vector : array of num_elems
*/
void npa_issue_vector_request( npa_client_handle   client, 
                               unsigned int        num_elems, 
                               npa_resource_state *vector );

/**
   @brief npa_issue_isoc_request -  Issue an ISOCHRONOUS work request 

   The client must be of ISOCHRONOUS work model

   The client must issue npa_complete_request() when the work is completed
   
   Resource state should be updated to accound for new work
   Note: The hint may be overriden/ignored by the resource

   @param client : handle to client being used
   @param deadline : the interval (in ms) when the work must be completed
   @param level_hint : a hint as to what an appropriate level might
   be. May be overridden by resource
*/
void npa_issue_isoc_request( npa_client_handle   client,
                             npa_resource_time   deadline,
                             npa_resource_state  level_hint );


/**
   @brief npa_issue_limit_max_request - Issue the LIMIT_MAX  request

   If there are multiple concurrent max requests, the minimum of the
   requests will be used.

   The client must be of LIMIT_MAX work model.

   Note: The behavior of concurrent LIMIT_MAX and FIXED request or
   LIMIT_MAX < LIMIT_MIN requests is undefined
 
   @param client : handle to client being used
   @param max : maximum state of resource
*/
void npa_issue_limit_max_request( npa_client_handle  client,
                                  npa_resource_state max );


/**
   @brief npa_complete_request - Complete the work request

   This is used to notify the resource that the work request is
   complete and the client no longer needs the resource

   Any request state that has been learned will persist
   
   Resource state should be updated to account for completed work

   @param client : handle to client being used

*/
void npa_complete_request( npa_client_handle client );


/**
   @brief npa_cancel_request - Cancel the work request

  This can be used by any work model to terminate the work request.
  This internally invokes npa_complete_request and then clears any
  adapted state

  This function is used when future client requests will be
  qualitatively different than previous requests made via this client

   Resource state should be updated to account for terminated work

   @param client : handle to client being used
*/
void npa_cancel_request( npa_client_handle client );


/*============================================================================
  NPA Event API

  NPA Events are invoked asynchronously to the resource update and will be 
  invoked in a serial manner to avoid requiring event handlers for a single 
  resource to be reentrant. 

  If the same event function is registered with multiple resources, there is 
  no guarantee that the event function will not be invoked from multiple 
  threads and it is the responsibility of the person registering the event 
  handler to deal with reentrancy 
============================================================================*/


/**
   @brief npa_create_change_event_cb - Create a change event handler with
   the given resource

   This will return a NULL handle if the resource does not exist or
   the event handler could not be created.

   The event_name must be < 64 characters in length.

   The event_name string is not copied.  Any string pointer passed to
   the framework must be immutable. It is hoped that future versions
   will relax this restriction.  

   The callback pointer must be non-NULL.  When invoked, the callback
   arguments will be ( context, event_type, npa_event_data *, sizeof(
   npa_event_data) )

   @param resource_name : name of the resource that is to be used
   @param event_name : name of the event handler that's being created 
   @param callback : callback to be invoked when event is triggered
   @param context : context to be passed back in callback invocation

   @return New event handle, NULL if the event handle could not be created
*/
npa_event_handle 
npa_create_change_event_cb( const char   *resource_name, 
                            const char   *event_name,
                            npa_callback  callback,
                            void         *context );

/**
   @brief npa_create_watermark_event_cb - Create a watermark event
   handler with the given resource

   The event handler will not be invoked until the watermarks have
   been set

   The event_name must be < 64 characters in length.

   The event_name string is not copied.  Any string pointer passed to
   the framework must be immutable. It is hoped that future versions
   will relax this restriction.  

   The callback pointer must be non-NULL.  When invoked, the callback
   arguments will be ( context, event_type, npa_event_data *, sizeof(
   npa_event_data) )

   @param resource_name : name of the resource that is to be used
   @param event_name : name of the event handler that's being created 
   @param callback : callback to be invoked when event is triggered
   @param context : context to be passed back in callback invocation

   @return New event handle, NULL if the event handle could not be created
*/
npa_event_handle 
npa_create_watermark_event_cb( const char   *resource_name, 
                               const char   *event_name,
                               npa_callback  callback,
                               void         *context );


/**
   @brief npa_set_event_watermarks - Set the watermarks for triggering
   the event handler.

   Watermarks are defined as the difference between the aggregated
   request value and the resource maximum.  Headroom > 0 means the
   resource is oversubscribed, as the request value is greater than
   the resource can provide.

   For example: If the caller wanted to be notified when the resource
   state rose past qthe hi watermark of (max-20) units or dropped below
   the low watermark of (max-40) units, the hi and lo watermarks would
   be set to -20 and -40, respectively

   If the user wishes to recieve notification when the resource is
   oversubscribed, set both the hi and lo watermark to zero.  Then the
   user will receive a NPA_EVENT_HI_WATERMARK notification when the
   resource is oversubscribed and NPA_EVENT_LO_WATERMARK when the
   resource is operating normally.
   
   No watermark event will be triggered until the watermarks have been
   set.  If the lo watermark > hi watermark, no watermark events will
   be triggered.

   The event may be triggered during this function if the resource is
   below the lo watermark or above the hi watermark when this function
   is invoked
   
   @param event : handle of event being manipulated
   @param hi_watermark : hi watermark setting
   @param lo_watermark : lo watermark setting

*/
void npa_set_event_watermarks( npa_event_handle         event,
                               npa_resource_state_delta hi_watermark,
                               npa_resource_state_delta lo_watermark );


/**
    @brief npa_destroy_event_handle - Destroy the event handler
    
    After invocation, the event handle is invalidated and must not be
    reused

    @param event : handle of event being destroyed
*/
void npa_destroy_event_handle( npa_event_handle event );


/*============================================================================
  NPA Query API

  NPA Queries are used to query the underlying resource state
============================================================================*/

/**
   @brief npa_create_query_handle - Create a query handle to a resource 
   
   @param resource_name : name of resource being queried

   @return Query handle to resource
*/
npa_query_handle npa_create_query_handle( const char * resource_name );



/**
   @brief npa_destroy_query_handle - Destroy the query handle

   Only query handles that have been explicitly created via
   npa_create_query_handle need to be explicitly destroyed

   @param query : Query handle to destroy
*/

void npa_destroy_query_handle( npa_query_handle query );

/**
   @brief npa_query - Query the current state of the resource

   Note that due to concurrency, the resource may no longer be in the
   returned state.

   @param query : query handle to resource being queried
   @param query_id : type of query being issued
   @param query_result : value from the query

   @return success/failure of query (npa_query_status)

*/
npa_query_status npa_query( npa_query_handle query, 
                            uint32           query_id, 
                            npa_query_type  *query_result );


/** @brief npa_query_by_name - Query the current state of the named
   resource

   This is a convenience function that wraps the

   h = npa_create_query_handle(...);
   npa_query(h, ...);
   npa_destroy_query_handle(h);

   sequence. If you are making mulitple queries to the same resource,
   calling this function multiple times will have a poorer performance
   relative to managing the query handle directly.

   Note that due to concurrency, the resource may no longer be in the
   returned state.

   @param resource_name : name of resource being queried
   @param query_id : type of query being issued
   @param query_result : value from the query

   @return success/failure of query (npa_query_status)

*/
npa_query_status npa_query_by_name( const char      *resource_name, 
                                    uint32           query_id, 
                                    npa_query_type  *query_result );

/**
   @brief npa_query_by_client - Query a resource via a client to that resource

   Note that due to concurrency, the resource may no longer be in the
   returned state.

   @param client : client of resource being queried
   @param query_id : type of query being issued
   @param query_result : value from the query

   @return success/failure of query (npa_query_status)

*/
npa_query_status npa_query_by_client( npa_client_handle  client, 
                                      uint32             query_id, 
                                      npa_query_type    *query_result );


/**
   @brief npa_query_by_event - Query a resource via an event of that resource

   Note that due to concurrency, the resource may no longer be in the
   returned state.

   @param event : event of resource being queried
   @param query_id : type of query being issued
   @param query_result : value from the query

   @return success/failure of query (npa_query_status)

*/
npa_query_status npa_query_by_event( npa_event_handle   event, 
                                     uint32             query_id, 
                                     npa_query_type    *query_result );

/**
   @brief npa_query_resource_available - Query if resource is currently
   available 

   Query and return if the given resource is currently available.  If
   the resource is not currently available, this does not mean that it
   may not become available at a future time. If you wish to be
   notified when the resource becomes available, use
   npa_resource_available_cb() to register a callback to be invoked
   when the resource becomes available.

   @param resource_name :  name of the resource that's being queried

   @return NPA_QUERY_SUCCESS if resource is currently available,
   NPA_QUERY_UNKNOWN_RESOURCE if resource is not currently available
*/
npa_query_status npa_query_resource_available( const char *resource_name );

/**
   
   @brief npa_resources_available_cb - Notify when all of a list of
   resources have all become available to be used

   This function is a helper function to block on the availability of
   mutiple rsources at once.  The resources array passed in must
   remain valid until the callback is invoked.

   When invoked, the callback arguments will be 
   (context, 0, 
   *resource_name[num_resources], 
   strlen( *resource_name[num_resources] ) ); 

   @param num_resources : number of resources that are being queried
   @param resources : array of  resources that are being queried 

   @param callback : callback funtion to invoke when resource is
   available

   @param context : user registered pointer to be supplied to callback
   when it is invoked
*/

void npa_resources_available_cb( unsigned int    num_resources,
                                 const char     *resources[],
                                 npa_callback    callback,
                                 void           *context );


/**
   
   @brief npa_resource_available_cb - Notify when resource is available
   to be used

   Register an callback to be triggered (once) when the given resource
   is available and ready for use.  The callback may be invoked during
   this function's execution if the resource is already available when
   the function is invoked.

   When invoked, the callback arguments will be
   (context, 0, *resource_name, strlen( resource_name ) );

   @param resource_name : name of the resource that's being queried

   @param callback : callback funtion to invoke when resource is
   available

   @param context : user registered pointer to be supplied to callback
   when it is invoked
*/

void npa_resource_available_cb( const char     *resource_name,
                                npa_callback    callback,
                                void           *context );

/**
   
   @brief npa_dal_event_callback - Callback function that translates
   npa callbacks into dal_event triggers.

   This function can be registered as the NPA callback, with the
   DALSYSEventHandle as the context argument, and when invoked it will
   trigger the dal event.  The other input arguments will be passed on to
   the dal event without translation.

   @param dal_event_handle : dal_event_handle (passed in as context)

   @param event_type : int

   @param data : pointer to callback data
   
   @param data_size : size of void *
*/
void npa_dal_event_callback( void         *dal_event_handle,
                             unsigned int  event_type, 
                             void         *data,       
                             unsigned int  data_size );


/**
   
   @brief NPA_DAL_CALLBACK - Macro that takes the dal event handle and
   returns the npa_dal_event_callback and dal event for easy
   registration in the npa_*_cb functions

   If the dal event is NULL, a NULL callback will be registered.  Note
   that some callback functions require a non-NULL callback, and will
   generate an ERR_FATAL if you attempt to register a NULL callback

   This is primarily intended for use in the compatibility macros used
   to map the legacy DAL event API onto the direct callback API, but
   it can also be used independently.

   @param dal_event_handle : dal_event_handle (passed in as context)
*/
#define NPA_DAL_CALLBACK( dal_event_handle ) \
  ((NULL == (dal_event_handle)) ? NULL : npa_dal_event_callback), \
    (dal_event_handle)

/* Legacy API - invoke callback functions with dal_event callback.
 *
 * These calls are deprecated and should not be used by new code
 */



/**
   @brief npa_create_async_client_dal - Creates an asynchronous client of
   a resource

   Create an asynchronous client of the given resource with the given
   work model.  Requests made via an asynchronous client will be
   processed asynchronously. The new client will not be created with an
   initial request.

   If the named resource does not exist or it exists but doesn't
   support the requested client_type, a NULL handle will be
   returned. Callers are expected to check for NULL handles.
   
   A client must never attempt to use a NULL handle to update a
   resource.

   The client_name must be < 64 characters in length.

   The client_name string is not copied.  Any string pointer passed to
   the framework must be immutable. It is hoped that future versions
   will relax this restriction.  

   If non-NULL, the DAL event will be triggered when the asynchronous
   request has completed.  If the DAL Event is a callback, it will be
   invoked with the following arguments (context, 0, client_handle,
   sizeof(client_handle) );

   Restrictions: 

   In general, the same client can not have multiple simultaneous
   requests issued.  It is the responsibility of the user to make sure
   the previous request has completed before issuing a new request via
   the same client.
   
   Due to current limitations in DALSYS, async client requests can not
   be issued while INTLOCK'd.

   It is hoped that both these restrictions will be removed in future
   versions.

   @param resource_name : name of the resource that is to be used
   @param client_name : name of the client that's being created 
   @param client_type : the work model that this client will use for
   work requests

   @param dal_event : dal event to be triggered when work request is
   complete 

   @return Handle to new client, NULL if client could not be created

*/
#define npa_create_async_client_dal( a, b, c, dal_event ) \
  npa_create_async_client_cb_ex( a, b, c, NPA_DAL_CALLBACK(dal_event), \
                                 0, NULL )

/**
   @brief npa_create_async_client - See npa_create_async_client_dal

   Compatibility wrapper for npa npa_create_async_client
*/
#define npa_create_async_client    npa_create_async_client_dal


/**
   @brief npa_create_change_event_dal - Create a change event handler
   with the given resource

   This will return a NULL handle if the resource does not exist or
   the event handler could not be created.

   The event_name must be < 64 characters in length.

   The event_name string is not copied.  Any string pointer passed to
   the framework must be immutable. It is hoped that future versions
   will relax this restriction.  

   @param resource_name : name of the resource that is to be used
   @param event_name : name of the event handler that's being created 
   @param dal_event : dal event to be trigger when npa event occurs

   @return New event handle, NULL if the event handle could not be created
*/
#define npa_create_change_event_dal( a, b, dal_event ) \
  npa_create_change_event_cb( a, b, NPA_DAL_CALLBACK(dal_event) )

/**
   @brief npa_create_change_event - Compatibility wrapper for
   npa_create_change_event_dal
*/
#define npa_create_change_event    npa_create_change_event_dal

/**
   @brief npa_create_change_watermark_dal - Create a watermark event
   handler with the given resource

   This will return a NULL handle if the resource does not exist or
   the event handler could not be created.

   The event_name must be < 64 characters in length.

   The event_name string is not copied.  Any string pointer passed to
   the framework must be immutable. It is hoped that future versions
   will relax this restriction.  

   @param resource_name : name of the resource that is to be used
   @param event_name : name of the event handler that's being created 
   @param dal_event : dal event to be trigger when npa event occurs

   @return New event handle, NULL if the event handle could not be created
*/
#define npa_create_watermark_event_dal( a, b, dal_event ) \
  npa_create_watermark_event_cb( a, b, NPA_DAL_CALLBACK(dal_event) )

/**
   @brief npa_create_watermark_event - Compatibility wrapper for
   npa_create_watermark_event_dal
*/
#define npa_create_watermark_event npa_create_watermark_event_dal


/**
   
   @brief npa_resource_available_dal - Notify when resource is
   available to be used

   Register an dal event to be triggered (once) when the given
   resource is available and ready for use.  The event may be
   triggered during this function's execution if the resource is
   already available when the function is invoked.

   When invoked, the callback arguments will be
   (context, 0, *resource_name, strlen( resource_name ) );

   @param resource_name : name of the resource that's being queried

   @param dal_event : dal_event to trigger when resource is available
*/
#define npa_resource_available_dal( a, dal_event ) \
  npa_resource_available_cb( a, NPA_DAL_CALLBACK(dal_event) )

/**
   @brief npa_resource_available - Compatibility wrapper for
   npa_resource_available_dal
*/
#define npa_resource_available     npa_resource_available_dal

#endif /* NPA_H */
