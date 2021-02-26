/*===========================================================================

                              SEC Fuse Null Lib

GENERAL DESCRIPTION
  This module provides support to sec fuse lib check. 

Copyright 2017 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who        what, where, why
--------   ---        -------------------------------------------------------
06/22/17   na         Initial revision.
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/


/*===========================================================================
                    VARIABLE DECLARATIONS FOR MODULE
===========================================================================*/

/*===========================================================================
                        LOCAL FUNCTION DEFINITIONS
=============================================================================*/

/*===========================================================================
**  Function :  JTAGDebugDisableFusesBlown
** ==========================================================================
*/
/**
 Check if the debug disable fuses are blown

  @retval TRUE Any of the related DEBUG_DISABLE fuses are blown
  @retval FALSE None of the related DEBUG_DISABLE fuses are blown

**/
BOOLEAN 
EFIAPI
JTAGDebugDisableFusesBlown(void)
{
  return FALSE; //debug disable check failed, debug enabled
}
