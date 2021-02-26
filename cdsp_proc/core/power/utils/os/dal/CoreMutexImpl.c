/*==============================================================================
@file CoreMutexImpl.c

Implementation for DAL mutexes

Copyright (c) 2009-2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

$Header: //components/rel/core.qdsp6/2.1.c4/power/utils/os/dal/CoreMutexImpl.c#1 $
==============================================================================*/
#include "CoreMutex.h"
#include "DALSys.h"

static int LockMutex( void *mtx )
{
  DALSYS_SyncEnter( mtx );
  return 0;
}

static int TryLockMutex( void *mtx )
{
  return DALSYS_SyncTryEnter( mtx );
}

static int UnlockMutex( void *mtx )
{
  DALSYS_SyncLeave( mtx );
  return 0;
}

static CoreMutexVtable mutex_vtable =
{
  LockMutex,
  TryLockMutex,
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
