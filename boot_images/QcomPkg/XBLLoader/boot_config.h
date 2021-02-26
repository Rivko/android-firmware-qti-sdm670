#ifndef BOOT_CONFIG_H
#define BOOT_CONFIG_H

/**
 * @file boot_config.h
 * @brief
 * Header file for bootloader configuration logic. For every image loaded by
 * a bootloader, a specific configuration table entry must exist which provides
 * specific information unique to the image. In this file we define the structure
 * for the boot configuration table entry as well declaring the externalized function
 * boot_config_process_bl() which should be called once for every bootloader. 
 */

/*==========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    
    
when       who     what, where, why
--------   ---     ---------------------------------------------------------
05/15/17   c_vkuppa Remove rollback protection from boot_configuration_table_entry
03/03/17   ds	   remove clobber prot
01/27/17   digants whitelist pointer added in boot_config struct
10/22/15   kpa     Code cleanup, rename boot_prepare_and_handoff_to_deviceprogrammer_DDR
07/01/15   as      Added seg_elf_entry_point to boot_configuration_table_entry
05/13/15   kedara  Added sbl_qsee_interface_index to boot_configuration_table_entry
04/23/15   kedara  Added boot_prepare_and_handoff_to_deviceprogrammer_DDR
11/20/14   kedara  Code cleanup,update to to include MIheader.h MIbib.h
08/05/14   ck      Added boot_config_process_entry prototype
06/13/14   ck      Added CONFIG_IMG_ELF_SEG image type
03/17/14   ck      Removed boot_config_update_tz_xpu
12/09/13   ck      Removed boot_config_update_all_fuse_versions as this logic is now in QSEE
03/11/13   kedara  Added defines for per image xpu configuration.
02/19/13   kedara  Added boot_config_update_tz_xpu.
02/14/13   kedara  Added boot_config_update_all_fuse_versions.
10/25/12   dh      Add flag to indicate use version rollback protection or not
08/16/12   kedara  Added flag to load elf in chunks (feature ssa).
06/14/12   kedara  Added whitelist clobber table entry 
03/27/12   dh      include miheader.h instead of secboot_types.h
06/03/11   kedara  Added string variable to aid logger messages identify images
03/04/11   dxiang  Support loading of images with custom partition ID information 
01/21/11   dxiang  Initial Creation 

============================================================================
                     Copyright 2011-2015,2017 Qualcomm Technologies, Inc.
                            All Rights Reserved.
                    Qualcomm Confidential and Proprietary
===========================================================================*/

/*==========================================================================
                               INCLUDE FILES
===========================================================================*/
#include "boot_comdef.h"
#include "boot_mc.h"
#include "boot_sbl_shared.h"
#include "MIbib.h"
#include "MIheader.h"
#include "boot_whitelist_prot.h"

/*==========================================================================
                          PUBLIC DATA DECLARATIONS
===========================================================================*/

/**
 * Image types 
 */
typedef enum
{
  CONFIG_IMG_DEFAULT = ~0,                /**< No specific image type */
  CONFIG_IMG_QC = 0,                      /**< Qualcomm image type */
  CONFIG_IMG_ELF,                         /**< ELF formatted image type */
  CONFIG_IMG_BIN,                         /**< Binary image type */
  CONFIG_IMG_ELF_SEG,                     /**< ELF Segment(s) */
  CONFIG_IMG_MAX_SIZE = 0x7FFFFFFF
}config_image_type;

/**
 * Boot configuration table entry 
 */
typedef struct
{
  image_type host_img_id;                 /**< Image ID of the host */
  config_image_type host_img_type;        /**< Image type of the host */ 
  image_type target_img_id;               /**< Image ID of the target */ 
  config_image_type target_img_type;      /**< Image type of the target */ 
  secboot_sw_type target_img_sec_type;    /**< Secure Software ID of the target */
  boot_boolean load;                      /**< Target will be loaded */
  boot_boolean auth;                      /**< Target will be authenticated */
  boot_boolean exec;                      /**< Target will execute immediately after
                                               loading/authentication */
  boot_boolean jump;                      /**< Target will be jumped to after
                                               post procedures are complete */ 
  boot_procedure_func_type exec_func;     /**< Function pointer to execute function.
                                               Must be defined if exec is TRUE */ 
  boot_procedure_func_type jump_func;     /**< Function pointer to jump function.
                                               Must be defined if jump is TRUE */ 
  boot_procedure_func_type *pre_procs;    /**< Function pointer array to pre-procedures */
  boot_procedure_func_type *post_procs;   /**< Function pointer array to post-procedures */
  boot_logical_func_type boot_load_cancel;/**< Function pointer to cancel loading */ 
  uint8 * target_img_partition_id;        /**< Target image partition ID information */
  uint8 * target_img_str;                  /**< Target image name information  */
  boot_boolean boot_ssa_enabled;           /**< Subsystem self authentication (ssa)*/
  boot_boolean enable_xpu;                 /**< Enable XPU configuration for the image */
  uint32 xpu_proc_id;                      /**< XPU proc id to be passed to TZ */
  uint32 sbl_qsee_interface_index;        /**< Index of this image's entry in sbl qsee interface struct */
  uint64 seg_elf_entry_point;             /**< Entry point for segmented ELF */
  whitelst_tbl_entry_type *whitelist_ptr;  /**< ptr to the image whitelist table */
  uint32 RESERVED_3;                      /**< RESERVED */
  uint32 RESERVED_4;                      /**< RESERVED */
}boot_configuration_table_entry;


/*==========================================================================
                          PUBLIC FUNCTION DECLARATIONS
===========================================================================*/

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
                          boot_configuration_table_entry * boot_config_entry);


/*===========================================================================
**  Function :  boot_config_process_bl
** ==========================================================================
*/
/*!
* 
* @brief
*   Function to process and execute boot code based on information from the  
*   configuration table 
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
void boot_config_process_bl
( 
  bl_shared_data_type *bl_shared_data
);

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
);


boot_configuration_table_entry* sbl1_parse_config_entry();
#endif /* BOOT_CONFIG_H */
