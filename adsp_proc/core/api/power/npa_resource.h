/*============================================================================
@file npa_resource.h

NPA Resource Function/Structure Declarations

This file should be included only by people defining NPA resources or writing
plugins, NOT by NPA clients. The latter should use see npa.h

Copyright (c) 2009-2016 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

$Header: //components/rel/core.qdsp6/2.1/api/power/npa_resource.h#1 $
==========================================================================*/
#ifndef NPA_RESOURCE_H
#define NPA_RESOURCE_H

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "npa.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------
 * Enumerations/Constants
 * -------------------------------------------------------------------------*/

/* These are logically part of the npa_client_type enumeration, but aren't
   public client types. They are only visible to resource authors */
#define NPA_CLIENT_INITIALIZE  NPA_CLIENT_RESERVED1
#define NPA_CLIENT_INTERNAL    NPA_CLIENT_RESERVED2

/* Resource attributes are used to override default resource behavior */
typedef enum
{
  /* Use default resource behavior */ 
  NPA_RESOURCE_DEFAULT              = 0,          

  /* Invoke the driver function unconditionally - not just on state changes */
  NPA_RESOURCE_DRIVER_UNCONDITIONAL = 0x00000001, 

  /* Only allow one client of the resource to be created at a time */
  NPA_RESOURCE_SINGLE_CLIENT        = 0x00000002, 

  /* This resource is a vector resource */
  NPA_RESOURCE_VECTOR_STATE = 0x00000004, 

  /* Allow resource to be accessed remotely */
  NPA_RESOURCE_REMOTE_ACCESS_ALLOWED = 0x00000008, 

  /* Allow resource to be accessed from another process */
  NPA_RESOURCE_INTERPROCESS_ACCESS_ALLOWED = 0x00000008,
  
  /* Is a remote resource; set by the framework for all remote resources
     The value of this enum is used in NPADump.cmm (is_remote). Keep in sync */
  NPA_RESOURCE_REMOTE = 0x00000010,
  
  /* A state query to remote resources with this attribute returns
     remote state, if supported by protocol */
  NPA_RESOURCE_REMOTE_PROXY = 0x00000020,
  
  /* Is a remote resource that must not be initialized via NPA */
  NPA_RESOURCE_REMOTE_NO_INIT = 0x00000040,
  
  /* Resource supports SUPPRESSIBLE requests. Resources need not set this
   * explicitly as the framework deduces this from whether the resources
   * supports clients of type NPA_CLIENT_SUPPRESSIBLE[_VECTOR]
   */
  NPA_RESOURCE_SUPPORTS_SUPPRESSIBLE = 0x00000080,

  /* Invoke the driver function unconditionally for the first request 
     to the resource */
  NPA_RESOURCE_DRIVER_UNCONDITIONAL_FIRST = 0x00000200,

  /* One-shot attribute that will cause the next request to the resource
   * to invoke the driver function, so long as the client is not issuing
   * a redundant request. The attribute is unset by the framework after
   * such a request. Set to the same value as UNCONDITIONAL_FIRST since
   * the latter only applies to the first request to the resource.
   */
  NPA_RESOURCE_DRIVER_UNCONDITIONAL_NEXT = 
                      NPA_RESOURCE_DRIVER_UNCONDITIONAL_FIRST,

  /* Allow NPA scheduler to fork a request to this resource in its LPR */
  NPA_RESOURCE_LPR_ISSUABLE = 0x00000400,

  /* Allows clients to switch type between required and suppressible */
  NPA_RESOURCE_ALLOW_CLIENT_TYPE_CHANGE = 0x00000800,
  
  /* Pre- and post-change events are enabled by default for all resources.
     The below attribute is provided for source-compatibility purposes */
  NPA_RESOURCE_ENABLE_PREPOST_CHANGE_EVENTS = 0x00001000,
  
  /* All IMPULSE requests will call the driver function, irrespective
     of the output of the update function */
  NPA_RESOURCE_IMPULSE_UNCONDITIONAL = 0x00002000,

} npa_resource_attribute;


typedef enum
{
  NPA_NODE_DEFAULT              = 0, /* Use default node behavior */

  /* Don't create a lock for this node. If this attribute is defined, it is
     responsibility of the resource author to manage concurrency */
  NPA_NODE_NO_LOCK              = 0x00000001,

  /* This node can be disabled at runtime */
  NPA_NODE_DISABLEABLE          = 0x00000002,

  /* This node is capable of forking; ignored */
  NPA_NODE_FORKABLE             = 0x00000004,

} npa_node_attribute;


/* Resource author query ids.  All resource author queries ids are in
 * 1024-2047 of the reserved section */
enum
{
  NPA_RESOURCE_AUTHOR_QUERY_START = NPA_MAX_PUBLIC_QUERY,
  /* Get the resource attribute vector */
  NPA_QUERY_RESOURCE_ATTRIBUTES,
  /* Get the node attribute vector */
  NPA_QUERY_NODE_ATTRIBUTES,
  NPA_MAX_RESOURCE_AUTHOR_QUERY = 2047
};

/* Framework request attributes */
enum
{
  NPA_REQUEST_DROP_VOTE = NPA_REQUEST_RESERVED_ATTRIBUTE1,
  
  NPA_REQUEST_REMOTE_CLIENT = NPA_REQUEST_RESERVED_ATTRIBUTE2,
};

/*----------------------------------------------------------------------------
 * Types/Typedefs
 * -------------------------------------------------------------------------*/
/** User data - user defines (and must cast) to whatever they want */
typedef void* npa_user_data;


/** Internal callback structure */
typedef struct npa_event_callback
{
  npa_callback  callback;
  npa_user_data context;
} npa_event_callback;


/** Internal work request structure */
typedef struct npa_work_request
{
  npa_resource_state  state;    /* Valid for all */
  union                         /* Used for reference data */
  {
    npa_resource_state *vector;
    char               *string;
    void               *reference;
  } pointer;
} npa_work_request;


/** npa_client structure definition */
typedef struct npa_client 
{
  struct npa_client            *prev, *next;   /* Linked list */
  const char                   *name;          /* Name of client */
  const char                   *resource_name; /* Name of resource (may be alias) */
  struct npa_resource          *resource;      /* Resource client is linked to */
  npa_user_data                 resource_data; /* Arbitrary resource data */
  npa_client_type               type;          /* Client type */
  npa_work_request              work[3];       /* Use ACTIVE/INCOMING/PENDING_REQUEST
                                                * macros to access */
  unsigned int                  index;         /* Last request serviced */
  unsigned int                  sequence;      /* Sequence number */
  struct npa_async_client_data *async;         /* Async client parameters */
  void                         *log;           /* ULog associated with client */

  unsigned int                  request_attr;  /* Request attributes */
  

  /* Pointer to function used to actually issue requests via this client;
     What this is set to determines whether the client is sync/async */
  void (*issue_request)( npa_client_handle client, int new_request ); 

  /* Scheduler data structures. If NULL, this is either not a scheduleable
     client or the scheduler feature is not enabled */
  struct npa_scheduler_data *request_ptr;
} npa_client;


/* Helper macros for accessing the correct request structure, as they
 * are double buffered
 *
 * TODO: These macros will need to change if the buffering gets
 * greater than double buffered */

/* Access requests being processed */
#define NPA_ACTIVE_REQUEST( client )  ((client)->work[((client)->index)])
#define NPA_PENDING_REQUEST( client ) ((client)->work[((client)->index)^1])

/* Map request for NPA_MAX_STATE into request for the maximum of the
   current resource. Intended for use in plugin functions. */
#define NPA_REMAP_MAX_REQUEST( resource, client )                       \
  NPA_PENDING_REQUEST(client).state =                                   \
    ( NPA_PENDING_REQUEST(client).state == NPA_MAX_STATE ) ?            \
    client->resource->definition->max : NPA_PENDING_REQUEST(client).state


/** Event structure definition */
typedef struct npa_event
{
  struct npa_event          *prev, *next;    /* Linked list */
  unsigned int               trigger_type;   /* Bitfield of one/more enums
                                                from npa_event_trigger_type */

  const char                *name;       /* Event Name */
  struct npa_resource       *resource;   /* Resource event is linked to
                                            (may be NULL for custom events) */
  union 
  {
    npa_resource_state_delta watermark;
    npa_resource_state       threshold;
  } lo;

  union 
  {
    npa_resource_state_delta watermark;
    npa_resource_state       threshold;
  } hi;

  npa_event_callback         callback;       /* Callback */

  /* Registration data for custom events. For a particular event, this is
     defined between the registrant and the entity posting the event */
  void                      *reg_data; 

  /* Internal data structure used to post this event in a no-dups manner;
     Used only for custom events ATM */
  struct npa_event_action   *action; 
} npa_event;


/* Return a pointer to the first event in a resource's event list */
#define NPA_EVENT_FIRST_EVENT( event ) \
  ( event->resource->events.list->reg_events )

/* Return a pointer to the next event in a resource's event list */
#define NPA_EVENT_NEXT_EVENT( event ) ( event->next )

/* Return a event's trigger type */
#define NPA_EVENT_TRIGGER_TYPE( event ) \
  ( event->trigger_type & NPA_TRIGGER_TYPE_MASK )

/* Macro to access the custom registration data field in events */
#define NPA_EVENT_REG_DATA( event ) ( event->reg_data )


typedef struct npa_link
{
  struct npa_link *next, *prev;
  const char      *name;
  struct npa_resource    *resource;
} npa_link;

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


/* Resource latency structure */
typedef struct npa_resource_latency {
  uint32 request;  /* The request latency for the resource */
  uint32 fork;     /* The fork latency for the resource */
  uint32 notify;   /* The Notify latency for the resource */
} npa_resource_latency;
  

/* Query Resource Function
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
 * of the query function to lock the resource
 * 
 */
typedef npa_query_status (*npa_resource_query_fcn)( 
  struct npa_resource *resource, 
  unsigned int         query_id, 
  npa_query_type      *query_result );

/* Query Link Function 
 *
 * This is an alternative optional function to enable the resource to support
 * an extended query API. If defined, it will be invoked for any
 * queries. If the query function does not wish to support the query,
 * it shall return NPA_QUERY_UNSUPPORTED_QUERY_ID.  If the query
 * function returns that the id is unsupported, the framework will
 * attempt to handle the query id.
 *
 * The resource is *not* locked when the query function is invoked.
 * If the query requires atomic processing, it is the responsibility
 * of the query function to lock the resource
 * 
 * This function takes a resource link (an npa_link data structure) as the 
 * first parameter. The resource can use link->name to determine what
 * resource name the query handle was created against. This allows the resource
 * to account for aliases (alternative names to a particular resource) and
 * respond to the query per alias.
 */
typedef npa_query_status (*npa_link_query_fcn)(
  struct npa_link  *resource_link,
  unsigned int      query_id,
  npa_query_type   *query_result );


/* Plugin Definition */
typedef struct npa_resource_plugin
{
  npa_resource_update_fcn  update_fcn;        /* Update function */
  unsigned int             supported_clients; /* Bitmask of supported clients */

  /* Plugin Vtable */

  /* Create Client Function - A new client has been created, allow 
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
   * allow the plugin a chance to do something before the new
   * client is returned to the user.  This function receives whatever
   * optional arguments the user may have passed in. If the third
   * argument is not NULL, the second argument is required to be the 
   * length of the buffer pointed by the third argument. This function
   * should return 0 if the extended creation was successful or a
   * non-zero error code if the extended creation failed.  In the
   * event of failure, the error code will be logged to the NPA log
   * and the user will receive a NULL handle 
   *
   * Note: It is an error for a plugin to define both the Client
   * Create Function and the Extended Client Create Function */
  unsigned int (*create_client_ex_fcn)( npa_client *, unsigned int, void * );


  /* Cancel Client Function - The user has canceled their client
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
  npa_client_handle  handle;        /* Client Handle; is created by NPA */
} npa_node_dependency;


/*
 * @brief Helper macro to access dependency client handles by index from within
 * a node's driver function. 
 *
 *    mynode_example_driver_fn ( npa_resource *resource,
 *                               npa_client_handle    client,
 *                               npa_resource_state   state )
 *   {
 *     npa_pass_request_attributes( client, NPA_DEPENDENCY(resource, 0) );
 *     npa_issue_required_request( NPA_DEPENDENCY(resource, 0), state );
 *     ...
 */
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
  npa_resource_state     max;           /* Max state for scalar resources;
                                           Max length of vector for vectors */
  const npa_resource_plugin *plugin;    /* Plugin */
  unsigned int           attributes;    /* Resource Attributes */
  npa_user_data          data;          /* User data field  */
  npa_resource_query_fcn query_fcn;         /* Query Fcn */
  npa_link_query_fcn     link_query_fcn;    /* Query Link Fcn */

 /* Handle to this resource; the field will be initialized in the framework
    when the node is created. Do not define or define as NULL */
  struct npa_resource   *handle;       
} npa_resource_definition;


/* Get the resource associated with a given node, given the current resource  */
#define NPA_RESOURCE( resource, index ) \
  ((resource)->node->resources[(index)].handle)


/* Node Definition Structure
 *
 * This is created by the Node author.  This can be constant */
typedef struct npa_node_definition
{
  const char                *name;         /* Node name */
  npa_resource_driver_fcn    driver_fcn;   /* Driver function */
  unsigned int               attributes;   /* Node Attributes */
  npa_user_data              data;         /* For resource author use */
  unsigned int               dependency_count; /* Number of dependencies */
  npa_node_dependency       *dependencies;     /* Array of dependencies */
  unsigned int               resource_count;   /* Number of resources */
  npa_resource_definition   *resources;        /* Array of resources */
} npa_node_definition;


/* Internal node structure */
typedef struct npa_resource
{
  /* Resource definition, as provided by the resource author */
  npa_resource_definition  *definition; 

  /* Node definition, as provided by the resource author */
  npa_node_definition      *node;

  /* Index of the resource within its node */
  unsigned int              index;

  npa_client               *clients;           /* client list */

  /* Events */
  union
  {
    npa_event *creation_events;   /* For resource_available callbacks */
    struct npa_event_list *list;  /* For all other events */
  } events;

  /* Points to the plugin defining the resource's current aggregation
   * scheme, supported client types, custom query routines etc. Can be
   * changed dynamically using the npa_change_resource_plugin API
   */
  const npa_resource_plugin *active_plugin;

  /** State fields for scalar resources; these may be undefined for vectors */

  /* Overall aggregation */
  npa_resource_state request_state;

  /* Active state of resource */
  npa_resource_state active_state;

  /* Internal state fields; NPA assumes the first four to be the required,
     suppressible, suppressible2 and semiactive aggregations for scalars */
  npa_resource_state internal_state[8];

  /** State fields for vector resources; these are NULL for scalars */

  /* For vector resources, NPA allocates a vector of "npa_resource_state"s of
     size resource->definition->max to this pointer */
  npa_resource_state       *state_vector;

  /* Aggregation of REQUIRED vector requests. For vector resources that
   * support suppressible clients, NPA allocates a vector of the same size
   * as state_vector to this pointer.
   *
   * For RPM-remoted vector resources, this field is written into the
   * sleep set. Access via NPA_RESOURCE_REQUIRED_VECTOR(resource).
   */
  npa_resource_state       *required_state_vector;

  /* Vector resources that have a SUPPRESSIBLE dependency on another resource
   * may ask NPA to allocate a vector to store their suppressible aggregation
   * by specifying the NPA_RESOURCE_ALLOC_EXTRA_STATE_VECTORS attribute.
   * Access via NPA_RESOURCE_SUPPRESSIBLE_VECTOR(resource).
   */
  npa_resource_state       *suppressible_state_vector;

  /* Vector resources that have a SUPPRESSIBLE2 dependency on another resource
   * may ask NPA to allocate a vector to store their suppressible aggregation
   * by specifying the NPA_RESOURCE_ALLOC_EXTRA_STATE_VECTORS attribute.
   * Access via NPA_RESOURCE_SUPPRESSIBLE2_VECTOR(resource).
   */
  npa_resource_state       *suppressible2_state_vector;

  /* Aggregation of all REQUIRED and SUPPRESSIBLE vector requests. For vector
   * resources that support clients of type NPA_CLIENT_SUPPRESSIBLE2_VECTOR,
   * NPA allocates a vector of the same size as state_vector to this pointer.
   *
   * For RPM remoted vector resources, this vector is written into the
   * semi-active set. Access via NPA_RESOURCE_SEMI_ACTIVE_VECTOR(resource).
   */
  npa_resource_state       *semiactive_state_vector;

  npa_resource_state        active_max;        /* The current max */
  npa_resource_state_delta  active_headroom;   /* The current headroom */

  /* Locks */
  struct CoreMutex         *node_lock;         /* Node lock */
  struct CoreMutex         *event_lock;        /* Lock for events */

  /* NPA internal data - not for resource use */
  struct npa_resource_internal_data *_internal;

  /* Sequence number, used to track order of request processing */
  unsigned int sequence;

  /* Ulog handle associated with the resource; usually, "NPA Log" */
  void *log;

  /* For resources subject to scheduled requests, this structure
     contains the request, fork and notify latencies */
  npa_resource_latency     *latency;
} npa_resource;


/* Dummy declaration for SC purposes; code no longer uses join tokens */
typedef void* npa_join_token;


/*----------------------------------------------------------------------------
 * NPA Resource API
 * -------------------------------------------------------------------------*/

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
 * all the dependencies exist, the resource will be constructed,
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
 * unneeded. If invoked, the callback arguments are (context, 0,
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
                         void                *context);



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
void npa_alias_resource_cb( const char   *resource_name, 
                            const char   *alias_name,
                            npa_callback  alias_cb,
                            void         *context);

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
void npa_define_marker( const char *marker_name );


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
                                        npa_resource_attribute  attributes );


/**
 * <!-- npa_stub_resource -->
 *
 * @brief Define an stub resource
 *
 * Stub resources are used to satisfy dependencies, similar to
 * markers, but they allow clients to be created, requests issued,
 * etc.  A stub resource does not do anything with client requests,
 * and will always have a state of 0
 *
 * Currently it is an error to redefine a resource defined as a stub
 * resource, either as another stub or as a real resource.  It is
 * hoped that future versions of the framework will allow stub
 * resources to be redefined as real resources.
 * 
 * @param stub_name : The name of the marker to create.  The string
 *        is not copied, so the pointer must point to a static string.
 */ 
#define npa_stub_resource( stub_name ) \
  npa_alias_resource_cb( "/dev/null", stub_name, NULL, NULL );


/*======================================================================
  Author API

  These are special purpose functions intended for use by the node
  author.  
  ======================================================================*/

/** 
 * <!-- npa_issue_dependency_request -->
 *
 * @brief Issue a request to a dependency
 *
 * This function may be used by a resource to issue a request to its
 * dependency. It takes the current client to the resource as one of its
 * parameters and issues a required request, a suppressible request or both
 * on the appropriate dependency client handles, based on the current client's
 * type and request attributes.
 *
 * @param cur_client : The current client to the resource that is invoking this
 *                     routine
 * @param req_client : The required client handle to the resource's 
 * dependency. If there is no required client, pass NULL.
 * @param req_state : A required request to place on the dependency
 * @param sup_client : The suppressible client handle to the resource's 
 * dependency. If there is no suppressible client, pass NULL
 * @param sup_state : A suppressible request to place on the dependency
 */
void npa_issue_dependency_request( npa_client_handle cur_client,
                                   npa_client_handle req_client,
                                   npa_resource_state req_state,
                                   npa_client_handle sup_client,
                                   npa_resource_state sup_state );


/** 
 * <!-- npa_issue_dependency_vector_request -->
 *
 * @brief Issue a vector request to a dependency 
 *
 * This function may be used by a resource to issue a request to its
 * dependency. It takes the current client to the resource as one of its
 * parameters and issues a required request, a suppressible request or both
 * on the appropriate dependency client handles, based on the current client's
 * type and request attributes.
 *
 * @param cur_client : The current client to the resource that is invoking this
 *                     routine
 * @param req_client : The required client handle to the resource's 
 * dependency. If there is no required client, pass NULL.
 * @param req_num_elems : Number of elements in the required vector request
 * @param req_vector : A required vector request to place on the dependency
 * @param sup_client : The suppressible client handle to the resource's 
 * dependency. If there is no suppressible client, pass NULL
 * @param sup_num_elems : Number of elements in the suppressible vector request
 * @param sup_vector : A suppressible vector request to place on the dependency
 */
void npa_issue_dependency_vector_request( npa_client_handle cur_client,
                                          npa_client_handle req_client,
                                          unsigned int req_num_elems,
                                          npa_resource_state *req_vector,
                                          npa_client_handle sup_client,
                                          unsigned int sup_num_elems,
                                          npa_resource_state *sup_vector );


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
 * can be retrieved for coupled resources from the current resource
 * by the NPA_RESOURCE macro.
 *
 * @param resource : Resource whose state is changing
 * @param state : New state of the resource
 */
void npa_assign_resource_state( npa_resource       *resource,
                                npa_resource_state  state );


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
npa_resource *npa_query_get_resource( npa_query_handle query_handle ); 


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
void npa_enable_node( npa_node_definition *node, npa_resource_state default_state[] );


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
void npa_disable_node( npa_node_definition *node ); 


/**
 * <!-- npa_fork_resource -->
 *
 * @brief Stubbed. Retained only for source compatibility with client code.
 *
 * @return Always NULL.
 */
npa_join_token npa_fork_resource( npa_resource       *resource,
                                  npa_join_function   join_func,
                                  void               *join_data );


/**
 * <!-- npa_resource_lock -->
 *
 * @brief Lock the resource.
 *
 * If the resource is forked, it will be joined before this function returns.
 * Note that the resource shares its lock with other resources in the node.
 * So this will, in effect, lock the node.
 *
 * @param resource : Handle to the resource
 */
void npa_resource_lock( npa_resource *resource );

/**
 * <!-- npa_resource_trylock -->
 *
 * @brief Try to lock the resource. If another task already has the lock,
 * the routine returns immediately without waiting to acquire it.
 *
 * If the lock was acquired, the routine returns 0. Else, -1.
 *
 * @param resource : Handle to the resource
 *
 * @return 0 if a lock was acquired, -1 otherwise.
 */
int npa_resource_trylock( npa_resource *resource );

/**
 * <!-- npa_resource_unlock -->
 *
 * @brief  Unlock the resource that was previously locked by
 * npa_resource_lock.
 *   
 * @param resource : Handle to the resource
 */
void npa_resource_unlock( npa_resource *resource );


/**
 * <!-- npa_request_has_attribute -->
 *
 * @brief Returns true if the given attribute is set on the client handle.
 * 
 * Request attributes are set on client handles and are valid for the duration
 * of the current request.
 *
 * @param client : The client handle
 * @param attr : The attribute to test for
 *
 * @return TRUE if the attribute is set on the client handle
 */
unsigned int npa_request_has_attribute( npa_client_handle     client,
                                        npa_request_attribute attr );


/**
 * <!-- npa_get_request_attributes -->
 *
 * @brief Returns a bitset of all request attributes set on the given client.
 *
 * @param client : The client handle
 */
unsigned int npa_get_request_attributes( npa_client_handle client );


/**
 * <!-- npa_pass_request_attributes -->
 *
 * @brief Add request attributes of the 'current' client to the 'dependency' client.
 *
 * Does not unset any attributes set previously on the 'dependency' client.
 *
 * @param current : Handle of a npa_client whose attributes will be applied
 *                  to the other.
 * @param dependency : Handle of a npa_client whose attributes will be updated. 
 *
 * @return The dependency client handle passed in
 */
npa_client_handle npa_pass_request_attributes( npa_client_handle current,
                                               npa_client_handle dependency );


/** 
 * <!-- npa_unset_request_attribute -->
 *
 * @brief Resource authors may call this routine from their driver functions
 * to explicitly unset a request attributes from a client. This prevents
 * actions such as "next request passthrough" that the framework may setup
 * based on the presence of some attributes, after return from the driver.
 */
void npa_unset_request_attribute( npa_client_handle     client, 
                                  npa_request_attribute attr );


/*======================================================================
  Plugin API
  ======================================================================*/
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
                                 const npa_resource_plugin *plugin );


/*======================================================================
  Standard Plugins

  These plugins are intended for common cases. If there is not one
  sufficient for the resource author's needs, they should create a new
  plugin.

  The plugins typically use the first two elements of the resource's
  internal state array. The binary_plugin/update function uses all
  4 elements of the internal state array.
  ======================================================================*/

/* Resource state is either max value or off(0) if there is atleast one active
   non-zero request or not, respectively */
extern const npa_resource_plugin npa_binary_plugin;

/* Resource state is the max of active requests */
extern const npa_resource_plugin npa_max_plugin;

/* Resource state is the min of active requests */
extern const npa_resource_plugin npa_min_plugin;

/* Resource state is the sum of active requests */
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

/* Resource state is the bitwise OR of active requests */
extern const npa_resource_plugin npa_or_plugin;

/* Resource state is the bitwise AND of active requests, with each request
   reduced to 0 or 1, depending on whether it was non-zero or zero */
extern const npa_resource_plugin npa_binary_and_plugin;

/* Resource supports no clients - used for markers */
extern const npa_resource_plugin npa_no_client_plugin;

/* Update functions used by the above plugins.
 * Resource authors who define custom client_create/destroy functions
 * will need to create their own plugins, but can use these routines
 * as their update functions.
 */

/** Min update */
npa_resource_state npa_min_update_fcn( npa_resource      *resource,
                                       npa_client_handle  client);

/** Max update */
npa_resource_state npa_max_update_fcn( npa_resource      *resource,
                                       npa_client_handle  client );

/** Sum update */
npa_resource_state npa_sum_update_fcn( npa_resource      *resource,
                                       npa_client_handle  client );
                                       
/** Binary update - see comment against plugin above */
npa_resource_state npa_binary_update_fcn( npa_resource      *resource,
                                          npa_client_handle  client );
                                          
/** Bitwise OR update */
npa_resource_state npa_or_update_fcn( npa_resource       *resource,
                                      npa_client_handle   client);
                                      
/** Binary AND update - see comment against plugin above */
npa_resource_state npa_binary_and_update_fcn( npa_resource       *resource,
                                              npa_client_handle   client);
                                      
/** Identity update */
npa_resource_state npa_identity_update_fcn( npa_resource      *resource,
                                            npa_client_handle  client );
                                     
/** Always On update */
npa_resource_state npa_always_on_update_fcn( npa_resource      *resource,
                                             npa_client_handle  client );
                                             
/** Impulse update */
npa_resource_state npa_impulse_update_fcn( npa_resource      *resource,
                                           npa_client_handle  client );
  
/* Helper macros to access the various aggregations for scalar resources */
#define NPA_REQUIRED_INDEX      0
#define NPA_SUPPRESSIBLE_INDEX  1
#define NPA_SUPPRESSIBLE2_INDEX 2
#define NPA_SEMIACTIVE_INDEX    3
     
#define NPA_RESOURCE_REQUIRED_REQUEST( resource ) \
  MIN( resource->internal_state[NPA_REQUIRED_INDEX], resource->active_max )

#define NPA_RESOURCE_SUPPRESSIBLE_REQUEST( resource ) \
  MIN( resource->internal_state[NPA_SUPPRESSIBLE_INDEX], resource->active_max )

#define NPA_RESOURCE_SUPPRESSIBLE2_REQUEST( resource ) \
  MIN( resource->internal_state[NPA_SUPPRESSIBLE2_INDEX], resource->active_max )

#define NPA_RESOURCE_SEMIACTIVE_REQUEST( resource ) \
  MIN( resource->internal_state[NPA_SEMIACTIVE_INDEX], resource->active_max )

/* Helper macros to access various aggregations for vector resources */
#define NPA_RESOURCE_REQUIRED_VECTOR( resource ) \
  resource->required_state_vector

#define NPA_RESOURCE_SUPPRESSIBLE_VECTOR( resource ) \
  resource->suppressible_state_vector

#define NPA_RESOURCE_SUPPRESSIBLE2_VECTOR( resource ) \
  resource->suppressible2_state_vector

#define NPA_RESOURCE_SEMIACTIVE_VECTOR( resource ) \
  resource->semi_active_state_vector
  
/*======================================================================
  INTERNAL client API
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
void npa_issue_internal_request( npa_client_handle client );

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
 * @return NPA_SUCCESS on success, NPA_ERROR otherwise.
 */
npa_status npa_resource_add_system_event_callback( const char   *resource_name,
                                                   npa_callback  callback,
                                                   void         *context );


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
 * @return NPA_SUCCESS on success, NPA_ERROR otherwise.
 */
npa_status npa_resource_remove_system_event_callback( const char   *resource_name,
                                                      npa_callback  callback,
                                                      void         *context );

/**
 * <!-- npa_post_custom_event -->
 *
 * @brief Post a custom event to NPA's event queue. The callback associated
 * with this event with be called (dispatched) with the given event_data
 * from the event workloop. Multiple calls to this function will translate
 * to multiple invocations of the callback.
 *
 * @param event : The custom event
 * @param type : Type of custom event that occurred
 * @param data : Event data associated with this particular post
 */
void npa_post_custom_event( npa_event_handle event, 
                            npa_event_type type, void *event_data );


/**
 * <!-- npa_post_custom_event_nodups -->
 *
 * @brief Post a custom event to NPA's event queue. If the same event
 * was previously queued, but not yet dispatched, simply update the
 * event_data associated with the event with the given.
 * In effect, this "collapses" multiple instances of the same event
 * with the callback only being triggered once with the latest
 * event_data. Note that a single invocation is not guaranteed
 * as the previous instance of the event may already have been
 * dispatched before this post.
 *
 * @param event : The custom event to either post afresh or update in the
 * dispatch queue
 * @param type : Type of custom event that occurred
 * @param data : Event data to post or update the event with.
 */
void npa_post_custom_event_nodups( npa_event_handle event, 
                                   npa_event_type type, void *data );


/**
 * <!-- npa_post_custom_events -->
 *
 * @brief Post all custom events of the given type registered with
 * the given resource, with the given event_data. 
 *
 * @param resource : Resource whose event registration list will be
 * perused for custom events to post
 * @param type : Type of custom event that occurred
 * @param data : Event data associated with this particular post
 */
void npa_post_custom_events( npa_resource  *resource, 
                             npa_event_type type, void *event_data );

/**
 * <!-- npa_dispatch_custom_event -->
 *
 * @brief Dispatch or invoke the callback of a custom event with the
 * given event_data. This routine will invoke the callback inline (i.e.
 * in context of the calling thread, not from the event workloop).
 * Since callbacks may take an arbitrary amount of time to execute,
 * resource authors must use this API with due care.
 *
 * @param event : The custom event
 * @param type : Type of custom event that occurred
 * @param data : Event data to dispatch the event with
 */
void npa_dispatch_custom_event( npa_event_handle event,
                                npa_event_type type, void *data );

/**
 * <!-- npa_dispatch_custom_events -->
 *
 * @brief Dispatch all custom events of the given type registered with
 * the given resource, with the given event_data. 
 * This routine will invoke the callbacks inline (i.e. in context of the
 * calling thread, not from the event workloop). Since callbacks may take
 * an arbitrary amount of time to execute, resource authors must use
 * this API with due care.
 *
 * @param resource : Resource whose event registration list will be
 * perused for custom events to dispatch
 * @param type : Type of custom event that occurred
 * @param data : Event data to dispatch the event with
 */
void npa_dispatch_custom_events( npa_resource *resource, 
                                 npa_event_type type, void *data );

/**
 * <!-- npa_get_first_event_of_trigger_type -->
 *
 * @brief Get the first event registered with the given resource with
 * the given trigger type. trigger_type may include multiple trigger types
 * (bitwise ORd) and the API will return the first event that matches
 * any of these.
 *  
 * @param resource : Resource whose event list is to be perused
 * @param trigger_type : Bitwise OR of trigger types of interest
 *
 * @return The first event handle in resource whose trigger_type is included
 * in the given trigger_type parameter. NULL if there is no match.
 */
npa_event_handle
npa_get_first_event_of_trigger_type( npa_resource *resource, 
                                     unsigned int  trigger_type );

/**
 * <!-- npa_get_next_event_of_trigger_type -->
 *
 * @brief Get the next event from the given event with the given trigger type.
 * This takes an event handle as parameter and may be used (in a loop) after
 * get_first_event_of_trigger_type to retrieve subsequent events registered
 * for the given trigger_type with the resource.
 *  
 * @param event : The event in the resource's event list to look from; the
 * first event looked at is "event->next"
 * @param trigger_type : Bitwise OR of trigger types of interest
 *
 * @return The first event handle after "event" whose trigger_type is included
 * in the given trigger_type parameter. NULL if there is no match.
 */
npa_event_handle
npa_get_next_event_of_trigger_type( npa_event_handle event, 
                                    unsigned int     trigger_type );

/**
 * <!-- npa_dispatch_pre_change_events -->
 *
 * @brief Dispatches pre state change events registered with the resource.
 * This routine will invoke callbacks inline (i.e. in context of the calling
 * thread, not the event workloop). Since callbacks may take an arbitrary
 * amount of time to execute, resource authors must use this API with care.
 * Callbacks must also be careful to not cause recursions or deadlocks by
 * issuing problematic requests from within.
 *
 * @param resource   : Resource whose event registration list will be
 * perused for pre change events to dispatch
 * @param from_state : Current (pre-change) resource state
 * @param to_state   : New state we are transitioning to
 * @param data       : Custom event data from whoever calls this routine
 */
void npa_dispatch_pre_change_events( npa_resource       *resource,
                                     npa_resource_state  from_state,
                                     npa_resource_state  to_state,
                                     void               *data );


/**
 * <!-- npa_dispatch_post_change_events -->
 *
 * @brief Dispatches post change events registered with the resource.
 * This routine will invoke callbacks inline (i.e. in context of the calling
 * thread, not the event workloop). Since callbacks may take an arbitrary
 * amount of time to execute, resource authors must use this API with care.
 * Callbacks must also be careful not to cause recursions or deadlocks by
 * issuing problematic requests from within.
 *
 * @param resource   : Resource whose event registration list will be
 * perused for post change events to dispatch
 * @param event_type : The type of event to dispatch
 * @param from_state : Previous resource state
 * @param to_state   : Current (post-change) resource state
 * @param data       : Custom event data from whoever calls this routine
 */
void npa_dispatch_post_change_events( npa_resource       *resource,
                                      npa_event_type      event_type,
                                      npa_resource_state  from_state,
                                      npa_resource_state  to_state,
                                      void               *data );

/**
 * <!-- npa_resource_has_attribute -->
 *
 * @brief Returns TRUE if the resource has the given attribute. If more than
 * one attribute (ORd together) is provided, returns TRUE if the resource
 * has any of those attributes.
 */
unsigned int
npa_resource_has_attribute( npa_resource *resource, unsigned int attribute );

/**
 * <!-- npa_set_resource_attribute -->
 *
 * @brief Sets the given attribute (or attributes if more than one is
 * ORd together) for the resource
 */
void
npa_set_resource_attribute( npa_resource *resource, unsigned int attribute );


/* Legacy API - invoke callback functions with dal_event callback.
 *
 * These calls are deprecated and should not be used by new code
 */
/**
 * <!-- npa_define_node_dal -->
 *
 * @brief  Define an NPA node
 *
 * Define a new node.  Simply adding the definition of a new node may
 * not be sufficient to create the node and all of its associated
 * resources and add it to the resource graph.  In order to construct
 * the node/resources, all of the nodes' dependencies must exist. When
 * all the dependencies exist, the resource will be constructed,
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
 * The define_event will be triggered after the node is fully
 * constructed.  The define_event can be NULL if creation
 * notification is unneeded.
 *
 * If the event is a callback event, the arguments will be 
 * (user_data, 0, node->name, sizeof(const char *)).
 *
 * @param node : Definition of node to be created 
 * @param initial_state : State vector to initialize the resources with 
 * @param dal_event : Optional dal_event to be triggered upon
 *      successful creation of the node.  The arguments will be (
 *      context, 0, *alias_name, strlen(alias_name) )
 *
 * @note npa_define_node_cb is the prefered API for new code
 */
#define npa_define_node_dal( a, b, dal_event ) \
  npa_define_node_cb( a, b, NPA_DAL_CALLBACK(dal_event) )


/**
 * <!-- npa_define_node -->
 *
 * @brief Compatibility wrapper for npa_define_node_dal. 
 *
 * See npa_define_node_dal for API details.
 *
 * @note npa_define_node_cb is the preferred API for new code
 */
#define npa_define_node    npa_define_node_dal


/**
 * <!-- npa_alias_resource_dal -->
 *
 * @brief Create an alias for an existing resource.  This allows the same resource to be accessed with multiple names.
 *
 * The alias doesn't exist until alias_event is triggered
 *
 * An error fatal will occur if there is already a resource with the
 * requested alias name
 *
 * @param resource_name : The name of the resource to be linked
 * @param alias_name : The alias for the resource
 * @param dal_event : Optional event to be triggered upon successful
 *     alias creation of the alias.  
 * @param context : User supplied context argument
 *
 * @note  Legacy API - deprecated and should not be used by new code
 */
#define npa_alias_resource_dal( a, b, dal_event ) \
  npa_alias_resource_cb( a, b, NPA_DAL_CALLBACK(dal_event) ) 


/**
 * <!-- npa_define_node -->
 *
 * @brief Compatibility wrapper for npa_alias_resoruce_dal
 *
 * @note Legacy API - deprecated and should not be used by new code
 */
#define npa_alias_resource npa_alias_resource_dal

#ifdef __cplusplus
}
#endif

#endif /* NPA_RESOURCE_H */

