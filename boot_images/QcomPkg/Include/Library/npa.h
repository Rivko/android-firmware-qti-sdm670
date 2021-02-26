/*============================================================================
@file npa.h

NPA client API. This is NOT the API for resource authors. 

The interface uses strings and opaque types. This is done intentionally to
minimize client dependencies on the implementation. The data structures and
implementation can change radically without requiring changes to client code.

Copyright (c) 2009-2013 , 2018 by Qualcomm Technologies, Inc. All Rights Reserved
All Rights Reserved.
Qualcomm Confidential and Proprietary

============================================================================*/
#ifndef NPA_H
#define NPA_H

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "DALSys.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------
 * Enumerations
 * -------------------------------------------------------------------------*/

/* NPA status */
typedef enum
{
  NPA_SUCCESS = 0,
  NPA_ERROR   = -1, /* General error */
} npa_status;

/* Client work models; clients may only be of one of these types */
typedef enum
{
  NPA_NO_CLIENT          = 0x7fffffff, /* Do not create a client */
  NPA_CLIENT_RESERVED1   = (1 << 0), /* Reserved client type */
  NPA_CLIENT_RESERVED2   = (1 << 1), /* Reserved client type */

  NPA_CLIENT_CUSTOM1     = (1 << 2), /* Custom client types - can be */
  NPA_CLIENT_CUSTOM2     = (1 << 3), /* customized by resource */
  NPA_CLIENT_CUSTOM3     = (1 << 4), 
  NPA_CLIENT_CUSTOM4     = (1 << 5), 

  /* Standard client types */
  NPA_CLIENT_REQUIRED    = (1 << 6), /* Request must honor request */
  NPA_CLIENT_ISOCHRONOUS = (1 << 7), /* Placeholder for isochronous
                                      * work model */
  NPA_CLIENT_IMPULSE     = (1 << 8), /* Poke the resource - node
                                      * defines the impulse
                                      * response */
  NPA_CLIENT_LIMIT_MAX   = (1 << 9), /* Set maximum value for the
                                      * resource */
  NPA_CLIENT_VECTOR      = (1 << 10), /* Vector request */
  NPA_CLIENT_SUPPRESSIBLE = (1 << 11), /* Suppressible Scalar
                                        * request */
  /* Suppressible Vector request */
  NPA_CLIENT_SUPPRESSIBLE_VECTOR = ((1 << 12) | NPA_CLIENT_VECTOR), 

  /* Another block of custom client types; can be customized by resource */
  NPA_CLIENT_CUSTOM5     = (1 << 13),
  NPA_CLIENT_CUSTOM6     = (1 << 14),

} npa_client_type;


/* Event trigger types - these are the types of event triggers that
 * can be generated  */
typedef enum 
{
  NPA_TRIGGER_RESERVED_EVENT_0,
  NPA_TRIGGER_RESERVED_EVENT_1,
  NPA_TRIGGER_RESERVED_EVENT_2,
  
  NPA_TRIGGER_CHANGE_EVENT,
  NPA_TRIGGER_WATERMARK_EVENT,
  NPA_TRIGGER_THRESHOLD_EVENT,

  NPA_TRIGGER_MAX_EVENT = 0x0000ffff,

  NPA_TRIGGER_RESERVED_BIT_30 = 0x40000000,
  /* Cast to int to suppress a compiler warning */
  NPA_TRIGGER_RESERVED_BIT_31 = ( int )0x80000000
}  npa_event_trigger_type;

#define NPA_TRIGGER_TYPE_MASK  NPA_TRIGGER_MAX_EVENT

/* Event types */
typedef enum
{
  NPA_EVENT_RESERVED,                 /* Reserved event */
  NPA_EVENT_RESERVED_1,               /* Reserved event */
  NPA_EVENT_RESERVED_2,               /* Reserved event */

  /* Watermark events */
  NPA_EVENT_HI_WATERMARK,       /* Resource passed hi watermark */
  NPA_EVENT_LO_WATERMARK,       /* Resource passed lo watermark */

  /* State change event */
  NPA_EVENT_CHANGE,             /* Resource changed state */

  /* Threshold events */
  NPA_EVENT_THRESHOLD_LO,      
  NPA_EVENT_THRESHOLD_NOMINAL,
  NPA_EVENT_THRESHOLD_HI,

  /* Custom events */
  NPA_EVENT_CUSTOM,

  NPA_NUM_EVENT_TYPES
} npa_event_type;


/* Retain some fork/join type and function definitions for source
 * compatibility reasons */
 
/* Fork preference */
typedef enum
{
  NPA_FORK_DEFAULT = 0,  /* Up to resource/extension author */
  NPA_FORK_DISALLOWED,   /* Disable forking */
  NPA_FORK_ALLOWED,      /* Explicitly allow forking */
} npa_fork_pref;

/* User-specified callback that is invoked when a resource
 * or transaction is joined
 *
 * The first arg is the resource handle (if a resource is joined) or
 * the transaction handle (if a transaction is joined).
 * The second arg is the context data passed into npa_fork_resource(),
 * npa_fork_transaction() or npa_fork_transaction_extension().
 */
typedef unsigned int (*npa_join_function) ( void *, void * );

typedef enum
{ /* Default request behavior */
  NPA_REQUEST_DEFAULT            = 0,

  /* Hint that the client does not want to wait for request completion */
  NPA_REQUEST_FIRE_AND_FORGET    = 0x00000001,
  
  /* The NEXT_AWAKE request attribute is a hint to the resource that this
   * request applies at next wakeup. In the case of RPM-based resources, the
   * attribute signifies that the request goes into NAS (Next Active Set)
   * rather than the current Active Set.
   */
  NPA_REQUEST_NEXT_AWAKE       = 0x00000002,

  /* The CHANGED_TYPE attribute signals to the resource that the client
   * has just changed type from required to suppressible or suppressible
   * to required and the resource must re-aggregate accordingly.
   */
  NPA_REQUEST_CHANGED_TYPE      = 0x00000004,

  /* ENUM_MAX should be set to the next higher power of 2 */ 
  NPA_REQUEST_ENUM_MAX         = 0x00000008,

} npa_request_attribute;

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
  /* Query the enabled/disabled state of the resource 
   *   Non-zero if resource is disabled */
  NPA_QUERY_RESOURCE_DISABLED,
  /* Query all of the latencies for the resource */
  NPA_QUERY_RESOURCE_LATENCY,
  /* Query the request latency for the resource */
  NPA_QUERY_RESOURCE_REQUEST_LATENCY,
  /* Query the fork latency for the resource */
  NPA_QUERY_FORK_REQUEST_LATENCY,
  /* Query the Notify latency for the resource */
  NPA_QUERY_NOTIFY_REQUEST_LATENCY,
  /* Deprecated - Query the join latency for the resource 
   * Use NPA_QUERY_NOTIFY_REQUEST_LATENCY instead
   */
  NPA_QUERY_JOIN_REQUEST_LATENCY = NPA_QUERY_NOTIFY_REQUEST_LATENCY,

  /* End public resource query space */
  NPA_MAX_PUBLIC_QUERY     = 1023,

  /* Reserved section of query space */
  NPA_QUERY_RESERVED_BEGIN = 1024,
  
  /* Query availability of remote resource, used in NPA remoting */
  NPA_QUERY_REMOTE_RESOURCE_AVAILABLE = 1025,
  
  NPA_QUERY_RESERVED_END   = 4095

  /* Resource-specific query ids beyond this */
};

/* Query Status */
typedef enum {
  NPA_QUERY_SUCCESS = 0,
  NPA_QUERY_UNSUPPORTED_QUERY_ID,
  NPA_QUERY_UNKNOWN_RESOURCE,
  NPA_QUERY_NULL_POINTER,
  NPA_QUERY_NO_VALUE, /* Used if no current state information is available. */
} npa_query_status;

/*----------------------------------------------------------------------------
 * Types/Typedefs
 * -------------------------------------------------------------------------*/
/* Work request types */
typedef unsigned int npa_resource_state; 
typedef          int npa_resource_state_delta; 

typedef uint32 npa_time_sclk;
typedef npa_time_sclk npa_resource_time;

/* Callback Type */
typedef void ( *npa_callback )( void         *context,
                                unsigned int  event_type, 
                                void         *data,       
                                unsigned int  data_size );

typedef void ( *npa_simple_callback )( void *context );

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
typedef struct npa_link * npa_query_handle;

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
   NPA_QUERY_TYPE_VALUE64,

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
    unsigned long long  value64;
    char               *name;
    void               *reference;
  } data;
} npa_query_type;

/* Maximum NPA name string length */
#define NPA_MAX_STR_LEN 63

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/* Helper macros for variable length arrays */
#define NPA_ARRAY_SIZE( array ) (sizeof(array)/sizeof(array[0]))
#define NPA_ARRAY( array ) NPA_ARRAY_SIZE(array), array
#define NPA_EMPTY_ARRAY 0, NULL

/*----------------------------------------------------------------------------
 * NPA Client API
 * -------------------------------------------------------------------------*/

/** 
 * <!-- npa_create_sync_client_ex -->
 *
 * @brief Creates a synchronous client of a resource
 *
 * Create a synchronous client of the given resource with the given
 * work model.  The new client will not be created with have an
 * initial request. Work requests made via synchronous clients will be
 * fully processed before returning.
 *
 * If the named resource does not exist or it exists but doesn't
 * support the requested client_type, a NULL handle will be
 * returned. Callers are expected to check for NULL handles.
 *
 * A client must never attempt to use a NULL handle to update a
 * resource.
 *
 * There are two resource-optional arguments, client_value (an
 * unsigned int) and client_ref( a void *). The meaning of these
 * arguments, if any, is defined by the resource. If unnecessary, 0 &
 * NULL can be passed as these arguments.  Note that the macro
 * npa_create_sync_client can be used for this case.
 *
 * The client_name must be < 64 characters in length.
 *
 * The client_name string is not copied.  Any string pointer passed to
 * the framework must be immutable. It is hoped that future versions
 * will relax this restriction.  
 *
 * @param resource_name : Name of the resource that is to be used
 * @param client_name : Name of the client that's being created
 * @param client_type : The work model that this client will use for
 *                      work requests. Only one work model enumeration
 *                      may be used.
 * @param client_value : Resource-specific value argument
 * @param client_ref : Resource-specific pointer argument
 *
 * @return Handle to new client, NULL if client could not be created
 */
npa_client_handle npa_create_sync_client_ex( const char       *resource_name, 
                                             const char       *client_name,
                                             npa_client_type   client_type,
                                             unsigned int      client_value,
                                             void             *client_ref );


/** 
 * <!-- npa_create_sync_client -->
 *
 * @brief Creates a synchronous client of a resource
 *
 * Create a synchronous client of the given resource with the given
 * work model.  The new client will not be created with have an
 * initial request. Work requests made via synchronous clients will be
 * fully processed before returning.
 *
 * If the named resource does not exist or it exists but doesn't
 * support the requested client_type, a NULL handle will be
 * returned. Callers are expected to check for NULL handles.
 *
 * A client must never attempt to use a NULL handle to update a
 * resource.
 *
 * The client_name must be < 64 characters in length.
 *
 * The client_name string is not copied.  Any string pointer passed to
 * the framework must be immutable. It is hoped that future versions
 * will relax this restriction.  
 *
 * @param resource_name : Name of the resource that is to be used
 * @param client_name : Name of the client that's being created
 * @param client_type : The work model that this client will use for
 *                      work requests
 *
 * @return Handle to new client, NULL if client could not be created
 */
#define npa_create_sync_client( resource_name, client_name, client_type ) \
  npa_create_sync_client_ex( resource_name, client_name, client_type, 0, NULL )


/** 
 * <!-- npa_create_async_client_cb_ex -->
 *
 * @brief Creates an asynchronous client of a resource
 *
 * Create an asynchronous client of the given resource with the given
 * work model.  Requests made via an asynchronous client will be
 * processed asynchronously. The new client will not be created with an
 * initial request.
 *
 * If the named resource does not exist or it exists but doesn't
 * support the requested client_type, a NULL handle will be
 * returned. Callers are expected to check for NULL handles.
 *  
 * A client must never attempt to use a NULL handle to update a
 * resource.
 *
 * There are two resource-optional arguments, client_value (an
 * unsigned int) and client_ref( a void *). The meaning of these
 * arguments, if any, is defined by the resource. If unnecessary, 0 &
 * NULL can be passed as these arguments.  Note that the macro
 * npa_create_sync_client can be used for this case.
 *
 * The client_name must be < 64 characters in length.
 *
 * The client_name string is not copied.  Any string pointer passed to
 * the framework must be immutable. It is hoped that future versions
 * will relax this restriction.  
 *
 * If non-NULL, the npa_callback will be triggered when the
 * asynchronous request has completed.  when invoked, the arguments
 * will be (context, 0, client_handle, sizeof(client_handle) );
 *
 * Restrictions: 
 *
 * In general, the same client can not have multiple simultaneous
 * requests issued.  It is the responsibility of the user to make sure
 * the previous request has completed before issuing a new request via
 * the same client.
 * 
 * Due to current limitations in DALSYS, async client requests can not
 * be issued while INTLOCK'd.1
 *
 * It is hoped that both these restrictions will be removed in future
 * versions.
 *
 * @param resource_name : Name of the resource that is to be used
 * @param client_name : Name of the client that's being created 
 * @param client_type : The work model that this client will use for
 *                      work requests.  Only one work model enumeration
 *                      may be used.
 * @param callback: Callback to be invoked when work
 *                  request is complete
 * @param context : Context to be passed back in callback invocation
 * @param client_value : resource-specific value argument
 * @param client_ref : resource-specific pointer argument
 *
 * @return Handle to new client, NULL if client could not be created
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
 * <!-- npa_create_async_client_cb -->
 *
 * @brief Creates an asynchronous client of a resource
 *
 * Create an asynchronous client of the given resource with the given
 * work model.  Requests made via an asynchronous client will be
 * processed asynchronously. The new client will not be created with an
 * initial request.
 *
 * If the named resource does not exist or it exists but doesn't
 * support the requested client_type, a NULL handle will be
 * returned. Callers are expected to check for NULL handles.
 *  
 * A client must never attempt to use a NULL handle to update a
 * resource.
 *
 * The client_name must be < 64 characters in length.
 *
 * The client_name string is not copied.  Any string pointer passed to
 * the framework must be immutable. It is hoped that future versions
 * will relax this restriction.  
 *
 * If non-NULL, the npa_callback will be triggered when the
 * asynchronous request has completed.  when invoked, the arguments
 * will be (context, 0, client_handle, sizeof(client_handle) );
 *
 * Restrictions: 
 *
 * In general, the same client can not have multiple simultaneous
 * requests issued.  It is the responsibility of the user to make sure
 * the previous request has completed before issuing a new request via
 * the same client.
 *  
 * Due to current limitations in DALSYS, async client requests can not
 * be issued while INTLOCK'd.
 *
 * It is hoped that both these restrictions will be removed in future
 * versions.
 *
 * @param resource_name : Name of the resource that is to be used
 * @param client_name : Name of the client that's being created 
 * @param client_type : The work model that this client will use for
 *                      work requests
 * @param callback : Callback to be invoked when work request is complete
 * @param context : Context to be passed back in callback invocation
 *
 *  @return Handle to new client, NULL if client could not be created
 */
#define npa_create_async_client_cb( resource_name, client_name, client_type, callback, context )         \
  npa_create_async_client_cb_ex( resource_name, \
                                 client_name, client_type,      \
                                 callback, context,             \
                                 0, NULL )

/** 
 * <!-- npa_destroy_client -->
 *
 * @brief Cancel the client request (if any) and free the client structure. 
 *
 * The client handle can not be used to issue requests after being destroyed
 *
 * @param client : Handle to client being destroyed
 *
 */
void npa_destroy_client( npa_client_handle client );


/** 
 * <!-- npa_issue_scalar_request -->
 *
 * @brief Issue a scalar client work request
 *
 * Resource state should be updated to account for new request. 
 *  
 * @param client : Handle to client being used
 * @param state : Requested resource state
 */
void npa_issue_scalar_request( npa_client_handle   client, 
                               npa_resource_state  state );


/** 
 * <!-- npa_issue_required_request -->
 *
 * @brief Issue the REQUIRED client work request
 *
 * Resource state should be updated to account for new request. 
 *  
 * The client must be of REQUIRED work model
 * 
 * @param client : Handle to client being used
 * @param state : Requested resource state
 */
#define npa_issue_required_request( client, state ) \
  npa_issue_scalar_request( client, state )


/** 
 * <!-- npa_modify_required_request -->
 *
 * @brief Modify the existing REQUIRED client work request
 *
 * This is intended to be used in a feedback loop, so the client can
 * request incrementally more (or less) resource 
 *
 * Resource state should be updated to account for new request. 
 *  
 * The client must be of REQUIRED work model
 * 
 * @param client : Handle to client being used
 * @param delta : Modifier to resource state
*/
void npa_modify_required_request( npa_client_handle         client, 
                                  npa_resource_state_delta  delta );


/** 
 * <!-- npa_issue_suppressible_request -->
 *
 * @brief Issue the SUPPRESSIBLE client work request
 *
 * Suppressible requests are requests that the resource can choose to
 * suppress. Examples of suppressible requests are clocks that want to
 * be at a certain frequency if the XO is running, but do not want to
 * prohibit XO shutdown. I.e., if the resource is on, the request
 * should be satisfied, but if the only requests to the resource are
 * suppressible, the resource can be placed into a low power mode.
 *
 * If your request can not tolerate the resource going into low power
 * modes, use required request instead.
 *
 * Resource state should be updated to account for new request.
 *  
 * The client must be of SUPPRESSIBLE work model
 *
 * @param client : Handle to client being used
 * @param state : Requested resource state
 */
#define npa_issue_suppressible_request( client, state ) \
  npa_issue_scalar_request( client, state )


/** 
 * <!-- npa_issue_scalar_request_unconditional -->
 *
 * @brief Issue a scalar client work request 
 *
 * Resource state should be updated to account for new request. 
 *
 * For uncondtional request, the first shortcircuiting check will
 * be bypassed. The client can issue duplicate requests and aggregation
 * will still be performed. However, this does not guarantee that the driver
 * function will be invoked. If the state of the resource does not change, 
 * the driver will be short-circuited  
 *  
 * @param client : Handle to client being used
 * @param state : Requested resource state
 */
void npa_issue_scalar_request_unconditional( npa_client_handle   client, 
                                             npa_resource_state  state );


/** 
 * <!-- npa_issue_required_request_unconditional -->
 *
 * @brief Issue the REQUIRED client work request
 *
 * Resource state should be updated to account for new request. 
 * 
 * For uncondtional request, the first shortcircuiting check will
 * be bypassed. The client can issue duplicate requests and aggregation
 * will still be performed. However, this does not guarantee that the driver
 * function will be invoked. If the state of the resource does not change, 
 * the driver will be short-circuited  
 *
 * The client must be of REQUIRED work model
 * 
 * @param client : Handle to client being used
 * @param state : Requested resource state
 */
#define npa_issue_required_request_unconditional( client, state ) \
  npa_issue_scalar_request_unconditional( client, state )
  
  
  /** 
 * <!-- npa_issue_suppressible_request_unconditional -->
 *
 * @brief Issue the SUPPRESSIBLE client work request
 *
 * Suppressible requests are requests that the resource can choose to
 * supress. Examples of suppressible requests are clocks that want to
 * be at a certain frequency if the XO is running, but do not want to
 * prohibit XO shutdown. I.e., if the resource is on, the request
 * should be satisfied, but if the only requests to the resource are
 * suppressible, the resource can be placed into a low power mode.
 *
 * If your request can not tolerate the resource going into low power
 * modes, use required request instead.
 *
 * Resource state should be updated to account for new request.
 *
 * For uncondtional request, the first shortcircuiting check will
 * be bypassed. The client can issue duplicate requests and aggregation
 * will still be performed. However, this does not guarantee that the driver
 * function will be invoked. If the state of the resource does not change, 
 * the driver will be short-circuited  
 *  
 * The client must be of SUPPRESSIBLE work model
 *
 * @param client : Handle to client being used
 * @param state : Requested resource state
 */
#define npa_issue_suppressible_request_unconditional( client, state ) \
  npa_issue_scalar_request_unconditional( client, state )


/** 
 * <!-- npa_issue_impulse_request -->
 *
 * @brief Issue an IMPULSE work request 
 *
 * The client must be of IMPULSE work model.
 * The resource defines the impulse response
 * Resource state should be updated to account for new request
 *
 * @param client : Handle to client being used
 */
void npa_issue_impulse_request( npa_client_handle client );


/** 
 * <!-- npa_issue_vector_request -->
 *
 * @brief Issue the VECTOR client work request
 *
 * Resource state should be updated to account for new request.
 *
 * The vector pointer must remain valid for the duration of the
 * request, but not beyond.  This includes scheduled requests 
 * where the vector information must remain valid until the 
 * scheduled request is been issued and the callback has 
 * occurred. 
 *
 * A reference to the vector is maintained in the 
 * client data structure as NPA_ACTIVE_REQUEST(client).pointer.vector. 
 * If the client's work model is changed (from required to suppressible 
 * or vice-versa), this pointer may be used by the resource to 
 * re-aggregate under the new type. It is up to clients to ensure that 
 * the pointed-to-vector is valid in such scenarios (or work out 
 * alternate arrangements with resource)
 *
 * The interpretation of the vector elements is defined by the
 * resource.
 *  
 * The client must be of VECTOR (either normal or SUPPRESSIBLE) work
 * model
 * 
 * @param client : Handle to client being used
 * @param num_elems : Number of elements in the vector
 * @param vector : Array of num_elems
 */
void npa_issue_vector_request( npa_client_handle   client, 
                               unsigned int        num_elems, 
                               npa_resource_state *vector );


/** 
 * <!-- npa_issue_required_vector_request -->
 *
 * @brief Issue the VECTOR client work request
 *
 * Resource state should be updated to account for new request.
 *
 * The vector pointer must remain valid for the duration of the
 * request, but not beyond.  This includes scheduled requests 
 * where the vector information must remain valid until the 
 * scheduled request is been issued and the callback has 
 * occurred. If the resource needs the data to persist, it is 
 * the responsibility of the resource to copy the data.
 *
 * The interpretation of the vector elements is defined by the
 * resource.
 *  
 * The client must be of VECTOR work model
 * 
 * @param client : Handle to client being used
 * @param num_elems : Number of elements in the vector
 * @param vector : Array of num_elems
 */ 
#define npa_issue_required_vector_request( client, num_elems, vector ) \
  npa_issue_vector_request( client, num_elems, vector )


/** 
 * <!-- npa_issue_suppressible_vector_request -->
 * 
 * @brief Issue a SUPPRESSIBLE VECTOR client work request
 *
 * Suppressible requests are requests that the resource can choose to
 * suppress. Examples of suppressible requests are bus flows that want
 * are not valid if the CPU goes to sleep. I.e., if the CPU is on, the
 * request should be satisfied, but if the CPU goes to sleep, the
 * request can be suppressed.
 *
 * If your request can not tolerate the resource going into low power
 * modes, use required request instead.
 *
 * Resource state should be updated to account for new request.
 *
 * The vector pointer must remain valid for the duration of the
 * request, but not beyond.  This includes scheduled requests 
 * where the vector information must remain valid until the 
 * scheduled request is been issued and the callback has 
 * occurred. If the resource needs the data to persist, it is 
 * the responsibility of the resource to copy the data.
 *
 * The interpretation of the vector elements is defined by the
 * resource.
 *  
 * The client must be of SUPPRESSIBLE_VECTOR work model
 * 
 * @param client : Handle to client being used
 * @param num_elems : Number of elements in the vector
 * @param vector : Array of num_elems
 */
#define npa_issue_suppressible_vector_request( client, num_elems, vector ) \
  npa_issue_vector_request( client, num_elems, vector )


/** 
 * <!-- npa_issue_isoc_request -->
 *
 * @brief Issue an ISOCHRONOUS work request 
 *
 * The client must be of ISOCHRONOUS work model
 *
 * The client must issue npa_complete_request() when the work is completed
 *  
 * Resource state should be updated to account for new work
 * Note: The hint may be overridden/ignored by the resource
 *
 * @param client : Handle to client being used
 * @param deadline : The interval (in ms) when the work must be completed
 * @param level_hint : A hint as to what an appropriate level might
 *                     be. May be overridden by resource
 */
void npa_issue_isoc_request( npa_client_handle   client,
                             npa_resource_time   deadline,
                             npa_resource_state  level_hint );


/** 
 * <!-- npa_issue_limit_max_request -->
 *
 * @brief Issue the LIMIT_MAX  request
 *
 * If there are multiple concurrent max requests, the minimum of the
 * requests will be used.
 *
 * The client must be of LIMIT_MAX work model.
 *
 * Note: The behavior of concurrent LIMIT_MAX and FIXED request or
 * LIMIT_MAX < LIMIT_MIN requests is undefined
 *
 * @param client : handle to client being used
 * @param max : maximum state of resource
 */
void npa_issue_limit_max_request( npa_client_handle  client,
                                  npa_resource_state max );


/** 
 * <!-- npa_complete_request -->
 *
 * @brief Complete the work request
 *
 *  This is used to notify the resource that the work request is
 *  complete and the client no longer needs the resource
 *
 *  Any request state that has been learned will persist
 *  
 *  Resource state should be updated to account for completed work
 *
 *  @param client : Handle to client being used
 */
void npa_complete_request( npa_client_handle client );


/** 
 * <!-- npa_cancel_request -->
 *
 * @brief  Cancel the work request
 *
 * This can be used by any work model to terminate the work request.
 * This internally invokes npa_complete_request and then clears any
 * adapted state
 *
 * This function is used when future client requests will be
 * qualitatively different than previous requests made via this client
 *
 * Resource state should be updated to account for terminated work
 *
 * @param client: Handle to client being used
 */
void npa_cancel_request( npa_client_handle client );


/** 
 * <!-- npa_set_client_fork_pref -->
 *
 * @brief Stubbed. Does nothing. Retained for SC reasons.
 */
void npa_set_client_fork_pref( npa_client_handle client,
                               npa_fork_pref pref ); 


/** 
 * <!-- npa_get_client_fork_pref -->
 *
 * @brief Stubbed. Always returns NPA_FORK_DEFAULT
 */
npa_fork_pref npa_get_client_fork_pref( npa_client_handle client );


/** 
 * <!-- npa_join_request -->
 *
 * @brief Stubbed. Does nothing. Retained for SC reasons.
 */
void npa_join_request( npa_client_handle client );


/** 
 * <!-- npa_set_request_attribute -->
 *
 * @brief Associates the given attribute to the next request from this client.
 *  
 * Request attributes only apply to the first issue_request after
 * npa_set_request_attribute 
 */
void npa_set_request_attribute( npa_client_handle     client,
                                npa_request_attribute attr );


/**
 * <! -- npa_set_client_type_required -->
 * 
 * @brief Changes client type from suppressible to required.
 *
 * If client is of type NPA_CLIENT_SUPPRESSIBLE will change type to 
 * NPA_CLIENT_REQUIRED
 * If client is of type NPA_CLIENT_SUPPRESSIBLE_VECTOR will change type
 * to NPA_CLIENT_VECTOR
 * In all other cases, will error fatal.
 *
 * Vector clients who switch types may need to ensure that a pointer 
 * to their previous/active request from the client is still available in 
 * NPA_ACTIVE_REQUEST(client).pointer.vector.
 *
 * Once client type is changed a request for the client's current active 
 * state will be issued with the new client type
 *
 * @param client client handle that will have the work model changed
 */
void npa_set_client_type_required( npa_client_handle client );


/**
 * <! -- npa_set_client_type_suppressible -->
 * 
 * @brief Changes client type from required to suppressible
 *
 * If client is of type NPA_CLIENT_REQUIRED will change type to 
 * NPA_CLIENT_SUPPRESSIBLE 
 * If client is of type NPA_CLIENT_VECTOR will change type
 * to NPA_CLIENT_SUPPRESSIBLE_VECTOR 
 * In all other cases, will error fatal.
 *
 * Vector clients who switch types may need to ensure that a pointer 
 * to their previous/active request from the client is still available in 
 * NPA_ACTIVE_REQUEST(client).pointer.vector.
 *
 * Once client type is changed a request for the client's current active 
 * state will be issued with the new client type
 *
 * @param client client handle that will have the work model changed
 */
void npa_set_client_type_suppressible( npa_client_handle client );

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
 * <!-- npa_create_event_cb -->
 * 
 * @brief Create an NPA event handler with the given resource
 *
 * This will return a NULL handle if the resource does not exist or
 * the event handler could not be created.
 *
 * The event_name must be < 64 characters in length.
 *
 * The event_name string is not copied.  Any string pointer passed to
 * the framework must be immutable. It is hoped that future versions
 * will relax this restriction.  
 *
 * The callback pointer must be non-NULL.  When invoked, the callback
 * arguments will be ( context, event_type, npa_event_data *, sizeof(
 * npa_event_data) )
 *
 * NOTE (For Resource Authors): This function should not be called
 * from within a driver or update function unless you know that there
 * will be no other events that might also be attempting to lock the
 * same resource, otherwise the possibility for deadlock exists.  If
 * you are not using the functions from within a resource, there are
 * no usage restrictions.
 *
 * @param resource_name : Name of the resource that is to be used
 * @param event_name : Name of the event handler that's being created 
 * @param trigger_type : Type of trigger for event handler to be created 
 * @param callback : Callback to be invoked when event is triggered
 * @param context : Context to be passed back in callback invocation
 *
 * @return New event handle, NULL if the event handle could not be created
 */
npa_event_handle 
npa_create_event_cb( const char            *resource_name, 
                     const char            *event_name,
                     npa_event_trigger_type trigger_type,
                     npa_callback           callback,
                     void                  *context );


/** 
 * <!-- npa_create_change_event_cb -->
 *
 * @brief Create a change event handler with the given resource
 *
 * See npa_create_event_cb for usage restrictions
 *
 * @param resource_name : Name of the resource that is to be used
 * @param event_name : Name of the event handler that's being created 
 * @param callback : Callback to be invoked when event is triggered
 * @param context : Context to be passed back in callback invocation
 *
 *  @return New event handle, NULL if the event handle could not be created
*/
#define npa_create_change_event_cb( resource_name, event_name, callback, context ) \
  npa_create_event_cb( resource_name, event_name,              \
                       NPA_TRIGGER_CHANGE_EVENT,               \
                       callback, context )


/** 
 * <!-- npa_create_watermark_event_cb -->
 *
 *  @brief Create a watermark event handler with the given resource
 *
 *  The event handler will not be invoked until the watermarks have
 *  been set
 *
 *  See npa_create_event_cb for usage restrictions
 *
 *  @param resource_name : Name of the resource that is to be used
 *  @param event_name : Name of the event handler that's being created 
 *  @param callback : Callback to be invoked when event is triggered
 *  @param context : Context to be passed back in callback invocation
 *
 *  @return New event handle, NULL if the event handle could not be created
 */
#define npa_create_watermark_event_cb( resource_name, event_name, \
                                       callback, context )        \
  npa_create_event_cb( resource_name, event_name,                 \
                       NPA_TRIGGER_WATERMARK_EVENT,               \
                       callback, context )


/** 
 * <!-- npa_set_event_watermarks -->
 *
 * @brief Set the watermarks for triggering the event handler.
 *
 * Watermarks are defined as the difference between the aggregated
 * request value and the resource maximum.  Headroom > 0 means the
 * resource is oversubscribed, as the request value is greater than
 * the resource can provide.
 *
 * For example: If the caller wanted to be notified when the resource
 * state rose past the hi watermark of (max-20) units or dropped below
 * the low watermark of (max-40) units, the hi and lo watermarks would
 * be set to -20 and -40, respectively
 *
 * If the user wishes to receive notification when the resource is
 * oversubscribed, set both the hi and lo watermark to zero.  Then the
 * user will receive a NPA_EVENT_HI_WATERMARK notification when the
 * resource is oversubscribed and NPA_EVENT_LO_WATERMARK when the
 * resource is operating normally.
 *  
 * No watermark event will be triggered until the watermarks have been
 * set.  If the lo watermark > hi watermark, no watermark events will
 * be triggered.
 *
 * The event may be triggered during this function if the resource is
 * below the lo watermark or above the hi watermark when this function
 * is invoked
 *  
 * @param event : Handle of event being manipulated
 * @param hi_watermark : Hi watermark setting
 * @param lo_watermark : Lo watermark setting
 */
void npa_set_event_watermarks( npa_event_handle         event,
                               npa_resource_state_delta hi_watermark,
                               npa_resource_state_delta lo_watermark );



/** 
 * <!-- npa_set_event_thresholds -->
 *
 * @brief Set the absolute thresholds for triggering the event handler
 *
 * If the user wishes to receive notification that the resource's state
 * crosses certain thresholds.  The hi and lo thresholds defines three
 * regions.  It is only valid to set the threshold on threshold events.
 *
 *   Low Region     - state < lo_threshold
 *   Nominal Region - lo_threshold <= state <= hi_threshold
 *   Hi Region      - hi_threshold < state 
 *
 * On every resource state change, if the resource has changed the
 * region it is in, then the user event will be triggered.  Also, the
 * user event will be triggered when the thresholds are initially set
 * to notify which region the resource is in at that time.
 *
 * If lo_threshold > hi_threshold, the event is disabled. 
 *
 * @param event : Handle to event being set
 * @param hi_threshold : Hi threshold setting
 * @param lo_threshold : Lo threshold setting
 */
void npa_set_event_thresholds( npa_event_handle   event, 
                               npa_resource_state lo_threshold, 
                               npa_resource_state hi_threshold );


/** 
 * <!-- npa_destroy_event_handle -->
 *
 * @brief Destroy the event handler
 *   
 * After invocation, the event handle is invalidated and must not be
 * reused
 *
 * NOTE (For Resource Authors): This function should not be called from
 * within a driver or update function unless you know that there will
 * be no other events that might also be attempting to lock the same
 * resource, otherwise the possibility for deadlock exists.  If you are
 * not using the functions from within a resource, there are no usage
 * restrictions.
 *
 * @param event : Handle of event being destroyed
 */
void npa_destroy_event_handle( npa_event_handle event );


/*============================================================================
  NPA Query API

  NPA Queries are used to query the underlying resource state
============================================================================*/

/**
 * <!-- npa_create_query_handle -->
 *
 * @brief Create a query handle to a resource 
 *  
 * @param resource_name : Name of resource being queried
 *
 * @return Query handle to resource
 */
npa_query_handle npa_create_query_handle( const char * resource_name );


/**
 * <!-- npa_destroy_query_handle -->
 *
 * @brief Destroy the query handle
 *
 * Only query handles that have been explicitly created via
 * npa_create_query_handle need to be explicitly destroyed
 *
 * @param query : Query handle to destroy
 */
void npa_destroy_query_handle( npa_query_handle query );


/** 
 * <!-- npa_query -->
 *
 * @brief Query the current state of the resource
 *
 * Note that due to concurrency, the resource may no longer be in the
 * returned state.
 *
 * @param query : Query handle to resource being queried
 * @param query_id : Type of query being issued
 * @param query_result : Value from the query
 *
 * @return success/failure of query (npa_query_status)
 */
npa_query_status npa_query( npa_query_handle query, 
                            uint32           query_id, 
                            npa_query_type  *query_result );


/** 
 * <!-- npa_query_by_name -->
 *
 * @brief Query the current state of the named resource
 *
 * This is a convenience function that wraps the
 *
 * h = npa_create_query_handle(...);
 * npa_query(h, ...);
 * npa_destroy_query_handle(h);
 *
 * sequence. If you are making multiple queries to the same resource,
 * calling this function multiple times will have a poorer performance
 * relative to managing the query handle directly.
 *
 * Note that due to concurrency, the resource may no longer be in the
 * returned state.
 *
 * @param resource_name : Name of resource being queried
 * @param query_id : Type of query being issued
 * @param query_result : Value from the query
 *
 * @return success/failure of query (npa_query_status)
 */
npa_query_status npa_query_by_name( const char      *resource_name, 
                                    uint32           query_id, 
                                    npa_query_type  *query_result );


/** 
 * <!-- npa_query_by_client -->
 *
 * @brief Query a resource via a client to that resource
 *
 * Note that due to concurrency, the resource may no longer be in the
 * returned state.
 *
 * @param client : Client of resource being queried
 * @param query_id : Type of query being issued
 * @param query_result : Value from the query
 *
 * @return success/failure of query (npa_query_status)
 */
npa_query_status npa_query_by_client( npa_client_handle  client, 
                                      uint32             query_id, 
                                      npa_query_type    *query_result );


/** 
 * <!-- npa_query_by_event -->
 *
 * @brief Query a resource via an event of that resource
 *
 * Note that due to concurrency, the resource may no longer be in the
 * returned state.
 *
 * @param event : Event of resource being queried
 * @param query_id : Type of query being issued
 * @param query_result : Value from the query
 *
 * @return success/failure of query (npa_query_status)
 */
npa_query_status npa_query_by_event( npa_event_handle   event, 
                                     uint32             query_id, 
                                     npa_query_type    *query_result );


/** 
 * <!-- npa_query_resource_available -->
 *
 * @brief Query if resource is currently available 
 *
 * Query and return if the given resource is currently available.  If
 * the resource is not currently available, this does not mean that it
 * may not become available at a future time. If you wish to be
 * notified when the resource becomes available, use
 * npa_resource_available_cb() to register a callback to be invoked
 * when the resource becomes available.
 *
 * @param resource_name : Name of the resource that's being queried
 *
 * @return NPA_QUERY_SUCCESS if resource is currently available,
 * NPA_QUERY_UNKNOWN_RESOURCE if resource is not currently available
 */
npa_query_status npa_query_resource_available( const char *resource_name );


/** 
 * <!-- npa_resources_available_cb -->
 *
 * @brief Notify when all of a list of resources have all become available to be used
 *
 * This function is a helper function to block on the availability of
 * multiple resources at once.  The resources array passed in must
 * remain valid until the callback is invoked.
 *
 * When invoked, the callback arguments will be 
 * (context, 0, resources[], num_resources ); 
 *
 * @param num_resources : Number of resources that are being queried
 * @param resources : Array of resources that are being queried 
 * @param callback : Callback function to invoke when resource is available
 * @param context : User registered pointer to be supplied to callback
 *                  when it is invoked
 */
void npa_resources_available_cb( unsigned int    num_resources,
                                 const char     *resources[],
                                 npa_callback    callback,
                                 void           *context );


/** 
 * <!-- npa_resource_available_cb -->
 *
 * @brief Register a callback function to be called when the named resource is
 * available to be used
 *
 * Register a callback to be triggered (once) when the given resource
 * is available and ready for use.  The callback may be invoked during
 * this function's execution if the resource is already available when
 * the function is invoked.
 *
 * When invoked, the callback arguments will be
 * (context, 0, *resource_name, strlen( resource_name ) );
 *
 * @param resource_name : Name of the resource that's being queried
 * @param callback : Callback function to invoke when resource is available
 * @param context : User registered pointer to be supplied to callback when it
 *                  is invoked
 */
void npa_resource_available_cb( const char     *resource_name,
                                npa_callback    callback,
                                void           *context );


/** 
 * <!-- npa_dal_event_callback -->
 *
 * @brief Callback function that translates npa callbacks into dal_event triggers.
 *
 * This function can be registered as the NPA callback, with the
 * DALSYSEventHandle as the context argument, and when invoked it will
 * trigger the dal event.  The other input arguments will be passed on to
 * the dal event without translation.
 *
 * @param dal_event_handle : dal_event_handle (passed in as context)
 * @param event_type : int
 * @param data : Pointer to callback data
 * @param data_size : Size of void *
 */
void npa_dal_event_callback( void         *dal_event_handle,
                             unsigned int  event_type, 
                             void         *data,       
                             unsigned int  data_size );


/** 
 * <!-- NPA_DAL_CALLBACK -->
 *
 * @brief Macro that takes the dal event handle and returns the
 * npa_dal_event_callback and dal event for easy
 * registration in the npa_*_cb functions
 *
 * If the dal event is NULL, a NULL callback will be registered.  Note
 * that some callback functions require a non-NULL callback, and will
 * generate an ERR_FATAL if you attempt to register a NULL callback
 *
 * This is primarily intended for use in the compatibility macros used
 * to map the legacy DAL event API onto the direct callback API, but
 * it can also be used independently.
 *
 * @param dal_event_handle : dal_event_handle (passed in as context)
 */
#define NPA_DAL_CALLBACK( dal_event_handle ) \
  ((NULL == (dal_event_handle)) ? NULL : npa_dal_event_callback), \
    (dal_event_handle)


/* Legacy API - invoke callback functions with dal_event callback.
 *
 * These calls are deprecated and should not be used by new code
 */

/** 
 * <!-- npa_create_async_client_dal -->
 *
 * @brief Creates an asynchronous client of a resource
 *
 * Create an asynchronous client of the given resource with the given
 * work model.  Requests made via an asynchronous client will be
 * processed asynchronously. The new client will not be created with an
 * initial request.
 *
 * If the named resource does not exist or it exists but doesn't
 * support the requested client_type, a NULL handle will be
 * returned. Callers are expected to check for NULL handles.
 *  
 * A client must never attempt to use a NULL handle to update a
 * resource.
 *
 * The client_name must be < 64 characters in length.
 *
 * The client_name string is not copied.  Any string pointer passed to
 * the framework must be immutable. It is hoped that future versions
 * will relax this restriction.  
 *
 * If non-NULL, the DAL event will be triggered when the asynchronous
 * request has completed.  If the DAL Event is a callback, it will be
 * invoked with the following arguments (context, 0, client_handle,
 * sizeof(client_handle) );
 *
 * Restrictions: 
 *
 * In general, the same client can not have multiple simultaneous
 * requests issued.  It is the responsibility of the user to make sure
 * the previous request has completed before issuing a new request via
 * the same client.
 * 
 * Due to current limitations in DALSYS, async client requests can not
 * be issued while INTLOCK'd.
 *
 * It is hoped that both these restrictions will be removed in future
 * versions.
 *
 * @param resource_name : Name of the resource that is to be used
 * @param client_name : Name of the client that's being created 
 * @param client_type : The work model that this client will use for
 * work requests
 * @param dal_event : dal event to be triggered when work request is
 * complete 
 *
 * @return Handle to new client, NULL if client could not be created
 */
#define npa_create_async_client_dal( a, b, c, dal_event ) \
  npa_create_async_client_cb_ex( a, b, c, NPA_DAL_CALLBACK(dal_event), \
                                 0, NULL )


/** 
 * <!-- npa_create_async_client -->
 *
 * @brief Compatibility wrapper for npa npa_create_async_client
 *
 * See npa_create_async_client_dal
 */
#define npa_create_async_client    npa_create_async_client_dal


/** 
 * <!-- npa_create_change_event_dal -->
 *
 * @brief Create a change event handler with the given resource
 *
 * This will return a NULL handle if the resource does not exist or
 * the event handler could not be created.
 *
 * The event_name must be < 64 characters in length.
 *
 * The event_name string is not copied.  Any string pointer passed to
 * the framework must be immutable. It is hoped that future versions
 * will relax this restriction.  
 *
 * @param resource_name : Name of the resource that is to be used
 * @param event_name : Name of the event handler that's being created 
 * @param dal_event : dal event to be trigger when npa event occurs
 *
 * @return New event handle, NULL if the event handle could not be created
 */
#define npa_create_change_event_dal( a, b, dal_event )  \
  npa_create_event_cb( a, b,                            \
                       NPA_TRIGGER_CHANGE_EVENT,        \
                       NPA_DAL_CALLBACK(dal_event) )


/** 
 * <!-- npa_create_change_event -->
 *
 * @brief Compatibility wrapper for npa_create_change_event_dal
 */
#define npa_create_change_event    npa_create_change_event_dal


/** 
 * <!-- npa_create_change_watermark_dal -->
 *
 * @brief Create a watermark event handler with the given resource
 *
 * The event_name must be < 64 characters in length.
 *
 * The event_name string is not copied.  Any string pointer passed to
 * the framework must be immutable. It is hoped that future versions
 * will relax this restriction.  
 *
 * @param resource_name : Name of the resource that is to be used
 * @param event_name : Name of the event handler that's being created 
 * @param dal_event : dal event to be trigger when npa event occurs
 *
 * @return A new event handle, or a NULL handle if the resource does not exist
 * or the event handler could not be created.
 */
#define npa_create_watermark_event_dal( a, b, dal_event )       \
  npa_create_event_cb( a, b,                                    \
                       NPA_TRIGGER_WATERMARK_EVENT,             \
                       NPA_DAL_CALLBACK(dal_event) )


/** 
* <!-- npa_create_watermark_event -->
 *
 * @brief Compatibility wrapper for  npa_create_watermark_event_dal
 */
#define npa_create_watermark_event npa_create_watermark_event_dal


/** 
 * <!-- npa_resource_available_dal --> 
 *
 * @brief Notify when resource is available to be used
 *
 * Register an dal event to be triggered (once) when the given
 * resource is available and ready for use.  The event may be
 * triggered during this function's execution if the resource is
 * already available when the function is invoked.
 *
 * When invoked, the callback arguments will be
 * (context, 0, *resource_name, strlen( resource_name ) );
 *
 * @param resource_name : Name of the resource that's being queried
 * @param dal_event : dal_event to trigger when resource is available
 */
#define npa_resource_available_dal( a, dal_event ) \
  npa_resource_available_cb( a, NPA_DAL_CALLBACK(dal_event) )


/** 
 * <!-- npa_resource_available -->
 *
 * @brief Compatibility wrapper for npa_resource_available_dal
 */
#define npa_resource_available     npa_resource_available_dal

#ifdef __cplusplus
}
#endif

#endif /* NPA_H */