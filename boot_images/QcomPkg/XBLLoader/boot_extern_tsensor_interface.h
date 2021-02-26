#ifndef BOOT_EXTERN_TSENSOR_INTERFACE_H
#define BOOT_EXTERN_TSENSOR_INTERFACE_H
/*===========================================================================

                    BOOT EXTERN TSENS DRIVER DEFINITIONS

DESCRIPTION
  Contains wrapper definition for external tsensor drivers

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2012-2014 by Qualcomm Technologies, Inc.  All Rights Reserved.
============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    
    
when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/23/14   ck      Removed feature flag as API is always enabled in XBL
05/07/13   dh      Update return value to TSENS_SUCCESS
06/18/12   dh      Initial Creation.

===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include <Library/TsensLib.h>
#include <Library/BootTempCheckLib.h>

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
BootTempCheckStatusType boot_BootTempCheck(void);


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
TsensResultType boot_Tsens_Init(void);

#endif /* BOOT_EXTERN_TSENSOR_INTERFACE_H */
