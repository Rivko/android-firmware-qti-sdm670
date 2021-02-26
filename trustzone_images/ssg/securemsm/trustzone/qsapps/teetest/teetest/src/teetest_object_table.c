/*===========================================================================
Copyright (c) 2017 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

#include "object.h"
#include "ObjectTable.h"

static Object tee_Objects[MAX_INBOUND_OBJECTS];
static int32_t tee_ObjectRefs[MAX_INBOUND_OBJECTS];
static ObjectTable tee_object_table = {&tee_Objects[0], &tee_ObjectRefs[0], MAX_INBOUND_OBJECTS};

int tee_object_table_add(Object o){
  return ObjectTable_addObject(&tee_object_table, o);
}

Object tee_object_table_recover(int handle){
  return ObjectTable_recoverObject(&tee_object_table, handle);
}
