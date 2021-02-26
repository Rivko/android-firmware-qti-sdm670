/*!
 * @file VppClient.cpp
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
#include <cutils/native_handle.h>

#define VPP_LOG_TAG     VPP_LOG_IPC_CLIENT_TAG
#define VPP_LOG_MODULE  VPP_LOG_IPC_CLIENT
#include "vpp_dbg.h"
#include "vpp.h"
#include "vpp_def.h"
#include "vpp_ion.h"
#include "buf_pool.h"
#include "VppClient.h"
#include "HidlVppUtils.h"
#include "HidlVppCallbacks.h"

namespace android {

// Hidl VppCallbacks Implementation
using vendor::qti::hardware::vpp::V1_1::implementation::HidlVppCallbacks;

enum
{
    // The time taken for the vpp constructor
    PL_VPPCLIENT_CONSTRUCTOR,
    // The time taken for the vpp init
    PL_VPPCLIENT_INIT,
    // The time taken for the vpp term
    PL_VPPCLIENT_TERM,
    // The time taken for the vpp set_ctrl
    PL_VPPCLIENT_SETCTRL,
    // The time taken for the vpp set_param
    PL_VPPCLIENT_SETPARAM,
    // The time taken for the vpp quebuf
    PL_VPPCLIENT_QUEUE_BUF_IN,
    // The time taken for the vpp quebuf
    PL_VPPCLIENT_QUEUE_BUF_OUT,
    // The time taken for the vpp quebuf
    PL_VPPCLIENT_RECONFIG,
    // The time taken for the vpp flush
    PL_VPPCLIENT_FLUSH,
    // The time taken for the vpp set_vid_prop
    PL_VPPCLIENT_SETVIDPROP,
    PL_VPPCLIENT_STATS_MAX,
};

// Configure characteristics for each individual stat
static const t_StVppStatsConfig astVppStatsCfg[] =
{
    VPP_PROF_DECL(PL_VPPCLIENT_CONSTRUCTOR, 1, 1),
    VPP_PROF_DECL(PL_VPPCLIENT_INIT, 1, 1),
    VPP_PROF_DECL(PL_VPPCLIENT_TERM, 1, 1),
    VPP_PROF_DECL(PL_VPPCLIENT_SETCTRL, 1, 1),
    VPP_PROF_DECL(PL_VPPCLIENT_SETPARAM, 1, 1),
    VPP_PROF_DECL(PL_VPPCLIENT_QUEUE_BUF_IN, 1, 1),
    VPP_PROF_DECL(PL_VPPCLIENT_QUEUE_BUF_OUT, 1, 1),
    VPP_PROF_DECL(PL_VPPCLIENT_RECONFIG, 1, 1),
    VPP_PROF_DECL(PL_VPPCLIENT_FLUSH, 1, 1),
    VPP_PROF_DECL(PL_VPPCLIENT_SETVIDPROP, 1, 1)
};

static const uint32_t u32VppStatsCnt = VPP_STATS_CNT(astVppStatsCfg);

// Callback function to return the VppRequirements during setCtrl() call
void vppReq_cb(const VppRequirements& vppSetCtrlRet)
{
    LOGD("vppReq_cb %u, fmt_mask=0x%llx",
         vppSetCtrlRet.metadata.cnt,
         (unsigned long long)vppSetCtrlRet.inColorFmtMask);

    VppClient *client = (VppClient *)vppSetCtrlRet.cookie;
    if (client != NULL)
        client->setVppRequirements(vppSetCtrlRet);
    else
        LOGE("vppReq_cb: the cookie returned is NULL");
}

void VppClient::setVppRequirements(const VppRequirements& stVppReq)
{
    mVppReq = stVppReq;
}

VppRequirements& VppClient::getVppRequirements(void)
{
    return mVppReq;
}

void VppClient::VppClientDeathRecipient::serviceDied(uint64_t cookie,
                  const ::android::wp<::android::hidl::base::V1_0::IBase>& who)
{
    struct vpp_event e;

    LOGE("vpp service died. cookie: %llu, who: %p",
         static_cast<unsigned long long>(cookie), &who);

    /*
    * Set the vpp client bypass flag to true
    * The queuebuf can use this to directly route the INPUT buffer
    * to OUTPUT_CALLBACK.
    */
    {
        Mutex::Autolock a(mClient->mVppClientMutex);
        mClient->mVppClientBypass = true;
    }

    e.type = VPP_EVENT_ERROR;
    // Trigger an ERROR event to the OMX module.
    if (mClient->mCb.vpp_event)
        mClient->mCb.vpp_event(mClient->mCb.pv, e);

    /* clear/flush all the buffers and return it back to the client*/
    mClient->clearVppBufs();
}

VppClient::VppClient() :
    mDeathRecipient(new VppClientDeathRecipient(this)),
    mVppClientBypass(false)
{
    // Initialize the vpp stats module
    uint32_t u32Ret = VPP_OK;
    uint32_t u32StatsEn;

    // Read properties, debug-controls
    readProperties(&u32StatsEn);

    memset(&mStCtx, 0, sizeof(mStCtx));

    // Set mStatHandle to NULL, in case the init returns error, stop will also retun error
    mStatHandle = NULL;
    u32Ret = u32VppStats_Init(&mStCtx);
    if (u32Ret == VPP_OK)
    {
        u32Ret = u32VppStats_SetEnable(&mStCtx, u32StatsEn);
        LOGE_IF(u32Ret != VPP_OK, "u32VppStats_SetEnable returned error %u", u32Ret);

        // Register the calls or stats
        u32Ret = u32VppStats_Register(&mStCtx, astVppStatsCfg, u32VppStatsCnt, &mStatHandle);
        LOGE_IF(u32Ret != VPP_OK, "u32VppStats_Register returned error %u", u32Ret);

        u32Ret = u32VppStats_Start(&mStCtx, mStatHandle, PL_VPPCLIENT_CONSTRUCTOR);
        LOGE_IF(u32Ret != VPP_OK, "u32VppStats_Start returned error %u", u32Ret);
    }
    else
        LOGE("u32VppStats_Init returned error %u", u32Ret);

    // Get the Vpp Service from the Service manager
    mVppService = IHidlVppService::tryGetService("vppService");
    if (mVppService == NULL)
    {
        LOGE("ERROR: unable to acquire vpp service...");
        goto ERR_VPPCLIENT;
    }

    // Get the Vpp Service from the Service manager
    mVppSession = mVppService->getNewVppSession(0);
    if (mVppSession == NULL)
    {
        LOGE("vppSession is null");
        goto ERR_VPPCLIENT;
    }

    // This callback object handle is sent to the Service during the 'init()' call
    mCallback = new HidlVppCallbacks(this);
    if (mCallback == NULL)
    {
        LOGE("mCallback is null");
        goto ERR_VPPCLIENT;
    }

    // The 'HidlVppDeathRecipient::servicedied()' method gets called whenever the service dies.
    mVppService->linkToDeath(mDeathRecipient, 0 /*cookie*/);

ERR_VPPCLIENT:
    u32Ret = u32VppStats_Stop(&mStCtx, mStatHandle, PL_VPPCLIENT_CONSTRUCTOR);
    LOGE_IF(u32Ret != VPP_OK, "u32VppStats_Stop returned error %u", u32Ret);
}

VppClient::~VppClient()
{
    uint32_t u32Ret = VPP_OK;

    LOGI("destructor called, this=%p", this);

    // Terminate the session in case the destructor gets called before the term()
    this->term();
    if (mVppService != NULL)
    {
        mVppService->unlinkToDeath(mDeathRecipient);
        mVppService = NULL;
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

void *VppClient::init(uint32_t flags, struct vpp_callbacks cb)
{
    uint32_t u32Ret = VPP_OK;

    // Check if the Vpp session is available
    if (mVppSession == NULL)
        return NULL;

    // Reset the Client Buffer Ids
    mVppClientBufId = 0;

    /*
    * Callbacks from the calling module needs to be stored
    * VppClientCallback methods will use it trigger the callback
    * to the calling module.
    */
    mCb = cb;

    u32VppStats_Start(&mStCtx, mStatHandle, PL_VPPCLIENT_INIT);
    u32Ret = mVppSession->vppInit(flags, mCallback);
    u32VppStats_Stop(&mStCtx, mStatHandle, PL_VPPCLIENT_INIT);

    return ((u32Ret == VPP_OK) ? this : NULL);
}

void VppClient::term()
{
    // Check if the Vpp session is available
    if (mVppSession == NULL)
        return;

    u32VppStats_Start(&mStCtx, mStatHandle, PL_VPPCLIENT_TERM);
    mVppSession->vppTerm();
    u32VppStats_Stop(&mStCtx, mStatHandle, PL_VPPCLIENT_TERM);
}

uint32_t VppClient::setCtrl(struct hqv_control ctrl, struct vpp_requirements *req)
{
    uint32_t u32Ret = VPP_OK;
    HqvControl stHqvCtrl;
    VppRequirements stVppReq;

    if (mVppSession == NULL)
        return VPP_ERR_STATE;

    if (req == NULL)
        return VPP_ERR_PARAM;

    // Convert the data structures to Hidl compatible data
    u32Ret = hqvCtrlToHidl(ctrl, stHqvCtrl);
    if (u32Ret != VPP_OK)
    {
        LOGE("VppClient::setCtrl hqvCtrlToHidl returned Err=%u", u32Ret);
        return u32Ret;
    }

    u32VppStats_Start(&mStCtx, mStatHandle, PL_VPPCLIENT_SETCTRL);
    mVppSession->vppSetCtrl((uint64_t)this, stHqvCtrl, vppReq_cb);
    u32VppStats_Stop(&mStCtx, mStatHandle, PL_VPPCLIENT_SETCTRL);

    stVppReq = getVppRequirements();
    u32Ret = hidlToVppReq(req, stVppReq);
    if (u32Ret != VPP_OK)
    {
        LOGE("VppClient::setCtrl hidlToVppReq returned Err=%u", u32Ret);
        return u32Ret;
    }

    return stVppReq.retStatus;
}

uint32_t VppClient::setParameter(enum vpp_port port, struct vpp_port_param param)
{
    uint32_t u32Ret = VPP_OK;
    VppPortParam stParam;

    if (mVppSession == NULL)
        return VPP_ERR_STATE;

    // Convert the data structures to Hidl compatible data
    vppPortParamToHidl(param, stParam);

    u32VppStats_Start(&mStCtx, mStatHandle, PL_VPPCLIENT_SETPARAM);
    u32Ret = mVppSession->vppSetParameter((VppPort)port, stParam);
    u32VppStats_Stop(&mStCtx, mStatHandle, PL_VPPCLIENT_SETPARAM);

    return u32Ret;
}

uint32_t VppClient::queueBuf(enum vpp_port ePort, struct vpp_buffer *buf)
{
    uint32_t u32Ret = VPP_OK, u32Key;
    VppBuffer stVppBuf;
    bool bVppBypass;

    if (mVppSession == NULL)
        return VPP_ERR_STATE;

    if (buf == NULL)
        return VPP_ERR_PARAM;

    /*
    * Register the queued buffer.In case of the Service crashes/dies the
    * available registered buffers will be returned to the client(caller module).
    * The buffers are deregistered in the IBD/OBD callbacks.
    */
    VppClientBuf sClientBuf = {
        .ePortType = ePort,
        .bufferId = getNextBufferId(),
        .pstBuffer = buf,
    };

    registerVppBuf(sClientBuf);

    {
        Mutex::Autolock a(mVppClientMutex);
        bVppBypass = mVppClientBypass;
    }
    LOGI("VppClient:queueBuf Id:%u bypass:%u port:%u ts=%" PRIu64 " ",
         sClientBuf.bufferId, bVppBypass, (uint32_t)ePort, buf->timestamp);

    // If in bypass then send this buffer directly to the OBD
    if (false == bVppBypass)
    {
        // Convert the data structures to Hidl compatible data
        u32Ret = vppBufferToHidl(buf, stVppBuf, VPP_FALSE);
        if (u32Ret != VPP_OK)
        {
            deregisterVppBuf(sClientBuf.bufferId);
            LOGE("VppClient::queueBuf vppBufferToHidl returned Err=%u", u32Ret);
            return u32Ret;
        }

        // Get the next available Buffer id
        stVppBuf.bufferId = sClientBuf.bufferId;

        u32Key = (ePort==VPP_PORT_INPUT) ? PL_VPPCLIENT_QUEUE_BUF_IN : PL_VPPCLIENT_QUEUE_BUF_OUT;

        u32VppStats_Start(&mStCtx, mStatHandle, u32Key);
        u32Ret = mVppSession->vppQueueBuf((VppPort)ePort, stVppBuf);
        if (u32Ret != VPP_OK)
        {
            deregisterVppBuf(sClientBuf.bufferId);
            LOGE("VppClient::queueBuf vppQueueBuf returned Err=%u", u32Ret);
        }
        u32VppStats_Stop(&mStCtx, mStatHandle, u32Key);

        releaseNativeHandle(stVppBuf.pixel.handleFd.getNativeHandle());
        releaseNativeHandle(stVppBuf.extradata.handleFd.getNativeHandle());
    }
    else
    {
        clearVppBufs();
    }

    return u32Ret;
}

uint32_t VppClient::reconfigure(struct vpp_port_param input_param,
                                struct vpp_port_param output_param)
{
    uint32_t u32Ret = VPP_OK;
    VppPortParam stInPortParam, stOutPortParam;

    if (mVppSession == NULL)
        return VPP_ERR_STATE;

    // Convert the data structures to Hidl compatible data
    vppPortParamToHidl(input_param, stInPortParam);
    vppPortParamToHidl(output_param, stOutPortParam);

    u32VppStats_Start(&mStCtx, mStatHandle, PL_VPPCLIENT_RECONFIG);
    u32Ret = mVppSession->vppReconfigure(stInPortParam, stOutPortParam);
    u32VppStats_Stop(&mStCtx, mStatHandle, PL_VPPCLIENT_RECONFIG);

    return u32Ret;
}

uint32_t VppClient::flush(enum vpp_port ePort)
{
    uint32_t u32Ret = VPP_OK;
    bool bVppBypass;

    if (mVppService == NULL)
        return VPP_ERR_STATE;

    {
        Mutex::Autolock a(mVppClientMutex);
        bVppBypass = mVppClientBypass;
    }
    // Return all the OUTPUT-PORT buffers if in bypass.
    if (true == bVppBypass)
    {
        struct vpp_event e;
        if (VPP_PORT_OUTPUT == ePort)
        {
            /*
            * Copy the contents of the container to a temporary and clear its contents
            * The copied buffer references will be used for Callbacks.
            */
            std::vector<VppClientBuf> lVppBufs;
            {
                Mutex::Autolock a(mVppClientMutex);
                lVppBufs = mVppClientFlushBufs;
                mVppClientFlushBufs.clear();
            }

            for (VppClientBuf &stBuf : lVppBufs)
            {
                mCb.output_buffer_done(mCb.pv, stBuf.pstBuffer);
            }
        }
        /* Send the FLUSH_DONE event for the port being requested*/
        e.type = VPP_EVENT_FLUSH_DONE;
        e.flush_done.port = ePort;
        mCb.vpp_event(mCb.pv, e);
    }
    else
    {
        u32VppStats_Start(&mStCtx, mStatHandle, PL_VPPCLIENT_FLUSH);
        u32Ret = mVppSession->vppFlush((VppPort)ePort);
        u32VppStats_Stop(&mStCtx, mStatHandle, PL_VPPCLIENT_FLUSH);
    }

    return u32Ret;
}

uint32_t VppClient::setVidProp(struct video_property prop)
{
    uint32_t u32Ret = VPP_OK;
    VideoProperty stProp;

    if (mVppSession == NULL)
        return VPP_ERR_STATE;

    // Convert the data structures to Hidl compatible data
    u32Ret = vppPropToHidl(prop, stProp);
    if (u32Ret != VPP_OK)
        return VPP_ERR_STATE;

    u32VppStats_Start(&mStCtx, mStatHandle, PL_VPPCLIENT_SETVIDPROP);
    u32Ret = mVppSession->vppSetVidProp(stProp);
    u32VppStats_Stop(&mStCtx, mStatHandle, PL_VPPCLIENT_SETVIDPROP);

    return u32Ret;
}

// Register the VppClientBuf to the pool
bool VppClient::registerVppBuf(VppClientBuf buf)
{
    Mutex::Autolock a(mVppClientMutex);

    // Register the buffer to Vpp Client buffer container
    mVppClientBufs.push_back(buf);
    LOGI("VppClient::registerVppBuf buffer with size %u id %u ts=%" PRIu64 " flags:%u",
         (uint32_t)mVppClientBufs.size(), buf.bufferId,
         buf.pstBuffer->timestamp, buf.pstBuffer->flags);

    return true;
}

// Deregister the VppClientBuf from the pool
struct vpp_buffer* VppClient::deregisterVppBuf(uint32_t bufId)
{
    struct vpp_buffer *pstVppBuf = NULL;
    Mutex::Autolock a(mVppClientMutex);

    // Find the buffer corresponding to the cookie and deregister it
    for (unsigned i = 0; i < mVppClientBufs.size(); ++i)
    {
        if (mVppClientBufs[i].bufferId == bufId)
        {
            pstVppBuf = mVppClientBufs[i].pstBuffer;
            // Erase it from the container
            mVppClientBufs.erase(mVppClientBufs.begin()+i);
            LOGI("VppClient:de-registered buffer with size %u bufId %u ",
                 (uint32_t)mVppClientBufs.size(), bufId);
        }
    }

    return pstVppBuf;
}

// De-Register and return all the VppClientBufs from the pool
bool VppClient::clearVppBufs( void )
{
    /*
    * Copy the contents of the container to a temporary and clear its contents
    * The copied buffer references will be used for Callbacks.
    */
    std::vector<VppClientBuf> lVppBufs;
    {
        Mutex::Autolock a(mVppClientMutex);
        lVppBufs = mVppClientBufs;
        mVppClientBufs.clear();
    }

    LOGI("VppClient::clearVppBufs size %u",(uint32_t)lVppBufs.size());
    for (VppClientBuf &stBuf : lVppBufs)
    {
        /*
        * The output buffer is not yet filled up might contain garbage data.
        * The input buffer can still be used for displaying.
        */
        if(stBuf.ePortType == VPP_PORT_OUTPUT)
        {
            // Set the filled_len to 0, Omx will recycle the buffer
            stBuf.pstBuffer->pixel.filled_len = 0;
            mVppClientFlushBufs.push_back(stBuf);
        }
        else
        {
            mCb.output_buffer_done(mCb.pv, stBuf.pstBuffer);
        }
    }

    return true;
}

// Returns the next buffer id
uint32_t VppClient::getNextBufferId(void)
{
    return (mVppClientBufId++);
}

}
