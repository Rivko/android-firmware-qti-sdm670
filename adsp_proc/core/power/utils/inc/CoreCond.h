/*============================================================================
@file CoreCond.h

Defines methods needed for condition variables.
Virtual methods are implemented in os/<os>/CoreCondImpl.c

Copyright (c) 2009-2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

$Header: //components/rel/core.qdsp6/2.1/power/utils/inc/CoreCond.h#1 $
============================================================================*/
#ifndef CORECOND_H
#define CORECOND_H

#include "DALSys.h"

typedef int ( *CoreCondMethodType1 )( void* );
typedef int ( *CoreCondMethodType2 )( void*, void* );

/* All condition variable implementations must define these routines */
typedef struct CoreCondVtable
{
  CoreCondMethodType2 wait;
  CoreCondMethodType1 signal;
  CoreCondMethodType1 broadcast;
  CoreCondMethodType1 destroy;
} CoreCondVtable;

typedef struct CoreCondType
{
  void *cv;
  CoreCondVtable *vtable;
  unsigned int attr;
} CoreCondType;

typedef unsigned int CoreCondAttrType;

/* Default */
#define CORE_CONDATTR_DEFAULT   0x0

/*
 * Create a condition variable with attributes specified.
 * Invokes the implemenation-defined Core_CondInit function to create
 * the real CV type.
 */
CoreCondType* Core_CondCreate( CoreCondAttrType attr );

/*
 * Destroy the condition variable.
 * Invokes the destroy function of the real CV type.
 */
void Core_CondDestroy( CoreCondType *cond );

/*
 * The real CV implementation on a target must define this function.
 */
int Core_CondInit( CoreCondType *cond, CoreCondAttrType attr );

#define Core_CondWait( cond, mutex )  \
             (cond)->vtable->wait( (cond)->cv, (mutex)->mtx )
#define Core_CondSignal( cond ) \
             (cond)->vtable->signal( (cond)->cv )
#define Core_CondBroadcast( cond ) \
             (cond)->vtable->broadcast( (cond)->cv )

/*
 * To implement condition variables types on a specific platform
 *
 * Initialize a CoreCondVtable structure with four function pointers
 * (for Wait, Signal, Broadcast & Destroy). All functions must subscribe to 
 * the CoreCondMethodType1/2 signatures, as appropriate (see CoreSync.h).
 *
 * Your condition variable construct can be of any type. If composed of
 * multiple elements, ensure that the real condvar type (that
 * whose pointer is passed into Wait/Signal/Broadcast/Destroy routines) is
 * the first field in the struct.
 *
 * Define the Core_CondInit function. Allocate your condvar type and
 * store the pointer into the cv field of CoreCondType. Set the vtable
 * field to the address of your CoreCondVtable struct. Return 0 if the
 * condition variable was created successfully. A non-zero value indicates 
 * an error.
 *
 * In your Destroy routine, free the condvar type you allocated.
 */

#endif /* CORECOND_H */
