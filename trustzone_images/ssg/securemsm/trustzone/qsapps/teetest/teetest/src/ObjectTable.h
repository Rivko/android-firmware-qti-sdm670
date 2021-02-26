// Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
// Qualcomm Technologies Proprietary and Confidential.

// ObjectTable
//
// An object table keeps track of local objects held by a remote domain,
// while will identify them with small non-negative integers.  The object
// table also counts reference to handles (not to be confused with the
// internal reference counting performed by each object).  When the handle
// reference count goes to zero, the object table slot is freed.
//
// The object table suports the following operations:
//
//  addObject()      Store an object in a slot in the table, yielding a handle.
//  recoverObject()  Recover an object, given a handle.
//  retainHandle()   Increment the reference count.
//  releaseHandle()  Decrement the reference count, and free the slot when it
//                   reaches zero.
//


#ifndef __OBJECTTABLE_H
#define __OBJECTTABLE_H

#include "object.h"

#define MAX_INBOUND_OBJECTS         4

static Object myOobjects[MAX_INBOUND_OBJECTS];

typedef struct {
  // An array of objects held by a remote domain.
  Object *objects;

  // An array of reference counts managed by the remote domain.
  int32_t *objectRefs;

  // Size of the objects[] and objectRefs[] arrays.
  size_t objectsLen;
} ObjectTable;


// Add an object to the table, assigning it a handle.  Set its 'ref' value
// to 1.
//
// On success, return the handle.
// On failure, return -1.
//
static inline int ObjectTable_addObject(ObjectTable *me, Object obj)
{
  for (int n = 0; n < (int) me->objectsLen; ++n) {
    if (Object_isNull(me->objects[n])) {
      me->objectRefs[n] = 1;
      me->objects[n] = obj;
      Object_retain(obj);
      return n;
    }
  }
  return -1;
}


// Return the kernel object to which an outbound object forwards invokes.
// If there is no object at that slot, return Object_NULL.  Otherwise, the
// returned object has been retained, and the caller is repsonsible for
// releasing it.
//
static inline Object ObjectTable_recoverObject(ObjectTable *me, int h)
{
  if (h >= 0 && h < (int) me->objectsLen) {
    Object o = me->objects[h];
    if (!Object_isNull(o)) {
      Object_retain(o);
      return o;
    }
  }
  return Object_NULL;
}


// Empty the object table entry and release the object.
//
static inline void ObjectTable_closeHandle(ObjectTable *me, int h)
{
  if (!Object_isNull(me->objects[h])) {
    Object_release(me->objects[h]);
    me->objects[h].invoke = NULL;
  }
}


// Increment the count in the references table.
//
static inline void ObjectTable_retainHandle(ObjectTable *me, int h)
{
  if (h >= 0 && h < (int) me->objectsLen) {
    ++me->objectRefs[h];
  }
}

// Decrement the count in the references table, and release the associated
// object when it reaches zero.
//
static inline void ObjectTable_releaseHandle(ObjectTable *me, int h)
{
  if (h >=0 && h < (int) me->objectsLen) {
      int ref = --me->objectRefs[h];
      if (ref == 0) {
        ObjectTable_closeHandle(me, h);
      }
  }
}



#endif // __OBJECTTABLE_H
