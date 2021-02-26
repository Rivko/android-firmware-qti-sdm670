#ifndef __PM_UEFI_H__
#define __PM_UEFI_H__
/*===========================================================================


                  PMIC Header File

DESCRIPTION
  This file contains prototype definitions to support interaction
  with the QUALCOMM Power Management ICs.

  Copyright (c) 2011-2015 Qualcomm Technologies, Inc.  All Rights Reserved. 
  Qualcomm Technologies Proprietary and Confidential.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
08/08/15   aab     Added pm_device_setup()
07/23/15   al      Refactoring code
05/20/14   al      Architecture update
11/22/13   va      PmicLib Dec Addition
05/06/13   al      Adding pbs_info and device_info for chargertest 
10/25/12   al      Add api for uefi protocol installation 
07/27/12   al      removed device_index from pm_init
07/27/12   al      Added PmicDeviceIndex.
02/13/11   dy      Created.
===========================================================================*/

/*===========================================================================

                        PMIC INCLUDE FILES

===========================================================================*/
#include <Uefi.h>
#include "com_dtypes.h"
#include "pm_err_flags.h"

/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/
typedef enum
{
  PM_DEVICE_0,
  PM_DEVICE_1,
  PM_DEVICE_2,
  PM_DEVICE_3,
  PM_DEVICE_4,
  PM_DEVICE_INVALID
} pm_device_index;




/*===========================================================================

                        GENERIC FUNCTION PROTOTYPES

===========================================================================*/
/*===========================================================================

FUNCTION pm_uefi_init                                EXTERNAL FUNCTION

DESCRIPTION
    This function initializes the PMIC for operation.

===========================================================================*/
pm_err_flag_type pm_init(void);

/*===========================================================================

FUNCTION pm_install_protocol                                EXTERNAL FUNCTION

DESCRIPTION
    This function installs pmic protocols for uefi

===========================================================================*/
pm_err_flag_type pm_install_protocol(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable);

/*===========================================================================

FUNCTION pm_post_pmic_initialization                                EXTERNAL FUNCTION

DESCRIPTION
    This function executes post pmic initialization calls

===========================================================================*/
pm_err_flag_type pm_post_pmic_initialization(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable);


/*===========================================================================

FUNCTION pm_platform_config                                EXTERNAL FUNCTION

DESCRIPTION
    This function performs target specific configuration after PMIC protocols are installed

===========================================================================*/
pm_err_flag_type pm_platform_config(void);



/*===========================================================================

FUNCTION pm_post_pmic_usb_initialization                                EXTERNAL FUNCTION

DESCRIPTION
    This function performs target specific configuration after PMIC protocols are installed

===========================================================================*/
pm_err_flag_type pm_post_pmic_usb_initialization(void);

#endif /* __PM_UEFI_H__ */
