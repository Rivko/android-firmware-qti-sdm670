/** @file npawrapper_remote_resource.c
   
  Map NPA Remoting api in common code to NPA DXE interface

  Copyright (c) 2013-2015, Qualcomm Technologies Inc. All rights reserved. 

**/

/*=============================================================================
                              EDIT HISTORY

when       who     what, where, why
--------   ---     -----------------------------------------------------------
05/05/15   rli     Add error log if NPA protocol is not located
03/27/13   rli     Initial revision.
=============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Protocol/EFINpa.h>


/**
 * <!-- npa_remote_define_resource_cb -->
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
 * @return None
 */
void npa_remote_define_resource_cb( npa_remote_resource_definition *resource, 
                                    npa_resource_state              initial_state,
                                    npa_callback                    define_callback,
                                    npa_user_data                   define_context )
{
    EFI_NPA_PROTOCOL * npa_protocol;
    EFI_STATUS status;
        status = gBS->LocateProtocol(&gEfiNpaProtocolGuid, NULL, (void**)&npa_protocol);
        if(status == EFI_SUCCESS)
        {
            npa_protocol->RemoteDefineResourceCb( resource, initial_state, define_callback, define_context);
        }
        else
        {
            DEBUG ((EFI_D_ERROR, "Locate NpaProtocol failed:%r\n", status));
        }
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

npa_resource_state npa_remote_resource_local_aggregation_driver_fcn( 
  struct npa_resource *resource,
  npa_client_handle    client,
  npa_resource_state   state 
)
{
    EFI_NPA_PROTOCOL * npa_protocol;
    EFI_STATUS status;
	npa_resource_state ret_state = 0;
        status = gBS->LocateProtocol(&gEfiNpaProtocolGuid, NULL, (void**)&npa_protocol);
        if(status == EFI_SUCCESS)
        {
            npa_protocol->RemoteResourceLocalAggregationFcn( resource, client, state, &ret_state);
        }
        else
        {
            DEBUG ((EFI_D_ERROR, "Locate NpaProtocol failed:%r\n", status));
        }
   return ret_state;
}

npa_resource_state 
npa_remote_resource_local_aggregation_no_initial_request_driver_fcn (
  struct npa_resource *resource,
  npa_client_handle    client,
  npa_resource_state   state 
  )
{
    EFI_NPA_PROTOCOL * npa_protocol;
    EFI_STATUS status;
	npa_resource_state ret_state = 0;
        status = gBS->LocateProtocol(&gEfiNpaProtocolGuid, NULL, (void**)&npa_protocol);
        if(status == EFI_SUCCESS)
        {
            npa_protocol->RemoteResourceLocalAggregationNoInitialRequestDriverFcn( resource, client, state, &ret_state);
        }
        else
        {
            DEBUG ((EFI_D_ERROR, "Locate NpaProtocol failed:%r\n", status));
        }
   return ret_state;
}


npa_resource_state 
npa_remote_resource_remote_aggregation_driver_fcn (
  struct npa_resource *resource,
  npa_client_handle    client,
  npa_resource_state   state 
  )
{
    EFI_NPA_PROTOCOL * npa_protocol;
    EFI_STATUS status;
	npa_resource_state ret_state = 0;
        status = gBS->LocateProtocol(&gEfiNpaProtocolGuid, NULL, (void**)&npa_protocol);
        if(status == EFI_SUCCESS)
        {
            npa_protocol->RemoteResourceRemoteAggregationDriverFcn( resource, client, state, &ret_state);
        }
        else
        {
            DEBUG ((EFI_D_ERROR, "Locate NpaProtocol failed:%r\n", status));
        }
   return ret_state;
}


/**
 * <!-- npa_remote_resource_available -->
 *
 * The remote protocol's query function must respond to a query of
 * type NPA_QUERY_REMOTE_RESOURCE_AVAILABLE, by invoking this function
 * with 'query_result.reference' as context, whenever it is that
 * the remote resource is available.
 */
void npa_remote_resource_available( void *context )
{
    EFI_NPA_PROTOCOL * npa_protocol;
    EFI_STATUS status;
        status = gBS->LocateProtocol(&gEfiNpaProtocolGuid, NULL, (void**)&npa_protocol);
        if(status == EFI_SUCCESS)
        {
            npa_protocol->RemoteResourceAvailable( context );
        }
        else
        {
            DEBUG ((EFI_D_ERROR, "Locate NpaProtocol failed:%r\n", status));
        }
}



