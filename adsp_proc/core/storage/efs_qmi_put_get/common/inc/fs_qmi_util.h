/***********************************************************************
 * fs_qmi_util.h
 *
 * APIs for utility functions use by efs_qmi_put_get module.
 * Copyright (C) 2011 QUALCOMM Technologies, Inc.
 *
 * Provides APIs for utility functions that the efs_qmi service and client
 * can use.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.qdsp6/2.1/storage/efs_qmi_put_get/common/inc/fs_qmi_util.h#1 $ $DateTime: 2017/07/21 22:10:47 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2011-10-04   dks   Create

===========================================================================*/

#ifndef __FS_QMI_UTIL_H__
#define __FS_QMI_UTIL_H__

#include "fs_qmi_std_interface.h"
#include "comdef.h"


/* Convert efs_errno value to IDL defined errno value */
fs_qmi_errno_type fs_qmi_get_qmi_errno (int efs_err);

/* Convert IDL defined errno to efs_errno value */
int fs_qmi_get_efs_errno (fs_qmi_errno_type fs_qmi_err);

/* Convert IDL defined oflag to efs oflag value */
int32 fs_qmi_get_efs_oflag (fs_qmi_oflag_type fs_qmi_oflag);

/* Convert efs oflag value IDL defined oflag value */
fs_qmi_oflag_type fs_qmi_get_qmi_oflag (int32 efs_oflag);


#endif /* not __FS_QMI_UTIL_H__ */
