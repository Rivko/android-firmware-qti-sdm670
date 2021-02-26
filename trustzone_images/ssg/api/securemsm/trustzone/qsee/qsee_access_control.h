#ifndef QSEE_ACCESS_CONTROL_H
#define QSEE_ACCESS_CONTROL_H

/* @file qsee_access_control.h

  This file contains access control definitions for the secure apps

  Copyright (c) 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

**/

/*
  The definitions here must not be modified, i.e. add, remove, or change.
  Please contact the access control team for related questions
**/

#include "IAccessControl.h"

/* Virtual Machine ids */
typedef enum 
{
    AC_VM_NONE                                   = IAccessControl_VM_NONE,
    AC_VM_TZ                                     = IAccessControl_VM_TZ,
    AC_VM_HLOS                                   = IAccessControl_VM_HLOS,
    AC_VM_HYP                                    = IAccessControl_VM_HYP,
    AC_VM_ADSP_Q6_ELF                            = IAccessControl_VM_ADSP_Q6_ELF,
    AC_VM_CP_TOUCH                               = IAccessControl_VM_CP_TOUCH,
    AC_VM_CP_BITSTREAM                           = IAccessControl_VM_CP_BITSTREAM,
    AC_VM_CP_PIXEL                               = IAccessControl_VM_CP_PIXEL,
    AC_VM_CP_NON_PIXEL                           = IAccessControl_VM_CP_NON_PIXEL,
    AC_VM_CP_CAMERA                              = IAccessControl_VM_CP_CAMERA,
    AC_VM_MSS_MSA                                = IAccessControl_VM_MSS_MSA,
    AC_VM_CP_SECDISP                             = IAccessControl_VM_CP_SECDISP,
    AC_VM_CP_APP                                 = IAccessControl_VM_CP_APP,
    AC_VM_ALL                                    = IAccessControl_VM_ALL,
    AC_VM_SPSS_SP                                = IAccessControl_VM_SPSS_SP,
    AC_VM_SPSS_NONSP                             = IAccessControl_VM_SPSS_NONSP,
    AC_VM_CP_CAMERA_PREVIEW                      = IAccessControl_VM_CP_CAMERA_PREVIEW,
    AC_VM_CDSP_Q6_ELF                            = IAccessControl_VM_CDSP_Q6_ELF,
    AC_VM_ADSP_SHARED                            = IAccessControl_VM_ADSP_SHARED,
    AC_VM_SPSS_SP_SHARED                         = IAccessControl_VM_SPSS_SP_SHARED,
    AC_VM_KERNEL_PROTECTION                      = IAccessControl_VM_KERNEL_PROTECTION,
    AC_VM_SPSS_HLOS_SHARED                       = IAccessControl_VM_SPSS_HLOS_SHARED,
    AC_VM_CP_NPU                                 = IAccessControl_VM_CP_NPU,
    AC_VM_CP_CDSP                                = IAccessControl_VM_CP_CDSP,
    AC_VM_MAX                                    = IAccessControl_VM_MAX,
} ACVirtualMachineId;

#endif //QSEE_ACCESS_CONTROL_H
