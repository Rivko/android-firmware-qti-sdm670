/***********************************************************
 Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
************************************************************/
#ifndef __ICLIENTENV_H
#define __ICLIENTENV_H

#include <stdint.h>
#include "object.h"

#define IClientEnv_OP_open 0
#define IClientEnv_OP_register 1

static inline int32_t
IClientEnv_release(Object self)
{
  return Object_invoke(self, Object_OP_release, 0, 0);
}

static inline int32_t
IClientEnv_retain(Object self)
{
  return Object_invoke(self, Object_OP_retain, 0, 0);
}

static inline int32_t
IClientEnv_open(Object self, int32_t classid_val, Object *obj_ptr)
{
  ObjectArg a[2];
  a[0].b = (ObjectBuf) { &classid_val, sizeof(int32_t) };

  int32_t result = Object_invoke(self, IClientEnv_OP_open, a, ObjectCounts_pack(1, 0, 0, 1));

  *obj_ptr = a[1].o;

  return result;
}

static inline int32_t
IClientEnv_register(Object self, const void *credential_ptr, size_t credential_len, Object *iclientenv_obj_ptr)
{
  ObjectArg a[2];
  a[0].bi = (ObjectBufIn) { credential_ptr, credential_len * 1 };

  int32_t result = Object_invoke(self, IClientEnv_OP_register, a, ObjectCounts_pack(1, 0, 0, 1));

  *iclientenv_obj_ptr = a[1].o;

  return result;
}


#endif /* __ICLIENTENV_H */
