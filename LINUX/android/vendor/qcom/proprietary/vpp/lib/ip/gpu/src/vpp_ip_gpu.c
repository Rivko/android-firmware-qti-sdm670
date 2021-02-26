/*!
 * @file vpp_ip_gpu.c
 *
 * @cr
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.

 * @services
 */

#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#ifndef NO_COMPILE_MMSW
#include <unistd.h>
#endif

#define VPP_LOG_TAG     VPP_LOG_MODULE_GPU_TAG
#define VPP_LOG_MODULE  VPP_LOG_MODULE_GPU
#include "vpp_dbg.h"

#include "vpp.h"
#include "vpp_reg.h"
#include "vpp_queue.h"
#include "vpp_ip.h"
#include "vpp_ip_gpu.h"
#include "vpp_utils.h"
#include "vpp_stats.h"

#include "mmsw_framedescriptor.h"
#include "mmsw_detail_enhancement.h"

/************************************************************************
 * Local definitions
 ***********************************************************************/

// #define GPU_LOG_FRAMEDESC
// #define GPU_LOG_FRAME

#define GPU_CMD_Q_SZ                        30
#define GPU_PORT_BUF_Q_SZ                   VPP_INTERNAL_BUF_MAX

#define GPU_DUMP_NM_LEN                     256
#define GPU_DUMP_BUF_IN_NM                  "gpu_%p_in.yuv"
#define GPU_DUMP_BUF_OUT_NM                 "gpu_%p_out.yuv"

#define GPU_CL_NM_LEN                       256
#define GPU_CL_PROGRAM_NM_DEFAULT           "/system/etc/firmware/DE.o." LOCAL_BOARD_PLATFORM
#define GPU_CL_PROGRAM_NM_8976              "/system/etc/firmware/DE.o.msm8976"

#define GPU_CADE_LEVEL_DEFAULT              1.0
#define GPU_CADE_LEVEL_MIN                  0.0
#define GPU_CADE_LEVEL_MAX                  4.0

#define GPU_CADE_LUMA_CSC_DEFAULT           1.0
#define GPU_CADE_LUMA_CSC_MIN               0.0
#define GPU_CADE_LUMA_CSC_MAX               2.0

#define GPU_CADE_CHROMA_CSC_DEFAULT         1.0
#define GPU_CADE_CHROMA_CSC_MIN             0.0
#define GPU_CADE_CHROMA_CSC_MAX             2.0

#define GPU_CADE_PROCESS_PERCENT_DEFAULT    100.0
#define GPU_CADE_PROCESS_PERCENT_MIN        0.0
#define GPU_CADE_PROCESS_PERCENT_MAX        100.0

#define GPU_ALGO_CADE                       (1 << 0)
#define GPU_ALGO_CADE_DEMO                  (1 << 1)
#define GPU_CTRL_DE_LVL                     (1 << 16)
#define GPU_CTRL_DE_LUMA_CSC                (1 << 17)
#define GPU_CTRL_DE_CHROMA_CSC              (1 << 18)
#define GPU_CTRL_DE_PROC_PERCENT            (1 << 19)

#define GPU_PORT_PARAM_WIDTH_MIN            96
#define GPU_PORT_PARAM_HEIGHT_MIN           64

#define GPU_CB_GET(ctx) (ctx ? (t_StVppIpGpuCb *)ctx : NULL)

#define GPU_STATS(cb, stat)                 (cb)->stats.u32##stat++

#ifdef GPU_LOG_FRAMEDESC
#define GPU_DUMP_FRAME_DESC(framedesc, str) vVppIpGpu_DumpFrameDesc(framedesc, str)
#else
#define GPU_DUMP_FRAME_DESC(framedesc, str)
#endif

//#ifdef GPU_LOG_FRAME
#define GPU_DUMP_FRAME(cb, buf, port)       vVppIpGpu_DumpFrame(cb, buf, port)
//#else
//#define GPU_DUMP_FRAME(cb, buf, port)
//#endif

typedef struct {
    mmsw_detail_enhancement_ctx_t ctx;

    // mmsw_detail_enhancement_param_quality
    mmsw_detail_enhancement_quality_option_t quality_option;

    // mmsw_detail_enhancement_param_de_level
    float de_level;

    // mmsw_detail_enhancement_param_luma_csc_level
    float luma_csc_lvl;

    // mmsw_detail_enhancement_param_chroma_csc_level
    float chroma_csc_lvl;

    // mmsw_detail_enhancement_param_process_width_percent
    float process_percent_left;
    float process_percent_right;
    float process_percent_top;
    float process_percent_bottom;

} t_StVppIpGpuDrvCadeCb;

typedef struct {
    struct hqv_ctrl_cade cade;
    struct hqv_ctrl_global_demo demo;
    uint32_t u32EnableMask;
    uint32_t u32LoadMask;
    uint32_t u32ComputeMask;
} t_StVppIpGpuCfg;

typedef struct {
    t_StVppQueue stQ;
    t_StVppBuf *apBufNode[GPU_PORT_BUF_Q_SZ];
} t_StVppIpGpuPort;

typedef struct {
    t_StVppIpBase stBase;

    uint32_t u32InternalFlags;

    sem_t sem;
    pthread_t thread;
    pthread_cond_t cond;
    pthread_mutex_t mutex;
    uint32_t u32WorkerErr;

    t_StVppIpGpuPort stInput;
    t_StVppIpGpuPort stOutput;

    t_StVppQueue stCmdQ;
    t_StVppIpCmd astCmdNode[GPU_CMD_Q_SZ];

    uint32_t eState;

    t_StVppIpGpuCfg stCfg;
    uint32_t u32InputMin;
    uint32_t u32OutputMin;
    uint32_t u32BypassCnt;
    uint32_t u32InterlacedCnt;

    t_StVppIpGpuDrvCadeCb stDrvCade;

    struct {
        STAT_DECL(InQCnt);          // Number of frames queued to input
        STAT_DECL(OutQCnt);         // Number of frames queued to output
        STAT_DECL(InProcCnt);       // Number of input frames processed
        STAT_DECL(OutProcCnt);      // Number of output frames processed into
        STAT_DECL(IBDCnt);          // Number of IBD issued
        STAT_DECL(OBDCnt);          // Number of OBD issued
        STAT_DECL(BypassBuf);       // Bypass buffer due to flag
        STAT_DECL(BypassIntBuf);    // Bypass buffer due to interlaced
    } stats;

    struct {
        uint32_t u32OpenRet;
        uint32_t u32CloseRet;
    } async_res;
} t_StVppIpGpuCb;

enum {
    GPU_STAT_PROC,
};

/************************************************************************
 * Local static variables
 ***********************************************************************/
static const t_StVppStatsConfig astGpuStatsCfg[] = {
    VPP_PROF_DECL(GPU_STAT_PROC, 100, 1),
};

static const uint32_t u32GpuStatCnt = VPP_STATS_CNT(astGpuStatsCfg);

/************************************************************************
 * Forward Declarations
 ************************************************************************/

/************************************************************************
 * Local Functions
 ***********************************************************************/
mmsw_frame_format_t eVppIpGpu_MmswColorFormatGet(enum vpp_color_format fmt)
{
    if (fmt == VPP_COLOR_FORMAT_NV12_VENUS)
        return mmsw_frame_format_nv12adreno;

    else if (fmt == VPP_COLOR_FORMAT_NV21_VENUS)
        return mmsw_frame_format_nv21adreno;

    else
    {
        LOGE("unsupported color format: %u", fmt);
        return mmsw_frame_format_nv12adreno;
    }
}

void vVppIpGpu_DumpFrame(t_StVppIpGpuCb *pstCb, t_StVppBuf *pstBuf,
                         enum vpp_port port)
{
    char cPath[GPU_DUMP_NM_LEN];
    struct vpp_port_param *pstParam;

    if (pstCb->stBase.bSecureSession)
        return;

    if (port == VPP_PORT_INPUT)
    {
        snprintf(cPath, GPU_DUMP_NM_LEN, GPU_DUMP_BUF_IN_NM, pstCb);
        pstParam = &pstCb->stBase.stInputParam;
    }
    else
    {
        snprintf(cPath, GPU_DUMP_NM_LEN, GPU_DUMP_BUF_OUT_NM, pstCb);
        pstParam = &pstCb->stBase.stOutputParam;
    }

    u32VppBuf_Dump(pstCb->stBase.pstCtx, pstBuf, *pstParam, cPath);
}

void vVppIpGpu_DumpFrameDesc(mmsw_frame_desc_t *fd, char *str)
{
    LOGI("%s, fd=%p, ud=%p, fmt=%u, w=%u, h=%u, stride=%u, "
         "fd=%d, base=%p, uvOff=%u, sz=%zu",
         str, fd, fd->userData, fd->format, fd->width, fd->height,
         fd->stride, fd->fd, fd->baseAddress,
         (uint32_t)fd->uvOffset, fd->baseSize);
}

static void vVppIpGpu_Compute(t_StVppIpGpuCb *pstCb)
{
    uint32_t bLocked = VPP_FALSE;
    float fLvl;

    if (pstCb->stCfg.u32ComputeMask)
    {
        pthread_mutex_lock(&pstCb->mutex);
        bLocked = VPP_TRUE;
    }

    if (pstCb->stCfg.u32ComputeMask & GPU_ALGO_CADE)
    {
        if (pstCb->stCfg.cade.mode == HQV_MODE_AUTO)
        {
            pstCb->stDrvCade.de_level = GPU_CADE_LEVEL_DEFAULT;
            pstCb->stDrvCade.luma_csc_lvl = GPU_CADE_LUMA_CSC_DEFAULT;
            pstCb->stDrvCade.chroma_csc_lvl = GPU_CADE_CHROMA_CSC_DEFAULT;
        }
        else if (pstCb->stCfg.cade.mode == HQV_MODE_MANUAL)
        {
            // CADE Level
            fLvl = fVppUtils_ScaleFloat(CADE_LEVEL_MIN, CADE_LEVEL_MAX, GPU_CADE_LEVEL_MIN,
                                        GPU_CADE_LEVEL_MAX,
                                        (float)pstCb->stCfg.cade.cade_level);
            pstCb->stDrvCade.de_level = fLvl;

            // CADE Luma Level
            fLvl = fVppUtils_ScaleFloat(CADE_CONTRAST_MIN, CADE_CONTRAST_MAX,
                                        GPU_CADE_LUMA_CSC_MIN, GPU_CADE_LUMA_CSC_MAX,
                                        (float)pstCb->stCfg.cade.contrast);
            pstCb->stDrvCade.luma_csc_lvl = fLvl;

            // CADE Chroma Level
            fLvl = fVppUtils_ScaleFloat(CADE_SATURATION_MIN, CADE_SATURATION_MAX,
                                        GPU_CADE_CHROMA_CSC_MIN, GPU_CADE_CHROMA_CSC_MAX,
                                        (float)pstCb->stCfg.cade.saturation);
            pstCb->stDrvCade.chroma_csc_lvl = fLvl;

        }

        pstCb->stCfg.u32LoadMask |= GPU_CTRL_DE_LVL;
        pstCb->stCfg.u32LoadMask |= GPU_CTRL_DE_LUMA_CSC;
        pstCb->stCfg.u32LoadMask |= GPU_CTRL_DE_CHROMA_CSC;

        pstCb->stCfg.u32ComputeMask &= ~GPU_ALGO_CADE;
        pstCb->stCfg.u32LoadMask |= GPU_ALGO_CADE;
    }
    if (pstCb->stCfg.u32ComputeMask & GPU_ALGO_CADE_DEMO)
    {
        // Split screen process percentage
        fLvl = fVppUtils_ScaleFloat(SPLIT_PERCENT_MIN, SPLIT_PERCENT_MAX,
                                    GPU_CADE_PROCESS_PERCENT_MIN, GPU_CADE_PROCESS_PERCENT_MAX,
                                    (float)pstCb->stCfg.demo.process_percent);
        switch(pstCb->stCfg.demo.process_direction)
        {
            case HQV_SPLIT_RIGHT_TO_LEFT:
                pstCb->stDrvCade.process_percent_top = GPU_CADE_PROCESS_PERCENT_MIN;
                pstCb->stDrvCade.process_percent_bottom = GPU_CADE_PROCESS_PERCENT_MAX;
                pstCb->stDrvCade.process_percent_right = GPU_CADE_PROCESS_PERCENT_MAX;
                pstCb->stDrvCade.process_percent_left = GPU_CADE_PROCESS_PERCENT_MAX - fLvl;
                break;
            case HQV_SPLIT_TOP_TO_BOTTOM:
                pstCb->stDrvCade.process_percent_top = GPU_CADE_PROCESS_PERCENT_MIN;
                pstCb->stDrvCade.process_percent_left = GPU_CADE_PROCESS_PERCENT_MIN;
                pstCb->stDrvCade.process_percent_right = GPU_CADE_PROCESS_PERCENT_MAX;
                pstCb->stDrvCade.process_percent_bottom = fLvl;
                break;
            case HQV_SPLIT_BOTTOM_TO_TOP:
                pstCb->stDrvCade.process_percent_left = GPU_CADE_PROCESS_PERCENT_MIN;
                pstCb->stDrvCade.process_percent_right = GPU_CADE_PROCESS_PERCENT_MAX;
                pstCb->stDrvCade.process_percent_bottom = GPU_CADE_PROCESS_PERCENT_MAX;
                pstCb->stDrvCade.process_percent_top = GPU_CADE_PROCESS_PERCENT_MAX - fLvl;
                break;
            case HQV_SPLIT_LEFT_TO_RIGHT:
            default:
                pstCb->stDrvCade.process_percent_top = GPU_CADE_PROCESS_PERCENT_MIN;
                pstCb->stDrvCade.process_percent_bottom = GPU_CADE_PROCESS_PERCENT_MAX;
                pstCb->stDrvCade.process_percent_left = GPU_CADE_PROCESS_PERCENT_MIN;
                pstCb->stDrvCade.process_percent_right = fLvl;
                break;
        }

        pstCb->stCfg.u32LoadMask |= GPU_CTRL_DE_PROC_PERCENT;
        pstCb->stCfg.u32ComputeMask &= ~GPU_ALGO_CADE_DEMO;
        pstCb->stCfg.u32LoadMask |= GPU_ALGO_CADE;

    }
    if (bLocked)
    {
        pthread_mutex_unlock(&pstCb->mutex);
    }
}

static void vVppIpGpu_Load(t_StVppIpGpuCb *pstCb)
{
    // This function must be protected by mutex prior to calling.

    mmsw_error_t err;
    t_StVppIpGpuDrvCadeCb *p;

    p = &pstCb->stDrvCade;

    LOGD_IF(pstCb->stCfg.u32LoadMask, "GPU: u32LoadMask=0x%x",
            pstCb->stCfg.u32LoadMask);

    if (pstCb->stCfg.u32LoadMask & GPU_ALGO_CADE)
    {
        if (pstCb->stCfg.u32LoadMask & GPU_CTRL_DE_LVL)
        {
            err = mmsw_detail_enhancement_set_param(p->ctx,
                mmsw_detail_enhancement_param_de_level, (void *)&p->de_level);

            if (err != mmsw_error_success)
                LOGE("failed to set GPU_CTRL_DE_LEVEL, err=%d", err);

            pstCb->stCfg.u32LoadMask &= ~GPU_CTRL_DE_LVL;
        }

        if (pstCb->stCfg.u32LoadMask & GPU_CTRL_DE_LUMA_CSC)
        {
            err = mmsw_detail_enhancement_set_param(p->ctx,
                mmsw_detail_enhancement_param_luma_csc_level,
                (void *)&p->luma_csc_lvl);

            if (err != mmsw_error_success)
                LOGE("failed to set GPU_CTRL_DE_LUMA_CSC, err=%d", err);

            pstCb->stCfg.u32LoadMask &= ~GPU_CTRL_DE_LUMA_CSC;
        }

        if (pstCb->stCfg.u32LoadMask & GPU_CTRL_DE_CHROMA_CSC)
        {
            err = mmsw_detail_enhancement_set_param(p->ctx,
                mmsw_detail_enhancement_param_chroma_csc_level,
                (void *)&p->chroma_csc_lvl);

            if (err != mmsw_error_success)
                LOGE("failed to set GPU_CTRL_DE_CHROMA_CSC, err=%d", err);

            pstCb->stCfg.u32LoadMask &= ~GPU_CTRL_DE_CHROMA_CSC;
        }

        if (pstCb->stCfg.u32LoadMask & GPU_CTRL_DE_PROC_PERCENT)
        {
            err = mmsw_detail_enhancement_set_param(p->ctx,
                mmsw_detail_enhancement_param_process_percent_left,
                (void *)&p->process_percent_left);
            err |= mmsw_detail_enhancement_set_param(p->ctx,
                mmsw_detail_enhancement_param_process_percent_right,
                (void *)&p->process_percent_right);
            err |= mmsw_detail_enhancement_set_param(p->ctx,
                mmsw_detail_enhancement_param_process_percent_top,
                (void *)&p->process_percent_top);
            err |= mmsw_detail_enhancement_set_param(p->ctx,
                mmsw_detail_enhancement_param_process_percent_bottom,
                (void *)&p->process_percent_bottom);

            if (err != mmsw_error_success)
                LOGE("failed to set GPU_CTRL_DE_PROC_PERCENT, err=%d", err);

            pstCb->stCfg.u32LoadMask &= ~GPU_CTRL_DE_PROC_PERCENT;
        }

        pstCb->stCfg.u32LoadMask &= ~GPU_ALGO_CADE;
    }
}

static uint32_t u32VppIpGpu_CmdGet(t_StVppIpGpuCb *pstCb, t_StVppIpCmd *pstCmd)
{
    LOGI("%s()", __func__);
    int32_t idx;
    idx = vpp_queue_dequeue(&pstCb->stCmdQ);

    if (idx < 0)
    {
        return VPP_ERR;
    }
    else
    {
        *pstCmd = pstCb->astCmdNode[idx];
        LOG_CMD("GetCmd", pstCmd->eCmd);
    }

    return VPP_OK;
}

static uint32_t u32VppIpGpu_CmdPut(t_StVppIpGpuCb *pstCb, t_StVppIpCmd stCmd)
{
    int32_t idx;
    uint32_t u32Ret = VPP_OK;

    pthread_mutex_lock(&pstCb->mutex);

    LOG_CMD("InsertCmd", stCmd.eCmd);
    idx = vpp_queue_enqueue(&pstCb->stCmdQ);
    if (idx < 0)
    {
        u32Ret = VPP_ERR;
    }
    else
    {
        pstCb->astCmdNode[idx] = stCmd;
        pthread_cond_signal(&pstCb->cond);
    }

    pthread_mutex_unlock(&pstCb->mutex);

    return u32Ret;
}

static uint32_t u32VppIpGpu_PortBufPeek(t_StVppIpGpuPort *pstPort,
                                        t_StVppBuf **ppstBuf)
{
    int32_t idx;
    idx = vpp_queue_peek_head(&pstPort->stQ);

    if (idx < 0)
    {
        *ppstBuf = NULL;
        return VPP_ERR;
    }
    else
        *ppstBuf = pstPort->apBufNode[idx];

    return VPP_OK;
}


static uint32_t u32VppIpGpu_PortBufGet(t_StVppIpGpuPort *pstPort,
                                       t_StVppBuf **ppstBuf)
{
    int32_t idx;
    idx = vpp_queue_dequeue(&pstPort->stQ);
    if (idx < 0)
    {
        *ppstBuf = NULL;
        return VPP_ERR;
    }
    else
        *ppstBuf = pstPort->apBufNode[idx];

    return VPP_OK;
}

static uint32_t u32VppIpGpu_PortBufPut(t_StVppIpGpuCb *pstCb,
                                       t_StVppIpGpuPort *pstPort,
                                       t_StVppBuf *pBuf)
{
    int32_t idx;
    uint32_t u32Ret = VPP_OK;

    idx = vpp_queue_enqueue(&pstPort->stQ);
    if (idx < 0)
    {
        u32Ret = VPP_ERR;
    }
    else
    {
        pstPort->apBufNode[idx] = pBuf;
        pthread_cond_signal(&pstCb->cond);
    }

    return u32Ret;
}

static void vVppIpGpu_SignalWorkerStart(t_StVppIpGpuCb *pstCb)
{
    pthread_mutex_lock(&pstCb->mutex);

    pstCb->u32InternalFlags |= IP_WORKER_STARTED;

    pthread_cond_signal(&pstCb->cond);

    pthread_mutex_unlock(&pstCb->mutex);
}

static void vVppIpGpu_WaitWorkerStart(t_StVppIpGpuCb *pstCb)
{
    pthread_mutex_lock(&pstCb->mutex);

    while (!(pstCb->u32InternalFlags & IP_WORKER_STARTED))
        pthread_cond_wait(&pstCb->cond, &pstCb->mutex);

    pthread_mutex_unlock(&pstCb->mutex);
}

static uint32_t u32ProcBufReqMet(t_StVppIpGpuCb *pstCb)
{
    // Determine if the buffers in the ports satisfy the requirements
    // to trigger processing
    uint32_t u32InQSz, u32OutQSz;

    t_StVppBuf *pstBuf;

    // This function requires that the caller has already locked the mutex
    // which guards these two queues.
    u32InQSz = vpp_queue_count(&pstCb->stInput.stQ);
    u32OutQSz = vpp_queue_count(&pstCb->stOutput.stQ);

    LOGI("CHECK: u32InQSz=%u, u32OutQSz=%u, u32BypassCnt=%u, "
         "u32InterlacedCnt=%u u32InputMin=%u, u32OutputMin=%u", u32InQSz,
         u32OutQSz, pstCb->u32BypassCnt, pstCb->u32InterlacedCnt,
         pstCb->u32InputMin, pstCb->u32OutputMin);

    if (u32InQSz)
    {
        u32VppIpGpu_PortBufPeek(&pstCb->stInput, &pstBuf);

        if (!pstBuf)
        {
            LOGE("u32InQSz != 0 but u32VppIpGpu_PortBufPeek returned NULL buffer!");
            return VPP_FALSE;
        }

        if (pstCb->u32BypassCnt &&
            VPP_FLAG_IS_SET(pstBuf->u32InternalFlags, VPP_BUF_FLAG_BYPASS))
            return VPP_TRUE;

        if (pstCb->u32InterlacedCnt &&
            pstBuf->eBufType != eVppBufType_Progressive)
            return VPP_TRUE;

        if (pstBuf->stPixel.u32FilledLen == 0)
            return VPP_TRUE;
    }

    if (u32InQSz >= pstCb->u32InputMin && u32OutQSz >= pstCb->u32OutputMin)
    {
        return VPP_TRUE;
    }

    return VPP_FALSE;
}

static uint32_t u32WorkerThreadShouldSleep(t_StVppIpGpuCb *pstCb)
{
    uint32_t u32Ret = VPP_TRUE;
    uint32_t u32CmdQSz, u32ProcMet;

    // This is a predicate function used for determining if the function worker
    // thread should sleep or not. Worker thread uses a condition variable to
    // wake itself and the mutex which is used is the same as that which guards
    // these functions. Therefore, there is no need to lock a mutex prior to
    // checking the command queues within this context.
    u32CmdQSz = vpp_queue_count(&pstCb->stCmdQ);
    u32ProcMet = u32ProcBufReqMet(pstCb);

    if (u32CmdQSz)
    {
        u32Ret = VPP_FALSE;
    }
    else if (pstCb->eState == VPP_IP_STATE_ACTIVE && u32ProcMet)
    {
        u32Ret = VPP_FALSE;
    }

    LOGI("CHECK: shouldSleep=%u, u32CmdQSz=%u, u32ProcMet=%u", u32Ret,
         u32CmdQSz, u32ProcMet);

    return u32Ret;
}

static uint32_t u32VppIpGpu_ValidatePortParam(struct vpp_port_param *pstParam)
{
    if (!pstParam)
        return VPP_ERR;

    if (pstParam->width > pstParam->stride)
    {
        LOGE("validation failed: width=%u, stride=%u", pstParam->width,
             pstParam->stride);
        return VPP_ERR;
    }
    if (pstParam->height > pstParam->scanlines)
    {
        LOGE("validation failed: height=%u, scanlines=%u", pstParam->height,
             pstParam->scanlines);
        return VPP_ERR;
    }
    if (pstParam->width < GPU_PORT_PARAM_WIDTH_MIN)
    {
        LOGE("validation failed: width=%u, min=%u", pstParam->width,
             GPU_PORT_PARAM_WIDTH_MIN);
        return VPP_ERR;
    }
    if (pstParam->height < GPU_PORT_PARAM_HEIGHT_MIN)
    {
        LOGE("validation failed: height=%u, min=%u", pstParam->height,
             GPU_PORT_PARAM_HEIGHT_MIN);
        return VPP_ERR;
    }
    if (pstParam->fmt != VPP_COLOR_FORMAT_NV12_VENUS &&
        pstParam->fmt != VPP_COLOR_FORMAT_NV21_VENUS)
    {
        LOGE("validation failed: fmt=%u", pstParam->fmt);
        return VPP_ERR;
    }

    return VPP_OK;
}

static uint32_t u32VppIpGpu_ValidatePortParams(t_StVppIpGpuCb *pstCb,
                                               struct vpp_port_param *pstInput,
                                               struct vpp_port_param *pstOutput)
{

    VPP_UNUSED(pstCb);

    if (pstInput->height != pstOutput->height)
    {
        LOGE("validation failed: height, input: %u, output: %u",
             pstInput->height, pstOutput->height);
        return VPP_ERR;
    }
    if (pstInput->width != pstOutput->width)
    {
        LOGE("validation failed: width, input: %u, output: %u",
             pstInput->width, pstOutput->width);
        return VPP_ERR;
    }
    if (pstInput->fmt != pstOutput->fmt)
    {
        LOGE("validation failed: fmt, input: %u, output: %u",
             pstInput->fmt, pstOutput->fmt);
        return VPP_ERR;
    }
    if (u32VppIpGpu_ValidatePortParam(pstInput))
    {
        LOGE("validation failed: input port params.");
        return VPP_ERR;
    }
    if (u32VppIpGpu_ValidatePortParam(pstOutput))
    {
        LOGE("validation failed: output port params.");
        return VPP_ERR;
    }

    return VPP_OK;
}

static uint32_t u32VppIpGpu_ValidateConfig(t_StVppIpGpuCb *pstCb)
{
    return u32VppIpGpu_ValidatePortParams(pstCb, &pstCb->stBase.stInputParam,
                                          &pstCb->stBase.stOutputParam);
}

static void vVppIpGpu_GetClProgramName(char *cpName, uint32_t u32MaxNameLength)
{
    if((u32VppUtils_IsSoc(MSM8956) == VPP_TRUE) || (u32VppUtils_IsSoc(MSM8976) == VPP_TRUE))
        strlcpy(cpName, GPU_CL_PROGRAM_NM_8976, u32MaxNameLength);
    else
        strlcpy(cpName, GPU_CL_PROGRAM_NM_DEFAULT, u32MaxNameLength);
}

static uint32_t u32VppIpGpu_ProcCmdInit(t_StVppIpGpuCb *pstCb)
{
    FILE *fp_gpu;
    int rc;
    size_t sz;
    char cName[GPU_CL_NM_LEN];
    char *pProgram;
    uint32_t u32Flags = 0;
    mmsw_error_t error = mmsw_error_success;

#ifndef NO_COMPILE_MMSW

    vVppIpGpu_GetClProgramName(cName, sizeof(cName));
    fp_gpu = fopen(cName, "rb");
    if (!fp_gpu)
    {
        LOGE("unable to open gpu program %s, err=%d, (%s)", cName, errno, strerror(errno));
        goto ERR_GPU_OPEN;
    }

    rc = fseek(fp_gpu, 0L, SEEK_END);
    if (rc)
    {
        LOGE("unable to seek gpu program");
        goto ERR_GPU_SEEK;
    }

    sz = ftell(fp_gpu);
    rc = fseek(fp_gpu, 0L, SEEK_SET);

    pProgram = malloc(sz);
    if (!pProgram)
    {
        LOGE("unable to malloc file program");
        goto ERR_MALLOC;
    }

    size_t sz_rd = fread(pProgram, 1, sz, fp_gpu);

    u32Flags |= pstCb->stBase.bSecureSession ? mmsw_detail_enhancement_session_flag_secure : 0;
    LOGD("program size: %zu, read size: %zu\n", sz, sz_rd);
    error = mmsw_detail_enhancement_init(&pstCb->stDrvCade.ctx,
                                         u32Flags,
                                         pstCb->stDrvCade.quality_option,
                                         mmsw_detail_enhancement_program_binary,
                                         pProgram,
                                         sz);

    free(pProgram);
    fclose(fp_gpu);
#endif

    if (error != mmsw_error_success)
    {
        LOGE("cade initialized failed, error=%d", error);
        return VPP_ERR;
    }

    return VPP_OK;

ERR_MALLOC:
ERR_GPU_SEEK:
    fclose(fp_gpu);

ERR_GPU_OPEN:
    return VPP_ERR;

}

static uint32_t u32VppIpGpu_ProcCmdExit(t_StVppIpGpuCb *pstCb)
{
    uint32_t u32InQSz, u32OutQSz;
    mmsw_error_t error = mmsw_error_success;

#ifndef NO_COMPILE_MMSW
    error = mmsw_detail_enhancement_deinit(pstCb->stDrvCade.ctx);
#endif
    pstCb->stDrvCade.ctx = NULL;

    pthread_mutex_lock(&pstCb->mutex);
    u32InQSz = vpp_queue_count(&pstCb->stInput.stQ);
    u32OutQSz = vpp_queue_count(&pstCb->stOutput.stQ);
    pthread_mutex_unlock(&pstCb->mutex);

    if (u32InQSz || u32OutQSz)
        LOGD("processed close, u32InQSz=%u, u32OutQSz=%u", u32InQSz, u32OutQSz);

    if (error != mmsw_error_success)
        return VPP_ERR;

    return VPP_OK;
}

static uint32_t u32VppIpGpu_ProcCmdOpen(t_StVppIpGpuCb *pstCb)
{
    mmsw_error_t error = mmsw_error_success;
    uint32_t u32Ret = VPP_OK;

#ifndef NO_COMPILE_MMSW
    error = mmsw_detail_enhancement_open(pstCb->stDrvCade.ctx);
    if (error != mmsw_error_success)
        u32Ret = VPP_ERR_HW;
#endif

    pstCb->async_res.u32OpenRet = u32Ret;

    LOGI("u32VppIpGpu_ProcCmdOpen() posting semaphore, u32Ret=%u", u32Ret);
    sem_post(&pstCb->sem);

    return u32Ret;
}

static uint32_t u32VppIpGpu_ProcCmdClose(t_StVppIpGpuCb *pstCb)
{
    mmsw_error_t error = mmsw_error_success;
    uint32_t u32Ret = VPP_OK;

#ifndef NO_COMPILE_MMSW
    error = mmsw_detail_enhancement_close(pstCb->stDrvCade.ctx);
    if (error != mmsw_error_success)
        u32Ret = VPP_ERR_HW;
#endif

    pstCb->async_res.u32CloseRet = u32Ret;

    LOGI("u32VppIpGpu_ProcCmdClose() posting semaphore, u32Ret=%u", u32Ret);
    sem_post(&pstCb->sem);

    return u32Ret;
}

static uint32_t u32VppIpGpu_FlushPort(t_StVppIpGpuCb *pstCb, enum vpp_port ePort)
{
    t_StVppBuf *pBuf;
    t_StVppIpGpuPort *pstPort;

    if (ePort == VPP_PORT_INPUT)
    {
        pstPort = &pstCb->stInput;
    }
    else if (ePort == VPP_PORT_OUTPUT)
    {
        pstPort = &pstCb->stOutput;
    }
    else
    {
        return VPP_ERR_PARAM;
    }

    while (u32VppIpGpu_PortBufGet(pstPort, &pBuf) == VPP_OK)
    {
        VPP_FLAG_SET(pBuf->u32InternalFlags, VPP_BUF_FLAG_FLUSHED);
        pBuf->stPixel.u32FilledLen = 0;
        vVppIpCbLog(&pstCb->stBase.stCb, pBuf, eVppLogId_IpBufDone);
        u32VppIp_CbBufDone(&pstCb->stBase.stCb, ePort, pBuf);
    }

    return VPP_OK;
}

static uint32_t u32VppIpGpu_ProcCmdFlush(t_StVppIpGpuCb *pstCb,
                                         t_StVppIpCmd *pstCmd)
{
    uint32_t u32;
    t_StVppEvt stEvt;

    // Flush Port
    u32 = u32VppIpGpu_FlushPort(pstCb, pstCmd->flush.ePort);

    if (u32 == VPP_OK)
    {
        stEvt.eType = VPP_EVT_FLUSH_DONE;
        stEvt.flush.ePort = pstCmd->flush.ePort;
        u32VppIpCbEvent(&pstCb->stBase.stCb, stEvt);
    }

    return u32;
}

static uint32_t u32VppIpGpu_ProcCmdDrain(t_StVppIpGpuCb *pstCb)
{
    t_StVppEvt stEvt;

    pthread_mutex_lock(&pstCb->mutex);

    if (!vpp_queue_count(&pstCb->stInput.stQ))
    {
        // No more input buffers in the queue, drain has already been
        // completed.
        stEvt.eType = VPP_EVT_DRAIN_DONE;
        u32VppIpCbEvent(&pstCb->stBase.stCb, stEvt);
    }
    else
    {
        pstCb->u32InternalFlags |= IP_DRAIN_PENDING;
    }

    pthread_mutex_unlock(&pstCb->mutex);

    return VPP_OK;
}

static void vVppIpGpu_TranslateFrameParams(t_StVppIpGpuCb *pstCb,
                                           struct vpp_port_param *pstParam,
                                           t_StVppBuf *pstBuf,
                                           mmsw_frame_desc_t *pstFrame)
{
    pstFrame->format = eVppIpGpu_MmswColorFormatGet(pstParam->fmt);
    pstFrame->width = pstParam->width;
    pstFrame->height = pstParam->height;
    pstFrame->stride = u32VppUtils_GetStride(pstParam);
    pstFrame->fd = pstBuf->stPixel.fd;

    pstFrame->yOffset = pstBuf->stPixel.u32Offset;
    pstFrame->uvOffset = u32VppUtils_GetUVOffset(pstParam);
    pstFrame->baseSize = pstBuf->stPixel.u32AllocLen;
    pstFrame->baseAddress = NULL;

    if (!pstCb->stBase.bSecureSession)
    {
        pstFrame->baseAddress = pstBuf->stPixel.pvBase;
    }

}

static uint32_t u32VppIpGpu_ProcessBuffer(t_StVppIpGpuCb *pstCb)
{
    uint32_t u32BufSz, u32AvailSz, u32OutMin;
    t_StVppMemBuf *pstIntBuf;
    mmsw_error_t error = mmsw_error_success;
    mmsw_frame_desc_t mFrameIn, mFrameOut;
    t_StVppBuf *pstBufIn = NULL, *pstBufOut = NULL;

    LOG_ENTER();

    pthread_mutex_lock(&pstCb->mutex);

    vVppIpGpu_Load(pstCb);

    u32OutMin = pstCb->u32OutputMin;

    u32VppIpGpu_PortBufGet(&pstCb->stInput, &pstBufIn);
    if (!pstBufIn)
    {
        LOGE("port_buf_get returned null buffer!");
        pthread_mutex_unlock(&pstCb->mutex);
        LOG_EXIT_RET(VPP_ERR);
    }

    if (u32OutMin == 0 || pstBufIn->eBufType != eVppBufType_Progressive ||
        VPP_FLAG_IS_SET(pstBufIn->u32InternalFlags, VPP_BUF_FLAG_BYPASS) ||
        pstBufIn->stPixel.u32FilledLen == 0)
    {
        if (VPP_FLAG_IS_SET(pstBufIn->u32InternalFlags, VPP_BUF_FLAG_BYPASS))
        {
            GPU_STATS(pstCb, BypassBuf);
            pstCb->u32BypassCnt -= 1;
        }

        if (pstBufIn->eBufType != eVppBufType_Progressive)
        {
            GPU_STATS(pstCb, BypassIntBuf);
            pstCb->u32InterlacedCnt -= 1;
        }
    }
    else
    {
        u32VppIpGpu_PortBufGet(&pstCb->stOutput, &pstBufOut);
    }

    pthread_mutex_unlock(&pstCb->mutex);

    if (pstBufIn && pstBufOut)
    {
        // Make sure that output buffer size is sufficient for input buffer
        pstIntBuf = &pstBufOut->stPixel;

        u32BufSz = u32VppUtils_GetPxBufferSize(&pstCb->stBase.stInputParam);
        u32AvailSz = pstIntBuf->u32AllocLen - pstIntBuf->u32Offset;
        if (u32AvailSz < u32BufSz)
        {
            LOGE("required buffer size exceeds available, bypassing this buffer");
            LOGE("req_sz=%u, buf:{fd=%d, u32Offset=%u, u32AllocLen=%u, u32FilledLen=%u}",
                 u32BufSz, pstIntBuf->fd, pstIntBuf->u32Offset,
                 pstIntBuf->u32AllocLen, pstIntBuf->u32FilledLen);

            // Send input buffer to output, return output buffer
            GPU_STATS(pstCb, OBDCnt);
            vVppIpCbLog(&pstCb->stBase.stCb, pstBufIn, eVppLogId_IpBufDone);
            u32VppIp_CbBufDone(&pstCb->stBase.stCb, VPP_PORT_OUTPUT, pstBufIn);

            pstBufOut->stPixel.u32FilledLen = 0;
            GPU_STATS(pstCb, OBDCnt);
            vVppIpCbLog(&pstCb->stBase.stCb, pstBufOut, eVppLogId_IpBufDone);
            u32VppIp_CbBufDone(&pstCb->stBase.stCb, VPP_PORT_OUTPUT, pstBufOut);

            LOG_EXIT_RET(VPP_ERR);
        }

        // Normal processing
        memset(&mFrameIn, 0, sizeof(mmsw_frame_desc_t));
        memset(&mFrameOut, 0, sizeof(mmsw_frame_desc_t));

        vVppIpGpu_TranslateFrameParams(pstCb, &pstCb->stBase.stInputParam,
                                       pstBufIn, &mFrameIn);
        vVppIpGpu_TranslateFrameParams(pstCb, &pstCb->stBase.stOutputParam,
                                       pstBufOut, &mFrameOut);

        GPU_DUMP_FRAME_DESC(&mFrameIn, "in");
        GPU_DUMP_FRAME_DESC(&mFrameOut, "out");

        GPU_STATS(pstCb, InProcCnt);
        GPU_STATS(pstCb, OutProcCnt);

        if(VPP_FLAG_IS_SET(pstBufIn->u32InternalFlags, VPP_BUF_FLAG_DUMP))
        {
            GPU_DUMP_FRAME(pstCb, pstBufIn, VPP_PORT_INPUT);
        }
        LOGI("calling detail enhancement");

        vVppIpCbLog(&pstCb->stBase.stCb, pstBufIn, eVppLogId_IpProcStart);
        vVppIpCbLog(&pstCb->stBase.stCb, pstBufOut, eVppLogId_IpProcStart);

        VPP_IP_PROF_START(&pstCb->stBase, GPU_STAT_PROC);
#ifndef NO_COMPILE_MMSW
        error = mmsw_detail_enhancement(pstCb->stDrvCade.ctx, &mFrameIn, &mFrameOut);
#else
        usleep(6000);
        error = mmsw_error_success;
#endif
        VPP_IP_PROF_STOP(&pstCb->stBase, GPU_STAT_PROC);

        vVppIpCbLog(&pstCb->stBase.stCb, pstBufIn, eVppLogId_IpProcDone);
        vVppIpCbLog(&pstCb->stBase.stCb, pstBufOut, eVppLogId_IpProcDone);
        LOGI("detail enhancement returned, error=%u", error);

        if(VPP_FLAG_IS_SET(pstBufOut->u32InternalFlags, VPP_BUF_FLAG_DUMP))
        {
            GPU_DUMP_FRAME(pstCb, pstBufOut, VPP_PORT_OUTPUT);
        }

        VPP_FLAG_CLR(pstBufOut->u32InternalFlags, VPP_BUF_FLAG_FLUSHED);

        // Copy from input to output:
        if (error != mmsw_error_success)
        {
            LOGE("error processing buffer, sending input to output, err=%u", error);
            pstBufOut->stPixel.u32FilledLen = 0;

            GPU_STATS(pstCb, OBDCnt);
            vVppIpCbLog(&pstCb->stBase.stCb, pstBufIn, eVppLogId_IpBufDone);
            u32VppIp_CbBufDone(&pstCb->stBase.stCb, VPP_PORT_OUTPUT, pstBufIn);
        }
        else
        {
            pstBufOut->stPixel.u32FilledLen = u32BufSz;
            pstBufOut->pBuf->timestamp = pstBufIn->pBuf->timestamp;
            pstBufOut->pBuf->flags = pstBufIn->pBuf->flags;
            pstBufOut->pBuf->cookie_in_to_out = pstBufIn->pBuf->cookie_in_to_out;
            pstBufOut->u32TimestampFrameRate = pstBufIn->u32TimestampFrameRate;
            pstBufOut->u32OperatingRate = pstBufIn->u32OperatingRate;
            u32VppBuf_PropagateExtradata(pstBufIn, pstBufOut,
                                         eVppBufType_Progressive,
                                         VPP_EXTERNAL_EXTRADATA_TYPE);
            u32VppBuf_GrallocMetadataCopy(pstBufIn, pstBufOut);

            GPU_STATS(pstCb, IBDCnt);
            vVppIpCbLog(&pstCb->stBase.stCb, pstBufIn, eVppLogId_IpBufDone);
            u32VppIp_CbBufDone(&pstCb->stBase.stCb, VPP_PORT_INPUT, pstBufIn);
        }

        GPU_STATS(pstCb, OBDCnt);
        vVppIpCbLog(&pstCb->stBase.stCb, pstBufOut, eVppLogId_IpBufDone);
        u32VppIp_CbBufDone(&pstCb->stBase.stCb, VPP_PORT_OUTPUT, pstBufOut);
    }
    else if (pstBufIn && !pstBufOut)
    {
        // Bypass
        GPU_STATS(pstCb, OBDCnt);
        LOGI("bypassing input buffer");
        vVppIpCbLog(&pstCb->stBase.stCb, pstBufIn, eVppLogId_IpBufDone);
        u32VppIp_CbBufDone(&pstCb->stBase.stCb, VPP_PORT_OUTPUT, pstBufIn);
    }
    else
    {
        // Error - should never get into here
        LOG_EXIT_RET(VPP_ERR);
    }

    LOG_EXIT_RET(VPP_OK);
}

static void vVppIpGpu_HandlePendingDrain(t_StVppIpGpuCb *pstCb)
{
    t_StVppEvt stEvt;

    pthread_mutex_lock(&pstCb->mutex);

    if (pstCb->u32InternalFlags & IP_DRAIN_PENDING &&
        !vpp_queue_count(&pstCb->stInput.stQ))
    {
        pstCb->u32InternalFlags &= ~IP_DRAIN_PENDING;
        stEvt.eType = VPP_EVT_DRAIN_DONE;
        u32VppIpCbEvent(&pstCb->stBase.stCb, stEvt);
    }

    pthread_mutex_unlock(&pstCb->mutex);
}

static void *vpVppIpGpu_Worker(void *pv)
{
    LOGD("%s started", __func__);

    t_StVppIpGpuCb *pstCb = (t_StVppIpGpuCb *)pv;

    // Initialize the hardware
    pstCb->u32WorkerErr = u32VppIpGpu_ProcCmdInit(pstCb);

    // Signal back to main thread that we've launched and are ready to go
    vVppIpGpu_SignalWorkerStart(pstCb);

    if (pstCb->u32WorkerErr != VPP_OK)
        goto GPU_WORKER_EXIT;

    while (1)
    {
        pthread_mutex_lock(&pstCb->mutex);
        while (u32WorkerThreadShouldSleep(pstCb))
            pthread_cond_wait(&pstCb->cond, &pstCb->mutex);

        uint32_t u32Ret;
        t_StVppIpCmd stCmd;
        u32Ret = u32VppIpGpu_CmdGet(pstCb, &stCmd);
        if (u32Ret == VPP_OK)
        {
            pthread_mutex_unlock(&pstCb->mutex);

            // Process the command
            LOG_CMD("ProcessCmd", stCmd.eCmd);

            if (stCmd.eCmd == VPP_IP_CMD_THREAD_EXIT)
            {
                u32VppIpGpu_ProcCmdExit(pstCb);
                break;
            }

            else if (stCmd.eCmd == VPP_IP_CMD_OPEN)
                u32VppIpGpu_ProcCmdOpen(pstCb);

            else if (stCmd.eCmd == VPP_IP_CMD_CLOSE)
                u32VppIpGpu_ProcCmdClose(pstCb);

            else if (stCmd.eCmd == VPP_IP_CMD_FLUSH)
                u32VppIpGpu_ProcCmdFlush(pstCb, &stCmd);

            else if (stCmd.eCmd == VPP_IP_CMD_DRAIN)
                u32VppIpGpu_ProcCmdDrain(pstCb);

            else
                LOGE("unknown command in queue");

            continue;
        }

        if (pstCb->eState == VPP_IP_STATE_ACTIVE)
        {
            pthread_mutex_unlock(&pstCb->mutex);
            u32VppIpGpu_ProcessBuffer(pstCb);
            vVppIpGpu_HandlePendingDrain(pstCb);
            continue;
        }

        LOGE("%s woke up, but did no work", __func__);
        pthread_mutex_unlock(&pstCb->mutex);
    }

GPU_WORKER_EXIT:
    LOGD("%s exited", __func__);

    return NULL;
}

/************************************************************************
 * Global Functions
 ***********************************************************************/

void * vpVppIpGpu_Init(t_StVppCtx *pstCtx, uint32_t u32Flags, t_StVppCallback cbs)
{
    LOG_ENTER_ARGS("flags=0x%x", u32Flags);

    int rc;
    uint32_t u32;
    t_StVppIpGpuCb *pstCb;

    VPP_RET_IF_NULL(pstCtx, NULL);

    if (u32VppIp_ValidateCallbacks(&cbs))
    {
        LOGE("given invalid callbacks.");
        goto ERROR_CALLBACKS;
    }

    pstCb = calloc(sizeof(t_StVppIpGpuCb), 1);
    if (!pstCb)
    {
        LOGE("calloc failed for gpu context");
        goto ERROR_MALLOC;
    }

    u32VppIp_SetBase(pstCtx, u32Flags, cbs, &pstCb->stBase);

    u32 = VPP_IP_PROF_REGISTER(&pstCb->stBase, astGpuStatsCfg, u32GpuStatCnt);
    LOGE_IF(u32 != VPP_OK, "ERROR: unable to register stats, u32=%u", u32);

    vpp_queue_init(&pstCb->stInput.stQ, GPU_PORT_BUF_Q_SZ);
    vpp_queue_init(&pstCb->stOutput.stQ, GPU_PORT_BUF_Q_SZ);
    vpp_queue_init(&pstCb->stCmdQ, GPU_CMD_Q_SZ);

    rc = sem_init(&pstCb->sem, 0, 0);
    if (rc)
    {
        LOGE("unable to initialize gpu mutex");
        goto ERROR_SEM_INIT;
    }

    rc = pthread_mutex_init(&pstCb->mutex, NULL);
    if (rc)
    {
        LOGE("unable to initialize gpu mutex");
        goto ERROR_MUTEX_INIT;
    }

    rc = pthread_cond_init(&pstCb->cond, NULL);
    if (rc)
    {
        LOGE("unable to init condition variable");
        goto ERROR_COND_INIT;
    }

    rc = pthread_create(&pstCb->thread, NULL, vpVppIpGpu_Worker, pstCb);
    if (rc)
    {
        LOGE("unable to spawn gpu worker thread");
        goto ERROR_THREAD_CREATE;
    }

    // Set the default input to output requirement
    pstCb->u32InputMin = 1;
    pstCb->u32OutputMin = 1;

    // Set default GPU post processing parameters
    pstCb->stDrvCade.de_level = GPU_CADE_LEVEL_DEFAULT;
    pstCb->stCfg.u32LoadMask |= GPU_CTRL_DE_LVL;

    pstCb->stDrvCade.process_percent_left = GPU_CADE_PROCESS_PERCENT_MIN;
    pstCb->stDrvCade.process_percent_right = GPU_CADE_PROCESS_PERCENT_MAX;
    pstCb->stDrvCade.process_percent_top = GPU_CADE_PROCESS_PERCENT_MIN;
    pstCb->stDrvCade.process_percent_bottom = GPU_CADE_PROCESS_PERCENT_MAX;
    pstCb->stCfg.u32LoadMask |= GPU_CTRL_DE_PROC_PERCENT;

    pstCb->stDrvCade.luma_csc_lvl = GPU_CADE_LUMA_CSC_DEFAULT;
    pstCb->stCfg.u32LoadMask |= GPU_CTRL_DE_LUMA_CSC;

    pstCb->stDrvCade.chroma_csc_lvl = GPU_CADE_CHROMA_CSC_DEFAULT;
    pstCb->stCfg.u32LoadMask |= GPU_CTRL_DE_CHROMA_CSC;

    pstCb->stDrvCade.quality_option = mmsw_detail_enhancement_quality_cade;
    pstCb->stCfg.u32LoadMask |= GPU_ALGO_CADE;

    // Wait for the thread to launch before returning
    vVppIpGpu_WaitWorkerStart(pstCb);

    if (pstCb->u32WorkerErr != VPP_OK)
    {
        LOGE("Worker initialization error, err=%u", pstCb->u32WorkerErr);
        goto ERROR_WORKER_START;
    }

    VPP_IP_STATE_SET(pstCb, VPP_IP_STATE_INITED);

    return pstCb;

ERROR_WORKER_START:
    rc = pthread_join(pstCb->thread, NULL);
    if (rc)
        LOGE("pthread_join failed: %d --> %s", rc, strerror(rc));

ERROR_THREAD_CREATE:
    LOGI("destroying condition variable");
    pthread_cond_destroy(&pstCb->cond);

ERROR_COND_INIT:
    LOGI("destroying mutex");
    pthread_mutex_destroy(&pstCb->mutex);

ERROR_MUTEX_INIT:
    LOGI("destroying semaphore");
    sem_destroy(&pstCb->sem);

ERROR_SEM_INIT:
    u32 = VPP_IP_PROF_UNREGISTER(&pstCb->stBase);
    LOGE_IF(u32 != VPP_OK, "ERROR: unable to unregister stats, u32=%u", u32);

    free(pstCb);

ERROR_MALLOC:
ERROR_CALLBACKS:
    return NULL;
}

void vVppIpGpu_Term(void *ctx)
{
    int rc;
    uint32_t u32;
    t_StVppIpGpuCb *pstCb;
    t_StVppIpCmd stCmd;

    LOGI("%s\n", __func__);

    VPP_RET_VOID_IF_NULL(ctx);
    pstCb = GPU_CB_GET(ctx);

    stCmd.eCmd = VPP_IP_CMD_THREAD_EXIT;
    u32VppIpGpu_CmdPut(pstCb, stCmd);

    rc = pthread_join(pstCb->thread, NULL);
    if (rc)
        LOGE("pthread_join failed: %d --> %s", rc, strerror(rc));

    u32VppIpGpu_FlushPort(pstCb, VPP_PORT_INPUT);
    u32VppIpGpu_FlushPort(pstCb, VPP_PORT_OUTPUT);

    rc = pthread_mutex_destroy(&pstCb->mutex);
    if (rc)
        LOGE("pthread_mutex_destroy failed: %d --> %s", rc, strerror(rc));

    rc = sem_destroy(&pstCb->sem);
    if (rc)
        LOGE("sem_destroy failed: %d --> %s", rc, strerror(rc));

    vpp_queue_term(&pstCb->stCmdQ);
    vpp_queue_term(&pstCb->stInput.stQ);
    vpp_queue_term(&pstCb->stOutput.stQ);

    u32 = VPP_IP_PROF_UNREGISTER(&pstCb->stBase);
    LOGE_IF(u32 != VPP_OK, "ERROR: unable to unregister stats, u32=%u", u32);

    free(pstCb);
}

uint32_t u32VppIpGpu_Open(void *ctx)
{
    LOGI("%s\n", __func__);

    t_StVppIpGpuCb *pstCb;
    t_StVppIpCmd stCmd;

    VPP_RET_IF_NULL(ctx, VPP_ERR_PARAM);
    pstCb = GPU_CB_GET(ctx);

    if (!VPP_IP_STATE_EQUAL(pstCb, VPP_IP_STATE_INITED))
        return VPP_ERR_STATE;

    // Validate that the port configuration is valid
    if (u32VppIpGpu_ValidateConfig(pstCb) != VPP_OK)
        return VPP_ERR_PARAM;

    VPP_IP_STATE_SET(pstCb, VPP_IP_STATE_ACTIVE);

    stCmd.eCmd = VPP_IP_CMD_OPEN;
    u32VppIpGpu_CmdPut(pstCb, stCmd);

    LOGI(">> waiting on semaphore");
    sem_wait(&pstCb->sem);
    LOGI(">> got semaphore");

    return pstCb->async_res.u32OpenRet;
}

uint32_t u32VppIpGpu_Close(void *ctx)
{
    LOGI("%s\n", __func__);

    t_StVppIpGpuCb *pstCb;
    t_StVppIpCmd stCmd;

    VPP_RET_IF_NULL(ctx, VPP_ERR_PARAM);
    pstCb = GPU_CB_GET(ctx);

    if (!VPP_IP_STATE_EQUAL(pstCb, VPP_IP_STATE_ACTIVE))
        return VPP_ERR_STATE;

    VPP_IP_STATE_SET(pstCb, VPP_IP_STATE_INITED);

    stCmd.eCmd = VPP_IP_CMD_CLOSE;
    u32VppIpGpu_CmdPut(pstCb, stCmd);

    LOGI(">> waiting on semaphore");
    sem_wait(&pstCb->sem);
    LOGI(">> got semaphore");

    return pstCb->async_res.u32CloseRet;
}


uint32_t u32VppIpGpu_SetParam(void *ctx, enum vpp_port port,
                              struct vpp_port_param param)
{
    LOGI("%s(), port=%u", __func__, port);

    uint32_t u32Ret = VPP_OK;
    t_StVppIpGpuCb *pstCb;

    VPP_RET_IF_NULL(ctx, VPP_ERR_PARAM);
    pstCb = GPU_CB_GET(ctx);

    if (!VPP_IP_STATE_EQUAL(pstCb, VPP_IP_STATE_INITED))
        return VPP_ERR_STATE;

    if (u32VppIpGpu_ValidatePortParam(&param))
    {
        LOGE("given invalid port params.");
        return VPP_ERR_PARAM;
    }

    pthread_mutex_lock(&pstCb->mutex);

    if (port == VPP_PORT_INPUT)
        pstCb->stBase.stInputParam = param;
    else if (port == VPP_PORT_OUTPUT)
        pstCb->stBase.stOutputParam = param;
    else
        u32Ret = VPP_ERR_PARAM;

    LOG_PARAM_PTR(I, &pstCb->stBase.stInputParam, &pstCb->stBase.stOutputParam);

    pthread_mutex_unlock(&pstCb->mutex);

    return u32Ret;
}

uint32_t u32VppIpGpu_SetCtrl(void *ctx, struct hqv_control ctrl)
{
    LOGI("%s\n", __func__);

    uint32_t u32Ret = VPP_OK;
    t_StVppIpGpuCb *pstCb;

    VPP_RET_IF_NULL(ctx, VPP_ERR_PARAM);
    pstCb = GPU_CB_GET(ctx);

    // Set
    if (ctrl.mode == HQV_MODE_OFF)
    {
        pstCb->stCfg.cade.mode = HQV_MODE_OFF;

        VPP_FLAG_CLR(pstCb->stCfg.u32EnableMask, GPU_ALGO_CADE);
    }
    else if (ctrl.ctrl_type == HQV_CONTROL_GLOBAL_DEMO &&
             (ctrl.mode == HQV_MODE_MANUAL || ctrl.mode == HQV_MODE_AUTO))
    {
        pstCb->stCfg.demo = ctrl.demo;
        VPP_FLAG_SET(pstCb->stCfg.u32ComputeMask, GPU_ALGO_CADE_DEMO);
    }
    else if (ctrl.mode == HQV_MODE_AUTO)
    {
        pstCb->stCfg.cade.mode = HQV_MODE_AUTO;

        VPP_FLAG_SET(pstCb->stCfg.u32EnableMask, GPU_ALGO_CADE);
        VPP_FLAG_SET(pstCb->stCfg.u32ComputeMask, GPU_ALGO_CADE);
    }
    else if (ctrl.mode == HQV_MODE_MANUAL)
    {
        if (ctrl.ctrl_type == HQV_CONTROL_CADE)
        {
            pstCb->stCfg.cade = ctrl.cade;

            if (pstCb->stCfg.cade.mode == HQV_MODE_AUTO ||
                pstCb->stCfg.cade.mode == HQV_MODE_MANUAL)
            {
                VPP_FLAG_SET(pstCb->stCfg.u32EnableMask, GPU_ALGO_CADE);
                VPP_FLAG_SET(pstCb->stCfg.u32ComputeMask, GPU_ALGO_CADE);
            }
            else
            {
                LOGE("stCfg.cade.mode = %u", pstCb->stCfg.cade.mode);
                VPP_FLAG_CLR(pstCb->stCfg.u32EnableMask, GPU_ALGO_CADE);
            }
        }
        else
        {
            LOGE("Unsupported control: ctrl_type=%u", ctrl.ctrl_type);
            u32Ret = VPP_ERR_INVALID_CFG;
        }
    }
    else
    {
        LOGE("Unsupported mode: mode=%u", ctrl.mode);
        u32Ret = VPP_ERR_INVALID_CFG;
    }
    if (u32Ret == VPP_OK)
        vVppIpGpu_Compute(pstCb);

    // TODO: Either need to move this out of here and into init or we need to
    // guard it with a mutex (we shouldn't really be changing what the input
    // and output requirements are...)
    if (pstCb->stCfg.u32EnableMask & GPU_ALGO_CADE)
    {
        pstCb->u32InputMin = 1;
        pstCb->u32OutputMin = 1;
    }
    else if (!pstCb->stCfg.u32EnableMask)
    {
        pstCb->u32InputMin = 1;
        pstCb->u32OutputMin = 0;
    }
    LOGI("u32InputMin=%u, u32OutputMin=%u", pstCb->u32InputMin, pstCb->u32OutputMin);

    return u32Ret;
}

uint32_t u32VppIpGpu_GetBufferRequirements(void *ctx,
                                           t_StVppIpBufReq *pstInputBufReq,
                                           t_StVppIpBufReq *pstOutputBufReq)
{
    LOGI("%s\n", __func__);

    t_StVppIpGpuCb *pstCb;
    uint32_t u32PxSz;

    VPP_RET_IF_NULL(ctx, VPP_ERR_PARAM);
    pstCb = GPU_CB_GET(ctx);

    u32PxSz = u32VppUtils_GetPxBufferSize(&pstCb->stBase.stInputParam);

    if (pstInputBufReq)
    {
        pstInputBufReq->u32MinCnt = pstCb->u32InputMin;
        pstInputBufReq->u32PxSz = u32PxSz;
        pstInputBufReq->u32ExSz = 0;
    }

    if (pstOutputBufReq)
    {
        pstOutputBufReq->u32MinCnt = pstCb->u32OutputMin;
        pstOutputBufReq->u32PxSz = u32PxSz;
        pstOutputBufReq->u32ExSz = 0;
    }

    return VPP_OK;
}

uint32_t u32VppIpGpu_QueueBuf(void *ctx, enum vpp_port ePort,
                              t_StVppBuf *pBuf)
{
    LOGI("%s() port=%u\n", __func__, (uint32_t)ePort);

    uint32_t u32Ret = VPP_OK;
    t_StVppIpGpuCb *pstCb;
    t_EVppBufType eBufType;

    if (ePort != VPP_PORT_INPUT && ePort != VPP_PORT_OUTPUT)
    {
        LOGE("given invalid port. port=%u", ePort);
        return VPP_ERR_PARAM;
    }

    VPP_RET_IF_NULL(pBuf, VPP_ERR_PARAM);
    VPP_RET_IF_NULL(ctx, VPP_ERR_PARAM);
    pstCb = GPU_CB_GET(ctx);

    if (!VPP_IP_STATE_EQUAL(pstCb, VPP_IP_STATE_ACTIVE))
    {
        LOGE("invalid state. state=%u, exp=%u", pstCb->eState,
             VPP_IP_STATE_ACTIVE);
        return VPP_ERR_STATE;
    }

    pthread_mutex_lock(&pstCb->mutex);

    vVppIpCbLog(&pstCb->stBase.stCb, pBuf, eVppLogId_IpQueueBuf);

    if (ePort == VPP_PORT_INPUT)
    {
        if (VPP_FLAG_IS_SET(pBuf->u32InternalFlags, VPP_BUF_FLAG_BYPASS))
            pstCb->u32BypassCnt += 1;

        if (pBuf->eBufType != eVppBufType_Progressive)
            pstCb->u32InterlacedCnt += 1;

        u32Ret = u32VppIpGpu_PortBufPut(pstCb, &pstCb->stInput, pBuf);
        if (u32Ret == VPP_OK)
        {
            GPU_STATS(pstCb, InQCnt);
        }
        else
        {
            if (VPP_FLAG_IS_SET(pBuf->u32InternalFlags, VPP_BUF_FLAG_BYPASS))
                pstCb->u32BypassCnt -= 1;

            if (pBuf->eBufType != eVppBufType_Progressive)
                pstCb->u32InterlacedCnt -= 1;
        }
    }
    else if (ePort == VPP_PORT_OUTPUT)
    {
        u32Ret = u32VppIpGpu_PortBufPut(pstCb, &pstCb->stOutput, pBuf);
        if (u32Ret == VPP_OK)
            GPU_STATS(pstCb, OutQCnt);
    }

    pthread_mutex_unlock(&pstCb->mutex);

    return u32Ret;
}

uint32_t u32VppIpGpu_Flush(void *ctx, enum vpp_port ePort)
{
    LOGI("%s\n", __func__);

    uint32_t u32Ret = VPP_OK;
    t_StVppIpGpuCb *pstCb;
    t_StVppIpCmd stCmd;

    if (ePort != VPP_PORT_INPUT && ePort != VPP_PORT_OUTPUT)
    {
        LOGE("given invalid port. port=%u", ePort);
        return VPP_ERR_PARAM;
    }

    VPP_RET_IF_NULL(ctx, VPP_ERR_PARAM);
    pstCb = GPU_CB_GET(ctx);

    stCmd.eCmd = VPP_IP_CMD_FLUSH;
    stCmd.flush.ePort = ePort;
    u32VppIpGpu_CmdPut(pstCb, stCmd);

    return u32Ret;
}

uint32_t u32VppIpGpu_Drain(void *ctx)
{
    LOGI("%s\n", __func__);

    uint32_t u32Ret = VPP_OK;
    t_StVppIpGpuCb *pstCb;
    t_StVppIpCmd stCmd;

    VPP_RET_IF_NULL(ctx, VPP_ERR_PARAM);
    pstCb = GPU_CB_GET(ctx);

    if (!VPP_IP_STATE_EQUAL(pstCb, VPP_IP_STATE_ACTIVE))
    {
        LOGE("invalid state. state=%u, exp=%u", pstCb->eState,
             VPP_IP_STATE_ACTIVE);
        return VPP_ERR_STATE;
    }

    stCmd.eCmd = VPP_IP_CMD_DRAIN;
    u32VppIpGpu_CmdPut(pstCb, stCmd);

    return u32Ret;
}

uint32_t u32VppIpGpu_Reconfigure(void *ctx,
                                 struct vpp_port_param in_param,
                                 struct vpp_port_param out_param)
{
    LOG_ENTER();

    uint32_t u32QSz;
    uint32_t u32Ret = VPP_OK;
    t_StVppIpGpuCb *pstCb;

    VPP_RET_IF_NULL(ctx, VPP_ERR_PARAM);
    pstCb = GPU_CB_GET(ctx);

    if (!VPP_IP_STATE_EQUAL(pstCb, VPP_IP_STATE_ACTIVE))
    {
        LOGE("invalid state. state=%u, exp=%u", pstCb->eState,
             VPP_IP_STATE_ACTIVE);
        return VPP_ERR_STATE;
    }

    pthread_mutex_lock(&pstCb->mutex);

    u32QSz = vpp_queue_count(&pstCb->stInput.stQ);
    if (u32QSz)
    {
        u32Ret = VPP_ERR_STATE;
        LOGE("ERROR: reconfigure called with buffers in input port, cnt=%u", u32QSz);
    }
    else
    {
        if (u32VppIpGpu_ValidatePortParams(pstCb, &in_param, &out_param))
        {
            u32Ret = VPP_ERR_PARAM;
            LOGE("ERROR: invalid new port params.");
        }
        else
        {
            pstCb->stBase.stInputParam = in_param;
            pstCb->stBase.stOutputParam = out_param;
            LOGI("reconfigured to new params");
            LOG_PARAM_PTR(I, &pstCb->stBase.stInputParam, &pstCb->stBase.stOutputParam);
        }
    }

    pthread_mutex_unlock(&pstCb->mutex);

    LOG_EXIT_RET(u32Ret);
}
