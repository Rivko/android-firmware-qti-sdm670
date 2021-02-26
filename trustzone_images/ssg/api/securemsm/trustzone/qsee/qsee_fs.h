#ifndef QSEE_FS_H
#define QSEE_FS_H
/*===========================================================================
  Copyright (c) 2008 - 2015,2017,2020 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/ssg.tz/1.0.2/api/securemsm/trustzone/qsee/qsee_fs.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/29/15   aw      Fixed CR900655 - Remove link API as it is no longer supported
06/24/15   ss      Added fs error number 
04/02/15   cz      Changed "struct _fs_stat" the same as HLOS, work for 32bit and 64bit Apps
02/21/14   rk      Added support for open_dir, read_dir
02/04/14   rk      Merged missing changes from fs.h while removing from qsapps/libs
11/16/11   cz      Added the file_dir_chown_chmod function
02/18/11   CM      Initial Version. 
03/11/11   jct     Defined command structures and additional types

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/** @addtogroup playready
  @} */
#include <stddef.h>
#include <stdint.h>

/* copied from linuxbuild/libc/kernel/common/asm-generic/errno-base.h */

#define EPERM 1
#define ENOENT 2
#define ESRCH 3
#define EINTR 4
#define EIO 5
#define ENXIO 6
#define E2BIG 7
#define ENOEXEC 8
#define EBADF 9
#define ECHILD 10
#define EAGAIN 11
#define ENOMEM 12
#define EACCES 13
#define EFAULT 14
#define ENOTBLK 15
#define EBUSY 16
#define EEXIST 17
#define EXDEV 18
#define ENODEV 19
#define ENOTDIR 20
#define EISDIR 21
#define EINVAL 22
#define ENFILE 23
#define EMFILE 24
#define ENOTTY 25
#define ETXTBSY 26
#define EFBIG 27
#define ENOSPC 28
#define ESPIPE 29
#define EROFS 30
#define EMLINK 31
#define EPIPE 32
#define EDOM 33
#define ERANGE 34

/* copied from linuxbuild\bionic\libc\kernel\common\linux\linux.h */

#define S_IFMT 00170000
#define S_IFSOCK 0140000
#define S_IFLNK 0120000
#define S_IFREG 0100000
#define S_IFBLK 0060000
#define S_IFDIR 0040000
#define S_IFCHR 0020000
#define S_IFIFO 0010000
#define S_ISUID 0004000
#define S_ISGID 0002000
#define S_ISVTX 0001000

#define S_ISLNK(m) (((m) & S_IFMT) == S_IFLNK)
#define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#define S_ISCHR(m) (((m) & S_IFMT) == S_IFCHR)
#define S_ISBLK(m) (((m) & S_IFMT) == S_IFBLK)
#define S_ISFIFO(m) (((m) & S_IFMT) == S_IFIFO)
#define S_ISSOCK(m) (((m) & S_IFMT) == S_IFSOCK)

#define S_IRWXU 00700
#define S_IRUSR 00400
#define S_IWUSR 00200
#define S_IXUSR 00100

#define S_IRWXG 00070
#define S_IRGRP 00040
#define S_IWGRP 00020
#define S_IXGRP 00010

#define S_IRWXO 00007
#define S_IROTH 00004
#define S_IWOTH 00002
#define S_IXOTH 00001

/* copied from linuxbuild\bionic\libc\kernel\common\asm-generic\fcntl.h */

#define O_ACCMODE 00000003
#define O_RDONLY 00000000
#define O_WRONLY 00000001
#define O_RDWR 00000002
#ifndef O_CREAT
#define O_CREAT 00000100  
#endif
#ifndef O_EXCL
#define O_EXCL 00000200  
#endif
#ifndef O_NOCTTY
#define O_NOCTTY 00000400  
#endif
#ifndef O_TRUNC
#define O_TRUNC 00001000  
#endif
#ifndef O_APPEND
#define O_APPEND 00002000
#endif
#ifndef O_NONBLOCK
#define O_NONBLOCK 00004000
#endif
#ifndef O_SYNC
#define O_SYNC 00010000
#endif
#ifndef FASYNC
#define FASYNC 00020000  
#endif
#ifndef O_DIRECT
#define O_DIRECT 00040000  
#endif
#ifndef O_LARGEFILE
#define O_LARGEFILE 00100000
#endif
#ifndef O_DIRECTORY
#define O_DIRECTORY 00200000  
#endif
#ifndef O_NOFOLLOW
#define O_NOFOLLOW 00400000  
#endif
#ifndef O_NOATIME
#define O_NOATIME 01000000
#endif
#ifndef O_NDELAY
#define O_NDELAY O_NONBLOCK
#endif

#ifndef F_GETFL
/* get file status flags */
#define F_GETFL 3
#endif

#ifndef AT_FDCWD
/* openat() relative to current working directory */
#define AT_FDCWD  -100
#endif

/* Values for the WHENCE argument to lseek.  
 * copied from linuxbuild\bionic\libc\include\unistd.h
 */
# define SEEK_SET 0 /* Seek from beginning of file.  */
# define SEEK_CUR 1 /* Seek from current position.  */
# define SEEK_END 2 /* Seek from end of file.  */

/*
 * Note: The kernel zero's the padded region because glibc might read them
 * in the hope that the kernel has stretched to using larger sizes.
 * copied from linuxbuild\bionic\libc\include\sys\stat.h
 */
#ifndef fs_stat
typedef struct _fs_stat {
    uint64_t             st_dev;          /* ID of device containing file */
    unsigned char       __pad0[4];
    uint32_t             __st_ino;
    unsigned int        st_mode;         /* protection */
    unsigned int        st_nlink;        /* number of hard links */
    uint32_t             st_uid;          /* user ID of owner */
    uint32_t             st_gid;          /* group ID of owner */
    uint64_t             st_rdev;         /* device ID (if special file) */
    unsigned char       __pad3[4];
    int64_t              st_size;         /* total size, in bytes */
    uint32_t             st_blksize;      /* blocksize for filesystem I/O */
    uint64_t             st_blocks;       /* number of blocks allocated */
    uint32_t             st_atime;        /* time of last access */
    uint32_t             st_atime_nsec;
    uint32_t             st_mtime;        /* time of last modification */
    uint32_t             st_mtime_nsec;
    uint32_t             st_ctime;        /* time of last status change */
    uint32_t             st_ctime_nsec;
    uint64_t             st_ino;          /* inode number */
}__attribute__ ((packed)) fs_stat;

#endif

#ifndef _DEFINED_DIR
#define _DEFINED_DIR
 typedef void DIR;
#endif

typedef struct tzDirent
{
  uint64_t d_ino;
  int64_t d_off;
  unsigned short d_reclen;
  unsigned char d_type;
  char d_name[256];
} __attribute__ ((packed)) tzDirent_t;


/* QSEE file operation interface prototypes and their system equivalent alias.
 * The system equivalent prototypes are derived or partially derived from
 * various system headers including:
 *    linuxbuild\bionic\libc\include\fcntl.h
 *    linuxbuild\bionic\libc\include\unistd.h
 *    linuxbuild\bionic\libc\include\sys\stat.h
 *    linuxbuild\bionic\libc\include\dirent.h
 */

/*
 * Protos copied or derived from linuxbuild\bionic\libc\include\fcntl.h
 */

/**
 * Open the specified file with the given mode
 *
 * @param path: file path
 * @param mode: mode to open file
 *
 * @return int: file descriptor or -1 upon failure
 */
int open(const char*  path, int  mode, ...);

/**
 * Open relative file
 * 
 * @param fd: file descriptor
 * @param path: pathname of file
 * @param mode: status flags
 *
 * @return int: file descriptor or -1 upon failure
 */
int openat(int fd, const char*  path, int  mode, ...);

/**
 * Remove a relative directory
 *
 * @param dirfd: file descriptor
 * @param pathname: pathname of file
 * @param flags
 *
 * @return int: 0 for success, -1 upon failure
 */
int unlinkat(int dirfd, const char *pathname, int flags);

/**
 * Perform command operation on file
 *
 * @param fd: file descriptor
 * @param command: operation to be performed
 *
 * @return int: dependent on command
 */
int fcntl(int   fd, int   command, ...);

/**
 * Create a file
 *
 * @param path: pathname of file
 * @param mode: access mode
 *
 * @return int: 0 for success, -1 upon failure
 */
int creat(const char*  path, uint32_t  mode);

/*
 * Protos copied or derived from linuxbuild\bionic\libc\include\unistd.h
 */

/**
 * Read a number of bytes from given file
 *
 * @param fd: file descriptor
 * @param buf: buffer to be read into
 * @param count: number of bytes to read
 *
 * @return int32: number of bytes read
 */
int32_t read(int, void *, uint32_t);

/**
 * Write a number of bytes to a given file
 *
 * @param fd: file descriptor
 * @param buf: buffer to write from
 * @param count: number of bytes to write
 *
 * @return int32: number of bytes written
 */
int32_t write(int, const void *, uint32_t);

/**
 * Close an open file
 *
 * @param fd: file descriptor
 *
 * @return int: 0 for success, -1 upon failure
 */
int close(int);

/**
 * Reposition file offset
 *
 * @param fildes: file descriptor
 * @param offset: new offset
 * @param whence: directive
 *
 * @return int32: resulting offset
 */
int32_t lseek(int, int32_t, int);

/**
 * Unlink a linked file
 *
 * @param pathname: pathname of file
 *
 * @return int: 0 for success, -1 upon failure
 */
int unlink(const char *);

/**
 * Remove a directory
 *
 * @param path: pathname of directory
 *
 * @return int: 0 for success, -1 upon failure
 */
int rmdir(const char *);

/**
 * Synchronize file with storage
 *
 * @param fd: file descriptor
 *
 * @return int: 0 for success, -1 upon failure
 */
int fsync(int fd);


/*
 * Protos copied or derived from linuxbuild\bionic\libc\include\sys\stat.h
 */

/**
 * Get file status traversing file link
 *
 * @param filedes: file descriptor
 * @param buf: pointer to status structure
 *
 * @return int: 0 for success, -1 upon failure
 */
int fstat(int, fs_stat *);

/**
 * Get the file status including file link 
 *
 * @param path: pathname of file
 * @param buf: pointer to status buffer
 *
 * @return int: 0 for success, -1 upon failure
 */
int lstat(const char *, fs_stat *);

/**
 * Create a directory
 *
 * @param pathname: pathname of directory
 * @param mode: permissions mode
 *
 * @return int: 0 for success, -1 upon failure
 */
int mkdir (const char *, uint32_t);


/*
 * Protos copied or derived from linuxbuild\bionic\libc\include\dirent.h
 */

/**
 * Open a directory
 *
 * @param path: path of the directory to open
 *
 * @return DIR*: directory stream pointer for success, NULL upon failure
 */
DIR* opendir(const char *path);

/**
 * Read a directory
 *
 * @param pdir: directory stream pointer
 * @param entry: pointer to directory entry struct to be filled as a result
 *
 * @return struct tzDirent*: same as entry for success, NULL upon failure
 */
struct tzDirent* readdir(DIR* pdir, struct tzDirent* entry);

/**
  Closes a directory.

  @param[in]  pdir     Pointer to directory stream.

  @note1
  Further details are available at http://man7.org/linux/man-pages/man3/closedir.3.html

  @return
  SUCCESS -- 0 \n
  FAILURE -- -1
 */
int closedir(DIR* pdir);

/*
 * Protos copied or derived from linuxbuild\bionic\libc\include\stdio.h
 */

/**
 * Remove a file
 *
 * @param pathname: pathname of file/directory
 *
 * @return int: 0 for success, -1 upon failure
 */
int remove(const char *);


/*
 * Custom prototypes
 */

/**
 * Get current directory location
 *
 * @param pathname: pathname of directory
 *
 * @return int32: current location of directory stream
 */
int32_t telldir(const char *);

/**
 * Test a directory
 *
 * @param pathname: pathname of directory
 *
 * @return int: 0 if directory found, else -1
 */
int testdir(const char *);

/**
 * Rename a file
 *
 * @param oldfilename: name of file to be renamed from
 * @param newfilename: name of file to be renamed to
 *
 * @return int: 0 for success, -1 upon failure
 */
int frename(const char *, const char *);

/**
 * @param pathname: path: e.g., /persist/playready/drmms/
 *                  word: e.g., "playready" 
 *              word_len: strlen(playready)
 *                 owner: e.g., "media.system"
 *             owner_len: e.g., strlen(media.system)
 *                   mod: e.g., "700" or "777"
 *               mod_len: e.g., strlen("777")
 *                 level: e.g., 0 : chown media.system /persist/playready
 *                                  chmod 700 /persist/playready 
 *                 level: e.g., 1 : chown media.system /persist/playready
 *                                  chmod 700 /persist/playready 
 *                                  chown media.system /persist/playready/ *
 *                                  chmod 700 /persist/playready/ * 
 *                 level: e.g., 2 : chown media.system /persist/playready
 *                                  chmod 700 /persist/playready 
 *                                  chown media.system /persist/playready/ * 
 *                                  chmod 700 /persist/playready/ * 
 *                                  chown media.system /persist/playready/ * / *
 *                                  chmod 700 /persist/playready/ * / * 
 * @return int: 0 for success, -1 upon failure
 */
int file_dir_chown_chmod(char *path, uint32_t path_len,
                         char *word, uint32_t word_len,
                         char *owner, uint32_t owner_len,
                         char *mod, uint32_t mod_len, uint32_t level);

/**
 * Get last HLOS errno
 *
 * @return int: Last errno value
 */
int get_error_number(void);

/**
 * Get the free size of a specified partition
 *
 * @param path: pathname of file
 * @param size: location to return the remaining bytes in partition
 *
 * @return int: 0 for success, -1 upon failure
 */
int file_get_partition_free_size(const char *path, uint64_t *size);

#endif /* QSEE_FS_H */
