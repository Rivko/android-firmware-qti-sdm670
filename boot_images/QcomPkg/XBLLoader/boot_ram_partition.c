/*===========================================================================

                RAM Partition Table Function Implementations

DESCRIPTION
  This source file gives the implementation of the RAM Partition Table 
  population functions.
  
Copyright 2010-2015 by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

    This section contains comments describing changes made to this file.
    Notice that changes are listed in reverse chronological order.
    

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/07/15   kpa     added support for passing highest bank bit information to ram
                   partition table
03/24/15   kpa     Added field "available_length" to ram partition table
10/21/14   ck      Changed direct SCL_IMEM usage to IMEM_RANGE as multiple IMEM regions can exist
10/21/14   ck      Updated to support 64 bit addressing
08/20/14   kedara  Fix compiler warnings.
10/17/13   ck      Removing unnecessary macro redirections.
07/24/12   kedara  Fix compilation issue when smem base starts from addr 0x0
04/30/12   dh      Remove add_appsbl_ram_partition
09/12/11   dh      Switch smem calls to boot extern driver interface calls
06/22/11   dh      Change all DDR partitions' category type to RAM_PARTITION_SDRAM
                   instead of specific bus type.
05/23/11   plc     Added logic to add_system_partitions, to check shared 
                   information structure for DDR partition information, and 
				   if present, populate this data in the RAM Partition Table
01/21/11   dh      fixed klocwork warning
11/03/10   dh      Added imem and iram partition
09/21/10   dh      modified to include EBI1's channel type
07/21/10   dh      Initial Creation

===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "boot_ram_partition.h"
#include "boot_ddr_info.h"
#include "boot_extern_smem_interface.h"
#include "boot_sbl_shared.h"

#define CONVERT_TO_BYTE_SHIFT	20

/*=========================================================================

                      FUNCTION DECLARATIONS 
					  
===========================================================================*/

/*===========================================================================

**  Function :  load_ram_partition_table

** ==========================================================================
*/
/*!
* 
* @brief
*    Function initializes usable_ram_partition_table in shared memory.
* 
* @param[out]  result Function return status. If it equals RAM_PART_SUCCESS 
*                     then the table is successfully allocated.
*     
* @par Dependencies
*   None
*   
* @retval
*   usable_ram_part_table_t usable_ram_part_tbl_ptr
*     - returns pointer to usable ram partition table in shared memory. 
*    
* @par Side Effects
*   None
*/
static usable_ram_part_table_t load_ram_partition_table (uint32 *result)
{
  /* allocate memory in shared memory region for usable_ram_part_table */
  usable_ram_part_table_t usable_ram_part_tbl_ptr 
    = (usable_ram_part_table_t) boot_smem_alloc(SMEM_USABLE_RAM_PARTITION_TABLE,
                                           sizeof(usable_ram_part_table));
  *result = RAM_PART_OTHER_ERR;

  do
  {
    /* verify shared memory allocation for usable_ram_part_table didn't fail */
    if(usable_ram_part_tbl_ptr == NULL)
    {
      *result = RAM_PART_NULL_PTR_ERR;
      break;
    }

    /* verify if usable_ram_parition_table is in shared memory region */
    if((((uintnt)usable_ram_part_tbl_ptr) > SCL_SHARED_RAM_BASE) &&
      (((uintnt)usable_ram_part_tbl_ptr + sizeof(usable_ram_part_table))
      <=(SCL_SHARED_RAM_BASE + SCL_SHARED_RAM_SIZE)))
    {
      usable_ram_part_tbl_ptr->num_partitions = 0;
      *result = RAM_PART_SUCCESS;
    }
    else
    {
      *result = RAM_PART_OUT_OF_BOUND_PTR_ERR;
      break;
    }
    
  }while(0);
  
  /* return pointer to usable ram part table in shared memory */
  return usable_ram_part_tbl_ptr;
}


/*===========================================================================

**  Function :  add_single_ram_partition

** ==========================================================================
*/
/*!
* 
* @brief
*    Function adds a single partition entry into the ram partition table.
* 
* @param[in] usable_ram_part_tbl_ptr Pointer to ram partition table that 
*                                    should be updated.
*
* @param[in] uintnt start_address start address of this partition
*
*
* @param[in] uint32 length length of this partition in bytes
*
*
* @param[in] uint32 partition_attribute attribute of this partition
*
*
* @param[in] uint32 partition_domain domain of this partition
*
*
* @param[in] uint32 partition_type type of this partition
*
* @par Dependencies
*   None
*   
* @retval
*   ram_partition_return_type
*   RAM_PART_SUCCESS 	  - if function is successful.
*   RAM_PART_TABLE_FULL_ERR - if table is full or not enough space in 
*                             the table
* 
* @par Side Effects
*   None
*/
static ram_partition_return_type add_single_ram_partition
(
  usable_ram_part_table_t usable_ram_part_tbl_ptr,
  uintnt start_address,            
  uintnt length,       
  uint32 partition_attribute,       
  uint32 partition_category, 
  uint32 partition_domain, 
  uint32 partition_type
)
{
  ram_part_entry_t ram_part_entry_ptr = NULL;
  ram_partition_return_type result = RAM_PART_TABLE_FULL_ERR;

  if(usable_ram_part_tbl_ptr->num_partitions < RAM_NUM_PART_ENTRIES)
  {
    ram_part_entry_ptr = &usable_ram_part_tbl_ptr->
                         ram_part_entry[usable_ram_part_tbl_ptr->num_partitions];
                      
    ram_part_entry_ptr->start_address = start_address;
    ram_part_entry_ptr->length = length; 
    ram_part_entry_ptr->available_length = length; 
    ram_part_entry_ptr->partition_attribute = partition_attribute;
    ram_part_entry_ptr->partition_category = partition_category;
    ram_part_entry_ptr->partition_domain = partition_domain;
    ram_part_entry_ptr->partition_type = partition_type;
    usable_ram_part_tbl_ptr->num_partitions++;
    result = RAM_PART_SUCCESS;
  }
  
  return result;
}


/*===========================================================================

**  Function :  add_system_ram_partitions

** ==========================================================================
*/
/*!
* 
* @brief
*    Function adds all the DDR interfaces to ram partition table.
*    Each DDR interface will be added as a single entry in the table.
*    Partition category  will be RAM_PARTITION_SDRAM for all DDR entries.
* 
* @param[in] usable_ram_part_tbl_ptr Pointer to the ram partition table that 
*                                    should be populated
* 
* @par Dependencies
*   None
*   
* @retval
*   ram_partition_return_type
*   RAM_PART_SUCCESS        - if function is successful.
*   RAM_PART_TABLE_FULL_ERR - if table is full or not enough space in 
*                             the table  
*   RAM_PART_CATEGORY_NOT_EXIST_ERR - if unable to populate certain DDR information
*                             
* @par Side Effects
*   None
*/
static ram_partition_return_type add_system_ram_partitions
(
  usable_ram_part_table_t usable_ram_part_tbl_ptr
)
{
  ram_part_entry_t ram_part_entry_ptr = NULL;
  sbl_if_shared_ddr_info_type *ddr_info = NULL;
  sbl_if_shared_extended_ddr_info_type *ddr_extended_info = NULL;
  uint32 partition_index = 0; 
  ram_partition_return_type result = RAM_PART_SUCCESS;
  sbl_if_shared_ddr_device_info_type *ddr_shared_info = boot_get_ddr_info();
  
  /*ram_part_entry_ptr points to first empty slot in the table*/
  ram_part_entry_ptr =
      &usable_ram_part_tbl_ptr->
        ram_part_entry[usable_ram_part_tbl_ptr->num_partitions];
  

  /*Loop through ddr_info and add all DDR interfaces into the table*/
  for(; partition_index < ddr_shared_info->noofddr  &&
        usable_ram_part_tbl_ptr->num_partitions < RAM_NUM_PART_ENTRIES;
        partition_index++)
  {
    ddr_info = &ddr_shared_info->ddr_info[partition_index];
    
    ram_part_entry_ptr->partition_category = RAM_PARTITION_SDRAM;
    ram_part_entry_ptr->start_address = ddr_info->cs_addr;	  
    ram_part_entry_ptr->length = ddr_info->ramsize << CONVERT_TO_BYTE_SHIFT;
    ram_part_entry_ptr->available_length = ram_part_entry_ptr->length;
    ram_part_entry_ptr->partition_attribute = RAM_PARTITION_READWRITE;
    ram_part_entry_ptr->partition_domain = RAM_PARTITION_DEFAULT_DOMAIN;
    ram_part_entry_ptr->partition_type = RAM_PARTITION_SYS_MEMORY;
    
    /*Add the extended ddr information to current ram partition entry*/
    if(ddr_shared_info->ddr_partition_info != NULL)
      {
      ddr_extended_info = &ddr_shared_info->ddr_partition_info[partition_index];
      
      /*Make sure the base address of ddr extended info matches the current ddr base address*/
      if (ddr_extended_info->sdram_addr != ram_part_entry_ptr->start_address)
      {
        result = RAM_PART_CATEGORY_NOT_EXIST_ERR;
        break;
      }
      ram_part_entry_ptr->num_partitions = ddr_extended_info->num_partitions;
      ram_part_entry_ptr->highest_bank_bit = ddr_extended_info->highest_bank_bit;
    }
    
      ram_part_entry_ptr++;
      usable_ram_part_tbl_ptr->num_partitions++;
    }
   
  return result;
}


/*===========================================================================

**  Function :  add_imem_ram_partition

** ==========================================================================
*/
/*!
* 
* @brief
*    Function adds a partition entry for imem memory into the 
*    ram partition table.
* 
* @param[in] usable_ram_part_tbl_ptr Pointer to ram partition table that 
*                                    should be updated.
*   
* @par Dependencies
*   None
*   
* @retval
*   ram_partition_return_type
*   RAM_PART_SUCCESS 	  - if function is successful.
*   RAM_PART_TABLE_FULL_ERR - if table is full or not enough space in 
*                             the table
* 
* @par Side Effects
*   None
*/
static ram_partition_return_type add_imem_ram_partition
(
  usable_ram_part_table_t usable_ram_part_tbl_ptr
)
{
  ram_partition_return_type result = RAM_PART_SUCCESS;

  /* Using IMEM_RANGE values as targets can have multiple IMEM regions
     and a single SCL_IMEM_* value might not exist. */

  if(IMEM_RANGE_START_ADDR != 0) /* there's imem partition to add*/
  {
    result = 
      add_single_ram_partition(usable_ram_part_tbl_ptr,
                               IMEM_RANGE_START_ADDR,
                               (IMEM_RANGE_END_ADDR - IMEM_RANGE_START_ADDR),
                               RAM_PARTITION_READWRITE,
                               RAM_PARTITION_IMEM,
                               RAM_PARTITION_DEFAULT_DOMAIN,
                               RAM_PARTITION_SYS_MEMORY);
  }
  
  return result;
}


/*===========================================================================

**  Function :  add_iram_ram_partition

** ==========================================================================
*/
/*!
* 
* @brief
*    Function adds a partition entry for iram memory into the 
*    ram partition table.
* 
* @param[in] usable_ram_part_tbl_ptr Pointer to ram partition table that 
*                                    should be updated.
*   
* @par Dependencies
*   None
*   
* @retval
*   ram_partition_return_type
*   RAM_PART_SUCCESS 	  - if function is successful.
*   RAM_PART_TABLE_FULL_ERR - if table is full or not enough space in 
*                             the table
* 
* @par Side Effects
*   None
*/
static ram_partition_return_type add_iram_ram_partition
(
  usable_ram_part_table_t usable_ram_part_tbl_ptr
)
{
  ram_partition_return_type result = RAM_PART_SUCCESS;

  if(SCL_IRAM_SIZE != 0) /* there's iram to add*/
  {
    result = add_single_ram_partition(usable_ram_part_tbl_ptr,
                                      SCL_IRAM_BASE,
                                      SCL_IRAM_SIZE,
                                      RAM_PARTITION_READWRITE,
                                      RAM_PARTITION_IRAM,
                                      RAM_PARTITION_DEFAULT_DOMAIN,
                                      RAM_PARTITION_SYS_MEMORY);
  }
  
  return result;
}


/*===========================================================================

**  Function :  add_boot_region_ram_partition

** ==========================================================================
*/
/*!
* 
* @brief
*    Function adds sbl1 DDR memory region into the ram partition table.
* 
* @param[in] usable_ram_part_tbl_ptr Pointer to ram partition table that 
*                                    should be updated.
*   
* @par Dependencies
*   None
*   
* @retval
*   ram_partition_return_type
*   RAM_PART_SUCCESS        - if function is successful.
*   RAM_PART_TABLE_FULL_ERR - if table is full
* 
* @par Side Effects
*   None
*/
static ram_partition_return_type add_boot_region_ram_partition
(
  usable_ram_part_table_t usable_ram_part_tbl_ptr
)
{
  uint32 category = RAM_PARTITION_DEFAULT_CATEGORY;
  ram_partition_return_type result = RAM_PART_SUCCESS;
  
  get_partition_category(usable_ram_part_tbl_ptr,
                         SCL_SBL1_DDR_DATA_BASE,
                         &category);

  if(SCL_SBL1_DDR_SIZE != 0) /* there's boot region partition to add*/
  {
    result = add_single_ram_partition(usable_ram_part_tbl_ptr,
                                      SCL_SBL1_DDR_BASE,
                                      SCL_SBL1_DDR_SIZE,
                                      RAM_PARTITION_READWRITE,
                                      category,
                                      RAM_PARTITION_DEFAULT_DOMAIN,
                                      RAM_PARTITION_BOOT_REGION_MEMORY1);
  }
  
  return result;
}


/*===========================================================================

**  Function :  add_ram_partitions

** ==========================================================================
*/
/*!
* 
* @brief
*    For each type of memory (system memories and boot memory), 
*    this function adds a partition entry in the ram partition table
*    
* @param[in] usable_ram_part_tbl_ptr Pointer to ram partition table that 
*                                    should be updated.
* 
* @par Dependencies
*   None
*   
* @retval
*   ram_partition_return_type
*   RAM_PART_SUCCESS 	  - if function is successful.
*   RAM_PART_TABLE_FULL_ERR - if table is full or not enough space in 
*                             the table
*                             
* @par Side Effects
*   None
*/
static ram_partition_return_type add_ram_partitions 
(
  usable_ram_part_table_t usable_ram_part_tbl_ptr
)
{
  ram_partition_return_type result = RAM_PART_OTHER_ERR;
  
  do
  {
    result = add_system_ram_partitions(usable_ram_part_tbl_ptr);
    if (result != RAM_PART_SUCCESS) 
    {
      break;
    }
    
    result = add_iram_ram_partition(usable_ram_part_tbl_ptr);
    if (result != RAM_PART_SUCCESS) 
    {
      break;
    }
    
    result = add_imem_ram_partition(usable_ram_part_tbl_ptr);
    if (result != RAM_PART_SUCCESS) 
    {
      break;
    }
    
    /* only after system ram partitions are added should we call add appsbl 
    and add boot region, because add_boot_region_ram_partition and
    add_appsbl_ram_partition will use the system ram partitions 
    entries in the table to figure out which ram categories appsbl/boot 
    region memory belong to*/
    result = add_boot_region_ram_partition(usable_ram_part_tbl_ptr); 
    if (result != RAM_PART_SUCCESS) 
    {
      break;
    }

  }while(0);
  
  return result;
}


/*===========================================================================

**  Function :  boot_populate_ram_partition_table

** ==========================================================================
*/
/*!
* 
* @brief
*    Function initializes usable_ram_partition_table in shared memory.
*    Then for each type of memory (system memories,boot memory and appsbl memory)
*    this function adds a partition entry in the ram partition table
*    Once complete, function writes magic numbers and version number into table.    
* 
* @param[in] bl_shared_data Pointer to the shared data passed between functions
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
void boot_populate_ram_partition_table 
( 
  bl_shared_data_type *bl_shared_data 
)
{
  usable_ram_part_table_t usable_ram_part_tbl_ptr = NULL;
  ram_partition_return_type result = RAM_PART_OTHER_ERR;
  
  /* allocate memory for ram partition table in shared memory region */
  usable_ram_part_tbl_ptr = 
      load_ram_partition_table((uint32*)&result);

  BL_VERIFY(result == RAM_PART_SUCCESS && usable_ram_part_tbl_ptr != NULL, 
            (uint16)result|BL_ERROR_GROUP_MPROC);

  /* add all partitions to the table*/
  result = add_ram_partitions(usable_ram_part_tbl_ptr);
          
BL_VERIFY(result == RAM_PART_SUCCESS, (uint16)result|BL_ERROR_GROUP_MPROC);

  usable_ram_part_tbl_ptr->magic1  = RAM_PART_MAGIC1;
  usable_ram_part_tbl_ptr->magic2  = RAM_PART_MAGIC2;
  usable_ram_part_tbl_ptr->version = RAM_PARTITION_VERSION;
}


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
*                                      category.
* 
* @par Side Effects
*   None
*/
ram_partition_return_type get_partition_category 
(
  usable_ram_part_table_t usable_ram_part_tbl_ptr, 
  uintnt mem_base, 
  uint32 *partition_category
)
{
  uint32 counter = 0;
  ram_part_entry_t ram_part_entry_ptr = 0;
  ram_partition_return_type result = RAM_PART_CATEGORY_NOT_EXIST_ERR;
  *partition_category = RAM_PARTITION_DEFAULT_CATEGORY;

  for( ; counter < usable_ram_part_tbl_ptr->num_partitions; counter++)
  { 
    /* retrieve ram part entry info at current counter index */
    ram_part_entry_ptr = &usable_ram_part_tbl_ptr->ram_part_entry[counter];
    /* if mem_base is within partition range of ram_part_entry_ptr then
    * return partition_category of ram_part_entry_ptr
    */
    if(mem_base >= ram_part_entry_ptr->start_address && 
       mem_base <= 
      (ram_part_entry_ptr->start_address + ram_part_entry_ptr->length))
      {
        *partition_category = ram_part_entry_ptr->partition_category;
        result = RAM_PART_SUCCESS;
        break;
      }
  }
  return result;
}
