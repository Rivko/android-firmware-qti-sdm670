#ifndef BOOT_SECURE_WATCHDOG_H
#define BOOT_SECURE_WATCHDOG_H

/*===========================================================================

                            boot secure watchdog 
                              Header File

GENERAL DESCRIPTION
  This header file contains declarations and definitions for the secure
  watchdog feature in Bear family chipsets.

Copyright 2013, 2015 by Qualcomm Technologies Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------- 
10/05/15   ck      Corrected copyright
12/27/13   ck      Initial implimentation 
============================================================================*/
 
/*===========================================================================
                              INCLUDES
===========================================================================*/ 
#include "boot_sbl_shared.h"
 
/*===========================================================================
                      PUBLIC DATA DECLARATIONS
===========================================================================*/ 
#define RESET_DEBUG_SW_ENTRY_ENABLE  0x1
 
 
/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/ 


/*===========================================================================

**  Function :  boot_enable_secure_watchdog

** ==========================================================================
*/
/*!
* 
* @brief
*   Enables secure watchdog feature in Bear family chipsets.
*     Enables:
*       PMIC_ABNORMAL_RESET_DEBUG_EN
*       MSM_TSENSE_RESET_DEBUG_EN
*       SECURE_WDOG_DEBUG_EN
*     Clears
*       RESET_ACCESS_FIRST_PASS
*       RESET_DEBUG_FIRST_PASS
*
*   Sets RESET_DEBUG_SW_ENTRY register to SBL Segment #2 (sys_debug) entry point
*
* @param[in]
*   bl_shared_data * - Pointer to SBL shared data structure
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
void boot_secure_watchdog_init(bl_shared_data_type * bl_shared_data);


#endif /* BOOT_SECURE_WATCHDOG_H*/
