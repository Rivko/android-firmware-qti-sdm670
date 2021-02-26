#ifndef BOOT_EXTERN_POWER_INTERFACE_H
#define BOOT_EXTERN_POWER_INTERFACE_H
/*===========================================================================

                    BOOT EXTERN POWER DRIVER DEFINITIONS

DESCRIPTION
  Contains wrapper definition for external power drivers

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2013-2014 by Qualcomm Technologies, Inc.  All Rights Reserved.
============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    
    
when        who     what, where, why
--------   ---     ----------------------------------------------------------
09/23/14   ck       Removed feature flag as API is always enabled in XBL
06/20/14   kedara   Fix llvm compiler warnings.
09/04/13   jz       Initial Creation.

===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/ 

/*===========================================================================

  FUNCTION RAILWAY_INIT

  DESCRIPTION
        This function initialize the railway driver and all of its associated rail control.

  PARAMETERS
    None.

  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    None

===========================================================================*/
void boot_railway_init(void);


/*===========================================================================

  FUNCTION CPR_INIT

  DESCRIPTION
        This function initializes the CPR driver.

  PARAMETERS
    None.

  DEPENDENCIES
    Needs to call railway_init first.

  RETURN VALUE
    None.

  SIDE EFFECTS
    None

===========================================================================*/
void boot_cpr_init(void);


/*===========================================================================

**  Function :  boot_populate_cpr_settings

** ==========================================================================
*/
/*!
* 
* @brief
*   This function calls cpr_externalize_state to populate the CPR settings and voltages in smem   
* 
* @param
*   None
*
* @par Dependencies
*   smem must have been initialized.
*   
* @retval
*   None                                                             
* 
* @par Side Effects
*   None
* 
*/
void boot_populate_cpr_settings(void);

void boot_aop_mini_dump_init(void);

#endif /* BOOT_EXTERN_POWER_INTERFACE_H */
