/*==============================================================================
  FILE:         latency_node.c
  
  OVERVIEW:     This file provides the latency NPA node definitions

  DEPENDENCIES: None

                Copyright (c) 2016 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/sleep2.0/src/npa_nodes/latency_node.c#2 $
$DateTime: 2017/11/16 08:24:49 $
==============================================================================*/
#ifndef FEATURE_SLEEP_NO_LATENCY_NODE

#include "comdef.h"
#include "CoreVerify.h"
#include "sleepi.h"
#include "sleep_npa.h"
#include "npa_resource.h"
#include "npa_remote_resource.h"
#include "npa_log.h"
#include "sleep_utils.h"
#include "sleepActive.h"

/*==============================================================================
                              INTERNAL FUNCTIONS
 =============================================================================*/
/**
 * sleepLatencyNode_changeEventCB
 *
 * @brief Callback used to trigger ATS when the latency nodes are updated
 *
 * @param See NPA header prototype
 */
static void sleepLatencyNode_changeEventCB(void          *userData,
                                           unsigned int  eventType,
                                           void          *eventData,
                                           unsigned int  size)
{
  /* Trigger Active Timer Solver -- latency is updated */
  sleepActive_SetSignal((sleepActive_SignalType)userData);
  return;
}
 
/**
 * sleepLatencyNode_driver
 *
 * @brief Common driver function for the various latency node resource that updates it.
 *
 * @param resource: Pointer to the resource
 * @param client: Client of this resource
 * @param state: Resource state as aggregated by the Update function
 *
 * @return The state of the resource if set else NPA_MAX_STATE
 */
static npa_resource_state sleepLatencyNode_driver(npa_resource       *resource,
                                                  npa_client         *client,
                                                  npa_resource_state state)
{
  /* Zero latency is interpreted as no restriction, so set value to max */
  if(0 == state)
  {  
    state = NPA_MAX_STATE;
  }

  return state;
}

/**
 * sleepLatencyuSecNode_driver
 *
 * @brief Driver function for the /core/cpu/latency resource that updates it.
 *
 * @param resource: Pointer to the resource
 * @param client: Client of this resource
 * @param state: New state for the resource
 *
 * @return The state of the resource if set else NPA_MAX_STATE
*/
static npa_resource_state sleepLatencyuSecNode_driver(npa_resource        *resource,
                                                      npa_client          *client,
                                                      npa_resource_state  state)
{
  npa_client_handle clientHandle = NPA_DEPENDENCY(resource, 0);

  /* Convert the request to ticks and forward it to the corresponding
   * base latency resource */
  npa_issue_required_request(clientHandle, US_TO_TICKS(state));

  return state;
}

/*==============================================================================
                        LATENCY NODE DEFINITION (TICKS)
 =============================================================================*/
/* Definition of the (ticks) latency resource. */
static npa_resource_definition g_sleepNormalLatencyResource[] = 
{ 
  {  
    SLEEP_NORMAL_LATENCY_NODE_NAME,             /* Name */
    "ticks",                                    /* Units */
    NPA_MAX_STATE,                              /* Max State */
    &npa_min_plugin,                            /* Plugin */
    NPA_RESOURCE_INTERPROCESS_ACCESS_ALLOWED |  /* Attributes */
    NPA_RESOURCE_ENABLE_PREPOST_CHANGE_EVENTS,
    NULL,                                       /* User Data */
    NULL,                                       /* Query Function */
    NULL                                        /* Query Link Function */
  }
};

/* Definition of the (ticks) latency node. */
static npa_node_definition g_sleepNormalLatencyNode = 
{ 
  "/node/core/cpu/latency",                 /* name */
  sleepLatencyNode_driver,                  /* driver_fcn */
  NPA_NODE_DEFAULT,                         /* attributes */
  NULL,                                     /* data */
  NPA_EMPTY_ARRAY,                          /* dependencies */
  NPA_ARRAY(g_sleepNormalLatencyResource)   /* resources */
};

#ifdef USLEEP_ISLAND_MODE_ENABLE
static npa_resource_definition g_sleepIslandLatencyResource[] = 
{ 
  {  
    SLEEP_ISLAND_LATENCY_NODE_NAME,             /* Name */
    "ticks",                                    /* Units */
    NPA_MAX_STATE,                              /* Max State */
    &npa_min_plugin,                            /* Plugin */
    NPA_RESOURCE_INTERPROCESS_ACCESS_ALLOWED |  /* Attributes */
    NPA_RESOURCE_ENABLE_PREPOST_CHANGE_EVENTS,
    NULL,                                       /* User Data */
    NULL,                                       /* Query Function */
    NULL                                        /* Query Link Function */
  }
};

static npa_node_definition g_sleepIslandLatencyNode = 
{ 
  "/node/core/cpu/island/latency",          /* name */
  sleepLatencyNode_driver,                  /* driver_fcn */
  NPA_NODE_DEFAULT,                         /* attributes */
  NULL,                                     /* data */
  NPA_EMPTY_ARRAY,                          /* dependencies */
  NPA_ARRAY(g_sleepIslandLatencyResource)   /* resources */
};

#endif /* USLEEP_ISLAND_MODE_ENABLE */

/*==============================================================================
                      LATENCY NODE DEFINITION (uSec)
 =============================================================================*/
/* Node dependencies for the uSec latency node */
static npa_node_dependency g_sleepNormalUSecLatencyDependencies[1] =
{
  {
    SLEEP_NORMAL_LATENCY_NODE_NAME,
    NPA_CLIENT_REQUIRED,
  },
};

/* Definition of the (uSec) latency resource. */
static npa_resource_definition g_sleepNormalUSecLatencyResource[] = 
{ 
  {  
    SLEEP_USEC_NORMAL_LATENCY_NODE_NAME,        /* Name */
    "uSec",                                     /* Units */
    NPA_MAX_STATE,                              /* Max State */
    &npa_min_plugin,                            /* Plugin */
    NPA_RESOURCE_INTERPROCESS_ACCESS_ALLOWED,   /* Attributes */
    NULL,                                       /* User Data */
    NULL,                                       /* Query Function */
    NULL                                        /* Query Link Function */
  }
};

/* Definition of the (uSec) latency node. */
static npa_node_definition g_sleepNormalUSecLatencyNode = 
{ 
  "/node/core/cpu/latency/usec",                  /* name */
  sleepLatencyuSecNode_driver,                    /* driver_fcn */
  NPA_NODE_DEFAULT,                               /* attributes */
  NULL,                                           /* data */
  NPA_ARRAY(g_sleepNormalUSecLatencyDependencies),/* dependencies */
  NPA_ARRAY(g_sleepNormalUSecLatencyResource)     /* resources */
};

#ifdef USLEEP_ISLAND_MODE_ENABLE
static npa_node_dependency g_sleepIslandUSecLatencyDependencies[1] =
{
  {
    SLEEP_ISLAND_LATENCY_NODE_NAME,
    NPA_CLIENT_REQUIRED,
  },
};

static npa_resource_definition g_sleepIslandUSecLatencyResource[] = 
{ 
  {  
    SLEEP_USEC_ISLAND_LATENCY_NODE_NAME,        /* Name */
    "uSec",                                     /* Units */
    NPA_MAX_STATE,                              /* Max State */
    &npa_min_plugin,                            /* Plugin */
    NPA_RESOURCE_INTERPROCESS_ACCESS_ALLOWED,   /* Attributes */
    NULL,                                       /* User Data */
    NULL,                                       /* Query Function */
    NULL                                        /* Query Link Function */
  }
};

static npa_node_definition g_sleepIslandUSecLatencyNode = 
{ 
  "/node/core/cpu/island/latency/usec",           /* name */
  sleepLatencyuSecNode_driver,                    /* driver_fcn */
  NPA_NODE_DEFAULT,                               /* attributes */
  NULL,                                           /* data */
  NPA_ARRAY(g_sleepIslandUSecLatencyDependencies),/* dependencies */
  NPA_ARRAY(g_sleepIslandUSecLatencyResource)     /* resources */
};
#endif /* USLEEP_ISLAND_MODE_ENABLE */

#endif /* !FEATURE_SLEEP_NO_LATENCY_NODE */

/*==============================================================================
                              EXTERNAL FUNCTIONS
 =============================================================================*/
/*
 * sleepLatencyNode_define
 */
void sleepLatencyNode_define(void)
{
#ifndef FEATURE_SLEEP_NO_LATENCY_NODE
  npa_resource_state  initial_state = 0;
  npa_event_handle    normalLatencyEventHandle;
  
  /* Register the latency node with npa framework */
  npa_define_node(&g_sleepNormalLatencyNode, &initial_state, NULL); 
  npa_remote_publish_resource(NPA_ALL_PDS, SLEEP_NORMAL_LATENCY_NODE_NAME);

  /* Add "latency" to the list of resources logged in "Sleep Requests" */
  CORE_VERIFY(NPA_SUCCESS == (npa_add_resource_log_by_handle("Sleep Requests",
                                                             g_sleepNormalLatencyResource->handle)));

  normalLatencyEventHandle = npa_create_custom_event(SLEEP_NORMAL_LATENCY_NODE_NAME,
                                                     "normalLatencyEvent",
                                                     NPA_TRIGGER_POST_CHANGE_EVENT,
                                                     NULL,
                                                     sleepLatencyNode_changeEventCB,
                                                     (void*)SLEEP_SIGNAL_NORMAL_LATENCY);

  /* Register the uSec latency node */
  npa_define_node(&g_sleepNormalUSecLatencyNode, NULL, NULL);
  npa_remote_publish_resource(NPA_ALL_PDS, SLEEP_USEC_NORMAL_LATENCY_NODE_NAME);
                                                             
  CORE_VERIFY(NPA_SUCCESS == (npa_add_resource_log_by_handle("Sleep Requests",
                                                             g_sleepNormalUSecLatencyResource->handle)));

#ifdef USLEEP_ISLAND_MODE_ENABLE
  npa_event_handle islandLatencyEventHandle;

  npa_define_node(&g_sleepIslandLatencyNode, &initial_state, NULL);
  npa_remote_publish_resource(NPA_ALL_PDS, SLEEP_ISLAND_LATENCY_NODE_NAME);

  CORE_VERIFY(NPA_SUCCESS == (npa_add_resource_log_by_handle("Sleep Requests",
                                                             g_sleepIslandLatencyResource->handle)));

  islandLatencyEventHandle = npa_create_custom_event(SLEEP_ISLAND_LATENCY_NODE_NAME,
                                                     "islandLatencyEvent",
                                                     NPA_TRIGGER_POST_CHANGE_EVENT,
                                                     NULL,
                                                     sleepLatencyNode_changeEventCB,
                                                     (void*)SLEEP_SIGNAL_ISLAND_LATENCY);

  npa_define_node(&g_sleepIslandUSecLatencyNode, NULL, NULL);
  npa_remote_publish_resource(NPA_ALL_PDS, SLEEP_USEC_ISLAND_LATENCY_NODE_NAME);
  CORE_VERIFY(NPA_SUCCESS == (npa_add_resource_log_by_handle("Sleep Requests",
                                                             g_sleepIslandUSecLatencyResource->handle))); 
#endif /* USLEEP_ISLAND_MODE_ENABLE */
#endif /* FEATURE_SLEEP_NO_LATENCY_NODE */

  return;
}

