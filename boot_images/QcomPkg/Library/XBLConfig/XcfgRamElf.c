/*=============================================================================

                            XBLConfig Module Sources

GENERAL DESCRIPTION
  This file defines XBL config api's that interface with XBLRamLib's elf image
  load module.

Copyright 2017 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/
/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when         who     what, where, why
----------   ---     ----------------------------------------------------------
05/10/17     kpa      rename XBLCONFIG_SIZE_OVERFLOW_ERR to XBLCONFIG_OVERFLOW_ERR
05/03/17     kpa      update integer overflow checks
01/24/17     kpa      Initial version
===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "XBLConfig.h"
#include "boot_elf_loader.h"
#include "XcfgRamElf.h"
#include "XcfgFlashInterface.h"
#include "DALSys.h"
#include "boot_util.h"
#include "XcfgUtils.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

/* XBLConfig elf info structure containing pointer to 
hash buffer, elf header and segment info for ram lib */
static xcfg_ram_elf_info_type xcfg_ram_elf_info;

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
    *max_logical_blk_ptr = xcfg_ram_elf_info.max_phnum;
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
*   Note: A logical block for elf file type is a segment.
*         The api also builds up a list of all blocks/segments
*         present for image
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
  xcfg_ram_segment_info_type *xcfg_segment_info_ptr;
  boolean seg_status = FALSE;
  uintnt blk_destn_addr =0;
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
                               (void *)xcfg_ram_elf_info.elf_hdr,
                               (void*)xcfg_ram_elf_info.prog_hdr,
                               index,
                               blk_size_ptr,
                               is_blk_loadable_ptr,
                               blk_offset_ptr,
                               &blk_destn_addr
                               );
    if(seg_status == FALSE)
    {
      status = XBLCONFIG_META_DATA_LOAD_ERR;
      break;
    }
    
    xcfg_segment_info_ptr = xcfg_ram_elf_info.segment_info_ptr;
    
    /*while searching for meta data also keep track of segment offset and size */ 
    xcfg_segment_info_ptr[index].offset = *blk_offset_ptr;
    xcfg_segment_info_ptr[index].size = *blk_size_ptr;
    xcfg_segment_info_ptr[index].is_verified = FALSE;   
    xcfg_segment_info_ptr[index].p_addr = blk_destn_addr;
    xcfg_segment_info_ptr[index].is_valid = TRUE;

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
 return xcfg_get_data(block_offset, block_size, buffer);
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
    status = xcfg_free((void**)&(xcfg_ram_elf_info.segment_info_ptr));
    
    /* if any buffer freeing failed, indicate it to caller */    
    if(status != XBLCONFIG_SUCCESS)
      break; 

    status = XBLCONFIG_SUCCESS;

  }while(0);

  return status;  
}

/*===========================================================================
**  Function :  xcfg_ram_get_config_item_segment
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
*   phy_addr                  pointer to data block destination address
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
static xblconfig_status_type xcfg_ram_get_config_item_segment
(
  xcfg_ram_elf_info_type *xcfg_elf_info_ptr,
  uintnt config_item_start_offset,
  uintnt config_item_size,
  uintnt *segment_index,
  uintnt *segment_offset,
  uintnt *segment_size,
  uintnt *phy_addr
)  
{
  xblconfig_status_type status = XBLCONFIG_VERIFY_ERR;  
  uintnt index =0 ;
  xcfg_ram_segment_info_type *xcfg_segment_info_ptr;
  
  do
  {
    if( segment_index == NULL || segment_offset == NULL || 
        segment_size == NULL || phy_addr == NULL || xcfg_elf_info_ptr == NULL
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
      /*Check for integer overflow */
      if((xcfg_segment_info_ptr[index].offset + xcfg_segment_info_ptr[index].size)
         < xcfg_segment_info_ptr[index].offset)
      {
       status = XBLCONFIG_OVERFLOW_ERR;
        break;
      }
      if((config_item_start_offset + config_item_size)
         < config_item_start_offset)
      {
       status = XBLCONFIG_OVERFLOW_ERR;
        break;
      }      
      
      if( xcfg_segment_info_ptr[index].offset <= config_item_start_offset &&
          (xcfg_segment_info_ptr[index].offset + xcfg_segment_info_ptr[index].size)
            >= (config_item_start_offset + config_item_size) )
      {
        /* Config data fully contained within segment*/
        *segment_index = index;
        *segment_offset = xcfg_segment_info_ptr[index].offset;
        *segment_size = xcfg_segment_info_ptr[index].size;
        *phy_addr = xcfg_segment_info_ptr[index].p_addr;
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
  xcfg_ram_elf_info_type *xcfg_elf_info_ptr,
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
    
    if (segment_index >= xcfg_ram_elf_info.max_phnum)
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
*  prior to data access 
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
  uintnt hash_segment_size  = 0;
  boolean hash_segment_status = FALSE;
  
  xcfg_ram_elf_info_type *xcfg_elf_info_ptr = &xcfg_ram_elf_info;
  uintnt max_phnum = 0;
  uintnt *max_phnum_ptr;
  
  do
  {

    /*First locate hash segment */
    if(xcfg_ram_elf_info.elf_hdr == NULL || xcfg_ram_elf_info.prog_hdr == NULL ||
       xcfg_ram_elf_info.hash_buffer == NULL )
    {
      /*Ramlib data structs for elf and prog headers are expected to be
        set before interface init. In case of ramlib the data is already available
        since xbl config is part of xbl elf.
      */
      status = XBLCONFIG_RAM_ELF_INFO_INIT_ERR;
      break;
    }  
    /* Hash segment shall have been already authenticated as a part of
       XBL elf load since XBLConfig segments for Ram Lib usecase are part of 
       XBL elf. No need to explicitly reauthenticate
    */

    /* Get hash segment data size of xbl config image */
    hash_segment_status = boot_get_elf_hash_segment_size(
                                    xcfg_ram_elf_info.elf_hdr,
                                    xcfg_ram_elf_info.prog_hdr,
                                    &hash_segment_size);
    if(hash_segment_status != TRUE)
    {
      status = XBLCONFIG_HASH_SEG_ERR;
      break;
    }
   
    xcfg_elf_info_ptr->hash_buffer_size = hash_segment_size;
    
    /* Configure internal data structs */    
    if ( TRUE != boot_get_elf_prog_header_count(xcfg_ram_elf_info.elf_hdr,
                                                &max_phnum))
    {
      /* Could not get max prog header count from elf header */
      status = XBLCONFIG_META_DATA_LOAD_ERR;
      break;
    }
    
    /*overflow check */
    if(sizeof(xcfg_ram_segment_info_type) > (XCFG_UINT32_MAX / max_phnum))
    {
      status = XBLCONFIG_OVERFLOW_ERR;
      break;
    }    
    
    /* Prepare segment info data struct, keeping track of all segment
       offset and sizes
    */
    status = xcfg_malloc( max_phnum * sizeof(xcfg_ram_segment_info_type),
                          (void **)&(xcfg_elf_info_ptr->segment_info_ptr));
    if(status != XBLCONFIG_SUCCESS)
    {
      break;
    }
     
    qmemset( (void *)(xcfg_elf_info_ptr->segment_info_ptr), 0x0,
             (uint32)(max_phnum * sizeof(xcfg_ram_segment_info_type)));

    max_phnum_ptr = &xcfg_elf_info_ptr->max_phnum;
    *max_phnum_ptr = max_phnum;
    
    status = XBLCONFIG_SUCCESS;
    
  }while(0);

  /* Free malloced memory in case of error */
  if(status != XBLCONFIG_SUCCESS)  
  {
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
*   This function is a stub to verify the loaded XBLConfig item.
*   It is an intermediate api to add image meta data (eg segment info) 
*   needed by authentication api's which do the actual validation.
* 
* @param[in]    
*     start_offset      location of requested item data within partition
*     config_item_size  Pointer to size of data to be verified
*     buffer            Pointer to data to be verified
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
  /*This function is a stub to verify the loaded XBLConfig item
  Since the config segment is already present in memory and 
  authenticated by the time xbl ram lib runs, it not necessary
  to re-authenticate or validate the data again */
  return XBLCONFIG_SUCCESS;
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
   
  }while(0);
  return status;  
}

/*===========================================================================
**  Function :  xblconfig_ramlib_init
** ==========================================================================
*/
/*!
* 
* @brief
*   This function sets elf meta info consumed by xbl config ramlib
*   
* @param[in]
*    elf_hdr            pointer to elf header
*    prog_hdr           pointer to prog header table
*    hash_buffer        pointer to buffer containing hash segment.
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
xblconfig_status_type xblconfig_ramlib_init
(
  void *elf_hdr,
  void *prog_hdr,
  void *hash_buffer
)
{
  xblconfig_status_type status = XBLCONFIG_INIT_ERR;
  do
  {
    if(elf_hdr == NULL || prog_hdr == NULL ||
       hash_buffer == NULL)
    {
      status = XBLCONFIG_NULL_PTR_ERR;
      break;      
    }
    xcfg_ram_elf_info.hash_buffer = hash_buffer;
    xcfg_ram_elf_info.elf_hdr = elf_hdr;
    xcfg_ram_elf_info.prog_hdr = prog_hdr;
    
    status = XBLCONFIG_SUCCESS;
    
  }while(0);
  return status;
}

/*===========================================================================
**  Function :  xcfg_ramlib_get_physical_address
** ==========================================================================
*/
/*!
* 
* @brief
*   This function gives physical address corresponding to given offset for
*   config data that is already loaded in ram
*   
* @param[in]
*    blk_offset         data block offset that needs to be translated
*                       to phy addr where its already loaded
*    blk_size           data block size
*
* @param[out] 
*    physical_addr      pointer to elf header
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
xblconfig_status_type xcfg_ramlib_get_physical_address
(
  uintnt blk_offset,
  uintnt blk_size,
  uintnt *physical_addr
)
{
  xblconfig_status_type status = XBLCONFIG_INIT_ERR;
  uintnt segment_index, segment_offset, segment_size, phy_addr;
  do
  {
    if(physical_addr == NULL || blk_size == 0)
    {
      status = XBLCONFIG_NULL_PTR_ERR;
      break;        
    }
    /* get segment index of config data item */
    status = xcfg_ram_get_config_item_segment(&xcfg_ram_elf_info,
                                          blk_offset,
                                          blk_size,
                                          &segment_index,
                                          &segment_offset,
                                          &segment_size,
                                          &phy_addr );
    if(status != XBLCONFIG_SUCCESS)
    {
      break;
    }
    else
    {
      /*reinit status to default */
      status = XBLCONFIG_INIT_ERR;      
    }
    
    /*Ensure the block offset is bigger than or equal to its 
      segment start, although the above get_config_item_segment
      should have also checked the same
    */
    if(segment_offset > blk_offset)
    {
      status = XBLCONFIG_INVALID_SIZE;
      break;
    }
    
    /* get the location in memory of requested block */
    *physical_addr = phy_addr + (blk_offset - segment_offset);
    
    if(*physical_addr < phy_addr)
    {
      status = XBLCONFIG_OVERFLOW_ERR;
      break;
    }    
    status = XBLCONFIG_SUCCESS;
    
  }while(0);
  
  return status;
}

