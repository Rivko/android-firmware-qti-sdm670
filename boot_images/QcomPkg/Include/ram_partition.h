#ifndef RAM_PARTITION_H
#define RAM_PARTITION_H

#define RAM_PARTITION_H_MAJOR  03
#define RAM_PARTITION_H_MINOR  00

/**
 @file ram_partition.h
 @brief
 RAM partition table definitions.
 
 This header file gives the external definition of the RAM partition table.
 The table holds attributes for each RAM region and is stored in shared memory.
*/ 

/*==============================================================================
                  Copyright 2008-2013 Qualcomm Technologies, Inc.
                            All Rights Reserved.
                    Qualcomm Confidential and Proprietary
================================================================================
                              EDIT HISTORY

================================================================================
when       who     what, where, why
--------   ---     -------------------------------------------------------------
09/18/15   bh      Add support for v1 ram partition table for STI
03/23/15   kedara  Added field "available_length" to ram partition table, increase
                   version number.
10/30/14   vk      Move to latest revision including highest_bak_bit
07/15/13   vk      Merge and support v0 and v1 in UEFI
06/17/13   dh      Revision for V1 to support 64 bits addresses
                   Update RAM_PARTITION_H_MAJOR to 0x2, RAM_PARTITION_H_MINOR to 0x1
                   RAM_PARTITION_VERSION to 0x1
08/03/11   dh      Remove unused enum type in ram_partition_category_t
06/22/11   dh      Add a RAM_PARTITION_SDRAM type to ram_partition_category_t
05/23/11   plc     Change "RESERVED2" of ram_partition_entry type to 
                   num_partitions
10/26/10   dxiang  Doxygen markup standards changes 
09/21/10   dh      Added EBI1 Channel information in ram_partition_category_t 
                   for future targets,update minor version to 03
08/06/10   dh      Remove RAM_PARTITION_SYS_MEMORY1 to RAM_PARTITION_SYS_MEMORY6
                   having only RAM_PARTITION_SYS_MEMORY instead. Minor version 
                   is not changed since last change has not been picked up.
07/21/10   dh      Added partition_type field in ram_partition_entry
                   Added ram_parition_type_t enum for partition_type.
06/15/10   sl      Removed API headers, which will be private
03/03/10   aus     Common file across targets
01/08/09   aus     Porting to 7x30
10/15/09   tw      Expose API header from private header to public header
04/03/09   kedar   Added usable_ram_part_table typedef for
                   usable_ram_partition_table struct
12/05/08   dhaval  Initial Creation.
==============================================================================*/

#include "com_dtypes.h"

/** @addtogroup ram_partition_table_api
@{ */

/**
 * Total length of zero filled name string. This is not a C
 * string, as it can occupy the total number of bytes, and if
 * it does, it does not require a zero terminator. It cannot
 * be manipulated with standard string handling library functions.
 */
#define RAM_PART_NAME_LENGTH 16

/** 
 * Number of RAM partition entries which are usable by APPS.
 */
#define RAM_NUM_PART_ENTRIES 32

/**
 * @name Magic numbers
 * Used in identifying valid RAM partition table.
 */
/** @{ */
#define RAM_PART_MAGIC1     0x9DA5E0A8
#define RAM_PART_MAGIC2     0xAF9EC4E2
/** @} */

/**
 * Must increment this version number whenever RAM structure of 
 * RAM partition table changes.
 */
#define RAM_PARTITION_VERSION   0x2

/**
 * Value which indicates the partition can grow to fill the
 * rest of RAM. Must only be used on the last partition.
 */
#define RAM_PARTITION_GROW  0xFFFFFFFF

/**
 RAM partition API return types.
 */
typedef enum
{
  RAM_PART_SUCCESS = 0,             /**< Successful return from API */
  RAM_PART_NULL_PTR_ERR,            /**< Partition table/entry null pointer */
  RAM_PART_OUT_OF_BOUND_PTR_ERR,    /**< Partition table pointer is not in SMEM */
  RAM_PART_TABLE_EMPTY_ERR,         /**< Trying to delete entry from empty table */
  RAM_PART_TABLE_FULL_ERR,          /**< Trying to add entry to full table */
  RAM_PART_CATEGORY_NOT_EXIST_ERR,  /**< Partition doesn't belong to any memory category */
  RAM_PART_OTHER_ERR,               /**< Unknown error */
  
  /**Please add new enum value here and keep the value   
  incremental to the predecessor*/
  
  RAM_PART_RETURN_MAX_SIZE = 0x7FFFFFFF
} ram_partition_return_type;

/** 
 RAM partition attributes.
 */
typedef enum 
{
  RAM_PARTITION_DEFAULT_ATTRB = ~0,  /**< No specific attribute definition */
  RAM_PARTITION_READ_ONLY = 0,       /**< Read-only RAM partition */
  RAM_PARTITION_READWRITE,           /**< Read/write RAM partition */
  
  /**Please add new enum value here and keep the value   
     incremental to the predecessor*/
  
  RAM_PARTITION_ATTRIBUTE_MAX_SIZE = 0x7FFFFFFF
} ram_partition_attribute_t;

/**
 RAM partition categories.
 */
typedef enum 
{
  RAM_PARTITION_DEFAULT_CATEGORY = ~0,  /**< No specific category defintion */
  RAM_PARTITION_IRAM=4,                   /**< IRAM RAM partition */
  RAM_PARTITION_IMEM=5,                   /**< IMEM RAM partition */
  RAM_PARTITION_SDRAM=14,                  /**< SDRAM type without specific bus information**/

  /**Please add new enum value here and keep the value   
     incremental to the predecessor, do NOT modify the existing values*/

  RAM_PARTITION_CATEGORY_MAX_SIZE = 0x7FFFFFFF
} ram_partition_category_t;

/**
 RAM Partition domains.
 
 @note
 - For shared RAM partition, domain value would be 0b11:\n
 RAM_PARTITION_APPS_DOMAIN | RAM_PARTITION_MODEM_DOMAIN.
 */
typedef enum 
{
  RAM_PARTITION_DEFAULT_DOMAIN = 0,  /**< 0b00: No specific domain defintion */
  RAM_PARTITION_APPS_DOMAIN = 1,     /**< 0b01: APPS RAM partition */
  RAM_PARTITION_MODEM_DOMAIN = 2,    /**< 0b10: MODEM RAM partition */
  
  /**Please add new enum value here and keep the value   
     incremental to the predecessor, do NOT modify the existing values*/
  
  RAM_PARTITION_DOMAIN_MAX_SIZE = 0x7FFFFFFF
} ram_partition_domain_t;

/**
 RAM Partition types.
 
 @note
 - The RAM_PARTITION_SYS_MEMORY type represents DDR rams that are attached
 to the current system.
 */
typedef enum 
{
  RAM_PARTITION_SYS_MEMORY = 1,        /**< system memory */
  RAM_PARTITION_BOOT_REGION_MEMORY1,   /**< boot loader memory 1 */
  RAM_PARTITION_BOOT_REGION_MEMORY2,   /**< boot loader memory 2, reserved */
  RAM_PARTITION_APPSBL_MEMORY,         /**< apps boot loader memory */
  RAM_PARTITION_APPS_MEMORY,           /**< apps  usage memory */  
  
  /**Please add new enum value here and keep the value   
     incremental to the predecessor, do NOT modify the existing values*/
    
  RAM_PARTITION_TYPE_MAX_SIZE = 0x7FFFFFFF
} ram_partition_type_t;

/**
 @brief
 Holds information for an entry in the RAM partition table.
 */
struct ram_partition_entry 
{
  char name[RAM_PART_NAME_LENGTH];  /**< Partition name, unused for now */
  uint64 start_address;             /**< Partition start address in RAM */
  uint64 length;                    /**< Partition length in RAM in Bytes */
  uint32 partition_attribute;       /**< Partition attribute */
  uint32 partition_category;        /**< Partition category */
  uint32 partition_domain;          /**< Partition domain */
  uint32 partition_type;            /**< Partition type */
  uint32 num_partitions;            /**< Number of partitions on device */
  uint32 hw_info;                   /**< hw information such as type and frequency */
  uint8 highest_bank_bit;           /**< Highest bit corresponding to a bank */
  uint8 reserve0;                   /**< Reserved for future use */
  uint8 reserve1;                   /**< Reserved for future use */
  uint8 reserve2;                   /**< Reserved for future use */ 
  uint32 reserved5;                 /**< Reserved for future use */
  uint64 available_length;          /**< Available Partition length in RAM in Bytes */
};

typedef struct ram_partition_entry *ram_part_entry_t;
typedef struct ram_partition_entry *ram_part_entry_ptr_type;

/**
 @brief
 Defines the RAM partition table structure

 @note
 - No matter how you change the structure, do not change the placement of the 
 first four elements so that future compatibility will always be guaranteed
 at least for the identifiers.
 
 @note
 - The other portion of the structure may be changed as necessary to accommodate
 new features. Be sure to increment version number if you change it.
 */
struct usable_ram_partition_table 
{
  uint32 magic1;          /**< Magic number to identify valid RAM partition table */
  uint32 magic2;          /**< Magic number to identify valid RAM partition table */
  uint32 version;         /**< Version number to track structure definition changes
                               and maintain backward compatibilities */
  uint32 reserved1;       /**< Reserved for future use */

  uint32 num_partitions;  /**< Number of RAM partition table entries */
  
  uint32 reserved2;       /** < Added for 8 bytes alignment of header */
  
  /** RAM partition table entries */ 
  struct ram_partition_entry ram_part_entry[RAM_NUM_PART_ENTRIES];
};
typedef struct usable_ram_partition_table *usable_ram_part_table_t;
typedef struct usable_ram_partition_table usable_ram_part_table;

/* Version 1 structure 32 Bit */

/**
 @brief
 Holds information for an entry in the RAM partition table.
 */
struct ram_partition_entry_v1 
{
  char name[RAM_PART_NAME_LENGTH];  /**< Partition name, unused for now */
  uint64 start_address;             /**< Partition start address in RAM */
  uint64 length;                    /**< Partition length in RAM in Bytes */
  uint32 partition_attribute;       /**< Partition attribute */
  uint32 partition_category;        /**< Partition category */
  uint32 partition_domain;          /**< Partition domain */
  uint32 partition_type;            /**< Partition type */
  uint32 num_partitions;            /**< Number of partitions on device */
  uint32 hw_info;                   /**< hw information such as type and frequency */
  uint32 reserved4;                 /**< Reserved for future use */
  uint32 reserved5;                 /**< Reserved for future use */
};
typedef struct ram_partition_entry_v1 *ram_part_entry_t_v1;
typedef struct ram_partition_entry_v1 *ram_part_entry_ptr_type_v1;

/**
 @brief
 Defines the RAM partition table structure

 @note
 - No matter how you change the structure, do not change the placement of the 
 first four elements so that future compatibility will always be guaranteed
 at least for the identifiers.
 
 @note
 - The other portion of the structure may be changed as necessary to accommodate
 new features. Be sure to increment version number if you change it.
 */
struct usable_ram_partition_table_v1 
{
  uint32 magic1;          /**< Magic number to identify valid RAM partition table */
  uint32 magic2;          /**< Magic number to identify valid RAM partition table */
  uint32 version;         /**< Version number to track structure definition changes
                               and maintain backward compatibilities */
  uint32 reserved1;       /**< Reserved for future use */

  uint32 num_partitions;  /**< Number of RAM partition table entries */
  
  uint32 reserved2;       /** < Added for 8 bytes alignment of header */
  
  /** RAM partition table entries */ 
  struct ram_partition_entry_v1 ram_part_entry_v1[RAM_NUM_PART_ENTRIES];
};
typedef struct usable_ram_partition_table_v1 *usable_ram_part_table_t_v1;
typedef struct usable_ram_partition_table_v1 usable_ram_part_table_v1;

/** @} */ /* end_addtogroup */

/* End of Version 1 structure 32 Bit */

/* Version 0 structure 32 Bit */

/**
 @brief
 Holds information for an entry in the RAM partition table.
 */
struct ram_partition_entry_v0 
{
  char name[RAM_PART_NAME_LENGTH];  /**< Partition name, unused for now */
  uint32 start_address;             /**< Partition start address in RAM */
  uint32 length;                    /**< Partition length in RAM in Bytes */
  uint32 partition_attribute;       /**< Partition attribute */
  uint32 partition_category;        /**< Partition category */
  uint32 partition_domain;          /**< Partition domain */
  uint32 partition_type;            /**< Partition type */
  uint32 num_partitions;            /**< Number of partitions on device */
  uint32 reserved3;                 /**< Reserved for future use */
  uint32 reserved4;                 /**< Reserved for future use */
  uint32 reserved5;                 /**< Reserved for future use */
};
typedef struct ram_partition_entry_v0 *ram_part_entry_t_v0;
typedef struct ram_partition_entry_v0 *ram_part_entry_ptr_type_v0;

/**
 @brief
 Defines the RAM partition table structure

 @note
 - No matter how you change the structure, do not change the placement of the 
 first four elements so that future compatibility will always be guaranteed
 at least for the identifiers.
 
 @note
 - The other portion of the structure may be changed as necessary to accommodate
 new features. Be sure to increment version number if you change it.
 */
struct usable_ram_partition_table_v0
{
  uint32 magic1;          /**< Magic number to identify valid RAM partition table */
  uint32 magic2;          /**< Magic number to identify valid RAM partition table */
  uint32 version;         /**< Version number to track structure definition changes
                               and maintain backward compatibilities */
  uint32 reserved1;       /**< Reserved for future use */

  uint32 num_partitions;  /**< Number of RAM partition table entries */
  
  /** RAM partition table entries */ 
  struct ram_partition_entry_v0 ram_part_entry_v0[RAM_NUM_PART_ENTRIES];
};
typedef struct usable_ram_partition_table_v0 *usable_ram_part_table_t_v0;
typedef struct usable_ram_partition_table_v0 usable_ram_part_table_v0;


/** @} */ /* end_addtogroup */

/* End of Version 0 structure 32 Bit */

#endif /* RAM_PARTITION_H */

