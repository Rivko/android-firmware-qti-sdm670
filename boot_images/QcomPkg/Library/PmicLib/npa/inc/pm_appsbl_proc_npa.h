#ifndef PM_UEFI_PROC_NPA_H
#define PM_UEFI_PROC_NPA_H

/*! \file
 *  
 *  \brief  pm_uefi_proc_npa.h ----This file contains prototype definitions processor npa layer
 *  \details This file contains prototype definitions processor npa layer
 *          which consists mainly of the initialization function prototype
 *  
 *    PMIC code generation Version: 1.0.0.0
 *    PMIC code generation NPA Client Version: PM8019_NPA_version_0p09_12_10_2012 - Approved
 *    PMIC code generation NPA Device Setting Value Version: PM8019_NPA_version_0p08_12_4_2012 - Approved
 *    This file contains code for Target specific settings and modes.
 *  
 *    &copy; Copyright (c) 2010-2012 Qualcomm Technologies Incorporated, All Rights Reserved
 */

/*===========================================================================

                EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module over time.


===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/

/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/

/*===========================================================================

                        DEFINITIONS

===========================================================================*/

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/

/*===========================================================================

                FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================

FUNCTION pm_uefi_proc_npa_init

DESCRIPTION
    This function initializes the NPA for uefi.

    It does the following:
    1)  It initializes the PMIC NPA software driver for nodes and resources.

INPUT PARAMETERS
  None.

RETURN VALUE
  None.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void pm_uefi_proc_npa_init(void);

void pm_uefi_proc_core_vol_init(void);

#endif //PM_UEFI_PROC_NPA_H



