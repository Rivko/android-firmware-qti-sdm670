#ifndef SBL1_DDR_TRAINING_H
#define SBL1_DDR_TRAINING_H

/*===========================================================================

                             SBL1 Target Header File

GENERAL DESCRIPTION
  Target-specific initialization and interface APIs.
  
Copyright 2010-2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/14/15   rp      Initial revision
============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_target.h"

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================

**  Function :  sbl_dload_target_entry

** ==========================================================================
*/
/*!
* 
* @brief
*   This function is defined in each SBL Bootloader to handle SBL-specific
*   requirements to enter a download routine
*  
* @par Dependencies
*   None
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void ddr_training_entry(void);


#endif  /* SBL1_DDR_TRAINING_H */
