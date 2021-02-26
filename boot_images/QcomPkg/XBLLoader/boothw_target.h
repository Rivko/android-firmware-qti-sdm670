#ifndef BOOTHW_TARGET_H
#define BOOTHW_TARGET_H
/*===========================================================================

                        Boot Hardware Target
                          Header File 

DESCRIPTION
 Contains common defintion used by boot downloader.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright 2009, 2012-2015 by Qualcomm Technologies Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/05/15   ck      Corrected copyright
10/29/14   kpa     Support 64 bit memory map
05/07/14   kedara  Removed target.h inclusion. Include boot_comdef.h
03/18/14   ck      Removed boot_hw_warm_reset as it is redundant
03/18/14   ck      Added enum for reset type and added to boot_hw_reset parameter list
01/14/14   ck      Added boot_switch_to_aarch64
07/24/12   aus     Added boot_hw_warm_reset
04/04/13   dh      Add boot_hw_powerdown
10/26/09   msm     Changes for CMI.
===========================================================================*/

#include "boot_comdef.h"

/*===========================================================================

                           DATA DECLARATIONS

===========================================================================*/
typedef enum
{
  BOOT_HARD_RESET_TYPE = 0,
  BOOT_WARM_RESET_TYPE = 1,
  BOOT_DVDD_HARD_RESET_TYPE = 2
} boot_hw_reset_type;


/*===========================================================================

  FUNCTION boot_switch_to_aarch64()

  DESCRIPTION
    This function configures and then switches the processor to 64 bit mode
    (AARCH64).  

  PARAMETERS
    entry_point = The address of where the processor should start executing
    from after the switch to AARCH64

    carried_parameter = Parameter that will be passed to entry point after
    the switch to AARCH64.

  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    This function will never return.
    
===========================================================================*/
void boot_switch_to_aarch64(uintnt entry_point,
                            uintnt carried_parameter);


/*===========================================================================

  FUNCTION boot_hw_reset()

  DESCRIPTION
    This function resets the phone.

  PARAMETERS
    None.

  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    None.
    
===========================================================================*/
void boot_hw_reset(boot_hw_reset_type reset_type);


/*===========================================================================

**  Function :  boot_hw_powerdown

** ==========================================================================
*/
/*!
* 
* @brief
*   This function powers off the PMIC
* 
* @par Dependencies
*   None
*
* @retval
*   None
* 
* @par Side Effects
*   Target is switched off
* 
*/
void boot_hw_powerdown(void);

/*===========================================================================

**  Function :  boot_set_pwrkey_reset_type

** ==========================================================================
*/
/*!
* 
* @brief
*   This function sets the power key reset to hardreset
*   or warmreset as per the provided argument
* 
* @par Dependencies
*   None
*
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void boot_set_pwrkey_reset_type(boot_hw_reset_type reset_type);

/*===========================================================================

**  Function :  boot_set_pshold_reset_type

** ==========================================================================
*/
/*!
* 
* @brief
*   This function sets the pshold reset to hardreset
*   or warmreset as per the provided argument
* 
* @par Dependencies
*   None
*
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void boot_set_pshold_reset_type(boot_hw_reset_type reset_type);

/*===========================================================================

**  Function :  boot_enable_aarch64

** ==========================================================================
*/
/*!
* 
* @brief
*   This function sets the A64 bit in the RMR register.
* 
* @par Parameters
*   None
*
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void boot_enable_aarch64(void);


/*===========================================================================

**  Function :  boot_generate_warm_reset

** ==========================================================================
*/
/*!
* 
* @brief
*   This function causes the processor to generate a warm reset.
* 
* @par Parameters
*   carried_value = Value to be carried in R0 across reset.
*
* @retval
*   None
* 
* @par Side Effects
*   Function does not return
* 
*/
void boot_generate_warm_reset(uint32 carried_value);


/*===========================================================================

**  Function :  boot_configure_vbar

** ==========================================================================
*/
/*!
* 
* @brief
*   This function configures VBAR to a different base address.
* 
* @par Parameters
*   vbar_addr = Address to which VBAR will be configured to.
*
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void boot_configure_vbar(uintnt vbar_addr);


#endif /*BOOTHW_TARGET_H*/
