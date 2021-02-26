/*==============================================================================
@file CoreMutex.h

Common API for OS mutex operations

Copyright (c) 2009-2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

==============================================================================*/
#ifndef COREMUTEX_H
#define COREMUTEX_H

/*
 * Global toggle to enable/disable all CoreMutex locks.
 * Access via Core_MutexEnable() and Core_MutexDisable() APIs.
 */
extern unsigned int CoreMutex_DisableLocks;

typedef int ( *CoreMutexMethod )( void* );

/* All mutex types must define the following routines */
typedef struct CoreMutexVtable
{
  CoreMutexMethod lock;
  CoreMutexMethod unlock;
  CoreMutexMethod destroy;
} CoreMutexVtable;

/* CoreMutexAttrType is a bitfield 
 * Not all attributes are supported on all platforms.  Before using an
 * attribute be sure to check the os/<os name>/CoreMutexImpl.c file to
 * make sure the attribute you are using is supported.
 */
typedef enum CoreMutexAttr
{
  CORE_MUTEXATTR_DEFAULT = 0x0,
  CORE_MUTEXATTR_NONRECURSIVE = 0x1,
  CORE_MUTEXATTR_CVCOMPAT = 0x2,
} CoreMutexAttr;

/* Mutex types. Not all types need be supported on all platforms */
typedef enum CoreMutexLockType
{
  CORE_MUTEXTYPE_OS,       /* Use the default OS lock */
  CORE_MUTEXTYPE_STUB,     /* No lock; provides stubbed out 
                              implementations of lock/unlock/destroy */
  CORE_MUTEXTYPE_INTLOCK,  /* Interrupt Lock */
} CoreMutexLockType;

typedef struct CoreMutex
{
  void *mtx;
  CoreMutexVtable *vtable;
  CoreMutexAttr attr;
} CoreMutex;

/* The type CoreMutexType is deprecated. New code must use CoreMutex instead */
typedef CoreMutex CoreMutexType;

/*
 * Create a mutex with type and attributes specified.
 * Invokes the implementation-defined Core_MutexInit function to create
 * the real mutex type. Returns a pointer to the created CoreMutex.
 */
CoreMutex* Core_MutexCreateEx( CoreMutexLockType type, CoreMutexAttr attr );

/*
 * Create a mutex of type CORE_MUTEXTYPE_OS with the given attributes
 */
#define Core_MutexCreate(attr) Core_MutexCreateEx( CORE_MUTEXTYPE_OS, (attr) )

/*
 * Destroy the mutex. Invokes the destroy function of the real mutex type.
 */
void Core_MutexDestroy( CoreMutex *mutex );

/*
 * The real mutex implementation on a target must define this function.
 */
int Core_MutexInit( CoreMutex *mutex, 
                    CoreMutexLockType type, CoreMutexAttr attr );

/*
 * Macros to Lock/Unlock the mutex
 */
#define Core_MutexLock( mutex ) \
  do { \
    if ( !CoreMutex_DisableLocks ) { (mutex)->vtable->lock( (mutex)->mtx ); } \
  } while(0)

#define Core_MutexUnlock( mutex ) \
  do { \
    if ( !CoreMutex_DisableLocks ) { (mutex)->vtable->unlock( (mutex)->mtx ); } \
  } while(0)

/*
 * Disable all CoreMutex locks. Not reentrant-safe.
 */
void Core_MutexDisable( void );

/*
 * Enable all CoreMutex locks. All locks are enabled by default; invoke only
 * if you have previously called Core_MutexDisable. Not reentrant-safe.
 */
void Core_MutexEnable( void );

/*
 * To implement lock types on a specific platform.
 *
 * Initialize one or more CoreMutexVtable structures depending on the lock 
 * types you wish to support on the platform.  Each CoreMutexVtable structure 
 * must have 3 function pointers - lock, unlock and destroy - each subscribing 
 * to the CoreMutexMethod signature
 *
 * Your mutex construct can be of any type. If composed of multiple elements
 * ensure that the real mutex type (that whose pointer is passed into 
 * Lock/Unlock/Destroy routines) is the first field in the struct.
 *
 * Define the Core_MutexInit function. Allocate your mutex type and
 * store the pointer into the mtx field of CoreMutex. Set the vtable
 * field to the address of your CoreMutexVtable struct. Return 0 if the
 * mutex was created successfully. A non-zero value indicates an error.
 *
 * In your Destroy routine, free the mutex type you allocated.
 */

#endif /* COREMUTEX_H */
