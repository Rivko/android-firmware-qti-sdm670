/*!
 * @file HidlVppUtils.cpp
 *
 * @cr
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * @services
 */

#include <sys/types.h>
#include <sys/mman.h>
#include <inttypes.h>
#include <cutils/properties.h>
#include <stdlib.h>
#include <string.h>
#include <qcom/display/gr_priv_handle.h>

#include "qdMetaData.h"

#include "vpp_dbg.h"
#include "vpp.h"
#include "vpp_def.h"
#include <cutils/native_handle.h>
#include <vendor/qti/hardware/vpp/1.1/types.h>
#include "HidlVppUtils.h"

uint64_t u64LogLevelIpc;

/************************************************************************
 * Local definitions
 ***********************************************************************/
#define CLEAR_PRIV_HDL_BASE_PARAMS(pH)    {\
                                              ((private_handle_t *)pH)->base = 0;\
                                              ((private_handle_t *)pH)->base_metadata = 0;\
                                          }

namespace android {

/*********** From vpp to hidl *******************************************/
uint32_t hqvCtrlToHidl(const struct hqv_control& ctrl, HqvControl& stHqvCtrl)
{
    using vendor::qti::hardware::vpp::V1_1::HqvMode;
    using vendor::qti::hardware::vpp::V1_1::HqvControlType;
    using vendor::qti::hardware::vpp::V1_1::HqvHueMode;
    using vendor::qti::hardware::vpp::V1_1::HqvFrcMode;
    using vendor::qti::hardware::vpp::V1_1::HqvFrcLevel;
    using vendor::qti::hardware::vpp::V1_1::HqvFrcInterp;
    using vendor::qti::hardware::vpp::V1_1::HqvDiMode;
    using vendor::qti::hardware::vpp::V1_1::HqvEarMode;
    using vendor::qti::hardware::vpp::V1_1::HqvQbrMode;
    using vendor::qti::hardware::vpp::V1_1::HqvSplitDirection;

    uint32_t u32Ret = VPP_OK;

    stHqvCtrl.mode = (HqvMode)ctrl.mode;
    stHqvCtrl.ctrlType = (HqvControlType)ctrl.ctrl_type;

    switch (ctrl.ctrl_type)
    {
        case HQV_CONTROL_NONE:
            break;
        case HQV_CONTROL_CADE:
            stHqvCtrl.u.cade.mode = (HqvMode)ctrl.cade.mode;
            stHqvCtrl.u.cade.cadeLevel = ctrl.cade.cade_level;
            stHqvCtrl.u.cade.contrast = ctrl.cade.contrast;
            stHqvCtrl.u.cade.saturation = ctrl.cade.saturation;
            break;
        case HQV_CONTROL_AIE:
            stHqvCtrl.u.aie.mode = (HqvMode)ctrl.aie.mode;
            stHqvCtrl.u.aie.hueMode = (HqvHueMode)ctrl.aie.hue_mode;
            stHqvCtrl.u.aie.cadeLevel = ctrl.aie.cade_level;
            stHqvCtrl.u.aie.ltmLevel = ctrl.aie.ltm_level;
            stHqvCtrl.u.aie.ltmSatGain = ctrl.aie.ltm_sat_gain;
            stHqvCtrl.u.aie.ltmSatOffset = ctrl.aie.ltm_sat_offset;
            stHqvCtrl.u.aie.ltmAceStr = ctrl.aie.ltm_ace_str;
            stHqvCtrl.u.aie.ltmAceBriL = ctrl.aie.ltm_ace_bright_l;
            stHqvCtrl.u.aie.ltmAceBriH = ctrl.aie.ltm_ace_bright_h;
            break;
        case HQV_CONTROL_FRC:
            stHqvCtrl.u.frc.mode = (HqvFrcMode)ctrl.frc.mode;
            stHqvCtrl.u.frc.level = (HqvFrcLevel)ctrl.frc.level;
            stHqvCtrl.u.frc.interp = (HqvFrcInterp)ctrl.frc.interp;
            break;
        case HQV_CONTROL_DI:
            stHqvCtrl.u.di.mode = (HqvDiMode)ctrl.di.mode;
            break;
        case HQV_CONTROL_TNR:
            stHqvCtrl.u.tnr.mode = (HqvMode)ctrl.tnr.mode;
            stHqvCtrl.u.tnr.level = ctrl.tnr.level;
            break;
        case HQV_CONTROL_CNR:
            stHqvCtrl.u.cnr.mode = (HqvMode)ctrl.cnr.mode;
            stHqvCtrl.u.cnr.level = ctrl.cnr.level;
            break;
        case HQV_CONTROL_GLOBAL_DEMO:
            stHqvCtrl.u.demo.processPercent = ctrl.demo.process_percent;
            stHqvCtrl.u.demo.processDirection = (HqvSplitDirection)ctrl.demo.process_direction;
            break;
        case HQV_CONTROL_EAR:
            stHqvCtrl.u.ear.mode = (HqvEarMode)ctrl.ear.mode;
            break;
        case HQV_CONTROL_QBR:
            stHqvCtrl.u.qbr.mode = (HqvQbrMode)ctrl.qbr.mode;
            break;
        case HQV_CONTROL_MEAS:
            stHqvCtrl.u.meas.enable = ctrl.meas.enable;
            break;
        case HQV_CONTROL_CUST:
        case HQV_CONTROL_MAX:
        default:
            LOGE("Unsupported Control Type %u",(uint32_t)ctrl.ctrl_type);
            u32Ret = VPP_ERR_PARAM;
            break;
    };
    return u32Ret;
}

uint32_t vppReqToHidl(const struct vpp_requirements *pstReq, VppRequirements& stReq)
{
    if (!pstReq)
        return VPP_ERR_PARAM;

    stReq.inColorFmtMask = pstReq->in_color_fmt_mask;

    stReq.metadata.cnt = pstReq->metadata.cnt;

    for (size_t i = 0; i < META_MAX_CNT; ++i)
    {
        stReq.metadata.meta[i] = pstReq->metadata.meta[i];
    }

    for (size_t i = 0; i < VPP_RESOLUTION_MAX; ++i)
    {
        stReq.inFactor[i].add = pstReq->in_factor[i].add;
        stReq.inFactor[i].mul = pstReq->in_factor[i].mul;

        stReq.outFactor[i].add = pstReq->out_factor[i].add;
        stReq.outFactor[i].mul = pstReq->out_factor[i].mul;
    }
    return VPP_OK;
}

void vppPortParamToHidl(const struct vpp_port_param param, VppPortParam& stParam)
{
    using vendor::qti::hardware::vpp::V1_1::VppColorFormat;

    stParam.fmt = (VppColorFormat)param.fmt;
    stParam.height = param.height;
    stParam.width = param.width;
    stParam.stride = param.stride;
    stParam.scanlines = param.scanlines;
}

uint32_t vppPropToHidl(const struct video_property prop, VideoProperty& stProp)
{
    using vendor::qti::hardware::vpp::V1_1::VidPropType;
    using vendor::qti::hardware::vpp::V1_1::VppCodecType;

    uint32_t u32Ret = VPP_OK;

    stProp.propertyType = (VidPropType)prop.property_type;

    switch (prop.property_type)
    {
        case VID_PROP_CODEC:
            stProp.u.codec.eCodec = (VppCodecType)prop.codec.eCodec;
            break;
        case VID_PROP_NON_REALTIME:
            stProp.u.nonRealtime.bNonRealtime = (uint32_t)prop.non_realtime.bNonRealtime;
            break;
        case VID_PROP_OPERATING_RATE:
            stProp.u.operatingRate.u32OperatingRate = prop.operating_rate.u32OperatingRate;
            break;
        case VID_PROP_MAX:
        default:
            LOGE("Unsupported property_type %u", prop.property_type);
            u32Ret = VPP_ERR_PARAM;
            break;
    };
    return u32Ret;
}

static uint32_t vppMemBufToHidl(const struct vpp_mem_buffer memBuf, VppMemBuffer& stVppMemBuf,
                                bool isCallerSvc)
{
    native_handle_t *nativeHandle = nullptr;
    if (memBuf.fd >= 0)
    {
        // If the caller is service, close the duped Fd
        if (isCallerSvc == VPP_TRUE)
            close(memBuf.fd);
        else
        {
            nativeHandle = native_handle_create(1 /* numFds */, 0 /* numInts */);
            if (nativeHandle == nullptr)
            {
                LOGE("native_handle_create() return nullptr");
                return VPP_ERR;
            }
            nativeHandle->data[0] = dup(memBuf.fd);
        }
    }

    stVppMemBuf.handleFd.setTo(nativeHandle);
    stVppMemBuf.allocLen = memBuf.alloc_len;

    stVppMemBuf.filledLen = memBuf.filled_len;
    stVppMemBuf.offset = memBuf.offset;
    stVppMemBuf.validDataLen = memBuf.valid_data_len;

    return VPP_OK;
}

uint32_t vppBufferToHidl(const struct vpp_buffer *buf, VppBuffer& stVppBuf, bool isCallerSvc)
{
    uint32_t u32Ret;

    if (!buf)
        return VPP_ERR_PARAM;

    stVppBuf.flags = buf->flags;
    stVppBuf.timestamp = buf->timestamp;
    stVppBuf.bufferId = (uint64_t)buf->cookie;
    stVppBuf.cookieInToOut = (uint64_t)buf->cookie_in_to_out;

    if (buf->pvGralloc)
    {
        // This will force the service to mmap the gralloc Fds
        if (isCallerSvc == VPP_TRUE)
        {
            // Workaround for bug where copyMetaData maps the buffer
            // into the address space but doesn't unmap it. This
            // prevents a memory leak.
            private_handle_t *p = (private_handle_t *)buf->pvGralloc;
            if (p && p->base_metadata)
            {
                munmap(reinterpret_cast<void *>(p->base_metadata),
                       getMetaDataSize());
                p->base_metadata = (uintptr_t)nullptr;
            }
            releaseNativeHandle((const native_handle_t *)buf->pvGralloc);
            stVppBuf.pvGralloc = nullptr;
        }
        else
            stVppBuf.pvGralloc.setTo((native_handle_t *)buf->pvGralloc);
    }
    else
        stVppBuf.pvGralloc = nullptr;

    u32Ret = vppMemBufToHidl(buf->pixel, stVppBuf.pixel, isCallerSvc);
    if (u32Ret != VPP_OK)
        return VPP_ERR_PARAM;

    u32Ret = vppMemBufToHidl(buf->extradata, stVppBuf.extradata, isCallerSvc);
    if (u32Ret != VPP_OK)
        return VPP_ERR_PARAM;

    return VPP_OK;
}

uint32_t vppEventToHidl(const struct vpp_event e, VppEvent& stEvt)
{
    using vendor::qti::hardware::vpp::V1_1::VppEventType;

    uint32_t u32Ret = VPP_OK;

    stEvt.type = (VppEventType)e.type;

    switch (e.type)
    {
        case VPP_EVENT_FLUSH_DONE:
            stEvt.u.flushDone.port = (VppPort)e.flush_done.port;
            break;
        case VPP_EVENT_RECONFIG_DONE:
            stEvt.u.reconfigDone.reconfigStatus = e.port_reconfig_done.reconfig_status;
            vppReqToHidl(&e.port_reconfig_done.req, stEvt.u.reconfigDone.req);
            break;
        case VPP_EVENT_ERROR:
            break;
        default:
            LOGE("Unsupported Event type %u", e.type);
            u32Ret = VPP_ERR_PARAM;
            break;
    }
    return u32Ret;
}

/*********** From hidl to vpp *******************************************/
uint32_t hidlToHqvCtrl(struct hqv_control& ctrl, const HqvControl& stHqvCtrl)
{
    uint32_t u32Ret = VPP_OK;

    ctrl.mode = (hqv_mode)stHqvCtrl.mode;
    ctrl.ctrl_type = (hqv_control_type)stHqvCtrl.ctrlType;

    switch (ctrl.ctrl_type)
    {
        case HQV_CONTROL_NONE:
            break;
        case HQV_CONTROL_CADE:
            ctrl.cade.mode = (hqv_mode)stHqvCtrl.u.cade.mode;
            ctrl.cade.cade_level = stHqvCtrl.u.cade.cadeLevel;
            ctrl.cade.contrast = stHqvCtrl.u.cade.contrast;
            ctrl.cade.saturation = stHqvCtrl.u.cade.saturation;
            break;
        case HQV_CONTROL_AIE:
            ctrl.aie.mode = (hqv_mode)stHqvCtrl.u.aie.mode;
            ctrl.aie.hue_mode = (hqv_hue_mode)stHqvCtrl.u.aie.hueMode;
            ctrl.aie.cade_level = stHqvCtrl.u.aie.cadeLevel;
            ctrl.aie.ltm_level = stHqvCtrl.u.aie.ltmLevel;
            ctrl.aie.ltm_sat_gain = stHqvCtrl.u.aie.ltmSatGain;
            ctrl.aie.ltm_sat_offset = stHqvCtrl.u.aie.ltmSatOffset;
            ctrl.aie.ltm_ace_str = stHqvCtrl.u.aie.ltmAceStr;
            ctrl.aie.ltm_ace_bright_l = stHqvCtrl.u.aie.ltmAceBriL;
            ctrl.aie.ltm_ace_bright_h = stHqvCtrl.u.aie.ltmAceBriH;
            break;
        case HQV_CONTROL_FRC:
            ctrl.frc.mode = (hqv_frc_mode)stHqvCtrl.u.frc.mode;
            ctrl.frc.level = (hqv_frc_level)stHqvCtrl.u.frc.level;
            ctrl.frc.interp = (hqv_frc_interp)stHqvCtrl.u.frc.interp;
            break;
        case HQV_CONTROL_DI:
            ctrl.di.mode = (hqv_di_mode)stHqvCtrl.u.di.mode;
            break;
        case HQV_CONTROL_TNR:
            ctrl.tnr.mode = (hqv_mode)stHqvCtrl.u.tnr.mode;
            ctrl.tnr.level = stHqvCtrl.u.tnr.level;
            break;
        case HQV_CONTROL_CNR:
            ctrl.cnr.mode = (hqv_mode)stHqvCtrl.u.cnr.mode;
            ctrl.cnr.level = stHqvCtrl.u.cnr.level;
            break;
        case HQV_CONTROL_GLOBAL_DEMO:
            ctrl.demo.process_percent = stHqvCtrl.u.demo.processPercent;
            ctrl.demo.process_direction = (hqv_split_direction)stHqvCtrl.u.demo.processDirection;
            break;
        case HQV_CONTROL_EAR:
            ctrl.ear.mode = (hqv_ear_mode)stHqvCtrl.u.ear.mode;
            break;
        case HQV_CONTROL_QBR:
            ctrl.qbr.mode = (hqv_qbr_mode)stHqvCtrl.u.qbr.mode;
            break;
        case HQV_CONTROL_MEAS:
            ctrl.meas.enable = stHqvCtrl.u.meas.enable;
            break;
        case HQV_CONTROL_CUST:
        case HQV_CONTROL_MAX:
        default:
            LOGE("Unsupported Control Type %u", (uint32_t)ctrl.ctrl_type);
            u32Ret = VPP_ERR_PARAM;
            break;
    }
    return u32Ret;
}

uint32_t hidlToVppReq(struct vpp_requirements *pstReq, const VppRequirements& stReq)
{
    if (!pstReq)
        return VPP_ERR_PARAM;

    pstReq->in_color_fmt_mask = stReq.inColorFmtMask;

    pstReq->metadata.cnt = stReq.metadata.cnt;

    for (size_t i = 0; i < META_MAX_CNT; ++i)
    {
        pstReq->metadata.meta[i] = stReq.metadata.meta[i];
    }

    for (size_t i = 0; i < VPP_RESOLUTION_MAX; ++i)
    {
        pstReq->in_factor[i].add = stReq.inFactor[i].add;
        pstReq->in_factor[i].mul = stReq.inFactor[i].mul;

        pstReq->out_factor[i].add = stReq.outFactor[i].add;
        pstReq->out_factor[i].mul = stReq.outFactor[i].mul;
    }
    return VPP_OK;
}

void hidlToVppPortParam(struct vpp_port_param& param, const VppPortParam& stParam)
{
    param.fmt = (vpp_color_format)stParam.fmt;
    param.height = stParam.height;
    param.width = stParam.width;
    param.stride = stParam.stride;
    param.scanlines = stParam.scanlines;
}

uint32_t hidlToVppProp(struct video_property& prop, const VideoProperty& stProp)
{
    uint32_t u32Ret = VPP_OK;

    prop.property_type = (vid_prop_type)stProp.propertyType;

    switch (prop.property_type)
    {
        case VID_PROP_CODEC:
            prop.codec.eCodec = (vpp_codec_type)stProp.u.codec.eCodec;
            break;
        case VID_PROP_NON_REALTIME:
            prop.non_realtime.bNonRealtime = stProp.u.nonRealtime.bNonRealtime;
            break;
        case VID_PROP_OPERATING_RATE:
            prop.operating_rate.u32OperatingRate = stProp.u.operatingRate.u32OperatingRate;
            break;
        case VID_PROP_MAX:
        default:
            LOGE("Unsupported propterty_type %u", prop.property_type);
            u32Ret = VPP_ERR_PARAM;
            break;
    };
    return u32Ret;
}

static void hidlToVppMemBuf(struct vpp_mem_buffer& memBuf, const VppMemBuffer& stVppMemBuf)
{
    auto nh = stVppMemBuf.handleFd.getNativeHandle();
    if (nh == nullptr)
        memBuf.fd = -1;
    else
        memBuf.fd = dup(nh->data[0]);

    memBuf.alloc_len = stVppMemBuf.allocLen;
    memBuf.filled_len = stVppMemBuf.filledLen;
    memBuf.offset = stVppMemBuf.offset;
    memBuf.pvMapped = nullptr;
    memBuf.valid_data_len = stVppMemBuf.validDataLen;
}

uint32_t hidlToVppBuffer(struct vpp_buffer *buf, const VppBuffer& stVppBuf)
{
    if (!buf)
        return VPP_ERR_PARAM;

    buf->flags = stVppBuf.flags;
    buf->timestamp = stVppBuf.timestamp;
    buf->cookie = reinterpret_cast<void *>(stVppBuf.bufferId);
    buf->cookie_in_to_out = (void *)stVppBuf.cookieInToOut;
    if (stVppBuf.pvGralloc)
    {
        buf->pvGralloc = (void *)native_handle_clone(stVppBuf.pvGralloc.getNativeHandle());
        // Set base/base_metadata to 0.
        // This will force the service to mmap the gralloc Fds
        if (buf->pvGralloc)
        {
            CLEAR_PRIV_HDL_BASE_PARAMS(buf->pvGralloc);
        }
    }
    else
        buf->pvGralloc = nullptr;

    hidlToVppMemBuf(buf->pixel, stVppBuf.pixel);
    hidlToVppMemBuf(buf->extradata, stVppBuf.extradata);

    return VPP_OK;
}

uint32_t hidlToVppEvent(struct vpp_event& e, const VppEvent& stEvt)
{
    uint32_t u32Ret = VPP_OK;

    e.type = (vpp_event_type)stEvt.type;

    switch (e.type)
    {
        case VPP_EVENT_FLUSH_DONE:
            e.flush_done.port = (vpp_port)stEvt.u.flushDone.port;
            break;
        case VPP_EVENT_RECONFIG_DONE:
            e.port_reconfig_done.reconfig_status = stEvt.u.reconfigDone.reconfigStatus;
            hidlToVppReq(&e.port_reconfig_done.req, stEvt.u.reconfigDone.req);
            break;
        case VPP_EVENT_ERROR:
            break;
        default:
            LOGE("Unsupported Event type %u", e.type);
            u32Ret = VPP_ERR_PARAM;
            break;
    }
    return u32Ret;
}

void releaseNativeHandle(const native_handle_t *nh)
{
    if (nh != nullptr)
    {
        uint32_t u32 = native_handle_close(nh);
        if (u32 != 0)
            LOGE("ERROR : native_handle_close returned err = %u",u32);
        else
        {
            u32 = native_handle_delete(const_cast<native_handle_t*>(nh));
            if (u32 != 0)
                LOGE("ERROR : native_handle_delete returned err = %u",u32);
        }
    }
}

void readProperties(uint32_t *pu32StatsEn)
{
    char property_value[256] = {0};

    property_get(VPP_PROPERTY_LOG_CORE, property_value, "0");
    u64LogLevelIpc = strtoull(property_value, NULL, 0);

    property_get(VPP_PROPERTY_STATS_LEVEL, property_value, "0");
    *pu32StatsEn = strtoul(property_value, NULL, 0);

    // Log after setting global (in case defined log level disables this log)
    LOGD("%s=0x%llx", VPP_PROPERTY_LOG_CORE, (long long unsigned int)u64LogLevelIpc);
}

}
