/*============================================================================
  FILE:         node_definition_rscp.c

  OVERVIEW:     This file provides the NPA node definition for the
                RSCp LPR node. 

  DEPENDENCIES: None

                Copyright (c) 2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
==============================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/sleep2.0/src/npa_nodes/node_definition_rscp.c#1 $
$DateTime: 2018/07/30 01:21:36 $
============================================================================*/
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
 * rscpNode_driver
 *
 * @brief Driver function for the RSCp node. This function gets called
 *        every time a client request is made that changes the state of the node.
 *
 * @param resource: The NPA resource being requested.
 * @param client:   Pointer to the client making the request.
 * @param state:    New state of the resource.
 *
 * @return: New state of the resource.
 */
static npa_resource_state rscpNode_driver(npa_resource        *resource,
                                          npa_client          *client,
                                          npa_resource_state  state)
{
  npa_client_handle lprClient = NPA_DEPENDENCY(resource, 0);

  /* At init time, register the LPR with the sleep LPR node */
  if(client->type == NPA_CLIENT_INITIALIZE)
  {
    sleepLPR_define("RSCp", lprClient);
  }

  /* XML mode order:
   * 
   * SLEEP_LPRM_NUM(0) -- "chip_sleep"
   */
  if(SLEEP_RSCP_LPR_DISABLE_ALL == state)
  {
    npa_issue_required_request(lprClient, 0);
  }
  else
  {
    npa_issue_required_request(lprClient, SLEEP_ALL_LPRMS);
  }
  
  return state;
}

/*==============================================================================
                             NODE DEFINITION
 =============================================================================*/
/* The dependency array for the RSCp node.  This node depends
 * on the sleep LPR node, as it makes requests to that node when the state
 * of this node changes. */
static npa_node_dependency g_rscpDependency[1] =
{
  {
    SLEEP_LPR_NODE_NAME,
    NPA_CLIENT_REQUIRED,
  },
};

/* Resource definition for the RSCp node resource. */
static npa_resource_definition g_rscpResource[] =
{
  {
    SLEEP_RSCP_LPR_NODE_NAME, /* Name */
    "Enable/Disable",         /* Units */
    SLEEP_RSCP_LPR_MAX_STATE, /* Max State */
    &npa_max_plugin,          /* Plugin */
    NPA_RESOURCE_DEFAULT,     /* Attributes */
    NULL,                     /* User Data */
  }
};

/* Node definition for the RSCp resource. */
npa_node_definition g_rscpNode =
{
  SLEEP_LPR_CLIENT_NODE(SLEEP_RSCP_LPR_NODE_NAME),  /* Name */
  rscpNode_driver,                                  /* Driver_fcn */
  NPA_NODE_DEFAULT,                                 /* Attributes */
  NULL,                                             /* Data */
  NPA_ARRAY(g_rscpDependency),                       /* Dependencies */
  NPA_ARRAY(g_rscpResource)                          /* Resources */
};

