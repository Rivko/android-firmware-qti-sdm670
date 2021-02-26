/*==============================================================================
  FILE:         sleep_client.c
 
  OVERVIEW:     This file provides the client functionality that's specific 
                to offline synthesis. 

  DEPENDENCIES: None

                Copyright (c) 2016 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/sleep2.0/src/client/sleep_client.c#1 $
$DateTime: 2018/07/30 01:21:36 $
==============================================================================*/
#include <stdlib.h>
#include "sleepi.h"
#include "npa_resource.h"
#include "sleep_lpri.h"
#include "sleep_client.h"
#include "synthLPR.h"
#include "synthRegistry.h"
#include "synthLPRM.h"
#include "SleepLPR_synth_modes.h"

/*==============================================================================
                             INTERNAL FUNCTIONS
 =============================================================================*/
/**
 * SleepClient_modeUpdated
 *
 * @brief This function notifies all the parent synthLPRM's of a given component
 *        mode about its new status. It should to be called only when mode status
 *        has been changed (enabled to disabled or vice versa) otherwise there is
 *        unnecessary processing.
 *
 * @param changed_mode: Pointer to a component mode which got its status 
 *                      updated.
 */
static void SleepClient_modeUpdated( sleep_lprm *changed_mode )
{
  int32 i = 0;

  /* If the LPR is not yet registered, or if this mode is not part of a
   * synthesized mode, do nothing. */
  if( FALSE == changed_mode->parent_LPR->registered ||
      NULL == changed_mode->synth_LPRM_parent_array )
  {
    sleepLog_printf( SLEEP_LOG_LEVEL_REQUESTS, 3, 
                      "  Mode is not a component of any synthesized mode "
                      "(lpr: \"%s\") (lprm: \"%s\") (registered: %d)", 
                      changed_mode->parent_LPR->resource_name, 
                      changed_mode->mode_name, 
                      changed_mode->parent_LPR->registered );
    return;
  }

  /* Refer to Sleep Documents on why we have different order of notifying 
   * parent modes based on mode status. */
  if( changed_mode->mode_enabled == FALSE )
  {
    /* Mode is disabled - Update from higher latency parent mode to lower
     * latency parent mode */
    for( i = 0; (uint32)i < changed_mode->synth_LPRM_parent_count; i++ )
    {
      synthLPRM_update((sleep_synth_lprm *)(changed_mode->synth_LPRM_parent_array[i]),
                       changed_mode );
    }
  }
  else
  {
    /* Mode is enabled - Updating from lower latency parent mode to higher
     * latency parent mode */
    for( i = changed_mode->synth_LPRM_parent_count - 1; i >= 0; i-- )
    {
      synthLPRM_update((sleep_synth_lprm *)(changed_mode->synth_LPRM_parent_array[i]), 
                       changed_mode );
    }
  }

  return;
}

/*==============================================================================
                              GLOBAL FUNCTIONS
 =============================================================================*/
/*
 * SleepClient_create
 */
void SleepClient_create( SleepClient *hClient, sleep_lpr *sleepLPR )
{
  CORE_VERIFY_PTR(hClient);
  CORE_VERIFY_PTR(sleepLPR);

  hClient->lpr_ptr = sleepLPR;
  hClient->m_lastRequest = 0;

  return;
}

/*
 * SleepClient_destroy
 */
void SleepClient_destroy( SleepClient *hClient )
{
  return;
}

/*
 * SleepClient_request
 */
void SleepClient_request( SleepClient *hClient, npa_resource_state request)
{
  npa_resource_state  disabled, enabled, changed;
  sleep_lpr           *lpr;
  int32               i;
  uint32              mode_mask;

  CORE_VERIFY_PTR(hClient);

  /* Disable the enabled modes */
  disabled = hClient->m_lastRequest & ~request;

  /* Enable the disabled modes */
  enabled = request & ~hClient->m_lastRequest;

  /* LPR against which request was issued */
  lpr = hClient->lpr_ptr;

  /* Loop through all of the component modes in this LPR and update
   * their state. */
  for( i = 0; (uint32)i < lpr->mode_count; i++ )
  {
    mode_mask = (1 << i);
    if( TRUE == lpr->modes[i].global_config_disabled )
    {
      /* Only one of them needs to be cleared but this keeps code simple */
      disabled  &= ~mode_mask;
      enabled   &= ~mode_mask;
    }
    else if( disabled & mode_mask )
    {
      sleepLog_QDSSPrintf( SLEEP_LOG_LEVEL_REQUESTS, 
                           SLEEP_MODE_DISABLED_NUM_ARGS,
                           SLEEP_MODE_DISABLED_STR, SLEEP_MODE_DISABLED, 
                           lpr->resource_name, 
                           lpr->modes[i].mode_name );
      lpr->modes[i].mode_enabled = FALSE;
    }
    else if( enabled & mode_mask )
    {
      sleepLog_QDSSPrintf( SLEEP_LOG_LEVEL_REQUESTS, 
                           SLEEP_MODE_ENABLED_NUM_ARGS,
                           SLEEP_MODE_ENABLED_STR, SLEEP_MODE_ENABLED, 
                           lpr->resource_name, lpr->modes[i].mode_name );
      lpr->modes[i].mode_enabled = TRUE;
    }
  }

  /* Mask containing modes that were changed with this request */
  changed = enabled | disabled;

  /* Refer to Sleep Documents on why we process modes in different order 
   * based on enabled/disabled mask. */
  if( disabled > enabled )
  {
    /* Lower latency modes were disabled */
    for( i = 0; (uint32)i < lpr->mode_count; i++ )
    {
      if( changed & (1 << i) )
      {
        SleepClient_modeUpdated( &(lpr->modes[i]) );
      }
    }
  }
  else
  {
    /* Lower latency modes were enabled */
    for( i = lpr->mode_count - 1; i >= 0; i-- )
    {
      if( changed & (1 << i) )
      {
        SleepClient_modeUpdated( &(lpr->modes[i]) );
      }
    }
  }

  hClient->m_lastRequest = request;

  return;
}

/*
 * SleepClient_request
 */
npa_resource_state SleepClient_requestClient (npa_resource       *resource,
                                              npa_client         *client,
                                              npa_resource_state state)
{
  SleepClient *sleep_client;

  /* This check should always fail as NPA framework ensures that both
   * resource and client are non-null. It is added here for sanity
   * purpose */
  if( NULL == resource || NULL == client )
  {
    sleepLog_printf(SLEEP_LOG_LEVEL_ERROR, 2,
                    "Null Client or Resource (Resource: 0x%x)"
                    " (Client: 0x%x)", resource, client);

    return 0;
  }

  sleep_client = (SleepClient *)client->resource_data;

  if( NULL == sleep_client )
  {
    /* Ideally we should have ERR_FATAL here but it comes back to 
     * sleep even in those cases where issue lies somewhere outside
     * like npa client being used in multiple threads. However, it
     * would be helpful to have it in extensive test build */
    sleepLog_printf(SLEEP_LOG_LEVEL_ERROR, 2,
                    "Null Client Sleep Request (Resource: 0x%x)"
                    " (Client: 0x%x)", resource, client);
    
    return 0;
  }

  SleepClient_request( sleep_client, NPA_PENDING_REQUEST(client).state );

  return state;
}

/*
 * SleepClient_registerSleepLPR
 */
void SleepClient_registerSleepLPR(sleep_lpr   *sleepLPR,
                                  npa_client  *client)
{
  CORE_LOG_VERIFY_PTR(sleepLPR, sleepLog_printf(SLEEP_LOG_LEVEL_ERROR,
                      0, "ERROR (message: \"NULL Lpr\")"));

  sleepLog_printf(SLEEP_LOG_LEVEL_DEBUG, 1,
                  "Registering LPR (Name: \"%s\")",
                  sleepLPR->resource_name);

  /* Now configure the client's internal state so that we can look up its
   * registered resources properly when it makes a request.*/
  CORE_VERIFY_PTR(client->resource_data = malloc(sizeof(SleepClient)));
  SleepClient_create( client->resource_data, sleepLPR );

  if( NULL == client->resource_data )
  {
    sleepLog_printf( SLEEP_LOG_LEVEL_ERROR, 2,
                      "Memory allocation failure (LPR: \"%s\") (Client: 0x%x)",
                      sleepLPR->resource_name, client);

    CORE_VERIFY(0);
  }

  return;
}

