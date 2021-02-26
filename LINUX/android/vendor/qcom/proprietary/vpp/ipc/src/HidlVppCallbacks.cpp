/*!
 * @file HidlVppCallbacks.cpp
 *
 * @cr
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * @services
 */

#include <sys/types.h>
#include <inttypes.h>
#include <cutils/native_handle.h>

#define VPP_LOG_TAG     VPP_LOG_IPC_CLIENT_TAG
#define VPP_LOG_MODULE  VPP_LOG_IPC_CLIENT
#include "vpp_dbg.h"
#include "vpp.h"
#include "vpp_def.h"
#include "vpp_ion.h"
#include "buf_pool.h"
#include "vpp_stats.h"
#include "HidlVppCallbacks.h"
#include "HidlVppUtils.h"

using namespace android;

namespace vendor {
namespace qti {
namespace hardware {
namespace vpp {
namespace V1_1 {
namespace implementation {

HidlVppCallbacks::HidlVppCallbacks(VppClient *client) :
    mClient(client)
{
    LOGI("HidlVppCallbacks constructor");
}

HidlVppCallbacks::~HidlVppCallbacks()
{
    LOGI("HidlVppCallbacks destructor");
}

// Methods from ::vendor::qti::hardware::vpp::V1_1::IHidlVppCallbacks follow.
Return<uint32_t> HidlVppCallbacks::inputBufferDone(const VppBuffer& buf)
{
    struct vpp_buffer *pstExtBuf = NULL;
    uint32_t u32 = VPP_OK;

    // deregister the buffer which matches the vppClientBufId
    pstExtBuf = mClient->deregisterVppBuf(buf.bufferId);
    if (pstExtBuf == NULL)
    {
        LOGE("HidlVppCallbacks::inputBufferDone deregVppBuf returned false");
        u32 = VPP_ERR;
    }
    else
    {
        if (mClient->mCb.input_buffer_done)
        {
            LOGI("HidlVppCallbacks::inputBufferDone BufId:%u ts=%" PRIu64 " flags:%u",
                 buf.bufferId, buf.timestamp, buf.flags);
            mClient->mCb.input_buffer_done(mClient->mCb.pv, pstExtBuf);
        }
        else
        {
            LOGE("HidlVppCallbacks::inputBufferDone client has no ibd!");
            u32 = VPP_ERR;
        }
    }
    return u32;
}

Return<uint32_t> HidlVppCallbacks::outputBufferDone(const VppBuffer& buf)
{
    struct vpp_buffer *pstExtBuf = NULL;
    uint32_t u32 = VPP_OK;

    // deregister the buffer which matches the vppClientBufId
    pstExtBuf = mClient->deregisterVppBuf(buf.bufferId);
    if (pstExtBuf == NULL)
    {
        LOGE("HidlVppCallbacks::outputBufferDone deregVppBuf returned false");
        u32 = VPP_ERR;
    }
    else
    {
        pstExtBuf->flags            = buf.flags;
        pstExtBuf->timestamp        = buf.timestamp;
        pstExtBuf->cookie_in_to_out = (void *)buf.cookieInToOut;

        pstExtBuf->pixel.offset     = buf.pixel.offset;
        pstExtBuf->pixel.alloc_len  = buf.pixel.allocLen;
        pstExtBuf->pixel.filled_len = buf.pixel.filledLen;

        pstExtBuf->extradata.offset     = buf.extradata.offset;
        pstExtBuf->extradata.alloc_len  = buf.extradata.allocLen;
        pstExtBuf->extradata.filled_len = buf.extradata.filledLen;

        if (mClient->mCb.output_buffer_done)
        {
            LOGI("HidlVppCallbacks::outputBufferDone BufId:%u ts=%" PRIu64 " flags:%u",
                 buf.bufferId, buf.timestamp, buf.flags);
            mClient->mCb.output_buffer_done(mClient->mCb.pv, pstExtBuf);
        }
        else
        {
            LOGE("HidlVppCallbacks::outputBufferDone client has no obd!");
            u32 = VPP_ERR;
        }
    }
    return u32;
}

Return<uint32_t> HidlVppCallbacks::vppEvent(const VppEvent& e)
{
    struct vpp_event stEvt;
    uint32_t u32 = VPP_OK;

    LOGI("HidlVppCallbacks::vppEvent, e.type=%u", e.type);

    u32 = hidlToVppEvent(stEvt, e);
    if (u32 != VPP_OK)
    {
        LOGE("ERROR: from convertVppBufferToHidl Err=%u", u32);
    }
    else
    {
        if (mClient->mCb.vpp_event)
        {
            mClient->mCb.vpp_event(mClient->mCb.pv, stEvt);
        }
        else
        {
            LOGE("HidlVppCallbacks::vppEvent client has no vpp_event!");
            u32 = VPP_ERR;
        }
    }
    return u32;
}

}  // namespace implementation
}  // namespace V1_1
}  // namespace vpp
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
