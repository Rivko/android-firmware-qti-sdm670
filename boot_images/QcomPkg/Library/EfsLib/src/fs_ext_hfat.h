/***********************************************************************
 * fs_ext_hfat.h
 *
 * HFAT glue interface
 * Copyright (C) 2006, 2009-2010, 2013 QUALCOMM Technologies, Inc.
 *
 * Verbose description.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-05-24   wek   Make EXTFS ops constant.
2010-02-08   rp    CMI compliance, remove #ifdef from API files.
2009-08-11   wek   Remove hfat drive number coupled to hotplug device index.
2009-05-08   ebb   Removed FS_FAT_xxx macros.
2006-09-05   sh    fs_hfat_ops must be extern.  Fixed.
2006-08-27   sh    Add macros to provide INIT/OPS/FORMAT using HFAT
2006-08-09   sch   Create

===========================================================================*/

#ifndef __FS_EXT_HFAT_H__
#define __FS_EXT_HFAT_H__

#include "fs_config_i.h"

#ifdef FEATURE_EFS_EXTFS_HFAT

void fs_hfat_init (void);
int fs_hfat_format_core (int driveno, const void *hdev);
int fs_hfat_checkfat_core (int driveno, const void *hdev, int param,
          int *error_mask, void (*callback) (void *), void *cparam);

extern const struct fs_extfs_ops fs_hfat_ops;

#else

extern int __dont_complain_about_empty_file;

#endif /* FEATURE_EFS_EXTFS_HFAT */

#endif /* not __FS_EXT_HFAT_H__ */
