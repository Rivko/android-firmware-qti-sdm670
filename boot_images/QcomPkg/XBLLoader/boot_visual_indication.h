#ifndef BOOT_VISUAL_INDICATION_H
#define BOOT_VISUAL_INDICATION_H
/*=============================================================================

                        Boot Visual Indication

GENERAL DESCRIPTION
  This file defines SBL visual indication functions

Copyright 2013, 2015 by Qualcomm Technologies Inc.  All Rights Reserved.
=============================================================================*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ------------------------------------------------------------
10/05/15   ck      Corrected copyright
07/11/15   rp	   Changed boot_enable_led function declaration to include led color as input
04/09/13   dh      Initial creation
=============================================================================*/

/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boot_comdef.h"

/*=============================================================================

                              FUNCTION DEFINITIONS

=============================================================================*/

/*===========================================================================

**  Function :  boot_enable_led

** ==========================================================================
*/
/*! 
 * @brief This function turn on/off an led according to input
 * @return None.
 *
 */ 
boot_boolean boot_enable_led(uint32 led_color, boot_boolean led_on);


/*===========================================================================

**  Function :  boot_toggle_led

** ==========================================================================
*/
/*! 
 * @brief This function executes the pmic rgb led functions to toggle LED
 * @return TRUE if operation is successful, FALSE if not.
 *
 */ 
boot_boolean boot_toggle_led(void);


/*===========================================================================

**  Function :  boot_toggle_led_init

** ==========================================================================
*/
/*! 
 * @brief This function initializes the timer count for led toggling
 * @return None.
 *
 */ 
void boot_toggle_led_init(void);


#endif  /* BOOT_VISUAL_INDICATION_H */

