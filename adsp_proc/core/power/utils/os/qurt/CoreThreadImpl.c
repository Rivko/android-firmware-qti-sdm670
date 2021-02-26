/*==============================================================================
@file CoreThreadImpl.c

Implementation of QURT specific thread routines for CoreThread

Copyright (c) 2009-2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

$Header: //components/rel/core.qdsp6/2.1/power/utils/os/qurt/CoreThreadImpl.c#1 $
==============================================================================*/
#include "CoreThread.h"
#include "CoreVerify.h"
#include "qurt.h"
#include "qurt_tls.h"

unsigned int Core_GetThreadSelfPriority( void )
{
  return (255 - qurt_thread_get_priority( qurt_thread_get_id() ));
}

void Core_SetThreadPriority( unsigned int tid,
                             unsigned int priority)
{
  qurt_thread_set_priority( tid, (255 - priority) );
}

unsigned int Core_TlsAlloc( void )
{
  int key = 0;
  CORE_VERIFY( qurt_tls_create_key( &key, NULL ) == QURT_EOK );
  return key;
}

void Core_TlsSet( unsigned int key, void *data )
{
  CORE_VERIFY( qurt_tls_set_specific( key, data ) == QURT_EOK );
}
