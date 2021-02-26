/****************************************************************************
 * hfat_intf.h
 *
 * This file exposes all the glue functions needed by HFAT
 *
 * Copyright (C) 2006,2008,2009 QUALCOMM Technologies, Inc.
 *
 * Verbose Description
 *
 ***************************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when         who   what, where, why
----------   ---   ---------------------------------------------------------
2009-08-11   wek   Remove hfat drive number coupled to hotplug device index.
2008-06-04   ebb   Switched #include order to prevent compiler warning.
2008-05-23   rp    Introducing /inc folder as per new SCM.
2006-07-13   sch   create

===========================================================================*/
#ifndef _HFAT_INTF_H
#define _HFAT_INTF_H

#include "hfat_udefs_f.h"
#include "hfat_api_f.h"

F_DRIVER* hfat_device_init (int driveno, const void *hdev);
extern int fn_getsize_from_fp (F_FILE *fp, uint32 *numbytes);

#endif
