/*=============================================================================

                            XBLConfig Module Sources

GENERAL DESCRIPTION
  This file defines XBL config api's to load data from Ram load module.

Copyright 2017 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/
/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when         who     what, where, why
----------   ---     ----------------------------------------------------------
05/10/17     kpa      rename XBLCONFIG_SIZE_OVERFLOW_ERR to XBLCONFIG_OVERFLOW_ERR
03/03/17     kpa      Initial version
===========================================================================*/
 

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "XBLConfig.h"
#include "XcfgRamElf.h"
#include "boot_util.h"
#include "XcfgUtils.h"

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
*   This function loads data from xbl segment in ram to destinition buffer
* 
* @param[in] 
*  uintnt file_offset          - Start of data to be loaded 
*                                
*  uintnt file_size            - Size of data to be loaded.
*
* @param[out] 
*  void *buffer                - pointer to buffer, loaded with data
* 
*       
* @par Dependencies
*  XBLConfig data segment is assumed to be loaded before calling the api
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
  uintnt phy_addr;
  
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
    if(file_size > XCFG_UINT32_MAX)
    {
      status = XBLCONFIG_OVERFLOW_ERR;
      break;
    }
    
    status = xcfg_ramlib_get_physical_address(file_offset, file_size, &phy_addr);
    
    if(status != XBLCONFIG_SUCCESS)
    {
      break;
    }
    else
    {
      //reset status to default
      status = XBLCONFIG_CONFIG_DATA_LOAD_ERR;
    }
    
    qmemcpy( buffer, (void *)phy_addr, (uint32) file_size);
    status = XBLCONFIG_SUCCESS;

  }while(0);

  return status;
}  
  