/** @file EFIHALIOMMUProtocol.h

  HAL IOMMU Protocol for UEFI

  Copyright (c) 2010-2011,2014 Qualcomm Technologies, Inc. All rights reserved.

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 06/19/17   amo     Created

=============================================================================*/

#ifndef __HAL_IOMMU_PROTOCOL_H__
#define __HAL_IOMMU_PROTOCOL_H__

#include "HALIOMMUTypes.h"

#define HAL_IOMMU_REVISION 0x0000000000000001

typedef 
void
(EFIAPI *EFI_HAL_IOMMU_PROTOCOL_INIT)(
   OUT   HAL_IOMMU_fptable_t            **p_fp_table
   );

typedef 
void
(EFIAPI *EFI_HAL_IOMMU_PROTOCOL_DEINIT)(
   VOID
   );

typedef struct _EFI_HAL_IOMMU_PROTOCOL {
   UINT64                            revision;
   EFI_HAL_IOMMU_PROTOCOL_INIT       _HAL_IOMMU_protocol_init;
   EFI_HAL_IOMMU_PROTOCOL_DEINIT     _HAL_IOMMU_protocol_deinit;
} 
EFI_HAL_IOMMU_PROTOCOL; 

extern EFI_GUID gEfiHalIommuProtocolGuid;

#endif /*__HAL_IOMMU_PROTOCOL_H__*/
