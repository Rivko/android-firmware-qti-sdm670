#ifndef BOOT_SBL_QSEE_INTERFACE_H
#define BOOT_SBL_QSEE_INTERFACE_H

/*===========================================================================

                          Boot SBL QSEE Interface

GENERAL DESCRIPTION
 Contains common defintion used by SBL and QSEE.

Copyright 2013-2015,2017 by Qualcomm Technologies Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/03/17   ds	   remove clobber prot
06/25/15   as      Removed boot_sbl_qsee_interface_get_appsbl_entry
06/25/15   as      Added boot_sbl_qsee_interface_get_image_entry
06/22/15   ck      Added boot_sbl_qsee_interface_get_appsbl_entry
05/25/15   kpa     Added "reset_required" to boot_sbl_qsee_interface
02/19/15   kpa     Added appsbl_entry_index to boot_sbl_qsee_interface
02/19/15   kpa     Update boot_sbl_qsee_interface
10/29/14   kpa     Support 64 bit memory map
11/20/14   kedara  Code cleanup,update to to include MIheader.h
10/29/14   ck      Updated to support 64 bit addresses
05/07/14   kedara  Include boot_target.h for BOOT_IMAGES_NUM_ENTRIES define
04/02/14   ck      Updated boot_sbl_qsee_interface to Bear 2.0 BAS spec
04/01/14   ck      Updated pbl_secboot_verified_info_type to secboot_verified_info_type
                   as Bear PBL now uses common secboot lib.
03/21/14   ck      Added boot_sbl_qsee_interface_get_eident and
                   boot_sbl_qsee_interface_get_entry_point functions
12/02/13   ck      Initial creation
============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include BOOT_PBL_H
#include "MIprogressive.h"
#include "MIheader.h"
#include "boot_target.h"
#include "boot_whitelist_prot.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/* SBL - QSEE Magic Number 1 */
#define SBL_QSEE_MAGIC_NUMBER_1  0xCE473D94

/* SBL - QSEE Magic Number 2 */
#define SBL_QSEE_MAGIC_NUMBER_2  0x3CB237BA

/* Third version */
#define SBL_QSEE_INTERFACE_VERSION 4

/*---------------------------------------------------------------------------
  Reset types support.
---------------------------------------------------------------------------*/
typedef enum
{
  NO_RESET = 0x0,
  RESET_DEVICE_BY_QSEE = 0x1
} reset_required_status_type;

/*---------------------------------------------------------------------------
  SBL Boot Images Entry Structure
  
  For each image loaded by SBL, there is a matching image entry in the
  boot_sbl_qsee_interface structure that is passed to QSEE at the end of 
  SBL execution.  The boot_images_entry structure is populated by the logic
  that loads the image.
---------------------------------------------------------------------------*/

typedef struct boot_images_entry
{
  secboot_sw_type image_id;
  uint32 e_ident;
  uint64 entry_point;
  secboot_verified_info_type image_verified_info;
  uint32 reserved_1;
  uint32 reserved_2;
  uint32 reserved_3;
  uint32 reserved_4;

} boot_images_entry;


/*---------------------------------------------------------------------------
  SBL - QSEE Interface Structure
  
  The following is needed by QSEE and is populated during SBL execution.
---------------------------------------------------------------------------*/

typedef struct boot_sbl_qsee_interface
{  
  uint32 magic_1;
  uint32 magic_2;
  uint32 version;
  uint32 number_images;
  uint32 reset_required;
  boot_images_entry boot_image_entry[BOOT_IMAGES_NUM_ENTRIES];
  uint64 ddr_enter_self_refresh;
  uint64 ddr_exit_self_refresh;
  uint32 appsbl_entry_index;
  uint32 reserved_2;
} boot_sbl_qsee_interface;


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


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
*   None
*
*/
int32
boot_sbl_qsee_interface_get_image_entry(boot_sbl_qsee_interface * sbl_qsee_interface,
                                        secboot_sw_type image_id);


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
                                   secboot_sw_type image_id);


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
                                        secboot_sw_type image_id);


#endif /* BOOT_SBL_QSEE_INTERFACE_H */
