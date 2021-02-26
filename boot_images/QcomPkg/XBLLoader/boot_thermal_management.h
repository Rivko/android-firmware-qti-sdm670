#ifndef BOOT_THERMAL_MANAGEMENT_H
#define BOOT_THERMAL_MANAGEMENT_H
/*=============================================================================

                        Boot Thermal Management

GENERAL DESCRIPTION
  This file does SBL thermal managememnt 

Copyright 2012, 2015 by Qualcomm Technologies Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ------------------------------------------------------------
10/05/15   ck      Corrected copyright
09/25/12   dh      Initial revision
=============================================================================*/

/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boot_comdef.h"

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
*   we wait few seconds to check again. If after a certain waiting period the temperature 
*   is still not valid we shutdown the device.
* 
* @param[in] 
*   None 
*     
* @par Dependencies
*   Must be called after DALSYS_InitMod and busywait_init
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void boot_check_device_temp( void );

#endif  /* BOOT_THERMAL_MANAGEMENT_H */

