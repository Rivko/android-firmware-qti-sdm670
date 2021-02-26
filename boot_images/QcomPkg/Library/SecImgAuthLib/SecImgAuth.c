/**
* @file sec_img_auth.c
* @brief Secure PIL Auth Security implementation
*
* This file implements the Secure Authentication Routines,
* including signature validation and hash validation for the
* hash table segment, elf header and program header table
* segment
*
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
19/08/14   mm       Adapted for 64 bit architecture 
26/10/13   mm       Adapted for PBL 
26/03/13   dm       Added overflow checks & error checks
02/20/11   vg       Ported from TZ PIL.
===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "comdef.h"
#include "SecImgEnc.h"
#include "SecImgAuthi.h"
#include "secboot.h"
#include "secboot_hw.h"
#include "SecImgAuthEnv.h"
#include "SecImgUtil.h"
#include "UIE.h"

static secboot_verified_info_type s_pbl_verified_info;

/**
 * @brief get pbl debug information from sec img auth library
 *
 * @param[in, out] debug_reenable - pbl debug reenable flag
 *
 * @return 0 on success, negative on failure
 */
static sec_img_auth_error_type sec_img_auth_get_pbl_debug_info
(
  uint32 *debug_reenable
)
{
  (*debug_reenable) = s_pbl_verified_info.enable_debug;
  return SEC_IMG_AUTH_SUCCESS;
}

/**
 * @brief Authenticate the ELF and Program Headers by checking
 *        that the hash of the headers matches the hash entry
 *        in the hash table segment.
 *
 * @return 0 on success, else failure
 *
 * @param[in] img_handle Handle to the image being authenticated.
 * @param[in] elf Pointer to the ELF header.
 * @param[in] crypto_ftbl Pointer to the crypto function table.
 */
static sec_img_auth_error_type
sec_img_auth_elf_headers (
  const sec_img_auth_priv_t *img_handle,
  const sec_img_auth_elf_info_t * elf,
  secboot_crypto_hash_ftbl_type * crypto_ftbl)
{
  uint8 *hash = NULL;
  uint8 sha_digest[CEML_HASH_DIGEST_SIZE_SHA256];
  memset(sha_digest, 0x0, CEML_HASH_DIGEST_SIZE_SHA256);
  secboot_error_type   sec_err = E_SECBOOT_FAILURE;  
  sec_img_auth_error_type  ret = SEC_IMG_AUTH_FAILURE;
  do
  {

    if ((elf == NULL) || (elf->prog_hdr_num == 0) || (elf->hash_seg == NULL) || 
         (crypto_ftbl == NULL) || (elf->prog_hdr_num == 0))
    {
      ret = SEC_IMG_AUTH_INVALID_ARG;
      break;
    }

    hash = elf->hash_seg;

    if (hash == NULL)
    {
      ret = SEC_IMG_AUTH_INVALID_ARG;
      break;
    }  

    if (img_handle->elf.elf_format == ELF_CLASS_32)
    {
      if ((elf->elf_prg_hdr.hdr_32.elf_hdr == NULL))
      {
        ret = SEC_IMG_AUTH_INVALID_ARG;
        break;
      }
      sec_err = secboot_hash (crypto_ftbl, CEML_HASH_ALGO_SHA256,
                (uint8 *) elf->elf_prg_hdr.hdr_32.elf_hdr, sizeof (Elf32_Ehdr),
                (uint8 *) elf->elf_prg_hdr.hdr_32.prog_hdr,
                ((sizeof (Elf32_Phdr) * elf->prog_hdr_num)),
                (uint8 *) (sha_digest), sizeof (sha_digest));

    }
    else
    {
      if ((elf->elf_prg_hdr.hdr_64.elf_hdr == NULL))
      {
        ret = SEC_IMG_AUTH_INVALID_ARG;
        break;
      }
      sec_err = secboot_hash (crypto_ftbl, CEML_HASH_ALGO_SHA256,
                (uint8 *) elf->elf_prg_hdr.hdr_64.elf_hdr, sizeof (Elf64_Ehdr),
                (uint8 *) elf->elf_prg_hdr.hdr_64.prog_hdr,
                ((sizeof (Elf64_Phdr) * elf->prog_hdr_num)),
                (uint8 *) (sha_digest), sizeof (sha_digest));
    }
    if (sec_err != E_SECBOOT_SUCCESS)
    {
      ret = (sec_img_auth_error_type)sec_err;
      break;
    }

    /* Check the hash is correct */
    if(MEMCMP(hash, sha_digest, CEML_HASH_DIGEST_SIZE_SHA256) != 0)
    {
      ret =  SEC_IMG_ELF_HEADERS_INVALID_DIGEST;
      break;
    }
    else
    {
      ret = SEC_IMG_AUTH_SUCCESS;
      break;
    }
  }
  while (0);
  return ret;
}


/**
 * @brief Perform a hash and verify for a segment.
 *
 * @param[in] img_handle Handle to the image being authenticated.
 * @param[in] hash Pointer to a pre-computed SHA hash
 * @param[in] hash_len The lenght in bytes of the SHA hash
 * @param[in] seg The segment to computer the hash on
 * @param[in] seg_len The length on the segment
 * @param[in] seg_num segment number used for loading 
 * @param[in] crypto_ftbl Pointer to the crypto function table.
 *
 * @return 0 on success, else failure
 */
static sec_img_auth_error_type
sec_img_auth_segment (
  const sec_img_auth_priv_t *img_handle,
  const uint8 * hash, uint32 hash_len,
  const uint8 * seg, uint32 seg_len, uint32 seg_num,
  secboot_crypto_hash_ftbl_type * crypto_ftbl)
{
  sec_img_auth_error_type ret = SEC_IMG_AUTH_FAILURE;
  uint8 sha_digest[CEML_HASH_DIGEST_SIZE_SHA256];
  memset(sha_digest, 0x0, CEML_HASH_DIGEST_SIZE_SHA256);
  secboot_error_type   sec_err = E_SECBOOT_FAILURE;  
  
  do
  {
    if ((hash == NULL) || (seg == NULL) || (seg_len == 0) ||
         (crypto_ftbl == NULL))
    {
      ret = SEC_IMG_AUTH_INVALID_ARG;
      break;
    }

 
    if (hash_len != CEML_HASH_DIGEST_SIZE_SHA256)
    {
      ret = SEC_IMG_AUTH_INVALID_DIGEST_LEN;
      break;
    }

    if (uie_is_segment_encrypted(img_handle->uie, seg_num))
    {
      ret = uie_decrypt_segment(img_handle->uie, seg_num,
                                (void *)seg, seg_len,
                                sha_digest);
      if (ret != SEC_IMG_AUTH_SUCCESS)
      {
        ret = SEC_IMG_AUTH_FAILURE; break;
      }
    }
    else
    {
      sec_err = secboot_hash(crypto_ftbl, CEML_HASH_ALGO_SHA256, 
                    (uint8*)seg, seg_len, NULL, 0,
                    (uint8*)(sha_digest), sizeof(sha_digest));
    if (sec_err != E_SECBOOT_SUCCESS)
    {
      ret = (sec_img_auth_error_type)sec_err;
      break;
    }
  }

    /** Check the hash is correct */
    if(MEMCMP(hash, sha_digest, CEML_HASH_DIGEST_SIZE_SHA256) != 0)
    {
      ret = SEC_IMG_AUTH_SEGMENT_INVALID_DIGEST;
      break;
    }
    else
    {
      ret = SEC_IMG_AUTH_SUCCESS;
      break;
    }

  }while (0);
  return ret;
}

/*----------------------------------------------------------------------------
 * Function Defintions 
 * -------------------------------------------------------------------------*/
 
/**
 * @brief Verify the hash on all loadable program segments
 *
 * @param[in] elf   ELF header of the loaded image
 *
 * @return 0 on success, else failure
 */
sec_img_auth_error_type
sec_img_auth_segments (
  const sec_img_auth_priv_t *img_handle,
  uint32 sec_img_id,
  const sec_img_auth_elf_info_t * elf,
  sec_img_auth_verified_info_s *v_info)
{
  uint32 i          = 0;
  uint8 *hash       = NULL;
  sec_img_auth_error_type  ret        = SEC_IMG_AUTH_FAILURE;
  secboot_crypto_hash_ftbl_type crypto_ftbl;
  do
  {
    if((elf == NULL)||(elf->hash_seg == NULL)||(elf->hash_array_sz == 0)||
        (elf->prog_hdr_num == 0))
    {
      ret = SEC_IMG_AUTH_INVALID_ARG;
      break;
    }
    
    crypto_ftbl.CeMLDeInit = CeMLDeInit;
    crypto_ftbl.CeMLHashAtomic = CeMLHashAtomic;
    crypto_ftbl.CeMLHashDeInit = CeMLHashDeInit;
    crypto_ftbl.CeMLHashFinal = CeMLHashFinal;
    crypto_ftbl.CeMLHashInit = CeMLHashInit;
    crypto_ftbl.CeMLHashUpdate = CeMLHashUpdate;
    crypto_ftbl.CeMLInit = CeMLInit;
  
    hash = elf->hash_seg;
   
    /* Skip first hash.
     * This was already checked in signature validation */  
    hash += CEML_HASH_DIGEST_SIZE_SHA256;

    for ( i = 1; i < elf->prog_hdr_num; i++)
    {
      do
      {
        if (img_handle->elf.elf_format == ELF_CLASS_32)
        {
          if (elf->elf_prg_hdr.hdr_32.prog_hdr[i].p_paddr + elf->elf_prg_hdr.hdr_32.prog_hdr[i].p_memsz < elf->elf_prg_hdr.hdr_32.prog_hdr[i].p_paddr)
          {
            return SEC_IMG_AUTH_INTEGER_OVERFLOW;
          }

          if (sec_img_auth_is_valid_segment(img_handle->elf.elf_format, 
              &elf->elf_prg_hdr.hdr_32.prog_hdr[i]) &&
              (elf->elf_prg_hdr.hdr_32.prog_hdr[i].p_filesz != 0) && 
	            (sec_img_auth_is_valid_area_32(img_handle->elf.whitelist_area, 
	                                           elf->elf_prg_hdr.hdr_32.prog_hdr[i].p_paddr,
	                                           elf->elf_prg_hdr.hdr_32.prog_hdr[i].p_paddr + elf->elf_prg_hdr.hdr_32.prog_hdr[i].p_memsz)))
          {
            ret = sec_img_auth_segment(img_handle, hash, CEML_HASH_DIGEST_SIZE_SHA256,
                  (uint8*)(uintptr_t)elf->elf_prg_hdr.hdr_32.prog_hdr[i].p_paddr,
                  elf->elf_prg_hdr.hdr_32.prog_hdr[i].p_filesz, i, &crypto_ftbl);

            /* Fill in the failing segment information */               
            v_info->ph_segment = i;
            BREAKIF (ret != SEC_IMG_AUTH_SUCCESS);
          }
          else
          {
            /* This segment is not to be checked. So success.*/
            ret = SEC_IMG_AUTH_SUCCESS;
          }
        }
        else
        {
          if (elf->elf_prg_hdr.hdr_64.prog_hdr[i].p_paddr + elf->elf_prg_hdr.hdr_64.prog_hdr[i].p_memsz < elf->elf_prg_hdr.hdr_64.prog_hdr[i].p_paddr)
          {
            return SEC_IMG_AUTH_INTEGER_OVERFLOW;
          }

          if (sec_img_auth_is_valid_segment(img_handle->elf.elf_format, 
              &elf->elf_prg_hdr.hdr_64.prog_hdr[i]) &&
              (elf->elf_prg_hdr.hdr_64.prog_hdr[i].p_filesz != 0) &&
	            (sec_img_auth_is_valid_area_64(img_handle->elf.whitelist_area, 
	                                           elf->elf_prg_hdr.hdr_64.prog_hdr[i].p_paddr,
	                                           elf->elf_prg_hdr.hdr_64.prog_hdr[i].p_paddr + elf->elf_prg_hdr.hdr_64.prog_hdr[i].p_memsz)))
          {
             ret = sec_img_auth_segment(img_handle, hash, 
                   CEML_HASH_DIGEST_SIZE_SHA256,
                   (uint8*)(uintnt)elf->elf_prg_hdr.hdr_64.prog_hdr[i].p_paddr,
                   elf->elf_prg_hdr.hdr_64.prog_hdr[i].p_filesz, 
                   i, &crypto_ftbl);

            /* Fill in the failing segment information */               
            v_info->ph_segment = i;
            BREAKIF (ret != SEC_IMG_AUTH_SUCCESS);
          }
          else
          {
            /* This segment is not to be checked. So success.*/
            ret = SEC_IMG_AUTH_SUCCESS;
          }
        }
       hash += CEML_HASH_DIGEST_SIZE_SHA256;  
      } while (0);
    
      BREAKIF (ret != SEC_IMG_AUTH_SUCCESS);
    }

    BREAKIF (ret != SEC_IMG_AUTH_SUCCESS);
  }while (0);
  
  return ret;
}

/**
 * @brief This function checks if the image associated with the code segment
 *        needs to be authenticated. If authentication is required, callers
 *        MUST authenticate the image successfully before allowing it to execute.
 *
 * @param[in]     code_segment Code segment in SECURE_BOOTn register containing 
 *                             authentication information of the image.
 * @param[in,out] auth_enabled Pointer to a uint32 indicating whether authentication 
 *                             is required.Will be populated to 0 if authentication 
 *                             is not required,1 if authentication is required.
 *                 
 * @return 0 on success, else failure
 */

sec_img_auth_error_type
sec_img_auth_hw_is_auth_enabled (uint32 sec_img_id,
  sec_img_auth_secboot_info_t * auth_info)
{
  secboot_hw_etype secboot_err = E_SECBOOT_HW_FAILURE;
  sec_img_auth_error_type ret = SEC_IMG_AUTH_FAILURE;

  do
  {
    if(auth_info == NULL)
    {
      ret = SEC_IMG_AUTH_INVALID_ARG;
      break;
    }
  
    ret = sec_img_auth_get_code_segment(sec_img_id, &auth_info->code_seg);
    BREAKIF(ret != SEC_IMG_AUTH_SUCCESS);
        
    secboot_err = secboot_hw_is_auth_enabled(auth_info->code_seg, 
                  &auth_info->auth_enabled);
  
    if(secboot_err != E_SECBOOT_HW_SUCCESS)
    {
      ret = (sec_img_auth_error_type)secboot_err;
      break;
    }
    /* If control reaches here, its a success scenario */
    ret = SEC_IMG_AUTH_SUCCESS;
  }while(0);
    
  return ret;
}

/**
 * @brief check if the image is signed or not
 *
 * @param[in] elf ELF header of the loaded image
 *
 * @return 0 if the image is not signed, return 1 if the image is signed
 */
int32 sec_pil_is_signed_image(const sec_img_auth_elf_info_t *elf)
{
  if (elf != NULL)
  {
    if ( (elf->sig_sz != 0) && (elf->cert_ptr != NULL) && (elf->cert_sz != 0))
    {
      return 1;
    }
  }
  return 0;
}

/**
 * @brief This function verifies hash table segment signature and validates
 *        ELF and program headers hash against thier entry in hash table.          
 *
 * @param[in] sec_img_id           The peripheral image.
 *
 * @param[in] img_auth_info        The authentication info for image.
 *                                 A precondition that img_auth_info->elf is not NULL
 *
 * @param[out] verified_info       The verification info after authentication is done
 *
 * @return 0 on success, else failure
 */

sec_img_auth_error_type
sec_img_auth_verify_sig (uint32 sec_img_id,
  const sec_img_auth_priv_t * img_auth_info,
  sec_img_auth_verified_info_s * verified_info)
{

  secboot_error_type   sec_err = E_SECBOOT_FAILURE;  
  secboot_hw_etype   sec_hw_err = E_SECBOOT_HW_FAILURE;  
  secboot_image_info_type image_info;
  uint32 sw_version = 0;
  secboot_crypto_hash_ftbl_type crypto_ftbl;
  secboot_ftbl_type secboot_ftbl;
  secboot_hw_ftbl_type secboot_hw_ftbl;
  sec_img_auth_error_type ret = SEC_IMG_AUTH_FAILURE;
  const sec_img_auth_elf_info_t *elf = NULL;
  sec_img_auth_secboot_info_t auth_info;
  secboot_auth_image_context  context;
   
  do
  {
    if((verified_info == NULL)||(img_auth_info == NULL))
    {
      ret = SEC_IMG_AUTH_INVALID_ARG;
      break;
    }

    /* skip the entire secboot for RSA opr, only for XBL authentication, as XBL's */
    /* meta data is already auth'ed by PBL and is locked as read-only memory.     */
    if (sec_img_auth_env_allow_skip_RSA(sec_img_id))
    {
      ret = SEC_IMG_AUTH_SUCCESS;
      break;
    }
    
    elf = &img_auth_info->elf;
 
    crypto_ftbl.CeMLDeInit = CeMLDeInit;
    crypto_ftbl.CeMLHashAtomic = CeMLHashAtomic;
    crypto_ftbl.CeMLHashDeInit = CeMLHashDeInit;
    crypto_ftbl.CeMLHashFinal = CeMLHashFinal;
    crypto_ftbl.CeMLHashInit = CeMLHashInit;
    crypto_ftbl.CeMLHashUpdate = CeMLHashUpdate;
    crypto_ftbl.CeMLInit = CeMLInit;
 
    ret = sec_img_auth_hw_is_auth_enabled(sec_img_id,&auth_info);
    BREAKIF(SEC_IMG_AUTH_SUCCESS != ret);

    // enforce the signed image in non-secure device. unsigned image is not accepted
    if (sec_pil_is_signed_image(elf) == 0)
    {
      ret = SEC_IMG_AUTH_FAILURE;
      break;
    }

    sec_err = secboot_get_ftbl(&secboot_ftbl);
    if (sec_err != E_SECBOOT_SUCCESS)
    {
      /* The Secure Boot Library errors are propogated as is. */
      ret = (sec_img_auth_error_type)sec_err;
      break;
    }

    sec_hw_err = secboot_hw_get_ftbl(&secboot_hw_ftbl);
    if (sec_hw_err != E_SECBOOT_HW_SUCCESS)
    {
      /* The Secure Boot Library errors are propogated as is. */
      ret = (sec_img_auth_error_type)sec_err;
      break;
    }

    if(auth_info.auth_enabled == TRUE)
    {
      ret = sec_img_auth_get_sw_version(sec_img_id, &sw_version);
    }
    BREAKIF(ret != SEC_IMG_AUTH_SUCCESS);

    image_info.header_ptr_1 = elf->hash_seg_hdr;
    image_info.header_len_1 = elf->hash_seg_hdr_sz;
    image_info.code_ptr_1= elf->hash_seg;
    image_info.code_len_1 = elf->hash_array_sz;
    image_info.signature_ptr = elf->sig_ptr;
    image_info.signature_len = elf->sig_sz;
    image_info.sw_type = sec_img_id;
    image_info.sw_version = sw_version;
    image_info.x509_chain_ptr = elf->cert_ptr;
    image_info.x509_chain_len = elf->cert_sz; 

    /* call into secboot authenticate image function */
    MEMSET(&context, 0, sizeof(context));

    ret = sec_img_auth_get_pbl_debug_info(&(context.pbl_reenable_debug));
    BREAKIF(ret != SEC_IMG_AUTH_SUCCESS);

    context.code_segment = auth_info.code_seg;
    context.image_sw_id = sec_img_id;
    context.secboot_ftbl_ptr = &secboot_ftbl;
    context.secboot_hw_ftbl_ptr = &secboot_hw_ftbl;
    context.crypto_hash_ftbl_ptr = &crypto_ftbl;

    ret = sec_img_auth_authenticate_image_context(&context,
                &image_info,
                &(verified_info->v_info));
    /* Check the result of authentication */ 
    BREAKIF(ret != SEC_IMG_AUTH_SUCCESS);

    ret = sec_img_auth_elf_headers(img_auth_info, elf, &crypto_ftbl);
    BREAKIF(ret != SEC_IMG_AUTH_SUCCESS);
  
  }
  while(0);

  return ret;  
}

sec_img_auth_error_type sec_img_auth_init
(
  secboot_verified_info_type *pbl_verified_info
)
{
  if (NULL == pbl_verified_info)
  {
    return SEC_IMG_AUTH_INVALID_ARG;
  }
  MEMCPY(&s_pbl_verified_info, pbl_verified_info, sizeof(s_pbl_verified_info));
  return SEC_IMG_AUTH_SUCCESS;
}
