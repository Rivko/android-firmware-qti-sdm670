#ifndef BOOT_EXTERN_ACCESS_CONTROL_INTERFACE_H
#define BOOT_EXTERN_ACCESS_CONTROL_INTERFACE_H
/*===========================================================================

               BOOT EXTERN ACCESS CONTROL DRIVER DEFINITIONS

DESCRIPTION
  Contains wrapper definition for external access control drivers

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
03/25/15   ck      Initial Creation.

===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"


/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/ 

/*===========================================================================

**  Function :  boot_access_control_init

** ==========================================================================
*/
/*!
 *This function initializes the access control functionality..
 */
boolean boot_access_control_init(void);


#endif /* BOOT_EXTERN_ACCESS_CONTROL_INTERFACE_H */

