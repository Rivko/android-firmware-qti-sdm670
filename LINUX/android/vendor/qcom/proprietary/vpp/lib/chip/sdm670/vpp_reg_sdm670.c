/*!
 * @file vpp_reg_sdm670.c
 *
 * @cr
 * Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.

 * @services
 */

#include <sys/types.h>

#include "vpp.h"
#include "vpp_reg.h"
#ifdef VPP_TARGET_USES_FRC
#include "vpp_ip_frc_core.h"
#endif

/************************************************************************
 * Common Definition
 ***********************************************************************/
t_EVppColorFmt color_fmt_frc[] = {
    VPP_COLOR_FORMAT_UBWC_NV12,
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
            1000,           // Max credits
            VPP_QUIRK_NONE, // Quirks
            0,              // stride multiple
            0,              // scanline multiple
            1,              // max concurrent sessions
            VPP_TRUE,       // supports cpz?
            VPP_FALSE,      // requires mmap'd buffers?
            RES_FHD_MAX_W,  // max supported width
            RES_FHD_MAX_H,  // max supported height
            324,            // min supported width
            128,            // min supported height
            color_fmt_frc,  // supported input port color formats
            color_fmt_frc,  // supported output port color formats
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

/************************************************************************
 * FRC Usecase Definition
 ***********************************************************************/
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

static t_StVppUsecase * apstUcSdm670[] = {
    &uc_frc,
};

t_StVppUsecase ** ppstVppUsecase_GetRegistry(uint32_t *o_pu32Cnt)
{
    *o_pu32Cnt = sizeof(apstUcSdm670) / sizeof(t_StVppUsecase *);
    return apstUcSdm670;
}

t_StVppUsecase * pstVppUsecase_GetAuto()
{
    return &uc_frc;
}
