/*===========================================================================
                               arc_hal.c

DESCRIPTION:
  This file implements the HAL interface for ARC driver.

Copyright (c) 2016 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.  
QUALCOMM Proprietary.  Export of this technology or software is regulated 
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

//===========================================================================
//                       Includes and Defines
//===========================================================================

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------
#include "HALhwio.h"
#include "arc_hal.h"
#include "arc_hal_hwio.h"

//===========================================================================
//                       Function Defintions
//===========================================================================

//---------------------------------------------------------------------------
//  Local Function Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  Public Function Definitions
//---------------------------------------------------------------------------

/**
 * <!-- arc_rm_hal_ols -->
 *
 * @brief This function returns the value representing various Operating
 *        Levels (OLs) for an input ARC RM.
 *
 * There are multiple OLs associated with each RM. Please refer to
 * ARC_HAL_OL_* macros on how to extract relevant OL.
 *
 * @param rm_id: ARC RM id.
 *
 * @return ARC RM operating levels
 */
uint32 arc_rm_hal_ols (int8 rm_id)
{
  uint32 rm_ols;
  rm_ols = HWIO_INI (RMm_OPERATIONAL_LEVEL, rm_id);

  return rm_ols;
}

