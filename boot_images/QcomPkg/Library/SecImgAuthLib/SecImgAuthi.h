#ifndef SEC_PIL_AUTHI_H
#define SEC_PIL_AUTHI_H

/**
@file sec_img_auth.h
@brief Secure Image Auth Security implementation
*/

/*===========================================================================
   
* Copyright (c) 2013-2014
* Qualcomm Technologies Incorporated.
* All Rights Reserved.
* Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE


when       who      what, where, why
--------   ---      ------------------------------------
01/29/15   kpa     Increase SEC_IMG_AUTH_PAGE_2K size to accomodate max 30
                   elf segments
11/23/13   mm      Support for Image Authentication in ROM
04/05/12   vg      Added segmented authentication changes.
02/28/12   vg      Fixed review comments
02/20/12   vg      Ported from TZ PIL.

===========================================================================*/
#include "UIE.h"
#include "sec_img_auth.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

#define SEC_IMG_AUTH_PAGE_2K           2800

#define UINT32_MAX_VALUE 0xFFFFFFFF
#define UINT64_MAX_VALUE 0xFFFFFFFFFFFFFFFFULL

#define BREAKIF(cond)  { if (cond) break; }

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

typedef struct sec_img_auth_secboot_auth_info_s
{
  uint32 auth_enabled;
  uint32 code_seg;
}sec_img_auth_secboot_info_t;

typedef struct sec_img_elf_prog_hdr_32
{
  Elf32_Ehdr  *elf_hdr;       /**< Pointer to the elf header */
  Elf32_Phdr  *prog_hdr;      /**< Pointer to the start of the program header; */
}sec_img_elf_prog_hdr_32_type;

typedef struct sec_img_elf_prog_hdr_64
{
  Elf64_Ehdr  *elf_hdr;       /**< Pointer to the elf header */
  Elf64_Phdr  *prog_hdr;      /**< Pointer to the start of the program header; */
}sec_img_elf_prog_hdr_64_type;

typedef union sec_img_elf_prog_hdr 
{
  sec_img_elf_prog_hdr_32_type  hdr_32;       /**< Pointer to the elf header */
  sec_img_elf_prog_hdr_64_type  hdr_64;       /**< Pointer to the elf header */
} sec_img_elf_prog_hdr_type;

typedef struct sec_img_auth_elf_info_s
{
  uint32 elf_format;
  sec_img_elf_prog_hdr_type  elf_prg_hdr;  /**< Pointer to the elf header */
  uint32      prog_hdr_num;   /**< Number of program header entries */
  uint8*      hash_seg_hdr;   /**< Pointer to the hash segment header*/
  uint32      hash_seg_hdr_sz; /**< Hash segment header size in bytes */
  uint8*      hash_seg;       /**< Pointer to the hash segment */
  uint32      hash_array_sz;    /**< Hash Array size in bytes */
  uint8*      sig_ptr;        /**< Pointer to the signature */
  uint32      sig_sz;         /**< Signature Size */
  uint8*      cert_ptr;       /**< Pointer to the certificate */
  uint32      cert_sz;        /**< Size of the certificate */
  uint8*      uie_hdr_ptr;    /**< Pointer to the UIE header */
  sec_img_auth_whitelist_area_param_t *whitelist_area; /* Whitelist to be used for validating the segments*/
} sec_img_auth_elf_info_t;

typedef struct sec_img_auth_priv_s
{
  sec_img_auth_elf_info_t elf;  /* Elf Info */
  uint32 img_id;  /* Image Id */
  sec_img_auth_state_et state;  /* State */
  sec_img_auth_secboot_info_t auth_info; /* Auth Info containing the result */
  uie_ctx * uie;
} sec_img_auth_priv_t;

typedef struct sec_img_auth_seg_auth_info_s
{
  uint8  cur_prog_hdr;
  uint32 img_id;
  boolean init;  
  uint8 *read_ptr;
  uint32 len_read;  
  uint8 *hash;
  uint32 hash_len;
  uint32 cur_elf_len;  
} sec_img_auth_seg_auth_info_t;

/*----------------------------------------------------------------------------
 * Function Declarations
 * -------------------------------------------------------------------------*/
 
/**
 * @brief Verify the hash on all loadable program segments
 *
 * @param[in] img_id  Image Id
 * @param[in] elf   ELF header of the loaded image
 *
 * @return 0 on success, negative on failure
 */
sec_img_auth_error_type 
sec_img_auth_segments(
  const sec_img_auth_priv_t *img_auth_handle,
  uint32 img_id,
  const sec_img_auth_elf_info_t *elf,
  sec_img_auth_verified_info_s *v_info); 

/**
 * @brief This function checks if the image associated with the code segment
 *        needs to be authenticated. If authentication is required, callers
 *        MUST authenticate the image successfully before allowing it to execute.          
 *
 * @param[in] Code segment in SECURE_BOOTn register containing authentication 
 *            information of the image.
 * @param[in,out] Pointer to a uint32 indicating whether authentication is required. 
 *                Will be populated to 0 if authentication is not required, 
 *                1 if authentication is required. 
 * @return 0 on success, negative on failure
 */
sec_img_auth_error_type 
sec_img_auth_hw_is_auth_enabled(uint32 sec_img_id, 
  sec_img_auth_secboot_info_t *auth_info);

/**
 * @brief This function verifies hash table segment signature and validates
 *        ELF and program headers hash against thier entry in hash table.          
 *
 * @param[in] img_id Image Id 
 * @param[in] elf  Pointer to the ELF info of the loaded image.
 *
 * @return 0 on success, negative on failure
 */
sec_img_auth_error_type 
sec_img_auth_verify_sig(uint32 img_id,
  const sec_img_auth_priv_t *img_auth_info,
  sec_img_auth_verified_info_s *v_info);

/**
 * @brief Find the elf segment that matches seg_flags
 *
 * @param[in] phdr The program header entry start ptr
 * @param[in] end The last program header entry ptr
 * @param[in] seg_flags The type of the segment
 *
 * @return - NULL if no match if found
 */
void *sec_img_auth_elf_find_seg(const sec_img_auth_priv_t *img_data_handle , uint32 seg_flags);

sec_img_auth_error_type sec_img_auth_validate_hash_segment_entries_32
(
  const Elf32_Phdr * phdr, 
  const Elf32_Phdr * end,
  uint32 seg_flags
);

sec_img_auth_error_type sec_img_auth_validate_hash_segment_entries_64
(
  const Elf64_Phdr * phdr, 
  const Elf64_Phdr * end,
  uint32 seg_flags
);

/**
   Checks if there hash segment size matches the expected size.

   @param[in] Elf header info struct
*/
sec_img_auth_error_type sec_img_auth_validate_hash_segment_len_32
(
  sec_img_auth_elf_info_t * img_data
);


/**
   Checks if there hash segment size matches the expected size.

   @param[in] Elf header info struct
*/
sec_img_auth_error_type sec_img_auth_validate_hash_segment_len_64
(
  sec_img_auth_elf_info_t * img_data
);

/**
 * Checks if the complete image is in valid area
 *
 * @param[in] img_data    Image Information
 * @param[in] whitelist_area    Whitelist regions.
 *
 * @param \c TRUE if the memory range is valid, \c FALSE otherwise.
 */
sec_img_auth_error_type 
sec_img_auth_validate_image_area_32
(
  const sec_img_auth_elf_info_type * img_data,
  sec_img_auth_whitelist_area_param_t *whitelist_area,
  sec_img_auth_verified_info_s * v_info
);

/**
 * Checks if the complete image is in valid area
 *
 * @param[in] img_data    Image Information
 * @param[in] whitelist_area    Whitelist regions.
 *
 * @param \c TRUE if the memory range is valid, \c FALSE otherwise.
 */
sec_img_auth_error_type 
sec_img_auth_validate_image_area_64
(
  const sec_img_auth_elf_info_type * img_data,
  sec_img_auth_whitelist_area_param_t *whitelist_area,
  sec_img_auth_verified_info_s * v_info
);


sec_img_auth_error_type 
sec_img_auth_validate_elf_entry_32
(
  uint32 sec_img_id,
  sec_img_auth_whitelist_area_param_t *whitelist_area,
  const sec_img_auth_priv_t * img_data
);

sec_img_auth_error_type 
sec_img_auth_validate_elf_entry_64
(
  uint32 sec_img_id,
  sec_img_auth_whitelist_area_param_t *whitelist_area,
  const sec_img_auth_priv_t * img_data
);


/**
 * @brief Verify if the boot header has the correct lengths
 *
 * @param[in] mi Pointer to the header of the hash segment
 *
 * @return \c TRUE if the boot header lengths are correct, otherwise \c FALSE.
 */
sec_img_auth_error_type
sec_img_auth_validate_mi_boot 
(
 const sec_img_auth_priv_t *s_img_auth_handle,
const mi_boot_image_header_type * mi
);


sec_img_auth_error_type
sec_img_auth_populate_elf_info_32 
(
  uint32 sec_img_id,
  const sec_img_auth_elf_info_type *img_data,
  sec_img_auth_priv_t * img_auth_handle
);

sec_img_auth_error_type
sec_img_auth_populate_elf_info_64
(
  uint32 sec_img_id,
  const sec_img_auth_elf_info_type *img_data,
  sec_img_auth_priv_t * img_auth_handle
);

/**
 * @brief In all segments if memory size is greater than file size then zero out
 *        the memory area of size equal to difference between memory size and file size.
 *        In general this difference is maintained to capture ZI data.
 *
 * @note Also verify that the segment size looks correct
 *
 * @param[in] elf Pointer to the ELF segment
 *
 * @return 0 on success, negative on failure
 */
sec_img_auth_error_type 
sec_img_auth_zero_segments_32
(
 const sec_img_auth_elf_info_t *elf,
 sec_img_auth_verified_info_s *v_info
);


/**
 * @brief In all segments if memory size is greater than file size then zero out
 *        the memory area of size equal to difference between memory size and file size.
 *        In general this difference is maintained to capture ZI data.
 *
 * @note Also verify that the segment size looks correct
 *
 * @param[in] elf Pointer to the ELF segment
 *
 * @return 0 on success, negative on failure
 */
sec_img_auth_error_type 
sec_img_auth_zero_segments_64
(
 const sec_img_auth_elf_info_t *elf,
 sec_img_auth_verified_info_s *v_info
);

/**
 * Checks if the complete image is in valid area
 *
 * @param[in] start   Start of the memory area - inclusive.
 * @param[in] end     End of the memory area - exclusive.
 *
 * @param \c TRUE if the memory range is valid, \c FALSE otherwise.
 */
boolean 
sec_img_auth_is_valid_area_32
(
  sec_img_auth_whitelist_area_param_t *whitelist,
  uint32 start, uint32 end
);


/**
 * Checks if the complete image is in valid area
 *
 * @param[in] start   Start of the memory area - inclusive.
 * @param[in] end     End of the memory area - exclusive.
 *
 * @param \c TRUE if the memory range is valid, \c FALSE otherwise.
 */
boolean 
sec_img_auth_is_valid_area_64
(
  sec_img_auth_whitelist_area_param_t *whitelist,
  uint64 start, uint64 end
);

/**
 * Checks if the sec img library should skip RSA operation. It returns TRUE for the
 * authentication of XBL image itself, whose meta data has been auth'ed by PBL and
 * is locked in read-only memory.
 *
 * @param[in] sec_img_id   image id to check
 *
 * @param \c TRUE if image id matches that of XBL image's, \c FALSE otherwise.
 */
static inline boolean sec_img_auth_env_allow_skip_RSA
(
  uint32 sec_img_id
)
{
  /* IMPORTANT Note: that RSA Skipping is dangerous so don't add other images without 
     approval.*/
  if (sec_img_id != SECBOOT_SBL_SW_TYPE)
  {
    return FALSE;
  }
  return TRUE;
}

#endif /* SEC_PIL_AUTHI_H */
