#ifndef BOOT_EXTERN_SECCFG_INTERFACE_H
#define BOOT_EXTERN_SECCFG_INTERFACE_H


/*===========================================================================

                    BOOT EXTERN SECCFG DRIVER DEFINITIONS

DESCRIPTION
  Contains wrapper definition for external seccfg driver

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2013-2014 by QUALCOMM Technologies Incorporated.  All Rights Reserved.
============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    
    
when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/17/17   yps     Remove boot_qsee_is_retail_unlocked and remove boot_qsee_zero_peripheral_memory implement
09/23/14   ck      Removed feature flag as API is always enabled in XBL
07/31/14   ck      Changed static boot_qsee_is_retail_unlocked to return FALSE
04/25/14   ck      When seccfg library does not exist functions must be inlined
03/17/14   ck      Added seccfg function for determining if ramdump is allowed
03/17/14   ck      Renamed to boot_extern_seccfg_interface
12/16/13   ck      Initial Creation

===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/

#include "boot_comdef.h"
#include "secboot.h"


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
boot_qsee_is_memory_dump_allowed(secboot_verified_info_type * secboot_info);


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
void boot_qsee_zero_peripheral_memory(void);

#endif /* BOOT_EXTERN_SECCFG_INTERFACE_H */
