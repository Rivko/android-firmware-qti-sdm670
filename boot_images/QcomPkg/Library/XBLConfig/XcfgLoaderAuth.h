#ifndef XBL_CONFIG_AUTH_H
#define XBL_CONFIG_AUTH_H

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
02/14/17     kpa      Rename XBLCONFIG_ERR_NONE to XBLCONFIG_SUCCESS
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
xblconfig_status_type xcfg_auth_image_hashtable(void);

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
*     config_item_offset location of requested item data within partition
*     config_item_size   size of data to be verified
*     segment_index      index of segment containing data to be verified
*     segment_offset     offset of segment containing data to be verified
*     segment_size       size of segment containing data to be verified
*     buffer             Pointer to data to be verified
*     hash_buffer        Pointer to xbl config hash buffer
*     hash_buffer_size   size of hash buffer


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
);

#endif /* XBL_CONFIG_AUTH_H */
