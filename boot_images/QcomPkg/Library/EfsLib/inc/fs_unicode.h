/***********************************************************************
 * fs_unicode.h
 *
 * Unicode conversion framework.
 * Copyright (C) 2006-2008,2012-2013 QUALCOMM Technologies, Inc.
 *
 * API for Unicoode (FAT) to/from EFS filename conversion
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-05-24   wek   Make unicode ops constant.
2012-03-20   wek   Cleanup customer header file inclusion.
2012-03-08   wek   Rename macros and remove header files to compile for boot.
2008-05-23   rp    Introducing /inc folder as per new SCM.
2007-07-10   sh    Added Shift-JIS short filename conversion support.
2007-03-09   sh    Moved some UTF-specific stuff out.
2007-02-16   yg    Changed struct definition & removed unused macro.
2007-02-14   yg    Added some macros, and changed struct contents to ptr.
2007-02-07   sh    Use standard UTF types.
2006-12-04   sh    Create

===========================================================================*/

#ifndef __FS_UNICODE_H__
#define __FS_UNICODE_H__

#include "fs_config_i.h"
#include "fs_convert_utf.h"     /* for the UTF types */

struct fs_fat_shortname {
  unsigned char *name;
  /* implied '.' */
  unsigned char *ext;
};

/*
 * Unicode conversion routines.
 *
 * In this context, "str" is a simple NUL-terminated string.  These
 * are EFS2-compliant pathnames.  UTF-16 strings are
 * Windows-compatible Unicode long filenames.
 */
struct unicode_conversion_ops {
  int (*to_utf16)   (const char *name, UTF16 *utf16, int max_words);
  int (*from_utf16) (const UTF16 *utf16, char *path, int max_chars);
  int (*to_shortname) (int pass,
                       const UTF16 *utf16,
                       struct fs_fat_shortname *shortname);
  int (*from_shortname) (const struct fs_fat_shortname *shortname,
                         UTF16 *utf16);
};

/* Points to the current (active) set of active conversion routines */
extern const struct unicode_conversion_ops *fs_convert;

/* Available handler sets for UTF-8 conversion are declared in this file */
#include "fs_unicode_utf8.h"

/*
 * Default conversion mapping.  The target may override this default
 * by pre-defining the value of EFS_DEFAULT_UNICODE_CONVERSION in
 * custefs.h or similar.
 */
#ifndef EFS_DEFAULT_UNICODE_CONVERSION
  /* This is most likely what the target expects */
  #ifdef FEATURE_EFS_CONVERT_SHIFT_JIS
    #define EFS_DEFAULT_UNICODE_CONVERSION fs_convert_utf8_sjis
  #else
    #define EFS_DEFAULT_UNICODE_CONVERSION fs_convert_utf8
  #endif
#endif

#endif /* not __FS_UNICODE_H__ */
