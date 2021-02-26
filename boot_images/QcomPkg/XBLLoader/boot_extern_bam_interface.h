#ifndef BOOT_EXTERN_BAM_INTERFACE_H
#define BOOT_EXTERN_BAM_INTERFACE_H
/*===========================================================================

                    BOOT EXTERN BAM DEFINITIONS

DESCRIPTION
  Contains wrapper definition for external api to initialize BAM interface
  

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2012, 2014 by Qualcomm Technologies Incorporated.  All Rights Reserved.
============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    
    
when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/23/14   ck      Removed feature flag as API is always enabled in XBL
09/14/12   kedara   Initial Creation.

===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "bam_drv.h"

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
bam_status_type boot_bam_drv_init(void);

#endif /* BOOT_EXTERN_BAM_INTERFACE_H */
