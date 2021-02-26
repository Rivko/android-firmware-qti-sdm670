/*=============================================================================

                             Boot Authenticator

GENERAL DESCRIPTION
  This module performs SBL binary image authenticating, utilizing the
  functions provided by the PBL fucntion pointers.

Copyright 2010-2016,2017 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/23/16   kpa     Update boot_auth_image_hashtable return status, code cleanup
11/09/16   digant  Make SMC for is_auth_enabled
04/11/16   bd      Move TPM Hash's to SMEM
05/25/15   kpa     Ensure secboot_verified_info_type data is propagaged 
                   to sbl qsee interface
01/05/14   kpa     Fix warnings for updated mi_boot_image_header_type
10/29/14   kpa     Support 64 bit memory map
11/26/14   ck      Changed FEATURE_BOOT_EXTERN_SECBOOT_COMPLETED logic to FEATURE_USE_PBL_SECBOOT_API
10/27/14   kedara  Added SAFE 1.0
09/03/14   ck      Removed MSA and MBA functions as they are not supported in HoneyBadger.
08/08/14   ck      Fixed warning regarding initialization of sbl_sec_handle
06/10/14   jz      Check auth_enabled befor calling PBL secboot driver,
                   fixed boot_is_auth_enabled to check return status in the caller
06/06/14   kedara  Update to support 64-bit MSM8996
04/01/14   ck      Updated pbl_secboot_verified_info_type to secboot_verified_info_type
                   as Bear PBL uses common secboot lib now.
12/10/13   ck      Replaced return value of boot_is_auth_enabled to void since secboot
                   status is checked inside of function
12/06/13   ck      Removed bl_shared_data_type parameter from boot_is_auth_enabled
08/06/13   aus     Added support for new secboot_verified_info structure
05/07/13   dh      In boot_secboot_ftbl_init, Copy the pbl secboot function 
                   pointers to sbl local function tables individually to avoid
                   mismatch between pbl secboot function table structure and
                   sbl secboot function table structure
04/29/12   kedara  Update boot_auth_load_mba_header to check secure boot status
04/15/12   kedara  Added boot_auth_load_mba_header ,boot_is_msa_enabled_in_hw.
02/13/13   dh      SBL keeps its own copy of secboot function table. Based on 
                   FEATURE_BOOT_EXTERN_SECBOOT_COMPLETED flag we copy over 
                   PBL secboot function table or local secboot lib function table.
12/06/12   dh      Change SHARED_IMEM_TPM_HASH_REGION_BASE to SHARED_IMEM_TPM_HASH_REGION_OFFSET
11/28/12   dh      Add roll back version protection logic 
11/19/12   dh      add boot_store_tpm_hash_block to store tpm hash to shared imem
11/08/12   kedara  Support loading hash segment to seperate buffer.
10/17/12   dh      Check return value of secboot_init
09/26/12   kedara  Added boot_get_sbl_auth_verified_info
09/21/12   kedara  Modified auth function to return a value instead of
                   entering error handler
04/12/12   dh      Rewrote boot_pbl_auth_image to use latest pbl secboot API
12/01/11   dh      ported Rollback prevention version check.
10/27/11   dh      Added boot_auth_init_qd_certs
10/18/11   dh      Added boot_get_ou_field_info
09/01/11   dh      remove boot_pbl_auth_sbl2_image
08/10/11   kpa     Moved api supporting QDST to boot_auth_qd_cert.c
06/28/11   kpa     Added BOOT_MSM_HW_ID define to be used in QDST.
02/11/11   dh      Added a size check for signature_size in boot_pbl_auth_image
01/28/11   dxiang  Define boot_auth_image() utilizing PBL Function Pointers
11/28/10   plc     Added support for Secure-Boot with Qualcomm Development 
                   certificates
10/01/10   plc     Initial revision.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include BOOT_PBL_H
#include "boot_authenticator.h"
#include "boot_sbl_if.h"
#include "boot_util.h"
#include "boot_logger.h"
#include "boot_target.h"
#include "secboot_hw.h"
#include "secboot.h"
#include "boot_error_if.h"
#include "boot_shared_imem_cookie.h"
#include "boot_smem.h"
#include "boot_extern_secboot_interface.h"
#include "boot_extern_sec_img_interface.h"
#include "boot_extern_smem_interface.h"
#include "boot_fastcall_tz.h"


/*=========================================================================== 
 
                                    GLOBALS
 
===========================================================================*/

static ALIGN(32)  secboot_image_info_type     sbl_auth_image_info;
static ALIGN(32)  secboot_verified_info_type  sbl_auth_verified_info;
static ALIGN(32)  sec_img_auth_verified_info_s  sbl_sec_img_auth_verified_info;

/*===========================================================================

                              FUNCTION DEFINITIONS

===========================================================================*/

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
secboot_hw_etype boot_is_auth_enabled(boot_boolean *is_auth_enabled)
{
  secboot_hw_etype sec_hw_err = E_SECBOOT_HW_SUCCESS;
  boot_tzt_result_regs_t result_regs;
  *is_auth_enabled = 0;

  boot_fastcall_tz( TZ_SECBOOT_STATUS_CMD,
                    TZ_SECBOOT_STATUS_PARAM_ID,
                    1,0,0,0,
                    &result_regs
                  );

  if(result_regs.reg[0]==1)
  {    
    *is_auth_enabled = 1;
  }
   	return sec_hw_err;
}


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
void boot_auth_load_header( mi_boot_image_header_type* image_header )
{
  /* Verify the image header structure */
  BL_VERIFY(( image_header != NULL ),
               BL_ERR_NULL_PTR_PASSED|BL_ERROR_GROUP_BOOT );

  qmemset((void *)&sbl_auth_image_info, 0, sizeof(sbl_auth_image_info));

  sbl_auth_image_info.header_ptr_1   = (uint8*)image_header;
  sbl_auth_image_info.header_len_1   = sizeof(*image_header);  
  sbl_auth_image_info.code_len_1     = image_header->code_size;
  sbl_auth_image_info.x509_chain_len = image_header->cert_chain_size;
  sbl_auth_image_info.signature_len  = image_header->signature_size;
  
}

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
bl_error_boot_type boot_auth_image(bl_shared_data_type *bl_shared_data, uint32 image_type)
{
  /* This api is deprecated. 
    TBD: All consumers replace this api use by boot_auth_image_hashtable 
  */
    return BL_ERR_IMG_SECURITY_FAIL;
}

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
  bl_shared_data_type *bl_shared_data
)
{
  return &sbl_auth_verified_info;
}

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
*  authentication routines. (Assumes sbl_auth_image_info is populated with
*  correct data)
* 
* @retval
*  None
* 
* @par Side Effects
*  None
* 
*/
void boot_update_auth_image_info( uint8* hash_buffer_base_addr )
{
  /* Verify the buffer pointer */
  BL_VERIFY( ( hash_buffer_base_addr != NULL ),
               BL_ERR_NULL_PTR_PASSED|BL_ERROR_GROUP_BOOT );
  
  sbl_auth_image_info.code_ptr_1     = hash_buffer_base_addr + 
                                           sizeof(mi_boot_image_header_type);
  
  sbl_auth_image_info.signature_ptr  = sbl_auth_image_info.code_ptr_1 +
                                       sbl_auth_image_info.code_len_1 ;
                                       
  sbl_auth_image_info.x509_chain_ptr = sbl_auth_image_info.signature_ptr + 
                                       sbl_auth_image_info.signature_len ;
    
}


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
  bl_shared_data_type *bl_shared_data, 
  secboot_verified_info_type *sbl1_auth_verified_info
)
{
#if (defined(SHARED_IMEM_TPM_HASH_REGION_SIZE) && defined(SHARED_IMEM_TPM_HASH_REGION_OFFSET))

  /* 
     Store the hash of the image last authenticated on to shared imem.
     Hash block pointer is updated to next available
     free location. Along with the hash, hash block also contains other info
     about image such as image address.
  */
    
  /* First uint32 represents how many blocks there are */
  uintnt tpm_hash_imem_base = SHARED_IMEM_TPM_HASH_REGION_OFFSET + (uintnt)boot_shared_imem_cookie_ptr;
  uint32 *num_of_hashes = NULL;
  uint16 remaining_block_length = 0;
  uintnt *tpm_hash_block_ptr = (uintnt *)bl_shared_data->
                               sbl_shared_data->
                               next_avail_tpm_hash_blk_ptr;
  uint32 tpm_hash_region_size = 0;
  static boot_boolean move_hash_post_smem_init = FALSE;
  boot_boolean smem_init_done = boot_smem_get_init_status();
  uintnt tpm_hash_base = 0;	

	BL_VERIFY(( tpm_hash_imem_base < (uintnt)tpm_hash_block_ptr ),
							BL_ERR_TPM_HASH_BLK_STR_FAIL|BL_ERROR_GROUP_BOOT );  
		
	if(smem_init_done == FALSE)
	{
		tpm_hash_base = tpm_hash_imem_base;
		tpm_hash_region_size = SHARED_IMEM_TPM_HASH_REGION_SIZE;
	}
	else if((smem_init_done == TRUE) && (move_hash_post_smem_init == FALSE)) 		
	{
		/* first call post SMEM init 
			 If smem_init_done is TRUE on very first call to this function,
			 then tpm_hash_block_ptr - tpm_hash_imem_base = 4(number of hashes) */       
		tpm_hash_region_size = SMEM_TPM_HASH_REGION_SIZE;
		tpm_hash_base = (uintnt)smem_alloc( SMEM_XBL_LOADER_CORE_INFO, tpm_hash_region_size);
		BL_VERIFY(( tpm_hash_base > 0 ), BL_ERR_NULL_PTR_PASSED|BL_ERROR_GROUP_BOOT );
    BL_VERIFY(( (uintnt)tpm_hash_region_size >  ((uintnt)tpm_hash_block_ptr - tpm_hash_imem_base)),
							BL_ERR_TPM_HASH_BLK_STR_FAIL|BL_ERROR_GROUP_BOOT );              

		qmemcpy((void *)tpm_hash_base, (void *)tpm_hash_imem_base, 
				(uintnt)tpm_hash_block_ptr - tpm_hash_imem_base);	
				
		tpm_hash_block_ptr = (uintnt *)(tpm_hash_base 
						+ ((uintnt)tpm_hash_block_ptr - tpm_hash_imem_base));
    BL_VERIFY(( (uintnt)tpm_hash_block_ptr >  tpm_hash_base ),
							BL_ERR_TPM_HASH_BLK_STR_FAIL|BL_ERROR_GROUP_BOOT );              

		move_hash_post_smem_init = TRUE;
	}
	else /*  post SMEM init */
	{
		tpm_hash_region_size = SMEM_TPM_HASH_REGION_SIZE;
		tpm_hash_base = (uintnt)smem_alloc( SMEM_XBL_LOADER_CORE_INFO, tpm_hash_region_size);
		BL_VERIFY(( tpm_hash_base > 0 ), BL_ERR_NULL_PTR_PASSED|BL_ERROR_GROUP_BOOT );
	}

	num_of_hashes = (uint32 *)tpm_hash_base;  

	/* Integer Overflow Check */
  BL_VERIFY((uintnt)tpm_hash_base < ( tpm_hash_base + (uintnt)tpm_hash_region_size ),
                BL_ERR_TPM_HASH_BLK_STR_FAIL|BL_ERROR_GROUP_BOOT );		
                
	/* Integer Underflow Check */
  BL_VERIFY((uintnt)tpm_hash_block_ptr < ( tpm_hash_base + (uintnt)tpm_hash_region_size ),
                BL_ERR_TPM_HASH_BLK_STR_FAIL|BL_ERROR_GROUP_BOOT );				
						
	/* Verify there is enough space remain to store one hash block */
	remaining_block_length = (tpm_hash_base + tpm_hash_region_size) - 
													(uintnt)tpm_hash_block_ptr;	

	BL_VERIFY(remaining_block_length >= sizeof(secboot_image_hash_info_type),
						BL_ERR_TPM_HASH_BLK_STR_FAIL|BL_ERROR_GROUP_BOOT);	

	/* Store hash to shared imem */
	qmemcpy(tpm_hash_block_ptr, 
					&sbl1_auth_verified_info->image_hash_info, 
					sizeof(secboot_image_hash_info_type));

	/* Update pointer to point to next available location */
	bl_shared_data->sbl_shared_data->next_avail_tpm_hash_blk_ptr = 
						(uint32 *)((uintnt)tpm_hash_block_ptr + sizeof(secboot_image_hash_info_type));
	*num_of_hashes += 1;
#endif  
  
}/*  boot_store_tpm_hash_block */

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
)
{
  bl_error_boot_type sec_img_auth_status = BL_ERR_IMG_SECURITY_FAIL;
	
  sec_img_auth_status = boot_sec_img_auth_verify_metadata(image_id,
                                                     sbl_elf_info,
                                                     sbl_white_list_param,
                                                     &sbl_sec_img_auth_verified_info);

  /* Copy the secboot_verified_info_type for the loaded image from sec img auth struct */
  qmemcpy(&sbl_auth_verified_info, &(sbl_sec_img_auth_verified_info.v_info),
          sizeof(secboot_verified_info_type));

  return sec_img_auth_status;
}
	
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
*                    ERROR VALUES - In case of failure.
* 
*/
bl_error_boot_type boot_auth_compute_verify_hash( uint32 image_id )
{
  bl_error_boot_type sec_img_auth_status = BL_ERR_IMG_SECURITY_FAIL;
    
  sec_img_auth_status = boot_sec_img_auth_hash_elf_segments(image_id,
                                                       &sbl_sec_img_auth_verified_info);

  /* Copy the secboot_verified_info_type for the loaded image from sec img auth struct */
  qmemcpy(&sbl_auth_verified_info, &(sbl_sec_img_auth_verified_info.v_info),
          sizeof(secboot_verified_info_type));
          
  return sec_img_auth_status;
}
