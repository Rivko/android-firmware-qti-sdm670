/**************************************************************************
 * FILE: devprg_log.h
 *
 * Logging APIs to log errors, warnings, information and debug data.
 *
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Proprietary
 *
 *************************************************************************/

/*==========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/DevPrgLib/devprg_log.h#1 $
  $DateTime: 2017/09/18 09:26:13 $
  $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2016-12-16   wek   Modify the logging routine.
2015-11-17   wek   Create.

==========================================================================*/
#ifndef __DEVPRG_LOG_H__
#define __DEVPRG_LOG_H__

enum devprg_log_lvl
{
  LOG_NONE, /* Disable all logs. */
  LOG_ERROR,
  LOG_WARN,
  LOG_INFO,
  LOG_DEBUG,
  LOG_MAX_LIMIT
};

enum devprg_usb_log_state
{
  LOG_USB_DISABLE,
  LOG_USB_ENABLE
};

/* Print a log message either to console or send it through USB. */
int devprg_log(enum devprg_log_lvl lvl, const char *log, ...);
/* Set the verbose level */
int devprg_set_log_level(enum devprg_log_lvl level);
/* Disable/Enable logging through USB. */
int devprg_log_usb_set_state(enum devprg_usb_log_state state);


/* Define all the log macros as logMessage. This should actually map to a
 * specific function to raise or reduce the verbosity level. */
#define DP_LOGE(f, ...)  devprg_log(LOG_ERROR, f, __VA_ARGS__) /* Error */
#define DP_LOGW(f, ...)  devprg_log(LOG_WARN,  f, __VA_ARGS__) /* Warning */
#define DP_LOGI(f, ...)  devprg_log(LOG_INFO,  f, __VA_ARGS__) /* Info */
#define DP_LOGD(f, ...)  devprg_log(LOG_DEBUG, f, __VA_ARGS__) /* Debug, most verbose. */


#endif /* __DEVPRG_LOG_H__ */
