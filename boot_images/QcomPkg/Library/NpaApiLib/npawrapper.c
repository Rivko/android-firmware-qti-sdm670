/** @file npawrapper.c
   
  Map NPA api in common code to NPA DXE interface

  Copyright (c) 2013-2015, Qualcomm Technologies Inc. All rights reserved. 

**/

/*=============================================================================
                              EDIT HISTORY

when       who     what, where, why
--------   ---     -----------------------------------------------------------
05/05/15   rli     Add error log if NPA protocol is not located
03/27/13   rli     Initial revision.
=============================================================================*/
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Protocol/EFINpa.h>



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
 *
 * @return Handle to new client, NULL if client could not be created
 */
npa_client_handle npa_create_sync_client_ex( const char       *resource_name, 
                                             const char       *client_name,
                                             npa_client_type   client_type,
                                             unsigned int      client_value,
                                             void             *client_ref )
{
    EFI_NPA_PROTOCOL * npa_protocol;
    EFI_STATUS status;
    npa_client_handle handle = NULL;
        status = gBS->LocateProtocol(&gEfiNpaProtocolGuid, NULL, (void**)&npa_protocol);
        if(status == EFI_SUCCESS)
        {
            npa_protocol->CreateSyncClientEx( resource_name, 
                                              client_name,
                                              client_type,
                                              client_value,
                                              client_ref,
                                              &handle);
        }
        else
        {
            DEBUG ((EFI_D_ERROR, "Locate NpaProtocol failed:%r\n", status));
        }
    return handle;
}

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
 *                      work requests
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
                               void             *client_ref )
{
    EFI_NPA_PROTOCOL * npa_protocol;
    EFI_STATUS status;
    npa_client_handle handle = NULL;
        status = gBS->LocateProtocol(&gEfiNpaProtocolGuid, NULL, (void**)&npa_protocol);
        if(status == EFI_SUCCESS)
        {
            npa_protocol->CreateAsyncClientCbEx( resource_name,
                                                 client_name,
                                                 client_type,
                                                 callback,
                                                 context,
                                                 client_value,
                                                 client_ref,
                                                 &handle
                                               );
        }
        else
        {
            DEBUG ((EFI_D_ERROR, "Locate NpaProtocol failed:%r\n", status));
        }
    return handle;
}


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
void npa_destroy_client( npa_client_handle client )
{
    EFI_NPA_PROTOCOL * npa_protocol;
    EFI_STATUS status;
        status = gBS->LocateProtocol(&gEfiNpaProtocolGuid, NULL, (void**)&npa_protocol);
        if(status == EFI_SUCCESS)
        {
            npa_protocol->DestroyClient( client );
        }
        else
        {
            DEBUG ((EFI_D_ERROR, "Locate NpaProtocol failed:%r\n", status));
        }
}


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
                               npa_resource_state  state )
{
    EFI_NPA_PROTOCOL * npa_protocol;
    EFI_STATUS status;
        status = gBS->LocateProtocol(&gEfiNpaProtocolGuid, NULL, (void**)&npa_protocol);
        if(status == EFI_SUCCESS)
        {
            npa_protocol->ScalarRequest( client, state);
        }
        else
        {
            DEBUG ((EFI_D_ERROR, "Locate NpaProtocol failed:%r\n", status));
        }
}



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
                                  npa_resource_state_delta  delta )
{
    EFI_NPA_PROTOCOL * npa_protocol;
    EFI_STATUS status;
        status = gBS->LocateProtocol(&gEfiNpaProtocolGuid, NULL, (void**)&npa_protocol);
        if(status == EFI_SUCCESS)
        {
            npa_protocol->ModifyRequiredRequest( client, delta);
        }
        else
        {
            DEBUG ((EFI_D_ERROR, "Locate NpaProtocol failed:%r\n", status));
        }
}

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
void npa_issue_impulse_request( npa_client_handle client )
{
    EFI_NPA_PROTOCOL * npa_protocol;
    EFI_STATUS status;
        status = gBS->LocateProtocol(&gEfiNpaProtocolGuid, NULL, (void**)&npa_protocol);
        if(status == EFI_SUCCESS)
        {
            npa_protocol->IssueImpulseRequest( client );
        }
        else
        {
            DEBUG ((EFI_D_ERROR, "Locate NpaProtocol failed:%r\n", status));
        }
}


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
 */
void npa_issue_vector_request( npa_client_handle   client, 
                               unsigned int        num_elems, 
                               npa_resource_state *vector )
{
    EFI_NPA_PROTOCOL * npa_protocol;
    EFI_STATUS status;
        status = gBS->LocateProtocol(&gEfiNpaProtocolGuid, NULL, (void**)&npa_protocol);
        if(status == EFI_SUCCESS)
        {
            npa_protocol->IssueVectorRequest( client, num_elems, vector);
        }
        else
        {
            DEBUG ((EFI_D_ERROR, "Locate NpaProtocol failed:%r\n", status));
        }
}

/** 
 * <!-- npa_issue_isoc_request -->
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
 */
void npa_issue_isoc_request( npa_client_handle   client,
                             npa_resource_time   deadline,
                             npa_resource_state  level_hint )
{
    EFI_NPA_PROTOCOL * npa_protocol;
    EFI_STATUS status;
        status = gBS->LocateProtocol(&gEfiNpaProtocolGuid, NULL, (void**)&npa_protocol);
        if(status == EFI_SUCCESS)
        {
            npa_protocol->IssueIsocRequest( client, deadline, level_hint);
        }
        else
        {
            DEBUG ((EFI_D_ERROR, "Locate NpaProtocol failed:%r\n", status));
        }
}


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
                                  npa_resource_state max )
{
    EFI_NPA_PROTOCOL * npa_protocol;
    EFI_STATUS status;
        status = gBS->LocateProtocol(&gEfiNpaProtocolGuid, NULL, (void**)&npa_protocol);
        if(status == EFI_SUCCESS)
        {
            npa_protocol->IssueLimitMaxRequest( client, max );
        }
        else
        {
            DEBUG ((EFI_D_ERROR, "Locate NpaProtocol failed:%r\n", status));
        }
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
 */
void npa_complete_request( npa_client_handle client )
{
    EFI_NPA_PROTOCOL * npa_protocol;
    EFI_STATUS status;
        status = gBS->LocateProtocol(&gEfiNpaProtocolGuid, NULL, (void**)&npa_protocol);
        if(status == EFI_SUCCESS)
        {
            npa_protocol->CompleteRequest(client);
        }
        else
        {
            DEBUG ((EFI_D_ERROR, "Locate NpaProtocol failed:%r\n", status));
        }
}

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
void npa_cancel_request( npa_client_handle client )
{
    EFI_NPA_PROTOCOL * npa_protocol;
    EFI_STATUS status;
        status = gBS->LocateProtocol(&gEfiNpaProtocolGuid, NULL, (void**)&npa_protocol);
        if(status == EFI_SUCCESS)
        {
            npa_protocol->CancelRequest(client);
        }
        else
        {
            DEBUG ((EFI_D_ERROR, "Locate NpaProtocol failed:%r\n", status));
        }
}


/** 
 * <!-- npa_set_client_fork_pref -->
 *
 * @brief Stubbed. Does nothing. Retained for SC reasons.
 */
void npa_set_client_fork_pref( npa_client_handle client,
                               npa_fork_pref pref )
{
    EFI_NPA_PROTOCOL * npa_protocol;
    EFI_STATUS status;
        status = gBS->LocateProtocol(&gEfiNpaProtocolGuid, NULL, (void**)&npa_protocol);
        if(status == EFI_SUCCESS)
        {
            npa_protocol->SetClientForkPref( client, pref);
        }
        else
        {
            DEBUG ((EFI_D_ERROR, "Locate NpaProtocol failed:%r\n", status));
        }
}


/** 
 * <!-- npa_get_client_fork_pref -->
 *
 * @brief Stubbed. Always returns NPA_FORK_DEFAULT
 */
npa_fork_pref npa_get_client_fork_pref( npa_client_handle client )
{
    EFI_NPA_PROTOCOL * npa_protocol;
    EFI_STATUS status;
    npa_fork_pref pref = NPA_FORK_DEFAULT;
        status = gBS->LocateProtocol(&gEfiNpaProtocolGuid, NULL, (void**)&npa_protocol);
        if(status == EFI_SUCCESS)
        {
            npa_protocol->GetClientForkPref( client, &pref);
        }
        else
        {
            DEBUG ((EFI_D_ERROR, "Locate NpaProtocol failed:%r\n", status));
        }
    return pref;
}



/** 
 * <!-- npa_join_request -->
 *
 * @brief Stubbed. Does nothing. Retained for SC reasons.
 */
void npa_join_request( npa_client_handle client )
{
    EFI_NPA_PROTOCOL * npa_protocol;
    EFI_STATUS status;
        status = gBS->LocateProtocol(&gEfiNpaProtocolGuid, NULL, (void**)&npa_protocol);
        if(status == EFI_SUCCESS)
        {
            npa_protocol->JoinRequest( client );
        }
        else
        {
            DEBUG ((EFI_D_ERROR, "Locate NpaProtocol failed:%r\n", status));
        }
}

/** 
 * <!-- npa_set_request_attribute -->
 *
 * @brief Associates the given attribute to the next request from this client.
 *  
 * Request attributes only apply to the first issue_request after
 * npa_set_request_attribute 
 */
void npa_set_request_attribute( npa_client_handle     client,
                                npa_request_attribute attr )
{
    EFI_NPA_PROTOCOL * npa_protocol;
    EFI_STATUS status;
        status = gBS->LocateProtocol(&gEfiNpaProtocolGuid, NULL, (void**)&npa_protocol);
        if(status == EFI_SUCCESS)
        {
            npa_protocol->SetRequestAttribute( client, attr);
        }
        else
        {
            DEBUG ((EFI_D_ERROR, "Locate NpaProtocol failed:%r\n", status));
        }
}


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
                     void                  *context )
{
    EFI_NPA_PROTOCOL * npa_protocol;
    EFI_STATUS status;
    npa_event_handle handle = NULL;
        status = gBS->LocateProtocol(&gEfiNpaProtocolGuid, NULL, (void**)&npa_protocol);
        if(status == EFI_SUCCESS)
        {
            npa_protocol->CreateEventCb( resource_name, 
                                         event_name,
                                         trigger_type,
                                         callback,
                                         context,
                                         &handle
                                        );
        }
        else
        {
            DEBUG ((EFI_D_ERROR, "Locate NpaProtocol failed:%r\n", status));
        }
    return handle;
}

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
 */
void npa_set_event_watermarks( npa_event_handle         event,
                               npa_resource_state_delta hi_watermark,
                               npa_resource_state_delta lo_watermark )
{
    EFI_NPA_PROTOCOL * npa_protocol;
    EFI_STATUS status;
        status = gBS->LocateProtocol(&gEfiNpaProtocolGuid, NULL, (void**)&npa_protocol);
        if(status == EFI_SUCCESS)
        {
            npa_protocol->SetEventWatermarks( event, hi_watermark, lo_watermark);
        }
        else
        {
            DEBUG ((EFI_D_ERROR, "Locate NpaProtocol failed:%r\n", status));
        }
}


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
                               npa_resource_state hi_threshold )
{
    EFI_NPA_PROTOCOL * npa_protocol;
    EFI_STATUS status;
        status = gBS->LocateProtocol(&gEfiNpaProtocolGuid, NULL, (void**)&npa_protocol);
        if(status == EFI_SUCCESS)
        {
            npa_protocol->SetEventThresholds( event, lo_threshold, hi_threshold);
        }
        else
        {
            DEBUG ((EFI_D_ERROR, "Locate NpaProtocol failed:%r\n", status));
        }
}


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
void npa_destroy_event_handle( npa_event_handle event )
{
    EFI_NPA_PROTOCOL * npa_protocol;
    EFI_STATUS status;
        status = gBS->LocateProtocol(&gEfiNpaProtocolGuid, NULL, (void**)&npa_protocol);
        if(status == EFI_SUCCESS)
        {
            npa_protocol->DestroyEventHandle( event );
        }
        else
        {
            DEBUG ((EFI_D_ERROR, "Locate NpaProtocol failed:%r\n", status));
        }
}


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
npa_query_handle npa_create_query_handle( const char * resource_name )
{
    EFI_NPA_PROTOCOL * npa_protocol;
    EFI_STATUS status;
    npa_query_handle handle = NULL;
        status = gBS->LocateProtocol(&gEfiNpaProtocolGuid, NULL, (void**)&npa_protocol);
        if(status == EFI_SUCCESS)
        {
            npa_protocol->CreateQueryHandle( resource_name, &handle);
        }
        else
        {
            DEBUG ((EFI_D_ERROR, "Locate NpaProtocol failed:%r\n", status));
        }
    return handle;
}

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
void npa_destroy_query_handle( npa_query_handle query )
{
    EFI_NPA_PROTOCOL * npa_protocol;
    EFI_STATUS status;
        status = gBS->LocateProtocol(&gEfiNpaProtocolGuid, NULL, (void**)&npa_protocol);
        if(status == EFI_SUCCESS)
        {
            npa_protocol->DestroyQueryHandle( query );
        }
        else
        {
            DEBUG ((EFI_D_ERROR, "Locate NpaProtocol failed:%r\n", status));
        }
}



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
                            npa_query_type  *query_result )
{
    EFI_NPA_PROTOCOL * npa_protocol;
    EFI_STATUS status;
    npa_query_status ret_status = NPA_QUERY_NO_VALUE;
        status = gBS->LocateProtocol(&gEfiNpaProtocolGuid, NULL, (void**)&npa_protocol);
        if(status == EFI_SUCCESS)
        {
            npa_protocol->Query( query, query_id, query_result, &ret_status);
        }
        else
        {
            DEBUG ((EFI_D_ERROR, "Locate NpaProtocol failed:%r\n", status));
        }
    return ret_status;
}


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
 *
 * @return success/failure of query (npa_query_status)
 */
npa_query_status npa_query_by_name( const char      *resource_name, 
                                    uint32           query_id, 
                                    npa_query_type  *query_result )
{
    EFI_NPA_PROTOCOL * npa_protocol;
    EFI_STATUS status;
    npa_query_status ret_status = NPA_QUERY_NO_VALUE;
        status = gBS->LocateProtocol(&gEfiNpaProtocolGuid, NULL, (void**)&npa_protocol);
        if(status == EFI_SUCCESS)
        {
            npa_protocol->QueryByName( resource_name, query_id, query_result, &ret_status);
        }
        else
        {
            DEBUG ((EFI_D_ERROR, "Locate NpaProtocol failed:%r\n", status));
        }
    return ret_status;
}



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
                                      npa_query_type    *query_result )
{
    EFI_NPA_PROTOCOL * npa_protocol;
    EFI_STATUS status;
    npa_query_status ret_status = NPA_QUERY_NO_VALUE;
        status = gBS->LocateProtocol(&gEfiNpaProtocolGuid, NULL, (void**)&npa_protocol);
        if(status == EFI_SUCCESS)
        {
            npa_protocol->QueryByClient( client, query_id, query_result, &ret_status);
        }
        else
        {
            DEBUG ((EFI_D_ERROR, "Locate NpaProtocol failed:%r\n", status));
        }
    return ret_status;
}

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
                                     npa_query_type    *query_result )

{
    EFI_NPA_PROTOCOL * npa_protocol;
    EFI_STATUS status;
    npa_query_status ret_status = NPA_QUERY_NO_VALUE;
        status = gBS->LocateProtocol(&gEfiNpaProtocolGuid, NULL, (void**)&npa_protocol);
        if(status == EFI_SUCCESS)
        {
            npa_protocol->QueryByEvent( event, query_id, query_result, &ret_status);
        }
        else
        {
            DEBUG ((EFI_D_ERROR, "Locate NpaProtocol failed:%r\n", status));
        }
    return ret_status;
}
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
npa_query_status npa_query_resource_available( const char *resource_name )
{
    EFI_NPA_PROTOCOL * npa_protocol;
    EFI_STATUS status;
    npa_query_status ret_status = NPA_QUERY_NO_VALUE;
        status = gBS->LocateProtocol(&gEfiNpaProtocolGuid, NULL, (void**)&npa_protocol);
        if(status == EFI_SUCCESS)
        {
            npa_protocol->QueryResourceAvailable( resource_name, &ret_status);
        }
        else
        {
            DEBUG ((EFI_D_ERROR, "Locate NpaProtocol failed:%r\n", status));
        }
    return ret_status;
}

/** 
 * <!-- npa_resources_available_cb -->
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
 */
void npa_resources_available_cb( unsigned int    num_resources,
                                 const char     *resources[],
                                 npa_callback    callback,
                                 void           *context )
{
    EFI_NPA_PROTOCOL * npa_protocol;
    EFI_STATUS status;
        status = gBS->LocateProtocol(&gEfiNpaProtocolGuid, NULL, (void**)&npa_protocol);
        if(status == EFI_SUCCESS)
        {
            npa_protocol->ResourcesAvailableCb( num_resources, resources, callback, context );
        }
        else
        {
            DEBUG ((EFI_D_ERROR, "Locate NpaProtocol failed:%r\n", status));
        }
}


/** 
 * <!-- npa_resource_available_cb -->
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
 */
void npa_resource_available_cb( const char     *resource_name,
                                npa_callback    callback,
                                void           *context )
{
    EFI_NPA_PROTOCOL * npa_protocol;
    EFI_STATUS status;
        status = gBS->LocateProtocol(&gEfiNpaProtocolGuid, NULL, (void**)&npa_protocol);
        if(status == EFI_SUCCESS)
        {
            npa_protocol->ResourceAvailableCb( resource_name, callback, context);
        }
        else
        {
            DEBUG ((EFI_D_ERROR, "Locate NpaProtocol failed:%r\n", status));
        }
}



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
                             unsigned int  data_size )
{

    EFI_NPA_PROTOCOL * npa_protocol;
    EFI_STATUS status;
        status = gBS->LocateProtocol(&gEfiNpaProtocolGuid, NULL, (void**)&npa_protocol);
        if(status == EFI_SUCCESS)
        {
            npa_protocol->DalEventCallback( dal_event_handle, event_type, data, data_size );
        }
        else
        {
            DEBUG ((EFI_D_ERROR, "Locate NpaProtocol failed:%r\n", status));
        }
}

