#ifndef BOOT_RAW_PARTITION_RAMDUMP_H
#define BOOT_RAW_PARTITION_RAMDUMP_H

/*===========================================================================

                         Boot Raw Partition RAM Dump Header File

GENERAL DESCRIPTION
  This header file contains declarations and definitions for memory
  dumps to raw parition.

Copyright 2013, 2014, 2017 by Qualcomm Technologies, Inc.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/13/17   kpa     Added MAX_CRASHDUMP_TABLE_ENTRIES
06/20/14   kedara  Support 64bit compilation using llvm.
07/10/13   dh      Move some defines from c file
03/19/13   dh      Initial version . 

============================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"
#include "boot_target.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

#define SECTION_NAME_LEN 20 
#define RAM_DUMP_VALID_MASK 0x00000001
#define RAM_DUMP_INSUFFICIENT_STORAGE_MASK 0x00000002
#define RAM_DUMP_HEADER_VER          0x00001000
#define RAM_DUMP_SECTION_HEADER_VER  0x00001000
#define RAM_DUMP_HEADER_SIGNATURE    {0x52,0x61,0x77,0x5F,0x44,0x6D,0x70,0x21}
/**
 * This struct represents the header of the whole raw parition ram dump
 * size is 56 bytes
 */
struct PACK (boot_raw_parition_dump_header)
{
  /* Signature indicating presence of ram dump */
  uint8 signature[8];
  
  /* Version number, should be 0x1000*/
  uint32 version;
  
  /* bit 0: dump valid
     bit 1: insufficant storage 
     bit 31:2 reserved, should be 0 */
  uint32 validity_flag;
  
  /* Not used by SBL ram dump */
  uint64 os_data;
  
  /* Not used by SBL ram dump */
  uint8 cpu_context[8];
  
  /* Not used by SBL ram dump */
  uint32 reset_trigger;
  
  /* Total size of the actual dump including headers */
  uint64 dump_size;
  
  /* Total size required */
  uint64 total_dump_size_required;
  
  /* Number of sections included in this dump */
  uint32 sections_count;
};


/**
 * Enum indicating the type of data in each dump section
 */
typedef enum
{
  RAW_PARITION_DUMP_RESERVED = 0,
  
  /* Device memory dump */
  RAW_PARITION_DUMP_DDR_TYPE      = 1, 
  
  /* CPU context, not used */
  RAW_PARITION_DUMP_CPU_CXT_TYPE  = 2,
  
  /* Silicon Vendor specific data */
  RAW_PARITION_DUMP_SV_TYPE      = 3,
  
  /* Force it to uint32 size */
  RAW_PARITION_DUMP_MAX      = 0x7FFFFFFF
  
}boot_raw_partition_dump_section_type;


/* Type specifc information. It's part of section header*/
union PACK(type_specific_info)
{
  /* Base address of a device memory dump */
  uint64 base_addr;
  
  /* Not used by SBL dump */
  uint8 cpu_content[6];
  
  /* Not used by SBL dump */
  uint8 sv_specific[16];
};

/**
 * This struct represents the header of one ram dump section
 *  size is 64 bytes
 */
struct PACK(boot_raw_partition_dump_section_header)
{
  /* bit 0: dump valid
     bit 1: insufficant storage 
     bit 31:2 reserved, should be 0 */
  uint32 validity_flag;
  
  /*Version number*/
  uint32 section_version;
  
  /* Type of this section */
  boot_raw_partition_dump_section_type section_type;

  /* Byte offset to the start of this section's data */
  uint64 section_offset;
  
  /* Total size of the section's data */
  uint64 section_size;
  
  /* Type specific information, 
     we use it to store base address of device memory*/
  union type_specific_info section_info;
  
  /* Name of this section */
  uint8 section_name[SECTION_NAME_LEN];
};


/* Max number of raw dump sections we support */
#define MAX_RAW_DUMP_SECTION_NUM       MAX_CRASHDUMP_TABLE_ENTRIES

/* Size of the overall header */
#define DUMP_HEADER_SIZE  (sizeof(struct boot_raw_parition_dump_header))

/* Size of a single section header */
#define SECTION_HEADER_SIZE  \
                  (sizeof(struct boot_raw_partition_dump_section_header))
                  
/* Max Size limit for Silicon Vendor data type, 
   such as pon status and ddr training data : 4KB */
#define SV_DATA_SIZE_LIMIT   (0x1000)

/* Max Size limit for Silicon Vendor internal memory, such as ocimem: 128MB */
#define SV_MEMORY_SIZE_LIMIT  (0x8000000)


extern struct boot_raw_parition_dump_header raw_dump_header;
extern struct boot_raw_partition_dump_section_header 
                     raw_dump_section_header_table[MAX_RAW_DUMP_SECTION_NUM];


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

**  Function :  boot_ram_dump_to_raw_parition

** ==========================================================================
*/
/*!
* 
* @brief  
*   This routine initiates the Ramdumps to raw partition. Checks for the imem 
*   cookie and go ahead with Ramdumps to raw partition if cookie is present.
*   Setup the Ram regions to be taken as dumps.
*
* @param[in] bl_shared_data Pointer to shared data
*
* @par Dependencies:
*   None
*
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void boot_ram_dump_to_raw_parition(void);


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
void boot_ram_dump_header_init(void);


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
void boot_update_section_header_table_by_index(uint32 i);

#endif  /* BOOT_RAW_PARTITION_RAMDUMP_H */

