/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        CONTENT PROTECTION UTILITIES

GENERAL DESCRIPTION
  Utility functions to aid with content protection.

EXTERNALIZED FUNCTIONS
  None

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2012 - 2015 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                       EDIT HISTORY FOR MODULE


when       who           what, where, why
--------   -----------   ----------------------------------------------------
01/13/2014     wt        Created
===========================================================================*/

#ifndef __CPUTILS_H__
#define __CPUTILS_H__
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#include "comdef.h"
#include <utils/Log.h>
#include "QSEEComAPI.h"
#include "common_log.h"
#include <linux/msm_ion.h>
#include <sys/mman.h>
#include <fcntl.h>
#include "content_protection_copy.h"


//types
//typedef unsigned int uintptr_t;

//mem tag use cases
#define QSEE_TAG_MEM_USECASE_CP 0x01
#define QSEE_TAG_MEM_USECASE_AUDIO_CP 0x02
#define QSEE_TAG_MEM_USECASE_VIDEO_PIXEL_CP 0x03
#define QSEE_TAG_MEM_USECASE_SD 0x04
#define QSEE_TAG_MEM_USECASE_MAX 0x7FFFFFFF

// TZ Sample App/Client command
#define CLIENT_CMD24_RUN_CP_UTILS_TAG_MEM   24

//Errors
typedef enum CPUtilsResult {
  CPUTILS_SUCCESS = 0,
  CPUTILS_ERROR_CP_TAG_MEM_FAILED,
  CPUTILS_ERROR_FEATURE_NOT_SUPPORTED,
  CPUTILS_FAILURE = 0x7FFFFFFF
} CPUtilsResult;

//TZ send command structures
typedef struct tz_cp_tag_mem_req_s
{
  /** First 4 bytes should always be command id */
  uint32                          cmd_id;
  uint32                          tag;
  tz_buf_array_s_t                secBufferHandle;
  uint32                          secBuffLength;
} __attribute__ ((packed)) tz_cp_tag_mem_req_t;

typedef struct tz_cp_tag_mem_rsp_s
{
  /** First 4 bytes should always be command id */
  uint32                          cmd_id;
  long                            ret;
} __attribute__ ((packed)) tz_cp_tag_mem_rsp_t;

/**
  This API tags a secure buffer with a CP tag.

  @return
  CPUTILS_SUCCESS                 - Success.
  CPUTILS_ERROR_CP_TAG_MEM_FAILED - Failure.

  @param[out]      oQSEEComHandle  handle
  @param[out]      tag             tag type
  @param[in]       in_fd           secure buffer handle
  @param[in]       buffLen         size memory to tag

  @dependencies
  None.

  @sideeffects
  None.
*/
CPUtilsResult cp_utils_tag_mem
(
  struct QSEECom_handle *oQSEEComHandle,
  uint32 tag,
  int32_t in_fd,
  uint32 buffLen
);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // __CPUTILS_H__
