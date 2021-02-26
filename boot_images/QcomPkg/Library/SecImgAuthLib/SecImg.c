/**
* @file sec_img_auth.c
* @brief Secure Image Authentication
*
* This file implements Secure Image Authentication. Secure Image authentication module authnticates
* the ELF image.
*/

/*===========================================================================
   Copyright (c) 2012-2013 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE



when       who      what, where, why
--------   ---      ------------------------------------
08/19/14   mm       Adapted for 64bit OBJ ELF CLASS 
26/10/13   mm       Adapted for Boot ROM
26/03/13   dm       Added overflow checks & error checks
02/26/12   vg       Moved inclusion of sec_img_auth_env.h to sec_img_auth.h
02/20/12   vg       Ported from TZ PIL.
===========================================================================*/


/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <comdef.h>
#include <string.h>
#include "SecImgAuthEnv.h"
#include "SecImgAuthi.h"
#include "SecDbg.h"

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/
sec_img_auth_priv_t s_img_handle;
sec_img_auth_seg_auth_info_t s_img_auth_seg_info;

/*---------------------------------------------------------------------------
  Macro to check for alignment
---------------------------------------------------------------------------*/
#define IS_ALIGNED(v)         (((uint32)(v) & 0x03) == 0)
#define IS_NOT_ALIGNED(v)     (!(IS_ALIGNED(v)))

#define SBL_KEY_LEN           16
static uint8 l1_key[SBL_KEY_LEN] = {0};
static uint8 l1_key_cached = FALSE;  
/*----------------------------------------------------------------------------
 * Static Function Definitions
 * -------------------------------------------------------------------------*/

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
sec_img_auth_get_elf_format 
(
 const  void * elf_hdr,
 uint32 *elf_format
)
{
  sec_img_auth_error_type ret = SEC_IMG_AUTH_FAILURE;  
  do
  { 
    if ((elf_hdr == NULL) || (elf_format == NULL))
    {
      ret = SEC_IMG_AUTH_INVALID_ARG;
      break;
    }
    /* We can cast the incoming header to Elf32 or ELF 64 cause the fields 
     * are the same between ELF32 and ELF 64 in the structure before the class
     */
    if (((Elf32_Ehdr *)elf_hdr)->e_ident[ELFINFO_CLASS_INDEX] == ELF_CLASS_32)
    {
      *elf_format = ELF_CLASS_32;
      ret = SEC_IMG_AUTH_SUCCESS;
      break;
    }
    else
    {
      if (((Elf64_Ehdr *)elf_hdr)->e_ident[ELFINFO_CLASS_INDEX] == ELF_CLASS_64)
      {
        *elf_format = ELF_CLASS_64;
        ret = SEC_IMG_AUTH_SUCCESS;
        break;
      }
      else
      {
        ret = SEC_IMG_AUTH_ELF_INVALID_CLASS;
        break;
      }
    }
  }while(0);

  return ret;
}

/**
 * @brief Verify if the boot header has the correct lengths
 *
 * @param[in] s_img_handle Pointer to the image info being authenticated 
 * @param[in] mi Pointer to the header of the hash segment
 *
 * @return \c TRUE if the boot header lengths are correct, otherwise \c FALSE.
 */
sec_img_auth_error_type
sec_img_auth_validate_mi_boot 
(
 const sec_img_auth_priv_t *s_img_handle,
 const mi_boot_image_header_type * mi
)
{
  uint32 total_len = 0;
  sec_img_auth_error_type ret = SEC_IMG_AUTH_FAILURE;  
  uint32 pad_len = 0;

  do
  {
    if ((mi == NULL) || (s_img_handle == NULL))
    {
      ret = SEC_IMG_AUTH_INVALID_ARG;
      break;
    }
    /* Total Len of Cert Chain Size and Signature size should not 
      * exceed the MAX Size Allowed.
      */
      if (mi->cert_chain_size > SECBOOT_MAX_CERT_CHAIN_SIZE)
      {
        ret = SEC_IMG_AUTH_MI_BOOT_INVALID_CERT_CHAIN_SIZE;
        break;
      }
    
      /* Verify the image_size is equal to total of code_size, cert_chain_size and
       * signature_size. We may have added 1 to 3 bytes to the image if the total
       * image size is not multiple of four. We should account for that also.
       */
      total_len = mi->code_size;

      total_len += mi->cert_chain_size;
      /* Integer overflow check */
      if(total_len < mi->cert_chain_size)
      {
        ret = SEC_IMG_AUTH_MI_BOOT_INVALID_SIZE;
        break;
      }

      total_len += mi->signature_size;
      /* Integer overflow check */
      if(total_len < mi->signature_size)
      {
        ret = SEC_IMG_AUTH_MI_BOOT_INVALID_SIZE;
        break;
      }
   
      /* Multiple of 4 bytes */
      pad_len = (total_len % sizeof(uint32)) == 0 ? 0 : sizeof(uint32) - (total_len % sizeof(uint32));
      total_len += pad_len;
 
      /* Now test for overflow like other above */
      if(total_len < pad_len)
      {
        ret = SEC_IMG_AUTH_MI_BOOT_INVALID_SIZE;
        break;
      }  
   
      /* Total Len should be equal to the image size */
      if (total_len != mi->image_size)
      {
        ret = SEC_IMG_AUTH_MI_BOOT_INVALID_SIZE;
        break;
      }

     /* If you reach here, you have a success scenario */
      ret = SEC_IMG_AUTH_SUCCESS;
   }
   while (0);
  return ret;
}

/*----------------------------------------------------------------------------
 * Function Definitions 
 * -------------------------------------------------------------------------*/

/**
 * @brief This funnction  frees the memory if any and zeros out data
 *        present in the data structures for img_auth. 
 *
 * @param[in]  img_auth       Pointer to context data. 
 *
 * Post Condition:
 * The memory pointed is cleared. The state is reset to INIT state.
 *
 */
static void sec_img_auth_clean_priv(sec_img_auth_priv_t *img_auth)
{
  if (img_auth != NULL)
  {
    MEMSET(img_auth, 0, sizeof(sec_img_auth_priv_t));
    img_auth->state = SEC_IMG_AUTH_STATE_INIT;
  }
}


/*----------------------------------------------------------------------------
 * External Function Defintions 
 * -------------------------------------------------------------------------*/

/**
 * @brief Authenticates hash table segment against its signature.
 *
 * @param[in] elf_hdr     Pointer to the ELF header 
 *
 * @return 0 on success, negative on failure.
 */

sec_img_auth_error_type
sec_img_auth_verify_metadata 
(
  uint32 sec_img_id,
  const sec_img_auth_elf_info_type * img_data,
  sec_img_auth_whitelist_area_param_t *whitelist_area,
  sec_img_auth_verified_info_s * v_info
)
{
  sec_img_auth_error_type ret = SEC_IMG_AUTH_FAILURE;  

  do
  {
    /* Check for the parameters. Internal functions will not check for these 
     * parameters. 
     */
    if ((img_data == NULL) || (img_data->elf_hdr == NULL)||
      (img_data->prog_hdr == NULL)||(img_data->hash_seg_hdr == NULL)||
      (whitelist_area == NULL) ||(v_info == NULL))
    {
      ret = SEC_IMG_AUTH_INVALID_ARG;
      break;
    }
   
   /* Verified information on success/failure */ 
   MEMSET((uint8*)v_info, 0, sizeof(sec_img_auth_verified_info_s));
 
   /* Start with clean initialized structures */
    s_img_handle.uie = NULL;
    sec_img_auth_clean_priv(&s_img_handle);

    /*Validate if its a valid ELF */
    ret = sec_img_auth_validate_elf(img_data->elf_hdr);
    BREAKIF(ret != SEC_IMG_AUTH_SUCCESS);
  
  
    ret = sec_img_auth_get_elf_format(img_data->elf_hdr, &(s_img_handle.elf.elf_format));
    BREAKIF(ret != SEC_IMG_AUTH_SUCCESS);    


    /* Check if Authentication is enabled */
    ret = sec_img_auth_hw_is_auth_enabled(sec_img_id, 
                                          &s_img_handle.auth_info);
    BREAKIF (ret != SEC_IMG_AUTH_SUCCESS)
 
    /* Cache the L1 Key if not read from fuses - only done once */
    if (l1_key_cached == FALSE)
    {
      ret = uie_get_l1_key(l1_key, SBL_KEY_LEN);
      BREAKIF(ret != UIE_STATUS_SUCCESS);
      l1_key_cached = TRUE;
    }

   /* Populate the ELF information */
    if(s_img_handle.elf.elf_format == ELF_CLASS_32)
    {
      ret = sec_img_auth_populate_elf_info_32(sec_img_id, img_data, 
                                         &s_img_handle); 
      BREAKIF (ret != SEC_IMG_AUTH_SUCCESS);
      
      /* Verify the signature if available. Else verify the Headers */
      ret = sec_img_auth_verify_sig(sec_img_id, &s_img_handle, v_info);
      BREAKIF (ret != SEC_IMG_AUTH_SUCCESS);

      /* Check the entry of the ELF. */
      ret = sec_img_auth_validate_elf_entry_32(sec_img_id, whitelist_area, &s_img_handle);
      BREAKIF (ret != SEC_IMG_AUTH_SUCCESS);
  }
  else
  {
      ret = sec_img_auth_populate_elf_info_64(sec_img_id, img_data, 
                                         &s_img_handle); 
      BREAKIF (ret != SEC_IMG_AUTH_SUCCESS);

      /* Verify the signature if available. Else verify the Headers */
      ret = sec_img_auth_verify_sig(sec_img_id, &s_img_handle, v_info);
      BREAKIF (ret != SEC_IMG_AUTH_SUCCESS);

      /* Check the entry of the ELF. */
      ret = sec_img_auth_validate_elf_entry_64(sec_img_id, whitelist_area, &s_img_handle);
      BREAKIF (ret != SEC_IMG_AUTH_SUCCESS);
  }

    /* Set the whitelist*/
    s_img_handle.elf.whitelist_area = whitelist_area;

    /* Get encryption parm if the uie header is present.
     * The UIE Header is populated while parsing the ELF info
     */
    if (uie_is_image_encrypted(s_img_handle.elf.uie_hdr_ptr))
    {
      s_img_handle.uie = uie_init(s_img_handle.elf.uie_hdr_ptr, l1_key, SBL_KEY_LEN);
      if(s_img_handle.uie == NULL)
      {
        ret = SEC_IMG_AUTH_FAILURE;
        break;
      }
    }

    /* If control reaches here, we have a success scenario*/
    s_img_handle.state = SEC_IMG_AUTH_STATE_ELF_AND_PROG_HDRS_HASH_MATCH;
        
  }
  while (0);

  if( ret != SEC_IMG_AUTH_SUCCESS)
  {
    /* Cleanup the structures */
    sec_img_auth_clean_priv(&s_img_handle);
    uie_deinit(s_img_handle.uie);

    s_img_handle.uie = NULL;
  }

  /* Set the state in the verified information to caller */
  if (v_info != NULL)
  {
    v_info->state = s_img_handle.state;
  }

  return ret;
}

/**
 * @brief Validates ELF segments against hash table and reset peripheral 
 *
 * @param[in] reset Brings up the subsytem if the reset is TRUE.
 *
 * @return 0 on success, negative on failure
 */
sec_img_auth_error_type
sec_img_auth_hash_elf_segments 
(
 uint32 sec_img_id, 
 sec_img_auth_verified_info_s *v_info
)
{
  sec_img_auth_error_type ret = SEC_IMG_AUTH_FAILURE;  
  sec_dbg_error_type status = SEC_DBG_STATUS_FAILURE;
  uint64 dbg_addr = 0;
  do
  {
    /* v_info is not memset to preserve the earlier state information.*/
    if(v_info == NULL)
    {
      ret = SEC_IMG_AUTH_INVALID_ARG;
      break;
    }

    if (s_img_handle.state != SEC_IMG_AUTH_STATE_ELF_AND_PROG_HDRS_HASH_MATCH)
    {
      ret = SEC_IMG_AUTH_STATE_INVALID;
      break;
    }

    if(s_img_handle.elf.elf_format == ELF_CLASS_32)
    {
      /* Zero Initialize the segments */
      ret = sec_img_auth_zero_segments_32(&s_img_handle.elf, v_info);
      BREAKIF (ret != SEC_IMG_AUTH_SUCCESS);
    }
    else
    {
      /* Zero Initialize the segments */
      ret = sec_img_auth_zero_segments_64(&s_img_handle.elf, v_info);
      BREAKIF (ret != SEC_IMG_AUTH_SUCCESS);
    }
   
    ret = sec_img_auth_segments(&s_img_handle, sec_img_id, &s_img_handle.elf, v_info);
    BREAKIF (ret != SEC_IMG_AUTH_SUCCESS);

    s_img_handle.state = SEC_IMG_AUTH_STATE_COMPLETE_SEGMENTS_HASH_MATCH;

  }
  while (0);

  if (v_info != NULL)
  {
    /* Set the state in the verified information to caller */
    v_info->state = s_img_handle.state;
  }

  /* Secure Debug Policy Initialization on successful authentication */
  if ((SECBOOT_APDP_SW_TYPE == sec_img_id) &&(SEC_IMG_AUTH_SUCCESS == ret))
  { 
	/*get the debug policy address from elf file*/
    dbg_addr = (s_img_handle.elf.elf_prg_hdr.hdr_64.elf_hdr->e_entry) ;
    dbg_addr = (dbg_addr & 0x00000000ffffffff);
	/*debug policy init to validate the debug policy structure*/
    status = sec_dbg_init(dbg_addr);
	if (status == SEC_DBG_STATUS_SUCCESS)
	{
	    sec_dbg_apply_policy();
	}
	else
	{
		ret = SEC_IMG_AUTH_SECDBG_INIT_FAILURE;
	}
  }

  /* Always cleanup the structures after hashing
   *  Since noone needs this anymore
   */
  sec_img_auth_clean_priv(&s_img_handle);
  uie_deinit(s_img_handle.uie);

  s_img_handle.uie = NULL;

  
  return ret;
}

/**
 * @brief Check for loadable, non paged segments
 *
 * @param[in] Program hdr entry for the segment
 *
 * @result \c TRUE if the segment is valid, \c FALSE otherwise.
 */
boolean sec_img_auth_is_valid_segment(
  uint32 elf_format,
  const void* phdr)
{
  uint32 p_flags = 0;
  uint32 p_type= 0;

  /* ELF class has previosly been checked and can either 
   * be ELF_CLASS_64 or ELF_CLASS_32
   */
  p_flags = (elf_format == ELF_CLASS_64) ? ((Elf64_Phdr *)phdr)->p_flags : ((Elf32_Phdr *)phdr)->p_flags;
  p_type= (elf_format == ELF_CLASS_64) ? ((Elf64_Phdr *)phdr)->p_type: ((Elf32_Phdr *)phdr)->p_type;

  /* Valid hashed segments are loadable */
  if(p_type == MI_PBT_ELF_PT_LOAD)
  {
    if ((MI_PBT_PAGE_MODE_VALUE(p_flags) ==
         MI_PBT_NON_PAGED_SEGMENT) &&
        (MI_PBT_SEGMENT_TYPE_VALUE(p_flags) !=
          MI_PBT_HASH_SEGMENT) &&
        (MI_PBT_ACCESS_TYPE_VALUE(p_flags) !=
          MI_PBT_NOTUSED_SEGMENT) &&
        (MI_PBT_ACCESS_TYPE_VALUE(p_flags) !=
          MI_PBT_SHARED_SEGMENT))
    {
      return TRUE;
    }
  }

  return FALSE;
}

/**
 * @brief Elf header parser, to check for the correct format.
 *
 * @param[in] elf_hdr   Pointer to the ELF header.
 *
 * @result \c TRUE if the header is in correct format, \c FALSE otherwise.
 */

sec_img_auth_error_type
sec_img_auth_validate_elf 
(
 const  void * elf_hdr
)
{
  sec_img_auth_error_type ret = SEC_IMG_AUTH_FAILURE;  
  do
  {
 
    /* All fields in ELF64_EHdr and ELF32_Ehdr are the same before the class
     * So any of the structures can be used for checking the fields before the class
     */
    if (((Elf64_Ehdr *)elf_hdr)->e_ident[ELFINFO_MAG0_INDEX] != ELFINFO_MAG0
        || ((Elf64_Ehdr *)elf_hdr)->e_ident[ELFINFO_MAG1_INDEX] != ELFINFO_MAG1
        || ((Elf64_Ehdr *)elf_hdr)->e_ident[ELFINFO_MAG2_INDEX] != ELFINFO_MAG2
        || ((Elf64_Ehdr *)elf_hdr)->e_ident[ELFINFO_MAG3_INDEX] != ELFINFO_MAG3)
    {
      ret = SEC_IMG_AUTH_ELF_INCORRECT_MAGIC;
      break;
    }

    if ((((Elf64_Ehdr *)elf_hdr)->e_ident[ELFINFO_CLASS_INDEX] != ELF_CLASS_32) &&
        (((Elf64_Ehdr *)elf_hdr)->e_ident[ELFINFO_CLASS_INDEX] != ELF_CLASS_64))
    {
      ret = SEC_IMG_AUTH_ELF_INVALID_CLASS;
      break;
    }
    /* ELF Current Version check. Current version is 1 */
    if (((Elf64_Ehdr *)elf_hdr)->e_ident[ELFINFO_VERSION_INDEX] != ELF_VERSION_CURRENT)
    {
      ret = SEC_IMG_AUTH_INVALID_VERSION;
      break;
    }
  
     
    if (((Elf64_Ehdr *)elf_hdr)->e_ident[ELFINFO_CLASS_INDEX] == ELF_CLASS_32)
    {
      /*Ensures size mentioned in ELF Header matches structure definition */
      if (((Elf32_Ehdr*)elf_hdr)->e_ehsize != sizeof(Elf32_Ehdr))
      {
        ret = SEC_IMG_AUTH_ELF_INVALID_EHSIZE;
        break;
      }

      /*Ensures size mentioned in ELF Header matches structure definition */
      if ((((Elf32_Ehdr *)elf_hdr)->e_phentsize) != (sizeof(Elf32_Phdr)))
      {
        ret = SEC_IMG_AUTH_ELF_INVALID_PHENTSIZE;
        break;
      }
      /*Ensures number of headers does not exceed Maximum allowed. */
      if(((Elf32_Ehdr *)elf_hdr)->e_phnum > MI_PBT_MAX_SEGMENTS)
      {
        ret = SEC_IMG_AUTH_ELF_INVALID_NUM_SEGMENTS;
        break;
      }
    }
    else
    {
      /*Ensures size mentioned in ELF Header matches structure definition */
      if (((Elf64_Ehdr *)elf_hdr)->e_ehsize != sizeof(Elf64_Ehdr))
      {
        ret = SEC_IMG_AUTH_ELF_INVALID_EHSIZE;
        break;
      }

      /*Ensures size mentioned in ELF Header matches structure definition */
      if ((((Elf64_Ehdr *)elf_hdr)->e_phentsize) != (sizeof(Elf64_Phdr)))
      {
        ret = SEC_IMG_AUTH_ELF_INVALID_PHENTSIZE;
        break;
      }
      /*Ensures number of headers does not exceed Maximum allowed. */
      if(((Elf64_Ehdr *)elf_hdr)->e_phnum > MI_PBT_MAX_SEGMENTS)
      {
        ret = SEC_IMG_AUTH_ELF_INVALID_NUM_SEGMENTS;
        break;
      }
    }
    /* Encountered some error other than initial value */
    if(ret != SEC_IMG_AUTH_FAILURE)
      break;

    /* If you reach here, you have a success scenario */
    ret = SEC_IMG_AUTH_SUCCESS;
  }
  while (0);

  return ret;
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
sec_img_auth_error_type 
sec_img_auth_is_auth_enabled
(
  uint32 code_seg, 
  uint32* is_auth_enabled
)
{
  secboot_hw_etype secboot_err = E_SECBOOT_HW_FAILURE;
  sec_img_auth_error_type ret = SEC_IMG_AUTH_FAILURE;

  do
  {
    if(is_auth_enabled == NULL)
    {
      ret = SEC_IMG_AUTH_INVALID_ARG;
      break;
    }

    secboot_err = secboot_hw_is_auth_enabled(code_seg, 
                                             is_auth_enabled);
  
    if(secboot_err != E_SECBOOT_HW_SUCCESS)
    {
      ret = (sec_img_auth_error_type)E_SECBOOT_HW_FAIL;
      break;
    }
    /* If control reaches here, its a success scenario */
    ret = SEC_IMG_AUTH_SUCCESS;
  }while(0);
    
  return ret;
}

sec_img_auth_error_type sec_img_auth_deinit(void)
{
  if (NULL == l1_key)
  {
    return SEC_IMG_AUTH_INVALID_ARG;
  }

  /* wipe out the l1 key in de-init */
  MEMSET(l1_key, 0, SBL_KEY_LEN);

  return SEC_IMG_AUTH_SUCCESS;
}

