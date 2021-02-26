
/***********************************************************************
 * fs_fcntl.h
 *
 * Posix interface for Embedded File System.
 * Copyright (C) 2002-2004, 2007-2008, 2010 QUALCOMM Technologies, Inc.
 *
 * This file declares the file contol related definitions for
 * POSIX interface
 *
 ***********************************************************************/

 /*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when          who     what, where, why
--------      ---     ------------------------------------------------------
2010-07-12    rp      Added defines for major and minor number for API files.
2008-05-23    rp      Introducing /inc folder as per new SCM.
2008-04-21    s h     Reserved open mode for SFS use.
2007-06-09    rp      Doxygen comments markup.
2004-10-29    nrs     Added O_AUTODIR flag as option for creating files
2004-10-15    dlb     Update copyright line.
2002-08-20    adm     Created.

===========================================================================*/


#ifndef _FS_FCNTL_H
#define _FS_FCNTL_H

/** Identifies the major version number of this API file. */
#define VER_FS_FCNTL_H_MAJOR     1
/** Identifies the minor version number of this API file. */
#define VER_FS_FCNTL_H_MINOR     1

#define O_ACCMODE          0003
#define O_RDONLY             00
#define O_WRONLY             01
#define O_RDWR               02
#define O_CREAT            0100 /**< not fcntl */
#define O_EXCL             0200 /**< not fcntl */
#define O_NOCTTY           0400 /**< not fcntl */
#define O_TRUNC           01000 /**< not fcntl */
#define O_APPEND          02000
#define O_NONBLOCK        04000
#define O_NDELAY        O_NONBLOCK
#define O_SYNC           010000
#define FASYNC           020000 /**< fcntl, for BSD compatibility */
#define O_DIRECT         040000 /**< direct disk access hint */
#define O_LARGEFILE     0100000
#define O_DIRECTORY     0200000 /**< must be a directory */
#define O_NOFOLLOW      0400000 /**< don't follow links */
#define O_ITEMFILE     01000000 /**< Create special ITEM file. */
#define O_AUTODIR      02000000 /**< Allow auto-creation of directories */
#define O_SFS_RSVD     04000000 /**< Reserved for Secure FS internal use */

# define SEEK_SET       0       /**< Seek from beginning of file.  */
# define SEEK_CUR       1       /**< Seek from current position.  */
# define SEEK_END       2       /**< Seek from end of file.  */

#define EFS_IOFBF       0       /**< Full buffering.    */
#define EFS_IOLBF       1       /**< Line buffering.    */
#define EFS_IONBF       2       /**< No buffering.      */

#endif /* End of _FS_FCNTL_H */
