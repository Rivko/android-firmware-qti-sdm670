#ifndef BOOT_EXTERN_SYSTEM_DEBUG_INTERFACE_H
#define BOOT_EXTERN_SYSTEM_DEBUG_INTERFACE_H
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
===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/ 

/* =============================================================================
**  Function : boot_backup_ramdump_sp
** =============================================================================
*/
/**
*   @brief
*   backup ramdump stack point
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
void boot_backup_ramdump_sp(void);

/* =============================================================================
**  Function : boot_restore_ramdump_sp
** =============================================================================
*/
/**
*   @brief
*   backup ramdump stack point
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
void boot_restore_ramdump_sp(void);


#endif /* BOOT_EXTERN_SYSTEM_DEBUG_INTERFACE_H */
