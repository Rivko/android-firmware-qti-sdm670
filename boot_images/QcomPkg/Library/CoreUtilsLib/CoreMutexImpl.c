/*==============================================================================
@file CoreMutexImpl.c

Implementation for DAL specific mutexes

Copyright (c) 2009-2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

==============================================================================*/
#include "CoreMutex.h"
#include "DALSys.h"

static int LockMutex( void *mtx )
{
  DALSYS_SyncEnter( mtx );
  return 0;
}

static int UnlockMutex( void *mtx )
{
  DALSYS_SyncLeave( mtx );
  return 0;
}

static CoreMutexVtable mutex_vtable =
{
  LockMutex,
  UnlockMutex,
  DALSYS_DestroyObject
};

int Core_MutexInit( CoreMutex *mutex, CoreMutexLockType type, CoreMutexAttr attr )
{
  int res;

  if ( type == CORE_MUTEXTYPE_INTLOCK )
  {
    res = DALSYS_SyncCreate(DALSYS_SYNC_ATTR_NO_PREEMPTION, &mutex->mtx, NULL);
  }
  else
  {
    res = DALSYS_SyncCreate( DALSYS_SYNC_ATTR_RESOURCE, &mutex->mtx, NULL );
  }
  if ( res == DAL_SUCCESS )
  {
    mutex->vtable = &mutex_vtable;
  }
  return res;
}
