/*=============================================================================

                            XBLConfig Module Sources

GENERAL DESCRIPTION
  This file defines XBL config api's to load data from storage device
  load module.

Copyright 2016, 2017 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/
/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when         who     what, where, why
----------   ---     ----------------------------------------------------------
05/10/17     kpa      rename XBLCONFIG_SIZE_OVERFLOW_ERR to XBLCONFIG_OVERFLOW_ERR
03/15/17     kpa      Update xcfg_get_data arguments
03/03/17     kpa      Updates to validate load address.
03/03/17     ds	      remove clobber prot
02/14/17     kpa      Rename XBLCONFIG_ERR_NONE to XBLCONFIG_SUCCESS
02/14/17     ds	      whitelist for xblconfig
01/20/17     kpa      Add buffer to local clobber list to allow loading
10/14/16     kpa      Initial version
===========================================================================*/
 

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "XBLConfig.h"
#include "boot_elf_loader.h"
#include "boot_flash_dev_if.h"
#include "XcfgUtils.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

extern uint8 XBLConfig_partition_id[];

/*=============================================================================
                              FUNCTION DEFINITIONS
=============================================================================*/

/*===========================================================================
**  Function :  xcfg_get_data
** ==========================================================================
*/
/*!
* 
* @brief
*   This function loads data from storage
* 
* @param[in] 
*  uintnt file_offset          - Start of data to be loaded in partition from
*                                storge device
*  uintnt file_size            - Size of data to be loaded.
*
* @param[out] 
*  void *buffer                - pointer to buffer, loaded with data
* 
*       
* @par Dependencies
*  Partition is assumed to be opened for image loading with flash lib data struct
*  configured
*   
* @retval
*   xblconfig_status_type.   XBLCONFIG_SUCCESS if no error else error value
* 
* @par Side Effects
*   None
* 
*/
xblconfig_status_type xcfg_get_data( uintnt file_offset, uintnt file_size, void *buffer)
{
  xblconfig_status_type status = XBLCONFIG_CONFIG_DATA_LOAD_ERR;
  boot_boolean success = FALSE;  
  boot_flash_trans_if_type *trans_if = NULL;
  
  do
  {
    if(buffer == NULL)
    {
      status = XBLCONFIG_NULL_PTR_ERR;
      break;      
    }
    if( file_size == 0)
    {
      status = XBLCONFIG_INVALID_SIZE;
      break;      
    }

    if( (file_offset > XCFG_UINT32_MAX) || 
        (file_size > XCFG_UINT32_MAX))
    {
      status = XBLCONFIG_OVERFLOW_ERR;
      break;      
    }
    
    boot_flash_configure_target_image(XBLConfig_partition_id);    
    trans_if = boot_flash_dev_open_image(GEN_IMG);
    
    /* Partition is present, then load it to memory */
    if (trans_if != NULL) 
    {    
                                 
      /* Read Config item data to destination address */
      success = boot_flash_trans_read(trans_if, buffer,
                                      (uint32)file_offset,
                                      (uint32)file_size,
                                      IMAGE_BODY_TYPE);
      if(success != TRUE)
      {
        break;
      }
      status = XBLCONFIG_SUCCESS;
      
      /* Close partition */
      boot_flash_dev_close_image(&trans_if);
    }
  }while(0);

  return status;
}  
  