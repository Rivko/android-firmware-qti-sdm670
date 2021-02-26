/*!
 * @file test_vpp.c
 *
 * @cr
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.

 * @services    <Describe what this is supposed to test>
 */

#include <sys/types.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#include "dvpTest.h"
#include "dvpTest_tb.h"

#define VPP_LOG_TAG     VPP_LOG_UT_VPP_CORE_TAG
#define VPP_LOG_MODULE  VPP_LOG_UT_VPP_CORE
#include "vpp_dbg.h"
#include "vpp_def.h"
#include "vpp_ctx.h"
#include "vpp_core.h"
#include "vpp_utils.h"
#include "vpp.h"
#include "vpp_uc.h"

#include "test_mock_registry.h"
#include "test_utils.h"


/************************************************************************
 * Local definitions
 ***********************************************************************/
static pthread_cond_t sCond = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t sMutex = PTHREAD_MUTEX_INITIALIZER;

static uint32_t u32InputFlushRx;
static uint32_t u32OutputFlushRx;
static uint32_t u32ReconfigDoneRx;
/************************************************************************
 * Local static variables
 ***********************************************************************/
static void vpp_ibd_cb(void *pv, struct vpp_buffer *buf);
static void vpp_obd_cb(void *pv, struct vpp_buffer *buf);
static void vpp_evt_cb(void *pv, struct vpp_event e);

static struct vpp_callbacks sCb = {
    .input_buffer_done = vpp_ibd_cb,
    .output_buffer_done = vpp_obd_cb,
    .vpp_event = vpp_evt_cb,
};

/************************************************************************
 * Forward Declarations
 ************************************************************************/

/************************************************************************
 * Local Functions
 ***********************************************************************/
static void vpp_ibd_cb(void *pv, struct vpp_buffer *buf)
{
    (void)(pv);
    struct bufnode *pBufNode = (struct bufnode *)buf->cookie;

    if ((pBufNode) && (pBufNode->pPool))
    {
        pthread_mutex_lock(&sMutex);
        put_buf(pBufNode->pPool, pBufNode);
        pthread_mutex_unlock(&sMutex);
    }
}

static void vpp_obd_cb(void *pv, struct vpp_buffer *buf)
{
    (void)(pv);
    struct bufnode *pBufNode = (struct bufnode *)buf->cookie;

    if ((pBufNode) && (pBufNode->pPool))
    {
        pthread_mutex_lock(&sMutex);
        put_buf(pBufNode->pPool, pBufNode);
        pthread_mutex_unlock(&sMutex);
    }
}

static void vpp_evt_cb(void *pv, struct vpp_event e)
{
    (void)(pv);
    LOGI("%s(), event_type=%u", __func__, e.type);
    if (e.type == VPP_EVENT_FLUSH_DONE)
    {
        pthread_mutex_lock(&sMutex);
        if (e.flush_done.port == VPP_PORT_INPUT)
        {
            u32InputFlushRx = VPP_TRUE;
        }
        else if (e.flush_done.port == VPP_PORT_OUTPUT)
        {
            u32OutputFlushRx = VPP_TRUE;
        }
        pthread_mutex_unlock(&sMutex);
        pthread_cond_signal(&sCond);
    }
    else if (e.type == VPP_EVENT_RECONFIG_DONE)
    {
        pthread_mutex_lock(&sMutex);

        DVP_ASSERT_EQUAL(VPP_OK, e.port_reconfig_done.reconfig_status);

        u32ReconfigDoneRx = VPP_TRUE;
        pthread_mutex_unlock(&sMutex);
        pthread_cond_signal(&sCond);
    }
}

/************************************************************************
 * Test Functions
 ***********************************************************************/
TEST_SUITE_INIT(VppSuiteInit)
{
    init_test_registry(TEST_SUITE_VPP_CLIENT);
}

TEST_SUITE_TERM(VppSuiteTerm)
{
    term_test_registry();
}

TEST_SETUP(VppTestInit)
{
}

TEST_CLEANUP(VppTestTerm)
{
}

TEST(Vpp_InitTermSingle)
{
    void *p;

    p = vpp_init(0, sCb);
    DVP_ASSERT_PTR_NNULL(p);
    vpp_term(p);
}


#define MAX_CONCURRENT_SESSIONS 2
TEST(Vpp_InitTermMultiple)
{
    uint32_t i;
    void *apvCtx[MAX_CONCURRENT_SESSIONS];
    memset(apvCtx, 0, sizeof(void *));


    for (i = 0; i < MAX_CONCURRENT_SESSIONS; i++)
    {
        apvCtx[i] = vpp_init(0, sCb);
        DVP_ASSERT_PTR_NNULL(apvCtx[i]);
    }

    for (i = 0; i < MAX_CONCURRENT_SESSIONS; i++)
    {
        if (i == 0)
            continue;
        DVP_ASSERT_PTR_NEQUAL(apvCtx[i], apvCtx[i - 1]);
    }

    for (i = 0; i < MAX_CONCURRENT_SESSIONS; i++)
    {
        vpp_term(apvCtx[i]);
    }

    // Make sure we don't crash
    vpp_term(NULL);

    for (i = 0; i < 10; i++)
    {
        void *p = vpp_init(0, sCb);
        DVP_ASSERT_PTR_NNULL(p);
        vpp_term(p);
        p = NULL;
    }
}

TEST(__unused Vpp_InitCpNotSupported)
{
    term_test_registry();
    init_test_registry(TEST_SUITE_VPP_CLIENT_NO_CP);
    void *p;

    p = vpp_init(VPP_SESSION_SECURE, sCb);
    DVP_ASSERT_PTR_NULL(p);

    p = vpp_init(0, sCb);
    DVP_ASSERT_PTR_NNULL(p);

    vpp_term(p);
    term_test_registry();
    init_test_registry(TEST_SUITE_VPP_CLIENT);
}

TEST(Vpp_SetCtrl)
{
    uint32_t u32;
    struct hqv_control ctrl;
    struct vpp_requirements req;

    void *p;

    memset(&ctrl, 0, sizeof(struct hqv_control));

    u32 = vpp_set_ctrl(NULL, ctrl, &req);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    p = vpp_init(0, sCb);
    DVP_ASSERT_PTR_NNULL(p);

    // Set some invalid controls
    ctrl.mode = 902389;
    u32 = vpp_set_ctrl(p, ctrl, &req);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    // Set to auto
    ctrl.mode = HQV_MODE_AUTO;
    u32 = vpp_set_ctrl(p, ctrl, &req);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    DVP_ASSERT_NEQUAL(req.in_color_fmt_mask, 0);

    // Set to off
    ctrl.mode = HQV_MODE_OFF;
    u32 = vpp_set_ctrl(p, ctrl, &req);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    DVP_ASSERT_EQUAL(req.in_color_fmt_mask, 0);

    // Set to manual
    ctrl.mode = HQV_MODE_MANUAL;
    ctrl.ctrl_type = HQV_CONTROL_CADE;
    ctrl.cade.mode = HQV_MODE_AUTO;
    u32 = vpp_set_ctrl(p, ctrl, &req);
    DVP_ASSERT_TRUE(u32 == VPP_OK || u32 == VPP_ERR_INVALID_CFG);

    vpp_term(p);
}

TEST(Vpp_SetParameter)
{
    uint32_t u32;
    struct vpp_port_param param;
    void *p;

    memset(&param, 0, sizeof(struct vpp_port_param));

    p = vpp_init(0, sCb);
    DVP_ASSERT_PTR_NNULL(p);

    // Garbage input
    u32 = vpp_set_parameter(NULL, VPP_PORT_INPUT, param);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);
    u32 = vpp_set_parameter(NULL, VPP_PORT_OUTPUT, param);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);
    u32 = vpp_set_parameter(NULL, 123198, param);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);
    u32 = vpp_set_parameter(p, 123198, param);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    // Garbage parameters
    param.width = 0;
    param.height = 0;
    param.fmt = VPP_COLOR_FORMAT_NV12_VENUS;

    // Setting input and output port will cause state transition
    u32 = vpp_set_parameter(p, VPP_PORT_INPUT, param);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    u32 = vpp_set_parameter(p, VPP_PORT_OUTPUT, param);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    // Valid input
    param.width = 1920;
    param.height = 1080;
    vpp_term(p);

    // Reset and try again. Should error because we didn't set hqv controls
    // However, still will be put into active state
    p = vpp_init(0, sCb);
    DVP_ASSERT_PTR_NNULL(p);
    u32 = vpp_set_parameter(p, VPP_PORT_INPUT, param);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    u32 = vpp_set_parameter(p, VPP_PORT_OUTPUT, param);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    // Now in active state. Should reject any attempts to set params.
    param.width = 1280;
    param.height = 720;
    u32 = vpp_set_parameter(p, VPP_PORT_INPUT, param);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_STATE);
    u32 = vpp_set_parameter(p, VPP_PORT_OUTPUT, param);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_STATE);

    vpp_term(p);
}

TEST(Vpp_QueueBuf)
{
    uint32_t u32;
    void *p;
    struct vpp_buffer buf;
    struct vpp_buffer *pBuf = &buf;

    p = vpp_init(0, sCb);
    DVP_ASSERT_PTR_NNULL(p);

    // Garbage
    u32 = vpp_queue_buf(NULL, (enum vpp_port)9102831092, NULL);
    DVP_ASSERT_NEQUAL(VPP_OK, u32);
    u32 = vpp_queue_buf(NULL, VPP_PORT_INPUT, NULL);
    DVP_ASSERT_NEQUAL(VPP_OK, u32);
    u32 = vpp_queue_buf(NULL, VPP_PORT_OUTPUT, NULL);
    DVP_ASSERT_NEQUAL(VPP_OK, u32);
    u32 = vpp_queue_buf(NULL, VPP_PORT_INPUT, pBuf);
    DVP_ASSERT_NEQUAL(VPP_OK, u32);
    u32 = vpp_queue_buf(NULL, VPP_PORT_OUTPUT, pBuf);
    DVP_ASSERT_NEQUAL(VPP_OK, u32);
    u32 = vpp_queue_buf(p, (enum vpp_port)9102831092, NULL);
    DVP_ASSERT_NEQUAL(VPP_OK, u32);
    u32 = vpp_queue_buf(p, VPP_PORT_INPUT, NULL);
    DVP_ASSERT_NEQUAL(VPP_OK, u32);
    u32 = vpp_queue_buf(p, VPP_PORT_OUTPUT, NULL);
    DVP_ASSERT_NEQUAL(VPP_OK, u32);

    // Valid input, invalid state
    u32 = vpp_queue_buf(p, VPP_PORT_INPUT, pBuf);
    DVP_ASSERT_NEQUAL(VPP_OK, u32);
    u32 = vpp_queue_buf(p, VPP_PORT_OUTPUT, pBuf);
    DVP_ASSERT_NEQUAL(VPP_OK, u32);

    vpp_term(p);
}

TEST(Vpp_Timestamp)
{
    uint32_t u32;
    void *p;
    struct hqv_control ctrl;
    struct vpp_requirements req;
    struct vpp_port_param param;
    t_StVppCb *pstCb;
    uint32_t u32FrameRate, u32DefaultRate, u32TimestampIncrement, u32Timestamp, u32Resolution, i;
    uint32_t u32MaxRateBypass = VPP_FALSE;
    struct video_property vid_prop;
    t_StVppUsecase *pstUc;
    struct buf_pool *buf_pool;
    struct buf_pool_params buf_params;
    struct bufnode *pNode;

    p = vpp_init(0, sCb);
    DVP_ASSERT_PTR_NNULL(p);

    param.width = 720;
    param.height = 480;
    param.fmt = VPP_COLOR_FORMAT_NV12_VENUS;
    param.stride = u32VppUtils_CalculateStride(param.width, param.fmt);
    param.scanlines = u32VppUtils_CalculateScanlines(param.width, param.fmt);
    u32Resolution = VPP_RESOLUTION_SD;

    buf_params_init_default(&buf_params, &param);
    u32 = init_buf_pool(p, &buf_pool, &buf_params, 50, VPP_TRUE);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    pstCb = ((t_StVppCtx *)p)->pstVppCb;

    ctrl.mode = HQV_MODE_AUTO;
    u32 = vpp_set_ctrl(p, ctrl, &req);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // Setting input and output port will cause state transition
    u32 = vpp_set_parameter(p, VPP_PORT_INPUT, param);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    u32 = vpp_set_parameter(p, VPP_PORT_OUTPUT, param);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // Valid input at use case default rate
    pstUc = pstCb->pstUc;

    if (!pstUc)
    {
        DVP_ASSERT_FAIL();
        LOGE("Null Use case. Skipping test");
        free_buf_pool(buf_pool, VPP_TRUE);
        vpp_term(p);
        return;
    }

    // Set flag if use case should go to bypass at VPP_MAX_FRAME_RATE
    for (i = 0; i < VPP_IP_BLOCK_MAX; i++)
    {
        if (pstUc->credits[i].ip == NULL)
            continue;
        if ((pstUc->credits[i].au32Credit[u32Resolution] * VPP_MAX_FRAME_RATE) >
            pstUc->credits[i].ip->capabilities.u32MaxCredits)
            u32MaxRateBypass = VPP_TRUE;
    }

    u32DefaultRate = u32VppUsecase_GetDefaultFps(pstUc);
    u32TimestampIncrement = 1000000 / u32DefaultRate;
    u32Timestamp = u32TimestampIncrement;
    for (i = 0; i < 10; i++)
    {
        pthread_mutex_lock(&sMutex);
        while (NULL == (pNode = get_buf(buf_pool)))
            pthread_cond_wait(&sCond, &sMutex);
        pthread_mutex_unlock(&sMutex);

        pNode->pExtBuf->timestamp = u32Timestamp;
        u32 = vpp_queue_buf(p, VPP_PORT_INPUT, pNode->pExtBuf);
        DVP_ASSERT_EQUAL(VPP_OK, u32);
        if (i > 0)
        {
            u32FrameRate = pstCb->stFrameRate.u32ProgrammedFrameRate;
            DVP_ASSERT_TRUE((u32FrameRate >= (u32DefaultRate - 1)) &&
                            (u32FrameRate <= u32DefaultRate));
        }
        if (i == 7)
            u32Timestamp += 10000; // Insert 1 frame glitch (100Hz), should not cause bypass
        else
            u32Timestamp += u32TimestampIncrement;
    }

    // Max frame rate will go to bypass if rate is too high for UC
    u32TimestampIncrement = 1000000 / VPP_MAX_FRAME_RATE;
    u32Timestamp = u32TimestampIncrement;

    for (i = 0; i < 10; i++)
    {
        pthread_mutex_lock(&sMutex);
        while (NULL == (pNode = get_buf(buf_pool)))
            pthread_cond_wait(&sCond, &sMutex);
        pthread_mutex_unlock(&sMutex);
        if (i == 0)
            pNode->pExtBuf->flags |= VPP_BUFFER_FLAG_EOS; // Send EOS to reset timestamp

        pNode->pExtBuf->timestamp = u32Timestamp;
        u32 = vpp_queue_buf(p, VPP_PORT_INPUT, pNode->pExtBuf);
        DVP_ASSERT_EQUAL(VPP_OK, u32);
        if (i > 0)
        {
            u32FrameRate = pstCb->stFrameRate.u32ProgrammedFrameRate;
            DVP_ASSERT_TRUE((u32FrameRate >= (VPP_MAX_FRAME_RATE - 1)) &&
                            (u32FrameRate <= VPP_MAX_FRAME_RATE));
            if (u32MaxRateBypass == VPP_TRUE)
                DVP_ASSERT_TRUE(pstCb->stFrameRate.u32Bypass);
            else
                DVP_ASSERT_FALSE(pstCb->stFrameRate.u32Bypass);
        }
        u32Timestamp += u32TimestampIncrement;
    }
    // Go back to use case default rate
    u32TimestampIncrement = 1000000 / u32DefaultRate;

    for (i = 0; i < 10; i++)
    {
        pthread_mutex_lock(&sMutex);
        while (NULL == (pNode = get_buf(buf_pool)))
            pthread_cond_wait(&sCond, &sMutex);
        pthread_mutex_unlock(&sMutex);

        pNode->pExtBuf->timestamp = u32Timestamp;
        u32 = vpp_queue_buf(p, VPP_PORT_INPUT, pNode->pExtBuf);
        DVP_ASSERT_EQUAL(VPP_OK, u32);
        u32Timestamp += u32TimestampIncrement;
    }
    // Should not bypass
    u32FrameRate = pstCb->stFrameRate.u32ProgrammedFrameRate;
    DVP_ASSERT_TRUE((u32FrameRate >= (u32DefaultRate - 1)) && (u32FrameRate <= u32DefaultRate));
    DVP_ASSERT_FALSE(pstCb->stFrameRate.u32Bypass);

    // Set Non-realtime and send max rate input.  Should ignore timestamp and use usecase default
    u32TimestampIncrement = 1000000 / VPP_MAX_FRAME_RATE;
    u32Timestamp = u32TimestampIncrement;
    vid_prop.property_type = VID_PROP_NON_REALTIME;
    vid_prop.non_realtime.bNonRealtime = 1;
    u32 = vpp_set_vid_prop(p, vid_prop);

    for (i = 0; i < 5; i++)
    {
        pthread_mutex_lock(&sMutex);
        while (NULL == (pNode = get_buf(buf_pool)))
            pthread_cond_wait(&sCond, &sMutex);
        pthread_mutex_unlock(&sMutex);
        if (i == 0)
            pNode->pExtBuf->flags |= VPP_BUFFER_FLAG_EOS; // Send EOS to reset timestamp

        pNode->pExtBuf->timestamp = u32Timestamp;
        u32 = vpp_queue_buf(p, VPP_PORT_INPUT, pNode->pExtBuf);
        DVP_ASSERT_EQUAL(VPP_OK, u32);
        u32FrameRate = pstCb->stFrameRate.u32ProgrammedFrameRate;
        DVP_ASSERT_TRUE((u32FrameRate >= (u32DefaultRate - 1)) &&
                        (u32FrameRate <= u32DefaultRate));
        DVP_ASSERT_FALSE(pstCb->stFrameRate.u32Bypass);
        u32Timestamp += u32TimestampIncrement;
    }

    // Set back to realtime, will go to bypass if max rate is too high for UC
    vid_prop.property_type = VID_PROP_NON_REALTIME;
    vid_prop.non_realtime.bNonRealtime = 0;
    u32 = vpp_set_vid_prop(p, vid_prop);
    for (i = 0; i < 5; i++)
    {
        pthread_mutex_lock(&sMutex);
        while (NULL == (pNode = get_buf(buf_pool)))
            pthread_cond_wait(&sCond, &sMutex);
        pthread_mutex_unlock(&sMutex);

        pNode->pExtBuf->timestamp = u32Timestamp;
        u32 = vpp_queue_buf(p, VPP_PORT_INPUT, pNode->pExtBuf);
        DVP_ASSERT_EQUAL(VPP_OK, u32);
        u32FrameRate = pstCb->stFrameRate.u32ProgrammedFrameRate;
        DVP_ASSERT_TRUE((u32FrameRate >= (VPP_MAX_FRAME_RATE - 1)) &&
                        (u32FrameRate <= VPP_MAX_FRAME_RATE));
        if (u32MaxRateBypass == VPP_TRUE)
            DVP_ASSERT_TRUE(pstCb->stFrameRate.u32Bypass);
        else
            DVP_ASSERT_FALSE(pstCb->stFrameRate.u32Bypass);
        u32Timestamp += u32TimestampIncrement;
    }

    // Set operating rate to usecase default.  Should not bypass
    vid_prop.property_type = VID_PROP_OPERATING_RATE;
    vid_prop.operating_rate.u32OperatingRate = u32DefaultRate;
    u32 = vpp_set_vid_prop(p, vid_prop);
    for (i = 0; i < 5; i++)
    {
        pthread_mutex_lock(&sMutex);
        while (NULL == (pNode = get_buf(buf_pool)))
            pthread_cond_wait(&sCond, &sMutex);
        pthread_mutex_unlock(&sMutex);
        pNode->pExtBuf->timestamp = u32Timestamp;
        u32 = vpp_queue_buf(p, VPP_PORT_INPUT, pNode->pExtBuf);
        DVP_ASSERT_EQUAL(VPP_OK, u32);
        u32FrameRate = pstCb->stFrameRate.u32ProgrammedFrameRate;
        DVP_ASSERT_TRUE((u32FrameRate >= (u32DefaultRate - 1)) &&
                        (u32FrameRate <= u32DefaultRate));
        DVP_ASSERT_FALSE(pstCb->stFrameRate.u32Bypass);
        u32Timestamp += u32TimestampIncrement;
    }

    // Disable operating rate, should use timestamp and go to bypass if max rate is too high for UC
    vid_prop.property_type = VID_PROP_OPERATING_RATE;
    vid_prop.operating_rate.u32OperatingRate = 0;
    u32 = vpp_set_vid_prop(p, vid_prop);
    for (i = 0; i < 5; i++)
    {
        pthread_mutex_lock(&sMutex);
        while (NULL == (pNode = get_buf(buf_pool)))
            pthread_cond_wait(&sCond, &sMutex);
        pthread_mutex_unlock(&sMutex);

        pNode->pExtBuf->timestamp = u32Timestamp;
        u32 = vpp_queue_buf(p, VPP_PORT_INPUT, pNode->pExtBuf);
        DVP_ASSERT_EQUAL(VPP_OK, u32);
        u32FrameRate = pstCb->stFrameRate.u32ProgrammedFrameRate;
        DVP_ASSERT_TRUE((u32FrameRate >= (VPP_MAX_FRAME_RATE - 1)) &&
                        (u32FrameRate <= VPP_MAX_FRAME_RATE));
        if (u32MaxRateBypass == VPP_TRUE)
            DVP_ASSERT_TRUE(pstCb->stFrameRate.u32Bypass);
        else
            DVP_ASSERT_FALSE(pstCb->stFrameRate.u32Bypass);
        u32Timestamp += u32TimestampIncrement;
    }

    pthread_mutex_lock(&sMutex);
    u32InputFlushRx = VPP_FALSE;
    pthread_mutex_unlock(&sMutex);
    vpp_flush(p, VPP_PORT_INPUT);
    pthread_mutex_lock(&sMutex);
    while (u32InputFlushRx != VPP_TRUE)
        pthread_cond_wait(&sCond, &sMutex);
    pthread_mutex_unlock(&sMutex);

    free_buf_pool(buf_pool, VPP_TRUE);

    vpp_term(p);
}

TEST(Vpp_Flush)
{
    uint32_t u32;
    struct hqv_control ctrl;
    struct vpp_requirements req;
    struct vpp_port_param param;
    void *p;

    t_StVppUsecase *pstUcAuto = getUsecaseAuto(TEST_SUITE_REAL_REGISTRY);
    if (!pstUcAuto)
    {
        DVP_ASSERT_FAIL();
        return;
    }

    u32 = u32VppUsecase_GetMaxResForUc(pstUcAuto);

    ctrl.mode = HQV_MODE_AUTO;
    param.fmt = VPP_COLOR_FORMAT_NV12_VENUS;
    param.width = u32GetMaxWidthForRes(u32);
    param.height = u32GetMaxHeightForRes(u32);;
    param.stride = u32VppUtils_CalculateStride(param.width, param.fmt);
    param.scanlines = u32VppUtils_CalculateScanlines(param.height, param.fmt);

    LOGI("%s: w/h = %u/%u", __func__, param.width, param.height);

    p = vpp_init(0, sCb);
    DVP_ASSERT_PTR_NNULL(p);

    // Garbage inputs
    u32 = vpp_flush(NULL, (enum vpp_port)9210834);
    DVP_ASSERT_NEQUAL(VPP_OK, u32);
    u32 = vpp_flush(NULL, VPP_PORT_INPUT);
    DVP_ASSERT_NEQUAL(VPP_OK, u32);
    u32 = vpp_flush(NULL, VPP_PORT_OUTPUT);
    DVP_ASSERT_NEQUAL(VPP_OK, u32);
    u32 = vpp_flush(p, (enum vpp_port)10989123);
    DVP_ASSERT_NEQUAL(VPP_OK, u32);

    // Invalid state
    u32 = vpp_flush(p, VPP_PORT_INPUT);
    DVP_ASSERT_NEQUAL(VPP_OK, u32);
    u32 = vpp_flush(p, VPP_PORT_OUTPUT);
    DVP_ASSERT_NEQUAL(VPP_OK, u32);

    // Transition to active valid state
    u32 = vpp_set_ctrl(p, ctrl, &req);
    DVP_ASSERT_EQUAL(VPP_OK, u32);
    u32 = vpp_set_parameter(p, VPP_PORT_INPUT, param);
    DVP_ASSERT_EQUAL(VPP_OK, u32);
    u32 = vpp_set_parameter(p, VPP_PORT_OUTPUT, param);
    DVP_ASSERT_EQUAL(VPP_OK, u32);

    // Valid state. No buffers. Should expect flush done events
    // Input
    pthread_mutex_lock(&sMutex);
    u32InputFlushRx = VPP_FALSE;
    pthread_mutex_unlock(&sMutex);
    u32 = vpp_flush(p, VPP_PORT_INPUT);
    DVP_ASSERT_EQUAL(VPP_OK, u32);
    pthread_mutex_lock(&sMutex);
    while (u32InputFlushRx != VPP_TRUE)
        pthread_cond_wait(&sCond, &sMutex);
    pthread_mutex_unlock(&sMutex);

    // Output
    pthread_mutex_lock(&sMutex);
    u32OutputFlushRx = VPP_FALSE;
    pthread_mutex_unlock(&sMutex);
    u32 = vpp_flush(p, VPP_PORT_OUTPUT);
    DVP_ASSERT_EQUAL(VPP_OK, u32);
    pthread_mutex_lock(&sMutex);
    while (u32OutputFlushRx != VPP_TRUE)
        pthread_cond_wait(&sCond, &sMutex);
    pthread_mutex_unlock(&sMutex);

    vpp_term(p);
}

TEST(Vpp_ReconfigBasic)
{
    uint32_t u32;
    void *p = NULL;
    struct vpp_requirements req;

    struct hqv_control ctrl = {
        .mode = HQV_MODE_AUTO,
    };

    t_StVppUsecase *pstUcAuto = getUsecaseAuto(TEST_SUITE_REAL_REGISTRY);
    if (!pstUcAuto)
    {
        DVP_ASSERT_FAIL();
        return;
    }

    u32 = u32VppUsecase_GetMaxResForUc(pstUcAuto);

    struct vpp_port_param param = {
        .width = u32GetMaxWidthForRes(u32),
        .height = u32GetMaxHeightForRes(u32),
        .fmt = VPP_COLOR_FORMAT_NV12_VENUS,
    };

    param.stride = u32VppUtils_CalculateStride(param.width, param.fmt);
    param.scanlines = u32VppUtils_CalculateScanlines(param.height, param.fmt);

    // Garbage params
    u32 = vpp_reconfigure(NULL, param, param);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);

    // Transition to inited state
    p = vpp_init(0, sCb);
    DVP_ASSERT_PTR_NNULL(p);

    // Should not be able to reconfigure in INITED state
    u32 = vpp_reconfigure(p, param, param);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_STATE);

    // Transition to active state
    u32 = vpp_set_ctrl(p, ctrl, &req);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    u32 = vpp_set_parameter(p, VPP_PORT_INPUT, param);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    u32 = vpp_set_parameter(p, VPP_PORT_OUTPUT, param);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // Request reconfigure with no buffers having been queued
    param.width = u32GetMaxWidthForRes(u32 ? u32 - 1 : u32);
    param.height = u32GetMaxHeightForRes (u32 ? u32 - 1 : u32);
    param.stride = u32VppUtils_CalculateStride(param.width, param.fmt);
    param.scanlines = u32VppUtils_CalculateScanlines(param.height, param.fmt);
    u32 = vpp_reconfigure(p, param, param);

    pthread_mutex_lock(&sMutex);
    while (u32ReconfigDoneRx != VPP_TRUE)
        pthread_cond_wait(&sCond, &sMutex);
    pthread_mutex_unlock(&sMutex);

    vpp_term(p);
}

TEST(Vpp_SetVidProp)
{
    uint32_t u32;
    void *p = NULL;
    struct video_property ctrl_set, ctrl_get;

    p = vpp_init(VPP_SESSION_NON_REALTIME, sCb);
    DVP_ASSERT_PTR_NNULL(p);

    // Verify non-realtime flag set
    ctrl_get.property_type = VID_PROP_NON_REALTIME;
    u32 = u32VppUtils_GetVidProp(p, &ctrl_get);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    DVP_ASSERT_EQUAL(ctrl_get.non_realtime.bNonRealtime, 1);

    // Valid set
    ctrl_set.property_type = VID_PROP_CODEC;
    ctrl_set.codec.eCodec = VPP_CODEC_TYPE_H263;
    u32 = vpp_set_vid_prop(p, ctrl_set);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // Null context set
    u32 = vpp_set_vid_prop(NULL, ctrl_set);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    // Invalid property type set
    ctrl_set.property_type = VID_PROP_MAX;
    u32 = vpp_set_vid_prop(p, ctrl_set);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    // Invalid property type get
    ctrl_get.property_type = VID_PROP_MAX;
    u32 = vpp_set_vid_prop(p, ctrl_set);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    // Valid get
    ctrl_get.property_type = VID_PROP_CODEC;
    u32 = u32VppUtils_GetVidProp(p, &ctrl_get);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    DVP_ASSERT_EQUAL(ctrl_set.codec.eCodec, ctrl_get.codec.eCodec);

    // Valid set codec out of range
    ctrl_set.property_type = VID_PROP_CODEC;
    ctrl_set.codec.eCodec = 0xFFFFFFFF;
    u32 = vpp_set_vid_prop(p, ctrl_set);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // Valid get (should be unknown)
    ctrl_get.property_type = VID_PROP_CODEC;
    u32 = u32VppUtils_GetVidProp(p, &ctrl_get);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    DVP_ASSERT_EQUAL(VPP_CODEC_TYPE_UNKNOWN, ctrl_get.codec.eCodec);

    // Null context get
    u32 = u32VppUtils_GetVidProp(NULL, &ctrl_get);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    // Valid set
    ctrl_set.property_type = VID_PROP_NON_REALTIME;
    ctrl_set.non_realtime.bNonRealtime = 0;
    u32 = vpp_set_vid_prop(p, ctrl_set);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // Valid get
    ctrl_get.property_type = VID_PROP_NON_REALTIME;
    u32 = u32VppUtils_GetVidProp(p, &ctrl_get);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    DVP_ASSERT_EQUAL(ctrl_get.non_realtime.bNonRealtime, 0);

    // Valid set operating rate
    ctrl_set.property_type = VID_PROP_OPERATING_RATE;
    ctrl_set.operating_rate.u32OperatingRate = 0xFFFFFFFF;
    u32 = vpp_set_vid_prop(p, ctrl_set);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // Valid get
    ctrl_get.property_type = VID_PROP_OPERATING_RATE;
    u32 = u32VppUtils_GetVidProp(p, &ctrl_get);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    DVP_ASSERT_EQUAL(ctrl_get.operating_rate.u32OperatingRate, VPP_MAX_FRAME_RATE);

    vpp_term(p);
}

TEST(Vpp_BootShutdown)
{
    uint32_t u32;
    void *p1, *p2;

    // Shutdown without booting should fail
    u32 = vpp_shutdown();
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    // Normal boot/shutdown
    u32 = vpp_boot();
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    u32 = vpp_shutdown();
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // Boot with open sessions should fail
    p1 = vpp_init(0, sCb);
    DVP_ASSERT_PTR_NNULL(p1);

    u32 = vpp_boot();
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    p2 = vpp_init(0, sCb);
    DVP_ASSERT_PTR_NNULL(p2);

    u32 = vpp_boot();
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    vpp_term(p1);
    u32 = vpp_boot();
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    // Boot after all sessions closed
    vpp_term(p2);
    u32 = vpp_boot();
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // Boot again should fail
    u32 = vpp_boot();
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    // Shutdown should fail with sessions open
    p1 = vpp_init(0, sCb);
    DVP_ASSERT_PTR_NNULL(p1);

    u32 = vpp_shutdown();
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    p2 = vpp_init(0, sCb);
    DVP_ASSERT_PTR_NNULL(p2);

    u32 = vpp_shutdown();
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    vpp_term(p2);
    u32 = vpp_shutdown();
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    // Shutdown after all sessions closed
    vpp_term(p1);
    u32 = vpp_shutdown();
    DVP_ASSERT_EQUAL(u32, VPP_OK);
}

/************************************************************************
 * Global Functions
 ***********************************************************************/
TEST_CASES VppTests[] = {
    TEST_CASE(Vpp_InitTermSingle),
    TEST_CASE(Vpp_InitTermMultiple),
    // TEST_CASE(Vpp_InitCpNotSupported),
    TEST_CASE(Vpp_SetCtrl),
    TEST_CASE(Vpp_SetParameter),
    TEST_CASE(Vpp_QueueBuf),
    TEST_CASE(Vpp_Timestamp),
    TEST_CASE(Vpp_Flush),
    TEST_CASE(Vpp_ReconfigBasic),
    TEST_CASE(Vpp_SetVidProp),
    TEST_CASE(Vpp_BootShutdown),
    TEST_CASE_NULL(),
};

TEST_SUITE(VppSuite,
           "Vpp Tests",
           VppSuiteInit,
           VppSuiteTerm,
           VppTestInit,
           VppTestTerm,
           VppTests);
