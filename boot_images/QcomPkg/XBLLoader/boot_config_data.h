#ifndef BOOT_CONFIG_DATA_H
#define BOOT_CONFIG_DATA_H

/*===========================================================================

                    BOOT CONFIG DATA TABLE DEFINITIONS

DESCRIPTION
  Contains definition of the config data table which is stored in eeprom

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2010-2015 by Qualcomm Technologies, Inc.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    
    
when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/27/15   kpa     Added prototypes for boot_cdt_contains_ddr_cdb and
                   boot_config_data_table_default_init
06/09/14   kpa     Update to support 64-bit MSM8996, different PACK syntax
12/20/13   kpa     Added boot_get_eepromII_info
12/05/12   jz      Added boot_update_config_data_table
10/25/11   dh      Add boot_copy_config_data_block
10/18/10   dh      Add cdt_info structure
09/08/10   dh      Add logic to read EEPROM
08/19/10   dh      Initial Creation.

===========================================================================*/
/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"
#include "boot_sbl_shared.h"

/*  CDT FORMAT  */ 
/*===========================================================================
magic number: uint32
version     : uint16
reserved 1  : 32 bits
reserved 2  : 32 bits

block0 meta data: 32 bits (see BLOCK META DATA FORMAT for detail)
block1 meta data: 32 bits
....
blockN meta data: 32 bits

block0 data : user defined length 
block1 data : user defined length 
....
blockN data : user defined length 
=============================================================================*/

/* BLOCK META DATA FORMAT*/
/*=============================================================================
data offset      : uint16
block size       : uint16
=============================================================================*/

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/
/**
 * 4 bytes.
 * Used in identifying valid cdt table.
 * It's the null ended ascii string "CDT"
 */
#define CONFIG_DATA_MAGIC 0x00544443


/** 
 * max size of the cdt in memory in bytes
 */
#define CONFIG_DATA_TABLE_MAX_SIZE 1024


/**
 * structure that holds an pointer to cdt and its actual size in bytes
 */
struct cdt_info
{
  /*size of the whole table*/
  uint32 size;
  /*pointer to array that contains the whole table's data*/
  uint8 *cdt_ptr;
};


/**
 * Structure that defines the header of configuration data table.
 */
struct PACK(cdt_header)
{
  uint32 magic;         /**< Magic number to identify valid cdt table */
  uint16 version;       /**< Version number to track structure definition 
                             changes and maintain backward compatibilities */
  uint32 reserved1;     /**< Reserved */
  uint32 reserved2;     /**< Reserved */
  /*pack the struct because the compiler defults at 4 bytes alignement*/
}; 


/**
 * Structure that defines the meta data of each block
 */
struct PACK(cdb_meta)
{
  uint16 offset;   /**< offset to the first byte of data of this block, 
                        it's relative to the starting location of cdt */
  uint16 size;     /**< size of this block's data in bytes*/
  /*pack the struct because the compiler defults at 4 bytes alignement*/
}; 


/**
 * Index of configuration data block
 */
typedef enum
{
  CONFIG_DATA_BLOCK_INDEX_V1_PLATFORM_ID = 0,
  CONFIG_DATA_BLOCK_INDEX_V1_DDR = 1,  
  /*force 4 bytes enum size*/
  CONFIG_DATA_BLOCK_INDEX_TYPE_MAX_SIZE = 0x7FFFFFFF
}config_data_block_index_type;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

**  Function :  get_config_data_block

** ==========================================================================
*/
/*!
* 
* @brief
*    given an configuration data block index , return a pointer to beginning 
*    of that cdb with the length of cdb.
* 
* @param[in] uint8* pointer to the raw cdt table
*
* @param[in] uint32 Index of the data block.
*
* @param[out] uint32* length of the data block in bytes
* 
* @return   pointer to the beginning of request cdb. 
*           Null if error or cdb doesn't exist 
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
uint8* boot_get_config_data_block
(
  const uint8 *raw_cdt, 
  uint32 index, 
  uint32 *length
);


/*===========================================================================

**  Function :  boot_copy_config_data_block

** ==========================================================================
*/
/*!
* 
* @brief
*   Copy a CDB data into a CDT table to the given CDB index. The source CDB data
*   size must match the target CDB size in the CDT.
* 
* @param[in] uint8* pointer to the destination CDT
*
* @param[in] uint32 index of the destination CDB
* 
* @param[in] uint8* pointer to source CDB
*
* @param[in] uint32 size of the source CDB
* 
* @return   TRUE if copy is successful, FALSE if not
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
boot_boolean boot_copy_config_data_block
(
  uint8 *dst_cdt, 
  uint32 cdb_index, 
  const uint8 *src_cdb_data, 
  uint32 src_cdb_size
);


/*===========================================================================

**  Function :  boot_config_data_table_init

** ==========================================================================
*/
/*!
* 
* @brief
*     Initialize the configuration data table. If EEPROM device is present , copy
*     the table to memory, if not, use the predefined table which is linked in
*     at compile time.
* 
* @param[in] bl_shared_data_type* sbl shared data pointer
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
void boot_config_data_table_init(bl_shared_data_type*);

/*===========================================================================

**  Function :  boot_config_data_table_default_init

** ==========================================================================
*/
/*!
* 
* @brief
*     Initialize the configuration data table to default compiled values. 
*     This allows DDR driver to be passed initial default DDR compiled 
*     configurations to reduce CDT churn throughout development lifecycle
*
* @param[in] bl_shared_data_type* shared data between sbl
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
void boot_config_data_table_default_init(bl_shared_data_type*);

/*===========================================================================

**  Function :  boot_update_config_data_table

** ==========================================================================
*/
/*!
* 
* @brief
*    copy the cdt from EEPROM/eMMC to memory, if any error happens, use the default
*    config_data_table array
* 
* @param[in] cdt_info* pointer to cdt info
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
void boot_update_config_data_table(struct cdt_info *cdt_info_ptr);

/*===========================================================================

**  Function :  boot_get_eepromII_info

** ==========================================================================
*/
/*!
* 
* @brief
*  Read the data from Secondary EEPROM and copy it to input buffer.
* 
* @param[in] uint8*  pointer to data buffer, to which info needs to be
*                    populated
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
#ifdef FEATURE_READ_SECONDARY_EEPROM
void boot_get_eepromII_info
(
  uint8* eepromII_data_buff_ptr, 
  uint32 eepromII_data_size
);
#else
static inline void boot_get_eepromII_info
(
  uint8* eepromII_data_buff_ptr, 
  uint32 eepromII_data_size
)
{ }
#endif /* FEATURE_READ_SECONDARY_EEPROM */

/*===========================================================================

**  Function :  boot_cdt_contains_ddr_cdb

** ==========================================================================
*/
/*!
* 
* @brief
*    Return boolean cdt_contains_ddr_cdb value, so SBL knows whether or not 
*    to call ddr_set_params() again to overwrite the default compiled CDT 
*    contents
*   
* @par Dependencies
*   None
*   
* @retval
*   boolean cdt_contains_ddr_cdb indicating if DDR CDB found in 
*             flashed CDT
* 
* @par Side Effects
*   None
*/
boolean boot_cdt_contains_ddr_cdb(void);

#endif /* BOOT_CONFIG_DATA_H */
