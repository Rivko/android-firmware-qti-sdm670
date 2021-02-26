#ifndef BOOT_EXTERN_EFS_COOKIE_INTERFACE_H
#define BOOT_EXTERN_EFS_COOKIE_INTERFACE_H
/*===========================================================================

                    BOOT EXTERN EFS COOKIE DRIVER DEFINITIONS

DESCRIPTION
  Contains wrapper definition for external efs cookie drivers

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2014 by Qualcomm Technologies Incorporated.  All Rights Reserved.
============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    
    
when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/30/14   ck      Initial Creation.

===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/


/*===========================================================================

**  Function :  boot_efs_handle_cookies

** ==========================================================================
*/
/*!
*
* @par Description:
*  Handles all the FS cookies that are set.
*  Also loads the EFS partitions from the eMMC card to the RAM.
**************************************************************************/
void boot_efs_handle_cookies (void);


#endif /* BOOT_EXTERN_EFS_COOKIE_INTERFACE_H */
