/*!
 * @file test_ip_me.c
 *
 * @cr
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * @services    Implements tests for the me (Motion Estimation) submodule
 */

#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <sched.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <malloc.h>
#include <time.h>

#include "buf_pool.h"
#include "test_utils.h"
#include "dvpTest.h"
#include "dvpTest_tb.h"

#define VPP_LOG_TAG     VPP_LOG_UT_ME_TAG
#define VPP_LOG_MODULE  VPP_LOG_UT_ME
#include "vpp_dbg.h"
#include "vpp.h"
#include "vpp_reg.h"
#include "vpp_ip.h"
#include "vpp_ip_frc_me.h"
#include "vpp_utils.h"

/************************************************************************
 * Local definitions
 ***********************************************************************/

#define INP_DIR_PATH            "/data/test/input/"
#define OUT_DIR_PATH            "/data/test/output/"
#define INPUT_FLUSH_DONE        (1 << 0)
#define OUTPUT_FLUSH_DONE       (1 << 1)
#define DRAIN_DONE              (1 << 2)
#define OUTPUT_EOS              (1 << 3)
#define TIME_DELAY              (33 * 1000)

struct me_test_ctx
{
    void *pvME;          // context returned from me init
    t_StVppCtx stCtx;
};

/************************************************************************
 * Local static variables
 ***********************************************************************/
static struct test_ctx stTctx;
static struct me_test_ctx stCtxMe;

/************************************************************************
 * Forward Declarations
 ************************************************************************/

/************************************************************************
 * Local Functions
 ***********************************************************************/
void test_me_input_buffer_done(void *pv, t_StVppBuf *pBuf)
{
    LOGI("%s() with buffer=0x%x", __func__, (unsigned int)pBuf);

    struct test_ctx *pstCtx = (struct test_ctx *)pv;
    struct bufnode *pstNode = pBuf->pBuf->cookie;

    pthread_mutex_lock(&pstCtx->mutex);  //Mutex to protect the pools
    DVP_ASSERT_TRUE(pstNode->owner == BUF_OWNER_LIBRARY);
    pstNode->owner = BUF_OWNER_CLIENT;

    put_buf(pstCtx->buf_pool, pstNode);
    pstCtx->u32InRxCnt++;
    LOGE("INPUT_BUF_DONE RxCnt:%u", pstCtx->u32InRxCnt);
    pthread_mutex_unlock(&pstCtx->mutex);
    pthread_cond_signal(&pstCtx->cond);
}

void test_me_output_buffer_done(void *pv, t_StVppBuf *pBuf)
{
    LOGI("%s() with buffer=0x%x", __func__, (unsigned int)pBuf);

    struct test_ctx *pstCtx = (struct test_ctx *)pv;
    struct bufnode *pstNode = pBuf->pBuf->cookie;
    t_EVppBufType eType;

    pthread_mutex_lock(&pstCtx->mutex);  //Mutex to protect the pools
    DVP_ASSERT_TRUE(pstNode->owner == BUF_OWNER_LIBRARY);
    pstNode->owner = BUF_OWNER_CLIENT;

    eType = eVppBuf_GetFrameType(pBuf);
    DVP_ASSERT_EQUAL(eType, eVppBufType_Progressive);

    pstCtx->u32OutRxCnt++;

    if (pstNode->pIntBuf->pBuf->flags & VPP_BUFFER_FLAG_EOS)
    {
       pstCtx->u32Flags |= OUTPUT_EOS;
       pstCtx->u32InEosRxCnt++;
    }

    if (pstNode->pIntBuf->stPixel.u32FilledLen)
        dump_buf(pstNode);
    else
        pstCtx->u32OutFlushCnt++;

    put_buf(pstCtx->buf_pool_ext, pstNode);

    LOGE("OUTPUT_BUF_DONE RxCnt:%u FillLen:%u", pstCtx->u32OutRxCnt, pBuf->stPixel.u32FilledLen);
    pthread_mutex_unlock(&pstCtx->mutex);
    pthread_cond_signal(&pstCtx->cond);
}

void test_me_event(void *pv, t_StVppEvt stEvt)
{
    LOGI("%s() got event: %u", __func__, stEvt.eType);
    struct test_ctx *pstCtx = (struct test_ctx *)pv;

    pthread_mutex_lock(&pstCtx->mutex);

    if (stEvt.eType == VPP_EVT_FLUSH_DONE)
    {
        LOGE("ME_EVENT: VPP_EVT_FLUSH_DONE %d", stEvt.flush.ePort);
        if (stEvt.flush.ePort == VPP_PORT_INPUT)
            pstCtx->u32Flags |= INPUT_FLUSH_DONE;
        else if (stEvt.flush.ePort == VPP_PORT_OUTPUT)
            pstCtx->u32Flags |= OUTPUT_FLUSH_DONE;
    }
    else if (stEvt.eType == VPP_EVT_DRAIN_DONE)
    {
        pstCtx->u32Flags |= DRAIN_DONE;
        LOGE("ME_EVENT: DRAIN DONE");
    }

    pthread_mutex_unlock(&pstCtx->mutex);
    pthread_cond_signal(&pstCtx->cond);
}

/************************************************************************
 * Test Functions
 ***********************************************************************/

TEST_SUITE_INIT(IpMeSuiteInit)
{
}

TEST_SUITE_TERM(IpMeSuiteTerm)
{
}

TEST_SETUP(IpMeTestInit)
{
    uint32_t u32Ret = VPP_OK;
    memset((void*)&stTctx, 0, sizeof(stTctx));
    memset((void*)&stCtxMe, 0, sizeof(stCtxMe));

    u32Ret = u32VppIon_Init(&stCtxMe.stCtx);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    LOGE_IF(u32Ret, "Error: u32VppIon_Init failed! ret=%u", u32Ret);
    u32Ret = tctx_common_init((struct test_ctx*)&stTctx);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
}

TEST_CLEANUP(IpMeTestTerm)
{
    uint32_t u32Ret = VPP_OK;
    u32Ret = tctx_common_destroy(&stTctx);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    u32Ret = u32VppIon_Term(&stCtxMe.stCtx);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    LOGE_IF(u32Ret, "Error: u32VppIon_Term failed! ret=%u", u32Ret);
}

static uint32_t u32InitMETest(uint32_t u32Width, uint32_t u32Height, const char *pcInFile,
                              const char *pcOutFile)
{
    t_StVppIpBufReq stInputBufReq, stOutputBufReq;
    uint32_t u32Ret = VPP_OK;

    memset(&stTctx, 0, sizeof(stTctx));
    stTctx.cb.input_buffer_done = test_me_input_buffer_done;
    stTctx.cb.output_buffer_done = test_me_output_buffer_done;
    stTctx.cb.event = test_me_event;
    stTctx.cb.pv = &stTctx;

    stTctx.pPrivateCtx = &stCtxMe;
    stTctx.params.u32Width = u32Width;
    stTctx.params.u32Height = u32Height;
    stTctx.params.eBufferType = eVppBufType_Progressive;

    tctx_set_port_params(&stTctx, u32Width, u32Height, VPP_COLOR_FORMAT_UBWC_NV12);

    // Input file params
    stTctx.params.eInputFileFormat = FILE_FORMAT_NV12_UBWC;
    stTctx.params.eInputFileType = FILE_TYPE_MULTI_FRAMES;
    stTctx.params.eInputBufFmt = VPP_COLOR_FORMAT_UBWC_NV12;
    strlcpy((char*)stTctx.params.cInputPath, (char*)INP_DIR_PATH, MAX_FILE_SEG_LEN);
    strlcpy((char*)stTctx.params.cInputName, pcInFile, MAX_FILE_SEG_LEN);

    // Output file params
    stTctx.params.eOutputFileFormat = FILE_FORMAT_NV12_UBWC;
    stTctx.params.eOutputFileType = FILE_TYPE_MULTI_FRAMES;
    stTctx.params.eOutputBufFmt = VPP_COLOR_FORMAT_UBWC_NV12;
    strlcpy((char*)stTctx.params.cOutputPath, (char*)OUT_DIR_PATH, MAX_FILE_SEG_LEN);
    strlcpy((char*)stTctx.params.cOutputName, pcOutFile, MAX_FILE_SEG_LEN);

    stCtxMe.pvME = vpVppIpFrcMe_Init(&stCtxMe.stCtx, 0, stTctx.cb);
    DVP_ASSERT_PTR_NNULL(stCtxMe.pvME);

    u32Ret = u32VppIpFrcMe_SetParam(stCtxMe.pvME, VPP_PORT_INPUT, stTctx.port_param);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    u32Ret = u32VppIpFrcMe_SetParam(stCtxMe.pvME, VPP_PORT_OUTPUT, stTctx.port_param);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    u32Ret = u32VppIpFrcMe_Open(stCtxMe.pvME);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    u32Ret = u32VppIpFrcMe_GetBufferRequirements(stCtxMe.pvME, &stInputBufReq, &stOutputBufReq);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    u32Ret = init_buf_pool(&stTctx.stVppCtx, &stTctx.buf_pool, &stTctx.params,
                           stInputBufReq.u32MinCnt, VPP_TRUE);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    u32Ret = init_buf_pool(&stTctx.stVppCtx, &stTctx.buf_pool_ext, &stTctx.params,
                           stOutputBufReq.u32MinCnt, VPP_FALSE);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    LOGE("Init Stage: IpBufs:%u  OpBuf:%u", stInputBufReq.u32MinCnt, stOutputBufReq.u32MinCnt);
    LOGE("buf_pool_size=%u, buf_pool_ext_size=%u", stTctx.buf_pool->u32ListSz,
                                                   stTctx.buf_pool_ext->u32ListSz);

    return VPP_OK;
}

static uint32_t u32TermMETest(void)
{
    uint32_t u32Ret = VPP_OK;

    LOGI("Before Flush buf_pool_size=%d, buf_pool_ext_size=%d", stTctx.buf_pool->u32ListSz,
                                                                stTctx.buf_pool_ext->u32ListSz);

    u32Ret = u32VppIpFrcMe_Flush(stCtxMe.pvME, VPP_PORT_INPUT);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppIpFrcMe_Flush(stCtxMe.pvME, VPP_PORT_OUTPUT);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    while (!((stTctx.u32Flags & INPUT_FLUSH_DONE) && (stTctx.u32Flags & OUTPUT_FLUSH_DONE)))
        sched_yield();

    LOGI("After Flush buf_pool_size=%d, buf_pool_ext_size=%d", stTctx.buf_pool->u32ListSz,
                                                               stTctx.buf_pool_ext->u32ListSz);
    u32Ret = u32VppIpFrcMe_Close(stCtxMe.pvME);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    LOGI("After Close buf_pool_size=%d, buf_pool_ext_size=%d", stTctx.buf_pool->u32ListSz,
                                                               stTctx.buf_pool_ext->u32ListSz);
    vVppIpFrcMe_Term(stCtxMe.pvME);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    LOGI("After Term buf_pool_size=%d, buf_pool_ext_size=%d", stTctx.buf_pool->u32ListSz,
                                                              stTctx.buf_pool_ext->u32ListSz);

    free_buf_pool(stTctx.buf_pool_ext, VPP_FALSE);
    free_buf_pool(stTctx.buf_pool, VPP_TRUE);

    return VPP_OK;
}

static uint32_t u32QueueInputBuf(uint32_t u32FrameNum, uint32_t u32Flags)
{
    struct bufnode *pstNode;
    uint32_t u32Ret = VPP_OK;

    pstNode = NULL;
    pthread_mutex_lock(&stTctx.mutex);
    pstNode = get_buf(stTctx.buf_pool);
    pthread_mutex_unlock(&stTctx.mutex);
    if (pstNode == NULL)
    {
        DVP_ASSERT_FAIL();
        return VPP_ERR;
    }

    fill_buf(pstNode);
    pstNode->owner = BUF_OWNER_LIBRARY;

    pstNode->pIntBuf->pBuf->timestamp = u32FrameNum * TIME_DELAY;
    pstNode->pIntBuf->pBuf->flags = 0;
    if (u32Flags & VPP_BUFFER_FLAG_EOS)
        pstNode->pIntBuf->pBuf->flags |= u32Flags;
    if (u32Flags & VPP_BUF_FLAG_BYPASS)
        pstNode->pIntBuf->u32InternalFlags |= u32Flags;

    u32Ret = u32VppIpFrcMe_QueueBuf(stCtxMe.pvME, VPP_PORT_INPUT, pstNode->pIntBuf);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    LOGI("Queue VPP_PORT_INPUT FrNum:%u Fd:%u Flags:%u", (u32FrameNum + 1),
                                                         pstNode->pIntBuf->stPixel.fd,
                                                         pstNode->pIntBuf->pBuf->flags);

    return VPP_OK;
}

static uint32_t u32QueueOutputBuf(uint32_t u32FrameNum)
{
    struct bufnode *pstNode;
    uint32_t u32Ret = VPP_OK;

    pstNode = NULL;
    pthread_mutex_lock(&stTctx.mutex);
    pstNode = get_buf(stTctx.buf_pool_ext);
    pthread_mutex_unlock(&stTctx.mutex);
    if (pstNode == NULL)
    {
        DVP_ASSERT_FAIL();
        return VPP_ERR;
    }

    pstNode->owner = BUF_OWNER_LIBRARY;
    pstNode->pIntBuf->pBuf->flags = 0;

    u32Ret = u32VppIpFrcMe_QueueBuf(stCtxMe.pvME, VPP_PORT_OUTPUT, pstNode->pIntBuf);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    LOGI("Queue VPP_PORT_OUTPUT FrNum:%u Fd:%u", (u32FrameNum + 1), pstNode->pIntBuf->stPixel.fd);

    return VPP_OK;
}

static uint32_t u32RunPerfTest(uint32_t u32Width, uint32_t u32Height, uint32_t u32FramesToRun,
                               const char *pcIn, const char *pcOut)
{
    struct timeval tv_start;
    struct timeval tv_end;
    uint32_t u32Ret = VPP_OK;
    uint32_t u32InFramesToRun, u32InFramesQueued = 0, u32OutFramesQueued = 0;

    u32InFramesToRun = u32FramesToRun;

    u32Ret = u32InitMETest(u32Width, u32Height, pcIn, pcOut);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    gettimeofday(&tv_start, NULL);
    while ((u32InFramesQueued < u32InFramesToRun) || (stTctx.u32InRxCnt < u32InFramesToRun))
    {
        // Queue output buf
        if ((stTctx.buf_pool_ext->u32ListSz) && (stTctx.u32InRxCnt < u32InFramesToRun))
        {
            u32Ret = u32QueueOutputBuf(u32OutFramesQueued);
            DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

            u32OutFramesQueued++;
        }

        // Queue input buf
        if ((stTctx.buf_pool->u32ListSz) && (u32InFramesQueued < u32InFramesToRun))
        {
            uint32_t u32Flags = 0;
            if (u32InFramesQueued == (u32InFramesToRun - 1))
                u32Flags |= VPP_BUFFER_FLAG_EOS;

            u32Ret = u32QueueInputBuf(u32InFramesQueued, u32Flags);
            DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

            u32InFramesQueued++;
        }

        if (!(stTctx.buf_pool_ext->u32ListSz && stTctx.buf_pool->u32ListSz))
           sched_yield();
    }
    gettimeofday(&tv_end, NULL);
    uint32_t u32Diff = u32VppUtils_GetTvDiff(&tv_end, &tv_start);
    LOGI("Frames run = %u, time taken = %u us", stTctx.u32InRxCnt, u32Diff);

    u32Ret = u32TermMETest();
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    return VPP_OK;
}

TEST(ME_Performance)
{
    uint32_t u32Ret = VPP_OK;

    u32Ret = u32RunPerfTest(1920, 1080, 10, "ivy_wall_1920x1080_nv12_UBWC.yuv",
                            "ivy_wall_1920x1080_nv12_UBWC.bin");
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    u32Ret = u32RunPerfTest(1280, 720, 10, "Slomo_Pitching_nv12_UBWC_1280x720.yuv",
                            "Slomo_Pitching_nv12_UBWC_1280x720.bin");
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
}

TEST(ME_Drain)
{
    uint32_t u32Ret = VPP_OK;
    uint32_t u32DrainStage = VPP_FALSE;
    uint32_t u32InFramesToRun, u32InFramesQueued = 0, u32OutFramesQueued = 0;

    u32InFramesToRun = 20;

    u32Ret = u32InitMETest(1920, 1080, "ivy_wall_1920x1080_nv12_UBWC.yuv",
                           "ivy_wall_1920x1080_nv12_UBWC.bin");
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    while ((u32InFramesQueued < u32InFramesToRun) || (stTctx.u32InRxCnt < u32InFramesToRun))
    {
        // Queue output buf
        if ((stTctx.buf_pool_ext->u32ListSz) && (stTctx.u32InRxCnt < u32InFramesToRun))
        {
            u32Ret = u32QueueOutputBuf(u32OutFramesQueued);
            DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

            u32OutFramesQueued++;
        }

        // Queue input buf
        if (stTctx.buf_pool->u32ListSz && (u32InFramesQueued < u32InFramesToRun)
            && (u32DrainStage == VPP_FALSE))
        {
            uint32_t u32Flags = 0;
            if (u32InFramesQueued == (u32InFramesToRun - 1))
                u32Flags |= VPP_BUFFER_FLAG_EOS;

            u32Ret = u32QueueInputBuf(u32InFramesQueued, u32Flags);
            DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

            u32InFramesQueued++;
        }

        if (u32DrainStage == VPP_TRUE)
        {
            if (stTctx.u32Flags & DRAIN_DONE)
            {
                stTctx.u32Flags &= ~DRAIN_DONE;
                u32DrainStage = VPP_FALSE;
                LOGI("After Drain buf_pool_size=%d, buf_pool_ext_size=%d",
                     stTctx.buf_pool->u32ListSz, stTctx.buf_pool_ext->u32ListSz);
            }
        }
        else if (u32InFramesQueued == 5)
        {
            LOGE("Queue VPP_PORT_INPUT Drain Buffer");
            u32VppIpFrcMe_Drain(stCtxMe.pvME);
            u32DrainStage = VPP_TRUE;
        }

        if (!(stTctx.buf_pool_ext->u32ListSz && stTctx.buf_pool->u32ListSz))
           sched_yield();
    }
    u32Ret = u32TermMETest();
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
}

TEST(ME_NoEOS)
{
    uint32_t u32Ret = VPP_OK;
    uint32_t u32InFramesToRun, u32InFramesQueued = 0, u32OutFramesQueued = 0;

    u32InFramesToRun = 20;

    u32Ret = u32InitMETest(1920, 1080, "ivy_wall_1920x1080_nv12_UBWC.yuv",
                           "ivy_wall_1920x1080_nv12_UBWC.bin");
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    while (u32InFramesQueued < u32InFramesToRun)
    {
        // Queue output buf
        if (stTctx.buf_pool_ext->u32ListSz && (stTctx.u32InRxCnt < u32InFramesToRun))
        {
            u32Ret = u32QueueOutputBuf(u32OutFramesQueued);
            DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

            u32OutFramesQueued++;
        }

        // Queue input buf
        if (stTctx.buf_pool->u32ListSz && (u32InFramesQueued < u32InFramesToRun))
        {
            u32Ret = u32QueueInputBuf(u32InFramesQueued, 0);
            DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

            u32InFramesQueued++;
        }

        if (!(stTctx.buf_pool_ext->u32ListSz && (stTctx.buf_pool->u32ListSz)))
           sched_yield();
    }
    u32Ret = u32TermMETest();
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
}

TEST(ME_Bypass)
{
    uint32_t u32Ret = VPP_OK;
    uint32_t u32InFramesToRun, u32InFramesQueued = 0, u32OutFramesQueued = 0;

    u32InFramesToRun = 20;

    u32Ret = u32InitMETest(1920, 1080, "ivy_wall_1920x1080_nv12_UBWC.yuv",
                           "ivy_wall_1920x1080_nv12_UBWC.bin");
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    while ((u32InFramesQueued < u32InFramesToRun) || (stTctx.u32InRxCnt < u32InFramesToRun))
    {
        // Queue output buf
        if ((stTctx.buf_pool_ext->u32ListSz) && (stTctx.u32InRxCnt < u32InFramesToRun))
        {
            u32Ret = u32QueueOutputBuf(u32OutFramesQueued);
            DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

            u32OutFramesQueued++;
        }

        // Queue input buf
        if (stTctx.buf_pool->u32ListSz && (u32InFramesQueued < u32InFramesToRun))
        {
            uint32_t u32Flags = 0;
            if ((u32InFramesQueued == 0) || (u32InFramesQueued == 4))
                u32Flags |= VPP_BUF_FLAG_BYPASS;

            if (u32InFramesQueued == (u32InFramesToRun-1))
                u32Flags |= VPP_BUFFER_FLAG_EOS;

            u32Ret = u32QueueInputBuf(u32InFramesQueued, u32Flags);
            DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

            u32InFramesQueued++;
        }

        if (!(stTctx.buf_pool_ext->u32ListSz && stTctx.buf_pool->u32ListSz))
           sched_yield();
    }
    u32Ret = u32TermMETest();
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
}

TEST(ME_BufferCnt)
{
    uint32_t u32Ret = VPP_OK;
    uint32_t u32InFramesToRun, u32InFramesQueued = 0, u32OutFramesQueued = 0;

    u32InFramesToRun = 20;

    u32Ret = u32InitMETest(1920, 1080, "ivy_wall_1920x1080_nv12_UBWC.yuv",
                           "ivy_wall_1920x1080_nv12_UBWC.bin");
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    // Queue 20 input and 20 output buffers
    while (u32InFramesQueued < u32InFramesToRun)
    {
        // Queue output buf
        if (stTctx.buf_pool_ext->u32ListSz)
        {
            u32Ret = u32QueueOutputBuf(u32OutFramesQueued);
            DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

            u32OutFramesQueued++;
        }

        // Queue input buf
        if (stTctx.buf_pool->u32ListSz)
        {
            uint32_t u32Flags = 0;
            if (u32InFramesQueued == (u32InFramesToRun - 1))
                u32Flags |= VPP_BUFFER_FLAG_EOS;

            u32Ret = u32QueueInputBuf(u32InFramesQueued, u32Flags);
            DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

            u32InFramesQueued++;
        }

        if (!(stTctx.buf_pool_ext->u32ListSz && (stTctx.buf_pool->u32ListSz)))
           sched_yield();
    }

    // Wait for all the input buffers to be returned
    while (stTctx.u32InRxCnt < u32InFramesToRun)
    {
        if (stTctx.buf_pool_ext->u32ListSz)
        {
            u32Ret = u32QueueOutputBuf(u32OutFramesQueued);
            DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

            u32OutFramesQueued++;
        }
        sched_yield();
    }
    LOGI("ME_BufferCnt 1: IpRxed:%u OpRxed:%u", stTctx.u32InRxCnt, stTctx.u32OutRxCnt);

    // Wait for the output EOS to be released from TME
    while (!(stTctx.u32Flags & OUTPUT_EOS))
        sched_yield();
    LOGI("ME_BufferCnt 2: IpRxed:%u OpRxed:%u", stTctx.u32InRxCnt, stTctx.u32OutRxCnt);

    DVP_ASSERT_EQUAL(stTctx.u32InRxCnt, u32InFramesToRun);
    DVP_ASSERT_EQUAL((stTctx.u32OutRxCnt - stTctx.u32OutFlushCnt), (u32InFramesToRun -1));

    u32Ret = u32TermMETest();
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
}

TEST(ME_EOS)
{
    uint32_t u32Ret = VPP_OK;
    uint32_t u32InFramesToRun, u32InFramesQueued = 0, u32OutFramesQueued = 0;

    u32InFramesToRun = 20;

    u32Ret = u32InitMETest(1920, 1080, "ivy_wall_1920x1080_nv12_UBWC.yuv",
                           "ivy_wall_1920x1080_nv12_UBWC.bin");
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    while (stTctx.u32InRxCnt < u32InFramesToRun)
    {
        // Queue output buf
        if (stTctx.buf_pool_ext->u32ListSz && (stTctx.u32InRxCnt < u32InFramesToRun))
        {
            u32Ret = u32QueueOutputBuf(u32OutFramesQueued);
            DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

            u32OutFramesQueued++;
        }

        // Queue input buf
        if (stTctx.buf_pool->u32ListSz && (u32InFramesQueued < u32InFramesToRun))
        {
            uint32_t u32Flags = 0;
            if ((u32InFramesQueued == 0) || (u32InFramesQueued == 2) ||
                (u32InFramesQueued == 3) || (u32InFramesQueued == 6) ||
                (u32InFramesQueued == 8) || (u32InFramesQueued == 9) ||
                (u32InFramesQueued == (u32InFramesToRun - 1)))
                u32Flags |= VPP_BUFFER_FLAG_EOS;

            u32Ret = u32QueueInputBuf(u32InFramesQueued, u32Flags);
            DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

            u32InFramesQueued++;
        }

        if (!(stTctx.buf_pool_ext->u32ListSz && (stTctx.buf_pool->u32ListSz)))
           sched_yield();
    }
    u32Ret = u32TermMETest();
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
}

TEST(ME_TestApi)
{
    LOG_ENTER();

    t_StVppIpBufReq stInputBufReq, stOutputBufReq;
    uint32_t u32Ret = VPP_OK;
    uint32_t u32Width = 1920, u32Height = 1080;
    struct hqv_control stHqvCtrl;
    struct vpp_port_param stInParam, stOutParam;

    // Sending NULL context
    stCtxMe.pvME = NULL;
    u32Ret = u32VppIpFrcMe_SetParam(stCtxMe.pvME, VPP_PORT_INPUT, stTctx.port_param);
    DVP_ASSERT_EQUAL(u32Ret, VPP_ERR_PARAM);

    // Sending NULL context
    stHqvCtrl.frc.mode = HQV_FRC_MODE_SLOMO;
    stCtxMe.pvME = NULL;
    u32Ret = u32VppIpFrcMe_SetCtrl(stCtxMe.pvME, stHqvCtrl);
    DVP_ASSERT_EQUAL(u32Ret, VPP_ERR_PARAM);

    // Sending NULL context
    stCtxMe.pvME = NULL;
    u32Ret = u32VppIpFrcMe_Open(stCtxMe.pvME);
    DVP_ASSERT_EQUAL(u32Ret, VPP_ERR_PARAM);

    // Sending NULL context
    stCtxMe.pvME = NULL;
    u32Ret = u32VppIpFrcMe_GetBufferRequirements(stCtxMe.pvME, &stInputBufReq, &stOutputBufReq);
    DVP_ASSERT_EQUAL(u32Ret, VPP_ERR_PARAM);
    // Sending valid context but NULL InputBufReq
    stCtxMe.pvME = (void *)0x12345678;
    u32Ret = u32VppIpFrcMe_GetBufferRequirements(stCtxMe.pvME, NULL, &stOutputBufReq);
    DVP_ASSERT_EQUAL(u32Ret, VPP_ERR_PARAM);
    // Sending valid context but NULL OutputBufReq
    stCtxMe.pvME = (void *)0x12345678;
    u32Ret = u32VppIpFrcMe_GetBufferRequirements(stCtxMe.pvME, &stInputBufReq, NULL);
    DVP_ASSERT_EQUAL(u32Ret, VPP_ERR_PARAM);

    // Sending NULL context
    stCtxMe.pvME = NULL;
    u32Ret = u32VppIpFrcMe_QueueBuf(stCtxMe.pvME, VPP_PORT_INPUT, (t_StVppBuf *)0x111111);
    DVP_ASSERT_EQUAL(u32Ret, VPP_ERR_PARAM);
    // Sending NULL Buffer
    stCtxMe.pvME = (void *)0x12345678;
    u32Ret = u32VppIpFrcMe_QueueBuf(stCtxMe.pvME, VPP_PORT_INPUT, NULL);
    DVP_ASSERT_EQUAL(u32Ret, VPP_ERR_PARAM);
    // Sending incorrect PORT
    stCtxMe.pvME = (void *)0x12345678;
    u32Ret = u32VppIpFrcMe_QueueBuf(stCtxMe.pvME, VPP_PORT_MAX, NULL);
    DVP_ASSERT_EQUAL(u32Ret, VPP_ERR_PARAM);
    u32Ret = u32VppIpFrcMe_QueueBuf(stCtxMe.pvME, 0x999, NULL);
    DVP_ASSERT_EQUAL(u32Ret, VPP_ERR_PARAM);

    // Sending NULL context
    stCtxMe.pvME = NULL;
    u32Ret = u32VppIpFrcMe_Flush(stCtxMe.pvME, VPP_PORT_INPUT);
    DVP_ASSERT_EQUAL(u32Ret, VPP_ERR_PARAM);
    // Sending incorrect port
    stCtxMe.pvME = (void *)0x12345678;
    u32Ret = u32VppIpFrcMe_Flush(stCtxMe.pvME, VPP_PORT_MAX);
    DVP_ASSERT_EQUAL(u32Ret, VPP_ERR_PARAM);
    u32Ret = u32VppIpFrcMe_Flush(stCtxMe.pvME, 0x888);
    DVP_ASSERT_EQUAL(u32Ret, VPP_ERR_PARAM);

    // Sending NULL context
    stCtxMe.pvME = NULL;
    u32Ret = u32VppIpFrcMe_Drain(stCtxMe.pvME);
    DVP_ASSERT_EQUAL(u32Ret, VPP_ERR_PARAM);

    // Sending NULL context
    stCtxMe.pvME = NULL;
    u32Ret = u32VppIpFrcMe_Close(stCtxMe.pvME);
    DVP_ASSERT_EQUAL(u32Ret, VPP_ERR_PARAM);

    // Init ME with the valid params
    memset(&stTctx, 0, sizeof(stTctx));
    stTctx.cb.input_buffer_done = test_me_input_buffer_done;
    stTctx.cb.output_buffer_done = test_me_output_buffer_done;
    stTctx.cb.event = test_me_event;
    stTctx.cb.pv = &stTctx;

    stTctx.pPrivateCtx = &stCtxMe;
    stTctx.params.u32Width = u32Width;
    stTctx.params.u32Height = u32Height;
    stTctx.params.eBufferType = eVppBufType_Progressive;

    tctx_set_port_params(&stTctx, u32Width, u32Height, VPP_COLOR_FORMAT_UBWC_NV12);

    // Input file params
    stTctx.params.eInputFileFormat = FILE_FORMAT_NV12_UBWC;
    stTctx.params.eInputFileType = FILE_TYPE_MULTI_FRAMES;
    stTctx.params.eInputBufFmt = VPP_COLOR_FORMAT_UBWC_NV12;
    strlcpy((char*)stTctx.params.cInputPath, (char*)INP_DIR_PATH, MAX_FILE_SEG_LEN);
    strlcpy((char*)stTctx.params.cInputName, "ivy_wall_1920x1080_nv12_UBWC.yuv", MAX_FILE_SEG_LEN);

    // Output file params
    stTctx.params.eOutputFileFormat = FILE_FORMAT_NV12_UBWC;
    stTctx.params.eOutputFileType = FILE_TYPE_MULTI_FRAMES;
    stTctx.params.eOutputBufFmt = VPP_COLOR_FORMAT_UBWC_NV12;
    strlcpy((char*)stTctx.params.cOutputPath, (char*)OUT_DIR_PATH, MAX_FILE_SEG_LEN);
    strlcpy((char*)stTctx.params.cOutputName, "ivy_wall_1920x1080_nv12_UBWC.bin", MAX_FILE_SEG_LEN);

    stCtxMe.pvME = vpVppIpFrcMe_Init(&stCtxMe.stCtx, 0, stTctx.cb);
    DVP_ASSERT_PTR_NNULL(stCtxMe.pvME);

    // Sending valid params after INIT but state is not ACTIVE
    u32Ret = u32VppIpFrcMe_GetBufferRequirements(stCtxMe.pvME, &stInputBufReq, &stOutputBufReq);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    DVP_ASSERT_EQUAL(stInputBufReq.u32PxSz, 0);
    DVP_ASSERT_EQUAL(stInputBufReq.u32MinCnt, 0);
    DVP_ASSERT_EQUAL(stOutputBufReq.u32PxSz, 0);
    DVP_ASSERT_EQUAL(stOutputBufReq.u32PxSz, 0);
    u32Ret = u32VppIpFrcMe_QueueBuf(stCtxMe.pvME, VPP_PORT_INPUT, (t_StVppBuf *)0x111111);
    DVP_ASSERT_EQUAL(u32Ret, VPP_ERR_STATE);
    u32Ret = u32VppIpFrcMe_Drain(stCtxMe.pvME);
    DVP_ASSERT_EQUAL(u32Ret, VPP_ERR_STATE);
    u32Ret = u32VppIpFrcMe_Close(stCtxMe.pvME);
    DVP_ASSERT_EQUAL(u32Ret, VPP_ERR_STATE);

    // Try Open ME with invalid port-params
    // Width mismatch
    stInParam.width = u32Width;
    stOutParam.width = u32Width - 1;
    stInParam.height = stOutParam.height = u32Height;
    stInParam.fmt = stOutParam.fmt = VPP_COLOR_FORMAT_UBWC_NV12;
    u32Ret = u32VppIpFrcMe_SetParam(stCtxMe.pvME, VPP_PORT_INPUT, stInParam);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppIpFrcMe_SetParam(stCtxMe.pvME, VPP_PORT_OUTPUT, stOutParam);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppIpFrcMe_Open(stCtxMe.pvME);
    DVP_ASSERT_EQUAL(u32Ret, VPP_ERR);
    // Height mismatch
    stInParam.width = stOutParam.width = u32Width;
    stInParam.height = u32Height;
    stOutParam.height = u32Height - 10;
    stInParam.fmt = stOutParam.fmt = VPP_COLOR_FORMAT_UBWC_NV12;
    u32Ret = u32VppIpFrcMe_SetParam(stCtxMe.pvME, VPP_PORT_INPUT, stInParam);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppIpFrcMe_SetParam(stCtxMe.pvME, VPP_PORT_OUTPUT, stOutParam);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppIpFrcMe_Open(stCtxMe.pvME);
    DVP_ASSERT_EQUAL(u32Ret, VPP_ERR);
    // format mismatch
    stInParam.width = stOutParam.width = u32Width;
    stInParam.height = stOutParam.height = u32Height;
    stInParam.fmt = VPP_COLOR_FORMAT_NV12_VENUS;
    stOutParam.fmt = VPP_COLOR_FORMAT_NV12_VENUS;
    u32Ret = u32VppIpFrcMe_SetParam(stCtxMe.pvME, VPP_PORT_INPUT, stInParam);
    DVP_ASSERT_EQUAL(u32Ret, VPP_ERR_PARAM);
    u32Ret = u32VppIpFrcMe_SetParam(stCtxMe.pvME, VPP_PORT_OUTPUT, stOutParam);
    DVP_ASSERT_EQUAL(u32Ret, VPP_ERR_PARAM);
    // width greater than MAX allowed
    stInParam.width = stOutParam.width = FRC_ME_MAX_W + 1;
    stInParam.height = stOutParam.height = u32Height;
    stInParam.fmt = stOutParam.fmt = VPP_COLOR_FORMAT_UBWC_NV12;
    u32Ret = u32VppIpFrcMe_SetParam(stCtxMe.pvME, VPP_PORT_INPUT, stInParam);
    DVP_ASSERT_EQUAL(u32Ret, VPP_ERR_PARAM);
    u32Ret = u32VppIpFrcMe_SetParam(stCtxMe.pvME, VPP_PORT_OUTPUT, stOutParam);
    DVP_ASSERT_EQUAL(u32Ret, VPP_ERR_PARAM);
    // width less than MIN allowed
    stInParam.width = stOutParam.width = FRC_ME_MIN_W - 1;
    stInParam.height = stOutParam.height = u32Height;
    stInParam.fmt = stOutParam.fmt = VPP_COLOR_FORMAT_UBWC_NV12;
    u32Ret = u32VppIpFrcMe_SetParam(stCtxMe.pvME, VPP_PORT_INPUT, stInParam);
    DVP_ASSERT_EQUAL(u32Ret, VPP_ERR_PARAM);
    u32Ret = u32VppIpFrcMe_SetParam(stCtxMe.pvME, VPP_PORT_OUTPUT, stOutParam);
    DVP_ASSERT_EQUAL(u32Ret, VPP_ERR_PARAM);
    // height greater than MAX allowed
    stInParam.width = stOutParam.width = u32Width;
    stInParam.height = stOutParam.height = FRC_ME_MAX_H + 1;
    stInParam.fmt = stOutParam.fmt = VPP_COLOR_FORMAT_UBWC_NV12;
    u32Ret = u32VppIpFrcMe_SetParam(stCtxMe.pvME, VPP_PORT_INPUT, stInParam);
    DVP_ASSERT_EQUAL(u32Ret, VPP_ERR_PARAM);
    u32Ret = u32VppIpFrcMe_SetParam(stCtxMe.pvME, VPP_PORT_OUTPUT, stOutParam);
    DVP_ASSERT_EQUAL(u32Ret, VPP_ERR_PARAM);
    // height less than MIN allowed
    stInParam.width = stOutParam.width = u32Width;
    stInParam.height = stOutParam.height = FRC_ME_MIN_H - 1;
    stInParam.fmt = stOutParam.fmt = VPP_COLOR_FORMAT_UBWC_NV12;
    u32Ret = u32VppIpFrcMe_SetParam(stCtxMe.pvME, VPP_PORT_INPUT, stInParam);
    DVP_ASSERT_EQUAL(u32Ret, VPP_ERR_PARAM);
    u32Ret = u32VppIpFrcMe_SetParam(stCtxMe.pvME, VPP_PORT_OUTPUT, stOutParam);
    DVP_ASSERT_EQUAL(u32Ret, VPP_ERR_PARAM);

    // Open ME with valid port-params
    u32Ret = u32VppIpFrcMe_SetParam(stCtxMe.pvME, VPP_PORT_INPUT, stTctx.port_param);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppIpFrcMe_SetParam(stCtxMe.pvME, VPP_PORT_OUTPUT, stTctx.port_param);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppIpFrcMe_Open(stCtxMe.pvME);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    // Try to queue NULL buffers or invalid port
    u32Ret = u32VppIpFrcMe_QueueBuf(stCtxMe.pvME, VPP_PORT_INPUT, NULL);
    DVP_ASSERT_EQUAL(u32Ret, VPP_ERR_PARAM);
    u32Ret = u32VppIpFrcMe_QueueBuf(stCtxMe.pvME, 0x123, (t_StVppBuf *)0x11111);
    DVP_ASSERT_EQUAL(u32Ret, VPP_ERR_PARAM);

    // Do input/output flush without queueing any buffer
    u32Ret = u32VppIpFrcMe_Flush(stCtxMe.pvME, VPP_PORT_INPUT);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppIpFrcMe_Flush(stCtxMe.pvME, VPP_PORT_OUTPUT);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    while (!((stTctx.u32Flags & INPUT_FLUSH_DONE) && (stTctx.u32Flags & OUTPUT_FLUSH_DONE)))
        sched_yield();

    // Get buffer requirements from ME and allocate them accordingly
    u32Ret = u32VppIpFrcMe_GetBufferRequirements(stCtxMe.pvME, &stInputBufReq, &stOutputBufReq);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    DVP_ASSERT_NEQUAL(stInputBufReq.u32PxSz, 0);
    DVP_ASSERT_NEQUAL(stOutputBufReq.u32PxSz, 0);
    DVP_ASSERT_EQUAL(stInputBufReq.u32ExSz, 0);
    DVP_ASSERT_EQUAL(stOutputBufReq.u32ExSz, 0);
    DVP_ASSERT_NEQUAL(stInputBufReq.u32MinCnt, 0);
    DVP_ASSERT_NEQUAL(stOutputBufReq.u32MinCnt, 0);

    u32Ret = init_buf_pool(&stTctx.stVppCtx, &stTctx.buf_pool, &stTctx.params,
                           stInputBufReq.u32MinCnt, VPP_TRUE);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    u32Ret = init_buf_pool(&stTctx.stVppCtx, &stTctx.buf_pool_ext, &stTctx.params,
                           stOutputBufReq.u32MinCnt, VPP_FALSE);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    LOGI("Testing input flush case now..");
    // Queue few input buffers and perform flush, all input buffers should be returned
    for (uint32_t i = 0; i < stInputBufReq.u32MinCnt; i++)
    {
        u32Ret = u32QueueInputBuf(i, 0);
        DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    }
    stTctx.u32Flags = 0;
    u32Ret = u32VppIpFrcMe_Flush(stCtxMe.pvME, VPP_PORT_INPUT);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    while (!(stTctx.u32Flags & INPUT_FLUSH_DONE))
        sched_yield();
    DVP_ASSERT_EQUAL(stTctx.buf_pool->u32ListSz, stInputBufReq.u32MinCnt);

    LOGI("Testing output flush case now..");
    // Queue few output buffers and perform flush, all output buffers should be returned
    for (uint32_t i = 0; i < stOutputBufReq.u32MinCnt; i++)
    {
        u32Ret = u32QueueOutputBuf(i);
        DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    }
    stTctx.u32Flags = 0;
    u32Ret = u32VppIpFrcMe_Flush(stCtxMe.pvME, VPP_PORT_OUTPUT);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    while (!(stTctx.u32Flags & OUTPUT_FLUSH_DONE))
        sched_yield();
    DVP_ASSERT_EQUAL(stTctx.buf_pool_ext->u32ListSz, stOutputBufReq.u32MinCnt);

    LOGI("Testing input bypass case now..");
    // Queue few input bypass buffers all input buffers should be returned
    stTctx.u32InRxCnt = 0;
    for (uint32_t i = 0; i < stInputBufReq.u32MinCnt; i++)
    {
        u32Ret = u32QueueInputBuf(i, VPP_BUF_FLAG_BYPASS);
        DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    }
    while (stTctx.u32InRxCnt < stInputBufReq.u32MinCnt)
        sched_yield();
    DVP_ASSERT_EQUAL(stTctx.buf_pool->u32ListSz, stInputBufReq.u32MinCnt);

    LOGI("Testing drain case now..");
    // Queue few input buffers and perform drain, all input buffers should be returned
    for (uint32_t i = 0; i < stInputBufReq.u32MinCnt; i++)
    {
        u32Ret = u32QueueInputBuf(i, 0);
        DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    }
    stTctx.u32Flags = 0;
    u32Ret = u32VppIpFrcMe_Drain(stCtxMe.pvME);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    while (!(stTctx.u32Flags & DRAIN_DONE))
    {
        static uint32_t u32Cnt = 0;
        if (stTctx.buf_pool_ext->u32ListSz)
        {
            u32Ret = u32QueueOutputBuf(u32Cnt++);
            DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
        }
        sched_yield();
    }
    DVP_ASSERT_EQUAL(stTctx.buf_pool->u32ListSz, stInputBufReq.u32MinCnt);

    LOGI("Closing now..");
    u32Ret = u32VppIpFrcMe_Close(stCtxMe.pvME);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    DVP_ASSERT_EQUAL(stTctx.buf_pool_ext->u32ListSz, stOutputBufReq.u32MinCnt);

    vVppIpFrcMe_Term(stCtxMe.pvME);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    free_buf_pool(stTctx.buf_pool_ext, VPP_FALSE);
    free_buf_pool(stTctx.buf_pool, VPP_TRUE);

    LOG_EXIT();
}

TEST(ME_MultipleInitTerm)
{
    uint32_t u32Ret = VPP_OK;
    memset(&stTctx, 0, sizeof(stTctx));
    stTctx.cb.input_buffer_done = test_me_input_buffer_done;
    stTctx.cb.output_buffer_done = test_me_output_buffer_done;
    stTctx.cb.event = test_me_event;
    stTctx.cb.pv = &stTctx;

    for (uint32_t i = 0; i < 100; i++)
    {
        stCtxMe.pvME = vpVppIpFrcMe_Init(&stCtxMe.stCtx, 0, stTctx.cb);
        DVP_ASSERT_PTR_NNULL(stCtxMe.pvME);

        vVppIpFrcMe_Term(stCtxMe.pvME);
        DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    }
}

TEST(ME_MultipleOpenClose)
{
    t_StVppIpBufReq stInputBufReq, stOutputBufReq;
    uint32_t u32Ret = VPP_OK;

    memset(&stTctx, 0, sizeof(stTctx));
    stTctx.cb.input_buffer_done = test_me_input_buffer_done;
    stTctx.cb.output_buffer_done = test_me_output_buffer_done;
    stTctx.cb.event = test_me_event;
    stTctx.cb.pv = &stTctx;

    stCtxMe.pvME = vpVppIpFrcMe_Init(&stCtxMe.stCtx, 0, stTctx.cb);
    DVP_ASSERT_PTR_NNULL(stCtxMe.pvME);

    tctx_set_port_params(&stTctx, 1920, 1080, VPP_COLOR_FORMAT_UBWC_NV12);

    u32Ret = u32VppIpFrcMe_SetParam(stCtxMe.pvME, VPP_PORT_INPUT, stTctx.port_param);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    u32Ret = u32VppIpFrcMe_SetParam(stCtxMe.pvME, VPP_PORT_OUTPUT, stTctx.port_param);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    for (uint32_t i = 0; i < 100; i++)
    {
        u32Ret = u32VppIpFrcMe_Open(stCtxMe.pvME);
        DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

        u32Ret = u32VppIpFrcMe_GetBufferRequirements(stCtxMe.pvME, &stInputBufReq, &stOutputBufReq);
        DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

        u32Ret = u32VppIpFrcMe_Close(stCtxMe.pvME);
        DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    }

    vVppIpFrcMe_Term(stCtxMe.pvME);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
}

TEST(ME_Reconfig)
{
    struct vpp_port_param stInParam, stOutParam;
    t_StVppIpBufReq stInputBufReq, stOutputBufReq;
    uint32_t u32InFramesToRun, u32InFramesQueued = 0, u32OutFramesQueued = 0;
    uint32_t u32Ret = VPP_OK;

    // Start the session with 720P input and then reconfigure it to a 1080P session
    u32InFramesToRun = 20;
    u32Ret = u32InitMETest(1280, 720, "Slomo_Pitching_nv12_UBWC_1280x720.yuv",
                           "Slomo_Pitching_nv12_UBWC_1280x720.bin");
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    u32Ret = u32VppIpFrcMe_GetBufferRequirements(stCtxMe.pvME, &stInputBufReq, &stOutputBufReq);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    while (u32InFramesQueued < u32InFramesToRun)
    {
        // Queue output buf
        if (stTctx.buf_pool_ext->u32ListSz)
        {
            u32Ret = u32QueueOutputBuf(u32OutFramesQueued);
            DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

            u32OutFramesQueued++;
        }

        // Queue input buf
        if (stTctx.buf_pool->u32ListSz)
        {
            u32Ret = u32QueueInputBuf(u32InFramesQueued, 0);
            DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

            u32InFramesQueued++;
        }

        if (!(stTctx.buf_pool_ext->u32ListSz && stTctx.buf_pool->u32ListSz))
           sched_yield();
    }

    // Drain before doing the reconfigure
    stTctx.u32Flags &= ~DRAIN_DONE;
    u32Ret = u32VppIpFrcMe_Drain(stCtxMe.pvME);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    while (!(stTctx.u32Flags & DRAIN_DONE))
    {
        if (stTctx.buf_pool_ext->u32ListSz)
        {
            u32Ret = u32QueueOutputBuf(u32OutFramesQueued);
            DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
            u32OutFramesQueued++;
        }
        sched_yield();
    }
    DVP_ASSERT_EQUAL(stTctx.buf_pool->u32ListSz, stInputBufReq.u32MinCnt);

    // Flush the output buffers, some might be in the ME queue
    u32Ret = u32VppIpFrcMe_Flush(stCtxMe.pvME, VPP_PORT_OUTPUT);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    while (!(stTctx.u32Flags & OUTPUT_FLUSH_DONE))
        sched_yield();

    DVP_ASSERT_EQUAL(stTctx.buf_pool_ext->u32ListSz, stOutputBufReq.u32MinCnt);

    free_buf_pool(stTctx.buf_pool_ext, VPP_FALSE);
    free_buf_pool(stTctx.buf_pool, VPP_FALSE);

    // Start reconfiguring the session to 1080P
    stInParam.width = stOutParam.width = 1920;
    stInParam.height = stOutParam.height = 1080;
    stInParam.fmt = stOutParam.fmt = VPP_COLOR_FORMAT_UBWC_NV12;
    u32Ret = u32VppIpFrcMe_Reconfigure(stCtxMe.pvME, stInParam, stOutParam);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    stTctx.u32InRxCnt = 0;
    stTctx.u32OutRxCnt = 0;
    stTctx.u32Flags = 0;
    u32InFramesQueued = 0,
    u32OutFramesQueued = 0;

    stTctx.pPrivateCtx = &stCtxMe;
    stTctx.params.u32Width = 1920;
    stTctx.params.u32Height = 1080;
    stTctx.params.eBufferType = eVppBufType_Progressive;
    tctx_set_port_params(&stTctx, 1920, 1080, VPP_COLOR_FORMAT_UBWC_NV12);

    // Input file params
    stTctx.params.eInputFileFormat = FILE_FORMAT_NV12_UBWC;
    stTctx.params.eInputFileType = FILE_TYPE_MULTI_FRAMES;
    stTctx.params.eInputBufFmt = VPP_COLOR_FORMAT_UBWC_NV12;
    strlcpy((char*)stTctx.params.cInputPath, (char*)INP_DIR_PATH, MAX_FILE_SEG_LEN);
    strlcpy((char*)stTctx.params.cInputName, "ivy_wall_1920x1080_nv12_UBWC.yuv", MAX_FILE_SEG_LEN);

    // Output file params
    stTctx.params.eOutputFileFormat = FILE_FORMAT_NV12_UBWC;
    stTctx.params.eOutputFileType = FILE_TYPE_MULTI_FRAMES;
    stTctx.params.eOutputBufFmt = VPP_COLOR_FORMAT_UBWC_NV12;
    strlcpy((char*)stTctx.params.cOutputPath, (char*)OUT_DIR_PATH, MAX_FILE_SEG_LEN);
    strlcpy((char*)stTctx.params.cOutputName, "ivy_wall_1920x1080_nv12_UBWC.bin", MAX_FILE_SEG_LEN);

    u32Ret = u32VppIpFrcMe_GetBufferRequirements(stCtxMe.pvME, &stInputBufReq, &stOutputBufReq);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    u32Ret = init_buf_pool(&stTctx.stVppCtx, &stTctx.buf_pool, &stTctx.params,
                           stInputBufReq.u32MinCnt, VPP_FALSE);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    u32Ret = init_buf_pool(&stTctx.stVppCtx, &stTctx.buf_pool_ext, &stTctx.params,
                           stOutputBufReq.u32MinCnt, VPP_FALSE);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    while (u32InFramesQueued < u32InFramesToRun)
    {
        // Queue output buf
        if (stTctx.buf_pool_ext->u32ListSz)
        {
            u32Ret = u32QueueOutputBuf(u32OutFramesQueued);
            DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

            u32OutFramesQueued++;
        }

        // Queue input buf
        if (stTctx.buf_pool->u32ListSz)
        {
            u32Ret = u32QueueInputBuf(u32InFramesQueued, 0);
            DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

            u32InFramesQueued++;
        }

        if (!(stTctx.buf_pool_ext->u32ListSz && stTctx.buf_pool->u32ListSz))
           sched_yield();
    }

    u32Ret = u32TermMETest();
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
}

/************************************************************************
 * Global Functions
 ***********************************************************************/
TEST_CASES IpMeTests[] = {
    TEST_CASE(ME_EOS),
    TEST_CASE(ME_Drain),
    TEST_CASE(ME_TestApi),
    TEST_CASE(ME_MultipleInitTerm),
    TEST_CASE(ME_MultipleOpenClose),
    TEST_CASE(ME_Performance),
    TEST_CASE(ME_NoEOS),
    TEST_CASE(ME_Bypass),
    TEST_CASE(ME_BufferCnt),
    TEST_CASE(ME_Reconfig),
    TEST_CASE_NULL(),
};

TEST_SUITE(IpMeSuite,
        "IpMeTests",
        IpMeSuiteInit,
        IpMeSuiteTerm,
        IpMeTestInit,
        IpMeTestTerm,
        IpMeTests);