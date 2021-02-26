#ifndef XBL_CONFIG_FLASH_INTERFACE_H
#define XBL_CONFIG_FLASH_INTERFACE_H

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
03/15/17     kpa      Update xcfg_get_data function parameters
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
xblconfig_status_type xcfg_get_data(uintnt file_offset, uintnt file_size, void *buffer);

#endif /* XBL_CONFIG_FLASH_INTERFACE_H */

  