/***********************************************************************
 * fs_qmi_std_interface.h
 *
 * Maps modem_filesystem_v01.h structure names into fs compatible names.
 * Copyright (C) 2011, 2013 QUALCOMM Technologies, Inc.
 *
 * The modem_filesystem_v01.h is a header file containing structures and APIs
 * generated from its IDL file modem_filesystem_v01.idl
 * The IDL file is maintained by the MPSS API Change Control Board (CCB) and
 * follows naming conventions that are standard for all IDLs. This does not
 * match the naming conventions we have in the rest of the files in FS. Hence
 * typedef all the IDL names to FS suitable names.
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.qdsp6/2.1/storage/efs_qmi_put_get/common/inc/fs_qmi_std_interface.h#1 $ $DateTime: 2017/07/21 22:10:47 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-03-28   dks   Added support to query commands/TLVs supported by service.
2011-11-14   dks   Fix security vulnerabilities.
2011-10-04   dks   Create.

===========================================================================*/

#ifndef __FS_QMI_STD_INTERFACE_H__
#define __FS_QMI_STD_INTERFACE_H__

#include "modem_filesystem_v01.h"

/*  Max file path length in bytes. */
#define FS_QMI_MAX_FILE_PATH MFS_MAX_FILE_PATH_V01

/* QMI has a limit on the size of the data it can transfer. This limit is
   currently advertised as 4K bytes of which 3K is what EFS restricts for file
   size. This is not the same as the maximum item file size as per the IDL:
   MFS_MAX_ITEM_FILE_SIZE_V01. The IDL interface should not be linked to the
   implementation. If QMI increases the transfer size for a client, changing
   the IDL will break backward compatibility. To avoid this the IDL maximum is
   kept fairly high. However keeping it too high will occupy too much bandwidth
   since QMI allocates buffers of that maximum size no matter what the actual
   data size. So the size is a trade-off between trying to keep the IDL stable
   and the not hog too much of QMI bandwidth. */
#ifndef FS_QMI_MAX_ITEM_SIZE
  #define FS_QMI_MAX_ITEM_SIZE     3072
#endif

#if (FS_QMI_MAX_ITEM_SIZE > MFS_MAX_ITEM_FILE_SIZE_V01)
  #error "efs qmi item file size greater than IDL limit. Make it smaller"
#endif



#define fs_qmi_errno_type mfs_errno_type_v01

#define FS_QMI_ERRNO_TYPE_MIN_ENUM_VAL MFS_ERRNO_TYPE_MIN_ENUM_VAL_V01
#define FS_QMI_ENOERR                  MFS_ENOERR_V01
#define FS_QMI_EPERM                   MFS_EPERM_V01
#define FS_QMI_ENOENT                  MFS_ENOENT_V01
#define FS_QMI_EEXIST                  MFS_EEXIST_V01
#define FS_QMI_EBADF                   MFS_EBADF_V01
#define FS_QMI_ENOMEM                  MFS_ENOMEM_V01
#define FS_QMI_EACCES                  MFS_EACCES_V01
#define FS_QMI_EBUSY                   MFS_EBUSY_V01
#define FS_QMI_EXDEV                   MFS_EXDEV_V01
#define FS_QMI_ENODEV                  MFS_ENODEV_V01
#define FS_QMI_ENOTDIR                 MFS_ENOTDIR_V01
#define FS_QMI_EISDIR                  MFS_EISDIR_V01
#define FS_QMI_EINVAL                  MFS_EINVAL_V01
#define FS_QMI_EMFILE                  MFS_EMFILE_V01
#define FS_QMI_ETXTBSY                 MFS_ETXTBSY_V01
#define FS_QMI_ENOSPC                  MFS_ENOSPC_V01
#define FS_QMI_ESPIPE                  MFS_ESPIPE_V01
#define FS_QMI_ENAMETOOLONG            MFS_ENAMETOOLONG_V01
#define FS_QMI_ENOTEMPTY               MFS_ENOTEMPTY_V01
#define FS_QMI_ELOOP                   MFS_ELOOP_V01
#define FS_QMI_EILSEQ                  MFS_EILSEQ_V01
#define FS_QMI_ESTALE                  MFS_ESTALE_V01
#define FS_QMI_EDQUOT                  MFS_EDQUOT_V01
#define FS_QMI_ENOCARD                 MFS_ENOCARD_V01
#define FS_QMI_EBADFMT                 MFS_EBADFMT_V01
#define FS_QMI_ENOTITM                 MFS_ENOTITM_V01
#define FS_QMI_EROLLBACK               MFS_EROLLBACK_V01
#define FS_QMI_FS_ERANGE               MFS_FS_ERANGE_V01
#define FS_QMI_EEOF                    MFS_EEOF_V01
#define FS_QMI_EUNKNOWN_SFAT           MFS_EUNKNOWN_SFAT_V01
#define FS_QMI_EUNKNOWN_HFAT           MFS_EUNKNOWN_HFAT_V01
#define FS_QMI_ERRNO_TYPE_MAX_ENUM_VAL MFS_ERRNO_TYPE_MAX_ENUM_VAL_V01


#define fs_qmi_oflag_type mfs_oflag_type_v01

#define FS_QMI_O_ACCMODE   MFS_O_ACCMODE_V01
#define FS_QMI_O_RDONLY    MFS_O_RDONLY_V01
#define FS_QMI_O_WRONLY    MFS_O_WRONLY_V01
#define FS_QMI_O_RDWR      MFS_O_RDWR_V01
#define FS_QMI_O_CREAT     MFS_O_CREAT_V01
#define FS_QMI_O_EXCL      MFS_O_EXCL_V01
#define FS_QMI_O_NOCTTY    MFS_O_NOCTTY_V01
#define FS_QMI_O_TRUNC     MFS_O_TRUNC_V01
#define FS_QMI_O_APPEND    MFS_O_APPEND_V01
#define FS_QMI_O_NONBLOCK  MFS_O_NONBLOCK_V01
#define FS_QMI_O_SYNC      MFS_O_SYNC_V01
#define FS_QMI_FASYNC      MFS_FASYNC_V01
#define FS_QMI_O_DIRECT    MFS_O_DIRECT_V01
#define FS_QMI_O_LARGEFILE MFS_O_LARGEFILE_V01
#define FS_QMI_O_DIRECTORY MFS_O_DIRECTORY_V01
#define FS_QMI_O_NOFOLLOW  MFS_O_NOFOLLOW_V01
#define FS_QMI_O_ITEMFILE  MFS_O_ITEMFILE_V01
#define FS_QMI_O_AUTODIR   MFS_O_AUTODIR_V01
#define FS_QMI_O_SFS_RSVD  MFS_O_SFS_RSVD_V01
#define FS_QMI_O_MAX_OFLAG MFS_O_MAX_OFLAG_V01

#define fs_qmi_efs_put_req_msg mfs_put_req_msg_v01

#define fs_qmi_efs_put_resp_msg mfs_put_resp_msg_v01

#define fs_qmi_efs_get_req_msg mfs_get_req_msg_v01

#define fs_qmi_efs_get_resp_msg mfs_get_resp_msg_v01

#define FS_QMI_EFS_PUT_REQ  QMI_MFS_PUT_REQ_V01
#define FS_QMI_EFS_PUT_RESP QMI_MFS_PUT_RESP_V01
#define FS_QMI_EFS_GET_REQ  QMI_MFS_GET_REQ_V01
#define FS_QMI_EFS_GET_RESP QMI_MFS_GET_RESP_V01

#define fs_qmi_get_service_object()   mfs_get_service_object_v01()
#define fs_qmi_get_service_impl_v01() mfs_get_service_impl_v01()

#endif /* not __FS_QMI_STD_INTERFACE_H__ */
