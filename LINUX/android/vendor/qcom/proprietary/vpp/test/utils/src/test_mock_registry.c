/*!
 * @file test_mock_registry.c
 *
 * @cr
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.

 * @services    <Describe what this is supposed to test>
 */

#include <sys/types.h>
#include <string.h>

#include "dvpTest.h"
#include "dvpTest_tb.h"

#include "vpp.h"
#include "vpp_core.h"
// #include "vpp_ctx.h"
#include "vpp_reg.h"
// #include "vpp_uc.h"
#include <media/msm_vidc.h>

#include "test_mock_registry.h"
#include "test_mock_ip.h"
/************************************************************************
 * Local definitions
 ***********************************************************************/
static t_StVppPortCfg port_cfg_common[] = {
    {
        VPP_UC_INTERNAL_REQ(1)
        {
            VPP_UC_PORT(VPP_COLOR_FORMAT_NV12_VENUS, VPP_COLOR_FORMAT_NV12_VENUS, \
                        0, eVppBufPxDataType_Raw)
        },
    },
};

static t_StVppPortCfg port_cfg_gpu_hvx_common[] = {
    {
        VPP_UC_INTERNAL_REQ(2)
        {
            VPP_UC_PORT(VPP_COLOR_FORMAT_NV21_VENUS, VPP_COLOR_FORMAT_NV21_VENUS, \
                        10, eVppBufPxDataType_Raw)
            VPP_UC_PORT(VPP_COLOR_FORMAT_NV21_VENUS, VPP_COLOR_FORMAT_NV21_VENUS, \
                        0, eVppBufPxDataType_Raw)
        },
    },
    {
        VPP_UC_INTERNAL_REQ(2)
        {
            VPP_UC_PORT(VPP_COLOR_FORMAT_NV12_VENUS, VPP_COLOR_FORMAT_NV12_VENUS, \
                        10, eVppBufPxDataType_Raw)
            VPP_UC_PORT(VPP_COLOR_FORMAT_NV12_VENUS, VPP_COLOR_FORMAT_NV12_VENUS,
                        0, eVppBufPxDataType_Raw)
        },
    },
};

static t_StVppPortCfg port_cfg_gpu_frc_common[] = {
    {
        VPP_UC_INTERNAL_REQ(2)
        {
            VPP_UC_PORT(VPP_COLOR_FORMAT_NV21_VENUS, VPP_COLOR_FORMAT_NV21_VENUS, \
                        0, eVppBufPxDataType_Raw)
            VPP_UC_PORT(VPP_COLOR_FORMAT_NV21_VENUS, VPP_COLOR_FORMAT_NV21_VENUS, \
                        0, eVppBufPxDataType_Raw)
        },
    },
    {
        VPP_UC_INTERNAL_REQ(2)
        {
            VPP_UC_PORT(VPP_COLOR_FORMAT_NV12_VENUS, VPP_COLOR_FORMAT_NV12_VENUS, \
                        0, eVppBufPxDataType_Raw)
            VPP_UC_PORT(VPP_COLOR_FORMAT_NV12_VENUS, VPP_COLOR_FORMAT_NV12_VENUS,
                        0, eVppBufPxDataType_Raw)
        },
    },
};

static t_StVppPortCfg port_cfg_hvx_gpu_hvx_uctest[] = {
    {
        VPP_UC_INTERNAL_REQ(3)
        {
            VPP_UC_PORT(VPP_COLOR_FORMAT_NV21_VENUS, VPP_COLOR_FORMAT_NV21_VENUS, \
                        12, eVppBufPxDataType_Raw)
            VPP_UC_PORT(VPP_COLOR_FORMAT_NV21_VENUS, VPP_COLOR_FORMAT_NV21_VENUS, \
                        10, eVppBufPxDataType_Raw)
            VPP_UC_PORT(VPP_COLOR_FORMAT_NV21_VENUS, VPP_COLOR_FORMAT_NV21_VENUS, \
                        0, eVppBufPxDataType_Raw)
        },
    },
    {
        VPP_UC_INTERNAL_REQ(3)
        {
            VPP_UC_PORT(VPP_COLOR_FORMAT_UBWC_TP10, VPP_COLOR_FORMAT_UBWC_TP10, \
                        9, eVppBufPxDataType_Raw)
            VPP_UC_PORT(VPP_COLOR_FORMAT_UBWC_TP10, VPP_COLOR_FORMAT_UBWC_TP10, \
                        5, eVppBufPxDataType_Raw)
            VPP_UC_PORT(VPP_COLOR_FORMAT_UBWC_TP10, VPP_COLOR_FORMAT_UBWC_TP10, \
                        0, eVppBufPxDataType_Raw)
        },
    },
    {
        VPP_UC_INTERNAL_REQ(3)
        {
            VPP_UC_PORT(VPP_COLOR_FORMAT_NV12_VENUS, VPP_COLOR_FORMAT_NV12_VENUS, \
                        8, eVppBufPxDataType_Raw)
            VPP_UC_PORT(VPP_COLOR_FORMAT_NV12_VENUS, VPP_COLOR_FORMAT_NV12_VENUS, \
                        6, eVppBufPxDataType_Raw)
            VPP_UC_PORT(VPP_COLOR_FORMAT_NV12_VENUS, VPP_COLOR_FORMAT_NV12_VENUS,
                        0, eVppBufPxDataType_Raw)
        },
    },
};

/************************************************************************
 * Mock HVX Definition
 ***********************************************************************/
static t_StVppAlgo algo_hvx[] = {
    {
        .ctrl = HQV_CONTROL_DI,
    },
    {
        .ctrl = HQV_CONTROL_TNR,
    },
    {
        .ctrl = HQV_CONTROL_CNR,
        .u32MetaCnt = 1,
        .meta = (uint32_t[]) {
            MSM_VIDC_EXTRADATA_METADATA_MBI,
        },
    },
    {
        .ctrl = HQV_CONTROL_AIE,
    },
    {
        .ctrl = HQV_CONTROL_FRC,
    },
};

t_EVppColorFmt color_fmt_hvx[] = {
    VPP_COLOR_FORMAT_NV12_VENUS,
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
            VPP_TRUE,
            4096,
            2160,
            128,
            96,
            color_fmt_hvx,
            color_fmt_hvx,
            NULL,
            NULL,
            vpVppIpMock_Init,
            vVppIpMock_Term,
            u32VppIpMock_Open,
            u32VppIpMock_Close,
            u32VppIpMock_SetParam,
            u32VppIpMock_SetCtrl,
            u32VppIpMock_GetBufferRequirements,
            u32VppIpMock_QueueBuf,
            u32VppIpMock_Flush,
            u32VppIpMock_Drain,
            NULL);

/************************************************************************
 * Mock GPU Definition
 ***********************************************************************/
static t_StVppAlgo algo_gpu[] = {
    {
        .ctrl = HQV_CONTROL_CADE,
    },
    {
        .ctrl = HQV_CONTROL_AIE,
    },
    {
        .ctrl = HQV_CONTROL_DI,
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
            VPP_FALSE,
            VPP_TRUE,
            1920,
            1088,
            96,
            64,
            color_fmt_gpu,
            color_fmt_gpu,
            NULL,
            NULL,
            vpVppIpMock_Init,
            vVppIpMock_Term,
            u32VppIpMock_Open,
            u32VppIpMock_Close,
            u32VppIpMock_SetParam,
            u32VppIpMock_SetCtrl,
            u32VppIpMock_GetBufferRequirements,
            u32VppIpMock_QueueBuf,
            u32VppIpMock_Flush,
            u32VppIpMock_Drain,
            NULL);

VPP_IP_DECL(GPU_NO_CP,
            VPP_IP_GPU,
            algo_gpu,
            1000,
            0,
            0,
            0,
            2,
            VPP_FALSE,
            VPP_TRUE,
            1920,
            1088,
            96,
            64,
            color_fmt_gpu,
            color_fmt_gpu,
            NULL,
            NULL,
            vpVppIpMock_Init,
            vVppIpMock_Term,
            u32VppIpMock_Open,
            u32VppIpMock_Close,
            u32VppIpMock_SetParam,
            u32VppIpMock_SetCtrl,
            u32VppIpMock_GetBufferRequirements,
            u32VppIpMock_QueueBuf,
            u32VppIpMock_Flush,
            u32VppIpMock_Drain,
            NULL);

/************************************************************************
 * Usecase Definitions
 ***********************************************************************/
static t_StVppUsecase uc_mock_aie = {
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

static t_StVppUsecase uc_mock_di = {
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
    VPP_UC_FACTOR(3, 1, 0, 2)
    VPP_UC_DEFAULT_FPS(30)
    VPP_UC_FALLBACK(NULL, NULL, NULL, NULL)
};

static t_StVppUsecase uc_mock_di_tnr = {
    .name = "DI+TNR",
    .algos = {
        .ctrls = (enum hqv_control_type[]) {
            HQV_CONTROL_DI,
            HQV_CONTROL_TNR,
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
    VPP_UC_FACTOR(3, 1, 0, 2)
    VPP_UC_DEFAULT_FPS(30)
    VPP_UC_FALLBACK(NULL, NULL, NULL, NULL)
};

#ifdef VPP_PIPELINE_MOCK_ALL_SUPPORTED_UCS
static t_StVppUsecase uc_mock_di_cnr = {
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
    VPP_UC_FACTOR(3, 1, 0, 2)
    VPP_UC_DEFAULT_FPS(30)
    VPP_UC_FALLBACK(NULL, NULL, NULL, NULL)
};

static t_StVppUsecase uc_mock_di_aie = {
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
    VPP_UC_FACTOR(3, 1, 0, 2)
    VPP_UC_DEFAULT_FPS(30)
    VPP_UC_FALLBACK(NULL, NULL, NULL, &uc_mock_aie)
};

static t_StVppUsecase uc_mock_tnr_aie = {
    .name = "TNR+AIE",
    .algos = {
        .ctrls = (enum hqv_control_type[]) {
            HQV_CONTROL_TNR,
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
    VPP_UC_FALLBACK(NULL, NULL, NULL, &uc_mock_aie)
};
#endif

static t_StVppUsecase uc_mock_di_tnr_aie = {
    .name = "DI+TNR+AIE",
    .algos = {
        .ctrls = (enum hqv_control_type[]) {
            HQV_CONTROL_DI,
            HQV_CONTROL_TNR,
            HQV_CONTROL_AIE,
        },
        .u32CtrlCnt = 3,
    },
    .composition = (t_StVppUsecaseIp[]) {
        {
            VPP_UC_COMP(HVX, 2, VPP_FALSE)
        },
        {
            VPP_UC_COMP(GPU, 2, VPP_FALSE)
        },
    },
    .u32IpCnt = 2,
    .credits = {
        VPP_UC_CREDITS(VPP_IP_HVX, HVX, 10, 15, 25, NOT_SUPPORTED)
        VPP_UC_CREDITS(VPP_IP_GPU, GPU, 15, 20, 30, NOT_SUPPORTED)
    },
    VPP_UC_PORT_CFG(port_cfg_common)
    VPP_UC_FACTOR(3, 1, 0, 2)
    VPP_UC_DEFAULT_FPS(30)
    VPP_UC_FALLBACK(NULL, NULL, NULL, NULL)
};

static t_StVppUsecase uc_mock_di_cnr_aie = {
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
        VPP_UC_CREDITS(VPP_IP_HVX, HVX, 15, 30, 30, NOT_SUPPORTED)
    },
    VPP_UC_PORT_CFG(port_cfg_common)
    VPP_UC_FACTOR(3, 1, 0, 2)
    VPP_UC_DEFAULT_FPS(30)
    VPP_UC_FALLBACK(NULL, NULL, NULL, &uc_mock_aie)
};

static t_StVppUsecase uc_mock_tnr = {
    .name = "TNR",
    .algos = {
        .ctrls = (enum hqv_control_type[]) {
            HQV_CONTROL_TNR,
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
        VPP_UC_CREDITS(VPP_IP_HVX, HVX, 15, 30, 15, NOT_SUPPORTED)
    },
    VPP_UC_PORT_CFG(port_cfg_common)
    VPP_UC_FACTOR(0, 1, 0, 1)
    VPP_UC_DEFAULT_FPS(30)
    VPP_UC_FALLBACK(NULL, NULL, NULL, NULL)
};

static t_StVppUsecase uc_mock_cnr = {
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
        VPP_UC_CREDITS(VPP_IP_HVX, HVX, 15, 15, 15, NOT_SUPPORTED)
    },
    VPP_UC_PORT_CFG(port_cfg_common)
    VPP_UC_FACTOR(0, 1, 0, 1)
    VPP_UC_DEFAULT_FPS(30)
    VPP_UC_FALLBACK(NULL, NULL, NULL, NULL)
};

static t_StVppUsecase uc_mock_cnr_aie = {
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
    VPP_UC_FALLBACK(NULL, NULL, NULL, &uc_mock_aie)
};

static t_StVppUsecase uc_mock_frc = {
    .name = "FRC",
    .algos = {
        .ctrls = (enum hqv_control_type[]) {
            HQV_CONTROL_FRC,
        },
        .u32CtrlCnt = 1,
    },
    .composition = (t_StVppUsecaseIp[]) {
        {
            VPP_UC_COMP(HVX, 1, VPP_FALSE)
        },
        {
            VPP_UC_COMP(GPU, 1, VPP_FALSE)
        },
    },
    .u32IpCnt = 2,
    .credits = {
        VPP_UC_CREDITS(VPP_IP_HVX, HVX, 10, 15, 20, NOT_SUPPORTED)
        VPP_UC_CREDITS(VPP_IP_GPU, GPU, 10, 15, 20, NOT_SUPPORTED)
    },
    VPP_UC_PORT_CFG(port_cfg_common)
    VPP_UC_FACTOR(2, 1, 0, 2)
    VPP_UC_DEFAULT_FPS(30)
    VPP_UC_FALLBACK(NULL, NULL, NULL, NULL)
};

static t_StVppUsecase uc_mock_cade = {
    .name = "MOCK_CADE",
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

static t_StVppUsecase uc_mock_cade_no_cp = {
    .name = "MOCK_CADE_NO_CP",
    .algos = {
        .ctrls = (enum hqv_control_type[]) {
            HQV_CONTROL_CADE,
        },
        .u32CtrlCnt = 1,
    },
    .composition = (t_StVppUsecaseIp[]) {
        {
            VPP_UC_COMP(GPU_NO_CP, 1, VPP_FALSE)
        },
    },
    .u32IpCnt = 1,
    .credits = {
        VPP_UC_CREDITS(VPP_IP_GPU, GPU_NO_CP, 30, 30, 30, NOT_SUPPORTED)
    },
    VPP_UC_PORT_CFG(port_cfg_common)
    VPP_UC_FACTOR(0, 1, 0, 1)
    VPP_UC_DEFAULT_FPS(30)
    VPP_UC_FALLBACK(NULL, NULL, NULL, NULL)
};

static t_StVppUsecase uc_mock_auto = {
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

/************************************************************************
 * Usecases/Registry for pipeline reconfigure multi test
 ***********************************************************************/

t_StVppUsecase uc_mock_pl_aie = {
    .name = "AIE_1",
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

t_StVppUsecase uc_mock_pl_aie2 = {
    .name = "AIE_2",
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

t_StVppUsecase uc_mock_pl_cade = {
    .name = "MOCK_PL_CADE",
    .algos = {
        .ctrls = (enum hqv_control_type[]) {
            HQV_CONTROL_CADE,
        },
        .u32CtrlCnt = 1,
    },
    .composition = (t_StVppUsecaseIp[]) {
         {
            VPP_UC_COMP(GPU, 2, VPP_FALSE)
        },
    },
    .u32IpCnt = 1,
    .credits = {
        VPP_UC_CREDITS(VPP_IP_GPU, GPU, 15, 15, 15, 30)
    },
    VPP_UC_PORT_CFG(port_cfg_common)
    VPP_UC_FACTOR(0, 1, 0, 1)
    VPP_UC_DEFAULT_FPS(30)
    VPP_UC_FALLBACK(NULL, NULL, NULL, NULL)
};

t_StVppUsecase uc_mock_pl_zero_comp = {
    .name = "CADE (zero composition)",
    .algos = {
        .ctrls = (enum hqv_control_type[]) {
            HQV_CONTROL_CADE,
        },
        .u32CtrlCnt = 1,
    },
    .composition = (t_StVppUsecaseIp[]) {
    },
    .u32IpCnt = 0,
    .credits = {
    },
    VPP_UC_FACTOR(0, 1, 0, 1)
    VPP_UC_DEFAULT_FPS(30)
    VPP_UC_FALLBACK(NULL, NULL, NULL, NULL)
};

t_StVppUsecase uc_mock_pl_two_comp = {
    .name = "CADE (two composition)",
    .algos = {
        .ctrls = (enum hqv_control_type[]) {
            HQV_CONTROL_CADE,
        },
        .u32CtrlCnt = 1,
    },
    .composition = (t_StVppUsecaseIp[]) {
        {
            VPP_UC_COMP(GPU, 2, VPP_FALSE)
        },
        {
            VPP_UC_COMP(HVX, 2, VPP_FALSE)
        },
    },
    .u32IpCnt = 2,
    .credits = {
        VPP_UC_CREDITS(VPP_IP_GPU, GPU, 15, 15, 15, 30)
        VPP_UC_CREDITS(VPP_IP_HVX, HVX, 15, 15, 15, 30)
    },
    VPP_UC_PORT_CFG(port_cfg_gpu_hvx_common)
    VPP_UC_FACTOR(0, 1, 0, 1)
    VPP_UC_DEFAULT_FPS(30)
    VPP_UC_FALLBACK(NULL, NULL, NULL, NULL)
};

t_StVppUsecase uc_mock_uctest_hvx = {
    .name = "UCTEST_HVX_AIE",
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

t_StVppUsecase uc_mock_uctest_hvx_maxfps = {
    .name = "UCTEST_HVX_AIE",
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
    .credits = {
        VPP_UC_CREDITS(VPP_IP_HVX, HVX, 15, 15, 15, 30)
    },
    VPP_UC_PORT_CFG(port_cfg_common)
    VPP_UC_FACTOR(0, 1, 0, 1)
    VPP_UC_DEFAULT_FPS(30)
    VPP_UC_MAX_IN_FPS(P, 60, 50, 40, 30)
    VPP_UC_MAX_IN_FPS(I, 0, 25, 20, 0)
    VPP_UC_FALLBACK(NULL, NULL, NULL, NULL)
};


t_StVppUsecase uc_mock_uctest_fb_hvx = {
    .name = "UCTEST_HVX_CNR_AIE",
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
        VPP_UC_CREDITS(VPP_IP_HVX, HVX, 5, 10, 15, NOT_SUPPORTED)
    },
    VPP_UC_PORT_CFG(port_cfg_common)
    VPP_UC_FACTOR(0, 1, 0, 1)
    VPP_UC_DEFAULT_FPS(30)
    VPP_UC_FALLBACK(NULL, NULL, NULL, &uc_mock_uctest_hvx)
};

t_StVppUsecase uc_mock_uctest_gpu = {
    .name = "UCTEST_GPU_AIE",
    .algos = {
        .ctrls = (enum hqv_control_type[]) {
            HQV_CONTROL_AIE,
        },
        .u32CtrlCnt = 1,
    },
    .composition = (t_StVppUsecaseIp[]) {
        {
            VPP_UC_COMP(GPU, 2, VPP_FALSE)
        },
    },
    .u32IpCnt = 1,
    .credits = {
        VPP_UC_CREDITS(VPP_IP_GPU, GPU, 15, 20, 25, NOT_SUPPORTED)
    },
    VPP_UC_PORT_CFG(port_cfg_common)
    VPP_UC_FACTOR(0, 1, 0, 1)
    VPP_UC_DEFAULT_FPS(30)
    VPP_UC_FALLBACK(NULL, NULL, NULL, NULL)
};

t_StVppUsecase uc_mock_uctest_gpu_single_sess = {
    .name = "UCTEST_GPU_CADE",
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
        VPP_UC_CREDITS(VPP_IP_GPU, GPU, 10, 10, 10, NOT_SUPPORTED)
    },
    VPP_UC_PORT_CFG(port_cfg_common)
    VPP_UC_FACTOR(0, 1, 0, 1)
    VPP_UC_DEFAULT_FPS(30)
    VPP_UC_FALLBACK(NULL, NULL, NULL, NULL)
};

t_StVppUsecase uc_mock_uctest_multiip = {
    .name = "UCTEST_HVX_GPU_FRC",
    .algos = {
        .ctrls = (enum hqv_control_type[]) {
            HQV_CONTROL_FRC,
        },
        .u32CtrlCnt = 1,
    },
    .composition = (t_StVppUsecaseIp[]) {
        {
            VPP_UC_COMP(HVX, 1, VPP_FALSE)
        },
        {
            VPP_UC_COMP(GPU, 1, VPP_FALSE)
        },
    },
    .u32IpCnt = 2,
    .credits = {
        VPP_UC_CREDITS(VPP_IP_HVX, HVX, 15, 20, 25, NOT_SUPPORTED)
        VPP_UC_CREDITS(VPP_IP_GPU, GPU, 10, 20, 30, NOT_SUPPORTED)
    },
    VPP_UC_PORT_CFG(port_cfg_gpu_hvx_common)
    VPP_UC_FACTOR(0, 1, 0, 1)
    VPP_UC_DEFAULT_FPS(30)
    VPP_UC_FALLBACK(NULL, NULL, NULL, NULL)
};

t_StVppUsecase uc_mock_uctest_singleip_multicredit = {
    .name = "UCTEST_HVX_AIE_GPU_CREDITS",
    .algos = {
        .ctrls = (enum hqv_control_type[]) {
            HQV_CONTROL_TNR,
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
        VPP_UC_CREDITS(VPP_IP_GPU, GPU, 6, 12, 18, 24)
    },
    VPP_UC_PORT_CFG(port_cfg_common)
    VPP_UC_FACTOR(0, 1, 0, 1)
    VPP_UC_DEFAULT_FPS(30)
    VPP_UC_FALLBACK(NULL, NULL, NULL, NULL)
};

t_StVppUsecase uc_max_res_single = {
    .name = "UC_TEST_MAX_RES_SINGLE",
    .algos = {
        .ctrls = (enum hqv_control_type[]) {
            HQV_CONTROL_TNR,
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
        VPP_UC_CREDITS(VPP_IP_HVX, HVX, NOT_SUPPORTED, NOT_SUPPORTED, NOT_SUPPORTED, NOT_SUPPORTED)
    },
    VPP_UC_PORT_CFG(port_cfg_common)
    VPP_UC_FACTOR(0, 1, 0, 1)
    VPP_UC_DEFAULT_FPS(30)
    VPP_UC_FALLBACK(NULL, NULL, NULL, NULL)
};

t_StVppUsecase uc_max_res_double = {
    .name = "UC_TEST_MAX_RES_DOUBLE",
    .algos = {
        .ctrls = (enum hqv_control_type[]) {
            HQV_CONTROL_TNR,
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
        VPP_UC_CREDITS(VPP_IP_HVX, HVX, NOT_SUPPORTED, NOT_SUPPORTED, NOT_SUPPORTED, NOT_SUPPORTED)
        VPP_UC_CREDITS(VPP_IP_GPU, GPU, NOT_SUPPORTED, NOT_SUPPORTED, NOT_SUPPORTED, NOT_SUPPORTED)
    },
    VPP_UC_PORT_CFG(port_cfg_common)
    VPP_UC_FACTOR(0, 1, 0, 1)
    VPP_UC_DEFAULT_FPS(30)
    VPP_UC_FALLBACK(NULL, NULL, NULL, NULL)
};

t_StVppUsecase uc_mock_uctest_two_ip = {
    .name = "UCTEST_GPUCADE_HVXCNR",
    .algos = {
        .ctrls = (enum hqv_control_type[]) {
            HQV_CONTROL_CADE,
            HQV_CONTROL_CNR,
        },
        .u32CtrlCnt = 2,
    },
    .composition = (t_StVppUsecaseIp[]) {
        {
            VPP_UC_COMP(GPU, 1, VPP_FALSE)
        },
        {
            VPP_UC_COMP(HVX, 1, VPP_FALSE)
        },
    },
    .u32IpCnt = 2,
    .credits = {
        VPP_UC_CREDITS(VPP_IP_HVX, HVX, 15, 20, 25, NOT_SUPPORTED)
        VPP_UC_CREDITS(VPP_IP_GPU, GPU, 10, 20, 30, NOT_SUPPORTED)
    },
    VPP_UC_PORT_CFG(port_cfg_gpu_hvx_common)
    VPP_UC_FACTOR(0, 1, 0, 1)
    VPP_UC_DEFAULT_FPS(30)
    VPP_UC_FALLBACK(NULL, NULL, NULL, NULL)
};

t_StVppUsecase uc_mock_uctest_two_ip_external_buf = {
    .name = "UCTEST_GPUCADE_FRC",
    .algos = {
        .ctrls = (enum hqv_control_type[]) {
            HQV_CONTROL_CADE,
            HQV_CONTROL_FRC,
        },
        .u32CtrlCnt = 2,
    },
    .composition = (t_StVppUsecaseIp[]) {
        {
            VPP_UC_COMP(GPU, 1, VPP_FALSE)
        },
        {
            VPP_UC_COMP(HVX, 1, VPP_FALSE)
        },
    },
    .u32IpCnt = 2,
    .credits = {
        VPP_UC_CREDITS(VPP_IP_HVX, HVX, 15, 20, 25, NOT_SUPPORTED)
        VPP_UC_CREDITS(VPP_IP_GPU, GPU, 10, 20, 30, NOT_SUPPORTED)
    },
    VPP_UC_PORT_CFG(port_cfg_gpu_frc_common)
    VPP_UC_FACTOR(0, 1, 0, 1)
    VPP_UC_DEFAULT_FPS(30)
    VPP_UC_FALLBACK(NULL, NULL, NULL, NULL)
};

t_StVppUsecase uc_mock_uctest_three_ip = {
    .name = "UCTEST_HVX_DI_GPU_CADE_SCALE_HVX_CNR_AIE",
    .algos = {
        .ctrls = (enum hqv_control_type[]) {
            HQV_CONTROL_DI,
            HQV_CONTROL_CADE,
            HQV_CONTROL_CNR,
            HQV_CONTROL_AIE,
        },
        .u32CtrlCnt = 4,
    },
    .composition = (t_StVppUsecaseIp[]) {
        {
            VPP_UC_COMP(HVX, 1, VPP_FALSE)
            .ipAlgos = {
                .ctrls = (enum hqv_control_type[]) {
                    HQV_CONTROL_DI,
                },
                .u32CtrlCnt = 1,
            },
        },
        {
            VPP_UC_COMP(GPU, 1, VPP_TRUE)
            .ipAlgos = {
                .ctrls = (enum hqv_control_type[]) {
                    HQV_CONTROL_CADE,
                },
                .u32CtrlCnt = 1,
            },
        },
        {
            VPP_UC_COMP(HVX, 1, VPP_FALSE)
            .ipAlgos = {
                .ctrls = (enum hqv_control_type[]) {
                    HQV_CONTROL_CNR,
                    HQV_CONTROL_AIE,
                },
                .u32CtrlCnt = 2,
            },
        },
    },
    .u32IpCnt = 3,
    .credits = {
        VPP_UC_CREDITS(VPP_IP_HVX, HVX, 15, 25, 30, NOT_SUPPORTED)
        VPP_UC_CREDITS(VPP_IP_GPU, GPU, 10, 20, 30, NOT_SUPPORTED)
    },
    VPP_UC_PORT_CFG(port_cfg_hvx_gpu_hvx_uctest)
    VPP_UC_FACTOR(0, 1, 0, 1)
    VPP_UC_DEFAULT_FPS(30)
    VPP_UC_FALLBACK(NULL, NULL, NULL, NULL)
};


t_StVppUsecase uc_mock_hvx_auto = {
    .name = "UCTEST_HVX_AUTO",
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
        VPP_UC_CREDITS(VPP_IP_HVX, HVX, 5, 10, 15, 30)
    },
    VPP_UC_PORT_CFG(port_cfg_common)
    VPP_UC_FACTOR_BY_RES(VPP_RESOLUTION_SD, 2, 1, 0, 2)
    VPP_UC_FACTOR_BY_RES(VPP_RESOLUTION_HD, 2, 1, 0, 2)
    VPP_UC_FACTOR_BY_RES(VPP_RESOLUTION_FHD, 2, 1, 0, 2)
    VPP_UC_FACTOR_BY_RES(VPP_RESOLUTION_UHD, 0, 1, 0, 1)
    VPP_UC_DEFAULT_FPS(30)
    VPP_UC_FALLBACK(NULL, NULL, NULL, &uc_mock_uctest_hvx)
};
/************************************************************************
 * Registries
 ***********************************************************************/
static t_StVppUsecase *apstUcTestReg[] = {
    &uc_mock_aie,
    &uc_mock_di,
    &uc_mock_di_tnr,
    &uc_mock_di_tnr_aie,
    &uc_mock_cnr_aie,
    &uc_mock_di_cnr_aie,
    &uc_mock_frc,
};

static t_StVppUsecase *apstUcVppClientReg[] = {
    &uc_mock_cade,
};

static t_StVppUsecase *apstUcVppClientNoCpReg[] = {
    &uc_mock_cade_no_cp,
};

#ifdef VPP_PIPELINE_MOCK_ALL_SUPPORTED_UCS
static t_StVppUsecase *apstUcVppPipelineReg[] = {
    &uc_mock_aie,
    &uc_mock_di,
    &uc_mock_di_tnr,
    &uc_mock_di_cnr,
    &uc_mock_di_aie,
    // &uc_mock_di_tnr_aie,
    &uc_mock_di_cnr_aie,
    &uc_mock_tnr,
    &uc_mock_tnr_aie,
    &uc_mock_cnr,
    &uc_mock_cnr_aie,
    // &uc_mock_frc,
    &uc_mock_cade,
};
#else
static t_StVppUsecase *apstUcVppPipelineReg[] = {
    &uc_mock_aie,
    &uc_mock_di,
    &uc_mock_tnr,
    &uc_mock_cnr,
    // &uc_mock_frc,
    &uc_mock_cade,
    &uc_mock_uctest_two_ip,
    &uc_mock_uctest_three_ip,
    &uc_mock_uctest_two_ip_external_buf,
};
#endif

static t_StVppUsecase *apstUcVppRegEmpty[] = {
};

static t_StVppUsecase *apstUcVppRegCustomCtrl[] = {
    &uc_mock_aie,
    &uc_mock_cade,
};

static t_StVppUsecase *apstUcVppMaxRes[] = {
    &uc_max_res_single,
    &uc_max_res_double,
};

/************************************************************************
 * Local Functions
 ***********************************************************************/
#ifndef VPP_TEST_BUILD
void vVppUsecase_InitTestRegistry(uint32_t u32RegSz,
                                  t_StVppUsecase **ppstUcReg,
                                  t_StVppUsecase *pstUcAuto)
{
    VPP_UNUSED(u32RegSz);
    VPP_UNUSED(ppstUcReg);
    VPP_UNUSED(pstUcAuto);
}

void vVppUsecase_TermTestRegistry()
{
}
#endif

/************************************************************************
 * Global Functions
 ***********************************************************************/
void get_registry_details(enum test_suite ts,
                          uint32_t *pu32Sz,
                          t_StVppUsecase ***pppstUcReg,
                          t_StVppUsecase **ppstUcAuto)
{
    uint32_t u32Sz;
    t_StVppUsecase **ppstUcReg = NULL;
    t_StVppUsecase *pstUcAuto = NULL;

    switch (ts)
    {
        case TEST_SUITE_USECASE:
            u32Sz = sizeof(apstUcTestReg) / sizeof (t_StVppUsecase *);
            ppstUcReg = apstUcTestReg;
            pstUcAuto = &uc_mock_auto;
            break;
        case TEST_SUITE_VPP_CLIENT:
            u32Sz = sizeof(apstUcVppClientReg) / sizeof (t_StVppUsecase *);
            ppstUcReg = apstUcVppClientReg;
            pstUcAuto = &uc_mock_auto;
            break;
        case TEST_SUITE_VPP_CLIENT_NO_CP:
            u32Sz = sizeof(apstUcVppClientNoCpReg) / sizeof (t_StVppUsecase *);
            ppstUcReg = apstUcVppClientNoCpReg;
            pstUcAuto = &uc_mock_cade;
            break;
        case TEST_SUITE_PIPELINE:
            u32Sz = sizeof(apstUcVppPipelineReg) / sizeof (t_StVppUsecase *);
            ppstUcReg = apstUcVppPipelineReg;
            pstUcAuto = &uc_mock_cade;
            // pstUcAuto = NULL;
            break;
        case TEST_SUITE_PIPELINE_UC_COMP_IS_TWO:
            u32Sz = sizeof(apstUcVppRegEmpty) / sizeof (t_StVppUsecase *);
            ppstUcReg = apstUcVppRegEmpty;
            pstUcAuto = &uc_mock_pl_two_comp;
            break;

        case TEST_SUITE_PIPELINE_UC_COMP_IS_ZERO:
            u32Sz = sizeof(apstUcVppRegEmpty) / sizeof (t_StVppUsecase *);
            ppstUcReg = apstUcVppRegEmpty;
            pstUcAuto = &uc_mock_pl_zero_comp;
            break;
        case TEST_SUITE_PIPELINE_CUSTOM_CTRL:
            u32Sz = sizeof(apstUcVppRegCustomCtrl) / sizeof (t_StVppUsecase *);
            ppstUcReg = apstUcVppRegCustomCtrl;
            pstUcAuto = &uc_mock_pl_zero_comp;
            break;

        case TEST_SUITE_USECASE_MAX_RES:
            u32Sz = sizeof(apstUcVppMaxRes) / sizeof (t_StVppUsecase *);
            ppstUcReg = apstUcVppMaxRes;
            pstUcAuto = NULL;
            break;

        case TEST_SUITE_REAL_REGISTRY:
        default:
            ppstUcReg = ppstVppUsecase_GetRegistry(&u32Sz);
            pstUcAuto = pstVppUsecase_GetAuto();
            break;
    }

    if (pu32Sz)
        *pu32Sz = u32Sz;
    if (pppstUcReg)
        *pppstUcReg = ppstUcReg;
    if (ppstUcAuto)
        *ppstUcAuto = pstUcAuto;
}


void init_test_registry(enum test_suite ts)
{
    uint32_t u32Sz;
    t_StVppUsecase **ppstUcReg = NULL;
    t_StVppUsecase *pstUcAuto = NULL;

    get_registry_details(ts, &u32Sz, &ppstUcReg, &pstUcAuto);

    vVppUsecase_InitTestRegistry(u32Sz, ppstUcReg, pstUcAuto);

}

void term_test_registry()
{
    vVppUsecase_TermTestRegistry();
}

t_StVppUsecase *getUsecaseAt(enum test_suite ts, uint32_t idx)
{
    uint32_t u32Sz;
    t_StVppUsecase **ppstUcReg = NULL;
    t_StVppUsecase *pstUcAuto = NULL;

    get_registry_details(ts, &u32Sz, &ppstUcReg, &pstUcAuto);

    if (ppstUcReg && idx < u32Sz)
    {
        return ppstUcReg[idx];
    }

    return NULL;
}

t_StVppUsecase *getUsecaseAuto(enum test_suite ts)
{
    uint32_t u32Sz;
    t_StVppUsecase **ppstUcReg = NULL;
    t_StVppUsecase *pstUcAuto = NULL;

    get_registry_details(ts, &u32Sz, &ppstUcReg, &pstUcAuto);

    return pstUcAuto;
}

void populateInternalCtrl(t_StVppUsecase *pstUc, t_StVppHqvCtrl *pstCtrl,
                          uint32_t bIsAuto)
{
    // Set some default parameters based on the usecase
    uint32_t i;
    enum hqv_control_type ctrl_type;

    if (!pstCtrl)
        return;

    memset(pstCtrl, 0, sizeof(t_StVppHqvCtrl));

    if (!pstUc || bIsAuto)
    {
        pstCtrl->mode = HQV_MODE_AUTO;
    }
    else
    {
        pstCtrl->mode = HQV_MODE_MANUAL;

        for (i = 0; i < pstUc->algos.u32CtrlCnt; i++)
        {
            ctrl_type = pstUc->algos.ctrls[i];
            switch (ctrl_type)
            {
                case HQV_CONTROL_CADE:
                    pstCtrl->cade.mode = HQV_MODE_MANUAL;
                    pstCtrl->cade.cade_level = 50;
                    pstCtrl->cade.contrast = 50;
                    pstCtrl->cade.saturation = 50;
                    break;

                case HQV_CONTROL_DI:
                    pstCtrl->di.mode = HQV_DI_MODE_VIDEO_1F;
                    break;

                case HQV_CONTROL_TNR:
                    pstCtrl->tnr.mode = HQV_MODE_MANUAL;
                    pstCtrl->tnr.level = 50;
                    break;

                case HQV_CONTROL_CNR:
                    pstCtrl->cnr.mode = HQV_MODE_MANUAL;
                    pstCtrl->cnr.level = 50;
                    break;

                case HQV_CONTROL_AIE:
                    pstCtrl->aie.mode = HQV_MODE_MANUAL;
                    pstCtrl->aie.hue_mode = HQV_HUE_MODE_OFF;
                    pstCtrl->aie.cade_level = 50;
                    pstCtrl->aie.ltm_level = 50;
                    break;

                case HQV_CONTROL_FRC:
                    pstCtrl->frc.mode = HQV_FRC_MODE_SLOMO;
                    pstCtrl->frc.level = HQV_FRC_LEVEL_HIGH;
                    pstCtrl->frc.interp = HQV_FRC_INTERP_2X;
                    break;

                case HQV_CONTROL_EAR:
                    pstCtrl->ear.mode = HQV_EAR_MODE_MEDIUM;
                    break;

                case HQV_CONTROL_MEAS:
                    pstCtrl->meas.enable = VPP_TRUE;
                    break;

                case HQV_CONTROL_QBR:
                    pstCtrl->qbr.mode = HQV_QBR_MODE_ON;
                    break;

                case HQV_CONTROL_NONE:
                case HQV_CONTROL_CUST:
                case HQV_CONTROL_GLOBAL_DEMO:
                case HQV_CONTROL_MAX:
                default:
                    break;
            }
        }
    }
}

uint32_t uc_sends_all_input_to_output(t_StVppUsecase *pstUc)
{
    if (!pstUc)
        return VPP_FALSE;

    // For now, the only case where this is true is if the usecase is FRC only.
    // If FRC is paired with anything else, that something else will send
    // buffers back to the input port.

    if (pstUc->algos.u32CtrlCnt == 1 &&
        pstUc->algos.ctrls[0] == HQV_CONTROL_FRC)
        return VPP_TRUE;

    return VPP_FALSE;
}

uint32_t uc_frame_interpolate(t_StVppUsecase *pstUc)
{
    uint32_t i;

    if (!pstUc)
        return VPP_FALSE;

    // This covers the multi-IP case for FRC, where the bypassed FRC buffer is
    // an external output buffer that has been processed by previous IPs

    if (pstUc->algos.u32CtrlCnt > 1)
    {
        for (i = 0; i < pstUc->algos.u32CtrlCnt; i++)
        {
            if (pstUc->algos.ctrls[i] == HQV_CONTROL_FRC)
                return VPP_TRUE;
        }
    }

    return VPP_FALSE;
}
