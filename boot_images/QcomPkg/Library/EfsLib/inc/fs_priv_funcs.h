/***********************************************************************
 * fs_priv_funcs.h
 *
 * EFS Private Functions
 * Copyright (C) 2009-2013, 2015 QUALCOMM Technologies, Inc.
 *
 * EFS APIs that are not public and called only internally by other EFS APIs.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/EfsLib/inc/fs_priv_funcs.h#1 $ $DateTime: 2017/09/18 09:26:13 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2015-02-05   vm    Avoid reading sfs folder while extracting EFS to TAR.
2013-06-19   wek   Add function to do a complete iteration of efs scrub.
2013-06-19   dks   Added NAND Flash scrub support.
2012-12-14   wek   Implement modified efs_utime for item files.
2012-10-18   wek   Make efs_raw_put/get wrappers around efs_put/get.
2012-04-30   nr    Register shutdown callback with RCINIT.
2011-10-11   nr    Added Zipped Tar file support.
2011-07-05   wek   Add file system image creation APIs.
2011-07-01   wek   Create a TAR file from a given directory.
2010-11-19   vr    Moved from efs/src to efs/inc since fs_tar needs this.
2010-10-28   rp    Add efs_auto_mkdir().
2010-09-01   wek   Remove compile time calculations based on page size.
2010-08-10   dks   Moved efs_iter define here for access from benchmark.
2010-02-06   yog   Validate the input param for NULL check.
2010-05-06   wek   Implement golden copy for remote storage page manager.
2010-02-08   rp    CMI compliance, remove #ifdef from API files.
2009-11-04   dks   path arg included in get_fs_data and get_device_info
2009-05-18   rp    Create

===========================================================================*/

#ifndef __FS_PRIV_FUNCS_H__
#define __FS_PRIV_FUNCS_H__

#include "fs_config_i.h"
#include "fs_sys_types.h"


/*************************************************************************
 * FUNCTION      efs_get_fs_data
 *
 * DESCRIPTION
 *               EFS2 has the ability to initialize from a bootstrap image,
 *               called a factory image.  This image contains a header, some
 *               map pages, and then the data clusters from the filesystem in
 *               cluster order. This function returns this factory image one
 *               page at a time. The image_info_ptr should be initialized by
 *               setting info_cluster_sent to zero. Each successive call to
 *               this function will return the next page of factory data in
 *               page_buffer. These pages should be placed in consecutive good
 *               blocks of a NAND flash, and will cause a filesystem to be
 *               built up from that image.
 *
 * INPUT         path             - file path
 *               image_info_ptr   - stores sequence number of cluster map pages
 *                                  & data pages. Specifies if more data is to
 *                                  be sent or not
 *              page_buffer       - buffer for the pages read
 *
 * DEPENDENCIES  None
 *
 * RETURN VALUE  If successful, returns 0.
 *               Otherwise, returns -1, and sets errno to indicate the error.
 *
 * SIDE EFFECTS   None
 **********************************************************************/
int efs_get_fs_data (const char * path,
                     struct fs_factimage_read_info * image_info_ptr,
                     byte * page_buffer);

/***********************************************************************
 * FUNCTION      efs_get_device_info
 *
 * DESCRIPTION   This function will get the attributes of the flash device
 *
 * INPUT         path     - file path
 *               dev_attr - pointer to device information
 *
 * DEPENDENCIES  None
 *
 * RETURN VALUE  If successful efs_get_device_info will return zero else -1.
 *
 * SIDE EFFECTS  None
 ***********************************************************************/
int efs_get_device_info (const char * path, struct fs_device_attr * dev_attr);


/***********************************************************************
 * FUNCTION      efs_wait_for_xact
 *
 * DESCRIPTION   This function waits for a transaction to finish.
 *
 ************************************************************************/
void efs_wait_for_xact (void);

/***********************************************************************
 * FUNCTION      efs_make_golden_copy
 *
 * DESCRIPTION   Takes the most recent copy of EFS and back it up into
 *               permanent storage. It can be used for recovery.
 *
 * INPUT         path - path to mountpoint to backup.
 *
 ************************************************************************/
int efs_make_golden_copy (const char *path);

/***********************************************************************
 * FUNCTION      fs_is_path_valid
 *
 * DESCRIPTION   Validates the supplied path for NULL and empty string.
 *
 * INPUT         path - path to be validated.
 *
 ************************************************************************/
int fs_is_path_valid (const char *path);

/***********************************************************************
 * FUNCTION      fs_get_shifts
 *
 * DESCRIPTION   It computes the number of shifts needed to get input.
 *               Input is assumed to be power of 2, if not it round down
 *               input to the nearest power of 2. If input is 8, the
 *               return value is 3. This function can also be interpreted
 *               as returning the floor of log_base2 of value.
 *
 * INPUT         value - Number to calcule log.
 *
 ************************************************************************/
uint32 fs_get_shifts (uint32 value);

/* This is the prototype of the callback function which wil be called
   by efs_iter() function defined belown for each entry that it finds in
   the current directory. Path is the entry we found, and stat_buf contains
   the file information for the entry, and param is any parameter that is
   passed by the function calling efs_iter which needs to be passed down to
   the callback. */

typedef int (*EFS_ITER_CALLBACK)(char *path, struct fs_stat *stat_buf,
                                 const void *param);
/***********************************************************************
 * FUNCTION      efs_iter
 *
 * DESCRIPTION   Iterates through all the entries of any given directory
 *                recursively and invokes the callback function supplied.
 *
 * INPUT         path              - The directory whose entries we have
 *                                   to iterate over recursively.
 *               want_dirs         - Pass 1, if you want the callback to
 *                                    happen for sub-directories too
 *               efs_iter_callback - The callback function. Please refer
 *                                   to the callback function prototype
 *                                   for more information. If the callback
 *                                   function's return value is not 0 then
 *                                   an error is assumed and the iteration
 *                                   proceeds with the next entry.
 *               param             - This supplied param will be passed to
 *                                   the callback function.
 * RETURN VALUE  -ve if error, with number equal to the -ve count of the
 *                callback invocation that returned error.
                  0 if we have iterated through all entries without error.
 *
 * SIDE EFFECTS  None
 *
 ************************************************************************/
int efs_iter (const char *path, int want_dirs,
              EFS_ITER_CALLBACK efs_iter_callback, const void *param);


/***********************************************************************
 * FUNCTION      efs_auto_mkdir
 *
 * DESCRIPTION   Creates intermediate missing directories in the supplied path
 *
 * INPUT         path   - Path whose intermediate missing directories need to
 *                        be created.
 *
 * RETURN VALUE  -ve if error, 0 if all missing directories are created.
 *
 ************************************************************************/
int efs_auto_mkdir (const char * path);

/* Handle for recursive iterators. */
struct fs_dir_recursive_data;

/***********************************************************************
 * FUNCTION     efs_opendir_recursive
 *
 * DESCRIPTION  Opens an iterator to recursively scan all entries in a
 *              given directory.
 *
 * INPUT        dirname  - Path to the directory to scan recursively.
 *
 * RETURN VALUE  On success a valid handle. On error returns NULL, setting
 *               efs_errno accordingly.
 *
 * SIDE EFFECTS  The path_buff lock is aquired and not released.
 *
 ************************************************************************/
struct fs_dir_recursive_data *efs_opendir_recursive (const char *dirname);

/***********************************************************************
 * FUNCTION     efs_readdir_recursive
 *
 * DESCRIPTION  Returns the full path of the next entry. The first entry
 *              returned is the path given in efs_opendir_recursive ().
 *
 * INPUT        rec_dirp  - Recursive directory pointer given by
 *                          efs_opendir_recursive ()
 *
 * INPUT        ignore_device_specific_files  - Ignore directories that
 *                          are device specific.
 *                          1 --> Ignore device specific files
 *                          0 --> Don't ignore device specific files
 *
 * RETURN VALUE  On success returns a valid path. Returns NULL when all
 *               available entries were retured or on error. On error
 *               efs_errno will be set accordingly.
 *
 * SIDE EFFECTS  None.
 *
 ************************************************************************/
const char *efs_readdir_recursive (struct fs_dir_recursive_data *rec_dirp,
                                   int ignore_device_specific_files);

/***********************************************************************
 * FUNCTION     efs_closedir_recursive
 *
 * DESCRIPTION  Close an iterator opened to recursively scan all entries in a
 *              given directory.
 *
 * INPUT        rec_dirp  - Recursive directory pointer given by
 *                          efs_opendir_recursive ()
 *
 * RETURN VALUE  Returns the number of errors encountered while iterating.
 *               On error -1 is returned and efs_errno is set accordingly.
 *
 * SIDE EFFECTS  The path_buff lock is released.
 *
 ************************************************************************/
int efs_closedir_recursive (struct fs_dir_recursive_data *rec_dirp);


/* All supported File System image type File Formats */
enum fs_filesystem_image_type
{
  FS_FILESYSTEM_IMAGE_TYPE_TAR      = 0,  /* TAR format.         */
  FS_FILESYSTEM_IMAGE_TYPE_ZIP_TAR  = 1,  /* ZIPPED TAR format. */
};

/****************************************************************************
 * FUNCTION      efs_filesystem_image_open
 *
 * DESCRIPTION   This function prepare for the dump of the File System
 *               image in the format requested.
 *
 * INPUT         path       - Path to indicate which File System.
 *               image_type - In what format the File System image is needed?
 *
 * RETURN VALUE  -ve if error, 0 on success.
 *
 ***************************************************************************/
int efs_filesystem_image_open (const char *path,
                               enum fs_filesystem_image_type image_type);

/****************************************************************************
 * FUNCTION      efs_filesystem_image_read
 *
 * DESCRIPTION   This function is used to read the next page of the file
 *               system image in the requested format.
 *
 * INPUT         image_handle - Handle obtained from above OPEN function.
 *               buff         - Buffer to store the read bytes.
 *               buff_size    - How big is the above buffer?
 *
 * RETURN VALUE  -ve if error, 0 on success.
 *
 ***************************************************************************/
int efs_filesystem_image_read (int image_handle, uint8 *buff,
                               uint32 buff_size);

/****************************************************************************
 * FUNCTION      efs_filesystem_image_close
 *
 * DESCRIPTION   This function is used to indicate the file system to end the
 *               output of the file system image reading process.
 *
 * INPUT         image_handle - Handle obtained from above OPEN function.
 *
 * RETURN VALUE  -ve if error, 0 on success.
 *
 ***************************************************************************/
int efs_filesystem_image_close (int image_handle);


/****************************************************************************
 * FUNCTION      fs_private_borrow_ram
 *
 * DESCRIPTION   This function is used to borrow ram from the lower layers.
 *               Data is clobbered.
 *
 * OUTPUT         ram_addr     - Address of the pointer to overwrite with start
 *                               address of the borrowed ram.
 *                ram_size     - The size of the borrowed ram.
 *
 * RETURN VALUE  -ve if error, 0 on success.
 *
 ***************************************************************************/
int efs_private_borrow_ram (void **ram_addr, uint32 *ram_size);


/****************************************************************************
 * FUNCTION      efs_rcinit_shutdown_callback
 *
 * DESCRIPTION   This function is registered as a callback with RCINIT. In this
 *               function we handle the cleanup activity required during a
 *               graceful shutdown or a reboot.
 *
 ***************************************************************************/
 void efs_rcinit_shutdown_callback (void);

/****************************************************************************
 * FUNCTION      efs_pure_put
 *
 * DESCRIPTION   Stores a value into an item file. The resultant file is
 *               either the old file's data or the new file's data.
 * *
 * INPUT         path   - The path of the item file.
 *               data   - The start address of the item file data.
 *               length - Length of the data buffer.
 *               oflag  - A bitwise OR of one of the following:
 *                        O_CREATE: Create the item file if it does not exist.
 *                        O_EXCL: Fail if the item already exists.
 *                        Other flags will have no effect.
 *               mode   - Permission bits of the item file being modified.
 *
 * RETURN VALUE  Returns a zero if the item written is successful, or -1 on
 *               error. The efs_errno is set accordingly.
 *
 * ERRORS      On error, the efs_errno will be set to one of the following:
 *             ENOENT - a directory component of 'path' does not exist.
 *             EEXIST - The entry already exists and O_EXCL was specified, or
 *                      The entry exists and is not already an item-file.
 *             ELOOP  - A symlink named in 'path' contains a loop.
 *             ENAMETOOLONG - The 'path' specified, or the data is too long.
 *             EINVAL - Invalid parameter.
 *
 ***************************************************************************/
int efs_pure_put (const char *path, void *data, fs_size_t length,
                  int oflag, int mode);

/****************************************************************************
 * FUNCTION      efs_pure_get
 *
 * DESCRIPTION   Retrieves the contents of an item file.
 * *
 * INPUT         path   - The path of the item file.
 *               data   - The start address of the item file data.
 *               length - Length of the data buffer.
 *
 * RETURN VALUE  Returns the number of bytes copied in the buffer, -1 for an
 *               error. The efs_errno is set accordingly.
 *
 * ERRORS      On error, the efs_errno will be set to one of the following:
 *             ENOENT - The item or a component of the path does not exist.
 *             ELOOP  - A symlink named in 'path' contains a loop.
 *             ENAMETOOLONG - The 'path' specified is too long.
 *             ENOTITM - The named file is not of type S_IFITM.
 *             FS_ERANGE - The item file is larger than the specified buffer.
 *             EINVAL - Invalid parameter.
 *
 ***************************************************************************/
int efs_pure_get (const char *path, void *data, fs_size_t length);


/****************************************************************************
 * FUNCTION      efs_utime_item
 *
 * DESCRIPTION   Special implmentation of efs_utime for item files 2.
 *               For other types of entries (regular files and directories)
 *               it will call the same implementation of efs_utime.
 *
 * INPUT         path  - The path of the entry.
 *               times - Pointer to the structure that holds the new time of
 *                       the entry. For item files only modtime is used.
 *
 * RETURN VALUE  Returns 0 on success or -1 on error.
 *
 * ERRORS   On error, the efs_errno will be set to one of the following:
 *          ENOENT - The entry or a component of 'path' does not exist.
 *          ELOOP  - A symlink named in 'path' contains a loop.
 *          ENAMETOOLONG - The 'path' specified is too long.
 *          EINVAL - Invalid parameter.
 *          EPERM  - Trying to set the time for an item file 1.
 *
 ***************************************************************************/
int efs_utime_item (const char *path, const struct fs_utimbuf *times);


/****************************************************************************
 * FUNCTION      efs_scrub
 *
 * DESCRIPTION   Scrubbing is the operation of moving all valid pages to new
 *               erased blocks and erasing written blocks. This operation is
 *               only supported on NAND Filesytem. Flash parts only support
 *               fixed number of reads from a page without being erased and
 *               re-programmed, after which the page may no longer retain
 *               its data. The normal GC operation only moves blocks that
 *               contain garbage pages. This keeps the GC operation effecient.
 *               However, this restriction means that if there are some flash
 *               blocks that contain data that never changes, but are read many
 *               times, then those blocks can lose data after a certain large
 *               number of reads. For modems that might need to last very long,
 *               a scrub operation can be done at regular intervals which will
 *               move the read-only blocks also into new erased blocks and
 *               force an erase on the current block. This API need not be used
 *               for normal cell-phone type lifetime of 3-5 years.
 *
 * INPUT         path     A valid path of a file or directory on the mounted
 *                        file system that needs to be scrubbed.
 *
 * RETURN VALUE  Returns  0 on completion, 1 on partial completion, and -1 if
 *                        scrubbing is not supported for the filesystem.
 *
 * ERRORS   On error, the efs_errno will be set to one of the following:
 *          ENOENT - The entry or a component of 'path' does not exist.
 *          EINVAL - Invalid parameter.
 *          EPERM  - Trying to scrub a filesystem that is not NAND-Flash type.
 *
 ***************************************************************************/
int efs_scrub (const char *path);

/****************************************************************************
 * FUNCTION      efs_full_scrub
 *
 * DESCRIPTION   The function efs_scrub needs to be called repeatedly to
 *               scrub the whole file system. This helper function will
 *               take care of calling efs_scrub until it returns that it is
 *               done or returns error.
 *
 * INPUT         path     A valid path of a file or directory on the mounted
 *                        file system that needs to be scrubbed.
 *
 * RETURN VALUE  Returns  0 on completion, -1 on error.
 *
 * ERRORS   On error, the efs_errno will be set to one of the following:
 *          ENOENT - The entry or a component of 'path' does not exist.
 *          EINVAL - Invalid parameter.
 *          EPERM  - Trying to scrub a filesystem that is not NAND-Flash type.
 *
 ***************************************************************************/
int efs_full_scrub (const char *path);

#endif /* not __FS_PRIV_FUNCS_H__ */
