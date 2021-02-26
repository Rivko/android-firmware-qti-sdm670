/*===========================================================================
  FILE: tracer_cfgrpm.c

  OVERVIEW:     Wrapper to configure RPM processor.
                Typically for Modem images (ADSP if no modem).

  DEPENDENCIES:

               Copyright (c) 2012 Qualcomm Technologies Incorporated.
               All Rights Reserved.
               Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================
  $Header: //components/rel/core.qdsp6/2.1/debugtrace/tracer/src/tracer_cfgrpm.c#1 $
===========================================================================*/

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/

#include "tracer_cfgrpm.h"
#include "rpm_swevent.h"

/*---------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ------------------------------------------------------------------------*/

#define TCRCFG_RPM_CTRL_SETTING_ETM         2

#define TCRCFG_RPM_ETM_TOGGLE               1

/*---------------------------------------------------------------------------
 * Externalized Function Definitions
 * ------------------------------------------------------------------------*/

tracer_cmdresp_t tracer_cfgrpm_swe(uint32 setting, uint32 type,
                                   uint64 bitmask)
{
   rpm_swevent_cfg_send(setting, type, bitmask);
   return TRACER_CMDRESP_SUCCESS;
}

tracer_cmdresp_t tracer_cfgrpm_etm(uint8 state)
{
   rpm_swevent_cfg_send((uint32)TCRCFG_RPM_CTRL_SETTING_ETM,
                        (uint32)TCRCFG_RPM_ETM_TOGGLE,
                        (uint64)state);
   return TRACER_CMDRESP_SUCCESS;
}

