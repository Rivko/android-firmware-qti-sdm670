/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                          MEMORY DEBUG INFORMATION 

GENERAL DESCRIPTION
  This module dumps security memory region to destinition address

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright 2015, 2016 by Qualcomm Technologies, Inc.  All Rights Reserved..


*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*===========================================================================

                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.xf/2.1/QcomPkg/XBLLoader/boot_dload_dump.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/23/16   digant  make boot_dload_is_security_region_dump_allowed non-static
05/03/16   kpa     Ensure qsee_is_memory_dump_allowed api is called only in dload
03/01/16   kpa     Take qsee qhee backups only if dload cookies are set.
09/23/15   kpa     Call into qsee_is_memory_dump_allowed for qsee dump check
07/20/15   kpa     Update boot_dload_dump_valid_segment.
06/24/15   kpa     Initial Creation.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "boot_shared_imem_cookie.h"
#include "boot_authenticator.h"
#include "boot_dload_dump.h"
#include "boot_error_if.h"
#include "boot_util.h"
#include "boot_dload.h"
#include "boot_extern_pimem_interface.h"
#include "boot_extern_seccfg_interface.h"
#include "boot_logger.h"

/*===========================================================================

                     EXTERNAL DEFINITIONS FOR MODULE

===========================================================================*/


/*===========================================================================
**  Function :  boot_dload_is_security_region_dump_allowed
** ==========================================================================
*/
/*!
* 
* @brief
*   This function will determine if sbl can dump tz in dload mode
*
* @param[in] shared_data Pointer to shared data
*        
* @par Dependencies
*   None
* 
*/
boot_boolean boot_dload_is_security_region_dump_allowed(bl_shared_data_type *bl_shared_data) 
{
  boolean mem_dump_supported = TRUE;

  int32 sbl1_entry = 0;

  BL_VERIFY( bl_shared_data != NULL , BL_ERR_NULL_PTR_PASSED|BL_ERROR_GROUP_BOOT);

  sbl1_entry = boot_sbl_qsee_interface_get_image_entry(&(bl_shared_data->sbl_qsee_interface), SECBOOT_SBL1_SW_TYPE);

  BL_VERIFY(sbl1_entry != -1, BL_ERR_IMG_NOT_FOUND|BL_ERROR_GROUP_BOOT);

  mem_dump_supported = (boolean)boot_qsee_is_memory_dump_allowed(
                       &(bl_shared_data->sbl_qsee_interface.boot_image_entry[sbl1_entry].image_verified_info));

  return mem_dump_supported;
}

/*===========================================================================
**  Function :  boot_dload_dump_valid_segment
** ==========================================================================
*/
/*!
* 
* @brief
*   This function will determine if Source and destinition addresses are 
*   valid/ accessible and within ranges.
*
* @param[in] 
*            region_src Source address for segment dump
*            region_size Size of segment to be dumped
*        
* @par Dependencies
*   None
* 
*/
static boot_boolean boot_dload_dump_valid_segment
(
  uintnt region_src,
  uintnt region_size
)
{
  uint32 size_in_mb = 0;
  PIMEM_STATUS pimem_enable_status;
  boot_boolean status = FALSE;

  do
  {
    /*If Pimem is disabled skip segments lying in the range */  
    pimem_enable_status = boot_pimem_get_status(&size_in_mb);
    if( PIMEM_OK != pimem_enable_status)
    {
      /* Check if segment has no overlap with pimem region */
      if((region_src < SCL_pIMEM_BASE && 
          (region_src + region_size) < SCL_pIMEM_BASE &&
          (region_src + region_size) > region_src /*no overflow */ 
          ) ||
          ( (region_src + region_size) > region_src && /*no overflow */
            ((SCL_pIMEM_BASE + SCL_pIMEM_SIZE) > SCL_pIMEM_BASE) &&
            (region_src > (SCL_pIMEM_BASE + SCL_pIMEM_SIZE)) && 
            (region_src + region_size) > (SCL_pIMEM_BASE + SCL_pIMEM_SIZE) 
          )
        )
      {
        status = TRUE;
        break;
      }
      else{
        boot_log_message("pImem Disabled, segment not added to dump.");
      }
    }
    else
    { /* Pimem is enabled */
      status = TRUE;
    }
    
  }while(0);
  return status;
}


/*===========================================================================
**  Function :  boot_dload_dump_region
** ==========================================================================
*/
/*!
* 
* @brief
*   This function dumps regions for a single image (Qsee or qhee)
*
* @param[in] shared_imem_offset     Offset to locate dump info.
*            shared_imem_magic_num  Magic number to indicate cookie presence
*        
* @par Dependencies
*   None
* 
*/
static void boot_dload_dump_region(uintnt shared_imem_offset, uintnt shared_imem_magic_num) 
{
  struct security_region_dload_dump_shared_imem_cookie_type *shared_imem_dload_dump_cookie_ptr;
  struct security_region_dload_dump_info_type *security_region_dload_dump_info_ptr;
  struct boot_dump_entry_type *dump_region;
  uint32 dump_entry_index = 0;

  shared_imem_dload_dump_cookie_ptr = (struct security_region_dload_dump_shared_imem_cookie_type *)
    (uintnt)(shared_imem_offset + (uintnt)boot_shared_imem_cookie_ptr);
  
  if(shared_imem_dload_dump_cookie_ptr->magic_num == shared_imem_magic_num)
  { 
    if(shared_imem_dload_dump_cookie_ptr->dst_addr != 0x0)
    {
      /* get dump info struct address */
      security_region_dload_dump_info_ptr = (struct security_region_dload_dump_info_type *)(uintnt)
                                               shared_imem_dload_dump_cookie_ptr->dst_addr;
                                               
      if(security_region_dload_dump_info_ptr->magic_num == shared_imem_magic_num && 
         security_region_dload_dump_info_ptr->version_num >= DLOAD_DUMP_INFO_VERSION)
      {
         BL_VERIFY((security_region_dload_dump_info_ptr->num_of_entries 
                   <= BOOT_DUMP_NUM_ENTRIES), BL_ERR_RAM_DUMP_FAIL|BL_ERROR_GROUP_BOOT);
         for(dump_entry_index =0; 
             dump_entry_index < security_region_dload_dump_info_ptr->num_of_entries;
             dump_entry_index++ )
         {
            dump_region = &security_region_dload_dump_info_ptr->boot_dump_entry[dump_entry_index];
            /*Dump the segment if valid/accessible else skip */
            if( TRUE == boot_dload_dump_valid_segment((uintnt)dump_region->region_src,
                                                      (uintnt)dump_region->region_size))
            {
              qmemscpy((void*)(uintnt)dump_region->region_dst,
              SCL_pIMEM_SIZE, /* TZ Segment would not exceed this max size */
              (void*)(uintnt)dump_region->region_src, 
              dump_region->region_size);
            }
        }
        /* Once Dumps are successfully collected clear dump cookie */
        security_region_dload_dump_info_ptr->magic_num = 0x0;
      }                  
    }
  }
}


/*===========================================================================
**  Function :  boot_dload_dump_security_regions
** ==========================================================================
*/
/*!
* 
* @brief
*   This function will dump backup qsee and qhee regions according to 
*   information passed by qsee/qhee's shared imem cookies, so they can be 
*   ramdumped later.
*
* @param[in] shared_data Pointer to shared data
*        
* @par Dependencies
*   Called only in dload path
* 
*/
void boot_dload_dump_security_regions(bl_shared_data_type *bl_shared_data )
{
 
  if( boot_dload_is_dload_mode_set() == TRUE )
  {
    /*Call qsee is memory dump allowed api only in dload mode */
    if(boot_dload_is_security_region_dump_allowed(bl_shared_data) == TRUE)
    {
      /* Dump Qsee region */
      boot_dload_dump_region(QSEE_DLOAD_DUMP_SHARED_IMEM_OFFSET, 
                           QSEE_DLOAD_DUMP_SHARED_IMEM_MAGIC_NUM);
    
      /* Dump Qhee region */
      boot_dload_dump_region(QHEE_DLOAD_DUMP_SHARED_IMEM_OFFSET, 
                             QHEE_DLOAD_DUMP_SHARED_IMEM_MAGIC_NUM);
    }
  }
}

