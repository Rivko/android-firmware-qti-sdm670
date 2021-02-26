/***********************************************************************
 * fs_os_msg.h
 *
 * Diag MSG Wrapper for all FS Modules
 * Copyright (C) 2015 QUALCOMM Technologies, Inc.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.qdsp6/2.1.c4/storage/fs_osal/inc/fs_os_msg.h#1 $ $DateTime: 2018/07/30 01:21:36 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2015-03-13   dks   Create

===========================================================================*/

#ifndef __FS_OS_MSG_H__
#define __FS_OS_MSG_H__

#include "fs_os_config_i.h"
#include "msg.h"

#if defined (FS_OS_MPSS_BUILD)
  #define FS_OS_PROC_NAME "MPSS "
#elif defined (FS_OS_AUDIO_BUILD)
  #define FS_OS_PROC_NAME "ADSP "
#elif defined (FS_OS_SENSORS_BUILD)
  #define FS_OS_PROC_NAME "SNSR "
#elif defined (FS_OS_SPARROW_BUILD)
  #define FS_OS_PROC_NAME "APSS "
#else
  #define FS_OS_PROC_NAME " "
#endif

/* Resuing MSG_SSID_RFS_ACCESS for fs_osal since SSID are expensive */

#define FS_OS_MSG_ERROR_0(a) \
        MSG(MSG_SSID_RFS_ACCESS,MSG_LEGACY_ERROR,FS_OS_PROC_NAME a)
#define FS_OS_MSG_ERROR_1(a,b) \
        MSG_1(MSG_SSID_RFS_ACCESS,MSG_LEGACY_ERROR,FS_OS_PROC_NAME a,b)
#define FS_OS_MSG_ERROR_2(a,b,c) \
        MSG_2(MSG_SSID_RFS_ACCESS,MSG_LEGACY_ERROR,FS_OS_PROC_NAME a,b,c)
#define FS_OS_MSG_ERROR_3(a,b,c,d) \
        MSG_3(MSG_SSID_RFS_ACCESS,MSG_LEGACY_ERROR,FS_OS_PROC_NAME a,b,c,d)

#define FS_OS_MSG_HIGH_0(a) \
        MSG(MSG_SSID_RFS_ACCESS,MSG_LEGACY_HIGH,FS_OS_PROC_NAME a)
#define FS_OS_MSG_HIGH_1(a,b) \
        MSG_1(MSG_SSID_RFS_ACCESS,MSG_LEGACY_HIGH,FS_OS_PROC_NAME a,b)
#define FS_OS_MSG_HIGH_2(a,b,c) \
        MSG_2(MSG_SSID_RFS_ACCESS,MSG_LEGACY_HIGH,FS_OS_PROC_NAME a,b,c)
#define FS_OS_MSG_HIGH_3(a,b,c,d) \
        MSG_3(MSG_SSID_RFS_ACCESS,MSG_LEGACY_HIGH,FS_OS_PROC_NAME a,b,c,d)

#define FS_OS_MSG_MED_0(a) \
        MSG(MSG_SSID_RFS_ACCESS,MSG_LEGACY_MED,FS_OS_PROC_NAME a)
#define FS_OS_MSG_MED_1(a,b) \
        MSG_1(MSG_SSID_RFS_ACCESS,MSG_LEGACY_MED,FS_OS_PROC_NAME a,b)
#define FS_OS_MSG_MED_2(a,b,c) \
        MSG_2(MSG_SSID_RFS_ACCESS,MSG_LEGACY_MED,FS_OS_PROC_NAME a,b,c)
#define FS_OS_MSG_MED_3(a,b,c,d) \
        MSG_3(MSG_SSID_RFS_ACCESS,MSG_LEGACY_MED,FS_OS_PROC_NAME a,b,c,d)

#define FS_OS_MSG_LOW_0(a) \
        MSG(MSG_SSID_RFS_ACCESS,MSG_LEGACY_LOW,FS_OS_PROC_NAME a)
#define FS_OS_MSG_LOW_1(a,b) \
        MSG_1(MSG_SSID_RFS_ACCESS,MSG_LEGACY_LOW,FS_OS_PROC_NAME a,b)
#define FS_OS_MSG_LOW_2(a,b,c) \
        MSG_2(MSG_SSID_RFS_ACCESS,MSG_LEGACY_LOW,FS_OS_PROC_NAME a,b,c)
#define FS_OS_MSG_LOW_3(a,b,c,d) \
        MSG_3(MSG_SSID_RFS_ACCESS,MSG_LEGACY_LOW,FS_OS_PROC_NAME a,b,c,d)

#define FS_OS_MSG_STRING_1(a,b) \
        MSG_SPRINTF_1(MSG_SSID_RFS_ACCESS,MSG_LEGACY_HIGH,FS_OS_PROC_NAME a,b)

#endif /* not __FS_OS_MSG_H__ */
