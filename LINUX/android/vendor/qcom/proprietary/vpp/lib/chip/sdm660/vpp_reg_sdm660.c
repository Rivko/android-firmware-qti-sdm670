/*!
 * @file vpp_reg_sdm660.c
 *
 * @cr
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.

 * @services
 */

#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <media/msm_vidc.h>

#include "vpp.h"
#include "vpp_reg.h"
#include "vpp_ip_hvx.h"
#include "vpp_ip_gpu.h"
#include "vpp_utils.h"

/************************************************************************
 * Common Definition
 ***********************************************************************/
t_EVppColorFmt color_fmt_common[] = {
    VPP_COLOR_FORMAT_NV12_VENUS,
};

static t_StVppPortCfg port_cfg_common[] = {
    {
        VPP_UC_INTERNAL_REQ(1)
        {
            VPP_UC_PORT(VPP_COLOR_FORMAT_NV12_VENUS, VPP_COLOR_FORMAT_NV12_VENUS, \
                        0, eVppBufPxDataType_Raw)
        },
    },
};

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

VPP_IP_DECL(GPU,
            VPP_IP_GPU,
            algo_gpu,
            1000,
            0,
            0,
            0,
            2,
            VPP_TRUE,
            VPP_FALSE,
            RES_FHD_MAX_W,
            RES_FHD_MAX_H,
            96,
            64,
            color_fmt_common,
            color_fmt_common,
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
 * HVX Definition
 ***********************************************************************/
static t_StVppAlgo algo_hvx[] = {
    {
        .ctrl = HQV_CONTROL_DI,
    },
    {
        .ctrl = HQV_CONTROL_CNR,
    },
    {
        .ctrl = HQV_CONTROL_AIE,
    },
    {
        .ctrl = HQV_CONTROL_GLOBAL_DEMO,
    },
};

VPP_IP_DECL(HVX,
            VPP_IP_HVX,
            algo_hvx,
            1000,
            VPP_QUIRK_NO_CONCURRENT_CP_NCP_PID,
            0,
            0,
            2,
            VPP_FALSE,
            VPP_FALSE,
            RES_UHD_MAX_W,
            RES_UHD_MAX_H,
            128,
            96,
            color_fmt_common,
            color_fmt_common,
            NULL,
            NULL,
            vpVppIpHvx_Init,
            vVppIpHvx_Term,
            u32VppIpHvx_Open,
            u32VppIpHvx_Close,
            u32VppIpHvx_SetParam,
            u32VppIpHvx_SetCtrl,
            u32VppIpHvx_GetBufferRequirements,
            u32VppIpHvx_QueueBuf,
            u32VppIpHvx_Flush,
            u32VppIpHvx_Drain,
            u32VppIpHvx_Reconfigure);

/************************************************************************
 * Usecase Definition
 ***********************************************************************/
static t_StVppUsecase uc_aie = {
    .name = "AIE",
    .algos = {
        .ctrls = (enum hqv_control_type[]) {
            HQV_CONTROL_AIE,
        },
        .u32CtrlCnt = 1,
    },
    .composition = (t_StVppUsecaseIp[]) {
        {
            VPP_UC_COMP(HVX, 2, VPP_FALSE)
        },
    },
    .u32IpCnt = 1,
    .credits = {
        VPP_UC_CREDITS(VPP_IP_HVX, HVX, 15, 15, 15, 30)
    },
    VPP_UC_PORT_CFG(port_cfg_common)
    VPP_UC_FACTOR(0, 1, 0, 1)
    VPP_UC_DEFAULT_FPS(30)
    VPP_UC_FALLBACK(NULL, NULL, NULL, NULL)
};

static t_StVppUsecase uc_di = {
    .name = "DI",
    .algos = {
        .ctrls = (enum hqv_control_type[]) {
            HQV_CONTROL_DI,
        },
        .u32CtrlCnt = 1,
    },
    .composition = (t_StVppUsecaseIp[]) {
        {
            VPP_UC_COMP(HVX, 2, VPP_FALSE)
        },
    },
    .u32IpCnt = 1,
    .credits = {
        VPP_UC_CREDITS(VPP_IP_HVX, HVX, 15, 15, 15, NOT_SUPPORTED)
    },
    VPP_UC_PORT_CFG(port_cfg_common)
    VPP_UC_FACTOR(2, 1, 0, 2)
    VPP_UC_DEFAULT_FPS(30)
    VPP_UC_MAX_IN_FPS(I, 33, 33, 33, 0)
    VPP_UC_FALLBACK(NULL, NULL, NULL, NULL)
};

static t_StVppUsecase uc_di_cnr = {
    .name = "DI+CNR",
    .algos = {
        .ctrls = (enum hqv_control_type[]) {
            HQV_CONTROL_DI,
            HQV_CONTROL_CNR,
        },
        .u32CtrlCnt = 2,
    },
    .composition = (t_StVppUsecaseIp[]) {
        {
            VPP_UC_COMP(HVX, 2, VPP_FALSE)
        },
    },
    .u32IpCnt = 1,
    .credits = {
        VPP_UC_CREDITS(VPP_IP_HVX, HVX, 15, 30, 30, NOT_SUPPORTED)
    },
    VPP_UC_PORT_CFG(port_cfg_common)
    VPP_UC_FACTOR(2, 1, 0, 2)
    VPP_UC_DEFAULT_FPS(30)
    VPP_UC_MAX_IN_FPS(I, 33, 33, 33, 0)
    VPP_UC_FALLBACK(NULL, NULL, NULL, NULL)
};

static t_StVppUsecase uc_di_aie = {
    .name = "DI+AIE",
    .algos = {
        .ctrls = (enum hqv_control_type[]) {
            HQV_CONTROL_DI,
            HQV_CONTROL_AIE,
        },
        .u32CtrlCnt = 2,
    },
    .composition = (t_StVppUsecaseIp[]) {
        {
            VPP_UC_COMP(HVX, 2, VPP_FALSE)
        },
    },
    .u32IpCnt = 1,
    .credits = {
        VPP_UC_CREDITS(VPP_IP_HVX, HVX, 15, 15, 15, NOT_SUPPORTED)
    },
    VPP_UC_PORT_CFG(port_cfg_common)
    VPP_UC_FACTOR(2, 1, 0, 2)
    VPP_UC_DEFAULT_FPS(30)
    VPP_UC_MAX_IN_FPS(I, 33, 33, 33, 0)
    VPP_UC_FALLBACK(NULL, NULL, NULL, &uc_aie)
};

static t_StVppUsecase uc_di_cnr_aie = {
    .name = "DI+CNR+AIE",
    .algos = {
        .ctrls = (enum hqv_control_type[]) {
            HQV_CONTROL_DI,
            HQV_CONTROL_CNR,
            HQV_CONTROL_AIE,
        },
        .u32CtrlCnt = 3,
    },
    .composition = (t_StVppUsecaseIp[]) {
        {
            VPP_UC_COMP(HVX, 2, VPP_FALSE)
        },
    },
    .u32IpCnt = 1,
    .credits = {
        VPP_UC_CREDITS(VPP_IP_HVX, HVX, 15, 30, NOT_SUPPORTED, NOT_SUPPORTED)
    },
    VPP_UC_PORT_CFG(port_cfg_common)
    VPP_UC_FACTOR(2, 1, 0, 2)
    VPP_UC_DEFAULT_FPS(30)
    VPP_UC_MAX_IN_FPS(I, 33, 33, 0, 0)
    VPP_UC_FALLBACK(NULL, NULL, &uc_di_aie, &uc_aie)
};

static t_StVppUsecase uc_cnr = {
    .name = "CNR",
    .algos = {
        .ctrls = (enum hqv_control_type[]) {
            HQV_CONTROL_CNR,
        },
        .u32CtrlCnt = 1,
    },
    .composition = (t_StVppUsecaseIp[]) {
        {
            VPP_UC_COMP(HVX, 2, VPP_FALSE)
        },
    },
    .u32IpCnt = 1,
    .credits = {
        VPP_UC_CREDITS(VPP_IP_HVX, HVX, 15, 15, 15, 30)
    },
    VPP_UC_PORT_CFG(port_cfg_common)
    VPP_UC_FACTOR(0, 1, 0, 1)
    VPP_UC_DEFAULT_FPS(30)
    VPP_UC_FALLBACK(NULL, NULL, NULL, NULL)
};

static t_StVppUsecase uc_cnr_aie = {
    .name = "CNR+AIE",
    .algos = {
        .ctrls = (enum hqv_control_type[]) {
            HQV_CONTROL_CNR,
            HQV_CONTROL_AIE,
        },
        .u32CtrlCnt = 2,
    },
    .composition = (t_StVppUsecaseIp[]) {
        {
            VPP_UC_COMP(HVX, 2, VPP_FALSE)
        },
    },
    .u32IpCnt = 1,
    .credits = {
        VPP_UC_CREDITS(VPP_IP_HVX, HVX, 15, 15, 15, NOT_SUPPORTED)
    },
    VPP_UC_PORT_CFG(port_cfg_common)
    VPP_UC_FACTOR(0, 1, 0, 1)
    VPP_UC_DEFAULT_FPS(30)
    VPP_UC_FALLBACK(NULL, NULL, NULL, &uc_aie)
};

static t_StVppUsecase uc_auto_hvx = {
    .name = "AUTO",
    .algos = {
        .ctrls = (enum hqv_control_type[]) {
            HQV_CONTROL_CNR,
            HQV_CONTROL_AIE,
            HQV_CONTROL_DI,
        },
        .u32CtrlCnt = 3,
    },
    .composition = (t_StVppUsecaseIp[]) {
        {
            VPP_UC_COMP(HVX, 2, VPP_FALSE)
        },
    },
    .u32IpCnt = 1,
    .credits = {
        VPP_UC_CREDITS(VPP_IP_HVX, HVX, 15, 15, 15, 30)
    },
    VPP_UC_PORT_CFG(port_cfg_common)
    VPP_UC_FACTOR_BY_RES(VPP_RESOLUTION_SD, 2, 1, 0, 2)
    VPP_UC_FACTOR_BY_RES(VPP_RESOLUTION_HD, 2, 1, 0, 2)
    VPP_UC_FACTOR_BY_RES(VPP_RESOLUTION_FHD, 2, 1, 0, 2)
    VPP_UC_FACTOR_BY_RES(VPP_RESOLUTION_UHD, 0, 1, 0, 1)
    VPP_UC_DEFAULT_FPS(30)
    VPP_UC_MAX_IN_FPS(I, 33, 33, 33, 0)
    VPP_UC_FALLBACK(NULL, NULL, NULL, NULL)
};

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
        VPP_UC_CREDITS(VPP_IP_GPU, GPU, 30, 30, 30, NOT_SUPPORTED)
    },
    VPP_UC_PORT_CFG(port_cfg_common)
    VPP_UC_FACTOR(0, 1, 0, 1)
    VPP_UC_DEFAULT_FPS(30)
    VPP_UC_FALLBACK(NULL, NULL, NULL, NULL)
};

static t_StVppUsecase uc_auto_gpu = {
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
        VPP_UC_CREDITS(VPP_IP_GPU, GPU, 30, 30, 30, NOT_SUPPORTED)
    },
    VPP_UC_PORT_CFG(port_cfg_common)
    VPP_UC_FACTOR(0, 1, 0, 1)
    VPP_UC_DEFAULT_FPS(30)
    VPP_UC_FALLBACK(NULL, NULL, NULL, NULL)
};

static t_StVppUsecase * apstUcsdm660_hvx[] = {
    &uc_aie,
    &uc_di,
    &uc_di_cnr,
    &uc_di_aie,
    &uc_di_cnr_aie,
    &uc_cnr,
    &uc_cnr_aie,
};

static t_StVppUsecase * apstUcsdm660_gpu[] = {
    &uc_cade,
};

t_StVppUsecase ** ppstVppUsecase_GetRegistry(uint32_t *o_pu32Cnt)
{
    if (u32VppUtils_IsSoc(SDM660) == VPP_TRUE)
    {
        *o_pu32Cnt = sizeof(apstUcsdm660_hvx) / sizeof(t_StVppUsecase *);
        return apstUcsdm660_hvx;
    }

    *o_pu32Cnt = sizeof(apstUcsdm660_gpu) / sizeof(t_StVppUsecase *);
    return apstUcsdm660_gpu;

}

t_StVppUsecase * pstVppUsecase_GetAuto()
{
    if (u32VppUtils_IsSoc(SDM660) == VPP_TRUE)
        return &uc_auto_hvx;

    return &uc_auto_gpu;
}
