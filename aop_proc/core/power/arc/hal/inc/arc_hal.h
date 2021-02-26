#ifndef __ARC_HAL_H__
#define __ARC_HAL_H__
/*===========================================================================
                                arc_hal.h

DESCRIPTION:
  This file contains HAL interface for the ARC HW block.

Copyright (c) 2016 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.  
QUALCOMM Proprietary.  Export of this technology or software is regulated 
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

//===========================================================================
//                       Includes and Type Definitions
//===========================================================================

//---------------------------------------------------------------------------
//  Include Files
//---------------------------------------------------------------------------
#include "comdef.h"
#include "HALhwio.h"
#include "arc_hal_hwio.h"

//---------------------------------------------------------------------------
//  Defines
//---------------------------------------------------------------------------

/* Macros to obtain various type of ARC Operating levels (OLs).
 *
 * _CURRENT - Current OL
 * _SOLVED - OL selected by solver if kicked in
 * _AGGREGATED - Aggregated OL based on votes from all DRVs (usually max aggr)
 * _SEQUENCE - OL that will be achieved at the end of current sequence.
 * _DESTINATION - Final OL.
 *
 */
#define _ARC_EXTRACT_OL(ols, ol_type) \
  (((ols) & HWIO_FMSK(RMm_OPERATIONAL_LEVEL, ol_type)) >> \
    HWIO_SHFT(RMm_OPERATIONAL_LEVEL, ol_type))

#define ARC_HAL_OL_CURRENT(ols)        _ARC_EXTRACT_OL(ols, CURRENT_OL)
#define ARC_HAL_OL_SOLVED(ols)         _ARC_EXTRACT_OL(ols, SOLVED_OL)
#define ARC_HAL_OL_AGGREGATED(ols)     _ARC_EXTRACT_OL(ols, AGGREGATED_OL)
#define ARC_HAL_OL_SEQUENCE(ols)       _ARC_EXTRACT_OL(ols, SEQUENCE_OL)
#define ARC_HAL_OL_DESTINATION(ols)    _ARC_EXTRACT_OL(ols, DESTINATION_OL)

//===========================================================================
//                       Function Declarations
//===========================================================================

/**
 * <!-- arc_rm_hal_ols -->
 *
 * @brief This function returns the value representing various Operating
 *        Levels (OLs) for an input ARC RM.
 *
 * There are multiple OLs associated with each RM. Please refer to
 * ARC_HAL_OL_* macros on how to extract relevant OL.
 *
 * @param rm_id: ARC RM id. It is up to client to perform validation on
 *               this parameter.
 *
 * @return ARC RM operating levels.
 */
uint32 arc_rm_hal_ols (int8 rm_id);

#endif   // __ARC_HAL_H__
