/*
@file app_main.c
@brief App main entry point.

*/
/*===========================================================================
   Copyright (c) 2018 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

#include <stdint.h>
#include "qsee_log.h"
#include "object.h"
#include "IOpener.h"
#include "CKeyGen.h"
#include "CKeyGen_open.h"

int32_t tz_module_open(uint32_t uid, Object cred, Object *objOut)
{
  qsee_log(QSEE_LOG_MSG_DEBUG, "KeyGenTest tz_module_open() uid: %d", uid);

  switch (uid) {

     case CKeyGen_UID:
        return CKeyGen_open(cred, objOut);

     default:
        break;
  }

  *objOut = Object_NULL;
  return Object_ERROR_INVALID;
}

void tz_app_init(void)
{
  /*  App specific initialization code*/
  qsee_log(QSEE_LOG_MSG_DEBUG, "KeyGenTest Init ");
}

void tz_app_cmd_handler(void* cmd, uint32_t cmdlen,
                        void* rsp, uint32_t rsplen)
{
  qsee_log(QSEE_LOG_MSG_DEBUG, "KeyGenTest received command.");
  return;
}

void tz_app_shutdown(void)
{
  qsee_log(QSEE_LOG_MSG_DEBUG, "KeyGenTest Shutdown.");
  return;
}
