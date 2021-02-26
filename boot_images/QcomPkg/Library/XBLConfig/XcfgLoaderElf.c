/*=============================================================================

                            XBLConfig Module Sources

GENERAL DESCRIPTION
  This file defines XBL config api's that interface with XBLLoader's elf image
  load module.

Copyright 2016, 2017 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/
/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when         who     what, where, why
----------   ---     ----------------------------------------------------------
07/12/17     ds       remove the xcfg_set_is_verified_flag call
05/10/17     kpa      rename XBLCONFIG_SIZE_OVERFLOW_ERR to XBLCONFIG_OVERFLOW_ERR
05/03/17     kpa      update integer overflow checks
03/10/17     kpa      Update boot_get_elf_phdr_entry_info arguments
03/03/17     kpa      set freed malloc memory pointers to null
03/03/17     ds	      remove clobber prot
02/14/17     kpa      Rename XBLCONFIG_ERR_NONE to XBLCONFIG_SUCCESS
01/20/17     kpa      Use internal buffer for hash (pre ddr init)
10/14/16     kpa      Initial version
===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "XBLConfig.h"
#include "boot_elf_loader.h"
#include "XcfgLoaderAuth.h"
#include "XcfgLoaderElf.h"
#include "XcfgFlashInterface.h"
#include "boot_hash_buffer.h"
#include "boot_util.h"
#include "XcfgUtils.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/
extern uint8 XBLConfig_partition_id[];

/* XBLConfig elf info structure containing pointer to 
hash buffer and segment info */
static xcfg_elf_info_type xcfg_elf_info;

/*=============================================================================
                              FUNCTION DEFINITIONS
=============================================================================*/

/*===========================================================================
**  Function :  xcfg_image_get_max_logical_block_count
** ==========================================================================
*/
/*!
* 
* @brief
*   This function returns the max number of image logical blocks
*  (segments in case of elf image)
* 
* @param[out] 
*   max_logical_blk_ptr      pointer to max logical blocks
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
xblconfig_status_type xcfg_image_get_max_logical_block_count
(
  uintnt *max_logical_blk_ptr
)
{
  xblconfig_status_type status = XBLCONFIG_META_DATA_LOAD_ERR;
  do
  {
    if( max_logical_blk_ptr == NULL)
    {
      status = XBLCONFIG_NULL_PTR_ERR;
      break;      
    }
    *max_logical_blk_ptr = xcfg_elf_info.max_phnum;
    status = XBLCONFIG_SUCCESS;
    
  }while(0);

  return status;  
}

/*===========================================================================
**  Function :  xcfg_image_get_block_info
** ==========================================================================
*/
/*!
* 
* @brief
*   This function returns info for requested logical block
*   
*   Note: A logical block for elf file type is a segment
*
* @param[in]   
*    index        index pointing to desired logical block.
*
* @param[out] 
*   blk_size_ptr        pointer to block size
*   is_blk_loadable_ptr pointer to flag indicating loadable block
*   blk_offset_ptr      pointer to block offset on storage device
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
xblconfig_status_type xcfg_image_get_block_info
(
  uintnt index,
  uintnt *blk_size_ptr,
  boolean *is_blk_loadable_ptr,
  uintnt *blk_offset_ptr  
)
{
  xblconfig_status_type status = XBLCONFIG_META_DATA_LOAD_ERR;
  xcfg_segment_info_type *xcfg_segment_info_ptr;
  boolean seg_status = FALSE;
  uintnt blk_destn_addr_dummy = 0;
  do
  {
    if( blk_size_ptr == NULL || is_blk_loadable_ptr == NULL ||
        blk_offset_ptr == NULL
      )
    {
      status = XBLCONFIG_NULL_PTR_ERR;
      break;      
    }
    
    seg_status = boot_get_elf_phdr_entry_info(
                               (void *)boot_get_elf_header(),
                               (void*)boot_get_program_header(),
                               index,
                               blk_size_ptr,
                               is_blk_loadable_ptr,
                               blk_offset_ptr,
                               &blk_destn_addr_dummy
                               );
    if(seg_status == FALSE)
    {
      status = XBLCONFIG_META_DATA_LOAD_ERR;
      break;
    }
    
    xcfg_segment_info_ptr = xcfg_elf_info.segment_info_ptr;
    
    /*while searching for meta data also keep track of segment offset and size */ 
    xcfg_segment_info_ptr[index].offset = *blk_offset_ptr;
    xcfg_segment_info_ptr[index].size = *blk_size_ptr;
    xcfg_segment_info_ptr[index].is_verified = FALSE;   

    status = XBLCONFIG_SUCCESS;
    
  }while(0);

  return status;  
}

/*===========================================================================
**  Function :  xcfg_get_block_data
** ==========================================================================
*/
/*!
* 
* @brief
*   This function returns xbl config data at input offset
*  (segments in case of elf image)
* 
* @param[in] 
*  uintnt block_offset         - Start of data to be loaded in partition from
*                                storge device
*  uintnt block_size           - Size of data to be loaded.
*
* @param[out] 
*  void *buffer                - pointer to buffer, loaded with data
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
xblconfig_status_type xcfg_get_block_data
(
  uintnt block_offset,
  uintnt block_size, 
  void *buffer  
)
{
  xblconfig_status_type status = XBLCONFIG_READ_ERR;
  boolean block_read_status = FALSE;
  do
  {
    if( buffer == NULL )
    {
      status = XBLCONFIG_NULL_PTR_ERR;
      break;
    }
    if( block_size == 0 )
    {
      status = XBLCONFIG_INVALID_SIZE;
      break;
    }

    block_read_status = boot_get_elf_segment_data( block_offset,
                                                   block_size,
                                                   buffer);
    if (block_read_status != TRUE)
      break;
    
    status = XBLCONFIG_SUCCESS;
        
  }while(0);

  return status;  
}

/*===========================================================================
**  Function :  xcfg_image_interface_reset
** ==========================================================================
*/
/*!
* 
* @brief
*   This function resets xbl config image loading interface
* 
* @param[out] 
*   None 
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
xblconfig_status_type xcfg_image_interface_reset (void)
{
  xblconfig_status_type status = XBLCONFIG_SUCCESS;
  do
  {
    /* Close the image partition and release the flash translation interface */
    boot_elf_load_deinit();    
    
  }while(0);

  return status;  
}

/*===========================================================================
**  Function :  xcfg_image_interface_deinit
** ==========================================================================
*/
/*!
* 
* @brief
*   This function deinitializes xbl config image loading interface
*   It free's up structures allocated by image loading interface
* 
* @param[out] 
*   None 
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
xblconfig_status_type xcfg_image_interface_deinit (void)
{
  xblconfig_status_type status = XBLCONFIG_DEINIT_ERR;
  do
  {
    status = xcfg_free((void**)&(xcfg_elf_info.hash_buffer));
    if(status != XBLCONFIG_SUCCESS)
      break; 
    
    status = xcfg_free((void**)&(xcfg_elf_info.segment_info_ptr));
    if(status != XBLCONFIG_SUCCESS)
      break;       

  }while(0);

  return status;  
}

/*===========================================================================
**  Function :  xcfg_get_config_item_segment
** ==========================================================================
*/
/*!
* 
* @brief
*   This function parses program header table and locates segment cointaining
* data at given offset.
* 
* @param[in]   
*    xcfg_elf_info_ptr        Pointer to struct containing segments info
*    config_item_start_offset location of requested item data within partition
*    config_item_size         size of data
*
* @param[out] 
*   segment_index             pointer to segment index
*   segment_offset            pointer to segment offset
*   segment_size              pointer to segment size
*   is_verified               pointer to verified flag status
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
static xblconfig_status_type xcfg_get_config_item_segment
(
  xcfg_elf_info_type *xcfg_elf_info_ptr,
  uintnt config_item_start_offset,
  uintnt config_item_size,
  uintnt *segment_index,
  uintnt *segment_offset,
  uintnt *segment_size,
  boolean *is_verified
)  
{
  xblconfig_status_type status = XBLCONFIG_VERIFY_ERR;  
  uintnt index =0, entry_end_offset =0;
  xcfg_segment_info_type *xcfg_segment_info_ptr;
  
  do
  {
    if( xcfg_elf_info_ptr == NULL || segment_index == NULL ||
        segment_offset == NULL || segment_size == NULL || is_verified == NULL
      )
    {
      status = XBLCONFIG_NULL_PTR_ERR;
      break;      
    }
    if( xcfg_elf_info_ptr->segment_info_ptr == NULL)
    {
      status = XBLCONFIG_INIT_ERR;
      break;      
    }  
    
    xcfg_segment_info_ptr  = xcfg_elf_info_ptr->segment_info_ptr;    
    for(index =0;index < xcfg_elf_info_ptr->max_phnum; index++)
    {
      entry_end_offset = (xcfg_segment_info_ptr[index].offset + xcfg_segment_info_ptr[index].size);
      if(entry_end_offset < xcfg_segment_info_ptr[index].offset )
      {
        status = XBLCONFIG_OVERFLOW_ERR;
        break;
      }
      
      if( xcfg_segment_info_ptr[index].offset <= config_item_start_offset &&
          xcfg_segment_info_ptr[index].size >= config_item_size &&
          entry_end_offset > config_item_start_offset)
      {
        /* Config data fully contained within segment*/
        *segment_index = index;
        *segment_offset = xcfg_segment_info_ptr[index].offset;
        *segment_size = xcfg_segment_info_ptr[index].size;
        *is_verified = xcfg_segment_info_ptr[index].is_verified;
        status = XBLCONFIG_SUCCESS;        
        break;
      }      
    }
  }while(0);
  return status; 
}

/*===========================================================================
**  Function :  xcfg_set_is_verified_flag
** ==========================================================================
*/
/*!
* 
* @brief
*   This function sets the flag indicating a segment has been hash
*  verified once
* 
* @param[in]   
*  segment_index               verified segment  index
*
* @param[out] 
*    None                      pointer to verified flag status
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
static xblconfig_status_type xcfg_set_is_verified_flag
( 
  xcfg_elf_info_type *xcfg_elf_info_ptr,
  uintnt segment_index
)
{
  xblconfig_status_type status = XBLCONFIG_VERIFY_ERR;
  do
  {
    if( xcfg_elf_info_ptr == NULL)
    {
      status = XBLCONFIG_NULL_PTR_ERR;
      break;      
    }
    
    if (segment_index >= xcfg_elf_info.max_phnum)
    {
      status = XBLCONFIG_INVALID_SIZE;
      break;      
    }
    
    xcfg_elf_info_ptr->segment_info_ptr[segment_index].is_verified = TRUE;
    status = XBLCONFIG_SUCCESS;
    
  }while(0);

  return status;
}

/*===========================================================================
**  Function :  xcfg_image_interface_init
** ==========================================================================
*/
/*!
* 
* @brief
*   This function initializes image interface and performs steps necessary
*  prior to data access such as image authentication
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
xblconfig_status_type xcfg_image_interface_init( void )
{
  xblconfig_status_type status = XBLCONFIG_META_DATA_LOAD_ERR;
  mi_boot_image_header_type qc_image_header;
  uint8* hash_buffer_ptr = NULL;
  uintnt hash_segment_size  = 0;
  boolean hash_segment_status = FALSE;
  void **xcfg_elf_hash_buffer  = &(xcfg_elf_info.hash_buffer);
  
  xcfg_elf_info_type *xcfg_elf_info_ptr = &xcfg_elf_info;
  uintnt max_phnum = 0;
  
  do
  {
    /*First load hash segment */
    boot_move_hash_segment_buffer();    
    boot_elf_load_init_hash_segment(XBLConfig_partition_id, &qc_image_header);
  
    /*Authenticate hash segment */
    status = xcfg_auth_image_hashtable();
    if(status != XBLCONFIG_SUCCESS)
      break;
 
    /* Get hash segment data of xbl config image */
    hash_buffer_ptr = boot_get_hash_segment_buffer();
    hash_segment_status = boot_get_elf_hash_segment_size(
                                    (void *)boot_get_elf_header(),
                                    (void *)boot_get_program_header(),
                                    &hash_segment_size);
    if(hash_segment_status != TRUE)
    {
      status = XBLCONFIG_HASH_SEG_ERR;
      break;
    }
    
    if(hash_segment_size > XCFG_UINT32_MAX)
    {
      /*Malloc and memcpy utils support 32 bit sizes. In reality
        hash segment size also not expected to exceed 32 bit */
      status = XBLCONFIG_OVERFLOW_ERR;
      break;
    }    
       
    status = xcfg_malloc(hash_segment_size, (xcfg_elf_hash_buffer));
    if(status != XBLCONFIG_SUCCESS)
    {
      break;
    }
    
    if(hash_buffer_ptr != NULL && (*xcfg_elf_hash_buffer) != NULL)
    {
      qmemcpy((*xcfg_elf_hash_buffer), hash_buffer_ptr, hash_segment_size);
    }
    
    xcfg_elf_info_ptr->hash_buffer_size = hash_segment_size;
    
    boot_restore_hash_segment_buffer();
                                                
    /* After Authentication, configure internal data structs */
    
    if ( TRUE != boot_get_elf_prog_header_count( 
                                         (void *)boot_get_elf_header(),
                                         &max_phnum))
    {
      /* Could not get max prog header count from elf header */
      status = XBLCONFIG_META_DATA_LOAD_ERR;
      break;
    }
    
    /*overflow check */
    if(sizeof(xcfg_segment_info_type) > (XCFG_UINT32_MAX / max_phnum))
    {
      status = XBLCONFIG_OVERFLOW_ERR;
      break;
    }    
    
    /* Prepare segment info data struct, keeping track of all segment
       offset and sizes
    */    
    status = xcfg_malloc(max_phnum * sizeof(xcfg_segment_info_type),
                         (void **)&(xcfg_elf_info_ptr->segment_info_ptr));
    if(status != XBLCONFIG_SUCCESS)
    {
      break;
    }
     
    qmemset( (void *)(xcfg_elf_info_ptr->segment_info_ptr), 0x0,
             (uint32)(max_phnum * sizeof(xcfg_segment_info_type)));

    xcfg_elf_info_ptr->max_phnum = max_phnum;
    
    status = XBLCONFIG_SUCCESS;
    
  }while(0);

  /* Free malloced memory in case of error */
  if(status != XBLCONFIG_SUCCESS)  
  {
    /* Since this is memory leak cleanup after error, ignore 
       the status and continue freeing other buffers 
    */    
    xcfg_free((void**)&xcfg_elf_hash_buffer);    
    xcfg_free((void**)&(xcfg_elf_info_ptr->segment_info_ptr));    
  }
  
  return status;
  
} /* xcfg_image_interface_init */

/*===========================================================================
**  Function :  xcfg_validate_data
** ==========================================================================
*/
/*!
* 
* @brief
*   This function verifies the loaded XBLConfig item.
*   It is an intermediate api to add image meta data (eg segment info) 
*   needed by authentication api's which do the actual validation.
* 
* @param[in]    
*     start_offset             location of requested item data within partition
*     config_item_buffer_size  size of data to be verified
*     buffer                   Pointer to data to be verified
*
* @param[out] 
*  None
*        
* @par Dependencies
*   buffer is assumed to be populated with data to be verified
*   
* @retval
*   xblconfig_status_type. XBLCONFIG_SUCCESS if no error else error value
* 
* @par Side Effects
*   None
* 
*/
xblconfig_status_type xcfg_validate_data
(
  uintnt start_offset,
  uintnt config_item_buffer_size,
  void *buffer  
)
{
  uintnt segment_index, segment_offset, segment_size;
  boolean is_verified = FALSE;  
  xblconfig_status_type status = XBLCONFIG_VERIFY_ERR;
  
  do
  { 
    if(buffer == NULL)
    {
      status = XBLCONFIG_NULL_PTR_ERR;
      break;      
    }
    
    /* get segment index of config data item */
    status = xcfg_get_config_item_segment(&xcfg_elf_info,
                                          start_offset,
                                          config_item_buffer_size,
                                          &segment_index,
                                          &segment_offset,
                                          &segment_size,
                                          &is_verified );
    if(status != XBLCONFIG_SUCCESS)
    {
      break;
    }
    
    if(is_verified != TRUE)
    {
      // Hashing item for first time    
      status = xcfg_verify_data(start_offset,
                                config_item_buffer_size,
                                segment_index,
                                segment_offset,
                                segment_size,
                                buffer, 
                                xcfg_elf_info.hash_buffer,
                                xcfg_elf_info.hash_buffer_size );

      if(status != XBLCONFIG_SUCCESS)
      {
        break;
      }
    }
  }while(0);
  
  return status;
}


/*===========================================================================
**  Function :  xcfg_verified_config_data_read
** ==========================================================================
*/
/*!
* 
* @brief
*   This function reads XBLConfig item and later proceeds to validate it
*   (eg verify its hash in context of elf image).
*
* @param[in]    
*     start_offset      location of requested item data within partition
*     config_item_size  size of data to be verified
*     buffer            Pointer to data to be verified
*
* @param[out] 
*  None
*        
* @par Dependencies
*   buffer is assumed to be big enough for data to be read and verified
*   
* @retval
*   xblconfig_status_type. XBLCONFIG_SUCCESS if no error else error value
* 
* @par Side Effects
*   None
* 
*/
xblconfig_status_type xcfg_verified_config_data_read
(
  uintnt start_offset,
  uintnt config_item_size,
  void *buffer  
)
{
  xblconfig_status_type status = XBLCONFIG_VERIFY_ERR;
  do
  {
    if( buffer == NULL)
    {
      status = XBLCONFIG_NULL_PTR_ERR;
      break;      
    }

    if(config_item_size == 0)
    {
      status = XBLCONFIG_INVALID_SIZE;
      break;
    }
     
    /* Load config item data into input buffer */
    status = xcfg_get_data(start_offset,
                           config_item_size,
                           buffer);
    if(status != XBLCONFIG_SUCCESS)
    {
      break;
    }
     
     /* Validate the loaded data */
    status = xcfg_validate_data( start_offset,
                                 config_item_size,
                                 buffer
                                );     
  }while(0);
  return status;  
}
