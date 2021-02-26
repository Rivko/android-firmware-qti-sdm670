/*===========================================================================

  rpm_synchandle.h - synchronization macros

  Copyright (c) 2012-2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#ifndef RPM_SYNCHANDLE_H
#define RPM_SYNCHANDLE_H

#include "CoreVerify.h"
#include "DALSys.h"

extern bool rpm_lockless_mode;

#define RPMSyncHandle DALSYSSyncHandle

#define RPMInitSyncHandle(lock) \
    if(DAL_SUCCESS != DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE, &lock, 0)) \
        ERR_FATAL("Couldn't create RPM lock.", 0, 0, 0); 

#define RPM_SyncEnter(lock) \
    if( !rpm_lockless_mode ){ DALSYS_SyncEnter(lock); }

#define RPM_SyncExit(lock) \
    if( !rpm_lockless_mode ){ DALSYS_SyncLeave(lock); }

#endif // RPM_SYNCHANDLE_H

