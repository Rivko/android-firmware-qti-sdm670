/*!
 * @file vpp_utils.h
 *
 * @cr
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.

 * @services
 */
#ifndef _VPP_UTILS_H_
#define _VPP_UTILS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/time.h>
#include <fcntl.h>
#include <cutils/properties.h>

#include "vpp.h"
#include "vpp_ctx.h"

#define VPP_FLAG_SET(reg, flag)        (reg |= (flag))
#define VPP_FLAG_CLR(reg, flag)        (reg &= ~(flag))
#define VPP_FLAG_IS_SET(reg, flag)     (reg & (flag))

enum soc_family {
    MSM8956,
    MSM8976,
    MSM8996,
    SDM660,
};

typedef struct StVppVidPropCb {
    struct vid_prop_codec codec;
    struct vid_prop_non_realtime non_realtime;
    struct vid_prop_operating_rate operating_rate;
} t_StVppVidPropCb;

typedef struct StVppUtilsCb {
    uint32_t u32BufDumpEnable;
} t_StVppUtilsCb;

/***************************************************************************
 * Function Prototypes
 ***************************************************************************/
uint32_t u32VppUtils_Init(t_StVppCtx *pstCtx, uint32_t u32Flags);
uint32_t u32VppUtils_Term(t_StVppCtx *pstCtx);
float fVppUtils_ScaleFloat(float old_min, float old_max,
                           float new_min, float new_max, float x);

uint32_t u32VppUtils_IsFmtUbwc(enum vpp_color_format fmt);
uint32_t u32VppUtils_GetNumPlanes(enum vpp_color_format fmt);
uint32_t u32VppUtils_CalcStrideForPlane(uint32_t u32Width,
                                        enum vpp_color_format fmt,
                                        uint32_t u32Plane);
uint32_t u32VppUtils_GetPlaneSize(uint32_t u32W, uint32_t u32H,
                                  enum vpp_color_format fmt,
                                  uint32_t u32Plane);
uint32_t u32VppUtils_CalculateStride(uint32_t u32Width,
                                     enum vpp_color_format fmt);
uint32_t u32VppUtils_CalcScanlinesForPlane(uint32_t u32Height,
                                           enum vpp_color_format fmt,
                                           uint32_t u32Plane);
uint32_t u32VppUtils_CalculateScanlines(uint32_t u32Height,
                                        enum vpp_color_format fmt);
uint32_t u32VppUtils_GetStride(struct vpp_port_param *pstPort);
uint32_t u32VppUtils_GetUVOffset(struct vpp_port_param *pstPort);
uint32_t u32VppUtils_GetBufferSize(struct vpp_port_param *pstPort);
uint32_t u32VppUtils_GetPxBufferSize(struct vpp_port_param *pstPort);
uint32_t u32VppUtils_GetExtraDataBufferSize(struct vpp_port_param *pstPort);
uint32_t u32VppUtils_GetTvUs(struct timeval *tv);
uint32_t u32VppUtils_GetTvDiff(struct timeval *pstTvEnd,
                               struct timeval *pstTvStart);
uint32_t u32VppUtils_GetMbiSize(uint32_t width, uint32_t height);
uint32_t u32VppUtils_ReadPropertyU32(char *pcProp, uint32_t *pu32Val,
                                     char *pcDefault);
uint32_t u32VppUtils_SetVidProp(t_StVppCtx *pstCtx, struct video_property stProp);
uint32_t u32VppUtils_GetVidProp(t_StVppCtx *pstCtx, struct video_property *pstProp);
uint32_t u32VppUtils_SetBufDumpEnable(t_StVppCtx *pstCtx, uint32_t u32BufDumpEnable);
uint32_t u32VppUtils_GetBufDumpEnable(t_StVppCtx *pstCtx, uint32_t *u32BufDumpEnable);
uint32_t u32VppUtils_GetVppResolution(struct vpp_port_param *pstPortParam);
uint32_t u32VppUtils_IsSoc(enum soc_family eSoc);
#ifdef __cplusplus
 }
#endif

#endif /* _VPP_UTILS_H_ */
