#ifndef __AC_OEM_CONFIG_H__
#define __AC_OEM_CONFIG_H__
/*==============================================================================
  @file ACOEMConfig.h
  Public interface for OEM spare region configurations.

  Copyright (c) 2018 Qualcomm Technologies Incorporated.
  All rights reserved.
  Qualcomm Technologies Inc. Confidential and Proprietary
  ==============================================================================*/

/*==============================================================================

  EDIT HISTORY FOR FILE

  $Header: //components/rel/ssg.tz/1.0.2/securemsm/accesscontrol/api/ACOEMConfig.h#1 $
  $DateTime: 2018/06/21 00:26:43 $
  $Author: pwbldsvc $


  when       who      what, where, why
  --------   ---      ------------------------------------
  04/10/18   kpalanic       Created

  ==============================================================================*/

/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/
#include "ACCommon.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define AC_VM_MASK(vmid) (1ULL << vmid)

#define AC_VALID_OEM_CONFIG_VM_MASK ( AC_VM_MASK(AC_VM_MSS_MSA) |\
                                      AC_VM_MASK(AC_VM_HLOS) )


/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
typedef struct {
    uint64 readVmMask;
    uint64 writeVmMask;
    uint32 regionId;
} ACOEMRegionPermissions;

#endif // __AC_OEM_CONFIG_H__
