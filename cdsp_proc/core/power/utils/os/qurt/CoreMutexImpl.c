/*==============================================================================
@file CoreMutexImpl.c

Implementation for QuRT mutexes

Copyright (c) 2009-2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

$Header: //components/rel/core.qdsp6/2.1.c4/power/utils/os/qurt/CoreMutexImpl.c#1 $
==============================================================================*/
#include "CoreMutex.h"
#include "CoreVerify.h"
#include <qurt_pimutex.h>
#include "DALSys.h"

static int lock_pimutex( void *mtx )
{
  qurt_pimutex_lock( mtx );
  return 0;
}

static int trylock_pimutex( void *mtx )
{
  return qurt_pimutex_try_lock( mtx );
}

static int unlock_pimutex( void *mtx )
{
  qurt_pimutex_unlock( mtx );
  return 0;
}

static int destroy_pimutex( void *mtx )
{
  qurt_pimutex_destroy( mtx );
  return DALSYS_Free( mtx );
}

static CoreMutexVtable pimutex_vtable = {
  lock_pimutex,
  trylock_pimutex,
  unlock_pimutex,
  destroy_pimutex
};

int Core_MutexInit( CoreMutex *mutex, CoreMutexLockType type, CoreMutexAttr attr )
{
  CORE_DAL_VERIFY( DALSYS_Malloc( sizeof(qurt_mutex_t), &mutex->mtx ) );

  qurt_pimutex_init( mutex->mtx );
  mutex->vtable = &pimutex_vtable;
  return 0;
}
