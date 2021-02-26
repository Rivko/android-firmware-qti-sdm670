/*=============================================================================

                               SBL1 Target

GENERAL DESCRIPTION
  Target-specific initialization and interface implementation.
  
Copyright 2011-2017 by QUALCOMM Technologies Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who      what, where, why
--------   ---      ------------------------------------------------------------
05/15/17   c_vkuppa Remove rollback protection from ddr_training_config_entry
03/03/17   ds		remove clobber prot
02/14/17   ds		whitelist for ddr training
05/03/16   rp		Call DDR training based on conditional loading check
10/22/15   plc      Replaced tlb invalidate with generic EL-agnostic call
07/14/15   rp       Initial revision
=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boot_comdef.h"
#include "boot_dload.h"
#include "boot_cache_mmu.h"
#include "boot_mc.h"
#include "boot_sbl_shared.h"
#include "boot_config.h"
#include "boot_page_table_armv8.h"
#include "boot_sbl_if.h"
#include "boot_hash_buffer.h"
#include "boot_flash_dev_if.h"
#include "boot_loader.h"
#include "sbl1_ddr_training.h"
#include "boot_extern_ddr_interface.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

/* Shared data between SBL */
extern struct bl_shared_data_type bl_shared_data;

extern uint8 sbl1_partition_id[];


/* Whitelist region and Configuration table entry used by ddr_training_entry to
   load DDR Training image.  Placed here so it is in RW region and not stack. */
static whitelst_tbl_entry_type ddr_training_whitelist[] = DDR_TRAINING_WHITELIST;

/*==========================================================================
                     PRE/POST PROCEDURE DEFINITIONS 
===========================================================================*/
/* Initialize the elf header for ddr training image loading */ 
static void ddr_training_configure_header(bl_shared_data_type *bl_shared_data )
{
  boot_elf_ziinit_structures(NULL);
  boot_set_elf_and_prog_header(
      bl_shared_data->sbl_shared_data->pbl_shared_data->elf_meta_info.elf_hdr,
      bl_shared_data->sbl_shared_data->pbl_shared_data->elf_meta_info.prog_hdr
    );
  boot_set_hash_segment_buffer(
      (void *)bl_shared_data->sbl_shared_data->pbl_shared_data->elf_meta_info.hash_seg_hdr
  );

  /* Configure the target image using custom partition ID information */ 
  boot_flash_configure_target_image(sbl1_partition_id);
}
    
/*==========================================================================
          FUNCTIONS TO EXECUTE BEFORE DDR TRAINING IMAGE LOADING
===========================================================================*/
boot_procedure_func_type load_ddr_training_pre_procs[] = 
{
  /* Initialize the elf header for ddr training image loading */
  ddr_training_configure_header,

  /*----------------------------------------------------------------------
   Last entry in the table.
  ----------------------------------------------------------------------*/
  NULL 
};    
    
/*==========================================================================
                 DEFINE DDR TRAINING BOOT CONFIG TABLE
===========================================================================*/    
boot_configuration_table_entry ddr_training_config_entry =
  { SBL1_IMG,                                     /* host_img_id */
    CONFIG_IMG_QC,                                /* host_img_type */
    GEN_IMG,                                      /* target_img_id */
    CONFIG_IMG_ELF_SEG,                           /* target_img_type */
    SECBOOT_SBL1_SW_TYPE,                         /* target_img_sec_type */
    FALSE,                                        /* load - Set this to true to enable conditional loading of DDR Training */
    TRUE,                                         /* auth */
    FALSE,                                        /* exec */
    FALSE,                                        /* jump */
    NULL,                                         /* exec_function */
    NULL,                                         /* jump-function */
    load_ddr_training_pre_procs,                  /* pre_procs */
    NULL,                                         /* post_procs */
    NULL,                                         /* load_cancel */
    sbl1_partition_id,                            /* target_img_partition_id */
    (uint8 *)"DDR Training Image Loaded, Delta",  /* target_img_str */
    FALSE,                                        /* boot_ssa_enabled */
    FALSE,                                        /* enable_xpu */
    0x0,                                          /* xpu_proc_id */
    0x0,                                          /* sbl_qsee_interface_index */
	0x0,						  				  /* seg_elf_entry_point*/
	ddr_training_whitelist						  /* whitelist_ptr*/    
  };


/* mem_block used by ddr_training_entry to change DDR Training MMU region to
   executable before jumping into image.  Placed here so it is in RW region
   and not stack. */
struct mem_block ddr_training_mem_block =
  { SCL_DDR_TRAINING_CODE_BASE,
    SCL_DDR_TRAINING_CODE_BASE,
    SCL_DDR_TRAINING_CODE_SIZE >> 10,
    MMU_L3_SECTION_MAPPING,
    MMU_PAGETABLE_MEM_READ_ONLY,
    MMU_PAGETABLE_MEM_WRITE_BACK_CACHE,
    MMU_PAGETABLE_EXECUTABLE_REGION, };

/*===========================================================================

**  Function :  sbl_dload_target_entry

** ==========================================================================
*/
/*!
* 
* @brief
*   This function is defined in each SBL Bootloader to handle SBL-specific
*   requirements to enter a download routine
*  
* @par Dependencies
*   None
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void ddr_training_entry ()
{
	if(ddr_training_config_entry.load == TRUE)
	{
	  boot_boolean result = FALSE;

	  /* Use the boot_configuration_table_entry for the DDR Training image. */
	  boot_config_process_entry(&bl_shared_data,
								&ddr_training_config_entry);

	  /* DDR_Training image has been loaded.  Change MMU permissions to allow
		 allow execution before jumping into image.  Entry point is always
		 the beginning of DDR_Training's code memory region as ELF entry point value
		 is occupied with XBLLoader. */
	  result =
		boot_mmu_page_table_map_single_mem_block(
		 (uint64*)mmu_get_page_table_base(),
		  &ddr_training_mem_block);

	  BL_VERIFY(result,
				BL_ERR_MMU_PGTBL_MAPPING_FAIL|BL_ERROR_GROUP_BOOT);

	  ddr_training_mem_block.p_base = SCL_DDR_TRAINING_DATA_BASE;
	  ddr_training_mem_block.v_base = SCL_DDR_TRAINING_DATA_BASE;
	  ddr_training_mem_block.size_in_kbytes = (SCL_DDR_TRAINING_DATA_SIZE + SCL_DDR_TRAINING_DATA_ZI_SIZE) >> 10;
	  ddr_training_mem_block.access = MMU_PAGETABLE_MEM_READ_WRITE;
	  ddr_training_mem_block.executable = MMU_PAGETABLE_NON_EXECUTABLE_REGION;
				
	  result =
		boot_mmu_page_table_map_single_mem_block(
		 (uint64*)mmu_get_page_table_base(),
		  &ddr_training_mem_block);

	  BL_VERIFY(result,
				BL_ERR_MMU_PGTBL_MAPPING_FAIL|BL_ERROR_GROUP_BOOT);            

	  mmu_invalidate_tlb();

	  /* "Exit" XBLLoader and enter DDR Training. */
	  ((void (*)())(uintnt)(SCL_DDR_TRAINING_CODE_BASE))(); 
	 
	}
	else
	{
		boot_ddr_do_phy_training_init();
	}
 
}


