/** @file  
                        scm_util.h

  Header file contaning scm utility functuion prototype

  Copyright (c) 2016 Copyright Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

**/

/*=============================================================================
                EDIT HISTORY


 when       who    what, where, why
 --------   ---     ----------------------------------------------------------
 07/28/16   SA     Initial version

=============================================================================*/

#ifndef SCM_UTIL_H
#define SCM_UTIL_H

#include <Uefi.h>

#define ERROR_SECURITY_STATE 0xFFFFFFFF
#define EFI_FAILURE (80)

UINT32 readSecurityState(void);
EFI_STATUS is_secure_device(BOOLEAN* secure_state);
EFI_STATUS is_secure_device_nocheck_rpmb(BOOLEAN *is_secure_no_rpmb);
EFI_STATUS set_fuse(UINT32 fuse_id);
EFI_STATUS get_fuse(UINT32 fuse_id, BOOLEAN *get_fuse_id);
BOOLEAN allow_set_fuse(UINT32 version);
EFI_STATUS ScmGetFeatureVersion(UINT32  FeatureId, UINT32 *Version);
#endif
