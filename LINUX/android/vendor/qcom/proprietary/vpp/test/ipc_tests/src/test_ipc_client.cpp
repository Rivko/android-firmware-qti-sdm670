/*!
 * @file test_ipc_client.cpp
 *
 * @cr
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.

 * @services
 */

#include <sys/types.h>
#include <inttypes.h>
#include <string.h>
#include <stdio.h>

#include "dvpTest.h"
#include "dvpTest_tb.h"

#include "test_utils.h"
#include "buf_pool.h"

#define VPP_LOG_TAG     VPP_LOG_UT_IPC_CLIENT_TAG
#define VPP_LOG_MODULE  VPP_LOG_UT_IPC_CLIENT
#include "vpp_dbg.h"
#include "vpp.h"
#include "vpp_utils.h"
#include "vpp_uc.h"
#include "VppClient.h"

using namespace android;

/************************************************************************
 * Local definitions
 ***********************************************************************/

/************************************************************************
 * Local static variables
 ***********************************************************************/
static pthread_cond_t sCond = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t sMutex = PTHREAD_MUTEX_INITIALIZER;

static uint32_t u32InputFlushRx;
static uint32_t u32OutputFlushRx;
static uint32_t u32ReconfigDoneRx;

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

static struct vpp_callbacks sCb = {
    .input_buffer_done = vpp_ibd_cb,
    .output_buffer_done = vpp_obd_cb,
    .vpp_event = vpp_evt_cb,
};

/************************************************************************
 * Test Functions
 ***********************************************************************/
TEST_SUITE_INIT(vppClientSuiteInit)
{
}

TEST_SUITE_TERM(vppClientSuiteTerm)
{
}

TEST_SETUP(vppClientTestInit)
{
}

TEST_CLEANUP(vppClientTestTerm)
{
}

TEST(vppClientCnstDest)
{
    std::vector<sp<VppClient>> clients;
    uint32_t i;

    for (i = 0; i < 10; i++)
    {
        try {
            sp<VppClient> vppClient = new VppClient();
            DVP_ASSERT_PTR_NNULL(vppClient.get());
            clients.push_back(vppClient);
        } catch (const std::exception& e) {
            LOGE("Exception was caught: %s", e.what());
            DVP_ASSERT_FAIL();
        } catch (...) {
            LOGE("Exception of an undetermined type");
            DVP_ASSERT_FAIL();
        }
    }

    try {
        clients.clear();
    } catch (const std::exception& e) {
        LOGE("Exception was caught: %s", e.what());
        DVP_ASSERT_FAIL();
    } catch (...) {
        LOGE("Exception of an undetermined type");
        DVP_ASSERT_FAIL();
    }
}

TEST(vppClientSetCtrl)
{
    uint32_t u32;
    struct hqv_control ctrl;
    struct vpp_requirements req;
    void *p;

    memset(&ctrl, 0, sizeof(struct hqv_control));

    sp<VppClient> vppClient = new VppClient();
    DVP_ASSERT_PTR_NNULL(vppClient.get());

    u32 = vppClient->setCtrl(ctrl, &req);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    p = vppClient->init(0, sCb);
    DVP_ASSERT_PTR_NNULL(p);

    // Set some invalid controls
    ctrl.mode = (enum hqv_mode)902389;
    u32 = vppClient->setCtrl(ctrl, &req);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    // Set to auto
    ctrl.mode = HQV_MODE_AUTO;
    u32 = vppClient->setCtrl(ctrl, &req);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // Set to off
    ctrl.mode = HQV_MODE_OFF;
    u32 = vppClient->setCtrl(ctrl, &req);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // Set to manual
    ctrl.mode = HQV_MODE_MANUAL;
    ctrl.ctrl_type = HQV_CONTROL_CADE;
    ctrl.cade.mode = HQV_MODE_AUTO;
    u32 = vppClient->setCtrl(ctrl, &req);
    DVP_ASSERT_TRUE(u32 == VPP_OK || u32 == VPP_ERR_INVALID_CFG);

    vppClient->term();
}

TEST(vppClientSetParam)
{
    uint32_t u32;
    struct vpp_port_param param;
    void *p;

    memset(&param, 0, sizeof(struct vpp_port_param));

    sp<VppClient> vppClient = new VppClient();
    DVP_ASSERT_PTR_NNULL(vppClient.get());

    p = vppClient->init(0, sCb);
    DVP_ASSERT_PTR_NNULL(p);

    // Garbage input
    u32 = vppClient->setParameter((enum vpp_port)123198, param);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);
    u32 = vppClient->setParameter((enum vpp_port)123198, param);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    // Garbage parameters
    param.width = 0;
    param.height = 0;
    param.fmt = VPP_COLOR_FORMAT_NV12_VENUS;

    // Setting input and output port will cause state transition
    u32 = vppClient->setParameter(VPP_PORT_INPUT, param);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    u32 = vppClient->setParameter(VPP_PORT_OUTPUT, param);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    // Valid input
    param.width = 1920;
    param.height = 1080;
    vppClient->term();

    // Reset and try again. Should error because we didn't set hqv controls
    // However, still will be put into active state
    p = vppClient->init(0, sCb);
    DVP_ASSERT_PTR_NNULL(p);
    u32 = vppClient->setParameter(VPP_PORT_INPUT, param);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    u32 = vppClient->setParameter(VPP_PORT_OUTPUT, param);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    // Now in active state. Should reject any attempts to set params.
    param.width = 1280;
    param.height = 720;
    u32 = vppClient->setParameter(VPP_PORT_INPUT, param);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_STATE);
    u32 = vppClient->setParameter(VPP_PORT_OUTPUT, param);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_STATE);

    vppClient->term();
}

TEST(vppClientQueueBuf)
{
    uint32_t u32;
    void *p;
    struct vpp_buffer buf;
    struct vpp_buffer *pBuf = &buf;

    /* Buffer params needs to be initialized as these params are accessed in the HIDL layer*/
    pBuf->pvGralloc = nullptr;
    pBuf->pixel.fd = pBuf->extradata.fd = -1;

    sp<VppClient> vppClient = new VppClient();
    DVP_ASSERT_PTR_NNULL(vppClient.get());

    p = vppClient->init(0, sCb);
    DVP_ASSERT_PTR_NNULL(p);

    // Garbage
    u32 = vppClient->queueBuf((enum vpp_port)9102831092, NULL);
    DVP_ASSERT_NEQUAL(VPP_OK, u32);
    u32 = vppClient->queueBuf(VPP_PORT_INPUT, NULL);
    DVP_ASSERT_NEQUAL(VPP_OK, u32);
    u32 = vppClient->queueBuf(VPP_PORT_OUTPUT, NULL);
    DVP_ASSERT_NEQUAL(VPP_OK, u32);
    u32 = vppClient->queueBuf(VPP_PORT_INPUT, pBuf);
    DVP_ASSERT_NEQUAL(VPP_OK, u32);
    u32 = vppClient->queueBuf(VPP_PORT_OUTPUT, pBuf);
    DVP_ASSERT_NEQUAL(VPP_OK, u32);
    u32 = vppClient->queueBuf((enum vpp_port)9102831092, NULL);
    DVP_ASSERT_NEQUAL(VPP_OK, u32);
    u32 = vppClient->queueBuf(VPP_PORT_INPUT, NULL);
    DVP_ASSERT_NEQUAL(VPP_OK, u32);
    u32 = vppClient->queueBuf(VPP_PORT_OUTPUT, NULL);
    DVP_ASSERT_NEQUAL(VPP_OK, u32);

    // Valid input, invalid state
    u32 = vppClient->queueBuf(VPP_PORT_INPUT, pBuf);
    DVP_ASSERT_NEQUAL(VPP_OK, u32);
    u32 = vppClient->queueBuf(VPP_PORT_OUTPUT, pBuf);
    DVP_ASSERT_NEQUAL(VPP_OK, u32);

    vppClient->term();
}

TEST(vppClientFlush)
{
    uint32_t u32;
    struct hqv_control ctrl;
    struct vpp_requirements req;
    struct vpp_port_param param;
    void *p;

    sp<VppClient> vppClient = new VppClient();
    DVP_ASSERT_PTR_NNULL(vppClient.get());

    ctrl.mode = HQV_MODE_AUTO;
    ctrl.ctrl_type = HQV_CONTROL_NONE;
    param.width = 1920;
    param.height = 1080;
    param.stride = 1920;
    param.scanlines = 1088;
    param.fmt = VPP_COLOR_FORMAT_NV12_VENUS;

    LOGI("%s: w/h = %u/%u", __func__, param.width, param.height);

    p = vppClient->init(0, sCb);
    DVP_ASSERT_PTR_NNULL(p);

    // Garbage inputs
    u32 = vppClient->flush((enum vpp_port)9210834);
    DVP_ASSERT_NEQUAL(VPP_OK, u32);
    u32 = vppClient->flush(VPP_PORT_INPUT);
    DVP_ASSERT_NEQUAL(VPP_OK, u32);
    u32 = vppClient->flush(VPP_PORT_OUTPUT);
    DVP_ASSERT_NEQUAL(VPP_OK, u32);
    u32 = vppClient->flush((enum vpp_port)10989123);
    DVP_ASSERT_NEQUAL(VPP_OK, u32);

    // Invalid state
    u32 = vppClient->flush(VPP_PORT_INPUT);
    DVP_ASSERT_NEQUAL(VPP_OK, u32);
    u32 = vppClient->flush(VPP_PORT_OUTPUT);
    DVP_ASSERT_NEQUAL(VPP_OK, u32);

    // Transition to active valid state
    u32 = vppClient->setCtrl(ctrl, &req);
    DVP_ASSERT_EQUAL(VPP_OK, u32);
    u32 = vppClient->setParameter(VPP_PORT_INPUT, param);
    DVP_ASSERT_EQUAL(VPP_OK, u32);
    u32 = vppClient->setParameter(VPP_PORT_OUTPUT, param);
    DVP_ASSERT_EQUAL(VPP_OK, u32);

    // Valid state. No buffers. Should expect flush done events
    // Input
    pthread_mutex_lock(&sMutex);
    u32InputFlushRx = VPP_FALSE;
    pthread_mutex_unlock(&sMutex);
    u32 = vppClient->flush(VPP_PORT_INPUT);
    DVP_ASSERT_EQUAL(VPP_OK, u32);
    pthread_mutex_lock(&sMutex);
    while (u32InputFlushRx != VPP_TRUE)
        pthread_cond_wait(&sCond, &sMutex);
    pthread_mutex_unlock(&sMutex);

    // Output
    pthread_mutex_lock(&sMutex);
    u32OutputFlushRx = VPP_FALSE;
    pthread_mutex_unlock(&sMutex);
    u32 = vppClient->flush(VPP_PORT_OUTPUT);
    DVP_ASSERT_EQUAL(VPP_OK, u32);
    pthread_mutex_lock(&sMutex);
    while (u32OutputFlushRx != VPP_TRUE)
        pthread_cond_wait(&sCond, &sMutex);
    pthread_mutex_unlock(&sMutex);

    vppClient->term();
}

TEST(vppClientReconfigBasic)
{
    uint32_t u32;
    void *p = NULL;
    struct vpp_requirements req;

    sp<VppClient> vppClient = new VppClient();
    DVP_ASSERT_PTR_NNULL(vppClient.get());

    struct hqv_control ctrl = {
        .mode = HQV_MODE_AUTO,
        .ctrl_type = HQV_CONTROL_NONE,
    };

    struct vpp_port_param param = {
        .width = 1920,
        .height = 1080,
        .stride = 1920,
        .scanlines = 1088,
        .fmt = VPP_COLOR_FORMAT_NV12_VENUS,
    };

    // Error state
    u32 = vppClient->reconfigure(param, param);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);

    // Transition to inited state
    p = vppClient->init(0, sCb);
    DVP_ASSERT_PTR_NNULL(p);

    // Should not be able to reconfigure in INITED state
    u32 = vppClient->reconfigure(param, param);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_STATE);

    // Transition to active state
    u32 = vppClient->setCtrl(ctrl, &req);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    u32 = vppClient->setParameter(VPP_PORT_INPUT, param);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    u32 = vppClient->setParameter(VPP_PORT_OUTPUT, param);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // Request reconfigure with no buffers having been queued
    param.width = 720;
    param.height = 576;
    param.stride = 768;
    param.scanlines = 576;
    u32 = vppClient->reconfigure(param, param);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    pthread_mutex_lock(&sMutex);
    while (u32ReconfigDoneRx != VPP_TRUE)
        pthread_cond_wait(&sCond, &sMutex);
    pthread_mutex_unlock(&sMutex);

    vppClient->term();
}

TEST(vppClientSetVidProp)
{
    uint32_t u32;
    void *p = NULL;
    struct video_property ctrl_set;

    sp<VppClient> vppClient = new VppClient();
    DVP_ASSERT_PTR_NNULL(vppClient.get());

    p = vppClient->init(VPP_SESSION_NON_REALTIME, sCb);
    DVP_ASSERT_PTR_NNULL(p);

    // Valid set
    ctrl_set.property_type = VID_PROP_CODEC;
    ctrl_set.codec.eCodec = VPP_CODEC_TYPE_H263;
    u32 = vppClient->setVidProp( ctrl_set);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // Invalid property type set
    ctrl_set.property_type = VID_PROP_MAX;
    u32 = vppClient->setVidProp(ctrl_set);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    // Valid set codec out of range
    ctrl_set.property_type = VID_PROP_CODEC;
    ctrl_set.codec.eCodec = (enum vpp_codec_type)0xFFFFFFFF;
    u32 = vppClient->setVidProp(ctrl_set);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // Valid set
    ctrl_set.property_type = VID_PROP_NON_REALTIME;
    ctrl_set.non_realtime.bNonRealtime = 0;
    u32 = vppClient->setVidProp(ctrl_set);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // Valid set operating rate
    ctrl_set.property_type = VID_PROP_OPERATING_RATE;
    ctrl_set.operating_rate.u32OperatingRate = 0xFFFFFFFF;
    u32 = vppClient->setVidProp(ctrl_set);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    vppClient->term();
}

/************************************************************************
 * Global Functions
 ***********************************************************************/
TEST_CASES vppClientTests[] = {
    TEST_CASE(vppClientCnstDest),
    TEST_CASE(vppClientSetParam),
    TEST_CASE(vppClientSetCtrl),
    TEST_CASE(vppClientQueueBuf),
    TEST_CASE(vppClientFlush),
    TEST_CASE(vppClientReconfigBasic),
    TEST_CASE(vppClientSetVidProp),
    TEST_CASE_NULL(),
};

TEST_SUITE(vppClientSuite,
           "VppClientTests",
           vppClientSuiteInit,
           vppClientSuiteTerm,
           vppClientTestInit,
           vppClientTestTerm,
           vppClientTests);