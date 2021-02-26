/*===========================================================================

                    BOOT EXTERN SECCFG DRIVER DEFINITIONS

DESCRIPTION
  Contains wrapper definition for external seccfg driver

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2013-2015 by Qualcomm Technologies Incorporated.  All Rights Reserved.
============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    
    
when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/17/17   yps     Remove boot_qsee_is_retail_unlocked and remove boot_qsee_zero_peripheral_memory implement
07/15/16   yps     Remove Sec lib
10/05/15   ck      Corrected copyright
03/17/14   ck      Added seccfg function for determining if ramdump is allowed
03/17/14   ck      Renamed to boot_extern_seccfg_interface
12/16/13   ck      Initial Creation

===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/

#include "boot_extern_seccfg_interface.h"
#include "seccfg.h"
#include "boot_fastcall_tz.h"
#include "boot_sbl_shared.h"

/*===========================================================================
                      FUNCTION DEFINITIONS
===========================================================================*/ 

/**
 * @brief This function will determine if ram dumps are allowed or not
 *
 * @param[in]
 *   secboot_info  Pointer to shared data
 *
 * @return
 *   Boolean indicating if ram dumps are allowed or not
 *
 * @sideeffects
 *   None
 *
 *
 */
boot_boolean
boot_qsee_is_memory_dump_allowed(secboot_verified_info_type * secboot_info)
{

    boot_tzt_result_regs_t result;

    boot_fastcall_tz(TZ_ALLOWS_MEM_DUMP_CMD,
                     TZ_ALLOWS_MEM_DUMP_CMD_PARAM_ID,
                     secboot_info,
                     0,0,0, &result);

    return result.reg[0];
  }

/**
 * @brief This function will zero out peripheral memory when retail ram dumps are allowed
 *
 * @param[in]
 *   None
 *
 * @return
 *   None
 *
 * @sideeffects
 *   None
 *
 *
 */
void boot_qsee_zero_peripheral_memory(void)
{

}
