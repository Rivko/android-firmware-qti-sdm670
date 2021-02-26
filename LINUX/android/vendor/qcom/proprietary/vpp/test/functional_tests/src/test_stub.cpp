/*!
 * @file test_stub.cpp
 *
 * @cr
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.

 * @services
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <sys/types.h>

#include "dvpTest.h"
#include "dvpTest_tb.h"
#include "test_utils.h"
#include "buf_pool.h"
#include "vpp_utils.h"
#include "vpp_buf.h"

#include "vpp_dbg.h"
#include "test_stub.h"

#ifdef VPP_SERVICE

/************************************************************************
 * Local definitions
 ***********************************************************************/

/************************************************************************
 * Local static variables
 ***********************************************************************/

/************************************************************************
 * Forward Declarations
 ************************************************************************/
extern struct test_ctx *pstTestCtx;

/************************************************************************
 * Local Functions
 ***********************************************************************/

/************************************************************************
 * Global Functions
 ***********************************************************************/
// Initialize the VPP library.
void* vppCl_VppInit(uint32_t flags, struct vpp_callbacks cb)
{
    VppClient *ctx = new VppClient();

    VPP_RET_IF_NULL(ctx, NULL);

    return (ctx->init(flags, cb));
}

// Terminates the VPP library.
void vppCl_VppTerm(void *ctx)
{
    VPP_RET_VOID_IF_NULL(ctx);

    static_cast<VppClient *>(ctx)->term();
    delete (static_cast<VppClient *>(ctx));
}

// Configures VPP control parameters.
uint32_t vppCl_VppSetCtrl(void *ctx, struct hqv_control ctrl, struct vpp_requirements *req)
{
    VPP_RET_IF_NULL(ctx, VPP_ERR_PARAM);

    return (static_cast<VppClient *>(ctx)->setCtrl(ctrl, req));
}

// Set the parameters for a given port.
uint32_t vppCl_VppSetParam(void *ctx, enum vpp_port ePort, struct vpp_port_param param)
{
    VPP_RET_IF_NULL(ctx, VPP_ERR_PARAM);

    return (static_cast<VppClient *>(ctx)->setParameter(ePort, param));
}

// Enqueue a buffer to the specified port.
uint32_t vppCl_VppQueueBuf(void *ctx, enum vpp_port ePort, struct vpp_buffer *buf)
{
    VPP_RET_IF_NULL(ctx, VPP_ERR_PARAM);

    return (static_cast<VppClient *>(ctx)->queueBuf(ePort, buf));
}

// Request for a port reconfiguration.
uint32_t vppCl_VppReconfig(void *ctx, struct vpp_port_param input_param,
                             struct vpp_port_param output_param)
{
    VPP_RET_IF_NULL(ctx, VPP_ERR_PARAM);

    return (static_cast<VppClient *>(ctx)->reconfigure(input_param, output_param));
}

// Request that a port is flushed.
uint32_t vppCl_VppFlush(void *ctx, enum vpp_port port)
{
    VPP_RET_IF_NULL(ctx, VPP_ERR_PARAM);

    return (static_cast<VppClient *>(ctx)->flush(port));
}

// Sets VPP video properties.
uint32_t vppCl_VppSetVidProp(void *ctx, struct video_property prop)
{
    VPP_RET_IF_NULL(ctx, VPP_ERR_PARAM);

    return (static_cast<VppClient *>(ctx)->setVidProp(prop));
}

// Get use-case params from the global context
uint32_t get_uc_algo_enabled(void * p, uint32_t u32Algo)
{
    t_StVppUsecase *pstUc;
    uint32_t i;

    VPP_UNUSED(p);
    if (pstTestCtx->hqvCtrl.mode == HQV_MODE_MANUAL)
    {
        if (pstTestCtx->u32hqvCtrlSetFlag & (1 << u32Algo))
            return VPP_TRUE;
    }
    else if (pstTestCtx->hqvCtrl.mode == HQV_MODE_AUTO)
    {
        pstUc = pstVppUsecase_GetAuto();
        if (pstUc == NULL)
            return VPP_FALSE;

        for (i = 0; i < pstUc->algos.u32CtrlCnt; i++)
        {
            if(pstUc->algos.ctrls[i] == u32Algo)
                return VPP_TRUE;
        }
    }
    return VPP_FALSE;
}

#else // _VPP_SERVICE_

// Get use-case params from the registry
uint32_t get_uc_algo_enabled(void * p, uint32_t u32Algo)
{
    t_StVppCb *pstCb;
    t_StVppUsecase *pstUc;
    uint32_t i;

    pstCb = ((t_StVppCtx *)p)->pstVppCb;

    pstUc = pstCb->pstUc;

    if (pstUc == NULL)
        return VPP_FALSE;

    for (i = 0; i < pstUc->algos.u32CtrlCnt; i++)
    {
        if(pstUc->algos.ctrls[i] == u32Algo)
            return VPP_TRUE;
    }

    return VPP_FALSE;
}

#endif // _VPP_SERVICE_

uint32_t setTestVppHqvCtrl(void *pVppCtx, struct test_ctx *pTestCtx, struct vpp_requirements *pReq)
{
    struct hqv_control ctrl;
    uint32_t u32 = VPP_OK;

    memset(&ctrl, 0, sizeof(ctrl));

    if (pTestCtx->u32hqvCtrlSetFlag & (1 << HQV_CONTROL_CADE))
    {
        ctrl.mode = pTestCtx->hqvCtrl.mode;
        ctrl.ctrl_type = HQV_CONTROL_CADE;
        ctrl.cade.mode = pTestCtx->hqvCtrl.cade.mode;
        if(ctrl.cade.mode == HQV_MODE_MANUAL)
        {
            ctrl.cade.cade_level = pTestCtx->hqvCtrl.cade.cade_level;
            ctrl.cade.contrast = pTestCtx->hqvCtrl.cade.contrast;
            ctrl.cade.saturation = pTestCtx->hqvCtrl.cade.saturation;
        }
        u32 |= VPP_SET_CTRL(pVppCtx, ctrl, pReq);
    }
    if (pTestCtx->u32hqvCtrlSetFlag & (1 << HQV_CONTROL_DI))
    {
        ctrl.mode = pTestCtx->hqvCtrl.mode;
        ctrl.ctrl_type = HQV_CONTROL_DI;
        ctrl.di.mode = pTestCtx->hqvCtrl.di.mode;

        u32 |= VPP_SET_CTRL(pVppCtx, ctrl, pReq);
    }
    if (pTestCtx->u32hqvCtrlSetFlag & (1 << HQV_CONTROL_TNR))
    {
        ctrl.mode = pTestCtx->hqvCtrl.mode;
        ctrl.ctrl_type = HQV_CONTROL_TNR;
        ctrl.tnr.mode = pTestCtx->hqvCtrl.tnr.mode;
        if(ctrl.tnr.mode == HQV_MODE_MANUAL)
            ctrl.tnr.level = pTestCtx->hqvCtrl.tnr.level;

        u32 |= VPP_SET_CTRL(pVppCtx, ctrl, pReq);
    }
    if (pTestCtx->u32hqvCtrlSetFlag & (1 << HQV_CONTROL_CNR))
    {
        ctrl.mode = pTestCtx->hqvCtrl.mode;
        ctrl.ctrl_type = HQV_CONTROL_CNR;
        ctrl.cnr.mode = pTestCtx->hqvCtrl.cnr.mode;
        if(ctrl.cnr.mode == HQV_MODE_MANUAL)
            ctrl.cnr.level = pTestCtx->hqvCtrl.cnr.level;

        u32 |= VPP_SET_CTRL(pVppCtx, ctrl, pReq);
    }
    if (pTestCtx->u32hqvCtrlSetFlag & (1 << HQV_CONTROL_AIE))
    {
        ctrl.mode = pTestCtx->hqvCtrl.mode;
        ctrl.ctrl_type = HQV_CONTROL_AIE;
        ctrl.aie.mode = pTestCtx->hqvCtrl.aie.mode;
        ctrl.aie.hue_mode = pTestCtx->hqvCtrl.aie.hue_mode;
        if(ctrl.aie.mode == HQV_MODE_MANUAL)
        {
            ctrl.aie.cade_level = pTestCtx->hqvCtrl.aie.cade_level;
            ctrl.aie.ltm_level = pTestCtx->hqvCtrl.aie.ltm_level;
            ctrl.aie.ltm_sat_gain = pTestCtx->hqvCtrl.aie.ltm_sat_gain;
            ctrl.aie.ltm_sat_offset = pTestCtx->hqvCtrl.aie.ltm_sat_offset;
            ctrl.aie.ltm_ace_str = pTestCtx->hqvCtrl.aie.ltm_ace_str;
            ctrl.aie.ltm_ace_bright_l = pTestCtx->hqvCtrl.aie.ltm_ace_bright_l;
            ctrl.aie.ltm_ace_bright_h = pTestCtx->hqvCtrl.aie.ltm_ace_bright_h;
        }
        u32 |= VPP_SET_CTRL(pVppCtx, ctrl, pReq);
    }
    if(pTestCtx->u32hqvCtrlSetFlag & (1 << HQV_CONTROL_EAR))
    {
        ctrl.mode = pTestCtx->hqvCtrl.mode;
        ctrl.ctrl_type = HQV_CONTROL_EAR;
        ctrl.ear.mode = pTestCtx->hqvCtrl.ear.mode;
        u32 |= VPP_SET_CTRL(pVppCtx, ctrl, pReq);
    }
    if(pTestCtx->u32hqvCtrlSetFlag & (1 << HQV_CONTROL_QBR))
    {
        ctrl.mode = pTestCtx->hqvCtrl.mode;
        ctrl.ctrl_type = HQV_CONTROL_QBR;
        ctrl.qbr.mode = pTestCtx->hqvCtrl.qbr.mode;
        u32 |= VPP_SET_CTRL(pVppCtx, ctrl, pReq);
    }
    if (pTestCtx->u32hqvCtrlSetFlag & (1 << HQV_CONTROL_FRC))
    {
        ctrl.mode = pTestCtx->hqvCtrl.mode;
        ctrl.ctrl_type = HQV_CONTROL_FRC;
        ctrl.frc.mode = pTestCtx->hqvCtrl.frc.mode;
        ctrl.frc.level = pTestCtx->hqvCtrl.frc.level;
        ctrl.frc.interp = pTestCtx->hqvCtrl.frc.interp;
        u32 |= VPP_SET_CTRL(pVppCtx, ctrl, pReq);
    }
    if (pTestCtx->u32hqvCtrlSetFlag & (1 << HQV_CONTROL_GLOBAL_DEMO))
    {
        ctrl.mode = pTestCtx->hqvCtrl.mode;
        ctrl.ctrl_type = HQV_CONTROL_GLOBAL_DEMO;
        ctrl.demo.process_percent = pTestCtx->hqvCtrl.demo.process_percent;
        ctrl.demo.process_direction = pTestCtx->hqvCtrl.demo.process_direction;

        u32 |= VPP_SET_CTRL(pVppCtx, ctrl, pReq);
    }

    return u32;
}
