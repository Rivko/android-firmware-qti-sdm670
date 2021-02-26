/*!
 * @file test_ipc_service.cpp
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
#include <vendor/qti/hardware/vpp/1.1/IHidlVppCallbacks.h>

#include "dvpTest.h"
#include "dvpTest_tb.h"

#include "test_utils.h"
#include "buf_pool.h"

#define VPP_LOG_TAG     VPP_LOG_UT_IPC_SVC_TAG
#define VPP_LOG_MODULE  VPP_LOG_UT_IPC_SVC
#include "vpp_dbg.h"
#include "vpp.h"
#include "vpp_utils.h"
#include "vpp_uc.h"
#include "HidlVpp.h"
#include "HidlVppUtils.h"
#include "HidlVppCallbacks.h"
#include "VppClient.h"

using namespace android;
using ::vendor::qti::hardware::vpp::V1_1::IHidlVpp;
using ::vendor::qti::hardware::vpp::V1_1::implementation::HidlVpp;
using ::vendor::qti::hardware::vpp::V1_1::implementation::HidlVppCallbacks;
using ::vendor::qti::hardware::vpp::V1_1::IHidlVppCallbacks;
using ::vendor::qti::hardware::vpp::V1_1::HqvMode;
using ::vendor::qti::hardware::vpp::V1_1::HqvControlType;
using ::vendor::qti::hardware::vpp::V1_1::VppColorFormat;
using ::vendor::qti::hardware::vpp::V1_1::VppPort;
using ::vendor::qti::hardware::vpp::V1_1::VidPropType;
using ::vendor::qti::hardware::vpp::V1_1::VppCodecType;

/************************************************************************
 * Local definitions
 ***********************************************************************/
#define VPP_CLIENT_PTR                          0xDEADBEEF
#define SET_FLAG_MLOCKED(flag, setTo)           pthread_mutex_lock(&sMutex); \
                                                flag = setTo; \
                                                pthread_mutex_unlock(&sMutex);

#define VPP_SET_CTRL_MLOCKED(s, ctrl, cb)       SET_FLAG_MLOCKED(u32SetCtrlCbRx, VPP_FALSE); \
                                                s->vppSetCtrl(VPP_CLIENT_PTR, ctrl, cb); \
                                                pthread_mutex_lock(&sMutex); \
                                                while (u32SetCtrlCbRx != VPP_TRUE) \
                                                    pthread_cond_wait(&sCond, &sMutex); \
                                                pthread_mutex_unlock(&sMutex);

#define VPP_FLUSH_MLOCKED(s, flag, p)           SET_FLAG_MLOCKED(flag, VPP_FALSE); \
                                                s->vppFlush((VppPort)p); \
                                                pthread_mutex_lock(&sMutex); \
                                                while (flag != VPP_TRUE) \
                                                    pthread_cond_wait(&sCond, &sMutex); \
                                                pthread_mutex_unlock(&sMutex);

namespace vendor {
namespace qti {
namespace hardware {
namespace vpp {
namespace V1_1 {
namespace implementation {

struct TestVppCallbacks : public IHidlVppCallbacks
{
    TestVppCallbacks(VppClient *client);
    ~TestVppCallbacks();
    // Methods from ::vendor::qti::hardware::vpp::V1_1::IHidlVppCallbacks follow.
    Return<uint32_t> inputBufferDone(const VppBuffer& buf) override;
    Return<uint32_t> outputBufferDone(const VppBuffer& buf) override;
    Return<uint32_t> vppEvent(const VppEvent& e) override;

    // Methods from ::android::hidl::base::V1_0::IBase follow.
    VppClient *mClient;
};

}  // namespace implementation
}  // namespace V1_1
}  // namespace vpp
}  // namespace hardware
}  // namespace qti
}  // namespace vendor

using ::vendor::qti::hardware::vpp::V1_1::implementation::TestVppCallbacks;

/************************************************************************
 * Local static variables
 ***********************************************************************/
static pthread_cond_t sCond = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t sMutex = PTHREAD_MUTEX_INITIALIZER;

static uint32_t u32InputFlushRx;
static uint32_t u32OutputFlushRx;
static uint32_t u32ReconfigDoneRx;
static uint32_t u32SetCtrlCbRx;

static VppRequirements gVppReq;

/************************************************************************
 * Local Functions
 ***********************************************************************/
namespace vendor {
namespace qti {
namespace hardware {
namespace vpp {
namespace V1_1 {
namespace implementation {
TestVppCallbacks::TestVppCallbacks(VppClient *client) :
    mClient(client)
{
    LOGI("HidlVppCallbacks constructor");
}

TestVppCallbacks::~TestVppCallbacks()
{
    LOGI("HidlVppCallbacks destructor");
}

// Methods from ::vendor::qti::hardware::vpp::V1_1::IHidlVppCallbacks follow.
Return<uint32_t> TestVppCallbacks::inputBufferDone(const VppBuffer& buf)
{
    uint32_t u32 = VPP_OK;
    VPP_UNUSED(buf);
    return u32;
}

Return<uint32_t> TestVppCallbacks::outputBufferDone(const VppBuffer& buf)
{
    uint32_t u32 = VPP_OK;
    VPP_UNUSED(buf);
    return u32;
}

Return<uint32_t> TestVppCallbacks::vppEvent(const VppEvent& e)
{
    uint32_t u32 = VPP_OK;
    LOGI("%s(), event_type=%u", __func__, e.type);
    if (e.type == (VppEventType)VPP_EVENT_FLUSH_DONE)
    {
        pthread_mutex_lock(&sMutex);
        if (e.u.flushDone.port == (VppPort)VPP_PORT_INPUT)
        {
            u32InputFlushRx = VPP_TRUE;
        }
        else if (e.u.flushDone.port == (VppPort)VPP_PORT_OUTPUT)
        {
            u32OutputFlushRx = VPP_TRUE;
        }
        pthread_mutex_unlock(&sMutex);
        pthread_cond_signal(&sCond);
    }
    else if (e.type == (VppEventType)VPP_EVENT_RECONFIG_DONE)
    {
        pthread_mutex_lock(&sMutex);

        DVP_ASSERT_EQUAL(VPP_OK, e.u.reconfigDone.reconfigStatus);

        u32ReconfigDoneRx = VPP_TRUE;
        pthread_mutex_unlock(&sMutex);
        pthread_cond_signal(&sCond);
    }
    return u32;
}
}  // namespace implementation
}  // namespace V1_1
}  // namespace vpp
}  // namespace hardware
}  // namespace qti
}  // namespace vendor

/* Callback function to return the VppRequirements during setCtrl() call*/
void vppReq_cb(const VppRequirements& vppSetCtrlRet)
{
    pthread_mutex_lock(&sMutex);

    u32SetCtrlCbRx = VPP_TRUE;
    gVppReq = vppSetCtrlRet;

    pthread_mutex_unlock(&sMutex);
    pthread_cond_signal(&sCond);
}

 /************************************************************************
 * Global Functions
 ***********************************************************************/

/************************************************************************
 * Test Functions
 ***********************************************************************/
TEST_SUITE_INIT(vppSvcSuiteInit)
{
}

TEST_SUITE_TERM(vppSvcSuiteTerm)
{
}

TEST_SETUP(vppSvcTestInit)
{
}

TEST_CLEANUP(vppSvcTestTerm)
{
}

TEST(vppSvcCnstDest)
{
    std::vector<sp<IHidlVpp>> vppSessions;
    uint32_t i;

    for (i = 0; i < 10; i++)
    {
        try {
            sp<IHidlVpp> vppSession = new HidlVpp();
            DVP_ASSERT_PTR_NNULL(vppSession.get());
            vppSessions.push_back(vppSession);
        } catch (const std::exception& e) {
            LOGE("Exception was caught: %s", e.what());
            DVP_ASSERT_FAIL();
        } catch (...) {
            LOGE("Exception of an undetermined type");
            DVP_ASSERT_FAIL();
        }
    }

    try {
        vppSessions.clear();
    } catch (const std::exception& e) {
        LOGE("Exception was caught: %s", e.what());
        DVP_ASSERT_FAIL();
    } catch (...) {
        LOGE("Exception of an undetermined type");
        DVP_ASSERT_FAIL();
    }
}

TEST(vppSvcInitTerm)
{
    uint32_t u32, i;
    VppClient *client = (VppClient *)VPP_CLIENT_PTR;
    sp<IHidlVppCallbacks> cb = new HidlVppCallbacks(client);
    DVP_ASSERT_PTR_NNULL(cb.get());

    sp<IHidlVpp> vppSession = new HidlVpp();
    DVP_ASSERT_PTR_NNULL(vppSession.get());

    for (i = 0; i < 10; i++)
    {
        u32 = vppSession->vppInit(0, cb);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        vppSession->vppTerm();
    }
}

TEST(vppSvcSetCtrl)
{
    uint32_t u32;
    HqvControl ctrl;

    memset(&ctrl, 0, sizeof(struct HqvControl));

    VppClient *client = (VppClient *)VPP_CLIENT_PTR;
    sp<IHidlVppCallbacks> cb = new HidlVppCallbacks(client);
    DVP_ASSERT_PTR_NNULL(cb.get());

    sp<IHidlVpp> vppSession = new HidlVpp();
    DVP_ASSERT_PTR_NNULL(vppSession.get());

    VPP_SET_CTRL_MLOCKED(vppSession, ctrl, vppReq_cb);
    DVP_ASSERT_EQUAL(gVppReq.cookie, VPP_CLIENT_PTR);
    DVP_ASSERT_NEQUAL(gVppReq.retStatus, VPP_OK);

    u32 = vppSession->vppInit(0, cb);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // Set some invalid controls
    ctrl.mode = (HqvMode)902389;
    VPP_SET_CTRL_MLOCKED(vppSession, ctrl, vppReq_cb);
    DVP_ASSERT_EQUAL(gVppReq.cookie, VPP_CLIENT_PTR);
    DVP_ASSERT_NEQUAL(gVppReq.retStatus, VPP_OK);

    // Set to auto
    ctrl.mode = (HqvMode)HQV_MODE_AUTO;
    VPP_SET_CTRL_MLOCKED(vppSession, ctrl, vppReq_cb);
    DVP_ASSERT_EQUAL(gVppReq.cookie, VPP_CLIENT_PTR);
    DVP_ASSERT_EQUAL(gVppReq.retStatus, VPP_OK);

    // Set to off
    ctrl.mode = (HqvMode)HQV_MODE_OFF;
    VPP_SET_CTRL_MLOCKED(vppSession, ctrl, vppReq_cb);
    DVP_ASSERT_EQUAL(gVppReq.cookie, VPP_CLIENT_PTR);
    DVP_ASSERT_EQUAL(gVppReq.retStatus, VPP_OK);

    // Set to manual
    ctrl.mode = (HqvMode)HQV_MODE_MANUAL;
    ctrl.ctrlType = (HqvControlType)HQV_CONTROL_CADE;
    ctrl.u.cade.mode = (HqvMode)HQV_MODE_AUTO;
    VPP_SET_CTRL_MLOCKED(vppSession, ctrl, vppReq_cb);
    DVP_ASSERT_EQUAL(gVppReq.cookie, VPP_CLIENT_PTR);
    DVP_ASSERT_TRUE(gVppReq.retStatus == VPP_OK || gVppReq.retStatus == VPP_ERR_INVALID_CFG);

    vppSession->vppTerm();
}

TEST(vppSvcSetParam)
{
    uint32_t u32;
    VppPortParam param;

    memset(&param, 0, sizeof(struct VppPortParam));

    VppClient *client = (VppClient *)VPP_CLIENT_PTR;
    sp<IHidlVppCallbacks> cb = new HidlVppCallbacks(client);
    DVP_ASSERT_PTR_NNULL(cb.get());

    sp<IHidlVpp> vppSession = new HidlVpp();
    DVP_ASSERT_PTR_NNULL(vppSession.get());

    u32 = vppSession->vppInit(0, cb);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // Garbage input
    u32 = vppSession->vppSetParameter((VppPort)123198, param);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);
    u32 = vppSession->vppSetParameter((VppPort)123198, param);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    // Garbage parameters
    param.width = 0;
    param.height = 0;
    param.fmt = (VppColorFormat)VPP_COLOR_FORMAT_NV12_VENUS;

    // Setting input and output port will cause state transition
    u32 = vppSession->vppSetParameter((VppPort)VPP_PORT_INPUT, param);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    u32 = vppSession->vppSetParameter((VppPort)VPP_PORT_OUTPUT, param);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    // Valid input
    param.width = 1920;
    param.height = 1080;
    vppSession->vppTerm();

    // Reset and try again. Should error because we didn't set hqv controls
    // However, still will be put into active state
    u32 = vppSession->vppInit(0, cb);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    u32 = vppSession->vppSetParameter((VppPort)VPP_PORT_INPUT, param);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    u32 = vppSession->vppSetParameter((VppPort)VPP_PORT_OUTPUT, param);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    // Now in active state. Should reject any attempts to set params.
    param.width = 1280;
    param.height = 720;
    u32 = vppSession->vppSetParameter((VppPort)VPP_PORT_INPUT, param);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_STATE);
    u32 = vppSession->vppSetParameter((VppPort)VPP_PORT_OUTPUT, param);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_STATE);

    vppSession->vppTerm();
}

TEST(vppSvcQueueBuf)
{
    uint32_t u32;
    struct vpp_buffer memBuf;
    VppBuffer buf;

    // Buffer params needs to be initialized as these params are accessed in the HIDL layer
    memBuf.pvGralloc = nullptr;
    memBuf.pixel.fd = memBuf.extradata.fd = -1;

    vppBufferToHidl(&memBuf, buf, VPP_FALSE);

    VppClient *client = (VppClient *)VPP_CLIENT_PTR;
    sp<IHidlVppCallbacks> cb = new HidlVppCallbacks(client);
    DVP_ASSERT_PTR_NNULL(cb.get());

    sp<IHidlVpp> vppSession = new HidlVpp();
    DVP_ASSERT_PTR_NNULL(vppSession.get());

    u32 = vppSession->vppInit(0, cb);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // Garbage
    u32 = vppSession->vppQueueBuf((VppPort)9102831092, buf);
    DVP_ASSERT_NEQUAL(VPP_OK, u32);
    u32 = vppSession->vppQueueBuf((VppPort)VPP_PORT_INPUT, buf);
    DVP_ASSERT_NEQUAL(VPP_OK, u32);
    u32 = vppSession->vppQueueBuf((VppPort)VPP_PORT_OUTPUT, buf);
    DVP_ASSERT_NEQUAL(VPP_OK, u32);
    u32 = vppSession->vppQueueBuf((VppPort)9102831092, buf);
    DVP_ASSERT_NEQUAL(VPP_OK, u32);
    u32 = vppSession->vppQueueBuf((VppPort)VPP_PORT_INPUT, buf);
    DVP_ASSERT_NEQUAL(VPP_OK, u32);
    u32 = vppSession->vppQueueBuf((VppPort)VPP_PORT_OUTPUT, buf);
    DVP_ASSERT_NEQUAL(VPP_OK, u32);

    vppSession->vppTerm();
}

TEST(vppSvcFlush)
{
    uint32_t u32;

    VppClient *client = (VppClient *)VPP_CLIENT_PTR;
    sp<IHidlVppCallbacks> cb = new TestVppCallbacks(client);
    DVP_ASSERT_PTR_NNULL(cb.get());

    sp<IHidlVpp> vppSession = new HidlVpp();
    DVP_ASSERT_PTR_NNULL(vppSession.get());

    HqvControl ctrl = {
        .mode = (HqvMode)HQV_MODE_AUTO,
        .ctrlType = (HqvControlType)HQV_CONTROL_NONE,
    };

    VppPortParam param = {
        .width = 1920,
        .height = 1080,
        .stride = 1920,
        .scanlines = 1088,
        .fmt = (VppColorFormat)VPP_COLOR_FORMAT_NV12_VENUS,
    };

    LOGI("%s: w/h = %u/%u", __func__, param.width, param.height);

    // Garbage inputs
    u32 = vppSession->vppFlush((VppPort)9210834);
    DVP_ASSERT_NEQUAL(VPP_OK, u32);
    u32 = vppSession->vppFlush((VppPort)VPP_PORT_INPUT);
    DVP_ASSERT_NEQUAL(VPP_OK, u32);
    u32 = vppSession->vppFlush((VppPort)VPP_PORT_OUTPUT);
    DVP_ASSERT_NEQUAL(VPP_OK, u32);
    u32 = vppSession->vppFlush((VppPort)10989123);
    DVP_ASSERT_NEQUAL(VPP_OK, u32);

    // Transition to inited state
    u32 = vppSession->vppInit(0, cb);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // Invalid state
    u32 = vppSession->vppFlush((VppPort)VPP_PORT_INPUT);
    DVP_ASSERT_NEQUAL(VPP_OK, u32);
    u32 = vppSession->vppFlush((VppPort)VPP_PORT_OUTPUT);
    DVP_ASSERT_NEQUAL(VPP_OK, u32);

    // Transition to active valid state
    VPP_SET_CTRL_MLOCKED(vppSession, ctrl, vppReq_cb);
    DVP_ASSERT_EQUAL(gVppReq.cookie, VPP_CLIENT_PTR);
    DVP_ASSERT_EQUAL(gVppReq.retStatus, VPP_OK);

    u32 = vppSession->vppSetParameter((VppPort)VPP_PORT_INPUT, param);
    DVP_ASSERT_EQUAL(VPP_OK, u32);
    u32 = vppSession->vppSetParameter((VppPort)VPP_PORT_OUTPUT, param);
    DVP_ASSERT_EQUAL(VPP_OK, u32);

    // Valid state. No buffers. Should expect flush done events
    // Input
    VPP_FLUSH_MLOCKED(vppSession, u32InputFlushRx, VPP_PORT_INPUT);

    // Output
    VPP_FLUSH_MLOCKED(vppSession, u32OutputFlushRx, VPP_PORT_OUTPUT);

    vppSession->vppTerm();
}

TEST(vppSvcReconfigBasic)
{
    uint32_t u32;

    VppClient *client = (VppClient *)VPP_CLIENT_PTR;
    sp<IHidlVppCallbacks> cb = new TestVppCallbacks(client);
    DVP_ASSERT_PTR_NNULL(cb.get());

    sp<IHidlVpp> vppSession = new HidlVpp();
    DVP_ASSERT_PTR_NNULL(vppSession.get());

    HqvControl ctrl = {
        .mode = (HqvMode)HQV_MODE_AUTO,
        .ctrlType = (HqvControlType)HQV_CONTROL_NONE,
    };

    VppPortParam param = {
        .width = 1920,
        .height = 1080,
        .stride = 1920,
        .scanlines = 1088,
        .fmt = (VppColorFormat)VPP_COLOR_FORMAT_NV12_VENUS,
    };

    // Error state
    u32 = vppSession->vppReconfigure(param, param);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);

    // Transition to inited state
    u32 = vppSession->vppInit(0, cb);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // Should not be able to reconfigure in INITED state
    u32 = vppSession->vppReconfigure(param, param);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_STATE);

    // Transition to active state
    VPP_SET_CTRL_MLOCKED(vppSession, ctrl, vppReq_cb);
    DVP_ASSERT_EQUAL(gVppReq.cookie, VPP_CLIENT_PTR);
    DVP_ASSERT_EQUAL(gVppReq.retStatus, VPP_OK);

    u32 = vppSession->vppSetParameter((VppPort)VPP_PORT_INPUT, param);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    u32 = vppSession->vppSetParameter((VppPort)VPP_PORT_OUTPUT, param);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // Request reconfigure with no buffers having been queued
    param.width = 720;
    param.height = 576;
    param.stride = 768;
    param.scanlines = 576;
    u32 = vppSession->vppReconfigure(param, param);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    pthread_mutex_lock(&sMutex);
    while (u32ReconfigDoneRx != VPP_TRUE)
        pthread_cond_wait(&sCond, &sMutex);
    pthread_mutex_unlock(&sMutex);

    vppSession->vppTerm();
}

TEST(vppSvcSetVidProp)
{
    uint32_t u32;
    VideoProperty ctrl_set;

    VppClient *client = (VppClient *)VPP_CLIENT_PTR;
    sp<IHidlVppCallbacks> cb = new HidlVppCallbacks(client);
    DVP_ASSERT_PTR_NNULL(cb.get());

    sp<IHidlVpp> vppSession = new HidlVpp();
    DVP_ASSERT_PTR_NNULL(vppSession.get());

    u32 = vppSession->vppInit(VPP_SESSION_NON_REALTIME, cb);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // Valid set
    ctrl_set.propertyType = (VidPropType)VID_PROP_CODEC;
    ctrl_set.u.codec.eCodec = (VppCodecType)VPP_CODEC_TYPE_H263;
    u32 = vppSession->vppSetVidProp(ctrl_set);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // Invalid property type set
    ctrl_set.propertyType = (VidPropType)VID_PROP_MAX;
    u32 = vppSession->vppSetVidProp(ctrl_set);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    // Valid set codec out of range
    ctrl_set.propertyType = (VidPropType)VID_PROP_CODEC;
    ctrl_set.u.codec.eCodec = (VppCodecType)0xFFFFFFFF;
    u32 = vppSession->vppSetVidProp(ctrl_set);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // Valid set
    ctrl_set.propertyType = (VidPropType)VID_PROP_NON_REALTIME;
    ctrl_set.u.nonRealtime.bNonRealtime = 0;
    u32 = vppSession->vppSetVidProp(ctrl_set);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // Valid set operating rate
    ctrl_set.propertyType = (VidPropType)VID_PROP_OPERATING_RATE;
    ctrl_set.u.operatingRate.u32OperatingRate = 0xFFFFFFFF;
    u32 = vppSession->vppSetVidProp(ctrl_set);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    vppSession->vppTerm();
}

/************************************************************************
 * Global Functions
 ***********************************************************************/
TEST_CASES vppSvcTests[] = {
    TEST_CASE(vppSvcCnstDest),
    TEST_CASE(vppSvcInitTerm),
    TEST_CASE(vppSvcSetCtrl),
    TEST_CASE(vppSvcSetParam),
    TEST_CASE(vppSvcQueueBuf),
    TEST_CASE(vppSvcFlush),
    TEST_CASE(vppSvcReconfigBasic),
    TEST_CASE(vppSvcSetVidProp),
    TEST_CASE_NULL(),
};

TEST_SUITE(vppSvcSuite,
           "VppServiceTests",
           vppSvcSuiteInit,
           vppSvcSuiteTerm,
           vppSvcTestInit,
           vppSvcTestTerm,
           vppSvcTests);