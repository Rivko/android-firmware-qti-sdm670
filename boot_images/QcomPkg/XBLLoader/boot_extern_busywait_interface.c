/*===========================================================================

                    BOOT EXTERN BUSYWAIT DRIVER DEFINITIONS

DESCRIPTION
  Contains wrapper definition for external busywait drivers

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
05/29/12   dh      Initial Creation.

===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "boot_extern_busywait_interface.h"
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
DALResult boot_busywait_init(void)
{
  return busywait_init();
}


/*===========================================================================

**  Function :  boot_busywait

** ==========================================================================
*/
/*!
  * This function pauses the execution of a thread for a specified time.
 */
void boot_busywait(uint32 pause_time_us)
{
  busywait(pause_time_us);
}
