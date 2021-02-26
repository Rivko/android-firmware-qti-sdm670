
/***********************************************************************
 * fs_errno.h
 *
 * Posix interface for Embedded File System.
 * Copyright (C) 2002-2008, 2010-2013 QUALCOMM Technologies, Inc.
 *
 * This file declares error numbers used in the posix interface
 *
 ***********************************************************************/

 /*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when          who     what, where, why
--------      ---     ------------------------------------------------------
2013-01-31     rp     Add ENOTHINGTOSYNC
2012-10-23     nr     Add ETIMEDOUT
2011-08-14    wek     Get the errno from global header file.
2010-07-12     rp     Added defines for major and minor number for API files.
2008-05-23     rp     Introducing /inc folder as per new SCM.
2007-06-09     rp     Doxygen comments markup.
2007-03-12     rp     Added EILSEQ and ENOERR.
2007-01-31     rp     Added EROLLBACK for Rollback feature.
2006-08-10    sch     Add code for unspecified HFAT errors.
2006-07-07    dlb     Add code for unspecified SFAT errors.
2006-05-10    dlb     Add ESTALE.
2006-01-11    nrs     Fixed Copyright header
2005-09-19    dlb     Update error numbers to use standard values.
2005-09-16    nrs     Added EDQUOT
2005-03-23    dlb     Add ENODEV.
2004-10-15    dlb     Update copyright line.
2004-10-07    dlb     Whitespace cleanup.
2004-07-19    dlb     Add EXDEV code.
2003-09-30     pg     Added ENOCARD and EBADFMT err code.
2003-06-17     gr     Added ETXTBSY err code.
2002-08-20    adm     Created.

===========================================================================*/


#ifndef _FS_ERRNO_H
#define _FS_ERRNO_H

/** Identifies the major version number of this API file. */
#define VER_FS_ERRNO_H_MAJOR     1
/** Identifies the minor version number of this API file. */
#define VER_FS_ERRNO_H_MINOR     2

#include "errno.h"

/* These are constants for the errnos that we use and return in EFS2.
 * Most of these values should be already defined in errno.h */

#ifndef ENOERR
  #define ENOERR           0      /**< Success */
#endif

#ifndef EPERM
  #define EPERM         1001      /**< Operation not permitted */
#endif

#ifndef ENOENT
  #define ENOENT        1002      /**< No such file or directory */
#endif

#ifndef EEXIST
  #define EEXIST        1006      /**< File exists */
#endif

#ifndef EBADF
  #define EBADF         1009      /**< Bad file descriptor */
#endif

#ifndef ENOMEM
  #define ENOMEM        1012      /**< Out of memory. */
#endif

#ifndef EACCES
  #define EACCES        1013      /**< Permission denied. */
#endif

#ifndef EBUSY
  #define EBUSY         1016      /**< Device or resource is busy */
#endif

#ifndef EXDEV
  #define EXDEV         1018      /**< Attempt to cross device. */
#endif

#ifndef ENODEV
  #define ENODEV        1019      /**< No such device. */
#endif

#ifndef ENOTDIR
  #define ENOTDIR       1020      /**< Not a directory */
#endif

#ifndef EISDIR
  #define EISDIR        1021      /**< Is a directory */
#endif

#ifndef EINVAL
  #define EINVAL        1022      /**< Invalid argument */
#endif

#ifndef EMFILE
  #define EMFILE        1024      /**< Too many open files */
#endif

#ifndef ETXTBSY
  #define ETXTBSY       1026      /**< File or directory already open */
#endif

#ifndef ENOSPC
  #define ENOSPC        1028      /**< No space left on device */
#endif

#ifndef ESPIPE
  #define ESPIPE        1029      /**< Seek is not permitted */
#endif

#ifndef ENAMETOOLONG
  #define ENAMETOOLONG  1036      /**< File name too long */
#endif

#ifndef ENOTEMPTY
  #define ENOTEMPTY     1039      /**< Directory not empty */
#endif

#ifndef ELOOP
  #define ELOOP         1040      /**< Too many symbolic links encountered */
#endif

#ifndef EILSEQ
  #define EILSEQ        1092      /**< Illegal byte sequence */
#endif

#ifndef ETIMEDOUT
  #define ETIMEDOUT     1110      /**< Operation/Connection timed out. */
#endif

#ifndef ESTALE
  #define ESTALE        1116      /**< Stale remote file handle. */
#endif

#ifndef EDQUOT
  #define EDQUOT        1122      /**< Attempt to write beyond quota */
#endif


/* Non-posix errnos used within EFS. */
#define ENOCARD        301      /**< No Media present */
#define EBADFMT        302      /**< Bad Formated Media */
#define ENOTITM        303      /**< Not an EFS2 item file. */
#define EROLLBACK      304      /**< Rollback failed while backing orig file */
#define FS_ERANGE      305      /**< Parameter out of range. */
#define ENOTHINGTOSYNC 306      /**< Nothing to Sync. EFS is not dirty. */

#define EEOF           0x8000   /**< End of file. Internal error code */

#define EUNKNOWN_SFAT  0x8001   /**< Untranslated SFAT error code. */
#define EUNKNOWN_HFAT  0x8002   /**< Untranslated HFAT error code. */

#endif /* End of _FS_ERRNO_H */
