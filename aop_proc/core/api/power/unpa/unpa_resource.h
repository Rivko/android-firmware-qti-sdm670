/*============================================================================
@file unpa_resource.h

UNPA resource API. Define and operate resources.

Copyright (c) 2016 QUALCOMM Technologies Incorporated.
All Rights Reserved.
QUALCOMM Technologies Proprietary/GTDR

============================================================================*/
#ifndef UNPA_RESOURCE_H
#define UNPA_RESOURCE_H

#include "unpa.h"
#include "rex.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------
 * Defines/Enums
 * -------------------------------------------------------------------------*/
 
/* Supported client types. Set unpa_resource_definition->client_types to any
   ORed combination of client types in unpa.h or one of the below values */

/* If the resource uses an UNPA-provided update_fcn and supports all
   client types the update function does, use this value */
#define UNPA_UPDATE_CLIENT_TYPES 0x0

/* If the resource supports clients of any type, use this value */
#define UNPA_ALL_CLIENT_TYPES 0xff

 
/* Resource attributes - we use #defines, rather than an enum to enable bit
   fields in unpa_resource */
#define UNPA_RESOURCE_DEFAULT 0x0

/* Call the driver function irrespective of whether the current request
   results in an aggregation different from active_state or not */ 
#define UNPA_RESOURCE_DRIVER_UNCONDITIONAL 0x1

/* Call the driver function with one of UNPA_REQUEST_ADD/REMOVE_CLIENT 
   request attributes, when a client is added or removed */
#define UNPA_RESOURCE_CLIENT_CHANGE_NOTIFY 0x2

/* Resource supports asynchronous requests */
#define UNPA_RESOURCE_ASYNC_REQUESTS 0x4


/* Helper macros to access client request fields */
#define UNPA_ACTIVE_REQUEST( client )  ( client->active_request )
#define UNPA_PENDING_REQUEST( client ) ( client->pending_request )

/* Macro to access the sleep state of the resource */
#define UNPA_RESOURCE_SLEEP_STATE( resource ) ( resource->sleep_state )

/* Macros to access request aggregations by type */
#define UNPA_REQUIRED_INDEX 0
#define UNPA_SUPPRESSIBLE_INDEX 1
#define UNPA_SLEEP_INDEX 2


/* Special client type used within UNPA. The driver function of the resource
 * is invoked with a client of this type, during resource definition/creation,
 * to allow any initialization. See unpa_define_resource for more.
 */
#define UNPA_CLIENT_INITIALIZE 0x0


/* Internal request attributes */

/* If a resource has the UNPA_RESOURCE_CLIENT_CHANGE_NOTIFY attribute,
 * UNPA sets one of these request attribute and invokes the driver function
 * to indicate that "client" has been added to or removed from the resource.
 */
#define UNPA_REQUEST_ADD_CLIENT    UNPA_REQUEST_RESERVED_ATTR1
#define UNPA_REQUEST_REMOVE_CLIENT UNPA_REQUEST_RESERVED_ATTR2

/* This attribute is set by UNPA on a complete/cancel request, to distinguish
   that action from a issue request(0) */
#define UNPA_REQUEST_DROP_VOTE     UNPA_REQUEST_RESERVED_ATTR3

/* Set by UNPA when the issue_request_async API is used. Resource must invoke
   client->done_cb after request completion */
#define UNPA_REQUEST_ASYNC         UNPA_REQUEST_RESERVED_ATTR4


/*----------------------------------------------------------------------------
 * Types
 * -------------------------------------------------------------------------*/
 
/* An update function defines how the resource aggregates requests */
typedef unpa_resource_state ( *unpa_resource_update_fcn ) ( 
  struct unpa_resource *resource, unpa_client *client );

/* A driver function defines how the resource applies requests */  
typedef unpa_resource_state ( *unpa_resource_driver_fcn ) (
  struct unpa_resource *resource, unpa_client *client, 
  unpa_resource_state state );   
 
/* A UNPA resource definition may be placed in code or RO memory */
typedef struct unpa_resource_definition
{
  /* Length of "name", incl. the '\0', must be < UNPA_MAX_NAME_LEN */
  const char *name;
  
  /* Pointer to the function used to aggregate resource requests */
  unpa_resource_update_fcn update_fcn;
  
  /* Pointer to the function that applies the aggregated request */
  unpa_resource_driver_fcn driver_fcn;
  
  /* Max possible resource state; used to initialize or reset active_max
     in unpa_resource */
  unpa_resource_state max;
  
  /* Bitmask of supported client types */
  uint32_t client_types : 8;
  
  /* Resource attributes */
  uint32_t attributes : 24;  
} unpa_resource_definition;

/* This data structure represents the dynamic state of the UNPA resource */
typedef struct unpa_resource
{
  /* Pointer to a unpa_resource_definition data structure */
  unpa_resource_definition *definition;
  
  /* Linked list of clients registered with this resource */
  unpa_client *clients;
  
  /* The active state of the resource; for sysPM based resources, this
     represents the AS setting */
  unpa_resource_state active_state;
  
  /* The sleep state of the resource; for sysPM based resources, this
     represents the SS setting */
  unpa_resource_state sleep_state;
  
  /* The aggregation of requests of a particular type */
  unpa_resource_state agg_state[3];
  
  /* The aggregation of resource requests will be clipped to active_max, 
     before being passed into the driver function */
  unpa_resource_state active_max;
  
  /* Internal resource attributes, used by the framework */
  uint32_t internal_attr;
  
  /* In multi-threaded operating modes, serialises requests to the resource */
  rex_crit_sect_type lock;
  
  /* Allows resource authors to associate any user data with this resource */
  void *user_data;
  
  /* Pointer to the next resource in the global unpa_resources */
  struct unpa_resource *next;
} unpa_resource;
 
/*----------------------------------------------------------------------------
 * Functions
 * -------------------------------------------------------------------------*/

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
 * @param initial_state: Initial state of the resource. You may ignore this
 * value in the NPA_CLIENT_INITIALIZE section, if your resource needs no init.
 *
 * @return Returns a pointer to the created unpa_resource data structure. 
 */ 
unpa_resource* unpa_create_resource( unpa_resource_definition *definition, 
                                     unpa_resource_state initial_state );
                           
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
void unpa_stub_resource( const char *resource_name );


/** Update functions */

/** 
 * @brief Returns the minimum of all active non-zero requests or 0, if all
 * active_requests are 0.
 */
unpa_resource_state
unpa_min_update_fcn( unpa_resource *resource, unpa_client *client );

/** 
 * @brief Returns the maximum of all active requests.
 */
unpa_resource_state
unpa_max_update_fcn( unpa_resource *resource, unpa_client *client );

/** 
 * @brief Returns the sum of all active requests
 */
unpa_resource_state
unpa_sum_update_fcn( unpa_resource *resource, unpa_client *client );

/**
 * @brief Returns 1 if there is atleast one active non-zero request, or 0.
 * IMPORTANT: This is different from how the plugin/update_fcn is defined
 * in the parent/full NPA, where we return active_max if there is atleast
 * one non-zero vote. Modifying this for UNPA saves 2 words per resource.
 */
unpa_resource_state
unpa_binary_update_fcn( unpa_resource *resource, unpa_client *client );

/**
 * @brief Map client->type (one of REQUIRED, SUPPRESSIBLE or SLEEP_ONLY)
 * to index in the resource->agg_state array. Err-fatals if invoked with
 * any other client type.
 */
uint32_t unpa_get_agg_index( unpa_client *client );

#ifdef __cplusplus
}
#endif

#endif /* UNPA_RESOURCE_H */
