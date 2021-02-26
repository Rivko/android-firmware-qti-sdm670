#ifndef SMMUTYPES_H
#define SMMUTYPES_H
/*=============================================================================

                       S M M U  D A L  D R I V E R 

FILE:      SMMUTypes.h

GENERAL DESCRIPTION
  This file contains data structures needed for driver configuration

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
 
Copyright (c) 2010 - 2015
Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/
/*=============================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tz/2.0.2/kernel/smmu/ACv3.0/common/inc/SMMUTypes.h#1 $
$DateTime: 2018/02/06 02:57:17 $
$Author: pwbldsvc $
$Change: 15399902 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
08/20/14   sc      Cleaned up
12/14/10   kcr     Initial Version
==========================================================================*/
#include "com_dtypes.h"

typedef enum
{
  SMMU_ERR_OK = 0,
  SMMU_ERR_HAL_SUCCESS = 0,
  SMMU_ERR_GENERIC_ERROR,
  SMMU_ERR_INVALID_PARAM,
  SMMU_ERR_NULL_POINTER,
  SMMU_ERR_OUT_OF_RANGE,
  SMMU_ERR_VMID_NOT_FOUND,
  SMMU_ERR_VMID_NOT_INIT,
  SMMU_ERR_INIT_PT_FAILED,
  SMMU_ERR_REGISTER_MMU_FAILED,
  SMMU_ERR_GET_PT_ATTR_FAILED,
  SMMU_ERR_INVALID_CACHE_ATTR,
  SMMU_ERR_MAP_FAILED,
  SMMU_ERR_UNMAP_FAILED,
  SMMU_ERR_INIT_CLOCK_FAILED,
  SMMU_ERR_ENABLE_CLOCK_FAILED,
  SMMU_ERR_DISABLE_CLOCK_FAILED,
  SMMU_ERR_PAGETABLE_NOT_INIT,
  SMMU_ERR_MALLOC_FAILED,
  SMMU_ERR_GET_DAL_PROP_HANDLE_FAILED,
  SMMU_ERR_UNSUPPORTED_INSTANCE_FOR_TARGET,
  SMMU_ERR_RESULT_SIZE = 0x7FFFFFFF, // force to 32-bit enum
} SMMUResult_t;

typedef uint32 SMMUAddr_t;

#define SMMU_INVALID_ADDR 0xFFFFFFFF
#endif /*SMMUTYPES_H*/
