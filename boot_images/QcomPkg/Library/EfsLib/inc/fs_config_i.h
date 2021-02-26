/***********************************************************************
 * fs_config_i.h
 *
 * EFS2 configuration settings.
 * Copyright (C) 2006-2013 QUALCOMM Technologies, Inc.
 *
 * This file contains all of the settable parameters used in EFS2.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-12-09   wek   Add existing feature flag for romfs.
2013-05-07   wek   Add EFS2 Read Only support.
2013-04-11   dks   Make FS_HEARTBEAT_TIMER be set runtime based on a file.
2013-03-19   dks   Clean up legacy dog usage, make fs_task not dog monitored.
2013-02-28   nr    Remove references to third party feature FEATURE_QC_ZLIB.
2013-03-08   dks   Add apps sync control, remove timeout in read/write-iovec.
2013-03-06   nr    Add message to indicate a security vulnerabilty is enabled.
2012-12-18   wek   Decrease the shortname threshold to 103.
2012-12-18   dks   Make number of EFS buffer configurable.
2012-12-12   wek   Increase item file size to 128.
2012-11-09   nr    Update rmts to use new api's.
2012-10-26   rp    Malloc the safe benchmark buffers.
2012-10-18   wek   Make efs_raw_put/get wrappers around efs_put/get.
2012-08-22   rp    Remove FDI.
2012-05-23   dks   Cleanup code to remove Halcyon behavior.
2012-05-12   dks   Support storing/reading cookies to/from emmc partitions.
2011-10-25   wek   Remove the feature to enable/disable fs diag access.
2011-10-11   nr    Added FEATURE_FS_ZIP_TAR and FEATURE_FS_ZIP.
2011-08-17   rp    Add EFS OTP (One-Time-Programmable) support.
2011-08-01   nr    Change EFS Buffer use and replacement strategy.
2011-04-25   wek   Add a dummy max transaction macro.
2011-02-04   wek   Use page size macro for max page size.
2011-01-20   dks   Define EFS_USES_EFS_COOKIES only for RMTS.
2011-01-12   vr    Support EFS removal based on FEATURE_EFS2.
2010-12-07   wek   Remove documentation with page size macro name.
2010-11-30   wek   Remove duplicated macro for max pages per block
2010-11-18   dsk   Renamed EFS_COOKIE_IRAM_START_ADDRESS.
2010-11-15   rp    Replace Boot cookies with EFS cookies.
2010-11-11   wek   Remove dependency max transaction macro.
2010-09-29   yog   Compilation error fix for BOOT_ENV.
2010-07-30   wek   Dynamically allocate all buffers dependent on page size.
2010-07-19   wek   Add define for maximum supported page size.
2010-07-07   yog   Added macros definition under FEATURE_EFS_BOOT_ENV flag.
2010-06-01   wek   Journal oveflow fix.
2010-04-23   yog   Removed compiler warnings.
2010-03-19   yog   Added support for files having max filesize upto 4GB.
2010-03-05   vr    Add support for QEFS2 in BOOT
2010-02-08   rp    CMI compliance, remove #ifdef from API files.
2009-10-09   wek   Added define for symlink resolve limit.
2009-08-10   wek   Fix comments for doxygen.
2009-07-06   ebb   Added support for boot config FEATURE_EFS_BOOT_ENV.
2009-05-29   ds    Enabled DIAG_FS_ACCESS_VALIDATION & EFS_NAND_FACTORY_START
2009-04-01   yg    Moved some private config items into the private headers
2009-01-20   yg    Allow configured number of bad blocks.
2008-12-15   yg    Rename overriding bad block % configuration setting.
2008-12-12   rp    Introducing i/o-vec interface.
2008-12-02   yg    Added bad block % configuration setting.
2008-10-27   yg    Remove remote EFS related code for CMI.
2008-07-31   rp    Allow targets to override the max file descriptors.
2008-05-23   rp    Introducing /inc folder as per new SCM.
2008-04-28   np    Add support for multiple FTL partitions.
2007-11-28   rp    Corrected the multiple SD slots feature.
2007-11-09   rp    Revert multi-sd card change as 7500-target has issues.
2007-10-19   rp    Corrected FS_MAX_EXTFS_MOUNTS to match the hotplug dvc list
2007-10-01   rp    Added FDI support.
2007-09-13   sch   Defined FS_MAX_PAGES_PER_BLOCK
2007-08-19   rp    Added #define FS_MAX_DESCRIPTORS.
2007-06-09   rp    Doxygen comments markup.
2007-04-05   s h   Comments and cleanup.
2007-04-02   rp    Added HOTPLUG_NOTIFY_MAX_CLIENTS.
2006-12-05   dlb   Max transaction size configurable.
2006-10-26   s h   Allow target to define FS_MAX_ITERATORS
2006-10-26   dlb   Put freemap changes back.
2006-10-11   dlb   Back out freemap changes for now.
2006-10-05   s h   Increase the default path length limits
2006-09-08   dlb   Add configuration parameters for buffer cache.
2006-06-14   dlb   Increase number of iterators.
2006-06-01   s h   More NODEV mountpoints, please
2006-05-22   dlb   Allow path and name max to be specified externally.
2006-06-17   yg    Changed some consts
2006-05-09   s h   Increased the number of nodev mountpoints.
2006-05-09   dlb   Add nodevfs.
2006-05-05   dlb   Return name and path limits to original values.
2006-04-18   dlb   Move hotplug definitions here as well.
2006-04-05   dlb   Create

===========================================================================*/

#ifndef __FS_CONFIG_I_H__
#define __FS_CONFIG_I_H__

#include "fs_config.h"

/* Since some of our customers are using FEATURE_EFS2 to decide between the
 * the EFS1(compat)-API and the EFS2 API, lets avoid this feature FEATURE_EFS2
 * Note: Do not use this feature FEATURE_EFS2 in our EFS-VU, in case if you
 * need to refer the EFS2-API layer then use the feature FEATURE_EFS_API
 * instead. Really FEATURE_EFS2 means FEATURE_EFS_API layer. */
#ifdef FEATURE_EFS2
  #define FEATURE_EFS_API
#endif

#if defined FEATURE_EFS_EFS2_ON_MNAND || \
  defined FEATURE_EFS_EFS2_ON_NAND || \
  defined FEATURE_EFS_EFS2_ON_NOR || \
  defined FEATURE_EFS_EFS2_ON_RMTS || \
  defined FEATURE_EFS_EFS2_ON_RAMFS || \
  defined FEATURE_EFS_EFS2_ON_SPI_NOR
  /* By default, make QEFS as the default file-system. */
  #ifdef FEATURE_EFS2
    #define FEATURE_EFS_QEFS2
    #define FEATURE_EFS_ROOT_IS_QEFS2
  #endif /* FEATURE_EFS2 */
#endif

/**********************************************************************
 * CONFIGURATION ITEMS
 *
 * These parameters can be configured as desired.  Note that changing their
 * values doesn't affect existing files in the filesystem.  If a build is
 * loaded with smaller parameters for these values than earlier builds,
 * files may become inaccessible.
 */

/**
 * Maximum number of files that can be kept opened simultaneously.
 * Targets can override this value by this using \#define
 * EFS_MAX_FILE_DESCRIPTORS
 */
#ifdef EFS_MAX_FILE_DESCRIPTORS
  #define FS_MAX_DESCRIPTORS      EFS_MAX_FILE_DESCRIPTORS
#else
  #define FS_MAX_DESCRIPTORS      128
#endif

/**
 * Not a meaningful value.  Passed back via dia in the max_dir_entries
 * parameter.  Not used anywhere else in the code. */
#define FS_DIR_ENTRY_SIZE 50

/** Maximum number of EFS2 mountpoints.   Only "1" is supported!  */
#define FS_MAX_EFS_MOUNTS 1

/** The maximum number of EXTFS mountpoints.  Must match the size
 * of the device table in hotplug.   There must be one for each possible FAT
 * mountpoint: USB + SD/MMC + FTL + NULL device. */
#ifndef FS_MAX_EXTFS_MOUNTS
  #define FS_MAX_EXTFS_MOUNTS     (8 + 5 + 2 + 1)
#endif

/** Maximum number of ROMFS mounts */
#ifdef FEATURE_EFS_ROMFS
  #define FS_MAX_ROMFS_MOUNTS     2
#else
  #define FS_MAX_ROMFS_MOUNTS     0
#endif

/**
 * Maximum number of NODEV mounts.  Needs to be at least the size of the
 * EXTFS mounts.  Can be larger if they are used for something else.
 * Add three for additional mountpoints used by unit testing.
 * */
#define FS_MAX_NODEV_MOUNTS     (FS_MAX_EXTFS_MOUNTS +  \
                                 3)

/** Maximum number of OTP mounts */
#ifdef FEATURE_EFS_OTP
  #define FS_MAX_OTP_MOUNTS        1
#else
  #define FS_MAX_OTP_MOUNTS        0
#endif


/**
 * The maximum number of mountpoints.  Defines the total number of
 * mountpoints available.  This should be the sum of the maximum for each
 * filesystem type compiled in. */
#define FS_MAX_MOUNTS (FS_MAX_EFS_MOUNTS    +  \
                       FS_MAX_EXTFS_MOUNTS  +  \
                       FS_MAX_ROMFS_MOUNTS  +  \
                       FS_MAX_NODEV_MOUNTS  +  \
                       FS_MAX_OTP_MOUNTS)

/**
 * The maximum number of open directory iterators (returned by opendir).
 * These aren't all that expensive.  Although some mountpoint types may
 * allocate a FS_PATH_MAX sized buffer for each one. */
#ifndef FS_MAX_ITERATORS
  #define FS_MAX_ITERATORS        5
#endif

/**
 * The maximum number of flash pages (writeable unit) in one flash
 * block (erase unit).  A 4KB-page device has 128 pages per block.
 * There may be more in 4KB-page MLC devices or future devices, which
 * will require adjustment to this value.
 *
 * This value must be at least as large as the largest block size
 * supported by this build.
 */
#ifndef FS_MAX_PAGES_PER_BLOCK
  #define FS_MAX_PAGES_PER_BLOCK              128
#endif

/**
 * Maximum device page size supported.
 */
#ifdef EFS_PAGE_SIZE
  #define FS_MAX_PAGE_SIZE   EFS_PAGE_SIZE
#else
  #define FS_MAX_PAGE_SIZE   8192
#endif

/**
 * Minimum device page size supported.
 * This is the minimum page size supported by the file system from Page
 * Manager layer and above. There is a method of supporting smaller device
 * page sizes in device layer.
 */
#ifndef FS_MIN_PAGE_SIZE
  #define FS_MIN_PAGE_SIZE   512
#endif

/**
 * Free bitmap size configuration.  This is the amount of buffer space we
 * set aside to hold the freemap.  The value is a size in bytes for the
 * cache.  It must be large enough to hold the number of clusters needed to
 * hold the freemap (1 bit per data cluster).  It needs to be at least
 *   ceil (total_clusters / 8)
 * rounded up to the nearest cluster size.
 */
#ifndef FS_BUFFER_FREEMAP_SIZE
  #define FS_BUFFER_FREEMAP_SIZE          65536
#endif



/*
 * We pick a default window size to 10 for the modified LRU algorithm used
 * for buffer cache victim selection. This checks the next 10 buffer slots
 * from the direct mapped index and replaces the oldest of the 10.
 * Higher values mean lesser probability of thrashing at the cost of
 * higher computaion. Lower values increse the probaility of thrashing.
 * We use 10 as the extensive tests show that this is good enough for
 * most use cases without performance overhead.
 */
#ifndef FS_BUFFER_LRU_WINDOW
  #define FS_BUFFER_LRU_WINDOW 10
#endif


/**
 * Number of links to follow before giving up.
 *
 * The file system will resolve this many symbolic link to find the actual
 * entry. If for some reason we have resolved this many links and we still
 * don't find the entry we declare there is a loop and quit.
 */
#define FS_SYMLINK_RESOLVE_LIMIT      8

/*
 * Set how much % of blocks can go bad. For real small partition this
 * might turn out to be just 1 block. This configuration really depends
 * on how much space can we lose in EFS overhead. Also allowing too many
 * bad blocks in the parition could make EFS very very slow when maximum
 * allowed number of bad blocks have developed and the EFS is full. Because
 * EFS will be trying to freeup enough space by doing GC which it would never
 * be able to attain, since there will be some amount of garbage in the system
 * at any given instance. */
#ifdef FS_MAX_BAD_BLOCK_PERCENT_ALLOWED
  #define MAX_BAD_BLOCK_PERCENT_ALLOWED   FS_MAX_BAD_BLOCK_PERCENT_ALLOWED
#else
  #define MAX_BAD_BLOCK_PERCENT_ALLOWED   4
#endif

/* Configure minimum suported bad blocks in the Data region */
#define FS_MIN_DATA_BAD_BLOCK_ALLOWED          3

/* Configure minimum suported bad blocks in the PTable region */
#define FS_MIN_PTABLE_BAD_BLOCK_ALLOWED          3


#ifndef DOXYGEN_SHOULD_SKIP_THIS

/**********************************************************************
 * The following defines are used to determine the different sizes of
 * EFS db entry 'values' and EFS db entry 'keys'. EFS entry values store
 * the entry data, for example: inode number for regular files or file
 * data for item file. The EFS db entry key is the a sequence of bytes
 * that uniquely identify the entry, the key is formed by concatenating
 * the parent inode number with the entry name.
 *
 * When the cluster size is 512 bytes the database can nicely store
 * key value pairs that are in total 245 bytes long. This is because
 * for each key/value pair the database stores 2 bytes for the length
 * of the key and value, adding to 247bytes. This size equals to half
 * of the data field in a db node.
 *
 * The size of 245 bytes for key/value pair is a concern for item files.
 * If the item file name content is 128 bytes and the item file name greater
 * than 109 bytes, it causes the total value of the key value pair greater
 * 245 (1 + 4 + item_name_len + 1 + 2 + item_content_len). Large key/value
 * pairs are not handled by DB module. */


/* Maximum length of a EFS item file contents. If it larger than this
 * it is stored as a regular file. The maximum item file size of 128 is
 * chosen because on average 95.8% of modem file system entries are below
 * 128 bytes. 95.8% is based on file count, not file size. There are very
 * few entries (0.6%) that are between 128 and 209 that could be stored as
 * an item file. */
#define FS_ITEM_MAX  128


/* Maximum length of a symlink destination. This value was chosen to 107
 * when the FS_SHORTNAME_THRESHOLD was 128. This value can be increased
 * to 136 (FS_MAX_DB_VALUE - 1). */

#define FS_MAX_SYMLINK_TARGET  107

/* Size of each piece of a longname stored in the value field of the
 * database. This value was chosen to 108 when the FS_SHORTNAME_THRESHOLD
 * was 128. This value can be increased to 136(FS_MAX_DB_VALUE - 1). */
#define FS_LONGNAME_PIECE_SIZE  108

/* Maximum 'value' size for a database entry.  Must be able to store the
 * largest item or symlink used by the filesystem.  There is only one
 * buffer of this size. The maximum value in EFS is for item file 2 'N',
 * where we store the 'N', mode bits, gid, time and file contents. */
#define FS_MAX_DB_VALUE  (1 + 2 + 2 + 4 + FS_ITEM_MAX)

/* Threshold above which file names are stored differently. Because the
 * names are stored in a B-Tree, the length of the name stored there is
 * limited.  Names that are longer than this threshold are stored using a
 * hash and long name auxiliary entries. The value of 103 is chosen
 * to comply with EFS Database limitations. In the file system the item
 * file 2 will generate the largest key/value pair. The length of 103
 * will guarantee that the maximum length of key/value pair for item file 2
 * to be 245, the maximum value supported by the Database module. */
#define FS_SHORTNAME_THRESHOLD  103

/* This is the number of characters from the full filename that are stored
 * in the hash entry.  Prefixing the shortname with characters from the
 * long name causes the names to still generally sort correctly. */
#define FS_SHORTNAME_PREFIX_LEN 8




/* How many bytes are in 1 SDCC sector. */
#define FS_SDCC_SECTOR_SIZE_IN_BYTES        (512)

/**
 * Buffer cache configuration. This is the number of cluster size buffers used
 * by the buffering code (fs_buffer.c). It needs to be at least as large as the
 * largest number of clusters that will be dirtied by a single transaction. It
 * can be made larger, which will reduce some reads, but keep in mind that
 * cache misses will cause a scan through the entire pool.
 */
#define FS_BUFFER_MIN_NUMBER_BUFFERS      30

#ifdef EFS_BUFFER_NUMBER_BUFFERS
  #define FS_BUFFER_NUMBER_BUFFERS        EFS_BUFFER_NUMBER_BUFFERS
#else
  #define FS_BUFFER_NUMBER_BUFFERS        FS_BUFFER_MIN_NUMBER_BUFFERS
#endif

#if (FS_BUFFER_NUMBER_BUFFERS < 30)
  #error "FS_BUFFER_NUMBER_BUFFERS cannot be less than 30"
#elif (FS_BUFFER_NUMBER_BUFFERS > 64)
  #error "FS_BUFFER_NUMBER_BUFFERS defined very high : 64"
#endif


/**********************************************************************
 * These parameters should not be adjusted, but are computed based on the
 * above values, and other customer defined values. */

/* Maximum IO-Vec items in one transacton. */
#ifndef FS_MAX_IOVEC_ITEMS_COUNT
  /* In one transaction utmost, how many freemap pages we will have to write.*/
  #define FS_MAX_IOVEC_ITEMS_FREEMAP_OVERHEAD_COUNT     \
                      (FS_MAX_TRANSACTION_DATA_PAGES)
  #define FS_MAX_IOVEC_ITEMS_COUNT            (FS_BUFFER_NUMBER_BUFFERS +   \
                                   FS_MAX_IOVEC_ITEMS_FREEMAP_OVERHEAD_COUNT)
#endif


/* Padding to allow for user pages as well.  Used by the page manager and
 * buffer code to have enough padding to write a full transaction. */
#define FS_TRANSACTION_PAD \
  (FS_MAX_TRANSACTION_DATA_PAGES + (FS_MAX_TRANSACTION_DATA_PAGES >> 2))


/* Maximum FAT transaction size.  The HFAT filesystem doesn't have a limit,
 * but we set one so that a given request doesn't block the filesystem for
 * too long.  There are also some versions of underlying drivers that have
 * problems over 64k. */
#ifndef FS_MAX_FAT_TRANSACTION
  #define FS_MAX_FAT_TRANSACTION  65536
#endif

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

/** FTL supports multiple partitions. Maximum number of FTL partitions
 *  can be chosen with the following definition.
 */
#ifndef MAX_FTL_PARTITIONS
  #if defined(FS_UNIT_TEST)
    #define MAX_FTL_PARTITIONS                    2
  #else
    #define MAX_FTL_PARTITIONS                    1
  #endif
#endif

/* If no active EFS superblocks are found, then search for the factory
 *  image if this feature is turned on
 */
#ifndef FEATURE_EFS_BOOT_ENV
  #define FEATURE_EFS_NAND_FACTORY_START
#endif


/*-----------------------------------------------------------------------------
  Configure the Sync timer interval.
-----------------------------------------------------------------------------*/
#ifdef FEATURE_EFS_SYNC_TIMER_IN_MILLISECS
  /* For now cannot override default value of period syncs and want to control
     syncs from apps. */
  #ifdef FEATURE_EFS_APPS_CONTROLS_EFS_SYNC
    #error "Cannot override sync period and control syncs"
  #endif
  #define FS_SYNC_TIMER          (FEATURE_EFS_SYNC_TIMER_IN_MILLISECS)
#else
  /* If apps processor is controlling the Syncs, set default value of periodic
     syncs to 0 to disable it. */
  #ifdef FEATURE_EFS_APPS_CONTROLS_EFS_SYNC
    #define FS_SYNC_TIMER        (0)
  #else
    #define FS_SYNC_TIMER        (90*1000) /* 90 Seconds. */
  #endif
#endif


/*-----------------------------------------------------------------------------
  Configure the read/write iovec timeout.
-----------------------------------------------------------------------------*/
#ifdef FEATURE_EFS_RMTS_IOVEC_TIMEOUT_IN_MILLISECS
  #define FS_RMTS_IOVEC_TIMEOUT   (FEATURE_EFS_RMTS_IOVEC_TIMEOUT_IN_MILLISECS)
#else
  #define FS_RMTS_IOVEC_TIMEOUT   (0) /* No timeout, block forever. */
#endif

/* By default define this number of FS_HFAT_MAXVOLUME_COUNT. This will
 * determine the number of volumes in HFAT and how to do the maping in
 * extfs layer. */
#ifndef FS_HFAT_MAXVOLUME_COUNT
  #define FS_HFAT_MAXVOLUME_COUNT   3
#endif


/* Macro declaration for max value of 2GB and 4GB filesize. */
#define FS_MAX_2GB_VALUE      0x7FFFFFFF
#define FS_MAX_4GB_VALUE      0xFFFFFFFF
#define FS_MIN_2GB_VALUE      (-(2 * 1024 * 1024 * 1024LL))
#define FS_MIN_4GB_VALUE      (-(4 * 1024 * 1024 * 1024LL))

/* File size restriction for 32 bit and 64 bit api's. */
enum fs_restrict_api_to_gb
{
  FS_RESTRICT_API_TO_2GB,
  FS_RESTRICT_API_TO_4GB
};

/** Byte-Alignment requirements specified by the DMA. The buffers that we pass
 * to Flash-drivers if byte-aligned to these many bytes will optimize the data
 * transfer speed. */
#ifndef FS_DM_ALIGN_BYTES
  #define FS_DM_ALIGN_BYTES        128
#endif

#if FS_DM_ALIGN_BYTES <= 0
  #error "Invalid memory alignment configuration"
#endif

/* Alignment keyword syntax varies with each compiler. */
#if   defined (__GNUC__)
  #define FS_ALIGN __attribute__((__aligned__(FS_DM_ALIGN_BYTES)))
#elif defined (__arm)
  #define FS_ALIGN __align(FS_DM_ALIGN_BYTES)
#elif defined (_MSC_VER)
  #define FS_ALIGN __declspec(align(FS_DM_ALIGN_BYTES))
#elif defined (_lint)
  #define FS_ALIGN /* Nothing. */
#else
  #error "Unrecognized C compiler.  Need alignment syntax for this target."
#endif

/* Is this pointer 32-byte aligned?
 * Note that this assumes pointers are uint32-equivalent */
#define FS_OKAY_FOR_DM(x) (((uint32)(x) & (FS_DM_ALIGN_BYTES-1)) == 0)

#if defined FEATURE_EFS_EFS2_ON_RMTS

  #ifdef EFS_COOKIE_IMEM_START_ADDRESS
    #ifndef EFS_COOKIE_IMEM_SIZE_IN_BYTES
      #error "EFS_COOKIE_IMEM_SIZE_IN_BYTES is not defined."
    #endif

    #define FEATURE_EFS_USE_IMEM_COOKIES
  #endif

#endif /* FEATURE_EFS_EFS2_ON_RMTS */

/*-----------------------------------------------------------------------------
  FS_ZIP_TAR feature needs both TAR and ZIP modules. So make sure both these
  modules are enabled.
-----------------------------------------------------------------------------*/
#ifdef FEATURE_FS_ZIP_TAR
  #ifndef FEATURE_FS_TAR
    #error "FEATURE_FS_ZIP_TAR feature needs the FEATURE_FS_TAR feature"
  #endif
  #ifndef FEATURE_EFS_ZIP
    #error "FEATURE_FS_ZIP_TAR feature needs the FEATURE_EFS_ZIP feature"
  #endif
#endif

/*-----------------------------------------------------------------------------
  ADD ALL SECURITY VULNERABLE FEATURES TO THE LIST BELOW.
-----------------------------------------------------------------------------*/
#if defined(FEATURE_EFS_ENABLE_FACTORY_IMAGE_SECURITY_HOLE) ||\
    defined(FS_PROFILE_ENABLE_EXTENSIVE_PROFILE)
  /* Set the feature to say security vulnerability was enabled. */
  #define FS_BUILD_HAS_SECURITY_VULNERABILITY_ENABLED
#endif

#endif /* not __FS_CONFIG_H__ */
