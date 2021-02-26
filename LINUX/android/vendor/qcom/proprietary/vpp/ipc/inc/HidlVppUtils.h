/*!
 * @file HidlVppUtils.h
 *
 * @cr
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * @services
 */

#ifndef _VPP_HIDL_UTILS_H_
#define _VPP_HIDL_UTILS_H_

namespace android {

using ::vendor::qti::hardware::vpp::V1_1::HqvControl;
using ::vendor::qti::hardware::vpp::V1_1::VideoProperty;
using ::vendor::qti::hardware::vpp::V1_1::VppBuffer;
using ::vendor::qti::hardware::vpp::V1_1::VppMemBuffer;
using ::vendor::qti::hardware::vpp::V1_1::VppPort;
using ::vendor::qti::hardware::vpp::V1_1::VppPortParam;
using ::vendor::qti::hardware::vpp::V1_1::VppRequirements;
using ::vendor::qti::hardware::vpp::V1_1::VppEvent;

uint32_t hqvCtrlToHidl(const struct hqv_control& ctrl, HqvControl& stHqvCtrl);
uint32_t vppReqToHidl(const struct vpp_requirements *pstReq, VppRequirements& stReq);
void vppPortParamToHidl(const struct vpp_port_param param, VppPortParam& stParam);
uint32_t vppPropToHidl(const struct video_property prop, VideoProperty& stProp);
uint32_t vppBufferToHidl(const struct vpp_buffer *buf, VppBuffer& stVppBuf, bool isCallerSvc);
uint32_t vppEventToHidl(const struct vpp_event e, VppEvent& stEvt);

uint32_t hidlToHqvCtrl(struct hqv_control& ctrl, const HqvControl& stHqvCtrl);
uint32_t hidlToVppReq(struct vpp_requirements *pstReq, const VppRequirements& stReq);
void hidlToVppPortParam(struct vpp_port_param& param, const VppPortParam& stParam);
uint32_t hidlToVppProp(struct video_property& prop, const VideoProperty& stProp);
uint32_t hidlToVppBuffer(struct vpp_buffer *buf, const VppBuffer& stVppBuf);
uint32_t hidlToVppEvent(struct vpp_event& e, const VppEvent& stEvt);

void releaseNativeHandle(const native_handle_t *nh);
void readProperties(uint32_t *pu32StatsEn);

};
#endif // _VPP_HIDL_UTILS_H_
