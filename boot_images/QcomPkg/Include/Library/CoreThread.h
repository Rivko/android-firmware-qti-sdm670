/*============================================================================
@file CoreThread.h

Abstraction layer for some thread APIs.

Copyright (c) 2009-2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR

============================================================================*/
#ifndef CORETHREAD_H
#define CORETHREAD_H

#include "CoreVerify.h"

#if defined( __cplusplus )
  #define CORE_STATIC_INLINE static inline
#else
  #define CORE_STATIC_INLINE static __inline
#endif

/* Implementation must define all of these functions */

/* Return priority of the current thread; see note below */
unsigned int Core_GetThreadSelfPriority( void );

/* Set priority of the thread with id 'tid'; see note below */
void Core_SetThreadPriority( unsigned int tid, unsigned int priority);
                             
/**
  @note (GetThreadSelf/SetThread)Priority API and the macro
  REQUEST_THREAD_PRIORITY in npa_target_config.c take priority values in
  REX/DALWL format, 0-255 with 255 being highest. This is opposite to the
  notion in QURT, Blast etc., where 0 is the highest priority.
*/
                             
/* Allocate a thread local storage (TLS) key. Aborts on failure */
unsigned int Core_TlsAlloc( void );

/* Save 'data' against 'key' in TLS of the current thread */
void Core_TlsSet( unsigned int key, void *data );

/**
  The below functions are also OS-specific but are used in a performance-
  critical path and so, inlined.
   
  // Return the current thread id
  unsigned int Core_ThreadSelf( void );

  // Returns pointer stored against 'key' in TLS of the current thread, or NULL 
  void* Core_TlsGet( unsigned int key );
*/

#if defined( WINSIM )

#include <windows.h>
CORE_STATIC_INLINE unsigned int Core_ThreadSelf( void )
{
  return GetCurrentThreadId();
}

CORE_STATIC_INLINE void* Core_TlsGet( unsigned int key )
{
  return TlsGetValue( key );
}

#elif defined(WIN8EA)

CORE_STATIC_INLINE unsigned int Core_ThreadSelf( void )
{
  return (unsigned int)KeGetCurrentThread();
}

#define CORE_UTILS_EMULATE_TLS

#elif defined( USES_L4 ) || defined( FEATURE_L4 )

#include "l4/thread.h"
#ifndef L4_THREADID_RAW
  #define L4_THREADID_RAW(tid) (tid.raw)
#endif

CORE_STATIC_INLINE unsigned int Core_ThreadSelf( void )
{
  return (unsigned int)L4_THREADID_RAW(L4_Myself());
}

#define CORE_UTILS_EMULATE_TLS

#elif defined( USES_BLAST ) || defined( FEATURE_BLAST )

#include "blast.h"
#include "blast_tls.h"
CORE_STATIC_INLINE unsigned int Core_ThreadSelf( void )
{
  return blast_thread_myid();
}

CORE_STATIC_INLINE void* Core_TlsGet( unsigned int key )
{
  return blast_tls_getspecific( key );
}

#elif defined( USES_PTHREAD ) || defined( FEATURE_PTHREAD )

#include <pthread.h>
CORE_STATIC_INLINE unsigned int Core_ThreadSelf( void )
{
  return (unsigned int)pthread_self();
}

CORE_STATIC_INLINE void* Core_TlsGet( unsigned int key )
{
  return pthread_getspecific( key );
}

#elif defined( FEATURE_REX ) || defined( FEATURE_RIVAIMG )

#include "rex.h"
CORE_STATIC_INLINE unsigned int Core_ThreadSelf( void )
{
  rex_tcb_type *rex_tcb = rex_self();
  return rex_tcb->thread_id;
}

CORE_STATIC_INLINE void* Core_TlsGet( unsigned int key )
{
  rex_tls_value_t data;

  /* Get the Key */
  if ( rex_tls_getspecific( key, &data ) == REX_TLS_SUCCESS )
  {
    return data;
  }
  /* failed to get key so return NULL */
  return NULL;
}

#elif defined( USES_QURT ) || defined( FEATURE_QURT )

#include "qurt.h"
#include "qurt_tls.h"
CORE_STATIC_INLINE unsigned int Core_ThreadSelf( void )
{
  return qurt_thread_get_id();
}

CORE_STATIC_INLINE void* Core_TlsGet( unsigned int key )
{
  return qurt_tls_get_specific( key );
}

#elif defined( TARGET_UEFI)
CORE_STATIC_INLINE unsigned int Core_ThreadSelf( void )
{
  return 0;
}
void* Core_TlsGet( unsigned int key );

#else

#error "ERROR: CoreThread routines not implemented"

#endif

#ifdef CORE_UTILS_EMULATE_TLS
extern void* Core_TlsGet( unsigned int key );
#endif

#endif /* CORETHREAD_H */
