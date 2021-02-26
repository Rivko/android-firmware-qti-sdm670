#ifndef _UNPA_H_
#define _UNPA_H_
/*==============================================================================
  FILE:         unpa.h
  
  OVERVIEW:     uNPA client API. Register clients and issue requests

  DEPENDENCIES: None

                Copyright (c) 2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/api/power/unpa/unpa.h#3 $
$DateTime: 2017/10/27 10:53:19 $
==============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

#include "comdef.h"

/*==============================================================================
                              MACRO DEFINITIONS
 =============================================================================*/ 
/* strlen of names (of resources, clients etc.) must be < than this max */
#define UNPA_MAX_NAME_LEN         32

/* Max possible state of a resource */
#define UNPA_MAX_STATE            0xFFFFFFFF

/* Client types */
#define UNPA_CLIENT_REQUIRED      0x1 /* Request applies to both the active and 
                                       * sleep sets (or equivalents) */
#define UNPA_CLIENT_SUPPRESSIBLE  0x2 /* Request only applies to the active set (or equivalent) */
#define UNPA_CLIENT_SLEEP         0x4 /* Request only applies to the sleep set (or equivalent) */


/* Request attributes are one-time flags that may be set by OR-ing
 * one or more of the below into unpa_client->request_attr before issuing
 * a request. Attributes only apply to that request and the field is reset
 * internally to UNPA_REQUEST_DEFAULT before return. While attributes have
 * a notional meaning, consequent actions are up to/dependent on the resource.*/

/* Default - No special request attributes */
#define UNPA_REQUEST_DEFAULT      0x0

/*==============================================================================
                                TYPE DEFINITIONS
 =============================================================================*/
typedef uint32 unpa_resource_state;
 
typedef union unpa_request
{
  unpa_resource_state val;
} unpa_request;
 
typedef struct unpa_client
{
  /* Name is limited to UNPA_MAX_NAME_LEN chars, including the '\0' */
  const char            *name;
  
  /* Client type */
  uint32                 type : 8;
  
  /* Client attributes */
  uint32                 attr : 8;
  
  /* Request attributes */
  uint32                 request_attr : 16;
  
  /* Active request from client */
  unpa_request           active_request; 
  
  /* Pending request from client (=active_request, if the last request from
   * client was processed. Not, if we are in the midst of processing it) */
  unpa_request           pending_request;
  
  /* Resource to which this client is a client to; will be set by UNPA
   * during register */
  struct unpa_resource  *resource;
  
  /* Client-specific resource data; allows resource authors to associate
   * client-specific data to this structure */
  void                  *resource_data;
  
  /* Pointer to the next client to "resource" */
  struct unpa_client    *next;
} unpa_client;

typedef void* unpa_query_handle;

/* Supported query ids */
typedef enum
{
  /* With a query handle created using unpa_get_query_handle */
  UNPA_QUERY_ACTIVE_STATE = 0,
  UNPA_QUERY_SLEEP_STATE,
  UNPA_QUERY_ACTIVE_MAX,
  
  /* With a query handle created using unpa_get_client_query_handle */
  UNPA_QUERY_ACTIVE_REQUEST,
} unpa_query_id;

/* Possible query return values */
typedef enum
{
  UNPA_QUERY_QDI_ERROR = -1,
  UNPA_QUERY_SUCCESS = 0,
  UNPA_QUERY_UNSUPPORTED,
} unpa_query_status;

/* The result of a query. Content is valid only if the query
   returned UNPA_QUERY_SUCCESS */
typedef struct unpa_query_result
{
  union
  {
    unpa_resource_state val;
  };
} unpa_query_result;

/*==============================================================================
                              GLOBAL FUNCTIONS
 =============================================================================*/
/** 
 * unpa_createClient
 *
 * @brief Allocate and initialize a client to "resourceName".
 *        If a resource with the given name is defined and accepts clients of
 *        type "clientType", a new unpa_client structure is allocated,
 *        initialized and a pointer to it returned to caller.
 *        If the client cannot be created, NULL is returned.
 *
 * @param clientName:    Name of the client; 
 *                       strlen(clientName) must be < UNPA_MAX_NAME_LEN.
 * @param clientType:    Type of client; one of the UNPA_CLIENT_* defines
 * @param resourceName:  Name of the resource to create a client to
 *
 * @return If successful, a pointer to a unpa_client structure; else, NULL
 */
#define unpa_createClient(clientName, clientType, resourceName) \
          unpa_newClient(clientName, clientType, resourceName, NULL) 

/** 
 * unpa_initClient
 *
 * @brief Initialize the given unpa_client data structure as a client to
 *        "resourceName". If a resource with the given name is defined and
 *        accepts clients of type "clientType", the unpa_client structure
 *        is initialized and a pointer to it returned to caller.
 *        If the client cannot be created, NULL is returned.
 *
 * @param clientName:    Name of the client;
 *                       strlen(clientName) must be < UNPA_MAX_NAME_LEN.
 * @param clientType:    Type of client; one of the UNPA_CLIENT_* defines
 * @param resourceName:  Name of the resource to create a client to
 * @param client:        Pointer to a (uninitialized, static) unpa_client.
 *
 * @return If successful, "client"; else, NULL
 */
#define unpa_initClient(clientName, clientType, resourceName, client) \
          unpa_newClient(clientName, clientType, resourceName, client) 
 
/** 
 * unpa_newClient
 *
 * @brief Do not use this routine directly. Instead, use either
 * unpa_createClient or unpa_initClient depending on whether you want UNPA
 * to allocate a unpa_client data structure or pass in one.
 */
unpa_client* unpa_newClient(const char  *clientName, 
                            uint32       clientType,
                            const char  *resourceName,
                            unpa_client *client);

/** 
 * unpa_destroyClient 
 *  
 * @brief Unlink a client from its resource and destroy it. The current request
 *        from client is cancelled. destroyClient may be used with both
 *        allocated (createClient) or just initialized (initClient) clients. 
 *
 * @param client: Pointer to the unpa_client to destroy.
 */
void unpa_destroyClient(unpa_client *client);

/** 
 * unpa_issueRequest
 *
 * @brief Issue a request with client. 
 *        Note that clients are non-reentrant and use must be serialised by caller. 
 *        For readability, use one of the below, issue_required_request or
 *        issue_suppressible_request macros, depending on your client type.
 */
void unpa_issueRequest(unpa_client* client, unpa_resource_state request);

/** 
 * unpa_issueRequiredRequest
 *
 * @brief Issue a synchronous, required request to the resource.
 *
 * @param client: The client
 * @param request: The request
 */
#define unpa_issueRequiredRequest     unpa_issueRequest

/** 
 * unpa_issueSuppressibleRequest
 *
 * @brief Issue a synchronous, suppressible request to the resource.
 *
 * @param client: The client
 * @param request: The request
 */
#define unpa_issueSuppressibleRequest unpa_issueRequest

/** 
 * unpa_tryIssueRequest
 *
 * @brief Attempts to issue a request witht client, using a try_lock
 *        on the resource; if the lock succeeds, processes the request and
 *        returns 0. Else, returns -1.
 *        For readability, use one of the below, try_issue_required_request or
 *        try_issue_suppressible_request macros, depending on your client type.
 */
int32 unpa_tryIssueRequest(unpa_client* client, unpa_resource_state request);

/** 
 * unpa_tryIssueRequiredRequest
 *
 * @brief Attempt to issue a synchronous, required request to the resource.
 *        See unpa_try_issue_request
 *
 * @param client:   The client
 * @param request:  The request
 */
#define unpa_tryIssueRequiredRequest     unpa_tryIssueRequest

/** 
 * unpa_tryIssueSuppressibleRequest
 *
 * @brief Issue a synchronous, suppressible request to the resource.
 *        See unpa_try_issue_request 
 *
 * @param client:   The client
 * @param request:  The request
 */
#define unpa_tryIssueSuppressibleRequest unpa_tryIssueRequest

/** 
 * unpa_cancelRequest
 *
 * @brief Cancel (or drop) the previous request from client.
 *        The resource will see this as a 0-valued request.
 *        Equivalent to unpa_completeRequest
 *
 * @param client: The client
 */
void unpa_cancelRequest(unpa_client* client);

/** 
 * unpa_completeRequest
 *
 * @brief Complete (or drop) the previous request from client.
 *        The resource will see this as a 0-valued request.
 *        Equivalent to unpa_cancel_request 
 *
 * @param client: The client
 */
#define unpa_completeRequest unpa_cancelRequest

/** 
 * unpa_tryCancelRequest
 *
 * @brief Attempts to cancel (or drop) the previous request from client,
 *        using a try_lock on the resource; if the lock succeeds, processes the
 *        cancel and returns 0. Else, returns -1.
 *        The resource will see this as a 0-valued request.
 *        Equivalent to unpa_try_complete_request
 *
 * @param client: The client
 */
int32 unpa_tryCancelRequest(unpa_client* client);

/** 
 * unpa_tryCompleteRequest
 *
 * @brief Complete (or drop) the previous request from client.
 *        The resource will see this as a 0-valued request.
 *        Equivalent to unpa_cancel_request 
 *
 * @param client: The client
 */
#define unpa_tryCompleteRequest unpa_tryCancelRequest

/** 
 * unpa_getQueryHandle
 *
 * @brief Retrieve a handle that may be used in unpa_query to
 *        query certain runtime attributes of the resource, such as active_state.
 *
 * @param resourceName: Name of the resource
 *
 * @return A query handle to the resource or NULL, if the resource was
 *         not (yet) defined.
 */
unpa_query_handle unpa_getQueryHandle(const char *resourceName);

/** 
 * unpa_getClientQueryHandle
 *
 * @brief Retrieve a handle that may be used in unpa_query to
 *        query certain runtime attributes of a particular client to a resource.
 *
 * @param clientName:   Name of the client
 * @param resourceName: Name of the resource
 *
 * @return A query handle to the client or NULL, if the client doesn't exist.
 */
unpa_query_handle unpa_getClientQueryHandle(const char *clientName,
                                            const char *resourceName );

/** 
 * unpa_query
 *
 * @brief Query certain runtime attributes of the resource or client, 
 *        using the given unpa_query_handle.
 *
 * @param handle: Query handle, obtained using unpa_get_query_handle or
 *                unpa_get_client_query_handle. Will be interpreted as a query handle
 *                to a resource or client, based on the query id passed in.
 * @param id:     One of the unpa_query_id enums.
 * @param result: OUT parameter with the results of the query.
 *
 * @return UNPA_QUERY_SUCCESS if successful; else, one of the other
 *         unpa_query_status enums.
 */
unpa_query_status unpa_query(unpa_query_handle  handle, 
                             unpa_query_id      id,
                             unpa_query_result *result);

#ifdef __cplusplus
}
#endif

#endif /* _UNPA_H_ */
