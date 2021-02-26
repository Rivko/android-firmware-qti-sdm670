// Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
// Qualcomm Technologies Proprietary and Confidential.

#include "ITestSandbox_invoke.h"
#include "CTestSandbox.h"
#include "CTestSandbox_open.h"
#include "CSPI.h"
#include "ISPI.h"
#include "CI2C.h"
#include "II2C.h"
#include "CHwFuse.h"
#include "IHwFuse.h"
#include "CCrypto.h"
#include "ICrypto.h"
#include "object.h"
#include "qsee_env.h"
#include "qsee_log.h"

static inline int32_t
CTestSandbox_test(void *cxt,
                  const uint32_t id)
{
   Object o= Object_NULL;

   // IPC guarantees non-NULL pointers

   switch (id) {
      case ITestSandbox_TEST_VIOLATIONS:
         o= qsee_open_singleton(CSPI_UID);
         if (!Object_isNull(o)) {
            qsee_log(QSEE_LOG_MSG_ERROR, "CTestSandbox_test() SPI Open succeeded (fault)");

            ISPI_release(o);
            goto bail;
         }

         o= qsee_open_singleton(CHwFuse_UID);
         if (!Object_isNull(o)) {
            qsee_log(QSEE_LOG_MSG_ERROR, "CTestSandbox_test() HwFuse Open succeeded (fault)");

            IHwFuse_release(o);
            goto bail;
         }

         break;

      case ITestSandbox_TEST_ACCESS:
         o= qsee_open_singleton(CCrypto_UID);
         if (Object_isNull(o)) {
            qsee_log(QSEE_LOG_MSG_ERROR, "CTestSandbox_test() Crypto Open failed");

            goto bail;
         }

         ICrypto_release(o);

         o= qsee_open_singleton(CI2C_UID);
         if (Object_isNull(o)) {
            qsee_log(QSEE_LOG_MSG_ERROR, "CTestSandbox_test() I2C Open failed");

            goto bail;
         }

         II2C_release(o);

         break;

      default:
         break;
   }

   return Object_OK;

bail:
   return Object_ERROR;
}


// This implementation does not require a context record, so `retain` and
// `release` are no-ops.


#define CTestSandbox_release(ctx)   Object_OK
#define CTestSandbox_retain(ctx)    Object_OK

static ITestSandbox_DEFINE_INVOKE(CTestSandbox_invoke, CTestSandbox_, void*)

int32_t CTestSandbox_open(Object cred, Object *objOut)
{
   *objOut = (Object) { CTestSandbox_invoke, NULL };
   return Object_OK;
}
