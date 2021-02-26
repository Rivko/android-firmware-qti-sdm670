/*==============================================================================
  FILE:         uSleep_lpr.c

  OVERVIEW:     This file contains the low power modes related support functions 

  DEPENDENCIES: Object file generated from source is marked as island section
  
                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/uSleep/src/lpr/uSleep_lpr.c#1 $
$DateTime: 2017/07/21 22:10:47 $
==============================================================================*/
#include <stdlib.h>
#include "comdef.h"
#include "CoreVerify.h"
#include "uSleep_lpri.h"
#include "uSleep_target.h"
#include "uATS.h"

/*==============================================================================
                          EXTERNAL FUNCTION DEFINITIONS
 =============================================================================*/
#ifdef DYNAMIC_LPR_CALLBACKS
/*
 * uSleepLPR_registerCallback 
 */ 
uSleep_status uSleepLPR_registerCallback(char                   *cbName,
                                         uSleep_lpr_cb          func, 
                                         uSleep_mode_choices    mode,
                                         uSleep_lpr_cb_location location,
                                         void                   *userData, 
                                         uint32                 latency,
                                         uSleep_lpr_cb_attr     attributes)
{
  uSleep_LPR          *synthMode;
  uSleep_dynamic_lpr  *LPRCB;
  uSleep_lpr_callback *list;
  uint32              idx     = 0;
  uSleep_status       status  = USLEEP_SUCCESS;

  CORE_VERIFY(mode < USLEEP_NUMBER_LOW_POWER_MODES);

  /* Get the right list to add to */
  synthMode = &g_uSleepLPR[mode];
  LPRCB     = &synthMode->registered_lprs;

  if(USLEEP_LPR_CB_ENTER == location)
  {
    list = LPRCB->enter;
  }
  else
  {
    list = LPRCB->exit;
  }

  /* Find the end of the list */
  do
  {
    list = &list[idx];
  }while((NULL != list->func) && (++idx < MAX_DYN_LPR_SUPPORTED));

  if(idx > MAX_DYN_LPR_SUPPORTED)
  {
    status = USLEEP_DYN_LPR_NO_ROOM;
  }
  else
  {
    if(NULL != cbName)
    {
      list->name = cbName;
    }
    else
    {
      list->name = "Dynamic LPR";
    }

    list->func      = func;
    list->userData  = userData;

    /* Update cacheable flag if CB is not cacheable */
    if((TRUE == synthMode->cached) &&
       (0 == (USLEEP_LPR_CB_ATTR_CACHABLE & attributes)))
    {
      /* All LPRM's must be cacheable */
      synthMode->cached = FALSE;
    }
  }

  return status;
}
#endif /* DYNAMIC_LPR_CALLBACKS */

/*
 * uSleepLPR_setModeStatus 
 */
void uSleepLPR_setModeStatus(uSleep_mode_choices index, boolean enable)
{
  CORE_VERIFY(index < USLEEP_NUMBER_LOW_POWER_MODES);

  if(TRUE == enable)
  {
    /* Set user enabled flag */
    g_uSleepLPR[index].enabled_flags |= USLEEP_LPR_ENABLE_MASK;
  }
  else
  {
    /* Clear user enabled flag */
    g_uSleepLPR[index].enabled_flags &= ~USLEEP_LPR_ENABLE_MASK;
  }

  /* Invoke uATS to reselect the power mode to enter */
  uATS_SetSignal(USLEEP_SIGNAL_REGISTRY);

  return;
}

/*
 * uSleepLPR_runLPRFunctions 
 */
void uSleepLPR_runLPRFunctions(boolean    enterFunction,
                               uint64     wakeupDeadline,
                               boolean    reEntrant,
                               uSleep_LPR *mode)
{
  if(TRUE == enterFunction)
  {
    if(NULL != mode->enter_func)
    {
      uSleepTarget_enterLowPowerMode(mode, wakeupDeadline, reEntrant);
    }
  }
  else
  {
    if(NULL != mode->exit_func)
    {
      uSleepTarget_exitLowPowerMode(mode);
    }
  }

  return;
}

/*
 * uSleepLPR_powerModeBaseInit
 */
void uSleepLPR_powerModeBaseInit(void)
{
  /* Config the base settings for all low power modes */
  uSleepTarget_configBasePowerMode(TARGET_POWER_MODE_APCR_PLL_LPM);
  return;
}

