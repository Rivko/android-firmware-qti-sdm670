/*===========================================================================

                         Boot SBL QSEE Interface File

GENERAL DESCRIPTION
  This header file contains declarations and definitions for sbl-qsee
  interface utilities.

Copyright 2014-2015 by Qualcomm Technologies Incorporated.  All Rights Reserved.
============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when       who         what, where, why
--------   ---         --------------------------------------------------
03/03/17   ds		   remove clobber prot
06/25/15   as          Added boot_sbl_qsee_interface_get_loader_entry
06/25/15   as          Added boot_sbl_qsee_interface_get_image_entry
06/22/15   ck          Added boot_sbl_qsee_interface_get_appsbl_entry
10/30/14   kpa         change uint64 datatype to uintnt
10/29/14   ck          Updated to support 64 bit addresses
03/21/14   ck          Initial version  

===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "boot_sbl_qsee_interface.h"


/*=========================================================================
                            
                       FUNCTION DEFINITIONS

=========================================================================*/


/*===========================================================================

**  Function :  boot_sbl_qsee_interface_get_loader_entry

** ==========================================================================
*/
/*!
*
* @brief
*   Utility function that gets the appsbl entry number.
*
* @par Dependencies
*   None
*
* @retval
*   int32  Indicates the entry position of the appsbl image.  -1 if not found.
*
* @par Side Effects
*   None
*
*/
int32
boot_sbl_qsee_interface_get_loader_entry(boot_sbl_qsee_interface * sbl_qsee_interface)
{
  uint32 image_count = 0;
  uint32 whitelist_entry_count = 0;
  whitelst_tbl_entry_type whitelist_table[] = {XBL_LOADER_WHITELIST_REGION_TABLE};


  /* To determine which entry is loader use the LOADER whitelist. */
  for (image_count = 0;
       image_count < BOOT_IMAGES_NUM_ENTRIES;
       image_count++)
  {
    if (sbl_qsee_interface->boot_image_entry[image_count].image_id == SECBOOT_SBL_SW_TYPE)
    {
      /* Iterate through whitelist regions checking against entry point. */
      whitelist_entry_count = 0;
      while (whitelist_table[whitelist_entry_count].size != 0)
      {
        if ((sbl_qsee_interface->boot_image_entry[image_count].entry_point >=
             whitelist_table[whitelist_entry_count].start_addr) &&
            (sbl_qsee_interface->boot_image_entry[image_count].entry_point <
             (whitelist_table[whitelist_entry_count].start_addr + whitelist_table[whitelist_entry_count].size)))
        {
          return image_count;
        }

        whitelist_entry_count++;
      }
    }
  }


  /* APPSBL was not found so return -1 */
  return -1;
}


/*===========================================================================

**  Function :  boot_sbl_qsee_interface_get_appsbl_entry

** ==========================================================================
*/
/*!
* 
* @brief  
*   Utility function that gets the appsbl entry number.
*
* @par Dependencies
*   None
*
* @retval
*   int32  Indicates the entry position of the appsbl image.  -1 if not found.
* 
* @par Side Effects
*   None
*  
*/
int32
boot_sbl_qsee_interface_get_appsbl_entry(boot_sbl_qsee_interface * sbl_qsee_interface)
{
  uint32 image_count = 0;
  uint32 whitelist_entry_count = 0;
  whitelst_tbl_entry_type whitelist_table[] = {XBL_CORE_WHITELIST_REGION_TABLE};


  /* First check table for APPSBL ID. */
  for (image_count = 0;
       image_count < BOOT_IMAGES_NUM_ENTRIES;
       image_count++)
  {
    if (sbl_qsee_interface->boot_image_entry[image_count].image_id ==
        SECBOOT_APPSBL_SW_TYPE)
    {
      return image_count;
    }
  }


  /* If APPSBL wasn't found then it is part of XBL's multiple ELF. */
  /* To determine which entry is APPSBL use the CORE whitelist. */
  for (image_count = 0;
       image_count < BOOT_IMAGES_NUM_ENTRIES;
       image_count++)
  {
    if (sbl_qsee_interface->boot_image_entry[image_count].image_id == SECBOOT_SBL_SW_TYPE)
    {
      /* Iterate through whitelist regions checking against entry point. */
      whitelist_entry_count = 0;
      while (whitelist_table[whitelist_entry_count].size != 0)
      {
        if ((sbl_qsee_interface->boot_image_entry[image_count].entry_point >= 
             whitelist_table[whitelist_entry_count].start_addr) &&
            (sbl_qsee_interface->boot_image_entry[image_count].entry_point <
             (whitelist_table[whitelist_entry_count].start_addr + whitelist_table[whitelist_entry_count].size)))
        {
          return image_count;
        }

        whitelist_entry_count++;
      }
    }
  }

  
  /* APPSBL was not found so return -1 */
  return -1;
}


/*===========================================================================

**  Function :  boot_sbl_qsee_interface_get_image_entry

** ==========================================================================
*/
/*!
*
* @brief
*   Utility function that gets the image entry number.
*
* @par Dependencies
*   None
*
* @retval
*   int32  Indicates the entry position of the image.  -1 if not found.
*
* @par Side Effects
*   Do not use this function for finding APPSBL.
*
*/
int32
boot_sbl_qsee_interface_get_image_entry(boot_sbl_qsee_interface * sbl_qsee_interface,
                                        secboot_sw_type image_id)
{
  uint32 image_count = 0;

  if(image_id == SECBOOT_SBL_SW_TYPE)
  {
    return boot_sbl_qsee_interface_get_loader_entry(sbl_qsee_interface);
  }
  if(image_id == SECBOOT_APPSBL_SW_TYPE)
  {
    return boot_sbl_qsee_interface_get_appsbl_entry(sbl_qsee_interface);
  }

  /* First check table for APPSBL ID. */
  for (image_count = 0;
       image_count < BOOT_IMAGES_NUM_ENTRIES;
       image_count++)
  {
    if (sbl_qsee_interface->boot_image_entry[image_count].image_id == image_id)
    {
      return image_count;
    }
  }

  /* image was not found so return -1 */
  return -1;
}


/*===========================================================================

**  Function :  boot_sbl_qsee_interface_get_eident

** ==========================================================================
*/
/*!
* 
* @brief  
*   Utility function that retrieves the eident from the sbl qsee interface
*   for the given image type.
*
* @par Dependencies
*   None
*
* @retval
*   uint32 ELF ident of image type passed if found in sbl qsee interface.
           Returns 0 if not found.
* 
* @par Side Effects
*   None
* 
*/
uint32
boot_sbl_qsee_interface_get_eident(boot_sbl_qsee_interface * sbl_qsee_interface,
                                   secboot_sw_type image_id)
{
  uint32 image_count = 0;
  uint32 eident_value = 0;


  /* Search image entries in QSEE interface for image_id and get elf type. */
  for (image_count = 0;
       image_count < BOOT_IMAGES_NUM_ENTRIES;
       image_count++)
  {
    if (sbl_qsee_interface->boot_image_entry[image_count].image_id == image_id)
    {
      eident_value = sbl_qsee_interface->boot_image_entry[image_count].e_ident;
      break;
    }
  } 

  return eident_value;
}


/*===========================================================================

**  Function :  boot_sbl_qsee_interface_get_entry_point

** ==========================================================================
*/
/*!
* 
* @brief  
*   Utility function that retrieves the entry point from the sbl qsee interface
*   for the given image type.
*
* @par Dependencies
*   None
*
* @retval
*   uintnt Entry point of image type passed if found in sbl qsee interface.
           Returns 0 if not found.
* 
* @par Side Effects
*   None
* 
*/
uintnt
boot_sbl_qsee_interface_get_entry_point(boot_sbl_qsee_interface * sbl_qsee_interface,
                                        secboot_sw_type image_id)
{
  uint32 image_count = 0;
  uintnt entry_point = 0;


  /* Search image entries in QSEE interface for image_id and get entry_point. */
  for (image_count = 0;
       image_count < BOOT_IMAGES_NUM_ENTRIES;
       image_count++)
  {
    if (sbl_qsee_interface->boot_image_entry[image_count].image_id == image_id)
    {
      entry_point =
        sbl_qsee_interface->boot_image_entry[image_count].entry_point;

      break;
    }
  } 

  return entry_point;
}
