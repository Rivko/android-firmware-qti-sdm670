/*===========================================================================

                    BOOT EXTERN POWER DRIVER DEFINITIONS

DESCRIPTION
  Contains wrapper definition for external power drivers

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2013-2015 by Qualcomm Technologies Incorporated.  All Rights Reserved.
============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    
    
when       who    what, where, why
--------   ---    ----------------------------------------------------------
10/05/15   ck     Corrected copyright
09/24/14   ck     Updated to rbcpr_api.h function names
09/04/13   jz     Initial Creation.

===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "railway.h"
#include "cpr.h"

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
void boot_railway_init()
{
  railway_init();
}

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
void boot_cpr_init()
{
  cpr_init();
}

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
void boot_populate_cpr_settings(void)
{
  cpr_externalize_state();
}

/*===========================================================================

**  Function :  boot_aop_mini_dump_init

** ==========================================================================
*/
/*!
* 
* @brief
*   This function is to inintialize AOP minidump regions   
* 
* @param
*   None
*
* @par Dependencies
*   smem and miniudmp table must have been initialized.
*   
* @retval
*   None                                                             
* 
* @par Side Effects
*   None
* 
*/
void boot_aop_mini_dump_init(void)
{
  aop_mini_dump_init();
}