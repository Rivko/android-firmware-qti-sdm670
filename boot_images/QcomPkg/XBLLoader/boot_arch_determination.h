#ifndef BOOT_ARCH_DETERMINATION_H
#define BOOT_ARCH_DETERMINATION_H

/*===========================================================================

                     Boot Arch Determination Interface

GENERAL DESCRIPTION
 Contains common defintions for functions pertaining to runtime determination
 of architecture mode.

Copyright 2015 by Qualcomm Technologies Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/21/15   ck      Initial creation
============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"


/*===========================================================================
                      PUBLIC DATA DECLARATIONS
===========================================================================*/
typedef enum
{
  BOOT_ARCH_MODE_UNKNOWN = 0,
  BOOT_ARCH_MODE_AARCH32,
  BOOT_ARCH_MODE_AARCH64
} boot_arch_mode_type;


struct aarch64_entry
{
  uint32 bitmask;
  uint32 opcode;
};


struct aarch32_entry
{
  uint32 bitmask;
  uint32 opcode;
};


struct thumb_16bit_entry
{
  uint16 bitmask;
  uint16 opcode;
};


/*===========================================================================
                      PUBLIC FUNCTION DECLARATIONS
===========================================================================*/

/*===========================================================================
**  Function :  boot_arch_determination_by_branch
** ==========================================================================
*/
/*!
* 
* @brief  
*   This function takes a pointer to what should be a branch instruction.
*   The function determines if it is a thumb, aarch32, or aarch64 instruction.
*   If the mode cannot be determined it means the instruction is not a branch.
*
* @par Dependencies
*   None
*
* @retval
*   BOOT_ARCH_MODE_TYPE
* 
* @par Side Effects
*   None
* 
*/
boot_arch_mode_type
boot_arch_determination_by_branch(const uint32 * instruction);


#endif /* BOOT_ARCH_DETERMINATION_H */
