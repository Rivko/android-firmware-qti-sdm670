/*
 * Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */
#include "sselog.h"
#include "SecureTouchError.h"
#include "drTsController.h"
#include "TouchApi.h"

extern uint32_t g_drTs_sid;
extern struct drTsFunc g_ts_dr_func;

uint32_t touchClose(const uint32_t sid) {
  uint32_t rv = SSE_OK;
  ENTER;
  do {
    LOG_I("Close Session: %d",sid);
    if ((g_drTs_sid == DR_SID_INVALID) || (g_drTs_sid != sid)) {
      LOG_E("Bad sid");
      rv = E_SSE_INVALID_PARAMETER;
      break;
    }

    if (!(g_ts_dr_func.drTsClose) ||
        !(g_ts_dr_func.drTsProcessEvents)) {
      rv = E_SSE_NULL_POINTER;
	  LOG_E("Failed to initialise secure touch::drTsClose driver");
      break;
    }

    // first we empty the queue of currently registered events
    if (0 != g_ts_dr_func.drTsProcessEvents()) {
      LOG_E("Failed to read event from Touch controller, do not close");
      rv = E_ST_DRIVER_ERROR;
      break;
    }
    // then we close our connection
    if (0 != g_ts_dr_func.drTsClose()) {
      LOG_E("Failed to close Touch interface");
      // nothing we can do about it
    }
    g_drTs_sid = DR_SID_INVALID;

  } while(0);
  EXITRV;
}
