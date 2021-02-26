/**
* @file sec_img_auth.c
* @brief Secure Image Authentication
*
* This file implements Secure Image Authentication. Secure Image authentication module authnticates
* the ELF image.
*/

/*===========================================================================
   Copyright (c) 2012-2015 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE



when       who      what, where, why
--------   ---      ------------------------------------
05/04/15   hw       Skip RSA for saving xbl boot time
12/02/14   hw       Load ZI segment with filesz 0
08/22/14   mm       Support for 64 bit 
26/10/13   mm       Adapted for Boot ROM
26/03/13   dm       Added overflow checks & error checks
02/26/12   vg       Moved inclusion of sec_img_auth_env.h to sec_img_auth.h
02/20/12   vg       Ported from TZ PIL.
===========================================================================*/


/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "SecImgAuthEnv.h"
#include "SecImgAuthi.h"

/*---------------------------------------------------------------------------
  Macro to check for alignment
---------------------------------------------------------------------------*/
#define IS_ALIGNED(v)         (((uint32)(v) & 0x03) == 0)
#define IS_NOT_ALIGNED(v)     (!(IS_ALIGNED(v)))

/*----------------------------------------------------------------------------
 * Static Function Definitions
 * -------------------------------------------------------------------------*/

/**
 * @brief Find the segment of the given p_flags
 *
 * @param[in] phdr The program header entry start ptr
 * @param[in] end The program header entry end ptr
 * @param[in] seg_flags The p_flags of a given segment
 *
 * @return Valid program header pointer if found, \c NULL otherwise.
 */
Elf32_Phdr *
sec_img_auth_elf_find_seg_32 
(
  const Elf32_Phdr * phdr, 
  const Elf32_Phdr * end,
  uint32 seg_flags
)
{
  Elf32_Phdr *cur= (Elf32_Phdr *) phdr;

  if ((phdr == NULL) || (end == NULL) || (end < phdr))
    return NULL;

  while ((uint32*)cur < (uint32*)end)
  {
    if (MI_PBT_SEGMENT_TYPE_VALUE(cur->p_flags) == seg_flags)
    {
      return cur;
    }
    cur++;
  }
  return NULL;
}

/*
 * @brief Find the segment of the given p_flags. 
 * Checks that only one program header entry exists with the given flags
 *
 * @param[in] phdr The program header entry start ptr
 * @param[in] end The program header entry end ptr
 * @param[in] seg_flags The p_flags of a given segment
 *
 * @return Valid program header pointer if found, \c NULL otherwise.
 */
sec_img_auth_error_type sec_img_auth_validate_hash_segment_entries_32
(
  const Elf32_Phdr * phdr, 
  const Elf32_Phdr * end,
  uint32 seg_flags
)
{
  uint32 count = 0;
  Elf32_Phdr *cur= (Elf32_Phdr *) phdr;
  sec_img_auth_error_type ret = SEC_IMG_AUTH_FAILURE;  

  if ((phdr == NULL) || (end == NULL) || (end < phdr))
    return ret;

  while (cur < end)
  {
    if (MI_PBT_SEGMENT_TYPE_VALUE(cur->p_flags) == seg_flags)
    {
      count++;
    }
    cur++;
  }
  if( count == 1)
  {
    ret = SEC_IMG_AUTH_SUCCESS;
  }
  else
  {
    ret = SEC_IMG_AUTH_INCORRECT_HASH_TABLE_SEGMENTS_COUNT;
  }
  return ret;
}






/**
   Checks if there hash segment size matches the expected size.

   @param[in] Elf header info struct
*/
sec_img_auth_error_type sec_img_auth_validate_hash_segment_len_32
(
  sec_img_auth_elf_info_t * img_data
)
{
  uint32      prog_tbl_sz = 0;
  uint32      exp_hash_seg_sz = 0;
  Elf32_Phdr* hash_prog_hdr = NULL;

  prog_tbl_sz = (uint32)(sizeof(Elf32_Phdr)) * 
                img_data->elf_prg_hdr.hdr_32.elf_hdr->e_phnum;

 /* Interger overflow check */
  if(((uint32)img_data->elf_prg_hdr.hdr_32.prog_hdr + prog_tbl_sz) < 
     (uint32)img_data->elf_prg_hdr.hdr_32.prog_hdr)
  {
     return SEC_IMG_AUTH_HASH_SEG_INVALID_SIZE;
  }

  hash_prog_hdr = sec_img_auth_elf_find_seg_32(img_data->elf_prg_hdr.hdr_32.prog_hdr,
                                     (Elf32_Phdr*)((uint8*) img_data->elf_prg_hdr.hdr_32.prog_hdr +
                                     prog_tbl_sz),
                                     MI_PBT_HASH_SEGMENT);

  /* The hash program header should not be NULL. Must have a HASH Segment. */
  if( NULL == hash_prog_hdr )
  {
    return SEC_IMG_AUTH_HASH_SEG_INVALID_SIZE;
  }

  /* Hash Segment Hdr size and Hash Segent Size are already checked for less
   * than 2K. The check for Signature Size and Cert Size are done in Validating MIBIB.
   */

  /* The expected hash segment size */
  exp_hash_seg_sz = (img_data->hash_seg_hdr_sz + img_data->hash_array_sz +
                     img_data->sig_sz + img_data->cert_sz);

  /* Encryption parm is present if the expected size of the hash segment is smaller */
  if (uie_is_image_encrypted(img_data->uie_hdr_ptr))
  {
    exp_hash_seg_sz += UIE_ENCRYPTION_HEADER_LEN;
  }

  /* The expected size of the hash segment should match the size of the Hash Table Segment */
  if (hash_prog_hdr->p_filesz != exp_hash_seg_sz)
  {
    /* The calculated size of the hash segment should never be larger than its filesz */
    return SEC_IMG_AUTH_HASH_SEG_INVALID_SIZE;
  }

  return SEC_IMG_AUTH_SUCCESS;
}




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
)
{
  uint32 i = 0;

  if ((NULL == whitelist) || (NULL == whitelist->area))
  {
    return FALSE;
  }
  
  for(i=0; i < whitelist->count; i++)
  {
    /* Variables end and whitelist->area[i].end are both exclusive. */
    if ((whitelist->area[i].start <= start ) && (end <= whitelist->area[i].end))
    {
      return TRUE;
    }
  }
  return FALSE;
}



/**
* @brief This function verifies that a given address lies within
*        a defined address range.       
 *
* @param[in] address    address to be checked
* @param[in] start_addr start address of defined address range
* @param[in] bytes      size of address range in bytes
*
* @return TRUE on success, FALSE on failure
*/
sec_img_auth_error_type 
sec_img_auth_check_address_32
(
  uint32 address, 
  uint32 start_addr, 
  uint32 bytes
)
{
  sec_img_auth_error_type ret = SEC_IMG_AUTH_FAILURE;

  if (((start_addr < (UINT32_MAX_VALUE - bytes)) &&
       (address >= start_addr)             &&
       (address < (start_addr + bytes))))
  {
    ret = SEC_IMG_AUTH_SUCCESS; /* addr is within valid range */
  }

  return ret;
}

/**
* @brief This function verifies that a the ELF entry lies within
*        the whitelist.       
*
* @param[in] sec_img_id     image id to check
* @param[in] whitelist_area whitelist to compare against 
* @param[in] img_data image data to validate 
*
* @return TRUE on success, FALSE on failure
*/
sec_img_auth_error_type 
sec_img_auth_validate_elf_entry_32
(
  uint32 sec_img_id,
  sec_img_auth_whitelist_area_param_t *whitelist_area,
  const sec_img_auth_priv_t * img_data
)
{
  int i = 0;
  uint32 ph_count = 0;
  Elf32_Phdr *ph_hdr = NULL;
  sec_img_auth_error_type ret = SEC_IMG_AUTH_FAILURE;

  /* entry point check is also skipped if RSA check is not required, where xbl is  */
  /* loading ramdump segment. ramdump is part of xbl image and has differet entry  */
  /* point from that in xbl elf header. */
  if (sec_img_auth_env_allow_skip_RSA(sec_img_id))
  {
    return SEC_IMG_AUTH_SUCCESS;
  }

  /* Parameters are already checked in above function */
  /* Check that ELF header entry field is an address that is located within a loadable */ 
  /* segement. This ensures that the entry address is jumping to signed code. */
  ph_count = img_data->elf.elf_prg_hdr.hdr_32.elf_hdr->e_phnum;
  ph_hdr = img_data->elf.elf_prg_hdr.hdr_32.prog_hdr;

  for(i = 0; i < ph_count; i++, ph_hdr++)
  {
   if (ph_hdr->p_vaddr + ph_hdr->p_memsz < ph_hdr->p_vaddr)
   {
      return SEC_IMG_AUTH_INTEGER_OVERFLOW;
   }

   /* Segment is a valid loadable segment */
   if(((sec_img_auth_is_valid_segment(img_data->elf.elf_format, ph_hdr)) && (ph_hdr->p_filesz != 0)) &&
      (sec_img_auth_is_valid_area_32(whitelist_area, ph_hdr->p_vaddr, ph_hdr->p_vaddr + ph_hdr->p_memsz)))
   {
      /* Check if it falls within the range of one of the ELF segments */
      ret = sec_img_auth_check_address_32(img_data->elf.elf_prg_hdr.hdr_32.elf_hdr->e_entry,
            img_data->elf.elf_prg_hdr.hdr_32.prog_hdr[i].p_paddr,
            img_data->elf.elf_prg_hdr.hdr_32.prog_hdr[i].p_filesz);
      if(ret == SEC_IMG_AUTH_SUCCESS)
      {
        /* ELF entry falls in valid area */
        return SEC_IMG_AUTH_SUCCESS;
      }

      /* Also check for virtual */
      ret = sec_img_auth_check_address_32(img_data->elf.elf_prg_hdr.hdr_32.elf_hdr->e_entry,
            img_data->elf.elf_prg_hdr.hdr_32.prog_hdr[i].p_vaddr,
            img_data->elf.elf_prg_hdr.hdr_32.prog_hdr[i].p_filesz);
      if(ret == SEC_IMG_AUTH_SUCCESS)
      {
        /* ELF entry falls in valid area */
        return SEC_IMG_AUTH_SUCCESS;
      }
    }
  }
  return SEC_IMG_AUTH_VALIDATE_IMAGE_AREA_INVALID_ENTRY;
}


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
)
{
  uint32 i = 0;
  uint32 len = 0;
  /* For all ELF segments, zero initialize the ZI region */
  /* Parameters are already checked in above function */
  /* For all loadable segments, make sure they fall in one of the whitelist areas */

   for(i = 0; i < elf->elf_prg_hdr.hdr_32.elf_hdr->e_phnum; i++)
   {
     if((sec_img_auth_is_valid_segment(elf->elf_format, 
	    (void*)&(elf->elf_prg_hdr.hdr_32.prog_hdr[i])))) 
     {
      /* This is a corrupted image */
      if ((elf->elf_prg_hdr.hdr_32.prog_hdr[i].p_filesz > 
           elf->elf_prg_hdr.hdr_32.prog_hdr[i].p_memsz) ||
         ((elf->elf_prg_hdr.hdr_32.prog_hdr[i].p_paddr + 
           elf->elf_prg_hdr.hdr_32.prog_hdr[i].p_memsz) < 
           elf->elf_prg_hdr.hdr_32.prog_hdr[i].p_paddr ))
      {
        /* Segment failing the sanity check */
        v_info->ph_segment = i;
        return SEC_IMG_AUTH_CORRUPTED_IMAGE;
      }

      len =  elf->elf_prg_hdr.hdr_32.prog_hdr[i].p_memsz - 
             elf->elf_prg_hdr.hdr_32.prog_hdr[i].p_filesz;

      /* Zero out the gap after the loadable segments within the memory region. 
        * Integer overflow is not needed as checked above.
        */

      if ((sec_img_auth_is_valid_area_32(elf->whitelist_area, elf->elf_prg_hdr.hdr_32.prog_hdr[i].p_paddr, 
           elf->elf_prg_hdr.hdr_32.prog_hdr[i].p_paddr + elf->elf_prg_hdr.hdr_32.prog_hdr[i].p_memsz)) && (len))
      {
        MEMSET((void *)(uintnt)(elf->elf_prg_hdr.hdr_32.prog_hdr[i].p_paddr + 
                    elf->elf_prg_hdr.hdr_32.prog_hdr[i].p_filesz), 0, len);
      }
     }
    }
 
  return SEC_IMG_AUTH_SUCCESS;
}
   

 



/**
 * @brief Populate the PIL info given the provided ELF image.
 *
 * @param[in] elf_hdr Pointer to the elf header
 * @param[in] img_auth Elf header info struct
 * @postcondition This routine allocates memory within \c sec_img_auth_elf_info_t. So
 *                the caller must free this memory when it is no longer needed.
 *
 * @return 0 on success, negative on failure.
 */
sec_img_auth_error_type
sec_img_auth_populate_elf_info_32 
(
  uint32 sec_img_id,
  const sec_img_auth_elf_info_type *img_data,
  sec_img_auth_priv_t * img_handle
)
{
  mi_boot_image_header_type   *mi_boot_hdr = NULL;
  sec_img_auth_elf_info_t *img_auth = NULL;
  uint32 elf_hdr_sz = 0;
  uint32 prog_tbl_sz = 0;
  uint32 hash_array_sz =0;
  uint32 image_hdr_size = 0;
  uint32 hash_seg_addr = 0;
  uint32 hash_seg_hdr_addr = 0;
  uint32 hash_seg_hdr_sz = 0;
  sec_img_auth_error_type ret = SEC_IMG_AUTH_FAILURE;  

  do
  {  
    /* Check input parameters */
    if((img_data == NULL) || (img_handle == NULL) ||
       (img_data->elf_hdr == NULL)|| 
       (img_data->hash_seg_hdr == NULL))
    {
      ret = SEC_IMG_AUTH_INVALID_ARG;
      break;
    }
  
    /* Check for the ELF */
    img_auth = &img_handle->elf;

    /*Ensure the program table size does not overflow.*/
    elf_hdr_sz = sizeof(Elf32_Ehdr);
    prog_tbl_sz =
      (uint32) (sizeof (Elf32_Phdr)) * (((Elf32_Ehdr*) img_data->elf_hdr)->e_phnum);

    /* Check for integer overflow */
    if (prog_tbl_sz !=
      (uint64) (sizeof (Elf32_Phdr) * (uint64)((Elf32_Ehdr*) img_data->elf_hdr)->e_phnum))
    {
      ret = SEC_IMG_AUTH_PROG_TBL_INVALID_SIZE;
      break;
    }

    if(prog_tbl_sz > (MI_PBT_MAX_SEGMENTS * sizeof (Elf32_Phdr)))
    {
      ret = SEC_IMG_AUTH_PROG_TBL_INVALID_SIZE;
      break;
    }

    /* Ensure there is only one hash table entry */
    /* Interger overflow check */
    if(((uint32)img_data->prog_hdr + prog_tbl_sz) < (uint32)img_data->prog_hdr)
    {
      return SEC_IMG_AUTH_HASH_SEG_INVALID_SIZE;
    }

    ret = sec_img_auth_validate_hash_segment_entries_32((Elf32_Phdr*)img_data->prog_hdr,
          (Elf32_Phdr*)((uint8*) img_data->prog_hdr + prog_tbl_sz),
          MI_PBT_HASH_SEGMENT);
    BREAKIF(ret != SEC_IMG_AUTH_SUCCESS);    

    /* Ensure the hash segment size without the cert chain does not overflow */
    hash_seg_hdr_addr = (uint32)img_data->hash_seg_hdr;
    hash_seg_hdr_sz = sizeof(mi_boot_image_header_type);
    hash_seg_addr = (uint32)hash_seg_hdr_addr + sizeof (mi_boot_image_header_type);
    hash_array_sz = ((Elf32_Ehdr*)img_data->elf_hdr)->e_phnum * CEML_HASH_DIGEST_SIZE_SHA256;
    /* Check for integer overflow */
    if (hash_seg_addr < hash_seg_hdr_addr)
    {
      ret = SEC_IMG_AUTH_HASH_SEG_INVALID_SIZE;
      break;
    }
    
    /*The complete hash segment is not overflowing*/
    if (hash_seg_addr + hash_array_sz < hash_seg_addr)
    {
      ret = SEC_IMG_AUTH_HASH_SEG_TOTAL_INVALID_SIZE;
      break;
    }

    /* Total size is Elf header + array of program headers + hash segment. */
    if(hash_array_sz > (MI_PBT_MAX_SEGMENTS * CEML_HASH_DIGEST_SIZE_SHA256))
    {
      ret = SEC_IMG_AUTH_HASH_SEG_INVALID_SIZE;
      break;
    }

    /* Each of these elements cannot be greater than 2K.
     * This is program header segments which are bounded times the Hash Size which is 32 bytes
     *  Also each of the headers are bounded. Program Table size is also bounded value.
     */
    if((elf_hdr_sz > SEC_IMG_AUTH_PAGE_2K) 
       || (prog_tbl_sz > SEC_IMG_AUTH_PAGE_2K)
       || (hash_seg_hdr_sz > SEC_IMG_AUTH_PAGE_2K)
       || (hash_array_sz > SEC_IMG_AUTH_PAGE_2K))
    {
      ret = SEC_IMG_AUTH_ELF_HDR_INVALID_SIZE;
      break;
    }

    /* The sum of all the items cannot also be greater than 2K */ 
    /* Overflow is not possile */
    image_hdr_size = elf_hdr_sz + prog_tbl_sz + hash_seg_hdr_sz + hash_array_sz;

    if (image_hdr_size > SEC_IMG_AUTH_PAGE_2K)
    {
      ret = SEC_IMG_AUTH_ELF_HDR_INVALID_SIZE;
      break;
    }
    img_auth->elf_prg_hdr.hdr_32.elf_hdr = img_data->elf_hdr;
    img_auth->elf_prg_hdr.hdr_32.prog_hdr = img_data->prog_hdr;
    img_auth->prog_hdr_num = img_auth->elf_prg_hdr.hdr_32.elf_hdr->e_phnum;
    
    img_auth->hash_seg_hdr = img_data->hash_seg_hdr;
    img_auth->hash_seg_hdr_sz = hash_seg_hdr_sz;
    img_auth->hash_seg = (uint8*)(uintnt)hash_seg_addr;
    img_auth->hash_array_sz = hash_array_sz;

    /*Check it is 4 byte aligned. Error if not aligned. */
    if (IS_NOT_ALIGNED(img_auth->hash_seg_hdr))
    {
      ret = SEC_IMG_AUTH_MIHDR_NOT_ALIGNED;
      break;
    }

    /*Assign the MI Header */
    mi_boot_hdr= (mi_boot_image_header_type*)img_auth->hash_seg_hdr;

    ret = sec_img_auth_validate_mi_boot((const sec_img_auth_priv_t *) img_handle, mi_boot_hdr);
    BREAKIF(ret != SEC_IMG_AUTH_SUCCESS);    

    img_auth->sig_sz = mi_boot_hdr->signature_size;
    img_auth->cert_sz = mi_boot_hdr->cert_chain_size; 

    if((mi_boot_hdr->signature_size > 0) && (img_auth->cert_sz > 0))
    {
      /* Signature now comes after hash table */
      img_auth->sig_ptr = (uint8*)(uintnt)hash_seg_addr + hash_array_sz;
    
       /*Overflow check to make sure certs do not overflow */
       if (((uint32)img_auth->sig_ptr  + img_auth->sig_sz) < (uint32)img_auth->sig_ptr)
       {
         ret = SEC_IMG_AUTH_SIG_INVALID_SIZE;
         break;
       }
       img_auth->cert_ptr = (uint8*)(img_auth->sig_ptr + img_auth->sig_sz);
     
      /* Overflow check to ensure the end of the cert chain does not overflow */
      if (((uint32)img_auth->cert_ptr  + img_auth->cert_sz) < (uint32)img_auth->cert_ptr)
      {
        ret = SEC_IMG_AUTH_CERT_INVALID_SIZE;
        break;
      }
    }
    img_auth->uie_hdr_ptr = (uint8*)(uintnt)hash_seg_addr + hash_array_sz + img_auth->sig_sz + img_auth->cert_sz;
    /*Size of hash segment is multiple of SHA hashes of all individual segments */
    if(mi_boot_hdr->code_size != img_auth->hash_array_sz)
    {
      ret = SEC_IMG_AUTH_HASH_SEG_ARRAY_INVALID_SIZE;
      break;
    } 
    
   /* Not running over the Hash Table Segment File Size */
   /*  Check that the hash segment length does not exceed the expected length */

   ret = sec_img_auth_validate_hash_segment_len_32(img_auth);
   BREAKIF (ret != SEC_IMG_AUTH_SUCCESS);
  
   /*Reaches here in case of success scenario */
   ret = SEC_IMG_AUTH_SUCCESS;
  }
  while (0);

  return ret;
}



