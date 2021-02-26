/*==============================================================================
  FILE:         unpa_internal.h
  
  OVERVIEW:     Client routines in the UNPA framework

  DEPENDENCIES: None

                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/unpa/src/framework/unpa_client.c#1 $
$DateTime: 2018/07/30 01:21:36 $
==============================================================================*/
#include <stdlib.h>
#include <string.h>
#include "comdef.h"
#include "CoreVerify.h"
#include "unpa_internal.h"

/*==============================================================================
                                  FUNCTIONS
 =============================================================================*/
/* 
 * unpa_newClient
 */
unpa_client* unpa_newClient(const char  *clientName,
                            uint32       clientType,
                            const char  *resourceName,
                            unpa_client *client)
{
  unpa_resource *resource;
  uint32         supportedTypes;
  
  CORE_VERIFY_PTR(clientName);
  CORE_VERIFY_PTR(resourceName);
  CORE_VERIFY(strlen(clientName) < UNPA_MAX_NAME_LEN);
  
  resource = unpa_getResource(resourceName);
  if (!resource)
  {
    return NULL;
  }
  
  supportedTypes = resource->definition->client_types;

  if (supportedTypes == UNPA_UPDATE_CLIENT_TYPES)
  {
    supportedTypes = unpa_getSupportedClientTypes(resource->definition->update_fcn);
    CORE_VERIFY(supportedTypes != 0x00);
  }
  
  if (!(supportedTypes & clientType))
  { 
    /* The resource does not support clients of the given type */
    return NULL;
  }
  
  if (!client)
  {
    client = (unpa_client *)calloc(1, sizeof(unpa_client));
    CORE_VERIFY_PTR(client);
    client->attr = UNPA_CLIENT_ON_HEAP;
  }
  else
  {
    memset(client, 0, sizeof(unpa_client));
  }
  
  client->name         = clientName;
  client->type         = clientType;
  client->resource     = resource;
  client->request_attr = UNPA_REQUEST_DEFAULT;
  
  UNPA_MUTEX_LOCK(&resource->lock);
  
  client->next      = resource->clients;
  resource->clients = client;
  
  if (unpa_resourceHasAttribute(resource,
                                UNPA_RESOURCE_CLIENT_CHANGE_NOTIFY))
  {
    client->request_attr = UNPA_REQUEST_ADD_CLIENT;
    resource->definition->driver_fcn(resource, client, 0);
    client->request_attr = 0;
  }
  
  UNPA_MUTEX_UNLOCK(&resource->lock);

  return client;  
}
 
/* 
 * unpa_destroyClient
 */
void unpa_destroyClient(unpa_client *client)
{
  unpa_client   *c, *prevc;
  unpa_resource *resource;
  
  CORE_VERIFY_PTR(client);
  CORE_VERIFY_PTR(resource = client->resource);
  
  /* Cancel any active request from client */
  unpa_cancelRequest(client);
  
  UNPA_MUTEX_LOCK(&resource->lock);
  
  for (prevc = NULL, c = client->resource->clients; c != NULL;
       prevc = c, c = c->next )
  {
    if (c == client)
    {
      if (prevc == NULL)
      {
        client->resource->clients = c->next;
      }
      else
      {
        prevc->next = c->next;
      }
      break;
    }
  }
  
  /* Verify that the client was indeed associated with resource and is now unlinked */
  CORE_VERIFY_PTR(c);
  
  if (unpa_resourceHasAttribute(resource,
                                UNPA_RESOURCE_CLIENT_CHANGE_NOTIFY))
  {
    client->request_attr = UNPA_REQUEST_REMOVE_CLIENT;
    resource->definition->driver_fcn(resource, client, 0);
  }
  
  UNPA_MUTEX_UNLOCK(&resource->lock);
  
  /* Ensure clients cannot be used after destroy */
  client->resource = NULL;
  
  if (client->attr & UNPA_CLIENT_ON_HEAP)
  {
    free(client);
  }

  return;
}

/* 
 * unpa_issueRequest
 */
void unpa_issueRequest(unpa_client* client, unpa_resource_state request)
{
  CORE_VERIFY_PTR(client);
  CORE_VERIFY_PTR(client->resource);

  if (request != client->active_request.val)
  {
    client->pending_request.val = request;
    
    //UNPA_LOG( &unpa.log, "issue_request (client: XXX) (type: XXX) (resource: XXX) (request: XXX) (req_attr: XXX)" );
    
    UNPA_MUTEX_LOCK(&client->resource->lock);
    
    /* Update the resource; Resource is unlocked within this routine */
    unpa_updateResource(client);  
  }
  else
  {
    //UNPA_LOG( &unpa.log, "redundant_request (client: XXX) (request: XXX)" );
  }

  client->request_attr = UNPA_REQUEST_DEFAULT;
  return;
}

/* 
 * unpa_cancelRequest 
 */
void unpa_cancelRequest(unpa_client* client)
{
  client->request_attr |= UNPA_REQUEST_DROP_VOTE;
  unpa_issueRequest(client, 0);
  return;
}

/* 
 * unpa_tryIssueRequest 
 */
int32 unpa_tryIssueRequest(unpa_client *client, unpa_resource_state request)
{
  int err = 0;

  CORE_VERIFY_PTR(client);
  CORE_VERIFY_PTR(client->resource);

  if (request != client->active_request.val)
  {
    client->pending_request.val = request;
    
    //UNPA_LOG( &unpa.log, "try_issue_request (client: XXX) (type: XXX) (resource: XXX) (request: XXX) (req_attr: XXX)" );
    
    err = UNPA_MUTEX_TRY_LOCK(&client->resource->lock);
    if (err == 0)
    {
      unpa_updateResource(client);
      /* Resource is unlocked in above routine */
    }
  }
  else
  {
    //UNPA_LOG( &unpa.log, "redundant_request (client: XXX) (request: XXX)" );
  }

  client->request_attr = UNPA_REQUEST_DEFAULT;
  return err;
}

/* 
 * unpa_tryCancelRequest 
 */
int32 unpa_tryCancelRequest(unpa_client* client)
{
  client->request_attr |= UNPA_REQUEST_DROP_VOTE;
  return unpa_tryIssueRequest(client, 0);
}

