/*============================================================================
@file npa_awake_until.c

Implements the NPA Awake Until resource for users to specify when they
need to be "awake until". Also implements the "/core/cpu/busy" resource for
users to specify that they expect to do work (1) or not (0). 

Copyright (c) 2009-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR

$Header: //components/rel/core.qdsp6/2.1.c4/power/npa/src/scheduler/npa_awake_until.c#1 $
============================================================================*/

#include "awake_until.h"
#include "npa_scheduler_internal.h"
#include "DDITimetick.h"

/* Function Prototypes */
static npa_resource_state
awake_until_request_driver( npa_resource *resource, npa_client_handle client,
                            npa_resource_state state);

static npa_query_status
awake_until_query( npa_resource *resource, unsigned int query_id, 
                   npa_query_type *query_result );

static npa_resource_state 
awake_until_update_fcn( npa_resource *resource, npa_client_handle client );

static unsigned int awake_until_client_create( npa_client *client, 
                                               unsigned int client_value,
                                               void *client_ref);

static void awake_until_client_destroy( npa_client *client );

static npa_resource_state busy_request_driver( npa_resource       *resource,
                                               npa_client         *client,
                                               npa_resource_state  state );

/* Types */
typedef struct
{
  DalDeviceHandle *timer; /* 64 bit timer and signal that awake_until initialized */
  DalTimetickTime64Type active_state;
} awake_until_control;


/* Control information for the awake_until feature */
static awake_until_control awake_until;

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

static const npa_resource_plugin awake_until_plugin = 
{
  awake_until_update_fcn,
  /* Supported client types */
  NPA_CLIENT_REQUIRED, 
  NULL,                          /* Create client function */
  awake_until_client_destroy,    /* Destroy client function */
  awake_until_client_create      /* Extended Client Create Function */
};

/* awake until resource and node definition
 * These resource will keep track of the latest "awake until"
 * value reported by the clients.
 *
 * It is unconditional since the value is wrapable so 
 * the max value can actually be a smaller value.
 */
static npa_resource_definition awake_until_resource[] = 
{ 
  {
    AWAKE_UNTIL_RESOURCE_NAME, /* Name */
    "T_TICK",                  /* Units */
    0xffffffff,                /* Max - full range of 32 bit clock */
    &awake_until_plugin,       /* Plugin */
    NPA_RESOURCE_DEFAULT 
    | NPA_RESOURCE_DRIVER_UNCONDITIONAL, /* Attributes */
    (npa_user_data) &awake_until, /* data - point to control */
    awake_until_query,            /* query */
    NULL                          /* handle - filled in on create */
  }
};


static npa_node_definition awake_until_node = 
{ 
  "/core/cpu/awake_until",
  awake_until_request_driver, 
  NPA_NODE_DEFAULT,             /* Just make sure the lock doesn't get created */
  (npa_user_data) NULL,
  NPA_EMPTY_ARRAY,
  NPA_ARRAY(awake_until_resource)
};


/* /core/cpu/busy definition */
static npa_resource_definition busy_resource[] = 
{ 
  {
    "/core/cpu/busy",         /* Name */
    "BINARY",                 /* Units */
    1,                        /* Binary */
    &npa_binary_plugin,       /* Plugin */
    NPA_RESOURCE_DEFAULT,     /* Attributes */
    NULL                      /* data - point to control */
  }
};

static npa_node_definition busy_node = 
{ 
  "/core/cpu/busy_node",
  busy_request_driver,
  NPA_NODE_DEFAULT,             /* Just make sure the lock doesn't get created */
  (npa_user_data) NULL,
  NPA_EMPTY_ARRAY,
  NPA_ARRAY(busy_resource)
};

 /*--------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/**
 * <!-- busy_request_driver -->
 *
 * @brief Driver function for the /core/cpu/busy_node node. 
 *
 * @param resource : Will be the "/core/cpu/awake_until" resource
 * @param client : The client making the request
 * @param request : The request state passed into this driver
 *
 * @return The request state passed into this driver. 
 */
static npa_resource_state 
  awake_until_request_driver( struct npa_resource *resource,
                                  npa_client_handle    client,
                                  npa_resource_state   request)
{
  /* Deprecated */
  return(request);
}


/**
 * <!-- awake_until_query -->
 *
 * @brief awake_until query handler 
 */
static npa_query_status 
  awake_until_query( struct npa_resource *resource,
                     unsigned int query_id, 
                     npa_query_type *query_result )
{
  return NPA_QUERY_NO_VALUE;
}


/**
 * <!-- awake_until_update_fcn -->
 *
 * @brief Resource update function
 *
 * No action taken on the update, just return the pending request.
 *
 * @param resource : Resource the request is for.
 * @param client : Handle of client making the request
 *
 * @return Returns the current/updated awake until time.
 */
static npa_resource_state 
  awake_until_update_fcn( npa_resource  *resource,
                          npa_client_handle client )
{
  npa_resource_state request = NPA_PENDING_REQUEST(client).state;
  return(request);
}


/** 
 * <!-- awake_until_client_create -->
 *
 * @brief Create enough memory to store a 64 bit TTICK value 
 *
 * @param client : The client to add the 64 bit memory pointer to.
 * @param client_value : Not used
 * @param client_ref :  Not used
 */
static unsigned int awake_until_client_create(npa_client *client, 
                                              unsigned int client_value,
                                              void *client_ref)
{
  /* create memory for resource information for this client */
  client->resource_data = Core_Malloc( sizeof(uint64) );
  CORE_VERIFY_PTR( client->resource_data );
  *((uint64 *)(client->resource_data)) = 0;
  return(0);
}


/**
 * <!-- awake_until_client_destroy -->
 *
 * @brief Free up memory allocated by awake_until_client_create() 
 *
 * @param client : A client that had previously been passed to awake_until_client_create
 */
static void awake_until_client_destroy(npa_client *client)
{
  Core_Free( client->resource_data );
  client->resource_data = NULL;
}


/**
 * <!-- busy_request_driver -->
 *
 * @brief Driver function for the /core/cpu/busy_node node. 
 *
 * @param resource : Will be the /core/cpu/busy resource
 * @param : The client making the request
 * @param : The state passed into this driver function.
 *
 * @return The state passed to this driver function. 
 */
static npa_resource_state busy_request_driver( npa_resource       *resource,
                                               npa_client         *client,
                                               npa_resource_state  state )
{
  return state;
}


/*======================================================================
 * Public Functions
 *======================================================================*/
/**
 * <!-- awake_until_init -->
 *
 * @brief Initialize the awake_until_node and busy_node and define the 
 * awake_until marker.
 *
 * Creates the needed resources, nodes, and data structures
 * to support the awake_until feature.
 */
void awake_until_init(void)
{
  if (NULL == awake_until.timer)
  { /* not initialized yet */
    ULOG_RT_PRINTF_0(npa_scheduler.log,"awake_until_init");

    /* initial state should be 0 time */
    npa_define_node( &awake_until_node, NULL, NULL );

    /* initial state should be 0 time */
    npa_define_node( &busy_node, NULL, NULL );

    /* let world know we are up */
    npa_define_marker("/init/awake_until");
  }
}
