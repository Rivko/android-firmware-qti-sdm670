#ifndef BOOT_EXTERN_SYSTEM_DEBUG_INTERFACE_H
#define BOOT_EXTERN_SYSTEM_DEBUG_INTERFACE_H
/*===========================================================================

                    BOOT EXTERN SDI DRIVER DEFINITIONS

DESCRIPTION
  Contains wrapper definition for SDI image functionality

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
06/17/15   as      Initial Creation.
===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "boot_sbl_shared.h"
#include "sysdbg.h"

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/ 

/* =============================================================================
**  Function : boot_system_debug_init
** =============================================================================
*/
/**
*   @brief
*   Initialize System Debug Infrastructure.
*
*   @param  None
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
void boot_system_debug_init(void);

#endif /* BOOT_EXTERN_SYSTEM_DEBUG_INTERFACE_H */
