/*===========================================================================

                  Boot Arch Determination Interface File

GENERAL DESCRIPTION
 Contains common functions pertaining to determining arch of an image.

Copyright 2015 by Qualcomm Technologies Incorporated.  All Rights Reserved.
============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when       who         what, where, why
--------   ---         --------------------------------------------------
02/05/15   ck          Initial revision

===========================================================================*/

/*==========================================================================
                               INCLUDE FILES
===========================================================================*/
#include "boot_arch_determination.h"


const struct aarch64_entry aarch64_entries[] =
  { { 0xFF000000, 0x54000000 },  /* aarch64_b_cond */
    { 0x7F000000, 0x35000000 },  /* aarch64_cbnz */
    { 0x7F000000, 0x34000000 },  /* aarch64_cbz */
    { 0x7F000000, 0x37000000 },  /* aarch64_tbnz */
    { 0x7F000000, 0x36000000 },  /* aarch64_tbz */
    { 0xFC000000, 0x14000000 },  /* aarch64_b */
    { 0xFC000000, 0x94000000 },  /* aarch64_bl */
    { 0xFFFFFC1F, 0xD63F0000 },  /* aarch64_blr */
    { 0xFFFFFC1F, 0xD61F0000 },  /* aarch64_br */
    { 0, 0 }
  };


const struct aarch32_entry aarch32_entries[] =
  { { 0x0F000000, 0x0A000000 },  /* aarch32_thumb32_b_cond */
    { 0xF800D000, 0xF0008000 },  /* aarch32_thumb32_bc_w */
    { 0xF800D000, 0xF0009000 },  /* aarch32_thumb32_bc_w2 */
    { 0xF800D000, 0xF000D000 },  /* aarch32_thumb32_bl */
    { 0x0F000000, 0x0B000000 },  /* aarch32_thumb32_bl_cond */
    { 0xF800D001, 0xF000C000 },  /* aarch32_thumb32_blx */
    { 0xFE000000, 0xFA000000 },  /* aarch32_thumb32_blx2 */
    { 0x0FFFFFF0, 0x012FFF20 },  /* aarch32_thumb32_blx_cond */
    { 0x0FFFFFF0, 0x012FFF10 },  /* aarch32_thumb32_bl_cond2 */
    { 0xFFF0FFFF, 0xF3C08F00 },  /* aarch32_thumb32_bxj */
    { 0x0FFFFFF0, 0x012FFF20 },  /* aarch32_thumb32_bxj_cond */
    { 0xFFF0FFE0, 0xE8D0F000 },  /* aarch32_thumb32_tbb_tbh */
    { 0, 0}
  };

    
const struct thumb_16bit_entry thumb_16bit_entries[] =
  { { 0xFE00, 0xDE00 },  /* thumb16_b_cond */
    { 0xF500, 0xB100 },  /* thumb16_cbnz_cbz */
    { 0xF800, 0xE000 },  /* thumb16_b */
    { 0xFF87, 0x4780 },  /* thumb16_blx */
    { 0xFF87, 0x4700 },  /* thumb16_bx */
    { 0, 0}
  };


/*=========================================================================  
                       FUNCTION DEFINITIONS
=========================================================================*/

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
boot_arch_determination_by_branch(const uint32 * instruction)
{
  int count = 0;


  /* Evaluate for AARCH64. */
  count = 0;
  while (TRUE)
  {
    /* If end of entries is found exit evaluation. */
    if (aarch64_entries[count].opcode == 0)
    {
      break;
    }

    if ((*instruction & aarch64_entries[count].bitmask) ==
        aarch64_entries[count].opcode)
    {
      return BOOT_ARCH_MODE_AARCH64;
    }

    count++;
  }


  /* Evaluate for AARCH32/32bit Thumb. */
  count = 0;
  while (TRUE)
  {
    /* If end of entries is found exit evaluation. */
    if (aarch32_entries[count].opcode == 0)
    {
      break;
    }

    if ((*instruction & aarch32_entries[count].bitmask) ==
        aarch32_entries[count].opcode)
    {
      return BOOT_ARCH_MODE_AARCH32;
    }

    count++;
  }
  

  /* Evaluate for 16bit Thumb. */
  count = 0;
  while (TRUE)
  {
    /* If end of entries is found exit evaluation. */
    if (thumb_16bit_entries[count].opcode == 0)
    {
      break;
    }

    if (((uint16)*instruction & thumb_16bit_entries[count].bitmask) ==
        thumb_16bit_entries[count].opcode)
    {
      return BOOT_ARCH_MODE_AARCH32;
    }

    count++;
  }
  

  return BOOT_ARCH_MODE_UNKNOWN;
}
