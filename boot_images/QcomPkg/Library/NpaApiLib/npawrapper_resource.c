/** @file npawrapper_resource.c
   
  Map NPA Resource api in common code to NPA DXE interface

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

/*----------------------------------------------------------------------------
 * NPA Resource API
 * -------------------------------------------------------------------------*/

/**
 * <!-- npa_define_node_cb -->
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
 */

void npa_define_node_cb( npa_node_definition *node, 
                         npa_resource_state   initial_state[],
                         npa_callback         node_cb,
                         void                *context)
{
    EFI_NPA_PROTOCOL * npa_protocol;
    EFI_STATUS status;
        status = gBS->LocateProtocol(&gEfiNpaProtocolGuid, NULL, (void**)&npa_protocol);
        if(status == EFI_SUCCESS)
        {
            npa_protocol->DefineNodeCb( node, 
                                        initial_state,
                                        node_cb,
                                        context);
        }
        else
        {
            DEBUG ((EFI_D_ERROR, "Locate NpaProtocol failed:%r\n", status));
        }
}


/**
 * <!-- npa_alias_resource_cb -->
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
 */ 
void npa_alias_resource_cb( const char   *resource_name, 
                            const char   *alias_name,
                            npa_callback  alias_cb,
                            void         *context)
{
    EFI_NPA_PROTOCOL * npa_protocol;
    EFI_STATUS status;
        status = gBS->LocateProtocol(&gEfiNpaProtocolGuid, NULL, (void**)&npa_protocol);
        if(status == EFI_SUCCESS)
        {
            npa_protocol->AliasResourceCb( resource_name, 
                                           alias_name,
                                           alias_cb,
                                           context
                                          );
        }
        else
        {
            DEBUG ((EFI_D_ERROR, "Locate NpaProtocol failed:%r\n", status));
        }
}

/**
 * <!-- npa_define_marker -->
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
 */ 
void npa_define_marker( const char *marker_name )
{
    EFI_NPA_PROTOCOL * npa_protocol;
    EFI_STATUS status;
        status = gBS->LocateProtocol(&gEfiNpaProtocolGuid, NULL, (void**)&npa_protocol);
        if(status == EFI_SUCCESS)
        {
            npa_protocol->DefineMarker( marker_name );
        }
        else
        {
            DEBUG ((EFI_D_ERROR, "Locate NpaProtocol failed:%r\n", status));
        }
}


/**
 * <!-- npa_define_marker_with_attributes -->
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
 */
void npa_define_marker_with_attributes( const char             *marker_name,
                                        npa_resource_attribute  attributes )
{
    EFI_NPA_PROTOCOL * npa_protocol;
    EFI_STATUS status;
        status = gBS->LocateProtocol(&gEfiNpaProtocolGuid, NULL, (void**)&npa_protocol);
        if(status == EFI_SUCCESS)
        {
            npa_protocol->DefineMarkerWithAttributes( marker_name, attributes);
        }
        else
        {
            DEBUG ((EFI_D_ERROR, "Locate NpaProtocol failed:%r\n", status));
        }
}



/*======================================================================
  Author API

  These are special purpose functions intended for use by the node
  author.  
  ======================================================================*/
/**
 * <!-- npa_assign_resource_state -->
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
 */
void npa_assign_resource_state( npa_resource       *resource,
                                npa_resource_state  state )
{
    EFI_NPA_PROTOCOL * npa_protocol;
    EFI_STATUS status;
        status = gBS->LocateProtocol(&gEfiNpaProtocolGuid, NULL, (void**)&npa_protocol);
        if(status == EFI_SUCCESS)
        {
            npa_protocol->AssignResourceState( resource, state);
        }
        else
        {
            DEBUG ((EFI_D_ERROR, "Locate NpaProtocol failed:%r\n", status));
        }
}


/**
 * <!-- npa_query_get_resource -->
 *
 * @brief Retrieve the resource associated with the given query handle.
 *
 * This function is intended to be used by resources that wish to define a
 * query API that goes beyond what npa_resource_query_fcn can support.
 *   
 * @param query_handle : Handle to resource being queried
 *
 * @return Pointer to resource associated with the query handle
 */
npa_resource *npa_query_get_resource( npa_query_handle query_handle )
{
    EFI_NPA_PROTOCOL * npa_protocol;
    EFI_STATUS status;
    npa_resource *resource = NULL;
        status = gBS->LocateProtocol(&gEfiNpaProtocolGuid, NULL, (void**)&npa_protocol);
        if(status == EFI_SUCCESS)
        {
            npa_protocol->QueryGetResource( query_handle, &resource);
        }
        else
        {
            DEBUG ((EFI_D_ERROR, "Locate NpaProtocol failed:%r\n", status));
        }
    return resource;
}


/**
 * <!-- npa_enable_node -->
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
 */
void npa_enable_node( npa_node_definition *node, npa_resource_state default_state[] )
{
    EFI_NPA_PROTOCOL * npa_protocol;
    EFI_STATUS status;
        status = gBS->LocateProtocol(&gEfiNpaProtocolGuid, NULL, (void**)&npa_protocol);
        if(status == EFI_SUCCESS)
        {
            npa_protocol->EnableNode( node, default_state);
        }
        else
        {
            DEBUG ((EFI_D_ERROR, "Locate NpaProtocol failed:%r\n", status));
        }
}


/**
 * <!-- npa_disable_node -->
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
 */
void npa_disable_node( npa_node_definition *node ) 
{
    EFI_NPA_PROTOCOL * npa_protocol;
    EFI_STATUS status;
        status = gBS->LocateProtocol(&gEfiNpaProtocolGuid, NULL, (void**)&npa_protocol);
        if(status == EFI_SUCCESS)
        {
            npa_protocol->DisableNode( node);
        }
        else
        {
            DEBUG ((EFI_D_ERROR, "Locate NpaProtocol failed:%r\n", status));
        }
}


/**
 * <!-- npa_fork_resource -->
 *
 * @brief Stubbed. Retained only for source compatibility with client code.
 *
 * @return Always NULL.
 */
npa_join_token npa_fork_resource( npa_resource       *resource,
                                  npa_join_function   join_func,
                                  void               *join_data )
{
    EFI_NPA_PROTOCOL * npa_protocol;
    EFI_STATUS status;
    npa_join_token retval = NULL;
        status = gBS->LocateProtocol(&gEfiNpaProtocolGuid, NULL, (void**)&npa_protocol);
        if(status == EFI_SUCCESS)
        {
            npa_protocol->ForkResource( resource, join_func, join_data, &retval);
        }
        else
        {
            DEBUG ((EFI_D_ERROR, "Locate NpaProtocol failed:%r\n", status));
        }
    return retval;
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
 */
void npa_resource_lock( npa_resource *resource )
{
    EFI_NPA_PROTOCOL * npa_protocol;
    EFI_STATUS status;
        status = gBS->LocateProtocol(&gEfiNpaProtocolGuid, NULL, (void**)&npa_protocol);
        if(status == EFI_SUCCESS)
        {
            npa_protocol->ResourceLock( resource);
        }
        else
        {
            DEBUG ((EFI_D_ERROR, "Locate NpaProtocol failed:%r\n", status));
        }
}


/**
 * <!-- npa_resource_unlock -->
 *
 * @brief  Unlock the resource (and by extension, the node) that was previously
 * locked by npa_resource_lock.
 *   
 * @param resource : Handle to the resource
 */
void npa_resource_unlock( npa_resource *resource )
{
    EFI_NPA_PROTOCOL * npa_protocol;
    EFI_STATUS status;
        status = gBS->LocateProtocol(&gEfiNpaProtocolGuid, NULL, (void**)&npa_protocol);
        if(status == EFI_SUCCESS)
        {
            npa_protocol->ResourceUnlock( resource );
        }
        else
        {
            DEBUG ((EFI_D_ERROR, "Locate NpaProtocol failed:%r\n", status));
        }
}


/**
 * <!-- npa_request_has_request -->
 *
 * @brief Returns true if the current request  has the given attribute set.
 * 
 * Attributes may be set on client handles and this is cached in the resource
 * structure for the duration of the request.
 *
 * @param client : Handle to the client to check
 * @param attr : The attribute to test for
 *
 * @return TRUE if the attribute is present
 */
unsigned int npa_request_has_attribute( npa_client_handle     client,
                                        npa_request_attribute attr )
{
    EFI_NPA_PROTOCOL * npa_protocol;
    EFI_STATUS status;
    unsigned int retval = 0;
        status = gBS->LocateProtocol(&gEfiNpaProtocolGuid, NULL, (void**)&npa_protocol);
        if(status == EFI_SUCCESS)
        {
            npa_protocol->RequestHasAttribute( client, attr, &retval);
        }
        else
        {
            DEBUG ((EFI_D_ERROR, "Locate NpaProtocol failed:%r\n", status));
        }
    return retval;
}


/**
 * <!-- npa_get_request_attributes -->
 *
 * @brief  Returns a bitset representing all  request attributes set on this client.
 *
 * @param client : Handle of the npa_client to read
 */
unsigned int npa_get_request_attributes( npa_client_handle client )
{
    EFI_NPA_PROTOCOL * npa_protocol;
    EFI_STATUS status;
    unsigned int retval = 0;
        status = gBS->LocateProtocol(&gEfiNpaProtocolGuid, NULL, (void**)&npa_protocol);
        if(status == EFI_SUCCESS)
        {
            npa_protocol->GetRequestAttributes( client, &retval);
        }
        else
        {
            DEBUG ((EFI_D_ERROR, "Locate NpaProtocol failed:%r\n", status));
        }
    return retval;
}


/**
 * <!-- npa_pass_request_attributes -->
 *
 * @brief Add request attributes of the 'current' client to the 'dependency' client.
 *
 * Does not unset any attributes set previously on the 'dependency' client.
 *
 * @param current : Handle of a npa_client who's attributes will be applied
 *                  to the other.
 * @param dependency : Handle of a npa_client who's attributes will be updated. 
 */
npa_client_handle npa_pass_request_attributes( npa_client_handle current,
                                  npa_client_handle dependency )
{
    EFI_NPA_PROTOCOL * npa_protocol;
    EFI_STATUS status;
        status = gBS->LocateProtocol(&gEfiNpaProtocolGuid, NULL, (void**)&npa_protocol);
        if(status == EFI_SUCCESS)
        {
            npa_protocol->PassRequestAttributes( current, dependency );
        }
        else
        {
            DEBUG ((EFI_D_ERROR, "Locate NpaProtocol failed:%r\n", status));
        }
	return dependency;
}


/*======================================================================
  Internal Client API

  Note: The internal client type is created/destroyed via the standard
  client api. 
  ======================================================================*/

/**
 * <!-- npa_issue_internal_request -->
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
 */
void npa_issue_internal_request( npa_client_handle client )
{
    EFI_NPA_PROTOCOL * npa_protocol;
    EFI_STATUS status;
        status = gBS->LocateProtocol(&gEfiNpaProtocolGuid, NULL, (void**)&npa_protocol);
        if(status == EFI_SUCCESS)
        {
            npa_protocol->IssueInternalRequest( client );
        }
        else
        {
            DEBUG ((EFI_D_ERROR, "Locate NpaProtocol failed:%r\n", status));
        }
}

/*======================================================================
  Event API
  ======================================================================*/

/**
 * <!-- npa_resource_add_system_event_callback -->
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
 *
 * @return NPA_SUCCESS on sucess, NPA_ERROR otherwise.
 */
npa_status npa_resource_add_system_event_callback( const char   *resource_name,
                                                   npa_callback  callback,
                                                   void         *context )
{
    EFI_NPA_PROTOCOL * npa_protocol;
    EFI_STATUS status;
    npa_status retval = NPA_ERROR;
        status = gBS->LocateProtocol(&gEfiNpaProtocolGuid, NULL, (void**)&npa_protocol);
        if(status == EFI_SUCCESS)
        {
            npa_protocol->ResourceAddSystemEventCallback( resource_name, callback, context, &retval);
        }
        else
        {
            DEBUG ((EFI_D_ERROR, "Locate NpaProtocol failed:%r\n", status));
        }
    return retval;
}


/**
 * <!-- npa_resource_remove_system_event_callback -->
 *
 * @brief Remove a system event callback function from a resource
 *  
 * The callback and context pointers should be the same as those when the 
 * callback is added.
 *
 * @param resource_name : name of the resource that is to be used
 * @param callback : callback to be invoked
 * @param context : context to be passed back in callback invocation
 *
 * @return NPA_SUCCESS on sucess, NPA_ERROR otherwise.
 */
npa_status npa_resource_remove_system_event_callback( const char   *resource_name,
                                                      npa_callback  callback,
                                                      void         *context )
{
    EFI_NPA_PROTOCOL * npa_protocol;
    EFI_STATUS status;
    npa_status retval = NPA_ERROR;
        status = gBS->LocateProtocol(&gEfiNpaProtocolGuid, NULL, (void**)&npa_protocol);
        if(status == EFI_SUCCESS)
        {
            npa_protocol->ResourceRemoveSystemEventCallback( resource_name, callback, context, &retval);
        }
        else
        {
            DEBUG ((EFI_D_ERROR, "Locate NpaProtocol failed:%r\n", status));
        }
    return retval;
}


