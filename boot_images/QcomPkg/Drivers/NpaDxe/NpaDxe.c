/** @file NpaDxe.c

  This file implements NPA EFI protocol interface.

  Copyright (c) 2013-2015, , 2018 by Qualcomm Technologies, Inc. All Rights Reserved
  reserved. 

**/

/*=============================================================================
                              EDIT HISTORY

 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 07/23/15   rli      Added RpmApi protocol
 06/08/15   sj       Added kvp_destroy() in EFI_ddr_health_monitor()
 05/05/15   rli      Uninstall protocols on npa_deinit
 08/20/14   rli      Added RpmServices protocol.
 04/24/13   rli      Added deinit API.
 03/27/13   rli      Initial revision.

=============================================================================*/


/*=========================================================================
      Include Files
==========================================================================*/

#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/EFINpa.h>
#include <Protocol/EFIRpmServices.h>
#include <Protocol/EFIRpm.h>
#include <Library/rpmclient.h>
#include <Library/DebugLib.h>

/*=========================================================================
      Globals and prototypes
==========================================================================*/
STATIC EFI_HANDLE  gProtocolHandle = NULL;
static EFI_STATUS EFI_npa_deinit(void);

/*=========================================================================
      Functions
==========================================================================*/
/*----------------------------------------------------------------------------
 * NPA Client API
 * -------------------------------------------------------------------------*/
/**
 * <!-- npa_init -->
 *
  @brief npa_init - Initializes the NPA subsystem. No other NPA functions may
  be invoked prior to npa_init.
* @return       EFI_SUCCESS
 */
static EFI_STATUS EFI_npa_init( void)
{
    npa_init();
  return EFI_SUCCESS;
}

/** 
 * <!-- npa_create_sync_client_ex -->
 *
 * @brief Creates a synchronous client of a resource
 *
 * Create a synchronous client of the given resource with the given
 * work model.  The new client will not be created with have an
 * initial request. Work requests made via syncronous clients will be
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
 *                      work requests
 * @param client_value : Resource-specific value argument
 * @param client_ref : Resource-specific pointer argument
 * @param[out] client_handle: An opaque NPA client handle to new client, 
                        NULL if client could not be created
 *
 * @return       EFI_SUCCESS
 */
static EFI_STATUS EFI_npa_create_sync_client_ex( const char       *resource_name, 
                                             const char       *client_name,
                                             npa_client_type   client_type,
                                             unsigned int      client_value,
                                             void             *client_ref,
                                             npa_client_handle *client_handle)
{
    *client_handle = npa_create_sync_client_ex(resource_name, 
                                             client_name,
                                             client_type,
                                             client_value,
                                             client_ref);
  return EFI_SUCCESS;
}

/** 
 * <!-- EFI_npa_destroy_client -->
 *
 * @brief Cancel the client request (if any) and free the client structure. 
 *
 * The client handle can not be used to issue requests after being destroyed
 *
 * @param client : Handle to client being destroyed
 *
 *  @return       EFI_SUCCESS
 */
static EFI_STATUS EFI_npa_destroy_client( npa_client_handle client )
{
     npa_destroy_client( client );
  return EFI_SUCCESS;
}

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
 *
 *  @return       EFI_SUCCESS
 */
static EFI_STATUS EFI_npa_complete_request( npa_client_handle client )
{
    npa_complete_request( client );

  return EFI_SUCCESS;
}

/** 
 * <!-- EFI_npa_issue_scalar_request -->
 *
 * @brief Issue a scalar client work request
 *
 * Resource state should be updated to account for new request. 
 *  
 * @param client : Handle to client being used
 * @param state : Requested resource state
 *
 *  @return       EFI_SUCCESS
 */
static EFI_STATUS EFI_npa_issue_scalar_request( 
                               npa_client_handle   client, 
                               npa_resource_state  state )
{
    npa_issue_scalar_request(client, state );

  return EFI_SUCCESS;
}

/** 
 * <!-- EFI_npa_modify_required_request -->
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
static EFI_STATUS EFI_npa_modify_required_request( npa_client_handle         client, 
                                  npa_resource_state_delta  delta )
{
    npa_modify_required_request( client, delta );

  return EFI_SUCCESS;
}

/** 
 * <!-- EFI_npa_issue_impulse_request -->
 *
 * @brief Issue an IMPULSE work request 
 *
 * The client must be of IMPULSE work model.
 * The resource defines the impulse response
 * Resource state should be updated to account for new request
 *
 * @param client : Handle to client being used
 *
 *  @return       EFI_SUCCESS
 */
static EFI_STATUS EFI_npa_issue_impulse_request( npa_client_handle client )
{
    npa_issue_impulse_request( client );

  return EFI_SUCCESS;
}


/** 
 * <!-- EFI_npa_issue_vector_request -->
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
 * the repsonsibility of the resource to copy the data. 
 *
 * The interpretation of the vector elements is defined by the
 * resource.
 *  
 * The client must be of VECTOR (either normal or SUPRESSIBLE) work
 * model
 * 
 * @param client : Handle to client being used
 * @param num_elems : Number of elements in the vector
 * @param vector : Array of num_elems
 *
 *  @return       EFI_SUCCESS
 */
static EFI_STATUS EFI_npa_issue_vector_request( npa_client_handle   client, 
                               unsigned int        num_elems, 
                               npa_resource_state *vector )
{
  npa_issue_vector_request( client, num_elems, vector );

  return EFI_SUCCESS;
}

/** 
 * <!-- EFI_npa_resource_available_cb -->
 *
 * @brief Notify when resource is available to be used
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
 * @param callback : Callback funtion to invoke when resource is available
 * @param context : User registered pointer to be supplied to callback when it
 *                  is invoked
 *
 *  @return       EFI_SUCCESS
 */
static EFI_STATUS EFI_npa_resource_available_cb( const char     *resource_name,
                                npa_callback    callback,
                                void           *context )
{
    npa_resource_available_cb( resource_name,
                               callback,
                               context );

  return EFI_SUCCESS;
}


/** 
 * <!-- EFI_npa_query_by_event -->
 *
 * @brief Query a resource via an event of that resource
 *
 * Note that due to concurrency, the resource may no longer be in the
 * returned state.
 *
 * @param event : Event of resource being queried
 * @param query_id : Type of query being issued
 * @param query_result : Value from the query
 * @param status success/failure of query (npa_query_status)
 *
 * @return       EFI_SUCCESS
 */
static EFI_STATUS EFI_npa_query_by_event( npa_event_handle   event, 
                                          uint32             query_id, 
                                          npa_query_type    *query_result,
                                          npa_query_status  *status
                                         )
{
    *status = npa_query_by_event( event, 
                                 query_id, 
                                 query_result);
  return EFI_SUCCESS;
}
/** 
 * <!-- EFI_npa_query_resource_available -->
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
 * @param status NPA_QUERY_SUCCESS if resource is currently available,
 * NPA_QUERY_UNKNOWN_RESOURCE if resource is not currently available
 *
 * @return       EFI_SUCCESS
*/
static EFI_STATUS EFI_npa_query_resource_available( const char *resource_name,  npa_query_status *status)
{
    *status = npa_query_resource_available(resource_name );
  return EFI_SUCCESS;
}
/** 
 * <!-- EFI_npa_resources_available_cb -->
 *
 * @brief Notify when all of a list of resources have all become available to be used
 *
 * This function is a helper function to block on the availability of
 * mutiple rsources at once.  The resources array passed in must
 * remain valid until the callback is invoked.
 *
 * When invoked, the callback arguments will be 
 * (context, 0, resources[], num_resources ); 
 *
 * @param num_resources : Number of resources that are being queried
 * @param resources : Array of resources that are being queried 
 * @param callback : Callback funtion to invoke when resource is available
 * @param context : User registered pointer to be supplied to callback
 *                  when it is invoked
 * @return       EFI_SUCCESS
 */
static EFI_STATUS EFI_npa_resources_available_cb( unsigned int    num_resources,
                                 const char     *resources[],
                                 npa_callback    callback,
                                 void           *context )
{
    npa_resources_available_cb( num_resources,
                                 resources,
                                 callback,
                                 context );
  return EFI_SUCCESS;
}
/** 
 * <!-- EFI_npa_dal_event_callback -->
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
static EFI_STATUS EFI_npa_dal_event_callback( void         *dal_event_handle,
                             unsigned int  event_type, 
                             void         *data,       
                             unsigned int  data_size )
{
    npa_dal_event_callback( dal_event_handle,
                            event_type, 
                            data,       
                            data_size );

  return EFI_SUCCESS;
}

/**
 * <!-- EFI_npa_remote_define_resource_cb -->
 *
 * @brief Define a local proxy to a remote resource.
 *
 * This function will construct all NPA structures needed to create
 * a remoted resource and add it to the local resource graph.
 *
 * The local proxy will not be created until both the protocol
 * used to talk to the remote resource and the remote resource
 * is available. The define_callback may be used to determine
 * when clients may be created to the local proxy/remote resource.
 *
 * If the remote resource is not remotely accessible (does not
 * define NPA_RESOURCE_REMOTE_ACCESS_ALLOWED), the local proxy
 * will not be created.
 *
 * @param remote_defn: An instance of npa_remote_resource_definition that
 * describes the remoted resource.
 * @param initial_state: initial state of the local proxy
 * @param define_callback: optional callback that will be invoked when
 * the local proxy (and hence the remote resource) is available.
 * @param define_context: optional context argument to define_callback
 *
 * @return   EFI_SUCCESS
 */
static EFI_STATUS EFI_npa_remote_define_resource_cb( npa_remote_resource_definition *resource, 
                                    npa_resource_state              initial_state,
                                    npa_callback                    define_callback,
                                    npa_user_data                   define_context )
{
  DEBUG((EFI_D_ERROR,"\nNPA REMOTING is NOT Supported\n"));

  return EFI_SUCCESS;
}

/* Stubbed driver functions used to define aggregation semantics.
 * In the future, these may be deprecated and attributes may be used
 * to specify the same semantics.
 *
 * Internally, we set the resource driver function to the protocol driver
 * function in all 3 cases, but perform some setup to cater to the different
 * semantics.
 *
 * Resources that specify local_aggregation_driver need no setup.
 *
 * Resources that specify remote_aggregation_driver are configured to invoke
 * the protocol's custom client_create and destroy functions. They MUST
 * have their plugin field set to npa_identity_plugin
 *
 * Resources that specify local_aggregation_no_initial_request get the
 * NPA_REMOTE_RESOURCE_NO_INIT attribute set. The protocol/resource driver
 * function should check for this attribute, when client->type is
 * NPA_CLIENT_INITIALIZE, and not forward the initial state to the
 * remote resource, if true.
*/
static EFI_STATUS EFI_npa_remote_resource_local_aggregation_driver_fcn
( 
    struct npa_resource *resource,
    npa_client_handle    client,
    npa_resource_state   state,
    npa_resource_state   *ret_state
)
{
  DEBUG((EFI_D_ERROR,"\nNPA REMOTING is NOT Supported\n"));	

  return EFI_SUCCESS;
}
static EFI_STATUS EFI_npa_remote_resource_local_aggregation_no_initial_request_driver_fcn
(
  struct npa_resource *resource,
  npa_client_handle    client,
  npa_resource_state   state,
  npa_resource_state  *ret_state
)
{
  DEBUG((EFI_D_ERROR,"\nNPA REMOTING is NOT Supported\n"));

  return EFI_SUCCESS;
}

static EFI_STATUS EFI_npa_remote_resource_remote_aggregation_driver_fcn 
(
  struct npa_resource *resource,
  npa_client_handle    client,
  npa_resource_state   state,
  npa_resource_state *ret_state
)
{
  DEBUG((EFI_D_ERROR,"\nNPA REMOTING is NOT Supported\n"));

  return EFI_SUCCESS;
}

/**
 * <!-- EFI_npa_remote_resource_available -->
 *
 * The remote protocol's query function must respond to a query of
 * type NPA_QUERY_REMOTE_RESOURCE_AVAILABLE, by invoking this function
 * with 'query_result.reference' as context, whenever it is that
 * the remote resource is available.
 */
static EFI_STATUS EFI_npa_remote_resource_available( void *context )
{
  DEBUG((EFI_D_ERROR,"\nNPA REMOTING is NOT Supported\n"));

  return EFI_SUCCESS;
}
/**
 * <!-- EFI_npa_create_query_handle -->
 *
 * @brief Create a query handle to a resource 
 *  
 * @param resource_name : Name of resource being queried
 * @param handle: handle to resource
 *
 * @return   EFI_SUCCESS
 */
static EFI_STATUS EFI_npa_create_query_handle
(
    const char * resource_name,
    npa_query_handle *handle
)
{
    *handle = npa_create_query_handle( resource_name );
  return EFI_SUCCESS;
}

/**
 * <!-- EFI_npa_destroy_query_handle -->
 *
 * @brief Destroy the query handle
 *
 * Only query handles that have been explicitly created via
 * npa_create_query_handle need to be explicitly destroyed
 *
 * @param query : Query handle to destroy
 *
 * @return   EFI_SUCCESS
 */
static EFI_STATUS EFI_npa_destroy_query_handle( npa_query_handle query )
{
     npa_destroy_query_handle( query );
  return EFI_SUCCESS;
}
/** 
 * <!-- EFI_npa_query -->
 *
 * @brief Query the current state of the resource
 *
 * Note that due to concurrency, the resource may no longer be in the
 * returned state.
 *
 * @param query : Query handle to resource being queried
 * @param query_id : Type of query being issued
 * @param query_result : Value from the query
 * @param status: success/failure of query (npa_query_status)
 *
 * @return   EFI_SUCCESS
 */
static EFI_STATUS EFI_npa_query
(
    npa_query_handle query, 
    uint32           query_id, 
    npa_query_type  *query_result,
    npa_query_status *status
)
{
    *status = npa_query( query, query_id, query_result );
  return EFI_SUCCESS;
}
/** 
 * <!-- EFI_npa_query_by_name -->
 *
 * @brief Query the current state of the named resource
 *
 * This is a convenience function that wraps the
 *
 * h = npa_create_query_handle(...);
 * npa_query(h, ...);
 * npa_destroy_query_handle(h);
 *
 * sequence. If you are making mulitple queries to the same resource,
 * calling this function multiple times will have a poorer performance
 * relative to managing the query handle directly.
 *
 * Note that due to concurrency, the resource may no longer be in the
 * returned state.
 *
 * @param resource_name : Name of resource being queried
 * @param query_id : Type of query being issued
 * @param query_result : Value from the query
 * @param status: success/failure of query (npa_query_status)
 *
 * @return   EFI_SUCCESS
 */
static EFI_STATUS EFI_npa_query_by_name
(
    const char      *resource_name, 
    uint32           query_id, 
    npa_query_type  *query_result,
    npa_query_status *status
)
{
    *status = npa_query_by_name( resource_name, query_id, query_result);
  return EFI_SUCCESS;
}

/** 
 * <!-- EFI_npa_query_by_client -->
 *
 * @brief Query a resource via a client to that resource
 *
 * Note that due to concurrency, the resource may no longer be in the
 * returned state.
 *
 * @param client : Client of resource being queried
 * @param query_id : Type of query being issued
 * @param query_result : Value from the query
 * @param status: success/failure of query (npa_query_status)
 *
 * @return   EFI_SUCCESS
 */
static EFI_STATUS EFI_npa_query_by_client
(
    npa_client_handle  client, 
    uint32             query_id, 
    npa_query_type  *query_result,
    npa_query_status *status
)
{
    *status = npa_query_by_client( client, query_id, query_result );
  return EFI_SUCCESS;
}

/** 
 * <!-- EFI_npa_create_event_cb -->
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
 * @param handle : New event handle, NULL if the event handle could not be created
 *
 * @return   EFI_SUCCESS
 */
static EFI_STATUS EFI_npa_create_event_cb
(
    const char            *resource_name, 
    const char            *event_name,
    npa_event_trigger_type trigger_type,
    npa_callback           callback,
    void                  *context,
    npa_event_handle      *handle
)
{ 
    *handle = npa_create_event_cb( resource_name, event_name, trigger_type, callback, context );
  return EFI_SUCCESS;
}

/** 
 * <!-- EFI_npa_set_event_watermarks -->
 *
 * @brief Set the watermarks for triggering the event handler.
 *
 * Watermarks are defined as the difference between the aggregated
 * request value and the resource maximum.  Headroom > 0 means the
 * resource is oversubscribed, as the request value is greater than
 * the resource can provide.
 *
 * For example: If the caller wanted to be notified when the resource
 * state rose past qthe hi watermark of (max-20) units or dropped below
 * the low watermark of (max-40) units, the hi and lo watermarks would
 * be set to -20 and -40, respectively
 *
 * If the user wishes to recieve notification when the resource is
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
 *
 * @return   EFI_SUCCESS
*/
static EFI_STATUS EFI_npa_set_event_watermarks
(
    npa_event_handle         event,
    npa_resource_state_delta hi_watermark,
    npa_resource_state_delta lo_watermark
)
{
     npa_set_event_watermarks( event, hi_watermark, lo_watermark );
  return EFI_SUCCESS;
}

/** 
 * <!-- EFI_npa_set_event_thresholds -->
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
 *
 * @return   EFI_SUCCESS
*/
static EFI_STATUS EFI_npa_set_event_thresholds
(
    npa_event_handle   event, 
    npa_resource_state lo_threshold, 
    npa_resource_state hi_threshold 
)
{
    npa_set_event_thresholds( event, lo_threshold, hi_threshold );
  return EFI_SUCCESS;
}
/** 
 * <!-- EFI_npa_destroy_event_handle -->
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
 *
 * @return   EFI_SUCCESS
 */
static EFI_STATUS EFI_npa_destroy_event_handle( npa_event_handle event )
{
     npa_destroy_event_handle( event );
  return EFI_SUCCESS;
}

/** 
 * <!-- EFI_npa_cancel_request -->
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
 *
 * @return   EFI_SUCCESS
 */
static EFI_STATUS EFI_npa_cancel_request( npa_client_handle client )
{
    npa_cancel_request( client );
  return EFI_SUCCESS;
}


/** 
 * <!-- EFI_npa_set_client_fork_pref -->
 *
 * @brief Stubbed. Does nothing. Retained for SC reasons.
 *
 * @return   EFI_SUCCESS
 */
static EFI_STATUS EFI_npa_set_client_fork_pref
(
    npa_client_handle client,
    npa_fork_pref pref
)
{
    npa_set_client_fork_pref( client, pref );
  return EFI_SUCCESS;
}

/** 
 * <!-- EFI_npa_get_client_fork_pref -->
 *
 * @brief Stubbed. 
 * @param pref : Always returns NPA_FORK_DEFAULT
 *
 * @return   EFI_SUCCESS
 */
static EFI_STATUS EFI_npa_get_client_fork_pref
( 
    npa_client_handle client,
    npa_fork_pref *pref
)
 {
    *pref = npa_get_client_fork_pref( client );
  return EFI_SUCCESS;
 }

/** 
 * <!-- EFI_npa_join_request -->
 *
 * @brief Stubbed. Does nothing. Retained for SC reasons.
 * @return   EFI_SUCCESS
 */
static EFI_STATUS EFI_npa_join_request( npa_client_handle client )
{
    npa_join_request( client );
  return EFI_SUCCESS;
}

/** 
 * <!-- EFI_npa_set_request_attribute -->
 *
 * @brief Associates the given attribute to the next request from this client.
 *  
 * Request attributes only apply to the first issue_request after
 * npa_set_request_attribute 
 *
 * @return   EFI_SUCCESS
 */
static EFI_STATUS EFI_npa_set_request_attribute
(
    npa_client_handle     client,
    npa_request_attribute attr
)
{
     npa_set_request_attribute( client, attr );
  return EFI_SUCCESS;
}

/** 
 * <!-- EFI_npa_create_async_client_cb_ex -->
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
 *                      work requests
 * @param callback: Callback to be invoked when work
 *                  request is complete
 * @param context : Context to be passed back in callback invocation
 * @param client_value : resource-specific value argument
 * @param client_ref : resource-specific pointer argument
 * @param handle : Handle to new client, NULL if client could not be created
 *
 * @return   EFI_SUCCESS
*/
static EFI_STATUS EFI_npa_create_async_client_cb_ex
(
    const char       *resource_name, 
    const char       *client_name,
    npa_client_type   client_type,
    npa_callback      callback,
    void             *context, 
    unsigned int      client_value,
    void             *client_ref,
    npa_client_handle *handle
)
{
    *handle = npa_create_async_client_cb_ex ( resource_name, 
                                              client_name,
                                              client_type,
                                              callback,
                                              context, 
                                              client_value,
                                              client_ref
                                            );
  return EFI_SUCCESS;
}

/** 
 * <!-- EFI_npa_issue_isoc_request -->
 *
 * @brief Issue an ISOCHRONOUS work request 
 *
 * The client must be of ISOCHRONOUS work model
 *
 * The client must issue npa_complete_request() when the work is completed
 *  
 * Resource state should be updated to accound for new work
 * Note: The hint may be overriden/ignored by the resource
 *
 * @param client : Handle to client being used
 * @param deadline : The interval (in ms) when the work must be completed
 * @param level_hint : A hint as to what an appropriate level might
 *                     be. May be overridden by resource
 *
 * @return   EFI_SUCCESS
 */
static EFI_STATUS EFI_npa_issue_isoc_request
(
    npa_client_handle   client,
    npa_resource_time   deadline,
    npa_resource_state  level_hint
)
{
    npa_issue_isoc_request( client, deadline, level_hint );

  return EFI_SUCCESS;
}


/** 
 * <!-- EFI_npa_issue_limit_max_request -->
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
 *
 * @return   EFI_SUCCESS
 */
static EFI_STATUS EFI_npa_issue_limit_max_request
(
    npa_client_handle  client,
    npa_resource_state max
)
{
    npa_issue_limit_max_request( client, max );
  return EFI_SUCCESS;
}

/*----------------------------------------------------------------------------
 * NPA Resource API
 * -------------------------------------------------------------------------*/

/**
 * <!-- EFI_npa_define_node_cb -->
 *
 * @brief Define a new NPA node
 *
 * Define a new node.  Simply adding the definition of a new node may
 * not be sufficient to create the node and all of its associated
 * resources and add it to the resource graph.  In order to construct
 * the node/resources, all of the nodes' dependencies must exist. When
 * all the dependencies exist, the resource will be constructucted,
 * initialized, and added to the resource graph.  Only after the
 * resource is initialized and added to the graph can clients be
 * created against it.
 * 
 * Resource names must be globally unique. A resource will not be
 * created if there is already an existing resource with the same
 * name and a fatal error will be triggered
 * 
 * The initial state vector must have an initial state value for each
 * resource defined by the given node.  Behavior is undefined if the
 * initial state vector has a different size than the resource vector.
 * 
 * The define_event callback will be invoked after the node is fully
 * constructed.  The callback can be NULL if creation notification is
 * unneeded. If invoked, the callback arguemnts are (context, 0,
 * node->name, sizeof(const char *)).
 * 
 * @param node : Definition of node to be created 
 * @param initial_state : State vector to initialize the resources with 
 * @param define_cb : Optional callback to be invoked upon successful 
 *        creation of the node.  
 * @param context : User supplied context argument
 * @return       EFI_SUCCESS
 */
static EFI_STATUS EFI_npa_define_node_cb( 
                         npa_node_definition *node, 
                         npa_resource_state   initial_state[],
                         npa_callback         node_cb,
                         void                *context)
{
    npa_define_node_cb( node, 
                        initial_state,
                        node_cb,
                        context);

  return EFI_SUCCESS;
}

/**
 * <!-- EFI_npa_alias_resource_cb -->
 *
 * @brief Create an alias for an existing resource.
 *
 * This allows the same resource to be accessed with
 * multiple names.
 *
 * The alias doesn't exist until alias_event is triggered
 *
 * An error fatal will occur if there is already a resource with the
 * requested alias name
 *
 * @param resource_name : The name of the resource to be linked
 * @param alias_name : The alias for the resource
 * @param alias_cb : Optional callback to be invoked upon successful
 *        alias creation of the alias.  The arguments will be ( context,
 *        0, *alias_name, strlen(alias_name) )
 * @param context : User supplied context argument
 *
 * @return       EFI_SUCCESS
 */ 
static EFI_STATUS EFI_npa_alias_resource_cb
(
   const char   *resource_name, 
   const char   *alias_name,
   npa_callback  alias_cb,
   void         *context
)
{

    npa_alias_resource_cb( resource_name, alias_name, alias_cb, context);
  return EFI_SUCCESS;
}

/**
 * <!-- EFI_npa_define_marker -->
 *
 * @brief Define an npa marker
 *
 * A marker is a name in the npa resource space.  It allows
 * users/subsystems to signal existence without having to define a
 * full NPA resource.
 *     
 * Markers can be listed as dependencies, or used to trigger a
 * resource_available callback, however markers do not support client
 * requests and do not have a meaningful state.  In order to depend
 * on a marker, a resource should define the dependency's client type
 * as NPA_NO_CLIENT.  Dependencies defined as NPA_NO_CLIENT will
 * cause node creation to be properly dependent, but will not
 * allocate a client.
 *
 * An error fatal will occur if there is already a resource with the
 * requested marker name
 *
 * @param marker_name : The name of the marker to create.  The string
 *        is not copied, so the pointer must point to a static string.
 *
 * @return       EFI_SUCCESS
 */ 
static EFI_STATUS EFI_npa_define_marker( const char *marker_name )
{
    npa_define_marker( marker_name );
  return EFI_SUCCESS;
}


/**
 * <!-- EFI_npa_define_marker_with_attributes -->
 *
 * @brief Define an npa marker with specific resource attribute(s)
 *
 * A marker is a name in the npa resource space.  It allows
 * users/subsystems to signal existence without having to define a
 * full NPA resource.
 *  
 * Markers can be listed as dependencies, or used to trigger a
 * resource_available callback, however markers do not support client
 * requests and do not have a meaningful state.  In order to depend
 * on a marker, a resource should define the dependency's client type
 * as NPA_NO_CLIENT.  Dependencies defined as NPA_NO_CLIENT will
 * cause node creation to be properly dependent, but will not
 * allocate a client.
 *
 * An error fatal will occur if there is already a resource with the
 * requested marker name
 *
 * This function only supports the resource attribute
 * NPA_RESOURCE_INTERPROCESS_ACCESS_ALLOWED for now. This is to
 * allow markers to work across address space. If it is called
 * with other attributes, there will be an error fatal.
 * 
 * @param marker_name : The name of the marker to create.  The string
 *        is not copied, so the pointer must point to a static string.
 * @param attributes: the resource attribute(s) of the marker
 *
 * @return       EFI_SUCCESS
 */
static EFI_STATUS EFI_npa_define_marker_with_attributes
(
    const char    *marker_name,
   npa_resource_attribute  attributes
)
{
    npa_define_marker_with_attributes( marker_name, attributes );
  return EFI_SUCCESS;
}



/*======================================================================
  Author API

  These are special purpose functions intended for use by the node
  author.  
  ======================================================================*/
/**
 * <!-- EFI_npa_assign_resource_state -->
 *
 * @brief Assign the resource state directly, bypassing the update and
 * driver functions.  This will trigger events as necessary.
 *
 * This function is intended for use in a compound node when one
 * resource's state change can cause another coupled resource's state
 * to change. This function must be called from within the node's
 * atomic section - which means it can be invoked from within the
 * update or driver function.  It require the npa_resource *, which
 * can be retrived for coupled resources from the current resource
 * by the NPA_RESOURCE macro.
 *
 * @param resource : Resource whose state is changing
 * @param state : New state of the resource
 *
 * @return       EFI_SUCCESS
 */
static EFI_STATUS EFI_npa_assign_resource_state
(
    npa_resource       *resource,
    npa_resource_state  state
)
{
    npa_assign_resource_state( resource, state );
  return EFI_SUCCESS;
}


/**
 * <!-- EFI_npa_query_get_resource -->
 *
 * @brief Retrieve the resource associated with the given query handle.
 *
 * This function is intended to be used by resources that wish to define a
 * query API that goes beyond what npa_resource_query_fcn can support.
 *   
 * @param query_handle : Handle to resource being queried
 * @param resource : Pointer to resource associated with the query handle
 *
 * @return       EFI_SUCCESS
 */
static EFI_STATUS EFI_npa_query_get_resource
(
    npa_query_handle query_handle,
    npa_resource **resource
) 
{
    *resource = npa_query_get_resource( query_handle ); 
  return EFI_SUCCESS;
}

/**
 * <!-- EFI_npa_enable_node -->
 *
 * @brief Enable/re-enable the given node.  
 *
 * If the node being enabled is not currently defined, this node is
 * assumed to be hotpluggable.  In this case, this function will make a
 * copy of all the relevant node data and define the node for the user.
 * This copy allows the user definition of the node to be removed at a
 * later time, through the function npa_disable_node().  If this enable
 * call is defining the node, resources within the node may not be
 * available upon completion of the call depending on the node's
 * dependencies.  This mirrors the functionality of npa_define_node().
 *
 * If the node is being enabled for the first time, the default state
 * vector defines the initial state of the resource in the node. If the
 * node is being re-enabled, the resources will be reinitialized with
 * the previous resource state if the node has valid clients.  If no
 * clients exist, the resources will be re-initialized with the default
 * state vector. In either case, an enabled node will have its driver
 * function invoked with an INITIALIZE client, so it can reset as
 * necessary.  
 *
 * If the default state vector is NULL, resources will be initialized
 * to 0.
 *
 * Note: In order to enable a node, the node must have the
 * NPA_NODE_DISABLEABLE attribute defined and can not define the
 * NPA_NODE_NO_LOCK attribute
 *
 * Note: It is an error to attempt to enable/disable a node if the node
 * definition passed in is inconsistent (different resources,
 * dependencies, etc) with the node as defined in the resource graph.
 *
 * Note: This function may be used to construct a resource, much like
 * npa_define_node().  Unlike that function, this does not provide for
 * a callback when the node has been constructed. This is because the
 * callback functionality is only relevant if this call is being used
 * to define the resource (i.e., the first invocation) and not on
 * subsequent invocations, and if you are using this function in that
 * circumstance, the expectation is that the driver can be unloaded and
 * the framework has no way to validate the callback function pointer
 * and would be at risk of calling a function is not present
 *
 * @note If this call is being used to re-enable a previously disabled
 * node, the enable call must be made from the same thread that invoked
 * npa_disable_node.
 *
 * @param node : definition of node to be enabled
 * @param default_state : vector of default state for the node
 *
 * @return       EFI_SUCCESS
 */
static EFI_STATUS EFI_npa_enable_node
(
    npa_node_definition *node,
    npa_resource_state default_state[] 
)
{
    npa_enable_node( node, default_state );
  return EFI_SUCCESS;
}

/**
 * <!-- EFI_npa_disable_node -->
 *
 * @brief Disable the given node. 
 *
 * When a node is disabled, the node is first locked and all the node
 * dependency requests are canceled.  The node is then left in a locked
 * state, which prohibits other client operations from happening while
 * the node is disabled.
 *
 * A disabled node will still respond to queries and event registration
 * as these do not use the node lock.  Note that it is not currently
 * legal to disable a node with a custom query function. If necessary,
 * this restriction can be removed.
 *
 * Note: In order to enable a node, the node must have the
 * NPA_NODE_DISABLEABLE attribute defined and can not define the
 * NPA_NODE_NO_LOCK attribute
 *
 * Note: The behavior of this function is undefined if the node is
 * disabled before it has been fully constructed.
 *
 * @param node : Pointer to a node definition that is to be disabled
 *
 * @return       EFI_SUCCESS
 */
static EFI_STATUS EFI_npa_disable_node( npa_node_definition *node )
{
    npa_disable_node( node );
  return EFI_SUCCESS;
}

/**
 * <!-- EFI_npa_fork_resource -->
 *
 * @brief Stubbed. Retained only for source compatibility with client code.
 *
 * @return       EFI_SUCCESS
 */
static EFI_STATUS EFI_npa_fork_resource
(
    npa_resource       *resource,
    npa_join_function   join_func,
    void               *join_data,
    npa_join_token     *retval
)
{
    *retval = npa_fork_resource( resource, join_func, join_data );

  return EFI_SUCCESS;
}


/**
 * <!-- npa_resource_lock -->
 *
 * @brief  Lock the resource.
 *
 * If the resource is forked, it will be joined before this function returns.
 * Note that the resource shares its lock with other resources in the node.
 * So this will, in effect, lock the node.
 *
 * @param resource : Handle to the resource
 *
 * @return       EFI_SUCCESS
 */
static EFI_STATUS EFI_npa_resource_lock( npa_resource *resource )
{
    npa_resource_lock( resource );
  return EFI_SUCCESS;
}


/**
 * <!-- npa_resource_unlock -->
 *
 * @brief  Unlock the resource (and by extension, the node) that was previously
 * locked by npa_resource_lock.
 *   
 * @param resource : Handle to the resource
 *
 * @return       EFI_SUCCESS
 */
static EFI_STATUS EFI_npa_resource_unlock( npa_resource *resource )
{
    npa_resource_unlock( resource );
  return EFI_SUCCESS;
}


/**
 * <!-- EFI_npa_request_has_attribute -->
 *
 * @brief Returns true if the current request  has the given attribute set.
 * 
 * Attributes may be set on client handles and this is cached in the resource
 * structure for the duration of the request.
 *
 * @param client : Handle to the client to check
 * @param attr : The attribute to test for
 * @param retval : TRUE if the attribute is present
 *
 * @return       EFI_SUCCESS
 */
static EFI_STATUS EFI_npa_request_has_attribute
(
    npa_client_handle     client,
    npa_request_attribute attr,
    unsigned int *retval
)
{
    *retval = npa_request_has_attribute( client, attr );
  return EFI_SUCCESS;
}

/**
 * <!-- EFI_npa_get_request_attributes -->
 *
 * @brief  Requests a bitset representing all  request attributes set on this client.
 *
 * @param client : Handle of the npa_client to read
 * @param retval : a bitset representing all  request attributes set on this client
 *
 * @return       EFI_SUCCESS
 */
static EFI_STATUS EFI_npa_get_request_attributes
(
    npa_client_handle client,
    unsigned int *retval
)
{
    *retval = npa_get_request_attributes( client );
  return EFI_SUCCESS;
}


/**
 * <!-- EFI_npa_pass_request_attributes -->
 *
 * @brief Add request attributes of the 'current' client to the 'dependency' client.
 *
 * Does not unset any attributes set previously on the 'dependency' client.
 *
 * @param current : Handle of a npa_client who's attributes will be applied
 *                  to the other.
 * @param dependency : Handle of a npa_client who's attributes will be updated. 
 *
 * @return       EFI_SUCCESS
 */
static EFI_STATUS EFI_npa_pass_request_attributes
(
    npa_client_handle current,
    npa_client_handle dependency
)
{
    npa_pass_request_attributes( current, dependency );
  return EFI_SUCCESS;
}


/*======================================================================
  Internal Client API

  Note: The internal client type is created/destroyed via the standard
  client api. 
  ======================================================================*/

/**
 * <!-- EFI_npa_issue_internal_request -->
 *
 * @brief  Issue the INTERNAL client work request
 *
 * Note: The semantics of an internal client request are
 * resource/plugin specific, there's no general API for setting the
 * work request fields - it's the responsibility of the user to set the
 * internal client state appropriately outside of the framework.  This
 * function is used to issue the request after the internal fields have
 * been established.
 *
 * The client must be of INTERNAL work model. Clients with internal
 * client types are created/destroyed via the standard client api.
 *
 * @param client : handle to client being used
 *
 * @return       EFI_SUCCESS
 */
static EFI_STATUS EFI_npa_issue_internal_request( npa_client_handle client )
{
    npa_issue_internal_request( client );
  return EFI_SUCCESS;
}

/*======================================================================
  Event API
  ======================================================================*/

/**
 * <!-- EFI_npa_resource_add_system_event_callback -->
 *
 * @brief Add a system event callback function to a resource
 *  
 * The callback function will be invoked on such event modifications as follows:
 * 1) A change event is created or destroyed;
 * 2) A watermark or threshold event is enabled, disabled or destroyed;
 * 3) Event watermarks or thresholds are changed.
 *
 * When invoked, the callback arguments will be 
 * (context, 0, npa_event *, sizeof(npa_event) ); 
 *
 * Note: a resource supports only one system event callback, so it is an error
 * to add a second. It is hoped that future versions will relax this 
 * restriction.
 *
 * @param resource_name : name of the resource that is to be used
 * @param callback : callback to be invoked
 * @param context : context to be passed back in callback invocation
 * @param retval : NPA_SUCCESS on sucess, NPA_ERROR otherwise.
 *
 * @return       EFI_SUCCESS
 */
static EFI_STATUS EFI_npa_resource_add_system_event_callback
(
    const char   *resource_name,
    npa_callback  callback,
    void         *context,
    npa_status   *retval
)
{
    *retval = npa_resource_add_system_event_callback( resource_name, callback, context );
  return EFI_SUCCESS;
}


/**
 * <!-- EFI_npa_resource_remove_system_event_callback -->
 *
 * @brief Remove a system event callback function from a resource
 *  
 * The callback and context pointers should be the same as those when the 
 * callback is added.
 *
 * @param resource_name : name of the resource that is to be used
 * @param callback : callback to be invoked
 * @param context : context to be passed back in callback invocation
 * @param retval : NPA_SUCCESS on sucess, NPA_ERROR otherwise.
 *
 * @return       EFI_SUCCESS
 */
static EFI_STATUS EFI_npa_resource_remove_system_event_callback
(
    const char   *resource_name,
    npa_callback  callback,
    void         *context,
    npa_status   *retval
)
{
    *retval = npa_resource_remove_system_event_callback( resource_name, callback, context );
  return EFI_SUCCESS;
}

/*=========================================================================
      Data Declarations
==========================================================================*/
static EFI_NPA_PROTOCOL NpaProtocol =
{
  EFI_NPA_PROTOCOL_REVISION,
  EFI_npa_init,
  EFI_npa_create_sync_client_ex,
  EFI_npa_complete_request,
  EFI_npa_define_node_cb,
  EFI_npa_issue_scalar_request,
  EFI_npa_resource_available_cb,
  EFI_npa_remote_define_resource_cb,
  EFI_npa_remote_resource_local_aggregation_driver_fcn,
  EFI_npa_remote_resource_local_aggregation_no_initial_request_driver_fcn,
  EFI_npa_remote_resource_remote_aggregation_driver_fcn,
  EFI_npa_remote_resource_available,
  EFI_npa_dal_event_callback,
  EFI_npa_resources_available_cb,
  EFI_npa_query_resource_available,
  EFI_npa_destroy_client,
  EFI_npa_modify_required_request,
  EFI_npa_issue_impulse_request,
  EFI_npa_issue_vector_request,
  EFI_npa_create_query_handle,
  EFI_npa_destroy_query_handle,
  EFI_npa_query,
  EFI_npa_query_by_name,
  EFI_npa_query_by_client,
  EFI_npa_query_by_event,
  EFI_npa_create_event_cb,
  EFI_npa_set_event_watermarks,
  EFI_npa_set_event_thresholds,
  EFI_npa_destroy_event_handle,
  EFI_npa_cancel_request,
  EFI_npa_set_client_fork_pref,
  EFI_npa_get_client_fork_pref,
  EFI_npa_join_request,
  EFI_npa_set_request_attribute,
  EFI_npa_create_async_client_cb_ex,
  EFI_npa_issue_isoc_request,
  EFI_npa_issue_limit_max_request,
  EFI_npa_alias_resource_cb,
  EFI_npa_define_marker,
  EFI_npa_define_marker_with_attributes,
  EFI_npa_assign_resource_state,
  EFI_npa_query_get_resource,
  EFI_npa_enable_node,
  EFI_npa_disable_node,
  EFI_npa_fork_resource,
  EFI_npa_resource_lock,
  EFI_npa_resource_unlock,
  EFI_npa_request_has_attribute,
  EFI_npa_get_request_attributes,
  EFI_npa_pass_request_attributes,
  EFI_npa_issue_internal_request,
  EFI_npa_resource_add_system_event_callback,
  EFI_npa_resource_remove_system_event_callback,
  EFI_npa_deinit,
};

/**
 * <!-- EFI_npa_deinit -->
 *
 * @brief Npa destructor 
 *
 * @return       EFI_SUCCESS
 */
static EFI_STATUS EFI_npa_deinit(void)
{
   // rpmclient_deinit();
    EFI_STATUS  Status = EFI_SUCCESS;
    //
    // Unpublish our protocols (NpaProtocol, RpmServicesProtocol)
    //
    Status = gBS->UninstallMultipleProtocolInterfaces (
                    gProtocolHandle,
                    &gEfiNpaProtocolGuid,(void **)&NpaProtocol,
                    NULL );
  return Status;
}

/**
 * Entry point for the NPA DXE driver.
 */
EFI_STATUS EFIAPI NpaDxeEntryPoint(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable)
{
  EFI_STATUS  status;

  //Initialize NPA
  npa_init();

  //Initialize RPM
  //rpmclient_init();

  /* ********************************* */

    status = gBS->InstallMultipleProtocolInterfaces(
                    &gProtocolHandle,
                    &gEfiNpaProtocolGuid,(void **)&NpaProtocol,
                    NULL );

  return status;
}

