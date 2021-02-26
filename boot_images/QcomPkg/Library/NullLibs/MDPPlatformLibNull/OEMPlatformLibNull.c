/*=============================================================================

File: OEMPlatformLibNull.c

OEM platforms specific. This file is to be modified by the OEM only. No Qualcomm
platform specifics or other Qualcomm code should be here.

This file contains all the platform specific configuration that OEM needs to
provide in order to setup OEM specific platforms. The file MDPPlatformLib.c
could be used as sample.

     Copyright (c) 2013-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
     Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

#include "OEMPlatformLib.h"

/* -----------------------------------------------------------------------
** Defines
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Variables
** ----------------------------------------------------------------------- */


/*===========================================================================
Function Definitions
===========================================================================*/


/* ---------------------------------------------------------------------- */
/**
** FUNCTION: OEMPlatformConfigure()
** 
** DESCRIPTION:
**   Detect, configure, power up, initialize of platform panels
**
*//* -------------------------------------------------------------------- */
MDP_Status OEMPlatformConfigure(MDP_Display_IDType eDisplayId, MDPPlatformConfigType eConfig, MDPPlatformParams *pPlatformParams)
{
  return MDP_STATUS_OK;
}



#ifdef __cplusplus
}
#endif
