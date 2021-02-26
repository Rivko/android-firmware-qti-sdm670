/** 
  @file  EFINpa.h
  @brief Node Power Architecture Protocol for UEFI.
*/
/*=============================================================================
  Copyright (c) 2013 Qualcomm Technologies, Incorporated.
  All rights reserved.
  Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     ----------------------------------------------------------
 11/20/13   llg     (Tech Pubs) Edited function syntax; otherwise, Doxygen will not run
 08/19/13   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
 04/24/13   rli     Added deinit API.
 03/27/13   rli     Initial revision.

=============================================================================*/
#ifndef __EFINPA_H__
#define __EFINPA_H__

/*===========================================================================
  INCLUDE FILES
===========================================================================*/
#include <Library/npa.h>
#include <Library/npa_init.h>
#include <Library/npa_resource.h>
#include <Library/npa_remote_resource.h>


/*===========================================================================
  MACRO DECLARATIONS
===========================================================================*/

/** @ingroup efi_npa_constants
  Protocol version.
*/
#define EFI_NPA_PROTOCOL_REVISION 0x0000000000010002
/** @} */ /* end_addtogroup efi_npa_constants */

/*  Protocol GUID definition */
/** @ingroup efi_npa_protocol */
#define EFI_NPA_PROTOCOL_GUID \
  { 0x79d6c870, 0x725e, 0x489e, { 0xa0, 0xa1, 0x27, 0xe7, 0xa5, 0xd0, 0xcb, 0x35 } }

/** @cond */
/*===========================================================================
  EXTERNAL VARIABLES
===========================================================================*/
/**
  External reference to the PMIC NPA Protocol GUID.
 */
extern EFI_GUID gEfiNpaProtocolGuid;
/** @endcond */

/*===========================================================================
  TYPE DEFINITIONS
===========================================================================*/
/* EFI_NPA_INIT */
/** @ingroup efi_npa_init
  @par Summary
  Initializes the NPA subsystem. No other NPA functions may be invoked prior
  to this function.

  @return
  EFI_SUCCESS -- Function completed successfully.
 */
typedef EFI_STATUS (EFIAPI* EFI_NPA_INIT)( void);

/* EFI_NPA_CREATE_SYNC_CLIENT_EX */
/** @ingroup efi_npa_create_sync_client_ex
  @par Summary
  Creates a synchronous client of a resource.
 
  @param[in]  resource_name  Name of the resource to be used.
  @param[in]  client_name    Name of the client to be created; 
                             must be < 64 characters in length.
  @param[in]  client_type    Work model that this client uses for
                             work requests. The work models are: 
                             - Required
                             - Impulse
                             - Vector
                             - Isochronous
                             - Limit_max
                             @tablebulletend 
  @param[in]  client_value   Resource-specific value argument.
  @param[in]  client_ref     Resource-specific pointer argument.
  @param[out] client_handle  Opaque NPA client handle to the new client, 
                             or NULL if the client cannot be created.

  @par Description
  This function creates a synchronous client of the specified resource with
  the specified work model. The new client is created without an initial
  request. Work requests made via synchronous clients are fully processed
  before returning.
  @par
  If the named resource does not exist, or it exists but does not
  support the requested client type, a NULL handle is returned.
  Callers are expected to check for NULL handles.
  @par
  A client must never attempt to use a NULL handle to update a
  resource.
  @par
  There are two resource-optional arguments, client_value (an
  unsigned int) and client_ref (a void). The meaning of these
  arguments, if any, is defined by the resource. If unnecessary,
  0 and NULL can be passed as these arguments. Note that the macro
  npa_create_sync_client can be used for this case:
  @par
  @code
  #define npa_create_sync_client( resource_name, client_name, client_type ) \
  npa_create_sync_client_ex( resource_name, client_name, client_type, 0, NULL )
  @endcode
  @newpage 
  @par
  The client_name string is not copied. Any string pointer passed to
  the framework must be immutable. Future versions might relax this
  restriction. 
 
  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.

  @dependencies
  EFI_NPA_INIT() must have already been called.
 */
typedef EFI_STATUS (EFIAPI* EFI_NPA_CREATE_SYNC_CLIENT_EX)(
    const char        *resource_name, 
    const char        *client_name,
    npa_client_type   client_type,
    unsigned int      client_value,
    void              *client_ref,
    npa_client_handle *client_handle
);

/* EFI_NPA_COMPLETE_REQUEST */
/** @ingroup efi_npa_complete_request
  @par Summary
  Completes the work request.
  
  @param[in]  client  Handle to the client in use.

  @par Description
  This function is used to notify the resource that the work request is
  complete and the client no longer needs the resource.
  @par
  Any request state that has been learned will persist.
  @par
  The resource state must be updated to account for the completed work.
   
  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.

  @dependencies
  EFI_NPA_INIT() must have already been called.
 */
typedef EFI_STATUS (EFIAPI* EFI_NPA_COMPLETE_REQUEST)( npa_client_handle client );

/* EFI_NPA_DEFINE_NODE_CB */
/** @ingroup efi_npa_define_node_cb
  @par Summary
  Defines a new NPA node.
 
  @param[in]  node           Definition of the node to be created. Definition 
                             includes the node name, driver function used to 
                             update the resource, node attributes, number of 
                             dependencies, and number of resources. 
  @param[in]  initial_state  State vector with which to initialize the
                             resources. 
  @param[in]  node_cb        Optional callback to be invoked upon successful
                             creation of the node. 
  @param[in]  context        User-supplied context argument.

  @par Description
  Merely adding the definition of a new node may not be sufficient to
  create the node, and all of its associated resources, and add it
  to the resource graph. To construct the node/resources, all the
  nodes' dependencies must exist. When all the dependencies exist,
  the resource is constructed, initialized, and added to the resource
  graph. Only after the resource is initialized and added to the graph
  can clients be created against it.
  @par
  Resource names must be globally unique. A resource cannot be
  created if there is already an existing resource with the same
  name, and an error fatal is triggered.
  @par
  The initial state vector must have an initial state value for each
  resource defined by the specified node. The behavior is undefined if the
  initial state vector has a different size than the resource vector.
  @par
  The define event callback is invoked after the node is fully
  constructed. The callback can be NULL if creation notification is
  not required. If invoked, the callback arguments are: 
  @par
  @code
  (context, 0, node->name, sizeof(const char *)).
  @endcode

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.

  @dependencies
  EFI_NPA_INIT() must have already been called.
 */
typedef EFI_STATUS (EFIAPI* EFI_NPA_DEFINE_NODE_CB)( 
    npa_node_definition *node, 
    npa_resource_state   initial_state[],
    npa_callback         node_cb,
    void                *context
);

/* EFI_NPA_SCALAR_REQUEST */
/** @ingroup efi_npa_scalar_request
  @par Summary
  Issues a scalar client work request.
  @par
  The resource state must be updated to account for the new request.
  
  @param[in]  client  Handle to the client in use.
  @param[in]  state   Requested resource state.
 
  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.

  @dependencies
  EFI_NPA_INIT() must have already been called.
 */
typedef EFI_STATUS (EFIAPI* EFI_NPA_SCALAR_REQUEST)( 
    npa_client_handle   client, 
    npa_resource_state  state 
);

/* EFI_NPA_MODIFY_REQUIRED_REQUEST */
/** @ingroup efi_npa_modify_required_request
  @par Summary
  Modifies the existing REQUIRED client work request.

  @param[in] client  Handle to the client in use.
  @param[in] delta   Modifier to the resource state.

  @par Description
  This function is intended to be used in a feedback loop; the client
  can request incrementally more (or fewer) resources.
  @par
  The resource state must be updated to account for the new request.
  @par
  The client must be a REQUIRED work model.
 
  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.

  @dependencies
  EFI_NPA_INIT() must have already been called.
*/
typedef EFI_STATUS (EFIAPI* EFI_NPA_MODIFY_REQUIRED_REQUEST)(
  npa_client_handle         client, 
  npa_resource_state_delta  delta 
);

/* EFI_NPA_ISSUE_IMPULSE_REQUEST */
/** @ingroup efi_npa_issue_impulse_request
  @par Summary
  Issues an IMPULSE client work request.

  @param[in] client  Handle to the client in use.

  @par Description
  The resource defines the impulse response.
  @par
  The resource state must be updated to account for the new request.
  @par
  The client must be an IMPULSE work model.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.

  @dependencies
  EFI_NPA_INIT() must have already been called.
 */
typedef EFI_STATUS (EFIAPI* EFI_NPA_ISSUE_IMPULSE_REQUEST)( npa_client_handle client );

/* EFI_NPA_ISSUE_VECTOR_REQUEST */
/** @ingroup efi_npa_issue_vector_request
  @par Summary
  Issues a VECTOR client work request.

  @param[in] client     Handle to the client in use.
  @param[in] num_elems  Number of elements in the vector.
  @param[in] vector     Array of num_elems.

  @par Description
  The vector pointer must remain valid for the duration of the
  request, but not beyond. This includes scheduled requests 
  where the vector information must remain valid until the 
  scheduled request has been issued and the callback has 
  occurred. If the resource needs the data to persist, it is 
  the responsibility of the resource to copy the data. 
  @par
  The interpretation of the vector elements is defined by the
  resource.
  @par
  The resource state must be updated to account for the new request.
  @par
  The client must be a VECTOR (either normal or suppressible) work
  model.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.

  @dependencies
  EFI_NPA_INIT() must have already been called.
 */
typedef EFI_STATUS (EFIAPI* EFI_NPA_ISSUE_VECTOR_REQUEST)(
    npa_client_handle   client, 
    unsigned int        num_elems, 
    npa_resource_state *vector 
);

/* EFI_NPA_RESOURCE_AVAILABLE_CB */
/** @ingroup efi_npa_resource_available_cb
  @par Summary
  Notifies when a resource is available for use.

  @param[in]  resource_name  Name of the resource to be queried.
  @param[in]  callback       Callback function to invoke when the resource is 
                             available.
  @param[in]  context        User-registered pointer to be supplied to the 
                             callback when it is invoked.

  @par Description
  This function registers a callback to be triggered (once) when the
  specified resource is available and ready for use. The callback can be
  invoked during this function's execution if the resource is already
  available when the function is invoked.
  @par
  When invoked, the callback arguments are:
  @par
  @code
  (context, 0, *resource_name, strlen( resource_name ) );
  @endcode

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.

  @dependencies
  EFI_NPA_INIT() must have already been called.
 */
typedef EFI_STATUS (EFIAPI* EFI_NPA_RESOURCE_AVAILABLE_CB)( 
   const char     *resource_name,
   npa_callback    callback,
   void           *context 
);

/* EFI_NPA_REMOTE_DEFINE_RESOURCE_CB */
/** @ingroup efi_npa_remote_define_resource_cb
  @par Summary
  Defines a local proxy to a remote resource.

  @param[in]  resource         An instance of the NPA remote resource
                               definition that describes the remoted resource.
  @param[in]  initial_state    Initial state of the local proxy.
  @param[in]  define_callback  Optional callback that is invoked when the
                               local proxy (and, hence, the remote resource)
                               is available.
  @param[in]  define_context   Optional context argument to define_callback.

  @par Description
  This function constructs all the NPA structures needed to create
  a remoted resource and add it to the local resource graph.
  @par
  The local proxy is not created until both the protocol
  used to talk to the remote resource and the remote resource
  is available. The define callback is used to determine
  when clients can be created to the local proxy/remote resource.
  @par
  If the remote resource is not remotely accessible (does not
  define NPA_RESOURCE_REMOTE_ACCESS_ALLOWED), the local proxy
  is not created.
 
  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.

  @dependencies
  EFI_NPA_INIT() must have already been called.
 */
typedef EFI_STATUS (EFIAPI* EFI_NPA_REMOTE_DEFINE_RESOURCE_CB)(
   npa_remote_resource_definition *resource, 
   npa_resource_state              initial_state,
   npa_callback                    define_callback,
   npa_user_data                   define_context 
);

/** @cond */
/* Stubbed driver functions used to define aggregation semantics.
   In the future, these may be deprecated and attributes may be used
   to specify the same semantics.

   Internally, we set the resource driver function to the protocol driver
   function in all 3 cases, but perform some setup to cater to the different
   semantics.

   Resources that specify local_aggregation_driver need no setup.

   Resources that specify remote_aggregation_driver are configured to invoke
   the protocol's custom client_create and destroy functions. They MUST
   have their plugin field set to npa_identity_plugin

   Resources that specify local_aggregation_no_initial_request get the
   NPA_REMOTE_RESOURCE_NO_INIT attribute set. The protocol/resource driver
   function should check for this attribute, when client->type is
   NPA_CLIENT_INITIALIZE, and not forward the initial state to the
   remote resource, if true.
*/

typedef EFI_STATUS (EFIAPI* EFI_NPA_REMOTE_RESOURCE_LOCAL_AGGREGATION_FCN)
( 
    struct npa_resource *resource,
    npa_client_handle    client,
    npa_resource_state   state,
    npa_resource_state   *ret_state
);

typedef EFI_STATUS (EFIAPI* EFI_NPA_REMOTE_RESOURCE_LOCAL_AGGREGATION_NO_INITIAL_REQUEST_DRIVER_FCN)
(
  struct npa_resource *resource,
  npa_client_handle    client,
  npa_resource_state   state,
  npa_resource_state  *ret_state
);

typedef EFI_STATUS (EFIAPI* EFI_NPA_REMOTE_RESOURCE_REMOTE_AGGREGATION_DRIVER_FCN)
(
  struct npa_resource *resource,
  npa_client_handle    client,
  npa_resource_state   state,
  npa_resource_state *ret_state
);

/*
   <!-- EFI_NPA_REMOTE_RESOURCE_AVAILABLE -->

   The remote protocol's query function must respond to a query of
   type NPA_QUERY_REMOTE_RESOURCE_AVAILABLE, by invoking this function
   with 'query_result.reference' as context, whenever it is that
   the remote resource is available.
 */
typedef EFI_STATUS (EFIAPI* EFI_NPA_REMOTE_RESOURCE_AVAILABLE)( void *context );
/** @endcond */

/* EFI_NPA_QUERY_BY_EVENT */
/** @ingroup efi_npa_query_by_event
  @par Summary
  Queries a resource via an event of that resource.
  @par
  Note that due to concurrency, the resource may no longer be in the
  Returned state.

  @param[in]  event         Event of the resource to be queried.
  @param[in]  query_id      Type of query to be issued.
  @param[in]  query_result  Value from the query.
  @param[out] status        Success or failure of the query.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.

  @dependencies
  EFI_NPA_INIT() must have already been called.
 */
typedef EFI_STATUS (EFIAPI* EFI_NPA_QUERY_BY_EVENT)(
   npa_event_handle   event, 
   uint32             query_id, 
   npa_query_type    *query_result,
   npa_query_status   *status
);

/* EFI_NPA_QUERY_RESOURCE_AVAILABLE */
/** @ingroup efi_npa_query_resource_available
  @par Summary
  Queries if a resource is currently available.

  @param[in]  resource_name  Name of the resource to be queried.
  @param[out] status         NPA_QUERY_SUCCESS if the resource is currently
                             available; NPA_QUERY_UNKNOWN_RESOURCE if the
                             resource is not currently available.

  @par Description
  This function queries and returns the availability of the specified 
  resource. If the resource is not currently available, it means 
  that it might become available at a future time. If users want to be
  notified when the resource becomes available, they can use
  EFI_NPA_RESOURCES_AVAILABLE_CB() to register a callback to be invoked
  when the resource becomes available.
 
  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.

  @dependencies
  EFI_NPA_INIT() must have already been called.
*/
typedef EFI_STATUS (EFIAPI* EFI_NPA_QUERY_RESOURCE_AVAILABLE)(
   const char *resource_name,
   npa_query_status *status
);

/* EFI_NPA_RESOURCES_AVAILABLE_CB */
/** @ingroup efi_npa_resources_available_cb
  @par Summary
  Notifies when all of a list of resources are all available for use.

  @param[in]  num_resources  Number of resources to be queried.
  @param[in]  resources      Array of resources to be queried. 
  @param[in]  callback       Callback function to invoke when the resource 
                             is available.
  @param[in]  context        User-registered pointer to be supplied to the 
                             callback when it is invoked.

  @par Description
  This function is a helper function to block on the availability of
  multiple resources simultaneously. The resources array passed in must
  remain valid until the callback is invoked.
  @par
  When invoked, the callback arguments are:
  @par
  @code
  (context, 0, resources[], num_resources ); 
  @endcode
 
  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.

  @dependencies
  EFI_NPA_INIT() must have already been called.
 */
typedef EFI_STATUS (EFIAPI* EFI_NPA_RESOURCES_AVAILABLE_CB)(
  unsigned int    num_resources,
  const char     *resources[],
  npa_callback    callback,
  void           *context 
);
                                 
/* EFI_NPA_DAL_EVENT_CALLBACK */
/** @ingroup efi_npa_dal_event_callback
  @par Summary
  Callback function that translates NPA callbacks into DAL event triggers.

  @param[in]  dal_event_handle  DAL event handle (passed in as context).
  @param[in]  event_type        Event type.
  @param[in]  data              Pointer to the callback data.
  @param[in]  data_size         Size of data.
  
  @par Description
  This function can be registered as the NPA callback, with the
  DALSYSEventHandle as the context argument, and when invoked, it
  triggers the DAL event. The other input arguments are passed on to
  the DAL event without translation.
 
  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.

  @dependencies
  EFI_NPA_INIT() must have already been called.
 */
typedef EFI_STATUS (EFIAPI* EFI_NPA_DAL_EVENT_CALLBACK)(
   void  *dal_event_handle,
   unsigned int  event_type, 
   void         *data,       
   unsigned int  data_size
);

/* EFI_NPA_DESTROY_CLIENT */
/** @ingroup efi_npa_destroy_client
  @par Summary
  Cancels the client request (if any) and frees the client structure. 
  @par
  The client handle cannot be used to issue requests after it is destroyed.

  @param[in]  client  Client handle to be destroyed.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.

  @dependencies
  EFI_NPA_INIT() must have already been called.
 */
typedef EFI_STATUS (EFIAPI* EFI_NPA_DESTROY_CLIENT)( npa_client_handle client );

/* EFI_NPA_CREATE_QUERY_HANDLE */
/** @ingroup efi_npa_create_query_handle
  @par Summary
  Creates a query handle to a resource.
  
  @param[in]  resource_name  Name of the resource to be queried.
  @param[in]  handle         Handle to the resource.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.

  @dependencies
  EFI_NPA_INIT() must have already been called.
 */
typedef EFI_STATUS (EFIAPI* EFI_NPA_CREATE_QUERY_HANDLE)(
    const char * resource_name,
    npa_query_handle *handle
);

/* EFI_NPA_DESTROY_QUERY_HANDLE */
/** @ingroup efi_npa_destroy_query_handle
  @par Summary
  Destroys the query handle.
  @par
  Only query handles that have been explicitly created via 
  EFI_NPA_CREATE_QUERY_HANDLE() need to be explicitly destroyed.

  @param[in]  query  Query handle to be destroyed.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.

  @dependencies
  EFI_NPA_INIT() must have already been called.
 */
typedef EFI_STATUS (EFIAPI* EFI_NPA_DESTROY_QUERY_HANDLE)( npa_query_handle query );

/* EFI_NPA_QUERY */
/** @ingroup efi_npa_query
  @par Summary
  Queries the current state of a resource.
  @par
  Note that due to concurrency, the resource may no longer be in the 
  Returned state.

  @param[in]  query         Query handle to the resource to be queried.
  @param[in]  query_id      Type of query to be issued.
  @param[in]  query_result  Value from the query.
  @param[out] status        Success or failure of the query.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.

  @dependencies
  EFI_NPA_INIT() must have already been called.
 */
typedef EFI_STATUS (EFIAPI* EFI_NPA_QUERY)(
    npa_query_handle query, 
    uint32           query_id, 
    npa_query_type  *query_result,
    npa_query_status *status
);

/* EFI_NPA_QUERY_BY_NAME */
/** @ingroup efi_npa_query_by_name
  @par Summary
  Queries the current state of the named resource.
  @par
  Note that due to concurrency, the resource may no longer be in the
  Returned state.

  @param[in]  resource_name  Name of the resource to be queried.
  @param[in]  query_id       Type of query to be issued.
  @param[in]  query_result   Value from the query.
  @param[out] status         Success or failure of the query.

  @par Description
  This is a convenience function that wraps the following sequence:
  @par
  @code
  h = npa_create_query_handle(...);
  npa_query(h, ...);
  npa_destroy_query_handle(h);
  @endcode
  @par
  If a user makes multiple queries to the same resource, calling this
  function multiple times has a poorer performance relative to managing
  the query handle directly.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.

  @dependencies
  EFI_NPA_INIT() must have already been called.
 */
typedef EFI_STATUS (EFIAPI* EFI_NPA_QUERY_BY_NAME)(
    const char      *resource_name, 
    uint32           query_id, 
    npa_query_type  *query_result,
    npa_query_status *status
);

/* EFI_NPA_QUERY_BY_CLIENT */
/** @ingroup efi_npa_query_by_client
  @par Summary
  Queries a resource via a client to that resource.
  @par
  Note that due to concurrency, the resource may no longer be in the
  Returned state.

  @param[in]  client        Client of the resource to be queried.
  @param[in]  query_id      Type of query to be issued.
  @param[in]  query_result  Value from the query.
  @param[out] status        Success or failure of the query.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.

  @dependencies
  EFI_NPA_INIT() must have already been called.
 */
typedef EFI_STATUS (EFIAPI* EFI_NPA_QUERY_BY_CLIENT)(
    npa_client_handle  client, 
    uint32             query_id, 
    npa_query_type  *query_result,
    npa_query_status *status
);

/* EFI_NPA_CREATE_EVENT_CB */
/** @ingroup efi_npa_create_event_cb
  @par Summary
  Creates an NPA event handler with the specified resource.

  @param[in]  resource_name  Name of the resource to be used.
  @param[in]  event_name     Name of the event handler to be created; 
                             must be < 64 characters in length.
  @param[in]  trigger_type   Type of trigger for the event handler to be 
                             created. Event triggers that can be generated 
                             are:
                             - Change event
                             - Watermark event
                             - Threshold event
                             @tablebulletend 
  @param[in]  callback       Callback to be invoked when the event is 
                             triggered.
  @param[in]  context        Context to be passed back in the callback 
                             invocation.
  @param[out] handle         New event handle, or NULL if the event handle 
                             cannot be created.

  @par Description
  The event_name string is not copied. Any string pointer passed to
  the framework must be immutable. Future versions might relax this 
  restriction. 
  @par
  The callback pointer must be non-NULL. When invoked, the callback
  arguments are:
  @par
  @code
  ( context, event_type, npa_event_data *, sizeof(npa_event_data) )
  @endcode

  @note1hang (For resource authors) Do not call this function from within
             a driver or update function unless there are no other events
             attempting to lock the same resource; otherwise a deadlock
             can occur. If functions are not called from within a resource,
             there are no usage restrictions.
 
  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.

  @dependencies
  EFI_NPA_INIT() must have already been called.
 */
typedef EFI_STATUS (EFIAPI* EFI_NPA_CREATE_EVENT_CB)(
    const char            *resource_name, 
    const char            *event_name,
    npa_event_trigger_type trigger_type,
    npa_callback           callback,
    void                  *context,
    npa_event_handle      *handle
);

/* EFI_NPA_SET_EVENT_WATERMARKS */
/** @ingroup efi_npa_set_event_watermarks
  @par Summary
  Sets the watermarks for triggering the event handler.

  @param[in]  event         Handle of the event to be manipulated.
  @param[in]  hi_watermark  High watermark setting.
  @param[in]  lo_watermark  Low watermark setting.

  @par Description
  Watermarks are defined as the difference between the aggregated
  request value and the resource maximum. A headroom > 0 means the
  resource is oversubscribed, i.e., the request value is greater than
  the resource can provide.
  @par
  An example of using this function is the user wants to be notified
  when the resource state rises past the high watermark of maximum
  -20 units or drops below the low watermark of maximum -40 units. Then,
  the high and low watermarks are set to -20 and -40, respectively.
  @par
  If the user wants to be notified when the resource is oversubscribed,
  both the high and low watermarks are set to 0. Then, the user receives
  an NPA_EVENT_HI_WATERMARK notification when the resource is oversubscribed
  and an NPA_EVENT_LO_WATERMARK when the resource is operating normally.
  @par
  No watermark event is triggered until the watermarks are set.
  If lo_watermark > hi_watermark, no watermark events are triggered.
  @par
  The event might be triggered during this function if the resource is
  below the low watermark or above the high watermark when this function
  is invoked.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.

  @dependencies
  EFI_NPA_INIT() must have already been called.
*/
typedef EFI_STATUS (EFIAPI* EFI_NPA_SET_EVENT_WATERMARKS)(
    npa_event_handle         event,
    npa_resource_state_delta hi_watermark,
    npa_resource_state_delta lo_watermark
);

/* EFI_NPA_SET_EVENT_THRESHOLDS 
   The high and low thresholds defines three regions. It is only valid to
   set the threshold on threshold events. 

   - Low -- state < lo_threshold
   - Nominal -- lo_threshold <= state <= hi_threshold
   - Hi Region -- hi_threshold < state 
*/
/** @ingroup efi_npa_set_event_thresholds
  @par Summary
  Sets the absolute thresholds for triggering the event handler.

  @param[in]  event         Handle to the event to be set.
  @param[in]  hi_threshold  High threshold setting.
  @param[in]  lo_threshold  Low threshold setting.

  @par Description
  This function is used if the user wants to receive notification that the 
  resource's state crosses certain thresholds. The high and low thresholds 
  define three regions. It is only valid to set the threshold on threshold 
  events. The regions are: \n
  - Low -- state < lo_threshold
  - Nominal -- lo_threshold @le state @le hi_threshold
  - High -- hi_threshold < state 
  @par
  On every resource state change, if the resource has changed the
  region it is in, the user event is triggered. Also, the user event is
  triggered when the thresholds are initially set; this is to notify
  which region the resource is in at that time.
  @par
  If lo_threshold > hi_threshold, the event is disabled. 

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.

  @dependencies
  EFI_NPA_INIT() must have already been called.
*/
typedef EFI_STATUS (EFIAPI* EFI_NPA_SET_EVENT_THRESHOLDS)(
    npa_event_handle   event, 
    npa_resource_state lo_threshold, 
    npa_resource_state hi_threshold 
);

/* EFI_NPA_DESTROY_EVENT_HANDLE */
/** @ingroup efi_npa_destroy_event_handle
  @par Summary
  Destroys the event handler.
  @par 
  After invocation, the event handle is invalidated and must not be
  reused.

  @note1hang (For resource authors) Do not call this function from within
             a driver or update function unless there are no other events
             attempting to lock the same resource; otherwise a deadlock
             can occur. If functions are not called from within a resource,
             there are no usage restrictions.

  @param[in]  event  Event handle to be destroyed.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.

  @dependencies
  EFI_NPA_INIT() must have already been called.
 */
typedef EFI_STATUS (EFIAPI* EFI_NPA_DESTROY_EVENT_HANDLE)( npa_event_handle event );

/* EFI_NPA_CANCEL_REQUEST */
/** @ingroup efi_npa_cancel_request
  @par Summary
  Cancels the work request.

  @param[in]  client  Handle to the client in use.

  @par Description
  This can be used by any work model to terminate the work request.
  This internally invokes EFI_NPA_COMPLETE_REQUEST() and then clears any
  adapted state.
  @par
  This function is used when future client requests will be
  qualitatively different than previous requests made via this client.
  @par
  The resource state must be updated to account for the terminated work.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.

  @dependencies
  EFI_NPA_INIT() must have already been called.
 */
typedef EFI_STATUS (EFIAPI* EFI_NPA_CANCEL_REQUEST)( npa_client_handle client );

/** @cond */
/* 
  <!-- EFI_NPA_SET_CLIENT_FORK_PREF -->
 
  @brief Stubbed. Does nothing. Retained for SC reasons.
 
  @return   EFI_SUCCESS
 */
typedef EFI_STATUS (EFIAPI* EFI_NPA_SET_CLIENT_FORK_PREF)
(
    npa_client_handle client,
    npa_fork_pref pref
);

/* 
  <!-- EFI_NPA_GET_CLIENT_FORK_PREF -->
 
  @brief Stubbed. 
  @param[in]  pref  Always returns NPA_FORK_DEFAULT
 
  @return   EFI_SUCCESS
 */
typedef EFI_STATUS (EFIAPI* EFI_NPA_GET_CLIENT_FORK_PREF)
( 
    npa_client_handle client,
    npa_fork_pref *pref
);

/* 
  <!-- EFI_NPA_JOIN_REQUEST -->
 
  @brief Stubbed. Does nothing. Retained for SC reasons.
  @return   EFI_SUCCESS
 */
typedef EFI_STATUS (EFIAPI* EFI_NPA_JOIN_REQUEST)( npa_client_handle client );
/** @endcond */

/* EFI_NPA_SET_REQUEST_ATTRIBUTE */
/** @ingroup efi_npa_set_request_attribute
  @par Summary
  Associates the specified attribute to the next request from this client.
  @par
  Request attributes only apply to the first issued request after
  %EFI_NPA_SET_REQUEST_ATTRIBUTE(). 
  
  @param[in]  client  Handle to the client in use.
  @param[in]  attr    Attribute to be associated. Attributes for the 
                      request are: 
                      - NPA_REQUEST_FIRE_AND_FORGET -- Client does not want 
                        to wait for the request completion
                      - NPA_REQUEST_NEXT_AWAKE -- Request applies at the 
                        next wake-up
                      @tablebulletend 
  
  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.

  @dependencies
  EFI_NPA_INIT() must have already been called.
 */
typedef EFI_STATUS (EFIAPI* EFI_NPA_SET_REQUEST_ATTRIBUTE)(
    npa_client_handle     client,
    npa_request_attribute attr
);

/* EFI_NPA_CREATE_ASYNC_CLIENT_CB_EX */
/** @ingroup efi_npa_create_async_client_cb_ex
  @par Summary
  Creates an asynchronous client of a resource.

  @param[in]  resource_name  Name of the resource to be used.
  @param[in]  client_name    Name of the client to be created; 
                             must be < 64 characters in length. 
  @param[in]  client_type    Work model that this client uses for
                             work requests. The work models are: 
                             - Required
                             - Impulse
                             - Vector
                             - Isochronous
                             - Limit_max
                             @tablebulletend 
  @param[in]  callback       Callback to be invoked when the work request 
                             is complete.
  @param[in]  context        Context to be passed back in the callback 
                             invocation.
  @param[in]  client_value   Resource-specific value argument.
  @param[in]  client_ref     Resource-specific pointer argument.
  @param[in]  handle         Handle to the new client, or NULL if the client 
                             cannot be created.

  @par Description
  This function creates an asynchronous client of the specified resource with
  the specified work model. Requests made via an asynchronous client are
  processed asynchronously. The new client is created without an initial
  request.
  @par
  If the named resource does not exist, or it exists but does not
  support the requested client type, a NULL handle is returned.
  Callers are expected to check for NULL handles.
  @par
  A client must never attempt to use a NULL handle to update a
  resource.
  @newpage 
  @par
  There are two resource-optional arguments, client_value (an
  unsigned int) and client_ref (a void). The meaning of these
  arguments, if any, is defined by the resource. If unnecessary,
  0 and NULL can be passed as these arguments. Note that the macro
  npa_create_async_client_cb can be used for this case:
  @par
  @codewide
  #define npa_create_async_client_cb( resource_name, client_name, client_type, callback, context ) \
  npa_create_async_client_cb_ex( resource_name, \
                                 client_name, client_type, \
                                 callback, context, \
                                 0, NULL )
  @endcodewide
  @par
  The client_name string is not copied. Any string pointer passed to
  the framework must be immutable. Future versions might relax this 
  restriction. 
  @par
  If non-NULL, the NPA callback is triggered when the asynchronous
  request is complete. When invoked, the arguments are:
  @par
  @code
  (context, 0, client_handle, sizeof(client_handle) );
  @endcode
  @par
  Restrictions are: \n
  - In general, the same client cannot have multiple simultaneous
    requests issued. It is the responsibility of the user to ensure
    that the previous request is complete before issuing a new request
    via the same client.
  - Due to current limitations in DALSYS, asynchronous client requests
    cannot be issued while INTLOCKed.
  @par
  Both these restrictions might be removed in future versions.
 
  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.

  @dependencies
  EFI_NPA_INIT() must have already been called.
*/
typedef EFI_STATUS (EFIAPI* EFI_NPA_CREATE_ASYNC_CLIENT_CB_EX)(
    const char       *resource_name, 
    const char       *client_name,
    npa_client_type   client_type,
    npa_callback      callback,
    void             *context, 
    unsigned int      client_value,
    void             *client_ref,
    npa_client_handle *handle
);

/* EFI_NPA_ISSUE_ISOC_REQUEST */
/** @ingroup efi_npa_issue_isoc_request
  @par Summary
  Issues an ISOCHRONOUS client work request.

  @param[in]  client      Handle to the client in use.
  @param[in]  deadline    Interval (in ms) when the work must be completed.
  @param[in]  level_hint  A hint to indicate an appropriate level. This
                          might be overridden/ignored by the resource.

  @par Description
  The client must issue EFI_NPA_COMPLETE_REQUEST() when the work is completed.
  @par
  The resource state must be updated to account for the new work.
  @par
  The client must be an ISOCHRONOUS work model.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.

  @dependencies
  EFI_NPA_INIT() must have already been called.
 */
typedef EFI_STATUS (EFIAPI* EFI_NPA_ISSUE_ISOC_REQUEST)(
    npa_client_handle   client,
    npa_resource_time   deadline,
    npa_resource_state  level_hint
);

/* EFI_NPA_ISSUE_LIMIT_MAX_REQUEST */
/** @ingroup efi_npa_issue_limit_max_request
  @par Summary
  Issues a LIMIT_MAX client work request.

  @param[in]  client  Handle to the client in use.
  @param[in]  max     Maximum state of the resource.

  @par Description
  If there are multiple concurrent maximum requests, the minimum of the
  requests is used.
  @par
  The client must be a LIMIT_MAX work model.

  @note1hang The behavior of a concurrent LIMIT_MAX and FIXED request or
  LIMIT_MAX < LIMIT_MIN requests is undefined.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.

  @dependencies
  EFI_NPA_INIT() must have already been called.
 */
typedef EFI_STATUS (EFIAPI* EFI_NPA_ISSUE_LIMIT_MAX_REQUEST)(
    npa_client_handle  client,
    npa_resource_state max
);

/* EFI_NPA_ALIAS_RESOURCE_CB */
/** @ingroup efi_npa_alias_resource_cb
  @par Summary
  Creates an alias for an existing resource.

  @param[in]  resource_name  Name of the resource to be linked.
  @param[in]  alias_name     Alias for the resource.
  @param[in]  alias_cb       Optional callback to be invoked upon successful
                             creation of the alias. The arguments are:
@verbatim
( context, 0, *alias_name, strlen(alias_name) )
@endverbatim
  @param[in]  context        User-supplied context argument.

  @par Description
  This function allows the same resource to be accessed with multiple names.
  @par
  The alias does not exist until an alias event is triggered.
  @par
  An error fatal occurs if there is already a resource with the
  requested alias name.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.

  @dependencies
  EFI_NPA_INIT() must have already been called.
 */ 
typedef EFI_STATUS (EFIAPI* EFI_NPA_ALIAS_RESOURCE_CB)(
   const char   *resource_name, 
   const char   *alias_name,
   npa_callback  alias_cb,
   void         *context
);

/* EFI_NPA_DEFINE_MARKER */
/** @ingroup efi_npa_define_marker
  @par Summary
  Defines an NPA marker.

  @param[in]  marker_name  Name of the marker to create. The string is not
                           copied, so the pointer must point to a static
                           string.

  @par Description
  A marker is a name in the NPA resource space. It allows
  users/subsystems to signal existence without having to define a
  full NPA resource.
  @par
  Markers can be listed as dependencies or used to trigger a
  resource available callback. However, markers do not support client
  requests and do not have a meaningful state. To depend
  on a marker, a resource must define the dependency's client type
  as NPA_NO_CLIENT. Dependencies defined as NPA_NO_CLIENT
  cause the node creation to be properly dependent, but do not
  allocate a client.
  @par
  An error fatal occurs if there is already a resource with the 
  requested marker name.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.

  @dependencies
  EFI_NPA_INIT() must have already been called.
 */ 
typedef EFI_STATUS (EFIAPI* EFI_NPA_DEFINE_MARKER)( const char *marker_name );

/* EFI_NPA_DEFINE_MARKER_WITH_ATTRIBUTES */
/** @ingroup efi_npa_define_marker_with_attributes
  @par Summary
  Defines an NPA marker with specific resource attributes.

  @param[in]  marker_name  Name of the marker to create. The string is not
                           copied, so the pointer must point to a static
                           string.
  @param[in]  attributes   Resource attributes of the marker.

  @par Description
  A marker is a name in the NPA resource space. It allows
  users/subsystems to signal existence without having to define a
  full NPA resource.
  @par
  Markers can be listed as dependencies or used to trigger a
  resource available callback. However, markers do not support client
  requests and do not have a meaningful state. To depend
  on a marker, a resource must define the dependency's client type
  as NPA_NO_CLIENT. Dependencies defined as NPA_NO_CLIENT
  cause the node creation to be properly dependent, but do not
  allocate a client.
  @par
  An error fatal occurs if there is already a resource with the 
  requested marker name.
  @par
  Currently, this function only supports the resource attribute
  NPA_RESOURCE_INTERPROCESS_ACCESS_ALLOWED. This allows markers
  to work across the address space. If the function is called
  with other attributes, an error fatal occurs.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.

  @dependencies
  EFI_NPA_INIT() must have already been called.
 */
typedef EFI_STATUS (EFIAPI* EFI_NPA_DEFINE_MARKER_WITH_ATTRIBUTES)(
    const char    *marker_name,
   npa_resource_attribute  attributes
);

/* EFI_NPA_ASSIGN_RESOURCE_STATE */
/** @ingroup efi_npa_assign_resource_state
  @par Summary
  Assigns the resource state directly, bypassing the update and
  driver functions. This triggers events as necessary.

  @param[in]  resource  Resource whose state is changing.
  @param[in]  state     New state of the resource.

  @par Description
  This function is intended for use in a compound node when one
  resource's state change can cause another coupled resource's state
  to change. This function must be called from within the node's
  atomic section, i.e., it is invoked from within the update or driver
  function. The function requires the NPA resource pointer, which can
  be retrieved for coupled resources from the current resource by the
  NPA_RESOURCE macro, which is predefined:
  @par
  @code
  #define NPA_RESOURCE( resource, index ) \
  ((resource)->node->resources[(index)].handle)
  @endcode

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.

  @dependencies
  EFI_NPA_INIT() must have already been called.
 */
typedef EFI_STATUS (EFIAPI* EFI_NPA_ASSIGN_RESOURCE_STATE)(
    npa_resource       *resource,
    npa_resource_state  state
);

/* EFI_NPA_QUERY_GET_RESOURCE */
/** @ingroup efi_npa_query_get_resource
  @par Summary
  Retrieves the resource associated with the specified query handle.
  @par
  This function is intended to be used by resources that want to define a
  query function that goes beyond what npa_resource_query_fcn (a pointer
  to a custom query function) can support.

  @param[in]  query_handle  Handle to the resource to be queried.
  @param[in]  resource      Pointer to the resource associated with the 
                            query handle.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.

  @dependencies
  EFI_NPA_INIT() must have already been called.
 */
typedef EFI_STATUS (EFIAPI* EFI_NPA_QUERY_GET_RESOURCE)(
    npa_query_handle query_handle,
    npa_resource **resource
);

/* EFI_NPA_ENABLE_NODE */
/** @ingroup efi_npa_enable_node
  @par Summary
  Enables/re-enables the specified node. 

  @param[in]  node           Definition of the node to be enabled. Definition 
                             includes the node name, driver function used to 
                             update the resource, node attributes, number of 
                             dependencies, and number of resources. 
  @param[in]  default_state  Vector of the default state for the node.

  @par Description
  If the node to be enabled is not currently defined, the node is
  assumed to be hot pluggable. In this case, this function makes a
  copy of all the relevant node data and defines the node for the user.
  The copy allows the user definition of the node to be removed at a
  later time via EFI_NPA_DISABLE_NODE(). If this enable
  call is defining the node, resources within the node may not be
  available upon completion of the call depending on the node's
  dependencies. This mirrors the functionality of EFI_NPA_DEFINE_NODE_CB().
  @par
  When the node is enabled for the first time, the default state
  vector defines the initial state of the resource in the node. When the
  node is re-enabled, the resources are reinitialized with
  the previous resource state if the node has valid clients. If no
  clients exist, the resources are re-initialized with the default
  state vector. In either case, an enabled node has its driver
  function invoked with an INITIALIZE client so it can reset as
  necessary. 
  @par
  If the default state vector is NULL, resources are initialized
  to 0.
  @par
  Notes: \n
  - To enable a node, the node must have the
    NPA_NODE_DISABLEABLE attribute defined and cannot define the
    NPA_NODE_NO_LOCK attribute.
  - It is an error to attempt to enable/re-enable a node if the node
    definition passed in is inconsistent (different resources,
    dependencies, etc.) with the node as defined in the resource graph.
  - This function may be used to construct a resource, much like
    EFI_NPA_DEFINE_NODE_CB(). Unlike that function, this function does not
    provide for a callback when the node has been constructed. This is 
    because the callback functionality is only relevant if this call is
    used to define the resource, i.e., the first invocation, and not on
    subsequent invocations. When this function is used for subsequent
    invocations, the expectation is that the driver can be unloaded. Hence,
    the framework has no way to validate the callback function pointer
    and is at risk of calling a function that is not present. 
    @newpage 
  - If this call is used to re-enable a previously disabled
    node, the enable call must be made from the same thread that invoked
    EFI_NPA_DISABLE_NODE().

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.

  @dependencies
  EFI_NPA_INIT() must have already been called.
 */
typedef EFI_STATUS (EFIAPI* EFI_NPA_ENABLE_NODE)(
    npa_node_definition *node,
    npa_resource_state default_state[] 
);

/* EFI_NPA_DISABLE_NODE */
/** @ingroup efi_npa_disable_node
  @par Summary
  Disables the specified node. 

  @param[in]  node  Pointer to a node definition to be disabled.

  @par Description
  When a node is disabled, the node is first locked and all the node
  dependency requests are canceled. The node is then left in a locked
  state, which prohibits other client operations from occurring while
  the node is disabled.
  @par
  A disabled node still responds to queries and event registration
  because these do not use the node lock. Note that, currently, 
  it is not permissible to disable a node with a custom query function.
  If necessary, this restriction can be removed.
  @par
  Notes:
  - To disable a node, the node must have the NPA_NODE_DISABLEABLE 
    attribute defined and cannot define the NPA_NODE_NO_LOCK attribute.
  - The behavior of this function is undefined if the node is
    disabled before it has been fully constructed.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.

  @dependencies
  EFI_NPA_INIT() must have already been called.
 */
typedef EFI_STATUS (EFIAPI* EFI_NPA_DISABLE_NODE)( npa_node_definition *node );

/** @cond */
/*
  <!-- EFI_NPA_FORK_RESOURCE -->
 
  @brief Stubbed. Retained only for source compatibility with client code.
 
  @return       EFI_SUCCESS
 */
typedef EFI_STATUS (EFIAPI* EFI_NPA_FORK_RESOURCE)(
    npa_resource       *resource,
    npa_join_function   join_func,
    void               *join_data,
    npa_join_token     *retval
);
/** @endcond */

/* EFI_NPA_RESOURCE_LOCK */
/** @ingroup efi_npa_resource_lock
  @par Summary
  Locks the resource.

  @param[in]  resource  Handle to the resource.

  @par Description
  If the resource is forked, it is joined before this function returns.
  Note that the resource shares its lock with other resources in the node.
  In effect, this locks the node.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.

  @dependencies
  EFI_NPA_INIT() must have already been called.
 */
typedef EFI_STATUS (EFIAPI* EFI_NPA_RESOURCE_LOCK)( npa_resource *resource );

/* EFI_NPA_RESOURCE_UNLOCK */
/** @ingroup efi_npa_resource_unlock
  @par Summary
  Unlocks the resource (and by extension, the node) that was previously
  locked by EFI_NPA_RESOURCE_LOCK().

  @param[in]  resource  Handle to the resource.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.

  @dependencies
  EFI_NPA_INIT() must have already been called.
 */
typedef EFI_STATUS (EFIAPI* EFI_NPA_RESOURCE_UNLOCK)( npa_resource *resource );

/* EFI_NPA_REQUEST_HAS_ATTRIBUTE */
/** @ingroup efi_npa_request_has_attribute
  @par Summary
  Checks if the current request has the specified attribute set.
  @par
  Attributes may be set on client handles, and this is cached in the resource
  structure for the duration of the request.

  @param[in]  client  Handle to the client to be checked.
  @param[in]  attr    Attribute for which to test. Attributes for the 
                      request are: 
                      - NPA_REQUEST_FIRE_AND_FORGET -- Client does not want 
                        to wait for the request completion
                      - NPA_REQUEST_NEXT_AWAKE -- Request applies at the 
                        next wake-up
                      @tablebulletend 
  @param[out] retval  TRUE if the attribute is present; otherwise, FALSE.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.

  @dependencies
  EFI_NPA_INIT() must have already been called.
 */
typedef EFI_STATUS (EFIAPI* EFI_NPA_REQUEST_HAS_ATTRIBUTE)(
   npa_client_handle     client,
   npa_request_attribute attr,
   unsigned int *retval
);

/* EFI_NPA_GET_REQUEST_ATTRIBUTES */
/** @ingroup efi_npa_get_request_attributes
  @par Summary
  Requests a bitset representing all request attributes set on this client.

  @param[in]  client  Handle of the client to be read.
  @param[out] retval  Bitset representing all request attributes set on this 
                      client.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.

  @dependencies
  EFI_NPA_INIT() must have already been called.
 */
typedef EFI_STATUS (EFIAPI* EFI_NPA_GET_REQUEST_ATTRIBUTES)(
    npa_client_handle client,
    unsigned int *retval
);

/* EFI_NPA_PASS_REQUEST_ATTRIBUTES */
/** @ingroup efi_npa_pass_request_attributes
  @par Summary
  Adds the request attributes of the ``current'' client to the ``dependency'' 
  client.
  @par
  This function does not unset any attributes set previously on the 
  ``dependency'' client.

  @param[in]  current     Handle of the NPA client whose attributes are 
                          to be applied to the other NPA client.
  @param[in]  dependency  Handle of the NPA client whose attributes are 
                          to be updated. 

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.

  @dependencies
  EFI_NPA_INIT() must have already been called.
 */
typedef EFI_STATUS (EFIAPI* EFI_NPA_PASS_REQUEST_ATTRIBUTES)(
    npa_client_handle current,
    npa_client_handle dependency
);

/* EFI_NPA_ISSUE_INTERNAL_REQUEST */
/** @ingroup efi_npa_issue_internal_request
  @par Summary
  Issues the INTERNAL client work request.

  @param[in]  client  Handle to the client in use.

  @par Description
  The client must be an INTERNAL work model. Clients with internal
  client types are created/destroyed via the standard client API.

  @note1hang The semantics of an internal client request are
  resource-/plugin-specific. There is no general API for setting the
  work request fields; it is the responsibility of the user to set the
  internal client state appropriately outside the framework. This
  function is used to issue the request after the internal fields have
  been established.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.

  @dependencies
  EFI_NPA_INIT() must have already been called.
 */
typedef EFI_STATUS (EFIAPI* EFI_NPA_ISSUE_INTERNAL_REQUEST)( npa_client_handle client );

/* EFI_NPA_RESOURCE_ADD_SYSTEM_EVENT_CALLBACK */
/** @ingroup efi_npa_resource_add_system_event_callback
  @par Summary
  Adds a system event callback function to a resource.

  @param[in]  resource_name  Name of the resource to be used.
  @param[in]  callback       Callback to be invoked.
  @param[in]  context        Context to be passed back in the callback 
                             invocation.
  @param[out] retval         NPA_SUCCESS on success; otherwise, NPA_ERROR.

  @par Description
  The callback function is invoked on the following event modifications:
  - A change event is created or destroyed,
  - A watermark or threshold event is enabled, disabled, or destroyed,
  - Event watermarks or thresholds are changed.
  @par
  When invoked, the callback arguments are:
  @par
  @code
  (context, 0, npa_event *, sizeof(npa_event) ); 
  @endcode

  @note1hang A resource supports only one system event callback; it is an
  error to add a second callback. Future versions might relax this
  restriction.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.

  @dependencies
  EFI_NPA_INIT() must have already been called.
 */
typedef EFI_STATUS (EFIAPI* EFI_NPA_RESOURCE_ADD_SYSTEM_EVENT_CALLBACK)(
    const char   *resource_name,
    npa_callback  callback,
    void         *context,
    npa_status   *retval
);

/* EFI_NPA_RESOURCE_REMOVE_SYSTEM_EVENT_CALLBACK */
/** @ingroup efi_npa_resource_remove_system_event_callback
  @par Summary
  Removes a system event callback function from a resource.
  @par
  The callback and context pointers must be the same as those when the 
  callback was added.

  @param[in]  resource_name  Name of the resource to be used.
  @param[in]  callback       Callback to be invoked.
  @param[in]  context        Context to be passed back in the callback 
                             invocation.
  @param[out] retval         NPA_SUCCESS on success; otherwise, NPA_ERROR.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.

  @dependencies
  EFI_NPA_INIT() must have already been called.
 */
typedef EFI_STATUS (EFIAPI* EFI_NPA_RESOURCE_REMOVE_SYSTEM_EVENT_CALLBACK)(
    const char   *resource_name,
    npa_callback  callback,
    void         *context,
    npa_status   *retval
);

/* EFI_NPA_DEINIT */
/** @ingroup efi_npa_deinit
  @par Summary
  NPA destructor.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.

  @dependencies
  EFI_NPA_INIT() must have already been called.
 */
typedef EFI_STATUS (EFIAPI* EFI_NPA_DEINIT)(void);

/*===========================================================================
  PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup efi_npa_protocol
  @par Summary
  Node Power Architecture Driver Protocol interface.

  @par Parameters
  @inputprotoparams{npa_proto_params.tex}
*/
typedef struct _EFI_NPA_PROTOCOL {
  UINT64                           Revision;
  EFI_NPA_INIT                     NpaInit;
  EFI_NPA_CREATE_SYNC_CLIENT_EX    CreateSyncClientEx;
  EFI_NPA_COMPLETE_REQUEST         CompleteRequest;
  EFI_NPA_DEFINE_NODE_CB           DefineNodeCb;
  EFI_NPA_SCALAR_REQUEST           ScalarRequest;
  EFI_NPA_RESOURCE_AVAILABLE_CB    ResourceAvailableCb;
  EFI_NPA_REMOTE_DEFINE_RESOURCE_CB RemoteDefineResourceCb;
/** @cond */
  EFI_NPA_REMOTE_RESOURCE_LOCAL_AGGREGATION_FCN RemoteResourceLocalAggregationFcn;
  EFI_NPA_REMOTE_RESOURCE_LOCAL_AGGREGATION_NO_INITIAL_REQUEST_DRIVER_FCN RemoteResourceLocalAggregationNoInitialRequestDriverFcn;
  EFI_NPA_REMOTE_RESOURCE_REMOTE_AGGREGATION_DRIVER_FCN RemoteResourceRemoteAggregationDriverFcn;
  EFI_NPA_REMOTE_RESOURCE_AVAILABLE RemoteResourceAvailable;
/** @endcond */
  EFI_NPA_DAL_EVENT_CALLBACK        DalEventCallback;
  EFI_NPA_RESOURCES_AVAILABLE_CB    ResourcesAvailableCb;
  EFI_NPA_QUERY_RESOURCE_AVAILABLE  QueryResourceAvailable;
  EFI_NPA_DESTROY_CLIENT            DestroyClient;
  EFI_NPA_MODIFY_REQUIRED_REQUEST   ModifyRequiredRequest;
  EFI_NPA_ISSUE_IMPULSE_REQUEST     IssueImpulseRequest;
  EFI_NPA_ISSUE_VECTOR_REQUEST      IssueVectorRequest;
  EFI_NPA_CREATE_QUERY_HANDLE       CreateQueryHandle;
  EFI_NPA_DESTROY_QUERY_HANDLE      DestroyQueryHandle;
  EFI_NPA_QUERY                     Query; 
  EFI_NPA_QUERY_BY_NAME             QueryByName;
  EFI_NPA_QUERY_BY_CLIENT           QueryByClient;
  EFI_NPA_QUERY_BY_EVENT            QueryByEvent;
  EFI_NPA_CREATE_EVENT_CB           CreateEventCb;
  EFI_NPA_SET_EVENT_WATERMARKS      SetEventWatermarks;
  EFI_NPA_SET_EVENT_THRESHOLDS      SetEventThresholds;
  EFI_NPA_DESTROY_EVENT_HANDLE      DestroyEventHandle;
  EFI_NPA_CANCEL_REQUEST            CancelRequest;
/** @cond */
  EFI_NPA_SET_CLIENT_FORK_PREF      SetClientForkPref;
  EFI_NPA_GET_CLIENT_FORK_PREF      GetClientForkPref;
  EFI_NPA_JOIN_REQUEST              JoinRequest;
/** @endcond */
  EFI_NPA_SET_REQUEST_ATTRIBUTE     SetRequestAttribute;
  EFI_NPA_CREATE_ASYNC_CLIENT_CB_EX CreateAsyncClientCbEx;
  EFI_NPA_ISSUE_ISOC_REQUEST        IssueIsocRequest;
  EFI_NPA_ISSUE_LIMIT_MAX_REQUEST   IssueLimitMaxRequest;
  EFI_NPA_ALIAS_RESOURCE_CB         AliasResourceCb;
  EFI_NPA_DEFINE_MARKER             DefineMarker;
  EFI_NPA_DEFINE_MARKER_WITH_ATTRIBUTES DefineMarkerWithAttributes;
  EFI_NPA_ASSIGN_RESOURCE_STATE     AssignResourceState;
  EFI_NPA_QUERY_GET_RESOURCE        QueryGetResource;
  EFI_NPA_ENABLE_NODE               EnableNode;
  EFI_NPA_DISABLE_NODE              DisableNode;
/** @cond */
  EFI_NPA_FORK_RESOURCE             ForkResource;
/** @endcond */
  EFI_NPA_RESOURCE_LOCK             ResourceLock;
  EFI_NPA_RESOURCE_UNLOCK           ResourceUnlock;
  EFI_NPA_REQUEST_HAS_ATTRIBUTE     RequestHasAttribute;
  EFI_NPA_GET_REQUEST_ATTRIBUTES    GetRequestAttributes;
  EFI_NPA_PASS_REQUEST_ATTRIBUTES   PassRequestAttributes;
  EFI_NPA_ISSUE_INTERNAL_REQUEST    IssueInternalRequest;
  EFI_NPA_RESOURCE_ADD_SYSTEM_EVENT_CALLBACK ResourceAddSystemEventCallback;
  EFI_NPA_RESOURCE_REMOVE_SYSTEM_EVENT_CALLBACK ResourceRemoveSystemEventCallback;
  EFI_NPA_DEINIT                    NpaDeinit;
 }EFI_NPA_PROTOCOL;

#endif /* EFINPA_H */
