/*===========================================================================

                    BOOT EXTERN DDR TRAINING DRIVER DEFINITIONS

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
07/28/15   rp      Added boot_ddr_post_training api
06/08/15   rp	     Updated DDR training API to return a boolean
02/26/15   ck      Removed include of TARGET_DEF_H
02/12/15   kpa     Initial Creation.
=============================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "ddr_common.h"
#include "ddr_drivers.h"
#include "ddr_common_params.h"
#include "ddr_test.h"
#include "boot_error_if.h"

/* =============================================================================
**  Function : boot_ddr_do_ddr_training
** =============================================================================
*/
/**
*   @brief
*   do DDR PHY training.
*
*   @param  None
*
*   @retval  False
*
*   @dependencies
*   Needs to initialize railway and CPR.
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
boolean boot_ddr_do_ddr_training(void)
{
	return FALSE;
}

/* =============================================================================
**  Function : boot_ddr_post_training
** =============================================================================
*/
/**
*   @brief
*   This function takes care of all post training activities.
*
*   @param  None
*
*   @retval  None
*
*   @dependencies
*   Needs to initialize railway and CPR.
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
boolean boot_ddr_post_training(void)
{
  return FALSE;
}
