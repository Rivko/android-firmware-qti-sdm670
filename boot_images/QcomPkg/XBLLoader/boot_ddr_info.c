/*=============================================================================

                       Boot DDR Information

GENERAL DESCRIPTION
  This module implements functions that update and share ddr info in SBL

Copyright 2010-2011, 2013, 2015, 2017 by Qualcomm Technologies Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who             what, where, why
--------   ---         --------------------------------------------------
05/25/17   dai         Added support for filling ddr details in shared memory
10/05/15   ck          Corrected copyright
08/07/15   kpa         added support for passing highest bank bit information to ram
                       partition table
06/14/13   dh          Update SBL_DDR_SIZE to MAX_DDR_INTERFACE_NUM
05/23/11   plc         Add support for retrieving partition information 
                       for each DDR device
07/22/10   dh          Initial version created by Kedar  

===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "boot_ddr_info.h"
#include "boot_extern_smem_interface.h"
#include "boot_sbl_shared.h"
#include "boot_extern_ddr_interface.h"


/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/
#define MAX_DDR_INTERFACE_NUM     8

/* Structure to store DDR information*/
static sbl_if_shared_ddr_device_info_type boot_ddr_size_info;
boolean boot_ddr_info_set = FALSE;

/*=========================================================================
                            
                       FUNCTION DEFINITIONS

=========================================================================*/
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
void boot_share_ddr_info
(
  sbl_if_shared_ddr_device_info_type *shared_dev_info, 
  ddr_size_info *ddr_available
)
{
  static sbl_if_shared_ddr_info_type ddr_info[MAX_DDR_INTERFACE_NUM];
  uint8 i;
  BL_VERIFY( shared_dev_info != NULL , BL_ERR_NULL_PTR_PASSED|BL_ERROR_GROUP_BOOT );

  shared_dev_info->ddr_info = ddr_info;
  shared_dev_info->noofddr = 0;
  /* Ensure that "ddr_partition_info*" is NULL for legacy DDR driver backwards compatibility */
  shared_dev_info->ddr_partition_info = NULL;
 
  for(i = 0; i < DDR_MAX_NUM_CH; i++)
  {
    if(ddr_available->ddr_cs0[i] != 0)
    {
      shared_dev_info->ddr_info[shared_dev_info->noofddr].ramsize = ddr_available->ddr_cs0[i];
      shared_dev_info->ddr_info[shared_dev_info->noofddr].cs_addr = ddr_available->ddr_cs0_addr[i];
      shared_dev_info->noofddr++;
    }
    if(ddr_available->ddr_cs1[i] != 0)
    {
      shared_dev_info->ddr_info[shared_dev_info->noofddr].ramsize = ddr_available->ddr_cs1[i];
      shared_dev_info->ddr_info[shared_dev_info->noofddr].cs_addr = ddr_available->ddr_cs1_addr[i];
      shared_dev_info->noofddr++;
    }
  }
}

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
  sbl_if_shared_ddr_device_info_type *shared_dev_info, 
  ddr_size_info *ddr_available,
  ddr_size_partition *ddr_available_partition
)
{
  static sbl_if_shared_ddr_info_type ddr_info[MAX_DDR_INTERFACE_NUM];
  static sbl_if_shared_extended_ddr_info_type ddr_partition_info[MAX_DDR_INTERFACE_NUM];
  uint8 i; 
  BL_VERIFY( shared_dev_info != NULL , BL_ERR_NULL_PTR_PASSED|BL_ERROR_GROUP_BOOT );

  shared_dev_info->ddr_info = ddr_info;
  shared_dev_info->ddr_partition_info = ddr_partition_info;
  shared_dev_info->noofddr = 0;
 
  for (i = 0; i < DDR_MAX_NUM_CH; i++)
  {
    if(ddr_available->ddr_cs0[i] != 0)
    {
      shared_dev_info->ddr_info[shared_dev_info->noofddr].ramsize = ddr_available->ddr_cs0[i];
      shared_dev_info->ddr_info[shared_dev_info->noofddr].cs_addr = ddr_available->ddr_cs0_addr[i];
      shared_dev_info->ddr_partition_info[shared_dev_info->noofddr].num_partitions = ddr_available_partition->ddr_cs0[i].num_partitions;
      shared_dev_info->ddr_partition_info[shared_dev_info->noofddr].sdram_addr = ddr_available_partition->ddr_cs0[i].ddr_addr;
      shared_dev_info->ddr_partition_info[shared_dev_info->noofddr].highest_bank_bit = ddr_available->highest_bank_bit;
      shared_dev_info->noofddr++;
    }
    if(ddr_available->ddr_cs1[i] != 0)
    {
      shared_dev_info->ddr_info[shared_dev_info->noofddr].ramsize = ddr_available->ddr_cs1[i];
      shared_dev_info->ddr_info[shared_dev_info->noofddr].cs_addr = ddr_available->ddr_cs1_addr[i];
      shared_dev_info->ddr_partition_info[shared_dev_info->noofddr].num_partitions = ddr_available_partition->ddr_cs1[i].num_partitions;
      shared_dev_info->ddr_partition_info[shared_dev_info->noofddr].sdram_addr = ddr_available_partition->ddr_cs1[i].ddr_addr;
      shared_dev_info->ddr_partition_info[shared_dev_info->noofddr].highest_bank_bit = ddr_available->highest_bank_bit;	
      shared_dev_info->noofddr++;
    }
  }
}

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
void boot_set_ddr_info 
( 
  struct bl_shared_data_type *bl_shared_data 
)
{
 
  BL_VERIFY( (bl_shared_data != NULL) &&
               (bl_shared_data->sbl_shared_data != NULL) &&
               (bl_shared_data->sbl_shared_data->ddr_shared_info != NULL), 
                BL_ERR_NULL_PTR_PASSED|BL_ERROR_GROUP_BOOT );
               
  boot_ddr_size_info = *(bl_shared_data->sbl_shared_data->ddr_shared_info);
  boot_ddr_info_set = TRUE;
          
}


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
sbl_if_shared_ddr_device_info_type *boot_get_ddr_info ( void )
{
    BL_VERIFY( ( boot_ddr_info_set == TRUE), BL_ERR_DDR_INFO_NOT_SET|BL_ERROR_GROUP_DDR );
  return &boot_ddr_size_info;
}



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


void boot_populate_ddr_details_shared_table 
( 
  bl_shared_data_type *bl_shared_data 
)
{
  ddr_details_entry_ptr_type ddr_details_entry_ptr = NULL;
  ddr_partition_return_type result = DDR_PART_OTHER_ERR;
  
  ddr_details_entry_ptr = (ddr_details_entry_ptr_type) boot_smem_alloc(DDRINFO_UEFI_SMEM_ID,
                                           sizeof(ddr_details_entry));

  do
  {
    /* verify shared memory allocation for ddr_det_entry_ptr_type didn't fail */
    if(ddr_details_entry_ptr == NULL)
    {
      result = DDR_PART_NULL_PTR_ERR;
      break;
    }

    /* verify if ddr info table is in shared memory region */
    if((((uintnt)ddr_details_entry_ptr) > SCL_SHARED_RAM_BASE) &&
      (((uintnt)ddr_details_entry_ptr + sizeof(ddr_details_entry))
      <=(SCL_SHARED_RAM_BASE + SCL_SHARED_RAM_SIZE)))
    {
      result = DDR_PART_SUCCESS;
    }
    else
    {
      result = DDR_PART_OUT_OF_BOUND_PTR_ERR;
      break;
    }
    
  }while(0);  
  
  BL_VERIFY(result == DDR_PART_SUCCESS && ddr_details_entry_ptr != NULL, 
            (uint16)result|BL_ERR_FAIL_ALLOCATE_SMEM);

  boot_get_ddr_details_table(ddr_details_entry_ptr);
      	

}

