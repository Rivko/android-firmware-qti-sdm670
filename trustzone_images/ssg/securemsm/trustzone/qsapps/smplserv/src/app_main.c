/*
@file app_main.c
@brief App main entry point.

*/
/*===========================================================================
   Copyright (c) 2011-2017 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

#include <stdint.h>
#include "qsee_log.h"
#include "object.h"
#include "COEMUnwrapKeys.h"
#include "COEMUnwrapKeys_open.h"
#include "CTestSandbox.h"
#include "CTestSandbox_open.h"
#include "CHavenTokenApp.h"
#include "CTestSMCInvoke_open.h"
#include "IOpener.h"

int32_t tz_module_open(uint32_t uid, Object cred, Object *objOut)
{
  qsee_log(QSEE_LOG_MSG_DEBUG, "Sample Server tz_module_open() uid: %d", uid);

  switch (uid) {
     case COEMUnwrapKeys_UID:
        return COEMUnwrapKeys_open(cred, objOut);

     case CTestSandbox_UID:
        return CTestSandbox_open(cred, objOut);

     case CHavenTokenApp_UID:
        // we make the SMCInvoke test service available in disguise as HavenTokenApp
        // so clientEnv let it be exposed to HLOS.
        return CTestSMCInvoke_open(cred, objOut);

     default:
        break;
  }

  *objOut = Object_NULL;
  return Object_ERROR_INVALID;
}

int32_t app_getAppObject(Object credentials, Object *obj_ptr)
{
  qsee_log(QSEE_LOG_MSG_DEBUG, "Sample Server app_getAppObject()");

  return CTestSMCInvoke_open(credentials, obj_ptr);
}

void tz_app_init(void)
{
  qsee_log_set_mask(QSEE_LOG_MSG_ERROR | QSEE_LOG_MSG_FATAL | QSEE_LOG_MSG_DEBUG);

  /*  App specific initialization code*/
  qsee_log(QSEE_LOG_MSG_DEBUG, "Sample Server init.");
}

void tz_app_cmd_handler(void* cmd, uint32_t cmdlen,
                        void* rsp, uint32_t rsplen)
{
  qsee_log(QSEE_LOG_MSG_DEBUG, "Sample Server received command.");
  return;
}

void tz_app_shutdown(void)
{
  qsee_log(QSEE_LOG_MSG_DEBUG, "Sample Server shutdown.");
  return;
}
