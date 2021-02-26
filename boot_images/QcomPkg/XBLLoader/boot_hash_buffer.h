#ifndef BOOT_HASH_BUFFER_H
#define BOOT_HASH_BUFFER_H
/*===========================================================================

                    BOOT HASH BUFFER DEFINITIONS

DESCRIPTION
 This module Contains buffer to hold the hash segment for loading elf images.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2012-2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================
                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/11/17   kpa     Update move, restore hash buffer api argument
10/05/15   ck      Corrected copyright
10/14/14   kedara  Add hash-segment-ptr setting prototypes for PMIC elf loading
10/01/14   kedara  Added boot_set_hash_segment_buffer
10/18/13   dhaval  internal api to return hash buffer size to load hash segment
07/09/13   dh      move HASH_SEGMENT_SIZE define to .builds
11/08/12   kedara  Added boot_get_hash_segment_buffer.
09/24/12   kedara  Initial revision.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "boot_comdef.h"

/*=============================================================================

            GLOBAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains global definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

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
uint8 *boot_get_hash_segment_buffer( void );

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
void boot_set_hash_segment_buffer( void *hash_buff_ptr );

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
uint32 boot_get_hash_buffer_size( void );

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
void boot_set_hash_table_auth_result(boolean result);

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
boolean boot_get_hash_table_auth_result( void );

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
void boot_move_hash_segment_buffer( void );

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
void boot_restore_hash_segment_buffer( void );

#endif /* BOOT_HASH_BUFFER_H */
