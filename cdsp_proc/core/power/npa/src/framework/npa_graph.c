/*============================================================================
@file npa_graph.c

Routines that define, create and modify the NPA resource graph

Copyright (c) 2009-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR

$Header: //components/rel/core.qdsp6/2.1.c4/power/npa/src/framework/npa_graph.c#1 $
============================================================================*/

#include "npa_internal.h"
#include "npa_inlines.h"
#include "npa_log.h"
#include "CoreString.h"

#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Define structures for marker resources */
static npa_resource_definition marker_resource_definition = 
{
  "marker resource",
  "none",
  0,
  &npa_no_client_plugin,
  NPA_RESOURCE_DEFAULT,
  /* Rest is defined as 0/NULL */
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
  &marker_node_definition,
  0,
  NULL,
  {NULL}, 
  &npa_no_client_plugin
  /* Rest is defined as 0/NULL */
};


/* Structures for global markers that work across address spaces */
static npa_resource_definition global_marker_resource_definition = 
{
  "global marker resource",
  "none",
  0,
  &npa_no_client_plugin,
  NPA_RESOURCE_INTERPROCESS_ACCESS_ALLOWED,
  /* Rest is defined as 0/NULL */
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
  &global_marker_node_definition,
  0,
  NULL,
  {NULL}, 
  &npa_no_client_plugin
  /* Rest is defined as 0/NULL */
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
  null_update_fcn,              
  0xffffffff                    /* All clients supported */
};


static npa_resource_definition null_resource[] =
{
  {
    "/dev/null",                /* Name */
    "n/a",                      /* Units */
    NPA_MAX_STATE,              /* Max value */
    &null_plugin                /* Plugin */
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
 * @brief Check if the node the user passed in has the same identity as the
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
  if ( resource_name )
  {
    npa_link *link = npa_get_resources();
    while ( link )
    {
      if ( 0 == strncmp( resource_name, link->name, NPA_MAX_STR_LEN + 1 ) )
      {
        return link;
      }
      link = link->next;
    }
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

  if ( NULL == resource ) { return; }
  
  /* Perhaps I should lock the resource here, but if you're calling into this,
     you take your chances... */
  for ( client = resource->clients; client != NULL; client = client->next )
  {
    npa_destroy_client( client );
  }

  if ( npa_is_resource_type( resource, NPA_PLACEHOLDER_RESOURCE ) )
  {
    event = resource->events.creation_events;
  }
  else
  {
    event = 
      resource->events.list ? resource->events.list->reg_events : NULL;
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

  event = resource->events.creation_events;
  
  while ( event )
  {
    npa_event *dead_event;

    CORE_VERIFY( NPA_TRIGGER_RESOURCE_AVAILABLE == event->trigger_type );

    npa_dispatch_event_callback( &event->callback, 0, 
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
    if ( placeholder ) 
    {
      npa.placeholders  = placeholder->next;
      if ( npa.placeholders )
      {
        npa.placeholders->prev = NULL;
      } 
    }
    npa_unlock_graph();
    npa_destroy_placeholder_link( placeholder );
  } while ( placeholder );
}


/**
 * <!--npa_trigger_wildcard_callback -->
 */
static void npa_trigger_wildcard_callback( void *arg1, void *arg2 )
{
  npa_event_callback *callback = (npa_event_callback *)arg1;
  npa_link *link = (npa_link *)arg2;

  callback->callback( callback->context, 0, 
                      (void *)link->name, strlen( link->name ) );
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
  npa_link *placeholder_link, *new_link, *resource_link;

  CORE_VERIFY_PTR( resource );
  CORE_VERIFY_PTR( name );

  /* Make a new link to be used in either the resource or placeholder list */
  new_link = (npa_link *)Core_PoolAlloc( &npa.link_pool );
  CORE_VERIFY_PTR( new_link );

  /* Initialize link */
  new_link->name = name;

  npa_lock_graph();

  placeholder_link = npa_find_resource_link( name );

  if ( npa_is_link_valid( placeholder_link ) )
  {
    ULOG_RT_PRINTF_2( npa.log,
                      "\tFAILED resource \"%s\" already exists."
                      "Defined by node %s", 
                      name, placeholder_link->resource->node->name );
    CORE_VERIFY( 0 );
  }

  if ( NULL == placeholder_link )
  {
    /* Use the new link for the real resource */
    new_link->resource = resource;
    resource_link = new_link;

    /* Insert new_link at head of resource list */
    new_link->next = npa.resources;
    new_link->prev = NULL;

    if ( npa.resources )
    {
      npa.resources->prev = new_link;
    }
    npa.resources = new_link;

  }
  else
  {
    /* There's already a placeholder with this name in the resource list;
       use that to hold the new resource instead of modifying the list */
    new_link->resource = placeholder_link->resource;
    
    /* and use the placeholder_link for the new resource */
    placeholder_link->resource = resource;
    resource_link = placeholder_link;

    /* Add new_link at head of placeholder list */
    new_link->next = npa.placeholders;
    new_link->prev = NULL;

    if ( npa.placeholders )
    {
      npa.placeholders->prev = new_link;
    }
    npa.placeholders = new_link;
  }

  npa_unlock_graph();

  /* Trigger every registered wildcard callback once for this new resource */
  if ( npa_is_resource_type( resource, NPA_INSTANTIATED_RESOURCE ) )
  {
    npa_list_Apply2( &npa.wildcard_resource_available_callbacks,
                     npa_trigger_wildcard_callback, resource_link );
  }
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

  ULOG_RT_PRINTF_3( npa.log, "\tdefine resource (name: \"%s\") "
                    "(units: \"%s\") (initial state: %d)",
                    definition->name, definition->units, init_state );

  resource = npa_find_raw_resource( definition->name );
  if ( NULL == resource /* Klocworks */
       || npa_is_resource_type( resource, NPA_UNDEFINED_RESOURCE ) )
  {
    /* Allocate the new npa_resource and lock. */
    resource = npa_new_placeholder_resource();
    new_resource = 1;
  }
  else
  {
    if ( !npa_is_resource_type( resource, NPA_PLACEHOLDER_RESOURCE ) )
    {
      ULOG_RT_PRINTF_1( npa.log, "\tFAILED resource %s already defined",
                        definition->name );
      CORE_VERIFY( 0 );
    }
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

/**
 * <!-- npa_resource_has_dependency_of_type -->
 *
 * @brief Returns TRUE if the resource has a dependency of a type included
 * in the bitwise OR of client types in "type".
 *
 * @param resource : The resource being checked
 * @param type : A bitwise OR of types of interest. The routine returns
 * TRUE if the resource has a dependency of any one of these types.
 *
 * @return TRUE if the resource has such a dependency, FALSE otherwise.
 */
unsigned int npa_resource_has_dependency_of_type( npa_resource *resource, 
                                                  unsigned int  types )
{
  npa_node_definition *node;
  unsigned int i;

  CORE_VERIFY_PTR( resource );

  node = resource->node;

  for ( i = 0; i < node->dependency_count; ++i )
  {
    if ( node->dependencies[i].client_type & types )
    {
      return TRUE;
    }
  }

  return FALSE;
}

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
  npa_resource *resource;
  unsigned int supported_clients;

  CORE_VERIFY_PTR( definition );
  CORE_VERIFY_PTR( node_definition );
  CORE_VERIFY_PTR( 
    resource = (npa_resource *)Core_PoolAlloc( &npa.resource_pool ) );

  memset( resource, 0, sizeof( npa_resource ) );

  resource->definition    = definition;
  resource->active_plugin = definition->plugin;
  resource->active_max    = definition->max;
  resource->node          = node_definition;
  resource->node_lock     = node_lock;

  /* create internal data */
  resource->_internal = 
    (npa_resource_internal_data *)Core_PoolAlloc( &npa.internal_pool );
  
  CORE_VERIFY_PTR( resource->_internal );
  memset( resource->_internal, 0, sizeof(npa_resource_internal_data) );

  /* Set to the global lock; override after the resource is available and
     the first non-resource-available event is registered against it */
  resource->event_lock = npa.event_lock;

  resource->log = npa.log;

  supported_clients = resource->active_plugin->supported_clients;

  /* If resource supports clients of type NPA_CLIENT_SUPPRESSIBLE(_VECTOR),
     set the NPA_RESOURCE_SUPPORTS_SUPPRESSIBLE attribute */
  if ( supported_clients &
       ( NPA_CLIENT_SUPPRESSIBLE | NPA_CLIENT_SUPPRESSIBLE_VECTOR ) )
  {
    _npa_set_resource_attribute( resource, 
                                 NPA_RESOURCE_SUPPORTS_SUPPRESSIBLE );
  }

  if ( supported_clients &
       ( NPA_CLIENT_SUPPRESSIBLE2 | NPA_CLIENT_SUPPRESSIBLE2_VECTOR ) )
  {
    resource->_internal->attributes |= NPA_RESOURCE_SUPPORTS_SUPPRESSIBLE2;
  }

  /* Tag resource if it has a suppressible dependency */
  if ( npa_resource_has_dependency_of_type( resource,
         NPA_CLIENT_SUPPRESSIBLE | NPA_CLIENT_SUPPRESSIBLE_VECTOR ) )
  {
    resource->_internal->attributes |= 
      NPA_RESOURCE_HAS_SUPPRESSIBLE_DEPENDENCY;
  }

  /* Tag resource if it has a suppressible2 dependency */
  if ( npa_resource_has_dependency_of_type( resource,
         NPA_CLIENT_SUPPRESSIBLE2 | NPA_CLIENT_SUPPRESSIBLE2_VECTOR ) )
  {
    resource->_internal->attributes |= 
      NPA_RESOURCE_HAS_SUPPRESSIBLE2_DEPENDENCY;
  }
  
  /* If a vector resource, create state vectors */
  if ( _npa_resource_has_attribute( resource, NPA_RESOURCE_VECTOR_STATE ) )
  {
    unsigned int vector_size = definition->max * sizeof( npa_resource_state );

    /* We do things a little differently if the resource is using the 
       identity update function */
    if ( resource->active_plugin->update_fcn == npa_identity_update_fcn )
    {
      if ( supported_clients & NPA_CLIENT_VECTOR )
      {
        CORE_VERIFY_PTR( resource->required_state_vector =
                           (npa_resource_state *)Core_Malloc( vector_size ) );
        memset( resource->required_state_vector, 0, vector_size );
      }

      if ( supported_clients & NPA_CLIENT_SUPPRESSIBLE_VECTOR )
      {
        CORE_VERIFY_PTR( resource->suppressible_state_vector =
                            (npa_resource_state *)Core_Malloc( vector_size ) );
        memset( resource->suppressible_state_vector, 0, vector_size );
      }

      if ( supported_clients & NPA_CLIENT_SUPPRESSIBLE2_VECTOR )
      {
        CORE_VERIFY_PTR( resource->suppressible2_state_vector =
                            (npa_resource_state *)Core_Malloc( vector_size ) );
        memset( resource->suppressible2_state_vector, 0, vector_size );
      }
    }
    else
    {
      CORE_VERIFY_PTR( resource->state_vector =
                       (npa_resource_state *)Core_Malloc( vector_size ) );
      memset( resource->state_vector, 0, vector_size );

      /* Initialize the required and semi_active state vectors to state_vector.
         Override conditionally further below */
      resource->required_state_vector = \
        resource->semiactive_state_vector = resource->state_vector;

      if ( _npa_resource_has_attribute( resource, 
                                        NPA_RESOURCE_SUPPORTS_SUPPRESSIBLE ) )
      {
        CORE_VERIFY_PTR( resource->required_state_vector = 
                         (npa_resource_state *)Core_Malloc( vector_size ) );
        memset( resource->required_state_vector, 0, vector_size );

        /* If resource has a SUPPRESSIBLE dependency on another resource, we
           allocate a vector for it to aggregate suppressible requests into */
        if ( npa_resource_has_dependency_of_type( 
                              resource, NPA_CLIENT_SUPPRESSIBLE_VECTOR ) )
        {
          CORE_VERIFY_PTR( resource->suppressible_state_vector =
                           (npa_resource_state *)Core_Malloc( vector_size ) );
          memset( resource->suppressible_state_vector, 0, vector_size );
        }
      }

      if ( supported_clients & NPA_CLIENT_SUPPRESSIBLE2_VECTOR )
      {
        CORE_VERIFY_PTR( resource->semiactive_state_vector =
                         (npa_resource_state *)Core_Malloc( vector_size ) );
        memset( resource->semiactive_state_vector, 0, vector_size );

        /* If resource has a SUPPRESSIBLE2 dependency on another resource, we
           allocate a vector for it to aggregate suppressible2 requests into */
        if ( npa_resource_has_dependency_of_type( 
                              resource, NPA_CLIENT_SUPPRESSIBLE2_VECTOR ) )
        {
          CORE_VERIFY_PTR( resource->suppressible2_state_vector =
                           (npa_resource_state *)Core_Malloc( vector_size ) );
          memset( resource->suppressible2_state_vector, 0, vector_size );
        }
      }
    }

    /* The DRIVER_UNCONDITIONAL attribute is always set for vectors */
    _npa_set_resource_attribute( resource, NPA_RESOURCE_DRIVER_UNCONDITIONAL );
  }

  /* Turn off redundant attributes */
  if ( _npa_resource_has_attribute( resource, 
                                    NPA_RESOURCE_DRIVER_UNCONDITIONAL ) )
  {
    _npa_unset_resource_attribute( resource,
                                   NPA_RESOURCE_DRIVER_UNCONDITIONAL_FIRST );
  }

  /* Set this attribute on all resources */
  _npa_set_resource_attribute( resource, 
                               NPA_RESOURCE_ENABLE_PREPOST_CHANGE_EVENTS );

  /* Cannot have both a legacy create and an extended create function defined */
  CORE_VERIFY( !( definition->plugin->create_client_fcn &&
                  definition->plugin->create_client_ex_fcn ) );

  return resource;
}


/**
 * <!-- npa_create_node -->
 */
static void npa_create_node( npa_node_definition *definition,
                             npa_event_callback  *callback )
{
  npa_client               client;
  npa_node_dependency     *dependency;
  npa_resource_definition *resource_def;
  CoreMutex               *node_lock = NULL;
  unsigned int             i;

  CORE_VERIFY_PTR( callback );
  CORE_VERIFY_PTR( definition );
  CORE_VERIFY_PTR( &definition->resources[0] );

  /* Klocworks - Make sure there is at least one resource in the definition */
  CORE_VERIFY( definition->resource_count );
  
  ULOG_RT_PRINTF_2( npa.log, 
                    "npa_create_node (node: 0x%08x) (node_name: \"%s\")",
                    definition, definition->name );

  /* Create resource lock */
  if ( npa_node_has_attribute( definition, NPA_NODE_NO_LOCK ) )
  {
    /* Create a stubbed lock */
    CORE_VERIFY_PTR( node_lock = Core_MutexCreateEx( CORE_MUTEXTYPE_STUB,
                                                     CORE_MUTEXATTR_DEFAULT ) );
  }
  else
  {
    /* Create a real lock */
    CORE_VERIFY_PTR( node_lock = Core_MutexCreate( CORE_MUTEXATTR_DEFAULT ) );
  }

  /* Create dependencies (if specified and requested) */
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
    CORE_VERIFY_PTR( resource_def->handle = npa_new_resource( resource_def, 
                                                              definition, 
                                                              node_lock ) );  
    /* copy the request over from the placeholder */
    resource_def->handle->request_state = placeholder->request_state;

    /* Initialize the index for this resource */
    resource_def->handle->index = i;
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

    npa_update_resource_state( resource,
                               resource->node->driver_fcn(
                                 resource, 
                                 &client, 
                                 resource->request_state ) );
    
    ULOG_RT_PRINTF_4( npa.log, "\tcreated resource "
                      "(resource_name: \"%s\") (resource: 0x%08x) "
                      "(initial state: %d) (active state: %d)",
                      resource_def->name, resource,
                      resource->request_state, resource->active_state );

    /* Clear out request state so it doesn't interfere with client requests */
    resource->request_state = 0;
  }

  for ( i = 0, resource_def = &definition->resources[i]; 
        i < definition->resource_count;
        resource_def = &definition->resources[++i] )
  {
    /* Add resource to the list of resources */
    npa_link_resource( resource_def->name, resource_def->handle );
  }
  
  /* Invoke the node define callback */
  if ( callback->callback )
  {
    npa_dispatch_event_callback( callback, 0,
                                 (void *)definition->name, 
                                 strlen( definition->name ) );
  }

  ULOG_RT_PRINTF_2( npa.log, 
                    "\tend npa_create_node (node: 0x%08x) (node_name: \"%s\")",
                    definition, definition->name );
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
void npa_define_node_continuation( void         *user_data, 
                                   unsigned int  cb_event_type,
                                   void         *cb_data, 
                                   unsigned int  size )
{
  npa_node_definition *node;
  npa_continuation *continuation = (npa_continuation *)user_data;
  CORE_VERIFY_PTR( continuation );
  
  node = continuation->data.node.definition;

  /* Find next unavailable resource */
  while ( continuation->data.node.dependency < node->dependency_count &&
          npa_find_resource( 
            node->dependencies[continuation->data.node.dependency].name ) )
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
    ULOG_RT_PRINTF_2( npa.log, "\tnode waiting on resource "
                      "(node: \"%s\") (resource: \"%s\")",
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
void npa_alias_resource_continuation( void *user_data, unsigned int cb_event_type,
                                      void *cb_data, unsigned int size )
{
  npa_continuation *continuation = (npa_continuation *)user_data;
  CORE_VERIFY_PTR( continuation );

  CORE_VERIFY( 0 == strncmp( continuation->data.alias.resource_name, 
                             (const char *)cb_data,
                             NPA_MAX_STR_LEN + 1) );

  npa_link_resource( continuation->data.alias.alias_name, 
                     npa_find_resource( continuation->data.alias.resource_name ) );

  if ( continuation->callback.callback ) 
  {
    npa_dispatch_event_callback( &continuation->callback, 0,
                                 (void *)continuation->data.alias.alias_name, 
                                 strlen( continuation->data.alias.alias_name ) );
  }

  ULOG_RT_PRINTF_2( npa.log, 
                    "\tend npa_alias_resource (resource: \"%s\") (alias: \"%s\")", 
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

  /* Assert a bunch of conditions */
  CORE_VERIFY_PTR( definition );

  CORE_VERIFY( definition->name &&
               strlen( definition->name ) <= NPA_MAX_STR_LEN );

  CORE_VERIFY( definition->resources && definition->resource_count > 0 );

  ULOG_RT_PRINTF_4( npa.log, "npa_define_node "
                    "(node: 0x%08x) (node_name: \"%s\") " 
                    "(resource count: %d) (dependency count: %d)", 
                    definition, definition->name,
                    definition->resource_count, definition->dependency_count );

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
                      "(node: 0x%08x) (node_name: \"%s\") (dependency: \"%s\")",
                      definition, definition->name,
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

  CORE_VERIFY_PTR( resource_name );
  CORE_VERIFY( alias_name && strlen( alias_name ) <= NPA_MAX_STR_LEN );

  ULOG_RT_PRINTF_2( npa.log, "npa_alias_resource "
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
    CORE_VERIFY( npa_is_resource_type( resource, NPA_PLACEHOLDER_RESOURCE ) );
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
  ULOG_RT_PRINTF_1( npa.log, "npa_enable_node - initial construction "
                    "(node: \"%s\")", user_node->name );

  CORE_VERIFY( !npa_node_has_attribute( user_node, NPA_NODE_NO_LOCK ) );
  
  /* Copy the node and add the DSIABLEABLE attribute */
  graph_node = npa_copy_node( user_node );
  graph_node->attributes |= NPA_NODE_DISABLEABLE;
  
  npa_define_node_cb( graph_node, default_state, NULL, NULL );
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
 
  ULOG_RT_PRINTF_1( npa.log, "npa_enable_instantiated_node "
                    "(node: \"%s\")", user_node->name );
      
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

      npa_update_resource_state( resource,
                                 resource->node->driver_fcn(
                                   resource, 
                                   &client, 
                                   enable_state ) );

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
                              npa_resource_state   default_state[] )
{
  npa_event *event;
  npa_node_definition      *node = NULL;
  npa_continuation *continuation = NULL;
  npa_event      *creation_event = NULL;

  CORE_VERIFY( npa_is_resource_type( resource, NPA_DEFINED_RESOURCE ) );

  Core_MutexLock( resource->event_lock );

  event = resource->events.creation_events;
  /* If a creation callback is registered with this resource, trigger it */
  while ( event )
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

    if ( creation_event->prev )
    {
      creation_event->prev->next = creation_event->next;
    }
    if ( creation_event->next )
    {
      creation_event->next->prev = creation_event->prev;
    }
    if ( resource->events.creation_events == creation_event )
    {
      resource->events.creation_events = creation_event->next;
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

  ULOG_RT_PRINTF_1( npa.log, "\tend npa_enable_node (node: \"%s\")",
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
  unsigned int node_instantiated;
  npa_resource *resource;
  npa_node_definition *graph_node;
  
  CORE_VERIFY_PTR( user_node );

  /* make sure the graph doesn't change state */
  npa_lock_graph();

  CORE_VERIFY_PTR( 
    resource = npa_find_raw_resource( user_node->resources[0].name ) );

  if ( npa_is_resource_type( resource, NPA_INSTANTIATED_RESOURCE ) )
  {
    /* The node is instantiated, so don't have to worry about the
       graph changing state anymore */
    npa_unlock_graph();

    node_instantiated = 1;
    /* Now, lock the resource so it can't be accessed. The associated
       unlock will occur in npa_enable_node */
    Core_MutexLock( resource->node_lock );
  }
  else 
  {
    node_instantiated = 0;
  }

  CORE_VERIFY_PTR( graph_node = resource->node );

  ULOG_RT_PRINTF_1( npa.log, "npa_disable_node (node: \"%s\")",
                    graph_node->name );

  CORE_VERIFY( npa_node_has_attribute( graph_node, NPA_NODE_DISABLEABLE ) );

  CORE_VERIFY( !npa_node_has_attribute( graph_node, NPA_NODE_NO_LOCK ) );

  CORE_VERIFY( npa_node_equal( user_node, graph_node, 
                               (npa_node_attribute)~NPA_NODE_DISABLEABLE ) );
  
  for ( i = 0; i < graph_node->resource_count; i++ )
  {
    if ( node_instantiated )
    {
      resource = graph_node->resources[i].handle;
      CORE_VERIFY( resource->definition->query_fcn == NULL );
      resource->definition->query_fcn = npa_disabled_query_fcn;
    }
    else
    {
      resource = npa_find_defined_resource( graph_node->resources[i].name );
    }
    CORE_VERIFY_PTR( resource );

    resource->sequence |= NPA_RESOURCE_DISABLED;

    ULOG_RT_PRINTF_1( npa.log, "\tdisable resource (resource: \"%s\")",
                      graph_node->resources[i].name );
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

  ULOG_RT_PRINTF_1( npa.log, "\tend npa_disable_node (node: \"%s\")",
                    graph_node->name );
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
  CORE_VERIFY( marker_name && strlen( marker_name ) <= NPA_MAX_STR_LEN );

  ULOG_RT_PRINTF_1( npa.log, "npa_define_marker (name: \"%s\")", marker_name );

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
void npa_define_marker_with_attributes( const char *marker_name,
                                        npa_resource_attribute attributes )
{
  CORE_VERIFY( marker_name && strlen( marker_name ) <= NPA_MAX_STR_LEN );

  /* Only NPA_RESOURCE_INTERPROCESS_ACCESS_ALLOWED is supported for now. */
  CORE_VERIFY( attributes == NPA_RESOURCE_INTERPROCESS_ACCESS_ALLOWED );

  ULOG_RT_PRINTF_1( npa.log, 
                    "npa_define_marker_with_attributes (name: \"%s\") ",
                    marker_name );

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
void npa_resources_available_continuation( void         *user_data, 
                                           unsigned int  cb_event_type,
                                           void         *cb_data, 
                                           unsigned int  size )
{
  npa_continuation *continuation = (npa_continuation *)user_data;
  CORE_VERIFY_PTR( continuation );

  while ( continuation->data.resources.index < continuation->data.resources.count &&
          npa_find_resource( 
            continuation->data.resources.names[continuation->data.resources.index] ) )
  {
    continuation->data.resources.index++;
  }

  if ( continuation->data.resources.index == 
       continuation->data.resources.count )
  {
    ULOG_RT_PRINTF_2( npa.log, "\tnpa_resources_available complete "
                      "(callback: 0x%08x) (context: 0x%08x)",
                      continuation->callback.callback,
                      continuation->callback.context );

    npa_dispatch_event_callback( &continuation->callback, cb_event_type, 
                                 (void *)continuation->data.resources.names,
                                 continuation->data.resources.count );
    npa_destroy_continuation( continuation );
  }
  else 
  {
    const char *resource =  
      continuation->data.resources.names[continuation->data.resources.index++];

    ULOG_RT_PRINTF_3( npa.log, "\tnpa_resources_available waiting on resource "
                      "(resource: \"%s\") "
                      "(callback: 0x%08x) (context: 0x%08x)",
                      resource,
                      continuation->callback.callback,
                      continuation->callback.context );

    npa_resource_available_cb( resource, npa_resources_available_continuation,
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
void npa_resources_available_cb( unsigned int   num_resources,
                                 const char    *resources[],
                                 npa_callback   callback,
                                 void          *context )
{
  npa_continuation *continuation;

  CORE_VERIFY( resources && num_resources != 0 );
  CORE_VERIFY_PTR( callback );

  continuation = npa_new_continuation( callback, context );

  /* Initialize this continuation's state */
  continuation->data.resources.count = num_resources;
  continuation->data.resources.names = resources;
  continuation->data.resources.index = 0;

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

  /* Resource already exists - just call the callback directly */
  if ( npa_is_resource_type( resource, NPA_INSTANTIATED_RESOURCE ) )
  {
    /* Don't need to keep graph locked */
    npa_unlock_graph();

    /* Invoke the callback from outside the lock */
    callback( context, 0, (void *)resource_name, strlen( resource_name ) );
  }
  else 
  {
    /* Resource is not defined */
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
  npa_event_callback *event_callback;
  npa_link *link;

  CORE_VERIFY_PTR( callback );

  /* allocate an npa_event_callback */
  CORE_VERIFY_PTR( event_callback = (npa_event_callback *)
                                    Core_PoolAlloc( &npa.callback_pool ) );
  event_callback->callback = callback;
  event_callback->context = context;

  /* Insert the callback into wildcard list */
  npa_lock_graph();

  npa_list_PushFront( &npa.wildcard_resource_available_callbacks, 
                      event_callback );

  npa_unlock_graph();

  /* Trigger this new callback once for every existing resource/alias.
   * Note: This is outside the lock, so the callback can be re-entered
   * and invoked multiply for the same resource. The callback should be
   * able to handle this.
   */
  link = npa_get_resources();
  while ( link )
  {
    if ( npa_is_resource_type( link->resource, NPA_INSTANTIATED_RESOURCE ) )
    {
      npa_trigger_wildcard_callback( event_callback, link );
    }
    link = link->next;
  }
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
  npa_resource_definition *definition;

  CORE_VERIFY_PTR( resource );

  definition = resource->definition;

  /* If the plugin is NULL, restore from the definition */
  if ( plugin == NULL )
  {
    plugin = definition->plugin;
  }

  /* Make sure all the original supported clients continue to be supported */
  CORE_VERIFY( 
    ( definition->plugin->supported_clients & plugin->supported_clients ) == 
    definition->plugin->supported_clients );

  client = resource->clients;

  Core_MutexLock( resource->node_lock );

  while ( client )
  {
    /* Plugin being replaced; call the old plugin's client_destroy function */
    if ( resource->active_plugin->destroy_client_fcn )
    {
      resource->active_plugin->destroy_client_fcn( client );
    }

    /* Then, call the new plugins's client_create function */
    if ( plugin->create_client_fcn )
    {
      plugin->create_client_fcn( client );
      CORE_VERIFY( client->resource == resource );
    }

    client = client->next;
  }
  
  /* Largish section of code that will likely never be used, but in the
     interests of completeness... */
  if ( _npa_resource_has_attribute( resource, NPA_RESOURCE_VECTOR_STATE ) )
  {
    unsigned int vector_size = definition->max * sizeof(npa_resource_state);

    /* TODO: Or not. In the below section, we may be able to repurpose some
     * buffers without allocating afresh, but that is a whole lot of code and
     * nobody really uses npa_change_resource_plugin anyway, so pass for now.
     */

    /* If the old plugin used the identity_update_fcn and the new plugin
       doesn't, we need to recreate the required and semi_active vectors */
    if ( resource->active_plugin->update_fcn == npa_identity_update_fcn &&
         plugin->update_fcn != npa_identity_update_fcn )
    {
      /* First thing, allocate a new overall state_vector */
      npa_resource_state *vector;
      CORE_VERIFY_PTR( vector = 
                       (npa_resource_state *)Core_Malloc( vector_size ) );
      core_memcpy( vector, vector_size, resource->state_vector, vector_size );
      resource->state_vector = vector;
      
      /* We leave the required_state_vector (if it exists) alone */
      
      /* If the new plugin supports SUPPRESSIBLE2 clients, we need to allocate
         a semi_active_state_vector */
      if ( plugin->supported_clients & NPA_CLIENT_SUPPRESSIBLE2_VECTOR )
      {
        CORE_VERIFY_PTR( vector =
                         (npa_resource_state *)Core_Malloc( vector_size ) );
        core_memcpy( vector, vector_size, resource->state_vector, vector_size );
        resource->semiactive_state_vector = vector;
      }

      /* Free the suppressible and suppressible2 vectors, if they exist */
      Core_Free( resource->suppressible_state_vector );
      Core_Free( resource->suppressible2_state_vector );
    }
    else if ( resource->active_plugin->update_fcn != npa_identity_update_fcn &&
              plugin->update_fcn == npa_identity_update_fcn )
    {
      /* If instead, the new plugin uses the identity_update_fcn and the old
         one didn't... */

      if ( resource->required_state_vector == NULL )
      {
        CORE_VERIFY_PTR( resource->required_state_vector =
                         (npa_resource_state *)Core_Malloc( vector_size ) );
        memset( resource->required_state_vector, 0, vector_size );
      }

      if ( resource->suppressible_state_vector == NULL )
      {
        CORE_VERIFY_PTR( resource->suppressible_state_vector =
                         (npa_resource_state *)Core_Malloc( vector_size ) );
        memset( resource->suppressible_state_vector, 0, vector_size );
      }

      if ( resource->suppressible2_state_vector == NULL )
      {
        CORE_VERIFY_PTR( resource->suppressible_state_vector =
                         (npa_resource_state *)Core_Malloc( vector_size ) );
        memset( resource->suppressible_state_vector, 0, vector_size );
      }

      /* Free state_vector and semi_active_state_vector, if they exist */
      Core_Free( resource->state_vector );
      Core_Free( resource->semiactive_state_vector );

      /* This may be wrong, but there isn't a right. I expect these to be
         overwritten on the first request after this */
      resource->state_vector = \
        resource->semiactive_state_vector = resource->required_state_vector;
    }
  }

  resource->active_plugin = plugin;
  Core_MutexUnlock( resource->node_lock );
}


/**
 * <!-- npa_reset -->
 *
 * @brief FOR TEST PURPOSES ONLY. Do not call this function unless you're 
 * absolutely sure of what you're upto. And even then, you probably shouldn't.
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

  if ( link ) 
  { 
    CORE_VERIFY( NULL == link->prev );
    link->prev = &sentinel;
  }
  
  /* First, remove all the aliases/placeholders */
  while ( link )
  {
    next_link = link->next;
    if ( npa_is_resource_type( link->resource, NPA_INSTANTIATED_RESOURCE ) ||
         npa_is_link_alias( link ) )
    {
      link->prev->next = next_link;
      if ( next_link )
      {
        next_link->prev = link->prev;
      }
      
      Core_PoolFree( &npa.link_pool, link );
    }
    link = next_link;
  }

  /* Now remove all the remaining links */
  link = sentinel.next;
  while ( link )
  {
    next_link = link->next;

    link->prev->next = next_link;
    if ( next_link )
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
