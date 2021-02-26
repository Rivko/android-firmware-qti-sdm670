/**
 * @file boot_config.c
 * @brief
 * Source file for boot configuration logic. For every entry in a particular 
 * configuration table, the boot configuration logic parses the data and executes 
 * appropriately. 
 *
 * The functions defined in this file are:
 *  boot_config_process_bl() 
 *  boot_config_process_entry(). 
 *
 * Please see comments for more details. 
 */

/* Copyright 2010-2017 by Qualcomm Technologies, Inc.  All Rights Reserved. */
/*==========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    
when       who     what, where, why
--------   ---     ---------------------------------------------------------
01/27/17   digants call api for setting img whitelist
11/23/16   kpa     update sec img auth api return types
05/20/16   bd      Store image HASH to IMEM/SMEM
04/29/16   kpa     skip hash check if image auth is disabled from config entry flag
10/22/15   kpa     Code cleanup, rename boot_prepare_and_handoff_to_deviceprogrammer_DDR
07/13/15   kpa     Update sbl qsee interface to be populated only if image is loaded.
07/01/15   as      Fetch segmented ELF's entry point from boot_config
05/26/15   kpa     pass config entry whitelist parameter to boot_populate_sbl_info
05/13/15   kpa     Store sbl qsee interface entry index for every image.
04/24/15   kpa     Added boot_prepare_and_handoff_to_deviceprogrammer_DDR
02/20/15   ck      Fixed bug where whitelist table needs to be set implicitly
02/03/15   wek     Don't load images for device programmer.
01/05/15   kedara  Fix warnings for updated mi_boot_image_header_type
10/27/14   kedara  Added SAFE 1.0
10/09/14   kedara  Fix XBL core segment start address in shared qsee interface
09/24/14   kedara  Updates to support loading XBL Core segment.
08/20/14   kedara  Fix compiler warnings.
08/05/14   ck      boot_config_process_entry is no longer static
07/18/14   ck      Removed Sahara / Flashless boot support
06/13/14   ck      Added CONFIG_IMG_ELF_SEG image type
06/03/14   ck      Added logic to report bytes read when loading elf image
04/10/14   jz      Updated to populate QSEE interface info for both flash/flashless
03/17/14   ck      Removed call to and function boot_config_update_tz_xpu as tz executes after SBL
02/04/14   ck      Removed exit_ptr assignment as it's not needed in Bear family.
12/16/13   ck      Replaced TZ fastcall with stub for future securemsm library call.
12/09/13   jz      Added 64bit elf loading support
12/09/13   ck      Removed boot_config_update_all_fuse_versions as this logic is now in QSEE
12/03/13   ck      Added logic to populate SBL - QSEE interface
10/18/13   dhaval  explicit hash-verify elf-header after hash-table auth
03/19/13   kedara  Fix target_img_partition_id to support uint32 values.
03/11/13   kedara  Enable boot config entries to configure xpu settings per image.
02/19/13   kedara  Added boot_config_update_tz_xpu.
02/14/13   kedara  Added boot_config_update_all_fuse_versions.
11/28/12   dh      Call boot_rollback_update_fuse_version after image is executed
                   to blow roll back protection fuse
11/08/12   kedara  Updates to enable hashing and authentication of elf's.
09/21/12   jz      Fixed redundant call to boot_elf_load_init_hash_segment
09/20/12   kedara  Modified the return value of the auth function
08/17/12   kedara  Support loading/auth of elf images in chunks (ssa).
07/31/12   dh      Klocwork fix
01/17/12   aus     Import whitelist table before loading the image
08/24/11   aus     Added support for sahara secure boot
07/06/11   aus     Clean - up 
06/27/11   aus     Added flashless boot support
05/27/11   kpa     Add boot logger messages
03/18/11   dxiang  Support ELF image loading 
03/04/11   dxiang  Support loading of images with custom partition ID information
01/21/11   dxiang  Initial Creation 
*/


/*=============================================================================
                               INCLUDE FILES
=============================================================================*/
#include <stdio.h>
#include "boot_config.h"
#include "boot_error_if.h"
#include "boot_loader.h"
#include "boot_authenticator.h"
#include "boot_sbl_shared.h"
#include "boot_sbl_if.h"
#include "boot_logger.h"
#include "boot_flash_dev_if.h"
#include "boot_hash_buffer.h"
#include "boot_elf_header.h"
#include "boot_sbl_qsee_interface.h"
#include "boot_util.h"
#include "boot_authenticator.h"
#include "boot_flash_target.h"
#include "boot_mc.h"
#include "boot_fastcall_tz.h"

/*============================================================================
                            FUNCTION DEFINITIONS
=============================================================================*/


/*===========================================================================
**  Function : boot_config_process_entry 
** ==========================================================================
*/
/*!
* 
* @brief
*   Function to process and execute boot code based on information from a 
*   single entry in the configuration table. The current boot configuration
*   logic includes the follow steps for each entry:
*
*   1. Execute pre_procs ~ These are any procedures that should run before
*      the particular image is loaded.
*   2. Check for boot_load_cancel ~ Configuration table entries may optionally
*      define a function to check if loading should be canceled. In this certain
*      scenario, the logic will stop all loading. 
*   3. Load/Authenticate ~ Based on the target image type, we load and authenticate
*      the target image if necessary.
*   4. Execute ~ If the configuration table entry has execute as TRUE, we run a specific
*      execute routine which is defined in the configuration. This happens immediately
*      after loading/authentication and should be mutually exclusive with jumping.
*   5. Execute post_procs ~ These are any procedures that should run after the target
*      image has been loaded/authenticated/executed.
*   6. Jump ~ If the configuration table entry has jump as TRUE, we run a specific
*      jumping routine which is defined in the configuration. This always happens after
*      the post_procs have finished and should be mutually exclusive with executing. 
* 
* @param[in] bl_shared_data - Pointer to the shared data structure
* @param[in] boot_config_entry - One entry in the configuration table
*     
* @par Dependencies
*   None
*   
* @retval
*   None
* 
* @par Side Effects
*   None
*/
void
boot_config_process_entry(bl_shared_data_type * bl_shared_data,
                          boot_configuration_table_entry * boot_config_entry)
{
  /*------------------------------------------------------------------
   Local Variables 
  ------------------------------------------------------------------*/
  mi_boot_image_header_type qc_image_header;
  uint32 config_start_time=0;
  sec_img_auth_error_type populate_sbl_info;
  bl_error_boot_type sec_img_auth_error_status = BL_ERR_IMG_SECURITY_FAIL;
  boot_images_entry * qsee_interface_image_entry = NULL;
  uint32 bytes_read = 0;
  sec_img_auth_elf_info_type sbl_elf_info;
  sec_img_auth_whitelist_area_param_t sbl_white_list_param;
  
  /*------------------------------------------------------------------
   Perform Pre-Procedures 
  ------------------------------------------------------------------*/
  BL_VERIFY(boot_config_entry != NULL, BL_ERR_NULL_PTR_PASSED|BL_ERROR_GROUP_BOOT );

  boot_do_procedures(bl_shared_data, boot_config_entry->pre_procs);

  /*------------------------------------------------------------------
   Preliminary check if loading should be canceled 
  ------------------------------------------------------------------*/
  if(boot_config_entry->boot_load_cancel != NULL)
  {
    if( boot_config_entry->boot_load_cancel(bl_shared_data) == TRUE )
    {
      boot_config_entry->load = FALSE;
    }
  }
  

  /*------------------------------------------------------------------
   Image Loading and Authentication
  ------------------------------------------------------------------*/
  /* Reset boot statistics flash byte counter. */
  boot_statistics_reset_flash_byte_counter();

  config_start_time = boot_log_get_time();
    /* Format string for boot log. */
    snprintf(bootlog_buffer, BOOT_LOG_TEMP_BUFFER_SIZE, "%s Image Loaded, Start",
    (char *)boot_config_entry->target_img_str);
    boot_log_message(bootlog_buffer);

  if (bl_shared_data->build_type == SBL_BUILD_DEVICEPROGRAMMER_DDR)
  {
    boot_config_entry->load = FALSE;
  }

  if(boot_config_entry->load == TRUE)
  {
    

    /* Set the image whitelist for whitelist based image loading */
    BL_VERIFY(boot_set_active_img_whitelist_table(boot_config_entry->whitelist_ptr),
              BL_ERR_IMG_SECURITY_FAIL|BL_ERROR_GROUP_SECURITY); // to do: make suitable error code
    
    /*------------------------------------------------------------------
     Get the next image entry from the QSEE Interface and fill out
     preliminary info.
     -----------------------------------------------------------------*/
    qsee_interface_image_entry =
      &(bl_shared_data->sbl_qsee_interface.boot_image_entry[bl_shared_data->sbl_qsee_interface.number_images]);
    
    qsee_interface_image_entry->image_id = boot_config_entry->target_img_sec_type;
    
    /*Store the index of sbl qsee interface struct for the current image being loaded */
    boot_config_entry->sbl_qsee_interface_index = bl_shared_data->sbl_qsee_interface.number_images;
    
    bl_shared_data->sbl_qsee_interface.number_images++;  
  
  
    switch(boot_config_entry->target_img_type)
    {
      /* QC Image Type no longer supported */

      /* ELF Image Type */
      case CONFIG_IMG_ELF:
     
        /* Enable subsys self auth (ssa) support if applicable. Set auth mode
           according to the config table entry */
        boot_elf_set_auth_and_ssa_enable( boot_config_entry->auth,
                                          boot_config_entry->boot_ssa_enabled);
        
        /* Target image partition ID pointer must be defined */
          BL_VERIFY(boot_config_entry->target_img_partition_id != NULL, BL_ERR_NULL_PTR_PASSED|BL_ERROR_GROUP_BOOT );  
		 
        /* Initialize the hash data segment from the ELF image */
        boot_elf_load_init_hash_segment(boot_config_entry->
                                          target_img_partition_id,
                                        &qc_image_header); 

        /* For ELF images, authenticate the hash data segment first before loading any segments */ 
        if(boot_config_entry->auth == TRUE)
        {
          //load elf image info
          populate_sbl_info = boot_populate_sbl_info(&sbl_elf_info, &sbl_white_list_param,
                                                boot_config_entry->whitelist_ptr);
          BL_VERIFY( populate_sbl_info == SEC_IMG_AUTH_SUCCESS, (uint16)populate_sbl_info|BL_ERROR_GROUP_SECURITY );
          
          /* Authenticate the image metadata (hashtable)*/ 
          sec_img_auth_error_status = boot_auth_image_hashtable
                                      ((uint32)boot_config_entry->target_img_sec_type,
                                        &sbl_elf_info,
                                        &sbl_white_list_param
                                      );
          BL_VERIFY( sec_img_auth_error_status == SMC_SUCCESS, 
                    (uint16)sec_img_auth_error_status|BL_ERROR_GROUP_SECIMG );
          
          /* At this point authentication is successful, store the hash info to shared imem*/
          boot_store_tpm_hash_block(bl_shared_data, boot_get_sbl_auth_verified_info(bl_shared_data));
					
          /* set flag to track successful hash-table authentication */
          boot_set_hash_table_auth_result((sec_img_auth_error_status == SMC_SUCCESS));
        }
        
        /* Load the ELF segments and verify segments hash */ 
        boot_load_elf_image(NULL, 
                            (uint32)boot_config_entry->target_img_sec_type,
                             boot_config_entry->auth
                            );
        
        /* Restore to Default state. Turn off subsys self auth mode */
        boot_elf_set_auth_and_ssa_enable(FALSE, FALSE);
        
        /* Copy the secboot_verified_info_type for the loaded image
           to the QSEE Interface image entry */
        qmemcpy(&(qsee_interface_image_entry->image_verified_info),
                boot_get_sbl_auth_verified_info(bl_shared_data),
                sizeof(secboot_verified_info_type));
        
        /* Assign entry point and e_ident class to QSEE Interface image entry */
        qsee_interface_image_entry->entry_point = boot_get_elf_entry_address();
        
        qsee_interface_image_entry->e_ident = boot_get_elf_eident_class();
        
        break;
        

      /* Segmented ELF Image Type */
      case CONFIG_IMG_ELF_SEG:
     
        /* Enable subsys self auth (ssa) support if applicable. Set auth mode
           according to the config table entry */
        boot_elf_set_auth_and_ssa_enable( boot_config_entry->auth,
                                          boot_config_entry->boot_ssa_enabled);
        
        /* Target image partition ID pointer must be defined */
        BL_VERIFY(boot_config_entry->target_img_partition_id != NULL,
                  BL_ERR_NULL_PTR_PASSED|BL_ERROR_GROUP_BOOT );  
		 
        /* For CONFIG_IMG_ELF_SEG type image, elf header, program header, hash table is
          assumed to be loaded and initialized at this point. Not loaded explicitly as
          in case of CONFIG_IMG_ELF via boot_elf_load_init_hash_segment. Directly proceed to
          loading segments

          Since segments of type CONFIG_IMG_ELF_SEG (eg loader, core) share elfheader, 
          progheader, hash table with loader, those are already authenticated at this point.
          If boot_config_entry->auth is set to true, sec_img_auth_verify_metadata
          performs other validations but does not authenticate hash table.
        */
        if(boot_config_entry->auth == TRUE)
        {
          //load elf image info
          populate_sbl_info = boot_populate_sbl_info(&sbl_elf_info, &sbl_white_list_param,
                                                boot_config_entry->whitelist_ptr);
          BL_VERIFY( populate_sbl_info == SEC_IMG_AUTH_SUCCESS, (uint16)populate_sbl_info|BL_ERROR_GROUP_SECURITY );
          
          /* Authenticate the image metadata (hashtable)*/ 
          sec_img_auth_error_status = boot_auth_image_hashtable(
                                        (uint32)boot_config_entry->target_img_sec_type,
                                        &sbl_elf_info,
                                        &sbl_white_list_param
                                        );
          BL_VERIFY( sec_img_auth_error_status == SMC_SUCCESS, 
                     (uint16)sec_img_auth_error_status|BL_ERROR_GROUP_SECIMG );
          
          /* set flag to track successful hash-table authentication */
          boot_set_hash_table_auth_result((sec_img_auth_error_status == SMC_SUCCESS));
        }
        
        /* Load the ELF segments and verify segments hash */ 
        boot_load_elf_image(boot_config_entry->whitelist_ptr,
                            (uint32)boot_config_entry->target_img_sec_type,
                            boot_config_entry->auth
                            );
        
        /* Restore to Default state. Turn off subsys self auth mode */
        boot_elf_set_auth_and_ssa_enable(FALSE, FALSE);
        
        /* Copy the secboot_verified_info_type for the loaded image
           to the QSEE Interface image entry */
        qmemcpy(&(qsee_interface_image_entry->image_verified_info),
                boot_get_sbl_auth_verified_info(bl_shared_data),
                sizeof(secboot_verified_info_type));
        
        /* Assign entry point and e_ident class to QSEE Interface image entry */
        qsee_interface_image_entry->entry_point = boot_config_entry->seg_elf_entry_point;
        
        qsee_interface_image_entry->e_ident = boot_get_elf_eident_class();
        
        break;


      /* Unsupported Image */
      default:
        BL_ERR_FATAL(BL_ERR_UNSUPPORTED_IMG|BL_ERROR_GROUP_BOOT);
    }
  }
  
  BL_VERIFY(boot_config_entry->target_img_str != NULL,
            BL_ERR_NULL_PTR_PASSED|BL_ERROR_GROUP_BOOT );


  /* Retreive number of bytes read from flash via boot statistics. */
  bytes_read = boot_statistics_get_flash_byte_counter();

  /* Format number of bytes read into string for boot log. */
  snprintf(bootlog_buffer,
             BOOT_LOG_TEMP_BUFFER_SIZE,
       "%s Image Loaded, Delta - (%d Bytes)",
       (char *)boot_config_entry->target_img_str,
       bytes_read);

  boot_log_delta_time(config_start_time, bootlog_buffer);
  
  boot_invalidate_img_whitelist();
  boot_invalidate_hdr_meta_whitelist();
  
  
  
  /*------------------------------------------------------------------
   Execute the loaded image 
  ------------------------------------------------------------------*/
  /* Jump and exec properties should be mutually exclusive */ 
  if(boot_config_entry->exec == TRUE && 
     boot_config_entry->jump == FALSE &&
     boot_config_entry->load == TRUE)
  {
    /* Image must be authenticated in order to execute */
    BL_VERIFY(boot_config_entry->auth == TRUE , BL_ERR_AUTH_DISABLED|BL_ERROR_GROUP_BOOT); 

    /* Verify valid execute function pointer in the config entry */
    BL_VERIFY(boot_config_entry->exec_func != NULL , BL_ERR_NULL_PTR_PASSED|BL_ERROR_GROUP_BOOT);

    /* Execute */
    (boot_config_entry->exec_func)(bl_shared_data);
  }
  
  /*------------------------------------------------------------------
   Perform Post-Procedures 
  ------------------------------------------------------------------*/
  boot_do_procedures(bl_shared_data, boot_config_entry->post_procs);
  
  /*------------------------------------------------------------------
   Jump to the loaded image 
  ------------------------------------------------------------------*/
  /* Jump and exec properties should be mutually exclusive */ 
  if(boot_config_entry->jump == TRUE && 
     boot_config_entry->exec == FALSE &&
     boot_config_entry->load == TRUE)
  {
    /* Image must be authenticated in order to jump */
    BL_VERIFY(boot_config_entry->auth == TRUE , BL_ERR_AUTH_DISABLED|BL_ERROR_GROUP_BOOT); 

    /* Verify valid jump function pointer in the config entry */
    BL_VERIFY(boot_config_entry->jump_func != NULL , BL_ERR_NULL_PTR_PASSED|BL_ERROR_GROUP_BOOT);

    /* Jump */
    (boot_config_entry->jump_func)(bl_shared_data);
  }

  return;
}


/*===========================================================================
**  Function :  boot_config_process_bl
** ==========================================================================
*/
/*!
* 
* @brief
*   Function to process and execute boot code based on information from the  
*   configuration table. This parses through the entire table and calls
*   boot_config_process_entry() on each entry corresponding to the host 
*   image in order. 
* 
* @param[in] bl_shared_data - Pointer to the shared data structure
* @param[in] host_img - Image ID of the host boot loader 
* @param[in] boot_config_table - Bootloader specific configuration table
*     
* @par Dependencies
*   None
*   
* @retval
*   None
* 
* @par Side Effects
*   None
*/
//extern boot_configuration_table_entry* sbl1_parse_config_entry(void); //changed here
//extern void sbl1_xblconfig_init(bl_shared_data_type *bl_shared_data );


void boot_config_process_bl 
( 
  bl_shared_data_type *bl_shared_data
)
{
  boot_configuration_table_entry *curr_entry = NULL;

  BL_VERIFY( bl_shared_data != NULL,
             BL_ERR_NULL_PTR_PASSED|BL_ERROR_GROUP_BOOT);
       

  curr_entry=sbl1_parse_config_entry();
  
  /* For every entry in the boot configuration table */
  while(curr_entry->host_img_id != NONE_IMG)
  {           
    
      boot_config_process_entry(bl_shared_data,
                                curr_entry);  
      curr_entry=sbl1_parse_config_entry();
  }
 
  return;
}

/*===========================================================================
**  Function :  boot_handoff_to_devprogDDR_or_DDRDebugImage
** ==========================================================================
*/
/*!
* 
* @brief
*   Function to process and execute pre/post-proc arrays from the  
*   configuration table. This parses through the entire table and calls
*   boot_do_procedures on each pre-proc array and post-proc array 
*   entry corresponding to the host image in order.  This is similar to 
*   boot_config_process_bl, but instead of calling boot_config_process_entry,
*   which also contains elf loading apis, just process the associated 
*   function array pointers specified in sbl1_config.c, ensuring that 
*   Deviceprogrammer or DDRDebugImage(DDI) functional flow is identical
*   to SBL's, up until DeviceprogrammerDDR or DDI handoff, except 
*   for not loading images.
* 
* @param[in] bl_shared_data - Pointer to the shared data structure
* @param[in] host_img - Image ID of the host boot loader 
* @param[in] boot_config_table - Bootloader specific configuration table
*     
* @par Dependencies
*   None
*   
* @retval
*   None
* 
* @par Side Effects
*   None
*/
void boot_handoff_to_devprogDDR_or_DDRDebugImage
( 
  bl_shared_data_type *bl_shared_data  
)
{
  boot_configuration_table_entry *curr_entry = NULL;

  BL_VERIFY(  bl_shared_data != NULL, BL_ERR_NULL_PTR_PASSED|BL_ERROR_GROUP_BOOT);

  curr_entry=sbl1_parse_config_entry();
  
  /* For every entry in the boot configuration table */
  while(curr_entry->host_img_id != NONE_IMG)
  {           
      /*------------------------------------------------------------------
        Perform Pre-Procedures
      ------------------------------------------------------------------*/
      boot_do_procedures(bl_shared_data, curr_entry->pre_procs);
      /*------------------------------------------------------------------
        Perform Post-Procedures 
      ------------------------------------------------------------------*/
      boot_do_procedures(bl_shared_data, curr_entry->post_procs);  	  
      
      curr_entry=sbl1_parse_config_entry();
  }
 
  return;
}
