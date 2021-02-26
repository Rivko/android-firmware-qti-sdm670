/***********************************************************************
 * fs_ext_hfat.c
 *
 * Interface between EFS External Filesystem and HFAT library
 * Copyright (C) 2006-2013 QUALCOMM Technologies, Inc.
 *
 * The HFAT library provides a FAT-compatible set of filesystem operations.
 * This code provides the "glue" between the EFS "External" filesystem and
 * the HFAT calls.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/EfsLib/src/fs_ext_hfat.c#1 $ $DateTime: 2017/09/18 09:26:13 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-10-09   nr    Replace banned memory functions with safer versions.
2013-05-24   wek   Make extfs hfat ops constant.
2012-05-18   wek   Remove jzap header file.
2012-03-20   wek   Cleanup customer header file inclusion.
2012-03-08   wek   Rename macros and remove header files to compile for boot.
2011-11-02   wek   Use library supplied strl cpy and cat instead of fs_strl*.
2011-07-22   vm    Eliminate compiler warnings in boot
2010-10-11   vr    Klocwork Bug fixes
2010-03-19   yog   Added support for files having max filesize upto 4GB.
2010-02-08   rp    CMI compliance, remove #ifdef from API files.
2009-12-18   vr    Added set_volume_name and get_volume_name APIs.
2009-08-11   wek   Remove hfat drive number coupled to hotplug device index.
2009-07-06   ebb   Included fs_config.h to pick up boot flags.
2009-04-03   wek   Added utime method & updated getstat to set atime.
2009-04-01   ebb   Added memory protect/unprotect calls around format.
2009-03-25   wek   Move FAT to POSIX time conversions functions to fs_util.c.
2009-01-29   wek   Added hfat_init() to fs_hfat_init().
2008-08-12   np    Cache HFAT last name-directoryentry pair for performance.
2008-05-23   rp    Introducing /inc folder as per new SCM.
2008-02-13   ebb   Added support for FAT12 on media <= 64MB
2008-01-24   sch   Added e_hfat_getsize
2007-12-07   rp    Enhancements to the statvfs struct.
2007-09-25   sch   Don't translate the error in hfat_update_iterator
2007-09-12   sh    Correct free space counts for >4GB.
2007-05-25   umr   Added support for EFS memory protection feature.
2007-04-30   sch   corrected the mapping for some HFAT error codes
2007-03-14    rp   Fix the typo from 'utmost' to 'at most'.
2007-03-12    rp   When UTF conversions fail return appropriate error code.
2007-03-06    rp   Fix the size HFAT_MAX_PATH_UTF8 to include just 1 NULL.
2007-02-23    rp   Restrict max path name size to 255 utf-16s.
2007-02-22   umr   Move hfat utf8 local buffer to global space.
2007-02-16   umr   Move hfat local variables to global space.
2007-02-13   s h   Limit conversion lengths appropriately.
2007-02-02   umr   Fill in Ctime fields in EPOCH time.
2007-02-07   s h   Convert to use plug-in wrappers for conversion
2007-01-30   sch   Use WCHAR HFAT API
2007-01-12   dlb   Use safer string functions.
2006-12-06   s h   Compiler warning fix.
2006-12-05   dlb   Max transaction size configurable.
2006-12-04   sch   Made changes to error codes returned from HFAT
2006-10-11   s h   Add chmod support for HIDDEN flag (S_ISVTX)
2006-10-11   s h   Add chmod support for READONLY flag.
2006-10-30   sch   Returned -ENOENT instead of -EINVAL when e_hfat_open fails
2006-10-17   sch   Minor cleanup.
2006-10-17   sch   Deleted the volume if initialization fails
2006-10-16   sch   Allowed opening of multiple files.
2006-09-27   s h   Select FAT16/32 based on media size.
2006-09-06   s h   Correct one-too-far in fd table index
2006-09-05   s h   Conditional compilation for this entire file.
2006-08-27   s h   Fix HFAT format to accept certain error conditions.
2006-08-27   s h   Use driveno (hdev index) consistently.
2006-08-27   s h   Corrected some return values to reflect errno.
2006-08-10   sch   Implemented hfat_format
                   Corrected getstat() to return correct stats for root dir
                   Implemented error mapping mechanism
2006-07-18   sch   Create

===========================================================================*/

#include "fs_config_i.h"

#ifdef FEATURE_EFS_EXTFS_HFAT

#include "fs_extfs.h"
#include "fs_errno.h"
#include "fs_public.h"
#include "fs_err.h"
#include "fs_ext_hfat.h"
#include "fs_global_lock.h"
#include "fs_util.h"
#include "hfat_interface.h"
#include "fs_babylon.h"
#include "fs_unicode.h"
#include "assert.h"
#include "stringl/stringl.h"
#include "fs_protected_memory.h"

#include "coldplug_api.h"

/* The maximum length of a DOS directory path (only the drive letter,
 * colon, leading slash, directory) is 246 bytes.
 *
 * Long filenames (by themselves) can be up to 256 characters (incl
 * NULL).
 *
 * The maximum length of a whole file path (drive letter, colon,
 * leading slash, directory, trailing slash, filename, extension) is
 * 259 Unicode words.  We use this value.
 */
#ifndef HFAT_MAX_PATH_UTF16
  #define HFAT_MAX_PATH_UTF16 (259 + 1) /* Unicode words count, +1 for NULL */
#endif

/* UTF8 chars count, each UTF16 can be at most 3 UTF8s, and +1 for NULL. */
#define HFAT_MAX_PATH_UTF8 (((HFAT_MAX_PATH_UTF16 - 1) * 3) + 1)

/* To be compatible with others like Windows, we are limiting our
   max path length to 255 UTF-16s, and + 1 for NULL */
#ifndef HFAT_MAX_PATH_UTF16_ALLOW
  #define HFAT_MAX_PATH_UTF16_ALLOW (255 + 1)
#endif

#if HFAT_MAX_PATH_UTF8 > FS_PATH_MAX
#error "EFS filename length (FS_PATH_MAX) is too restrictive for FAT."
#endif

/* Sanity check to make sure that ALLOW'ed value is less than MAX. */
#if HFAT_MAX_PATH_UTF16_ALLOW > HFAT_MAX_PATH_UTF16
#error "HFAT_MAX_PATH_UTF16_ALLOW cannot be > HFAT_MAX_PATH_UTF16"
#endif

/* Maximum length of a full pathname , not including the trailing '\0'
 * character. The path also contains the leading drive-no like a:/
 * The unit here is character-symbols. */
#define HFAT_MAX_PATH_LEN_UTF8 (HFAT_MAX_PATH_UTF16_ALLOW-1)

/* Maximum length of the 'name' component in the filepath. Deduct 3 for
 * for (drive-letter, colon, trailing-slash) like a:/
 * The unit here is character-symbols. */
#define HFAT_MAX_NAME_LEN_UTF8 (HFAT_MAX_PATH_LEN_UTF8-3)

/* Global variable that holds UTF-16 name */
static UTF16 utf16_name[HFAT_MAX_PATH_UTF16];

static UTF16 utf16_name1[HFAT_MAX_PATH_UTF16];
static UTF16 utf16_name2[HFAT_MAX_PATH_UTF16];
static uint8 utf16_name1_inuse = 0, utf16_name2_inuse = 0;

/* Global variable that holds UTF-8 name */
static char utf8_name[HFAT_MAX_PATH_UTF8];
static uint8 utf8_name_inuse = 0;

/* HFAT directory iterator states */
enum iterator_state
{
  ITER_STATE_DOT,
  ITER_STATE_DOT_DOT,
  ITER_STATE_NORMAL
};

/* HFAT directory iterator object */
struct hfat_iterator
{
  unsigned int index;         /* to know the exact position in the iterator */

  FN_WFIND find;                /* Structure to hold f_find stats */

  /* In each iterator, we store the whole directory name with "<slash>*.*"
   * appended.   The "+1" is a lint tax */
  char dirname[HFAT_MAX_PATH_UTF8 + 4 + 1];

  unsigned int available;

  int state;
} hfat_iterators[FS_MAX_ITERATORS];

/* Mark iterator as unused and available */
static void
reset_iterator (struct hfat_iterator *iter)
{
  iter->index = 0;
  iter->available = 1;
}

/* HFAT deals in file pointers, while EFS uses integer file
 * descriptors.  This table stores the file pointers for each
 * externally-visible active file descriptor.  One entry is used for
 * each open file, and NULL is stored in the free slots. */
struct fd_translator
{
  F_FILE *fp;                   /* HFAT file pointer */
};
static struct fd_translator fd_table[F_MAXFILES];

/* Map an EFS file descriptor to HFAT file handle */
static F_FILE *
fd_to_file (int fd)
{
  /* This suggests a severe internal failure, since fd's are 'trusted'
   * and not user-visible.  A Fatal stop is appropriate.  */
  if ((fd < 0) || (fd >= F_MAXFILES))
    FS_ERR_FATAL ("fd out of range %d\n", fd, 0, 0);

  return fd_table[fd].fp;
}

/* The file handled associated with this fd is finished */
static void
reset_fd (int fd)
{
  if ((fd < 0) || (fd >= F_MAXFILES))
    FS_ERR_FATAL ("fd out of range %d\n", fd, 0, 0);

  fd_table[fd].fp = NULL;
}

/* Figure out which hfat device number to use based on the DOS-style
 * drive letter ("A:" = 0). */
static int
hfat_driveno (const char *name)
{
  if (name[1] == ':') {
    if (name[0] >= 'A' && name[0] <= 'Z')
      return name[0] - 'A';
    if (name[0] >= 'a' && name[0] <= 'z')
      return name[0] - 'a';
  }

  FS_ERR_FATAL ("Invalid drive letter used for hfat mount", 0, 0, 0);
  /* Eliminate compier warning. */
  return 0;                     /* unreached */
}

/* Map a given HFAT error value to an approximate EFS2 error code */
static int
hfat_to_efs_err (unsigned int err)
{
//  if (err)
//    printf ("hfat_to_efs_err(%d)!\n", err), fflush (stdout);
  switch (err)
  {
    case F_NO_ERROR:
      return 0;                 /* success */
    case F_ERR_INVALIDDIR:
      return -ENOTDIR;
    case F_ERR_INVALIDNAME:
      return -EINVAL;
    case F_ERR_NOTFOUND:
      return -ENOENT;
    case F_ERR_DUPLICATED:
      return -EEXIST;
    case F_ERR_TOOLONGNAME:
      return -ENAMETOOLONG;
    case F_ERR_INVALIDDRIVE:
    case F_ERR_CARDREMOVED:
    case F_ERR_NOTFORMATTED:
      return -ENODEV;
    case F_ERR_BUSY:
    case F_ERR_DRVALREADYMNT:
      return -EBUSY;
    case F_ERR_LOCKED:
      return -ETXTBSY;
    case F_ERR_ALLOCATION:
    case F_ERR_NOMOREENTRY:
      return -ENOMEM;
    case F_ERR_ACCESSDENIED:
      return -EACCES;
    case F_ERR_NOTOPEN:
      return -EBADF;
    case F_ERR_EOF:
      return -EEOF;
    case F_ERR_NOTUSEABLE:
      return -ESPIPE;
    case F_ERR_NOTEMPTY:
      return -ENOTEMPTY;
    case F_ERR_INVALIDSECTOR:
      return -EINVAL;
    case F_ERR_ISDIR:
      return -EISDIR;

    case F_ERR_RESERVED:
    case F_ERR_INITFUNC:
    case F_ERR_READ:
    case F_ERR_WRITE:
    case F_ERR_WRITEPROTECT:
    case F_ERR_INVFATTYPE:
    case F_ERR_MEDIATOOSMALL:
    case F_ERR_MEDIATOOLARGE:
    case F_ERR_NOTSUPPSECTORSIZE:
    case F_ERR_UNKNOWN:
    case F_ERR_DELFUNC:
    case F_ERR_NOTFORREAD:
      return -EUNKNOWN_HFAT;    /* catch-all ambiguous error */
  }
  return -EINVAL;
}

/* Start the filesystem (called during mount). */
static int
e_hfat_start (const char *name, const void *hdev)
{
  int ret;
  int driveno = hfat_driveno (name);

  /* Attempt to initialize the HFAT volume.  Reads the partition table
   * and verifies the formatting. */
  ret = f_initvolume (driveno, hfat_device_init, hdev);

  /* If that failed, we can not mount the volume */
  if (ret)
  {
    /* HFAT leaves the volume allocated, so we must delete it
       manually.  Discard the return value from f_delvolume() because
       the f_initvolume() error cause is more interesting. */
    (void) f_delvolume (driveno);
  }

  return hfat_to_efs_err (ret);
}

/* Stop HFAT operation on this volume during unmounting */
static int
e_hfat_stop (const char *name)
{
  int ret;

  ret = f_delvolume (hfat_driveno (name));

  return hfat_to_efs_err (ret);
}

/* Fill in EFS mode bits based on FAT attribute flags */
static void
mode_from_attr (fs_mode_t * mode, unsigned char attr)
{
  *mode = S_IRUSR | S_IRGRP | S_IROTH;  /* All files are readable */

  /* If the file is not READ-ONLY, then set ALL the write perm flags */
  if (!(attr & F_ATTR_READONLY))
    *mode |= S_IWUSR | S_IWGRP | S_IWOTH;

  if (attr & F_ATTR_DIR)
    *mode |= S_IFDIR;           /* Directory */

  /* Anything other than Directories and Volume labels are regular */
  if (!(attr & (F_ATTR_DIR | F_ATTR_VOLUME)))
    *mode |= S_IFREG;

  if (attr & F_ATTR_HIDDEN)
    *mode |= S_ISVTX;           /* Hidden file = "Sticky"  */

  /* These flags are also available, but disregarded here :
     F_ATTR_ARC
     F_ATTR_SYSTEM
     F_ATTR_LFN
   */
}

/* base_stat:
 * Used to determine the fundamental type of the file entry.
 */
static int
e_hfat_base_stat (const char *name, fs_mode_t * mode)
{
  int ret;
  F_STAT fstat;

  /* convert the given UTF-8 path to UTF-16, bail if we can't */
  ret = fs_convert->to_utf16 (name, utf16_name, HFAT_MAX_PATH_UTF16_ALLOW);
  if (ret != ENOERR) {
    return ret;
  }

  /* Fetch file attributes from HFAT */
  ret = f_wstat (utf16_name, &fstat);

  if (ret)
    return hfat_to_efs_err (ret);

  mode_from_attr (mode, fstat.attr);
  return 0;
}

/* Open a file by full pathname */
static int
e_hfat_open (const char *name, int mode)
{
  FN_FILE *fp = NULL;
  int fd;
  UTF16 wmode[3];
  int ret;

  /* convert the given UTF-8 path to UTF-16, bail if we can't */
  ret = fs_convert->to_utf16 (name, utf16_name, HFAT_MAX_PATH_UTF16_ALLOW);
  if (ret != ENOERR) {
    return ret;
  }

  switch (mode)
  {
    case O_RDONLY:
      wmode[0] = 'r';
      wmode[1] = 0;
      fp = f_wopen (utf16_name, wmode);
      break;
    case O_RDWR:
      wmode[0] = 'r';
      wmode[1] = '+';
      wmode[2] = 0;
      fp = f_wopen (utf16_name, wmode);
      break;
    case (O_CREAT | O_EXCL | O_RDWR):
      wmode[0] = 'w';
      wmode[1] = '+';
      wmode[2] = 0;
      fp = f_wopen (utf16_name, wmode);
      break;
    default:
      /* The modes are from a restricted set used by extfs */
      EFS_BAB_DEBUG_PRINTF ("Invalid internal mode used");
      break;
  }

  /* We don't get any reason (from HFAT) for why an f_open() failed,
     so all errors (null file pointer) are treated identically. */
  if (!fp)
    return -ENOENT;

  /* Store this fp in our table, converting it to an integer fd index */
  for (fd = 0; fd < F_MAXFILES; fd++)
  {
    if (fd_table[fd].fp == NULL)        /* free to use? */
    {
      fd_table[fd].fp = fp;     /* map it */
      return fd;                /* Return fd */
    }
  }
  /* We could not find any open fd slot to store this fp, so this
     seems the most appropriate error.  Increase F_MAXFILES, or open
     fewer files.  Regrettably, we have to close the file as well. */
  (void) f_close (fp);
  return -EMFILE;               /* no room at the inn */
}

/* Seek to a specific position in this open file descriptor */
static int
e_hfat_lseek (int fd, fs_size_t pos, int mode)
{
  int ret;
  int whence;
  fs_size_t file_pos;
  F_FILE *fp;

  fp = fd_to_file (fd);

  // printf ("e_hfat_lseek (%d)\n", pos); fflush(stdout);

  /* In append mode, all writes always seek to the end of the file.
   * In normal writes, use the given pos. */
  if (mode & O_APPEND) {
    file_pos = 0;
    whence = SEEK_END;
  }
  else {
    file_pos = pos;
    whence = SEEK_SET;
  }

  ret = f_seek (fp, file_pos, whence);

//  if (ret)
//    printf ("Seek failed!\n"), fflush(stdout);
  return hfat_to_efs_err (ret);
}

static int
e_hfat_read (int fd, void *buf, fs_size_t count)
{
  F_FILE *fp;

  if (count > FS_MAX_FAT_TRANSACTION)
    count = FS_MAX_FAT_TRANSACTION;

  fp = fd_to_file (fd);

  //printf ("e_hfat_read (%d)\n", count); fflush(stdout);
  return f_read (buf, 1, count, fp);
}

static int
e_hfat_write (int fd, const void *buf, fs_size_t count)
{
  F_FILE *fp;

  if (count > FS_MAX_FAT_TRANSACTION)
    count = FS_MAX_FAT_TRANSACTION;

  fp = fd_to_file (fd);

  //printf ("e_hfat_write (%d)\n", count); fflush(stdout);
  return f_write (buf, 1, count, fp);
}

static int
e_hfat_close (int fd)
{
  int ret;
  F_FILE *fp;

  fp = fd_to_file (fd);

  ret = f_close (fp);

  reset_fd (fd);

  return hfat_to_efs_err (ret);
}

static int
e_hfat_mkdir (const char *name)
{
  int ret;

  /* convert the given UTF-8 path to UTF-16, bail if we can't */
  ret = fs_convert->to_utf16 (name, utf16_name, HFAT_MAX_PATH_UTF16_ALLOW);
  if (ret != ENOERR) {
    return ret;
  }

  ret = f_wmkdir (utf16_name);

  return hfat_to_efs_err (ret);
}

static int
e_hfat_chmod (const char *name, fs_mode_t mode)
{
  int ret;
  F_STAT fstat;

  /* convert the given UTF-8 path to UTF-16, bail if we can't */
  ret = fs_convert->to_utf16 (name, utf16_name, HFAT_MAX_PATH_UTF16_ALLOW);
  if (ret != ENOERR) {
    return ret;
  }

  /* Get the current DIR/File/Volume attributes */
  ret = f_wstat (utf16_name, &fstat);
  if (ret)
    return hfat_to_efs_err (ret);

  /* Fail if any illegal bits are attempted.  Note that even some
   * 'legal' bits will be ignored on FAT. */
  if (mode & ~07777)
    return -EINVAL;

  if ((mode & (S_IWUSR | S_IWGRP | S_IWOTH)) == 0)
    fstat.attr |= F_ATTR_READONLY;    /* Not writable */
  else
    fstat.attr &= ~F_ATTR_READONLY;   /* Writeable */

  if (mode & S_ISVTX)
    fstat.attr |= F_ATTR_HIDDEN;      /* Hidden */
  else
    fstat.attr &= ~F_ATTR_HIDDEN;     /* Not hidden */

  ret = f_wsetattr (utf16_name, fstat.attr);

  return hfat_to_efs_err (ret);
}

static int
e_hfat_rmdir (const char *name)
{
  int ret;

  /* convert the given UTF-8 path to UTF-16, bail if we can't */
  ret = fs_convert->to_utf16 (name, utf16_name, HFAT_MAX_PATH_UTF16_ALLOW);
  if (ret != ENOERR) {
    return ret;
  }

  ret = f_wrmdir (utf16_name);

  return hfat_to_efs_err (ret);
}

static int
e_hfat_unlink (const char *name)
{
  int ret;

  /* convert the given UTF-8 path to UTF-16, bail if we can't */
  ret = fs_convert->to_utf16 (name, utf16_name, HFAT_MAX_PATH_UTF16_ALLOW);
  if (ret != ENOERR) {
    return ret;
  }

  ret = f_wdelete (utf16_name);

  return hfat_to_efs_err (ret);
}

static int
e_hfat_rename (const char *oldname, const char *newname)
{
  int ret;

 (void)utf16_name1_inuse;
 (void)utf16_name2_inuse;
  ASSERT (utf16_name1_inuse == FALSE);
  ASSERT (utf16_name2_inuse == FALSE);

  /* Lock buffers */
  utf16_name1_inuse = TRUE;
  utf16_name2_inuse = TRUE;

  /* convert the given UTF-8 path to UTF-16, bail if we can't */
  ret = fs_convert->to_utf16 (oldname, utf16_name1, HFAT_MAX_PATH_UTF16_ALLOW);
  if (ret != ENOERR) {
    goto Cleanup;
  }

  /* convert the given UTF-8 path to UTF-16, bail if we can't */
  ret = fs_convert->to_utf16 (newname, utf16_name2, HFAT_MAX_PATH_UTF16_ALLOW);
  if (ret != ENOERR)
  {
    goto Cleanup;
  }

  ret = f_wrename (utf16_name1, utf16_name2);

  ret = hfat_to_efs_err (ret);

  goto Cleanup;

Cleanup:
  /* Free buffers */
  utf16_name1_inuse = FALSE;
  utf16_name2_inuse = FALSE;
  return ret;
}

static int
e_hfat_statvfs (const char *name, struct fs_statvfs *buf)
{
  int driveno = hfat_driveno (name);
  F_SPACE space;
  int ret;

  ret = f_getfreespace (driveno, &space);

  if (ret)
    return hfat_to_efs_err (ret);
  else
  {
    int blocksize = 512;        /* always */
    unsigned long long bytes;   /* This can get very big */
    unsigned long long blocks;  /* less big, but still big */
    fs_fsblkcnt_t max_blocks = 0xFFFFFFFF; /* The most blocks possible */

    /* HFAT returns sizes in bytes.. we need blocks */
    bytes = space.free_high;    /* Get it into 64 bits first */
    bytes <<= 32;               /* Now shift */
    bytes += space.free;        /* Add in low bits */
    blocks = bytes / blocksize;
    buf->f_bfree = (blocks > max_blocks) ? max_blocks : (fs_fsblkcnt_t) blocks;

    bytes = space.total_high;
    bytes <<= 32;
    bytes += space.total;
    blocks = bytes / blocksize;
    buf->f_blocks = (blocks > max_blocks) ? max_blocks : (fs_fsblkcnt_t)blocks;

    buf->f_bavail = buf->f_bfree; /* same */

    buf->f_bsize = blocksize;
    buf->f_files = FS_INVALID_FSFILCNT;
    buf->f_ffree = FS_INVALID_FSFILCNT;
    buf->f_favail = FS_INVALID_FSFILCNT;
    buf->f_fsid = 1;
    buf->f_flag = 0;
    buf->f_namemax = HFAT_MAX_NAME_LEN_UTF8;
    buf->f_balloc = FS_INVALID_FSBLKCNT;
    buf->f_hardalloc = FS_INVALID_HARDALLOC;
    buf->f_maxwrite = FS_MAX_FAT_TRANSACTION;

    buf->f_pathmax = HFAT_MAX_PATH_LEN_UTF8;
    buf->f_is_case_sensitive = 0; /* Filepath is NOT case-sensitive. */
    buf->f_is_case_preserving = 1; /* Filepath is case-preserved. */
    buf->f_max_file_size = FS_MAX_FILE_SIZE;
    buf->f_max_file_size_unit = 1;
    buf->f_max_open_files = F_MAXFILES;
    buf->f_name_rule = FS_FILENAME_RULE_FAT;
    buf->f_name_encoding = FS_FILENAME_ENCODING_UTF8;

    return 0;
  }
}

static int
hfat_update_iterator (struct hfat_iterator *iter)
{
  int ret;

  if (iter->index == 0)         /* new iterator, get first entry */
  {
    ASSERT (utf16_name2_inuse == FALSE);
    /* Lock buffer */
    utf16_name2_inuse = TRUE;

    /* convert the given UTF-8 path to UTF-16, bail if we can't */
    ret =  fs_convert->to_utf16 (iter->dirname, utf16_name2,
            HFAT_MAX_PATH_UTF16_ALLOW);
    if (ret != ENOERR)
    {
      /* Free buffer and return */
      utf16_name2_inuse = FALSE;
      return ret;
    }
    ret = fm_wfindfirst (utf16_name2, &iter->find);
    /* Free buffer */
    utf16_name2_inuse = FALSE;
  }
  else
    ret = f_wfindnext (&iter->find);    /* advance to next entry */

  return ret;
}

static struct hfat_iterator *
get_free_iterator (void)
{
  int i;
  for (i = 0; i < FS_MAX_ITERATORS; i++)
  {
    if (hfat_iterators[i].available)
    {
      hfat_iterators[i].available = 0;
      return &hfat_iterators[i];
    }
  }
  /* Failed to find an available iterator */
  return NULL;
}

/* Open a directory iterator.
 * This function takes a directory name that begins with a drive letter.
 * Example: "a:/mystuff/musicdir" */
static void *
e_hfat_opendir (const char *name)
{
  unsigned int len;
  struct hfat_iterator *iter;

  len = strlen (name);

  /* We require a drive letter, colon, and leading slash */
  if (name[0] == '\0' || name[1] != ':' || name[2] != '/' || len < 3)
    return NULL;

  /* Don't count any trailing '/' in the length */
  while (name[len - 1] == '/')
    len--;

  /* If the directory name is longer than we expect, fail */
  if (len > HFAT_MAX_PATH_UTF8)
    return NULL;

  /* Grab an iterator from iterator pool */
  iter = get_free_iterator ();

  if (!iter)
  {
    EFS_BAB_DEBUG_PRINTF ("No iterators available");
    return NULL;
  }

  /* Initialize the iterator index to 0 so that we know that we just
     created the iterator */
  iter->index = 0;

  /* Initialize the dirname field of the iterator. We need this in readdir
     to pass to f_findfirst() and f_findnext() */
  fs_memscpy (iter->dirname, sizeof (iter->dirname), name, len);

  /* HFAT requires this to find all the files in the directory */
  strlcpy (iter->dirname + len, "/*.*", sizeof (iter->dirname) - len);

  /* The root directory in FAT does not contain '.' and '..'.  We need
     to fake them with these two special iterator states.  Look for
     any root directory name ("?:/") and start with these fake
     entries. */
  if (name[0] != 0 && name[1] == ':' && name[2] == '/' && name[3] == 0)
    iter->state = ITER_STATE_DOT;
  else
    iter->state = ITER_STATE_NORMAL;

  return iter;
}

static int
e_hfat_readdir (void *iterator, struct fs_dirent *dirent)
{
  int ret;
  struct hfat_iterator *iter;
  unsigned char attr;
  fs_mode_t *mode;

  (void)utf8_name_inuse;
  /* We already initialized the iterator in opendir, which is passed
     to us as first argument. */
  iter = (struct hfat_iterator *) iterator;

  switch (iter->state)
  {
    case ITER_STATE_DOT:       /* First fake root entry: "." */
      iter->state = ITER_STATE_DOT_DOT;
      strlcpy (dirent->d_name, ".", sizeof (dirent->d_name));
      break;

    case ITER_STATE_DOT_DOT:   /* Second fake root entry: ".." */
      iter->state = ITER_STATE_NORMAL;
      strlcpy (dirent->d_name, "..", sizeof (dirent->d_name));
      break;

    case ITER_STATE_NORMAL:
      ret = hfat_update_iterator (iter);

      if (ret)
        return hfat_to_efs_err (ret);   /* Enumeration is done */

      attr = iter->find.attr;
      mode = &dirent->d_stat.st_mode;
      mode_from_attr (mode, attr);

      /* Get the file size */
      dirent->d_stat.st_size = (unsigned long) iter->find.filesize;

      dirent->d_stats_present = FS_DIRENT_HAS_TYPE | FS_DIRENT_HAS_ST_SIZE;

      {
        /* Check of utf8_name_inuse and lock the buffer */
        ASSERT (utf8_name_inuse == FALSE);
        utf8_name_inuse = TRUE;

        /* The name we got back from HFAT is UTF 16 encoded.
           We need to change it to UTF-8 before returning */
        fs_convert->from_utf16 (iter->find.filename, utf8_name,
                                sizeof(utf8_name));

        strlcpy (dirent->d_name, utf8_name, sizeof (dirent->d_name));
        utf8_name_inuse = FALSE;
      }
      /* XXX: The name will be shortened if it doesn't fit into the buffer.
       * There isn't a clean way to distinguish between a single iteration
       * result being too long, and the iterator itself failing.  So, for
       * now, just return the truncated name.  FS_NAME_MAX should be
       * defined long enough to work with this filesystem. */

      /* Advance the iterator */
      iter->index++;
      break;

    default:
      FS_ERR_FATAL ("Invalid iterator state : %d", iter->state, 0, 0);
  }

  return 0;
}

static int
e_hfat_closedir (void *iterator)
{
  struct hfat_iterator *iter;

  iter = (struct hfat_iterator *) iterator;

  reset_iterator (iter);

  return 0;
}

static int
e_hfat_truncate (int fd, fs_size_t pos)
{
  int ret;
  FN_FILE *fp;

  fp = fd_to_file (fd);

  ret = f_ftruncate (fp, pos);

  if (ret)
    return hfat_to_efs_err (ret);

  return 0;
}

static int
e_hfat_utime (const char *path, const struct fs_utimbuf *times)
{
  int ret;
  uint16 dostime, dosdate, adosdate;
  uint32 mtime, atime;

  /* convert the given UTF-8 path to UTF-16, bail if we can't */
  ret = fs_convert->to_utf16 (path, utf16_name, HFAT_MAX_PATH_UTF16_ALLOW);
  if (ret != ENOERR) {
    return ret;
  }

  /* Get new access and modification time. */
  if (times != NULL)
  {
    atime = times->actime;
    mtime = times->modtime;
  }
  else
  {
    atime = mtime = fs_time ();
  }

  /* Set time/date */
  dostime = fs_posix_to_fat_time (mtime);
  dosdate = fs_posix_to_fat_date (mtime);
  adosdate = fs_posix_to_fat_date (atime);
  ret = f_wsetaccmodtime (utf16_name, dostime, dosdate, adosdate);

  return hfat_to_efs_err (ret);
}

static int
e_hfat_getsize (int fd, uint32 *numbytes)
{
  int ret;
  F_FILE *fp;

  if ((fd < 0) || (fd > F_MAXFILES)) /* Invalid file descriptor */
    return -1;

  fp = fd_to_file (fd);

  ret = fn_getsize_from_fp (fp, numbytes);

  return hfat_to_efs_err (ret);
}

static int
e_hfat_getstat (const char *name, struct fs_stat *buf)
{
  int ret;
  uint16 *mode;
  F_STAT fstat;

  /* f_getattr does not work with root directories. */
  if (name[0] != 0 && name[1] == ':' && name[2] == '/' && name[3] == 0)
  {
    buf->st_mode = (buf->st_mode & ~S_IFMT) | S_IFDIR;
    buf->st_size = 0;           /* Root directory always has zero size */
    buf->st_ctime = 0;
    buf->st_mtime = 0;
    buf->st_atime = 0;

  }
  else
  {
    /* convert the given UTF-8 path to UTF-16, bail if we can't */
    ret = fs_convert->to_utf16 (name, utf16_name, HFAT_MAX_PATH_UTF16_ALLOW);
    if (ret != ENOERR) {
      return ret;
    }

    /* Find the attributes of the file */
    ret = f_wstat (utf16_name, &fstat);

    if (ret)
      return hfat_to_efs_err (ret);

    /* Translate the Attributes into mode */
    mode = &buf->st_mode;
    mode_from_attr (mode, fstat.attr);

    buf->st_size = fstat.filesize;
    buf->st_ctime = fs_fat_to_posix_time (fstat.modifiedtime,
                                        fstat.modifieddate);
    buf->st_mtime = buf->st_ctime;
    buf->st_atime = fs_fat_to_posix_time (0, fstat.lastaccessdate);
  }

  return 0;
}

/* Retrieves the volume name for the HFAT filesystem */
static int
e_hfat_get_volume_name (char *name, char *label, int len)
{
  int driveno = hfat_driveno (name);
  int ret;

  ret = f_getlabel (driveno, label, len);

  return hfat_to_efs_err (ret);
}

/* Sets the volume name for the HFAT filesystem */
static int
e_hfat_set_volume_name (const char *name, char *label)
{
  int driveno = hfat_driveno (name);
  int ret;

  ret = f_setlabel (driveno, label);

  return hfat_to_efs_err (ret);
}

static void
reset_fd_table (void)
{
  int i;
  struct fd_translator *fd_ptr = NULL;

  fd_ptr = &fd_table[0];

  for (i = 0; i < F_MAXFILES; i++)
  {
    fd_ptr->fp = NULL;
    fd_ptr++;
  }
}

void
fs_hfat_init (void)
{
  int i;

  hfat_init (); /* Initilialize HFAT globals. */

  reset_fd_table ();

  for (i = 0; i < FS_MAX_ITERATORS; i++)
    reset_iterator (&hfat_iterators[i]);
}

/* Format the media in drive number 'driveno'
   Returns 0 for success and error code for failure. */
int
fs_hfat_format_core (int driveno, const void *chdl)
{
  int ret;
  uint64 blocks = 0;
  uint32 bytes_per_block = 0;
  uint64 total_mb = 0;

  /* Unprotect memory
   * Due to the time taken to format we do not grab the global lock
   * which allows other threads to operate concurrently if using flash.
   * In lieu of this we must first unprotect the memory segment before
   * we operate on the HFAT protected memory region. */
  fs_efs2_unprotect_memory ();

  /* We don't yet have an initialized drive. */
  ret = f_initvolume (driveno, hfat_device_init, chdl);

  /* An error indicating Not Formatted is OK */
  if ((ret != F_NO_ERROR) && (ret != F_ERR_NOTFORMATTED))
    goto cleanup;

  /* Now we have an initialized volume..  Get the size. */
  ret = coldplug_get_size ((void *)chdl, &bytes_per_block, &blocks);

  /* HFAT only works with block sizes of 512 bytes */
  if (bytes_per_block == 512)
    total_mb = (blocks / (1024 / bytes_per_block)) / 1024;
  else
    ret = F_ERR_INVALIDDRIVE;

  /* Look at media size to determine FAT type */
  if (ret == 0)
  {
    int ftype = total_mb <= 64 ? F_FAT12_MEDIA :
      (total_mb <= 2048 ? F_FAT16_MEDIA : F_FAT32_MEDIA);

    ret = f_format (driveno, ftype);
  }

  /* Discard the volume */
  (void) f_delvolume (driveno);

 cleanup:
  /* Protect memory again */
  fs_efs2_protect_memory ();

  return hfat_to_efs_err (ret);
}

int
fs_hfat_checkfat_core (int driveno, const void *chdl, int param,
       int *error_mask, void (*callback) (void *), void *cparam)
{
  int result;

  /* Unprotect memory
   * Due to the time taken to format we do not grab the global lock
   * which allows other threads to operate concurrently if using flash.
   * In lieu of this we must first unprotect the memory segment before
   * we operate on the HFAT protected memory region. */
  fs_efs2_unprotect_memory ();

  /* We don't yet have an initialized drive. */
  result = f_initvolume (driveno, hfat_device_init, chdl);
  if (result != F_NO_ERROR)
    goto cleanup;

  result = f_checkfat (driveno, param, error_mask, callback, cparam);

  /* Discard the volume */
  (void) f_delvolume (driveno);

 cleanup:
  /* Protect memory again */
  fs_efs2_protect_memory ();

  return result;
}

const struct fs_extfs_ops fs_hfat_ops = {
  e_hfat_start,
  e_hfat_stop,
  e_hfat_base_stat,
  e_hfat_getstat,
  e_hfat_getsize,
  e_hfat_open,
  e_hfat_lseek,
  e_hfat_read,
  e_hfat_write,
  e_hfat_chmod,
  e_hfat_close,
  e_hfat_mkdir,
  e_hfat_rmdir,
  e_hfat_unlink,
  e_hfat_rename,
  e_hfat_statvfs,
  e_hfat_opendir,
  e_hfat_readdir,
  e_hfat_closedir,
  e_hfat_truncate,
  e_hfat_utime,
  e_hfat_get_volume_name,
  e_hfat_set_volume_name
};
#else /* FEATURE_EFS_EXTFS_HFAT */
extern int __dont_complain_about_empty_file;
#endif
