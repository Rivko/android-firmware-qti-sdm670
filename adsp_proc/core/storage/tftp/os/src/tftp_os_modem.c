/***********************************************************************
 * tftp_os_modem.c
 *
 * NON-HLOS OS Abstraction.
 * Copyright (c) 2014-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.qdsp6/2.1/storage/tftp/os/src/tftp_os_modem.c#1 $ $DateTime: 2017/07/21 22:10:47 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2017-06-05   rp    Change uid/gid for RAM-dump files.
2017-05-31   rp    Port to SLPI image.
2015-01-05   vm    Compile server for apss.
2014-12-30   dks   Fixes to config and log module.
2014-10-14   rp    Use asserts for control-logic, debug-asserts for data-logic
2014-06-04   rp    Create

===========================================================================*/

#include "tftp_config_i.h"
#include "tftp_comdef.h"
#include "tftp_file.h"

#if !defined (TFTP_NHLOS_BUILD)
  #error "This file should be included only for NHLOS Builds"
#endif

volatile int tftp_assert_run_infinite_loop = 0;

#if defined TFTP_TN_BUILD && defined FEATURE_TFTP_SERVER_BUILD

  #include "fs_public.h"
  #include "fs_errno.h"
  #include "rcinit.h"

  void
  tftp_os_init (void)
  {
#ifdef TFTP_SIMULATOR_BUILD
  tftp_assert_run_infinite_loop =  0;
#else
  tftp_assert_run_infinite_loop =  0;
#endif
    rcinit_handshake_startup ();
  }


  int32
  tftp_os_unlink (const char *path)
  {
    int32 result;

    result = efs_unlink (path);

    if (result != 0)
    {
      result = -(efs_errno);
    }
    return result;
  }

  static int32
  tftp_os_open_file (const char *path, int32 oflags, uint16 mode,
                      int32 uid, int32 gid)
  {
    int32 result;

    (void) uid;
    (void) gid;

    /* This mode when used is modified by umask */
    result = efs_open (path, oflags, mode);

    if (result < 0)
    {
      result = -(efs_errno);
    }

    return result;
  }

  int32
  tftp_os_file_open_helper(const char *path, int32 oflags, uint16 mode_bits,
                             int32 uid, int32 gid, tftp_file_handle *handle,
                             const char *stream_mode)
  {
    int32 open_res, fd;

    open_res = tftp_os_open_file (path, oflags, mode_bits, uid, gid);
    if (open_res < 0)
    {
      return open_res;
    }

    /* If we are here then the file was successfully opened. Now get a file
       stream from the fd  */
    fd = open_res;
    open_res = tftp_file_open_stream (handle, fd, stream_mode);
    return open_res;
  }

  int32
  tftp_os_mkdir (const char *path, uint16 mode, int32 uid, int32 gid)
  {
    int32 result;

    (void) uid;
    (void) gid;

    /* This mode when used is modified by umask */
    result = efs_mkdir (path, mode);

    if (result != 0)
    {
      result = -(efs_errno);
    }

    return result;
  }

  /* There is no need to change the mode on sparrow. */
  int32
  tftp_os_chown (const char *path, int32 uid, int32 gid)
  {
    (void) path;
    (void) uid;
    (void) gid;

    return 0;
  }

  int32 tftp_os_symlink (const char *symlink_path, const char *target_path,
                         int is_dir)
  {
    int32 result;

    (void) is_dir;
    result = efs_symlink (target_path, symlink_path);

    if (result != 0)
    {
      result = -(efs_errno);
    }

    return result;
  }

  int32
  ttftp_os_datasync_for_fd (int32 fd)
  {
    (void) fd;
    return 0;
  }

  int32
  tftp_os_datasync (const char *path)
  {
    (void) path;
    return 0;
  }

  int32
  tftp_os_rename (const char *oldpath, const char *newpath)
  {
    int32 result = 0;
    result = efs_rename(oldpath, newpath);

    if (result != 0)
    {
      result = -(efs_errno);
    }
    return result;
  }

#else

  void
  tftp_os_init (void)
  {
  #ifdef TFTP_SIMULATOR_BUILD
    tftp_assert_run_infinite_loop =  0;
  #else
    tftp_assert_run_infinite_loop =  0;
  #endif
  }

#endif

