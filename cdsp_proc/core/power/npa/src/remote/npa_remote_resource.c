/*============================================================================
@file npa_remote_resource.c

NPA Remote API 

Copyright (c) 2010-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

$Header: //components/rel/core.qdsp6/2.1.c4/power/npa/src/remote/npa_remote_resource.c#1 $
============================================================================*/
#include "npa_internal.h"
#include "npa_remote_resource.h"
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/* reuse the event/continuation pool for context structures */
#define npa_remote_node_continuation_pool npa.event_pool
CORE_STATIC_VERIFY( 
     sizeof( npa_remote_node_continuation ) <= sizeof( npa_event ) );

typedef struct npa_remote_resource {
  npa_resource_definition  resource;
  npa_node_definition      node;
  npa_node_dependency      dependencies;
} npa_remote_resource;

/* reuse the resource pool for new resource/node definitions */
#define npa_remote_resource_pool npa.resource_pool
CORE_STATIC_VERIFY( sizeof( npa_remote_resource) <= sizeof( npa_resource ) );

/* Forward declare, so that the code looks as linear as it runs */
static void 
npa_remote_protocol_available( void *context, unsigned int event_type, 
                               void *data, unsigned int data_size );

static void
npa_remote_resource_defined( void *context, unsigned int event_type, 
                             void *data, unsigned int data_size );


/* Stubbed drivers; names used to define aggregation semantics */
npa_resource_state 
npa_remote_resource_local_aggregation_driver_fcn( 
    npa_resource *resource, npa_client *client, npa_resource_state state )
{
  return state;
}

npa_resource_state 
npa_remote_resource_local_aggregation_no_initial_request_driver_fcn(
    npa_resource *resource, npa_client *client, npa_resource_state state )
{
  return state;
}

npa_resource_state 
npa_remote_resource_remote_aggregation_driver_fcn(
    npa_resource *resource, npa_client *client, npa_resource_state state )
{
  return state;
}


/**
 * npa_remote_define_resource_cb
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
void 
npa_remote_define_resource_cb( npa_remote_resource_definition *definition, 
                               npa_resource_state              initial_state,
                               npa_callback                    define_callback,
                               npa_user_data                   define_context )
{
  npa_remote_node_continuation  *remote_node_continuation;
  npa_remote_resource           *remote_resource;

  CORE_VERIFY_PTR( definition );

  CORE_VERIFY( 
    ( NULL != definition->local_resource_name ) &&
    ( NULL != definition->remote_resource_name ) &&
    ( NULL != definition->protocol_type ) &&
    ( NULL != definition->plugin ) &&
    ( NULL != definition->units ) &&
    ( strlen( definition->local_resource_name ) <=  NPA_MAX_STR_LEN) );

  ULOG_RT_PRINTF_4( npa.log, "npa_remote_define_resource_cb "
                    "(handle: 0x%08x) "
                    "(local name: \"%s\") " 
                    "(remote name: \"%s\") " 
                    "(protocol: \"%s\") ",
                    definition,
                    definition->local_resource_name,
                    definition->remote_resource_name,
                    definition->protocol_type );
                    
  CORE_VERIFY_PTR( remote_resource = 
       (npa_remote_resource *)Core_PoolAlloc( &npa_remote_resource_pool ) );
  memset( remote_resource, 0, sizeof(npa_remote_resource) );

  CORE_VERIFY_PTR( remote_node_continuation = (npa_remote_node_continuation *)
                     Core_PoolAlloc( &npa_remote_node_continuation_pool ) );
  remote_node_continuation->initial_state = initial_state;
  remote_node_continuation->callback = define_callback;
  remote_node_continuation->cb_data = define_context;
  remote_node_continuation->definition = definition;

  /* Create resource definition */
  {
    npa_resource_definition *resource = &remote_resource->resource;
    resource->name   = definition->local_resource_name;
    resource->units  = definition->units;
    resource->max    = definition->max;
    resource->plugin = definition->plugin;
    resource->data   = definition->data;

    /* Copy defined attributes and mark resource as remote */
    resource->attributes = definition->attributes | NPA_RESOURCE_REMOTE;

    if ( definition->driver_fcn == 
         npa_remote_resource_local_aggregation_no_initial_request_driver_fcn )
    {
      resource->attributes |= NPA_RESOURCE_REMOTE_NO_INIT;
    }
    else if ( definition->driver_fcn ==
              npa_remote_resource_remote_aggregation_driver_fcn )
    {
      /* Remote aggregation resources should let the protocol define custom
         client_create & destroy routines. Their plugins are reset to the
         npa_identity_plugin, irrespective of what was specified in the
         definition. */
      CORE_VERIFY( definition->plugin->create_client_ex_fcn == NULL &&
                   definition->plugin->create_client_fcn == NULL );
      resource->plugin = &npa_identity_plugin;
      resource->attributes |= NPA_RESOURCE_DRIVER_UNCONDITIONAL;
    }
  }

  /* Create node definition */
  {
    npa_node_definition *node = &remote_resource->node;
    
    /* Save the remote resource name as the node->name */
    node->name = definition->remote_resource_name;
    
    node->attributes = NPA_NODE_DEFAULT;
    node->driver_fcn = definition->driver_fcn;

    node->dependencies = &remote_resource->dependencies;
    node->dependency_count = 1;
    node->dependencies->name = definition->protocol_type;
    node->dependencies->client_type = NPA_NO_CLIENT;
    node->dependencies->handle = NULL;

    node->data = NULL;

    node->resource_count = 1;
    node->resources = &remote_resource->resource;

    /* Save node in continuation */
    remote_node_continuation->node = node;

    ULOG_RT_PRINTF_3( npa.log, 
                      "\tnode (handle: 0x%08x) (name: \"%s\") requires "
                      "protocol \"%s\" ",
                      node, node->name, definition->protocol_type );
  }

  npa_resource_available_cb( definition->protocol_type, 
                             npa_remote_protocol_available,
                             remote_node_continuation);
}


static void 
npa_remote_protocol_available( void         *context,
                               unsigned int  event_type, 
                               void         *data,       
                               unsigned int  data_size )
{
  npa_query_type input;
  npa_query_handle handle;
  npa_resource *protocol;
  npa_remote_node_continuation *remote_node_continuation = 
                                (npa_remote_node_continuation *)context;
  CORE_VERIFY_PTR( remote_node_continuation );

  /* data holds resource name, in this case, the protocol name */
  CORE_VERIFY_PTR( handle = npa_create_query_handle( (const char *)data ) );
  
  protocol = npa_query_get_resource( handle );
  
  /* Set the driver function of the node, from the protocol */
  remote_node_continuation->node->driver_fcn = protocol->node->driver_fcn;

  /* Set the custom query function of the resource, from the protocol */
  remote_node_continuation->node->resources[0].query_fcn = 
                                               protocol->definition->query_fcn;
  
  /* Admittedly, a bastardization of the query mechanism */
  input.type = NPA_QUERY_TYPE_REFERENCE;
  input.data.reference = remote_node_continuation;

  /* The protocol's custom_query_fcn must use this query to confirm that the
   * remote resource is available and invoke npa_remote_resource_available
   * function with the passed in context ('npa_query_type->data.reference')
   * OR
   * Save the context pointer and do something protocol-specific to get 
   * notification when the remote resource is infact available and then,
   * call npa_remote_resource_available
   */
  CORE_VERIFY( 
       NPA_QUERY_SUCCESS == npa_query( handle, 
                                       NPA_QUERY_REMOTE_RESOURCE_AVAILABLE, 
                                       &input ) );
}


void npa_remote_resource_available( void *context )
{
  npa_remote_node_continuation *remote_node_continuation = 
                                (npa_remote_node_continuation *)context;
  CORE_VERIFY_PTR( remote_node_continuation );

  npa_define_node_cb( remote_node_continuation->node,
                      &remote_node_continuation->initial_state,
                      npa_remote_resource_defined,
                      remote_node_continuation );
}


static void
npa_remote_resource_defined( void         *context,
                             unsigned int  event_type, 
                             void         *data,       
                             unsigned int  data_size )
{
  npa_remote_node_continuation *remote_node_continuation = 
                                (npa_remote_node_continuation *)context;
  CORE_VERIFY_PTR( remote_node_continuation );

  /* Unset this attribute, if set; Only used in node definition phase */
  remote_node_continuation->node->resources[0].attributes &= 
                                               ~NPA_RESOURCE_REMOTE_NO_INIT;

  /* Initialize the resource handle field in npa_remote_resource_definition */
  remote_node_continuation->definition->handle = 
              remote_node_continuation->node->resources[0].handle;

  /* User node define callback */
  if ( remote_node_continuation->callback )
  {
    char *local_name = 
          remote_node_continuation->definition->local_resource_name;
    remote_node_continuation->callback( remote_node_continuation->cb_data, 0,
                                        local_name, strlen( local_name ) );
  }
  Core_PoolFree( &npa_remote_node_continuation_pool, remote_node_continuation );
}

#ifdef __cplusplus
}
#endif
