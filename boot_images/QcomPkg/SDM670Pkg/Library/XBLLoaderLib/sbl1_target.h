#ifndef SBL1_TARGET_H
#define SBL1_TARGET_H

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

$Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Library/XBLLoaderLib/sbl1_target.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/31/11   dh      Initial revision
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
void sbl1_dload_entry(void);


#endif  /* SBL1_TARGET_H */
