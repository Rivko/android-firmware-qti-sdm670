/*==============================================================================
  FILE:         node_definition_cpu_vdd.c

  OVERVIEW:     This file provides the NPA node definition for the
                /core/cpu/vdd node.
 
  DEPENDENCIES: None

                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/sleep2.0/src/npa_nodes/node_definition_cpu_vdd.c#1 $
$DateTime: 2018/07/30 01:21:36 $
==============================================================================*/
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
 * cpuVddNode_driver
 *
 * @brief Driver function for the cpu_vdd node.  This function gets called
 *        every time a client request is made that changes the state of the node.
 *
 * @param resource: The NPA resource being requested.
 * @param client:   Pointer to the client making the request.
 * @param state:    New state of the resource.
 *
 * @return New state of the resource.
 */
static npa_resource_state cpuVddNode_driver(npa_resource        *resource,
                                            npa_client          *client,
                                            npa_resource_state  state)
{
  /* Get the modem_core_rail node's client handle to the sleep LPR node. */
  npa_client_handle lprClient = NPA_DEPENDENCY( resource, 0 );

  /* At init time, register the LPR with the sleep LPR node, and request
   * a bit mask to use for the MODEM_CORE_RAIL LPRMs from the sleep LPR node. */
  if(client->type == NPA_CLIENT_INITIALIZE)
  {
     sleepLPR_define("cpu_vdd", lprClient);
  }

  if(SLEEP_CPUVDD_LPR_DISABLE_ALL == state)
  {
     /* SLEEP_LPRM_NUM(0) -- "full_pc"
      * SLEEP_LPRM_NUM(1) -- "apcr pll lpm with sleep set" (for slpi only)
      * SLEEP_LPRM_NUM(2) -- "apcr pll lpm"
      * SLEEP_LPRM_NUM(3) -- "apcr"
      *
      * Corresponds to the LPRMs in the "cpu_vdd" lpr xml file that
      * allow cpu_vdd power rail to be collapsed or allow BHS to be turned off
      * Disable above modes when clients request this resource with this state
      * 
      * Also be aware that uSleep entry ties into this node, if more complex
      * voting is needed, it must also be taken into consideration */ 
    npa_issue_required_request(lprClient, 0);
  }
  else
  {
    /* SLEEP_ALL_LPRMS -- no modes depend on cpu_vdd power rail 
     * Issue a request to the sleep node to enable this node's LPRMs
     * during sleep. */
    npa_issue_required_request(lprClient, SLEEP_ALL_LPRMS);
  }

  return state;
}

/*==============================================================================
                             NODE DEFINITION
 =============================================================================*/
/* The dependency array for the cpu_vdd node.  This node depends
 * on the sleep LPR node, as it makes requests to that node when the state
 * of this node changes. */
static npa_node_dependency g_cpuVddDependency[1] =
{
  {
    SLEEP_LPR_NODE_NAME,
    NPA_CLIENT_REQUIRED,
  },
};

/* Resource definition for the /core/cpu/vdd resource. */
static npa_resource_definition g_cpuVddResource[] =
{
  {
    SLEEP_CPUVDD_LPR_NODE_NAME,       /* Name */
    "state",                          /* Units */
    SLEEP_CPUVDD_LPR_MAX_STATE,       /* Max State */
    &npa_max_plugin,                  /* Plugin */
    NPA_RESOURCE_DEFAULT,             /* Attributes */
    NULL,                             /* User Data */
    NULL,                             /* Query Function */
    NULL                              /* Link Query Function */
  }
};

/* Node definition for the /core/cpu/vdd resource. */
npa_node_definition g_cpuVddNode =
{
  SLEEP_LPR_CLIENT_NODE(SLEEP_CPUVDD_LPR_NODE_NAME),  /* Name */
  cpuVddNode_driver,                                  /* Driver_fcn */
  NPA_NODE_DEFAULT,                                   /* Attributes */
  NULL,                                               /* Data */
  NPA_ARRAY(g_cpuVddDependency),                      /* Dependencies */
  NPA_ARRAY(g_cpuVddResource)                         /* Resources */
};

