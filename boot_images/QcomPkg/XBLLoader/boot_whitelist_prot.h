#ifndef BOOT_WHITELST_PROTECTION_H
#define BOOT_WHITELST_PROTECTION_H

/*=============================================================================

                     Boot Whitelist Protection

GENERAL DESCRIPTION
  This file contains definition for whitelist protection module and declaration of
  public functions.

Copyright 2017 by QUALCOMM Technologies Inc.  All Rights Reserved.
=============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/27/17   ds      Initial revision
============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS
                      
===========================================================================*/

//maximum number of entries allowed in any whitelist table
#ifndef MAX_WHITELIST_TABLE_ENTRIES
#define MAX_WHITELIST_TABLE_ENTRIES 20
#endif

// struct for holding address range of an whitelist entry
typedef struct whitelst_tbl_entry 
{ 
  uint64 start_addr;
  uint64 size;
} whitelst_tbl_entry_type;

typedef whitelst_tbl_entry_type whitelist_table_array[MAX_WHITELIST_TABLE_ENTRIES];

// whitelist table with number of valid entries field
typedef struct whitelist_tbl_obj
{
  uint64 num_valid_entries;
  whitelist_table_array entries;
}whitelist_table;

//buffer type for choosing the correct whitelist
typedef enum buff_typ
{
  IMAGE_BODY_TYPE = 1,
  HDR_META_INFO_TYPE = 2
}buffer_type;


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*=========================================================================

**  Function :  boot_set_active_img_whitelist_table

** ==========================================================================
*/
/*!
* 
* @brief
*   Function to set the active image whitelist.
* 
* @param[in] pointer to a whitelist table array
*
* @par Dependencies
*   None
*   
* @retval
*   TRUE if the whitelist table was successfully set as active image whitelist
* 
* @par Side Effects
*   None
* 
*/
boolean boot_set_active_img_whitelist_table(whitelst_tbl_entry_type *whtlst_tbl_array_ptr);

/*=========================================================================

**  Function :  boot_set_entry_as_img_whitelist

** ==========================================================================
*/
/*!
* 
* @brief
* Function to set single address range as whitelist.
* 
* @param[in] start address of the range
* @param[in] size of the address range
*
* @par Dependencies
*   None
*   
* @retval
*   TRUE if the address range was successfully set as active image whitelist
* 
* @par Side Effects
*   None
* 
*/
boolean boot_set_entry_as_img_whitelist(uint64 start_addr, uint64 size);

/*=========================================================================

**  Function :  boot_invalidate_img_whitelist

** ==========================================================================
*/
/*!
* 
* @brief
* Function to invalidate current image whitelist.
* 
* @param[in] none
*
* @par Dependencies
*   None
*   
* @retval
*   none
* 
* @par Side Effects
*   None
* 
*/
void boot_invalidate_img_whitelist(void);

/*=========================================================================

**  Function :  boot_set_active_hdr_meta_whitelist_table

** ==========================================================================
*/
/*!
* 
* @brief
*   Function to set the active header meta whitelist.
* 
* @param[in] pointer to a whitelist table array
*
* @par Dependencies
*   None
*   
* @retval
*   TRUE if the whitelist table was successfully set as active hdr meta whitelist
* 
* @par Side Effects
*   None
* 
*/
boolean boot_set_active_hdr_meta_whitelist_table(whitelst_tbl_entry_type *whtlst_tbl_array_ptr);

/*=========================================================================

**  Function :  boot_set_entry_as_hdr_meta_whitelist

** ==========================================================================
*/
/*!
* 
* @brief
* Function to set single address range as whitelist.
* 
* @param[in] start address of the range
* @param[in] size of the address range
*
* @par Dependencies
*   None
*   
* @retval
*   TRUE if the address range was successfully set as active hdr meta whitelist
* 
* @par Side Effects
*   None
* 
*/
boolean boot_set_entry_as_hdr_meta_whitelist(uint64 start_addr, uint64 size);

/*=========================================================================

**  Function :  boot_invalidate_hdr_meta_whitelist

** ==========================================================================
*/
/*!
* 
* @brief
* Function to invalidate current hdr meta whitelist.
* 
* @param[in] none
*
* @par Dependencies
*   None
*   
* @retval
*   none
* 
* @par Side Effects
*   None
* 
*/
void boot_invalidate_hdr_meta_whitelist(void);

/*=========================================================================

**  Function :  boot_add_entry_to_hdr_meta_whtlst

** ==========================================================================
*/
/*!
* 
* @brief
* Function to add entry to active hdr meta whitelist.
* 
* @param[in] start address of the range
* @param[in] size of the address range
*
* @par Dependencies
*   None
*   
* @retval
*   TRUE if the address range was successfully added to active hdr meta whitelist
* 
* @par Side Effects
*   None
* 
*/
boolean boot_add_entry_to_hdr_meta_whtlst(uint64 start_addr, uint64 size);

/*=========================================================================

**  Function :  boot_whitelist_address_range_check

** ==========================================================================
*/
/*!
* 
* @brief
* Function to check address range against the current hdr meta whitelist.
* 
* @param[in] start address of the range
* @param[in] size of the address range
* @param[in] buffer type where data is intended to be loaded
*
* @par Dependencies
*   None
*   
* @retval
*   TRUE if the hdr meta whitelist bound check passes
* 
* @par Side Effects
*   None
* 
*/
boolean boot_whitelist_address_range_check(uint64 start_addr,uint64 size,buffer_type buf_typ);
#endif /* BOOT_WHITELST_PROTECTION_H */