/*===========================================================================

                    BOOT EXTERN TSENS DRIVER DEFINITIONS

DESCRIPTION
  Contains wrapper definition for external tsensor drivers

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2012, 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.
============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    
    
when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/21/14   jjo     Updated for XBL.
06/18/12   dh      Initial Creation.

===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "boot_extern_tsensor_interface.h"


/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/ 

/*===========================================================================

**  Function :  boot_BootTempCheck

** ==========================================================================
*/
/*!
*
*    Checks the TSENS boot temperature.
*
* @return
*  BOOT_TEMP_CHECK_TEMP_OK indicates that the temperature is in an acceptable range to boot up.
* 
*  BOOT_TEMP_CHECK_TSENS_TIMEOUT indicates that the TSENS sensor timed out while polling for a temperature ready status bit.
* 
*  BOOT_TEMP_CHECK_INVALID_TSENS_CAL indicates that the TSENS sensor does not have a valid calibration data.
* 
*  BOOT_TEMP_CHECK_TEMP_ABOVE_THRESHOLD indicates that the temperature is above the acceptable temperature range.
*
*  BOOT_TEMP_CHECK_TSENS_ERROR indicates that an internal TSENS error occured. 
*
* @par Dependencies
*    boot_Tsens_Init must be called once prior to this function 
*
*/

BootTempCheckStatusType boot_BootTempCheck(void)
{
  return BootTempCheck();
}



/* ============================================================================
**  Function : boot_Tsens_Init
** ============================================================================
*/
/*!
    Initializes TSENS in the boot and enables the TSENS sensor used in the
    boot.

    @param
      None
    @return
      None
*/
TsensResultType boot_Tsens_Init(void)
{
    return Tsens_Init();
}
