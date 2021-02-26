/*=============================================================================

                        Boot Thermal Management

GENERAL DESCRIPTION
  This file does SBL thermal management

Copyright 2012-2013, 2015 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when       who       what, where, why
--------   ---       ----------------------------------------------------------
05/05/15   jjo       Move looping logic to BootTempCheck
04/04/13   dh        Call boot_hw_powerdown to power down
12/04/12   dh        Move boot_Tsens_Init to sbl1_hw_init
09/25/12   dh        Initial revision
=============================================================================*/

/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#ifdef BOOT_TEMP_CHECK_THRESHOLD_DEGC

#include "boothw_target.h"
#include "boot_error_if.h"
#include "boot_extern_tsensor_interface.h"

#endif /* BOOT_TEMP_CHECK_THRESHOLD_DEGC */


/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/


/*=============================================================================

                              FUNCTION DEFINITIONS

=============================================================================*/

/*===========================================================================

**  Function :  boot_check_device_temp

** ==========================================================================
*/
/*!
*
* @brief
*   This function does a temperature check. If the device is too hot or too cold
*   we shut down the device.
*
* @param[in]
*   None
*
* @par Dependencies
*   Must be called after DALSYS_InitMod, busywait_init and boot_Tsens_Init
*
* @retval
*   None
*
* @par Side Effects
*   None
*
*/
void boot_check_device_temp( void )
{
#ifdef BOOT_TEMP_CHECK_THRESHOLD_DEGC

  BootTempCheckStatusType boot_temp;

  boot_temp = boot_BootTempCheck();

  do
  {
    if ((boot_temp == BOOT_TEMP_CHECK_TEMP_OK) ||
        (boot_temp == BOOT_TEMP_CHECK_INVALID_TSENS_CAL) ||
        (boot_temp == BOOT_TEMP_CHECK_SENSOR_NOT_AVAILABLE))
    {
      /* Continue with boot */
      break;
    }
    else if ((boot_temp == BOOT_TEMP_CHECK_TEMP_BELOW_THRESHOLD) ||
             (boot_temp == BOOT_TEMP_CHECK_TEMP_ABOVE_THRESHOLD))
    {
      boot_hw_powerdown();
    }
    else
    {
      BL_ERR_FATAL(boot_temp|BL_ERROR_GROUP_TSENS);
    }
  } while (0);

#endif /* BOOT_TEMP_CHECK_THRESHOLD_DEGC */
}

