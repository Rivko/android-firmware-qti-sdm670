/*==============================================================================
  FILE:         max_duration_node.c
  
  OVERVIEW:     This file provides the max duration NPA node definitions

  DEPENDENCIES: None

                Copyright (c) 2016 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/sleep2.0/src/npa_nodes/max_duration_node.c#1 $
$DateTime: 2017/07/21 22:10:47 $
==============================================================================*/
#ifndef FEATURE_SLEEP_NO_MAX_DURATION_NODE

#include "comdef.h"
#include "CoreVerify.h"
#include "sleepi.h"
#include "sleep_npa.h"
#include "npa_log.h"
#include "npa_resource.h"
#include "npa_remote_resource.h"
#include "sleep_utils.h"
#include "sleepActive.h"

/*==============================================================================
                            INTERNAL FUNCTIONS
 =============================================================================*/
/**
 * sleepMaxDurNode_createClientFcn
 *
 * @brief In order to specify to driver function the untis chosen 
 *        by the client to this resource
 *
 * Custom client function to determine units specified by client
 *
 * @param client: The handle to the client registered to the 
 *                resource.
 *
 * @return None
 */
static void sleepMaxDurNode_createClientFcn(npa_client *client)
{
  if(0 == strncmp(client->resource_name, SLEEP_USEC_MAX_DURATION_NODE_NAME, 
                  strlen(SLEEP_USEC_MAX_DURATION_NODE_NAME)))
  {
    /* client data in usec */
    client->resource_data = (void *)1;
  }
  else
  {
    /* client data in ticks */
    client->resource_data = (void *)0;
  }

  return;
}

/**
 * sleepMaxDurNode_update
 *
 * @brief This function is invoked by the /sleep/max_duration resource
 *        when a client request is made.
 *
 * Determines the new state of the resource by calculating max of the
 * clients' requests 
 *
 * @param resource: The NPA resource being requested (should be 
 *                  /sleep/max_duration).
 * @param client: The handle to the clients registered to the 
 *                resource.
 *
 * @return Returns the new state of the resource.
 *
 */
static npa_resource_state sleepMaxDurNode_update(npa_resource       *resource, 
                                                 npa_client_handle  client)
{
  npa_resource_state        request;
  static npa_resource_state req_state = 0;

  CORE_VERIFY_PTR(resource);
  CORE_VERIFY_PTR(client);

  if(client->resource_data == (void *)1)
  {
    /* convert usec to ticks */
    request = NPA_PENDING_REQUEST(client).state;
    NPA_PENDING_REQUEST(client).state = US_TO_TICKS(request);
  }

  /* Client is requesting a specific point at which we should be awake.
   * Take the min of the client votes.  This will return the min of the
   * required and suppressible clients. */
  req_state = npa_min_plugin.update_fcn(resource, client);

  return req_state;
}

/**
 * sleepNPA_maxDurationDriver
 *
 * @brief Driver function for the /sleep/max_duration resource 
 *        that updates its state.
 *
 * @param resource: Pointer to the /sleep/max_duration resource
 * @param client: Client of the resource
 * @param state: State of the resource.
 *
 * @return state of the resource.
 */
static npa_resource_state sleepMaxDurNode_driver(npa_resource       *resource,
                                                 npa_client         *client,
                                                 npa_resource_state state)
{
  /* Record the state so it can be queried from the sleep task before we go
   * to sleep. */
  if(0 == state)
  {  
    state = NPA_MAX_STATE;
  }

  /* There is a race condition where ATS may finish the query before NPA 
   * framework can process the return vlaue from driver causing ATS to use 
   * stale values. Until NPA has support for synchronous POST_CHANGE 
   * callbacks, update the final calculated state in separate field 
   * as recommended by NPA team - use this field for query purpose */
  resource->definition->data = (npa_user_data)state;

  /* Trigger Active Timer Solver -- max_duration is updated */
  sleepActive_SetSignal(SLEEP_SIGNAL_SOFT_DURATION);
  
  return state;
}

/**
 * sleepNPA_maxDurationQuery
 *
 * @brief Query function for the /sleep/max_duration resource that returns
 *        soft wakeup time
 *
 * @param resourceLink: Resource to which we are making request (max_duration node)
 * @param queryID:      ID for the query being made.
 * @param queryResult:  Stores to the result of the query
 *
 * @return Returns the status for the query (i.e. successfully handled or 
 *         unsupported).
 */
static npa_query_status sleepMaxDurNode_query(npa_link        *resourceLink,
                                              unsigned int    queryID, 
                                              npa_query_type  *queryResult)
{
  npa_resource_state  retVal;
  npa_query_status    status = NPA_QUERY_SUCCESS;

  CORE_VERIFY_PTR(resourceLink);
  CORE_VERIFY_PTR(queryResult);

  switch(queryID)
  {
    case NPA_QUERY_CURRENT_STATE:
    case SLEEP_MAX_DURATION_QUERY_TYPE:
    {
      retVal = (npa_resource_state)resourceLink->resource->definition->data;

      if(0 == strncmp(resourceLink->name, SLEEP_USEC_MAX_DURATION_NODE_NAME, 
                      strlen(SLEEP_USEC_MAX_DURATION_NODE_NAME)))
      {
        /* client request in usec, so return in usec */
        retVal = TICKS_TO_US(retVal);
      }

      queryResult->data.value = retVal;
      queryResult->type = NPA_QUERY_TYPE_VALUE;
      break;
    }

    default:
    {
      status = NPA_QUERY_UNSUPPORTED_QUERY_ID;
      break;
    }
  }

  return status;
}

/*==============================================================================
                            MAX DURATION NODE DEFINITION
 =============================================================================*/
/* The plugin for the /sleep/max_duration resource. */
const npa_resource_plugin g_sleepMaxDurationPlugin = 
{
  sleepMaxDurNode_update,           /* Update function */
  NPA_CLIENT_REQUIRED,              /* Supported client types */
  sleepMaxDurNode_createClientFcn,  /* Create client function */
  NULL                              /* Destroy client function */
};

/* Definition of the max duration resource. */
static npa_resource_definition g_sleepMaxDurationResource[] = 
{ 
  {  
    SLEEP_MAX_DURATION_NODE_NAME,             /* Name */
    "ticks",                                  /* Units - usec for alias */
    NPA_MAX_STATE,                            /* Max State */
    &g_sleepMaxDurationPlugin,                /* Plugin */
    NPA_RESOURCE_INTERPROCESS_ACCESS_ALLOWED, /* Attributes */
    NULL,                                     /* User Data */
    NULL,                                     /* Query Function */
    sleepMaxDurNode_query                     /* Query Link Function */
  }
};

/* Definition of the max duration node. */
static npa_node_definition g_sleepMaxDurationNode = 
{ 
  "/node/sleep/max_duration",           /* Name */
  sleepMaxDurNode_driver,               /* Driver_fcn */
  NPA_NODE_DEFAULT,                     /* Attributes */
  NULL,                                 /* Data */
  NPA_EMPTY_ARRAY,                      /* Dependencies */
  NPA_ARRAY(g_sleepMaxDurationResource) /* Resource Array */
};

#endif /* !FEATURE_SLEEP_NO_MAX_DURATION_NODE */

/*==============================================================================
                              EXTERNAL FUNCTIONS
 =============================================================================*/
void sleepMaxDurNode_define(void)
{
#ifndef FEATURE_SLEEP_NO_MAX_DURATION_NODE
  npa_resource_state initial_state[] = {NPA_MAX_STATE};

  npa_define_node(&g_sleepMaxDurationNode, initial_state, NULL);
  npa_alias_resource_cb(SLEEP_MAX_DURATION_NODE_NAME, SLEEP_USEC_MAX_DURATION_NODE_NAME, 
                        NULL, NULL);

  npa_remote_publish_resource(NPA_ALL_PDS, SLEEP_MAX_DURATION_NODE_NAME);
  npa_remote_publish_resource(NPA_ALL_PDS, SLEEP_USEC_MAX_DURATION_NODE_NAME);

  /* Log requests to "max_duration" resource in "Sleep Requests" */
  CORE_VERIFY(NPA_SUCCESS == (npa_add_resource_log_by_handle(
                              "Sleep Requests", 
                              g_sleepMaxDurationResource[0].handle)));
#endif

  return;
}

