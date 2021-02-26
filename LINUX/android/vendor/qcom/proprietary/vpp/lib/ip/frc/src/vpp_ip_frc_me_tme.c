/*!
 * @file vpp_ip_frc_me_tme.c
 *
 * @cr
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * @services: Interface to Motion Estimation(TME)
 */

#include <sys/types.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>

#define VPP_LOG_TAG     VPP_LOG_MODULE_FRC_ME_TAG
#define VPP_LOG_MODULE  VPP_LOG_MODULE_FRC_ME

#include "vpp_dbg.h"
#include "vpp.h"
#include "vpp_buf.h"
#include "vpp_ion.h"
#include "vpp_buf.h"
#include "vpp_callback.h"
#include "vpp_reg.h"
#include "vpp_queue.h"
#include "vpp_ip.h"
#include "vpp_utils.h"
#include "vpp_stats.h"
#include "vpp_ip_frc_me.h"
#include "vpp_ip_frc_me_tme.h"

/************************************************************************
 * Local definitions
 ***********************************************************************/
#define FRC_TME_POLL_TIMEOUT            1000      // 1 sec timeout
#define FRC_TME_LOG_IOCTL_ENABLE        0

#if FRC_TME_LOG_IOCTL_ENABLE
static uint32_t u32VppIpTme_LogIoctl(uint32_t fd, char *cmd, const char *func);
#define IOCTL(fd, c, p)        ioctl(fd, c, u32VppIpTme_LogIoctl(fd, (char *)#c, __func__) ? p : p)
#else
#define IOCTL(fd, c, p)        ioctl(fd, c, p)
#endif

/************************************************************************
 * Local static variables
 ***********************************************************************/
static const int aMeEventType[2] = {
    V4L2_EVENT_MSM_VIDC_FLUSH_DONE,
    V4L2_EVENT_MSM_VIDC_SYS_ERROR,
};

static int aMeEventType_size = sizeof(aMeEventType)/sizeof(int);

/************************************************************************
 * Forward Declarations
 *************************************************************************/

/************************************************************************
 * Local Functions
 ************************************************************************/
#if FRC_TME_LOG_IOCTL_ENABLE
static uint32_t u32VppIpTme_LogIoctl(uint32_t fd, char *cmd, const char *func)
{
    LOGE("ioctl from:%s fd:%d cmd:%s\n", func, fd, cmd);
    return VPP_OK;
}
#endif

static uint32_t u32VppIpTme_SetSecureSession(t_StVppIpFrcMeCb *pstCb)
{
    int rc;
    struct v4l2_control stControl;
    uint32_t u32Ret = VPP_OK;

    stControl.id = V4L2_CID_MPEG_VIDC_VIDEO_SECURE;
    stControl.value = 1;
    LOGI("%s Open Secure device", __func__);
    rc = IOCTL(pstCb->MeFd, VIDIOC_S_CTRL, &stControl);
    if (rc)
    {
        LOGE("Open secure device failed, rc=%d", rc);
        u32Ret = VPP_ERR;
    }

    return u32Ret;
}

// Set the Output format (CAPTURE_MPLANE)
static uint32_t u32VppIpTme_SetOutputFormat(t_StVppIpFrcMeCb *pstCb)
{
    int rc;
    struct v4l2_format stMeFormat;
    uint32_t u32Ret = VPP_OK;

    memset(&stMeFormat, 0, sizeof(stMeFormat));
    stMeFormat.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    stMeFormat.fmt.pix_mp.height = pstCb->stOutput.stParam.height;
    stMeFormat.fmt.pix_mp.width = pstCb->stOutput.stParam.width;
    stMeFormat.fmt.pix_mp.pixelformat = V4L2_PIX_FMT_TME;
    rc = IOCTL(pstCb->MeFd, VIDIOC_S_FMT, &stMeFormat);
    if (rc)
    {
        LOGE("Failed to set the output format, rc=%d", rc);
        u32Ret = VPP_ERR;
    }
    LOGI("Output buffer size = %d", stMeFormat.fmt.pix_mp.plane_fmt[0].sizeimage);

    return u32Ret;
}

// Setup the Output Profile and Level
static uint32_t u32VppIpTme_SetProfileLevel(t_StVppIpFrcMeCb *pstCb)
{
    int rc;
    struct v4l2_control stControl;
    uint32_t u32Ret = VPP_OK;

    // Set TME Codec Profile to PROFILE_1 (FRC , Bidirectional MV)
    stControl.id = V4L2_CID_MPEG_VIDC_VIDEO_TME_PROFILE;
    stControl.value = V4L2_MPEG_VIDC_VIDEO_TME_PROFILE_1;
    rc = IOCTL(pstCb->MeFd, VIDIOC_S_CTRL, &stControl);
    if (rc)
    {
        LOGE("Failed to set V4L2_CID_MPEG_VIDC_VIDEO_TME_PROFILE, rc=%d", rc);
        u32Ret = VPP_ERR;
    }
    else
    {
        // Set Profile Level to LEVEL_INTEGER
        stControl.id = V4L2_CID_MPEG_VIDC_VIDEO_TME_LEVEL;
        stControl.value =  V4L2_MPEG_VIDC_VIDEO_TME_LEVEL_INTEGER;
        rc = IOCTL(pstCb->MeFd, VIDIOC_S_CTRL, &stControl);
        if (rc)
        {
            LOGE("Failed to set V4L2_CID_MPEG_VIDC_VIDEO_TME_LEVEL, rc=%d", rc);
            u32Ret = VPP_ERR;
        }
    }

    return u32Ret;
}

// Set the TME Input format(OUTPUT_MPLANE)
static uint32_t u32VppIpTme_SetInputFormat(t_StVppIpFrcMeCb *pstCb)
{
    int rc;
    struct v4l2_format stMeFormat;
    uint32_t u32Ret = VPP_OK;

    memset(&stMeFormat, 0, sizeof(stMeFormat));
    stMeFormat.type = V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE;
    stMeFormat.fmt.pix_mp.height = pstCb->stInput.stParam.height;
    stMeFormat.fmt.pix_mp.width = pstCb->stInput.stParam.width;
    stMeFormat.fmt.pix_mp.pixelformat = V4L2_PIX_FMT_NV12_UBWC;
    rc = IOCTL(pstCb->MeFd, VIDIOC_S_FMT, &stMeFormat);
    if (rc)
    {
        LOGE("Failed to set the input format, rc=%d", rc);
        u32Ret = VPP_ERR;
    }
    LOGI("Input buffer size = %d", stMeFormat.fmt.pix_mp.plane_fmt[0].sizeimage);

    return u32Ret;
}

// Setup the input frame rate, this will be used by the driver to program the internal TME clocks
static uint32_t u32VppIpTme_SetFrameRate(t_StVppIpFrcMeCb *pstCb)
{
    int rc;
    struct v4l2_streamparm stParam;
    uint32_t u32Ret = VPP_OK;

    stParam.type = V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE;
    stParam.parm.output.timeperframe.numerator = 1;
    stParam.parm.output.timeperframe.denominator = FRC_ME_FRAME_RATE;
    rc = IOCTL(pstCb->MeFd, VIDIOC_S_PARM, &stParam);
    if (rc)
    {
        LOGE("Failed to set VIDIOC_S_PARM (FRAME_RATE), rc=%d", rc);
        u32Ret = VPP_ERR;
    }

    return u32Ret;
}

// Read back the configured params and update the ME Ip/Op buffer sizes
static uint32_t u32VppIpTme_ReadTmeParams(t_StVppIpFrcMeCb *pstCb)
{
    int rc;
    struct v4l2_format stMeFormat;
    uint32_t u32Ret = VPP_OK;

    VPP_RET_IF_NULL(pstCb, VPP_ERR_PARAM);

    memset(&stMeFormat, 0, sizeof(stMeFormat));
    stMeFormat.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    stMeFormat.fmt.pix_mp.height = pstCb->stOutput.stParam.height;
    stMeFormat.fmt.pix_mp.width = pstCb->stOutput.stParam.width;
    stMeFormat.fmt.pix_mp.pixelformat = V4L2_PIX_FMT_TME;
    rc = IOCTL(pstCb->MeFd, VIDIOC_G_FMT, &stMeFormat);
    if (rc)
    {
        LOGE("Failed VIDIOC_G_FMT(CAPTURE_MPLANE), rc=%d", rc);
        u32Ret = VPP_ERR;
    }
    else
    {
        pstCb->u32OpBufSize = stMeFormat.fmt.pix_mp.plane_fmt[0].sizeimage;

        memset(&stMeFormat, 0, sizeof(stMeFormat));
        stMeFormat.type = V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE;
        stMeFormat.fmt.pix_mp.height = pstCb->stInput.stParam.height;
        stMeFormat.fmt.pix_mp.width = pstCb->stInput.stParam.width;
        stMeFormat.fmt.pix_mp.pixelformat = V4L2_PIX_FMT_NV12_UBWC;
        rc = IOCTL(pstCb->MeFd, VIDIOC_G_FMT, &stMeFormat);
        if (rc)
        {
            LOGE("Failed VIDIOC_G_FMT(OUTPUT_MPLANE), rc=%d", rc);
            u32Ret = VPP_ERR;
        }
        else if (stMeFormat.fmt.pix_mp.num_planes > 1)
        {
             LOGE("pix_mp.num_planes > 1 %u", stMeFormat.fmt.pix_mp.num_planes);
             u32Ret = VPP_ERR;
        }
        else
            pstCb->u32IpBufSize = stMeFormat.fmt.pix_mp.plane_fmt[0].sizeimage;
    }
    LOGI("Read from TME: OpBufSize=%d IpBufSize=%d", pstCb->u32OpBufSize, pstCb->u32IpBufSize);

    return u32Ret;
}

static uint32_t u32VppIpTme_SubscribeToEvents(t_StVppIpFrcMeCb *pstCb)
{
    int i, rc;
    struct v4l2_event_subscription stMeSub;
    uint32_t u32Ret = VPP_OK;

    for (i = 0; i < aMeEventType_size; i++)
    {
        memset(&stMeSub, 0, sizeof(stMeSub));
        stMeSub.type = aMeEventType[i];
        rc = IOCTL(pstCb->MeFd, VIDIOC_SUBSCRIBE_EVENT, &stMeSub);
        if (rc)
        {
            LOGE("TME failed to subscribe to the event 0x%x rc=%d", stMeSub.type, rc);
            u32Ret = VPP_ERR;
        }
    }

    return u32Ret;
}

static uint32_t u32VppIpTme_UnSubscribeToEvents(t_StVppIpFrcMeCb *pstCb)
{
    int i, rc;
    struct v4l2_event_subscription stMeSub;
    uint32_t u32Ret = VPP_OK;

    for (i = 0; i < aMeEventType_size; i++)
    {
        memset(&stMeSub, 0, sizeof(stMeSub));
        stMeSub.type = aMeEventType[i];
        rc = IOCTL(pstCb->MeFd, VIDIOC_UNSUBSCRIBE_EVENT, &stMeSub);
        if (rc)
        {
            LOGE("TME failed to unSubscribe to the event 0x%x rc=%d", stMeSub.type, rc);
            u32Ret = VPP_ERR;
        }
    }

    return u32Ret;
}

// Request driver input buffer queue (TME returns minimum required)
static uint32_t u32VppIpTme_RequestInputBufQueue(t_StVppIpFrcMeCb *pstCb)
{
    int rc;
    struct v4l2_control stControl;
    struct v4l2_requestbuffers stMeBufReq;
    uint32_t u32Ret = VPP_OK;

    stControl.id = V4L2_CID_MIN_BUFFERS_FOR_OUTPUT;
    rc = IOCTL(pstCb->MeFd, VIDIOC_G_CTRL, &stControl);
    if (rc)
    {
        LOGE("Failed V4L2_CID_MIN_BUFFERS_FOR_OUTPUT, rc=%d", rc);
        u32Ret = VPP_ERR;
    }
    else
    {
        pstCb->u32IpBufsTmeQSize = stControl.value;
        if (pstCb->u32IpBufsTmeQSize > FRC_ME_MAX_IN_BUFS)
            pstCb->u32IpBufsTmeQSize = FRC_ME_MAX_IN_BUFS;

        memset(&stMeBufReq, 0, sizeof(stMeBufReq));
        stMeBufReq.memory = V4L2_MEMORY_USERPTR;
        stMeBufReq.count = pstCb->u32IpBufsTmeQSize;
        stMeBufReq.type = V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE;
        rc = IOCTL(pstCb->MeFd, VIDIOC_REQBUFS, &stMeBufReq);
        if (rc)
        {
            LOGE("Failed input VIDIOC_REQBUFS, rc=%d", rc);
            u32Ret = VPP_ERR;
        }
        LOGI("Input properties: mincount=%u", pstCb->u32IpBufsTmeQSize);
    }

    return u32Ret;
}

static uint32_t u32VppIpTme_ReleaseInputBufQueue(t_StVppIpFrcMeCb *pstCb)
{
    int rc;
    struct v4l2_requestbuffers stBufReq;
    uint32_t u32Ret = VPP_OK;

    stBufReq.type = V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE;
    stBufReq.memory = V4L2_MEMORY_USERPTR;
    stBufReq.count = 0;
    rc = IOCTL(pstCb->MeFd, VIDIOC_REQBUFS, &stBufReq);
    if (rc)
    {
        LOGE("Failed input VIDIOC_REQBUFS-OUTPUT_MPLANE, rc=%d", rc);
        u32Ret = VPP_ERR;
    }

    return u32Ret;
}

// Request driver output buffer queue (TME returns miniumum required)
static uint32_t u32VppIpTme_RequestOutputBufQueue(t_StVppIpFrcMeCb *pstCb)
{
    int rc;
    struct v4l2_control stControl;
    struct v4l2_requestbuffers stMeBufReq;
    uint32_t u32Ret = VPP_OK;

    stControl.id = V4L2_CID_MIN_BUFFERS_FOR_CAPTURE;
    rc = IOCTL(pstCb->MeFd, VIDIOC_G_CTRL, &stControl);
    if (rc)
    {
        LOGE("Failed V4L2_CID_MIN_BUFFERS_FOR_CAPTURE, rc=%d", rc);
        u32Ret = VPP_ERR;
    }
    else
    {
        pstCb->u32OpBufsTmeQSize = stControl.value;
        if (pstCb->u32OpBufsTmeQSize > FRC_ME_MAX_OUT_BUFS)
            pstCb->u32OpBufsTmeQSize = FRC_ME_MAX_OUT_BUFS;

        stMeBufReq.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
        stMeBufReq.count = pstCb->u32OpBufsTmeQSize;
        rc = IOCTL(pstCb->MeFd, VIDIOC_REQBUFS, &stMeBufReq);
        if (rc)
        {
            LOGE("Failed Output VIDIOC_REQBUFS-CAPTURE_MPLANE, rc=%d", rc);
            u32Ret = VPP_ERR;
        }
        LOGI("Output properties: mincount=%u", pstCb->u32OpBufsTmeQSize);
    }

    return u32Ret;
}

static uint32_t u32VppIpTme_ReleaseOutputBufQueue(t_StVppIpFrcMeCb *pstCb)
{
    int rc;
    struct v4l2_requestbuffers stBufReq;
    uint32_t u32Ret = VPP_OK;

    stBufReq.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    stBufReq.memory = V4L2_MEMORY_USERPTR;
    stBufReq.count = 0;
    rc = IOCTL(pstCb->MeFd, VIDIOC_REQBUFS, &stBufReq);
    if (rc)
    {
        LOGE("Failed input VIDIOC_REQBUFS-CAPTURE_MPLANE, ret=%d", rc);
        u32Ret = VPP_ERR;
    }

    return u32Ret;
}

// Set TME stream On for the port specified
static uint32_t u32VppIpTme_StreamOn(t_StVppIpFrcMeCb *pstCb, enum vpp_port ePort)
{
    int rc;
    uint32_t u32Port, u32Ret = VPP_OK;

    VPP_RET_IF_NULL(pstCb, VPP_ERR_PARAM);

    if (ePort == VPP_PORT_OUTPUT)
        u32Port = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    else
        u32Port = V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE;

    if (pstCb->auMeStream[ePort] == VPP_FALSE)
    {
        int rc = IOCTL(pstCb->MeFd, VIDIOC_STREAMON, &u32Port);
        if (rc)
        {
            LOGE("VIDIOC_STREAMON (%d) Failed rc = %d", ePort, rc);
            u32Ret = VPP_ERR;
        }
        else
            pstCb->auMeStream[ePort] = VPP_TRUE;
    }

    return u32Ret;
}

// Set TME stream Off for the port specified
static uint32_t u32VppIpTme_StreamOff(t_StVppIpFrcMeCb *pstCb, enum vpp_port ePort)
{
    int rc;
    t_StVppIpCmd stCmd;
    uint32_t u32Port, u32Ret = VPP_OK;

    VPP_RET_IF_NULL(pstCb, VPP_ERR_PARAM);

    if (ePort == VPP_PORT_OUTPUT)
        u32Port = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    else
        u32Port = V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE;

    rc = IOCTL(pstCb->MeFd, VIDIOC_STREAMOFF, &u32Port);
    if (rc)
    {
        LOGE("VIDIOC_STREAMOFF (%d) Failed rc=%d", ePort, rc);
        u32Ret = VPP_ERR;
    }
    else
        pstCb->auMeStream[ePort] = VPP_FALSE;

    return u32Ret;
}

/************************************************************************
 * Global Functions
 ************************************************************************/

// Open TME driver and create fd for notification.
uint32_t u32VppIpFrcMe_TmeOpenDev(t_StVppIpFrcMeCb *pstCb)
{
    LOG_ENTER();

    char *pcMeDeviceName;

    VPP_RET_IF_NULL(pstCb, VPP_ERR_PARAM);

    //Open ME driver
    pcMeDeviceName = (char*)"/dev/video33";
    pstCb->MeFd = -1;
    pstCb->MeFd = open(pcMeDeviceName, O_RDWR);

    if (pstCb->MeFd == 0)
    {
        LOGE("Got MeFd as 0 for msm_vidc_enc, Opening again!\n");
        pstCb->MeFd = open(pcMeDeviceName, O_RDWR);
    }

    if (pstCb->MeFd < 0)
    {
        LOGE("MeFd=%d <0. Open() failed! \n", (int)pstCb->MeFd);
        LOG_EXIT_RET(VPP_ERR_STATE);
    }
    LOGI("MeFd=%d success\n", pstCb->MeFd);

    //Create fd for event notification
    pstCb->MePollNotification = eventfd(0, 0);
    if (pstCb->MePollNotification < 0)
    {
        LOGE("MePollNotification=%d <0. Open() failed!\n", pstCb->MePollNotification);
        close(pstCb->MeFd);
        pstCb->MeFd = -1;
        LOG_EXIT_RET(VPP_ERR_STATE);
    }

    LOG_EXIT_RET(VPP_OK);
}

// Close TME driver and fd for notification.
uint32_t u32VppIpFrcMe_TmeCloseDev(t_StVppIpFrcMeCb *pstCb)
{
    LOG_ENTER();

    VPP_RET_IF_NULL(pstCb, VPP_ERR_PARAM);

    if (pstCb->MePollNotification > 0)
    {
        close(pstCb->MePollNotification);
        pstCb->MePollNotification = -1;
    }

    if (pstCb->MeFd > 0)
    {
        close(pstCb->MeFd);
        pstCb->MeFd = -1;
    }

    LOGI("TME device is closed now");
    LOG_EXIT_RET(VPP_OK);
}

uint32_t u32VppIpFrcMe_TmeAcquireResources(t_StVppIpFrcMeCb *pstCb)
{
    LOG_ENTER();

    uint32_t u32, u32Ret = VPP_OK;

    VPP_RET_IF_NULL(pstCb, VPP_ERR_PARAM);

    // Subscribe to the events
    u32Ret = u32VppIpTme_SubscribeToEvents(pstCb);
    if (u32Ret)
    {
        LOGE("Failed to subscribe to the events 0x%x", u32Ret);
        LOG_EXIT_RET(u32Ret);
    }

    // Set Output format
    u32Ret = u32VppIpTme_SetOutputFormat(pstCb);
    if (u32Ret)
    {
        LOGE("Failed to set the output format, ret=%u", u32Ret);
        goto ERROR_SET_OUT_FORMAT;
    }

    // Set Codec Profile & Level
    u32Ret = u32VppIpTme_SetProfileLevel(pstCb);
    if (u32Ret)
    {
        LOGE("Failed to set the output profile/level, ret=%u", u32Ret);
        goto ERROR_SET_PROF_LVL;
    }

    // Set input port format (NV12_UBWC only)
    u32Ret = u32VppIpTme_SetInputFormat(pstCb);
    if (u32Ret)
    {
        LOGE("Failed to set the output profile/level, ret=%u", u32Ret);
        goto ERROR_SET_INP_FORMAT;
    }

    u32Ret = u32VppIpTme_SetFrameRate(pstCb);
    if (u32Ret)
   {
       LOGE("Failed to set the output profile/level, ret=%u", u32Ret);
       goto ERROR_SET_FRAME_RATE;
   }

    // Request number of input buffers
    u32Ret = u32VppIpTme_RequestInputBufQueue(pstCb);
    if (u32Ret)
    {
        LOGE("Failed RequestInputBuffers(), ret=%u", u32Ret);
        goto ERROR_REQ_INP_BUF_Q;
    }

    // Request number of output buffers
    u32Ret = u32VppIpTme_RequestOutputBufQueue(pstCb);
    if (u32Ret)
    {
        LOGE("Failed RequestOutputBuffers(), ret=%u", u32Ret);
        goto ERROR_REQ_OUT_BUF_Q;
    }

    // Open secure device
    if (pstCb->stBase.bSecureSession)
    {
        u32Ret = u32VppIpTme_SetSecureSession(pstCb);
        if (u32Ret)
        {
            LOGE("Failed u32VppIpTme_ConfigSecureSession, ret=%u", u32Ret);
            goto ERROR_SECURE_SESS;
        }
    }

    // Read back the configured TME params
    u32Ret = u32VppIpTme_ReadTmeParams(pstCb);
    if (u32Ret)
    {
        LOGE("Failed ReadTmeParams() ret=%u", u32Ret);
        goto ERROR_READ_TME_PARAMS;
    }

    // Stream Output On
    u32Ret = u32VppIpTme_StreamOn(pstCb, VPP_PORT_OUTPUT);
    if (u32Ret != VPP_OK)
    {
        LOGE("Failed StreamOn(Output) ret=%u", u32Ret);
        goto ERROR_SET_OUT_STREAM_ON;
    }

    // Stream Input On
    u32Ret = u32VppIpTme_StreamOn(pstCb, VPP_PORT_INPUT);
    if (u32Ret != VPP_OK)
    {
        LOGE("Failed StreamOn(Input) ret=%u", u32Ret);
        goto ERROR_SET_INP_STREAM_ON;
    }

    LOG_EXIT_RET(VPP_OK);

ERROR_SET_INP_STREAM_ON:
    u32 = u32VppIpTme_StreamOff(pstCb, VPP_PORT_OUTPUT);
    LOGE_IF(u32, "u32VppIpTme_StreamOff(OUTPUT) failed ret=%u", u32);
ERROR_SET_OUT_STREAM_ON:
ERROR_READ_TME_PARAMS:
ERROR_SECURE_SESS:
    u32 = u32VppIpTme_ReleaseOutputBufQueue(pstCb);
    LOGE_IF(u32, "u32VppIpTme_ReleaseOutputBufQueue() failed ret=%u", u32);
ERROR_REQ_OUT_BUF_Q:
    u32 = u32VppIpTme_ReleaseInputBufQueue(pstCb);
    LOGE_IF(u32, "u32VppIpTme_ReleaseInputBufQueue() failed ret=%u", u32);
ERROR_REQ_INP_BUF_Q:
ERROR_SET_FRAME_RATE:
ERROR_SET_INP_FORMAT:
ERROR_SET_PROF_LVL:
ERROR_SET_OUT_FORMAT:
    u32 = u32VppIpTme_UnSubscribeToEvents(pstCb);
    LOGE_IF(u32, "u32VppIpTme_UnSubscribeToEvents() failed ret=%u", u32);
    LOG_EXIT_RET(u32Ret);
}

uint32_t u32VppIpFrcMe_TmeReleaseResources(t_StVppIpFrcMeCb *pstCb)
{
    LOG_ENTER();

    int rc;
    uint32_t u32 = VPP_OK;

    VPP_RET_IF_NULL(pstCb, VPP_ERR_PARAM);

    u32 = u32VppIpTme_StreamOff(pstCb, VPP_PORT_INPUT);
    LOGE_IF(u32, "u32VppIpTme_StreamOff Failed, ret=%u", u32);

    u32 = u32VppIpTme_ReleaseInputBufQueue(pstCb);
    LOGE_IF(u32, "u32VppIpTme_ReleaseInputBufQueue Failed, ret=%u", u32);

    u32 = u32VppIpTme_StreamOff(pstCb, VPP_PORT_OUTPUT);
    LOGE_IF(u32, "u32VppIpTme_StreamOff Failed, ret=%u", u32);

    u32 = u32VppIpTme_ReleaseOutputBufQueue(pstCb);
    LOGE_IF(u32, "u32VppIpTme_ReleaseOutputBufQueue Failed, ret=%u", u32);

    u32 = u32VppIpTme_UnSubscribeToEvents(pstCb);
    LOGE_IF(u32, "u32VppIpTme_UnSubscribeToEvents Failed, ret=%u", u32);

    LOG_EXIT_RET(VPP_OK);
}

uint32_t u32VppIpFrcMe_TmeQBuf(t_StVppIpFrcMeCb *pstCb, t_StVppBuf *pstBuf, enum vpp_port ePort)
{
    LOG_ENTER();

    int rc;
    struct v4l2_buffer stBuf;
    struct v4l2_plane astPlane[VIDEO_MAX_PLANES];

    VPP_RET_IF_NULL(pstCb, VPP_ERR_PARAM);
    VPP_RET_IF_NULL(pstBuf, VPP_ERR_PARAM);
    if (ePort >= VPP_PORT_MAX)
        return VPP_ERR_PARAM;

    memset(&stBuf, 0, sizeof(stBuf));
    memset(&astPlane, 0, sizeof(astPlane));

    stBuf.index = pstBuf->u32Idx;
    stBuf.memory = V4L2_MEMORY_USERPTR;
    stBuf.length = FRC_ME_NUM_PLANES;
    astPlane[0].length =  pstBuf->stPixel.u32AllocLen;
    astPlane[0].m.userptr =  FRC_ME_V4L2_SECURE_UNMAPPED_BUF_VAL;
    astPlane[0].data_offset =  pstBuf->stPixel.u32Offset;
    astPlane[0].reserved[0] =  pstBuf->stPixel.fd;
    astPlane[0].reserved[1] =  0;
    stBuf.m.planes = astPlane;

    if (ePort == VPP_PORT_INPUT)
    {
        stBuf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE;
        astPlane[0].bytesused = pstBuf->stPixel.u32FilledLen;

        if (VPP_FLAG_IS_SET(pstBuf->pBuf->flags, VPP_BUFFER_FLAG_EOS))
        {
            VPP_FLAG_SET(stBuf.flags, V4L2_QCOM_BUF_FLAG_EOS);
            if (VPP_FLAG_IS_SET(pstBuf->u32InternalFlags, FRC_ME_BUF_INTERN_EOS))
                LOGI("Sending Internal EOS buffer to TME");
            else
                LOGI("Sending EOS(external) buffer to TME");
        }
        // Update the timeStamp
        stBuf.timestamp.tv_sec = (pstBuf->pBuf->timestamp / FRC_ME_1SEC);
        stBuf.timestamp.tv_usec = (pstBuf->pBuf->timestamp % FRC_ME_1SEC);

        LOGI("Queue I/p buf idx=%d fd=%d bytesused=%d TS:%llu EOS=%d",
             stBuf.index, astPlane[0].reserved[0], astPlane[0].bytesused,
             (long long unsigned int)pstBuf->pBuf->timestamp,
             VPP_FLAG_IS_SET(pstBuf->pBuf->flags, VPP_BUFFER_FLAG_EOS));
    }
    else
    {
        stBuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
        astPlane[0].bytesused = 0;
        LOGI("Queue O/p buf idx=%d fd=%d", stBuf.index, astPlane[0].reserved[0]);
    }

    rc = IOCTL(pstCb->MeFd, VIDIOC_QBUF, &stBuf);
    if (rc)
    {
        LOGE("VIDIOC_QBUF (%d idx=%d) Failed rc=%d", ePort, stBuf.index, rc);
        LOG_EXIT_RET(VPP_ERR);
    }

    LOG_EXIT_RET(VPP_OK);
}

uint32_t u32VppIpFrcMe_TmeFlush(t_StVppIpFrcMeCb *pstCb)
{
    LOG_ENTER();

    int rc;
    struct v4l2_encoder_cmd stEnc;

    VPP_RET_IF_NULL(pstCb, VPP_ERR_PARAM);

    stEnc.cmd = V4L2_QCOM_CMD_FLUSH;
    // TME requires FLUSH on both ports
    stEnc.flags = V4L2_QCOM_CMD_FLUSH_OUTPUT | V4L2_QCOM_CMD_FLUSH_CAPTURE;

    rc = IOCTL(pstCb->MeFd, VIDIOC_ENCODER_CMD, &stEnc);
    if (rc)
    {
        LOGE("Flush TME Failed, ret=%d", rc);
        LOG_EXIT_RET(VPP_ERR);
    }

    LOG_EXIT_RET(VPP_OK);
}

uint32_t u32VppIpFrcMe_TmePollEvents(struct pollfd *pstFdesc)
{
    LOG_ENTER();

    int rc;
    uint32_t u32Ret = VPP_OK;

    VPP_RET_IF_NULL(pstFdesc, VPP_ERR_PARAM);

    rc = poll(pstFdesc, FRC_ME_POLL_NUM_FDS, FRC_TME_POLL_TIMEOUT);
    if (rc < 0)
    {
        LOGE("ERROR returned from driver %d", rc);
        u32Ret = VPP_ERR_RESOURCES;
    }
    else if (rc == 0)
    {
        LOGE("---> ME polling timedout !");
        u32Ret = VPP_PENDING;
    }

    LOG_EXIT_RET(u32Ret);
}

uint32_t u32VppIpFrcMe_TmeGetEvents(t_StVppIpFrcMeCb *pstCb, struct v4l2_event *pstEvent)
{
    LOG_ENTER();

    int rc;
    uint32_t u32Ret = VPP_OK;

    VPP_RET_IF_NULL(pstCb, VPP_ERR_PARAM);
    VPP_RET_IF_NULL(pstEvent, VPP_ERR_PARAM);

    rc = IOCTL(pstCb->MeFd, VIDIOC_DQEVENT, pstEvent);
    if (rc)
    {
        LOGE("VIDIOC_DQEVENT Failed rc=%d", rc);
        u32Ret = VPP_ERR;
    }

    LOG_EXIT_RET(u32Ret);
}

uint32_t u32VppIpFrcMe_TmeGetBuf(t_StVppIpFrcMeCb *pstCb, struct v4l2_buffer *pstV4l2buf)
{
    LOG_ENTER();

    int rc;
    uint32_t u32Ret = VPP_OK;

    VPP_RET_IF_NULL(pstCb, VPP_ERR_PARAM);
    VPP_RET_IF_NULL(pstV4l2buf, VPP_ERR_PARAM);

    rc = IOCTL(pstCb->MeFd, VIDIOC_DQBUF, pstV4l2buf);
    if (rc)
    {
        //LOGE("VIDIOC_DQBUF Failed rc=%d", rc);
        u32Ret = VPP_ERR;
    }

    LOG_EXIT_RET(u32Ret);
}