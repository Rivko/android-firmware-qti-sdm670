#ifndef BOOT_EXTERN_SEC_IMG_INTERFACE_H
#define BOOT_EXTERN_SEC_IMG_INTERFACE_H
/*===========================================================================

                    BOOT EXTERN SECURE IMAGE AUTH DRIVER DEFINITIONS

DESCRIPTION
  Contains wrapper definition for external image authentication drivers

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2014,2016,2017 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    
    
when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/11/17   kpa     header cleanup
02/10/17   kpa     Updates to partial hash api
10/14/16   kpa     Added partial hashing api support, code cleanup
11/23/16   kpa     Update return status for security apis
08/15/15   kpa     Added boot_sec_img_auth_re_enable_debug
09/12/14   plc     Add support for engineering certificates
05/05/14   wg      Initial Creation.

===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/

#include "boot_comdef.h"
#include "sec_img_auth.h"
#include "boot_error_if.h"

/**
 * @brief This function authenticates image Metadata
 *      
 *
 * @param[in,out] sec_img_id           	SW Image ID
 *                sec_img_data          Pointer to ELF header         
 *                                     
 * 				  
 * @return BL_ERR_NONE on success. Appropriate error code on failure.
 *
 * @sideeffects  None
 *
 *
 */
  int boot_sec_img_auth_verify_metadata(uint32 sec_img_id, 
                                 const sec_img_auth_elf_info_type *sec_img_data,
                                 sec_img_auth_whitelist_area_param_t *whitelist_area,
                                 sec_img_auth_verified_info_s *v_info);

/**
 * @brief This function authenticates an ELF images hash segments
 *      
 *
 * @param[in,out] sec_img_id              SW Image ID
 *                v_info                  Pointer to location for verifed info
 *                                   
 *
 * @return BL_ERR_NONE on success. Appropriate error code on failure.
 *
 * @sideeffects  None
 *
 *
 */
  int boot_sec_img_auth_hash_elf_segments(uint32 sec_img_id,
                                      sec_img_auth_verified_info_s *v_info);

/**
 * @brief This function validates the ELF image
 *      
 *
 * @param[in,out] elf_hdr               Pointer to ELF header
 *                                    
 *                        
 *
 * @return BL_ERR_NONE on success. Appropriate error code on failure.
 *
 * @sideeffects  None
 *
 *
 */
  int boot_sec_img_auth_validate_elf(uint32 format, const void *elf_hdr);

/**
 * @brief This function checks if the image segment is valid
 *
 *
 * @param[in,out] format              File format
 *                entry               Pointer to hash segment to be checked
 *                     
 *
 * @return \c TRUE if the segment is valid, \c FALSE otherwise.
 *
 * @sideeffects  None
 *
 *
 */
  boolean boot_sec_img_auth_is_valid_segment(uint32 format, const void *entry);

/**
 * @brief This function checks whether authentication is enabled
 *      
 *
 * @param[in,out] code_seg            Type of code segment descriptor
 *                *is_auth_enabled    Pointer to store auth_en flag. 
 *                                                           
 *
 * @return SEC_IMG_AUTH_SUCCESS on success. Appropriate error code on failure.
 *
 * @sideeffects  None
 *
 *
 */
  sec_img_auth_error_type boot_sec_img_auth_is_auth_enabled(uint32 code_seg, 
																										uint32 *is_auth_enabled);

/**
 * @brief This function initializes module to compute partial hash
 *
 *
 * @param[in] 
 *                hash_buffer         Hash segment of data to be hashed
 *                hash_buff_size      Hash segment size
 *                partial_buff        Pointer to data to be hashed
 *                partial_buff_size   Size of data buffer
 *                segment_num         Index of segment containing data
 *
 * @return BL_ERR_NONE on success. Appropriate error code on failure.
 *
 * @sideeffects  None
 *
 *
 */
int boot_sec_img_auth_partial_hash_init
( 
  void   *hash_buffer, 
  uintnt hash_buff_size,
  void   *partial_buff, 
  uintnt partial_buff_size,
  uintnt segment_num
);

/**
 * @brief This function updates running hash for new data buffer
 *      
 *
 * @param[in] 
 *                hash_buffer         Hash segment of data to be hashed
 *                hash_buff_size      Hash segment size
 *                partial_buff        Pointer to data to be hashed
 *                partial_buff_size   Size of data buffer
 *                segment_num         Index of segment containing data
 *
 * @return BL_ERR_NONE on success. Appropriate error code on failure.
 *
 * @sideeffects  None
 *
 *
 */
int boot_sec_img_auth_partial_hash_update
( 
  void   *hash_buffer, 
  uintnt hash_buff_size,
  void   *partial_buff, 
  uintnt partial_buff_size,
  uintnt segment_num
);

/**
 * @brief This function indicates security module that no further hash data
 *        shall be added and it should compute final hash.
 *      
 *
 * @param[in] 
 *                hash_buffer         Hash segment of data to be hashed
 *                hash_buff_size      Hash segment size
 *                partial_buff        Pointer to data to be hashed
 *                partial_buff_size   Size of data buffer
 *                segment_num         Index of segment containing data
 *
 * @return BL_ERR_NONE on success. Appropriate error code on failure.
 *
 * @sideeffects  None
 *
 *
 */
int boot_sec_img_auth_partial_hash_finalize
( 
  void   *hash_buffer, 
  uintnt hash_buff_size,
  void   *partial_buff, 
  uintnt partial_buff_size,
  uintnt segment_num
);

#endif /* BOOT_EXTERN_SEC_IMG_INTERFACE_H */
