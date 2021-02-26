/*===========================================================================

                    BOOT EXTERN DDI  DEFINITIONS

DESCRIPTION
  Contains wrapper definition for external ddi

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2017 by Qualcomm Technologies Incorporated.  All Rights Reserved.
============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    
    
when       who        what, where, why
--------   ---         ----------------------------------------------------------
07/14/17   daison      Initial Creation.
=============================================================================*/

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/ 

#include "boot_extern_ddi_interface.h"
#include "boot_dload.h"

/* ============================================================================
**  Function : boot_ddi_entry
** ============================================================================
*/
/*!
*   @brief
*   DDI entry function
*
*   @param  None.
*
*   @par Dependencies
*   None
*
*   @par Side Effects
*   None
*
*   @retval None
*
*   @sa None
*/
void boot_ddi_entry(void)
{
  if(!boot_dload_is_dload_mode_set())
  {
    ddi_entry();
  }
}

