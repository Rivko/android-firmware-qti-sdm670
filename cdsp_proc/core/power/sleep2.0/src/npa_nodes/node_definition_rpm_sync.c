/*============================================================================
  FILE:         node_definition_rpm_sync.c

  OVERVIEW:     This file provides the NPA node definition for the
                /core/cpu/rpm_sync node.

  DEPENDENCIES: None

                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
============================================================================*/
#include "comdef.h"
#include "npa.h"
#include "npa_resource.h"
#include "sleep_lpr.h"
#include "sleep_npa.h"
#include "sleep_npai.h"

/*==============================================================================
                       INTERNAL FUNCTION DECLARATIONS
 =============================================================================*/
/**
 * rpmSyncNode_driver
 * 
 * @brief Driver function for the rpm_sync node.  This function gets called
 *        every time a client request is made that changes the state of the node.
 *
 * @param resource: The NPA resource being requested.
 * @param client:   Pointer to the client making the request.
 * @param state:    New state of the resource.
 *
 * @return New state of the resource.
 *
*/
static npa_resource_state rpmSyncNode_driver(npa_resource        *resource,
                                             npa_client          *client,
                                             npa_resource_state  state )
{
  /* Get the node's client handle to the sleep LPR node. */
  npa_client_handle lprClient = NPA_DEPENDENCY(resource, 0);

  /* At init time, register the LPR with the sleep LPR node, and request
   * the rpm LPR */
  if(client->type == NPA_CLIENT_INITIALIZE)
  {
     sleepLPR_define("rpmh", lprClient);
  }

  /* Ordered LPRM list:
   * RPM LPRM 0 : "sync" */
  if(SLEEP_RPM_LPR_DISABLE_ALL == state)
  {
    /* Request "/sleep/lpr" node to disable all LPRMs for rpm LPR */
    npa_issue_required_request(lprClient, 0);
  }
  else
  {
    /* If no other state, assume enable all - 
     * Aggregated zero request enables all the LPRMs for rpm */ 
    npa_issue_required_request(lprClient, SLEEP_ALL_LPRMS);
  }

  return state;
}

/*==============================================================================
                             NODE DEFINITION
 =============================================================================*/
/* The dependency array for the rpm_sync node.  This node depends
 * on the sleep LPR node, as it makes requests to that node when the state
 * of this node changes. */
static npa_node_dependency g_rpmSyncDependency[1] =
{
  {
    SLEEP_LPR_NODE_NAME,
    NPA_CLIENT_REQUIRED,
  },
};

/* Resource definition for the /core/cpu/rpm_sync resource. */
static npa_resource_definition g_rpmSyncResource[] =
{
  {
    SLEEP_RPM_LPR_NODE_NAME,        /* Name */
    "disable/enable",               /* Units */
    SLEEP_RPM_LPR_MAX_STATE,        /* Max State */
    &npa_max_plugin,                /* Plugin */
    NPA_RESOURCE_DEFAULT,           /* Attributes */
    NULL,                           /* User Data */
    NULL                            /* Query Function */
  }
};

/**
 *
 * @brief sleep_rpm_sync_node
 *
 * Node definition for the /core/cpu/rpm_sync resource.
 *
*/
npa_node_definition g_rpmNode =
{
  SLEEP_LPR_CLIENT_NODE(SLEEP_RPM_LPR_NODE_NAME), /* Name */
  rpmSyncNode_driver,                             /* Driver_fcn */
  NPA_NODE_DEFAULT,                               /* Attributes */
  NULL,                                           /* Data */
  NPA_ARRAY(g_rpmSyncDependency),                 /* Dependencies */
  NPA_ARRAY(g_rpmSyncResource)                    /* Resources */
};

