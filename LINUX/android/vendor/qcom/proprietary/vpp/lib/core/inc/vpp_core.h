/*!
 * @file vpp_core.h
 *
 * @cr
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.

 * @services
 */
#ifndef _VPP_H_
#define _VPP_H_

#include "vpp.h"
#include "vpp_reg.h"
#include "vpp_stats.h"

typedef enum {
    VPP_STATE_NULL,
    VPP_STATE_INITED,
    VPP_STATE_ACTIVE,
    VPP_STATE_ERROR,
    VPP_STATE_MAX,
} t_EVppState;

#define VPP_FLAG_INPUT_PORT_SET         (1 << 0)
#define VPP_FLAG_OUTPUT_PORT_SET        (1 << 1)
#define VPP_FLAG_RECONFIG_PENDING       (1 << 2)
#define VPP_FLAG_RECONFIG_IN_PROGRESS   (1 << 3)

typedef struct {
    uint32_t u32SessionFlags;
    struct vpp_port_param input_port;
    struct vpp_port_param output_port;
} t_StVppParam;

#define VPP_FRAMERATE_HISTO_SZ 5
#define VPP_MAX_FRAME_RATE 240
#define VPP_MIN_FRAME_RATE 1
#define VPP_DEFAULT_FRAME_RATE 30
#define VPP_FRAME_RATE_DELTA_MAX 10
#define VPP_FRAME_RATE_UPDATE_THRESHOLD 3

typedef struct {
    uint32_t u32AvgFrameRate;
    uint32_t u32ProgrammedFrameRate;
    uint32_t u32Bypass;
    uint32_t u32aFrameRates[VPP_FRAMERATE_HISTO_SZ];
    uint32_t u32Index;
    uint32_t u32Entries;
    uint32_t u32PrevTimestamp;
    uint32_t u32PrevFrameRate;
    t_EVppBufType ePrevBufType;
} t_StVppFrameRate;

typedef struct {
    enum hqv_mode mode;
    struct hqv_ctrl_cade cade;
    struct hqv_ctrl_tnr tnr;
    struct hqv_ctrl_cnr cnr;
    struct hqv_ctrl_aie aie;
    struct hqv_ctrl_di di;
    struct hqv_ctrl_frc frc;
    struct hqv_ctrl_ear ear;
    struct hqv_ctrl_qbr qbr;
    struct hqv_ctrl_meas meas;
    struct hqv_ctrl_custom custom;
    struct hqv_ctrl_global_demo demo;
} t_StVppHqvCtrl;

typedef struct StVppCb {
    struct vpp_callbacks stCb;
    pthread_mutex_t mutex;

    t_StVppParam stParam;
    t_StVppParam stReconfigParam;

    t_StVppHqvCtrl stCtrl;

    t_EVppState eState;

    t_StVppUsecase *pstUc;
    t_StVppUsecase *pstUcFb;

    // Internal flags, VPP_FLAG_INPUT_PORT_SET, VPP_FLAG_OUTPUT_PORT_SET
    uint32_t u32VppFlags;

    t_StVppFrameRate stFrameRate;

    struct {
        STAT_DECL(QInputBuf);
        STAT_DECL(QOutputBuf);
        STAT_DECL(IBD);
        STAT_DECL(OBD);
        STAT_DECL(InputFlush);
        STAT_DECL(OutputFlush);
        STAT_DECL(InputFlushDone);
        STAT_DECL(OutputFlushDone);
    } VPP_STAT_CB_NM;
} t_StVppCb;
/***************************************************************************
 * Function Prototypes
 ***************************************************************************/

#endif /* _VPP_H_ */
