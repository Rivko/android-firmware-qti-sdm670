/*!
 * @file vpp_ip_hvx_core.c
 *
 * @cr
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.

 * @services
 */

#include <sys/types.h>
#include <sys/mman.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "AEEStdErr.h"

#define VPP_LOG_TAG     VPP_LOG_MODULE_HVX_CORE_TAG
#define VPP_LOG_MODULE  VPP_LOG_MODULE_HVX_CORE
#include "vpp_dbg.h"

#include "vpp.h"
#include "vpp_ion.h"
#include "vpp_reg.h"
#include "vpp_ip.h"
#include "vpp_utils.h"
#include "vpp_stats.h"
#include "vpp_ip_hvx_core.h"


/************************************************************************
 * Local definitions
 ***********************************************************************/
#define HVX_MIGRATE_CNT_MAX                 1000
#define HVX_VERSION_LEN                     128
#define HVX_LIB_NAME_LEN                    64
#define HVX_PROPERTY_LOGFLAGS               "vendor.media.vpp.hvx.logflags"
#define HVX_PROPERTY_STATSFLAGS             "vendor.media.vpp.hvx.statsflags"
#define HVX_PROPERTY_STATSPERIOD            "vendor.media.vpp.hvx.statsperiod"
#define HVX_HANDLE_INVALID                  ((remote_handle64)(-1))

enum {
    HVX_CORE_STAT_REGBUF,
    HVX_CORE_STAT_UNREGBUF,

    HVX_CORE_STAT_SVC_OPEN,
    HVX_CORE_STAT_SVC_CLOSE,
    HVX_CORE_STAT_SVC_INIT,
    HVX_CORE_STAT_SVC_DEINIT,
    HVX_CORE_STAT_SVC_SEND_CTX,
    HVX_CORE_STAT_SVC_RETRIEVE_CTX,
    HVX_CORE_STAT_SVC_PREPARE_CTX,
    HVX_CORE_STAT_SVC_GET_CTX,
    HVX_CORE_STAT_SVC_GET_DIAG_CTX,

    HVX_CORE_STAT_MIGRATE,
    HVX_CORE_STAT_ALLOC_CTX,
    HVX_CORE_STAT_FREE_CTX,
    HVX_CORE_STAT_CORE_OPEN,

    HVX_CORE_STAT_SET_DBG_LVL,
    HVX_CORE_STAT_GET_VER_INFO,
};

/************************************************************************
 * Local static variables
 ***********************************************************************/
static const t_StVppStatsConfig astHvxCoreStatsCfg[] = {
    VPP_PROF_DECL(HVX_CORE_STAT_REGBUF, 100, 0),
    VPP_PROF_DECL(HVX_CORE_STAT_UNREGBUF, 100, 0),

    VPP_PROF_DECL(HVX_CORE_STAT_SVC_OPEN, 1, 1),
    VPP_PROF_DECL(HVX_CORE_STAT_SVC_CLOSE, 1, 1),
    VPP_PROF_DECL(HVX_CORE_STAT_SVC_INIT, 1, 1),
    VPP_PROF_DECL(HVX_CORE_STAT_SVC_DEINIT, 1, 1),
    VPP_PROF_DECL(HVX_CORE_STAT_SVC_SEND_CTX, 1, 1),
    VPP_PROF_DECL(HVX_CORE_STAT_SVC_RETRIEVE_CTX, 1, 1),
    VPP_PROF_DECL(HVX_CORE_STAT_SVC_PREPARE_CTX, 1, 1),
    VPP_PROF_DECL(HVX_CORE_STAT_SVC_GET_CTX, 1, 1),
    VPP_PROF_DECL(HVX_CORE_STAT_SVC_GET_DIAG_CTX, 1, 1),

    VPP_PROF_DECL(HVX_CORE_STAT_MIGRATE, 1, 1),
    VPP_PROF_DECL(HVX_CORE_STAT_ALLOC_CTX, 1, 1),
    VPP_PROF_DECL(HVX_CORE_STAT_FREE_CTX, 1, 1),
    VPP_PROF_DECL(HVX_CORE_STAT_CORE_OPEN, 1, 1),

    VPP_PROF_DECL(HVX_CORE_STAT_SET_DBG_LVL, 1, 1),
    VPP_PROF_DECL(HVX_CORE_STAT_GET_VER_INFO, 1, 1),
};

static const uint32_t u32HvxCoreStatCnt = VPP_STATS_CNT(astHvxCoreStatsCfg);

/************************************************************************
 * Forward Declarations
 ************************************************************************/
extern void *remote_register_fd(int fd, int size);
#pragma weak remote_register_fd

extern void remote_register_buf(void* buf, int size, int fd);
#pragma weak remote_register_buf

/************************************************************************
 * Local Functions
 ***********************************************************************/

static void vVppIpHvxCore_SetDebugLevels(t_StVppIpHvxCoreCb *pstCb)
{
    int32_t s32Ret;
    vpp_svc_statlogs_cfg_t stDebugCfg;

    stDebugCfg.log_flag = pstCb->debug_cfg.u32LogFlags;
    stDebugCfg.stats_flag = pstCb->debug_cfg.u32StatsFlags;
    stDebugCfg.stats_period = pstCb->debug_cfg.u32StatsPeriod;

    s32Ret = vpp_svc_set_config(pstCb->hvx_handle, CFG_TYPE_STATSLOGS, (unsigned char*)&stDebugCfg,
                                sizeof(vpp_svc_statlogs_cfg_t));
    if (s32Ret != 0)
    {
        LOGE("unable to set stats config on firmware, s32Ret=%d", s32Ret);
    }
}

static void vVppIpHvxCore_ReadProperties(t_StVppIpHvxCoreCb *pstCb)
{
    VPP_RET_VOID_IF_NULL(pstCb);

    u32VppUtils_ReadPropertyU32(HVX_PROPERTY_LOGFLAGS,
                                &pstCb->debug_cfg.u32LogFlags, "0");
    u32VppUtils_ReadPropertyU32(HVX_PROPERTY_STATSFLAGS,
                                &pstCb->debug_cfg.u32StatsFlags, "0");
    u32VppUtils_ReadPropertyU32(HVX_PROPERTY_STATSPERIOD,
                                &pstCb->debug_cfg.u32StatsPeriod, "0");

    LOGI("HVX debug levels: log_flag=0x%x, stats_flag=0x%x, stats_period=0x%x",
         pstCb->debug_cfg.u32LogFlags, pstCb->debug_cfg.u32StatsFlags,
         pstCb->debug_cfg.u32StatsPeriod);
}

static void vVppIpHvxCore_GetVersionInfo(t_StVppIpHvxCoreCb *pstCb)
{
    int32_t s32Ret, i;
    vpp_svc_cap_resource_list_t* pstCapRes;

    VPP_RET_VOID_IF_NULL(pstCb);

    if (pstCb->once.u32TsRead)
        return;
    pstCb->once.u32TsRead = VPP_TRUE;

    pstCapRes = pstCb->pstCapabilityResources;

    for (i = 0; i < pstCapRes->resourceLen; i++)
    {
        s32Ret = vpp_svc_get_buildts_id(pstCb->hvx_handle, pstCapRes->resource[i].vpp_func_id,
                                        (unsigned char *)pstCapRes->resource[i].build_ts,
                                        MAX_TS_LEN);
        if (s32Ret == AEE_SUCCESS)
        {
            LOGI("HVX func_id[%d] version: %s",
                 pstCapRes->resource[i].vpp_func_id,
                 pstCapRes->resource[i].build_ts);
        }
        else if (s32Ret == AEE_EREADONLY)
            LOGI("HVX in protected mode. Unable to get HVX version for func_id[%d]", i);
        else
            LOGE("Unable to get HVX version for func_id[%d], s32Ret=%d", i, s32Ret);
    }
}

static uint32_t u32VppIpHvxCore_HandleSessionMigration(t_StVppIpHvxCoreCb *pstCb)
{
    int rc;
    uint32_t u32Heap, i;
    uint32_t u32Ret = VPP_ERR_HW;

    VPP_RET_IF_NULL(pstCb, VPP_ERR_PARAM);

    VPP_IP_PROF_START(&pstCb->stBase, HVX_CORE_STAT_MIGRATE);

    if (!pstCb->stBase.bSecureSession)
        return VPP_OK;

    u32Heap = u32VppIon_GetHeapId(pstCb->stBase.pstCtx,
                                  pstCb->stBase.bSecureSession);

    for (i = 0; i < HVX_MIGRATE_CNT_MAX; i++)
    {
        rc = vpp_svc_migrate_pd(pstCb->hvx_handle, u32Heap);
        if (rc == AEE_SUCCESS)
        {
            u32Ret = VPP_OK;
            LOGI("vpp_svc_migrate_pd(), i=%u, rc=%d", i, rc);
            break;
        }
        else if (rc != AEE_EINTERRUPTED)
        {
            LOGE("ERROR: vpp_svc_migrate_pd(), rc=%d", rc);
            break;
        }
    }

    LOGE_IF(i == HVX_MIGRATE_CNT_MAX, "migrate count reached max, max=%u", i);

    VPP_IP_PROF_STOP(&pstCb->stBase, HVX_CORE_STAT_MIGRATE);
    return u32Ret;
}

static void vVppIpHvxCore_RegisterIon(t_StVppIonBuf *pstIon, void **ppv)
{
    if (!ppv)
    {
        LOGE("ppv null");
        return;
    }

    *ppv = MAP_FAILED;
    if (!pstIon)
    {
        LOGE("pstIon is null");
        return;
    }

    if (remote_register_fd)
    {
        *ppv = remote_register_fd(pstIon->fd_ion_mem, pstIon->len);
        if (*ppv == (void *)-1 || *ppv == NULL)
        {
            LOGE("unable to remote_register_fd, fd=%d, dummy_ptr=%p",
                 pstIon->fd_ion_mem, *ppv);
            *ppv = MAP_FAILED;
        }
        else
        {
            LOGI("registering buffer, fd=%d, sz=%u, new_ptr=%p",
                 pstIon->fd_ion_mem, pstIon->len, *ppv);
        }
    }
    else
        LOGE("unable to register buffer, fd=%d ptr=%p, sz=%u",
             pstIon->fd_ion_mem, pstIon->buf, pstIon->len);
}

static void vVppIpHvxCore_UnregisterIon(t_StVppIonBuf *pstIon, void *pv)
{
    if (!pstIon || !pv)
    {
        LOGE("NULL input: pstIon=%p, pv=%p", pstIon, pv);
        return;
    }

    if (remote_register_buf)
    {
        LOGI("unregistering buffer, secure=%u, fd=%d, ptr=%p, sz=%u",
             pstIon->bSecure, pstIon->fd_ion_mem, pv, pstIon->len);
        remote_register_buf(pv, pstIon->len, -1);
    }
    else
    {
        LOGI("unable to unregister buffer, fd=%d, ptr=%p, sz=%u",
             pstIon->fd_ion_mem, pstIon->buf, pstIon->len);
    }
}

static uint32_t u32VppIpHvxCore_AllocateContext(t_StVppIpHvxCoreCb *pstHvxCore,
                                                uint32_t u32ReqLen)
{
    uint32_t u32Ret = VPP_OK;
    t_StVppIonBuf *pstIonBuf;

    VPP_RET_IF_NULL(pstHvxCore, VPP_ERR_PARAM);

    pstIonBuf = &pstHvxCore->ctx.stIonCtxBuf;

    LOGD("allocating context buffer, sz=%u", u32ReqLen);

    uint32_t u32;
    VPP_IP_PROF_START(&pstHvxCore->stBase, HVX_CORE_STAT_ALLOC_CTX);
    u32 = u32VppIon_Alloc(pstHvxCore->stBase.pstCtx, u32ReqLen,
                          pstHvxCore->stBase.bSecureSession, pstIonBuf);
    VPP_IP_PROF_STOP(&pstHvxCore->stBase, HVX_CORE_STAT_ALLOC_CTX);
    if (u32 != VPP_OK)
    {
        LOGE("Error: context ion alloc failed, secure=%u, fd=%d, addr=%p, sz=%u",
             pstHvxCore->stBase.bSecureSession, pstIonBuf->fd_ion_mem,
             pstIonBuf->buf, u32ReqLen);

        return VPP_ERR_NO_MEM;
    }
    else
    {
        pstHvxCore->ctx.bAllocated = VPP_TRUE;
        pstHvxCore->ctx.u32AllocatedSize = u32ReqLen;
        pstHvxCore->ctx.u32Offset = 0;
    }

    LOGD("allocated ion mem fd=%d, addr=%p, sz=%u", pstIonBuf->fd_ion_mem,
         pstIonBuf->buf, u32ReqLen);

    vVppIpHvxCore_RegisterIon(pstIonBuf, (void *)&pstHvxCore->ctx.pvRemote);

    return u32Ret;

}

static uint32_t u32VppIpHvxCore_FreeContext(t_StVppIpHvxCoreCb *pstHvxCore)
{
    t_StVppIonBuf *pstIonBuf;

    if (!pstHvxCore)
        return VPP_ERR_PARAM;

    if (!pstHvxCore->ctx.bAllocated)
        return VPP_OK;

    pstIonBuf = &pstHvxCore->ctx.stIonCtxBuf;

    LOGD("freeing context buffer");

    vVppIpHvxCore_UnregisterIon(pstIonBuf, &pstHvxCore->ctx.pvRemote);

    VPP_IP_PROF_START(&pstHvxCore->stBase, HVX_CORE_STAT_FREE_CTX);
    u32VppIon_Free(pstHvxCore->stBase.pstCtx, pstIonBuf);
    VPP_IP_PROF_STOP(&pstHvxCore->stBase, HVX_CORE_STAT_FREE_CTX);

    pstHvxCore->ctx.bAllocated = VPP_FALSE;
    pstHvxCore->ctx.u32AllocatedSize = 0;

    return VPP_OK;
}

/************************************************************************
 * Global Functions
 ***********************************************************************/

vpp_svc_field_fmt_t eVppIpHvxCore_SvcFieldFormatGet(t_EVppBufType eBufType)
{
    vpp_svc_field_fmt_t eFmt;
    switch (eBufType)
    {
        case eVppBufType_Interleaved_TFF:
            eFmt = FIELD_FMT_INTERLEAVED_TFF;
            break;

        case eVppBufType_Interleaved_BFF:
            eFmt = FIELD_FMT_INTERLEAVED_BFF;
            break;
        case eVppBufType_Frame_TFF:
            eFmt = FIELD_FMT_TFF;
            break;

        case eVppBufType_Frame_BFF:
            eFmt = FIELD_FMT_BFF;
            break;

        case eVppBufType_Progressive:
            eFmt = FIELD_FMT_PROGRESSIVE;
            break;

        case eVppBufType_Max:
        default:
            LOGE("Requested invalid format: eBufType=%d", eBufType);
            eFmt = FIELD_FMT_PROGRESSIVE;
            break;
    }
    return eFmt;
}

vpp_svc_pixel_fmt_t eVppIpHvxCore_SvcPixelFormatGet(enum vpp_color_format fmt)
{
    switch (fmt)
    {
        case VPP_COLOR_FORMAT_NV12_VENUS:
            return PIXEL_FMT_NV12;
        case VPP_COLOR_FORMAT_NV21_VENUS:
            return PIXEL_FMT_NV21;
        case VPP_COLOR_FORMAT_P010:
            return PIXEL_FMT_P010;
        case VPP_COLOR_FORMAT_UBWC_NV12:
            return PIXEL_FMT_NV12_UBWC;
        case VPP_COLOR_FORMAT_UBWC_NV21:
            return PIXEL_FMT_NV21_UBWC;
        case VPP_COLOR_FORMAT_UBWC_TP10:
            return PIXEL_FMT_TP10_UBWC;
        default:
            LOGE("unsupported VPP color format: %u, returning default HVX format %u",
                 fmt, PIXEL_FMT_NV12);
            return PIXEL_FMT_NV12;
    }
}

inline void *hvx_alloc_int(uint32_t sz, const char *str)
{
    LOGI("-- allocating %d bytes for: %s", sz, str);
    return memalign(128, sz);
}

void vVppIpHvxCore_RegisterBuffer(t_StVppIpHvxCoreCb *pstHvxCore, t_StVppMemBuf *pstIntMemBuf)
{
    if (!pstHvxCore || !pstHvxCore->stBase.pstCtx || !pstIntMemBuf)
        return;

    if (VPP_FLAG_IS_SET(pstIntMemBuf->u32IntBufFlags, VPP_BUF_FLAG_HVX_REGISTERED))
        return;

    // In a secure session, buffer management logic can not map this into
    // virtual address space for us, but the interface to HVX requires that the
    // buffer have a virtual address. Thus, we need to ask the adsp driver to
    // give us a virtual address to use. We will replace the vaddr here in the
    // buffer, and when we do the unregister call, we will replace it with
    // NULL. Must take care here not to modify the u32MappedLen or anything
    // like that.
    if (pstIntMemBuf->eMapping == eVppBuf_Unmapped)
    {
        void *pv = NULL;
        if (remote_register_fd)
        {
            VPP_IP_PROF_START(&pstHvxCore->stBase, HVX_CORE_STAT_REGBUF);
            pv = remote_register_fd(pstIntMemBuf->fd, pstIntMemBuf->u32AllocLen);
            VPP_IP_PROF_STOP(&pstHvxCore->stBase, HVX_CORE_STAT_REGBUF);

            if (pv == (void *)-1 || pv == NULL)
            {
                LOGE("unable to remote_register_fd, fd=%d, alloc_len=%u, "
                     "dummy_ptr=%p", pstIntMemBuf->fd, pstIntMemBuf->u32AllocLen, pv);
            }
            else
            {
                pstIntMemBuf->pvBase = pv;
                VPP_FLAG_SET(pstIntMemBuf->u32IntBufFlags, VPP_BUF_FLAG_HVX_REGISTERED);
                LOGI("remote_register_fd, fd=%d, sz=%u, new_ptr=%p",
                     pstIntMemBuf->fd, pstIntMemBuf->u32AllocLen, pv);
            }
        }
        else
            LOGE("unable to remote_register_fd, fd=%d, ptr=%p, sz=%u",
                 pstIntMemBuf->fd, pv, pstIntMemBuf->u32AllocLen);

    }
    else
    {
        if (remote_register_buf)
        {
            VPP_IP_PROF_START(&pstHvxCore->stBase, HVX_CORE_STAT_REGBUF);
            remote_register_buf(pstIntMemBuf->pvBase, pstIntMemBuf->u32MappedLen,
                                pstIntMemBuf->fd);
            VPP_IP_PROF_STOP(&pstHvxCore->stBase, HVX_CORE_STAT_REGBUF);

            VPP_FLAG_SET(pstIntMemBuf->u32IntBufFlags, VPP_BUF_FLAG_HVX_REGISTERED);

            LOGI("remote_register_buf, fd=%d, ptr=%p, sz=%u", pstIntMemBuf->fd,
                 pstIntMemBuf->pvBase, pstIntMemBuf->u32MappedLen);
        }
        else
            LOGE("unable to remote_register_buf, fd=%d, ptr=%p, sz=%u",
                 pstIntMemBuf->fd, pstIntMemBuf->pvBase, pstIntMemBuf->u32MappedLen);
    }
}

void vVppIpHvxCore_UnregisterBuffer(t_StVppIpHvxCoreCb *pstHvxCore, t_StVppMemBuf *pstIntMemBuf)
{
    if (!pstHvxCore || !pstHvxCore->stBase.pstCtx || !pstIntMemBuf)
        return;

    if (!VPP_FLAG_IS_SET(pstIntMemBuf->u32IntBufFlags, VPP_BUF_FLAG_HVX_REGISTERED))
        return;

    if (remote_register_buf)
    {
        uint32_t u32MappedLen = pstIntMemBuf->u32MappedLen;
        if (pstIntMemBuf->eMapping == eVppBuf_Unmapped)
        {
            u32MappedLen = pstIntMemBuf->u32AllocLen;
        }

        VPP_IP_PROF_START(&pstHvxCore->stBase, HVX_CORE_STAT_UNREGBUF);
        remote_register_buf(pstIntMemBuf->pvBase, u32MappedLen, -1);
        VPP_IP_PROF_STOP(&pstHvxCore->stBase, HVX_CORE_STAT_UNREGBUF);

        if (pstIntMemBuf->eMapping == eVppBuf_Unmapped)
        {
            pstIntMemBuf->pvBase = VPP_BUF_UNMAPPED_BUF_VAL;
        }

        VPP_FLAG_CLR(pstIntMemBuf->u32IntBufFlags, VPP_BUF_FLAG_HVX_REGISTERED);

        LOGI("unregistering buffer, fd=%d, ptr=%p, sz=%u", pstIntMemBuf->fd,
             pstIntMemBuf->pvBase, u32MappedLen);
    }
}

uint32_t bVppIpHvxCore_IsSecure(t_StVppIpHvxCoreCb *pstHvxCore)
{
    if (!pstHvxCore)
        return VPP_FALSE;

    return pstHvxCore->stBase.bSecureSession;
}

uint32_t u32VppIpHvxCore_SvcParamSetROI(t_StVppIpHvxCoreCb *pstHvxCore, uint32_t bEnable,
                                        uint32_t bCompute, uint32_t u32XStart, uint32_t u32YStart,
                                        uint32_t u32XEnd, uint32_t u32YEnd, uint32_t u32LineWidth,
                                        uint32_t u32LineY, uint32_t u32LineCr, uint32_t u32LineCb)
{
    VPP_RET_IF_NULL(pstHvxCore, VPP_ERR_PARAM);

    pstHvxCore->pstParams->gen_params.roi_en = bEnable;
    if (bCompute) {
        pstHvxCore->pstParams->gen_params.roi_x_tl = u32XStart;
        pstHvxCore->pstParams->gen_params.roi_y_tl = u32YStart;
        pstHvxCore->pstParams->gen_params.roi_x_br = u32XEnd;
        pstHvxCore->pstParams->gen_params.roi_y_br = u32YEnd;
        pstHvxCore->pstParams->gen_params.transit_reg_wid = u32LineWidth;
        pstHvxCore->pstParams->gen_params.transit_reg_yval = u32LineY;
        pstHvxCore->pstParams->gen_params.transit_reg_c1val = u32LineCr;
        pstHvxCore->pstParams->gen_params.transit_reg_c2val = u32LineCb;
    }
    return VPP_OK;
}

uint32_t u32VppIpHvxCore_SvcParamSetHeaderIdxAlgo(t_StVppIpHvxCoreCb *pstHvxCore, uint32_t u32Index,
                                                  uint32_t u32Algo)
{
    VPP_RET_IF_NULL(pstHvxCore, VPP_ERR_PARAM);

    (pstHvxCore->pstParams->header + u32Index)->vpp_func_id = u32Algo;
    (pstHvxCore->pstParams->header + u32Index)->process_flags = 1;

    return VPP_OK;
}

uint32_t u32VppIpHvxCore_SvcParamSetDataSize(t_StVppIpHvxCoreCb *pstHvxCore, uint32_t u32AlgoCnt,
                                             uint32_t u32DataLength)
{
    VPP_RET_IF_NULL(pstHvxCore, VPP_ERR_PARAM);

    pstHvxCore->pstParams->headerLen = u32AlgoCnt;
    pstHvxCore->pstParams->user_dataLen = u32DataLength;

    return VPP_OK;
}

vpp_svc_config_hdr_t *pstVppIpHvxCore_SvcParamGetHeaderIdxAddr(t_StVppIpHvxCoreCb *pstHvxCore,
                                                               uint32_t u32Index)
{
    if (!pstHvxCore)
        return NULL;

    return (vpp_svc_config_hdr_t*)(pstHvxCore->pstParams->header + u32Index);
}

void *pvVppIpHvxCore_SvcParamGetDataOffsetAddr(t_StVppIpHvxCoreCb *pstHvxCore, uint32_t u32Offset)
{
    if (!pstHvxCore)
        return NULL;

    return (void *)(pstHvxCore->pstParams->user_data + u32Offset);
}

uint32_t u32VppIpHvxCore_BufParamSetSize(t_StVppIpHvxCoreCb *pstHvxCore,
                                         uint32_t u32Width, uint32_t u32Height)
{
    VPP_RET_IF_NULL(pstHvxCore, VPP_ERR_PARAM);

    pstHvxCore->stHvxBufParams.u32InWidth = u32Width;
    pstHvxCore->stHvxBufParams.u32InHeight = u32Height;

    return VPP_OK;
}

uint32_t u32VppIpHvxCore_BufParamGetSize(t_StVppIpHvxCoreCb *pstHvxCore,
                                         uint32_t *pu32Width, uint32_t *pu32Height)
{
    VPP_RET_IF_NULL(pstHvxCore, VPP_ERR_PARAM);
    VPP_RET_IF_NULL(pu32Width, VPP_ERR_PARAM);
    VPP_RET_IF_NULL(pu32Height, VPP_ERR_PARAM);

    *pu32Width = pstHvxCore->stHvxBufParams.u32InWidth;
    *pu32Height = pstHvxCore->stHvxBufParams.u32InHeight;

    return VPP_OK;

}

uint32_t u32VppIpHvxCore_BufParamSetPixFmt(t_StVppIpHvxCoreCb *pstHvxCore,
                                           enum vpp_svc_pixel_fmt_t eFmt)
{
    VPP_RET_IF_NULL(pstHvxCore, VPP_ERR_PARAM);

    pstHvxCore->stHvxBufParams.ePixFmt = eFmt;

    return VPP_OK;
}

uint32_t u32VppIpHvxCore_BufParamGetPixFmt(t_StVppIpHvxCoreCb *pstHvxCore,
                                           enum vpp_svc_pixel_fmt_t *peFmt)
{
    VPP_RET_IF_NULL(pstHvxCore, VPP_ERR_PARAM);
    VPP_RET_IF_NULL(peFmt, VPP_ERR_PARAM);

    *peFmt = pstHvxCore->stHvxBufParams.ePixFmt;

    return VPP_OK;

}

uint32_t u32VppIpHvxCore_BufParamSetFldFmt(t_StVppIpHvxCoreCb *pstHvxCore,
                                           enum vpp_svc_field_fmt_t eFmt)
{
    VPP_RET_IF_NULL(pstHvxCore, VPP_ERR_PARAM);

    pstHvxCore->stHvxBufParams.eFieldFmt = eFmt;

    return VPP_OK;
}

uint32_t u32VppIpHvxCore_BufParamGetFldFmt(t_StVppIpHvxCoreCb *pstHvxCore,
                                           enum vpp_svc_field_fmt_t *peFmt)
{
    VPP_RET_IF_NULL(pstHvxCore, VPP_ERR_PARAM);
    VPP_RET_IF_NULL(peFmt, VPP_ERR_PARAM);

    *peFmt = pstHvxCore->stHvxBufParams.eFieldFmt;

    return VPP_OK;
}

uint32_t u32VppIpHvxCore_BufParamSetPlaneSize(t_StVppIpHvxCoreCb *pstHvxCore, uint32_t *pu32Size,
                                              uint32 u32Cnt)
{
    uint32_t i;

    VPP_RET_IF_NULL(pstHvxCore, VPP_ERR_PARAM);
    VPP_RET_IF_NULL(pu32Size, VPP_ERR_PARAM);

    if (u32Cnt > MAX_NUM_PLANES)
    {
        LOGE("u32Cnt=%u higher than MAX_NUM_PLANES=%u", u32Cnt, MAX_NUM_PLANES);
        return VPP_ERR_PARAM;
    }

    for (i = 0; i < MAX_NUM_PLANES; i++)
    {
        if (i < u32Cnt)
            pstHvxCore->stHvxBufParams.au32PlSizeBytes[i] = pu32Size[i];
        else
            pstHvxCore->stHvxBufParams.au32PlSizeBytes[i] = 0;
    }

    return VPP_OK;
}

uint32_t u32VppIpHvxCore_BufParamGetPlaneSize(t_StVppIpHvxCoreCb *pstHvxCore, uint32_t *pu32Size,
                                              uint32 u32Cnt)
{
    uint32_t i;

    VPP_RET_IF_NULL(pstHvxCore, VPP_ERR_PARAM);
    VPP_RET_IF_NULL(pu32Size, VPP_ERR_PARAM);

    if (u32Cnt > MAX_NUM_PLANES)
    {
        LOGE("u32Cnt=%u higher than MAX_NUM_PLANES=%u", u32Cnt, MAX_NUM_PLANES);
        return VPP_ERR_PARAM;
    }

    for (i = 0; i < u32Cnt; i++)
    {
        pu32Size[i] = pstHvxCore->stHvxBufParams.au32PlSizeBytes[i];
    }

    return VPP_OK;
}

uint32_t u32VppIpHvxCore_BufParamGetPlaneTotalSize(t_StVppIpHvxCoreCb *pstHvxCore,
                                                   uint32_t *pu32Size)
{
    uint32_t i;

    VPP_RET_IF_NULL(pstHvxCore, VPP_ERR_PARAM);
    VPP_RET_IF_NULL(pu32Size, VPP_ERR_PARAM);

    *pu32Size = 0;
    for (i = 0; i < MAX_NUM_PLANES; i++)
    {
        *pu32Size += pstHvxCore->stHvxBufParams.au32PlSizeBytes[i];
    }

    return VPP_OK;
}


uint32_t u32VppIpHvxCore_BufParamSetPlaneStride(t_StVppIpHvxCoreCb *pstHvxCore,
                                                uint32_t *pu32Stride, uint32 u32Cnt)
{
    uint32_t i;

    VPP_RET_IF_NULL(pstHvxCore, VPP_ERR_PARAM);
    VPP_RET_IF_NULL(pu32Stride, VPP_ERR_PARAM);

    if (u32Cnt > MAX_NUM_PLANES)
    {
        LOGE("u32Cnt=%u higher than MAX_NUM_PLANES=%u", u32Cnt, MAX_NUM_PLANES);
        return VPP_ERR_PARAM;
    }

    for (i = 0; i < MAX_NUM_PLANES; i++)
    {
        if (i < u32Cnt)
            pstHvxCore->stHvxBufParams.au32PlStride[i] = pu32Stride[i];
        else
            pstHvxCore->stHvxBufParams.au32PlStride[i] = 0;
    }

    return VPP_OK;
}

uint32_t u32VppIpHvxCore_BufParamGetPlaneStride(t_StVppIpHvxCoreCb *pstHvxCore,
                                                uint32_t *pu32Stride, uint32 u32Cnt)
{
    uint32_t i;

    VPP_RET_IF_NULL(pstHvxCore, VPP_ERR_PARAM);
    VPP_RET_IF_NULL(pu32Stride, VPP_ERR_PARAM);

    if (u32Cnt > MAX_NUM_PLANES)
    {
        LOGE("u32Cnt=%u higher than MAX_NUM_PLANES=%u", u32Cnt, MAX_NUM_PLANES);
        return VPP_ERR_PARAM;
    }

    for (i = 0; i < u32Cnt; i++)
    {
        pu32Stride[i] = pstHvxCore->stHvxBufParams.au32PlStride[i];
    }

    return VPP_OK;
}

uint32_t u32VppIpHvxCore_BufParamInit(t_StVppIpHvxCoreCb *pstHvxCore, uint32_t u32Width,
                                      uint32_t u32Height, enum vpp_color_format eFmt)
{
    uint32_t u32Planes, i;
    uint32_t au32Size[MAX_NUM_PLANES], au32Stride[MAX_NUM_PLANES];
    vpp_svc_pixel_fmt_t pix_fmt = _32BIT_PLACEHOLDER_vpp_svc_pixel_fmt_t;

    VPP_RET_IF_NULL(pstHvxCore, VPP_ERR_PARAM);

    memset(au32Size, 0, sizeof(au32Size));
    memset(au32Stride, 0, sizeof(au32Stride));

    u32Planes = u32VppUtils_GetNumPlanes(eFmt);
    if (u32Planes > MAX_NUM_PLANES)
    {
        LOGE("u32VppUtils_GetNumPlanes returned %u planes for color format %u, but max is %u",
             u32Planes, eFmt, MAX_NUM_PLANES);
        return VPP_ERR;
    }

    for (i = 0; i < u32Planes; i++)
    {
        au32Stride[i] = u32VppUtils_CalcStrideForPlane(u32Width, eFmt, i);
        au32Size[i] = u32VppUtils_GetPlaneSize(u32Width, u32Height, eFmt, i);
        LOGI("Plane[%u]: Width=%u Height=%u ePixFmt=%u Stride=%u PlaneSize=%u",
             i, u32Width, u32Height, pix_fmt, au32Stride[i], au32Size[i]);
    }

    pix_fmt = eVppIpHvxCore_SvcPixelFormatGet(eFmt);
    u32VppIpHvxCore_BufParamSetSize(pstHvxCore, u32Width, u32Height);
    u32VppIpHvxCore_BufParamSetPixFmt(pstHvxCore, pix_fmt);
    u32VppIpHvxCore_BufParamSetPlaneSize(pstHvxCore, au32Size, u32Planes);
    u32VppIpHvxCore_BufParamSetPlaneStride(pstHvxCore, au32Stride, u32Planes);
    u32VppIpHvxCore_BufParamSetFldFmt(pstHvxCore, FIELD_FMT_PROGRESSIVE);

    return VPP_OK;
}

uint32_t u32VppIpHvxCore_BuffInInit(t_StVppIpHvxCoreCb *pstHvxCore, uint32_t u32NumBuffers)
{
    uint32_t u32PixDataAllocSize;
    uint32_t i;
    vpp_svc_frame_group_descriptor_t* pstBufferdataIn;

    VPP_RET_IF_NULL(pstHvxCore, VPP_ERR_PARAM);
    if (u32NumBuffers > MAX_BUFFS_IN_GROUP)
    {
        LOGE("Trying to init %u buffers but max is %u", u32NumBuffers, MAX_BUFFS_IN_GROUP);
        return VPP_ERR_PARAM;
    }

    pstBufferdataIn = pstHvxCore->pstBufferdataIn;
    u32PixDataAllocSize = sizeof(vpp_svc_raw_buffer_t) * u32NumBuffers;

    memset((void*)pstBufferdataIn, 0, sizeof(vpp_svc_frame_group_descriptor_t));

    pstBufferdataIn->pixel_data = (vpp_svc_raw_buffer_t*)HVX_ALLOC(u32PixDataAllocSize);
    if (!pstBufferdataIn->pixel_data)
        return VPP_ERR_NO_MEM;

    pstBufferdataIn->numbuffers = u32NumBuffers;
    pstBufferdataIn->pixel_dataLen = u32NumBuffers;

    for (i = 0; i < pstBufferdataIn->numbuffers; i++)
    {
        pstBufferdataIn->bufferattributes[i].index = i;
    }

    return VPP_OK;
}

void vVppIpHvxCore_BuffInTerm(t_StVppIpHvxCoreCb *pstHvxCore)
{
    if ((pstHvxCore) && (pstHvxCore->pstBufferdataIn))
    {
        if (pstHvxCore->pstBufferdataIn->pixel_data)
            free(pstHvxCore->pstBufferdataIn->pixel_data);

        pstHvxCore->pstBufferdataIn->pixel_data = NULL;
    }
}

uint32_t u32VppIpHvxCore_BuffInCompute(t_StVppIpHvxCoreCb *pstHvxCore)
{
    t_StHvxCoreBufParams *pstBufParams;
    vpp_svc_frame_group_descriptor_t* pstBufferdataIn;
    uint32_t u32FrameSize = 0;
    uint32_t au32Stride[MAX_NUM_PLANES];
    uint32_t au32Size[MAX_NUM_PLANES];
    uint32_t i, j;

    VPP_RET_IF_NULL(pstHvxCore, VPP_ERR_PARAM);

    memset(au32Size, 0, sizeof(au32Size));
    memset(au32Stride, 0, sizeof(au32Stride));
    pstBufParams = &pstHvxCore->stHvxBufParams;
    pstBufferdataIn = pstHvxCore->pstBufferdataIn;
    u32VppIpHvxCore_BufParamGetPlaneSize(pstHvxCore, au32Size, MAX_NUM_PLANES);
    u32VppIpHvxCore_BufParamGetPlaneTotalSize(pstHvxCore, &u32FrameSize);
    u32VppIpHvxCore_BufParamGetPlaneStride(pstHvxCore, au32Stride, MAX_NUM_PLANES);

    pstBufferdataIn->width = pstBufParams->u32InWidth;
    pstBufferdataIn->height = pstBufParams->u32InHeight;
    pstBufferdataIn->pixelformat = pstBufParams->ePixFmt;
    pstBufferdataIn->fieldformat = pstBufParams->eFieldFmt;

    for (i = 0; i < pstBufferdataIn->numbuffers; i++)
    {
        u32VppIpHvxCore_BufInSetAttrSize(pstHvxCore, i, au32Size, MAX_NUM_PLANES);
        u32VppIpHvxCore_BufInSetAttrStride(pstHvxCore, i, au32Stride, MAX_NUM_PLANES);
        u32VppIpHvxCore_BufInSetUserDataLen(pstHvxCore, i, u32FrameSize);

        LOGD("pstBufferdataIn[%u] len=%u", i, u32FrameSize);
        for (j = 0; j < MAX_NUM_PLANES; j++)
        {
            LOGD("Plane[%u] Stride=%u SizeBytes=%u", j, au32Stride[j], au32Size[j]);
        }
    }

    return VPP_OK;
}

uint32_t u32VppIpHvxCore_BufInSetSize(t_StVppIpHvxCoreCb *pstHvxCore,
                                         uint32_t u32Width, uint32_t u32Height)
{
    VPP_RET_IF_NULL(pstHvxCore, VPP_ERR_PARAM);

    pstHvxCore->pstBufferdataIn->width = u32Width;
    pstHvxCore->pstBufferdataIn->height = u32Height;

    return VPP_OK;
}

uint32_t u32VppIpHvxCore_BufInSetPixFmt(t_StVppIpHvxCoreCb *pstHvxCore,
                                        enum vpp_svc_pixel_fmt_t eFmt)
{
    VPP_RET_IF_NULL(pstHvxCore, VPP_ERR_PARAM);

    pstHvxCore->pstBufferdataIn->pixelformat = eFmt;

    return VPP_OK;
}

uint32_t u32VppIpHvxCore_BufInSetFldFmt(t_StVppIpHvxCoreCb *pstHvxCore,
                                        enum vpp_svc_field_fmt_t eFmt)
{
    VPP_RET_IF_NULL(pstHvxCore, VPP_ERR_PARAM);

    pstHvxCore->pstBufferdataIn->fieldformat = eFmt;

    return VPP_OK;
}

uint32_t u32VppIpHvxCore_BufInSetNumBuffers(t_StVppIpHvxCoreCb *pstHvxCore, uint32_t u32NumBuffers)
{
    VPP_RET_IF_NULL(pstHvxCore, VPP_ERR_PARAM);

    pstHvxCore->pstBufferdataIn->numbuffers = u32NumBuffers;
    pstHvxCore->pstBufferdataIn->pixel_dataLen = u32NumBuffers;

    return VPP_OK;
}

uint32_t u32VppIpHvxCore_BufInSetAttrSize(t_StVppIpHvxCoreCb *pstHvxCore, uint32_t u32Idx,
                                          uint32_t *pu32Size, uint32_t u32Cnt)
{
    uint32_t i;

    VPP_RET_IF_NULL(pstHvxCore, VPP_ERR_PARAM);
    VPP_RET_IF_NULL(pu32Size, VPP_ERR_PARAM);

    if (u32Idx >= pstHvxCore->pstBufferdataIn->numbuffers)
    {
        LOGE("u32Idx=%u but allocated=%u", u32Idx, pstHvxCore->pstBufferdataIn->numbuffers);
        return VPP_ERR_PARAM;
    }

    if (u32Cnt > MAX_NUM_PLANES)
    {
        LOGE("u32Cnt=%u higher than MAX_NUM_PLANES=%u", u32Cnt, MAX_NUM_PLANES);
        return VPP_ERR_PARAM;
    }

    for (i = 0; i < MAX_NUM_PLANES; i++)
    {
        if (i < u32Cnt)
            pstHvxCore->pstBufferdataIn->bufferattributes[u32Idx].plane_sizebytes[i] = pu32Size[i];
        else
            pstHvxCore->pstBufferdataIn->bufferattributes[u32Idx].plane_sizebytes[i] = 0;
    }

    return VPP_OK;
}

uint32_t u32VppIpHvxCore_BufInSetAttrStride(t_StVppIpHvxCoreCb *pstHvxCore, uint32_t u32Idx,
                                            uint32_t *pu32Stride, uint32_t u32Cnt)
{
    uint32_t i;

    VPP_RET_IF_NULL(pstHvxCore, VPP_ERR_PARAM);
    VPP_RET_IF_NULL(pu32Stride, VPP_ERR_PARAM);

    if (u32Idx >= pstHvxCore->pstBufferdataIn->numbuffers)
    {
        LOGE("u32Idx=%u but allocated=%u", u32Idx, pstHvxCore->pstBufferdataIn->numbuffers);
        return VPP_ERR_PARAM;
    }

    if (u32Cnt > MAX_NUM_PLANES)
    {
        LOGE("u32Cnt=%u higher than MAX_NUM_PLANES=%u", u32Cnt, MAX_NUM_PLANES);
        return VPP_ERR_PARAM;
    }

    for (i = 0; i < MAX_NUM_PLANES; i++)
    {
        if (i < u32Cnt)
            pstHvxCore->pstBufferdataIn->bufferattributes[u32Idx].plane_stride[i] = pu32Stride[i];
        else
            pstHvxCore->pstBufferdataIn->bufferattributes[u32Idx].plane_stride[i] = 0;
    }

    return VPP_OK;
}

uint32_t u32VppIpHvxCore_BufInSetAttrUbwc(t_StVppIpHvxCoreCb *pstHvxCore, uint32_t u32Idx,
                                          t_StVppUbwcStats *pstBufStats)
{
    vpp_svc_ubwc_stats_t *pstUbwcStats;

    VPP_RET_IF_NULL(pstHvxCore, VPP_ERR_PARAM);

    if (u32Idx >= pstHvxCore->pstBufferdataIn->numbuffers)
    {
        LOGE("u32Idx=%u but allocated=%u", u32Idx, pstHvxCore->pstBufferdataIn->numbuffers);
        return VPP_ERR_PARAM;
    }

    pstUbwcStats = &pstHvxCore->pstBufferdataIn->bufferattributes[u32Idx].ubwc_stats;

    if (!pstBufStats)
    {
        memset(pstUbwcStats, 0, sizeof(vpp_svc_ubwc_stats_t));
    }
    else
    {
        if (pstBufStats->eVersion != eVppUbwcStatVer_2p0)
            return VPP_ERR_PARAM;

        pstUbwcStats->valid = pstBufStats->bValid ? 1 : 0;
        pstUbwcStats->num_32b_comp = pstBufStats->stats.st2p0.u32Stat32;
        pstUbwcStats->num_64b_comp = pstBufStats->stats.st2p0.u32Stat64;
        pstUbwcStats->num_96b_comp = pstBufStats->stats.st2p0.u32Stat96;
        pstUbwcStats->num_128b_comp = pstBufStats->stats.st2p0.u32Stat128;
        pstUbwcStats->num_160b_comp = pstBufStats->stats.st2p0.u32Stat160;
        pstUbwcStats->num_192b_comp = pstBufStats->stats.st2p0.u32Stat192;
        pstUbwcStats->num_256b_comp = pstBufStats->stats.st2p0.u32Stat256;
    }

    return VPP_OK;
}

uint32_t u32VppIpHvxCore_BufInSetUserDataLen(t_StVppIpHvxCoreCb *pstHvxCore, uint32_t u32Idx,
                                             uint32_t u32Size)
{
    VPP_RET_IF_NULL(pstHvxCore, VPP_ERR_PARAM);

    if (u32Idx >= pstHvxCore->pstBufferdataIn->numbuffers)
    {
        LOGE("u32Idx=%u but allocated=%u", u32Idx, pstHvxCore->pstBufferdataIn->numbuffers);
        return VPP_ERR_PARAM;
    }

    pstHvxCore->pstBufferdataIn->pixel_data[u32Idx].user_dataLen = u32Size;

    return VPP_OK;
}

uint32_t u32VppIpHvxCore_BufInSetUserDataAddr(t_StVppIpHvxCoreCb *pstHvxCore, uint32_t u32Idx,
                                              void* vpAddr)
{
    VPP_RET_IF_NULL(pstHvxCore, VPP_ERR_PARAM);

    if (u32Idx >= pstHvxCore->pstBufferdataIn->numbuffers)
    {
        LOGE("u32Idx=%u but allocated=%u", u32Idx, pstHvxCore->pstBufferdataIn->numbuffers);
        return VPP_ERR_PARAM;
    }

    pstHvxCore->pstBufferdataIn->pixel_data[u32Idx].user_data = vpAddr;

    return VPP_OK;
}

uint32_t u32VppIpHvxCore_BuffOutInit(t_StVppIpHvxCoreCb *pstHvxCore, uint32_t u32NumBuffers)
{
    uint32_t u32PixDataAllocSize;
    vpp_svc_frame_group_descriptor_t* pstBufferdataOut;

    VPP_RET_IF_NULL(pstHvxCore, VPP_ERR_PARAM);
    if (u32NumBuffers > MAX_BUFFS_IN_GROUP)
    {
        LOGE("Trying to init %u buffers but max is %u", u32NumBuffers, MAX_BUFFS_IN_GROUP);
        return VPP_ERR_PARAM;
    }

    u32PixDataAllocSize = sizeof(vpp_svc_raw_buffer_t) * u32NumBuffers;
    pstBufferdataOut = pstHvxCore->pstBufferdataOut;

    memset((void*)pstBufferdataOut, 0, sizeof(vpp_svc_frame_group_descriptor_t));

    pstBufferdataOut->pixel_data = (vpp_svc_raw_buffer_t*)HVX_ALLOC(u32PixDataAllocSize);
    if (!pstBufferdataOut->pixel_data)
        return VPP_ERR_NO_MEM;

    pstBufferdataOut->numbuffers = u32NumBuffers;
    pstBufferdataOut->pixel_dataLen = u32NumBuffers;

    return VPP_OK;
}

void vVppIpHvxCore_BuffOutTerm(t_StVppIpHvxCoreCb *pstHvxCore)
{
    if ((pstHvxCore) && (pstHvxCore->pstBufferdataOut))
    {
        if (pstHvxCore->pstBufferdataOut->pixel_data)
            free(pstHvxCore->pstBufferdataOut->pixel_data);

        pstHvxCore->pstBufferdataOut->pixel_data = NULL;
    }
}

uint32_t u32VppIpHvxCore_BuffOutCompute(t_StVppIpHvxCoreCb *pstHvxCore)
{
    t_StHvxCoreBufParams *pstBufParams;
    vpp_svc_frame_group_descriptor_t* pstBufferdataOut;
    uint32_t u32FrameSize;
    uint32_t i;

    VPP_RET_IF_NULL(pstHvxCore, VPP_ERR_PARAM);

    pstBufParams = &pstHvxCore->stHvxBufParams;
    pstBufferdataOut = pstHvxCore->pstBufferdataOut;
    u32VppIpHvxCore_BufParamGetPlaneTotalSize(pstHvxCore, &u32FrameSize);

    for (i = 0; i < pstBufferdataOut->numbuffers; i++)
    {
        u32VppIpHvxCore_BufOutSetUserDataLen(pstHvxCore, i, u32FrameSize);
        LOGD("pstBufferdataOut[%d] len=%d", i, u32FrameSize);
    }

    return VPP_OK;
}

uint32_t u32VppIpHvxCore_BufOutGetAttrUbwc(t_StVppIpHvxCoreCb *pstHvxCore, uint32_t u32Idx,
                                           t_StVppUbwcStats *pstBufStats)
{
    vpp_svc_ubwc_stats_t *pstUbwcStats;

    VPP_RET_IF_NULL(pstHvxCore, VPP_ERR_PARAM);
    VPP_RET_IF_NULL(pstBufStats, VPP_ERR_PARAM);

    if (u32Idx >= pstHvxCore->pstBufferdataOut->numbuffers)
    {
        LOGE("u32Idx=%u but allocated=%u", u32Idx, pstHvxCore->pstBufferdataOut->numbuffers);
        return VPP_ERR_PARAM;
    }

    pstUbwcStats = &pstHvxCore->pstBufferdataOut->bufferattributes[u32Idx].ubwc_stats;

    pstBufStats->eVersion = eVppUbwcStatVer_2p0;
    pstBufStats->bValid = pstUbwcStats->valid ? VPP_TRUE : VPP_FALSE;
    pstBufStats->stats.st2p0.u32Stat32 = pstUbwcStats->num_32b_comp;
    pstBufStats->stats.st2p0.u32Stat64 = pstUbwcStats->num_64b_comp;
    pstBufStats->stats.st2p0.u32Stat96 = pstUbwcStats->num_96b_comp;
    pstBufStats->stats.st2p0.u32Stat128 = pstUbwcStats->num_128b_comp;
    pstBufStats->stats.st2p0.u32Stat160 = pstUbwcStats->num_160b_comp;
    pstBufStats->stats.st2p0.u32Stat192 = pstUbwcStats->num_192b_comp;
    pstBufStats->stats.st2p0.u32Stat256 = pstUbwcStats->num_256b_comp;

    return VPP_OK;
}

uint32_t u32VppIpHvxCore_BufOutSetNumBuffers(t_StVppIpHvxCoreCb *pstHvxCore, uint32_t u32NumBuffers)
{
    VPP_RET_IF_NULL(pstHvxCore, VPP_ERR_PARAM);

    pstHvxCore->pstBufferdataOut->numbuffers = u32NumBuffers;
    pstHvxCore->pstBufferdataOut->pixel_dataLen = u32NumBuffers;

    return VPP_OK;
}

uint32_t u32VppIpHvxCore_BufOutSetUserDataLen(t_StVppIpHvxCoreCb *pstHvxCore, uint32_t u32Idx,
                                              uint32_t u32Size)
{
    VPP_RET_IF_NULL(pstHvxCore, VPP_ERR_PARAM);

    if (u32Idx >= pstHvxCore->pstBufferdataOut->numbuffers)
    {
        LOGE("u32Idx=%u but allocated=%u", u32Idx, pstHvxCore->pstBufferdataOut->numbuffers);
        return VPP_ERR_PARAM;
    }

    pstHvxCore->pstBufferdataOut->pixel_data[u32Idx].user_dataLen = u32Size;

    return VPP_OK;
}

uint32_t u32VppIpHvxCore_BufOutSetUserDataAddr(t_StVppIpHvxCoreCb *pstHvxCore, uint32_t u32Idx,
                                               void* vpAddr)
{
    VPP_RET_IF_NULL(pstHvxCore, VPP_ERR_PARAM);

    if (u32Idx >= pstHvxCore->pstBufferdataOut->numbuffers)
    {
        LOGE("u32Idx=%u but allocated=%u", u32Idx, pstHvxCore->pstBufferdataOut->numbuffers);
        return VPP_ERR_PARAM;
    }

    pstHvxCore->pstBufferdataOut->pixel_data[u32Idx].user_data = vpAddr;

    return VPP_OK;
}

int VppIpHvxCore_Process(t_StVppIpHvxCoreCb *pstHvxCore)
{
    VPP_RET_IF_NULL(pstHvxCore, VPP_ERR_PARAM);

    return vpp_svc_process(pstHvxCore->hvx_handle, pstHvxCore->pstParams,
                           (const vpp_svc_frame_group_descriptor_t*)pstHvxCore->pstBufferdataIn,
                           pstHvxCore->pstBufferdataOut);
}

uint32_t u32VppIpHvxCore_AlgoInit(t_StVppIpHvxCoreCb *pstHvxCore, uint32_t u32Index,
                                  uint32_t u32Algo, const char *cpAlgoLibName)
{
    vpp_svc_cap_resource_list_t* pstCapabilityResources;

    VPP_RET_IF_NULL(pstHvxCore, VPP_ERR_PARAM);

    pstCapabilityResources = pstHvxCore->pstCapabilityResources;
    pstCapabilityResources->resource[u32Index].vpp_func_id = u32Algo;
    pstCapabilityResources->resource[u32Index].feature_flags = 1;
    if (cpAlgoLibName)
        strlcpy(pstCapabilityResources->resource[u32Index].name_loc, cpAlgoLibName,
                HVX_LIB_NAME_LEN);

    return VPP_OK;
}

uint32_t u32VppIpHvxCore_SendCtx(t_StVppIpHvxCoreCb *pstHvxCore)
{
    int rc;

    VPP_RET_IF_NULL(pstHvxCore, VPP_ERR_PARAM);

    VPP_IP_PROF_START(&pstHvxCore->stBase, HVX_CORE_STAT_SVC_SEND_CTX);
    rc = vpp_svc_send_ctx(pstHvxCore->hvx_handle,
                          pstHvxCore->ctx.stIonCtxBuf.fd_ion_mem,
                          pstHvxCore->ctx.u32Offset,
                          pstHvxCore->ctx.u32Length);
    VPP_IP_PROF_STOP(&pstHvxCore->stBase, HVX_CORE_STAT_SVC_SEND_CTX);

    if (rc != AEE_SUCCESS)
    {
        LOGE("Error: Send context failed, rc=%d", rc);
        return VPP_ERR_HW;
    }

    return VPP_OK;
}

uint32_t u32VppIpHvxCore_RetrieveCtx(t_StVppIpHvxCoreCb *pstHvxCore)
{
    int rc;

    VPP_RET_IF_NULL(pstHvxCore, VPP_ERR_PARAM);

    VPP_IP_PROF_START(&pstHvxCore->stBase, HVX_CORE_STAT_SVC_RETRIEVE_CTX);
    rc = vpp_svc_retrieve_ctx(pstHvxCore->hvx_handle);
    VPP_IP_PROF_STOP(&pstHvxCore->stBase, HVX_CORE_STAT_SVC_RETRIEVE_CTX);

    if (rc != AEE_SUCCESS)
    {
        LOGE("Error: Retrieve context failed, rc=%d", rc);
        return VPP_ERR_HW;
    }

    return VPP_OK;
}

uint32_t u32VppIpHvxCore_PrepareCtx(t_StVppIpHvxCoreCb *pstHvxCore)
{
    int rc;

    VPP_RET_IF_NULL(pstHvxCore, VPP_ERR_PARAM);

    VPP_IP_PROF_START(&pstHvxCore->stBase, HVX_CORE_STAT_SVC_PREPARE_CTX);

    rc = vpp_svc_prepare_ctx(pstHvxCore->hvx_handle,
                             pstHvxCore->ctx.u32FrameSizeBytes,
                             pstHvxCore->ctx.u32ProcessingFlags,
                             pstHvxCore->ctx.u32CtxSz,
                             pstHvxCore->ctx.u32DiagCtxSz);

    VPP_IP_PROF_STOP(&pstHvxCore->stBase, HVX_CORE_STAT_SVC_PREPARE_CTX);

    if (rc != AEE_SUCCESS)
    {
        LOGE("Error: context config failed, rc=%d", rc);
        return VPP_ERR_HW;
    }

    return VPP_OK;
}

uint32_t u32VppIpHvxCore_Open(t_StVppIpHvxCoreCb *pstHvxCore,
                              uint32_t vpp_processing_flags,
                              uint32_t framesize_bytes)
{
    int rc;
    uint32_t u32Ret = VPP_OK;
    uint32_t u32;
    uint32_t u32RequiredLength;
    uint32_t vpp_ctx_size = 0;
    const char *pcUri;

    vpp_svc_cap_resource_list_t* pstCapabilityResources;

    LOG_ENTER();

    if (!pstHvxCore || !pstHvxCore->stBase.pstCtx)
        return VPP_ERR_PARAM;

    pcUri = vpp_svc_URI CDSP_DOMAIN;
    if (pstHvxCore->stBase.bSecureSession)
        pcUri = vpp_svc_URI CDSP_DOMAIN VPP_HVX_SECURE_SESSION;

    VPP_IP_PROF_START(&pstHvxCore->stBase, HVX_CORE_STAT_CORE_OPEN);

    VPP_IP_PROF_START(&pstHvxCore->stBase, HVX_CORE_STAT_SVC_OPEN);
    rc = vpp_svc_open(pcUri, &pstHvxCore->hvx_handle);
    VPP_IP_PROF_STOP(&pstHvxCore->stBase, HVX_CORE_STAT_SVC_OPEN);

    if ((rc != AEE_SUCCESS) || (pstHvxCore->hvx_handle == HVX_HANDLE_INVALID))
    {
        LOGE("ERROR: vpp_svc_open failed! rc=%d, handle=0x%"PRIx64, rc, pstHvxCore->hvx_handle);
        u32Ret = VPP_ERR_HW;
        goto ERROR_SVC_OPEN;
    }
    LOGD("vpp_svc_open successful: handle=0x%"PRIx64, pstHvxCore->hvx_handle);

    VPP_IP_PROF_START(&pstHvxCore->stBase, HVX_CORE_STAT_SET_DBG_LVL);
    vVppIpHvxCore_SetDebugLevels(pstHvxCore);
    VPP_IP_PROF_STOP(&pstHvxCore->stBase, HVX_CORE_STAT_SET_DBG_LVL);

    LOGD("flags=0x%x", vpp_processing_flags);

    pstCapabilityResources = pstHvxCore->pstCapabilityResources;

    LOGI("calling vpp_svc_init");

    VPP_IP_PROF_START(&pstHvxCore->stBase, HVX_CORE_STAT_SVC_INIT);
    rc = vpp_svc_init(pstHvxCore->hvx_handle, pstCapabilityResources);
    VPP_IP_PROF_STOP(&pstHvxCore->stBase, HVX_CORE_STAT_SVC_INIT);
    if (rc != AEE_SUCCESS)
    {
        LOGE("vpp_svc_init() error, rc=%d", rc);
        u32Ret = VPP_ERR_HW;
        goto ERR_VPP_SVC_INIT;
    }
    LOGI("vpp_svc_init() successful");

    // Get the version information from the firmware. Must be called after vpp_svc_init()!
    VPP_IP_PROF_START(&pstHvxCore->stBase, HVX_CORE_STAT_GET_VER_INFO);
    vVppIpHvxCore_GetVersionInfo(pstHvxCore);
    VPP_IP_PROF_STOP(&pstHvxCore->stBase, HVX_CORE_STAT_GET_VER_INFO);

    // Calculate context size and allocate context
    VPP_IP_PROF_START(&pstHvxCore->stBase, HVX_CORE_STAT_SVC_GET_CTX);
    rc = vpp_svc_get_ctx_size(pstHvxCore->hvx_handle, vpp_processing_flags,
                              framesize_bytes, &vpp_ctx_size);
    VPP_IP_PROF_STOP(&pstHvxCore->stBase, HVX_CORE_STAT_SVC_GET_CTX);

    if (rc != AEE_SUCCESS)
    {
        LOGE("get_ctx_sz error, rc=%d ctx_sz=%u", rc, vpp_ctx_size);
        u32Ret = VPP_ERR_HW;
        goto ERR_GET_CTX_SZ;
    }

    u32RequiredLength = (vpp_ctx_size + 127) & (~127);

    LOGD("framesize_bytes=%d, vpp_ctx_size=%d, u32RequiredLength=%u",
         framesize_bytes, vpp_ctx_size, u32RequiredLength);

    // if needs to allocate ion buffer
    if (pstHvxCore->ctx.bAllocated &&
        pstHvxCore->ctx.u32AllocatedSize < u32RequiredLength)
    {
        LOGD("freeing previously allocated ctx (sz=%u) for new ctx (sz=%u)",
             pstHvxCore->ctx.u32AllocatedSize, u32RequiredLength);
        u32VppIpHvxCore_FreeContext(pstHvxCore);
    }

    if (!pstHvxCore->ctx.bAllocated)
    {
        u32 = u32VppIpHvxCore_AllocateContext(pstHvxCore, u32RequiredLength);
        if (u32 != VPP_OK)
        {
            LOGE("ERROR: AllocateContext failed, u32=%u", u32);
            u32Ret = VPP_ERR_NO_MEM;
            goto ERR_ALLOC;
        }
    }

    pstHvxCore->ctx.u32Length = u32RequiredLength;
    pstHvxCore->ctx.u32FrameSizeBytes = framesize_bytes;
    pstHvxCore->ctx.u32ProcessingFlags = vpp_processing_flags;
    pstHvxCore->ctx.u32CtxSz = vpp_ctx_size;
    pstHvxCore->ctx.u32DiagCtxSz = 0;

    //Send context
    u32 = u32VppIpHvxCore_SendCtx(pstHvxCore);
    if (u32 != VPP_OK)
    {
        LOGE("Error: context send failed, u32=%u", u32);
        u32Ret = VPP_ERR_HW;
        goto ERR_SEND_CTX;
    }

    u32Ret = u32VppIpHvxCore_HandleSessionMigration(pstHvxCore);
    if (u32Ret != VPP_OK)
    {
        LOGE("session migration failed, u32Ret=%u", u32Ret);
        u32Ret = VPP_ERR_HW;
        goto ERR_VPP_MIGRATE_SESSION;
    }

    //Prepare context for use
    u32 = u32VppIpHvxCore_PrepareCtx(pstHvxCore);
    if (u32 != VPP_OK)
    {
        LOGE("Error: context config failed, u32=%u", u32);
        u32Ret = VPP_ERR_HW;
        goto ERR_PREPARE_CTX;
    }

    VPP_IP_PROF_STOP(&pstHvxCore->stBase, HVX_CORE_STAT_CORE_OPEN);
    return VPP_OK;

ERR_PREPARE_CTX:
ERR_VPP_MIGRATE_SESSION:
    u32VppIpHvxCore_RetrieveCtx(pstHvxCore);

ERR_SEND_CTX:
    u32VppIpHvxCore_FreeContext(pstHvxCore);

ERR_ALLOC:
ERR_GET_CTX_SZ:
    VPP_IP_PROF_START(&pstHvxCore->stBase, HVX_CORE_STAT_SVC_DEINIT);
    rc = vpp_svc_deinit(pstHvxCore->hvx_handle, pstCapabilityResources);
    VPP_IP_PROF_STOP(&pstHvxCore->stBase, HVX_CORE_STAT_SVC_DEINIT);
    LOGE_IF(rc != AEE_SUCCESS, "vpp_svc_deinit() failed, rc=%d", rc);

ERR_VPP_SVC_INIT:
    rc = vpp_svc_close(pstHvxCore->hvx_handle);
    LOGE_IF(rc != AEE_SUCCESS, "ERROR: vpp_svc_close failed rc=%d", rc);

ERROR_SVC_OPEN:
    VPP_IP_PROF_STOP(&pstHvxCore->stBase, HVX_CORE_STAT_CORE_OPEN);

    return u32Ret;
}

uint32_t u32VppIpHvxCore_Close(t_StVppIpHvxCoreCb *pstHvxCore)
{
    uint32_t u32;
    uint32_t u32Ret = VPP_OK;
    int rc;
    vpp_svc_cap_resource_list_t* pstCapabilityResources;

    LOG_ENTER();

    if (!pstHvxCore || ! pstHvxCore->stBase.pstCtx)
        return VPP_ERR_PARAM;

    u32 = u32VppIpHvxCore_RetrieveCtx(pstHvxCore);
    if (u32 != VPP_OK)
    {
        LOGE("ERROR: unable to retreive context, u32=%u", u32);
        u32Ret = VPP_ERR_HW;
    }

    pstCapabilityResources = pstHvxCore->pstCapabilityResources;
    rc = vpp_svc_deinit(pstHvxCore->hvx_handle, pstCapabilityResources);
    if (rc != AEE_SUCCESS)
    {
        LOGE("Error: HVX close failed rc=%d", rc);
        u32Ret = VPP_ERR_HW;
    }

    VPP_IP_PROF_START(&pstHvxCore->stBase, HVX_CORE_STAT_SVC_CLOSE);
    rc = vpp_svc_close(pstHvxCore->hvx_handle);
    VPP_IP_PROF_STOP(&pstHvxCore->stBase, HVX_CORE_STAT_SVC_CLOSE);
    if (rc != AEE_SUCCESS)
    {
        LOGE("ERROR: vpp_svc_close failed rc=%d", rc);
        u32Ret = VPP_ERR_HW;
    }

    return u32Ret;
}

t_StVppIpHvxCoreCb *pstVppIpHvxCore_Init(t_StVppCtx *pstCtx, uint32_t u32Flags,
                                         uint32_t u32CtrlCnt, uint32_t u32UserDataSize)
{
    uint32_t u32;
    t_StVppCallback cbs;
    t_StVppIpHvxCoreCb *pstHvxCore = NULL;
    uint32_t u32Length;

    LOG_ENTER();

    VPP_RET_IF_NULL(pstCtx, NULL);

    memset(&cbs, 0, sizeof(t_StVppCallback));

    pstHvxCore = calloc(sizeof(t_StVppIpHvxCoreCb), 1);
    if (!pstHvxCore)
    {
        LOGE("calloc failed for hvx core");
        return NULL;
    }
    u32VppIp_SetBase(pstCtx, u32Flags, cbs, &pstHvxCore->stBase);

    u32 = VPP_IP_PROF_REGISTER(&pstHvxCore->stBase, astHvxCoreStatsCfg, u32HvxCoreStatCnt);
    LOGE_IF(u32 != VPP_OK, "ERROR: unable to register stats, u32=%u", u32);

    vVppIpHvxCore_ReadProperties(pstHvxCore);

    pstHvxCore->pstCapabilityResources = (vpp_svc_cap_resource_list_t *)
        HVX_ALLOC(sizeof(vpp_svc_cap_resource_list_t));
    if (pstHvxCore->pstCapabilityResources == NULL)
    {
        LOGE("Error: pstCapabilityResources alloc failed");
        goto ERROR_CAPABILITY;
    }

    pstHvxCore->pstCapabilityResources->resourceLen = u32CtrlCnt;
    u32Length = sizeof(vpp_svc_cap_resource_t) * u32CtrlCnt;
    pstHvxCore->pstCapabilityResources->resource =
        (vpp_svc_cap_resource_t*)HVX_ALLOC(u32Length);
    if (pstHvxCore->pstCapabilityResources->resource == NULL)
    {
        LOGE("Error: pstCapabilityResources->resource alloc faild");
        goto ERROR_CAPABILITY_RES;
    }

    pstHvxCore->pstBufferdataIn = (vpp_svc_frame_group_descriptor_t*)
        HVX_ALLOC(sizeof(vpp_svc_frame_group_descriptor_t));
    if (pstHvxCore->pstBufferdataIn == NULL)
    {
        LOGE("Error: pstBufferdataIn alloc failed");
        goto ERROR_BUFFER_IN;
    }

    pstHvxCore->pstBufferdataOut = (vpp_svc_frame_group_descriptor_t*)
        HVX_ALLOC(sizeof(vpp_svc_frame_group_descriptor_t));
    if (pstHvxCore->pstBufferdataOut == NULL)
    {
        LOGE("Error: pstBufferdataOut alloc failed");
        goto ERROR_BUFFER_OUT;
    }

    pstHvxCore->pstParams = (vpp_svc_params_t*)HVX_ALLOC(sizeof(vpp_svc_params_t));
    if (pstHvxCore->pstParams == NULL)
    {
        LOGE("Error: pstHvxCore->pstParams alloc failed");
        goto ERROR_PARAMS;
    }

    u32Length = sizeof(vpp_svc_config_hdr_t) * u32CtrlCnt;
    pstHvxCore->pstParams->header = (vpp_svc_config_hdr_t*)HVX_ALLOC(u32Length);
    if (pstHvxCore->pstParams->header == NULL)
    {
        LOGE("Error: pstParams->header alloc failed");
        goto ERROR_HEADER;
    }

    pstHvxCore->pstParams->user_data = (unsigned char*)HVX_ALLOC(u32UserDataSize);
    if (pstHvxCore->pstParams->user_data == NULL)
    {
        LOGE("Error: alloc pstParams->user_data failed");
        goto ERROR_USERDATA;
    }

    return pstHvxCore;

ERROR_USERDATA:
    if (pstHvxCore->pstParams->header)
        free(pstHvxCore->pstParams->header);

ERROR_HEADER:
    if (pstHvxCore->pstParams)
        free(pstHvxCore->pstParams);

ERROR_PARAMS:
    if (pstHvxCore->pstBufferdataOut)
        free(pstHvxCore->pstBufferdataOut);

ERROR_BUFFER_OUT:
    if (pstHvxCore->pstBufferdataIn)
        free(pstHvxCore->pstBufferdataIn);

ERROR_BUFFER_IN:
    if (pstHvxCore->pstCapabilityResources->resource)
        free(pstHvxCore->pstCapabilityResources->resource);

ERROR_CAPABILITY_RES:
    if (pstHvxCore->pstCapabilityResources)
        free(pstHvxCore->pstCapabilityResources);

ERROR_CAPABILITY:
    u32 = VPP_IP_PROF_UNREGISTER(&pstHvxCore->stBase);
    LOGE_IF(u32 != VPP_OK, "ERROR: unable to unregister stats, u32=%u", u32);

    if (pstHvxCore)
        free(pstHvxCore);

    return NULL;
}

void vVppIpHvxCore_Term(t_StVppIpHvxCoreCb *pstHvxCore)
{
    uint32_t u32;

    LOG_ENTER();

    VPP_RET_VOID_IF_NULL(pstHvxCore);

    u32 = u32VppIpHvxCore_FreeContext(pstHvxCore);
    LOGE_IF(u32 != VPP_OK, "ERROR: unable to free context, u32=%u", u32);

    u32 = VPP_IP_PROF_UNREGISTER(&pstHvxCore->stBase);
    LOGE_IF(u32 != VPP_OK, "ERROR: unable to unregister stats, u32=%u", u32);

    if (pstHvxCore->pstBufferdataOut)
    {
        free(pstHvxCore->pstBufferdataOut);
        pstHvxCore->pstBufferdataOut = NULL;
    }

    if (pstHvxCore->pstBufferdataIn)
    {
        free(pstHvxCore->pstBufferdataIn);
        pstHvxCore->pstBufferdataIn = NULL;
    }

    if (pstHvxCore->pstParams)
    {
        if (pstHvxCore->pstParams->user_data)
            free(pstHvxCore->pstParams->user_data);

        if (pstHvxCore->pstParams->header)
            free(pstHvxCore->pstParams->header);

        free(pstHvxCore->pstParams);
        pstHvxCore->pstParams = NULL;
    }

    if (pstHvxCore->pstCapabilityResources)
    {
        if (pstHvxCore->pstCapabilityResources->resource)
            free(pstHvxCore->pstCapabilityResources->resource);
        free(pstHvxCore->pstCapabilityResources);
        pstHvxCore->pstCapabilityResources = NULL;
    }

    free(pstHvxCore);
}
