/*!
  @file
  dsi_netctrl_platform.c

  @brief
  This file implements the platform specific functions.

*/

/*===========================================================================

  Copyright (c) 2010-2014, 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header:  $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/19/10   js      Created
===========================================================================*/
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "dsi_netctrl_platform.h"

/*===========================================================================
  FUNCTION:  dsi_format_log_msg
===========================================================================*/
/*!
    @brief
    formats log messages

    @return
    none
*/
/*=========================================================================*/
void dsi_format_log_msg
(
  char * buf, 
  int buf_size,
  char * fmt,
  ...
)
{
  va_list ap;

  va_start(ap, fmt);

  vsnprintf(buf, (size_t)buf_size, fmt, ap);

  va_end(ap);
}

void * dsi_malloc(size_t memsize)
{
  void * ptr = malloc(memsize);

  DSI_LOG_VERBOSE("dsi_mem_debug: malloc ptr [%p] memsize [%zu]", ptr, memsize);

  return ptr;
}

void dsi_free(void * ptr)
{
  if (NULL==ptr)
  {
    DSI_LOG_DEBUG("%s","dsi_mem_debug: attempt to free NULL ptr");
  }
  else
  {
    DSI_LOG_VERBOSE("dsi_mem_debug free ptr [%p]", ptr);
    free(ptr);
  }
}
