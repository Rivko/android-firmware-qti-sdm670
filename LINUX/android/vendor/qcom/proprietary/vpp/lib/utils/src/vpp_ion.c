/*!
 * @file vpp_ion.c
 *
 * @cr
 * Copyright (c) 2015, 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * @services
 *
 * @description
 */

#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>
#include <pthread.h>
#include <linux/msm_ion.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#define VPP_LOG_TAG     VPP_LOG_MODULE_ION_TAG
#define VPP_LOG_MODULE  VPP_LOG_MODULE_ION
#include "vpp_dbg.h"

#include "vpp.h"
#include "vpp_def.h"
#include "vpp_ctx.h"

#include "vpp_ion.h"

/************************************************************************
 * Local definitions
 ***********************************************************************/
#define ION_BUF_ALIGN 4096
#define ION_BUF_ALIGN_SECURE ION_BUF_ALIGN

#define VPP_ION_SESSION_MAX 16
/************************************************************************
 * Local static variables
 ***********************************************************************/
typedef struct {
    int fd_dev_ion;
    pthread_mutex_t mutex;
    t_StVppCtx *apstCtx[VPP_ION_SESSION_MAX];
} t_StVppIonCb;

static t_StVppIonCb sStIonCb = {
    .fd_dev_ion = -1,
    .mutex = PTHREAD_MUTEX_INITIALIZER,
};

/************************************************************************
 * Forward Declarations
 ***********************************************************************/
/************************************************************************
 * Local functions
 ***********************************************************************/
static uint32_t u32VppIon_IsSessionInitialized(t_StVppCtx *pstCtx)
{
    uint32_t i;
    for (i = 0; i < VPP_ION_SESSION_MAX; i++)
    {
        if (sStIonCb.apstCtx[i] == pstCtx)
            return VPP_TRUE;
    }

    return VPP_FALSE;
}

/************************************************************************
 * Global functions
 ***********************************************************************/
uint32_t u32VppIon_Init(t_StVppCtx *pstCtx)
{
    uint32_t i, u32 = VPP_OK;
    uint32_t u32IdxFree = VPP_ION_SESSION_MAX;

    if (!pstCtx)
        return VPP_ERR_PARAM;

    if (u32VppIon_IsSessionInitialized(pstCtx))
    {
        LOGE("%s(), trying to reinitialize ion for ctx=%p", __func__, pstCtx);
    }

    pthread_mutex_lock(&sStIonCb.mutex);

    if (sStIonCb.fd_dev_ion < 0)
    {
        sStIonCb.fd_dev_ion = open("/dev/ion", O_RDONLY);
        if (sStIonCb.fd_dev_ion < 0)
        {
            LOGE("Unable to open /dev/ion, err=%s", strerror(errno));
            u32 = VPP_ERR_RESOURCES;
            goto vpp_ion_init_exit;
        }
        else
            LOGI("%s(), opened ion dev node, fd=%d", __func__, sStIonCb.fd_dev_ion);
    }

    for (i = 0; i < VPP_ION_SESSION_MAX; i++)
    {
        if (sStIonCb.apstCtx[i] == NULL)
        {
            u32IdxFree = i;
            continue;
        }

        if (sStIonCb.apstCtx[i] == pstCtx)
        {
            LOGE("%s(), found existing context=%p in slot=%u", __func__, pstCtx, i);
            goto vpp_ion_init_exit;
        }
    }

    if (i == VPP_ION_SESSION_MAX)
    {
        if (u32IdxFree < VPP_ION_SESSION_MAX)
        {
            sStIonCb.apstCtx[u32IdxFree] = pstCtx;
            LOGI("%s(), ctx=%p stored in slot=%u", __func__, pstCtx, u32IdxFree);
        }
        else
        {
            LOGE("%s(), ion dev opened, but out of context slots", __func__);
            u32 = VPP_ERR_RESOURCES;
        }
    }

vpp_ion_init_exit:

    pthread_mutex_unlock(&sStIonCb.mutex);

    return u32;
}

uint32_t u32VppIon_Term(t_StVppCtx *pstCtx)
{
    uint32_t i, u32Cnt = 0;

    if (!pstCtx)
        return VPP_ERR_PARAM;

    if (!u32VppIon_IsSessionInitialized(pstCtx))
    {
        LOGE("%s(), trying to term uninitialized session, pstCtx=%p", __func__,
             pstCtx);
        return VPP_ERR_PARAM;
    }

    pthread_mutex_lock(&sStIonCb.mutex);

    for (i = 0; i < VPP_ION_SESSION_MAX; i++)
    {
        if (sStIonCb.apstCtx[i] != NULL)
        {
            if (sStIonCb.apstCtx[i] != pstCtx)
                u32Cnt += 1;
            else
            {
                LOGI("%s(), purging ctx=%p stored in slot=%u", __func__, pstCtx, i);
                sStIonCb.apstCtx[i] = NULL;
            }
        }
    }

    if (u32Cnt == 0)
    {
        close(sStIonCb.fd_dev_ion);
        LOGI("closed ion dev node, fd=%d", sStIonCb.fd_dev_ion);
        sStIonCb.fd_dev_ion = -1;
    }

    pthread_mutex_unlock(&sStIonCb.mutex);

    return VPP_OK;
}

uint32_t u32VppIon_Alloc(t_StVppCtx *pstCtx, uint32_t u32Len, uint32_t u32Secure,
                         t_StVppIonBuf *buf)
{
    int rc = 0;
    struct ion_allocation_data alloc_data;
    struct ion_fd_data fd_data;
    struct ion_handle_data hdl_data;

    if (!pstCtx)
        return VPP_ERR_PARAM;

    if (!buf)
        return VPP_ERR_PARAM;

    if (!u32VppIon_IsSessionInitialized(pstCtx))
    {
        LOGE("%s() called with uninitialized session", __func__);
        return VPP_ERR_STATE;
    }

    LOG_ENTER_ARGS("pstCtx=%p, fd_dev_ion=%d, u32Len=%u, bSecure=%u, buf=%p",
                   pstCtx, sStIonCb.fd_dev_ion, u32Len, u32Secure, buf);

    memset(buf, 0, sizeof(t_StVppIonBuf));
    memset(&alloc_data, 0, sizeof(struct ion_allocation_data));
    memset(&fd_data, 0, sizeof(struct ion_fd_data));
    memset(&hdl_data, 0, sizeof(struct ion_handle_data));

    // Allocate ion memory
    alloc_data.len = (u32Len + (ION_BUF_ALIGN - 1)) & ~(ION_BUF_ALIGN - 1);

    if (u32Secure)
    {
        alloc_data.align = ION_BUF_ALIGN_SECURE;
        alloc_data.flags = ION_FLAG_SECURE | ION_FLAG_CP_PIXEL;
        alloc_data.heap_id_mask = ION_HEAP(ION_SECURE_HEAP_ID);
    }
    else
    {
        alloc_data.align = ION_BUF_ALIGN;
        alloc_data.flags = 0;
        alloc_data.heap_id_mask = ION_HEAP(ION_SYSTEM_HEAP_ID);
    }


    if ((size_t)u32Len != alloc_data.len)
    {
        LOGD("requested len=%u, aligned to=%u", u32Len, (uint32_t)alloc_data.len);
    }

    rc = ioctl(sStIonCb.fd_dev_ion, ION_IOC_ALLOC, &alloc_data);
    if (rc < 0)
    {
        LOGE("ion: ION_IOC_ALLOC failed, rc=%d, err=%s", rc, strerror(rc));
        goto ION_ALLOC_FAILED;
    }

    // Get a file descriptor for the buffer
    fd_data.handle = alloc_data.handle;
    rc = ioctl(sStIonCb.fd_dev_ion, ION_IOC_SHARE, &fd_data);
    if (rc < 0)
    {
        LOGE("ion: ION_IOC_SHARE failed: %s", strerror(errno));
        goto ION_MAP_FAILED;
    }

    buf->buf = MAP_FAILED;
    if (!u32Secure)
    {
        // Map the ion buffer into virtual address space
        int prot_flags = PROT_READ | PROT_WRITE;
        buf->buf = mmap(NULL, alloc_data.len, prot_flags, MAP_SHARED, fd_data.fd, 0);
        if (buf->buf == MAP_FAILED)
        {
            LOGE("mmap failed: %s, errno=%d", strerror(errno), errno);
            goto ION_MAP_FAILED;
        }
    }

    buf->fd_ion_mem = fd_data.fd;
    buf->handle = alloc_data.handle;
    buf->len = alloc_data.len;
    buf->bSecure = u32Secure;

    return VPP_OK;

ION_MAP_FAILED:
    hdl_data.handle = alloc_data.handle;
    ioctl(sStIonCb.fd_dev_ion, ION_IOC_FREE, &hdl_data);

ION_ALLOC_FAILED:
    return VPP_ERR;
}

uint32_t u32VppIon_Free(t_StVppCtx *pstCtx, t_StVppIonBuf *pstBuf)
{
    int rc = 0;
    struct ion_handle_data handle_data;

    if (!pstCtx)
        return VPP_ERR_PARAM;

    if (!pstBuf)
        return VPP_ERR_PARAM;

    if (!u32VppIon_IsSessionInitialized(pstCtx))
    {
        LOGE("%s() called with uninitialized session", __func__);
        return VPP_ERR_STATE;
    }

    memset(&handle_data, 0, sizeof(struct ion_handle_data));

    if (!pstBuf->bSecure)
    {
        rc = munmap(pstBuf->buf, pstBuf->len);
        if (rc < 0)
        {
            LOGE("munmap: error, rc=%d", rc);
        }
    }

    if (pstBuf->fd_ion_mem)
    {
        close(pstBuf->fd_ion_mem);
    }

    if (sStIonCb.fd_dev_ion)
    {
        handle_data.handle = pstBuf->handle;
        ioctl(sStIonCb.fd_dev_ion, ION_IOC_FREE, &handle_data);
    }
    memset(pstBuf, 0, sizeof(t_StVppIonBuf));

    return VPP_OK;
}

uint32_t u32VppIon_GetHeapId(t_StVppCtx *pstCtx, uint32_t bSecure)
{
    VPP_UNUSED(pstCtx);
    if (bSecure)
        return ION_SECURE_HEAP_ID;
    else
        return ION_SYSTEM_HEAP_ID;
}

#if 0
int32_t dvp_ion_cache_ops(t_StVppIonBuf *buf, unsigned int cmd)
{
    struct ion_flush_data flush_data;
    struct ion_custom_data custom_data;
    int rc;

    CDBG_ENTER();

    if (!buf)
        return VPP_ERR_PARAM;

    if (!buf->fd_dev_ion)
    {
        // HACK: We can occasionally run into this case when the buffer is
        // allocated by a third party (for example OMX IL client) and we are
        // not passed the ion device fd.  However, it is required if we are to
        // correctly perform the cache operation.  If we do not get this
        // though, we can not return an error, since this would cause the
        // mm-camera-interface to not return a notify callback.  Unfortunately,
        // we don't have a mechanism to tell the mm-camera-interface that we
        // were unable to successfully perform the cache operation, so we just
        // return DVP_SERVICE_OK for now.

        LOGE("fd_dev_ion is null");

        return VPP_OK;
    }

    memset(&flush_data, 0, sizeof(struct ion_flush_data));
    memset(&custom_data, 0, sizeof(struct ion_custom_data));
    flush_data.vaddr = buf->buf;
    flush_data.fd = buf->fd_ion_mem;
    flush_data.handle = buf->handle;
    flush_data.length = buf->len;

    custom_data.cmd = cmd;
    custom_data.arg = (unsigned long)&flush_data;

    rc = ioctl(buf->fd_dev_ion, ION_IOC_CUSTOM, &custom_data);
    if (rc < 0)
    {
        CDBG_ERROR("cache ops failed: cmd=%d, rc=%d", cmd, rc);
        CDBG_EXIT_RET(DVP_SERVICE_ERR);
    }

    CDBG_EXIT_RET(DVP_SERVICE_OK);
}
#endif
