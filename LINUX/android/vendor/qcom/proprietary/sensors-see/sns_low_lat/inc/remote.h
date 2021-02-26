/*
 * Copyright (c) 2012-2014,2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef REMOTE_H
#define REMOTE_H

#include <stdint.h>
#include <sys/types.h>

typedef uint32_t remote_handle;
typedef uint64_t remote_handle64; //! used by multi domain modules
                                  //! 64 bit handles are translated to 32 bit values
                                  //! by the transport layer

typedef struct {
   void *pv;
   size_t nLen;
} remote_buf;

typedef union {
   remote_buf     buf;
   remote_handle    h;
   remote_handle64 h64; //! used by multi domain modules
} remote_arg;

/*Retrives method attribute from the scalars parameter*/
#define REMOTE_SCALARS_METHOD_ATTR(dwScalars)   (((dwScalars) >> 29) & 0x7)

/*Retrives method index from the scalars parameter*/
#define REMOTE_SCALARS_METHOD(dwScalars)        (((dwScalars) >> 24) & 0x1f)

/*Retrives number of input buffers from the scalars parameter*/
#define REMOTE_SCALARS_INBUFS(dwScalars)        (((dwScalars) >> 16) & 0x0ff)

/*Retrives number of output buffers from the scalars parameter*/
#define REMOTE_SCALARS_OUTBUFS(dwScalars)       (((dwScalars) >> 8) & 0x0ff)

/*Retrives number of input handles from the scalars parameter*/
#define REMOTE_SCALARS_INHANDLES(dwScalars)     (((dwScalars) >> 4) & 0x0f)

/*Retrives number of output handles from the scalars parameter*/
#define REMOTE_SCALARS_OUTHANDLES(dwScalars)    ((dwScalars) & 0x0f)

#define REMOTE_SCALARS_MAKEX(nAttr,nMethod,nIn,nOut,noIn,noOut) \
          ((((uint32_t)   (nAttr) &  0x7) << 29) | \
           (((uint32_t) (nMethod) & 0x1f) << 24) | \
           (((uint32_t)     (nIn) & 0xff) << 16) | \
           (((uint32_t)    (nOut) & 0xff) <<  8) | \
           (((uint32_t)    (noIn) & 0x0f) <<  4) | \
            ((uint32_t)   (noOut) & 0x0f))

#define REMOTE_SCALARS_MAKE(nMethod,nIn,nOut)  REMOTE_SCALARS_MAKEX(0,nMethod,nIn,nOut,0,0)

#define REMOTE_SCALARS_LENGTH(sc) (REMOTE_SCALARS_INBUFS(sc) +\
                                   REMOTE_SCALARS_OUTBUFS(sc) +\
                                   REMOTE_SCALARS_INHANDLES(sc) +\
                                   REMOTE_SCALARS_OUTHANDLES(sc))

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

#define NUM_DOMAINS 4

#ifndef DEFAULT_DOMAIN_ID
#define DEFAULT_DOMAIN_ID 0
#endif

#define ADSP_DOMAIN_ID    0
#define MDSP_DOMAIN_ID    1
#define SDSP_DOMAIN_ID    2
#define CDSP_DOMAIN_ID    3

#define ADSP_DOMAIN "&_dom=adsp"
#define MDSP_DOMAIN "&_dom=mdsp"
#define SDSP_DOMAIN "&_dom=sdsp"
#define CDSP_DOMAIN "&_dom=cdsp"

/* All other values are reserved */

/* opens a remote_handle "name"
 * returns 0 on success
 */
__QAIC_REMOTE_EXPORT int __QAIC_REMOTE(remote_handle_open)(const char* name, remote_handle *ph) __QAIC_REMOTE_ATTRIBUTE;
__QAIC_REMOTE_EXPORT int __QAIC_REMOTE(remote_handle64_open)(const char* name, remote_handle64 *ph) __QAIC_REMOTE_ATTRIBUTE;

/* invokes the remote handle
 * see retrive macro's on dwScalars format
 * pra, contains the arguments in the following order, inbufs, outbufs, inhandles, outhandles.
 * implementors should ignore and pass values asis that the transport doesn't understand.
 */
__QAIC_REMOTE_EXPORT int __QAIC_REMOTE(remote_handle_invoke)(remote_handle h, uint32_t dwScalars, remote_arg *pra) __QAIC_REMOTE_ATTRIBUTE;
__QAIC_REMOTE_EXPORT int __QAIC_REMOTE(remote_handle64_invoke)(remote_handle64 h, uint32_t dwScalars, remote_arg *pra) __QAIC_REMOTE_ATTRIBUTE;

/* closes the remote handle
 */
__QAIC_REMOTE_EXPORT int __QAIC_REMOTE(remote_handle_close)(remote_handle h) __QAIC_REMOTE_ATTRIBUTE;
__QAIC_REMOTE_EXPORT int __QAIC_REMOTE(remote_handle64_close)(remote_handle64 h) __QAIC_REMOTE_ATTRIBUTE;

/* map memory to the remote domain
 *
 * @param fd, fd assosciated with this memory
 * @param flags, flags to be used for the mapping
 * @param vaddrin, input address
 * @param size, size of buffer
 * @param vaddrout, output address
 * @retval, 0 on success
 */
__QAIC_REMOTE_EXPORT int __QAIC_REMOTE(remote_mmap)(int fd, uint32_t flags, uint32_t vaddrin, int size, uint32_t* vaddrout) __QAIC_REMOTE_ATTRIBUTE;

/* unmap memory from the remote domain
 *
 * @param vaddrout, remote address mapped
 * @param size, size to unmap.  Unmapping a range partially may  not be supported.
 * @retval, 0 on success, may fail if memory is still mapped
 */
__QAIC_REMOTE_EXPORT int __QAIC_REMOTE(remote_munmap)(uint32_t vaddrout, int size) __QAIC_REMOTE_ATTRIBUTE;

/* Register a file descriptor for a buffer.  This is only valid on
 * android with ION allocated memory.  Users of fastrpc should register
 * a buffer allocated with ION to enable sharing that buffer to the
 * dsp via the smmu.  Some versions of libadsprpc.so lack this
 * function, so users should set this symbol as weak.
 *
 * #pragma weak  remote_register_buf
 *
 * @param buf, virtual address of the buffer
 * @param size, size to of the buffer
 * @fd, the file descriptor, callers can use -1 to deregister.
 */
__QAIC_REMOTE_EXPORT void __QAIC_REMOTE(remote_register_buf)(void* buf, int size, int fd) __QAIC_REMOTE_ATTRIBUTE;

/*
 * This is the default mode for the driver.  While the driver is in parallel
 * mode it will try to invalidate output buffers after it transfers control
 * to the dsp.  This allows the invalidate operations to overlap with the
 * dsp processing the call.  This mode should be used when output buffers
 * are only read on the application processor and only written on the aDSP.
 */
#define REMOTE_MODE_PARALLEL  0

/*
 * When operating in SERIAL mode the driver will invalidate output buffers
 * before calling into the dsp.  This mode should be used when output
 * buffers have been written to somewhere besides the aDSP.
 */
#define REMOTE_MODE_SERIAL    1

/*
 * Internal transport prefix
 */
#define ITRANSPORT_PREFIX "'\":;./\\"

/*
 * Set the mode of operation.
 *
 * Some versions of libadsprpc.so lack this function, so users should set
 * this symbol as weak.
 *
 * #pragma weak  remote_set_mode
 *
 * @param mode, the mode
 * @retval, 0 on success
 */
__QAIC_REMOTE_EXPORT int __QAIC_REMOTE(remote_set_mode)(uint32_t mode) __QAIC_REMOTE_ATTRIBUTE;

/* Register a file descriptor.  This can be used when users do not have
 * a mapping to pass to the RPC layer.  The generated address is a mapping
 * with PROT_NONE, any access to this memory will fail, so it should only
 * be used as an ID to identify this file descriptor to the RPC layer.
 *
 * To deregister use remote_register_buf(addr, size, -1).
 *
 * #pragma weak  remote_register_fd
 *
 * @param fd, the file descriptor.
 * @param size, size to of the buffer
 * @retval, (void*)-1 on failure, address on success.
 * 
 */
__QAIC_REMOTE_EXPORT void *__QAIC_REMOTE(remote_register_fd)(int fd, int size) __QAIC_REMOTE_ATTRIBUTE;


#endif // REMOTE_H
