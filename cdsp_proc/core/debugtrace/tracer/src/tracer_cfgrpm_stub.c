/*===========================================================================
  FILE: tracer_cfgrpm_stub.c

  OVERVIEW:    Wrapper to configure RPM processor.
               Typically for non-Modem images.

  DEPENDENCIES:

               Copyright (c) 2012 Qualcomm Technologies Incorporated.
               All Rights Reserved.
               Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================
  $Header: //components/rel/core.qdsp6/2.1.c4/debugtrace/tracer/src/tracer_cfgrpm_stub.c#1 $
===========================================================================*/

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/

#include "tracer_cfgrpm.h"

/*---------------------------------------------------------------------------
 * Externalized Function Definitions
 * ------------------------------------------------------------------------*/
tracer_cmdresp_t tracer_cfgrpm_swe(uint32 setting, uint32 type,
                                   uint64 bitmask)
{
   return TRACER_CMDRESP_F_AVAIL;   // Not configurable from here.
}

tracer_cmdresp_t tracer_cfgrpm_etm(uint8 state)
{
   return TRACER_CMDRESP_F_AVAIL;   // Not configurable from here.
}

