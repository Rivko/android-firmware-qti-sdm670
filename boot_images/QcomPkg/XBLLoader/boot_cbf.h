#ifndef BOOT_CBF_H
#define BOOT_CBF_H

/*===========================================================================

                     Boot Coherent Bus Fabric Interface

GENERAL DESCRIPTION
 Contains common defintions for functions pertaining to the CBF.

Copyright 2015 by Qualcomm Technologies Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/05/15   ck      Initial creation
============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"


/*===========================================================================
                      PUBLIC DATA DECLARATIONS
===========================================================================*/
/* Settling time in uS that is needed between register writes. */
#define BOOT_CBF_CLAMP_CBFPD_ASSERT_DELAY_US           2
#define BOOT_CBF_CLAMP_CBFPD_DEASSERT_DELAY_US         1
#define BOOT_CBF_POR_RST_DEASSERT_DELAY_US             1

#define BOOT_CBF_ALL_CLAMP_ASSERT_DELAY_US             1
#define BOOT_CBF_APM_TGLSELCBF_DELAY_US                1
#define BOOT_CBF_APM_ENFEW_0_DELAY_US                  1
#define BOOT_CBF_APM_ENFEW_1_DELAY_US                  1
#define BOOT_CBF_APM_ENCBFREST_DELAY_US                1
#define BOOT_CBF_GDHS_ENFEW_0_DELAY_US                 1
#define BOOT_CBF_GDHS_ENFEW_1_DELAY_US                 1
#define BOOT_CBF_GDHS_ENREST_DELAY_US                  1
#define BOOT_CBF_ALL_CLAMP_DEASSERT_DELAY_US           1


/*===========================================================================
                      PUBLIC FUNCTION DECLARATIONS
===========================================================================*/

/*===========================================================================
**  Function :  boot_cbf_initialization
** ==========================================================================
*/
/*!
* 
* @brief  
*   This function configures the CBF and is done prior to bringing up any
*   L3 blocks.
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
void boot_cbf_initialization(void);


/*===========================================================================
**  Function :  boot_cbf_l3_block_initialization
** ==========================================================================
*/
/*!
* 
* @brief  
*   This function configures an L3 block
*
* @par Dependencies
*   Block is expected to be good acording to partial goods info
*
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void boot_cbf_l3_block_initialization(uint32 block_number);


#endif /* BOOT_CBF_H */
