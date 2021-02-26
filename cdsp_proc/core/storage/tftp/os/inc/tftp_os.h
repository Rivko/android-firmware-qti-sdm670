/***********************************************************************
 * tftp_os.h
 *
 * The TFTP OS Abstraction layer.
 * Copyright (c) 2013-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 * The TFTP OS Abstraction layer.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.qdsp6/2.1.c4/storage/tftp/os/inc/tftp_os.h#1 $ $DateTime: 2018/07/30 01:21:36 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2017-06-05   rp    Change uid/gid for RAM-dump files.
2015-01-05   dks   Compiling server for TN Apps.
2014-06-04   rp    Switch to IPCRouter sockets.
2013-11-21   nr    Abstract the OS layer across the OS's.
2013-11-14   rp    Create

===========================================================================*/

#ifndef __TFTP_OS_H__
#define __TFTP_OS_H__


#include "tftp_config_i.h"
#include "tftp_comdef.h"
#include "tftp_file.h"

#if defined (TFTP_NHLOS_BUILD)
  #include "tftp_os_modem.h"
#elif defined (TFTP_LA_BUILD)
  #include "tftp_os_la.h"
#elif defined (TFTP_WINDOWS_BUILD)
  #include "tftp_os_windows.h"
#else
  #error "OS : Unknown config"
#endif

#define TFTP_DEFAULT_DIR_MODE 0700 /* The default mode bits for directories.*/
#define TFTP_SHARED_DIR_MODE  0770 /* The mode bits for shared directories.*/

#define TFTP_DEFAULT_FILE_MODE 0600 /* The default mode bits for files.*/
#define TFTP_SHARED_FILE_MODE  0660 /* The mode bits for shared files.*/

void tftp_os_init (void);

int32 tftp_os_file_open_helper(const char *path, int32 oflags,
                              uint16 mode_bits, int32 uid, int32 gid,
                               tftp_file_handle *handle,
                               const char *stream_mode);

int32 tftp_os_mkdir (const char *path, uint16 mode, int32 uid, int32 gid);

int32 tftp_os_chown (const char *path, int32 uid, int32 gid);

int32 tftp_os_auto_mkdir (const char *path, uint16 mode, int32 uid, int32 gid);

int32 tftp_os_symlink (const char *symlink_path, const char *target_path,
                       int is_dir);

int32 tftp_os_unlink (const char *path);

int32 tftp_os_rename (const char *oldpath, const char *newpath);

int32 tftp_os_datasync (const char *path);

int32 tftp_os_datasync_for_fd (int32 fd);

int32 tftp_os_lchown_recursive (char *path, int32 uid, int32 gid);

#endif /* not __TFTP_OS_H__ */
