/*!
 * @file vpp_pipeline.c
 *
 * @cr
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.

 * @services    Implements the pipeline, which coordinates buffer flow to the
 *              various IP blocks within the VPP.
 */

#include <sys/types.h>
#include <stdlib.h>
#include <string.h>

#include "vpp.h"
#define VPP_LOG_TAG     VPP_LOG_MODULE_PIPELINE_TAG
#define VPP_LOG_MODULE  VPP_LOG_MODULE_PIPELINE
#include "vpp_dbg.h"
#include "vpp_core.h"
#include "vpp_ctx.h"
#include "vpp_ip.h"
#include "vpp_uc.h"
#include "vpp_buf.h"
#include "vpp_stats.h"
#include "vpp_utils.h"
#include "vpp_ion.h"

#include "vpp_pipeline.h"

/************************************************************************
 * Local definitions
 ***********************************************************************/

#define VPP_PL_CB_GET(ctx) ((t_StVppPipelineCb *)ctx->pstPlCb)
#define VPP_PL_STATE_EQUAL(plcb, state) ((uint32_t)(plcb->eState == state))
#define VPP_PL_LAST_IP_IDX(plcb) (plcb->u32IpCnt - 1)

#define IP_CTX_GET(pstPlCb, i)                  pstPlCb->pstIpInfo[i].pvIpCtx
#define IP_CTX_SET(pstPlCb, i, val)             pstPlCb->pstIpInfo[i].pvIpCtx = val

#define IP_GET(pPl, pIdx)                       pPl->pstUcOpen->composition[pIdx]

#define IP_FUNC(pPl, pIdx)                      IP_GET(pPl, pIdx).ip->fnc

#define IP_INIT(pPlCb, pCtx, pIdx, flags, cbs)  IP_FUNC(pPlCb, pIdx).init(pCtx, flags, cbs)
#define IP_TERM(pPlCb, pIdx)                    IP_FUNC(pPlCb, pIdx).term(IP_CTX_GET(pPlCb, pIdx))
#define IP_OPEN(pPlCb, pIdx)                    IP_FUNC(pPlCb, pIdx).open(IP_CTX_GET(pPlCb, pIdx))
#define IP_CLOSE(pPlCb, pIdx)                   IP_FUNC(pPlCb, pIdx).close(IP_CTX_GET(pPlCb, pIdx))
#define IP_SET_PARAM(pPlCb, pIdx, port, param)  IP_FUNC(pPlCb, pIdx).set_param(IP_CTX_GET(pPlCb, pIdx), port, param)
#define IP_SET_CTRL(pPlCb, pIdx, ctrl)          IP_FUNC(pPlCb, pIdx).set_ctrl(IP_CTX_GET(pPlCb, pIdx), ctrl)
#define IP_GET_BUF_REQ(pPlCb, pIdx, in, out)    IP_FUNC(pPlCb, pIdx).get_buffer_requirements(IP_CTX_GET(pPlCb, pIdx), in, out)
#define IP_QUEUE_BUF(pPlCb, pIdx, port, buf)    IP_FUNC(pPlCb, pIdx).queue_buf(IP_CTX_GET(pPlCb, pIdx), port, buf)
#define IP_FLUSH(pPlCb, pIdx, port)             IP_FUNC(pPlCb, pIdx).flush(IP_CTX_GET(pPlCb, pIdx), port)
#define IP_DRAIN(pPlCb, pIdx)                   IP_FUNC(pPlCb, pIdx).drain(IP_CTX_GET(pPlCb, pIdx))
#define IP_RCFG(pPlCb, pIdx, in, out)           IP_FUNC(pPlCb, pIdx).reconfigure(IP_CTX_GET(pPlCb, pIdx), in, out)

#define BUF_CNT_QUEUE(pPlCb, pIdx, port, buf)   { \
                                                    pPlCb->pstIpInfo[pIdx].stBufCnt.u32Total[port]++; \
                                                    if (buf->u32InternalFlags & VPP_BUF_FLAG_INTERNAL) \
                                                    { \
                                                        pPlCb->pstIpInfo[pIdx].stBufCnt.u32Internal[port]++; \
                                                    } \
                                                }

#define BUF_CNT_DEQUEUE(pPlCb, pIdx, port, buf) { \
                                                    pPlCb->pstIpInfo[pIdx].stBufCnt.u32Total[port]--; \
                                                    if (buf->u32InternalFlags & VPP_BUF_FLAG_INTERNAL) \
                                                    { \
                                                        pPlCb->pstIpInfo[pIdx].stBufCnt.u32Internal[port]--; \
                                                    } \
                                                }

#define IP_LOG(LVL, pPlCb, idx, msg, ...) LOG##LVL("UC=%s, IP[%u]=%s: " msg, pPlCb->pstUcOpen->name, idx, IP_GET(pPlCb, idx).ip->name, ##__VA_ARGS__)

#define LOG_PL_CMD vVppPipeline_LogCmd

#define PRINT_UC(uc) uc, uc ? uc->name : "NULL"

enum {
    /*! The time taken to open the pipeline, including all submodules */
    PL_STAT_OPEN,
    /*! The time taken to close the pipeline, including all submodules */
    PL_STAT_CLOSE,
    /*! The time taken from when the first reconfigure request comes in to the
     * pipeline to the time that reconfigure done is issued. */
    PL_STAT_RCFG_LATENCY,
    /*! The time taken from when the drain done event comes from the last block
     * in the pipeline to the time that the reconfigure done is issued from
     * pipeline. */
    PL_STAT_RCFG_RAW,
};
/************************************************************************
 * Local static variables
 ***********************************************************************/

static const t_StVppStatsConfig astPlStatsCfg[] = {
    VPP_PROF_DECL(PL_STAT_OPEN, 1, 1),
    VPP_PROF_DECL(PL_STAT_CLOSE, 1, 1),
    VPP_PROF_DECL(PL_STAT_RCFG_LATENCY, 1, 1),
    VPP_PROF_DECL(PL_STAT_RCFG_RAW, 1, 1),
};

static const uint32_t u32PlStatCnt = VPP_STATS_CNT(astPlStatsCfg);

void dump_param(t_StVppParam *pstParam)
{
    LOGD("pstParam: flags=0x%x, input:{w=%u, h=%u, fmt=%u}, "
         "output:{w=%u, h=%u, fmt=%u}", pstParam->u32SessionFlags,
         pstParam->input_port.width, pstParam->input_port.height,
         pstParam->input_port.fmt, pstParam->output_port.width,
         pstParam->output_port.height, pstParam->output_port.fmt);
}

void dump_controls(t_StVppHqvCtrl *pstCtrl)
{
    LOGD("pstCtrl: mode=%u, ", pstCtrl->mode);
    LOG_CADE(D, &pstCtrl->cade);
    LOG_TNR(D, &pstCtrl->tnr);
    LOG_CNR(D, &pstCtrl->cnr);
    LOG_AIE(D, &pstCtrl->aie);
    LOG_DI(D, &pstCtrl->di);
    LOG_FRC(D, &pstCtrl->frc);
    LOG_EAR(D, &pstCtrl->ear);
    LOG_QBR(D, &pstCtrl->qbr);
}

void vVppPipeline_LogCmd(const char *pc, t_EVppPipelineCmd eCmd)
{

    switch (eCmd)
    {
        LOG_CASE(pc, VPP_PL_CMD_THREAD_EXIT);
        LOG_CASE(pc, VPP_PL_CMD_OPEN)
        LOG_CASE(pc, VPP_PL_CMD_CLOSE);
        LOG_CASE(pc, VPP_PL_CMD_FLUSH);
        LOG_CASE(pc, VPP_PL_CMD_DRAIN_REQUEST);
        LOG_CASE(pc, VPP_PL_CMD_DRAIN_COMPLETE);
        LOG_CASE(pc, VPP_PL_CMD_QUEUE_INTERNAL);
        LOG_CASE(pc, VPP_PL_CMD_STOP);
        LOG_CASE(pc, VPP_PL_CMD_MAX);
    }
}

/************************************************************************
 * Forward Declarations
 ************************************************************************/

/************************************************************************
 * Local Functions
 ***********************************************************************/

static void vVppPipeline_DumpBufStats(t_StVppPipelineCb *pstPlCb)
{
    uint32_t i;

    VPP_RET_VOID_IF_NULL(pstPlCb);

    LOGI("PL Buf Count: PL InQ %d, PL OutQ %d, PL IbdQ %d", u32VppBufPool_Cnt(&pstPlCb->stInputQ),
         u32VppBufPool_Cnt(&pstPlCb->stOutputQ), u32VppBufPool_Cnt(&pstPlCb->stIbdQ));
    for(i=0; i<pstPlCb->u32IpCnt; i++)
    {
        LOGI("PL Buf Count: IP[%d] In %d (%d Internal), Out %d (%d Internal)", i,
             pstPlCb->pstIpInfo[i].stBufCnt.u32Total[VPP_PORT_INPUT],
             pstPlCb->pstIpInfo[i].stBufCnt.u32Internal[VPP_PORT_INPUT],
             pstPlCb->pstIpInfo[i].stBufCnt.u32Total[VPP_PORT_OUTPUT],
             pstPlCb->pstIpInfo[i].stBufCnt.u32Internal[VPP_PORT_OUTPUT]);
    }
}

static void vVppPipeline_SetState(t_StVppPipelineCb *pstCb,
                                  t_EVppPipelineState eState)
{
    LOGI("pipeline state transition: %u to %u", pstCb->eState, eState);
    pstCb->eState = eState;
}

static uint32_t u32VppPipeline_FlushInternalBuffers(t_StVppPipelineCb *pstPlCb)
{
    uint32_t u32WaitFlags = 0;
    uint32_t u32Ip;
    uint32_t u32Ret, u32Err = VPP_OK;

    if (!pstPlCb)
        return VPP_ERR_PARAM;

    if (pstPlCb->u32IpCnt <= 1)
        return VPP_OK;

    pthread_mutex_lock(&pstPlCb->int_flush_mutex);
    pstPlCb->u32PipelineFlags |= PL_INTERNAL_FLUSH_PENDING;
    pthread_mutex_unlock(&pstPlCb->int_flush_mutex);

    for (u32Ip = 0; u32Ip < pstPlCb->u32IpCnt; u32Ip++)
    {
        if (u32Ip > 0)
        {
            pthread_mutex_lock(&pstPlCb->mutex);
            pstPlCb->pstIpInfo[u32Ip].u32PendingFlags |= PL_INPUT_FLUSH_PENDING;
            pthread_mutex_unlock(&pstPlCb->mutex);
            u32Ret = IP_FLUSH(pstPlCb, u32Ip, VPP_PORT_INPUT);
            if (u32Ret != VPP_OK)
            {
                LOGE("unable to flush input port on IP%u", u32Ip);
                pthread_mutex_lock(&pstPlCb->mutex);
                pstPlCb->pstIpInfo[u32Ip].u32PendingFlags &= ~PL_INPUT_FLUSH_PENDING;
                pthread_mutex_unlock(&pstPlCb->mutex);
            }
        }
        if (u32Ip < VPP_PL_LAST_IP_IDX(pstPlCb))
        {
            pthread_mutex_lock(&pstPlCb->mutex);
            pstPlCb->pstIpInfo[u32Ip].u32PendingFlags |= PL_OUTPUT_FLUSH_PENDING;
            pthread_mutex_unlock(&pstPlCb->mutex);
            u32Ret = IP_FLUSH(pstPlCb, u32Ip, VPP_PORT_OUTPUT);
            if (u32Ret != VPP_OK)
            {
                LOGE("unable to flush output port on IP%u", u32Ip);
                pthread_mutex_lock(&pstPlCb->mutex);
                pstPlCb->pstIpInfo[u32Ip].u32PendingFlags &= ~PL_OUTPUT_FLUSH_PENDING;
                pthread_mutex_unlock(&pstPlCb->mutex);
            }
        }
    }

    LOGI("waiting for flags to be cleared");
    for (u32Ip = 0; u32Ip < pstPlCb->u32IpCnt; u32Ip++)
    {
        pthread_mutex_lock(&pstPlCb->mutex);
        if (u32Ip == 0)
        {
            while (pstPlCb->pstIpInfo[u32Ip].u32PendingFlags & (PL_OUTPUT_FLUSH_PENDING))
            {
                pthread_cond_wait(&pstPlCb->cond_from_worker, &pstPlCb->mutex);
            }
        }
        else if (u32Ip == VPP_PL_LAST_IP_IDX(pstPlCb))
        {
            while (pstPlCb->pstIpInfo[u32Ip].u32PendingFlags & (PL_INPUT_FLUSH_PENDING))
            {
                pthread_cond_wait(&pstPlCb->cond_from_worker, &pstPlCb->mutex);
            }
        }
        else
        {
            while (pstPlCb->pstIpInfo[u32Ip].u32PendingFlags &
                   (PL_OUTPUT_FLUSH_PENDING | PL_INPUT_FLUSH_PENDING))
            {
                pthread_cond_wait(&pstPlCb->cond_from_worker, &pstPlCb->mutex);
            }
        }

        pthread_mutex_unlock(&pstPlCb->mutex);
        LOGI("Flush flag for IP%u cleared", u32Ip);
    }
    pthread_mutex_lock(&pstPlCb->mutex);
    pstPlCb->u32PipelineFlags &= ~PL_INTERNAL_FLUSH_PENDING;
    pthread_mutex_unlock(&pstPlCb->mutex);

    return u32Err;
}

static uint32_t u32VppPipeline_UnassociateInternalBuffers(t_StVppPipelineCb *pstPlCb,
                                                          uint32_t u32Ip)
{
    uint32_t i;
    t_StVppBuf *pstBuf;
    struct vpp_buffer *pstExtBuf;

    if (!pstPlCb || (u32Ip >= (VPP_PL_LAST_IP_IDX(pstPlCb))))
        return VPP_ERR_PARAM;

    for (i = 0; i < pstPlCb->pstInternalPool[u32Ip].u32AllocCnt; i++)
    {
        pstBuf = &pstPlCb->pstInternalPool[u32Ip].pstInternalBuf[i];
        pstExtBuf = &pstPlCb->pstInternalPool[u32Ip].pstVppExtBuf[i];

        // External buffer
        memset(&pstExtBuf->pixel, 0, sizeof(pstExtBuf->pixel));
        memset(&pstExtBuf->extradata, 0, sizeof(pstExtBuf->extradata));
        pstExtBuf->pvGralloc = NULL;

        // Internal buffer
        pstBuf->pBuf = NULL;
        memset(&pstBuf->stPixel, 0, sizeof(t_StVppMemBuf));
        memset(&pstBuf->stExtra, 0, sizeof(t_StVppMemBuf));
    }

    return VPP_OK;
}

static uint32_t u32VppPipeline_AssociateInternalBuffers(t_StVppPipelineCb *pstPlCb, uint32_t u32Ip)
{
    uint32_t i;
    t_StVppBuf *pstBuf;
    struct vpp_buffer *pstExtBuf;
    t_StVppIonBuf *pstIonPx, *pstIonEx;

    if (!pstPlCb || (u32Ip >= (VPP_PL_LAST_IP_IDX(pstPlCb))))
        return VPP_ERR_PARAM;

    for (i = 0; i < pstPlCb->pstInternalPool[u32Ip].u32AllocCnt; i++)
    {
        pstBuf = &pstPlCb->pstInternalPool[u32Ip].pstInternalBuf[i];
        pstExtBuf = &pstPlCb->pstInternalPool[u32Ip].pstVppExtBuf[i];
        pstIonPx = &pstPlCb->pstInternalPool[u32Ip].pstInternalAllocBuf[i].stPx;
        pstIonEx = &pstPlCb->pstInternalPool[u32Ip].pstInternalAllocBuf[i].stEx;

        // Fill out the external buffer structure
        pstExtBuf->pvGralloc = pstPlCb->pstInternalPool[u32Ip].pstInternalAllocBuf[i].pvGralloc;
        pstExtBuf->pixel.fd = pstIonPx->fd_ion_mem;
        pstExtBuf->pixel.offset = 0;
        pstExtBuf->pixel.alloc_len = pstIonPx->len;
        pstExtBuf->pixel.filled_len = 0;
        pstExtBuf->pixel.valid_data_len = pstIonPx->len;

        if (pstPlCb->pstInternalPool[u32Ip].u32AllocExSize)
        {
            pstExtBuf->extradata.fd = pstIonEx->fd_ion_mem;
            pstExtBuf->extradata.offset = 0;
            pstExtBuf->extradata.alloc_len = pstIonEx->len;
            pstExtBuf->extradata.filled_len = 0;
            pstExtBuf->extradata.valid_data_len = pstIonEx->len;
        }

        // Internal buffer structure
        pstBuf->pBuf = pstExtBuf;
        pstBuf->eBufPxType = eVppBufPxDataType_Raw;
        pstBuf->u32InternalPool = u32Ip;
        pstBuf->u32InternalFlags = VPP_BUF_FLAG_INTERNAL;

        pstBuf->stPixel.pvBase = pstIonPx->buf;
        pstBuf->stPixel.eMapping = eVppBuf_MappedInternal;
        pstBuf->stPixel.fd = pstExtBuf->pixel.fd;
        pstBuf->stPixel.u32AllocLen = pstExtBuf->pixel.alloc_len;
        pstBuf->stPixel.u32FilledLen = pstExtBuf->pixel.filled_len;
        pstBuf->stPixel.u32Offset = pstExtBuf->pixel.offset;
        pstBuf->stPixel.u32ValidLen = pstExtBuf->pixel.valid_data_len;
        pstBuf->stPixel.u32MappedLen = pstExtBuf->pixel.alloc_len;

        if (VPP_FLAG_IS_SET(pstPlCb->pstParam->u32SessionFlags, VPP_SESSION_SECURE))
        {
            pstBuf->stPixel.pvBase = VPP_BUF_UNMAPPED_BUF_VAL;
            pstBuf->stPixel.eMapping = eVppBuf_Unmapped;
            pstBuf->stPixel.u32MappedLen = 0;
        }

        if (pstPlCb->pstInternalPool[u32Ip].u32AllocExSize)
        {
            pstBuf->stExtra.pvBase = pstIonEx->buf;
            pstBuf->stPixel.eMapping = eVppBuf_MappedInternal;
            pstBuf->stExtra.fd = pstExtBuf->extradata.fd;
            pstBuf->stExtra.u32AllocLen = pstExtBuf->extradata.alloc_len;
            pstBuf->stExtra.u32FilledLen = pstExtBuf->extradata.filled_len;
            pstBuf->stExtra.u32Offset = pstExtBuf->extradata.offset;
            pstBuf->stExtra.u32ValidLen = pstExtBuf->extradata.valid_data_len;
            pstBuf->stExtra.u32MappedLen = pstExtBuf->extradata.alloc_len;
        }

        LOGI("linked: Ip[%u]:%u pstBuf=%p, pstExtBuf=%p, pstIonPx=%p, pstIonEx=%p, pvGralloc %p",
             u32Ip, i, pstBuf, pstExtBuf, pstIonPx, pstIonEx, pstExtBuf->pvGralloc);
    }

    return VPP_OK;
}

static uint32_t u32VppPipeline_UnprepareInternalBufferPool(t_StVppPipelineCb *pstPlCb,
                                                           uint32_t u32Ip)
{

    t_StVppBuf *pstBuf;
    uint32_t u32Cnt = 0;
    uint32_t u32Ret = VPP_OK;

    if (!pstPlCb || (u32Ip >= (VPP_PL_LAST_IP_IDX(pstPlCb))))
        return VPP_ERR_PARAM;

    pthread_mutex_lock(&pstPlCb->mutex);

    while ((pstBuf = pstVppBufPool_Get(&pstPlCb->pstInternalPool[u32Ip].stInternalBufQ)) != NULL)
    {
        LOGI("retrieved pstBuf=%p from internal pool %u", pstBuf, u32Ip);
        u32Cnt++;
    }

    if (u32Cnt != pstPlCb->pstInternalPool[u32Ip].u32AllocCnt)
    {
        pthread_mutex_unlock(&pstPlCb->mutex);
        LOGD("unexpected buffer count from internal pool %u, expected=%u, actual=%u. Flushing",
             u32Ip, pstPlCb->pstInternalPool[u32Ip].u32AllocCnt, u32Cnt);
        u32VppPipeline_FlushInternalBuffers(pstPlCb);
        pthread_mutex_lock(&pstPlCb->mutex);
        while ((pstBuf = pstVppBufPool_Get(&pstPlCb->pstInternalPool[u32Ip].stInternalBufQ)) != NULL)
        {
            LOGI("retrieved pstBuf=%p from internal pool %u", pstBuf, u32Ip);
            u32Cnt++;
        }
        if (u32Cnt != pstPlCb->pstInternalPool[u32Ip].u32AllocCnt)
        {
            LOGE("Missing buffers from internal pool %u, expected=%u, actual=%u.",
                 u32Ip, pstPlCb->pstInternalPool[u32Ip].u32AllocCnt, u32Cnt);
            u32Ret = VPP_ERR;
        }
    }

    pthread_mutex_unlock(&pstPlCb->mutex);

    return u32Ret;
}

static uint32_t u32VppPipeline_PrepareInternalBufferPool(t_StVppPipelineCb *pstPlCb, uint32_t u32Ip)
{
    uint32_t i;
    uint32_t u32Ret = VPP_OK;
    t_StVppBuf *pstBuf;

    if (!pstPlCb || (u32Ip >= (VPP_PL_LAST_IP_IDX(pstPlCb))))
        return VPP_ERR_PARAM;

    pthread_mutex_lock(&pstPlCb->mutex);

    for (i = 0; i < pstPlCb->pstInternalPool[u32Ip].u32AllocCnt; i++)
    {
        u32Ret = u32VppBufPool_Put(&pstPlCb->pstInternalPool[u32Ip].stInternalBufQ,
                                   &pstPlCb->pstInternalPool[u32Ip].pstInternalBuf[i]);
        if (u32Ret != VPP_OK)
        {
            LOGE("unable to put buffer into internal pool, u32Ret=%u", u32Ret);
            while ((pstBuf = pstVppBufPool_Get(&pstPlCb->pstInternalPool[u32Ip].stInternalBufQ)) != NULL)
                LOGI("retrieved pstBuf=%p from pool", pstBuf);
            break;
        }
    }

    pthread_mutex_unlock(&pstPlCb->mutex);

    return u32Ret;
}

static uint32_t u32VppPipeline_FreeInternalBufferMem(t_StVppCtx *pstCb,
                                                     t_StVppPipelineCb *pstPlCb, uint32_t u32Ip)
{
    uint32_t i, u32Ret, u32Err = VPP_OK;

    if (!pstCb || !pstPlCb || (u32Ip >= (VPP_PL_LAST_IP_IDX(pstPlCb))))
        return VPP_ERR_PARAM;

    pthread_mutex_lock(&pstPlCb->mutex);

    for (i = 0; i < pstPlCb->pstInternalPool[u32Ip].u32AllocCnt; i++)
    {
        u32Ret = u32VppIon_Free(pstCb,
                             &pstPlCb->pstInternalPool[u32Ip].pstInternalAllocBuf[i].stPx);
        if (u32Ret != VPP_OK)
        {
            LOGE("unable to ion free ion px at Ip=%u, i=%u, u32Ret=%u", u32Ip, i, u32Ret);
            u32Err = VPP_ERR;
        }

        if (pstPlCb->pstInternalPool[u32Ip].u32AllocExSize != 0)
        {
            u32Ret = u32VppIon_Free(pstCb,
                                 &pstPlCb->pstInternalPool[u32Ip].pstInternalAllocBuf[i].stEx);
            if (u32Ret != VPP_OK)
            {
                LOGE("unable to ion free ion ex at Ip=%u, i=%u, u32Ret=%u", u32Ip, i, u32Ret);
                u32Err = VPP_ERR;
            }
        }

        if (pstPlCb->pstInternalPool[u32Ip].pstInternalAllocBuf[i].pvGralloc)
            vVppBuf_GrallocFree(pstPlCb->pstInternalPool[u32Ip].pstInternalAllocBuf[i].pvGralloc);
    }

    pstPlCb->pstInternalPool[u32Ip].u32AllocPxSize= 0;
    pstPlCb->pstInternalPool[u32Ip].u32AllocExSize= 0;
    pstPlCb->pstInternalPool[u32Ip].u32AllocCnt = 0;

    pthread_mutex_unlock(&pstPlCb->mutex);
    return u32Err;
}

static uint32_t u32VppPipeline_AllocInternalBufferMem(t_StVppCtx *pstCb, t_StVppPipelineCb *pstPlCb,
                                                      uint32_t u32Ip)
{
    uint32_t u32Ret, i;
    uint32_t u32Px, u32Ex, u32Gr, u32BufSz, u32ExBufSz, u32Cnt;
    uint32_t u32Secure = 0;

    if (!pstCb || !pstPlCb || (u32Ip >= (VPP_PL_LAST_IP_IDX(pstPlCb))))
        return VPP_ERR_PARAM;

    if (VPP_FLAG_IS_SET(pstPlCb->pstParam->u32SessionFlags, VPP_SESSION_SECURE))
        u32Secure = 1;

    // Allocate the same extradata size for all internal pools and ignore IP requirements
    u32ExBufSz = u32VppUtils_GetExtraDataBufferSize(&pstPlCb->pstParam->input_port);

    u32BufSz = pstPlCb->pstIpInfo[u32Ip].stOutBufReq.u32PxSz;
    u32Cnt = pstPlCb->pstInternalPool[u32Ip].u32MinReqCnt;

    for (u32Px = 0; u32Px < u32Cnt; u32Px++)
    {
        u32Ret = u32VppIon_Alloc(pstCb,
                                 u32BufSz,
                                 u32Secure,
                                 &pstPlCb->pstInternalPool[u32Ip].pstInternalAllocBuf[u32Px].stPx);
        if (u32Ret != VPP_OK)
        {
            LOGE("unable to ion allocate px buffers, Ip=%u, u32Px=%u, u32Secure=%u, u32Ret=%u",
                 u32Ip, u32Px, u32Secure, u32Ret);
            for (; u32Px; u32Px--)
            {
                i = u32Px - 1;
                u32Ret = u32VppIon_Free(pstCb,
                                        &pstPlCb->pstInternalPool[u32Ip].pstInternalAllocBuf[i].stPx);
                if (!u32Ret)
                    LOGE("unable to ion free internal px buffer at Ip=%u i=%u, u32Ret=%u",
                         u32Ip, i, u32Ret);
            }
            return VPP_ERR_NO_MEM;
        }
    }

    pstPlCb->pstInternalPool[u32Ip].u32AllocExSize = u32ExBufSz;
    if (u32ExBufSz > 0)
    {
        for (u32Ex = 0; u32Ex < u32Cnt; u32Ex++)
        {
            u32Ret = u32VppIon_Alloc(pstCb,
                                     u32ExBufSz,
                                     0,
                                     &pstPlCb->pstInternalPool[u32Ip].pstInternalAllocBuf[u32Ex].stEx);
            if (u32Ret != VPP_OK)
            {
                LOGE("unable to ion allocate ex buffers, Ip=%u, u32Ex=%u, u32Ret=%u",
                     u32Ip, u32Ex, u32Ret);
                for (; u32Ex; u32Ex--)
                {
                    i = u32Ex - 1;
                    u32Ret = u32VppIon_Free(pstCb,
                                            &pstPlCb->pstInternalPool[u32Ip].pstInternalAllocBuf[i].stEx);
                    if (!u32Ret)
                        LOGE("unable to ion free internal ex buffer at Ip=%u i=%u, u32Ret=%u",
                             u32Ip, i, u32Ret);
                }
                // Not considered a fatal error if extradata allocation fails, so just continue
                pstPlCb->pstInternalPool[u32Ip].u32AllocExSize = 0;
                break;
            }
        }
    }

    for (u32Gr = 0; u32Gr < u32Cnt; u32Gr++)
    {
        pstPlCb->pstInternalPool[u32Ip].pstInternalAllocBuf[u32Gr].pvGralloc =
            pvVppBuf_GrallocAllocate();
        if (!pstPlCb->pstInternalPool[u32Ip].pstInternalAllocBuf[u32Gr].pvGralloc)
        {
            LOGE("unable to allocate gralloc buffers, Ip=%u, u32Pv=%u",
                 u32Ip, u32Gr);
            for (; u32Gr; u32Gr--)
            {
                i = u32Gr - 1;
                if (pstPlCb->pstInternalPool[u32Ip].pstInternalAllocBuf[i].pvGralloc)
                {
                    vVppBuf_GrallocFree(pstPlCb->pstInternalPool[u32Ip].pstInternalAllocBuf[i].pvGralloc);
                    pstPlCb->pstInternalPool[u32Ip].pstInternalAllocBuf[i].pvGralloc = NULL;
                }
            }
            // Not considered a fatal error if gralloc allocation fails, so just continue
            break;
        }
    }
    pstPlCb->pstInternalPool[u32Ip].u32AllocPxSize = u32BufSz;
    pstPlCb->pstInternalPool[u32Ip].u32AllocCnt = u32Cnt;

    return VPP_OK;
}

static uint32_t u32VppPipeline_FreeInternalBufStructs(t_StVppPipelineCb *pstPlCb, uint32_t u32Ip)
{

    if (!pstPlCb || (u32Ip >= (VPP_PL_LAST_IP_IDX(pstPlCb))))
        return VPP_ERR_PARAM;

    pthread_mutex_lock(&pstPlCb->mutex);

    if (pstPlCb->pstInternalPool[u32Ip].pstInternalBuf)
        free(pstPlCb->pstInternalPool[u32Ip].pstInternalBuf);

    if (pstPlCb->pstInternalPool[u32Ip].pstVppExtBuf)
        free(pstPlCb->pstInternalPool[u32Ip].pstVppExtBuf);

    if (pstPlCb->pstInternalPool[u32Ip].pstInternalAllocBuf)
        free(pstPlCb->pstInternalPool[u32Ip].pstInternalAllocBuf);

    pstPlCb->pstInternalPool[u32Ip].pstInternalBuf = NULL;
    pstPlCb->pstInternalPool[u32Ip].pstVppExtBuf = NULL;
    pstPlCb->pstInternalPool[u32Ip].pstInternalAllocBuf = NULL;
    pstPlCb->pstInternalPool[u32Ip].u32MinReqCnt = 0;

    pthread_mutex_unlock(&pstPlCb->mutex);

    return VPP_OK;
}

static uint32_t u32VppPipeline_AllocInternalBufStructs(t_StVppPipelineCb *pstPlCb, uint32_t u32Ip)
{
    t_StVppIpInternalReqs stReq;
    uint32_t u32BufAllocMinCnt, u32Ret;

    if (!pstPlCb || !pstPlCb->pstInternalPool || (u32Ip >= VPP_PL_LAST_IP_IDX(pstPlCb)))
        return VPP_ERR_PARAM;

    u32Ret = u32VppUsecase_GetInternalBufReq(pstPlCb->pstUcOpen, pstPlCb->pstParam, u32Ip,
                                             &stReq);
    if (u32Ret != VPP_OK)
    {
        LOGE("Error getting buffer requirements for IP%u, u32Ret=%u", u32Ip, u32Ret);
        return u32Ret;
    }

    if (stReq.u32InternalOutBufReq == 0)
    {
        LOGI("IP%u needs external buffers, not allocating internal", u32Ip);
        pstPlCb->pstInternalPool[u32Ip].u32MinReqCnt = 0;
        return VPP_OK;
    }
    // Allocate the greater of use case request or IP request
    u32BufAllocMinCnt = (stReq.u32InternalOutBufReq > pstPlCb->pstIpInfo[u32Ip].stOutBufReq.u32MinCnt) ?
        stReq.u32InternalOutBufReq : pstPlCb->pstIpInfo[u32Ip].stOutBufReq.u32MinCnt;

    LOGI("%s: IP[%u]: u32MinReqCnt %u, eBufType %u", __func__, u32Ip,
         u32BufAllocMinCnt, stReq.eOutBufType);
    pstPlCb->pstInternalPool[u32Ip].u32MinReqCnt = u32BufAllocMinCnt;
    pstPlCb->pstInternalPool[u32Ip].eBufType = stReq.eOutBufType;
    pstPlCb->pstInternalPool[u32Ip].pstInternalBuf =
        (t_StVppBuf *)calloc(u32BufAllocMinCnt, sizeof(t_StVppBuf));
    if (!pstPlCb->pstInternalPool[u32Ip].pstInternalBuf)
    {
        LOGE("unable to allocate pstInternalBuf");
        goto ERR_ALLOC;
    }

    pstPlCb->pstInternalPool[u32Ip].pstVppExtBuf =
        (struct vpp_buffer *)calloc(u32BufAllocMinCnt, sizeof(struct vpp_buffer));
    if (!pstPlCb->pstInternalPool[u32Ip].pstVppExtBuf)
    {
        LOGE("unable to allocate pstVppExtBuf");
        goto ERR_ALLOC;
    }

    pstPlCb->pstInternalPool[u32Ip].pstInternalAllocBuf =
        (t_StVppPipelineInternalBuffers *)calloc(u32BufAllocMinCnt,
         sizeof(t_StVppPipelineInternalBuffers));
    if (!pstPlCb->pstInternalPool[u32Ip].pstInternalAllocBuf)
    {
        LOGE("unable to allocate pstInternalAllocBuf");
        goto ERR_ALLOC;
    }

    return VPP_OK;

ERR_ALLOC:
    u32VppPipeline_FreeInternalBufStructs(pstPlCb, u32Ip);
    return VPP_ERR_NO_MEM;
}

static uint32_t u32VppPipeline_FreeIpInternalBuffers(t_StVppCtx *pstCb, uint32_t u32Ip)
{
    t_StVppPipelineCb *pstPlCb;
    uint32_t u32Ret;
    uint32_t u32Err = VPP_OK;

    if (!pstCb || !pstCb->pstPlCb || (u32Ip >= (VPP_PL_LAST_IP_IDX(pstCb->pstPlCb))))
        return VPP_ERR_PARAM;

    pstPlCb = pstCb->pstPlCb;

    u32Ret = u32VppPipeline_UnprepareInternalBufferPool(pstPlCb, u32Ip);
    if (u32Ret != VPP_OK)
    {
        u32Err = VPP_ERR;
        LOGE("unable to unprepare internal buffers for IP%u, u32Ret=%u", u32Ip, u32Ret);
    }

    u32Ret = u32VppPipeline_UnassociateInternalBuffers(pstPlCb, u32Ip);
    if (u32Ret != VPP_OK)
    {
        u32Err = VPP_ERR;
        LOGE("unable to unassociate internal buffers for IP%u, u32Ret=%u", u32Ip, u32Ret);
    }

    u32Ret = u32VppPipeline_FreeInternalBufferMem(pstCb, pstPlCb, u32Ip);
    if (u32Ret != VPP_OK)
    {
        u32Err = VPP_ERR;
        LOGE("unable to free internal ion buffers for IP%u, u32Ret=%u", u32Ip, u32Ret);
    }

    u32Ret = u32VppPipeline_FreeInternalBufStructs(pstPlCb, u32Ip);
    if (u32Ret != VPP_OK)
    {
        u32Err = VPP_ERR;
        LOGE("unable to free internal buffer structures for IP%u, u32Ret=%u", u32Ip, u32Ret);
    }

    return u32Err;
}

static uint32_t u32VppPipeline_FreeInternalBuffers(t_StVppCtx *pstCb)
{
    t_StVppPipelineCb *pstPlCb;
    uint32_t u32Ip, u32Ret;
    uint32_t u32Err = VPP_OK;

    if (!pstCb || !pstCb->pstPlCb)
        return VPP_ERR_PARAM;

    pstPlCb = pstCb->pstPlCb;

    for (u32Ip = 0; u32Ip < VPP_PL_LAST_IP_IDX(pstPlCb); u32Ip++)
    {
        u32Ret = u32VppPipeline_FreeIpInternalBuffers(pstCb, u32Ip);
        if (u32Ret != VPP_OK)
        {
            u32Err = VPP_ERR;
            LOGE("unable to free internal buffers for IP%u, u32Ret=%u", u32Ip, u32Ret);
        }
    }

    pthread_mutex_lock(&pstPlCb->mutex);
    if (pstPlCb->pstInternalPool)
        free(pstPlCb->pstInternalPool);

    pstPlCb->pstInternalPool = NULL;
    pthread_mutex_unlock(&pstPlCb->mutex);

    return u32Err;
}

static uint32_t u32VppPipeline_AllocateIpInternalBuffers(t_StVppCtx *pstCb, uint32_t u32Ip)
{
    t_StVppPipelineCb *pstPlCb;
    uint32_t u32Ret;

    if (!pstCb || !pstCb->pstPlCb || (u32Ip >= (VPP_PL_LAST_IP_IDX(pstCb->pstPlCb))))
        return VPP_ERR_PARAM;

    pstPlCb = pstCb->pstPlCb;

    u32Ret = u32VppPipeline_AllocInternalBufStructs(pstPlCb, u32Ip);
    if (u32Ret != VPP_OK)
    {
        LOGE("unable to allocate internal buffer structures for IP%u, u32Ret=%u", u32Ip, u32Ret);
        goto ERR_ALLOC;
    }

    u32Ret = u32VppPipeline_AllocInternalBufferMem(pstCb, pstPlCb, u32Ip);
    if (u32Ret != VPP_OK)
    {
        LOGE("unable to ion allocate internal buffers for IP%u, u32Ret=%u", u32Ip, u32Ret);
        goto ERR_ALLOC;
    }

    u32Ret = u32VppPipeline_AssociateInternalBuffers(pstPlCb, u32Ip);
    if (u32Ret != VPP_OK)
    {
        LOGE("unable link internal buffers for IP%u, u32Ret=%u", u32Ip, u32Ret);
        goto ERR_ALLOC;
    }

    u32Ret = u32VppPipeline_PrepareInternalBufferPool(pstPlCb, u32Ip);
    if (u32Ret != VPP_OK)
    {
        LOGE("unable prepare internal pool for IP%u, u32Ret=%u", u32Ip, u32Ret);
        goto ERR_ALLOC;
    }
    return VPP_OK;

ERR_ALLOC:
    u32VppPipeline_FreeIpInternalBuffers(pstCb, u32Ip);
    return VPP_ERR;

}

static uint32_t u32VppPipeline_AllocateInternalBuffers(t_StVppCtx *pstCb)
{
    uint32_t u32Min, u32Ip, u32Ret;
    t_StVppPipelineCb *pstPlCb;

    if (!pstCb || !pstCb->pstPlCb || (pstCb->pstPlCb->u32IpCnt == 0))
        return VPP_ERR_PARAM;

    pstPlCb = pstCb->pstPlCb;

    pstPlCb->pstInternalPool = (t_StVppPipelineInternalBufPool *)
        calloc(VPP_PL_LAST_IP_IDX(pstPlCb), sizeof(t_StVppPipelineInternalBufPool));
    if (!pstPlCb->pstInternalPool)
    {
        LOGE("unable to allocate pstInternalPool");
        return VPP_ERR_NO_MEM;
    }

    for (u32Ip = 0; u32Ip < VPP_PL_LAST_IP_IDX(pstPlCb); u32Ip++)
    {
        u32Ret = u32VppPipeline_AllocateIpInternalBuffers(pstCb, u32Ip);
        if (u32Ret != VPP_OK)
        {
            u32VppPipeline_FreeInternalBuffers(pstCb);
            return u32Ret;
        }
    }
    return VPP_OK;
}

static uint32_t u32VppPipeline_ReconfigInternalBuffers(t_StVppCtx *pstCb)
{
    uint32_t u32Ip, u32BufSz, u32ExBufSz;
    uint32_t u32UcMinCnt, u32BufAllocMinCnt;
    t_StVppPipelineCb *pstPlCb;
    t_StVppIpInternalReqs stReq;
    uint32_t bFlushed = VPP_FALSE;
    uint32_t u32Ret = VPP_OK;

    if (!pstCb || !pstCb->pstPlCb)
        return VPP_ERR_PARAM;

    pstPlCb = pstCb->pstPlCb;

    for (u32Ip = 0; u32Ip < VPP_PL_LAST_IP_IDX(pstPlCb); u32Ip++)
    {
        u32BufSz = pstPlCb->pstIpInfo[u32Ip].stOutBufReq.u32PxSz;
        u32ExBufSz = pstPlCb->pstIpInfo[u32Ip].stOutBufReq.u32ExSz;

        u32Ret = u32VppUsecase_GetInternalBufReq(pstPlCb->pstUcOpen, pstPlCb->pstParam, u32Ip,
                                                 &stReq);
        if (u32Ret != VPP_OK)
        {
            LOGE("Error getting buffer requirements for IP%u, u32Ret=%u", u32Ip, u32Ret);
            return u32Ret;
        }
        u32UcMinCnt = stReq.u32InternalOutBufReq;
        u32BufAllocMinCnt = (u32UcMinCnt > pstPlCb->pstIpInfo[u32Ip].stOutBufReq.u32MinCnt) ?
            u32UcMinCnt : pstPlCb->pstIpInfo[u32Ip].stOutBufReq.u32MinCnt;
        pstPlCb->pstInternalPool[u32Ip].u32MinReqCnt = u32BufAllocMinCnt;

        if ((u32BufAllocMinCnt > pstPlCb->pstInternalPool[u32Ip].u32AllocCnt) ||
            (u32BufSz > pstPlCb->pstInternalPool[u32Ip].u32AllocPxSize) ||
            (u32ExBufSz > pstPlCb->pstInternalPool[u32Ip].u32AllocExSize))
        {
            if (bFlushed == VPP_FALSE)
            {
                u32Ret = u32VppPipeline_FlushInternalBuffers(pstPlCb);
                if (u32Ret != VPP_OK)
                {
                    LOGE("Could not flush internal buffers for IP%u, u32Ret=%u", u32Ip, u32Ret);
                    return u32Ret;
                }
                bFlushed = VPP_TRUE;
            }
            LOGI("Reallocating internal buffers for IP%u", u32Ip);
            u32Ret = u32VppPipeline_FreeIpInternalBuffers(pstCb, u32Ip);
            if (u32Ret != VPP_OK)
            {
                LOGE("Could not free internal buffers for IP%u, u32Ret=%u", u32Ip, u32Ret);
                return u32Ret;
            }
            u32Ret = u32VppPipeline_AllocateIpInternalBuffers(pstCb, u32Ip);
            if (u32Ret != VPP_OK)
            {
                LOGE("Could not reallocate internal buffers for IP%u, u32Ret=%u", u32Ip, u32Ret);
                return u32Ret;
            }
            if (VPP_PL_STATE_EQUAL(pstPlCb, VPP_PLSTATE_ACTIVE))
                vVppPipeline_SetState(pstPlCb, VPP_PLSTATE_PENDING_ACTIVE);
        }
    }

    return u32Ret;
}

static uint32_t u32VppPipeline_QueueInternalBuffers(t_StVppCtx *pstCb)
{
    t_StVppBuf *pstBuf;
    uint32_t u32Ip;
    uint32_t u32Ret = VPP_OK;
    uint32_t u32, u32Err;
    t_StVppPipelineCb *pstPlCb;

    if (!pstCb || !pstCb->pstPlCb)
        return VPP_ERR_PARAM;

    pstPlCb = pstCb->pstPlCb;

    if (!pstPlCb->pstInternalPool)
    {
        LOGE("Internal pool is NULL");
        return VPP_ERR_PARAM;
    }

    for (u32Ip = 0; u32Ip < VPP_PL_LAST_IP_IDX(pstPlCb); u32Ip++)
    {
        while ((pstBuf = pstVppBufPool_Get(&pstPlCb->pstInternalPool[u32Ip].stInternalBufQ)) != NULL)
        {
            pstBuf->u32InternalFlags = VPP_BUF_FLAG_INTERNAL;
            pstBuf->stPixel.u32FilledLen = 0;
            pstBuf->stExtra.u32FilledLen = 0;
            pstBuf->eQueuedPort = VPP_PORT_OUTPUT;
            pstBuf->eCurrentIpPort = VPP_PORT_OUTPUT;
            pthread_mutex_lock(&pstPlCb->mutex);
            BUF_CNT_QUEUE(pstPlCb, u32Ip, VPP_PORT_OUTPUT, pstBuf);
            pthread_mutex_unlock(&pstPlCb->mutex);
            u32 = IP_QUEUE_BUF(pstPlCb, u32Ip, VPP_PORT_OUTPUT, pstBuf);
            if (u32 != VPP_OK)
            {
                LOGE("unable to queue internal buffer, u32=%u", u32);
                pthread_mutex_lock(&pstPlCb->mutex);
                BUF_CNT_DEQUEUE(pstPlCb, u32Ip, VPP_PORT_OUTPUT, pstBuf);
                u32VppBufPool_Put(&pstPlCb->pstInternalPool[u32Ip].stInternalBufQ, pstBuf);
                pthread_mutex_unlock(&pstPlCb->mutex);
                u32Ret = VPP_ERR;
                break;
            }
        }
        if (u32Ret != VPP_OK)
            break;
    }

    if (u32Ret != VPP_OK)
    {
        u32Err = u32VppPipeline_FlushInternalBuffers(pstPlCb);
        if (u32Err != VPP_OK)
            LOGE("unable to flush internal buffers after queue error, u32Err=%u", u32Err);
        if (VPP_PL_STATE_EQUAL(pstPlCb, VPP_PLSTATE_ACTIVE))
            vVppPipeline_SetState(pstPlCb, VPP_PLSTATE_PENDING_ACTIVE);
    }

    return u32Ret;
}

static void vVppPipeline_FlushIbdQueue(t_StVppCtx *pstCb)
{
    t_StVppPipelineCb *pstPlCb;
    t_StVppBuf *pstBuf;

    if (!pstCb || !pstCb->pstPlCb)
        return;

    pstPlCb = pstCb->pstPlCb;

    pthread_mutex_lock(&pstPlCb->mutex);
    while ((pstBuf = pstVppBufPool_Get(&pstPlCb->stIbdQ)) != NULL)
    {
        VPP_STATS(pstPlCb, IssueIBD);
        VPP_LOG_BUF(pstCb, pstBuf, eVppLogId_PlBufDone);
        u32VppIp_CbBufDone(&pstPlCb->stCallbacks, VPP_PORT_INPUT, pstBuf);
    }
    pthread_mutex_unlock(&pstPlCb->mutex);
}

static uint32_t u32VppPipeline_FindOutputBufDest(t_StVppPipelineCb *pstPlCb)
{
    uint32_t u32OutOver = 0;
    uint32_t u32InOver = 0;
    uint32_t bDestFound = 0;
    uint32_t u32Dest = 0;
    uint32_t u32IpCnt, u32ReqOut, u32InCnt, u32OutCnt, u32OutOverTemp, u32InOverTemp, i;

    if (!pstPlCb)
        return 0;

    u32IpCnt = pstPlCb->u32IpCnt;

    if (u32IpCnt == 1)
        return 0;

    u32Dest = u32IpCnt - 1; // Default queue to last IP

    for (i = u32IpCnt; i-- > 0;)
    {
        u32InCnt = pstPlCb->pstIpInfo[i].stBufCnt.u32Total[VPP_PORT_INPUT];
        u32OutCnt = pstPlCb->pstIpInfo[i].stBufCnt.u32Total[VPP_PORT_OUTPUT];
        if (i == u32IpCnt - 1)
        {
            // Last IP
            u32ReqOut = pstPlCb->pstIpInfo[i].stOutBufReq.u32MinCnt;
            if (u32OutCnt >= u32ReqOut)
                u32OutOver = u32OutCnt - u32ReqOut;
        }
        else
        {
            if (pstPlCb->pstInternalPool[i].u32MinReqCnt != 0)
                continue;
            // IP doesn't use internal pool, needs external buffers
            u32ReqOut = pstPlCb->pstIpInfo[i].stOutBufReq.u32MinCnt +
                pstPlCb->pstIpInfo[i + 1].stInBufReq.u32MinCnt;
        }
        if ((u32InCnt >= pstPlCb->pstIpInfo[i].stInBufReq.u32MinCnt) &&
            (u32OutCnt < pstPlCb->pstIpInfo[i].stOutBufReq.u32MinCnt))
        {
            // Give buffer to last IP that is starved
            u32Dest = i;
            break;
        }
        else
        {
            if (u32OutCnt < u32ReqOut)
            {
                // Give preference to first IP that doesn't have enough buffers
                u32Dest = i;
                bDestFound = 1;
            }
            else
            {
                if (bDestFound == 0)
                {
                    if (u32InCnt > u32OutCnt)
                    {
                        u32InOverTemp = u32InCnt - u32OutCnt;
                        if (u32InOverTemp > u32InOver)
                        {
                            u32InOver = u32InOverTemp;
                            u32Dest = i;
                            continue;
                        }
                    }
                    if (u32InOver == 0)
                    {
                        // Otherwise queue to IP that exceeds the min by the least
                        u32OutOverTemp = u32OutCnt - u32ReqOut;
                        if (u32OutOverTemp <= u32OutOver)
                        {
                            u32OutOver = u32OutOverTemp;
                            u32Dest = i;
                        }
                    }
                }
            }
        }
    }
    return u32Dest;
}

// TODO: refactor these two functions (CmdGet/CmdPut) between this and
// GPU so that code gets reused...
static uint32_t u32VppPipeline_CmdGet(t_StVppPipelineCb *pstPlCb,
                                      t_StVppPipelineCmd *pstCmd)
{
    LOGI("%s()", __func__);
    int32_t idx;
    idx = vpp_queue_dequeue(&pstPlCb->stCmdQ);

    if (idx < 0)
    {
        return VPP_ERR;
    }
    else
    {
        *pstCmd = pstPlCb->astCmdNode[idx];
        LOG_PL_CMD("PL:GetCmd", pstCmd->eCmd);
    }

    return VPP_OK;
}

static uint32_t u32VppPipeline_CmdPut(t_StVppPipelineCb *pstPlCb,
                                      t_StVppPipelineCmd stCmd)
{
    int32_t idx;
    uint32_t u32Ret = VPP_OK;

    pthread_mutex_lock(&pstPlCb->mutex);

    LOG_PL_CMD("PL:InsertCmd", stCmd.eCmd);
    idx = vpp_queue_enqueue(&pstPlCb->stCmdQ);
    if (idx < 0)
    {
        u32Ret = VPP_ERR;
    } else
    {
        pstPlCb->astCmdNode[idx] = stCmd;
        pthread_cond_signal(&pstPlCb->cond_to_worker);
    }

    pthread_mutex_unlock(&pstPlCb->mutex);

    return u32Ret;
}

static uint32_t u32VppPipeline_HandleDrainDone(t_StVppCtx *pstCtx,
                                               t_StVppPipelineCb *pstPlCb,
                                               uint32_t u32IpPos)
{
    // This is issued from an IP block's callback. We need to put this onto the
    // Pipeline worker thread, because when the VPP interface layer calls into
    // here, it may cause for IP blocks to get torn down (if the new UC has
    // different composition from the current one, for example).

    uint32_t u32Ret = VPP_OK;
    VPP_UNUSED(pstCtx);

    t_StVppPipelineCmd stCmd;

    memset(&stCmd, 0, sizeof(t_StVppPipelineCmd));
    stCmd.eCmd = VPP_PL_CMD_DRAIN_COMPLETE;

    VPP_STATS(pstPlCb, DrainDoneCb);

    if (u32IpPos == VPP_PL_LAST_IP_IDX(pstPlCb))
    {
        u32VppPipeline_CmdPut(pstPlCb, stCmd);
    }
    else
    {
        u32Ret = IP_DRAIN(pstPlCb, u32IpPos + 1);
        if (u32Ret != VPP_OK)
        {
            IP_LOG(E, pstPlCb, 0, "IP_DRAIN returned u32Ret=%u", u32Ret);
            VPP_FLAG_CLR(pstPlCb->u32PipelineFlags, PL_RECONFIGURE_PENDING);
        }
    }

    return u32Ret;
}

static void vVppPipeline_ReturnIbdBufs(t_StVppPipelineCb *pstCb, t_StVppPipelineCbCookie *pstCookie,
                                       t_StVppBuf *pstBuf, enum vpp_port ePort)
{
    t_StVppBuf *pstIbdBuf;

    if (!pstCb || !pstCookie || !pstBuf || (ePort >= VPP_PORT_MAX))
        return;

    pstIbdBuf = pstVppBufPool_Peek(&pstCb->stIbdQ, 0);
    while ((pstIbdBuf) && (pstIbdBuf->pBuf->timestamp <= pstBuf->pBuf->timestamp))
    {
        pthread_mutex_lock(&pstCb->mutex);
        pstIbdBuf = pstVppBufPool_Get(&pstCb->stIbdQ);
        pthread_mutex_unlock(&pstCb->mutex);
        if (!pstIbdBuf)
        {
            LOGE("Peeked IBD buf no longer available!");
            break;
        }
        if (ePort == VPP_PORT_OUTPUT)
        {
            LOGI("Bypassing buffers up to ts=0x%llx. Current bypass input buffer %p ts=0x%llx",
                 (long long unsigned int)pstBuf->pBuf->timestamp, pstIbdBuf,
                 (long long unsigned int)pstIbdBuf->pBuf->timestamp);
            VPP_STATS(pstCb, IssueOBD);
        }
        else
        {
            LOGI("Dequeueing %p from IBD queue ts=0x%llx", pstIbdBuf,
                 (long long unsigned int)pstIbdBuf->pBuf->timestamp);
            VPP_STATS(pstCb, IssueIBD);
        }
        VPP_LOG_BUF(pstCookie->pstCtx, pstIbdBuf, eVppLogId_PlBufDone);
        u32VppIp_CbBufDone(&pstCb->stCallbacks, ePort, pstIbdBuf);
        pstIbdBuf = pstVppBufPool_Peek(&pstCb->stIbdQ, 0);
    }
}

void vVppPipeline_InputBufferDoneCb(void *pv, t_StVppBuf *pstBuf)
{
    t_StVppPipelineCbCookie *pstCookie = (t_StVppPipelineCbCookie *)pv;
    t_StVppPipelineCb *pstCb = pstCookie->pstCtx->pstPlCb;
    uint32_t u32Pos = pstCookie->u32IpPos;
    uint32_t u32, u32Dest, i;

    LOGI("input buffer done from block: %u", u32Pos);
    pthread_mutex_lock(&pstCb->mutex);
    BUF_CNT_DEQUEUE(pstCb, u32Pos, VPP_PORT_INPUT, pstBuf);
    pthread_mutex_unlock(&pstCb->mutex);
    if (pstCb->u32IpCnt == 1)
    {
        if (pstBuf->u32InternalFlags & VPP_BUF_FLAG_FLUSHED &&
            pstCb->u32PipelineFlags & (PL_CLOSE_PENDING | PL_RECONFIGURE_TO_BYPASS))
        {
            LOGI("IBD storing input buffer");
            pthread_mutex_lock(&pstCb->mutex);
            u32VppBufPool_Put(&pstCb->stInputQ, pstBuf);
            pthread_mutex_unlock(&pstCb->mutex);
        }
        else
        {
            VPP_STATS(pstCb, IssueIBD);
            VPP_LOG_BUF(pstCookie->pstCtx, pstBuf, eVppLogId_PlBufDone);
            u32VppIp_CbBufDone(&pstCb->stCallbacks, VPP_PORT_INPUT, pstBuf);
        }
    }
    else
    {
        pthread_mutex_lock(&pstCb->int_flush_mutex);
        if (!(pstBuf->u32InternalFlags & VPP_BUF_FLAG_INTERNAL))
        {
            if (pstBuf->eQueuedPort == VPP_PORT_OUTPUT)
            {
                if (pstBuf->u32InternalFlags & VPP_BUF_FLAG_FLUSHED &&
                    pstCb->u32PipelineFlags & (PL_CLOSE_PENDING | PL_RECONFIGURE_TO_BYPASS))
                {
                    LOGI("storing output buffer %p", pstBuf);
                    pthread_mutex_lock(&pstCb->mutex);
                    u32VppBufPool_Put(&pstCb->stOutputQ, pstBuf);
                    pthread_mutex_unlock(&pstCb->mutex);
                }
                else if ((pstBuf->u32InternalFlags & VPP_BUF_FLAG_FLUSHED) ||
                         (pstCb->u32PipelineFlags & PL_INTERNAL_FLUSH_PENDING) ||
                         (pstCb->pstIpInfo[0].u32PendingFlags & PL_INPUT_FLUSH_PENDING) ||
                         (pstCb->pstIpInfo[VPP_PL_LAST_IP_IDX(pstCb)].u32PendingFlags &
                         PL_OUTPUT_FLUSH_PENDING))
                {
                    // External buffer at input of IP during flush
                    LOGD("Received IBD %p on external output buffer during flush. Flushing", pstBuf);
                    pstBuf->stPixel.u32FilledLen = 0;
                    VPP_STATS(pstCb, IssueOBD);
                    VPP_LOG_BUF(pstCookie->pstCtx, pstBuf, eVppLogId_PlBufDone);
                    u32VppIp_CbBufDone(&pstCb->stCallbacks, VPP_PORT_OUTPUT, pstBuf);
                }
                else
                {
                    // Internal IP finished with external output buffer. Can queue back to reuse
                    u32Dest = u32VppPipeline_FindOutputBufDest(pstCb);
                    LOGI("Received IBD %p on external output buffer. Queue back to IP%u",
                         pstBuf, u32Dest);
                    pstBuf->stPixel.u32FilledLen = 0;
                    pthread_mutex_lock(&pstCb->mutex);
                    BUF_CNT_QUEUE(pstCb, u32Dest, VPP_PORT_OUTPUT, pstBuf);
                    pthread_mutex_unlock(&pstCb->mutex);
                    u32 = IP_QUEUE_BUF(pstCb, u32Dest, VPP_PORT_OUTPUT, pstBuf);
                    if (u32 != VPP_OK)
                    {
                        LOGE("Error queueing external buffer to IP%u output. Flushing", u32Dest);
                        pthread_mutex_lock(&pstCb->mutex);
                        BUF_CNT_DEQUEUE(pstCb, u32Dest, VPP_PORT_OUTPUT, pstBuf);
                        pthread_mutex_unlock(&pstCb->mutex);
                        VPP_STATS(pstCb, IssueOBD);
                        VPP_LOG_BUF(pstCookie->pstCtx, pstBuf, eVppLogId_PlBufDone);
                        u32VppIp_CbBufDone(&pstCb->stCallbacks, VPP_PORT_OUTPUT, pstBuf);
                    }
                }
            }
            else if (pstBuf->u32InternalFlags & VPP_BUF_FLAG_FLUSHED)
            {
                if (pstCb->u32PipelineFlags & (PL_CLOSE_PENDING | PL_RECONFIGURE_TO_BYPASS))
                {
                    LOGI("storing input buffer");
                    pthread_mutex_lock(&pstCb->mutex);
                    u32VppBufPool_Put(&pstCb->stInputQ, pstBuf);
                    pthread_mutex_unlock(&pstCb->mutex);
                }
                else
                {
                    LOGI("External IBD %p", pstBuf);
                    VPP_STATS(pstCb, IssueIBD);
                    VPP_LOG_BUF(pstCookie->pstCtx, pstBuf, eVppLogId_PlBufDone);
                    u32VppIp_CbBufDone(&pstCb->stCallbacks, VPP_PORT_INPUT, pstBuf);
                }
            }
            else
            {
                LOGI("Storing %p IBD Queue ts=0x%llx",
                     pstBuf, (long long unsigned int)pstBuf->pBuf->timestamp);
                pthread_mutex_lock(&pstCb->mutex);
                u32VppBufPool_Put(&pstCb->stIbdQ, pstBuf);
                pthread_mutex_unlock(&pstCb->mutex);
            }
        }
        else  // Otherwise internal
        {
            pstBuf->stPixel.u32FilledLen = 0;
            pstBuf->stExtra.u32FilledLen = 0;
            pstBuf->eCurrentIpPort = VPP_PORT_OUTPUT;
            if ((pstBuf->u32InternalFlags & VPP_BUF_FLAG_FLUSHED) ||
                (pstCb->u32PipelineFlags & PL_INTERNAL_FLUSH_PENDING) ||
                (pstCb->pstIpInfo[0].u32PendingFlags & PL_INPUT_FLUSH_PENDING) ||
                (pstCb->pstIpInfo[VPP_PL_LAST_IP_IDX(pstCb)].u32PendingFlags & PL_OUTPUT_FLUSH_PENDING))
            {
                LOGI("Flushing IBD buffer from IP%u to Internal Pool%d", u32Pos,
                     pstBuf->u32InternalPool);
                pthread_mutex_lock(&pstCb->mutex);
                u32 = u32VppBufPool_Put(&pstCb->pstInternalPool[pstBuf->u32InternalPool].stInternalBufQ,
                                        pstBuf);
                pthread_mutex_unlock(&pstCb->mutex);
            }
            else
            {
                LOGI("Queueing IBD buffer from IP%u to output port of IP%u", u32Pos,
                     pstBuf->u32InternalPool);
                pstBuf->u32InternalFlags = VPP_BUF_FLAG_INTERNAL;
                pstBuf->eCurrentIpPort = VPP_PORT_OUTPUT;
                pthread_mutex_lock(&pstCb->mutex);
                BUF_CNT_QUEUE(pstCb, pstBuf->u32InternalPool, VPP_PORT_OUTPUT, pstBuf);
                pthread_mutex_unlock(&pstCb->mutex);
                u32 = IP_QUEUE_BUF(pstCb, pstBuf->u32InternalPool, VPP_PORT_OUTPUT, pstBuf);
                if (u32 != VPP_OK)
                {
                    LOGE("Error queueing internal buffer to IP%u output. Return to internal pool",
                         pstBuf->u32InternalPool);
                    pthread_mutex_lock(&pstCb->mutex);
                    BUF_CNT_DEQUEUE(pstCb, pstBuf->u32InternalPool, VPP_PORT_OUTPUT, pstBuf);
                    u32VppBufPool_Put(&pstCb->pstInternalPool[pstBuf->u32InternalPool].stInternalBufQ,
                                            pstBuf);
                    pthread_mutex_unlock(&pstCb->mutex);
                }
            }
            if (u32)
                LOGE("Error with IBD buffer");
        }
        pthread_mutex_unlock(&pstCb->int_flush_mutex);
    }
    vVppPipeline_DumpBufStats(pstCb);
}

void vVppPipeline_OutputBufferDoneCb(void *pv, t_StVppBuf *pstBuf)
{
    t_StVppPipelineCbCookie *pstCookie = (t_StVppPipelineCbCookie *)pv;
    t_StVppPipelineCb *pstCb = pstCookie->pstCtx->pstPlCb;
    uint32_t u32Pos = pstCookie->u32IpPos;
    t_StVppBuf *pstIbdBuf;
    uint32_t u32,i;

    LOGI("output buffer %p done from block: %u internal port %d, queued port %d",
         pstBuf, u32Pos, pstBuf->eCurrentIpPort, pstBuf->eQueuedPort);
    pthread_mutex_lock(&pstCb->mutex);
    BUF_CNT_DEQUEUE(pstCb, u32Pos, pstBuf->eCurrentIpPort, pstBuf);
    pthread_mutex_unlock(&pstCb->mutex);

    if (pstCb->u32IpCnt == 1)
    {
        if (pstBuf->u32InternalFlags & VPP_BUF_FLAG_FLUSHED &&
            pstBuf->stPixel.u32FilledLen)
        {
            IP_LOG(E, pstCb, u32Pos, "received flushed buffer with non-zero "
                   "fill_len (len=%u)... setting to zero",
                   pstBuf->stPixel.u32FilledLen);
            pstBuf->stPixel.u32FilledLen = 0;
        }

        if (pstBuf->u32InternalFlags & VPP_BUF_FLAG_FLUSHED &&
            pstCb->u32PipelineFlags & (PL_CLOSE_PENDING | PL_RECONFIGURE_TO_BYPASS))
        {
            LOGI("storing output buffer");
            pthread_mutex_lock(&pstCb->mutex);
            u32VppBufPool_Put(&pstCb->stOutputQ, pstBuf);
            pthread_mutex_unlock(&pstCb->mutex);
        }
        else
        {
            VPP_STATS(pstCb, IssueOBD);
            VPP_LOG_BUF(pstCookie->pstCtx, pstBuf, eVppLogId_PlBufDone);
            u32VppIp_CbBufDone(&pstCb->stCallbacks, VPP_PORT_OUTPUT, pstBuf);
        }
    }
    else
    {
        pthread_mutex_lock(&pstCb->int_flush_mutex);
        if (pstBuf->u32InternalFlags & VPP_BUF_FLAG_FLUSHED &&
            pstBuf->stPixel.u32FilledLen)
        {
            IP_LOG(E, pstCb, u32Pos, "received flushed buffer with non-zero "
                   "fill_len (len=%u)... setting to zero",
                   pstBuf->stPixel.u32FilledLen);
            pstBuf->stPixel.u32FilledLen = 0;
        }
        if ((u32Pos == VPP_PL_LAST_IP_IDX(pstCb)) ||
            (pstBuf->u32InternalFlags & VPP_BUF_FLAG_FLUSHED) ||
            (pstCb->u32PipelineFlags & PL_INTERNAL_FLUSH_PENDING) ||
            (pstCb->pstIpInfo[0].u32PendingFlags & PL_INPUT_FLUSH_PENDING) ||
            (pstCb->pstIpInfo[VPP_PL_LAST_IP_IDX(pstCb)].u32PendingFlags & PL_OUTPUT_FLUSH_PENDING))
        {
            if (pstBuf->u32InternalFlags & VPP_BUF_FLAG_INTERNAL)
            {
                pstBuf->stPixel.u32FilledLen = 0;
                pstBuf->stExtra.u32FilledLen = 0;
                if ((pstBuf->u32InternalFlags & VPP_BUF_FLAG_FLUSHED) ||
                    (pstCb->u32PipelineFlags & PL_INTERNAL_FLUSH_PENDING) ||
                    (pstCb->pstIpInfo[0].u32PendingFlags & PL_INPUT_FLUSH_PENDING) ||
                    (pstCb->pstIpInfo[VPP_PL_LAST_IP_IDX(pstCb)].u32PendingFlags & PL_OUTPUT_FLUSH_PENDING))
                {
                    LOGI("Flushing OBD buffer from IP%u to Internal Pool%u",
                         u32Pos, pstBuf->u32InternalPool);
                    pthread_mutex_lock(&pstCb->mutex);
                    u32 = u32VppBufPool_Put(&pstCb->pstInternalPool[pstBuf->u32InternalPool].stInternalBufQ,
                                            pstBuf);
                    pthread_mutex_unlock(&pstCb->mutex);
                }
                else
                {
                    if (pstBuf->eBufPxType != eVppBufPxDataType_Measurement)
                        vVppPipeline_ReturnIbdBufs(pstCb, pstCookie, pstBuf, VPP_PORT_OUTPUT);

                    LOGI("Queueing internal OBD buffer at last IP%u back to output port of IP%u",
                         u32Pos, pstBuf->u32InternalPool);
                    pstBuf->u32InternalFlags = VPP_BUF_FLAG_INTERNAL;
                    pstBuf->eCurrentIpPort = VPP_PORT_OUTPUT;
                    pthread_mutex_lock(&pstCb->mutex);
                    BUF_CNT_QUEUE(pstCb, pstBuf->u32InternalPool, VPP_PORT_OUTPUT, pstBuf);
                    pthread_mutex_unlock(&pstCb->mutex);
                    u32 = IP_QUEUE_BUF(pstCb, pstBuf->u32InternalPool, VPP_PORT_OUTPUT, pstBuf);
                    if (u32 != VPP_OK)
                    {
                        LOGE("Error queueing internal buffer to IP%u output. Return to internal pool",
                             pstBuf->u32InternalPool);
                        pthread_mutex_lock(&pstCb->mutex);
                        BUF_CNT_DEQUEUE(pstCb, pstBuf->u32InternalPool, VPP_PORT_OUTPUT, pstBuf);
                        u32VppBufPool_Put(&pstCb->pstInternalPool[pstBuf->u32InternalPool].stInternalBufQ,
                                          pstBuf);
                        pthread_mutex_unlock(&pstCb->mutex);
                    }
                }
            }
            else
            {
                if (pstBuf->u32InternalFlags & VPP_BUF_FLAG_FLUSHED &&
                    pstCb->u32PipelineFlags &
                    (PL_CLOSE_PENDING | PL_RECONFIGURE_TO_BYPASS | PL_RECONFIGURE_PENDING))
                {
                    LOGI("storing output buffer");
                    pthread_mutex_lock(&pstCb->mutex);
                    u32VppBufPool_Put(&pstCb->stOutputQ, pstBuf);
                    pthread_mutex_unlock(&pstCb->mutex);
                }
                else if (u32Pos == VPP_PL_LAST_IP_IDX(pstCb))
                {
                    if (pstBuf->eBufPxType != eVppBufPxDataType_Measurement)
                        vVppPipeline_ReturnIbdBufs(pstCb, pstCookie, pstBuf, VPP_PORT_INPUT);

                    if ((pstBuf->u32InternalFlags & VPP_BUF_FLAG_FLUSHED) ||
                        (pstCb->u32PipelineFlags & PL_INTERNAL_FLUSH_PENDING) ||
                        (pstCb->pstIpInfo[0].u32PendingFlags & PL_INPUT_FLUSH_PENDING) ||
                        (pstCb->pstIpInfo[VPP_PL_LAST_IP_IDX(pstCb)].u32PendingFlags & PL_OUTPUT_FLUSH_PENDING))
                    {
                        pstBuf->stPixel.u32FilledLen = 0;
                        pstBuf->stExtra.u32FilledLen = 0;
                    }
                    VPP_STATS(pstCb, IssueOBD);
                    VPP_LOG_BUF(pstCookie->pstCtx, pstBuf, eVppLogId_PlBufDone);
                    u32VppIp_CbBufDone(&pstCb->stCallbacks, VPP_PORT_OUTPUT, pstBuf);
                }
                else
                {
                    // External buffer in internal pipeline during flush. Flush back to queued port
                    pstBuf->stPixel.u32FilledLen = 0;
                    pstBuf->stExtra.u32FilledLen = 0;
                    if (pstBuf->eQueuedPort == VPP_PORT_INPUT)
                    {
                        VPP_STATS(pstCb, IssueIBD);
                        VPP_LOG_BUF(pstCookie->pstCtx, pstBuf, eVppLogId_PlBufDone);
                        u32VppIp_CbBufDone(&pstCb->stCallbacks, VPP_PORT_INPUT, pstBuf);
                    }
                    else
                    {
                        VPP_STATS(pstCb, IssueOBD);
                        VPP_LOG_BUF(pstCookie->pstCtx, pstBuf, eVppLogId_PlBufDone);
                        u32VppIp_CbBufDone(&pstCb->stCallbacks, VPP_PORT_OUTPUT, pstBuf);
                    }
                }
            }
        }
        else
        {
            LOGI("Queueing OBD buffer from IP%u to input port of IP%u", u32Pos, u32Pos + 1);
            pstBuf->eCurrentIpPort = VPP_PORT_INPUT;
            pthread_mutex_lock(&pstCb->mutex);
            BUF_CNT_QUEUE(pstCb, u32Pos + 1, VPP_PORT_INPUT, pstBuf);
            pthread_mutex_unlock(&pstCb->mutex);
            u32 = IP_QUEUE_BUF(pstCb, u32Pos + 1, VPP_PORT_INPUT, pstBuf);
            if (u32 != VPP_OK)
            {
                LOGE("Error queueing buffer to IP%u input", u32Pos + 1);
                pthread_mutex_lock(&pstCb->mutex);
                BUF_CNT_DEQUEUE(pstCb, u32Pos + 1, VPP_PORT_INPUT, pstBuf);
                pthread_mutex_unlock(&pstCb->mutex);
                pstBuf->stPixel.u32FilledLen = 0;
                if (pstBuf->u32InternalFlags & VPP_BUF_FLAG_INTERNAL)
                {
                    LOGE("Error queueing internal buffer to IP%u input. Return to internal pool",
                         u32Pos + 1);
                    pthread_mutex_lock(&pstCb->mutex);
                    u32VppBufPool_Put(&pstCb->pstInternalPool[pstBuf->u32InternalPool].stInternalBufQ,
                                      pstBuf);
                    pthread_mutex_unlock(&pstCb->mutex);
                }
                else
                {
                    LOGE("Error queueing external buffer to IP%u input. Flushing", u32Pos + 1);
                    VPP_STATS(pstCb, IssueOBD);
                    VPP_LOG_BUF(pstCookie->pstCtx, pstBuf, eVppLogId_PlBufDone);
                    u32VppIp_CbBufDone(&pstCb->stCallbacks, VPP_PORT_OUTPUT, pstBuf);
                }
            }
        }
        pthread_mutex_unlock(&pstCb->int_flush_mutex);
    }
    vVppPipeline_DumpBufStats(pstCb);
}

void vVppPipeline_EventCb(void *pv, t_StVppEvt stEvt)
{
    t_StVppPipelineCbCookie *pstCookie = (t_StVppPipelineCbCookie *)pv;
    t_StVppCtx *pstCtx = pstCookie->pstCtx;
    t_StVppPipelineCb *pstPlCb = pstCookie->pstCtx->pstPlCb;
    uint32_t u32Pos = pstCookie->u32IpPos;

    LOGI("event from block: %u", u32Pos);

    if (stEvt.eType == VPP_EVT_FLUSH_DONE)
    {
        enum vpp_port ePort = stEvt.flush.ePort;

        if (VPP_FLAG_IS_SET(pstPlCb->u32PipelineFlags, PL_CLOSE_PENDING) ||
            VPP_FLAG_IS_SET(pstPlCb->u32PipelineFlags, PL_RECONFIGURE_TO_BYPASS) ||
            VPP_FLAG_IS_SET(pstPlCb->u32PipelineFlags, PL_INTERNAL_FLUSH_PENDING))
        {
            pthread_mutex_lock(&pstPlCb->mutex);

            if (ePort == VPP_PORT_INPUT)
                pstPlCb->pstIpInfo[u32Pos].u32PendingFlags &= ~PL_INPUT_FLUSH_PENDING;
            else if (ePort == VPP_PORT_OUTPUT)
                pstPlCb->pstIpInfo[u32Pos].u32PendingFlags &= ~PL_OUTPUT_FLUSH_PENDING;

            pthread_mutex_unlock(&pstPlCb->mutex);
            pthread_cond_broadcast(&pstPlCb->cond_from_worker);
            LOGI("Internal flush event from IP%u, port=%u Pending flags %d pstPlCb %p",
                 u32Pos, ePort, pstPlCb->pstIpInfo[u32Pos].u32PendingFlags, pstPlCb);
        }
        else
        {
            if (pstPlCb->u32IpCnt == 1)
            {
                u32VppIpCbEvent(&pstPlCb->stCallbacks, stEvt);
            }
            else
            {
                pthread_mutex_lock(&pstPlCb->mutex);
                if (ePort == VPP_PORT_INPUT)
                    pstPlCb->pstIpInfo[u32Pos].u32PendingFlags &= ~PL_INPUT_FLUSH_PENDING;
                else if (ePort == VPP_PORT_OUTPUT)
                    pstPlCb->pstIpInfo[u32Pos].u32PendingFlags &= ~PL_OUTPUT_FLUSH_PENDING;
                pthread_mutex_unlock(&pstPlCb->mutex);
                pthread_cond_broadcast(&pstPlCb->cond_from_worker);
                LOGI("flush done event from IP%u, port=%u Pending flags %d pstPlCb %p",
                     u32Pos, ePort, pstPlCb->pstIpInfo[u32Pos].u32PendingFlags, pstPlCb);
                u32VppIpCbEvent(&pstPlCb->stCallbacks, stEvt);
            }
        }
    }
    else if (stEvt.eType == VPP_EVT_DRAIN_DONE)
    {
        u32VppPipeline_HandleDrainDone(pstCtx, pstPlCb, u32Pos);
    }
    else
    {
        LOGE("unrecognized event from block: %u", u32Pos);
    }
}

void vVppPipeline_LogCb(void *pv, t_StVppBuf *pstBuf, t_EVppBufIpLogId eId)
{
    if (!pv || !pstBuf || eId >= eVppLogId_IpMax)
        return;

    t_StVppPipelineCbCookie *pstCookie = (t_StVppPipelineCbCookie *)pv;
    t_StVppCtx *pstCtx = pstCookie->pstCtx;
    uint32_t u32Pos = pstCookie->u32IpPos;
    uint32_t u32Id = eVppLogId_IpStart + (eVppLogId_IpMax * u32Pos) + eId;
    VPP_LOG_BUF(pstCtx, pstBuf, u32Id);
}

static uint32_t u32VppPipeline_IsCustCtrlForIp(t_StVppPipelineCb *pstPlCb,
                                              uint32_t u32Idx,
                                              struct hqv_ctrl_custom *pstCust)
{
    if (pstCust->id == IP_GET(pstPlCb, u32Idx).ip->eIp)
        return VPP_TRUE;
    return VPP_FALSE;
}

static void vVppPipeline_CustomCtrlPut(t_StVppPipelineCb *pstPlCb,
                                       struct hqv_ctrl_custom *pstCustCtrl)
{
    t_StVppPipelineCustCtrlNode *pstNode;

    if (!pstPlCb || !pstCustCtrl)
        return;

    pstNode = (t_StVppPipelineCustCtrlNode *)
        malloc(sizeof(t_StVppPipelineCustCtrlNode));

    if (!pstNode)
    {
        LOGE("Unable to allocate memory for custom control node");
        return;
    }

    memcpy(&pstNode->ctrl, pstCustCtrl, sizeof(struct hqv_ctrl_custom));

    pstNode->pNext = pstPlCb->stCustCtrlList.pHead;
    pstPlCb->stCustCtrlList.pHead = pstNode;
    pstPlCb->stCustCtrlList.u32Cnt += 1;

    LOGI("stored custom control, cnt=%u, pHead=%p, ctrl: {id=%u, len=%u, data[0]=0x%x}",
         pstPlCb->stCustCtrlList.u32Cnt,
         pstPlCb->stCustCtrlList.pHead,
         pstNode->ctrl.id,
         pstNode->ctrl.len,
         ((uint32_t *)pstNode->ctrl.data)[0]);
}

static uint32_t u32VppPipeline_CustomCtrlGet(t_StVppPipelineCb *pstPlCb,
                                             uint32_t bUseCurrentUc,
                                             struct hqv_ctrl_custom *pstCustCtrl)
{
    uint32_t i, bFound = VPP_FALSE;
    uint32_t u32DbgCnt = 0;
    t_StVppPipelineCustCtrlNode *pstNode = NULL, *pstNodePrev = NULL;

    LOG_ENTER();

    if (!pstPlCb || !pstCustCtrl)
        LOG_EXIT_RET(VPP_ERR_PARAM);

    if (!pstPlCb->stCustCtrlList.u32Cnt)
        LOG_EXIT_RET(VPP_ERR);

    if (bUseCurrentUc)
    {
        pstNode = pstPlCb->stCustCtrlList.pHead;
        while (pstNode != NULL)
        {
            for (i = 0; i < pstPlCb->u32IpCnt; i++)
            {
                if (u32VppPipeline_IsCustCtrlForIp(pstPlCb, i, &pstNode->ctrl))
                {
                    if (pstNodePrev == NULL)
                        pstPlCb->stCustCtrlList.pHead = pstNode->pNext;
                    else
                        pstNodePrev->pNext = pstNode->pNext;

                    pstPlCb->stCustCtrlList.u32Cnt -= 1;
                    bFound = VPP_TRUE;
                    LOGI("found custom control at position %u for IP%u",
                         u32DbgCnt, i);
                    break;
                }
            }

            if (bFound)
                break;
            else
            {
                u32DbgCnt += 1;
                pstNodePrev = pstNode;
                pstNode = pstNode->pNext;
            }
        }
    }
    else
    {
        LOGI("removing first custom control");
        pstNode = pstPlCb->stCustCtrlList.pHead;
        pstPlCb->stCustCtrlList.pHead = pstNode->pNext;
        pstPlCb->stCustCtrlList.u32Cnt -= 1;
    }

    if (pstNode)
    {
        memcpy(pstCustCtrl, &pstNode->ctrl, sizeof(struct hqv_ctrl_custom));
        free(pstNode);
        LOG_EXIT_RET(VPP_OK);
    }

    LOG_EXIT_RET(VPP_ERR);
}

static uint32_t u32VppPipeline_SubmodInit(t_StVppCtx *pstCtx,
                                          t_StVppPipelineCb *pstPlCb,
                                          t_StVppUsecase *pstUc)
{
    uint32_t i, err = 0;
    void *tmp;
    t_StVppCallback cbs;

    if (!pstPlCb || !pstPlCb || !pstUc)
        return VPP_ERR_PARAM;

    memset(&cbs, 0, sizeof(t_StVppCallback));

    cbs.input_buffer_done = vVppPipeline_InputBufferDoneCb;
    cbs.output_buffer_done = vVppPipeline_OutputBufferDoneCb;
    cbs.event = vVppPipeline_EventCb;
    cbs.log = vVppPipeline_LogCb;

    for (i = 0; i < pstUc->u32IpCnt; i++)
    {
        if (!pstUc->composition[i].ip)
            break;

        pstPlCb->pstIpInfo[i].stCbCookie.pstCtx = pstCtx;
        pstPlCb->pstIpInfo[i].stCbCookie.u32IpPos = i;
        cbs.pv = &pstPlCb->pstIpInfo[i].stCbCookie;

        IP_LOG(I, pstPlCb, i, ">> initializing");
        tmp = IP_INIT(pstPlCb, pstCtx, i, pstPlCb->pstParam->u32SessionFlags, cbs);
        IP_LOG(I, pstPlCb, i, "<< initialized, ctx=%p", tmp);
        if (!tmp)
        {
            err = 1;
            break;
        }
        IP_CTX_SET(pstPlCb, i, tmp);
        pstPlCb->u32IpCnt += 1;
    }

    if (err)
    {
        for (; i > 0; i--)
        {
            IP_TERM(pstPlCb, i - 1);
            IP_CTX_SET(pstPlCb, i - 1, NULL);
            pstPlCb->u32IpCnt -= 1;
        }
        return VPP_ERR;
    }

    LOGI("Total initialized ip: %u", pstPlCb->u32IpCnt);
    return VPP_OK;
}


static uint32_t u32VppPipeline_SubmodTerm(t_StVppPipelineCb *pstPlCb)
{
    uint32_t i;

    if (!pstPlCb)
        return VPP_ERR_PARAM;

    for (i = 0; i < pstPlCb->u32IpCnt; i++)
    {
        IP_LOG(I, pstPlCb, i, "terminating");
        IP_TERM(pstPlCb, i);
        IP_CTX_SET(pstPlCb, i, NULL);
    }

    pstPlCb->u32IpCnt = 0;

    return VPP_OK;
}

static uint32_t u32VppPipeline_SubmodOpen(t_StVppPipelineCb *pstPlCb)
{
    uint32_t u32Ret = VPP_ERR_INVALID_CFG;
    uint32_t i;

    if (!pstPlCb)
        return VPP_ERR_PARAM;

    for (i = 0; i < pstPlCb->u32IpCnt; i++)
    {
        u32Ret = IP_OPEN(pstPlCb, i);

        if (u32Ret != VPP_OK)
        {
            IP_LOG(E, pstPlCb, i, "error opening submodule");
            break;
        }
    }

    if (u32Ret != VPP_OK)
    {
        for (; i > 0; i--)
        {
            LOGE("closing index: %u", i);
            IP_CLOSE(pstPlCb, i);
        }
    }

    return u32Ret;
}

static uint32_t u32VppPipeline_SubmodClose(t_StVppPipelineCb *pstPlCb)
{
    uint32_t u32Ret, u32Error = VPP_OK, i;

    if (!pstPlCb)
        return VPP_ERR_PARAM;

    for (i = 0; i < pstPlCb->u32IpCnt; i++)
    {
        u32Ret = IP_CLOSE(pstPlCb, i);
        if (u32Ret != VPP_OK)
        {
            IP_LOG(E, pstPlCb, i, "error closing submodule, err=%u", u32Ret);
            u32Error = u32Ret;
        }
    }

    return u32Error;
}

static uint32_t u32VppPipeline_SubmodParamSet(t_StVppPipelineCb *pstPlCb, t_StVppUsecase *pstUc)
{
    uint32_t u32Ret = VPP_OK, u32InRet, u32OutRet, i;
    t_StVppParam stParamTemp;

    if (!pstPlCb || !pstUc)
        return VPP_ERR_PARAM;

    for (i = 0; i < pstPlCb->u32IpCnt; i++)
    {
        u32Ret = u32VppUsecase_GetIpPortParam(pstUc, pstPlCb->pstParam, i, &stParamTemp);
        if (u32Ret != VPP_OK)
        {
            LOGE("Error getting IP params for IP%u, u32=%u", i, u32Ret);
            return u32Ret;
        }

        u32InRet = IP_SET_PARAM(pstPlCb, i, VPP_PORT_INPUT, stParamTemp.input_port);
        u32OutRet = IP_SET_PARAM(pstPlCb, i, VPP_PORT_OUTPUT, stParamTemp.output_port);

        if (u32InRet != VPP_OK || u32OutRet != VPP_OK)
        {
            IP_LOG(E, pstPlCb, i, "error setting param, u32InRet=%u, u32OutRet=%u",
                   u32InRet, u32OutRet);
            u32Ret = VPP_ERR;
        }
    }

    return u32Ret;
}

static uint32_t u32VppPipeline_SubmodCtrlSetSingle(t_StVppPipelineCb *pstPlCb,
                                                   uint32_t u32IpIdx,
                                                   t_StVppHqvCtrl *pstCtrl,
                                                   enum hqv_control_type type)
{
    uint32_t u32Ret;
    struct hqv_control ctrl;

    if (!pstPlCb || !pstCtrl)
        return VPP_ERR_PARAM;

    memset(&ctrl, 0, sizeof(struct hqv_control));

    ctrl.mode = pstCtrl->mode;

    if ((ctrl.mode != HQV_MODE_AUTO) || (ctrl.ctrl_type >= HQV_CONTROL_GLOBAL_START))
    {
        ctrl.mode = pstCtrl->mode;
        ctrl.ctrl_type = type;

        switch (type)
        {
            case HQV_CONTROL_CADE:
                ctrl.cade = pstCtrl->cade;
                break;
            case HQV_CONTROL_DI:
                ctrl.di = pstCtrl->di;
                break;
            case HQV_CONTROL_TNR:
                ctrl.tnr = pstCtrl->tnr;
                break;
            case HQV_CONTROL_CNR:
                ctrl.cnr = pstCtrl->cnr;
                break;
            case HQV_CONTROL_AIE:
                ctrl.aie = pstCtrl->aie;
                break;
            case HQV_CONTROL_FRC:
                ctrl.frc = pstCtrl->frc;
                break;
            case HQV_CONTROL_EAR:
                ctrl.ear = pstCtrl->ear;
                break;
            case HQV_CONTROL_QBR:
                ctrl.qbr = pstCtrl->qbr;
                break;
            case HQV_CONTROL_MEAS:
                ctrl.meas = pstCtrl->meas;
                break;
            case HQV_CONTROL_GLOBAL_DEMO:
                ctrl.demo = pstCtrl->demo;
                break;
            case HQV_CONTROL_CUST:
                LOGE("HQV_CONTROL_CUST requested in SetSingle, but is invalid");
                break;
            case HQV_CONTROL_MAX:
                LOGE("HQV_CONTROL_MAX and mode not HQV_MODE_AUTO!");
            default:
                LOGE("Invalid type passed to SetSingle");
                break;
        }
    }

    IP_LOG(D, pstPlCb, u32IpIdx, "setting ctrl=%u", type);

    u32Ret = IP_SET_CTRL(pstPlCb, u32IpIdx, ctrl);

    if (u32Ret != VPP_OK)
        IP_LOG(E, pstPlCb, u32IpIdx, "setting ctrl=%u failed, u32Ret=%u",
               type, u32Ret);

    return u32Ret;
}

static uint32_t u32VppPipeline_SubmodCtrlDisableSingle(t_StVppPipelineCb *pstPlCb,
                                                       uint32_t u32IpIdx,
                                                       t_StVppHqvCtrl *pstCtrl,
                                                       enum hqv_control_type type)
{
    uint32_t u32Ret;
    struct hqv_control ctrl;

    if (!pstPlCb || !pstCtrl)
        return VPP_ERR_PARAM;

    memset(&ctrl, 0, sizeof(struct hqv_control));

    if (pstCtrl->mode == HQV_MODE_AUTO)
    {
        ctrl.mode = HQV_MODE_OFF;
    }
    else
    {
        ctrl.mode = pstCtrl->mode;
        ctrl.ctrl_type = type;

        switch (type)
        {
            case HQV_CONTROL_CADE:
                ctrl.cade.mode = HQV_MODE_OFF;
                break;
            case HQV_CONTROL_DI:
                ctrl.di.mode = HQV_DI_MODE_OFF;
                break;
            case HQV_CONTROL_TNR:
                ctrl.tnr.mode = HQV_MODE_OFF;
                break;
            case HQV_CONTROL_CNR:
                ctrl.cnr.mode = HQV_MODE_OFF;
                break;
            case HQV_CONTROL_AIE:
                ctrl.aie.mode = HQV_MODE_OFF;
                break;
            case HQV_CONTROL_FRC:
                ctrl.frc.mode = HQV_FRC_MODE_OFF;
                break;
            case HQV_CONTROL_EAR:
                ctrl.ear.mode = HQV_EAR_MODE_OFF;
                break;
            case HQV_CONTROL_QBR:
                ctrl.qbr.mode = HQV_QBR_MODE_OFF;
                break;
            case HQV_CONTROL_MEAS:
                ctrl.meas.enable = VPP_FALSE;
                break;
            case HQV_CONTROL_CUST:
                break;
            case HQV_CONTROL_MAX:
            default:
                break;
        }
    }
    IP_LOG(D, pstPlCb, u32IpIdx, "disabling ctrl=%u", type);

    u32Ret = IP_SET_CTRL(pstPlCb, u32IpIdx, ctrl);

    if (u32Ret != VPP_OK)
        IP_LOG(E, pstPlCb, u32IpIdx, "disabling ctrl=%u failed, u32Ret=%u",
               type, u32Ret);

    return u32Ret;
}

static uint32_t u32VppPipeline_SubmodCtrlSet(t_StVppPipelineCb *pstPlCb,
                                             t_StVppUsecase *pstUc,
                                             t_StVppHqvCtrl *pstCtrl,
                                             uint32_t bDisable)
{
    uint32_t idx_ip, idx_algo, u32Ret;
    enum hqv_control_type type;

    if (!pstPlCb || !pstUc || !pstCtrl)
        return VPP_ERR_PARAM;

    if (pstCtrl->mode == HQV_MODE_AUTO)
    {
        for (idx_ip = 0; idx_ip < pstPlCb->u32IpCnt; idx_ip++)
        {
            if (bDisable)
                u32VppPipeline_SubmodCtrlDisableSingle(pstPlCb, idx_ip, pstCtrl,
                                                       HQV_CONTROL_MAX);
            else
                u32VppPipeline_SubmodCtrlSetSingle(pstPlCb, idx_ip, pstCtrl,
                                                   HQV_CONTROL_MAX);
        }
    }
    else
    {
        // foreach algo in the use case
        for (idx_algo = 0; idx_algo < pstUc->algos.u32CtrlCnt; idx_algo++)
        {
            type = pstUc->algos.ctrls[idx_algo];

            // check if each ip block supports the algo
            for (idx_ip = 0; idx_ip < pstPlCb->u32IpCnt; idx_ip++)
            {
                if (u32VppUsecase_IpIndexSupportsAlgo(pstUc, idx_ip, type))
                {
                    if (bDisable)
                        u32VppPipeline_SubmodCtrlDisableSingle(pstPlCb, idx_ip,
                                                               pstCtrl, type);
                    else
                        u32VppPipeline_SubmodCtrlSetSingle(pstPlCb, idx_ip,
                                                           pstCtrl, type);
                }
            }
        }
    }

    return VPP_OK;
}

uint32_t u32VppPipeline_SubmodCustomCtrlSet(t_StVppPipelineCb *pstPlCb)
{
    uint32_t u32, i;
    struct hqv_control stCtrl;
    struct hqv_ctrl_custom *pstCustCtrl;

    LOG_ENTER();
    if (!pstPlCb)
        LOG_EXIT_RET(VPP_ERR_PARAM);

    memset(&stCtrl, 0, sizeof(struct hqv_control));
    stCtrl.mode = HQV_MODE_MANUAL;
    stCtrl.ctrl_type = HQV_CONTROL_CUST;
    pstCustCtrl = &stCtrl.custom;

    // For each custom control that is stored, pull it from the list and
    // send it to the IP block for which that custom control is destined.
    while (u32VppPipeline_CustomCtrlGet(pstPlCb, VPP_TRUE, pstCustCtrl) == VPP_OK)
    {
        for (i = 0; i < pstPlCb->u32IpCnt; i++)
        {
            if (u32VppPipeline_IsCustCtrlForIp(pstPlCb, i, pstCustCtrl))
            {
                IP_LOG(D, pstPlCb, i, "setting custom ctrl, id=%u",
                       pstCustCtrl->id);
                u32 = IP_SET_CTRL(pstPlCb, i, stCtrl);
                if (u32 != VPP_OK)
                    IP_LOG(E, pstPlCb, i, "setting custom ctrl, id=%u, err=%u",
                           pstCustCtrl->id, u32);
                break;
            }
        }

        if (i == pstPlCb->u32IpCnt)
        {
            // Should never hit this case, since CustomCtrlGet() should only
            // return custom controls based on the composition of the current
            // usecase.
            LOGE("found custom control for IP that is not in use");
        }
    }

    LOG_EXIT_RET(VPP_OK);
}

static uint32_t u32VppPipeline_SubmodGlobalCtrlSet(t_StVppPipelineCb *pstPlCb,
                                                   t_StVppHqvCtrl *pstCtrl)
{
    uint32_t idx_ip, idx_algo, u32Ret;
    enum hqv_control_type type;

    LOG_ENTER();

    for(type = HQV_CONTROL_GLOBAL_START; type < HQV_CONTROL_MAX; type++)
    {
        // check if each ip block supports the algo
        for (idx_ip = 0; idx_ip < pstPlCb->u32IpCnt; idx_ip++)
        {
            if (u32VppIp_SupportsAlgo(IP_GET(pstPlCb, idx_ip).ip, type))
            {
                u32VppPipeline_SubmodCtrlSetSingle(pstPlCb, idx_ip, pstCtrl, type);
            }
        }
    }
    LOG_EXIT_RET(VPP_OK);
}

uint32_t u32VppPipeline_SubmodGetBufReq(t_StVppPipelineCb *pstPlCb)
{
    uint32_t i, u32Ret = VPP_OK;

    if (!pstPlCb)
        return VPP_ERR_PARAM;

    for (i = 0; i < pstPlCb->u32IpCnt; i++)
    {
        u32Ret = IP_GET_BUF_REQ(pstPlCb, i, &pstPlCb->pstIpInfo[i].stInBufReq,
                                &pstPlCb->pstIpInfo[i].stOutBufReq);
        if (u32Ret != VPP_OK)
        {
            IP_LOG(E, pstPlCb, i, "error getting IP buffer requirements");
            break;
        }
    }

    return u32Ret;
}

uint32_t u32VppPipeline_SubmodImplementReconfigure(t_StVppPipelineCb *pstPlCb)
{
    uint32_t i;

    if (!pstPlCb)
        return VPP_FALSE;

    for (i = 0; i < pstPlCb->u32IpCnt; i++)
        if (!IP_FUNC(pstPlCb, i).reconfigure)
            return VPP_FALSE;

    return VPP_TRUE;
}

uint32_t u32VppPipeline_SubmodReconfigure(t_StVppPipelineCb *pstPlCb, t_StVppUsecase *pstUc)
{
    uint32_t i, u32Ret = VPP_OK;
    t_StVppParam stParamTemp;

    if (!pstPlCb)
        return VPP_ERR_PARAM;

    for (i = 0; i < pstPlCb->u32IpCnt; i++)
    {
        u32Ret = u32VppUsecase_GetIpPortParam(pstUc, pstPlCb->pstParam, i, &stParamTemp);
        if (u32Ret != VPP_OK)
        {
            LOGE("Error during reconfig getting IP params for IP%u, u32=%u", i, u32Ret);
            return u32Ret;
        }

        u32Ret = IP_RCFG(pstPlCb, i, stParamTemp.input_port,
                         stParamTemp.output_port);
        if (u32Ret != VPP_OK)
        {
            IP_LOG(E, pstPlCb, i, "error reconfiguring submodule");
            break;
        }
    }

    return u32Ret;
}

uint32_t u32VppPipeline_FlushIpHoldBufs(t_StVppPipelineCb *pstPlCb,
                                        uint32_t u32Flag)
{
    uint32_t u32Ret = VPP_OK;
    uint32_t u32InputIp, u32OutputIp;
    uint32_t *pu32InputFlags, *pu32OutputFlags;

    u32InputIp = 0;
    u32OutputIp = VPP_PL_LAST_IP_IDX(pstPlCb);

    pu32InputFlags = &pstPlCb->pstIpInfo[u32InputIp].u32PendingFlags;
    pu32OutputFlags = &pstPlCb->pstIpInfo[u32OutputIp].u32PendingFlags;

    pthread_mutex_lock(&pstPlCb->mutex);
    pstPlCb->u32PipelineFlags |= u32Flag;
    *pu32InputFlags |= PL_INPUT_FLUSH_PENDING;
    *pu32OutputFlags |= PL_OUTPUT_FLUSH_PENDING;
    pthread_mutex_unlock(&pstPlCb->mutex);

    if (!VPP_FLAG_IS_SET(pstPlCb->u32PipelineFlags, PL_INTERNAL_FLUSH_PENDING))
    {
        u32Ret = u32VppPipeline_FlushInternalBuffers(pstPlCb);
        LOGE_IF(u32Ret != VPP_OK, "FlushInternalBuffers failed");
    }

    u32Ret = IP_FLUSH(pstPlCb, u32InputIp, VPP_PORT_INPUT);
    if (u32Ret != VPP_OK)
    {
        LOGE("unable to flush input port");
        pthread_mutex_lock(&pstPlCb->mutex);
        *pu32InputFlags &= ~PL_INPUT_FLUSH_PENDING;
        pthread_mutex_unlock(&pstPlCb->mutex);
    }

    u32Ret = IP_FLUSH(pstPlCb, u32OutputIp, VPP_PORT_OUTPUT);
    if (u32Ret != VPP_OK)
    {
        LOGE("unable to flush input port");
        pthread_mutex_lock(&pstPlCb->mutex);
        *pu32OutputFlags &= ~PL_OUTPUT_FLUSH_PENDING;
        pthread_mutex_unlock(&pstPlCb->mutex);
    }

    LOGI("waiting for flags to be cleared: pu32InputFlags %u, pu32OutputFlags %u",
         *pu32InputFlags, *pu32OutputFlags);
    pthread_mutex_lock(&pstPlCb->mutex);
    while ((*pu32InputFlags & PL_INPUT_FLUSH_PENDING) ||
           (*pu32OutputFlags & PL_OUTPUT_FLUSH_PENDING))
    {
        pthread_cond_wait(&pstPlCb->cond_from_worker, &pstPlCb->mutex);
    }

    pstPlCb->u32PipelineFlags &= ~u32Flag;

    pthread_mutex_unlock(&pstPlCb->mutex);
    LOGI("flag cleared");

    return u32Ret;
}

uint32_t u32VppPipeline_InternalOpen(t_StVppCtx *pstCb, t_StVppHqvCtrl *pstCtrl,
                                     t_StVppUsecase *pstUc)
{
    /*!
     * This function expects that all validation has already occurred on the
     * parameters
     */
    uint32_t u32;
    t_StVppPipelineCb *pstPlCb;

    pstPlCb = pstCb->pstPlCb;
    pstPlCb->pstUcOpen = pstUc;

    pstPlCb->pstIpInfo = calloc(pstUc->u32IpCnt, sizeof(t_StVppPipelineIpInfo));

    if (!pstPlCb->pstIpInfo)
    {
        LOGE("Error allocating pipeline IP info memory");
        u32 = VPP_ERR_NO_MEM;
        goto ERROR_PIPELINE_ALLOC;
    }

    u32 = u32VppPipeline_SubmodInit(pstCb, pstPlCb, pstUc);
    if (u32 != VPP_OK)
    {
        LOGE("SubmodInit returned %u", u32);
        goto ERROR_SUBMOD_INIT;
    }

    u32 = u32VppPipeline_SubmodParamSet(pstPlCb, pstUc);
    if (u32 != VPP_OK)
    {
        LOGE("SubmodParamSet returned %u", u32);
        goto ERROR_SUBMOD_PARAMSET;
    }

    u32 = u32VppPipeline_SubmodCtrlSet(pstPlCb, pstUc, pstCtrl, VPP_FALSE);
    LOGE_IF(u32 != VPP_OK, "SubmodCtrlSet returned %u", u32);

    u32 = u32VppPipeline_SubmodCustomCtrlSet(pstPlCb);
    LOGE_IF(u32 != VPP_OK, "SubmodCustomCtrlSet returned %u", u32);

    u32 = u32VppPipeline_SubmodGlobalCtrlSet(pstPlCb, pstCtrl);
    LOGE_IF(u32 != VPP_OK, "SubmodGlobalCtrlSet returned %u", u32);

    u32 = u32VppPipeline_SubmodOpen(pstPlCb);
    if (u32 != VPP_OK)
    {
        LOGE("SubmodOpen returned %u", u32);
        goto ERROR_SUBMOD_OPEN;
    }

    u32 = u32VppPipeline_SubmodGetBufReq(pstPlCb);
    LOGE_IF(u32 != VPP_OK, "SubmodGetBufReq returned %u", u32);

    if (pstPlCb->u32IpCnt > 1)
    {
        u32 = u32VppPipeline_AllocateInternalBuffers(pstCb);
        if (u32 != VPP_OK)
        {
            LOGE("AllocateInternalBuffers returned %u", u32);
            goto ERROR_SUBMOD_OPEN;
        }
    }

    return u32;

ERROR_SUBMOD_OPEN:
ERROR_SUBMOD_PARAMSET:
    u32VppPipeline_SubmodTerm(pstPlCb);
ERROR_SUBMOD_INIT:
    if (pstPlCb->pstIpInfo)
        free(pstPlCb->pstIpInfo);
ERROR_PIPELINE_ALLOC:
    pstPlCb->pstUcOpen = NULL;

    return u32;
}


uint32_t u32VppPipeline_InternalClose(t_StVppCtx *pstCb)
{
    uint32_t u32Ret = VPP_OK;
    t_StVppPipelineCb *pstPlCb;

    pstPlCb = VPP_PL_CB_GET(pstCb);

    if (pstPlCb->pstUcOpen)
    {
        u32Ret = u32VppPipeline_FlushIpHoldBufs(pstPlCb, PL_CLOSE_PENDING);
        LOGE_IF(u32Ret != VPP_OK, "FlushIpHoldBufs failed");

        vVppPipeline_FlushIbdQueue(pstCb);

        u32Ret = u32VppPipeline_FreeInternalBuffers(pstCb);
        LOGE_IF(u32Ret != VPP_OK, "FreeInternalBuffers failed");

        u32Ret = u32VppPipeline_SubmodClose(pstPlCb);
        LOGE_IF(u32Ret != VPP_OK, "submod close failed");

        u32Ret = u32VppPipeline_SubmodTerm(pstPlCb);
        LOGE_IF(u32Ret != VPP_OK, "submod term failed");

        if (pstPlCb->pstIpInfo)
            free(pstPlCb->pstIpInfo);
    }

    pstPlCb->pstUcOpen = NULL;

    return u32Ret;
}

static uint32_t u32VppPipeline_HandleReconfigureCompEqCloseOpen(t_StVppCtx *pstCb,
                                                                t_StVppPipelineCb *pstPlCb,
                                                                t_StVppUsecase *pstUc,
                                                                t_StVppHqvCtrl *pstCtrl)
{
    uint32_t u32;

    LOG_ENTER();

    u32 = u32VppPipeline_SubmodClose(pstPlCb);
    if (u32 != VPP_OK)
    {
        LOGE("SubmodClose returned %u", u32);
        goto ERROR_CLOSE;
    }

    u32 = u32VppPipeline_SubmodParamSet(pstPlCb, pstUc);
    if (u32 != VPP_OK)
    {
        LOGE("SubmodParamSet returned %u", u32);
        goto ERROR_PARAMSET;
    }

    if (pstUc != pstPlCb->pstUcOpen)
    {
        // Possible that the composition is the same but number of
        // controls that can be enabled are different.
        LOGD("Reconfigure, different usecases, pstUcOld=%p, pstUcNew=%p",
             pstPlCb->pstUcOpen, pstUc);
        u32 = u32VppPipeline_SubmodCtrlSet(pstCb->pstPlCb, pstPlCb->pstUcOpen,
                                           pstCtrl, VPP_TRUE);
        LOGE_IF(u32 != VPP_OK, "SubmodCtrlSet (disable) returned %u",
                u32);

        u32 = u32VppPipeline_SubmodCtrlSet(pstCb->pstPlCb, pstUc, pstCtrl,
                                           VPP_FALSE);
        LOGE_IF(u32 != VPP_OK, "SubmodCtrlSet (enable) returned %u",
                u32);
    }

    u32 = u32VppPipeline_SubmodOpen(pstCb->pstPlCb);
    if (u32 != VPP_OK)
    {
        LOGE("SubmodOpen returned %u", u32);
        goto ERROR_OPEN;
    }

    pstPlCb->pstUcOpen = pstUc;
    LOG_EXIT_RET(u32);

ERROR_OPEN:
ERROR_PARAMSET:
ERROR_CLOSE:
    u32VppPipeline_SubmodTerm(pstCb->pstPlCb);
    pstPlCb->pstUcOpen = NULL;
    LOG_EXIT_RET(u32);
}

static uint32_t u32VppPipeline_HandleReconfigureCompEqRcfg(t_StVppCtx *pstCb,
                                                           t_StVppPipelineCb *pstPlCb,
                                                           t_StVppUsecase *pstUc,
                                                           t_StVppHqvCtrl *pstCtrl)
{
    uint32_t u32 = VPP_OK;

    VPP_UNUSED(pstCb);
    VPP_UNUSED(pstCtrl);

    LOG_ENTER();

    LOGD("optimized reconfigure of pipeline");
    u32 = u32VppPipeline_SubmodReconfigure(pstPlCb, pstUc);
    if (u32 != VPP_OK)
    {
        LOGE("SubmodReconfigure returned %u", u32);
        goto ERROR_RECONFIGURE;
    }

    pstPlCb->pstUcOpen = pstUc;
    LOG_EXIT_RET(u32);

ERROR_RECONFIGURE:
    u32VppPipeline_SubmodTerm(pstCb->pstPlCb);
    pstPlCb->pstUcOpen = NULL;
    LOG_EXIT_RET(u32);
}

static uint32_t u32VppPipeline_HandleReconfigureCompEqual(t_StVppCtx *pstCb,
                                                          t_StVppPipelineCb *pstPlCb,
                                                          t_StVppUsecase *pstUc,
                                                          t_StVppHqvCtrl *pstCtrl)
{
    uint32_t u32 = VPP_OK;

    if (pstUc == pstPlCb->pstUcOpen &&
        u32VppPipeline_SubmodImplementReconfigure(pstPlCb))
    {
        u32 = u32VppPipeline_HandleReconfigureCompEqRcfg(pstCb, pstPlCb,
                                                         pstUc, pstCtrl);
    }
    else
    {
        u32 = u32VppPipeline_HandleReconfigureCompEqCloseOpen(pstCb, pstPlCb,
                                                              pstUc, pstCtrl);
    }

    if (u32 != VPP_OK)
    {
        LOGE("u32VppPipeline_HandleReconfigureCompEqual, u32=%u", u32);
        return VPP_ERR;
    }

    u32 = u32VppPipeline_SubmodGetBufReq(pstPlCb);
    LOGE_IF(u32 != VPP_OK, "SubmodGetBufReq returned %u", u32);

    if ((u32 == VPP_OK) && (pstPlCb->u32IpCnt > 1))
    {
        u32 = u32VppPipeline_ReconfigInternalBuffers(pstCb);
        if (u32 != VPP_OK)
        {
            LOGE("ReconfigInternalBuffers returned %u", u32);
        }
    }

    return u32;
}

static uint32_t u32VppPipeline_HandleReconfigureCompDiff(t_StVppCtx *pstCb,
                                                         t_StVppPipelineCb *pstPlCb,
                                                         t_StVppUsecase *pstUc,
                                                         t_StVppHqvCtrl *pstCtrl)
{
    uint32_t u32;

    LOG_ENTER();

    if (!pstUc)
        return VPP_ERR_PARAM;

    // The usecase composition is different, thus there can either be a real
    // usecase that is opened, or bypass.
    if (pstPlCb->pstUcOpen)
    {
        u32 = u32VppPipeline_InternalClose(pstCb);
        if (u32 != VPP_OK)
        {
            LOGE("u32VppPipeline_InternalClose, u32=%u", u32);
            return VPP_ERR;
        }
    }

    u32 = u32VppPipeline_InternalOpen(pstCb, pstCtrl, pstUc);
    if (u32 != VPP_OK)
    {
        LOGE("u32VppPipeline_InternalOpen, u32=%u", u32);
        return VPP_ERR;
    }

    return u32;
}

static void vVppPipeline_FlushPort(t_StVppCtx *pstCtx,
                                   t_StVppPipelineCb *pstPlCb,
                                   enum vpp_port ePort)
{
    t_StVppBuf *pstBuf;
    t_StVppBufPool pstQ;

    pthread_mutex_lock(&pstPlCb->mutex);
    if (ePort == VPP_PORT_INPUT)
    {
        while ((pstBuf = pstVppBufPool_Get(&pstPlCb->stInputQ)) != NULL)
        {
            pstBuf->stPixel.u32FilledLen = 0;
            VPP_STATS(pstPlCb, FlushedInputBuf);
            VPP_LOG_BUF(pstCtx, pstBuf, eVppLogId_PlBufDone);
            u32VppIp_CbBufDone(&pstPlCb->stCallbacks, ePort, pstBuf);
        }
    }
    else
    {
        while ((pstBuf = pstVppBufPool_Get(&pstPlCb->stOutputQ)) != NULL)
        {
            pstBuf->stPixel.u32FilledLen = 0;
            VPP_STATS(pstPlCb, FlushedOutputBuf);
            VPP_LOG_BUF(pstCtx, pstBuf, eVppLogId_PlBufDone);
            u32VppIp_CbBufDone(&pstPlCb->stCallbacks, ePort, pstBuf);
        }
    }
    pthread_mutex_unlock(&pstPlCb->mutex);
}

static uint32_t u32VppPipeline_ProcCmdFlush(t_StVppCtx *pstCtx,
                                            t_StVppPipelineCb *pstPlCb,
                                            t_StVppPipelineCmd *pstCmd)
{
    t_StVppEvt stEvt;
    enum vpp_port ePort;
    t_StVppBuf *pstBuf;
    uint32_t u32Ret = VPP_OK;

    LOG_ENTER();

    if (pstCmd->eCmd != VPP_PL_CMD_FLUSH)
        return VPP_ERR_PARAM;

    ePort = pstCmd->flush.ePort;

    if ((VPP_PL_STATE_EQUAL(pstPlCb, VPP_PLSTATE_INITED)) || (pstPlCb->bBypass))
    {
        vVppPipeline_FlushPort(pstCtx, pstPlCb, ePort);
        stEvt.eType = VPP_EVT_FLUSH_DONE;
        stEvt.flush.ePort = ePort;
        u32VppIpCbEvent(&pstPlCb->stCallbacks, stEvt);
        LOG_EXIT_RET(VPP_OK);
    }

    if (pstPlCb->u32IpCnt > 1)
    {
        if (ePort == VPP_PORT_INPUT)
        {
            pthread_mutex_lock(&pstPlCb->mutex);
            pstPlCb->pstIpInfo[0].u32PendingFlags |= PL_INPUT_FLUSH_PENDING;
            pthread_mutex_unlock(&pstPlCb->mutex);
        }
        else
        {
            pthread_mutex_lock(&pstPlCb->mutex);
            pstPlCb->pstIpInfo[VPP_PL_LAST_IP_IDX(pstPlCb)].u32PendingFlags |=
                PL_OUTPUT_FLUSH_PENDING;
            pthread_mutex_unlock(&pstPlCb->mutex);
        }

        if (!VPP_FLAG_IS_SET(pstPlCb->u32PipelineFlags, PL_INTERNAL_FLUSH_PENDING))
        {
            LOGI("Flush internal buffers");
            u32VppPipeline_FlushInternalBuffers(pstPlCb);
            if (VPP_PL_STATE_EQUAL(pstPlCb, VPP_PLSTATE_ACTIVE))
                vVppPipeline_SetState(pstPlCb, VPP_PLSTATE_PENDING_ACTIVE);
        }
        if (ePort == VPP_PORT_INPUT)
        {
            vVppPipeline_FlushIbdQueue(pstCtx);
            LOGI("Flush IP[0] input");
            u32Ret = IP_FLUSH(pstPlCb, 0, ePort);
            if (u32Ret != VPP_OK)
            {
                IP_LOG(E, pstPlCb, 0, "IP_FLUSH returned u32Ret=%u", u32Ret);
                pthread_mutex_lock(&pstPlCb->mutex);
                pstPlCb->pstIpInfo[0].u32PendingFlags &= ~PL_INPUT_FLUSH_PENDING;
                pthread_mutex_unlock(&pstPlCb->mutex);
            }
            pthread_mutex_lock(&pstPlCb->mutex);
            while (pstPlCb->pstIpInfo[0].u32PendingFlags & (PL_INPUT_FLUSH_PENDING))
            {
                pthread_cond_wait(&pstPlCb->cond_from_worker, &pstPlCb->mutex);
            }
            pthread_mutex_unlock(&pstPlCb->mutex);
        }
        else
        {
            LOGI("Flush IP[%u] output", (VPP_PL_LAST_IP_IDX(pstPlCb)));
            u32Ret = IP_FLUSH(pstPlCb, VPP_PL_LAST_IP_IDX(pstPlCb), ePort);
            if (u32Ret != VPP_OK)
            {
                IP_LOG(E, pstPlCb, VPP_PL_LAST_IP_IDX(pstPlCb),
                       "IP_FLUSH returned u32Ret=%u", u32Ret);
                pthread_mutex_lock(&pstPlCb->mutex);
                pstPlCb->pstIpInfo[VPP_PL_LAST_IP_IDX(pstPlCb)].u32PendingFlags &=
                    ~PL_OUTPUT_FLUSH_PENDING;
                pthread_mutex_unlock(&pstPlCb->mutex);
            }
            pthread_mutex_lock(&pstPlCb->mutex);
            while (pstPlCb->pstIpInfo[VPP_PL_LAST_IP_IDX(pstPlCb)].u32PendingFlags &
                   (PL_OUTPUT_FLUSH_PENDING))
            {
                pthread_cond_wait(&pstPlCb->cond_from_worker, &pstPlCb->mutex);
            }
            pthread_mutex_unlock(&pstPlCb->mutex);
        }
    }

    LOG_EXIT_RET(u32Ret);
}

static uint32_t u32VppPipeline_ProcCmdDrainRequest(t_StVppCtx *pstCtx,
                                                   t_StVppPipelineCb *pstPlCb,
                                                   t_StVppPipelineCmd *pstCmd)
{
    VPP_UNUSED(pstCmd);

    t_StVppBuf *pstBuf;
    t_StVppPipelineCmd stCmd;
    memset(&stCmd, 0, sizeof(t_StVppPipelineCmd));

    pthread_mutex_lock(&pstPlCb->mutex);
    while ((pstBuf = pstVppBufPool_Get(&pstPlCb->stInputQ)) != NULL)
    {
        VPP_STATS(pstPlCb, IssueOBD);
        VPP_LOG_BUF(pstCtx, pstBuf, eVppLogId_PlBufDone);
        u32VppIp_CbBufDone(&pstPlCb->stCallbacks, VPP_PORT_OUTPUT, pstBuf);
    }
    pthread_mutex_unlock(&pstPlCb->mutex);

    stCmd.eCmd = VPP_PL_CMD_DRAIN_COMPLETE;
    u32VppPipeline_CmdPut(pstPlCb, stCmd);

    return VPP_OK;
}

static uint32_t u32VppPipeline_ProcCmdDrainComplete(t_StVppCtx *pstCtx,
                                                    t_StVppPipelineCb *pstPlCb,
                                                    t_StVppPipelineCmd *pstCmd)
{
    VPP_UNUSED(pstCtx);
    VPP_UNUSED(pstCmd);

    t_StVppEvt stEvt;

    if (VPP_FLAG_IS_SET(pstPlCb->u32PipelineFlags, PL_RECONFIGURE_PENDING))
    {
        u32VppStats_Start(pstPlCb->pstCtx, pstPlCb->pvStatsHdl, PL_STAT_RCFG_RAW);

        pthread_mutex_lock(&pstPlCb->mutex);
        VPP_FLAG_SET(pstPlCb->u32PipelineFlags, PL_RECONFIGURE_CALLBACK);
        pthread_mutex_unlock(&pstPlCb->mutex);

        stEvt.eType = VPP_EVT_RECONFIG_RDY;
        u32VppIpCbEvent(&pstPlCb->stCallbacks, stEvt);

        pthread_mutex_lock(&pstPlCb->mutex);
        VPP_FLAG_CLR(pstPlCb->u32PipelineFlags, PL_RECONFIGURE_CALLBACK);
        pthread_mutex_unlock(&pstPlCb->mutex);

        if (VPP_FLAG_IS_SET(pstPlCb->u32PipelineFlags, PL_RECONFIGURE_PENDING))
        {
            pstPlCb->u32ReconfigErr = VPP_ERR;
            VPP_FLAG_CLR(pstPlCb->u32PipelineFlags, PL_RECONFIGURE_PENDING);
            LOGE("Pipeline was not reconfigured during callback");
        }

        stEvt.eType = VPP_EVT_RECONFIG_DONE;
        stEvt.reconfig.u32Err = pstPlCb->u32ReconfigErr;
        if (pstPlCb->bBypass)
            stEvt.reconfig.pstUc = NULL;
        else
            stEvt.reconfig.pstUc = pstPlCb->pstUcOpen;

        u32VppStats_Stop(pstPlCb->pstCtx, pstPlCb->pvStatsHdl, PL_STAT_RCFG_RAW);
        u32VppStats_Stop(pstPlCb->pstCtx, pstPlCb->pvStatsHdl, PL_STAT_RCFG_LATENCY);
        u32VppIpCbEvent(&pstPlCb->stCallbacks, stEvt);
    }
    else
    {
        LOGE("Called with invalid state");
        return VPP_ERR_STATE;
    }

    return VPP_OK;
}

static uint32_t u32VppPipeline_ProcCmdClose(t_StVppCtx *pstCtx,
                                            t_StVppPipelineCb *pstPlCb)
{
    uint32_t u32Ret;

    LOG_ENTER();

    if (!pstCtx || !pstPlCb)
    {
        LOGE("Null parameters! pstCtx=%p, pstPlCb=%p", pstCtx, pstPlCb);
        return VPP_ERR_PARAM;
    }
    else
    {
        vVppPipeline_DumpBufStats(pstPlCb);

        u32Ret = u32VppPipeline_InternalClose(pstCtx);
        LOGE_IF(u32Ret, "u32VppPipeline_InternalClose failed! u32Ret=%u", u32Ret);

        vVppPipeline_SetState(pstPlCb, VPP_PLSTATE_INITED);
    }
    pstPlCb->async_res.u32CloseRet = u32Ret;

    LOGI("%s() posting semaphore", __func__);
    sem_post(&pstPlCb->sem);

    return u32Ret;
}

uint32_t u32VppPipeline_WorkerShouldSleep(t_StVppPipelineCb *pstPlCb)
{
    uint32_t u32Ret = VPP_TRUE;
    uint32_t u32CmdQSz, u32InQSz, u32OutQSz;

    u32CmdQSz = vpp_queue_count(&pstPlCb->stCmdQ);
    u32InQSz = u32VppBufPool_Cnt(&pstPlCb->stInputQ);
    u32OutQSz = u32VppBufPool_Cnt(&pstPlCb->stOutputQ);

    if (u32CmdQSz)
    {
        u32Ret = VPP_FALSE;
    }
    else if (VPP_PL_STATE_EQUAL(pstPlCb, VPP_PLSTATE_ACTIVE) &&
             pstPlCb->bBypass && u32InQSz)
    {
        u32Ret = VPP_FALSE;
    }

    LOGI("CHECK: shouldSleep=%u, u32CmdQSz=%u, eState=%u, bBypass=%u, "
         "u32InQSz=%u, u32OutQSz=%u", u32Ret, u32CmdQSz, pstPlCb->eState,
         pstPlCb->bBypass, u32InQSz, u32OutQSz);

    return u32Ret;
}

void *vpVppPipeline_Worker(void *pv)
{
    uint32_t u32Ret;
    t_StVppCtx *pstCtx;
    t_StVppPipelineCb *pstPlCb;
    t_StVppPipelineCmd stCmd;

    LOGD("%s started", __func__);

    pstCtx = (t_StVppCtx *)pv;
    pstPlCb = pstCtx->pstPlCb;

    pthread_mutex_lock(&pstPlCb->mutex);
    VPP_FLAG_SET(pstPlCb->u32PipelineFlags, PL_WORKER_STARTED);
    pthread_mutex_unlock(&pstPlCb->mutex);
    pthread_cond_signal(&pstPlCb->cond_from_worker);

    while (1)
    {
        pthread_mutex_lock(&pstPlCb->mutex);
        while (u32VppPipeline_WorkerShouldSleep(pstPlCb))
            pthread_cond_wait(&pstPlCb->cond_to_worker, &pstPlCb->mutex);

        u32Ret = u32VppPipeline_CmdGet(pstPlCb, &stCmd);
        if (u32Ret == VPP_OK)
        {
            pthread_mutex_unlock(&pstPlCb->mutex);

            // Process the command
            LOG_PL_CMD("PL:ProcessCmd", stCmd.eCmd);

            if (stCmd.eCmd == VPP_PL_CMD_THREAD_EXIT)
                break;

            else if (stCmd.eCmd == VPP_PL_CMD_FLUSH)
                u32VppPipeline_ProcCmdFlush(pstCtx, pstPlCb, &stCmd);

            else if (stCmd.eCmd == VPP_PL_CMD_DRAIN_REQUEST)
                u32VppPipeline_ProcCmdDrainRequest(pstCtx, pstPlCb, &stCmd);

            else if (stCmd.eCmd == VPP_PL_CMD_DRAIN_COMPLETE)
                u32VppPipeline_ProcCmdDrainComplete(pstCtx, pstPlCb, &stCmd);

            else if (stCmd.eCmd == VPP_PL_CMD_QUEUE_INTERNAL)
                u32VppPipeline_QueueInternalBuffers(pstCtx);

            else if (stCmd.eCmd == VPP_PL_CMD_CLOSE)
                u32VppPipeline_ProcCmdClose(pstCtx, pstPlCb);

            else
                LOGE("unknown command in queue");

            continue;
        }

        if (!VPP_PL_STATE_EQUAL(pstPlCb, VPP_PLSTATE_ACTIVE))
        {
            LOGD("got buffer, but state is not active");
            pthread_mutex_unlock(&pstPlCb->mutex);
            continue;
        }
        else
        {
            t_StVppBuf *pstBuf;
            pstBuf = pstVppBufPool_Get(&pstPlCb->stInputQ);
            pthread_mutex_unlock(&pstPlCb->mutex);
            if (!pstBuf)
            {
                LOGE("unable to dequeue buffer from pool");
                continue;
            }

            VPP_STATS(pstPlCb, IssueOBD);
            VPP_LOG_BUF(pstCtx, pstBuf, eVppLogId_PlBufDone);
            u32VppIp_CbBufDone(&pstPlCb->stCallbacks, VPP_PORT_OUTPUT, pstBuf);
        }
    }

    LOGD("%s exited", __func__);

    return NULL;
}

/************************************************************************
 * Global Functions
 ***********************************************************************/

uint32_t u32VppPipeline_Init(t_StVppCtx *pstCtx, t_StVppCallback stCbs)
{
    int rc = 0;
    uint32_t u32;
    void *pvStatsHdl = NULL;
    pthread_mutexattr_t int_flush_mutexattr;

   t_StVppQueue stCmdQ;
    t_StVppPipelineCb *pstPlCb;

    if (!pstCtx)
        return VPP_ERR_PARAM;

    pstPlCb = calloc(1, sizeof(t_StVppPipelineCb));

    if (!pstPlCb)
        return VPP_ERR_NO_MEM;

    pstCtx->pstPlCb = pstPlCb;
    pstPlCb->pstCtx = pstCtx;
    pstPlCb->stCallbacks = stCbs;

    u32 = u32VppStats_Register(pstCtx, astPlStatsCfg, u32PlStatCnt, &pvStatsHdl);
    LOGE_IF(u32 != VPP_OK, "unable to register pipeline stats, u32=%u", u32);
    pstPlCb->pvStatsHdl = pvStatsHdl;

    u32VppBufPool_Init(&pstPlCb->stInputQ);
    u32VppBufPool_Init(&pstPlCb->stOutputQ);
    u32VppBufPool_Init(&pstPlCb->stIbdQ);

    vpp_queue_init(&pstPlCb->stCmdQ, PL_CMD_Q_SZ);

    rc = pthread_mutex_init(&pstPlCb->mutex, NULL);
    if (rc)
        goto ERR_MUTEX_INIT;

    rc = pthread_mutexattr_init (&int_flush_mutexattr);
    if (rc)
        goto ERR_COND_FLUSH_MUTEXATTR_INIT;

    rc = pthread_mutexattr_settype (&int_flush_mutexattr, PTHREAD_MUTEX_RECURSIVE);
    if (rc)
        goto ERR_COND_FLUSH_MUTEXATTR_SETTYPE;

    rc = pthread_mutex_init(&pstPlCb->int_flush_mutex, &int_flush_mutexattr);
    if (rc)
        goto ERR_COND_FLUSH_MUTEXATTR_SETTYPE;

    rc = pthread_cond_init(&pstPlCb->cond_to_worker, NULL);
    if (rc)
        goto ERR_COND_TW_INIT;

    rc = pthread_cond_init(&pstPlCb->cond_from_worker, NULL);
    if (rc)
        goto ERR_COND_FW_INIT;

    rc = pthread_create(&pstPlCb->thread, NULL, vpVppPipeline_Worker, pstCtx);
    if (rc)
        goto ERR_THREAD_CREATE;

    pthread_mutex_lock(&pstPlCb->mutex);
    while (!VPP_FLAG_IS_SET(pstPlCb->u32PipelineFlags, PL_WORKER_STARTED))
        pthread_cond_wait(&pstPlCb->cond_from_worker, &pstPlCb->mutex);
    pthread_mutex_unlock(&pstPlCb->mutex);

    vVppPipeline_SetState(pstPlCb, VPP_PLSTATE_INITED);

    return VPP_OK;

ERR_THREAD_CREATE:
    pthread_cond_destroy(&pstPlCb->cond_from_worker);

ERR_COND_FW_INIT:
    pthread_cond_destroy(&pstPlCb->cond_to_worker);

ERR_COND_TW_INIT:
    pthread_mutex_destroy(&pstPlCb->int_flush_mutex);

ERR_COND_FLUSH_MUTEXATTR_SETTYPE:
    pthread_mutexattr_destroy (&int_flush_mutexattr);

ERR_COND_FLUSH_MUTEXATTR_INIT:
    pthread_mutex_destroy(&pstPlCb->mutex);

ERR_MUTEX_INIT:
    u32 = u32VppStats_Unregister(pstCtx, pstPlCb->pvStatsHdl);
    LOGE_IF(u32 != VPP_OK, "ec: unable to unregister pipeline stats, u32=%u", u32);

    free(pstPlCb);
    pstCtx->pstPlCb = NULL;

    return VPP_ERR;
}

uint32_t u32VppPipeline_Term(t_StVppCtx *pstCb)
{
    uint32_t u32;
    t_StVppPipelineCb *pstPlCb;
    t_StVppBuf *pstBuf;
    t_StVppPipelineCmd stCmd;
    struct hqv_ctrl_custom stCustCtrlTmp;

    if (!pstCb || !pstCb->pstPlCb)
        return VPP_ERR_PARAM;

    pstPlCb = VPP_PL_CB_GET(pstCb);

    stCmd.eCmd = VPP_PL_CMD_THREAD_EXIT;
    u32VppPipeline_CmdPut(pstPlCb, stCmd);

    pthread_join(pstPlCb->thread, NULL);

    // Flush out input and output pool if they're not empty...
    vVppPipeline_FlushPort(pstCb, pstPlCb, VPP_PORT_INPUT);
    vVppPipeline_FlushPort(pstCb, pstPlCb, VPP_PORT_OUTPUT);

    vpp_queue_term(&pstPlCb->stCmdQ);

    u32VppBufPool_Term(&pstPlCb->stInputQ);
    u32VppBufPool_Term(&pstPlCb->stOutputQ);
    u32VppBufPool_Term(&pstPlCb->stIbdQ);

    // Flush out custom controls if there are any
    while(u32VppPipeline_CustomCtrlGet(pstPlCb, VPP_FALSE, &stCustCtrlTmp) == VPP_OK);

    pthread_cond_destroy(&pstPlCb->cond_to_worker);
    pthread_cond_destroy(&pstPlCb->cond_from_worker);
    pthread_mutex_destroy(&pstPlCb->mutex);
    pthread_mutex_destroy(&pstPlCb->int_flush_mutex);

    u32 = u32VppStats_Unregister(pstPlCb->pstCtx, pstPlCb->pvStatsHdl);
    LOGE_IF(u32 != VPP_OK, "unable to unregister pipeline stats, u32=%u", u32);

    free(pstPlCb);
    pstCb->pstPlCb = NULL;

    return VPP_OK;
}

uint32_t u32VppPipeline_Open(t_StVppCtx *pstCb, t_StVppUsecase *pstUc,
                             t_StVppParam *pstParam, t_StVppHqvCtrl *pstCtrl)
{
    uint32_t u32Ret = VPP_OK;
    uint32_t u32SubmodCnt;
    t_StVppPipelineCb *pstPlCb;

    if (!pstCb || !pstCb->pstPlCb || !pstCtrl)
        return VPP_ERR_PARAM;

    pstPlCb = VPP_PL_CB_GET(pstCb);

    if (!VPP_PL_STATE_EQUAL(pstPlCb, VPP_PLSTATE_INITED))
        return VPP_ERR_STATE;

    u32VppStats_Start(pstPlCb->pstCtx, pstPlCb->pvStatsHdl, PL_STAT_OPEN);

    pstPlCb->pstParam = pstParam;
    pstPlCb->bBypass = VPP_FALSE;

    LOGD("UC=%p (%s)", PRINT_UC(pstUc));
    dump_param(pstParam);
    dump_controls(pstCtrl);

    if (pstUc)
    {
        u32SubmodCnt = u32VppUsecase_SubmodCount(pstUc);
        if (u32SubmodCnt == 0)
        {
            // Bypass
            u32Ret = VPP_ERR_PARAM;
            LOGD("IP block exceeds max supported in pipeline, cnt=%u", u32SubmodCnt);
        }
        else
        {
            u32Ret = u32VppPipeline_InternalOpen(pstCb, pstCtrl, pstUc);
            if (u32Ret != VPP_OK)
            {
                LOGE("IP open failed, enabling bypass, u32Ret=%u", u32Ret);
            }
        }
    }

    if (!pstUc || u32Ret != VPP_OK)
    {
        LOGD("Enabling pipeline bypass");
        pstPlCb->bBypass = VPP_TRUE;
    }

    vVppPipeline_SetState(pstPlCb, VPP_PLSTATE_PENDING_ACTIVE);

    u32VppStats_Stop(pstPlCb->pstCtx, pstPlCb->pvStatsHdl, PL_STAT_OPEN);

    return u32Ret;
}

uint32_t u32VppPipeline_Close(t_StVppCtx *pstCb)
{
    t_StVppPipelineCb *pstPlCb;
    t_StVppPipelineCmd stCmd;
    uint32_t u32Ret = VPP_OK;

    LOG_ENTER();

    if (!pstCb || !pstCb->pstPlCb)
        return VPP_ERR_PARAM;

    pstPlCb = VPP_PL_CB_GET(pstCb);

    u32VppStats_Start(pstPlCb->pstCtx, pstPlCb->pvStatsHdl, PL_STAT_CLOSE);

    stCmd.eCmd = VPP_PL_CMD_CLOSE;
    u32VppPipeline_CmdPut(pstPlCb, stCmd);

    LOGI(">> waiting on semaphore");
    sem_wait(&pstPlCb->sem);
    LOGI(">> got semaphore");

    u32VppStats_Stop(pstPlCb->pstCtx, pstPlCb->pvStatsHdl, PL_STAT_CLOSE);

    return pstPlCb->async_res.u32CloseRet;
}

uint32_t u32VppPipeline_SetHqvCtrl(t_StVppCtx *pstCb, struct hqv_control ctrl)
{
    uint32_t u32Ret = VPP_OK;
    uint32_t i;

    t_StVppPipelineCb *pstPlCb;

    if (!pstCb || !pstCb->pstPlCb)
        return VPP_ERR_PARAM;

    pstPlCb = VPP_PL_CB_GET(pstCb);

    if (pstPlCb->eState == VPP_PLSTATE_INITED)
    {
        if (ctrl.ctrl_type != HQV_CONTROL_CUST)
        {
            LOGE("Invalid set control in pipeline");
            return VPP_ERR_STATE;
        }

        LOGI("received custom control in inited state");
        vVppPipeline_CustomCtrlPut(pstPlCb, &ctrl.custom);
        return VPP_OK;
    }
    else if ((pstPlCb->eState != VPP_PLSTATE_ACTIVE) &&
             (pstPlCb->eState != VPP_PLSTATE_PENDING_ACTIVE))
    {
        return VPP_ERR_STATE;
    }

    // This function ignores HQV mode in the top level control. It is expected
    // that the caller of this function will filter out any calls if the
    // HQV mode is invalid. This function just passes on the ctrl and params.
    for (i = 0; i < pstPlCb->u32IpCnt; i++)
    {
        if (ctrl.ctrl_type == HQV_CONTROL_CUST)
        {
            if (u32VppPipeline_IsCustCtrlForIp(pstPlCb, i, &ctrl.custom))
            {
                IP_LOG(D, pstPlCb, i, "setting custom ctrl=%u", ctrl.ctrl_type);
                u32Ret = IP_SET_CTRL(pstPlCb, i, ctrl);
            }
            else
            {
                vVppPipeline_CustomCtrlPut(pstPlCb, &ctrl.custom);
            }
        }
        else if (u32VppIp_SupportsAlgo(IP_GET(pstPlCb, i).ip, ctrl.ctrl_type))
        {
            IP_LOG(D, pstPlCb, i, "setting ctrl=%u", ctrl.ctrl_type);
            u32Ret = IP_SET_CTRL(pstPlCb, i, ctrl);
        }
    }

    return u32Ret;
}


uint32_t u32VppPipeline_Reconfigure(t_StVppCtx *pstCb, t_StVppUsecase *pstUc,
                                    t_StVppParam *pstParam, t_StVppHqvCtrl *pstCtrl)
{
    uint32_t u32Ret = VPP_OK;
    uint32_t bCompEq, u32Dest;
    t_StVppPipelineCb *pstPlCb;
    t_StVppBuf *pstBuf;

    if (!pstCb || !pstCb->pstPlCb || !pstCtrl || !pstParam)
        return VPP_ERR_PARAM;

    pstPlCb = VPP_PL_CB_GET(pstCb);

    LOG_ENTER();

    if (!VPP_FLAG_IS_SET(pstPlCb->u32PipelineFlags, PL_RECONFIGURE_CALLBACK))
    {
        LOGE("Called when reconfigure callback flag not set!");
        LOG_EXIT();
        return VPP_ERR_STATE;
    }

    dump_param(pstParam);
    dump_controls(pstCtrl);

    pstPlCb->pstParam = pstParam;

    LOGD("%s, bBypass=%u, uc_open:{%p, %s}, uc_new:{%p, %s}, ", __func__,
         pstPlCb->bBypass, PRINT_UC(pstPlCb->pstUcOpen), PRINT_UC(pstUc));

    if (pstUc)
    {
        bCompEq = u32VppUsecase_CompositionEqual(pstUc, pstPlCb->pstUcOpen);
        LOGD("%s, bCompEqual=%u", __func__, bCompEq);

        if (pstPlCb->bBypass)
        {
            LOGD("Reconfiguring out of bypass");
            pstPlCb->bBypass = VPP_FALSE;
        }

        if (bCompEq)
        {
            // Identical usecases implies equal composition as well
            u32Ret = u32VppPipeline_HandleReconfigureCompEqual(pstCb, pstPlCb,
                                                               pstUc, pstCtrl);
            LOGE_IF(u32Ret != VPP_OK, "HandleReconfigureCompEqual, u32=%u", u32Ret);
        }
        else
        {
            u32Ret = u32VppPipeline_HandleReconfigureCompDiff(pstCb, pstPlCb,
                                                              pstUc, pstCtrl);
            LOGE_IF(u32Ret != VPP_OK, "HandleReconfigureCompDiff, u32=%u", u32Ret);
        }

        if (u32Ret == VPP_OK)
        {

            uint32_t u32;
            while ((pstBuf = pstVppBufPool_Get(&pstPlCb->stInputQ)) != NULL)
            {
                pstBuf->eCurrentIpPort = VPP_PORT_INPUT;
                pthread_mutex_lock(&pstPlCb->mutex);
                BUF_CNT_QUEUE(pstPlCb, 0, VPP_PORT_INPUT, pstBuf);
                pthread_mutex_unlock(&pstPlCb->mutex);
                u32 = IP_QUEUE_BUF(pstPlCb, 0, VPP_PORT_INPUT, pstBuf);
                if (u32 != VPP_OK)
                {
                    LOGE("queue in buf to reconfigured IP[0] failed, u32=%u", u32);
                    pthread_mutex_lock(&pstPlCb->mutex);
                    BUF_CNT_DEQUEUE(pstPlCb, 0, VPP_PORT_INPUT, pstBuf);
                    pthread_mutex_unlock(&pstPlCb->mutex);
                }
            }
            while ((pstBuf = pstVppBufPool_Get(&pstPlCb->stOutputQ)) != NULL)
            {
                pstBuf->eCurrentIpPort = VPP_PORT_OUTPUT;
                u32Dest = u32VppPipeline_FindOutputBufDest(pstPlCb);
                pthread_mutex_lock(&pstPlCb->mutex);
                BUF_CNT_QUEUE(pstPlCb, u32Dest, VPP_PORT_OUTPUT, pstBuf);
                pthread_mutex_unlock(&pstPlCb->mutex);
                u32 = IP_QUEUE_BUF(pstPlCb, u32Dest, VPP_PORT_OUTPUT, pstBuf);
                if (u32 != VPP_OK)
                {
                    LOGE("queue out buf to reconfigured IP[%d] failed, u32=%u", u32Dest, u32);
                    pthread_mutex_lock(&pstPlCb->mutex);
                    BUF_CNT_DEQUEUE(pstPlCb, u32Dest, VPP_PORT_OUTPUT, pstBuf);
                    pthread_mutex_unlock(&pstPlCb->mutex);
                }
            }
        }

    }

    if (u32Ret != VPP_OK || !pstUc)
    {
        // pstUc being NULL signifies that we were requested to be reconfigured
        // into bypass mode. If u32Ret != VPP_OK, then there was an error
        // during reconfiguration and we have to put the pipeline into bypass.
        // In this case, we need to also report an error.
        //
        // If pstUc == NULL, we could have been reconfigured from a previous
        // bypass state to a new bypass state, or we are being reconfigured
        // from a normal running state to bypass state. If the latter is the
        // case, we need to make sure that we get all buffers out of the
        // pipeline.
        //
        // If we are the former, we need to make sure that we are reporting the
        // error so that we can tell the usecase / concurrency manager.

        LOGD("Reconfiguring into bypass mode, pstUc=%p, u32Ret=%u", pstUc, u32Ret);

        if (!pstPlCb->bBypass && !pstUc)
        {
            // Normal mode, going into bypass
            u32Ret = u32VppPipeline_FlushIpHoldBufs(pstPlCb, PL_RECONFIGURE_TO_BYPASS);
        }

        if (u32Ret != VPP_OK)
        {
            // Bypass or Normal mode, going into normal mode, error.
            pstPlCb->u32ReconfigErr = u32Ret;
        }

        pstPlCb->bBypass = VPP_TRUE;
    }

    VPP_FLAG_CLR(pstPlCb->u32PipelineFlags, PL_RECONFIGURE_PENDING);

    LOG_EXIT();

    return u32Ret;
}

uint32_t u32VppPipeline_QueueBuf(t_StVppCtx *pstCb, enum vpp_port ePort,
                                 t_StVppBuf *pstBuf)
{
    uint32_t u32IpCnt, u32Ret = VPP_OK;
    t_StVppPipelineCb *pstPlCb;
    t_StVppPipelineCmd stCmd;
    uint32_t u32Dest = 0;

    if (!pstCb || !pstCb->pstPlCb || !pstBuf)
        return VPP_ERR_PARAM;

    if (ePort != VPP_PORT_INPUT && ePort != VPP_PORT_OUTPUT)
        return VPP_ERR_PARAM;

    pstPlCb = VPP_PL_CB_GET(pstCb);
    u32IpCnt = pstPlCb->u32IpCnt;

    if ((!VPP_PL_STATE_EQUAL(pstPlCb, VPP_PLSTATE_ACTIVE)) &&
        (!VPP_PL_STATE_EQUAL(pstPlCb, VPP_PLSTATE_PENDING_ACTIVE)))
        return VPP_ERR_STATE;

    if ((ePort == VPP_PORT_INPUT) && (VPP_PL_STATE_EQUAL(pstPlCb, VPP_PLSTATE_PENDING_ACTIVE)))
    {
        vVppPipeline_SetState(pstPlCb, VPP_PLSTATE_ACTIVE);
        if (u32IpCnt > 1)
        {
            stCmd.eCmd = VPP_PL_CMD_QUEUE_INTERNAL;
            u32VppPipeline_CmdPut(pstPlCb, stCmd);

        }
    }
    pstBuf->eCurrentIpPort = ePort;
    VPP_LOG_BUF(pstCb, pstBuf, eVppLogId_PlQueueBuf);

    pthread_mutex_lock(&pstPlCb->mutex);
    if (pstPlCb->bBypass == VPP_FALSE &&
        !VPP_FLAG_IS_SET(pstPlCb->u32PipelineFlags, PL_RECONFIGURE_CALLBACK))
    {
        pthread_mutex_unlock(&pstPlCb->mutex);
        if (ePort == VPP_PORT_INPUT)
        {
            pthread_mutex_lock(&pstPlCb->mutex);
            BUF_CNT_QUEUE(pstPlCb, 0, ePort, pstBuf);
            pthread_mutex_unlock(&pstPlCb->mutex);
            u32Ret = IP_QUEUE_BUF(pstPlCb, 0, ePort, pstBuf);
            if (u32Ret != VPP_OK)
            {
                pthread_mutex_lock(&pstPlCb->mutex);
                BUF_CNT_DEQUEUE(pstPlCb, 0, ePort, pstBuf);
                pthread_mutex_unlock(&pstPlCb->mutex);
            }
        }
        else
        {
            u32Dest = u32VppPipeline_FindOutputBufDest(pstPlCb);
            LOGI("Queue output buf to IP[%d], pstBuf %p", u32Dest, pstBuf);
            pthread_mutex_lock(&pstPlCb->mutex);
            BUF_CNT_QUEUE(pstPlCb, u32Dest, ePort, pstBuf);
            pthread_mutex_unlock(&pstPlCb->mutex);
            u32Ret = IP_QUEUE_BUF(pstPlCb, u32Dest, ePort, pstBuf);
            if (u32Ret != VPP_OK)
            {
                pthread_mutex_lock(&pstPlCb->mutex);
                BUF_CNT_DEQUEUE(pstPlCb, u32Dest, ePort, pstBuf);
                pthread_mutex_unlock(&pstPlCb->mutex);
            }
        }
    }
    else
    {
        if (ePort == VPP_PORT_INPUT)
            u32VppBufPool_Put(&pstPlCb->stInputQ, pstBuf);
        else
            u32VppBufPool_Put(&pstPlCb->stOutputQ, pstBuf);

        pthread_mutex_unlock(&pstPlCb->mutex);
        pthread_cond_signal(&pstPlCb->cond_to_worker);
    }
    vVppPipeline_DumpBufStats(pstPlCb);
    return u32Ret;
}

uint32_t u32VppPipeline_RequestFlush(t_StVppCtx *pstCb, enum vpp_port ePort)
{
    t_StVppPipelineCb *pstPlCb;
    t_StVppPipelineCmd stCmd;
    uint32_t u32Ret = VPP_OK;
    uint32_t bAsyncFlush = VPP_FALSE;

    if (!pstCb || !pstCb->pstPlCb)
        return VPP_ERR_PARAM;

    if (ePort != VPP_PORT_INPUT && ePort != VPP_PORT_OUTPUT)
        return VPP_ERR_PARAM;

    pstPlCb = VPP_PL_CB_GET(pstCb);

    if (ePort == VPP_PORT_INPUT)
        VPP_STATS(pstPlCb, RequestFlushInput);
    else
        VPP_STATS(pstPlCb, RequestFlushOutput);

    LOGI("%s(), state=%u, bypass=%u, blocks=%u, port=%u", __func__,
         pstPlCb->eState, pstPlCb->bBypass, pstPlCb->u32IpCnt, ePort);

    if (VPP_PL_STATE_EQUAL(pstPlCb, VPP_PLSTATE_INITED))
    {
        // When inited, there should be no buffers in flight; at most there
        // will be buffers in the holding ports.
        bAsyncFlush = VPP_TRUE;
    }
    else if ((VPP_PL_STATE_EQUAL(pstPlCb, VPP_PLSTATE_ACTIVE)) ||
             (VPP_PL_STATE_EQUAL(pstPlCb, VPP_PLSTATE_PENDING_ACTIVE)))
    {
        // Blocks in the pipeline, or we're in bypass mode
        if (pstPlCb->bBypass)
        {
            bAsyncFlush = VPP_TRUE;
        }
        else if (pstPlCb->u32IpCnt == 1)
        {
            u32Ret = IP_FLUSH(pstPlCb, 0, ePort);
            if (u32Ret != VPP_OK)
            {
                IP_LOG(E, pstPlCb, 0, "IP_FLUSH returned u32Ret=%u", u32Ret);
                return u32Ret;
            }
        }
        else
        {
            bAsyncFlush = VPP_TRUE;
        }
    }
    else
    {
        u32Ret = VPP_ERR;
    }

    if (bAsyncFlush)
    {
        stCmd.eCmd = VPP_PL_CMD_FLUSH;
        stCmd.flush.ePort = ePort;
        u32VppPipeline_CmdPut(pstPlCb, stCmd);
    }

    return u32Ret;
}

uint32_t u32VppPipeline_RequestReconfigure(t_StVppCtx *pstCb)
{
    t_StVppPipelineCb *pstPlCb;
    t_StVppPipelineCmd stCmd;
    uint32_t u32Ret = VPP_OK;
    uint32_t bAsyncDrain = VPP_FALSE;

    if (!pstCb || !pstCb->pstPlCb)
        return VPP_ERR_PARAM;

    pstPlCb = VPP_PL_CB_GET(pstCb);

    VPP_STATS(pstPlCb, RequestReconfigure);

    LOGI("%s(), state=%u, bypass=%u, blocks=%u", __func__, pstPlCb->eState,
         pstPlCb->bBypass, pstPlCb->u32IpCnt);

    if (VPP_FLAG_IS_SET(pstPlCb->u32PipelineFlags, PL_RECONFIGURE_PENDING))
    {
        LOGE("Multiple reconfigure requests received");
        return VPP_ERR_PARAM;
    }

    pstPlCb->u32ReconfigErr = VPP_OK;

    u32VppStats_Start(pstPlCb->pstCtx, pstPlCb->pvStatsHdl, PL_STAT_RCFG_LATENCY);

    if (VPP_PL_STATE_EQUAL(pstPlCb, VPP_PLSTATE_INITED))
    {
        // When inited, there should be no buffers being processed...
        // the worker thread should just be able to return immediately.
        bAsyncDrain = VPP_TRUE;
        LOGD("RequestReconfigure in VPP_PLSTATE_INITED");
    }
    else if ((VPP_PL_STATE_EQUAL(pstPlCb, VPP_PLSTATE_ACTIVE)) ||
             (VPP_PL_STATE_EQUAL(pstPlCb, VPP_PLSTATE_PENDING_ACTIVE)))
    {
        // Blocks in the pipeline, or we're in bypass mode
        if (pstPlCb->bBypass)
        {
            bAsyncDrain = VPP_TRUE;
        }
        else
        {
            VPP_FLAG_SET(pstPlCb->u32PipelineFlags, PL_RECONFIGURE_PENDING);

            u32Ret = IP_DRAIN(pstPlCb, 0);
            if (u32Ret != VPP_OK)
            {
                IP_LOG(E, pstPlCb, 0, "IP_DRAIN returned u32Ret=%u", u32Ret);
                VPP_FLAG_CLR(pstPlCb->u32PipelineFlags, PL_RECONFIGURE_PENDING);
                return u32Ret;
            }
        }
    }
    else
    {
        u32Ret = VPP_ERR;
    }

    if (bAsyncDrain)
    {
        VPP_FLAG_SET(pstPlCb->u32PipelineFlags, PL_RECONFIGURE_PENDING);
        stCmd.eCmd = VPP_PL_CMD_DRAIN_REQUEST;
        u32VppPipeline_CmdPut(pstPlCb, stCmd);
    }

    return u32Ret;
}
