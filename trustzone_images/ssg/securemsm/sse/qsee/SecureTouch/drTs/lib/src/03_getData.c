/*
 * Copyright (c) 2012 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */
#include "sselog.h"
#include "SecureTouchError.h"
#include "drTsController.h"
#include "TouchApi.h"
#include <qsee_timer.h>

struct tsFingerData *g_fingers_ptr = NULL;
extern uint32_t g_drTs_sid;
extern struct drTsFunc g_ts_dr_func;

uint32_t touchGetData(const uint32_t sid, struct tsFingerData *fingers) {
  uint8_t i = 0;
  uint32_t rv = SSE_OK;
  ENTER;
  do {
    if ((g_drTs_sid == DR_SID_INVALID) || (g_drTs_sid != sid)) {
      LOG_E("Bad sid");
      rv = E_SSE_INVALID_PARAMETER;
      break;
    }
    // map in the passed fingers structure
    g_fingers_ptr = fingers;
    if (g_fingers_ptr == NULL) {
      LOG_E("Bad buffer to store the fingers data");
      rv = E_SSE_INVALID_PARAMETER;
      break;
    }

     if (!g_ts_dr_func.drTsProcessEvents) {
      LOG_E("No drTsProcessEvents function");
      rv = E_SSE_NULL_POINTER;
      break;
    }

    if (0 != g_ts_dr_func.drTsProcessEvents()) {
      LOG_E("Failed to read event from Touch controller");
      rv = E_SSE_UNKNOWN;
      break;
    }
    for (i = 0; i < MAX_FINGER_NUM; i++) {
      if (g_fingers_ptr->finger[i].event) {
        LOG_I("[%u]: %u, (%u, %u)",i,
          g_fingers_ptr->finger[i].event,
          g_fingers_ptr->finger[i].x,
          g_fingers_ptr->finger[i].y);
      }
    }
    g_fingers_ptr = NULL;
  } while(0);
  EXITV(rv);
}

/** Notify the upper layer of a touch event.
 *
 */
uint32_t drNotifyEvent(const uint32_t x, const uint32_t y,
                       const uint32_t code, const uint8_t finger)
{
  uint32_t rv = 0;
  ENTER;
  do {
    if (finger >= MAX_FINGER_NUM) {
      LOG_E("Only %u fingers supported. Received event for finger %u",
        MAX_FINGER_NUM, finger);
      rv = 0xFFFFFFFD;
      break;
    }
    if (g_fingers_ptr == NULL) {
      LOG_W("TL is not requesting data, data is silently discarded");
      break;
    }
    g_fingers_ptr->finger[finger].event |= code;
    g_fingers_ptr->finger[finger].x = x;
    g_fingers_ptr->finger[finger].y = y;
    g_fingers_ptr->finger[finger].timestamp = qsee_get_uptime();
    g_fingers_ptr->finger[finger].dirty = 1;
    LOG_I("[%u]: %u, (%u, %u)",finger, g_fingers_ptr->finger[finger].event,
      g_fingers_ptr->finger[finger].x, g_fingers_ptr->finger[finger].y);
  } while (0);
  EXITV(rv);
}
