/*==============================================================================
  FILE:         uSleep_lpr_modes.c

  OVERVIEW:     This file contains the low power modes supported in island mode
               

  DEPENDENCIES: Object file generated from source is marked as island section
  
                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/uSleep/src/lpr/uSleep_lpr_modes.c#1 $
$DateTime: 2018/07/30 01:21:36 $
==============================================================================*/
#include "comdef.h"
#include "uSleep_lpr.h"
#include "uSleep_target.h"
#include "uSleep_os.h"

/*==============================================================================
                             FORWARD DECLRATIONS
 =============================================================================*/
void uSleepLPR_powerModeBaseInit(void);

void uSleepLPR_clockgateEnter(uint64 wakeupTime, boolean reEntrant);

void uSleepLPR_cachedApcrPlusEnter(uint64 wakeupTime, boolean reEntrant);

void uSleepLPR_apcrPlusEnter(uint64 wakeupTime, boolean reEntrant);

/*==============================================================================
                             INTERNAL VARIABLES
 =============================================================================*/
/* Always enabled, default low power mode when no other mode can be chosen */
uSleep_LPR g_uSleepDefaultLPR = {"Clock Gate",                 /* LPR name */
                                 {0,0},                        /* Seed enter, exit latency */
                                 0,                            /* Backoff latency */
                                 uSleepLPR_powerModeBaseInit,  /* Init function */
                                 uSleepLPR_clockgateEnter,     /* Enter & exit functions for the LPR */
                                 NULL,
                                 TRUE,                         /* Cacheable flag */
                                 USLEEP_LPR_ENABLED};          /* Default internal enable flag */

#ifdef USLEEP_IMAGE_SLPI

/* Supported LPR's in uSleep. */
uSleep_LPR g_uSleepLPR[USLEEP_NUMBER_LOW_POWER_MODES] =
{
#if defined(USLEEP_SUPPORT_UNCACHED_APCR)
  /* Really for debugging or idle time execution requirement, prefered is cacheable APCR
   * Uncached APCR+ pll off. */
  {"APCR+",
    {1172, 1172}, 1172, /* We could be running on sleep clock, so we loose resolution */
    NULL, uSleepLPR_apcrPlusEnter, NULL,
    FALSE,
    USLEEP_LPR_ENABLED
  },

#else
  /* Cached APCR+ pll off*/
  {"Cached APCR+",
    {587, 587}, 587,
    NULL, uSleepLPR_cachedApcrPlusEnter, NULL,
    TRUE,
    USLEEP_LPR_ENABLED
  },
#endif
};

#elif defined(USLEEP_IMAGE_CDSP)

//Not supported

#elif defined(USLEEP_IMAGE_ADSP)

uSleep_LPR g_uSleepLPR[USLEEP_NUMBER_LOW_POWER_MODES] =
{
#if defined(USLEEP_SUPPORT_UNCACHED_APCR)
  /* Uncached APCR pll off. */ 
  {"APCR+",
    {1025, 1260}, 1260,
    NULL, uSleepLPR_apcrEnter, NULL,
    FALSE,
    USLEEP_LPR_ENABLED       /* Really for debugging, prefered is cacheable APCR */
  },
#else
  /* Cached APCR pll off*/
  {"Cached APCR+",
    {50, 150}, 150,
    NULL, uSleepLPR_cachedApcrEnter, NULL,
    TRUE,
    USLEEP_LPR_ENABLED
  },
#endif
};

#endif /* USLEEP_IMAGE_SLPI */
