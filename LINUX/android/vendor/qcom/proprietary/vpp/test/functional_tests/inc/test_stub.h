/*!
 * @file test_stub.h
 *
 * @cr
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.

 * @services
 */

#ifndef _TEST_STUB_H_
#define _TEST_STUB_H_

#ifdef VPP_SERVICE
#include "VppClient.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

using namespace android;

#ifndef VPP_SERVICE

#define VPP_INIT(flags, cb)                          vpp_init(flags, cb)
#define VPP_TERM(ctx)                                vpp_term(ctx)
#define VPP_SET_CTRL(ctx, ctrl, req)                 vpp_set_ctrl(ctx, ctrl, req)
#define VPP_SET_PARAM(ctx, port, param)              vpp_set_parameter(ctx, port, param)
#define VPP_QUEUE_BUF(ctx, ePort, buf)               vpp_queue_buf(ctx, ePort, buf)
#define VPP_RECONFIG(ctx, input_param, output_param) vpp_reconfigure(ctx, input_param, output_param)
#define VPP_FLUSH(ctx, port)                         vpp_flush(ctx, port)
#define VPP_SET_VID_PROP(ctx, prop)                  vpp_set_vid_prop(ctx, prop)

#else //VPP_SERVICE

#define VPP_INIT(flags, cb)                          vppCl_VppInit(flags, cb)
#define VPP_TERM(ctx)                                vppCl_VppTerm(ctx)
#define VPP_SET_CTRL(ctx, ctrl, req)                 vppCl_VppSetCtrl(ctx, ctrl, req)
#define VPP_SET_PARAM(ctx, port, param)              vppCl_VppSetParam(ctx, port, param)
#define VPP_QUEUE_BUF(ctx, ePort, buf)               vppCl_VppQueueBuf(ctx, ePort, buf)
#define VPP_RECONFIG(ctx, input_param, output_param) vppCl_VppReconfig(ctx, input_param, output_param)
#define VPP_FLUSH(ctx, port)                         vppCl_VppFlush(ctx, port)
#define VPP_SET_VID_PROP(ctx, prop)                  vppCl_VppSetVidProp(ctx, prop)

// Initialize the VPP library.
void* vppCl_VppInit(uint32_t flags, struct vpp_callbacks cb);
// Terminates the VPP library.
void vppCl_VppTerm(void *ctx);
// Configures VPP control parameters.
uint32_t vppCl_VppSetCtrl(void *ctx, struct hqv_control ctrl, struct vpp_requirements *req);
// Set the parameters for a given port.
uint32_t vppCl_VppSetParam(void *ctx, enum vpp_port ePort, struct vpp_port_param param);
// Enqueue a buffer to the specified port.
uint32_t vppCl_VppQueueBuf(void *ctx, enum vpp_port ePort, struct vpp_buffer *buf);
// Request for a port reconfiguration.
uint32_t vppCl_VppReconfig(void *ctx, struct vpp_port_param input_param,
                           struct vpp_port_param output_param);
//Request that a port is flushed.
uint32_t vppCl_VppFlush(void *ctx, enum vpp_port port);
//Sets VPP video properties.
uint32_t vppCl_VppSetVidProp(void *ctx, struct video_property prop);

#endif //VPP_SERVICE

uint32_t setTestVppHqvCtrl(void *pVppCtx, struct test_ctx *pTestCtx,
                           struct vpp_requirements *pReq);
uint32_t get_uc_algo_enabled(void *p, uint32_t u32Algo);

#ifdef __cplusplus
}
#endif

#endif // _TEST_STUB_H_
