
/*===========================================================================

                    BOOT HASH BUFFER DEFINITIONS

DESCRIPTION
 This module Contains buffer to hold the hash segment for loading elf images.
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2012-2014,2016,2017 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================
                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/11/17   kpa     Update move, restore hash buffer api argument
01/20/17   kpa     Update .bss.BOOT_INTERNAL_HEAP to .bss.BOOT_INTERNAL_MEMZI
10/06/16   kpa     Flush hash segment data after copying.
10/14/14   kedara  Added support for loading PMIC elf image
10/01/14   kedara  Added boot_set_hash_segment_buffer
06/11/14   kedara  Updates to support 64 bit compilation on llvm.
05/12/14   kedara  Include boot_target.h for HASH_SEGMENT_SIZE define.
12/26/13   ck      New functions added boot_get_hash_buffer_size, boot_set_hash_table_auth_result
                   and boot_get_hash_table_auth_result
12/09/13   jz      Ported from fix for blacklist CR 562437
11/08/12   kedara  Added boot_get_hash_segment_buffer.
11/01/13   plc     Update for Arm6 syntax (no "inline")
09/24/12   kedara  Initial revision.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "boot_hash_buffer.h"
#include "boot_comdef.h"
#include "boot_target.h"
#include "boot_util.h"
#include "boot_cache.h"

/*=============================================================================

            GLOBAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains global definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

/* A global buffer to use for loading hash segment. This is shared by both
  elf loader and sahara modules
 */

/* A global buffer to use for loading hash segment. This is shared by both
  elf loader and sahara modules
 */
static uint8 ALIGN(32) boot_hash_segment[HASH_SEGMENT_SIZE] SECTION(".bss.BOOT_DDR_ZI_DATA_ZONE");
static boolean is_elf_hash_table_auth_result SECTION(".bss.BOOT_INTERNAL_MEMZI") = FALSE;

uint8 *boot_hash_segment_ptr = boot_hash_segment;

/**/
#ifndef HASH_SEGMENT_TEMP_BASE
#define HASH_SEGMENT_TEMP_BASE 0x0
#endif /*HASH_SEGMENT_TEMP_BASE*/


/*===========================================================================

**  Function : boot_get_hash_segment_buffer

** ==========================================================================
*/
/*!
* 
* @brief
*    This function returns the address of buffer to be used for loading hash
*    segment
*
* @param[in] 
*  None
* 
* @par Dependencies
*  None
* 
* @retval
*  (uint8 *) Pointer to buffer for loading hash segment.
* 
* @par Side Effects
*    None
* 
*/
uint8 *boot_get_hash_segment_buffer( void )
{
  return boot_hash_segment_ptr;
}

/*===========================================================================

**  Function : boot_set_hash_segment_buffer

** ==========================================================================
*/
/*!
* 
* @brief
*    This function sets the global hash buffer with provided value
*
* @param[in] 
*  hash_buff_ptr:  Pointer to Hash segment data to be used.
* 
* @par Dependencies
*  None
* 
* @retval
*  none
* 
* @par Side Effects
*    None
* 
*/
void boot_set_hash_segment_buffer( void *hash_buff_ptr )
{
  if(hash_buff_ptr != NULL)
  {
    qmemcpy(boot_hash_segment_ptr, hash_buff_ptr, HASH_SEGMENT_SIZE);
    
    /* Flush the hash segment data region after copying over */
    dcache_flush_region((void *)boot_hash_segment_ptr, HASH_SEGMENT_SIZE);
  }
}

/*===========================================================================

**  Function : boot_get_hash_buffer_size

** ==========================================================================
*/
/*!
* 
* @brief
*    This function returns the size of buffer to be used for loading hash
*    segment
*
* @param[in] 
*  None
* 
* @par Dependencies
*  None
* 
* @retval
*  uint32 size of the hash segment buffer.
* 
* @par Side Effects
*    None
* 
*/
uint32 boot_get_hash_buffer_size( void )
{
  return HASH_SEGMENT_SIZE;
}

/*===========================================================================

**  Function : boot_set_hash_table_auth_result

** ==========================================================================
*/
/*!
* 
* @brief
*    This function sets the hash table authentication result (TRUE, FALSE)
*
* @param[in] 
*  uint32  result  -  caller sets TRUE, FALSE for this result variable 
* 
* @par Dependencies
*  None
* 
* @retval
*  None
* 
* @par Side Effects
*    None
* 
*/
void boot_set_hash_table_auth_result(boolean result)
{
  is_elf_hash_table_auth_result = result;
}

/*===========================================================================

**  Function : boot_get_hash_table_auth_result

** ==========================================================================
*/
/*!
* 
* @brief
*    This function returns the variable holding status for hash table
*    authentication result (TRUE, FALSE).
*
* @param[in] 
*  None 
* 
* @par Dependencies
*  None
* 
* @retval
*  boolean is_elf_hash_table_auth_result   
* 
* @par Side Effects
*    None
* 
*/
boolean boot_get_hash_table_auth_result( void )
{
  return is_elf_hash_table_auth_result;
}

/*===========================================================================

**  Function : boot_move_hash_segment_buffer

** ==========================================================================
*/
/*!
* 
* @brief
*    This function moves the hash segment buffer to temporary location 
*    for supporting loading of pmic.mbn image, before DDR is initialized.
*
* @param[in] 
*  None
* 
* @par Dependencies
*  None
* 
* @retval
*  None   
* 
* @par Side Effects
*    None
* 
*/
void boot_move_hash_segment_buffer( void )
{
  boot_hash_segment_ptr = (uint8 *)HASH_SEGMENT_TEMP_BASE;
}

/*===========================================================================

**  Function : boot_restore_hash_segment_buffer

** ==========================================================================
*/
/*!
* 
* @brief
*    This function restores the hash segment buffer back to standard location 
*    after loading of pmic.mbn image has completed, since DDR is initialized.
*
* @param[in] 
*  None
* 
* @par Dependencies
*  None
* 
* @retval
*  None   
* 
* @par Side Effects
*    None
* 
*/
void boot_restore_hash_segment_buffer( void )
{
  boot_hash_segment_ptr = boot_hash_segment;
}
