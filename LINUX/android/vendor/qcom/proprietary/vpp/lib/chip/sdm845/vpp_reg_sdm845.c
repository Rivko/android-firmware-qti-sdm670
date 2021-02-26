/*!
 * @file vpp_reg_sdm845.c
 *
 * @cr
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.

 * @services
 */

#include <sys/types.h>

#include "vpp.h"
#include "vpp_reg.h"
#include "vpp_ip_hcp.h"
#ifdef VPP_TARGET_USES_HVX
#include "vpp_ip_hvx.h"
#endif
#ifdef VPP_TARGET_USES_FRC
#include "vpp_ip_frc_core.h"
#endif

#ifndef OMX_EXTRADATA_DOES_NOT_EXIST
#include "OMX_QCOMExtns.h"
#endif

/************************************************************************
 * Common Definition
 ***********************************************************************/
t_EVppColorFmt color_fmt_common[] = {
    VPP_COLOR_FORMAT_NV12_VENUS,
    VPP_COLOR_FORMAT_NV21_VENUS,
    VPP_COLOR_FORMAT_P010,
    VPP_COLOR_FORMAT_UBWC_NV12,
    VPP_COLOR_FORMAT_UBWC_NV21,
    VPP_COLOR_FORMAT_UBWC_TP10,
};

t_EVppColorFmt color_fmt_frc[] = {
    VPP_COLOR_FORMAT_UBWC_NV12,
};

static t_StVppPortCfg port_cfg_hvx_common[] = {
    {
        VPP_UC_INTERNAL_REQ(1)
        {
            VPP_UC_PORT(VPP_COLOR_FORMAT_NV12_VENUS, VPP_COLOR_FORMAT_NV12_VENUS, \
                        0, eVppBufPxDataType_Raw)
        },
    },
};

static t_StVppPortCfg port_cfg_frc_common[] = {
    {
        VPP_UC_INTERNAL_REQ(1)
        {
            VPP_UC_PORT(VPP_COLOR_FORMAT_UBWC_NV12, VPP_COLOR_FORMAT_UBWC_NV12, \
                        0, eVppBufPxDataType_Raw)
        },
    },
};

static t_StVppPortCfg port_cfg_hcp_common[] = {
    {
        VPP_UC_INTERNAL_REQ(1)
        {
            VPP_UC_PORT(VPP_COLOR_FORMAT_NV12_VENUS, VPP_COLOR_FORMAT_NV12_VENUS, \
                        0, eVppBufPxDataType_Raw)
        },
    },
    {
        VPP_UC_INTERNAL_REQ(1)
        {
            VPP_UC_PORT(VPP_COLOR_FORMAT_NV21_VENUS, VPP_COLOR_FORMAT_NV21_VENUS, \
                        0, eVppBufPxDataType_Raw)
        },
    },
    {
        VPP_UC_INTERNAL_REQ(1)
        {
            VPP_UC_PORT(VPP_COLOR_FORMAT_P010, VPP_COLOR_FORMAT_P010, \
                        0, eVppBufPxDataType_Raw)
        },
    },
    {
        VPP_UC_INTERNAL_REQ(1)
        {
            VPP_UC_PORT(VPP_COLOR_FORMAT_UBWC_NV12, VPP_COLOR_FORMAT_UBWC_NV12, \
                        0, eVppBufPxDataType_Raw)
        },
    },
    {
        VPP_UC_INTERNAL_REQ(1)
        {
            VPP_UC_PORT(VPP_COLOR_FORMAT_UBWC_NV21, VPP_COLOR_FORMAT_UBWC_NV21, \
                        0, eVppBufPxDataType_Raw)
        },
    },
    {
        VPP_UC_INTERNAL_REQ(1)
        {
            VPP_UC_PORT(VPP_COLOR_FORMAT_UBWC_TP10, VPP_COLOR_FORMAT_UBWC_TP10, \
                        0, eVppBufPxDataType_Raw)
        },
    },
};

static t_StVppPortCfg port_cfg_hvx_hcp_common[] = {
    {
        VPP_UC_INTERNAL_REQ(2)
        {
            VPP_UC_PORT(VPP_COLOR_FORMAT_NV12_VENUS, VPP_COLOR_FORMAT_NV12_VENUS, \
                        10, eVppBufPxDataType_Raw)
            VPP_UC_PORT(VPP_COLOR_FORMAT_NV12_VENUS, VPP_COLOR_FORMAT_NV12_VENUS, \
                        0, eVppBufPxDataType_Raw)
        },
    },
};

static t_StVppPortCfg port_cfg_hcp_frc_common[] = {
    {
        VPP_UC_INTERNAL_REQ(2)
        {
            VPP_UC_PORT(VPP_COLOR_FORMAT_NV12_VENUS, VPP_COLOR_FORMAT_NV12_VENUS, \
                        0, eVppBufPxDataType_Raw)
            VPP_UC_PORT(VPP_COLOR_FORMAT_NV12_VENUS, VPP_COLOR_FORMAT_NV12_VENUS, \
                        0, eVppBufPxDataType_Raw)
        },
    },
};

/************************************************************************
 * HVX Definition
 ***********************************************************************/
#ifdef VPP_TARGET_USES_HVX
static t_StVppAlgo algo_hvx[] = {
    {
        .ctrl = HQV_CONTROL_QBR,
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
            NULL);
#endif

/************************************************************************
 * HCP Definition
 ***********************************************************************/
static t_StVppAlgo algo_hcp[] = {
    {
        .ctrl = HQV_CONTROL_AIE,
    },
    {
        .ctrl = HQV_CONTROL_EAR,
#ifndef OMX_EXTRADATA_DOES_NOT_EXIST
        .u32MetaCnt = 2,
        .meta = (uint32_t []) {
            OMX_QcomIndexParamVideoQPExtraData,
            OMX_QcomIndexParamFrameInfoExtraData,
        },
#endif
    },
    {
        .ctrl = HQV_CONTROL_QBR,
    },
    {
        .ctrl = HQV_CONTROL_GLOBAL_DEMO,
    },
    {
        .ctrl = HQV_CONTROL_MEAS,
    },
};

VPP_IP_DECL(HCP,
            VPP_IP_HCP,
            algo_hcp,
            960,            // Max credits
            VPP_QUIRK_NONE, // Quirks
            0,              // stride multiple
            0,              // scanline multiple
            8,              // max concurrent sessions
            VPP_TRUE,       // supports cpz?
            VPP_FALSE,      // requires mmap'd buffers?
            RES_UHD_MAX_W,  // max supported width
            RES_UHD_MAX_H,  // max supported height
            128,            // min supported width
            96,             // min supported height
            color_fmt_common, // supported input port color formats
            color_fmt_common, // supported output port color formats
            u32VppIpHcp_Boot,
            u32VppIpHcp_Shutdown,
            vpVppIpHcp_Init,
            vVppIpHcp_Term,
            u32VppIpHcp_Open,
            u32VppIpHcp_Close,
            u32VppIpHcp_SetParam,
            u32VppIpHcp_SetCtrl,
            u32VppIpHcp_GetBufferRequirements,
            u32VppIpHcp_QueueBuf,
            u32VppIpHcp_Flush,
            u32VppIpHcp_Drain,
            u32VppIpHcp_Reconfigure);

/************************************************************************
 * FRC Definition
 ***********************************************************************/
#ifdef VPP_TARGET_USES_FRC
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
            VPP_FALSE,
            RES_FHD_MAX_W,
            RES_FHD_MAX_H,
            324,
            128,
            color_fmt_frc,
            color_fmt_frc,
            NULL,
            NULL,
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
            u32VppIpFrc_Reconfigure);
#endif

/************************************************************************
 * Usecase Definition
 ***********************************************************************/

/************************************************************************
 * HCP Usecase Definition
 ***********************************************************************/

static t_StVppUsecase uc_meas = {
    .name = "MEAS",
    .algos = {
        .ctrls = (enum hqv_control_type[]) {
            HQV_CONTROL_MEAS,
        },
        .u32CtrlCnt = 1,
    },
    .composition = (t_StVppUsecaseIp[]) {
        {
            VPP_UC_COMP(HCP, 8, VPP_FALSE)
        },
    },
    .u32IpCnt = 1,
    .credits = {
        VPP_UC_CREDITS(VPP_IP_HCP, HCP, 2, 2, 8, 16)
    },
    VPP_UC_PORT_CFG(port_cfg_hcp_common)
    VPP_UC_FACTOR(0, 1, 0, 1)
    VPP_UC_DEFAULT_FPS(30)
    VPP_UC_FALLBACK(NULL, NULL, NULL, NULL)
};

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
            VPP_UC_COMP(HCP, 8, VPP_FALSE)
        },
    },
    .u32IpCnt = 1,
    .credits = {
        VPP_UC_CREDITS(VPP_IP_HCP, HCP, 2, 2, 8, 16)
    },
    VPP_UC_PORT_CFG(port_cfg_hcp_common)
    VPP_UC_FACTOR(0, 1, 0, 1)
    VPP_UC_DEFAULT_FPS(30)
    VPP_UC_FALLBACK(NULL, NULL, NULL, NULL)
};

static t_StVppUsecase uc_ear = {
    .name = "EAR",
    .algos = {
        .ctrls = (enum hqv_control_type[]) {
            HQV_CONTROL_EAR,
        },
        .u32CtrlCnt = 1,
    },
    .composition = (t_StVppUsecaseIp[]) {
        {
            VPP_UC_COMP(HCP, 8, VPP_FALSE)
        },
    },
    .u32IpCnt = 1,
    .credits = {
        VPP_UC_CREDITS(VPP_IP_HCP, HCP, 2, 2, 8, 16)
    },
    VPP_UC_PORT_CFG(port_cfg_hcp_common)
    VPP_UC_FACTOR(0, 1, 0, 1)
    VPP_UC_DEFAULT_FPS(30)
    VPP_UC_FALLBACK(NULL, NULL, NULL, NULL)
};

static t_StVppUsecase uc_ear_aie = {
    .name = "EAR+AIE",
    .algos = {
        .ctrls = (enum hqv_control_type[]) {
            HQV_CONTROL_EAR,
            HQV_CONTROL_AIE,
        },
        .u32CtrlCnt = 2,
    },
    .composition = (t_StVppUsecaseIp[]) {
        {
            VPP_UC_COMP(HCP, 8, VPP_FALSE)
        },
    },
    .u32IpCnt = 1,
    .credits = {
        VPP_UC_CREDITS(VPP_IP_HCP, HCP, 2, 2, 8, 16)
    },
    VPP_UC_PORT_CFG(port_cfg_hcp_common)
    VPP_UC_FACTOR(0, 1, 0, 1)
    VPP_UC_DEFAULT_FPS(30)
    VPP_UC_FALLBACK(NULL, NULL, NULL, NULL)
};

#if defined(VPP_TARGET_USES_HVX)
static t_StVppUsecase uc_qbr = {
    .name = "QBR",
    .algos = {
        .ctrls = (enum hqv_control_type[]) {
            HQV_CONTROL_QBR,
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
        VPP_UC_CREDITS(VPP_IP_HVX, HVX, 15, 15, 15, 30) // TODO: confirm QBR conc.
    },
    VPP_UC_PORT_CFG(port_cfg_hvx_common)
    VPP_UC_FACTOR(0, 1, 0, 1)
    VPP_UC_DEFAULT_FPS(30)
    VPP_UC_FALLBACK(NULL, NULL, NULL, NULL)
};

static t_StVppUsecase uc_qbr_aie = {
    .name = "QBR+AIE",
    .algos = {
        .ctrls = (enum hqv_control_type[]) {
            HQV_CONTROL_QBR,
            HQV_CONTROL_AIE,
        },
        .u32CtrlCnt = 2,
    },
    .composition = (t_StVppUsecaseIp[]) {
        {
            VPP_UC_COMP(HVX, 2, VPP_FALSE)
        },
        {
            VPP_UC_COMP(HCP, 8, VPP_FALSE)
        },
    },
    .u32IpCnt = 2,
    .credits = {
        VPP_UC_CREDITS(VPP_IP_HVX, HVX, 15, 15, 15, 30) // TODO: confirm QBR conc.
        VPP_UC_CREDITS(VPP_IP_HCP, HCP, 2, 2, 8, 16)
    },
    VPP_UC_PORT_CFG(port_cfg_hvx_hcp_common)
    VPP_UC_FACTOR(0, 1, 0, 1)
    VPP_UC_DEFAULT_FPS(30)
    VPP_UC_FALLBACK(NULL, NULL, NULL, NULL)
};

static t_StVppUsecase uc_qbr_ear = {
    .name = "QBR+EAR",
    .algos = {
        .ctrls = (enum hqv_control_type[]) {
            HQV_CONTROL_QBR,
            HQV_CONTROL_EAR,
        },
        .u32CtrlCnt = 2,
    },
    .composition = (t_StVppUsecaseIp[]) {
        {
            VPP_UC_COMP(HVX, 2, VPP_FALSE)
        },
        {
            VPP_UC_COMP(HCP, 8, VPP_FALSE)
        },
    },
    .u32IpCnt = 2,
    .credits = {
        VPP_UC_CREDITS(VPP_IP_HVX, HVX, 15, 15, 15, 30) // TODO: confirm QBR conc.
        VPP_UC_CREDITS(VPP_IP_HCP, HCP, 2, 2, 8, 16)
    },
    VPP_UC_PORT_CFG(port_cfg_hvx_hcp_common)
    VPP_UC_FACTOR(0, 1, 0, 1)
    VPP_UC_DEFAULT_FPS(30)
    VPP_UC_FALLBACK(NULL, NULL, NULL, NULL)
};

static t_StVppUsecase uc_qbr_ear_aie = {
    .name = "QBR+EAR+AIE",
    .algos = {
        .ctrls = (enum hqv_control_type[]) {
            HQV_CONTROL_QBR,
            HQV_CONTROL_EAR,
            HQV_CONTROL_AIE,
        },
        .u32CtrlCnt = 3,
    },
    .composition = (t_StVppUsecaseIp[]) {
        {
            VPP_UC_COMP(HVX, 2, VPP_FALSE)
        },
        {
            VPP_UC_COMP(HCP, 8, VPP_FALSE)
        },
    },
    .u32IpCnt = 2,
    .credits = {
        VPP_UC_CREDITS(VPP_IP_HCP, HCP, 2, 2, 8, 16)
    },
    VPP_UC_PORT_CFG(port_cfg_hvx_hcp_common)
    VPP_UC_FACTOR(0, 1, 0, 1)
    VPP_UC_DEFAULT_FPS(30)
    VPP_UC_FALLBACK(NULL, NULL, NULL, NULL)
};
#endif

/************************************************************************
 * FRC Usecase Definition
 ***********************************************************************/
#ifdef VPP_TARGET_USES_FRC
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
        VPP_UC_CREDITS(VPP_IP_FRC, FRC, 60, 60, 60, NOT_SUPPORTED)
#ifdef VPP_TARGET_USES_HVX
        VPP_UC_CREDITS(VPP_IP_HVX, HVX, 60, 60, 60, NOT_SUPPORTED)
#endif
    },
    VPP_UC_PORT_CFG(port_cfg_frc_common)
    VPP_UC_FACTOR(1, 1, 0, 3)
    VPP_UC_DEFAULT_FPS(7)
    VPP_UC_FALLBACK(NULL, NULL, NULL, NULL)
};
#endif

static t_StVppUsecase uc_auto = {
    .name = "AUTO",
    .algos = {
        .ctrls = (enum hqv_control_type[]) {
            HQV_CONTROL_EAR,
            HQV_CONTROL_AIE,
        },
        .u32CtrlCnt = 2,
    },
    .composition = (t_StVppUsecaseIp[]) {
        {
            VPP_UC_COMP(HCP, 8, VPP_FALSE)
        },
    },
    .u32IpCnt = 1,
    .credits = {
        VPP_UC_CREDITS(VPP_IP_HCP, HCP, 2, 2, 8, 16)
    },
    VPP_UC_PORT_CFG(port_cfg_hcp_common)
    VPP_UC_FACTOR(0, 1, 0, 1)
    VPP_UC_DEFAULT_FPS(30)
    VPP_UC_FALLBACK(NULL, NULL, NULL, NULL)
};

#define VPP_845_HCP_UC
#define VPP_845_FRC_UC
// #define VPP_845_MEAS_UC
// #define VPP_845_HVX_UC
// #define VPP_845_MULTI_IP_UC

static t_StVppUsecase * apstUcSdm845[] = {
#ifdef VPP_845_MEAS_UC
    &uc_meas,
#endif
#ifdef VPP_845_HCP_UC
    &uc_aie,
    &uc_ear,
    &uc_ear_aie,
#endif
#if defined(VPP_845_HVX_UC) && defined(VPP_TARGET_USES_HVX)
    &uc_qbr,
#endif
#if defined(VPP_845_MULTI_IP_UC) && defined(VPP_TARGET_USES_HVX)
    &uc_qbr_aie,
    &uc_qbr_ear,
    &uc_qbr_ear_aie,
#endif
#if defined(VPP_845_FRC_UC) && defined(VPP_TARGET_USES_FRC)
    &uc_frc,
#endif
};

t_StVppUsecase ** ppstVppUsecase_GetRegistry(uint32_t *o_pu32Cnt)
{
    *o_pu32Cnt = sizeof(apstUcSdm845) / sizeof(t_StVppUsecase *);
    return apstUcSdm845;
}

t_StVppUsecase * pstVppUsecase_GetAuto()
{
    return &uc_auto;
}
