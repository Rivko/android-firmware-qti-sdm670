#ifndef BOOT_SMEM_H
#define BOOT_SMEM_H

/*===========================================================================

                       Boot Shared Memory Header File

GENERAL DESCRIPTION
  This file contains functions which use shared memory.

Copyright 2010, 2016 by Qualcomm Technologies Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/11/16   bd      Add API to get SMEM init status flag
10/05/15   ck      Corrected copyright
10/20/10   aus     Added API to call smem_boot_debug_init() 
09/10/10   aus     Saving PMIC power on status to SMEM and moved boot_smem_init_for_dload()
                   to boot_init_for_dload() function.
07/22/10   aus     Initial creation
============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_sbl_shared.h"

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

**  Function :  boot_cache_set_memory_barrier

** ==========================================================================
*/
/*!
* 
* @brief
*   This function will set the location of the actual memory barrier location
*   in shared memory.
*  
* @param[in] bl_shared_data Pointer to shared data structure
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
void boot_cache_set_memory_barrier
(
  bl_shared_data_type *bl_shared_data
);

/*===========================================================================

**  Function :  boot_smem_store_pon_status

** ==========================================================================
*/
/*!
* 
* @brief
*   Store the power on reset status value to smem, so apps processor can access later.
*  
* @par Dependencies
*   Must be called after smem is initialized. ie after smem_boot_init()
*   
* @retval
*   None                                                               
* 
* @par Side Effects
*   None
* 
*/
void boot_smem_store_pon_status
( 
  bl_shared_data_type *bl_shared_data 
);

/*===========================================================================

**  Function :  boot_smem_init

** ==========================================================================
*/
/*!
* 
* @brief
*   Wrapper function that calls smem_boot_init().
*  
* @param[in] bl_shared_data Pointer to shared data structure
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
void boot_smem_init
( 
  bl_shared_data_type *bl_shared_data 
);

/*===========================================================================

**  Function :  boot_smem_debug_init

** ==========================================================================
*/
/*!
* 
* @brief
*    This puts shared memory in debug mode. It changes the behaviour of smem_alloc().
*    1. If no SMEM initialization APIs have been called, smem_alloc() will 
*       call smem_init().
*    2. If smem_boot_debug_init() has been called, but smem_boot_init() has not 
*       been called, smem_alloc() will return NULL.
*    3. If smem_boot_init() or smem_init() have been called, smem_alloc() will 
*       return a pointer to the appropriate item in shared memory.
*  
* @param[in] bl_shared_data Pointer to shared data structure
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
void boot_smem_debug_init
( 
   bl_shared_data_type *bl_shared_data 
);

/*===========================================================================

**  Function :  boot_smem_init_get_status

** ==========================================================================
*/
/*!
*
* @brief
*   Return SMEM initialization status.
* *
* @par Dependencies
*   None
*
* @retval
*   Returns SMEM initialization status
*
* @par Side Effects
*   None
*
*/
boot_boolean boot_smem_get_init_status(void);


#endif  /* BOOT_SMEM_H */

