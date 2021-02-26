/*============================================================================
@file unpa.c

Init, resource definition routines in the UNPA framework

Copyright (c) 2016 QUALCOMM Technologies Incorporated.
All Rights Reserved.
QUALCOMM Technologies Proprietary/GTDR

============================================================================*/

#include "unpa_internal.h"

/*----------------------------------------------------------------------------
 * Globals
 * -------------------------------------------------------------------------*/

unpa_struct unpa;

/*----------------------------------------------------------------------------
 * Stub resources
 * -------------------------------------------------------------------------*/
 
static unpa_resource_state
unpa_stub_update_fcn( unpa_resource *resource, unpa_client *client )
{
  return 0;
}
 
static unpa_resource_state
unpa_stub_driver_fcn( unpa_resource *resource, unpa_client *client,
                      unpa_resource_state state )
{
  return 0;
}

static unpa_resource_definition unpa_stub_resrc_defn = {
  "STUB",
  unpa_stub_update_fcn,
  unpa_stub_driver_fcn,
  0xFFFFFFFF,
  UNPA_ALL_CLIENT_TYPES,
  UNPA_RESOURCE_DEFAULT,
};

static unpa_resource *unpa_stub_resrc;

/*----------------------------------------------------------------------------
 * Init
 * -------------------------------------------------------------------------*/

/** 
 * <!-- unpa_init -->
 * @brief Initializes the uNPA framework.
 */ 
void unpa_init( void )
{
   rex_init_crit_sect( &unpa.lock);
   
  // Init log here
  
  unpa.resources = NULL;
  
  // Create the stub resource
  unpa_stub_resrc = unpa_create_resource( &unpa_stub_resrc_defn, 0 );  
}

/*----------------------------------------------------------------------------
 * Functions
 * -------------------------------------------------------------------------*/

/**
 * <!-- unpa_resource_has_attribute -->
 * @brief Returns TRUE if the resource has the given attribute.
 */
uint32_t
unpa_resource_has_attribute( unpa_resource *resource, uint32_t attribute )
{
  return ( resource->definition->attributes & attribute );
}

/**
 * <!-- unpa_get_resource -->
 * Returns the unpa_resource* with the given name or NULL.
 */
unpa_resource* unpa_get_resource( const char *resource_name )
{
  unpa_resource *resource;
  
  rex_enter_crit_sect( &unpa.lock );
  
  resource = unpa.resources;
  
  while ( resource )
  {
    if ( 0 == strncmp( resource_name, resource->definition->name, 
                       UNPA_MAX_NAME_LEN+1 ) )
    {
      break;
    }
    resource = resource->next;
  }
  
  /* If we don't see a defined resource, look among stubs */
  if ( !resource )
  {
    int i;
    for ( i = 0; i < UNPA_MAX_STUBS && unpa.stubs[i] != NULL; ++i )
    {
      if ( 0 == strncmp( resource_name, unpa.stubs[i], UNPA_MAX_NAME_LEN+1 ) )
      {
        resource = unpa_stub_resrc;
        break;
      }
    }
  }
  
  rex_leave_crit_sect( &unpa.lock);
  
  return resource;
}

/** 
 * <!-- unpa_create_resource -->
 *
 * @brief Creates a resource based on the given definition and makes it
 * available to clients. After the new resource is linked into unpa_resources,
 * a request with value "initial_state" is issued to the resource with a
 * special client_type NPA_CLIENT_INITIALIZE. This may be used to perform
 * resource specific initializations. 
 * The active_max of the resource is set to definition->max
 *
 * @param definition: Pointer to a unpa_resource_definition.
 * @param initial_state: Initial_state of the resource. The driver function
 * is invoked with this value as "state" and client->type set to
 * UNPA_CLIENT_INITIALIZE before the resource is made available to clients.
 *
 * @return Returns a pointer to the created unpa_resource data structure.
 */ 
unpa_resource* unpa_create_resource( unpa_resource_definition *definition, 
                                     unpa_resource_state initial_state )
{
  unpa_client client;
  unpa_resource *resource;

  CORE_VERIFY_PTR( definition );
  CORE_VERIFY( strlen( definition->name ) < UNPA_MAX_NAME_LEN );
  
  rex_enter_crit_sect( &unpa.lock); 
  
  /* Verify that no resource with the given name is already defined/stubbed */
  resource = unpa_get_resource( definition->name );
  CORE_VERIFY( resource == NULL );  
  
//  resource = (unpa_resource *) malloc_island( sizeof(unpa_resource) );
    resource = (unpa_resource *) malloc( sizeof(unpa_resource) );
  CORE_VERIFY_PTR( resource );
  memset( resource, 0, sizeof(unpa_resource) );
  
  resource->definition = definition;
  resource->active_max = definition->max;
  
  /* Initialize the resource's own lock */
  rex_init_crit_sect( &resource->lock );
  
  /* Call into resource with the special INITIALIZE client */
  memset( &client, 0, sizeof(unpa_client) );
  client.type = UNPA_CLIENT_INITIALIZE;
  client.pending_request.val = initial_state;
  
  resource->active_state = 
    definition->driver_fcn( resource, &client, initial_state ); 
    
  /* Link in resource */
  resource->next = unpa.resources;
  unpa.resources = resource;
  
  rex_leave_crit_sect( &unpa.lock);
  
  return resource;
}

/** 
 * <!-- unpa_stub_resource -->
 *
 * @brief Define a stub resource that accepts clients and requests,
 * but does nothing. This allows client code to be decoupled from
 * resource availability during development.
 * The active state of a stubbed resource is always 0.
 * There can only be UNPA_MAX_STUBS number of stub resources defined. 
 *
 * @param resource_name: Name of the resource to be stubbed. Length of
 * the name, incl. the '\0', must be < UNPA_MAX_NAME_LEN.
 */ 
void unpa_stub_resource( const char *resource_name )
{
  unpa_resource *resource;
  int i;
  
  CORE_VERIFY_PTR( resource_name );
  CORE_VERIFY( strlen( resource_name ) < UNPA_MAX_NAME_LEN );
  
  rex_enter_crit_sect( &unpa.lock); 

  /* Verify that no such resource is already defined/stubbed */  
  resource = unpa_get_resource( resource_name );
  CORE_VERIFY( resource == NULL );
  
  for ( i = 0; i < UNPA_MAX_STUBS; ++i )
  {
    if ( unpa.stubs[i] == NULL )
    {
      unpa.stubs[i] = resource_name;
      break;
    }
  }
  
  /* If we didn't find a slot, we have too many stubs */
  CORE_VERIFY( i != UNPA_MAX_STUBS );
  
  rex_leave_crit_sect( &unpa.lock);
}
