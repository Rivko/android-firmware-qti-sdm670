/*=============================================================================

                     Boot Whitelist Protection

GENERAL DESCRIPTION
  This file contains the implementation for the boot-specific whitelist based
  protection module, to facilitate preventing the bootloader address spaces
  from being overwritten by a flash load.

Copyright 2017 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who          what, where, why
--------   ---          -------------------------------------------------------
01/25/17   digants      Initial revision.
=============================================================================*/

/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/

#include "boot_whitelist_prot.h"
#include "boot_util.h"

/*=============================================================================

LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

/* The current image whitelist that the addresses are checked against */
static whitelist_table curr_image_whitelst_tbl = {0};

/* The current header and meta whitelist that the addresses are checked against */
static whitelist_table hdr_meta_whitelst_tbl = {0};
/*=============================================================================

FUNCTION DEFINITIONS

=============================================================================*/


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
boolean boot_set_active_img_whitelist_table(whitelst_tbl_entry_type *whtlst_tbl_array_ptr)
{
  uint64 entry_num = 0;
  uint64 start_addr = 0, size = 0;

  if(whtlst_tbl_array_ptr == NULL)
  {
    return FALSE;
  }
  
  boot_invalidate_img_whitelist();

  // add all the entries to the current image whitelist from the array passed to us
  do
  {
    start_addr = (whtlst_tbl_array_ptr+entry_num)->start_addr;
    size = (whtlst_tbl_array_ptr+entry_num)->size;
    
    // break the loop once the size is 0 or max allowable entries is reached
    if((!size) || (entry_num >= MAX_WHITELIST_TABLE_ENTRIES))
    {
      break;
    }
    
    // check for address space wraparound
    if((start_addr + size) < start_addr)
    {
      boot_invalidate_img_whitelist();
      return FALSE;
    }
    
    curr_image_whitelst_tbl.entries[entry_num].start_addr = start_addr;
    curr_image_whitelst_tbl.entries[entry_num].size = size;
    entry_num+=1;
  }
  while(1);

  if(entry_num>0)
  {
    curr_image_whitelst_tbl.num_valid_entries = entry_num;
    return TRUE;
  }
  return FALSE;
}

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
boolean boot_set_entry_as_img_whitelist(uint64 start_addr, uint64 size)
{
  whitelst_tbl_entry_type new_whtlst[]={{start_addr,size},{0,0}};
  
  if((!size) || ((start_addr + size) < start_addr))
  {
    return FALSE;
  }
 
  return boot_set_active_img_whitelist_table(new_whtlst);  
}

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
void boot_invalidate_img_whitelist(void)
{
  qmemset((void *)(&curr_image_whitelst_tbl), 0x0, sizeof(curr_image_whitelst_tbl));
}

/*=========================================================================

**  Function :  boot_add_entry_to_active_img_whtlst

** ==========================================================================
*/
/*!
* 
* @brief
* Function to add entry to active image whitelist.
* 
* @param[in] start address of the range
* @param[in] size of the address range
*
* @par Dependencies
*   None
*   
* @retval
*   TRUE if the address range was successfully added to active image whitelist
* 
* @par Side Effects
*   None
* 
*/
boolean boot_add_entry_to_active_img_whtlst(uint64 start_addr, uint64 size)
{
  if(!size || (((uint64)start_addr + (uint64)size) < (uint64)(start_addr)) 
    || (curr_image_whitelst_tbl.num_valid_entries >= MAX_WHITELIST_TABLE_ENTRIES))
  {
    return FALSE;
  }
  
  curr_image_whitelst_tbl.entries[curr_image_whitelst_tbl.num_valid_entries].start_addr=start_addr;
  curr_image_whitelst_tbl.entries[curr_image_whitelst_tbl.num_valid_entries].size=size;
  curr_image_whitelst_tbl.num_valid_entries += 1;
  return TRUE;
}

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
boolean boot_set_active_hdr_meta_whitelist_table(whitelst_tbl_entry_type *whtlst_tbl_array_ptr)
{
  uint64 entry_num = 0;
  uint64 start_addr = 0, size = 0;

  if(whtlst_tbl_array_ptr == NULL)
  {
    return FALSE;
  }
  
  boot_invalidate_hdr_meta_whitelist();

  do
  {
    start_addr = (whtlst_tbl_array_ptr+entry_num)->start_addr;
    size = (whtlst_tbl_array_ptr+entry_num)->size;
    
    if(!start_addr || !size || (entry_num >= MAX_WHITELIST_TABLE_ENTRIES))
    {
      break;
    }
    
    if((start_addr + size) < start_addr)
    {
      boot_invalidate_hdr_meta_whitelist();
      return FALSE;
    }

    hdr_meta_whitelst_tbl.entries[entry_num].start_addr = start_addr;
    hdr_meta_whitelst_tbl.entries[entry_num].size = size;
    entry_num+=1;
  }
  while(1);

  if(entry_num>0)
  {
    hdr_meta_whitelst_tbl.num_valid_entries = entry_num;
    return TRUE;
  }
  return FALSE;
}

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
boolean boot_set_entry_as_hdr_meta_whitelist(uint64 start_addr, uint64 size)
{
  whitelst_tbl_entry_type new_whtlst[]={{start_addr,size},{0,0}};
  
  if(!start_addr || !size 
    || ((start_addr + size) < start_addr))
  {
    return FALSE;
  }
 
  return boot_set_active_hdr_meta_whitelist_table(new_whtlst);  
}

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
boolean boot_add_entry_to_hdr_meta_whtlst(uint64 start_addr, uint64 size)
{
  if(!start_addr || !size 
    || (((uint64)start_addr + (uint64)size) < (uint64)(start_addr)) 
    || (hdr_meta_whitelst_tbl.num_valid_entries >= MAX_WHITELIST_TABLE_ENTRIES))
  {
    return FALSE;
  }
  
  hdr_meta_whitelst_tbl.entries[hdr_meta_whitelst_tbl.num_valid_entries].start_addr=start_addr;
  hdr_meta_whitelst_tbl.entries[hdr_meta_whitelst_tbl.num_valid_entries].size=size;
  hdr_meta_whitelst_tbl.num_valid_entries += 1;
  return TRUE;
}

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
void boot_invalidate_hdr_meta_whitelist(void)
{
  qmemset((void *)(&hdr_meta_whitelst_tbl), 0x0, sizeof(hdr_meta_whitelst_tbl));
}

/*=========================================================================

**  Function :  check_range_against_whtlst

** ==========================================================================
*/
/*!
* 
* @brief
* Function to check address range against the current hdr meta whitelist.
* 
* @param[in] start address of the range
* @param[in] size of the address range
* @param[in] pointer to the whitelist to check the range against
*
* @par Dependencies
*   None
*   
* @retval
*   TRUE if the bound check against whitelist passes
* 
* @par Side Effects
*   None
* 
*/
static boolean check_range_against_whtlst(uint64 start_addr,uint64 size,whitelist_table *whitelist)
{
  uint64 entry_num=0;
  whitelst_tbl_entry_type whtlst_entry;

  if(!size || ((start_addr + size) < start_addr))
  {
    return FALSE;
  }

  if(whitelist->num_valid_entries > 0)
  {
    for(entry_num=0;entry_num<(whitelist->num_valid_entries);entry_num++)
    {
      whtlst_entry = whitelist->entries[entry_num];
      if((whtlst_entry.start_addr <= start_addr) 
        && (start_addr <= (whtlst_entry.start_addr+whtlst_entry.size)) 
        && ((start_addr+size) <= (whtlst_entry.start_addr+whtlst_entry.size)))
      {
        return TRUE;
      }
    }
  }
  return FALSE;
}

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
boolean boot_whitelist_address_range_check(uint64 start_addr,uint64 size,buffer_type buf_typ)
{
  if(!size || ((start_addr + size) < start_addr))
  {
    return FALSE;
  }
  
  switch(buf_typ)
  {
  case IMAGE_BODY_TYPE:
    return check_range_against_whtlst(start_addr,size,(whitelist_table *)(&curr_image_whitelst_tbl));
    break;
  case HDR_META_INFO_TYPE:
    return check_range_against_whtlst(start_addr,size,(whitelist_table *)(&hdr_meta_whitelst_tbl));
    break;
  default:
    return FALSE;
    break;
  }
}