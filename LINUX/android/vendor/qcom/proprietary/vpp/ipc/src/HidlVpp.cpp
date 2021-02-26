/*!
 * @file HidlVpp.cpp
 *
 * @cr
 * Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * @services
 */

#include <sys/types.h>
#include <inttypes.h>

#define VPP_LOG_TAG     VPP_LOG_IPC_SERVICE_TAG
#define VPP_LOG_MODULE  VPP_LOG_IPC_SERVICE
#include "vpp_dbg.h"
#include "HidlVpp.h"
#include "HidlVppUtils.h"

namespace vendor {
namespace qti {
namespace hardware {
namespace vpp {
namespace V1_1 {
namespace implementation {

enum {
    // The time taken for the vpp init
    PL_VPP_INIT,
    // The time taken for the vpp term
    PL_VPP_TERM,
    // The time taken for the vpp set_ctrl
    PL_VPP_SET_CTRL,
    // The time taken for the vpp set_param
    PL_VPP_SET_PARAM,
    // The time taken for the vpp input quebuf
    PL_VPP_QUEUE_BUF_IN,
    // The time taken for the vpp output quebuf
    PL_VPP_QUEUE_BUF_OUT,
    // The time taken for the vpp quebuf
    PL_VPP_RECONFIG,
    // The time taken for the vpp flush
    PL_VPP_FLUSH,
    // The time taken for the vpp set_vid_prop
    PL_VPP_SET_VID_PROP,
    PL_VPP_STATS_MAX,
};

// Configure characteristics for each individual stat
static const t_StVppStatsConfig astVppLibStatsCfg[] = {
    VPP_PROF_DECL(PL_VPP_INIT, 1, 1),
    VPP_PROF_DECL(PL_VPP_TERM, 1, 1),
    VPP_PROF_DECL(PL_VPP_SET_CTRL, 1, 1),
    VPP_PROF_DECL(PL_VPP_SET_PARAM, 1, 1),
    VPP_PROF_DECL(PL_VPP_QUEUE_BUF_IN, 1, 1),
    VPP_PROF_DECL(PL_VPP_QUEUE_BUF_OUT, 1, 1),
    VPP_PROF_DECL(PL_VPP_RECONFIG, 1, 1),
    VPP_PROF_DECL(PL_VPP_FLUSH, 1, 1),
    VPP_PROF_DECL(PL_VPP_SET_VID_PROP, 1, 1)
};

static const uint32_t u32VppLibStatsCnt = VPP_STATS_CNT(astVppLibStatsCfg);

void HidlVpp::HidlVppDeathRecipient::serviceDied(uint64_t cookie, const wp<IBase>& who)
{
    VPP_UNUSED(who);

    LOGE("callback client died, for c=%" PRIu64, cookie);

    if (mHidlVpp)
    {
        Mutex::Autolock l(mHidlVpp->mDeathMutex);
        mHidlVpp->mClientDied = true;
    }
}

HidlVpp::HidlVpp() : pvCtx(NULL),
    mClientDied(false),
    mClientDeathNotification(new HidlVppDeathRecipient(this))
{
    uint32_t i, u32StatsEn, u32Ret = VPP_OK;

    LOGI("HidlVpp constructor called, this=%p", this);

    memset(mVppBuffers, 0, sizeof(struct vpp_buffer) * VPP_BUF_POOL_MAXSIZE);

    for (i = 0; i < VPP_BUF_POOL_MAXSIZE; i++)
        mBufPool.push_front(&mVppBuffers[i]);

    // Read properties, debug-controls
    android::readProperties(&u32StatsEn);

    mStatHandle = NULL;
    memset(&mStCtx, 0, sizeof(mStCtx));
    u32Ret = u32VppStats_Init(&mStCtx);
    if (u32Ret == VPP_OK)
    {
        u32Ret = u32VppStats_SetEnable(&mStCtx, u32StatsEn);
        LOGE_IF(u32Ret != VPP_OK, "u32VppStats_SetEnable returned error %u", u32Ret);

        // Register the calls or stats
        u32Ret = u32VppStats_Register(&mStCtx, astVppLibStatsCfg, u32VppLibStatsCnt, &mStatHandle);
        LOGE_IF(u32Ret != VPP_OK, "u32VppStats_Register returned error %u", u32Ret);
    }
    else
        LOGE("u32VppStats_Init returned error %u", u32Ret);
}

HidlVpp::~HidlVpp()
{
    uint32_t u32Ret = VPP_OK;

    {
        Mutex::Autolock l(mDeathMutex);
        mClientDied = true;
    }

    LOGI("HidlVpp destructor called for session w/ this=%p", this);
    if (pvCtx != NULL)
    {
        vpp_term(pvCtx);
        pvCtx = NULL;
    }

    if (mStatHandle)
    {
         // Unregister the stats from the module
         u32Ret = u32VppStats_Unregister(&mStCtx, mStatHandle);
         LOGE_IF(u32Ret != VPP_OK, "u32VppStats_Unregister failed: %u", u32Ret);

         // Terminate the vpp stats module
         u32Ret = u32VppStats_Term(&mStCtx);
         LOGE_IF(u32Ret != VPP_OK, "u32VppStats_Term failed: %u", u32Ret);
    }
}

// Methods from ::vendor::qti::hardware::vpp::V1_1::IHidlVpp follow.
Return<uint32_t> HidlVpp::vppInit(uint32_t flags, const sp<IHidlVppCallbacks>& cb)
{
    struct vpp_callbacks stCallbacks;

    stCallbacks.pv = static_cast<void *>(this);
    stCallbacks.input_buffer_done = vppIBD;
    stCallbacks.output_buffer_done = vppOBD;
    stCallbacks.vpp_event = vppEVT;

    mHidlCb = cb;
    mHidlCb->linkToDeath(mClientDeathNotification, 0);

    u32VppStats_Start(&mStCtx, mStatHandle, PL_VPP_INIT);
    pvCtx = vpp_init(flags, stCallbacks);
    u32VppStats_Stop(&mStCtx, mStatHandle, PL_VPP_INIT);

    if (!pvCtx)
    {
        LOGE("ERROR: vpp_init returned null");
        return VPP_ERR;
    }
    LOGI("this=%p, vpp_init returned ctx=%p", this, pvCtx);

    return VPP_OK;
}

Return<void> HidlVpp::vppTerm()
{
    if (pvCtx != NULL)
    {
        u32VppStats_Start(&mStCtx, mStatHandle, PL_VPP_TERM);
        vpp_term(pvCtx);
        u32VppStats_Stop(&mStCtx, mStatHandle, PL_VPP_TERM);

        pvCtx = NULL;
    }
    if (mHidlCb != NULL) {
        mHidlCb->unlinkToDeath(mClientDeathNotification);
        mHidlCb = NULL;
    }
    return Void();
}

Return<void> HidlVpp::vppSetCtrl(uint64_t cookie, const HqvControl& ctrl, vppSetCtrl_cb _hidl_cb)
{
    struct hqv_control stHqvCtrl;
    struct vpp_requirements stVppReq;
    uint32_t u32 = VPP_OK;
    VppRequirements retReq;

    u32 = hidlToHqvCtrl(stHqvCtrl, ctrl);
    if (u32 != VPP_OK)
        LOGE("ERROR: from hidlToHqvCtrl Err=%u", u32);
    else
    {
        u32VppStats_Start(&mStCtx, mStatHandle, PL_VPP_SET_CTRL);
        u32 = vpp_set_ctrl(pvCtx, stHqvCtrl, &stVppReq);
        u32VppStats_Stop(&mStCtx, mStatHandle, PL_VPP_SET_CTRL);

        if (u32 != VPP_OK)
            LOGE("vpp_set_ctrl returned error, u32=%u", u32);
        else
        {
            u32 = vppReqToHidl(&stVppReq, retReq);
            LOGE_IF(u32 != VPP_OK, "ERROR: from vppReqToHidl Err=%u", u32);
        }
    }

    retReq.retStatus = u32;
    retReq.cookie = cookie;
    // For Hidl, multiple params need to returned using the callback
    _hidl_cb(retReq);

    return Void();
}

Return<uint32_t> HidlVpp::vppSetParameter(VppPort port, const VppPortParam& param)
{
    struct vpp_port_param stParam;
    uint32_t u32 = VPP_OK;

    hidlToVppPortParam(stParam, param);

    u32VppStats_Start(&mStCtx, mStatHandle, PL_VPP_SET_PARAM);
    u32 = vpp_set_parameter(pvCtx, (vpp_port)port, stParam);
    u32VppStats_Stop(&mStCtx, mStatHandle, PL_VPP_SET_PARAM);
    LOGE_IF(u32 != VPP_OK, "vpp_set_parameter returned error, u32=%u", u32);

    return u32;
}

Return<uint32_t> HidlVpp::vppQueueBuf(VppPort port, const VppBuffer& buf)
{
    struct vpp_buffer *pstExtBuf = NULL;
    uint32_t u32 = VPP_OK, u32Key;

    Mutex::Autolock a(mMutex);
    if (mBufPool.size() > 0)
    {
        pstExtBuf = mBufPool.back();
        mBufPool.pop_back();
    }

    VPP_RET_IF_NULL(pstExtBuf, VPP_ERR);

    u32 = hidlToVppBuffer(pstExtBuf, buf);
    if (u32 != VPP_OK)
    {
        LOGE("hidlToVppBuffer returned error %u", u32);
        return u32;
    }

    u32Key = (port == VppPort::VPP_PORT_INPUT) ? PL_VPP_QUEUE_BUF_IN : PL_VPP_QUEUE_BUF_OUT;

    u32VppStats_Start(&mStCtx, mStatHandle, u32Key);
    u32 = vpp_queue_buf(pvCtx, (enum vpp_port)port, pstExtBuf);
    u32VppStats_Stop(&mStCtx, mStatHandle, u32Key);

    LOGE_IF(u32 != VPP_OK, "vpp_queue_buf(%u) returned error, u32=%u", port, u32);

    return u32;
}

Return<uint32_t> HidlVpp::vppReconfigure(const VppPortParam& inputParam, const VppPortParam& outputParam)
{
    vpp_port_param stIpParam, stOutParam;
    uint32_t u32 = VPP_OK;

    hidlToVppPortParam(stIpParam, inputParam);
    hidlToVppPortParam(stOutParam, outputParam);

    u32VppStats_Start(&mStCtx, mStatHandle, PL_VPP_RECONFIG);
    u32 = vpp_reconfigure(pvCtx, stIpParam, stOutParam);
    u32VppStats_Stop(&mStCtx, mStatHandle, PL_VPP_RECONFIG);

    LOGE_IF(u32 != VPP_OK, "vpp_reconfigure returned error, u32=%u", u32);

    return u32;
}

Return<uint32_t> HidlVpp::vppFlush(VppPort port)
{
    uint32_t u32 = VPP_OK;

    u32VppStats_Start(&mStCtx, mStatHandle, PL_VPP_FLUSH);
    u32 = vpp_flush(pvCtx, (enum vpp_port)port);
    u32VppStats_Stop(&mStCtx, mStatHandle, PL_VPP_FLUSH);

    LOGE_IF(u32 != VPP_OK, "vpp_flush returned error, u32=%u", u32);

    return u32;
}

Return<uint32_t> HidlVpp::vppSetVidProp(const VideoProperty& prop)
{
    struct video_property stProp;
    uint32_t u32 = VPP_OK;

    u32 = hidlToVppProp(stProp, prop);
    if (u32 != VPP_OK)
    {
        LOGE("hidlToVppProp returned error %u", u32);
        return u32;
    }

    u32VppStats_Start(&mStCtx, mStatHandle, PL_VPP_SET_VID_PROP);
    u32 = vpp_set_vid_prop(pvCtx, stProp);
    u32VppStats_Stop(&mStCtx, mStatHandle, PL_VPP_SET_VID_PROP);

    LOGE_IF(u32 != VPP_OK, "vpp_set_vid_prop returned error, u32=%u", u32);

    return u32;
}

uint32_t HidlVpp::vppBufferDone(void *pv, struct vpp_buffer *buf, enum vpp_port ePort)
{
    VppBuffer stVppBuf;
    uint32_t u32 = VPP_OK;
    Return<uint32_t> r = VPP_OK;
    const char *w = ePort == VPP_PORT_INPUT ? "i" : "o";

    if (!pv || !buf || (ePort >= VPP_PORT_MAX))
    {
        LOGE("ERROR: pv=%p, buf=%p port=%u", pv, buf, (uint32_t)ePort);
        return VPP_ERR_PARAM;
    }

    HidlVpp *vpp = static_cast<HidlVpp *>(pv);

    u32 = vppBufferToHidl(buf, stVppBuf, VPP_TRUE);

    {
        Mutex::Autolock a(vpp->mMutex);
        vpp->mBufPool.push_front(buf);
    }

    if (u32 != VPP_OK)
    {
        LOGE("vppBufferToHidl failed, u32=%u", u32);
        return u32;
    }

    Mutex::Autolock l(vpp->mDeathMutex);

    if (vpp->mClientDied)
    {
        LOGE("client is dead, not issuing callback!");
        return VPP_ERR;
    }

    if (ePort == VPP_PORT_INPUT)
        r = vpp->mHidlCb->inputBufferDone(stVppBuf);
    else
        r = vpp->mHidlCb->outputBufferDone(stVppBuf);

    u32 = r.isOk() ? static_cast<uint32_t>(r) : VPP_ERR;
    LOGE_IF(!r.isOk(), "Binder callback failed for %sbd", w);
    LOGE_IF(u32 != VPP_OK, "%sbd failed, u32=%u", w, u32);

    return u32;
}

void HidlVpp::vppIBD(void *pv, struct vpp_buffer *buf)
{
    if (!pv || !buf)
    {
        LOGE("ERROR: pv=%p buf=%p", pv, buf);
        return;
    }

    LOGI("got ibd for HidlVpp instance = %p, ts=%" PRIu64, pv, buf->timestamp);

    if (vppBufferDone(pv, buf, VPP_PORT_INPUT) != VPP_OK)
        LOGE("ERROR: from callback for port VPP_PORT_INPUT");
}

void HidlVpp::vppOBD(void *pv, struct vpp_buffer *buf)
{
    if (!pv || !buf)
    {
        LOGE("ERROR: pv=%p buf=%p", pv, buf);
        return;
    }

    LOGI("got obd for HidlVpp instance = %p, ts=%" PRIu64, pv, buf->timestamp);

    if (vppBufferDone(pv, buf, VPP_PORT_OUTPUT) != VPP_OK)
        LOGE("ERROR: from callback for port VPP_PORT_OUTPUT");
}

void HidlVpp::vppEVT(void *pv, struct vpp_event e)
{
    VppEvent stEvt;
    uint32_t u32 = VPP_OK;
    Return<uint32_t> r = VPP_OK;

    if (!pv)
    {
        LOGE("ERROR: pv=%p", pv);
        return;
    }
    HidlVpp *vpp = static_cast<HidlVpp *>(pv);
    LOGI("got evt for HidlVpp instance = %p", vpp);

    u32 = vppEventToHidl(e, stEvt);
    if (u32 != VPP_OK)
    {
        LOGE("ERROR: from vppEventToHidl Err=%u", u32);
        return;
    }

    r = vpp->mHidlCb->vppEvent(stEvt);
    u32 = r.isOk() ? static_cast<uint32_t>(r) : VPP_ERR;
    LOGE_IF(!r.isOk(), "Binder callback failed for evt");
    LOGE_IF(u32 != VPP_OK, "vppEvent failed, u32=%u", u32);
}

}  // namespace implementation
}  // namespace V1_1
}  // namespace vpp
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
