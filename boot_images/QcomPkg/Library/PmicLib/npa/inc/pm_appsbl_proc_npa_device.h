#ifndef PM_APPSBL_PROC_NPA_DEVICE_H
#define PM_APPSBL_PROC_NPA_DEVICE_H

/*! \file
 *  
 *  \brief  pm_uefi_proc_npa_device.h ----This file contains prototype definitions npa device layer
 *  \details This file contains prototype definitions npa device layer
 *          and the #DEFINE for the devices
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

//#include "pm_lib_api.h"         /* interface files for all library APIs */
#include "npa.h"
#include "npa_resource.h"

/* Device Remote Resource */
//#define PMIC_NPA_RMT_PROTOCOL_RPM_TYPE    "/protocol/rpm/rpm"

/*===========================================================================

FUNCTION pm_appsbl_proc_npa_device_init

DESCRIPTION
    This function initializes the NPA for appsbl.

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
void pm_appsbl_proc_npa_device_init(void);


#endif //PM_APPSBL_PROC_NPA_DEVICE_H


