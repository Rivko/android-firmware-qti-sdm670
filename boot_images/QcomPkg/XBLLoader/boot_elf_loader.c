/*===========================================================================

                              ELF Image Loader

GENERAL DESCRIPTION
  This module performs generic ELF image loading. 

Copyright 2011 - 2017 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who        what, where, why
--------   ---        -------------------------------------------------------
06/06/17   ds         remove cache flush after hashing since the regions are locked by xbl_sec
03/10/17   kpa        Update boot_get_elf_phdr_entry_info arguments
03/07/17   kpa        Update .bss.BOOT_INTERNAL_HEAP to .bss.BOOT_INTERNAL_MEMZI
03/03/17   ds         remove clobber prot
02/22/17   yps        Fix compiler migration issue
01/27/17   digants    changes for whitelist based image loading 
01/20/17   kpa        update boot_get_elf_phdr_entry_info
01/20/17   kpa        Add buffer to local clobber list to allow loading
10/14/16   kpa        Added api's to support xbl config image loading
11/23/16   kpa        update sec img auth api return types
10/06/16   kpa        update cache flush for conditionally loaded segments
09/19/16   kpa        Flush elf and program header buffers before passing info to xbl sec
04/29/16   kpa        skip hash check if image auth is disabled from config entry flag
04/19/16   kpa        Add "boot_clobber_remove_local_hole" for each corresponding
                      "boot_clobber_add_local_hole"
03/17/16   bd         Remove clobber holes after their usage is done
02/09/16   kpa        Flush the segment memory region after decryption.
09/02/15   kpa        Flush the segment memory region after loading
07/15/15   ck         Removed static from boot_elf_load_deinit and added ability to call anytime
07/01/15   as         Fetch segmented ELF's entry point from boot_config
05/26/15   kpa        Add config entry whitelist parameter to boot_populate_sbl_info
10/27/14   kedara     Use boot_get_hash_segment_buffer to get hash seg base address
02/05/15   ck         Added integer overflow check in boot_elf_load_seg_in_chunks
01/28/15   ck         Added clobber check and protection in boot_elf_load_seg_in_chunks
01/19/15   plc        Check hash-pointer calculation for overflow to catch potential out-of-bounds condition
10/27/14   kedara     Added SAFE 1.0
10/14/14   kedara     Added support for pmic elf image loading
10/09/14   kedara     Added boot_elf_get_filtered_segment_start
09/24/14   kedara     Added boot_set_elf_and_prog_header
09/17/14   ck         Fixed null whitelist loading bug in boot_elf_load_and_verify_image
06/13/14   ck         Whitelist region is now passed to elf loading logic for CONFIG_IMG_ELF_SEG
06/11/14   jz         Fixed boot_is_auth_enabled to check return status in the caller
06/09/14   kedara     Update to support 64 bit compilation on llvm.
05/12/14   kedara     Update MAX_ADDRESS name to MAX_ADDRESS_64.
04/11/14   ck         Added explicit clobber check using ELF mem size when loading a segment.
                      Added loaded ELF segment to clobber protection to prevent another image
                      from overwriting it.
03/05/14   ck         Passing QC header buffer to boot_elf_open_and_parse_elf_header 
                      and boot_elf_verify_image_header so it can be cleared before use
02/12/14   ck         Validate program header before loading the segment
12/10/13   ck         boot_is_auth_enabled no longer has a return value
12/09/13   jz         Added 64bit elf loading support
12/06/13   ck         Replaced calls to boot_pbl_is_auth_enabled with boot_is_auth_enabled
12/02/13   ck         Added boot_get_elf_eident_class function
10/18/13   dhaval     explicit hash-verify for ehdr+phdr after hash-table auth.
                      Changes to do segment hash cheks only if ehdr+phdr hash 
				      check was successful
08/24/13   lm         Fixed bootup failure issue when SDI as elf featue enabled and
				      SDI image not flashed case.
07/29/13   aus        Support for loading ELF SDI image
07/29/13   dh         Fix a bug in hash buffer size check
07/16/13   aus        Update the hash size check
05/31/13   kedara     Add hash segment buffer to local clobber table.
04/17/13   kedara     Support mpss authentication, Non-MSA boot flow.
03/28/13   dhaval     updates to support sha256 
02/19/13   kedara     Added boot_get_elf_header, boot_get_program_header.
01/30/13   kedara     Subsystem self authentication updates.
11/08/12   kedara     Updates to enable hashing and authentication of elf's.
09/28/12   kedara     Relocate boot_rmb_set_pmi_elf_load_ready. Check memsize
                      before validating segment dest addr.
09/21/12   jz         Removed redundant call to boot_elf_load_init_hash_segment
09/20/12   kedara     Load hash segment to fixed buffer in DDR ZI.
09/12/12   kedara     Fix bss segment zero out logic.
06/15/12   kedara     Added subsystem self authentication (ssa) for elf images.
06/11/12   kedara     include boot_flash_dev_if.h. Added crypto auth lib
                      wrapper api boot_secboot_hash*
08/24/11   aus        Fixed the hash verification for case when the segment 
                      ELF size is 0x0
07/06/11   aus        Fixed adding the clobber hole for image header
03/14/11   dxiang     Initial revision.
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "boot_loader.h"
#include "boot_error_handler.h"
#include "boot_util.h"
#include "MIprogressive.h"
#include "boot_flash_dev_if.h"
#include "boot_rmb.h"
#include "boot_hash_buffer.h"
#include "boot_hash.h"
#include "boot_elf_header.h"
#include "boot_extern_sec_img_interface.h"
#include "boot_authenticator.h"
#include "boot_sbl_qsee_interface.h"
#include "boot_cache.h"
#include "stdint.h"
#include "boot_fastcall_tz.h"

/*===========================================================================
                    VARIABLE DECLARATIONS FOR MODULE
===========================================================================*/
/* Data Chunk size to be 1MB */
#define MAX_CHUNK_SIZE 0x100000

#define MAX_ADDRESS_32 0xFFFFFFFF
#define MAX_ADDRESS_64 0xFFFFFFFFFFFFFFFF

/* Specifies number of whitelist regions images can be loaded to */
#define MAX_IMG_BUFFERS 15

/* Variables to be placed in DDR so as to be accessible by drivers (eg Crypto)
    using BAM.
*/

/* Program Header array
   In consideration of limited memory, the maximum number of program header 
   entries may be configurable from the top level Scons file of any image 
   which may use the ELF loader module. */
static Elf32_Phdr phdr_array_32[MI_PBT_MAX_SEGMENTS] SECTION(".bss.BOOT_INTERNAL_MEMZI");
static Elf64_Phdr phdr_array[MI_PBT_MAX_SEGMENTS] SECTION(".bss.BOOT_INTERNAL_MEMZI");

/* ELF Header */
static Elf32_Ehdr ehdr_32 SECTION(".bss.BOOT_INTERNAL_MEMZI");
static Elf64_Ehdr ehdr SECTION(".bss.BOOT_INTERNAL_MEMZI");

/* Output digest buffer for comparing hashes */
static uint8 verify_digest_buf[PBL_SHA_HASH_MAX_LEN] SECTION(".bss.BOOT_INTERNAL_MEMZI");
extern hash_alg_t sbl_hash_algorithm SECTION(".bss.BOOT_INTERNAL_MEMZI");
extern uint32 sbl_hash_len SECTION(".bss.BOOT_INTERNAL_MEMZI");

/* ELF format (object class), default to 64bit */
static uint8 elf_format SECTION(".bss.BOOT_INTERNAL_MEMZI");

static void *ehdr_ptr, *phdr_array_ptr SECTION(".bss.BOOT_INTERNAL_MEMZI");
static uint32 ehdr_size, phdr_size SECTION(".bss.BOOT_INTERNAL_MEMZI");
static uint16 phdr_num SECTION(".bss.BOOT_INTERNAL_MEMZI");


/* Flash translation interface */
static boot_flash_trans_if_type * trans_if = NULL;

/* Start of the hash entries in memory */
static uint8 * hash_segment_start = NULL;


/* Subsystem Self authentication (SSA) enable check */
static boolean elf_ssa_enabled = FALSE;

/* Flag to indicate if config table entry enables authentication */
static boolean elf_cfg_table_auth = FALSE;

/* Subsystem Self auth meta info variables */
static uint8* hash_segment_base_addr = 0x0;
static uint32  hash_segment_length = 0x0;
static uint32  total_elf_segments_size = 0x0;

/* flag to track whether ehdr+phdr hashing was successful or not */
static boolean boot_elf_is_ehdr_phdr_hash_success = FALSE;

/* Temporary buffer to use for loading hash segment */
static uint8 *hash_segment_base_ptr = NULL;

static sec_img_auth_whitelist_area_t sbl_white_list_area[MAX_IMG_BUFFERS];

static whitelst_tbl_entry_type elf_hdr_meta_whitelist[] = {{(uint64)(&ehdr_32),(uint64)(sizeof(ehdr_32))},
  {(uint64)(&phdr_array_32),(uint64)(sizeof(phdr_array_32))},{(uint64)(&ehdr),(uint64)(sizeof(ehdr))},
  {(uint64)(&phdr_array),(uint64)(sizeof(phdr_array))},{0,0}};

/*===========================================================================
                        LOCAL FUNCTION DEFINITIONS
=============================================================================*/

/*===========================================================================
**  Function :  boot_populate_sbl_info
** ==========================================================================
*/
/*!
* 
* @brief
*   This function populates meta data pointers to be passed to sec img auth
*   lib
*
* @par Dependencies
*   None 
* 
* @retval
*   SEC_IMG_AUTH_SUCCESS - If ELF images is verified successfully
*   ERROR VALUES - In case of failure.
* 
*/
uint32 boot_populate_sbl_info (sec_img_auth_elf_info_type *sbl_elf_info, 
                                sec_img_auth_whitelist_area_param_t *sbl_white_list_param,
                                whitelst_tbl_entry_type * config_entry_whitelist_table )
{
  uint32 index = 0;
  sec_img_auth_error_type status = SEC_IMG_AUTH_FAILURE;

  do
  {  
    sbl_white_list_area[0].start = (uintnt)0x0;
    sbl_white_list_area[0].end   = (uintnt)0xFFFFFFFFFFFFFFFF;
    sbl_white_list_param->area = &sbl_white_list_area[0];
    sbl_white_list_param->count = 0x1;

    if(NULL != config_entry_whitelist_table)
    {
      while((0x0 !=config_entry_whitelist_table->size) &&
            (index < (MAX_IMG_BUFFERS -1))
           )
      {
        /*Fill up the secImgAuth whitelist */
        sbl_white_list_area[index].start = config_entry_whitelist_table->start_addr;
        sbl_white_list_area[index].end = (config_entry_whitelist_table->start_addr + 
                                          config_entry_whitelist_table->size);
        config_entry_whitelist_table++;
        index++;      
      }
      
      if(config_entry_whitelist_table->start_addr != 0x0)
      { 
        /* At this point we should reach end of config entry whitelist
           and have null value unless the index maxed out the limit
        */
        status = SEC_IMG_AUTH_MI_BOOT_INVALID_SIZE;
        break;
      }
      sbl_white_list_param->count = index;
    }

    if (elf_format == ELF_CLASS_32)
    {
      sbl_elf_info->elf_hdr      = (void *)&ehdr_32;
      sbl_elf_info->prog_hdr     = (void *)phdr_array_32;
      sbl_elf_info->hash_seg_hdr = boot_get_hash_segment_buffer();
    }
    else if (elf_format == ELF_CLASS_64)
    {
      sbl_elf_info->elf_hdr      = (void *)&ehdr;
      sbl_elf_info->prog_hdr     = (void *)phdr_array;
      sbl_elf_info->hash_seg_hdr = boot_get_hash_segment_buffer();
    }
    else
    {
      status = SEC_IMG_AUTH_ELF_INVALID_CLASS;
      break;
    }
    /*If we reach this point all is good */
    status = SEC_IMG_AUTH_SUCCESS;
  }while(0);

  return status;
}

/*===========================================================================
**  Function :  boot_elf_verify_header
** ==========================================================================
*/
/*!
* 
* @brief
*   This function verifies the ELF Header by sanity checking various fields
*   within the ELF Header structure 
*
* @par Dependencies
*   None 
* 
* @retval
*   TRUE - If ELF Header has passed verification
*   FALSE - Otherwise
* 
*/
static boolean boot_elf_verify_header()
{
  /* Sanity check the ELF header,
    * and ensure that there is at least 1 prog-header entry
    * and total number of prog-hdr segments are not more than the allocated prog-hdr buffer
    */
  if (elf_format == ELF_CLASS_32)
    return (  ehdr_32.e_ident[ELFINFO_MAG0_INDEX] == ELFINFO_MAG0 && 
            ehdr_32.e_ident[ELFINFO_MAG1_INDEX] == ELFINFO_MAG1 &&
            ehdr_32.e_ident[ELFINFO_MAG2_INDEX] == ELFINFO_MAG2 &&
            ehdr_32.e_ident[ELFINFO_MAG3_INDEX] == ELFINFO_MAG3 &&
            ehdr_32.e_ident[ELFINFO_CLASS_INDEX] == ELF_CLASS_32 &&
            ehdr_32.e_ident[ELFINFO_VERSION_INDEX] == ELF_VERSION_CURRENT &&
            ehdr_32.e_ehsize == sizeof(Elf32_Ehdr) &&
            ehdr_32.e_phentsize == sizeof(Elf32_Phdr) &&
            ehdr_32.e_phnum != 0 &&
            ehdr_32.e_phnum <= MI_PBT_MAX_SEGMENTS);
            
  else if (elf_format == ELF_CLASS_64)
  return (  ehdr.e_ident[ELFINFO_MAG0_INDEX] == ELFINFO_MAG0 && 
            ehdr.e_ident[ELFINFO_MAG1_INDEX] == ELFINFO_MAG1 &&
            ehdr.e_ident[ELFINFO_MAG2_INDEX] == ELFINFO_MAG2 &&
            ehdr.e_ident[ELFINFO_MAG3_INDEX] == ELFINFO_MAG3 &&
            ehdr.e_ident[ELFINFO_CLASS_INDEX] == ELF_CLASS_64 &&
            ehdr.e_ident[ELFINFO_VERSION_INDEX] == ELF_VERSION_CURRENT &&
            ehdr.e_ehsize == sizeof(Elf64_Ehdr) &&
            ehdr.e_phentsize == sizeof(Elf64_Phdr) &&
            ehdr.e_phnum != 0 &&
            ehdr.e_phnum <= MI_PBT_MAX_SEGMENTS);

  else
    return FALSE; // invalid

} 

/*===========================================================================
**  Function :  boot_elf_phdr_is_loadable
** ==========================================================================
*/
/*!
* 
* @brief
*   This function checks if a specific program header should be loaded
*   into memory or not
*
* @param[in] phdr - Pointer to the Program Header structure 
*
* @par Dependencies
*   None 
* 
* @retval
*   TRUE - If Program Header should be loaded
*   FALSE - Otherwise
* 
*/
static boolean boot_elf_phdr_is_loadable(void * phdr)
{
  uint32 phdr_type;
  uint32 phdr_flags;
  
  BL_VERIFY( phdr != NULL, BL_ERR_NULL_PTR_PASSED|BL_ERROR_GROUP_BOOT );

  phdr_type = (elf_format == ELF_CLASS_64) ? ((Elf64_Phdr *)phdr)->p_type : ((Elf32_Phdr *)phdr)->p_type;
  phdr_flags = (elf_format == ELF_CLASS_64) ? ((Elf64_Phdr *)phdr)->p_flags : ((Elf32_Phdr *)phdr)->p_flags;

  return (  phdr_type == MI_PBT_ELF_PT_LOAD  &&
            MI_PBT_PAGE_MODE_VALUE(phdr_flags) == MI_PBT_NON_PAGED_SEGMENT  &&
            MI_PBT_ACCESS_TYPE_VALUE(phdr_flags) != MI_PBT_NOTUSED_SEGMENT  &&
           /* hash segment to be loaded explicitly to fixed buffer */
            MI_PBT_SEGMENT_TYPE_VALUE(phdr_flags) != MI_PBT_HASH_SEGMENT   &&
            MI_PBT_ACCESS_TYPE_VALUE(phdr_flags) != MI_PBT_SHARED_SEGMENT  );
} 

/*===========================================================================
**  Function :  boot_elf_enable_hash_checking
** ==========================================================================
*/
/*!
* 
* @brief
*   This function determines whether or not ELF hash verification should be
*   skipped. Default behavior is to verify all data segments with their
*   respective hash entries. If secure boot is enabled, hash checking is 
*   forced active.
*
* @par Dependencies
*   Define FEATURE_BOOT_SKIP_ELF_HASH_VERIFICATION to skip verification
* 
* @retval
*   TRUE - If ELF hash verification is enabled
*   FALSE - Otherwise
* 
*/
static boolean boot_elf_enable_hash_checking()
{
  boot_boolean is_auth_enabled = FALSE;

  secboot_hw_etype status = boot_is_auth_enabled(&is_auth_enabled);
  BL_VERIFY((status == E_SECBOOT_HW_SUCCESS), (uint16)status|BL_ERROR_GROUP_SECURITY);

  if (is_auth_enabled && elf_cfg_table_auth)
  {
    return TRUE;
  }
  else
  {
#ifdef FEATURE_BOOT_SKIP_ELF_HASH_VERIFICATION
    return FALSE;
#else
    return TRUE;
#endif
  }
}

/*===========================================================================
**  Function :  boot_elf_load_seg_generic
** ==========================================================================
*/
/*!
* 
* @brief
*   This function loads the data segment into memory using the information
*   from a specific program header and input destination address.  
*
* @param[in] phdr - Pointer to the Program Header structure 
*            dest_addr - pointer to memory region where seg needs to be
*                        loaded
*
* @par Dependencies
*   None 
* 
* @retval
*   None
* 
*/
static void boot_elf_load_seg_generic(void * phdr, uint8 *dest_addr, buffer_type bf_type)
{
  boolean success = FALSE;
  uint64 phdr_offset, phdr_filesz, phdr_memsz;

  BL_VERIFY( (phdr != NULL), BL_ERR_NULL_PTR_PASSED|BL_ERROR_GROUP_BOOT);
   
  phdr_offset = (elf_format == ELF_CLASS_64) ? ((Elf64_Phdr *)phdr)->p_offset : ((Elf32_Phdr *)phdr)->p_offset;
  phdr_filesz = (elf_format == ELF_CLASS_64) ? ((Elf64_Phdr *)phdr)->p_filesz : ((Elf32_Phdr *)phdr)->p_filesz;
  phdr_memsz = (elf_format == ELF_CLASS_64) ? ((Elf64_Phdr *)phdr)->p_memsz : ((Elf32_Phdr *)phdr)->p_memsz;



  /* As we are checking for memsz to be covered in whitelist , do a check only if memsz is not 0
  	this is to avoid false failure for dummy segements in QSEE/QHEE image */
  if(phdr_memsz)
  {
    /* There is a whitelist check in the flash read logic but that will only
	check the elf file size length.  Need to ensure that zero init regions
  	are also checked.  So make an explicit whitelist check using mem size to
  	be safe. */			
  	BL_VERIFY(boot_whitelist_address_range_check((uint64)dest_addr,(uint64)phdr_memsz,bf_type),
            BL_ERR_CLOBBER_PROTECTION_VIOLATION|BL_ERROR_GROUP_BOOT);
  }
  /* Only process segments with length greater than zero */
  /* 0x0 is a valid address.  Hence no need to validate dest_addr for null pointer */
  if(phdr_memsz && phdr_filesz && (phdr_memsz >= phdr_filesz))
  {
    /* Copy the data segment flash into memory */
    success = boot_flash_trans_read( trans_if,
                                     dest_addr,
                                     (uint32)phdr_offset,
                                     (uint32)phdr_filesz,
                                     bf_type);
    BL_VERIFY( success, BL_ERR_FLASH_READ_FAIL|BL_ERROR_GROUP_BOOT );
  }
   
 
  /* If uninitialized data exists, make sure to zero-init */
  if ((phdr_memsz > phdr_filesz) && ((dest_addr + phdr_filesz) >= dest_addr))
  {
    qmemset((uint8 *)(uintnt)(dest_addr + phdr_filesz), 
            0,
            (phdr_memsz - phdr_filesz)); 
  }

  /* Flush the segment memory region after loading */
  dcache_flush_region((void *)dest_addr, phdr_memsz);

}


/*===========================================================================
**  Function :  boot_elf_load_seg_in_chunks
** ==========================================================================
*/
/*!
* 
* @brief
*   This function loads the data segment into memory using the information
*   from a specific program header.  
*
* @param[in] phdr - Pointer to the Program Header structure 
*
* @par Dependencies
*   None 
* 
* @retval
*   None
* 
*/
static void boot_elf_load_seg_in_chunks(void * phdr)
{
  boolean success = FALSE;  
  uint32 chunk_size = MAX_CHUNK_SIZE;
  uint64 remaining_data = 0, chunk_dest_addr =0, chunk_offset =0;
  uint64 phdr_entry_offset, phdr_entry_paddr, phdr_entry_filesz, phdr_entry_memsz;

  boot_boolean is_auth_enabled = FALSE;
  secboot_hw_etype status = E_SECBOOT_HW_FAILURE;

  BL_VERIFY( phdr != NULL, BL_ERR_NULL_PTR_PASSED|BL_ERROR_GROUP_BOOT );

  if (elf_format == ELF_CLASS_64)
  {
    phdr_entry_offset = ((Elf64_Phdr *)phdr)->p_offset;
    phdr_entry_paddr = ((Elf64_Phdr *)phdr)->p_paddr;
    phdr_entry_filesz = ((Elf64_Phdr *)phdr)->p_filesz;
    phdr_entry_memsz = ((Elf64_Phdr *)phdr)->p_memsz;
  }
  else
  {
    phdr_entry_offset = ((Elf32_Phdr *)phdr)->p_offset;
    phdr_entry_paddr = ((Elf32_Phdr *)phdr)->p_paddr;
    phdr_entry_filesz = ((Elf32_Phdr *)phdr)->p_filesz;
    phdr_entry_memsz = ((Elf64_Phdr *)phdr)->p_memsz;
  }
  
  if(boot_sec_img_auth_is_valid_segment(elf_format, phdr))
  {
    /* Only process segments with valid parameters
       0x0 is a valid address. Hence no need to validate 
       dest_addr for null pointer 
    */
    if (phdr_entry_memsz && phdr_entry_filesz && (phdr_entry_memsz >= phdr_entry_filesz))
    {
      remaining_data = phdr_entry_filesz;
      chunk_dest_addr = phdr_entry_paddr;
      chunk_offset = phdr_entry_offset;
      while(remaining_data !=0)
      {
        if(remaining_data < chunk_size )
        {
          chunk_size = remaining_data;
        }
          
        /* Copy the data segment flash into memory */
        success = boot_flash_trans_read( trans_if,
                                         (uint8 *)(uintnt)chunk_dest_addr,
                                         chunk_offset,
                                         chunk_size,
                                         IMAGE_BODY_TYPE);
        BL_VERIFY( success, BL_ERR_FLASH_READ_FAIL|BL_ERROR_GROUP_BOOT );
          
        chunk_dest_addr += chunk_size;
        chunk_offset += chunk_size;
        remaining_data = remaining_data - chunk_size;
        
        /* Update size indicating how much elf is loaded */
        BL_VERIFY((total_elf_segments_size + chunk_size > total_elf_segments_size),
                  BL_ERR_IMG_SIZE_TOO_BIG|BL_ERROR_GROUP_BOOT);  

        total_elf_segments_size = total_elf_segments_size + chunk_size;

        status = boot_is_auth_enabled(&is_auth_enabled);
       BL_VERIFY((status == E_SECBOOT_HW_SUCCESS), (uint16)status|BL_ERROR_GROUP_SECURITY);

        if(!is_auth_enabled)
        { 
          /* Set total length of loaded segments as soon as chunk of data is
           loaded ONLY if MSA is enabled and SBL is not doing authentication.
           Else to be done post hash verification. mba will xpu lock segments
           as soon as length is updated 
          */
          boot_rmb_set_pmi_code_length(total_elf_segments_size);
        }
      }
    }
	
    /* If uninitialized data exists, make sure to zero-init */
    if ((phdr_entry_memsz > phdr_entry_filesz) && ((phdr_entry_paddr + phdr_entry_filesz) >= phdr_entry_paddr))
    {

      qmemset((uint8 *)(uintnt)(phdr_entry_paddr + phdr_entry_filesz),
              0,
              (phdr_entry_memsz - phdr_entry_filesz)); 
		
      /* Update size indicating how much elf is loaded */
      BL_VERIFY((total_elf_segments_size + (phdr_entry_memsz - phdr_entry_filesz) > total_elf_segments_size),
                BL_ERR_IMG_SIZE_TOO_BIG|BL_ERROR_GROUP_BOOT); 

      total_elf_segments_size = total_elf_segments_size + (phdr_entry_memsz - phdr_entry_filesz);

      status = boot_is_auth_enabled(&is_auth_enabled);
      BL_VERIFY((status == E_SECBOOT_HW_SUCCESS), (uint16)status|BL_ERROR_GROUP_SECURITY);

      if(!is_auth_enabled)
      {
        /* Set total length of loaded segments as soon as chunk of data is
            loaded ONLY if MSA is enabled and SBL is not doing authentication.
            Else to be done post hash verification. mba will xpu lock segments
            as soon as length is updated 
        */
        boot_rmb_set_pmi_code_length(total_elf_segments_size);
      }
    }

  }
}



/*===========================================================================
**  Function :  boot_elf_load_segment
** ==========================================================================
*/
/*!
* 
* @brief
*   This function loads the data segment into memory using the information
*   from a specific program header.  
*
* @param[in] phdr - Pointer to the Program Header structure 
*
* @par Dependencies
*   None 
* 
* @retval
*   None
* 
*/
static void boot_elf_load_seg(void * phdr)
{
  uint64 phdr_entry_paddr;
  
  BL_VERIFY( phdr != NULL, BL_ERR_NULL_PTR_PASSED|BL_ERROR_GROUP_BOOT );

  phdr_entry_paddr = (elf_format == ELF_CLASS_64) ? ((Elf64_Phdr *)phdr)->p_paddr : ((Elf32_Phdr *)phdr)->p_paddr;
  
  if(boot_sec_img_auth_is_valid_segment(elf_format, phdr))
  {
    boot_elf_load_seg_generic(phdr, (uint8 *)(uintnt)phdr_entry_paddr, IMAGE_BODY_TYPE);
  }
}

/*===========================================================================
**  Function :  boot_elf_verify_segment
** ==========================================================================
*/
/*!
* 
* @brief
*   This function will verify a particular Program Header with an expected
*   hash value. This function returns nothing, but completes upon success.
*   The program headers are verified differently as follows:
*   
*   1. Type MI_PBT_PHDR_SEGMENT - Hash over ELF Header + All Program Headers
*   2. Type MI_PBT_HASH_SEGMENT - Do not verify the hash segment itself
*   3. Any loadable segment - Hash over the full data segment 
*   4. Otherwise - Return
*
* @param[in] phdr - Pointer to the Program Header structure 
* @param[in] expected_hash - Byte Pointer to the expected hash value 
*
* @par Dependencies
*   None 
* 
* @retval
*   None
* 
*/
static void boot_elf_verify_segment(void * phdr, uint8 * expected_hash)
{
  uint32 phdr_entry_flags;
  uint64 phdr_entry_paddr, phdr_entry_filesz;
  bl_error_boot_type status;
  if (elf_format == ELF_CLASS_64)
  {
    phdr_entry_flags = ((Elf64_Phdr *)phdr)->p_flags;
    phdr_entry_paddr = ((Elf64_Phdr *)phdr)->p_paddr;
    phdr_entry_filesz = ((Elf64_Phdr *)phdr)->p_filesz;
  }
  else
  {
    phdr_entry_flags = ((Elf32_Phdr *)phdr)->p_flags;
    phdr_entry_paddr = ((Elf32_Phdr *)phdr)->p_paddr;
    phdr_entry_filesz = ((Elf32_Phdr *)phdr)->p_filesz;
  }
    
  /* ensure that hash_table is authenticated before trusting/using 
     hashes from hash table for segment hashing */
  BL_VERIFY( (boot_get_hash_table_auth_result() == TRUE), 
              BL_ERR_HASH_TBL_AUTH_FAIL|BL_ERROR_GROUP_BOOT);
   
  /* proceed to do elf segment hashing only if ehdr+phdr hash check
     was already completed successfully */
  BL_VERIFY( (boot_elf_is_ehdr_phdr_hash_success == TRUE), 
              BL_ERR_HASH_TBL_AUTH_FAIL|BL_ERROR_GROUP_BOOT);

  if( MI_PBT_SEGMENT_TYPE_VALUE(phdr_entry_flags) == MI_PBT_PHDR_SEGMENT)
  {
    /* ehdr+phdr is already verified, continue */ 
    return;
  }  
  /* Skip hash verification for the hash segment */
  else if( MI_PBT_SEGMENT_TYPE_VALUE(phdr_entry_flags) == MI_PBT_HASH_SEGMENT)
  {
    return;
  }
  /* Verify hash for the load segment only if the file size is not 0 */
  else if (boot_sec_img_auth_is_valid_segment(elf_format,phdr) && (phdr_entry_filesz > 0))
  {
    /* Computer hash over full data segment */
    status = boot_hash(sbl_hash_algorithm,
                          (uint8 *)(uintnt)phdr_entry_paddr,
                          (uint32)phdr_entry_filesz,
                          verify_digest_buf,
                          sbl_hash_len);  
    /* Computer hash over full data segment */
    BL_VERIFY(BL_ERR_NONE == status,(uint16)status|BL_ERROR_GROUP_CRYPTO);
    
    /* Compare calculated hash value to entry in hash table */
    BL_VERIFY (bByteCompare( verify_digest_buf, 
                                expected_hash, 
                                sbl_hash_len ) == 0,
                  BL_ERR_ELF_HASH_MISMATCH|BL_ERROR_GROUP_BOOT); 
  }
  else
  {
    return;
  }
}



/*===========================================================================
**  Function :  boot_elf_ziinit_structures
** ==========================================================================
*/
/*!
* 
* @brief
*   This function zero-initializes all global variables within the module which
*   are necessary to perform ELF loading.  
*
* @param[out] header_ptr - Pointer to the QC header that is read from the image
*
* @par Dependencies
*   None 
* 
* @retval
*   None
* 
*/
void boot_elf_ziinit_structures(mi_boot_image_header_type *header_ptr)
{
  /* reset the flag tracking hash table authentication result for current elf */
  boot_set_hash_table_auth_result(FALSE);
  
  /* reset the flag tracking ehdr+phdr hash check for current elf */
  boot_elf_is_ehdr_phdr_hash_success = FALSE;
  
  /* zi-init header which holds hash-table info */
  if (header_ptr)
    qmemset((uint8 *)header_ptr, 0, sizeof(mi_boot_image_header_type));

  /* zi-init elf header */
  qmemset((uint8 *)&ehdr_32, 0, sizeof(Elf32_Ehdr));
  qmemset((uint8 *)&ehdr, 0, sizeof(Elf64_Ehdr));

  /* zi-init prog-header */
  qmemset((uint8 *)(phdr_array_32), 0, (sizeof(Elf32_Phdr) * MI_PBT_MAX_SEGMENTS));
  qmemset((uint8 *)(phdr_array), 0, (sizeof(Elf64_Phdr) * MI_PBT_MAX_SEGMENTS));

  /* zi-init hash table segment */
  qmemset((uint8 *)(boot_get_hash_segment_buffer()), 0, HASH_SEGMENT_SIZE);
}


/*===========================================================================
**  Function :  boot_seg_verification
** ==========================================================================
*/
/*!
* 
* @brief
*   This function verifies loaded elf segment and updates hash pointer
*
* @par Dependencies
*   Must be called only after loading elf segment
*
* @param[in] index - Index of current segment in program header table 
* @param[in] curr_hash_ptr_val - Pointer to seg hash entry in hash table
* 
* @retval
*   None
*/
static void boot_seg_verification(uint32 index, uint8 **curr_hash_ptr_val)
{
  void *phdr_entry_ptr;
  uint32 phdr_entry_flags;
  uint64 phdr_entry_filesz;

  if (elf_format == ELF_CLASS_64)
  {
    phdr_entry_ptr = &phdr_array[index];
    phdr_entry_flags = phdr_array[index].p_flags;
    phdr_entry_filesz = phdr_array[index].p_filesz;
  }
  else
  {
    phdr_entry_ptr = &phdr_array_32[index];
    phdr_entry_flags = phdr_array_32[index].p_flags;
    phdr_entry_filesz = phdr_array_32[index].p_filesz;
  }
  	
  /* Verify the segment if necessary */
  if(boot_elf_enable_hash_checking())
  {
    /* Validate that the hash ptr offsets do not overflow beyond the allocated buffer and that 
       there is atleast one hash_digest_size of data left over in the buffer for the last hash 
       ptr offset */
    BL_VERIFY((( (uintnt)*curr_hash_ptr_val <= ((uintnt)hash_segment_start + HASH_SEGMENT_SIZE - sbl_hash_len)) && 
               (((uintnt)*curr_hash_ptr_val) >= (uintnt)hash_segment_start)), BL_ERR_INVALID_HASH_PTR|BL_ERROR_GROUP_BOOT);
    
    boot_elf_verify_segment((void *)phdr_entry_ptr, *curr_hash_ptr_val);

    /* Increment the hash pointer accordingly */
    if( MI_PBT_PAGE_MODE_VALUE(phdr_entry_flags) == MI_PBT_PAGED_SEGMENT )
    {
      /* Paged segments have a hash entry for each 4K page of data.
        Thus these hash entries must be skipped to find the hash
        entry for the next segment. */

      /* Compute number of 4K pages by rounding up */
      uintnt num_4k_pages = ( ( (uintnt)phdr_entry_filesz - 1 ) / SIZE4K ) + 1;  

      *curr_hash_ptr_val += num_4k_pages * sbl_hash_len;
    }
    else
    {
      *curr_hash_ptr_val += sbl_hash_len;
    } 
  }
}

/*===========================================================================
**  Function :  boot_elf_load_and_verify_image_in_chunks
** ==========================================================================
*/
/*!
* 
* @brief
*   This function loads all the data segments corresponding to the global
*   program header array in chunks and also verifies hashes if applicable. 
*
* @par Dependencies
*   boot_elf_load_init_hash_segment() must be called
* 
* @retval
*   None
*/
static void boot_elf_load_and_verify_image_in_chunks(void)
{
  boot_boolean is_auth_enabled = FALSE;
  uint32 index = 0;
  secboot_hw_etype status = E_SECBOOT_HW_FAILURE;


  /* Pointer to keep track of expected hash values ~ Initialized to the first
   * entry in the hash segment. */
  uint8 * curr_hash_ptr = hash_segment_start;  
  void *phdr_entry_ptr;
  uint32 phdr_flags;
  
  /* For all program headers */
  for(index = 0; index < phdr_num; index++)
  {
    if (elf_format == ELF_CLASS_64)
    {
      phdr_entry_ptr = &phdr_array[index];
      phdr_flags = phdr_array[index].p_flags;
    }
    else
  {
      phdr_entry_ptr = &phdr_array_32[index];
      phdr_flags = phdr_array_32[index].p_flags;
    }
	
    /* Load the corresponding data segment except hash segment
      It should be already loaded by this time 
    */
    if(MI_PBT_SEGMENT_TYPE_VALUE(phdr_flags) != MI_PBT_HASH_SEGMENT)
    {  
      /*Load segment in chunks and update loaded image size*/
      boot_elf_load_seg_in_chunks(phdr_entry_ptr);
    }

    /* verify segment hash */
    boot_seg_verification(index, &curr_hash_ptr);
    
    status = boot_is_auth_enabled(&is_auth_enabled);
    BL_VERIFY((status == E_SECBOOT_HW_SUCCESS), (uint16)status|BL_ERROR_GROUP_SECURITY);

    if(is_auth_enabled)
    {
     /* Set total length of loaded segments after hash verification when
        MSA is disabled and SBL is authenticating. mba will xpu lock
        segments as soon as length is updated.
      */
      boot_rmb_set_pmi_code_length(total_elf_segments_size);
    }    
    
  }
}

/*===========================================================================
**  Function :  boot_elf_load_and_verify_image
** ==========================================================================
*/
/*!
* 
* @brief
*   This function loads all the data segments corresponding to the global
*   program header array. After each segment is loaded into memory, this 
*   function will also verify the data segments with their respective hashes.
*
* @param[in]
*   elf_segment_whitelist - Pointer to the whitelist table
*                           specifying segment regions to load.
*                           NULL means all segments are loaded.
*
* @par Dependencies
*   boot_elf_load_init_hash_segment() must be called
* 
* @retval
*   None
*/
static void
boot_elf_load_and_verify_image(whitelst_tbl_entry_type * elf_segment_whitelist,
                               uint32 image_id, 
                               boot_boolean auth)
{
  uint32 index = 0;
  void *phdr_entry_ptr;
  bl_error_boot_type status;

  /* For all program headers */
  for(index = 0; index < phdr_num; index++)
  {
    phdr_entry_ptr = (elf_format == ELF_CLASS_64) ? (void *)&phdr_array[index] : (void *)&phdr_array_32[index];
	
    /* If elf_segment_whitelist was passed then only load segments that fall
       completely inside one of the whitelist regions. */
    if (elf_segment_whitelist &&
        !boot_elf_segment_in_whitelist(phdr_entry_ptr,
                                       elf_segment_whitelist))
    {
      continue;
    }

    boot_elf_load_seg(phdr_entry_ptr);
  }

  if(auth == TRUE )
  {
    /* Compute hash over full data segment */
     status= boot_auth_compute_verify_hash(image_id);
     BL_VERIFY(BL_ERR_NONE == status, (uint16)status|BL_ERROR_GROUP_SECURITY);
   }
}

/*===========================================================================
**  Function :  boot_elf_image_cache_flush
** ==========================================================================
*/
/*!
* 
* @brief
*   This function flushes the cache for image segments being loaded. 
*
* @param[in]
*   elf_segment_whitelist - Pointer to the whitelist table specifying segment
*                          regions to load.  NULL means all segments are loaded.
*
* @par Dependencies
*   Must run before  boot_elf_load_deinit
* 
*/
static void boot_elf_image_cache_flush(whitelst_tbl_entry_type * elf_segment_whitelist)
{
  uint32 index;
  void *phdr, *dest_addr;
  uintnt phdr_memsz;
  
  /*Iterate over program header table and flush loadable segments */
  for(index = 0; index < phdr_num; index++)
  {
    phdr = (elf_format == ELF_CLASS_64) ? (void *)&phdr_array[index] : (void *)&phdr_array_32[index];  
    phdr_memsz = (elf_format == ELF_CLASS_64) ? ((Elf64_Phdr *)phdr)->p_memsz : ((Elf32_Phdr *)phdr)->p_memsz;
    dest_addr = (elf_format == ELF_CLASS_64) ?  (void *)((Elf64_Phdr *)phdr)->p_paddr 
                : (void *)(uintnt)((Elf32_Phdr *)phdr)->p_paddr;
	
    /* If elf_segment_whitelist was passed then only load segments that fall
       completely inside one of the whitelist regions. */
    if (elf_segment_whitelist &&
        !boot_elf_segment_in_whitelist(phdr, elf_segment_whitelist))
    {
      continue;
    }

    /* Flush the cache for loaded image. Needed to ensure decrypted data
      returned by UIE driver (if enabled) is written to destination memory */
    if(boot_sec_img_auth_is_valid_segment(elf_format, phdr))
    {
      dcache_flush_region(dest_addr, phdr_memsz);
    }
  }  
}

/*===========================================================================
**  Function :  boot_elf_load_deinit
** ==========================================================================
*/
/*!
* 
* @brief
*   This function deinitializes the necessary variables when ELF loading is 
*   complete. The flash translation interface is closed, and reset to NULL
*   so that it may be re-used again by the module for a new target image. 
*
* @par Dependencies
*   None 
* 
*/
void boot_elf_load_deinit(void)
{
  /* reset the flag tracking hash table authentication result for current elf */
  boot_set_hash_table_auth_result(FALSE);

  /* reset the flag tracking ehdr+phdr hash check for current elf */
  boot_elf_is_ehdr_phdr_hash_success = FALSE;

  /* Release the translation layer resource as long as the pointer is not null.*/
  if (trans_if != NULL)
  {
    boot_flash_dev_close_image(&trans_if);

    /* Assign trans_if to NULL to re-initialize interface */
    trans_if = NULL;
  }
}

/*===========================================================================
**  Function :  boot_get_elf_entry_address
** ==========================================================================
*/
/*!
* 
* @brief
*   This function returns the value of the elf header entry pointer. 
*
* @par Dependencies
*   None 
* 
*/
uint64 boot_get_elf_entry_address(void)
{
   return (elf_format == ELF_CLASS_64) ? ehdr.e_entry : ehdr_32.e_entry;
}

/*===========================================================================
**  Function :  boot_get_elf_eident_class
** ==========================================================================
*/
/*!
* 
* @brief
*   This function returns the value of the elf e_ident class value.
*   1 = 32 bit elf, 2 = 64 bit elf according to ELF spec.
*
* @par Dependencies
*   None 
* 
*/
uint32 boot_get_elf_eident_class(void)
{
   return elf_format;
}

/*===========================================================================
**  Function :  boot_get_elf_header
** ==========================================================================
*/
/*!
* 
* @brief
*   This function returns the address of elf header. 
*
* @par Dependencies
*   None 
* 
*/
uint64 boot_get_elf_header(void)
{
   return (elf_format == ELF_CLASS_64) ? (uint64)&ehdr : (uint64)&ehdr_32;
}

/*===========================================================================
**  Function :  boot_set_elf_and_prog_header
** ==========================================================================
*/
/*!
* 
* @brief
*  This function sets the global elf header and program header variables 
*  with provided data. 
*
* @param[in]
*   elf_hdr_ptr - Pointer to elf header
*
* @param[in]
*   prog_hdr_ptr - Pointer to Program header
*
* @par Dependencies
*   Elf header global struct is assumed to be zero initialized before
*  invoking the api. 
* 
* @retval
*   Boolean TRUE if success, FALSE otherwise
*/
boolean boot_set_elf_and_prog_header(void *elf_hdr_ptr, void *prog_hdr_ptr)
{
  uint32 phdr_array_size = 0;
  boolean status = FALSE;
  
  do
  {  
    if(NULL == elf_hdr_ptr || NULL == prog_hdr_ptr)
    { 
      break;
    }
    
    /*Copy over elf header to local struct */
    qmemcpy(&ehdr, elf_hdr_ptr, sizeof(Elf64_Ehdr));
  
    /* Check the EI_CLASS to see if it's 32bit or 64bit ELF 
     * Note the e_ident format is the same in both 32bit and 64bit
    */
    elf_format = ((Elf64_Ehdr *)elf_hdr_ptr)->e_ident[ELFINFO_CLASS_INDEX];

    if (elf_format == ELF_CLASS_64)
    {
      ehdr_ptr =&ehdr;
      ehdr_size = sizeof(ehdr);

      /*Copy over 64 bit elf header to local struct and clear ehdr_32*/
      qmemcpy(&ehdr, elf_hdr_ptr, sizeof(Elf64_Ehdr));
      qmemset(&ehdr_32, 0, sizeof(ehdr_32));
      
      phdr_size = ehdr.e_phentsize;	
      phdr_num = ehdr.e_phnum;
      phdr_array_ptr = phdr_array;
    }
    else
    {
      ehdr_ptr =&ehdr_32;
      ehdr_size = sizeof(ehdr_32);
      
      /* It's 32bit ELF so update ehdr_32 and clear ehdr */
      qmemcpy(&ehdr_32, elf_hdr_ptr, ehdr_size);
      qmemset(&ehdr, 0, sizeof(Elf64_Ehdr));
  	
      phdr_size = ehdr_32.e_phentsize;
      phdr_num = ehdr_32.e_phnum;
      phdr_array_ptr = phdr_array_32;
    }
    
    /*Copy over Program header to local struct */
    phdr_array_size = phdr_num * phdr_size;
    qmemcpy(phdr_array_ptr, prog_hdr_ptr, phdr_array_size);
  
    dcache_flush_region((void *)ehdr_ptr, sizeof(Elf64_Ehdr)); 
    dcache_flush_region((void *)phdr_array_ptr, phdr_array_size);            

    status = TRUE;
  }while(0);
  
  return status;
}

/*===========================================================================
**  Function :  boot_get_program_header
** ==========================================================================
*/
/*!
* 
* @brief
*   This function returns the address of program header. 
*
* @par Dependencies
*   None 
* 
*/
uint64 boot_get_program_header(void)
{
   return (elf_format == ELF_CLASS_64) ? (uint64)phdr_array : (uint64)phdr_array_32;
}

/*===========================================================================
**  Function :  boot_elf_populate_meta_info
** ==========================================================================
*/
/*!
* 
* @brief
*  This function will populate a buffer with the meta data for the current
*  elf image being processed. Meta data format:
*  elf header followed by program header table followed by
*  hash segment  
*
* @param[in] 
*  None
*
* @param[out] 
*  None
*
* @par Dependencies
*   Must be called only after boot_elf_load_init_hash_segment.
*   If the buffer lies in DDR, api must be called after DDR init.
*/
static void boot_elf_populate_meta_info(void)
{
  uint8 *meta_buff_ptr = NULL;

  if(elf_ssa_enabled == TRUE)
  { 
   /*Proceed only if subsytem expects the meta data */
    /*Get pointer to meta buffer */
    meta_buff_ptr = boot_rmb_get_meta_info_buffer();

    BL_VERIFY((NULL !=  meta_buff_ptr), BL_ERR_NULL_PTR_PASSED|BL_ERROR_GROUP_BOOT);

    /* zi init max possible meta_buff_ptr with ehdr, phdr, 
       hash-segment_buffer max sizes */
    qmemset((uint8*)meta_buff_ptr,
            0,
            (ehdr_size + 
             (phdr_size * MI_PBT_MAX_SEGMENTS) +
             boot_get_hash_buffer_size()));

    /* Copy elf header to buffer */
    qmemcpy(meta_buff_ptr, ehdr_ptr, ehdr_size);

    /* Update buffer to point to next free location */
    meta_buff_ptr = meta_buff_ptr + ehdr_size;

    /* Copy program header table to buffer */
    qmemcpy(meta_buff_ptr, phdr_array_ptr,
                   (phdr_size * phdr_num));

    /* Update buffer to point to next free location */
    meta_buff_ptr = meta_buff_ptr + 
                    (phdr_size * phdr_num);

    /* Copy hash segment to buffer*/
    qmemcpy(meta_buff_ptr, (const void *) hash_segment_base_addr,
            hash_segment_length);

    /*Signal subsystem authenticator image to retrieve meta info buffer */
    boot_rmb_populate_meta_info();
  }
}

/*===========================================================================
**  Function :  boot_elf_ssa_load_image_and_metadata
** ==========================================================================
*/
/*!
* 
* @brief
*  This function will populate a buffer with the meta data for the current
*  elf image being processed. It then loads the elf image and signals mba
*  via rmb registers, to process metadata .
*
* @param[in] 
*  None
*
* @param[out] 
*  None
*
* @par Dependencies
*   Must be called only after boot_elf_load_init_hash_segment.
*   If the buffer lies in DDR, api must be called after DDR init.
*/
static void boot_elf_ssa_load_image_and_metadata(void)
{
  uint32 ehdr_entry = (elf_format == ELF_CLASS_64) ? ehdr.e_entry : ehdr_32.e_entry;

  /* Initialize total segments size of elf to be loaded */
  total_elf_segments_size = 0;

  /* Get meta information for the elf to be loaded. send it to subsystem
    image expecting it (eg MBA)
  */
  boot_elf_populate_meta_info();

  /* Set the image start address. It is assumed the elf segments are in
    increasing order and first segment start address is lowest address
  */
  boot_rmb_set_pmi_code_start_addr( (uint32 *)&ehdr_entry);
  
  /*Signal MBA image to start processing PMI elf segments and
    jump to elf
  */
  boot_rmb_set_pmi_elf_load_ready();

  /* Load all data segments into memory for segmented authentication
  */ 
  boot_elf_load_and_verify_image_in_chunks();
    
}

/*===========================================================================
**  Function :  boot_elf_open_and_parse_elf_header
** ==========================================================================
*/
/*!
* 
* @brief
*   This function opens the elf header in the partition and parses the header
*
* @param[in] partition_id - Pointer to the partition ID information
* @param[in] header_ptr   - Pointer to the QC header buffer that must be cleared
*
* @par Dependencies
*   None 
* 
* @retval
*   None
* 
*/
static void boot_elf_open_and_parse_elf_header(uint8 * partition_id,
                                               mi_boot_image_header_type * header_ptr)
{
  boolean success = FALSE;
  
  // set the elf header, prg header whietlist and add entry for hash buffer region
  BL_VERIFY(boot_set_active_hdr_meta_whitelist_table(elf_hdr_meta_whitelist),BL_ERR_IMG_SECURITY_FAIL|BL_ERROR_GROUP_BOOT);
  BL_VERIFY(boot_add_entry_to_hdr_meta_whtlst((uint64)(header_ptr),(uint64)(sizeof(mi_boot_image_header_type))),
  BL_ERR_IMG_SECURITY_FAIL|BL_ERROR_GROUP_BOOT);
  // hash-table is in the temporary RPM memory pre-ddr-init then it gets moved to DDR
  // so query for hash-table pointer and then add it to the hdr_meta_whitelist
  BL_VERIFY(boot_add_entry_to_hdr_meta_whtlst((uint64)(boot_get_hash_segment_buffer()),(uint64)(HASH_SEGMENT_SIZE)),
  BL_ERR_IMG_SECURITY_FAIL|BL_ERROR_GROUP_BOOT);

  /* re-initialize all elf-hdr, prog-hdr, hash-table buffers to 0 */
  boot_elf_ziinit_structures(header_ptr);

  /* Configure the target image using custom partition ID information */ 
  boot_flash_configure_target_image(partition_id);

  /* Open the translation interface. 
     For each ELF image to be loaded, the translation interface must be opened once and only 
     used for that particular partition ID/target image. When the loading process is finished, 
     the translation interface will be released and reset to NULL. This allows for subsequent 
     target ELF images to re-open and use the translation interface when they are loaded. */
  if (trans_if == NULL)
  {
    trans_if = boot_flash_dev_open_image(GEN_IMG);
  }
  
  BL_VERIFY( trans_if != NULL, BL_ERR_NULL_PTR_PASSED|BL_ERROR_GROUP_BOOT );

                               
  /* Copy the ELF header from flash into our local structure */
  success = boot_flash_trans_read( trans_if,
                                   &ehdr,
                                   0,
                                   sizeof(Elf64_Ehdr),
                                   HDR_META_INFO_TYPE);
  BL_VERIFY( success, BL_ERR_FAILED_TO_COPY_IMG_HDR|BL_ERROR_GROUP_BOOT );

  
  dcache_flush_region(&ehdr, sizeof(Elf64_Ehdr));                               
  
  /* Check the EI_CLASS to see if it's 32bit or 64bit ELF 
    * Note the e_ident format is the same in both 32bit and 64bit
    */
  elf_format = ehdr.e_ident[ELFINFO_CLASS_INDEX];

  if (elf_format == ELF_CLASS_64)
  {
    ehdr_ptr =&ehdr;
    ehdr_size = sizeof(ehdr);
	
    phdr_size = ehdr.e_phentsize;	
    phdr_num = ehdr.e_phnum;
    phdr_array_ptr = phdr_array;
  }
  else
  {
    ehdr_ptr =&ehdr_32;
    ehdr_size = sizeof(ehdr_32);
    /* It's 32bit ELF so update ehdr_32 and clear ehdr */
    qmemcpy(&ehdr_32, &ehdr, ehdr_size);
    qmemset(&ehdr, 0, sizeof(Elf64_Ehdr));
	
    phdr_size = ehdr_32.e_phentsize;
    phdr_num = ehdr_32.e_phnum;
    phdr_array_ptr = phdr_array_32;
  }
}
 
/*===========================================================================
                        PUBLIC FUNCTION DEFINITIONS
=============================================================================*/

/*===========================================================================
**  Function :  boot_elf_verify_image_header
** ==========================================================================
*/
/*!
* 
* @brief
*   This function validates the elf header in the partition 
*
* @param[in] partition_id - Pointer to the partition ID information
* @param[in] header_ptr   - Pointer to the QC header buffer that must be cleared
*
* @par Dependencies
*   None 
* 
* @retval
*   Boolean TRUE is valid header, FALSE otherwise
* 
*/
boolean boot_elf_verify_image_header(uint8 * partition_id,
                                     mi_boot_image_header_type * header_ptr)
{
  boolean success = FALSE;
  bl_error_boot_type sec_img_success = BL_ERR_IMG_SECURITY_FAIL; 
  
  BL_VERIFY( partition_id != NULL, BL_ERR_NULL_PTR_PASSED|BL_ERROR_GROUP_BOOT );

  boot_elf_open_and_parse_elf_header(partition_id,
                                     header_ptr);
	
  /* Verify the ELF Header */
  if (elf_format == ELF_CLASS_32)
     sec_img_success = boot_sec_img_auth_validate_elf(elf_format, (const void *)&ehdr_32);
  else
  {
    elf_format = ELF_CLASS_64;
    sec_img_success = boot_sec_img_auth_validate_elf(elf_format, (const void *)&ehdr);
  }
  
  if(sec_img_success == SMC_SUCCESS)
  {
    success = TRUE;
  }

  /* Release the translation layer resource */
  boot_flash_dev_close_image( &trans_if );
 
  return success;
}

/*===========================================================================
**  Function :  boot_elf_load_init_hash_segment
** ==========================================================================
*/
/*!
* 
* @brief
*   This function initializes all global variables within the module which are
*   necessary to perform ELF loading. This function will also load the 
*   hash data segment into memory, read in the QC header, and assign 
*   hash_segment_start to point to the start of the hash entries. 
*
* @param[in] partition_id - Pointer to the partition ID information
* @param[out] header_ptr - Pointer to the QC header that is read from the image
*
* @par Dependencies
*   None 
* 
* @retval
*   None
* 
*/
void boot_elf_load_init_hash_segment(uint8 * partition_id, mi_boot_image_header_type *header_ptr)
{
  boolean success = FALSE;
  uint32 index = 0;  
  void *phdr_entry_ptr;
  uint16 phdr_array_size;
  uint32 phdr_flags;
  uint64 ehdr_phoff, phdr_offset, phdr_memsz;
  bl_error_boot_type sec_img_success = BL_ERR_IMG_SECURITY_FAIL;

  BL_VERIFY( partition_id != NULL && header_ptr != NULL, BL_ERR_NULL_PTR_PASSED|BL_ERROR_GROUP_BOOT );

 boot_elf_open_and_parse_elf_header(partition_id,
                                    header_ptr);

  /* Verify the ELF Header */
  if (elf_format == ELF_CLASS_32)
    sec_img_success = boot_sec_img_auth_validate_elf(elf_format, (const void *) &ehdr_32);
  else
  {
    elf_format = ELF_CLASS_64;
    sec_img_success = boot_sec_img_auth_validate_elf(elf_format, (const void *) &ehdr);
  }

  BL_VERIFY( (sec_img_success == SMC_SUCCESS),
             (uint16)sec_img_success|BL_ERROR_GROUP_SECIMG );

  ehdr_phoff = (elf_format == ELF_CLASS_64) ? ehdr.e_phoff : ehdr_32.e_phoff;

  phdr_array_size = phdr_num * phdr_size;

    /* Load the program headers */
  success = boot_flash_trans_read( trans_if,
                                   phdr_array_ptr,
                                   (uint32)ehdr_phoff,
                                   phdr_array_size,
                                   HDR_META_INFO_TYPE);
  BL_VERIFY( success, BL_ERR_FAILED_TO_COPY_IMG_HDR|BL_ERROR_GROUP_BOOT );

  dcache_flush_region(phdr_array_ptr, phdr_array_size); 

  /* Locate hash segment */
  for(index = 0; index < phdr_num; index++)
  {
    if (elf_format == ELF_CLASS_64)
    {
      phdr_entry_ptr = &phdr_array[index];
      phdr_flags = phdr_array[index].p_flags;
      phdr_offset = phdr_array[index].p_offset;
      phdr_memsz = phdr_array[index].p_memsz;
    }
    else
  {
      phdr_entry_ptr = &phdr_array_32[index];
      phdr_flags = phdr_array_32[index].p_flags;
      phdr_offset = phdr_array_32[index].p_offset;
      phdr_memsz = phdr_array_32[index].p_memsz;
    }
	
    /* Check for the hash segment program header to parse QC header */
    if( MI_PBT_SEGMENT_TYPE_VALUE(phdr_flags) == MI_PBT_HASH_SEGMENT)
    {
  
      /* Read in the QC header once hash segment is found */
      success = boot_flash_trans_read( trans_if,
                                       header_ptr,
                                       (uint32)phdr_offset,
                                       sizeof(*header_ptr),
                                       HDR_META_INFO_TYPE);
      BL_VERIFY( success, BL_ERR_ELF_HDR_READ_FAIL|BL_ERROR_GROUP_BOOT );

        
      /* Verify Buffer to load hash has sufficient space */
      BL_VERIFY( (phdr_memsz <= HASH_SEGMENT_SIZE),
                  BL_ERR_BUFFER_SIZE_TOO_SMALL|BL_ERROR_GROUP_BOOT );
      
      /* Get buffer to load hash segment */
      hash_segment_base_ptr = boot_get_hash_segment_buffer();
      
     
      /* Load full hash segment into memory */
      boot_elf_load_seg_generic(phdr_entry_ptr, hash_segment_base_ptr,HDR_META_INFO_TYPE);

      
      /* After hash segment is loaded into memory, assign start address of hash entries */
      hash_segment_start = (uint8 *)(hash_segment_base_ptr + sizeof(*header_ptr));
     
      if(elf_ssa_enabled == TRUE)
      {
        /*Store hash segment base addr and length */
         hash_segment_base_addr = (uint8 *)hash_segment_base_ptr;
         hash_segment_length  = phdr_memsz;
      }
      
      /* Exit loop once hash segment is found and processed */ 
      break;
    } 
  }
}

/*===========================================================================
**  Function :  boot_load_elf_image
** ==========================================================================
*/
/*!
* 
* @brief
*   This function will load an ELF formatted image from the requested 
*   partition_id. The QC header located at the beginning of the hash segment
*   will also be loaded into the location pointed to by header_ptr
*
* @param[in]
*  elf_segment_whitelist - Pointer to the whitelist table specifying segment
*                          regions to load.  NULL means all segments are loaded.
*
*  image_id              - ID of the image to be loaded
*
* @par Dependencies
*   BOOT_FEATURE_LOAD_ELF must be defined 
* 
* @retval
*   None
* 
*/
void boot_load_elf_image(whitelst_tbl_entry_type * elf_segment_whitelist,
                         uint32 image_id, 
                         boot_boolean auth )
{
  /* Open the translation interface if Not already opened. (Usually its opened when elf header
     gets loaded from storage device. However in case of image type CONFIG_IMG_ELF_SEG
     elf header, prog header, hash table are already provided and hence trans_if needs to be
     initialized at this point)
     
     For each ELF image to be loaded, the translation interface must be opened once and only 
     used for that particular partition ID/target image. When the loading process is finished, 
     the translation interface will be released and reset to NULL. This allows for subsequent 
     target ELF images to re-open and use the translation interface when they are loaded. 
  */
  if (trans_if == NULL)
  {
    trans_if = boot_flash_dev_open_image(GEN_IMG);
  }

  if(elf_ssa_enabled == TRUE)
  {
    boot_elf_ssa_load_image_and_metadata();
  }
  else
  {
    /* Load all data segments into memory and verify their hashes with the hash segment entries */ 
    boot_elf_load_and_verify_image(elf_segment_whitelist, image_id, auth);
  }
  
  /* Close the image partition and release the flash translation interface */
  boot_elf_load_deinit();
}

/*===========================================================================
**  Function :  boot_elf_set_auth_and_ssa_enable
** ==========================================================================
*/
/*!
* 
* @brief
*   This function will set/reset Flag to Turn on/Turn off authentication and
*   Subsystem image loading mode for elf images.
*
* @param[in] 
*  boolean auth_from_cfg_table   - TRUE if config table entry for the image
*                                  enables authentication else FALSE.
*  boolean ssa_value             - TRUE if Subsystem image loading mode to be
*                                  turned on else FALSE
*
* @param[out] 
*  None
*
* @par Dependencies
*  None
* 
* @retval
*   None
* 
*/
void boot_elf_set_auth_and_ssa_enable
( 
  boolean auth_from_cfg_table,
  boolean ssa_value
)
{
  elf_cfg_table_auth = auth_from_cfg_table;
  elf_ssa_enabled = ssa_value;
}


/*===========================================================================
**  Function :  boot_elf_segment_in_whitelist
** ==========================================================================
*/
/*!
* 
* @brief
*   This function will set/reset Flag to Turn on/Turn off authentication and
*   Subsystem image loading mode for elf images.
*
* @param[in] 
*  boolean auth_from_cfg_table   - TRUE if config table entry for the image
*                                  enables authentication else FALSE.
*  boolean ssa_value             - TRUE if Subsystem image loading mode to be
*                                  turned on else FALSE
*
* @param[out] 
*  None
*
* @par Dependencies
*  None
* 
* @retval
*   None
* 
*/
boolean
boot_elf_segment_in_whitelist(void * phdr_entry_ptr,
                              whitelst_tbl_entry_type * elf_segment_whitelist)
{
  uint64 segment_base_address = 0;
  uint64 segment_memory_size = 0;
  uint64 segment_top_address = 0;
  uint64 whitelist_top_address = 0;
  boolean result = FALSE;


  /* If program header pointer is not valid return false. */
  if (phdr_entry_ptr == NULL)
  {
    return FALSE;
  }

  /* Assign base address and memory size based on elf type. */
  switch (elf_format)
  {
  case ELF_CLASS_64:
    segment_base_address = ((Elf64_Phdr *)phdr_entry_ptr)->p_paddr;
    segment_memory_size = ((Elf64_Phdr *)phdr_entry_ptr)->p_memsz;
    break;

  case ELF_CLASS_32:
    segment_base_address = ((Elf32_Phdr *)phdr_entry_ptr)->p_paddr;
    segment_memory_size = ((Elf32_Phdr *)phdr_entry_ptr)->p_memsz;
    break;
  }

 
  /* Ensure segment does not overflow the memory map */
  BL_VERIFY((MAX_ADDRESS - segment_base_address) > segment_memory_size,
            BL_ERR_INTEGER_OVERFLOW|BL_ERROR_GROUP_BOOT);

 
  /* Determine if segment completely fits into any of the whitelist regions */
  while (elf_segment_whitelist->start_addr &&
         elf_segment_whitelist->size)
  {
    /* Ensure whitelist region does not overflow the memory map */
    BL_VERIFY((MAX_ADDRESS - elf_segment_whitelist->start_addr) >
              elf_segment_whitelist->size,
              BL_ERR_INTEGER_OVERFLOW|BL_ERROR_GROUP_BOOT);

    /* Use temp variables to keep the code clean. */
    segment_top_address = segment_base_address + segment_memory_size;
    whitelist_top_address = elf_segment_whitelist->start_addr +
                            elf_segment_whitelist->size;

    if ((segment_base_address >= elf_segment_whitelist->start_addr) &&
        (segment_top_address <= whitelist_top_address))
    {
      result = TRUE;
      break;
    }

    elf_segment_whitelist++;
  }

  return result;
}

/*===========================================================================
**  Function :  boot_get_elf_hash_segment_size
** ==========================================================================
*/
/*!
* 
* @brief
*   This function parses the input program header table to locate hash segment
*   and returns its size in memory.
*
* @param[in] 
*  void *elf_hdr                - Pointer to elf header
*  void *prog_hdr               - Pointer to Program header table
*
*
* @param[out] 
*  uintnt *hash_segment_size      - Size of hash segment if found
*
* @par Dependencies
*  It is assumed the user shall be validating and passing correct
*  elf header and program header table.
* 
* @retval
*   boolean     TRUE if hash segment found, else FALSE
* 
*/
boolean boot_get_elf_hash_segment_size
(
  void *elf_hdr,
  void *prog_hdr,
  uintnt *hash_segment_size
)
{
  boolean status = FALSE;
  uint8 elf_format;
  uintnt phdr_num = 0, index = 0;
  uint32 phdr_flags;
  uint64 phdr_memsz =0;
  bl_error_boot_type sec_img_success = BL_ERR_IMG_SECURITY_FAIL; 
  
  BL_VERIFY((NULL !=  elf_hdr) && (NULL !=  prog_hdr) &&
            (NULL !=  hash_segment_size),
             BL_ERR_NULL_PTR_PASSED|BL_ERROR_GROUP_BOOT);  
             
  /* Check the EI_CLASS to see if it's 32bit or 64bit ELF 
   * Note the e_ident format is the same in both 32bit and 64bit
   */
  elf_format = ((Elf64_Ehdr *)elf_hdr)->e_ident[ELFINFO_CLASS_INDEX];
  
  /* Verify elf header*/
  sec_img_success = boot_sec_img_auth_validate_elf(elf_format, (const void *)elf_hdr);
  BL_VERIFY((sec_img_success == SMC_SUCCESS),
            (uint16)sec_img_success|BL_ERROR_GROUP_SECIMG);
  
  if (elf_format == ELF_CLASS_64)
  {
    phdr_num = ((Elf64_Ehdr *)elf_hdr)->e_phnum;
  }
  else
  {
    phdr_num = ((Elf32_Ehdr *)elf_hdr)->e_phnum;
  }

  /* Locate hash segment */
  for(index = 0; index < phdr_num; index++)
  {
    if (elf_format == ELF_CLASS_64)
    {
      phdr_flags = (((Elf64_Phdr *)prog_hdr)[index]).p_flags;
      phdr_memsz = (((Elf64_Phdr *)prog_hdr)[index]).p_memsz;
    }
    else
    {
      phdr_flags = (((Elf32_Phdr *)prog_hdr)[index]).p_flags;
      phdr_memsz = (((Elf32_Phdr *)prog_hdr)[index]).p_memsz;
    }
	
    /* Check for the hash segment program header to parse QC header */
    if( MI_PBT_SEGMENT_TYPE_VALUE(phdr_flags) == MI_PBT_HASH_SEGMENT)
    {
       *hash_segment_size = (uintnt)phdr_memsz;
       status = TRUE;
       break;
    }
  }
  return status;
}

/*===========================================================================
**  Function :  boot_get_elf_prog_header_count
** ==========================================================================
*/
/*!
* 
* @brief
*   This function parses the input elf header table to get
*   program header table entry count
*
* @param[in] 
*  void *elf_hdr                - Pointer to elf header
*
* @param[out] 
*  uintnt *max_phnum            - Pointer to program header entry count
*
* @par Dependencies
*  It is assumed the user shall be validating and passing correct
*  elf header.
* 
* @retval
*   boolean     TRUE if hash segment found, else FALSE
* 
*/
boolean boot_get_elf_prog_header_count
(
  void *elf_hdr,
  uintnt *max_phnum
)
{
  boolean status = FALSE;
  uint8 elf_format;
  bl_error_boot_type sec_img_success = BL_ERR_IMG_SECURITY_FAIL;   
  
  BL_VERIFY((NULL !=  elf_hdr) && (NULL !=  max_phnum),
             BL_ERR_NULL_PTR_PASSED|BL_ERROR_GROUP_BOOT);  
             
  /* Check the EI_CLASS to see if it's 32bit or 64bit ELF 
   * Note the e_ident format is the same in both 32bit and 64bit
   */
  elf_format = ((Elf64_Ehdr *)elf_hdr)->e_ident[ELFINFO_CLASS_INDEX];
  
  /* Verify elf header*/
  sec_img_success = boot_sec_img_auth_validate_elf(elf_format, (const void *)elf_hdr);
  BL_VERIFY((sec_img_success == SMC_SUCCESS),
            (uint16)sec_img_success|BL_ERROR_GROUP_SECIMG);    

  *max_phnum =0;
  if (elf_format == ELF_CLASS_64)
  {
    *max_phnum = ((Elf64_Ehdr *)elf_hdr)->e_phnum;
    status = TRUE;
  }
  else
  {
    *max_phnum = ((Elf32_Ehdr *)elf_hdr)->e_phnum;
    status = TRUE;
  }

  return status;
}

/*===========================================================================
**  Function :  boot_get_elf_phdr_entry_info
** ==========================================================================
*/
/*!
* 
* @brief
*   This function returns segment details for requested prog header table entry
*
* @param[in] 
*  void *elf_hdr              - Pointer to elf header
*  void *prog_hdr             - Pointer to Program header table
*  uintnt index               - Index of the entry whose details are requested
*
*
* @param[out] 
*  uintnt *seg_file_size_ptr      - Segment file size
*  boolean *is_seg_loadable_ptr   - Flag indicating if the segment is loadable
*  uintnt *file_offset            - offset of segment data on storage
*  uintnt *physical_addr          - destination physical address of segment
*
* @par Dependencies
*  It is assumed the user shall be validating and passing correct
*  elf header.
* 
* @retval
*   boolean     TRUE if hash segment found, else FALSE
* 
*/
boolean boot_get_elf_phdr_entry_info
(
  void *elf_hdr,
  void *prog_hdr,
  uintnt index,
  uintnt *seg_file_size_ptr,
  boolean *is_seg_loadable_ptr,
  uintnt *file_offset,
  uintnt *physical_addr
)  
{
  boolean status = FALSE;
  uint8 elf_format;
  bl_error_boot_type sec_img_success = BL_ERR_IMG_SECURITY_FAIL;  
  void *prog_hdr_entry_addr =NULL;  
  
  BL_VERIFY((NULL != elf_hdr) && (NULL != prog_hdr) &&
            (NULL != seg_file_size_ptr) && (NULL != is_seg_loadable_ptr) &&
            (NULL != file_offset) && (NULL != physical_addr),
             BL_ERR_NULL_PTR_PASSED|BL_ERROR_GROUP_BOOT);  
             
  /* Check the EI_CLASS to see if it's 32bit or 64bit ELF 
   * Note the e_ident format is the same in both 32bit and 64bit
   */
  elf_format = ((Elf64_Ehdr *)elf_hdr)->e_ident[ELFINFO_CLASS_INDEX];
  
  /* Verify elf header*/
  sec_img_success = boot_sec_img_auth_validate_elf(elf_format, (const void *)elf_hdr);
  BL_VERIFY((sec_img_success == SMC_SUCCESS), 
            (uint16)sec_img_success|BL_ERROR_GROUP_SECIMG);
  
   prog_hdr_entry_addr =  (elf_format == ELF_CLASS_64) ? 
                         (void *)&(((Elf64_Phdr *)prog_hdr)[index]):
                         (void *)&(((Elf32_Phdr *)prog_hdr)[index]);
                         
  if(boot_sec_img_auth_is_valid_segment(elf_format, prog_hdr_entry_addr))
  {
    *is_seg_loadable_ptr = TRUE;
  }
  else
  {
    *is_seg_loadable_ptr = FALSE;  
  }

  *seg_file_size_ptr = (elf_format == ELF_CLASS_64) ? 
                         (((Elf64_Phdr *)prog_hdr)[index]).p_filesz :
                         (((Elf32_Phdr *)prog_hdr)[index]).p_filesz;
  
  *file_offset = (elf_format == ELF_CLASS_64) ?
                  (((Elf64_Phdr *)prog_hdr)[index]).p_offset :
                  (((Elf32_Phdr *)prog_hdr)[index]).p_offset;  
                  
  *physical_addr = (elf_format == ELF_CLASS_64) ?
                  (((Elf64_Phdr *)prog_hdr)[index]).p_paddr :
                  (((Elf32_Phdr *)prog_hdr)[index]).p_paddr;                    
                  
                  
  
  status = TRUE;

  return status;
}

/*===========================================================================
**  Function :  boot_get_elf_segment_data
** ==========================================================================
*/
/*!
* 
* @brief
*   This function loads data from storage
* 
* @param[in] 
*  uintnt file_offset          - Start of data to be loaded in partition from
*                                storage device
*  uintnt file_size            - Size of data to be loaded.
*
* @param[out] 
*   void *buffer              - pointer to buffer, loaded with data
* 
*       
* @par Dependencies
*  Partition is assumed to be opened for image loading with flash lib data struct
*  configured
*   
* @retval
*   xblconfig_status_type.   XBLCONFIG_ERR_NONE if no error else error value
* 
* @par Side Effects
*   None
* 
*/
boolean boot_get_elf_segment_data( uintnt file_offset, uintnt file_size, void *buffer) 
{
  boolean success = FALSE;
  
  /* Only process requests with length greater than zero */
  if( (file_size > 0) && (buffer != NULL) &&
      file_offset <= UINT32_MAX && file_size <= UINT32_MAX )
  {
 
    /* Copy the data segment flash into memory */
    success = boot_flash_trans_read( trans_if,
                                     buffer,
                                     (uint32)file_offset,
                                     (uint32)file_size,
                                     IMAGE_BODY_TYPE);
    BL_VERIFY( success, BL_ERR_FLASH_READ_FAIL|BL_ERROR_GROUP_BOOT );
    
    /* Flush the segment memory region after loading */
    dcache_flush_region(buffer, file_size);

  }
  
  return success;
}

