/*=============================================================================

FILE:      ul_arb.c

DESCRIPTION: This file implements the NPA ICB Arbiter Node

    Copyright (c) 2010-2017 QUALCOMM Technologies, Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
-------------------------------------------------------------------------------

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/ICBArbLib/ul_arb.c#1 $
  $DateTime: 2017/09/18 09:26:13 $
  $Author: pwbldsvc $
  $Change: 14402555 $
                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

 when        who     what, where, why
----------   ---     -----------------------------------------------------------
2017/03/16   sds     ALC changed to MAX aggregation
2017/03/08   sds     Change BCM-as-clock to BCM-as-standalone generic framework
2017/02/24   sds     Add support for BCM-as-clock implementation
2016/11/08   sds     Add BCM query functions
2016/10/27   sds     Only create NPA node if we successfully initialize AxiCfg.
2016/10/21   sds     Remove dependency on rpm protocol.
2016/01/21   sds     Branch for Hoya from 2.0.
=============================================================================*/

/*============================================================================
                          INCLUDES
============================================================================*/
#include "icbarb.h"
#include "ul_i.h"
#include "DALSys.h"
#include "DALStdDef.h"
#include "npa_resource.h"
#include "ULogFront.h"
#include "ddiaxicfgint.h"
#include "string.h"

/*============================================================================
                          DEFINES
============================================================================*/
#define MICROSECONDS_PER_SECOND  1000000
#define PERCENTS_IN_A_WHOLE      100
#define ICBARB_REQUEST_TYPE_MASK 0xFFFF
#define ICBARB_REQUEST_FLAG_MASK 0xFFFF0000

#define FAL_REQUEST_INVALID      0xFFFFFFFF
#define FAL_INVALID              0xFF

/*============================================================================
                          MACROS 
============================================================================*/
#define ALIGN8( x )     ( ((x)+7) & ~7 )

/*============================================================================
                          LOCAL TYPES
============================================================================*/
typedef struct
{
   uint32_t              num_pairs;
   icb_route_type *      routes;
   icb_bw_request_type * requests;
   icb_bw_request_type * query_requests;
} icb_vector_type;

typedef struct
{
   ICBArb_FALType req;
   uint32_t       alc;
} icb_fal_type;

/*============================================================================
                          LOG DATA
============================================================================*/
/* ICB Arbiter Internal Log Handle */
ULogHandle icb_log;

/* Size of Arbiter Log - Defaults to 0 so that logging is disabled */
/* Change this to 8k or 16k to enable internal logging */
uint32_t icb_log_size = 0;

/*============================================================================
                          STATIC DATA
============================================================================*/
/* To pass errors that occur in ul_issue_pair_request() */
static ICBArb_ErrorType error_state = ICBARB_ERROR_SUCCESS;  

/* *******************************************************************
 * NPA Data Definitions
 * *******************************************************************/
static npa_resource *self = NULL;

static npa_resource_state _ul_driver_fcn
(
   npa_resource *resource,
   npa_client   *client,
   npa_resource_state state
);

/*============================================================================
  NPA Definitions for Standalone BCMs
============================================================================*/
static npa_resource_state _ul_standalone_driver_fcn
(
   npa_resource *resource,
   npa_client   *client,
   npa_resource_state state
);

static npa_resource_state _ul_fal_update_fcn
(
   npa_resource      *resource,
   npa_client_handle  client
);

static npa_resource_state _ul_fal_driver_fcn
(
   npa_resource *resource,
   npa_client   *client,
   npa_resource_state state
);

static unsigned int _ul_fal_create_client_ex
(
   npa_client * client, 
   unsigned int data, 
   void *       reference
);

static void _ul_fal_destroy_client
(
   npa_client *client
);

static npa_resource_plugin ul_fal_plugin = 
{
   _ul_fal_update_fcn,
   NPA_CLIENT_VECTOR | NPA_CLIENT_SUPPRESSIBLE_VECTOR,
   NULL,  /* using the ex version of create */
   _ul_fal_destroy_client,
   _ul_fal_create_client_ex,
   NULL
};

/*============================================================================
               STATIC FUNCTION DECLARATIONS
============================================================================*/
static bool _convert_request_to_internal(ICBArb_RequestType *request, icb_bw_request_type *internal);
static ICBArb_ErrorType _query_slave(ICBArb_QueryTypeType query_type, ICBArb_QueryUnionType *query_data);
static ICBArb_ErrorType _query_bcm_buffer_size(ICBArb_QueryBCMType *query);
static ICBArb_ErrorType _query_bcm_request(ICBArb_QueryBCMType *query);
static ICBArb_ErrorType _query_bcm_free_buffer(ICBArb_QueryBCMType *query);

/*============================================================================
                          STATIC FUNCTIONS
============================================================================*/

/**=============================================================================
  _ul_init_cb

  @brief Performs post ICB arbiter node creation initialization

  @param None

  @return None
==============================================================================*/
static void _ul_init_cb
(
   void         *context,
   unsigned int  event_type,
   void         *data,
   unsigned int  data_size
)
{
   /* Unused parameters */
   (void)event_type;
   (void)data;
   (void)data_size;

   /* Tell AxiCfg to init the hardware. */
   AxiCfgInt_Reset();

   /* Save our handle to ourselves */
   self = ((npa_node_definition *)context)->resources[0].handle;
}

/*==============================================================================

  FUNCTION   _ul_driver_fcn

  DESCRIPTION 
    Empty function -- all request actions handled by update function below.

  PARAMETERS
    resource - Resource information on this node
    client   - Client making the request
    state    - the state

  RETURN VALUE    
    The state that was passed into the function

==============================================================================*/
static npa_resource_state _ul_driver_fcn
(
   npa_resource *     resource,
   npa_client   *     client,
   npa_resource_state state
) 
{
   /* Unused parameters */
   (void)client;
   (void)state;

   /* Currently the only state passed by update is NORMAL so just return
   ** the current state.
   */
   return resource->active_state;
}


/*==============================================================================

  FUNCTION   _ul_update_fcn

  DESCRIPTION 
    Passes along the client request to the driver function

  PARAMETERS
    resource - Resource information on this node
    client   - Client making the request

  RETURN VALUE    
    The requested flow

==============================================================================*/
static npa_resource_state _ul_update_fcn
(
   npa_resource      *resource,
   npa_client_handle  client
)
{
   uint32_t i, j;
   icb_route_type      *route;
   icb_bw_request_type *request;
   bool is_complete_request = false;
   icb_topology_type *topology = 
      (icb_topology_type *)(resource->definition->data);
   icb_vector_type *master_route_vector = 
      (icb_vector_type *)(client->resource_data);
   uint64_t request_len = (uint64_t)(NPA_PENDING_REQUEST( client ).state) * sizeof(npa_resource_state)/ 
                       sizeof(ICBArb_RequestType);
   ICBArb_RequestType *pending_request  = 
      (ICBArb_RequestType *)(NPA_PENDING_REQUEST( client ).pointer.vector);

   icb_bw_request_type bw, old_request;
   bw.client = client;

   /* Check for 0 length vector, which means reset all bandwidths to zero */
   if ( 0 == request_len )
   {
      is_complete_request = true;
      request_len = master_route_vector->num_pairs;
   }

   /* Loop through master clients and issue requests */
   for ( i = 0; i < request_len; i++ )
   {
      if ( !is_complete_request )
      {
         /* Convert units to internal Ib & Ab */
         if ( !_convert_request_to_internal( &pending_request[i], &bw ) )
         {
            /* Invalid request type or attempted divide-by-zero due to
               zero time interval */

            /* We need to undo all the requests so far so there are no
             * remnants of the failed request */
            is_complete_request = true;
            i = 0;
            error_state = ICBARB_ERROR_INVALID_ARG;
            continue;
         }
      }
      else
      {
         /* Set BW to zero if CompleteRequest */
         bw.ib         = 0;
         bw.ab         = 0;
         bw.latency_ns = 0;
      }
      
      route = &master_route_vector->routes[i];

      /* Copy the old request. */
      old_request = master_route_vector->requests[i];

      /* Store the new request */
      request = &master_route_vector->requests[i];
      request->ib          = bw.ib;
      request->ab          = bw.ab;
      request->latency_ns  = bw.latency_ns;

      /* Issue the request over all hops. */
      for( j = 0;
           j < route->route_list->num_pairs;
           j++ )
      {
         ul_issue_pair_request(
            client,
           &bw,
           &old_request,
           &route->route_list->pairs[j]);
      }
   }

   /* "commit" all queued reqeusts */
   ul_request_commit( topology, client->request_attr );

   return 0;
}

/*==============================================================================

  FUNCTION   _ul_create_client_ex

  DESCRIPTION 
    Do client-specific initialization

  PARAMETERS
    client - The new client

  RETURN VALUE    
    None

==============================================================================*/
static unsigned int _ul_create_client_ex
(
   npa_client * client, 
   unsigned int data, 
   void *       reference
)
{
   uint32_t i, j, error;
   uint32_t num_pairs;
   ICBArb_MasterSlaveType *pair;
   icb_route_type *routes = NULL;
   icb_bw_request_type *requests = NULL, *query_requests = NULL;
   unsigned int resource_index = client->resource->index;
   
   pair = (ICBArb_MasterSlaveType *)reference;
   num_pairs = data / sizeof(ICBArb_MasterSlaveType);
   
   /* Allocate client vector of master client handles */
   if( DAL_SUCCESS != 
       DALSYS_Malloc( ALIGN8( sizeof(icb_vector_type) ) + 
                      ALIGN8( num_pairs * sizeof(icb_route_type) ) + 
                      ALIGN8( num_pairs * sizeof(icb_bw_request_type) ) +
                      ALIGN8( num_pairs * sizeof(icb_bw_request_type) ),
                      (void **)&( client->resource_data ) ) )
   {
      client->resource_data = NULL;
      error = ICBARB_ERROR_OUT_OF_MEMORY;
      goto ul_create_client_ex_error;
   }

   DALSYS_memset( client->resource_data,
                  0,
                  ALIGN8( sizeof(icb_vector_type) ) +
                  ALIGN8( num_pairs * sizeof(icb_route_type) ) +
                  ALIGN8( num_pairs * sizeof(icb_bw_request_type) ) +
                  ALIGN8( num_pairs * sizeof(icb_bw_request_type) ) );

   routes = (icb_route_type *)((uint8*)client->resource_data + ALIGN8( sizeof(icb_vector_type) ) );
   requests = (icb_bw_request_type *)( (uint8*)routes + ALIGN8( num_pairs * sizeof(icb_route_type) ) );
   query_requests = (icb_bw_request_type *)( (uint8*)requests + ALIGN8( num_pairs * sizeof(icb_bw_request_type) ) );

   /* Store away vector length */
   ((icb_vector_type *)
      (client->resource_data))->num_pairs = num_pairs; 
   /* Store away master client array */
   ((icb_vector_type *)
      (client->resource_data))->routes = routes;
   ((icb_vector_type *)
      (client->resource_data))->requests = requests;
   ((icb_vector_type *)
      (client->resource_data))->query_requests = query_requests;

   for ( i = 0; i < num_pairs; i++  )
   {
      /* Get Routes */
      routes[i].master_id  = pair[i].eMaster;
      routes[i].slave_id   = pair[i].eSlave;
      routes[i].route_list =
         ul_get_route( resource_index, routes[i].master_id, routes[i].slave_id );

      if ( NULL == routes[i].route_list )
      {
         error = (uint32_t)ICBARB_ERROR_NO_ROUTE_TO_SLAVE;
         goto ul_create_client_ex_error;
      }

      for( j = 0; j < routes[i].route_list->num_pairs; j++ )
      {
         requests[i].client = client;

         /* Add the request to each node's queue */
         ul_add_bw_request(
           &requests[i],
           &routes[i].route_list->pairs[j].slave->request_list );
         ul_add_bw_request(
           &requests[i],
           &routes[i].route_list->pairs[j].master->request_list );
      }
   }

   return 0;  /* Success */

ul_create_client_ex_error:
   /* Need to remove requests from slaves */
   if ( client->resource_data )
   {
      for ( i = 0; i < num_pairs; i++ )
      {
         if ( NULL != routes[i].route_list )
         {
            for ( j = 0; j < routes[i].route_list->num_pairs; j++ )
            {
               ul_remove_bw_request(
                  &requests[i],
                  &routes[i].route_list->pairs[j].slave->request_list);
               ul_remove_bw_request(
                  &requests[i],
                  &routes[i].route_list->pairs[j].master->request_list);
            }
         }
      }

      DALSYS_Free(client->resource_data);
      client->resource_data = NULL;
   }
   return error;
}

/*==============================================================================

  FUNCTION   _ul_destroy_client

  DESCRIPTION 
    Do client-specific cleanup

  PARAMETERS
    client - The client to destroy

  RETURN VALUE    
    None

==============================================================================*/
static void _ul_destroy_client( npa_client *client )
{
   uint32_t i,j;
   icb_vector_type *master_route_vector = 
      (icb_vector_type *)(client->resource_data);

   if ( master_route_vector )
   {
      /* Need to remove requests from slaves */
      if ( master_route_vector->requests )
      {
          for ( i = 0; i < master_route_vector->num_pairs; i++ )
          {
            if ( NULL != master_route_vector->routes[i].route_list )
            {
                for ( j = 0; j < master_route_vector->routes[i].route_list->num_pairs; j++ )
                {
                  ul_remove_bw_request(
                    &master_route_vector->requests[i],
                    &master_route_vector->routes[i].route_list->pairs[j].slave->request_list);
                  ul_remove_bw_request(
                    &master_route_vector->requests[i],
                    &master_route_vector->routes[i].route_list->pairs[j].master->request_list);
                }
            }
          }
      }

      /* Free the memory from the client */
      DALSYS_Free( master_route_vector );
   }
   
   client->resource_data = NULL;
   return;
}

static npa_resource_plugin ul_plugin = 
{
   _ul_update_fcn,
   NPA_CLIENT_VECTOR | NPA_CLIENT_SUPPRESSIBLE_VECTOR,
   NULL,  /* using the ex version of create */
   _ul_destroy_client,
   _ul_create_client_ex,
   NULL
};

/*============================================================================
               NPA ICB Arbiter API Functions
============================================================================*/

/*==============================================================================

  FUNCTION   icbarb_init

  DESCRIPTION
    Initialize and define the ICB arbiter node.

  PARAMETERS
    void

  RETURN VALUE    
    void

==============================================================================*/
void icbarb_init( void )
{
   unsigned int init[1] = {0,};

   /* Initialize AxiCfg layer. */
   if( AxiCfgInt_Init() )
   {
      icb_info_type *      icb_info;
      npa_node_definition *ul_node_defn;
      uint32_t             i;

      /* Initialize the Arbiter Internal Log */
      ULogFront_RealTimeInit(&icb_log,
                             "ICB Arb Log",
                             icb_log_size,
                             ULOG_MEMORY_LOCAL,
                             ULOG_LOCK_NONE);

      /* Initialize internal data structures, bail if bad info. */
      icb_info = ul_int_init();
      if( NULL == icb_info )
      {
         return;
      }
      ul_node_defn = icb_info->node_definition;

      /* Create NPA node if we were able to fetch the data. */
      if( NULL != ul_node_defn )
      {
         /* First, fill in our function pointers and plugin. */
         ul_node_defn->driver_fcn = _ul_driver_fcn;
         for( i = 0; i < ul_node_defn->resource_count; i++ )
         {
            ul_node_defn->resources[i].plugin = &ul_plugin;
         }

         /* Define the ICB Arbiter Node */
         npa_define_node_cb( ul_node_defn, init, _ul_init_cb, ul_node_defn );
      }

      /* Create NPA nodes for any standalone BCMs. */
      for( i = 0; i < icb_info->num_standalone_nodes; i++ )
      {
         icb_standalone_node_type *node = icb_info->standalone_node_definitions[i];
         icb_standalone_data_type *data = (icb_standalone_data_type *)node->node_definition.resources[0].data;

         /* First, fill in our function pointers and plugin. */
         switch( data->type )
         {
            case ICB_STANDALONE_INTERFACE_CLOCK:
               node->node_definition.driver_fcn          = _ul_standalone_driver_fcn;
               node->node_definition.resources[0].plugin = &npa_max_plugin;
               break;
            case ICB_STANDALONE_INTERFACE_FAL:
               node->node_definition.driver_fcn          = _ul_fal_driver_fcn;
               node->node_definition.resources[0].plugin = &ul_fal_plugin;
               break;
            default:
               continue;
         }

         /* Define clock node. */
         npa_define_node_cb( &node->node_definition, init, NULL, NULL );
      }
   }
}

/*==============================================================================

  FUNCTION   icbarb_post_init

  DESCRIPTION
    Flush out initial requests made during subsystem initialization and set
    the arbiter to run mode.

  PARAMETERS
    void

  RETURN VALUE    
    void

==============================================================================*/
void icbarb_post_init( void )
{
   /* If our node hasn't been created, skip this. */
   if( NULL != self )
   {
      /* Lock our NPA node to prevent concurrent access to internal structures. */
      npa_resource_lock(self);

      /* Prod internal layer to do post init step. */
      ul_int_post_init();

      /* Unlock now that we are done. */
      npa_resource_unlock(self);
   }
}

/*==============================================================================

  FUNCTION   icbarb_issue_request

  DESCRIPTION
     Allows a client to apply a vector of arbitration settings to the routes
     already established by a previous icbarb_create_client() call.
     The vector is a set of unions of arbitration settings of Types 1 and 2
     above (and more types in the future, as needed.)
 
     Note that the number of elements in the vector must match
     the number of master/slave pairs in the previous icbarb_create_client()
     call for that client.  If the number of elements in the
     icbarb_issue_request() call does not match, then the function
     returns an error (ICBARB_ERROR_VECTOR_LENGTH_MISMATCH.)

  PARAMETERS
     client - Client making the request
     requests - Array of Arbitration info
     num_requests - number of elements
 
  RETURN VALUE
     The result of the request (e.g. success or an error code)
 
  SIDE EFFECTS
     If the result of the request is ICBARB_ERROR_REQUEST_REJECTED, then the
     request is set to zero (similar to the icbarb_complete_request()
     call below.)  This is because there is the possibility of the driver
     getting into an inconsistent state if this kind of cleanup is not done.

==============================================================================*/
ICBArb_ErrorType icbarb_issue_request
(
   npa_client_handle   client, 
   ICBArb_RequestType *requests,  
   uint32              num_requests
)
{
   ICBArb_ErrorType error;

   icb_vector_type *master_route_vector;

   /* Check input parameters */
   if( NULL == client || NULL == requests )
   {
     return ICBARB_ERROR_INVALID_ARG;
   }

   master_route_vector = (icb_vector_type *)(client->resource_data);

   /* Check master client vector info. */
   if( NULL == master_route_vector )
   {
     return ICBARB_ERROR_INVALID_ARG;
   }

   /* Check for vector length match */
   if ( num_requests != master_route_vector->num_pairs )
   {
      return ICBARB_ERROR_VECTOR_LENGTH_MISMATCH;
   }

   npa_issue_vector_request( client, 
                             (num_requests * sizeof(ICBArb_RequestType))/sizeof(npa_resource_state),
                             (npa_resource_state *)requests );

   if ( ICBARB_ERROR_SUCCESS != error_state )
   {
      error = error_state;
      error_state = ICBARB_ERROR_SUCCESS;
      return error;
   }

  return ICBARB_ERROR_SUCCESS;
}

/*==============================================================================

  FUNCTION   icbarb_complete_request

  DESCRIPTION
    The effect of this function is the same as a icbarb_issue_request() call
    where the vector elements are all 0.

  PARAMETERS
     client - Client making the request
 
  RETURN VALUE
     void

==============================================================================*/
void icbarb_complete_request( npa_client_handle client )
{
   npa_complete_request( client );

   return;
}


/*==============================================================================

  FUNCTION   icbarb_destroy_client

  DESCRIPTION
     Sets all the routes to 0 request, deletes the route information,
     and frees the associated memory.
 
     This will cancel the client request (if any) and free the client
     structure. The client handle can not be used to issue requests after
     being destroyed.

  PARAMETERS
     client - Client making the request
 
  RETURN VALUE
     void

==============================================================================*/
void icbarb_destroy_client( npa_client_handle client )
{
   /* Destroy the client */
   npa_destroy_client( client );

   return;
}

/*============================================================================
                             HELPER FUNCTIONS
============================================================================*/
ICBArb_CreateClientVectorType *icbarb_fill_client_vector
(
  ICBArb_CreateClientVectorType *psVector,
  ICBArb_MasterSlaveType        *aMasterSlave,
  npa_callback                   callback
)
{
  (void)psVector;
  (void)callback;

  return (void *)aMasterSlave;
}

/**=============================================================================

   @brief icbarb_create_client - Creates a new ICB arbiter client

   DEPRICATED : Use native NPA functions with ICBARB_CREATE_CLIENT_VECTOR macro instead.
   Allows a client to set up a vector of master/slave pair that then
   processed to determine the internal routing parameters, which are stored
   and associated with the client.

   @param client_name : name of the client that's being created
   @param pairs : Array of Master/Slave pairs
   @param num_pairs : number of elements in aMasterSlave
 
   @return A handle to the created NPA client, NULL if the client
           creation failed

==============================================================================*/
npa_client_handle icbarb_create_client
(
   const char             *client_name,
   ICBArb_MasterSlaveType *pairs, 
   uint32                  num_pairs
)
{
   return npa_create_sync_client_ex( "/icb/arbiter", 
                                     client_name,
                                     NPA_CLIENT_VECTOR, 
                                     num_pairs * sizeof(ICBArb_MasterSlaveType),
                                     (void *)pairs );
}

/**=============================================================================

   @brief icbarb_create_client_ex - Creates a new ICB arbiter client with a
                                     callback function for state change
                                     notification (oversubscription notification)

   DEPRICATED : Use native NPA functions with ICBARB_CREATE_CLIENT_VECTOR macro instead.
   Allows a client to set up a vector of master/slave pair that then
   processed to determine the internal routing parameters, which are stored
   and associated with the client.  An additional argument allows registration
   for state change notification, which allows the client to be notified 
   when the fabrics are oversubscribed.

   @param client_name : name of the client that's being created
   @param pairs : Array of Master/Slave pairs
   @param num_pairs : number of elements in aMasterSlave
   @param callback: Oversubscription notification callback function pointer
 
   @return A handle to the created NPA client, NULL if the client
           creation failed

==============================================================================*/
npa_client_handle icbarb_create_client_ex
(
   const char             *client_name,
   ICBArb_MasterSlaveType *pairs, 
   uint32                  num_pairs, 
   npa_callback            callback
)
{
   (void)callback;

   return npa_create_sync_client_ex( "/icb/arbiter", 
                                     client_name,
                                     NPA_CLIENT_VECTOR, 
                                     num_pairs * sizeof(ICBArb_MasterSlaveType),
                                     (void *)pairs );
}


/**=============================================================================

   @brief icbarb_create_suppressible_client_ex -
                                     Creates a new ICB arbiter client with a
                                     callback function for state change
                                     notification (oversubscription notification)

   DEPRICATED : Use native NPA functions with ICBARB_CREATE_CLIENT_VECTOR macro instead.
   Allows a client to set up a vector of master/slave pair that then
   processed to determine the internal routing parameters, which are stored
   and associated with the client.  An additional argument allows registration
   for state change notification, which allows the client to be notified 
   when the fabrics are oversubscribed.  All clock requests from this client
   will be made with suppressible requests so that the clock requests may be
   removed when the originating processor enters low power mode.

   @param client_name : name of the client that's being created
   @param aMasterSlave : Array of Master/Slave pairs
   @param num_pairs : number of elements in aMasterSlave
   @param callback: Oversubscription notification callback function pointer
 
   @return A handle to the created NPA client, NULL if the client
           creation failed

==============================================================================*/
npa_client_handle icbarb_create_suppressible_client_ex
(
   const char             *client_name,
   ICBArb_MasterSlaveType *pairs, 
   uint32                  num_pairs, 
   npa_callback            callback
)
{
   (void)callback;

   return npa_create_sync_client_ex( "/icb/arbiter", 
                                     client_name,
                                     NPA_CLIENT_SUPPRESSIBLE_VECTOR, 
                                     num_pairs * sizeof(ICBArb_MasterSlaveType),
                                     (void *)pairs );
}

/**=============================================================================

   @brief icbarb_query - Queries ICB parameters
   
   Provides access to ICB internal data.
   
   @param query : Pointer to query specification
   
   @return The result of the query (e.g. success or an error code)

==============================================================================*/
ICBArb_ErrorType icbarb_query( ICBArb_QueryType *query )
{
   ICBArb_ErrorType error = ICBARB_ERROR_REQUEST_REJECTED;

   /* Make sure the arbiter node actually came up. */
   if( NULL != self )
   {
      /* Check input parameters. */
      if ( NULL != query )
      {
         /* Differentiate into query type. */
         switch ( query->queryType )
         {
            case ICBARB_QUERY_RESOURCE_COUNT:
            case ICBARB_QUERY_RESOURCE_NAMES:
               error = ICBARB_ERROR_UNSUPPORTED;
               break;

            case ICBARB_QUERY_SLAVE_PORT_COUNT:
               error = _query_slave(
                  query->queryType,
                  &query->queryData );
               break;

            case ICBARB_QUERY_BCM_BUFFER_SIZE:
               error = _query_bcm_buffer_size( &query->queryData.bcmRequest );
               break;

            case ICBARB_QUERY_BCM_REQUEST:
               error = _query_bcm_request( &query->queryData.bcmRequest );
               break;

            case ICBARB_QUERY_BCM_REQUEST_FREE:
               error = _query_bcm_free_buffer( &query->queryData.bcmRequest );
               break;

            default:
               error = ICBARB_ERROR_INVALID_ARG;
               break;
         }
      }
      else
      {
         error = ICBARB_ERROR_INVALID_ARG;
      }
   }

   return error;
}

/*============================================================================
                          STATIC HELPER FUNCTIONS
============================================================================*/

/*============================================================================

  FUNCTION   _convert_request_to_internal

  DESCRIPTION
     Converts the units of various types of input requests to the internal
     Ib & Ab representation.

  PARAMETERS
     request  (in)  - Pointer to the input external request struct
     PInternal (out) - Pointer to the output internal request struct.  The
                       memory for this structure must be allocated by the
                       calling function.
 
  RETURN VALUE
     true if successful
     false if there is an unsupported request type, or if any time parameter
           is zero with nonzero data burst.
============================================================================*/
static bool _convert_request_to_internal
(
   ICBArb_RequestType * request, 
   icb_bw_request_type *internal )
{
   /* Clear the internal structure */
   memset(internal, 0, sizeof(icb_bw_request_type));

   /* Scan request type information. */
   switch ( request->arbType & ICBARB_REQUEST_TYPE_MASK )
   {
   case ICBARB_REQUEST_TYPE_1:
      if ( ( 0 == request->arbData.type1.uTransferTimeUs ) || 
           ( 0 == request->arbData.type1.uPeriodUs ) )
      {
         /* Protect against divide-by-zero */
         return false;
      }
      /* Only calculate if the data burst size is greater than zero. */
      else if ( 0 < request->arbData.type1.uDataBurst )
      {
         /* All operands are nonzero.  Round-up divisions to protect against
          * underflow.
          * Formulae:  Ib = (db + tt  - 1) / tt * 1000000
          *            Ab = (db + per - 1) / per * 1000000
          */
         internal->ib = ( request->arbData.type1.uDataBurst + 
                          request->arbData.type1.uTransferTimeUs - 1 ) / 
                          request->arbData.type1.uTransferTimeUs *
                          MICROSECONDS_PER_SECOND;
         internal->ab = ( request->arbData.type1.uDataBurst + 
                          request->arbData.type1.uPeriodUs - 1 ) / 
                          request->arbData.type1.uPeriodUs *
                          MICROSECONDS_PER_SECOND;
      }
      /* Check latency flag */
      if( request->arbType & ICBARB_REQUEST_FLAG_LATENCY )
      {
         internal->latency_ns = request->arbData.type1.uLatencyNs;
      }
      break;
   case ICBARB_REQUEST_TYPE_2:
      /* Round-up division to protect against underflow 
       * Formula:  Ab = ( throughput + 100 - 1 ) / 100 * percent_usage
       */
      internal->ib = request->arbData.type2.uThroughPut;
      internal->ab = ( request->arbData.type2.uThroughPut + 
                       PERCENTS_IN_A_WHOLE - 1 ) / 
                       PERCENTS_IN_A_WHOLE  *
                       request->arbData.type2.uUsagePercentage;
      /* Check latency flag */
      if( request->arbType & ICBARB_REQUEST_FLAG_LATENCY )
      {
         internal->latency_ns = request->arbData.type2.uLatencyNs;
      }
      break;
   case ICBARB_REQUEST_TYPE_3:
      internal->ib = request->arbData.type3.uIb;
      internal->ab = request->arbData.type3.uAb; 
      /* Check latency flag */
      if( request->arbType & ICBARB_REQUEST_FLAG_LATENCY )
      {
         internal->latency_ns = request->arbData.type3.uLatencyNs;
      }
      break;
   default:
      return false;
   }

   return true;  // Success
}

/**=============================================================================

   @brief _query_slave - Queries ICB slave resource parameters
   
   This function queries various parameters from the target slave.

   @param query_type : The type of query
   @param queryData : Pointer to query data storage
   
   @return The result of the query (e.g. success or an error code)

==============================================================================*/
static ICBArb_ErrorType _query_slave
(
  ICBArb_QueryTypeType   query_type,
  ICBArb_QueryUnionType *query_data
)
{
  ul_int_slave_list_type slave_list;
  uint32_t i;
  ICBArb_ErrorType error = ICBARB_ERROR_UNKNOWN;

  switch( query_type )
  {
    case ICBARB_QUERY_SLAVE_PORT_COUNT:
      ul_target_get_slave_list( &slave_list );

      /* Find the slave we care about. */
      for( i = 0; i < slave_list.num_slaves; i++ )
      {
        if( slave_list.slaves[i]->id ==
            query_data->slavePortCount.eSlaveId )
        {
          break;
        }
      }

      /* If we've found it, update query data. */
      if( i != slave_list.num_slaves )
      {
        query_data->slavePortCount.uNumPorts =
          slave_list.slaves[i]->num_agg_ports;
        error = ICBARB_ERROR_SUCCESS;
      }
      break;
    default:
      error = ICBARB_ERROR_INVALID_ARG;
      break;
  }

  return error;
}

/** _generate_query
 *
 *   @brief Generate a query request to the internal engine.
 *   
 *   @param[inout] query -            Pointer to query data storage
 *   @param[inout] num_command_sets - Pointer to place to store the number of commands
 *   
 *   @return The result of the query (e.g. success or an error code)
 */
static ICBArb_ErrorType _generate_query
(
   ICBArb_QueryBCMType *query, 
   uint32_t *           num_command_sets
)
{
   ICBArb_ErrorType        error = ICBARB_ERROR_SUCCESS;
   icb_topology_type *     topology = (icb_topology_type *)query->client->resource->definition->data;
   icb_bw_request_type *   request;
   icb_vector_type *       route_vector = (icb_vector_type *)(query->client->resource_data);
   icb_route_type *        route;
   uint32_t                num_requests = ((icb_vector_type *)(query->client->resource_data))->num_pairs;
   uint32_t                route_idx, pair_idx;

   /* Make sure the request length matches the client
    * if the request is not for current state. */
   if( NULL != query->requests && num_requests != query->num_requests )
   {
      return ICBARB_ERROR_INVALID_ARG;
   }

   /* Issue query requests to the engine. */
   for( route_idx = 0; route_idx < num_requests; route_idx++ )
   {
      /* If the client is looking for current state, set the 
       * bandwidth to pass to NULL. */
      if( NULL == query->requests )
      {
         request = NULL;
      }
      /* Otherwise, convert their requested bandwidth to our internal type. */
      else
      {
         icb_bw_request_type * query_requests =
            ((icb_vector_type *)(query->client->resource_data))->query_requests;

         /* Get the query request buffer for this index. */
         request = &query_requests[route_idx];

         /* Convert units to internal Ib & Ab */
         if( !_convert_request_to_internal( &query->requests[route_idx], request ) )
         {
            /* Invalid request type or attempted divide-by-zero due to
               zero time interval */
            error = ICBARB_ERROR_INVALID_ARG;
            break;
         }
         request->client = query->client;
      }

      route = &route_vector->routes[route_idx];
      for( pair_idx = 0; 
           pair_idx < route->route_list->num_pairs;
           pair_idx ++ )
      {
         ul_issue_query_request( request, &route->route_list->pairs[pair_idx] );
      }
   }

   /* Finally, fetch the number of command sets queued.
    * Skip this if there was an error. */
   if( ICBARB_ERROR_SUCCESS == error )
   {
      /* Commit the query. */
      ul_query_commit();

      *num_command_sets = ul_fetch_hw_queue_length(topology);
   }
   /* In the case of an error, force to zero.
    * Caller will clean up by default to prevent redundant cleanup. */
   else
   {
      *num_command_sets = 0;
   }

   return error;
}

/** _flush_query
 * 
 * @brief Flush all results from previous queries
 * 
 */
void _flush_query
(
   ICBArb_QueryBCMType *query
)
{
   icb_topology_type *topology =
      (icb_topology_type *)query->client->resource->definition->data;
   icb_bw_request_type * query_requests =
      ((icb_vector_type *)(query->client->resource_data))->query_requests;
   uint32_t num_requests = 
      ((icb_vector_type *)(query->client->resource_data))->num_pairs;
   uint32_t i;

   /* Instruct internal layer to flush the topology our query used. */
   ul_flush_query(topology);

   /* Clear query request. */
   for( i = 0; i < num_requests; i++ )
   {
      query_requests[i].ib         = 0;
      query_requests[i].ab         = 0;
      query_requests[i].latency_ns = 0;
   }
}

/** _query_bcm_buffer_size
 *
 *   @brief Queries number of BCMs required for a given request
 *   
 *   @param[inout] query - Pointer to query data storage
 *   
 *   @return The result of the query (e.g. success or an error code)
 */
static ICBArb_ErrorType _query_bcm_buffer_size
(
   ICBArb_QueryBCMType *query
)
{
   ICBArb_ErrorType error = ICBARB_ERROR_INVALID_ARG;

   /* Verify requesting client and proceed. */
   if( NULL != query->client )
   {
      uint32_t num_command_sets;

      /* Lock the node before we generate commands. */
      npa_resource_lock(self);

      error = _generate_query( query, &num_command_sets );
      query->num_command_sets = num_command_sets;

      /* Flush the generated results, since we only want the size. */
      _flush_query(query);

      /* Unlock now that we're done. */
      npa_resource_unlock(self);
   }

   return error;
}

/** _query_bcm_request
 *
 *   @brief Queries the BCM command sets for a given request
 *   
 *   @param[inout] query -     Pointer to query data storage
 *   
 *   @return The result of the query (e.g. success or an error code)
 */
static ICBArb_ErrorType _query_bcm_request
(
   ICBArb_QueryBCMType *query
)
{
   uint32_t num_command_sets;
   ICBArb_ErrorType error = ICBARB_ERROR_INVALID_ARG;
   icb_topology_type *topology =
      (icb_topology_type *)query->client->resource->definition->data;
   
   /* Lock the node before we generate commands. */
   npa_resource_lock(self);

   /* Generate our command sets. */
   error = _generate_query( query, &num_command_sets );

   /* If we successfully generated the query set, check to see
    * if we need to allocate output storage for the command sets. */
   if( ICBARB_ERROR_SUCCESS == error )
   {
      /* Allocate output storage? */
      if( NULL == query->command_sets )
      {
         if ( DAL_SUCCESS != DALSYS_Malloc( 
                  num_command_sets * sizeof(rpmh_command_set_t),
                  (void *)&query->command_sets ) )
         {
            query->num_command_sets = 0;
            error = ICBARB_ERROR_OUT_OF_MEMORY;
         }
         else
         {
            query->num_command_sets = num_command_sets;
         }
      }
      /* Client has preallocated. Make sure they've allocated enough. */
      else if( num_command_sets > query->num_command_sets )
      {
         error = ICBARB_ERROR_VECTOR_LENGTH_MISMATCH;
      }
   }

   /* Fetch the request and flush any state changes. */
   if( error == ICBARB_ERROR_SUCCESS )
   {
      query->num_command_sets = ul_fetch_hw_queue( topology,
                                                   query->command_sets, 
                                                   query->num_command_sets );
   }
   _flush_query(query);

   /* Unlock now that we're done. */
   npa_resource_unlock(self);
   return error;
}

/** _query_bcm_free_buffer
 *
 *   @brief Free a previously allocated BCM command set buffer.
 *   
 *   @param[inout] query - Pointer to query data storage
 *   
 *   @return The result of the operation (e.g. success or an error code)
 */
static ICBArb_ErrorType _query_bcm_free_buffer
(
   ICBArb_QueryBCMType *query
)
{
   ICBArb_ErrorType error = ICBARB_ERROR_INVALID_ARG;

   if( query->command_sets != NULL )
   {
      DALSYS_Free((void *)query->command_sets);
      query->num_command_sets = 0;
      query->command_sets     = NULL;
      error = ICBARB_ERROR_SUCCESS;
   }

   return error;
}

/*============================================================================
                          STANDALONE BCM FUNCTIONS
============================================================================*/
static npa_resource_state _ul_standalone_driver_fcn
(
   npa_resource *     resource,
   npa_client *       client,
   npa_resource_state state
)
{
   /* Issue this singleton request. */
   ul_issue_resource_request(client, resource);

   return state;
}

static unsigned int _ul_fal_create_client_ex
(
   npa_client * client, 
   unsigned int data, 
   void *       reference
)
{
   (void)data;
   (void)reference;

   /* Allocate space for vector storage. */
   if( DAL_SUCCESS == 
       DALSYS_Malloc( ALIGN8( sizeof(icb_fal_type) ),
                      (void **)&( client->resource_data ) ) )
   {
      icb_fal_type *request         = (icb_fal_type *)client->resource_data;
      request->req.fal_tolerance_ns = FAL_REQUEST_INVALID;
      request->req.idle_time_ns     = FAL_REQUEST_INVALID;
      request->alc                  = FAL_INVALID;
   }
   /* Malloc failure. */
   else
   {
      /* Signal client creation failure. */
      client->resource_data = NULL;
   }

   return 0;
}

static void _ul_fal_destroy_client
(
   npa_client *client
)
{
   /* Clean up request storage. */
   if( client->resource_data )
   {
      DALSYS_Free( client->resource_data );
   }
}

static void ul_fal_update_request
(
   icb_fal_type *  request,
   ICBArb_FALType *pending_request
)
{
   /* Verify pending request buffer.
    * If not NULL, copy the buffer.
    * Otherwise, this must be a complete request. */
   if( NULL != pending_request )
   {
      request->req = *pending_request;
   }
   /* Complete request. Reset to INVALID request. */
   else
   {
      request->req.fal_tolerance_ns = FAL_REQUEST_INVALID;
      request->req.idle_time_ns     = FAL_REQUEST_INVALID;
   }

   /* Check for completed/just created clients and mark with our
    * "do not consider" sentinel. */
   if( FAL_REQUEST_INVALID == request->req.fal_tolerance_ns &&
       FAL_REQUEST_INVALID == request->req.idle_time_ns )
   {
      request->alc = FAL_INVALID;
   }
   {
      /* TODO: update with table lookup */
      request->alc = 15;
   }
}

static npa_resource_state _ul_fal_update_fcn
(
   npa_resource      *resource,
   npa_client_handle  client
)
{
   uint32_t request_len            = NPA_PENDING_REQUEST( client ).state;
   ICBArb_FALType *pending_request = (ICBArb_FALType *)NPA_PENDING_REQUEST( client ).pointer.vector;
   icb_fal_type *request           = (icb_fal_type *)client->resource_data;
   uint32_t states[2]              = { FAL_INVALID, FAL_INVALID };
   npa_resource_state state        = resource->active_state;

   /* Make sure the incoming buffer is the right size,
    * but also check for a complete... */
   if( 0 == request_len )
   {
      request->req.fal_tolerance_ns = FAL_REQUEST_INVALID;
      request->req.idle_time_ns     = FAL_REQUEST_INVALID;
      request_len                   = sizeof(ICBArb_FALType)/sizeof(npa_resource_state);
   }
   if( (sizeof(ICBArb_FALType)/sizeof(npa_resource_state)) == request_len )
   {
      npa_client_handle c = resource->clients;

      /* Update this client's request. */
      ul_fal_update_request(request, pending_request);

      /* Find our new active state. */
      while( c )
      {
         request = (icb_fal_type *)c->resource_data;

         /* Skip clients with the invalid sentinel set. */
         if( request->alc != FAL_INVALID )
         {
            switch( c->type )
            {
               case NPA_CLIENT_VECTOR:
                  states[NPA_REQUIRED_INDEX] = MAX( states[NPA_REQUIRED_INDEX],
                                                    request->alc );
                  break;
               case NPA_CLIENT_SUPPRESSIBLE_VECTOR:
                  states[NPA_SUPPRESSIBLE_INDEX] = MAX( states[NPA_SUPPRESSIBLE_INDEX],
                                                        request->alc );
                  break;
               default:
                  break;
            }
         }

         c = c->next;
      }

      /* If nothing is valid, use the default value. */
      if( FAL_INVALID == states[NPA_REQUIRED_INDEX] &&
          FAL_INVALID == states[NPA_SUPPRESSIBLE_INDEX] )
      {
         /* TODO: Use a real default from cmddb */
         state                          = 15;
         states[NPA_SUPPRESSIBLE_INDEX] = 15;
         states[NPA_REQUIRED_INDEX]     = 0;
      }
      /* If both are valid, use the MAX of both. */
      else if( FAL_INVALID != states[NPA_REQUIRED_INDEX] &&
               FAL_INVALID != states[NPA_SUPPRESSIBLE_INDEX] )
      {
         state = MAX( states[NPA_REQUIRED_INDEX], states[NPA_SUPPRESSIBLE_INDEX] );
      }
      /* If REQUIRED is invalid, we can safely use SUPPRESSIBLE here
       * since we know that they cannot both be invalid at this point. */
      else if( FAL_INVALID == states[NPA_REQUIRED_INDEX] )
      {
         state                      = states[NPA_SUPPRESSIBLE_INDEX];
         states[NPA_REQUIRED_INDEX] = 0;
      }
      /* SUPPRESSIBLE must be invalid, use REQUIRED. */
      else
      {
         state                          = states[NPA_REQUIRED_INDEX];
         states[NPA_SUPPRESSIBLE_INDEX] = states[NPA_REQUIRED_INDEX];
      }

      /* Update internal state. */
      resource->internal_state[NPA_REQUIRED_INDEX]     = states[NPA_REQUIRED_INDEX];
      resource->internal_state[NPA_SUPPRESSIBLE_INDEX] = states[NPA_SUPPRESSIBLE_INDEX];
   }

   return state;
}

static npa_resource_state _ul_fal_driver_fcn
(
   npa_resource *     resource,
   npa_client *       client,
   npa_resource_state state
)
{
   npa_resource_state new_state = state;

   /* Check for initialization to fix our state. */
   if( NPA_CLIENT_INITIALIZE == client->type )
   {
      /* TODO: Use a real default from cmddb */
      resource->internal_state[NPA_REQUIRED_INDEX]     = 0;
      resource->internal_state[NPA_SUPPRESSIBLE_INDEX] = 15;
      new_state                                        = 15;
   }

   /* Issue this singleton request. */
   ul_issue_resource_request(client, resource);

   return new_state;
}

/*============================================================================
                          NOTIFICATION INTERFACE
============================================================================*/
/** ul_insert_notification
 *
 * @brief Insert a notification into the given list.
 */
void ul_insert_notification
(
   icb_notify_list_type *list,
   icb_notify_type *     elem
)
{
   /* Check input parameters. */
   if( NULL == list || NULL == elem)
   {
      return;
   }

   /* Insert at the head of the list. */
   elem->next = list->head;
   list->head = elem;
   list->count += 1;
}

/** ul_remove_notification
 *
 * @brief Remove a notification from the given list.
 */
void ul_remove_notification
(
   icb_notify_list_type *list,
   icb_notify_type *     elem
)
{
   uint32_t i;
   icb_notify_type *prev, *iter;

   /* Check input parameters. */
   if( NULL == list || NULL == elem)
   {
      return;
   }

   /* Search the list for the element we want.
    * Bound our search to the number of items we know are in the list. */
   prev = NULL;
   iter = list->head;
   for( i = 0; i < list->count && NULL != iter; i++ )
   {
      /* If we found the item, remove it. */
      if( iter == elem )
      {
         /* If we're replacing the head, update our list header. */
         if( NULL == prev )
         {
            list->head = iter->next;
         }
         /* Otherwise, update the previous item in the list. */
         else
         {
            prev->next = iter->next;
         }
         list->count -= 1;
         break;
      }
      /* Otherwise, advance down the list. */
      else
      {
         prev = iter;
         iter = iter->next;
      }
   }
}

/** ul_get_notification_list
 *
 * @brief Get the notification list for a given node.
 *
 * @pre   Caller must check that definition is non-NULL.
 */
icb_notify_list_type *ul_get_notification_list
(
   icb_notify_definition_type *definition
)
{
   icb_notify_list_type *notify_list = NULL;

   if( ICB_NOTIFY_MASTER_NODE == definition->node_type )
   {
      ul_int_master_list_type master_list;
      uint32_t                i;

      /* Fetch the master list and search for the requested node. */
      ul_target_get_master_list( &master_list );
      for( i = 0; i < master_list.num_masters; i++ )
      {
         /* If we find what we're looking for, allocate some storage and place it on the list. */
         if( (ICBId_MasterType)definition->id == master_list.masters[i]->id )
         {
            notify_list = &master_list.masters[i]->notifications;
         }
      }
   }
   else if( ICB_NOTIFY_SLAVE_NODE == definition->node_type )
   {
      ul_int_slave_list_type slave_list;
      uint32_t               i;

      ul_target_get_slave_list( &slave_list );
      for( i = 0; i < slave_list.num_slaves; i++ )
      {
         /* If we find what we're looking for, allocate some storage and place it on the list. */
         if( (ICBId_SlaveType)definition->id == slave_list.slaves[i]->id )
         {
            notify_list = &slave_list.slaves[i]->notifications;
         }
      }
   }

   return notify_list;
}

/** ul_notify_update_status
 *
 * @brief Forces an update of the notification status.
 */
void ul_notify_update_status
(
   icb_notify_type *notify
)
{
   if( NULL != notify )
   {
      icb_node_state_type *state = NULL;

      /* Get the state block for the given node. */
      if( ICB_NOTIFY_MASTER_NODE == notify->definition.node_type )
      {
         state = ul_get_master_state( (ICBId_MasterType)notify->definition.id );
      }
      else if( ICB_NOTIFY_SLAVE_NODE == notify->definition.node_type )
      {
         state = ul_get_slave_state( (ICBId_SlaveType)notify->definition.id );
      }

      /* Assuming we found one, initialize and callback. */
      if( NULL != state )
      {
         icb_notify_status_type status;

         /* We need to have a known state for the rest of this to work.
          * Set both the state and previous state to what we are now. */
         notify->status.ib    = MAX(state->ib, state->supp_ib);
         notify->status.ab    = state->ab + state->supp_ab;
         notify->status.total = state->total_bw;
         notify->status_valid = true;

         notify->prev_state.ib =    notify->status.ib;
         notify->prev_state.ab =    notify->status.ab;
         notify->prev_state.total = notify->status.total;

         /* Pass a copy of the state data.
         * We don't want to pass a pointer to the actual storage location. */
         status = notify->status;
         notify->callback(notify, &status, notify->callback_data);
      }
   }
}

/**=============================================================================
   @copydoc icb_notify_register()
==============================================================================*/
icb_notify_handle_type icb_notify_register
(
   icb_notify_definition_type *definition,
   icb_notify_cb_type          callback,
   void                       *callback_data
)
{
   icb_notify_handle_type handle = NULL;
   icb_notify_list_type * notify_list;

   /* Check out input parameters.
    * Callback data can be NULL.
    * Make sure we're also initialized by looking at self.
    * Check the notification and crossing types as well. */
   if( NULL == self ||
       NULL == definition ||
       NULL == callback ||
       definition->threshold_type >= ICB_NOTIFY_NUM_THRESHOLD_TYPES ||
       definition->crossing_type  >= ICB_NOTIFY_CROSSING_NUM_TYPES)
   {
      return handle;
   }

   /* Try to find the notification list for the node asked for. */
   notify_list = ul_get_notification_list( definition );

   /* If we found something, malloc and add to the appropriate notification list. */
   if( NULL != notify_list )
   {
      icb_notify_type *notify;

      if( DAL_SUCCESS ==
            DALSYS_Malloc( sizeof(icb_notify_type),
                           (void **)&notify ) )
      {

         /* Copy the definition. */
         memset(notify, 0, sizeof(icb_notify_type));
         notify->definition = *definition;

         /* Store callback information. */
         notify->callback      = callback;
         notify->callback_data = callback_data;

         /* Lock the arbiter node and do the insertion. */
         npa_resource_lock(self);
         ul_insert_notification(notify_list, notify);
         notify->list = notify_list;

         /* Set the initial previous state to the high threshold value. */
         switch(notify->definition.threshold_type)
         {
            case ICB_NOTIFY_AB:
               notify->prev_state.ab =    notify->definition.high_threshold[0];
               break;
            case ICB_NOTIFY_IB:
               notify->prev_state.ib =    notify->definition.high_threshold[0];
               break;
            case ICB_NOTIFY_TOTAL:
               notify->prev_state.total = notify->definition.high_threshold[0];
               break;
            case ICB_NOTIFY_IB_AB:
               notify->prev_state.ib =    notify->definition.high_threshold[0];
               notify->prev_state.ab =    notify->definition.high_threshold[1];
               break;
            default:
               /* Do nothing. We can't get here. */
               break;
         }

         /* Force an update of the notification status on this request. */
         ul_notify_update_status(notify);

         /* Unlock the arbiter node. */
         npa_resource_unlock(self);

         handle = (icb_notify_handle_type)notify;
      }
   }

   return handle; 
}

/**=============================================================================
   @copydoc icb_notify_update()
==============================================================================*/
bool icb_notify_update
(
   icb_notify_handle_type      handle,
   icb_notify_definition_type *definition
)
{
   bool ret = false;

   /* Check input parameters and make sure we're initialized. */
   if( NULL != self &&
       NULL != handle &&
       NULL != definition )
   {
      /* Try to find the notification list for the node asked for. */
      icb_notify_list_type *notify_list = ul_get_notification_list( definition );

      /* If it's valid, continue. */
      if( NULL != notify_list )
      {
         bool new = false;
         icb_notify_type *notify = (icb_notify_type *)handle;

         /* Lock the arbiter node before we change it's internal state. */
         npa_resource_lock( self );

         /* First, we need to check if the node to watch has changed. */
         if( definition->node_type != notify->definition.node_type ||
             definition->id        != notify->definition.id )
         {
            /* If they're different, remove the notification from the old node. */
            ul_remove_notification( notify->list, notify );
            new = true;
         }

         /* Copy the new definition. */
         notify->definition = *definition;

         /* Clear the status. */
         notify->status_valid = false;
         memset( &notify->status, 0, sizeof(notify->status) );

         /* If we removed it from a different node earlier, add to the new node. */
         if( true == new )
         {
            ul_insert_notification( notify_list, notify );
            notify->list = notify_list;
         }

         /* Set the initial previous state to the high threshold value. */
         switch(notify->definition.threshold_type)
         {
            case ICB_NOTIFY_AB:
               notify->prev_state.ab =    notify->definition.high_threshold[0];
               break;
            case ICB_NOTIFY_IB:
               notify->prev_state.ib =    notify->definition.high_threshold[0];
               break;
            case ICB_NOTIFY_TOTAL:
               notify->prev_state.total = notify->definition.high_threshold[0];
               break;
            case ICB_NOTIFY_IB_AB:
               notify->prev_state.ib =    notify->definition.high_threshold[0];
               notify->prev_state.ab =    notify->definition.high_threshold[1];
               break;
            default:
               /* Do nothing. We can't get here. */
               break;
         }

         /* Force an update of the notification status on this request. */
         ul_notify_update_status(notify);

         /* Unlock the arbiter node. */
         npa_resource_unlock( self );

         ret = true;
      }
   }

   return ret;
}

/**=============================================================================
   @copydoc icb_notify_deregister()
==============================================================================*/
bool icb_notify_deregister
(
   icb_notify_handle_type handle
)
{
   bool ret = false;

   /* Verify input parameters and initialization status. */
   if( NULL != self &&
       NULL != handle )
   {
      icb_notify_list_type *notify_list;
      icb_notify_type *notify = (icb_notify_type *)handle;

      notify_list = ul_get_notification_list( &notify->definition );
      
      /* If we found the correct notification list, clean up. */
      if( NULL != notify_list )
      {
         /* Lock the arbiter node and do the removal. */
         npa_resource_lock( self );
         ul_remove_notification( notify_list, notify );
         npa_resource_unlock( self );

         DALSYS_Free( notify );
         notify = NULL;
         ret = true;
      }
   }

   return ret;
}

/**=============================================================================
   @copydoc icb_notify_get_notification_status()
==============================================================================*/
bool icb_notify_get_notification_status
(
  icb_notify_handle_type  handle,
  icb_notify_status_type *status
)
{
   bool ret = false;
   icb_notify_type *notify = (icb_notify_type *)handle;

   /* Check input parameters and make sure we're initialized. */
   if( NULL != self &&
       NULL != notify &&
       NULL != status )
   {
      /* Lock around the access to the notification status.
       * The UL will be updating this during transactions. */
      npa_resource_lock(self);
      *status = notify->status;
      npa_resource_unlock(self);

      ret = true;
   }

   return ret;
}

/**=============================================================================
   @copydoc icb_notify_reset_notification_status()
==============================================================================*/
bool icb_notify_reset_notification_status
(
  icb_notify_handle_type  handle,
  icb_notify_status_type *status
)
{
   bool ret = false;
   icb_notify_type *notify = (icb_notify_type *)handle;

   /* Check input parameters and make sure we're initialized. */
   if( NULL != self &&
       NULL != notify &&
       NULL != status )
   {
      /* Lock around the access to the notification status.
       * The UL will be updating this during transactions. */
      npa_resource_lock( self );
      notify->status.high_threshold &= ~status->high_threshold;
      notify->status.low_threshold &= ~status->low_threshold;
      npa_resource_unlock( self );

      ret = true;
   }

   return ret;
}

/* vim: set ts=3 sts=3 sw=3 et :*/ 
