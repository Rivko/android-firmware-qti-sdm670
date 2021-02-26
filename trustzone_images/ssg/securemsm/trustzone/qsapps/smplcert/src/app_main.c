/*
@file app_main.c
@brief App main entry point.

*/
/*===========================================================================
   Copyright (c) 2011-2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

#include <comdef.h>
#include <string.h>
#include "qsee_log.h"
#include "qsee_heap.h"
#include "object.h"
#include "CCertValidate.h"
#include "CCertValidate_open.h"

int32_t tz_module_open(uint32_t uid, Object cred, Object *objOut)
{
  qsee_log(QSEE_LOG_MSG_DEBUG, "Sample Cert Validate tz_module_open() uid: %d", uid);

  if (uid == CCertValidate_UID) {
    return CCertValidate_open(cred, objOut);
  }

  *objOut = (Object){ NULL, NULL };
  return Object_ERROR_INVALID;
}

void tz_app_init(void)
{
  /*  App specific initialization code*/  
  qsee_log(QSEE_LOG_MSG_DEBUG, "Sample Cert Validate init.");
}

void tz_app_cmd_handler(void* cmd, uint32 cmdlen, 
                        void* rsp, uint32 rsplen)
{
  qsee_log(QSEE_LOG_MSG_DEBUG, "Sample Cert Validate received command.");
  return;
}

void tz_app_shutdown(void)
{
  qsee_log(QSEE_LOG_MSG_DEBUG, "Sample Cert Validate shutdown.");
  return;
}
