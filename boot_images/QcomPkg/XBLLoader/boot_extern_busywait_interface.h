#ifndef BOOT_EXTERN_BUSYWAIT_INTERFACE_H
#define BOOT_EXTERN_BUSYWAIT_INTERFACE_H
/*===========================================================================

                    BOOT EXTERN BUSYWAIT DRIVER DEFINITIONS

DESCRIPTION
  Contains wrapper definition for external busy wait drivers

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2011-2012, 2014 by Qualcomm Technologies Incorporated.  All Rights Reserved.
============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    
    
when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/23/14   ck      Removed feature flag as API is always enabled in XBL
07/20/12   kpa     Make boot_busywait_init return DAL_SUCCESS  
09/08/11   dh      Initial Creation.

===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"
#include "busywait.h"

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/ 

/*===========================================================================

**  Function :  boot_busywait_init

** ==========================================================================
*/
/*!
 *This function initializes the busywait functionality..
 */
DALResult boot_busywait_init(void);


/*===========================================================================

**  Function :  boot_busywait

** ==========================================================================
*/
/*!
  * This function pauses the execution of a thread for a specified time.
 */
void boot_busywait(uint32 pause_time_us);

#endif /* BOOT_EXTERN_SMEM_INTERFACE_H */

