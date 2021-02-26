/*===========================================================================

                    BOOT EXTERN APT DRIVER DEFINITIONS

DESCRIPTION
  Contains wrapper definition for external APT drivers

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2013, 2015 by Qualcomm Technologies Incorporated.  All Rights Reserved.
============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    
when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/05/15   ck      Corrected copyright
06/19/13   jihok   Initial Creation.

===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "boot_apt_test.h"

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/ 

/*===========================================================================

  FUNCTION boot_apt_test

  DESCRIPTION
        This function return apt_test() to be used for initialization.

  PARAMETERS
    None.

  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    None.

===========================================================================*/
void boot_apt_test()
{
  apt_test();
}
