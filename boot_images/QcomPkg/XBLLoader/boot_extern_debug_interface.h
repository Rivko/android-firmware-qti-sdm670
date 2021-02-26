#ifndef BOOT_EXTERN_DEBUG_INTERFACE_H
#define BOOT_EXTERN_DEBUG_INTERFACE_H
/*===========================================================================

                    BOOT EXTERN DDR DRIVER DEFINITIONS

DESCRIPTION
  Contains wrapper definition for external ddr drivers

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
06/12/17   rp      Updated boot_ddr_enter_self_refresh return type
04/25/17   kpa     Update boot_ddr_exit_self_refresh return type
02/26/15   ck      Removed include of TARGET_DEF_H
02/12/15   kpa     Initial Creation.
===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "boot_error_if.h"

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/ 

/* =============================================================================
**  Function : boot_ddr_enter_self_refresh
** =============================================================================
*/
/**
*   @brief
*   Enter DDR Self Refresh.
*
*   @param  None
*
*   @retval  TRUE if success, else FALSE
*
*   @dependencies
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
boolean boot_ddr_enter_self_refresh(void);

/* =============================================================================
**  Function : boot_ddr_exit_self_refresh
** =============================================================================
*/
/**
*   @brief
*   Enter DDR Self Refresh.
*
*   @param  None
*
*   @retval  TRUE if success, else FALSE.
*
*   @dependencies
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
boolean boot_ddr_exit_self_refresh(void);

#endif /* BOOT_EXTERN_DEBUG_INTERFACE_H */
