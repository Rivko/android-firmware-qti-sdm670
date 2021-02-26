/** @file BlkIoSelectLib.h
   
  Library routines for selecting BlkIoHandle

  Copyright (c) 2017, Qualcomm Technologies, Inc. All rights reserved.
   
**/

/*=============================================================================
                              EDIT HISTORY
 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 04/28/17   vk      Initial revision
=============================================================================*/
#ifndef __BootSelectLIB_H__
#define __BootSelectLIB_H__

#include <Uefi.h>
#include <Library/QcomLib.h>

EFI_STATUS
GetBootDevBlkIOHandle (OPTIONAL EFI_GUID *PartitionType, OPTIONAL CHAR16* PartitionLabel, IN HandleInfo *BlkIoHandle);

#endif /* __BootSelectLIB_H__ */
