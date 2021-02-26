#ifndef XBL_CONFIG_RAM_LIB_H
#define XBL_CONFIG_RAM_LIB_H

/*=============================================================================

                            XBLConfig Module Sources

GENERAL DESCRIPTION
  This file defines XBL config api's to support initializing XBLconfig module
  and access a config item.

Copyright 2017 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/
/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when         who     what, where, why
----------   ---     ----------------------------------------------------------
03/08/17     kpa      Initial version
===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "XBLConfig.h"



/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

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
);

#endif /* XBL_CONFIG_RAM_LIB_H */

