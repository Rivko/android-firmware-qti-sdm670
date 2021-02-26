/*==============================================================================
  FILE:         unpa_query.c
  
  OVERVIEW:     Query routines in the UNPA framework

  DEPENDENCIES: None

                Copyright (c) 2016 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/unpa/src/framework/unpa_query.c#1 $
$DateTime: 2018/07/30 01:21:36 $
==============================================================================*/
#include <string.h>
#include "CoreVerify.h"
#include "comdef.h"
#include "unpa_internal.h"

/*==============================================================================
                                  FUNCTIONS
 =============================================================================*/
/* 
 * unpa_getQueryHandle
 */
unpa_query_handle unpa_getQueryHandle(const char *resourceName)
{
  CORE_VERIFY_PTR(resourceName);
  return (unpa_query_handle)unpa_getResource(resourceName);
}

/*
 * unpa_getClientQueryHandle
 */
unpa_query_handle unpa_getClientQueryHandle(const char *clientName,
                                            const char *resourceName)
{
  unpa_resource *resource;
  unpa_client   *client = NULL;
  
  CORE_VERIFY_PTR(clientName);
  CORE_VERIFY_PTR(resourceName);
  
  resource = unpa_getResource(resourceName);
  if (resource)
  {
    client = resource->clients;
    while (client)
    {
      if (0 == strncmp(clientName, client->name, UNPA_MAX_NAME_LEN + 1))
      {
        break;
      }
      client = client->next;
    }
  }
  
  return (unpa_query_handle)client;
}

/* 
 * unpa_query
 */
unpa_query_status unpa_query(unpa_query_handle  handle,
                             unpa_query_id      id,
                             unpa_query_result  *result)
{
  unpa_query_status status = UNPA_QUERY_SUCCESS;
  
  CORE_VERIFY_PTR(handle);
  CORE_VERIFY_PTR(result);
  
  switch (id)
  {
    /* Resource queries */
    case UNPA_QUERY_ACTIVE_STATE:
    {
      result->val = ((unpa_resource *)handle)->active_state;
      break;
    }

    case UNPA_QUERY_SLEEP_STATE:
    { 
      result->val = ((unpa_resource *)handle)->sleep_state;
      break;
    }

    case UNPA_QUERY_ACTIVE_MAX:
    {
      result->val = ((unpa_resource *)handle)->active_max;
      break;
    }

    /* Client queries */
    case UNPA_QUERY_ACTIVE_REQUEST:
    {
      result->val = ((unpa_client *)handle)->active_request.val;
      break;
    }

    default:
      status = UNPA_QUERY_UNSUPPORTED;
  }
  
  return status;
}
