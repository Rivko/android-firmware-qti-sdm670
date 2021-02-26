#ifndef BOOT_EXTERN_VSENSE_INTERFACE_H
#define BOOT_EXTERN_VSENSE_INTERFACE_H 

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

#include "vsense.h"
#include "boot_sbl_shared.h"

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

 boolean boot_vsense_init(void);
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
void boot_vsense_copy_to_smem(bl_shared_data_type *bl_shared_data);

#endif //BOOT_EXTERN_VSENSE_INTERFACE_H
