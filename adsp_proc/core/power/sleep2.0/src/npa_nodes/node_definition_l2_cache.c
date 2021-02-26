/*==============================================================================
  FILE:         node_definition_l2_cache.c

  OVERVIEW:     This file provides the NPA node definition for the
                /core/cpu/l2cache node. This node is used to cotnrol if
                target will do power collapse with l2 retention or
                no retention.

  DEPENDENCIES: None

                Copyright (c) 2016 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/sleep2.0/src/npa_nodes/node_definition_l2_cache.c#1 $
$DateTime: 2017/07/21 22:10:47 $
==============================================================================*/
#include "comdef.h"
#include "npa.h"
#include "npa_resource.h"
#include "sleep_lpr.h"
#include "CoreVerify.h"
#include "sleep_npa.h"
#include "sleep_npai.h"

/*==============================================================================
                      INTERNAL FUNCTION DECLARATIONS
 =============================================================================*/
/**
 * l2cacheNode_driver
 *
 * @brief Driver function for the l2cache node. This function gets called
 *        every time a client request is made that changes the state of the node.
 *
 * @param resource: The NPA resource being requested.
 * @param client:   Pointer to the client making the request.
 * @param state:    New state of the resource.
 *
 * @return New state of the resource.
 */
static npa_resource_state l2cacheNode_driver(npa_resource       *resource,
                                             npa_client         *client,
                                             npa_resource_state state )
{
  /* Get the L2 cache resources's client handle to the sleep LPR node. */
  npa_client_handle lprClient = NPA_DEPENDENCY(resource, 0);

  /* At init time, register the LPR with the sleep LPR node */
  if(client->type == NPA_CLIENT_INITIALIZE)
  {
    sleepLPR_define("l2", lprClient);
  }

  /* XML mode order:
   * 
   * SLEEP_LPRM_NUM(0) -- "noret"
   * SLEEP_LPRM_NUM(1) -- "ret"
   */
  if(SLEEP_L2_CACHE_LPR_RETENTION_ONLY == state)
  {
    /* Some client is requesting L2 to be on i.e. power collapse with l2 retention
     * mode. */
    npa_issue_required_request(lprClient, SLEEP_LPRM_NUM(1));
  }
  else
  {
    /* We can enter in to non-retention mode if we have enough time */
    npa_issue_required_request(lprClient, SLEEP_ALL_LPRMS);
  }

  return state;
}

/*==============================================================================
                               NODE DEFINITION
 =============================================================================*/
/* The dependency array for the l2cache node.  This node depends
 * on the sleep LPR node, as it makes requests to that node when the state
 * of this node changes. */
static npa_node_dependency g_l2CacheDependency[1] =
{
  {
    SLEEP_LPR_NODE_NAME,
    NPA_CLIENT_REQUIRED,
  },
};

/* Resource definition for the /core/cpu/l2cache resource. */
static npa_resource_definition g_l2CacheResource[] =
{
  {
    SLEEP_L2_CACHE_LPR_NODE_NAME, /* Name */
    "on/off",                     /* Units */
    SLEEP_L2_CACHE_LPR_MAX_STATE, /* Max State */
    &npa_binary_plugin,           /* Plugin */
    NPA_RESOURCE_DEFAULT,         /* Attributes */
    NULL,                         /* User Data */
  }
};

/* Node definition for the /core/cpu/l2cache resource. */
npa_node_definition g_l2CacheNode =
{
  SLEEP_LPR_CLIENT_NODE(SLEEP_L2_CACHE_LPR_NODE_NAME),  /* Name */
  l2cacheNode_driver,                                   /* Driver_fcn */
  NPA_NODE_DEFAULT,                                     /* Attributes */
  NULL,                                                 /* Data */
  NPA_ARRAY(g_l2CacheDependency),                       /* Dependencies */
  NPA_ARRAY(g_l2CacheResource)                          /* Resources */
};

