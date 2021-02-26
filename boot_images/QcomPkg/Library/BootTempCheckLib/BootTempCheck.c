/*============================================================================
  FILE:         BootTempCheck.c

  OVERVIEW:     Implementation of temperature check in the boot

  DEPENDENCIES: None

                Copyright (c) 2012, 2014-2015 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary.
============================================================================*/
/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2015-05-05  jjo  Add looping logic.
  2014-09-25  jjo  Switch to deci deg C.
  2014-04-02  jjo  Move boot sensor index here.
  2012-07-30  jjo  Added lower threshold.
  2012-05-23  jjo  Added DAL properties.
  2012-05-20  jjo  Ported to 8974.

============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "DALSys.h"
#include "BootTempCheckLib.h"
#include "BootTempCheckBsp.h"
#include "TsensLib.h"
#include "DALStdDef.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Function Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/
/* ============================================================================
**
**  BootTempCheck
**
**  Description:
**    Checks the TSENS boot temperature.
**
**  Parameters:
**    None
**
**  Return:
**    BootTempCheckStatusType
**
**  Dependencies:
**    None
**
** ========================================================================= */
BootTempCheckStatusType BootTempCheck(void)
{
   static BootTempCheckBspType *pBootTempCheckBsp;
   static DALBOOL bInitialized = FALSE;
   uint32 uLoopCount = 0;
   DALSYS_PROPERTY_HANDLE_DECLARE(hBootTempCheckProperties);
   DALSYSPropertyVar propertyVar;
   TsensResultType tsensResult;
   DALResult status;
   int32 nDeciDegC;
   int32 nDegC;

   /* Get the BSP */
   if (bInitialized == FALSE)
   {
      status = DALSYS_GetDALPropertyHandleStr("/core/power/boot_temp_check", hBootTempCheckProperties);
      if (status != DAL_SUCCESS)
      {
         return BOOT_TEMP_CHECK_TSENS_ERROR;
      }

      status = DALSYS_GetPropertyValue(hBootTempCheckProperties,
                                       "BOOT_TEMP_CHECK_BSP",
                                       0,
                                       &propertyVar);
      if (status != DAL_SUCCESS)
      {
         return BOOT_TEMP_CHECK_TSENS_ERROR;
      }

      pBootTempCheckBsp = (BootTempCheckBspType *)propertyVar.Val.pStruct;

      bInitialized = TRUE;
   }

   for ( ; ; )
   {
      tsensResult = Tsens_GetTemp(pBootTempCheckBsp->uSensor, &nDeciDegC);

      if (tsensResult == TSENS_SUCCESS)
      {
         nDegC = nDeciDegC / 10;

         if (nDegC > pBootTempCheckBsp->nUpperThresholdDegC)
         {
            if (uLoopCount == pBootTempCheckBsp->uMaxNumIterations)
            {
               return BOOT_TEMP_CHECK_TEMP_ABOVE_THRESHOLD;
            }
         }
         else if (nDegC < pBootTempCheckBsp->nLowerThresholdDegC)
         {
            if (uLoopCount == pBootTempCheckBsp->uMaxNumIterations)
            {
               return BOOT_TEMP_CHECK_TEMP_BELOW_THRESHOLD;
            }
         }
         else
         {
            return BOOT_TEMP_CHECK_TEMP_OK;
         }
      }
      else if (tsensResult == TSENS_ERROR_INVALID_CALIBRATION)
      {
         return BOOT_TEMP_CHECK_INVALID_TSENS_CAL;
      }
      else if (tsensResult == TSENS_ERROR_SENSOR_NOT_AVAILABLE)
      {
         return BOOT_TEMP_CHECK_SENSOR_NOT_AVAILABLE;
      }
      else if (tsensResult == TSENS_ERROR_TIMEOUT)
      {
         return BOOT_TEMP_CHECK_TSENS_TIMEOUT;
      }
      else
      {
         return BOOT_TEMP_CHECK_TSENS_ERROR;
      }

      DALSYS_BusyWait(pBootTempCheckBsp->uWaitPerIterationMicroSec);
      uLoopCount++;
   }
}

