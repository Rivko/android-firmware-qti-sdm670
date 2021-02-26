/*=============================================================================

                                BOOT RAM Initialization 

GENERAL DESCRIPTION
  This file contains the software that performs the SBL RAM initialization 
  and scatter loading regions.

Copyright 2010-2011, 2013-2015, 2017 by Qualcomm Technologies Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ------------------------------------------------------------
07/13/17   yps     Flush boot imem region after init boot imem which is cacheable
10/05/15   ck      Corrected copyright
02/12/14   ck      boot_shared_imem_cookie structure now has version entry that gets set
10/22/13   ck      Removed rpm_sync_cookie from boot_shared_imem_init as it is
                   no longer needed in Bear Family
05/01/13   jz      Clear RPM sync cookie at startup
06/07/11   dh      use boot_shared_imem_cookie_ptr instead of hardcoding 
                   cookie addresses.
02/24/11   dh      add boot_shared_imem_init to initialized boot shared imem
08/06/10   aus     Clean-up
07/22/10   aus     Initial revision
=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boot_comdef.h"
#include "boot_ram_init.h"
#include "boot_util.h"
#include "boot_sbl_if.h"
#include "boot_shared_imem_cookie.h"
#include "boot_cache.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/
/*Define a global pointer which points to the boot shared imem cookie structure */
struct boot_shared_imem_cookie_type *boot_shared_imem_cookie_ptr = 
                  (struct boot_shared_imem_cookie_type *)SHARED_IMEM_BOOT_BASE;


/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/
/*============================================================================

**  Function : boot_ram_init

** ==========================================================================
*/
/*!
* 
* @brief
*   Copy initialized data (RW-DATA) into RAM. And, Zero-out (ZI-DATA) in RAM.
*   See the file sbl*.scl for a description of the structure of RAM.
* 
* @param[in] boot_ram_init_data Pointer which contains the addresses for the 
*                               RW and ZI load regions and execute regions
*
* @par Dependencies
*   None.
* 
* @retval
*   True if success, fail otherwise
* 
* @par Side Effects
*   Memory not in the RW region and Zero-Init region is  
*   not affected by this function -- those locations are left in an
*   unknown state.
*   
*/
boolean boot_ram_init( boot_ram_init_data_type *boot_ram_init_data )
{
  
  /* Copy RW data from load to image region only if load and image
     regions are different. */
  if ( boot_ram_init_data->load_rw_base != boot_ram_init_data->image_rw_base)
  {
    /* Copy RW data from the load region to the execute region .*/
    qmemcpy( boot_ram_init_data->image_rw_base,
             boot_ram_init_data->load_rw_base,
             (uint32)boot_ram_init_data->image_rw_len);
  }

  /* Zero out the zero-init data segment. */
  qmemset( boot_ram_init_data->image_zi_base, 
           0, 
           (uint32)boot_ram_init_data->image_zi_len);

  return TRUE;
  
} /* boot_ram_init() */

/*============================================================================

**  Function : boot_scatterload_region

** ==========================================================================
*/
/*!
* 
* @brief
*   Copy load region to image region from flash into IRAM or IMEM.
* 
* @param[in] boot_scatterload_data Pointer which contains addresses of the 
                                   scatter loaded load and execute region 
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
boolean boot_scatterload_region ( boot_scatterload_data_type *boot_scatterload_data )
{
  qmemcpy( boot_scatterload_data->image_region_base,
           boot_scatterload_data->load_region_base,
           (uint32) boot_scatterload_data->image_region_len );
  
  return TRUE;
} /* boot_scatterload_region() */


/*===========================================================================

**  Function :  boot_shared_imem_init

** ==========================================================================
*/
/*!
* 
* @brief
*   This funcion will initialize the boot shared imem to all 0xFFFFFFFF value if
*   it has not been initialized yet.
* 
* @param[in] bl_shared_data Pointer to shared data
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
void boot_shared_imem_init(bl_shared_data_type *bl_shared_data)
{
  /*Make sure size of the cookie struct doesn't exceed the boot shared imem size*/
  BL_VERIFY(sizeof(struct boot_shared_imem_cookie_type) <=
            SHARED_IMEM_BOOT_SIZE,
            BL_ERR_COOKIE_TOO_BIG|BL_ERROR_GROUP_BOOT);
  
  /*First check to see if magic number is present*/
  if (boot_shared_imem_cookie_ptr != NULL)
  {
    if (boot_shared_imem_cookie_ptr->shared_imem_magic !=
        BOOT_SHARED_IMEM_MAGIC_NUM)
    {
      /*If magic number is not there then 
        intialize the whole boot shared imem to 0xFFFFFFFF*/
      qmemset((void*)boot_shared_imem_cookie_ptr,
              SHARED_IMEM_REGION_DEF_VAL,
              SHARED_IMEM_BOOT_SIZE);

      boot_shared_imem_cookie_ptr->shared_imem_magic =
        BOOT_SHARED_IMEM_MAGIC_NUM;
      
      boot_shared_imem_cookie_ptr->shared_imem_version =
        BOOT_SHARED_IMEM_VERSION_NUM;
	  dcache_flush_region((void *)SHARED_IMEM_BOOT_BASE, SHARED_IMEM_BOOT_SIZE);
    }
  }
}
