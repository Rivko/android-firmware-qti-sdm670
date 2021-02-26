#ifndef XBL_CONFIG_IMAGE_INT_H
#define XBL_CONFIG_IMAGE_INT_H
/*=============================================================================

                      XBLConfig Image Interface Module Sources

GENERAL DESCRIPTION
  This file defines XBL config api's that interface with Elf image
  load module.

Copyright 2016, 2017 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/
/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when         who     what, where, why
----------   ---     ----------------------------------------------------------
03/27/17     kpa      Added xcfg_image_interface_reset
03/03/17     kpa      Added xcfg_load_addr_validation_init, deinit
02/14/17     kpa      Rename XBLCONFIG_ERR_NONE to XBLCONFIG_SUCCESS
                      added xcfg_validate_data
10/14/16     kpa      Initial version
===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "XBLConfig.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/


/*=============================================================================
                              FUNCTION DEFINITIONS
=============================================================================*/


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
xblconfig_status_type xcfg_image_interface_init( void );

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
);

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
);

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
);

/*===========================================================================
**  Function :  xcfg_image_interface_deinit
** ==========================================================================
*/
/*!
* 
* @brief
*   This function deinitializes xbl config image loading interface
*  It free's up structures allocated by image loading interface
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
xblconfig_status_type xcfg_image_interface_deinit( void);

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
xblconfig_status_type xcfg_image_interface_reset( void);

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
  uintnt config_item_size,
  void *buffer  
);

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
);

/*===========================================================================
**  Function :  xcfg_load_addr_validation_init
** ==========================================================================
*/
/*!
* 
* @brief
*   This function initializes load address validation module
*  (such as whitelist checks)
* 
* @param[in]    
*  None
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
xblconfig_status_type xcfg_load_addr_validation_init (void);

/*===========================================================================
**  Function :  xcfg_load_addr_validation_deinit
** ==========================================================================
*/
/*!
* 
* @brief
*   This function deinitializes load address validation module
*  (such as whitelist checks)
* 
* @param[in]    
*  None
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
xblconfig_status_type xcfg_load_addr_validation_deinit (void);

#endif /* XBL_CONFIG_IMAGE_INT_H */
