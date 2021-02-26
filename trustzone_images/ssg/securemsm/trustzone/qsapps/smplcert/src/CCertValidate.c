// Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
// Qualcomm Technologies Proprietary and Confidential.

#include "IValidate_invoke.h"
#include "CCertValidate.h"
#include "CCertValidate_open.h"
#include "stringl.h"

static inline int32_t
CCertValidate_validate(void *cxt,
                      const void *wrapped_ptr,
                      size_t wrapped_len)
{
  // IPC guarantees non-NULL pointers

  // sample implementation: verify that the buffer can be read
  if (0 == wrapped_len % 2)
  {
    const char * cert_ptr = wrapped_ptr;
    if (0x30 == cert_ptr[0] &&
        0x82 == cert_ptr[1])
    {
      return Object_OK;
    }
  }
  return Object_ERROR;
}


// This implementation does not require a context record, so `retain` and
// `release` are no-ops.

#define CCertValidate_release(ctx)   Object_OK
#define CCertValidate_retain(ctx)    Object_OK

static IValidate_DEFINE_INVOKE(CCertValidate_invoke, CCertValidate_, void*)

int32_t CCertValidate_open(Object cred, Object *objOut)
{
   *objOut = (Object) { CCertValidate_invoke, NULL };
   return Object_OK;
}
