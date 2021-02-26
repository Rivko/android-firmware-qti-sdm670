/***********************************************************************
 * rfs_api.h
 *
 * Remote File System - API file.
 * Copyright (c) 2013-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 *
 * API file for the Remote File System module.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.qdsp6/2.1.c4/api/storage/rfs_api.h#1 $ $DateTime: 2018/07/30 01:21:36 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2015-01-02   dks   Fix rfs_put API to perform stat at API layer.
2014-12-03   dks   Move rfs init to a task
2013-11-05   dks   Create

===========================================================================*/

#ifndef __RFS_API_H__
#define __RFS_API_H__

#include "rfs_types.h"
#include "rfs_fcntl.h"

/**
 Open a file in the filesystem. If successful, the resulting file descriptor
 will be the smallest available non-negative integer that can be used as a
 handle to refer to this file in the calls described below. Each open file
 descriptor maintains its own file offset and open flags.

 @param path
    The path of the file that needs to be opened.

 @param oflag
    The 'oflag' argument describes how this file should be opened, and
    contains one of the following values:\n
      RFS_O_RDONLY  - Open for reading only.\n
      RFS_O_WRONLY  - Open for writing only.\n
      RFS_O_RDWR    - Open for reading and writing.\n\n
    In addition, the following flags can be bitwise OR'd with this value:\n
      RFS_O_APPEND  - All writes will self-seek to the end of the file before
                  writing.\n
      RFS_O_CREAT   - Create the file if it does not exist.\n
      RFS_O_TRUNC   - Truncate the file to zero bytes on successful open.\n\n
      RFS_O_ENCRYPT - Encrypt the data before saving in backing store.\n\n
      RFS_O_COMPRESS- Compress the file before saving in backing store.\n\n

 @param mode
    This option is not implemented currently and kept for API future
    compatibility.
    If O_CREAT is a part of 'oflag', mode argument will be used to define the
    file permissions given to the newly created file. \n


 @return
    Returns a file descriptor >= 0 on success and -ve value on failure that
    indicates the exact reason of failure as defined in rfs_errno.h.

 @b Example

 @code
    int fd, result;
    char buff[100];
    int32 offset;

    fd = rfs_open ("/file1", RFS_O_RDWR | RFS_O_TRUNC);
    if (fd < 0)
    {
      MSG_HIGH ("rsf_open failed with error : %d", fd); //error handling
      return;
    }

    result = rfs_write ("/file1", buff, 100);
    if (result < 0)
    {
      MSG_HIGH ("rsf_write failed with error : %d", result); //error handling
      return;
    }

    offset = rfs_seek (fd, 0, RFS_SEEK_SET);
    if (offset != 0)
    {
      MSG_HIGH ("rfs_seek failed with error : %d", offset); //error handling
      return;
    }

    result = rfs_read ("/file1", buff, 100);
    if (result < 0)
    {
      MSG_HIGH ("rsf_read failed with error : %d", result); //error handling
      return;
    }

    result = rfs_close (fd);
    if (result != 0)
    {
      MSG_HIGH ("rsf_close failed with error : %d", result); //error handling
    }
 @endcode

*/
int rfs_open (const char *path, int oflag, int mode);


/**
 Close the file descriptor.  The descriptor will no longer refer to any
 file, and may be returned by subsequent calls to efs_open.

 @param filedes
    The file-descriptor obtained earlier via rfs_open() function.

 @return
    Returns 0 on success and -ve value on failure that indicates the exact
    reason of failure as defines in rfs_errno.h

 @par Errors:
   EBADF : The file descriptor is not found.

 @b Example

 @code
   int fd;

 @see
    rfs_open which has code example for using rfs_close.
*/
int rfs_close (int filedes);


/**
 Attempt to read 'nbytes' of data from the file associated with the specified
 file descriptor.

 @param filedes
    The file-descriptor obtained earlier via rfs_open() function.

 @param buf
    The buffer where the read bytes from the file will be stored.

 @param nbyte
    The number of bytes to read from the file.

 @return
    Returns the number of bytes successfully read, or -ve value for an error
    which indicates the exact reason for failure as defined in rfs_errno.h\n
    Zero is a valid result, and indicates that the end of file has been
    reached.

 @see
    rfs_open which has code example for using rfs_read.
*/
int32 rfs_read (int filedes, void *buf, uint32 nbyte);

/**
 Attempt to write 'nbytes' of data to the file associated with the specified
 file descriptor.

 @param filedes
    The file-descriptor obtained earlier via rfs_open() function.

 @param buf
    The buffer where the bytes to write to the file are supplied.

 @param nbyte
    The number of bytes to write to the file.

 @return
    Returns the number of bytes successfully written, or -ve value for an error
    which indicates the exact reason of failure as defined in rfs_errno.h\n

 @see
    rfs_open which has code example for using rfs_read.
*/
int32 rfs_write (int filedes, const void *buf, uint32 nbyte);

/**
 Change the file offset for the opened file descriptor.

 Changing the file offset does not modify the file. If you seek past
 the end of the file, and then write, the gap will be filled with zero
 bytes. This gap may not actually allocate space.

 @param filedes
    The file-descriptor of an open file obtained via previous rfs_open() call.

 @param offset
    Number of bytes to seek.

 @param whence
    Position from where to seek 'offset' bytes:
      RFS_SEEK_SET - Seek 'offset' bytes from start of the file.
      RFS_SEEK_CUR - Seek 'offset' bytes from current file offset.
      RFS_SEEK_END - Seek 'offset' bytes from end of the file. \n

 @return
    Upon success, seek returns the resulting offset, as bytes from the
    beginning of the file.  Otherwise -ve value is returned indicating exact
    reason for failure as defined in rfs_errno.h

 @see
    rfs_open which has code example for using rfs_seek.
*/
int32 rfs_seek (int filedes, int32 offset, int whence);

/**
 Attempt to delete a file. If the underlying filesystem implementation does
 not support deleting the file, the file data will be turncated to contain zero
 bytes of data.

 @param path
    The file that needs to be deleted.

 @return
    Returns 0 for success, or -ve value on error which indicates exact reason
    for failure as defined in rfs_errno.h

*/
int rfs_unlink (const char *path);

/**
 Attempt to get information about specified path.

 @param path
    The file that needs to be looked up.

 @param buf
    The buffer to hold stat information. The mode data return in stat may not
    contain mode supplied at file creation time. This is based on whether the
    underlying filesystem implement support for mode.

 @return
    Returns 0 for success, or -ve value on error which indicates exact reason
    for failure as defined in rfs_errno.h

*/
int rfs_stat (const char *path, struct rfs_stat_buf *buf);


/**
 Create/Overwrite an existing file with data supplied. This is an efficient
 write that does not require a file handle to be obtained. This API will either
 create the file if it does not exist, or replaces the file data with new data
 if it already exists. The old data is completely lost.

 @param path
    File to be written.

 @param data
    Data to be written.

 @param length
    Number of bytes to write.

 @param oflag
    The 'oflag' argument describes how this file should be opened.
    Ihe following flags can be bitwise OR'd to create the oflag value:\n
     RFS_O_CREAT   - Create the file if it does not exist.\n
     RFS_O_ENCRYPT - Encrypt the data before saving in backing store.\n\n
     RFS_O_COMPRESS- Compress the file before saving in backing store.\n\n
    Any other oflag option will be ignored if provided additional to above.
    If the file exists, data is truncated by default for the write.

 @param mode
    This option is not implemented currently and kept for API future
    compatibility.
    When implemented, If RFS_O_CREAT is a part of 'oflag', mode argument will
    be used to define the file permissions given to the newly created file. \n

 @return
    Returns number of bytes written, or -ve value on error which indicates
    exact reason for failure as defined in rfs_errno.h
*/
int32 rfs_put (const char *path, void *data, uint32 length, int oflag,
               int mode);

/**
 Read file data supplied. This is an efficient read that does not require a
 file handle to be obtained. The API has lesser control over how the data is
 read as compared to rfs_read API. Use this API to whole read file data in one
 shot into the buffer.

 @param path
    File to be read.

 @param data
    Buffer to read data into.

 @param length
    Number of bytes to read.

 @return
    Returns number of bytes read, or -ve value on error which indicates exact
    reason for failure as defined in rfs_errno.h
*/
int32 rfs_get (const char *path, void *data, uint32 length);

#endif /* not __RFS_API_H__ */
