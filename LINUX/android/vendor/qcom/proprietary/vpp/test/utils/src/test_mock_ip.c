/*!
 * @file test_mock_ip.c
 *
 * @cr
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.

 * @services
 */

#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <semaphore.h>

#define VPP_LOG_MODULE VPP_LOG_TEST_UTILS_MOCK
#define VPP_LOG_TAG VPP_LOG_TEST_UTILS_MOCK_TAG
#include "vpp_dbg.h"

#include "vpp.h"
#include "vpp_reg.h"
#include "vpp_queue.h"
#include "vpp_ip.h"
#include "test_mock_ip.h"
#include "vpp_utils.h"

/************************************************************************
 * Local definitions
 ***********************************************************************/
// #define MOCK_IP_COPY_IN_TO_OUT

#define MOCK_CMD_Q_SZ                       30
#define MOCK_PORT_BUF_Q_SZ                  VPP_INTERNAL_BUF_MAX

#define MOCK_ALGO_CADE                      (1 << 0)
#define MOCK_ALGO_DI                        (1 << 1)
#define MOCK_ALGO_TNR                       (1 << 2)
#define MOCK_ALGO_CNR                       (1 << 3)
#define MOCK_ALGO_AIE                       (1 << 4)
#define MOCK_ALGO_FRC                       (1 << 5)

#define MOCK_NON_DI_ALGO (MOCK_ALGO_CADE | MOCK_ALGO_TNR | MOCK_ALGO_CNR | \
                          MOCK_ALGO_AIE | MOCK_ALGO_FRC)


#define MOCK_CB_GET(ctx) (ctx ? (t_StVppIpMockCb *)ctx : NULL)

#define MOCK_RET_VOID_IF_NULL(ptr) \
    if (!ptr) { \
        LOGE(#ptr " was null"); \
        return; \
    }

#define MOCK_RET_IF_NULL(ptr) \
    if (!ptr) { \
        LOGE(#ptr " was null"); \
        return VPP_ERR_PARAM; \
    }

typedef struct {
    struct hqv_ctrl_cade cade;
    uint32_t u32EnableMask;
    uint32_t u32LoadMask;
    uint32_t u32ComputeMask;
} t_StVppIpMockCfg;

typedef struct {
    struct vpp_port_param stParam;
    t_StVppQueue stQ;
    t_StVppBuf *apBufNode[MOCK_PORT_BUF_Q_SZ];
} t_StVppMockPort;

typedef struct {
    t_StVppCallback stCb;

    uint32_t u32Flags;
    uint32_t u32FailFlags;

    sem_t sem;
    pthread_t thread;
    pthread_cond_t cond;
    pthread_mutex_t mutex;

    t_StVppMockPort stInput;
    t_StVppMockPort stOutput;

    t_StVppQueue stCmdQ;
    t_StVppIpCmd astCmdNode[MOCK_CMD_Q_SZ];

    uint32_t eState;

    t_StVppIpMockCfg stCfg;
    uint32_t u32InputMin;
    uint32_t u32OutputMin;
    uint32_t u32BypassCnt;
    uint32_t u32InterlacedCnt;
} t_StVppIpMockCb;

/************************************************************************
 * Local static variables
 ***********************************************************************/

/************************************************************************
 * Forward Declarations
 ************************************************************************/

/************************************************************************
 * Local Functions
 ***********************************************************************/

static void vVppIpMock_Compute(t_StVppIpMockCb *pstCb)
{
    uint32_t bLocked = VPP_FALSE;

    if (pstCb->stCfg.u32ComputeMask)
    {
        pthread_mutex_lock(&pstCb->mutex);
        bLocked = VPP_TRUE;
    }

    if (pstCb->stCfg.u32ComputeMask & MOCK_ALGO_CADE)
    {
        if (pstCb->stCfg.cade.mode == HQV_MODE_AUTO)
        {
        }
        else if (pstCb->stCfg.cade.mode == HQV_MODE_MANUAL)
        {
        }

        pstCb->stCfg.u32ComputeMask &= ~MOCK_ALGO_CADE;
        pstCb->stCfg.u32LoadMask |= MOCK_ALGO_CADE;
    }

    if (bLocked)
    {
        pthread_mutex_unlock(&pstCb->mutex);
    }
}

static void vVppIpMock_Load(t_StVppIpMockCb *pstCb)
{
    if (pstCb->stCfg.u32LoadMask & MOCK_ALGO_CADE)
    {
        LOGD("MOCK: u32LoadMask=0x%x", pstCb->stCfg.u32LoadMask);
        pstCb->stCfg.u32LoadMask &= ~MOCK_ALGO_CADE;
    }
}

static uint32_t u32VppIpMock_CmdGet(t_StVppIpMockCb *pstCb, t_StVppIpCmd *pstCmd)
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

static uint32_t u32VppIpMock_CmdPut(t_StVppIpMockCb *pstCb, t_StVppIpCmd stCmd)
{
    int32_t idx;
    uint32_t u32Ret = VPP_OK;

    pthread_mutex_lock(&pstCb->mutex);

    LOG_CMD("MOCK InsertCmd", stCmd.eCmd);
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

static uint32_t u32VppIpMock_PortBufPeek(t_StVppMockPort *pstPort,
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

static uint32_t u32VppIpMock_PortBufGet(t_StVppMockPort *pstPort,
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

static uint32_t u32VppIpMock_PortBufPut(t_StVppIpMockCb *pstCb,
                                       t_StVppMockPort *pstPort,
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

static void vVppIpMock_SignalWorkerStart(t_StVppIpMockCb *pstCb)
{
    pthread_mutex_lock(&pstCb->mutex);

    pstCb->u32Flags |= IP_WORKER_STARTED;

    pthread_cond_signal(&pstCb->cond);

    pthread_mutex_unlock(&pstCb->mutex);
}

static void vVppIpMock_WaitWorkerStart(t_StVppIpMockCb *pstCb)
{
    pthread_mutex_lock(&pstCb->mutex);

    while (!(pstCb->u32Flags & IP_WORKER_STARTED))
        pthread_cond_wait(&pstCb->cond, &pstCb->mutex);

    pthread_mutex_unlock(&pstCb->mutex);
}

static uint32_t u32ProcBufReqMet(t_StVppIpMockCb *pstCb)
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
        u32VppIpMock_PortBufPeek(&pstCb->stInput, &pstBuf);
        if (!pstBuf)
        {
            LOGE("u32VppIpMock_PortBufPeek returned null buffer");
            return VPP_FALSE;
        }

        if (VPP_FLAG_IS_SET(pstBuf->u32InternalFlags, VPP_BUF_FLAG_BYPASS))
        {
            LOGI("awake because of bypass");
            return VPP_TRUE;
        }

        if (!VPP_FLAG_IS_SET(pstCb->stCfg.u32EnableMask, MOCK_ALGO_DI) &&
            pstBuf->eBufType != eVppBufType_Progressive)
        {
            LOGI("awake because di not enabled, buf is interlaced");
            return VPP_TRUE;
        }

        if (VPP_FLAG_IS_SET(pstCb->stCfg.u32EnableMask, MOCK_ALGO_DI) &&
            !VPP_FLAG_IS_SET(pstCb->stCfg.u32EnableMask, MOCK_NON_DI_ALGO) &&
            pstBuf->eBufType == eVppBufType_Progressive)
        {
            LOGI("awake because only di enabled, buf is progressive");
            return VPP_TRUE;
        }

        if (pstBuf->stPixel.u32FilledLen == 0)
        {
            LOGI("awake because of empty input buffer");
            return VPP_TRUE;
        }

    }

    if (u32InQSz >= pstCb->u32InputMin && u32OutQSz >= pstCb->u32OutputMin)
    {
        LOGI("min buf req met");
        return VPP_TRUE;
    }

    return VPP_FALSE;
}

static uint32_t u32WorkerThreadShouldSleep(t_StVppIpMockCb *pstCb)
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

static uint32_t u32VppIpMock_ValidateConfig(t_StVppIpMockCb *pstCb)
{
    struct vpp_port_param *pInput, *pOutput;

    pInput = &pstCb->stInput.stParam;
    pOutput = &pstCb->stOutput.stParam;

    if (pInput->height != pOutput->height)
    {
        LOGE("validation failed: height, input: %u, output: %u",
             pInput->height, pOutput->height);
        return VPP_ERR;
    }
    if (pInput->width != pOutput->width)
    {

        LOGE("validation failed: width, input: %u, output: %u",
             pInput->width, pOutput->width);
        return VPP_ERR;
    }
    if (pInput->fmt != pOutput->fmt)
    {
        LOGE("validation failed: fmt, input: %u, output: %u",
             pInput->fmt, pOutput->fmt);
        return VPP_ERR;
    }

    return VPP_OK;
}

static uint32_t u32VppIpMock_ProcCmdInit(t_StVppIpMockCb *pstCb)
{
    VPP_UNUSED(pstCb);
    return VPP_OK;
}

static uint32_t u32VppIpMock_ProcCmdExit(t_StVppIpMockCb *pstCb)
{
    uint32_t u32InQSz, u32OutQSz;

    pthread_mutex_lock(&pstCb->mutex);
    u32InQSz = vpp_queue_count(&pstCb->stInput.stQ);
    u32OutQSz = vpp_queue_count(&pstCb->stOutput.stQ);
    pthread_mutex_unlock(&pstCb->mutex);

    if (u32InQSz || u32OutQSz)
        LOGD("processed close, u32InQSz=%u, u32OutQSz=%u", u32InQSz, u32OutQSz);

    return VPP_OK;
}

static uint32_t u32VppIpMock_FlushPort(t_StVppIpMockCb *pstCb, enum vpp_port ePort)
{
    t_StVppBuf *pBuf;
    t_StVppMockPort *pstPort;

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

    while (u32VppIpMock_PortBufGet(pstPort, &pBuf) == VPP_OK)
    {
        VPP_FLAG_SET(pBuf->u32InternalFlags, VPP_BUF_FLAG_FLUSHED);
        pBuf->stPixel.u32FilledLen = 0;
        u32VppIp_CbBufDone(&pstCb->stCb, ePort, pBuf);
    }

    return VPP_OK;
}

static uint32_t u32VppIpMock_ProcCmdFlush(t_StVppIpMockCb *pstCb,
                                         t_StVppIpCmd *pstCmd)
{
    uint32_t u32;
    t_StVppEvt stEvt;

    // Flush Port
    u32 = u32VppIpMock_FlushPort(pstCb, pstCmd->flush.ePort);

    if (u32 == VPP_OK)
    {
        stEvt.eType = VPP_EVT_FLUSH_DONE;
        stEvt.flush.ePort = pstCmd->flush.ePort;
        u32VppIpCbEvent(&pstCb->stCb, stEvt);
    }

    return u32;
}

static uint32_t u32VppIpMock_ProcCmdDrain(t_StVppIpMockCb *pstCb)
{
    t_StVppEvt stEvt;

    pthread_mutex_lock(&pstCb->mutex);

    if (!vpp_queue_count(&pstCb->stInput.stQ))
    {
        // No more input buffers in the queue, drain has already been
        // completed.
        stEvt.eType = VPP_EVT_DRAIN_DONE;
        u32VppIpCbEvent(&pstCb->stCb, stEvt);
    }
    else
    {
        pstCb->u32Flags |= IP_DRAIN_PENDING;
    }

    pthread_mutex_unlock(&pstCb->mutex);

    return VPP_OK;
}

static uint32_t u32VppIpMock_ProcessBuffer(t_StVppIpMockCb *pstCb)
{
    uint32_t u32BufSz, u32AvailSz, u32OutMin;
    uint32_t i;
    struct vpp_mem_buffer *pstMemBuf;
    t_StVppBuf *pstBufIn = NULL, *pstBufPeek = NULL;

    t_StVppBuf *apstBufOut[2];
    memset(apstBufOut, 0, sizeof(apstBufOut));

    LOG_ENTER();

    pthread_mutex_lock(&pstCb->mutex);

    vVppIpMock_Load(pstCb);

    u32OutMin = pstCb->u32OutputMin;

    u32VppIpMock_PortBufPeek(&pstCb->stInput, &pstBufPeek);
    if (!pstBufPeek)
    {
        // Should never get here since predicate function should ensure that
        // we only get here if there are enough buffers for us to process.
        pthread_mutex_unlock(&pstCb->mutex);
        return VPP_ERR;
    }

    if (u32OutMin == 0 ||
        VPP_FLAG_IS_SET(pstBufPeek->u32InternalFlags, VPP_BUF_FLAG_BYPASS) ||

        // DI disabled but buffer is interlaced
        (!VPP_FLAG_IS_SET(pstCb->stCfg.u32EnableMask, MOCK_ALGO_DI) &&
         pstBufPeek->eBufType != eVppBufType_Progressive) ||

        // Only DI enabled but buffer is progressive
        (VPP_FLAG_IS_SET(pstCb->stCfg.u32EnableMask, MOCK_ALGO_DI) &&
         !VPP_FLAG_IS_SET(pstCb->stCfg.u32EnableMask, MOCK_NON_DI_ALGO) &&
         pstBufPeek->eBufType == eVppBufType_Progressive) ||

        // empty buffer, needs to be bypassed
        (pstBufPeek->stPixel.u32FilledLen == 0))
    {
        u32VppIpMock_PortBufGet(&pstCb->stInput, &pstBufIn);

        if (!pstBufIn)
        {
            LOGE("unable to get input buffer from port");
            pthread_mutex_unlock(&pstCb->mutex);
            return VPP_ERR;
        }

        if (VPP_FLAG_IS_SET(pstBufIn->u32InternalFlags, VPP_BUF_FLAG_BYPASS))
            pstCb->u32BypassCnt -= 1;

        if (pstBufIn->eBufType != eVppBufType_Progressive)
            pstCb->u32InterlacedCnt -= 1;
    }
    else
    {
        u32VppIpMock_PortBufGet(&pstCb->stInput, &pstBufIn);
        if (!pstBufIn)
        {
            LOGE("unable to get input buffer from port");
            pthread_mutex_unlock(&pstCb->mutex);
            return VPP_ERR;
        }

        for (i = 0; i < pstCb->u32OutputMin; i++)
        {
            u32VppIpMock_PortBufGet(&pstCb->stOutput, &apstBufOut[i]);
            if (!apstBufOut[i])
            {
                LOGE("unable to get output buffer %u from port", i);
                break;
            }
        }

        if (i != pstCb->u32OutputMin)
        {
            for (; i; i--)
            {
                apstBufOut[i]->stPixel.u32FilledLen = 0;
                u32VppIp_CbBufDone(&pstCb->stCb, VPP_PORT_OUTPUT, apstBufOut[i]);
            }
            u32VppIp_CbBufDone(&pstCb->stCb, VPP_PORT_OUTPUT, pstBufIn);
        }
    }

    pthread_mutex_unlock(&pstCb->mutex);

    uint32_t bOk = VPP_TRUE;
    for (i = 0; i < pstCb->u32OutputMin; i++)
    {
        if (!apstBufOut[i])
        {
            bOk = VPP_FALSE;
            LOGI("required buffer at index: %u is null, possibly bypass", i);
            break;
        }
    }

    if (pstBufIn && bOk)
    {
        u32BufSz = u32VppUtils_GetPxBufferSize(&pstCb->stInput.stParam);
        for (i = 0; i < pstCb->u32OutputMin; i++)
        {
            pstMemBuf = &apstBufOut[i]->pBuf->pixel;
            u32AvailSz = apstBufOut[i]->stPixel.u32ValidLen;
            if (u32AvailSz < u32BufSz)
            {
                LOGE("required buffer size exceeds available, bypassing this buffer");
                LOGE("req_sz=%u, buf:{fd=%d, off=%d, alloc_len=%u, filled_len=%u}",
                     u32BufSz, pstMemBuf->fd, pstMemBuf->offset,
                     pstMemBuf->alloc_len, pstMemBuf->filled_len);

                bOk = VPP_FALSE;
            }
        }
        if (bOk != VPP_TRUE)
        {
            // Send input buffer to output, return output buffer
            u32VppIp_CbBufDone(&pstCb->stCb, VPP_PORT_OUTPUT, pstBufIn);
            if (pstBufIn->eBufType != eVppBufType_Progressive)
                pstCb->u32InterlacedCnt -= 1;

            for (i = 0; i < pstCb->u32OutputMin; i++)
            {
                apstBufOut[i]->stPixel.u32FilledLen = 0;
                u32VppIp_CbBufDone(&pstCb->stCb, VPP_PORT_OUTPUT, apstBufOut[i]);
            }

            LOG_EXIT_RET(VPP_ERR);
        }
        for (i = 0; i < pstCb->u32OutputMin; i++)
        {
#ifdef MOCK_IP_COPY_IN_TO_OUT
            memcpy(apstBufOut[i]->stPixel.pv, pstBufIn->stPixel.pv, u32BufSz);
#endif
            // Copy from input to output:
            apstBufOut[i]->pBuf->timestamp = pstBufIn->pBuf->timestamp + i;
            apstBufOut[i]->pBuf->cookie_in_to_out = pstBufIn->pBuf->cookie_in_to_out;
            apstBufOut[i]->pBuf->flags = pstBufIn->pBuf->flags;
            apstBufOut[i]->stPixel.u32FilledLen = u32BufSz;

            // Only propagate EOS flag on the last frame generated
            if (i + 1 < pstCb->u32OutputMin &&
                VPP_FLAG_IS_SET(apstBufOut[i]->pBuf->flags, VPP_BUFFER_FLAG_EOS))
            {
                VPP_FLAG_CLR(apstBufOut[i]->pBuf->flags, VPP_BUFFER_FLAG_EOS);
            }

            u32VppBuf_CopyExtradata(pstBufIn, apstBufOut[i], eVppBufType_Progressive,
                                    VPP_EXTERNAL_EXTRADATA_TYPE);
            VPP_FLAG_CLR(apstBufOut[i]->u32InternalFlags, VPP_BUF_FLAG_FLUSHED);
        }

        // Return the buffers
        if (VPP_FLAG_IS_SET(pstCb->stCfg.u32EnableMask, MOCK_ALGO_FRC))
        {
            for (i = 0; i < pstCb->u32OutputMin; i++)
            {
                apstBufOut[i]->eBufType = eVppBufType_Progressive;
                VPP_FLAG_CLR(apstBufOut[i]->pBuf->flags, VPP_BUFFER_FLAG_EOS);
                u32VppIp_CbBufDone(&pstCb->stCb, VPP_PORT_OUTPUT, apstBufOut[i]);
            }
            u32VppIp_CbBufDone(&pstCb->stCb, VPP_PORT_OUTPUT, pstBufIn);
        }
        else
        {
            u32VppIp_CbBufDone(&pstCb->stCb, VPP_PORT_INPUT, pstBufIn);
            if (pstBufIn->eBufType != eVppBufType_Progressive)
                pstCb->u32InterlacedCnt -= 1;

            for (i = 0; i < pstCb->u32OutputMin; i++)
            {
                apstBufOut[i]->eBufType = eVppBufType_Progressive;
                u32VppIp_CbBufDone(&pstCb->stCb, VPP_PORT_OUTPUT, apstBufOut[i]);
            }
        }
    }
    else if (pstBufIn && !bOk)
    {
        u32VppIp_CbBufDone(&pstCb->stCb, VPP_PORT_OUTPUT, pstBufIn);
    }
    else
    {
        // Error - should never get into here
        LOG_EXIT_RET(VPP_ERR);
    }

    LOG_EXIT_RET(VPP_OK);
}

static void vVppIpMock_HandlePendingDrain(t_StVppIpMockCb *pstCb)
{
    t_StVppEvt stEvt;

    pthread_mutex_lock(&pstCb->mutex);

    if (pstCb->u32Flags & IP_DRAIN_PENDING &&
        !vpp_queue_count(&pstCb->stInput.stQ))
    {
        pstCb->u32Flags &= ~IP_DRAIN_PENDING;
        stEvt.eType = VPP_EVT_DRAIN_DONE;
        u32VppIpCbEvent(&pstCb->stCb, stEvt);
    }

    pthread_mutex_unlock(&pstCb->mutex);
}

static void *vpVppIpMock_Worker(void *pv)
{
    LOGD("%s started", __func__);

    t_StVppIpMockCb *pstCb = (t_StVppIpMockCb *)pv;

    // Initialize the hardware
    u32VppIpMock_ProcCmdInit(pstCb);

    // Signal back to main thread that we've launched and are ready to go
    vVppIpMock_SignalWorkerStart(pstCb);

    while (1)
    {
        pthread_mutex_lock(&pstCb->mutex);
        while (u32WorkerThreadShouldSleep(pstCb))
            pthread_cond_wait(&pstCb->cond, &pstCb->mutex);

        uint32_t u32Ret;
        t_StVppIpCmd stCmd;
        u32Ret = u32VppIpMock_CmdGet(pstCb, &stCmd);
        if (u32Ret == VPP_OK)
        {
            pthread_mutex_unlock(&pstCb->mutex);

            // Process the command
            LOG_CMD("ProcessCmd", stCmd.eCmd);

            if (stCmd.eCmd == VPP_IP_CMD_THREAD_EXIT)
            {
                u32VppIpMock_ProcCmdExit(pstCb);
                break;
            }

            else if (stCmd.eCmd == VPP_IP_CMD_FLUSH)
                u32VppIpMock_ProcCmdFlush(pstCb, &stCmd);

            else if (stCmd.eCmd == VPP_IP_CMD_DRAIN)
                u32VppIpMock_ProcCmdDrain(pstCb);

            else
                LOGE("unknown command in queue");

            continue;
        }

        if (pstCb->eState == VPP_IP_STATE_ACTIVE)
        {
            pthread_mutex_unlock(&pstCb->mutex);
            u32VppIpMock_ProcessBuffer(pstCb);
            vVppIpMock_HandlePendingDrain(pstCb);
            continue;
        }

        LOGE("%s woke up, but did no work", __func__);
        pthread_mutex_unlock(&pstCb->mutex);
    }

    LOGD("%s exited", __func__);

    return NULL;
}

/************************************************************************
 * Global Functions
 ***********************************************************************/


static uint32_t u32GlobalFailFlags;

void vVppIpMock_SetFailPoint(void *ctx, uint32_t u32FailFlags)
{
    t_StVppIpMockCb *pstCb;
    LOGD("u32FailFlags=%u", u32FailFlags);


    if (VPP_FLAG_IS_SET(u32FailFlags, MOCK_FAIL_INIT))
        VPP_FLAG_SET(u32GlobalFailFlags, MOCK_FAIL_INIT);
    else
        VPP_FLAG_CLR(u32GlobalFailFlags, MOCK_FAIL_INIT);

    if (ctx)
    {
        pstCb = MOCK_CB_GET(ctx);
        pstCb->u32FailFlags = u32FailFlags;
    }
}

void * vpVppIpMock_Init(t_StVppCtx *pstCtx, uint32_t u32Flags, t_StVppCallback cbs)
{
    LOG_ENTER();

    VPP_UNUSED(pstCtx);

    int rc;
    t_StVppIpMockCb *pstCb;

    LOGD("u32GlobalFailFlags=0x%x", u32GlobalFailFlags);
    if (VPP_FLAG_IS_SET(u32GlobalFailFlags, MOCK_FAIL_INIT))
        return NULL;

    if (u32Flags & VPP_SESSION_SECURE)
    {
        LOGE("Secure session requested. Not supported.");
        return NULL;
    }

    pstCb = calloc(sizeof(t_StVppIpMockCb), 1);
    if (!pstCb)
    {
        LOGE("calloc failed for mock context");
        goto ERROR_MALLOC;
    }

    pstCb->stCb = cbs;

    vpp_queue_init(&pstCb->stInput.stQ, MOCK_PORT_BUF_Q_SZ);
    vpp_queue_init(&pstCb->stOutput.stQ, MOCK_PORT_BUF_Q_SZ);
    vpp_queue_init(&pstCb->stCmdQ, MOCK_CMD_Q_SZ);

    rc = sem_init(&pstCb->sem, 0, 0);
    if (rc)
    {
        LOGE("unable to initialize mock mutex");
        goto ERROR_SEM_INIT;
    }

    rc = pthread_mutex_init(&pstCb->mutex, NULL);
    if (rc)
    {
        LOGE("unable to initialize mock mutex");
        goto ERROR_MUTEX_INIT;
    }

    rc = pthread_cond_init(&pstCb->cond, NULL);
    if (rc)
    {
        LOGE("unable to init condition variable");
        goto ERROR_COND_INIT;
    }

    rc = pthread_create(&pstCb->thread, NULL, vpVppIpMock_Worker, pstCb);
    if (rc)
    {
        LOGE("unable to spawn mock worker thread");
        goto ERROR_THREAD_CREATE;
    }

    // Set the default input to output requirement
    pstCb->u32InputMin = 1;
    pstCb->u32OutputMin = 1;

    // Set default post processing parameters
    pstCb->stCfg.u32LoadMask |= MOCK_ALGO_CADE;

    // Wait for the thread to launch before returning
    vVppIpMock_WaitWorkerStart(pstCb);

    VPP_IP_STATE_SET(pstCb, VPP_IP_STATE_INITED);

    return pstCb;

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
    free(pstCb);

ERROR_MALLOC:
    return NULL;
}

void vVppIpMock_Term(void *ctx)
{
    int rc;
    t_StVppIpMockCb *pstCb;
    t_StVppIpCmd stCmd;

    LOG_ENTER();

    MOCK_RET_VOID_IF_NULL(ctx);
    pstCb = MOCK_CB_GET(ctx);

    stCmd.eCmd = VPP_IP_CMD_THREAD_EXIT;
    u32VppIpMock_CmdPut(pstCb, stCmd);

    rc = pthread_join(pstCb->thread, NULL);
    if (rc)
        LOGE("pthread_join failed: %d --> %s", rc, strerror(rc));

    u32VppIpMock_FlushPort(pstCb, VPP_PORT_INPUT);
    u32VppIpMock_FlushPort(pstCb, VPP_PORT_OUTPUT);

    rc = pthread_mutex_destroy(&pstCb->mutex);
    if (rc)
        LOGE("pthread_mutex_destroy failed: %d --> %s", rc, strerror(rc));

    rc = sem_destroy(&pstCb->sem);
    if (rc)
        LOGE("sem_destroy failed: %d --> %s", rc, strerror(rc));

    vpp_queue_term(&pstCb->stCmdQ);
    vpp_queue_term(&pstCb->stInput.stQ);
    vpp_queue_term(&pstCb->stOutput.stQ);

    free(pstCb);
}

uint32_t u32VppIpMock_Open(void *ctx)
{
    LOGI("%s\n", __func__);

    t_StVppIpMockCb *pstCb;

    MOCK_RET_IF_NULL(ctx);
    pstCb = MOCK_CB_GET(ctx);

    if (!VPP_IP_STATE_EQUAL(pstCb, VPP_IP_STATE_INITED))
        return VPP_ERR_STATE;

    // Validate that the port configuration is valid
    if (u32VppIpMock_ValidateConfig(pstCb) != VPP_OK)
        return VPP_ERR_PARAM;

    VPP_IP_STATE_SET(pstCb, VPP_IP_STATE_ACTIVE);

    return VPP_OK;
}

uint32_t u32VppIpMock_Close(void *ctx)
{
    LOGI("%s\n", __func__);

    t_StVppIpMockCb *pstCb;

    MOCK_RET_IF_NULL(ctx);
    pstCb = MOCK_CB_GET(ctx);

    if (!VPP_IP_STATE_EQUAL(pstCb, VPP_IP_STATE_ACTIVE))
        return VPP_ERR_STATE;

    VPP_IP_STATE_SET(pstCb, VPP_IP_STATE_INITED);

    return VPP_OK;
}


uint32_t u32VppIpMock_SetParam(void *ctx, enum vpp_port port,
                              struct vpp_port_param param)
{
    LOGI("%s(), port=%u", __func__, port);

    uint32_t u32Ret = VPP_OK;
    t_StVppIpMockCb *pstCb;

    MOCK_RET_IF_NULL(ctx);
    pstCb = MOCK_CB_GET(ctx);

    if (!VPP_IP_STATE_EQUAL(pstCb, VPP_IP_STATE_INITED))
        return VPP_ERR_STATE;

    pthread_mutex_lock(&pstCb->mutex);

    if (port == VPP_PORT_INPUT)
        pstCb->stInput.stParam = param;
    else if (port == VPP_PORT_OUTPUT)
        pstCb->stOutput.stParam = param;
    else
        u32Ret = VPP_ERR_PARAM;

    LOG_PARAM_PTR(I, &pstCb->stInput.stParam, &pstCb->stOutput.stParam);

    pthread_mutex_unlock(&pstCb->mutex);

    return u32Ret;
}

uint32_t u32VppIpMock_SetCtrl(void *ctx, struct hqv_control ctrl)
{
    LOGI("%s\n", __func__);

    uint32_t u32Ret = VPP_OK;
    t_StVppIpMockCb *pstCb;

    MOCK_RET_IF_NULL(ctx);
    pstCb = MOCK_CB_GET(ctx);

    // Set
    if (ctrl.ctrl_type == HQV_CONTROL_CUST)
    {
        uint32_t i;
        uint32_t *pu32;
        uint32_t u32Len;

        u32Len = HQV_CUSTOM_PAYLOAD_SZ / sizeof(uint32_t);
        u32Len = ctrl.custom.len / sizeof(uint32_t);

        pu32 = (uint32_t *)ctrl.custom.data;

        LOGI("custom control, len=%u", ctrl.custom.len);
        for (i = 0; i < u32Len; i++)
        {
            LOGI("[%u]: 0x%x", i, pu32[i]);
        }
    }
    else if (ctrl.mode == HQV_MODE_OFF)
    {
        pstCb->stCfg.cade.mode = HQV_MODE_OFF;
        VPP_FLAG_CLR(pstCb->stCfg.u32EnableMask, MOCK_ALGO_CADE);
    }
    else if (ctrl.mode == HQV_MODE_AUTO)
    {
        pstCb->stCfg.cade.mode = HQV_MODE_AUTO;

        VPP_FLAG_SET(pstCb->stCfg.u32EnableMask, MOCK_ALGO_CADE);
        VPP_FLAG_SET(pstCb->stCfg.u32ComputeMask, MOCK_ALGO_CADE);
    }
    else if (ctrl.mode == HQV_MODE_MANUAL)
    {
        if (ctrl.ctrl_type == HQV_CONTROL_CADE)
        {
            pstCb->stCfg.cade = ctrl.cade;

            if (pstCb->stCfg.cade.mode == HQV_MODE_AUTO ||
                pstCb->stCfg.cade.mode == HQV_MODE_MANUAL)
            {
                VPP_FLAG_SET(pstCb->stCfg.u32EnableMask, MOCK_ALGO_CADE);
                VPP_FLAG_SET(pstCb->stCfg.u32ComputeMask, MOCK_ALGO_CADE);
            }
            else
            {
                LOGE("stCfg.cade.mode = %u", pstCb->stCfg.cade.mode);
                VPP_FLAG_CLR(pstCb->stCfg.u32EnableMask, MOCK_ALGO_CADE);
            }
        }
        else if (ctrl.ctrl_type == HQV_CONTROL_DI)
        {
            VPP_FLAG_SET(pstCb->stCfg.u32EnableMask, MOCK_ALGO_DI);
            VPP_FLAG_SET(pstCb->stCfg.u32ComputeMask, MOCK_ALGO_DI);
        }
        else if (ctrl.ctrl_type == HQV_CONTROL_CNR)
        {
            VPP_FLAG_SET(pstCb->stCfg.u32EnableMask, MOCK_ALGO_CNR);
            VPP_FLAG_SET(pstCb->stCfg.u32ComputeMask, MOCK_ALGO_CNR);
        }
        else if (ctrl.ctrl_type == HQV_CONTROL_TNR)
        {
            VPP_FLAG_SET(pstCb->stCfg.u32EnableMask, MOCK_ALGO_TNR);
            VPP_FLAG_SET(pstCb->stCfg.u32ComputeMask, MOCK_ALGO_TNR);
        }
        else if (ctrl.ctrl_type == HQV_CONTROL_AIE)
        {
            VPP_FLAG_SET(pstCb->stCfg.u32EnableMask, MOCK_ALGO_AIE);
            VPP_FLAG_SET(pstCb->stCfg.u32ComputeMask, MOCK_ALGO_AIE);
        }
        else if (ctrl.ctrl_type == HQV_CONTROL_FRC)
        {
            VPP_FLAG_SET(pstCb->stCfg.u32EnableMask, MOCK_ALGO_FRC);
            VPP_FLAG_SET(pstCb->stCfg.u32ComputeMask, MOCK_ALGO_FRC);
        }
        else
        {
            LOGE("Unsupported control: ctrl_type=%u", ctrl.ctrl_type);
            u32Ret = VPP_ERR_INVALID_CFG;
        }
    }
    if (u32Ret == VPP_OK)
        vVppIpMock_Compute(pstCb);

    pthread_mutex_lock(&pstCb->mutex);
    if (pstCb->stCfg.u32EnableMask)
    {
        if (pstCb->stCfg.u32EnableMask & MOCK_ALGO_DI)
        {
            pstCb->u32InputMin = 1;
            pstCb->u32OutputMin = 2;
        }
        else
        {
            pstCb->u32InputMin = 1;
            pstCb->u32OutputMin = 1;
        }
    }
    else if (!pstCb->stCfg.u32EnableMask)
    {
        pstCb->u32InputMin = 1;
        pstCb->u32OutputMin = 0;
    }
    LOGI("u32InputMin=%u, u32OutputMin=%u", pstCb->u32InputMin, pstCb->u32OutputMin);
    pthread_mutex_unlock(&pstCb->mutex);

    return u32Ret;
}

uint32_t u32VppIpMock_GetBufferRequirements(void *ctx,
                                            t_StVppIpBufReq *pstInputBufReq,
                                            t_StVppIpBufReq *pstOutputBufReq)
{
    LOGI("%s\n", __func__);

    t_StVppIpMockCb *pstCb;
    uint32_t u32PxSz;

    MOCK_RET_IF_NULL(ctx);
    pstCb = MOCK_CB_GET(ctx);

    u32PxSz = u32VppUtils_GetPxBufferSize(&pstCb->stInput.stParam);

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

uint32_t u32VppIpMock_QueueBuf(void *ctx, enum vpp_port ePort,
                               t_StVppBuf *pBuf)
{
    LOGI("%s() port=%u\n", __func__, (uint32_t)ePort);

    uint32_t u32Ret = VPP_OK;
    t_StVppIpMockCb *pstCb;

    MOCK_RET_IF_NULL(ctx);
    pstCb = MOCK_CB_GET(ctx);

    pthread_mutex_lock(&pstCb->mutex);

    if (ePort == VPP_PORT_INPUT)
    {
        if (VPP_FLAG_IS_SET(pBuf->u32InternalFlags, VPP_BUF_FLAG_BYPASS))
            pstCb->u32BypassCnt += 1;

        if (pBuf->eBufType != eVppBufType_Progressive)
            pstCb->u32InterlacedCnt += 1;

        u32Ret = u32VppIpMock_PortBufPut(pstCb, &pstCb->stInput, pBuf);
        if (u32Ret != VPP_OK)
        {
            if (VPP_FLAG_IS_SET(pBuf->u32InternalFlags, VPP_BUF_FLAG_BYPASS))
                pstCb->u32BypassCnt -= 1;

            if (pBuf->eBufType != eVppBufType_Progressive)
                pstCb->u32InterlacedCnt -= 1;
        }
    }
    else if (ePort == VPP_PORT_OUTPUT)
    {
        u32Ret = u32VppIpMock_PortBufPut(pstCb, &pstCb->stOutput, pBuf);
    }

    pthread_mutex_unlock(&pstCb->mutex);

    return u32Ret;
}

uint32_t u32VppIpMock_Flush(void *ctx, enum vpp_port ePort)
{
    LOGI("%s\n", __func__);

    uint32_t u32Ret = VPP_OK;
    t_StVppIpMockCb *pstCb;
    t_StVppIpCmd stCmd;

    MOCK_RET_IF_NULL(ctx);
    pstCb = MOCK_CB_GET(ctx);

    stCmd.eCmd = VPP_IP_CMD_FLUSH;
    stCmd.flush.ePort = ePort;
    u32VppIpMock_CmdPut(pstCb, stCmd);

    return u32Ret;
}

uint32_t u32VppIpMock_Drain(void *ctx)
{
    LOGI("%s\n", __func__);

    uint32_t u32Ret = VPP_OK;
    t_StVppIpMockCb *pstCb;
    t_StVppIpCmd stCmd;

    MOCK_RET_IF_NULL(ctx);
    pstCb = MOCK_CB_GET(ctx);

    stCmd.eCmd = VPP_IP_CMD_DRAIN;
    u32VppIpMock_CmdPut(pstCb, stCmd);

    return u32Ret;
}
