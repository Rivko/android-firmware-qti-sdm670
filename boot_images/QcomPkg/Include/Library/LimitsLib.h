#ifndef __LIMITS_LIB_H__
#define __LIMITS_LIB_H__
/*============================================================================
  @file LimitsLib.h

  MSM limits library API.

  This file contains data structures and functions used to configure, control,
  and query limits sw.

               Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
               All Rights Reserved.
               Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/
#include "DALStdDef.h"
#include <Protocol/EFILimits.h>

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ----------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ----------------------------------------------------------------------*/
/**
 * @brief Provides limits driver throttle recommendation for HLOS SW.

   Determines HLOS throttle recommendation based on Subsys ID. It is
   expected that this driver will only make throttling recommendations during
   initial HW validation.

   @param Subsys : Subsys ID to determine throttling recommendation for
          Throttle : Return EFI_LIMITS_THROTTLE_TYPE

   @return       EFI_SUCCESS
 */
EFI_STATUS LimitsSubsysThrottle( EFI_LIMITS_SUBSYS_ID Subsys,
                                 EFI_LIMITS_THROTTLE_TYPE *Throttle);

/**
   @brief Initializes Limits library

   Initialize the Limits library for core.

   @return 0 on success, else an error.
 */
int32 LimitsInit(void);

#endif /* #ifndef __LIMITS_LIB_H__ */

