/*=============================================================================

                            XBLConfig Module Sources

GENERAL DESCRIPTION
  This file defines XBL config api's that interface with XBLLoader's image 
  authentication module.

Copyright 2016, 2017 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/
/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when         who     what, where, why
----------   ---     ----------------------------------------------------------
07/12/17     ds       correct the offset passed to transient data hash call
05/10/17     kpa      use wrapper api xcfg_malloc instead of dalsys malloc
05/04/17     kpa      remove xblconfig image id temp define
03/15/17     kpa      Update buffer free logic in xcfg_verify_transient_data
03/03/17     kpa      Added xcfg_load_addr_validation_init, deinit
02/14/17     kpa      Rename XBLCONFIG_ERR_NONE to XBLCONFIG_SUCCESS
02/10/17     kpa      update boot_sec_img_auth_partial_hash api's
10/14/16     kpa      Initial version
===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/

#include "MIheader.h"
#include "boot_authenticator.h"
#include "boot_elf_loader.h"
#include "XcfgLoaderAuth.h"
#include "XcfgFlashInterface.h"
#include "boot_extern_sec_img_interface.h"
#include "boot_whitelist_prot.h"
#include "XcfgUtils.h"
#include "boot_fastcall_tz.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

#define PARTIAL_HASH_BUFF_SIZE 1024

/*=============================================================================
                              FUNCTION DEFINITIONS
=============================================================================*/

/*===========================================================================
**  Function :  xcfg_verify_transient_data
** ==========================================================================
*/
/*!
* 
* @brief
*   This function adds data to running hash.
* 
* @param[in]    
*     seg_start_offset  location of segment data within partition
*     seg_end_offset    end offset of segment data chunk
*     hash_buffer       Pointer to xbl config hash buffer
*     hash_buffer_size  Size of hash buffer
*     segment_index     index of segment containing data to be verified
*     segment_size      size of segment containing data to be verified
*
* @param[out] 
*  None
*        
* @par Dependencies
*   None
*   
* @retval
*   xblconfig_status_type. XBLCONFIG_SUCCESS if no error else error value
* 
* @par Side Effects
*   None
* 
*/
static xblconfig_status_type xcfg_verify_transient_data
(
  uintnt seg_start_offset,
  uintnt seg_end_offset,
  void *hash_buffer,
  uintnt hash_buffer_size,
  uintnt segment_index,
  uintnt segment_size
)
{
  xblconfig_status_type status = XBLCONFIG_SUCCESS, free_status = XBLCONFIG_SUCCESS;
  void *partial_hash_data_buff = NULL;
  uintnt segment_chunk_size, data_size;
  do
  {
    if( seg_start_offset > seg_end_offset)
    {
      status = XBLCONFIG_INVALID_SIZE;
      break;      
    }
    
    /* Allocate buffer for streaming hash compute */
    status = xcfg_malloc(PARTIAL_HASH_BUFF_SIZE, (void **)&partial_hash_data_buff);
    if(status != XBLCONFIG_SUCCESS)
    {
      break;
    }
     
    segment_chunk_size = seg_end_offset - seg_start_offset;
    data_size = PARTIAL_HASH_BUFF_SIZE;
    while(segment_chunk_size > data_size)
    {
      /* Load segment data into buffer */
      if(XBLCONFIG_SUCCESS != xcfg_get_data( seg_start_offset,
                                             data_size,
                                             partial_hash_data_buff)
        )
      {
        status = XBLCONFIG_VERIFY_ERR;
        break;
      }
   
      /* Add config item data to running hash */
      if(SMC_SUCCESS != boot_sec_img_auth_partial_hash_update( 
                                   hash_buffer,
                                   hash_buffer_size,
                                   partial_hash_data_buff,
                                   data_size,
                                   segment_index )
        )
      {
        status = XBLCONFIG_VERIFY_ERR;        
        break;
      }
               
      segment_chunk_size = segment_chunk_size - data_size;
      seg_start_offset = seg_start_offset + data_size;
    }
    
    if(segment_chunk_size > 0 && status == XBLCONFIG_SUCCESS)
    {
      /*segment_chunk_size has to be less than data size at this point */
      /* Load segment data into buffer */
      if(XBLCONFIG_SUCCESS != xcfg_get_data( 
                                 seg_start_offset,
                                 segment_chunk_size,
                                 partial_hash_data_buff))
      {
        status = XBLCONFIG_VERIFY_ERR;          
        break;
      }
   
      /* Add config item data to running hash */
      if( SMC_SUCCESS != boot_sec_img_auth_partial_hash_update( 
                                  hash_buffer,
                                  hash_buffer_size,
                                  partial_hash_data_buff,
                                  segment_chunk_size,
                                  segment_index)
        )
      {
        status = XBLCONFIG_VERIFY_ERR;
      }      
    }
    
  }while(0);
  
  /*In all cases success or failure, free the buffer if memory is allocated */
  free_status = xcfg_free((void**)&partial_hash_data_buff);
  if(status == XBLCONFIG_SUCCESS)
  {
    /* Overwrite main logic status with cleanup status only if no error
    in main logic */
    status = free_status;    
  }
  
  return status;  
}      
/*===========================================================================
**  Function :  xcfg_auth_image_hashtable
** ==========================================================================
*/
/*!
* 
* @brief
*   This function authenticates XBLConfig Image hash table
* 
* @param[out] 
*  None
*        
* @par Dependencies
*   None
*   
* @retval
*   xblconfig_status_type.   XBLCONFIG_SUCCESS if no error else error value
* 
* @par Side Effects
*   None
* 
*/
xblconfig_status_type xcfg_auth_image_hashtable(void)
{
  xblconfig_status_type status = XBLCONFIG_SUCCESS;
  sec_img_auth_elf_info_type xcfg_auth_elf_info;
  sec_img_auth_whitelist_area_param_t xcfg_auth_white_list_param;  
  sec_img_auth_error_type populate_xcfg_info_status;  
  bl_error_boot_type sec_img_auth_error_status = BL_ERR_IMG_SECURITY_FAIL;

  do{
    populate_xcfg_info_status = boot_populate_sbl_info(&xcfg_auth_elf_info,
                                               &xcfg_auth_white_list_param,
                                               NULL);
    if( populate_xcfg_info_status != SEC_IMG_AUTH_SUCCESS)
    {
      status = XBLCONFIG_VERIFY_ERR;
      break;      
    }
    sec_img_auth_error_status = boot_auth_image_hashtable((uint32)SECBOOT_XBLCONFIG_SW_TYPE,
                                              &xcfg_auth_elf_info,
                                              &xcfg_auth_white_list_param);
    if( sec_img_auth_error_status != SMC_SUCCESS)
    {
      status = XBLCONFIG_VERIFY_ERR;
      break;      
    }                    
  }while(0);
  return status;
} /* xcfg_auth_image_hashtable */


/*===========================================================================
**  Function :  xcfg_verify_data
** ==========================================================================
*/
/*!
* 
* @brief
*   This function verifies input data.
* 
* @param[in]    
*     start_offset      location of requested item data within partition
*     config_item_size  size of data to be verified
*     segment_index     index of segment containing data to be verified
*     segment_offset    offset of segment containing data to be verified
*     segment_size      size of segment containing data to be verified
*     buffer            Pointer to data to be verified
*     hash_buffer       Pointer to xbl config hash buffer
*     hash_buffer_size  Size of hash buffer
*
* @param[out] 
*  None
*        
* @par Dependencies
*   None
*   
* @retval
*   xblconfig_status_type. XBLCONFIG_SUCCESS if no error else error value
* 
* @par Side Effects
*   None
* 
*/
xblconfig_status_type xcfg_verify_data
(
  uintnt config_item_offset,
  uintnt config_item_size,
  uintnt segment_index,
  uintnt segment_offset,
  uintnt segment_size,
  void *buffer,
  void *hash_buffer,
  uintnt hash_buffer_size
)
{
  xblconfig_status_type status = XBLCONFIG_SUCCESS;
  bl_error_boot_type result = BL_ERR_IMG_SECURITY_FAIL;
  do
  {
    if( buffer == NULL || hash_buffer == NULL)
    {
      status = XBLCONFIG_NULL_PTR_ERR;
      break;      
    }
    
    result = boot_sec_img_auth_partial_hash_init( hash_buffer,
                                                  hash_buffer_size,
                                                  buffer,
                                                  config_item_size,
                                                  segment_index);
    if(result != SMC_SUCCESS)
    {
      status = XBLCONFIG_VERIFY_ERR;
      break;
    }
    
    /* Compute hash for intial data if applicable */
    if(segment_offset < config_item_offset)
    {
      status = xcfg_verify_transient_data(segment_offset, config_item_offset,
                  hash_buffer, hash_buffer_size, segment_index, segment_size);
                  
      if(status != XBLCONFIG_SUCCESS )
        break;
    }

    /* Add config item data to running hash */
    result = boot_sec_img_auth_partial_hash_update( hash_buffer,
                                                    hash_buffer_size,
                                                    buffer,
                                                    config_item_size,
                                                    segment_index );
               
    if(result != SMC_SUCCESS)
    {
      status = XBLCONFIG_VERIFY_ERR;          
      break;
    }
    
    /* check for overflow */
    if(
       ((config_item_offset + config_item_size) < config_item_offset) ||
       ((segment_offset + segment_size) < segment_offset)
      )
    {
      status = XBLCONFIG_OVERFLOW_ERR;
      break;
    }
    
    /* Compute hash for remaining segment data if applicable */
    if((config_item_offset + config_item_size) < 
       (segment_offset + segment_size)
      )
    {
      status = xcfg_verify_transient_data(
                (config_item_offset + config_item_size),
                (segment_offset + segment_size),
                hash_buffer, hash_buffer_size, segment_index, segment_size );
                
      if(status != XBLCONFIG_SUCCESS )
      {
        break;
      }
    }    

    result = boot_sec_img_auth_partial_hash_finalize( hash_buffer, 
                                                      hash_buffer_size,
                                                      buffer,
                                                      config_item_size,
                                                      segment_index );
    if(result != SMC_SUCCESS)
    {
      status = XBLCONFIG_VERIFY_ERR;
    }
     
  }while(0);
  return status;  
}  
