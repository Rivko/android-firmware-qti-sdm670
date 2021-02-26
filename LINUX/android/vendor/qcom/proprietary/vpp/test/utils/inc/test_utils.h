/*!
 * @file test_utils.h
 *
 * @cr
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.

 * @services
 */
#ifndef _TEST_UTILS_H_
#define _TEST_UTILS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>
#include <pthread.h>

#include "vpp_ctx.h"
#include "vpp_callback.h"
#include "vpp_queue.h"
#include "vpp_core.h"

#include "buf_pool.h"

enum ts_freq {
    TS_FREQ_7HZ = 7,
    TS_FREQ_15HZ = 15,
    TS_FREQ_30HZ = 30,
    TS_FREQ_60HZ = 60,
    TS_FREQ_120HZ = 120,
};

struct ts_mgr {
    t_StVppQueue stQ;
    enum ts_freq freq;
    uint32_t bEnableAsserts;
    uint64_t *pu64Ts;
    uint64_t u64CurrentTs;
    uint64_t u64TsStep;
    uint64_t u64TsLastDequeue;
    uint32_t u32OuttoInRatio;
    pthread_mutex_t mutex;
};

struct flag_mgr {
    t_StVppQueue stQ;
    uint32_t *pu32Flags;
    uint32_t *pu32ExpectedBuffer;
    pthread_mutex_t mutex;
};

struct frame_mgr {
    t_StVppQueue stQ;
    uint32_t *pu32FrameType;
    pthread_mutex_t mutex;
};

struct test_ctx {
    t_StVppCtx stVppCtx;

    enum buf_pool_protection eProtection;

    void *pPrivateCtx;
    struct vpp_port_param port_param;
    t_StVppCallback cb;

    struct buf_pool_params params;
    uint32_t u32hqvCtrlSetFlag;
    t_StVppHqvCtrl hqvCtrl;

    pthread_cond_t cond;
    pthread_mutex_t mutex;

    uint32_t u32FramesOut;    // number of frames to generate

    uint32_t u32InRxCnt;        // number of IBD
    uint32_t u32OutRxCnt;       // number of OBD

    uint32_t u32InPutCnt;       // number of queue input bufs
    uint32_t u32OutPutCnt;      // number of queue output bufs

    uint32_t u32InHoldCnt;      // number of bufs held on input
    uint32_t u32OutHoldCnt;     // number of bufs held on output

    uint32_t u32InFlushCnt;     // number of flushed buffers on input
    uint32_t u32OutFlushCnt;    // number of flushed buffers on output

    uint32_t u32InEosRxCnt;     // number of buffers on input port with eos received
    uint32_t u32OutEosRxCnt;    // number of buffers on output port with eos received

    uint32_t u32Flags;

    struct buf_pool *buf_pool;
    struct buf_pool *buf_pool_ext;
};

typedef struct {
    uint32_t u32InRxCnt;        // number of IBD
    uint32_t u32OutRxCnt;       // number of OBD
    uint32_t u32InPutCnt;       // number of queue input bufs
    uint32_t u32OutPutCnt;      // number of queue output bufs
    uint32_t u32InHoldCnt;      // number of bufs held on input
    uint32_t u32OutHoldCnt;     // number of bufs held on output

    uint32_t u32OutFlushFlag;
    uint32_t u32ReconfigDoneFlag;
    uint32_t u32BypassFlag;
    uint32_t u32BypassStartCount;
    uint32_t u32BypassStopCount;
    uint32_t u32OutputFlushDoneFlag;
    uint32_t u32InputFlushDoneFlag;
    uint32_t u32ExpectedErrBufCnt;
    uint32_t u32ObdPending;
}t_StSessionParams;

typedef struct {
    t_StVppParam stNewParams;
    struct buf_pool *reconfig_pool;
    struct buf_pool_params reconfig_pool_params;
    struct vpp_port_param port_param;
    t_StVppUsecase *pstNewUc;
}t_StReconfig;

#define MAX_RECONFIG_STREAMS 5
#define MAX_SESSIONS 2
struct functional_test_ctx {
    uint32_t u32TestMask;
    uint32_t u32StreamCount;
    uint32_t u32hqvCtrlSetFlag;
    t_StVppHqvCtrl hqvCtrl;


    t_StSessionParams astSessionParams[MAX_SESSIONS];
    t_StReconfig reconfig_streams[MAX_RECONFIG_STREAMS];
};

/***************************************************************************
 * Function Prototypes
 ***************************************************************************/

uint32_t ts_mgr_init(struct ts_mgr *pTsMgr, enum ts_freq freq, uint32_t u32Cnt);
void ts_mgr_destroy(struct ts_mgr *pTsMgr);
uint64_t ts_get_next(struct ts_mgr *pTsMgr);
uint32_t ts_put_ts(struct ts_mgr *pTsMgr, uint64_t u64Ts);
uint32_t ts_get_freq(struct ts_mgr *pTsMgr);

uint32_t flag_mgr_init(struct flag_mgr *pFlagMgr, uint32_t u32Cnt);
void flag_mgr_destroy(struct flag_mgr *pFlagMgr);
uint32_t flag_queue(struct flag_mgr *pFlagMgr, uint32_t u32Flags, uint32_t u32ExpectedBuf);
uint32_t flag_dequeue(struct flag_mgr *pFlagMgr, uint32_t u32Flags, uint32_t u32Buf);

uint32_t frametype_mgr_init(struct frame_mgr *pFrameMgr, uint32_t u32Cnt);
void frametype_mgr_destroy(struct frame_mgr *pFrameMgr);
uint32_t frametype_queue(struct frame_mgr *pFrameMgr, uint32_t u32FrameType);
uint32_t frametype_dequeue(struct frame_mgr *pFrameMgr, uint32_t u32FrameType, uint32_t bIgnoreMatch);

uint32_t tctx_common_init(struct test_ctx *pCtx);
uint32_t tctx_common_destroy(struct test_ctx *pCtx);
uint32_t tctx_common_reset_cnt(struct test_ctx *pCtx);
void set_port_params(struct vpp_port_param *p,
                     uint32_t w, uint32_t h, enum vpp_color_format fmt);
void tctx_set_port_params(struct test_ctx *pCtx, uint32_t width,
                          uint32_t height, enum vpp_color_format fmt);

uint32_t get_session_flags(struct test_ctx *pCtx);

uint32_t u32GetMaxWidthForRes(t_EVppRes eRes);
uint32_t u32GetMaxHeightForRes(t_EVppRes eRes);
uint32_t u32GetMulFactor(struct vpp_port_param *pstParam, struct vpp_requirements *pstReq);

#ifdef __cplusplus
 }
#endif

#endif /* _TEST_UTILS_H_ */
