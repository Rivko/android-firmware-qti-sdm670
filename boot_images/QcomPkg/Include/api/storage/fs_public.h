/***********************************************************************
 * fs_public.h
 *
 * Public API header for EFS2.
 *
 * This file documents API calls that may be made by code external to EFS.
 *
 * The comment text found in this file is intended to be a thorough
 * and complete reference, so please report any inaccuracies or
 * confusing text as bugs.
 *
 * Copyright (C) 2002-2013,2017 QUALCOMM Technologies, Inc.
 *
 ***********************************************************************/

 /*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when          who     what, where, why
--------      ---     ------------------------------------------------------
2017-07-12    rp      Remove fs_lib.h
2013-10-29    wek     Add efs read only initialize API to public.
2013-10-10    rp      Hotplug to poll only devices of interest.
2013-07-22    rp      Add new EFS API to store data in SuperBlock.
2012-10-18    wek     Make efs_raw_put/get wrappers around efs_put/get.
2012-08-22    rp      Remove FDI.
2011-03-08    rp      Add support to lock/unlock EFS.
2010-12-09    rp      Improve documentation for the efs_sync API.
2010-07-12    rp      Added defines for major and minor number for API files.
2010-06-30    yog     Validate the input param for NULL check.
2010-05-13    wek     Make sync_no_wait non blocking.
2010-04-23    yog     Added more documentation for Truncate function.
2010-03-19    yog     Added support for files having max filesize upto 4GB.
2010-02-23    yog     Added void as arg for efs_prepare_shutdown API.
2010-02-09    wek     Added efs_prepare_shutdown API.
2010-02-08    rp      CMI compliance, remove #ifdef from API files.
2010-01-19    vr      Added efs_sync_no_wait and efs_sync_get_status APIs.
2009-12-18    vr      Added set_volume_name and get_volume_name APIs.
2009-11-04    dks     Added path argument to efs_image_prepare API
2009-08-10    wek     Fix comments for doxygen.
2009-07-06    ebb     Added efs_boot_initialize API.
2009-06-22    ebb     Added efs_sync API.
2009-05-29    wek     Re-order and document several APIs.
2009-04-03    wek     Added efs_utime() API.
2009-04-01    yg      Removed remote reset function.
2008-09-10    np      Added efs_shred function.
2008-05-23    rp      Introducing /inc folder as per new SCM.
2008-02-19    rp      Documentation: changed from @retval to @returns.
2008-01-09    yg      Improve documentation for stat functions.
2008-01-08    np      Improve header & documentation for open/close/read/write.
2007-12-21    sh      Improve header & documentation for mknod/mkfifo.
2007-12-18    rp      Improved documentation of Q&R, Put/Get-Item APIs.
2007-11-02    rp      Added extern declaration for fs_fresh_start variable.
2007-07-09    jeh     Doxygen comments markup additions, edits, etc.
2007-06-09    rp      Doxygen comments markup.
2006-12-21    s h     Implement FIFO and Socket file nodes.
2006-09-25    dlb     Clarify comments.
2006-01-26    nrs     Corrected Q&R header comments
2006-01-11    nrs     Fixed Copyright header
2005-11-28    nrs     Merge 7500 orphans
2005-11-12    sh      Use rex tcb err_num instead of global efs_errno
2005-10-28    nrs     Changed raw_put and raw_get length from int to fs_size_t
2005-10-18    nrs     Fixed comments and prototypes for Q&R
2005-08-23    sh      Added mknod.
2005-08-16    dlb     Update comments for put.
2005-08-03    dlb     Add umount support.
2005-07-20    nrs     Added implementation for quotas and reservations
2005-07-15    nrs     Added efs_chown
2005-02-15    nrs     Changed header for efs_raw_put to reflect implementation
2004-10-29    nrs     Add header to fs_lib.h for ease of use
2004-09-04    nrs     Changed headers for efs_raw_get, efs_raw_put
2004-03-23    dlb     Repair broken comments.
2003-09-09    dlb     Factory image works with bitmap and tree.
2003-05-02     gr     Added master reset functionality.
2003-03-04     gr     Renamed the parameters to efs_rename so that the C++
                      reserved word "new" is not used. Deleted duplicate
                      prototype of the efs_rename function.
2002-08-20    adm     Created.

===========================================================================*/


#ifndef __FS_PUBLIC_H__
#define __FS_PUBLIC_H__

#include "fs_sys_types.h"
#include "fs_fcntl.h"
#include "coldplug_api.h"


/** Identifies the major version number of this API file. */
#define VER_FS_PUBLIC_H_MAJOR     1
/** Identifies the minor version number of this API file. */
#define VER_FS_PUBLIC_H_MINOR     1

/** \mainpage EFS Documentation
 *
 * This documentation assumes that you are familiar with the operation of file
 * systems, as well as with the operation of AMSS software. In addition, a
 * familiarity with the Posix file system interface is helpful.
 *
 * For assistance or clarification on information in this guide, submit a
 * Service Request to QUALCOMM CDMA Technologies at
 * <A HREF="https://support.cdmatech.com/"> https://support.cdmatech.com/ </A>
 *
 * If you do not have access to the World Wide Web, you may send email to
 * <A HREF="mailto:support.cdmatech@qualcomm.com"> support </A>
 *
 * <h2>Software architecture</h2>
 *
 * @image html architect.gif "EFS software architecture"
 * @image latex architect.png "EFS software architecture" width=\textwidth
 *
 */



/**
 * @defgroup efs EFS
 *
 * The EFS module controls the default flash media on the phone and mounts
 * the root filesystem on it. It provides POSIX compliant APIs to manipulate
 * the files on this media.
 *
 */

/**
 * @defgroup efs_api EFS API functions
 * @ingroup efs
 * The EFS provides POSIX compliant APIs to manipulate the files.
 *
 * The APIs are derived from IEEE 1003.1-2001:
 *
 *   Standard for Information Technology--
 *   Portable Operating System Interface (POSIX (R))
 *
 * However, this is only a partial implementation of the filesystem aspects
 * of this standard.  Please refer to this specification for more details
 * on the operation and semantics of this type of filesystem.
 *
 * All of these functions are available under conventional operating
 * systems.  As such, we encode all of these names with an efs_ prefix.
 *
 * EFS APIs classified by category: \n
 *
 * @b File @b basic @b i/o
 * @par
 * \link ::efs_open efs_open \endlink \n
 * \link ::efs_close efs_close \endlink \n
 * \link ::efs_read efs_read \endlink \n
 * \link ::efs_write efs_write \endlink \n
 *
 * \link efs_stat File Stats functions \endlink
 * @par
 * \link ::efs_stat efs_stat \endlink \n
 * \link ::efs_fstat efs_fstat \endlink \n
 * \link ::efs_lstat efs_lstat \endlink \n
 * \link ::efs_statvfs efs_statvfs \endlink \n
 * \link ::efs_fstatvfs efs_fstatvfs \endlink \n
 *
 * @b Directory @b APIs
 * @par
 * \link ::efs_mkdir efs_mkdir \endlink \n
 * \link ::efs_rmdir efs_rmdir \endlink \n
 *
 * @b All @b file @b i/0
 * @par
 * \link ::efs_creat efs_creat \endlink \n
 * \link ::efs_mknod efs_mknod \endlink \n
 * \link ::efs_mkfifo efs_mkfifo \endlink \n
 * \link ::efs_truncate efs_truncate \endlink \n
 * \link ::efs_ftruncate efs_ftruncate \endlink \n
 * \link ::efs_truncate64 efs_truncate64 \endlink \n
 * \link ::efs_ftruncate64 efs_ftruncate64 \endlink \n
 * \link ::efs_unlink efs_unlink \endlink \n
 * \link ::efs_rename efs_rename \endlink \n
 *
 * @b File @b seek @b functions
 * @par
 * \link ::efs_lseek efs_lseek \endlink \n
 * \link ::efs_lseek64 efs_lseek64 \endlink \n
 *
 * @b Directory/File @b Iteration
 * @par
 * \link ::efs_opendir efs_opendir \endlink \n
 * \link ::efs_readdir efs_readdir \endlink \n
 * \link ::efs_closedir efs_closedir \endlink \n
 *
 * @b Misc
 * @par
 * \link ::efs_chmod efs_chmod \endlink \n
 * \link ::efs_symlink efs_symlink \endlink \n
 * \link ::efs_readlink efs_readlink \endlink \n
 * \link ::efs_raw_put efs_raw_put \endlink \n
 * \link ::efs_raw_get efs_raw_get \endlink \n
 * \link ::efs_shred efs_shred \endlink \n
 * \link ::efs_utime efs_utime \endlink \n
 * \link ::efs_sync efs_sync \endlink \n
 * \link ::efs_sync_no_wait efs_sync_no_wait \endlink \n
 * \link ::efs_sync_get_status efs_sync_get_status \endlink \n
 *
 * @b Non @b EFS
 * @par
 * \link ::efs_initialize efs_initialize \endlink \n
 * \link ::efs_power_down efs_power_down \endlink \n
 * \link ::efs_reset efs_reset \endlink \n
 *
 * @b Advanced @b API
 * @par
 * \link ::efs_image_prepare efs_image_prepare \endlink \n
 * \link ::efs_chown efs_chown \endlink \n
 * \link ::efs_set_reservation efs_set_reservation \endlink \n
 * \link ::efs_set_quota efs_set_quota \endlink \n
 * \link ::efs_get_group_info efs_get_group_info \endlink \n
 * \link ::efs_get_volume_name efs_get_volume_name \endlink \n
 * \link ::efs_set_volume_name efs_set_volume_name \endlink \n
 *
 */

/**
 * @defgroup efs_errno Error handling
 * @ingroup efs
 * \n \n
 *
 * All of the EFS APIs have a specific return value that indicates an
 * error.  For functions returning an integer value, the value '-1' is used
 * to indicate an error, and a NULL pointer is used for functions that
 * return a pointer.  When this value is returned, the global variable
 * efs_errno will be set to a value giving more specific details as to the
 * reason for this error, To interpret this  efs_errno see fs_errno.h
 */

/**
 * @defgroup efs_pathnames Pathnames
 * @ingroup efs
 *
 * EFS2 pathnames consist of null terminated strings of UTF-8 encoded
 * character data.  Directories are separated using the forward slash
 * character '/'.  Please note that EFS2 does not check the pathnames for
 * UTF-8 validity, a name component is simply terminated when a '/' or null
 * is encountered.
 *
 * Absolute pathnames begin with a slash, and relative pathnames omit the
 * initial slash.  (Since the current implementation does not allow the
 * 'current' directory to be changed, these resolve the same).  There are
 * two special names: "." which refers to the current directory, and ".."
 * which refers to the parent directory.
 *
 * An individual name component of a path (between slashes) can be no more
 * than FS_NAME_MAX characters in length.  An entire path is limited
 * FS_PATH_MAX characters.
 */


/**
 * @defgroup efs_hfat HFAT
 *
 * EFS2 supports removable media (MMC/SD, USB Mass storage) through the
 * notion of a hotplug manager.  This task keeps track of devices in the
 * system, and performs auto mounting and unmounting of the appropriate
 *
 * The removable media while available will be mounted as /mmc1, /mmc2 etc.
 * All of the EFS APIs can be called for these paths as well.
 */

/**
 * @defgroup hfat_pathnames HFAT Pathnames.
 * @ingroup efs_hfat
 *
 * HFAT pathnames and their length restrictions are driven by the FAT
 * filesystem. At the EFS API level all the filepaths are in UTF-8, but inside
 * the FAT filesystem all paths are stored in either SFN (Short File Name) or
 * LFN (Long File Name). The SFN are in MSDOS 8.3 format (8 characters for
 * filename, 3 characters for extension) and the LFN. Please consult the FAT
 * spec for more information on the SFN and LFN.
 *
 * Please see the \link unicode_pathnames \endlink section for actual size
 * limits.
 *
 */

/**
 * @defgroup efs_unicode Unicode
 *
 * All the EFS APIs take the filepath in UTF-8 format.
 *
 * In the EFS filesystem the filepath gets stored internally in the UTF-8
 * format itself, Whereas in the FAT filesystem the unicode filepath is
 * internally stored in the UTF-16 format as required by the FAT filesystem.
 *
 */

/**
 * @defgroup unicode_pathnames Unicode Pathnames.
 * @ingroup efs_unicode
 *
 * For the EFS filesystem the path lengths remains the same as explained in the
 * \link efs_pathnames \endlink section.
 *
 * For HFAT the max pathname length is 510 bytes including the terminating
 * NULL character. More specifically 255 2-byte unicode words plus 2-bytes for
 * the NULL.
 *
 * As per the FAT spec the max pathname length is 520 bytes, but we have chosen
 * 510 bytes to be compatible with OS like WinXP.
 */

/**
 * @defgroup unicode_hfat User specific UTF conversions.
 * @ingroup efs_unicode
 *
 * Users who want to have their own specific UTF conversions can do so by
 * changing fs_unicode_utf8.c, By default this file will have UTF-8 to UTF-16
 * conversions and use OEM code page 437 to generate short-file-name.
 *
 * Users can replace the following functions to have their own specific UTF
 * coding:\n
 * ::utf8_to_utf16 \n
 * ::utf8_from_utf16 \n
 * ::utf16_to_shortname \n
 * ::utf16_from_shortname \n
 *
 * Users can also replace the OEM code page 437 to their preferred code page
 * for Short-File-Names by replacing the array ::OEM_code_page
 */

 /**
  * @b fs_fresh_start
  *
  *
  * Using a debugger like JTAG, and setting this variable to a non-zero value
  * at the bootup time, will wipe out the entire EFS contents and start a
  * fresh EMPTY file-system.
  *
  * To do this from jtag, set a breakpoint at the start of efs_initialize(),
  * then set fs_fresh_start to a non-zero value, and continue.
  *
  * Warning : This variable if set to a non-zero value will BLOW-AWAY the
  * entire file-system data, so use with extreme care and precaution.
  *
  */
 extern int fs_fresh_start;



/**
 *
 * The efs_errno can be referred to learn more about an error when any of
 * the EFS APIs fail.
 *
 * @par Example
 * @code
 *    int result;
 *    result = efs_mkdir ("/efs", 0644);
 *    if (result != 0)
 *      printf ("refer fs_errno.h for reason %d", efs_errno);
 *
 * @endcode
 *
 */
#define efs_errno (*(efs_get_errno_address () ))

/**
 * Gets the pointer value of address location of efs_errno variable.
 * Users please do not call this function directly, instead use the
 * macro efs_errno
 **/
int* efs_get_errno_address (void);

/**********************************************************************
 * Public API.
 */

/**
 * Open a file in the filesystem.  If successful, the resulting file
 * descriptor will be the smallest available non-negative integer that can be
 * used as a handle to refer to this file in the calls described below.
 * Each open file descriptor maintains its own file offset and open flags.
 *
 * @param path
 *    The path of the file that needs to be opened.
 *
 * @param oflag
 *    The 'oflag' argument describes how this file should be opened, and
 *    contains one of the following values:\n
 *      O_RDONLY  - Open for reading only.\n
 *      O_WRONLY  - Open for writing only.\n
 *      O_RDWR    - Open for reading and writing.\n\n
 *    In addition, the following flags can be bitwise OR'd with this value:\n
 *      O_APPEND  - All writes will self-seek to the end of the file before
 *                  writing.\n
 *      O_CREAT   - Create the file if it does not exist.\n
 *      O_EXCL    - Fail if the file exists.\n
 *      O_TRUNC   - Truncate the file to zero bytes on successful open.\n\n
 *    These flags can be used to specify common desired ways of opening files:
 *      O_CREAT | O_TRUNC     - Normal for writing a file.  Creates if doesn't
 *                              exist.  Resulting file is zero bytes long.\n
 *      O_CREAT | O_EXCL      - Creates a file but fails if it exists.\n\n
 *
 * @param ...
 *    @b mode
 *    If O_CREAT is a part of 'oflag', a third argument
 *    (named mode) must be passed to efs_open to define the file
 *    permissions given to the newly created file. \n
 *    One or more of the following permission bits can be
 *    OR'd as the mode parameter:
 *  @par
 *    ::S_IRUSR   : Read permission for User      \n
 *    ::S_IWUSR   : Write permission for User     \n
 *    ::S_IXUSR   : Execute permission for User   \n
 *    ::S_IRGRP   : Read permission for Group     \n
 *    ::S_IWGRP   : Write permission for Group    \n
 *    ::S_IXGRP   : Execute permission for Group  \n
 *    ::S_IROTH   : Read permission for Other     \n
 *    ::S_IWOTH   : Write permission for Other    \n
 *    ::S_IXOTH   : Execute permission for Other  \n
 *    ::S_ISUID   : Set UID on execution          \n
 *    ::S_ISGID   : Set GID on execution          \n
 *    ::S_ISVTX   : Sticky bit (Hidden attrib in FAT)    \n
 *
 * @see
 *    Please see the definitions of "Posix permission bit
 *    in fs_sys_types.h. \n
 *
 * @return
 *    Returns a file descriptor >= 0 on success and -1 on failure.
 *    On failure efs_errno is set to appropriate value indicating
 *    the reason of failure.
 *
 * @par Errors:
 *   EEXIST : Cannot create a file with the pathname because another file
 *            with the same name exists and an exclusive open is requested
 *            or a special (exclusive) file with the same pathname exists.\n
 *   ENOENT : No entry for the pathname is found, the file cannot be
 *            opened (and a new file is not created because the O_CREAT
 *            flag is not supplied).\n
 *   EMFILE : Maximum number of open descriptor is exceeded.\n
 *   EISDIR : Opening a file with  a write flag (O_WRONLY or O_RDWR)
 *            denied because a directory with the same name exists.\n
 *   ENOSPC : No space left on the device.\n
 *   ENAMETOOLONG : File-name/directory name exceeded NAME_MAX limit or the
 *                  pathname exceeded PATH_MAX limit.\n
 *   ENOMEM : No more dynamic memory is available.\n
 *   ENODEV : The device does not exist.\n
 *   ENOTDIR: The file could not be created under a path that is not
 *            a directory. Another possibility is an open with the
 *            O_CREAT flag tried to create a file in the rom file-system.\n
 *   EINVAL : Invalid parameter, path.\n
 *
 * @b Example
 *
 * @code
 *    int result, i;
 *    static uint8 buffer[256]
 *    int fd = efs_open ("/file1", O_TRUNC | O_WRONLY, 0644);
 *    if (fd < 0)
 *      error_handling();
 *    for (i=0; i< 256; i++)
 *      buffer[i] = (uint8)i;
 *    result = efs_write (fd, buffer, 256);
 *    if (result != 256) {
 *      do_error_handling();
 *    }
 *    result = efs_close (fd);
 *    if (result != 256){
 *      do_error_handling();
 *      }
 *    fd = efs_open ("/file1", O_RDONLY, 0644);
 *    result = efs_read (fd, buffer, 256);
 *    if (result != 256) {
 *      do_error_handling();
 *    }
 *    result = efs_close (fd);
 *    if (result != 0){
 *      do_error_handling();
 *      }
 *
 * @endcode
 *
 **********************************************************************/
int efs_open (const char *path, int oflag, ...);

/**
 * Close the file descriptor.  The descriptor will no longer refer to any
 * file, and may be returned by subsequent calls to efs_open.
 *
 * @param filedes
 *    The file-descriptor obtained earlier via efs_open() function.
 *
 * @return
 *    Returns 0 on success and -1 on failure.
 *
 * @par Errors:
 *   EBADF : The file descriptor is not found or it cannot be closed.
 *
 * @see
 *    efs_open has an example for efs_close as well.
 **********************************************************************/
int efs_close (int filedes);

/**
 * Attempt to read 'nbytes' of data from the file associated with the
 * specified file descriptor.
 *
 * @param filedes
 *    The file-descriptor obtained earlier via efs_open() function.
 *
 * @param buf
 *    The buffer where the read bytes from the file will be stored..
 *
 * @param nbyte
 *    The number of bytes to read from the file.
 *
 * @par Errors:
 *   EBADF  : The file read is not permitted or the file descriptor is
 *            not found.\n
 *   EINVAL : Invalid parameter.\n
 *
 * @return
 *    Returns the number of bytes successfully read, or -1 for an error.\n
 *    Zero is a valid result, and generally indicates that the end of file has
 *    been reached.\n
 *    It is permitted for efs_read to return fewer bytes than were requested,
 *    even if the data is available in the file.
 *
 * @see
 *    efs_open  example section.
 **********************************************************************/
fs_ssize_t efs_read (int filedes, void *buf, fs_size_t nbyte);

/**
 * Attempt to write 'nbytes' of data to the file associated with the
 * specified file descriptor. The write ioperation may happen at the current
 * file pointer or at the end-of-file if the file is opened with O_APPEND.
 *
 * @param filedes
 *    The file-descriptor obtained earlier via efs_open() function.
 *
 * @param buf
 *    The buffer that needs to get written onto the file.
 *
 * @param nbyte
 *    The number of bytes to write to the file.
 *
 * @par Errors:
 *   ENOSPC : The write operation could not be done because the device
 *            is full.\n
 *   EBADF  : The write permission denied or the filedes is invalid.\n
 *   EINVAL : Invalid parameter.\n
 *
 * @return
 *    Returns the number of bytes successfully written or -1 for an error.\n
 *    Zero generally indicates that the filesystem is full (writing again
 *    should result in an ENOSPC error).\n
 *    It is permitted for efs_write to write fewer bytes than were requested,
 *    even if space is available.
 *
 * @see
 *    efs_open() has an example which use efs_write() as well.
 *    efs_sync() to synchronize any previous writes.
 **********************************************************************/
fs_ssize_t efs_write (int filedes, const void *buf, fs_size_t nbyte);

/**
 * Checks if the file can possibly to be accessed by the user
 * with the given mode.  The mode is a mask of the values R_OK,
 * W_OK, X_OK, and F_OK.  The first three check the appropiate
 * bits of the file.  F_OK can be used to check for the existence
 * of the file.
 *
 * @note
 *    efs_access is not currently implemented.
 *
 * @par Errors:
 *   EINVAL : errno is always set to EINVAL to note that efs_access is not
 *            implemented.\n
 **********************************************************************/
int efs_access (const char *path, int amode);

/**
 * efs_creat (path, mode) is equivalent to
 *   efs_open (path, O_WRONLY | O_CREAT | O_TRUNC, mode)
 **********************************************************************/
int efs_creat (const char *path, fs_mode_t mode);

/**
 * Create a new "special" file.
 *
 * Note that EFS assigns NO SIGNIFICANCE to these files.  It simply
 * records their existence and makes them visible during directory
 * enumeration, with the special mode bits intact.  The actual device
 * driver handling must be performed by code outside the filesystem.
 * For example, special files can not be used for read/write
 * filesystem operations.  Also, EFS does not perform the FIFO service
 * traditionally associated with \c S_IFIFO filenames.
 *
 * @param path
 *    The file path to be created.
 *
 * @param mode
 *    'mode' specifies the usual read/write/execute permission bits of the new
 *    file, as well as the type of node to be created.
 *    Please see the definitions of "Posix permission bits" in fs_sys_types.h.
 *    One of either \c S_IFBLK or \c S_IFCHR is required to be set in the
 *    mode for the creation of Block or Character device files,
 *    respectively. These device files require a Device Number, which
 *    will be stored with the file entry for use by the operating
 *    system.
 *    Special files of type FIFO and Socket are also possible by using
 *    the \c S_IFIFO or \c S_IFSOCK mode.  In these cases, the \c dev parameter
 *    is ignored, as Socket and FIFO files have no associated device
 *    number.
 *
 * @param dev
 *    This parameter specifies the 16-bit Device number to be stored
 *    with a new device node.  Traditionally, each of the Major and
 *    Minor numbers are 8-bits, with the Major number in the MSB.
 *    Since EFS assigns no meaning to this value, the actual
 *    interpretation depends on the operating system's interpretation
 *    of device nodes.  This value should be zero for files other than
 *    \c S_IFBLK or \c S_IFCHR.
 *
 * @return 0 for success, or -1 for an error.
 **********************************************************************/
int efs_mknod (const char *path, int mode, fs_devspecial_t dev);

/**
 * Create a new "FIFO" special file (sometimes called a named pipe).
 *
 * This is a simplified case of efs_mknod() that is limited to
 * creating files of type \c S_IFIFO.
 *
 * Note that EFS assigns NO SIGNIFICANCE to these files; it simply
 * records their existence and makes them visible during directory
 * enumeration.  The actual FIFO handling must be performed by handler
 * code above the filesystem.  EFS does not internally perform the
 * FIFO service traditionally associated with these files.
 *
 * @param path
 *    The file path to be created as a FIFO.
 *
 * @param mode
 *    'mode' specifies the permission bits of the new file.\n
 *    \c S_IFIFO will be assumed, and is optional in the mode argument.
 *    Please see the definitions of "Posix permission bits" in
 *    \link fs_sys_types.h \endlink
 *
 * @return 0 for success, or -1 for an error.
 **********************************************************************/
int efs_mkfifo (const char *path, int mode);

/**
 * Create a new directory.
 *
 * @param path
 *    The file path.
 *
 * @param mode
 *    'mode' specifies the permission bits of the new
 *    directory.
 *    Please see the definitions of "Posix permission bits" in
 *    \link fs_sys_types.h \endlink
 *
 * @par Errors:
 *   ENOENT : No such path was found.\n
 *   EINVAL : Invalid operation or parameter.\n
 *   ENOSPC : The operation could not be done because the device is full.\n
 *   ENAMETOOLONG : File-name/directory name exceeded NAME_MAX
 *                  limit or the pathname exceeded PATH_MAX limit.
 *
 * @b Example
 * @code
 *    int result = efs_mkdir ("/fs_test", 0755);
 *    if (result == -1)
 *      error_handling();
 * @endcode
 *
 * @return 0 for success, or -1 for an error.
 *
 * @see Please see the definitions of "Posix permission bits" in
 *    \link fs_sys_types.h \endlink
 **********************************************************************/
int efs_mkdir (const char *path, fs_mode_t mode);

/**
 * @defgroup efs_stat EFS Stat functions
 * @ingroup efs_api
 *
 * EFS2 has 3 functions for finding information out about files and other
 * entities in the filesystem: efs_stat(), efs_fstat(), and efs_lstat().
 *
 * efs_fstat() is used to get information about an opened file handle.
 *
 * efs_lstat() and efs_stat() are very similar, in that they both return
 * information about a given pathname.  They differ in how they treat the
 * last component if it is a symlink.  'efs_stat()' will "dereference" this
 * symlink, returning the information about what the symlink points to.  It
 * gives the same perspective that a call to 'efs_open()' or
 * 'efs_opendir()' would give of the symlink.  These are the correct
 * semantics for most applications wanting to find out about files.
 *
 * The efs_lstat() call on the other hand gives the information about any
 * symlinks.  This call should be used by any type of application that
 * needs to be aware of symlinks.  The most common is a type of file
 * browser that wishes to give visibility into the real file types,
 * including the presence of symlinks.
 *
 * The 'struct fs_stat' structure is defined in fs_sys_types.h.  Please see
 * the comments there for more information.
 *
 * \link ::efs_stat efs_stat \endlink \n
 * \link ::efs_fstat efs_fstat \endlink \n
 * \link ::efs_lstat efs_lstat \endlink \n
 * \link ::efs_statvfs efs_statvfs \endlink \n
 * \link ::efs_fstatvfs efs_fstatvfs \endlink \n
 *
 */

/**
 * @par Name:
 *   efs_stat  - get the information about the file by path.
 *
 * @par Synopsis:
 *  @code
 *    #include "fs_public.h"
 *    int efs_stat (const char *path, struct fs_stat *buf);
 *  @endcode
 *
 * @par Description:
 *   efs_stat returns the information about the file given its path. efs_stat
 *   and efs_lstat() are very similar, in that they both return the
 *   information about a given pathname. They differ in how they treat the
 *   last component of path. If it is a symlink, efs_stat will "dereference"
 *   this symlink, returning the information about what the symlink points to.
 *   It gives the same perspective that a call to 'efs_open()' or
 *   'efs_opendir()' would give of the symlink. These are the correct semantics
 *   for most applications wanting to find out about files. The efs_lstat()
 *   call on the other hand gives the information about the symlink itself.
 *
 *   The efs_stat function returns the following information in fs_stat:\n
 * @code
 *   struct fs_stat {
 *      fs_dev_t         st_dev;
 *      fs_inode_t       st_ino;
 *      fs_mode_t        st_mode;
 *      fs_nlink_t       st_nlink;
 *      fs_size_t        st_size;
 *      unsigned long    st_blksize;
 *      unsigned long    st_blocks;
 *      fs_time_t        st_atime;
 *      fs_time_t        st_mtime;
 *      fs_time_t        st_ctime;
 *      fs_devspecial_t  st_rdev;
 *      uint16           st_uid;
 *      uint16           st_gid;
 *    };
 * @endcode
 *
 * @par
 * - st_dev     : Unique Device ID among the mounted file systems.\n
 * - st_ino     : INode number associated with the file.\n
 * - st_mode    : Mode associated with the file.\n
 * - st_nlink   : Number of active links that are referencing this file. The
 *                space occupied by the file is released after its references
 *                reduces to 0.\n
 * - st_size    : File size in bytes.\n
 * - st_blksize : Block size, smallest allocation unit of the file system. Unit
 *                in which block count is represented.\n
 * - st_blocks  : Number of blocks allocated for this file in st_blksize
 *                units.\n
 * - st_atime   : Last access time, this is not updated, it might not have a
 *                proper value.\n
 * - st_mtime   : Last modification time, currently this would indicate the
 *                time when file was created.\n
 * - st_ctime   : Last status change time, currently this would indicate the
 *                time when file was created.\n
 * - st_rdev    : Major & Minor device number for special device files.\n
 * - st_uid     : Owner / User ID the file.\n
 * - st_gid     : Group ID of the file. The stored file data blocks are charged
 *                to the quota of this group ID.\n
 *
 *   These following macros can be used to test a given mode to determine its
 *   type, given the st_mode returned in the fs_stat structure.\n
 *
 * @par
 *   ::S_ISFIFO(m)  : Entry is a FIFO file type.\n
 *   ::S_ISCHR(m)   : Entry is a character device file.\n
 *   ::S_ISDIR(m)   : Entry is a directory.\n
 *   ::S_ISBLK(m)   : Entry is a block device file.\n
 *   ::S_ISREG(m)   : Entry is a regular file.\n
 *   ::S_ISLNK(m)   : Entry is a symbolic link.\n
 *   ::S_ISSOCK(m)  : Entry is a socket.\n
 *   ::S_ISITM(m)   : Entry is an item file.\n
 *
 *   ::S_ISSPECIAL(m) indicates if the file is one of special type of files, is
 *   ORd combination of S_ISFIFO, S_ISCHR, S_ISBLK, S_ISSOCK.
 *
 *  @code
 *    {
 *      struct fs_stat f_stat;
 *      ASSERT (efs_stat (file_path, &f_stat) == 0);
 *
 *      if (S_ISDIR(fstat.st_mode))
 *         // handle for directory...
 *      else if (S_ISLNK(fstat.st_mode))
 *         // handle for link...
 *      else if (S_ISREG(fstat.st_mode))
 *         // handle for regular files...
 *      else if (S_ISSPECIAL (fstat.st_mode))
 *         // handle for special files...
 *    }
 *  @endcode
 *
 * @param path
 *   The path is NULL terminated string indicating the path for which
 *   the stat information is needed.
 *
 * @param buf
 *   Pointer to the fs_stat structure where the return values are expected.
 *
 * @return
 *   Returns 0 on success and -1 on failure. On failure efs_errno is set to
 *   appropriate value indicating the reason of failure.
 *
 * @sa
 *   efs_lstat, efs_fstat, fs_dirent, efs_readdir
 *
 **********************************************************************/
int efs_stat (const char *path, struct fs_stat *buf);

/**
 * @par Name:
 *   efs_fstat - get the information about the file, with its open file handle.
 *
 * @par Synopsis:
 *  @code
 *    #include "fs_public.h"
 *    int efs_fstat (int fildes, struct fs_stat *buf);
 *  @endcode
 *
 * @par Description:
 *   efs_fstat() is similar to efs_stat, is used to get information about the
 *   file, but this API returns information about the file whose opened file
 *   handle is available instead of path. Refer to ::efs_statdocumentation
 *   for more information about the fs_stat structure and its related macros.
 *
 * @param fildes
 *   The valid opened file descriptor of the file for which the stat info
 *   is needed.
 *
 * @param buf
 *   Pointer to the fs_stat structure where the return values are expected.
 *
 * @return
 *   Returns 0 on success and -1 on failure. On failure efs_errno is set to
 *   appropriate value indicating the reason of failure.
 *
 * @sa
 *   efs_lstat, efs_stat
 *
 **********************************************************************/
int efs_fstat(int fildes, struct fs_stat *buf);

/**
 * @par Name:
 *   efs_lstat - get the information about the file by path.
 *
 * @par Synopsis:
 *  @code
 *    #include "fs_public.h"
 *    int efs_lstat (const char *path, struct fs_stat *buf);
 *  @endcode
 *
 * @par Description:
 *   efs_lstat() and efs_stat() are very similar, in that they both return
 *   information about a given pathname. They differ in how they treat the
 *   last component if it is a symlink. 'efs_stat()' will "dereference" this
 *   symlink, returning the information about what the symlink points to.
 *   The efs_lstat() call on the other hand gives the information about
 *   any symlinks. This call should be used by any type of application
 *   that needs to be aware of symlinks. The most common is a type of file
 *   browser that wishes to give visibility into the real file types,
 *   including the presence of symlinks. Refer to ::efs_stat documentation
 *   for more information about the fs_stat structure and its related macros.
 *
 * @param path
 *   The path is NULL terminated string indicating the path for which
 *   the stat information is needed.
 *
 * @param buf
 *   Pointer to the fs_stat structure where the return values are expected.
 *
 * @return
 *   Returns 0 on success and -1 on failure. On failure efs_errno is set to
 *   appropriate value indicating the reason of failure.
 *
 * @sa
 *   efs_stat, efs_fstat, fs_dirent, efs_readdir
 *
 **********************************************************************/
int efs_lstat (const char *path, struct fs_stat *buf);

/**
 * @par Name:
 *   efs_statvfs - Obtain information about an entire filesystem.
 *
 * @par Synopsis:
 *  @code
 *    #include "fs_public.h"
 *    #include "fs_sys_types.h"
 *    int efs_statvfs (const char *path, struct fs_statvfs *buf);
 *  @endcode
 *
 * @par Description:
 *   Get detailed information about the filesystem specified by the path.
 *   Root or any mounted path can be given to get information. If root
 *   path is given, information about the root file system is returned,
 *   otherwise information about the mounted filesystem specified by this
 *   path or the filesystem, in which the given path exists is returned.
 *   The information is returned in the structure fs_statvfs. The content
 *   details are as given below.
 *
 *   @code
 *   struct fs_statvfs {
 *      unsigned long      f_bsize;
 *      fs_fsblkcnt_t      f_blocks;
 *      fs_fsblkcnt_t      f_bfree;
 *      fs_fsblkcnt_t      f_bavail;
 *      fs_fsfilcnt_t      f_files;
 *      fs_fsfilcnt_t      f_ffree;
 *      fs_fsfilcnt_t      f_favail;
 *      unsigned long      f_fsid;
 *      unsigned long      f_flag;
 *      unsigned long      f_namemax;
 *      unsigned long      f_maxwrite;
 *      fs_fsblkcnt_t      f_balloc;
 *      fs_fsblkcnt_t      f_hardalloc;
 *   };
 *   @endcode
 *
 * - f_bsize    : Fundamental file system block size. Minimum allocation in
 *                 file system happen at this size.\n
 * - f_blocks   : Maximum possible number of blocks available in the whole
 *                 file system.\n
 * - f_bfree    : Number of free blocks available currently.\n
 * - f_bavail   : Number of free blocks available currently.\n
 * - f_fsid     : File system ID, this varies depending on the implementation
 *                 of file system. Currently EFS=0, HFAT=1.\n
 * - f_namemax  : Maximum length of the name part of the string for a file,
 *                 directory or symlink.\n
 * - f_maxwrite : Maximum number of bytes that can be written in a single
 *                 write call.\n
 *
 *   Currently these following fields don't contain any useful information,
 *   so ignore these fields.\n
 *
 * - f_files  :\n
 * - f_ffree  :\n
 * - f_favail :\n
 * - f_flag   :\n
 * - f_balloc :\n
 * - f_hardalloc :\n
 *
 * @param path
 *   The path is a valid path of a file or directory on the mounted filesystem
 *
 * @param buf
 *   buf is the pointer to fs_statvfs structure where the return stat values
 *   of the filesystem are expected.
 *
 * @return
 *   Returns 0 on success and -1 on failure. On failure efs_errno is set to
 *   appropriate value indicating the reason of failure.
 *
 * @sa
 *   efs_stat, efs_fstatvfs.
 **********************************************************************/
int efs_statvfs (const char *path, struct fs_statvfs *buf);

/**
 * @par Name:
 *   efs_fstatvfs - Obtain information about an entire filesystem.
 *
 * @par Synopsis:
 *  @code
 *    #include "fs_public.h"
 *    #include "fs_sys_types.h"
 *    int efs_fstatvfs(int fildes, struct fs_statvfs *buf);
 *  @endcode
 *
 * @par Description:
 *   Get detailed information about the filesystem on which the opened file
 *   is present, whose handle is available. The information is returned in
 *   the structure fs_statvfs the content details are as given below.
 *
 * @param fildes
 *   fildes is the opened handle for the file which is residing on the
 *   mounted filesystem for which the stat info is needed.
 *
 * @param buf
 *   buf is the pointer to fs_statvfs structure where the return stat values
 *   of the filesystem are expected.
 *
 * @return
 *   Returns 0 on success and -1 on failure. On failure efs_errno is set to
 *   appropriate value indicating the reason of failure.
 *
 * @sa
 *   efs_fstat, efs_statvfs.
 **********************************************************************/
int efs_fstatvfs(int fildes, struct fs_statvfs *buf);


/**
 * efs_lseek
 *
 * Change the file offset for the opened file descriptor.
 *
 * Changing the file offset does not modify the file.  If you lseek past
 * the end of the file, and then write, the gap will be filled with zero
 * bytes.  This gap may not actually allocate space.
 *
 * Using this efs_lseek() API file can be seeked upto (2GB -1) offset, if
 * needed to seek to offsets upto (4GB -1) length then use efs_lseek64() API.
 *
 * @param filedes
 *    The file-descriptor of an open file obtained from efs_open() function.
 *
 * @param offset
 *    The new offset of the file.
 *
 * @param whence
 *    'whence' indicates how the new offset is computed:
 *      SEEK_SET - Set to 'offset'.
 *      SEEK_CUR - Set to 'offset' + current position.
 *      SEEK_END - Set to 'offset' + file size.\n
 *
 * @par Errors:
 *   EINVAL : Invalid operation.
 *   EBADF  : File descriptor not found.
 *   ESPIPE : Some file descriptors (like pipes, fifos) are not seek-able.
 *
 * @b Example
 *
 * @code
 *    int result, i;
 *    static uint8 buffer[256]
 *    int fd = efs_open ("/file1", O_TRUNC | O_WRONLY, 0644);
 *    if (fd < 0)
 *      error_handling();
 *    for (i=0; i< 256; i++)
 *      buffer[i] = (uint8)i;
 *    result = efs_write (fd, buffer, 256);
 *    if (result != 256) {
 *      do_error_handling();
 *    }
 *    result = efs_close (fd);
 *    if (result != 0){
 *      do_error_handling();
 *      }
 *    fd = efs_open ("/file1", O_RDONLY, 0644); result =
 *    efs_lseek (fd, 128, SEEK_SET);
 *    if (result != 128)
 *      do_error_handling();
 *    result = efs_read (fd, buffer, 128);
 *    if (result != 128) {
 *      do_error_handling();
 *    }
 *    for (i=128; i<256; i++)
 *       assert (buffer[i-128] == (uint8) i);
 *
 *    result = efs_close (fd);
 *
 *    if (result != 0){
 *      do_error_handling();
 *      }
 *
 * @endcode
 *
 * @return
 *    Upon success, lseek returns the resulting offset, as bytes from the
 *    beginning of the file.  Otherwise (off_t) -1 is returned.
 **********************************************************************/
fs_off_t efs_lseek (int filedes, fs_off_t offset, int whence);

/**
 * efs_lseek64
 *
 * Similiar to efs_lseek(). Moves the file offset for the opened file
 * descriptor.\n
 * Main difference between efs_lseek() and efs_lseek64() is that with
 * efs_lseek64() file offset can be changed beyond 2GB. The upper limit
 * of offset for efs_lseek64() is (4GB-1) bytes.\n
 * Parameter offset lies in range
 *           (-4 * 1024 * 1024 * 1024, +4 * 1024 * 1024 * 1024 -1)
 *
 * Changing the file offset does not modify the file.  If you lseek past
 * the end of the file, and then write, the gap will be filled with zero
 * bytes. This gap may not actually allocate space.
 *
 * @param filedes
 *    The file-descriptor of an open file obtained from efs_open() function.
 *
 * @param offset
 *    The new offset of the file. Offset is of data type fs_off64_t, hence
 *    upto 4GB limit offset can be supported.\n
 *    Offset lies in range
 *           (-4 * 1024 * 1024 * 1024, +4 * 1024 * 1024 * 1024 -1)
 *
 * @param whence
 *    'whence' indicates how the new offset is computed:
 *      SEEK_SET - Set to 'offset'.
 *      SEEK_CUR - Set to 'offset' + current position.
 *      SEEK_END - Set to 'offset' + file size.\n
 *
 * @par Errors:
 *   EINVAL : Invalid operation.
 *   EBADF  : File descriptor not found.
 *   ESPIPE : Some file descriptors (like pipes, fifos) are not seek-able.
 *
 * @b Example
 *
 * @code
 *    int result;
 *    static uint8 buffer[65535];
 *    struct fs_stat stat;
 *    long long write_bytes;
 *
 *    int fd = efs_open ("/mmc1/file1", O_TRUNC | O_WRONLY, 0644);
 *    if (fd < 0)
 *      error_handling();
 *
 *    // Write 3GB data in open file 'fd'
 *    write_bytes = 0xC0000000ULL;
 *    do {
 *      result = efs_write (fd, buffer, write_bytes);
 *
 *      if (result != 0) {
 *        do_error_handling();
 *      }
 *
 *      write_bytes -= result;
 *    }while (write_bytes > 0 && result != 0);
 *
 *    result = efs_close (fd);
 *    if (result != 0) {
 *      do_error_handling();
 *    }
 *
 *    fd = efs_open ("/mmc1/file1", O_RDONLY, 0644);
 *    result = efs_stat ("/mmc1/file1", &stat);
 *    if (result != 0) {
 *      do_error_handling();
 *    }
 *
 *    // SEEK to the 2.5GB offset position from the starting file_pos.
 *    result = efs_lseek64 (fd, 0xA0000000ULL, SEEK_SET);
 *    if (result != OxA0000000ULL) {
 *       do_error_handling();
 *    }
 *
 *    result = efs_close (fd);
 *    if (result != 0){
 *      do_error_handling();
 *    }
 *
 * @endcode
 *
 * @return
 *    Upon success, lseek64 returns the resulting offset, as bytes from the
 *    beginning of the file.  Otherwise (fs_off64_t) -1 is returned.
 *
 * @see
 *    efs_open(), efs_close(), efs_stat(), efs_write() for example.\n
 *    efs_lseek()
 **********************************************************************/
fs_off64_t efs_lseek64 (int filedes, fs_off64_t offset, int whence);


/**
 * Truncate a file to a specified length.  The file will be extended, or
 * shrunk to the newly specified size.  On extension, the filesystem does
 * not actually allocate space to the file, until data is written. When
 * shortening a file, the data past the new end is lost.
 *
 * Using this efs_truncate() API file can be truncated upto (2GB -1) length,
 * if needed to truncate upto (4GB -1) length then use efs_truncate64() API.
 *
 * @param path
 *    The path of the file whose length has to be changed.
 *
 * @param length
 *    The new size of the file.
 *
 *    If the supplied length is greater than the current file-size, then it
 *    depends upon the underlying filesystem to determine if the file can grow
 *    in size or not. If the file is an EFS2 filesystem file, then the file
 *    will grow in size to the newly specified length. If the file is an
 *    HFAT filesystem file, then the file will not grow in size and truncate
 *    call will result in an error with efs_errno being set to EEOF.
 *
 * @par Errors:
 *   EINVAL : Truncation is not possible. Invalid operation or parameter.\n
 *   ENOENT : A File with the path is not found.\n
 *   ENODEV : The device does not exist.\n
 *   ENAMETOOLONG : File-name/directory name exceeded NAME_MAX
 *                  limit or the pathname exceeded PATH_MAX limit.\n
 *   EEOF : On HFAT filesystem not allow to truncate the file beyond EOF.
 *
 * @return
 *    Returns 0 on success, or -1 for error.
 **********************************************************************/
fs_off_t efs_truncate (const char *path, fs_off_t length);

/**
 * Truncate a file to a specified length.
 * Main difference between efs_truncate64() and efs_truncate() is that with
 * efs_truncate64() file can also be truncated for the length greater then 2GB.
 * The upper limit of length for efs_truncate64() is (4GB-1) bytes.\n
 *
 * @param path
 *    The path of the file whose length has to be changed.
 *
 * @param length
 *    The new size of the file.\n
 *    Length lies in range
 *           (-4 * 1024 * 1024 * 1024, +4 * 1024 * 1024 * 1024 -1)
 *
 *    If the supplied length is greater than the current file-size, then it
 *    depends upon the underlying filesystem to determine if the file can grow
 *    in size or not. If the file is an EFS2 filesystem file, then the file
 *    will grow in size to the newly specified length. If the file is an
 *    HFAT filesystem file, then the file will not grow in size and truncate
 *    call will result in an error with efs_errno being set to EEOF.
 *
 *
 * @par Errors:
 *   EINVAL : Truncation is not possible. Invalid operation or parameter.\n
 *   ENOENT : A File with the path is not found.\n
 *   ENODEV : The device does not exist.\n
 *   ENAMETOOLONG : File-name/directory name exceeded NAME_MAX
 *                  limit or the pathname exceeded PATH_MAX limit.\n
 *   EEOF : On HFAT filesystem not allow to truncate the file beyond EOF.
 *
 * @return
 *    Returns 0 on success, or -1 for error.
 *
 * @see
 *    efs_truncate(), efs_ftruncate64().
 **********************************************************************/
fs_off64_t efs_truncate64 (const char *path, fs_off64_t length);

/**
 * Truncate an open file.  Similar to efs_truncate.  The file must be
 * opened for writing.
 *
 * Using this efs_ftruncate() API file can be truncated upto (2GB -1) length,
 * if needed to truncate upto (4GB -1) length then use efs_ftruncate64() API.
 *
 * @param fildes
 *    The file-descriptor of an open file.
 *
 * @param length
 *    The new size of the file.
 *
 * @par Errors:
 *   EINVAL : Truncation is not possible. Invalid operation.\n
 *   EBADF  : A File with the file descriptor is not found.\n
 *   ENODEV : The device does not exist.\n
 *   ENAMETOOLONG : File-name/directory name exceeded NAME_MAX
 *                  limit or the pathname exceeded PATH_MAX limit.\n
 *   EEOF : On HFAT filesystem not allow to truncate the file beyond EOF.
 *
 * @return
 *    Returns 0 on success, or -1 for error.
 **********************************************************************/
fs_off_t efs_ftruncate (int fildes, fs_off_t length);

/**
 * Truncate a open file to a specified length, similiar to efs_truncate64.
 * Main difference between efs_ftruncate64() and efs_ftruncate() is that
 * with efs_ftruncate64() file can also be truncated for the length greater
 * then 2GB.
 * The upper limit of length for efs_ftruncate64() is (4GB-1) bytes.\n
 *
 * @param fildes
 *    The file-descriptor of an open file.
 *
 * @param length
 *    The new size of the file.\n
 *    Length lies in range
 *           (-4 * 1024 * 1024 * 1024, +4 * 1024 * 1024 * 1024 -1)
 *
 * @par Errors:
 *   EINVAL : Truncation is not possible. Invalid operation.\n
 *   EBADF  : A File with the file descriptor is not found.\n
 *   ENODEV : The device does not exist.\n
 *   ENAMETOOLONG : File-name/directory name exceeded NAME_MAX
 *                  limit or the pathname exceeded PATH_MAX limit.\n
 *   EEOF : On HFAT filesystem not allow to truncate the file beyond EOF.
 *
 * @return
 *    Returns 0 on success, or -1 for error.
 *
 * @see
 *    efs_truncate64(), efs_ftruncate().
 **********************************************************************/
fs_off64_t efs_ftruncate64 (int fildes, fs_off64_t length);

/**
 * Delete a name in the filesystem.  If this is the only reference to this
 * file, the file itself will also be deleted.  efs_unlink can be used on
 * all filesystem objects, except for directories.  Use ::efs_rmdir for
 * directories.
 *
 * @param path
 *    The file that needs to be deleted.
 *
 * @par Errors:
 *   ENOENT  : No such path was found.\n
 *   EPERM   : permission denied.\n
 *   ETXTBSY : The file is open or used. So it cannot be unlinked.\n
 *   ENAMETOOLONG : File-name/directory name exceeded NAME_MAX
 *                  limit or the pathname exceeded PATH_MAX limit.\n
 *   EINVAL  : Invalid parameter.\n
 *
 * @return
 *    Returns 0 for success, or -1 for error.
 *
 * @sa
 *    efs_sync
 **********************************************************************/
int efs_unlink (const char *path);

/**
 * @par Name:
 *   efs_opendir - open a directory for iteration.
 *
 * @par Synopsis:
 *  @code
 *      #include "fs_public.h"
 *      EFSDIR *efs_opendir (const char *dirname);
 *  @endcode
 *
 * @par Description:
 *    Open a directory for iteration.  The pointer returned is an opaque
 *    handle that can be later passed to efs_readdir to iterate through
 *    the entries of the opened directory. This same pointer must be passed
 *    to closedir to close the iterator.\n
 *
 *  Note that there are only a small number of directory iterators available
 *  in EFS2.
 *
 * @param dirname
 *   The dirname is the base directory path under which to iterate.
 *
 * @return
 *   Returns valid directory iterator pointer on successfully opening the
 *   directory for iteration, otherwise NULL on error.
 *
 * @sa
 *   efs_readdir, efs_closedir
 *
 **********************************************************************/
EFSDIR *efs_opendir (const char *dirname);

/**
 * @par Name:
 *   efs_readdir - read next entry from the opened directory iterator
 *
 * @par Synopsis:
 *  @code
 *    #include "fs_public.h"
 *    struct fs_dirent *efs_readdir (EFSDIR *dirp);
 *  @endcode
 *
 * @par Description:
 *    Read next available entry from the opened directory iterator. If
 *    an entry is available then the fs_dirent structure filled with
 *    details about enumerated entry is returned. If no other entries
 *    are found then NULL is returned.
 *
 *  @code
 *    struct fs_dirent {
 *      char           d_name[FS_NAME_MAX+1];
 *      struct fs_stat d_stat;
 *      uint32         d_stats_present;
 *    };
 *  @endcode
 *
 *  @par
 *    Posix requires that this structure contain at least 'd_name', but it is
 *    allowed to contain more. Some filesystems have more information
 *    available, whereas others make that kind of lookup expensive. We add
 *    two fields here, d_stat is a regular stat structure. 'd_stats_present"
 *    is a bitmask of the above masks for a given implementation to define
 *    which fields it has filled in.
 *
 *  @par
 *    Note that any code that uses readdir must be prepared for
 *    d_stats_present to be zero, and to call ::efs_stat for each entry. These
 *    values are generally only implemented for filesystem types where the
 *    information is readily available alongwith the name, or getting it
 *    through ::efs_stat is very inefficient.
 *
 *    Bit masks for d_stats_present are defined as:
 *
 *  @code
 *    ::FS_DIRENT_HAS_ST_DEV       = (1 << 1)
 *    ::FS_DIRENT_HAS_ST_INO       = (1 << 2)
 *    ::FS_DIRENT_HAS_ST_MODE      = (1 << 3)
 *    ::FS_DIRENT_HAS_ST_NLINK     = (1 << 4)
 *    ::FS_DIRENT_HAS_ST_SIZE      = (1 << 5)
 *    ::FS_DIRENT_HAS_ST_BLKSIZE   = (1 << 6)
 *    ::FS_DIRENT_HAS_ST_BLOCKS    = (1 << 7)
 *    ::FS_DIRENT_HAS_ST_ATIME     = (1 << 8)
 *    ::FS_DIRENT_HAS_ST_MTIME     = (1 << 9)
 *    ::FS_DIRENT_HAS_ST_CTIME     = (1 << 10)
 *    ::FS_DIRENT_HAS_ST_RDEV      = (1 << 11)
 *    ::FS_DIRENT_HAS_ST_UID       = (1 << 12)
 *    ::FS_DIRENT_HAS_ST_GID       = (1 << 13)
 *    ::FS_DIRENT_HAS_TYPE         = (1 << 14)
 *  @endcode
 *
 *    A macro has been created to check these flags:\n
 *    FS_MASK_CHECK(value, required)\n
 *    where value is d_stats_present from the returned ::fs_dirent structure
 *    and required the flag in question that we are interested in.
 *
 *  @code
 *    {
 *      struct fs_dirent *dirent;
 *      struct fs_stat f_stat;
 *      uint32 fmode;
 *      EFSDIR * dirp = efs_opendir (path);
 *
 *      while ((dirent = efs_readdir (dirp)) != 0)
 *      {
 *        // Check if mode is returned, otherwise call stat to get mode
 *        if (FS_MASK_CHECK (dirent->d_stats_present, FS_DIRENT_HAS_ST_MODE))
 *          fmode = dirent->d_stat.st_mode
 *        else
 *        {
 *          ASSERT (efs_stat (file_path, &f_stat) == 0);
 *          fmode = f_stat.st_mode;
 *        }
 *
 *        if (S_ISDIR(fmode))
 *          // handle for directory...
 *        else if (S_ISLNK(fmode))
 *          // handle for link...
 *        else if (S_ISREG(fmode))
 *          // handle for regular files...
 *        else if (S_ISSPECIAL (fmode))
 *          // handle for special files...
 *      }
 *      efs_closedir (dirp);
 *    }
 *  @endcode
 *
 * @param dirp
 *    The iterator handle obtained earlier from the ::efs_opendir() function.
 *
 * @return
 *    On success, returns a pointer to a struct ::fs_dirent, which contains
 *    a field d_name which is the name of this directory component.  Returns
 *    a NULL pointer on error or end of file.
 *
 * @sa
 *   efs_opendir, efs_closedir
 *
 **********************************************************************/
struct fs_dirent *efs_readdir (EFSDIR *dirp);

/**
 * @par Name:
 *   efs_closedir - close the directory iterator.
 *
 * @par Synopsis:
 *  @code
 *    #include "fs_public.h"
 *    int efs_closedir (EFSDIR *dirp);
 *  @endcode
 *
 * @par Description:
 *   Close the directory iterator. This releases the iterator for reuse.
 *
 * @param dirp
 *    The iterator handle obtained earlier from the ::efs_opendir() function.
 *
 * @return
 *    Returns 0 on success or -1 for an error.
 *
 * @sa
 *   efs_opendir, efs_readdir
 *
 **********************************************************************/
int efs_closedir (EFSDIR *dirp);

/**
 * Renames a file. Files and directories (under the same file-system) can
 * be renamed. The arguments 'oldpath' and 'newpath' do not have to be in
 * the same directory (but must be on the same filesystem device).
 *
 * @param oldpath pathname before the rename operation.
 * @param newpath pathname after the rename operation.
 *
 * @par Errors:
 *    EINVAL : Invalid operation denied. The reasons can be possible
 *             permission access violation or a creation of a cycle
 *             symbolic links if the rename suceeded.
 *    EISDIR : The newpath is a directory.
 *    EXDEV  : A rename operation accross different file systems is
 *             not permitted.
 *    ENOTEMPTY : The oldpath is a directory which is not
 *                  empty.
 *
 * @b Example
 *
 * @code
 *    int result, i;
 *    static uint8 buffer[256]
 *    int fd = efs_open ("/file1", O_TRUNC | O_WRONLY, 0644);
 *    if (fd < 0)
 *      error_handling();
 *    for (i=0; i< 256; i++)
 *      buffer[i] = (uint8)i;
 *    result = efs_write (fd, buffer, 256);
 *    if (result != 256) {
 *      do_error_handling();
 *    }
 *    result = efs_close (fd);
 *    if (result != 0){
 *      do_error_handling();
 *      }
 *    result = efs_rename ("/file1", "/file2");
 *
 *    if (result != 0){
 *      do_error_handling();
 *      }
 *
 * @endcode
 *
 * @return
 *    Returns 0 on success, or -1 on error.
 *
 * @note
 *    efs_rename is atomic, and will either successfully rename the file, or
 *    leave the file in its original location.
 *
 * @sa
 *    efs_sync
 **********************************************************************/
int efs_rename (const char *oldpath, const char *newpath);

/**
 * Remove a directory.  Only empty directories can be removed.
 *
 * @param path
 *    The file the needs to be removed.
 *
 * @par Errors:
 *   ENOTDIR   : The parameter path is not a directory.\n
 *   ENOTEMPTY : The directory is not empty.\n
 *   ETXTBSY   : The directory is used or open.\n
 *   EINVAL    : Invalid parameter.\n
 *
 * @return
 *    Returns 0 on success or -1 on error.
 **********************************************************************/
int efs_rmdir (const char *path);

/**
 * @par Name:
 *  efs_chmod - Change the mode of a specified file or directory.
 *
 * @par Synopsis:
 *  @code
 *    #include "fs_public.h"
 *    #include "fs_sys_types.h"
 *    int efs_chmod (const char *path, fs_mode_t mode);
 *  @endcode
 *
 * @par Description:
 *  The efs_chmod() function will change the file permission bits, ::S_ISUID,
 *  ::S_ISGID and ::S_ISVTX bits of the file/dir given by the 'path' argument
 *  to the corresponding bits in the 'mode' argument.
 *
 * @param path
 *  The path whose mode bits has to be changed. If the file-path is a symlink,
 *  then the symlink will be resolved and the mode-bits of the actual
 *  file that this symlink points to will be changed.
 *
 * @param mode
 *  The desired mode bits, only the permission/suid/sticky bits can be set.
 *  These mode bits are the least 12 significant bits.
 *
 * @return
 *  Returns 0 on success or -1 on error. Upon error refer efs_errno.
 *
 * @par Errors:
 *  Upon error, the efs_errno will be set to\n
 *    ::ENOENT - The path does not exist.\n
 *    ::EINVAL - If the mode bit has more bits set other than the 12 least-
 *               significant bits.\n
 *
 * @sa
 *    efs_chmod_recursive, efs_chown
 *
 **********************************************************************/
int efs_chmod (const char *path, fs_mode_t mode);

/**
 * @par Name:
 *  efs_sync - Synchronize filesystem to the backing store.
 *
 * @par Synopsis:
 *  @code
 *    #include "fs_public.h"
 *    int efs_sync (const char *path);
 *  @endcode
 *
 * @par Description:
 *  By calling efs_sync the filesystem guarantees upon return that all
 *  cached content will be committed to the underlying filesystem.
 *  Depending on the underlying filesystem the sync call may not be
 *  supported or even necessary. In these cases sync will just return
 *  success. Calling efs_sync is a costly operation, it should only be
 *  called when the application needs to guarantee the data has been
 *  committed to permanent storage.
 *
 *  This API can take up to 4 seconds to return, so callers of this API
 *  have to be prepared that their task can be blocked for this duration.
 *
 * @param path
 *  The path is a valid path of a file or directory on the mounted
 *  filesystem. To sync the root filesystem a path of "/" must be
 *  passed.
 *
 * @return
 *  Returns 0 on success or -1 on error.
 *
 * @par Errors:
 *  Upon error, the efs_errno will be set to one of the following:\n
 *  ::ENOENT  - The directory specified by the path does not exist\n
 *  ::ENODEV  - Device does not exist or not available.
 *  ::EINVAL  - Invalid parameter.\n
 *
 * @sa
 *    efs_sync_no_wait, efs_sync_get_status
 *
 **********************************************************************/
int efs_sync (const char *path);

/**
 * @par Name:
 *  efs_sync_no_wait - Synchronize filesystem to the backing store, Start the
 *  sync process and return immediately without blocking the caller.
 *
 * @par Synopsis:
 *  @code
 *    #include "fs_public.h"
 *    int efs_sync_no_wait (const char *path, fs_sync_token_t *sync_token);
 *  @endcode
 *
 * @par Description:
 *  By calling efs_sync_no_wait the filesystem will begin the process
 *  of committing cached content to the underlying filesystem.
 *  Unlike the efs_sync() API, the efs_sync_no_wait() does not block the
 *  caller for the entire sync duration. The efs_sync_no_wait() will
 *  start the sync and will immediately return to the caller, and upon return
 *  a "sync_token" will be provided to the caller, the caller can now or later
 *  use this "sync_token" to learn about the status of this particular sync
 *  operation by calling the efs_sync_get_status() API.
 *
 *  This API will just queue a efs_sync request and return, but the actual
 *  efs_sync() that will be peformed later to fulfil this request can take
 *  up to 4 seconds.
 *
 *  Depending on the underlying filesystem the sync call may not be
 *  supported or even necessary. In these cases sync will just return
 *  success.
 *
 * @param path
 *  The path is a valid path of a file or directory on the mounted
 *  filesystem. To sync the root filesystem a path of "/" must be
 *  passed.
 *
 * @param sync_token
 *  Pointer to the sync_token, on success of efs_sync_no_wait() API,
 *  a "sync_token" will be issued in this parameter. The caller can now or
 *  later use this "sync_token" to learn about the status of the sync
 *  by calling the efs_sync_get_status() API.
 *
 * @return
 *  Returns 0 on successfully issuing sync call.
 *  Returns -1 in case of error.
 *
 * @par Errors:
 *  Upon error, the efs_errno will be set to one of the following:\n
 *  ::ENOENT  - The directory specified by the path does not exist\n
 *  ::ENODEV  - Device does not exist or not available.\n
 *  ::EINVAL  - Invalid parameter.\n
 *
 * @sa
 *    efs_sync, efs_sync_get_status
 *
 **********************************************************************/
int efs_sync_no_wait (const char *path, fs_sync_token_t *sync_token);

/**
 * @par Name:
 *  efs_sync_get_status - Returns status of an sync operation started by an
 *  previously issued efs_sync_no_wait() API.
 *
 * @par Synopsis:
 *  @code
 *    #include "fs_public.h"
 *    int efs_sync_get_status (const char *path,
 *        fs_sync_token_t sync_token, enum fs_sync_result *sync_result);
 *  @endcode
 *
 * @par Description:
 *  efs_sync_get_status() API can be used to learn about the status of an sync
 *  that was commenced by an previous efs_sync_no_wait() call.
 *
 *
 * @param path
 *  The path is a valid path of a file or directory on the mounted
 *  filesystem.
 *
 * @param sync_token
 *  The sync_token that was issued in the efs_sync_no_wait() call.
 *
 * @param fs_sync_result
 *  Pointer to the fs_sync_result, on success this parameter will contain the
 *  result of the sync operation. Possible results are FS_SYNC_PENDING if sync
 *  is pending and FS_SYNC_COMPLETE if sync has completed.
 *
 * @return
 *  Returns 0 on success or -1 in case of error.
 *
 * @par Errors:
 *  Upon error, the efs_errno will be set to one of the following:\n
 *  ::ENOENT  - The directory specified by the path does not exist\n
 *  ::EPERM   - Operation not permitted\n
 *  ::EINVAL  - Invalid parameter.\n
 *
 * @sa
 *    efs_sync_no_wait, efs_sync
 *
 **********************************************************************/
int efs_sync_get_status (const char *path, fs_sync_token_t sync_token,
                         enum fs_sync_result *sync_result);

/**
 * This define is present when efs_sync API is available
 */
#define EFS_HAVE_SYNC

/**
 * @par Name:
 *  efs_symlink - make a new name for a file
 *
 * @par Synopsis:
 *  @code
 *    #include "fs_public.h"
 *    int efs_symlink (const char *oldpath, const char *newpath);
 *  @endcode
 *
 * @par Description:
 *  Create a symlink to the 'oldpath'.  Future references to 'newpath' act as
 *  if 'oldpath' were substituted for the name component of the path.  If
 *  'oldpath' is a relative path, it is interpreted relative to the directory
 *  'newpath' resides in.
 *
 * @param oldpath
 *  The path for which we have to create a new name. The oldpath should not
 *  contain any symlink.
 *
 * @param newpath
 *  The name name for the oldpath. This new name should not exceed
 *  FS_MAX_SYMLINK_TARGET bytes.
 *
 * @return
 *  Returns 0 on success or -1 on error.
 *
 * @par Errors:
 *  Upon error, the efs_errno will be set to one of the following:\n
 *  ::ENOENT  - The file specified by the oldpath does not exist\n
 *  ::ENOSPC  - There is not enough space to create the symlink\n
 *  ::ENAMETOOLONG  - The newpath exceeds FS_MAX_SYMLINK_TARGET bytes.\n
 *  ::EINVAL  - Invalid parameter.\n
 *
 * @sa
 *  efs_readlink
 *
 **********************************************************************/
int efs_symlink (const char *oldpath, const char *newpath);


/**
 * @par Name:
 *  efs_readlink - Read the value of a symlink.
 *
 * @par Synopsis:
 *  @code
 *    #include "fs_public.h"
 *    #include "fs_sys_types.h"
 *    int efs_readlink (const char *path, char *buf, fs_size_t bufsiz);
 *  @endcode
 *
 * @par Description:
 *  Read the value of a symlink.  Up to 'bufsiz' bytes of data will be
 *  copied into 'buf'.  Note that the result is not null terminated.
 *  Returns the number of bytes placed into 'buf', or '-1' on error.
 *
 * @param path
 *  The path of the symlink.
 *
 * @param buf
 *  The destination buffer where the path that symlink points to will be
 *  copied. Note that the buffer will not be null-terminated.
 *
 * @param bufsiz
 *  The size of the destination buffer buf.
 *
 * @return
 *  Returns the number of bytes placed into 'buf', or '-1' on error.
 *
 * @par Errors:
 *  Upon error, the efs_errno will be set to one of the following:\n
 *    ::ENOENT  - The file specified by the path does not exist\n
 *    ::ENAMETOOLONG  - If the supplied buf is too small to hold the link.\n
 *    ::EINVAL  - Invalid parameter.\n
 *
 * @sa
 *  efs_symlink
 *
 **********************************************************************/
int efs_readlink (const char *path, char *buf, fs_size_t bufsiz);

/**
 * @par Name:
 *  efs_raw_put - Store a value in an EFS2 'special item' file.
 *
 * @par Synopsis:
 *  @code
 *    #include "fs_public.h"
 *    #include "fs_sys_types.h"
 *    int efs_raw_put (const char *path, void *data, fs_size_t length,
 *                     int oflag, int mode);
 *  @endcode
 *
 * @deprecated
 *  This function will call efs_put() directly, for full description see
 *  efs_put(). This API is deprecated and provided for legacy reasons, new
 *  applications should use efs_put() instead.
 *
 * @param path
 *  The path of the item-file.
 *
 * @param data
 *  The start-address of the data.
 *
 * @param length
 *  The length indicates the size of the item-file and it should not exceed
 *  FS_ITEM_MAX bytes.
 *
 * @param oflag
 *  See efs_put() for complete list of 'oflag' values.
 *
 * @param mode
 *  'mode' will set the initial permission bits of the item file.
 *
 * @return
 *  See efs_put().
 *
 * @par Errors:
 *  See efs_put().
 *
 * @sa
 *  efs_raw_get
 *
 **********************************************************************/
int efs_raw_put (const char *path, void *data, fs_size_t length,
    int oflag, int mode);

/**
 * @par Name:
 *  efs_raw_get - Retrieve an EFS2 'special item' file.  See 'efs_put' for a
 *  description of these files.
 *
 * @par Synopsis:
 *  @code
 *    #include "fs_public.h"
 *    #include "fs_sys_types.h"
 *    int efs_raw_get (const char *path, void *data, fs_size_t length);
 *  @endcode
 *
 * @deprecated
 *  This function will call efs_get() directly, for full description see
 *  efs_get(). This API is deprecated and provided for legacy reasons, new
 *  applications should use efs_get() instead.
 *
 * @param path
 *  The path of the item-file.
 *
 * @param data
 *  The start-address of the data.
 *
 * @param length
 *  The length indicates the size of the buffer 'data' in bytes.
 *
 * @return
 *  See efs_get().
 *
 * @par Errors:
 *  See efs_get().
 *
 * @sa
 *  efs_raw_put
 *
 **********************************************************************/
int efs_raw_get (const char *path, void *data, fs_size_t length);

/**
 * @par Name:
 * efs_utime - change access and modification time for a given file.
 *
 * @par Synopsis:
 *  @code
 *    #include "fs_public.h"
 *    #include "fs_sys_types.h"
 *    int efs_utime (const char *path, const struct fs_utimbuf times);
 *  @endcode
 *
 * @par Description:
 *  Sets the access and modification time of an entry located at path.
 *  It is currently implemented only for EFS and FAT file systems.
 *
 *  The times argument is a pointer to fs_utimbuf structure that holds
 *  the new access and modification time of the entry. If the times
 *  argument is NULL the access and modification time of the file will
 *  be set to the current system time. The current system time
 *  depends if the device time has synchronized to the network. For
 *  this reason there are cases where the system time is not reliable.
 *  Some examples is working with an device with no RF connection or
 *  a freshly booted device.
 *
 *  The change time field of the file (ctime) will always be updated
 *  to the current system time.
 *
 *  FAT stores access and modification time differently than other
 *  file systems. It stores access date (not time) and modification
 *  time to the two second precision. Values supplied to set this fields
 *  will be rounded down to the nearest value. For example, if we set
 *  access and modification time to March 18th 2009 13:32:59, this API
 *  will set access time as March 18th 2009 00:00:00 and modification
 *  time as March 18th 2009 13:32:58.
 *
 * @param path
 *  The path whose access and modification time has to be changed. If
 *  the path is a symlink, then the symlink will be resolved and the
 *  access and modification time of the actual entry that this symlink
 *  points to will be changed.
 *
 * @param times
 *  Pointer to structure fs_utimbuf. The member actime specifies the new
 *  access time, and the member modtime specifies the new modification
 *  time.
 *
 * @return
 *  Returns 0 on success or -1 on error. Upon error refer efs_errno.
 *
 * @par Errors:
 *  Upon error, the efs_errno will be set to one of the following:\n
 *    ::ENOENT - a directory component of 'path' does not exist.\n
 *    ::ELOOP  - A symlink named in 'path' contains a loop.\n
 *    ::ENAMETOOLONG - The 'path' specified, or the data is too long.\n
 *    ::EINVAL - Invalid parameter.\n
 *
 * @b Example
 *
 * @code
 *    struct utimbuf times;
 *    times.actime = NEW_ACCESS_TIME;
 *    times.modtime = NEW_MODIFICATION_TIME;
 *    if (efs_utimbuf ("/file1", &times))
 *       error_handling ();
 *    file_time_attributes_updated();
 *
 * @endcode
 *
 **********************************************************************/
int efs_utime (const char *path, const struct fs_utimbuf *times);

/** Defined when the API efs_utime() is available. */
#define EFS_HAVE_UTIME

/**
 * @par Name:
 *  efs_chown - Change the Owner ID or Group ID of the specified file or
 *  directory.
 *
 * @par Synopsis:
 *  @code
 *    #include "fs_public.h"
 *    int efs_chown (const char *path, int uid_val, int gid_val);
 *  @endcode
 *
 * @par Description:
 *  The file or directory is specified by path and must exist prior to the
 *  call to efs_chown. The two parameters that this function takes in
 *  addition to the path are the owner ID and the group ID. If a value
 *  is not being changed, then the parameter should be set to -1.
 *
 * @param path
 *  The file or directory path whose user-id or group-id has to be changed.
 *
 * @param uid_val
 *  The new user-id, or -1 if you don't want to change.
 *
 * @param gid_val
 *  The new group-id, or -1 if you don't want to change.
 *
 * @return
 *  Returns 0 on success or -1 on error.
 *
 * @par Errors:
 *  Upon error, the efs_errno will be set to one of the following:\n
 *    ::ENOENT  - The file specified by the path does not exist\n
 *    ::ELOOP  - A symlink named in 'path' contains a loop.\n
 *    ::ENAMETOOLONG - The 'path' specified, or the data is too long.\n
 *    ::EACCES - The path is to the root directory\n
 *    ::EINVAL  - The group id parameter value is greater than the allowed
 *                number of groups\n
 *    ::ENOTDIR - One of the components of the path is not a directory\n
 *    ::ENOSPC  - There is not enough space to move the file to a new group\n
 *    ::EDQUOT  - Changing the group id will result in the new group being
 *               over the quota, therefore the operation failed.\n
 *
 * @sa
 *   efs_chown_recursive, efs_chmod
 *
 **********************************************************************/
int efs_chown (const char *path, int uid_val, int gid_val);

/**
 * @par Name:
 *  efs_set_reservation - Set the reservation value for a given group.
 *
 * @par Synopsis:
 *  @code
 *    #include "fs_public.h"
 *    #include "fs_sys_types.h"
 *    int efs_set_reservation (const char *path, uint32 groupid,
                               fs_size_t size);
 *  @endcode
 *
 * @par Description:
 *  A Reservation is a space reserved prior to use in order to guarantee
 *  availability for use later. This API can be used to reserve such space.
 *  As files are written to the file system and to the group which reserved
 *  the space, blocks will be reduced from the reservation available value.
 *  If the reservation available value is reduced to 0, EFS will then
 *  attempt to continue the write using available space from the free pool.
 *  When a file is deleted from the directory, the size of that file will be
 *  returned to the reservation available as long as the reservation is not
 *  currently exceeded.
 *  Any meta-data for inodes or database leaf nodes created under the
 *  directory will be charged to the general pool first; if no space is
 *  available, then to the reservation, therefore any reservation size will
 *  be an approximation of the space reserved. So it is a wise idea to
 *  reserve a few more blocks than necessary to ensure the proper
 *  reservation of space.
 *  A sample usage scenario of Reservation would be that an app which wants
 *  to download an  image wants to ensure space for the image before it
 *  starts the download.
 *
 * @param path
 *  The path where to set aside the reservation space.
 *
 * @param groupid
 *  This is the ID value for groups of files. A group ID allows files and
 *  directories to be grouped together for purpose of the reservation and
 *  quota system. The concept of groups is to create a ruleset to be applied
 *  to various number of files and/or directories. Every directory and
 *  regular file inherits the GID of its parent.
 *
 * @param size
 *  The size parameter should be passed in as the number of bytes. Be aware
 *  however that this function will convert the size to the number of pages as
 *  that is the smallest unit of storage in EFS. Therefore if the byte size
 *  is not divisible by the page size then an additional page will be set
 *  aside for the reservation.
 *
 * @return
 *  Returns 0 on success or -1 on error. Upon error refer efs_errno.
 *
 * @par Errors:
 *  Upon error, the efs_errno will be set to one of the following:\n
 *    ::ENOENT - a directory component of 'path' does not exist.\n
 *    ::ELOOP  - A symlink named in 'path' contains a loop.\n
 *    ::ENAMETOOLONG - The 'path' specified, or the data is too long.\n
 *    ::EPERM - The group id parameter value is out of the the allowed
 *              number of groups\n
 *    ::ENOSPC - Unable to reserve the space desired\n
 *    ::EINVAL - Invalid parameter.\n
 * @sa
 *   efs_set_quota, efs_chown
 *
 **********************************************************************/
int efs_set_reservation (const char *path, uint32 groupid, fs_size_t size);

/**
 * @par Name:
 *  efs_set_quota - Set the quota value for a given group.
 *
 * @par Synopsis:
 *  @code
 *    #include "fs_public.h"
 *    #include "fs_sys_types.h"
 *    int efs_set_quota (const char *path, uint32 groupid, fs_size_t size);
 *  @endcode
 *
 * @par Description:
 *  A Quota is a limit set to prevent overuse of the file system by an
 *  assigned group. Quotas are a self policing mechanism, used to ensure
 *  that a group or application doesn't inadvertently hog the file system or
 *  wants to limit itself based on the available space for performance
 *  issues. This API can be used to set such a quota restriction for a group
 *
 * @param path
 *  The path on which to set the quota restriction.
 *
 * @param groupid
 *  This is the ID value for groups of files. A group ID allows files and
 *  directories to be clumped together for purpose of the reservation and
 *  quota system. The concept of groups is to create a ruleset to be applied
 *  to a various number of files and/or directories. Every directory and
 *  regular file inherits the GID of its parent. groupid 0 is reserved and
 *  cannot be used, EFS will return a permission denied error.
 *
 * @param size
 *  The size parameter should be passed in as the number of bytes. Be aware
 *  however that this function will convert the size to the number of pages as
 *  that is the smallest unit to be stored in EFS. Therefore if the byte size
 *  is not divisible by the EFS Page size, then an additional page will be set
 *  aside for the quota.
 *
 * @return
 *  Returns 0 on success or -1 on error. Upon error refer efs_errno.
 *
 * @par Errors:
 *  Upon error, the efs_errno will be set to one of the following:\n
 *    ::ENOENT - a directory component of 'path' does not exist.\n
 *    ::ELOOP  - A symlink named in 'path' contains a loop.\n
 *    ::ENAMETOOLONG - The 'path' specified, or the data is too long.\n
 *    ::EPERM - The group id parameter value is out of the the allowed
 *              number of groups\n
 *    ::EINVAL - Invalid parameter.\n
 * @sa
 *  efs_set_reservation, efs_chown
 *
 **********************************************************************/
int efs_set_quota (const char *path, uint32 groupid, fs_size_t size);

/**
 * @par Name:
 *  efs_get_group_info - Get the quota and reservation information for the
 *  given group id.
 *
 * @par Synopsis:
 *  @code
 *    #include "fs_public.h"
 *    #include "fs_sys_types.h"
 *    int efs_get_group_info (const char *path, uint32 groupid,
 *                            struct fs_group_info *ginfo);
 *  @endcode
 *
 * @par Description:
 *  This function will return the group information of a given group id. The
 *  information will be stored in a group_info structure. Please refer to
 *  the fs_group_info structure for details on the meanings of the fields.
 *
 * @param path
 *  The path whose group info to retrieve.
 *
 * @param groupid
 *  This is the ID value for groups of files. A group ID allows files and
 *  directories to be clumped together for purpose of the reservation and
 *  quota system. The concept of groups is to create a ruleset to be applied
 *  to a various number of files and/or directories. Every directory and
 *  regular file inherits the GID of its parent. groupid 0 is reserved and
 *  cannot be used, EFS will return a permission denied error.
 *
 * @param ginfo
 *  The return value, on success this struct will be populated with how much
 *  was the Quota and Reservation values set aside for this groupid and of
 *  which how much has been consumed till now.
 *
 * @return
 *  Returns 0 on success or -1 on error. Upon error refer efs_errno.
 *
 * @par Errors:
 *   Upon error, the efs_errno will be set to one of the following:\n
 *    ::ENOENT - a directory component of 'path' does not exist.\n
 *    ::ELOOP  - A symlink named in 'path' contains a loop.\n
 *    ::ENAMETOOLONG - The 'path' specified, or the data is too long.\n
 *    ::EPERM  - The group id parameter value is out of the the allowed
 *                number of groups\n
 *    ::EINVAL - Invalid parameter.\n
 * @sa
 *  efs_set_reservation, efs_set_quota, efs_chown
 *
 **********************************************************************/
int efs_get_group_info (const char *path, uint32 groupid,
    struct fs_group_info *ginfo);

/**
 * @par Name:
 *  efs_shred - Erases the (old) obsolete data residing
 *  in the device.
 *
 * @par Synopsis:
 *  @code
 *    #include "fs_public.h"
 *    #include "fs_sys_types.h"
 *    int efs_shred ();
 *  @endcode
 *
 * @par Description:
 *  When a file is erased or truncated some of its data content may still
 *  reside in the physical device. The function 'efs_shred' provides a way to
 *  erase obsolete data residing in the device. This function returns
 *  1 if the shredding job is not complete. It is designed to do partial
 *  shred on each call so that the task doesn't consume huge amount of
 *  CPU time to complete shredding the whole device, thus causing watchdog
 *  error if the caller task doesn't get chance to report to dog within its
 *  configured time. After shredding of all the obsolete data as of when
 *  efs_shred called first, it would return 0. If a file modification call is
 *  issued after the first efs_shred call, data becoming obsolete by that
 *  call may reside in the device. Shred is only supported for the NAND
 *  devices only. Shred is an expensive operation and it is intended to be
 *  used when refurbishing the device in the service center or factory.
 *  Shred does not erase obsolete file or directory names.
 *
 * @param path
 *  The path is a valid path of a file or directory on the mounted file system
 *  that needs to be shredded.
 *
 * @b Example
 *
 * @code
 *    int result;
 *    while (1)
 *    {
 *      result = efs_shred ("/");
 *      if (result == 0)
 *        break; // Shred is complete.
 *      else if (result == -1)
 *        // Shred is not a valid command.
 *        break;
 *      // When shred returns a positive number, partial shred is done.
 *      // We should sleep to prevent watchdog expiration and issue another
 *      // shred call until efs_shred returns 0 or shred is complete.
 *      sleep ();
 *    }
 * @endcode
 *
 * @return
 *  Returns 0 on completion and a positive number on partial
 *  completion. It returns -1 if shred cannot be done.
 *
 **********************************************************************/
int
efs_shred (const char *path);

/**
 * @par Name:
 *   efs_mount - Mount a file system at a given directory.
 *
 * @par Synopsis:
 *  @code
 *    #include "fs_public.h"
 *    int  efs_mount (const char *special, const char *dir,
 *                    const char *fstype, int flags, const void *data);
 *  @endcode
 *
 * @par Description:
 *  Mount a file system at a given directory.  Unlike Posix, the name (fstype)
 *  given is a parsed string describing the source of the file system.
 *
 * @param special
 *  Currently not used.
 *
 * @param dir
 *  Directory where to mount the file system.
 *
 * @param fstype
 *  File system type, e.g. EFS2, nodev, extfs (for FAT).
 *
 * @param flags
 *  Currently not used, should be zero. Future purpose flags.
 *
 * @param data
 *  Pointer to where the file system data resides.
 *
 * @return
 *  Returns 0 for success, -1 on error.
 *
 * @par Errors:
 *   Upon error, the efs_errno will be set to one of the following:\n
 *    ::ENOENT - a directory component of 'path' does not exist.\n
 *    ::ELOOP  - A symlink named in 'path' contains a loop.\n
 *    ::ENAMETOOLONG - The 'path' specified, or the data is too long.\n
 *    ::EMFILE - Mount Table Full.\n
 *    ::ENODEV - Invalid file system type.\n
 *    ::EINVAL - Invalid parameter.\n
 *
 * @sa
 *  efs_mount, efs_remount
 *
 */
int efs_mount (const char *special, const char *dir,
    const char *fstype, int flags, const void *data);

/**
 * @par Name:
 *   efs_umount - Remove a mount file system at a given directory.
 *
 * @par Synopsis:
 *  @code
 *    #include "fs_public.h"
 *    int efs_umount (const char *target);
 *  @endcode
 *
 * @par Description:
 *  Unmounts a previously mounted file system. Unlike Posix, the umount is
 *  performed, even if the file system is busy. All open handles will result
 *  in ENODEV being returned for any access attempt.
 *
 * @param target
 *  Path where the file system is mounted.
 *
 * @return
 *  Returns 0 for success, -1 on error.
 *
 * @par Errors:
 *   Upon error, the efs_errno will be set to one of the following:\n
 *    ::ENOENT - a directory component of 'path' does not exist.\n
 *    ::ELOOP  - A symlink named in 'path' contains a loop.\n
 *    ::ENAMETOOLONG - The 'path' specified, or the data is too long.\n
 *    ::EINVAL - Not a valid mount point.\n
 *
 * @sa
 *  efs_umount, efs_remount
 *
 */

int efs_umount (const char *target);

/**
 * @par Name:
 *   efs_remount - Relocates where an existing mount point is in the file
 *                 system.
 *
 * @par Synopsis:
 *  @code
 *    #include "fs_public.h"
 *    int efs_remount (const char *oldtarget, const char *newtarget);
 *  @endcode
 *
 * @par Description:
 *  Cannot be used (yet) to remount root, but allows existing mount points
 *  to be moved. Opened handles will continue to work, although pathnames
 *  used to access them will change. This is similar behavior to what would
 *  be seen with directory renaming. Returns EINVAL if oldtarget isn't an
 *  existing active mount. The newtarget directory must exist.
 *
 *  The results are undefined if newtarget is a subdirectory of oldtarget.
 *
 * @param oldtarget
 *  Old path of the file system.
 *
 * @param newtarget
 *  New path for the file system.
 *
 * @return
 *  Returns 0 for success, -1 on error.
 *
 * @par Errors:
 *   Upon error, the efs_errno will be set to one of the following:\n
 *    ::ENOENT - a directory component of 'path' does not exist.\n
 *    ::ELOOP  - A symlink named in 'path' contains a loop.\n
 *    ::ENAMETOOLONG - The 'path' specified, or the data is too long.\n
 *    ::EINVAL - Not a valid mount point.\n
 *
 * @sa
 *  efs_umount, efs_mount
 *
 */
int efs_remount (const char *oldtarget, const char *newtarget);

int efs_image_prepare (const char *path);

/**
 * @par Name:
 *  efs_get_volume_name - Retrieves the volume name for a filesystem.
 *
 * @par Synopsis:
 *  @code
 *    #include "fs_public.h"
 *    int efs_get_volume_name (const char *path, char *label, int len);
 *  @endcode
 *
 * @par Description:
 *  Each filesystem that supports volume names can have a label associated
 *  with it. This API can be used to retrieve this label.
 *
 * @param path
 *  The path where the filesystem is mounted.
 *
 * @param label
 *  This is the buffer where the volume name is returned to the caller. The
 *  caller is required to allocate this buffer.
 *
 * @param len
 *  This is the length parameter for the label buffer that was allocated.
 *  The maximum length is 10. The buffer allocated is not used beyond
 *  this limit.
 *
 * @return
 *  Returns 0 on success or -1 on error. Upon error refer efs_errno.
 *
 * @par Errors:
 *  Upon error, the efs_errno will be set to one of the following:\n
 *    ::ENOENT - a directory component of 'path' does not exist.\n
 *    ::ELOOP  - A symlink named in 'path' contains a loop.\n
 *    ::ENAMETOOLONG - The 'path' specified is too long.\n
 *    ::EINVAL - Volume name not supported. Invalid operation.\n
 *    ::ENODEV - The device does not exist.\n
 *
 * @sa
 *  efs_set_volume_name
 *
 **********************************************************************/
int efs_get_volume_name (const char *path, char *label, int len);

/**
 * @par Name:
 *  efs_set_volume_name - Sets the volume name for a filesystem.
 *
 * @par Synopsis:
 *  @code
 *    #include "fs_public.h"
 *    int efs_set_volume_name (const char *path, char *label);
 *  @endcode
 *
 * @par Description:
 *  Each filesystem that supports volume names can have a label associated
 *  with it. This API can be used to set this label.
 *
 * @param path
 *  The path where the filesystem is mounted.
 *
 * @param label
 *  This is the buffer where the volume name is passed by the caller. The
 *  caller allocates and sets the desired label. Maximium limit of
 *  a permitted label is 10. Any data beyond this limit is automatically
 *  truncated. Only upper-case characters are permitted. Lower case characters
 *  are automatically converted to uppercase.
 *
 * @return
 *  Returns 0 on success or -1 on error. Upon error refer efs_errno.
 *
 * @par Errors:
 *  Upon error, the efs_errno will be set to one of the following:\n
 *    ::ENOENT - a directory component of 'path' does not exist.\n
 *    ::ELOOP  - A symlink named in 'path' contains a loop.\n
 *    ::ENAMETOOLONG - The 'path' specified, or the data is too long.\n
 *    ::EINVAL - Volume name not supported. Invalid operation.\n
 *    ::ENODEV - The device does not exist.\n
 *
 * @sa
 *  efs_get_volume_name
 *
 **********************************************************************/
int efs_set_volume_name (const char *path, char *label);


/**
 *
 * Please NOTE :
 * This API will lock the filesystem and locking the filesystem can have
 * system wide impact as it will block some time sensitive EFS clients for the
 * locked duration. So the usage of this API has to be carefully studied on
 * a scenario by scenario basis. Please don't use this API if you are not sure
 * or if a system wide study has not been done for your scenario.
 *
 * @par Name:
 *  efs_lock_filesystem - Lock the access to the filesystem.
 *
 * @par Synopsis:
 *  @code
 *    #include "fs_public.h"
 *    int efs_lock_filesystem (const char *path);
 *  @endcode
 *
 * @par Description:
 *  Lock the file system. This API will block all future EFS clients from
 *  accessing the filesystem until the filesystem gets unlocked by the
 *  efs_unlock_filesystem API.
 *
 *  The lock/unlock APIs must be called by the same task; i.e., the task that
 *  calls the efs_lock_filesystem API must be the same task that calls
 *  efs_unlock_filesystem API as well.
 *
 *  When the filesystem gets unlocked, the task with the highest scheduling
 *  priority of all the waiting tasks gets unblocked first and will be the
 *  first one to get the EFS services.
 *
 *  When using the API please be sure to:
 *    - Not keep the filesystem locked for more than 1 second.
 *    - Unlock the filesystem using the same task that locked it.
 *
 *  When using the API it is recommendated to:
 *    - Put the phone in offline mode (Airplane mode). This reduces the number
 *        of active EFS clients.
 *
 * @param path
 *    Which filesytem to lock. Currently meant only for "/".
 *
 * @return
 *    Returns 0 on success and -1 on failure. Upon failure the efs_errno
 *    will be set to the exact fail reason.
 *
 * @par Errors:
 *   ::EINVAL - Invalid argument.\n
 *   ::EPERM  - The file system does not allow locking.\n
 *
 * @sa
 *  efs_unlock_filesystem
 *
 **********************************************************************/
int efs_lock_filesystem (const char *path);


/**
 *
 * Please NOTE :
 * This API is used to unlock the filesystem that was locked using the
 * efs_lock_filesystem API. Locking the filesystem can have system wide
 * impact as it will block some time sensitive EFS clients for the locked
 * duration. So the usage of this API has to be carefully studied on a
 * scenario by scenario basis. Please don't use this API if you are not
 * sure or if a system wide study has not been done for your scenario.
 *
 * @par Name:
 *  efs_unlock_filesystem - Unlock the access to the filesystem.
 *
 * @par Synopsis:
 *  @code
 *    #include "fs_public.h"
 *    int efs_unlock_filesystem (const char *path);
 *  @endcode
 *
 * @par Description:
 *  Unlock the file system. This API will unblock access to the filesystem
 *  which was blocked using the efs_lock_filesystem API.
 *
 *  The lock/unlock APIs must be called by the same task; i.e., the task that
 *  calls the efs_lock_filesystem API must be the same task that calls
 *  efs_unlock_filesystem API as well.
 *
 *  When the filesystem gets unlocked, the task with the highest scheduling
 *  priority of all the waiting tasks gets unblocked first and will be the
 *  first one to get the EFS services.
 *
 *  When using the API please be sure to:
 *    - Not keep the filesystem locked for more than 1 second.
 *    - Unlock the filesystem using the same task that locked it.
 *
 * @param path
 *    Which filesytem to unlock. Currently meant only for "/".
 *
 * @return
 *    Returns 0 on success and -1 on failure. Upon failure the efs_errno
 *    will be set to the exact fail reason.
 *
 * @par Errors:
 *   ::EINVAL - Invalid argument.\n
 *   ::EPERM  - The file system does not allow locking.\n
 *
 * @sa
 *  efs_lock_filesystem
 *
 **********************************************************************/

int efs_unlock_filesystem (const char *path);


/**********************************************************************
 **********************************************************************
 * End of common public interface.
 *
 * The routines below are used to initialize and configure the filesystem.
 **********************************************************************
 *********************************************************************/

/**
 * efs_initialize
 *
 * Initialize the filesystem and mount the root filesystem.
 *
 * Returns 0 on success, otherwise returns an error code (does not set
 * errno).
 **********************************************************************/
int efs_initialize (void);

/**
 * efs_boot_initialize
 *
 * Initialize the filesystem and mount romfs as the root filesystem.
 * This should only be called from boot code exactly once.
 *
 * Also mount the first FAT partition of the external SD-card as "/mmc1".
 *
 * Returns 0 on success, otherwise returns an error code (does not set
 * errno).
 **********************************************************************/
int efs_boot_initialize (void);

/**
 * efs_boot_int_romfs
 *
 * Initialize the filesystem and mount romfs as the root filesystem.
 * This should only be called from boot code.
 *
 * Unlike efs_boot_initialize() API, this API does not mount any
 * external FAT partition as "/mmc1".
 *
 * It is okay to call this API multiple times and calling again will
 * immediately return success.
 *
 * Returns 0 on success, otherwise returns an error code (does not set
 * errno).
 **********************************************************************/
int efs_boot_init_romfs (void);

/**
 * efs_read_only_initialize
 *
 * Initialize EFS on NAND filesystem on read only mode. This is expected to
 * be called only in boot to reduce the code space associated with writes.
 *
 * This API expect that is is called only once during the init process.
 *
 * Returns 0 on success, otherwise returns an error code (does not set
 * errno).
 **********************************************************************/
int efs_read_only_initialize (void);

/**
 * efs_power_down
 *
 * Place the filesystem in a state where it is safe to remove power.
 **********************************************************************/
int efs_power_down (void);

/**
 * Reset the filesystem and stop the system. The mobile must be
 * rebooted before it can be used again.
 *
 * This function never returns.
 */
void efs_reset (void);

/**
 * Same as efs_reset, except doesn't call FS_ERR_FATAL.  Should only be
 * used from within the FS_ERR_FATAL handler.
 */
void efs_reset_nostop (void);

/**
 * Does cleanup before shutting down the File System.
 *
 * This should be called only when shutdown or reset is going to occur.
 * It will do all the housekeeping and cleanup needed to guarantee the
 * data is safely stored in the device. This is only critical when using
 * the remote storage feature.
 *
 * @return
 *  Returns 0 on success or -1 on error. Upon error refer efs_errno.
 */
int efs_prepare_shutdown (void);

/**
 * @par Name:
 *  efs_store_data_in_superblock - Store given data in SuperBlock.
 *
 * @par Description:
 *  Store the supplied data onto the SuperBlock. This API is not supported
 *  on all Page-Managers and currently is supported only for Remote-Storage
 *  Page-Manager. Page-Managers that don't support this API will return
 *  EPERM error code.
 *
 * @param path
 *  The path is a valid path of a file or directory on the mounted
 *  filesystem.
 *
 * @param data
 *  The data will be stored in the superblock as is.
 *
 * @param data_size
 *  Size of the data supplied in the previous argument.
 *
 * @return
 *  Returns 0 on success or -1 on error.
 *
 * @par Errors:
 *  Upon error, the efs_errno will be set to one of the following:\n
 *  ::EPERM   - Storing data in SuperBlock is not supported.
 *  ::EINVAL  - When any input argument to this API is not correct.
 *  ::ENOSPC  - When the supplied data is too big to fit in superblock.
 *
 **********************************************************************/
int efs_store_data_in_superblock (const char *path, const void *data,
                                  uint32 data_size);

/*************************************************************************//**
 * @par Name:
 *   efs_boot_mount_first_fat - Mounts the first FAT partition in the given
 *   device at the given mount-path.
 *
 * @par Description:
 *   Mounts the first FAT partition for the given device type at the supplied
 *   mount-path.
 *
 * @param dev_type
 *   Coldplug device type (eg: eMMC or UFS). See enum coldplug_device_type for
 *   the complete list of all the coldplug devices.
 *
 * @param is_embedded_device
 *   Is it an internal device (hardwired) or an external device (pluggable)?
 *   1 -> Internal device
 *   0 -> External device
 *
 * @param mount_path
 *   File system mount-path at which to mount the FAT partition.
 *
 * @return:
 *   Upon success the COLDPLUG_RESULT_SUCCESS will be returned otherwise
 *   an appropriate error code will be returned. For a complete list of
 *   all the return values see enum ::coldplug_result.
*****************************************************************************/
int efs_boot_mount_first_fat(enum coldplug_device_type device_type,
    int is_embedded_device, const char *mount_path);

#endif /* End of __FS_PUBLIC_H__ */
