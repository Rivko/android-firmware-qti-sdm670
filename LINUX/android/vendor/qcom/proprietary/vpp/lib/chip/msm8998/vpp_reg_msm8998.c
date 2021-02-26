/*!
 * @file vpp_reg_msm8998.c
 *
 * @cr
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.

 * @services
 */

#include <sys/types.h>
#include <media/msm_vidc.h>

#include "vpp.h"
#include "vpp_reg.h"
#include "vpp_ip_hvx.h"
#include "vpp_ip_frc_core.h"

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
            VPP_TRUE,
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
 * FRC Definition
 ***********************************************************************/
static t_StVppAlgo algo_frc[] = {
    {
        .ctrl = HQV_CONTROL_FRC,
    },
};

VPP_IP_DECL(FRC,
            VPP_IP_FRC,
            algo_frc,
            1000,
            VPP_QUIRK_NO_CONCURRENT_CP_NCP_PID,
            0,
            0,
            2,
            VPP_TRUE,
            VPP_TRUE,
            RES_FHD_MAX_W,
            RES_FHD_MAX_H,
            324,
            128,
            color_fmt_common,
            color_fmt_common,
            vpVppIpFrc_Init,
            vVppIpFrc_Term,
            u32VppIpFrc_Open,
            u32VppIpFrc_Close,
            u32VppIpFrc_SetParam,
            u32VppIpFrc_SetCtrl,
            u32VppIpFrc_GetBufferRequirements,
            u32VppIpFrc_QueueBuf,
            u32VppIpFrc_Flush,
            u32VppIpFrc_Drain,
            NULL);

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

static t_StVppUsecase uc_frc = {
    .name = "FRC",
    .algos = {
        .ctrls = (enum hqv_control_type[]) {
            HQV_CONTROL_FRC,
        },
        .u32CtrlCnt = 1,
    },
    .composition = (t_StVppUsecaseIp[]) {
        {
            VPP_UC_COMP(FRC, 1, VPP_FALSE)
        },
    },
    .u32IpCnt = 1,
    .credits = {
        VPP_UC_CREDITS(VPP_IP_FRC, FRC, 15, 15, 15, NOT_SUPPORTED)
        VPP_UC_CREDITS(VPP_IP_HVX, HVX, 15, 15, 15, NOT_SUPPORTED)
    },
    VPP_UC_PORT_CFG(port_cfg_common)
    VPP_UC_FACTOR(1, 1, 0, 1)
    VPP_UC_DEFAULT_FPS(30)
    VPP_UC_FALLBACK(NULL, NULL, NULL, NULL)
};

static t_StVppUsecase uc_auto = {
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

static t_StVppUsecase * apstUc8998[] = {
    &uc_aie,
    &uc_di,
    &uc_di_cnr,
    &uc_di_aie,
    &uc_di_cnr_aie,
    &uc_cnr,
    &uc_cnr_aie,
    &uc_frc,
};

t_StVppUsecase ** ppstVppUsecase_GetRegistry(uint32_t *o_pu32Cnt)
{
    *o_pu32Cnt = sizeof(apstUc8998) / sizeof(t_StVppUsecase *);
    return apstUc8998;
}

t_StVppUsecase * pstVppUsecase_GetAuto()
{
    return &uc_auto;
}
