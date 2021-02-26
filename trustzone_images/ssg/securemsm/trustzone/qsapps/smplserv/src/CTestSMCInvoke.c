// Copyright (c) 2016 Qualcomm Technologies, Inc.  All Rights Reserved.
// Qualcomm Technologies Proprietary and Confidential.

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include "ITestSMCInvoke_invoke.h"
#include "ICredentials.h"
#include "stringl.h"
#include "qsee_time.h"
#include "qsee_log.h"
#include "qsee_heap.h"

#define MILLI_SECONDS                 1000
#define MICRO_SECONDS                 (MILLI_SECONDS * MILLI_SECONDS)

#define CMD_NAME  "__name__"
#define CMD_TIME  "__hlostime__"
#define CMD_CCRED "__ccred__"

/*******************************************************************************
 * Local data types
 * ****************************************************************************/

typedef struct TestSMCInvoke {
  int32_t refs;
  Object cred;
} TestSMCInvoke;

/*******************************************************************************
 * SMC Invoke test: perform SMC Invoke related tests over Mink IPC
 * ****************************************************************************/
static inline int32_t
CTestSMCInvoke_test(TestSMCInvoke *me,
                    const void *cmd_ptr,
                    size_t cmd_len,
                    void *result_ptr,
                    size_t result_len,
                    size_t *result_lenout)
{
  int res = 0;

  // we look for a particular string and read the UTC time from HLOS, causing a listener call
  if (strncmp(cmd_ptr,CMD_TIME, sizeof(CMD_TIME)) == 0) {
    struct tztimespec timeSpec  = {0, 0};
    uint64_t msec = 0;

    res = time_getutcsec(&timeSpec);
    if (res) {
      return ITestSMCInvoke_ERROR_TEST_FAILED;
    }
    msec = timeSpec.tv_nsec / MICRO_SECONDS + timeSpec.tv_sec * MILLI_SECONDS;
    qsee_log(QSEE_LOG_MSG_DEBUG, "%s(): msec from HLOS=%d", __FUNCTION__, msec);
    res = snprintf(result_ptr, result_len, "%llu", msec);
    if (res < 0) {
      return ITestSMCInvoke_ERROR_TEST_FAILED;
    }
    *result_lenout = (size_t)res;
    qsee_log(QSEE_LOG_MSG_DEBUG, "%s(): Returning time=\"%s\" (%zu chars)", __FUNCTION__, result_ptr, *result_lenout);

    return Object_OK;
  }

  // we return the caller's name, taken from the passed credentials
  if (strncmp(cmd_ptr,CMD_NAME,sizeof(CMD_NAME)) == 0) {
    memset(result_ptr, 0, result_len);

    res = ICredentials_getValueByName(me->cred, "n", 1, result_ptr, result_len, result_lenout);
    if (res < 0) {
      return ITestSMCInvoke_ERROR_TEST_FAILED;
    }
    qsee_log(QSEE_LOG_MSG_DEBUG, "%s() Returning name=\"%s\" (%zu chars)", __FUNCTION__, result_ptr, *result_lenout);
    return Object_OK;
  }

  // we return the caller's client credentials, taken from the passed credentials
  if (strncmp(cmd_ptr,CMD_CCRED,sizeof(CMD_CCRED)) == 0) {
    size_t i = 0;
    bool print = true;
    char *c_result_ptr = (char*)result_ptr;
    memset(result_ptr, 0, result_len);

    res = ICredentials_getValueByName(me->cred, "ccrd", 4, result_ptr, result_len, result_lenout);
    if (res < 0) {
      return ITestSMCInvoke_ERROR_TEST_FAILED;
    }
    // we want to have at least one character to allow debug printing
    if (*result_lenout >= result_len) {
      qsee_log(QSEE_LOG_MSG_ERROR, "%s() Buffer too small, no room for last NULL char: %zu", __FUNCTION__, *result_lenout);
      return ITestSMCInvoke_ERROR_TEST_FAILED;
    }
    c_result_ptr[*result_lenout]=0;
    // this can be binary when coming from HLOS, check if we can print it
    for (i = 0; i < *result_lenout; ++i) {
      if (!isprint(c_result_ptr[i])) {
        print = false;
        break;
      }
    }
    if (print) {
      qsee_log(QSEE_LOG_MSG_DEBUG, "%s() Returning ccred=\"%s\" (%zu chars)", __FUNCTION__, c_result_ptr, *result_lenout);
    } else {
      qsee_log(QSEE_LOG_MSG_DEBUG, "%s() ccred not printable (%zu bytes)", __FUNCTION__, *result_lenout);
    }
    return Object_OK;
  }

  return ITestSMCInvoke_ERROR_TEST_UNKNOWN;
}


static int32_t CTestSMCInvoke_retain(TestSMCInvoke *me)
{
  ++me->refs;
  return Object_OK;
}

static int32_t CTestSMCInvoke_release(TestSMCInvoke *me)
{
  if (--me->refs == 0) {
    Object_ASSIGN_NULL(me->cred);
    qsee_free(me);
  }
  return Object_OK;
}

static ITestSMCInvoke_DEFINE_INVOKE(CTestSMCInvoke_invoke, CTestSMCInvoke_, TestSMCInvoke *)

/**
 * Externally visible function: return an SMCInvoke test object
 */
int32_t CTestSMCInvoke_open(Object cred, Object *objOut)
{
  TestSMCInvoke *me = (TestSMCInvoke*)qsee_realloc(NULL, sizeof(TestSMCInvoke));
  if (!me) {
     return Object_ERROR_KMEM;
  }
  Object_INIT(me->cred, cred);
  me->refs = 1;
  *objOut = (Object) { CTestSMCInvoke_invoke, me };
  return Object_OK;
}
