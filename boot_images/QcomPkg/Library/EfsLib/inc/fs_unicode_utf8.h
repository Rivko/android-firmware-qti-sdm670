/***********************************************************************
 * fs_unicode_utf8.h
 *
 * Shared declarations for UTF8 filename conversions
 * Copyright (C) 2007-2008, 2013 QUALCOMM Technologies, Inc.
 *
 * This is NOT a stand-alone header file.  It is used to declare an exported
 * conversion set for use in fs_hotplug.h
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-05-24   wek   Make unicode ops constant.
2008-05-23   rp    Introducing /inc folder as per new SCM.
2007-07-10   sh    Added Shift-JIS short filename conversion support.
2007-03-09   sh    Populated
2007-02-07   sh    Create

===========================================================================*/

#ifndef __FS_UNICODE_UTF8_H__
#define __FS_UNICODE_UTF8_H__

/* This module provides these Unicode conversion sets: */
extern const struct unicode_conversion_ops fs_convert_utf8;
extern const struct unicode_conversion_ops fs_convert_utf8_sjis;

#endif /* not __FS_UNICODE_UTF8_H__ */
