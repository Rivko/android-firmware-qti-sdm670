#ifndef BOOT_HOB_H
#define BOOT_HOB_H

/*===========================================================================

                          Boot HOB (Hand Off Block) Interface

GENERAL DESCRIPTION
 Contains common defintions for boot HOB generation.

Copyright 2014-2015 by Qualcomm Technologies Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/28/15   as      Added boot_hob_add_active_ddr_channels
07/03/15   as      Added boot_hob_add_platform_info
06/10/15   as      Updated boot_hob_add_ddr_spd
05/06/15   as      Added boot_hob_add_bert_record
04/16/15   ck      Modified boot_hob_add_sbl_qsee_interface argument list
03/13/15   ck      Added boot_hob_add_sbl_qsee_interface
03/05/15   ck      Added boot_hob_add_acpi
10/27/14   ck      Initial creation
============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"
#include "boot_sbl_qsee_interface.h"
#include "DDRTargetLib.h"
#include "boot_bert.h"
#include "DDIPlatformInfo.h"


/*===========================================================================
                      PUBLIC DATA DECLARATIONS
===========================================================================*/
#define BOOT_HOB_FULL_CONFIG_BOOT_MODE      0x00
#define BOOT_HOB_RECOVERY_BOOT_MODE         0x20

/*===========================================================================
                      PUBLIC FUNCTION DECLARATIONS
===========================================================================*/

/*===========================================================================
**  Function :  boot_hob_create_list
** ==========================================================================
*/
/*!
* 
* @brief  
*   This function generates a new hob list at the location specified.  The
*   first entry in a hob list is an EFI_HOB_TYPE_HANDOFF.  Minimal information
*   needed is DDR base address and length.
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
void boot_hob_create_list(uintnt hob_list_region_base_address,
                          uintnt hob_list_region_size,
                          boot_boolean recovery_mode);


/*===========================================================================
**  Function :  boot_hob_add_cpu_partial_goods
** ==========================================================================
*/
/*!
* 
* @brief  
*   This function adds a new cpu partial goods hob entry to an existing list.
*   The cpu partial goods value is the untouched 64 bit cpu partial goods
*   fuse row.
*
* @par Dependencies
*   boot_hob_create_list must be called first.
*
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void boot_hob_add_cpu_partial_goods(uint64 cpu_partial_goods,
                                    uintnt hob_list_region_base_address,
                                    uintnt hob_list_region_size);


/*===========================================================================
**  Function :  boot_hob_add_l3_partial_goods
** ==========================================================================
*/
/*!
* 
* @brief  
*   This function adds a new l3 partial goods hob entry to an existing list.
*   The l3 partial goods data consists of a single bit per l3 block
*   indicating if that  block is good or bad.  A "1" indicates that the l3
*   block is good.  A "0" indicates it is bad.  A bad l3 block means that
*   both interleave fuses for the corrosponding block are blown.  If either
*   fuse is not blown then it is a good l3 block.  Block order starts from LSB.
*
* @par Dependencies
*   boot_hob_create_list must be called first.
*
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void boot_hob_add_l3_partial_goods(uint64 l3_partial_goods,
                                   uintnt hob_list_region_base_address,
                                   uintnt hob_list_region_size);


/*===========================================================================
**  Function :  boot_hob_add_active_ddr_channels
** ==========================================================================
*/
/*!
*
* @brief
*   This function adds a new DDR DIMM map hob entry to an existing list.
*   Data consists of a mapping to know which memory controller is in use
*   and which one is lying empty.
*
* @par Dependencies
*   boot_hob_create_list must be called first.
*
* @retval
*   None
*
* @par Side Effects
*   None
*
*/
void boot_hob_add_active_ddr_channels(ddr_dimm_pop_map * ddr_dimm_map,
                                      size_t ddr_dimm_map_size,
                                      uintnt hob_list_region_base_address,
                                      uintnt hob_list_region_size);


/*===========================================================================
**  Function :  boot_hob_add_ddr_spd
** ==========================================================================
*/
/*!
* 
* @brief  
*   This function adds a new DDR SPD hob entry to an existing list.
*   Data consists of a 64 bit header indicating the controller number,
*   slot number, and if data is present.  Following is the SPD info.
*   Size of the SPD block varies depending on DDR type.
*
* @par Dependencies
*   boot_hob_create_list must be called first.
*
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void boot_hob_add_ddr_spd(spd_info_per_dimm * ddr_spd_smbios,
                          size_t ddr_spd_smbios_size,
                          uintnt hob_list_region_base_address,
                          uintnt hob_list_region_size);


/*===========================================================================
**  Function :  boot_hob_add_ddr_single_instance
** ==========================================================================
*/
/*!
* 
* @brief  
*   This function takes a filled out DDR single instance structure from
*   the DDR driver and copies it into a HOB for use by IMC and other images.
*
* @par Dependencies
*   boot_hob_create_list must be called first.
*
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void boot_hob_add_ddr_single_instance(ddr_shared_params * ddr_single_instance,
                                      uintnt hob_list_region_base_address,
                                      uintnt hob_list_region_size);


/*===========================================================================
**  Function :  boot_hob_add_bert_record
** ==========================================================================
*/
/*!
*
* @brief
*   This function takes the base address for BERT record and copies it
*   into a HOB for use by IMC and other images.
*
* @par Dependencies
*   boot_hob_create_list must be called first.
*
* @retval
*   None
*
* @par Side Effects
*   None
*
*/
void boot_hob_add_bert_record(bert_record_type * bert_record,
                              uintnt hob_list_region_base_address,
                              uintnt hob_list_region_size);


/*===========================================================================
**  Function :  boot_hob_add_platform_info
** ==========================================================================
*/
/*!
*
* @brief
*   This function takes the base address for platform info and copies it
*   into a HOB for use by IMC and other images.
*
* @par Dependencies
*   boot_hob_create_list must be called first.
*
* @retval
*   None
*
* @par Side Effects
*   None
*
*/
void boot_hob_add_platform_info(DalPlatformInfoPlatformInfoType * platform_info,
                                uintnt hob_list_region_base_address,
                                uintnt hob_list_region_size);


/*===========================================================================
**  Function :  boot_hob_add_acpi
** ==========================================================================
*/
/*!
* 
* @brief  
*   This function adds the ACPI base address to a hob entry.
*
* @par Dependencies
*   boot_hob_create_list must be called first.
*
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void boot_hob_add_acpi(uintnt acpi_base_address,
                       uintnt hob_list_region_base_address,
                       uintnt hob_list_region_size);


/*===========================================================================
**  Function :  boot_hob_add_sbl_qsee_interface
** ==========================================================================
*/
/*!
* 
* @brief  
*   This function adds the SBL - QSEE interface base address to a hob entry.
*
* @par Dependencies
*   boot_hob_create_list must be called first.
*
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void
boot_hob_add_sbl_qsee_interface(boot_sbl_qsee_interface * sbl_qsee_interface,
                                uintnt hob_list_region_base_address,
                                uintnt hob_list_region_size);


#endif /* BOOT_HOB_H */
