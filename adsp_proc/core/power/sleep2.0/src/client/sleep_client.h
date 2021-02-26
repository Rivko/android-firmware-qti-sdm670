#ifndef _SLEEP_CLIENT_H_
#define _SLEEP_CLIENT_H_
/*==============================================================================
  FILE:         sleep_client.h
  
  OVERVIEW:     This file contains the definition of a class that
                enables/disables the modes requested by NPA client for a
                particular resource. 

  DEPENDENCIES: None

                Copyright (c) 2016 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/sleep2.0/src/client/sleep_client.h#1 $
$DateTime: 2017/07/21 22:10:47 $
==============================================================================*/
#include "synthLPR.h"
#include "npa_resource.h"
#include "CoreVerify.h"
#include "sleep_log.h"
#include "sleep_lpri.h"
#include "SleepLPR_synth_modes.h"

/*==============================================================================
                             TYPE DEFINITIONS
 =============================================================================*/
typedef struct SleepClient_s
{
  /* Pointer to the LPR associated with this SleepClient */
  sleep_lpr *lpr_ptr;

  /* Last request made for the LPR through this SleepClient */
  npa_resource_state m_lastRequest;
} SleepClient;

/*==============================================================================
                        GLOBAL FUNCTION DECLARATIONS
 =============================================================================*/
/**
 * SleepClient_create
 *
 * @brief Constructor that will create a SleepClient object for input 
 *        sleep_lpr struct.
 *
 * @param lpr:  Pointer to sleep_lpr for which we want to create sleep client
 *              that will enable/disable the modes as requested.
 * 
 * @param sleepLPR: Pointer to sleep_synth_lpr for the above sleep_lpr that is
 *                  registered with the sleep registry.
 */
void SleepClient_create( SleepClient *hClient, sleep_lpr *sleepLPR );

/**
 * SleepClient_destroy
 *
 * @brief Destructor for the SleepClient class.
 */
void SleepClient_destroy( SleepClient *hClient );


/**
 * SleepClient_request
 *
 * @brief Enables/disables the modes of corresponding LPR as requested.
 *
 * @param request: Bitmask containing 1's for the modes we want to enable
 *                 and 0's for the modes we want to disable.
 */
void SleepClient_request( SleepClient *hClient, npa_resource_state request);

/**
 * SleepClient_registerSleepLPR
 *
 * @brief Registers the low power resource with sleep system and configures
 *        clients so that we can query the resources.
 *
 * @param sleepLPR: Low power resource we want to register with sleep system.
 * @param client: Pointer to npa client.
 */
void SleepClient_registerSleepLPR( sleep_lpr   *sleepLPR,
                                   npa_client  *client);

/**
 * SleepClient_requestClient
 *
 * @brief Enables/disables the modes as request by state bitmask.
 * 
 * This function gets called from sleep LPR node's driver function.
 *
 * @param resource: NPA resource node corresponding to sleep LPR node.
 * @param client: Handle to client of sleep LPR node.
 * @param state: Bitmask corresponding to the new state of the client.
 *
 * @return New state of the client.
 */
npa_resource_state SleepClient_requestClient (npa_resource       *resource,
                                              npa_client         *client,
                                              npa_resource_state state);

#endif /* _SLEEP_CLIENT_H_ */
