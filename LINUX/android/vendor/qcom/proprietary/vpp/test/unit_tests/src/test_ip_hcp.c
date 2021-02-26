/*!
 * @file test_ip_hcp.c
 *
 * @cr
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.

 * @services    Tests IP block related routines
 */

#include <sys/types.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include "AEEStdDef.h"

#include "dvpTest.h"
#include "dvpTest_tb.h"

#define VPP_LOG_TAG     VPP_LOG_UT_IP_TAG
#define VPP_LOG_MODULE  VPP_LOG_UT_IP
#include "vpp_dbg.h"
#include "vpp.h"
#include "vpp_ctx.h"
#include "vpp_ip_hcp.h"
#include "vpp_ip_hcp_priv.h"

#include "test_utils.h"
#include "buf_pool.h"
#include "vpp_utils.h"

/************************************************************************
 * Local definitions
 ***********************************************************************/
typedef struct {
    uint32_t bCpStatus;
    struct hqv_control stHqvCtrl;
    struct vpp_port_param stPortParam;
} t_StHcpIpTestSessCfg;

struct hcp_test_ctx {
    // To be set before calling TestSetup(), or will use default values.
    t_StHcpIpTestSessCfg stCfg;

    uint32_t u32TestFlags;
    t_StVppIpHcpSessionCb *pstHcpCb;
    uint32_t u32ExpExtraLen;
    uint32_t u32NumExpectedBypassBufs;
    uint32_t u32BypassCnt;
    uint32_t u32DrainCnt;
};

struct hcp_knobs {
    uint32_t u32EarMode;
    uint32_t u32DeGain;
    uint32_t u32SatGain;
    uint32_t u32SatOff;
    uint32_t u32AceStrCon;
    uint32_t u32AceBriL;
    uint32_t u32AceBriH;
};

#define INPUT_FLUSH_DONE        (1<<0)
#define OUTPUT_FLUSH_DONE       (1<<1)
#define DRAIN_DONE              (1<<2)

typedef enum {
    eHcpIpTest_CalledInit,
    eHcpIpTest_CalledSetParam,
    eHcpIpTest_CalledOpen
} t_EHcpIpTestFlags;

#define HCPIPTEST_CALLED_INIT           (1 << eHcpIpTest_CalledInit)
#define HCPIPTEST_CALLED_SETPARAM       (1 << eHcpIpTest_CalledSetParam)
#define HCPIPTEST_CALLED_OPEN           (1 << eHcpIpTest_CalledOpen)

#define HCPIPTEST_HCP_TEST_CTX(p)       ((struct hcp_test_ctx *)(p)->pPrivateCtx)

/************************************************************************
 * Local static variables
 ***********************************************************************/
static struct test_ctx stTestCtx;

/************************************************************************
 * Forward Declarations
 ************************************************************************/
typedef void (*assert_fnc)(int, uint32_t, const char *, const char *, const char *);
extern AEEResult u32HcpMock_EnableErrorAction(assert_fnc pFunc,
                                              uint32_t u32Hang,
                                              uint32_t bEnable);
#pragma weak u32HcpMock_EnableErrorAction

/************************************************************************
 * Local Functions
 ***********************************************************************/
static void test_hcp_ibd(void *pv, t_StVppBuf *pstBuf)
{
    LOGI("Running test_hcp_ibd");

    if (!pv || !pstBuf)
    {
        DVP_ASSERT_FAIL();
        LOGE("Null ptrs given. pv=%p, pstBuf=%p", pv, pstBuf);
        return;
    }

    struct bufnode *pNode = pstBuf->pBuf->cookie;
    struct test_ctx *pstTestCtx = (struct test_ctx *)pv;

    LOGI("IBD_owner:%d", pNode->port_owner);

    DVP_ASSERT_TRUE(pNode->owner == BUF_OWNER_LIBRARY);
    pNode->owner = BUF_OWNER_CLIENT;

    put_buf(pstTestCtx->buf_pool, pNode);
    pthread_mutex_lock(&pstTestCtx->mutex);
    pstTestCtx->u32InRxCnt++;
    pstTestCtx->u32InHoldCnt--;
    pthread_mutex_unlock(&pstTestCtx->mutex);
    LOGI("-> Done: %u IBDs processed", pstTestCtx->u32InRxCnt);

    pthread_cond_signal(&pstTestCtx->cond);
}

static void test_hcp_obd(void *pv, t_StVppBuf *pstBuf)
{
    LOGI("Running test_hcp_obd");

    if (!pv || !pstBuf)
    {
        DVP_ASSERT_FAIL();
        LOGE("Null ptrs given. pv=%p, pstBuf=%p", pv, pstBuf);
        return;
    }

    struct bufnode *pNode = pstBuf->pBuf->cookie;
    struct test_ctx *pstTestCtx = (struct test_ctx *)pv;
    struct hcp_test_ctx *pstHcpTestCtx = HCPIPTEST_HCP_TEST_CTX(pstTestCtx);

    LOGI("INJECT_OBD:internal_buf_owner=%d", pstBuf->eQueuedPort);

    DVP_ASSERT_TRUE(pNode->owner == BUF_OWNER_LIBRARY);
    pNode->owner = BUF_OWNER_CLIENT;

    LOGI("OBD_owner:%d", pNode->port_owner);

    if (pNode->pIntBuf->stPixel.u32FilledLen)
    {
        dump_buf(pNode);
        DVP_ASSERT_EQUAL(pNode->pIntBuf->stExtra.u32FilledLen,
                         pstHcpTestCtx->u32ExpExtraLen);
        LOGI("extradata: expFillLen=%u, act_fill_len=%u",
             pstHcpTestCtx->u32ExpExtraLen,
             pNode->pIntBuf->stExtra.u32FilledLen);
        validate_extradata_integrity(pNode);
    }

    if (pstBuf->eQueuedPort == VPP_PORT_INPUT)
    {
        pthread_mutex_lock(&pstTestCtx->mutex);
        pstHcpTestCtx->u32BypassCnt++;
        DVP_ASSERT_TRUE(pstHcpTestCtx->u32NumExpectedBypassBufs >=
                        pstHcpTestCtx->u32BypassCnt);
        LOGD("bypass bufs, given=%u, expected=%u", pstHcpTestCtx->u32BypassCnt,
             pstHcpTestCtx->u32NumExpectedBypassBufs);
        pstTestCtx->u32OutRxCnt++;
        pstTestCtx->u32InHoldCnt--;
        pthread_mutex_unlock(&pstTestCtx->mutex);
        LOGI("recvd %u-th BYPASS buffer.", pstHcpTestCtx->u32BypassCnt);
    }
    else if (pstBuf->eQueuedPort == VPP_PORT_OUTPUT)
    {
        // Bypassed (or flushed)
        if (pNode->pIntBuf->stPixel.u32FilledLen == 0)
        {
            LOGD("given filled_len == 0 on output buffer.");
        }
        pthread_mutex_lock(&pstTestCtx->mutex);
        pstTestCtx->u32OutRxCnt++;
        pstTestCtx->u32OutHoldCnt--;
        pthread_mutex_unlock(&pstTestCtx->mutex);
    }
    else
        DVP_ASSERT_FAIL();

    put_buf(pstTestCtx->buf_pool, pNode);
    LOGI("-> Done: %u OBDs processed", pstTestCtx->u32OutRxCnt);

    pthread_cond_signal(&pstTestCtx->cond);
}

static void test_hcp_evt(void *pv, t_StVppEvt stEvt)
{
    struct test_ctx *pstTestCtx;
    struct hcp_test_ctx *pstHcpCtx;

    LOGI("Running test_hcp_evt");

    if (!pv)
    {
        LOGE("got null ctx.");
        DVP_ASSERT_FAIL();
        return;
    }

    pstTestCtx = (struct test_ctx *)pv;
    pstHcpCtx = HCPIPTEST_HCP_TEST_CTX(pstTestCtx);

    pthread_mutex_lock(&pstTestCtx->mutex);

    switch (stEvt.eType)
    {
        case VPP_EVT_FLUSH_DONE:
            LOGI("received flush done, port=%u", stEvt.flush.ePort);
            if (stEvt.flush.ePort == VPP_PORT_INPUT)
            {
                VPP_FLAG_SET(pstTestCtx->u32Flags, INPUT_FLUSH_DONE);
                pstTestCtx->u32InFlushCnt++;
            }
            else if (stEvt.flush.ePort == VPP_PORT_OUTPUT)
            {
                VPP_FLAG_SET(pstTestCtx->u32Flags, OUTPUT_FLUSH_DONE);
                pstTestCtx->u32OutFlushCnt++;
            }
            else
            {
                LOGE("got unknown port for flush, port=%u", stEvt.flush.ePort);
                DVP_ASSERT_FAIL();
            }
            break;
        case VPP_EVT_DRAIN_DONE:
            LOGI("received drain done");
            VPP_FLAG_SET(pstTestCtx->u32Flags, DRAIN_DONE);
            pstHcpCtx->u32DrainCnt++;
            break;
        case VPP_EVT_RECONFIG_DONE:
            if (stEvt.reconfig.pstUc)
                DVP_ASSERT_EQUAL(stEvt.reconfig.u32Err, VPP_OK);
            break;
        // Intentional fall-through.
        case VPP_EVT_ERR_SYSTEM:
        case VPP_EVT_ERR_MAX_CLIENTS:
        case VPP_EVT_ERR_HW_OVERLOAD:
        case VPP_EVT_ERR_HW_UNSUPPORTED:
            LOGE("got error event, evt_type=%u, err=%d", stEvt.eType,
                 stEvt.reconfig.u32Err);
            break;
        default:
            LOGE("got unknown event, evt_type=%u", stEvt.eType);
            break;
    }

    pthread_cond_signal(&pstTestCtx->cond);
    pthread_mutex_unlock(&pstTestCtx->mutex);
}

static void set_callbacks(t_StVppCallback *pstCbs, void *pv)
{
    if (!pstCbs)
        return;

    pstCbs->pv = pv;
    pstCbs->input_buffer_done = test_hcp_ibd;
    pstCbs->output_buffer_done = test_hcp_obd;
    pstCbs->event = test_hcp_evt;
}

static uint32_t u32HcpIpTest_GetHqvCtrlFromOpMode(t_EVppIpHcpOpMode eOpMode,
                                                  struct hqv_control *pstCtrl)
{
    if (eOpMode >= HCP_OP_MODE_MAX || eOpMode == HCP_OP_MODE_BYPASS || !pstCtrl)
        return VPP_ERR_PARAM;

    if (eOpMode == HCP_OP_MODE_PROC)
    {
        pstCtrl->mode = HQV_MODE_MANUAL;
        pstCtrl->ctrl_type = HQV_CONTROL_AIE;
        pstCtrl->aie.mode = HQV_MODE_AUTO;
    }
    else
    {
        pstCtrl->mode = HQV_MODE_MANUAL;
        pstCtrl->ctrl_type = HQV_CONTROL_MEAS;
        pstCtrl->meas.enable = VPP_TRUE;
    }

    return VPP_OK;
}

static uint32_t u32HcpIpTest_GetOpModeFromHqvCtrl(struct hqv_control stCtrl,
                                                  t_EVppIpHcpOpMode *o_eOpMode)
{
    VPP_RET_IF_NULL(o_eOpMode, VPP_ERR_PARAM);

    if (stCtrl.mode == HQV_MODE_AUTO)
        *o_eOpMode = HCP_OP_MODE_PROC;

    else if (stCtrl.mode == HQV_MODE_OFF)
        *o_eOpMode = HCP_OP_MODE_BYPASS;

    else if (stCtrl.mode == HQV_MODE_MANUAL &&
             stCtrl.ctrl_type == HQV_CONTROL_AIE &&
             stCtrl.aie.mode == HQV_MODE_AUTO)
        *o_eOpMode = HCP_OP_MODE_PROC;

    else if (stCtrl.mode == HQV_MODE_MANUAL &&
             stCtrl.ctrl_type == HQV_CONTROL_AIE &&
             stCtrl.aie.mode == HQV_MODE_OFF)
        *o_eOpMode = HCP_OP_MODE_BYPASS;

    else if (stCtrl.mode == HQV_MODE_MANUAL &&
             stCtrl.ctrl_type == HQV_CONTROL_AIE &&
             stCtrl.aie.mode == HQV_MODE_MANUAL &&
             stCtrl.aie.ltm_sat_gain >= HCP_SAT_GAIN_MIN &&
             stCtrl.aie.ltm_sat_offset >= HCP_SAT_OFFSET_MIN &&
             stCtrl.aie.ltm_ace_bright_l >= HCP_ACE_BRI_L_MIN &&
             stCtrl.aie.ltm_ace_bright_h >= HCP_ACE_BRI_H_MIN &&
             stCtrl.aie.ltm_ace_str >= HCP_ACE_STR_MIN &&
             stCtrl.aie.cade_level >= HCP_DE_GAIN_MIN &&
             stCtrl.aie.ltm_sat_gain <= HCP_SAT_GAIN_MAX &&
             stCtrl.aie.ltm_sat_offset <= HCP_SAT_OFFSET_MAX &&
             stCtrl.aie.ltm_ace_bright_l <= HCP_ACE_BRI_L_MAX &&
             stCtrl.aie.ltm_ace_bright_h <= HCP_ACE_BRI_H_MAX &&
             stCtrl.aie.ltm_ace_str <= HCP_ACE_STR_MAX &&
             stCtrl.aie.cade_level <= HCP_DE_GAIN_MAX)
        *o_eOpMode = HCP_OP_MODE_PROC;

    else if (stCtrl.mode == HQV_MODE_MANUAL &&
             stCtrl.ctrl_type == HQV_CONTROL_EAR &&
             stCtrl.ear.mode == HQV_EAR_MODE_OFF)
        *o_eOpMode = HCP_OP_MODE_BYPASS;

    else if (stCtrl.mode == HQV_MODE_MANUAL &&
             stCtrl.ctrl_type == HQV_CONTROL_EAR &&
             stCtrl.ear.mode > HQV_EAR_MODE_OFF &&
             stCtrl.ear.mode < HQV_EAR_MODE_MAX)
        *o_eOpMode = HCP_OP_MODE_PROC;

    else if (stCtrl.mode == HQV_MODE_MANUAL &&
             stCtrl.ctrl_type == HQV_CONTROL_MEAS)
    {
        if (stCtrl.meas.enable == VPP_FALSE)
            *o_eOpMode = HCP_OP_MODE_BYPASS;

        else
            *o_eOpMode = HCP_OP_MODE_MEAS;
    }

    else
        return VPP_ERR_PARAM;

    return VPP_OK;
}

static uint32_t u32HcpIpTest_GetMaxStride(enum vpp_color_format eFmt)
{
    switch (eFmt)
    {
        case VPP_COLOR_FORMAT_NV12_VENUS:
        case VPP_COLOR_FORMAT_NV21_VENUS:
            return HCP_WIDTH_MAX;
        case VPP_COLOR_FORMAT_P010:
            return HCP_WIDTH_MAX * 2;
        case VPP_COLOR_FORMAT_UBWC_NV12:
        case VPP_COLOR_FORMAT_UBWC_NV21:
        case VPP_COLOR_FORMAT_UBWC_TP10:
            return u32VppUtils_CalcStrideForPlane(HCP_WIDTH_MAX, eFmt, 1);
        default:
            DVP_ASSERT_FAIL();
            return 0;
    }
}

static uint32_t u32HcpIpTest_SetPortParams(uint32_t u32Width,
                                           uint32_t u32Height,
                                           enum vpp_color_format eFmt,
                                           struct vpp_port_param *o_stParam)
{
    uint32_t u32PlaneNum;

    if (!o_stParam)
        return VPP_ERR_PARAM;

    o_stParam->width = u32Width;
    o_stParam->height = u32Height;
    o_stParam->fmt = eFmt;

    switch (eFmt)
    {
        // Intentional fall-through
        case VPP_COLOR_FORMAT_NV12_VENUS:
        case VPP_COLOR_FORMAT_NV21_VENUS:
        case VPP_COLOR_FORMAT_P010:
            u32PlaneNum = 0;
            break;
        // Intentional fall-through
        case VPP_COLOR_FORMAT_UBWC_NV12:
        case VPP_COLOR_FORMAT_UBWC_NV21:
        case VPP_COLOR_FORMAT_UBWC_TP10:
            u32PlaneNum = 1;
            break;
        default:
            DVP_ASSERT_FAIL();
            u32PlaneNum = 0;
            break;
    }
    o_stParam->stride =
        u32VppUtils_CalcStrideForPlane(u32Width, eFmt, u32PlaneNum);
    o_stParam->scanlines =
        u32VppUtils_CalcScanlinesForPlane(u32Height, eFmt, u32PlaneNum);

    return VPP_OK;
}

static uint32_t u32HcpIpTest_SetCtxPortParams(struct test_ctx *pCtx,
                                              uint32_t u32Width,
                                              uint32_t u32Height,
                                              enum vpp_color_format eFmt)
{
    uint32_t u32;
    struct hcp_test_ctx *pstHcpTestCtx;

    if (!pCtx)
        return VPP_ERR_PARAM;

    pthread_mutex_lock(&pCtx->mutex);

    pstHcpTestCtx = HCPIPTEST_HCP_TEST_CTX(pCtx);
    u32 = u32HcpIpTest_SetPortParams(u32Width, u32Height, eFmt,
                                     &pstHcpTestCtx->stCfg.stPortParam);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    pthread_mutex_unlock(&pCtx->mutex);

    return VPP_OK;
}

static uint32_t u32HcpIpTest_SetCtxHqvCtrl(struct test_ctx *pCtx,
                                           t_EVppIpHcpOpMode eOpMode)
{
    uint32_t u32;
    struct hcp_test_ctx *pstHcpTestCtx;

    if (!pCtx || eOpMode >= HCP_OP_MODE_MAX)
        return VPP_ERR_PARAM;

    pthread_mutex_lock(&pCtx->mutex);

    pstHcpTestCtx = HCPIPTEST_HCP_TEST_CTX(pCtx);
    u32 = u32HcpIpTest_GetHqvCtrlFromOpMode(eOpMode,
                                            &pstHcpTestCtx->stCfg.stHqvCtrl);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    pthread_mutex_unlock(&pCtx->mutex);

    return VPP_OK;
}

static uint32_t u32HcpIpTest_ValidateBufReqs(struct test_ctx *pCtx,
                                             t_StVppIpBufReq *pstReq,
                                             enum vpp_port ePort)
{
    t_StVppIpHcpSessionCb *pstCb;
    uint32_t u32PxBufSz;
    uint32_t u32MinCnt;
    struct hcp_test_ctx *pstHcpTestCtx;

    if (!pCtx || !pstReq || ePort >= VPP_PORT_MAX)
        return VPP_ERR_PARAM;

    pthread_mutex_lock(&pCtx->mutex);

    pstHcpTestCtx = HCPIPTEST_HCP_TEST_CTX(pCtx);
    pstCb = pstHcpTestCtx->pstHcpCb;
    if (!pstCb)
    {
        pthread_mutex_unlock(&pCtx->mutex);
        return VPP_ERR_PARAM;
    }

    if (!VPP_FLAG_IS_SET(pstHcpTestCtx->u32TestFlags, HCPIPTEST_CALLED_SETPARAM))
    {
        u32PxBufSz = 0;
        u32MinCnt = 1;
    }
    else
    {
        u32PxBufSz =
            u32VppUtils_GetPxBufferSize(&pstHcpTestCtx->stCfg.stPortParam);
        u32MinCnt = 1;
    }

    if (pstCb->stCfg.eOpMode == HCP_OP_MODE_PROC)
    {
        DVP_ASSERT_EQUAL(pstReq->u32PxSz, u32PxBufSz);
        DVP_ASSERT_EQUAL(pstReq->u32MinCnt, u32MinCnt);
    }
    else if (pstCb->stCfg.eOpMode == HCP_OP_MODE_MEAS)
    {
        if (ePort == VPP_PORT_INPUT)
        {
            DVP_ASSERT_EQUAL(pstReq->u32PxSz, u32PxBufSz);
            DVP_ASSERT_EQUAL(pstReq->u32MinCnt, u32MinCnt);
        }
        else
        {
            // Intentionally comparing received buffer size for output buffers
            // with HCP IP's intermediate buffer sizes. The intermediate buffer
            // sizes are the values given from HCP FW via PROP_ID_BUFREQS.
            // Therefore, the correct value for output buffer size in MEAS mode
            // is stored in the intermediate MEAS buffer size.
            DVP_ASSERT_EQUAL(pstReq->u32PxSz,
                             pstCb->stIntBufs.stIntBufMeas.u32Sz);
            DVP_ASSERT_EQUAL(pstReq->u32MinCnt, u32MinCnt);
        }
    }
    else if (pstCb->stCfg.eOpMode == HCP_OP_MODE_BYPASS)
    {
        if (ePort == VPP_PORT_INPUT)
        {
            DVP_ASSERT_EQUAL(pstReq->u32PxSz, u32PxBufSz);
            DVP_ASSERT_EQUAL(pstReq->u32MinCnt, u32MinCnt);
        }
        else
        {
            if (VPP_FLAG_IS_SET(pstHcpTestCtx->u32TestFlags, HCPIPTEST_CALLED_SETPARAM))
                u32MinCnt = 0;
            DVP_ASSERT_EQUAL(pstReq->u32MinCnt, u32MinCnt);
        }
    }
    else
    {
        LOGE("invalid opmode when validating buf reqs, opmode=%u",
             pstCb->stCfg.eOpMode);
        DVP_ASSERT_FAIL();
    }

    pthread_mutex_unlock(&pCtx->mutex);

    return VPP_OK;
}

static uint32_t u32HcpIpTest_ValidateBothBufReqs(struct test_ctx *pCtx,
                                                 t_StVppIpBufReq *pstInReq,
                                                 t_StVppIpBufReq *pstOutReq)
{
    uint32_t u32;

    if (!pCtx || !pstInReq || !pstOutReq)
        return VPP_ERR_PARAM;

    u32 = u32HcpIpTest_ValidateBufReqs(pCtx, pstInReq, VPP_PORT_INPUT);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    u32 = u32HcpIpTest_ValidateBufReqs(pCtx, pstOutReq, VPP_PORT_OUTPUT);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    return VPP_OK;
}

static uint32_t u32HcpIpTest_ComparePortParams(struct vpp_port_param stParam1,
                                               struct vpp_port_param stParam2)
{
    DVP_ASSERT_EQUAL(stParam1.width, stParam2.width);
    DVP_ASSERT_EQUAL(stParam1.height, stParam2.height);
    DVP_ASSERT_EQUAL(stParam1.stride, stParam2.stride);
    DVP_ASSERT_EQUAL(stParam1.scanlines, stParam2.scanlines);
    DVP_ASSERT_EQUAL(stParam1.fmt, stParam2.fmt);

    if ((stParam1.width != stParam2.width) ||
        (stParam1.height != stParam2.height) ||
        (stParam1.stride != stParam2.stride) ||
        (stParam1.scanlines != stParam2.scanlines) ||
        (stParam1.fmt != stParam2.fmt))
        return VPP_ERR;
    return VPP_OK;
}

static uint32_t u32HcpIpTest_Open(t_StVppIpHcpSessionCb *pstSessCb,
                                  t_EVppIpHcpOpMode eExpectedOpMode)
{
    uint32_t u32;

    if (!pstSessCb)
        return VPP_ERR_PARAM;

    u32 = u32VppIpHcp_Open((void *)pstSessCb);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    DVP_ASSERT_EQUAL(pstSessCb->stCfg.eOpMode, eExpectedOpMode);
    if (eExpectedOpMode == HCP_OP_MODE_PROC ||
        eExpectedOpMode == HCP_OP_MODE_MEAS)
    {
        DVP_ASSERT_EQUAL(pstSessCb->stDrvCtx.eOpModeActive, eExpectedOpMode);
    }
    DVP_ASSERT_EQUAL(pstSessCb->eState, VPP_IP_STATE_ACTIVE);
    return u32;
}

static uint32_t u32HcpIpTest_FillBuf(struct test_ctx *pstTestCtx,
                                     enum vpp_port ePort,
                                     t_EVppBufType eBufType,
                                     struct bufnode **o_ppNode)
{
    struct bufnode *pNode;
    struct hcp_test_ctx *pstHcpCtx;

    if (!pstTestCtx || ePort >= VPP_PORT_MAX || eBufType >= eVppBufType_Max ||
        !o_ppNode)
        return VPP_ERR_PARAM;

    pstHcpCtx = HCPIPTEST_HCP_TEST_CTX(pstTestCtx);
    pthread_mutex_lock(&pstTestCtx->mutex);
    while (NULL == (pNode = get_buf(pstTestCtx->buf_pool)))
        pthread_cond_wait(&pstTestCtx->cond, &pstTestCtx->mutex);
    pthread_mutex_unlock(&pstTestCtx->mutex);

    fill_buf(pNode);
    if (ePort == VPP_PORT_INPUT)
    {
        fill_extra_data(pNode, eBufType, 0);
        pstHcpCtx->u32ExpExtraLen = pNode->pExtBuf->extradata.filled_len;
    }
    pNode->owner = BUF_OWNER_LIBRARY;
    pNode->pIntBuf->eQueuedPort = ePort;
    pNode->pIntBuf->eBufType = eBufType;

    *o_ppNode = pNode;
    return VPP_OK;
}

static uint32_t u32HcpIpTest_QueueGivenBuf(struct test_ctx *pstTestCtx,
                                           struct bufnode *pNode,
                                           enum vpp_port ePort)
{
    uint32_t u32;
    void *pvSessCb;

    if (!pstTestCtx || !pNode || ePort >= VPP_PORT_MAX)
        return VPP_ERR_PARAM;

    pthread_mutex_lock(&pstTestCtx->mutex);
    if (ePort == VPP_PORT_INPUT)
    {
        pstTestCtx->u32InPutCnt++;
        pstTestCtx->u32InHoldCnt++;
    }
    else
    {
        pstTestCtx->u32OutPutCnt++;
        pstTestCtx->u32OutHoldCnt++;
    }
    pthread_mutex_unlock(&pstTestCtx->mutex);

    pvSessCb = HCPIPTEST_HCP_TEST_CTX(pstTestCtx)->pstHcpCb;
    u32 = u32VppIpHcp_QueueBuf(pvSessCb, ePort, pNode->pIntBuf);
    return u32;
}

static uint32_t u32HcpIpTest_QueueBuf(struct test_ctx *pstTestCtx,
                                      enum vpp_port ePort,
                                      t_EVppBufType eBufType)
{
    uint32_t u32;
    struct bufnode *pNode;

    if (!pstTestCtx || eBufType >= eVppBufType_Max || ePort >= VPP_PORT_MAX)
        return VPP_ERR_PARAM;

    LOGD("fill buffer. port=%u, buf_type=%u", ePort, eBufType);
    u32 = u32HcpIpTest_FillBuf(pstTestCtx, ePort, eBufType, &pNode);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    LOGD("queue buffer to HCP. port=%u, buf_type=%u", ePort, eBufType);
    u32 = u32HcpIpTest_QueueGivenBuf(pstTestCtx, pNode, ePort);
    return u32;
}

static uint32_t u32HcpIpTest_QueueBufBothPorts(struct test_ctx *pstTestCtx,
                                               t_EVppBufType eBufType,
                                               uint32_t u32BufCnt)
{
    uint32_t u32;
    uint32_t u32Cnt;
    uint32_t u32Ret = 0;

    for (u32Cnt = 0; u32Cnt < u32BufCnt; u32Cnt++)
    {
        u32 = u32HcpIpTest_QueueBuf(pstTestCtx, VPP_PORT_INPUT, eBufType);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
        u32Ret |= u32;

        u32 = u32HcpIpTest_QueueBuf(pstTestCtx, VPP_PORT_OUTPUT, eBufType);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
        u32Ret |= u32;
    }

    return u32Ret;
}

static uint32_t u32HcpIpTest_QueueBufCustomResolution(struct test_ctx *pstTestCtx,
                                                      enum vpp_port ePort,
                                                      t_EVppBufType eBufType,
                                                      uint32_t u32Width,
                                                      uint32_t u32Height)
{
    uint32_t u32;
    struct bufnode *pNode;
    struct vpp_port_param stTempParams = {};

    if (!pstTestCtx || eBufType >= eVppBufType_Max || ePort >= VPP_PORT_MAX)
        return VPP_ERR_PARAM;

    u32 = u32HcpIpTest_FillBuf(pstTestCtx, ePort, eBufType, &pNode);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // TODO: Changing resolution is just changing filled_len?
    stTempParams = pstTestCtx->port_param;
    stTempParams.width = u32Width;
    stTempParams.height = u32Height;
    stTempParams.stride = u32VppUtils_CalculateStride(stTempParams.width,
                                                      stTempParams.fmt);
    stTempParams.scanlines = u32VppUtils_CalculateScanlines(stTempParams.height,
                                                            stTempParams.fmt);
    pNode->pIntBuf->pBuf->pixel.filled_len = u32VppUtils_GetPxBufferSize(&stTempParams);
    pNode->pIntBuf->stPixel.u32FilledLen = pNode->pIntBuf->pBuf->pixel.filled_len;

    LOGD("queue buffer to HCP. port=%u, buf_type=%u, width=%u, height=%u "
         "filled_len=%u", ePort, eBufType, u32Width, u32Height,
         pNode->pIntBuf->stPixel.u32FilledLen);
    u32 = u32HcpIpTest_QueueGivenBuf(pstTestCtx, pNode, ePort);
    return u32;
}

static uint32_t u32HcpIpTest_QueueBufCustomFlag(struct test_ctx *pstTestCtx,
                                                enum vpp_port ePort,
                                                t_EVppBufType eBufType,
                                                uint32_t u32Flag)
{
    uint32_t u32;
    struct bufnode *pNode;

    if (!pstTestCtx || eBufType >= eVppBufType_Max || ePort >= VPP_PORT_MAX)
        return VPP_ERR_PARAM;

    LOGD("fill buffer. port=%u, buf_type=%u", ePort, eBufType);
    u32 = u32HcpIpTest_FillBuf(pstTestCtx, ePort, eBufType, &pNode);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // Set internal flags of buffer.
    VPP_FLAG_SET(pNode->pIntBuf->u32InternalFlags, u32Flag);

    LOGD("queue buffer to HCP. port=%u, buf_type=%u, flag=0x%x", ePort,
         eBufType, pNode->pIntBuf->u32InternalFlags);
    u32 = u32HcpIpTest_QueueGivenBuf(pstTestCtx, pNode, ePort);
    return u32;
}

static void vHcpIpTest_WaitForAllInputBuffers(struct test_ctx *pCtx,
                                              struct hcp_test_ctx *pHcpCtx)
{
    if (!pCtx || !pHcpCtx)
    {
        DVP_ASSERT_FAIL();
        return;
    }

    pthread_mutex_lock(&pCtx->mutex);
    while (pCtx->u32InHoldCnt != 0)
        pthread_cond_wait(&pCtx->cond, &pCtx->mutex);
    DVP_ASSERT_EQUAL(pHcpCtx->u32NumExpectedBypassBufs, pHcpCtx->u32BypassCnt);
    pHcpCtx->u32BypassCnt = 0;
    pthread_mutex_unlock(&pCtx->mutex);
}

static void vHcpIpTest_WaitForAllOutputBuffers(struct test_ctx *pCtx,
                                               struct hcp_test_ctx *pHcpCtx)
{
    if (!pCtx || !pHcpCtx)
    {
        DVP_ASSERT_FAIL();
        return;
    }

    pthread_mutex_lock(&pCtx->mutex);
    while (pCtx->u32OutHoldCnt != 0)
        pthread_cond_wait(&pCtx->cond, &pCtx->mutex);
    pthread_mutex_unlock(&pCtx->mutex);
}

static void vHcpIpTest_WaitForAllBuffers(struct test_ctx *pCtx,
                                         struct hcp_test_ctx *pHcpCtx)
{
    if (!pCtx || !pHcpCtx)
    {
        DVP_ASSERT_FAIL();
        return;
    }

    vHcpIpTest_WaitForAllInputBuffers(pCtx, pHcpCtx);
    vHcpIpTest_WaitForAllOutputBuffers(pCtx, pHcpCtx);
}

static void vHcpIpTest_WaitForFlushDone(struct test_ctx *pCtx,
                                        enum vpp_port ePort)
{
    uint32_t u32FlagTarget = 0;

    if (!pCtx || ePort >= VPP_PORT_MAX)
    {
        DVP_ASSERT_FAIL();
        return;
    }

    if (ePort == VPP_PORT_INPUT)
        u32FlagTarget = INPUT_FLUSH_DONE;
    else if (ePort == VPP_PORT_OUTPUT)
        u32FlagTarget = OUTPUT_FLUSH_DONE;

    pthread_mutex_lock(&pCtx->mutex);

    while (!VPP_FLAG_IS_SET(pCtx->u32Flags, u32FlagTarget))
        pthread_cond_wait(&pCtx->cond, &pCtx->mutex);
    VPP_FLAG_CLR(pCtx->u32Flags, u32FlagTarget);

    pthread_mutex_unlock(&pCtx->mutex);
}

static void vHcpIpTest_WaitForDrainDone(struct test_ctx *pCtx)
{
    if (!pCtx)
    {
        DVP_ASSERT_FAIL();
        return;
    }

    pthread_mutex_lock(&pCtx->mutex);

    while (!VPP_FLAG_IS_SET(pCtx->u32Flags, DRAIN_DONE))
        pthread_cond_wait(&pCtx->cond, &pCtx->mutex);
    VPP_FLAG_CLR(pCtx->u32Flags, DRAIN_DONE);

    pthread_mutex_unlock(&pCtx->mutex);
}

static void vHcpIpTest_ClearTestCtxBufCnt(struct test_ctx *pCtx,
                                          pthread_mutex_t *pCtxMutex)
{
    struct hcp_test_ctx *pHcpCtx;

    if (!pCtx)
    {
        DVP_ASSERT_FAIL();
        return;
    }

    if (pCtxMutex)
        pthread_mutex_lock(pCtxMutex);

    pCtx->u32InRxCnt = 0;
    pCtx->u32OutRxCnt = 0;

    pCtx->u32InPutCnt = 0;
    pCtx->u32OutPutCnt = 0;

    pCtx->u32InHoldCnt = 0;
    pCtx->u32OutHoldCnt = 0;

    pCtx->u32InFlushCnt = 0;
    pCtx->u32OutFlushCnt = 0;

    pCtx->u32InEosRxCnt = 0;
    pCtx->u32OutEosRxCnt = 0;

    pHcpCtx = HCPIPTEST_HCP_TEST_CTX(pCtx);
    if (pHcpCtx)
    {
        pHcpCtx->u32NumExpectedBypassBufs = 0;
        pHcpCtx->u32BypassCnt = 0;
        pHcpCtx->u32DrainCnt = 0;
    }

    if (pCtxMutex)
        pthread_mutex_unlock(pCtxMutex);
}

static void vHcpIpTest_InitTestCtx(struct test_ctx *pCtx)
{
    uint32_t u32;

    u32 = tctx_common_init(pCtx);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    pCtx->pPrivateCtx = calloc(1, sizeof(struct hcp_test_ctx));
    DVP_ASSERT_PTR_NNULL(pCtx->pPrivateCtx);

    // Default values.
    HCPIPTEST_HCP_TEST_CTX(pCtx)->stCfg.bCpStatus = VPP_FALSE;
    HCPIPTEST_HCP_TEST_CTX(pCtx)->stCfg.stHqvCtrl.mode = HQV_MODE_AUTO;
    u32 = u32HcpIpTest_SetCtxPortParams(pCtx, 1920, 1080,
                                        VPP_COLOR_FORMAT_NV12_VENUS);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
}

static void vHcpIpTest_TermTestCtx(struct test_ctx *pCtx)
{
    struct hcp_test_ctx *pstHcpTestCtx;

    if (pCtx)
    {
        pstHcpTestCtx = HCPIPTEST_HCP_TEST_CTX(pCtx);
        if (pstHcpTestCtx)
            free(pstHcpTestCtx);
        tctx_common_destroy(pCtx);
    }
}

static void vHcpIpTest_InitCtxBufPool(struct test_ctx *pCtx,
                                      uint32_t u32BufPoolSize)
{
    uint32_t u32;

    if (!pCtx)
    {
        DVP_ASSERT_FAIL();
        return;
    }

    u32 = buf_params_init_default(&pCtx->params, &pCtx->port_param);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = init_buf_pool(&pCtx->stVppCtx, &pCtx->buf_pool,
                        &pCtx->params, u32BufPoolSize, VPP_TRUE);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
}

static void vHcpIpTest_TermCtxBufPool(struct test_ctx *pCtx)
{
    if (!pCtx)
    {
        DVP_ASSERT_FAIL();
        return;
    }

    free_buf_pool(pCtx->buf_pool, VPP_TRUE);
}

static void vHcpIpTest_TestSetupInit(struct test_ctx *pCtx)
{
    struct hcp_test_ctx *pstHcpTestCtx;
    t_StVppIpHcpSessionCb *pstSessCb;

    if (!pCtx)
    {
        DVP_ASSERT_FAIL();
        return;
    }

    pstHcpTestCtx = HCPIPTEST_HCP_TEST_CTX(pCtx);

    if (!VPP_FLAG_IS_SET(pstHcpTestCtx->u32TestFlags, HCPIPTEST_CALLED_INIT))
    {
        set_callbacks(&pCtx->cb, pCtx);

        if (pstHcpTestCtx->stCfg.bCpStatus)
        {
            pCtx->eProtection = PROTECTION_ZONE_SECURE;
            pCtx->params.eProtection = pCtx->eProtection;
        }

        void *pv = vpVppIpHcp_Init(&pCtx->stVppCtx, get_session_flags(pCtx),
                                   pCtx->cb);
        if (!pv)
        {
            DVP_ASSERT_FAIL();
            LOGE("vpVppIpHcp_Init returned NULL");
            return;
        }

        pstHcpTestCtx->pstHcpCb = (t_StVppIpHcpSessionCb *)pv;
        pstSessCb = pstHcpTestCtx->pstHcpCb;
        DVP_ASSERT_EQUAL(pstSessCb->eState, VPP_IP_STATE_INITED);

        VPP_FLAG_SET(pstHcpTestCtx->u32TestFlags, HCPIPTEST_CALLED_INIT);
    }
}

static void vHcpIpTest_SetParamBoth(struct test_ctx *pCtx)
{
    uint32_t u32;
    struct hcp_test_ctx *pstHcpTestCtx;
    t_StVppIpHcpSessionCb *pstSessCb;

    if (!pCtx)
    {
        DVP_ASSERT_FAIL();
        return;
    }

    pstHcpTestCtx = HCPIPTEST_HCP_TEST_CTX(pCtx);
    pstSessCb = pstHcpTestCtx->pstHcpCb;

    u32 = u32VppIpHcp_SetParam(pstSessCb, VPP_PORT_INPUT, pCtx->port_param);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = u32VppIpHcp_SetParam(pstSessCb, VPP_PORT_OUTPUT, pCtx->port_param);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
}

static void vHcpIpTest_TestSetupSetParam(struct test_ctx *pCtx,
                                         uint32_t u32BufPoolSize)
{
    struct hcp_test_ctx *pstHcpTestCtx;

    if (!pCtx)
    {
        DVP_ASSERT_FAIL();
        return;
    }

    pstHcpTestCtx = HCPIPTEST_HCP_TEST_CTX(pCtx);

    vHcpIpTest_TestSetupInit(pCtx);

    if (!VPP_FLAG_IS_SET(pstHcpTestCtx->u32TestFlags, HCPIPTEST_CALLED_SETPARAM))
    {
        pCtx->port_param = pstHcpTestCtx->stCfg.stPortParam;

        vHcpIpTest_InitCtxBufPool(pCtx, u32BufPoolSize);

        vHcpIpTest_SetParamBoth(pCtx);

        VPP_FLAG_SET(pstHcpTestCtx->u32TestFlags, HCPIPTEST_CALLED_SETPARAM);
    }
}

static void vHcpIpTest_TestSetup(struct test_ctx *pCtx,
                                 uint32_t u32BufPoolSize)
{
    t_EVppIpHcpOpMode eExpectedOpMode;
    uint32_t u32;
    struct hcp_test_ctx *pstHcpTestCtx;
    t_StVppIpHcpSessionCb *pstSessCb;

    if (!pCtx)
    {
        DVP_ASSERT_FAIL();
        return;
    }

    pstHcpTestCtx = HCPIPTEST_HCP_TEST_CTX(pCtx);

    vHcpIpTest_TestSetupSetParam(pCtx, u32BufPoolSize);

    if (!VPP_FLAG_IS_SET(pstHcpTestCtx->u32TestFlags, HCPIPTEST_CALLED_OPEN))
    {
        pstSessCb = pstHcpTestCtx->pstHcpCb;

        u32 = u32HcpIpTest_GetOpModeFromHqvCtrl(pstHcpTestCtx->stCfg.stHqvCtrl,
                                                &eExpectedOpMode);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        u32 = u32VppIpHcp_SetCtrl(pstSessCb, pstHcpTestCtx->stCfg.stHqvCtrl);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        u32 = u32HcpIpTest_Open(pstSessCb, eExpectedOpMode);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        VPP_FLAG_SET(pstHcpTestCtx->u32TestFlags, HCPIPTEST_CALLED_OPEN);
    }
}

static void vHcpIpTest_TestCleanupClose(struct test_ctx *pCtx)
{
    uint32_t u32;
    struct hcp_test_ctx *pHcpCtx;
    void *pv;

    if (!pCtx)
    {
        DVP_ASSERT_FAIL();
        return;
    }

    pHcpCtx = HCPIPTEST_HCP_TEST_CTX(pCtx);
    pv = pHcpCtx->pstHcpCb;

    if (VPP_FLAG_IS_SET(pHcpCtx->u32TestFlags, HCPIPTEST_CALLED_OPEN))
    {
        u32 = u32VppIpHcp_Close(pv);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
    }

    VPP_FLAG_CLR(pHcpCtx->u32TestFlags, HCPIPTEST_CALLED_OPEN);
}

static void vHcpIpTest_TestCleanup(struct test_ctx *pCtx)
{
    struct hcp_test_ctx *pHcpCtx;

    if (!pCtx)
    {
        DVP_ASSERT_FAIL();
        return;
    }

    pHcpCtx = HCPIPTEST_HCP_TEST_CTX(pCtx);

    vHcpIpTest_TestCleanupClose(pCtx);

    if (VPP_FLAG_IS_SET(pHcpCtx->u32TestFlags, HCPIPTEST_CALLED_INIT))
    {
        vVppIpHcp_Term(pHcpCtx->pstHcpCb);
        pHcpCtx->pstHcpCb = NULL;

        DVP_ASSERT_EQUAL(pCtx->u32InHoldCnt, 0);
        DVP_ASSERT_EQUAL(pCtx->u32OutHoldCnt, 0);

        if (VPP_FLAG_IS_SET(pHcpCtx->u32TestFlags, HCPIPTEST_CALLED_SETPARAM))
            vHcpIpTest_TermCtxBufPool(pCtx);
    }

    VPP_FLAG_CLR(pHcpCtx->u32TestFlags, HCPIPTEST_CALLED_SETPARAM);
    VPP_FLAG_CLR(pHcpCtx->u32TestFlags, HCPIPTEST_CALLED_INIT);
}

#define SET_CTRL_ON(ctrl, type)     do {\
                                        ctrl->ctrl_type = type;\
                                        if (type == HQV_CONTROL_AIE)\
                                            ctrl->aie.mode = HQV_MODE_AUTO;\
                                        else\
                                            ctrl->ear.mode = HQV_EAR_MODE_MEDIUM;\
                                    } while(0)
#define SET_CTRL_OFF(ctrl, type)    do {\
                                        ctrl->ctrl_type = type;\
                                        if (type == HQV_CONTROL_AIE)\
                                            ctrl->aie.mode = HQV_MODE_OFF;\
                                        else\
                                            ctrl->ear.mode = HQV_EAR_MODE_OFF;\
                                    } while(0)
static uint32_t u32HcpIpTest_SelectOpModeAndCtrl(uint32_t u32RotationType,
                                                 uint32_t u32RotationCount,
                                                 enum hqv_control_type eCtrlType,
                                                 t_EVppIpHcpOpMode *o_peOpMode,
                                                 struct hqv_control *o_pstCtrl)
{
    if (!o_peOpMode || !o_pstCtrl ||
        (eCtrlType != HQV_CONTROL_AIE && eCtrlType != HQV_CONTROL_EAR))
    {
        DVP_ASSERT_FAIL();
        LOGE("u32RotationType=%u, u32RotationCount=%u, eCtrlType=%u, "
             "o_peOpMode=%p, o_pstCtrl=%p", u32RotationType, u32RotationCount,
             eCtrlType, o_peOpMode, o_pstCtrl);
        return VPP_ERR_PARAM;
    }

    // PROC -> MEAS -> BYPASS -> PROC -> ...
    if (u32RotationType == 0)
    {
        switch (u32RotationCount % 3)
        {
            case 0: // PROC
                *o_peOpMode = HCP_OP_MODE_PROC;

                o_pstCtrl->mode = HQV_MODE_MANUAL;
                SET_CTRL_ON(o_pstCtrl, eCtrlType);
                break;
            case 1: // MEAS
                *o_peOpMode = HCP_OP_MODE_MEAS;

                o_pstCtrl->mode = HQV_MODE_MANUAL;
                o_pstCtrl->ctrl_type = HQV_CONTROL_MEAS;
                o_pstCtrl->meas.enable = 1;
                break;
            case 2: // NOTHING => BYPASS
                *o_peOpMode = HCP_OP_MODE_BYPASS;

                if ((u32RotationCount - 2) % 6 == 0)
                {
                    o_pstCtrl->mode = HQV_MODE_MANUAL;
                    o_pstCtrl->ctrl_type = HQV_CONTROL_MEAS;
                    o_pstCtrl->meas.enable = 0;
                }
                else
                {
                    o_pstCtrl->mode = HQV_MODE_OFF;
                }
                break;
            default:
                // Shouldn't be here.
                DVP_ASSERT_FAIL();
                break;
        }
        LOGD("SwitchOpModes: PMB: mode=%u, cnt=%u", *o_peOpMode,
             u32RotationCount);
    }

    // BYPASS -> MEAS -> PROC -> BYPASS -> ...
    else if (u32RotationType == 1)
    {
        switch (u32RotationCount % 3)
        {
            case 0: // NOTHING => BYPASS
                *o_peOpMode = HCP_OP_MODE_BYPASS;

                if (u32RotationCount % 6 == 0)
                {
                    o_pstCtrl->mode = HQV_MODE_MANUAL;
                    SET_CTRL_OFF(o_pstCtrl, eCtrlType);
                }
                else
                {
                    o_pstCtrl->mode = HQV_MODE_OFF;
                }
                break;
            case 1: // MEAS
                *o_peOpMode = HCP_OP_MODE_MEAS;

                o_pstCtrl->mode = HQV_MODE_MANUAL;
                o_pstCtrl->ctrl_type = HQV_CONTROL_MEAS;
                o_pstCtrl->meas.enable = 1;
                break;
            case 2: // PROC
                *o_peOpMode = HCP_OP_MODE_PROC;

                o_pstCtrl->mode = HQV_MODE_MANUAL;
                SET_CTRL_ON(o_pstCtrl, eCtrlType);
                break;
            default:
                // Shouldn't be here.
                DVP_ASSERT_FAIL();
                break;
        }
        LOGD("SwitchOpModes: BMP: mode=%u, cnt=%u", *o_peOpMode,
             u32RotationCount);
    }

    // PROC -> MEAS -> PROC -> ...
    else if (u32RotationType == 2)
    {
        if (u32RotationCount % 2 == 0)
        {
            // PROC
            *o_peOpMode = HCP_OP_MODE_PROC;

            o_pstCtrl->mode = HQV_MODE_MANUAL;
            SET_CTRL_ON(o_pstCtrl, eCtrlType);
        }
        else
        {
            // MEAS
            *o_peOpMode = HCP_OP_MODE_MEAS;

            o_pstCtrl->mode = HQV_MODE_MANUAL;
            o_pstCtrl->ctrl_type = HQV_CONTROL_MEAS;
            o_pstCtrl->meas.enable = 1;
        }
        LOGD("SwitchOpModes: PM: mode=%u, cnt=%u", *o_peOpMode,
             u32RotationCount);
    }

    // PROC -> BYPASS -> PROC -> ...
    else if (u32RotationType == 3)
    {
        switch (u32RotationCount % 2)
        {
            case 0:
                *o_peOpMode = HCP_OP_MODE_PROC;

                o_pstCtrl->mode = HQV_MODE_MANUAL;
                SET_CTRL_ON(o_pstCtrl, eCtrlType);
                break;
            case 1:
                *o_peOpMode = HCP_OP_MODE_BYPASS;

                o_pstCtrl->mode = HQV_MODE_MANUAL;
                SET_CTRL_OFF(o_pstCtrl, eCtrlType);
                break;
        }
        LOGD("SwitchOpModes: PB: mode=%u, cnt=%u", *o_peOpMode,
             u32RotationCount);
    }

    // MEAS -> BYPASS -> MEAS -> ...
    else if (u32RotationType == 4)
    {
        if (u32RotationCount % 2 == 0)
        {
            // MEAS
            *o_peOpMode = HCP_OP_MODE_MEAS;

            o_pstCtrl->mode = HQV_MODE_MANUAL;
            o_pstCtrl->ctrl_type = HQV_CONTROL_MEAS;
            o_pstCtrl->meas.enable = VPP_TRUE;
        }
        else
        {
            // BYPASS
            *o_peOpMode = HCP_OP_MODE_BYPASS;

            if ((u32RotationCount - 1) % 4 == 0)
            {
                o_pstCtrl->mode = HQV_MODE_MANUAL;
                o_pstCtrl->ctrl_type = HQV_CONTROL_MEAS;
                o_pstCtrl->meas.enable = VPP_FALSE;
            }
            else
            {
                o_pstCtrl->mode = HQV_MODE_OFF;
            }
        }
        LOGD("SwitchOpModes: MB: mode=%u, cnt=%u", *o_peOpMode,
             u32RotationCount);
    }

    // AUTO -> MANUAL -> OFF -> AUTO -> ...
    else if (u32RotationType == 5)
    {
        switch (u32RotationCount % 3)
        {
            case 0:
                // AUTO
                *o_peOpMode = HCP_OP_MODE_PROC;

                o_pstCtrl->mode = HQV_MODE_AUTO;
                break;
            case 1:
                // MANUAL (MEAS)
                *o_peOpMode = HCP_OP_MODE_MEAS;

                o_pstCtrl->mode = HQV_MODE_MANUAL;
                o_pstCtrl->ctrl_type = HQV_CONTROL_MEAS;
                o_pstCtrl->meas.enable = 1;
                break;
            case 2:
                // OFF
                *o_peOpMode = HCP_OP_MODE_BYPASS;

                o_pstCtrl->mode = HQV_MODE_OFF;
                break;
            default:
                // Something went wrong, shouldn't be here.
                DVP_ASSERT_FAIL();
                break;
        }
        LOGD("SwitchOpModes: AMO: mode=%u, cnt=%u", *o_peOpMode,
             u32RotationCount);
    }

    // OFF -> MANUAL -> AUTO -> OFF -> ...
    else if (u32RotationType == 6)
    {
        switch (u32RotationCount % 3)
        {
            case 0:
                // OFF
                *o_peOpMode = HCP_OP_MODE_BYPASS;

                o_pstCtrl->mode = HQV_MODE_OFF;
                break;
            case 1:
                // MANUAL (MEAS)
                *o_peOpMode = HCP_OP_MODE_MEAS;

                o_pstCtrl->mode = HQV_MODE_MANUAL;
                o_pstCtrl->ctrl_type = HQV_CONTROL_MEAS;
                o_pstCtrl->meas.enable = 1;
                break;
            case 2:
                // AUTO
                *o_peOpMode = HCP_OP_MODE_PROC;

                o_pstCtrl->mode = HQV_MODE_AUTO;
                break;
            default:
                // Something went wrong, shouldn't be here.
                DVP_ASSERT_FAIL();
                break;
        }
        LOGD("SwitchOpModes: OMA: mode=%u, cnt=%u", *o_peOpMode,
             u32RotationCount);
    }

    // BYPASS -> BYPASS -> ...
    else if (u32RotationType == 7)
    {
        switch (u32RotationCount % 4)
        {
            case 0:
                // OFF
                o_pstCtrl->mode = HQV_MODE_OFF;
                break;
            case 1:
                // OFF MEAS
                o_pstCtrl->mode = HQV_MODE_MANUAL;
                o_pstCtrl->ctrl_type = HQV_CONTROL_MEAS;
                o_pstCtrl->meas.enable = VPP_FALSE;
                break;
            case 2:
                // OFF PROC(AIE)
                o_pstCtrl->mode = HQV_MODE_MANUAL;
                o_pstCtrl->ctrl_type = HQV_CONTROL_AIE;
                o_pstCtrl->aie.mode = HQV_MODE_OFF;
                break;
            case 3:
                // OFF PROC(EAR)
                o_pstCtrl->mode = HQV_MODE_MANUAL;
                o_pstCtrl->ctrl_type = HQV_CONTROL_EAR;
                o_pstCtrl->ear.mode = HQV_EAR_MODE_OFF;
                break;
        }

        *o_peOpMode = HCP_OP_MODE_BYPASS;
        LOGD("SwitchOpModes: BBB: mode=%u, cnt=%u", *o_peOpMode,
             u32RotationCount);
    }

    // AIE > BYPASS > AIE+EAR > BYPASS > EAR > BYPASS > AIE > ...
    else if (u32RotationType == 8)
    {
        switch (u32RotationCount % 6)
        {
            case 0: // AIE
                *o_peOpMode = HCP_OP_MODE_PROC;

                o_pstCtrl->mode = HQV_MODE_MANUAL;
                o_pstCtrl->ctrl_type = HQV_CONTROL_AIE;
                o_pstCtrl->aie.mode = HQV_MODE_AUTO;
                break;
            case 1: // BYPASS
                *o_peOpMode = HCP_OP_MODE_BYPASS;

                o_pstCtrl->mode = HQV_MODE_OFF;
                break;
            case 2: // AIE+EAR
                *o_peOpMode = HCP_OP_MODE_PROC;

                o_pstCtrl->mode = HQV_MODE_MANUAL;
                o_pstCtrl->ctrl_type = HQV_CONTROL_EAR;
                o_pstCtrl->ear.mode = HQV_EAR_MODE_MEDIUM;
                break;
            case 3: // BYPASS
                *o_peOpMode = HCP_OP_MODE_BYPASS;

                o_pstCtrl->mode = HQV_MODE_OFF;
                break;
            case 4: // EAR
                *o_peOpMode = HCP_OP_MODE_PROC;

                o_pstCtrl->mode = HQV_MODE_MANUAL;
                o_pstCtrl->ctrl_type = HQV_CONTROL_AIE;
                o_pstCtrl->aie.mode = HQV_MODE_OFF;
                break;
            case 5: // BYPASS
                *o_peOpMode = HCP_OP_MODE_BYPASS;

                o_pstCtrl->mode = HQV_MODE_MANUAL;
                o_pstCtrl->ctrl_type = HQV_CONTROL_EAR;
                o_pstCtrl->ear.mode = HQV_EAR_MODE_OFF;
                break;
        }

        LOGD("SwitchOpModes: ABTBEB: mode=%u, cnt=%u",
             *o_peOpMode, u32RotationCount);
    }
    else
    {
        LOGE("invalid rotation type.");
        DVP_ASSERT_FAIL();
        return VPP_ERR_PARAM;
    }

    return VPP_OK;
}

static uint32_t bAieWouldBypass(struct hcp_knobs stKnobs) {
    return (stKnobs.u32DeGain == HCP_DE_GAIN_OFF &&
            stKnobs.u32SatGain == HCP_SAT_GAIN_OFF &&
            stKnobs.u32SatOff == HCP_SAT_OFFSET_OFF &&
            stKnobs.u32AceStrCon == HCP_ACE_STR_OFF &&
            stKnobs.u32AceBriL == HCP_ACE_BRI_L_OFF &&
            stKnobs.u32AceBriH == HCP_ACE_BRI_H_OFF);
}

static uint32_t bEarWouldBypass(enum hqv_ear_mode mode) {
    if (mode == HQV_EAR_MODE_OFF ||
        mode == HQV_EAR_MODE_BYPASS ||
        mode >= HQV_EAR_MODE_MAX)
        return VPP_TRUE;
    return VPP_FALSE;
}

static uint32_t getDriverEarMode(enum hqv_ear_mode mode) {
    return bEarWouldBypass(mode) ? 0 : mode - 1;
}

static void vHcpIpTest_TestKnobsAndOpModes_CheckKnobs(struct test_ctx *pstTestCtx,
                                                      t_EVppIpHcpOpMode eOpMode,
                                                      struct hqv_control stCtrl,
                                                      uint32_t bAieOn,
                                                      uint32_t bEarOn)
{
    uint32_t u32;
    struct hcp_test_ctx *pstCtxHcp;
    t_StVppIpHcpSessionCb *pstSessCb;

#ifndef HCP_TEST_MEAS_MODE
    // TODO: Remove this when FW has MEAS mode enabled.
    if (eOpMode == HCP_OP_MODE_MEAS)
        return;
#endif
    if (!pstTestCtx)
    {
        DVP_ASSERT_FAIL();
        return;
    }
    pstCtxHcp = HCPIPTEST_HCP_TEST_CTX(pstTestCtx);
    if (!pstCtxHcp)
    {
        DVP_ASSERT_FAIL();
        return;
    }
    pstSessCb = pstCtxHcp->pstHcpCb;
    if (!pstSessCb)
    {
        DVP_ASSERT_FAIL();
        return;
    }

    pthread_mutex_lock(&pstTestCtx->mutex);
    if (eOpMode == HCP_OP_MODE_BYPASS)
        pstCtxHcp->u32NumExpectedBypassBufs = 1;
    pthread_mutex_unlock(&pstTestCtx->mutex);

    u32 = u32HcpIpTest_QueueBuf(pstTestCtx, VPP_PORT_INPUT,
                                eVppBufType_Progressive);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    if (eOpMode != HCP_OP_MODE_BYPASS)
    {
        u32 = u32HcpIpTest_QueueBuf(pstTestCtx, VPP_PORT_OUTPUT,
                                    eVppBufType_Progressive);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
    }

    vHcpIpTest_WaitForAllBuffers(pstTestCtx, pstCtxHcp);
    pthread_mutex_lock(&pstTestCtx->mutex);
    pstCtxHcp->u32NumExpectedBypassBufs = 0;
    pthread_mutex_unlock(&pstTestCtx->mutex);

    switch (eOpMode)
    {
        case HCP_OP_MODE_PROC:
            if (stCtrl.mode == HQV_MODE_AUTO)
            {
                DVP_ASSERT_EQUAL(pstSessCb->stDrvCtx.u32EarMode,
                                 HCP_EAR_MODE_DFLT);
                DVP_ASSERT_EQUAL(pstSessCb->stDrvCtx.u32SatGain,
                                 HCP_SAT_GAIN_DFLT);
                DVP_ASSERT_EQUAL(pstSessCb->stDrvCtx.u32AceStrCon,
                                 HCP_ACE_STR_DFLT);
                DVP_ASSERT_EQUAL(pstSessCb->stDrvCtx.u32DeGain,
                                 HCP_DE_GAIN_DFLT);
            }
            else
            {
                if (bAieOn)
                {
                    DVP_ASSERT_EQUAL(pstSessCb->stDrvCtx.u32SatGain,
                                     HCP_SAT_GAIN_DFLT);
                    DVP_ASSERT_EQUAL(pstSessCb->stDrvCtx.u32AceStrCon,
                                     HCP_ACE_STR_DFLT);
                    DVP_ASSERT_EQUAL(pstSessCb->stDrvCtx.u32DeGain,
                                     HCP_DE_GAIN_DFLT);
                }
                if (bEarOn)
                {
                    DVP_ASSERT_EQUAL(pstSessCb->stDrvCtx.u32EarMode,
                                     getDriverEarMode(HQV_EAR_MODE_MEDIUM));
                }
            }
            break;
        case HCP_OP_MODE_MEAS:
        case HCP_OP_MODE_BYPASS:
                DVP_ASSERT_EQUAL(pstSessCb->stDrvCtx.u32EarMode,
                                 HCP_EAR_MODE_MIN);
                DVP_ASSERT_EQUAL(pstSessCb->stDrvCtx.u32SatGain,
                                 HCP_SAT_GAIN_MIN);
                DVP_ASSERT_EQUAL(pstSessCb->stDrvCtx.u32AceStrCon,
                                 HCP_ACE_STR_MIN);
                DVP_ASSERT_EQUAL(pstSessCb->stDrvCtx.u32DeGain,
                                 HCP_DE_GAIN_MIN);
            break;
        default:
            DVP_ASSERT_FAIL();
            break;
    }
}

static void vHcpIpTest_TestKnobsAndOpModes(struct test_ctx *pstTestCtx,
                                           uint32_t bCheckKnobs)
{
    t_EVppIpHcpOpMode eExpectedOpMode;
    uint32_t u32;
    uint32_t u32Cnt;
    uint32_t u32CaseCnt;
    uint32_t u32CtrlCnt;
    uint32_t bEarOn = VPP_FALSE;
    uint32_t bAieOn = VPP_FALSE;
    uint32_t u32BufCnt = 0;
    enum hqv_control_type eCtrlType;
    struct hqv_control stCtrl;
    t_StVppIpHcpSessionCb *pstSessCb;

    if (!pstTestCtx)
    {
        DVP_ASSERT_FAIL();
        return;
    }

    t_StVppCtx stCtx;
    memset(&stCtx, 0, sizeof(t_StVppCtx));

    t_StVppCallback cbs;
    memset(&cbs, 0, sizeof(cbs));
    set_callbacks(&cbs, NULL);

    u32 = u32HcpIpTest_SetCtxPortParams(pstTestCtx, 1920, 1080,
                                        VPP_COLOR_FORMAT_NV12_VENUS);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    if (bCheckKnobs)
        u32BufCnt = 2;

    vHcpIpTest_TestSetupSetParam(pstTestCtx, u32BufCnt);
    pstSessCb = HCPIPTEST_HCP_TEST_CTX(pstTestCtx)->pstHcpCb;

    // AIE or EAR
    for (u32CtrlCnt = 0; u32CtrlCnt < 2; u32CtrlCnt++)
    {
        if (u32CtrlCnt == 0)
            eCtrlType = HQV_CONTROL_AIE;
        else
            eCtrlType = HQV_CONTROL_EAR;

        // Different Op mode rotations
        for (u32CaseCnt = 0; u32CaseCnt < 9; u32CaseCnt++)
        {
            // Clear existing controls.
            u32 = u32VppIpHcp_ResetCtrl(pstSessCb);
            DVP_ASSERT_EQUAL(u32, VPP_OK);

            // How many times to rotate between op modes
            for (u32Cnt = 0; u32Cnt < 30; u32Cnt++)
            {
                u32 = u32HcpIpTest_SelectOpModeAndCtrl(u32CaseCnt, u32Cnt,
                                                       eCtrlType,
                                                       &eExpectedOpMode,
                                                       &stCtrl);
                DVP_ASSERT_EQUAL(u32, VPP_OK);

                if (stCtrl.ctrl_type == HQV_CONTROL_AIE)
                {
                    if (stCtrl.aie.mode == HQV_MODE_OFF)
                        bAieOn = VPP_FALSE;
                    else if (stCtrl.aie.mode == HQV_MODE_AUTO ||
                             stCtrl.aie.mode == HQV_MODE_MANUAL)
                        bAieOn = VPP_TRUE;
                }
                else if (stCtrl.ctrl_type == HQV_CONTROL_EAR)
                {
                    bEarOn = !bEarWouldBypass(stCtrl.ear.mode);
                }

                u32 = u32VppIpHcp_SetCtrl(pstSessCb, stCtrl);
                DVP_ASSERT_EQUAL(u32, VPP_OK);

                u32 = u32HcpIpTest_Open(pstSessCb, eExpectedOpMode);
                DVP_ASSERT_EQUAL(u32, VPP_OK);

                if (bCheckKnobs)
                    vHcpIpTest_TestKnobsAndOpModes_CheckKnobs(pstTestCtx,
                                                              eExpectedOpMode,
                                                              stCtrl, bAieOn,
                                                              bEarOn);

                u32 = u32VppIpHcp_Close(pstSessCb);
                DVP_ASSERT_EQUAL(u32, VPP_OK);
            }
        }
    }

    // Switch OpMode in ACTIVE state.
    // PROC
    eExpectedOpMode = HCP_OP_MODE_PROC;
    stCtrl.mode = HQV_MODE_MANUAL;
    stCtrl.ctrl_type = HQV_CONTROL_AIE;
    stCtrl.aie.mode = HQV_MODE_AUTO;
    u32 = u32VppIpHcp_SetCtrl(pstSessCb, stCtrl);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = u32HcpIpTest_Open(pstSessCb, eExpectedOpMode);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // MEAS
    stCtrl.mode = HQV_MODE_MANUAL;
    stCtrl.ctrl_type = HQV_CONTROL_MEAS;
    stCtrl.meas.enable = 1;
    u32 = u32VppIpHcp_SetCtrl(pstSessCb, stCtrl);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);
    DVP_ASSERT_EQUAL(pstSessCb->stCfg.eOpMode, eExpectedOpMode);
    DVP_ASSERT_EQUAL(pstSessCb->stDrvCtx.eOpModeActive, eExpectedOpMode);

    vHcpIpTest_TestCleanup(pstTestCtx);
}

static void *pvHcpIpTest_WorkerQueueBufs(void *pv)
{
    uint32_t u32;
    uint32_t u32BufPoolSz = 20;
    uint32_t u32BufCnt = 20;
    struct test_ctx *pstTestCtx;

    if (!pv)
    {
        DVP_ASSERT_FAIL();
        return NULL;
    }

    pstTestCtx = (struct test_ctx *)pv;

    vHcpIpTest_TestSetup(pstTestCtx, u32BufPoolSz);

    u32 = u32HcpIpTest_QueueBufBothPorts(pstTestCtx, eVppBufType_Progressive,
                                         u32BufCnt);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    vHcpIpTest_WaitForAllBuffers(pstTestCtx, pstTestCtx->pPrivateCtx);
    DVP_ASSERT_EQUAL(pstTestCtx->u32InRxCnt, u32BufCnt);
    DVP_ASSERT_EQUAL(pstTestCtx->u32OutRxCnt, u32BufCnt);

    vHcpIpTest_TestCleanup(pstTestCtx);

    return NULL;
}

static void vHcpIpTest_TestConcurrency(t_StHcpIpTestSessCfg *pstCfg,
                                       uint32_t u32NumSess)
{
    uint32_t u32;
    uint32_t u32Cnt;
    struct test_ctx astTestCtx[HCP_MAX_SESSIONS];
    pthread_t thread[HCP_MAX_SESSIONS];

    if (!pstCfg)
    {
        DVP_ASSERT_FAIL();
        return;
    }

    if (u32NumSess > HCP_MAX_SESSIONS)
    {
        LOGE("%s: too many sessions. given=%u, max=%u", __func__,
             u32NumSess, HCP_MAX_SESSIONS);
        DVP_ASSERT_FAIL();
        return;
    }

    for (u32Cnt = 0; u32Cnt < u32NumSess; u32Cnt++)
    {
        vHcpIpTest_InitTestCtx(&astTestCtx[u32Cnt]);

        pthread_mutex_lock(&astTestCtx[u32Cnt].mutex);
        HCPIPTEST_HCP_TEST_CTX(&astTestCtx[u32Cnt])->stCfg = pstCfg[u32Cnt];
        pthread_mutex_unlock(&astTestCtx[u32Cnt].mutex);

        u32 = pthread_create(&thread[u32Cnt], NULL,
                             pvHcpIpTest_WorkerQueueBufs,
                             &astTestCtx[u32Cnt]);
        DVP_ASSERT_EQUAL(u32, 0);
    }

    for (u32Cnt = 0; u32Cnt < u32NumSess; u32Cnt++)
    {
        u32 = pthread_join(thread[u32Cnt], NULL);
        DVP_ASSERT_EQUAL(u32, 0);

        vHcpIpTest_TermTestCtx(&astTestCtx[u32Cnt]);
    }
}

static void vHcpIpTest_TestBufferExchange(struct test_ctx *pstTestCtx,
                                          uint32_t u32Width,
                                          uint32_t u32Height,
                                          uint32_t u32Fmt,
                                          t_EVppIpHcpOpMode eOpMode,
                                          uint32_t u32NumBufs)
{
    uint32_t u32;
    uint32_t u32BufPoolSize = 20;
    struct hcp_test_ctx *pstCtxHcp;

    if (!pstTestCtx || u32Fmt >= VPP_COLOR_FORMAT_MAX)
    {
        DVP_ASSERT_FAIL();
        return;
    }

    pstCtxHcp = HCPIPTEST_HCP_TEST_CTX(pstTestCtx);

    LOGI("BufferExchange: width=%u, height=%u, fmt=%u, opmode=%u",
         u32Width, u32Height, u32Fmt, eOpMode);

    u32 = u32HcpIpTest_SetCtxPortParams(&stTestCtx, u32Width, u32Height,
                                        u32Fmt);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    LOGD("BufferExchange: width=%u, height=%u, stride=%u, "
         "scanlines=%u, fmt=%u, opmode=%u",
         pstCtxHcp->stCfg.stPortParam.width,
         pstCtxHcp->stCfg.stPortParam.height,
         pstCtxHcp->stCfg.stPortParam.stride,
         pstCtxHcp->stCfg.stPortParam.scanlines,
         pstCtxHcp->stCfg.stPortParam.fmt, eOpMode);

    // Change Op mode.
    u32 = u32HcpIpTest_SetCtxHqvCtrl(pstTestCtx, eOpMode);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    vHcpIpTest_TestSetup(pstTestCtx, u32BufPoolSize);

    u32 = u32HcpIpTest_QueueBufBothPorts(pstTestCtx, eVppBufType_Progressive,
                                         u32NumBufs);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    vHcpIpTest_WaitForAllBuffers(pstTestCtx, pstCtxHcp);
    DVP_ASSERT_EQUAL(pstTestCtx->u32InRxCnt, u32NumBufs);
    DVP_ASSERT_EQUAL(pstTestCtx->u32OutRxCnt, u32NumBufs);

    vHcpIpTest_TestCleanup(pstTestCtx);
}

/************************************************************************
 * Test Functions
 ***********************************************************************/
TEST_SUITE_INIT(HcpSuiteInit)
{
    if (u32HcpMock_EnableErrorAction)
        u32HcpMock_EnableErrorAction(vDvpTb_Assert, VPP_FALSE, VPP_TRUE);
}

TEST_SUITE_TERM(HcpSuiteTerm)
{
    if (u32HcpMock_EnableErrorAction)
        u32HcpMock_EnableErrorAction(NULL, VPP_FALSE, VPP_FALSE);
}

TEST_SETUP(HcpTestInit)
{
    vHcpIpTest_InitTestCtx(&stTestCtx);
}

TEST_CLEANUP(HcpTestTerm)
{
    vHcpIpTest_TermTestCtx(&stTestCtx);
}

TEST(HcpBootShutdown)
{
    uint32_t u32;

    u32 = u32VppIpHcp_Boot();
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = u32VppIpHcp_Shutdown();
    DVP_ASSERT_EQUAL(u32, VPP_OK);
}

TEST(HcpBootShutdownBackToBack)
{
    uint32_t u32, i;

    for (i = 0; i < 200; i++)
    {
        LOGI("%s, i=%u", __func__, i);

        u32 = u32VppIpHcp_Boot();
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        u32 = u32VppIpHcp_Shutdown();
        DVP_ASSERT_EQUAL(u32, VPP_OK);
    }
}

TEST(HcpRepeatedBoot)
{
    uint32_t u32, i;

    for (i = 0; i < 100; i++)
    {
        LOGI("%s, i=%u", __func__, i);

        u32 = u32VppIpHcp_Boot();
        if (!i)
            DVP_ASSERT_EQUAL(u32, VPP_OK);
        else
            DVP_ASSERT_EQUAL(u32, VPP_ERR_STATE);
    }

    u32 = u32VppIpHcp_Shutdown();
    DVP_ASSERT_EQUAL(u32, VPP_OK);
}

TEST(HcpRepeatedShutdown)
{
    uint32_t u32, i;

    u32 = u32VppIpHcp_Boot();
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    for (i = 0; i < 100; i++)
    {
        LOGI("%s, i=%u", __func__, i);
        u32 = u32VppIpHcp_Shutdown();

        if (!i)
            DVP_ASSERT_EQUAL(u32, VPP_OK);
        else
            DVP_ASSERT_EQUAL(u32, VPP_ERR_STATE);
    }
}

TEST(HcpInitTermNoBoot)
{
    void *pv;

    t_StVppCtx stCtx;
    memset(&stCtx, 0, sizeof(t_StVppCtx));
    t_StVppCallback cbs;
    set_callbacks(&cbs, NULL);

    pv = vpVppIpHcp_Init(&stCtx, 0, cbs);
    DVP_ASSERT_PTR_NNULL(pv);

    vVppIpHcp_Term(pv);
}

TEST(HcpInitToMax)
{
    uint32_t i, j;

    t_StVppCtx stCtx;
    memset(&stCtx, 0, sizeof(t_StVppCtx));

    void *apv[HCP_MAX_SESSIONS];
    memset(apv, 0, sizeof(apv));

    void *pv = NULL;
    t_StVppCallback cbs;
    set_callbacks(&cbs, NULL);

    for (i = 0; i < HCP_MAX_SESSIONS * 3; i++)
    {
        pv = vpVppIpHcp_Init(&stCtx, 0, cbs);
        if (i < HCP_MAX_SESSIONS)
        {
            DVP_ASSERT_PTR_NNULL(pv);
            apv[i] = pv;
            for (j = 0; j < i; j++)
            {
                DVP_ASSERT_PTR_NEQUAL(apv[j], pv);
            }
        }
        else
        {
            LOGD("i=%u, pv=%p", i, pv);
            DVP_ASSERT_PTR_NULL(pv);
        }
    }

    for (i = 0; i < HCP_MAX_SESSIONS; i++)
    {
        vVppIpHcp_Term(apv[i]);
    }
}

TEST(HcpBasicControlApi)
{
    uint32_t u32;
    struct vpp_port_param port;

    t_StVppCtx stCtx;
    t_StVppCallback cbs;

    memset(&stCtx, 0, sizeof(t_StVppCtx));
    set_callbacks(&cbs, NULL);

    port.width = 1920;
    port.height = 1080;
    port.stride = 1920;
    port.scanlines = 1088;
    port.fmt = VPP_COLOR_FORMAT_NV12_VENUS;

    u32 = u32VppIon_Init(&stCtx);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = u32VppIpHcp_Boot();
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    void *pv = vpVppIpHcp_Init(&stCtx, 0, cbs);
    DVP_ASSERT_PTR_NNULL(pv);

    u32 = u32VppIpHcp_SetParam(pv, VPP_PORT_INPUT, port);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = u32VppIpHcp_SetParam(pv, VPP_PORT_OUTPUT, port);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = u32VppIpHcp_Open(pv);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = u32VppIpHcp_Close(pv);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    vVppIpHcp_Term(pv);

    u32 = u32VppIpHcp_Shutdown();
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = u32VppIon_Term(&stCtx);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
}

TEST(HcpSetCtrlChangesOpMode)
{
    /*
     * Test that calling SetCtrl() back and forth, causing an op mode switch
     * actually causes the IP block to change op mode on itself and firmware.
     * This test does not call open, so that the firmware remains in ALLOC
     * state and can have this be changed back and forth.
     */
    uint32_t u32;

    t_StVppCtx stCtx;
    t_StVppCallback cbs;
    t_StVppIpHcpSessionCb *pstSessCb;
    struct hqv_control ctrl;

    memset(&stCtx, 0, sizeof(t_StVppCtx));
    set_callbacks(&cbs, NULL);

    u32 = u32VppIon_Init(&stCtx);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = u32VppIpHcp_Boot();
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    void *pv = vpVppIpHcp_Init(&stCtx, 0, cbs);
    if (!pv)
    {
        DVP_ASSERT_FAIL();
        LOGE("vpVppIpHcp_Init returned NULL");
        return;
    }

    pstSessCb = (t_StVppIpHcpSessionCb *)pv;
    DVP_ASSERT_EQUAL(pstSessCb->stCfg.eOpMode, HCP_OP_MODE_BYPASS);

    ctrl.mode = HQV_MODE_MANUAL;
    ctrl.ctrl_type = HQV_CONTROL_MEAS;
    ctrl.meas.enable = VPP_TRUE;
    u32 = u32VppIpHcp_SetCtrl(pv, ctrl);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    DVP_ASSERT_EQUAL(pstSessCb->stCfg.eOpMode, HCP_OP_MODE_MEAS);
    DVP_ASSERT_EQUAL(pstSessCb->stDrvCtx.eOpModeActive, HCP_OP_MODE_MEAS);

    ctrl.ctrl_type = HQV_CONTROL_AIE;
    ctrl.aie.mode = HQV_MODE_AUTO;
    u32 = u32VppIpHcp_SetCtrl(pv, ctrl);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    DVP_ASSERT_EQUAL(pstSessCb->stCfg.eOpMode, HCP_OP_MODE_PROC);
    DVP_ASSERT_EQUAL(pstSessCb->stDrvCtx.eOpModeActive, HCP_OP_MODE_PROC);

    ctrl.mode = HQV_MODE_MANUAL;
    ctrl.ctrl_type = HQV_CONTROL_MEAS;
    ctrl.meas.enable = VPP_TRUE;
    u32 = u32VppIpHcp_SetCtrl(pv, ctrl);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    DVP_ASSERT_EQUAL(pstSessCb->stCfg.eOpMode, HCP_OP_MODE_MEAS);
    DVP_ASSERT_EQUAL(pstSessCb->stDrvCtx.eOpModeActive, HCP_OP_MODE_MEAS);

    ctrl.ctrl_type = HQV_CONTROL_EAR;
    ctrl.ear.mode = HQV_EAR_MODE_LOW;
    u32 = u32VppIpHcp_SetCtrl(pv, ctrl);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    DVP_ASSERT_EQUAL(pstSessCb->stCfg.eOpMode, HCP_OP_MODE_PROC);
    DVP_ASSERT_EQUAL(pstSessCb->stDrvCtx.eOpModeActive, HCP_OP_MODE_PROC);

    ctrl.mode = HQV_MODE_MANUAL;
    ctrl.ctrl_type = HQV_CONTROL_MEAS;
    ctrl.meas.enable = VPP_TRUE;
    u32 = u32VppIpHcp_SetCtrl(pv, ctrl);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    DVP_ASSERT_EQUAL(pstSessCb->stCfg.eOpMode, HCP_OP_MODE_MEAS);
    DVP_ASSERT_EQUAL(pstSessCb->stDrvCtx.eOpModeActive, HCP_OP_MODE_MEAS);

    vVppIpHcp_Term(pv);

    u32 = u32VppIpHcp_Shutdown();
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = u32VppIon_Term(&stCtx);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
}

TEST(HcpOpenWithoutSetCtrl)
{
    /*!
     * This test checks that calling Open() without calling SetCtrl() will not
     * return an error. Furthermore, it checks that the HCP IP is in bypass
     * mode since no controls were given to it.
     */

    uint32_t u32;
    struct vpp_port_param stParam;
    t_StVppIpHcpSessionCb *pstSessCb;

    t_StVppCtx stCtx;
    memset(&stCtx, 0, sizeof(t_StVppCtx));

    t_StVppCallback cbs;
    memset(&cbs, 0, sizeof(cbs));
    set_callbacks(&cbs, NULL);

    u32 = u32VppIpHcp_Boot();
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    void *pv = vpVppIpHcp_Init(&stCtx, 0, cbs);
    if (!pv)
    {
        DVP_ASSERT_FAIL();
        LOGE("vpVppIpHcp_Init returned NULL");
        return;
    }
    pstSessCb = (t_StVppIpHcpSessionCb *)pv;

    stParam.width = 1920;
    stParam.height = 1080;
    stParam.stride = 1920;
    stParam.scanlines = 1088;
    stParam.fmt = VPP_COLOR_FORMAT_NV12_VENUS;

    u32 = u32VppIpHcp_SetParam(pv, VPP_PORT_INPUT, stParam);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    u32 = u32VppIpHcp_SetParam(pv, VPP_PORT_OUTPUT, stParam);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = u32VppIpHcp_Open(pv);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    DVP_ASSERT_EQUAL(pstSessCb->stCfg.eOpMode, HCP_OP_MODE_BYPASS);

    u32 = u32VppIpHcp_Close(pv);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    vVppIpHcp_Term(pv);

    u32 = u32VppIpHcp_Shutdown();
    DVP_ASSERT_EQUAL(u32, VPP_OK);
}

TEST(HcpSetParam)
{
    /*!
     * Try all invalid and valid parameters when calling SetParam(). This
     * includes invalid width, height, stride, scanlines, color format, as well
     * as mismatched width, height, stride, scanlines, color format between
     * input and output params when calling Open(). Furthermore, it checks that
     * if in MEAS op mode, a mismatch between input and output params does not
     * cause an error in Open().
     */
    uint32_t u32;
    struct hqv_control stCtrl;
    struct vpp_port_param stParam;
    struct vpp_port_param stParamDflt;
    t_StVppIpHcpSessionCb *pstSessCb;

    t_StVppCtx stCtx;
    memset(&stCtx, 0, sizeof(t_StVppCtx));

    t_StVppCallback cbs;
    memset(&cbs, 0, sizeof(cbs));
    set_callbacks(&cbs, NULL);

    memset(&stCtrl, 0, sizeof(stCtrl));

    u32 = u32VppIon_Init(&stCtx);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = u32VppIpHcp_Boot();
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    void *pv = vpVppIpHcp_Init(&stCtx, 0, cbs);
    if (!pv)
    {
        DVP_ASSERT_FAIL();
        LOGE("vpVppIpHcp_Init returned NULL");
        return;
    }
    pstSessCb = (t_StVppIpHcpSessionCb *)pv;

    stParamDflt.width = 1920;
    stParamDflt.height = 1080;
    stParamDflt.stride = 1920;
    stParamDflt.scanlines = 1088;
    stParamDflt.fmt = VPP_COLOR_FORMAT_NV12_VENUS;
    stParam = stParamDflt;

    // Invalid cases for SetParam().
    u32 = u32VppIpHcp_SetParam(NULL, VPP_PORT_INPUT, stParam);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);

    u32 = u32VppIpHcp_SetParam(pv, VPP_PORT_MAX, stParam);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);

    stParam.fmt = VPP_COLOR_FORMAT_MAX;
    u32 = u32VppIpHcp_SetParam(pv, VPP_PORT_INPUT, stParam);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);
    stParam.fmt = stParamDflt.fmt;

    stParam.width = HCP_WIDTH_MIN - HCP_WIDTH_MULTIPLE;
    u32 = u32VppIpHcp_SetParam(pv, VPP_PORT_INPUT, stParam);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);
    stParam.width = stParamDflt.width;

    stParam.width = HCP_WIDTH_MAX + HCP_WIDTH_MULTIPLE;
    u32 = u32VppIpHcp_SetParam(pv, VPP_PORT_INPUT, stParam);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);
    stParam.width = stParamDflt.width;

    if (HCP_WIDTH_MULTIPLE > 1)
    {
        stParam.width = 1920 + 1;
        u32 = u32VppIpHcp_SetParam(pv, VPP_PORT_INPUT, stParam);
        DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);
        stParam.width = stParamDflt.width;
    }

    stParam.height = HCP_HEIGHT_MIN - HCP_HEIGHT_MULTIPLE;
    u32 = u32VppIpHcp_SetParam(pv, VPP_PORT_INPUT, stParam);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);
    stParam.height = stParamDflt.height;

    stParam.height = HCP_HEIGHT_MAX + HCP_HEIGHT_MULTIPLE;
    u32 = u32VppIpHcp_SetParam(pv, VPP_PORT_INPUT, stParam);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);
    stParam.height = stParamDflt.height;

    if (HCP_HEIGHT_MULTIPLE > 1)
    {
        stParam.height = 1080 + 1;
        u32 = u32VppIpHcp_SetParam(pv, VPP_PORT_INPUT, stParam);
        DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);
        stParam.height = stParamDflt.height;
    }

    stParam.stride = HCP_STRIDE_MIN - HCP_STRIDE_MULTIPLE;
    u32 = u32VppIpHcp_SetParam(pv, VPP_PORT_INPUT, stParam);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);
    stParam.stride = stParamDflt.stride;

    stParam.stride = HCP_STRIDE_MAX + HCP_STRIDE_MULTIPLE;
    u32 = u32VppIpHcp_SetParam(pv, VPP_PORT_INPUT, stParam);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);
    stParam.stride = stParamDflt.stride;

    if (HCP_STRIDE_MULTIPLE > 1)
    {
        stParam.stride = 1920 + 1;
        u32 = u32VppIpHcp_SetParam(pv, VPP_PORT_INPUT, stParam);
        DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);
        stParam.stride = stParamDflt.stride;
    }

    stParam.scanlines = HCP_SCANLINES_MIN - HCP_SCANLINES_MULTIPLE;
    u32 = u32VppIpHcp_SetParam(pv, VPP_PORT_INPUT, stParam);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);
    stParam.scanlines = stParamDflt.scanlines;

    stParam.scanlines = HCP_SCANLINES_MAX + HCP_SCANLINES_MULTIPLE;
    u32 = u32VppIpHcp_SetParam(pv, VPP_PORT_INPUT, stParam);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);
    stParam.scanlines = stParamDflt.scanlines;

    if (HCP_SCANLINES_MULTIPLE > 1)
    {
        stParam.scanlines = 1088 + 1;
        u32 = u32VppIpHcp_SetParam(pv, VPP_PORT_INPUT, stParam);
        DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);
        stParam.scanlines = stParamDflt.scanlines;
    }

    // Valid cases.
    stParam.fmt = VPP_COLOR_FORMAT_NV12_VENUS;
    u32 = u32VppIpHcp_SetParam(pv, VPP_PORT_INPUT, stParam);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    stParam.fmt = VPP_COLOR_FORMAT_NV21_VENUS;
    u32 = u32VppIpHcp_SetParam(pv, VPP_PORT_INPUT, stParam);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    stParam.fmt = VPP_COLOR_FORMAT_P010;
    u32 = u32VppIpHcp_SetParam(pv, VPP_PORT_INPUT, stParam);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    stParam.fmt = VPP_COLOR_FORMAT_UBWC_NV12;
    u32 = u32VppIpHcp_SetParam(pv, VPP_PORT_INPUT, stParam);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    stParam.fmt = VPP_COLOR_FORMAT_UBWC_NV21;
    u32 = u32VppIpHcp_SetParam(pv, VPP_PORT_INPUT, stParam);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    stParam.fmt = VPP_COLOR_FORMAT_UBWC_TP10;
    u32 = u32VppIpHcp_SetParam(pv, VPP_PORT_INPUT, stParam);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    stParam.fmt = VPP_COLOR_FORMAT_NV12_VENUS;
    stParam.width = HCP_WIDTH_MIN;
    stParam.height = HCP_HEIGHT_MIN;
    stParam.stride = HCP_STRIDE_MIN;
    stParam.scanlines = HCP_SCANLINES_MIN;
    u32 = u32VppIpHcp_SetParam(pv, VPP_PORT_INPUT, stParam);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    stParam.fmt = VPP_COLOR_FORMAT_NV12_VENUS;
    stParam.width = HCP_WIDTH_MAX;
    stParam.height = HCP_HEIGHT_MAX;
    stParam.stride = u32HcpIpTest_GetMaxStride(stParam.fmt);
    stParam.scanlines = HCP_SCANLINES_MAX;
    u32 = u32VppIpHcp_SetParam(pv, VPP_PORT_INPUT, stParam);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // Input and Output mismatch invalid cases.
    stCtrl.mode = HQV_MODE_MANUAL;
    stCtrl.ctrl_type = HQV_CONTROL_AIE;
    stCtrl.aie.mode = HQV_MODE_AUTO;
    u32 = u32VppIpHcp_SetCtrl(pv, stCtrl);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    stParam = stParamDflt;
    u32 = u32VppIpHcp_SetParam(pv, VPP_PORT_INPUT, stParam);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    stParam.width = HCP_WIDTH_MAX;
    u32 = u32VppIpHcp_SetParam(pv, VPP_PORT_OUTPUT, stParam);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    u32 = u32VppIpHcp_Open(pv);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);

    stParam = stParamDflt;
    u32 = u32VppIpHcp_SetParam(pv, VPP_PORT_INPUT, stParam);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    stParam.height = HCP_HEIGHT_MAX;
    u32 = u32VppIpHcp_SetParam(pv, VPP_PORT_OUTPUT, stParam);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    u32 = u32VppIpHcp_Open(pv);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);

    stParam = stParamDflt;
    u32 = u32VppIpHcp_SetParam(pv, VPP_PORT_INPUT, stParam);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    stParam.stride = HCP_STRIDE_MAX;
    u32 = u32VppIpHcp_SetParam(pv, VPP_PORT_OUTPUT, stParam);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    u32 = u32VppIpHcp_Open(pv);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);

    stParam = stParamDflt;
    u32 = u32VppIpHcp_SetParam(pv, VPP_PORT_INPUT, stParam);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    stParam.scanlines = HCP_SCANLINES_MAX;
    u32 = u32VppIpHcp_SetParam(pv, VPP_PORT_OUTPUT, stParam);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    u32 = u32VppIpHcp_Open(pv);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);

    stParam = stParamDflt;
    u32 = u32VppIpHcp_SetParam(pv, VPP_PORT_INPUT, stParam);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    stParam.fmt = VPP_COLOR_FORMAT_P010;
    u32 = u32VppIpHcp_SetParam(pv, VPP_PORT_OUTPUT, stParam);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    u32 = u32VppIpHcp_Open(pv);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);

    // Valid cases for input and output matching.
    stParam = stParamDflt;
    u32 = u32VppIpHcp_SetParam(pv, VPP_PORT_INPUT, stParam);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    u32 = u32VppIpHcp_SetParam(pv, VPP_PORT_OUTPUT, stParam);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    u32 = u32VppIpHcp_Open(pv);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    u32 = u32VppIpHcp_Close(pv);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // Valid case: setting output port to anything when in MEAS mode.
    stCtrl.mode = HQV_MODE_MANUAL;
    stCtrl.ctrl_type = HQV_CONTROL_MEAS;
    stCtrl.meas.enable = 1;
    u32 = u32VppIpHcp_SetCtrl(pv, stCtrl);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    stParam = stParamDflt;
    u32 = u32VppIpHcp_SetParam(pv, VPP_PORT_INPUT, stParam);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    stParam.width = HCP_WIDTH_MAX + 2;
    stParam.height = HCP_HEIGHT_MAX + 2;
    stParam.stride = HCP_STRIDE_MAX + 2;
    stParam.scanlines = HCP_SCANLINES_MAX + 2;
    stParam.fmt = VPP_COLOR_FORMAT_MAX + 2;
    u32 = u32VppIpHcp_SetParam(pv, VPP_PORT_OUTPUT, stParam);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    u32 = u32VppIpHcp_Open(pv);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    u32 = u32VppIpHcp_Close(pv);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // Invalid case: calling SetParam() in ACTIVE state.
    stParam = stParamDflt;
    u32 = u32VppIpHcp_SetParam(pv, VPP_PORT_INPUT, stParam);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    u32 = u32VppIpHcp_SetParam(pv, VPP_PORT_OUTPUT, stParam);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    u32 = u32VppIpHcp_Open(pv);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = u32VppIpHcp_SetParam(pv, VPP_PORT_INPUT, stParam);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_STATE);
    u32 = u32VppIpHcp_SetParam(pv, VPP_PORT_OUTPUT, stParam);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_STATE);

    u32 = u32VppIpHcp_Close(pv);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    vVppIpHcp_Term(pv);

    u32 = u32VppIpHcp_Shutdown();
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = u32VppIon_Term(&stCtx);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
}

TEST(HcpSetParamMaxStridePerColorFormat)
{
    /*
     * Will test the max value for the stride for each of the color formats,
     * since the stride value changes per format.
     */
    uint32_t u32Ret;
    uint32_t u32MaxStride;
    enum vpp_color_format eFmt;
    void *pv;
    struct vpp_port_param stParam;
    struct vpp_port_param stParamDefault;

    stParamDefault.width = 1920;
    stParamDefault.height = 1080;
    stParamDefault.stride = 1920;
    stParamDefault.scanlines = 1088;
    stParamDefault.fmt = VPP_COLOR_FORMAT_NV12_VENUS;
    stParam = stParamDefault;

    vHcpIpTest_TestSetupInit(&stTestCtx);
    pv = HCPIPTEST_HCP_TEST_CTX(&stTestCtx)->pstHcpCb;

    for (eFmt = 0; eFmt < VPP_COLOR_FORMAT_MAX; eFmt++)
    {
        // get max stride.
        u32MaxStride = u32HcpIpTest_GetMaxStride(eFmt);
        stParam.fmt = eFmt;

        // At exactly the maximum stride value.
        stParam.stride = u32MaxStride;
        u32Ret = u32VppIpHcp_SetParam(pv, VPP_PORT_INPUT, stParam);
        DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
        stParam = stParamDefault;

        // Above the maximum stride value.
        stParam.stride = u32MaxStride + HCP_STRIDE_MULTIPLE;
        u32Ret = u32VppIpHcp_SetParam(pv, VPP_PORT_INPUT, stParam);
        DVP_ASSERT_EQUAL(u32Ret, VPP_ERR_PARAM);
        stParam = stParamDefault;
    }

    vHcpIpTest_TestCleanup(&stTestCtx);
}

TEST(HcpSetCtrlValidity)
{
    /*!
     * Gives invalid knobs to HCP, to see that it returns an error. This
     * includes giving each config level an out-of-range value as well as
     * invalid enums to hqv mode, hqv ctrl type, and hcp mode.
     */
    uint32_t u32;
    struct hqv_control stCtrl;
    memset(&stCtrl, 0, sizeof(stCtrl));

    t_StVppCtx stCtx;
    memset(&stCtx, 0, sizeof(t_StVppCtx));

    t_StVppCallback cbs;
    memset(&cbs, 0, sizeof(cbs));
    set_callbacks(&cbs, NULL);

    u32 = u32VppIpHcp_Boot();
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    void *pv = vpVppIpHcp_Init(&stCtx, 0, cbs);
    if (!pv)
    {
        DVP_ASSERT_FAIL();
        LOGE("vpVppIpHcp_Init returned NULL");
        return;
    }

    // Invalid cases.
    u32 = u32VppIpHcp_SetCtrl(NULL, stCtrl);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);

    memset(&stCtrl, 0, sizeof(stCtrl));
    stCtrl.mode = HQV_MODE_MAX;
    u32 = u32VppIpHcp_SetCtrl(pv, stCtrl);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);

    memset(&stCtrl, 0, sizeof(stCtrl));
    stCtrl.mode = HQV_MODE_MANUAL;
    stCtrl.ctrl_type = HQV_CONTROL_MAX;
    u32 = u32VppIpHcp_SetCtrl(pv, stCtrl);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);

    memset(&stCtrl, 0, sizeof(stCtrl));
    stCtrl.mode = HQV_MODE_MANUAL;
    stCtrl.ctrl_type = HQV_CONTROL_CADE;
    u32 = u32VppIpHcp_SetCtrl(pv, stCtrl);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);

    memset(&stCtrl, 0, sizeof(stCtrl));
    stCtrl.mode = HQV_MODE_MANUAL;
    stCtrl.ctrl_type = HQV_CONTROL_GLOBAL_DEMO;
    stCtrl.demo.process_percent = HCP_SPLIT_MAX + 1;
    stCtrl.demo.process_direction = HQV_SPLIT_LEFT_TO_RIGHT;
    u32 = u32VppIpHcp_SetCtrl(pv, stCtrl);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);

    memset(&stCtrl, 0, sizeof(stCtrl));
    stCtrl.mode = HQV_MODE_MANUAL;
    stCtrl.ctrl_type = HQV_CONTROL_AIE;
    stCtrl.aie.mode = HQV_MODE_MAX;
    u32 = u32VppIpHcp_SetCtrl(pv, stCtrl);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);

    memset(&stCtrl, 0, sizeof(stCtrl));
    stCtrl.mode = HQV_MODE_MANUAL;
    stCtrl.ctrl_type = HQV_CONTROL_EAR;
    stCtrl.ear.mode = HQV_EAR_MODE_MAX + 1;
    u32 = u32VppIpHcp_SetCtrl(pv, stCtrl);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);

    memset(&stCtrl, 0, sizeof(stCtrl));
    stCtrl.mode = HQV_MODE_MANUAL;
    stCtrl.ctrl_type = HQV_CONTROL_AIE;
    stCtrl.aie.mode = HQV_MODE_MANUAL;
    stCtrl.aie.ltm_sat_gain = AIE_LTM_SAT_GAIN_MAX + 1;
    u32 = u32VppIpHcp_SetCtrl(pv, stCtrl);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);

    memset(&stCtrl, 0, sizeof(stCtrl));
    stCtrl.mode = HQV_MODE_MANUAL;
    stCtrl.ctrl_type = HQV_CONTROL_AIE;
    stCtrl.aie.mode = HQV_MODE_MANUAL;
    stCtrl.aie.ltm_sat_offset = AIE_LTM_SAT_OFFSET_MAX + 1;
    u32 = u32VppIpHcp_SetCtrl(pv, stCtrl);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);

    memset(&stCtrl, 0, sizeof(stCtrl));
    stCtrl.mode = HQV_MODE_MANUAL;
    stCtrl.ctrl_type = HQV_CONTROL_AIE;
    stCtrl.aie.mode = HQV_MODE_MANUAL;
    stCtrl.aie.ltm_ace_str = AIE_LTM_ACE_STR_MAX + 1;
    u32 = u32VppIpHcp_SetCtrl(pv, stCtrl);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);

    memset(&stCtrl, 0, sizeof(stCtrl));
    stCtrl.mode = HQV_MODE_MANUAL;
    stCtrl.ctrl_type = HQV_CONTROL_AIE;
    stCtrl.aie.mode = HQV_MODE_MANUAL;
    stCtrl.aie.ltm_ace_bright_l = AIE_LTM_ACE_BRI_L_MAX + 1;
    u32 = u32VppIpHcp_SetCtrl(pv, stCtrl);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);

    memset(&stCtrl, 0, sizeof(stCtrl));
    stCtrl.mode = HQV_MODE_MANUAL;
    stCtrl.ctrl_type = HQV_CONTROL_AIE;
    stCtrl.aie.mode = HQV_MODE_MANUAL;
    stCtrl.aie.ltm_ace_bright_h = AIE_LTM_ACE_BRI_H_MAX + 1;
    u32 = u32VppIpHcp_SetCtrl(pv, stCtrl);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);

    memset(&stCtrl, 0, sizeof(stCtrl));
    stCtrl.mode = HQV_MODE_MANUAL;
    stCtrl.ctrl_type = HQV_CONTROL_AIE;
    stCtrl.aie.mode = HQV_MODE_MANUAL;
    stCtrl.aie.cade_level = AIE_CADE_LEVEL_MAX + 1;
    u32 = u32VppIpHcp_SetCtrl(pv, stCtrl);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);

    // Valid cases.
    memset(&stCtrl, 0, sizeof(stCtrl));
    stCtrl.mode = HQV_MODE_AUTO;
    u32 = u32VppIpHcp_SetCtrl(pv, stCtrl);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    memset(&stCtrl, 0, sizeof(stCtrl));
    stCtrl.mode = HQV_MODE_OFF;
    u32 = u32VppIpHcp_SetCtrl(pv, stCtrl);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    memset(&stCtrl, 0, sizeof(stCtrl));
    stCtrl.mode = HQV_MODE_MANUAL;
    stCtrl.ctrl_type = HQV_CONTROL_AIE;
    stCtrl.aie.mode = HQV_MODE_AUTO;
    u32 = u32VppIpHcp_SetCtrl(pv, stCtrl);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    memset(&stCtrl, 0, sizeof(stCtrl));
    stCtrl.mode = HQV_MODE_MANUAL;
    stCtrl.ctrl_type = HQV_CONTROL_AIE;
    stCtrl.aie.mode = HQV_MODE_OFF;
    u32 = u32VppIpHcp_SetCtrl(pv, stCtrl);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    memset(&stCtrl, 0, sizeof(stCtrl));
    stCtrl.mode = HQV_MODE_MANUAL;
    stCtrl.ctrl_type = HQV_CONTROL_AIE;
    stCtrl.aie.mode = HQV_MODE_MANUAL;
    stCtrl.aie.ltm_sat_gain = HCP_SAT_GAIN_DFLT;
    stCtrl.aie.ltm_sat_offset = HCP_SAT_OFFSET_DFLT;
    stCtrl.aie.ltm_ace_str = HCP_ACE_STR_DFLT;
    stCtrl.aie.ltm_ace_bright_l = HCP_ACE_BRI_L_DFLT;
    stCtrl.aie.ltm_ace_bright_h = HCP_ACE_BRI_H_DFLT;
    stCtrl.aie.cade_level = HCP_DE_GAIN_DFLT;
    u32 = u32VppIpHcp_SetCtrl(pv, stCtrl);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    memset(&stCtrl, 0, sizeof(stCtrl));
    stCtrl.mode = HQV_MODE_MANUAL;
    stCtrl.ctrl_type = HQV_CONTROL_EAR;
    stCtrl.ear.mode = HQV_EAR_MODE_HIGH;
    u32 = u32VppIpHcp_SetCtrl(pv, stCtrl);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    memset(&stCtrl, 0, sizeof(stCtrl));
    stCtrl.mode = HQV_MODE_MANUAL;
    stCtrl.ctrl_type = HQV_CONTROL_EAR;
    stCtrl.ear.mode = HQV_EAR_MODE_OFF;
    u32 = u32VppIpHcp_SetCtrl(pv, stCtrl);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    memset(&stCtrl, 0, sizeof(stCtrl));
    stCtrl.mode = HQV_MODE_MANUAL;
    stCtrl.ctrl_type = HQV_CONTROL_MEAS;
    stCtrl.meas.enable = VPP_TRUE;
    u32 = u32VppIpHcp_SetCtrl(pv, stCtrl);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    memset(&stCtrl, 0, sizeof(stCtrl));
    stCtrl.mode = HQV_MODE_MANUAL;
    stCtrl.ctrl_type = HQV_CONTROL_GLOBAL_DEMO;
    stCtrl.demo.process_percent = HCP_SPLIT_DFLT;
    stCtrl.demo.process_direction = HQV_SPLIT_TOP_TO_BOTTOM;
    u32 = u32VppIpHcp_SetCtrl(pv, stCtrl);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    vVppIpHcp_Term(pv);

    u32 = u32VppIpHcp_Shutdown();
    DVP_ASSERT_EQUAL(u32, VPP_OK);
}

TEST(HcpResetCtrl)
{
    /*!
     * Checks invalid and valid params passed into HcpResetCtrl(). Also checks
     * that it does indeed reset the ctrls in the HCP. Also checks that the
     * API does not work when HCP is in ACTIVE state.
     */
    uint32_t u32;
    struct vpp_port_param port;
    struct hqv_control stCtrl;
    t_StVppIpHcpSessionCb *pstSessCb;

    t_StVppCtx stCtx;
    memset(&stCtx, 0, sizeof(t_StVppCtx));

    t_StVppCallback cbs;
    memset(&cbs, 0, sizeof(cbs));
    set_callbacks(&cbs, NULL);

    port.width = 1920;
    port.height = 1080;
    port.stride = 1920;
    port.scanlines = 1088;
    port.fmt = VPP_COLOR_FORMAT_NV12_VENUS;

    u32 = u32VppIon_Init(&stCtx);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = u32VppIpHcp_Boot();
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    void *pv = vpVppIpHcp_Init(&stCtx, 0, cbs);
    if (!pv)
    {
        DVP_ASSERT_FAIL();
        LOGE("vpVppIpHcp_Init returned NULL");
        return;
    }
    pstSessCb = (t_StVppIpHcpSessionCb *)pv;

    u32 = u32VppIpHcp_SetParam(pv, VPP_PORT_INPUT, port);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = u32VppIpHcp_SetParam(pv, VPP_PORT_OUTPUT, port);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // Invalid case of ResetCtrl().
    u32 = u32VppIpHcp_ResetCtrl(NULL);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);
    DVP_ASSERT_EQUAL(pstSessCb->eState, VPP_IP_STATE_INITED);

    // Valid case of ResetCtrl().
    u32 = u32VppIpHcp_ResetCtrl(pv);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    DVP_ASSERT_EQUAL(pstSessCb->eState, VPP_IP_STATE_INITED);

    // Check that ResetCtrl() does indeed reset the controls.
    stCtrl.mode = HQV_MODE_MANUAL;
    stCtrl.ctrl_type = HQV_CONTROL_AIE;
    stCtrl.aie.mode = HQV_MODE_AUTO;
    u32 = u32VppIpHcp_SetCtrl(pv, stCtrl);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    DVP_ASSERT_EQUAL(pstSessCb->stCfg.stAie.mode, HQV_MODE_AUTO);

    u32 = u32VppIpHcp_ResetCtrl(pv);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    DVP_ASSERT_EQUAL(pstSessCb->stCfg.stAie.mode, HQV_MODE_OFF);

    // Invalid case of ResetCtrl(), by calling ResetCtrl() in ACTIVE state.
    u32 = u32VppIpHcp_Open(pv);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    DVP_ASSERT_EQUAL(pstSessCb->eState, VPP_IP_STATE_ACTIVE);

    u32 = u32VppIpHcp_ResetCtrl(pv);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_STATE);

    u32 = u32VppIpHcp_Close(pv);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    vVppIpHcp_Term(pv);

    u32 = u32VppIpHcp_Shutdown();
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = u32VppIon_Term(&stCtx);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
}

TEST(HcpSwitchOpModes)
{
    /*!
     * Open and close session multiple times, switching between MEAS, PROC,
     * and BYPASS modes, both in INITED state and ACTIVE state. As well,
     * switches op mode via HQV AUTO, MANUAL, OFF.
     */
    vHcpIpTest_TestKnobsAndOpModes(&stTestCtx, VPP_FALSE);
}

static void HcpSetProcCtrlDuringActiveCheckKnobs(struct test_ctx *pstTestCtx,
                                                 uint32_t u32BufPairs,
                                                 struct hcp_knobs stKnobs)
{
    uint32_t u32;
    struct hcp_test_ctx *pstHcpTestCtx;
    t_StVppIpHcpSessionCb *pstSessCb;
    uint32_t bWouldBypass;

    if (!pstTestCtx)
    {
        DVP_ASSERT_FAIL();
        return;
    }

    pstHcpTestCtx = HCPIPTEST_HCP_TEST_CTX(pstTestCtx);
    pstSessCb = pstHcpTestCtx->pstHcpCb;

    bWouldBypass = (bEarWouldBypass(stKnobs.u32EarMode) &&
                    bAieWouldBypass(stKnobs));

    if (bWouldBypass)
        pstHcpTestCtx->u32NumExpectedBypassBufs = u32BufPairs;

    u32 = u32HcpIpTest_QueueBufBothPorts(pstTestCtx, eVppBufType_Progressive,
                                         u32BufPairs);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    if (bWouldBypass)
    {
        vHcpIpTest_WaitForAllInputBuffers(pstTestCtx, pstHcpTestCtx);
        pstHcpTestCtx->u32NumExpectedBypassBufs = 0;

        u32 = u32VppIpHcp_Flush(pstSessCb, VPP_PORT_OUTPUT);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
        vHcpIpTest_WaitForFlushDone(pstTestCtx, VPP_PORT_OUTPUT);
    }
    else
    {
        vHcpIpTest_WaitForAllBuffers(pstTestCtx, pstTestCtx->pPrivateCtx);
    }

    DVP_ASSERT_EQUAL(getDriverEarMode(stKnobs.u32EarMode),
                     pstSessCb->stDrvCtx.u32EarMode);
    DVP_ASSERT_EQUAL(stKnobs.u32DeGain, pstSessCb->stDrvCtx.u32DeGain);
    DVP_ASSERT_EQUAL(stKnobs.u32SatGain, pstSessCb->stDrvCtx.u32SatGain);
    DVP_ASSERT_EQUAL(stKnobs.u32SatOff, pstSessCb->stDrvCtx.u32SatOff);
    DVP_ASSERT_EQUAL(stKnobs.u32AceStrCon, pstSessCb->stDrvCtx.u32AceStrCon);
    DVP_ASSERT_EQUAL(stKnobs.u32AceBriL, pstSessCb->stDrvCtx.u32AceStrBriL);
    DVP_ASSERT_EQUAL(stKnobs.u32AceBriH, pstSessCb->stDrvCtx.u32AceStrBriH);
}

TEST(HcpSetProcCtrlDuringActive)
{
    /*!
     * Enable and disable both AIE and EAR controls during ACTIVE state, and
     * make sure we can turn them on and off if they were already enabled.
     */

    uint32_t u32;
    uint32_t u32BufCnt = 2; // 1 per port to validate knobs
    struct hqv_control stCtrl;
    t_StVppIpHcpSessionCb *pstSessCb;

    struct test_ctx *pstTestCtx = &stTestCtx;

    t_StVppCtx stCtx;
    memset(&stCtx, 0, sizeof(t_StVppCtx));

    t_StVppCallback cbs;
    memset(&cbs, 0, sizeof(cbs));
    set_callbacks(&cbs, NULL);

    u32 = u32HcpIpTest_SetCtxPortParams(pstTestCtx, 1920, 1080,
                                        VPP_COLOR_FORMAT_NV12_VENUS);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    vHcpIpTest_TestSetupSetParam(pstTestCtx, u32BufCnt);
    pstSessCb = HCPIPTEST_HCP_TEST_CTX(pstTestCtx)->pstHcpCb;

    struct hcp_knobs stKnobs;
    memset(&stKnobs, 0, sizeof(struct hcp_knobs));

    // so that we can detect byp accordingly
    stKnobs.u32SatOff = HCP_SAT_OFFSET_OFF;

    stCtrl.mode = HQV_MODE_MANUAL;

    // EAR only
    stCtrl.ctrl_type = HQV_CONTROL_EAR;
    stCtrl.ear.mode = HQV_EAR_MODE_LOW;
    u32 = u32VppIpHcp_SetCtrl(pstSessCb, stCtrl);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = u32HcpIpTest_Open(pstSessCb, HCP_OP_MODE_PROC);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    uint32_t i;
    for (i = HQV_EAR_MODE_OFF; i < HQV_EAR_MODE_MAX; i++)
    {
        LOGE("HcpSetProcCtrlDuringActive: ear mode=%u", i);
        stKnobs.u32EarMode = i;
        stCtrl.ear.mode = i;
        u32 = u32VppIpHcp_SetCtrl(pstSessCb, stCtrl);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        HcpSetProcCtrlDuringActiveCheckKnobs(pstTestCtx, 1, stKnobs);
    }

    // Turn on AIE in addition to EAR, since we are in PROC mode
    LOGE("HcpSetProcCtrlDuringActive: aie, 17/39/55/1/9/81");
    stKnobs.u32DeGain = 17;
    stKnobs.u32SatGain = 39;
    stKnobs.u32SatOff = 1;
    stKnobs.u32AceStrCon = 55;
    stKnobs.u32AceBriL = 9;
    stKnobs.u32AceBriH = 81;
    stCtrl.ctrl_type = HQV_CONTROL_AIE;
    stCtrl.aie.mode = HQV_MODE_MANUAL;
    stCtrl.aie.cade_level = stKnobs.u32DeGain;
    stCtrl.aie.ltm_sat_gain = stKnobs.u32SatGain;
    stCtrl.aie.ltm_sat_offset = stKnobs.u32SatOff;
    stCtrl.aie.ltm_ace_str = stKnobs.u32AceStrCon;
    stCtrl.aie.ltm_ace_bright_l = stKnobs.u32AceBriL;
    stCtrl.aie.ltm_ace_bright_h = stKnobs.u32AceBriH;
    u32 = u32VppIpHcp_SetCtrl(pstSessCb, stCtrl);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    HcpSetProcCtrlDuringActiveCheckKnobs(pstTestCtx, 1, stKnobs);

    // Turn off EAR by setting HQV_EAR_MODE_OFF
    LOGE("HcpSetProcCtrlDuringActive: ear off");
    stCtrl.ctrl_type = HQV_CONTROL_EAR;
    stCtrl.ear.mode = HQV_EAR_MODE_OFF;
    u32 = u32VppIpHcp_SetCtrl(pstSessCb, stCtrl);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    stKnobs.u32EarMode = 0;
    HcpSetProcCtrlDuringActiveCheckKnobs(pstTestCtx, 1, stKnobs);

    LOGE("HcpSetProcCtrlDuringActive: aie off");
    // Turn off AIE by setting AIE mode to HQV_MODE_OFF
    stCtrl.ctrl_type = HQV_CONTROL_AIE;
    stCtrl.aie.mode = HQV_MODE_OFF;
    u32 = u32VppIpHcp_SetCtrl(pstSessCb, stCtrl);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    stKnobs.u32DeGain = HCP_DE_GAIN_OFF;
    stKnobs.u32SatGain = HCP_SAT_GAIN_OFF;
    stKnobs.u32SatOff = HCP_SAT_OFFSET_OFF;
    stKnobs.u32AceStrCon = HCP_ACE_STR_OFF;
    stKnobs.u32AceBriL = HCP_ACE_BRI_L_OFF;
    stKnobs.u32AceBriH = HCP_ACE_BRI_H_OFF;
    HcpSetProcCtrlDuringActiveCheckKnobs(pstTestCtx, 1, stKnobs);

    vHcpIpTest_TestCleanup(pstTestCtx);
}

TEST(HcpQueueBufBasicTest)
{
    /*!
     * This will check for correct functionality during buffer exchanges. Uses a
     * basic, valid case to see if basic buffer exchange works correctly.
     */
    uint32_t u32BufCnt = 5;

    vHcpIpTest_TestBufferExchange(&stTestCtx, 720, 480,
                                  VPP_COLOR_FORMAT_NV12_VENUS,
                                  HCP_OP_MODE_PROC, u32BufCnt);
}

TEST(HcpQueueBufInvalid)
{
    /*
     * This test checks that any invalid params given to the queue_buf()
     * function will lead to an error. As well, will check that calling
     * queue_buf() in the wrong state will also return an error.
     */
    uint32_t u32;
    uint32_t u32BufCnt = 2;
    struct bufnode *pNode;
    t_StVppIpHcpSessionCb *pstSessCb;
    struct hcp_test_ctx *pstCtxHcp = HCPIPTEST_HCP_TEST_CTX(&stTestCtx);

    vHcpIpTest_TestSetupSetParam(&stTestCtx, u32BufCnt);
    pstSessCb = pstCtxHcp->pstHcpCb;
    void *pv = pstSessCb;

    // Invalid case: where queue_buf() is called in wrong state.
    pNode = get_buf(stTestCtx.buf_pool);
    if (pNode != NULL)
    {
        fill_buf(pNode);
        pNode->owner = BUF_OWNER_LIBRARY;
        fill_extra_data(pNode, stTestCtx.params.eBufferType, 0);
        pstCtxHcp->u32ExpExtraLen = pNode->pExtBuf->extradata.filled_len;
        pNode->pIntBuf->eQueuedPort = VPP_PORT_INPUT;
    }
    else
    {
        DVP_ASSERT_FAIL();
        LOGE("get_buf returned NULL buffer");
        return;
    }
    u32 = u32VppIpHcp_QueueBuf(pv, VPP_PORT_INPUT, pNode->pIntBuf);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_STATE);

    // Setup into ACTIVE state.
    vHcpIpTest_TestSetup(&stTestCtx, u32BufCnt);

    // Invalid cases.
    u32 = u32VppIpHcp_QueueBuf(NULL, VPP_PORT_INPUT, pNode->pIntBuf);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);

    u32 = u32VppIpHcp_QueueBuf(pv, VPP_PORT_MAX, pNode->pIntBuf);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);

    u32 = u32VppIpHcp_QueueBuf(pv, VPP_PORT_INPUT, NULL);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);

    // TODO: other invalid cases for queue buf?

    // Valid case.
    u32 = u32VppIpHcp_QueueBuf(pv, VPP_PORT_INPUT, pNode->pIntBuf);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    pthread_mutex_lock(&stTestCtx.mutex);
    stTestCtx.u32InPutCnt++;
    stTestCtx.u32InHoldCnt++;
    pthread_mutex_unlock(&stTestCtx.mutex);

    // Cleanup.
    vHcpIpTest_TestCleanup(&stTestCtx);
}

TEST(HcpQueueBufBasicBypass)
{
    /*
     * Checks conditions where HCP should return buffers via BYPASS.
     */
    t_EVppIpHcpOpMode eExpectedOpMode;
    uint32_t u32;
    uint32_t u32Cnt;
    uint32_t u32BufPoolSize = 10;
    uint32_t u32Temp;
    struct hqv_control stCtrl = {};
    t_StVppIpHcpSessionCb *pstSessCb;
    struct hcp_test_ctx *pstCtxHcp = HCPIPTEST_HCP_TEST_CTX(&stTestCtx);
    struct bufnode *pNode;
    enum vpp_port ePort;

    vHcpIpTest_TestSetupSetParam(&stTestCtx, u32BufPoolSize);
    void *pv = pstCtxHcp->pstHcpCb;
    pstSessCb = pstCtxHcp->pstHcpCb;

    // Bypass case: Open without SetCtrl().
    LOGI("Bypass case: Open() without SetCtrl().");
    pstCtxHcp->u32NumExpectedBypassBufs = 1;
    eExpectedOpMode = HCP_OP_MODE_BYPASS;
    u32 = u32HcpIpTest_Open(pstSessCb, eExpectedOpMode);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    LOGD("queueing buffer to input.");
    u32 = u32HcpIpTest_QueueBuf(&stTestCtx, VPP_PORT_INPUT,
                                eVppBufType_Progressive);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    vHcpIpTest_WaitForAllInputBuffers(&stTestCtx, pstCtxHcp);

    u32 = u32VppIpHcp_Close(pv);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // Bypass case: Turn off all controls.
    LOGI("Bypass case: Turn off all controls.");
    pstCtxHcp->u32NumExpectedBypassBufs = 1;
    eExpectedOpMode = HCP_OP_MODE_BYPASS;
    stCtrl.mode = HQV_MODE_OFF;
    u32 = u32VppIpHcp_SetCtrl(pv, stCtrl);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = u32HcpIpTest_Open(pstSessCb, eExpectedOpMode);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = u32HcpIpTest_QueueBuf(&stTestCtx, VPP_PORT_INPUT,
                                eVppBufType_Progressive);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    vHcpIpTest_WaitForAllInputBuffers(&stTestCtx, pstCtxHcp);

    u32 = u32VppIpHcp_Close(pv);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // Bypass case: Queue non-progressive buffers.
    for (u32Cnt = 0; u32Cnt < eVppBufType_Max; u32Cnt++)
    {
        if (u32Cnt == eVppBufType_Progressive)
            continue;

        LOGI("Bypass case: Queue buffers of type=%u.", u32Cnt);
        pstCtxHcp->u32NumExpectedBypassBufs = 1;
        eExpectedOpMode = HCP_OP_MODE_PROC;
        stCtrl.mode = HQV_MODE_AUTO;
        u32 = u32VppIpHcp_SetCtrl(pv, stCtrl);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        u32 = u32HcpIpTest_Open(pstSessCb, eExpectedOpMode);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        u32 = u32HcpIpTest_QueueBuf(&stTestCtx, VPP_PORT_INPUT, u32Cnt);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        vHcpIpTest_WaitForAllInputBuffers(&stTestCtx, pstCtxHcp);

        u32 = u32VppIpHcp_Close(pv);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
    }

    // Bypass case: filled_len = 0 on input buffer (all IPs)
    LOGI("Bypass case: filled_len = 0 on input buffer.");
    pstCtxHcp->u32NumExpectedBypassBufs = 1;
    eExpectedOpMode = HCP_OP_MODE_PROC;
    stCtrl.mode = HQV_MODE_AUTO;
    u32 = u32VppIpHcp_SetCtrl(pv, stCtrl);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = u32HcpIpTest_Open(pstSessCb, eExpectedOpMode);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = u32HcpIpTest_QueueBufCustomResolution(&stTestCtx, VPP_PORT_INPUT,
                                                eVppBufType_Progressive, 0, 0);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    vHcpIpTest_WaitForAllInputBuffers(&stTestCtx, pstCtxHcp);

    u32 = u32VppIpHcp_Close(pv);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // Bypass case: Bypass flag on the input buffer.
    LOGI("Bypass case: Bypass flag set on input buffer.");
    pstCtxHcp->u32NumExpectedBypassBufs = 1;
    eExpectedOpMode = HCP_OP_MODE_PROC;
    stCtrl.mode = HQV_MODE_AUTO;
    u32 = u32VppIpHcp_SetCtrl(pv, stCtrl);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = u32HcpIpTest_Open(pstSessCb, eExpectedOpMode);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = u32HcpIpTest_QueueBufCustomFlag(&stTestCtx, VPP_PORT_INPUT,
                                          eVppBufType_Progressive,
                                          VPP_BUF_FLAG_BYPASS);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    vHcpIpTest_WaitForAllInputBuffers(&stTestCtx, pstCtxHcp);

    u32 = u32VppIpHcp_Close(pv);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // Bypass case: u32ValidLen < size based on port params, on input port.
    LOGI("Bypass case: valid_len is smaller than size based by port params (input)");
    pstCtxHcp->u32NumExpectedBypassBufs = 1;
    eExpectedOpMode = HCP_OP_MODE_PROC;
    stCtrl.mode = HQV_MODE_AUTO;
    u32 = u32VppIpHcp_SetCtrl(pv, stCtrl);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = u32HcpIpTest_Open(pstSessCb, eExpectedOpMode);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    ePort = VPP_PORT_INPUT;
    u32 = u32HcpIpTest_FillBuf(&stTestCtx, ePort, eVppBufType_Progressive,
                               &pNode);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    u32Temp = pNode->pIntBuf->pBuf->pixel.valid_data_len;
    pNode->pIntBuf->pBuf->pixel.valid_data_len =
        u32VppUtils_GetPxBufferSize(&pstCtxHcp->stCfg.stPortParam) - 1;
    pNode->pIntBuf->stPixel.u32ValidLen =
        pNode->pIntBuf->pBuf->pixel.valid_data_len;
    u32 = u32HcpIpTest_QueueGivenBuf(&stTestCtx, pNode, ePort);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    vHcpIpTest_WaitForAllInputBuffers(&stTestCtx, pstCtxHcp);

    // Need to set back these values, they only get set in initialization
    pthread_mutex_lock(&stTestCtx.buf_pool->mutex);
    pNode->pIntBuf->pBuf->pixel.valid_data_len = u32Temp;
    pNode->pIntBuf->stPixel.u32ValidLen = u32Temp;
    pthread_mutex_unlock(&stTestCtx.buf_pool->mutex);
    u32Temp = 0;

    u32 = u32VppIpHcp_Close(pv);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // Bypass case: u32ValidLen < size based on port params, on output port.
    LOGI("Bypass case: valid_len is smaller than size based by port params (output)");
    eExpectedOpMode = HCP_OP_MODE_PROC;
    stCtrl.mode = HQV_MODE_AUTO;
    u32 = u32VppIpHcp_SetCtrl(pv, stCtrl);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = u32HcpIpTest_Open(pstSessCb, eExpectedOpMode);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    ePort = VPP_PORT_OUTPUT;
    u32 = u32HcpIpTest_FillBuf(&stTestCtx, ePort, eVppBufType_Progressive,
                               &pNode);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    u32Temp = pNode->pIntBuf->pBuf->pixel.valid_data_len;
    pNode->pIntBuf->pBuf->pixel.valid_data_len =
        u32VppUtils_GetPxBufferSize(&pstCtxHcp->stCfg.stPortParam) - 1;
    pNode->pIntBuf->stPixel.u32ValidLen =
        pNode->pIntBuf->pBuf->pixel.valid_data_len;
    u32 = u32HcpIpTest_QueueGivenBuf(&stTestCtx, pNode, ePort);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    vHcpIpTest_WaitForAllOutputBuffers(&stTestCtx, pstCtxHcp);

    // Need to set back these values, they only get set in initialization
    pthread_mutex_lock(&stTestCtx.buf_pool->mutex);
    pNode->pIntBuf->pBuf->pixel.valid_data_len = u32Temp;
    pNode->pIntBuf->stPixel.u32ValidLen = u32Temp;
    pthread_mutex_unlock(&stTestCtx.buf_pool->mutex);
    u32Temp = 0;

    u32 = u32VppIpHcp_Close(pv);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // Cleanup.
    vHcpIpTest_TestCleanup(&stTestCtx);
}

TEST(HcpFlushBasic)
{
    /*
     * Sees if a valid Flush command works correctly, behaving correctly.
     * Calls the Flush() function without queueing any buffers, to see that
     * the FLUSH_DONE is received on the specified port. Runs Flush() on both
     * input and output ports.
     */
    uint32_t u32;
    uint32_t u32BufCnt = 0;
    enum vpp_port ePort;
    t_StVppIpHcpSessionCb *pstSessCb;

    vHcpIpTest_TestSetupSetParam(&stTestCtx, u32BufCnt);
    pstSessCb = HCPIPTEST_HCP_TEST_CTX(&stTestCtx)->pstHcpCb;

    // Flush in INITED state.
    LOGI("Calling Flush() on input port.");
    ePort = VPP_PORT_INPUT;
    u32 = u32VppIpHcp_Flush(pstSessCb, ePort);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    vHcpIpTest_WaitForFlushDone(&stTestCtx, ePort);

    LOGI("Calling Flush() on output port.");
    ePort = VPP_PORT_OUTPUT;
    u32 = u32VppIpHcp_Flush(pstSessCb, ePort);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    vHcpIpTest_WaitForFlushDone(&stTestCtx, ePort);

    // Transition to ACTIVE state.
    vHcpIpTest_TestSetup(&stTestCtx, u32BufCnt);

    // Flush in ACTIVE state.
    LOGI("Calling Flush() on input port.");
    ePort = VPP_PORT_INPUT;
    u32 = u32VppIpHcp_Flush(pstSessCb, ePort);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    vHcpIpTest_WaitForFlushDone(&stTestCtx, ePort);

    LOGI("Calling Flush() on output port.");
    ePort = VPP_PORT_OUTPUT;
    u32 = u32VppIpHcp_Flush(pstSessCb, ePort);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    vHcpIpTest_WaitForFlushDone(&stTestCtx, ePort);

    vHcpIpTest_TestCleanup(&stTestCtx);
}

TEST(HcpFlushInvalid)
{
    /*
     * Will call Flush(), given invalid parameters, to make sure it returns an
     * error. Also will try to queue buffers in the flushed port before
     * FLUSH_DONE is given.
     */

    uint32_t u32;
    uint32_t u32InBufCnt = 5;
    uint32_t u32OutBufCnt = 5;
    uint32_t u32BufCnt = u32InBufCnt + u32OutBufCnt;
    enum vpp_port ePort;
    void *pv;

    vHcpIpTest_TestSetup(&stTestCtx, u32BufCnt);
    pv = HCPIPTEST_HCP_TEST_CTX(&stTestCtx)->pstHcpCb;

    // Invalid case: Flush with null pv pointer.
    u32 = u32VppIpHcp_Flush(NULL, VPP_PORT_INPUT);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);

    // Invalid case: Flush on invalid port.
    u32 = u32VppIpHcp_Flush(pv, VPP_PORT_MAX);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);

    // Valid case: Flush after giving some buffers.
    u32 = u32HcpIpTest_QueueBufBothPorts(&stTestCtx, eVppBufType_Progressive,
                                         u32InBufCnt);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    ePort = VPP_PORT_OUTPUT;
    u32 = u32VppIpHcp_Flush(pv, ePort);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    vHcpIpTest_WaitForFlushDone(&stTestCtx, ePort);
    DVP_ASSERT_EQUAL(stTestCtx.u32OutHoldCnt, 0);

    ePort = VPP_PORT_INPUT;
    u32 = u32VppIpHcp_Flush(pv, ePort);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    vHcpIpTest_WaitForFlushDone(&stTestCtx, ePort);
    DVP_ASSERT_EQUAL(stTestCtx.u32InHoldCnt, 0);

    vHcpIpTest_TestCleanup(&stTestCtx);
}

TEST(HcpFlushMultiple)
{
    /*
     * Will call Flush() multiple times to make sure multiple FLUSH_DONE events
     * are received.
     */

    uint32_t u32;
    uint32_t u32Cnt;
    uint32_t u32NumFlushes = 10;
    enum vpp_port ePort;
    void *pv;

    vHcpIpTest_TestSetup(&stTestCtx, 0);
    pv = HCPIPTEST_HCP_TEST_CTX(&stTestCtx)->pstHcpCb;

    // Call Flush() multiple times and wait for multiple FLUSH_DONE events.
    for (u32Cnt = 0; u32Cnt < u32NumFlushes; u32Cnt++)
    {
        ePort = VPP_PORT_INPUT;
        u32 = u32VppIpHcp_Flush(pv, ePort);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
    }
    pthread_mutex_lock(&stTestCtx.mutex);
    while (stTestCtx.u32InFlushCnt != u32NumFlushes)
        pthread_cond_wait(&stTestCtx.cond, &stTestCtx.mutex);
    pthread_mutex_unlock(&stTestCtx.mutex);

    for (u32Cnt = 0; u32Cnt < u32NumFlushes; u32Cnt++)
    {
        ePort = VPP_PORT_OUTPUT;
        u32 = u32VppIpHcp_Flush(pv, ePort);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
    }
    pthread_mutex_lock(&stTestCtx.mutex);
    while (stTestCtx.u32OutFlushCnt != u32NumFlushes)
        pthread_cond_wait(&stTestCtx.cond, &stTestCtx.mutex);
    pthread_mutex_unlock(&stTestCtx.mutex);

    vHcpIpTest_TestCleanup(&stTestCtx);
}

TEST(HcpDrainBasic)
{
    /*
     * This test will call Drain() on a session with no queued buffers, to see
     * that the DRAIN done command is given back.
     */
    uint32_t u32;
    void *pv;

    vHcpIpTest_TestSetup(&stTestCtx, 0);
    pv = HCPIPTEST_HCP_TEST_CTX(&stTestCtx)->pstHcpCb;

    u32 = u32VppIpHcp_Drain(pv);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    vHcpIpTest_WaitForDrainDone(&stTestCtx);

    vHcpIpTest_TestCleanup(&stTestCtx);
}

TEST(HcpDrainInvalid)
{
    /*
     * This test will call Drain() in all of its invalid scenarios.
     */
    uint32_t u32;
    uint32_t u32InBufCnt = 10;
    uint32_t u32OutBufCnt = 10;
    uint32_t u32BufCnt = u32InBufCnt + u32OutBufCnt;
    t_StVppIpHcpSessionCb *pstSessCb;

    vHcpIpTest_TestSetupSetParam(&stTestCtx, u32BufCnt);
    pstSessCb = HCPIPTEST_HCP_TEST_CTX(&stTestCtx)->pstHcpCb;

    // Invalid case: Calling Drain() in INITED state.
    u32 = u32VppIpHcp_Drain(pstSessCb);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_STATE);

    // Transition to ACTIVE state.
    vHcpIpTest_TestSetup(&stTestCtx, u32BufCnt);

    // Invalid case: Calling Drain() with a null ctx pointer;
    u32 = u32VppIpHcp_Drain(NULL);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);

    // Valid case with queued buffers.
    u32 = u32HcpIpTest_QueueBufBothPorts(&stTestCtx, eVppBufType_Progressive,
                                         u32InBufCnt);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = u32VppIpHcp_Drain((void *)pstSessCb);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    vHcpIpTest_WaitForDrainDone(&stTestCtx);
    DVP_ASSERT_EQUAL(stTestCtx.u32InHoldCnt, 0);

    vHcpIpTest_TestCleanup(&stTestCtx);
}

TEST(HcpDrainMultiple)
{
    /*
     * This test will call Drain() multiple times, to see that multiple
     * DRAIN_DONE events are received.
     */

    uint32_t u32;
    uint32_t u32Cnt;
    uint32_t u32NumDrains = 10;
    t_StVppIpHcpSessionCb *pstSessCb;
    struct hcp_test_ctx *pstCtxHcp = HCPIPTEST_HCP_TEST_CTX(&stTestCtx);

    vHcpIpTest_TestSetup(&stTestCtx, 0);
    pstSessCb = pstCtxHcp->pstHcpCb;

    for (u32Cnt = 0; u32Cnt < u32NumDrains; u32Cnt++)
    {
        u32 = u32VppIpHcp_Drain((void *)pstSessCb);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
    }
    pthread_mutex_lock(&stTestCtx.mutex);
    while (pstCtxHcp->u32DrainCnt != u32NumDrains)
        pthread_cond_wait(&stTestCtx.cond, &stTestCtx.mutex);
    pthread_mutex_unlock(&stTestCtx.mutex);

    vHcpIpTest_TestCleanup(&stTestCtx);
}

TEST(HcpReconfigBasic)
{
    /*
     * This test to see that a Reconfigure() call will change the port params
     * held inside the HCP session Cb.
     */
    uint32_t u32;
    struct vpp_port_param stParam;
    t_StVppIpHcpSessionCb *pstHcpSessCb;

    vHcpIpTest_TestSetup(&stTestCtx, 0);
    pstHcpSessCb = HCPIPTEST_HCP_TEST_CTX(&stTestCtx)->pstHcpCb;

    stParam.width = 720;
    stParam.height = 480;
    stParam.stride = 768;
    stParam.scanlines = 480;
    stParam.fmt = VPP_COLOR_FORMAT_NV21_VENUS;

    u32 = u32VppIpHcp_Reconfigure((void *)pstHcpSessCb, stParam, stParam);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // Check that values are saved correctly.
    u32 = u32HcpIpTest_ComparePortParams(pstHcpSessCb->stInput.stParam,
                                         stParam);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    u32 = u32HcpIpTest_ComparePortParams(pstHcpSessCb->stOutput.stParam,
                                         stParam);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    vHcpIpTest_TestCleanup(&stTestCtx);
}

TEST(HcpReconfigInvalidProcMode)
{
    /*
     * Call Reconfigure() in invalid conditions, including invalid params and
     * invalid HCP state when Reconfigure() is called.
     */
    uint32_t u32;
    uint32_t u32BufCnt = 10;
    enum vpp_port ePort;
    struct vpp_port_param stParam;
    struct vpp_port_param stParamTemp;
    t_StVppIpHcpSessionCb *pstHcpSessCb;
    struct hcp_test_ctx *pstCtxHcp = HCPIPTEST_HCP_TEST_CTX(&stTestCtx);

    stParam.width = 720;
    stParam.height = 480;
    stParam.stride = 768;
    stParam.scanlines = 480;
    stParam.fmt = VPP_COLOR_FORMAT_NV21_VENUS;

    vHcpIpTest_TestSetupSetParam(&stTestCtx, u32BufCnt);
    pstHcpSessCb = pstCtxHcp->pstHcpCb;

    // Invalid case: call Reconfigure() during INITED state.
    u32 = u32VppIpHcp_Reconfigure(pstHcpSessCb, stParam, stParam);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_STATE);

    // Transition into ACTIVE state for further test cases.
    vHcpIpTest_TestSetup(&stTestCtx, u32BufCnt);
    pstHcpSessCb = pstCtxHcp->pstHcpCb;

    // Invalid case: null session cb pointer.
    u32 = u32VppIpHcp_Reconfigure(NULL, stParam, stParam);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);

    // Invalid case: param width is out of range or invalid.
    stParamTemp = stParam;
    stParamTemp.width = HCP_WIDTH_MIN - HCP_WIDTH_MULTIPLE;
    u32 = u32VppIpHcp_Reconfigure(pstHcpSessCb, stParamTemp, stParamTemp);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);
    stParamTemp = stParam;
    stParamTemp.width = HCP_WIDTH_MAX + HCP_WIDTH_MULTIPLE;
    u32 = u32VppIpHcp_Reconfigure(pstHcpSessCb, stParamTemp, stParamTemp);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);
    stParamTemp = stParam;
    stParamTemp.width = HCP_WIDTH_MIN + 1;
    u32 = u32VppIpHcp_Reconfigure(pstHcpSessCb, stParamTemp, stParamTemp);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);

    // Invalid case: param height is out of range or invalid.
    stParamTemp = stParam;
    stParamTemp.height = HCP_HEIGHT_MIN - HCP_HEIGHT_MULTIPLE;
    u32 = u32VppIpHcp_Reconfigure(pstHcpSessCb, stParamTemp, stParamTemp);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);
    stParamTemp = stParam;
    stParamTemp.height = HCP_HEIGHT_MAX + HCP_HEIGHT_MULTIPLE;
    u32 = u32VppIpHcp_Reconfigure(pstHcpSessCb, stParamTemp, stParamTemp);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);
    stParamTemp = stParam;
    stParamTemp.height = HCP_HEIGHT_MIN + 1;
    u32 = u32VppIpHcp_Reconfigure(pstHcpSessCb, stParamTemp, stParamTemp);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);

    // Invalid case: param stride is out of range or invalid.
    stParamTemp = stParam;
    stParamTemp.stride = HCP_STRIDE_MIN - HCP_STRIDE_MULTIPLE;
    u32 = u32VppIpHcp_Reconfigure(pstHcpSessCb, stParamTemp, stParamTemp);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);
    stParamTemp = stParam;
    stParamTemp.stride = HCP_STRIDE_MAX + HCP_STRIDE_MULTIPLE;
    u32 = u32VppIpHcp_Reconfigure(pstHcpSessCb, stParamTemp, stParamTemp);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);
    stParamTemp = stParam;
    stParamTemp.stride = HCP_STRIDE_MIN + 1;
    u32 = u32VppIpHcp_Reconfigure(pstHcpSessCb, stParamTemp, stParamTemp);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);

    // Invalid case: param scanlines is out of range or invalid.
    stParamTemp = stParam;
    stParamTemp.scanlines = HCP_SCANLINES_MIN - HCP_SCANLINES_MULTIPLE;
    u32 = u32VppIpHcp_Reconfigure(pstHcpSessCb, stParamTemp, stParamTemp);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);
    stParamTemp = stParam;
    stParamTemp.scanlines = HCP_SCANLINES_MAX + HCP_SCANLINES_MULTIPLE;
    u32 = u32VppIpHcp_Reconfigure(pstHcpSessCb, stParamTemp, stParamTemp);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);
    stParamTemp = stParam;
    stParamTemp.scanlines = HCP_SCANLINES_MIN + 1;
    u32 = u32VppIpHcp_Reconfigure(pstHcpSessCb, stParamTemp, stParamTemp);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);

    // Invalid case: param color format is invalid.
    stParamTemp = stParam;
    stParamTemp.fmt = VPP_COLOR_FORMAT_MAX;
    u32 = u32VppIpHcp_Reconfigure(pstHcpSessCb, stParamTemp, stParamTemp);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);

    // Invalid case: the input and output params don't match.
    stParamTemp = stParam;
    stParamTemp.width = 1920;
    u32 = u32VppIpHcp_Reconfigure(pstHcpSessCb, stParam, stParamTemp);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);
    stParamTemp = stParam;
    stParamTemp.height = 1080;
    u32 = u32VppIpHcp_Reconfigure(pstHcpSessCb, stParam, stParamTemp);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);
    stParamTemp = stParam;
    stParamTemp.stride = 1920;
    u32 = u32VppIpHcp_Reconfigure(pstHcpSessCb, stParam, stParamTemp);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);
    stParamTemp = stParam;
    stParamTemp.scanlines = 1088;
    u32 = u32VppIpHcp_Reconfigure(pstHcpSessCb, stParam, stParamTemp);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);
    stParamTemp = stParam;
    stParamTemp.fmt = VPP_COLOR_FORMAT_P010;
    u32 = u32VppIpHcp_Reconfigure(pstHcpSessCb, stParam, stParamTemp);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);

    // Invalid case: reconfigure when HCP is holding input buffers.
    u32 = u32HcpIpTest_QueueBuf(&stTestCtx, VPP_PORT_INPUT,
                                eVppBufType_Progressive);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    u32 = u32VppIpHcp_Reconfigure(pstHcpSessCb, stParam, stParam);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_STATE);
    // Flush input port, so that subsequent test cases don't fail.
    ePort = VPP_PORT_INPUT;
    u32 = u32VppIpHcp_Flush(pstHcpSessCb, ePort);
    vHcpIpTest_WaitForFlushDone(&stTestCtx, ePort);

    // Valid case: reconfigure when HCP is holding an output buffer.
    u32 = u32HcpIpTest_QueueBuf(&stTestCtx, VPP_PORT_OUTPUT,
                                eVppBufType_Progressive);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    u32 = u32VppIpHcp_Reconfigure(pstHcpSessCb, stParam, stParam);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    // Check that HCP has accepted and stored the given values.
    u32 = u32HcpIpTest_ComparePortParams(pstHcpSessCb->stInput.stParam,
                                         stParam);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    u32 = u32HcpIpTest_ComparePortParams(pstHcpSessCb->stOutput.stParam,
                                         stParam);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    vHcpIpTest_TestCleanup(&stTestCtx);
}

TEST(HcpReconfigInvalidMeasMode)
{
    /*
     * Calls Reconfigure() during MEAS mode, and sees when it is invalid.
     */
    uint32_t u32;
    uint32_t u32BufCnt = 0;
    struct vpp_port_param stParamsTemp;
    t_StVppIpHcpSessionCb *pstHcpSessCb;
    struct hcp_test_ctx *pstCtxHcp = HCPIPTEST_HCP_TEST_CTX(&stTestCtx);

    u32 = u32HcpIpTest_SetCtxHqvCtrl(&stTestCtx, HCP_OP_MODE_MEAS);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    vHcpIpTest_TestSetup(&stTestCtx, u32BufCnt);
    pstHcpSessCb = pstCtxHcp->pstHcpCb;

    // Valid case: output params can be garbage.
    stParamsTemp.width = HCP_WIDTH_MAX + 1;
    stParamsTemp.height = HCP_HEIGHT_MAX + 1;
    stParamsTemp.stride = HCP_STRIDE_MAX + 1;
    stParamsTemp.scanlines = HCP_SCANLINES_MAX + 1;
    stParamsTemp.fmt = VPP_COLOR_FORMAT_MAX;
    u32 = u32VppIpHcp_Reconfigure(pstHcpSessCb, pstCtxHcp->stCfg.stPortParam,
                                  stParamsTemp);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // Invalid case: null session pointer.
    u32 = u32VppIpHcp_Reconfigure(NULL, pstCtxHcp->stCfg.stPortParam,
                                  pstCtxHcp->stCfg.stPortParam);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);

    // Invalid case: input width invalid.
    stParamsTemp = pstCtxHcp->stCfg.stPortParam;
    stParamsTemp.width = HCP_WIDTH_MIN - HCP_WIDTH_MULTIPLE;
    u32 = u32VppIpHcp_Reconfigure(pstHcpSessCb, stParamsTemp,
                                  pstCtxHcp->stCfg.stPortParam);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);
    stParamsTemp = pstCtxHcp->stCfg.stPortParam;
    stParamsTemp.width = HCP_WIDTH_MAX + HCP_WIDTH_MULTIPLE;
    u32 = u32VppIpHcp_Reconfigure(pstHcpSessCb, stParamsTemp,
                                  pstCtxHcp->stCfg.stPortParam);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);
    stParamsTemp = pstCtxHcp->stCfg.stPortParam;
    stParamsTemp.width = HCP_WIDTH_MIN + 1;
    u32 = u32VppIpHcp_Reconfigure(pstHcpSessCb, stParamsTemp,
                                  pstCtxHcp->stCfg.stPortParam);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);

    // Invalid case: input height invalid.
    stParamsTemp = pstCtxHcp->stCfg.stPortParam;
    stParamsTemp.height = HCP_HEIGHT_MIN - HCP_HEIGHT_MULTIPLE;
    u32 = u32VppIpHcp_Reconfigure(pstHcpSessCb, stParamsTemp,
                                  pstCtxHcp->stCfg.stPortParam);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);
    stParamsTemp = pstCtxHcp->stCfg.stPortParam;
    stParamsTemp.height = HCP_HEIGHT_MAX + HCP_HEIGHT_MULTIPLE;
    u32 = u32VppIpHcp_Reconfigure(pstHcpSessCb, stParamsTemp,
                                  pstCtxHcp->stCfg.stPortParam);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);
    stParamsTemp = pstCtxHcp->stCfg.stPortParam;
    stParamsTemp.height = HCP_HEIGHT_MIN + 1;
    u32 = u32VppIpHcp_Reconfigure(pstHcpSessCb, stParamsTemp,
                                  pstCtxHcp->stCfg.stPortParam);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);

    // Invalid case: input stride invalid.
    stParamsTemp = pstCtxHcp->stCfg.stPortParam;
    stParamsTemp.stride = HCP_STRIDE_MIN - HCP_STRIDE_MULTIPLE;
    u32 = u32VppIpHcp_Reconfigure(pstHcpSessCb, stParamsTemp,
                                  pstCtxHcp->stCfg.stPortParam);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);
    stParamsTemp = pstCtxHcp->stCfg.stPortParam;
    stParamsTemp.stride = HCP_STRIDE_MAX + HCP_STRIDE_MULTIPLE;
    u32 = u32VppIpHcp_Reconfigure(pstHcpSessCb, stParamsTemp,
                                  pstCtxHcp->stCfg.stPortParam);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);
    stParamsTemp = pstCtxHcp->stCfg.stPortParam;
    stParamsTemp.stride = HCP_STRIDE_MIN + 1;
    u32 = u32VppIpHcp_Reconfigure(pstHcpSessCb, stParamsTemp,
                                  pstCtxHcp->stCfg.stPortParam);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);

    // Invalid case: input scanlines invalid.
    stParamsTemp = pstCtxHcp->stCfg.stPortParam;
    stParamsTemp.scanlines = HCP_SCANLINES_MIN - HCP_SCANLINES_MULTIPLE;
    u32 = u32VppIpHcp_Reconfigure(pstHcpSessCb, stParamsTemp,
                                  pstCtxHcp->stCfg.stPortParam);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);
    stParamsTemp = pstCtxHcp->stCfg.stPortParam;
    stParamsTemp.scanlines = HCP_SCANLINES_MAX + HCP_SCANLINES_MULTIPLE;
    u32 = u32VppIpHcp_Reconfigure(pstHcpSessCb, stParamsTemp,
                                  pstCtxHcp->stCfg.stPortParam);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);
    stParamsTemp = pstCtxHcp->stCfg.stPortParam;
    stParamsTemp.scanlines = HCP_SCANLINES_MIN + 1;
    u32 = u32VppIpHcp_Reconfigure(pstHcpSessCb, stParamsTemp,
                                  pstCtxHcp->stCfg.stPortParam);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);

    // Invalid case: input fmt invalid.
    stParamsTemp = pstCtxHcp->stCfg.stPortParam;
    stParamsTemp.fmt = VPP_COLOR_FORMAT_MAX;
    u32 = u32VppIpHcp_Reconfigure(pstHcpSessCb, stParamsTemp,
                                  pstCtxHcp->stCfg.stPortParam);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);

    vHcpIpTest_TestCleanup(&stTestCtx);
}

TEST(HcpGetBufReqsValid)
{
    /*
     * This test case will be on calling GetBufferRequirements(), to see that
     * its basic functionality is correct.
     */
    uint32_t u32Cnt;
    uint32_t u32CntMax = 10;
    uint32_t u32;
    struct hqv_control stCtrl = {};
    t_StVppIpHcpSessionCb *pstHcpCb;
    t_StVppIpBufReq stInBufReq;
    t_StVppIpBufReq stOutBufReq;
    struct hcp_test_ctx *pstCtxHcp = HCPIPTEST_HCP_TEST_CTX(&stTestCtx);

    // Go to INITED state.
    vHcpIpTest_TestSetupInit(&stTestCtx);
    pstHcpCb = pstCtxHcp->pstHcpCb;

    // Valid: No setCtrl & no setParam.
    u32 = u32VppIpHcp_GetBufferRequirements(pstHcpCb, &stInBufReq,
                                            &stOutBufReq);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    u32 = u32HcpIpTest_ValidateBothBufReqs(&stTestCtx, &stInBufReq,
                                           &stOutBufReq);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    for (u32Cnt = 0; u32Cnt < u32CntMax; u32Cnt++)
    {
        // Valid: Set ctrl to PROC & no setParam.
        u32 = u32HcpIpTest_GetHqvCtrlFromOpMode(HCP_OP_MODE_PROC, &stCtrl);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
        u32 = u32VppIpHcp_SetCtrl(pstHcpCb, stCtrl);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
        u32 = u32VppIpHcp_GetBufferRequirements(pstHcpCb, &stInBufReq,
                                                &stOutBufReq);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
        u32 = u32HcpIpTest_ValidateBothBufReqs(&stTestCtx, &stInBufReq,
                                               &stOutBufReq);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        // Valid: Set ctrl to MEAS & no setParam;
        u32 = u32HcpIpTest_GetHqvCtrlFromOpMode(HCP_OP_MODE_MEAS, &stCtrl);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
        u32 = u32VppIpHcp_SetCtrl(pstHcpCb, stCtrl);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
        u32 = u32VppIpHcp_GetBufferRequirements(pstHcpCb, &stInBufReq,
                                                &stOutBufReq);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
        u32 = u32HcpIpTest_ValidateBothBufReqs(&stTestCtx, &stInBufReq,
                                               &stOutBufReq);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
    }

    // Set port params.
    vHcpIpTest_TestSetupSetParam(&stTestCtx, 0);

    for (u32Cnt = 0; u32Cnt < u32CntMax; u32Cnt++)
    {
        // Valid: Set ctrl to MEAS & setParam;
        u32 = u32HcpIpTest_GetHqvCtrlFromOpMode(HCP_OP_MODE_MEAS, &stCtrl);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
        u32 = u32VppIpHcp_SetCtrl(pstHcpCb, stCtrl);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
        u32 = u32VppIpHcp_GetBufferRequirements(pstHcpCb, &stInBufReq,
                                                &stOutBufReq);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
        u32 = u32HcpIpTest_ValidateBothBufReqs(&stTestCtx, &stInBufReq,
                                               &stOutBufReq);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        // Valid: Set ctrl to PROC & setParam;
        u32 = u32HcpIpTest_GetHqvCtrlFromOpMode(HCP_OP_MODE_PROC, &stCtrl);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
        u32 = u32VppIpHcp_SetCtrl(pstHcpCb, stCtrl);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
        u32 = u32VppIpHcp_GetBufferRequirements(pstHcpCb, &stInBufReq,
                                                &stOutBufReq);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
        u32 = u32HcpIpTest_ValidateBothBufReqs(&stTestCtx, &stInBufReq,
                                               &stOutBufReq);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
    }

    // Go to ACTIVE state.
    vHcpIpTest_TestSetup(&stTestCtx, 0);

    u32 = u32VppIpHcp_GetBufferRequirements(pstHcpCb, &stInBufReq,
                                            &stOutBufReq);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    u32 = u32HcpIpTest_ValidateBothBufReqs(&stTestCtx, &stInBufReq,
                                           &stOutBufReq);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // Cleanup.
    vHcpIpTest_TestCleanup(&stTestCtx);

    // Valid: Go into ACTIVE state without setCtrl.
    vHcpIpTest_TestSetupSetParam(&stTestCtx, 0);
    pstHcpCb = pstCtxHcp->pstHcpCb;

    u32 = u32HcpIpTest_Open(pstHcpCb, HCP_OP_MODE_BYPASS);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = u32VppIpHcp_GetBufferRequirements(pstHcpCb, &stInBufReq,
                                            &stOutBufReq);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    u32 = u32HcpIpTest_ValidateBothBufReqs(&stTestCtx, &stInBufReq,
                                           &stOutBufReq);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = u32VppIpHcp_Close(pstHcpCb);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    vHcpIpTest_TestCleanup(&stTestCtx);
}

TEST(HcpGetBufReqsInvalid)
{
    /*
     * GetBufferRequirements() will be called with invalid parameters.
     */
    uint32_t u32;
    t_StVppIpHcpSessionCb *pstHcpCb;
    t_StVppIpBufReq stInBufReq;
    t_StVppIpBufReq stOutBufReq;

    vHcpIpTest_TestSetupInit(&stTestCtx);
    pstHcpCb = HCPIPTEST_HCP_TEST_CTX(&stTestCtx)->pstHcpCb;

    // Invalid case: null pointers.
    u32 = u32VppIpHcp_GetBufferRequirements(NULL, &stInBufReq, &stOutBufReq);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);

    // Valid case.
    u32 = u32VppIpHcp_GetBufferRequirements(pstHcpCb, NULL, NULL);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // Valid case.
    u32 = u32VppIpHcp_GetBufferRequirements(pstHcpCb, &stInBufReq,
                                            &stOutBufReq);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    u32 = u32HcpIpTest_ValidateBothBufReqs(&stTestCtx, &stInBufReq,
                                           &stOutBufReq);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // Valid case.
    u32 = u32VppIpHcp_GetBufferRequirements(pstHcpCb, NULL, &stOutBufReq);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    u32 = u32HcpIpTest_ValidateBufReqs(&stTestCtx, &stInBufReq, VPP_PORT_INPUT);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // Valid case.
    u32 = u32VppIpHcp_GetBufferRequirements(pstHcpCb, &stInBufReq, NULL);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    u32 = u32HcpIpTest_ValidateBufReqs(&stTestCtx, &stOutBufReq,
                                       VPP_PORT_OUTPUT);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // Cleanup.
    vHcpIpTest_TestCleanup(&stTestCtx);
}

TEST(HcpSwitchOpModesWithKnobs)
{
    /*
     * Open and close session multiple times, switching OP modes, and checking
     * that the knob values are correct. Will queue one input and one output
     * buffer to send the knob values to FW.
     */
    vHcpIpTest_TestKnobsAndOpModes(&stTestCtx, VPP_TRUE);
}

TEST(HcpBufferExchange)
{
    /*
     * Will check successful buffer exchange for all supported color formats,
     * resolutions, frame rates, etc.
     */
    uint32_t u32;
    uint32_t u32ResCnt;
    uint32_t u32FmtCnt;
    uint32_t u32BufCnt = 20;
    struct vpp_port_param stParam;
    uint32_t au32Widths[] =  {HCP_WIDTH_MIN,  426, 640, 720, 1280, 1920, 3840, HCP_WIDTH_MAX};
    uint32_t au32Heights[] = {HCP_HEIGHT_MIN, 240, 360, 480, 720,  1080, 2160, HCP_HEIGHT_MAX};
    uint32_t u32ResLen = sizeof(au32Widths) / sizeof(au32Widths[0]);
    t_EVppIpHcpOpMode eOpMode;

    eOpMode = HCP_OP_MODE_PROC;
    while (eOpMode != HCP_OP_MODE_MAX)
    {
        for (u32FmtCnt = 0; u32FmtCnt < VPP_COLOR_FORMAT_MAX; u32FmtCnt++)
        {
            for (u32ResCnt = 0; u32ResCnt < u32ResLen; u32ResCnt++)
            {
                u32 = u32HcpIpTest_SetPortParams(au32Widths[u32ResCnt],
                                                 au32Heights[u32ResCnt],
                                                 u32FmtCnt, &stParam);
                DVP_ASSERT_EQUAL(u32, VPP_OK);

                // Use of UBWC color formats could cause stride/scanlines to
                // be larger than what HCP can accept.
                if (stParam.stride > u32HcpIpTest_GetMaxStride(stParam.fmt) ||
                    stParam.scanlines > HCP_SCANLINES_MAX)
                {
                    LOGE("resolution too high: width=%u, height=%u, stride=%u, "
                         "scanlines=%u, fmt=%u", stParam.width, stParam.height,
                         stParam.stride, stParam.scanlines, stParam.fmt);
                    continue;
                }

                vHcpIpTest_TestBufferExchange(&stTestCtx, au32Widths[u32ResCnt],
                                              au32Heights[u32ResCnt], u32FmtCnt,
                                              eOpMode, u32BufCnt);

                // Reset buffer counters for next iteration.
                vHcpIpTest_ClearTestCtxBufCnt(&stTestCtx, &stTestCtx.mutex);
            }
        }

#ifdef HCP_TEST_MEAS_MODE
        // Iterate through all resolutions & formats, in both OP modes.
        if (eOpMode == HCP_OP_MODE_PROC)
            eOpMode = HCP_OP_MODE_MEAS;
        else if (eOpMode == HCP_OP_MODE_MEAS)
            eOpMode = HCP_OP_MODE_MAX;
        else
        {
            DVP_ASSERT_FAIL();
            break;
        }
#else
        eOpMode = HCP_OP_MODE_MAX;
#endif
    }
}

#define MID(high, low)  ((high + low)/2)
#define ARR_SZ(_arr) (sizeof(_arr)/sizeof((_arr)[0]))
#define ARR_SAFE_GET(_arr, _idx) ((_idx) < ARR_SZ(_arr) ? _arr[(_idx)] : 0)
#define FOREACH_KNOB(_arr, _val) uint32_t u32Iter##_arr; \
                                 for (u32Iter##_arr = 0, \
                                      (_val) = ARR_SAFE_GET(_arr, u32Iter##_arr); \
                                      (u32Iter##_arr) < ARR_SZ(_arr); \
                                      (u32Iter##_arr)++, \
                                      (_val) = ARR_SAFE_GET(_arr, u32Iter##_arr))

TEST(HcpBufferExchangeKnobs)
{
    /*
     * Run basic buffer exchange for various values of knobs, to make sure
     * all combinations work.
     */
    uint32_t u32;
    uint32_t u32ExpectedInRxBufCnt = 0;
    uint32_t u32ExpectedOutRxBufCnt = 0;
    uint32_t u32BufCnt = 20;
    struct hqv_control stSplitScreenCtrl;
    struct hqv_control stEarCtrl;
    t_StVppIpHcpSessionCb *pstSessCb;
    struct hcp_test_ctx *pstCtxHcp = HCPIPTEST_HCP_TEST_CTX(&stTestCtx);

    uint32_t u32KnobTxStartCnt, u32KnobRxStartCnt;
    uint32_t u32KnobTxEndCnt, u32KnobRxEndCnt;

    // Individual knob values
    uint32_t u32SplitScreen;
    uint32_t u32EarMode;
    uint32_t u32DeGain;
    uint32_t u32AceStrCon, u32AceBriL, u32AceBriH;
    uint32_t u32SatGain, u32SatOff;

    uint32_t au32EarMode[HQV_EAR_MODE_MAX] = {
        HQV_EAR_MODE_OFF,
        HQV_EAR_MODE_BYPASS,
        HQV_EAR_MODE_LOW,
        HQV_EAR_MODE_MEDIUM,
        HQV_EAR_MODE_HIGH,
        HQV_EAR_MODE_STREAM_ADAPTIVE,
        HQV_EAR_MODE_FRAME_ADAPTIVE,
    };

    uint32_t au32DeGain[] = {HCP_DE_GAIN_MIN,
                             MID(HCP_DE_GAIN_MAX, HCP_DE_GAIN_MIN),
                             HCP_DE_GAIN_MAX};
    uint32_t au32SatGain[] = {HCP_SAT_GAIN_MIN,
                              MID(HCP_SAT_GAIN_MAX, HCP_SAT_GAIN_MIN),
                              HCP_SAT_GAIN_MAX};
    uint32_t au32SatOff[] = {HCP_SAT_OFFSET_MIN,
                             MID(HCP_SAT_OFFSET_MIN, HCP_SAT_OFFSET_MAX),
                             HCP_SAT_OFFSET_MIN};
    uint32_t au32AceStrCon[] = {HCP_ACE_STR_MIN,
                                MID(HCP_ACE_STR_MAX, HCP_ACE_STR_MIN),
                                HCP_ACE_STR_MAX};
    uint32_t au32AceBriL[] = {HCP_ACE_BRI_L_MIN,
                              MID(HCP_ACE_BRI_L_MIN, HCP_ACE_BRI_L_MAX),
                              HCP_ACE_BRI_L_MAX};
    uint32_t au32AceBriH[] = {HCP_ACE_BRI_H_MIN,
                              MID(HCP_ACE_BRI_H_MIN, HCP_ACE_BRI_H_MAX),
                              HCP_ACE_BRI_H_MAX};

    uint32_t au32SplitScreen[] = {HCP_SPLIT_MIN,
                                  MID(HCP_SPLIT_MAX, HCP_SPLIT_MIN),
                                  HCP_SPLIT_MAX};

    stSplitScreenCtrl.mode = HQV_MODE_MANUAL;
    stSplitScreenCtrl.ctrl_type = HQV_CONTROL_GLOBAL_DEMO;
    stSplitScreenCtrl.demo.process_direction = HQV_SPLIT_LEFT_TO_RIGHT;

    stEarCtrl.mode = HQV_MODE_MANUAL;
    stEarCtrl.ctrl_type = HQV_CONTROL_EAR;

    u32 = u32HcpIpTest_SetCtxPortParams(&stTestCtx, 720, 480,
                                        VPP_COLOR_FORMAT_NV12_VENUS);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    vHcpIpTest_TestSetupSetParam(&stTestCtx, u32BufCnt);
    pstSessCb = pstCtxHcp->pstHcpCb;

    FOREACH_KNOB(au32SplitScreen, u32SplitScreen) {
    FOREACH_KNOB(au32AceStrCon, u32AceStrCon) {
    FOREACH_KNOB(au32EarMode, u32EarMode) {
    FOREACH_KNOB(au32SatGain, u32SatGain) {
    FOREACH_KNOB(au32DeGain, u32DeGain) {
    FOREACH_KNOB(au32SatOff, u32SatOff) {
    FOREACH_KNOB(au32AceBriL, u32AceBriL) {
    FOREACH_KNOB(au32AceBriH, u32AceBriH) {

        vHcpIpTest_SetParamBoth(&stTestCtx);

        LOGI("BufferExchangeKnobs: ear=%u, sat={gain=%u, off=%u}, "
             "ace={con=%u, bril=%u, brih=%u}, "
             "de_gain=%u, splitscreen=%u",
             u32EarMode, u32SatGain, u32SatOff,
             u32AceStrCon, u32AceBriL, u32AceBriH,
             u32DeGain, u32SplitScreen);

        struct hcp_knobs stKnobs = {};
        stKnobs.u32DeGain = u32DeGain;
        stKnobs.u32SatGain = u32SatGain;
        stKnobs.u32SatOff = u32SatOff;
        stKnobs.u32AceStrCon = u32AceStrCon;
        stKnobs.u32AceBriL = u32AceBriL;
        stKnobs.u32AceBriH = u32AceBriH;

        if (u32SplitScreen == 0 ||
            (bEarWouldBypass(u32EarMode) && bAieWouldBypass(stKnobs)))
        {
            pstCtxHcp->u32NumExpectedBypassBufs = u32BufCnt;
        }

        // Split screen.
        stSplitScreenCtrl.demo.process_percent = u32SplitScreen;
        u32 = u32VppIpHcp_SetCtrl(pstSessCb, stSplitScreenCtrl);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        // EAR
        stEarCtrl.ear.mode = u32EarMode;
        u32 = u32VppIpHcp_SetCtrl(pstSessCb, stEarCtrl);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        // AIE
        pthread_mutex_lock(&stTestCtx.mutex);
        pstCtxHcp->stCfg.stHqvCtrl.mode = HQV_MODE_MANUAL;
        pstCtxHcp->stCfg.stHqvCtrl.ctrl_type = HQV_CONTROL_AIE;
        pstCtxHcp->stCfg.stHqvCtrl.aie.mode = HQV_MODE_MANUAL;
        pstCtxHcp->stCfg.stHqvCtrl.aie.ltm_sat_gain = u32SatGain;
        pstCtxHcp->stCfg.stHqvCtrl.aie.ltm_sat_offset = u32SatOff;
        pstCtxHcp->stCfg.stHqvCtrl.aie.ltm_ace_str = u32AceStrCon;
        pstCtxHcp->stCfg.stHqvCtrl.aie.ltm_ace_bright_l = u32AceBriL;
        pstCtxHcp->stCfg.stHqvCtrl.aie.ltm_ace_bright_h = u32AceBriH;
        pstCtxHcp->stCfg.stHqvCtrl.aie.cade_level = u32DeGain;
        pthread_mutex_unlock(&stTestCtx.mutex);

        vHcpIpTest_TestSetup(&stTestCtx, u32BufCnt);

        // Knobs are sent when Open() is called, so by this point
        // if we are in active state, we can call SetCtrl()
        // again. By the end of the test, the knobs should have
        // been sent to firmware.
        u32KnobTxStartCnt = HCP_STAT_GET(pstSessCb, FwTxPropSetKnob);
        u32KnobRxStartCnt = HCP_STAT_GET(pstSessCb, FwRxPropSetKnob);

        u32 = u32VppIpHcp_SetCtrl(pstSessCb,
                                  pstCtxHcp->stCfg.stHqvCtrl);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        u32 = u32HcpIpTest_QueueBufBothPorts(&stTestCtx,
                                             eVppBufType_Progressive,
                                             u32BufCnt);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        u32ExpectedInRxBufCnt = u32BufCnt;
        u32ExpectedOutRxBufCnt = u32BufCnt;
        if (pstCtxHcp->u32NumExpectedBypassBufs)
        {
            u32ExpectedInRxBufCnt = u32BufCnt -
                pstCtxHcp->u32NumExpectedBypassBufs;
            u32ExpectedOutRxBufCnt = u32BufCnt +
                pstCtxHcp->u32NumExpectedBypassBufs;
            vHcpIpTest_WaitForAllInputBuffers(&stTestCtx,
                                              stTestCtx.pPrivateCtx);
            u32 = u32VppIpHcp_Flush(pstSessCb, VPP_PORT_OUTPUT);
            DVP_ASSERT_EQUAL(u32, VPP_OK);
            vHcpIpTest_WaitForFlushDone(&stTestCtx, VPP_PORT_OUTPUT);
        }
        else
        {
            vHcpIpTest_WaitForAllBuffers(&stTestCtx,
                                         stTestCtx.pPrivateCtx);
        }

        DVP_ASSERT_EQUAL(stTestCtx.u32InRxCnt, u32ExpectedInRxBufCnt);
        DVP_ASSERT_EQUAL(stTestCtx.u32OutRxCnt, u32ExpectedOutRxBufCnt);

        u32KnobTxEndCnt = HCP_STAT_GET(pstSessCb, FwTxPropSetKnob);
        u32KnobRxEndCnt = HCP_STAT_GET(pstSessCb, FwRxPropSetKnob);


        if (!pstCtxHcp->u32NumExpectedBypassBufs)
        {
            DVP_ASSERT_TRUE(u32KnobTxEndCnt > u32KnobTxStartCnt);
            DVP_ASSERT_TRUE(u32KnobRxEndCnt > u32KnobRxStartCnt);
        }

        LOGI("u32KnobTxStartCnt=%u, u32KnobTxEndCnt=%u",
             u32KnobTxStartCnt, u32KnobTxEndCnt);
        LOGI("u32KnobRxStartCnt=%u, u32KnobRxEndCnt=%u",
             u32KnobRxStartCnt, u32KnobRxEndCnt);

        vHcpIpTest_TestCleanupClose(&stTestCtx);

        // Reset buffer counters for next iteration.
        vHcpIpTest_ClearTestCtxBufCnt(&stTestCtx,
                                      &stTestCtx.mutex);
    } } } } } } } }
    vHcpIpTest_TestCleanup(&stTestCtx);
}

TEST(HcpConcurrencyTwoProcModeSess)
{
    /*
     * Try two concurrent sessions in PROC mode, and see if buffer exchanges
     * work.
     */
    uint32_t u32;
    uint32_t u32Cnt;
    t_StHcpIpTestSessCfg astCfg[2] = {};

    u32 = u32VppIpHcp_Boot();
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    for (u32Cnt = 0; u32Cnt < sizeof(astCfg) / sizeof(astCfg[0]); u32Cnt++)
    {
        u32 = u32HcpIpTest_GetHqvCtrlFromOpMode(HCP_OP_MODE_PROC,
                                                &astCfg[u32Cnt].stHqvCtrl);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
        u32 = u32HcpIpTest_SetPortParams(1920, 1080, VPP_COLOR_FORMAT_NV12_VENUS,
                                         &astCfg[u32Cnt].stPortParam);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
        astCfg[u32Cnt].bCpStatus = VPP_FALSE;
    }

    vHcpIpTest_TestConcurrency(&astCfg[0], sizeof(astCfg)/sizeof(astCfg[0]));

    u32 = u32VppIpHcp_Shutdown();
    DVP_ASSERT_EQUAL(u32, VPP_OK);
}

TEST(HcpConcurrencyTwoProcModeSessNoBoot)
{
    /*
     * Try two concurrent sessions in PROC mode, without calling Boot(). Will
     * only do queueing buffers, and see that there is successful buffer
     * exchange on both sessions.
     */
    uint32_t u32Cnt;
    uint32_t u32;
    t_StHcpIpTestSessCfg astCfg[2] = {};

    for (u32Cnt = 0; u32Cnt < sizeof(astCfg) / sizeof(astCfg[0]); u32Cnt++)
    {
        u32 = u32HcpIpTest_GetHqvCtrlFromOpMode(HCP_OP_MODE_PROC,
                                                &astCfg[u32Cnt].stHqvCtrl);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
        u32 = u32HcpIpTest_SetPortParams(1920, 1080, VPP_COLOR_FORMAT_NV12_VENUS,
                                         &astCfg[u32Cnt].stPortParam);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
        astCfg[u32Cnt].bCpStatus = VPP_FALSE;
    }

    vHcpIpTest_TestConcurrency(&astCfg[0], sizeof(astCfg)/sizeof(astCfg[0]));
}

TEST(__unused HcpConcurrencyTwoMeasModeSess)
{
    /*
     * This tests concurrency between two MEAS mode sessions. Will queue some
     * buffers and see that both sessions exchange buffers successfully.
     */
    uint32_t u32Cnt;
    uint32_t u32;
    t_StHcpIpTestSessCfg astCfg[2] = {};

    for (u32Cnt = 0; u32Cnt < sizeof(astCfg) / sizeof(astCfg[0]); u32Cnt++)
    {
        u32 = u32HcpIpTest_GetHqvCtrlFromOpMode(HCP_OP_MODE_MEAS,
                                                &astCfg[u32Cnt].stHqvCtrl);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
        u32 = u32HcpIpTest_SetPortParams(1920, 1080, VPP_COLOR_FORMAT_NV12_VENUS,
                                         &astCfg[u32Cnt].stPortParam);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
        astCfg[u32Cnt].bCpStatus = VPP_FALSE;
    }

    vHcpIpTest_TestConcurrency(&astCfg[0], sizeof(astCfg)/sizeof(astCfg[0]));
}

TEST(__unused HcpConcurrencyProcMeasModeSess)
{
    /*
     * This test will test end-to-end concurrency of having a PROC mode
     * session running with a MEAS mode session, both concurrently having
     * buffer exchange occurring.
     */
    uint32_t u32Cnt;
    uint32_t u32;
    t_StHcpIpTestSessCfg astCfg[2] = {};

    u32 = u32HcpIpTest_GetHqvCtrlFromOpMode(HCP_OP_MODE_PROC,
                                            &astCfg[0].stHqvCtrl);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = u32HcpIpTest_GetHqvCtrlFromOpMode(HCP_OP_MODE_MEAS,
                                            &astCfg[1].stHqvCtrl);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    for (u32Cnt = 0; u32Cnt < sizeof(astCfg) / sizeof(astCfg[0]); u32Cnt++)
    {
        u32 = u32HcpIpTest_SetPortParams(1920, 1080, VPP_COLOR_FORMAT_NV12_VENUS,
                                         &astCfg[u32Cnt].stPortParam);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
        astCfg[u32Cnt].bCpStatus = VPP_FALSE;
    }

    vHcpIpTest_TestConcurrency(&astCfg[0], sizeof(astCfg)/sizeof(astCfg[0]));
}

TEST(HcpConcurrencyCpNoCpProcSess)
{
    /*
     * This test will have two concurrent sessions, one with CPZ enabled and
     * one without. Will check that buffer exchange for both sessions are
     * successful.
     */
    uint32_t u32Cnt;
    uint32_t u32;
    t_StHcpIpTestSessCfg astCfg[2] = {};

    astCfg[0].bCpStatus = VPP_FALSE;
    astCfg[1].bCpStatus = VPP_TRUE;

    for (u32Cnt = 0; u32Cnt < sizeof(astCfg) / sizeof(astCfg[0]); u32Cnt++)
    {

        u32 = u32HcpIpTest_GetHqvCtrlFromOpMode(HCP_OP_MODE_PROC,
                                                &astCfg[u32Cnt].stHqvCtrl);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        u32 = u32HcpIpTest_SetPortParams(1920, 1080, VPP_COLOR_FORMAT_NV12_VENUS,
                                         &astCfg[u32Cnt].stPortParam);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
    }

    vHcpIpTest_TestConcurrency(&astCfg[0], sizeof(astCfg)/sizeof(astCfg[0]));
}

TEST(HcpConcurrencyEightProcSess)
{
    /*
     * This test will have eight concurrent sessions that are all sucessfully
     * exchanging buffers. To minimize cost per session, each session will have
     * a 32x32 resolution, NV12 color format, in PROC mode, using only EAR mode
     * at level 1.
     */
    uint32_t u32Cnt;
    uint32_t u32;
    t_StHcpIpTestSessCfg astCfg[8] = {};

    for (u32Cnt = 0; u32Cnt < sizeof(astCfg) / sizeof(astCfg[0]); u32Cnt++)
    {
        astCfg[u32Cnt].stHqvCtrl.mode = HQV_MODE_MANUAL;
        astCfg[u32Cnt].stHqvCtrl.ctrl_type = HQV_CONTROL_EAR;
        astCfg[u32Cnt].stHqvCtrl.ear.mode = HQV_EAR_MODE_LOW;

        u32 = u32HcpIpTest_SetPortParams(32, 32, VPP_COLOR_FORMAT_NV12_VENUS,
                                         &astCfg[u32Cnt].stPortParam);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        astCfg[u32Cnt].bCpStatus = VPP_FALSE;
    }

    vHcpIpTest_TestConcurrency(&astCfg[0], sizeof(astCfg)/sizeof(astCfg[0]));
}

/************************************************************************
 * Global Functions
 ***********************************************************************/
TEST_CASES HcpTests[] = {
    TEST_CASE(HcpBootShutdown),
    TEST_CASE(HcpBootShutdownBackToBack),
    TEST_CASE(HcpRepeatedBoot),
    TEST_CASE(HcpRepeatedShutdown),
    TEST_CASE(HcpInitTermNoBoot),
    TEST_CASE(HcpInitToMax),
    TEST_CASE(HcpBasicControlApi),
    TEST_CASE(HcpSetCtrlChangesOpMode),
    TEST_CASE(HcpOpenWithoutSetCtrl),
    TEST_CASE(HcpSetParam),
    TEST_CASE(HcpSetParamMaxStridePerColorFormat),
    TEST_CASE(HcpSetCtrlValidity),
    TEST_CASE(HcpResetCtrl),
    TEST_CASE(HcpSwitchOpModes),
    TEST_CASE(HcpSetProcCtrlDuringActive),
    TEST_CASE(HcpQueueBufBasicTest),
    TEST_CASE(HcpQueueBufInvalid),
    TEST_CASE(HcpQueueBufBasicBypass),
    TEST_CASE(HcpFlushBasic),
    TEST_CASE(HcpFlushInvalid),
    TEST_CASE(HcpFlushMultiple),
    TEST_CASE(HcpDrainBasic),
    TEST_CASE(HcpDrainInvalid),
    TEST_CASE(HcpDrainMultiple),
    TEST_CASE(HcpReconfigBasic),
    TEST_CASE(HcpReconfigInvalidProcMode),
    TEST_CASE(HcpReconfigInvalidMeasMode),
    TEST_CASE(HcpGetBufReqsValid),
    TEST_CASE(HcpGetBufReqsInvalid),
    TEST_CASE(HcpSwitchOpModesWithKnobs),
    TEST_CASE(HcpBufferExchange),
    TEST_CASE(HcpBufferExchangeKnobs),
    TEST_CASE(HcpConcurrencyTwoProcModeSess),
    TEST_CASE(HcpConcurrencyTwoProcModeSessNoBoot),
    // TEST_CASE(HcpConcurrencyTwoMeasModeSess),
    // TEST_CASE(HcpConcurrencyProcMeasModeSess),
    TEST_CASE(HcpConcurrencyCpNoCpProcSess),
    TEST_CASE(HcpConcurrencyEightProcSess),
    TEST_CASE_NULL(),
};

TEST_SUITE(HcpSuite,
           "HcpTests",
           HcpSuiteInit,
           HcpSuiteTerm,
           HcpTestInit,
           HcpTestTerm,
           HcpTests);
