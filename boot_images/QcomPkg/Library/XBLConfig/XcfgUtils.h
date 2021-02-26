#ifndef XBL_CONFIG_UTILS_H
#define XBL_CONFIG_UTILS_H

/*=============================================================================

                            XBLConfig Module Sources

GENERAL DESCRIPTION
  This file defines XBL config Utility api's.

Copyright 2017 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/
/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when         who     what, where, why
----------   ---     ----------------------------------------------------------
05/10/17     kpa      Added xcfg_malloc wrapper
04/05/17     kpa      Initial version
===========================================================================*/
 

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/

#include "XBLConfig.h"
#include "stdint.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

#define XCFG_UINT32_MAX     UINT32_MAX

/*=============================================================================
                              FUNCTION DEFINITIONS
=============================================================================*/

/*===========================================================================
**  Function :  xcfg_malloc
** ==========================================================================
*/
/*!
* 
* @brief
*   This function is a wrapper for underlying DAL memory alloc api
* 
* @param[in] 
*   buff_size      Requested size of memory to be alloced
*   xcfg_buff_ptr  Return parameter, callee will populate this to point to the
*                  block of alloced memory, valid only upon DAL_SUCCESS return
*                  code
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
xblconfig_status_type xcfg_malloc
(
  uintnt buff_size,
  void **xcfg_buff_ptr
);

/*===========================================================================
**  Function :  xcfg_free
** ==========================================================================
*/
/*!
* 
* @brief
*   This function is a wrapper for underlying DAL memory free api
* 
* @param[in] 
*   xcfg_buff_ptr            pointer to buffer being freed
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
xblconfig_status_type xcfg_free
(
  void **xcfg_buff_ptr
);

#endif /* XBL_CONFIG_UTILS_H */

  