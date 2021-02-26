#ifndef ICBARB_H
#define ICBARB_H
/*=============================================================================

FILE:      icbarb.h

DESCRIPTION: This defines the NPA ICB Arbiter Node interface

        Copyright (c) 2010-2017 QUALCOMM Technologies, Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
-------------------------------------------------------------------------------

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Include/api/systemdrivers/icbarb.h#1 $
  $DateTime: 2017/09/18 09:26:13 $
  $Author: pwbldsvc $
  $Change: 14402555 $
                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 03/08/17   sds     Add support for FAL standalone resource
 02/24/17   sds     Fix compilation warning
 11/08/16   sds     Update query interface to add BCM queries.
 06/01/16   sds     Change request types slightly to be easier to manage.
 06/20/14   sds     Extend request types to add latency request in addition to BW.
 03/11/14   sds     Extend query function - slave port count.
 07/07/13   jc      Added query function.
 06/18/12   dj      Rename struct members & vars starting with u32
 07/27/11   rng     Re-architected interface implementation to support
                    native NPA client creation functions.
 07/14/11   rng     Added support for suppressible requests
 04/27/11   rng     Increased Bandwidth variables to uint64
 10/04/10   pm      Added oversubscription event support
 08/24/10   pm      Changed function, type, and enum names for consistency
 08/20/10   pm      Changed filename to icbarb.h
 04/16/10   pm      Initial version

=============================================================================*/
#include "npa_resource.h"
#include "icbid.h"
#include "rpmh_client.h"

/*============================================================================
                          DATA TYPES
============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif
/* 
 *  Return codes
 */
typedef enum
{
   ICBARB_ERROR_SUCCESS = 0,
   ICBARB_ERROR_UNSUPPORTED,
   ICBARB_ERROR_INVALID_ARG,
   ICBARB_ERROR_INVALID_MASTER,
   ICBARB_ERROR_NO_ROUTE_TO_SLAVE,
   ICBARB_ERROR_VECTOR_LENGTH_MISMATCH,
   ICBARB_ERROR_OUT_OF_MEMORY,
   ICBARB_ERROR_UNKNOWN,
   ICBARB_ERROR_REQUEST_REJECTED,

   /* Insert new errors above here */
   ICBARB_ERROR_MAX,

   ICBARB_ERROR_PLACEHOLDER = 0x7FFFFFFF  /* Force to 32 bits */
} ICBArb_ErrorType;

/* 
 *  Pair of master and slave
 */
typedef struct
{
   ICBId_MasterType eMaster;
   ICBId_SlaveType  eSlave; 
} ICBArb_MasterSlaveType;

typedef struct
{
   npa_callback             callback;
   ICBArb_MasterSlaveType *aMasterSlave;
} ICBArb_CreateClientVectorType;

/* 
 *  Tells which type of arbitration settings are being represented.
 */
typedef enum
{
   /* Pick a request format (TYPE_1/TYPE_2/TYPE_3) and any required additional flags. */
   /* Basic request types. */
   ICBARB_REQUEST_TYPE_1        = 0x1,  /**< "Bursty" master */
   ICBARB_REQUEST_TYPE_2        = 0x2,  /**< CPU-style master */
   ICBARB_REQUEST_TYPE_3        = 0x4,  /**< Ib/Ab pair */

   /* Request flags. */
   ICBARB_REQUEST_FLAG_LATENCY  = 0x10000, /**< Latency requested */
   
   /* Composite request types, for convenience */
   /* Basic request with latency request. */
   ICBARB_REQUEST_TYPE_1_LAT    = ICBARB_REQUEST_TYPE_1 |
                                  ICBARB_REQUEST_FLAG_LATENCY,     /* "Bursty" master */
   ICBARB_REQUEST_TYPE_2_LAT    = ICBARB_REQUEST_TYPE_2 |
                                  ICBARB_REQUEST_FLAG_LATENCY,     /* CPU-style master */
   ICBARB_REQUEST_TYPE_3_LAT    = ICBARB_REQUEST_TYPE_3 |
                                  ICBARB_REQUEST_FLAG_LATENCY,     /* Ib/Ab pair */

   /* DEPRECATED request types */
   ICBARB_REQUEST_TYPE_1_TIER_3 = ICBARB_REQUEST_TYPE_1,  /* "Bursty" master */
   ICBARB_REQUEST_TYPE_2_TIER_3 = ICBARB_REQUEST_TYPE_2,  /* CPU-style master */
   ICBARB_REQUEST_TYPE_3_TIER_3 = ICBARB_REQUEST_TYPE_3,  /* Ib/Ab pair */

   /* Max bitmask + 1 */
   ICBARB_REQUEST_TYPE_MAX      = (ICBARB_REQUEST_TYPE_3 | ICBARB_REQUEST_FLAG_LATENCY) + 1,
} ICBArb_RequestTypeType;

/* 
 *  "Burst" Master Parameters
 */
typedef struct
{
   uint64 uDataBurst;        // Bytes
   uint32 uTransferTimeUs;   // microseconds
   uint32 uPeriodUs;         // microseconds
   uint32 uLatencyNs;        // Latency requirement in nanoseconds
                             // Ignored for basic request types
} ICBArb_Request1Type;

/* 
 *  CPU-style Master Parameters
 */
typedef struct
{
   uint64 uThroughPut;       // Bytes/second
   uint32 uUsagePercentage;  // Whole percentage (0-100)
   uint32 uLatencyNs;        // Latency requirement in nanoseconds
                             // Ignored for basic request types
} ICBArb_Request2Type;

/* 
 *  Ib/Ab Parameters
 */
typedef struct
{
   uint64 uIb;        // Instantaneous bandwidth in Bytes/second
   uint64 uAb;        // Arbitrated bandwidth in Bytes/second
   uint32 uLatencyNs; // Latency requirement in nanoseconds
                      // Ignored for basic request types
} ICBArb_Request3Type;

/* 
 *  Union of all Arb Types
 */
typedef union
{
   ICBArb_Request1Type type1;
   ICBArb_Request2Type type2;
   ICBArb_Request3Type type3;
} ICBArb_RequestUnionType;

/* 
 *  Structure that contains one master/slave combo worth of Arb data
 */
typedef struct
{
   ICBArb_RequestTypeType  arbType;   /* Which type of arb data is it? (enum) */
   ICBArb_RequestUnionType arbData;   /* The actual data (union) */
} ICBArb_RequestType;

/*============================================================================
                          DATA TYPES - QUERY
============================================================================*/
/* 
 *  Query codes
 */
typedef enum
{
   /* Get count of resources for the specified route */
   ICBARB_QUERY_RESOURCE_COUNT = 0,

   /* Get name strings of the resources for the specified route */
   ICBARB_QUERY_RESOURCE_NAMES,

   /* Get the number of ports for the specified slave */
   ICBARB_QUERY_SLAVE_PORT_COUNT,

   /* Get the number of command_sets required for BCM buffer. */
   ICBARB_QUERY_BCM_BUFFER_SIZE,

   /* Get the BCM request list for a given client.
   ** If command_sets in the request structure is NULL, memory will be allocated.
   ** Call with REQUEST_DEALLOC afterwards to free.
   ** If command_sets is not NULL, it will assume the caller has filled the correct buffer size. 
   ** Caller should allocate space for QUERY_BCM_REQUEST equal to 
   ** sizeof(rpmh_command_set_t) * uNumCommandSets */
   ICBARB_QUERY_BCM_REQUEST,

   /* Free any memory used by ICBARB_QUERY_BCM_REQUEST. */
   ICBARB_QUERY_BCM_REQUEST_FREE,

   /* Insert new queries above here */
   ICBARB_QUERY_MAX,

   ICBARB_QUERY_PLACEHOLDER = 0x7FFFFFFF  /* Force to 32 bits */
} ICBArb_QueryTypeType;

/* 
 *  Resource count query data for ICBARB_QUERY_RESOURCE_COUNT
 */
typedef struct
{
   npa_client_handle client;    /* Client making the request */
   uint32 uNumResource;         /* Resource count (output) */
} ICBArb_QueryResourceCountType;

/* 
 *  Resource names query data for ICBARB_QUERY_RESOURCE_NAMES
 *
 *  Note: uNumResource value must match that returned by a
 *        ICBARB_QUERY_RESOURCE_COUNT query for the master/slave combo.
 */
typedef struct
{
   npa_client_handle client;    /* Client making the request */
   const char **aResourceNames; /* Pointer to resource names array (output) */
   uint32 uNumResource;         /* Number of elements in names array (input) */
} ICBArb_QueryResourceNamesType;

/*
 * Slave port count query data for ICBARB_QUERY_SLAVE_PORT_COUNT
 */
typedef struct
{
   ICBId_SlaveType eSlaveId; /* The slave to query */
   uint32 uNumPorts;         /* Number of ports for the slave */
} ICBArb_QuerySlavePortCountType;

typedef struct
{
   npa_client_handle   client;           /**< Client to use for the query */
   uint32              num_requests;     /**< Number of requests in request array. */
   ICBArb_RequestType *requests;         /**< Pointer to requests to use to generate command values.
                                              NULL to fetch based on current state. */
   uint32              num_command_sets; /**< Number of command sets */
   rpmh_command_set_t *command_sets;     /**< Command set array */
} ICBArb_QueryBCMType;
/* 
 *  Union of all request types
 */
typedef union
{
   ICBArb_QueryResourceCountType  resourceCount;
   ICBArb_QueryResourceNamesType  resourceNames;
   ICBArb_QuerySlavePortCountType slavePortCount;
   ICBArb_QueryBCMType            bcmRequest;
} ICBArb_QueryUnionType;

/* 
 *  Query specification structure
 */
typedef struct
{
   ICBArb_QueryTypeType  queryType;  /* Which type of query is it? (enum) */
   ICBArb_QueryUnionType queryData;  /* The actual data (union) */
} ICBArb_QueryType;

/*============================================================================
                          DATA TYPES - FAL
============================================================================*/
/* 
 *  FAL request structure
 */
typedef struct
{
   uint32 fal_tolerance_ns;
   uint32 idle_time_ns;
} ICBArb_FALType;

/*============================================================================
                          FUNCTIONS
============================================================================*/

/**=============================================================================

   @brief icbarb_init - Initialize and define the ICB arbiter node

==============================================================================*/
void icbarb_init( void );

/**=============================================================================

   @brief icbarb_post_init - Complete initialization

   To be called once all initial client requests are made.

==============================================================================*/
void icbarb_post_init( void );

/**=============================================================================

   @brief ICBARB_CREATE_CLIENT_VECTOR

   This macro is used to fill in the client_value and client_ref fields of any
   npa_create_*_client_ex() function.

   To correctly use this you must allocate a vector to be passed into the npa call:

   ICBArb_CreateClientVectorType  <your_vector>;

   It is acceptable to allocate this vector on the stack as it will not be needed
   to persist once the call to npa_create_*_client_ex() has returned.

   Then you can call the npa client creation function.  All vector based client
   types are supported.  Below is an example using npa_create_sync_client_ex():

   npa_create_sync_client_ex( "/icb/arbiter",
                              "/client/MY_CLIENT",
                              NPA_CLIENT_VECTOR,
                              ICBARB_CREATE_CLIENT_VECTOR(
                                &<your_vector>,
                                <MasterSlaveArray>,
                                NumMasterSlavePairs,
                                <your_npa_callback_function>) );

   Note that the npa_callback function at the end can be NULL if you do not wish
   to receive oversubscription notifications.

   @param psVector           : Pointer to a ICBArb_CreateClientVectorType structure
   @param aMasterSlave       : An ICBArb_MasterSlaveType array
   @param uNumMasterSlaves   : Number of master/slave pairs in the aMasterSlave array
   @param npa_callback       : Oversubscription callback function (may be NULL)

==============================================================================*/
#define ICBARB_CREATE_CLIENT_VECTOR( psVector, aMasterSlave, uNumMasterSlave, npa_callback ) \
        (uNumMasterSlave) * sizeof(ICBArb_MasterSlaveType),                                                     \
        icbarb_fill_client_vector( psVector, aMasterSlave, npa_callback )

ICBArb_CreateClientVectorType *icbarb_fill_client_vector(
  ICBArb_CreateClientVectorType *psVector,
  ICBArb_MasterSlaveType        *aMasterSlave,
  npa_callback                   callback);

/**=============================================================================

   @brief icbarb_create_client - Creates a new ICB arbiter client

   DEPRECATED : Use native NPA functions with ICBARB_CREATE_CLIENT_VECTOR macro instead.
   Allows a client to set up a vector of master/slave pair that then
   processed to determine the internal routing parameters, which are stored
   and associated with the client.

   @param client_name : name of the client that's being created
   @param aMasterSlave : Array of Master/Slave pairs
   @param uNumMasterSlave : number of elements in aMasterSlave
 
   @return A handle to the created NPA client, NULL if the client
           creation failed

==============================================================================*/
npa_client_handle icbarb_create_client(
   const char             *pszClientName,
   ICBArb_MasterSlaveType *aMasterSlave, 
   uint32                  uNumMasterSlave);

/**=============================================================================

   @brief icbarb_create_client_ex - Creates a new ICB arbiter client with a
                                     callback function for state change
                                     notification (oversubscription notification)

   DEPRECATED : Use native NPA functions with ICBARB_CREATE_CLIENT_VECTOR macro instead.
   Allows a client to set up a vector of master/slave pair that then
   processed to determine the internal routing parameters, which are stored
   and associated with the client.  An additional argument allows registration
   for state change notification, which allows the client to be notified 
   when the fabrics are oversubscribed.

   @param client_name : name of the client that's being created
   @param aMasterSlave : Array of Master/Slave pairs
   @param uNumMasterSlave : number of elements in aMasterSlave
   @param callback: Oversubscription notification callback function pointer
 
   @return A handle to the created NPA client, NULL if the client
           creation failed

==============================================================================*/
npa_client_handle icbarb_create_client_ex(
   const char             *pszClientName,
   ICBArb_MasterSlaveType *aMasterSlave, 
   uint32                  uNumMasterSlave, 
   npa_callback            callback);

/**=============================================================================

   @brief icbarb_create_suppressible_client_ex -
                                     Creates a new ICB arbiter client with a
                                     callback function for state change
                                     notification (oversubscription notification)

   DEPRECATED : Use native NPA functions with ICBARB_CREATE_CLIENT_VECTOR macro instead.
   Allows a client to set up a vector of master/slave pair that then
   processed to determine the internal routing parameters, which are stored
   and associated with the client.  An additional argument allows registration
   for state change notification, which allows the client to be notified 
   when the fabrics are oversubscribed.  All clock requests from this client
   will be made with suppressible requests so that the clock requests may be
   removed when the originating processor enters low power mode.

   @param client_name : name of the client that's being created
   @param aMasterSlave : Array of Master/Slave pairs
   @param uNumMasterSlave : number of elements in aMasterSlave
   @param callback: Oversubscription notification callback function pointer
 
   @return A handle to the created NPA client, NULL if the client
           creation failed

==============================================================================*/
npa_client_handle icbarb_create_suppressible_client_ex(
   const char             *pszClientName,
   ICBArb_MasterSlaveType *aMasterSlave, 
   uint32                  uNumMasterSlave, 
   npa_callback            callback);

/**=============================================================================

   @brief icbarb_issue_request - Issues an ICB arbiter request
   
   Allows a client to apply a vector of arbitration settings to the routes
   already established by a previous icbarb_create_client() call.
   The vector is a set of unions of arbitration settings of Types 1 and 2
   above (and more types in the future, as needed.)
   
   Note that the number of elements in the vector must match
   the number of master/slave pairs in the previous icbarb_create_client()
   call for that client.  If the number of elements in the
   icbarb_issue_request() call does not match, then the function
   returns an error (ICBARB_ERROR_VECTOR_LENGTH_MISMATCH.)
   
   @param client : Client making the request
   @parama MasterArbCombined : Array of Arbitration info
   @param uNumMasterArbCombined : number of elements in aMasterArbCombinedType
   
   @return The result of the request (e.g. success or an error code)
 
  SIDE EFFECTS
     If the result of the request is ICBARB_ERROR_REQUEST_REJECTED, then the
     request is set to zero (similar to the icbarb_complete_request()
     call below.)  This is because there is the possibility of the driver
     getting into an inconsistent state if this kind of cleanup is not done.

==============================================================================*/
ICBArb_ErrorType icbarb_issue_request( 
   npa_client_handle   client, 
   ICBArb_RequestType *aRequest,  
   uint32              uNumRequest);               

/**=============================================================================

   @brief icbarb_complete_request - Completes an ICB arbiter request
   
   The effect of this function is the same as a icbarb_issue_request() call
   where the vector elements are all 0.
   
   @param client : Client making the request
 
==============================================================================*/
void icbarb_complete_request( npa_client_handle client );

/**=============================================================================

   @brief icbarb_destroy_client - Destroys an ICB arbiter client
   
   Sets all the routes to 0 request, deletes the route information,
   and frees the associated memory.
   
   This will cancel the client request (if any) and free the client
   structure. The client handle can not be used to issue requests after
   being destroyed.
   
   @param client : Client making the request

==============================================================================*/
void icbarb_destroy_client( npa_client_handle client );

/**=============================================================================

   @brief icbarb_query - Queries ICB parameters
   
   Provides access to ICB internal data.
   
   @param query : Pointer to query specification
   
   @return The result of the query (e.g. success or an error code)

==============================================================================*/
ICBArb_ErrorType icbarb_query( ICBArb_QueryType *pQuery );

#ifdef __cplusplus
}
#endif

#endif /* ICBARB_H */

/* vim: set ts=3 sts=3 sw=3 et :*/ 
