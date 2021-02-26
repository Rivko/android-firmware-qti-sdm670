/*
* Copyright (c) 2015 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

 /*
@file: usleepmgr.c
@brief: ADSPPM usleep manager implementation.

$Header: //components/rel/core.qdsp6/2.1.c4/power/qdsp_pm/src/driver/qdsp_pm_mgr/resource_mgr/usleepmgr.c#1 $
*/

#include "adsppm.h"
#include "adsppm_utils.h"
#include "usleepmgr.h"
#include "hal_sleep.h"
#include "npa.h"

static npa_resource_state USleepMgr_USleepNode_Driver(
			npa_resource       *resource,
            npa_client         *client,
            npa_resource_state state);

/* Dependencies required for dsp pm usleep node */
static npa_node_dependency gQdsppm_USleepNodeDependency[1] =
{
  {
    USLEEP_NODE_NAME,
    NPA_CLIENT_REQUIRED,
  },
};

/* Main usleep node used to control entry into uImage */
static npa_resource_definition gQdsppm_USleepNodeResource[] =
{
  {
    QDSPPM_USLEEP_NODE_NAME,  /* Name */
    "bitmask",                /* Units */
    0x01,                     /* Max State */
    &npa_binary_and_plugin,   /* Custom plugin */
    NPA_RESOURCE_INTERPROCESS_ACCESS_ALLOWED, /* Attributes */
    NULL,                     /* User Data */
    NULL                      /* Query Function */
  }
};

/* uSleep driver node to the idle plugin */
static npa_node_definition gQdsppm_USleepNode =
{
  "/core/qdsppm_uSleep",         			/* Name */
  USleepMgr_USleepNode_Driver,				/* Driver_fcn */
  NPA_NODE_DEFAULT,                   		/* Attributes */
  NULL,                               		/* Data */
  NPA_ARRAY(gQdsppm_USleepNodeDependency),  /* Dependencies */
  NPA_ARRAY(gQdsppm_USleepNodeResource)     /* Resources */
};

boolean gUSleepNodeCreated = FALSE;

/**
 * USleepMgr_USleepNode_RegisterCB
 * 
 * @brief Registers dspPm uSleep node
 * 
 * @param see npa_callback defintion for function parameters
 */
static void USleepMgr_USleepNode_RegisterCB(
			void         *context,
            unsigned int  eventType,
            void         *data,
            unsigned int  dataSize)
{
  npa_resource_state qdsppm_USleepInitialState = 0; /* Default is disabled */
  
  /* Creates the main uSleep node */
  npa_define_node(&gQdsppm_USleepNode, &qdsppm_USleepInitialState, NULL);

  gUSleepNodeCreated = TRUE;
  return;
}

Adsppm_Status USleepMgr_Init(void)
{
    Adsppm_Status result = Adsppm_Status_Success;
    ADSPPM_LOG_FUNC_ENTER;

    /* Register a callback for when the necessary dependencies are available,
     * in order to register the nodes and other requirements that dsp pm uSleep owns. */
    npa_resource_available_cb(USLEEP_NODE_NAME,
                              USleepMgr_USleepNode_RegisterCB, NULL);

    ADSPPM_LOG_FUNC_EXIT(result);
    return result;
}


/*==============================================================================
                        MAIN DSP PM uSLEEP NODE FUNCTION DEFINITIONS
 =============================================================================*/
/**
 * USleepMgr_USleepNode_Driver
 * 
 * @brief Driver function for the uSleep node
 *
 * @param resource: The NPA resource being requested.
 * @param client:   Pointer to the client making the request.
 * @param state:    New state of the resource.
 *
 * @return: New state of the resource
 */
static npa_resource_state USleepMgr_USleepNode_Driver(
			npa_resource       *resource,
            npa_client         *client,
            npa_resource_state state)
{
  /* Get the dependency of the uSleep node. */
  npa_client_handle client_handle = NPA_DEPENDENCY(resource, 0);

  /* Enable or disable the LPI. */
  npa_issue_required_request(client_handle, state);

  return state;
}

boolean USleepMgr_GetUSleepVoteEnabled(void)
{
	npa_query_type aggr_vote;
    npa_query_status error = NPA_QUERY_SUCCESS;

	/* If node is not created, return FALSE*/
	if(!gUSleepNodeCreated)
		return FALSE;

	error = npa_query_by_name(
							QDSPPM_USLEEP_NODE_NAME,
							NPA_QUERY_CURRENT_STATE,
							&aggr_vote);
	if(0 != error)
    {
        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
					"npa_query_by_name: Failed due to NPA error %u", error);
		return FALSE;
    }
    ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO_EXT, "USleepNPA aggr_vote %u", aggr_vote.data.state);

    return (aggr_vote.data.state == QDSPPM_USLEEP_ALLOW) ? TRUE : FALSE;
}    

