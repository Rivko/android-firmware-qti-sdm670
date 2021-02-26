#ifndef _UNPA_RESOURCE_H_
#define _UNPA_RESOURCE_H_
/*==============================================================================
  FILE:         unpa_resource.h
  
  OVERVIEW:     UNPA resource API. Define and operate resources.

  DEPENDENCIES: None

                Copyright (c) 2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/api/power/unpa/unpa_resource.h#2 $
$DateTime: 2017/10/27 10:53:19 $
==============================================================================*/
#include "comdef.h"
#include "unpa.h"
#include "qurt_mutex.h"
#include "unpa_event.h"

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
                              MACRO DEFINITIONS
 =============================================================================*/ 
/* Supported client types.
 * Set unpa_resource_definition->client_types to any ORed combination of client
 * types in unpa.h or one of the below values */

#define UNPA_UPDATE_CLIENT_TYPES  0x00  /* Resource uses an UNPA-provided update_fcn 
                                         * and supports those client types the
                                         * update_fcn does */

#define UNPA_ALL_CLIENT_TYPES     0xff  /* Resource supports clients of any type */


/* Resource attributes - use #defines, rather than an enum to enable bit
 * fields in unpa_resource */
#define UNPA_RESOURCE_DEFAULT               0x00

#define UNPA_RESOURCE_DRIVER_UNCONDITIONAL  0x01  /* Call the driver function irrespective 
                                                   * of whether the current request results in an
                                                   * aggregation different from active_state or not */

#define UNPA_RESOURCE_CLIENT_CHANGE_NOTIFY  0x02  /* Call the driver function with one of 
                                                   * UNPA_REQUEST_ADD/REMOVE_CLIENT request 
                                                   * attributes, when a client is added or removed */



/* Helper macros to access client request fields */
#define UNPA_ACTIVE_REQUEST(client)         (client->active_request)
#define UNPA_PENDING_REQUEST(client)        (client->pending_request)

/* Macro to access the sleep state of the resource */
#define UNPA_RESOURCE_SLEEP_STATE(resource) (resource->sleep_state)

/* Macros to access request aggregations by type */
#define UNPA_REQUIRED_INDEX       0
#define UNPA_SUPPRESSIBLE_INDEX   1
#define UNPA_SLEEP_INDEX          2


/* Special client type used within UNPA. The driver function of the resource
 * is invoked with a client of this type, during resource definition/creation,
 * to allow any initialization. See unpa_define_resource for more. */
#define UNPA_CLIENT_INITIALIZE 0x0

/* Internal request attributes */

/* If a resource has the UNPA_RESOURCE_CLIENT_CHANGE_NOTIFY attribute,
 * UNPA sets one of these request attribute and invokes the driver function
 * to indicate that "client" has been added to or removed from the resource. */
#define UNPA_REQUEST_ADD_CLIENT     0x1
#define UNPA_REQUEST_REMOVE_CLIENT  0x2

/* This attribute is set by UNPA on complete/cancel request, to distinguish
 * those from a issue_request(0) */
#define UNPA_REQUEST_DROP_VOTE      0x4

/* This attribute is set by UNPA if the request is from a proxy for a remote
   (userPD) client */
#define UNPA_REQUEST_REMOTE_CLIENT  0x8

/*==============================================================================
                                TYPE DEFINITIONS
 =============================================================================*/
/* An update function defines how the resource aggregates requests */
typedef unpa_resource_state (*unpa_resource_update_fcn)(struct unpa_resource  *resource, 
                                                        unpa_client           *client);

/* A driver function defines how the resource applies requests */  
typedef unpa_resource_state (*unpa_resource_driver_fcn)(struct unpa_resource *resource, 
                                                        unpa_client          *client, 
                                                        unpa_resource_state   state);
 
/* A UNPA resource definition may be placed in code or RO memory */
typedef struct unpa_resource_definition
{
  /* Length of "name", incl. the '\0', must be < UNPA_MAX_NAME_LEN */
  const char               *name;
  
  /* Pointer to the function used to aggregate resource requests */
  unpa_resource_update_fcn  update_fcn;
  
  /* Pointer to the function that applies the aggregated request */
  unpa_resource_driver_fcn  driver_fcn;
  
  /* Max possible resource state; used to initialize or reset active_max in unpa_resource */
  unpa_resource_state       max;
  
  /* Bitmask of supported client types */
  uint32                    client_types : 8;
  
  /* Resource attributes */
  uint32                    attributes : 24;
} unpa_resource_definition;

/* This data structure represents the dynamic state of the UNPA resource */
typedef struct unpa_resource
{
  /* Pointer to a unpa_resource_definition data structure */
  unpa_resource_definition *definition;
  
  /* Linked list of clients registered with this resource */
  unpa_client              *clients;
  
  /* The active state of the resource; for sysPM based resources, this represents the AS setting */
  unpa_resource_state       active_state;
  
  /* The sleep state of the resource; for sysPM based resources, this represents the SS setting */
  unpa_resource_state       sleep_state;
  
  /* The aggregation of requests of a particular type */
  unpa_resource_state       agg_state[3];
  
  /* The aggregation of resource requests will be clipped to active_max, before being passed
   * into the driver function */
  unpa_resource_state       active_max;
  
  /* In multi-threaded operating modes, serialises requests to the resource */
  qurt_mutex_t              lock;
  
  /* List of events registered with this resource */
  struct unpa_event        *events;

  /* Allows resource authors to associate any user data with this resource */
  void                     *user_data;
  
  /* Pointer to the next resource in the global unpa_resources */
  struct unpa_resource     *next;
} unpa_resource;
 
/*==============================================================================
                              GLOBAL FUNCTIONS
 =============================================================================*/
/** 
 * unpa_createResource
 *
 * @brief Allocates and initializes a new resource based on the given definition.
 *        After the new resource is linked into unpa_resources, a request with
 *        value "initialState" is issued to the resource with a special
 *        client_type NPA_CLIENT_INITIALIZE. This may be used to perform
 *        resource specific initializations.
 *        The active_max of the resource is set to definition->max
 *
 * @param definition:     Pointer to a unpa_resource_definition.
 * @param initialState:   Initial state of the resource. You may ignore this
 *                        value in the NPA_CLIENT_INITIALIZE section, if your resource
 *                        needs no init.
 *
 * @return Returns a pointer to the created unpa_resource data structure. 
 */
#define unpa_createResource(definition, initialState) \
          unpa_newResource(definition, initialState, NULL)

/** 
 * unpa_initResource
 *
 * @brief Initializes the given unpa_resource data structure based on the
 *        given definition. After the new resource is linked into unpa_resources,
 *        a request with value "initialState" is issued to the resource with a
 *        special client_type NPA_CLIENT_INITIALIZE. This may be used to perform
 *        resource specific initializations. 
 *        The active_max of the resource is set to definition->max
 *
 * @param definition:     Pointer to a unpa_resource_definition.
 * @param initialState:   Initial state of the resource. You may ignore this
 *                        value in the NPA_CLIENT_INITIALIZE section, if your resource
 *                        needs no init.
 * @param resource:       Pointer to a (uninitialized, static) unpa_resource.
 *
 * @return Returns a pointer to the initialized unpa_resource data structure. 
 */
#define unpa_initResource(definition, initialState, resource) \
          unpa_newResource(definition, initialState, resource)

/** 
 * unpa_newResource
 *
 * @brief Do not use this routine directly. Instead, use either
 * unpa_createResource or unpa_initResource depending on whether you want
 * UNPA to allocate a new resource data structure or pass in one.
 *
 * @param definition:     Pointer to a unpa_resource_definition.
 * @param initial_state:  Initial state of the resource. You may ignore this
 *                        value in the NPA_CLIENT_INITIALIZE section, if your resource
 *                        needs no init.
 * @param resource:       Pointer to a unpa_resource

 *
 * @return Returns a pointer to the created unpa_resource data structure. 
 */
unpa_resource* unpa_newResource(unpa_resource_definition *definition,
                                unpa_resource_state       initialState,
                                unpa_resource            *resource);

/** 
 * unpa_stubResource
 *
 * @brief Define a stub resource that accepts clients and requests, but does nothing. 
 *        This allows client code to be decoupled from resource availability during development.
 *        The active state of a stubbed resource is always 0.
 *        There can only be UNPA_MAX_STUBS number of stub resources defined.
 *
 * @param resource_name: Name of the resource to be stubbed. Length of the name, incl. 
 *                       the '\0', must be < UNPA_MAX_NAME_LEN.
 */ 
void unpa_stubResource(const char *resourceName);

/** 
 * unpa_getAggIndex 
 *  
 * @brief Map client->type (one of REQUIRED, SUPPRESSIBLE or SLEEP_ONLY)
 *        to index in the resource->agg_state array. Err-fatals if invoked with
 *        any other client type.
 */
uint32 unpa_getAggIndex(unpa_client *client);

/** Update functions */
/** 
 * unpa_minUpdateFcn 
 *  
 * @brief Returns the minimum of all active non-zero requests or 0, if all
 *        active_requests are 0.
 */
unpa_resource_state unpa_minUpdateFcn(unpa_resource *resource, unpa_client *client);

/** 
 * unpa_maxUpdateFcn 
 *  
 * @brief Returns the maximum of all active requests. 
 */
unpa_resource_state unpa_maxUpdateFcn(unpa_resource *resource, unpa_client *client);

/**
 * unpa_binaryUpdateFcn 
 *  
 * @brief Returns 1 if there is atleast one active non-zero request, or 0. 
 *        IMPORTANT: This is different from how the plugin/update_fcn is defined
 *        in full NPA, where we return active_max if there is atleast one
 *        non-zero vote. Modifying this for uNPA saves 2 words per resource.
 */
unpa_resource_state unpa_binaryUpdateFcn(unpa_resource *resource, unpa_client *client);

/** 
 * unpa_orUpdateFcn 
 *  
 * @brief Returns the binary OR of all active requests. 
 */
unpa_resource_state unpa_orUpdateFcn(unpa_resource *resource, unpa_client *client);

#ifdef __cplusplus
}
#endif

#endif /* _UNPA_RESOURCE_H_ */
