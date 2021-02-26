/** @file UefiPlatCfg.h
  Pareser for UEFI platform configuration data.

  Copyright (c) 2012-2013, 2016 Qualcomm Technologies, Inc.  All
  Rights Reserved. Qualcomm Technologies Proprietary and
  Confidential.

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 02/18/16   bh      Update memory map parsing and non-FD bank parsing
 02/01/13   vk      Common Sec, text based cfg file params
 01/17/13   vk      Add UefiCfgLib
 11/21/12   niting  Added uefiplat.cfg for memory map configuration
 11/19/12   niting  Initial revision.
 
=============================================================================*/

#ifndef _UEFI_PLAT_CFG_H_
#define _UEFI_PLAT_CFG_H_

#include <Uefi.h>
#include <Pi/PiHob.h>
#include <Library/ArmLib.h>
#include <Library/UefiCfgLib.h>


/* Parses UEFI platform configuration data (i.e. UEFI memory map) and
   stores/installs the necessary items to be accessed by other UEFI modules
*/
EFI_STATUS
EFIAPI
LoadAndParsePlatformCfg ( VOID );

/* Adds all banks from ram partition table, as well as remainder of bank
   containing FD region to the memory map table */
EFI_STATUS
EFIAPI
UpdateSystemMemoryRegions( VOID );

/* Initializes the cache with all the memory regions */
EFI_STATUS
EFIAPI
InitCacheWithMemoryRegions (VOID);

/* Gets the Memory Map that was parsed from the platform cfg file */
EFI_STATUS
EFIAPI
GetMemRegionCfgInfo ( MemRegionInfo** MemoryRegions, UINTN* NumMemoryRegions );

/* Gets the configuration parameters parsed from config file */
EFI_STATUS EFIAPI
GetConfigTable (ConfigPair** ConfigTblPtr, UINTN* ConfigCnt);
#endif //_UEFI_PLAT_CFG_H_
