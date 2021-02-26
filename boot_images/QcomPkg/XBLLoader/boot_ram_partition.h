#ifndef BOOT_RAM_PARTITION_H
#define BOOT_RAM_PARTITION_H

/*===========================================================================

              RAM Partition Table Function Definitions Header File

GENERAL DESCRIPTION
  This header file gives boot specific external definition for RAM 
  Partition Table feature implementation.
  
Copyright 2010-2011, 2013-2014 by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    
    
when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/20/14   ck      Updated functions to support 64 bit addresses
05/09/14   kedara  removed add_boot_region_ram_partition prototype
                   since its api internal to ram partition logic
05/09/14   kedara   Include com_dtypes.h instead of comdef.h
10/17/13   ck      Removing unnecessary macro redirections.
09/12/11   dh      Clean up the address defines
11/03/10   dh      Added IMEM and IRAM size and base
07/21/10   dh      Initial Creation.

===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "com_dtypes.h"
#include RAM_PARTITION_TABLE_H
#include "boot_sbl_if.h"
#include "boot_sbl_shared.h"


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

**  Function :  boot_populate_ram_partition_table

** ==========================================================================
*/
/*!
* 
* @brief
*    Function initializes usable_ram_partition_table in shared memory.
*    Then for each type of memory (system memories,boot memory and appsbl memory),
*    this function adds a partition entry in the ram partition table
*    Once complete, function writes magic numbers and version number into table. 
* 
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
*/
void boot_populate_ram_partition_table ( bl_shared_data_type *bl_shared_data );

  
/*===========================================================================

**  Function :  get_partition_category

** ==========================================================================
*/
/*!
* 
* @brief
*    Given a memory address mem_base function returns the
*    partition_category this address belongs to. This function can only be 
*    used after all system memory partitions are added into the table.
* 
* @param[in]  usable_ram_part_tbl_ptr Pointer to table containing usable memory 
*                                     partitions.
* @param[in]  mem_base                Base of the memory address
* @param[out] partition_category      Returned value of partition category
*   
* @par Dependencies
*   None
*   
* @retval
*   ram_partition_return_type
*   RAM_PART_SUCCESS                - if function is successful.
*   RAM_PART_CATEGORY_NOT_EXIST_ERR - if function can't retrieve partition
*                                     category.
* 
* @par Side Effects
*   None
*/
ram_partition_return_type get_partition_category(usable_ram_part_table_t ,
                                                 uintnt,
                                                 uint32 *);

#endif /* BOOT_RAM_PARTITION_H */
