/***********************************************************************
 * fs_public.c
 *
 * Public interface for EFS2
 * Copyright (C) 2002-2017, 2019 QUALCOMM Technologies, Inc.
 *
 * This file contains POSIX interface functions using which EFS2
 * can be utilized.
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/EfsLib/src/fs_public.c#2 $ $DateTime: 2019/11/07 23:37:41 $ $Author: pwbldsvc $

when          who     what, where, why
--------      ---     ------------------------------------------------------
2019-09-23    eo      fix signed and unsigned arithmetic diag read/write.
2017-01-25    rp      Add support track efs_sync callers.
2017-01-11    rp      Add support to extract efs-zip-tar via delayed diag.
2016-12-19    rp      Cleanup in recursive iteration of files during zip-tar
2016-09-21    np      Add support to extract all non secure EFS-files zip-tar
2016-06-25    rp      Fix Klockwork warning.
2016-05-23    rp      Support OEMs to create their own EFS-golden-copy.
2016-04-25    rp      Incorporate code review comments for EFS quota change.
2016-05-04    rp      Add support to erase efs-image on efs_reset for rmts
2016-04-27    rp      Do EFS golden copy files list restore outside efs-init
2016-04-25    rp      Add support to enforce EFS quota.
2016-04-14    rp      Add EFS golden copy creation from files list.
2016-03-24    wek     Don't unlink or renamed open item files.
2016-01-21    mj      Added support for RCINIT Group 7 shutdown notification.
2015-05-27    vm      Add support to log EFS APIs called for debug.
2015-04-23    vm      Force the EFS Sync during shutdown.
2015-02-23    vm      Add support to compress EFS RAM into shared RAM on sync
2014-04-26    vm      Use correct FS_MSG macros to save RAM.
2014-02-26    wek     Add compile time romfs flag around read-only init.
2013-12-09    wek     Add existing feature flag for romfs.
2013-11-01    wek     Allow syncs after creation of golden copy.
2013-10-10    rp      Hotplug to poll only devices of interest.
2013-07-22    rp      Add new EFS API to store data in SuperBlock.
2013-06-19    wek     Add function to do a complete iteration of efs scrub.
2013-07-17    wek     Add a feature around the inclusion of fs stdio libs.
2013-06-19    dks     Added NAND Flash scrub support.
2013-05-24    wek     Make File descriptor ops constant.
2013-05-17    wek     Fix bug in extensive profiling logging wrong errno.
2013-05-14    nr      Fix compiler warnings.
2013-05-10    dks     Featurize CM sync for when apps does not control sync.
2013-05-07    wek     Add EFS2 Read Only support.
2013-05-06    nr      Disable profiling when extracting EFS images.
2013-04-26    wek     Move all buffers to DDR when compiling for boot.
2013-04-11    dks     Add F3 msg to identify shutdown notification from RCINIT.
2013-04-05    nr      Add support to capture flash counters for efs_initialize.
2013-03-28    nr      Add support to profile efs_initialize.
2013-02-05    wek     Add debug variable to have easy access to efs tcb.
2013-01-28    nr      Clean up references to tmc task.h.
2013-01-28    nr      Fix Klockwork errors with respect to va_list args.
2013-01-11    dks     Move writing to Golden partition outside of efs_sync.
2012-12-26    nr      Implement support of Extensive profiling.
2012-12-14    wek     Implement modified efs_utime for item files.
2012-12-18    dks     Update golden copy strategy for new rmts layout.
2012-11-28    nr      Implement EFS Profile framework.
2012-10-22    wek     Update babylon tag name for pure put/get.
2012-10-22    wek     Move efs_raw_put/get functions from public to lib.
2012-10-18    wek     Make efs_raw_put/get wrappers around efs_put/get.
2012-09-10    nr      Implement new FS Benchmarking scheme.
2012-08-30    wek     Allow zero size item files.
2012-08-22    rp      Remove FDI.
2012-08-14    wek     Implement chmod for item files.
2012-08-09    dks     Rename fs_cookie.h to fs_cookie_i.h.
2012-07-30    nr      Fix GPT based FAT mount during boot.
2012-06-12    dks     Cleanup IMEM cookies and cookie APIs.
2012-06-05    dks     Include fs_babylon.h unconditionally.
2012-05-31    rp      Optimize EFS untar timings.
2012-05-24    nr      Fix bugs in fs_zip and fs_tar initialization.
2012-05-18    wek     Remove jzap header file.
2012-05-02    dks     Support storing/reading cookies to/from emmc partitions.
2012-04-30    rp      Add safe benchmark command to measure efs_sync timings.
2012-04-30    nr      Register shutdown callback with RCINIT.
2012-03-28    nr      Add colors and clean debug MSGs and ERRORs.
2012-03-20    wek     Cleanup customer header file inclusion.
2012-03-08    wek     Rename macros and remove header files to compile for boot
2012-01-07    wek     Move remotefs write iovec outside efs global lock.
2011-12-06    dks     Added efs_async_put support.
2011-12-04    dks     Initialize all locks before using them.
2011-11-02    nr      Klocwork error fixes in Modem code.
2011-10-24    nr      Minor Cleanup and Fixes.
2011-10-24    vm      Klocwork error fixes
2011-10-13    rp      Added support to log EFS-Sync timings.
2011-10-11    nr      Added gzip support in file system image creation APIs.
2011-10-10    dks     Correct Hotplug feature flag.
2011-10-04    dks     Make EFS VU independent of Hotplug and Hfat VUs.
2011-10-03    dks     Move efs_get_errno_address to fs_errno.c.
2011-09-08    wek     Support opaque declaration of task error number.
2011-08-17    rp      Add EFS OTP (One-Time-Programmable) support.
2011-07-05    wek     Add file system image creation APIs.
2011-05-10    wek     Remove sync lock from get sync status API.
2011-05-09    dks     Make need_dual_sync a run-time flag.
2011-04-21    wek     Remove compiler warnings.
2011-03-25    dks     Support Golden-Copy in RAM for fusion.
2011-03-09    dks     Add babylon port init.
2011-03-08    rp      Add support to lock/unlock EFS.
2011-01-12    vr      Support EFS removal based on FEATURE_EFS2.
2010-12-03    rp      Featurize FS-TAR module.
2010-12-03    rp      Featurize fs_init_complete prototype for Boot modules.
2010-12-01    rp      Don't compile fs_init_complete for Boot modules.
2010-11-29    rp      In fs_init_complete, don't use nameidata_buffer's vp.
2010-11-18    rp      Support TAR image Golden-Copy for RMTS.
2010-11-18    rp      Add FS-TAR module.
2010-11-08    wek     Remove impractical and unused paranoia module.
2010-11-04    wek     For boot build on write don't check for null buffer.
2010-11-03    wek     Make stdio independent of page size macro.
2010-10-22    wek     For boot build on read don't check for null buffer.
2010-09-17    yog     Allow times arg for efs_utime API to be NULL.
2010-07-30    wek     Dynamically allocate all buffers dependent on page size
2010-07-19    wek     Added initialization of malloc library.
2010-02-06    yog     Validate the input param for NULL check.
2010-05-13    wek     Make sync_no_wait non blocking.
2010-05-10    dks     Added SPI-NOR device type.
2010-05-07    rp      Added Ram filesystem.
2010-05-07    wek     Fix lock release sequence in efs_sync_get_status.
2010-05-06    wek     Implement golden copy for remote storage page manager.
2010-04-28    vr      Relocate fs_task from EFS1 to EFS2
2010-04-23    yog     Removed compiler warnings.
2010-03-19    yog     Added support for files having max filesize upto 4GB.
2010-03-15    vr      Fix Misaligned FTL Buffer passed to Flash.
2010-03-15    rp      Optimize the efs sync timer.
2010-03-05    vr      Add support for QEFS2 in BOOT
2010-02-09    wek     Added efs_prepare_shutdown API.
2010-02-08    rp      CMI compliance, remove #ifdef from API files.
2010-01-19    rp      Remove creation of the export directory.
2010-01-19    vr      Added efs_sync_no_wait and efs_sync_get_status APIs.
2009-12-18    vr      Added set_volume_name and get_volume_name APIs.
2009-11-23    wek     Close files while unmounting.
2009-11-23    wek     Fix rename acrross mount points dereferencing NULL.
2009-11-20    wek     Close directory iterators while unmounting.
2009-11-17    vr      Return EXDEV error for all renames across mountpoints.
2009-11-04    dks     Removed feature define based switching of functions.
2009-08-11    wek     On mount calls use void pointer instead of char.
2009-07-06    ebb     Added changes to compile EFS into oemsbl boot image.
2009-06-22    ebb     Created stub for efs_sync to allow clients to call it.
2009-05-26    rp      Include fs_diag_i.h to call fs_diag_init().
2009-05-18    rp      Fix compiler warning for 8200 which has gcc with more -Ws
2009-05-08    ebb     Added blockdev_init to efs_initialize.
2009-04-15    ebb     Removed from protected seg so clients can access fs_iters
2009-04-03    wek     Added efs_utime() API.
2009-03-25    wek     Clean fs_stat structure in file stat functions.
2009-03-16    ebb     Added dummy variable to test efs memory protection.
2009-03-04    ebb     Initialize efs PD before calling fs_efs2_unprotect_mem
2009-02-03    ebb     Removed romfs dependency on FEATURE_EFS_QEFS2.
2009-01-29    wek     Made hfat_init run on target, added to fs_extfs_init.
2009-01-28    wek     Updated initialization of stdio and rollback.
2008-12-12    rp      Introducing Managed Nand.
2008-12-02    yg      Fail rename if the destination file is open.
2008-11-19    yg      Move cpu_hog task init to benchmark init.
2008-11-06    sh      Removed the last vestiges of ZAP
2008-11-03    rp      Introduce device abstraction.
2008-10-27    yg      Removing remote EFS related code for CMI.
2008-10-17    yog     Featurized pragma directive to exclude for FS_STANDALONE.
2008-10-06    np      Added efs_shred API.
2008-05-23    rp      Changes for new SCM.
2008-03-19    sh      Improved Babylon message logging for several calls.
2008-01-24    sch     Used getsize before getstat in seek and append
2007-12-05    rp      Moved FS_PARANOIA_CHECKS feature to fs_paranoia.c
2007-12-10    rp      Added FDI support to the efs_get_device_info() API.
2007-11-16    sh      Rename TIMETEST2 calls to Babylon calls
2007-11-02    rp      Move fs_fresh_start variable from fs_pm_super.c file.
2007-10-26    rp      Reset paranoia computed crc on efs_initialize().
2007-10-18    rp      Featurized QEFS2.
2007-10-01    rp      Add FDI support.
2007-09-10    sh      Adjust seek position after write with O_APPEND
2007-08-19    rp      FS_GLOBAL_LOCK() cleanup.
2007-06-14    dlb     Fix vnode leak querying group information.
2007-04-23    umr     Added support for EFS memory protection feature.
2007-05-16    rp      The truncate API's will return EINVAL for -ve args.
2007-05-11    umr     Added stack check on simulator build
2007-04-24    sch     Allowed renaming of directories
2006-12-21    s h     Implement FIFO and Socket file nodes.
2006-11-10    s h     Add support for efs_chmod().
2006-11-02     rp     Added efs_stdio_init() in efs_initialize().
2006-10-26    s h     Removed all references to obsolete fs_testapp
2006-10-05    umr     Added call to initialize deltree lock in efs_initialize
2006-09-20    dlb     Lint cleanup.
2006-08-15    dlb     Initialize upgrade system.
2006-07-19    dlb     Move transaction limits down to each FS.
2006-07-06    dlb     Remove 'size' field from vnode.
2006-07-05    dlb     Move read-end check down to VFS code.
2006-06-27    dlb     Pass stat through readdir.
2006-06-23     sh     Removed conditional FEATURE_FS_BENCHMARK.. stubs now.
2006-06-19    dlb     Enable diag in off-target test.
2006-06-19    nrs     Allow renames on different size items
2006-06-07     sh     Featurized hotplug_init()
2006-06-02     sh     Removed FTL layer initialization. (moved to hotplug)
2006-05-23     yg     Added FTL layer initialization.
2006-05-11     sh     Renamed efs_hotplug_... to hotplug_...
2006-05-09    dlb     Add nodevfs.
2006-03-31     sh     Lint cleanup
2006-03-29     sh     Hotplug init is no longer optional.
2006-01-31    dlb     Correct errno of access call.
2006-01-26     sh     Added TIMETEST2 messages for public API
2006-01-26     sh     Added TIMETEST2 message support
2006-01-11    nrs     Fixed Copyright header
2005-12-20    dlb     Initialize descriptor table.
2005-11-28    nrs     Merge 7500 orphans
2005-11-12     sh     Use rex tcb err_num instead of global efs_errno
2005-11-10    nrs     Merge Fix
2005-11-09    sh      Renamed FS_FEATURE_EXTFS to FEATURE_EFS_EXTFS
2005-10-28    nrs     Changed raw_put and raw_get length from int to fs_size_t
2005-10-26     sh     Lint cleanup
2005-10-20    sh      Initialize fs_benchmark task and cpu_hog task
2005-10-05    nrs     Clean and fix quotas and reservations
2005-09-02    dlb     Don't follow last symlink on symlink creation.
2005-09-02    dlb     Add sfat mounting feature on /mmc1.
2005-08-26    sh      Don't ERR_FATAL on efs_chmod, even unimplemented
2005-08-23    sh      Add support for mknod, device files
2005-08-10    dlb     Umount of opened dirs.
2005-08-10    dlb     Nodev root of mpoint properly.
2005-08-09    dlb     Add nodev support, for cleanup on umount.
2005-08-08    sh      Enforce identical item size for put
2005-08-03    dlb     Add umount support.
2005-07-22    nrs     Added implementation for quotas & reservations
2005-07-15    nrs     Added implementation for efs_chown
2005-07-13    dlb     Mountpoint cleanup.
2005-06-06    dlb     Extensions for SFAT.
2005-05-26    sd      Compilation fixes for L4.
2005-05-10     sh     Lock filesystem before starting superblock reset
2005-04-26    dlb     Add 2K page support.
2005-04-13    dlb     Fix testapp build for remote.
2005-04-06    nrs     Removed fs_diag dependencies from Unit Test
2005-03-28    dlb     Move unref of mkdir vp up one level.
2005-02-17    dlb     Remove transaction mention from mkdir.
2005-01-27    dlb     Allow standalone builds.
2004-11-02    dlb     Remove all mention of transactions here.
2004-10-21    nrs     Modified fs_public_desc_write() to not use transactions
2004-10-13    dlb     Fixed flush outside of xact on unlink.
2004-10-12    dlb     Add remote EFS client support.
2004-10-11    nrs     Modified rename to work with files that currently exist
2004-10-11    nrs     Modified unlink to remove transaction calls
2004-10-07    nrs     Changed efs_put, efs_get to efs_raw_put, efs_raw_get
2004-10-07    dlb     Whitespace cleanup.
2004-07-26    dlb     Support extfs.
2004-07-19    dlb     Restructure vnode interface.
2004-02-20    dlb     Disallow renames of symlinks.
2003-11-18     gr     Restored call to FS_GLOBAL_UNLOCK in efs_rename.
2003-10-22    dlb     Fix error code on write of 0 bytes.
2003-09-09    dlb     Factory image works with bitmap and tree.
2003-08-20    dlb     Remove delayed truncate code.
2003-07-26     gr     Modified efs_lseek to return an error if an attempt to
                      seek before the beginning of the file is made.
2003-07-23     gr     Added check in ftruncate to return an error if the input
                      file was opened in read-only mode.
2003-06-17    jkl     Clean up code.
2003-06-13     gr     Modified efs_unlink and efs_rmdir to return an error if
                      the file or directory being deleted is open.
2003-06-12    adm     Add support for factory start.
2003-06-12     cr     Featurize testapp init.
2003-05-30     gr     Fixed a bug in efs_open. Modified efs_rename to return
                      failure if called on a directory.
2003-05-23     cr     Add initialization of on target test application.
                      The init won't happen unless one takes a breakpoint in
                      debugger and changes state on static variables.
2003-05-16     gr     Fixed a bug in efs_rename.
2003-05-02     gr     Added master reset functionality.
2003-04-15    cr      Moved global lock to fs_efs2.h as part of delayed
                      truncation implementation to fix CR#28410.
2003-03-24     gr     Code modifications to ensure that all writes to flash
                      have transaction wrappers.
2003-03-12     gr     Implemented efs_rename.
2003-03-06    bgc     Moved vnode flush to inside the transaction in
                      fs_public_desc_write().
2003-03-04     gr     Renamed the parameters to efs_rename so that the C++
                      reserved word "new" is not used.
2002-08-20    adm     Created.

===========================================================================*/

#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "fs_sys_types.h"
#include "fs_err.h"
#include "fs_fcntl.h"
#include "fs_errno.h"
#include "fs_public.h"
#include "fs_desc.h"
#include "fs_vnode.h"
#include "fs_namei.h"
#include "fs_mount.h"
#include "fs_romfs.h"
#include "fs_nodev.h"
#include "fs_osal.h"
#include "fs_config_i.h"
//#include "fs_sync.h"
//#include "fs_pathbuff.h"
#include "fs_priv_funcs.h"
#include "fs_util.h"
#include "fs_babylon.h"
//#include "fs_cookie_i.h"
#include "assert.h"
//#include "fs_timetick.h"
#include "fs_profile.h"
#include "fs_protected_memory.h"
//#include "fs_task_i.h"

#ifndef FEATURE_EFS_COMPILE_WITHOUT_DIAG
  #include "fs_diag_i.h"
  #include "fs_diagd.h"
#endif

#ifdef FEATURE_FS_COLDPLUG
  #include "coldplug_api.h"
#endif

#ifdef FEATURE_EFS_EXTFS_HFAT
  #include "hfat_api_f.h"
#endif

/* These PM header files are needed because efs_reset_nostop() calls PM
 * functions directly. */
#ifdef FEATURE_EFS_EFS2_ON_RAMFS
#include "fs_ramfs_pm.h"
#endif

#ifdef FEATURE_EFS_EFS2_ON_MNAND
#include "fs_mnand_pm.h"
#endif

#ifdef FEATURE_EFS_EFS2_ON_RMTS
#include "fs_rmts_pm.h"
#endif

#ifdef FEATURE_EFS2

#ifdef FEATURE_FS_TAR
  #include "fs_tar_i.h"
#endif

//#include "fs_zip.h"


#ifdef FEATURE_FS_ZIP_TAR
  #include "fs_zip_tar_i.h"
#endif


#ifdef FEATURE_EFS_QEFS2
  #include "fs_efs2.h"
  #include "fs_db.h"
  #include "fs_benchmark.h"
  #include "fs_upgrade.h"
  #include "fs_privatedir.h"
  #include "fs_device.h"
  #include "fs_malloc.h"
#endif

#ifndef FEATURE_EFS_BOOT_ENV
  #include "fs_estdio.h"
  #include "fs_stdio.h"
  #include "fs_timings.h"
  #include "fs_golden_copy.h"
  #include "fs_golden_copy_files_list.h"
  #include "fs_quota.h"
#endif

#ifdef FEATURE_EFS_EXTFS
#include "fs_extfs.h"
#endif

#include "fs_global_lock.h"

//#include "fs_ftl.h"
//#include "fs_otp.h"

#ifdef FEATURE_FS_ASYNC_PUT
  #include "fs_async_put_i.h"
#endif

#ifdef FS_UNIT_TEST
  #include "fs_init_complete.h"
#endif

#ifndef FS_ROOT_FS_NAME
  #define FS_ROOT_FS_NAME "EFS2"
#endif


 /* This below file will control how the final shutdown efs-sync is done.
  *
  * At device shutdown, if EFS is not dirty then as an optimization the
  * EFS-module will not do an efs-sync. This below file can control this
  * behaviour.
  *
  * If the below file exists and contains a 1, then the EFS-module will
  * forcefully do an efs-sync at device shutdown regardless of EFS is dirty
  * or not.
  *
  * If the below file contains a 0 or does not exist, then EFS-modlue will
  * do an efs-sync at shutdown only if the EFS is dirty.
  *
  */
#define FS_CONFIG_FILE    "/efs_force_shutdown_sync"

/* This global variable when set to non-zero value will blow away the entire
 * file-system data contents, refer fs_public.h file for more information. */
int fs_fresh_start = 0;

/* This global variable when set to non-zero value will cause efs_sync to fail.
 * We set this variable during shutdown group7 callback notification in order
 * to block all efs_sync's till we do a final shutdown efs_sync in group 0 */
#ifndef FEATURE_EFS_BOOT_ENV
 static int fs_reject_efs_sync = 0;
#endif

/* Debug variable, to have easy access to EFS Task Control Block. */
fs_os_tcb_type *efs_tcb;

struct nameidata nameidata_buffer;
struct nameidata nameidata_buf2;

/* Vops. */

static int fs_public_desc_close (struct fs_descriptor *file);
static fs_ssize_t fs_public_desc_write (struct fs_descriptor *file,
    const void *buf, fs_size_t count);
static fs_ssize_t fs_public_desc_read (struct fs_descriptor *file,
    void *buf, fs_size_t count);

static const struct desc_ops public_desc_ops = {
  fs_public_desc_close,
  fs_public_desc_write,
  fs_public_desc_read,
};

/**********************************************************************
 * Iterators.
 *
 * All iterators are allocated and deallocated from a common pool.  We make
 * the assumption that there will not be many iterators in use.
 */

struct fs_dir_data {
  int                   busy;
  void                 *iter;
  struct fs_dirent      dirent;
  struct fs_vnode      *vp;
};

static struct fs_dir_data all_iters[FS_MAX_ITERATORS];


/* File system image handle.
 * There can be only one file system image created at a time. Creating a file
 * system image requires that no client changes the file system. While
 * creating the image we will aquire all file system locks. Therefore, we
 * can't have two clients creating a file system image, one handle is enough.
 */

struct fs_filesystem_image_handle
{
  uint8 in_use;
  enum fs_filesystem_image_type image_type;
  void *handle;
  int allow_device_specific_files;
  int allow_secure_files;
};

struct fs_filesystem_image_handle fs_img_handle;

/* Data for exceptions. */
struct efs_catch_block *_fs_catch = NULL;
struct efs_catch_block _fs_static_catch;


/* This flag identifies whether we are currently in boot mode. Currently
 * it allows us to mount a device over a file which is normally
 * disallowed. This is necessary when compiling into boot code because
 * our root filesystem is ROMFS */
int fs_in_boot_mode = FALSE;


/*------------------------------------------------------------------------
    EFS API Debug : To debug EFS APIs called
 -----------------------------------------------------------------------*/
enum fs_api_debug_id
{
  FS_API_DEBUG_ID_EFS_OPEN          = 0,
  FS_API_DEBUG_ID_EFS_READ          = 1,
  FS_API_DEBUG_ID_EFS_WRITE         = 2,
  FS_API_DEBUG_ID_EFS_PUT           = 3,
  FS_API_DEBUG_ID_EFS_GET           = 4,
  FS_API_DEBUG_ID_EFS_CLOSE         = 5,
  FS_API_DEBUG_ID_EFS_SYNC          = 6,
  FS_API_DEBUG_ID_MAX               = 7,
};

static void fs_api_debug_init (void);
static void fs_api_debug_add (enum fs_api_debug_id id, int fd,
                              const char *file_path);

#ifdef FEATURE_ENABLE_EFS_API_DEBUG

#ifndef FS_API_DEBUG_PATH_NAME_MAX
  #define FS_API_DEBUG_PATH_NAME_MAX      50
#endif

#ifndef FS_API_DEBUG_MAX_ENTRIES
  #define FS_API_DEBUG_MAX_ENTRIES        500
#endif

#ifndef FS_API_DEBUG_TASK_NAME_MAX
  #define FS_API_DEBUG_TASK_NAME_MAX      15
#endif

static uint8 fs_api_debug_config[FS_API_DEBUG_ID_MAX] =
{
  1, /* FS_API_DEBUG_ID_EFS_OPEN */
  0, /* FS_API_DEBUG_ID_EFS_READ */
  1, /* FS_API_DEBUG_ID_EFS_WRITE */
  1, /* FS_API_DEBUG_ID_EFS_PUT */
  0, /* FS_API_DEBUG_ID_EFS_GET */
  0, /* FS_API_DEBUG_ID_EFS_CLOSE */
  1, /* FS_API_DEBUG_ID_EFS_SYNC */
};

struct fs_api_debug_info
{
  enum fs_api_debug_id id;
  int fd;
  char task_name[FS_API_DEBUG_TASK_NAME_MAX + 1] ;
  char path_name[FS_API_DEBUG_MAX_ENTRIES + 1];
};

static struct fs_api_debug_info fs_api_debug[FS_API_DEBUG_MAX_ENTRIES];
static uint32 fs_api_debug_index = 0;

static void
fs_api_debug_init (void)
{
  memset (fs_api_debug, 0, sizeof (fs_api_debug));
  fs_api_debug_index = 0;
}

static void
fs_api_debug_add (enum fs_api_debug_id id, int fd, const char *file_path)
{
  struct fs_api_debug_info *debug_entry;
  fs_os_tcb_type *curr_tcb;

  FS_ASSERT(id < FS_API_DEBUG_ID_MAX);
  if (!fs_api_debug_config[id])
    return;

  if (fs_api_debug_index >= FS_API_DEBUG_MAX_ENTRIES)
    fs_api_debug_index = 0;

  debug_entry = &fs_api_debug[fs_api_debug_index];
  memset (debug_entry, 0, sizeof (*debug_entry));

  debug_entry->id = id;
  debug_entry->fd = fd;

  curr_tcb = fs_os_self ();
  fs_os_get_task_name (curr_tcb, debug_entry->task_name,
                       sizeof (debug_entry->task_name), NULL);

  if (file_path != NULL)
  {
    strlcpy (debug_entry->path_name, file_path,
             sizeof (debug_entry->path_name));
  }

  ++fs_api_debug_index;
}

#else  /* FEATURE_ENABLE_EFS_API_DEBUG */

static inline void
fs_api_debug_init (void)
{
}

static inline void
fs_api_debug_add (enum fs_api_debug_id id, int fd, const char *file_path)
{
  (void) id; (void) fd; (void) file_path;
}

#endif /* FEATURE_ENABLE_EFS_API_DEBUG */

#ifdef FEATURE_FS_COLDPLUG
static int
efs_boot_fat_mount_helper (enum coldplug_device_type device_type,
    const char *mount_path, int is_embedded_device)
{
  int result;
  struct fs_stat sbuf;
  struct coldplug_iter *citer = NULL;
  struct coldplug_iter_record *citer_rec = NULL;

  memset (&sbuf, 0, sizeof(sbuf));
  result = efs_stat (mount_path, &sbuf);
  if (result != 0) {
    result = -ENOENT;
    goto End;
  }

  citer = coldplug_iter_open (device_type, is_embedded_device, 1);
  if (citer == NULL)
    goto Error;

  citer_rec = coldplug_iter_next (citer);
  if (citer_rec == NULL)
    goto Error;

  result = efs_mount ("", mount_path, "extfs", 0, citer_rec->parti_name);
  if (result != 0)
    goto Error;

  result = 0;
  goto End;

Error:
  result = COLDPLUG_RESULT_MOUNT_FAILED;

End:
  if (citer != NULL)
    coldplug_iter_close(citer);
  return result;
}
#endif


/***********************************************************************
FUNCTION      efs_boot_init_romfs

DESCRIPTION   This function should only be called to initialize efs
              from boot code. Upon return EFS will be initialized.

              It is okay to call this API multiple times and calling
              again will immediately return success.

DEPENDENCIES

RETURN VALUE  If the mount was successful 0, else the error no.

SIDE EFFECTS
**********************************************************************/
int efs_boot_init_romfs_complete = 0;

int
efs_boot_init_romfs (void)
{
  int result;
  int i;

  static const struct efs_romfs_node fs_boot_nodes[] =
  {
    {"/", NULL, 0},
    {EFS_BOOT_INTERNAL_FAT, NULL, 0},
    {EFS_BOOT_EXTERNAL_FAT, NULL, 0},
    {NULL, NULL, 0}
  };

  /*-------------------------------------------------------------------------
   * It is okay to call this API multiple times. Calling this API again
   * will immediately return success.
  -------------------------------------------------------------------------*/
  if (efs_boot_init_romfs_complete)
  {
    return 0;
  }
  efs_boot_init_romfs_complete = 1;

  /* Initialize the babylon port */
  EFS_BAB_PORT_INIT ();

  EFS_BAB_START (EFS_BAB_INITIALIZE);

  /* Setup global lock and get memory PD token */
  FS_GLOBAL_LOCK_INIT();

  /* EFS Lock is not initialized yet. Release protected memory */
  fs_efs2_unprotect_memory ();

  fs_global_lock ();
  //FS_PATH_BUFF_LOCK_INIT();

  _fs_catch = NULL;

  fs_desc_init ();
  fs_vnode_init ();

  fs_mount_init ();
  fs_romfs_init ();
  fs_nodevfs_init ();

#ifdef FEATURE_EFS_EXTFS
  fs_extfs_init ();
#endif

  /* Disable long filename support for boot code */
#ifdef FEATURE_EFS_EXTFS_HFAT
  f_enable_lfn_support (FALSE);
#endif

  /* Enable boot mode */
  fs_in_boot_mode = TRUE;

  /* Mount romfs as the root filesystem. */
  result = fs_mount_root ("romfs", (const char *)fs_boot_nodes);
  if (result != 0)
  {
    FS_ERR_FATAL ("Couldn't mount romfs as root!", 0, 0, 0);
  }

  /* All of the iterators are available. */
  for (i = 0; i < FS_MAX_ITERATORS; i++)
    all_iters[i].busy = 0;

  fs_global_unlock ();

  EFS_BAB_END (EFS_BAB_INITIALIZE);
  return result;
}


int
efs_boot_initialize (void)
{
  int result;

  result = efs_boot_init_romfs ();
  if (result != 0)
  {
    FS_ERR_FATAL ("efs_boot_init failed!", 0, 0, 0);
  }

#ifdef FEATURE_FS_COLDPLUG
  result = efs_boot_fat_mount_helper (COLDPLUG_DEVICE_SD,
               EFS_BOOT_INTERNAL_FAT, 1);
  result = efs_boot_fat_mount_helper (COLDPLUG_DEVICE_SD,
               EFS_BOOT_EXTERNAL_FAT, 0);
#endif /* FEATURE_FS_COLDPLUG */

  result = 0;
  return result;
}


#ifdef FEATURE_EFS2_READONLY
int
efs_read_only_initialize (void)
{
  int result = -1;
  int i;

  /* Initialize the babylon port */
  EFS_BAB_PORT_INIT ();

  EFS_BAB_START (EFS_BAB_INITIALIZE);

  /* Setup global lock and get memory PD token */
  FS_GLOBAL_LOCK_INIT();

  /* EFS Lock is not initialized yet. Release protected memory */
  fs_efs2_unprotect_memory ();

  fs_global_lock ();

  FS_PATH_BUFF_LOCK_INIT();

  _fs_catch = NULL;

  fs_malloc_init ();

  fs_device_init ();

  fs_upgrade_init ();
  fs_desc_init ();
  fs_vnode_init ();
  fs_mount_init ();

#ifdef FEATURE_EFS_QEFS2
  fs_efs2_read_only_init ();
#endif

  fs_nodevfs_init ();

  /* Mount the root filesystem.
   * If mounting the root fails, which can happen during first ever boot
   * we mount a "nodev" so that any EFS APIs called can return gracefully
   * indicating an error */
  result = fs_mount_root ("EFS2_read_only", "");
  if (result != 0)
  {
    result = fs_mount_root ("nodev", NULL);
    if (result != 0)
    {
      FS_ERR_FATAL ("Couldn't mount nodev!", 0, 0, 0);
    }
  }

  /* All of the iterators are available. */
  for (i = 0; i < FS_MAX_ITERATORS; i++)
    all_iters[i].busy = 0;

  /* No further memory allocations are allowed after efs_initialize. */
  fs_disable_malloc ();
  fs_global_unlock ();

  EFS_BAB_END (EFS_BAB_INITIALIZE);

  return result;
}
#endif /* FEATURE_EFS2_READONLY */

/***********************************************************************
FUNCTION      efs_initialize

DESCRIPTION   This function should be called to mount the
              root file system. It calls the start function specified
              in the fs_mount_ops structure to do the actual initialization
              where both the file system and the dbtree are initialized.

DEPENDENCIES

RETURN VALUE  If the mount was successful 0, else the error no.

SIDE EFFECTS
**********************************************************************/
#ifndef FEATURE_EFS_BOOT_ENV

static void fs_init_complete (void);
static void efs_golden_copy_init (void);
static void efs_golden_copy_restore (void);


/* Static struct to hold the efs init flash counters. */
static struct fs_flash_counters efs_initialize_flash_counters;

int
efs_initialize (void)
{
  struct fs_profile_handle profile_handle;
  fs_timetick start_timetick, net_timeticks;
  uint32 total_time_in_us;
  int result;
  int i;

  /* Initialize the babylon port */
  EFS_BAB_PORT_INIT ();

  EFS_BAB_START (EFS_BAB_INITIALIZE);

  /* Setup global lock and get memory PD token */
  fs_sync_lock_init ();
  FS_GLOBAL_LOCK_INIT();

  /* EFS Lock is not initialized yet. Release protected memory */
  fs_efs2_unprotect_memory ();

  fs_sync_lock ();
  fs_global_lock ();

  FS_PATH_BUFF_LOCK_INIT();

  _fs_catch = NULL;

  /* Get the reference of the File System TCB for easy access. */
  efs_tcb = fs_os_self ();

  fs_api_debug_init ();

  /* Reset flash and hotplug counters */
  fs_counters_init ();

  /* Reset the ram variable that holds the efs init flash counters. */
  memset (&efs_initialize_flash_counters, 0xFF,
          sizeof(efs_initialize_flash_counters));

  /* Initialize the timetick api's */
  fs_timetick_init ();

  /* We can only get the timetick after we initialize it. */
  start_timetick = fs_timetick_get_timetick ();

  fs_timings_init ();

  fs_malloc_init ();

  fs_device_init ();

  fs_ftl_init ();

#ifdef FEATURE_FS_HOTPLUG
  hotplug_init ();
#endif

  fs_upgrade_init ();
  fs_desc_init ();
  fs_vnode_init ();
  fs_mount_init ();

#ifdef FEATURE_EFS_QEFS2
  fs_efs2_init ();
#endif

  fs_romfs_init ();

  fs_nodevfs_init ();
#ifdef FEATURE_EFS_EXTFS
  fs_extfs_init ();
#endif

#ifdef FEATURE_FS_TAR
  fs_tar_init ();
#endif

  fs_zip_init ();

#ifdef FEATURE_FS_ZIP_TAR
  fs_zip_tar_init ();
#endif

  fs_cookie_init ();

  /* Mount the root filesystem. */
  result = fs_mount_root (FS_ROOT_FS_NAME, "");

  if (result != 0)
    FS_ERR_FATAL ("No flash device found", 0, 0, 0);

  /* All of the iterators are available. */
  for (i = 0; i < FS_MAX_ITERATORS; i++)
    all_iters[i].busy = 0;

  memset (&fs_img_handle, 0, sizeof (struct fs_filesystem_image_handle));

#ifndef FEATURE_EFS_COMPILE_WITHOUT_DIAG
  fs_diag_init ();
  fs_diagd_init ();
#endif /* FEATURE_EFS_COMPILE_WITHOUT_DIAG */

  fs_benchmark_init();

  /* Create the efs's private meta-data folder */
  efs_privatedir_init ();

  /* Init the profile module. */
  fs_profile_init ();

#ifdef FEATURE_FS_HOTPLUG
  hotplug_task_init ();
#endif

  /* Rollback all files that were not closed in previous boot session */
  efs_rollback_recover ();

  /* Initialize stdio related functions */
  efs_estdio_init ();
  efs_stdio_init ();

  /* No further memory allocations are allowed after efs_initialize. */
  fs_disable_malloc ();

  fs_otp_init ();

  fs_init_complete ();

  net_timeticks = fs_timetick_get_timetick () - start_timetick;
  total_time_in_us = (uint32) fs_timetick_timetick_diff_to_us (net_timeticks);

  /* Take the flash counter snapshot for all the init calls. */
  fs_counters_take_snaphot (&efs_initialize_flash_counters);

  /* We start the profiling after the init and log the total time as an integer
   * to allow for the profile init to occur before the actual profiling.  */
  fs_profile_open (&profile_handle, FS_PROFILE_ID_EFS_INITIALIZE);
  fs_profile_start (&profile_handle);
  fs_profile_stop (&profile_handle);

  fs_profile_log_int (&profile_handle, total_time_in_us);

  fs_profile_close (&profile_handle);

  efs_golden_copy_init ();

  efs_quota_init_for_clients ();

  fs_global_unlock ();
  fs_sync_unlock ();

  EFS_BAB_END (EFS_BAB_INITIALIZE);
  return result;
}

static void
fs_init_complete (void)
{
  struct nameidata *ndp = &nameidata_buffer;
  struct fs_vnode *vp;
  int result;

  fs_global_lock ();
  EFS_BAB_START (EFS_BAB_FS_INIT_COMPLETE);

  result = fs_namei (ndp, "/", O_NOFOLLOW);
  if (result != 0)
  {
    FS_ERR_FATAL ("cannot find root FS",0,0,0);
  }

  if (ndp->dvp != NULL)
    fs_vnode_unref (ndp->dvp);

  vp = ndp->vp;
  ndp->vp = NULL;
  ASSERT (vp != NULL);

#ifdef FS_UNIT_TEST
  fs_init_complete_notify ();
#endif

  result = vp->vops->fs_init_complete (vp);

  fs_vnode_unref (vp);

  if (result < 0)
    EFS_BAB_RETURN (EFS_BAB_FS_INIT_COMPLETE, -result);

  EFS_BAB_END (EFS_BAB_FS_INIT_COMPLETE);

  fs_global_unlock ();

}

#endif /* FEATURE_EFS_BOOT_ENV */


/***********************************************************************
FUNCTION      efs_power_down

DESCRIPTION   This function calls the file system finalize function to
              make sure the shut down is handled correctly and normally.

DEPENDENCIES  None

RETURN VALUE  If the shut down was successful 0.

SIDE EFFECTS  None
**********************************************************************/
int
efs_power_down (void)
{
  int rv;

  EFS_BAB_API (EFS_BAB_POWER_DOWN);

  /* Must first grab global lock and unprotect memory,
   * fs_root_mpoint is protected under fs_mount.c. */
  fs_global_lock ();

  /* XXX: Should unmount everything. */
  rv = fs_root_mpoint->ops->stop (fs_root_mpoint);

  fs_global_unlock ();
  return rv;
}


/* Check whether given input file/dir path is valid or not. */
int
fs_is_path_valid (const char *path)
{
  if (path == NULL)
    return -1;

  if (path[0] == '\0')
    return -1;

  return 0;
}

/* Open/create a file. */
int
efs_open (const char *path, int oflag, ...)
{
  fs_mode_t             mode = 0;
  struct nameidata *ndp = &nameidata_buffer;
  int result = 0, fd = -1;
  struct fs_descriptor *file = 0;
  struct fs_profile_handle profile_handle;

  /* Check for the filepath validity. */
  result = fs_is_path_valid (path);
  if (result != 0)
  {
    efs_errno = EINVAL;
    return -1;
  }

  /* Posix requires the optional third argument for the mode.  Extract this
   * if we are creating a file.
   * Lint is so very very confused by va_arg.  Suppress everything it finds. */
/*lint -save -e746 -e10 -e64 -e718 -e529 */
  if ((oflag & O_CREAT) != 0)
  {
    va_list arg;
    memset(&arg, 0, sizeof(arg));
    va_start (arg, oflag);
    mode = va_arg (arg, int);
    va_end (arg);
  }
/*lint -restore */

  fs_global_lock ();
  EFS_BAB_START (EFS_BAB_OPEN);

  EFS_BAB_PRINT (EFS_BAB_OPEN, path);
  EFS_BAB_VALUE (EFS_BAB_OPEN, (uint32) oflag, FALSE, FALSE, TRUE);

  fs_profile_open (&profile_handle, FS_PROFILE_ID_OPEN);
  fs_profile_start (&profile_handle);

  /* Allocate a descriptor. */
  file = fs_desc_alloc ();
  if (file == NULL) {
    efs_errno = EMFILE;
    goto error_case;
  }

  result = fs_namei (ndp, path, oflag);

  if (result != 0) {
    goto error_case;
  }

  /* Don't allow open on device special files */
  if (ndp->vp && (S_ISSPECIAL(ndp->vp->mode))) {
    efs_errno = EEXIST;
    fs_vnode_unref (ndp->dvp);
    fs_vnode_unref (ndp->vp);
    goto error_case;
  }

  /* If the name lookup succeeds, return an error if the user is attempting
   * to open a directory with O_WRONLY or O_RDWR specified.
   */
  if ((ndp->vp != NULL) && S_ISDIR (ndp->vp->mode) &&
      ((oflag & O_RDWR) || (oflag & O_WRONLY))) {
    efs_errno = EISDIR;
    fs_vnode_unref (ndp->dvp);
    fs_vnode_unref (ndp->vp);
    goto error_case;
  }

  /* fs_namei already checked the open flags for O_CREAT and O_EXCL, so, if
   * we get this far, we can either create, or open the file. */

  if (ndp->vp == NULL) {
    result = ndp->dvp->vops->create (ndp, mode);
    if (result < 0) {
      fs_vnode_unref (ndp->dvp);
      goto error_case;
    }
  }

  /* Now, ndp->vp points to our file.  We don't need the directory any
   * more, so unreference that. */
  fs_vnode_unref (ndp->dvp);
  ndp->dvp = 0;

  /* If the file should be truncated (and was opened for writing). */
  if ((oflag & O_TRUNC) &&
      (((oflag & O_ACCMODE) + 1 ) & 2) != 0)
  {
    result = ndp->vp->vops->truncate (ndp->vp, 0);
    /* XXX: What do we do if there was a problem truncating the file? */
  }

  /* XXX: Handle different operation types. */
  /* Fill in the descriptor */
  file->state = FS_DESC_STATE_FILE;

  /* Move the referenced vnode into the descriptor. */
  file->vp = ndp->vp;
  ndp->vp = 0;

  file->mode = oflag;
  file->file_pos = 0;

  file->dops = &public_desc_ops;

  EFS_BAB_RETURN (EFS_BAB_OPEN, file->fd);
  EFS_BAB_END (EFS_BAB_OPEN);

  /* Copy the file descriptor */
  fd = file->fd;

  fs_profile_stop (&profile_handle);
  fs_profile_log_int (&profile_handle, fd);
  fs_profile_log_int (&profile_handle, oflag);
  fs_profile_log_int (&profile_handle, 0); /* Errno is sucess */
  fs_profile_log_string (&profile_handle, path);
  fs_profile_close (&profile_handle);

  fs_api_debug_add (FS_API_DEBUG_ID_EFS_OPEN, fd, path);

  fs_global_unlock ();

  return fd;

error_case:
  if (file != NULL)
    fs_desc_free (file);
  if (result < 0)
    efs_errno = -result;
  EFS_BAB_RETURN (EFS_BAB_OPEN, -result);
  EFS_BAB_END (EFS_BAB_OPEN);

  fs_profile_stop (&profile_handle);
  fs_profile_log_int (&profile_handle, fd);
  fs_profile_log_int (&profile_handle, oflag);
  fs_profile_log_int (&profile_handle, -result);
  fs_profile_log_string (&profile_handle, path);
  fs_profile_close (&profile_handle);

  fs_global_unlock ();
  return -1;
}

static fs_ssize_t
_efs_read_core (int filedes, void *buf, fs_size_t nbyte)
{
  struct fs_descriptor *desc_ptr;
  int result;

  if ((desc_ptr = fs_desc_lookup (filedes)) == NULL)
  {
    efs_errno = EBADF;
    return -1;
  }

  result = desc_ptr->dops->read (desc_ptr, buf, nbyte);

  if (result < 0) {
    efs_errno = -result;
    return -1;
  } else
    return result;
}

fs_ssize_t
efs_read (int filedes, void *buf, fs_size_t nbyte)
{
  fs_ssize_t result;
  struct fs_profile_handle profile_handle;

#ifndef FEATURE_EFS_BOOT_ENV
  /* check validity for the input buffer when not in boot context. */
  if (buf == NULL)
  {
    efs_errno = EINVAL;
    return -1;
  }
#endif
  /* Just return if asked to read zero bytes. */
  if (nbyte == 0)
    return 0;

  fs_global_lock ();

  fs_profile_open (&profile_handle, FS_PROFILE_ID_READ);
  fs_profile_start (&profile_handle);

  EFS_BAB_START (EFS_BAB_READ);
  EFS_BAB_VALUE (EFS_BAB_READ, (uint16)filedes, TRUE, TRUE, FALSE);
  EFS_BAB_VALUE (EFS_BAB_READ, (uint32)nbyte, TRUE, TRUE, TRUE);
  result = _efs_read_core (filedes, buf, nbyte);
  EFS_BAB_RETURN (EFS_BAB_READ, result);
  EFS_BAB_END (EFS_BAB_READ);

  fs_profile_stop (&profile_handle);
  fs_profile_log_int (&profile_handle, filedes);
  fs_profile_log_int (&profile_handle, nbyte);
  if (result < 0)
  {
    fs_profile_log_int (&profile_handle, efs_errno);
  }
  else
  {
    fs_profile_log_int (&profile_handle, 0);
  }
  fs_profile_close (&profile_handle);

  fs_api_debug_add (FS_API_DEBUG_ID_EFS_READ, filedes, NULL);

  fs_global_unlock ();

  return result;
}

static fs_ssize_t
fs_public_desc_read (struct fs_descriptor *file,
    void *buf, fs_size_t count)
{
  fs_ssize_t result;

  /* Check permissions, use the same trick described in
   * fs_public_desc_write. */
  if ((((file->mode & O_ACCMODE) + 1) & 1) == 0) {
    return -EBADF;
  }

  /* Do the actual read. */
  if (count > 0)
    result = file->vp->vops->read (file->vp, file->file_pos, buf, count);
  else
    result = 0;

  /* Adjust the file pointer. */
  if (result > 0) {
    file->file_pos += result;
  }

  return result;
}

static fs_ssize_t
_efs_write_core (int filedes, const void *buf, fs_size_t nbyte)
{
  struct fs_descriptor *desc_ptr;
  int result;

  if ((desc_ptr = fs_desc_lookup (filedes)) == NULL)
  {
    efs_errno = EBADF;
    return -1;
  }

  result = desc_ptr->dops->write (desc_ptr, buf, nbyte);

  /* It is not an error to write fewer bytes than requested. However, we
   * consider it an error if not even one byte could be written.
   */
  if (result <= 0) {
    if (result == 0)
      result = -ENOSPC;
    efs_errno = -result;
    return -1;
  } else
    return result;
}

fs_ssize_t
efs_write (int filedes, const void *buf, fs_size_t nbyte)
{
  fs_ssize_t result;
  struct fs_profile_handle profile_handle;

#ifndef FEATURE_EFS_BOOT_ENV
  /* check validity of the input buffer. */
  if (buf == NULL)
  {
    efs_errno = EINVAL;
    return -1;
  }
#endif

  /* Just return if asked to write zero bytes. */
  if (nbyte == 0)
    return 0;

  fs_global_lock ();

  fs_profile_open (&profile_handle, FS_PROFILE_ID_WRITE);
  fs_profile_start (&profile_handle);

  EFS_BAB_START (EFS_BAB_WRITE);
  EFS_BAB_VALUE (EFS_BAB_WRITE, (uint16)filedes, TRUE, TRUE, FALSE);
  EFS_BAB_VALUE (EFS_BAB_WRITE, (uint32)nbyte, TRUE, TRUE, TRUE);
  result = _efs_write_core (filedes, buf, nbyte);
  EFS_BAB_RETURN (EFS_BAB_WRITE, result);
  EFS_BAB_END   (EFS_BAB_WRITE);

  fs_profile_stop (&profile_handle);
  fs_profile_log_int (&profile_handle, filedes);
  fs_profile_log_int (&profile_handle, nbyte);
  if (result < 0)
  {
    fs_profile_log_int (&profile_handle, efs_errno);
  }
  else
  {
    fs_profile_log_int (&profile_handle, 0);
  }

  fs_profile_close (&profile_handle);
  fs_api_debug_add (FS_API_DEBUG_ID_EFS_WRITE, filedes, NULL);

  fs_global_unlock ();
  return result;
}

static fs_ssize_t
fs_public_desc_write (struct fs_descriptor *file,
                      const void *buf, fs_size_t count)
{
  fs_ssize_t result;
  fs_size_t  write_pos;

  /* Check that we have permissions to write this file.  This depends on
   * the specific values of O_RDONLY=0, O_WRONLY=1, and O_RDWR=2 that posix
   * use.  By adding 1, the value is just a pair of bits with the lowest
   * bit indicating read, the the next bit indicating write.  Neat, isn't
   * it. */

  if ((((file->mode & O_ACCMODE) + 1) & 2) == 0) {
    return -EBADF;
  }

  write_pos = file->file_pos;

  /* Do the actual write. */
  if (count > 0) {
    result = file->vp->vops->write (file->vp, write_pos, buf,
                                    count, file->mode);

    /* Adjust the file pointer appropriately. */
    if (result > 0) {
      if (file->mode & O_APPEND) {
        struct fs_stat sbuf;
        uint32 numbytes;
        if (file->vp->vops->getsize (file->vp, &numbytes) == 0)
          file->file_pos = numbytes;
        else if (file->vp->vops->getstat (file->vp, &sbuf) == 0)
          file->file_pos = sbuf.st_size;
      } else {
        file->file_pos += result;
      }
    }
  }
  else
    result = 0;

  return result;
}

/* efs_mkdir */
int
efs_mkdir (const char *path, fs_mode_t mode)
{
  struct nameidata *ndp = &nameidata_buffer;
  int result;
  struct fs_profile_handle profile_handle;

  /* Check for the dir path validity. */
  result = fs_is_path_valid (path);
  if (result != 0)
  {
    efs_errno = EINVAL;
    return -1;
  }

  fs_global_lock ();
  EFS_BAB_START (EFS_BAB_MKDIR);
  EFS_BAB_PRINT (EFS_BAB_MKDIR, path);

  fs_profile_open (&profile_handle, FS_PROFILE_ID_MKDIR);
  fs_profile_start (&profile_handle);

  result = fs_namei (ndp, path, O_EXCL | O_CREAT);

  if (result < 0)
  {
    goto clean_up;
  }

  result = ndp->dvp->vops->mkdir (ndp, mode);

  if (ndp->dvp != NULL)
    fs_vnode_unref (ndp->dvp);
  if (ndp->vp != NULL)
    fs_vnode_unref (ndp->vp);

clean_up:

  fs_profile_stop (&profile_handle);
  fs_profile_log_int (&profile_handle, mode);
  fs_profile_log_int (&profile_handle, -result);
  fs_profile_log_string (&profile_handle, path);
  fs_profile_close (&profile_handle);

  if (result != 0)
    EFS_BAB_RETURN (EFS_BAB_MKDIR, -result);
  EFS_BAB_END (EFS_BAB_MKDIR);
  fs_global_unlock ();
  if (result == 0)
    return 0;
  else {
    efs_errno = -result;
    return -1;
  }


}

int
efs_creat (const char *path, fs_mode_t mode)
{
  int result;

  /* Check for the filepath validity. */
  result = fs_is_path_valid (path);
  if (result != 0)
  {
    efs_errno = EINVAL;
    return -1;
  }

  EFS_BAB_API (EFS_BAB_CREAT);
  return efs_open (path, O_CREAT | O_WRONLY | O_TRUNC, mode);
}

int
efs_close (int filedes)
{
  struct fs_descriptor *desc_ptr;
  int result = 0;
  struct fs_profile_handle profile_handle;

  fs_global_lock ();

  fs_profile_open (&profile_handle, FS_PROFILE_ID_CLOSE);
  fs_profile_start (&profile_handle);

  EFS_BAB_START (EFS_BAB_CLOSE);
  EFS_BAB_VALUE (EFS_BAB_CLOSE, (uint16)filedes, TRUE, TRUE, FALSE);

  /* Check if file descriptor is valid */
  if ((desc_ptr = fs_desc_lookup (filedes)) == NULL)
  {
    result = -EBADF;
  } else {
    result = desc_ptr->dops->close (desc_ptr);
    fs_desc_free (desc_ptr);
  }

  EFS_BAB_END (EFS_BAB_CLOSE);

  fs_profile_stop (&profile_handle);
  fs_profile_log_int (&profile_handle, filedes);
  fs_profile_log_int (&profile_handle, -result);
  fs_profile_close (&profile_handle);

  fs_api_debug_add (FS_API_DEBUG_ID_EFS_CLOSE, filedes, NULL);

  fs_global_unlock ();

  if (result < 0)
  {
    efs_errno = -result;
    result = -1;
  }

  return result;
}

/* Close handler for filesystem (through vnode) operations. */
static int
fs_public_desc_close (struct fs_descriptor *file)
{
  /* Dereference the vnode.  There should be no need to flush. */
  file->state = 0;
  fs_vnode_unref (file->vp);
  file->vp = 0;

  return 0;
}

/* Helper function for lseek api. */
static fs_off64_t
efs_lseek_helper (int filedes, fs_off64_t offset, int whence,
                  enum fs_restrict_api_to_gb restrict_to_gb)
{
  struct fs_descriptor *desc_ptr;
  uint32 cur_file_pos = 0;
  fs_off64_t new_file_pos = -1;

  if (offset > FS_MAX_4GB_VALUE || offset < FS_MIN_4GB_VALUE) {
    efs_errno = EINVAL;
    return -1;
  }

  /* Check if file descriptor is valid */
  if ((desc_ptr = fs_desc_lookup (filedes)) == NULL) {
    efs_errno = EBADF;
    return -1;
  }

  /* This is only valid with files. */
  if (desc_ptr->state != FS_DESC_STATE_FILE) {
    efs_errno = ESPIPE;
    return -1;
  }

  switch (whence)
  {
    case SEEK_SET:
      cur_file_pos = 0;
      break;

    case SEEK_CUR:
      cur_file_pos = desc_ptr->file_pos;
      break;

    case SEEK_END:
      /* The size of the file is first obtained using getsize function. It
       * might fail either if 'getsize' is not defined for the file system
       * or if the call cannot be performed on this vp.
       * In that case, we try getstat() to obtain the size */
      {
        uint32 num_bytes;
        int result = 0;

        result = desc_ptr->vp->vops->getsize (desc_ptr->vp, &num_bytes);

        if (result == 0)
          cur_file_pos = num_bytes;
        else
        {
          static struct  fs_stat sbuf;
          result = desc_ptr->vp->vops->getstat (desc_ptr->vp, &sbuf);

          if (result == 0)
            cur_file_pos = sbuf.st_size;
          else {
            efs_errno = -result;
            return -1;
          }
        }
        break;
      }

      default:
      efs_errno = EINVAL;
      return -1;
    }

  new_file_pos = (fs_off64_t) cur_file_pos;
  new_file_pos += offset;

  if (new_file_pos < 0) {
    efs_errno = EINVAL;
    return -1;
  }

  /* Check if we have to restrict it to (2GB-1) bytes.
   * If new_file_pos is greater then FS_MAX_2GB_VALUE then return Error. */
  if ( (restrict_to_gb == FS_RESTRICT_API_TO_2GB) && \
       ((uint32)new_file_pos > FS_MAX_2GB_VALUE) ) {
    efs_errno = EINVAL;
    return -1;
  }

  /* Check for the underrun and overrun of the filesize
   * If result + offset lies between 0 and 4GB then only change file_pos
   * value or else throw error and set errno as EINVAL. */
  if ( (new_file_pos >= 0) && (new_file_pos <= FS_MAX_4GB_VALUE) ) {
    desc_ptr->file_pos = (uint32) new_file_pos;
  } else {
    efs_errno = EINVAL;
    return -1;
  }

  return new_file_pos;
}


fs_off_t
efs_lseek (int filedes, fs_off_t offset, int whence)
{
  fs_off64_t new_file_pos, offset_arg;
  fs_off_t result = -1;

  fs_global_lock ();
  EFS_BAB_START (EFS_BAB_LSEEK);
  EFS_BAB_VALUE (EFS_BAB_LSEEK, (uint16)filedes, TRUE, TRUE, FALSE);
  EFS_BAB_VALUE (EFS_BAB_LSEEK, (uint32)offset,  TRUE, TRUE, TRUE);
  EFS_BAB_VALUE (EFS_BAB_LSEEK, (uint16)whence,  TRUE, TRUE, FALSE);

  offset_arg = (fs_off64_t) offset;

  /* perform seek operation till 2GB file_pos. */
  new_file_pos = efs_lseek_helper (filedes,
                                   offset_arg,
                                   whence,
                                   FS_RESTRICT_API_TO_2GB);

  /* check if new_file_pos is in range of 0 to 2GB. */
  if ((new_file_pos >= 0) && (new_file_pos <= FS_MAX_2GB_VALUE))
    result = (fs_off_t) new_file_pos;

  EFS_BAB_RETURN (EFS_BAB_LSEEK, result);
  EFS_BAB_END (EFS_BAB_LSEEK);
  fs_global_unlock ();

  return result;
}

fs_off64_t
efs_lseek64 (int filedes, fs_off64_t offset, int whence)
{
  fs_off64_t result = -1;

  fs_global_lock ();
  EFS_BAB_START (EFS_BAB_LSEEK64);
  EFS_BAB_VALUE (EFS_BAB_LSEEK64, (uint16)filedes, TRUE, TRUE, FALSE);
  EFS_BAB_VALUE (EFS_BAB_LSEEK64, (uint32)offset,  TRUE, TRUE, TRUE);
  EFS_BAB_VALUE (EFS_BAB_LSEEK64, (uint16)whence,  TRUE, TRUE, FALSE);

  result = efs_lseek_helper (filedes,
                             offset,
                             whence,
                             FS_RESTRICT_API_TO_4GB);

  EFS_BAB_RETURN (EFS_BAB_LSEEK64, result);
  EFS_BAB_END (EFS_BAB_LSEEK64);
  fs_global_unlock ();

  return result;
}

/* Helper function for truncate api. */
static int
efs_truncate_helper (const char *path, uint32 length, uint32 babylon_id)
{
  struct nameidata *ndp = &nameidata_buffer;
  int result;

  /* Check for the file path validity. */
  result = fs_is_path_valid (path);
  if (result != 0)
  {
    efs_errno = EINVAL;
    return -1;
  }

  fs_global_lock ();
  EFS_BAB_START (babylon_id);

  result = fs_namei (ndp, path, 0);

  if (result == 0)
    result = ndp->vp->vops->truncate (ndp->vp, length);

  if (result < 0)
  {
    efs_errno = -result;
    result = -1;
  }

  if (ndp->dvp != NULL)
    fs_vnode_unref (ndp->dvp);
  if (ndp->vp != NULL)
    fs_vnode_unref (ndp->vp);

  EFS_BAB_END (babylon_id);
  fs_global_unlock ();
  return result;
}

fs_off_t
efs_truncate (const char *path, fs_off_t length)
{
  int result = -1;

  /* Check if the input length is in range of 0 to 2GB.
   * If length is greater than 2GB then would rollover to negative number. */
  if (length < 0) {
    efs_errno = EINVAL;
    return -1;
  }

  /* call the truncate helper function. */
  result = efs_truncate_helper (path, (uint32) length, EFS_BAB_TRUNCATE);

  return result;
}

fs_off64_t
efs_truncate64 (const char *path, fs_off64_t length)
{
  fs_off64_t result = -1;

  /* Check if the input length is in range of 0 to 4GB. */
  if ( (length < 0) || (length > FS_MAX_4GB_VALUE) ) {
    efs_errno = EINVAL;
    return -1;
  }

  /* call the truncate helper function. */
  result = (fs_off64_t) efs_truncate_helper (path,
                                             (uint32) length,
                                             EFS_BAB_TRUNCATE64);

  return result;
}

/* Helper function for ftruncate api. */
static int
efs_ftruncate_helper (int fd, uint32 length, uint32 babylon_id)
{
  struct fs_descriptor *desc;
  int result;

  fs_global_lock ();
  EFS_BAB_START (babylon_id);

  desc = fs_desc_lookup (fd);
  if (desc == NULL) {
    result = -EBADF;
    goto clean_up;
  }

  /* Check that we have permissions to write this file.  Use the trick
   * described in fs_public_desc_write.
   */
  if ((((desc->mode & O_ACCMODE) + 1) & 2) == 0) {
    result = -EINVAL;
    goto clean_up;
  }

  result = desc->vp->vops->truncate (desc->vp, length);

clean_up:
  EFS_BAB_END (babylon_id);
  fs_global_unlock ();
  if (result < 0) {
    efs_errno = -result;
    return -1;
  } else
    return result;
}

fs_off_t
efs_ftruncate (int fd, fs_off_t length)
{
  int result = -1;

  /* Check if the input length is in range of 0 to 2GB.
   * If length is greater than 2GB then would rollover to negative number. */
  if (length < 0) {
    efs_errno = EINVAL;
    return -1;
  }

  /* call the ftruncate helper function. */
  result = efs_ftruncate_helper (fd, (uint32) length, EFS_BAB_FTRUNCATE);

  return result;
}

fs_off64_t
efs_ftruncate64 (int fd, fs_off64_t length)
{
  fs_off64_t result = -1;

  /* Check if the input length is in range of 0 to 4GB. */
  if ( (length < 0) || (length > FS_MAX_4GB_VALUE) ) {
    efs_errno = EINVAL;
    return -1;
  }

  /* call the ftruncate helper function. */
  result = (fs_off64_t) efs_ftruncate_helper (fd,
                                              (uint32) length,
                                              EFS_BAB_FTRUNCATE64);

  return result;
}

int
efs_stat (const char *path, struct fs_stat *buf)
{
  struct nameidata *ndp = &nameidata_buffer;
  int result;
  struct fs_vnode *vp;

  /* check validity for the file path and input buffer. */
  result = fs_is_path_valid (path);
  if ( (result != 0) || (buf == NULL) )
  {
    efs_errno = EINVAL;
    return -1;
  }

  /* Clean the stat structure, it may carry junk data. */
  memset (buf, 0, sizeof (struct fs_stat));

  fs_global_lock ();
  EFS_BAB_START (EFS_BAB_STAT);
  EFS_BAB_PRINT (EFS_BAB_STAT, path);

  result = fs_namei (ndp, path, 0);

  if (result == 0) {
    vp = ndp->vp;
    result = vp->vops->getstat (vp, buf);
  }

  if (ndp->dvp != NULL)
    fs_vnode_unref (ndp->dvp);
  if (ndp->vp != NULL)
    fs_vnode_unref (ndp->vp);

  if (result < 0)
    EFS_BAB_RETURN(EFS_BAB_STAT, -result);

  EFS_BAB_END (EFS_BAB_STAT);
  fs_global_unlock ();

  if (result < 0) {
    efs_errno = -result;
    return -1;
  } else
    return 0;
}

static int
efs_fstat_core (int fd, struct fs_stat *buf)
{
  struct fs_descriptor *desc_ptr;
  struct fs_vnode      *vp;
  int result;

  /* Check if file descriptor is valid */
  if ((desc_ptr = fs_desc_lookup (fd)) == NULL)
  {
    efs_errno = EBADF;
    return -1;
  }

  /* This is only valid with files. */
  if (desc_ptr->state != FS_DESC_STATE_FILE) {
    efs_errno = ESPIPE;
    return -1;
  }

  vp = desc_ptr->vp;

  result = vp->vops->getstat (vp, buf);

  if (result == 0)
    return 0;
  else {
    efs_errno = -result;
    return -1;
  }
}

int
efs_fstat (int fd, struct fs_stat *buf)
{
  int result;

  if (buf == NULL)
  {
    efs_errno = EINVAL;
    return -1;
  }

  /* Clean the stat structure, it may carry junk data. */
  memset (buf, 0, sizeof (struct fs_stat));

  fs_global_lock ();
  EFS_BAB_START (EFS_BAB_FSTAT);
  EFS_BAB_VALUE (EFS_BAB_FSTAT, (uint16)fd, TRUE, TRUE, FALSE);

  result = efs_fstat_core (fd, buf);

  if (result != 0)
    EFS_BAB_RETURN (EFS_BAB_FSTAT, - efs_errno);

  EFS_BAB_END (EFS_BAB_FSTAT);
  fs_global_unlock ();
  return result;
}



int
efs_lstat (const char *path, struct fs_stat *buf)
{
  struct nameidata *ndp = &nameidata_buffer;
  int result;
  struct fs_vnode *vp;

  /* check validity for the file path and input buffer. */
  result = fs_is_path_valid (path);
  if ( (result != 0) || (buf == NULL) )
  {
    efs_errno = EINVAL;
    return -1;
  }

  /* Clean the stat structure, it may carry junk data. */
  memset (buf, 0, sizeof (struct fs_stat));

  fs_global_lock ();
  EFS_BAB_START (EFS_BAB_LSTAT);
  EFS_BAB_PRINT (EFS_BAB_LSTAT, path);

  result = fs_namei (ndp, path, O_NOFOLLOW);

  if (result != 0)
    goto error_case;

  vp = ndp->vp;

  result = vp->vops->getstat (vp, buf);

error_case:
  if (ndp->dvp != NULL)
    fs_vnode_unref (ndp->dvp);
  if (ndp->vp != NULL)
    fs_vnode_unref (ndp->vp);

  if (result < 0)
    EFS_BAB_RETURN (EFS_BAB_LSTAT, -result);
  EFS_BAB_END (EFS_BAB_LSTAT);
  fs_global_unlock ();

  if (result < 0) {
    efs_errno = -result;
    return -1;
  } else
    return 0;
} /* efs_lstat */

int
efs_statvfs (const char *path, struct fs_statvfs *buf)
{
  struct nameidata *ndp = &nameidata_buffer;
  int result;
  struct fs_vnode *vp;

  /* check validity for the path name and input buffer. */
  result = fs_is_path_valid (path);
  if ( (result != 0) || (buf == NULL) )
  {
    efs_errno = EINVAL;
    return -1;
  }

  fs_global_lock ();
  EFS_BAB_START (EFS_BAB_STATVFS);
  EFS_BAB_PRINT (EFS_BAB_STATVFS, path);

  result = fs_namei (ndp, path, 0);

  if (result != 0)
    goto error_case;

  vp = ndp->vp;

  result = vp->vops->getstatvfs (vp, buf);

error_case:
  if (ndp->dvp != NULL)
    fs_vnode_unref (ndp->dvp);
  if (ndp->vp != NULL)
    fs_vnode_unref (ndp->vp);

  if (result < 0)
    EFS_BAB_RETURN(EFS_BAB_STATVFS, -result);

  EFS_BAB_END (EFS_BAB_STATVFS);
  fs_global_unlock ();

  if (result < 0)
  {
    efs_errno = -result;
    return -1;
  }

  return 0;
} /* END efs_statvfs */

int
efs_fstatvfs(int fd, struct fs_statvfs *buf)
{
  struct fs_descriptor * desc_ptr;
  struct fs_vnode * vp;
  int result;

  /* check validity for the input buffer. */
  if (buf == NULL)
  {
    efs_errno = EINVAL;
    return -1;
  }

  fs_global_lock();
  EFS_BAB_START (EFS_BAB_FSTATVFS);
  /* Check if file descriptor is valid */
  if ((desc_ptr = fs_desc_lookup (fd)) == NULL)
  {
    result = -EBADF;
    goto error_case;
  }

  /* This is only valid with files. */
  if (desc_ptr->state != FS_DESC_STATE_FILE) {
    result = -ESPIPE;
    goto error_case;
  }

  vp = desc_ptr->vp;

  result = vp->vops->getstatvfs (vp, buf);

error_case:
  EFS_BAB_END (EFS_BAB_FSTATVFS);
  fs_global_unlock ();

  if (result < 0) {
    efs_errno = -result;
    return -1;
  }
  return 0;
}

int
efs_unlink (const char *path)
{
  struct nameidata *ndp = &nameidata_buffer;
  int result;

  /* Check for the file path validity. */
  result = fs_is_path_valid (path);
  if (result != 0)
  {
    efs_errno = EINVAL;
    return -1;
  }

  fs_global_lock ();
  EFS_BAB_START (EFS_BAB_UNLINK);
  EFS_BAB_PRINT (EFS_BAB_UNLINK, path);

  result = fs_namei (ndp, path, O_NOFOLLOW);

  if (result != 0) {
    goto error_case;
  }

  if (!S_ISREG (ndp->vp->mode) &&
      !S_ISLNK (ndp->vp->mode) &&
      !S_ISSPECIAL (ndp->vp->mode) &&
      !S_ISITM (ndp->vp->mode)) {
    /* Posix actually says to return EPERM for this case.  EISDIR seems to
     * make more sense, but we'll comply with Posix. */
    result = -EPERM;
    fs_vnode_unref (ndp->vp);
    goto error_case;
  }

  if ((S_ISREG (ndp->vp->mode)) ||
      (S_ISITM (ndp->vp->mode))) {
    /* If the file is already open, return an error.
     * XXX: This is not ideal behavior. At some point, we should handle
     * unlinking open files.
     */
    if (fs_vnode_getref (ndp->vp) > 1) {
      result = -ETXTBSY;
      fs_vnode_unref (ndp->vp);
      goto error_case;
    }
  }

  result = ndp->dvp->vops->unlink (ndp);

error_case:
  if (ndp->dvp != NULL)
    fs_vnode_unref (ndp->dvp);
  /* The entry vnode was unreferenced by the unlink operation itself. */

  if (result < 0)
    EFS_BAB_RETURN(EFS_BAB_UNLINK, -result);
  EFS_BAB_END (EFS_BAB_UNLINK);
  fs_global_unlock ();

  if (result < 0) {
    efs_errno = -result;
    return -1;
  } else
    return 0;

} /* END efs_unlink */

static EFSDIR *
efs_opendir_core (const char *dirname)
{
  int i;
  EFSDIR *iter;
  struct nameidata *ndp = &nameidata_buffer;
  int result;

  for (i = 0; i < FS_MAX_ITERATORS; i++) {
    if (!all_iters[i].busy)
      break;
  }
  if (i == FS_MAX_ITERATORS) {
    efs_errno = EMFILE;
    return NULL;
  }
  iter = &all_iters[i];

  result = fs_namei (ndp, dirname, 0);

  if (result != 0) {
    iter->iter = NULL;
    goto error_case;
  }

  iter->iter = ndp->vp->vops->opendir (ndp->vp);
  if (iter->iter == NULL) {
    fs_vnode_unref (ndp->vp);
    result = -ENOTDIR;
  } else {
    iter->busy = 1;

    /* Copy the reference in, so don't unref it until close. */
    iter->vp = ndp->vp;
  }

error_case:
  if (ndp->dvp != NULL)
    fs_vnode_unref (ndp->dvp);

  if (result < 0) {
    efs_errno = -result;
    return NULL;
  } else
    return iter;
}

EFSDIR *
efs_opendir (const char *dirname)
{
  int result;
  EFSDIR *d;

  /* Check for the dirname validity. */
  result = fs_is_path_valid (dirname);
  if (result != 0)
  {
    efs_errno = EINVAL;
    return NULL;
  }

  fs_global_lock ();
  EFS_BAB_START (EFS_BAB_OPENDIR);
  EFS_BAB_PRINT (EFS_BAB_OPENDIR, dirname);
  d = efs_opendir_core (dirname);
  if (d == NULL)
    EFS_BAB_RETURN(EFS_BAB_OPENDIR, - efs_errno); /* neg indicates error */
  EFS_BAB_END (EFS_BAB_OPENDIR);
  fs_global_unlock ();
  return d;
}

static struct fs_dirent *
efs_readdir_core (EFSDIR *dirp)
{
  int result;

  if (dirp == NULL || !dirp->busy) {
    efs_errno = EINVAL;
    return 0;
  }

  result = dirp->vp->vops->readdir (dirp->vp, dirp->iter,
                                    &dirp->dirent);

  if (result == -EEOF) {
    dirp->dirent.d_name[0] = '\0';
    return 0;
  }
  else if (result < 0) {
    efs_errno = -result;
    return 0;
  } else {
    return &dirp->dirent;
  }
}

struct fs_dirent *
efs_readdir (EFSDIR *dirp)
{
  struct fs_dirent *d;
  fs_global_lock ();
  EFS_BAB_START (EFS_BAB_READDIR);
  d = efs_readdir_core (dirp);
  if (d)
    EFS_BAB_PRINT (EFS_BAB_READDIR, d->d_name);
  EFS_BAB_END (EFS_BAB_READDIR);
  fs_global_unlock ();
  return d;
}


static int
efs_closedir_core (EFSDIR *dirp)
{
  int result;

  if (dirp == NULL || !dirp->busy) {
    efs_errno = EINVAL;
    return -1;
  }

  result = dirp->vp->vops->closedir (dirp->vp, dirp->iter);

  dirp->busy = 0;
  fs_vnode_unref (dirp->vp);

  if (result < 0) {
    efs_errno = -result;
    return -1;
  } else
    return 0;
}

int
efs_closedir (EFSDIR *dirp)
{
  int result;
  fs_global_lock ();
  EFS_BAB_START (EFS_BAB_CLOSEDIR);
  result = efs_closedir_core (dirp);
  if (result < 0)
    EFS_BAB_RETURN (EFS_BAB_CLOSEDIR, - efs_errno);
  EFS_BAB_END (EFS_BAB_CLOSEDIR);
  fs_global_unlock ();
  return result;
}

int
efs_rename (const char *oldpath, const char *newpath)
{
  struct nameidata *ndp1 = &nameidata_buffer;
  struct nameidata *ndp2 = &nameidata_buf2;
  int result;

  /* Check for the file oldpath and newpath name validity. */
  result = fs_is_path_valid (oldpath);
  if (result != 0)
  {
    efs_errno = EINVAL;
    return -1;
  }
  result = fs_is_path_valid (newpath);
  if (result != 0)
  {
    efs_errno = EINVAL;
    return -1;
  }

  fs_global_lock ();
  EFS_BAB_START (EFS_BAB_RENAME);
  EFS_BAB_PRINT (EFS_BAB_RENAME, oldpath);
  EFS_BAB_PRINT (EFS_BAB_RENAME, newpath);

  ndp1->dvp = ndp1->vp = NULL;
  ndp2->dvp = ndp2->vp = NULL;

  result = fs_namei (ndp1, oldpath, O_NOFOLLOW);

  if (result == 0) {
    result = fs_namei (ndp2, newpath, O_CREAT);
  }

  /* Posix requires rename to the same file name to succeed. So
   * detect that condition since in that case the reference count
   * would show > 1 since the first namei already incremented the
   * reference for that. */
  if (((ndp2->vp != 0) &&
       (S_ISREG (ndp2->vp->mode) || S_ISITM (ndp2->vp->mode))) &&
      (result == 0) && (ndp1->vp != ndp2->vp)) {
    /* If the destination file is open, return an error.  */
    if (fs_vnode_getref (ndp2->vp) > 1)
      result = -ETXTBSY;
  }

  /* old path and new path must be on the same file system */
  if ((result == 0) && (ndp1->dvp->mp != ndp2->dvp->mp))
    result = -EXDEV;

  if (result == 0) {
    result = ndp1->dvp->vops->rename (ndp1, ndp2);
  }

  /* rename call unrefs on error. */
  if (ndp1->dvp != NULL) {
    fs_vnode_unref (ndp1->dvp);
  }
  if (ndp1->vp != NULL) {
    fs_vnode_unref (ndp1->vp);
  }
  if (ndp2->dvp != NULL) {
    fs_vnode_unref (ndp2->dvp);
  }
  if (ndp2->vp != NULL) {
    fs_vnode_unref (ndp2->vp);
  }

  if (result != 0) {
    EFS_BAB_RETURN (EFS_BAB_RENAME, -result);
    efs_errno = -result;
    result = -1;
  }

  EFS_BAB_END (EFS_BAB_RENAME);
  fs_global_unlock ();

  return result;
}

int
efs_rmdir (const char *path)
{
  struct nameidata *ndp = &nameidata_buffer;
  int result;

  /* Check for the file path validity. */
  result = fs_is_path_valid (path);
  if (result != 0)
  {
    efs_errno = EINVAL;
    return -1;
  }

  fs_global_lock ();
  EFS_BAB_START (EFS_BAB_RMDIR);
  EFS_BAB_PRINT (EFS_BAB_RMDIR, path);

  result = fs_namei (ndp, path, 0);

  if (result != 0)
    goto error_case;

  /* If they try to delete '.', or '..', or trailing slash. */
  if (ndp->vp == ndp->dvp ||
      (ndp->length == 2 && ndp->next[0] == '.' && ndp->next[1] == '.'))
  {
    result = -EINVAL;
    goto error_case;
  }

  result = ndp->dvp->vops->rmdir (ndp);

  if (result != 0)
    fs_vnode_unref (ndp->vp);
  ndp->vp = NULL;

error_case:
  if (ndp->dvp != NULL)
    fs_vnode_unref (ndp->dvp);
  if (ndp->vp != NULL)
    fs_vnode_unref (ndp->vp);

  EFS_BAB_END (EFS_BAB_RMDIR);
  fs_global_unlock ();

  if (result < 0) {
    efs_errno = -result;
    return -1;
  } else
    return 0;

} /* END efs_rmdir */

int
efs_chmod (const char *path, fs_mode_t mode)
{
  int result;
  struct nameidata *ndp = &nameidata_buffer;

  /* Check for the file path validity. */
  result = fs_is_path_valid (path);
  if (result != 0)
  {
    efs_errno = EINVAL;
    return -1;
  }

  fs_global_lock ();
  EFS_BAB_START (EFS_BAB_CHMOD);

  /* Check to see if the path is valid */
  result = fs_namei (ndp, path, 0);

  /* If file exists, change the mode on this vp */
  if (result == 0)
    result = ndp->vp->vops->chmod (ndp, mode);

  if (ndp->dvp != NULL)
    fs_vnode_unref (ndp->dvp);
  if (ndp->vp != NULL)
    fs_vnode_unref (ndp->vp);
  EFS_BAB_END (EFS_BAB_CHMOD);
  fs_global_unlock ();

  if (result < 0)
  {
    efs_errno = -result;
    return -1;
  }
  else
    return 0;
} /* END efs_chmod */

/**********************************************************************
 * Mountpoints.
 */
int
efs_mount (const char *special, const char *dir,
    const char *fstype, int flags, const void *data)
{
  int result;
  struct nameidata *ndp = &nameidata_buffer;

  (void) flags;
  (void) special;

  /* Check for the dir path validity and Null check for fstype.
   * Field data can be passed as NULL for nodev mount. */
  result = fs_is_path_valid (dir);
  if ( (result != 0) || (fstype == NULL) )
  {
    efs_errno = EINVAL;
    return -1;
  }

  fs_global_lock ();
  EFS_BAB_START (EFS_BAB_MOUNT);
  EFS_BAB_PRINT (EFS_BAB_MOUNT, dir);

  result = fs_namei (ndp, dir, 0);

  /* Ignore the directory check when compiling for the bootloader.
   * In this scenario we actually just mount over a file in romfs */
  if (result == 0 && !S_ISDIR (ndp->vp->mode) && !fs_in_boot_mode) {
    result = -ENOTDIR;
  }

  if (result == 0) {
    result = fs_mount_mount (ndp->vp, fstype, data);
  }

  if (ndp->dvp != NULL)
    fs_vnode_unref (ndp->dvp);
  if (ndp->vp != NULL)
    fs_vnode_unref (ndp->vp);

  EFS_BAB_END (EFS_BAB_MOUNT);
  fs_global_unlock ();

  if (result < 0) {
    efs_errno = -result;
    return -1;
  } else
    return 0;
}

int
efs_umount (const char *target)
{
  int result;
  struct nameidata *ndp = &nameidata_buffer;
  struct fs_mount *mp = NULL;
  int i;

  /* Check for the unmount target name validity. */
  result = fs_is_path_valid (target);
  if (result != 0)
  {
    efs_errno = EINVAL;
    return -1;
  }

  fs_global_lock ();
  EFS_BAB_START (EFS_BAB_UMOUNT);
  EFS_BAB_PRINT (EFS_BAB_UMOUNT, target);

  result = fs_namei (ndp, target, 0);

  if (result == 0) {
    result = fs_mount_umount (ndp->vp);
    mp = ndp->vp->mp;
  }

  if (result == 0 && mp != NULL) {
    fs_desc_close_and_nodev (mp);

    /* printf ("umount ref = %d\n", ndp->vp->ref); */
    fs_vnode_make_nodev (ndp->vp);

    /* Cleanup any iterators. */
    for (i = 0; i < FS_MAX_ITERATORS; i++) {
      if (all_iters[i].busy &&
          all_iters[i].vp != NULL &&
          all_iters[i].vp->mp == mp)
      {
        /* If this vnode hasn't been obliterated already, clean it up. */
        all_iters[i].vp->vops->closedir (all_iters[i].vp, all_iters[i].iter);
        fs_vnode_make_nodev (all_iters[i].vp);
      }
    }
  }

  if (ndp->dvp != NULL)
    fs_vnode_unref (ndp->dvp);
  if (ndp->vp != NULL)
    fs_vnode_unref (ndp->vp);

#ifdef FS_UNIT_TEST
  if (result == 0) {
    fs_vnode_validate_umount (mp);
  }
#endif

  EFS_BAB_END (EFS_BAB_UMOUNT);
  fs_global_unlock ();

  if (result < 0) {
    efs_errno = -result;
    return -1;
  } else
    return 0;
}

int
efs_remount (const char *oldtarget, const char *newtarget)
{
  int result;
  struct nameidata *ndp1 = &nameidata_buffer;
  struct nameidata *ndp2 = &nameidata_buf2;

  /* Check for the oldtarget and newtarget name validity. */
  result = fs_is_path_valid (oldtarget);
  if (result != 0)
  {
    efs_errno = EINVAL;
    return -1;
  }
  result = fs_is_path_valid (newtarget);
  if (result != 0)
  {
    efs_errno = EINVAL;
    return -1;
  }

  fs_global_lock ();
  EFS_BAB_START (EFS_BAB_REMOUNT);
  EFS_BAB_PRINT (EFS_BAB_REMOUNT, oldtarget);
  EFS_BAB_PRINT (EFS_BAB_REMOUNT, newtarget);
  ndp1->dvp = ndp1->vp = NULL;
  ndp2->dvp = ndp2->vp = NULL;

  result = fs_namei (ndp1, oldtarget, 0);

  if (result == 0 && !S_ISDIR (ndp1->vp->mode)) {
    result = -EINVAL;
  }

  if (result == 0) {
    result = fs_namei (ndp2, newtarget, 0);
  }

  if (result == 0 && !S_ISDIR (ndp2->vp->mode)) {
    result = -ENOTDIR;
  }

  if (result == 0) {
    result = fs_mount_remount (ndp1->vp, ndp2->vp);
  }

  if (ndp1->dvp != NULL) {
    fs_vnode_unref (ndp1->dvp);
  }
  if (ndp1->vp != NULL) {
    fs_vnode_unref (ndp1->vp);
  }
  if (ndp2->dvp != NULL) {
    fs_vnode_unref (ndp2->dvp);
  }
  if (ndp2->vp != NULL) {
    fs_vnode_unref (ndp2->vp);
  }

  if (result != 0) {
    efs_errno = -result;
    result = -1;
  }

  EFS_BAB_END (EFS_BAB_REMOUNT);
  fs_global_unlock ();

  return result;
}

int
efs_symlink (const char *oldpath, const char *newpath)
{
  struct nameidata *ndp = &nameidata_buffer;
  int result;

  /* Check for the file oldpath and newpath name validity. */
  result = fs_is_path_valid (oldpath);
  if (result != 0)
  {
    efs_errno = EINVAL;
    return -1;
  }
  result = fs_is_path_valid (newpath);
  if (result != 0)
  {
    efs_errno = EINVAL;
    return -1;
  }

  fs_global_lock ();
  EFS_BAB_START (EFS_BAB_SYMLINK);
  EFS_BAB_PRINT (EFS_BAB_SYMLINK, oldpath);
  EFS_BAB_PRINT (EFS_BAB_SYMLINK, newpath);

  result = fs_namei (ndp, newpath, O_EXCL | O_CREAT | O_NOFOLLOW);
  if (result < 0) {
    goto error_case;
  }

  result = ndp->dvp->vops->symlink (ndp, oldpath);

error_case:
  if (ndp->dvp != NULL)
    fs_vnode_unref (ndp->dvp);

  EFS_BAB_END (EFS_BAB_SYMLINK);
  fs_global_unlock ();

  if (result < 0) {
    efs_errno = -result;
    return -1;
  } else
    return 0;
}

int
efs_readlink (const char *path, char *buf, fs_size_t bufsiz)
{
  struct nameidata *ndp = &nameidata_buffer;
  int result;

  /* Check for the filepath validity. */
  result = fs_is_path_valid (path);
  if ( (result != 0) || (buf == NULL) || (bufsiz == 0) )
  {
    efs_errno = EINVAL;
    return -1;
  }

  fs_global_lock ();
  EFS_BAB_START (EFS_BAB_READLINK);
  EFS_BAB_PRINT (EFS_BAB_READLINK, path);

  result = fs_namei (ndp, path, O_NOFOLLOW);
  if (result == 0) {
    result = ndp->vp->vops->readlink (ndp->vp, buf, bufsiz);
  }

  if (ndp->dvp != NULL)
    fs_vnode_unref (ndp->dvp);
  if (ndp->vp != NULL)
    fs_vnode_unref (ndp->vp);

  EFS_BAB_END (EFS_BAB_READLINK);
  fs_global_unlock ();

  if (result < 0) {
    efs_errno = -result;
    return -1;
  } else
    return result;
}

int
efs_access (const char *path, int amode)
{
  int result;

  (void) amode;

  /* Check for the filepath validity. */
  result = fs_is_path_valid (path);
  if (result != 0)
  {
    efs_errno = EINVAL;
    return -1;
  }

  /* efs_access is not implemented in EFS2.  At this point, there is no
   * intent on doing so.  It is part of a security model not well suited to
   * mobile devices. */
  EFS_BAB_API (EFS_BAB_ACCESS);
  efs_errno = EINVAL;
  return -1;
}

int
efs_mknod (const char *path, int mode, fs_devspecial_t dev)
{
  struct nameidata *ndp = &nameidata_buffer;
  int result = 0;

  /* Check for the path validity. */
  result = fs_is_path_valid (path);
  if (result != 0)
  {
    efs_errno = EINVAL;
    return -1;
  }

  fs_global_lock();
  EFS_BAB_START (EFS_BAB_MKNOD);
  EFS_BAB_PRINT (EFS_BAB_MKNOD, path);

  result = fs_namei (ndp, path, O_EXCL | O_CREAT);

  /* If the name lookup succeeds, the file already exists */
  if (ndp->vp != NULL)
    result = -EEXIST;

  /* Just attempt to create the device file (XXX ndp->dvp safe???)*/
  if (result == 0)
    result = ndp->dvp->vops->mknod (ndp, mode, dev);

  /* Done.  */
  if (ndp->dvp)
    fs_vnode_unref (ndp->dvp);

  if (ndp->vp)
    fs_vnode_unref (ndp->vp);

  EFS_BAB_END (EFS_BAB_MKNOD);
  fs_global_unlock ();

  if (result != 0) {
    efs_errno = -result;
    result = -1;
  }

  return result;
}

int
efs_mkfifo (const char *path, int mode)
{
  int result;

  /* Check for the path validity. */
  result = fs_is_path_valid (path);
  if (result != 0)
  {
    efs_errno = EINVAL;
    return -1;
  }

  return efs_mknod (path, (mode & ~S_IFMT) | S_IFIFO, 0);
}


/**********************************************************************
 * FUNCTION      efs_pure_put
 *
 * Store a value in a special item file.
 */
int
efs_pure_put (const char *path, void *data, fs_size_t length,
                      int oflag, int mode)
{
  struct nameidata *ndp = &nameidata_buffer;
  int result = 0;

  /* Check for the filepath validity. */
  result = fs_is_path_valid (path);
  if (result != 0 || (data == NULL && length != 0))
  {
    efs_errno = EINVAL;
    return -1;
  }

  fs_global_lock ();
  EFS_BAB_START (EFS_BAB_PURE_PUT);
  EFS_BAB_PRINT (EFS_BAB_PURE_PUT, path);

  if (result == 0)
    result = fs_namei (ndp, path, oflag);

  if (result == 0 && ndp->vp) {
    /* Verify that if we did recover a 'node' that it is really an item
     * type. */
    if (!S_ISITM (ndp->vp->mode))
      result = -EEXIST;
  }

  if (result == 0) {
    result = ndp->dvp->vops->put_item (ndp, data, length, mode);
  }

  if (ndp->dvp) {
    fs_vnode_unref (ndp->dvp);
  }
  if (ndp->vp)
    fs_vnode_unref (ndp->vp);

  if (result != 0) {
    efs_errno = -result;
    result = -1;
  }

  EFS_BAB_END (EFS_BAB_PURE_PUT);
  fs_api_debug_add (FS_API_DEBUG_ID_EFS_PUT, -1, path);

  fs_global_unlock ();

  return result;
}


/* Read contents of item. */
int
efs_pure_get (const char *path, void *data, fs_size_t length)
{
  struct nameidata *ndp = &nameidata_buffer;
  int result;
  struct fs_vnode *vp;

  /* Check for the filepath validity. */
  result = fs_is_path_valid (path);
  if (result != 0 || (data == NULL && length != 0))
  {
    efs_errno = EINVAL;
    return -1;
  }

  fs_global_lock ();
  EFS_BAB_START (EFS_BAB_PURE_GET);
  EFS_BAB_PRINT (EFS_BAB_PURE_GET, path);

  result = fs_namei (ndp, path, 0);

  if (result == 0) {
    vp = ndp->vp;
    result = vp->vops->get_item (vp, data, length);
  }

  if (ndp->dvp != NULL)
    fs_vnode_unref (ndp->dvp);
  if (ndp->vp != NULL)
    fs_vnode_unref (ndp->vp);

  EFS_BAB_END (EFS_BAB_PURE_GET);

  fs_api_debug_add (FS_API_DEBUG_ID_EFS_GET, -1, path);

  fs_global_unlock ();

  if (result < 0) {
    efs_errno = -result;
    return -1;
  } else
    return result;
}

void
efs_wait_for_xact (void)
{
  EFS_BAB_API (EFS_BAB_WAIT_FOR_XACT);
  fs_global_lock();
} /* efs_wait_for_xact */

/**********************************************************************
 * FUNCTION      efs_reset
 *
 * DESCRIPTION
 *             Resets the filesystem and stops the system. The mobile must be
 *             rebooted before it can be used again.
 ************************************************************************/
void
efs_reset (void)
{
  EFS_BAB_API (EFS_BAB_RESET);

  /* We grab the lock to ensure no other efs operations */
#ifndef FEATURE_EFS_BOOT_ENV
  fs_sync_lock ();
#endif
  fs_global_lock ();

  efs_reset_nostop ();

  FS_ERR_FATAL ("filesystem reset. reboot needed", 0, 0, 0);
} /* efs_reset */

/***********************************************************************
 * FUNCTION    efs_reset_nostop (void)
 *
 * DESCRIPTION
 *   Same as efs_reset, except that it returns.  Should only be used from
 *   within the FS_ERR_FATAL handler.
 **********************************************************************/
void
efs_reset_nostop (void)
{
  EFS_BAB_API (EFS_BAB_RESET_NOSTOP);

  /* Invalidate all superblocks and halt the system. This will force a
   * fresh start on the next reboot.
   *
   * Make sure we prevent all filesystem operations until we reboot, so we
   * don't trip on the inconsistent state while the erase is running.
   */
#ifndef FEATURE_EFS_BOOT_ENV
  fs_sync_lock ();
#endif
  fs_global_lock();

#ifdef FEATURE_EFS_QEFS2
  #if defined (FEATURE_EFS_EFS2_ON_NAND)      ||   \
      defined (FEATURE_EFS_EFS2_ON_NOR)       ||   \
      defined (FEATURE_EFS_EFS2_ON_SPI_NOR)
  {
    fs_pm_super_invalidate_superblocks ();
  }
  #elif defined (FEATURE_EFS_EFS2_ON_MNAND)
  {
    fs_mnand_pm_erase_efs ();
  }
 #elif defined(FEATURE_EFS_ROMFS)
  /* Do nothing, booting from romfs... */
  #elif defined (FEATURE_EFS_EFS2_ON_RMTS)
  {
    fs_rmts_pm_erase_efs ();
  }
  #elif defined (FEATURE_EFS_EFS2_ON_RAMFS)
  {
    fs_ramfs_pm_erase_efs ();
  }
  #else
    #error "Provide reset function"
  #endif
#endif

  fs_global_unlock ();
#ifndef FEATURE_EFS_BOOT_ENV
  fs_sync_unlock ();
#endif
}

int
efs_get_fs_data (const char *path,
                 struct fs_factimage_read_info *image_info_ptr,
                 byte *page_buffer)
{
  int result;
  struct nameidata *ndp = &nameidata_buffer;

  /* Check for the filepath validity. */
  result = fs_is_path_valid (path);
  if ( (result != 0) || (page_buffer == NULL) )
  {
    efs_errno = EINVAL;
    return -1;
  }

  if (image_info_ptr == NULL)
  {
    efs_errno = EINVAL;
    return -1;
  }

  if (image_info_ptr->stream_state < 0)
  {
    efs_errno = EINVAL;
    return -1;
  }

  EFS_BAB_API (EFS_BAB_GET_FS_DATA);

  fs_global_lock ();

  /* Check to see if the path is valid */
  result = fs_namei (ndp, path, 0);

  if (result == 0)
    result = ndp->vp->vops->get_fs_data (ndp->vp, image_info_ptr, page_buffer);

  if (ndp->dvp != NULL)
    fs_vnode_unref(ndp->dvp);

  if (ndp->vp != NULL)
    fs_vnode_unref(ndp->vp);

  fs_global_unlock ();

  if (result < 0)
  {
    efs_errno = -result;
    return -1;
  }
  else
    return 0;

} /* end of efs_get_fs_data */

int
efs_get_device_info (const char *path, struct fs_device_attr *dev_attr)
{
  int result;
  struct nameidata *ndp = &nameidata_buffer;

  /* Check for the filepath validity. */
  result = fs_is_path_valid (path);
  if ( (result != 0) || (dev_attr == NULL) )
  {
    efs_errno = EINVAL;
    return -1;
  }

  fs_global_lock ();
  EFS_BAB_START (EFS_BAB_GET_DEVICE_INFO);

  /* Check to see if the path is valid */
  result = fs_namei (ndp, path, O_RDONLY);

  if (result == 0)
    result = ndp->vp->vops->get_device_attr (ndp->vp, dev_attr);

  if (ndp->dvp != NULL)
    fs_vnode_unref(ndp->dvp);

  if (ndp->vp != NULL)
    fs_vnode_unref(ndp->vp);

  EFS_BAB_END (EFS_BAB_GET_DEVICE_INFO);
  fs_global_unlock ();

  if (result < 0)
  {
    efs_errno = -result;
    return -1;
  }
  else
    return 0;

} /* efs_get_device_info */


/***********************************************************************
 * FUNCTION
 *   efs_image_prepare
 *
 * DESCRIPTION
 *   Make progress toward preparing this image for a factory dump.  This
 *   does a small amount of work, of whatever kind is needed to prepare to
 *   make an exportable image of this filesystem.  This function should be
 *   called repeatedly until it returns that there is no work left to do.
 *
 * RETURN VALUE
 *   Returns a count indicating any additional work.  This number should
 *   only be compared against zero.  Positive indicates that there is work
 *   to do, and negative indicates an error has occured.
 ***********************************************************************/
int
efs_image_prepare (const char *path)
{
  int result;
  struct nameidata *ndp = &nameidata_buffer;

  /* Check for the path validity. */
  result = fs_is_path_valid (path);
  if (result != 0)
  {
    efs_errno = EINVAL;
    return -1;
  }

  fs_global_lock ();
  EFS_BAB_START (EFS_BAB_IMAGE_PREPARE);

  result = fs_namei (ndp, path, 0);

  if (result == 0)
    result = ndp->vp->vops->image_prepare(ndp->vp);

  if (ndp->dvp != NULL)
    fs_vnode_unref(ndp->dvp);

  if (ndp->vp != NULL)
    fs_vnode_unref(ndp->vp);

  EFS_BAB_END (EFS_BAB_IMAGE_PREPARE);
  fs_global_unlock ();

  if (result < 0)
  {
    efs_errno = -result;
    return -1;
  }
  else
    return result;
}


/*************************************************************************
 *
 * efs_chown
 *
 * change the owner and/or group id
 *
 */

int
efs_chown (const char *path, int uid_val, int gid_val)
{

  int result;
  struct nameidata *ndp = &nameidata_buffer;

  /* Check for the path validity. */
  result = fs_is_path_valid (path);
  if (result != 0)
  {
    efs_errno = EINVAL;
    return -1;
  }

  fs_global_lock ();
  EFS_BAB_START (EFS_BAB_CHOWN);
  /* Check to see if the path is valid */
  result = fs_namei (ndp, path, 0);

  if (result != 0)
    goto cleanup;

  /* Everything checks out so let us change the group id */

  result = ndp->vp->vops->chown(ndp->vp, uid_val, gid_val);

cleanup:
  if (ndp->dvp != NULL)
    fs_vnode_unref(ndp->dvp);
  if (ndp->vp != NULL)
    fs_vnode_unref(ndp->vp);
  EFS_BAB_END (EFS_BAB_CHOWN);
  fs_global_unlock ();

  if (result < 0)
  {
    efs_errno = -result;
    return -1;
  }
  else
    return 0;
}

/*************************************************************************
 *
 * efs_set_reservation
 *
 * sets the amount of space for a reservation
 *
 */

int
efs_set_reservation(const char *path, uint32 groupid, uint32 size)
{
  int result;
  struct nameidata *ndp = &nameidata_buffer;

  /* Check for the path validity. */
  result = fs_is_path_valid (path);
  if (result != 0)
  {
    efs_errno = EINVAL;
    return -1;
  }

  fs_global_lock ();
  EFS_BAB_START (EFS_BAB_SET_RESERVATION);

  result = fs_namei (ndp, path, 0);

  if (result != 0)
    goto cleanup;

  /* Everything checks out so let us set the reservation
   */

  result = ndp->vp->vops->set_reservation(ndp->vp, groupid, size);

cleanup:
  if (ndp->dvp != NULL)
    fs_vnode_unref(ndp->dvp);
  if (ndp->vp != NULL)
    fs_vnode_unref(ndp->vp);
  EFS_BAB_END (EFS_BAB_SET_RESERVATION);
  fs_global_unlock ();

  if (result < 0)
  {
    efs_errno = -result;
    return -1;
  }
  else
    return 0;

}

/*************************************************************************
 *
 * efs_set_quota
 *
 * sets the quota limit
 *
 */

int
efs_set_quota(const char *path, uint32 groupid, uint32 size)
{
  int result;
  struct nameidata *ndp = &nameidata_buffer;

  /* Check for the path validity. */
  result = fs_is_path_valid (path);
  if (result != 0)
  {
    efs_errno = EINVAL;
    return -1;
  }

  fs_global_lock ();
  EFS_BAB_START (EFS_BAB_SET_QUOTA);

  /* We need a buffer to pass in updated values to the superblock so just
   * get a buffer for where we are
   */
  result = fs_namei (ndp, path, 0);

  if (result != 0)
    goto cleanup;

  /* Everything checks out so let us set the quota
   */

  result = ndp->vp->vops->set_quota(ndp->vp, groupid, size);

cleanup:
  if (ndp->dvp != NULL)
    fs_vnode_unref(ndp->dvp);
  if (ndp->vp != NULL)
    fs_vnode_unref(ndp->vp);
  EFS_BAB_END (EFS_BAB_SET_QUOTA);
  fs_global_unlock ();

  if (result < 0)
  {
    efs_errno = -result;
    return -1;
  }
  else
    return 0;

}
/*************************************************************************
 *
 * efs_get_group_info
 *
 * gets the information for quotas and reservations for a specific group
 * id
 *
 */

int
efs_get_group_info(const char *path, uint32 groupid,
    struct fs_group_info * ginfo)
{
  int result;
  struct nameidata *ndp = &nameidata_buffer;

  /* Check for the path validity. */
  result = fs_is_path_valid (path);
  if ( (result != 0) || (ginfo == NULL) )
  {
    efs_errno = EINVAL;
    return -1;
  }

  fs_global_lock ();
  EFS_BAB_START (EFS_BAB_GET_GROUP_INFO);

  result = fs_namei (ndp, path, 0);

  if (result != 0)
    goto cleanup;

  /* Everything checks out so let us get the reservation
   */

  result = ndp->vp->vops->get_group_info(ndp->vp, groupid, ginfo);

cleanup:
  if (ndp->dvp != NULL)
    fs_vnode_unref(ndp->dvp);
  if (ndp->vp != NULL)
    fs_vnode_unref(ndp->vp);
  EFS_BAB_END (EFS_BAB_GET_GROUP_INFO);
  fs_global_unlock ();

  if (result < 0)
  {
    efs_errno = -result;
    return -1;
  }
  else
    return 0;

}

/*************************************************************************
 *
 * FUNCTION      efs_shred
 *
 * DESCRIPTION   Deletes the old invalid (obsolete) data from the device.
 *
 * PARAMETER     The parameter path is used to determine which filesystem
 *               need to be shredded.
 *
 * RETURN VALUE
 *        0   :  if all the obsolete data, as of when the first time shred
 *               is called, is deleted from the device.
 *        1   :  if the shred is incomplete, another efs_shred call should
 *               be issued until 0 is returned.
 *        -1  :  if the shred is not possible.
 * *
 */
int
efs_shred (const char *path)
{
  struct nameidata *ndp = &nameidata_buffer;
  struct fs_vnode *vp;
  int result = -1;

  /* Check for the path validity. */
  result = fs_is_path_valid (path);
  if (result != 0)
  {
    efs_errno = EINVAL;
    return -1;
  }

  fs_global_lock ();
  EFS_BAB_START (EFS_BAB_SHRED);
  EFS_BAB_PRINT (EFS_BAB_SHRED, path);

  result = fs_namei (ndp, path, 0);

  if (result != 0)
    goto error_case;

  vp = ndp->vp;

  result = vp->vops->shred (vp);

error_case:
  if (ndp->dvp != NULL)
    fs_vnode_unref (ndp->dvp);
  if (ndp->vp != NULL)
    fs_vnode_unref (ndp->vp);

  if (result < 0)
    EFS_BAB_RETURN(EFS_BAB_SHRED, -result);

  EFS_BAB_END (EFS_BAB_SHRED);
  fs_global_unlock ();

  if (result < 0)
  {
    efs_errno = -result;
    return -1;
  }
  return result;
}

/*************************************************************************
 *
 * FUNCTION      efs_scrub
 *
 * DESCRIPTION   Forces all data in flash to be moved to new erased blocks.
 *
 * PARAMETER     The parameter path is used to determine which filesystem
 *               need to be scrubbed.
 *
 * RETURN VALUE
 *        0   :  All blocks, as of when the first time scrub was called, have
 *               been moved to a new block.
 *        1   :  Scrub is incomplete, another efs_scrub call should be issued
 *               until 0 is returned.
 *       -1   :  Scrub is not possible for reasons specified in efs_errno.
 */
int
efs_scrub (const char *path)
{
  struct nameidata *ndp = &nameidata_buffer;
  struct fs_vnode *vp;
  int result = -1;

  /* Check for the path validity. */
  result = fs_is_path_valid (path);
  if (result != 0)
  {
    efs_errno = EINVAL;
    return -1;
  }

  fs_global_lock ();
  EFS_BAB_START (EFS_BAB_SCRUB);
  EFS_BAB_PRINT (EFS_BAB_SCRUB, path);

  result = fs_namei (ndp, path, 0);

  if (result != 0)
    goto error_case;

  vp = ndp->vp;

  result = vp->vops->scrub (vp);

error_case:
  if (ndp->dvp != NULL)
    fs_vnode_unref (ndp->dvp);
  if (ndp->vp != NULL)
    fs_vnode_unref (ndp->vp);

  if (result < 0)
    EFS_BAB_RETURN(EFS_BAB_SCRUB, -result);

  EFS_BAB_END (EFS_BAB_SCRUB);
  fs_global_unlock ();

  if (result < 0)
  {
    efs_errno = -result;
    return -1;
  }

  return result;
}

int
efs_full_scrub (const char *path)
{
  int result;

  do
  {
    result = efs_scrub (path);
  } while (result == 1);

  return result;
}


int
efs_utime (const char *path, const struct fs_utimbuf *times)
{
  int result;
  struct nameidata *ndp = &nameidata_buffer;

  /* Check for the path validity. */
  result = fs_is_path_valid (path);
  if (result != 0)
  {
    efs_errno = EINVAL;
    return -1;
  }

  fs_global_lock ();
  EFS_BAB_START (EFS_BAB_UTIME);

  /* Check to see if the path is valid */
  result = fs_namei (ndp, path, 0);

  /* If file exists, change the time on this vp */
  if (result == 0)
    result = ndp->vp->vops->utime (ndp->vp, times);

  if (ndp->dvp != NULL)
    fs_vnode_unref (ndp->dvp);
  if (ndp->vp != NULL)
    fs_vnode_unref (ndp->vp);

  if (result < 0)
    EFS_BAB_RETURN (EFS_BAB_UTIME, -result);

  EFS_BAB_END (EFS_BAB_UTIME);
  fs_global_unlock ();

  if (result < 0)
  {
    efs_errno = -result;
    return -1;
  }
  else
    return 0;
} /* END efs_utime */


int
efs_utime_item (const char *path, const struct fs_utimbuf *times)
{
  int result;
  struct nameidata *ndp = &nameidata_buffer;

  /* Check for the path validity. */
  result = fs_is_path_valid (path);
  if (result != 0)
  {
    efs_errno = EINVAL;
    return -1;
  }

  fs_global_lock ();

  /* Check to see if the path is valid */
  result = fs_namei (ndp, path, 0);

  /* If file exists, change the time on this vp */
  if (result == 0)
    result = ndp->vp->vops->utime_item (ndp->vp, times);

  if (ndp->dvp != NULL)
    fs_vnode_unref (ndp->dvp);
  if (ndp->vp != NULL)
    fs_vnode_unref (ndp->vp);

  if (result < 0)
    EFS_BAB_RETURN (EFS_BAB_UTIME, -result);

  fs_global_unlock ();

  if (result < 0)
  {
    efs_errno = -result;
    return -1;
  }
  else
    return 0;
} /* END efs_utime_iternal */


#ifndef FEATURE_EFS_BOOT_ENV

int
efs_sync_helper (const char *path, int do_force_sync, int is_shutdown_sync)
{
  int result;
  struct nameidata *ndp = &nameidata_buffer;
  struct fs_vnode *vp = NULL;
  uint64 start_time, end_time;
  uint32 total_sync_time;
  struct fs_profile_handle profile_handle;

  /* Check for the path validity. */
  result = fs_is_path_valid (path);
  if (result != 0)
  {
    efs_errno = EINVAL;
    return -1;
  }

  /* Check if efs sync needs to blocked due to imminent shutdown*/
  if(fs_reject_efs_sync != 0)
  {
    FS_MSG_HIGH_0 ("EFS sync not permitted due to imminent shutdown");
    efs_errno = EPERM;
    return -1;
  }

  fs_sync_lock ();
  fs_global_lock ();

  /* Flush the pending logs to disk. This will dirty the file system. */
  fs_profile_flush_logs ();

  EFS_BAB_START (EFS_BAB_SYNC);

  start_time = fs_timetick_get_timetick ();
  fs_profile_open (&profile_handle, FS_PROFILE_ID_SYNC);
  fs_profile_start (&profile_handle);

  /* Check to see if the path is valid */
  result = fs_namei (ndp, path, 0);

  /* Unref the parent directory, we don't need it */
  if (ndp->dvp != NULL)
    fs_vnode_unref (ndp->dvp);

  /* Save the vnode pointer and free the global nameidata buffer */
  vp = ndp->vp;
  ndp->vp = NULL;

  /* Start the sync opertation. Returns 0 when we need to wait for sync
   * positive when sync not needed, negative on error. */
  if (result == 0)
    result = vp->vops->sync_start (vp, do_force_sync, is_shutdown_sync);

  if (result == 0)
  {
    /* Give up the global-lock, we still have the sync lock. */
    fs_global_unlock ();
    result = vp->vops->sync_wait (vp);
    fs_global_lock ();

    /* Do sync cleanup. The result of sync_wait should be stored by lower
     * layer, safe to discard. */
    result = vp->vops->sync_end (vp);
  }

  if (vp != NULL)
    fs_vnode_unref (vp);

  if (result < 0)
    EFS_BAB_RETURN (EFS_BAB_SYNC, -result);

  EFS_BAB_END (EFS_BAB_SYNC);

  end_time = fs_timetick_get_timetick ();
  total_sync_time =
    (uint32)(fs_timetick_timetick_diff_to_us (end_time - start_time));
  FS_MSG_MED_2 ("%d,%d, EFS: EFS-Sync complete (time_us, result)",
                total_sync_time, result);

  /* Delay profiling stop the end so we are inside the global lock again.
   * The logged integers give us the required timing data.   */
  {
    uint32 encryption_time, write_iovec_time, compression_time;
    int res;

    fs_profile_stop (&profile_handle);

    /* If result is positive, then there was nothing to sync. */
    if (result > 0)
    {
      encryption_time = 0;
      write_iovec_time = 0;
      compression_time = 0;
      res = 0;
    }
    else
    {
      struct fs_timings_type* timings;
      timings = fs_timings_get_instance ();
      compression_time = timings->efs_sync_timings.compression_time;
      encryption_time = timings->efs_sync_timings.encryption_time;
      write_iovec_time = timings->efs_sync_timings.write_iovec_time;
      res = -result;
    }

    fs_profile_log_int (&profile_handle, total_sync_time);
    fs_profile_log_int (&profile_handle, compression_time);
    fs_profile_log_int (&profile_handle, encryption_time);
    fs_profile_log_int (&profile_handle, write_iovec_time);
    fs_profile_log_int (&profile_handle, res);
    fs_profile_log_string (&profile_handle, path);
    fs_profile_close (&profile_handle);
  }

  fs_api_debug_add (FS_API_DEBUG_ID_EFS_SYNC, 0, path);
  fs_global_unlock ();
  fs_sync_unlock ();

  if (result < 0)
  {
    efs_errno = -result;
    return -1;
  }

  return 0;
}

int
efs_sync (const char *path)
{
  return efs_sync_helper (path, 0, 0);
}

int
efs_sync_no_wait (const char *path, fs_sync_token_t *sync_token)
{
  int result;
  struct nameidata *ndp = &nameidata_buffer;
  struct fs_profile_handle profile_handle;

  /* Check for the path validity and Null check for sync_token. */
  result = fs_is_path_valid (path);
  if ( (result != 0) || (sync_token == NULL) )
  {
    efs_errno = EINVAL;
    return -1;
  }

  fs_global_lock ();
  /* Flush the pending logs to disk. This will dirty the file system. */
  fs_profile_flush_logs ();

  fs_profile_open (&profile_handle, FS_PROFILE_ID_SYNC_NO_WAIT);
  fs_profile_start (&profile_handle);
  EFS_BAB_START (EFS_BAB_SYNC_NO_WAIT);

  /* Check to see if the path is valid */
  result = fs_namei (ndp, path, 0);

  /* Request an non blocking efs_sync */
  if (result == 0)
    result = ndp->vp->vops->sync_no_wait (ndp->vp, sync_token);

  if (ndp->dvp != NULL)
    fs_vnode_unref (ndp->dvp);
  if (ndp->vp != NULL)
    fs_vnode_unref (ndp->vp);

  if (result < 0)
    EFS_BAB_RETURN (EFS_BAB_SYNC_NO_WAIT, -result);

  EFS_BAB_END (EFS_BAB_SYNC_NO_WAIT);
  fs_profile_stop (&profile_handle);
  fs_profile_log_int (&profile_handle, -result);
  fs_profile_close (&profile_handle);
  fs_global_unlock();

  if (result < 0)
  {
    efs_errno = -result;
    return -1;
  }

  return 0;
}

int
efs_sync_get_status (const char *path, fs_sync_token_t sync_token,
                     enum fs_sync_result *sync_result)
{
  int result;
  struct nameidata *ndp = &nameidata_buffer;
  struct fs_vnode *vp = NULL;

  /* Check for the path validity and Null check for sync_result. */
  result = fs_is_path_valid (path);
  if ( (result != 0) || (sync_result == NULL) )
  {
    efs_errno = EINVAL;
    return -1;
  }

  fs_global_lock ();
  EFS_BAB_START (EFS_BAB_SYNC);

  /* Check to see if the path is valid */
  result = fs_namei (ndp, path, 0);

  if (result != 0)
    goto cleanup;

  vp = ndp->vp;

  /* Path is valid, get sync status */
  result = vp->vops->sync_get_status (vp, sync_token, sync_result);

cleanup:
  if (ndp->dvp != NULL)
    fs_vnode_unref (ndp->dvp);
  if (vp != NULL)
    fs_vnode_unref (vp);

  if (result !=  0)
    EFS_BAB_RETURN(EFS_BAB_SYNC, -result);

  EFS_BAB_END (EFS_BAB_SYNC);

  fs_global_unlock ();

  if (result != 0)
  {
    efs_errno = -result;
    return -1;
  }

  return 0;
}

/* For remote storage devices overwrites the golden copy */
int
efs_make_golden_copy (const char *path)
{
  int result;
  struct nameidata *ndp;

  /* Check for the path validity. */
  result = fs_is_path_valid (path);
  if (result != 0)
  {
    efs_errno = EINVAL;
    return -1;
  }

  fs_sync_lock ();
  fs_global_lock ();

  result = efs_sync (path);
  if (result != 0)
  {
    FS_ERR_FATAL ("%d, %d, Failed to sync during call to make_golden", result,
                  efs_errno, 0);
  }

  ndp = &nameidata_buffer;

  /* Check to see if the path is valid */
  result = fs_namei (ndp, path, 0);

  /* Try to make a golden copy of the current contents of the file system.
   * After this call the code should never return for Remote Storage device */
  if (result == 0)
    result = ndp->vp->vops->make_golden_copy (ndp->vp);

  if (ndp->dvp != NULL)
    fs_vnode_unref (ndp->dvp);
  if (ndp->vp != NULL)
    fs_vnode_unref (ndp->vp);

  fs_global_unlock ();
  fs_sync_unlock ();

  if (result < 0)
  {
    efs_errno = -result;
    return -1;
  }
  else
  {
    return 0;
  }
}


volatile int fs_shutdown_do_logging = 0;

#define FS_SHUTDOWN_LOGGING_CFG        "/efs_shutdown_log.cfg"
#define FS_SHUTDOWN_FROM_RCINIT_LOG    "/efs_shutdown_from_rcinit"
#define FS_SHUTDOWN_FROM_CM_LOG        "/efs_shutdown_from_cm"

typedef enum
{
  FS_SHUTDOWN_FROM_RCINIT,
  FS_SHUTDOWN_FROM_CM
} fs_shutdown_mechanism;

static void
efs_log_shutdown_mechanism (fs_shutdown_mechanism shutdown_hint)
{
  int result, shutdown_log_size;
  struct fs_stat stat_buff;
  char *log_file;
  uint32 shutdown_count;

  result = efs_stat (FS_SHUTDOWN_LOGGING_CFG, &stat_buff);
  if (fs_shutdown_do_logging == 1)
  {
    result = 0;
  }

  if (result != 0)
  {
    return;
  }

  switch (shutdown_hint)
  {
    case FS_SHUTDOWN_FROM_RCINIT:
    {
      log_file = FS_SHUTDOWN_FROM_RCINIT_LOG;
      break;
    }

    case FS_SHUTDOWN_FROM_CM:
    {
      log_file = FS_SHUTDOWN_FROM_CM_LOG;
      break;
    }

    default:
      return;
  }

  shutdown_log_size = sizeof (shutdown_count);
  result = efs_get (log_file, &shutdown_count, shutdown_log_size);
  if (result != shutdown_log_size)
  {
    shutdown_count = 1;
  }
  else
  {
    shutdown_count++;
  }

  (void) efs_put (log_file, &shutdown_count, shutdown_log_size, O_CREAT, 0666);
}


static int
efs_prepare_shutdown_core (fs_shutdown_mechanism shutdown_hint)
{
  int result = 0, error;
  uint32 force_sync_flag = 0, force_sync_flag_size;

  FS_MSG_HIGH_1 ("%d, EFS : efs_prepare_shutdown", shutdown_hint);

#ifdef FEATURE_FS_ASYNC_PUT
  (void) fs_async_put_prepare_shutdown ();
#endif

/* Make sure efs_sync is at the end, so any data being written gets synced. */
  efs_log_shutdown_mechanism (shutdown_hint);

/* Check if the sync needs to be forced before shutdown */
  force_sync_flag_size = sizeof (force_sync_flag);
  efs_errno = ENOERR;
  result = efs_get (FS_CONFIG_FILE, &force_sync_flag, force_sync_flag_size);
  error = efs_errno;
  FS_MSG_HIGH_3 ("[%d,%d,%d] force shutdown efs-sync file",
                 result, error, force_sync_flag);

  fs_reject_efs_sync = 0;
  result = efs_sync_helper ("/", (int)force_sync_flag, 1);

  return result;
}

int
efs_prepare_shutdown (void)
{
  int result = 0;

#ifndef FEATURE_EFS_APPS_CONTROLS_EFS_SYNC
  result = efs_prepare_shutdown_core (FS_SHUTDOWN_FROM_CM);
#endif

  return result;
}

void
efs_rcinit_shutdown_callback (void)
{
  FS_MSG_HIGH_0 ("Shutdown callback from RCINIT");
  (void) efs_prepare_shutdown_core (FS_SHUTDOWN_FROM_RCINIT);
}

void
efs_rcinit_group7_shutdown_callback (void)
{
  fs_reject_efs_sync = 1;
  FS_MSG_HIGH_0 ("Group 7 Shutdown callback from RCINIT");
}

#endif /* FEATURE_EFS_BOOT_ENV */

int
efs_get_volume_name (const char *path, char *label, int len)
{
  struct nameidata *ndp = &nameidata_buffer;
  int result;

  /* Check for the path validity, Null check for label and len field. */
  /* Bail for bad arguments */
  result = fs_is_path_valid (path);
  if ( (result != 0) || (label == NULL) || (len <= 0) )
  {
    efs_errno = EINVAL;
    return -1;
  }

  fs_global_lock ();
  EFS_BAB_START (EFS_BAB_GET_VOLUME_NAME);
  EFS_BAB_PRINT (EFS_BAB_GET_VOLUME_NAME, path);

  /* Check to see if the path is valid */
  result = fs_namei (ndp, path, 0);

  /* If path exists, attempt to get the volume name. */
  if (result == 0)
    result = ndp->vp->vops->get_volume_name (ndp->vp, label, len);

  if (ndp->dvp != NULL)
    fs_vnode_unref (ndp->dvp);
  if (ndp->vp != NULL)
    fs_vnode_unref (ndp->vp);

  if (result != 0)
    EFS_BAB_RETURN (EFS_BAB_GET_VOLUME_NAME, -result);

  EFS_BAB_END (EFS_BAB_GET_VOLUME_NAME);
  fs_global_unlock ();

  if (result < 0)
  {
    efs_errno = -result;
    return -1;
  }
  else
    return 0;
}

int
efs_set_volume_name (const char *path, char *label)
{
  struct nameidata *ndp = &nameidata_buffer;
  int result;

  /* Check for the path validity, Null check for label and len field. */
  /* Bail for bad arguments */
  result = fs_is_path_valid (path);
  if ( (result != 0) || (label == NULL) )
  {
    efs_errno = EINVAL;
    return -1;
  }

  fs_global_lock ();
  EFS_BAB_START (EFS_BAB_SET_VOLUME_NAME);
  EFS_BAB_PRINT (EFS_BAB_SET_VOLUME_NAME, path);

  /* Check to see if the path is valid */
  result = fs_namei (ndp, path, 0);

  /* If path exists, attempt to set the volume name */
  if (result == 0)
    result = ndp->vp->vops->set_volume_name (ndp->vp, label);

  if (ndp->dvp != NULL)
    fs_vnode_unref (ndp->dvp);
  if (ndp->vp != NULL)
    fs_vnode_unref (ndp->vp);

  if (result != 0)
    EFS_BAB_RETURN (EFS_BAB_SET_VOLUME_NAME, -result);

  EFS_BAB_END (EFS_BAB_SET_VOLUME_NAME);
  fs_global_unlock ();

  if (result < 0)
  {
    efs_errno = -result;
    return -1;
  }
  else
    return 0;
}

int
efs_lock_filesystem (const char *path)
{
  int result;
  struct nameidata *ndp = &nameidata_buffer;

  /* Check for the filepath validity. */
  result = fs_is_path_valid (path);
  if (result != 0)
  {
    efs_errno = EINVAL;
    return -1;
  }

  /* We allow lock/unlock only for the root-filesystem. */
  if ((path[0] != '/') || (path[1] != '\0'))
  {
    efs_errno = EPERM;
    return -1;
  }

  fs_global_lock ();

  EFS_BAB_START (EFS_BAB_LOCK_EFS);

  result = fs_namei (ndp, path, 0);

  if (result == 0)
  {
    result = ndp->vp->vops->lock_efs (ndp->vp);
  }

  if (ndp->dvp != NULL)
    fs_vnode_unref (ndp->dvp);
  if (ndp->vp != NULL)
    fs_vnode_unref (ndp->vp);

  if (result < 0)
  {
    EFS_BAB_RETURN (EFS_BAB_LOCK_EFS, -result);
    efs_errno = -result;
    fs_global_unlock ();
    result = -1;
  }

  EFS_BAB_END (EFS_BAB_LOCK_EFS);

  return result;
}

int
efs_unlock_filesystem (const char *path)
{
  int result;

  /* Check for the filepath validity. */
  result = fs_is_path_valid (path);
  if (result != 0)
  {
    efs_errno = EINVAL;
    return -1;
  }

  /* We allow lock/unlock only for the root-filesystem. */
  if ((path[0] != '/') || (path[1] != '\0'))
  {
    efs_errno = EPERM;
    return -1;
  }

  EFS_BAB_START (EFS_BAB_UNLOCK_EFS);

  if (result < 0)
  {
    EFS_BAB_RETURN (EFS_BAB_UNLOCK_EFS, -result);
    efs_errno = -result;
    result = -1;
  }
  else
  {
    fs_global_unlock ();
    result = 0;
  }

  EFS_BAB_END (EFS_BAB_UNLOCK_EFS);
  return result;
}

#ifndef FEATURE_EFS_BOOT_ENV

int
efs_private_borrow_ram (void **ram_addr, uint32 *ram_size)
{
  struct nameidata *ndp = &nameidata_buffer;
  int result = -1;

  *ram_addr = NULL;

  fs_global_lock ();

  result = fs_namei (ndp, "/", 0);
  if ((result != 0) || (ndp == NULL) || (ndp->vp == NULL))
  {
    FS_ERR_FATAL ("[%d] Cannot find root FS", efs_errno, 0, 0);
  }

  result = ndp->vp->vops->borrow_ram (ndp->vp, ram_addr, ram_size);

  if (ndp->dvp != NULL)
    fs_vnode_unref (ndp->dvp);

  if (ndp->vp != NULL)
    fs_vnode_unref (ndp->vp);

  fs_global_unlock ();

  if (result != 0)
  {
    efs_errno = result;
    result = -1;
  }
  return result;
}

static int
efs_get_buf_for_fsg (const char *path, uint8 **buf, uint32 *buf_size,
                     uint32 *hdr_offset)
{
  struct nameidata *ndp = &nameidata_buffer;
  int result = -1;

  *buf = NULL;
  *buf_size = 0;
  *hdr_offset = 0;

  fs_global_lock ();

  result = fs_namei (ndp, path, 0);
  if ((result != 0) || (ndp == NULL) || (ndp->vp == NULL))
  {
    FS_ERR_FATAL ("[%d] Cannot find root FS", efs_errno, 0, 0);
  }

  result = ndp->vp->vops->get_buf_for_fsg (ndp->vp, buf, buf_size, hdr_offset);

  if (ndp->dvp != NULL)
    fs_vnode_unref (ndp->dvp);

  if (ndp->vp != NULL)
    fs_vnode_unref (ndp->vp);

  fs_global_unlock ();

  if (result != 0)
  {
    efs_errno = result;
    result = -1;
  }
  return result;
}


int
efs_filesystem_image_open (const char *path,
  enum fs_filesystem_image_type image_type,
  int allow_device_specific_files, int allow_secure_files)
{
  int result;

  /* Check for the filepath validity. */
  result = fs_is_path_valid (path);
  if (result != 0)
  {
    efs_errno = EINVAL;
    return -1;
  }

  fs_sync_lock ();
  FS_PATH_BUFF_LOCK();
  fs_global_lock ();
  fs_profile_disable ();

  if (fs_img_handle.in_use != 0)
  {
    efs_errno = EMFILE;
    goto image_open_error;
  }

  fs_img_handle.handle = NULL;
  fs_img_handle.allow_device_specific_files = allow_device_specific_files;
  fs_img_handle.allow_secure_files = allow_secure_files;

  switch (image_type)
  {
    case FS_FILESYSTEM_IMAGE_TYPE_TAR:
    {
#ifdef FEATURE_FS_TAR
      fs_img_handle.handle = fs_tar_create_open (path,
        allow_device_specific_files, allow_secure_files);
#else
      efs_errno = EPERM;
#endif
      if (fs_img_handle.handle == NULL)
        goto image_open_error;
    }
    break;

    case FS_FILESYSTEM_IMAGE_TYPE_ZIP_TAR:
    {
#ifdef FEATURE_FS_ZIP_TAR
      fs_img_handle.handle = fs_zip_tar_open (path,
        allow_device_specific_files, allow_secure_files);
#else
      efs_errno = EPERM;
#endif /* FEATURE_FS_ZIP_TAR */
      if (fs_img_handle.handle == NULL)
        goto image_open_error;
    }
    break;

    default:
      efs_errno = EPERM;
      goto image_open_error;
  }
  fs_img_handle.image_type = image_type;
  fs_img_handle.in_use = 1;
  return 0;

image_open_error:
  /* Release the file system locks in the correct order. */
  fs_profile_enable ();
  fs_global_unlock ();
  FS_PATH_BUFF_UNLOCK();
  fs_sync_unlock ();
  return -1;
}

int
efs_filesystem_image_read (int image_handle, uint8 *buff, uint32 buff_size)
{
  int result = -1;
  int status = -1;
  uint64 start_time, end_time, diff_time;

  /* Image handle must be zero, since that is the only value returned by
   * open.
   */
  if ((image_handle != 0) || (buff == NULL) || (buff_size == 0))
  {
    efs_errno = EINVAL;
    return -1;
  }

  /* To make sure this task has the locks, grab the file system locks in the
   * correct order. */
  fs_sync_lock ();
  FS_PATH_BUFF_LOCK();
  fs_global_lock ();

  if (fs_img_handle.in_use == 0)
  {
    efs_errno = EINVAL;
    goto image_read_error;
  }

  start_time = fs_timetick_get_timetick ();
  switch (fs_img_handle.image_type)
  {
    case FS_FILESYSTEM_IMAGE_TYPE_TAR:
#ifdef FEATURE_FS_TAR
      result = fs_tar_create_read (fs_img_handle.handle, buff, buff_size);
      status = 0;
#endif
      break;

    case FS_FILESYSTEM_IMAGE_TYPE_ZIP_TAR:
#ifdef FEATURE_FS_ZIP_TAR
      result = fs_zip_tar_create_read (fs_img_handle.handle, buff, buff_size);
      status = 0;
#endif
      break;

    default:
      efs_errno = EPERM;
      status = 0;
      break;
  }

  if (status != 0)
  {
    efs_errno = EPERM;
  }

  end_time = fs_timetick_get_timetick ();
  diff_time = fs_timetick_timetick_diff_to_us (end_time - start_time);
  FS_MSG_HIGH_2 ("%d, %d EFS Zip Tar creation [time_us, result]",
                 (uint32)diff_time, result);

image_read_error:
  /* Release the file system locks in the correct order. */
  fs_global_unlock ();
  FS_PATH_BUFF_UNLOCK();
  fs_sync_unlock ();
  return result;
}

int
efs_filesystem_image_close (int image_handle)
{
  int result = -1;
  int status = -1;

  if (image_handle != 0)
  {
    efs_errno = EINVAL;
    return -1;
  }

  /* To make sure this task has the locks, grab the file system locks in the
   * correct order. */
  fs_sync_lock ();
  FS_PATH_BUFF_LOCK();
  fs_global_lock ();

  if (fs_img_handle.in_use == 0)
  {
    efs_errno = EINVAL;
    goto img_close_cleanup;
  }

  switch (fs_img_handle.image_type)
  {
    case FS_FILESYSTEM_IMAGE_TYPE_TAR:
#ifdef FEATURE_FS_TAR
      result = fs_tar_create_close (fs_img_handle.handle);
      status = 0;
#endif
      break;

    case FS_FILESYSTEM_IMAGE_TYPE_ZIP_TAR:
#ifdef FEATURE_FS_ZIP_TAR
      result = fs_zip_tar_close (fs_img_handle.handle);
      status = 0;
#endif
      break;

    default:
      efs_errno = EPERM;
      status = 0;
      goto img_close_cleanup;
  }

  if (status != 0)
  {
    efs_errno = EPERM;
    goto img_close_cleanup;
  }

  fs_img_handle.in_use = 0;
  fs_img_handle.handle = NULL;

  /* Release the file system locks in the correct order twice, once for this
   * call the other for the open call. */
  fs_global_unlock ();
  FS_PATH_BUFF_UNLOCK();
  fs_sync_unlock ();

img_close_cleanup:
  fs_profile_enable ();
  fs_global_unlock ();
  FS_PATH_BUFF_UNLOCK();
  fs_sync_unlock ();
  return result;
}

int
efs_store_data_in_superblock (const char *path, const void *data,
                              uint32 data_size)
{
  int result;
  struct nameidata *ndp = &nameidata_buffer;

  /* Check for the path validity and validate input arguments. */
  result = fs_is_path_valid (path);
  if ( (result != 0) || (data == NULL) || (data_size == 0))
  {
    efs_errno = EINVAL;
    return -1;
  }

  fs_global_lock ();
  EFS_BAB_START (EFS_BAB_STORE_DATA_IN_SB);

  /* Check to see if the path is valid */
  result = fs_namei (ndp, path, 0);

  if (result != 0)
    goto cleanup;

  result = ndp->vp->vops->store_data_in_sb (ndp->vp, data, data_size);

cleanup:
  if (ndp->dvp != NULL)
    fs_vnode_unref (ndp->dvp);
  if (ndp->vp != NULL)
    fs_vnode_unref (ndp->vp);

  if (result !=  0)
    EFS_BAB_RETURN(EFS_BAB_STORE_DATA_IN_SB, -result);

  EFS_BAB_END (EFS_BAB_STORE_DATA_IN_SB);

  fs_global_unlock ();

  if (result != 0)
  {
    efs_errno = -result;
    return -1;
  }

  return 0;
}


struct fs_golden_copy_info_type
{
  int                         is_golden_copy_from_files_list_supported;
  fs_os_tcb_type              *fs_task_tcb;

  fs_os_crit_sect_type        fs_golden_copy_crit_sect;
  int                         is_in_progress;
  int                         last_errno;

  enum fs_golden_copy_type    fsg_type;
  uint8                       *ram_addr;
  uint32                      ram_size;
  uint32                      hdr_size;

  int did_restore_happen;  /* debug only field */
};
static struct fs_golden_copy_info_type fs_golden_copy_info;

struct fs_golden_copy_files_list_info
{
  const char **files_list;
  uint32 count;

  /* Debug fields : To store data about golden-copy restore files count */
  uint32 restored_files_count; /* Total good restored files count */
  uint32 only_in_image; /* Files present only in image and not in files-list */
  uint32 only_in_list; /* Files present only in files-list not in image */
};
static struct fs_golden_copy_files_list_info
              fs_golden_copy_files_list[FS_GOLDEN_COPY_FILES_LIST_MAX];

#ifdef FS_UNIT_TEST
int
sim_efs_golden_copy_files_list_enabled (void)
{
  return fs_golden_copy_info.is_golden_copy_from_files_list_supported;
}

int
sim_efs_golden_copy_set_files_list (uint32 files_count,
                                    const char **files_list)
{
  enum fs_golden_copy_files_list_type type = FS_GOLDEN_COPY_FILES_LIST_DEFAULT;

  fs_golden_copy_files_list[type].count = files_count;
  fs_golden_copy_files_list[type].files_list = files_list;
  fs_golden_copy_files_list[type].restored_files_count = 0;
  fs_golden_copy_files_list[type].only_in_image = 0;
  fs_golden_copy_files_list[type].only_in_list = 0;
  return 0;
}
#endif /* FS_UNIT_TEST */

int
efs_golden_copy_set_oem_files_list (enum fs_golden_copy_files_list_type type,
                                 uint32 files_count, const char **files_list)
{
  if (type >= FS_GOLDEN_COPY_FILES_LIST_MAX)
    return -1;

  if ((files_count == 0) ||
      (files_list == NULL))
      return -1;

  fs_sync_lock ();
  fs_global_lock ();

  fs_golden_copy_files_list[type].files_list = files_list;
  fs_golden_copy_files_list[type].count = files_count;
  fs_golden_copy_files_list[type].restored_files_count = 0;
  fs_golden_copy_files_list[type].only_in_image = 0;
  fs_golden_copy_files_list[type].only_in_list = 0;

  fs_global_unlock ();
  fs_sync_unlock ();

  return 0;
}

int
efs_golden_copy_set_files_list (uint32 files_count, const char **files_list)
{
  enum fs_golden_copy_files_list_type type = FS_GOLDEN_COPY_FILES_LIST_DEFAULT;

  ASSERT(fs_golden_copy_files_list[type].count == 0);
  ASSERT(fs_golden_copy_files_list[type].files_list == NULL);

  fs_golden_copy_files_list[type].count = files_count;
  fs_golden_copy_files_list[type].files_list = files_list;
  fs_golden_copy_files_list[type].restored_files_count = 0;
  fs_golden_copy_files_list[type].only_in_image = 0;
  fs_golden_copy_files_list[type].only_in_list = 0;
  return 0;
}


static int
efs_golden_copy_is_file_present (const char *filepath1,
                                 enum fs_golden_copy_files_list_type type)
{
  uint32 i, len1, len2, files_count;
  const char *filepath2, **files_list;
  int result;

  ASSERT (filepath1 != NULL);
  len1 = strlen (filepath1);
  ASSERT (len1 != 0);
  ASSERT (type < FS_GOLDEN_COPY_FILES_LIST_MAX);
  if (type >= FS_GOLDEN_COPY_FILES_LIST_MAX)
  {
    return 0;
  }

  files_count = fs_golden_copy_files_list[type].count;
  files_list = fs_golden_copy_files_list[type].files_list;

  for ( i = 0; i < files_count; ++i )
  {
    filepath2 = files_list[i];
    len2 = strlen (filepath2);
    if (len1 == len2)
    {
      result = memcmp (filepath1, filepath2, len1);
      if (result == 0)
        return 1;
    }
  }

  return 0;
}

int
efs_make_golden_copy_no_wait (const char *path,
                              enum fs_golden_copy_type golden_copy_type)
{
  int result;

  FS_MSG_HIGH_1("efs_golden_copy_no_wait begin : %d", golden_copy_type);

  /* Check for the path validity. */
  result = fs_is_path_valid (path);
  if (result != 0)
  {
    efs_errno = EINVAL;
    return -1;
  }
  switch (golden_copy_type)
  {
    case FS_GOLDEN_COPY_TYPE_FULL_SNAPSHOT:
    case FS_GOLDEN_COPY_TYPE_FILES_LIST:
    case FS_GOLDEN_COPY_TYPE_FILES_LIST_OEM_1:
    case FS_GOLDEN_COPY_TYPE_FILES_LIST_OEM_2:
      break;

    default:
      efs_errno = EINVAL;
      return -1;
  }

  fs_os_enter_crit_sect (&fs_golden_copy_info.fs_golden_copy_crit_sect);
  if (fs_golden_copy_info.is_in_progress)
  {
    result = -EBUSY;
  }
  else
  {
    result = 0;
    fs_golden_copy_info.is_in_progress = 1;
    fs_golden_copy_info.last_errno = 0;
  }
  fs_os_leave_crit_sect (&fs_golden_copy_info.fs_golden_copy_crit_sect);

  if (result != 0)
    goto End;

  fs_global_lock ();

  fs_golden_copy_info.fsg_type = golden_copy_type;

  ASSERT (fs_golden_copy_info.fs_task_tcb != NULL);
  (void) fs_os_set_sigs (fs_golden_copy_info.fs_task_tcb,
                         FS_EFS_GOLDEN_COPY_SIG);
  result = 0;

  fs_global_unlock ();

End:
  FS_MSG_HIGH_1("efs_golden_copy_no_wait end : %d", result);
  if (result < 0)
  {
    efs_errno = -result;
    return -1;
  }
  else
  {
    return 0;
  }
}


int
efs_golden_copy_no_wait_get_status (const char *path,
                                    enum fs_golden_copy_status *status)
{
  int result;

  /* Check for the path validity. */
  result = fs_is_path_valid (path);
  if (result != 0)
  {
    efs_errno = EINVAL;
    return -1;
  }
  if (status == NULL)
  {
    efs_errno = EINVAL;
    return -1;
  }

  fs_os_enter_crit_sect (&fs_golden_copy_info.fs_golden_copy_crit_sect);
  if (fs_golden_copy_info.is_in_progress)
  {
    *status = FS_GOLDEN_COPY_STATUS_IN_PROGRESS;
    result = 0;
  }
  else
  {
    *status = FS_GOLDEN_COPY_STATUS_COMPLETE;
    result = fs_golden_copy_info.last_errno;
  }
  fs_os_leave_crit_sect (&fs_golden_copy_info.fs_golden_copy_crit_sect);

  FS_MSG_HIGH_2("fsg get status, %d,%d", result, *status);

  if (result != 0)
  {
    efs_errno = result;
    return -1;
  }
  else
  {
    return 0;
  }
}


static int
efs_write_to_golden_copy (const char *path, uint8 *ram_addr, uint32 ram_size,
                          enum fs_golden_copy_partition_type fsg_partition)
{
  struct nameidata *ndp = &nameidata_buffer;
  int result = -1;

  fs_global_lock ();

  result = fs_namei (ndp, path, 0);
  if ((result != 0) || (ndp->vp == NULL))
  {
    FS_ERR_FATAL ("[%d] Cannot find root FS", efs_errno, 0, 0);
  }

  result = ndp->vp->vops->write_to_fsg (ndp->vp, ram_addr, ram_size,
                                        fsg_partition);
  if (ndp->dvp != NULL)
    fs_vnode_unref (ndp->dvp);

  if (ndp->vp != NULL)
    fs_vnode_unref (ndp->vp);

  fs_global_unlock ();

  if (result != 0)
  {
    efs_errno = -result;
    result = -1;
  }
  return result;
}


static int
efs_make_golden_copy_from_files_list (const char *path,
              enum fs_golden_copy_files_list_type files_list_type,
              enum fs_golden_copy_partition_type fsg_partition)
{
  const char *filepath;
  int result;
  uint32 i, buf_size, image_size, files_count;
  const char **files_list;
  uint8 *buf;

  ASSERT (path != NULL);
  ASSERT (files_list_type < FS_GOLDEN_COPY_FILES_LIST_MAX);
  ASSERT (fsg_partition < FS_GOLDEN_COPY_PARTITION_TYPE_MAX);
  if (files_list_type >= FS_GOLDEN_COPY_FILES_LIST_MAX)
  {
    result = -EINVAL;
    goto End;
  }
  if (fsg_partition >= FS_GOLDEN_COPY_PARTITION_TYPE_MAX)
  {
    result = -EINVAL;
    goto End;
  }

  files_count = fs_golden_copy_files_list[files_list_type].count;
  files_list = fs_golden_copy_files_list[files_list_type].files_list;

  FS_MSG_HIGH_1("fsg files list : %d", files_count);

  if (!fs_golden_copy_info.is_golden_copy_from_files_list_supported)
  {
    FS_MSG_ERROR_0 ("FSG from files list is not supported");
    result = -EPERM;
    goto End;
  }

  result = efs_get_buf_for_fsg (path, &fs_golden_copy_info.ram_addr,
              &fs_golden_copy_info.ram_size, &fs_golden_copy_info.hdr_size);
  if (result != 0)
    goto End;

  ASSERT (fs_golden_copy_info.ram_addr != NULL);
  ASSERT (fs_golden_copy_info.ram_size > 0);
  ASSERT (fs_golden_copy_info.hdr_size <= fs_golden_copy_info.ram_size);

  buf = fs_golden_copy_info.ram_addr;
  buf += fs_golden_copy_info.hdr_size;
  buf_size = fs_golden_copy_info.ram_size - fs_golden_copy_info.hdr_size;

  fs_golden_copy_create_begin (buf, buf_size, files_list_type);

  for ( i = 0; i < files_count; ++i )
  {
    filepath = files_list[i];
    fs_golden_copy_create_add_file (filepath);
  }

  image_size = fs_golden_copy_create_end ();
  ASSERT ((image_size > 0) && (image_size <= buf_size));

  image_size += fs_golden_copy_info.hdr_size;
  ASSERT (image_size <= fs_golden_copy_info.ram_size);

  result = efs_write_to_golden_copy (path, fs_golden_copy_info.ram_addr,
                                     image_size, fsg_partition);
  if (result != 0)
    goto End;

  result = 0;

End:
  FS_MSG_HIGH_1("fsg files list: %d", result);
  if (result < 0)
  {
    efs_errno = -result;
    return -1;
  }
  else
  {
    return 0;
  }
}

int
efs_golden_copy_signal_received (void)
{
  int result = -1;

  FS_MSG_HIGH_0("make fsg signal rcvd");

  fs_os_enter_crit_sect (&fs_golden_copy_info.fs_golden_copy_crit_sect);
  ASSERT (fs_golden_copy_info.is_in_progress == 1);
  fs_os_leave_crit_sect (&fs_golden_copy_info.fs_golden_copy_crit_sect);

  fs_sync_lock ();
  fs_global_lock ();

  ASSERT (fs_golden_copy_info.fs_task_tcb != NULL);

  switch (fs_golden_copy_info.fsg_type)
  {
    case FS_GOLDEN_COPY_TYPE_FULL_SNAPSHOT:
      result = efs_make_golden_copy ("/");
      break;

    case FS_GOLDEN_COPY_TYPE_FILES_LIST:
      result = efs_make_golden_copy_from_files_list ("/",
                FS_GOLDEN_COPY_FILES_LIST_DEFAULT,
                FS_GOLDEN_COPY_PARTITION_TYPE_DEFAULT);
      break;

    case FS_GOLDEN_COPY_TYPE_FILES_LIST_OEM_1:
      result = efs_make_golden_copy_from_files_list ("/",
                FS_GOLDEN_COPY_FILES_LIST_OEM_1,
                FS_GOLDEN_COPY_PARTITION_TYPE_OEM_1);
      break;

    case FS_GOLDEN_COPY_TYPE_FILES_LIST_OEM_2:
      result = efs_make_golden_copy_from_files_list ("/",
                FS_GOLDEN_COPY_FILES_LIST_OEM_2,
                FS_GOLDEN_COPY_PARTITION_TYPE_OEM_2);
      break;

    default:
      result = -EINVAL;
      ASSERT (0);
      break;
  };

  fs_global_unlock ();
  fs_sync_unlock ();

  fs_os_enter_crit_sect (&fs_golden_copy_info.fs_golden_copy_crit_sect);
  if (result == 0)
    fs_golden_copy_info.last_errno = 0;
  else
    fs_golden_copy_info.last_errno = efs_errno;
  fs_golden_copy_info.is_in_progress = 0;
  fs_os_leave_crit_sect (&fs_golden_copy_info.fs_golden_copy_crit_sect);

  FS_MSG_HIGH_1("make fsg signal : %d", result);
  return result;
}


static void
efs_golden_copy_restore (void)
{
  enum fs_golden_copy_files_list_type type = FS_GOLDEN_COPY_FILES_LIST_DEFAULT;
  struct fs_golden_copy_entry *entry;
  const char *file_path;
  int result, oflag, mode;
  uint32 buf_size;
  uint8 *buf;

  fs_sync_lock ();
  fs_global_lock ();

  if (!fs_golden_copy_info.is_golden_copy_from_files_list_supported)
    goto End;

  if ((fs_golden_copy_files_list[type].files_list == NULL) ||
      (fs_golden_copy_files_list[type].count == 0))
  {
    FS_ERR_FATAL ("golden-copy files list is empty : %d %d %d", 0, 0, 0);
  }

  result = fs_golden_copy_is_okay_to_restore ();
  if (result != 1)
    goto End;

  fs_golden_copy_info.did_restore_happen = 1;

  result = efs_get_buf_for_fsg ("/", &fs_golden_copy_info.ram_addr,
             &fs_golden_copy_info.ram_size, &fs_golden_copy_info.hdr_size);
  if (result != 0)
    goto End;

  ASSERT (fs_golden_copy_info.ram_addr != NULL);
  ASSERT (fs_golden_copy_info.ram_size > 0);
  ASSERT (fs_golden_copy_info.hdr_size <= fs_golden_copy_info.ram_size);

  buf = fs_golden_copy_info.ram_addr;
  buf += fs_golden_copy_info.hdr_size;
  buf_size = fs_golden_copy_info.ram_size - fs_golden_copy_info.hdr_size;

  result = fs_golden_copy_restore_begin (buf, buf_size);
  if (result != 0)
    goto End;

  oflag = O_CREAT | O_TRUNC | O_WRONLY | O_AUTODIR;
  mode = 0644;

  entry = fs_golden_copy_restore_get_next ();
  while (entry != NULL)
  {
    file_path = (const char *)entry->file_path;
    ASSERT (file_path != NULL);
    ASSERT (entry->files_list_type < FS_GOLDEN_COPY_FILES_LIST_MAX);

    result = efs_golden_copy_is_file_present ((const char *)file_path,
                                              entry->files_list_type);
    if (result == 1)
    {
      ++fs_golden_copy_files_list[type].restored_files_count;
      result = efs_put(file_path, entry->file_data, entry->hdr.file_size,
                       oflag, mode);
      if (result != 0)
        goto End_1;
    }
    else
    {
      ++fs_golden_copy_files_list[type].only_in_image;
    }

    entry = fs_golden_copy_restore_get_next ();
  }

End_1:
  fs_golden_copy_restore_end ();

End:
  fs_golden_copy_files_list[type].only_in_list =
    (fs_golden_copy_files_list[type].count -
     fs_golden_copy_files_list[type].restored_files_count);

  fs_global_unlock ();
  fs_sync_unlock ();

  FS_MSG_HIGH_3("%d, %d, %d : restore fsg status",
    fs_golden_copy_files_list[type].count,
    fs_golden_copy_files_list[type].only_in_image,
    fs_golden_copy_files_list[type].only_in_list);

  return;
}

static void
efs_golden_copy_init (void)
{
  fs_sync_lock ();
  fs_global_lock ();

  memset(&fs_golden_copy_info, 0, sizeof(fs_golden_copy_info));

  fs_os_init_crit_sect (&fs_golden_copy_info.fs_golden_copy_crit_sect);

#ifdef FEATURE_EFS_RMTS_GOLDEN_COPY_FROM_FILES_LIST
  fs_golden_copy_info.is_golden_copy_from_files_list_supported = 1;
#endif

  fs_golden_copy_info.fs_task_tcb = fs_os_self ();
  ASSERT(fs_golden_copy_info.fs_task_tcb != NULL);

  fs_golden_copy_files_list_init ();

  fs_global_unlock ();
  fs_sync_unlock ();
}

void efs_rcinit_group_3_func (void)
{
  ASSERT (fs_golden_copy_info.fs_task_tcb != NULL);
  (void) fs_os_set_sigs (fs_golden_copy_info.fs_task_tcb,
                         FS_EFS_GOLDEN_COPY_RESTORE_SIG);
}

int
efs_golden_copy_restore_signal_received (void)
{
  FS_MSG_HIGH_0("fsg restore signal rcvd");
  efs_golden_copy_restore ();
  FS_MSG_HIGH_0("fsg restore complete");
  return 0;
}

#endif /* #ifndef FEATURE_EFS_BOOT_ENV */

int efs_boot_mount_first_fat(enum coldplug_device_type device_type,
			int is_embedded_device, const char *mount_path)
{
	int result;
	struct coldplug_iter *citer = NULL;
	struct coldplug_iter_record *citer_rec = NULL;
  
  result = efs_boot_init_romfs ();
  if (result != 0)
    goto End;
  
  (void) coldplug_open_device(device_type, is_embedded_device);

	citer = coldplug_iter_open(device_type, is_embedded_device, 1);
	if (citer == NULL) {
		result = COLDPLUG_RESULT_ENOMEM;
		goto Error;
	}

	citer_rec = coldplug_iter_next(citer);
	if (citer_rec == NULL) {
		result = COLDPLUG_RESULT_ENODEV;
		goto Error;
	}

	result = efs_mount("", mount_path, "extfs", 0, citer_rec->parti_name);
	if (result != 0)
		goto Error;

	result = 0;
	goto End;

Error:
	goto End;
End:
	if (citer != NULL)
		coldplug_iter_close(citer);
	return result;
}

#endif /* FEATURE_EFS2 */
