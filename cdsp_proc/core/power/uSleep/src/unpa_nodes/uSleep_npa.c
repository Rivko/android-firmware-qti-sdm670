/*==============================================================================
  FILE:         uSleep_npa.c

  OVERVIEW:     This file provides the various uSleep node setup

  DEPENDENCIES: None
  
                Copyright (c) 2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/uSleep/src/unpa_nodes/uSleep_npa.c#1 $
$DateTime: 2018/07/30 01:21:36 $
==============================================================================*/
#ifdef UNPA_SUPPORT

#include "comdef.h"
#include "CoreVerify.h"
#include "uSleep_npa.h"
#include "unpa_resource.h"
#include "unpa.h"
#include "uSleep_util.h"

/*==============================================================================
                          LATENCY NODE DEFINITION
 =============================================================================*/
/**
 * uSleepNPA_latencyDriver
 *
 * @brief Driver function for the cpu/vdd node resource that updates it.
 *
 * @param resource: Pointer to the resource (i.e. /core/cpu/latency/usec) (unused).
 * @param client:   Client of this resource (unused).
 * @param state:    Resource state as aggregated by the Update function.
 *
 * @return The state of the resource if set else UNPA_MAX_STATE
*/
static unpa_resource_state uSleepNPA_latencyDriver(unpa_resource       *resource,
                                                    unpa_client         *client,
                                                    unpa_resource_state state)
{
  /* Record the state so it can be queried from the sleep task before we go
   * to sleep. */
  if(0 == state)
  {  
    state = UNPA_MAX_STATE;
  }

  return state;
}

/* Latency node definition structure */
static unpa_resource_definition g_uSleepLatencyDefinition = 
{ 
  USLEEP_LATENCY_NODE_NAME,     /* Resource name */
  unpa_minUpdateFcn,            /* Update function */
  uSleepNPA_latencyDriver,      /* Driver function */
  UNPA_MAX_STATE,               /* Max State */
  UNPA_UPDATE_CLIENT_TYPES,     /* Supported client types */
  UNPA_RESOURCE_DEFAULT,        /* Attributes */
};

/*==============================================================================
                          CPU/VDD NODE DEFINITION
 =============================================================================*/
/**
 * uSleepNPA_cpuVddDriver
 *
 * @brief Driver function for the latency node resource that updates it.
 *
 * @param resource: Pointer to the resource name (unused).
 * @param client:   Client of this resource (unused).
 * @param state:    Resource state as aggregated by the Update function.
 *
 * @return The state of the resource if set else UNPA_MAX_STATE
*/
static unpa_resource_state uSleepNPA_cpuVddDriver(unpa_resource       *resource,
                                                  unpa_client         *client,
                                                  unpa_resource_state state)
{
  /* Enable or disable the low power mode optons */
  uSleep_setLowPowerModeInternal((uint32)state);
  return state;
}

/* Latency node definition structure */
static unpa_resource_definition g_uSleepCpuVddDefinition = 
{ 
  USLEEP_CPUVDD_LPR_NODE_NAME,        /* Resource name */
  unpa_maxUpdateFcn,                  /* Update function - higher values disable more modes */
  uSleepNPA_cpuVddDriver,             /* Driver function */
  USLEEP_CPUVDD_LPR_DISABLE_LPM_PLUS, /* Max State */
  UNPA_UPDATE_CLIENT_TYPES,           /* Supported client types */
  UNPA_RESOURCE_DEFAULT,              /* Attributes */
};

static unpa_resource g_uSleepLatencyResource;
static unpa_resource g_uSleepCpuVddResource;

/*==============================================================================
                         EXTERNAL FUNCTION DEFINITIONS
 =============================================================================*/
/*
 * uSleepNPA_initialize
 */
void uSleepNPA_initialize(void)
{
  /* Create the latency node */
  unpa_initResource(&g_uSleepLatencyDefinition, 0, &g_uSleepLatencyResource);

  /* Create the cpu/vdd node */
  unpa_initResource(&g_uSleepCpuVddDefinition, USLEEP_CPUVDD_LPR_ENABLE_ALL, &g_uSleepCpuVddResource);
  
  return;
}

#endif /* UNPA_SUPPORT */

