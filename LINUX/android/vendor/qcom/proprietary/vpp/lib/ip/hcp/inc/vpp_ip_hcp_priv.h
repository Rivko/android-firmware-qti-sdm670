/*!
 * @file vpp_ip_hcp_priv.h
 *
 * @cr
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.

 * @services
 */

#ifndef _VPP_IP_HCP_PRIV_H_
#define _VPP_IP_HCP_PRIV_H_

#include <stdlib.h>
#include <string.h>
#include "vpp_ion.h"
#include "vpp_ip.h"
#include "vpp_buf.h"
#include "vpp_reg.h"

#include "hcp_hfi.h"
#include "hcp_rpc.h"

#define BIT(_n) (1 << (_n))

#define HCP_USE_DMA_BUF
// #define HCP_DBG_WRITE_DUMP_BUF
// #define HCP_DBG_PA_CNTS
// #define HCP_DISABLE_ASSERTS
// #define HCP_DBG_GLOBAL_STARTUP
// #define HCP_DBG_UBWC_STATS

#define HCP_DUMP_NM_LEN 256
#define HCP_DUMP_BUF_IN_NM                  "hcp_%p_in.yuv"
#define HCP_DUMP_BUF_OUT_NM                 "hcp_%p_out.yuv"

#define HCP_PROPERTY_FWLOGFLAGS             "vendor.media.vpp.hcp.logflags"

enum {
    eHcpFlag_PortParam,

    eHcpFlag_AlgoMeas,
    eHcpFlag_AlgoProc,

    eHcpFlag_AlgoAuto,
    eHcpFlag_AlgoLtmSat,
    eHcpFlag_AlgoLtmAce,
    eHcpFlag_AlgoEar,
    eHcpFlag_AlgoDe,

    eHcpFlag_AlgoDemo,
};

#define HCP_FLAG_PORT_PARAM     BIT(eHcpFlag_PortParam)

#define HCP_FLAG_ALGO_MEAS      BIT(eHcpFlag_AlgoMeas)
#define HCP_FLAG_ALGO_PROC      BIT(eHcpFlag_AlgoProc)

#define HCP_FLAG_ALGO_AUTO      BIT(eHcpFlag_AlgoAuto)
#define HCP_FLAG_ALGO_LTM_SAT   BIT(eHcpFlag_AlgoLtmSat)
#define HCP_FLAG_ALGO_LTM_ACE   BIT(eHcpFlag_AlgoLtmAce)
#define HCP_FLAG_ALGO_EAR       BIT(eHcpFlag_AlgoEar)
#define HCP_FLAG_ALGO_DE        BIT(eHcpFlag_AlgoDe)

#define HCP_FLAG_ALGO_DEMO      BIT(eHcpFlag_AlgoDemo)

enum {
    eIpBufFlag_Mapped = eVppBufFlag_IpStart,
};

#define VPP_BUF_FLAG_HCP_MAPPED BIT(eIpBufFlag_Mapped)

#define HCP_HNDL_UNINIT         ((remote_handle64)(-1))
#define HCP_HNDL_GLOB(_pg)      ((_pg)->hndl)
#define HCP_HNDL_SESS(_ps)      (HCP_HNDL_GLOB((_ps)->pstGlobal))

#define HCP_HNDL_GLOB_IS_VALID(_pg) (HCP_HNDL_GLOB(_pg) != HCP_HNDL_UNINIT)
#define HCP_HNDL_SESS_IS_VALID(_ps) (HCP_HNDL_SESS(_ps) != HCP_HNDL_UNINIT)

#define HCP_MAX_SESSIONS        8
#define HCP_CMD_Q_SZ            15
#define HCP_MSG_Q_NODES         (HCP_MAX_SESSIONS * 16)
#define HCP_RAW_PKT_PER_SESS    2
#define HCP_BUF_ATTR_PER_SESS   2

#define HCP_MAX_IN_BUF_PER_TRANS  1
#define HCP_MAX_OUT_BUF_PER_TRANS 1

#if (HCP_MAX_IN_BUF_PER_TRANS + HCP_MAX_OUT_BUF_PER_TRANS) > HCP_RAW_PKT_PER_SESS
#error insufficient raw packets
#endif

#if (HCP_MAX_IN_BUF_PER_TRANS + HCP_MAX_OUT_BUF_PER_TRANS) > HCP_BUF_ATTR_PER_SESS
#error insufficient buf attrs
#endif

#define HCP_CB_GET(ctx)         (ctx ? (t_StVppIpHcpSessionCb *)ctx : NULL)

#define HCP_SYS_TUNE_COOKIE_H   ((uint32_t)(0xABCD << 16))
#define HCP_SYS_TUNE_COOKIE_L   ((uint32_t)(0xDCBA << 16))

#define HCP_INTP_COOKIE_H(_p)   ((uint32_t)((0xBEEF << 16) | (_p)->u32SessId))
#define HCP_INTP_COOKIE_L(_p)   ((uint32_t)((0xDEAD << 16) | (_p)->stDrvCtx.id))
#define HCP_INTM_COOKIE_H(_p)   ((uint32_t)((0xCAFE << 16) | (_p)->u32SessId))
#define HCP_INTM_COOKIE_L(_p)   ((uint32_t)((0xBABE << 16) | (_p)->stDrvCtx.id))

#define HCP_P_COOKIE_H(_pcb, _buf) ((uint32_t)(_buf)->u32Idx)
#define HCP_P_COOKIE_L(_pcb, _buf) ((uint32_t)((((_pcb)->u32SessId) << 16) | ((_pcb)->stDrvCtx.id)))

#define HCP_M_COOKIE_H(_pcb, _buf) ((uint32_t)0)
#define HCP_M_COOKIE_L(_pcb, _buf) ((uint32_t)((((_pcb)->u32SessId) << 16) | ((_pcb)->stDrvCtx.id)))

#define HCP_HEAP_ALIGN          128
#define HCP_ALLOC(sz)           hcp_alloc_int(sz, #sz)
#define HCP_FREE(pv)            hcp_free_int(pv)

#define HCP_STATIC_IMG_TS_SZ    128 // TODO: should be defined by hfi
#define HCP_DYNAMIC_IMG_TS_SZ   128 // TODO: should be defined by hfi

#define HCP_DEFAULT_FRAMERATE   60

// Port param boundaries
#define HCP_WIDTH_MIN           32
#define HCP_WIDTH_MAX           4096
#define HCP_WIDTH_MULTIPLE      2

#define HCP_HEIGHT_MIN          32
#define HCP_HEIGHT_MAX          4096
#define HCP_HEIGHT_MULTIPLE     2

#define HCP_STRIDE_MIN          32
#define HCP_STRIDE_MAX          (HCP_WIDTH_MAX * 2)
#define HCP_STRIDE_MULTIPLE     2

#define HCP_SCANLINES_MIN       32
#define HCP_SCANLINES_MAX       4096
#define HCP_SCANLINES_MULTIPLE  2

// Knob boundaries
#define HCP_EAR_MODE_MIN        0
#define HCP_EAR_MODE_MAX        5
#define HCP_EAR_MODE_OFF        0
#define HCP_EAR_MODE_DFLT       4

#define HCP_SAT_GAIN_MIN        0
#define HCP_SAT_GAIN_MAX        100
#define HCP_SAT_GAIN_OFF        0
#define HCP_SAT_GAIN_DFLT       50

#define HCP_SAT_OFFSET_MIN      0
#define HCP_SAT_OFFSET_MAX      100
#define HCP_SAT_OFFSET_OFF      50
#define HCP_SAT_OFFSET_DFLT     50

#define HCP_ACE_STR_MIN         0
#define HCP_ACE_STR_MAX         100
#define HCP_ACE_STR_OFF         0
#define HCP_ACE_STR_DFLT        50

#define HCP_ACE_BRI_L_MIN       0
#define HCP_ACE_BRI_L_MAX       100
#define HCP_ACE_BRI_L_OFF       0
#define HCP_ACE_BRI_L_DFLT      25

#define HCP_ACE_BRI_H_MIN       0
#define HCP_ACE_BRI_H_MAX       100
#define HCP_ACE_BRI_H_OFF       0
#define HCP_ACE_BRI_H_DFLT      25

#define HCP_DE_GAIN_MIN         0
#define HCP_DE_GAIN_MAX         100
#define HCP_DE_GAIN_OFF         0
#define HCP_DE_GAIN_DFLT        25

#define HCP_SPLIT_MIN           0
#define HCP_SPLIT_MAX           100
#define HCP_SPLIT_DFLT          100

#define HCP_STAT(cb, stat)          (cb)->stats.u32##stat++
#define HCP_STAT_GET(cb, stat)      (cb)->stats.u32##stat
#define HCP_STAT_EQ(cb, st1, st2)   (HCP_STAT_GET(cb, st1) == HCP_STAT_GET(cb, st2))

#define HCP_SESS_RAW_PKT(_pCb, _i)  (&(_pCb)->st.pstPkt[_i])
#define HCP_SESS_CMD_PKT(_pCb)      ((_pCb)->st.pstCmdPkt)
#define HCP_SESS_CMD_HDR(_pCb)      (&(_pCb)->st.pstCmdPkt->stHdr)
#define HCP_SESS_CMD_BDY(_pCb)      ((_pCb)->st.pstCmdPkt->u32Bdy)
#define HCP_SESS_GET_HW_ID(_pCb)    ((_pCb)->stDrvCtx.id)

#define HCP_ENUM_HW_STATE(_E) \
    _E(HCP_HW_STATE_NULL) \
    _E(HCP_HW_STATE_ALLOC) \
    _E(HCP_HW_STATE_RDY) \
    _E(HCP_HW_STATE_CLOSING) \
    _E(HCP_HW_STATE_ERROR) \
    _E(HCP_HW_STATE_MAX)

#define HCP_ENUM_CMD_STATE(_E) \
    _E(HCP_CMD_STATE_NULL) \
    _E(HCP_CMD_STATE_OPEN_GET_BUF_REQ) \
    _E(HCP_CMD_STATE_OPEN_REALLOC_BUFS) \
    _E(HCP_CMD_STATE_OPEN_ALLOC_BUFS) \
    _E(HCP_CMD_STATE_OPEN_SEND_TO_FW) \
    _E(HCP_CMD_STATE_OPEN_WAIT_RSP_FW) \
    _E(HCP_CMD_STATE_OPEN_WAIT_CLOSE_DONE) \
    _E(HCP_CMD_STATE_OPEN_SIGNAL_CLIENT) \
    _E(HCP_CMD_STATE_OP_MODE_WAIT_FW_RSP) \
    _E(HCP_CMD_STATE_OP_MODE_SIGNAL_CLIENT) \
    _E(HCP_CMD_STATE_RESET_FW_WAIT_CLOSE) \
    _E(HCP_CMD_STATE_RESET_OPEN_FW) \
    _E(HCP_CMD_STATE_RESET_SIGNAL_CLIENT) \
    _E(HCP_CMD_STATE_DRAIN_PENDING_INPUT) \
    _E(HCP_CMD_STATE_DRAIN_DONE) \
    _E(HCP_CMD_STATE_FLUSH_WAIT_FW) \
    _E(HCP_CMD_STATE_FLUSH_INTERNAL) \
    _E(HCP_CMD_STATE_FLUSH_DONE) \
    _E(HCP_CMD_STATE_INTERNAL_DRAIN)

VPP_ENUM(t_EVppIpHcpHwState, HCP_ENUM_HW_STATE)
VPP_ENUM(t_EVppIpHcpCmdState, HCP_ENUM_CMD_STATE)

#define HCP_STATS(_E) \
    _E(HCP_STAT_HW_OPEN, 1, 1) \
    _E(HCP_STAT_HW_CLOSE, 1, 1) \
    _E(HCP_STAT_HW_SET_PROP_CP, 1, 1) \
    _E(HCP_STAT_HW_SET_PROP_KNOBS, 1, 1) \
    _E(HCP_STAT_HW_SET_PROP_OP_MODE, 1, 1) \
    _E(HCP_STAT_HW_SET_PROP_HINTS, 1, 1) \
    _E(HCP_STAT_HW_SET_PROP_DIAG_LVL, 1, 1) \
    _E(HCP_STAT_HW_GET_BUF_REQ, 1, 1) \
    _E(HCP_STAT_HW_SEND_SESS_BUF_PX, 1, 1) \
    _E(HCP_STAT_HW_SEND_SESS_BUF_MEAS, 1, 1) \
    _E(HCP_STAT_HW_SEND_BUFS, 1, 1) \
    _E(HCP_STAT_CLOSE_LATENCY, 1, 1) \
    _E(HCP_STAT_IN_RELEASE_DELTA, 1, 1) \
    _E(HCP_STAT_OUT_RELEASE_DELTA, 1, 1) \

VPP_STAT_ENUM(HCP_STATS);

typedef enum {
    HCP_OP_MODE_BYPASS,
    HCP_OP_MODE_PROC,
    HCP_OP_MODE_MEAS,
    HCP_OP_MODE_MAX,
} t_EVppIpHcpOpMode;

typedef enum {
    HCP_BUF_UNALLOCATED,
    HCP_BUF_OWNER_IP,
    HCP_BUF_OWNER_FW,
    HCP_BUF_OWNER_MAX,
} t_EVppIpHcpBufState;

typedef enum {
    HCP_BUF_TYPE_TUNING,
    HCP_BUF_TYPE_DIAG,
    HCP_BUF_TYPE_MAX
} t_EVppIpHcpSysBufType;

typedef enum {
    VPP_IP_CMD_RESET_FW = VPP_IP_CMD_MAX,
    VPP_IP_CMD_SET_OP_MODE,
    VPP_IP_CMD_INTERNAL_DRAIN,
} t_EVppIpCmdHcp;

typedef enum {
    HCP_GLOBAL_STATE_NULL,
    HCP_GLOBAL_STATE_INITIALIZING,
    HCP_GLOBAL_STATE_READY,
    HCP_GLOBAL_STATE_TERMINATING,
} t_EVppIpHcpGlobalState;

typedef struct {

    int id;
    t_EVppIpHcpHwState eHwState;
    t_EVppIpHcpOpMode eOpModeActive;

    // knobs
    struct {
        uint32_t u32KnobsLoaded;
        uint32_t u32EarMode;
        uint32_t u32DeGain;
        uint32_t u32SatGain;
        uint32_t u32SatOff;
        uint32_t u32AceStrCon;
        uint32_t u32AceStrBriL;
        uint32_t u32AceStrBriH;
        union t_uSplitScreenCtrl uSplitScreen;
    };

    // client buffers in firmware
    struct {
        uint32_t u32InBufCnt;
        uint32_t u32OutBufCnt;
    };

    struct {
        uint32_t u32InCntMin;
        uint32_t u32OutCntMin;
    } req;

    // cached params
    uint32_t u32CodecType;

} t_StVppIpHcpDrvCtx;

typedef struct {
    uint32_t u32Sz;

    t_StVppIonBuf stIon;
    t_EVppIpHcpSysBufType eSysBufType;
} t_StVppIpHcpSysBuf;

typedef struct {
    uint32_t u32Sz;
    uint32_t bSecure;

    void *pvRrb;
    t_StVppIonBuf stIon;
    t_EVppIpHcpBufState eBufState;
} t_StVppIpHcpIntBuf;

typedef struct {
    uint32_t bDirty;    // true if the sizes are outdated w.r.t. controls
    t_StVppIpHcpIntBuf stIntBufPx;
    t_StVppIpHcpIntBuf stIntBufMeas;
} t_StVppIpHcpIntBufs;

typedef struct {
    struct hqv_ctrl_aie stAie;
    struct hqv_ctrl_ear stEar;
    struct hqv_ctrl_meas stMeas;
    struct hqv_ctrl_global_demo stDemo;

    t_EVppIpHcpOpMode eOpMode;

    uint32_t u32EnableMask;
    uint32_t u32ComputeMask;
    uint32_t u32LoadMask;
} t_StVppIpHcpCfg;

typedef struct StVppIpHcpMsgQNode {
    struct StVppIpHcpMsgQNode *pNext;
    t_StHcpHfiMsgPkt stMsgPkt;
} t_StVppIpHcpMsgQNode;

typedef struct {
    t_StVppIpHcpMsgQNode *pstHead;
    t_StVppIpHcpMsgQNode *pstTail;
    uint32_t u32Cnt;
} t_StVppIpHcpMsgQ;

struct StVppIpHcpGlobalCb;

typedef struct {
    hcp_rpc_pkt_t *pstPkt;

    struct {
        // max size so that ALL commands can reuse this memory
        uint32_t u32CmdPktAllocSz;
        t_StHcpHfiCmdPkt *pstCmdPkt;
    };

    struct {
        uint32_t u32BufAttrAllocSz;
        t_StHcpHfiBufAttr *pstBufAttr;
    };

    struct {
        uint32_t u32RawBufAllocSz;
        hcp_rpc_raw_buffer_t *pstRawBuf;
    };

} t_StVppIpHcpSessionStructs;

typedef struct {
    // get_ts
    hcp_rpc_ts_strings_t *pstTs;

    // dedicated to be used for recv_pkt
    hcp_rpc_pkt_t *pstRawRspPkt;

    // cnt = HCP_MSG_Q_NODES
    t_StVppIpHcpMsgQNode *pstMsgQNodes;

    // for any commands that need to be sent
    hcp_rpc_pkt_t *pstRawCmdPkt;

    struct {
        // max size so that ALL commands can reuse this memory
        uint32_t u32CmdPktAllocSz;
        t_StHcpHfiCmdPkt *pstCmdPkt;
    };

    struct {
        t_StHcpHfiBufAttr *pstBufAttr;
    };

} t_StVppIpHcpGlobalStructs;

typedef struct {
    t_StVppIpBase stBase;
    struct StVppIpHcpGlobalCb *pstGlobal;

    uint32_t u32SessId; // internal session id

    union {
        struct {
            uint32_t bWorkerStarted : 1;
            uint32_t bWorkerEnded   : 1;
            uint32_t bOpened        : 1;
            uint32_t bOpenSignal    : 1;
            uint32_t bResetFwPending : 1;
            uint32_t bOpModePending : 1;
        };
        uint32_t u32;
    } flags;

    struct {
        uint32_t u32OpenRet;
        uint32_t u32CloseRet;
        uint32_t u32ResetFwRet;
        uint32_t u32OpModeRet;
    } async_res;

    pthread_t thread;
    pthread_cond_t cond;
    pthread_cond_t condToClient;
    pthread_mutex_t mutex;
    uint32_t u32WorkerErr;

    t_StVppIpPort stInput;
    t_StVppIpPort stOutput;

    t_StVppIpPort stFwIn;
    t_StVppIpPort stFwOut;

    t_StVppIpCmdQueue stCmdQ;

    uint32_t eState;

    t_EVppIpCmd eExecutingCmd;      // Command currently being executed
    t_EVppIpHcpCmdState eCmdState;  // Stage of the command being executed
    struct {
        enum vpp_port eFlushingPort;    // port that is currently being flushed
    };

    t_StVppIpHcpCfg stCfg;
    t_StVppIpHcpDrvCtx stDrvCtx;
    t_StVppIpHcpMsgQ stMsgQ;
    t_StVppIpHcpSessionStructs st;
    t_StVppIpHcpIntBufs stIntBufs;

    t_StVppIpBufReq stBufReqIn;
    t_StVppIpBufReq stBufReqOut;

    uint32_t u32BypassCnt;
    uint32_t u32InterlacedCnt;

    struct {
        uint32_t u32LogFlags;
    } stDbgCfg;

    struct {
        STAT_DECL(InQCnt);          // Number of frames queued to input
        STAT_DECL(OutQCnt);         // Number of frames queued to output

        STAT_DECL(InRxCnt);         // Number of frames queued to input, returned
        STAT_DECL(OutRxCnt);        // Number of frames queued to output, returned

        STAT_DECL(InQCntFailed);    // Number of frames failed to be queued to input
        STAT_DECL(OutQCntFailed);   // Number of frames failed to be queued to output

        STAT_DECL(InProcCnt);       // Number of input frames processed
        STAT_DECL(OutProcCnt);      // Number of output frames processed into
        STAT_DECL(IBDCnt);          // Number of IBD issued
        STAT_DECL(OBDCnt);          // Number of OBD issued
        STAT_DECL(BypassBuf);       // Bypass buffer due to flag
        STAT_DECL(BypassIntBuf);    // Bypass buffer due to interlaced

        STAT_DECL(InFlush);
        STAT_DECL(OutFlush);

        STAT_DECL(Drain);

        STAT_DECL(FwMsgRx);         // Total number of messages received from firmware

        // API + Responses
        STAT_DECL(FwTxClose);
        STAT_DECL(FwRxClose);
        STAT_DECL(FwTxCloseErr);
        STAT_DECL(FwRxCloseErr);

        // Commands
        STAT_DECL(FwTxCmdNop);
        STAT_DECL(FwRxCmdNop);

        // Properties
        STAT_DECL(FwTxSetProp);
        STAT_DECL(FwRxSetProp);

        STAT_DECL(FwTxPropOpMode);
        STAT_DECL(FwRxPropOpMode);

        STAT_DECL(FwTxPropCpMode);
        STAT_DECL(FwRxPropCpMode);

        STAT_DECL(FwTxPropResHints);
        STAT_DECL(FwRxPropResHints);

        STAT_DECL(FwTxPropSetKnob);
        STAT_DECL(FwRxPropSetKnob);

        STAT_DECL(FwTxPropSetDiag);
        STAT_DECL(FwRxPropSetDiag);

        STAT_DECL(FwTxGetProp);
        STAT_DECL(FwRxGetProp);

        STAT_DECL(FwTxGetPropBufReq);
        STAT_DECL(FwRxGetPropBufReq);
    } stats;
} t_StVppIpHcpSessionCb;

typedef struct StVppIpHcpGlobalCb {
    t_StVppCtx stVppCtx;

    remote_handle64 hndl;

    pthread_t threadRsp;
    pthread_cond_t cond;
    pthread_mutex_t mutex;

    t_EVppIpHcpGlobalState eState;

    union {
        struct {
            uint32_t bBootLock      : 1;
            uint32_t bRxResult      : 1;
            uint32_t bPendShutdown  : 1;
            uint32_t bTalkWithFw    : 1;
        };
        uint32_t u32;
    } u;
    uint32_t u32AsyncRes;

    void *pvTunings;
    t_StVppIpHcpGlobalStructs st;

    struct {
        t_StVppIpHcpSessionCb *apstSessions[HCP_MAX_SESSIONS];
        uint32_t u32Cnt;
    } sessions;

    t_StVppIpHcpMsgQ stMsgQ;

} t_StVppIpHcpGlobalCb;

static inline void *hcp_alloc_int(uint32_t sz, const char *pcDbg)
{
    int rc;
    void *p;

    LOGI("-- allocating %u bytes for: %s", sz, pcDbg);

    rc = posix_memalign(&p, HCP_HEAP_ALIGN, sz);
    if (!rc && p)
    {
        memset(p, 0, sz);
        return p;
    }
    LOGE("posix_memalign failed, rc=%d, pv=%p", rc, p);
    return NULL;
}

static inline void hcp_free_int(void *pv)
{
    if (pv)
        free(pv);
}

/***************************************************************************
 * Function Prototypes
 ***************************************************************************/

#endif /* _VPP_IP_HCP_PRIV_H_ */
