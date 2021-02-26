/*==============================================================================
@file CoreMutex.c

This file provides stub virtual functions for the methods
defined in CoreMutex.h

Copyright (c) 2010-2014 , 2018 by Qualcomm Technologies, Inc. All Rights Reserved
All Rights Reserved.
Qualcomm Confidential and Proprietary

==============================================================================*/
#include "CoreMutex.h"
#include "CoreVerify.h"
#include "DALSys.h"

/*
 * Global toggle to enable/disable all CoreMutex locks.
 */
unsigned int CoreMutex_DisableLocks;

/*
 * Disable all CoreMutex locks. Not reentrant-safe.
 */
void Core_MutexDisable( void )
{
  CoreMutex_DisableLocks = 1;
}

/*
 * Enable all CoreMutex locks. All locks are enabled by default; invoke only
 * if you have previously called Core_MutexDisable. Not reentrant-safe.
 */
void Core_MutexEnable( void )
{
  CoreMutex_DisableLocks = 0;
}

/* Stub mutex routines */
static int stub_mutex_lock( void *mtx )
{
  return 0;
}

static int stub_mutex_unlock( void *mtx )
{
  return 0;
}

static int stub_mutex_destroy( void *mtx )
{
  return 0;
}

static CoreMutexVtable stub_mutex_vtable = {
  stub_mutex_lock,
  stub_mutex_unlock,
  stub_mutex_destroy
};

/* Define a stub mutex */
static CoreMutexType stub_mutex = {
  NULL, &stub_mutex_vtable, CORE_MUTEXATTR_DEFAULT
};


CoreMutex* Core_MutexCreateEx( CoreMutexLockType type, CoreMutexAttr attr )
{
  CoreMutexType *mutex = NULL;

  if ( type == CORE_MUTEXTYPE_STUB )
  {
    return &stub_mutex;
  }

  CORE_VERIFY( (DALSYS_Malloc( sizeof(CoreMutex), (void**)&mutex )== DAL_SUCCESS) );
  if ( NULL != mutex ) /* Klocworks */
  {
      memset( mutex, 0, sizeof(CoreMutex) );
      mutex->attr = attr;

      if ( Core_MutexInit( mutex, type, attr ) != 0 )
      {
        DALSYS_Free( mutex );
        mutex = NULL;
      }
  }
  return mutex;
}

void Core_MutexDestroy( CoreMutex *mutex )
{
  /* Be tolerant of NULL input */
  if ( mutex && mutex != &stub_mutex )
  {
    CORE_VERIFY_PTR( mutex->vtable->destroy );
    mutex->vtable->destroy( mutex->mtx );
    DALSYS_Free( mutex );
  }
}