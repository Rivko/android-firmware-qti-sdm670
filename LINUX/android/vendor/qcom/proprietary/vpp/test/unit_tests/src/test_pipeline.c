/*!
 * @file test_pipeline.c
 *
 * @cr
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.

 * @services    Unit tests for the pipeline
 */

#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include <inttypes.h>
#include <unistd.h>

#include "dvpTest.h"
#include "dvpTest_tb.h"

#define VPP_LOG_TAG     VPP_LOG_UT_PL_TAG
#define VPP_LOG_MODULE  VPP_LOG_UT_PL
#include "vpp_dbg.h"

#include "vpp.h"
#include "vpp_core.h"

#ifdef VPP_TARGET_USES_GPU
#include "vpp_ip_gpu.h"
#endif

#include "vpp_stats.h"
#include "vpp_callback.h"
#include "vpp_ion.h"
#include "vpp_pipeline.h"
#include "vpp_utils.h"
#include "vpp_uc.h"

#include "buf_pool.h"
#include "test_utils.h"
#include "test_mock_registry.h"
#include "test_mock_ip.h"

#define LOG_U32(L, u32) LOG##L(#u32"=%u", u32)
#define LOG_PORT_STATS(L, ptctx) \
    do { \
        LOG_U32(L, ptctx->u32InPutCnt); \
        LOG_U32(L, ptctx->u32InHoldCnt); \
        LOG_U32(L, ptctx->u32InRxCnt); \
        LOG_U32(L, ptctx->u32InFlushCnt); \
        LOG_U32(L, ptctx->u32OutPutCnt); \
        LOG_U32(L, ptctx->u32OutHoldCnt); \
        LOG_U32(L, ptctx->u32OutRxCnt); \
    } while (0)

/***********************************************************************
 * Local definitions
 ***********************************************************************/
#define BUF_POOL_SZ 20

#define RUN_FRC_TESTS_WITHOUT_EOS

#define BYPASS_EXP_PTR_Q_SZ (BUF_POOL_SZ * 2)
#define PL_ENABLE_PROF
#define PL_ENABLE_BUF_DUMP_PATH

#define DO_RES_SUPPORTED        (1 << 0)
#define DO_RES_NOT_SUPPORTED    (1 << 1)
#define DO_SINGLE_BUFTYPE       (1 << 2)
#define DO_ALL                  (DO_RES_SUPPORTED|DO_RES_NOT_SUPPORTED)
#define TEST_CB_FNC(name) name##_cb
#define TEST_CB_DECL(name) static void name##_cb(struct pipeline_test_info *pstTest)

struct pipeline_test_info {
    struct test_ctx *pstCtx;
    struct pipeline_test_ctx *pstPlCtx;
    t_StVppUsecase *pstUc;
    uint32_t bUcIsAuto;
    t_EVppRes eRes;
    t_EVppColorFmt eFmt;
    t_EVppBufType eBufType;
    t_EVppBufType eOutBufType;
    uint32_t bUcSupportsDi;
    uint32_t bUcSupportsProg;
};
typedef void (*test_fnc_cb)(struct pipeline_test_info *pstTest);

enum vpp_test_flags {
    TEST_PL_IN_FLUSH_PENDING,
    TEST_PL_IN_FLUSH_RECEIVED,
    TEST_PL_OUT_FLUSH_PENDING,
    TEST_PL_OUT_FLUSH_RECEIVED,
    TEST_PL_RECONFIG_RDY,
    TEST_PL_RECONFIG_DONE,
    TEST_PL_EOS_RECEIVED,
};

#define TEST_FLAG_IN_FLUSH_PEND         (1 << TEST_PL_IN_FLUSH_PENDING)
#define TEST_FLAG_IN_FLUSH_RECEIVED     (1 << TEST_PL_IN_FLUSH_RECEIVED)
#define TEST_FLAG_OUT_FLUSH_PEND        (1 << TEST_PL_OUT_FLUSH_PENDING)
#define TEST_FLAG_OUT_FLUSH_RECEIVED    (1 << TEST_PL_OUT_FLUSH_RECEIVED)
#define TEST_FLAG_RECONFIG_RDY          (1 << TEST_PL_RECONFIG_RDY)
#define TEST_FLAG_RECONFIG_DONE         (1 << TEST_PL_RECONFIG_DONE)
#define TEST_FLAG_EOS_RECEIVED          (1 << TEST_PL_EOS_RECEIVED)

#define FT_P                            eVppBufType_Progressive
#define FT_TFF                          eVppBufType_Interleaved_TFF
#define FT_BFF                          eVppBufType_Interleaved_BFF

#define ITOP_Q_SZ                       64

struct buffer_rotation_cb {
    void (*cb_before_queue_input)(struct test_ctx *pstCtx, struct bufnode *pNode, uint32_t u32Idx);
    void (*cb_before_queue_output)(struct test_ctx *pstCtx, struct bufnode *pNode);

    void (*cb_after_queue_input)(struct test_ctx *pstCtx);
    void (*cb_after_queue_output)(struct test_ctx *pstCtx);
} ;

struct expected_buffer {
    t_EVppBufType eBufType;
    enum vpp_port port_owner;
};

enum pl_bypass_reason {
    PL_BYP_REASON_BYP_FLAG,
    PL_BYP_REASON_FL_ZERO,
};

struct pipeline_test_ctx {
    t_StVppParam stParam;
    t_StVppHqvCtrl stCtrl;
    t_StVppUsecase *pstUc;
    uint32_t u32TestFlags;
    uint32_t bUcSendsInputToOutput;
    uint32_t bUcInterpolatesOutput;

    uint32_t bTsMgrInit;
    struct ts_mgr stTsMgr;

    struct {
        struct buf_pool *pool;
        t_EVppBufType eQueueBufType;
        t_EVppBufType eExpectedBufType;
        uint32_t u32ExpectedFillLen;
    } basic;
    uint32_t u32EmptyEosTest;

    struct {
        uint32_t u32ExpectedFillLen;
    } input_to_output;

    struct {
        struct buf_pool *pool;
        uint32_t u32OutputProcCnt; // Number of processed output buffers
        uint32_t u32OutputFlushCnt; // Number of flushed output buffers
        uint32_t bFlushedOutputReceived;
    } flush;

    uint32_t bReconfigureTest;
    struct {
        t_StVppParam stNewParams;
        struct buf_pool *reconfig_pool;
        struct buf_pool_params reconfig_pool_params;

        t_StVppUsecase *pstNewUc;

        uint32_t u32ExpectedFillLen;
        uint32_t bBypass;
        uint32_t bReconfigFlush;
        uint32_t bFirstFlushedBufRx;
    } reconfig;

    uint32_t bBypassTest;
    uint32_t bBypassIpTest;
    uint32_t bBypassAllocLenTest;
    struct {
        uint32_t bFlushing;
        uint32_t u32ExpectedFillLen;
        uint32_t u32IpBypassMod;
        uint32_t u32IpBypassPutCnt;
        uint32_t u32IpBypassRxCnt;
        uint32_t u32InputRxOnOutput;
        enum pl_bypass_reason eReason;

        pthread_mutex_t mutex;
        t_StVppQueue stInputObdOrder;
        void *apvInputOrderNodes[BYPASS_EXP_PTR_Q_SZ];
        uint32_t bEnableObdInputOrder;
    } bypass;

    uint32_t bItoPTest;
    struct {
        uint32_t bUcSupportsDi;
        uint32_t bUcSupportsProg;
        struct expected_buffer astInExpBuf[ITOP_Q_SZ];
        struct expected_buffer astOutExpBuf[ITOP_Q_SZ];
        uint32_t au32InExpBufCnt[eVppBufType_Max];
        t_StVppQueue stQInBufType;
        t_StVppQueue stQOutBufType;
        pthread_mutex_t mutex;
        uint32_t u32ExpInRxCnt;
        uint32_t u32ExpOutRxCnt;
        uint32_t bFirstFlushOutRx;
    } itop;
};

/************************************************************************
 * Local static variables
 ***********************************************************************/
static const char *apcResName[VPP_RESOLUTION_MAX] = {
    [VPP_RESOLUTION_SD] = "SD",
    [VPP_RESOLUTION_HD] = "HD",
    [VPP_RESOLUTION_FHD] = "FHD",
    [VPP_RESOLUTION_UHD] = "UHD",
};

static const char *apcBufTypeName[eVppBufType_Max] = {
    [eVppBufType_Progressive] = "P",
    [eVppBufType_Interleaved_TFF] = "ITFF",
    [eVppBufType_Interleaved_BFF] = "IBFF",
    [eVppBufType_Frame_TFF] = "FTFF",
    [eVppBufType_Frame_BFF] = "FBFF",
};

static const char *apcFmtName[VPP_COLOR_FORMAT_MAX] = {
    [VPP_COLOR_FORMAT_NV12_VENUS] = "Linear_NV12",
    [VPP_COLOR_FORMAT_NV21_VENUS] = "Linear_NV21",
    [VPP_COLOR_FORMAT_P010] = "Linear_P010",
    [VPP_COLOR_FORMAT_UBWC_NV12] = "UBWC_NV12",
    [VPP_COLOR_FORMAT_UBWC_NV21] = "UBWC_NV21",
    [VPP_COLOR_FORMAT_UBWC_TP10] = "UBWC_TP10",
};

static const enum clip_reg aeClip[VPP_RESOLUTION_MAX][VPP_COLOR_FORMAT_MAX] = {
    [VPP_RESOLUTION_SD] = {
        [VPP_COLOR_FORMAT_NV12_VENUS] = CLIP_CANYON_720x480,
        [VPP_COLOR_FORMAT_NV21_VENUS] = CLIP_NULL,
        [VPP_COLOR_FORMAT_P010] = CLIP_P010_SD,
        [VPP_COLOR_FORMAT_UBWC_NV12] = CLIP_UBWC_NV12_SD,
        [VPP_COLOR_FORMAT_UBWC_NV21] = CLIP_NULL,
        [VPP_COLOR_FORMAT_UBWC_TP10] = CLIP_UBWC_TP10_SD,
    },
    [VPP_RESOLUTION_HD] = {
        [VPP_COLOR_FORMAT_NV12_VENUS] = CLIP_BEACH_1280x720,
        [VPP_COLOR_FORMAT_NV21_VENUS] = CLIP_NULL,
        [VPP_COLOR_FORMAT_P010] = CLIP_P010_HD,
        [VPP_COLOR_FORMAT_UBWC_NV12] = CLIP_SLOMO_PITCH_UBWC_NV12_1280x720,
        [VPP_COLOR_FORMAT_UBWC_NV21] = CLIP_NULL,
        [VPP_COLOR_FORMAT_UBWC_TP10] = CLIP_UBWC_TP10_HD,
    },
    [VPP_RESOLUTION_FHD] = {
        [VPP_COLOR_FORMAT_NV12_VENUS] = CLIP_FRC_1920x1080,
        [VPP_COLOR_FORMAT_NV21_VENUS] = CLIP_NULL,
        [VPP_COLOR_FORMAT_P010] = CLIP_P010_FHD,
        [VPP_COLOR_FORMAT_UBWC_NV12] = CLIP_UBWC_NV12_FHD,
        [VPP_COLOR_FORMAT_UBWC_NV21] = CLIP_NULL,
        [VPP_COLOR_FORMAT_UBWC_TP10] = CLIP_UBWC_TP10_FHD,
    },
    [VPP_RESOLUTION_UHD] = {
        [VPP_COLOR_FORMAT_NV12_VENUS] = CLIP_NEW_YORK_3840x2160,
        [VPP_COLOR_FORMAT_NV21_VENUS] = CLIP_NULL,
        [VPP_COLOR_FORMAT_P010] = CLIP_P010_UHD,
        [VPP_COLOR_FORMAT_UBWC_NV12] = CLIP_UBWC_NV12_UHD,
        [VPP_COLOR_FORMAT_UBWC_NV21] = CLIP_NULL,
        [VPP_COLOR_FORMAT_UBWC_TP10] = CLIP_UBWC_TP10_UHD,
    },
};
/************************************************************************
 * Forward Declarations
 ************************************************************************/
extern const uint32_t u32ResolutionMaxHeight[VPP_RESOLUTION_MAX];
extern const uint32_t u32ResolutionMaxWidth[VPP_RESOLUTION_MAX];

/************************************************************************
 * Local Functions
 ***********************************************************************/

static void enqueue_expected_type(struct pipeline_test_ctx *pstPlCtx,
                                  struct expected_buffer *pstExpBuf,
                                  enum vpp_port port)
{
    int32_t idx;

    struct expected_buffer *pNodes;
    t_StVppQueue *pstQ;

    if (port == VPP_PORT_INPUT)
    {
        pstQ = &pstPlCtx->itop.stQInBufType;
        pNodes = pstPlCtx->itop.astInExpBuf;
    }
    else
    {
        pstQ = &pstPlCtx->itop.stQOutBufType;
        pNodes = pstPlCtx->itop.astOutExpBuf;
    }

    pthread_mutex_lock(&pstPlCtx->itop.mutex);

    idx = vpp_queue_enqueue(pstQ);
    if (idx < 0)
        DVP_ASSERT_FAIL();
    else
        memcpy(&pNodes[idx], pstExpBuf, sizeof(struct expected_buffer));

    pthread_mutex_unlock(&pstPlCtx->itop.mutex);
}

static uint32_t dequeue_expected_type(struct pipeline_test_ctx *pstPlCtx,
                                      struct expected_buffer *pstExpBuf,
                                      enum vpp_port port)
{
    int32_t idx;
    uint32_t u32 = VPP_OK;
    struct expected_buffer *pNodes;
    t_StVppQueue *pstQ;

    if (port == VPP_PORT_INPUT)
    {
        pstQ = &pstPlCtx->itop.stQInBufType;
        pNodes = pstPlCtx->itop.astInExpBuf;
    }
    else
    {
        pstQ = &pstPlCtx->itop.stQOutBufType;
        pNodes = pstPlCtx->itop.astOutExpBuf;
    }

    pthread_mutex_lock(&pstPlCtx->itop.mutex);
    idx = vpp_queue_dequeue(pstQ);

    if (idx < 0)
        u32 = VPP_ERR;
    else
        *pstExpBuf = pNodes[idx];

    pthread_mutex_unlock(&pstPlCtx->itop.mutex);

    return u32;
}

static uint32_t enqueue_expected_bypass_buffer(struct pipeline_test_ctx *pstPlCtx,
                                               t_StVppBuf *pstBuf)
{
    int32_t i;

    if (!pstPlCtx || !pstBuf)
        return VPP_ERR;

    pthread_mutex_lock(&pstPlCtx->bypass.mutex);
    i = vpp_queue_enqueue(&pstPlCtx->bypass.stInputObdOrder);
    if (i < 0)
    {
        pthread_mutex_unlock(&pstPlCtx->bypass.mutex);
        return VPP_ERR;
    }
    else
    {
        pstPlCtx->bypass.apvInputOrderNodes[i] = pstBuf;
    }

    pthread_mutex_unlock(&pstPlCtx->bypass.mutex);
    return VPP_OK;
}

t_StVppBuf *dequeue_expected_bypass_buffer(struct pipeline_test_ctx *pstPlCtx)
{
    int32_t i;
    t_StVppBuf * pstTmp = NULL;

    if (!pstPlCtx)
        return NULL;

    pthread_mutex_lock(&pstPlCtx->bypass.mutex);
    i = vpp_queue_dequeue(&pstPlCtx->bypass.stInputObdOrder);
    if (i < 0)
    {
        pthread_mutex_unlock(&pstPlCtx->bypass.mutex);
        return NULL;
    }

    pstTmp = pstPlCtx->bypass.apvInputOrderNodes[i];
    pthread_mutex_unlock(&pstPlCtx->bypass.mutex);
    return pstTmp;
}

void pipeline_ibd(void *pv, t_StVppBuf *pBuf)
{
    uint32_t u32;
    struct test_ctx *pCtx = (struct test_ctx *)pv;
    struct pipeline_test_ctx *pPlTestCtx =
        (struct pipeline_test_ctx *)pCtx->pPrivateCtx;
    struct bufnode *pNode = pBuf->pBuf->cookie;
    t_EVppBufType eBufType;

    LOGI("%s() pBuf: %p", __func__, pBuf);
    // Can only get input buffers from IBD
    DVP_ASSERT_EQUAL(pNode->port_owner, VPP_PORT_INPUT);

    u32 = u32VppBuf_GetFrameTypeExtradata(pBuf, VPP_EXTERNAL_EXTRADATA_TYPE, &eBufType);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    if (pPlTestCtx->bItoPTest)
    {
        struct expected_buffer stExpBuf;
        memset(&stExpBuf, 0, sizeof(struct expected_buffer));

        u32 = dequeue_expected_type(pPlTestCtx, &stExpBuf, VPP_PORT_INPUT);
        pPlTestCtx->itop.au32InExpBufCnt[eBufType]--;
        DVP_ASSERT_EQUAL(u32, VPP_OK);
        // Can't guarantee IBD order for multi-IP since it's possible input gets bypassed to next IP
        if (pPlTestCtx->pstUc->u32IpCnt == 1)
            DVP_ASSERT_EQUAL(stExpBuf.eBufType, eBufType);
        DVP_ASSERT_EQUAL(stExpBuf.port_owner, pNode->port_owner);
    }
    else
    {
        DVP_ASSERT_EQUAL(eBufType, pPlTestCtx->basic.eQueueBufType);
    }
    validate_extradata_integrity(pNode);

    pthread_mutex_lock(&pCtx->mutex);

    if (VPP_FLAG_IS_SET(pNode->pExtBuf->flags, VPP_BUFFER_FLAG_EOS))
        pCtx->u32InEosRxCnt += 1;

    if (VPP_FLAG_IS_SET(pNode->pIntBuf->u32InternalFlags, VPP_BUF_FLAG_FLUSHED))
        pCtx->u32InFlushCnt += 1;

    u32 = restore_bufnode_internal_buf(pNode);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    pCtx->u32InRxCnt += 1;
    pCtx->u32InHoldCnt -= 1;
    put_buf(pNode->pPool, pNode);

    if (pPlTestCtx->bReconfigureTest)
        DVP_ASSERT_FALSE(pPlTestCtx->reconfig.bBypass);

    if (pPlTestCtx->bBypassTest && !pPlTestCtx->bypass.bFlushing)
        DVP_ASSERT_FAIL();

    if (pPlTestCtx->bBypassAllocLenTest)
    {
        // Two cases where buffer can be returned on IBD for insufficient
        // buffer size: (i) the input buffer size itself is too small so it is
        // returned on ibd, (ii) the output buffer size is too small so the
        // input is bypassed to the output.
        if (!pPlTestCtx->bypass.bFlushing)
        {
            uint32_t u32RequiredSz, u32AvailSz;
            u32RequiredSz = u32VppUtils_GetPxBufferSize(&pCtx->port_param);
            u32AvailSz = pNode->pIntBuf->stPixel.u32ValidLen;
            DVP_ASSERT_TRUE(u32AvailSz < u32RequiredSz);
        }

        if (pNode->pExtBuf->flags & VPP_BUFFER_FLAG_EOS)
        {
            VPP_FLAG_SET(pPlTestCtx->u32TestFlags, TEST_FLAG_EOS_RECEIVED);
        }
    }
    else if (pPlTestCtx->bItoPTest)
    {
        if (pPlTestCtx->bUcSendsInputToOutput)
        {
            if (!VPP_FLAG_IS_SET(pPlTestCtx->u32TestFlags, TEST_FLAG_OUT_FLUSH_PEND))
            {
                DVP_ASSERT_FAIL();
            }
        }
    }

    pthread_mutex_unlock(&pCtx->mutex);
    pthread_cond_signal(&pCtx->cond);
}

void pipeline_obd(void *pv, t_StVppBuf *pBuf)
{
    uint32_t u32;
    struct test_ctx *pCtx = (struct test_ctx *)pv;
    struct pipeline_test_ctx *pPlTestCtx =
        (struct pipeline_test_ctx *)pCtx->pPrivateCtx;
    struct bufnode *pNode = pBuf->pBuf->cookie;
    t_EVppBufType eBufType;

    LOGI("%s() pBuf: %p", __func__, pBuf);

    eBufType = eVppBuf_GetFrameType(pBuf);

    pthread_mutex_lock(&pCtx->mutex);

    pCtx->u32OutRxCnt += 1;

    if (VPP_FLAG_IS_SET(pNode->pIntBuf->u32InternalFlags, VPP_BUF_FLAG_FLUSHED))
        pCtx->u32OutFlushCnt += 1;

    if (VPP_FLAG_IS_SET(pNode->pExtBuf->flags, VPP_BUFFER_FLAG_EOS))
        pCtx->u32OutEosRxCnt += 1;

    if (pPlTestCtx->bReconfigureTest)
    {
        uint32_t u32Exp;
        u32Exp = pPlTestCtx->reconfig.u32ExpectedFillLen;
        if (pPlTestCtx->reconfig.bReconfigFlush)
        {
            if (pPlTestCtx->reconfig.bFirstFlushedBufRx)
            {
                u32Exp = 0;
            }
            else if (pNode->pIntBuf->stPixel.u32FilledLen == 0)
            {
                u32Exp = 0;
                pPlTestCtx->reconfig.bFirstFlushedBufRx = VPP_TRUE;
            }
        }
        if (pCtx->u32InHoldCnt == 0)
        {
            // During a reconfigure scenario, it is possible that after an IP
            // is reconfigured to a greater resolution, the output buffers that
            // are held in the output port are too small for processing. In
            // this case, the IP block can detect this and return the output
            // buffers. This can occur after immediately after
            // close/reconfigure/open has been called on the ip block, but
            // before pipeline has  issued the reconfigure done event. Thus
            // buffers with filled_len == 0 can be returned on the output port.
            // However, this can only happen after all input buffers have been
            // processed (since we are in reconfigure case), so it should
            // be safe to do this check if u32InHoldCnt is zero. However,
            // this condition could also be true if the IP block returns
            // IBD before OBD, so the valid output filled length is 0 or
            // the expected length.
            DVP_ASSERT_TRUE(pNode->pIntBuf->stPixel.u32FilledLen == u32Exp ||
                            pNode->pIntBuf->stPixel.u32FilledLen == 0);
            LOGI("expected=%u or 0, actual=%u", u32Exp,
                 pNode->pIntBuf->stPixel.u32FilledLen);
        }
        else
        {
            LOGI("expected=%u, actual=%u", u32Exp,
                 pNode->pIntBuf->stPixel.u32FilledLen);
            DVP_ASSERT_EQUAL(u32Exp, pNode->pIntBuf->stPixel.u32FilledLen);
        }

    }

    if (pPlTestCtx->bReconfigureTest && pPlTestCtx->reconfig.bBypass &&
        !pPlTestCtx->bypass.bFlushing)
        DVP_ASSERT_EQUAL(pNode->port_owner, VPP_PORT_INPUT);

    if (pPlTestCtx->bBypassTest)
    {
        if (!pPlTestCtx->bypass.bFlushing)
            DVP_ASSERT_EQUAL(pNode->port_owner, VPP_PORT_INPUT);
        else if (pNode->port_owner == VPP_PORT_OUTPUT)
            DVP_ASSERT_EQUAL(0, pNode->pIntBuf->stPixel.u32FilledLen);
    }

    if (pNode->port_owner == VPP_PORT_INPUT)
    {
        uint32_t u32Idx = pNode->pIntBuf->u32BufNum;
        uint32_t bBypass = u32Idx && pPlTestCtx->bypass.u32IpBypassMod &&
            (u32Idx % pPlTestCtx->bypass.u32IpBypassMod == 0);

        pCtx->u32InHoldCnt -= 1;
        if (pPlTestCtx->bReconfigureTest)
        {
            DVP_ASSERT_EQUAL(pNode->pIntBuf->stPixel.u32FilledLen,
                             pPlTestCtx->reconfig.u32ExpectedFillLen);
        }
        else if (pPlTestCtx->bBypassTest ||
                 pPlTestCtx->bBypassIpTest ||
                 pPlTestCtx->bBypassAllocLenTest ||
                 pPlTestCtx->bItoPTest)
        {
            if (pPlTestCtx->bypass.eReason != PL_BYP_REASON_FL_ZERO ||
                (pPlTestCtx->bypass.eReason == PL_BYP_REASON_FL_ZERO && !bBypass))
                DVP_ASSERT_EQUAL(pNode->pIntBuf->stPixel.u32FilledLen,
                                 pPlTestCtx->bypass.u32ExpectedFillLen);
        }
        else if (pPlTestCtx->bUcSendsInputToOutput)
        {
            // The u32FilledLen may not be equal to the expected u32FilledLen in
            // the case where empty eos buffer is being issued, since the
            // u32FilledLen willl be zero. However, in this case, the EOS flag
            // must also be set.
            if (!pNode->pIntBuf->stPixel.u32FilledLen)
            {
                if (pPlTestCtx->bypass.eReason != PL_BYP_REASON_FL_ZERO ||
                    (pPlTestCtx->bypass.eReason == PL_BYP_REASON_FL_ZERO && !bBypass))
                    DVP_ASSERT_TRUE(VPP_FLAG_IS_SET(pNode->pExtBuf->flags,
                                                    VPP_BUFFER_FLAG_EOS));
            }
            else
            {
                DVP_ASSERT_EQUAL(pNode->pIntBuf->stPixel.u32FilledLen,
                                 pPlTestCtx->input_to_output.u32ExpectedFillLen);
            }
        }
        else if (VPP_FLAG_IS_SET(pNode->pExtBuf->flags, VPP_BUFFER_FLAG_EOS) &&
                 pNode->pIntBuf->stPixel.u32FilledLen == 0)
        {
            // empty case, just for explicit logic
        }
        else
            DVP_ASSERT_FAIL();

        if (pPlTestCtx->bBypassIpTest)
        {
            // For algos which send the input to the output (FRC, for example),
            // some of the input buffers which are received on the output may
            // not have the bypass flag set. Thus, we should only assert on
            // usecases which do not send the input to the output under normal
            // circumstances.

            pPlTestCtx->bypass.u32InputRxOnOutput += 1;

            if (!pPlTestCtx->bUcSendsInputToOutput &&
                pPlTestCtx->bypass.eReason != PL_BYP_REASON_FL_ZERO)
            {
                DVP_ASSERT_TRUE(VPP_FLAG_IS_SET(pNode->pIntBuf->u32InternalFlags,
                                                VPP_BUF_FLAG_BYPASS));
            }

            if (pPlTestCtx->bypass.bEnableObdInputOrder)
            {
                t_StVppBuf *pstExp;
                pstExp = dequeue_expected_bypass_buffer(pPlTestCtx);
                DVP_ASSERT_PTR_EQUAL(pstExp, pNode->pIntBuf);
            }

            if (VPP_FLAG_IS_SET(pNode->pIntBuf->u32InternalFlags, VPP_BUF_FLAG_BYPASS) ||
                (pPlTestCtx->bypass.eReason == PL_BYP_REASON_FL_ZERO &&
                 pNode->pIntBuf->stPixel.u32FilledLen == 0))
                pPlTestCtx->bypass.u32IpBypassRxCnt += 1;
        }

        if (!pPlTestCtx->bItoPTest)
            DVP_ASSERT_EQUAL(eBufType, pPlTestCtx->basic.eQueueBufType);

        validate_extradata_integrity(pNode);
    }
    else if (pNode->port_owner == VPP_PORT_OUTPUT)
    {
        pCtx->u32OutHoldCnt -= 1;

        if (pPlTestCtx->bBypassAllocLenTest)
            DVP_ASSERT_EQUAL(pNode->pIntBuf->stPixel.u32FilledLen, 0);

        if (pNode->pIntBuf->stPixel.u32FilledLen)
        {
            pPlTestCtx->flush.u32OutputProcCnt += 1;

            // When we call flush, after the first flushed buffer is received,
            // we should not get any more.
            if (!pPlTestCtx->bBypassAllocLenTest)
                DVP_ASSERT_FALSE(pPlTestCtx->flush.bFlushedOutputReceived);

            if (!pPlTestCtx->bItoPTest)
                DVP_ASSERT_EQUAL(eBufType, pPlTestCtx->basic.eExpectedBufType);
            validate_extradata_integrity(pNode);

            if (!pPlTestCtx->bReconfigureTest &&
                !pPlTestCtx->bBypassAllocLenTest)
            {
                DVP_ASSERT_EQUAL(pNode->pIntBuf->stPixel.u32FilledLen,
                                 pPlTestCtx->basic.u32ExpectedFillLen);
                if (pNode->pIntBuf->stPixel.u32FilledLen !=
                    pPlTestCtx->basic.u32ExpectedFillLen)
                {
                    LOGE("u32ExpectedFillLen=%u, actual=%u",
                         pPlTestCtx->basic.u32ExpectedFillLen,
                         pNode->pIntBuf->stPixel.u32FilledLen);
                }
            }
        }
        else
        {
            pPlTestCtx->flush.u32OutputFlushCnt += 1;
            if (!pPlTestCtx->bBypassAllocLenTest)
                pPlTestCtx->flush.bFlushedOutputReceived = VPP_TRUE;
        }
    }
    else
    {
        DVP_ASSERT_FAIL();
    }

    if (pPlTestCtx->bItoPTest)
    {
        if (VPP_FLAG_IS_SET(pPlTestCtx->u32TestFlags, TEST_FLAG_OUT_FLUSH_PEND))
        {
            // if received the first flushed buffer (u32FilledLen == 0), the
            // expected port owner should be output
            if (!pPlTestCtx->itop.bFirstFlushOutRx)
            {
                if (pNode->pIntBuf->stPixel.u32FilledLen == 0)
                    pPlTestCtx->itop.bFirstFlushOutRx = VPP_TRUE;
            }
            else
            {
                DVP_ASSERT_EQUAL(pNode->pIntBuf->stPixel.u32FilledLen, 0);
                DVP_ASSERT_EQUAL(pNode->port_owner, VPP_PORT_OUTPUT);
            }
        }
        else if (!pPlTestCtx->bUcSendsInputToOutput)
        {
            struct expected_buffer stExpBuf;
            memset(&stExpBuf, 0, sizeof(struct expected_buffer));

            u32 = dequeue_expected_type(pPlTestCtx, &stExpBuf, VPP_PORT_OUTPUT);
            if (pPlTestCtx->pstUc->u32IpCnt == 1)
            {
                DVP_ASSERT_EQUAL(u32, VPP_OK);
                DVP_ASSERT_EQUAL(stExpBuf.eBufType, eBufType);
                DVP_ASSERT_EQUAL(stExpBuf.port_owner, pNode->port_owner);
                LOGE_IF(stExpBuf.port_owner != pNode->port_owner, "exp port_owner=%u"
                        "actual port_owner=%u", stExpBuf.port_owner, pNode->port_owner);
            }
        }
    }

    u32 = restore_bufnode_internal_buf(pNode);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    if (pNode->pIntBuf->stPixel.u32FilledLen)
        dump_buf(pNode);

    if (pNode->pExtBuf->flags & VPP_BUFFER_FLAG_EOS)
    {
        VPP_FLAG_SET(pPlTestCtx->u32TestFlags, TEST_FLAG_EOS_RECEIVED);
    }

    put_buf(pNode->pPool, pNode);

    pthread_mutex_unlock(&pCtx->mutex);

    pthread_cond_signal(&pCtx->cond);
}

void pipeline_event(void *pv, t_StVppEvt stEvt)
{
    struct test_ctx *pstCtx = (struct test_ctx *)pv;
    struct pipeline_test_ctx *pPlTestCtx =
        (struct pipeline_test_ctx *)pstCtx->pPrivateCtx;

    uint32_t u32MissingNodes, u32OutRx;

    LOGI("%s() got event: %u", __func__, stEvt.eType);


    if (stEvt.eType == VPP_EVT_FLUSH_DONE)
    {
        pthread_mutex_lock(&pstCtx->mutex);

        u32MissingNodes = get_missing_node_count(pPlTestCtx->basic.pool);

        if (stEvt.flush.ePort == VPP_PORT_INPUT)
        {
            DVP_ASSERT_TRUE(VPP_FLAG_IS_SET(pPlTestCtx->u32TestFlags, TEST_FLAG_IN_FLUSH_PEND));
            VPP_FLAG_CLR(pPlTestCtx->u32TestFlags, TEST_FLAG_IN_FLUSH_PEND);
            VPP_FLAG_SET(pPlTestCtx->u32TestFlags, TEST_FLAG_IN_FLUSH_RECEIVED);

            DVP_ASSERT_EQUAL(u32MissingNodes, pstCtx->u32OutHoldCnt);
            DVP_ASSERT_EQUAL(pstCtx->u32InHoldCnt, 0);
            if (!pPlTestCtx->bBypassTest &&
                !pPlTestCtx->bBypassIpTest &&
                !pPlTestCtx->bBypassAllocLenTest &&
                !pPlTestCtx->bUcSendsInputToOutput)
            {
                DVP_ASSERT_EQUAL(pstCtx->u32InRxCnt, pstCtx->u32InPutCnt);
            }
            else
            {
                uint32_t u32TotalPut = pstCtx->u32InPutCnt + pstCtx->u32OutPutCnt;
                u32TotalPut -= pstCtx->u32OutHoldCnt;
                LOG_U32(I, u32TotalPut);
                LOG_U32(I, pstCtx->u32InRxCnt);
                LOG_U32(I, pstCtx->u32OutRxCnt);
                DVP_ASSERT_EQUAL(pstCtx->u32InRxCnt, u32TotalPut - pstCtx->u32OutRxCnt);
            }

            LOG_U32(I, u32MissingNodes);
            LOG_PORT_STATS(I, pstCtx);
        }
        else if (stEvt.flush.ePort == VPP_PORT_OUTPUT)
        {
            DVP_ASSERT_TRUE(VPP_FLAG_IS_SET(pPlTestCtx->u32TestFlags, TEST_FLAG_OUT_FLUSH_PEND));
            VPP_FLAG_CLR(pPlTestCtx->u32TestFlags, TEST_FLAG_OUT_FLUSH_PEND);
            VPP_FLAG_SET(pPlTestCtx->u32TestFlags, TEST_FLAG_OUT_FLUSH_RECEIVED);

            DVP_ASSERT_EQUAL(u32MissingNodes, pstCtx->u32InHoldCnt);
            LOGE_IF(u32MissingNodes != pstCtx->u32InHoldCnt, "u32MissingNodes=%u "
                    "pstCtx->u32InHoldCnt=%u", u32MissingNodes, pstCtx->u32InHoldCnt);
            DVP_ASSERT_EQUAL(pstCtx->u32OutHoldCnt, 0);

            if (pPlTestCtx->bBypassTest)
            {
                DVP_ASSERT_TRUE(pstCtx->u32OutRxCnt >= pstCtx->u32OutPutCnt);
            }
            else
            {
                // Total number buffers received equals number we put in
                if (!pPlTestCtx->u32EmptyEosTest)
                {
                    u32OutRx = pPlTestCtx->flush.u32OutputProcCnt +
                        pPlTestCtx->flush.u32OutputFlushCnt;
                    DVP_ASSERT_EQUAL(pstCtx->u32OutPutCnt, u32OutRx);
                }
            }

            LOG_U32(I, u32MissingNodes);
            LOG_U32(I, pPlTestCtx->flush.u32OutputProcCnt);
            LOG_U32(I, pPlTestCtx->flush.u32OutputFlushCnt);
        }
        pthread_mutex_unlock(&pstCtx->mutex);
    }
    else if (stEvt.eType == VPP_EVT_RECONFIG_RDY)
    {
        uint32_t u32Ret;

        pthread_mutex_lock(&pstCtx->mutex);
        DVP_ASSERT_EQUAL(pstCtx->u32InHoldCnt, 0);
        if (pPlTestCtx->bReconfigureTest && pPlTestCtx->bBypassTest)
        {
            DVP_ASSERT_EQUAL(pstCtx->u32InHoldCnt, 0);
            DVP_ASSERT_TRUE(pstCtx->u32OutRxCnt >= pstCtx->u32InPutCnt);
        }
        else
        {
            if (pPlTestCtx->bUcSendsInputToOutput)
            {
                // We expect all input buffers to be returned on the output
                // port. Therefore, at this point, the number of buffers that
                // we expect on the output port is equal to total input plus
                // total output, minus output held.
                uint32_t u32ExpOut;
                u32ExpOut = pstCtx->u32OutPutCnt;
                u32ExpOut -= pstCtx->u32OutHoldCnt;
                u32ExpOut += pstCtx->u32InPutCnt;

                DVP_ASSERT_EQUAL(pstCtx->u32OutRxCnt, u32ExpOut);
                DVP_ASSERT_EQUAL(pstCtx->u32InRxCnt, 0);
                DVP_ASSERT_EQUAL(pstCtx->u32InHoldCnt, 0);
            }
            else
            {
                DVP_ASSERT_EQUAL(pstCtx->u32InRxCnt, pstCtx->u32InPutCnt);
            }
        }

        VPP_FLAG_SET(pPlTestCtx->u32TestFlags, TEST_FLAG_RECONFIG_RDY);
        pthread_mutex_unlock(&pstCtx->mutex);

        LOGD("Received event: VPP_EVT_RECONFIG_RDY");

        u32Ret = u32VppPipeline_Reconfigure(&pstCtx->stVppCtx,
                                            pPlTestCtx->reconfig.pstNewUc,
                                            &pPlTestCtx->reconfig.stNewParams,
                                            &pPlTestCtx->stCtrl);

    }
    else if (stEvt.eType == VPP_EVT_RECONFIG_DONE)
    {
        pthread_mutex_lock(&pstCtx->mutex);
        DVP_ASSERT_TRUE(VPP_FLAG_IS_SET(pPlTestCtx->u32TestFlags, TEST_FLAG_RECONFIG_RDY));
        VPP_FLAG_SET(pPlTestCtx->u32TestFlags, TEST_FLAG_RECONFIG_DONE);
        pthread_mutex_unlock(&pstCtx->mutex);
        LOGD("Received event: VPP_EVT_RECONFIG_DONE");
    }

    pthread_cond_signal(&pstCtx->cond);
}

#ifdef PL_TEST_ENABLE_TIMEOUT
static void get_wait_time(struct timespec *pTs, uint32_t u32Ms)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);

    pTs->tv_sec = tv.tv_sec;
    pTs->tv_nsec = (tv.tv_usec + (u32Ms * 1000UL)) * 1000UL;
}
#endif

enum clip_reg pipeline_get_clip(t_EVppRes eRes, t_EVppColorFmt eFmt)
{
    if (eRes >= VPP_RESOLUTION_MAX || eFmt >= VPP_COLOR_FORMAT_MAX)
    {
        LOGE("Invalid Params. eRes=%u (Max=%u), eFmt=%u (Max=%u)",
             eRes, VPP_RESOLUTION_MAX, eFmt, VPP_COLOR_FORMAT_MAX);
        return CLIP_NULL;
    }

    return aeClip[eRes][eFmt];
}

t_EVppRes pipeline_get_res(enum clip_reg clip)
{
    uint32_t w, h;
    uint32_t u32;

    u32 = get_res_for_clip(clip, &w, &h, NULL);
    if (u32 != VPP_OK)
        return NOT_SUPPORTED;

    if((h > u32ResolutionMaxHeight[VPP_RESOLUTION_UHD]) ||
       (w > u32ResolutionMaxWidth[VPP_RESOLUTION_UHD]))
        return NOT_SUPPORTED;
    else if((h > u32ResolutionMaxHeight[VPP_RESOLUTION_FHD]) ||
            (w > u32ResolutionMaxWidth[VPP_RESOLUTION_FHD]))
        return VPP_RESOLUTION_UHD;
    else if((h > u32ResolutionMaxHeight[VPP_RESOLUTION_HD]) ||
            (w > u32ResolutionMaxWidth[VPP_RESOLUTION_HD]))
        return VPP_RESOLUTION_FHD;
    else if((h > u32ResolutionMaxHeight[VPP_RESOLUTION_SD]) ||
            (w > u32ResolutionMaxWidth[VPP_RESOLUTION_SD]))
        return VPP_RESOLUTION_HD;
    else
        return VPP_RESOLUTION_SD;
}

static void pipeline_context_init(struct test_ctx *pstCtx,
                                  struct pipeline_test_ctx *pstPlCtx)
{
    uint32_t u32;
    int rc = 0;

    tctx_common_init(pstCtx);

    memset(pstPlCtx, 0, sizeof(struct pipeline_test_ctx));
    pstCtx->pPrivateCtx = pstPlCtx;

    // Callbacks
    pstCtx->cb.pv = pstCtx;
    pstCtx->cb.input_buffer_done = pipeline_ibd;
    pstCtx->cb.output_buffer_done = pipeline_obd;
    pstCtx->cb.event = pipeline_event;

    u32 = ts_mgr_init(&pstPlCtx->stTsMgr, TS_FREQ_15HZ, BUF_POOL_SZ * 2);

    if (u32 == VPP_OK)
    {
        pstPlCtx->stTsMgr.bEnableAsserts = VPP_FALSE;
        pstPlCtx->bTsMgrInit = VPP_TRUE;
    }
    else
    {
        pstPlCtx->bTsMgrInit = VPP_FALSE;
    }

    u32 = vpp_queue_init(&pstPlCtx->bypass.stInputObdOrder, BYPASS_EXP_PTR_Q_SZ);
    rc = pthread_mutex_init(&pstPlCtx->bypass.mutex, NULL);
    if (u32 == VPP_OK && rc == 0)
    {
        pstPlCtx->bypass.bEnableObdInputOrder = VPP_TRUE;
    }
    else
    {
        if (rc == 0)
            pthread_mutex_destroy(&pstPlCtx->bypass.mutex);

        pstPlCtx->bypass.bEnableObdInputOrder = VPP_FALSE;
        DVP_ASSERT_FAIL();
    }
}

static void pipeline_context_term(struct test_ctx *pstCtx,
                                  struct pipeline_test_ctx *pstPlCtx)
{
    if (pstPlCtx->bypass.bEnableObdInputOrder)
    {
        DVP_ASSERT_EQUAL(0, vpp_queue_count(&pstPlCtx->bypass.stInputObdOrder));
        vpp_queue_term(&pstPlCtx->bypass.stInputObdOrder);
        pthread_mutex_destroy(&pstPlCtx->bypass.mutex);
    }

    if (pstPlCtx->bTsMgrInit)
    {
        ts_mgr_destroy(&pstPlCtx->stTsMgr);
        pstPlCtx->bTsMgrInit = VPP_FALSE;
    }

    tctx_common_destroy(pstCtx);
}

static void pipeline_param_init(struct test_ctx *pstCtx, enum clip_reg clip)
{
    if (!pstCtx || !pstCtx->pPrivateCtx)
    {
        LOGE("bad params");
        return;
    }

    struct pipeline_test_ctx *pltctx =
        (struct pipeline_test_ctx *)pstCtx->pPrivateCtx;

    populate_port_params(clip, VPP_PORT_INPUT, &pstCtx->port_param);

    t_StVppParam *pstParam = &pltctx->stParam;
    memset(pstParam, 0, sizeof(t_StVppParam));

    pstParam->u32SessionFlags = 0;
    populate_port_params(clip, VPP_PORT_INPUT, &pstParam->input_port);
    populate_port_params(clip, VPP_PORT_OUTPUT, &pstParam->output_port);
}

static void pipeline_ctrl_init(struct test_ctx *pstCtx, t_StVppUsecase *pstUc,
                               uint32_t bIsAuto)
{
    if (!pstCtx || !pstCtx->pPrivateCtx)
    {
        LOGE("bad params");
        return;
    }

    struct pipeline_test_ctx *pltctx =
        (struct pipeline_test_ctx *)pstCtx->pPrivateCtx;

    t_StVppHqvCtrl *pstCtrl = &pltctx->stCtrl;
    memset(pstCtrl, 0, sizeof(t_StVppHqvCtrl));

    populateInternalCtrl(pstUc, pstCtrl, bIsAuto);

    pstCtrl->demo.process_percent = 100;
}

uint32_t should_rotate_buffers_run(struct test_ctx *pstCtx,
                                   uint32_t u32InQueuedCnt,
                                   uint32_t u32QueueInputCnt,
                                   uint32_t u32OutputFactor,
                                   uint32_t u32BufferAdd)
{
    uint32_t bShouldRun = VPP_FALSE;

    // Add extra buffers "just in case" the IP block is in the middle of
    // processing the buffers and has them dequeued. In this case, what would
    // happen is that when the buffers are returned by the IP block, the IP
    // will eventually have insufficient buffers to finish processing. In multi-IP
    // cases that uses client buffers internally, more buffers are needed since
    // internal IPs may be holding buffers as well.

    pthread_mutex_lock(&pstCtx->mutex);
    if (u32InQueuedCnt < u32QueueInputCnt ||
        pstCtx->u32OutHoldCnt < ((pstCtx->u32InHoldCnt * u32OutputFactor) + u32BufferAdd))
    {
        bShouldRun = VPP_TRUE;
    }
    pthread_mutex_unlock(&pstCtx->mutex);

    // LOGI("should_run=%u, in_hold=%u, out_hold=%u, in_queued=%u",
    //      bShouldRun, pstCtx->u32InHoldCnt, pstCtx->u32OutHoldCnt,
    //      u32InQueuedCnt);
    return bShouldRun;
}

static void rotate_buffers(struct test_ctx *pstCtx,
                           struct buf_pool *pstPool,
                           struct buffer_rotation_cb *callbacks,
                           uint32_t u32QueueInputCnt,
                           uint32_t bIssueEOS,
                           uint32_t u32Ft)
{
    uint32_t u32Ret;
    struct bufnode *pNode;
    struct pipeline_test_ctx *pstPlCtx;
    uint32_t u32InQueuedCnt = 0, u32OutQueuedCnt = 0;

    uint32_t u32OutputFactor = 2;
    uint32_t u32BufferAdd = 2;
    t_StVppUsecase *pstUc;

    LOG_ENTER();

    pstPlCtx = (struct pipeline_test_ctx *)pstCtx->pPrivateCtx;
    pstUc = pstPlCtx->pstUc;

    if (pstPlCtx->bUcInterpolatesOutput)
        u32BufferAdd = 12;

    while (should_rotate_buffers_run(pstCtx, u32InQueuedCnt, u32QueueInputCnt,
                                     u32OutputFactor, u32BufferAdd))
    {


#if 1
        pthread_mutex_lock(&pstCtx->mutex);
        while (NULL == (pNode = get_buf(pstPool)))
            pthread_cond_wait(&pstCtx->cond, &pstCtx->mutex);
        pthread_mutex_unlock(&pstCtx->mutex);
#else
        pNode = get_buf(pstPool);
        if (!pNode)
            continue;
#endif

        if (pstUc && (u32VppUsecase_IsMappedPxBufRequired(pstUc) == VPP_TRUE) &&
            (pstPool->params.eProtection == PROTECTION_ZONE_NONSECURE))
            pNode->pIntBuf->stPixel.eMapping = eVppBuf_MappedInternal;
        else
            pNode->pIntBuf->stPixel.eMapping= eVppBuf_Unmapped;

        pNode->pExtBuf->flags = 0;
        pNode->pIntBuf->u32InternalFlags = 0;
        pthread_mutex_lock(&pstCtx->mutex);

        // LOGI("dbg: in_hold=%u, out_hold=%u, holdxfactor=%u, in_queued=%u, out_queued=%u",
        //      pstCtx->u32InHoldCnt, pstCtx->u32OutHoldCnt,
        //      pstCtx->u32InHoldCnt * u32OutputFactor,
        //      u32InQueuedCnt, u32OutQueuedCnt);

        if (u32InQueuedCnt >= u32QueueInputCnt || !pstCtx->u32OutHoldCnt ||
            pstCtx->u32OutHoldCnt < (pstCtx->u32InHoldCnt * u32OutputFactor))
        {
            // LOGI("u32InHoldCnt=%u, u32OutHoldCnt=%u, q_output",
            //      pstCtx->u32InHoldCnt, pstCtx->u32OutHoldCnt);
            pNode->port_owner = VPP_PORT_OUTPUT;
            u32OutQueuedCnt += 1;
            pstCtx->u32OutPutCnt += 1;
            pstCtx->u32OutHoldCnt += 1;

        }
        else
        {
            // LOGI("u32InHoldCnt=%u, u32OutHoldCnt=%u, q_input",
            //      pstCtx->u32InHoldCnt, pstCtx->u32OutHoldCnt);
            pNode->port_owner = VPP_PORT_INPUT;

            if ((u32InQueuedCnt + 1 == u32QueueInputCnt) && bIssueEOS)
                pNode->pExtBuf->flags |= VPP_BUFFER_FLAG_EOS;

            u32InQueuedCnt += 1;
            pstCtx->u32InPutCnt += 1;
            pstCtx->u32InHoldCnt += 1;
        }

        pthread_mutex_unlock(&pstCtx->mutex);

        if (pNode->port_owner == VPP_PORT_INPUT)
        {
            fill_buf(pNode);

            if (u32Ft < eVppBufType_Max)
                fill_extra_data(pNode, u32Ft, 0);

            pNode->pIntBuf->eBufType = u32Ft;
            pNode->pIntBuf->u32OperatingRate = ts_get_freq(&pstPlCtx->stTsMgr);
            pNode->pIntBuf->u32TimestampFrameRate = pNode->pIntBuf->u32OperatingRate;
            pNode->pExtBuf->timestamp = ts_get_next(&pstPlCtx->stTsMgr);
        }

        if (callbacks && callbacks->cb_before_queue_input &&
            pNode->port_owner == VPP_PORT_INPUT)
            callbacks->cb_before_queue_input(pstCtx, pNode, u32InQueuedCnt - 1);
        else if (callbacks && callbacks->cb_before_queue_output &&
                 pNode->port_owner == VPP_PORT_OUTPUT)
            callbacks->cb_before_queue_output(pstCtx, pNode);

        pNode->pIntBuf->eBufPxType = eVppBufPxDataType_Raw;
        pNode->pIntBuf->eQueuedPort = pNode->port_owner;
        pNode->pIntBuf->eCurrentIpPort = pNode->port_owner;
        LOGI("sending %s buffer: flags=0x%x, ts=%"PRIu64,
             pNode->port_owner == VPP_PORT_INPUT ? "input" : "output",
             pNode->pIntBuf->u32InternalFlags, pNode->pExtBuf->timestamp);
        u32Ret = u32VppPipeline_QueueBuf(&pstCtx->stVppCtx, pNode->port_owner, pNode->pIntBuf);

        if (!pstPlCtx->bBypassAllocLenTest)
            DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

        if (callbacks && callbacks->cb_after_queue_input &&
            pNode->port_owner == VPP_PORT_INPUT)
            callbacks->cb_after_queue_input(pstCtx);
        else if (callbacks && callbacks->cb_after_queue_output &&
                 pNode->port_owner == VPP_PORT_OUTPUT)
            callbacks->cb_after_queue_output(pstCtx);
    }
    LOG_EXIT();
}

static uint32_t usecase_supports_res(t_StVppUsecase *pstUc, t_EVppRes eRes)
{
    uint32_t u32IpCnt;
    t_StVppUsecaseIpCredits *pstIp;

    VPP_RET_IF_NULL(pstUc, VPP_FALSE);

    for (u32IpCnt = 0; u32IpCnt < VPP_IP_MAX; u32IpCnt ++)
    {
        pstIp = &pstUc->credits[u32IpCnt];
        if (pstIp->au32Credit[eRes] == NOT_SUPPORTED)
        {
            return VPP_FALSE;
        }
    }

    return VPP_TRUE;
}

static uint32_t usecase_supports_fmt(t_StVppUsecase *pstUc, t_EVppColorFmt eFmt)
{
    struct vpp_requirements stVppReq;

    VPP_RET_IF_NULL(pstUc, VPP_FALSE);

    if (u32VppUsecase_GetRequirements(pstUc, &stVppReq) != VPP_OK)
    {
        LOGE("Error getting VPP use case requirements");
        return VPP_FALSE;
    }
    else if (stVppReq.in_color_fmt_mask & (1 << eFmt))
        return VPP_TRUE;

    return VPP_FALSE;
}

static uint32_t usecase_get_supported_fmt(t_StVppUsecase *pstUc, t_EVppColorFmt *peFmt)
{
    t_EVppColorFmt eFmt;

    VPP_RET_IF_NULL(pstUc, VPP_ERR_PARAM);
    VPP_RET_IF_NULL(peFmt, VPP_ERR_PARAM);

    for (eFmt = 0; eFmt < VPP_COLOR_FORMAT_MAX; eFmt++)
    {
        if (usecase_supports_fmt(pstUc, eFmt))
        {
            *peFmt = eFmt;
            return VPP_OK;
        }
    }
    LOGE("No supported formats found for pstUc=%p (%s)", pstUc, pstUc->name);
    return VPP_ERR;
}

uint32_t pipeline_wait_eos(struct test_ctx *pstCtx,
                           struct pipeline_test_ctx *pstPlCtx)
{
#ifdef PL_TEST_ENABLE_TIMEOUT
    int rc = 0;
    struct timespec ts;
    get_wait_time(&ts, 10);
#endif

    pthread_mutex_lock(&pstCtx->mutex);
#ifdef PL_TEST_ENABLE_TIMEOUT
    while (rc != ETIMEDOUT &&
           !VPP_FLAG_IS_SET(pstPlCtx->u32TestFlags, TEST_FLAG_EOS_RECEIVED))
        rc = pthread_cond_timedwait(&pstCtx->cond, &pstCtx->mutex, &ts);
    DVP_ASSERT_EQUAL(rc, 0);
#else
    LOGI("waiting for eos...");
    while (!VPP_FLAG_IS_SET(pstPlCtx->u32TestFlags, TEST_FLAG_EOS_RECEIVED))
        pthread_cond_wait(&pstCtx->cond, &pstCtx->mutex);
    LOGI("received eos...");
#endif
    VPP_FLAG_CLR(pstPlCtx->u32TestFlags, TEST_FLAG_EOS_RECEIVED);
    pthread_mutex_unlock(&pstCtx->mutex);

    return VPP_OK;
}

#define PRINT_UC_NAME(uc) uc ? uc->name : "NULL"
#define PRINT_UC(uc) uc, PRINT_UC_NAME(uc)
// #define PL_RUN_SINGLE_RES       2 // 0 = SD, 1 = HD, 2 = FHD, 3 = UHD
// #define PL_RUN_SINGLE_FMT       VPP_COLOR_FORMAT_UBWC_NV12 // t_EVppColorFmt
// #define PL_RUN_SINGLE_SUITE     1 // 0 = pipeline registry, 1 = real registry
// #define PL_RUN_SINGLE_UC        3 // defines which usecase in registry to run
// #define PL_RUN_SINGLE_BUFTYPE   eVppBufType_Interleaved_TFF
// #define PL_RUN_SINGLE_PRINTF

void foreach_usecase_do(struct test_ctx *pstCtx,
                        struct pipeline_test_ctx *pstPlCtx,
                        uint32_t u32DoFlags,
                        test_fnc_cb cb)
{
    if (!cb)
    {
        DVP_ASSERT_FAIL();
        return;
    }

    /*!
     * foreach registry,
     *  foreach usecase in registry,
     *   foreach resolution in usecase,
     *    foreach color format in usecase,
     *     foreach supported frametype
     *      do fnc
     */
    uint32_t i, j, res, fmt;
    uint32_t bSupportedRes, bSupportedFmt;

    for (i = 0; i < 2; i++)
    {
#if defined(PL_RUN_SINGLE_SUITE)
        if (i != PL_RUN_SINGLE_SUITE)
            continue;
#endif

        uint32_t u32RegSz;
        enum test_suite ts;

        // Iterating over the registries
        if (i == 0)
            ts = TEST_SUITE_PIPELINE;
        else
            ts = TEST_SUITE_REAL_REGISTRY;

        get_registry_details(ts, &u32RegSz, NULL, NULL);
        LOGD("%s: test_suite=%u[%u], u32RegSz=%u", TEST_NAME(), ts, i, u32RegSz);

        for (j = 0; j < u32RegSz + 1; j++)
        {
#ifdef PL_RUN_SINGLE_UC
            if (j != PL_RUN_SINGLE_UC) continue;
#endif

            // For each test case in the registry
            t_StVppUsecase *pstUc;
            if (j == u32RegSz)
                pstUc = getUsecaseAuto(ts);
            else
                pstUc = getUsecaseAt(ts, j);

            LOGD("%s test_suite=%u UC=%s[%u]", TEST_NAME(), ts, PRINT_UC_NAME(pstUc), j);
            if (!pstUc)
            {
                LOGE("-- NULL usecase, skipping...");
                DVP_ASSERT_FAIL();
                continue;
            }

            for (res = 0; res < VPP_RESOLUTION_MAX; res++)
            {
#ifdef PL_RUN_SINGLE_RES
                if (res != PL_RUN_SINGLE_RES) continue;
#endif
                for (fmt = 0; fmt < VPP_COLOR_FORMAT_MAX; fmt++)
                {
#ifdef PL_RUN_SINGLE_FMT
                    if (fmt != PL_RUN_SINGLE_FMT) continue;
#endif
                    // Skip NV21 and UBWCNV21
                    if (fmt == VPP_COLOR_FORMAT_NV21_VENUS || fmt == VPP_COLOR_FORMAT_UBWC_NV21)
                        continue;

                    LOGD("*****************************************");
                    LOGD("* Test Name: %s",  TEST_NAME());
                    LOGD("* test_suite: %u, uc: %s[%u], resolution: %u (%s), format: %u (%s)",
                         ts, PRINT_UC_NAME(pstUc), j, res, apcResName[res], fmt, apcFmtName[fmt]);

                    // Figure out the supported buffer types for this usecase
                    // Figure out what input buffer types we will need
                    struct hqv_control ctrl_di = {
                        .mode = HQV_MODE_MANUAL,
                        .ctrl_type = HQV_CONTROL_DI,
                        .di = {
                            .mode = HQV_DI_MODE_AUTO,
                        },
                    };

                    uint32_t u32BufTypeCnt = 0;
                    uint32_t bUcSupportsDi = VPP_FALSE;
                    uint32_t bUcSupportsProg = VPP_FALSE;
                    t_EVppBufType aeBufTypes[eVppBufType_Max] = {0};

                    /*!             | TFF | BFF | P |
                     * DI           |  Y  |  Y  | N |
                     * DI + other   |  Y  |  Y  | Y |
                     * other        |  N  |  N  | Y |
                     */
                    bUcSupportsDi = u32VppUsecase_IsCtrlSupported(pstUc, ctrl_di);
#ifdef VPP_RES_DI_IS_DISABLED_FOR_AUTO
                    if (res >= VPP_RES_DI_IS_DISABLED_FOR_AUTO)
                        bUcSupportsDi = VPP_FALSE;
#endif
                    if (!bUcSupportsDi)
                    {
                        bUcSupportsProg = VPP_TRUE;
                        aeBufTypes[u32BufTypeCnt++] = eVppBufType_Progressive;
                    }
                    else
                    {
                        aeBufTypes[u32BufTypeCnt++] = eVppBufType_Interleaved_TFF;
                        aeBufTypes[u32BufTypeCnt++] = eVppBufType_Interleaved_BFF;
                        if (pstUc->algos.u32CtrlCnt > 1)
                        {
                            bUcSupportsProg = VPP_TRUE;
                            aeBufTypes[u32BufTypeCnt++] = eVppBufType_Progressive;
                        }
                    }

                    // Iterate over resolutions
                    bSupportedRes = usecase_supports_res(pstUc, res);
                    bSupportedFmt = usecase_supports_fmt(pstUc, fmt);

                    if (((bSupportedRes && VPP_FLAG_IS_SET(u32DoFlags, DO_RES_SUPPORTED)) ||
                        (!bSupportedRes && VPP_FLAG_IS_SET(u32DoFlags, DO_RES_NOT_SUPPORTED))) &&
                        bSupportedFmt)
                    {

                        struct pipeline_test_info pltinfo;
                        pltinfo.pstCtx = pstCtx;
                        pltinfo.pstPlCtx = pstPlCtx;
                        pltinfo.pstUc = pstUc;
                        pltinfo.eRes = (t_EVppRes)res;
                        pltinfo.eFmt = (t_EVppColorFmt)fmt;
                        pltinfo.bUcIsAuto = j == u32RegSz ? VPP_TRUE : VPP_FALSE;
                        pltinfo.eOutBufType = eVppBufType_Progressive;
                        pltinfo.bUcSupportsDi = bUcSupportsDi;
                        pltinfo.bUcSupportsProg = bUcSupportsProg;

#ifndef PL_RUN_SINGLE_BUFTYPE
                        if (VPP_FLAG_IS_SET(u32DoFlags, DO_SINGLE_BUFTYPE))
#endif
                        {
#ifndef PL_RUN_SINGLE_BUFTYPE
                            pltinfo.eBufType = eVppBufType_Progressive;
#else
                            pltinfo.eBufType = PL_RUN_SINGLE_BUFTYPE;
#endif
                            LOGD("* buf_type: %u (%s)", pltinfo.eBufType, apcBufTypeName[pltinfo.eBufType]);
                            LOGD("* Executing");
                            LOGD("*****************************************");

                            cb(&pltinfo);
                        }
#ifndef PL_RUN_SINGLE_BUFTYPE
                        else
                        {
                            uint32_t buftype;
                            for (buftype = 0; buftype < u32BufTypeCnt; buftype++)
                            {
                                pltinfo.eBufType = (t_EVppBufType)aeBufTypes[buftype];
                                LOGD("* buf_type: %u (%s)", pltinfo.eBufType, apcBufTypeName[pltinfo.eBufType]);
                                LOGD("* Executing");
                                LOGD("*****************************************");

#ifdef PL_RUN_SINGLE_PRINTF
                                printf("* Test Name: %s suite: %u, uc: %s[%u], "
                                       "res: %u (%s) buf_type: %u (%s)\n",
                                       TEST_NAME(), ts, PRINT_UC_NAME(pstUc), j, res,
                                       apcResName[res], pltinfo.eBufType,
                                       apcBufTypeName[pltinfo.eBufType]);
#endif

                                cb(&pltinfo);
                            }
                        }
#endif
                    }
                    else
                    {
                        LOGD("* Skipping");
                        LOGD("*****************************************");
                    }
                }
            }
        }
    }
}

enum pipeline_pool_index {
    TEST_PL_POOL_BASIC,
    TEST_PL_POOL_RECONFIG,
    TEST_PL_POOL_MAX,
};

static struct buf_pool *aapstPool[TEST_PL_POOL_MAX][VPP_RESOLUTION_MAX];
static t_StVppCtx stVppCtxBufOnly; // Used for buffer pool only

static struct buf_pool *get_pool_for_res(enum pipeline_pool_index idx, t_EVppRes eRes)
{
    if (idx < TEST_PL_POOL_MAX && eRes < VPP_RESOLUTION_MAX)
    {
        LOGI("returning buf pool: %p idx=%u, res=%u", aapstPool[idx][eRes], idx, eRes);
        return aapstPool[idx][eRes];
    }

    return NULL;
}

static void test_init(struct test_ctx *pstCtx,
                      struct pipeline_test_ctx *pstPlCtx,
                      t_StVppUsecase *pstUc,
                      uint32_t bUcIsAuto,
                      enum clip_reg clip,
                      uint32_t bPipelineInit,
                      uint32_t bPipelineOpen)
{
    uint32_t u32Ret;
    uint32_t bIsSecure;
    uint32_t u32ExpectedSize;
    t_EVppRes eRes;

    pipeline_context_init(pstCtx, pstPlCtx);
    pstPlCtx->pstUc = pstUc;
    pstCtx->pPrivateCtx = pstPlCtx;

    eRes = pipeline_get_res(clip);
    pipeline_param_init(pstCtx, clip);
    pipeline_ctrl_init(pstCtx, pstUc, bUcIsAuto);
    pstPlCtx->basic.pool = get_pool_for_res(TEST_PL_POOL_BASIC, eRes);
    if (!pstPlCtx->basic.pool)
    {
        DVP_ASSERT_FAIL();
        LOGE("Could not find buf pool for res=%u", eRes);
        return;
    }

    populate_pool_params(clip, &pstPlCtx->basic.pool->params);
    populate_port_params(clip, VPP_PORT_INPUT, &pstPlCtx->basic.pool->stInputPort);
    populate_port_params(clip, VPP_PORT_OUTPUT, &pstPlCtx->basic.pool->stOutputPort);

    u32ExpectedSize = u32VppUtils_GetPxBufferSize(&pstPlCtx->stParam.output_port);
    pstPlCtx->basic.u32ExpectedFillLen = u32ExpectedSize;

    if (uc_sends_all_input_to_output(pstUc))
    {
        LOGE("uc_sends_all_input_to_output");
        u32ExpectedSize = u32VppUtils_GetPxBufferSize(&pstPlCtx->stParam.input_port);
        pstPlCtx->bUcSendsInputToOutput = VPP_TRUE;
        pstPlCtx->input_to_output.u32ExpectedFillLen = u32ExpectedSize;
    }
    if (uc_frame_interpolate(pstUc))
        pstPlCtx->bUcInterpolatesOutput = VPP_TRUE;

#ifdef PL_ENABLE_PROF
    u32VppStats_SetEnable(&pstCtx->stVppCtx, 0xffffffff);
#endif

#ifdef PL_ENABLE_BUF_DUMP_PATH
    // TODO: pipeline test does not yet support secure session
    bIsSecure = VPP_FALSE;
    u32Ret = u32VppBuf_Init(&pstCtx->stVppCtx, bIsSecure, 0);
    vVppBuf_SetDumpParams(&pstCtx->stVppCtx, VPP_DEFAULT_DUMP_BUF_PATH);
#endif

    u32Ret = u32VppIon_Init(&pstCtx->stVppCtx);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    if (bPipelineInit)
    {
        u32Ret = u32VppPipeline_Init(&pstCtx->stVppCtx, pstCtx->cb);
        DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    }

    if (bPipelineOpen)
    {
        u32Ret = u32VppPipeline_Open(&pstCtx->stVppCtx, pstUc,
                                     &pstPlCtx->stParam, &pstPlCtx->stCtrl);
        DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    }
}

static void test_term(struct test_ctx *pstCtx,
                      struct pipeline_test_ctx *pstPlCtx,
                      uint32_t bPipelineClose,
                      uint32_t bPipelineTerm)
{
    uint32_t u32Ret;

    if (bPipelineClose)
    {
        u32Ret = u32VppPipeline_Close(&pstCtx->stVppCtx);
        DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    }

    if (bPipelineTerm)
    {
        u32Ret = u32VppPipeline_Term(&pstCtx->stVppCtx);
        DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    }

    u32Ret = u32VppIon_Term(&pstCtx->stVppCtx);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);


    pipeline_context_term(pstCtx, pstPlCtx);
}

static void request_flush_port(struct test_ctx *ptctx,
                               struct pipeline_test_ctx *ppltctx,
                               enum vpp_port port,
                               uint32_t bWaitFinish)
{
    uint32_t u32Ret;

#ifdef PL_TEST_ENABLE_TIMEOUT
    int rc = 0;
    struct timespec ts;
    get_wait_time(&ts, 500);
#endif

    if (port == VPP_PORT_INPUT)
    {
        pthread_mutex_lock(&ptctx->mutex);
        VPP_FLAG_SET(ppltctx->u32TestFlags, TEST_FLAG_IN_FLUSH_PEND);
        pthread_mutex_unlock(&ptctx->mutex);

        u32Ret = u32VppPipeline_RequestFlush(&ptctx->stVppCtx, VPP_PORT_INPUT);
        DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

        if (bWaitFinish)
        {
            pthread_mutex_lock(&ptctx->mutex);
#ifdef PL_TEST_ENABLE_TIMEOUT
            while (rc == 0 &&
                   !VPP_FLAG_IS_SET(ppltctx->u32TestFlags, TEST_FLAG_IN_FLUSH_RECEIVED))
                rc = pthread_cond_timedwait(&ptctx->cond, &ptctx->mutex, &ts);
            DVP_ASSERT_EQUAL(rc, 0);
#else
            while (!VPP_FLAG_IS_SET(ppltctx->u32TestFlags, TEST_FLAG_IN_FLUSH_RECEIVED))
                pthread_cond_wait(&ptctx->cond, &ptctx->mutex);
#endif
            VPP_FLAG_CLR(ppltctx->u32TestFlags, TEST_FLAG_IN_FLUSH_RECEIVED);
            pthread_mutex_unlock(&ptctx->mutex);
        }
    }
    else if (port == VPP_PORT_OUTPUT)
    {
        pthread_mutex_lock(&ptctx->mutex);
        VPP_FLAG_SET(ppltctx->u32TestFlags, TEST_FLAG_OUT_FLUSH_PEND);
        pthread_mutex_unlock(&ptctx->mutex);

        u32Ret = u32VppPipeline_RequestFlush(&ptctx->stVppCtx, VPP_PORT_OUTPUT);
        DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

        if (bWaitFinish)
        {
            pthread_mutex_lock(&ptctx->mutex);
#ifdef PL_TEST_ENABLE_TIMEOUT
            while (rc == 0 &&
                   !VPP_FLAG_IS_SET(ppltctx->u32TestFlags, TEST_FLAG_OUT_FLUSH_RECEIVED))
                rc = pthread_cond_timedwait(&ptctx->cond, &ptctx->mutex, &ts);
            DVP_ASSERT_EQUAL(rc, 0);
#else
            while (!VPP_FLAG_IS_SET(ppltctx->u32TestFlags, TEST_FLAG_OUT_FLUSH_RECEIVED))
                pthread_cond_wait(&ptctx->cond, &ptctx->mutex);
#endif

            VPP_FLAG_CLR(ppltctx->u32TestFlags, TEST_FLAG_OUT_FLUSH_RECEIVED);
            pthread_mutex_unlock(&ptctx->mutex);
        }
    }
}

static void bypass_request_flush_port(struct test_ctx *ptctx,
                                      struct pipeline_test_ctx *ppltctx,
                                      enum vpp_port port)
{
    ppltctx->bypass.bFlushing = VPP_TRUE;
    request_flush_port(ptctx, ppltctx, port, VPP_TRUE);
    ppltctx->flush.bFlushedOutputReceived = VPP_FALSE;
    ppltctx->bypass.bFlushing = VPP_FALSE;
}

static void reconfig_request_flush_port(struct test_ctx *ptctx,
                                        struct pipeline_test_ctx *ppltctx,
                                        enum vpp_port port)
{
    if (port == VPP_PORT_OUTPUT)
    {
        pthread_mutex_lock(&ptctx->mutex);
        ppltctx->reconfig.bReconfigFlush = VPP_TRUE;
        ppltctx->reconfig.bFirstFlushedBufRx = VPP_FALSE;
        pthread_mutex_unlock(&ptctx->mutex);
    }

    request_flush_port(ptctx, ppltctx, port, VPP_TRUE);

    if (port == VPP_PORT_OUTPUT)
    {
        pthread_mutex_lock(&ptctx->mutex);
        ppltctx->flush.bFlushedOutputReceived = VPP_FALSE;
        ppltctx->reconfig.bReconfigFlush = VPP_FALSE;
        pthread_mutex_unlock(&ptctx->mutex);
    }

}

static void queue_cnt_to_port(struct test_ctx *pstCtx,
                              enum vpp_port ePort,
                              struct buf_pool *pstPool,
                              uint32_t u32QueueCnt,
                              uint32_t u32ExtFlags,
                              uint32_t u32IntFlags,
                              uint32_t u32Ft)
{
    uint32_t u32PutCnt = 0;
    uint32_t u32Ret;
    struct bufnode *pNode;
    struct pipeline_test_ctx *pstPlCtx;
    t_StVppUsecase *pstUc;

    pstPlCtx = (struct pipeline_test_ctx *)pstCtx->pPrivateCtx;
    if (!pstPlCtx)
    {
        DVP_ASSERT_FAIL();
        LOGE("unable to get pipeline test context from test_ctx=%p", pstCtx);
        return;
    }
    pstUc = pstPlCtx->pstUc;

    while (u32PutCnt < u32QueueCnt)
    {
        pthread_mutex_lock(&pstCtx->mutex);
        while (NULL == (pNode = get_buf(pstPool)))
            pthread_cond_wait(&pstCtx->cond, &pstCtx->mutex);

        if (ePort == VPP_PORT_INPUT)
        {
            pstCtx->u32InPutCnt += 1;
            pstCtx->u32InHoldCnt += 1;
        }
        else
        {
            pstCtx->u32OutPutCnt += 1;
            pstCtx->u32OutHoldCnt += 1;
        }

        pthread_mutex_unlock(&pstCtx->mutex);

        if (ePort == VPP_PORT_INPUT)
        {
            fill_buf(pNode);
            fill_extra_data(pNode, u32Ft, 0);
            pNode->pIntBuf->eBufType = u32Ft;
            // TODO: we need to have a version of this where we do not queue
            // the timestamp to the block (to make sure that it can handle
            // randomized timestamps)
            pNode->pIntBuf->u32OperatingRate = ts_get_freq(&pstPlCtx->stTsMgr);
            pNode->pIntBuf->u32TimestampFrameRate = pNode->pIntBuf->u32OperatingRate;
            pNode->pExtBuf->timestamp = ts_get_next(&pstPlCtx->stTsMgr);
        }

        pNode->port_owner = ePort;

        if (u32ExtFlags)
            pNode->pExtBuf->flags |= u32ExtFlags;

        if (u32IntFlags)
            pNode->pIntBuf->u32InternalFlags = u32IntFlags;

        u32PutCnt += 1;

        if (pstUc && (u32VppUsecase_IsMappedPxBufRequired(pstUc) == VPP_TRUE) &&
            (pstPool->params.eProtection == PROTECTION_ZONE_NONSECURE))
            pNode->pIntBuf->stPixel.eMapping = eVppBuf_MappedInternal;
        else
            pNode->pIntBuf->stPixel.eMapping= eVppBuf_Unmapped;

        pNode->pIntBuf->eBufPxType = eVppBufPxDataType_Raw;
        pNode->pIntBuf->eQueuedPort = pNode->port_owner;
        pNode->pIntBuf->eCurrentIpPort = pNode->port_owner;
        u32Ret = u32VppPipeline_QueueBuf(&pstCtx->stVppCtx, pNode->port_owner,
                                         pNode->pIntBuf);
        DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    }
}

static void set_basic_buf_types(struct pipeline_test_ctx *pstPlCtx,
                                struct pipeline_test_info *pstTest)
{
    pstPlCtx->basic.eQueueBufType = pstTest->eBufType;
    pstPlCtx->basic.eExpectedBufType = pstTest->eOutBufType;
}

static void set_basic_buf_types_same(struct pipeline_test_ctx *pstPlCtx,
                                     t_EVppBufType eBufType)
{
    pstPlCtx->basic.eQueueBufType = eBufType;
    pstPlCtx->basic.eExpectedBufType = eBufType;
}

/************************************************************************
 * Test Functions
 ***********************************************************************/
TEST_SUITE_INIT(PipelineSuiteInit)
{
    uint32_t i, res, fmt, u32;
    struct vpp_port_param stPort;
    struct buf_pool *ppool;
    enum clip_reg clip;
    uint32_t u32BufSize, u32BufSizeTemp;
    struct buf_pool_params pool_params, pool_params_temp;

    u32 = u32VppIon_Init(&stVppCtxBufOnly);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    for (i = 0; i < TEST_PL_POOL_MAX; i++)
    {
        for (res = 0; res < VPP_RESOLUTION_MAX; res++)
        {
            memset(&pool_params, 0, sizeof(struct buf_pool_params));
            u32BufSize = 0;
            for (fmt = 0; fmt < VPP_COLOR_FORMAT_MAX; fmt++)
            {
                clip = pipeline_get_clip((t_EVppRes)res, (t_EVppColorFmt)fmt);
                if (clip == CLIP_NULL)
                    continue;

                memset(&pool_params_temp, 0, sizeof(struct buf_pool_params));

                // Buffer pool default params
                populate_pool_params(clip, &pool_params_temp);
                stPort.width = pool_params_temp.u32Width;
                stPort.height = pool_params_temp.u32Height;
                stPort.fmt = pool_params_temp.eInputBufFmt;
                stPort.stride = u32VppUtils_CalculateStride(stPort.width, stPort.fmt);
                stPort.scanlines = u32VppUtils_CalculateScanlines(stPort.height, stPort.fmt);
                u32BufSizeTemp = u32VppUtils_GetBufferSize(&stPort);
                if (u32BufSizeTemp > u32BufSize)
                {
                    u32BufSize = u32BufSizeTemp;
                    memcpy(&pool_params, &pool_params_temp, sizeof(struct buf_pool_params));
                }
            }

            u32 = init_buf_pool(&stVppCtxBufOnly, &ppool, &pool_params,
                                BUF_POOL_SZ, VPP_FALSE);
            DVP_ASSERT_EQUAL(u32, VPP_OK);

            aapstPool[i][res] = ppool;
        }
    }
}

TEST_SUITE_TERM(PipelineSuiteTerm)
{
    uint32_t i, res, u32;
    struct buf_pool *ppool;

    for (i = 0; i < TEST_PL_POOL_MAX; i++)
    {
        for (res = 0; res < VPP_RESOLUTION_MAX; res++)
        {
            ppool = aapstPool[i][res];
            free_buf_pool(ppool, VPP_FALSE);
        }
    }

    u32 = u32VppIon_Term(&stVppCtxBufOnly);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
}

TEST_SETUP(PipelineTestInit)
{
}

TEST_CLEANUP(PipelineTestTerm)
{
}


#define LOG_PARM(ctx, parm) LOGI(#parm "=%u", ctx->parm)

TEST_CB_DECL(Pipeline_TestBasicBufferRotation)
{
    struct test_ctx *pstCtx = pstTest->pstCtx;
    struct pipeline_test_ctx *pstPlCtx = pstTest->pstPlCtx;

    uint32_t u32InputBufRot = 34;
    enum clip_reg clip = pipeline_get_clip(pstTest->eRes, pstTest->eFmt);

    test_init(pstCtx, pstPlCtx, pstTest->pstUc, pstTest->bUcIsAuto, clip, VPP_TRUE, VPP_TRUE);
    set_basic_buf_types(pstPlCtx, pstTest);

    snprintf(pstPlCtx->basic.pool->params.cOutputName,
             sizeof(pstPlCtx->basic.pool->params.cOutputName),
             "%s_%s_%s_%s_%s.yuv", TEST_NAME(), pstTest->pstUc->name,
             (pstTest->eBufType < eVppBufType_Max) ? apcBufTypeName[pstTest->eBufType] : "NULL",
             (pstTest->eRes < VPP_RESOLUTION_MAX) ? apcResName[pstTest->eRes] : "NULL",
             (pstTest->eFmt < VPP_COLOR_FORMAT_MAX) ? apcFmtName[pstTest->eFmt] : "NULL");

    rotate_buffers(pstCtx, pstPlCtx->basic.pool, NULL, u32InputBufRot,
                   VPP_TRUE, pstTest->eBufType);
    pipeline_wait_eos(pstCtx, pstPlCtx);

    DVP_ASSERT_EQUAL(pstCtx->u32InHoldCnt, 0);

    if (!pstPlCtx->bUcSendsInputToOutput)
    {
        DVP_ASSERT_EQUAL(pstCtx->u32InPutCnt, pstCtx->u32InRxCnt);
    }

    LOG_PARM(pstCtx, u32InRxCnt);
    LOG_PARM(pstCtx, u32OutRxCnt);
    LOG_PARM(pstCtx, u32InPutCnt);
    LOG_PARM(pstCtx, u32OutPutCnt);
    LOG_PARM(pstCtx, u32InHoldCnt);
    LOG_PARM(pstCtx, u32OutHoldCnt);

    request_flush_port(pstCtx, pstPlCtx, VPP_PORT_OUTPUT, VPP_TRUE);
    test_term(pstCtx, pstPlCtx, VPP_TRUE, VPP_TRUE);

#if 0
    // Test: queue extra buffers to get flushed when PL
    // close is called
    {
        uint32_t u32Ret;
        uint32_t u32Tmp;
        struct bufnode *pNode;
        LOGI("starting close/term extra test");
        for (u32Tmp = 0; u32Tmp < 5; u32Tmp++)
        {
            pNode = get_buf(pstPlCtx->basic.pool);
            if (!pNode) continue;
            pNode->port_owner = VPP_PORT_INPUT;
            u32Ret = u32VppPipeline_QueueBuf(&pstCtx->stVppCtx,
                                             VPP_PORT_INPUT,
                                             pNode->pIntBuf);
        }
        for (u32Tmp = 0; u32Tmp < 5; u32Tmp++)
        {
            pNode = get_buf(pstPlCtx->basic.pool);
            if (!pNode) continue;
            pNode->port_owner = VPP_PORT_OUTPUT;
            u32Ret = u32VppPipeline_QueueBuf(&pstCtx->stVppCtx,
                                             VPP_PORT_OUTPUT,
                                             pNode->pIntBuf);
        }
    }
#endif

}

static void dump_port_stats(struct test_ctx *pstCtx)
{
    if (!pstCtx)
        return;

    LOG_PARM(pstCtx, u32InRxCnt);
    LOG_PARM(pstCtx, u32OutRxCnt);
    LOG_PARM(pstCtx, u32InPutCnt);
    LOG_PARM(pstCtx, u32OutPutCnt);
    LOG_PARM(pstCtx, u32InHoldCnt);
    LOG_PARM(pstCtx, u32OutHoldCnt);
}

TEST(Pipeline_TestBasicBufferRotation)
{
    /*!
     * There are two registries that we can pull from here (and both should always
     * pass). The first is the pipeline test registry. Working from pipeline
     * down, we don't care about concurrency, so we don't need to call
     * test_registry_init(). We can just get run the same test over the two
     * registries.
     */

    struct test_ctx tctx;
    struct pipeline_test_ctx pltctx;
    foreach_usecase_do(&tctx, &pltctx, DO_RES_SUPPORTED,
                       TEST_CB_FNC(Pipeline_TestBasicBufferRotation));
}

static void pl_empty_eos_ip_preinput_cb(struct test_ctx *pstCtx, struct bufnode *pNode,
                                        uint32_t u32Idx)
{
    // Make the input an empty eos
    VPP_UNUSED(pstCtx);
    VPP_UNUSED(u32Idx);

    if (!pNode)
        return;

    pNode->pExtBuf->flags |= VPP_BUFFER_FLAG_EOS;
    pNode->pIntBuf->stPixel.u32FilledLen = 0;

}

TEST_CB_DECL(Pipeline_TestBasicEmptyEos)
{
    struct test_ctx *pstCtx = pstTest->pstCtx;
    struct pipeline_test_ctx *pstPlCtx = pstTest->pstPlCtx;

    uint32_t u32InputBufRot = 10;
    enum clip_reg clip = pipeline_get_clip(pstTest->eRes, pstTest->eFmt);

    struct buffer_rotation_cb callbacks = {
        .cb_before_queue_input = pl_empty_eos_ip_preinput_cb,
    };

    test_init(pstCtx, pstPlCtx, pstTest->pstUc, pstTest->bUcIsAuto, clip, VPP_TRUE, VPP_TRUE);
    set_basic_buf_types(pstPlCtx, pstTest);

    pstPlCtx->u32EmptyEosTest = VPP_TRUE;

    // 1. Test that only one buffer can be queued, and that buffer has empty
    //    EOS
    // 2. Test that a few buffers on both input and output can be queued
    //    and that the EOS is bypassed to the output.

    snprintf(pstPlCtx->basic.pool->params.cOutputName,
             sizeof(pstPlCtx->basic.pool->params.cOutputName),
             "%s_%s_%s_%s_%s.yuv", TEST_NAME(), pstTest->pstUc->name,
             (pstTest->eBufType < eVppBufType_Max) ? apcBufTypeName[pstTest->eBufType] : "NULL",
             (pstTest->eRes < VPP_RESOLUTION_MAX) ? apcResName[pstTest->eRes] : "NULL",
             (pstTest->eFmt < VPP_COLOR_FORMAT_MAX) ? apcFmtName[pstTest->eFmt] : "NULL");

#if 1
    rotate_buffers(pstCtx, pstPlCtx->basic.pool, &callbacks, 1,
                   VPP_FALSE, pstTest->eBufType);

    pipeline_wait_eos(pstCtx, pstPlCtx);

    DVP_ASSERT_EQUAL(pstCtx->u32InHoldCnt, 0);
    DVP_ASSERT_EQUAL(pstCtx->u32InRxCnt, 0);
    DVP_ASSERT_EQUAL(pstCtx->u32OutRxCnt, 1);
    DVP_ASSERT_EQUAL(pstCtx->u32InEosRxCnt, 0);
    DVP_ASSERT_EQUAL(pstCtx->u32OutEosRxCnt, 1);

    dump_port_stats(pstCtx);

    request_flush_port(pstCtx, pstPlCtx, VPP_PORT_OUTPUT, VPP_TRUE);
    pstPlCtx->flush.bFlushedOutputReceived = VPP_FALSE;
    DVP_ASSERT_EQUAL(pstCtx->u32InHoldCnt, 0);

    tctx_common_reset_cnt(pstCtx);
#endif

    rotate_buffers(pstCtx, pstPlCtx->basic.pool, NULL, u32InputBufRot,
                   VPP_FALSE, pstTest->eBufType);

    rotate_buffers(pstCtx, pstPlCtx->basic.pool, &callbacks, 1,
                   VPP_FALSE, pstTest->eBufType);

    pipeline_wait_eos(pstCtx, pstPlCtx);

    DVP_ASSERT_EQUAL(pstCtx->u32InHoldCnt, 0);
    DVP_ASSERT_EQUAL(pstCtx->u32InEosRxCnt, 0);
    DVP_ASSERT_EQUAL(pstCtx->u32OutEosRxCnt, 1);

    if (!pstPlCtx->bUcSendsInputToOutput)
    {
        DVP_ASSERT_EQUAL(pstCtx->u32InPutCnt, pstCtx->u32InRxCnt + 1);
    }

    dump_port_stats(pstCtx);

    request_flush_port(pstCtx, pstPlCtx, VPP_PORT_OUTPUT, VPP_TRUE);

    pstPlCtx->u32EmptyEosTest = VPP_FALSE;
    test_term(pstCtx, pstPlCtx, VPP_TRUE, VPP_TRUE);
}

TEST(Pipeline_TestBasicEmptyEos)
{
    struct test_ctx tctx;
    struct pipeline_test_ctx pltctx;
    foreach_usecase_do(&tctx, &pltctx, DO_RES_SUPPORTED,
                       TEST_CB_FNC(Pipeline_TestBasicEmptyEos));
}

#if 0
static void pipeline_test_set_ctrl(struct test_ctx *pstCtx)
{
    uint32_t u32;
    struct pipeline_test_ctx *pltctx;
    t_StVppHqvCtrl *pstCtrl;
    struct hqv_control hqv_ctrl;

    if (!pstCtx || !pstCtx->pPrivateCtx)
        return;

    pltctx = (struct pipeline_test_ctx *)pstCtx->pPrivateCtx;
    pstCtrl = &pltctx->stCtrl;

    memset(&hqv_ctrl, 0, sizeof(struct hqv_control));

    pthread_mutex_lock(&pstCtx->mutex);
    if (pstCtx->u32InPutCnt % 5 == 0)
    {
        pstCtrl->cade.cade_level += 5;
        hqv_ctrl.mode = HQV_MODE_MANUAL;
        hqv_ctrl.ctrl_type = HQV_CONTROL_CADE;
        hqv_ctrl.cade.mode = HQV_MODE_MANUAL;
        hqv_ctrl.cade.cade_level = pstCtrl->cade.cade_level;

        u32 = u32VppPipeline_SetHqvCtrl(&pstCtx->stVppCtx, hqv_ctrl);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
    }
    pthread_mutex_unlock(&pstCtx->mutex);
}

TEST(Pipeline_TestSetCtrlDuringActiveVideo)
{
    uint32_t u32Ret;

    uint32_t u32PoolSz = 10;
    uint32_t u32InputBufRot = 20;

    struct buffer_rotation_cb brcb;
    struct test_ctx tctx;
    struct pipeline_test_ctx pltctx;

    t_StVppUsecase *pstUc = getTestUsecase();

    memset(&pltctx, 0, sizeof(struct pipeline_test_ctx));
    tctx.pPrivateCtx = &pltctx;

    pipeline_context_init(&tctx);
    tctx.pPrivateCtx = &pltctx;

    pipeline_param_init(&tctx);
    pipeline_ctrl_init(&tctx);

    u32Ret = init_buf_pool(&tctx.stVppCtx, &tctx.buf_pool, &tctx.params, u32PoolSz);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    u32Ret = u32VppPipeline_Init(&tctx.stVppCtx, tctx.cb);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    u32Ret = u32VppPipeline_Open(&tctx.stVppCtx, pstUc, &pltctx.stParam, &pltctx.stCtrl);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    brcb.cb_after_queue_input = pipeline_test_set_ctrl;
    brcb.cb_after_queue_output = NULL;

    rotate_buffers(&tctx, tctx.buf_pool, &brcb, u32InputBufRot, VPP_FALSE);

    pthread_mutex_lock(&tctx.mutex);
    while (tctx.u32InHoldCnt != 0 ||
           tctx.u32InRxCnt != u32InputBufRot ||
           tctx.u32OutRxCnt != u32InputBufRot)
    {
        LOGI("waiting for cond");
        pthread_cond_wait(&tctx.cond, &tctx.mutex);
    }

    DVP_ASSERT_EQUAL(tctx.u32InHoldCnt, 0);
    DVP_ASSERT_EQUAL(tctx.u32OutHoldCnt, 0);

    DVP_ASSERT_EQUAL(tctx.u32InPutCnt, tctx.u32InRxCnt);
    DVP_ASSERT_EQUAL(tctx.u32OutPutCnt, tctx.u32OutRxCnt);

    DVP_ASSERT_EQUAL(tctx.u32InPutCnt, tctx.u32InRxCnt);

    LOG_PARM(tctx, u32InRxCnt);
    LOG_PARM(tctx, u32OutRxCnt);
    LOG_PARM(tctx, u32InPutCnt);
    LOG_PARM(tctx, u32OutPutCnt);
    LOG_PARM(tctx, u32InHoldCnt);
    LOG_PARM(tctx, u32OutHoldCnt);
    pthread_mutex_unlock(&tctx.mutex);

    u32Ret = u32VppPipeline_Close(&tctx.stVppCtx);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    u32Ret = u32VppPipeline_Term(&tctx.stVppCtx);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    free_buf_pool(tctx.buf_pool);
    tctx_common_destroy(&tctx);

    LOGI("donedone!");
}

#endif


TEST(Pipeline_FlushBasic)
{
    uint32_t u32Ret;
    t_StVppCtx tctx;
    t_StVppPipelineCb stPlCb;

    memset(&tctx, 0, sizeof(t_StVppCtx));
    memset(&stPlCb, 0, sizeof(t_StVppPipelineCb));

    u32Ret = u32VppPipeline_RequestFlush(NULL, (enum vpp_port)9128392);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppPipeline_RequestFlush(NULL, VPP_PORT_INPUT);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppPipeline_RequestFlush(NULL, VPP_PORT_OUTPUT);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);

    u32Ret = u32VppPipeline_RequestFlush(&tctx, (enum vpp_port)1028390);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppPipeline_RequestFlush(&tctx, VPP_PORT_INPUT);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppPipeline_RequestFlush(&tctx, VPP_PORT_OUTPUT);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);

    tctx.pstPlCb = &stPlCb;
    tctx.pstPlCb->eState = VPP_PLSTATE_NULL;

    u32Ret = u32VppPipeline_RequestFlush(&tctx, VPP_PORT_INPUT);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppPipeline_RequestFlush(&tctx, VPP_PORT_OUTPUT);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);
}

TEST_CB_DECL(Pipeline_FlushEmpty)
{
    struct test_ctx *pstCtx = pstTest->pstCtx;
    struct pipeline_test_ctx *pstPlCtx = pstTest->pstPlCtx;
    enum clip_reg clip = pipeline_get_clip(pstTest->eRes, pstTest->eFmt);

    test_init(pstCtx, pstPlCtx, pstTest->pstUc, pstTest->bUcIsAuto, clip,
              VPP_TRUE, VPP_TRUE);
    set_basic_buf_types(pstPlCtx, pstTest);

    request_flush_port(pstCtx, pstPlCtx, VPP_PORT_INPUT, VPP_TRUE);
    request_flush_port(pstCtx, pstPlCtx, VPP_PORT_INPUT, VPP_TRUE);
    request_flush_port(pstCtx, pstPlCtx, VPP_PORT_INPUT, VPP_TRUE);
    request_flush_port(pstCtx, pstPlCtx, VPP_PORT_INPUT, VPP_TRUE);
    request_flush_port(pstCtx, pstPlCtx, VPP_PORT_INPUT, VPP_TRUE);
    request_flush_port(pstCtx, pstPlCtx, VPP_PORT_INPUT, VPP_TRUE);

    request_flush_port(pstCtx, pstPlCtx, VPP_PORT_OUTPUT, VPP_TRUE);
    request_flush_port(pstCtx, pstPlCtx, VPP_PORT_OUTPUT, VPP_TRUE);
    request_flush_port(pstCtx, pstPlCtx, VPP_PORT_OUTPUT, VPP_TRUE);
    request_flush_port(pstCtx, pstPlCtx, VPP_PORT_OUTPUT, VPP_TRUE);
    request_flush_port(pstCtx, pstPlCtx, VPP_PORT_OUTPUT, VPP_TRUE);
    request_flush_port(pstCtx, pstPlCtx, VPP_PORT_OUTPUT, VPP_TRUE);

    request_flush_port(pstCtx, pstPlCtx, VPP_PORT_INPUT, VPP_TRUE);
    request_flush_port(pstCtx, pstPlCtx, VPP_PORT_OUTPUT, VPP_TRUE);

    request_flush_port(pstCtx, pstPlCtx, VPP_PORT_OUTPUT, VPP_TRUE);
    request_flush_port(pstCtx, pstPlCtx, VPP_PORT_INPUT, VPP_TRUE);

    test_term(pstCtx, pstPlCtx, VPP_TRUE, VPP_TRUE);
}

TEST(Pipeline_FlushEmpty)
{
    /*!
     * Test that if there are no buffers on the input or output port, when we
     * call for a flush, we still get the flush done.
     */

    struct test_ctx tctx;
    struct pipeline_test_ctx pltctx;
    foreach_usecase_do(&tctx, &pltctx, DO_RES_SUPPORTED,
                       TEST_CB_FNC(Pipeline_FlushEmpty));
}

void Pipeline_FlushSimultaneouslyInternal(struct pipeline_test_info *pstTest,
                                          uint32_t bIssueInputFlush)
{
    uint32_t u32InQCnt = 10, u32OutQCnt = 10;

    struct test_ctx *pstCtx = pstTest->pstCtx;
    struct pipeline_test_ctx *pstPlCtx = pstTest->pstPlCtx;
    enum clip_reg clip = pipeline_get_clip(pstTest->eRes, pstTest->eFmt);

    test_init(pstCtx, pstPlCtx, pstTest->pstUc, pstTest->bUcIsAuto, clip,
              VPP_TRUE, VPP_TRUE);
    set_basic_buf_types(pstPlCtx, pstTest);

    if (bIssueInputFlush)
    {
        queue_cnt_to_port(pstCtx, VPP_PORT_INPUT, pstPlCtx->basic.pool,
                          u32InQCnt - 1, 0, 0, pstTest->eBufType);
        queue_cnt_to_port(pstCtx, VPP_PORT_INPUT, pstPlCtx->basic.pool, 1,
                          VPP_BUFFER_FLAG_EOS, 0, pstTest->eBufType);
    }
    else
    {
        queue_cnt_to_port(pstCtx, VPP_PORT_INPUT, pstPlCtx->basic.pool,
                          u32InQCnt - 1, 0, 0, pstTest->eBufType);
    }

    queue_cnt_to_port(pstCtx, VPP_PORT_OUTPUT, pstPlCtx->basic.pool, u32OutQCnt,
                      0, 0, pstTest->eBufType);

    request_flush_port(pstCtx, pstPlCtx, VPP_PORT_INPUT, VPP_FALSE);
    request_flush_port(pstCtx, pstPlCtx, VPP_PORT_OUTPUT, VPP_FALSE);

    pthread_mutex_lock(&pstCtx->mutex);
    while (!VPP_FLAG_IS_SET(pstPlCtx->u32TestFlags, TEST_FLAG_IN_FLUSH_RECEIVED) ||
           !VPP_FLAG_IS_SET(pstPlCtx->u32TestFlags, TEST_FLAG_OUT_FLUSH_RECEIVED))
        pthread_cond_wait(&pstCtx->cond, &pstCtx->mutex);
    VPP_FLAG_CLR(pstPlCtx->u32TestFlags, TEST_FLAG_IN_FLUSH_RECEIVED);
    VPP_FLAG_CLR(pstPlCtx->u32TestFlags, TEST_FLAG_OUT_FLUSH_RECEIVED);
    pthread_mutex_unlock(&pstCtx->mutex);

    test_term(pstCtx, pstPlCtx, VPP_TRUE, VPP_TRUE);
}

TEST_CB_DECL(Pipeline_FlushSimultaneously)
{
    Pipeline_FlushSimultaneouslyInternal(pstTest, VPP_TRUE);
#ifdef RUN_FRC_TESTS_WITHOUT_EOS
    Pipeline_FlushSimultaneouslyInternal(pstTest, VPP_FALSE);
#endif
}

TEST(Pipeline_FlushSimultaneously)
{
    /*!
     * Test that if we call flush on an input and on an output back to back,
     * without waiting for the first to finish, that both still flush and we
     * get them back proper. This test is different from others because others
     * will wait until one port is done before requesting the next to be
     * flushed.
     */

    struct test_ctx tctx;
    struct pipeline_test_ctx pltctx;
    foreach_usecase_do(&tctx, &pltctx, DO_RES_SUPPORTED,
                       TEST_CB_FNC(Pipeline_FlushSimultaneously));

}

void Pipeline_FlushActiveSinglePortInternal(struct pipeline_test_info *pstTest,
                                            uint32_t bIssueInputFlush)
{
    uint32_t u32PoolSz = 10;
    struct test_ctx *pstCtx = pstTest->pstCtx;
    struct pipeline_test_ctx *pstPlCtx = pstTest->pstPlCtx;
    enum clip_reg clip = pipeline_get_clip(pstTest->eRes, pstTest->eFmt);

    test_init(pstCtx, pstPlCtx, pstTest->pstUc, pstTest->bUcIsAuto, clip,
              VPP_TRUE, VPP_TRUE);
    set_basic_buf_types(pstPlCtx, pstTest);

    /*!
     * Input port test
     */
    if (bIssueInputFlush)
    {
        queue_cnt_to_port(pstCtx, VPP_PORT_INPUT, pstPlCtx->basic.pool,
                          u32PoolSz - 1, 0, 0, pstTest->eBufType);
        queue_cnt_to_port(pstCtx, VPP_PORT_INPUT, pstPlCtx->basic.pool,
                          1, VPP_BUFFER_FLAG_EOS, 0, pstTest->eBufType);
    }
    else
    {
        queue_cnt_to_port(pstCtx, VPP_PORT_INPUT, pstPlCtx->basic.pool,
                          u32PoolSz, 0, 0, pstTest->eBufType);
    }
    request_flush_port(pstCtx, pstPlCtx, VPP_PORT_INPUT, VPP_TRUE);

    /*!
     * Output port test
     */
    queue_cnt_to_port(pstCtx, VPP_PORT_OUTPUT, pstPlCtx->basic.pool, u32PoolSz,
                      0, 0, pstTest->eBufType);
    request_flush_port(pstCtx, pstPlCtx, VPP_PORT_OUTPUT, VPP_TRUE);

    test_term(pstCtx, pstPlCtx, VPP_TRUE, VPP_TRUE);
}


TEST_CB_DECL(Pipeline_FlushActiveSinglePort)
{
    Pipeline_FlushActiveSinglePortInternal(pstTest, VPP_TRUE);
#ifdef RUN_FRC_TESTS_WITHOUT_EOS
    Pipeline_FlushActiveSinglePortInternal(pstTest, VPP_FALSE);
#endif
}

TEST(Pipeline_FlushActiveSinglePort)
{
    struct test_ctx tctx;
    struct pipeline_test_ctx pltctx;
    foreach_usecase_do(&tctx, &pltctx, DO_RES_SUPPORTED,
                       TEST_CB_FNC(Pipeline_FlushActiveSinglePort));
}

TEST_CB_DECL(Pipeline_FlushInitedState)
{
    uint32_t u32Ret;
    uint32_t u32PoolSz = 10;

    struct test_ctx *pstCtx = pstTest->pstCtx;
    struct pipeline_test_ctx *pstPlCtx = pstTest->pstPlCtx;
    enum clip_reg clip = pipeline_get_clip(pstTest->eRes, pstTest->eFmt);

    test_init(pstCtx, pstPlCtx, pstTest->pstUc, pstTest->bUcIsAuto, clip,
              VPP_TRUE, VPP_TRUE);
    set_basic_buf_types(pstPlCtx, pstTest);

    // Close forces all of the buffers to be drained from the IP block and
    // transitions the block back into INITED state.
    queue_cnt_to_port(pstCtx, VPP_PORT_INPUT, pstPlCtx->basic.pool, u32PoolSz,
                      0, 0, pstTest->eBufType);

    u32Ret = u32VppPipeline_Close(&pstCtx->stVppCtx);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    request_flush_port(pstCtx, pstPlCtx, VPP_PORT_INPUT, VPP_TRUE);

    // Reopen, queue to output, close, flush
    u32Ret = u32VppPipeline_Open(&pstCtx->stVppCtx, pstTest->pstUc,
                                 &pstPlCtx->stParam, &pstPlCtx->stCtrl);

    queue_cnt_to_port(pstCtx, VPP_PORT_OUTPUT, pstPlCtx->basic.pool, u32PoolSz,
                      0, 0, pstTest->eBufType);

    u32Ret = u32VppPipeline_Close(&pstCtx->stVppCtx);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    request_flush_port(pstCtx, pstPlCtx, VPP_PORT_OUTPUT, VPP_TRUE);

    test_term(pstCtx, pstPlCtx, VPP_TRUE, VPP_TRUE);
}

TEST(Pipeline_FlushInitedState)
{
    struct test_ctx tctx;
    struct pipeline_test_ctx pltctx;
    foreach_usecase_do(&tctx, &pltctx, DO_RES_SUPPORTED,
                       TEST_CB_FNC(Pipeline_FlushInitedState));
}

TEST_CB_DECL(Pipeline_FlushActiveStateNotBypass)
{
    /*!
     * Test that we can flush the pipeline while there are live buffers flowing
     * in both the input and output port. Allocate a sufficiently large buffer
     * pool to ensure that we can queue some buffers to both the input and
     * output port. Split the buffer pool and queue half to the input and half
     * to the output. Flushed buffers on the output should have u32FilledLen set
     * to zero. After queuing buffers to both ports, request a flush on the
     * output port. We may receive a few buffers that are still processed and
     * then buffers with u32FilledLen = 0. Once we receive the first buffer with
     * u32FilledLen = 0, all subsequent buffers should be the same, until we
     * receive a flush done on the output port. We can then flush the input
     * port and get the rest of the buffers. Similarly, we can do the same test
     * on the output, however, there is no guarantee that the buffer's
     * u32FilledLen need be set to zero. In this case, we just need to make sure
     * that we receive all of the input buffers, and that the number of output
     * buffers still held by the pipeline are greater than zero, and that we do
     * not receive an output flush done.
     */

    uint32_t u32InQCnt = 10, u32OutQCnt = 10;

    struct test_ctx *pstCtx = pstTest->pstCtx;
    struct pipeline_test_ctx *pstPlCtx = pstTest->pstPlCtx;
    enum clip_reg clip = pipeline_get_clip(pstTest->eRes, pstTest->eFmt);

    test_init(pstCtx, pstPlCtx, pstTest->pstUc, pstTest->bUcIsAuto, clip,
              VPP_TRUE, VPP_TRUE);
    set_basic_buf_types(pstPlCtx, pstTest);

    queue_cnt_to_port(pstCtx, VPP_PORT_INPUT, pstPlCtx->basic.pool, u32InQCnt,
                      0, 0, pstTest->eBufType);
    queue_cnt_to_port(pstCtx, VPP_PORT_OUTPUT, pstPlCtx->basic.pool, u32OutQCnt,
                      0, 0, pstTest->eBufType);

    // Wait for at least two OBD before we trigger flush
    pthread_mutex_lock(&pstCtx->mutex);
    while (pstCtx->u32OutRxCnt < 2)
        pthread_cond_wait(&pstCtx->cond, &pstCtx->mutex);
    pthread_mutex_unlock(&pstCtx->mutex);

    request_flush_port(pstCtx, pstPlCtx, VPP_PORT_OUTPUT, VPP_TRUE);
    request_flush_port(pstCtx, pstPlCtx, VPP_PORT_INPUT, VPP_TRUE);

    LOGI("Queueing buffers to output for next part of test...");

    // Since we flushed the output, queue back all the buffers to the output
    // port and flush the output this time, after we receive a few buffers.
    // This makes sure that we can still process if we have stuff lingering
    // in a port.
    uint32_t u32ProcCnt = pstCtx->u32OutRxCnt;
    pstPlCtx->flush.bFlushedOutputReceived = VPP_FALSE;
    queue_cnt_to_port(pstCtx, VPP_PORT_INPUT, pstPlCtx->basic.pool, u32InQCnt,
                      0, 0, pstTest->eBufType);
    queue_cnt_to_port(pstCtx, VPP_PORT_OUTPUT, pstPlCtx->basic.pool, u32OutQCnt,
                      0, 0, pstTest->eBufType);

    pthread_mutex_lock(&pstCtx->mutex);
    while (pstCtx->u32OutRxCnt < u32ProcCnt + 2)
        pthread_cond_wait(&pstCtx->cond, &pstCtx->mutex);
    pthread_mutex_unlock(&pstCtx->mutex);

    request_flush_port(pstCtx, pstPlCtx, VPP_PORT_INPUT, VPP_TRUE);

    // Get the rest of the buffers out
    request_flush_port(pstCtx, pstPlCtx, VPP_PORT_OUTPUT, VPP_TRUE);

    test_term(pstCtx, pstPlCtx, VPP_TRUE, VPP_TRUE);
}

TEST(Pipeline_FlushActiveStateNotBypass)
{
    struct test_ctx tctx;
    struct pipeline_test_ctx pltctx;
    foreach_usecase_do(&tctx, &pltctx, DO_RES_SUPPORTED,
                       TEST_CB_FNC(Pipeline_FlushActiveStateNotBypass));
}

static void reconfigure_test_init(struct test_ctx *ptctx,
                                  struct pipeline_test_ctx *ppltctx,
                                  t_StVppUsecase *pstUc,
                                  enum clip_reg clip)
{
    VPP_UNUSED(ptctx);
    t_EVppRes eRes;

    ppltctx->bReconfigureTest = VPP_TRUE;

    // For now, just keep the same test usecase
    ppltctx->reconfig.pstNewUc = pstUc;

    eRes = pipeline_get_res(clip);

    // New port parameters
    populate_port_params(clip, VPP_PORT_INPUT, &ppltctx->reconfig.stNewParams.input_port);
    populate_port_params(clip, VPP_PORT_OUTPUT, &ppltctx->reconfig.stNewParams.output_port);

    // Secondary buffer pool
    ppltctx->reconfig.reconfig_pool = get_pool_for_res(TEST_PL_POOL_RECONFIG, eRes);
    if (!ppltctx->reconfig.reconfig_pool)
    {
        DVP_ASSERT_FAIL();
        LOGE("Could not find reconfig_buf pool for res=%u", eRes);
        return;
    }

    populate_pool_params(clip, &ppltctx->reconfig.reconfig_pool->params);
    populate_port_params(clip, VPP_PORT_INPUT, &ppltctx->reconfig.reconfig_pool->stInputPort);
    populate_port_params(clip, VPP_PORT_OUTPUT, &ppltctx->reconfig.reconfig_pool->stOutputPort);

}

static void reconfigure_test_term(struct test_ctx *ptctx,
                                  struct pipeline_test_ctx *ppltctx)
{
    VPP_UNUSED(ptctx);
    VPP_UNUSED(ppltctx);
}

static void reconfigure_wait_done(struct test_ctx *ptctx,
                                  struct pipeline_test_ctx *ppltctx)
{
    pthread_mutex_lock(&ptctx->mutex);
    while (!VPP_FLAG_IS_SET(ppltctx->u32TestFlags, TEST_FLAG_RECONFIG_RDY) ||
           !VPP_FLAG_IS_SET(ppltctx->u32TestFlags, TEST_FLAG_RECONFIG_DONE))
        pthread_cond_wait(&ptctx->cond, &ptctx->mutex);
    pthread_mutex_unlock(&ptctx->mutex);

    DVP_ASSERT_TRUE(VPP_FLAG_IS_SET(ppltctx->u32TestFlags, TEST_FLAG_RECONFIG_RDY));
    DVP_ASSERT_TRUE(VPP_FLAG_IS_SET(ppltctx->u32TestFlags, TEST_FLAG_RECONFIG_DONE));
    VPP_FLAG_CLR(ppltctx->u32TestFlags, TEST_FLAG_RECONFIG_RDY);
    VPP_FLAG_CLR(ppltctx->u32TestFlags, TEST_FLAG_RECONFIG_DONE);

    // Hold counts should also be zero
    DVP_ASSERT_EQUAL(ptctx->u32InHoldCnt, 0);
}

TEST_CB_DECL(Pipeline_ReconfigureNoBuffers)
{
    uint32_t u32Ret;

    struct test_ctx *pstCtx = pstTest->pstCtx;
    struct pipeline_test_ctx *pstPlCtx = pstTest->pstPlCtx;
    enum clip_reg clip = pipeline_get_clip(pstTest->eRes, pstTest->eFmt);

    test_init(pstCtx, pstPlCtx, pstTest->pstUc, pstTest->bUcIsAuto, clip,
              VPP_TRUE, VPP_TRUE);
    pipeline_ctrl_init(pstCtx, pstTest->pstUc, pstTest->bUcIsAuto);
    reconfigure_test_init(pstCtx, pstPlCtx, pstTest->pstUc, clip);
    set_basic_buf_types(pstPlCtx, pstTest);

    u32Ret = u32VppPipeline_RequestReconfigure(&pstCtx->stVppCtx);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    reconfigure_wait_done(pstCtx, pstPlCtx);

    reconfigure_test_term(pstCtx, pstPlCtx);
    test_term(pstCtx, pstPlCtx, VPP_TRUE, VPP_TRUE);
}

TEST(Pipeline_ReconfigureNoBuffers)
{
    struct test_ctx tctx;
    struct pipeline_test_ctx pltctx;
    foreach_usecase_do(&tctx, &pltctx, DO_RES_SUPPORTED,
                       TEST_CB_FNC(Pipeline_ReconfigureNoBuffers));
}

TEST_CB_DECL(Pipeline_ReconfigureSameUc)
{
    /*!
     * The reconfigure ready callback shouldn't come until all of the input
     * buffers have been drained. This test queues 5 buffers to the input and
     * output ports and immediately requests for a reconfigure. Check to make
     * sure that when we get the reconfigure ready call, that we get all
     * buffers back on the input and output. There are two cases which are
     * currently handled: one-in-one-out algorithms, which process buffers and
     * return the input on the IBD and output on the OBD; and algorithms which
     * send the input to the output, so all buffers are returned on the output
     * port. In the reconfigure callback, depending on the case, check to see
     * if all input buffers are returned on the corresponding ports based on
     * counts (i.e. for the one-in-one-out case, check that the total
     * number of buffers returned on the input port is equal to the number
     * that was queued, and for the in-to-out case, check that the number of
     * buffers that are still held on the output port is equal to the number
     * of buffers queued on the input, plus the number of buffers that are
     * queued on the output, minus the number of buffers held on the output
     * port). Then, reconfigure the pipeline to the new configuration and queue
     * in 5 more buffers of proper size. Make sure that we get all back.
     */

    uint32_t u32QueueInCnt = 5;
    uint32_t u32Ret, i;

    struct test_ctx *pstCtx = pstTest->pstCtx;
    struct pipeline_test_ctx *pstPlCtx = pstTest->pstPlCtx;
    enum clip_reg clip = pipeline_get_clip(pstTest->eRes, pstTest->eFmt);

    enum clip_reg reconf_clip;

    for (i = 0; i < VPP_RESOLUTION_MAX; i++)
    {
        // Make sure that the usecase can support the resolution that we're
        // reconfiguring to, otherwise skip the test.
        if (!usecase_supports_res(pstTest->pstUc, (t_EVppRes)i))
        {
            LOGD("current usecase (%s) does not support res=%u. Skipping.",
                 pstTest->pstUc->name, i);
            continue;
        }
        else
        {
            LOGD("current usecase (%s) will be reconfigured from %u to %u.",
                 pstTest->pstUc->name, pstTest->eRes, i);
        }

        reconf_clip = pipeline_get_clip((t_EVppRes)i, pstTest->eFmt);

        test_init(pstCtx, pstPlCtx, pstTest->pstUc, pstTest->bUcIsAuto, clip,
                  VPP_TRUE, VPP_TRUE);
        pipeline_ctrl_init(pstCtx, pstTest->pstUc, pstTest->bUcIsAuto);
        reconfigure_test_init(pstCtx, pstPlCtx, pstTest->pstUc, reconf_clip);
        set_basic_buf_types(pstPlCtx, pstTest);

        pstPlCtx->reconfig.u32ExpectedFillLen =
            u32VppUtils_GetPxBufferSize(&pstPlCtx->stParam.input_port);

        rotate_buffers(pstCtx, pstPlCtx->basic.pool, NULL, u32QueueInCnt,
                       VPP_FALSE, pstTest->eBufType);

        u32Ret = u32VppPipeline_RequestReconfigure(&pstCtx->stVppCtx);
        DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

        reconfigure_wait_done(pstCtx, pstPlCtx);

        reconfig_request_flush_port(pstCtx, pstPlCtx, VPP_PORT_OUTPUT);

        // Queue in reconfigure sized buffers
        pstPlCtx->reconfig.u32ExpectedFillLen =
            u32VppUtils_GetPxBufferSize(&pstPlCtx->reconfig.stNewParams.input_port);
        rotate_buffers(pstCtx, pstPlCtx->reconfig.reconfig_pool, NULL, u32QueueInCnt,
                       VPP_TRUE, pstTest->eBufType);

        pthread_mutex_lock(&pstCtx->mutex);
        while (!VPP_FLAG_IS_SET(pstPlCtx->u32TestFlags, TEST_FLAG_EOS_RECEIVED))
            pthread_cond_wait(&pstCtx->cond, &pstCtx->mutex);
        VPP_FLAG_CLR(pstPlCtx->u32TestFlags, TEST_FLAG_EOS_RECEIVED);
        pthread_mutex_unlock(&pstCtx->mutex);

        reconfig_request_flush_port(pstCtx, pstPlCtx, VPP_PORT_OUTPUT);

        reconfigure_test_term(pstCtx, pstPlCtx);
        test_term(pstCtx, pstPlCtx, VPP_TRUE, VPP_TRUE);
    }

}

TEST(Pipeline_ReconfigureSameUc)
{
    struct test_ctx tctx;
    struct pipeline_test_ctx pltctx;
    foreach_usecase_do(&tctx, &pltctx, DO_RES_SUPPORTED,
                       TEST_CB_FNC(Pipeline_ReconfigureSameUc));
}

TEST_CB_DECL(Pipeline_ReconfigureDiffUcCompEqual)
{
    /*!
     * Test that we can reconfigure with a different usecase, with the same
     * underlying composition. This translates into a top level fallback
     * usecase.
     */

    uint32_t u32QueueInCnt = 5;
    uint32_t u32Ret, i;
    struct test_ctx *pstCtx = pstTest->pstCtx;
    struct pipeline_test_ctx *pstPlCtx = pstTest->pstPlCtx;
    enum clip_reg clip = pipeline_get_clip(pstTest->eRes, pstTest->eFmt);
    enum clip_reg reconf_clip;

    t_StVppUsecase stUc;

    // We can force the usecase to be different but have the same composition
    // by copying the first usecase to the second usecase.
    memcpy(&stUc, pstTest->pstUc, sizeof(t_StVppUsecase));

    for (i = 0; i < VPP_RESOLUTION_MAX; i++)
    {
        // Make sure that the usecase can support the resolution that we're
        // reconfiguring to, otherwise skip the test.
        if (!usecase_supports_res(pstTest->pstUc, (t_EVppRes)i))
        {
            LOGD("current usecase (%s) does not support res=%u. Skipping.",
                 pstTest->pstUc->name, i);
            continue;
        }
        else
        {
            LOGD("current usecase (%s) will be reconfigured from %u to %u.",
                 pstTest->pstUc->name, pstTest->eRes, i);
        }

        reconf_clip = pipeline_get_clip((t_EVppRes)i, pstTest->eFmt);
        test_init(pstCtx, pstPlCtx, pstTest->pstUc, pstTest->bUcIsAuto, clip,
                  VPP_TRUE, VPP_TRUE);
        pipeline_ctrl_init(pstCtx, pstTest->pstUc, pstTest->bUcIsAuto);
        reconfigure_test_init(pstCtx, pstPlCtx, &stUc, reconf_clip);
        set_basic_buf_types(pstPlCtx, pstTest);


        // Rotate some buffers
        pstPlCtx->reconfig.u32ExpectedFillLen =
            u32VppUtils_GetPxBufferSize(&pstPlCtx->stParam.input_port);
        rotate_buffers(pstCtx, pstPlCtx->basic.pool, NULL, u32QueueInCnt,
                       VPP_FALSE, pstTest->eBufType);

        u32Ret = u32VppPipeline_RequestReconfigure(&pstCtx->stVppCtx);
        DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

        reconfigure_wait_done(pstCtx, pstPlCtx);
        reconfig_request_flush_port(pstCtx, pstPlCtx, VPP_PORT_OUTPUT);

        // Queue in reconfigure sized buffers
        pstPlCtx->reconfig.u32ExpectedFillLen =
            u32VppUtils_GetPxBufferSize(&pstPlCtx->reconfig.stNewParams.input_port);
        rotate_buffers(pstCtx, pstPlCtx->reconfig.reconfig_pool, NULL, u32QueueInCnt,
                       VPP_TRUE, pstTest->eBufType);

        pthread_mutex_lock(&pstCtx->mutex);
        while (!VPP_FLAG_IS_SET(pstPlCtx->u32TestFlags, TEST_FLAG_EOS_RECEIVED))
            pthread_cond_wait(&pstCtx->cond, &pstCtx->mutex);
        VPP_FLAG_CLR(pstPlCtx->u32TestFlags, TEST_FLAG_EOS_RECEIVED);
        pthread_mutex_unlock(&pstCtx->mutex);

        reconfig_request_flush_port(pstCtx, pstPlCtx, VPP_PORT_OUTPUT);

        reconfigure_test_term(pstCtx, pstPlCtx);
        test_term(pstCtx, pstPlCtx, VPP_TRUE, VPP_TRUE);
    }
}

TEST(Pipeline_ReconfigureDiffUcCompEqual)
{
    struct test_ctx tctx;
    struct pipeline_test_ctx pltctx;
    foreach_usecase_do(&tctx, &pltctx, DO_RES_SUPPORTED,
                       TEST_CB_FNC(Pipeline_ReconfigureDiffUcCompEqual));
}

static uint32_t usecase_clone(t_StVppUsecase *pstUcOrig,
                              t_StVppUsecase *pstUcNew,
                              uint32_t bDiffComp,
                              t_StVppIpBlock *pstIp,
                              uint32_t u32IpLen)
{
    VPP_RET_IF_NULL(pstUcOrig, VPP_ERR_PARAM);
    VPP_RET_IF_NULL(pstUcNew, VPP_ERR_PARAM);
    if (bDiffComp && u32IpLen && !pstIp)
        return VPP_ERR_PARAM;

    memcpy(pstUcNew, pstUcOrig, sizeof(t_StVppUsecase));
    if (bDiffComp)
    {
        uint32_t i;
        t_StVppUsecaseIp *pstComposition;
        pstComposition = malloc(u32IpLen * sizeof(t_StVppUsecaseIp));
        if (!pstComposition)
        {
            DVP_ASSERT_FAIL();
            LOGE("Could not allocate memory for pstComposition");
            return VPP_ERR_NO_MEM;
        }
        pstUcNew->composition = pstComposition;
        for (i = 0; i < u32IpLen; i++)
        {
            if ((i < pstUcOrig->u32IpCnt) && (pstUcOrig->composition[i].ip))
            {
                memcpy(&pstIp[i], pstUcOrig->composition[i].ip, sizeof(t_StVppIpBlock));
                pstUcNew->composition[i].ip = &pstIp[i];
            }
            else
                break;
        }
    }
    return VPP_OK;
}

void usecase_clone_term(t_StVppUsecase *pstUcClone)
{
    if (pstUcClone && pstUcClone->composition)
        free(pstUcClone->composition);
}


TEST(Pipeline_ReconfigureDiffUcDiffComp)
{
    /*!
     * Test that we can reconfigure with a different usecase, with a different
     * underlying composition. This translates into a top level fallback
     * usecase.
     */

    uint32_t u32QueueInCnt = 5;
    uint32_t u32Ret, i, j;
    struct test_ctx tctx;
    struct pipeline_test_ctx pltctx;
    t_StVppUsecase stUc, *pstUc;
    t_StVppIpBlock astIp[VPP_IP_BLOCK_MAX];
    t_EVppColorFmt eFmt = VPP_COLOR_FORMAT_NV12_VENUS;

    pstUc = getUsecaseAuto(TEST_SUITE_PIPELINE);
    if (!pstUc)
    {
        DVP_ASSERT_FAIL();
        LOGE("got null usecase for auto! skipping...");
        return;
    }
    usecase_get_supported_fmt(pstUc, &eFmt);

    // In order for us to simulate this, we need to have both a different
    // address for the usecase, and a different address for the underlying IP
    // block.
    u32Ret = usecase_clone(pstUc, &stUc, VPP_TRUE, astIp, pstUc->u32IpCnt);
    if (u32Ret)
    {
        DVP_ASSERT_FAIL();
        LOGE("usecase_clone failed u32Ret=%u. Skipping", u32Ret);
        return;
    }

    enum clip_reg clip, reconf_clip;

    for (i = 0; i < VPP_RESOLUTION_MAX; i++)
    {
        clip = pipeline_get_clip((t_EVppRes)i, eFmt);
        for (j = 0; j < VPP_RESOLUTION_MAX; j++)
        {
            reconf_clip = pipeline_get_clip((t_EVppRes)j, eFmt);

            test_init(&tctx, &pltctx, pstUc, VPP_TRUE, clip, VPP_TRUE, VPP_TRUE);
            pipeline_ctrl_init(&tctx, pstUc, VPP_TRUE);
            reconfigure_test_init(&tctx, &pltctx, &stUc, reconf_clip);
            set_basic_buf_types_same(&pltctx, FT_P);

            // Rotate some buffers
            pltctx.reconfig.u32ExpectedFillLen =
                u32VppUtils_GetPxBufferSize(&pltctx.stParam.input_port);
            rotate_buffers(&tctx, pltctx.basic.pool, NULL, u32QueueInCnt,
                           VPP_FALSE, FT_P);

            queue_cnt_to_port(&tctx, VPP_PORT_OUTPUT, pltctx.basic.pool, 3, 0, 0, FT_P);

            u32Ret = u32VppPipeline_RequestReconfigure(&tctx.stVppCtx);
            DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

            reconfigure_wait_done(&tctx, &pltctx);

            pltctx.reconfig.bReconfigFlush = VPP_TRUE;
            request_flush_port(&tctx, &pltctx, VPP_PORT_INPUT, VPP_TRUE);
            request_flush_port(&tctx, &pltctx, VPP_PORT_OUTPUT, VPP_TRUE);
            pltctx.reconfig.bReconfigFlush = VPP_FALSE;

            // Reset flush flags
            pltctx.flush.bFlushedOutputReceived = VPP_FALSE;

            // Queue in reconfigure sized buffers
            pltctx.reconfig.u32ExpectedFillLen =
                u32VppUtils_GetPxBufferSize(&pltctx.reconfig.stNewParams.input_port);
            rotate_buffers(&tctx, pltctx.reconfig.reconfig_pool, NULL, u32QueueInCnt,
                           VPP_TRUE, FT_P);

            pthread_mutex_lock(&tctx.mutex);
            while (!VPP_FLAG_IS_SET(pltctx.u32TestFlags, TEST_FLAG_EOS_RECEIVED))
                pthread_cond_wait(&tctx.cond, &tctx.mutex);
            VPP_FLAG_CLR(pltctx.u32TestFlags, TEST_FLAG_EOS_RECEIVED);
            pthread_mutex_unlock(&tctx.mutex);

            reconfig_request_flush_port(&tctx, &pltctx, VPP_PORT_OUTPUT);

            reconfigure_test_term(&tctx, &pltctx);
            test_term(&tctx, &pltctx, VPP_TRUE, VPP_TRUE);

        }
    }
    usecase_clone_term(&stUc);
}

void reconfigure_same_res(uint32_t u32QCnt, t_EVppRes eRes, t_EVppColorFmt eFmt,
                          uint32_t u32UcLen, t_StVppUsecase **ppstUc, uint32_t u32TestIdx)
{
    uint32_t u32Ret, u32ExpectedSize, i;
    struct test_ctx tctx, *pstCtx;
    struct pipeline_test_ctx pltctx, *pstPlCtx;
    enum clip_reg clip, reconf_clip;


    if (!u32UcLen)
    {
        DVP_ASSERT_FAIL();
        return;
    }


    pstCtx = &tctx;
    pstPlCtx = &pltctx;
    clip = pipeline_get_clip(eRes, eFmt);
    reconf_clip = clip;

    test_init(&tctx, &pltctx, ppstUc[0], VPP_TRUE, clip, VPP_TRUE, VPP_TRUE);
    pipeline_ctrl_init(&tctx, ppstUc[0], VPP_TRUE);
    reconfigure_test_init(&tctx, &pltctx, NULL, reconf_clip);
    set_basic_buf_types_same(pstPlCtx, FT_P);

    // Overwrite the name of the buf pool so we can dump without overwriting
    // previous tests
    snprintf(pstPlCtx->basic.pool->params.cOutputName,
             sizeof(pstPlCtx->basic.pool->params.cOutputName),
             "%s_%02d.yuv", TEST_NAME(), u32TestIdx);


    for (i = 0; i < u32UcLen; i++)
    {
        // Rotate some buffers
        pltctx.reconfig.u32ExpectedFillLen =
            u32VppUtils_GetPxBufferSize(&pltctx.stParam.input_port);


        if (ppstUc[i] == NULL)
        {
            pltctx.reconfig.bBypass = VPP_TRUE;
            pstPlCtx->bBypassTest = VPP_TRUE;
        }
        else
        {
            pltctx.reconfig.bBypass = VPP_FALSE;
            pstPlCtx->bBypassTest = VPP_FALSE;
        }

        if (i + 1 == u32UcLen)
        {
            rotate_buffers(&tctx, pltctx.basic.pool, NULL, u32QCnt, VPP_TRUE, FT_P);
            break;
        }
        else
        {
            pltctx.reconfig.pstNewUc = ppstUc[i + 1];
        }

        LOGD("Rotating buffers, i=%u", i);
        rotate_buffers(&tctx, pltctx.basic.pool, NULL, u32QCnt, VPP_FALSE, FT_P);

        u32Ret = u32VppPipeline_RequestReconfigure(&tctx.stVppCtx);
        DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

        reconfigure_wait_done(&tctx, &pltctx);
        LOGD("Reconfigure done, i=%u", i);
        if (uc_sends_all_input_to_output(pltctx.reconfig.pstNewUc))
        {
            LOGE("uc_sends_all_input_to_output");
            u32ExpectedSize = u32VppUtils_GetPxBufferSize(&pltctx.stParam.input_port);
            pstPlCtx->bUcSendsInputToOutput = VPP_TRUE;
            pstPlCtx->input_to_output.u32ExpectedFillLen = u32ExpectedSize;
        }
        else
        {
            pstPlCtx->bUcSendsInputToOutput = VPP_FALSE;
        }

        pstPlCtx->bypass.bFlushing = VPP_TRUE;
        pstPlCtx->reconfig.bReconfigFlush = VPP_TRUE;
        pstPlCtx->reconfig.bFirstFlushedBufRx = VPP_FALSE;
        request_flush_port(pstCtx, pstPlCtx, VPP_PORT_OUTPUT, VPP_TRUE);
        pstPlCtx->bypass.bFlushing = VPP_FALSE;
        pstPlCtx->reconfig.bReconfigFlush = VPP_FALSE;
        pstPlCtx->reconfig.bFirstFlushedBufRx = VPP_FALSE;
        pstPlCtx->flush.bFlushedOutputReceived = VPP_FALSE;
        pstPlCtx->flush.u32OutputProcCnt = 0;
        pstPlCtx->flush.u32OutputFlushCnt = 0;

        LOGD("Flush done, i=%u", i);
        tctx_common_reset_cnt(&tctx);
    }

    pthread_mutex_lock(&tctx.mutex);
    while (!VPP_FLAG_IS_SET(pltctx.u32TestFlags, TEST_FLAG_EOS_RECEIVED))
        pthread_cond_wait(&tctx.cond, &tctx.mutex);
    VPP_FLAG_CLR(pltctx.u32TestFlags, TEST_FLAG_EOS_RECEIVED);
    pthread_mutex_unlock(&tctx.mutex);


    if (ppstUc[u32UcLen - 1] == NULL)
    {
        DVP_ASSERT_EQUAL(tctx.u32InRxCnt, 0);
        DVP_ASSERT_EQUAL(tctx.u32OutRxCnt, tctx.u32InPutCnt);
        DVP_ASSERT_EQUAL(tctx.u32OutHoldCnt, tctx.u32OutPutCnt);
    }
    else
    {
        if (pstPlCtx->bUcSendsInputToOutput)
        {
            DVP_ASSERT_EQUAL(tctx.u32InRxCnt, 0);
            DVP_ASSERT_TRUE(tctx.u32OutRxCnt >= tctx.u32InPutCnt);
            DVP_ASSERT_EQUAL(tctx.u32OutHoldCnt, tctx.u32OutPutCnt +
                             tctx.u32InPutCnt - tctx.u32OutRxCnt);
        }
        else
        {
            DVP_ASSERT_EQUAL(tctx.u32InRxCnt, tctx.u32InPutCnt);
            DVP_ASSERT_TRUE(tctx.u32OutRxCnt <= tctx.u32OutPutCnt);
            DVP_ASSERT_EQUAL(tctx.u32OutHoldCnt, tctx.u32OutPutCnt - tctx.u32OutRxCnt);
        }

    }
    DVP_ASSERT_EQUAL(tctx.u32InHoldCnt, 0);

    pstPlCtx->bypass.bFlushing = VPP_TRUE;
    pstPlCtx->reconfig.bReconfigFlush = VPP_TRUE;
    request_flush_port(pstCtx, pstPlCtx, VPP_PORT_OUTPUT, VPP_TRUE);
    pstPlCtx->bypass.bFlushing = VPP_FALSE;
    pstPlCtx->reconfig.bReconfigFlush = VPP_FALSE;

    reconfigure_test_term(&tctx, &pltctx);
    // test term automatically flushes buffers for us
    test_term(&tctx, &pltctx, VPP_TRUE, VPP_TRUE);
}

extern t_StVppUsecase uc_mock_pl_aie, uc_mock_pl_aie2, uc_mock_pl_cade;

struct multi_uc_table {
    uint32_t u32Cnt;
    t_StVppUsecase *apstUc[4];
};

enum tbl_def
{
    TBL_BYPASS,
    TBL_UC_ORIG,
    TBL_UC_COMP_SAME,
    TBL_UC_COMP_DIFF,
};

struct multi_uc_test_def {
    uint32_t u32Cnt;
    enum tbl_def def[4];
};

static struct multi_uc_test_def reconfig_multi_def[] = {
    { 3, { TBL_UC_ORIG, TBL_BYPASS,  TBL_UC_ORIG,      TBL_BYPASS }},
    { 3, { TBL_UC_ORIG, TBL_BYPASS,  TBL_UC_COMP_SAME, TBL_BYPASS }},
    { 3, { TBL_UC_ORIG, TBL_BYPASS,  TBL_UC_COMP_DIFF, TBL_BYPASS }},
    { 2, { TBL_UC_ORIG, TBL_BYPASS,  TBL_BYPASS,       TBL_BYPASS }},
    { 4, { TBL_BYPASS,  TBL_BYPASS,  TBL_BYPASS,       TBL_BYPASS }},
    { 3, { TBL_BYPASS,  TBL_UC_ORIG, TBL_UC_ORIG,      TBL_BYPASS }},
    { 3, { TBL_BYPASS,  TBL_UC_ORIG, TBL_UC_COMP_SAME, TBL_BYPASS }},
    { 3, { TBL_BYPASS,  TBL_UC_ORIG, TBL_UC_COMP_DIFF, TBL_BYPASS }},
    { 4, { TBL_BYPASS,  TBL_UC_ORIG, TBL_BYPASS,       TBL_UC_ORIG }},
    { 4, { TBL_BYPASS,  TBL_UC_ORIG, TBL_BYPASS,       TBL_UC_COMP_SAME }},
    { 4, { TBL_BYPASS,  TBL_UC_ORIG, TBL_BYPASS,       TBL_UC_COMP_DIFF }},
};

void make_test_table_entry(struct multi_uc_test_def *pstDef,
                           struct multi_uc_table *pstEntry,
                           t_StVppUsecase *pstUcOrig,
                           t_StVppUsecase *pstUcSame,
                           t_StVppUsecase *pstUcDiff)
{
    uint32_t i;
    t_StVppUsecase *pstUcTmp;

    pstEntry->u32Cnt = pstDef->u32Cnt;

    for (i = 0; i < pstDef->u32Cnt; i++)
    {
        switch (pstDef->def[i])
        {
            case TBL_UC_ORIG:
                pstUcTmp = pstUcOrig;
                break;
            case TBL_UC_COMP_SAME:
                pstUcTmp = pstUcSame;
                break;
            case TBL_UC_COMP_DIFF:
                pstUcTmp = pstUcDiff;
                break;
            case TBL_BYPASS:
            default:
                pstUcTmp = NULL;
                break;
        }
        pstEntry->apstUc[i] = pstUcTmp;
    }
}

void dump_table_entry(struct multi_uc_table *pstEntry)
{
    uint32_t i, j;
    t_StVppUsecase *pstUc;
    t_StVppUsecaseIp *pstIp;

    for (i = 0; i < pstEntry->u32Cnt; i++)
    {
        pstUc = pstEntry->apstUc[i];
        LOGD("UC=%p (%s)", PRINT_UC(pstUc));

        if (!pstUc)
            continue;

        for (j = 0; j < pstUc->u32IpCnt; j++)
        {
            pstIp = &pstUc->composition[j];
            if (pstIp->ip)
                LOGD("-- %s (%p)", pstIp->ip->name, pstIp->ip);
            else
                continue;
        }
    }
}

TEST(Pipeline_ReconfigureMultiMock)
{
    uint32_t u32QCnt = 5;
    t_EVppRes eRes = VPP_RESOLUTION_HD;
    t_EVppColorFmt eFmt = VPP_COLOR_FORMAT_NV12_VENUS;

    uint32_t u32Sz, i;
    struct multi_uc_table *pstTbl, *pstEntryNew;

    u32Sz = sizeof(reconfig_multi_def) / sizeof(struct multi_uc_test_def);
    LOGD("size = %u", u32Sz);

    pstTbl = calloc(u32Sz, sizeof(struct multi_uc_table));
    if (!pstTbl)
    {
        DVP_ASSERT_FAIL();
        return;
    }

    for (i = 0; i < u32Sz; i++)
    {
        pstEntryNew = &pstTbl[i];

        make_test_table_entry(&reconfig_multi_def[i],
                              pstEntryNew,
                              &uc_mock_pl_aie,
                              &uc_mock_pl_aie2,
                              &uc_mock_pl_cade);

        LOGD("Entry %u", i);
        dump_table_entry(pstEntryNew);

        LOGD("Starting test, multi_index=%u, u32QCnt=%u, eRes=%u, u32UcLen=%u",
             i, u32QCnt, eRes, pstEntryNew->u32Cnt);

        reconfigure_same_res(u32QCnt, eRes, eFmt, pstEntryNew->u32Cnt, pstEntryNew->apstUc, i);
    }

    free(pstTbl);
}

TEST(Pipeline_ReconfigureMultiReal)
{
    uint32_t u32QCnt = 5;
    uint32_t u32Ret;
    t_EVppRes eRes = VPP_RESOLUTION_HD;
    t_EVppColorFmt eFmt = VPP_COLOR_FORMAT_NV12_VENUS;

    uint32_t u32Sz, i;
    struct multi_uc_table *pstTbl, *pstEntryNew;

    u32Sz = sizeof(reconfig_multi_def) / sizeof(struct multi_uc_test_def);
    LOGD("size = %u", u32Sz);

    pstTbl = calloc(u32Sz, sizeof(struct multi_uc_table));
    if (!pstTbl)
    {
        DVP_ASSERT_FAIL();
        return;
    }

    t_StVppUsecase *pstUcOrig, stUcCompEqual, stUcCompDiff;
    t_StVppIpBlock astIp[VPP_IP_BLOCK_MAX];

    // Original usecase
    pstUcOrig = getUsecaseAuto(TEST_SUITE_REAL_REGISTRY);
    usecase_get_supported_fmt(pstUcOrig, &eFmt);

    // Different usecase, comp equal
    u32Ret = usecase_clone(pstUcOrig, &stUcCompEqual, VPP_FALSE, NULL, 0);
    if (u32Ret)
    {
        DVP_ASSERT_FAIL();
        LOGE("usecase_clone failed u32Ret=%u. Skipping", u32Ret);
        return;
    }

    // Different usecase, different composition
    u32Ret = usecase_clone(pstUcOrig, &stUcCompDiff, VPP_TRUE, astIp, pstUcOrig->u32IpCnt);
    if (u32Ret)
    {
        DVP_ASSERT_FAIL();
        LOGE("usecase_clone failed u32Ret=%u. Skipping", u32Ret);
        return;
    }

    for (i = 0; i < u32Sz; i++)
    {
        pstEntryNew = &pstTbl[i];

        make_test_table_entry(&reconfig_multi_def[i],
                              pstEntryNew,
                              pstUcOrig,
                              &stUcCompEqual,
                              &stUcCompDiff);

        LOGD("Entry %u", i);
        dump_table_entry(pstEntryNew);

        LOGD("Starting test, multi_index=%u, u32QCnt=%u, eRes=%u, u32UcLen=%u",
             i, u32QCnt, eRes, pstEntryNew->u32Cnt);
        reconfigure_same_res(u32QCnt, eRes, eFmt, pstEntryNew->u32Cnt, pstEntryNew->apstUc, i);
    }

    free(pstTbl);
    usecase_clone_term(&stUcCompDiff);
}

TEST(Pipeline_ReconfigureNormalToBypass)
{
    /*!
     * Test that we can pass NULL as a usecase into the reconfigure API to
     * indicate that there were no available resources, so the pipeline should
     * put itself into bypass mode.
     */

    uint32_t u32QueueInCnt = 5;
    uint32_t u32Ret;
    struct test_ctx tctx;
    struct pipeline_test_ctx pltctx;
    enum clip_reg clip, reconf_clip;
    t_StVppUsecase *pstUc;
    t_EVppColorFmt eFmt = VPP_COLOR_FORMAT_NV12_VENUS;

#if 0
    pstUc = getUsecaseAuto(TEST_SUITE_PIPELINE);
#else
    pstUc = getUsecaseAuto(TEST_SUITE_REAL_REGISTRY);
#endif
    usecase_get_supported_fmt(pstUc, &eFmt);

    clip = pipeline_get_clip(VPP_RESOLUTION_HD, eFmt);
    reconf_clip = clip;

    test_init(&tctx, &pltctx, pstUc, VPP_TRUE, clip, VPP_TRUE, VPP_TRUE);
    pipeline_ctrl_init(&tctx, pstUc, VPP_TRUE);
    reconfigure_test_init(&tctx, &pltctx, NULL, reconf_clip);
    set_basic_buf_types_same(&pltctx, FT_P);

    // Override the buffer pool and use the same one
    pltctx.reconfig.reconfig_pool = pltctx.basic.pool;

    // Rotate some buffers
    pltctx.reconfig.u32ExpectedFillLen =
        u32VppUtils_GetPxBufferSize(&pltctx.stParam.input_port);
    rotate_buffers(&tctx, pltctx.basic.pool, NULL, u32QueueInCnt, VPP_FALSE, FT_P);

    u32Ret = u32VppPipeline_RequestReconfigure(&tctx.stVppCtx);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    reconfigure_wait_done(&tctx, &pltctx);
    reconfig_request_flush_port(&tctx, &pltctx, VPP_PORT_OUTPUT);

    tctx_common_reset_cnt(&tctx);

    // Set bypass flag so IBD/OBD handlers can assert
    pltctx.reconfig.bBypass = VPP_TRUE;

    // Queue in reconfigure sized buffers
    pltctx.reconfig.u32ExpectedFillLen =
        u32VppUtils_GetPxBufferSize(&pltctx.reconfig.stNewParams.input_port);
    rotate_buffers(&tctx, pltctx.reconfig.reconfig_pool, NULL, u32QueueInCnt,
                   VPP_TRUE, FT_P);

    pthread_mutex_lock(&tctx.mutex);
    while (!VPP_FLAG_IS_SET(pltctx.u32TestFlags, TEST_FLAG_EOS_RECEIVED))
        pthread_cond_wait(&tctx.cond, &tctx.mutex);
    VPP_FLAG_CLR(pltctx.u32TestFlags, TEST_FLAG_EOS_RECEIVED);
    pthread_mutex_unlock(&tctx.mutex);

    DVP_ASSERT_EQUAL(tctx.u32InHoldCnt, 0);
    DVP_ASSERT_EQUAL(tctx.u32OutHoldCnt, tctx.u32OutPutCnt);

    DVP_ASSERT_EQUAL(tctx.u32InRxCnt, 0);
    DVP_ASSERT_EQUAL(tctx.u32OutRxCnt, tctx.u32InPutCnt);

    pltctx.reconfig.bBypass = VPP_FALSE;
    pltctx.reconfig.bReconfigFlush = VPP_TRUE;

    reconfigure_test_term(&tctx, &pltctx);
    // test term automatically flushes buffers for us
    test_term(&tctx, &pltctx, VPP_TRUE, VPP_TRUE);
}

void do_bypass_basic(struct test_ctx *pstCtx,
                     struct pipeline_test_ctx *pstPlCtx,
                     struct buf_pool *pstPool,
                     uint32_t u32InQueueCnt)
{
    pstPlCtx->bBypassTest = VPP_TRUE;
    pstPlCtx->bypass.u32ExpectedFillLen =
        u32VppUtils_GetPxBufferSize(&pstPlCtx->stParam.input_port);
    set_basic_buf_types_same(pstPlCtx, FT_P);

    rotate_buffers(pstCtx, pstPool, NULL, u32InQueueCnt, VPP_TRUE, FT_P);

    pipeline_wait_eos(pstCtx, pstPlCtx);

    DVP_ASSERT_EQUAL(pstCtx->u32InRxCnt, 0);
    DVP_ASSERT_EQUAL(pstCtx->u32InHoldCnt, 0);
    DVP_ASSERT_EQUAL(pstCtx->u32OutRxCnt, pstCtx->u32InPutCnt);
    DVP_ASSERT_EQUAL(pstCtx->u32OutHoldCnt, pstCtx->u32OutPutCnt);

    pstPlCtx->bypass.bFlushing = VPP_TRUE;
    request_flush_port(pstCtx, pstPlCtx, VPP_PORT_OUTPUT, VPP_TRUE);
    pstPlCtx->bypass.bFlushing = VPP_FALSE;
}

TEST(Pipeline_BypassBasic)
{
    struct test_ctx tctx, *pstCtx;
    struct pipeline_test_ctx pltctx, *pstPlCtx;
    enum clip_reg clip;
    t_EVppRes eRes = VPP_RESOLUTION_HD;
    t_EVppColorFmt eFmt = VPP_COLOR_FORMAT_NV12_VENUS;

    struct buf_pool *pstPool;

    pstCtx = &tctx;
    pstPlCtx = &pltctx;

    clip = pipeline_get_clip(eRes, eFmt);
    pstPool = get_pool_for_res(TEST_PL_POOL_BASIC, eRes);
    if (!pstPool)
    {
        DVP_ASSERT_FAIL();
        LOGE("Could not find buf pool for res=%u", eRes);
        return;
    }

    test_init(pstCtx, pstPlCtx, NULL, VPP_FALSE, clip, VPP_TRUE, VPP_TRUE);

    do_bypass_basic(pstCtx, pstPlCtx, pstPool, BUF_POOL_SZ / 3);

    test_term(pstCtx, pstPlCtx, VPP_TRUE, VPP_TRUE);
}

static void pl_bypass_ip_preinput_cb(struct test_ctx *pstCtx, struct bufnode *pNode,
                                     uint32_t u32Idx)
{
    uint32_t u32;
    uint32_t bBypass = VPP_FALSE;
    struct pipeline_test_ctx *pPlTestCtx =
        (struct pipeline_test_ctx *)pstCtx->pPrivateCtx;

    bBypass = u32Idx && (u32Idx % pPlTestCtx->bypass.u32IpBypassMod == 0);

    pNode->pIntBuf->u32BufNum = u32Idx;

    if (bBypass)
    {
        if (pPlTestCtx->bypass.eReason == PL_BYP_REASON_BYP_FLAG)
        {
            LOGD("setting bypass on buffer idx=%u", u32Idx);
            pNode->pIntBuf->u32InternalFlags |= VPP_BUF_FLAG_BYPASS;
        }
        else if (pPlTestCtx->bypass.eReason == PL_BYP_REASON_FL_ZERO)
        {
            LOGD("setting filled_len = 0 on idx=%u", u32Idx);
            pNode->pIntBuf->stPixel.u32FilledLen = 0;
        }
        pPlTestCtx->bypass.u32IpBypassPutCnt += 1;
    }
    else
    {
        LOGD("not setting bypass on buffer idx=%u", u32Idx);
    }

    if (pPlTestCtx->bypass.bEnableObdInputOrder)
    {
        if (bBypass || pPlTestCtx->bUcSendsInputToOutput)
        {
            // For FRC, this is only valid for cases where there are back to
            // back bypass buffers. Otherwise, the FRC algo can opt to send to
            // the output based on frame repeat
            u32 = enqueue_expected_bypass_buffer(pPlTestCtx, pNode->pIntBuf);
            DVP_ASSERT_EQUAL(VPP_OK, u32);
        }
    }
}

static void do_bypass_ip_block(struct pipeline_test_info *pstTest,
                               enum pl_bypass_reason eReason)
{
    uint32_t u32QueueInCnt = 10;
    uint32_t i;

    struct test_ctx *pstCtx = pstTest->pstCtx;
    struct pipeline_test_ctx *pstPlCtx = pstTest->pstPlCtx;
    struct buf_pool *pstPool;
    enum clip_reg clip;

    struct buffer_rotation_cb callbacks = {
        .cb_before_queue_input = pl_bypass_ip_preinput_cb,
    };

    clip = pipeline_get_clip(pstTest->eRes, pstTest->eFmt);
    test_init(pstCtx, pstPlCtx, pstTest->pstUc, pstTest->bUcIsAuto, clip, VPP_TRUE, VPP_TRUE);
    set_basic_buf_types(pstPlCtx, pstTest);

    pstPlCtx->bBypassIpTest = VPP_TRUE;

    pstPlCtx->bypass.u32ExpectedFillLen =
        u32VppUtils_GetPxBufferSize(&pstPlCtx->stParam.input_port);
    pstPool = get_pool_for_res(TEST_PL_POOL_BASIC, pstTest->eRes);
    if (!pstPool)
    {
        DVP_ASSERT_FAIL();
        LOGE("Could not find buf pool for res=%u", pstTest->eRes);
        return;
    }

    for (i = 1; i < 9; i++)
    {
        LOGI("%s, i=%u", __func__, i);
        tctx_common_reset_cnt(pstCtx);
        pstPlCtx->bypass.u32IpBypassPutCnt = 0;
        pstPlCtx->bypass.u32IpBypassRxCnt = 0;
        pstPlCtx->bypass.u32InputRxOnOutput = 0;
        pstPlCtx->flush.u32OutputProcCnt = 0;
        pstPlCtx->flush.u32OutputFlushCnt = 0;

        pstPlCtx->bypass.u32IpBypassMod = i;
        pstPlCtx->bypass.eReason = eReason;
        if (eReason == PL_BYP_REASON_FL_ZERO)
            pstPlCtx->bypass.bEnableObdInputOrder = VPP_TRUE;

        rotate_buffers(pstCtx, pstPool, &callbacks, u32QueueInCnt, VPP_TRUE,
                       pstTest->eBufType);

        pipeline_wait_eos(pstCtx, pstPlCtx);

        if (!pstPlCtx->bUcSendsInputToOutput)
        {
            DVP_ASSERT_EQUAL(pstCtx->u32InRxCnt,
                             pstCtx->u32InPutCnt - pstPlCtx->bypass.u32IpBypassPutCnt);
            DVP_ASSERT_EQUAL(pstPlCtx->bypass.u32IpBypassPutCnt,
                             pstPlCtx->bypass.u32InputRxOnOutput);
        }

        DVP_ASSERT_EQUAL(pstCtx->u32InHoldCnt, 0);

        LOG_U32(I, pstCtx->u32OutHoldCnt);
        LOG_U32(I, pstCtx->u32OutRxCnt);
        LOG_U32(I, pstCtx->u32InPutCnt);

        DVP_ASSERT_TRUE(pstCtx->u32OutRxCnt >= pstCtx->u32InPutCnt);
        DVP_ASSERT_EQUAL(pstPlCtx->bypass.u32IpBypassPutCnt,
                         pstPlCtx->bypass.u32IpBypassRxCnt);

        LOG_PORT_STATS(D, pstCtx);

        pstPlCtx->bypass.bFlushing = VPP_TRUE;
        request_flush_port(pstCtx, pstPlCtx, VPP_PORT_OUTPUT, VPP_TRUE);
        pstPlCtx->flush.bFlushedOutputReceived = VPP_FALSE;
        pstPlCtx->bypass.bFlushing = VPP_FALSE;
    }

    pstPlCtx->bypass.bEnableObdInputOrder = VPP_FALSE;
    pstPlCtx->bBypassIpTest = VPP_FALSE;

    test_term(pstCtx, pstPlCtx, VPP_TRUE, VPP_TRUE);
}

TEST_CB_DECL(Pipeline_BypassIpBlock)
{
    do_bypass_ip_block(pstTest, PL_BYP_REASON_BYP_FLAG);
}

TEST(Pipeline_BypassIpBlock)
{
    struct test_ctx tctx;
    struct pipeline_test_ctx pltctx;
    foreach_usecase_do(&tctx, &pltctx, DO_RES_SUPPORTED,
                       TEST_CB_FNC(Pipeline_BypassIpBlock));
}

TEST_CB_DECL(Pipeline_BypassIpOrder)
{
    uint32_t u32QueueInCnt = 10;

    struct test_ctx *pstCtx = pstTest->pstCtx;
    struct pipeline_test_ctx *pstPlCtx = pstTest->pstPlCtx;
    struct buf_pool *pstPool;
    enum clip_reg clip;

    struct buffer_rotation_cb callbacks = {
        .cb_before_queue_input = pl_bypass_ip_preinput_cb,
    };

    clip = pipeline_get_clip(pstTest->eRes, pstTest->eFmt);
    test_init(pstCtx, pstPlCtx, pstTest->pstUc, pstTest->bUcIsAuto, clip, VPP_TRUE, VPP_TRUE);
    set_basic_buf_types(pstPlCtx, pstTest);

    pstPlCtx->bBypassIpTest = VPP_TRUE;

    pstPlCtx->bypass.u32ExpectedFillLen =
        u32VppUtils_GetPxBufferSize(&pstPlCtx->stParam.input_port);
    pstPool = get_pool_for_res(TEST_PL_POOL_BASIC, pstTest->eRes);
    if (!pstPool)
    {
        DVP_ASSERT_FAIL();
        LOGE("Could not find buf pool for res=%u", pstTest->eRes);
        return;
    }

    pstPlCtx->bypass.u32IpBypassMod = 2;

    rotate_buffers(pstCtx, pstPool, &callbacks, u32QueueInCnt, VPP_TRUE,
                   pstTest->eBufType);

    pipeline_wait_eos(pstCtx, pstPlCtx);

    pstPlCtx->bypass.bFlushing = VPP_TRUE;
    request_flush_port(pstCtx, pstPlCtx, VPP_PORT_INPUT, VPP_TRUE);
    request_flush_port(pstCtx, pstPlCtx, VPP_PORT_OUTPUT, VPP_TRUE);
    pstPlCtx->bypass.bFlushing = VPP_FALSE;

    DVP_ASSERT_EQUAL(0, pstCtx->u32InHoldCnt);
    DVP_ASSERT_EQUAL(0, pstCtx->u32OutHoldCnt);

    pstPlCtx->bBypassIpTest = VPP_FALSE;

    test_term(pstCtx, pstPlCtx, VPP_TRUE, VPP_TRUE);
}

TEST(Pipeline_BypassIpOrder)
{
    struct test_ctx tctx;
    struct pipeline_test_ctx pltctx;
    foreach_usecase_do(&tctx, &pltctx, DO_RES_SUPPORTED,
                       TEST_CB_FNC(Pipeline_BypassIpOrder));
}

TEST(Pipeline_BypassIpError)
{
    /*!
     * Make sure that if there is an issue with initalizing the IP block, the
     * buffers will be bypassed by the pipeline.
     */
    uint32_t u32;
    uint32_t u32InQueueCnt = 10;

    struct test_ctx tctx, *pstCtx;
    struct pipeline_test_ctx pltctx, *pstPlCtx;
    enum clip_reg clip;
    t_EVppRes eRes = VPP_RESOLUTION_HD;
    t_EVppColorFmt eFmt = VPP_COLOR_FORMAT_NV12_VENUS;
    struct buf_pool *pstPool;
    t_StVppUsecase *pstUc;

    pstCtx = &tctx;
    pstPlCtx = &pltctx;

    pstUc = getUsecaseAuto(TEST_SUITE_PIPELINE);
    usecase_get_supported_fmt(pstUc, &eFmt);
    clip = pipeline_get_clip(eRes, eFmt);
    pstPool = get_pool_for_res(TEST_PL_POOL_BASIC, eRes);
    if (!pstPool)
    {
        DVP_ASSERT_FAIL();
        LOGE("Could not find buf pool for res=%u", eRes);
        return;
    }

    test_init(pstCtx, pstPlCtx, pstUc, VPP_TRUE, clip, VPP_TRUE, VPP_FALSE);

    vVppIpMock_SetFailPoint(NULL, MOCK_FAIL_INIT);
    u32 = u32VppPipeline_Open(&pstCtx->stVppCtx, pstUc, &pstPlCtx->stParam,
                              &pstPlCtx->stCtrl);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    do_bypass_basic(pstCtx, pstPlCtx, pstPool, u32InQueueCnt);

    test_term(pstCtx, pstPlCtx, VPP_TRUE, VPP_TRUE);

    vVppIpMock_SetFailPoint(NULL, 0);
}

void do_pipeline_bypass_ip_cnt(enum test_suite ts)
{
    uint32_t u32;
    uint32_t u32InQueueCnt = 10;

    struct test_ctx tctx, *pstCtx;
    struct pipeline_test_ctx pltctx, *pstPlCtx;
    enum clip_reg clip;
    t_EVppRes eRes = VPP_RESOLUTION_HD;
    t_EVppColorFmt eFmt = VPP_COLOR_FORMAT_NV12_VENUS;
    struct buf_pool *pstPool;
    t_StVppUsecase *pstUc;

    pstCtx = &tctx;
    pstPlCtx = &pltctx;

    pstUc = getUsecaseAuto(ts);
    usecase_get_supported_fmt(pstUc, &eFmt);
    clip = pipeline_get_clip(eRes, eFmt);
    pstPool = get_pool_for_res(TEST_PL_POOL_BASIC, eRes);
    if (!pstPool)
    {
        DVP_ASSERT_FAIL();
        LOGE("Could not find buf pool for res=%u", eRes);
        return;
    }

    test_init(pstCtx, pstPlCtx, pstUc, VPP_TRUE, clip, VPP_TRUE, VPP_FALSE);

    u32 = u32VppPipeline_Open(&pstCtx->stVppCtx, pstUc, &pstPlCtx->stParam,
                              &pstPlCtx->stCtrl);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    do_bypass_basic(pstCtx, pstPlCtx, pstPool, u32InQueueCnt);

    test_term(pstCtx, pstPlCtx, VPP_TRUE, VPP_TRUE);
}

#if 0
TEST(Pipeline_BypassIpCntExceedsSupported)
{
    do_pipeline_bypass_ip_cnt(TEST_SUITE_PIPELINE_UC_COMP_IS_TWO);
}
#endif

TEST(Pipeline_BypassIpCntIsZero)
{
    do_pipeline_bypass_ip_cnt(TEST_SUITE_PIPELINE_UC_COMP_IS_ZERO);
}

TEST_CB_DECL(Pipeline_BypassInsufficientBufferSz)
{
    uint32_t u32QueueInCnt = 10;

    struct test_ctx *pstCtx = pstTest->pstCtx;
    struct pipeline_test_ctx *pstPlCtx = pstTest->pstPlCtx;
    struct buf_pool *pstPool;
    enum clip_reg clip, effective_clip;

    clip = pipeline_get_clip(pstTest->eRes, pstTest->eFmt);

    t_EVppRes eResToUse;
    eResToUse = pstTest->eRes > VPP_RESOLUTION_SD ? pstTest->eRes - 1 : VPP_RESOLUTION_MAX;
    if (eResToUse == VPP_RESOLUTION_MAX)
    {
        return;
    }

    // Test will queue buffers that are one resolution step down (i.e. if HD,
    // test will queue SD). Need to figure out the size of the buffers that
    // we should be getting back, for asserts
    struct vpp_port_param port_param;
    effective_clip = pipeline_get_clip(eResToUse, pstTest->eFmt);
    get_res_for_clip(effective_clip, &port_param.width, &port_param.height, &port_param.fmt);
    test_init(pstCtx, pstPlCtx, pstTest->pstUc, pstTest->bUcIsAuto, clip, VPP_TRUE, VPP_TRUE);
    set_basic_buf_types_same(pstPlCtx, FT_P);

    pstPlCtx->bBypassAllocLenTest = VPP_TRUE;

    pstPlCtx->bypass.u32ExpectedFillLen = u32VppUtils_GetPxBufferSize(&port_param);

    pstPool = get_pool_for_res(TEST_PL_POOL_BASIC, eResToUse);
    if (!pstPool)
    {
        DVP_ASSERT_FAIL();
        LOGE("Could not find buf pool for res=%u", eResToUse);
        return;
    }

    pstPlCtx->basic.pool = pstPool;
    populate_pool_params(effective_clip, &pstPlCtx->basic.pool->params);
    populate_port_params(effective_clip, VPP_PORT_INPUT, &pstPlCtx->basic.pool->stInputPort);
    populate_port_params(effective_clip, VPP_PORT_OUTPUT, &pstPlCtx->basic.pool->stOutputPort);

    rotate_buffers(pstCtx, pstPool, NULL, u32QueueInCnt, VPP_TRUE, FT_P);

    pipeline_wait_eos(pstCtx, pstPlCtx);

    pstPlCtx->bBypassAllocLenTest = VPP_TRUE;
    bypass_request_flush_port(pstCtx, pstPlCtx, VPP_PORT_INPUT);
    bypass_request_flush_port(pstCtx, pstPlCtx, VPP_PORT_OUTPUT);

    test_term(pstCtx, pstPlCtx, VPP_TRUE, VPP_TRUE);
}

TEST(Pipeline_BypassInsufficientBufferSz)
{
    struct test_ctx tctx;
    struct pipeline_test_ctx pltctx;
    foreach_usecase_do(&tctx, &pltctx, DO_RES_SUPPORTED,
                       TEST_CB_FNC(Pipeline_BypassInsufficientBufferSz));
}

TEST_CB_DECL(Pipeline_BypassIpFilledLenZero)
{
    do_bypass_ip_block(pstTest, PL_BYP_REASON_FL_ZERO);
}

TEST(Pipeline_BypassIpFilledLenZero)
{
    struct test_ctx tctx;
    struct pipeline_test_ctx pltctx;
    foreach_usecase_do(&tctx, &pltctx, DO_RES_SUPPORTED,
                       TEST_CB_FNC(Pipeline_BypassIpFilledLenZero));
}

static void pl_bypass_input_flush_cb(struct test_ctx *pstCtx,
                                     struct bufnode *pNode,
                                     uint32_t u32Idx)
{
    VPP_UNUSED(pstCtx);
    VPP_UNUSED(u32Idx);

    pNode->pIntBuf->u32InternalFlags |= VPP_BUF_FLAG_BYPASS;
}

TEST(Pipeline_BypassFlush)
{
    struct test_ctx tctx, *pstCtx;
    struct pipeline_test_ctx pltctx, *pstPlCtx;
    enum clip_reg clip;
    t_EVppRes eRes = VPP_RESOLUTION_HD;
    t_EVppColorFmt eFmt = VPP_COLOR_FORMAT_NV12_VENUS;

    struct buf_pool *pstPool;

    struct buffer_rotation_cb callbacks = {
        .cb_before_queue_input = pl_bypass_input_flush_cb,
    };

    pstCtx = &tctx;
    pstPlCtx = &pltctx;

    clip = pipeline_get_clip(eRes, eFmt);

    test_init(pstCtx, pstPlCtx, NULL, VPP_FALSE, clip, VPP_TRUE, VPP_TRUE);
    set_basic_buf_types_same(pstPlCtx, FT_P);

    pstPlCtx->bBypassTest = VPP_TRUE;
    pstPlCtx->bypass.u32ExpectedFillLen =
        u32VppUtils_GetPxBufferSize(&pstPlCtx->stParam.input_port);
    pstPool = get_pool_for_res(TEST_PL_POOL_BASIC, eRes);
    if (!pstPool)
    {
        DVP_ASSERT_FAIL();
        LOGE("Could not find buf pool for res=%u", eRes);
        return;
    }

    // No buffers queued, flush input and output port, one at a time
    pstPlCtx->bypass.bFlushing = VPP_TRUE;
    LOGI("Flushing input, single");
    request_flush_port(pstCtx, pstPlCtx, VPP_PORT_INPUT, VPP_TRUE);
    LOGI("Flushing output, single");
    request_flush_port(pstCtx, pstPlCtx, VPP_PORT_OUTPUT, VPP_TRUE);

    // No buffers queued, flush input and output back to back
    LOGI("Flushing input/output simultaneously");
    request_flush_port(pstCtx, pstPlCtx, VPP_PORT_INPUT, VPP_FALSE);
    request_flush_port(pstCtx, pstPlCtx, VPP_PORT_OUTPUT, VPP_FALSE);
    pthread_mutex_lock(&pstCtx->mutex);
    while (!VPP_FLAG_IS_SET(pstPlCtx->u32TestFlags, TEST_FLAG_IN_FLUSH_RECEIVED) ||
           !VPP_FLAG_IS_SET(pstPlCtx->u32TestFlags, TEST_FLAG_OUT_FLUSH_RECEIVED))
        pthread_cond_wait(&pstCtx->cond, &pstCtx->mutex);
    VPP_FLAG_CLR(pstPlCtx->u32TestFlags, TEST_FLAG_IN_FLUSH_RECEIVED);
    VPP_FLAG_CLR(pstPlCtx->u32TestFlags, TEST_FLAG_OUT_FLUSH_RECEIVED);
    pthread_mutex_unlock(&pstCtx->mutex);
    pstPlCtx->bypass.bFlushing = VPP_FALSE;

    // Queue some buffers and flush them out
    LOGI("Queuing buffers for input flush");
    rotate_buffers(pstCtx, pstPool, &callbacks, BUF_POOL_SZ / 2, VPP_FALSE, FT_P);
    LOGI("Flushing input buffers");
    pstPlCtx->bypass.bFlushing = VPP_TRUE;
    request_flush_port(pstCtx, pstPlCtx, VPP_PORT_INPUT, VPP_TRUE);
    pstPlCtx->bypass.bFlushing = VPP_FALSE;

    pthread_mutex_lock(&pstCtx->mutex);
    uint32_t u32ExpInRxCnt = pstCtx->u32InPutCnt - pstCtx->u32OutRxCnt;
    DVP_ASSERT_EQUAL(pstCtx->u32InRxCnt, u32ExpInRxCnt);
    DVP_ASSERT_EQUAL(pstCtx->u32InHoldCnt, 0);
    DVP_ASSERT_EQUAL(pstCtx->u32OutHoldCnt, pstCtx->u32OutPutCnt);
    pthread_mutex_unlock(&pstCtx->mutex);

    // Flush out all of the buffers in the output port, there should be no
    // input buffers left
    LOGI("Flushing output buffers");
    pstPlCtx->bypass.bFlushing = VPP_TRUE;
    request_flush_port(pstCtx, pstPlCtx, VPP_PORT_OUTPUT, VPP_TRUE);
    pstPlCtx->bypass.bFlushing = VPP_FALSE;

    pthread_mutex_lock(&pstCtx->mutex);
    uint32_t u32ExpOutRxCnt = pstCtx->u32InPutCnt + pstCtx->u32OutPutCnt;
    u32ExpOutRxCnt -= pstCtx->u32InRxCnt;
    DVP_ASSERT_EQUAL(pstCtx->u32OutRxCnt, u32ExpOutRxCnt);
    DVP_ASSERT_EQUAL(pstCtx->u32InHoldCnt, 0);
    DVP_ASSERT_EQUAL(pstCtx->u32OutHoldCnt, 0);
    pthread_mutex_unlock(&pstCtx->mutex);

    tctx_common_reset_cnt(pstCtx);

    // Queue some buffers to both input and output, and flush output
    LOGI("Queuing buffers for output flush");
    rotate_buffers(pstCtx, pstPool, NULL, BUF_POOL_SZ / 2, VPP_TRUE, FT_P);
    pstPlCtx->bypass.bFlushing = VPP_TRUE;
    LOGI("Flushing output buffers");
    request_flush_port(pstCtx, pstPlCtx, VPP_PORT_OUTPUT, VPP_TRUE);
    pstPlCtx->bypass.bFlushing = VPP_FALSE;

    pipeline_wait_eos(pstCtx, pstPlCtx);

    test_term(pstCtx, pstPlCtx, VPP_TRUE, VPP_TRUE);
}


#define RANGE_0     0
#define RANGE_1     10
#define RANGE_2     20
#define RANGE_3     30

static void pl_i_to_p_to_i_preinput_cb(struct test_ctx *pstCtx,
                                       struct bufnode *pNode,
                                       uint32_t u32Idx)
{
    struct pipeline_test_ctx *pstPlCtx =
        (struct pipeline_test_ctx *)pstCtx->pPrivateCtx;

    uint32_t i, u32ExpOutCnt;
    t_EVppBufType eInputBufType = eVppBufType_Progressive;
    t_EVppBufType eOutputBufType;
    enum vpp_port ePortOwner = VPP_PORT_OUTPUT;

    if (u32Idx < RANGE_1 && u32Idx % 2)
        eInputBufType = eVppBufType_Interleaved_TFF;

    else if (u32Idx < RANGE_2 && u32Idx % 3)
        eInputBufType = eVppBufType_Interleaved_TFF;

    else if (u32Idx < RANGE_3 && u32Idx % 4)
        eInputBufType = eVppBufType_Interleaved_TFF;

    else if (u32Idx < RANGE_3 && u32Idx % 5)
        eInputBufType = eVppBufType_Interleaved_TFF;

    if (pstPlCtx->itop.bUcSupportsDi)
    {
        // Deinterlacer enabled
        if (eInputBufType != eVppBufType_Progressive)
        {
            u32ExpOutCnt = 2;
            if (pstPlCtx->bUcInterpolatesOutput)
                u32ExpOutCnt = 4;
            eOutputBufType = eVppBufType_Progressive;
            ePortOwner = VPP_PORT_OUTPUT;
        }
        else if (pstPlCtx->itop.bUcSupportsProg)
        {
            u32ExpOutCnt = 1;
            if (pstPlCtx->bUcInterpolatesOutput)
                u32ExpOutCnt = 2;
            eOutputBufType = eInputBufType;
            ePortOwner = VPP_PORT_OUTPUT;
        }
        else
        {
            u32ExpOutCnt = 1;
            eOutputBufType = eInputBufType;
            ePortOwner = VPP_PORT_INPUT;
        }
    }
    else
    {
        // Deinterlacer disabled
        if (eInputBufType != eVppBufType_Progressive)
        {
            u32ExpOutCnt = 1;
            eOutputBufType = eInputBufType;
            ePortOwner = VPP_PORT_INPUT;
        }
        else
        {
            u32ExpOutCnt = 1;
            if (pstPlCtx->bUcInterpolatesOutput)
                u32ExpOutCnt = 2;
            eOutputBufType = eVppBufType_Progressive;
            ePortOwner = VPP_PORT_OUTPUT;
        }
    }

    fill_extra_data(pNode, eInputBufType, 0);
    pNode->pIntBuf->eBufType = eInputBufType;

    if (!pstPlCtx->bUcSendsInputToOutput)
    {
        LOGE("u32Idx=%u, eBufType=%u", u32Idx, eInputBufType);
        struct expected_buffer stExpBuf;
        for (i = 0; i < u32ExpOutCnt; i++)
        {
            pstPlCtx->itop.u32ExpOutRxCnt += 1;
            stExpBuf.eBufType = eOutputBufType;
            stExpBuf.port_owner = ePortOwner;
            enqueue_expected_type(pstPlCtx, &stExpBuf, VPP_PORT_OUTPUT);
        }

        // If not bypass, expect input buffer
        if (ePortOwner != VPP_PORT_INPUT)
        {
            pstPlCtx->itop.u32ExpInRxCnt += 1;
            pstPlCtx->itop.au32InExpBufCnt[eInputBufType]++;
            stExpBuf.eBufType = eInputBufType;
            stExpBuf.port_owner = VPP_PORT_INPUT;
            enqueue_expected_type(pstPlCtx, &stExpBuf, VPP_PORT_INPUT);
        }

        LOGD("input_buf_type: %-4s, output_buf_type: %-4s, output_port_owner: %d, exp_out_cnt: %d (Total %d)",
             apcBufTypeName[eInputBufType], apcBufTypeName[eOutputBufType],
             ePortOwner, u32ExpOutCnt,pstPlCtx->itop.u32ExpOutRxCnt);
    }
}

TEST_CB_DECL(Pipeline_ItoPPtoI)
{
    struct test_ctx *pstCtx = pstTest->pstCtx;
    struct pipeline_test_ctx *pstPlCtx = pstTest->pstPlCtx;
    uint32_t i;
    uint32_t u32InputBufRot = 50;
    enum clip_reg clip = pipeline_get_clip(pstTest->eRes, pstTest->eFmt);

    struct buffer_rotation_cb callbacks = {
        .cb_before_queue_input = pl_i_to_p_to_i_preinput_cb,
    };

    test_init(pstCtx, pstPlCtx, pstTest->pstUc, pstTest->bUcIsAuto, clip, VPP_TRUE, VPP_TRUE);
    pstPlCtx->pstUc = pstTest->pstUc;
    pthread_mutex_init(&pstPlCtx->itop.mutex, NULL);
    vpp_queue_init(&pstPlCtx->itop.stQInBufType, ITOP_Q_SZ);
    vpp_queue_init(&pstPlCtx->itop.stQOutBufType, ITOP_Q_SZ);

    pstPlCtx->bypass.u32ExpectedFillLen =
        u32VppUtils_GetPxBufferSize(&pstPlCtx->stParam.input_port);

    pstPlCtx->bItoPTest = VPP_TRUE;
    pstPlCtx->itop.bUcSupportsDi = pstTest->bUcSupportsDi;
    pstPlCtx->itop.bUcSupportsProg = pstTest->bUcSupportsProg;

#if 0
    snprintf(pstPlCtx->basic.pool->params.cOutputName,
             sizeof(pstPlCtx->basic.pool->params.cOutputName),
             "%s_%s_%s_%s.yuv", TEST_NAME(), pstTest->pstUc->name,
             (pstTest->eBufType < eVppBufType_Max) ? apcBufTypeName[pstTest->eBufType] : "NULL",
             (pstTest->eRes < VPP_RESOLUTION_MAX) ? apcResName[pstTest->eRes] : "NULL");
#endif

    rotate_buffers(pstCtx, pstPlCtx->basic.pool, &callbacks, u32InputBufRot,
                   VPP_TRUE, eVppBufType_Max);
    pipeline_wait_eos(pstCtx, pstPlCtx);

    DVP_ASSERT_EQUAL(pstCtx->u32InHoldCnt, 0);
    if (pstPlCtx->bUcSendsInputToOutput)
    {
        // All buffers should either be processed or bypassed. Nothing should
        // be returned on the input port.
        DVP_ASSERT_EQUAL(0, pstCtx->u32InRxCnt);
    }
    else
    {
        DVP_ASSERT_EQUAL(pstPlCtx->itop.u32ExpInRxCnt, pstCtx->u32InRxCnt);
        DVP_ASSERT_EQUAL(pstPlCtx->itop.u32ExpOutRxCnt, pstCtx->u32OutRxCnt);
        for (i = 0; i < eVppBufType_Max; i++)
        {
            DVP_ASSERT_EQUAL(pstPlCtx->itop.au32InExpBufCnt[i], 0);
        }
    }

    LOG_PARM(pstCtx, u32InRxCnt);
    LOG_PARM(pstCtx, u32OutRxCnt);
    LOG_PARM(pstCtx, u32InPutCnt);
    LOG_PARM(pstCtx, u32OutPutCnt);
    LOG_PARM(pstCtx, u32InHoldCnt);
    LOG_PARM(pstCtx, u32OutHoldCnt);

    request_flush_port(pstCtx, pstPlCtx, VPP_PORT_OUTPUT, VPP_TRUE);

    vpp_queue_term(&pstPlCtx->itop.stQInBufType);
    vpp_queue_term(&pstPlCtx->itop.stQOutBufType);
    pthread_mutex_destroy(&pstPlCtx->itop.mutex);
    test_term(pstCtx, pstPlCtx, VPP_TRUE, VPP_TRUE);
}

TEST(Pipeline_ItoPPtoI)
{
    struct test_ctx tctx;
    struct pipeline_test_ctx pltctx;
    foreach_usecase_do(&tctx, &pltctx, DO_RES_SUPPORTED|DO_SINGLE_BUFTYPE,
                       TEST_CB_FNC(Pipeline_ItoPPtoI));
}

#define CUST_CTRL_UC_TEST_MAX 2
#define CUST_CTRL_TO_STORE 11
TEST(Pipeline_CustomBasic)
{
    uint32_t u32, i;
    t_StVppPipelineCb *pstPlCb;
    struct test_ctx tctx, *pstCtx;
    struct pipeline_test_ctx pltctx, *pstPlCtx;
    enum clip_reg clip;
    t_EVppRes eRes = VPP_RESOLUTION_HD;
    t_EVppColorFmt eFmt = VPP_COLOR_FORMAT_NV12_VENUS;

    t_StVppUsecase *apstUc[CUST_CTRL_UC_TEST_MAX];
    struct hqv_control ctrl;

    pstCtx = &tctx;
    pstPlCtx = &pltctx;

    // Test only supports two usecase IPs. If more need to be supported,
    // the test case has to be updated.
    DVP_ASSERT_EQUAL(2, CUST_CTRL_UC_TEST_MAX);

    for (i = 0; i < CUST_CTRL_UC_TEST_MAX; i++)
    {
        apstUc[i] = getUsecaseAt(TEST_SUITE_PIPELINE_CUSTOM_CTRL, i);
        DVP_ASSERT_EQUAL(1, u32VppUsecase_SubmodCount(apstUc[i]));
    }
    usecase_get_supported_fmt(apstUc[0], &eFmt);
    clip = pipeline_get_clip(eRes, eFmt);

    test_init(pstCtx, pstPlCtx, apstUc[0], VPP_TRUE, clip, VPP_TRUE, VPP_FALSE);

    memset(&ctrl, 0, sizeof(struct hqv_control));
    ctrl.mode = HQV_MODE_OFF;
    ctrl.ctrl_type = HQV_CONTROL_CUST;
    ctrl.custom.len = sizeof(uint32_t);

    // At this point, pipeline is in INITED state, so all custom controls that
    // are sent should be stored in the internal list.
    for (i = 0; i < CUST_CTRL_TO_STORE; i++)
    {
        uint32_t idx = i % CUST_CTRL_UC_TEST_MAX;

        ctrl.custom.id = apstUc[idx]->composition[0].ip->eIp;

        ((uint32_t *)ctrl.custom.data)[0] = 0xdeadbee0 | i;
        u32 = u32VppPipeline_SetHqvCtrl(&pstCtx->stVppCtx, ctrl);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        pstPlCb = pstCtx->stVppCtx.pstPlCb;
        DVP_ASSERT_EQUAL(pstPlCb->stCustCtrlList.u32Cnt, i + 1);
    }

    // After opening first usecase, expect to see the custom controls that
    // were destined for that first usecase to be gone.
    uint32_t u32ExpToFirst, u32ExpRemain;
    u32ExpToFirst = CUST_CTRL_TO_STORE / CUST_CTRL_UC_TEST_MAX;
    if (CUST_CTRL_TO_STORE % CUST_CTRL_UC_TEST_MAX > 0)
        u32ExpToFirst += 1;
    u32ExpRemain = CUST_CTRL_TO_STORE - u32ExpToFirst;
    LOGI("u32ExpToFirst=%u, u32ExpRemain=%u", u32ExpToFirst, u32ExpRemain);

    u32 = u32VppPipeline_Open(&pstCtx->stVppCtx, apstUc[0], &pstPlCtx->stParam,
                              &pstPlCtx->stCtrl);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    DVP_ASSERT_EQUAL(pstPlCb->stCustCtrlList.u32Cnt, u32ExpRemain);

    // Now that pipeline is opened, we can send some more controls in,
    // except that for the IP that is opened, the controls should go directly
    // to the IP block and for the IPs that are not used, the controls should
    // go to the store.
    for (i = 0; i < CUST_CTRL_TO_STORE; i++)
    {
        uint32_t idx = i % CUST_CTRL_UC_TEST_MAX;
        ctrl.custom.id = apstUc[idx]->composition[0].ip->eIp;
        if (idx)
            u32ExpRemain += 1;

        ((uint32_t *)ctrl.custom.data)[0] = 0xcafebab0 | i;
        u32 = u32VppPipeline_SetHqvCtrl(&pstCtx->stVppCtx, ctrl);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
        DVP_ASSERT_EQUAL(pstPlCb->stCustCtrlList.u32Cnt, u32ExpRemain);
    }

    u32 = u32VppPipeline_Close(&pstCtx->stVppCtx);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // Open the second session that exists, and all other controls should
    // get sent to this.
    u32 = u32VppPipeline_Open(&pstCtx->stVppCtx, apstUc[1], &pstPlCtx->stParam,
                              &pstPlCtx->stCtrl);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    DVP_ASSERT_EQUAL(pstPlCb->stCustCtrlList.u32Cnt, 0);

    test_term(pstCtx, pstPlCtx, VPP_TRUE, VPP_TRUE);
}

#define TEST_PL_BASIC       1
#define TEST_PL_FLUSH       1
#define TEST_PL_RECONFIG    1
#define TEST_PL_BYPASS      1
#define TEST_I_TO_P         1
#define TEST_CUSTOM_CTRL    1

/************************************************************************
 * Global Functions
 ***********************************************************************/
TEST_CASES PipelineTests[] = {
#if TEST_PL_BASIC
    TEST_CASE(Pipeline_TestBasicBufferRotation),
    TEST_CASE(Pipeline_TestBasicEmptyEos),
    // TEST_CASE(Pipeline_TestSetCtrlDuringActiveVideo),
#endif
#if TEST_PL_FLUSH
    TEST_CASE(Pipeline_FlushBasic),
    TEST_CASE(Pipeline_FlushEmpty),
    TEST_CASE(Pipeline_FlushSimultaneously),
    TEST_CASE(Pipeline_FlushActiveSinglePort),
    TEST_CASE(Pipeline_FlushInitedState),
    TEST_CASE(Pipeline_FlushActiveStateNotBypass),
#endif
#if TEST_PL_RECONFIG
    TEST_CASE(Pipeline_ReconfigureNoBuffers),
    TEST_CASE(Pipeline_ReconfigureSameUc),
    TEST_CASE(Pipeline_ReconfigureDiffUcCompEqual),
    TEST_CASE(Pipeline_ReconfigureDiffUcDiffComp),
#endif
#if TEST_PL_BYPASS || TEST_PL_RECONFIG
    TEST_CASE(Pipeline_ReconfigureNormalToBypass),
    TEST_CASE(Pipeline_ReconfigureMultiReal),
    TEST_CASE(Pipeline_ReconfigureMultiMock),
#endif
#if TEST_PL_BYPASS
    TEST_CASE(Pipeline_BypassBasic),
    TEST_CASE(Pipeline_BypassIpBlock),
    TEST_CASE(Pipeline_BypassIpOrder),
    TEST_CASE(Pipeline_BypassIpError),
//    TEST_CASE(Pipeline_BypassIpCntExceedsSupported),
    TEST_CASE(Pipeline_BypassIpCntIsZero),
    TEST_CASE(Pipeline_BypassInsufficientBufferSz),
    TEST_CASE(Pipeline_BypassIpFilledLenZero),
#endif
#if TEST_PL_BYPASS || TEST_PL_FLUSH
    TEST_CASE(Pipeline_BypassFlush),
#endif
#if TEST_I_TO_P
    TEST_CASE(Pipeline_ItoPPtoI),
#endif
#if TEST_CUSTOM_CTRL
    TEST_CASE(Pipeline_CustomBasic),
#endif
    TEST_CASE_NULL(),
};

TEST_SUITE(PipelineSuite,
           "PipelineTests",
           PipelineSuiteInit,
           PipelineSuiteTerm,
           PipelineTestInit,
           PipelineTestTerm,
           PipelineTests);
