/*===========================================================================
  Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
  ===========================================================================*/

/*===========================================================================

  EDIT HISTORY FOR FILE
  $Header: //components/rel/ssg.tz/1.0.2/securemsm/trustzone/qsapps/sampleapp/src/app_sandbox_test.c#1 $
  $DateTime: 2018/02/06 03:00:17 $
  $Author: pwbldsvc $

  # when       who     what, where, why
  # --------   ---     ---------------------------------------------------------

  ===========================================================================*/
#include "CTestSandbox.h"
#include "ITestSandbox.h"
#include "qsee_env.h"
#include "qsee_log.h"


int run_sandbox_test()
{
   Object o;
   int32_t err;

   qsee_log(QSEE_LOG_MSG_DEBUG, "run_sandbox_test() getting sandbox handle");

   err = qsee_open(CTestSandbox_UID, &o);
   if (!Object_isOK(err) || Object_isNull(o)) {
      qsee_log(QSEE_LOG_MSG_ERROR, "run_sandbox_test() qsee_open() failed");
      o = Object_NULL;
      goto bail;
   }

   qsee_log(QSEE_LOG_MSG_DEBUG, "run_sandbox_test() running tests");

   err = ITestSandbox_test(o, ITestSandbox_TEST_VIOLATIONS);
   if (!Object_isOK(err)) {
      qsee_log(QSEE_LOG_MSG_ERROR, "run_sandbox_test() Sandbox violation test failed");
      goto bail;
   }

   err = ITestSandbox_test(o, ITestSandbox_TEST_ACCESS);
   if (!Object_isOK(err)) {
      qsee_log(QSEE_LOG_MSG_ERROR, "run_sandbox_test() Sandbox access test failed");
      goto bail;
   }

   qsee_log(QSEE_LOG_MSG_DEBUG, "run_sandbox_test() done");

   ITestSandbox_release(o);
   return 0;

bail:
   qsee_log(QSEE_LOG_MSG_ERROR, "run_sandbox_test() bail");

   if (!Object_isNull(o)) {
      ITestSandbox_release(o);
   }
   return -1;
}
