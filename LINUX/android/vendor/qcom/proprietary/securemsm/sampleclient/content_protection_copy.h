#ifndef CONTENT_PROTECTION_COPY_H
#define CONTENT_PROTECTION_COPY_H

/** @file oemcrypto_copy.h
 * @brief
 * This file contains the definitions of the constants, data structures and
 * and interfaces to the oemcrypto_copy API.
 */
/*===========================================================================
  Copyright (c) 2011-2014 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/07/15   cz      Changed tz_buf_seg address to 64 bits
01/29/15   kr      Added ION_Malloc_system_heap API
12/17/13   wt      Created

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/** @addtogroup OEMCrypto_Copy
  @} */

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <linux/msm_ion.h>
#include <utils/Log.h>
#include "QSEEComAPI.h"
#include "common_log.h"
#include <sys/mman.h>
#include <getopt.h>
#include "comdef.h"


//globals
#define TZ_SC_AES_128_LEN             (16)
#define TZ_OUT_BUF_MAX                (512)
#define SAMPLECLIENT_BUF_SIZE         (100*1024)
#define SAMPLECLIENT_AUDIO_MAX_BUFFER (64*1024*2)
#define SAMPLECLIENT_VIDEO_MAX_BUFFER ((1024*1024*2) - SAMPLECLIENT_AUDIO_MAX_BUFFER)
#define MAX_CLIENT_NAME 25
#define SZ_1M 0x100000
static char sample_client_name[MAX_CLIENT_NAME] = "smpcpyap";
static pthread_mutex_t* g_mutex = NULL;

//Errors
typedef enum SampleClientResult {
  SAMPLE_CLIENT_SUCCESS                           = 0,
  SAMPLE_CLIENT_ERROR_COPY_FAILED                 = 1,
  SAMPLE_CLIENT_ERROR_INIT_FAILED                 = 2,
  SAMPLE_CLIENT_ERROR_TERMINATE_FAILED            = 3,
  SAMPLE_CLIENT_ERROR_ION_MALLOC_FAILED           = 4,
  SAMPLE_CLIENT_ERROR_ION_FREE_FAILED             = 5,
  SAMPLE_CLIENT_ERROR_NSS_COPY_FAILED             = 6,
  SAMPLE_CLIENT_ERROR_SNS_COPY_FAILED             = 7,
  SAMPLE_CLIENT_ERROR_MEM_SEG_COPY_FAILED         = 8,
  SAMPLE_CLIENT_ERROR_INVALID_PARAMS              = 9,
  SAMPLE_CLIENT_ERROR_FEATURE_NOT_SUPPORT         = 10,
  SAMPLE_CLIENT_ERROR_BUFFER_TOO_SHORT            = 11,
  SAMPLE_CLIENT_ERROR_SECURE_ION_MALLOC_FAILED    = 12,
  SAMPLE_CLIENT_ERROR_FEATURE_NOT_SUPPORTED       = 13,
  SAMPLE_CLIENT_FAILURE = 0x7FFFFFFF
} SampleClientResult;

//copy direction
typedef enum SampleClientCopyDir {
  SAMPLECLIENT_COPY_NONSECURE_TO_SECURE = 0,
  SAMPLECLIENT_COPY_SECURE_TO_NONSECURE,
  SAMPLECLIENT_COPY_INVALID_DIR
}SampleClientCopyDir;

// structure for non-contiguous buffers
struct tz_buf_seg
{
  uint64 address;
  uint32 size;
} __attribute__ ((packed));

typedef struct tz_buf_array_s
{
  struct tz_buf_seg buf_seg[TZ_OUT_BUF_MAX];
  uint32 seg_num;
} __attribute__ ((packed)) tz_buf_array_s_t;

//TZ send command structures
typedef struct tz_sc_copy_req_s
{
  /** First 4 bytes should always be command id */
  uint32                          cmd_id;
  uint32                          copyDir;
  uint64                          nonSecBuffer;
  uint32                          nonSecBufferLength;
  tz_buf_array_s_t                secBufferHandle;
  uint32                          secBufferOffset;
} __attribute__ ((packed)) tz_sc_copy_req_t;

typedef struct tz_sc_copy_rsp_s
{
  /** First 4 bytes should always be command id */
  uint32                          cmd_id;
  uint32                          secBufferLength;
  long                            ret;
} __attribute__ ((packed)) tz_sc_copy_rsp_t;


/**
  This API allocates a non-secure ION buffer.

  @return
  SAMPLE_CLIENT_SUCCESS                 - Success.
  SAMPLE_CLIENT_ERROR_ION_MALLOC_FAILED - Failure.

  @param[out]      ion_vir_addr pointer to ion virtual address
  @param[out]      ion_fd       pre-alloc ion memory return value
  @param[in]       ion_fd_data  ion data
  @param[in]       memsize      size of space to malloc

  @dependencies
  None.

  @sideeffects
  None.
*/
SampleClientResult SampleClient_ION_Malloc
(
  uint8**             ion_vir_addr,
  int*                ion_fd,
  struct ion_fd_data* ion_fd_data,
  uint32              memsize
);

/**
  This API allocates a non-secure System Heap ION buffer.

  @return
  SAMPLE_CLIENT_SUCCESS                 - Success.
  SAMPLE_CLIENT_ERROR_ION_MALLOC_FAILED - Failure.

  @param[out]      ion_vir_addr pointer to ion virtual address
  @param[out]      ion_fd       pre-alloc ion memory return value
  @param[in]       ion_fd_data  ion data
  @param[in]       memsize      size of space to malloc

  @dependencies
  None.

  @sideeffects
  None.
*/
SampleClientResult SampleClient_ION_Malloc_system_heap
(
  uint8**             ion_vir_addr,
  int*                ion_fd,
  struct ion_fd_data* ion_fd_data,
  uint32              memsize
);

/**
  This API allocates a secure ION buffer.

  @return
  SAMPLE_CLIENT_SUCCESS                        - Success.
  SAMPLE_CLIENT_ERROR_SECURE_ION_MALLOC_FAILED - Failure.

  @param[out]      ion_fd       pre-alloc ion memory return value
  @param[in]       ion_fd_data  ion data
  @param[in]       memsize      size of space to malloc

  @dependencies
  None.

  @sideeffects
  None.
*/
SampleClientResult SampleClient_ION_Secure_Malloc
(
  int* ion_fd,
  struct ion_fd_data* ion_fd_data,
  uint32 memsize
);

/**
  This API frees a ION buffer.

  @return
  SAMPLE_CLIENT_SUCCESS               - Success.
  SAMPLE_CLIENT_ERROR_ION_FREE_FAILED - Failure.

  @param[in]       ion_vir_addr pointer to memory to free
  @param[in]       ion_fd       pre-alloc ion memory return value
  @param[in]       ion_fd_data  ion data
  @param[in]       memsize      size of space to free

  @dependencies
  None.

  @sideeffects
  None.
*/
SampleClientResult SampleClient_ION_Free
(
  uint8**             ion_vir_addr,
  int*                ion_fd,
  struct ion_fd_data* ion_fd_data,
  uint32              memsize
);

/*
  This API loads the secure app and returns the handle to the client.

  @return
  SAMPLE_CLIENT_SUCCESS                  - Success.
  SAMPLE_CLIENT_ERROR_INIT_FAILED        - Failure.

  @param[out]       l_QSEEComHandle      pointer to the QSEECOM handle

  @dependencies
  This function should be invoked by the client before starting any secure session
  using Content_Protection_Copy().

  @sideeffects
  Should be used only for internal testing purpose.
*/
SampleClientResult Content_Protection_Copy_Init
(
   struct QSEECom_handle     **l_QSEEComHandle
);

/*
  This API unloads the secure app corresponding to the client handle.

  @return
  SAMPLE_CLIENT_SUCCESS                  - Success.
  SAMPLE_CLIENT_ERROR_TERMINATE_FAILED   - Failure.

  @param[in]       l_QSEEComHandle      pointer to the QSEECOM handle

  @dependencies
  This function should be invoked by the client after terminating all secure session
  using Content_Protection_Copy().

  @sideeffects
  Should be used only for internal testing purpose.
*/
SampleClientResult Content_Protection_Copy_Terminate
(
   struct QSEECom_handle     **l_QSEEComHandle
);

/**
  This API copies (AES-128-CBC)the content in the non-secure buffer or secure buffer, referenced by
  the *nonSecBuffer parameter, into the secure buffer or non-secure buffer respectively, referenced
  by the secBufferHandle.

  @return
  SAMPLE_CLIENT_SUCCESS              - Success.
  SAMPLE_CLIENT_ERROR_COPY_FAILED    - Failure.

  @param[in]       l_QSEEComHandle        pointer to the QSEECOM handle
  @param[in/out]   nonSecBuffer           non-secure buffer
  @param[in]       nonSecBufferLength     number of bytes in the non-secure buffer
  @param[in]       secBufferHandle        reference to the secure buffer
  @param[in]       secBufferOffset        offset from the beginning of the secure buffer which the
                                          data will be written/read
  @param[out]      secBufferLength        number of bytes written into the receiving buffer
  @param[in]       CopyDir                Specifies whether to copy from secure to non-secure buffer
                                          (or) from non-secure to secure buffer

  @dependencies
  This function must only be invoked after using Content_Protection_Copy_Init() and should not terminate
  until Content_Protection_Copy_Terminate() has been called.

  @sideeffects
  Should be used only for internal testing purpose.
*/
SampleClientResult Content_Protection_Copy
(
  struct QSEECom_handle*    l_QSEEComHandle,
  uint8*                    nonSecBuffer,
  const uint32              nonSecBufferLength,
  uint32                    secBufferHandle,
  uint32                    secBufferOffset,
  uint32*                   secBufferLength,
  SampleClientCopyDir       CopyDir
);

#endif /* CONTENT_PROTECTION_COPY_H */
