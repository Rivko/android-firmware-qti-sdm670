/**
@file AdspCoreSvc.cpp***********************

@brief This file contains the stubbed implementation for AdspCoreSvc.

*/

/*-----------------------------------------------------------------------
 Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
 Qualcomm Technologies Proprietary and Confidential.
-----------------------------------------------------------------------*/

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
/// ADSPCoreSvc header
#include "AdspCoreSvc.h"
#include "adsp_error_codes.h"

/* =======================================================================
**                          Function Definitions
** ======================================================================= */

/* =======================================================================
**         AdspCoreSvc Instance Create/Destroy/Workloop
** ======================================================================= */

ADSPResult AdspCoreSvc_Create (uint32_t inputParam, void **svcHandle)
{
    *svcHandle = NULL;
    return ADSP_EOK;
}
