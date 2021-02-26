#ifndef __VCS_H__
#define __VCS_H__
/*
  ===========================================================================
*/
/**
  @file VCS.h
  @brief  Public interface for a lightweight VCS that
          must be available prior to the DAL VCS driver being initialized.
*/
/*=========================================================================*/
/*
  ===========================================================================

  Copyright (c) 2017 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================
*/

/*=========================================================================
      Include Files
==========================================================================*/

#include "DALSys.h"
#include "voltage_level.h"


/*=========================================================================
      Interface Declarations
==========================================================================*/


/** @addtogroup vcs_lite_functions
@{ */


/* =========================================================================
**  Function : VCS_GetBootCorner
** =========================================================================*/
/**
  Query function that returns the boot corner state for the requested rail.

  @param szRail [in] String name of the rail.
  @param pLevel [in] Pointer to address where the boot corner should be stored.

  @return
  DAL_SUCCESS -- boot corner state found for the requested rail.
  DAL_ERROR -- boot corner state not found for the requested rail.
  DAL_ERROR_INVALID_PARAMETER -- Pointer to the rail name or level is null.
*/

DALResult VCS_GetBootCorner(const char *szRail, rail_voltage_level *pLevel);


/** @} */ /* end_addtogroup vcs_lite_functions */

#endif /* __VCS_H__ */

