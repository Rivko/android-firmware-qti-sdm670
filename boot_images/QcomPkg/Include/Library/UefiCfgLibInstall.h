/** @file UefiCfgShLibInstall.h
   
  Interface to install Shared UEFI Config lib.

  Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved. 
   
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 01/16/12   vk      Initial Version

=============================================================================*/
#ifndef __UEFI_CFG_SH_LIB_INSTALL__H__
#define __UEFI_CFG_SH_LIB_INSTALL__H__

#include <Uefi.h>
#include <Library/UefiCfgLib.h>

EFI_STATUS
UefiCfgShLibInit (MemRegionInfo *MemRegions, UINTN MemRegCount, 
                  ConfigPair* CfgTblPtr, UINTN CfgCount);

#endif /* __UEFI_CFG_SH_LIB_INSTALL__H__ */

