/*!
 * @file vpp_uc.h
 *
 * @cr
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.

 * @services
 */

#ifndef _VPP_UC_H_
#define _VPP_UC_H_

#include <pthread.h>

#include "vpp_reg.h"

#ifdef __cplusplus
extern "C" {
#endif


#ifndef VPP_CONCURRENT_SESSION_MAX
#define VPP_CONCURRENT_SESSION_MAX 10
#endif

typedef struct StVppConcSession{
    uint32_t bInUse;

    t_StVppUsecase *pstUc;

    t_EVppRes tResolution;

    // Number of credits that are being used in this session
    uint32_t au32Credit[VPP_IP_BLOCK_MAX];

    uint32_t u32Fps;

    uint32_t u32Flags; // CP

    // Max concurrent sessions supported by the use case
    uint32_t au32MaxConcSession[VPP_IP_BLOCK_MAX];

    t_StVppCtx *pstUcCtx;
    struct StVppConcSession *pNext;
} t_StVppConcSession;

typedef struct {

    pthread_mutex_t mutex;

    t_StVppConcSession *pHead;
    uint32_t abBooted[VPP_IP_BLOCK_MAX];
} t_StVppConcCb;

typedef struct StVppUcCb {
    uint32_t u32Id;
} t_StVppUcCb;

/***************************************************************************
 * Function Prototypes
 ***************************************************************************/

// Concurrency
uint32_t u32VppUsecase_Init(t_StVppCtx *pstCtx);
void vVppUsecase_Term(t_StVppCtx *pstCtx);

uint32_t u32VppUsecase_Configure(t_StVppCtx *pstCtx, t_StVppUsecase *pstUc,
                                 t_StVppHqvCtrl *pstCtrl, t_StVppParam *pstParam);
uint32_t u32VppUsecase_UpdateFramerate(t_StVppCtx *pstCtx, t_StVppUsecase *pstUc,
                                       uint32_t u32Fps, t_EVppBufType eBufType);

// Usecase
t_StVppUsecase *pstVppUsecase_Find(t_StVppHqvCtrl *pstCtrl, t_StVppParam *pstParam);

uint32_t u32VppUsecase_GetRequirements(t_StVppUsecase *pstUc, struct vpp_requirements *o_pstReq);

uint32_t u32VppUsecase_Validate(t_StVppHqvCtrl *pstCtrl, t_StVppUsecase *pstUc,
                                t_StVppParam *pstParam, t_StVppUsecase **o_pstUcFallBack);

uint32_t u32VppUsecase_CompositionEqual(t_StVppUsecase *pstUc1,
                                        t_StVppUsecase *pstUc2);

uint32_t u32VppUsecase_SubmodCount(t_StVppUsecase *pstUc);

uint32_t u32VppIsCpSupported();

uint32_t u32VppUsecase_GetMaxResForUc(t_StVppUsecase *pstUc);
uint32_t u32VppUsecase_IsCtrlSupported(t_StVppUsecase *pstUc, struct hqv_control stCtrl);
uint32_t u32VppUsecase_IsMappedPxBufRequired(t_StVppUsecase *pstUc);
uint32_t u32VppUsecase_GetDefaultFps(t_StVppUsecase *pstUc);
uint32_t u32VppUsecase_GetInternalBufReq(t_StVppUsecase *pstUc, t_StVppParam *pstParam,
                                         uint32_t u32IpIndex, t_StVppIpInternalReqs *o_pstReq);
uint32_t u32VppUsecase_IsScalingSupported(t_StVppUsecase *pstUc, uint32_t *u32pIpIndex);
uint32_t u32VppUsecase_IpIndexSupportsAlgo(t_StVppUsecase *pstUc, uint32_t u32IpIndex,
                                           enum hqv_control_type eCtrl);
uint32_t u32VppUsecase_GetIpPortParam(t_StVppUsecase *pstUc, t_StVppParam *pstUcParam,
                                      uint32_t u32IpIndex, t_StVppParam *o_pstIpParam);
uint32_t u32GetEnabledControlCount(t_StVppHqvCtrl *pstCtrl);
uint32_t u32VppUseCase_BootIPs();
uint32_t u32VppUseCase_ShutdownIPs();
#ifdef __cplusplus
}
#endif

#endif /* _VPP_UC_H_ */
