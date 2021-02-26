/*============================================================================
  FILE:         node_definition_pdc.c

  OVERVIEW:     This file provides the NPA node definition for the
                PDC LPR node. 

  DEPENDENCIES: None

                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
==============================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/sleep2.0/src/npa_nodes/node_definition_pdc.c#1 $
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
 * pdcNode_driver
 *
 * @brief Driver function for the PDC node. This function gets called
 *        every time a client request is made that changes the state of the node.
 *
 * @param resource: The NPA resource being requested.
 * @param client:   Pointer to the client making the request.
 * @param state:    New state of the resource.
 *
 * @return: New state of the resource.
 */
static npa_resource_state pdcNode_driver(npa_resource        *resource,
                                         npa_client          *client,
                                         npa_resource_state  state)
{
  npa_client_handle lprClient = NPA_DEPENDENCY(resource, 0);

  /* At init time, register the LPR with the sleep LPR node */
  if(client->type == NPA_CLIENT_INITIALIZE)
  {
    sleepLPR_define("PDC", lprClient);
  }

  /* XML mode order:
   * 
   * SLEEP_LPRM_NUM(0) -- "CX_RET"
   * SLEEP_LPRM_NUM(1) -- "CX_OFF" */
   
  /* State is set to disable, so reverse logic to send to lpr node */
  npa_issue_required_request(lprClient, ~state); 
  
  return state;
}

/*==============================================================================
                             NODE DEFINITION
 =============================================================================*/
/* The dependency array for the PDC node.  This node depends
 * on the sleep LPR node, as it makes requests to that node when the state
 * of this node changes. */
static npa_node_dependency g_pdcDependency[1] =
{
  {
    SLEEP_LPR_NODE_NAME,
    NPA_CLIENT_REQUIRED,
  },
};

/* Resource definition for the pdc node resource. */
static npa_resource_definition g_pdcResource[] =
{
  {
    SLEEP_PDC_LPR_NODE_NAME,  /* Name */
    "BitWise Disable",        /* Units */
    SLEEP_PDC_LPR_MAX_STATE,  /* Max State */
    &npa_or_plugin,           /* Plugin */
    NPA_RESOURCE_DEFAULT,     /* Attributes */
    NULL,                     /* User Data */
  }
};

/* Node definition for the PDC resource. */
npa_node_definition g_pdcNode =
{
  SLEEP_LPR_CLIENT_NODE(SLEEP_PDC_LPR_NODE_NAME), /* Name */
  pdcNode_driver,                                 /* Driver_fcn */
  NPA_NODE_DEFAULT,                               /* Attributes */
  NULL,                                           /* Data */
  NPA_ARRAY(g_pdcDependency),                     /* Dependencies */
  NPA_ARRAY(g_pdcResource)                        /* Resources */
};

