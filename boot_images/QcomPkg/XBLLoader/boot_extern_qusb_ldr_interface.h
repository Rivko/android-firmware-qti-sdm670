#ifndef BOOT_EXTERN_QUSB_EDL_INTERFACE_H
#define BOOT_EXTERN_QUSB_EDL_INTERFACE_H
/*===========================================================================

                    BOOT EXTERN QUSB DRIVER DEFINITIONS

DESCRIPTION
  Contains wrapper definition for external QUSB drivers

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

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/ 

/*===========================================================================

  FUNCTION boot_qusb_pbl_dload_check

  DESCRIPTION
        This function indicated to transition to forced EDL when D+ is grounded.

  PARAMETERS
    None.

  DEPENDENCIES
    None.

  RETURN VALUE
    boolean - returns whether we should enter PBL EDL mode.

  SIDE EFFECTS
    None.

===========================================================================*/
boolean boot_qusb_ldr_utils_fedl_check(void);

/*===========================================================================

  FUNCTION boot_qusb_ldr_utils_hs_phy_nondrive_mode_set

  DESCRIPTION
        This function puts USB phy in nondrive mode.

  PARAMETERS
    None.

  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    None.

===========================================================================*/
void boot_qusb_ldr_utils_hs_phy_nondrive_mode_set(void);

#endif /* BOOT_EXTERN_QUSB_EDL_INTERFACE_H*/
