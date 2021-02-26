#ifndef XBL_CONFIG_H
#define XBL_CONFIG_H

/*=============================================================================

                            XBLConfig Module Sources

GENERAL DESCRIPTION
  This file defines XBL config api's to support initializing XBLconfig module
  and access a config item.

Copyright 2016, 2017 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/
/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when         who     what, where, why
----------   ---     ----------------------------------------------------------
05/10/17     kpa      rename XBLCONFIG_SIZE_OVERFLOW_ERR to XBLCONFIG_OVERFLOW_ERR
03/10/17     kpa      Added error codes, update xblconfig_read parameters
03/03/17     kpa      Added error type and XCFG_LOADER_MAGIC_COOKIE_SIZE
02/14/17     kpa      Rename XBLCONFIG_ERR_NONE to XBLCONFIG_SUCCESS
01/20/17     kpa      Update magic cookie
10/14/16     kpa      Initial version
===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"



/*===========================================================================
                      DATA DECLARATIONS
===========================================================================*/

#define XCFG_LOADER_MAGIC_COOKIE_SIZE  4

/*  Define XBL config loader magic number ”CFGL” */
#define XCFG_LOADER_MAG0_INDEX    0      /* Header identification byte 0 index */
#define XCFG_LOADER_MAG0         'C'     /* Magic number byte 0              */

#define XCFG_LOADER_MAG1_INDEX    1      /* Header identification byte 1 index */
#define XCFG_LOADER_MAG1         'F'     /* Magic number byte 1              */

#define XCFG_LOADER_MAG2_INDEX    2     /* Header identification byte 2 index */
#define XCFG_LOADER_MAG2         'G'    /* Magic number byte 2              */

#define XCFG_LOADER_MAG3_INDEX    3     /* Header identification byte 3 index */
#define XCFG_LOADER_MAG3         'L'    /* Magic number byte 3              */


/*  Define XBL config Core magic number ”CFGU” */
#define XCFG_CORE_MAG0_INDEX    0      /* Header identification byte 0 index */
#define XCFG_CORE_MAG0         'C'     /* Magic number byte 0              */

#define XCFG_CORE_MAG1_INDEX    1      /* Header identification byte 1 index */
#define XCFG_CORE_MAG1         'F'     /* Magic number byte 1              */

#define XCFG_CORE_MAG2_INDEX    2     /* Header identification byte 2 index */
#define XCFG_CORE_MAG2         'G'    /* Magic number byte 2              */

#define XCFG_CORE_MAG3_INDEX    3     /* Header identification byte 3 index */
#define XCFG_CORE_MAG3         'U'    /* Magic number byte 3              */



#define XBL_CONFIG_MAJOR_VERSION          0X1
#define XBL_CONFIG_MINOR_VERSION          0X1

#define XBL_CONFIG_MAX_NUM_ENTRIES        0X9
#define XBL_CONFIG_MAX_HANDLES            0X5


typedef struct xcfg_metadata_header_type
{
  uint32 xcfg_type;
  uint8 major_version; 
  uint8 minor_version; 
  uint16 number_of_entries; 
  uint32 XBLconfig_meta_size;
} xcfg_metadata_header_type;


typedef struct xcfg_metadata_table_entries_type
{
  uint32 attributes;
  uint32 offset_from_meta_start; 
  uint32 size; 
  uint32 config_name_len; 
  char   config_name_start;
} __attribute__ ((packed)) xcfg_metadata_table_entries_type;


typedef struct xcfg_parsed_metadata_table_entry_type
{
  /* pointer to start of entry in blob */
  xcfg_metadata_table_entries_type *entry_ptr;

}xcfg_parsed_metadata_table_entry_type;

typedef struct xcfg_config_handle_type
{
  uintnt config_entry_index;
  boolean is_valid;
}xcfg_config_handle_type;

/*---------------------------------------------------------------------------
 ERRORS FROM XBL CONFIG
 ---------------------------------------------------------------------------*/
typedef enum
{
  XBLCONFIG_SUCCESS                            = 0X0000,
  XBLCONFIG_ITEM_NOT_FOUND                     = 0X0001,
  XBLCONFIG_INVALID_SIZE                       = 0X0002,
  XBLCONFIG_META_DATA_LOAD_ERR                 = 0X0003,  
  XBLCONFIG_CONFIG_DATA_LOAD_ERR               = 0X0004,  
  XBLCONFIG_INIT_ERR                           = 0X0005,
  XBLCONFIG_OPEN_ERR                           = 0X0006,
  XBLCONFIG_READ_ERR                           = 0X0007,
  XBLCONFIG_CLOSE_ERR                          = 0X0008,
  XBLCONFIG_MEMORY_ALLOCATION_ERR              = 0X0009,
  XBLCONFIG_HASH_SEG_ERR                       = 0X000A,
  XBLCONFIG_NULL_PTR_ERR                       = 0X000B,  
  XBLCONFIG_HANDLE_ERR                         = 0X000C,  
  XBLCONFIG_VERIFY_ERR                         = 0X000D,  
  XBLCONFIG_UNINITIALIZED                      = 0X000E,  
  XBLCONFIG_BUFFER_VALIDATION_ERR              = 0X000F,  
  XBLCONFIG_RAM_ELF_INFO_INIT_ERR              = 0X0010,
  XBLCONFIG_MEMORY_FREE_ERR                    = 0X0011,
  XBLCONFIG_DEINIT_ERR                         = 0X0012,  
  XBLCONFIG_OVERFLOW_ERR                       = 0X0013,  
  XBLCONFIG_ERR_MAX                            = 0x7FFF
} xblconfig_status_type;


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================
**  Function :  xblconfig_init
** ==========================================================================
*/
/*!
* 
* @brief
*   This function initializes xblconfig module
* 
* @param[in] 
*   None
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
xblconfig_status_type xblconfig_init(void);

/*===========================================================================
**  Function :  xblconfig_open
** ==========================================================================
*/
/*!
* 
* @brief
*   This function parses xbl config meta data to locate config item and
*   returns a config handle if found, along with its size
*
* @param[in]    config_name       Null terminated config item name string
*
* @param[out]   config_handle     Config item handle, if found
*               config_item_size  Total size of config item
*        
* @par Dependencies
*   XBLConfig module should have been initialized via xblconfig_init
*   before calling xblconfig_open
*   
* @retval
*   xblconfig_status_type   Status of operation
* 
* @par Side Effects
*   None
* 
*/
xblconfig_status_type xblconfig_open
(
  const char *config_name,
  uint32 *config_handle,
  uint32 *config_item_size
);

/*===========================================================================
**  Function :  xblconfig_read
** ==========================================================================
*/
/*!
* 
* @brief
*   This function copies config item data equal to input "size" from read offset
*   into provided buffer
* 
* @param[in]    config_handle   Handle to identify config item to be read
*               read_offset     Offset within config item to start read
*               size            Size of data to be read
*
* @param[out]   buffer          Data buffer provided by callee for reading data
*                    
* @par Dependencies
*   NOTE:   The user of xblconfig_read api has to allocate memory and
*           point it via buffer for loading data.
*   
* @retval
*   xblconfig_status_type   Status of operation
* 
* @par Side Effects
*   None
* 
*/
xblconfig_status_type xblconfig_read
( 
  const uint32 config_handle,
  uint32 read_offset,
  uint32 size,
  void *buffer
);

/*===========================================================================
**  Function :  xblconfig_close
** ==========================================================================
*/
/*!
* 
* @brief
*   This function free's config handle to be closed(sets to 0)
* 
* @param[in]  config_handle   Handle to be closed
*   None
*        
* @par Dependencies
*   NOTE:   The user of xblconfig_close api has to free memory 
*           used for reads associated with the handle to be closed.
*   
* @retval
*   xblconfig_status_type   Status of operation
* 
* @par Side Effects
*   None
* 
*/
xblconfig_status_type xblconfig_close (uint32 config_handle);

/*===========================================================================
**  Function :  xblconfig_deinit
** ==========================================================================
*/
/*!
* 
* @brief
*   This function deinitializes xblconfig module. Free's all internal allocated
*   buffers
* 
* @param[in] 
*   None
*        
* @par Dependencies
*   None
*   
* @retval
*   xblconfig_status_type   Status of operation
* 
* @par Side Effects
*   None
* 
*/
xblconfig_status_type xblconfig_deinit(void);

#endif /* XBL_CONFIG_H */

