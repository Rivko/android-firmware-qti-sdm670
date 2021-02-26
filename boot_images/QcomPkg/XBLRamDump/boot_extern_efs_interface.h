#ifndef BOOT_EXTERN_EFS_INTERFACE_H
#define BOOT_EXTERN_EFS_INTERFACE_H
/*===========================================================================

                    BOOT EXTERN EFS DRIVER DEFINITIONS

DESCRIPTION
  Contains wrapper definition for external efs drivers

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2011-2014 by QUALCOMM Technologies Incorporated.  All Rights Reserved.
============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    
    
when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/30/14   ck      Moved to XBLRamDump and removed efs_handle_cookies
09/23/14   ck      Removed feature flag as API is always enabled in XBL
09/10/14   ck      Added boot_vefs_open
05/10/13   kedara  Added support for reading EFS files to determine the modem 
                   NAND partition
04/22/13   dh      Add boot_efs_opendir, boot_efs_closedir and boot_befs_mkdir
04/09/12   dh      Add mapping to efs_errno macro
09/08/11   dh      Initial Creation.

===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/

#include <stdarg.h>
#include "fs_public.h"

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/
/**
 *
 * The efs_errno Macro can be referred to learn more about an error when any of
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
#define boot_efs_errno  efs_errno


/*===========================================================================

**  Function :  boot_efs_open

** ==========================================================================
*/
/*!
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
int boot_efs_open (const char *path, int oflag, ...);


/*===========================================================================

**  Function :  boot_vefs_open

** ==========================================================================
*/
/*!
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
 * @param argh
 *    va_list with x number of extra parameters
 
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
 **********************************************************************/
int boot_vefs_open (const char *path, int oflag, va_list arg);


/*===========================================================================

**  Function :  boot_efs_lseek

** ==========================================================================
*/
/*!
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
fs_off_t boot_efs_lseek (int filedes,
                         fs_off_t offset,
                         int whence);


/*===========================================================================

**  Function :  boot_efs_read

** ==========================================================================
*/
/*!
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
fs_ssize_t boot_efs_read (int filedes,
                          void *buf,
                          fs_size_t nbyte);


/*===========================================================================

**  Function :  boot_efs_close

** ==========================================================================
*/
/*!
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
int boot_efs_close (int filedes);


/*===========================================================================

**  Function :  boot_efs_boot_initialize

** ==========================================================================
*/
/*!
 * 
 * Initialize the filesystem and mount romfs as the root filesystem.
 * This should only be called from boot code.
 *
 * Returns 0 on success, otherwise returns an error code (does not set
 * errno).
 **********************************************************************/
int boot_efs_boot_initialize (void);


/*===========================================================================

**  Function :  boot_efs_unlink

** ==========================================================================
*/
/*!
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
int boot_efs_unlink (const char *path);


/*===========================================================================

**  Function :  boot_efs_write

** ==========================================================================
*/
/*!
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
fs_ssize_t boot_efs_write (int filedes,
                           const void *buf,
                           fs_size_t nbyte);


/*===========================================================================

**  Function :  boot_efs_read_only_initialize

** ==========================================================================
*/
/**
 * Initialize the filesystem and mount romfs as the root filesystem.
 * This should only be called from boot code.
 *
 * @par Dependencies
 *  Must be called after DDR initialization.
 *
 * Returns 0 on success, otherwise returns an error code (does not set
 * errno).
 **********************************************************************/

int boot_efs_read_only_initialize (void);


/*===========================================================================

**  Function :  boot_efs_get

** ==========================================================================
*/
/**
 * @par Synopsis:
 *  @code
 *    #include "fs_lib.h"
 *    #include "fs_sys_types.h"
 *    int boot_efs_get (const char *path, void *data, fs_size_t length);
 *  @endcode
 *
 * @par Dependencies
 *   Must be called after boot_efs_read_only_initialize
 *
 * @par Description:
 *  Retrieve an EFS2 'special item' file. 'data' should be a pointer to a
 *  buffer, and 'length' should indicate the size of this buffer in number of
 *  bytes. This function will retrieve the item-file, and return its
 *  contents in the supplied buffer 'data'.
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
 *  Returns the number of bytes copied in the buffer, or -1 for an error.
 *
 * @par Errors:
 *  Upon error, the efs_errno will be set to one of the following:\n
 *    ::ENOENT - The item or a component of the path does not exist.\n
 *    ::FS_ERANGE - The item file is larger than the specified buffer.\n
 *    ::EINVAL - Invalid parameter.\n
 *
 * @sa
 *    efs_put
 *
 **********************************************************************/
int boot_efs_get(const char *path,
                 void *data,
                 fs_size_t length);


/*===========================================================================

**  Function :  boot_efs_stat

** ==========================================================================
*/
/**
 * @par Synopsis:
 *  @code
 *    #include "fs_public.h"
 *    int boot_efs_stat (const char *path, struct fs_stat *buf);
 *  @endcode
 *
 * @par Dependencies
 *   Must be called after boot_efs_read_only_initialize
 *
 * @par Description:
 *   boot_efs_stat returns the information about the file given its path. 
 *   boot_efs_stat and efs_lstat() are very similar, in that they both return the
 *   information about a given pathname. They differ in how they treat the
 *   last component of path. If it is a symlink, boot_efs_stat will "dereference"
 *   this symlink, returning the information about what the symlink points to.
 *   It gives the same perspective that a call to 'efs_open()' or
 *   'efs_opendir()' would give of the symlink. These are the correct semantics
 *   for most applications wanting to find out about files. The efs_lstat()
 *   call on the other hand gives the information about the symlink itself.
 *
 *   The boot_efs_stat function returns the following information in fs_stat:\n
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
int boot_efs_stat(const char *path,
                  struct fs_stat *buf);


/*===========================================================================

**  Function :  boot_efs_opendir

** ==========================================================================
*/
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
EFSDIR * boot_efs_opendir (const char *dirname);


/*===========================================================================

**  Function :  boot_efs_mkdir

** ==========================================================================
*/
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
int boot_efs_mkdir(const char *path,
                   fs_mode_t mode);


/*===========================================================================

**  Function :  boot_efs_closedir

** ==========================================================================
*/
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
int boot_efs_closedir (EFSDIR *dirp);

/*===========================================================================

**  Function :  boot_efs_boot_mount_first_fat

** ==========================================================================
*/
/*!
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
int boot_efs_boot_mount_first_fat(enum coldplug_device_type device_type,
	int is_embedded_device, char *mount_path);

/*===========================================================================

**  Function :  boot_efs_umount

** ==========================================================================
*/
/**
 * @par Name:
 *   efs_umount - unmount the device.
 *
 * @par Synopsis:
 *  @code
 *    #include "fs_public.h"
 *  @endcode
 *
 * @par Description:
 *   Umounts the mount-path after storage opertaion has been done
 *
 * @param mount_path
 *    File system mount-path at which to mount the FAT partition
 *
 * @return
 *    Returns 0 on success or -1 for an error.
 *
 *
 **********************************************************************/
int boot_efs_umount(char *mount_path);
#endif /* BOOT_EXTERN_EFS_INTERFACE_H */
