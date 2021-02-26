/*
 * @file DDRInfoPartitionLib.c
 *
 * @brief DDRInfoPartitionLib functions
 *
 * Copyright (c) 2017 by Qualcomm Technologies, Inc. 
 * All Rights Reserved.
 *
 */
/*=======================================================================
                        Edit History

when       who     what, where, why
--------   ----    --------------------------------------------------- 
04/27/17   ddk     Initital Version
========================================================================*/
#include <Uefi.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DDRInfoPartitionLib.h>
#include <com_dtypes.h>
#include <Library/smem.h> 
#include <MemRegionInfo.h>
#include "boot_util.h"
#include "ddr_drivers.h"



EFI_STATUS
GetDDRDetails (IN OUT struct ddr_details_entry_info *ddr_detail)
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINT32 DDRInfoBuffSz = 0;
  VOID       *pDDRTable = NULL;

  if (ddr_detail == NULL)
    return EFI_INVALID_PARAMETER;

  /* Get the DDR details table */
  pDDRTable = smem_get_addr(DDRINFO_UEFI_SMEM_ID, (uint32*)&DDRInfoBuffSz);
  if (pDDRTable == NULL)
  {
    /*NOTE: We should be here only if SMEM is not initialized (virtio, SmemNullLib)*/
    DEBUG ((EFI_D_WARN, "WARNING: Unable to read DDR Info table from SMEM\n"));
    return EFI_NOT_READY;
  }


  qmemscpy((void *)ddr_detail, sizeof(ddr_details_entry), pDDRTable, sizeof(ddr_details_entry));   

  return Status;
}


