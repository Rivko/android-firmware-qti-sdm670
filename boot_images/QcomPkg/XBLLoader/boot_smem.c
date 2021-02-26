/*===========================================================================

                       Boot Shared Memory Source File

GENERAL DESCRIPTION
  This file contains functions which use shared memory.

Copyright 2010, 2012-2016 by Qualcomm Technologies Incorporated.  All Rights Reserved.
============================================================================*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ------------------------------------------------------------
04/11/16   bd      Add API to get SMEM init status flag
10/05/15   ck      Corrected copyright
03/26/14   jz      Cleanup boot_populate_secondary_eeprom_info
12/16/13   kedara  Added call to populate secondary eeprom info to smem.
09/30/13   lm      Added call to populate UUID in smem
09/04/13   jz      Added call to populate CPR settings and voltages
03/18/13   dhaval  Call image version APIs to populate boot image version in smem
02/26/13   dh      Call boot_pm_init_smem in boot_smem_store_pon_status
08/30/12   dh      Remove boot log as smem isn't time consuming operation
04/18/12   dh      change boot_pm_get_power_on_status to boot_pm_dev_get_power_on_reason
10/20/10   aus     Added API to call smem_boot_debug_init() 
09/30/10   dxiang  PMIC API has changed, used generic header 
09/13/10   dh      Add boot logger
09/10/10   aus     Saving PMIC power on status to SMEM and moved boot_smem_init_for_dload()
                   to boot_init_for_dload() function.
08/18/10   aus     Fixed compiler warnings
07/22/10   aus     Initial creation
=============================================================================*/

#define EEPROMII_DATA_SIZE_IN_BYTES 50

/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boot_smem.h"
#include "boot_cache_mmu.h"
#include "boot_extern_pmic_interface.h"
#include "boot_extern_smem_interface.h"
#include "boot_logger.h"
#include "image_version.h"
#include "boot_extern_power_interface.h"
#include "boot_config_data.h"

/* Global variable indicating SMEM initialization status */
boot_boolean smem_init_done = FALSE;

/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/

/*===========================================================================

**  Function :  boot_cache_set_memory_barrier

** ==========================================================================
*/
/*!
* 
* @brief
*   This function will set the location of the actual memory barrier location
*   in shared memory.
*  
* @param[in] bl_shared_data Pointer to shared data structure
*
* @par Dependencies
*   None
*   
* @retval
*   None                                                               
* 
* @par Side Effects
*   None
* 
*/
void boot_cache_set_memory_barrier
(
  bl_shared_data_type *bl_shared_data
)
{
/* Even though boot_smem_alloc is optioned in or out via
   FEATURE_BOOT_EXTERN_SMEM_COMPLETED, the return value of boot_smem_alloc
   is checked for NULL so if SMEM is optioned out then this logic must also */

#ifdef FEATURE_BOOT_EXTERN_SMEM_COMPLETED
  uint32 *memory_barrier_ptr = boot_smem_alloc( SMEM_MEMORY_BARRIER_LOCATION,
                                           sizeof(*memory_barrier_ptr) );
  BL_VERIFY( memory_barrier_ptr != NULL, BL_ERR_NULL_PTR_PASSED|BL_ERROR_GROUP_BOOT );
  
  cache_set_memory_barrier_location( memory_barrier_ptr );
#endif

} /* boot_cache_set_memory_barrier() */

/*===========================================================================

**  Function :  boot_populate_secondary_eeprom_info

** ==========================================================================
*/
/*!
* 
* @brief
*   This function will populate data from secondary eeprom to shared memory
*  
* @param[in]
*  None
*
* @par Dependencies
*  None
*   
* @retval
*  None                                                               
* 
* @par Side Effects
*  None
* 
*/
static void boot_populate_secondary_eeprom_info ( void )
{
#ifdef FEATURE_READ_SECONDARY_EEPROM
  uint8 *eepromII_info_smem_ptr = NULL;
  
  eepromII_info_smem_ptr = (uint8 *)boot_smem_alloc( SMEM_RF_EEPROM_DATA, 
                                            EEPROMII_DATA_SIZE_IN_BYTES );
  if( eepromII_info_smem_ptr != NULL)
  {
    /*Proceed only if smem supports RF data population */
    boot_get_eepromII_info(eepromII_info_smem_ptr,
                           EEPROMII_DATA_SIZE_IN_BYTES);
  }
#endif
} /* boot_populate_secondary_eeprom_info() */

/*===========================================================================

**  Function :  boot_smem_store_pon_status

** ==========================================================================
*/
/*!
* 
* @brief
*   Store the power on reset status value to smem, so apps processor can access later.
*  
* @par Dependencies
*   Must be called after smem is initialized. ie after smem_boot_init()
*   
* @retval
*   None                                                               
* 
* @par Side Effects
*   None
* 
*/
void boot_smem_store_pon_status
( 
  bl_shared_data_type *bl_shared_data 
)
{
   pm_err_flag_type status =boot_pm_init_smem();
  /* Storing the PON status to shared memory */
  BL_VERIFY(PM_ERR_FLAG__SUCCESS ==status , (uint16)status| BL_ERROR_GROUP_PMIC);
  
} /* boot_smem_store_pon_status() */

/*===========================================================================

**  Function :  boot_smem_init

** ==========================================================================
*/
/*!
* 
* @brief
*   Wrapper function that calls smem_boot_init().
*  
* @param[in] bl_shared_data Pointer to shared data structure
*
* @par Dependencies
*   None
*   
* @retval
*   None                                                               
* 
* @par Side Effects
*   None
* 
*/
void boot_smem_init
( 
  bl_shared_data_type *bl_shared_data 
)
{
  uint32 result = 0;
  
  /* Initialize shared memory */
  boot_smem_boot_init();

  /* initialize image_version_table with spaces. 
     this function can only be called after smem_boot_init */
  result = boot_image_version_initialize_version_table();
  BL_VERIFY( (result==0) , (uint16)result|BL_ERROR_GROUP_IMAGE_VERSION );

  /* populate boot image version in the SMEM_IMAGE_VERSION_TABLE */
  result = boot_image_version_populate_version(IMAGE_INDEX_SBL);
  BL_VERIFY( (result==0) , (uint16)result|BL_ERROR_GROUP_IMAGE_VERSION );

  /* populate CPR settings and voltages in the SMEM (this is required on every boot) */
  boot_populate_cpr_settings();

  /* populate UUID in the SMEM */
  boot_image_populate_uuid();

  /* populate secondary eeprom data to the SMEM */
  boot_populate_secondary_eeprom_info();
  
  smem_init_done = TRUE;
} /* boot_smem_init() */

/*===========================================================================

**  Function :  boot_smem_debug_init

** ==========================================================================
*/
/*!
* 
* @brief
*    This puts shared memory in debug mode. It changes the behaviour of smem_alloc().
*    1. If no SMEM initialization APIs have been called, smem_alloc() will 
*       call smem_init().
*    2. If smem_boot_debug_init() has been called, but smem_boot_init() has not 
*       been called, smem_alloc() will return NULL.
*    3. If smem_boot_init() or smem_init() have been called, smem_alloc() will 
*       return a pointer to the appropriate item in shared memory.
*  
* @param[in] bl_shared_data Pointer to shared data structure
*
* @par Dependencies
*   None
*   
* @retval
*   None                                                               
* 
* @par Side Effects
*   None
* 
*/
void boot_smem_debug_init
( 
   bl_shared_data_type *bl_shared_data 
)
{
  /* 
     SMEM behaves as follows:
     smem_alloc() will return NULL 
     smem_get_addr() will return valid pointer
  */
  boot_smem_boot_debug_init();
} /* boot_smem_debug_init */

/*===========================================================================

**  Function :  boot_smem_init_get_status

** ==========================================================================
*/
/*!
*
* @brief
*   Return SMEM initialization status.
* *
* @par Dependencies
*   None
*
* @retval
*   Returns SMEM initilization status
*
* @par Side Effects
*   None
*
*/
boot_boolean boot_smem_get_init_status(void)
{
  return smem_init_done;
}