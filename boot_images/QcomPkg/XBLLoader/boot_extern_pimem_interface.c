/*===========================================================================

                    BOOT EXTERN PIMEM DRIVER DEFINITIONS

DESCRIPTION
  Contains wrapper definition for external pimem drivers

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2015 by Qualcomm Technologies Incorporated.  All Rights Reserved.
============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    
    
when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/17/17   yps     Remove PIMEM init
05/18/15   kedara  Added pimem_get_status
02/11/15   kedara  Initial Creation.
=============================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "boot_extern_pimem_interface.h"
#include "boot_fastcall_tz.h"

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/ 

/* ============================================================================
**  Function : boot_pimem_get_status
** ============================================================================
*/
/*!
*   @brief
*   Return current pimem status
*
*   @details
*
*   @param size_in_mb   -   [OUT] pointer to fill with actual pimem vault size
*
*   @par Dependencies
*
*   @par Side Effects
*   None
*
*   @retval  PIMEM_STATUS - return the current status of pimem
*
*   @sa None
*/

PIMEM_STATUS boot_pimem_get_status(uint32 * size_in_mb)
{
  PIMEM_STATUS pimem_enable_status = -1;
  boot_tzt_result_regs_t result_regs;

  boot_fastcall_tz(TZ_PIMEM_GET_STATUS_CMD,
                      TZ_PIMEM_INIT_CMD_PARAM_ID,
                      0,0,0,0,
                      &result_regs);

  pimem_enable_status = (PIMEM_STATUS)result_regs.reg[0];
  *size_in_mb = (uint32)result_regs.reg[1];
  return pimem_enable_status;
}
