/*
 * Copyright (c) 2014 QUALCOMM Technologies Inc. All Rights Reserved.
 * Qualcomm Technologies Confidential and Proprietary
 *
 */
#ifndef REMOTE64_H
#define REMOTE64_H

#include "remote.h"

typedef struct {
   uint64_t pv;
   int64_t nLen;
} remote_buf64;

typedef struct {
   int32_t fd;
   uint32_t offset;
   uint32_t len;
} remote_dma_handle64;

typedef union {
   remote_buf64	 buf;
   remote_handle h;
   remote_handle64 h64; //! used by multi domain modules
   remote_dma_handle64 dma;
} remote_arg64;

#ifndef __QAIC_REMOTE
#define __QAIC_REMOTE(ff) ff
#endif //__QAIC_REMOTE

#ifndef __QAIC_REMOTE_EXPORT
#ifdef _WIN32
#define __QAIC_REMOTE_EXPORT __declspec(dllexport)
#else //_WIN32
#define __QAIC_REMOTE_EXPORT
#endif //_WIN32
#endif //__QAIC_REMOTE_EXPORT

#ifndef __QAIC_REMOTE_ATTRIBUTE
#define __QAIC_REMOTE_ATTRIBUTE
#endif

/* map memory to the remote domain
 *
 * @param fd, fd associated with this memory
 * @param flags, flags to be used for the mapping
 * @param vaddrin, input address
 * @param size, size of buffer
 * @param vaddrout, output address
 * @retval, 0 on success
 */
__QAIC_REMOTE_EXPORT int __QAIC_REMOTE(remote_mmap64)(int fd, uint32_t flags, uintptr_t vaddrin, int64_t size, uintptr_t *vaddrout) __QAIC_REMOTE_ATTRIBUTE;

/* unmap memory from the remote domain
 *
 * @param vaddrout, remote address mapped
 * @param size, size to unmap.  Unmapping a range partially may  not be supported.
 * @retval, 0 on success, may fail if memory is still mapped
 */
__QAIC_REMOTE_EXPORT int __QAIC_REMOTE(remote_munmap64)(uintptr_t vaddrout, int64_t size) __QAIC_REMOTE_ATTRIBUTE;

#endif // REMOTE64_H
