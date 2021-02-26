#ifndef BOOT_EXTERN_SMEM_INTERFACE_H
#define BOOT_EXTERN_SMEM_INTERFACE_H
/*===========================================================================

                    BOOT EXTERN SMEM DRIVER DEFINITIONS

DESCRIPTION
  Contains wrapper definition for external smem drivers

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2011, 2013-2014 by QUALCOMM Technologies Incorporated.  All Rights Reserved.
============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    
    
when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/23/14   ck      Removed feature flag as API is always enabled in XBL
08/20/14   kedara  Fix compilation warnings.
09/30/13   lm      API declaration to stub image_uuid api when smem driver
				   is not available
02/22/13   dhaval  API declaration to stub image_version api when smem driver
                   is not available
09/08/11   dh      Initial Creation.

===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"
#include "smem.h"
#include "image_version.h"
#include "image_uuid.h"

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/ 

/*===========================================================================

**  Function :  boot_smem_boot_init

** ==========================================================================
*/
/*!
 *Initialize the smem memory allocation structures and the SMEM itself.
 */
void boot_smem_boot_init(void);


/*===========================================================================

**  Function :  boot_smem_boot_debug_init

** ==========================================================================
*/
/*!
* This function may be called at boot to recover enough state from
* shared memory to allow debug to recover state information from
* shared memory that may be used for offline debugging. This
* function will:
*
* (1) Zero the spinlocks
* (2) Configure the local data structures to allow access to
*     shared memory.
*
* Debug may call smem_get_addr() to determine the address and size
* of shared memory items. Debug should not write to the shared
* memory items.
*
* After calling this function, and recovering the relevant
* debugging information, boot must call smem_boot_init() to
* reinitialize shared memory before continuing.
*
* This function is supported only in the first system bootloader.
*
*/
void boot_smem_boot_debug_init(void);


/*===========================================================================

**  Function :  boot_smem_alloc

** ==========================================================================
*/
/*!
*Requests a pointer to a buffer in shared memory.
*
*@param[in] smem_type   Type of memory.
*@param[in] buf_size    Size of the buffer requested. For pre-allocated 
*                       buffers, an ERR_FATAL occurs if the buffer size 
*                       requested does not match the size of the existing 
*                       buffer. If the buf_size is not 64-bit aligned, this 
*                       function increases the size until it is 64-bit aligned.
*
*@return
*NULL if smem_boot_init has not been called yet.\n
*Otherwise, returns either a valid SMEM address to the requested buffer or a 
*fatal error.
*
*@sideeffects
*This function uses spinlocks for exclusive access to the shared memory heap.
*/
void * boot_smem_alloc(smem_mem_type smem_type,
                      uint32 buf_size);


/*===========================================================================

**  Function :  boot_image_version_populate_version

** ==========================================================================
*/
/*!
* 
* @brief
*   This function calls image_version API to populate the image_version in smem   
* 
* @param[in] image_index index in the smem_image_version_table
*
* @par Dependencies
*   smem must have been initialized.
*   
* @retval
*   returns status based on image_version API return value                                                              
* 
* @par Side Effects
*   None
* 
*/
uint32 boot_image_version_populate_version(image_index_type image_index);


  /*===========================================================================

**  Function :  boot_image_version_initialize_version_table

** ==========================================================================
*/
/*!
* 
* @brief
*   This function calls image_version api to initialize image_version_table in
*   smem.
*  
* @param
*   None
*
* @par Dependencies
*   smem must have been initialized.
*   
* @retval
*   returns status based on image_version API return value                                                              
* 
* @par Side Effects
*   review image_version_initialize_version_table function
*/
uint32 boot_image_version_initialize_version_table(void);


/*===========================================================================

**  Function :  boot_image_populate_uuid

** ==========================================================================
*/
/*!
* 
* @brief
*   This function calls image_uuid API to populate the UUID in smem   
* 
* 
* @par Dependencies
*   smem must have been initialized.
*   
* @retval
*   None                                                              
* 
* @par Side Effects
*   None
* 
*/
void boot_image_populate_uuid(void);

#endif /* BOOT_EXTERN_SMEM_INTERFACE_H */

