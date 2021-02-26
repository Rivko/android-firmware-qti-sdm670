// Copyright (c) 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
// Qualcomm Technologies Proprietary and Confidential.

#include <stdint.h>
#include <stddef.h>
#include "object.h"
#include "qsee_heap.h"
#include "IAppLegacyTest.h"
#include "IAppLegacyTest_invoke.h"
#include "CAppLegacyTest_open.h"
#include "qsee_log.h"

extern void tz_app_cmd_handler(void* cmd, uint32_t cmdlen,
                               void* rsp, uint32_t rsplen);

/*******************************************************************************
 * AppLegacyTest: Call the old tz_app_cmd_handler via a Mink object
 * ****************************************************************************/

#define CAppLegacyTest_retain(ctx)       Object_OK
#define CAppLegacyTest_release(ctx)      Object_OK


static int32_t CAppLegacyTest_handleRequest(void* cxt,
                                            const void *request_ptr,
                                            size_t request_len,
                                            void *response_ptr,
                                            size_t response_len,
                                            size_t *response_lenout)
{
  (void)cxt;
  tz_app_cmd_handler((void*)request_ptr, (uint32_t)request_len, response_ptr, (uint32_t)response_len);
  *response_lenout = response_len;
  return Object_OK;
}

static IAppLegacyTest_DEFINE_INVOKE(CAppLegacyTest_invoke, CAppLegacyTest_, void *)

/**
 * Externally visible function: return an AppLegacyTest object
 */
int32_t CAppLegacyTest_open(Object* objOut)
{
  *objOut = (Object) { CAppLegacyTest_invoke, NULL };
  return Object_OK;
}
