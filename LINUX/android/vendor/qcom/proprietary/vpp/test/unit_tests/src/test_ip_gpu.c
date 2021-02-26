/*!
 * @file test_ip_gpu.c
 *
 * @cr
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.

 * @services    Implements tests for the GPU IP Block submodule
 */

#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#include "buf_pool.h"
#include "test_utils.h"

#include "dvpTest.h"
#include "dvpTest_tb.h"

#define VPP_LOG_TAG     VPP_LOG_UT_GPU_TAG
#define VPP_LOG_MODULE  VPP_LOG_UT_GPU
#include "vpp_dbg.h"
#include "vpp.h"
// #include "vpp_core.h"
// #include "vpp_ctx.h"
#include "vpp_reg.h"
#include "vpp_ip.h"
#include "vpp_ip_gpu.h"
#include "vpp_utils.h"


// #include "vpp_uc.h"

/************************************************************************
 * Local definitions
 ***********************************************************************/
#define INPUT_FLUSH_DONE        (1<<0)
#define OUTPUT_FLUSH_DONE       (1<<1)
#define DRAIN_DONE              (1<<2)

struct gpu_test_ctx {
    void *gpu_ctx; // context returned from gpu init
    uint32_t u32ExpExtraLen;
};

/************************************************************************
 * Local static variables
 ***********************************************************************/
static struct test_ctx tctx;
static struct gpu_test_ctx stCtxGpu;
/************************************************************************
 * Forward Declarations
 ************************************************************************/

/************************************************************************
 * Local Functions
 ***********************************************************************/
void test_gpu_input_buffer_done(void *pv, t_StVppBuf *pBuf)
{
    struct test_ctx *pCtx = (struct test_ctx *)pv;
    struct bufnode *pNode = pBuf->pBuf->cookie;

    DVP_ASSERT_TRUE(pNode->owner == BUF_OWNER_LIBRARY);
    pNode->owner = BUF_OWNER_CLIENT;

    put_buf(pCtx->buf_pool, pNode);

    pthread_cond_signal(&pCtx->cond);
}

void test_gpu_output_buffer_done(void *pv, t_StVppBuf *pBuf)
{
    struct test_ctx *pCtx = (struct test_ctx *)pv;
    struct gpu_test_ctx *pGpuCtx = (struct gpu_test_ctx *)pCtx->pPrivateCtx;
    struct bufnode *pNode = pBuf->pBuf->cookie;

    t_EVppBufType eType;

    DVP_ASSERT_TRUE(pNode->owner == BUF_OWNER_LIBRARY);
    pNode->owner = BUF_OWNER_CLIENT;

    eType = eVppBuf_GetFrameType(pBuf);
    DVP_ASSERT_EQUAL(eType, eVppBufType_Progressive);

    if (pNode->pIntBuf->stPixel.u32FilledLen)
    {
        dump_buf(pNode);
        DVP_ASSERT_EQUAL(pNode->pIntBuf->stExtra.u32FilledLen, pGpuCtx->u32ExpExtraLen);
        LOGI("extradata: expFillLen=%u, act_fill_len=%u\n", pGpuCtx->u32ExpExtraLen,
             pNode->pIntBuf->stExtra.u32FilledLen);
        validate_extradata_integrity(pNode);
    }

    put_buf(pCtx->buf_pool, pNode);

    pthread_cond_signal(&pCtx->cond);
}

void test_gpu_event(void *pv, t_StVppEvt stEvt)
{
    LOGI("%s() got event: %u", __func__, stEvt.eType);
    struct test_ctx *pCtx = (struct test_ctx *)pv;
    if (!pCtx)
    {
        LOGE("context was null");
        DVP_ASSERT_FAIL();
        return;
    }

    pthread_mutex_lock(&pCtx->mutex);

    if (stEvt.eType == VPP_EVT_FLUSH_DONE)
    {
        if (stEvt.flush.ePort == VPP_PORT_INPUT)
            pCtx->u32Flags |= INPUT_FLUSH_DONE;
        else if (stEvt.flush.ePort == VPP_PORT_OUTPUT)
            pCtx->u32Flags |= OUTPUT_FLUSH_DONE;
    }
    else if (stEvt.eType == VPP_EVT_DRAIN_DONE)
    {
        pCtx->u32Flags |= DRAIN_DONE;
    }

    pthread_mutex_unlock(&pCtx->mutex);

    pthread_cond_signal(&pCtx->cond);
}

/************************************************************************
 * Test Functions
 ***********************************************************************/


TEST_SUITE_INIT(IpGpuSuiteInit)
{
}

TEST_SUITE_TERM(IpGpuSuiteTerm)
{
}

TEST_SETUP(IpGpuTestInit)
{
    uint32_t u32Ret;

    tctx_common_init(&tctx);

    tctx.cb.input_buffer_done = test_gpu_input_buffer_done;
    tctx.cb.output_buffer_done = test_gpu_output_buffer_done;
    tctx.cb.event = test_gpu_event;
    tctx.cb.pv = &tctx;

    tctx.pPrivateCtx = &stCtxGpu;

    tctx_set_port_params(&tctx, 1920, 1080, VPP_COLOR_FORMAT_NV12_VENUS);

    stCtxGpu.gpu_ctx = vpVppIpGpu_Init(&tctx.stVppCtx, 0, tctx.cb);
    DVP_ASSERT_PTR_NNULL(stCtxGpu.gpu_ctx);

    u32Ret = u32VppIpGpu_SetParam(stCtxGpu.gpu_ctx, VPP_PORT_INPUT,
                                  tctx.port_param);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    u32Ret = u32VppIpGpu_SetParam(stCtxGpu.gpu_ctx, VPP_PORT_OUTPUT,
                                  tctx.port_param);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    u32Ret = u32VppIpGpu_Open(stCtxGpu.gpu_ctx);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    buf_params_init_default(&tctx.params, &tctx.port_param);
}

TEST_CLEANUP(IpGpuTestTerm)
{

    u32VppIpGpu_Flush(stCtxGpu.gpu_ctx, VPP_PORT_INPUT);
    u32VppIpGpu_Flush(stCtxGpu.gpu_ctx, VPP_PORT_OUTPUT);
    u32VppIpGpu_Close(stCtxGpu.gpu_ctx);
    vVppIpGpu_Term(stCtxGpu.gpu_ctx);

    tctx_common_destroy(&tctx);
}

TEST_SETUP(IpGpuNoInitTestInit)
{
    tctx_common_init(&tctx);

    tctx.cb.input_buffer_done = test_gpu_input_buffer_done;
    tctx.cb.output_buffer_done = test_gpu_output_buffer_done;
    tctx.cb.event = test_gpu_event;
    tctx.cb.pv = &tctx;

    tctx.pPrivateCtx = &stCtxGpu;

    tctx_set_port_params(&tctx, 1920, 1080, VPP_COLOR_FORMAT_NV12_VENUS);

    buf_params_init_default(&tctx.params, &tctx.port_param);
}

TEST_CLEANUP(IpGpuNoInitTestTerm)
{
    tctx_common_destroy(&tctx);
}

TEST(IpGpu_Testcase)
{
    uint32_t u32Ret;
    void *pCtx;
    uint32_t i;
    struct vpp_port_param port_param;
    t_StVppCallback cb;
    struct test_ctx stTestCtx;

    tctx_common_init(&stTestCtx);
    cb.input_buffer_done = test_gpu_input_buffer_done;
    cb.output_buffer_done = test_gpu_output_buffer_done;
    cb.event = test_gpu_event;
    cb.pv = &stTestCtx;
    pCtx = vpVppIpGpu_Init(&stTestCtx.stVppCtx, 0, cb);
    DVP_ASSERT_PTR_NNULL(pCtx);

    set_port_params(&port_param, 1920, 1080, VPP_COLOR_FORMAT_NV12_VENUS);

    u32Ret = u32VppIpGpu_SetParam(pCtx, VPP_PORT_INPUT, port_param);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    u32Ret = u32VppIpGpu_SetParam(pCtx, VPP_PORT_OUTPUT, port_param);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    u32Ret = u32VppIpGpu_Open(pCtx);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    for (i = 0; i < 10; i++)
    {
        u32Ret = u32VppIpGpu_QueueBuf(pCtx, VPP_PORT_INPUT, NULL);
        DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);

        u32Ret = u32VppIpGpu_QueueBuf(pCtx, VPP_PORT_OUTPUT, NULL);
        DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);
    }

    sleep(1);

    u32Ret = u32VppIpGpu_Flush(pCtx, VPP_PORT_INPUT);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    u32Ret = u32VppIpGpu_Flush(pCtx, VPP_PORT_OUTPUT);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    u32Ret = u32VppIpGpu_Close(pCtx);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    vVppIpGpu_Term(pCtx);

    tctx_common_destroy(&stTestCtx);
}

static void rotate_gpu_buffers()
{
    uint32_t u32Ret;
    uint32_t i;
    struct bufnode *pNode;
    uint32_t u32InBufCnt, u32OutBufCnt, u32BufTotal;

    u32InBufCnt = 5;
    u32OutBufCnt = 5;

    u32BufTotal = u32InBufCnt + u32OutBufCnt;

    u32Ret = init_buf_pool(&tctx.stVppCtx, &tctx.buf_pool, &tctx.params,
                           u32BufTotal, VPP_TRUE);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    // Queue to input pool
    for (i = 0; i < u32InBufCnt; i++)
    {
        pNode = get_buf(tctx.buf_pool);
        if (pNode)
        {
            fill_buf(pNode);

            pNode->owner = BUF_OWNER_LIBRARY;

            fill_extra_data(pNode, eVppBufType_Progressive, 0);
            stCtxGpu.u32ExpExtraLen = pNode->pIntBuf->stExtra.u32FilledLen;

            u32Ret = u32VppIpGpu_QueueBuf(stCtxGpu.gpu_ctx, VPP_PORT_INPUT,
                                          pNode->pIntBuf);
            DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
        }
        else
        {
            DVP_ASSERT_FAIL();
        }
    }

    for (i = 0; i < u32OutBufCnt; i++)
    {
        pNode = get_buf(tctx.buf_pool);
        if (pNode)
        {
            pNode->owner = BUF_OWNER_LIBRARY;

            DVP_ASSERT_FALSE(tctx.u32Flags & DRAIN_DONE);

            u32Ret = u32VppIpGpu_QueueBuf(stCtxGpu.gpu_ctx, VPP_PORT_OUTPUT,
                                          pNode->pIntBuf);
            DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
        }
        else
        {
            DVP_ASSERT_FAIL();
        }
    }
    pthread_mutex_lock(&tctx.mutex);

    LOGI("entering wait loop");
    while (tctx.buf_pool->u32ListSz != u32BufTotal)
        pthread_cond_wait(&tctx.cond, &tctx.mutex);

    LOGI("breaking out of wait loop");

    pthread_mutex_unlock(&tctx.mutex);

    LOGI("freeing buffer pools");
    free_buf_pool(tctx.buf_pool, VPP_TRUE);

}

/*!
 * Test that if we give the appropriate number of buffers to the GPU that we
 * get the proper number of buffers back on both ports. In this case, GPU only
 * has 1 to 1 algorithms, so if we give it an equal number on both ports, it
 * should return all of those buffers to us, assuming that we do not queue more
 * buffers to the port.
 */
TEST(IpGpu_BufferRotation)
{
    rotate_gpu_buffers();
}

/*!
 * Basic drain test case. Make sure that if we have never enqueued any buffers,
 * if we request a drain, we get back a drain done right away.
 */
TEST(IpGpu_DrainNoBuffers)
{
    uint32_t u32Ret;

    u32Ret = u32VppIpGpu_Drain(stCtxGpu.gpu_ctx);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    while (!(tctx.u32Flags & DRAIN_DONE))
        pthread_cond_wait(&tctx.cond, &tctx.mutex);
    pthread_mutex_unlock(&tctx.mutex);

    DVP_ASSERT_TRUE(tctx.u32Flags & DRAIN_DONE);
}

/*!
 *  Make sure that if we queue an equal number of input and output buffers, we
 *  get a drain done callback once all of the buffers have been returned.
 */
TEST(IpGpu_Drain)
{
    uint32_t u32Ret;
    uint32_t i;
    struct bufnode *pNode;

    uint32_t u32InBufCnt, u32OutBufCnt, u32BufTotal;

    u32InBufCnt = u32OutBufCnt = 5;
    u32BufTotal = u32InBufCnt + u32OutBufCnt;

    u32Ret = init_buf_pool(&tctx.stVppCtx, &tctx.buf_pool, &tctx.params,
                           u32BufTotal, VPP_TRUE);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    // Make sure that we queue input buffers before we queue output buffers
    // because we are requesting a notification for when all input buffers have
    // been drained.
    for (i = 0; i < u32InBufCnt; i++)
    {
        pNode = get_buf(tctx.buf_pool);
        if (pNode)
        {
            DVP_ASSERT_PTR_NNULL(pNode);

            fill_buf(pNode);
            fill_extra_data(pNode, eVppBufType_Progressive, 0);
            stCtxGpu.u32ExpExtraLen = pNode->pIntBuf->stExtra.u32FilledLen;

            pNode->owner = BUF_OWNER_LIBRARY;

            u32Ret = u32VppIpGpu_QueueBuf(stCtxGpu.gpu_ctx, VPP_PORT_INPUT, pNode->pIntBuf);
            DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
        }
        else
        {
            DVP_ASSERT_FAIL();
        }

    }

    u32Ret = u32VppIpGpu_Drain(stCtxGpu.gpu_ctx);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    for (i = 0; i < u32OutBufCnt; i++)
    {
        pNode = get_buf(tctx.buf_pool);
        if (pNode)
        {
            DVP_ASSERT_PTR_NNULL(pNode);

            pNode->owner = BUF_OWNER_LIBRARY;

            DVP_ASSERT_FALSE(tctx.u32Flags & DRAIN_DONE);

            u32Ret = u32VppIpGpu_QueueBuf(stCtxGpu.gpu_ctx, VPP_PORT_OUTPUT, pNode->pIntBuf);
            DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
        }
        else
        {
            DVP_ASSERT_FAIL();
        }
    }

    pthread_mutex_lock(&tctx.mutex);
    while (!(tctx.u32Flags & DRAIN_DONE))
        pthread_cond_wait(&tctx.cond, &tctx.mutex);
    pthread_mutex_unlock(&tctx.mutex);

    DVP_ASSERT_TRUE(tctx.u32Flags & DRAIN_DONE);
    DVP_ASSERT_EQUAL(tctx.buf_pool->u32ListSz, u32BufTotal);

    LOGI("freeing buffer pools");
    free_buf_pool(tctx.buf_pool, VPP_TRUE);
}

void queue_to_port(void *gpu_ctx, uint32_t u32Cnt, struct buf_pool *pool, enum vpp_port port)
{
    uint32_t i, u32Ret;
    struct bufnode *pNode;

    for (i = 0; i < u32Cnt; i++)
    {
        pNode = get_buf(pool);
        if (pNode)
        {
            pNode->owner = BUF_OWNER_LIBRARY;

            DVP_ASSERT_PTR_NNULL(pNode);
            pNode->pExtBuf->pixel.filled_len = u32VppUtils_GetPxBufferSize(&tctx.port_param);
            u32Ret = u32VppIpGpu_QueueBuf(gpu_ctx, port, pNode->pIntBuf);
            DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
        }
        else
        {
            DVP_ASSERT_FAIL();
        }
    }
}

void FlushOnTermSetup(void **ppvGpuCtx, struct test_ctx *pTestCtx, uint32_t bOpen)
{
    uint32_t u32Ret;

    *ppvGpuCtx = vpVppIpGpu_Init(&tctx.stVppCtx, 0, pTestCtx->cb);
    DVP_ASSERT_PTR_NNULL(*ppvGpuCtx);

    u32Ret = u32VppIpGpu_SetParam(*ppvGpuCtx, VPP_PORT_INPUT, pTestCtx->port_param);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    u32Ret = u32VppIpGpu_SetParam(*ppvGpuCtx, VPP_PORT_OUTPUT, pTestCtx->port_param);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    if (bOpen)
    {
        u32Ret = u32VppIpGpu_Open(*ppvGpuCtx);
        DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    }
}

TEST(IpGpuNoInit_FlushOnTerm)
{
    uint32_t u32Ret;
    uint32_t u32BufTotal = 5;

    u32Ret = init_buf_pool(&tctx.stVppCtx, &tctx.buf_pool, &tctx.params,
                           u32BufTotal, VPP_TRUE);

    FlushOnTermSetup(&stCtxGpu.gpu_ctx, &tctx, VPP_TRUE);
    queue_to_port(stCtxGpu.gpu_ctx, u32BufTotal, tctx.buf_pool, VPP_PORT_INPUT);
    DVP_ASSERT_EQUAL(0, tctx.buf_pool->u32ListSz);
    u32VppIpGpu_Close(stCtxGpu.gpu_ctx);
    vVppIpGpu_Term(stCtxGpu.gpu_ctx);
    DVP_ASSERT_EQUAL(tctx.buf_pool->u32ListSz, u32BufTotal);

    FlushOnTermSetup(&stCtxGpu.gpu_ctx, &tctx, VPP_TRUE);
    queue_to_port(stCtxGpu.gpu_ctx, u32BufTotal, tctx.buf_pool, VPP_PORT_OUTPUT);
    DVP_ASSERT_EQUAL(0, tctx.buf_pool->u32ListSz);
    u32VppIpGpu_Close(stCtxGpu.gpu_ctx);
    vVppIpGpu_Term(stCtxGpu.gpu_ctx);
    DVP_ASSERT_EQUAL(tctx.buf_pool->u32ListSz, u32BufTotal);

    FlushOnTermSetup(&stCtxGpu.gpu_ctx, &tctx, VPP_TRUE);
    queue_to_port(stCtxGpu.gpu_ctx, u32BufTotal / 2, tctx.buf_pool, VPP_PORT_INPUT);
    queue_to_port(stCtxGpu.gpu_ctx, u32BufTotal / 2, tctx.buf_pool, VPP_PORT_OUTPUT);
    DVP_ASSERT_TRUE(tctx.buf_pool->u32ListSz < u32BufTotal);
    vVppIpGpu_Term(stCtxGpu.gpu_ctx);
    DVP_ASSERT_EQUAL(tctx.buf_pool->u32ListSz, u32BufTotal);

    free_buf_pool(tctx.buf_pool, VPP_TRUE);
};

TEST(IpGpu_SwapInputOutput)
{
    uint32_t u32Ret;
    uint32_t u32InBufCnt, u32OutBufCnt, u32BufTotal;

    u32InBufCnt = 1;
    u32OutBufCnt = 1;

    u32BufTotal = u32InBufCnt + u32OutBufCnt;

    u32Ret = init_buf_pool(&tctx.stVppCtx, &tctx.buf_pool, &tctx.params,
                           u32BufTotal, VPP_TRUE);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    struct bufnode *pNodeIn, *pNodeOut;
    pNodeIn = get_buf(tctx.buf_pool);
    pNodeOut = get_buf(tctx.buf_pool);

    if (!pNodeIn || !pNodeOut)
    {
        DVP_ASSERT_FAIL();
        if (pNodeIn)
            put_buf(tctx.buf_pool, pNodeIn);

        if (pNodeOut)
            put_buf(tctx.buf_pool, pNodeOut);

        free_buf_pool(tctx.buf_pool, VPP_TRUE);
        return;
    }

    DVP_ASSERT_PTR_NNULL(pNodeIn);
    DVP_ASSERT_PTR_NNULL(pNodeOut);
    DVP_ASSERT_PTR_NEQUAL(pNodeIn, pNodeOut);

    fill_extra_data(pNodeIn, eVppBufType_Progressive, 0);
    stCtxGpu.u32ExpExtraLen = pNodeIn->pIntBuf->stExtra.u32FilledLen;

    // Frame 0
    fill_buf(pNodeIn);
    pNodeIn->owner = BUF_OWNER_LIBRARY;
    u32Ret = u32VppIpGpu_QueueBuf(stCtxGpu.gpu_ctx, VPP_PORT_INPUT, pNodeIn->pIntBuf);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    pNodeOut->owner = BUF_OWNER_LIBRARY;
    u32Ret = u32VppIpGpu_QueueBuf(stCtxGpu.gpu_ctx, VPP_PORT_OUTPUT, pNodeOut->pIntBuf);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    pthread_mutex_lock(&tctx.mutex);
    while (tctx.buf_pool->u32ListSz != u32BufTotal)
        pthread_cond_wait(&tctx.cond, &tctx.mutex);
    pthread_mutex_unlock(&tctx.mutex);

    struct bufnode *pNodeTmp = get_buf(tctx.buf_pool);

    if (pNodeTmp == pNodeIn)
    {
        pNodeOut = pNodeTmp;
        pNodeIn = get_buf(tctx.buf_pool);
    }
    else
    {
        pNodeIn = pNodeTmp;
        pNodeOut = get_buf(tctx.buf_pool);
    }

    if (!pNodeIn || !pNodeOut)
    {
        DVP_ASSERT_FAIL();
        if (pNodeIn)
            put_buf(tctx.buf_pool, pNodeIn);

        if (pNodeOut)
            put_buf(tctx.buf_pool, pNodeOut);

        free_buf_pool(tctx.buf_pool, VPP_TRUE);
        return;
    }

    // Read Frame 1 into output buffer and swap input with output
    pNodeIn->owner = BUF_OWNER_LIBRARY;
    pNodeOut->owner = BUF_OWNER_LIBRARY;

    fill_buf(pNodeOut);
    u32Ret = u32VppIpGpu_QueueBuf(stCtxGpu.gpu_ctx, VPP_PORT_INPUT, pNodeOut->pIntBuf);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    u32Ret = u32VppIpGpu_QueueBuf(stCtxGpu.gpu_ctx, VPP_PORT_OUTPUT, pNodeIn->pIntBuf);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    pthread_mutex_lock(&tctx.mutex);
    while (tctx.buf_pool->u32ListSz != u32BufTotal)
        pthread_cond_wait(&tctx.cond, &tctx.mutex);
    pthread_mutex_unlock(&tctx.mutex);

    free_buf_pool(tctx.buf_pool, VPP_TRUE);
}

static void SetCtrlTestSetup()
{
    uint32_t u32Ret;

    stCtxGpu.gpu_ctx = vpVppIpGpu_Init(&tctx.stVppCtx, 0, tctx.cb);
    DVP_ASSERT_PTR_NNULL(stCtxGpu.gpu_ctx);

    u32Ret = u32VppIpGpu_SetParam(stCtxGpu.gpu_ctx, VPP_PORT_INPUT,
                                  tctx.port_param);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    u32Ret = u32VppIpGpu_SetParam(stCtxGpu.gpu_ctx, VPP_PORT_OUTPUT,
                                  tctx.port_param);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);


}

static void SetCtrlTestTerm()
{
    vVppIpGpu_Term(stCtxGpu.gpu_ctx);
}


TEST(IpGpuNoInit_SetAuto)
{
    uint32_t u32Ret;

    struct hqv_control ctrl;
    ctrl.mode = HQV_MODE_AUTO;

    SetCtrlTestSetup();

    u32Ret = u32VppIpGpu_SetCtrl(stCtxGpu.gpu_ctx, ctrl);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    u32Ret = u32VppIpGpu_Open(stCtxGpu.gpu_ctx);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    // buffer exchange
    rotate_gpu_buffers();

    u32Ret = u32VppIpGpu_Close(stCtxGpu.gpu_ctx);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    SetCtrlTestTerm();
}

TEST(IpGpuNoInit_SetManual)
{
    uint32_t u32Ret;

    struct hqv_control ctrl;
    ctrl.mode = HQV_MODE_MANUAL;
    ctrl.ctrl_type = HQV_CONTROL_CADE;
    ctrl.cade.mode = HQV_MODE_MANUAL;
    ctrl.cade.cade_level = 100;
    ctrl.cade.contrast = 50;
    ctrl.cade.saturation = 50;

    SetCtrlTestSetup();

    u32Ret = u32VppIpGpu_SetCtrl(stCtxGpu.gpu_ctx, ctrl);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    u32Ret = u32VppIpGpu_Open(stCtxGpu.gpu_ctx);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    // buffer exchange
    rotate_gpu_buffers();

    u32Ret = u32VppIpGpu_Close(stCtxGpu.gpu_ctx);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    SetCtrlTestTerm();
}
/************************************************************************
 * Global Functions
 ***********************************************************************/
TEST_CASES IpGpuTests[] = {
    TEST_CASE(IpGpu_Testcase),
    TEST_CASE(IpGpu_BufferRotation),
    TEST_CASE(IpGpu_DrainNoBuffers),
    TEST_CASE(IpGpu_Drain),
    TEST_CASE(IpGpu_SwapInputOutput),
    TEST_CASE_NULL(),
};

TEST_SUITE(IpGpuSuite,
        "IpGpu Tests",
        IpGpuSuiteInit,
        IpGpuSuiteTerm,
        IpGpuTestInit,
        IpGpuTestTerm,
        IpGpuTests);

TEST_CASES IpGpuNoInitTests[] = {
    TEST_CASE(IpGpuNoInit_FlushOnTerm),
    TEST_CASE(IpGpuNoInit_SetAuto),
    TEST_CASE(IpGpuNoInit_SetManual),
    TEST_CASE_NULL(),
};

TEST_SUITE(IpGpuNoInitSuite,
           "IpGpu Tests",
           NULL, // Suite Init
           NULL, // Suite Term
           IpGpuNoInitTestInit,
           IpGpuNoInitTestTerm,
           IpGpuNoInitTests);
