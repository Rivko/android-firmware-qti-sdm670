/*===========================================================================

                    BOOT EXTERN USB DLOAD DRIVER DEFINITIONS

DESCRIPTION
  Contains wrapper definition for external usb dload drivers

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2015 by Qualcomm Technologies, Inc.  All Rights Reserved.
============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    
    
when        who      what, where, why
--------    ---      ----------------------------------------------------------
01/12/2016  kameya   Renamed file to boot_extern_qusb_ldr_interface.c
09/15/2015  kedara   Support for API to put HighSpeed PHY in non-drive state.
02/13/2015  kedara   Support to check for PBL dload entry

===========================================================================*/

/*
  Note: Only qusb_pbl_dload_check api from USB driver to be used in XBL loader
  to check if USB D+ line is grounded. Rest of the USB driver is in
  XBL ramdump image.
*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "com_dtypes.h"           // common defines - basic types as uint32 etc
#include "boot_extern_qusb_ldr_interface.h"
#include "qusb_ldr_utils.h"

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/

/*===========================================================================

**  Function :  boot_qusb_ldr_utils_fedl_check

** ==========================================================================
*/
/*!
 * 
 * @brief   This function checks if the USB D+ line is grounded.
 *
 * 
 * @param   core_id   -  [IN] USB core ID to initialize
 * 
 *
 * @return  boolean - returns whether we should enter PBL EDL mode.
 * 
 */
boolean boot_qusb_ldr_utils_fedl_check(void)
{
   return qusb_ldr_utils_fedl_check();
}

/*===========================================================================

**  Function :  boot_qusb_ldr_utils_hs_phy_nondrive_mode_set

** ==========================================================================
*/
/*!
 * 
 * @brief   This function puts USB phy in nondrive mode.
 *
 * 
 * @param   None.
 * 
 *
 * @return  None.
 * 
 */
void boot_qusb_ldr_utils_hs_phy_nondrive_mode_set(void)
{
   qusb_ldr_utils_hs_phy_nondrive_mode_set();
}