#ifndef BOOT_HYDRA_H
#define BOOT_HYDRA_H

/*===========================================================================

                          Boot Hydra Interface

GENERAL DESCRIPTION
 Contains common defintions for functions pertaining to the Hydra CPU.

Copyright 2014-2015 by Qualcomm Technologies Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/25/15   as      Update PMIC APCC and QLL Enable Support.
07/30/15   as      Add QLL Enable Support.
05/28/15   as      Add PMIC APCC Enable Support.
10/29/14   ck      Initial creation
============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"


/*===========================================================================
                      PUBLIC DATA DECLARATIONS
===========================================================================*/
/* Settling time in uS that is needed between register writes. */
#define BOOT_HYDRA_PM_APCC_PWR_CTL_DELAY_US                     10000
#define BOOT_HYDRA_L2_PWR_CTL_DELAY_US                          10000
#define BOOT_HYDRA_CORE_PWR_CTL_DELAY_US                        10000
#define BOOT_MBSU_HYDRASR_QLL_DELAY_US                          10000
#define BOOT_DMSU_HYDRASR_QLL_DELAY_US                          10000


/* All of the bitmask values for the PMIC APCC, L2 and Core blocks do not change.
   No need to duplicate redundant data. */
#define BOOT_HYDRA_PM_APCC_PWR_CTL_PMIC_APC_ON_BITMASK     0x80000000

#define BOOT_HYDRA_L2_PWR_CTL_CLAMP_APMPD_BITMASK               0x040
#define BOOT_HYDRA_L2_PWR_CTL_HS_DSBL_BITMASK                   0x010
#define BOOT_HYDRA_L2_PWR_CTL_POR_RESET_BITMASK                 0x004
#define BOOT_HYDRA_L2_PWR_CTL_CLAMP_L2PD_BITMASK                0x001

#define BOOT_HYDRA_CORE_PWR_CTL_CORE_PWRD_UP_BITMASK            0x100
#define BOOT_HYDRA_CORE_PWR_CTL_CLAMP_APMPD_BITMASK             0x040
#define BOOT_HYDRA_CORE_PWR_CTL_HS_DSBL_BITMASK                 0x010
#define BOOT_HYDRA_CORE_PWR_CTL_POR_RESET_BITMASK               0x004
#define BOOT_HYDRA_CORE_PWR_CTL_CLAMP_CPUPD_BITNMASK            0x001


/*===========================================================================
                      PUBLIC FUNCTION DECLARATIONS
===========================================================================*/

/*===========================================================================
**  Function :  boot_hydra_deassert_reset
** ==========================================================================
*/
/*!
* 
* @brief  
*   This function configures the hydra and then brings it out of reset.
*
* @par Dependencies
*   None
*
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void boot_hydra_deassert_reset(uint8 core_number,
                               uint64 entry_point,
                               boot_boolean secure_mode,
                               boot_boolean aarch64_mode);


/*===========================================================================
**  Function :  boot_hydra_configure_pmic_apcc
** ==========================================================================
*/
/*!
*
* @brief
*   This function configures the pmic apc rail for cores present.
*
* @par Dependencies
*   Busywait must have already been initialized
*
* @retval
*   None
*
* @par Side Effects
*   None
*
*/
void boot_hydra_configure_pmic_apcc(uint32 cpu_status);


/*===========================================================================
**  Function :  boot_hydra_configure_qll_sus
** ==========================================================================
*/
/*!
*
* @brief
*   This function configures the QLL MBSU/DMSU for cores present.
*
* @par Dependencies
*   Busywait must have already been initialized
*
* @retval
*   None
*
* @par Side Effects
*   None
*
*/
void boot_hydra_configure_qll_sus(uint32 cpu_status);


#endif /* BOOT_HYDRA_H */
