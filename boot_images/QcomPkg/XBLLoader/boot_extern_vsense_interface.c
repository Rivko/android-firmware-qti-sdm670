/*===========================================================================

                    BOOT EXTERN VSENSE DRIVER DEFINITIONS

DESCRIPTION
  Contains wrapper definition for external vsense drivers

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2011-2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/23/14   aks      Adding boot_vsense_init_rpm for SMEM access.
===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "boot_extern_vsense_interface.h"
#include "boot_error_if.h"
#include CUST_H

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/ 


/*===========================================================================

**  Function :  boot_vsense_init

** ==========================================================================
*/
/*!
*
*  This function calibrates the voltage sensors on the rails.
*  It should be invoked from the apps boot loader code.
*
* 
*
* @par Dependencies Shared memory must be ready.
*
* @return  FALSE if initialization failed.
*
* @par Side Effects  None.
*
*/
boolean boot_vsense_init(void)
{
  return vsense_init();
}

/*===========================================================================

**  Function :  boot_vsense_copy_to_smem

** ==========================================================================
*/
/*!
*
*  This function copies the calibrated data for the voltage sensor to 
*  shared memory.
*  It should be invoked from the apps boot loader code.
*
* 
*
* @par Dependencies Shared memory must be ready.
*
* @return  FALSE if initialization failed.
*
* @par Side Effects  None.
*
*/
void boot_vsense_copy_to_smem(bl_shared_data_type *bl_shared_data)
{
BL_VERIFY((vsense_copy_to_smem() == TRUE), BL_ERR_VSENSE_COPY_TO_SMEM_FAIL|BL_ERROR_GROUP_BOOT);
}
