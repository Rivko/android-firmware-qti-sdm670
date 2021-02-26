#ifndef SLEEP_CORELOCK_H
#define SLEEP_CORELOCK_H

/*=============================================================================
  FILE:         CoreLock.h
  
  OVERVIEW:     This file provides lock abstraction for different type of OS
                locks.
 
                Copyright (c) 2011 Qualcomm Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
=============================================================================*/
/*=============================================================================
$Header: 
$DateTime: 
$Author: 
=============================================================================*/

#include "CoreVerify.h"

#ifdef __cplusplus
extern "C" {
#endif 

#define CoreLockType DALSYSSyncHandle

#define Core_LockCreateObj(handle_ptr) \
  CORE_DAL_VERIFY(DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE, handle_ptr, NULL) )
  
#define Core_LockAcquire(handle) DALSYS_SyncEnter(handle)

#define Core_LockRelease(handle) DALSYS_SyncLeave(handle)


#ifdef __cplusplus
}
#endif

#endif

