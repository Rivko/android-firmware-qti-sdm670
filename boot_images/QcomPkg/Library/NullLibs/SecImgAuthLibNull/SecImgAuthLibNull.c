/** @file SecImgAuthLibNull.c
  
  Stub functions for SecImgAuthLib

  Copyright (c) 2014, Qualcomm Technologies, Inc. All rights reserved.
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 05/14/15   plc     Update for post 8996 BaseTargetPkg 
 11/18/14   kedara  Initial revision

=============================================================================*/


/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "com_dtypes.h"
#include "sec_img_auth.h"

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/ 


/**
 * @brief Authenticates hash table segment against its signature.
 *
 * @param[in] sec_img_id  Secure Image Identifier 
 * @param[in] elf_hdr     Pointer to the ELF header 
 *
 * @return 0 on success, negative on failure.
 */
sec_img_auth_error_type sec_img_auth_verify_metadata(uint32 sec_img_id, 
                                 const sec_img_auth_elf_info_type *sec_img_data,
                                 sec_img_auth_whitelist_area_param_t *whitelist_area,
                                 sec_img_auth_verified_info_s *v_info)
{
  return SEC_IMG_AUTH_SUCCESS;
}

/**
 * @brief Validates ELF segments against hash table 
 *
 * @param[in] sec_img_id  Secure Image Identifier 
 * @return 0 on success, negative on failure
 */
sec_img_auth_error_type sec_img_auth_hash_elf_segments(uint32 sec_img_id,
                                      sec_img_auth_verified_info_s *v_info)
{
  return SEC_IMG_AUTH_SUCCESS;
}

/**
 * @brief Elf header parser, to check for the correct format.
 *
 * @param[in] elf_hdr   Pointer to the ELF header.
 *
 * @result \c TRUE if the header is in correct format, \c FALSE otherwise.
 */

sec_img_auth_error_type sec_img_auth_validate_elf(const void* elf_hdr)
{
  return SEC_IMG_AUTH_SUCCESS;
}

/**
 * @brief Check for loadable, non paged segments
 *
 * @param[in] Program hdr entry for the segment
 *
 * @result \c TRUE if the segment is valid, \c FALSE otherwise.
 */
boolean sec_img_auth_is_valid_segment(uint32 format, const void * entry)
{
  uint32 phdr_type;
  uint32 phdr_flags;
  
  if( entry == NULL)
  {
    return FALSE;
  }

  phdr_type = (format == ELF_CLASS_64) ? ((Elf64_Phdr *)entry)->p_type : ((Elf32_Phdr *)entry)->p_type;
  phdr_flags = (format == ELF_CLASS_64) ? ((Elf64_Phdr *)entry)->p_flags : ((Elf32_Phdr *)entry)->p_flags;

  return (  phdr_type == MI_PBT_ELF_PT_LOAD  &&
            MI_PBT_PAGE_MODE_VALUE(phdr_flags) == MI_PBT_NON_PAGED_SEGMENT  &&
            MI_PBT_ACCESS_TYPE_VALUE(phdr_flags) != MI_PBT_NOTUSED_SEGMENT  &&
           /* hash segment to be loaded explicitly to fixed buffer */
            MI_PBT_SEGMENT_TYPE_VALUE(phdr_flags) != MI_PBT_HASH_SEGMENT   &&
            MI_PBT_ACCESS_TYPE_VALUE(phdr_flags) != MI_PBT_SHARED_SEGMENT  );
}

/**
 * @brief Check if the image auth is enabled or not
 *
 * @param[in] code_seg - code segment
 *
 * @param[in, out] is_auth_enabled - the flag for the image auth to be enabled or not
 *
 * @return 0 on success, negative on failure
 */
sec_img_auth_error_type sec_img_auth_is_auth_enabled(uint32 code_seg, uint32* is_auth_enabled)
{
  return SEC_IMG_AUTH_SUCCESS;
}

/**
 * @brief Initialize security internal struct secboot_verified_info_type
 *
 * @param[in] secboot_verified_info_type 
 *
 * @param[out] None
 *
 * @return sec_img_auth_error_type : status
 */
sec_img_auth_error_type sec_img_auth_init
(
  secboot_verified_info_type *pbl_verified_info
)
{
  return SEC_IMG_AUTH_SUCCESS;
}




