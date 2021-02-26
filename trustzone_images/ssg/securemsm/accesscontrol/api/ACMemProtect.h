#ifndef __AC_MEM_PROTECT_H__
#define __AC_MEM_PROTECT_H__
/*==============================================================================
  @file ACMemProtect.h
  Public interface for lock/unlockRegion APIs.

  Copyright (c) 2018, 2020 Qualcomm Technologies Incorporated.
  All rights reserved.
  Qualcomm Technologies Inc. Confidential and Proprietary
  ==============================================================================*/

#include "IAccessControl.h"
#include "ACCommon.h"

#define ACLockUnset (0)
#define ACIsLockSet(lock) (lock != ACLockUnset)

#define AC_PERM_MASK_R_WDC  IAccessControl_PERM_MASK_R_WDC /*Read and Write Don't Care mask*/
#define AC_PERM_MASK_RW     IAccessControl_PERM_MASK_RW /*Read and Write mask*/
#define AC_PERM_MASK_RO     IAccessControl_PERM_MASK_RO /*Read only mask*/

#define AC_VALIDATION_ALL_EXCLUSIVE IAccessControl_ValidationType_ALL_Exclusive
#define AC_VALIDATION_ANY_EXCLUSIVE IAccessControl_ValidationType_ANY_Exclusive
#define AC_VALIDATION_ALL_NON_EXCLUSIVE IAccessControl_ValidationType_ALL_NonExclusive
#define AC_VALIDATION_ANY_NON_EXCLUSIVE IAccessControl_ValidationType_ANY_NonExclusive

typedef IAccessControl_vmidPermission  ACVmidPermissionType;

AC_ERROR_CODE ACLockedRegionMutexInit(void);

/**
  @brief ACLockRegion: API to lock a given memory region if the vmidPermissions match with the
          existing permissions of the region based on the validationType.
          Refer IAccessControl.idl lockRegion() documentation for more explanation.
  @param[in]  vmidPermissions_ptr:  List of ACVmidPermissionType for validation.
  @param[in]  vmidPermissions_len:  vmidPermissions_ptr array length.
  @param[in]  uStart:               Start address of the memory region that needs to be locked.
  @param[in]  uSize:                Size of the memory region that needs to be locked.
  @param[in]  validationType:       One of the ValidationType_* values, see above for explanation.
  @param[out] pLockedRegionRef:     Handle to the locked region.
  @return AC_SUCCESS if success. AC_ERR_* otherwise.
  */
AC_ERROR_CODE ACLockRegion(const ACVmidPermissionType *vmidPermissions_ptr,
                                        size_t vmidPermissions_len,
                                        uint64_t uStart,
                                        uint64_t uSize,
                                        uint32_t validationType,
                                        uintptr_t *pLockedRegionRef);

/**
  @brief ACUnlockRegion: API to unlock a memory region that was locked before by ACLockRegion()
  @param[in]  lockHandle: Lock Handle that was returned previously by ACLockRegion().
  @return AC_SUCCESS if success. AC_ERR_* otherwise.
  */
AC_ERROR_CODE ACUnlockRegion(uintptr_t uLockedRegionRef);


/**
  @brief ACLockedRegionsPermissionsAllowed: API to check if the new permission change for the
          memory region is allowed in case the region is already locked.
  @param[in]  uStart:               Start address of the memory region.
  @param[in]  uSize:                Size of the memory region.
  @param[in]  uReadVMPerm:          Read VM permission mask.
  @param[in]  uWriteVMPerm:         Write VM permission mask.
  @return AC_SUCCESS if permission change is allowed. AC_ERR_MEMORY_IS_LOCKED otherwise.
  */
AC_ERROR_CODE ACLockedRegionsPermissionsAllowed(uint64_t uStart, uint64_t uSize,
                                                uint64_t uReadVMPerm, uint64_t uWriteVMPerm);

/**
  @brief ACLockedRegionsModificationAllowed: API to check if any permission change for the
          memory region is allowed in case the region is already locked.
  @param[in]  uStart:               Start address of the memory region.
  @param[in]  uSize:                Size of the memory region.
  @return AC_SUCCESS if permission change is allowed. AC_ERR_MEMORY_IS_LOCKED otherwise.
  */
AC_ERROR_CODE ACLockedRegionsModificationAllowed(uint64_t uStart, uint64_t uSize);

#endif //__AC_MEM_PROTECT_H__