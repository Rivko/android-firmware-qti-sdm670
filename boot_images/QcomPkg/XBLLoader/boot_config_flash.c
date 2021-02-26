/*===========================================================================

                    BOOT CONFIG DATA TABLE UPDATE FUNCTION

DESCRIPTION
  Contains function to update config data table with data stored in flash

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2012-2013, 2015-2017 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    
    
when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/03/17   ds	   remove clobber prot
01/27/17   digants whitelist protection changes for cdt loading
03/17/16   bd      Remove clobber holes after their usage is done
06/20/15   as      Make usage of this library flash type agnostic.
05/19/15   kpa     Continue bootup if cdt partition is present but cdt 
                   is not programmed 
01/27/15   kpa     Added boot_cdt_contains_ddr_cdb() and check in 
                   boot_update_config_data_table to see if we parsed DDR CDB
06/18/13   sl      Check flash type inside boot_update_config_data_table()
12/12/12   jz      Handle CDT not programmed in eMMC
12/05/12   jz      Initial Creation.
===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "boot_config_data.h"
#include "boot_util.h"
#include "boot_error_if.h"
#include "boot_sbl_if.h"
#include "boot_logger.h"
#include "boot_flash_dev_if.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

/**
*  size of the cdt header in bytes
*/
#define CDT_HEADER_SIZE sizeof(struct cdt_header)

/* max number of CDBs */
#define MAX_NUM_CDBS 10

/* size of the meta data in bytes */
#define META_DATA_SIZE (MAX_NUM_CDBS * sizeof(struct cdb_meta))

/* buffer to hold initial data (CDT header and meta data) read from flash */
static uint8 cdt_buf[CDT_HEADER_SIZE + META_DATA_SIZE];

//make whitelist for cdt_buf
static whitelst_tbl_entry_type cdt_buf_whitelist[]={{(uint64)(&cdt_buf),(uint64)(CDT_HEADER_SIZE + META_DATA_SIZE)},{0,0}};

#if (!defined(BOOT_PRE_SILICON)) && defined(BOOT_ERROR_ON_CDT_READ_FAIL)
static volatile boot_boolean error_on_read_fail = TRUE;
#else
static volatile boot_boolean error_on_read_fail = FALSE;
#endif

/* CDT partition ID */
extern uint8 cdt_partition_id[];

/* global to track if CDT contained DDR CDB or not, set to FALSE initially 
   until we find DDR CDB while parsing the flashed CDT */
static boolean cdt_contains_ddr_cdb = FALSE;

/*===========================================================================

**  Function :  boot_update_config_data_table

** ==========================================================================
*/
/*!
* 
* @brief
*    Read the cdt from flash and update the default config_data_table array
* 
* @param[in] cdt_info* pointer to cdt info (its cdt_ptr points to the default CDT)
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
void boot_update_config_data_table(struct cdt_info *cdt_info_ptr)
{ 
  uint32 cdb_start_offset = 0;
  uint32 cdb_size = 0;
  uint32 cdb_meta_offset = 0;
  uint32 cdb_index = 0;
  uint8 *cdb_ptr = NULL;
  struct cdb_meta *cdb_meta_ptr = NULL;
  boot_flash_trans_if_type *trans_if = NULL;

  BL_VERIFY(cdt_info_ptr != NULL && cdt_info_ptr->cdt_ptr != NULL, BL_ERR_NULL_PTR_PASSED|BL_ERROR_GROUP_BOOT);
  
  do
  {
    if (boot_pbl_get_flash_type() == NO_FLASH)
    {
      /* No flash detected, use defaults */
	  boot_log_message("Using default CDT");
      return;
    }

    /* Setup for loading CDT image */
    boot_flash_configure_target_image(cdt_partition_id);

    /* Create a flash translation interface for copying the image
       from flash to RAM */
    trans_if = boot_flash_dev_open_image(GEN_IMG);
    if (trans_if == NULL)
    {
      /* CDT not programmed, use defaults */	
      boot_log_message("Using default CDT");
      return;
    }

    
    /* Add cdt_buf address range to whitelist */
    if(!(boot_set_active_hdr_meta_whitelist_table(cdt_buf_whitelist)))
    {
      CDT_BL_ERROR(BL_ERR_IMG_SECURITY_FAIL);
    }
    
    /* Read the CDT header and meta data */
    if (boot_flash_trans_read(trans_if, cdt_buf, 0, sizeof(cdt_buf),HDR_META_INFO_TYPE) != TRUE)
    {
      /* Only error on read failure if flag is set */
      if(error_on_read_fail == TRUE)
      {
        boot_log_message("Error: Flash read fail");
        CDT_BL_ERROR(BL_ERR_GET_CDT_FAIL);        
      }
      break;
    }

								
    /* Check the magic number in CDT header */
    if (((struct cdt_header *)cdt_buf)->magic != CONFIG_DATA_MAGIC)
    {
      boot_log_message("Using default CDT");	
      break;
    }
    
    /* Check if the first offset to CDB points to valid address. 
       if it does not, enter error handler */
    cdb_meta_ptr = (struct cdb_meta *)(cdt_buf + CDT_HEADER_SIZE);
    cdb_start_offset = cdb_meta_ptr->offset;
    cdt_info_ptr->size = cdb_start_offset;
    if(cdb_start_offset == 0 || cdb_start_offset >= CONFIG_DATA_TABLE_MAX_SIZE)
    {
      boot_log_message("Error: Platform ID is not programmed");
      CDT_BL_ERROR(BL_ERR_GET_CDT_FAIL);
    }
    
    /* Loop through the meta data to locate/read the CDBs and update the default CDT.
    Check to see if we parsed a DDR CDB, if so, set the global.  Current CDT assumption
    is second block read is DDR CDB, but currently no cookie is embedded to identify
    the DDR CDB.
    This will trigger SBL to send the updated, parsed DDR CDB to DDR driver, to overwrite
    the initial default compiled DDR CDB that was initially passed. 
    */
    cdt_contains_ddr_cdb = FALSE;
	   
    for (cdb_index = 0, cdb_meta_offset = CDT_HEADER_SIZE;
         cdb_meta_offset < cdb_start_offset;
         cdb_index++, cdb_meta_ptr++, cdb_meta_offset+=sizeof(struct cdb_meta))
    {
      /* DDR CDB is 2nd entry in CDT, cdb_index==1 */
      if (cdb_index == 1)
      {
        cdt_contains_ddr_cdb = TRUE;
      }
	  
      /* Copy the meta_data to replace the default CDT directly */
      qmemscpy((cdt_info_ptr->cdt_ptr + cdb_meta_offset), 
                META_DATA_SIZE,
              cdb_meta_ptr, sizeof(*cdb_meta_ptr));
              
      /* Get the CDB to be updated in the default CDT */
      cdb_ptr = boot_get_config_data_block(cdt_info_ptr->cdt_ptr, 
                                           cdb_index,
                                           &cdb_size);

       /* Read the CDB data in flash to replace the default CDT directly */
      if (boot_flash_trans_read(trans_if,
                                cdb_ptr,
                                cdb_meta_ptr->offset,
                                cdb_meta_ptr->size,
                                IMAGE_BODY_TYPE) != TRUE)
      {
        /* Only error on read failure if flag is set */
        if(error_on_read_fail == TRUE)
        {
          boot_log_message("Error: Flash read fail");
          CDT_BL_ERROR(BL_ERR_GET_CDT_FAIL);        
        }
        break;
      }

      /* update the total size of the cdt */
      cdt_info_ptr->size += cdb_meta_ptr->size;
    }  
  } while (0);
  
  /* Release the translation layer resource */
  boot_flash_dev_close_image(&trans_if);
}

/*===========================================================================

**  Function :  boot_cdt_contains_ddr_cdb

** ==========================================================================
*/
/*!
* 
* @brief
*    Return boolean cdt_contains_ddr_cdb value, so SBL knows whether or not 
*    to call ddr_set_params() again to overwrite the default compiled CDT 
*    contents
*   
* @par Dependencies
*   None
*   
* @retval
*   boolean cdt_contains_ddr_cdb indicating if DDR CDB found in 
*             flashed CDT
* 
* @par Side Effects
*   None
*/
boolean boot_cdt_contains_ddr_cdb(void)
{ 
  return cdt_contains_ddr_cdb;
}
