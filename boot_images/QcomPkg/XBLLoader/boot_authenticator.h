#ifndef BOOT_AUTHENTICATOR_H
#define BOOT_AUTHENTICATOR_H
/*=============================================================================

                       Boot Authenticator Header File

GENERAL DESCRIPTION
  This module performs SBL binary image authenticating, utilizing the
  functions provided by the PBL fucntion pointers.


Copyright 2010-2014,2016,2017 by QUALCOMM Technologies Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/11/17   kpa     Add header secboot_hw.h
11/23/16   kpa     Update boot_auth_image_hashtable return status
10/27/14   kpa     Added SAFE 1.0
09/03/14   ck      Removed MSA and MBA functions as they are not supported in HoneyBadger.
06/06/14   jz      Restored boot_is_auth_enabled to check return value in the caller
12/10/13   ck      Replaced return value of boot_is_auth_enabled to void since secboot
                   status is checked inside of function
12/06/13   ck      Removed bl_shared_data_type parameter from boot_is_auth_enabled
04/29/12   kedara  Update boot_auth_load_mba_header to check secure boot status
04/15/12   kedara  Added boot_auth_load_mba_header, boot_is_msa_enabled_in_hw.
02/13/13   dh      Add boot_secboot_ftbl_init
11/19/12   dh      Add boot_is_auth_enabled and boot_store_tpm_hash_block
11/08/12   kedara  Update boot_auth_load_header comment,
                   add boot_update_auth_image_info.
09/26/12   kedara  Added boot_get_sbl_auth_verified_info
09/20/12   kedara  Modified the return value of the auth function
03/27/12   dh      Remove secboot_types.h
09/15/11   dh      Removed unused functions
02/08/11   dxiang  Create API to get *ou_field_info
01/28/11   dxiang  Add API for general authentication of images 
11/28/10   plc     Added support for Secure-Boot with Qualcomm Development 
                   certificates 
10/01/10   plc     Initial revision.
===========================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "MIheader.h"
#include "boot_error_if.h"
#include "secboot.h"
#include "secboot_hw.h"
#include "boot_extern_sec_img_interface.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS
                      
===========================================================================*/

/* Forward declarations */
struct bl_shared_data_type;
struct bl_shared_data_type;

/*===========================================================================

                       FUNCTION PROTOTYPES

===========================================================================*/


/*===========================================================================

**  Function : boot_auth_load_header

** ==========================================================================
*/
/*!
* 
* @brief
*    This function loads the header information to be accessed when
*    performing authentication routines.  
*
* @param[in] mi_boot_image_header_type* Pointer to the image header
* 
* @par Dependencies
*    mi_boot_image_header_type* is not NULL
* 
* @retval
*    None
* 
* @par Side Effects
*    None
* 
*/
void boot_auth_load_header( mi_boot_image_header_type* image_header );


/*===========================================================================

**  Function : boot_auth_image

** ==========================================================================
*/
/*!
* 
* @brief
*   This function authenticates a specific image with a specific image_type
*   ID. 
*
* @param[in] bl_shared_data - Pointer to shared data passed between functions
* @param[in] image_type - Secure image type ID
* 
* @par Dependencies
*   Image loaded successfully and boot_auth_load_header() has been called 
* 
* @retval
*    bl_error_boot_type Returns error code in case of authentication error
*                  Returns 0 in case of no error
* 
* @par Side Effects
*    None
* 
*/
bl_error_boot_type boot_auth_image( struct bl_shared_data_type *bl_shared_data, uint32 image_type );

/*===========================================================================
**  Function : boot_get_sbl_auth_verified_info
** ==========================================================================
*/
/*!
* 
* @brief
*   This function returns the address of the sbl_auth_verified_info data 
*   structure. This field is populated during authentication routines, and 
*   may be required by other modules. The secboot_verified_info_type corresponds
*   to the last authenticated image.
*
* @param[in] bl_shared_data - Pointer to shared data passed between functions
* 
* @par Dependencies
*   sbl_auth_verified_info is populated post authentication
* 
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
secboot_verified_info_type * boot_get_sbl_auth_verified_info
(
  struct bl_shared_data_type *bl_shared_data
);

/*===========================================================================

**  Function : boot_update_auth_image_info

** ==========================================================================
*/
/*!
* 
* @brief
*    This function updates the authentication structs to point to correct
*    addresses inside hash segment buffer. This is needed as hash segment now
*    is loaded to a fixed address buffer. But the hash header from compiled
*    image continues to point to incorrect compile time addresses.
*
* @param[in] 
*  uint8* hash_buffer_base_addr:  Pointer to hash buffer base addr
* 
* @par Dependencies
*  uint8* hash_buffer_base_addr* is not NULL.
*  Must be called *after* loading hash segment header and before calling
*  authentication routines.
* 
* @retval
*  None
* 
* @par Side Effects
*  None
* 
*/
void boot_update_auth_image_info( uint8* hash_buffer_base_addr );


/*===========================================================================
**  Function :  boot_store_tpm_hash_block
** ==========================================================================
*/
/*!
* 
* @brief
*   Store hash of loaded image into shared imem location if authentication is enabled
* 
* @param[in] bl_shared_data - Pointer to shared data passed between functions
*
* @param[in] sbl1_auth_verified_info image hash info
*
* @par Dependencies
*   None
*   
* @retval
*   Updated pointer value if ok.
*   NULL if error.
* 
* @par Side Effects
*   None
* 
*/
void boot_store_tpm_hash_block
( 
  struct bl_shared_data_type *bl_shared_data, 
  secboot_verified_info_type *sbl1_auth_verified_info
);


/*===========================================================================

**  Function : boot_is_auth_enabled

** ==========================================================================
*/
/*!
* 
* @brief
*    This function determins if secboot is enabled 
*
* @param[in] bl_shared_data Pointer to shared data passed between functions
*
* @param[out] is_auth_enabled Pointer to a boolean which will be set 
*                             to true if secboot is enabled, false if not
*
* @par Dependencies
*    None
* 
* @retval
*    secboot_hw_etype Returns error code in case of error
*                  Returns E_SECBOOT_HW_SUCCESS in case of no error
* 
* 
*/
secboot_hw_etype boot_is_auth_enabled(boot_boolean *is_auth_enabled);


/*===========================================================================
**  Function :  boot_auth_image_hashtable
** ==========================================================================
*/
/*!
* 
* @brief
*   This function authenticates the image metadata (hashtable)
*
* @par Dependencies
*   None 
* 
* @retval
*   BL_ERR_NONE - If ELF images is verified successfully
*   ERROR VALUES - In case of failure.
* 
*/
bl_error_boot_type boot_auth_image_hashtable 
(
  uint32 image_id, 
  sec_img_auth_elf_info_type *sbl_elf_info, 
  sec_img_auth_whitelist_area_param_t *sbl_white_list_param
);

/*===========================================================================
**  Function :  boot_auth_compute_verify_hash
** ==========================================================================
*/
/*!
* 
* @brief
*   This function loads the data segment into memory using the information
*   from a specific program header and input destination address.  
*
* @param[in] image_id -  image identification number 
*
* @par Dependencies
*   None 
* 
* @retval
*   boolean status - BL_ERR_NONE if authentication was successful
*                    EROR VALUES - In case of failure.
* 
*/
bl_error_boot_type boot_auth_compute_verify_hash( uint32 image_id );

#endif /* BOOT_AUTHENTICATOR_H */
