/*===========================================================================

                    BOOT EXTERN BAM DEFINITIONS

DESCRIPTION
  Contains wrapper definition for external api to initialize BAM interface
  

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2012, 2015 by Qualcomm Technologies Incorporated.  All Rights Reserved.
============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    
    
when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/05/15   ck      Corrected copyright
09/14/12   kedara  Initial Creation.

===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "boot_extern_bam_interface.h"

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/ 

/**
 * @brief Initialize the bam driver
 *
 * This function initializes the bam driver data structures. 
 * Clients of bam driver are not expected to call this function
 * directly. 
 *
 * @return Status of the operation.
 *
 */
bam_status_type boot_bam_drv_init(void)
{
  return bam_drv_init();
}
