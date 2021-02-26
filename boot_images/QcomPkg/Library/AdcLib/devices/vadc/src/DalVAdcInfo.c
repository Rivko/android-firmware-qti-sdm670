/*============================================================================
  FILE:         DalVAdcInfo.c

  OVERVIEW:     VADC DAL info file.

  DEPENDENCIES: None

                Copyright (c) 2009-2012 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/AdcLib/devices/vadc/src/DalVAdcInfo.c#1 $$DateTime: 2017/09/18 09:26:13 $$Author: pwbldsvc $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2012-06-13  jjo  Updated to use a DAL device ID string name.
  2009-03-16  jdt  Initial revision.

============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "DALReg.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Function Declarations
 * -------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

DALResult
VAdc_DalAdcDevice_Attach(const char *, DALDEVICEID, DalDeviceHandle **);

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/
const DALREG_DriverInfo DALVAdc_DriverInfo =
{
   VAdc_DalAdcDevice_Attach,
   0,
   NULL
};

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/
