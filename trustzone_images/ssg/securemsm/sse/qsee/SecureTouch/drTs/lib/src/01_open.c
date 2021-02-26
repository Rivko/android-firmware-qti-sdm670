/*
 * Copyright (c) 2013 , 2016 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */
#include "sselog.h"
#include "SecureTouchError.h"
#include "drTsController.h"
#include "TouchApi.h"

#include "atmel_mxt_ts.h"
#include "synaptics_ts.h"
#include "focaltech_ts.h"
#include "loopback_ts.h"

/*
Below are I2C slave address of the touch controllers in decimal representation
*/
#define SYNAPTICS_TC       32
#define FOCALTECH_TC       56
#define SYNAPTICS_332U_TC  75  //slave address of synaptics TC on 8953 is 0x4b -> 75
#define LOOPBACK_TC        255  // too large to be physical address to place the loopback in 

uint32_t g_drTs_sid = DR_SID_INVALID;
struct drTsFunc g_ts_dr_func = {
  .drTsOpen = NULL,
  .drTsClose = NULL,
  .drTsProcessEvents = NULL
};

uint32_t touchOpen(uint32_t *sid, const uint32_t width, const uint32_t height,uint32_t touchController) {
  uint32_t rv = SSE_OK;
  ENTER;
  do {
    if (g_drTs_sid != DR_SID_INVALID) {
      LOG_E("Secure touch already in use");
      rv = E_ST_BUSY;
      break;
    }

    switch ( touchController ) {
             case SYNAPTICS_TC:
             case SYNAPTICS_332U_TC:
                  //Synaptics touch controller
                  LOG_D("Detected Synaptics controller \r\n");
                  g_ts_dr_func.drTsOpen          = synaptics_drTsOpen;
                  g_ts_dr_func.drTsClose         = synaptics_drTsClose;
                  g_ts_dr_func.drTsProcessEvents = synaptics_drTsProcessEvents;
                  break;
             case FOCALTECH_TC:
                  //FocalTech touch controller
                  LOG_D("Detected FocalTech controller , disabled  \r\n");
                  //g_ts_dr_func.drTsOpen          = focalTech_drTsOpen;
                  //g_ts_dr_func.drTsClose         = focalTech_drTsClose;
                  //g_ts_dr_func.drTsProcessEvents = focalTech_drTsProcessEvents;
                  break;
	    case LOOPBACK_TC:
                  //Loopback touch controller
                  LOG_D("Detected HLOS-loopback controller \r\n");
                  g_ts_dr_func.drTsOpen          = loopback_drTsOpen;
                  g_ts_dr_func.drTsClose         = loopback_drTsClose;
                  g_ts_dr_func.drTsProcessEvents = loopback_drTsProcessEvents;
                  break;  
             default:
                  LOG_E("No controller info is passed, using synaptics as default \r\n");
                  if (width < 2000) { // fluid
                    LOG_D("Detected Fluid, using Synaptics driver");
                    g_ts_dr_func.drTsOpen          = synaptics_drTsOpen;
                    g_ts_dr_func.drTsClose         = synaptics_drTsClose;
                    g_ts_dr_func.drTsProcessEvents = synaptics_drTsProcessEvents;
                  } else {
                    LOG_D("Detected Liquid, using Atmel driver");
                    g_ts_dr_func.drTsOpen          = atmel_drTsOpen;
                    g_ts_dr_func.drTsClose         = atmel_drTsClose;
                    g_ts_dr_func.drTsProcessEvents = atmel_drTsProcessEvents;
                  }
                  break;
    }

    if (0 != g_ts_dr_func.drTsOpen(width,height)) {
      LOG_E("Failed to open Touch interface");
      rv = E_ST_DRIVER_ERROR; // the TZBSP I2C interface doesn't return much info on failures...
      break;
    }
    LOG_E("Successfull in opening touch interface \r\n");
    g_drTs_sid = 0; // poor man random...
    *sid = g_drTs_sid;
  } while(0);
  EXITRV;
}
