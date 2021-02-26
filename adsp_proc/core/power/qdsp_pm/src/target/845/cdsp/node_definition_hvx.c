/*==============================================================================
  FILE:         node_definition_hvx.c

  OVERVIEW:     This file provides the NPA node definition for the
                /CDSPPM/HVX node. This node is used to cotnrol if
                HVX has to be turned-off as part of APCR/Full PC

  DEPENDENCIES: None

                Copyright (c) 2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/qdsp_pm/src/target/845/cdsp/node_definition_hvx.c#1 $
$DateTime: 2017/07/21 22:10:47 $
==============================================================================*/
#include "comdef.h"
#include "npa.h"
#include "npa_resource.h"
#include "sleep_lpr.h"
#include "CoreVerify.h"
#include "sleep_npa.h"
//#include "sleep_npai.h"

/*==============================================================================
                      INTERNAL FUNCTION DECLARATIONS
 =============================================================================*/
/**
 * CdspHvxNode_driver
 *
 * @brief Driver function for the cdsppm hvx node. This function gets called
 *        every time a client request is made that changes the state of the node.
 *
 * @param resource: The NPA resource being requested.
 * @param client:   Pointer to the client making the request.
 * @param state:    New state of the resource.
 *
 * @return New state of the resource.
 */
static npa_resource_state CdspHvxNode_driver(npa_resource       *resource,
                                             npa_client         *client,
                                             npa_resource_state state )
{
  /* Get the Cdsppm Hvx resources's client handle to the sleep LPR node. */
  npa_client_handle lprClient = NPA_DEPENDENCY(resource, 0);

  /* At init time, register the LPR with the sleep LPR node */
  if(client->type == NPA_CLIENT_INITIALIZE)
  {
    sleepLPR_define("HVX", lprClient);
  }

  /* XML mode order:
   * 
   * SLEEP_LPRM_NUM(0) -- "off"
   * SLEEP_LPRM_NUM(1) -- "nop"
   */
  if(1 == state)
  {
    /* Some client is requesting hvx to be turned off */
    npa_issue_required_request(lprClient, SLEEP_LPRM_NUM(0));
  }
  else
  {
    /* No need to touch HVX node, as it is not enabled/turned-on */
    npa_issue_required_request(lprClient, SLEEP_LPRM_NUM(1));
  }

  return state;
}

/*==============================================================================
                               NODE DEFINITION
 =============================================================================*/
/* The dependency array for the CdspHvx node.  This node depends
 * on the sleep LPR node, as it makes requests to that node when the state
 * of this node changes. */
static npa_node_dependency g_CdspHvxDependency[1] =
{
  {
    SLEEP_LPR_NODE_NAME,
    NPA_CLIENT_REQUIRED,
  },
};

/* Resource definition for the /hvx resource. */
static npa_resource_definition g_CdspHvxResource[] =
{
  {
    "/CDSPPM/HVX/LPR",                /* Name */
    "on/off",                     /* Units */
    1, /* Max State */
    &npa_max_plugin,              /* Plugin */
    NPA_RESOURCE_DEFAULT,         /* Attributes */
    NULL,                         /* User Data */
  }
};

/* Node definition for the /hvx resource. */
npa_node_definition g_CdspHvxNode =
{
  "/CDSPPM/HVX",                                        /* Name */
  CdspHvxNode_driver,                                   /* Driver_fcn */
  NPA_NODE_DEFAULT,                                     /* Attributes */
  NULL,                                                 /* Data */
  NPA_ARRAY(g_CdspHvxDependency),                       /* Dependencies */
  NPA_ARRAY(g_CdspHvxResource)                          /* Resources */
};

