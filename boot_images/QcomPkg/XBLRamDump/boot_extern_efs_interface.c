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
09/10/14   ck      Added boot_vefs_open
04/22/13   dh      Add boot_efs_opendir, boot_efs_closedir and boot_befs_mkdir
09/08/11   dh      Initial Creation.

===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/

#include "boot_extern_efs_interface.h"

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/ 

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
int boot_efs_open (const char *path, int oflag, ...)
{
  int result = 0;

  va_list arg;
  va_start (arg, oflag);
  
  result = boot_vefs_open(path,
                          oflag,
                          arg);
  va_end (arg);

  return result;
}


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
int boot_vefs_open(const char * path,
                   int oflag,
                   va_list arg)
{
  fs_mode_t mode = 0;
  if ((oflag & O_CREAT) != 0)
  {
    mode = va_arg (arg, int);
  }
  
  return efs_open (path, oflag, mode);
}


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
fs_off_t boot_efs_lseek (int filedes, fs_off_t offset, int whence)
{
  return efs_lseek(filedes, offset, whence);
}


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
fs_ssize_t boot_efs_read (int filedes, void *buf, fs_size_t nbyte)
{
  return efs_read(filedes, buf, nbyte);
}


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
int boot_efs_close (int filedes)
{
  return efs_close(filedes);
}


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
int boot_efs_boot_initialize (void)
{
  return efs_boot_initialize();
}


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
int boot_efs_unlink (const char *path)
{
  return efs_unlink(path);
}


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
fs_ssize_t boot_efs_write (int filedes, const void *buf, fs_size_t nbyte)
{
  return efs_write(filedes, buf, nbyte);
}


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
EFSDIR * boot_efs_opendir(const char *dirname)
{
    return efs_opendir(dirname);
}


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
int boot_efs_mkdir (const char *path, fs_mode_t mode)
{
  return efs_mkdir(path, mode);
}


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
int boot_efs_closedir (EFSDIR *dirp)
{
  return efs_closedir(dirp);
}

int boot_efs_boot_mount_first_fat(enum coldplug_device_type device_type,
	int is_embedded_device, char *mount_path)
{
    return efs_boot_mount_first_fat(device_type, is_embedded_device, mount_path);
}
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
 *   mount_path: Device node path.
 *
 * @param dirp
 *    Device node
 *
 * @return
 *    Returns 0 on success or -1 for an error.
 *
 *
 **********************************************************************/
int boot_efs_umount(char *mount_path)
{
    return efs_umount(mount_path);
}

