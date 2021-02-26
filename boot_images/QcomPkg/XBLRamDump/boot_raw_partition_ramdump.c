/*===========================================================================

                         Boot Raw RAM Dump File

GENERAL DESCRIPTION
  This header file contains declarations and definitions for memory
  dumps to raw parition.

Copyright 2013-2015, 2017 by Qualcomm Technologies, Inc.  All Rights Reserved.
============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when       who             what, where, why
--------   ---         --------------------------------------------------
06/23/17   ds          coldplug changes
06/28/16   kpa         Fix rawdump base address to 64-bit
09/10/15   kpa         remove redundant mmu_flush_cache
06/20/14   kpa         Support 64 bit compilation using llvm
04/02/14   ck          Fixed KW issues
03/18/14   ck          Updated boot_hw_reset calls as they now take a reset type parameter
03/14/14   ck          Removed boot_dload_is_dload_mode_set call from boot_ram_dump_to_raw_parition
07/10/13   dh          Move some defines to header file, refactor some functions
06/26/13   dh          Move dload_mem_debug_init earlier to avoid nested partition opening
06/13/13   dh          Update SV_DATA_SIZE_LIMIT to 4kb so ddr training data 
                       will be sv data
05/03/13   dh          In case of insufficent storage , dump as much as we can
                       and reset after that
04/02/13   dh          Add logic to toggle led when writing dump
03/19/13   dh          Initial version  

===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "boot_raw_partition_ramdump.h"
#include "boot_dload_debug.h"
#include "boot_sdcc.h"
#include "boot_extern_coldplug_interface.h"
#include "boothw_target.h"
#include "boot_cache_mmu.h"
#include "boot_flash_dev_if.h"
#include "boot_shared_imem_cookie.h"
#include "boot_visual_indication.h"
#include "boot_dload.h"
#include "boot_util.h"
#include "LoaderUtils.h"
#include "boot_target.h"
#include "boot_extern_pmic_interface.h"
#include "boot_logger.h"

/* Size of data to write for each call to coldplug write(2MB)*/
#define RAM_DUMP_WRITE_SIZE (0x200000)

/* GUID for ram dump parition */
extern uint8 ram_dump_partition_id[];

/* Allocate header buffer in ddr */
/* Raw dump header */
struct boot_raw_parition_dump_header raw_dump_header SECTION(".bss.BOOT_UNCACHED_DDR_ZI_ZONE");

/* Table that stores the raw dump section headers */
struct boot_raw_partition_dump_section_header 
      raw_dump_section_header_table[MAX_RAW_DUMP_SECTION_NUM] SECTION(".bss.BOOT_UNCACHED_DDR_ZI_ZONE");

/* Number of sections we need to dump */
static uint32 ram_dump_sections_num;

/* Total number of bytes on ram dump parition */
static uint64 partition_size;

/* Pointer points to the parition offset we are going to write data to*/
static uint64 curr_partition_offset;

/* Dump header for first DUMP */
struct boot_raw_parition_dump_header first_dump_header;

/*=========================================================================
                            
                       FUNCTION DEFINITIONS

=========================================================================*/

/*===========================================================================

**  Function :  boot_raw_ram_dump_error_handler

** ==========================================================================
*/
/*!
* 
* @brief  
*   Utility function that handles error in raw ram dump process
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
static void boot_raw_ram_dump_error_handler()
{
  /* Turn on LED if there's an write error and enter error handler*/
  boot_enable_led(RAM_DUMP_LED, TRUE);
  BL_VERIFY(0, BL_ERR_RAM_DUMP_FAIL|BL_ERROR_GROUP_BOOT);
}

/*===========================================================================

**  Function :  boot_ram_dump_coldplug_write

** ==========================================================================
*/
/*!
* 
* @brief  
*   Utility function that writes data to storage and toggle led in between writes
*
* @par Dependencies
*   boot_toggle_led_init needs to be called 
*
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
static void boot_ram_dump_coldplug_write(void *buf_addr, uint64 offset, uint32 write_size)
{
  uint32 bytes_to_write = 0;
  
  while(write_size > 0)
  {
    bytes_to_write = write_size > RAM_DUMP_WRITE_SIZE ? RAM_DUMP_WRITE_SIZE : write_size;
  
    if (dev_sdcc_write_bytes(buf_addr, offset, bytes_to_write, GEN_IMG) != TRUE)
    {
      boot_raw_ram_dump_error_handler();
    }
            
    /*Toggle led in between writes*/
    boot_toggle_led();
  
    write_size -= bytes_to_write;
    buf_addr = (void *)(uintnt)((uint64)buf_addr + bytes_to_write);
      offset += bytes_to_write;
  }
}

/*===========================================================================

**  Function :  boot_ram_dump_read_header

** ==========================================================================
*/
/*!
* 
* @brief  
*   Utility function that reads the dump header at given offset
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
static void boot_ram_dump_read_header(void *buf_addr, uint64 offset)
{  
  if (dev_sdcc_read_bytes(buf_addr, offset, sizeof(struct boot_raw_parition_dump_header),
      boot_get_coldplug_dev_handle(GEN_IMG)) != TRUE)
  {
    boot_raw_ram_dump_error_handler();
  }
}

/*===========================================================================

**  Function :  dump_first_header

** ==========================================================================
*/
/*!
* 
* @brief  
*   Utility function that writes the dump header, always at offset zero
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
static void dump_first_header(void)
{ 
  boot_ram_dump_coldplug_write(&first_dump_header, 
                              0, 
                              DUMP_HEADER_SIZE);
}

/*===========================================================================

**  Function :  dump_overall_headers

** ==========================================================================
*/
/*!
* 
* @brief  
*   Utility function that writes the overall header to storage
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
static void dump_overall_headers()
{
  boot_ram_dump_coldplug_write(&raw_dump_header, 
                              raw_dump_header.total_dump_size_required, 
                              DUMP_HEADER_SIZE);
  dump_first_header();
}


/*===========================================================================

**  Function :  dump_section_headers

** ==========================================================================
*/
/*!
* 
* @brief  
*   Utility function that writes the section headers to storage
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
static void dump_section_headers()
{
  boot_ram_dump_coldplug_write(raw_dump_section_header_table, 
                              (raw_dump_header.total_dump_size_required + DUMP_HEADER_SIZE), 
                              SECTION_HEADER_SIZE * ram_dump_sections_num);
}


/*===========================================================================

**  Function :  get_raw_ram_dump_required_size

** ==========================================================================
*/
/*!
* 
* @brief  
*  Returns the required total raw partition ram dump size in bytes
*
* @par Dependencies
*   dload_mem_debug_init needs to be called 
*
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
static uint64 get_raw_ram_dump_required_size()
{
  uint32 i = 0;
  /* We first need space to store overall header and section header*/
  uint64 require_size = DUMP_HEADER_SIZE + 
                        SECTION_HEADER_SIZE * dload_mem_debug_num_ent();
  
  for(; i < dload_mem_debug_num_ent() ; i++)
  {
    require_size += dload_mem_debug_mem_length(i);
  }
  
  return require_size;
}


/*===========================================================================

**  Function :  boot_update_section_header_table_by_index

** ==========================================================================
*/
/*!
* 
* @brief  
*  Given an index of memory dump sections, fill up the information on
*  section header table
*
* @par Dependencies
*   dload_mem_debug_init needs to be called 
*
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void boot_update_section_header_table_by_index(uint32 i)
{
   uint64 curr_sec_size = 0;
   uintnt curr_sec_base_addr = 0;
   char * debug_filename = NULL;
   curr_sec_size = dload_mem_debug_mem_length(i);
   curr_sec_base_addr = dload_mem_debug_mem_base(i);
    
    /* If section size is less than 2KB then we know it's SV type and we 
       don't need specify base address */
    if(curr_sec_size < SV_DATA_SIZE_LIMIT)
    {
      raw_dump_section_header_table[i].section_type = RAW_PARITION_DUMP_SV_TYPE;      
    }
    /* If section size is less than 128MB but larger than 2KB then it's internal
       system memory, we need to add base address */
    else if(curr_sec_size < SV_MEMORY_SIZE_LIMIT)
    {
      raw_dump_section_header_table[i].section_type = RAW_PARITION_DUMP_SV_TYPE;       
      raw_dump_section_header_table[i].section_info.base_addr = curr_sec_base_addr;
    }
    /* For sections that's larger than 128MB it's DDR */
    else
    {
      raw_dump_section_header_table[i].section_type = RAW_PARITION_DUMP_DDR_TYPE;       
      raw_dump_section_header_table[i].section_info.base_addr = curr_sec_base_addr;      
    }
    
    raw_dump_section_header_table[i].section_version = RAM_DUMP_SECTION_HEADER_VER;
    /* section offset should be alway relative to support multiple dumps */ 
    raw_dump_section_header_table[i].section_offset = (curr_partition_offset - raw_dump_header.total_dump_size_required);
    raw_dump_section_header_table[i].section_size = curr_sec_size;
    
    /* Copy the file name to section name */
    debug_filename = dload_mem_debug_filename(i);
    BL_VERIFY(debug_filename,
              BL_ERR_NULL_PTR_PASSED|BL_ERROR_GROUP_BOOT);

    strlcpy((char *)&raw_dump_section_header_table[i].section_name, 
            debug_filename, 
            SECTION_NAME_LEN);
}


/*===========================================================================

**  Function :  boot_process_raw_ram_dump_sections

** ==========================================================================
*/
/*!
* 
* @brief  
*   This function will write all the section data to partition sequentially
*  It will update the section header with the operation result.
*  If there is not enough space left it will return false.
*
* @par Dependencies
*   dload_mem_debug_init needs to be called 
*
* @retval
*   False if there's not enough space left for one section's data
*   True if all sections' data has been written
* 
* @par Side Effects
*   None
* 
*/
static boot_boolean boot_process_raw_ram_dump_sections()
{
  uint32 i = 0;
  boot_boolean dump_success = TRUE;
  uint64 curr_sec_size = 0;
  uintnt curr_sec_base_addr = 0;
  uint64 free_space_size = 0;
  
  /* We start writing right after all the headers */
  curr_partition_offset = DUMP_HEADER_SIZE + (SECTION_HEADER_SIZE * ram_dump_sections_num);
  curr_partition_offset += raw_dump_header.total_dump_size_required;
                                
  /* Initial free space without all the headers */
  free_space_size = partition_size - curr_partition_offset;
  
 /* Iterate through mem_debug_info table and process each section*/
  for(; i < ram_dump_sections_num ; i++)
  {
    /* Set the size ,base and offset in current section's header */
    curr_sec_size = dload_mem_debug_mem_length(i);
    curr_sec_base_addr = dload_mem_debug_mem_base(i);
    
    /* Update the section header */
    boot_update_section_header_table_by_index(i);
    
    /* Now section header info is updated, 
       we need to write the raw section data to storage */
    if(free_space_size >= curr_sec_size)
    {
      boot_ram_dump_coldplug_write((void*)(uintnt)curr_sec_base_addr, 
                                  curr_partition_offset, 
                                  curr_sec_size);      
      
      /* Now section data has been written*/
      /* update the validility flag of the section header */ 
      raw_dump_section_header_table[i].validity_flag |= RAM_DUMP_VALID_MASK;
      
      /* Update the actual size and section count we have dumped in overall header */
      raw_dump_header.dump_size += curr_sec_size;
      raw_dump_header.sections_count++;
      
      /* Update free space and offset */
      curr_partition_offset += curr_sec_size;
      free_space_size -= curr_sec_size;
      
    }
    else
    {
      /* There is not enough space left */
      raw_dump_section_header_table[i].validity_flag |= 
                                    RAM_DUMP_INSUFFICIENT_STORAGE_MASK;
                                    
      /* Update the section size as the size we can dump */
      raw_dump_section_header_table[i].section_size = free_space_size;     
      
      /* Use up all the free space for this section */
      boot_ram_dump_coldplug_write((void*)(uintnt)curr_sec_base_addr, 
                                  curr_partition_offset, 
                                  free_space_size);
                                  
      /* Update the actual size and section count we have dumped in overall header */
      raw_dump_header.dump_size += free_space_size; 
      raw_dump_header.sections_count++;
      
      /* Update section headers to indicate insufficient storage of current section*/                                  
      dump_section_headers();
      
      dump_success = FALSE;
      
      break;
    }
    
    /* Update the overall header and section table headers */
    dump_overall_headers();
    dump_section_headers();
  }
  
  return dump_success;
}

/*===========================================================================

**  Function :  boot_process_raw_ram_dump

** ==========================================================================
*/
/*!
* 
* @brief  
*  This routine reads the memory regions and writes to the ram dump parition in emmc
*
* @par Dependencies
*   coldplug driver needs to be initialized 
*
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
static void boot_process_raw_ram_dump()
{
  uint32 headers_required_size = 0; 
  
  /* We first need space to store overall header and section header*/
  ram_dump_sections_num =  dload_mem_debug_num_ent();
  
  /* Make sure the number of sections we need to dump doesn't exceed
     the max we support */
  if(ram_dump_sections_num > MAX_RAW_DUMP_SECTION_NUM)
  {
    boot_raw_ram_dump_error_handler();
  }
            
  headers_required_size = DUMP_HEADER_SIZE + 
                          (SECTION_HEADER_SIZE * ram_dump_sections_num); 
  
  /* Initialize the header */
  boot_ram_dump_header_init();
  
  /* Write a fresh copy of overall header first to indicate new ram dump */
  dump_overall_headers();
          
  if(partition_size > headers_required_size)
  {

    boot_toggle_led_init();

    /* We can at least dump all the headers */
    raw_dump_header.dump_size = headers_required_size;
    
    /* Write a fresh copy of all the section headers to indicate new ram dump*/
    dump_section_headers();
    
    /* Dump each sections */
    if(boot_process_raw_ram_dump_sections() == TRUE)
    {
      /* If all sections finished successully set header to valid */
      raw_dump_header.validity_flag |= RAM_DUMP_VALID_MASK;      
    
    /* align dump size to next 1MB boundary */
	  if((raw_dump_header.dump_size  & 0xFFFFF) != 0x0)
	  {
      /* dump_size is 64-bit, hence for all practical purpose need not check integer overflow */
      raw_dump_header.dump_size = (raw_dump_header.dump_size & ~0xFFFFF) + 0x100000;
	  }
    }
    else
    {
      /* if it returns false we know there's not enough space*/
      raw_dump_header.validity_flag |= RAM_DUMP_INSUFFICIENT_STORAGE_MASK;
    }
    
  }
  else
  {
    /* There is not enough space to store the section headers.
      Set the insufficent storage bit */
    raw_dump_header.dump_size = DUMP_HEADER_SIZE;
    raw_dump_header.validity_flag |= RAM_DUMP_INSUFFICIENT_STORAGE_MASK;
  }

  /* first dump */  
  if(first_dump_header.total_dump_size_required == 0)
    qmemcpy(&first_dump_header, &raw_dump_header, sizeof(raw_dump_header));    
  	
  first_dump_header.total_dump_size_required += raw_dump_header.dump_size;

  if(first_dump_header.total_dump_size_required >= partition_size)
    first_dump_header.total_dump_size_required = 0; 
  
  /* increment the number of dumps counter */
  first_dump_header.os_data++;
  
  /* Now dump is finished, update the overall header */
  dump_overall_headers();

} /* boot_process_raw_ram_dump */


/*===========================================================================

**  Function :  boot_ram_dump_header_init

** ==========================================================================
*/
/*!
* 
* @brief  
*  Initialize the ram dump header 
*
* @par Dependencies
*   dload_mem_debug_init needs to be called 
*
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void boot_ram_dump_header_init()
{
  uint8 ram_dump_signature[] = RAM_DUMP_HEADER_SIGNATURE;
  
  /* Clear the raw ram dump header structures */
  qmemset(&raw_dump_header, 0x0, sizeof(raw_dump_header));
  qmemset(&raw_dump_section_header_table[0], 0x0, sizeof(raw_dump_section_header_table));
  
  /* Initialize the header signature and version number */
  qmemcpy(&raw_dump_header.signature, &ram_dump_signature, sizeof(ram_dump_signature));
  raw_dump_header.version = RAM_DUMP_HEADER_VER;
  
  /* Set the total dump size required and initialize section count to 0*/
  raw_dump_header.total_dump_size_required = 
                                  get_raw_ram_dump_required_size();
  /* Set the section count to 0 */
  raw_dump_header.sections_count = 0;
  
  /* Indicate whether minidump or fulldump in  */
  raw_dump_header.os_data = ((boot_dload_read_saved_cookie() & (SBL_MINIDUMP_MODE_BIT_MASK|SBL_DLOAD_MODE_BIT_MASK)) << 16);
  
  boot_ram_dump_read_header(&first_dump_header,0); // read header from first entry
  if(strncmp((const char *)first_dump_header.signature,(const char *)raw_dump_header.signature,sizeof(ram_dump_signature)) == 0)
  {
    raw_dump_header.total_dump_size_required = first_dump_header.total_dump_size_required; // handle overflow/wrap-around case
  }
  else
  {
    // no valid header at offset-0, this is the first entry 
    raw_dump_header.total_dump_size_required = 0;
    qmemcpy(&first_dump_header, &raw_dump_header, sizeof(raw_dump_header));    
  }
}


/*===========================================================================

**  Function :  boot_ram_dump_to_raw_parition

** ==========================================================================
*/
/*!
* 
* @brief :  
*  This routine Checks for the raw partition ram dump cookie 
*  and initiate the ram dumps to raw partition. It will reset the device
*  in case of succesful ram dump or in error of insufficent storage.
*
*
* @param[in] bl_shared_data Pointer to shared data
* 
* @par Dependencies:
* 
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void boot_ram_dump_to_raw_parition()
{
  boot_flash_trans_if_type *trans_if = NULL;
  
  /* Only perform ram dump if uefi dump cookie is set AND 
     memory debug is allowed */
  if((boot_shared_imem_cookie_ptr != NULL) &&
     (boot_shared_imem_cookie_ptr->uefi_ram_dump_magic == BOOT_RAW_RAM_DUMP_MAGIC_NUM) &&
     dload_mem_debug_supported())
  {

    /* Initialize the debug memory regions array
       This has to be done first since we may open ddr training data partition*/
    dload_mem_debug_init();
        
    /* First try to find the ram dump parition */
    boot_flash_configure_target_image(ram_dump_partition_id);

    trans_if = boot_flash_dev_open_image(GEN_IMG);

    /* If this partition exists */
    if(trans_if != NULL)
    {
      partition_size = boot_coldplug_get_partition_size_by_image_id(GEN_IMG);
      
      /* We can only proceed if this partition can hold at least the header 
         and if memory dump is allowed */
      if(partition_size >= sizeof(raw_dump_header))
      {        
        /* Process the dump */
        boot_process_raw_ram_dump();
    
        /* At this point we either finished saving the dump or there is 
            insufficent storage space , reset the device 
        */
        boot_hw_reset(BOOT_HARD_RESET_TYPE); 
      }
      
      /* close parition*/
      boot_flash_dev_close_image(&trans_if);
    }
    /* Continue to regular dload mode if raw dump partition doesn't exist */ 
  }
  
}/* boot_ram_dump_to_raw_parition*/

