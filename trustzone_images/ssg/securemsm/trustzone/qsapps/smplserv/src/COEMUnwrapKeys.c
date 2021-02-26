// Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
// Qualcomm Technologies Proprietary and Confidential.

#include "IUnwrapKeys_invoke.h"
#include "COEMUnwrapKeys.h"
#include "COEMUnwrapKeys_open.h"
#include "stringl.h"

// Null pointer global for use to alleviate build warnings seen when
// intentionally using a null pointer. Static assignment and use will result in a compiler warning.
void *null_ptr = NULL;

static inline int32_t
COEMUnwrapKeys_unwrap(void *cxt,
                      const void *wrapped_ptr,
                      size_t wrapped_len,
                      void *unwrapped_ptr,
                      size_t unwrapped_len,
                      size_t *unwrapped_lenout)
{
  // IPC guarantees non-NULL pointers

  // we look for a particular string and crash the app when we see it
  if (strcmp(wrapped_ptr,"__crash__") == 0)
  {
    int dummy =0;
    unwrapped_ptr = null_ptr;
    dummy = ((Object *)unwrapped_ptr)->invoke(NULL,0,NULL,0); // should crash
  }

  // sample implementation: copy input to output
  *unwrapped_lenout = memscpy(unwrapped_ptr, unwrapped_len, wrapped_ptr, wrapped_len);
  return Object_OK;
}


// This implementation does not require a context record, so `retain` and
// `release` are no-ops.

#define COEMUnwrapKeys_release(ctx)   Object_OK
#define COEMUnwrapKeys_retain(ctx)    Object_OK

static IUnwrapKeys_DEFINE_INVOKE(COEMUnwrapKeys_invoke, COEMUnwrapKeys_, void*)

int32_t COEMUnwrapKeys_open(Object cred, Object *objOut)
{
   *objOut = (Object) { COEMUnwrapKeys_invoke, NULL };
   return Object_OK;
}
