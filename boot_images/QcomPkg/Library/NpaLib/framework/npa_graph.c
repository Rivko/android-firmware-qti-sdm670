/*============================================================================
@file npa_graph.c

Routines that define, create and modify the NPA resource graph

Copyright (c) 2009-2014 QUALCOMM Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR

============================================================================*/

#include "npa_internal.h"
#include "npa_inlines.h"
#include "npa_log.h"
#include "CoreString.h"

#ifndef TARGET_UEFI
    #include <string.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif


/*
  Globals
*/

/* Define the marker structures */
static npa_resource_definition marker_resource_definition = 
{
  "marker resource",
  "none",
  0,
  &npa_no_client_plugin,
  NPA_RESOURCE_DEFAULT,
  /* Rest is defined as 0/NULL */
  NULL,
  NULL,
  NULL,
  NULL, /*handle*/  
};


static npa_node_definition marker_node_definition =
{
  "marker resource",
  NULL,
  NPA_NODE_DEFAULT,
  NULL,
  NPA_EMPTY_ARRAY,
  1,
  &marker_resource_definition
  /* Rest is defined as 0/NULL */
};


npa_resource marker_resource =
{
  &marker_resource_definition,
  0,
  &marker_node_definition,
  NULL,
  {NULL}, 
  &npa_no_client_plugin,
  /* Rest is defined as 0/NULL */
//  #if 0
  0,          /* request_state */
  0,          /* active_state */
  0,          /* required_state */
  0,          /* suppressible_state */
  {0,0,0,0},  /* internal_state[4] */
  NULL,      /* state_vector */
  NULL,      /* required_state_vector */
  NULL,      /* suppressible_state_vector */
  0,          /* active_max */
  0,          /* active_headroom */
  NULL,       /* current_client */
  NULL,       /* node_lock */
  NULL,       /* event_lock */
  NULL,       /* transaction */
  NULL,       /* _internal */
  0,          /*  sequence */
  NULL,        /* log_handle */
  NULL,       /* latency*/
  //#endif
};


/* structures for global markers that work across address space */
static npa_resource_definition global_marker_resource_definition = 
{
  "global marker resource",
  "none",
  0,
  &npa_no_client_plugin,
  NPA_RESOURCE_INTERPROCESS_ACCESS_ALLOWED,
  /* Rest is defined as 0/NULL */
  NULL,         /* data */
  NULL,         /* query_fcn */
  NULL,         /* link_query_fcn */
  NULL          /* handle */
};


static npa_node_definition global_marker_node_definition =
{
  "global marker resource",
  NULL,
  NPA_NODE_DEFAULT,
  NULL,
  NPA_EMPTY_ARRAY,
  1,
  &global_marker_resource_definition
  /* Rest is defined as 0/NULL */
};
npa_resource global_marker_resource =
{
  &global_marker_resource_definition,
  0,
  &global_marker_node_definition,
  NULL,
  {NULL}, 
  &npa_no_client_plugin,
  /* Rest is defined as 0/NULL */

  0,          /* request_state */
  0,          /* active_state */
  0,          /* required_state */
  0,          /* suppressible_state */
  {0,0,0,0},  /* internal_state[4] */
  NULL,      /* state_vector */
  NULL,      /* required_state_vector */
  NULL,      /* suppressible_state_vector */
  0,          /* active_max */
  0,          /* active_headroom */
  NULL, 	  /* current_client */
  NULL,       /* node_lock */
  NULL,       /* event_lock */
  NULL,       /* transaction */
  NULL,       /* _internal */
  0,          /*  sequence */
  NULL,        /* log_handle */
  NULL		  /* latency*/

};


/**
 * <!-- null_update_fcn -->
 *
 * @brief Update function for the npa_resource_plugin "null_plugin" used in /dev/null node. 
 *
 * @return 0
 */
static npa_resource_state null_update_fcn( npa_resource     *resource,
                                           npa_client_handle client )
{
  return 0;
}


/**
* <!-- null_driver_fcn -->
*
* @brief Driver function for the null_node used in the "/dev/null" resource
*
* @return 0
*/
static npa_resource_state null_driver_fcn( npa_resource      *resource,
                                           npa_client_handle  client,
                                           npa_resource_state state )
{
  return 0;
}


static npa_resource_plugin null_plugin =
{
  null_update_fcn,              /* Null update function */
  0xffffffff,                    /* All clients supported */
  NULL,                         /* Client Create Function */
  NULL,                         /* Client Destroy Function */
  NULL,                         /* Extended Client Create Function */
  NULL                          /* Cancel Client Function */

};


static npa_resource_definition null_resource[] =
{
  {
    "/dev/null",                /* Name */
    "n/a",                      /* Units */
    NPA_MAX_STATE,              /* Max value */
    &null_plugin,               /* Plugin */
	0,                          /* Attributes */
    NULL,                       /* Data */
    NULL,                       /* Query_fcn */
    NULL,						/* link_query_fcn */
	NULL,						/* handle */
    
  }
};


npa_node_definition null_node =
{
  "/dev/null",                  /* Name */
  null_driver_fcn,              /* Driver Fcn */
  NPA_NODE_DEFAULT,             /* Attributes */
  NULL,                         /* User Data */
  NPA_EMPTY_ARRAY,              /* Dependencies */
  NPA_ARRAY( null_resource )    /* Resources */
};


/**
 * @brief Alias node is used to signify alias resources by putting the address of this
 * node into the resource->node field.  This will keep the compiler happy and
 * still provide a clean way to catch if a resource is aliased or not.
 */
npa_node_definition npa_aliased_node_def =
{
  "npa_aliased_node_def",       /* Name */
  NULL,                         /* Driver Fcn */
  NPA_NODE_DEFAULT,             /* Attributes */
  NULL,                         /* User Data */
  NPA_EMPTY_ARRAY,              /* Dependencies */
  NPA_EMPTY_ARRAY               /* Resources */
};


/**
 * @brief List of update functions provided by NPA. Used by the framework to mark
 * resources with the NPA_RESOURCE_SUPPORTS_SUPPRESSIBLE attribute */
static npa_resource_update_fcn npa_update_fcn_list[] =
{
  npa_min_update_fcn,
  npa_max_update_fcn,
  npa_sum_update_fcn,
  npa_binary_update_fcn,
  npa_or_update_fcn,
  npa_identity_update_fcn,
  npa_always_on_update_fcn,
  npa_impulse_update_fcn
};


/* 
  Routines
*/

/**
 * <!-- npa_is_resource_disabled -->
 *
 * @brief Check the sequence bit flag for the NPA_RESOURCE_DISABLED bit. 
 */
static NPA_INLINE unsigned int npa_is_resource_disabled( npa_resource *resource )
{
  return ( resource->sequence & NPA_RESOURCE_DISABLED );
}


/**
 * <!-- npa_node_equal -->
 *
 * @brief Verify that the node the user passed in as the same identity as the
 * one in the graph 
 */
static int npa_node_equal( npa_node_definition *user_node,
                           npa_node_definition *graph_node,
                           npa_node_attribute   attr_mask )
{
  unsigned int i;

  CORE_VERIFY_PTR( user_node );
  CORE_VERIFY_PTR( graph_node );
  
  if ( ( 0 != strncmp( user_node->name, 
                       graph_node->name,
                       NPA_MAX_STR_LEN + 1 ) ) ||
       ( ( user_node->attributes & attr_mask ) != 
         ( graph_node->attributes & attr_mask ) ) ||
       ( user_node->dependency_count != graph_node->dependency_count ) ||
       ( user_node->resource_count != graph_node->resource_count ) )
  {
    return 0;
  }

  for( i = 0; i < user_node->dependency_count; i++ )
  {
    if ( 0 != strncmp( user_node->dependencies[i].name, 
                      graph_node->dependencies[i].name,
                      NPA_MAX_STR_LEN + 1 ) )
    {
      return 0;
    }
  }

  for( i = 0; i < user_node->resource_count; i++ )
  {
    if ( 0 != strncmp( user_node->resources[i].name, 
                      graph_node->resources[i].name,
                      NPA_MAX_STR_LEN + 1 ) )
    {
      return 0;
    }
  }
  return 1;
}


/**
 * <!-- npa_copy_buffer -->
 * 
 * @brief Allocate and copy the passed in buffer.
 *
 * @param src_buffer : buffer to copy
 * @param length : the length in bytes to allocate and copy. 
 * 
 * @return Pointer to new buffer with the data copied into it. 
*/
static void *npa_copy_buffer( const void *src_buffer, size_t length )
{
  void *new_buffer = NULL;

  if ( length > 0 )
  {
    new_buffer = Core_Malloc( length );
    CORE_VERIFY_PTR( new_buffer );
    core_memcpy( new_buffer, length, src_buffer, length );
  }

  return new_buffer;
}


/**
 * <!-- npa_copy_name -->
 * 
 * @brief Allocate and copy the string. 
 * 
 * @return A pointer to the new copied string. 
 */
static NPA_INLINE const char *npa_copy_name( const char * name )
{
  return (const char *)npa_copy_buffer( name, strlen(name)+1 );
}


/**
 * <!-- npa_copy_node -->
 * 
 * @brief Allocate and duplicate a node.
 * 
 * @return The new node. 
 */
static npa_node_definition *npa_copy_node( npa_node_definition *user_node )
{
  unsigned int i;
  npa_node_definition *node;

  node = (npa_node_definition *)
    npa_copy_buffer( user_node, sizeof( npa_node_definition ) );
  CORE_VERIFY_PTR( node ); /* Klocworks */

  node->dependencies = ( npa_node_dependency *)
    npa_copy_buffer( user_node->dependencies,
                     node->dependency_count * sizeof( npa_node_dependency ) );
  CORE_VERIFY_PTR( node->dependencies ); /* Klocworks */
  
  node->resources = ( npa_resource_definition *)
    npa_copy_buffer( user_node->resources,
                     node->resource_count * sizeof( npa_resource_definition ) );
  CORE_VERIFY_PTR( node->resources ); /* Klocworks */
  
  node->name = npa_copy_name( node->name );
  for ( i = 0; i < node->dependency_count; i++ )
  {
    node->dependencies[i].name = 
      npa_copy_name( user_node->dependencies[i].name );
  }
  for ( i = 0; i < node->resource_count; i++ )
  {
    node->resources[i].name = npa_copy_name( user_node->resources[i].name );
  }

  return node;
}


/**
 * <!-- npa_find_resource_link -->
 * 
 * @brief Walk the npa_link linked list of resource looking for a name match. 
 *
 * @param resource_name : The name to find. 
 * 
 * @return The found link or NULL.
 */
npa_link *npa_find_resource_link( const char *resource_name )
{
  npa_link *link = npa_get_resources();
  if ( NULL == resource_name ) {return NULL;}

  while ( NULL != link )
  {
    if ( 0 == strncmp(resource_name, 
                      link->name,
                      NPA_MAX_STR_LEN + 1) )
    {
      return link;
    }
    link = link->next;
  }
  return NULL;
}


/**
 * <!-- npa_find_raw_resource -->
 * 
 * @brief Walk the npa_link linked list of resource looking for a name match. 
 *
 * @param resource_name : The name to find. 
 *
 * @return The found resource or NULL
 */
npa_resource *npa_find_raw_resource( const char *resource_name )
{
  npa_link *link = npa_find_resource_link( resource_name );
  if ( link )
  {
    return link->resource;
  }
  return NULL;
}


/**
 * <!-- npa_find_resource_type -->
 * 
 * @brief Finds the resource by name and only returns it if the type is a match. 
 *
 * @param resource_name : The name to match
 * @param type : The npa_resource_type to check the found name against. 
 * 
 * @return The found resource if the type matches or NULL
 */
static NPA_INLINE npa_resource *
npa_find_resource_type( const char *resource_name, npa_resource_type type )
{
  npa_resource *resource = npa_find_raw_resource( resource_name );
  if ( npa_is_resource_type( resource, type ) )
  {
    return resource;
  }
  return NULL;
}


/**
 * <!-- npa_find_defined_resource -->
 * 
 * @brief Find the defined resource matching the name. 
 * 
 * @param resource_name : The name to match
 *
 * @return The found/matched resource or NULL
 */
static NPA_INLINE npa_resource *
npa_find_defined_resource( const char *resource_name )
{
  return npa_find_resource_type( resource_name, NPA_DEFINED_RESOURCE );
}


/** 
 * <!-- npa_find_resource -->
 *
 * This is the cooked interface - it masks out the missing
 * resources.  Most requests should be using this interface 
 */
npa_resource *npa_find_resource( const char *resource_name )
{
  return npa_find_resource_type( resource_name, NPA_INSTANTIATED_RESOURCE );
}


/**
 * <!-- npa_destroy_resource -->
 * 
 * @brief Free the resource and any associated clients. 
 *
 * @param resource : pointer to the resource to destroy
 */
void npa_destroy_resource( npa_resource *resource )
{
  npa_client *client;
  npa_event  *event;
  npa_resource_definition *resource_def;
  npa_node_definition     *node;
  unsigned int i;

  if ( NULL == resource ) {return;}
  
  /* Perhaps I should lock the resource here, but if you're calling
   * into this, you take your chances...  */
  for ( client = resource->clients; client != NULL; client = client->next )
  {
    npa_destroy_client( client );
  }

  if ( npa_is_resource_type( resource, NPA_PLACEHOLDER_RESOURCE ) )
  {
    event = resource->events.event;
  }
  else
  {
    event = resource->events.event_header ? resource->events.event_header->event : NULL;
  }

  for ( ; event != NULL; event = event->next )
  {
    npa_destroy_event_handle( event );
  }

 /* Clear handle */
  NPA_RESOURCE(resource, resource->index) = NULL;

  /* Check if this is the last resource of the node - if so, destroy
   * node objects */
  CORE_VERIFY_PTR( node = resource->node );
  for ( i = 0, resource_def = &node->resources[i]; 
        (i < node->resource_count) && (NULL == resource_def->handle);
        resource_def = &node->resources[++i] )
  { ; }

  /* If all the handles are NULL'd, we can destroy the node_lock */
  if ( i == node->resource_count)
  {
    Core_MutexDestroy( resource->node_lock );
  }

  Core_PoolFree( &npa.resource_pool, resource );
}


/**
 * <!-- npa_destroy_placeholder_link -->
 *
 * @brief Return the npa_link to the pool and call any 
 * NPA_TRIGGER_RESOURCE_AVAILABLE events that are on the resource's event list. 
 *
 * @param placeholder_link : the npa_link to free
 */
static NPA_INLINE void npa_destroy_placeholder_link( npa_link *placeholder_link )
{
  npa_resource *resource;
  npa_event    *event;

  if ( NULL == placeholder_link ) { return; }

  resource = placeholder_link->resource;
  CORE_VERIFY( !npa_is_resource_type( resource, NPA_UNDEFINED_RESOURCE ) &&
               !npa_is_resource_type( resource, NPA_INSTANTIATED_RESOURCE ) );

  event = resource->events.event;
  
  while ( NULL != event )
  {
    npa_event *dead_event;

    CORE_VERIFY( NPA_TRIGGER_RESOURCE_AVAILABLE == event->trigger_type );

    npa_trigger_callback_event( &event->callback,
                                0, 
                                (void *)placeholder_link->name, 
                                strlen( placeholder_link->name ) );
    dead_event = event;
    event = event->next;
    Core_PoolFree( &npa.event_pool, dead_event );
  }

  Core_MutexDestroy( resource->node_lock );

  Core_PoolFree( &npa.resource_pool, resource );
  Core_PoolFree( &npa.link_pool, placeholder_link );
}


/**
 * <!-- npa_cleanup_placeholders -->
 *
 * @brief Loop through the npa.placeholders list removing and calling
 * npa_destroy_placeholder_link on all the items.
 *
 * npa_destroy_placeholder_link will trigger any NPA_TRIGGER_RESOURCE_AVAILABLE
 * events that may have been queued up on the link's placeholder resource. 
 */
static void npa_cleanup_placeholders( void )
{
  npa_link *placeholder;

  do 
  {
    npa_lock_graph();
    placeholder = npa.placeholders;
    if ( NULL != placeholder ) 
    {
      npa.placeholders  = placeholder->next;
      if ( NULL != npa.placeholders )
      {
        npa.placeholders->prev = NULL;
      } 
    }
    npa_unlock_graph();
    npa_destroy_placeholder_link( placeholder );
  } while ( NULL != placeholder );
}


/**
 * <!--npa_trigger_wildcard_callback -->
 *
 */
static void npa_trigger_wildcard_callback( npa_link *resource_link,
                                           npa_callback_link *cb_link )
{
  if ( npa_is_resource_type( resource_link->resource, NPA_INSTANTIATED_RESOURCE ) )
  {
    npa_trigger_callback_event( &cb_link->cb_event,
                                0,
                                (void *)resource_link->name,
                                strlen( resource_link->name ) );
  }
}


/**
 * <!-- npa_link_resource -->
 *
 * @brief Add a npa_resource to the npa resource linked list and move any 
 * placeholder to the npa placeholders list. 
 *
 * If there's already a npa_link with a placeholder in the npa resource list,
 * the passed in npa_resource will be put into that link, and the old 
 * npa_resource will be placed on the npa placeholders list for processing later.
 * If there's not already a placeholder in the npa resource list, the passed in
 * npa_resource will go into a new link and linked into the resource list. 
 *
 * @param name : The resource name
 * @param resource : The npa_resource to add
 */
static void npa_link_resource( const char *name, npa_resource *resource )
{
  npa_link          *placeholder_link, *new_link, *resource_link;

  CORE_VERIFY_PTR( resource );
  CORE_VERIFY_PTR( name );

  /* Make a new link to be used in either the resource or placeholder list */
  new_link = (npa_link *)Core_PoolAlloc( &npa.link_pool );
  CORE_VERIFY_PTR( new_link );

  /* Initialize link */
  new_link->name = name;

  npa_lock_graph();

  placeholder_link = npa_find_resource_link( name );

  CORE_LOG_VERIFY( !npa_is_link_valid( placeholder_link ),
                  ULOG_RT_PRINTF_2( npa.log, 
                                    "\tFAILED resource \"%s\" already exists."
                                    "Defined by node %s", 
                                    name,
                                    placeholder_link->resource->node->name ) );

  if ( NULL == placeholder_link )
  {
    /* Use the new link for the real resource */
    new_link->resource = resource;
    resource_link = new_link;

    /* Insert new_link at head of resource list */
    new_link->next = npa.resources;
    new_link->prev = NULL;

    if ( NULL != npa.resources )
    {
      npa.resources->prev = new_link;
    }
    npa.resources = new_link;

  }
  else
  {
    /* There's already a placeholder linked in the resource list, use
     * that to hold the new resource to avoid modifying the list */
    /* use the new link for the placeholder.
     *
     * The name is the same for both links */
    new_link->resource = placeholder_link->resource;
    
    /* and use the placeholder_link for the new resource */
    placeholder_link->resource = resource;
    resource_link = placeholder_link;

    /* Add new_link at head of placeholder list */
    new_link->next = npa.placeholders;
    new_link->prev = NULL;

    if ( NULL != npa.placeholders )
    {
      npa.placeholders->prev = new_link;
    }
    npa.placeholders = new_link;
  }

  npa_unlock_graph();

  /* Trigger the wildcard callbacks if necessary.
   *
   * Note: These callbacks get evaluated outside of the graph lock.
   */
  if( npa_is_resource_type( resource, NPA_INSTANTIATED_RESOURCE ) )
  {
    /* Trigger every wildcard callback once for this new resource */
    npa_callback_link *cb_link = npa.wildcard_resource_available_callbacks;
    while( NULL != cb_link )
    {
      npa_trigger_wildcard_callback( resource_link, cb_link );
      cb_link = cb_link->next;
    }
  }

  return;
}


/**
 * <!-- npa_new_placeholder_resource -->
 *
 */
static npa_resource *npa_new_placeholder_resource( void )
{
  npa_resource *resource = \
                (npa_resource *)Core_PoolAlloc( &npa.resource_pool );

  CORE_VERIFY_PTR( resource );

  /* Clear the structure out */
  memset( resource, 0, sizeof( npa_resource ) );

  CORE_VERIFY_PTR( 
       resource->node_lock = Core_MutexCreate( CORE_MUTEXATTR_DEFAULT ) );
  resource->event_lock = npa.event_lock;

  return resource;
}


/**
 * <!-- npa_define_resource -->
 *
 * @brief Called by npa_define_node_cb to allocate, intialialize, and link
 * a new npa_resource.
 * 
 * @param node : Pointer to an npa_node_definition to add the npa_resource to.
 * @param definition : An npa_resource_definition to form an npa-resource with.
 * @param init_state : The initial state to set in the resource. 
 */
static void npa_define_resource( npa_node_definition *node,
                                 npa_resource_definition *definition,
                                 npa_resource_state init_state )
{
  npa_resource *resource;
  unsigned int new_resource = 0;
  /* If there's not a placeholder, something by that name
   * (resource/alias/marker) must have been previously defined. Name
   * collisions are bad... */
  ULOG_RT_PRINTF_3( npa.log, "\tdefine resource "
                    "(resource: \"%s\") "
                    "(units: \"%s\")"
                    "(initial state: %d)",
                    definition->name,
                    definition->units,
                    init_state );

  resource = npa_find_raw_resource( definition->name );
  if ( NULL == resource /* Klocworks */
       || npa_is_resource_type( resource, NPA_UNDEFINED_RESOURCE ))
  {
    /* Allocate the new npa_resource and lock. */
    resource = npa_new_placeholder_resource();
    new_resource = 1;
  }
  else
  {
    CORE_LOG_VERIFY(
      npa_is_resource_type( resource, NPA_PLACEHOLDER_RESOURCE ),
      ULOG_RT_PRINTF_1( npa.log,
                        "\tFAILED resource %s already defined",
                        definition->name ) );
  }
 
  /* Make this resource a defined resource, not just a placeholder */
  resource->node = node;

  /* Store the requested initial state in the resource */
  resource->request_state = init_state;

  if ( new_resource )
  {
    npa_link_resource( definition->name, resource );
  }
}


/** @brief default nodata resource internal data structure */
static npa_resource_internal_data npa_internal_data_nodata = {0, {{0,0,0}}};


/**
 * <!-- npa_new_resource -->
 *
 * @brief Allocate a new npa_resource and initialize it.
 *
 * Called by npa_create_node for all the resources defined in the node 
 * definition
 *
 * @param definition : Resource definition struct to form the resource.
 * @param node_definition : Node definition containing this resource
 * @param node_lock : Lock for the node containing this resource.
 *
 * @return Pointer to the new resource 
 */
static npa_resource *npa_new_resource( npa_resource_definition *definition,
  npa_node_definition     *node_definition,
  CoreMutex               *node_lock )
{
  npa_resource               *resource;
  npa_resource_internal_data *internal_ptr;
  unsigned i;

  CORE_VERIFY_PTR( definition );
  CORE_VERIFY_PTR( node_definition );
  CORE_VERIFY_PTR( 
    resource = (npa_resource *)Core_PoolAlloc( &npa.resource_pool ) );

  /* Clear the structure out */
  memset( resource, 0, sizeof( npa_resource ) );

  resource->definition    = definition;
  resource->active_plugin = definition->plugin;
  resource->active_max    = definition->max;
  resource->node          = node_definition;
  resource->node_lock     = node_lock;

  /* create internal data and link it */
  CORE_VERIFY_PTR( 
    internal_ptr 
      = (npa_resource_internal_data *) Core_PoolAlloc( &npa.internal_pool ) );

  /* Initialize to the no_data version memory. It will be updated
   * if the latency values are needed.
   */
  core_memcpy( internal_ptr,sizeof(*internal_ptr),
    &npa_internal_data_nodata, 
    sizeof(npa_internal_data_nodata) );
  resource->_internal = internal_ptr;

  /* TODO: perhaps want to exchange this with a unique lock when an
     event is created against this resource */
  resource->event_lock    = npa.event_lock;
  resource->log_handle    = npa.log;
  
  /* Go through list of update functions to see if they support suppressible 
   * requests and set NPA_RESOURCE_SUPPORTS_SUPPRESSIBLE where appropriate.
   */
  for ( i = 0; i < NPA_ARRAY_SIZE(npa_update_fcn_list); i++ )
  {
    if ( npa_update_fcn_list[i] == resource->active_plugin->update_fcn )
    {
      definition->attributes |= NPA_RESOURCE_SUPPORTS_SUPPRESSIBLE;
      break;
    }
  }

  /* Initialize active_attr from definition->attributes right now to allow the
     below npa_resource_has_attribute() calls to work. Later, update again. */
  resource->_internal->active_attr = definition->attributes;

  /* Create the state vector if necessary */
  if ( npa_resource_has_attribute( resource, NPA_RESOURCE_VECTOR_STATE ) )
  {
    unsigned int alloc_size = definition->max * sizeof( npa_resource_state );
    unsigned int required_offset = 0;
    unsigned int suppressible_offset = 0;

    definition->attributes |= NPA_RESOURCE_DRIVER_UNCONDITIONAL;

    if ( npa_resource_has_attribute( resource,
                                     NPA_RESOURCE_ALLOC_SUPPRESSIBLE_VECTOR ) )
    {
      alloc_size *= 3;
      required_offset = definition->max;
      suppressible_offset = definition->max*2;
    }
    else if ( npa_resource_has_attribute( resource, 
                                          NPA_RESOURCE_SUPPORTS_SUPPRESSIBLE ) )
    {
      alloc_size *= 2;
      required_offset = definition->max;
    }
    resource->state_vector = (npa_resource_state *)Core_Malloc( alloc_size );
    CORE_VERIFY_PTR( resource->state_vector );
    memset( resource->state_vector, 0, alloc_size );
    resource->required_state_vector = resource->state_vector + required_offset;
    resource->suppressible_state_vector = \
              resource->state_vector + suppressible_offset;
  }

  /* Turn off redundant attributes */
  if ( definition->attributes & NPA_RESOURCE_DRIVER_UNCONDITIONAL )
  {
    definition->attributes &= ~NPA_RESOURCE_DRIVER_UNCONDITIONAL_FIRST;
  }

  /* Now that definition->attributes has been updated to our satisfaction,
     change active_attr to this new value */
  resource->_internal->active_attr = definition->attributes;

  /* If they have the legacy create funcion, they can't have a an
   * extended function, and vice-versa */
  CORE_LOG_VERIFY( !( ( NULL != definition->plugin->create_client_fcn ) && 
                      ( NULL != definition->plugin->create_client_ex_fcn ) ),
                   ULOG_RT_PRINTF_1( npa.log, 
                                     "\tFAILED resource %s plugin defined both "
                                     "create_client_fcn and create_client_ex_fcn", 
                                     definition->name ) );
  /* Ideally, I'd like to do this:
     if ( NULL != definition->plugin->create_client_fcn )
     {
       definition->plugin->create_client_ex_fcn = npa_plugin_legacy_create_client_ex;
     }      
     but this can fail if plugin is const. 
  */
  return resource;
}


/**
 * <!-- npa_create_node -->
 *
 */
static void npa_create_node( npa_node_definition *definition,
                             npa_callback_event  *callback )
{
  npa_client               client;
  npa_node_dependency     *dependency;
  npa_resource_definition *resource_def;
  CoreMutex               *node_lock = NULL;
  unsigned int             i;

  CORE_VERIFY_PTR( callback );
  CORE_VERIFY_PTR( definition );
  CORE_VERIFY_PTR( &definition->resources[0] );
  /* Klocworks - Make sure there is at least one dependency count */
  CORE_VERIFY( definition->resource_count );
  
  ULOG_RT_PRINTF_2( npa.log, 
                    "npa_create_node "
                    "(handle: 0x%p) "
                    "(node: \"%s\")",
                    definition,
                    definition->name );

  /* Create resource lock */
  if ( npa_node_has_attribute( definition, NPA_NODE_NO_LOCK ) )
  {
    CORE_VERIFY_PTR( node_lock = Core_MutexCreateEx( CORE_MUTEXTYPE_STUB,
                                                     CORE_MUTEXATTR_DEFAULT ) );
  }
  else
  {
    CORE_VERIFY_PTR( node_lock = Core_MutexCreate( CORE_MUTEXATTR_CVCOMPAT ) );
  }

  /* Create dependencies (if necessary) */
  for ( i = 0, dependency = &definition->dependencies[i];
        i < definition->dependency_count;
        dependency = &definition->dependencies[++i] )
  {
    dependency->handle = NULL;
    if ( NPA_NO_CLIENT == dependency->client_type ) 
    {
      continue;
    }
    
    CORE_VERIFY_PTR ( 
      dependency->handle = npa_create_sync_client( dependency->name, 
                                                   definition->name, 
                                                   dependency->client_type ) );
  }


  /* Create resources */
  for ( i = 0, resource_def = &definition->resources[i]; 
        i < definition->resource_count;
        resource_def = &definition->resources[++i] )
  {
    npa_resource *placeholder = npa_find_defined_resource( resource_def->name );

    CORE_VERIFY_PTR( placeholder );
    if ( placeholder ) //KW fix
    {
        CORE_VERIFY_PTR( resource_def->handle = npa_new_resource( resource_def, 
                                                                  definition, 
                                                                  node_lock ) ); 
        if ( resource_def->handle )
        {
            /* copy the request over from the placeholder */
            resource_def->handle->request_state = placeholder->request_state;

            /* Initialize the index for this resource */
            resource_def->handle->index = i;
        }
    }
  }

  /* Create initialization client */
  memset( &client, 0, sizeof( npa_client ) );
  client.type = NPA_CLIENT_INITIALIZE;

  for ( i = 0, resource_def = &definition->resources[i]; 
        i < definition->resource_count;
        resource_def = &definition->resources[++i] )
  {
    /* Initialize resource */
    npa_resource *resource = resource_def->handle;

    /* Set the current_client so it's available for use and debugging. */
    resource->current_client = &client;

    npa_update_resource_state( resource,
                               resource->node->driver_fcn(
                                 resource, 
                                 &client, 
                                 resource->request_state ) );
    
    /* Clear the current_client. */
    resource->current_client = NULL;

    ULOG_RT_PRINTF_4( npa.log, 
                      "\tcreated resource "
                      "(resource: \"%s\") "
                      "(handle: 0x%p) "
                      "(initial state: %d) "
                      "(active state: %d)",
                      resource_def->name,
                      resource,
                      resource->request_state,
                      resource->active_state );

    /* Clear out request state so it doesn't interfere with client
     * requests */
    resource->request_state = 0;

    /* If the resource has the NPA_RESOURCE_DRIVER_UNCONDITIONAL_FIRST
     * attribute, set NPA_RESOURCE_DRIVER_UNCONDITIONAL in active_attr.
     * This will be reset to the original (definition) attributes at the
     * end of npa_process_request.
     */
    if ( npa_resource_has_attribute( resource, 
                                     NPA_RESOURCE_DRIVER_UNCONDITIONAL_FIRST ) )
    {
      resource->_internal->active_attr |= NPA_RESOURCE_DRIVER_UNCONDITIONAL;
    }
  }

  for ( i = 0, resource_def = &definition->resources[i]; 
        i < definition->resource_count;
        resource_def = &definition->resources[++i] )
  {
    /* Add resource to the list of resources */
    npa_link_resource( resource_def->name, resource_def->handle );
  }
  
  /* Invoke the node define callback */
  if ( NULL != callback->callback )
  {
    npa_trigger_callback_event( callback,
                                0,
                                (void *)definition->name, 
                                strlen( definition->name ) );
  }

  ULOG_RT_PRINTF_2( npa.log, 
                    "\tend npa_create_node "
                    "(handle: 0x%p) "
                    "(node: \"%s\")",
                    definition,
                    definition->name );
}


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
 * @param user_data : A npa_continuation to potentially process
 * @param cb_event_type : Not used
 * @param cb_data : Not used
 * @param size : Not used
 */
void npa_define_node_continuation(
  void         *user_data, 
  unsigned int  cb_event_type,
  void         *cb_data, 
  unsigned int  size )
{
  npa_node_definition *node;
  npa_continuation    *continuation =  (npa_continuation *)user_data;
  CORE_VERIFY_PTR( continuation );
  
  node = continuation->data.node.definition;

  /* Find next unavailable resource */
  while ( ( continuation->data.node.dependency < node->dependency_count ) &&
          ( NULL != npa_find_resource( 
            node->dependencies[continuation->data.node.dependency].name ) ) )
  {
    continuation->data.node.dependency++;
  }

  if ( continuation->data.node.dependency == node->dependency_count )
  {
    npa_resource *resource;

    /* We may be able to create the node - lock the graph */
    npa_lock_graph();

    CORE_VERIFY_PTR( 
      resource = npa_find_defined_resource( node->resources[0].name ) );

    if ( !resource )
    {
          npa_unlock_graph();
          return;
    }
    if ( !npa_is_resource_disabled( resource ) )
    {
      /* All the dependencies exist and the node is not disabled -
       * create it.  Leave the graph locked during creation to avoid
       * races with disable_node */
      npa_create_node( node, &continuation->callback );
      npa_unlock_graph();
      npa_destroy_continuation( continuation );

      return;
    }   
    else
    {
      /* The resource is disabled, so register this continuation on
       * the first resource.  When enable happens, it will get
       * triggered .  This is done while the graph is locked to avoid
       * a race with enable.  Then when enable occurs, it will trigger
       * the construction */
      npa_resource_available_cb( 
        node->resources[0].name, 
        npa_define_node_continuation,
        continuation );
      npa_unlock_graph();

      return;
    }
  }
  else 
  {
    ULOG_RT_PRINTF_3( npa.log, "\tnode waiting on resource "
                      "(handle: 0x%p) "
                      "(node: \"%s\") "
                      "(resource: \"%s\")",
                      node,
                      node->name,
                      node->dependencies[continuation->data.node.dependency].name );


    npa_resource_available_cb ( 
      node->dependencies[continuation->data.node.dependency++].name, 
      npa_define_node_continuation,
      continuation );
  }
}


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
void npa_alias_resource_continuation( 
  void         *user_data, 
  unsigned int  cb_event_type,
  void         *cb_data, 
  unsigned int  size )
{
  npa_continuation *continuation = (npa_continuation *)user_data;
  CORE_VERIFY_PTR( continuation );

  CORE_VERIFY( 0 == strncmp( continuation->data.alias.resource_name, 
                             (const char *)cb_data,
                             NPA_MAX_STR_LEN + 1) );

  npa_link_resource( continuation->data.alias.alias_name, 
                     npa_find_resource( continuation->data.alias.resource_name ) );

  if ( NULL != continuation->callback.callback ) 
  {
    npa_trigger_callback_event( &continuation->callback,
                                0,
                                (void *)continuation->data.alias.alias_name, 
                                strlen( continuation->data.alias.alias_name ) );
  }

  ULOG_RT_PRINTF_2( npa.log, 
                    "\tend npa_alias_resource "
                    "(resource: \"%s\") "
                    "(alias: \"%s\")", 
                    continuation->data.alias.resource_name, 
                    continuation->data.alias.alias_name );

  npa_destroy_continuation( continuation );
}


/**
 * <!-- npa_define_node_cb -->
 *
 * @brief Define a new NPA node with an optional registration of a creation
 * completed callback. 
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
void npa_define_node_cb( npa_node_definition *definition,
                         npa_resource_state   initial_state[],
                         npa_callback         callback,
                         npa_user_data        context )
{
  npa_continuation *continuation;
  unsigned int      count = 0;

  CORE_LOG_VERIFY_PTR( definition,
                      ULOG_RT_PRINTF_0( 
                        npa.log,
                        "\tFAILED npa_define_node definition is NULL" ) );

  CORE_LOG_VERIFY( definition->name && 
                   (strlen(definition->name) <= NPA_MAX_STR_LEN),
                   ULOG_RT_PRINTF_1( 
                     npa.log,
                     "\tFAILED npa_define_node \"%s\" node name is not legal",
                     (definition->name ? definition->name : "NULL") ) );
  
  CORE_LOG_VERIFY( (NULL != definition->resources) && 
                   (definition->resource_count > 0),
                   ULOG_RT_PRINTF_1( 
                     npa.log,
                     "\tFAILED npa_define_node \"%s\" has no resourcees",
                     definition->name ) );

  ULOG_RT_PRINTF_4( npa.log, "npa_define_node "
                    "(handle: 0x%p) "
                    "(node: \"%s\") " 
                    "(resource count: %d) "
                    "(dependency count: %d)", 
                    definition,
                    definition->name,
                    definition->resource_count,
                    definition->dependency_count);

  npa_lock_graph();
  /* Loop through the resources adding npa_resources and npa_links to the graph */
  for ( count = 0; count < definition->resource_count; count++ )
  {
    npa_define_resource( definition,
                         &definition->resources[count],
                         (initial_state) ? initial_state[count] : 0 );
  }
  npa_unlock_graph();

  /* Loop through the dependencies to print a log message for each. */
  for ( count = 0; count < definition->dependency_count; count++ )
  {
    ULOG_RT_PRINTF_3( npa.log, "\tnode dependency "
                      "(handle: 0x%p) "
                      "(node: \"%s\") "
                      "(dependency: \"%s\")",
                      definition,
                      definition->name,
                      definition->dependencies[count].name );
  }

  /* A continuation will finish node creation and call the passed in
     callback function when the node is created.  */
  continuation = npa_new_continuation( callback, context );
  continuation->data.node.definition = definition;
  continuation->data.node.dependency = 0;
  
  /* Trigger the continuation directly for the first time */
  npa_define_node_continuation( continuation, 0, NULL, 0 );

  /* Invoke any resource available callbacks */
  npa_cleanup_placeholders();

  return;
}


/**
 * <!-- npa_alias_resource_cb -->
 *
 * @brief Create an alias for an existing resource with an optional
 * registration of a creation completed callback. 
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
 */
void npa_alias_resource_cb( const char    *resource_name, 
                            const char    *alias_name,
                            npa_callback   callback,
                            npa_user_data  context )
{
  npa_continuation *continuation;
  npa_resource *resource;

  CORE_LOG_VERIFY_PTR( resource_name,
                      ULOG_RT_PRINTF_0( 
                        npa.log,
                        "\tFAILED npa_alias_resource resource_name is NULL" ) );

  CORE_LOG_VERIFY( alias_name &&
                       strlen(alias_name) <= NPA_MAX_STR_LEN,
                       ULOG_RT_PRINTF_1( 
                        npa.log,
                        "\tFAILED npa_alias_resource alias name \"%s\" is not legal",
                        (alias_name ? alias_name : "NULL") ) );

  ULOG_RT_PRINTF_2( npa.log, 
                    "npa_alias_resource "
                    "(resource: \"%s\") (alias: \"%s\")", 
                    resource_name, alias_name );

  resource = npa_find_raw_resource( alias_name );

  if ( NULL == resource /* Klocworks */
       || npa_is_resource_type( resource, NPA_UNDEFINED_RESOURCE ))
  {
    resource = npa_new_placeholder_resource();
    resource->node = npa_aliased_node;
    npa_link_resource( alias_name, resource );
  }
  else
  {
    CORE_LOG_VERIFY(
      npa_is_resource_type( resource, NPA_PLACEHOLDER_RESOURCE ),
      ULOG_RT_PRINTF_1( npa.log,
                        "\tFAILED resource %s already defined",
                        alias_name ) );
    resource->node = npa_aliased_node;
  }
 
  continuation = npa_new_continuation( callback, context );
  continuation->data.alias.resource_name = resource_name;
  continuation->data.alias.alias_name    = alias_name;

  npa_resource_available_cb ( resource_name,
                              npa_alias_resource_continuation,
                              continuation );

  /* Invoke any resource available callbacks */
  npa_cleanup_placeholders();
}


/**
 * <!-- npa_enable_new_node -->
 *
 */
static NPA_INLINE void npa_enable_new_node( npa_node_definition *user_node, 
                                            npa_resource_state default_state[] )
{
  npa_node_definition *graph_node;
  /* Enabling node for the first time - copy and construct it */
  ULOG_RT_PRINTF_1( npa.log, 
                    "npa_enable_node - initial construction (node: \"%s\")",
                    user_node->name );
  CORE_LOG_VERIFY( 
    !npa_node_has_attribute( user_node, NPA_NODE_NO_LOCK ),
    ULOG_RT_PRINTF_1( npa.log, 
                      "\tFAILED node \"%s\" has NO_LOCK attribute",
                      user_node->name ) ); 
  
  /* Copy the node and add the DSIABLEABLE attribute */
  graph_node = npa_copy_node( user_node );
  graph_node->attributes |= NPA_NODE_DISABLEABLE;
  
  npa_define_node_cb( graph_node, default_state, 
                      NULL, NULL );
}


/**
 * <!-- npa_enable_instantiated_node -->
 *
 */
static NPA_INLINE void npa_enable_instantiated_node( npa_node_definition *user_node, 
                                                     npa_resource        *resource,
                                                     npa_resource_state default_state[] )
{
  unsigned int i;
  npa_client client;
  npa_node_definition *graph_node;
 
  ULOG_RT_PRINTF_1( npa.log, 
                    "npa_enable_intantiated_node (node: \"%s\")",
                    user_node->name );
      
  memset( &client, 0, sizeof( npa_client ) );
  client.type = NPA_CLIENT_INITIALIZE;

  graph_node = resource->node;
  CORE_VERIFY( npa_node_equal( user_node, graph_node, 
                               (npa_node_attribute)~NPA_NODE_DISABLEABLE ) );

  if ( npa_is_resource_disabled( resource ) )
  {
    graph_node->driver_fcn = user_node->driver_fcn;
    graph_node->data       = user_node->data;
        
    for ( i = 0; i < graph_node->resource_count; i++ )
    {
      npa_resource *resource = graph_node->resources[i].handle;
      npa_resource_state enable_state = resource->request_state;
          
      resource->sequence &= ~NPA_RESOURCE_DISABLED;
          
      graph_node->resources[i].data = user_node->resources[i].data;
          
      graph_node->resources[i].query_fcn = user_node->resources[i].query_fcn;
      graph_node->resources[i].plugin = user_node->resources[i].plugin;
          
      if ( NULL == resource->clients )
      { 
        enable_state = (NULL == default_state) ? 0 : default_state[i];
      }
          
      /* Set the current_client so it's available for use and debugging. */
      resource->current_client = &client;

      npa_update_resource_state( resource,
                                 resource->node->driver_fcn(
                                   resource, 
                                   &client, 
                                   enable_state ) );

      /* Clear the current_client. */
      resource->current_client = NULL;    

      ULOG_RT_PRINTF_1( npa.log, 
                        "\tenable resource (resource: \"%s\")",
                        graph_node->resources[i].name );
    }
    Core_MutexUnlock( resource->node_lock );
  }
  else 
  {
    ULOG_RT_PRINTF_0( npa.log, 
                      "\tWARNING - enabling node that is not disabled" );
  }
}


/**
 * <!-- npa_enable_defined_node -->
 *
 */
static NPA_INLINE
void npa_enable_defined_node( npa_node_definition *user_node, 
                              npa_resource        *resource,
                              npa_resource_state   default_state[])
{
  npa_event *event;
  npa_node_definition      *node = NULL;
  npa_continuation *continuation = NULL;
  npa_event      *creation_event = NULL;

  CORE_VERIFY( npa_is_resource_type( resource, NPA_DEFINED_RESOURCE ) );

  /* This is going to manipulate the event structure - get that lock
   * first
   *
   * Note that this is invoked with the graph_lock held */
  Core_MutexLock( resource->event_lock );
  event = resource->events.event;
  /* If there is the creation callback for this resource has been
   * registered with this resource, trigger it */
  while( NULL != event )
  {
    CORE_VERIFY( NPA_TRIGGER_RESOURCE_AVAILABLE == event->trigger_type );
    CORE_VERIFY( npa_define_node_continuation == event->callback.callback );
    continuation = (npa_continuation *)event->callback.context;
    node = continuation->data.node.definition;

    if ( node == resource->node )
    {
      creation_event = event; 
      break;
    }
    event = event->next;
  }

  if ( NULL != creation_event )
  {
    CORE_VERIFY_PTR( node );
    CORE_VERIFY_PTR( continuation );

    CORE_VERIFY( npa_node_equal( node, user_node, 
                                 (npa_node_attribute)~NPA_NODE_DISABLEABLE ) );

    if ( NULL != creation_event->prev )
    {
      creation_event->prev->next = creation_event->next;
    }
    if ( NULL != creation_event->next )
    {
      creation_event->next->prev = creation_event->prev;
    }
    if ( resource->events.event == creation_event )
    {
      resource->events.event = creation_event->next;
    }
    Core_MutexUnlock( resource->event_lock );
    Core_PoolFree( &npa.event_pool, event );

    npa_create_node( node, &continuation->callback );
    npa_destroy_continuation( continuation );

    /* Invoke any resource available callbacks */
    npa_cleanup_placeholders();
  } 
  else
  {
    Core_MutexUnlock( resource->event_lock );
  }
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
void npa_enable_node( npa_node_definition *user_node, 
                      npa_resource_state   default_state[] )
{
  npa_resource *resource;

  CORE_VERIFY_PTR( user_node );

  /* This is modifying the state of the graph, so make sure the graph
   * doesn't change state while it's happening... */
  npa_lock_graph();
  resource = npa_find_raw_resource( user_node->resources[0].name );
  if ( NULL == resource )
  {
    /* This is the first time this node's been seen - treat like a
     * "normal" new node */
    npa_unlock_graph();
    npa_enable_new_node( user_node, default_state );
  }
  else
  {
    if ( npa_is_resource_type( resource, NPA_INSTANTIATED_RESOURCE ) )
    {
      /* The resource has already been instantiated, so we can
       * re-enable the existing node*/
      npa_unlock_graph();
      npa_enable_instantiated_node( user_node, resource, default_state );
    }
    else
    {
      /* The node is going from disabled to enabled without having
       * been instantiated, keep the graph locked while re-enabling to
       * avoid a race with the resource being instantiated */
      npa_enable_defined_node( user_node, resource, default_state );
      npa_unlock_graph();
    }
  }

  ULOG_RT_PRINTF_1( npa.log, 
                    "\tend npa_enable_node (node: \"%s\")",
                    user_node->name );
}

/**
 * <!-- npa_disabled_query_fcn -->
 * 
 * @brief Pass through query function for disabled resources.  
 *
 * This can be used as a flag to determine if the resource is disabled. 
 */
static npa_query_status npa_disabled_query_fcn( npa_resource    *resource, 
                                         unsigned int     id, 
                                         npa_query_type  *result )
{
  return NPA_QUERY_UNSUPPORTED_QUERY_ID;
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
void npa_disable_node( npa_node_definition *user_node )
{
  unsigned int i;
  unsigned int node_instantiated = 0;
  npa_resource *resource;
  npa_node_definition *graph_node = NULL;
  
  CORE_VERIFY_PTR( user_node );

  if ( user_node ) //KW
  {
      /* make sure the graph doesn't change state */
      npa_lock_graph();

      CORE_VERIFY_PTR( 
        resource = npa_find_raw_resource( user_node->resources[0].name ) );

      if ( resource ) //KW
      {
          if ( npa_is_resource_type( resource, NPA_INSTANTIATED_RESOURCE ) )
          {
            /* The node is instantiated, so don't have to worry about the
             * graph changing state anymore */
            npa_unlock_graph();

            node_instantiated = 1;
            /* If it's instantiated, lock the resource so it can't be
             * accessed.  The associated unlock will occur in
             * npa_enable_node() */
            Core_MutexLock( resource->node_lock );
          }
          else 
          {
            node_instantiated = 0;
          }

          CORE_VERIFY_PTR( graph_node = resource->node );

          if ( graph_node )
          {
              ULOG_RT_PRINTF_1( npa.log, 
                                "npa_disable_node (node: \"%s\")",
                                graph_node->name );

              CORE_LOG_VERIFY( npa_node_has_attribute( graph_node, NPA_NODE_DISABLEABLE ),
                               ULOG_RT_PRINTF_1( npa.log, 
                                                 "\tFAILED npa_disable_node \"%s\""
                                                 "does not have NPA_NODE_DISABLEABLE "
                                                 "attribute defined",
                                                 graph_node->name ) );
    
              CORE_VERIFY( npa_node_equal( user_node, graph_node, 
                                           (npa_node_attribute)~NPA_NODE_DISABLEABLE ) );
              CORE_VERIFY( !npa_node_has_attribute( graph_node, NPA_NODE_NO_LOCK ) );
  
              for ( i = 0; i < graph_node->resource_count; i++ )
              {
                if ( node_instantiated )
                {
                  resource = graph_node->resources[i].handle;
                  CORE_LOG_VERIFY( NULL == resource->definition->query_fcn,
                                   ULOG_RT_PRINTF_1(
                                     npa.log,
                                     "\tFAILED: can't disable resource \"%s\" "
                                     "it has non-NULL query function",
                                     resource->definition->name ) );
                  resource->definition->query_fcn = npa_disabled_query_fcn;
                }
                else
                {
                  resource = npa_find_defined_resource( graph_node->resources[i].name );
                }
                CORE_VERIFY_PTR( resource );
                if ( resource )
                {
                    resource->sequence |= NPA_RESOURCE_DISABLED;

                    ULOG_RT_PRINTF_1( npa.log, 
                                      "\tdisable resource (resource: \"%s\")",
                                      graph_node->resources[i].name );
                }
              }
              if ( !node_instantiated )
              {
                /* The node was not intantiated, and all the relevant resources
                 * are disaled, so unlock the graph.
                 *
                 * Technically, this unlock could be done after the first resource
                 * was flaged as disabled, as it's only the first resource that's
                 * checked in npa_define_node_continuation(), but it's logically
                 * cleaner to wait until all the resources are flagged.*/
                npa_unlock_graph();
              }

              ULOG_RT_PRINTF_1( npa.log, 
                                    "\tend npa_disable_node (node: \"%s\")",
                                    graph_node->name );
          } //KW graph_node
      } // KW resource
  } // KW user_node
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
  CORE_LOG_VERIFY( marker_name &&
                   strlen(marker_name) <= NPA_MAX_STR_LEN,
                   ULOG_RT_PRINTF_1( 
                     npa.log,
                     "\tFAILED npa_define_marker \"%s\" name is not legal",
                     (marker_name ? marker_name : "NULL") ) );

  ULOG_RT_PRINTF_1( npa.log, "npa_define_marker (marker: \"%s\") ",
                      marker_name);

  npa_link_resource( marker_name, &marker_resource );

  /* Invoke any resource available callbacks */
  npa_cleanup_placeholders();
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
  CORE_LOG_VERIFY( marker_name &&
                   strlen(marker_name) <= NPA_MAX_STR_LEN,
                   ULOG_RT_PRINTF_1( 
                     npa.log,
                     "\tFAILED npa_define_marker_with_attributes \"%s\" name is not legal",
                     (marker_name ? marker_name : "NULL") ) );

  /* Only NPA_RESOURCE_INTERPROCESS_ACCESS_ALLOWED is supported for now. */
  CORE_LOG_VERIFY( NPA_RESOURCE_INTERPROCESS_ACCESS_ALLOWED == attributes,
                   ULOG_RT_PRINTF_0( 
                     npa.log,
                     "\tFAILED resource attributes are not allowed for NPA markers" ) );

  ULOG_RT_PRINTF_1( npa.log, "npa_define_marker_with_attributes (marker: \"%s\") ",
                    marker_name);

  npa_link_resource( marker_name, &global_marker_resource );

  /* Invoke any resource available callbacks */
  npa_cleanup_placeholders();
}


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
void npa_resources_available_continuation(   
  void         *user_data, 
  unsigned int  cb_event_type,
  void         *cb_data, 
  unsigned int  size )
{
  npa_continuation *continuation = (npa_continuation *)user_data;
  CORE_VERIFY_PTR( continuation );

  while ( ( continuation->data.resources.index < continuation->data.resources.num_resources ) &&
          ( NULL != npa_find_resource( 
            continuation->data.resources.resources[continuation->data.resources.index] ) ) )
  {
    continuation->data.resources.index++;
  }

  if ( continuation->data.resources.index == continuation->data.resources.num_resources )
  {
    ULOG_RT_PRINTF_2( npa.log, "  npa_resources_available complete "
                      "(callback: 0x%p) "
                      "(context: 0x%p) ",
                      continuation->callback.callback,
                      continuation->callback.context );

    npa_trigger_callback_event( &continuation->callback, 
                                cb_event_type, 
                                (void *)continuation->data.resources.resources, 
                                continuation->data.resources.num_resources );
    npa_destroy_continuation( continuation );
  }
  else 
  {
    const char *resource =  
      continuation->data.resources.resources[continuation->data.resources.index++];

    ULOG_RT_PRINTF_3( npa.log, "  npa_resources_available waiting on resource "
                      "(resource: \"%s\") "
                      "(callback: 0x%p) "
                      "(context: 0x%p) ",
                      resource,
                      continuation->callback.callback,
                      continuation->callback.context );


    npa_resource_available_cb( resource, 
                               npa_resources_available_continuation,
                               continuation );
  }
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
 * @param num_resources: Number of resources that are being queried
 * @param resources: Array of resources that are being queried 
 * @param callback: Callback funtion to invoke when resource is available
 * @param context: User registered pointer to be supplied to callback
 * when it is invoked
 */
void npa_resources_available_cb( unsigned int    num_resources,
                                 const char     *resources[],
                                 npa_callback    callback,
                                 void           *context )
{
  npa_continuation *continuation;
 
  CORE_VERIFY( ( 0 == num_resources ) || ( NULL != resources ) );
  CORE_VERIFY_PTR( callback );
  continuation = npa_new_continuation( callback, context );

  /* Initialize this continuation's state */
  continuation->data.resources.num_resources = num_resources;
  continuation->data.resources.resources     = resources;
  continuation->data.resources.index         = 0;

  /* Trigger the continuation directly for the first time */
  npa_resources_available_continuation( continuation, 0, NULL, 0 );
  return;
}


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
 * @param resource_name: Name of the resource that's being queried
 * @param callback: Callback funtion to invoke when resource is available
 * @param context: User registered pointer to be supplied to callback when it
 * is invoked
 */
void npa_resource_available_cb( const char     *resource_name,
                                npa_callback    callback,
                                npa_user_data   context )
{
  npa_resource *resource;
  CORE_VERIFY_PTR( resource_name );
  CORE_VERIFY_PTR( callback );

  /* Avoid race condition on new resource being added */
  npa_lock_graph();
  
  resource = npa_find_raw_resource( resource_name );

  /* Resource already exists - just call the callback directly
   *
   * Note - this doesn't check for the resource being disabled -
   * should it? Probably, but that opens up another can of worms...*/
  if ( npa_is_resource_type( resource, NPA_INSTANTIATED_RESOURCE ) )
  {
    npa_callback_event cb_event;
    
    /* Don't need to keep graph locked */
    npa_unlock_graph();

    cb_event.callback = callback;
    cb_event.context = context;

    /* Trigger the event outside the lock */
    npa_trigger_callback_event( &cb_event,      
                                0, 
                                (void *)resource_name, 
                                strlen( resource_name ) );
  }
  else 
  {
    /* If the resource  */
    if ( npa_is_resource_type( resource, NPA_UNDEFINED_RESOURCE ) )
    {
      npa_link_resource( resource_name, npa_new_placeholder_resource() );
    }
    CORE_VERIFY_PTR( npa_new_event( resource_name,
                                    "Resource Available Callback",
                                    NPA_TRIGGER_RESOURCE_AVAILABLE,
                                    callback, context ) );
    npa_unlock_graph();
  }
  return;
}


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
                                    npa_user_data   context )
{
  npa_callback_link *cb_link;
  npa_link          *resource_link;
  CORE_VERIFY_PTR( callback );

  /* create the callback link */
  CORE_VERIFY_PTR( 
       cb_link = (npa_callback_link *)Core_PoolAlloc( &npa.callback_pool ) );
  cb_link->cb_event.callback = callback;
  cb_link->cb_event.context = context;

  /* Insert the callback into wildcard list */
  npa_lock_graph();

  cb_link->next = npa.wildcard_resource_available_callbacks;
  cb_link->prev = NULL;

  if( NULL != npa.wildcard_resource_available_callbacks )
  {
    npa.wildcard_resource_available_callbacks->prev = cb_link;
  }
  npa.wildcard_resource_available_callbacks = cb_link;
  npa_unlock_graph();

  /* Trigger this new callback once for every existing resource/alias.
   *
   * Note: This is outside the lock, so the callback can be
   * re-entered, and invoked multiply for the same resource.  It's the
   * callback's job to handle this possibility*/
  resource_link = npa_get_resources();
  while ( NULL != resource_link )
  {
    npa_trigger_wildcard_callback( resource_link, cb_link );
    resource_link = resource_link->next;
  }
  return;
}


/**
 * <!-- npa_change_resource_plugin -->
 *
 * @brief Change the plugin for the given resource.
 *
 * The new plugin must support all the client work models supported by
 * the plugin that was in the resource definition.  If the new plugin
 * is NULL, the original plugin will be restored.
 *
 * If they exist, the client destroy function of the plugin being
 * replaced and the client create function of the new plugin will be
 * invoked for all clients of the resource.
 *
 * It is a fatal error to attempt to change the plugin for a resource
 * that does not exist.
 *
 * @param resource_name : name of the resource whose plugin is to be changed
 * @param plugin : new plugin to use, NULL to restore original plugin
 */
void npa_change_resource_plugin( const char                *resource_name,
                                 const npa_resource_plugin *plugin )
{
  npa_client   *client;
  npa_resource *resource = npa_find_resource( resource_name );
  unsigned int i;

  CORE_VERIFY_PTR( resource );
  if ( resource ) //KW
  {
      /* If the plugin is NULL, restore the definition */
      if ( NULL == plugin )
      {
        plugin = resource->definition->plugin;
      }

      /* Make sure all the original supported clients continue to be supported */
      CORE_VERIFY( 
        ( resource->definition->plugin->supported_clients & 
          plugin->supported_clients ) == 
        resource->definition->plugin->supported_clients );

      client = resource->clients;

      Core_MutexLock( resource->node_lock );
      while ( NULL != client )
      {
        /* plugin is being replaced - call the old plugin's client destroy
         * function */
        if ( NULL != resource->active_plugin->destroy_client_fcn )
        {
          resource->active_plugin->destroy_client_fcn( client );
        }
        /* plugin is being replaced - call the new plugins's client create
         * function */
        if ( NULL != plugin->create_client_fcn )
        {
          plugin->create_client_fcn( client );
          CORE_VERIFY( client->resource == resource );
        }

        client = client->next;
      }
  
      /* Clear the SUPPORTS_SUPPRESSIBLE bit, then set again if the new plugin
       * supports suppressible requests */
      resource->definition->attributes &= ~NPA_RESOURCE_SUPPORTS_SUPPRESSIBLE;
      for ( i = 0; i < NPA_ARRAY_SIZE(npa_update_fcn_list); i++ )
      {
        if ( npa_update_fcn_list[i] == plugin->update_fcn )
        {
          resource->definition->attributes |= NPA_RESOURCE_SUPPORTS_SUPPRESSIBLE;
          break;
        }
      }
      resource->active_plugin = plugin;
      Core_MutexUnlock( resource->node_lock );
 }//if ( resource ) KW
}

/*
  Reset
*/

/**
 * <!-- npa_reset -->
 *
 * @brief FOR TEST PURPOSES ONLY. Do not call this function unless you're 
 * absolutely sure what you're doing. And even then, you probably shouldn't. 
 */
void npa_reset( void )
{
  npa_link *link;
  npa_link *next_link;
  npa_link  sentinel;

  npa_lock_graph();
  link = npa.resources; 

  sentinel.next = link;
  sentinel.prev = NULL;

  if ( link != NULL ) 
  { 
    CORE_VERIFY( NULL == link->prev );
    link->prev = &sentinel;
  };
  
  /* First, remove all the aliases/placeholders */
  while ( NULL != link )
  {
    next_link = link->next;
    if ( npa_is_resource_type( link->resource, NPA_INSTANTIATED_RESOURCE ) ||
         npa_is_link_alias( link ) )
    {
      link->prev->next = next_link;
      if ( NULL != next_link )
      {
        next_link->prev = link->prev;
      }
      
      Core_PoolFree( &npa.link_pool, link );
    }
    link = next_link;
  }

  /* Now remove all the remaining links */
  link = sentinel.next;
  while ( NULL != link )
  {
    next_link = link->next;

    link->prev->next = next_link;
    if ( NULL != next_link )
    {
      next_link->prev = link->prev;
    }

    npa_destroy_resource( link->resource );
    Core_PoolFree( &npa.link_pool, link );
    
    link = next_link;
  }
  npa.resources = sentinel.next;
  CORE_VERIFY( NULL == npa.resources );

  npa_unlock_graph();
}

#ifdef __cplusplus
}
#endif
