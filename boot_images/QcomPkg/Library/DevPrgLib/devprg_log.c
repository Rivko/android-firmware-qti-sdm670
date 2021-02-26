/**************************************************************************
 * FILE: devprg_log.c
 *
 * Logging APIs to log errors, warnings, information and debug data.
 *
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Proprietary
 *
 *************************************************************************/

/*==========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/DevPrgLib/devprg_log.c#1 $
  $DateTime: 2017/09/18 09:26:13 $
  $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2017-04-11   wek   Build the log string once to allow different channels.
2016-12-16   wek   Create.

==========================================================================*/

#include "devprg_log.h"
#include "devprg_xml.h"
#include "devprg_transfer.h"
#include "devprg_firehose.h"
#include <stringl/stringl.h>
#include <string.h>
#include <stdarg.h>


enum devprg_log_lvl global_log_level = LOG_INFO;
enum devprg_usb_log_state usb_log_state = LOG_USB_ENABLE;

/* Copy as much log possible to the buffer. */
static int devprg_build_log(char *buf, size_t buf_size,
                            enum devprg_log_lvl lvl, const char* log,
                            va_list list)
{
  size_t retval;
  size_t remain;
  int result;

  remain = buf_size;
  switch(lvl)
  {
  case LOG_ERROR:
    retval = strlcpy(buf, "ERROR: ", remain);
    break;
  case LOG_WARN:
    retval = strlcpy(buf, "WARN: ", remain);
    break;
  case LOG_INFO:
    retval = strlcpy(buf, "INFO: ", remain);
    break;
  case LOG_DEBUG:
    retval = strlcpy(buf, "DEBUG: ", remain);
    break;
  default:
    retval = strlcpy(buf, "OTHER: ", remain);
    break;
  }
  if (retval >= remain)
    return buf_size;
  remain -= retval;
  buf += retval;

  result = vsnprintf(buf, remain, log, list);

  /* If the output was truncated or error occurred, say the log filled
   * the remaining space. */
  if (result < 0 || (size_t)result >= remain)
    result = remain;

  remain -= result;
  buf += result;

  return buf_size - remain;
}

int devprg_log(enum devprg_log_lvl lvl, const char *log, ...)
{
  int res = 0;
  va_list list;
  dp_res_t error;
  size_t log_size, buf_size;
  char *log_buffer;
  log_buffer = devprg_get_log_buffer(&log_size);
  buf_size = log_size;

  /* Check with global log level to see if this log needs to be printed. */
  if (lvl > global_log_level)
    return res;

  va_start(list, log);
  res = devprg_build_log(log_buffer, log_size, lvl, log, list);
  va_end(list);
  log_size = res;

  if (usb_log_state == LOG_USB_ENABLE)
  {
    res = devprg_xml_add_log_tags(log_buffer, buf_size, log_size);

    if (res > 0)
    {
      error = devprg_tx_blocking(log_buffer, res);
      if (error != DEVPRG_SUCCESS)
        res = -1;
      else
        res = 0;
    }
    else if (res == 0)
    {
      res = -1;
    }
  }

  return res;
}

int devprg_set_log_level(enum devprg_log_lvl level)
{
  if (level <= LOG_MAX_LIMIT)
    global_log_level = level;
  else
    return -1;
  return 0;
}

int devprg_log_usb_set_state(enum devprg_usb_log_state state)
{
  usb_log_state = state;
  return 0;
}
