/*============================================================================
@file unpa.h

uNPA client API. Register clients and issue requests

Copyright (c) 2016 QUALCOMM Technologies Incorporated.
All Rights Reserved.
QUALCOMM Technologies Proprietary/GTDR

============================================================================*/
#ifndef UNPA_H
#define UNPA_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/*----------------------------------------------------------------------------
 * Defines/Enums
 * -------------------------------------------------------------------------*/
 
/* Names are restricted to this many chars, incl. '\0' */
#define UNPA_MAX_NAME_LEN 16

/* Max possible state of a resource */
#define UNPA_MAX_STATE 0xFFFFFFFF


/* Client types. We use #defines, rather than an enum to enable bit fields
   in unpa_client */

/* Request applies to both the active and sleep sets (or equivalents) */
#define UNPA_CLIENT_REQUIRED 0x1

/* Request only applies to the active set (or equivalent) */
#define UNPA_CLIENT_SUPPRESSIBLE 0x2

/* Request only applies to the sleep set (or equivalent) */
#define UNPA_CLIENT_SLEEP 0x4


/* Request attributes are one-time flags that may be set by OR-ing
 * one or more of the below into unpa_client->request_attr before issuing
 * a request. Attributes only apply to that request and the field is reset
 * internally to UNPA_REQUEST_DEFAULT before return. While attributes have
 * a notional meaning, consequent actions are up to/dependent on the resource.
 */

/* Default - no special request attributes */
#define UNPA_REQUEST_DEFAULT 0x0

/* Framework reserved request attributes - not for use of clients */
#define UNPA_REQUEST_RESERVED_ATTR1 0x1
#define UNPA_REQUEST_RESERVED_ATTR2 0x2
#define UNPA_REQUEST_RESERVED_ATTR3 0x4
#define UNPA_REQUEST_RESERVED_ATTR4 0x8
#define UNPA_REQUEST_RESERVED_ATTR5 0x10
#define UNPA_REQUEST_RESERVED_ATTR6 0x20
#define UNPA_REQUEST_RESERVED_ATTR7 0x40
#define UNPA_REQUEST_RESERVED_ATTR8 0x80

/* Ask the resource to treat the request as fire-and-forget; no need to wait
   for completion */
#define UNPA_REQUEST_FIRE_AND_FORGET 0x100


/*----------------------------------------------------------------------------
 * Types
 * -------------------------------------------------------------------------*/
 
typedef uint32_t unpa_resource_state;
 
typedef union unpa_request
{
  unpa_resource_state val;
} unpa_request;
 
typedef void ( *unpa_request_done_cb )( uint32_t data );
 
typedef struct unpa_client
{
  /* Name is limited to UNPA_MAX_NAME_LEN chars, including the '\0' */
  const char *name;
  
  /* Client type */
  uint32_t type : 8;
  
  /* Request attributes */
  uint32_t request_attr : 16;
  
  /* Framework internal attributes */
  uint32_t internal_attr : 8;
  
  /* Active request from client */
  unpa_request active_request; 
  
  /* Pending request from client (=active_request, if the last request from
     client was processed. Not, if we are in the midst of processing it) */
  unpa_request pending_request;
  
  /* Resource to which this client is a client to; will be set by UNPA
     during register */
  struct unpa_resource *resource;
  
  /* Client-specific resource data; allows resource authors to associate
     client-specific data to this structure */
  void *resource_data;
  
  /* Async completion callback; only valid if the request has the
     UNPA_REQUEST_ASYNC attribute set */
  unpa_request_done_cb done_cb;
  
  /* Async request data; resource authors must store the "request id"
     associated with the async request here, so it can be returned to caller */
  uint32_t async_data;
  
  /* Pointer to the next client to "resource" */
  struct unpa_client *next;
} unpa_client;

typedef void* unpa_query_handle;

/* Supported query ids */
typedef enum
{
  /* With a query handle created using unpa_get_query_handle */
  UNPA_QUERY_ACTIVE_STATE,
  UNPA_QUERY_SLEEP_STATE,
  UNPA_QUERY_ACTIVE_MAX,
  
  /* With a query handle created using unpa_get_client_query_handle */
  UNPA_QUERY_ACTIVE_REQUEST,
} unpa_query_id;

/* Possible query return values */
typedef enum
{
  UNPA_QUERY_SUCCESS = 0,
  UNPA_QUERY_UNSUPPORTED,
} unpa_query_status;

/* The result of a query. Content is valid only if the query
   returned UNPA_QUERY_SUCCESS */
typedef struct unpa_query_result
{
  /* union to allow for more result types in the future;
     anon union to allow for direct access */
  union
  {
    uint32_t val;
  };
} unpa_query_result;
 
/*----------------------------------------------------------------------------
 * Functions
 * -------------------------------------------------------------------------*/

/** 
 * <!-- unpa_create_client -->
 *
 * @brief Create a client to a resource; you cannot issue requests to resources
 * without first creating a client to it. If a resource with the given name
 * is defined and accepts clients of "type", a new unpa_client structure
 * will be created and and a pointer to it returned to caller.
 * If the client cannot be created, NULL is returned.
 *
 * @param client_name: Name of the client; length, including the '\0',
 * must be < UNPA_MAX_NAME_LEN.
 * @param client_type: Type of client; one of the UNPA_CLIENT_* defines
 * @param resource_name: Name of the resource to create a client to
 *
 * @return If successful, a pointer to a unpa_client structure; else, NULL
 */
unpa_client* unpa_create_client( const char *client_name, uint32_t client_type,
                                 const char *resource_name );

/** 
 * <!-- unpa_destroy_client -->
 * @brief Destroy a client. The current request from client is cancelled.
 *
 * @param client: Pointer to the unpa_client to destroy.
 */
void unpa_destroy_client( unpa_client *client );

/** 
 * <!-- unpa_set_request_attribute -->
 *
 * @brief Sets the attribute for the specific request
 * The attribute is "one shot" i.e. will be set for the duration of the request and will be
 * cleared out when the request is complete.
 *
 * @param client: The client
 * @param attribute: The attribute to set (usually a mask).
 */
void unpa_set_request_attribute( unpa_client* client, uint32_t attribute);
  
/** 
 * <!-- unpa_issue_request -->
 *
 * @brief Issue a request from client. 
 * Note that clients are non-reentrant and use must be serialised by caller. 
 * For readability, use one of the below, issue_required_request or
 * issue_suppressible_request macros, depending on your client type.
 */
void unpa_issue_request( unpa_client* client, unpa_resource_state request );

/** 
 * <!-- unpa_issue_required_request -->
 *
 * @brief Issue a synchronous, required request to the resource.
 *
 * @param client: The client
 * @param request: The request
 */
#define unpa_issue_required_request unpa_issue_request

/** 
 * <!-- unpa_issue_suppressible_request -->
 *
 * @brief Issue a synchronous, suppressible request to the resource.
 *
 * @param client: The client
 * @param request: The request
 */
#define unpa_issue_suppressible_request unpa_issue_request

/** 
 * <!-- unpa_issue_request_async -->
 *
 * @brief Issue a request from client such that the resource processes the
 * request asynchronously. Resource will dispatch "done_cb" after the
 * request is processed. See note on return value.
 * Clients are non-reentrant and use must be serialised by caller. 
 * For readability, use one of the below, issue_required_request_async or
 * issue_suppressible_request_async macros, depending on your client type.
 *
 * @return A "request id" for the async request that allows the caller
 * to map this request to the eventual callback. A return value of 0 indicates
 * that the request is done (was executed synchronously or was short-circuited)
 * and no callback will arrive.
 */
uint32_t unpa_issue_request_async( unpa_client* client, 
                                   unpa_resource_state request,
                                   unpa_request_done_cb done_cb );

/** 
 * <!-- unpa_issue_required_request_async -->
 *
 * @brief Issue an asynchronous, required request to the resource.
 *
 * @param client: The client
 * @param request: The request
 */
#define unpa_issue_required_request_async unpa_issue_request_async

/** 
 * <!-- unpa_issue_suppressible_request_async -->
 *
 * @brief Issue an asynchronous, suppressible request to the resource.
 *
 * @param client: The client
 * @param request: The request
 */
#define unpa_issue_suppressible_request_async unpa_issue_request_async


/** 
 * <!-- unpa_try_issue_request -->
 *
 * @brief Attempts to issue a request from "client", using a try_lock
 * on the resource; if the lock succeeds, processes the request and
 * returns 0. Else, returns -1.
 * For readability, use one of the below, try_issue_required_request or
 * try_issue_suppressible_request macros, depending on your client type.
 */
int32_t unpa_try_issue_request( unpa_client* client, 
                                unpa_resource_state request );
                                

/** 
 * <!-- unpa_try_issue_required_request -->
 *
 * @brief Attempt to issue a synchronous, required request to the resource.
 * See unpa_try_issue_request
 *
 * @param client: The client
 * @param request: The request
 */
#define unpa_try_issue_required_request unpa_try_issue_request

/** 
 * <!-- unpa_try_issue_suppressible_request -->
 *
 * @brief Issue a synchronous, suppressible request to the resource.
 * See unpa_try_issue_request 
 *
 * @param client: The client
 * @param request: The request
 */
#define unpa_try_issue_suppressible_request unpa_try_issue_request

/** 
 * <!-- unpa_cancel_request -->
 *
 * @brief Cancel (or drop) the previous request from client.
 * The resource will see this as a 0-valued request.
 * Equivalent to unpa_complete_request
 *
 * @param client: The client
 */
void unpa_cancel_request( unpa_client* client );

/** 
 * <!-- unpa_complete_request -->
 *
 * @brief Complete (or drop) the previous request from client.
 * The resource will see this as a 0-valued request.
 * Equivalent to unpa_cancel_request 
 *
 * @param client: The client
 */
#define unpa_complete_request unpa_cancel_request

/** 
 * <!-- unpa_cancel_request_async -->
 *
 * @brief Cancel (or drop) the previous request from client, asynchronously.
 * The resource will dispatch "done_cb" after the cancel vote is processed.
 * See note on return value.
 *
 * Equivalent to unpa_complete_request_async
 *
 * @return A "request id" for the async request that allows the caller
 * to map this request to the eventual callback. A return value of 0 indicates
 * that the request is done (was executed synchronously or was short-circuited)
 * and no callback will arrive.
 */
uint32_t unpa_cancel_request_async( unpa_client* client, 
                                    unpa_request_done_cb done_cb );

/** 
 * <!-- unpa_complete_request_async -->
 *
 * @brief Complete (or drop) the previous request from client, asynchronously.
 * Equivalent to unpa_cancel_request_async
 *
 * @param client: The client
 */
#define unpa_complete_request_async unpa_cancel_request_async

/** 
 * <!-- unpa_try_cancel_request -->
 *
 * @brief Attempts to cancel (or drop) the previous request from client,
 * using a try_lock on the resource; if the lock succeeds, processes the
 * cancel and returns 0. Else, returns -1.
 * The resource will see this as a 0-valued request.
 * Equivalent to unpa_try_complete_request
 *
 * @param client: The client
 */
int32_t unpa_try_cancel_request( unpa_client* client );

/** 
 * <!-- unpa_try_complete_request -->
 *
 * @brief Complete (or drop) the previous request from client.
 * The resource will see this as a 0-valued request.
 * Equivalent to unpa_cancel_request 
 *
 * @param client: The client
 */
#define unpa_try_complete_request unpa_try_cancel_request

/** 
 * <!-- unpa_get_query_handle -->
 *
 * @brief Retrieve a handle that may be used in unpa_query to
 * query certain runtime attributes of the resource, such as active_state.
 *
 * @param resource_name: Name of the resource
 *
 * @return A query handle to the resource or NULL, if the resource was
 * not (yet) defined.
 */
unpa_query_handle unpa_get_query_handle( const char *resource_name );

/** 
 * <!-- unpa_get_client_query_handle -->
 *
 * @brief Retrieve a handle that may be used in unpa_query to
 * query certain runtime attributes of a particular client to a resource.
 *
 * @param client_name: Name of the client
 * @param resource_name: Name of the resource
 *
 * @return A query handle to the client or NULL, if the client doesn't exist.
 */
unpa_query_handle unpa_get_client_query_handle( const char *client_name,
                                                const char *resource_name );

/** 
 * <!-- unpa_query -->
 *
 * @brief Query certain runtime attributes of the resource or client, 
 * using the given unpa_query_handle.
 *
 * @param handle: Query handle, obtained using unpa_get_query_handle or
 * unpa_get_client_query_handle. Will be interpreted as a query handle
 * to a resource or client, based on the query id passed in.
 * @param id: One of the unpa_query_id enums.
 * @param result: OUT parameter with the results of the query.
 *
 * @return UNPA_QUERY_SUCCESS if successful; else, one of the other
 * unpa_query_status enums.
 */
unpa_query_status 
unpa_query( unpa_query_handle handle, unpa_query_id id,
            unpa_query_result *result );


#ifdef __cplusplus
}
#endif

#endif /* UNPA_H */