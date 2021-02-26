#ifndef BOOT_DDR_INFO_H
#define BOOT_DDR_INFO_H

/*=============================================================================

                       Boot DDR Information

GENERAL DESCRIPTION
  This module implements functions that update and share ddr info in SBL

Copyright 2010-2011, 2015 by Qualcomm Technologies Inc.  All Rights Reserved.
=============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when         who     what, where, why
--------     ---     ----------------------------------------------------------
10/05/2015     ck      Corrected copyright
05/25/2011   plc     Added prototype for boot_share_extended_ddr_info
07/28/2010   dh      Initial creation(implemented by Kedar)

============================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "boot_sbl_if.h"
#include "ddr_drivers.h"

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

**  Function :  boot_share_ddr_info

** ==========================================================================
*/
/*!
* 
* @brief
*    This function gets the ddr information from ddr driver and adds it to 
*    shared data structure
* 
* @param[out] shared_dev_info Pointer to the shared device information
* @param[in] ddr_available Pointer to the DDR size information
*
* @par Dependencies
*   None
*   
* @retval
*   None
* 
* @par Side Effects
*   None
*/
void boot_share_ddr_info(sbl_if_shared_ddr_device_info_type *, ddr_size_info *);

/*===========================================================================

**  Function :  boot_share_extended_ddr_info

** ==========================================================================
*/
/*!
* 
* @brief
*    This function gets the ddr partition information from ddr driver and 
*    adds it to shared data structure, including number of partitions for 
*    each DDR device. 
* 
* @param[out] shared_dev_info Pointer to the shared device information
* @param[in] ddr_available Pointer to the DDR size information
* @param[in] ddr_available_partition Pointer to DDR partition information
*
* @par Dependencies
*   None
*   
* @retval
*   None
* 
* @par Side Effects
*   None
*/
void boot_share_extended_ddr_info
(
  sbl_if_shared_ddr_device_info_type *, ddr_size_info *, ddr_size_partition *
);

/*===========================================================================

**  Function :  boot_set_ddr_info

** ==========================================================================
*/
/*!
* 
* @brief
*   This function extracts pointer for DDR info from shared data structure.
*   should only be called after the ddr_shared_info pointer inside 
*   bl_shared_data is initialized
*   
* @param[in] bl_shared_data Pointer to the shared data structure 
*
* @par Dependencies
*   None
*   
* @retval
*   pointer to structure containing ddr information (ddr_size_info )
* 
* @par Side Effects
*   None
* 
*/
void boot_set_ddr_info ( struct bl_shared_data_type *bl_shared_data );


/*===========================================================================

**  Function :  boot_get_ddr_info

** ==========================================================================
*/
/*!
* 
* @brief
*   This function returns information about DDR.
*  
* @par Dependencies
*   None
*   
* @retval
*   pointer to structure containing ddr information (ddr_size_info )
* 
* @par Side Effects
*   None
* 
*/
sbl_if_shared_ddr_device_info_type *boot_get_ddr_info ( void );


/*===========================================================================

**  Function :  boot_populate_ddr_details_shared_table

** ==========================================================================
* @brief
*    This function initializes ddr details required in shared memory.
   
* @param[in] bl_shared_data Pointer to the shared data structure 
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


void boot_populate_ddr_details_shared_table ( bl_shared_data_type *bl_shared_data );

#endif  /* BOOT_DDR_INFO_H */
