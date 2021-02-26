/***********************************************************************
 * fs_config.h
 *
 * EFS2 configuration settings.
 * Copyright (C) 2010-2011 QUALCOMM Technologies, Inc.
 *
 * This file contains all of the settable parameters used in EFS2.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when         who   what, where, why
----------   ---   ---------------------------------------------------------
2012-03-30   wek   Avoid direct inclusion of customer header file.
2011-04-25   wek   Add a dummy max transaction macro.
2010-11-11   wek   Remove dependency max transaction macro.
2010-07-12   rp    Added defines for major and minor number for API files.
2010-02-08   rp    Create

===========================================================================*/

#ifndef __FS_CONFIG_H__
#define __FS_CONFIG_H__


#ifdef CUST_H
  #include CUST_H
#endif

/** Identifies the major version number of this API file. */
#define VER_FS_CONFIG_H_MAJOR     1
/** Identifies the minor version number of this API file. */
#define VER_FS_CONFIG_H_MINOR     1

/**********************************************************************
 * CONFIGURATION ITEMS
 *
 * These parameters can be configured as desired.  Note that changing their
 * values doesn't affect existing files in the filesystem.  If a build is
 * loaded with smaller parameters for these values than earlier builds,
 * files may become inaccessible.
 */

/**
 * Maximum length of a full pathname, not including a trailing '\\0'
 * character.  The filesystem only has a few buffers of this length, so it
 * doesn't increase EFS2's RAM usage much, however, other code (such as
 * fs_compat, and fs_am) us this value to allocate a large number of
 * buffers. */
#ifndef FS_PATH_MAX
  #define FS_PATH_MAX 1024
#endif

/**
 * Maximum length of the 'name' component of a file.  The name is the
 * longest string of characters delimited by beginning of string, the '/'
 * character or the end of the string.  This is the maximum length for a
 * filename or directory name.
 */
#ifndef FS_NAME_MAX
  #define FS_NAME_MAX 768
#endif

/**
 * @deprecated This macro should not be used. It has no meaning outside
 * the file system.
 *
 * Maximum number of data pages in a transaction. This needs to be small
 * enough so that all of the transaction data can be processed in a single
 * log page. */
#define FS_MAX_TRANSACTION_DATA_PAGES 16

/**
 * @deprecated The value of this macro is no longer accurate. The maximum
 * transaction size in bytes for the file system depends on the underlying
 * device characteristics. To get the number of bytes that can be written in
 * a single write call use the API efs_statvfs().
 *
 * This macro defines the lowest maximum transaction size between all the
 * possible devices the file system can run on. To obtain the best
 * performance use efs_statvfs() or efs_fstatvfs() to get the maximum write
 * size. */
#define FS_MAX_TRANSACTION (FS_MAX_TRANSACTION_DATA_PAGES * 512)

#endif /* not __FS_CONFIG_H__ */
