#ifndef BOOT_EXTERN_DEVICEPROGRAMMER_LITE_INTERFACE_H
#define BOOT_EXTERN_DEVICEPROGRAMMER_LITE_INTERFACE_H

/*===========================================================================

                    BOOT EXTERN DEVICEPROGRAMMER DRIVER DEFINITIONS

DESCRIPTION
  Contains wrapper definition for deviceprogrammer main function

INITIALIZATION AND SEQUENCING REQUIREMENTS
  DDR must be initialized
  
Copyright 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    
    
when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/14/14   wek     Indicate that deviceprogrammer functions don't return.
05/23/14   wek     Initial Creation.

===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/

#include "boot_sbl_if.h"

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/ 

/*===========================================================================

**  Function :  boot_deviceprogrammer_lite_main

** ==========================================================================
*/
/*!
 * Run deviceprogrammer before ddr has been initialized.
 *
 * @param bl_shared_data
 *    The shared bootloader information.
 *
 * @return
 *    This function never returns.
 *
*****************************************************************************/
#ifdef FEATURE_BOOT_EXTERN_DEVICEPROGRAMMER_LITE_COMPLETED
  void boot_hand_control_to_deviceprogrammer_lite_main
  (
   boot_pbl_shared_data_type *pbl_shared_data
  ) __attribute__((noreturn));

#else
  void boot_hand_control_to_deviceprogrammer_lite_main
  (
   boot_pbl_shared_data_type *pbl_shared_data
  );
#endif


#endif  /* BOOT_EXTERN_DEVICEPROGRAMMER_LITE_INTERFACE_H */
