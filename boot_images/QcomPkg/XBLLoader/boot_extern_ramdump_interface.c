/*===========================================================================

                    BOOT EXTERN SDI DRIVER DEFINITIONS

DESCRIPTION
  Contains wrapper definition for SDI image functionality

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2017 by Qualcomm Technologies Incorporated.  All Rights Reserved.
============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    
    
when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/02/17   yps      Initial Creation.
=============================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "boot_extern_ramdump_interface.h"
#include "boot_error_if.h"
#include "boot_dload.h"

static uint64 stack_pointer=0;

void boot_backup_ramdump_sp(void)
{
	boot_backup_sp((void*)&stack_pointer);
}
/* =============================================================================
**  Function : boot_system_debug_init
** =============================================================================
*/
/**
*   @brief
*   Initialize System Debug Infrastructure.
*
*   @param  bl_shared_data_type
*
*   @retval  None
*
*   @dependencies
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
void boot_restore_ramdump_sp(void)
{
  BL_VERIFY( (stack_pointer != 0), BL_ERR_RAM_DUMP_FAIL);
  boot_restore_sp((void*)&stack_pointer);
}
