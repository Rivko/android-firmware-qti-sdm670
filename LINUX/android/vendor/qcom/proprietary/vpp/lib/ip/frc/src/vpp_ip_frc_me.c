/*!
 * @file vpp_ip_frc_me.c
 *
 * @cr
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * @services: Interface to Motion Estimation.
 */

#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>

#define VPP_LOG_TAG     VPP_LOG_MODULE_FRC_ME_TAG
#define VPP_LOG_MODULE  VPP_LOG_MODULE_FRC_ME

#include "vpp_dbg.h"
#include "vpp.h"
#include "vpp_buf.h"
#include "vpp_ion.h"
#include "vpp_buf.h"
#include "vpp_callback.h"
#include "vpp_reg.h"
#include "vpp_queue.h"
#include "vpp_ip.h"
#include "vpp_utils.h"
#include "vpp_stats.h"
#include "vpp_ip_frc_me.h"
#include "vpp_ip_frc_me_tme.h"

/************************************************************************
 * Local definitions
 ***********************************************************************/

#define FRC_ME_GET_TIMESTAMP(sec, usec)      (((uint64_t)(sec) * FRC_ME_1SEC) + (usec))
#define FRC_ME_CB_GET(ctx)                   ((t_StVppIpFrcMeCb *)(ctx))
#define FRC_ME_TME_INPQ_INPROC(cb)           ((cb)->u32IpBufsSentToTME - (cb)->u32IpBufsRxedFromTME)
#define FRC_ME_TME_OUTQ_INPROC(cb)           ((cb)->u32OpBufsSentToTME - (cb)->u32OpBufsRxedFromTME)
#define FRC_ME_TME_INPQ_AVAIL(cb)            (FRC_ME_TME_INPQ_INPROC(cb) < (cb)->u32IpBufsTmeQSize)
#define FRC_ME_TME_OUTQ_AVAIL(cb)            (FRC_ME_TME_OUTQ_INPROC(cb) < (cb)->u32OpBufsTmeQSize)
//Debug options
#define FRC_ME_DUMP_BUF_ENABLE               0         //Enable file DUMP
#define FRC_ME_LOG_TME_BUF_STATUS(cb)        LOGI("TME: [I/P In:%u Max:%u]  [O/P In:%u Max:%u]", \
                                                  FRC_ME_TME_INPQ_INPROC((cb)), \
                                                  (cb)->u32IpBufsTmeQSize, \
                                                  FRC_ME_TME_OUTQ_INPROC((cb)), \
                                                  (cb)->u32OpBufsTmeQSize)
#define FRC_ME_LOCK(cb)                      pthread_mutex_lock(&(cb)->mutex)
#define FRC_ME_UNLOCK(cb)                    pthread_mutex_unlock(&(cb)->mutex)
#define FRC_ME_SIGNAL(cb)                    pthread_cond_signal(&(cb)->cond)
#define FRC_ME_WAIT(cb)                      pthread_cond_wait(&(cb)->cond, &(cb)->mutex)
#define FRC_ME_BYPASS_MASK                   (VPP_BUF_FLAG_BYPASS | FRC_ME_BUF_INTERN_BYPASS)
#define FRC_ME_IS_BUF_BYPASS(_p)             VPP_FLAG_IS_SET((_p)->u32InternalFlags, \
                                                             FRC_ME_BYPASS_MASK)

typedef struct {
    uint32_t u32Index;
} t_StVppIpFrcMeExpBuf;

typedef struct {
     uint32_t bProcBufMet    :1;
     uint32_t bQInput        :1;
     uint32_t bQOutput       :1;
     uint32_t bBypassInput   :1;
     uint32_t bQInternalEos  :1;

     uint32_t u32InpFreeIdx;
     uint32_t u32OutFreeIdx;
 }t_StFrcMeStateParams;

/************************************************************************
 * Local static variables
 ************************************************************************/
static const t_StVppStatsConfig astFrcMeStatsCfg[] = {
    VPP_PROF_DECL(FRC_ME_STAT_WORKER, 100, 0),
    VPP_PROF_DECL(FRC_ME_STAT_WORKER_SLEEP, 100, 0),
    VPP_PROF_DECL(FRC_ME_STAT_RECEIVER, 100, 0),
    VPP_PROF_DECL(FRC_ME_STAT_RECEIVER_SLEEP, 100, 0),
};

static const uint32_t u32FrcMeStatCnt = VPP_STATS_CNT(astFrcMeStatsCfg);

/************************************************************************
 * Forward Declarations
 *************************************************************************/

static void vVppIpFrcMe_InitMeInternalParams(t_StVppIpFrcMeCb *pstCb);
static uint32_t u32VppIpFrcMe_ValidateConfig(struct vpp_port_param stInput,
                                             struct vpp_port_param stOutput);
static uint32_t u32VppIpFrcMe_ValidatePortParam(struct vpp_port_param stParam);
static void *vpVppIpFrcMe_Receiver(void *pv);

/************************************************************************
 * Local Functions
 ************************************************************************/

#if FRC_ME_DUMP_BUF_ENABLE
// Dump YUV_IN or MBI_OUT buffer
static void vVppIpFrcMe_DumpBuffer(t_StVppIpFrcMeCb *pstCb, t_StVppBuf *pstBuf,
                                   t_EVppIpFrcMeDumpBuf eBuf, uint32_t u32Idx)
{
    uint32_t u32Len = 256;
    char cPath[u32Len];
    struct vpp_port_param *pstParam;

    VPP_RET_VOID_IF_NULL(pstCb);
    VPP_RET_VOID_IF_NULL(pstBuf);

    if (pstCb->stBase.bSecureSession)
        return;

    if (((eBuf == FRC_ME_BUF_ME_YUV_IN) &&
         VPP_FLAG_IS_SET(pstBuf->u32InternalFlags, VPP_BUF_FLAG_FLUSHED)) ||
         ((eBuf == FRC_ME_BUF_ME_MBI_OUT) && (pstBuf->stPixel.u32FilledLen == 0)))
        return;

    LOG_ENTER_ARGS("eBuf=%u", eBuf);

    switch (eBuf)
    {
        case FRC_ME_BUF_ME_YUV_IN:
            snprintf(cPath, u32Len, "frc_%p_me_yuv_in_%06d.yuv", pstCb, u32Idx);
            pstParam = &pstCb->stInput.stParam;
            u32VppBuf_Dump(pstCb->stBase.pstCtx, pstBuf, *pstParam, cPath);
            break;
        case FRC_ME_BUF_ME_MBI_OUT:
            snprintf(cPath, u32Len, "frc_%p_me_mbi_out_%06d.mbi", pstCb, u32Idx);
            u32VppBuf_DumpRawData(pstCb->stBase.pstCtx, pstBuf->stPixel.pvBase,
                                  pstBuf->stPixel.u32FilledLen, cPath);
            break;
        default:
            LOGE("Invaid eBuf=%d", eBuf);
            break;
    }
}
#endif

// Log the current ME status flags and counters
static uint32_t u32VppIpFrcMe_LogStatus(t_StVppIpFrcMeCb *pstCb)
{
    uint32_t u32InQSz, u32OutQSz;
    uint32_t u32Ret = VPP_OK;

    VPP_RET_IF_NULL(pstCb, VPP_ERR_PARAM);

    u32InQSz = u32VppBufPool_Cnt(&pstCb->stInput.stPendingQ);
    u32OutQSz = u32VppBufPool_Cnt(&pstCb->stOutput.stPendingQ);

    LOGI("u32InQSz=%u, u32OutQSz=%u, eFrcMeState=%u u32MeFlags=0x%x",
         u32InQSz, u32OutQSz, pstCb->eFrcMeState, pstCb->u32MeFlags);
    LOGI("InBufSentToTME=%u, InBufRxedFromTME=%u, OpBufsentToTME=%u, OpBufRxedFromTME=%u",
         pstCb->u32IpBufsSentToTME, pstCb->u32IpBufsRxedFromTME,
         pstCb->u32OpBufsSentToTME, pstCb->u32OpBufsRxedFromTME);
    return u32Ret;
}

static uint32_t u32VppIpFrcMe_LogBufferInfo(t_StVppBuf *pstBuf)
{
    VPP_RET_IF_NULL(pstBuf, VPP_ERR_PARAM);

    LOGI("Idx=%u, pstBuf=%p, pBuf=%p, Flags=0x%x, BufType=%u, BufPxType=%u\n", pstBuf->u32Idx,
          pstBuf, pstBuf->pBuf, pstBuf->u32InternalFlags, pstBuf->eBufType, pstBuf->eBufPxType);
    LOGI("Px: Base=%p, MappedLen=%u", pstBuf->stPixel.pvBase, pstBuf->stPixel.u32MappedLen);
    if (pstBuf->pBuf != NULL)
    {
        LOGI("Px: fd=%d, offset=0x%x, u32AllocLen=%u, u32FilledLen=%u flags=0x%x, timestamp=0x%llx",
             pstBuf->pBuf->pixel.fd, pstBuf->pBuf->pixel.offset, pstBuf->stPixel.u32AllocLen,
             pstBuf->stPixel.u32FilledLen, pstBuf->pBuf->flags,
             (long long unsigned int)pstBuf->pBuf->timestamp);
    }
    return VPP_OK;
}

static uint32_t u32VppIpFrcMe_MatchBuf(t_StVppBuf *pstBuf, void *pv)
{
    t_StVppIpFrcMeExpBuf *pstExp = (t_StVppIpFrcMeExpBuf *)pv;

    VPP_RET_IF_NULL(pstBuf, VPP_FALSE);
    VPP_RET_IF_NULL(pstExp, VPP_FALSE);

    if (pstBuf->u32Idx == pstExp->u32Index)
    {
        LOGI("found matching buffer, id=%u, pstBuf=%p", pstBuf->u32Idx, pstBuf);
        return VPP_TRUE;
    }

    LOGI("Not a matching buffer, id=%u, bufId=%u", pstBuf->u32Idx, pstExp->u32Index);
    return VPP_FALSE;
}

// Get cmd from the command queue.
// The caller is supposed to lock the mutex which protects the queue
static uint32_t u32VppIpFrcMe_CmdGet(t_StVppIpFrcMeCb *pstCb, t_StVppIpCmd *o_pstCmd)
{
    int32_t idx;
    uint32_t u32Ret = VPP_OK;

    VPP_RET_IF_NULL(pstCb, VPP_ERR_PARAM);
    VPP_RET_IF_NULL(o_pstCmd, VPP_ERR_PARAM);

    idx = vpp_queue_dequeue(&pstCb->stCmdQ);
    if (idx < 0)
    {
        // Queue empty or stCmdQ not valid
        u32Ret = VPP_ERR;
    }
    else
    {
        *o_pstCmd = pstCb->astCmdNode[idx];
        LOG_CMD("GetCmd", o_pstCmd->eCmd);
    }

    return u32Ret;
}

// Put cmd into the command queue.
static uint32_t u32VppIpFrcMe_CmdPut(t_StVppIpFrcMeCb *pstCb, t_StVppIpCmd *pstCmd)
{
    int32_t idx;
    uint32_t u32Ret = VPP_OK;

    VPP_RET_IF_NULL(pstCb, VPP_ERR_PARAM);
    VPP_RET_IF_NULL(pstCmd, VPP_ERR_PARAM);

    FRC_ME_LOCK(pstCb);

    LOG_CMD("InsertCmd in FrcMe Worker", pstCmd->eCmd);
    idx = vpp_queue_enqueue(&pstCb->stCmdQ);
    if (idx < 0)
    {
        // Queue full or stCmdQ not valid
        u32Ret = VPP_ERR;
    }
    else
    {
        pstCb->astCmdNode[idx] = *pstCmd;
        FRC_ME_SIGNAL(pstCb);
    }
    FRC_ME_UNLOCK(pstCb);

    return u32Ret;
}

// Flush Port's queue
// All buffers found in the PORT's queue are released and the flush flag is set.
static uint32_t u32VppIpFrcMe_ReleasePortBuf(t_StVppIpFrcMeCb *pstCb, enum vpp_port ePort)
{
    t_StVppBuf *pBuf;
    t_StVppIpPort *pstPort;
    uint32_t u32Ret = VPP_OK;

    VPP_RET_IF_NULL(pstCb, VPP_ERR_PARAM);

    if (ePort == VPP_PORT_INPUT)
        pstPort = &pstCb->stInput;
    else
        pstPort = &pstCb->stOutput;

    while (u32VppIp_PortBufGet(pstPort, &pBuf, &pstCb->mutex) == VPP_OK)
    {
        VPP_FLAG_SET(pBuf->u32InternalFlags, VPP_BUF_FLAG_FLUSHED);
        if (ePort == VPP_PORT_OUTPUT)
            pBuf->stPixel.u32FilledLen = 0;

        vVppIpCbLog(&pstCb->stBase.stCb, pBuf, eVppLogId_IpBufDone);
        u32VppIp_CbBufDone(&pstCb->stBase.stCb, ePort, pBuf);
    }

    return VPP_OK;
}

// Alloc internal EOS input buffer
static uint32_t u32VppIpFrcMe_AllocInternalEosBuf(t_StVppIpFrcMeCb *pstCb)
{
    uint32_t u32Sz, u32Ret = VPP_OK;

    VPP_RET_IF_NULL(pstCb, VPP_ERR_PARAM);

    pstCb->stBufEos.pBuf = &(pstCb->stVppBufEos);
    u32Sz = u32VppUtils_GetBufferSize(&pstCb->stInput.stParam);

    u32Ret = u32VppIon_Alloc(pstCb->stBase.pstCtx, u32Sz, pstCb->stBase.bSecureSession,
                             &pstCb->stIonBuf);
    if (u32Ret || (pstCb->stIonBuf.buf == NULL))
    {
        LOGE("u32VppIon_Alloc(Internal-EOS) failed: pstCtx=%p, secure=%u, size=%u, ret=%u",
              pstCb->stBase.pstCtx, pstCb->stBase.bSecureSession, u32Sz, u32Ret);
        LOG_EXITI_RET(VPP_ERR_RESOURCES);
    }
    pstCb->stBufEos.stPixel.fd = pstCb->stIonBuf.fd_ion_mem;
    pstCb->stBufEos.stPixel.u32Offset = 0;
    pstCb->stBufEos.stPixel.u32AllocLen = u32Sz;
    pstCb->stBufEos.stPixel.u32FilledLen = u32Sz;
    pstCb->stBufEos.stPixel.u32ValidLen = u32Sz;
    pstCb->stBufEos.pBuf->flags = VPP_BUFFER_FLAG_EOS;
    if (!pstCb->stBase.bSecureSession)
    {
        pstCb->stBufEos.stPixel.u32MappedLen = u32Sz;
        pstCb->stBufEos.stPixel.pvBase = pstCb->stIonBuf.buf;
    }
    else
    {
        pstCb->stBufEos.stPixel.u32MappedLen = 0;
        pstCb->stBufEos.stPixel.pvBase = VPP_BUF_UNMAPPED_BUF_VAL;
    }

    pstCb->stBufEos.u32InternalFlags = FRC_ME_BUF_INTERN_EOS;
    pstCb->stBufEos.eBufType = eVppBufType_Progressive;
    pstCb->stBufEos.eBufPxType = eVppBufPxDataType_Raw;
    LOGI("Allocated Internal-EOS buffer size:%u fd:%d", u32Sz, pstCb->stBufEos.stPixel.fd);

    return u32Ret;
}

// Free internal EOS Drain input buffer
static uint32_t u32VppIpFrcMe_FreeInternalEosBuf(t_StVppIpFrcMeCb *pstCb)
{
    uint32_t u32Ret = VPP_OK;

    u32Ret = u32VppIon_Free(pstCb->stBase.pstCtx, &pstCb->stIonBuf);
    LOGE_IF(u32Ret, "u32VppIon_Free Failed, ret=%u", u32Ret);

    LOGI("Freed Internal-EOS buffer");
    return u32Ret;
}

// Start pending FlushDrain process
static void vVppIpFrcMe_HandleFlushForPendingEOS(t_StVppIpFrcMeCb *pstCb)
{
    t_StVppEvt stEvt;
    uint32_t u32Ret = VPP_OK;

    if (VPP_FLAG_IS_SET(pstCb->u32MeFlags, FRC_ME_INPUT_EOS_RECEIVED) &&
        VPP_FLAG_IS_SET(pstCb->u32MeFlags, FRC_ME_OUTPUT_EOS_RECEIVED))
    {
        VPP_FLAG_CLR(pstCb->u32MeFlags, FRC_ME_INPUT_EOS_RECEIVED);
        VPP_FLAG_CLR(pstCb->u32MeFlags, FRC_ME_OUTPUT_EOS_RECEIVED);

        // Flush due to Drain command.
        if (VPP_FLAG_IS_SET(pstCb->u32MeFlags, IP_DRAIN_PENDING) &&
            !VPP_FLAG_IS_SET(pstCb->u32MeFlags, FRC_ME_FLUSH_DRAIN_STARTED))
        {
            VPP_FLAG_SET(pstCb->u32MeFlags, FRC_ME_FLUSH_DRAIN_STARTED);
            LOGI("Flushing due to Drain Internal-EOS");
        }
        else if (FRC_ME_IS_STATE(pstCb, FRC_ME_STATE_BYPASS))
        {
            LOGI("Flushing due to Bypass Internal-EOS");
        }
        else
            LOGI("Flushing due to External EOS");

        LOGI("Issuing TME Flush for EOS_PENDING");
        VPP_FLAG_SET(pstCb->u32MeFlags, IP_FLUSH_PENDING);
        u32Ret = u32VppIpFrcMe_TmeFlush(pstCb);
        LOGE_IF(u32Ret, "Flush TME Failed, ret=%u", u32Ret);
    }
}

// End pending FlushDrain process, this is called upon receiving FLUSH_DONE event from TME
static void vVppIpFrcMe_HandleFlushDoneEvent(t_StVppIpFrcMeCb *pstCb)
{
    t_StVppEvt stEvt;
    uint32_t bSendDrainDoneEvt = VPP_FALSE;

    VPP_RET_VOID_IF_NULL(pstCb);

    FRC_ME_LOCK(pstCb);
    if (VPP_FLAG_IS_SET(pstCb->u32MeFlags, IP_FLUSH_PENDING))
    {
        // Clear the flags
        VPP_FLAG_CLR(pstCb->u32MeFlags, IP_FLUSH_PENDING);

        if (VPP_FLAG_IS_SET(pstCb->u32MeFlags, FRC_ME_FLUSH_DRAIN_STARTED))
        {
            // Flush done due to drain command
            VPP_FLAG_CLR(pstCb->u32MeFlags, IP_DRAIN_PENDING);
            VPP_FLAG_CLR(pstCb->u32MeFlags, FRC_ME_FLUSH_DRAIN_STARTED);
            // Send DRAIN done event to the client
            bSendDrainDoneEvt = VPP_TRUE;
            LOGI("Flushing done to Drain Internal-EOS");
        }
        else if (FRC_ME_IS_STATE(pstCb, FRC_ME_STATE_BYPASS) &&
                 VPP_FLAG_IS_SET(pstCb->u32MeFlags, IP_EOS_PENDING))
        {
            LOGI("Flushing done to Bypass Internal-EOS");
        }

        // External-EOS, Drain-Internal-EOS, Bypass-Internal-EOS
        if (VPP_FLAG_IS_SET(pstCb->u32MeFlags, IP_EOS_PENDING))
        {
            VPP_FLAG_CLR(pstCb->u32MeFlags, IP_EOS_PENDING);
            LOGI("IP_EOS_PENDING - Cleared");
        }

        // ME flushes both input and output buffers from TME, move to IDLE state and let the worker
        // decide the next state based on the next input buffer in the queue
        FRC_ME_STATE_SET(pstCb, FRC_ME_STATE_IDLE);
    }
    else
        LOGE("IP_FLUSH_PENDING is NOT set: received FLUSH-DONE event");

    // Signal Flush-done
    FRC_ME_UNLOCK(pstCb);

    if (bSendDrainDoneEvt == VPP_TRUE)
    {
        stEvt.eType = VPP_EVT_DRAIN_DONE;
        u32VppIpCbEvent(&pstCb->stBase.stCb, stEvt);
        LOGI("%s ME Drain Done event sent", __func__);
    }
    FRC_ME_SIGNAL(pstCb);
}

// Sets Worker thread started flag
// Set a condition in order to wake up the waiting thread.
static void vVppIpFrcMe_SignalWorkerStart(t_StVppIpFrcMeCb *pstCb)
{
    VPP_RET_VOID_IF_NULL(pstCb);

    FRC_ME_LOCK(pstCb);
    VPP_FLAG_SET(pstCb->u32MeFlags, IP_WORKER_STARTED);
    FRC_ME_SIGNAL(pstCb);
    FRC_ME_UNLOCK(pstCb);
}

static void vVppIpFrcMe_WaitWorkerStart(t_StVppIpFrcMeCb *pstCb)
{
    VPP_RET_VOID_IF_NULL(pstCb);

    FRC_ME_LOCK(pstCb);
    while (!VPP_FLAG_IS_SET(pstCb->u32MeFlags, IP_WORKER_STARTED))
        FRC_ME_WAIT(pstCb);
    FRC_ME_UNLOCK(pstCb);
}

static void vVppIpFrcMe_SignalReceiverStart(t_StVppIpFrcMeCb *pstCb)
{
    VPP_RET_VOID_IF_NULL(pstCb);

    FRC_ME_LOCK(pstCb);
    VPP_FLAG_SET(pstCb->u32MeFlags, FRC_ME_RECEIVER_THREAD_STARTED);
    pthread_cond_signal(&pstCb->rxCond);
    FRC_ME_UNLOCK(pstCb);
}

static void vVppIpFrcMe_WaitReceiverStart(t_StVppIpFrcMeCb *pstCb)
{
    VPP_RET_VOID_IF_NULL(pstCb);

    FRC_ME_LOCK(pstCb);
    while (!VPP_FLAG_IS_SET(pstCb->u32MeFlags, FRC_ME_RECEIVER_THREAD_STARTED))
        pthread_cond_wait(&pstCb->rxCond, &pstCb->mutex);
    FRC_ME_UNLOCK(pstCb);
}

static uint32_t u32VppIpFrcMe_ProcCmdClose(t_StVppIpFrcMeCb *pstCb)
{
    LOG_ENTER();

    int rc;
    uint32_t u32, u32Ret = VPP_OK;

    FRC_ME_LOCK(pstCb);

    // We don't need to issue FLUSH to TME if it doesn't have any buffers(input and output)
    if (FRC_ME_TME_INPQ_INPROC(pstCb) || FRC_ME_TME_OUTQ_INPROC(pstCb))
    {
        // Flush ME
        VPP_FLAG_SET(pstCb->u32MeFlags, IP_FLUSH_PENDING);

        u32Ret = u32VppIpFrcMe_TmeFlush(pstCb);
        if (u32Ret == VPP_OK)
        {
            while (VPP_FLAG_IS_SET(pstCb->u32MeFlags, IP_FLUSH_PENDING))
                FRC_ME_WAIT(pstCb);
        }
        else
        {
            VPP_FLAG_CLR(pstCb->u32MeFlags, IP_FLUSH_PENDING);
            LOGE("u32VppIpFrcMe_TmeFlush Failed, ret=%u", u32Ret);
        }
    }
    // The receiver thread can exit now
    pstCb->u32MeExitReceiver = VPP_TRUE;
    FRC_ME_UNLOCK(pstCb);

    // Flush buffer from the internal ME queue
    u32 = u32VppIpFrcMe_ReleasePortBuf(pstCb, VPP_PORT_INPUT);
    LOGE_IF(u32, "u32VppIpFrcMe_FlushPort(VPP_PORT_INPUT) failed ret=%d", u32);
    u32 = u32VppIpFrcMe_ReleasePortBuf(pstCb, VPP_PORT_OUTPUT);
    LOGE_IF(u32, "u32VppIpFrcMe_FlushPort(VPP_PORT_OUTPUT) failed ret=%d", u32);

    u32 = u32VppIpFrcMe_TmeReleaseResources(pstCb);
    LOGE_IF(u32Ret != VPP_OK, "TmeReleaseResources() failed, u32Ret=%u", u32);

    // Stop ME
    if (!eventfd_write(pstCb->MePollNotification, 1))
    {
        if (pstCb->rxThread != 0)
        {
            rc = pthread_join(pstCb->rxThread, NULL);
            LOGE_IF(rc, "pthread_join failed %d-->%s", rc, strerror(rc));
        }
    }
    pstCb->rxThread = 0;

    rc = pthread_cond_destroy(&pstCb->rxCond);
    LOGE_IF(rc, "pthread_cond_destroy failed: %d-->%s", rc, strerror(rc));

    u32Ret = u32VppIpFrcMe_TmeCloseDev(pstCb);
    LOGE_IF(u32Ret != VPP_OK, "TmeCloseDev() failed, u32Ret=%u", u32Ret);

    u32 = u32VppIpFrcMe_FreeInternalEosBuf(pstCb);
    LOGE_IF(u32Ret != VPP_OK, "u32VppIpFrcMe_FreeInternalEosBuf() failed, u32Ret=%u", u32);

    VPP_IP_STATE_SET(pstCb, VPP_IP_STATE_INITED);

    pstCb->asyncRes.u32MeCloseRet = u32Ret;

    LOGI("%s() posting semaphore", __func__);
    sem_post(&pstCb->sem);

    LOG_EXIT_RET(u32Ret);
}

// ME is flushed (FLUSH cmd received).
// During Flush no other cmd or buffer is accepted.
static uint32_t u32VppIpFrcMe_ProcCmdFlush(t_StVppIpFrcMeCb *pstCb, t_StVppIpCmd *pstCmd)
{
    t_StVppEvt stEvt;
    uint32_t u32, u32FlushNeeded = VPP_FALSE, u32Ret = VPP_OK;

    VPP_RET_IF_NULL(pstCb, VPP_ERR_PARAM);
    VPP_RET_IF_NULL(pstCmd, VPP_ERR_PARAM);
    if (pstCmd->flush.ePort >= VPP_PORT_MAX)
        return VPP_ERR_PARAM;

    FRC_ME_LOCK(pstCb);
    //Flush ME
    while (VPP_FLAG_IS_SET(pstCb->u32MeFlags, IP_FLUSH_PENDING) ||
           VPP_FLAG_IS_SET(pstCb->u32MeFlags, IP_DRAIN_PENDING))
    {
        FRC_ME_WAIT(pstCb);
    }

    VPP_FLAG_SET(pstCb->u32MeFlags, IP_FLUSH_PENDING);
    u32FlushNeeded = ((pstCmd->flush.ePort == VPP_PORT_INPUT) ? FRC_ME_TME_INPQ_INPROC(pstCb)
                                                              : FRC_ME_TME_OUTQ_INPROC(pstCb));
    if (u32FlushNeeded)
    {
        u32Ret = u32VppIpFrcMe_TmeFlush(pstCb);
        if (u32Ret != VPP_OK)
        {
            VPP_FLAG_CLR(pstCb->u32MeFlags, IP_FLUSH_PENDING);
            LOGE("Flush TME Failed, port=%d, ret=%d", pstCmd->flush.ePort, u32Ret);
        }
        else
        {
            LOGI("External FLUSH command received, sent to TME.. waiting");
            while (VPP_FLAG_IS_SET(pstCb->u32MeFlags, IP_FLUSH_PENDING))
            {
                FRC_ME_WAIT(pstCb);
            }
            LOGI("External FLUSH command : TME flush done received");
        }
    }
    FRC_ME_UNLOCK(pstCb);

    // Flush buffer from the internal ME queue
    u32Ret = u32VppIpFrcMe_ReleasePortBuf(pstCb, pstCmd->flush.ePort);
    if (u32Ret != VPP_OK)
         LOGE("u32VppIpFrcMe_FlushPort Failed, port=%d, ret=%d", pstCmd->flush.ePort, u32Ret);
    else
    {
        stEvt.eType = VPP_EVT_FLUSH_DONE;
        stEvt.flush.ePort = pstCmd->flush.ePort;
        u32VppIpCbEvent(&pstCb->stBase.stCb, stEvt);
    }

    VPP_FLAG_CLR(pstCb->u32MeFlags, IP_FLUSH_PENDING);

    return u32Ret;
}

// Internal Drain EOS input buffer is injected into the input queue. The Drain pending flag is set.
// No other cmd or input buffer is accepted at the input during Drain.
static uint32_t u32VppIpFrcMe_ProcCmdDrain(t_StVppIpFrcMeCb *pstCb)
{
    t_StVppEvt stEvt;
    uint32_t u32Ret = VPP_OK;

    VPP_RET_IF_NULL(pstCb, VPP_ERR_PARAM);

    FRC_ME_LOCK(pstCb);
    VPP_FLAG_SET(pstCb->u32MeFlags, IP_DRAIN_PENDING);
    pstCb->stBufEos.pBuf->timestamp = pstCb->MeLastTimestamp + (FRC_ME_DEFAULT_DELAY / 2);
    FRC_ME_UNLOCK(pstCb);

    u32Ret = u32VppIp_PortBufPut(&pstCb->stInput, &pstCb->stBufEos, &pstCb->mutex, &pstCb->cond);
    LOGE_IF(u32Ret != VPP_OK, "u32VppIpFrcMe_PortBufPut() Failed ret=%u", u32Ret);

    return u32Ret;
}

static uint32_t u32VppIpFrcMe_ValidatePortParam(struct vpp_port_param stParam)
{
    if ((stParam.width > FRC_ME_MAX_W) || (stParam.width < FRC_ME_MIN_W))
    {
        LOGE("Failed: width=%u, min=%u, max=%u", stParam.width, FRC_ME_MIN_W, FRC_ME_MAX_W);
        return VPP_ERR;
    }
    if ((stParam.height > FRC_ME_MAX_H) || (stParam.height < FRC_ME_MIN_H))
    {
        LOGE("Failed: height=%u, min=%u, max=%u", stParam.height, FRC_ME_MIN_H, FRC_ME_MAX_H);
        return VPP_ERR;
    }
    if (stParam.fmt != VPP_COLOR_FORMAT_UBWC_NV12)
    {
        LOGE("Failed: unsupported fmt=%u", stParam.fmt);
        return VPP_ERR;
    }

    return VPP_OK;
}

// Checks for the supported resolutions and the video format.
static uint32_t u32VppIpFrcMe_ValidateConfig(struct vpp_port_param stInput,
                                             struct vpp_port_param stOutput)
{
    uint32_t u32Ret = VPP_OK;

    if ((stInput.height != stOutput.height) || (stInput.width != stOutput.width))
    {
        LOGE("Height ip:%u, op :%u", stInput.height, stOutput.height);
        LOGE("Width  ip:%u, op :%u", stInput.width, stOutput.width);
        u32Ret = VPP_ERR;
    }

    if (stInput.fmt != stOutput.fmt)
    {
        LOGE("Format ip:%u, op :%u", stInput.fmt, stOutput.fmt);
        u32Ret = VPP_ERR;
    }

    return u32Ret;
}

// Returns the first available free index. The caller must lock the mutex.
// The valid index needs to be one within the TME Q size ( 0 to size-1) - V4L2 driver requirement
static uint32_t u32VppIpFrcMe_GetFreeIndex(t_StVppIpFrcMeCb *pstCb, enum vpp_port ePort)
{
    uint32_t u32IdxMap, u32Idx, u32MaxCnt;

    if (ePort == VPP_PORT_OUTPUT)
    {
        u32MaxCnt = pstCb->u32OpBufsTmeQSize;
        u32IdxMap = pstCb->u32TmeOpBufIdxMap;
    }
    else
    {
        u32MaxCnt = pstCb->u32IpBufsTmeQSize;
        u32IdxMap = pstCb->u32TmeIpBufIdxMap;
    }

    // Get the bit field, which is not set to 1
    // If a bitfield is set to 1,
    for (u32Idx = 0; u32Idx < u32MaxCnt; u32Idx++)
        if (!VPP_FLAG_IS_SET(u32IdxMap, (1 << u32Idx)))
            return u32Idx;

    return FRC_ME_ERROR_INDEX;
}

static void vVppIpFrcMe_UpdateStateMachine(t_StVppIpFrcMeCb *pstCb)
{
    // Update the state machine based on the next buffer found
    if (u32VppBufPool_Cnt(&pstCb->stInput.stPendingQ))
    {
        t_StVppBuf *pstVppBuf = pstVppIp_PortBufPeek(&pstCb->stInput, 0, NULL);
        if (pstVppBuf != NULL)
        {
            // if the BYPASS flag (internal/buf) is set then need to bypass this buffer
            if (FRC_ME_IS_BUF_BYPASS(pstVppBuf))
            {
                LOGI("Found bypass input buffer in the queue");
                if (!FRC_ME_IS_STATE(pstCb, FRC_ME_STATE_BYPASS))
                    FRC_ME_STATE_SET(pstCb, FRC_ME_STATE_BYPASS);
            }
            else if (!FRC_ME_IS_STATE(pstCb, FRC_ME_STATE_FEED) &&
                     (!VPP_FLAG_IS_SET(pstCb->u32MeFlags, IP_DRAIN_PENDING) ||
                      VPP_FLAG_IS_SET(pstVppBuf->u32InternalFlags, FRC_ME_BUF_INTERN_EOS)))
            {
                FRC_ME_STATE_SET(pstCb, FRC_ME_STATE_FEED);
            }
        }
    }
}

// Checks if the available buffers are enough for processing in the current stage.
static uint32_t u32VppIpFrcMe_ProcBufReqMet(t_StVppIpFrcMeCb *pstCb)
{
    uint32_t u32Ret = VPP_FALSE;
    uint32_t u32MeProcInQCnt = u32VppBufPool_Cnt(&pstCb->stInput.stPendingQ);
    uint32_t u32MeProcOutQCnt = u32VppBufPool_Cnt(&pstCb->stOutput.stPendingQ);

    if (FRC_ME_IS_STATE(pstCb, FRC_ME_STATE_BYPASS))
    {
        // If EOS is pending then we can queue only the output buffers(In Bypass state)
        if (VPP_FLAG_IS_SET(pstCb->u32MeFlags, IP_EOS_PENDING))
        {
            if (u32MeProcOutQCnt && FRC_ME_TME_OUTQ_AVAIL(pstCb))
                u32Ret = VPP_TRUE;
        }
        else if (u32MeProcInQCnt)
        {
            // Can process(bypass) Input buf
            // This will either send an Internal-EOS or Bypass the Input buffer
            u32Ret = VPP_TRUE;
        }
    }
    else if (FRC_ME_IS_STATE(pstCb, FRC_ME_STATE_FEED))
    {
        // If there is a pending EOS then only output buffers need to be queued
        if (VPP_FLAG_IS_SET(pstCb->u32MeFlags, IP_EOS_PENDING))
        {
            if (u32MeProcOutQCnt && FRC_ME_TME_OUTQ_AVAIL(pstCb))
                u32Ret = VPP_TRUE;
        }
        else if ((u32MeProcInQCnt && FRC_ME_TME_INPQ_AVAIL(pstCb)) ||
                 (u32MeProcOutQCnt && FRC_ME_TME_OUTQ_AVAIL(pstCb)))
        {
            // Can process/queue Input buffer OR Ouput buffer
            u32Ret = VPP_TRUE;
        }
    }

    // Debug info on why this returns false
    if (u32Ret == VPP_FALSE)
        u32VppIpFrcMe_LogStatus(pstCb);

    return u32Ret;
}

// A predicate function used for determining if the worker thread should sleep or not.
static uint32_t u32VppIpFrcMe_WorkerThreadShouldSleep(t_StVppIpFrcMeCb *pstCb)
{
    uint32_t u32ProcMet = VPP_FALSE;
    uint32_t u32Ret = VPP_TRUE;
    uint32_t u32CmdQSz;

    VPP_RET_IF_NULL(pstCb, VPP_TRUE);

    u32CmdQSz = vpp_queue_count(&pstCb->stCmdQ);

    if (VPP_IP_STATE_EQUAL(pstCb, VPP_IP_STATE_ACTIVE))
    {
        // Flush the TME if the EOS buffers(inp-EOS and outEOS) are received.
        // Update the stateMachine only if there is no EOS Pending
        if (VPP_FLAG_IS_SET(pstCb->u32MeFlags, IP_EOS_PENDING))
            vVppIpFrcMe_HandleFlushForPendingEOS(pstCb);
        else
            vVppIpFrcMe_UpdateStateMachine(pstCb);

        u32ProcMet = u32VppIpFrcMe_ProcBufReqMet(pstCb);
    }

    // If ME can process either the Command or the Buffers, return FALSE
    if (!VPP_FLAG_IS_SET(pstCb->u32MeFlags, IP_FLUSH_PENDING))
    {
        if (u32CmdQSz && !VPP_FLAG_IS_SET(pstCb->u32MeFlags, IP_DRAIN_PENDING))
            u32Ret = VPP_FALSE;
        else if (u32ProcMet)
            u32Ret = VPP_FALSE;
    }

    LOGI("CHECK: shouldSleep=%u, u32CmdQSz=%u, u32ProcMet=%u", u32Ret, u32CmdQSz, u32ProcMet);

    return u32Ret;
}

static uint32_t u32VppIpFrcMe_ReturnBufToClient(t_StVppIpFrcMeCb *pstCb,
                                                enum vpp_port ePort,
                                                t_StVppBuf *pstBuf)
{
    uint32_t u32Ret;

    if (ePort == VPP_PORT_INPUT)
        pstCb->u32IpBufsSentToCl++;
    else
        pstCb->u32OpBufsSentToCl++;

    vVppIpCbLog(&pstCb->stBase.stCb, pstBuf, eVppLogId_IpBufDone);
    u32Ret = u32VppIp_CbBufDone(&pstCb->stBase.stCb, ePort, pstBuf);

    return u32Ret;
}

// Read Events from the Driver
static void vVppIpFrcMe_HandleReturnedEvents(t_StVppIpFrcMeCb *pstCb)
{
    uint32_t u32Ret;
    struct v4l2_event stEvent;

    u32Ret = u32VppIpFrcMe_TmeGetEvents(pstCb, &stEvent);
    if (u32Ret != VPP_OK)
    {
        LOGE("u32VppIpFrcMe_TmeGetEvents() failed");
        return;
    }

    u32VppIpFrcMe_LogStatus(pstCb);
    if (stEvent.type == V4L2_EVENT_MSM_VIDC_FLUSH_DONE)
    {
        LOGI("Queue V4L2_EVENT_MSM_VIDC_FLUSH_DONE");
        vVppIpFrcMe_HandleFlushDoneEvent(pstCb);
    }
    else
    {
        t_StVppEvt stEvt;

        if (stEvent.type == V4L2_EVENT_MSM_VIDC_HW_OVERLOAD)
        {
            stEvt.eType = VPP_EVT_ERR_HW_OVERLOAD;
            LOGE("V4L2_EVENT_MSM_VIDC_HW_OVERLOAD Event");
        }
        else if (stEvent.type == V4L2_EVENT_MSM_VIDC_SYS_ERROR)
        {
            stEvt.eType = VPP_EVT_ERR_SYSTEM;
            LOGE("V4L2_EVENT_MSM_VIDC_SYS_ERROR Event");
        }
        else if (stEvent.type == V4L2_EVENT_MSM_VIDC_MAX_CLIENTS)
        {
            stEvt.eType = VPP_EVT_ERR_MAX_CLIENTS;
            LOGE("V4L2_EVENT_MSM_VIDC_MAX_CLIENTS Event");
        }
        else if (stEvent.type == V4L2_EVENT_MSM_VIDC_HW_UNSUPPORTED)
        {
            stEvt.eType = VPP_EVT_ERR_HW_UNSUPPORTED;
            LOGE("V4L2_EVENT_MSM_VIDC_HW_UNSUPPORTED Event");
        }
        else
        {
            stEvt.eType = VPP_EVT_MAX;
            LOGE("UNKNOWN Event %u", stEvent.type);
        }

        // Notify the event
        u32VppIpCbEvent(&pstCb->stBase.stCb, stEvt);
    }
}

static void vVppIpFrcMe_HandleReturnedInputBuf(t_StVppIpFrcMeCb *pstCb)
{
    struct v4l2_buffer stBuf;
    struct v4l2_plane asPlane[VIDEO_MAX_PLANES];
    t_StVppBuf *pstVppBuf;
    uint32_t u32;

    memset(&asPlane, 0, sizeof(asPlane));
    memset(&stBuf, 0, sizeof(stBuf));

    stBuf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE;
    stBuf.memory = V4L2_MEMORY_USERPTR;
    stBuf.m.planes = asPlane;
    stBuf.length = FRC_ME_NUM_PLANES;

    while (!u32VppIpFrcMe_TmeGetBuf(pstCb, &stBuf))
    {
        t_StVppIpFrcMeExpBuf stExp = {
            .u32Index = stBuf.index,
        };

        FRC_ME_LOCK(pstCb);
        u32 = u32VppIp_PortBufGetMatching(&pstCb->stTmeIn, &pstVppBuf, NULL,
                                          u32VppIpFrcMe_MatchBuf, &stExp);
        if ((u32 != VPP_OK) || (pstVppBuf == NULL))
        {
            LOGE("unable to pull buffer from port, buffer lost, u32=%u", u32);
            FRC_ME_UNLOCK(pstCb);
            continue;
        }

        VPP_FLAG_CLR(pstCb->u32TmeIpBufIdxMap, (1 << stExp.u32Index));
        // Update status counters
        pstCb->u32IpBufsRxedFromTME++;
        FRC_ME_LOG_TME_BUF_STATUS(pstCb);

        u32VppIpFrcMe_LogBufferInfo(pstVppBuf);
        LOGI("%s DeQueue Input buffer %d %d", __func__, stBuf.index, pstVppBuf->stPixel.fd);

        if (VPP_FLAG_IS_SET(pstVppBuf->u32InternalFlags, FRC_ME_BUF_INTERN_EOS))
        {
            VPP_FLAG_SET(pstCb->u32MeFlags, FRC_ME_INPUT_EOS_RECEIVED);
            LOGI("Internal EOS input buffer released from TME!");
            FRC_ME_UNLOCK(pstCb);
        }
        else
        {
            if (VPP_FLAG_IS_SET(pstVppBuf->pBuf->flags, VPP_BUFFER_FLAG_EOS))
            {
                VPP_FLAG_SET(pstCb->u32MeFlags, FRC_ME_INPUT_EOS_RECEIVED);
                LOGI("External EOS input buffer released from TME!");
            }

            if (VPP_FLAG_IS_SET(pstCb->u32MeFlags, IP_FLUSH_PENDING))
                VPP_FLAG_SET(pstVppBuf->u32InternalFlags, VPP_BUF_FLAG_FLUSHED);

            FRC_ME_UNLOCK(pstCb);
#if FRC_ME_DUMP_BUF_ENABLE
            vVppIpFrcMe_DumpBuffer(pstCb, pstVppBuf, FRC_ME_BUF_ME_YUV_IN,
                                   pstCb->u32IpBufsRxedFromTME);
#endif
            // Return the Input buffer to the client
            u32VppIpFrcMe_ReturnBufToClient(pstCb, VPP_PORT_INPUT, pstVppBuf);
        }
        FRC_ME_SIGNAL(pstCb);
    }
}

static void vVppIpFrcMe_HandleReturnedOutputBuf(t_StVppIpFrcMeCb *pstCb)
{
    struct v4l2_buffer stBuf;
    struct v4l2_plane plane[VIDEO_MAX_PLANES];
    t_StVppBuf *pstVppBuf;
    uint32_t u32;

    memset(&plane, 0, sizeof(plane));
    memset(&stBuf, 0, sizeof(stBuf));

    stBuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    stBuf.memory = V4L2_MEMORY_USERPTR;
    stBuf.m.planes = plane;
    stBuf.length = FRC_ME_NUM_PLANES;

    while (!u32VppIpFrcMe_TmeGetBuf(pstCb, &stBuf))
    {
        t_StVppIpFrcMeExpBuf stExp = {
            .u32Index = stBuf.index,
        };

        FRC_ME_LOCK(pstCb);
        u32 = u32VppIp_PortBufGetMatching(&pstCb->stTmeOut, &pstVppBuf, NULL,
                                          u32VppIpFrcMe_MatchBuf, &stExp);
        if ((u32 != VPP_OK) || (pstVppBuf == NULL))
        {
            LOGE("unable to pull buffer from port, buffer lost, u32=%u", u32);
            FRC_ME_UNLOCK(pstCb);
            continue;
        }

        VPP_FLAG_CLR(pstCb->u32TmeOpBufIdxMap, (1 << stExp.u32Index));

        pstCb->u32OpBufsRxedFromTME++;
        FRC_ME_LOG_TME_BUF_STATUS(pstCb);

        // Copy info from v4l2 buffer
        pstVppBuf->eBufPxType = eVppBufPxDataType_Compressed;
        pstVppBuf->stPixel.u32FilledLen = stBuf.m.planes[0].bytesused;
        pstVppBuf->stPixel.u32Offset = stBuf.m.planes[0].data_offset;

        if (VPP_FLAG_IS_SET(stBuf.flags, V4L2_QCOM_BUF_FLAG_EOS))
        {
            VPP_FLAG_SET(pstVppBuf->pBuf->flags, VPP_BUFFER_FLAG_EOS);
            VPP_FLAG_SET(pstCb->u32MeFlags, FRC_ME_OUTPUT_EOS_RECEIVED);
            LOGI("Output EOS detected! Flags=0x%x", pstVppBuf->pBuf->flags);

            // If this EOS is due to Internal-EOS, discard this buffer
            if (VPP_FLAG_IS_SET(pstCb->u32MeFlags, FRC_ME_INTERN_INPUT_EOS_QUEUED))
            {
                pstVppBuf->stPixel.u32FilledLen = 0;
                VPP_FLAG_CLR(pstCb->u32MeFlags, FRC_ME_INTERN_INPUT_EOS_QUEUED);
                LOGI("Output EOS due to Internal-EOS: setting FilledLen = 0");
            }
        }
        pstVppBuf->pBuf->timestamp = FRC_ME_GET_TIMESTAMP(stBuf.timestamp.tv_sec,
                                                          stBuf.timestamp.tv_usec);

        u32VppIpFrcMe_LogBufferInfo(pstVppBuf);
        LOGI("%s Dequeue Output buffer idx:%d bytesused:%d TS:%lld", __func__, stBuf.index,
             stBuf.m.planes[0].bytesused, (long long unsigned int)pstVppBuf->pBuf->timestamp);

        // Do not return the buffer to the client, if we are in flush due to Drain command
        if (VPP_FLAG_IS_SET(pstCb->u32MeFlags, IP_FLUSH_PENDING) &&
            VPP_FLAG_IS_SET(pstCb->u32MeFlags, IP_DRAIN_PENDING))
        {
            // The buffer goes back to the OutputQ, will be returned back to the client if Flush
            // command comes in. Or the size is less than required (after the reconfig)
            u32VppIp_PortBufPut(&pstCb->stOutput, pstVppBuf, NULL, NULL);
            FRC_ME_UNLOCK(pstCb);
        }
        else
        {
            // Set the Flushed flag if the buffer is returned due to Flush command
            if (VPP_FLAG_IS_SET(pstCb->u32MeFlags, IP_FLUSH_PENDING))
                VPP_FLAG_SET(pstVppBuf->u32InternalFlags, VPP_BUF_FLAG_FLUSHED);

            FRC_ME_UNLOCK(pstCb);
#if FRC_ME_DUMP_BUF_ENABLE
            vVppIpFrcMe_DumpBuffer(pstCb, pstVppBuf, FRC_ME_BUF_ME_MBI_OUT,
                                   pstCb->u32OpBufsRxedFromTME);
#endif
            u32VppIpFrcMe_ReturnBufToClient(pstCb, VPP_PORT_OUTPUT, pstVppBuf);
        }
        FRC_ME_SIGNAL(pstCb);
    }
}

static uint32_t u32VppIpFrcMe_QueueInputBuf(t_StVppIpFrcMeCb *pstCb,
                                            t_StVppBuf *pstInpBuf)
{
    uint32_t u32BufReqSz, u32, u32Ret = VPP_OK;

    FRC_ME_LOCK(pstCb);

    // If there no input buffers with TME, there is no need to queue the EOS buffer.
    // Return the buffer back to the client and update the flags based on the current state
    if (!FRC_ME_TME_INPQ_INPROC(pstCb) &&
        VPP_FLAG_IS_SET(pstInpBuf->pBuf->flags, VPP_BUFFER_FLAG_EOS))
    {
        FRC_ME_UNLOCK(pstCb);
        LOGI("u32VppIpFrcMe_QueueInputBuf: Bypass EOS as TME has zero i/p buffers");
        // If internal-EOS due to DRAIN command, it safe to send the drain-done command
        if (VPP_FLAG_IS_SET(pstInpBuf->u32InternalFlags, FRC_ME_BUF_INTERN_EOS))
        {
            if (VPP_FLAG_IS_SET(pstCb->u32MeFlags, IP_DRAIN_PENDING))
            {
                // Send DRAIN done event to the client
                t_StVppEvt stEvt;
                stEvt.eType = VPP_EVT_DRAIN_DONE;
                u32VppIpCbEvent(&pstCb->stBase.stCb, stEvt);
                LOGI("%s ME Drain Done event sent", __func__);
                VPP_FLAG_CLR(pstCb->u32MeFlags, IP_DRAIN_PENDING);
            }
        }
        else
        {
            // This is the external-EOS buffer, return it to the client
            u32VppIpFrcMe_ReturnBufToClient(pstCb, VPP_PORT_INPUT, pstInpBuf);
        }
        return VPP_OK;
    }

    // Queue input buffer to TME if the size is valid, else return it back to the client
    u32BufReqSz = u32VppUtils_GetPxBufferSize(&pstCb->stInput.stParam);
    if (pstInpBuf->stPixel.u32ValidLen < u32BufReqSz)
    {
        u32Ret = VPP_ERR;
        LOGE("BufSize:%u < ReqBufSize:%u", pstInpBuf->stPixel.u32ValidLen, u32BufReqSz);
    }
    else
    {
        u32Ret = u32VppIpFrcMe_TmeQBuf(pstCb, pstInpBuf, VPP_PORT_INPUT);
        if (u32Ret != VPP_OK)
        {
            LOGE("u32VppIpFrcMe_TmeQBuf(Ip idx=%d) failed!", pstInpBuf->u32Idx);
        }
        else
        {
            // Set the index bit to indicate that this index is taken
            VPP_FLAG_SET(pstCb->u32TmeIpBufIdxMap, (1 << pstInpBuf->u32Idx));
            // Save the reference in the stTmeIn queue
            u32 = u32VppIp_PortBufPut(&pstCb->stTmeIn, pstInpBuf, NULL, NULL);
            LOGE_IF(u32, "u32VppIp_PortBufPut(stTmeIn) failed ret=%u", u32);

            // For EOS buffer,set IP_EOS_PENDING(no inp buffer to be queued untill this flag clears)
            if (VPP_FLAG_IS_SET(pstInpBuf->pBuf->flags, VPP_BUFFER_FLAG_EOS))
            {
                VPP_FLAG_SET(pstCb->u32MeFlags, IP_EOS_PENDING);
                if (VPP_FLAG_IS_SET(pstInpBuf->u32InternalFlags, FRC_ME_BUF_INTERN_EOS))
                    VPP_FLAG_SET(pstCb->u32MeFlags, FRC_ME_INTERN_INPUT_EOS_QUEUED);
                LOGI("EOS queued to TME: SET IP_EOS_PENDING");
            }
            pstCb->u32IpBufsSentToTME++;
            FRC_ME_LOG_TME_BUF_STATUS(pstCb);
        }
    }

    FRC_ME_UNLOCK(pstCb);
    return u32Ret;
}

// Queue output buffer to TME
static uint32_t u32VppIpFrcMe_QueueOutputBuf(t_StVppIpFrcMeCb *pstCb,
                                             t_StVppBuf *pstOutBuf)
{
    uint32_t u32RetBufToClient = VPP_FALSE;
    uint32_t u32, u32Ret = VPP_OK;

    FRC_ME_LOCK(pstCb);

    if (pstOutBuf->stPixel.u32ValidLen < pstCb->u32OpBufSize)
    {
        u32Ret = VPP_ERR;
        LOGE("BufSize:%u < ReqBufSize:%u", pstOutBuf->stPixel.u32ValidLen, pstCb->u32OpBufSize);
    }
    else
    {
        u32Ret = u32VppIpFrcMe_TmeQBuf(pstCb, pstOutBuf, VPP_PORT_OUTPUT);
        if (u32Ret != VPP_OK)
        {
            LOGE("u32VppIpFrcMe_TmeQBuf(Op idx=%d) failed! ret=%u", pstOutBuf->u32Idx, u32Ret);
            u32 = u32VppIp_PortBufPut(&pstCb->stOutput, pstOutBuf, NULL, NULL);
            LOGE_IF(u32, "u32VppIp_PortBufPut(stOutput) failed ret=%u", u32);
        }
        else
        {
            VPP_FLAG_SET(pstCb->u32TmeOpBufIdxMap, (1 << pstOutBuf->u32Idx));
            u32 = u32VppIp_PortBufPut(&pstCb->stTmeOut, pstOutBuf, NULL, NULL);
            LOGE_IF(u32, "u32VppIp_PortBufPut(stTmeOut) failed ret=%u", u32);

            pstCb->u32OpBufsSentToTME++;
            FRC_ME_LOG_TME_BUF_STATUS(pstCb);
        }
    }

    FRC_ME_UNLOCK(pstCb);
    return u32Ret;
}

static uint32_t u32VppIpFrcMe_GetStateParams(t_StVppIpFrcMeCb *pstCb,
                                             t_StFrcMeStateParams *pstParams)
{
    uint32_t u32Ret = VPP_OK;

    VPP_RET_IF_NULL(pstCb, VPP_ERR_PARAM);
    VPP_RET_IF_NULL(pstParams, VPP_ERR_PARAM);

    FRC_ME_LOCK(pstCb);

    // Update the state based on the next buffer
    // There might be a input buffer queued in after the previous check (@ WorkerThreadShouldSleep)
    if (!VPP_FLAG_IS_SET(pstCb->u32MeFlags, IP_EOS_PENDING))
        vVppIpFrcMe_UpdateStateMachine(pstCb);

    // If the flush is pending or the buffer requirement is not met, then return
    if (!VPP_FLAG_IS_SET(pstCb->u32MeFlags, IP_FLUSH_PENDING))
        pstParams->bProcBufMet = u32VppIpFrcMe_ProcBufReqMet(pstCb);

    if (!pstParams->bProcBufMet)
    {
        FRC_ME_UNLOCK(pstCb);
        return u32Ret;
    }

    if (FRC_ME_IS_STATE(pstCb, FRC_ME_STATE_FEED))
    {
        // Queue input buffer only if available in the internal queue and the TME queue is not full
        // Do not queue any i/p if EOS(internal/external) is pending
        if (u32VppBufPool_Cnt(&pstCb->stInput.stPendingQ) && FRC_ME_TME_INPQ_AVAIL(pstCb) &&
            !VPP_FLAG_IS_SET(pstCb->u32MeFlags, IP_EOS_PENDING))
        {
            pstParams->bQInput = VPP_TRUE;
        }

        // Queue output buffer only if available in the internal queue
        // and the TME queue is not full
        if (u32VppBufPool_Cnt(&pstCb->stOutput.stPendingQ) && FRC_ME_TME_OUTQ_AVAIL(pstCb))
        {
            pstParams->bQOutput = VPP_TRUE;
        }
    }
    else if (FRC_ME_IS_STATE(pstCb, FRC_ME_STATE_BYPASS))
    {
        // If there are no input buffers with TME, then bypass the current buffer
        // Wait till the EOS Pending is cleared to bypass the buffer
        if (!FRC_ME_TME_INPQ_INPROC(pstCb) &&
            !VPP_FLAG_IS_SET(pstCb->u32MeFlags, IP_EOS_PENDING))
        {
            pstParams->bBypassInput = VPP_TRUE;
        }
        else
        {
            if (!VPP_FLAG_IS_SET(pstCb->u32MeFlags, IP_EOS_PENDING))
            {
                // A new bypass input buffer came which requires ME Drain
                // Queue input internal-EOS buffer
                if (FRC_ME_TME_INPQ_AVAIL(pstCb))
                {
                    pstParams->bQInternalEos = VPP_TRUE;
                }
            }
            else
            {
                pstParams->bQOutput = VPP_TRUE;
            }
        }
    }

    // Get the free Index for Input/Output which needs to be queued
    if (pstParams->bQInput || pstParams->bQInternalEos)
    {
        pstParams->u32InpFreeIdx = u32VppIpFrcMe_GetFreeIndex(pstCb, VPP_PORT_INPUT);
        if (pstParams->u32InpFreeIdx == FRC_ME_ERROR_INDEX)
        {
            FRC_ME_UNLOCK(pstCb);
            LOGE("Input buffer index not available, Map:%u", pstCb->u32TmeIpBufIdxMap);
            return VPP_ERR;
        }
    }

    if (pstParams->bQOutput)
    {
        pstParams->u32OutFreeIdx = u32VppIpFrcMe_GetFreeIndex(pstCb, VPP_PORT_OUTPUT);
        if ( pstParams->u32OutFreeIdx == FRC_ME_ERROR_INDEX)
        {
            FRC_ME_UNLOCK(pstCb);
            LOGE("Output buffer index not available, Map:%u", pstCb->u32TmeOpBufIdxMap);
            return VPP_ERR;
        }
    }
    FRC_ME_UNLOCK(pstCb);
    return u32Ret;
}

// Implements the ME state machine
static uint32_t u32VppIpFrcMe_ProcessBuffer(t_StVppIpFrcMeCb *pstCb)
{
    uint32_t u32Ret = VPP_OK;
    t_StVppBuf *pstBuf = NULL;
    t_StFrcMeStateParams stMeStateParams;

    LOG_ENTER();
    VPP_RET_IF_NULL(pstCb, VPP_ERR_PARAM);

    memset(&stMeStateParams, 0, sizeof(t_StFrcMeStateParams));
    u32Ret = u32VppIpFrcMe_GetStateParams(pstCb, &stMeStateParams);
    if (u32Ret)
    {
        LOGE("u32VppIpFrcMe_GetStateParams() failed, ret=%u", u32Ret);
        return u32Ret;
    }

    if (FRC_ME_IS_STATE(pstCb, FRC_ME_STATE_IDLE) || !stMeStateParams.bProcBufMet)
    {
        LOGE("Should sleep, should not be here! %d", pstCb->eFrcMeState);
        return VPP_ERR_STATE;
    }

    // Get the buffer from the Q for either queueing to TME or bypassing it.
    if (stMeStateParams.bQInput || stMeStateParams.bBypassInput)
    {
        u32Ret = u32VppIp_PortBufGet(&pstCb->stInput, &pstBuf, &pstCb->mutex);
        if ((pstBuf == NULL) || u32Ret)
        {
            stMeStateParams.bQInput = VPP_FALSE;
            stMeStateParams.bBypassInput = VPP_FALSE;
            u32Ret = VPP_ERR;
            LOGE("u32VppIp_PortBufGet(Inp) failed, pstBuf:%p, ret=%u", pstBuf, u32Ret);
        }
    }

    if (stMeStateParams.bQInput)
    {
        pstBuf->u32Idx = stMeStateParams.u32InpFreeIdx;
        u32Ret = u32VppIpFrcMe_QueueInputBuf(pstCb, pstBuf);
        if (u32Ret != VPP_OK)
        {
            LOGE("u32VppIpFrcMe_QueueInputBuf() failed ret=%u", u32Ret);
            // Return all buffers except the internal-EOS buffer
            if (!VPP_FLAG_IS_SET(pstBuf->u32InternalFlags, FRC_ME_BUF_INTERN_EOS))
            {
                VPP_FLAG_SET(pstBuf->u32InternalFlags, VPP_BUF_FLAG_BYPASS);
                u32VppIpFrcMe_ReturnBufToClient(pstCb, VPP_PORT_INPUT, pstBuf);
            }
        }
    }
    else if (stMeStateParams.bQInternalEos && FRC_ME_IS_STATE(pstCb, FRC_ME_STATE_BYPASS))
    {
        pstBuf = &pstCb->stBufEos;
        pstBuf->u32Idx = stMeStateParams.u32InpFreeIdx;
        pstBuf->pBuf->timestamp = pstCb->MeLastTimestamp + FRC_ME_DEFAULT_DELAY;
        u32Ret = u32VppIpFrcMe_QueueInputBuf(pstCb, pstBuf);
        LOGE_IF(u32Ret, "u32VppIpFrcMe_QueueInputBuf(Int-EOS) failed ret=%u", u32Ret);
    }
    else if (stMeStateParams.bBypassInput && FRC_ME_IS_STATE(pstCb, FRC_ME_STATE_BYPASS))
    {
        VPP_FLAG_CLR(pstBuf->u32InternalFlags, FRC_ME_BUF_INTERN_BYPASS);
        FRC_ME_STATE_SET(pstCb, FRC_ME_STATE_IDLE);
        LOGI("Bypass input buffer fd:%d", pstBuf->stPixel.fd);
        u32VppIpFrcMe_ReturnBufToClient(pstCb, VPP_PORT_INPUT, pstBuf);
    }

    if (stMeStateParams.bQOutput)
    {
        u32Ret = u32VppIp_PortBufGet(&pstCb->stOutput, &pstBuf, &pstCb->mutex);
        if ((pstBuf == NULL) || u32Ret)
        {
            u32Ret = VPP_ERR;
            LOGE("u32VppIp_PortBufGet(Out) failed, pstBuf:%p, ret=%u", pstBuf, u32Ret);
        }
        else
        {
            pstBuf->u32Idx = stMeStateParams.u32OutFreeIdx;
            u32Ret = u32VppIpFrcMe_QueueOutputBuf(pstCb, pstBuf);
            if (u32Ret != VPP_OK)
            {
                LOGE("u32VppIpFrcMe_QueueOutputBuf() failed ret=%u", u32Ret);
                pstBuf->stPixel.u32FilledLen = 0;
                u32VppIpFrcMe_ReturnBufToClient(pstCb, VPP_PORT_OUTPUT, pstBuf);
            }
        }
    }

    LOG_EXIT_RET(u32Ret);
}

// Receiver thread in which ME is polled for buffers done and events.
// It sleeps if no driver buffers or events available.
static void *vpVppIpFrcMe_Receiver(void *pv)
{
    LOGD("%s started", __func__);

    struct pollfd pstFdesc[FRC_ME_POLL_NUM_FDS];
    uint32_t u32;
    t_StVppIpFrcMeCb *pstCb = (t_StVppIpFrcMeCb *)pv;

    LOGI("Receiver thread started");

    // Signal back to main thread that we've launched and are ready to go
    vVppIpFrcMe_SignalReceiverStart(pstCb);

    pstFdesc[0].fd = pstCb->MeFd;
    pstFdesc[0].events = POLLIN | POLLRDNORM | POLLOUT | POLLWRNORM | POLLRDBAND | POLLPRI;

    pstFdesc[1].fd = pstCb->MePollNotification;
    pstFdesc[1].events = POLLIN | POLLERR;

    while (1)
    {
        // Receiver exit command
        if (pstCb->u32MeExitReceiver == VPP_TRUE)
        {
            pstCb->u32MeExitReceiver = VPP_FALSE;
            break;
        }

        VPP_IP_PROF_START(&pstCb->stBase, FRC_ME_STAT_RECEIVER_SLEEP);
        u32 = u32VppIpFrcMe_TmePollEvents(pstFdesc);
        VPP_IP_PROF_STOP(&pstCb->stBase, FRC_ME_STAT_RECEIVER_SLEEP);
        if (u32 == VPP_ERR_RESOURCES)
            break;
        else if (u32 == VPP_PENDING)
            continue;

        // Receiver exit event
        if ((pstFdesc[1].revents & POLLIN) || (pstFdesc[1].revents & POLLERR))
        {
             LOGE("MeReceiver thread interrupted to be exited");
             break;
        }

        VPP_IP_PROF_START(&pstCb->stBase, FRC_ME_STAT_RECEIVER);
        // Read Input buffers returned my the driver
        if ((pstFdesc[0].revents & POLLOUT) || (pstFdesc[0].revents & POLLWRNORM))
            vVppIpFrcMe_HandleReturnedInputBuf(pstCb);

        // Read Output buffers returned my the driver
        if ((pstFdesc[0].revents & POLLIN) || (pstFdesc[0].revents & POLLRDNORM))
            vVppIpFrcMe_HandleReturnedOutputBuf(pstCb);

        // Read and take actions based on events returned by the driver
        if (pstFdesc[0].revents & POLLPRI)
            vVppIpFrcMe_HandleReturnedEvents(pstCb);

        VPP_IP_PROF_STOP(&pstCb->stBase, FRC_ME_STAT_RECEIVER);
    }
    return NULL;
}

// Main worker thread which processes commands and buffers.
static void *vpVppIpFrcMe_Worker(void *pv)
{
    t_StVppIpFrcMeCb *pstCb = (t_StVppIpFrcMeCb *)pv;

    LOGI("Worker thread started");

    // Signal back to main thread that we've launched and are ready to go
    vVppIpFrcMe_SignalWorkerStart(pstCb);

    while (1)
    {
        uint32_t u32Ret;
        t_StVppIpCmd stCmd;

        FRC_ME_LOCK(pstCb);
        while (u32VppIpFrcMe_WorkerThreadShouldSleep(pstCb))
        {
            VPP_IP_PROF_START(&pstCb->stBase, FRC_ME_STAT_WORKER_SLEEP);
            FRC_ME_WAIT(pstCb);
            VPP_IP_PROF_STOP(&pstCb->stBase, FRC_ME_STAT_WORKER_SLEEP);
        }

        VPP_IP_PROF_START(&pstCb->stBase, FRC_ME_STAT_WORKER);
        // No Processing of any Commands if we are already in the middle of EOS or DRAIN
        if (!VPP_FLAG_IS_SET(pstCb->u32MeFlags, IP_DRAIN_PENDING) &&
            !VPP_FLAG_IS_SET(pstCb->u32MeFlags, IP_EOS_PENDING))
        {
            u32Ret = u32VppIpFrcMe_CmdGet(pstCb, &stCmd);
            if (u32Ret == VPP_OK)
            {
                FRC_ME_UNLOCK(pstCb);
                // Process the commands from the command queue
                LOG_CMD("ProcessCmd", stCmd.eCmd);

                if (stCmd.eCmd == VPP_IP_CMD_THREAD_EXIT)
                {
                    VPP_IP_PROF_STOP(&pstCb->stBase, FRC_ME_STAT_WORKER);
                    break;
                }
                else if (stCmd.eCmd == VPP_IP_CMD_CLOSE)
                {
                    u32VppIpFrcMe_ProcCmdClose(pstCb);
                }
                else if (stCmd.eCmd == VPP_IP_CMD_FLUSH)
                {
                    u32VppIpFrcMe_ProcCmdFlush(pstCb, &stCmd);
                }
                else if (stCmd.eCmd == VPP_IP_CMD_DRAIN)
                {
                    u32VppIpFrcMe_ProcCmdDrain(pstCb);
                }
                else
                    LOGE("Unknown command in queue %u", stCmd.eCmd);

                VPP_IP_PROF_STOP(&pstCb->stBase, FRC_ME_STAT_WORKER);
                continue;
            }
        }

        if (pstCb->eState == VPP_IP_STATE_ACTIVE)
        {

            FRC_ME_UNLOCK(pstCb);
            u32VppIpFrcMe_ProcessBuffer(pstCb);

            VPP_IP_PROF_STOP(&pstCb->stBase, FRC_ME_STAT_WORKER);
            continue;
        }

        FRC_ME_UNLOCK(pstCb);
        LOGE("%s woke up, but did no work", __func__);
        VPP_IP_PROF_STOP(&pstCb->stBase, FRC_ME_STAT_WORKER);
    }

    LOGI("%s exited", __func__);

    return NULL;
}

static void vVppIpFrcMe_InitMeInternalParams(t_StVppIpFrcMeCb *pstCb)
{
    uint32_t u32;

    pstCb->u32MeFlags = 0;
    pstCb->MeLastTimestamp = 0;

    pstCb->u32IpBufsSentToTME = 0;
    pstCb->u32IpBufsRxedFromTME = 0;
    pstCb->u32OpBufsSentToTME = 0;
    pstCb->u32OpBufsRxedFromTME = 0;
    pstCb->u32IpEosSentToTME = 0;
    pstCb->u32IpEosRxedFromTME = 0;
    pstCb->u32IpBufsRxedFromCl = 0;
    pstCb->u32IpBufsSentToCl = 0;
    pstCb->u32OpBufsRxedFromCl = 0;
    pstCb->u32OpBufsSentToCl = 0;

    pstCb->u32MeExitReceiver = VPP_FALSE;

    pstCb->u32TmeIpBufIdxMap = 0;
    pstCb->u32TmeOpBufIdxMap = 0;

    FRC_ME_STATE_SET(pstCb, FRC_ME_STATE_IDLE);
}

/************************************************************************
 * Global Functions
 ************************************************************************/

// Allocate ME context, create OS objects and start the ME working thread.
void *vpVppIpFrcMe_Init(t_StVppCtx *pstCtx, uint32_t u32Flags, t_StVppCallback stCb)
{
    LOG_ENTER();

    int rc;
    uint32_t u32, u32Ret;
    t_StVppIpFrcMeCb *pstCb;

    VPP_RET_IF_NULL(pstCtx, NULL);

    //Allocate ME context
    pstCb = calloc(sizeof(t_StVppIpFrcMeCb), 1);
    if (!pstCb)
    {
        LOGE("calloc() failed for FrcMe context");
        goto ERROR_MALLOC;
    }

    u32Ret = u32VppIp_SetBase(pstCtx, u32Flags, stCb, &pstCb->stBase);
    if(u32Ret != VPP_OK)
    {
       LOGE("u32VppIp_SetBase() ret=%u", u32Ret);
       goto ERROR_SETBASE;
    }

    u32Ret = VPP_IP_PROF_REGISTER(&pstCb->stBase, astFrcMeStatsCfg, u32FrcMeStatCnt);
    LOGE_IF(u32Ret != VPP_OK, "unable to register stats, u32Ret=%u", u32Ret);

    // ME buffer and command queues initialization
    if (u32VppIp_PortInit(&pstCb->stInput) != VPP_OK)
    {
        LOGE("Unable to u32VppIp_PortInit() input queue");
        goto ERROR_INPUT_Q_INIT;
    }

    if (u32VppIp_PortInit(&pstCb->stOutput) != VPP_OK)
    {
        LOGE("Unable to u32VppIp_PortInit() output queue");
        goto ERROR_OUTPUT_Q_INIT;
    }

    // TME buffer queues initialization
    if (u32VppIp_PortInit(&pstCb->stTmeIn) != VPP_OK)
    {
        LOGE("Unable to u32VppIp_PortInit() TME input queue");
        goto ERROR_TME_INPUT_Q_INIT;
    }

    if (u32VppIp_PortInit(&pstCb->stTmeOut) != VPP_OK)
    {
        LOGE("Unable to u32VppIp_PortInit() TME output queue");
        goto ERROR_TME_OUTPUT_Q_INIT;
    }

    if (vpp_queue_init(&pstCb->stCmdQ, FRC_ME_CMD_Q_SZ) != VPP_OK)
    {
        LOGE("Unable to vpp_queue_init");
        goto ERROR_CMD_Q_INIT;
    }

    // ME Worker thread objects initialization
    rc = pthread_mutex_init(&pstCb->mutex, NULL);
    if (rc)
    {
        LOGE("Unable to initialize FrcMe mutex %d-->%s", rc, strerror(rc));
        goto ERROR_MUTEX_INIT;
    }

    rc = pthread_cond_init(&pstCb->cond, NULL);
    if (rc)
    {
        LOGE("Unable to init FrcMe condition variable %d-->%s", rc, strerror(rc));
        goto ERROR_COND_INIT;
    }

    rc = pthread_create(&pstCb->wkThread, NULL, vpVppIpFrcMe_Worker, pstCb);
    if (rc)
    {
        LOGE("Unable to spawn FrcMe worker thread %d-->%s", rc, strerror(rc));
        goto ERROR_THREAD_CREATE;
    }

    // Wait for the thread to launch
    vVppIpFrcMe_WaitWorkerStart(pstCb);

    VPP_IP_STATE_SET(pstCb, VPP_IP_STATE_INITED);
    return pstCb;

ERROR_THREAD_CREATE:
    rc = pthread_cond_destroy(&pstCb->cond);
    LOGE_IF(rc, "pthread_cond_destroy failed %d-->%s", rc, strerror(rc));

ERROR_COND_INIT:
    rc = pthread_mutex_destroy(&pstCb->mutex);
    LOGE_IF(rc, "pthread_mutex_destroy failed %d-->%s", rc, strerror(rc));

ERROR_MUTEX_INIT:
    vpp_queue_term(&pstCb->stCmdQ);

ERROR_CMD_Q_INIT:
    u32 = u32VppIp_PortTerm(&pstCb->stTmeOut);
    LOGE_IF(u32 != VPP_OK, "u32VppIp_PortTerm(stTmeOut) failed, u32=%u", u32);

ERROR_TME_OUTPUT_Q_INIT:
    u32 = u32VppIp_PortTerm(&pstCb->stTmeIn);
    LOGE_IF(u32 != VPP_OK, "u32VppIp_PortTerm(stTmeIn) failed, u32=%u", u32);

ERROR_TME_INPUT_Q_INIT:
    u32 = u32VppIp_PortTerm(&pstCb->stOutput);
    LOGE_IF(u32 != VPP_OK, "u32VppIp_PortTerm(stOutput) failed, u32=%u", u32);

ERROR_OUTPUT_Q_INIT:
    u32 = u32VppIp_PortTerm(&pstCb->stInput);
    LOGE_IF(u32 != VPP_OK, "u32VppIp_PortTerm(stInput) failed, u32=%u", u32);

ERROR_INPUT_Q_INIT:
    u32 = VPP_IP_PROF_UNREGISTER(&pstCb->stBase);
    LOGE_IF(u32 != VPP_OK, "Unable to unregister stats, u32=%u", u32);

ERROR_SETBASE:
    free(pstCb);

ERROR_MALLOC:
    return NULL;
}

// Terminate worker thread, flush ports, destroy OS objects, deallocate ME context
void vVppIpFrcMe_Term(void *ctx)
{
    LOG_ENTER();

    int rc;
    uint32_t u32Ret;
    t_StVppIpFrcMeCb *pstCb;
    t_StVppIpCmd stCmd;

    VPP_RET_VOID_IF_NULL(ctx);
    pstCb = FRC_ME_CB_GET(ctx);

    if (VPP_IP_STATE_EQUAL(pstCb, VPP_IP_STATE_ACTIVE))
    {
        LOGE("Still in active state. Closing.");
        u32Ret = u32VppIpFrcMe_Close(ctx);
        LOGE_IF(u32Ret, "u32VppIpFrcMe_Close failed, u32Ret=%u", u32Ret);
    }

    // Terminate worker thread
    stCmd.eCmd = VPP_IP_CMD_THREAD_EXIT;
    u32VppIpFrcMe_CmdPut(pstCb, &stCmd);

    rc = pthread_join(pstCb->wkThread, NULL);
    LOGE_IF(rc, "pthread_join failed: %d --> %s", rc, strerror(rc));

    // Flush ports
    u32Ret = u32VppIpFrcMe_ReleasePortBuf(pstCb, VPP_PORT_INPUT);
    LOGE_IF(u32Ret, "ReleasePortBuf(VPP_PORT_INPUT) failed, u32Ret=%u", u32Ret);

    u32Ret = u32VppIpFrcMe_ReleasePortBuf(pstCb, VPP_PORT_OUTPUT);
    LOGE_IF(u32Ret, "ReleasePortBuf(VPP_PORT_OUTPUT) failed, u32Ret=%u", u32Ret);

    rc = pthread_cond_destroy(&pstCb->cond);
    LOGE_IF(rc, "pthread_cond_destroy failed: %d --> %s", rc, strerror(rc));

    rc = pthread_mutex_destroy(&pstCb->mutex);
    LOGE_IF(rc, "pthread_mutex_destroy failed: %d --> %s", rc, strerror(rc));

    // Clean queues
    vpp_queue_term(&pstCb->stCmdQ);

    u32Ret = u32VppIp_PortTerm(&pstCb->stInput);
    LOGE_IF(u32Ret, "u32VppIp_PortTerm(stInput) failed, u32Ret=%u", u32Ret);
    u32Ret = u32VppIp_PortTerm(&pstCb->stOutput);
    LOGE_IF(u32Ret, "u32VppIp_PortTerm(stOutput) failed, u32Ret=%u", u32Ret);
    u32Ret = u32VppIp_PortTerm(&pstCb->stTmeIn);
    LOGE_IF(u32Ret, "u32VppIp_PortTerm(stTmeIn) failed, u32Ret=%u", u32Ret);
    u32Ret = u32VppIp_PortTerm(&pstCb->stTmeOut);
    LOGE_IF(u32Ret, "u32VppIp_PortTerm(stTmeOut) failed, u32Ret=%u", u32Ret);

    u32Ret = VPP_IP_PROF_UNREGISTER(&pstCb->stBase);
    LOGE_IF(u32Ret, "Unable to unregister stats, u32Ret=%u", u32Ret);

    // Deallocate ME context
    free(pstCb);

    LOG_EXIT();
}

// Validate configuration, Allocate internal-EOS, Open and configure TME.
uint32_t u32VppIpFrcMe_Open(void *ctx)
{
    LOG_ENTER();

    int rc;
    t_StVppIpFrcMeCb *pstCb;
    uint32_t u32, u32Ret = VPP_OK;

    VPP_RET_IF_NULL(ctx, VPP_ERR_PARAM);
    pstCb = FRC_ME_CB_GET(ctx);

    if (!VPP_IP_STATE_EQUAL(pstCb, VPP_IP_STATE_INITED))
        return VPP_ERR_STATE;

    FRC_ME_LOCK(pstCb);

    vVppIpFrcMe_InitMeInternalParams(pstCb);

    // Validate the port configuration params
    u32Ret = u32VppIpFrcMe_ValidateConfig(pstCb->stInput.stParam, pstCb->stOutput.stParam);
    if (u32Ret != VPP_OK)
    {
        LOGE("u32VppIpFrcMe_ValidateConfig() failed ret=%u", u32Ret);
        goto ERROR_RET_PARAM;
    }

    // Allocate and configure the drain/EOS buffer
    u32Ret = u32VppIpFrcMe_AllocInternalEosBuf(pstCb);
    if (u32Ret != VPP_OK)
    {
        LOGE("u32VppIpFrcMe_AllocInternalEosBuf() failed ret=%u", u32Ret);
        goto ERROR_ALLOC_EOS;
    }

    // Open TME Device
    u32Ret = u32VppIpFrcMe_TmeOpenDev(pstCb);
    if (u32Ret != VPP_OK)
    {
        LOGE("u32VppIpFrcMe_TmeOpenDev() failed ret=%u", u32Ret);
        goto ERROR_TME_OPEN_DEV;
    }

    // Configure TME
    u32Ret = u32VppIpFrcMe_TmeAcquireResources(pstCb);
    if (u32Ret != VPP_OK)
    {
        LOGE("u32VppIpFrcMe_TmeAcquireResources() failed!");
        goto ERROR_TME_CONFIG;
    }

    // Allocate Receiver OS objects
    rc = pthread_cond_init(&pstCb->rxCond, NULL);
    if (rc)
    {
        LOGE("Unable to init rxCond condition variable %d-->%s", rc, strerror(rc));
        goto ERROR_RCV_COND_INIT;
    }

    rc = pthread_create(&pstCb->rxThread, NULL, vpVppIpFrcMe_Receiver, pstCb);
    if (rc)
    {
        LOGE("Unable to spawn rxThread Receiver thread %d-->%s", rc, strerror(rc));
        goto ERROR_RCV_THREAD_CREATE;
    }

    VPP_IP_STATE_SET(pstCb, VPP_IP_STATE_ACTIVE);

    FRC_ME_UNLOCK(pstCb);
    // Wait for the thread to launch before returning
    vVppIpFrcMe_WaitReceiverStart(pstCb);

    LOG_EXIT_RET(VPP_OK);

ERROR_RCV_THREAD_CREATE:
    LOGI("destroying condition variable");
    rc = pthread_cond_destroy(&pstCb->rxCond);
    LOGE_IF(rc, "pthread_cond_destroy failed %d-->%s", rc, strerror(rc));

ERROR_RCV_COND_INIT:
    u32 = u32VppIpFrcMe_TmeReleaseResources(pstCb);
    LOGE_IF(u32Ret != VPP_OK, "TmeReleaseResources() failed, u32=%u", u32);

ERROR_TME_CONFIG:
    u32 = u32VppIpFrcMe_TmeCloseDev(pstCb);
    LOGE_IF(u32Ret != VPP_OK, "TmeCloseDev() failed, u32=%u", u32);

ERROR_TME_OPEN_DEV:
    u32 = u32VppIpFrcMe_FreeInternalEosBuf(pstCb);
    LOGE_IF(u32 != VPP_OK, "u32VppIpFrcMe_FreeInternalEosBuf() failed, u32=%u", u32);

ERROR_ALLOC_EOS:
ERROR_RET_PARAM:
    FRC_ME_UNLOCK(pstCb);
    LOG_EXIT_RET(u32Ret);
}

uint32_t u32VppIpFrcMe_Close(void *ctx)
{
    LOG_ENTER();

    t_StVppIpFrcMeCb *pstCb;
    t_StVppIpCmd stCmd;

    VPP_RET_IF_NULL(ctx, VPP_ERR_PARAM);
    pstCb = FRC_ME_CB_GET(ctx);

    if (!VPP_IP_STATE_EQUAL(pstCb, VPP_IP_STATE_ACTIVE))
    {
        LOGD("%s() VPP_ERR_STATE. pstCb->eState=%d", __func__, pstCb->eState);
        LOG_EXIT_RET(VPP_ERR_STATE);
    }

    stCmd.eCmd = VPP_IP_CMD_CLOSE;
    u32VppIpFrcMe_CmdPut(pstCb, &stCmd);

    LOGI(">> waiting on semaphore");
    sem_wait(&pstCb->sem);
    LOGI(">> got semaphore");

    LOG_EXIT_RET(pstCb->asyncRes.u32MeCloseRet);
}

uint32_t u32VppIpFrcMe_SetParam(void *ctx, enum vpp_port port, struct vpp_port_param param)
{
    LOG_ENTER();

    t_StVppIpFrcMeCb *pstCb;
    uint32_t u32Ret = VPP_OK;

    VPP_RET_IF_NULL(ctx, VPP_ERR_PARAM);
    pstCb = FRC_ME_CB_GET(ctx);

    if (!VPP_IP_STATE_EQUAL(pstCb, VPP_IP_STATE_INITED))
        LOG_EXITI_RET(VPP_ERR_STATE);

    if (u32VppIpFrcMe_ValidatePortParam(param) != VPP_OK)
    {
        LOGE("given invalid port params.");
        return VPP_ERR_PARAM;
    }

    FRC_ME_LOCK(pstCb);

    if (port == VPP_PORT_INPUT)
        pstCb->stInput.stParam = param;
    else if (port == VPP_PORT_OUTPUT)
        pstCb->stOutput.stParam = param;
    else
        u32Ret = VPP_ERR_PARAM;

    LOG_PARAM_PTR(I, &pstCb->stInput.stParam, &pstCb->stOutput.stParam);

    FRC_ME_UNLOCK(pstCb);

    LOG_EXIT_RET(u32Ret);
}

uint32_t u32VppIpFrcMe_SetCtrl(void *ctx, struct hqv_control stHqvCtrl)
{
    LOG_ENTER();

    VPP_UNUSED(stHqvCtrl);

    t_StVppIpFrcMeCb *pstCb;
    uint32_t u32Ret = VPP_OK;

    VPP_RET_IF_NULL(ctx, VPP_ERR_PARAM);
    pstCb = FRC_ME_CB_GET(ctx);

    if (!VPP_IP_STATE_EQUAL(pstCb, VPP_IP_STATE_INITED))
        LOG_EXITI_RET(VPP_ERR_STATE);

    LOG_EXIT_RET(u32Ret);
}

// Get the minimal number of input and output buffers necessary for processing
uint32_t u32VppIpFrcMe_GetBufferRequirements(void *ctx,
                                             t_StVppIpBufReq *o_pstInputBufReq,
                                             t_StVppIpBufReq *o_pstOutputBufReq)
{
    LOG_ENTER();

    t_StVppIpFrcMeCb *pstCb;

    VPP_RET_IF_NULL(ctx, VPP_ERR_PARAM);
    VPP_RET_IF_NULL(o_pstInputBufReq, VPP_ERR_PARAM);
    VPP_RET_IF_NULL(o_pstOutputBufReq, VPP_ERR_PARAM);

    pstCb = FRC_ME_CB_GET(ctx);

    if ((!VPP_IP_STATE_EQUAL(pstCb, VPP_IP_STATE_INITED)) &&
        (!VPP_IP_STATE_EQUAL(pstCb, VPP_IP_STATE_ACTIVE)))
        LOG_EXIT_RET(VPP_ERR_STATE);

    o_pstInputBufReq->u32MinCnt = pstCb->u32IpBufsTmeQSize;
    o_pstInputBufReq->u32PxSz = pstCb->u32IpBufSize;
    o_pstInputBufReq->u32ExSz = 0;
    LOGI("InputBuf: MinCnt:%u PxSz:%u", o_pstInputBufReq->u32MinCnt, o_pstInputBufReq->u32PxSz);

    o_pstOutputBufReq->u32MinCnt = pstCb->u32OpBufsTmeQSize;
    o_pstOutputBufReq->u32PxSz = pstCb->u32OpBufSize;
    o_pstOutputBufReq->u32ExSz = 0;
    LOGI("OutputBuf: MinCnt:%u PxSz:%u", o_pstOutputBufReq->u32MinCnt, o_pstOutputBufReq->u32PxSz);

    LOG_EXIT_RET(VPP_OK);
}

uint32_t u32VppIpFrcMe_QueueBuf(void *ctx, enum vpp_port ePort, t_StVppBuf *pBuf)
{
    LOG_ENTER();

    uint32_t u32BypassFlags;
    t_StVppIpFrcMeCb *pstCb;
    t_EVppBufType eBufType;
    uint32_t u32Ret = VPP_OK;

    VPP_RET_IF_NULL(ctx, VPP_ERR_PARAM);
    VPP_RET_IF_NULL(pBuf, VPP_ERR_PARAM);
    if (ePort >= VPP_PORT_MAX)
        return VPP_ERR_PARAM;

    pstCb = FRC_ME_CB_GET(ctx);

    if (!VPP_IP_STATE_EQUAL(pstCb, VPP_IP_STATE_ACTIVE))
        LOG_EXIT_RET(VPP_ERR_STATE);

    vVppIpCbLog(&pstCb->stBase.stCb, pBuf, eVppLogId_IpQueueBuf);

    if (ePort == VPP_PORT_INPUT)
    {

        if (!VPP_FLAG_IS_SET(pBuf->pBuf->flags, VPP_BUFFER_FLAG_EOS) &&
            (pBuf->stPixel.u32FilledLen != 0))
        {
            //Store the current input timestamp
            pstCb->MeLastTimestamp = pBuf->pBuf->timestamp;
        }

        if ((pBuf->stPixel.u32FilledLen == 0) || (pBuf->eBufType != eVppBufType_Progressive))
        {
            VPP_FLAG_SET(pBuf->u32InternalFlags, FRC_ME_BUF_INTERN_BYPASS);
            LOGI("Input with u32FilledLen=%u eBuftype=%u detected! Setting internal bypass flag",
                 pBuf->stPixel.u32FilledLen, pBuf->eBufType);
        }

        u32Ret = u32VppIp_PortBufPut(&pstCb->stInput, pBuf, &pstCb->mutex, &pstCb->cond);
        if (u32Ret == VPP_OK)
            pstCb->u32IpBufsRxedFromCl++;
        else
            LOGE("u32VppIpFrcMe_PortBufPut(stInput) failed");
    }
    else if (ePort == VPP_PORT_OUTPUT)
    {
        pBuf->pBuf->timestamp = 0;
        u32Ret = u32VppIp_PortBufPut(&pstCb->stOutput, pBuf, &pstCb->mutex, &pstCb->cond);
        if (u32Ret == VPP_OK)
            pstCb->u32OpBufsRxedFromCl++;
        else
            LOGE("u32VppIpFrcMe_PortBufPut(stOutput) failed");
    }

    LOG_EXIT_RET(u32Ret);
}

uint32_t u32VppIpFrcMe_Flush(void *ctx, enum vpp_port ePort)
{
    LOG_ENTER();

    t_StVppIpFrcMeCb *pstCb;
    t_StVppIpCmd stCmd;
    uint32_t u32Ret = VPP_OK;

    VPP_RET_IF_NULL(ctx, VPP_ERR_PARAM);
    if (ePort >= VPP_PORT_MAX)
        return VPP_ERR_PARAM;

    pstCb = FRC_ME_CB_GET(ctx);

    if ((!VPP_IP_STATE_EQUAL(pstCb, VPP_IP_STATE_INITED)) &&
        (!VPP_IP_STATE_EQUAL(pstCb, VPP_IP_STATE_ACTIVE)))
        LOG_EXIT_RET(VPP_ERR_STATE);

    stCmd.eCmd = VPP_IP_CMD_FLUSH;
    stCmd.flush.ePort = ePort;
    u32Ret = u32VppIpFrcMe_CmdPut(pstCb, &stCmd);
    LOGE_IF(u32Ret != VPP_OK, "u32VppIpFrcMe_CmdPut() failed, u32Ret=%u", u32Ret);

    LOG_EXIT_RET(u32Ret);
}

uint32_t u32VppIpFrcMe_Drain(void *ctx)
{
    LOG_ENTER();

    t_StVppIpFrcMeCb *pstCb;
    t_StVppIpCmd stCmd;
    uint32_t u32Ret = VPP_OK;

    VPP_RET_IF_NULL(ctx, VPP_ERR_PARAM);
    pstCb = FRC_ME_CB_GET(ctx);

    if (!VPP_IP_STATE_EQUAL(pstCb, VPP_IP_STATE_ACTIVE))
        LOG_EXIT_RET(VPP_ERR_STATE);

    stCmd.eCmd = VPP_IP_CMD_DRAIN;
    u32Ret = u32VppIpFrcMe_CmdPut(pstCb, &stCmd);
    LOGE_IF(u32Ret != VPP_OK, "u32VppIpFrcMe_CmdPut() failed, u32Ret=%u", u32Ret);

    LOG_EXIT_RET(u32Ret);
}

// Reconfig the ME module
uint32_t u32VppIpFrcMe_Reconfigure(void *ctx, struct vpp_port_param stInput,
                                   struct vpp_port_param stOutput)
{
    LOG_ENTER();

    uint32_t u32InQCnt, u32Ret = VPP_OK;
    t_StVppIpFrcMeCb *pstCb;
    t_StVppEvt stEvt;

    VPP_RET_IF_NULL(ctx, VPP_ERR_PARAM);
    pstCb = FRC_ME_CB_GET(ctx);

    if (!VPP_IP_STATE_EQUAL(pstCb, VPP_IP_STATE_ACTIVE))
        LOG_EXIT_RET(VPP_ERR_STATE);

    if (u32VppIpFrcMe_ValidatePortParam(stInput))
    {
        LOGE("u32VppIpFrcMe_ValidatePortParam(INPUT) failed");
        LOG_EXIT_RET(VPP_ERR_PARAM);
    }
    if (u32VppIpFrcMe_ValidatePortParam(stOutput))
    {
        LOGE("u32VppIpFrcMe_ValidatePortParam(OUTPUT) failed");
        LOG_EXIT_RET(VPP_ERR_PARAM);
    }

    // Validate that the port configuration is valid
    if (u32VppIpFrcMe_ValidateConfig(stInput, stOutput) != VPP_OK)
    {
        LOGE("u32VppIpFrcMe_ValidateConfig() failed");
        LOG_EXIT_RET(VPP_ERR_PARAM);
    }
    FRC_ME_LOCK(pstCb);

    u32InQCnt = u32VppBufPool_Cnt(&pstCb->stInput.stPendingQ);
    if (u32InQCnt || FRC_ME_TME_INPQ_INPROC(pstCb))
    {
        LOGE("Cannot Reconfig InpQ:%u InpTmeQ:%u", u32InQCnt, FRC_ME_TME_INPQ_INPROC(pstCb));
        u32Ret = VPP_ERR_STATE;
        goto ERROR_RET;
    }

    VPP_IP_PROF_START(&pstCb->stBase, FRC_ME_STAT_TME_REL_RES);
    u32Ret = u32VppIpFrcMe_TmeReleaseResources(pstCb);
    VPP_IP_PROF_STOP(&pstCb->stBase, FRC_ME_STAT_TME_REL_RES);
    if (u32Ret != VPP_OK)
    {
        LOGE("u32VppIpFrcMe_TmeReleaseResources() failed ret=%u", u32Ret);
        goto ERROR_RET;
    }

    u32Ret = u32VppIpFrcMe_FreeInternalEosBuf(pstCb);
    if (u32Ret != VPP_OK)
    {
        LOGE("u32VppIpFrcMe_FreeInternalEosBuf() failed ret=%u", u32Ret);
        goto ERROR_RET;
    }

    pstCb->stInput.stParam = stInput;
    pstCb->stOutput.stParam = stOutput;

    u32Ret = u32VppIpFrcMe_AllocInternalEosBuf(pstCb);
    if (u32Ret != VPP_OK)
    {
        LOGE("u32VppIpFrcMe_AllocInternalEosBuf() failed ret=%u", u32Ret);
        goto ERROR_RET;
    }

    // Configure TME
    VPP_IP_PROF_START(&pstCb->stBase, FRC_ME_STAT_TME_AQR_RES);
    u32Ret = u32VppIpFrcMe_TmeAcquireResources(pstCb);
    VPP_IP_PROF_STOP(&pstCb->stBase, FRC_ME_STAT_TME_AQR_RES);
    LOGE_IF(u32Ret, "u32VppIpFrcMe_TmeAcquireResources() failed! %u", u32Ret);

ERROR_RET:
    FRC_ME_UNLOCK(pstCb);
    LOG_EXIT_RET(u32Ret);

}
