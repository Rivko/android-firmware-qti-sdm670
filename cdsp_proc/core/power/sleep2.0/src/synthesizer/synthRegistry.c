/*==============================================================================
  FILE:         synthRegistry.c

  OVERVIEW:     Implementation of Sleep Registry which will keep track of all
                registered LPRs and their (enabled) modes.

  DEPENDENCIES: None

                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/sleep2.0/src/synthesizer/synthRegistry.c#1 $
$DateTime: 2018/07/30 01:21:36 $
==============================================================================*/
#include "comdef.h"
#include "sleepi.h"
#include "synthRegistry.h"
#include "synthLPRM.h"
#include "synthLPR.h"
#include "synthLPI.h"
#include "CoreVerify.h"
#include "sleep_lpri.h"
#include "sleep_config.h"
#include "npa.h"
#include "sleepActive.h"
#include "sleep_log.h"

/*==============================================================================
                              GLOBAL VARIABLES
 =============================================================================*/
/**
 * @brief g_LPRRegistry
 *
 * Registry for sleep system where low power resources (and their modes) are
 * registered.
 */
static synthRegistry g_LPRRegistry = {NULL};

/*============================================================================== 
                              FUNCTION DEFINITION
 =============================================================================*/
/*
 * synthRegistry_getRegistry
 */
synthRegistry *synthRegistry_getRegistry(void)
{
  return(&g_LPRRegistry);
}

/*
 * synthRegistry_init
 */
void synthRegistry_init(void)
{
  /* Trigger Active Time Solver Thread */
  sleepActive_SetSignal(SLEEP_SIGNAL_REGISTRY);

  return;
}

/*
 * synthRegistry_readGlobalConfig
 */
void synthRegistry_readGlobalConfig(sleep_lpr *sleepLPR)
{
  uint8   disable = FALSE;
  uint32  nMode;
  
  CORE_LOG_VERIFY_PTR(sleepLPR, sleepLog_printf(SLEEP_LOG_LEVEL_ERROR, 
                      0, "ERROR (message: \"NULL Lpr\")"));

  /* Check to see if this LPR has been disabled in the global
   * config settings. */
  sleepConfig_readBool((char *)sleepLPR->resource_name, 
                       NULL,
                       (char *)"disable", 
                       &disable );

  if(TRUE == disable)
  {
    sleepLog_printf(SLEEP_LOG_LEVEL_WARNING, 1, 
                    "WARNING (message: \"Disabled in the global config\")"
                    " (LPR Name: \"%s\")", sleepLPR->resource_name );
  }
  
  /* Loop through the modes that are being registered here */
  for(nMode = 0; nMode < sleepLPR->mode_count; nMode++)
  {
    sleep_lprm *sleepLPRM = &(sleepLPR->modes[nMode]);

    /* Check to see if this mode has been disabled in the global
     * config settings.  If the entire LPR is disabled, no need to read
     * each individual LPRM. */
    if(TRUE == disable)
    {
      sleepLPRM->global_config_disabled = TRUE;
    }
    else
    {
      sleepConfig_readBool((char *)sleepLPR->resource_name, 
                           (char *)sleepLPRM->mode_name, 
                           (char *)"disable",
                           &(sleepLPRM->global_config_disabled));

      if(TRUE == sleepLPRM->global_config_disabled)
      {
        sleepLog_printf(SLEEP_LOG_LEVEL_WARNING, 2, 
                        "WARNING (message: \"Disabled in the global config\")"
                        " (LPRM Name: \"%s.%s\")",
                        sleepLPR->resource_name, sleepLPRM->mode_name);
      }
    }
  }

  return;
}

/*
 * synthRegistry_registerSynthLPR
 */
void synthRegistry_registerSynthLPR(sleep_synth_lpr *synthLPR)
{
  CORE_LOG_VERIFY_PTR(synthLPR, sleepLog_printf(SLEEP_LOG_LEVEL_ERROR, 0,
                           "ERROR (message: \"NULL Pre-Synthesized Lpr\")"));

  sleepLog_printf(SLEEP_LOG_LEVEL_DEBUG, 1,
                   "Registering pre-synthesized LPR (Name: \"%s\")",
                   synthLPR->resource_name);

  /* init synthesized LPR */
  synthLPR_init(synthLPR);

  /* Make sure there is not already a registered synthesized LPR */
  CORE_VERIFY(NULL == g_LPRRegistry.LPR);
  g_LPRRegistry.LPR = synthLPR;

  return;
}

/*
 * synthRegistry_registerSynthLPI
 */
void synthRegistry_registerSynthLPI(sleep_synth_lpi *synthLPI)
{
#ifdef USLEEP_ISLAND_MODE_ENABLE
  CORE_LOG_VERIFY_PTR(synthLPI, sleepLog_printf(SLEEP_LOG_LEVEL_ERROR, 0,
                      "ERROR (message: \"NULL Pre-Synthesized Lpi\")"));

  sleepLog_printf(SLEEP_LOG_LEVEL_DEBUG, 1,
                   "Registering pre-synthesized LPI (Name: \"%s\")",
                   synthLPI->resource_name);

  /* init synthesized LPI */
  synthLPI_init(synthLPI);

  /* Make sure there is not already a registered synthesized LPR */
  CORE_VERIFY(NULL == g_LPRRegistry.LPI);
  g_LPRRegistry.LPI = synthLPI;
#endif

  return;
}

/*
 * synthRegistry_LPRUpdated
 */
void synthRegistry_LPRUpdated(sleep_synth_lpr *synthLPR)
{
  /* A Synth Mode was Enabled or Disabled. Trigger the Run-time Solver */
  sleepActive_SetSignal(SLEEP_SIGNAL_REGISTRY);

  return;
}

/*
 * synthRegistry_getStatus
 */
sleep_status synthRegistry_getStatus(registry_type type)
{
  sleep_status status;

  switch(type)
  {
    case REGISTRY_TYPE_LPR:
    {
      status = synthLPR_getStatus(g_LPRRegistry.LPR);
      break;
    }

    case REGISTRY_TYPE_LPI:
    {
      status = synthLPI_getStatus(g_LPRRegistry.LPI);
      break;
    }

    default:
      CORE_LOG_VERIFY(0, sleepLog_printf(SLEEP_LOG_LEVEL_ERROR,
                                         0, "ERROR (message: \"Invalid registry type\")"));
  }

  return status;
}
