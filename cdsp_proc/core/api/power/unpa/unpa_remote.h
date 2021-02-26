#ifndef _UNPA_REMOTE_H_
#define _UNPA_REMOTE_H_
/*==============================================================================
  FILE:         unpa.h
  
  OVERVIEW:     uNPA remote client API. Register clients and issue requests
                in user PD's

  DEPENDENCIES: None

                Copyright (c) 2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/api/power/unpa/unpa_remote.h#1 $
$DateTime: 2018/07/30 01:21:36 $
==============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

#include "comdef.h"
#include "unpa.h"

/*==============================================================================
                              TYPES & DEFINITIONS
 =============================================================================*/
typedef int32 unpa_remote_client;
typedef int32 unpa_remote_query_handle;

/*==============================================================================
                            REMOTE GLOBAL FUNCTIONS
 =============================================================================*/
/** 
 * unpaRemote_createClient
 *
 * @brief Create a client to a remote (in rootPD) resource.
 *
 * @param clientName:    Name of this client
 * @param clientType:    Type of this client
 * @param resourceName:  Name of rootPD resource to create a client to.
 *
 * @return A remote client handle for use in subsequent unpa_issue_remote_request
 *         calls or NULL, if creation failed.
 */
#define unpaRemote_createClient(clientName, clientType, resourceName) \
          unpaRemote_createClientEX(clientName, clientType, resourceName, 0, NULL)

unpa_remote_client unpaRemote_createClientEX(const char *clientName,
                                             uint32      clientType,
                                             const char *resourceName,
                                             uint32      clientVal,
                                             void       *clientRef);

/** 
 * unpaRemote_createQueryHandle
 *
 * @brief Create a query handle to a remote (in rootPD) resource.
 *
 * @param resourceName: Name of rootPD resource to create a query handle
 *
 * @return A remote query handle for use in subsequent unpa_remote_query
 *         calls or NULL, if creation failed.
 */
unpa_remote_query_handle unpaRemote_createQueryHandle(const char *resourceName);

/** 
 * unpaRemote_issueRequestEX
 *
 * @brief Issue a request to a remote (in rootPD) resource. If you don't need
 *        request attributes, use the simpler #define form - issueRequest.
 *
 * @param rch:          Remote client handle created using unpaRemote_createClient.
 * @param request:      The request.
 * @param requestAttr:  Request attributes.
 */
#define unpaRemote_issueRequest(remoteClient, request)  \
          unpaRemote_issueRequestEX(remoteClient, request, 0)

void unpaRemote_issueRequestEX(unpa_remote_client  rch, 
                               unpa_resource_state request,
                               uint32              requestAttr);

/** 
 * unpaRemote_query
 *
 * @brief Query a remote (in rootPD) resource
 *
 * @param rqh:    Remote query handle created using unpa_create_remote_query_handle
 * @param id:     The query id.
 * @param result: Pointer to structure to return results in.
 *
 * @return One of the unpa_query_status return codes.
 */
unpa_query_status unpaRemote_query(unpa_remote_query_handle rqh, 
                                   unpa_query_id            id,
                                   unpa_query_result       *result);
                                     
/** 
 * unpaRemote_destroyClient
 *
 * @brief Destroy a remote client
 *
 * @param rqh: Remote client handle created using unpa_create_remote_client
 */
#define unpaRemote_destroyClient(rch) unpaRemote_destroyProxy(rch)

/** 
 * unpaRemote_destroyQueryHandle -->
 *
 * @brief Destroy a remote query handle
 *
 * @param rqh: Remote query handle created using unpa_create_remote_query_handle
 */
#define unpaRemote_destroyQueryHandle(rqh) unpaRemote_destroyProxy(rqh)

/** 
 * unpaRemote_destroyProxy 
 *  
 * @brief Common routine to service destroy operations. Use the 
 *        appropriate #define form above for source clarity.
 */
void unpaRemote_destroyProxy(int32 rh);

#ifdef __cplusplus
}
#endif

#endif /* _UNPA_REMOTE_H_ */
