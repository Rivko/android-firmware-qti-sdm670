/*!
 * @file vpp_ip_frc_core.c
 *
 * @cr
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.

 * @services
 *
 *
 */

#define VPP_LOG_TAG     VPP_LOG_MODULE_FRC_CORE_TAG
#define VPP_LOG_MODULE  VPP_LOG_MODULE_FRC_CORE

#include <sys/types.h>
#include <sys/mman.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <inttypes.h>

#include "vpp.h"
#include "vpp_buf.h"
#include "vpp_callback.h"
#include "vpp_dbg.h"
#include "vpp_reg.h"
#include "vpp_utils.h"
#include "vpp_ip.h"
#include "vpp_stats.h"

#include "vpp_ip_frc_core.h"
#include "vpp_ip_frc_mc.h"
#include "vpp_ip_frc_me.h"

/************************************************************************
 * Local definitions
 ************************************************************************/

#define FRC_CMD_Q_SZ 10

typedef enum {
    VPP_IP_FRC_ME,
    VPP_IP_FRC_MC,
    VPP_IP_FRC_MAX
} t_EVppIpFrcBlk;

typedef struct {
    struct StVppIpFrcCb *pstCb;
    t_EVppIpFrcBlk eBlk;
} t_StVppIpFrcCbCtx;

typedef struct {
    t_StVppIonBuf stPx;
    t_StVppIonBuf stEx;
} t_StVppFrcMbiIonBuf;

typedef struct {
    struct StVppIpFrcCb *pstCb;
    t_EVppIpFrcBlk eBlk;

    pthread_t thread;

    t_StVppIpCmdQueue stCmdQ;

    uint32_t u32SubmodFlags;

    uint32_t u32Ret;
} t_StVppFrcSubmodule;

typedef struct StVppIpFrcCb {

    t_StVppIpBase stBase;

    sem_t sem;
    pthread_t thread;
    pthread_cond_t cond;
    pthread_mutex_t mutex;

    uint32_t eState;

    void *apvFrcCtx[VPP_IP_FRC_MAX];
    t_StVppIpFrcCbCtx astCbCtx[VPP_IP_FRC_MAX];

    t_StVppFrcSubmodule astSubmod[VPP_IP_FRC_MAX];

    t_StVppIpBufReq stMeInputReq;
    t_StVppIpBufReq stMeOutputReq;
    t_StVppIpBufReq stMcInputReq;
    t_StVppIpBufReq stMcOutputReq;

    /*!
     * MBI
     */
    t_StVppBufPool stMbiPool;
    t_StVppIpBufReq stMbiBufAlloc;

    // size of following is equal to stMbiPool.u32MinCnt
    t_StVppBuf *pstMbiIntBuf;
    struct vpp_buffer *pstVppExtBuf;
    t_StVppFrcMbiIonBuf *pstMbiIonBuf;

    // Buffer ownership counts
    uint32_t aaau32BlockBufCnt[VPP_IP_FRC_MAX]
                              [VPP_PORT_MAX]
                              [eVppBufPxDataType_Max];

    uint32_t u32FlushFlags;
    uint32_t u32DrainFlags;
    uint32_t u32FlushMcInCnt;
    uint32_t u32FlushMcOutCnt;
    uint32_t u32FlushMeInCnt;
    uint32_t u32FlushMeOutCnt;
    uint32_t u32DrainMcCnt;
    uint32_t u32DrainMeCnt;

    struct {
        STAT_DECL(InQCnt);
        STAT_DECL(OutQCnt);

        STAT_DECL(IBDCnt);
        STAT_DECL(OBDCnt);

        STAT_DECL(MeQInputYuv);
        STAT_DECL(MeQInputMbi);
        STAT_DECL(MeQOutputYuv);
        STAT_DECL(MeQOutputMbi);
        STAT_DECL(MeQInputYuvErr);
        STAT_DECL(MeQInputMbiErr);
        STAT_DECL(MeQOutputYuvErr);
        STAT_DECL(MeQOutputMbiErr);
        STAT_DECL(MeIBDYuv);
        STAT_DECL(MeIBDMbi);
        STAT_DECL(MeOBDYuv);
        STAT_DECL(MeOBDMbi);

        STAT_DECL(McQInputYuv);
        STAT_DECL(McQInputMbi);
        STAT_DECL(McQOutputYuv);
        STAT_DECL(McQOutputMbi);
        STAT_DECL(McQInputYuvErr);
        STAT_DECL(McQInputMbiErr);
        STAT_DECL(McQOutputYuvErr);
        STAT_DECL(McQOutputMbiErr);
        STAT_DECL(McIBDYuv);
        STAT_DECL(McIBDMbi);
        STAT_DECL(McOBDYuv);
        STAT_DECL(McOBDYuvFilled);
        STAT_DECL(McOBDYuvNotFilled);
        STAT_DECL(McOBDYuvFlushed);
        STAT_DECL(McOBDMbi);
    } stats;
} t_StVppIpFrcCb;

typedef enum {
    VPP_IP_FRC_BUF_CORE_IN,
    VPP_IP_FRC_BUF_CORE_OUT,
    VPP_IP_FRC_BUF_MC_YUV_IN,
    VPP_IP_FRC_BUF_MC_MBI_IN,
} t_EVppIpFrcDumpBuf;

enum {
    FRC_STAT_SUB_INIT,
    FRC_STAT_SUB_TERM,
    FRC_STAT_SUB_OPEN,
    FRC_STAT_SUB_CLOSE,
    FRC_STAT_SUB_RECONFIGURE,
    FRC_STAT_QBUF_INPUT,
    FRC_STAT_QBUF_OUTPUT,
    FRC_STAT_ME_OBD_DELTA,
    FRC_STAT_MC_OBD_DELTA,

    FRC_STAT_ME_INIT,
    FRC_STAT_ME_TERM,
    FRC_STAT_ME_OPEN,
    FRC_STAT_ME_CLOSE,
    FRC_STAT_ME_SET_PARAM,
    FRC_STAT_ME_SET_CTRL,
    FRC_STAT_ME_GET_BUF_REQ,
    FRC_STAT_ME_Q_BUF,
    FRC_STAT_ME_FLUSH,
    FRC_STAT_ME_DRAIN,
    FRC_STAT_ME_RECONFIGURE,

    FRC_STAT_MC_INIT,
    FRC_STAT_MC_TERM,
    FRC_STAT_MC_OPEN,
    FRC_STAT_MC_CLOSE,
    FRC_STAT_MC_SET_PARAM,
    FRC_STAT_MC_SET_CTRL,
    FRC_STAT_MC_GET_BUF_REQ,
    FRC_STAT_MC_Q_BUF,
    FRC_STAT_MC_FLUSH,
    FRC_STAT_MC_DRAIN,
    FRC_STAT_MC_RECONFIGURE,

    FRC_STAT_MBI_ALLOC,
    FRC_STAT_MBI_FREE,
};

#define VPP_FRC_DUMP_NM_LEN             256
#define VPP_FRC_DUMP_BUF_CORE_IN_NM     "frc_%p_core_in_%06d.yuv"
#define VPP_FRC_DUMP_BUF_CORE_OUT_NM    "frc_%p_core_out_%06d.yuv"
#define VPP_FRC_DUMP_BUF_MC_IN_YUV_NM   "frc_%p_mc_yuv_in_%06d.yuv"
#define VPP_FRC_DUMP_BUF_MC_IN_MBI_NM   "frc_%p_mc_mbi_in_%06d.mbi"

// #define VPP_FRC_DUMP_BUF_ENABLE
// #define VPP_FRC_ASSERT_FLUSH_FATAL

#ifdef VPP_FRC_CRC_BUF_ENABLE
#define VPP_FRC_CRC_BUF(_pstCb,_pBuf) u32Vpp_CrcBuffer(_pBuf, eVppBuf_Pixel,\
                                 (_pBuf->stPixel.u32FilledLen / 2),\
                                 (_pBuf->stPixel.u32FilledLen / 8),\
                                 _pstCb->stats.u32InQCnt, "VPP_FRC_CRC_BUF")
#else
#define VPP_FRC_CRC_BUF(_pstCb,_pBuf)
#endif

#define VPP_FRC_PORT_PARAM_WIDTH_MAX    RES_FHD_MAX_W
#define VPP_FRC_PORT_PARAM_WIDTH_MIN    324
#define VPP_FRC_PORT_PARAM_HEIGHT_MAX   RES_FHD_MAX_H
#define VPP_FRC_PORT_PARAM_HEIGHT_MIN   128

#define VPP_FRC_DRAIN_PENDING       (1 << 0)
#define VPP_FRC_DRAIN_PENDING_ME    (1 << 1)
#define VPP_FRC_DRAIN_PENDING_MC    (1 << 2)

#define VPP_FRC_FLUSH_INPUT         (1 << 0)
#define VPP_FRC_FLUSH_OUTPUT        (1 << 1)
#define VPP_FRC_FLUSH_MBI           (1 << 2)
#define VPP_FRC_FLUSH_ME_IN         (1 << 3)
#define VPP_FRC_FLUSH_ME_OUT        (1 << 4)
#define VPP_FRC_FLUSH_MC_IN         (1 << 5)
#define VPP_FRC_FLUSH_MC_OUT        (1 << 6)

#define VPP_FRC_FLUSH_PORTS_IN      (VPP_FRC_FLUSH_ME_IN|VPP_FRC_FLUSH_MC_IN)
#define VPP_FRC_FLUSH_PORTS_OUT     (VPP_FRC_FLUSH_MC_OUT)
#define VPP_FRC_FLUSH_PORTS_MBI     (VPP_FRC_FLUSH_ME_OUT|VPP_FRC_FLUSH_MC_IN)

#define FRC_CB_GET(ctx)         ((t_StVppIpFrcCb *)ctx)
#define FRC_CTX_GET(blk, ptr)   (((t_StVppIpFrcCb *)ptr)->apvFrcCtx[VPP_IP_FRC_##blk])

#define FRC_STATS(cb, stat)     (cb)->stats.u32##stat++
#define FRC_GET_STAT(cb, stat)  (cb)->stats.u32##stat

#define VPP_FRC_INIT(blk, cb, flags, cbs, ret) do { \
    VPP_IP_PROF_START(&cb->stBase, FRC_STAT_##blk##_INIT); \
    ret = stIpComp[VPP_IP_FRC_##blk].init(cb->stBase.pstCtx, flags, cbs); \
    VPP_IP_PROF_STOP(&cb->stBase, FRC_STAT_##blk##_INIT); \
} while (0)

#define VPP_FRC_TERM(blk, cb) do { \
    VPP_IP_PROF_START(&cb->stBase, FRC_STAT_##blk##_TERM); \
    stIpComp[VPP_IP_FRC_##blk].term(FRC_CTX_GET(blk, cb)); \
    VPP_IP_PROF_STOP(&cb->stBase, FRC_STAT_##blk##_TERM); \
} while (0)

#define VPP_FRC_OPEN(blk, cb, ret) do { \
    VPP_IP_PROF_START(&cb->stBase, FRC_STAT_##blk##_OPEN); \
    ret = stIpComp[VPP_IP_FRC_##blk].open(FRC_CTX_GET(blk, cb)); \
    VPP_IP_PROF_STOP(&cb->stBase, FRC_STAT_##blk##_OPEN); \
} while (0)

#define VPP_FRC_CLOSE(blk, cb, ret) do { \
    VPP_IP_PROF_START(&cb->stBase, FRC_STAT_##blk##_CLOSE); \
    ret = stIpComp[VPP_IP_FRC_##blk].close(FRC_CTX_GET(blk, cb)); \
    VPP_IP_PROF_STOP(&cb->stBase, FRC_STAT_##blk##_CLOSE); \
} while (0)

#define VPP_FRC_SET_PARAM(blk, cb, port, param, ret) do { \
    VPP_IP_PROF_START(&cb->stBase, FRC_STAT_##blk##_SET_PARAM); \
    ret = stIpComp[VPP_IP_FRC_##blk].set_param(FRC_CTX_GET(blk, cb), port, param); \
    VPP_IP_PROF_STOP(&cb->stBase, FRC_STAT_##blk##_SET_PARAM); \
} while (0)

#define VPP_FRC_SET_CTRL(blk, cb, ctrl, ret) do { \
    VPP_IP_PROF_START(&cb->stBase, FRC_STAT_##blk##_SET_CTRL); \
    ret = stIpComp[VPP_IP_FRC_##blk].set_ctrl(FRC_CTX_GET(blk, cb), ctrl); \
    VPP_IP_PROF_STOP(&cb->stBase, FRC_STAT_##blk##_SET_CTRL); \
} while (0)

#define VPP_FRC_GET_BUF_REQ(blk, cb, in, out, ret) do { \
    VPP_IP_PROF_START(&cb->stBase, FRC_STAT_##blk##_GET_BUF_REQ); \
    ret = stIpComp[VPP_IP_FRC_##blk].get_buffer_requirements(FRC_CTX_GET(blk, cb), in, out); \
    VPP_IP_PROF_STOP(&cb->stBase, FRC_STAT_##blk##_GET_BUF_REQ); \
} while (0)

#define VPP_FRC_QUEUE_BUF(blk, cb, port, buf, ret) do { \
    VPP_IP_PROF_START(&cb->stBase, FRC_STAT_##blk##_Q_BUF); \
    ret = stIpComp[VPP_IP_FRC_##blk].queue_buf(FRC_CTX_GET(blk, cb), port, buf); \
    VPP_IP_PROF_STOP(&cb->stBase, FRC_STAT_##blk##_Q_BUF); \
} while (0)

#define VPP_FRC_FLUSH(blk, cb, port, ret) do { \
    VPP_IP_PROF_START(&cb->stBase, FRC_STAT_##blk##_FLUSH); \
    ret = stIpComp[VPP_IP_FRC_##blk].flush(FRC_CTX_GET(blk, cb), port); \
    VPP_IP_PROF_STOP(&cb->stBase, FRC_STAT_##blk##_FLUSH); \
} while (0)

#define VPP_FRC_DRAIN(blk, cb, ret) do { \
    VPP_IP_PROF_START(&cb->stBase, FRC_STAT_##blk##_DRAIN); \
    ret = stIpComp[VPP_IP_FRC_##blk].drain(FRC_CTX_GET(blk, cb)); \
    VPP_IP_PROF_STOP(&cb->stBase, FRC_STAT_##blk##_DRAIN); \
} while (0)

#define VPP_FRC_RECONFIGURE(blk, cb, in, out, ret) do { \
    VPP_IP_PROF_START(&cb->stBase, FRC_STAT_##blk##_RECONFIGURE); \
    ret = stIpComp[VPP_IP_FRC_##blk].reconfigure(FRC_CTX_GET(blk, cb), in, out); \
    VPP_IP_PROF_STOP(&cb->stBase, FRC_STAT_##blk##_RECONFIGURE); \
} while (0)

#define FRC_GET_BLK_STR(blk) \
    (blk == VPP_IP_FRC_ME ? "ME" : \
    blk == VPP_IP_FRC_MC ? "MC" : "invalid_block")
#define FRC_GET_BUF_TYPE(buf) \
    (buf == eVppBufPxDataType_Raw ? "YUV" : \
     buf == eVppBufPxDataType_Compressed ? "MBI" : "INVALID")

#define DUMP_BUF_OWNERSHIP(cb) \
    LOGD("buffer ownership: ME: [P:%u, M:%u] MC: [Pi:%u, Po:%u, M:%u]", \
         cb->aaau32BlockBufCnt[VPP_IP_FRC_ME][VPP_PORT_INPUT][eVppBufPxDataType_Raw], \
         cb->aaau32BlockBufCnt[VPP_IP_FRC_ME][VPP_PORT_OUTPUT][eVppBufPxDataType_Compressed], \
         cb->aaau32BlockBufCnt[VPP_IP_FRC_MC][VPP_PORT_INPUT][eVppBufPxDataType_Raw], \
         cb->aaau32BlockBufCnt[VPP_IP_FRC_MC][VPP_PORT_OUTPUT][eVppBufPxDataType_Raw], \
         cb->aaau32BlockBufCnt[VPP_IP_FRC_MC][VPP_PORT_INPUT][eVppBufPxDataType_Compressed]);

#define LOG_BUF_COMMON(pb)      pb, pb->pBuf->timestamp
#define LOG_BUF_EXT(pb, field)  pb->pBuf->field.fd, pb->pBuf->field.offset, pb->pBuf->field.alloc_len, pb->pBuf->field.filled_len, pb->pBuf->flags
#define LOG_BUF_INT(pb, field)  pb->field.pvBase, pb->field.u32MappedLen, pb->u32InternalFlags
#define LOG_BUF_STR             "pstBuf=%p, ts=%"PRIu64", ext={fd=%d, offset=%u, alloc_len=%u, filled_len=%u, flags=0x%x}, int={pvBase=%p, u32MappedLen=%u, flags=0x%x}"
#define LOG_BUF_PX(pb)          LOGI("px->: " LOG_BUF_STR, LOG_BUF_COMMON(pb), LOG_BUF_EXT(pb, pixel), LOG_BUF_INT(pb, stPixel))
#define LOG_BUF_EX(pb)          LOGI("ex->: " LOG_BUF_STR, LOG_BUF_COMMON(pb), LOG_BUF_EXT(pb, extradata), LOG_BUF_INT(pb, stExtra))

#define LOG_BUF_CB(_cbevt, _blk, _buftype, _action) \
    LOGD("BufRx: [%s] %s (%s): %s", #_blk, #_cbevt, #_buftype, _action)

/************************************************************************
 * Local static variables
 ************************************************************************/
static t_StVppIpFnc stIpComp[VPP_IP_FRC_MAX] = {
    [VPP_IP_FRC_ME] = {
        .init = vpVppIpFrcMe_Init,
        .term = vVppIpFrcMe_Term,
        .open = u32VppIpFrcMe_Open,
        .close = u32VppIpFrcMe_Close,
        .set_param = u32VppIpFrcMe_SetParam,
        .set_ctrl = u32VppIpFrcMe_SetCtrl,
        .get_buffer_requirements = u32VppIpFrcMe_GetBufferRequirements,
        .queue_buf = u32VppIpFrcMe_QueueBuf,
        .flush = u32VppIpFrcMe_Flush,
        .drain = u32VppIpFrcMe_Drain,
        .reconfigure = u32VppIpFrcMe_Reconfigure,
    },
    [VPP_IP_FRC_MC] = {
        .init = vpVppIpFrcMc_Init,
        .term = vVppIpFrcMc_Term,
        .open = u32VppIpFrcMc_Open,
        .close = u32VppIpFrcMc_Close,
        .set_param = u32VppIpFrcMc_SetParam,
        .set_ctrl = u32VppIpFrcMc_SetCtrl,
        .get_buffer_requirements = u32VppIpFrcMc_GetBufferRequirements,
        .queue_buf = u32VppIpFrcMc_QueueBuf,
        .flush = u32VppIpFrcMc_Flush,
        .drain = u32VppIpFrcMc_Drain,
        .reconfigure = u32VppIpFrcMc_Reconfigure,
    },
};

static const t_StVppStatsConfig astFrcStatsCfg[] = {
    VPP_PROF_DECL(FRC_STAT_SUB_INIT, 1, 1),
    VPP_PROF_DECL(FRC_STAT_SUB_TERM, 1, 1),
    VPP_PROF_DECL(FRC_STAT_SUB_OPEN, 1, 1),
    VPP_PROF_DECL(FRC_STAT_SUB_CLOSE, 1, 1),
    VPP_PROF_DECL(FRC_STAT_QBUF_INPUT, 1, 1),
    VPP_PROF_DECL(FRC_STAT_QBUF_OUTPUT, 1, 1),
    VPP_PROF_DECL(FRC_STAT_ME_OBD_DELTA, 1, 1),
    VPP_PROF_DECL(FRC_STAT_MC_OBD_DELTA, 1, 1),

    VPP_PROF_DECL(FRC_STAT_ME_INIT, 1, 1),
    VPP_PROF_DECL(FRC_STAT_ME_TERM, 1, 1),
    VPP_PROF_DECL(FRC_STAT_ME_OPEN, 1, 1),
    VPP_PROF_DECL(FRC_STAT_ME_CLOSE, 1, 1),
    VPP_PROF_DECL(FRC_STAT_ME_SET_PARAM, 1, 1),
    VPP_PROF_DECL(FRC_STAT_ME_SET_CTRL, 1, 1),
    VPP_PROF_DECL(FRC_STAT_ME_GET_BUF_REQ, 1, 1),
    VPP_PROF_DECL(FRC_STAT_ME_Q_BUF, 1, 1),
    VPP_PROF_DECL(FRC_STAT_ME_FLUSH, 1, 1),
    VPP_PROF_DECL(FRC_STAT_ME_DRAIN, 1, 1),

    VPP_PROF_DECL(FRC_STAT_MC_INIT, 1, 1),
    VPP_PROF_DECL(FRC_STAT_MC_TERM, 1, 1),
    VPP_PROF_DECL(FRC_STAT_MC_OPEN, 1, 1),
    VPP_PROF_DECL(FRC_STAT_MC_CLOSE, 1, 1),
    VPP_PROF_DECL(FRC_STAT_MC_SET_PARAM, 1, 1),
    VPP_PROF_DECL(FRC_STAT_MC_SET_CTRL, 1, 1),
    VPP_PROF_DECL(FRC_STAT_MC_GET_BUF_REQ, 1, 1),
    VPP_PROF_DECL(FRC_STAT_MC_Q_BUF, 1, 1),
    VPP_PROF_DECL(FRC_STAT_MC_FLUSH, 1, 1),
    VPP_PROF_DECL(FRC_STAT_MC_DRAIN, 1, 1),

    VPP_PROF_DECL(FRC_STAT_MBI_ALLOC, 1, 1),
    VPP_PROF_DECL(FRC_STAT_MBI_FREE, 1, 1),
};

static const uint32_t u32FrcStatCount = VPP_STATS_CNT(astFrcStatsCfg);
/************************************************************************
 * Forward Declarations
 *************************************************************************/

/************************************************************************
 * Local Functions
 ************************************************************************/
#define VPP_FRC_DUMP_STAT(_pstCb, _name) \
    LOGD("%-17s: %u", #_name, _pstCb->stats.u32##_name)
static void vVppIpFrc_DumpStats(t_StVppIpFrcCb *pstCb)
{
    VPP_RET_VOID_IF_NULL(pstCb);

    VPP_FRC_DUMP_STAT(pstCb, InQCnt);
    VPP_FRC_DUMP_STAT(pstCb, OutQCnt);

    VPP_FRC_DUMP_STAT(pstCb, IBDCnt);
    VPP_FRC_DUMP_STAT(pstCb, OBDCnt);

    VPP_FRC_DUMP_STAT(pstCb, MeQInputYuv);
    VPP_FRC_DUMP_STAT(pstCb, MeQInputMbi);
    VPP_FRC_DUMP_STAT(pstCb, MeQOutputYuv);
    VPP_FRC_DUMP_STAT(pstCb, MeQOutputMbi);
    VPP_FRC_DUMP_STAT(pstCb, MeQInputYuvErr);
    VPP_FRC_DUMP_STAT(pstCb, MeQInputMbiErr);
    VPP_FRC_DUMP_STAT(pstCb, MeQOutputYuvErr);
    VPP_FRC_DUMP_STAT(pstCb, MeQOutputMbiErr);
    VPP_FRC_DUMP_STAT(pstCb, MeIBDYuv);
    VPP_FRC_DUMP_STAT(pstCb, MeIBDMbi);
    VPP_FRC_DUMP_STAT(pstCb, MeOBDYuv);
    VPP_FRC_DUMP_STAT(pstCb, MeOBDMbi);

    VPP_FRC_DUMP_STAT(pstCb, McQInputYuv);
    VPP_FRC_DUMP_STAT(pstCb, McQInputMbi);
    VPP_FRC_DUMP_STAT(pstCb, McQOutputYuv);
    VPP_FRC_DUMP_STAT(pstCb, McQOutputMbi);
    VPP_FRC_DUMP_STAT(pstCb, McQInputYuvErr);
    VPP_FRC_DUMP_STAT(pstCb, McQInputMbiErr);
    VPP_FRC_DUMP_STAT(pstCb, McQOutputYuvErr);
    VPP_FRC_DUMP_STAT(pstCb, McQOutputMbiErr);
    VPP_FRC_DUMP_STAT(pstCb, McIBDYuv);
    VPP_FRC_DUMP_STAT(pstCb, McIBDMbi);
    VPP_FRC_DUMP_STAT(pstCb, McOBDYuv);
    VPP_FRC_DUMP_STAT(pstCb, McOBDYuvFilled);
    VPP_FRC_DUMP_STAT(pstCb, McOBDYuvNotFilled);
    VPP_FRC_DUMP_STAT(pstCb, McOBDYuvFlushed);
    VPP_FRC_DUMP_STAT(pstCb, McOBDMbi);
}

static void vVppIpFrc_DumpBuffer(t_StVppIpFrcCb *pstCb,
                                 t_StVppBuf *pstBuf,
                                 t_EVppIpFrcDumpBuf eBuf)
{
    char cPath[VPP_FRC_DUMP_NM_LEN];
    struct vpp_port_param *pstParam;

    VPP_RET_VOID_IF_NULL(pstCb);
    VPP_RET_VOID_IF_NULL(pstBuf);

    if (pstCb->stBase.bSecureSession)
        return;

    if (VPP_FLAG_IS_SET(pstBuf->u32InternalFlags, VPP_BUF_FLAG_FLUSHED))
        return;

    LOG_ENTER_ARGS("eBuf=%u", eBuf);
#ifndef VPP_FRC_DUMP_BUF_ENABLE
    {
        uint32_t u32BufDumpEnable = 0;
        u32VppUtils_GetBufDumpEnable(pstCb->stBase.pstCtx, &u32BufDumpEnable);
        if (!(((eBuf == VPP_IP_FRC_BUF_CORE_IN) &&
            (VPP_FLAG_IS_SET(u32BufDumpEnable, VPP_BUFDUMP_INPUT))) ||
            ((eBuf == VPP_IP_FRC_BUF_CORE_OUT) &&
            (VPP_FLAG_IS_SET(u32BufDumpEnable, VPP_BUFDUMP_OUTPUT))) ||
            ((eBuf == VPP_IP_FRC_BUF_MC_MBI_IN) &&
            (VPP_FLAG_IS_SET(u32BufDumpEnable, VPP_BUFDUMP_COMPRESSED)))))
            return;
    }
#endif

    switch (eBuf)
    {
        case VPP_IP_FRC_BUF_CORE_IN:
            snprintf(cPath, VPP_FRC_DUMP_NM_LEN, VPP_FRC_DUMP_BUF_CORE_IN_NM,
                     pstCb, pstCb->stats.u32InQCnt);
            pstParam = &pstCb->stBase.stInputParam;
            break;
        case VPP_IP_FRC_BUF_CORE_OUT:
            snprintf(cPath, VPP_FRC_DUMP_NM_LEN, VPP_FRC_DUMP_BUF_CORE_OUT_NM,
                     pstCb, pstCb->stats.u32OBDCnt);
            pstParam = &pstCb->stBase.stOutputParam;
            break;
        case VPP_IP_FRC_BUF_MC_YUV_IN:
            snprintf(cPath, VPP_FRC_DUMP_NM_LEN, VPP_FRC_DUMP_BUF_MC_IN_YUV_NM,
                     pstCb, pstCb->stats.u32McQInputYuv);
            pstParam = &pstCb->stBase.stInputParam;
            break;
        case VPP_IP_FRC_BUF_MC_MBI_IN:
            snprintf(cPath, VPP_FRC_DUMP_NM_LEN, VPP_FRC_DUMP_BUF_MC_IN_MBI_NM,
                     pstCb, pstCb->stats.u32McQInputMbi);
            break;
        default:
            return;
    }

    if (eBuf == VPP_IP_FRC_BUF_MC_MBI_IN)
    {
        u32VppBuf_DumpRawData(pstCb->stBase.pstCtx, pstBuf->stPixel.pvBase,
                              pstBuf->stPixel.u32FilledLen, cPath);
    }
    else
    {
        u32VppBuf_Dump(pstCb->stBase.pstCtx, pstBuf, *pstParam, cPath);
    }

    LOG_EXIT();
}

static t_StVppIpFrcCb *pstVppIpFrc_GetCbFromCallback(void *pv,
                                                     t_EVppIpFrcBlk *peBlk)
{
    t_StVppIpFrcCbCtx *pstCbCtx = (t_StVppIpFrcCbCtx *)pv;

    if (!pstCbCtx || !peBlk)
    {
        LOGE("Invalid arguments, pstCbCtx=%p, peBlk=%p", pstCbCtx, peBlk);
        return NULL;
    }

    *peBlk = pstCbCtx->eBlk;
    return pstCbCtx->pstCb;
}


static uint32_t *pu32VppIpFrc_GetOwnershipCounter(t_StVppIpFrcCb *pstCb,
                                                  t_EVppIpFrcBlk eBlk,
                                                  t_StVppBuf *pstBuf)
{
    enum vpp_port ePort;

    if (!pstCb || !pstBuf)
        return NULL;

    if (eBlk >= VPP_IP_FRC_MAX)
        return NULL;

    if (pstBuf->eBufPxType >= eVppBufPxDataType_Max)
        return NULL;

    if (pstBuf->eBufPxType == eVppBufPxDataType_Compressed)
        ePort = eBlk == VPP_IP_FRC_ME ? VPP_PORT_OUTPUT : VPP_PORT_INPUT;
    else
        ePort = pstBuf->eCurrentIpPort;

    return &pstCb->aaau32BlockBufCnt[eBlk][ePort][pstBuf->eBufPxType];
}

static uint32_t u32VppIpFrc_GetBufferHoldCnt(t_StVppIpFrcCb *pstCb,
                                             t_EVppIpFrcBlk eBlk,
                                             enum vpp_port ePort,
                                             t_EVppBufPxDataType eBufType)
{
    // Mutex guarding these counters must be locked prior to calling this
    // function

    uint32_t u32Cnt = 0;

    if (!pstCb ||
        eBlk >= VPP_IP_FRC_MAX ||
        ePort >= VPP_PORT_MAX ||
        eBufType >= eVppBufPxDataType_Max)
        return 0;

    return pstCb->aaau32BlockBufCnt[eBlk][ePort][eBufType];
}

static uint32_t u32VppIpFrc_QueueBufInternal(t_StVppIpFrcCb *pstCb,
                                             t_EVppIpFrcBlk eBlk,
                                             enum vpp_port ePort,
                                             t_StVppBuf *pstBuf,
                                             uint32_t bMutexLocked)
{
    uint32_t u32;
    uint32_t *pu32Counter = NULL;

    if (!pstCb || !pstBuf)
        return VPP_ERR_PARAM;

    if (ePort != VPP_PORT_INPUT && ePort != VPP_PORT_OUTPUT)
        return VPP_ERR_PARAM;
    pu32Counter = pu32VppIpFrc_GetOwnershipCounter(pstCb, eBlk, pstBuf);

    if (!bMutexLocked)
        pthread_mutex_lock(&pstCb->mutex);

    VPP_FLAG_CLR(pstBuf->u32InternalFlags, VPP_BUF_FLAG_FLUSHED);

    if (eBlk == VPP_IP_FRC_ME)
    {
        if (pu32Counter)
            (*pu32Counter) += 1;

        if (ePort == VPP_PORT_OUTPUT)
            VPP_FLAG_CLR(pstBuf->pBuf->flags, VPP_BUFFER_FLAG_EOS);

        VPP_FRC_QUEUE_BUF(ME, pstCb, ePort, pstBuf, u32);

        if (u32 != VPP_OK)
        {
            if (pu32Counter)
                (*pu32Counter) -= 1;

            LOGE("unable to queue to ME, pstBuf=%p, ePort=%u, u32=%u",
                 pstBuf, ePort, u32);
        }
    }
    else if (eBlk == VPP_IP_FRC_MC)
    {
        if (pu32Counter)
            (*pu32Counter) += 1;

        VPP_FRC_QUEUE_BUF(MC, pstCb, ePort, pstBuf, u32);

        if (u32 != VPP_OK)
        {
            if (pu32Counter)
                (*pu32Counter) -= 1;
            LOGE("unable to queue to MC, pstBuf=%p, ePort=%u, u32=%u",
                 pstBuf, ePort, u32);
        }
    }
    else
    {
        u32 = VPP_ERR_PARAM;
    }

    DUMP_BUF_OWNERSHIP(pstCb);

    if (!bMutexLocked)
        pthread_mutex_unlock(&pstCb->mutex);

    return u32;
}


static void vVppIpFrc_UpdateOwnedBuffers(t_StVppIpFrcCb *pstCb,
                                         t_EVppIpFrcBlk eBlk,
                                         t_StVppBuf *pstBuf)
{
    uint32_t *pu32Counter;

    if (!pstCb || !pstBuf)
        return;

    if (eBlk != VPP_IP_FRC_MC && eBlk != VPP_IP_FRC_ME)
        return;

    pu32Counter = pu32VppIpFrc_GetOwnershipCounter(pstCb, eBlk, pstBuf);
    if (pu32Counter)
    {
        pthread_mutex_lock(&pstCb->mutex);
        (*pu32Counter) -= 1;
        DUMP_BUF_OWNERSHIP(pstCb);
        pthread_mutex_unlock(&pstCb->mutex);
    }
}

static uint32_t u32VppIpFrc_ValidatePortParam(struct vpp_port_param stParam)
{
    if (stParam.fmt != VPP_COLOR_FORMAT_UBWC_NV12)
    {
        LOGE("validation failed: fmt=%u", stParam.fmt);
        return VPP_ERR;
    }
    if (stParam.width < VPP_FRC_PORT_PARAM_WIDTH_MIN ||
        stParam.width > VPP_FRC_PORT_PARAM_WIDTH_MAX)
    {
        LOGE("validation failed: width=%u, min=%u, max=%u", stParam.width,
             VPP_FRC_PORT_PARAM_WIDTH_MIN, VPP_FRC_PORT_PARAM_WIDTH_MAX);
        return VPP_ERR;
    }
    if (stParam.height < VPP_FRC_PORT_PARAM_HEIGHT_MIN ||
        stParam.height > VPP_FRC_PORT_PARAM_HEIGHT_MAX)
    {
        LOGE("validation failed: height=%u, min=%u, max=%u", stParam.height,
             VPP_FRC_PORT_PARAM_HEIGHT_MIN, VPP_FRC_PORT_PARAM_HEIGHT_MAX);
        return VPP_ERR;
    }
    return VPP_OK;
}

static uint32_t u32VppIpFrc_ValidateConfig(struct vpp_port_param stInput,
                                           struct vpp_port_param stOutput)
{
    if (stInput.height != stOutput.height)
    {
        LOGD("validation failed: height, input: %u, output: %u",
             stInput.height, stOutput.height);
        return VPP_ERR;
    }
    if (stInput.width != stOutput.width)
    {

        LOGD("validation failed: width, input: %u, output: %u",
             stInput.width, stOutput.width);
        return VPP_ERR;
    }
    if (stInput.fmt != stOutput.fmt)
    {
        LOGD("validation failed: fmt, input: %u, output: %u",
             stInput.fmt, stOutput.fmt);
        return VPP_ERR;
    }
    if (u32VppIpFrc_ValidatePortParam(stInput))
    {
        LOGE("u32VppIpFrc_ValidatePortParam(INPUT) failed");
        return VPP_ERR;
    }
    if (u32VppIpFrc_ValidatePortParam(stOutput))
    {
        LOGE("u32VppIpFrc_ValidatePortParam(OUTPUT) failed");
        return VPP_ERR;
    }

    return VPP_OK;
}

static void vVppIpFrc_HandleMeIbd(t_StVppIpFrcCb *pstCb, t_StVppBuf *pstBuf)
{
    uint32_t u32;
    uint32_t bIssueIbd = VPP_FALSE;
    uint32_t bStoreMbi = VPP_FALSE;

    if (!pstCb || !pstBuf)
        return;

    if (pstBuf->eBufPxType == eVppBufPxDataType_Raw)
    {
        FRC_STATS(pstCb, MeIBDYuv);

        pthread_mutex_lock(&pstCb->mutex);

        if (VPP_FLAG_IS_SET(pstCb->u32FlushFlags, VPP_FRC_FLUSH_INPUT) ||
            VPP_FLAG_IS_SET(pstBuf->u32InternalFlags, VPP_BUF_FLAG_FLUSHED))
        {
            LOG_BUF_CB(IBD, ME, YUV, "issuing IBD (flush input)");
            VPP_FLAG_SET(pstBuf->u32InternalFlags, VPP_BUF_FLAG_FLUSHED);
            bIssueIbd = VPP_TRUE;
        }
        else
        {
            LOG_BUF_CB(IBD, ME, YUV, "queue to MC(INPUT)");
            vVppIpFrc_DumpBuffer(pstCb, pstBuf, VPP_IP_FRC_BUF_MC_YUV_IN);
            u32 = u32VppIpFrc_QueueBufInternal(pstCb, VPP_IP_FRC_MC,
                                               VPP_PORT_INPUT, pstBuf,
                                               VPP_TRUE);
            if (u32 != VPP_OK)
            {
                LOGE("unable to queue yuv to mc input, issuing IBD to client "
                     "u32=%u", u32);
                FRC_STATS(pstCb, McQInputYuvErr);
                bIssueIbd = VPP_TRUE;
            }
            else
            {
                FRC_STATS(pstCb, McQInputYuv);
            }
        }

        if (bIssueIbd)
        {
            u32VppIp_CbBufDone(&pstCb->stBase.stCb, VPP_PORT_INPUT, pstBuf);
            FRC_STATS(pstCb, IBDCnt);
        }

        pthread_mutex_unlock(&pstCb->mutex);
    }
    else if (pstBuf->eBufPxType == eVppBufPxDataType_Compressed)
    {
        FRC_STATS(pstCb, MeIBDMbi);

        pthread_mutex_lock(&pstCb->mutex);

        if (VPP_FLAG_IS_SET(pstCb->u32FlushFlags, VPP_FRC_FLUSH_MBI))
        {
            bStoreMbi = VPP_TRUE;
            LOG_BUF_CB(IBD, ME, MBI, "[ERROR] storing MBI (flush mbi)");
        }
        else
        {
            LOG_BUF_CB(IBD, ME, MBI, "[ERROR] queue to ME(OUTPUT)");
            u32 = u32VppIpFrc_QueueBufInternal(pstCb, VPP_IP_FRC_ME,
                                               VPP_PORT_OUTPUT, pstBuf,
                                               VPP_TRUE);
            if (u32 != VPP_OK)
            {
                LOGE("unable to queue yuv to me output, returning to "
                     "internal pool, u32=%u", u32);
                FRC_STATS(pstCb, MeQOutputMbiErr);
                bStoreMbi = VPP_TRUE;
            }
            else
            {
                FRC_STATS(pstCb, MeQOutputMbi);
            }
        }

        if (bStoreMbi)
        {
            u32 = u32VppBufPool_Put(&pstCb->stMbiPool, pstBuf);
            if (u32 != VPP_OK)
            {
                LOGE("unable to put back in mbi pool... this is very bad, "
                     "u32=%u", u32);
            }
        }

        pthread_mutex_unlock(&pstCb->mutex);
    }
}

static void vVppIpFrc_HandleMcIbd(t_StVppIpFrcCb *pstCb, t_StVppBuf *pstBuf)
{
    uint32_t u32;
    uint32_t bStoreMbi = VPP_FALSE;

    if (!pstCb || !pstBuf)
        return;

    if (pstBuf->eBufPxType == eVppBufPxDataType_Raw)
    {
        FRC_STATS(pstCb, McIBDYuv);
        LOG_BUF_CB(IBD, MC, YUV, "issuing IBD to client");

        u32VppIp_CbBufDone(&pstCb->stBase.stCb, VPP_PORT_INPUT, pstBuf);
        FRC_STATS(pstCb, IBDCnt);
    }
    else if (pstBuf->eBufPxType == eVppBufPxDataType_Compressed)
    {
        FRC_STATS(pstCb, McIBDMbi);

        pthread_mutex_lock(&pstCb->mutex);

        if (VPP_FLAG_IS_SET(pstCb->u32FlushFlags, VPP_FRC_FLUSH_MBI))
        {
            bStoreMbi = VPP_TRUE;
            LOG_BUF_CB(IBD, MC, MBI, "storing MBI (flush mbi)");
        }
        else
        {
            LOG_BUF_CB(IBD, MC, MBI, "queue to ME(OUTPUT)");
            u32 = u32VppIpFrc_QueueBufInternal(pstCb, VPP_IP_FRC_ME,
                                               VPP_PORT_OUTPUT, pstBuf,
                                               VPP_TRUE);
            if (u32 != VPP_OK)
            {
                bStoreMbi = VPP_TRUE;
                FRC_STATS(pstCb, MeQOutputMbiErr);
                LOGE("unable to queue yuv to me output, returning to "
                     "internal pool, u32=%u", u32);
            }
            else
            {
                FRC_STATS(pstCb, MeQOutputMbi);
            }
        }

        if (bStoreMbi)
        {
            u32 = u32VppBufPool_Put(&pstCb->stMbiPool, pstBuf);
            if (u32 != VPP_OK)
            {
                LOGE("unable to put back in mbi pool... this is very bad, "
                     "u32=%u", u32);
            }
        }

        pthread_mutex_unlock(&pstCb->mutex);
    }
}

static void vVppIpFrc_HandleMeObd(t_StVppIpFrcCb *pstCb, t_StVppBuf *pstBuf)
{
    uint32_t u32;
    uint32_t bStoreMbi = VPP_FALSE;
    t_EVppIpFrcBlk eMbiDstBlk = VPP_IP_FRC_MAX;

    if (!pstCb || !pstBuf)
        return;

    if (pstBuf->eBufPxType == eVppBufPxDataType_Raw)
    {
        FRC_STATS(pstCb, MeOBDYuv);
        LOG_BUF_CB(OBD, ME, YUV, "[ERROR] issuing IBD to client");

        u32VppIp_CbBufDone(&pstCb->stBase.stCb, VPP_PORT_INPUT, pstBuf);
        FRC_STATS(pstCb, IBDCnt);
    }
    else if (pstBuf->eBufPxType == eVppBufPxDataType_Compressed)
    {
        FRC_STATS(pstCb, MeOBDMbi);

        pthread_mutex_lock(&pstCb->mutex);

        if (VPP_FLAG_IS_SET(pstCb->u32FlushFlags, VPP_FRC_FLUSH_MBI))
        {
            bStoreMbi = VPP_TRUE;
            LOG_BUF_CB(OBD, ME, MBI, "storing MBI (flush MBI)");
        }
        else if (VPP_FLAG_IS_SET(pstCb->u32FlushFlags, VPP_FRC_FLUSH_INPUT))
        {
            eMbiDstBlk = VPP_IP_FRC_ME;
            LOG_BUF_CB(OBD, ME, MBI, "queue to ME(OUTPUT) (flush input)");
        }
        else
        {
            if (pstBuf->stPixel.u32FilledLen)
            {
                eMbiDstBlk = VPP_IP_FRC_MC;

                LOG_BUF_CB(OBD, ME, MBI, "queue to MC(INPUT) u32FilledLen != 0");
            }
            else
            {
                eMbiDstBlk = VPP_IP_FRC_ME;
                LOG_BUF_CB(OBD, ME, MBI, "queue to ME(OUTPUT) u32FilledLen == 0");
            }
        }

        if (!bStoreMbi)
        {
            if (eMbiDstBlk == VPP_IP_FRC_ME)
            {
                u32 = u32VppIpFrc_QueueBufInternal(pstCb, VPP_IP_FRC_ME,
                                                   VPP_PORT_OUTPUT, pstBuf,
                                                   VPP_TRUE);
                if (u32 != VPP_OK)
                {
                    bStoreMbi = VPP_TRUE;
                    FRC_STATS(pstCb, MeQOutputMbiErr);
                    LOGE("unable to queue mbi to me output, u32=%u", u32);
                }
                else
                {
                    FRC_STATS(pstCb, MeQOutputMbi);
                }
            }
            else if (eMbiDstBlk == VPP_IP_FRC_MC)
            {
                vVppIpFrc_DumpBuffer(pstCb, pstBuf, VPP_IP_FRC_BUF_MC_MBI_IN);
                if (FRC_GET_STAT(pstCb, McQInputMbi))
                    VPP_IP_PROF_STOP(&pstCb->stBase, FRC_STAT_ME_OBD_DELTA);
                VPP_IP_PROF_START(&pstCb->stBase, FRC_STAT_ME_OBD_DELTA);

                u32 = u32VppIpFrc_QueueBufInternal(pstCb, VPP_IP_FRC_MC,
                                                   VPP_PORT_INPUT, pstBuf,
                                                   VPP_TRUE);
                if (u32 != VPP_OK)
                {
                    bStoreMbi = VPP_TRUE;
                    FRC_STATS(pstCb, McQInputMbiErr);
                    LOGE("unable to queue mbi to mc input, u32=%u", u32);
                }
                else
                {
                    FRC_STATS(pstCb, McQInputMbi);
                }
            }
            else
            {
                bStoreMbi = VPP_TRUE;
            }
        }

        if (bStoreMbi)
        {
            u32 = u32VppBufPool_Put(&pstCb->stMbiPool, pstBuf);
            if (u32 != VPP_OK)
            {
                LOGE("unable to put back in mbi pool... this is very bad, "
                     "u32=%u", u32);
            }
        }

        pthread_mutex_unlock(&pstCb->mutex);
    }
}

static void vVppIpFrc_HandleMcObd(t_StVppIpFrcCb *pstCb, t_StVppBuf *pstBuf)
{
    uint32_t u32;
    uint32_t bStoreMbi = VPP_FALSE;

    if (!pstCb || !pstBuf)
        return;

    if (pstBuf->eBufPxType == eVppBufPxDataType_Raw)
    {
        FRC_STATS(pstCb, McOBDYuv);

        if (pstBuf->stPixel.u32FilledLen)
        {
            vVppIpFrc_DumpBuffer(pstCb, pstBuf, VPP_IP_FRC_BUF_CORE_OUT);
            if (pstCb->stats.u32McOBDYuvFilled)
                VPP_IP_PROF_STOP(&pstCb->stBase, FRC_STAT_MC_OBD_DELTA);
            VPP_IP_PROF_START(&pstCb->stBase, FRC_STAT_MC_OBD_DELTA);

            FRC_STATS(pstCb, McOBDYuvFilled);
        }
        else
            FRC_STATS(pstCb, McOBDYuvNotFilled);

        if (VPP_FLAG_IS_SET(pstBuf->u32InternalFlags, VPP_BUF_FLAG_FLUSHED))
            FRC_STATS(pstCb, McOBDYuvFlushed);

        LOG_BUF_CB(OBD, MC, YUV, "issuing OBD to client");
        u32VppIp_CbBufDone(&pstCb->stBase.stCb, VPP_PORT_OUTPUT, pstBuf);
        FRC_STATS(pstCb, OBDCnt);
    }
    else if (pstBuf->eBufPxType == eVppBufPxDataType_Compressed)
    {
        FRC_STATS(pstCb, McOBDMbi);

        pthread_mutex_lock(&pstCb->mutex);

        if (VPP_FLAG_IS_SET(pstCb->u32FlushFlags, VPP_FRC_FLUSH_MBI))
        {
            bStoreMbi = VPP_TRUE;
            LOG_BUF_CB(OBD, MC, MBI, "storing MBI");
        }
        else
        {
            LOG_BUF_CB(OBD, MC, MBI, "[ERROR]: queue to ME(OUTPUT)");
            u32 = u32VppIpFrc_QueueBufInternal(pstCb, VPP_IP_FRC_ME,
                                               VPP_PORT_OUTPUT, pstBuf,
                                               VPP_TRUE);
            if (u32 != VPP_OK)
            {
                bStoreMbi = VPP_TRUE;
                FRC_STATS(pstCb, MeQOutputMbiErr);
                LOGE("unable to queue yuv to me output, returning to "
                     "internal pool, u32=%u", u32);
            }
            else
            {
                FRC_STATS(pstCb, MeQOutputMbi);
            }
        }

        if (bStoreMbi)
        {
            u32 = u32VppBufPool_Put(&pstCb->stMbiPool, pstBuf);
            if (u32 != VPP_OK)
            {
                LOGE("unable to put back in mbi pool... this is very bad, "
                     "u32=%u", u32);
            }
        }

        pthread_mutex_unlock(&pstCb->mutex);
    }
}

static void vVppIpFrc_AssertFlushDone(t_StVppIpFrcCb *pstCb,
                                      t_EVppIpFrcBlk eBlk,
                                      enum vpp_port ePort,
                                      t_EVppBufPxDataType eBufType)
{
    uint32_t i;
    uint32_t u32Cnt;
    uint32_t u32Err = VPP_FALSE;

    if (!pstCb ||
        eBlk >= VPP_IP_FRC_MAX ||
        ePort >= VPP_PORT_MAX ||
        eBufType >= eVppBufPxDataType_Max)
    {
        LOGE("ERROR: %s invalid params, eBlk=%u, ePort=%u, eBufType=%u",
             __func__, eBlk, ePort, eBufType);
        u32Err = VPP_TRUE;
        goto fatal_exception;
    }

    u32Cnt = u32VppIpFrc_GetBufferHoldCnt(pstCb, eBlk, ePort, eBufType);
    if (u32Cnt)
    {
        for (i = 0; i < 10; i++)
        {
            LOGE("ERROR: %u on port %u issued flush done but still holding %u "
                 "buffers of type %u", eBlk, ePort, u32Cnt, eBufType);
            DUMP_BUF_OWNERSHIP(pstCb);
        }
        u32Err = VPP_TRUE;
    }

fatal_exception:
#ifdef VPP_FRC_ASSERT_FLUSH_FATAL
    if (u32Err)
    {
        LOGE("ERROR: fatal exception specified, hanging thread");
        while (1);
    }
#endif
    return;
}

static void vVppIpFrc_HandleFlushDone(t_StVppIpFrcCb *pstCb,
                                      enum vpp_port ePort,
                                      t_EVppIpFrcBlk eBlk)
{
    t_StVppEvt stEvt;
    uint32_t bIssueInputFlushDone = VPP_FALSE;
    uint32_t bIssueOutputFlushDone = VPP_FALSE;

    if (!pstCb)
        return;

    if (ePort != VPP_PORT_INPUT && ePort != VPP_PORT_OUTPUT)
    {
        LOGE("received invalid port for flush done event, port=%u", ePort);
        return;
    }

    if (eBlk != VPP_IP_FRC_ME && eBlk != VPP_IP_FRC_MC)
        return;

    pthread_mutex_lock(&pstCb->mutex);
    if (eBlk == VPP_IP_FRC_ME)
    {
        if (ePort == VPP_PORT_INPUT)
        {
            vVppIpFrc_AssertFlushDone(pstCb, eBlk, ePort, eVppBufPxDataType_Raw);
            pstCb->u32FlushMeInCnt = pstCb->u32FlushMeInCnt == 0? 0:
                                     pstCb->u32FlushMeInCnt - 1;
            if (pstCb->u32FlushMeInCnt == 0)
                VPP_FLAG_CLR(pstCb->u32FlushFlags, VPP_FRC_FLUSH_ME_IN);
            if (VPP_FLAG_IS_SET(pstCb->u32FlushFlags, VPP_FRC_FLUSH_INPUT) &&
                pstCb->u32FlushMeInCnt >= pstCb->u32FlushMcInCnt)
                bIssueInputFlushDone = VPP_TRUE;
        }
        else
        {
            vVppIpFrc_AssertFlushDone(pstCb, eBlk, ePort, eVppBufPxDataType_Compressed);
            pstCb->u32FlushMeOutCnt = pstCb->u32FlushMeOutCnt == 0? 0:
                                      pstCb->u32FlushMeOutCnt - 1;
            if (pstCb->u32FlushMeOutCnt == 0)
                VPP_FLAG_CLR(pstCb->u32FlushFlags, VPP_FRC_FLUSH_ME_OUT);
        }
    }
    else
    {
        vVppIpFrc_AssertFlushDone(pstCb, eBlk, ePort, eVppBufPxDataType_Raw);
        if (ePort == VPP_PORT_INPUT)
        {
            vVppIpFrc_AssertFlushDone(pstCb, eBlk, ePort, eVppBufPxDataType_Compressed);
            pstCb->u32FlushMcInCnt = pstCb->u32FlushMcInCnt == 0? 0:
                                     pstCb->u32FlushMcInCnt - 1;
            if (pstCb->u32FlushMcInCnt == 0)
                VPP_FLAG_CLR(pstCb->u32FlushFlags, VPP_FRC_FLUSH_MC_IN);
            if (VPP_FLAG_IS_SET(pstCb->u32FlushFlags, VPP_FRC_FLUSH_INPUT) &&
                pstCb->u32FlushMcInCnt >= pstCb->u32FlushMeInCnt)
                bIssueInputFlushDone = VPP_TRUE;
        }
        else
        {
            pstCb->u32FlushMcOutCnt = pstCb->u32FlushMcOutCnt == 0? 0:
                                      pstCb->u32FlushMcOutCnt - 1;
            if (pstCb->u32FlushMcOutCnt == 0)
                VPP_FLAG_CLR(pstCb->u32FlushFlags, VPP_FRC_FLUSH_MC_OUT);
            bIssueOutputFlushDone = VPP_TRUE;
        }
    }

    LOGI("flush event for block=%s, port=%u, u32FlushFlags=0x%x",
         FRC_GET_BLK_STR(eBlk), ePort, pstCb->u32FlushFlags);

    if (VPP_FLAG_IS_SET(pstCb->u32FlushFlags, VPP_FRC_FLUSH_INPUT))
    {
        if (!VPP_FLAG_IS_SET(pstCb->u32FlushFlags, VPP_FRC_FLUSH_PORTS_IN))
        {
            VPP_FLAG_CLR(pstCb->u32FlushFlags, VPP_FRC_FLUSH_INPUT);
        }
    }

    if (VPP_FLAG_IS_SET(pstCb->u32FlushFlags, VPP_FRC_FLUSH_OUTPUT))
    {
        if (!VPP_FLAG_IS_SET(pstCb->u32FlushFlags, VPP_FRC_FLUSH_PORTS_OUT))
        {
            VPP_FLAG_CLR(pstCb->u32FlushFlags, VPP_FRC_FLUSH_OUTPUT);
        }
    }

    if (VPP_FLAG_IS_SET(pstCb->u32FlushFlags, VPP_FRC_FLUSH_MBI))
    {
        if (!VPP_FLAG_IS_SET(pstCb->u32FlushFlags, VPP_FRC_FLUSH_PORTS_MBI))
        {
            VPP_FLAG_CLR(pstCb->u32FlushFlags, VPP_FRC_FLUSH_MBI);
            pthread_cond_signal(&pstCb->cond);
        }
    }

    pthread_mutex_unlock(&pstCb->mutex);

    if (bIssueInputFlushDone)
    {
        stEvt.eType = VPP_EVT_FLUSH_DONE;
        stEvt.flush.ePort = VPP_PORT_INPUT;
        u32VppIpCbEvent(&pstCb->stBase.stCb, stEvt);
    }

    if (bIssueOutputFlushDone)
    {
        stEvt.eType = VPP_EVT_FLUSH_DONE;
        stEvt.flush.ePort = VPP_PORT_OUTPUT;
        u32VppIpCbEvent(&pstCb->stBase.stCb, stEvt);
    }
}

static void vVppIpFrc_HandleDrainDone(t_StVppIpFrcCb *pstCb,
                                      t_EVppIpFrcBlk eBlk)
{
    t_StVppEvt stEvt;
    uint32_t u32;
    uint32_t bUnexpected = VPP_FALSE;
    uint32_t bDrainDone = VPP_FALSE;

    if (!pstCb)
        return;

    if (eBlk != VPP_IP_FRC_ME && eBlk != VPP_IP_FRC_MC)
    {
        LOGE("received invalid block for drain done event, blk=%u", eBlk);
        return;
    }

    pthread_mutex_lock(&pstCb->mutex);

    if (!VPP_FLAG_IS_SET(pstCb->u32DrainFlags, VPP_FRC_DRAIN_PENDING))
    {
        pthread_mutex_unlock(&pstCb->mutex);
        LOGE("received unexpected drain done from blk=%u (top level not set)",
             eBlk);
        return;
    }

    if (eBlk == VPP_IP_FRC_ME)
    {
        if (VPP_FLAG_IS_SET(pstCb->u32DrainFlags, VPP_FRC_DRAIN_PENDING_ME))
        {
            pstCb->u32DrainMeCnt = pstCb->u32DrainMeCnt == 0? 0:
                                   pstCb->u32DrainMeCnt - 1;
            if (pstCb->u32DrainMeCnt == 0)
                VPP_FLAG_CLR(pstCb->u32DrainFlags, VPP_FRC_DRAIN_PENDING_ME);

            VPP_FRC_DRAIN(MC, pstCb, u32);
            if (u32 != VPP_OK)
            {
                bDrainDone = VPP_TRUE;
                LOGE("unable to drain MC block, u32=%u", u32);
            }
            else
            {
                pstCb->u32DrainMcCnt++;
                VPP_FLAG_SET(pstCb->u32DrainFlags, VPP_FRC_DRAIN_PENDING_MC);
            }
        }
        else
        {
            bUnexpected = VPP_TRUE;
        }
    }
    else if (eBlk == VPP_IP_FRC_MC)
    {
        if (VPP_FLAG_IS_SET(pstCb->u32DrainFlags, VPP_FRC_DRAIN_PENDING_MC))
        {
            pstCb->u32DrainMcCnt = pstCb->u32DrainMcCnt == 0? 0:
                                   pstCb->u32DrainMcCnt - 1;
            if (pstCb->u32DrainMcCnt == 0)
                VPP_FLAG_CLR(pstCb->u32DrainFlags, VPP_FRC_DRAIN_PENDING_MC);
            bDrainDone = VPP_TRUE;
        }
        else
        {
            bUnexpected = VPP_TRUE;
        }
    }

    if (bUnexpected)
    {
        LOGE("received unexpected drain done from %s, but drain was not "
             "requested on the block", FRC_GET_BLK_STR(eBlk));
    }

    if (bDrainDone &&
        !VPP_FLAG_IS_SET(pstCb->u32DrainFlags, VPP_FRC_DRAIN_PENDING_MC) &&
        !VPP_FLAG_IS_SET(pstCb->u32DrainFlags, VPP_FRC_DRAIN_PENDING_ME))
    {
        VPP_FLAG_CLR(pstCb->u32DrainFlags, VPP_FRC_DRAIN_PENDING);
    }

    pthread_mutex_unlock(&pstCb->mutex);

    if (bDrainDone)
    {
        stEvt.eType = VPP_EVT_DRAIN_DONE;
        u32VppIpCbEvent(&pstCb->stBase.stCb, stEvt);
    }
}

static void vVppIpFrc_InputBufferDoneCb(void *pv, t_StVppBuf *pstBuf)
{
    uint32_t u32;
    t_StVppIpFrcCb *pstCb;
    t_EVppIpFrcBlk eBlk = VPP_IP_FRC_MAX;

    pstCb = pstVppIpFrc_GetCbFromCallback(pv, &eBlk);
    if (!pstCb || !pstBuf)
    {
        LOGE("%s(): Invalid args: pstCb=%p, pstBuf=%p", __func__, pstCb, pstBuf);
        return;
    }

    LOGI("Received IBD blk=%s, eBufPxType=%s", FRC_GET_BLK_STR(eBlk),
         FRC_GET_BUF_TYPE(pstBuf->eBufPxType));
    LOG_BUF_PX(pstBuf);
    LOG_BUF_EX(pstBuf);

    if (pstBuf->eBufPxType != eVppBufPxDataType_Raw &&
        pstBuf->eBufPxType != eVppBufPxDataType_Compressed)
    {
        LOGE("error, unknown buffer type");
        return;
    }

    vVppIpFrc_UpdateOwnedBuffers(pstCb, eBlk, pstBuf);

    if (eBlk == VPP_IP_FRC_ME)
    {
        vVppIpFrc_HandleMeIbd(pstCb, pstBuf);
    }
    else if (eBlk == VPP_IP_FRC_MC)
    {
        vVppIpFrc_HandleMcIbd(pstCb, pstBuf);
    }
    else
    {
        LOGE("error, got ibd from block=%u", eBlk);
    }
}

static void vVppIpFrc_OutputBufferDoneCb(void *pv, t_StVppBuf *pstBuf)
{
    uint32_t u32;
    t_StVppIpFrcCb *pstCb;
    t_EVppIpFrcBlk eBlk = VPP_IP_FRC_MAX;

    pstCb = pstVppIpFrc_GetCbFromCallback(pv, &eBlk);
    if (!pstCb || !pstBuf)
    {
        LOGE("%s(): Invalid args: pstCb=%p, pstBuf=%p", __func__, pstCb, pstBuf);
        return;
    }

    LOGI("Received OBD blk=%s, eBufPxType=%s", FRC_GET_BLK_STR(eBlk),
         FRC_GET_BUF_TYPE(pstBuf->eBufPxType));
    LOG_BUF_PX(pstBuf);
    LOG_BUF_EX(pstBuf);

    if (pstBuf->eBufPxType != eVppBufPxDataType_Raw &&
        pstBuf->eBufPxType != eVppBufPxDataType_Compressed)
    {
        LOGE("error, unknown buffer type");
        return;
    }

    vVppIpFrc_UpdateOwnedBuffers(pstCb, eBlk, pstBuf);

    if (eBlk == VPP_IP_FRC_ME)
    {
        vVppIpFrc_HandleMeObd(pstCb, pstBuf);
    }
    else if (eBlk == VPP_IP_FRC_MC)
    {
        vVppIpFrc_HandleMcObd(pstCb, pstBuf);
    }
    else
    {
        LOGE("error, got ibd from block=%u", eBlk);
    }
}

static void vVppIpFrc_EventCb(void *pv, t_StVppEvt stEvt)
{
    uint32_t u32;
    t_StVppIpFrcCb *pstCb;
    t_EVppIpFrcBlk eBlk = VPP_IP_FRC_MAX;

    pstCb = pstVppIpFrc_GetCbFromCallback(pv, &eBlk);
    if (!pstCb)
    {
        LOGE("%s(): Invalid args: pstCb=%p", __func__, pstCb);
        return;
    }

    LOGI("received event from blk=%s, event=%u", FRC_GET_BLK_STR(eBlk),
         stEvt.eType);

    if (stEvt.eType == VPP_EVT_FLUSH_DONE)
    {
        vVppIpFrc_HandleFlushDone(pstCb, stEvt.flush.ePort, eBlk);
    }
    else if (stEvt.eType == VPP_EVT_DRAIN_DONE)
    {
        vVppIpFrc_HandleDrainDone(pstCb, eBlk);
    }
    else
    {
        LOGE("received unexpected event");
    }

}

static void vVppIpFrc_LogCb(void *pv, t_StVppBuf *pstBuf, t_EVppBufIpLogId eId)
{
    VPP_UNUSED(pv);
    VPP_UNUSED(pstBuf);
    VPP_UNUSED(eId);
}

static void vVppIpFrc_DumpBufferReqs(const char *pcStr, t_StVppIpBufReq *pstReq)
{
    if (!pstReq || !pcStr)
        return;

    LOGI("%s: u32PxSz=%u, u32ExSz=%u, u32MinCnt=%u", pcStr, pstReq->u32PxSz,
         pstReq->u32ExSz, pstReq->u32MinCnt);
}

static uint32_t u32VppIpFrc_GetBufferReqs(t_StVppIpFrcCb *pstCb)
{
    uint32_t u32Ret = VPP_OK;
    uint32_t u32Err = VPP_OK;

    if (!pstCb)
        return VPP_ERR_PARAM;

    VPP_FRC_GET_BUF_REQ(MC, pstCb, &pstCb->stMcInputReq,
                        &pstCb->stMcOutputReq, u32Ret);
    if (u32Ret != VPP_OK)
    {
        LOGE("Unable to get buffer requirements for MC, u32Ret=%u", u32Ret);
        u32Err = VPP_ERR;
    }

    VPP_FRC_GET_BUF_REQ(ME, pstCb, &pstCb->stMeInputReq,
                        &pstCb->stMeOutputReq, u32Ret);
    if (u32Ret != VPP_OK)
    {
        LOGE("Unable to get buffer requirements for ME, u32Ret=%u", u32Ret);
        u32Err = VPP_ERR;
    }

    vVppIpFrc_DumpBufferReqs("ME_IN", &pstCb->stMeInputReq);
    vVppIpFrc_DumpBufferReqs("ME_OUT", &pstCb->stMeOutputReq);
    vVppIpFrc_DumpBufferReqs("MC_IN", &pstCb->stMcInputReq);
    vVppIpFrc_DumpBufferReqs("MC_OUT", &pstCb->stMcOutputReq);

    return u32Err;
}

static inline uint32_t VPP_MAX(uint32_t u32Left, uint32_t u32Right)
{
    if (u32Left > u32Right)
        return u32Left;
    else
        return u32Right;
}

static uint32_t u32VppIpFrc_FreeMbiStructures(t_StVppIpFrcCb *pstCb)
{
    if (!pstCb)
        return VPP_ERR_PARAM;

    if (pstCb->pstMbiIntBuf)
        free(pstCb->pstMbiIntBuf);

    if (pstCb->pstVppExtBuf)
        free(pstCb->pstVppExtBuf);

    if (pstCb->pstMbiIonBuf)
        free(pstCb->pstMbiIonBuf);

    pstCb->pstMbiIntBuf = NULL;
    pstCb->pstVppExtBuf = NULL;
    pstCb->pstMbiIonBuf = NULL;
    memset(&pstCb->stMbiBufAlloc, 0, sizeof(t_StVppIpBufReq));

    return VPP_OK;
}

static uint32_t u32VppIpFrc_AllocMbiStructures(t_StVppIpFrcCb *pstCb)
{
    uint32_t u32Cnt;

    if (!pstCb)
        return VPP_ERR_PARAM;

    if (pstCb->pstMbiIntBuf || pstCb->pstVppExtBuf || pstCb->pstMbiIonBuf)
    {
        LOGE("mbi structures already allocated: pstMbiIntBuf=%p, "
             "pstVppExtBuf=%p, pstMbiIonBuf=%p", pstCb->pstMbiIntBuf,
             pstCb->pstVppExtBuf, pstCb->pstMbiIonBuf);
        return VPP_ERR;
    }

    u32Cnt = pstCb->stMbiBufAlloc.u32MinCnt;

    pstCb->pstMbiIntBuf =
        (t_StVppBuf *)calloc(u32Cnt, sizeof(t_StVppBuf));
    if (!pstCb->pstMbiIntBuf)
    {
        LOGE("unable to allocate pstMbiIntBuf");
        goto ERR_ALLOC;
    }

    pstCb->pstVppExtBuf =
        (struct vpp_buffer *)calloc(u32Cnt, sizeof(struct vpp_buffer));
    if (!pstCb->pstVppExtBuf)
    {
        LOGE("unable to allocate pstVppExtBuf");
        goto ERR_ALLOC;
    }

    pstCb->pstMbiIonBuf =
        (t_StVppFrcMbiIonBuf *)calloc(u32Cnt, sizeof(t_StVppFrcMbiIonBuf));
    if (!pstCb->pstMbiIonBuf)
    {
        LOGE("unable to allocate pstMbiIonBuf");
        goto ERR_ALLOC;
    }

    return VPP_OK;

ERR_ALLOC:
    u32VppIpFrc_FreeMbiStructures(pstCb);
    return VPP_ERR_NO_MEM;
}

static uint32_t u32VppIpFrc_FreeMbiIonBuffers(t_StVppIpFrcCb *pstCb)
{
    uint32_t i, u32, u32Err = VPP_OK;

    for (i = 0; i < pstCb->stMbiBufAlloc.u32MinCnt; i++)
    {
        u32 = u32VppIon_Free(pstCb->stBase.pstCtx,
                             &pstCb->pstMbiIonBuf[i].stPx);
        if (u32 != VPP_OK)
        {
            LOGE("unable to ion free ion px at i=%u, u32=%u", i, u32);
            u32Err = VPP_ERR;
        }

        u32 = u32VppIon_Free(pstCb->stBase.pstCtx,
                             &pstCb->pstMbiIonBuf[i].stEx);
        if (u32 != VPP_OK)
        {
            LOGE("unable to ion free ion ex at i=%u, u32=%u", i, u32);
            u32Err = VPP_ERR;
        }
    }

    return u32Err;
}

static uint32_t u32VppIpFrc_AllocMbiIonBuffers(t_StVppIpFrcCb *pstCb)
{
    uint32_t u32, i;
    uint32_t u32Px, u32Ex;

    for (u32Px = 0; u32Px < pstCb->stMbiBufAlloc.u32MinCnt; u32Px++)
    {
        u32 = u32VppIon_Alloc(pstCb->stBase.pstCtx,
                              pstCb->stMbiBufAlloc.u32PxSz,
                              pstCb->stBase.bSecureSession,
                              &pstCb->pstMbiIonBuf[u32Px].stPx);
        if (u32 != VPP_OK)
        {
            LOGE("unable to ion allocate mbi px buffers, u32Px=%u, u32=%u",
                 u32Px, u32);
            goto ERR_ALLOC_MBI_PX;
        }
    }

    if (pstCb->stMbiBufAlloc.u32ExSz)
    {
        for (u32Ex = 0; u32Ex < pstCb->stMbiBufAlloc.u32MinCnt; u32Ex++)
        {
            u32 = u32VppIon_Alloc(pstCb->stBase.pstCtx,
                                  pstCb->stMbiBufAlloc.u32ExSz,
                                  VPP_FALSE,
                                  &pstCb->pstMbiIonBuf[u32Ex].stEx);
            if (u32 != VPP_OK)
            {
                LOGE("unable to ion allocate mbi ex buffers, u32Ex=%u, u32=%u",
                     u32Ex, u32);
                goto ERR_ALLOC_MBI_EX;
            }
        }
    }

    return VPP_OK;

ERR_ALLOC_MBI_EX:
    for (; u32Ex; u32Ex--)
    {
        i = u32Ex - 1;
        u32 = u32VppIon_Free(pstCb->stBase.pstCtx, &pstCb->pstMbiIonBuf[i].stEx);
        if (!u32)
            LOGE("unable to ion free mbi ex buffer at i=%u, u32=%u", i, u32);
    }

ERR_ALLOC_MBI_PX:
    for (; u32Px; u32Px--)
    {
        i = u32Px - 1;
        u32 = u32VppIon_Free(pstCb->stBase.pstCtx, &pstCb->pstMbiIonBuf[i].stPx);
        if (!u32)
            LOGE("unable to ion free mbi px buffer at i=%u, u32=%u", i, u32);
    }

    return VPP_ERR;
}

static uint32_t u32VppIpFrc_UnassociateMbiBuffers(t_StVppIpFrcCb *pstCb)
{
    uint32_t i;
    t_StVppBuf *pstBuf;
    struct vpp_buffer *pstExtBuf;

    if (!pstCb)
        return VPP_ERR_PARAM;

    for (i = 0; i < pstCb->stMbiBufAlloc.u32MinCnt; i++)
    {
        pstBuf = &pstCb->pstMbiIntBuf[i];
        pstExtBuf = &pstCb->pstVppExtBuf[i];

        // External buffer
        memset(&pstExtBuf->pixel, 0, sizeof(pstExtBuf->pixel));
        memset(&pstExtBuf->extradata, 0, sizeof(pstExtBuf->extradata));

        // Internal buffer
        pstBuf->pBuf = NULL;
        memset(&pstBuf->stPixel, 0, sizeof(t_StVppMemBuf));
        memset(&pstBuf->stExtra, 0, sizeof(t_StVppMemBuf));
    }

    return VPP_OK;
}

static uint32_t u32VppIpFrc_AssociateMbiBuffers(t_StVppIpFrcCb *pstCb)
{
    uint32_t i;
    t_StVppBuf *pstBuf;
    struct vpp_buffer *pstExtBuf;
    t_StVppIonBuf *pstIonPx, *pstIonEx;

    if (!pstCb)
        return VPP_ERR_PARAM;

    for (i = 0; i < pstCb->stMbiBufAlloc.u32MinCnt; i++)
    {
        pstBuf = &pstCb->pstMbiIntBuf[i];
        pstExtBuf = &pstCb->pstVppExtBuf[i];
        pstIonPx = &pstCb->pstMbiIonBuf[i].stPx;
        pstIonEx = &pstCb->pstMbiIonBuf[i].stEx;

        // Fill out the internal buffer structure
        pstBuf->stPixel.fd = pstIonPx->fd_ion_mem;
        pstBuf->stPixel.u32Offset = 0;
        pstBuf->stPixel.priv.u32PaOffset = 0;
        pstBuf->stPixel.u32AllocLen = pstIonPx->len;
        pstBuf->stPixel.u32FilledLen = 0;
        pstBuf->stPixel.u32ValidLen = pstIonPx->len;

        pstBuf->stExtra.fd = pstIonEx->fd_ion_mem;
        pstBuf->stExtra.u32Offset = 0;
        pstBuf->stExtra.priv.u32PaOffset = 0;
        pstBuf->stExtra.u32AllocLen = pstIonEx->len;
        pstBuf->stExtra.u32FilledLen = 0;
        pstBuf->stExtra.u32ValidLen = pstIonEx->len;

        // Internal buffer structure
        pstBuf->pBuf = pstExtBuf;
        pstBuf->eBufPxType = eVppBufPxDataType_Compressed;

        if (pstCb->stBase.bSecureSession)
        {
            pstBuf->stPixel.pvBase = VPP_BUF_UNMAPPED_BUF_VAL;
            pstBuf->stPixel.priv.pvPa = VPP_BUF_UNMAPPED_BUF_VAL;
            pstBuf->stPixel.u32MappedLen = 0;
            pstBuf->stPixel.priv.u32PaMappedLen = 0;
            pstBuf->stPixel.eMapping = eVppBuf_Unmapped;
        }
        else
        {
            pstBuf->stPixel.pvBase = pstIonPx->buf;
            pstBuf->stPixel.priv.pvPa = pstIonPx->buf;
            pstBuf->stPixel.u32MappedLen = pstIonPx->len;
            pstBuf->stPixel.priv.u32PaMappedLen = pstIonPx->len;
            pstBuf->stPixel.eMapping = eVppBuf_MappedInternal;
        }
        pstBuf->stExtra.pvBase = pstIonEx->buf;
        pstBuf->stExtra.priv.pvPa = pstIonEx->buf;
        pstBuf->stExtra.u32MappedLen = pstIonEx->len;
        pstBuf->stExtra.priv.u32PaMappedLen = pstIonEx->len;
        pstBuf->stExtra.eMapping = eVppBuf_MappedInternal;

        LOGI("linked: i=%u, pstBuf=%p, pstExtBuf=%p, pstIonPx=%p, pstIonEx=%p",
             i, pstBuf, pstExtBuf, pstIonPx, pstIonEx);
        LOG_BUF_PX(pstBuf);
        LOG_BUF_EX(pstBuf);
    }

    return VPP_OK;
}

static uint32_t u32VppIpFrc_UnprepareMbiBufferPool(t_StVppIpFrcCb *pstCb)
{

    t_StVppBuf *pstBuf;
    uint32_t u32Cnt = 0;

    if (!pstCb)
        return VPP_ERR_PARAM;

    pthread_mutex_lock(&pstCb->mutex);
    while ((pstBuf = pstVppBufPool_Get(&pstCb->stMbiPool)) != NULL)
    {
        u32Cnt += 1;
        LOGI("retrieved pstBuf=%p from pool", pstBuf);
    }
    pthread_mutex_unlock(&pstCb->mutex);

    if (u32Cnt != pstCb->stMbiBufAlloc.u32MinCnt)
    {
        LOGE("unexpected amount of buffers from mbi pool, expected=%u, actual=%u",
             pstCb->stMbiBufAlloc.u32MinCnt, u32Cnt);

        return VPP_ERR;
    }

    LOGI("removed %u buffers from mbi pool", u32Cnt);

    return VPP_OK;
}

static uint32_t u32VppIpFrc_PrepareMbiBufferPool(t_StVppIpFrcCb *pstCb)
{
    uint32_t i, u32;
    t_StVppBuf *pstBuf;

    if (!pstCb)
        return VPP_ERR_PARAM;

    pthread_mutex_lock(&pstCb->mutex);
    for (i = 0; i < pstCb->stMbiBufAlloc.u32MinCnt; i++)
    {
        u32 = u32VppBufPool_Put(&pstCb->stMbiPool, &pstCb->pstMbiIntBuf[i]);
        if (u32 != VPP_OK)
        {
            LOGE("unable to put buffer into mbi pool, u32=%u", u32);
            goto ERR_BUF_PUT;
        }
    }
    pthread_mutex_unlock(&pstCb->mutex);

    return VPP_OK;

ERR_BUF_PUT:
    while ((pstBuf = pstVppBufPool_Get(&pstCb->stMbiPool)) != NULL)
        LOGI("retrieved pstBuf=%p from pool", pstBuf);

    pthread_mutex_unlock(&pstCb->mutex);

    return VPP_ERR;
}

static uint32_t u32VppIpFrc_FreeMbiBuffers(t_StVppIpFrcCb *pstCb)
{
    uint32_t u32, u32Err = VPP_OK;

    if (!pstCb)
        return VPP_ERR_PARAM;

    u32 = u32VppIpFrc_UnprepareMbiBufferPool(pstCb);
    if (u32 != VPP_OK)
    {
        u32Err = VPP_ERR;
        LOGE("unable to unprepare mbi buffers, u32=%u", u32);
    }

    u32 = u32VppIpFrc_UnassociateMbiBuffers(pstCb);
    if (u32 != VPP_OK)
    {
        u32Err = VPP_ERR;
        LOGE("unable to unassociate mbi buffers, u32=%u", u32);
    }

    u32 = u32VppIpFrc_FreeMbiIonBuffers(pstCb);
    if (u32 != VPP_OK)
    {
        u32Err = VPP_ERR;
        LOGE("unable to free mbi buffers, u32=%u", u32);
    }

    u32 = u32VppIpFrc_FreeMbiStructures(pstCb);
    if (u32 != VPP_OK)
    {
        u32Err = VPP_ERR;
        LOGE("unable to free mbi structures, u32=%u", u32);
    }

    return u32Err;
}

static uint32_t u32VppIpFrc_AllocateMbiBuffers(t_StVppIpFrcCb *pstCb)
{
    uint32_t u32;

    if (!pstCb)
        return VPP_ERR_PARAM;

    pstCb->stMbiBufAlloc.u32MinCnt = VPP_MAX(pstCb->stMeOutputReq.u32MinCnt,
                                             pstCb->stMcInputReq.u32MinCnt);

    pstCb->stMbiBufAlloc.u32PxSz = VPP_MAX(pstCb->stMeOutputReq.u32PxSz,
                                           pstCb->stMcInputReq.u32PxSz);
    pstCb->stMbiBufAlloc.u32ExSz = VPP_MAX(pstCb->stMeOutputReq.u32ExSz,
                                           pstCb->stMcInputReq.u32ExSz);

    LOGI("allocating mbi, cnt=%u, u32PxSz=%u, u32ExSz=%u",
         pstCb->stMbiBufAlloc.u32MinCnt,
         pstCb->stMbiBufAlloc.u32PxSz,
         pstCb->stMbiBufAlloc.u32ExSz);

    u32 = u32VppIpFrc_AllocMbiStructures(pstCb);
    if (u32 != VPP_OK)
    {
        LOGE("unable to allocate mbi structures, u32=%u", u32);
        goto ERR_ALLOC_MBI_STRUCTS;
    }

    u32 = u32VppIpFrc_AllocMbiIonBuffers(pstCb);
    if (u32 != VPP_OK)
    {
        LOGE("unable to ion allocate mbi buffers, u32=%u", u32);
        goto ERR_ALLOC_BUFFERS;
    }

    u32 = u32VppIpFrc_AssociateMbiBuffers(pstCb);
    if (u32 != VPP_OK)
    {
        LOGE("unable link mbi buffers, u32=%u", u32);
        goto ERR_ASSOCIATE_BUFFERS;
    }

    u32 = u32VppIpFrc_PrepareMbiBufferPool(pstCb);
    if (u32 != VPP_OK)
    {
        LOGE("unable prepare mbi pool, u32=%u", u32);
        goto ERR_PREPARE_POOL;
    }

    return VPP_OK;

ERR_PREPARE_POOL:
    u32VppIpFrc_UnassociateMbiBuffers(pstCb);

ERR_ASSOCIATE_BUFFERS:
    u32VppIpFrc_FreeMbiIonBuffers(pstCb);

ERR_ALLOC_BUFFERS:
    u32VppIpFrc_FreeMbiStructures(pstCb);

ERR_ALLOC_MBI_STRUCTS:
    return VPP_ERR;
}

static uint32_t u32VppIpFrc_FlushMbiBuffers(t_StVppIpFrcCb *pstCb)
{
    uint32_t u32WaitFlags = 0;
    uint32_t u32, u32Err = VPP_OK;

    if (!pstCb)
        return VPP_ERR_PARAM;

    pthread_mutex_lock(&pstCb->mutex);

    VPP_FRC_FLUSH(ME, pstCb, VPP_PORT_OUTPUT, u32);
    if (u32 != VPP_OK)
    {
        LOGE("unable to flush ME output port, u32=%u", u32);
        u32Err = VPP_ERR;
    }
    else
    {
        pstCb->u32FlushMeOutCnt++;
        VPP_FLAG_SET(pstCb->u32FlushFlags, VPP_FRC_FLUSH_ME_OUT);
    }

    VPP_FRC_FLUSH(MC, pstCb, VPP_PORT_INPUT, u32);
    if (u32 != VPP_OK)
    {
        LOGE("unable to flush MC input port, u32=%u", u32);
        u32Err = VPP_ERR;
    }
    else
    {
        pstCb->u32FlushMcInCnt++;
        VPP_FLAG_SET(pstCb->u32FlushFlags, VPP_FRC_FLUSH_MC_IN);
    }

    VPP_FLAG_SET(pstCb->u32FlushFlags, VPP_FRC_FLUSH_MBI);

    u32WaitFlags = VPP_FRC_FLUSH_MBI | VPP_FRC_FLUSH_PORTS_MBI;
    while (VPP_FLAG_IS_SET(pstCb->u32FlushFlags, u32WaitFlags))
    {
        LOGI("waiting for flush signal, flags=0x%x", pstCb->u32FlushFlags);
        pthread_cond_wait(&pstCb->cond, &pstCb->mutex);
    }

    pthread_mutex_unlock(&pstCb->mutex);

    return u32Err;
}

static uint32_t u32VppIpFrc_QueueMbiBuffers(t_StVppIpFrcCb *pstCb)
{
    t_StVppBuf *pstBuf;
    uint32_t u32 = VPP_OK;
    uint32_t u32Err;

    if (!pstCb)
        return VPP_ERR_PARAM;

    pthread_mutex_lock(&pstCb->mutex);
    while ((pstBuf = pstVppBufPool_Get(&pstCb->stMbiPool)) != NULL)
    {
        u32 = u32VppIpFrc_QueueBufInternal(pstCb, VPP_IP_FRC_ME,
                                           VPP_PORT_OUTPUT, pstBuf,
                                           VPP_TRUE);
        if (u32 != VPP_OK)
        {
            LOGE("unable to queue mbi buffer, u32=%u", u32);
            FRC_STATS(pstCb, MeQOutputMbiErr);
            u32VppBufPool_Put(&pstCb->stMbiPool, pstBuf);
            break;
        }
        else
        {
            FRC_STATS(pstCb, MeQOutputMbi);
        }
    }
    pthread_mutex_unlock(&pstCb->mutex);

    if (u32 != VPP_OK)
    {
        u32Err = u32VppIpFrc_FlushMbiBuffers(pstCb);
        if (u32Err != VPP_OK)
            LOGE("unable to flush mbi buffers (error handling), u32=%u", u32Err);
    }

    return u32;
}

static uint32_t u32VppIpFrc_ProcCmdInit(t_StVppIpFrcCb *pstCb,
                                        t_EVppIpFrcBlk eBlk)
{
    void *vp = NULL;
    t_StVppCallback stFrcCb;
    t_StVppFrcSubmodule *pstSubmod;

    if (!pstCb || eBlk >= VPP_IP_FRC_MAX)
    {
        return VPP_ERR_PARAM;
    }

    pstSubmod = &pstCb->astSubmod[eBlk];

    stFrcCb.input_buffer_done = vVppIpFrc_InputBufferDoneCb;
    stFrcCb.output_buffer_done = vVppIpFrc_OutputBufferDoneCb;
    stFrcCb.event = vVppIpFrc_EventCb;
    stFrcCb.log = vVppIpFrc_LogCb;
    stFrcCb.pv = (void *)&pstCb->astCbCtx[eBlk];

    if (eBlk == VPP_IP_FRC_ME)
    {
        VPP_FRC_INIT(ME, pstCb, pstCb->stBase.u32Flags, stFrcCb, vp);
    }
    else if (eBlk == VPP_IP_FRC_MC)
    {
        VPP_FRC_INIT(MC, pstCb, pstCb->stBase.u32Flags, stFrcCb, vp);
    }

    LOGD("block=%u finished initializing, vp=%p", eBlk, vp);

    pthread_mutex_lock(&pstCb->mutex);
    pstCb->apvFrcCtx[eBlk] = vp;
    pstSubmod->u32Ret = vp ? VPP_OK : VPP_ERR;
    VPP_FLAG_SET(pstSubmod->u32SubmodFlags, 1 << VPP_IP_CMD_INIT);
    pthread_mutex_unlock(&pstCb->mutex);
    pthread_cond_signal(&pstCb->cond);

    return VPP_OK;
}

static uint32_t u32VppIpFrc_ProcCmdTerm(t_StVppIpFrcCb *pstCb,
                                        t_EVppIpFrcBlk eBlk)
{
    uint32_t u32Ret;
    t_StVppFrcSubmodule *pstSubmod;

    if (!pstCb || eBlk >= VPP_IP_FRC_MAX)
    {
        return VPP_ERR_PARAM;
    }

    pstSubmod = &pstCb->astSubmod[eBlk];

    if (pstCb->apvFrcCtx[eBlk])
    {
        if (eBlk == VPP_IP_FRC_ME)
        {
            VPP_FRC_TERM(ME, pstCb);
        }
        else if (eBlk == VPP_IP_FRC_MC)
        {
            VPP_FRC_TERM(MC, pstCb);
        }
    }

    pthread_mutex_lock(&pstCb->mutex);
    pstCb->apvFrcCtx[eBlk] = NULL;
    pstSubmod->u32Ret = VPP_OK;
    VPP_FLAG_SET(pstSubmod->u32SubmodFlags, 1 << VPP_IP_CMD_TERM);
    pthread_mutex_unlock(&pstCb->mutex);
    pthread_cond_signal(&pstCb->cond);

    return VPP_OK;
}

static uint32_t u32VppIpFrc_ProcCmdOpen(t_StVppIpFrcCb *pstCb,
                                        t_EVppIpFrcBlk eBlk)
{
    uint32_t u32Ret = VPP_ERR;
    t_StVppFrcSubmodule *pstSubmod;

    if (!pstCb || eBlk >= VPP_IP_FRC_MAX)
    {
        return VPP_ERR_PARAM;
    }

    pstSubmod = &pstCb->astSubmod[eBlk];

    if (eBlk == VPP_IP_FRC_ME)
    {
        VPP_FRC_OPEN(ME, pstCb, u32Ret);
    }
    else if (eBlk == VPP_IP_FRC_MC)
    {
        VPP_FRC_OPEN(MC, pstCb, u32Ret);
    }

    pthread_mutex_lock(&pstCb->mutex);
    pstSubmod->u32Ret = u32Ret;
    VPP_FLAG_SET(pstSubmod->u32SubmodFlags, 1 << VPP_IP_CMD_OPEN);
    pthread_mutex_unlock(&pstCb->mutex);
    pthread_cond_signal(&pstCb->cond);

    return VPP_OK;
}

static uint32_t u32VppIpFrc_ProcCmdClose(t_StVppIpFrcCb *pstCb,
                                         t_EVppIpFrcBlk eBlk)
{
    uint32_t u32Ret = VPP_ERR;
    t_StVppFrcSubmodule *pstSubmod;

    if (!pstCb || eBlk >= VPP_IP_FRC_MAX)
    {
        return VPP_ERR_PARAM;
    }

    pstSubmod = &pstCb->astSubmod[eBlk];

    if (eBlk == VPP_IP_FRC_ME)
    {
        VPP_FRC_CLOSE(ME, pstCb, u32Ret);
    }
    else if (eBlk == VPP_IP_FRC_MC)
    {
        VPP_FRC_CLOSE(MC, pstCb, u32Ret);
    }

    pthread_mutex_lock(&pstCb->mutex);
    pstSubmod->u32Ret = u32Ret;
    VPP_FLAG_SET(pstSubmod->u32SubmodFlags, 1 << VPP_IP_CMD_CLOSE);
    pthread_mutex_unlock(&pstCb->mutex);
    pthread_cond_signal(&pstCb->cond);

    return VPP_OK;
}

static uint32_t u32VppIpFrc_ProcCmdReconfigure(t_StVppIpFrcCb *pstCb,
                                               t_EVppIpFrcBlk eBlk)
{
    uint32_t u32Ret = VPP_ERR;
    t_StVppFrcSubmodule *pstSubmod;

    if (!pstCb || eBlk >= VPP_IP_FRC_MAX)
    {
        return VPP_ERR_PARAM;
    }

    pstSubmod = &pstCb->astSubmod[eBlk];

    if (eBlk == VPP_IP_FRC_ME)
    {
        VPP_FRC_RECONFIGURE(ME, pstCb, pstCb->stBase.stInputParam,
                            pstCb->stBase.stOutputParam, u32Ret);
    }
    else if (eBlk == VPP_IP_FRC_MC)
    {
        VPP_FRC_RECONFIGURE(MC, pstCb, pstCb->stBase.stInputParam,
                            pstCb->stBase.stOutputParam, u32Ret);
    }

    pthread_mutex_lock(&pstCb->mutex);
    pstSubmod->u32Ret = u32Ret;
    VPP_FLAG_SET(pstSubmod->u32SubmodFlags, 1 << VPP_IP_CMD_RECONFIGURE);
    pthread_mutex_unlock(&pstCb->mutex);
    pthread_cond_signal(&pstCb->cond);

    return VPP_OK;
}


static void *vpVppIpFrc_SubmodWorker(void *pv)
{
    uint32_t u32;
    t_StVppFrcSubmodule *pstSub = (t_StVppFrcSubmodule *)pv;
    t_StVppIpFrcCb *pstCb;
    t_StVppIpCmd stCmd;

    LOGD("%s started", __func__);

    if (!pstSub || !pstSub->pstCb)
    {
        LOGE("ERROR: worker did not get proper params");
        goto FRC_WORKER_EXIT;
    }

    pstCb = pstSub->pstCb;

    while (1)
    {
        u32 = u32VppIp_CmdGet(&pstSub->stCmdQ, &stCmd, VPP_TRUE);

        if (u32 != VPP_OK)
        {
            LOGE("ERROR: returned from command with error, u32=%u", u32);
            continue;
        }

        if (stCmd.eCmd == VPP_IP_CMD_INIT)
            u32VppIpFrc_ProcCmdInit(pstCb, pstSub->eBlk);

        else if (stCmd.eCmd == VPP_IP_CMD_TERM)
            u32VppIpFrc_ProcCmdTerm(pstCb, pstSub->eBlk);

        else if (stCmd.eCmd == VPP_IP_CMD_OPEN)
            u32VppIpFrc_ProcCmdOpen(pstCb, pstSub->eBlk);

        else if (stCmd.eCmd == VPP_IP_CMD_CLOSE)
            u32VppIpFrc_ProcCmdClose(pstCb, pstSub->eBlk);

        else if (stCmd.eCmd == VPP_IP_CMD_RECONFIGURE)
            u32VppIpFrc_ProcCmdReconfigure(pstCb, pstSub->eBlk);

        else if (stCmd.eCmd == VPP_IP_CMD_THREAD_EXIT)
        {
            LOGD("received exit command");
            break;
        }

        else
            LOGE("ERROR: received unknown command, cmd=%u", stCmd.eCmd);
    }

    pthread_mutex_lock(&pstCb->mutex);
    VPP_FLAG_SET(pstSub->u32SubmodFlags, 1 << VPP_IP_CMD_THREAD_EXIT);
    pthread_mutex_unlock(&pstCb->mutex);
    pthread_cond_signal(&pstCb->cond);

FRC_WORKER_EXIT:
    LOGD("%s exited", __func__);

    return NULL;
}

static uint32_t u32VppIpFrc_SubmodIssueCmd(t_StVppIpFrcCb *pstCb,
                                           t_EVppIpCmd eCmd)
{
    uint32_t u32, i;
    uint32_t u32Err = VPP_OK;
    t_StVppIpCmd stCmd;

    if (!pstCb || eCmd >= VPP_IP_CMD_MAX)
        return VPP_ERR_PARAM;

    stCmd.eCmd = eCmd;

    pthread_mutex_lock(&pstCb->mutex);
    for (i = 0; i < VPP_IP_FRC_MAX; i++)
    {
        VPP_FLAG_CLR(pstCb->astSubmod[i].u32SubmodFlags, (1 << eCmd));
        u32 = u32VppIp_CmdPut(&pstCb->astSubmod[i].stCmdQ, stCmd);
        if (u32)
        {
            u32Err = VPP_ERR;
            LOGE("ERROR: unable to issue command to block=%u, u32=%u", i, u32);
        }
    }
    pthread_mutex_unlock(&pstCb->mutex);

    return u32Err;
}

static uint32_t u32VppIpFrc_WaitCmdDone(t_StVppIpFrcCb *pstCb,
                                        t_EVppIpCmd eCmd)
{
    LOG_ENTER();
    uint32_t i;
    uint32_t u32ExpFlag;

    if (!pstCb || eCmd >= VPP_IP_CMD_MAX)
        return VPP_ERR_PARAM;

    u32ExpFlag = 1 << eCmd;

    pthread_mutex_lock(&pstCb->mutex);
    for (i = 0; i < VPP_IP_FRC_MAX; i++)
    {
        while((pstCb->astSubmod[i].u32SubmodFlags & u32ExpFlag) != u32ExpFlag)
            pthread_cond_wait(&pstCb->cond, &pstCb->mutex);

        VPP_FLAG_CLR(pstCb->astSubmod[i].u32SubmodFlags, u32ExpFlag);
    }
    pthread_mutex_unlock(&pstCb->mutex);

    LOG_EXIT_RET(VPP_OK);
}

static uint32_t u32VppIpFrc_GetCmdResult(t_StVppIpFrcCb *pstCb,
                                         t_EVppIpCmd eCmd,
                                         uint32_t *pu32Ret,
                                         uint32_t u32Len)
{
    uint32_t i;

    if (!pstCb || !pu32Ret || eCmd >= VPP_IP_CMD_MAX || u32Len >= VPP_IP_CMD_MAX)
        return VPP_ERR_PARAM;

    pthread_mutex_lock(&pstCb->mutex);
    for (i = 0; i < VPP_IP_FRC_MAX; i++)
    {
        pu32Ret[i] = pstCb->astSubmod[i].u32Ret;
    }
    pthread_mutex_unlock(&pstCb->mutex);

    return VPP_OK;
}

static uint32_t u32VppIpFrc_SubmodInit(t_StVppIpFrcCb *pstCb)
{
    uint32_t u32, i;
    t_StVppFrcSubmodule *pstSub;

    if (!pstCb)
        return VPP_ERR_PARAM;

    memset(pstCb->astSubmod, 0, sizeof(pstCb->astSubmod));

    for (i = 0; i < VPP_IP_FRC_MAX; i++)
    {
        pstSub = &pstCb->astSubmod[i];
        pstSub->pstCb = pstCb;
        pstSub->eBlk = (t_EVppIpFrcBlk)i;

        u32 = u32VppIp_CmdQueueInit(&pstSub->stCmdQ, FRC_CMD_Q_SZ);
        if (u32)
        {
            LOGE("ERROR: unable to initialze command queue, u32=%u", u32);
            goto ERR_CMD_Q_INIT;
        }
    }

    return VPP_OK;

ERR_CMD_Q_INIT:
    for (; i; i--)
    {
        pstSub = &pstCb->astSubmod[i - 1];
        u32 = u32VppIp_CmdQueueTerm(&pstSub->stCmdQ);
        if (u32)
            LOGE("ERROR: unable to term cmd q (error case), u32=%u", u32);
    }

    return VPP_ERR;
}

static uint32_t u32VppIpFrc_SubmodTerm(t_StVppIpFrcCb *pstCb)
{
    uint32_t u32, i;
    t_StVppFrcSubmodule *pstSub;

    if (!pstCb)
        return VPP_ERR_PARAM;

    for (i = 0; i < VPP_IP_FRC_MAX; i++)
    {
        pstSub = &pstCb->astSubmod[i];
        u32 = u32VppIp_CmdQueueTerm(&pstSub->stCmdQ);
        if (u32)
            LOGE("ERROR: unable to terminate command queue, u32=%u", u32);
    }

    memset(pstCb->astSubmod, 0, sizeof(pstCb->astSubmod));

    return VPP_OK;
}

static uint32_t u32VppIpFrc_SubmodLaunchWorker(t_StVppIpFrcCb *pstCb)
{
    int rc;
    uint32_t i;
    uint32_t u32 = VPP_OK, u32FailIdx = 0;
    t_StVppFrcSubmodule *pstSub;
    t_StVppIpCmd stCmd;
    uint32_t u32ExpFlag;

    if (!pstCb)
        return VPP_ERR_PARAM;

    for (i = 0; i < VPP_IP_FRC_MAX; i++)
    {
        pstSub = &pstCb->astSubmod[i];
        rc = pthread_create(&pstSub->thread, NULL, vpVppIpFrc_SubmodWorker, pstSub);

        if (rc)
        {
            LOGE("ERROR: unable to spawn worker, rc=%d, err=%s", rc, strerror(rc));
            u32FailIdx = i;
            u32 = VPP_ERR;
            break;
        }
    }

    if (u32 != VPP_OK)
    {

        u32ExpFlag = (1 << VPP_IP_CMD_TERM) | (1 << VPP_IP_CMD_THREAD_EXIT);

        pthread_mutex_lock(&pstCb->mutex);
        for (i = 0; i < u32FailIdx; i++)
        {
            pstSub = &pstCb->astSubmod[i];

            // issue term and thread exit
            VPP_FLAG_CLR(pstSub->u32SubmodFlags, u32ExpFlag);

            stCmd.eCmd = VPP_IP_CMD_TERM;
            u32 = u32VppIp_CmdPut(&pstSub->stCmdQ, stCmd);
            if (u32)
            {
                LOGE("ERROR: unable to issue TERM to block=%u", i);
                VPP_FLAG_SET(pstSub->u32SubmodFlags, 1 << VPP_IP_CMD_TERM);
            }

            stCmd.eCmd = VPP_IP_CMD_THREAD_EXIT;
            u32 = u32VppIp_CmdPut(&pstSub->stCmdQ, stCmd);
            if (u32)
            {
                LOGE("ERROR: unable to issue THREAD_EXIT to block=%u", i);
                VPP_FLAG_SET(pstSub->u32SubmodFlags, 1 << VPP_IP_CMD_THREAD_EXIT);
            }
        }

        for (i = 0; i < u32FailIdx; i++)
        {
            pstSub = &pstCb->astSubmod[i];
            while ((pstSub->u32SubmodFlags & u32ExpFlag) != u32ExpFlag)
                pthread_cond_wait(&pstCb->cond, &pstCb->mutex);

            pthread_join(pstSub->thread, NULL);
        }

        pthread_mutex_unlock(&pstCb->mutex);

        return VPP_ERR;
    }

    return VPP_OK;
}

static uint32_t u32VppIpFrc_SubmodJoinWorker(t_StVppIpFrcCb *pstCb)
{
    uint32_t i, u32;
    t_StVppIpCmd stCmd;
    t_StVppFrcSubmodule *pstSub;

    if (!pstCb)
        return VPP_ERR_PARAM;

    stCmd.eCmd = VPP_IP_CMD_THREAD_EXIT;

    for (i = 0; i < VPP_IP_FRC_MAX; i++)
    {
        pstSub = &pstCb->astSubmod[i];

        u32 = u32VppIp_CmdPut(&pstSub->stCmdQ, stCmd);
        if (u32)
        {
            LOGE("ERROR: unable to issue THREAD_EXIT to block=%u", i);
            VPP_FLAG_SET(pstSub->u32SubmodFlags, 1 << VPP_IP_CMD_THREAD_EXIT);
        }
    }

    for (i = 0; i < VPP_IP_FRC_MAX; i++)
    {
        pstSub = &pstCb->astSubmod[i];
        pthread_join(pstSub->thread, NULL);
    }

    return VPP_OK;
}

/************************************************************************
 * Global Functions
 ************************************************************************/

void *vpVppIpFrc_Init(t_StVppCtx *pstCtx, uint32_t u32Flags, t_StVppCallback cbs)
{
    int rc = 0;
    uint32_t u32;
    t_StVppIpFrcCb *pstCb;

    LOG_ENTER_ARGS("pstCtx=%p, u32Flags=0x%x", pstCtx, u32Flags);

    if (!pstCtx)
    {
        LOGE("pstCtx is null");
        LOG_EXIT_RET(NULL);
    }

    if (u32VppIp_ValidateCallbacks(&cbs))
    {
        LOGE("ERROR: invalid callbacks.");
        LOG_EXIT_RET(NULL);
    }

    pstCb = calloc(sizeof(t_StVppIpFrcCb), 1);
    if (!pstCb)
    {
        LOGE("calloc failed for frc context");
        goto ERR_CALLOC;
    }

    u32VppIp_SetBase(pstCtx, u32Flags, cbs, &pstCb->stBase);

    u32 = VPP_IP_PROF_REGISTER(&pstCb->stBase, astFrcStatsCfg, u32FrcStatCount);
    LOGE_IF(u32 != VPP_OK, "ERROR: unable to register stats, u32=%u", u32);

    u32 = u32VppBufPool_Init(&pstCb->stMbiPool);
    if (u32 != VPP_OK)
    {
        LOGE("unable to initialize mbi pool, u32Ret=%u", u32);
        goto ERR_INIT_MBI_POOL;
    }

    rc = sem_init(&pstCb->sem, 0, 0);
    if (rc)
    {
        LOGE("unable to initialize frc mutex");
        goto ERR_SEM_INIT;
    }

    rc = pthread_mutex_init(&pstCb->mutex, NULL);
    if (rc)
    {
        LOGE("unable to initialize frc mutex");
        goto ERR_MUTEX_INIT;
    }

    rc = pthread_cond_init(&pstCb->cond, NULL);
    if (rc)
    {
        LOGE("unable to init condition variable");
        goto ERR_COND_INIT;
    }

    // Setup the callbacks
    pstCb->astCbCtx[VPP_IP_FRC_ME].pstCb = (void *)pstCb;
    pstCb->astCbCtx[VPP_IP_FRC_ME].eBlk = VPP_IP_FRC_ME;

    pstCb->astCbCtx[VPP_IP_FRC_MC].pstCb = (void *)pstCb;
    pstCb->astCbCtx[VPP_IP_FRC_MC].eBlk = VPP_IP_FRC_MC;

    VPP_IP_PROF_START(&pstCb->stBase, FRC_STAT_SUB_INIT);

    u32 = u32VppIpFrc_SubmodInit(pstCb);
    if (u32 != VPP_OK)
    {
        LOGE("ERROR: submod init failed, u32=%u", u32);
        goto ERR_SUBMOD_INIT;
    }

    // Explicitly issue the init command before launching the thread so that
    // when the thread is launched, there is a command for it to process
    // immediately, instead of having to wait for the context to switch back to
    // main thread to issue the commands.
    u32 = u32VppIpFrc_SubmodIssueCmd(pstCb, VPP_IP_CMD_INIT);
    if (u32 != VPP_OK)
    {
        LOGE("ERROR: submod issue cmd failed, u32=%u", u32);
        goto ERR_ISSUE_CMD;
    }

    u32 = u32VppIpFrc_SubmodLaunchWorker(pstCb);
    if (u32 != VPP_OK)
    {
        LOGE("ERROR: submod launch failed, u32=%u", u32);
        goto ERR_SUBMOD_LAUNCH;
    }

    // At this point, all threads have been launched successfully and init has
    // been issued. If there were any issues in launching the thread, the error
    // handling will have picked it up, cleaned up the thread, and the pointers
    // for the submodule's context should be set to NULL.
    u32VppIpFrc_WaitCmdDone(pstCb, VPP_IP_CMD_INIT);

    VPP_IP_PROF_STOP(&pstCb->stBase, FRC_STAT_SUB_INIT);

    if (!FRC_CTX_GET(ME, pstCb) || !FRC_CTX_GET(MC, pstCb))
    {
        LOGE("unable to init, me=%p, mc=%p", FRC_CTX_GET(ME, pstCb),
             FRC_CTX_GET(MC, pstCb));
        goto ERR_INIT;
    }
    LOGI("init, me=%p, mc=%p", FRC_CTX_GET(ME, pstCb),
         FRC_CTX_GET(MC, pstCb));

    pthread_mutex_lock(&pstCb->mutex);
    VPP_IP_STATE_SET(pstCb, VPP_IP_STATE_INITED);
    pthread_mutex_unlock(&pstCb->mutex);

    LOG_EXIT_RET(pstCb);

ERR_INIT:
    if (FRC_CTX_GET(ME, pstCb))
    {
        LOGI("terming ME");
        VPP_FRC_TERM(ME, pstCb);
    }
    if (FRC_CTX_GET(MC, pstCb))
    {
        LOGI("terming MC");
        VPP_FRC_TERM(MC, pstCb);
    }

    u32 = u32VppIpFrc_SubmodJoinWorker(pstCb);
    LOGE_IF(u32 != VPP_OK, "ERROR: unable to join worker (error case), u32=%u", u32);

ERR_SUBMOD_LAUNCH:
ERR_ISSUE_CMD:
    u32 = u32VppIpFrc_SubmodTerm(pstCb);
    LOGE_IF(u32 != VPP_OK, "ERROR: unable to submod term (error case), u32=%u", u32);

ERR_SUBMOD_INIT:

ERR_COND_INIT:
    LOGI("destroying mutex");
    pthread_mutex_destroy(&pstCb->mutex);

ERR_MUTEX_INIT:
    LOGI("destroying semaphore");
    sem_destroy(&pstCb->sem);

ERR_SEM_INIT:
    u32VppBufPool_Term(&pstCb->stMbiPool);

ERR_INIT_MBI_POOL:
    u32 = VPP_IP_PROF_UNREGISTER(&pstCb->stBase);
    LOGE_IF(u32 != VPP_OK, "ERROR: unable to unregister stats, u32=%u", u32);

    free (pstCb);

ERR_CALLOC:
    LOG_EXIT_RET(NULL);
}

void vVppIpFrc_Term(void *ctx)
{
    int rc = 0;
    uint32_t u32;
    t_StVppIpFrcCb *pstCb;

    LOG_ENTER();

    VPP_RET_VOID_IF_NULL(ctx);
    pstCb = FRC_CB_GET(ctx);
    VPP_RET_VOID_IF_NULL(pstCb);

    if (!VPP_IP_STATE_EQUAL(pstCb, VPP_IP_STATE_INITED) &&
        !VPP_IP_STATE_EQUAL(pstCb, VPP_IP_STATE_ACTIVE))
    {
        LOGE("%s() called in an invalid state, state=%u", __func__,
             pstCb->eState);
        return;
    }

    if (VPP_IP_STATE_EQUAL(pstCb, VPP_IP_STATE_ACTIVE))
    {
        u32 = u32VppIpFrc_Close(ctx);
        if (u32)
            LOGE("%s() failed to close ME. ret=%u", __func__, u32);
    }

    VPP_IP_PROF_START(&pstCb->stBase, FRC_STAT_SUB_TERM);
    u32VppIpFrc_SubmodIssueCmd(pstCb, VPP_IP_CMD_TERM);
    u32VppIpFrc_WaitCmdDone(pstCb, VPP_IP_CMD_TERM);
    u32VppIpFrc_SubmodJoinWorker(pstCb);
    u32VppIpFrc_SubmodTerm(pstCb);
    VPP_IP_PROF_STOP(&pstCb->stBase, FRC_STAT_SUB_TERM);

    u32VppBufPool_Term(&pstCb->stMbiPool);

    rc = pthread_cond_destroy(&pstCb->cond);
    if (rc)
        LOGE("pthread_cond_destroy failed: %d --> %s", rc, strerror(rc));

    rc = pthread_mutex_destroy(&pstCb->mutex);
    if (rc)
        LOGE("pthread_mutex_destroy failed: %d --> %s", rc, strerror(rc));

    rc = sem_destroy(&pstCb->sem);
    if (rc)
        LOGE("sem_destroy failed: %d --> %s", rc, strerror(rc));

    vVppIpFrc_DumpStats(pstCb);

    u32 = VPP_IP_PROF_UNREGISTER(&pstCb->stBase);
    LOGE_IF(u32 != VPP_OK, "ERROR: unable to unregister stats, u32=%u", u32);

    free(pstCb);

    LOG_EXIT();
}

uint32_t u32VppIpFrc_Open(void *ctx)
{
    uint32_t u32RetMc, u32RetMe, u32Ret;

    uint32_t au32Ret[VPP_IP_FRC_MAX] = {0};

    LOG_ENTER_ARGS("ctx=%p", ctx);

    t_StVppIpFrcCb *pstCb;

    VPP_RET_IF_NULL(ctx, VPP_ERR_PARAM);
    pstCb = FRC_CB_GET(ctx);

    pthread_mutex_lock(&pstCb->mutex);
    if (!VPP_IP_STATE_EQUAL(pstCb, VPP_IP_STATE_INITED))
    {
        pthread_mutex_unlock(&pstCb->mutex);
        LOG_EXIT_RET(VPP_ERR_STATE);
    }

    u32Ret = u32VppIpFrc_ValidateConfig(pstCb->stBase.stInputParam, pstCb->stBase.stOutputParam);
    if (u32Ret != VPP_OK)
    {
        LOGE("u32VppIpFrc_ValidateConfig() failed ret=%u", u32Ret);
        pthread_mutex_unlock(&pstCb->mutex);
        LOG_EXIT_RET(u32Ret);
    }

    VPP_IP_STATE_SET(pstCb, VPP_IP_STATE_OPENING);
    pthread_mutex_unlock(&pstCb->mutex);

    VPP_IP_PROF_START(&pstCb->stBase, FRC_STAT_SUB_OPEN);
    u32VppIpFrc_SubmodIssueCmd(pstCb, VPP_IP_CMD_OPEN);
    u32VppIpFrc_WaitCmdDone(pstCb, VPP_IP_CMD_OPEN);
    u32VppIpFrc_GetCmdResult(pstCb, VPP_IP_CMD_OPEN, au32Ret,
                             sizeof(au32Ret) / sizeof(uint32_t));
    u32RetMe = au32Ret[VPP_IP_FRC_ME];
    u32RetMc = au32Ret[VPP_IP_FRC_MC];
    VPP_IP_PROF_STOP(&pstCb->stBase, FRC_STAT_SUB_OPEN);

    if (u32RetMe != VPP_OK || u32RetMc != VPP_OK)
    {
        LOGE("%s: u32RetMe=%u, u32RetMc=%u", __func__, u32RetMe, u32RetMc);
        goto ERROR_OPEN;
    }

    u32Ret = u32VppIpFrc_GetBufferReqs(pstCb);
    if (u32Ret != VPP_OK)
    {
        LOGE("Unable to get buffer requirements, u32Ret=%u", u32Ret);
        goto ERR_GET_BUF_REQ;
    }

    VPP_IP_PROF_START(&pstCb->stBase, FRC_STAT_MBI_ALLOC);
    u32Ret = u32VppIpFrc_AllocateMbiBuffers(pstCb);
    VPP_IP_PROF_STOP(&pstCb->stBase, FRC_STAT_MBI_ALLOC);

    if (u32Ret != VPP_OK)
    {
        LOGE("Unable to allocate mbi buffers, u32Ret=%u", u32Ret);
        goto ERR_ALLOCATE_MBI;
    }

    u32Ret = u32VppIpFrc_QueueMbiBuffers(pstCb);
    if (u32Ret != VPP_OK)
    {
        LOGE("Unable to queue mbi buffers, u32Ret=%u", u32Ret);
        goto ERR_QUEUE_MBI;
    }

    pthread_mutex_lock(&pstCb->mutex);
    VPP_IP_STATE_SET(pstCb, VPP_IP_STATE_ACTIVE);
    pthread_mutex_unlock(&pstCb->mutex);

    LOG_EXIT_RET(VPP_OK);

ERR_QUEUE_MBI:

    VPP_IP_PROF_START(&pstCb->stBase, FRC_STAT_MBI_FREE);
    u32Ret = u32VppIpFrc_FreeMbiBuffers(pstCb);
    VPP_IP_PROF_STOP(&pstCb->stBase, FRC_STAT_MBI_FREE);
    if (u32Ret != VPP_OK)
        LOGE("unable to free mbi buffers, u32=%u", u32Ret);

ERR_ALLOCATE_MBI:
ERR_GET_BUF_REQ:
ERROR_OPEN:
    if (u32RetMe)
        VPP_FRC_CLOSE(MC, pstCb, u32RetMe);

    if (u32RetMc)
        VPP_FRC_CLOSE(ME, pstCb, u32RetMc);

    pthread_mutex_lock(&pstCb->mutex);
    VPP_IP_STATE_SET(pstCb, VPP_IP_STATE_INITED);
    pthread_mutex_unlock(&pstCb->mutex);

    LOG_EXIT_RET(VPP_ERR);
}

uint32_t u32VppIpFrc_Close(void *ctx)
{
    LOG_ENTER_ARGS("ctx=%p", ctx);

    t_StVppIpFrcCb *pstCb;
    uint32_t u32;
    uint32_t au32Ret[VPP_IP_FRC_MAX] = {0};

    VPP_RET_IF_NULL(ctx, VPP_ERR_PARAM);
    pstCb = FRC_CB_GET(ctx);

    pthread_mutex_lock(&pstCb->mutex);
    if (!VPP_IP_STATE_EQUAL(pstCb, VPP_IP_STATE_ACTIVE))
    {
        pthread_mutex_unlock(&pstCb->mutex);
        return VPP_ERR_STATE;
    }
    VPP_IP_STATE_SET(pstCb, VPP_IP_STATE_CLOSING);
    pthread_mutex_unlock(&pstCb->mutex);

    u32 = u32VppIpFrc_FlushMbiBuffers(pstCb);
    if (u32 != VPP_OK)
    {
        LOGE("unable to flush mbi buffers, u32=%u", u32);
    }

    VPP_IP_PROF_START(&pstCb->stBase, FRC_STAT_MBI_FREE);
    u32 = u32VppIpFrc_FreeMbiBuffers(pstCb);
    VPP_IP_PROF_STOP(&pstCb->stBase, FRC_STAT_MBI_FREE);
    if (u32 != VPP_OK)
    {
        LOGE("unable to free mbi buffers, u32=%u", u32);
    }

    VPP_IP_PROF_START(&pstCb->stBase, FRC_STAT_SUB_CLOSE);
    u32VppIpFrc_SubmodIssueCmd(pstCb, VPP_IP_CMD_CLOSE);
    u32VppIpFrc_WaitCmdDone(pstCb, VPP_IP_CMD_CLOSE);
    u32VppIpFrc_GetCmdResult(pstCb, VPP_IP_CMD_CLOSE, au32Ret,
                             sizeof(au32Ret) / sizeof(uint32_t));
    VPP_IP_PROF_STOP(&pstCb->stBase, FRC_STAT_SUB_CLOSE);

    if (au32Ret[VPP_IP_FRC_ME] != VPP_OK || au32Ret[VPP_IP_FRC_ME] != VPP_OK)
    {
        LOGE("unable to close, u32Me=%u, u32Mc=%u", au32Ret[VPP_IP_FRC_ME],
             au32Ret[VPP_IP_FRC_MC]);
    }

    pthread_mutex_lock(&pstCb->mutex);
    VPP_IP_STATE_SET(pstCb, VPP_IP_STATE_INITED);
    pthread_mutex_unlock(&pstCb->mutex);

    return VPP_OK;
}

uint32_t u32VppIpFrc_SetParam(void *ctx, enum vpp_port ePort,
                              struct vpp_port_param stParam)
{
    uint32_t u32RetMc, u32RetMe;
    LOG_ENTER_ARGS("ctx=%p, port=%u", ctx, ePort);

    uint32_t u32Ret = VPP_OK;
    t_StVppIpFrcCb *pstCb;

    VPP_RET_IF_NULL(ctx, VPP_ERR_PARAM);
    pstCb = FRC_CB_GET(ctx);

    if (!VPP_IP_STATE_EQUAL(pstCb, VPP_IP_STATE_INITED))
        return VPP_ERR_STATE;

    if (ePort != VPP_PORT_INPUT && ePort != VPP_PORT_OUTPUT)
        return VPP_ERR_PARAM;

    if (u32VppIpFrc_ValidatePortParam(stParam))
        return VPP_ERR_PARAM;

    pthread_mutex_lock(&pstCb->mutex);

    if (ePort == VPP_PORT_INPUT)
        pstCb->stBase.stInputParam = stParam;
    else if (ePort == VPP_PORT_OUTPUT)
        pstCb->stBase.stOutputParam = stParam;

    pthread_mutex_unlock(&pstCb->mutex);

    LOG_PARAM_PTR(I, &pstCb->stBase.stInputParam, &pstCb->stBase.stOutputParam);

    VPP_FRC_SET_PARAM(MC, pstCb, ePort, stParam, u32RetMc);
    VPP_FRC_SET_PARAM(ME, pstCb, ePort, stParam, u32RetMe);

    if (u32RetMe != VPP_OK || u32RetMc != VPP_OK)
    {
        LOGE("%s: u32RetMe=%u, u32RetMc=%u", __func__, u32RetMe, u32RetMc);
        LOG_EXIT_RET(VPP_ERR);
    }

    LOG_EXIT_RET(u32Ret);
}

uint32_t u32VppIpFrc_SetCtrl(void *ctx, struct hqv_control ctrl)
{
    uint32_t u32RetMc, u32RetMe;
    LOG_ENTER_ARGS("ctx=%p", ctx);

    t_StVppIpFrcCb *pstCb;

    VPP_RET_IF_NULL(ctx, VPP_ERR_PARAM);
    pstCb = FRC_CB_GET(ctx);

    // TODO: store the control, but for now, just shove it to the IP blocks.

    VPP_FRC_SET_CTRL(ME, pstCb, ctrl, u32RetMe);
    VPP_FRC_SET_CTRL(MC, pstCb, ctrl, u32RetMc);

    if (u32RetMe != VPP_OK || u32RetMc != VPP_OK)
    {
        LOGE("%s: u32RetMe=%u, u32RetMc=%u", __func__, u32RetMe, u32RetMc);

        if (u32RetMe == VPP_ERR_PARAM || u32RetMe == VPP_ERR_INVALID_CFG ||
            u32RetMc == VPP_ERR_PARAM || u32RetMc == VPP_ERR_INVALID_CFG)
            return VPP_ERR_PARAM;
        return VPP_ERR;
    }

    return VPP_OK;
}

uint32_t u32VppIpFrc_GetBufferRequirements(void *ctx,
                                           t_StVppIpBufReq *pstInputBufReq,
                                           t_StVppIpBufReq *pstOutputBufReq)
{
    uint32_t u32;
    t_StVppIpFrcCb *pstCb;

    VPP_RET_IF_NULL(ctx, VPP_ERR_PARAM);
    pstCb = FRC_CB_GET(ctx);

    if (!VPP_IP_STATE_EQUAL(pstCb, VPP_IP_STATE_ACTIVE) &&
        !VPP_IP_STATE_EQUAL(pstCb, VPP_IP_STATE_INITED))
        return VPP_ERR_STATE;

    u32 = u32VppIpFrc_GetBufferReqs(pstCb);
    if (u32)
    {
        LOGE("%s: getting bufreqs from submodules failed. ret=%u", __func__,
             u32);
        return VPP_ERR;
    }

    LOGI("%s: ME: i_px=%u, i_ex=%u, i_min=%u, o_px=%u, o_ex=%u, o_min=%u",
         __func__, pstCb->stMeInputReq.u32PxSz, pstCb->stMeInputReq.u32ExSz,
         pstCb->stMeInputReq.u32MinCnt, pstCb->stMeOutputReq.u32PxSz,
         pstCb->stMeOutputReq.u32ExSz, pstCb->stMeOutputReq.u32MinCnt);
    LOGI("%s: MC: i_px=%u, i_ex=%u, i_min=%u, o_px=%u, o_ex=%u, o_min=%u",
         __func__, pstCb->stMcInputReq.u32PxSz, pstCb->stMcInputReq.u32ExSz,
         pstCb->stMcInputReq.u32MinCnt, pstCb->stMcOutputReq.u32PxSz,
         pstCb->stMcOutputReq.u32ExSz, pstCb->stMcOutputReq.u32MinCnt);

    if (pstCb->stMeOutputReq.u32MinCnt < pstCb->stMcInputReq.u32MinCnt)
    {
        // Not sure how to deal with this case without knowing the mul and add
        // factors of the FRC algo.
        VPP_NOT_IMPLEMENTED();
        return VPP_ERR;
    }

    if (pstInputBufReq)
    {
        pstInputBufReq->u32PxSz = pstCb->stMeInputReq.u32PxSz;
        pstInputBufReq->u32ExSz = pstCb->stMeInputReq.u32ExSz;
        pstInputBufReq->u32MinCnt = pstCb->stMeInputReq.u32MinCnt;
    }
    if (pstOutputBufReq)
    {
        pstOutputBufReq->u32PxSz = pstCb->stMcOutputReq.u32PxSz;
        pstOutputBufReq->u32ExSz = pstCb->stMcOutputReq.u32ExSz;
        pstOutputBufReq->u32MinCnt = pstCb->stMcOutputReq.u32MinCnt;
    }

    return VPP_OK;
}

uint32_t u32VppIpFrc_QueueBuf(void *ctx, enum vpp_port ePort,
                              t_StVppBuf *pBuf)
{
    uint32_t u32, u32Err = VPP_OK;
    uint32_t u32AvailSz;
    LOG_ENTER_ARGS("ctx=%p, ePort=%u", ctx, ePort);

    t_StVppIpFrcCb *pstCb;

    if (ePort != VPP_PORT_INPUT && ePort != VPP_PORT_OUTPUT)
        return VPP_ERR_PARAM;

    VPP_RET_IF_NULL(ctx, VPP_ERR_PARAM);
    VPP_RET_IF_NULL(pBuf, VPP_ERR_PARAM);
    pstCb = FRC_CB_GET(ctx);

    if (!VPP_IP_STATE_EQUAL(pstCb, VPP_IP_STATE_ACTIVE))
        return VPP_ERR_STATE;

    LOGI("QueueBuf: received pBuf=%p, eBufPxType=%u", pBuf, pBuf->eBufPxType);

    u32AvailSz = pBuf->stPixel.u32AllocLen;

    if (ePort == VPP_PORT_INPUT)
    {
        if (pstCb->stMeInputReq.u32PxSz && u32AvailSz < pstCb->stMeInputReq.u32PxSz)
            goto ERR_INSUFFICIENT_SZ;

        if (FRC_GET_STAT(pstCb, InQCnt))
            VPP_IP_PROF_STOP(&pstCb->stBase, FRC_STAT_QBUF_INPUT);
        VPP_IP_PROF_START(&pstCb->stBase, FRC_STAT_QBUF_INPUT);

        vVppIpFrc_DumpBuffer(pstCb, pBuf, VPP_IP_FRC_BUF_CORE_IN);
        FRC_STATS(pstCb, InQCnt);
        VPP_FRC_CRC_BUF(pstCb, pBuf);
        u32 = u32VppIpFrc_QueueBufInternal(pstCb, VPP_IP_FRC_ME,
                                           VPP_PORT_INPUT, pBuf, VPP_FALSE);
        if (u32 != VPP_OK)
        {
            LOGE("unable to queue buf to ME input, u32=%u", u32);
            FRC_STATS(pstCb, MeQInputYuvErr);
            u32Err = VPP_ERR;
        }
        else
        {
            FRC_STATS(pstCb, MeQInputYuv);
        }
    }
    else if (ePort == VPP_PORT_OUTPUT)
    {
        if (pstCb->stMeOutputReq.u32PxSz && u32AvailSz < pstCb->stMeOutputReq.u32PxSz)
            goto ERR_INSUFFICIENT_SZ;

        if (FRC_GET_STAT(pstCb, OutQCnt))
            VPP_IP_PROF_STOP(&pstCb->stBase, FRC_STAT_QBUF_OUTPUT);
        VPP_IP_PROF_START(&pstCb->stBase, FRC_STAT_QBUF_OUTPUT);

        FRC_STATS(pstCb, OutQCnt);
        u32 = u32VppIpFrc_QueueBufInternal(pstCb, VPP_IP_FRC_MC,
                                           VPP_PORT_OUTPUT, pBuf, VPP_FALSE);
        if (u32 != VPP_OK)
        {
            LOGE("unable to queue buf to MC output, u32=%u", u32);
            FRC_STATS(pstCb, McQOutputYuvErr);
            u32Err = VPP_ERR;
        }
        else
        {
            FRC_STATS(pstCb, McQOutputYuv);
        }
    }
    else
    {
        LOGE("received buffer on invalid port, port=%u", ePort);
        u32Err = VPP_ERR_PARAM;
    }

    LOG_EXIT_RET(u32Err);

ERR_INSUFFICIENT_SZ:
    LOGE("insufficient buf size received on port=%u, in_req=%u, out_req=%u",
         ePort, pstCb->stMeInputReq.u32PxSz, pstCb->stMcOutputReq.u32PxSz);
    LOG_BUF_PX(pBuf);
    LOG_BUF_EX(pBuf);

    if (ePort == VPP_PORT_INPUT)
    {
        u32VppIp_CbBufDone(&pstCb->stBase.stCb, VPP_PORT_INPUT, pBuf);
        FRC_STATS(pstCb, IBDCnt);
    }
    else
    {
        u32VppIp_CbBufDone(&pstCb->stBase.stCb, VPP_PORT_OUTPUT, pBuf);
        FRC_STATS(pstCb, OBDCnt);
    }

    LOG_EXIT_RET(VPP_OK);
}

uint32_t u32VppIpFrc_Flush(void *ctx, enum vpp_port ePort)
{
    uint32_t u32, u32Err = VPP_OK;
    uint32_t bFlushMe = VPP_FALSE;
    uint32_t bFlushMc = VPP_FALSE;
    uint32_t u32FlushFlag = 0;

    LOG_ENTER_ARGS("ctx=%p, ePort=%u", ctx, ePort);

    t_StVppIpFrcCb *pstCb;

    if (ePort != VPP_PORT_INPUT && ePort != VPP_PORT_OUTPUT)
        return VPP_ERR_PARAM;

    VPP_RET_IF_NULL(ctx, VPP_ERR_PARAM);
    pstCb = FRC_CB_GET(ctx);

    pthread_mutex_lock(&pstCb->mutex);

    if (ePort == VPP_PORT_INPUT)
    {
        VPP_FRC_FLUSH(ME, pstCb, ePort, u32);
        if (u32 != VPP_OK)
        {
            LOGE("unable to flush ME input port, u32=%u", u32);
            u32Err = VPP_ERR;
        }
        else
        {
            pstCb->u32FlushMeInCnt++;
            VPP_FLAG_SET(pstCb->u32FlushFlags, VPP_FRC_FLUSH_ME_IN);
        }

        VPP_FRC_FLUSH(MC, pstCb, ePort, u32);
        if (u32 != VPP_OK)
        {
            LOGE("unable to flush MC input port, u32=%u", u32);
            u32Err = VPP_ERR;
        }
        else
        {
            pstCb->u32FlushMcInCnt++;
            VPP_FLAG_SET(pstCb->u32FlushFlags, VPP_FRC_FLUSH_MC_IN);
        }

        VPP_FLAG_SET(pstCb->u32FlushFlags, VPP_FRC_FLUSH_INPUT);
    }
    else if (ePort == VPP_PORT_OUTPUT)
    {
        VPP_FRC_FLUSH(MC, pstCb, ePort, u32);
        if (u32 != VPP_OK)
        {
            LOGE("unable to flush ME input port, u32=%u", u32);
            u32Err = VPP_ERR;
        }
        else
        {
            pstCb->u32FlushMcOutCnt++;
            VPP_FLAG_SET(pstCb->u32FlushFlags, VPP_FRC_FLUSH_MC_OUT);
        }

        VPP_FLAG_SET(pstCb->u32FlushFlags, VPP_FRC_FLUSH_OUTPUT);
    }
    pthread_mutex_unlock(&pstCb->mutex);

    LOG_EXIT_RET(u32Err);
}

uint32_t u32VppIpFrc_Drain(void *ctx)
{
    uint32_t u32;

    LOG_ENTER_ARGS("ctx=%p", ctx);

    t_StVppIpFrcCb *pstCb;

    VPP_RET_IF_NULL(ctx, VPP_ERR_PARAM);
    pstCb = FRC_CB_GET(ctx);

    if (!VPP_IP_STATE_EQUAL(pstCb, VPP_IP_STATE_ACTIVE))
        return VPP_ERR_STATE;

    pthread_mutex_lock(&pstCb->mutex);

    VPP_FRC_DRAIN(ME, pstCb, u32);

    if (u32 != VPP_OK)
    {
        LOGE("unable to drain ME block, u32Ret=%u", u32);
    }
    else
    {
        pstCb->u32DrainMeCnt++;
        VPP_FLAG_SET(pstCb->u32DrainFlags, VPP_FRC_DRAIN_PENDING);
        VPP_FLAG_SET(pstCb->u32DrainFlags, VPP_FRC_DRAIN_PENDING_ME);
    }
    pthread_mutex_unlock(&pstCb->mutex);

    return u32;
}

uint32_t u32VppIpFrc_Reconfigure(void *ctx,
                                 struct vpp_port_param in_param,
                                 struct vpp_port_param out_param)
{
    uint32_t u32McInQCnt, u32MeInQCnt, u32RetMc, u32RetMe, u32Ret;
    uint32_t au32Ret[VPP_IP_FRC_MAX] = {0};

    LOG_ENTER_ARGS("ctx=%p", ctx);

    t_StVppIpFrcCb *pstCb;

    VPP_RET_IF_NULL(ctx, VPP_ERR_PARAM);
    pstCb = FRC_CB_GET(ctx);

    pthread_mutex_lock(&pstCb->mutex);
    if (!VPP_IP_STATE_EQUAL(pstCb, VPP_IP_STATE_ACTIVE))
    {
        pthread_mutex_unlock(&pstCb->mutex);
        return VPP_ERR_STATE;
    }
    u32MeInQCnt = u32VppIpFrc_GetBufferHoldCnt(pstCb, VPP_IP_FRC_ME, VPP_PORT_INPUT,
                                               eVppBufPxDataType_Raw);
    u32McInQCnt = u32VppIpFrc_GetBufferHoldCnt(pstCb, VPP_IP_FRC_MC, VPP_PORT_INPUT,
                                               eVppBufPxDataType_Raw);
    pthread_mutex_unlock(&pstCb->mutex);

    if (u32MeInQCnt || u32McInQCnt)
    {
        LOGE("Reconfig but ME (%u) and MC (%u) input queues aren't both empty",
             u32MeInQCnt, u32McInQCnt);
        return VPP_ERR_STATE;
    }

    if (u32VppIpFrc_ValidateConfig(in_param, out_param) != VPP_OK)
        return VPP_ERR_PARAM;

    u32Ret = u32VppIpFrc_FlushMbiBuffers(pstCb);
    LOGE_IF(u32Ret, "Unable to flush mbi buffers, u32=%u", u32Ret);

    pstCb->stBase.stInputParam = in_param;
    pstCb->stBase.stOutputParam = out_param;

    VPP_IP_PROF_START(&pstCb->stBase, FRC_STAT_SUB_RECONFIGURE);
    u32VppIpFrc_SubmodIssueCmd(pstCb, VPP_IP_CMD_RECONFIGURE);
    u32VppIpFrc_WaitCmdDone(pstCb, VPP_IP_CMD_RECONFIGURE);
    u32VppIpFrc_GetCmdResult(pstCb, VPP_IP_CMD_RECONFIGURE, au32Ret,
                             sizeof(au32Ret) / sizeof(uint32_t));
    u32RetMe = au32Ret[VPP_IP_FRC_ME];
    u32RetMc = au32Ret[VPP_IP_FRC_MC];
    VPP_IP_PROF_STOP(&pstCb->stBase, FRC_STAT_SUB_RECONFIGURE);

    if (u32RetMe != VPP_OK || u32RetMc != VPP_OK)
    {
        // Try Close/Open if Reconfigure API fails. This reallocates MBI too.
        LOGE("Reconfigure Error: u32RetMe=%u, u32RetMc=%u. Try Close/Open.",
             u32RetMe, u32RetMc);
        u32Ret = u32VppIpFrc_Close(ctx);
        if (u32Ret != VPP_OK)
        {
            LOGE("Close Error u32Ret=%u", u32Ret);
            goto ERROR_RECONFIGURE;
        }

        VPP_FRC_SET_PARAM(MC, pstCb, VPP_PORT_INPUT, in_param, u32RetMc);
        VPP_FRC_SET_PARAM(ME, pstCb, VPP_PORT_INPUT, in_param, u32RetMe);
        if (u32RetMe != VPP_OK || u32RetMc != VPP_OK)
        {
            LOGE("SetParam Input Error: u32RetMe=%u, u32RetMc=%u", u32RetMe, u32RetMc);
            goto ERROR_RECONFIGURE;
        }

        VPP_FRC_SET_PARAM(MC, pstCb, VPP_PORT_OUTPUT, out_param, u32RetMc);
        VPP_FRC_SET_PARAM(ME, pstCb, VPP_PORT_OUTPUT, out_param, u32RetMe);
        if (u32RetMe != VPP_OK || u32RetMc != VPP_OK)
        {
            LOGE("SetParam Output Error: u32RetMe=%u, u32RetMc=%u", u32RetMe, u32RetMc);
            goto ERROR_RECONFIGURE;
        }

        u32Ret = u32VppIpFrc_Open(ctx);
        if (u32Ret != VPP_OK)
        {
            LOGE("Open Error u32Ret=%u", u32Ret);
            goto ERROR_RECONFIGURE;
        }
    }
    else
    {
        uint32_t u32MinCnt, u32PxSz, u32ExSz;

        u32Ret = u32VppIpFrc_GetBufferReqs(pstCb);
        if (u32Ret != VPP_OK)
        {
            LOGE("Unable to get buffer requirements, u32Ret=%u", u32Ret);
            goto ERR_GET_BUF_REQ;
        }

        u32MinCnt = VPP_MAX(pstCb->stMeOutputReq.u32MinCnt,
                            pstCb->stMcInputReq.u32MinCnt);

        u32PxSz = VPP_MAX(pstCb->stMeOutputReq.u32PxSz,
                          pstCb->stMcInputReq.u32PxSz);

        u32ExSz = VPP_MAX(pstCb->stMeOutputReq.u32ExSz,
                          pstCb->stMcInputReq.u32ExSz);

        if (u32MinCnt > pstCb->stMbiBufAlloc.u32MinCnt ||
            u32PxSz > pstCb->stMbiBufAlloc.u32PxSz ||
            u32ExSz > pstCb->stMbiBufAlloc.u32ExSz)
        {
            LOGI("Reallocating MBI buffers during reconfig");
            VPP_IP_PROF_START(&pstCb->stBase, FRC_STAT_MBI_FREE);
            u32Ret = u32VppIpFrc_FreeMbiBuffers(pstCb);
            VPP_IP_PROF_STOP(&pstCb->stBase, FRC_STAT_MBI_FREE);
            LOGE_IF(u32Ret, "Unable to free mbi buffers, u32=%u", u32Ret);

            VPP_IP_PROF_START(&pstCb->stBase, FRC_STAT_MBI_ALLOC);
            u32Ret = u32VppIpFrc_AllocateMbiBuffers(pstCb);
            VPP_IP_PROF_STOP(&pstCb->stBase, FRC_STAT_MBI_ALLOC);

            if (u32Ret != VPP_OK)
            {
                LOGE("Unable to allocate mbi buffers, u32Ret=%u", u32Ret);
                goto ERR_ALLOCATE_MBI;
            }
        }

        u32Ret = u32VppIpFrc_QueueMbiBuffers(pstCb);
        if (u32Ret != VPP_OK)
        {
            LOGE("Unable to queue mbi buffers, u32Ret=%u", u32Ret);
            goto ERR_QUEUE_MBI;
        }
    }
    LOG_EXIT_RET(VPP_OK);

ERR_QUEUE_MBI:
    VPP_IP_PROF_START(&pstCb->stBase, FRC_STAT_MBI_FREE);
    u32Ret = u32VppIpFrc_FreeMbiBuffers(pstCb);
    VPP_IP_PROF_STOP(&pstCb->stBase, FRC_STAT_MBI_FREE);
    LOGE_IF(u32Ret, "unable to free mbi buffers, u32=%u", u32Ret);

ERR_ALLOCATE_MBI:
ERR_GET_BUF_REQ:
ERROR_RECONFIGURE:
    LOG_EXIT_RET(VPP_ERR);
}
