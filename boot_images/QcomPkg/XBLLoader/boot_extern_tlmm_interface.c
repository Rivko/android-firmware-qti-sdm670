/*===========================================================================

                    BOOT EXTERN TLMM DRIVER DEFINITIONS

DESCRIPTION
  Contains wrapper definition for external tlmm drivers

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2011, 2014-2015 by Qualcomm Technologies Incorporated.  All Rights Reserved.
============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    
    
when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/26/14   ck      Call to new TlmmLib
10/19/11   dh      Initial Creation.

===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "TlmmLib.h"
#include "HALhwio.h"
#include "HALbootHWIO.h"
#include "boot_extern_tlmm_interface.h"

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/ 

/*===========================================================================

  FUNCTION GPIO_INIT

  DESCRIPTION
        This function initializes the GPIOs for the TLMM configuration
        specified at boot time.

  PARAMETERS
    None.

  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    It uses the stack

===========================================================================*/
boolean boot_gpio_init()
{
  return tlmm_gpio_init();
}
