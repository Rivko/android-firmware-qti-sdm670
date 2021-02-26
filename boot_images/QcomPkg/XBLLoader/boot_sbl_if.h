#ifndef BOOT_SBL_IF_H
#define BOOT_SBL_IF_H

/*===========================================================================

                        Boot Module Interface Header File

GENERAL DESCRIPTION
  This header file contains the definition of the public interface to
  the SBL.

Copyright 2007, 2009-2011, 2013-2015 by Qualcomm Technologies Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/05/15   ck      Corrected copyright
08/07/15   kpa     added support for passing highest bank bit information to ram
                   partition table
04/08/14   ck      Added mmu_pagetable_size to boot_sbl_if_shared_info_type
12/26/13   ck      Removed boot_rpm_apps_shared_data_type, not needed in Bear family
06/12/13   dh      Update sbl_if_shared_ddr_info_type for LPAE support
03/05/13   dh      Added mmu_l1_pagetable_base to point to base of page table
02/29/11   dh      Added rpm_apps_shared_data, removed rpm_jump_trigger and 
					rpm_entry_addr_ptr
09/01/11   kpa     Added pointer next_avail_tpm_hash_blk_ptr.
09/01/11   kpa     Added  failsafe and non-failsafe version numbers. 
05/23/11   plc     Added sbl_if_shared_extended_ddr_info_type type for 
                   describing number of partitions for each DDR device
02/23/11   dh      Added SHARED_IMEM_BOOT_MAGIC_NUM  
11/03/10   dh      Added pointer to sdcc device information
10/18/10   dh      Added pointer to configuration data table information
08/03/10   dh      Added structures to share ddr information
06/23/10   plc     Improve SBL1 shared data structure to have pointer to 
                   location storing entry addr for RPM FW image
05/26/10   plc     Initial port to secboot 3.0 common code  
03/03/10   plc     Modification for initial sbl1 image
11/20/09   msm     Added generic pointer for target info passing in 
                   boot_dbl_if_shared_info_type.
11/18/09   as      Boot Optimization Changes
06/31/07   anb     Initial revision.

============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#ifndef _ARM_ASM_
#include "boot_comdef.h"
#include "boot_sbl_shared.h"
#endif /* _ARM_ASM_ */

/*===========================================================================

                      PUBLIC DATA DECLARATIONS
                      
===========================================================================*/
#ifndef _ARM_ASM_

/* 
 * Following magic number indicates the boot shared imem region is initialized
 * and the content is valid
 */
#define SHARED_IMEM_BOOT_MAGIC_NUM        0xC1F8DB40


/*---------------------------------------------------------------------------
  Structures to store the shared DDR information
---------------------------------------------------------------------------*/
typedef struct
{
  /* Chip Select Starting Address */
  uint64       cs_addr;
  /* DDR Size in Mega Bytes */  
  uint64       ramsize;    
}sbl_if_shared_ddr_info_type;

typedef struct
{
  /* Number of Partitions */
  uint32       num_partitions;

  /* highest address bit for bank address */
  uint32       highest_bank_bit;
  
  /* Chip Select 1st Partition Starting Address */
  uint64       sdram_addr;
}sbl_if_shared_extended_ddr_info_type;

typedef struct
{
  /* Number of total attached DDR */
  uint32       noofddr;    
  /* DDR's Information */
  sbl_if_shared_ddr_info_type  *ddr_info; 
  /* DDR's Extended Information */
  sbl_if_shared_extended_ddr_info_type *ddr_partition_info;
}sbl_if_shared_ddr_device_info_type;


/*---------------------------------------------------------------------------
  Defines the data structure that is filled with data that is shared with
  subsequent boot image. It may be necessary for subsequent boot images to make
  a deep copy of this structure for its own use.
---------------------------------------------------------------------------*/
typedef struct boot_sbl_if_shared_info_type
{ 
  /* Pointer to data structure passed by PBL                 */
  boot_pbl_shared_data_type              *pbl_shared_data;

  /* Pointer to system partition table                       */
  void                                   *sys_partition_pointer;    
  
  /* Pointer to DDR info */
  sbl_if_shared_ddr_device_info_type     *ddr_shared_info;

  /* Pointer to the boot logger's meta info structure*/
  void                                   *boot_logger_meta_info;

  /*pointer to the configuration data table info structure*/
  void                                   *config_data_table_info;
  
  /*pointer to sdcc device information*/
  void                                   *sdcc_boot_device_info;
  
  /* Rollback prevention version information */
  uint32 failsafe_image_max_version;
  uint32 non_failsafe_image_max_version;

  /* Pointer pointing to next available location to store image hash block. */
  uint32 *next_avail_tpm_hash_blk_ptr;
   
  /* Pointer to the base of L1 page table */
  uint32 *mmu_l1_pagetable_base;

  /* Total size of page tables */
  uint32 mmu_pagetable_size;

}boot_sbl_if_shared_info_type;


#endif /* _ARM_ASM_ */

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


#endif /* BOOT_SBL_IF_H */

