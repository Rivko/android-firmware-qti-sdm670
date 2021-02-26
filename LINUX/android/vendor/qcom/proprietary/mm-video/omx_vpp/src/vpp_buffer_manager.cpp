/* =========================================================================
Copyright (c) 2015-2018 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

Copyright (c) 2009, The Linux Foundation. All rights reserved.
--------------------------------------------------------------------------*/
/*============================================================================
                            O p e n M A X   w r a p p e r s
                O p e n  M A X   C o m p o n e n t  I n t e r f a c e

*//** @file vpp_buffer_manager.cpp
  This module contains the abstract interface for the OpenMAX components.

*//*========================================================================*/

//////////////////////////////////////////////////////////////////////////////
//                             Include Files
//////////////////////////////////////////////////////////////////////////////
#include <dlfcn.h>
#include <sys/prctl.h>
#include <gralloc_priv.h>
#include <media/hardware/HardwareAPI.h>
#include <cmath>
#include <cutils/ashmem.h>
#include <algorithm>
#include <sys/mman.h>

#include "OMX_QCOMExtns.h"


#define _ANDROID_
#include "vidc_debug.h"
#define USE_ION
#include "omx_vdec.h"
#undef USE_ION
#undef _ANDROID_

#include "vpp_buffer_manager.h"
#include "vpp_omx_component.h"

// #define REASSIGN_NON_RO_BUFFERS
// #define DBG_REASSIGN_VPP_OUT_IF_STARVED
#define VPP_FTB_COUNT 6

#define LOG_NO_OMXVPPBUF(_buf) DEBUG_PRINT_ERROR("unable to find OmxVppBuffer for OmxHeader=%p", (_buf))

#define DUMP_BUF_P(_lvl, _b, _pre) \
    DEBUG_PRINT_##_lvl("%s: omxBuf=%p, id=%u, obdIdx=%u, invalid=%u, omxHdr=%p, vppBuf=%p, handle=%p, pool=%u", \
                       (_pre), (_b), (_b)->bufferID, (_b)->obdIdx, (_b)->invalid, (_b)->omxHeader, \
                       &(_b)->vppBuffer, (_b)->handle, (_b)->pool)

#define DUMP_BUF(_lvl, _b) \
    DEBUG_PRINT_##_lvl("  omxBuf=%p, id=%u, obdIdx=%u, invalid=%u, omxHdr=%p, vppBuf=%p, handle=%p, pool=%u", \
                       (_b), (_b)->bufferID, (_b)->obdIdx, (_b)->invalid, (_b)->omxHeader, \
                       &(_b)->vppBuffer, (_b)->handle, (_b)->pool)

#define OMX_VPP_PROP_DEBUG_EXTRADATA    "vendor.vpp.debug.extradata"
#define DEBUG_EXTRADATA_LOG(fmt, args...) \
({                                        \
    if(mDebug)           \
    DEBUG_PRINT_LOW(fmt, ##args);         \
})

VppBufferManager::VppBufferManager(VppOmxComponent &component):
    mComponent(component),
    mDecoderPoolCnt(0),
    mVppPoolCnt(0),
    mDisableDynamicBufMode(false),
    mOmxMinBufCnt(0),
    mOmxFtbCount(0),
    mVppFtbCount(0),
    mMinVppOutCount(1),
    mOMXBufferID(0),
    mObdCnt(0),
    mOmxVppExtraData(NULL)
{
    DEBUG_PRINT_LOW(" Constructed the buffer manager. ");
}

VppBufferManager:: ~VppBufferManager()
{
    if (mOmxVppExtraData)
        delete mOmxVppExtraData;

    for (auto it = mBuffers.begin(); it != mBuffers.end(); it++) {
        OmxVppBuffer *buf = *it;
        DUMP_BUF_P(ERROR, buf, "Destructor");
        it = mBuffers.erase(it);
        delete buf;
    }

    DEBUG_PRINT_INFO("Destructed VppBufferManager");
}

bool VppBufferManager:: ResetBufferRequirements(OMX_PARAM_PORTDEFINITIONTYPE* portDef) {
    portDef->nBufferCountMin = mOmxMinBufCnt;
    portDef->nBufferCountActual = mOmxActBufCnt;
    return true;
}

static uint32_t getVppResolution(uint32_t width, uint32_t height)
{
    uint32_t u32VppRes = VPP_RESOLUTION_MAX;

    if (width <= RES_SD_MAX_W && height <= RES_SD_MAX_H)
        u32VppRes = VPP_RESOLUTION_SD;

    else if (width <= RES_HD_MAX_W && height <= RES_HD_MAX_H)
        u32VppRes = VPP_RESOLUTION_HD;

    else if (width <= RES_FHD_MAX_W && height <= RES_FHD_MAX_H)
        u32VppRes = VPP_RESOLUTION_FHD;

    else if (width <= RES_UHD_MAX_W && height <= RES_UHD_MAX_H)
        u32VppRes = VPP_RESOLUTION_UHD;

    return u32VppRes;
}

private_handle_t *VppBufferManager::GetPrivateHandle(OmxVppBuffer &buf)
{
    private_handle_t *handle = NULL;

    if (mDisableDynamicBufMode) {
        handle = (private_handle_t *)buf.handle;
    } else {
        struct VideoDecoderOutputMetaData *meta;
        meta = (struct VideoDecoderOutputMetaData *)buf.omxHeader->pBuffer;
        handle = (private_handle_t *)meta->pHandle;
    }

    return handle;
}

static bool isBypassRequirements(struct vpp_requirements &req,
                                 OMX_VIDEO_PORTDEFINITIONTYPE* videoDef)
{
    uint32_t u32Res;
    struct vpp_port_factor in, out;

    if (!videoDef) {
        DEBUG_PRINT_ERROR("isBypassRequirements: videoDef is null");
        return true;
    }

    u32Res = getVppResolution(videoDef->nFrameWidth, videoDef->nFrameHeight);

    if (u32Res >= VPP_RESOLUTION_MAX) {
        DEBUG_PRINT_ERROR("isBypassRequirements: w/h=%d/%d yielded invalid res: %u",
                          videoDef->nFrameWidth, videoDef->nFrameHeight, u32Res);
        return true;
    }

    in = req.in_factor[u32Res];
    out = req.out_factor[u32Res];
    return in.add == 0 && in.mul == 0 && out.add == 0 && out.mul == 0;
}

bool VppBufferManager::UpdateBufferRequirements(struct vpp_requirements &req,
                                                OMX_PARAM_PORTDEFINITIONTYPE* portDef,
                                                bool isClientSet)
{
    if (!portDef) {
        DEBUG_PRINT_ERROR("UpdateBufferRequirements: portDef is null");
        return false;
    }

    uint32_t u32ActCntPrev;

    if (isClientSet) {
        mOmxActBufCnt = portDef->nBufferCountActual;
        DEBUG_PRINT_LOW("client set counts={min: %d, act: %d}",
                        portDef->nBufferCountMin, portDef->nBufferCountActual);

        return true;
    }

    mVppFtbCount = 0;
    mOmxMinBufCnt = portDef->nBufferCountMin;
    u32ActCntPrev = portDef->nBufferCountActual;

    if (!isBypassRequirements(req, &portDef->format.video)) {
        mVppFtbCount = VPP_FTB_COUNT;
        if ((mOmxMinBufCnt + mVppFtbCount) > MAX_NUM_INPUT_OUTPUT_BUFFERS) {
            mVppFtbCount = MAX_NUM_INPUT_OUTPUT_BUFFERS - mOmxMinBufCnt;
            DEBUG_PRINT_LOW("mVppFtbCount clamped to %u", mVppFtbCount);
        }
    }

    portDef->nBufferCountMin = mOmxMinBufCnt + mVppFtbCount;

    if (portDef->nBufferCountActual < portDef->nBufferCountMin)
        portDef->nBufferCountActual = portDef->nBufferCountMin;

    mOmxActBufCnt = portDef->nBufferCountActual;

    DEBUG_PRINT_LOW("counts: original={min: %d, actual: %d} new={min: %d, actual: %d}",
                    mOmxMinBufCnt, u32ActCntPrev,
                    portDef->nBufferCountMin, portDef->nBufferCountActual);

    return true;
}

void VppBufferManager:: IncreasePoolCount(OmxBufferPool pool) {
    if (pool == OMX_POOL_DECODER)
        mDecoderPoolCnt++;
    else if (pool == OMX_POOL_VPP)
        mVppPoolCnt++;
}

void VppBufferManager:: DecreasePoolCount(OmxBufferPool pool) {
    if (pool == OMX_POOL_DECODER) {
        if (!mDecoderPoolCnt) {
            DEBUG_PRINT_ERROR("ERROR: decoder pool is already empty!");
            DebugBuffers();
            return;
        }
        mDecoderPoolCnt--;

    } else if (pool == OMX_POOL_VPP) {
        if (!mVppPoolCnt) {
            DEBUG_PRINT_ERROR("ERROR: vpp pool is already empty!");
            DebugBuffers();
            return;
        }
        mVppPoolCnt--;
    }
}

void VppBufferManager::AssignBufferToPool(OmxVppBuffer &buf) {

    // Determines how buffers should be distributed between the deecoder and
    // VPP pools.  This function will make decisions based on the state of the
    // component and the state of each pool.
    //
    // During startup, the preference is to give the buffers to the decoder
    // first so that the component can get buffers downstream as quickly as
    // possible.  To ensure that we do not starve VPP, we make sure that it has
    // at least one buffer. After this time, as we receive buffers that are not
    // owned by any pool from the client (i.e. client calls FTB with new
    // buffers), we will give the buffers to decoder until some threshold is
    // reached.
    //
    // After reaching this threshold, we will start to consider VPP's input
    // pool size. If, at any time, we detect that VPP is starved, we will
    // immediately give a buffer to VPP to ensure that the pipeline is not
    // starved. Otherwise we will distribute the buffers between decoder's pool
    // and VPP's pools.
    //
    // In the event that VPP becomes starved, when determining how to
    // distribute buffers to VPP, we will consider the buffers that are in each
    // pool. If VPP already has a full pool and decoder does not, then there
    // would be a preference to give the buffer to decoder.  But, since VPP is
    // starved, we want to give a buffer to VPP now. Therefore, we will try to
    // find the oldest buffer that is assigned to the VPP pool, which is
    // currently owned by the client, and unassign it from the VPP pool.  The
    // rationale is that the oldest buffer (i.e. the FBD that we issued longest
    // ago) will be the closest to being returned as an FTB, at which point the
    // decision can be taken as to whether to give the buffer to decoder again
    // or not.  If this is successful then the current buffer will be assigned
    // to VPP.
    //
    // Once we reach steady state, that is, both pools have their minimum
    // number of buffers, then we will try to evenly distribute the excess
    // buffers between the pools.
    //
    // In the case that the decoder is reconfiguring, we will always give
    // buffers to VPP, if it is holding input buffers, since decoder would not
    // require them until it has been reconfigured. Otherwise, return the
    // buffers to the client instead, to minimize the latency that would occur
    // later when a flush is possibly issued.

    AutoMutex autoLock(mVppBufferLock);

    buf.seen = true;

    if (mComponent.mFlushing) {
        UnassignBufferFromPool_l(buf);
    }

    if (mComponent.mDecoderReconfig &&
        !(buf.omxHeader->nFlags & OMX_BUFFERFLAG_READONLY)) {
        UnassignBufferFromPool_l(buf);
    }

    // Decide where this buffer should go if it does not belong to a pool.
    if (buf.pool != OMX_POOL_DECODER && buf.pool != OMX_POOL_VPP) {

        OmxBufferPool pool = OMX_POOL_NONE;

        BufferCounts cnts;
        GetBufferCounts_l(cnts);

        bool isVppFull = mVppPoolCnt >= mVppFtbCount;
        bool isDecoderFull = mDecoderPoolCnt >= mOmxMinBufCnt;
        bool isVppStarved = cnts.vppIn > cnts.vppOut;

        const char *reason = "decoder decisions not met";

        if (mComponent.mFlushing) {
            reason = "flushing";
            pool = OMX_POOL_NONE;

        } else if (mComponent.mDecoderReconfig) {
            reason = "decoder reconfig";
            pool = OMX_POOL_VPP;

            if (!cnts.vppIn) {
                reason = "reconfig, vpp has no input";
                pool = OMX_POOL_NONE;
            }

        } else if (!isVppStarved && isDecoderFull && isVppFull) {
            // split between them
            uint32_t dec_ex = mDecoderPoolCnt - mOmxMinBufCnt;
            uint32_t vpp_ex = mVppPoolCnt - mVppFtbCount;

            pool = OMX_POOL_VPP;
            reason = "decoder has more excess than vpp";

            if (vpp_ex > dec_ex) {
                pool = OMX_POOL_DECODER;
                reason = "vpp has more excess than decoder";
            }

        } else {

            uint32_t u32Num, u32Den;
            if (mOmxMinBufCnt < 7) {
                u32Num = 1;
                u32Den = 2;
            } else if (mOmxMinBufCnt < 10) {
                u32Num = 2;
                u32Den = 3;
            } else if (mOmxMinBufCnt < 21) {
                u32Num = 5;
                u32Den = 6;
            } else {
                u32Num = 9;
                u32Den = 10;
            }

            pool = OMX_POOL_DECODER;

            if (!mDecoderPoolCnt) {
                reason = "decoder pool size is 0";

            } else if (!mVppPoolCnt) {
                pool = OMX_POOL_VPP;
                reason = "vpp pool size is 0";

            } else if (mDecoderPoolCnt < ((mOmxMinBufCnt * u32Num) / u32Den)) {
                reason = "decoder does not have min percentage of its buffers yet";

            } else if (isVppStarved) {

                pool = OMX_POOL_VPP;
                reason = "vpp out is starved";

                if (isVppFull && !isDecoderFull) {
                    OmxVppBuffer *tmp = GetOldestClientOwnedVppBuffer_l();
                    if (!tmp) {
                        pool = OMX_POOL_DECODER;
                        reason = "vpp starved, but failed to find a repurposable buffer";
                        DEBUG_PRINT_ERROR("unable to repurpose old buffer, but the pool is reportedly full!");
                    } else {
                        reason = "vpp starved, pool full, repurposing client owned buffer";
                        UnassignBufferFromPool_l(*tmp);
                    }
                }

            } else if (!cnts.vppIn && (mDecoderPoolCnt < (mOmxMinBufCnt / 2))) {
                reason = "vpp has no input buffers and decoder cnt less than half";

            } else if (mVppPoolCnt > mDecoderPoolCnt) {
                reason = "vpp has more than decoder";

            } else if (isVppFull && !isDecoderFull) {
                reason = "vpp full, decoder not";

            } else {
                reason = "no need to give to decoder";
                pool = OMX_POOL_VPP;
            }
        }

        // Explicitly do the assignment here, since some functions that are
        // required by this function (e.g. GetOldestClientOwnedVppBuffer) will
        // search through all registered buffers. If we prematurely assign the
        // buffer, then those dependent functions will falsely identify this
        // buffer as part of a pool already.
        buf.pool = pool;
        IncreasePoolCount(buf.pool);

        DEBUG_PRINT_LOW("assigned buf=%p to %s, pools={vdec=%d/%d, vpp=%d/%d} (%s)",
                        buf.omxHeader, buf.getPoolStr(), mDecoderPoolCnt, mOmxMinBufCnt,
                        mVppPoolCnt, mVppFtbCount, reason);
    }
}

void VppBufferManager::UnassignBufferFromPool_l(OmxVppBuffer &buf) {
    DecreasePoolCount(buf.pool);
    buf.pool = OMX_POOL_NONE;
}

void VppBufferManager::UnassignBufferFromPool(OmxVppBuffer &buf) {
    AutoMutex autoLock(mVppBufferLock);
    UnassignBufferFromPool_l(buf);
}

void VppBufferManager::ReassignBufferToPool(OmxVppBuffer &buf) {
    UnassignBufferFromPool(buf);
    AssignBufferToPool(buf);
}

void VppBufferManager::ClearBuffers() {

    // We don't want to delete all buffers from the vector when this is called
    // since the client can call release_buffer after calling port enable,
    // which would call this.

    DEBUG_PRINT_LOW("ClearBuffers()");

    AutoMutex autoLock(mVppBufferLock);
    mOMXBufferID = 0;
    mObdCnt = 0;

    for (auto buf : mBuffers) {
        DUMP_BUF_P(INFO, buf, "Invalidating");
        DecreasePoolCount(buf->pool);
        buf->invalid = true;
    }
}

void VppBufferManager::DebugBuffers()
{
    DEBUG_PRINT_ERROR("DebugBuffers:");
    for (auto buf : mBuffers) {
        DUMP_BUF(INFO, buf);
    }
}

bool VppBufferManager::RegisterClientBuffer(OMX_BUFFERHEADERTYPE *buffer,
                                            OMX_U8* handle)
{
    if (!buffer) {
        DEBUG_PRINT_ERROR("null buffer header!");
        return false;
    }

    OmxVppBuffer *omxBuffer = new OmxVppBuffer();

    if (!omxBuffer) {
        DEBUG_PRINT_ERROR("Failed to allocate OmxVppBuffer, buffer=%p, handle=%p",
                          buffer, handle);
        return false;
    }

    omxBuffer->omxHeader = buffer;
    omxBuffer->owner = OMX_CLIENT;
    omxBuffer->pool = OMX_POOL_NONE;
    omxBuffer->handle = handle;
    omxBuffer->bufferID = mOMXBufferID++;

    AutoMutex autoLock(mVppBufferLock);
    mBuffers.push_back(omxBuffer);

    DUMP_BUF_P(INFO, omxBuffer, "RegisterClientBuffer");

    return true;
}

bool VppBufferManager::UnregisterClientBuffer(OMX_BUFFERHEADERTYPE *buffer)
{
    AutoMutex autoLock(mVppBufferLock);

    for (auto it = mBuffers.begin(); it != mBuffers.end(); it++) {
        OmxVppBuffer *buf = *it;

        if (buf->omxHeader == buffer) {
            DecreasePoolCount(buf->pool);

            DUMP_BUF_P(LOW, buf, "Unregistering");
            DEBUG_PRINT_LOW("unregistered buffer=%p, new sz=%zu, pools {dec=%u, vpp=%u}",
                            buffer, mBuffers.size(), mDecoderPoolCnt, mVppPoolCnt);

            mBuffers.erase(it);
            delete buf;

            return true;
        }
    }

    DEBUG_PRINT_ERROR("failed to unregister buffer=%p, sz=%zu, pools {dec=%u, vpp=%u}",
                      buffer, mBuffers.size(), mDecoderPoolCnt, mVppPoolCnt);
    DebugBuffers();

    return false;
}

VppBufferManager::OmxVppBuffer *VppBufferManager::GetOmxVppBuffer(OMX_BUFFERHEADERTYPE* buffer)
{
    AutoMutex autoLock(mVppBufferLock);

    for (auto buf : mBuffers) {
        if (buf->omxHeader == buffer) {
            return buf;
        }
    }

    DEBUG_PRINT_ERROR("could not find OmxVppBuffer for %p", buffer);
    return nullptr;
}

VppBufferManager::OmxVppBuffer *VppBufferManager::GetOldestClientOwnedVppBuffer_l() {

    OmxVppBuffer *oldest = nullptr;

#ifdef DBG_REASSIGN_VPP_OUT_IF_STARVED
    DEBUG_PRINT_LOW("Searching for oldest client owned vpp buffer:");
#endif
    for (auto buf : mBuffers) {
        if (buf->invalid)
            continue;

        if (buf->pool == OMX_POOL_VPP && buf->owner == OMX_CLIENT)
        {
#ifdef DBG_REASSIGN_VPP_OUT_IF_STARVED
            DUMP_BUF(INFO, buf);
#endif
            if (!oldest) {
                oldest = buf;
                continue;
            }
            if (buf->obdIdx < oldest->obdIdx)
                oldest = buf;
        }
    }

    if (!oldest) {
        DEBUG_PRINT_ERROR("unable to find oldest client owned vpp buffer!");
        DebugBuffers();
    }
#ifdef DBG_REASSIGN_VPP_OUT_IF_STARVED
    else {
        DEBUG_PRINT_LOW("Found oldest:");
        DUMP_BUF(INFO, oldest);
    }
#endif

    return oldest;
}

void VppBufferManager::PrepareBufferForVppIn(OmxVppBuffer &buf) {

    struct vpp_buffer *vppBuffer = &buf.vppBuffer;
    OMX_BUFFERHEADERTYPE *hdr = buf.omxHeader;
    uint32_t filledLen = 0;
#ifdef READ_SIZE_FROM_OUTPUTPORTPRIVATE
    if (!mComponent.mIsQCSoftwareDecoder) {
        struct vdec_output_frameinfo *bufInfo =
                (struct vdec_output_frameinfo *)hdr->pOutputPortPrivate;
        filledLen = bufInfo->len;
    } else {
        struct vdec_bufferpayload *bufInfo =
                (struct vdec_bufferpayload *)hdr->pOutputPortPrivate;
        filledLen = bufInfo->buffer_len;
    }
    DEBUG_PRINT_LOW("get size from component(%s): %u, calcualted size = %u",
            mComponent.mOmxRole, filledLen, mComponent.mPixelSize);
#else
    if (hdr->nFilledLen > 0)
        filledLen = mComponent.mPixelSize;
#endif
    private_handle_t *handle = GetPrivateHandle(buf);

    if (!handle) {
        buf.owner = OMX_CLIENT;
        return;
    }

    buf.owner = VPP_IN;

    memset(vppBuffer, 0, sizeof(struct vpp_buffer));

    vppBuffer->pixel.fd = handle->fd;
    vppBuffer->pixel.offset = hdr->nOffset;
    vppBuffer->pixel.alloc_len = handle->size;
    vppBuffer->pixel.filled_len = filledLen;
    vppBuffer->pixel.valid_data_len = filledLen;
    vppBuffer->extradata.fd = -1;

    if ((hdr->nFlags & OMX_BUFFERFLAG_EXTRADATA) && mOmxVppExtraData) {
        mOmxVppExtraData->useBufferToVpp(buf);
    }

    vppBuffer->timestamp = hdr->nTimeStamp;
    vppBuffer->cookie_in_to_out = NULL;
    vppBuffer->cookie = hdr;
    vppBuffer->pvGralloc = handle;
    ConvertOmxVppFlags(hdr->nFlags, vppBuffer->flags, true);
}

void VppBufferManager::PrepareBufferForVppOut(OmxVppBuffer &buf) {

    OMX_BUFFERHEADERTYPE *hdr = buf.omxHeader;
    struct vpp_buffer *vppBuffer = &buf.vppBuffer;
    private_handle_t *handle = GetPrivateHandle(buf);

    if (!handle) {
        buf.owner = OMX_CLIENT;
        return;
    }

    buf.owner = VPP_OUT;

    memset(vppBuffer, 0, sizeof(struct vpp_buffer));
    vppBuffer->extradata.fd = -1;

    if (mOmxVppExtraData) {
        mOmxVppExtraData->useBufferToVpp(buf);
    }

    vppBuffer->pixel.fd = handle->fd;
    vppBuffer->pixel.offset = hdr->nOffset;
    vppBuffer->pixel.alloc_len = handle->size;
    vppBuffer->pixel.valid_data_len = handle->size;

    hdr->nFilledLen = 0;
    hdr->nTimeStamp = 0;
    hdr->nFlags = 0;

    vppBuffer->cookie = hdr;
    vppBuffer->pvGralloc = handle;
}

void VppBufferManager::PrepareBufferForVdecOut(OmxVppBuffer &buf)
{
    OMX_BUFFERHEADERTYPE *hdr = buf.omxHeader;

    buf.owner = OMX_VDEC;
    hdr->nAllocLen = mDisableDynamicBufMode ?
        hdr->nAllocLen : sizeof(struct VideoDecoderOutputMetaData);
    hdr->nFilledLen = 0;
    hdr->nTimeStamp = 0;
}

void VppBufferManager::PrepareBufferForClient(OmxVppBuffer &buf,
                                              bool translateParams)
{
    struct vpp_buffer *vpp = &buf.vppBuffer;
    OMX_BUFFERHEADERTYPE *omx = buf.omxHeader;

    buf.owner = OMX_CLIENT;
    buf.obdIdx = ++mObdCnt;

    omx->nFilledLen = 0;
    omx->nTimeStamp = 0;
    omx->nAllocLen = mDisableDynamicBufMode ?
        omx->nAllocLen : sizeof(struct VideoDecoderOutputMetaData);

    if (!translateParams)
        return;

    omx->nTimeStamp = vpp->timestamp;

    if (mDisableDynamicBufMode) {
        omx->nFilledLen = vpp->pixel.filled_len;
    } else {
        omx->nFilledLen =
            vpp->pixel.filled_len ? sizeof(struct VideoDecoderOutputMetaData) : 0;
    }

    ConvertOmxVppFlags(vpp->flags, omx->nFlags, false);
    if (mOmxVppExtraData)
        mOmxVppExtraData->copyExtraDataToClient(buf);
}

VppBufferManager::OmxVppBuffer* VppBufferManager::GetFtbDestination(OMX_BUFFERHEADERTYPE* omxHdr)
{
    OmxVppBuffer *omxVppBuffer = GetOmxVppBuffer(omxHdr);

    if (!omxVppBuffer) {
        LOG_NO_OMXVPPBUF(omxHdr);
        return nullptr;
    }

    AssignBufferToPool(*omxVppBuffer);

    if (omxVppBuffer->pool == OMX_POOL_DECODER) {
        PrepareBufferForVdecOut(*omxVppBuffer);
    } else if (omxVppBuffer->pool == OMX_POOL_VPP) {
        PrepareBufferForVppOut(*omxVppBuffer);
    } else {
        PrepareBufferForClient(*omxVppBuffer, false);
    }

    DEBUG_PRINT_LOW("GetFtbDest: omxHdr=%p, pool=%s, dest=%s",
                     omxVppBuffer->omxHeader, omxVppBuffer->getPoolStr(),
                     omxVppBuffer->getOwnerStr());

    return omxVppBuffer;
}

VppBufferManager::OmxVppBuffer* VppBufferManager::GetVdecFbdDestination(OMX_BUFFERHEADERTYPE* buffer)
{
    OmxVppBuffer *omxVppBuffer = GetOmxVppBuffer(buffer);

    if (!omxVppBuffer) {
        LOG_NO_OMXVPPBUF(buffer);
        return nullptr;
    }

    bool isRo = buffer->nFlags & OMX_BUFFERFLAG_READONLY;
    bool isEos = buffer->nFlags & OMX_BUFFERFLAG_EOS;

    if (mComponent.mFlushing || mComponent.mDecoderReconfig) {
        omxVppBuffer->owner = OMX_CLIENT;
        DEBUG_PRINT_LOW("FbdDest: buffer: %p, to: client, flushing=%u, reconfig=%u",
                        buffer, mComponent.mFlushing, mComponent.mDecoderReconfig);
    } else if (!buffer->nFilledLen && isRo) {
        omxVppBuffer->owner = OMX_VDEC;
        DEBUG_PRINT_LOW("FbdDest: buffer: %p, to: vdec, fl=%u, ro=%u", buffer,
                        buffer->nFilledLen, isRo);
    } else if (!buffer->nFilledLen && !isEos) {
#ifdef REASSIGN_NON_RO_BUFFERS
        if (!isRo) {
            ReassignBufferToPool(*omxVppBuffer);
            DEBUG_PRINT_LOW("FbdDest: buffer: %p, to: %s (reassigned), fl=%u, eos=%u", buffer,
                             omxVppBuffer->pool == OMX_POOL_DECODER ? "decoder" : "vpp",
                             buffer->nFilledLen, isEos);
        }
#endif

        if (omxVppBuffer->pool == OMX_POOL_DECODER) {
            PrepareBufferForVdecOut(*omxVppBuffer);
        }

        else if (omxVppBuffer->pool == OMX_POOL_VPP) {
            PrepareBufferForVppOut(*omxVppBuffer);
        }

    } else {
#ifdef REASSIGN_NON_RO_BUFFERS
        if (!isRo && !isEos) {
            UnassignBufferFromPool(*omxVppBuffer);
        }
#endif
        PrepareBufferForVppIn(*omxVppBuffer);
        DEBUG_PRINT_LOW("FbdDest: buffer: %p, to: vpp, fl=%u, ro=%u, eos=%u", buffer,
                         omxVppBuffer->vppBuffer.pixel.filled_len, isRo, isEos);
    }

    return omxVppBuffer;
}

VppBufferManager::OmxVppBuffer* VppBufferManager::GetVppObdDestination(OMX_BUFFERHEADERTYPE* omxHdr)
{
    OmxVppBuffer *omxVppBuffer = GetOmxVppBuffer(omxHdr);

    if (!omxVppBuffer) {
        LOG_NO_OMXVPPBUF(omxHdr);
        return nullptr;
    }

    PrepareBufferForClient(*omxVppBuffer);

#ifdef REASSIGN_NON_RO_BUFFERS
    if (omxVppBuffer->pool == OMX_POOL_VPP &&
        mDecoderPoolCnt < mOmxMinBufCnt)
        UnassignBufferFromPool(*omxVppBuffer);
#endif

    return omxVppBuffer;
}

void VppBufferManager::PrintBufferStatistics()
{
    BufferCounts cnts;

    {
        AutoMutex autoLock(mVppBufferLock);
        GetBufferCounts_l(cnts);
    }

    DEBUG_PRINT_LOW("Buffers: Client [%d], VDEC [%d/%d], VPP_IN [%d], VPP_OUT [%d/%d], SEEN [%d]",
                    cnts.client, cnts.decoder, mDecoderPoolCnt, cnts.vppIn,
                    cnts.vppOut, mVppPoolCnt, cnts.seen);
}

void VppBufferManager::GetBufferCounts(BufferCounts &counts)
{
    AutoMutex autoLock(mVppBufferLock);
    GetBufferCounts_l(counts);
}

void VppBufferManager::GetBufferCounts_l(BufferCounts &counts)
{
    memset(&counts, 0, sizeof(BufferCounts));

    for (auto buf : mBuffers) {

        if (buf->invalid)
            continue;

        else if (buf->owner == OMX_CLIENT)
            counts.client++;

        else if (buf->owner == OMX_VDEC)
            counts.decoder++;

        else if(buf->owner == VPP_IN)
            counts.vppIn++;

        else if(buf->owner == VPP_OUT)
            counts.vppOut++;

        if (buf->seen)
            counts.seen++;
    }
}

bool VppBufferManager::GetBufferMode() {
    return mDisableDynamicBufMode;
}

void VppBufferManager::SetBufferMode(bool bDisableDynamicBufMode) {
    mDisableDynamicBufMode = bDisableDynamicBufMode;
}

//Helper function to convert from OMX flags to VPP flags
void VppBufferManager:: ConvertOmxVppFlags(uint32_t inflags, uint32_t& outflags,bool fromOmxToVpp) {
    uint32_t ctzOmxEosFlag = __builtin_ctz(OMX_BUFFERFLAG_EOS);
    uint32_t ctzOmxDCFlag = __builtin_ctz(OMX_BUFFERFLAG_DATACORRUPT);
    uint32_t ctzOmxSyncFlag = __builtin_ctz(OMX_BUFFERFLAG_SYNCFRAME);
    uint32_t ctzOmxROFlag = __builtin_ctz(OMX_BUFFERFLAG_READONLY);
    uint32_t ctzOmxExtraFlag = __builtin_ctz(OMX_BUFFERFLAG_EXTRADATA);

    uint32_t ctzVppEosFlag = __builtin_ctz(VPP_BUFFER_FLAG_EOS);
    uint32_t ctzVppDcFlag = __builtin_ctz(VPP_BUFFER_FLAG_DATACORRUPT);
    uint32_t ctzVppSyncFlag = __builtin_ctz(VPP_BUFFER_FLAG_SYNCFRAME);
    uint32_t ctzVppROFlag = __builtin_ctz(VPP_BUFFER_FLAG_READONLY);
    uint32_t ctzVppExtraFlag = __builtin_ctz(VPP_BUFFER_FLAG_EXTRADATA);

    DEBUG_PRINT_LOW(" Input Flags: %s and value is 0x%x",
        fromOmxToVpp ? "OMX" : "Vpp",
        inflags);

    if (fromOmxToVpp) {
        outflags = (((inflags & OMX_BUFFERFLAG_EOS) >> ctzOmxEosFlag) << ctzVppEosFlag ) | \
                (((inflags & OMX_BUFFERFLAG_DATACORRUPT) >> ctzOmxDCFlag) << ctzVppDcFlag ) | \
                (((inflags & OMX_BUFFERFLAG_SYNCFRAME) >> ctzOmxSyncFlag) << ctzVppSyncFlag) | \
                (((inflags & OMX_BUFFERFLAG_READONLY) >> ctzOmxROFlag) << ctzVppROFlag) | \
                (((inflags & OMX_BUFFERFLAG_EXTRADATA) >> ctzOmxExtraFlag) << ctzVppExtraFlag);
    } else {
        outflags = (((inflags & VPP_BUFFER_FLAG_EOS) >> ctzVppEosFlag) << ctzOmxEosFlag) | \
                (((inflags & VPP_BUFFER_FLAG_DATACORRUPT) >> ctzVppDcFlag) << ctzOmxDCFlag) | \
                (((inflags & VPP_BUFFER_FLAG_SYNCFRAME) >> ctzVppSyncFlag) << ctzOmxSyncFlag) | \
                (((inflags & VPP_BUFFER_FLAG_READONLY) >> ctzVppROFlag) << ctzOmxROFlag) | \
                (((inflags & VPP_BUFFER_FLAG_EXTRADATA) >> ctzVppExtraFlag) << ctzOmxExtraFlag);
    }

    DEBUG_PRINT_LOW(" Output Flags: %s and value is 0x%x",
        fromOmxToVpp ? "Vpp" : "OMX",
        outflags);
}

void VppBufferManager::EnableOmxVppExtraData(uint32_t size, uint32_t count) {
    if (mOmxVppExtraData) {
        DEBUG_PRINT_LOW("VppExtra: warning: mOmxVppExtraData is allocated!");
        delete mOmxVppExtraData;
        mOmxVppExtraData = NULL;
    }
    mOmxVppExtraData = new OmxVppExtraData(size, count,
            (enum ExtraDataType)mComponent.mVppExtradataMode, &mComponent);
    if (!mOmxVppExtraData || !mOmxVppExtraData->isValid()) {
       DEBUG_PRINT_LOW("VppExtra: failed to enable omx vpp extradata");
       if (mOmxVppExtraData) {
           delete mOmxVppExtraData;
           mOmxVppExtraData = NULL;
       }
       return;
    }
    DEBUG_PRINT_LOW("VppExtra: enable omx vpp extradata done");
}

void VppBufferManager::UseExtraBufferByClient(OMX_U32 bytes, OMX_U8 **buffer) {
    if (mOmxVppExtraData)
        mOmxVppExtraData->cacheBufferFromClient(bytes, buffer);
}

void VppBufferManager::FreeOmxVppExtraData() {
    if (mOmxVppExtraData) {
        delete mOmxVppExtraData;
        mOmxVppExtraData = NULL;
    }
}

VppBufferManager::OmxVppExtraData::OmxVppExtraData(uint32_t size, uint32_t count,
        enum ExtraDataType type, VppOmxComponent *cmp):
    mSize(size),
    mCount(count),
    mType(type),
    mCmp(cmp),
    mFd(-1),
    mBase(NULL),
    mCacheBuffers({})
{
    char property_value[PROPERTY_VALUE_MAX] = {0};
    property_get(OMX_VPP_PROP_DEBUG_EXTRADATA, property_value, "0");
    mDebug = atoi(property_value);
    DEBUG_EXTRADATA_LOG("VppExtra: constructor: size(%u), count(%u), type(%u)",
            mSize, mCount, mType);
    uint32_t totalSize = mSize * mCount;
    if (totalSize == 0 || mCmp == NULL)
        return;
    mFd = ashmem_create_region("OMX_VPP-Extradata", totalSize);
    if(mFd < 0) {
        clear();
        DEBUG_EXTRADATA_LOG("VppExtra: failed to create ashmem!");
        return;
    }
    mBase = (OMX_U8*)mmap(0, totalSize, PROT_READ|PROT_WRITE, MAP_SHARED, mFd, 0);
    if (mBase == MAP_FAILED) {
        mBase = NULL;
        clear();
        DEBUG_EXTRADATA_LOG("VppExtra: failed to mmap virutal memory");
        return;
    }

    if (mType == EXTRADATA_INTERNAL) {
        // Internal mode: omx vpp needs to invoke use_buffer to omx directly
        for (uint32_t i = 0; i < mCount; i++) {
            OMX_BUFFERHEADERTYPE *header;
            uint8_t *buf = (uint8_t *)mBase + i * mSize;
            OMX_ERRORTYPE err = mCmp->use_buffer(mCmp->mOmxHandle, &header,
                    OMX_CORE_OUTPUT_EXTRADATA_INDEX, mCmp->mAppData,
                    mSize, buf);
            DEBUG_EXTRADATA_LOG("VppExtra: internal mode: use_buffer to omx(0x%x): %u-%p",
                    err, i, header);
            if (err == OMX_ErrorNone) {
                DEBUG_EXTRADATA_LOG("VppExtra: cache omx extra header: %u-%p", i, header);
                mCacheBuffers.push_back(header);
            }
        }
        if (mCacheBuffers.size() != mCount) {
            DEBUG_EXTRADATA_LOG("VppExtra: failed to use buffer to omx");
            clear();
            return;
        }
    }
    DEBUG_EXTRADATA_LOG("VppExtra: constructor done: fd(%d), base(%p)",
            mFd, mBase);
}

VppBufferManager::OmxVppExtraData::~OmxVppExtraData() {
    clear();
}

void VppBufferManager::OmxVppExtraData::clear() {
    if (mType == EXTRADATA_INTERNAL) {
        // Internal mode: omx vpp needs to invoke free_buffer to omx directly
        for (uint32_t i = 0; i < mCacheBuffers.size(); i++) {
            OMX_ERRORTYPE err = mCmp->free_buffer(
                    mCmp->mOmxHandle, OMX_CORE_OUTPUT_EXTRADATA_INDEX,
                    (OMX_BUFFERHEADERTYPE *)mCacheBuffers[i]);
            DEBUG_EXTRADATA_LOG("VppExtra: internal mode: free_buffer to omx(0x%x): %u-%p",
                    err, i, mCacheBuffers[i]);
        }
    }
    mCacheBuffers.clear();
    if (mBase) {
        DEBUG_EXTRADATA_LOG("VppExtra: unmap VA(%p)", mBase);
        munmap(mBase, mSize * mCount);
        mBase = NULL;
    }
    if (mFd != -1) {
        DEBUG_EXTRADATA_LOG("VppExtra: close Fd(%d)", mFd);
        close(mFd);
        mFd = -1;
    }
}

bool VppBufferManager::OmxVppExtraData::isValid() {
    return (mFd >= 0) && mBase;
}

void VppBufferManager::OmxVppExtraData::cacheBufferFromClient(OMX_U32 bytes,
        OMX_U8 **clientBuf) {
    if (!isValid() || mType != EXTRADATA_EXTERNAL
            || clientBuf == NULL || bytes != mSize)
        return;
    uint32_t index = mCacheBuffers.size();
    uint8_t *pVppExtraBuf = (uint8_t *)mBase + index * mSize;
    DEBUG_EXTRADATA_LOG("VppExtra: external mode: cache buffer: %u-%p(replaced by vpp extra %p)",
            index, *clientBuf, pVppExtraBuf);
    mCacheBuffers.push_back(*clientBuf);
    *clientBuf = pVppExtraBuf;
}

void VppBufferManager::OmxVppExtraData::useBufferToVpp(OmxVppBuffer &buf) {
    bool isVpp = buf.owner == VPP_IN || buf.owner == VPP_OUT;
    if (!isVpp)
        return;
    struct vpp_buffer *vppBuffer = &buf.vppBuffer;
    uint32_t index = buf.bufferID;
    vppBuffer->extradata.fd = mFd;
    if (!isValid()) {
        DEBUG_EXTRADATA_LOG("VppExtra: no extra data(index:%u) for Vpp%s",
                index, buf.owner == VPP_IN ? "In" : "Out");
    } else {
        vppBuffer->extradata.offset = index * mSize;
        vppBuffer->extradata.alloc_len = mCount * mSize;
        vppBuffer->extradata.filled_len = buf.owner == VPP_IN ? mSize : 0;
        vppBuffer->extradata.valid_data_len = mSize;
        uint8_t *pVppExtraBuf = (uint8_t *)mBase + index * mSize;
        DEBUG_EXTRADATA_LOG("VppExtra: attach extra data: %u-%p for Vpp%s",
                index, pVppExtraBuf, buf.owner == VPP_IN ? "In" : "Out");
    }
}

void VppBufferManager::OmxVppExtraData::copyExtraDataToClient(OmxVppBuffer &buf) {
    if (!isValid())
        return;
    struct vpp_buffer *vpp = &buf.vppBuffer;
    OMX_BUFFERHEADERTYPE *omx = buf.omxHeader;
    uint32_t size = vpp->extradata.filled_len;

    if (mDebug && size) {
        uint8_t *otherExtra = (uint8_t *)mBase + vpp->extradata.offset;
        uint32_t len = size;
        DEBUG_EXTRADATA_LOG("VppExtra: dump extradata: %u-%p", buf.bufferID, otherExtra);
        while (len >= sizeof(OMX_OTHER_EXTRADATATYPE)) {
            mCmp->print_debug_extradata((OMX_OTHER_EXTRADATATYPE *)otherExtra);
            if (((OMX_OTHER_EXTRADATATYPE *)otherExtra)->eType == OMX_ExtraDataNone) {
                break;
            }
            otherExtra += ((OMX_OTHER_EXTRADATATYPE *)otherExtra)->nSize;
            len -= ((OMX_OTHER_EXTRADATATYPE *)otherExtra)->nSize;
        }
    }

    if (omx->nFlags & OMX_BUFFERFLAG_EXTRADATA) {
        bool isCopyDone = false;
        if (mType == EXTRADATA_EXTERNAL) {
            DEBUG_EXTRADATA_LOG("VppExtra: fill client buffer by external request");
            uint32_t index = buf.bufferID;
            bool isClientValid = index < mCacheBuffers.size() && mCacheBuffers[index];
            if (isClientValid && size) {
                uint8_t *pVppExtraData = (uint8_t *)mBase + vpp->extradata.offset;
                uint8_t *pClientExtraData = (uint8_t *)mCacheBuffers[index];
                DEBUG_EXTRADATA_LOG("VppExtra: copy extra data(index:%u, size:%u) from vpp(%p) to client(%p)",
                        index, size, pVppExtraData, pClientExtraData);
                memcpy(pClientExtraData, pVppExtraData, size);
                isCopyDone = true;
            }
        }
        if (!isCopyDone) {
            DEBUG_EXTRADATA_LOG("VppExtra: remove OMX Extradata flag not to notify the client");
            omx->nFlags &= ~OMX_BUFFERFLAG_EXTRADATA;
        }
    }
}
