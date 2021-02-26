
#ifndef NPA_RESOURCE_H
#define NPA_RESOURCE_H

/*==============================================================================
  @file npa_resource.h

  NPA Resource Function/Structure Declarations

  This file should not be included by NPA clients, but only by people
  defining NPA resources or writing plugins

  Copyright (c) 2009 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //components/rel/core.tz/2.0.2/api/power/npa_resource.h#1 $ */


/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "npa.h"

/*----------------------------------------------------------------------------
 * Enumerations/Constants
 * -------------------------------------------------------------------------*/

/* These is logically part of the npa_client_type enumeration, but
 * they arent public client types.  Only resource authors should use
 * this type. */
#define NPA_CLIENT_INITIALIZE  NPA_CLIENT_RESERVED1
#define NPA_CLIENT_INTERNAL    NPA_CLIENT_RESERVED2

/* Map client types enumerations onto bitfields for plugin usage.  The
 * plugin defines the supported plugins as the bitwise OR of supported
 * client types.
 *
 * If you add a new enum to the npa_client_type, this enum must be
 * updated to match */

/* Resource attributes are used to override default resource behavior */
typedef enum
{
  /* Use default resource behavior */ 
  NPA_RESOURCE_DEFAULT              = 0,          

  /* Invoke the driver function unconditionally - not just on state
   changes */
  NPA_RESOURCE_DRIVER_UNCONDITIONAL = 0x00000001, 

  /* Only allow one client of the resource to be created at a time */
  NPA_RESOURCE_SINGLE_CLIENT        = 0x00000002, 

  /* Allow resource to be accessed remotely */
  NPA_RESOURCE_REMOTE_ACCESS_ALLOWED = 0x00000004, 

  /* This resource is a vector resource */
  NPA_RESOURCE_VECTOR_STATE = 0x00000008, 

} npa_resource_attribute;

typedef enum
{
  NPA_NODE_DEFAULT              = 0, /* Use default node behavior */

  /* Don't create a lock for this node.  If this attribute is defined,
   * it is the responsibility of the resource author to manage
   * concurrency.  */
  NPA_NODE_NO_LOCK              = 0x00000001

} npa_node_attribute;

/* Resource author query ids.  All resource author queries ids are in
 * 1024-2047 of the reserved section */
enum {
  NPA_RESOURCE_AUTHOR_QUERY_START = NPA_MAX_PUBLIC_QUERY,
  /* Get the resource attribute vector */
  NPA_QUERY_RESOURCE_ATTRIBUTES,
  /* Get the node attribute vector */
  NPA_QUERY_NODE_ATTRIBUTES,
  NPA_MAX_RESOURCE_AUTHOR_QUERY = 2047
};

/*----------------------------------------------------------------------------
 * Types/Typedefs
 * -------------------------------------------------------------------------*/
/* User data - user defines (and must cast) to whatever they want */
typedef void * npa_user_data;

/* Internal callback structure */
typedef struct npa_callback_event
{
  npa_callback  callback;
  npa_user_data context;
} npa_callback_event;

/* Internal work request structure */
typedef struct npa_work_request
{
  npa_resource_state  state;    /* Valid for all */
  union                         /* Used for reference data */
  {
    npa_resource_state *vector;
    char               *string;
    void               *reference;
  } pointer;
  npa_resource_time   start;      /* valid for all */
  npa_resource_time   end;        /* Valid for ISOC */
  /* TODO: Frequency? Blob for adaptive data? */
} npa_work_request;

/* npa_client structure definition */
typedef struct npa_client 
{
  struct npa_client   *prev, *next;   /* Linked list */
  const char          *name;          /* Name of element */
  const char          *resource_name; /* Creation name of resource
                                       * (may be alias)  */
  struct npa_resource *resource;      /* Resource element is linked
                                       * to */
  npa_user_data        resource_data; /* Arbitrary resource data */
  npa_client_type      type;          /* Client type */
  npa_work_request     work[2];       /* Use ACTIVE/PENDING_REQUEST
                                       * macros to access */
  unsigned int         index;         /* Last request serviced */
  unsigned int         sequence;      /* Sequence number */
  struct npa_continuation *continuation; /* Continuation */

  void (*issue_request)( npa_client_handle client, 
                         int new_request ); /* Issue request function
                                              * Sync/Async */
} npa_client;

/* Helper macros for accessing the correct request structure, as they
 * are double buffered
 *
 * TODO: These macros will need to change if the buffering gets
 * greater than double buffered */

/* Access requests being processed */
#define NPA_ACTIVE_REQUEST( client )  ((client)->work[((client)->index)])
#define NPA_PENDING_REQUEST( client ) ((client)->work[((client)->index)^1])

/* Remap request for NPA_MAX_STATE into request for the maximum of the
 * current resource
 *
 * This macro is for use in plugin functions */
#define NPA_REMAP_MAX_REQUEST( resource, client )                       \
  NPA_PENDING_REQUEST(client).state =                                   \
    (NPA_PENDING_REQUEST(client).state == NPA_MAX_STATE) ?              \
    client->resource->definition->max : NPA_PENDING_REQUEST(client).state


/* Event structure definition */
typedef struct npa_event
{
  struct npa_event          *prev, *next;  /* Linked list */
  const char                *name;         /* Event Name */
  const char                *resource_name;/* Creation name of
                                            * resource (may be
                                            * alias)  */
  struct npa_resource       *resource;     /* Resource element is
                                            * linked to */
  npa_resource_state_delta   hi_watermark; /* Watermarks */
  npa_resource_state_delta   lo_watermark; /* Watermarks */
  npa_callback_event         callback;     /* Callback */
} npa_event;

/*======================================================================
  Resource types
======================================================================*/

/* Callback functions used to customize resource behavior */

/* Driver Function
 *
 * Typically the only function a node author needs to write. This
 * function applies the computed state to the given resource. */
typedef npa_resource_state (*npa_resource_driver_fcn) ( 
  struct npa_resource *resource,
  npa_client_handle    client,
  npa_resource_state   state 
  );

/* Update Function
 *
 * The Update Function receives the new request and determines the new
 * state for the resource. This function is part of the plugin.  There
 * are number of standard plugins provided by the framework */
typedef npa_resource_state (*npa_resource_update_fcn)( 
  struct npa_resource *resource,
  npa_client_handle    client
  );


/* Query Function
 *
 * This is an optional function to enable the resource to support an
 * extended query API. If defined, it will be invoked for any
 * queries. If the query function does not wish to support the query,
 * it shall return NPA_QUERY_UNSUPPORTED_QUERY_ID.  If the query
 * function returns that the id is unsupported, the framework will
 * attempt to handle the query id.
 *
 * The resource is *not* locked when the query function is invoked.
 * If the query requires atomic processing, it is the responsibility
 * of the query function to lock the resource */
typedef npa_query_status (*npa_resource_query_fcn)( 
  struct npa_resource *resource, 
  unsigned int         query_id, 
  npa_query_type      *query_result );


/* Plugin Definition */
typedef struct npa_resource_plugin
{
  npa_resource_update_fcn  update_fcn;        /* Update function */
  unsigned int             supported_clients; /* Bitmask of supported clients */

  /* Plugin Vtable */

  /* Create Client Function - A new client has been created, allow the
   * the plugin a chance to do something before the new client is
   * returned to the user
   *
   * Note: This function has been deprecated in favor of the Extended
   * Client Create Function. It is an error for a plugin to define
   * both the Client Create Function and the Extended Client Create
   * Function */
  void (*create_client_fcn) ( npa_client * ); /* Client Create Function */

  /* Destroy Client Function - The client has been destroyed, allow
   * the plugin an opportunity to clean up before the client is
   * freed */
  void (*destroy_client_fcn)( npa_client * ); /* Client Destroy Function */

  /* Extended Client Create Function - A new client has been created,
   * allow the the plugin a chance to do something before the new
   * client is returned to the user.  This function recieves whatever
   * optional arguments the user may have passed in.  This function
   * should return 0 if the extended creation was successful or a
   * non-zero error code if the extended creation failed.  In the
   * event of failure, the error code will be logged to the NPA log
   * and the user will receive a NULL handle 
   *
   * Note: It is an error for a plugin to define both the Client
   * Create Function and the Extended Client Create Function */
  unsigned int (*create_client_ex_fcn)( npa_client *, unsigned int, void * );

  /* Cancel Client Fucntion - The user has canceled their client
   * request, allow the plugin an opportunity to flush any learned
   * state associated with the client.
   *
   * This function may be NULL - if so, cancel will behave the same as
   * complete. */
  void (*cancel_client_fcn) ( npa_client *); /* Cancel Client Function */
} npa_resource_plugin;


/*======================================================================

======================================================================*/

/* Node Dependency structure
 *
 * A node defines its dependencies via a variable length array of
 * these structures.  The number of elements in the array is defined
 * by the dependency_count field in the node_definition structure
 */
typedef struct npa_node_dependency
{
  const char        *name;          /* Name of resource */
  npa_client_type    client_type;   /* Type of client to create */
  npa_client_handle  handle;        /* Client Handle - created by NPA */
}  npa_node_dependency;

/* Helper macro to access dependencies by index */
#define NPA_DEPENDENCY( resource, index ) \
  resource->node->dependencies[index].handle


/* Resource Definition Structure
 *
 * A node defines its resources via a variable length array of
 * these structures.  The number of elements in the array is defined
 * by the resource_count field in the node_definition structure
 */
typedef struct npa_resource_definition
{
  const char            *name;          /* Name of resource */
  const char            *units;         /* Units for this resource */
  npa_resource_state     max;           /* Maximum state/length of vector resource */
  const npa_resource_plugin *plugin;    /* Plugin */
  unsigned int           attributes;    /* Resource Attributes */
  npa_user_data          data;          /* User data field  */
  npa_resource_query_fcn query_fcn;     /* Query fcn */

 /* Handle to this resource 
  *
  * This field will be initialized in by the framework when the node
  * is created. Do not define or define as NULL  */
  struct npa_resource *handle;       
} npa_resource_definition;

/* Get the resource associated with a given node, given the current resource  */
#define NPA_RESOURCE( resource, index ) \
  ((resource)->node->resources[(index)].handle)

/* Helper macros for variable length arrays */
#define NPA_ARRAY_SIZE( array ) (sizeof(array)/sizeof(array[0]))
#define NPA_ARRAY( array ) NPA_ARRAY_SIZE(array), array
#define NPA_EMPTY_ARRAY 0, NULL

/* Node Definition Structure
 *
 * This is created by the Node author.  This can be constant */
typedef struct npa_node_definition
{
  const char                *name;         /* Node name */
  npa_resource_driver_fcn    driver_fcn;   /* Driver function used to
                                            * actually update resource */
  unsigned int               attributes;   /* Node Attributes */
  npa_user_data              data;         /* User data field  */
  unsigned int               dependency_count; /* Number of deps */
  npa_node_dependency       *dependencies;     /* Dep array */
  unsigned int               resource_count;   /* Number of resource */
  npa_resource_definition   *resources;        /* Resource Array */
} npa_node_definition;


/* Internal node structure */
typedef struct npa_resource
{
  /* The definition of the resource */
  npa_resource_definition  *definition; 
  unsigned int              index;

  /* Node pointer */
  npa_node_definition      *node;

  /* List of registered node users
   * These are head pointers to doubly-linked lists */
  npa_client               *clients;           /* client list */
  npa_event                *change_events;     /* change event list */
  npa_event                *watermark_events;  /* watermark event list */

  /* Dynamic resource behavior */
  const npa_resource_plugin *active_plugin;     /* The current plugin */

  /* State fields */
  npa_resource_state        request_state;     /* Requested state */
  npa_resource_state        active_state;      /* Active state of the HW */
  npa_resource_state        internal_state[4]; /* Internal state field
                                                * used by plugin as
                                                * neecessary */
  npa_resource_state       *state_vector; /* pointer to state vector -
                                           * only non-NULL if
                                           * NPA_RESOURCE_VECTOR_STATE
                                           * attibute is
                                           * defined. Length of vector
                                           * is
                                           * resource->definition->max */

  /* Dynamic state */
  npa_resource_state        active_max;        /* The current max */
  npa_resource_state_delta  active_headroom;   /* The current headroom */

  /* Locks */
  DALSYSSyncHandle          node_lock;         /* Node lock */
  DALSYSSyncHandle          event_lock;        /* Lock for events */

  unsigned int              sequence;          /* Sequence number */
  /* Note: the client lock is the same as the resource lock  */
} npa_resource;


/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * NPA Resource API
 * -------------------------------------------------------------------------*/

/**
   @brief npa_define_node_cb - Define an NPA node

   Define a new node.  Simply adding the definition of a new node may
   not be sufficient to create the node and all of its associated
   resources and add it to the resource graph.  In order to construct
   the node/resources, all of the nodes' dependencies must exist. When
   all the dependencies exist, the resource will be constructucted,
   initialized, and added to the resource graph.  Only after the
   resource is initialized and added to the graph can clients be
   created against it.

   Resource names must be globally unique. A resource will not be
   created if there is already an existing resource with the same
   name and a fatal error will be triggered
   
   The initial state vector must have an initial state value for each
   resource defined by the given node.  Behavior is undefined if the
   initial state vector has a different size than the resource vector.
   
   The define_event callback will be invoked after the node is fully
   constructed.  The callback can be NULL if creation notification is
   unneeded. If invoked, the callback arguemnts are (context, 0,
   node->name, sizeof(const char *)).

   @param node : Definition of node to be created 

   @param initial_state : State vector to initialize the resources with 

   @param define_cb : Optional callback to be invoked upon successful
       creation of the node.  

   @param context : User supplied context argument
*/
void npa_define_node_cb( npa_node_definition *node, 
                         npa_resource_state   initial_state[],
                         npa_callback         node_cb,
                         void                *context);



/**
    @brief npa_alias_resource_cb - Create an alias for an existing
    resource.  This allows the same resource to be accessed with
    multiple names.

    The alias doesn't exist until alias_event is triggered

    An error fatal will occur if there is already a resource with the
    requested alias name

    @param resource_name : The name of the resource to be linked

    @param alias_name : The alias for the resource

    @param alias_cb : Optional callback to be invoked upon successful
       alias creation of the alias.  The arguments will be ( context,
       0, *alias_name, strlen(alias_name) )

    @param context : User supplied context argument

*/ 
void npa_alias_resource_cb( const char   *resource_name, 
                            const char   *alias_name,
                            npa_callback  alias_cb,
                            void         *context);

/**
    @brief npa_define_marker - define an npa marker

    A marker is a name in the npa resource space.  It allows
    users/subsystems to signal existence without having to define a
    full NPA resource.
    
    Markers can be listed as dependencies, or used to trigger a
    resource_available callback, however markers do not support client
    requests and do not have a meaningful state.  In order to depend
    on a marker, a resource should define the dependency's client type
    as NPA_NO_CLIENT.  Dependencies defined as NPA_NO_CLIENT will
    cause node creation to be properly dependent, but will not
    allocate a client.

    An error fatal will occur if there is already a resource with the
    requested marker name

    @param marker_name : The name of the marker to create.  The string
    is not copied, so the pointer must point to a static string.

*/ 
void npa_define_marker( const char *marker_name );
/*======================================================================
  Author API

  These are special purpose functions intended for use by the node
  author.  
  ======================================================================*/
/**
   @brief npa_assign_resource_state - Assign the resource state
   directly, bypassing the update and driver functions.  This will
   trigger events as necessary.

   This function is intended for use in a compound node when one
   resource's state change can cause another coupled resource's state
   to change. This function must be called from within the node's
   atomic section - which means it can be invoked from within the
   update or driver function.  It require the npa_resource *, which
   can be retrived for coupled resources from the current resource
   by the NPA_RESOURCE macro.

   @param resource : resource whose state is changing
   @param state : new state of resource
*/
void npa_assign_resource_state( npa_resource       *resource,
                                npa_resource_state  state );


/** @brief npa_query_get_resource - retrieve the resource associated
   with the given query handle.  This function is intended to be used
   by resources that wish to define a query API that goes beyond what
   npa_resource_query_fcn can support
   
   @param query_handle : Handle to resource being queried

   @return Pointer to resource associated with the query handle
*/
npa_resource *npa_query_get_resource( npa_query_handle query_handle ); 

/*======================================================================
  Plugin API
  ======================================================================*/
/**
   @brief npa_change_resource_plugin - Change the plugin for the given
   resource.

   The new plugin must support all the client work models supported by
   the plugin that was in the resource definition.  If the new plugin
   is NULL, the original plugin will be restored.

   If they exist, the client destroy function of the plugin being
   replaced and the client create function of the new plugin will be
   invoked for all clients of the resource.

   It is a fatal error to attempt to change the plugin for a resource
   that does not exist.

   @param resource_name : name of the resource whose plugin is to be
   changed
   @param plugin : new plugin to use, NULL to restore original plugin
*/
void npa_change_resource_plugin( const char                *resource_name,
                                 const npa_resource_plugin *plugin );


/*======================================================================
  Standard Plugins

  These plugins are intended for common cases.  If there is not one
  sufficient for the resource authors needs, they should create a new
  plugin.

  Note: The plugins are defined as structures, but typically the
  pointers to the structure is used.  Resource authors will take the
  address of the structure when defining their resources
  ======================================================================*/

/* Resource state is off/on (max value) */
extern const npa_resource_plugin npa_binary_plugin;

/* Resource state is the max of concurrent requests */
extern const npa_resource_plugin npa_max_plugin;

/* Resource state is the min of concurrent requests */
extern const npa_resource_plugin npa_min_plugin;

/* Resource state is the sum of concurrent requests */
extern const npa_resource_plugin npa_sum_plugin;

/* Resource state is always the last client request, irrespective of
 * other client requests.
 *
 * This can be used if the resource requests are logical (enums or the
 * like) and the resource will examines the client request in the
 * driver function. */
extern const npa_resource_plugin npa_identity_plugin;

/* Resource state is always on (max value), irrespective of client requests */
extern const npa_resource_plugin npa_always_on_plugin;

/* Resource only supports impulse clients */
extern const npa_resource_plugin npa_impulse_plugin;

/* Resource state is the bitwise or of concurrent requests */
extern const npa_resource_plugin npa_or_plugin;

/* Resource supports no clients - used for markers */
extern const npa_resource_plugin npa_no_client_plugin;

/*======================================================================
  Internal Client API

  Note: The internal client type is created/destroyed via the standard
  client api. 
  ======================================================================*/

/**
   @brief npa_issue_internal_request - Issue the INTERNAL client work request

   Note: The semantics of an internal client request are
   resource/plugin specific, there's no general API for setting the
   work request fields - it's the responsibility of the user to set the
   internal client state appropriately outside of the framework.  This
   function is used to issue the request after the internal fields have
   been established.

   The client must be of INTERNAL work model. Clients with internal
   client types are created/destroyed via the standard client api.

   @param client : handle to client being used
*/
void npa_issue_internal_request( npa_client_handle client );



/* Legacy API - invoke callback functions with dal_event callback.
 *
 * These calls are deprecated and should not be used by new code
 */
/**
   @brief npa_define_node_dal - Define an NPA node

   Define a new node.  Simply adding the definition of a new node may
   not be sufficient to create the node and all of its associated
   resources and add it to the resource graph.  In order to construct
   the node/resources, all of the nodes' dependencies must exist. When
   all the dependencies exist, the resource will be constructucted,
   initialized, and added to the resource graph.  Only after the
   resource is initialized and added to the graph can clients be
   created against it.

   Resource names must be globally unique. A resource will not be
   created if there is already an existing resource with the same
   name and a fatal error will be triggered
   
   The initial state vector must have an initial state value for each
   resource defined by the given node.  Behavior is undefined if the
   initial state vector has a different size than the resource vector.
   
   The define_event will be triggered after the node is fully
   constructed.  The define_event can be NULL if creation
   notification is unneeded.

   If the event is a callback event, the arguments will be 
   (user_data, 0, node->name, sizeof(const char *)).

   @param node : Definition of node to be created 

   @param initial_state : State vector to initialize the resources with 

   @param dal_event : Optional dal_event to be triggered upon
       successful creation of the node.  The arguments will be (
       context, 0, *alias_name, strlen(alias_name) )
*/
#define npa_define_node_dal( a, b, dal_event ) \
  npa_define_node_cb( a, b, NPA_DAL_CALLBACK(dal_event) )
/**
   @brief npa_define_node - Compatibility wrapper for
   npa_define_node_dal
*/
#define npa_define_node    npa_define_node_dal


/**
    @brief npa_alias_resource_dal - Create an alias for an existing
    resource.  This allows the same resource to be accessed with
    multiple names.

    The alias doesn't exist until alias_event is triggered

    An error fatal will occur if there is already a resource with the
    requested alias name

    @param resource_name : The name of the resource to be linked

    @param alias_name : The alias for the resource

    @param dal_event : Optional event to be triggered upon successful
       alias creation of the alias.  

    @param context : User supplied context argument

*/ 
#define npa_alias_resource_dal( a, b, dal_event ) \
  npa_alias_resource_cb( a, b, NPA_DAL_CALLBACK(dal_event) ) 

/**
   @brief npa_define_node - Compatibility wrapper for
   npa_alias_resoruce_dal
*/
#define npa_alias_resource npa_alias_resource_dal

#endif /* NPA_RESOURCE_H */

