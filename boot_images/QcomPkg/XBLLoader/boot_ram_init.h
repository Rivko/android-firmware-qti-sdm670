#ifndef BOOT_RAM_INIT_H
#define BOOT_RAM_INIT_H

/*===========================================================================

                         Boot RAM Initialization Header File

GENERAL DESCRIPTION
  This header file contains declarations and definitions for the SBL RAM
  test and initialization.

Copyright 2011, 2014-2015 by Qualcomm Technologies Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/05/15   ck      Corrected copyright
10/29/14   kpa     Support 64 bit memory map
02/24/11   dh      Added boot_shared_imem_init
07/22/11   aus     Initial creation
                                                                 
============================================================================*/


/*============================================================================
 
                           INCLUDE FILES

============================================================================*/
#include "boot_sbl_shared.h"


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/* RAM Data Initialization structure */
typedef struct{  
  uint8  *load_rw_base;
  uint8  *image_rw_base;
  uintnt *image_rw_len;
  uint8  *image_zi_base;
  uintnt *image_zi_len;
}boot_ram_init_data_type;

/* Scatterload Region Initialization structure */
typedef struct{
  uint8  *load_region_base;
  uint8  *image_region_base;
  uintnt *image_region_len;
}boot_scatterload_data_type;

/*============================================================================

                      PUBLIC FUNCTION DECLARATIONS

============================================================================*/

/*============================================================================

**  Function : boot_ram_init

** ==========================================================================
*/
/*!
* 
* @brief
*   Copy initialized data (RW-DATA) into RAM. And, Zero-out (ZI-DATA) in RAM
*   for a particular stage of the bootloader
*   See the scatterloader file (.scl) for a description of the structure of RAM.
* 
* @param[in] boot_ram_init_data RAM initialization data structure
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
boolean boot_ram_init( boot_ram_init_data_type *boot_ram_init_data );

/*============================================================================

**  Function : boot_scatterload_region

** ==========================================================================
*/
/*!
* 
* @brief
*   Copy load region to image region from flash into IRAM or IMEM.
* 
* @param[in] boot_scatterload_data Boot scatterload initialization data structure
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
boolean boot_scatterload_region ( boot_scatterload_data_type *boot_scatterload_data );


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
void boot_shared_imem_init(bl_shared_data_type *bl_shared_data);

#endif  /* BOOT_RAM_INIT_H */
