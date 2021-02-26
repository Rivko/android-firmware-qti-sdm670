
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
07/11/15   rp	   Changed boot_enable_led function to have an additional argument for color
04/09/13   dh      Initial creation
=============================================================================*/

/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boot_visual_indication.h"
#include "boot_extern_pmic_interface.h"

static uint32 current_time = 0;
static uint32 previous_time = 0;
static boot_boolean current_led_status = FALSE;

/* The LED flash interval in microseconds*/
static uint32 LED_FLASH_INTERVAL_US = 500000;


/*===========================================================================

**  Function :  boot_enable_led

** ==========================================================================
*/
/*! 
 * @brief This function turn on/off an led according to input
 * @return None.
 *
 */ 
boot_boolean boot_enable_led(uint32 led_color, boot_boolean led_on)
{
  return (boot_pm_enable_led(led_color, led_on) ==  PM_ERR_FLAG__SUCCESS) ? TRUE : FALSE;
}


/*===========================================================================

**  Function :  boot_toggle_led_init

** ==========================================================================
*/
/*! 
 * @brief This function initializes the timer count for led toggling
 * @return None.
 *
 */ 
void boot_toggle_led_init(void)
{
  previous_time = boot_get_time();
}


/*===========================================================================

**  Function :  boot_toggle_led

** ==========================================================================
*/
/*! 
 * @brief This function executes the pmic rgb led functions to toggle LED
 * @return TRUE if operation is successful, FALSE if not.
 *
 */ 
boot_boolean boot_toggle_led(void)
{
  boot_boolean ret_val = TRUE;
  uint32 led_color = PM_RGB_SEGMENT_B;
  current_time = boot_get_time();
  
  /* There may be problem when current_time is out of range, but only 
   * one time, so do not take it into account
   */
  if ((current_time - previous_time) >= LED_FLASH_INTERVAL_US)
  {
    current_led_status = !current_led_status;
    ret_val = boot_enable_led(led_color, current_led_status);
    previous_time = current_time;
  }
  
  return ret_val;
}
