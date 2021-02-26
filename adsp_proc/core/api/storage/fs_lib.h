/***********************************************************************
 * fs_lib.h
 *
 * Library API for EFS2.
 * Copyright (C) 2002-2010, 2012, 2013 Qualcomm Technologies, Inc.
 *
 ***********************************************************************/

 /*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.qdsp6/2.1/api/storage/fs_lib.h#1 $ $DateTime: 2017/07/21 22:10:47 $ $Author: pwbldsvc $

when          who     what, where, why
--------      ---     ------------------------------------------------------
2013-02-12    dks     Fix efs_get API documentation.
2012-12-14    wek     Remove references to raw put in efs_put documentation.
2012-10-22    wek     Corrected error on efs_put oflags.
2010-07-12    rp      Added defines for major and minor number for API files.
2010-06-30    yog     Validate the input param for NULL check.
2009-08-10    wek     Fix comments for doxygen.
2009-05-15    sh      Corrected types for MD5 functions.
2009-03-30    wek     Update documentation in chmod and chown recursive.
2008-05-23    rp      Introducing /inc folder as per new SCM.
2008-02-22    np      Add md5sum calculation support.
2008-02-19    rp      Documentation: changed from @retval to @returns.
2008-01-09    yg      Documentation update for efs_deltree.
2007-12-18    rp      Improved documentation of put/get items.
2007-11-16    rp      Added efs_chown_recursive() and efs_chmod_recursive().
2007-06-09    rp      Doxygen comments markup.
2006-09-25    umr     Added efs_deltree() for deleting a whole tree.
2005-11-12     sh     Use rex tcb err_num instead of global efs_errno
2005-10-28    nrs     Changed length fields from int to fs_size_t
2005-08-16    dlb     Update comments for put.
2005-02-15    nrs     Changed header for efs_put to reflect implementation
2004-10-28    nrs     Add efs_put_mkdir for autocreation of directories
2004-09-20    nrs     Created.

===========================================================================*/

#ifndef __FS_LIB_H__
#define __FS_LIB_H__

#include "fs_sys_types.h"
#include "fs_fcntl.h"

/** Identifies the major version number of this API file. */
#define VER_FS_LIB_H_MAJOR     1
/** Identifies the minor version number of this API file. */
#define VER_FS_LIB_H_MINOR     1

/**********************************************************************
 * Error handling.
 *
 * See fs_public.h
 *
 */

/**********************************************************************
 * Pathnames.
 *
 * See fs_public.h
 *
 */

/**********************************************************************
 * Library  API.
 */

/**
 * @par Name:
 *  efs_put - Store a value in an EFS2 'special item' file.
 *
 * @par Synopsis:
 *  @code
 *    #include "fs_lib.h"
 *    #include "fs_sys_types.h"
 *    int efs_put (const char *path, void *data, fs_size_t length,
 *                 int oflag, int mode);
 *  @endcode
 *
 * @par Description:
 *  Store a value in an EFS2 'special item' file.  Item files are a unique
 *  file type (S_IFITM) that can hold a small amount of data (FS_ITEM_MAX),
 *  possibly more efficiently than regular files. When the amount of data
 *  is larger than FS_ITEM_MAX the data is placed into the file system as a
 *  regular file. This function will determine the type of file needed based
 *  on the length parameter, If the length is <= (FS_ITEM_MAX) bytes then
 *  this function will try to store this file efficiently on an item-file,
 *  but if the length exceeds (FS_ITEM_MAX) bytes then this function will
 *  store this data on a regular file.  'efs_put' can also automatically
 *  create needed parent directories, if O_AUTODIR is set.
 *
 * @param path
 *  The path of the item-file.
 *
 * @param data
 *  The start-address of the data.
 *
 * @param length
 *  The length indicates the size of the item-file and it can exceed
 *  (FS_ITEM_MAX) bytes.
 *
 * @param oflag
 *  'oflag' should be a bitwise OR of one of the following:\n
 *    O_CREAT   - Create the item if it does not exist.\n
 *    O_AUTODIR - Auto create parent directories.\n
 *    other flags (O_APPEND, O_TRUNC, O_EXCL) will have no effect.\n
 *
 * @param mode
 *  'mode' will set the initial permission bits of the item file.
 *
 * @return
 *  Returns a zero if the item is written successfully, or -1 for error.
 *
 * @par Errors:
 *  Upon error, the efs_errno will be set to one of the following:\n
 *    ::ENOENT - a directory component of 'path' does not exist.\n
 *    ::EEXIST - The entry already exists and O_EXCL was specified, or
 *             The entry exists and is not already an item.\n
 *    ::ELOOP  - A symlink named in 'path' contains a loop.\n
 *    ::ENAMETOOLONG - The 'path' specified, or a component of it is
 *                      too long.\n
 *    ::EINVAL - Invalid parameter.\n
 * @sa
 *    efs_get
 *
 **********************************************************************/
int efs_put (const char *path, void *data, fs_size_t length,
    int oflag, int mode);

/**
 * @par Name:
 *  efs_get - Retrieve an EFS2 'special item' file.  See 'efs_put' for a
 *  description of these files.
 *
 * @par Synopsis:
 *  @code
 *    #include "fs_lib.h"
 *    #include "fs_sys_types.h"
 *    int efs_get (const char *path, void *data, fs_size_t length);
 *  @endcode
 *
 * @par Description:
 *  Retrieve an EFS2 'special item' file. 'data' should be a pointer to a
 *  buffer, and 'length' should indicate the size of this buffer in number of
 *  bytes. This function will retrieve the item-file, and return its contents
 *  in the supplied buffer 'data'. Item files should always be read in whole.
 *  If a size other than the exact file size, i.e. greater or smaller than the
 *  file size is requested to be read, the read will fail. So users of efs_put
 *  and efs_get APIs are required to know the size of file being read. If your
 *  use case does not allow you to know the file size at all times, please use
 *  'Regular file' APIs to store and retrive your data.
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
 *    ::FS_ERANGE - The length of the buffer is different from the exact file
 *                  size.\n
 *    ::EINVAL - Invalid parameter.\n
 *
 * @sa
 *    efs_put
 *
 **********************************************************************/
int efs_get (const char *path, void *data, fs_size_t length);

/**
 * @par Name:
 *   efs_deltree - delete the directory and all its contents.
 *
 * @par Synopsis:
 *  @code
 *    #include "fs_public.h"
 *    int efs_deltree (const char *path);
 *  @endcode
 *
 * @par Description:
 *   Delete the directory and all its contents recursively. 'path' can be a
 *   directory, file, node or symlink. If the argument is a directory all its
 *   contents along with itself is deleted. If a non directory is passed as
 *   argument it is deleted. symlinks encountered are not dereferenced, just
 *   the link is deleted.
 *
 * @param path
 *   path is path whose contents need to be deleted, including itself.
 *
 * @return
 *   Returns 0 on success and -1 on failure. On failure efs_errno is set to
 *   appropriate value indicating the reason of failure.
 *
 * @sa
 *   efs_rmdir, efs_unlink, efs_opendir, efs_readdir, efs_closedir,
 *   efs_chown_recursive, efs_chmod_recursive
 */
int
efs_deltree (const char *path);

/**
 * @par Name:
 *  efs_chown_recursive - Same as efs_chown() API, but only that this
 *  function will do it recursively for every entry found inside the given
 *  path.
 *
 * @par Synopsis:
 *  @code
 *    #include "fs_lib.h"
 *    int efs_chown_recursive (const char *path, int uid_val, int gid_val);
 *  @endcode
 *
 * @par Description:
 *  Same as efs_chown() API, but only that this function will do it
 *  recursively for every entry found inside the given path. When a symlink
 *  is encountered efs_chown() modifies the entry the symlink points to, not
 *  the symlink.
 *
 * @param path
 *  The path to the root-directory. Change the Owner ID or Group ID of the
 *  files found inside this root-directory. File-paths can also be
 *  supplied, in which case this API becomes exactly like efs_chown() API.
 *
 * @param uid_val
 *  The desired user-id of the specified file or directory. Pass -1 if you
 *  do not want to change it this time and preserve its existing value.
 *
 * @param gid_val
 *  The desired group-id of the specified file or directory. Pass -1 if you
 *  do not want to change it this time and preserve its existing value.
 *
 * @return
 *  -ve on error, 0 on success. Upon error refer efs_errno.
 *
 * @par Errors:
 *  Upon error, the efs_errno will be set to one of the following:\n
 *    ::ENOENT  -  The path does not exist.\n
 *    ::ENOSPC  - There is not enough space to create the symlink\n
 *    ::EACCES - The path is to the root directory\n
 *    ::EINVAL  - The group id parameter value is greater than the allowed
 *              number of groups\n
 *    ::ENOTDIR - One of the components of the path is not a directory\n
 *    ::ENOSPC  - There is not enough space to move the file to a new group\n
 *    ::EDQUOT  - Changing the group id will result in the new group being
 *               over the quota, therefore the operation failed.\n
 *    ::EINVAL  - Invalid parameter.\n
 * @sa
 *    efs_chown
 *
 **********************************************************************/
int
efs_chown_recursive (const char *path, int uid_val, int gid_val);

/**
 * @par Name:
 *  efs_chmod_recursive - Same as efs_chmod() API, but only that this function
 *  will do it recursively for every entry found inside the given path.
 *
 * @par Synopsis:
 *  @code
 *    #include "fs_lib.h"
 *    #include "fs_sys_types.h"
 *    int efs_chmod_recursive (const char *path, fs_mode_t mode);
 *  @endcode
 *
 * @par Description:
 *  Same as efs_chmod() API, but only that this function will do it
 *  recursively for every entry found inside the given path. When a symlink
 *  is encountered efs_chmod() modifies the entry the symlink points to, not
 *  the symlink.
 *
 * @param path
 *  The path to the root-directory. Change the mode of the entries found
 *  inside this root-directory. File-paths can also be supplied, in which case
 *  this API becomes exactly like efs_chmod() API.
 *
 * @param mode
 *  The desired mode bits, only the permission/suid/sticky bits can be set.
 *  These mode bits are the least 12 significant bits.
 *
 * @return
 *  -ve on error, 0 on success. Upon error refer efs_errno.
 *
 * @par Errors:
 *  Upon error, the efs_errno will be set to one of the following:\n
 *    ::ENOENT  -  The path does not exist.\n
 *    ::EINVAL -   If the mode bit has more bits set other than the 12 least-
 *                 significant bits.\n
 * @sa
 *  efs_chmod
 *
 **********************************************************************/
int
efs_chmod_recursive (const char *path, fs_mode_t mode);

/**
 *  md5sum packet structure. It is used to return the hash calculated
 *  in the library function efs_md5sum.
 */
#define MD5_RESULT_SIZE_IN_BYTES 16
struct fs_md5sum {
  unsigned char hash[MD5_RESULT_SIZE_IN_BYTES];
};
typedef struct fs_md5sum *fs_md5sum_t;  /* Not used by EFS */

/**
 * @par Name:
 *   efs_md5sum - Calculate md5 hash for the file-data.
 *
 * @par Synopsis:
 *  @code
 *    #include "fs_lib.h"
 *    int efs_md5sum (const char *path, struct fs_md5sum *hash_out_data);
 *  @endcode
 *
 * @par Description:
 *   A hash function maps the input to a output which is usually a
 *   number. hash_output_data is a 128 bit digital fingerprint of
 *   the data inside the file named path. It is very unlikely (but
 *   possible that the md5sum values for two different files to be
 *   the same. RFC 1321 describes the details of the algorithm. The
 *   algorithm starts with 4 32-bit constant values as the hash
 *   result. Each-time 16 bytes of data is chosen and some bitwise
 *   operation between the hash result and 16 byte data is done until
 *   all the data exhaust. Even change of bit in any input byte would
 *   generate a different result.
 *
 * @param path
 *   pathname for the file on whose content the hash function
 *   need to be calculated.
 *
 * @param hash_result
 *   The result for the hash function is computed and put here.
 *
 * @return
 *   Returns 0 on success and -1 on failure.
 *
 */
int
efs_md5sum (const char *path, struct fs_md5sum *hash_result);

#endif /* End of __FS_LIB_H__ */
