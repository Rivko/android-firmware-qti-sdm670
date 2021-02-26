/*=============================================================================

                            XBLConfig Module Sources

GENERAL DESCRIPTION
  This file defines XBL config api's to support initializing XBLconfig module
  and access a config item.

Copyright 2016,2017 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/
/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when         who     what, where, why
----------   ---     ----------------------------------------------------------
10/16/17     kpa      Replace strcmp by strncmp
05/10/17     kpa      rename XBLCONFIG_SIZE_OVERFLOW_ERR to XBLCONFIG_OVERFLOW_ERR
05/03/17     kpa      update meta data buffer pointer used for validation.
03/15/17     kpa      fix memory leaks in error scenario, update get data api's
03/02/17     kpa      use heap instead of stack variable for xbl config cookie
02/14/17     kpa      code cleanup
02/10/17     kpa      Update error checks in xcfg_load_meta_data
01/20/17     kpa      Updates to metadata header and config item parsing logic
10/14/16     kpa      Initial version
===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "XBLConfig.h"
#include "XcfgImageInterface.h"
#include "string.h"
#include "boot_util.h"
#include "XcfgUtils.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/
#define EIGHT_BYTE_ALIGN_SIZE 0x8

/*Buffer to hold XBL config meta data header followed by its table */
static void *xcfg_metadata_buff = NULL;
static uintnt xcfg_metadata_start_offset =0;
static boolean is_xblconfig_initialized = FALSE;

/* List to manage variable length config entries */
static xcfg_parsed_metadata_table_entry_type *config_entry_list = NULL;
static uintnt max_config_entries =0;

/* Data structure to keep track of open config items */
static xcfg_config_handle_type *xcfg_handle_list = NULL;
static uint32 xcfg_handle_max_count =0, xcfg_handle_count =0;
#define INITIAL_CONFIG_HANDLES  5
#define SIZE_INCREASE_MULTIPLE 2

/*=============================================================================
                              FUNCTION DEFINITIONS
=============================================================================*/

/*===========================================================================
**  Function :  xcfg_load_meta_data
** ==========================================================================
*/
/*!
* 
* @brief
*   This function finds and loads xblconfig meta data
* 
* @param[out] 
*   xcfg_elf_metadata_buff_ptr      pointer to buffer loaded with meta data
*   xcfg_metadata_start_offset_ptr  pointer to store meta data start offset
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
static xblconfig_status_type xcfg_load_meta_data
(
  void **xcfg_elf_metadata_buff_ptr,
  uintnt *xcfg_metadata_start_offset_ptr
)
{
  uint8 *xcfg_cookie_ptr = NULL;
  xblconfig_status_type status = XBLCONFIG_INIT_ERR;
  uintnt max_logical_blocks = 0, index =0, cookie_size =0;
  uintnt blk_size =0, blk_offset =0, meta_data_seg_size =0;
  boolean is_blk_loadable = FALSE, meta_data_found = FALSE;
  
  do
  {
    status = xcfg_image_interface_init();
    if(status != XBLCONFIG_SUCCESS)
      break;    

    status =  xcfg_image_get_max_logical_block_count(&max_logical_blocks);
    if(status != XBLCONFIG_SUCCESS)
      break;
    
     /* Allocate temporary buffer for reading cookie */
     cookie_size = sizeof(uint8)*XCFG_LOADER_MAGIC_COOKIE_SIZE;
     status = xcfg_malloc(cookie_size, (void **)(&xcfg_cookie_ptr));  
     if(status != XBLCONFIG_SUCCESS)
     {
       break;
     }

    for(index=0; index < max_logical_blocks; index++)
    {
      status = xcfg_image_get_block_info( index,
                                          &blk_size,
                                          &is_blk_loadable,
                                          &blk_offset
                                        );
      if(status != XBLCONFIG_SUCCESS)
        break;
      
      /* Search for meta data in current block/segment */
      if(is_blk_loadable == TRUE && blk_size > 0 && meta_data_found == FALSE)
      {
        /* Read first four bytes to check if its a xbl Config data segment */
        status = xcfg_get_block_data(blk_offset, cookie_size, (void*)xcfg_cookie_ptr);
        /*Note: 'Block' is a generic term to denote fileformat's data items to be checked 
          (which is used to contain xblconfig blob. In case of elf file format its a segment
        */
        if(status != XBLCONFIG_SUCCESS)
          break;
          
        if( 
           xcfg_cookie_ptr[XCFG_LOADER_MAG0_INDEX] == XCFG_LOADER_MAG0 &&
           xcfg_cookie_ptr[XCFG_LOADER_MAG1_INDEX] == XCFG_LOADER_MAG1 &&
           xcfg_cookie_ptr[XCFG_LOADER_MAG2_INDEX] == XCFG_LOADER_MAG2 &&
           xcfg_cookie_ptr[XCFG_LOADER_MAG3_INDEX] == XCFG_LOADER_MAG3        
         )
        {
          status = xcfg_malloc(blk_size, (xcfg_elf_metadata_buff_ptr));  
          if(status != XBLCONFIG_SUCCESS)
          {
            break;
          }
        
          /* Load the xbl Config data segment */
          status = xcfg_get_block_data(blk_offset, blk_size, (*xcfg_elf_metadata_buff_ptr)); 
          if(status != XBLCONFIG_SUCCESS)
            break;          
          
          /* Store the offset of meta data start. That shall later be needed to get individual item 
             offsets */
          *xcfg_metadata_start_offset_ptr = blk_offset;
          meta_data_seg_size = blk_size;
          
          if (status == XBLCONFIG_SUCCESS)
          {
            meta_data_found = TRUE;         
          }
        }
      }
    }
    
    status = xcfg_free((void**)&xcfg_cookie_ptr);
    if(status != XBLCONFIG_SUCCESS)
    {
      break;
    }
    
    if(status == XBLCONFIG_SUCCESS  )
    {
      if(*xcfg_elf_metadata_buff_ptr == NULL || index != max_logical_blocks)
      {
        /* meta data was not found or all blocks were not processed 
        */
        status = XBLCONFIG_META_DATA_LOAD_ERR;
      }
      else
      {
        status = xcfg_validate_data(*xcfg_metadata_start_offset_ptr,
                                     meta_data_seg_size,
                                     *xcfg_elf_metadata_buff_ptr
                                   );
        if(status != XBLCONFIG_SUCCESS)
        {
          break;
        }
      }
    }
    xcfg_image_interface_reset();

  }while(0);

  /* Free malloced memory in case of error */
  if(status != XBLCONFIG_SUCCESS)
  {
    /* Since this is memory leak cleanup after error, ignore 
       the free api status and continue freeing all buffer's
    */          
    xcfg_free((void**)&xcfg_elf_metadata_buff_ptr);
    xcfg_free((void**)&xcfg_cookie_ptr);
  }  
  return status;  
}

/*===========================================================================
**  Function :  xcfg_populate_config_item_list
** ==========================================================================
*/
/*!
* 
* @brief
*   This function initializes the data structure that maintains config item
*   list.
* 
* @param[in] 
*  xcfg_parsed_metadata_table_entry_type    Pointer to config entry list 
*  xcfg_metadata_header_type                Pointer to meta data header
*        
* @par Dependencies
*   None
*   
* @retval
*   xblconfig_status_type XBLCONFIG_SUCCESS if no error else error value
* 
* @par Side Effects
*   None
* 
*/
static xblconfig_status_type xcfg_populate_config_item_list
(
  xcfg_parsed_metadata_table_entry_type *config_entry_list, 
  xcfg_metadata_header_type *xcfg_header
)
{
  xblconfig_status_type status = XBLCONFIG_INIT_ERR;
  uintnt index =0, extended_bytes =0, entry_ptr_val =0;

  xcfg_metadata_table_entries_type *item_entry_ptr;
  do
  {
    if (config_entry_list == NULL || xcfg_header == NULL)
    {
      status = XBLCONFIG_NULL_PTR_ERR;
      break;
    }
    item_entry_ptr = (xcfg_metadata_table_entries_type *) 
                        ((uintnt)xcfg_header + sizeof(xcfg_metadata_header_type));
    
    // Populate the config item meta list with pointers to each entry
    for(index =0; index < max_config_entries; index++)
    {
      config_entry_list[index].entry_ptr = item_entry_ptr;
     
      extended_bytes = (uintnt)item_entry_ptr->config_name_len % EIGHT_BYTE_ALIGN_SIZE; 
      if (extended_bytes != 0)
      {
        // name length is not a multiple of byte size
        extended_bytes = EIGHT_BYTE_ALIGN_SIZE - extended_bytes;
      }
     
     // Point to next entry in the meta data blob
     entry_ptr_val = (uintnt) item_entry_ptr
                     + sizeof(xcfg_metadata_table_entries_type)
                     + (uintnt)item_entry_ptr->config_name_len
                     + extended_bytes
                     - 0x1 ;/* substract size of first byte that is included
                              in xcfg_metadata_table_entries_type */

    if(entry_ptr_val < (uintnt)item_entry_ptr)
    {
      status = XBLCONFIG_OVERFLOW_ERR;
      break;
    }
                              
     item_entry_ptr = (xcfg_metadata_table_entries_type *)entry_ptr_val;     
    }
    status = XBLCONFIG_SUCCESS;
  
  }while(0);
  return status;
}

/*===========================================================================
**  Function :  xcfg_handle_init
** ==========================================================================
*/
/*!
* 
* @brief
*  Function to initialize data structure maintaining open handles.
*  Everytime this api is called, if handle list is running out of space
*  it shall double its size.
* 
* @param[out] 
*  xcfg_config_handle_type **xcfg_handle_ptr         Pointer to config handle list
*  uint32 *handle_max_count_ptr                      Pointer to max handle count
*  uint32 *handle_count_ptr                          Pointer to count of handles in use
*        
* @par Dependencies
*   None
*   
* @retval
*   xblconfig_status_type XBLCONFIG_SUCCESS if no error else error value
* 
* @par Side Effects
*   None
* 
*/
static xblconfig_status_type xcfg_handle_init
(
  xcfg_config_handle_type **xcfg_handle_ptr,
  uint32 *handle_max_count_ptr,
  uint32 *handle_count_ptr
)
{
  xblconfig_status_type status = XBLCONFIG_HANDLE_ERR;
  xcfg_config_handle_type *xcfg_handle_temp_ptr = NULL;
  uint32 handle_list_size =0;
  boolean handle_init_flag = FALSE;
  do
  {
    if( handle_max_count_ptr == NULL || handle_count_ptr == NULL ||
        xcfg_handle_ptr == NULL )
    {
      status = XBLCONFIG_NULL_PTR_ERR;
      break;
    }
    if(*handle_max_count_ptr != 0)
    {
      if(*xcfg_handle_ptr == NULL)
      {
        // Null pointer for handle list but max handle count being
        // non-zero, it should have been initialized earlier.
        // looks incorrect.
        status = XBLCONFIG_NULL_PTR_ERR;
        break;
      }
      
      if(*handle_count_ptr == *handle_max_count_ptr)
      {
        // Overflow check. at this point (*handle_max_count_ptr) is not 0
        if(sizeof(xcfg_config_handle_type) > (XCFG_UINT32_MAX / (*handle_max_count_ptr)))
        {
          status = XBLCONFIG_OVERFLOW_ERR;
          break;
        }
        
        handle_list_size = (*handle_max_count_ptr) * sizeof(xcfg_config_handle_type);
        
        // List of handles exhausted hence double the allocated memory for list
        if(handle_list_size > (XCFG_UINT32_MAX / SIZE_INCREASE_MULTIPLE))
        {
          status = XBLCONFIG_OVERFLOW_ERR;
          break;
        }        
        
        status = xcfg_malloc((handle_list_size * SIZE_INCREASE_MULTIPLE), (void **)&xcfg_handle_temp_ptr);
        if(status != XBLCONFIG_SUCCESS)
        {
          break;
        }
        
        qmemset((void *)xcfg_handle_temp_ptr, 0x0, (handle_list_size * SIZE_INCREASE_MULTIPLE));      
        
        //relocate list to new memory region
        qmemcpy((void *)xcfg_handle_temp_ptr, (void *)*xcfg_handle_ptr, 
                handle_list_size);
        
        status = xcfg_free((void**)xcfg_handle_ptr);
        if(status != XBLCONFIG_SUCCESS)
        {
          break;
        }        
        *xcfg_handle_ptr = xcfg_handle_temp_ptr;
        *handle_max_count_ptr = *handle_max_count_ptr * 2;
        status = XBLCONFIG_SUCCESS;
      }
    }
    else
    {
      //Since max handle count is zero, this is first time init
      if(*xcfg_handle_ptr != NULL)
        break;

      *handle_max_count_ptr = INITIAL_CONFIG_HANDLES;
      handle_list_size = (*handle_max_count_ptr) * sizeof(xcfg_config_handle_type);           
      
      status = xcfg_malloc(handle_list_size,(void **)xcfg_handle_ptr);
      if(status != XBLCONFIG_SUCCESS)
      {
        break;
      }

      qmemset((void *)*xcfg_handle_ptr, 0x0, handle_list_size);
      *handle_count_ptr =0;   
       handle_init_flag =TRUE;
      status = XBLCONFIG_SUCCESS;      
    }
  
  }while(0);
  
  /* Free malloced memory in case of error */
  if(status != XBLCONFIG_SUCCESS)
  {
    xcfg_free((void**)&xcfg_handle_temp_ptr);

    if(xcfg_handle_ptr != NULL)
    {
      if(*xcfg_handle_ptr != NULL && handle_init_flag == TRUE)
      {
        /* xcfg_handle_ptr was malloced in this invocation of api */
        xcfg_free((void**)xcfg_handle_ptr);
      }
    }
  }  
  return status;

}

/*===========================================================================
**  Function :  xcfg_handle_allocate
** ==========================================================================
*/
/*!
* 
* @brief
*  Function to initialize data structure maintaining open handles.
*  Everytime this api is called, if handle list is running out of space
*  it shall double its size.
* 
* @param[in] 
*  xcfg_config_handle_type **xcfg_handle_ptr  pointer to handle list
*  uint32 *handle_max_count                   Pointer to count of max 
*                                             handles available
*
* @param[out] 
*  uint32 *handle_count                 Pointer to current open handle count
*  uint32 *free_handle_index            Pointer filled with available handle
*                                       index
*        
* @par Dependencies
*   None
*   
* @retval
*   xblconfig_status_type XBLCONFIG_SUCCESS if no error else error value
* 
* @par Side Effects
*   None
* 
*/
static xblconfig_status_type xcfg_handle_allocate
(
  xcfg_config_handle_type **xcfg_handle_ptr,
  uint32 *handle_max_count,
  uint32 *handle_count_ptr,
  uint32 *free_handle_index  
)
{
  xblconfig_status_type status = XBLCONFIG_HANDLE_ERR;
  uintnt index;
  do
  { 
    if( (*handle_count_ptr) >= (*handle_max_count))
    {
      // no more space to allocate handle
      status = xcfg_handle_init(xcfg_handle_ptr, handle_max_count,
                                handle_count_ptr);
                                
      if(status != XBLCONFIG_SUCCESS || ((*handle_count_ptr) >= (*handle_max_count)))
        break;     
    }
    //allocate handle from list. Find first free entry.
    for(index =0; index < (*handle_max_count) ; index++)
    {
      if((*xcfg_handle_ptr)[index].is_valid == FALSE)
      {
        (*xcfg_handle_ptr)[index].is_valid = TRUE;
        break;
      }
    }
    if(index < (*handle_max_count))
    {
      *free_handle_index = index;
       status = XBLCONFIG_SUCCESS;
    }
    else
    {
      status = XBLCONFIG_HANDLE_ERR;
    }
    
  }while(0);
  return status;
}

/*===========================================================================
**  Function :  xblconfig_init
** ==========================================================================
*/
/*!
* 
* @brief
*   This function initializes xblconfig module
* 
* @param[in] 
*   None
*        
* @par Dependencies
*   None
*   
* @retval
*   xblconfig_status_type XBLCONFIG_SUCCESS if no error else error value
* 
* @par Side Effects
*   None
* 
*/
xblconfig_status_type xblconfig_init(void)
{
  xblconfig_status_type status = XBLCONFIG_INIT_ERR;
  /* XBLConfig header structure */
  xcfg_metadata_header_type *xcfg_header;

  do
  {
    if(is_xblconfig_initialized == TRUE)
    {
      /*XBL config lib is already initialized */
      status = XBLCONFIG_SUCCESS;
      break;      
    }
    
    /*Initialize module for validating buffers to which data can be loaded 
      from storage, example those from heap 
    */    
    status = xcfg_load_addr_validation_init();
    if(status != XBLCONFIG_SUCCESS)
      break;
    
    //Load and authenticate xbl config hash table and get meta data blob
    status = xcfg_load_meta_data( &xcfg_metadata_buff,
                                  &xcfg_metadata_start_offset);
                                      
    if(status != XBLCONFIG_SUCCESS)
      break;
    
    //Validate meta data header
    xcfg_header = (xcfg_metadata_header_type *)xcfg_metadata_buff;
    
    if(xcfg_header->major_version != XBL_CONFIG_MAJOR_VERSION &&
       xcfg_header->minor_version != XBL_CONFIG_MINOR_VERSION)
    {
      // unsupported meta data format.
      break;
    }
    max_config_entries = xcfg_header->number_of_entries;
    
   // Parse the meta data to index the entries.
   status = xcfg_malloc((max_config_entries * sizeof(xcfg_parsed_metadata_table_entry_type)),
                        (void **)&config_entry_list);
   if(status != XBLCONFIG_SUCCESS)
   {
     break;
   }
   
    status = xcfg_populate_config_item_list(config_entry_list, xcfg_header);
    if(status != XBLCONFIG_SUCCESS)
      break;
      
    //Initialize list for open handles
    status = xcfg_handle_init(&xcfg_handle_list, &xcfg_handle_max_count,
                              &xcfg_handle_count); 
    if(status != XBLCONFIG_SUCCESS)
      break;     
 
    /*If we reach this point initialization went ok */
    is_xblconfig_initialized = TRUE;
    
    status = xcfg_load_addr_validation_deinit();
    
  }while(0);
  
  /* Free malloced memory in case of error */
  if(status != XBLCONFIG_SUCCESS)
  {
    xcfg_free((void**)&config_entry_list);
  }
  
  return status;
} /* xblconfig_init */


/*===========================================================================
**  Function :  xblconfig_open
** ==========================================================================
*/
/*!
* 
* @brief
*   This function parses xbl config meta data to locate config item and
*   returns a config handle if found, along with its size
*
* @param[in]    config_name       Null terminated config item name string
*
* @param[out]   config_handle     Config item handle, if found
*               config_item_size  Total size of config item
*        
* @par Dependencies
*   XBLConfig module should have been initialized via xblconfig_init
*   before calling xblconfig_open
*   
* @retval
*   xblconfig_status_type   Status of operation
* 
* @par Side Effects
*   None
* 
*/
xblconfig_status_type xblconfig_open
(
  const char *config_name,
  uint32 *config_handle,
  uint32 *config_item_size
)
{
  xcfg_metadata_table_entries_type *item_entry_ptr;
  xblconfig_status_type status = XBLCONFIG_OPEN_ERR;
  uintnt index;
  
  do
  {
    if( config_name == NULL || config_handle == NULL ||
        config_item_size == NULL )
    {
      status = XBLCONFIG_NULL_PTR_ERR;
      break;
    }
    if(is_xblconfig_initialized == FALSE)
    {
      status = XBLCONFIG_UNINITIALIZED;
      break;
    }
    
    // parse config table entries to locate item.
    for(index=0; index < max_config_entries; index++)
    {
      item_entry_ptr = config_entry_list[index].entry_ptr;
      if(strncmp(config_name, &item_entry_ptr->config_name_start, item_entry_ptr->config_name_len) == 0)
      {
        //Config entry is found. Allocate handle
        status =  xcfg_handle_allocate(&xcfg_handle_list, &xcfg_handle_max_count,
                                       &xcfg_handle_count, config_handle);
        if(status != XBLCONFIG_SUCCESS)
          break;
        
        //Update handle with config data info
        xcfg_handle_list[(*config_handle)].config_entry_index = index;
        *config_item_size = item_entry_ptr->size;
        xcfg_handle_count++;
        break;
      }
    }
    if(index == max_config_entries)
    {
      status = XBLCONFIG_OPEN_ERR;
    }
  }while(0);
  return status;
}/* xblconfig_open */

/*===========================================================================
**  Function :  xblconfig_read
** ==========================================================================
*/
/*!
* 
* @brief
*   This function copies config item data equal to input "size" from read offset
*   into provided buffer
* 
* @param[in]    config_handle   Handle to identify config item to be read
*               read_offset     Offset within config item to start read
*               size            Size of data to be read
*
* @param[out]   buffer          Data buffer provided by callee for reading data
*                    
* @par Dependencies
*   NOTE:   The user of xblconfig_read api has to allocate memory and
*           point it via buffer for loading data.
*   
* @retval
*   xblconfig_status_type   Status of operation
* 
* @par Side Effects
*   None
* 
*/
xblconfig_status_type xblconfig_read
( 
  const uint32 config_handle,
  uint32 read_offset,
  uint32 size,
  void *buffer
)
{
  xblconfig_status_type status = XBLCONFIG_READ_ERR;
  xcfg_metadata_table_entries_type *item_entry_ptr;
  uintnt parsed_entry_index =0, test_offset =0;
  uint32 config_item_start =0, test_size =0;
  do
  {
    if(is_xblconfig_initialized == FALSE)
    {
      status = XBLCONFIG_UNINITIALIZED;
      break;
    }
    
    if(buffer == NULL)
    {
      status = XBLCONFIG_NULL_PTR_ERR;
      break;
    }
    if(size == 0)
    {
      status = XBLCONFIG_INVALID_SIZE;
      break;
    }
    if((config_handle >= xcfg_handle_max_count) ||
        xcfg_handle_list[config_handle].is_valid == FALSE)
    {
      break;
    }
    
    /* check for overflow */
    test_size = read_offset + size;
    if(test_size < size)
    {
      status = XBLCONFIG_OVERFLOW_ERR;
      break;
    }      

    /*Initialize module for validating buffers to which data can be loaded 
      from storage example those from heap 
    */    
    status = xcfg_load_addr_validation_init();
    if(status != XBLCONFIG_SUCCESS)
      break;
  
    parsed_entry_index = xcfg_handle_list[config_handle].config_entry_index;
    item_entry_ptr = config_entry_list[parsed_entry_index].entry_ptr;
    config_item_start = item_entry_ptr->offset_from_meta_start;
    
    /* ensure data to be read is within bounds. read offset + requested size
       will not exceed max config item size
    */
    if(read_offset > item_entry_ptr->size || 
      (read_offset + size) > item_entry_ptr->size)
    {
      status = XBLCONFIG_INVALID_SIZE;
      break;
    }
    
    test_offset =  xcfg_metadata_start_offset + config_item_start + read_offset;
    if(test_offset < xcfg_metadata_start_offset)
    {
      status = XBLCONFIG_OVERFLOW_ERR;
      break;
    }
                    
    status = xcfg_verified_config_data_read(test_offset, (uintnt)size, buffer);

    if(status != XBLCONFIG_SUCCESS)
      break;

    status = xcfg_load_addr_validation_deinit();

  }while(0);  
  return status;
}/* xblconfig_read */

/*===========================================================================
**  Function :  xblconfig_close
** ==========================================================================
*/
/*!
* 
* @brief
*   This function free's config handle to be closed(sets to 0)
* 
* @param[in]  config_handle   Handle to be closed
*   None
*        
* @par Dependencies
*   NOTE:   The user of xblconfig_close api has to free memory 
*           used for reads associated with the handle to be closed.
*   
* @retval
*   xblconfig_status_type   Status of operation
* 
* @par Side Effects
*   None
* 
*/
xblconfig_status_type xblconfig_close (uint32 config_handle)
{
  xblconfig_status_type status = XBLCONFIG_CLOSE_ERR;
  
  do
  {
    if((config_handle >= xcfg_handle_max_count) ||
        xcfg_handle_count ==0)
    {
      break;
    }
    if(is_xblconfig_initialized == FALSE)
    {
      status = XBLCONFIG_UNINITIALIZED;
      break;
    }
    
    if(xcfg_handle_list[config_handle].is_valid != FALSE)
    {
      /*Ensure handle is not already freed to avoid handle count
        underrun across multiple api invocations for same handle 
      */
      xcfg_handle_list[config_handle].config_entry_index = 0;
      xcfg_handle_list[config_handle].is_valid = FALSE;
      xcfg_handle_count--;    
      status = XBLCONFIG_SUCCESS;
    }
  }while(0);
  return status;
} /* xblconfig_close */

/*===========================================================================
**  Function :  xblconfig_deinit
** ==========================================================================
*/
/*!
* 
* @brief
*   This function deinitializes xblconfig module. Free's all internal allocated
*   buffers
* 
* @param[in] 
*   None
*        
* @par Dependencies
*   None
*   
* @retval
*   xblconfig_status_type   Status of operation
* 
* @par Side Effects
*   None
* 
*/
xblconfig_status_type xblconfig_deinit(void)
{
  xblconfig_status_type status = XBLCONFIG_DEINIT_ERR;

  do
  {
    /* Free all allocated buffers */
    status = xcfg_free((void**)&xcfg_metadata_buff);   
    if(status != XBLCONFIG_SUCCESS)
      break;

    status = xcfg_free((void**)&config_entry_list);
    if(status != XBLCONFIG_SUCCESS)
      break;
    
    status = xcfg_free((void**)&xcfg_handle_list);    
    if(status != XBLCONFIG_SUCCESS)
      break;    
   
    status = xcfg_image_interface_deinit();
    if(status != XBLCONFIG_SUCCESS)
      break;
    
    /* reset config item handle counts*/
    xcfg_handle_max_count =0;
    xcfg_handle_count =0;
    
    max_config_entries =0;
    
    /* If we reach this point all went ok. */
    is_xblconfig_initialized = FALSE;
    status = XBLCONFIG_SUCCESS;
   
  }while(0);
  return status;
} /* xblconfig_deinit */
