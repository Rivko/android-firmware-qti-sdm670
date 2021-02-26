/*=============================================================================

                            XBLConfig Module Sources

GENERAL DESCRIPTION
  This file contains common authentication routines used by xblconfig lib 

Copyright 2017 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/
/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when         who     what, where, why
----------   ---     ----------------------------------------------------------
07/25/17     yps     Added external heap base and size
05/11/17     kpa     Initial version
===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/

#include "XBLConfig.h"
#include "boot_whitelist_prot.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

/*Define whitelist for validating load addresses */
extern uintnt  Image$$BOOT_INTERNAL_HEAP$$Base;
extern uintnt  Image$$BOOT_INTERNAL_HEAP$$Length;
extern uintnt  Image$$BOOT_EXTERNAL_HEAP$$Base;
extern uintnt  Image$$BOOT_EXTERNAL_HEAP$$Length;

static whitelst_tbl_entry_type xblconfig_img_whitelist[] = 
  {
    {(uint64)&Image$$BOOT_INTERNAL_HEAP$$Base, (uint64)&Image$$BOOT_INTERNAL_HEAP$$Length},
    {(uint64)&Image$$BOOT_EXTERNAL_HEAP$$Base, (uint64)&Image$$BOOT_EXTERNAL_HEAP$$Length},
    {SCL_PMIC_CONFIG_BUF_BASE,SCL_PMIC_CONFIG_BUF_SIZE},
    {SCL_SBL1_DDR_DSF_CFG_BUF_BASE, SCL_SBL1_DDR_DSF_CFG_BUF_SIZE},
    {SCL_SBL1_DDR_SHRM_STRUCT_BASE, SCL_SBL1_DDR_SHRM_STRUCT_SIZE},
    {0,0}
  };

/*=============================================================================
                              FUNCTION DEFINITIONS
=============================================================================*/


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
xblconfig_status_type xcfg_load_addr_validation_init (void)
{  
  xblconfig_status_type status = XBLCONFIG_BUFFER_VALIDATION_ERR;

  /* Set the image whitelist for whitelist based image loading */
  if( TRUE == boot_set_active_img_whitelist_table(&xblconfig_img_whitelist[0]))
    status = XBLCONFIG_SUCCESS;

  return status;
}

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
xblconfig_status_type xcfg_load_addr_validation_deinit (void)
{  
  boot_invalidate_img_whitelist();
  
  return XBLCONFIG_SUCCESS;
}
