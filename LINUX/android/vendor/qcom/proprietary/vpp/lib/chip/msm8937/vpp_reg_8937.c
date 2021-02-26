/*!
 * @file vpp_reg_8937.c
 *
 * @cr
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * @services
 */

#include <sys/types.h>

#include "vpp.h"

#include "vpp_reg.h"

#include "vpp_ip_gpu.h"
#include <media/msm_vidc.h>

/************************************************************************
 * GPU Definition
 ***********************************************************************/
static t_StVppAlgo algo_gpu[] = {
    {
        .ctrl = HQV_CONTROL_CADE,
    },
    {
        .ctrl = HQV_CONTROL_GLOBAL_DEMO,
    },
};

t_EVppColorFmt color_fmt_gpu[] = {
    VPP_COLOR_FORMAT_NV12_VENUS,
};

static t_StVppPortCfg port_cfg_gpu_common[] = {
    {
        VPP_UC_INTERNAL_REQ(1)
        {
            VPP_UC_PORT(VPP_COLOR_FORMAT_NV12_VENUS, VPP_COLOR_FORMAT_NV12_VENUS, \
                        0, eVppBufPxDataType_Raw)
        },
    },
};

VPP_IP_DECL(GPU,
            VPP_IP_GPU,
            algo_gpu,
            1000,
            0,
            0,
            0,
            2,
            VPP_FALSE,
            VPP_FALSE,
            RES_HD_MAX_W,
            RES_HD_MAX_H,
            96,
            64,
            color_fmt_gpu,
            color_fmt_gpu,
            NULL,
            NULL,
            vpVppIpGpu_Init,
            vVppIpGpu_Term,
            u32VppIpGpu_Open,
            u32VppIpGpu_Close,
            u32VppIpGpu_SetParam,
            u32VppIpGpu_SetCtrl,
            u32VppIpGpu_GetBufferRequirements,
            u32VppIpGpu_QueueBuf,
            u32VppIpGpu_Flush,
            u32VppIpGpu_Drain,
            u32VppIpGpu_Reconfigure);

/************************************************************************
 * Usecase Definition
 ***********************************************************************/
static t_StVppUsecase uc_cade = {
    .name = "CADE",
    .algos = {
        .ctrls = (enum hqv_control_type[]) {
            HQV_CONTROL_CADE,
        },
        .u32CtrlCnt = 1,
    },
    .composition = (t_StVppUsecaseIp[]) {
        {
            VPP_UC_COMP(GPU, 1, VPP_FALSE)
        },
    },
    .u32IpCnt = 1,
    .credits = {
        VPP_UC_CREDITS(VPP_IP_GPU, GPU, 30, 30, NOT_SUPPORTED, NOT_SUPPORTED)
    },
    VPP_UC_PORT_CFG(port_cfg_gpu_common)
    VPP_UC_FACTOR(0, 1, 0, 1)
    VPP_UC_DEFAULT_FPS(30)
    VPP_UC_FALLBACK(NULL, NULL, NULL, NULL)
};

static t_StVppUsecase uc_auto = {
    .name = "AUTO",
    .algos = {
        .ctrls = (enum hqv_control_type[]) {
            HQV_CONTROL_CADE,
        },
        .u32CtrlCnt = 1,
    },
    .composition = (t_StVppUsecaseIp[]) {
        {
            VPP_UC_COMP(GPU, 1, VPP_FALSE)
        },
    },
    .u32IpCnt = 1,
    .credits = {
        VPP_UC_CREDITS(VPP_IP_GPU, GPU, 30, 30, NOT_SUPPORTED, NOT_SUPPORTED)
    },
    VPP_UC_PORT_CFG(port_cfg_gpu_common)
    VPP_UC_FACTOR(0, 1, 0, 1)
    VPP_UC_DEFAULT_FPS(30)
    VPP_UC_FALLBACK(NULL, NULL, NULL, NULL)
};

static t_StVppUsecase * apstUc8937[] = {
    &uc_cade,
};

t_StVppUsecase ** ppstVppUsecase_GetRegistry(uint32_t *o_pu32Cnt)
{
    *o_pu32Cnt = sizeof(apstUc8937) / sizeof(t_StVppUsecase *);
    return apstUc8937;
}

t_StVppUsecase * pstVppUsecase_GetAuto()
{
    return &uc_auto;
}

