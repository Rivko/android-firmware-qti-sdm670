/***********************************************************************
 * fs_sys_types.h
 *
 * Copyright (C) 2002-2010,2012, QUALCOMM Technologies, Inc.
 *
 * Type definitions for EFS2.
 *
 ***********************************************************************/

 /*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when          who     what, where, why
--------      ---     ------------------------------------------------------
05/07/14      kedara  Include com_dtypes.h instead of comdef.h
2012-08-22    rp      Remove FDI.
2012-03-20    wek     Cleanup customer header file inclusion.
2010-07-12    rp      Added defines for major and minor number for API files.
2010-05-10    dks     Added SPI-NOR and RMTS device types.
2010-05-07    rp      Added Ram file system.
2010-03-19    yog     Added support for files having max filesize upto 4GB.
2010-02-08    rp      CMI compliance, remove #ifdef from API files.
2010-01-19    vr      Added efs_sync_no_wait and efs_sync_get_status APIs.
2009-11-16    rp      Give friendly names to FAT partitions in boot context.
2009-11-04    dsk     Added fs_dev_type enum for device types.
2009-08-10    wek     Fix comments for doxygen.
2009-07-06    ebb     Added 'extern int efs_errno' if compiling in boot.
2009-07-01    wek     Added BREW group ID define.
2009-06-12    ebb     Moved rex_xxx calls to fs_osal.h abstraction.
2009-05-22    rp      Align buffers passed to flash on a 128-byte boundary.
2009-05-01    ebb     Added struct efs_romfs_node to expose romfs externally
2009-04-03    wek     Added fs_utimbuf structure for efs_utime() API.
2009-04-01    yg      Moved out private config item to its private header
2008-12-11    ebb     Fixed length of NAMED_VNODE_SIZE_SYMLINK.
2008-11-20    yg      Make FS_DM_ALIGN_BYTES overridable.
2008-09-16    yog     Remove trailing commas from the enum declaration.
2008-05-23    rp      Introducing /inc folder as per new SCM.
2008-03-12    yg      Use _MSC_VER instead of T_WINNT.
2008-03-12    rp      Define FS_ALIGN for T_WINNT builds.
2008-02-19    rp      Fixed doxygen compile error for using #define as text.
2007-12-18    rp      Documented the mode permission bits like UID, GID.
2007-12-07    rp      Enhancements to the statvfs struct.
2007-09-11    sh      Correct compiler/alignment flag use.
2007-09-10    rp      Fix documentation for FS_DM_ALIGN_BYTES.
2007-09-07    rp      Added FS_IS_QUOTA_RESERVED_GROUP from fs_buffer.h
2007-08-10    rp      Align buffers passed to flash on a 32-byte boundary.
2007-06-09    rp      Doxygen comments markup.
2006-12-21    s h     Implement FIFO and Socket file nodes.
2006-12-05    dlb     Add 'f_maxwrite' to statvfs structure.
2006-11-22    dlb     Define symbols for FS capabilities.
2006-11-10    sh      Added HIDDEN = S_ISVTX for FAT.
2006-11-10    sh      Added upper mode bits.
2006-09-25    dlb     Clarify comments.
2006-07-06    dlb     Create FS_OFFSET_APPEND.
2006-06-27    dlb     Pass stat through readdir.
2006-05-08    sh      Declared FS_INVALID_FSBLKCNT instead of '-1'
2006-05-07    sh      Declared FS_INVALID_HARDALLOC instead of '-1'
2006-04-13    dlb     Add uid/gid types.
2006-04-05    dlb     Move config entities out of this file into fs_config.h
2006-03-24    sh      Moved MAX mount counts into common header.
2006-01-27    sh      Renamed some Q&R defines to be more explanatory
2006-01-11    nrs     Fixed Copyright header
2005-12-23    nrs     Adjusted Max Item and Symlink sizes
2005-12-22    dlb     Extensions to statvfs results.
2005-12-09    dlb     Reduce item and symlink max to avoid problems.
2005-12-08     sh     Use rex_self() instead of rex_curr_task (for L4)
2005-11-12     sh     Use rex tcb err_num instead of global efs_errno
2005-10-25    nrs     Changed reservation_avail to reservation_alloc
2005-10-21    dlb     Mark unsigned constants as such.
2005-10-13    dlb     Clean up declarations.
2005-10-18    nrs     Fixed comments and defines for quotas and reservations
2005-10-13    dlb     Clean up declarations.
2005-10-12    nrs     Added Commenting for group structure
2005-08-31    nrs     Added invalid value for quotas and reservations
2005-08-30    dlb     Correct spelling of st_rdev field.
2005-08-16    dlb     Unify max item length defines.
2005-08-15     sh     Add device special files.
2005-08-04     sh     Expanded FS_NAME_MAX and FS_PATH_MAX.
2005-07-22    nrs     Added structure for group id
2005-07-20    nrs     Updated FS_UPPER_DATA_COUNT size for group ID info
2005-07-15    nrs     Added Group ID Max
2005-07-15    dlb     Mountpoint cleanup.
2005-07-13    nrs     Added Owner ID and Group ID to fs_stat structure
2005-07-05    dlb     Export a few additional fields in device info.
2005-01-04    dlb     Update copyright line.
2004-12-30    dlb     Remove infiltration of factory image code.
2004-10-07    dlb     Whitespace cleanup.
2004-03-23    dlb     Add Posix permission defines.
2004-01-14    adm     Increased the version number for factory start.
2003-10-13     gr     Increased FS_NAME_MAX so Java compliance tests would
                      pass.
2003-09-09    dlb     Factory image works with bitmap and tree.
2003-06-12    adm     Add support for factory start.
2002-08-20    adm     Created.

===========================================================================*/

#ifndef _FS_SYS_TYPES_H
#define _FS_SYS_TYPES_H

#include "com_dtypes.h"
#include "fs_config.h"

/** Identifies the major version number of this API file. */
#define VER_FS_SYS_TYPES_H_MAJOR     1
/** Identifies the minor version number of this API file. */
#define VER_FS_SYS_TYPES_H_MINOR     1

/* The type of an individual "file" is encoded in its mode.  This is the
 * field returned in the 'st_mode' structure on return from efs_stat() and
 * friends.  The values here that correlate with Posix are the same.
 * "Item" files are unique to EFS2. */

#define S_IFIFO  0010000        /**< FIFO */
#define S_IFCHR  0020000        /**< Character device */
#define S_IFDIR  0040000        /**< Directory */
#define S_IFBLK  0060000        /**< Block device */
#define S_IFREG  0100000        /**< Regular file */
#define S_IFLNK  0120000        /**< Symlink */
#define S_IFSOCK 0140000        /**< Socket */
#define S_IFITM  0160000        /**< Item File */
#define S_IFMT   0170000        /**< Mask of all values */

/* These macros can be used to test a given mode to determine its type.
 * Typical use will be.
 *
 *   struct fs_stat buf;
 *   result = efs_lstat (path, &buf);
 *   if (result != 0) ... Process error
 *   if (S_ISDIR (buf.st_mode))
 *     .. process directory
 *   else if (S_ISREG (buf.st_mode))
 *     .. process regular file
 *   ...
 *
 * S_IFSOCK & S_ISLNK are not in POSIX.1-1996, but in POSIX.1-2001
 */

#define S_ISFIFO(m)     (((m) & S_IFMT) == S_IFIFO)
#define S_ISCHR(m)      (((m) & S_IFMT) == S_IFCHR)
#define S_ISDIR(m)      (((m) & S_IFMT) == S_IFDIR)
#define S_ISBLK(m)      (((m) & S_IFMT) == S_IFBLK)
#define S_ISREG(m)      (((m) & S_IFMT) == S_IFREG)
#define S_ISLNK(m)      (((m) & S_IFMT) == S_IFLNK)
#define S_ISSOCK(m)     (((m) & S_IFMT) == S_IFSOCK)
#define S_ISITM(m)      (((m) & S_IFMT) == S_IFITM)

/** These types are not normal files, but 'special' handles for the OS */
#define S_ISSPECIAL(m) (S_ISCHR (m) ||  \
                        S_ISBLK (m) ||  \
                        S_ISFIFO (m) || \
                        S_ISSOCK (m))

/**
 * Posix permission bits.  Simple posix file permissions use a 3-level ACL,
 * with three privilege bits at each level.  The bits are read, write, and
 * execute.  Read and write have their obvious meaning for regular files.
 * Execute is currently not used by EFS2 (but could be checked).  For
 * directories, the meaning of the bits is a little different:
 *   - Read - Grants permission to open files in the directory.
 *   - Write - Grants permission to modify the "directory" (create, remove,
 *             or rename files)
 *   - Execute - Grants permission to search the directory (opendir).
 *
 * The three levels are "User", "Group", and "Other".  Currently, only
 * 'user' is defined in EFS2.  (Actually, none of these are checked).
 *
 * The values for these permissions are traditionally represented by octal
 * constants.  These defined values are a mixture of Posix and BSD
 * definitions.
 *
 *
 * FAT filesystems do not have Posix mode bits, so some translation is
 * required:
 *
 * FAT's "READONLY" attribute is represented by no write permissions
 * for all of user/group/other.  If one or more of these three has
 * write permission, then the file is not READONLY.
 *
 * FAT's "HIDDEN" attribute is uncomfortably shared with the Sticky
 * bit, S_ISVTX for now.
 *
 * FAT attributes should be avoided as much as possible, as
 * interpretations may change in future EFS versions.
 */

/* "User" permissions. */
#define S_IRUSR         0400    /**< User has Read permission. */
#define S_IWUSR         0200    /**< User has Write permission. */
#define S_IXUSR         0100    /**< User has eXecute permission. */

/* "Group" permissions. */
#define S_IRGRP         0040    /**< Group has Read permission. */
#define S_IWGRP         0020    /**< Group has Write permission. */
#define S_IXGRP         0010    /**< Group has eXecute permission. */

/* "Other" permissions. */
#define S_IROTH         0004    /**< Others has Read permission. */
#define S_IWOTH         0002    /**< Others has Write permission. */
#define S_IXOTH         0001    /**< Others has eXecutepermission. */

/* High bits. */
#define S_ISUID        04000    /**< Set UID on execution. */
#define S_ISGID        02000    /**< Set GID on execution. */
#define S_ISVTX        01000    /**< Sticky (HIDDEN attribute in HFAT) */

/* BSD definitions. */
#define S_IREAD         S_IRUSR
#define S_IWRITE        S_IWUSR
#define S_IEXEC         S_IXUSR

/* Common combinations of bits. */
#define ACCESSPERMS     0777
#define ALLPERMS        0777
#define DEFFILEMODE     0666

/* Type declarations. */
typedef int32  fs_ssize_t;
typedef uint32 fs_size_t;
typedef uint16 fs_dev_t;
typedef uint16 fs_mode_t;
typedef int32  fs_off_t;
typedef long long  fs_off64_t;
typedef uint32 fs_time_t;
typedef uint32 fs_fsblkcnt_t;
typedef uint32 fs_fsfilcnt_t;
typedef uint32 fs_inode_t;
typedef uint8  fs_entry_type_t;
typedef uint8  fs_nlink_t;
typedef uint32 fs_devspecial_t;
typedef uint16 fs_gid_t;
typedef uint16 fs_uid_t;

#define FS_INVALID_DEV          0xFFFFU
#define FS_INVALID_INODE        0xFFFFFFFFU
#define FS_INVALID_HARDALLOC    0xFFFFFFFFU
#define FS_INVALID_FSBLKCNT     0xFFFFFFFFU
#define FS_INVALID_FSFILCNT     0xFFFFFFFFU
#define FS_INVALID_MAXWRITE     0xFFFFFFFFU

/** Maximum value of the quota field (also the default) */
#define FS_MAX_RES_QUOTA        0xFFFFFFFFU

/* List of assigned group IDs */
/** GID used as "group zero", the default and immutable group */
#define FS_GROUP_ZERO           0

/** GID Reserved for future use. */
#define FS_GROUP_RESERVED       1

/** GID used by BREW, primarily for quota and reservations. */
#define FS_GROUP_BREW           2

/** UID is present, but unused.  We use this for the default UID */
#define FS_CURR_UID             0

/** Each Group uses this many upper data fields of the superblock */
#define FS_FIELDS_PER_GROUP     3

/** This is the first upper data field used by Q&R group information */
#define FS_GROUP_INFO_START     8

/**
 * The maximum number of Groups that can have quotas and reservations.
 * The superblock constrains this.
 */
#define FS_QR_GROUP_COUNT       8
#define FS_IS_QUOTA_RESERVED_GROUP(gid) ((gid > 0) && \
    (gid <= FS_QR_GROUP_COUNT))


/**
 * How many fields (uint32) do we store in the superblock on behalf of the
 * upper layer of code?
 */
#define FS_UPPER_DATA_COUNT     (FS_GROUP_INFO_START + \
    (FS_QR_GROUP_COUNT * FS_FIELDS_PER_GROUP))

/** Special file offsets to indicate special behavior. */
#define FS_OFFSET_APPEND        (-1)

/* Time structure to hold access and modification time, time representation is
 * the same as POSIX, seconds since Jan 1st, 1970. */
struct fs_utimbuf {
  fs_time_t actime;     /**< Access time. */
  fs_time_t modtime;    /**< Modification time. */
};

struct fs_dir_data;
typedef struct fs_dir_data EFSDIR;

/**
 * Structure for information filled in by the efs_stat(), efs_fstat(), and
 * efs_lstat() calls.  Not all fields will be meaningful on all
 * filesystems, but should have sane values. */
struct fs_stat {
  fs_dev_t   st_dev;       /**< Device ID.                   */
  fs_inode_t st_ino;       /**< Inode number.                */
  fs_mode_t  st_mode;      /**< File mode.                   */
  fs_nlink_t st_nlink;     /**< Number of links.             */
  fs_size_t  st_size;      /**< File size in bytes.          */
  unsigned long st_blksize;
  unsigned long st_blocks;
  fs_time_t  st_atime;     /**< Time of last access.         */
  fs_time_t  st_mtime;     /**< Time of last modification.   */
  fs_time_t  st_ctime;     /**< Time of last status change.  */
  fs_devspecial_t st_rdev; /**< Major & Minor device number. */
  uint16     st_uid;       /**< Owner ID.                    */
  uint16     st_gid;       /**< Group ID.                    */
};

/**
 * Bit definitions for each of the stat field members.  Used for readdir to
 * convey partial stat information.  'FS_DIRENT_HAS_ST_MODE' indicates the
 * the entire st_mode field is valid.  'FS_DIRENT_HAS_TYPE' indicates the
 * the file type bits are valid (S_IFMT bits). */
#define FS_DIRENT_HAS_ST_DEV        (1 << 1)
#define FS_DIRENT_HAS_ST_INO        (1 << 2)
#define FS_DIRENT_HAS_ST_MODE       (1 << 3)
#define FS_DIRENT_HAS_ST_NLINK      (1 << 4)
#define FS_DIRENT_HAS_ST_SIZE       (1 << 5)
#define FS_DIRENT_HAS_ST_BLKSIZE    (1 << 6)
#define FS_DIRENT_HAS_ST_BLOCKS     (1 << 7)
#define FS_DIRENT_HAS_ST_ATIME      (1 << 8)
#define FS_DIRENT_HAS_ST_MTIME      (1 << 9)
#define FS_DIRENT_HAS_ST_CTIME      (1 << 10)
#define FS_DIRENT_HAS_ST_RDEV       (1 << 11)
#define FS_DIRENT_HAS_ST_UID        (1 << 12)
#define FS_DIRENT_HAS_ST_GID        (1 << 13)
#define FS_DIRENT_HAS_TYPE          (1 << 14)

/**
 * Convenience macro for testing masks.  Verify that in 'value', at least
 * 'required' bits are set.  This macro evaluates its second argument more
 * than once. */
#define FS_MASK_CHECK(value, required) \
  (((value) & (required)) == (required))

/**
 * Posix requires that this structure contain at least 'd_name', but it is
 * allowed to contain more.  Some filesystems have more information
 * available, whereas others make that kind of lookup expensive.  We add
 * two fields here, d_stat is a regular stat structure.  'd_stats_present"
 * is a bitmask of the above masks for a given implementation to define
 * which fields it has filled in.
 *
 * Note that any code that uses readdir must be prepared for
 * d_stats_present to be zero, and to call lstat for each entry.  These
 * values are generally only implemented for filesystem types where the
 * information is readily available, or getting it through lstat is very
 * inefficient. */
struct fs_dirent {
  char           d_name[FS_NAME_MAX+1];
  struct fs_stat d_stat;
  uint32         d_stats_present;
};


/* This is the maximum file-size of a single file. */
#define FS_MAX_FILE_SIZE      0xFFFFFFFF

/* Filename rules. */
enum fs_filename_rule
{
  /* In this rule only the '/' character is not allowed, all other characters
     are allowed. '\0' is used to mark the end of the string. */
  FS_FILENAME_RULE_8BIT_RELAXED     = 1,

  /* In this rule all characters are allowed except these characters in the box
     [:*?<>/\"]. Also the 0xE5 is not allowed. '\0' is used to mark the end of
     the string.  There are also numerous rules like filename can't be all
     spaces, filename cannot end with a period, also some MS-DOS device names
     are prohibited like aux, con, nul, prn, com, lpt. Consult FAT spec from
     Microsoft for more information. */
  FS_FILENAME_RULE_FAT              = 2,

  /* FDI file system support is deprecated. So this enum is left as is to
     to preserve the API behaviour and is not used. */
  FS_FILENAME_RULE_FDI              = 3
};

/* Filename encoding schemes. */
enum fs_filename_encoding
{
  FS_FILENAME_ENCODING_8BIT     = 1,  /*< Any string of 8-bit characters.    */
  FS_FILENAME_ENCODING_UTF8     = 2   /**< Valid UTF-8 encoding is required. */
};

struct fs_statvfs {
  unsigned long f_bsize;   /**< Fundamental file system block size        */
  fs_fsblkcnt_t f_blocks;  /**< Total number of blocks in file system     */
  fs_fsblkcnt_t f_bfree;   /**< Total number of free blocks               */
  fs_fsblkcnt_t f_bavail;  /**< Number of free blocks available           */
  fs_fsfilcnt_t f_files;   /**< Total number of file serial numbers       */
  fs_fsfilcnt_t f_ffree;   /**< Total number of free file serial numbers  */
  fs_fsfilcnt_t f_favail;  /**< Number of file serial numbers available   */
  unsigned long f_fsid;    /**< File system ID                            */
  unsigned long f_flag;    /**< Bit mask of f_flag values                 */
  /** Maximum filename length, in units of character-symbols. The number of
    * bytes needed to represent a character will vary depending upon the
    * filename encoding scheme, For instance, In UTF8 encoding scheme,
    * representing a single character could take anywhere between 1 to 4 bytes.
    */
  unsigned long f_namemax;
  unsigned long f_maxwrite; /**< Maximum number of bytes in a single write */
  fs_fsblkcnt_t f_balloc;  /**< Blocks allocated in the general pool.     */
  fs_fsblkcnt_t f_hardalloc; /**< Hard allocation count.  Expensive, so
                                not usually computed. */

  /** Users can use this #define HAVE_EFS_STATVFS_ENHANCEMENTS to figure out
    * if a given EFS-VU has the following enhanced members or not. */

  /** Maximum path length, excluding trailing NULL. The unit here is in terms
    * character-symbols.  The number of bytes needed to represent a character
    * will vary depending upon the filename encoding scheme, For instance,
    * In UTF8 encoding scheme, representing a single character could take
    * anywhere between 1 to 4 bytes. */
  unsigned long f_pathmax;
  unsigned long f_is_case_sensitive; /**< Is Path case-sensitive? 1->Yes     */
  unsigned long f_is_case_preserving; /**< Is Path case-preserved? 1->Yes    */

  /** Max filesize in units mentioned by the member f_max_file_size_unit .*/
  unsigned long f_max_file_size;
  /** Multiply with the above member f_max_file_size. */
  unsigned long f_max_file_size_unit;

  /** How many files can be opened simultaneously for this file-system?
    * This member tells how many files can be kept opened for one particular
    * file-system, but there is a global-limit on how many files can be kept
    * opened simultaneously across all file-systems, which is configured by
    * the FS_MAX_DESCRIPTORS. */
  unsigned long f_max_open_files;

  enum fs_filename_rule         f_name_rule;     /**< Filenaming rules.      */
  enum fs_filename_encoding     f_name_encoding; /**< Encoding scheme.       */
};

/**  Device types. */
enum fs_dev_type
{
  FS_DEV_TYPE_UNKNOWN,
  FS_DEV_TYPE_EFS_NOR,
  FS_DEV_TYPE_EFS_NAND,
  FS_DEV_TYPE_EFS_MNAND,
  FS_DEV_TYPE_EFS_RAMFS,
  FS_DEV_TYPE_EFS_RMTS,
  FS_DEV_TYPE_EFS_SPI_NOR
};

struct fs_device_attr
{
  /* Query operations. */
  int device_maker_id;
  int device_id;
  int block_count;
  int block_size;
  int page_size;
  int total_page_size;
  byte device_type;
  uint32 total_blocks;
  uint32 bus_width;
  char device_name[FS_NAME_MAX+1];
};

struct fs_factimage_read_info {
  int8  info_cluster_sent;    /**< 0 indicates if info_cluster was not sent,
                                 else 1 */
  int16   cluster_map_seqno;  /**< Sequence number of cluster map pages */
  uint32 cluster_data_seqno;  /**< Sequence number of cluster data pages */
  int8   stream_state;        /**< 0 indicates no more data to be sent,
                               * otherwise set to 1*/
};

struct fs_group_info {
  uint32 quota_size;         /**< Size limit that can be written to the group*/
  uint32 reservation_size;   /**< Number of bytes reserved by the group */
  uint32 space_used;         /**< Number of bytes that have been written to the
                                group. This number can exceed reservation size
                                but never quota size */
};

/**
 * This is the struct type of a single romfs file. An array of these may be
 * declared and passed to mount to make the files available at runtime.
 *
 * extern const char hash_data[];
 * struct efs_romfs_node rom_nodes[] =
 * {
 *   {"hash_data", hash_data, sizeof (hash_data)},
 *   {"text.txt", "plaintext\nline 2\n123", 20},
 *   {NULL, NULL, 0}
 * };
 *
 * efs_mount ("", "/mount_point", "romfs", 0, rom_nodes);
 */
struct efs_romfs_node {
  const char *name;          /**< Filename of the ROMFS file to be mounted */
  void *data;                /**< Data contained within the romfs file */
  uint32 length;             /**< Length of data in the romfs file */
};

/**
 * Token type for efs_sync_no_wait API.
 */
typedef uint32 fs_sync_token_t;

/**
 * Sync result type for efs_sync_get_status API.
 */
enum fs_sync_result
{
  FS_SYNC_PENDING,
  FS_SYNC_COMPLETE
};

/**********************************************************************
 * These capability defines indicate whether or not various features are
 * present in this version of EFS2.  External code can conditionally
 * compile on these to avoid compilation dependencies on the filesystem.
 * Not are of concern to all clients, and an entity having a define here
 * does not necessarily indicate that this functionality should be used
 * outside of EFS2.
 */

/** Is fs_privatedir.h present. */
#define HAVE_EFS_PRIVATEDIR

/** Does fs_statvfs have the f_maxwrite field? */
#define HAVE_EFS_STATVFS_F_MAXWRITE

/** Does fs_statvfs have the enhanced members?
  * Users can use this define to figure out if a given EFS-VU has the enhanced
  * members or not.*/
#define HAVE_EFS_STATVFS_ENHANCEMENTS

/** Friendly names given to the FAT partitions under boot context:
 * The first FAT partition in the internal MMC card will be given the name
 * "sys_boot"
 * The first FAT partition in the external MMC card will be given the name
 * "mmc1" */
#define EFS_BOOT_INTERNAL_FAT     "sys_boot"
#define EFS_BOOT_EXTERNAL_FAT     "mmc1"


#endif /* End of _FS_SYS_TYPES_H */
