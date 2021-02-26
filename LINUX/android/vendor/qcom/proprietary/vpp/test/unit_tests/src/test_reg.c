/*!
 * @file test_reg.c
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

#include "dvpTest.h"
#include "dvpTest_tb.h"

#define VPP_LOG_TAG     VPP_LOG_UT_IP_TAG
#define VPP_LOG_MODULE  VPP_LOG_UT_IP
#include "vpp_dbg.h"
#include "vpp.h"
#include "vpp_ctx.h"
#include "vpp_utils.h"

#include "test_utils.h"
#include "buf_pool.h"

/************************************************************************
 * Local definitions
 ***********************************************************************/
struct ip_test_ctx
{
    // Used in get_filled_buf(), for an exception case for 8996's HVX.
    uint32_t bChangeBufferMapping;

    uint32_t bBypassExpected;
    uint32_t u32FlushDoneInCnt;
    uint32_t u32FlushDoneOutCnt;
    uint32_t u32DrainDoneCnt;
    uint32_t u32ExpExtraLen;

    struct ts_mgr stTsMgr;
};

typedef void (*test_fnc_cb)(const t_StVppIpBlock *pstIp);
#define TEST_CB_FNC(name) name##_cb
#define TEST_CB_DECL(name, _pip) static void name##_cb(const t_StVppIpBlock *_pip)

/************************************************************************
 * Local static variables
 ***********************************************************************/
static struct test_ctx stTestCtx;
static struct ip_test_ctx stIpTestCtx;

/************************************************************************
 * Forward Declarations
 ************************************************************************/
typedef void (*assert_fnc)(int, uint32_t, const char *, const char *, const char *);
extern uint32_t u32HcpMock_EnableErrorAction(assert_fnc pFunc,
                                              uint32_t u32Hang,
                                              uint32_t bEnable);
#pragma weak u32HcpMock_EnableErrorAction

/************************************************************************
 * Local Functions
 ***********************************************************************/
static void test_reg_ibd(void *pv, t_StVppBuf *pstBuf)
{
    struct test_ctx *pstTestCtx;
    struct bufnode *pstNode;
    uint32_t u32Ret;

    if (!pv || !pstBuf)
    {
        DVP_ASSERT_FAIL();
        return;
    }

    pstTestCtx = (struct test_ctx *)pv;
    pstNode = pstBuf->pBuf->cookie;

    LOGI("%s called.", __func__);

    DVP_ASSERT_EQUAL(pstNode->owner, BUF_OWNER_LIBRARY);
    pstNode->owner = BUF_OWNER_CLIENT;

    DVP_ASSERT_EQUAL(pstNode->port_owner, VPP_PORT_INPUT);

    pthread_mutex_lock(&pstTestCtx->mutex);

    pstTestCtx->u32InRxCnt++;
    pstTestCtx->u32InHoldCnt--;

    if (pstNode->pIntBuf->u32InternalFlags & VPP_BUF_FLAG_FLUSHED)
        pstTestCtx->u32InFlushCnt++;

    u32Ret = restore_bufnode_internal_buf(pstNode);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    put_buf(pstNode->pPool, pstNode);

    pthread_cond_signal(&pstTestCtx->cond);
    pthread_mutex_unlock(&pstTestCtx->mutex);
}

static void test_reg_obd(void *pv, t_StVppBuf *pstBuf)
{
    struct test_ctx *pstTestCtx;
    struct ip_test_ctx *pstIpTestCtx;
    struct bufnode *pstNode;
    uint32_t u32Ret;

    LOGI("%s called.", __func__);

    if (!pv || !pstBuf)
    {
        DVP_ASSERT_FAIL();
        return;
    }

    pstTestCtx = (struct test_ctx *)pv;
    pstIpTestCtx = (struct ip_test_ctx *)pstTestCtx->pPrivateCtx;
    pstNode = pstBuf->pBuf->cookie;

    DVP_ASSERT_EQUAL(pstNode->owner, BUF_OWNER_LIBRARY);
    pstNode->owner = BUF_OWNER_CLIENT;

    pthread_mutex_lock(&pstTestCtx->mutex);

    if (!pstIpTestCtx->bBypassExpected)
        DVP_ASSERT_EQUAL(pstNode->port_owner, VPP_PORT_OUTPUT);

    pstTestCtx->u32OutRxCnt++;
    if (pstNode->port_owner == VPP_PORT_INPUT)
        pstTestCtx->u32InHoldCnt--;
    else if (pstNode->port_owner == VPP_PORT_OUTPUT)
        pstTestCtx->u32OutHoldCnt--;
    else
        DVP_ASSERT_FAIL();

    if (pstNode->pIntBuf->u32InternalFlags & VPP_BUF_FLAG_FLUSHED &&
        pstNode->pIntBuf->stPixel.u32FilledLen == 0)
        pstTestCtx->u32OutFlushCnt++;

    if (pstNode->pIntBuf->stPixel.u32FilledLen)
    {
        dump_buf(pstNode);
        ts_put_ts(&pstIpTestCtx->stTsMgr, pstNode->pExtBuf->timestamp);
        DVP_ASSERT_EQUAL(pstNode->pIntBuf->stExtra.u32FilledLen,
                         pstIpTestCtx->u32ExpExtraLen);
        LOGI("extradata: expFillLen=%u, act_fill_len=%u",
             pstIpTestCtx->u32ExpExtraLen,
             pstNode->pIntBuf->stExtra.u32FilledLen);
        validate_extradata_integrity(pstNode);
    }

    u32Ret = restore_bufnode_internal_buf(pstNode);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    put_buf(pstNode->pPool, pstNode);

    pthread_cond_signal(&pstTestCtx->cond);
    pthread_mutex_unlock(&pstTestCtx->mutex);
}

static void test_reg_evt(void *pv, t_StVppEvt stEvt)
{
    struct test_ctx *pstTestCtx;
    struct ip_test_ctx *pstIpTestCtx;

    LOGI("%s called.", __func__);

    pstTestCtx = (struct test_ctx *)pv;
    pstIpTestCtx = (struct ip_test_ctx *)pstTestCtx->pPrivateCtx;

    pthread_mutex_lock(&pstTestCtx->mutex);

    switch (stEvt.eType)
    {
        case VPP_EVT_FLUSH_DONE:
            if (stEvt.flush.ePort == VPP_PORT_INPUT)
                pstIpTestCtx->u32FlushDoneInCnt++;

            else if (stEvt.flush.ePort == VPP_PORT_OUTPUT)
                pstIpTestCtx->u32FlushDoneOutCnt++;

            else
                DVP_ASSERT_FAIL();
            break;
        case VPP_EVT_DRAIN_DONE:
            pstIpTestCtx->u32DrainDoneCnt++;
            break;
        default:
            DVP_ASSERT_FAIL();
            break;
    }

    LOGI("%s: flush_done_in=%u, flush_done_out=%u, drain_done=%u", __func__,
         pstIpTestCtx->u32FlushDoneInCnt, pstIpTestCtx->u32FlushDoneOutCnt,
         pstIpTestCtx->u32DrainDoneCnt);

    pthread_cond_signal(&pstTestCtx->cond);
    pthread_mutex_unlock(&pstTestCtx->mutex);
}

static void prep_port(struct vpp_port_param *pstPort,
                      t_EVppRes eRes,
                      enum vpp_color_format eFmt)
{
    if (!pstPort)
        return;
    pstPort->fmt = eFmt;
    pstPort->width = u32GetMaxWidthForRes(eRes);
    pstPort->height = u32GetMaxHeightForRes(eRes);
    pstPort->stride = u32VppUtils_CalculateStride(pstPort->width, pstPort->fmt);
    pstPort->scanlines = u32VppUtils_CalculateScanlines(pstPort->height, pstPort->fmt);
}

static enum vpp_color_format get_supported_fmt(const t_StVppIpBlock *pstIp)
{
    if (!pstIp)
        return VPP_COLOR_FORMAT_NV12_VENUS;

    // Return the first supported color format in the IP block
    return pstIp->stInputPort.peColorFmts[0];
}

static void init_vpp(t_StVppCtx *pstCtx)
{
    uint32_t u32;

    u32 = u32VppIon_Init(pstCtx);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
}

static void term_vpp(t_StVppCtx *pstCtx)
{
    uint32_t u32;

    u32 = u32VppIon_Term(pstCtx);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
}

static void *send_init(const t_StVppIpFnc *pstFnc,
                       void *pCtx,
                       t_StVppCtx *pstCtx,
                       uint32_t u32Flags,
                       uint32_t bSendIbd,
                       uint32_t bSendObd,
                       uint32_t bSendEvt)
{
    if (!pstFnc)
    {
        DVP_ASSERT_FAIL();
        return NULL;
    }

    t_StVppCallback cbs;
    memset(&cbs, 0, sizeof(cbs));

    cbs.pv = pCtx;

    if (bSendIbd)
        cbs.input_buffer_done = test_reg_ibd;

    if (bSendObd)
        cbs.output_buffer_done = test_reg_obd;

    if (bSendEvt)
        cbs.event = test_reg_evt;

    return pstFnc->init(pstCtx, u32Flags, cbs);
}

static void send_term(const t_StVppIpFnc *pstFnc, void *ctx)
{
    if (!pstFnc)
    {
        DVP_ASSERT_FAIL();
        return;
    }

    pstFnc->term(ctx);
}

static uint32_t send_open(const t_StVppIpFnc *pstFnc, void *ctx)
{
    if (!pstFnc)
    {
        DVP_ASSERT_FAIL();
        return VPP_ERR_PARAM;
    }

    return pstFnc->open(ctx);
}

static uint32_t send_close(const t_StVppIpFnc *pstFnc, void *ctx)
{
    if (!pstFnc)
    {
        DVP_ASSERT_FAIL();
        return VPP_ERR_PARAM;
    }

    return pstFnc->close(ctx);
}

static uint32_t send_set_ctrl(const t_StVppIpFnc *pstFnc, void *ctx,
                              struct hqv_control ctrl)
{
    if (!pstFnc)
    {
        DVP_ASSERT_FAIL();
        return VPP_ERR_PARAM;
    }

    return pstFnc->set_ctrl(ctx, ctrl);
}

static uint32_t send_reset_ctrl(const t_StVppIpFnc *pstFnc, void *ctx)
{
    if (!pstFnc || !pstFnc->reset_ctrl)
    {
        DVP_ASSERT_FAIL();
        return VPP_ERR_PARAM;
    }

    return pstFnc->reset_ctrl(ctx);
}

static uint32_t send_set_param(const t_StVppIpFnc *pstFnc, void *ctx,
                               enum vpp_port port, struct vpp_port_param param)
{
    if (!pstFnc)
    {
        DVP_ASSERT_FAIL();
        return VPP_ERR_PARAM;
    }

    return pstFnc->set_param(ctx, port, param);
}

static uint32_t send_get_buffer_requirements(const t_StVppIpFnc *pstFnc,
                                             void *ctx,
                                             t_StVppIpBufReq *in_buf_req,
                                             t_StVppIpBufReq *out_buf_req)
{
    if (!pstFnc)
    {
        DVP_ASSERT_FAIL();
        return VPP_ERR_PARAM;
    }

    return pstFnc->get_buffer_requirements(ctx, in_buf_req, out_buf_req);
}

static uint32_t send_queue_buf(const t_StVppIpFnc *pstFnc, void *ctx,
                               enum vpp_port port, t_StVppBuf *buf)
{
    if (!pstFnc)
    {
        DVP_ASSERT_FAIL();
        return VPP_ERR_PARAM;
    }

    return pstFnc->queue_buf(ctx, port, buf);
}

static uint32_t send_flush(const t_StVppIpFnc *pstFnc, void *ctx,
                           enum vpp_port port)
{
    if (!pstFnc)
    {
        DVP_ASSERT_FAIL();
        return VPP_ERR_PARAM;
    }

    return pstFnc->flush(ctx, port);
}

static uint32_t send_drain(const t_StVppIpFnc *pstFnc, void *ctx)
{
    if (!pstFnc)
    {
        DVP_ASSERT_FAIL();
        return VPP_ERR_PARAM;
    }

    return pstFnc->drain(ctx);
}

static uint32_t send_reconfigure(const t_StVppIpFnc *pstFnc, void *ctx,
                                 struct vpp_port_param in_param,
                                 struct vpp_port_param out_param)
{
    if (!pstFnc || !pstFnc->reconfigure)
    {
        DVP_ASSERT_FAIL();
        return VPP_ERR_PARAM;
    }

    return pstFnc->reconfigure(ctx, in_param, out_param);
}

static struct bufnode *get_filled_buf(struct test_ctx *pstTestCtx,
                                      enum vpp_port ePort,
                                      t_EVppBufType eBufType,
                                      uint32_t u32ExtFlags,
                                      uint32_t u32IntFlags,
                                      pthread_mutex_t *pCtxMutex)
{
    struct bufnode *pstNode;
    struct ip_test_ctx *pstIpTestCtx;

    if (!pstTestCtx)
    {
        DVP_ASSERT_FAIL();
        return NULL;
    }

    pstIpTestCtx = (struct ip_test_ctx *)pstTestCtx->pPrivateCtx;

    if (pCtxMutex)
        pthread_mutex_lock(pCtxMutex);

    while (NULL == (pstNode = get_buf(pstTestCtx->buf_pool)))
        pthread_cond_wait(&pstTestCtx->cond, &pstTestCtx->mutex);

    if (pCtxMutex)
        pthread_mutex_unlock(pCtxMutex);

    if (ePort == VPP_PORT_INPUT)
    {
        fill_buf(pstNode);
        pstNode->pIntBuf->eBufType = eBufType;
        pstNode->pIntBuf->u32InternalFlags = u32IntFlags;
        pstNode->pIntBuf->u32OperatingRate = ts_get_freq(&pstIpTestCtx->stTsMgr);
        pstNode->pIntBuf->u32TimestampFrameRate = pstNode->pIntBuf->u32OperatingRate;
        pstNode->pExtBuf->flags = u32ExtFlags;
        pstNode->pExtBuf->timestamp = ts_get_next(&pstIpTestCtx->stTsMgr);

        fill_extra_data(pstNode, eBufType, 0);
        pstIpTestCtx->u32ExpExtraLen = pstNode->pIntBuf->stExtra.u32FilledLen;
    }

    // For HVX on 8996 since it requires a mapped pixel buffer.
    if (pstIpTestCtx->bChangeBufferMapping &&
        (pstTestCtx->buf_pool->params.eProtection == PROTECTION_ZONE_NONSECURE))
        pstNode->pIntBuf->stPixel.eMapping = eVppBuf_MappedInternal;
    else
        pstNode->pIntBuf->stPixel.eMapping= eVppBuf_Unmapped;

    pstNode->owner = BUF_OWNER_LIBRARY;
    pstNode->port_owner = ePort;
    pstNode->pIntBuf->eQueuedPort = pstNode->port_owner;
    pstNode->pIntBuf->eCurrentIpPort = pstNode->port_owner;

    return pstNode;
}

static uint32_t get_and_send_buf(const t_StVppIpFnc *pstFnc,
                                 struct test_ctx *pstTestCtx, void *pv,
                                 enum vpp_port ePort, t_EVppBufType eBufType,
                                 uint32_t u32ExtFlags, uint32_t u32IntFlags,
                                 pthread_mutex_t *pCtxMutex)
{
    struct bufnode *pstNode;

    if (!pstTestCtx)
    {
        DVP_ASSERT_FAIL();
        return VPP_ERR_PARAM;
    }

    pstNode = get_filled_buf(pstTestCtx, ePort, eBufType, u32ExtFlags,
                             u32IntFlags, pCtxMutex);
    DVP_ASSERT_PTR_NNULL(pstNode);

    if (pCtxMutex)
        pthread_mutex_lock(pCtxMutex);

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

    if (pCtxMutex)
        pthread_mutex_unlock(pCtxMutex);

    return send_queue_buf(pstFnc, pv, ePort, pstNode->pIntBuf);
}

static void wait_for_buffers(struct test_ctx *pstTestCtx, enum vpp_port ePort)
{
    if (!pstTestCtx || ePort >= VPP_PORT_MAX)
    {
        DVP_ASSERT_FAIL();
        return;
    }

    pthread_mutex_lock(&pstTestCtx->mutex);

    if (ePort == VPP_PORT_INPUT)
    {
        while (pstTestCtx->u32InHoldCnt != 0)
            pthread_cond_wait(&pstTestCtx->cond, &pstTestCtx->mutex);
        DVP_ASSERT_EQUAL(pstTestCtx->u32InHoldCnt, 0);
    }
    else
    {
        while (pstTestCtx->u32OutHoldCnt != 0)
            pthread_cond_wait(&pstTestCtx->cond, &pstTestCtx->mutex);
        DVP_ASSERT_EQUAL(pstTestCtx->u32OutHoldCnt, 0);
    }

    pthread_mutex_unlock(&pstTestCtx->mutex);
}

static void wait_for_flush_done(struct test_ctx *pstTestCtx,
                                enum vpp_port ePort, uint32_t u32FlushCnt)
{
    struct ip_test_ctx *pstIpTestCtx;

    if (!pstTestCtx || ePort >= VPP_PORT_MAX)
    {
        DVP_ASSERT_FAIL();
        return;
    }

    pstIpTestCtx = (struct ip_test_ctx *)pstTestCtx->pPrivateCtx;

    pthread_mutex_lock(&pstTestCtx->mutex);
    if (ePort == VPP_PORT_INPUT)
    {
        while (pstIpTestCtx->u32FlushDoneInCnt != u32FlushCnt)
            pthread_cond_wait(&pstTestCtx->cond, &pstTestCtx->mutex);
    }
    else
    {
        while (pstIpTestCtx->u32FlushDoneOutCnt != u32FlushCnt)
            pthread_cond_wait(&pstTestCtx->cond, &pstTestCtx->mutex);
    }
    pthread_mutex_unlock(&pstTestCtx->mutex);
}

static void wait_for_drain_done(struct test_ctx *pstTestCtx,
                                uint32_t u32DrainCnt)
{
    struct ip_test_ctx *pstIpTestCtx;

    if (!pstTestCtx)
    {
        DVP_ASSERT_FAIL();
        return;
    }

    pstIpTestCtx = (struct ip_test_ctx *)pstTestCtx->pPrivateCtx;

    pthread_mutex_lock(&pstTestCtx->mutex);
    while (pstIpTestCtx->u32DrainDoneCnt != u32DrainCnt)
        pthread_cond_wait(&pstTestCtx->cond, &pstTestCtx->mutex);
    pthread_mutex_unlock(&pstTestCtx->mutex);
}

static uint32_t init_tctx_buf_pool(struct test_ctx *pstTestCtx,
                                   uint32_t u32BufPoolSize,
                                   uint32_t bInitIon)
{
    uint32_t u32Ret1;
    uint32_t u32Ret2;

    if (!pstTestCtx)
    {
        DVP_ASSERT_FAIL();
        return VPP_ERR_PARAM;
    }

    u32Ret1 = buf_params_init_default(&pstTestCtx->params,
                                      &pstTestCtx->port_param);
    DVP_ASSERT_EQUAL(u32Ret1, VPP_OK);

    u32Ret2 = init_buf_pool(&pstTestCtx->stVppCtx, &pstTestCtx->buf_pool,
                            &pstTestCtx->params, u32BufPoolSize, bInitIon);
    DVP_ASSERT_EQUAL(u32Ret2, VPP_OK);

    return u32Ret1 | u32Ret2;
}

static uint32_t free_tctx_buf_pool(struct test_ctx *pstTestCtx,
                                   uint32_t bInitIon)
{
    if (!pstTestCtx)
    {
        DVP_ASSERT_FAIL();
        return VPP_ERR_PARAM;
    }

    free_buf_pool(pstTestCtx->buf_pool, bInitIon);

    return VPP_OK;
}

static void move_from_inited_to_active(const t_StVppIpFnc *pstFnc,
                                       void *pvCtx,
                                       struct hqv_control ctrl,
                                       struct vpp_port_param params)
{
    uint32_t u32;

    if (!pstFnc || !pvCtx)
    {
        DVP_ASSERT_FAIL();
        return;
    }

    u32 = send_set_ctrl(pstFnc, pvCtx, ctrl);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = send_set_param(pstFnc, pvCtx, VPP_PORT_INPUT, params);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = send_set_param(pstFnc, pvCtx, VPP_PORT_OUTPUT, params);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = send_open(pstFnc, pvCtx);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
}

static void *init_test(const t_StVppIpFnc *pstFnc,
                       struct test_ctx *pstTestCtx,
                       struct hqv_control ctrl,
                       struct vpp_port_param params)
{
    void *pv;

    if (!pstFnc || !pstTestCtx)
    {
        DVP_ASSERT_FAIL();
        return NULL;
    }

    pstTestCtx->port_param = params;
    memset(&pstTestCtx->stVppCtx, 0, sizeof(t_StVppCtx));
    init_vpp(&pstTestCtx->stVppCtx);

    pv = send_init(pstFnc, pstTestCtx, &pstTestCtx->stVppCtx, 0, VPP_TRUE,
                   VPP_TRUE, VPP_TRUE);
    DVP_ASSERT_PTR_NNULL(pv);

    move_from_inited_to_active(pstFnc, pv, ctrl, pstTestCtx->port_param);

    return pv;
}

static void term_test(const t_StVppIpFnc *pstFnc,
                      struct test_ctx *pstTestCtx,
                      void *pvCtx, uint32_t bTermTestCtxBufPool)
{
    uint32_t u32;

    if (!pstFnc || !pstTestCtx || !pvCtx)
    {
        DVP_ASSERT_FAIL();
        return;
    }

    u32 = send_close(pstFnc, pvCtx);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    send_term(pstFnc, pvCtx);

    wait_for_buffers(pstTestCtx, VPP_PORT_INPUT);
    wait_for_buffers(pstTestCtx, VPP_PORT_OUTPUT);

    if (bTermTestCtxBufPool)
        free_tctx_buf_pool(pstTestCtx, VPP_FALSE);

    term_vpp(&pstTestCtx->stVppCtx);
}

static void testcase_term_with_buffers(const t_StVppIpBlock *pstIp,
                                       struct test_ctx *pstTestCtx,
                                       enum vpp_port ePort, uint32_t u32BufCnt)
{
    uint32_t u32;
    uint32_t u32Cnt;
    uint32_t u32BufPoolSize = u32BufCnt;
    struct hqv_control ctrl;
    void *pv;
    const t_StVppIpFnc *pstFnc;

    if (!pstIp || !pstTestCtx || ePort >= VPP_PORT_MAX)
    {
        DVP_ASSERT_FAIL();
        return;
    }

    pstFnc = &pstIp->fnc;
    ctrl.mode = HQV_MODE_AUTO;
    prep_port(&pstTestCtx->port_param, VPP_RESOLUTION_HD,
              get_supported_fmt(pstIp));

    pv = init_test(pstFnc, &stTestCtx, ctrl, pstTestCtx->port_param);
    DVP_ASSERT_PTR_NNULL(pv);

    u32 = init_tctx_buf_pool(pstTestCtx, u32BufPoolSize, VPP_FALSE);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    for (u32Cnt = 0; u32Cnt < u32BufCnt; u32Cnt++)
    {
        u32 = get_and_send_buf(pstFnc, pstTestCtx, pv, ePort,
                               eVppBufType_Progressive, 0, 0,
                               &pstTestCtx->mutex);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
    }

    send_term(pstFnc, pv);
    wait_for_buffers(pstTestCtx, ePort);

    free_tctx_buf_pool(pstTestCtx, VPP_FALSE);
    term_vpp(&pstTestCtx->stVppCtx);

    pthread_mutex_lock(&pstTestCtx->mutex);
    if (ePort == VPP_PORT_INPUT)
    {
        DVP_ASSERT_EQUAL(pstTestCtx->u32InPutCnt, u32BufCnt);
        DVP_ASSERT_EQUAL(pstTestCtx->u32InFlushCnt, u32BufCnt);
        DVP_ASSERT_EQUAL(pstTestCtx->u32InRxCnt, u32BufCnt);
    }
    else
    {
        DVP_ASSERT_EQUAL(pstTestCtx->u32OutPutCnt, u32BufCnt);
        DVP_ASSERT_EQUAL(pstTestCtx->u32OutFlushCnt, u32BufCnt);
        DVP_ASSERT_EQUAL(pstTestCtx->u32OutRxCnt, u32BufCnt);
    }
    pthread_mutex_unlock(&pstTestCtx->mutex);
}

static void test_get_buffer_requirements_valid(const t_StVppIpFnc *pstFnc,
                                               void *pvCtx)
{
    uint32_t u32;
    t_StVppIpBufReq stInBufReq;
    t_StVppIpBufReq stOutBufReq;

    if (!pstFnc || !pvCtx)
    {
        DVP_ASSERT_FAIL();
        return;
    }

    // Valid case: all params are non-null.
    u32 = send_get_buffer_requirements(pstFnc, pvCtx, &stInBufReq,
                                       &stOutBufReq);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // Valid case: null input buffer requirements struct.
    u32 = send_get_buffer_requirements(pstFnc, pvCtx, NULL, &stOutBufReq);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // Valid case: null output buffer requirements struct.
    u32 = send_get_buffer_requirements(pstFnc, pvCtx, &stInBufReq, NULL);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // Valid case: null input and output buffer requirements struct.
    u32 = send_get_buffer_requirements(pstFnc, pvCtx, NULL, NULL);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
}

static void test_get_buffer_requirements_invalid(const t_StVppIpFnc *pstFnc,
                                                 void *pvSessCtx)
{
    uint32_t u32;
    t_StVppIpBufReq stInBufReq;
    t_StVppIpBufReq stOutBufReq;

    if (!pstFnc || !pvSessCtx)
    {
        DVP_ASSERT_FAIL();
        return;
    }

    // Invalid case: null ctx pointer.
    u32 = send_get_buffer_requirements(pstFnc, NULL, &stInBufReq, &stOutBufReq);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);
}

static void testcase_flush_buffers(const t_StVppIpBlock *pstIp,
                                   struct test_ctx *pstTestCtx,
                                   enum vpp_port ePort, uint32_t u32FlushCnt,
                                   uint32_t u32BufCnt)
{
    uint32_t u32;
    uint32_t u32Cnt;
    uint32_t u32BufPoolSize = u32BufCnt;
    struct hqv_control ctrl;
    void *pv;
    struct ip_test_ctx *pstIpTestCtx;
    const t_StVppIpFnc *pstFnc;

    if (!pstIp || !pstTestCtx || ePort >= VPP_PORT_MAX)
    {
        DVP_ASSERT_FAIL();
        return;
    }

    pstFnc = &pstIp->fnc;
    pstIpTestCtx = (struct ip_test_ctx *)pstTestCtx->pPrivateCtx;

    ctrl.mode = HQV_MODE_AUTO;
    prep_port(&pstTestCtx->port_param, VPP_RESOLUTION_HD,
              get_supported_fmt(pstIp));

    pv = init_test(pstFnc, &stTestCtx, ctrl, pstTestCtx->port_param);
    DVP_ASSERT_PTR_NNULL(pv);

    u32 = init_tctx_buf_pool(pstTestCtx, u32BufPoolSize, VPP_FALSE);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    for (u32Cnt = 0; u32Cnt < u32BufCnt; u32Cnt++)
    {
        u32 = get_and_send_buf(pstFnc, pstTestCtx, pv, ePort,
                               eVppBufType_Progressive, 0, 0,
                               &pstTestCtx->mutex);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
    }

    for (u32Cnt = 0; u32Cnt < u32FlushCnt; u32Cnt++)
    {
        u32 = send_flush(pstFnc, pv, ePort);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
    }

    wait_for_flush_done(pstTestCtx, ePort, u32FlushCnt);

    pthread_mutex_lock(&pstTestCtx->mutex);
    if (ePort == VPP_PORT_INPUT)
    {
        DVP_ASSERT_EQUAL(pstTestCtx->u32InPutCnt, u32BufCnt);
        DVP_ASSERT_EQUAL(pstTestCtx->u32InFlushCnt, u32BufCnt);
        DVP_ASSERT_EQUAL(pstTestCtx->u32InRxCnt, u32BufCnt);
        DVP_ASSERT_EQUAL(pstIpTestCtx->u32FlushDoneInCnt, u32FlushCnt);
    }
    else
    {
        DVP_ASSERT_EQUAL(pstTestCtx->u32OutPutCnt, u32BufCnt);
        DVP_ASSERT_EQUAL(pstTestCtx->u32OutFlushCnt, u32BufCnt);
        DVP_ASSERT_EQUAL(pstTestCtx->u32OutRxCnt, u32BufCnt);
        DVP_ASSERT_EQUAL(pstIpTestCtx->u32FlushDoneOutCnt, u32FlushCnt);
    }
    pthread_mutex_unlock(&pstTestCtx->mutex);

    term_test(pstFnc, pstTestCtx, pv, VPP_TRUE);
}

static void test_flush_invalid(const t_StVppIpFnc *pstFnc, void *pvCtx)
{
    uint32_t u32;

    if (!pstFnc || !pvCtx)
    {
        DVP_ASSERT_FAIL();
        return;
    }

    // Invalid case: null ctx pointer.
    u32 = send_flush(pstFnc, NULL, VPP_PORT_INPUT);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);

    // Invalid case: out of range port enum.
    u32 = send_flush(pstFnc, pvCtx, VPP_PORT_MAX);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);
}

static void testcase_drain_no_buffers(const t_StVppIpBlock *pstIp,
                                      struct test_ctx *pstTestCtx,
                                      uint32_t u32DrainCnt)
{
    uint32_t u32;
    uint32_t u32Cnt;
    void *pv;
    struct hqv_control ctrl;
    const t_StVppIpFnc *pstFnc;

    if (!pstIp || !pstTestCtx)
    {
        DVP_ASSERT_FAIL();
        return;
    }

    pstFnc = &pstIp->fnc;
    ctrl.mode = HQV_MODE_AUTO;
    prep_port(&pstTestCtx->port_param, VPP_RESOLUTION_HD,
              get_supported_fmt(pstIp));

    pv = init_test(pstFnc, pstTestCtx, ctrl, pstTestCtx->port_param);
    DVP_ASSERT_PTR_NNULL(pv);

    for (u32Cnt = 0; u32Cnt < u32DrainCnt; u32Cnt++)
    {
        u32 = send_drain(pstFnc, pv);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
    }

    wait_for_drain_done(pstTestCtx, u32DrainCnt);

    term_test(pstFnc, pstTestCtx, pv, VPP_FALSE);
}

static void test_setup(struct test_ctx *pstTestCtx,
                       struct ip_test_ctx *pstIpTestCtx,
                       t_EVppIp eIpBlock)
{
    uint32_t u32;

    if (!pstTestCtx || !pstIpTestCtx)
    {
        DVP_ASSERT_FAIL();
        return;
    }

    u32 = tctx_common_init(pstTestCtx);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    memset(pstIpTestCtx, 0, sizeof(*pstIpTestCtx));
    pstTestCtx->pPrivateCtx = pstIpTestCtx;
    u32 = ts_mgr_init(&pstIpTestCtx->stTsMgr, TS_FREQ_30HZ, 20);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    pstIpTestCtx->bChangeBufferMapping = VPP_FALSE;
    if (eIpBlock == VPP_IP_HVX && u32VppUtils_IsSoc(MSM8996))
        pstIpTestCtx->bChangeBufferMapping = VPP_TRUE;
}

static void test_cleanup(struct test_ctx *pstTestCtx)
{
    struct ip_test_ctx *pstIpTestCtx;

    if (!pstTestCtx)
    {
        DVP_ASSERT_FAIL();
        return;
    }

    pstIpTestCtx = (struct ip_test_ctx *)pstTestCtx->pPrivateCtx;

    tctx_common_destroy(pstTestCtx);
    ts_mgr_destroy(&pstIpTestCtx->stTsMgr);
}

static void get_ips(const t_StVppIpBlock **ppstIp, uint32_t u32Sz)
{
    uint32_t u32Cnt, i, j;
    t_StVppUsecase **ppUcReg;

    if (u32Sz < VPP_IP_MAX)
        return;

    memset(ppstIp, 0, sizeof(t_StVppIpBlock *) * VPP_IP_MAX);

    ppUcReg = ppstVppUsecase_GetRegistry(&u32Cnt);
    DVP_ASSERT_PTR_NNULL(ppUcReg);

    // walk each usecase and find all the IPs
    for (i = 0; i < u32Cnt; i++)
    {
        t_StVppUsecase *pstUc;
        pstUc = ppUcReg[i];
        if (!pstUc)
        {
            LOGE("found a null usecase... this is impossible...");
            continue;
        }
        for (j = 0; j < pstUc->u32IpCnt; j++)
        {
            t_StVppUsecaseIp *pstUcIp;
            pstUcIp = &pstUc->composition[j];
            if (pstUcIp->ip)
            {
                t_EVppIp eIp = pstUcIp->ip->eIp;
                if (ppstIp[eIp])
                    continue;
                else
                {
                    ppstIp[eIp] = pstUcIp->ip;
                }
            }
        }
    }

    for (i = 0; i < VPP_IP_MAX; i++)
    {
        LOGD("IP [%d] = %p", i, ppstIp[i]);
    }

}

//#define DO_ONLY_IP_BLOCK    VPP_IP_FRC
void foreach_ip_do(test_fnc_cb cb)
{
    uint32_t i;

    const t_StVppIpBlock *apstIp[VPP_IP_MAX];
    get_ips(apstIp, VPP_IP_MAX);

    for (i = 0; i < VPP_IP_MAX; i++)
    {
#ifdef DO_ONLY_IP_BLOCK
        if (i != DO_ONLY_IP_BLOCK)
            continue;
#endif
        if (apstIp[i])
        {
            test_setup(&stTestCtx, &stIpTestCtx, apstIp[i]->eIp);

            cb(apstIp[i]);

            test_cleanup(&stTestCtx);
        }
    }
}

/************************************************************************
 * Test Functions
 ***********************************************************************/
TEST_SUITE_INIT(RegSuiteInit)
{
    if (u32HcpMock_EnableErrorAction)
        u32HcpMock_EnableErrorAction(vDvpTb_Assert, VPP_FALSE, VPP_TRUE);
}

TEST_SUITE_TERM(RegSuiteTerm)
{
    if (u32HcpMock_EnableErrorAction)
        u32HcpMock_EnableErrorAction(NULL, VPP_FALSE, VPP_FALSE);
}

TEST_SETUP(RegTestInit)
{
}

TEST_CLEANUP(RegTestTerm)
{
}

TEST_CB_DECL(RegIpApi_ValidateFncPtrs, pstIp)
{
    /*!
     * Basic test that validates that all of the required APIs are defined and
     * are not null in the registry.
     */
    const t_StVppIpFnc *pstFnc = &pstIp->fnc;

    DVP_ASSERT_PTR_NNULL(pstFnc->init);
    DVP_ASSERT_PTR_NNULL(pstFnc->term);
    DVP_ASSERT_PTR_NNULL(pstFnc->open);
    DVP_ASSERT_PTR_NNULL(pstFnc->close);
    DVP_ASSERT_PTR_NNULL(pstFnc->set_param);
    DVP_ASSERT_PTR_NNULL(pstFnc->set_ctrl);
    DVP_ASSERT_PTR_NNULL(pstFnc->get_buffer_requirements);
    DVP_ASSERT_PTR_NNULL(pstFnc->queue_buf);
    DVP_ASSERT_PTR_NNULL(pstFnc->flush);
    DVP_ASSERT_PTR_NNULL(pstFnc->drain);

    // boot, shutdown, reset_ctrl, reconfigure optional
}

TEST(RegIpApi_ValidateFncPtrs)
{
    foreach_ip_do(TEST_CB_FNC(RegIpApi_ValidateFncPtrs));
}

TEST_CB_DECL(RegIpApi_InitTermBasic, pstIp)
{
    /*!
     * Basic test that calls Init(), followed by Term().
     */

    void *pv;
    t_StVppCtx stCtx;

    const t_StVppIpFnc *pstFnc = &pstIp->fnc;
    memset(&stCtx, 0, sizeof(t_StVppCtx));

    pv = send_init(pstFnc, &stTestCtx, &stCtx, 0, VPP_TRUE, VPP_TRUE, VPP_TRUE);
    DVP_ASSERT_PTR_NNULL(pv);

    send_term(pstFnc, pv);
}

TEST(RegIpApi_InitTermBasic)
{
    foreach_ip_do(TEST_CB_FNC(RegIpApi_InitTermBasic));
}


TEST_CB_DECL(RegIpApi_InitTerm, pstIp)
{
    /*!
     * Basic test that calls Init() and Term() multiple times with both
     * valid and invalid parameters.
     */

    void *pv1, *pv2;
    t_StVppCtx stCtx;

    const t_StVppIpFnc *pstFnc = &pstIp->fnc;
    memset(&stCtx, 0, sizeof(t_StVppCtx));

    pv1 = send_init(pstFnc, &stTestCtx, NULL, 0, VPP_FALSE, VPP_FALSE, VPP_FALSE);
    DVP_ASSERT_PTR_NULL(pv1);
    pv1 = send_init(pstFnc, &stTestCtx, NULL, 0, VPP_FALSE, VPP_FALSE, VPP_TRUE);
    DVP_ASSERT_PTR_NULL(pv1);
    pv1 = send_init(pstFnc, &stTestCtx, NULL, 0, VPP_FALSE, VPP_TRUE, VPP_FALSE);
    DVP_ASSERT_PTR_NULL(pv1);
    pv1 = send_init(pstFnc, &stTestCtx, NULL, 0, VPP_FALSE, VPP_TRUE, VPP_TRUE);
    DVP_ASSERT_PTR_NULL(pv1);
    pv1 = send_init(pstFnc, &stTestCtx, NULL, 0, VPP_TRUE, VPP_FALSE, VPP_FALSE);
    DVP_ASSERT_PTR_NULL(pv1);
    pv1 = send_init(pstFnc, &stTestCtx, NULL, 0, VPP_TRUE, VPP_FALSE, VPP_TRUE);
    DVP_ASSERT_PTR_NULL(pv1);
    pv1 = send_init(pstFnc, &stTestCtx, NULL, 0, VPP_TRUE, VPP_TRUE, VPP_FALSE);
    DVP_ASSERT_PTR_NULL(pv1);
    pv1 = send_init(pstFnc, &stTestCtx, NULL, 0, VPP_TRUE, VPP_TRUE, VPP_TRUE);
    DVP_ASSERT_PTR_NULL(pv1);

    pv1 = send_init(pstFnc, &stTestCtx, &stCtx, 0, VPP_FALSE, VPP_FALSE, VPP_FALSE);
    DVP_ASSERT_PTR_NULL(pv1);
    pv1 = send_init(pstFnc, &stTestCtx, &stCtx, 0, VPP_FALSE, VPP_FALSE, VPP_TRUE);
    DVP_ASSERT_PTR_NULL(pv1);
    pv1 = send_init(pstFnc, &stTestCtx, &stCtx, 0, VPP_FALSE, VPP_TRUE, VPP_FALSE);
    DVP_ASSERT_PTR_NULL(pv1);
    pv1 = send_init(pstFnc, &stTestCtx, &stCtx, 0, VPP_FALSE, VPP_TRUE, VPP_TRUE);
    DVP_ASSERT_PTR_NULL(pv1);
    pv1 = send_init(pstFnc, &stTestCtx, &stCtx, 0, VPP_TRUE, VPP_FALSE, VPP_FALSE);
    DVP_ASSERT_PTR_NULL(pv1);
    pv1 = send_init(pstFnc, &stTestCtx, &stCtx, 0, VPP_TRUE, VPP_FALSE, VPP_TRUE);
    DVP_ASSERT_PTR_NULL(pv1);
    pv1 = send_init(pstFnc, &stTestCtx, &stCtx, 0, VPP_TRUE, VPP_TRUE, VPP_FALSE);
    DVP_ASSERT_PTR_NULL(pv1);

    send_term(pstFnc, NULL);

    // valid cases
    pv1 = send_init(pstFnc, &stTestCtx, &stCtx, 0, VPP_TRUE, VPP_TRUE, VPP_TRUE);
    DVP_ASSERT_PTR_NNULL(pv1);

    pv2 = send_init(pstFnc, &stTestCtx, &stCtx, 0, VPP_TRUE, VPP_TRUE, VPP_TRUE);
    DVP_ASSERT_PTR_NNULL(pv1);
    DVP_ASSERT_PTR_NEQUAL(pv1, pv2);

    send_term(pstFnc, pv1);
    send_term(pstFnc, pv2);
}

TEST(RegIpApi_InitTerm)
{
    foreach_ip_do(TEST_CB_FNC(RegIpApi_InitTerm));
}

TEST_CB_DECL(RegIpApi_InitOpenCfgCloseTerm, pstIp)
{
    /*!
     * Basic test that calls Init(), SetParam(), SetCfg(), Open(),
     * Close(), Term(), with auto as the configuration, and port parameters
     * with 720p/NV12.
     */

    void *pv;
    uint32_t u32;
    t_StVppCtx stCtx;

    struct vpp_port_param port;
    struct hqv_control ctrl;

    const t_StVppIpFnc *pstFnc = &pstIp->fnc;
    memset(&stCtx, 0, sizeof(t_StVppCtx));
    prep_port(&port, VPP_RESOLUTION_HD, get_supported_fmt(pstIp));
    init_vpp(&stCtx);

    ctrl.mode = HQV_MODE_AUTO;

    pv = send_init(pstFnc, &stTestCtx, &stCtx, 0, VPP_TRUE, VPP_TRUE, VPP_TRUE);
    DVP_ASSERT_PTR_NNULL(pv);

    u32 = send_set_ctrl(pstFnc, pv, ctrl);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = send_set_param(pstFnc, pv, VPP_PORT_INPUT, port);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = send_set_param(pstFnc, pv, VPP_PORT_OUTPUT, port);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = send_open(pstFnc, pv);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = send_close(pstFnc, pv);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    send_term(pstFnc, pv);

    term_vpp(&stCtx);
}

TEST(RegIpApi_InitOpenCfgCloseTerm)
{
    foreach_ip_do(TEST_CB_FNC(RegIpApi_InitOpenCfgCloseTerm));
}

TEST_CB_DECL(RegIpApi_OpenCloseOpenClose, pstIp)
{
    /*
     * Basic test that calls Init(), SetParam(), SetCtrl(), Open(), Close(),
     * Open(), Close(), Term(), with no invalid conditions.
     */

    uint32_t u32;
    void *pv = NULL;
    t_StVppCtx stCtx;
    struct vpp_port_param param;
    struct hqv_control ctrl;

    const t_StVppIpFnc *pstFnc = &pstIp->fnc;
    memset(&stCtx, 0, sizeof(t_StVppCtx));
    prep_port(&param, VPP_RESOLUTION_HD, get_supported_fmt(pstIp));
    init_vpp(&stCtx);

    ctrl.mode = HQV_MODE_AUTO;

    pv = send_init(pstFnc, &stTestCtx, &stCtx, 0, VPP_TRUE, VPP_TRUE, VPP_TRUE);
    DVP_ASSERT_PTR_NNULL(pv);

    u32 = send_set_ctrl(pstFnc, pv, ctrl);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = send_set_param(pstFnc, pv, VPP_PORT_INPUT, param);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = send_set_param(pstFnc, pv, VPP_PORT_OUTPUT, param);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = send_open(pstFnc, pv);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = send_close(pstFnc, pv);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = send_open(pstFnc, pv);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = send_close(pstFnc, pv);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    send_term(pstFnc, pv);
}

TEST(RegIpApi_OpenCloseOpenClose)
{
    foreach_ip_do(TEST_CB_FNC(RegIpApi_OpenCloseOpenClose));
}

TEST_CB_DECL(RegIpApi_OpenWithoutSetCtrl, pstIp)
{
    /*!
     * Calls Open() without first calling SetCtrl(). Expecting successful return.
     */

    uint32_t u32;
    void *pv = NULL;
    t_StVppCtx stCtx;
    struct vpp_port_param param;

    const t_StVppIpFnc *pstFnc = &pstIp->fnc;
    memset(&stCtx, 0, sizeof(t_StVppCtx));
    prep_port(&param, VPP_RESOLUTION_HD, get_supported_fmt(pstIp));
    init_vpp(&stCtx);

    pv = send_init(pstFnc, &stTestCtx, &stCtx, 0, VPP_TRUE, VPP_TRUE, VPP_TRUE);
    DVP_ASSERT_PTR_NNULL(pv);

    u32 = send_set_param(pstFnc, pv, VPP_PORT_INPUT, param);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = send_set_param(pstFnc, pv, VPP_PORT_OUTPUT, param);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = send_open(pstFnc, pv);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = send_close(pstFnc, pv);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    send_term(pstFnc, pv);

    term_vpp(&stCtx);
}

TEST(RegIpApi_OpenWithoutSetCtrl)
{
    foreach_ip_do(TEST_CB_FNC(RegIpApi_OpenWithoutSetCtrl));
}

TEST_CB_DECL(RegIpApi_OpenCloseInvalid, pstIp)
{
    /*!
     * Call Open() and Close() under invalid conditions: invalid parameters,
     * without calling Init(), etc.
     */

    uint32_t u32;
    void *pv = NULL;
    t_StVppCtx stCtx;
    struct vpp_port_param param;
    struct hqv_control ctrl;

    const t_StVppIpFnc *pstFnc = &pstIp->fnc;
    memset(&stCtx, 0, sizeof(t_StVppCtx));
    prep_port(&param, VPP_RESOLUTION_HD, get_supported_fmt(pstIp));
    init_vpp(&stCtx);

    ctrl.mode = HQV_MODE_AUTO;

    // NULL context
    u32 = send_open(pstFnc, NULL);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);

    u32 = send_close(pstFnc, NULL);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);

    // Tests which have called Init() and have a valid context
    pv = send_init(pstFnc, &stTestCtx, &stCtx, 0, VPP_TRUE, VPP_TRUE, VPP_TRUE);
    DVP_ASSERT_PTR_NNULL(pv);

    // Close() without calling Open()
    u32 = send_close(pstFnc, pv);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_STATE);

    // Open without SetParam called
    u32 = send_open(pstFnc, pv);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    u32 = send_set_param(pstFnc, pv, VPP_PORT_INPUT, param);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = send_set_param(pstFnc, pv, VPP_PORT_OUTPUT, param);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    //  Open with SetParam called
    u32 = send_open(pstFnc, pv);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = send_close(pstFnc, pv);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    send_term(pstFnc, pv);

    // Open with SetCtrl but not SetParam
    pv = send_init(pstFnc, &stTestCtx, &stCtx, 0, VPP_TRUE, VPP_TRUE, VPP_TRUE);
    DVP_ASSERT_PTR_NNULL(pv);

    u32 = send_set_ctrl(pstFnc, pv, ctrl);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = send_open(pstFnc, pv);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    // Open with SetCtrl and SetParam
    u32 = send_set_param(pstFnc, pv, VPP_PORT_INPUT, param);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = send_set_param(pstFnc, pv, VPP_PORT_OUTPUT, param);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = send_open(pstFnc, pv);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // Double open
    u32 = send_open(pstFnc, pv);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_STATE);

    // Double close
    u32 = send_close(pstFnc, pv);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = send_close(pstFnc, pv);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_STATE);

    // Valid to Open() / Close()
    u32 = send_open(pstFnc, pv);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = send_close(pstFnc, pv);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    send_term(pstFnc, pv);

    term_vpp(&stCtx);
}

TEST(RegIpApi_OpenCloseInvalid)
{
    foreach_ip_do(TEST_CB_FNC(RegIpApi_OpenCloseInvalid));
}

TEST_CB_DECL(RegIpApi_SetCtrlInvalid, pstIp)
{
    /*
     * Calls SetCtrl() with invalid parameters.
     */
    uint32_t u32;
    void *pv;
    struct hqv_control ctrl;
    const t_StVppIpFnc *pstFnc = &pstIp->fnc;

    memset(&stTestCtx.stVppCtx, 0, sizeof(t_StVppCtx));
    prep_port(&stTestCtx.port_param, VPP_RESOLUTION_HD,
              get_supported_fmt(pstIp));
    init_vpp(&stTestCtx.stVppCtx);

    pv = send_init(pstFnc, &stTestCtx, &stTestCtx.stVppCtx, 0, VPP_TRUE,
                   VPP_TRUE, VPP_TRUE);
    DVP_ASSERT_PTR_NNULL(pv);

    // Invalid case: null ctx pointer.
    ctrl.mode = HQV_MODE_AUTO;
    u32 = send_set_ctrl(pstFnc, NULL, ctrl);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);

    // Invalid case: mode is invalid.
    ctrl.mode = HQV_MODE_MAX;
    u32 = send_set_ctrl(pstFnc, pv, ctrl);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);
    if (u32 != VPP_ERR_PARAM && u32 != VPP_ERR_INVALID_CFG)
        DVP_ASSERT_FAIL();

    // Invalid case: control type is invalid.
    ctrl.mode = HQV_MODE_MANUAL;
    ctrl.ctrl_type = HQV_CONTROL_MAX;
    u32 = send_set_ctrl(pstFnc, pv, ctrl);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);
    if (u32 != VPP_ERR_PARAM && u32 != VPP_ERR_INVALID_CFG)
        DVP_ASSERT_FAIL();

    // Valid case.
    ctrl.mode = HQV_MODE_AUTO;
    u32 = send_set_ctrl(pstFnc, pv, ctrl);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    send_term(pstFnc, pv);

    term_vpp(&stTestCtx.stVppCtx);
}

TEST(RegIpApi_SetCtrlInvalid)
{
    foreach_ip_do(TEST_CB_FNC(RegIpApi_SetCtrlInvalid));
}

TEST_CB_DECL(RegIpApi_ResetCtrlBasic, pstIp)
{
    /*
     * Call ResetCtrl() with valid parameters in a valid condition. Only runs
     * if the IP block has a ResetCtrl() implemented. If not, will skip.
     */
    uint32_t u32;
    void *pv;
    const t_StVppIpFnc *pstFnc = &pstIp->fnc;

    if (pstFnc->reset_ctrl)
    {
        memset(&stTestCtx.stVppCtx, 0, sizeof(t_StVppCtx));
        prep_port(&stTestCtx.port_param, VPP_RESOLUTION_HD,
                  get_supported_fmt(pstIp));
        init_vpp(&stTestCtx.stVppCtx);

        pv = send_init(pstFnc, &stTestCtx, &stTestCtx.stVppCtx, 0, VPP_TRUE,
                       VPP_TRUE, VPP_TRUE);
        DVP_ASSERT_PTR_NNULL(pv);

        u32 = send_reset_ctrl(pstFnc, pv);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        send_term(pstFnc, pv);

        term_vpp(&stTestCtx.stVppCtx);
    }
}

TEST(RegIpApi_ResetCtrlBasic)
{
    foreach_ip_do(TEST_CB_FNC(RegIpApi_ResetCtrlBasic));
}

TEST_CB_DECL(RegIpApi_ResetCtrlInvalid, pstIp)
{
    /*
     * Call ResetCtrl() with invalid parameters and in invalid states. Only
     * runs if the IP block has a ResetCtrl() implemented. If not, will skip.
     */
    uint32_t u32;
    void *pv;
    const t_StVppIpFnc *pstFnc = &pstIp->fnc;
    struct hqv_control ctrl;

    if (pstFnc->reset_ctrl)
    {
        ctrl.mode = HQV_MODE_AUTO;

        memset(&stTestCtx.stVppCtx, 0, sizeof(t_StVppCtx));
        prep_port(&stTestCtx.port_param, VPP_RESOLUTION_HD,
                  get_supported_fmt(pstIp));
        init_vpp(&stTestCtx.stVppCtx);

        pv = send_init(pstFnc, &stTestCtx, &stTestCtx.stVppCtx, 0, VPP_TRUE,
                       VPP_TRUE, VPP_TRUE);
        DVP_ASSERT_PTR_NNULL(pv);

        // Invalid case: null ctx pointer.
        u32 = send_reset_ctrl(pstFnc, NULL);
        DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);

        // Valid case.
        u32 = send_reset_ctrl(pstFnc, pv);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        move_from_inited_to_active(pstFnc, pv, ctrl, stTestCtx.port_param);

        // Invalid case: in ACTIVE state.
        u32 = send_reset_ctrl(pstFnc, pv);
        DVP_ASSERT_EQUAL(u32, VPP_ERR_STATE);

        send_term(pstFnc, pv);

        term_vpp(&stTestCtx.stVppCtx);
    }
}

TEST(RegIpApi_ResetCtrlInvalid)
{
    foreach_ip_do(TEST_CB_FNC(RegIpApi_ResetCtrlInvalid));
}

TEST_CB_DECL(RegIpApi_SetParamInvalid, pstIp)
{
    /*
     * Calls SetParam() with invalid parameters.
     */
    uint32_t u32;
    uint32_t u32Temp;
    void *pv;
    struct hqv_control ctrl;
    struct vpp_port_param params;
    const t_StVppIpFnc *pstFnc = &pstIp->fnc;

    memset(&stTestCtx.stVppCtx, 0, sizeof(t_StVppCtx));
    prep_port(&params, VPP_RESOLUTION_HD, get_supported_fmt(pstIp));
    init_vpp(&stTestCtx.stVppCtx);

    ctrl.mode = HQV_MODE_AUTO;

    pv = send_init(pstFnc, &stTestCtx, &stTestCtx.stVppCtx, 0, VPP_TRUE,
                   VPP_TRUE, VPP_TRUE);
    DVP_ASSERT_PTR_NNULL(pv);

    // Invalid case: null ctx pointer.
    u32 = send_set_param(pstFnc, NULL, VPP_PORT_INPUT, params);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);

    // Invalid case: invalid port enum.
    u32 = send_set_param(pstFnc, pv, VPP_PORT_MAX, params);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);

    // Invalid case: invalid port parameter colour format.
    u32Temp = params.fmt;
    params.fmt = VPP_COLOR_FORMAT_MAX;
    u32 = send_set_param(pstFnc, pv, VPP_PORT_INPUT, params);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);
    params.fmt = u32Temp;

    // Invalid case: width > stride.
    u32Temp = params.width;
    params.width = params.stride + 2;
    u32 = send_set_param(pstFnc, pv, VPP_PORT_INPUT, params);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);
    params.width = u32Temp;

    // Invalid case: height > scanlines.
    u32Temp = params.height;
    params.height = params.scanlines + 2;
    u32 = send_set_param(pstFnc, pv, VPP_PORT_INPUT, params);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);
    params.height = u32Temp;

    // Valid case.
    u32 = send_set_param(pstFnc, pv, VPP_PORT_INPUT, params);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    u32 = send_set_param(pstFnc, pv, VPP_PORT_OUTPUT, params);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = send_set_ctrl(pstFnc, pv, ctrl);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = send_open(pstFnc, pv);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // Invalid case: in ACTIVE state.
    u32 = send_set_param(pstFnc, pv, VPP_PORT_INPUT, stTestCtx.port_param);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_STATE);
    u32 = send_set_param(pstFnc, pv, VPP_PORT_OUTPUT, stTestCtx.port_param);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_STATE);

    u32 = send_close(pstFnc, pv);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    send_term(pstFnc, pv);

    term_vpp(&stTestCtx.stVppCtx);
}

TEST(RegIpApi_SetParamInvalid)
{
    foreach_ip_do(TEST_CB_FNC(RegIpApi_SetParamInvalid));
}

TEST_CB_DECL(RegIpApi_TermWithoutOpen, pstIp)
{
    /*
     * Calls Term() after a SetCtrl() and SetParam(), to see that it terminates
     * cleanly and successfully.
     */
    uint32_t u32;
    struct hqv_control ctrl;
    t_StVppCtx stCtx;
    struct vpp_port_param param;
    void *pv;

    const t_StVppIpFnc *pstFnc = &pstIp->fnc;
    memset(&stCtx, 0, sizeof(t_StVppCtx));
    ctrl.mode = HQV_MODE_AUTO;
    prep_port(&param, VPP_RESOLUTION_HD, get_supported_fmt(pstIp));
    init_vpp(&stCtx);

    pv = send_init(pstFnc, &stTestCtx, &stCtx, 0, VPP_TRUE, VPP_TRUE, VPP_TRUE);
    DVP_ASSERT_PTR_NNULL(pv);

    u32 = send_set_ctrl(pstFnc, pv, ctrl);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = send_set_param(pstFnc, pv, VPP_PORT_INPUT, param);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = send_set_param(pstFnc, pv, VPP_PORT_OUTPUT, param);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    send_term(pstFnc, pv);

    term_vpp(&stCtx);
}

TEST(RegIpApi_TermWithoutOpen)
{
    foreach_ip_do(TEST_CB_FNC(RegIpApi_TermWithoutOpen));
}

TEST_CB_DECL(RegIpApi_TermWithoutClose, pstIp)
{
    /*
     * Calls Term() after an Open() without a Close(), to see that the IP
     * block cleans up their resources correctly.
     */
    uint32_t u32;
    struct hqv_control ctrl;
    t_StVppCtx stCtx;
    struct vpp_port_param param;
    void *pv;

    const t_StVppIpFnc *pstFnc = &pstIp->fnc;
    memset(&stCtx, 0, sizeof(t_StVppCtx));
    ctrl.mode = HQV_MODE_AUTO;
    prep_port(&param, VPP_RESOLUTION_HD, get_supported_fmt(pstIp));
    init_vpp(&stCtx);

    pv = send_init(pstFnc, &stTestCtx, &stCtx, 0, VPP_TRUE, VPP_TRUE, VPP_TRUE);
    DVP_ASSERT_PTR_NNULL(pv);

    u32 = send_set_ctrl(pstFnc, pv, ctrl);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = send_set_param(pstFnc, pv, VPP_PORT_INPUT, param);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = send_set_param(pstFnc, pv, VPP_PORT_OUTPUT, param);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = send_open(pstFnc, pv);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    send_term(pstFnc, pv);

    term_vpp(&stCtx);
}

TEST(RegIpApi_TermWithoutClose)
{
    foreach_ip_do(TEST_CB_FNC(RegIpApi_TermWithoutClose));
}

TEST_CB_DECL(RegIpApi_TermWithInputBuffers, pstIp)
{
    /*
     * Calls Term() when IP block is holding input buffers. Expecting the
     * buffers to be flushed back.
     */
    testcase_term_with_buffers(pstIp, &stTestCtx, VPP_PORT_INPUT, 5);
}

TEST(RegIpApi_TermWithInputBuffers)
{
    foreach_ip_do(TEST_CB_FNC(RegIpApi_TermWithInputBuffers));
}

TEST_CB_DECL(RegIpApi_TermWithOutputBuffers, pstIp)
{
    /*
     * Calls Term() when IP block is holding output buffers. Expecting the
     * buffers to be flushed back.
     */
    testcase_term_with_buffers(pstIp, &stTestCtx, VPP_PORT_OUTPUT, 5);

}

TEST(RegIpApi_TermWithOutputBuffers)
{
    foreach_ip_do(TEST_CB_FNC(RegIpApi_TermWithOutputBuffers));
}

TEST_CB_DECL(RegIpApi_QueueBufBypassFlag, pstIp)
{
    /*
     * Queue one input to see the buffers go through and be received
     * successfully via callbacks. The internal flag of the queued buffer will
     * be set to BYPASS.
     */
    uint32_t u32;
    uint32_t u32Cnt;
    uint32_t u32BufPoolSize = 10;
    uint32_t u32BufCnt = u32BufPoolSize;
    uint32_t bTermTestCtxBufPool = VPP_TRUE;
    struct hqv_control ctrl;
    void *pv;

    const t_StVppIpFnc *pstFnc = &pstIp->fnc;
    ctrl.mode = HQV_MODE_AUTO;
    prep_port(&stTestCtx.port_param, VPP_RESOLUTION_HD,
              get_supported_fmt(pstIp));

    pv = init_test(pstFnc, &stTestCtx, ctrl, stTestCtx.port_param);
    DVP_ASSERT_PTR_NNULL(pv);

    pthread_mutex_lock(&stTestCtx.mutex);
    stIpTestCtx.bBypassExpected = VPP_TRUE;
    pthread_mutex_unlock(&stTestCtx.mutex);

    u32 = init_tctx_buf_pool(&stTestCtx, u32BufPoolSize, VPP_FALSE);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    for (u32Cnt = 0; u32Cnt < u32BufPoolSize; u32Cnt++)
    {
        u32 = get_and_send_buf(pstFnc, &stTestCtx, pv, VPP_PORT_INPUT,
                               eVppBufType_Progressive, 0, VPP_BUF_FLAG_BYPASS,
                               &stTestCtx.mutex);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
    }

    wait_for_buffers(&stTestCtx, VPP_PORT_INPUT);

    term_test(pstFnc, &stTestCtx, pv, bTermTestCtxBufPool);

    pthread_mutex_lock(&stTestCtx.mutex);
    DVP_ASSERT_EQUAL(stTestCtx.u32InPutCnt, u32BufCnt);
    DVP_ASSERT_EQUAL(stTestCtx.u32OutRxCnt, u32BufCnt);
    pthread_mutex_unlock(&stTestCtx.mutex);
}

TEST(RegIpApi_QueueBufBypassFlag)
{
    foreach_ip_do(TEST_CB_FNC(RegIpApi_QueueBufBypassFlag));
}

TEST_CB_DECL(RegIpApi_QueueBufBypassFilledLenZero, pstIp)
{
    /*
     * Calls QueueBuf() with input buffers that have filled_len=0. These
     * buffers should be bypassed.
     */
    uint32_t u32;
    uint32_t u32Cnt;
    uint32_t u32BufPoolSize = 10;
    uint32_t u32BufCnt = u32BufPoolSize;
    uint32_t bTermTestCtxBufPool = VPP_TRUE;
    struct hqv_control ctrl;
    struct bufnode *pstNode;
    void *pv;

    const t_StVppIpFnc *pstFnc = &pstIp->fnc;
    ctrl.mode = HQV_MODE_AUTO;
    prep_port(&stTestCtx.port_param, VPP_RESOLUTION_HD,
              get_supported_fmt(pstIp));

    pv = init_test(pstFnc, &stTestCtx, ctrl, stTestCtx.port_param);
    DVP_ASSERT_PTR_NNULL(pv);

    pthread_mutex_lock(&stTestCtx.mutex);
    stIpTestCtx.bBypassExpected = VPP_TRUE;
    pthread_mutex_unlock(&stTestCtx.mutex);

    u32 = init_tctx_buf_pool(&stTestCtx, u32BufPoolSize, VPP_FALSE);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    for (u32Cnt = 0; u32Cnt < u32BufCnt; u32Cnt++)
    {
        pstNode = get_filled_buf(&stTestCtx, VPP_PORT_INPUT,
                                 eVppBufType_Progressive, 0, 0,
                                 &stTestCtx.mutex);
        DVP_ASSERT_PTR_NNULL(pstNode);

        pstNode->pIntBuf->stPixel.u32FilledLen = 0;
        pstNode->pExtBuf->pixel.filled_len = 0;

        pthread_mutex_lock(&stTestCtx.mutex);
        stTestCtx.u32InPutCnt++;
        stTestCtx.u32InHoldCnt++;
        pthread_mutex_unlock(&stTestCtx.mutex);

        u32 = send_queue_buf(pstFnc, pv, VPP_PORT_INPUT, pstNode->pIntBuf);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
    }

    wait_for_buffers(&stTestCtx, VPP_PORT_INPUT);

    term_test(pstFnc, &stTestCtx, pv, bTermTestCtxBufPool);

    pthread_mutex_lock(&stTestCtx.mutex);
    DVP_ASSERT_EQUAL(stTestCtx.u32InPutCnt, u32BufCnt);
    DVP_ASSERT_EQUAL(stTestCtx.u32OutRxCnt, u32BufCnt);
    pthread_mutex_unlock(&stTestCtx.mutex);
}

TEST(RegIpApi_QueueBufBypassFilledLenZero)
{
    foreach_ip_do(TEST_CB_FNC(RegIpApi_QueueBufBypassFilledLenZero));
}

TEST_CB_DECL(RegIpApi_QueueBufInvalid, pstIp)
{
    /*
     * Calls QueueBuf() with invalid parameters, and in the wrong state.
     */
    uint32_t u32;
    uint32_t u32BufPoolSize = 2;
    uint32_t u32BufCnt = 1;
    struct hqv_control stCtrl;
    struct bufnode *pstNode;
    void *pv;
    const t_StVppIpFnc *pstFnc = &pstIp->fnc;

    pstFnc = &pstIp->fnc;
    memset(&stTestCtx.stVppCtx, 0, sizeof(t_StVppCtx));
    stCtrl.mode = HQV_MODE_AUTO;
    prep_port(&stTestCtx.port_param, VPP_RESOLUTION_HD,
              get_supported_fmt(pstIp));
    init_vpp(&stTestCtx.stVppCtx);

    u32 = init_tctx_buf_pool(&stTestCtx, u32BufPoolSize, VPP_FALSE);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    pstNode = get_filled_buf(&stTestCtx, VPP_PORT_OUTPUT,
                             eVppBufType_Progressive, 0, 0, &stTestCtx.mutex);
    DVP_ASSERT_PTR_NNULL(pstNode);

    pv = send_init(pstFnc, &stTestCtx, &stTestCtx.stVppCtx, 0,
                   VPP_TRUE, VPP_TRUE, VPP_TRUE);
    DVP_ASSERT_PTR_NNULL(pv);

    // Invalid case: in INITED state.
    u32 = send_queue_buf(pstFnc, pv, VPP_PORT_OUTPUT, pstNode->pIntBuf);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_STATE);

    move_from_inited_to_active(pstFnc, pv, stCtrl, stTestCtx.port_param);

    // Invalid case: null ctx pointer.
    u32 = send_queue_buf(pstFnc, NULL, VPP_PORT_OUTPUT, pstNode->pIntBuf);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);

    // Invalid case: out of range port enum.
    u32 = send_queue_buf(pstFnc, pv, VPP_PORT_MAX, pstNode->pIntBuf);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);

    // Invalid case: null buffer pointer.
    u32 = send_queue_buf(pstFnc, pv, VPP_PORT_OUTPUT, NULL);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);

    // Invalid case: closed ctx pointer.
    u32 = send_close(pstFnc, pv);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = send_queue_buf(pstFnc, pv, VPP_PORT_OUTPUT, pstNode->pIntBuf);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_STATE);

    move_from_inited_to_active(pstFnc, pv, stCtrl, stTestCtx.port_param);

    // Valid case.
    pthread_mutex_lock(&stTestCtx.mutex);
    stTestCtx.u32OutPutCnt++;
    stTestCtx.u32OutHoldCnt++;
    pthread_mutex_unlock(&stTestCtx.mutex);

    u32 = send_queue_buf(pstFnc, pv, VPP_PORT_OUTPUT, pstNode->pIntBuf);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = send_close(pstFnc, pv);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    send_term(pstFnc, pv);

    wait_for_buffers(&stTestCtx, VPP_PORT_OUTPUT);

    pthread_mutex_lock(&stTestCtx.mutex);
    DVP_ASSERT_EQUAL(stTestCtx.u32OutPutCnt, u32BufCnt);
    DVP_ASSERT_EQUAL(stTestCtx.u32OutFlushCnt, u32BufCnt);
    DVP_ASSERT_EQUAL(stTestCtx.u32OutRxCnt, u32BufCnt);
    pthread_mutex_unlock(&stTestCtx.mutex);

    u32 = free_tctx_buf_pool(&stTestCtx, VPP_FALSE);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    term_vpp(&stTestCtx.stVppCtx);
}

TEST(RegIpApi_QueueBufInvalid)
{
    foreach_ip_do(TEST_CB_FNC(RegIpApi_QueueBufInvalid));
}

TEST_CB_DECL(RegIpApi_GetBufReqBasic, pstIp)
{
    /*
     * Call GetBufferRequirements() with valid parameters, to see it returned
     * successfully.
     */
    uint32_t u32;
    void *pv;
    struct vpp_port_param param;
    struct hqv_control ctrl;
    t_StVppCtx stCtx;
    t_StVppIpBufReq stInBufReq;
    t_StVppIpBufReq stOutBufReq;

    const t_StVppIpFnc *pstFnc = &pstIp->fnc;
    memset(&stCtx, 0, sizeof(t_StVppCtx));
    prep_port(&param, VPP_RESOLUTION_HD, get_supported_fmt(pstIp));
    init_vpp(&stCtx);

    ctrl.mode = HQV_MODE_AUTO;

    pv = send_init(pstFnc, &stTestCtx, &stCtx, 0, VPP_TRUE, VPP_TRUE, VPP_TRUE);
    DVP_ASSERT_PTR_NNULL(pv);

    // Call get_buffer_requirements in INITED state.
    u32 = send_get_buffer_requirements(pstFnc, pv, &stInBufReq, &stOutBufReq);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    LOGI("%s: i_px=%u, i_ex=%u, i_min=%u, o_px=%u, o_ex=%u, o_min=%u", __func__,
         stInBufReq.u32PxSz, stInBufReq.u32ExSz, stInBufReq.u32MinCnt,
         stOutBufReq.u32PxSz, stOutBufReq.u32ExSz, stOutBufReq.u32MinCnt);

    move_from_inited_to_active(pstFnc, pv, ctrl, param);

    // Call get_buffer_requirements in ACTIVE state.
    u32 = send_get_buffer_requirements(pstFnc, pv, &stInBufReq, &stOutBufReq);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    LOGI("%s: i_px=%u, i_ex=%u, i_min=%u, o_px=%u, o_ex=%u, o_min=%u", __func__,
         stInBufReq.u32PxSz, stInBufReq.u32ExSz, stInBufReq.u32MinCnt,
         stOutBufReq.u32PxSz, stOutBufReq.u32ExSz, stOutBufReq.u32MinCnt);

    // Cleanup.
    u32 = send_close(pstFnc, pv);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    send_term(pstFnc, pv);

    term_vpp(&stCtx);
}

TEST(RegIpApi_GetBufReqBasic)
{
    foreach_ip_do(TEST_CB_FNC(RegIpApi_GetBufReqBasic));
}

TEST_CB_DECL(RegIpApi_GetBufReqValid, pstIp)
{
    /*
     * Will call get_buffer_requirements() in any valid condition and state, to
     * see it return as successful.
     */
    uint32_t u32;
    void *pv = NULL;
    t_StVppCtx stCtx;
    struct vpp_port_param param;
    struct hqv_control ctrl;

    const t_StVppIpFnc *pstFnc = &pstIp->fnc;
    memset(&stCtx, 0, sizeof(t_StVppCtx));
    prep_port(&param, VPP_RESOLUTION_HD, get_supported_fmt(pstIp));
    init_vpp(&stCtx);

    ctrl.mode = HQV_MODE_AUTO;

    pv = send_init(pstFnc, &stTestCtx, &stCtx, 0, VPP_TRUE, VPP_TRUE, VPP_TRUE);
    DVP_ASSERT_PTR_NNULL(pv);

    // Test valid params in INITED state.
    test_get_buffer_requirements_valid(pstFnc, pv);

    move_from_inited_to_active(pstFnc, pv, ctrl, param);

    // Test valid params in ACTIVE state.
    test_get_buffer_requirements_valid(pstFnc, pv);

    // Cleanup.
    u32 = send_close(pstFnc, pv);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    send_term(pstFnc, pv);

    term_vpp(&stCtx);
}

TEST(RegIpApi_GetBufReqValid)
{
    foreach_ip_do(TEST_CB_FNC(RegIpApi_GetBufReqValid));
}

TEST_CB_DECL(RegIpApi_GetBufReqInvalid, pstIp)
{
    /*
     * Will call get_buffer_requirements() with invalid parameters. Expecting
     * an unsuccessful error code.
     */
    uint32_t u32;
    void *pv = NULL;
    t_StVppCtx stCtx;
    struct vpp_port_param param;
    struct hqv_control ctrl;

    const t_StVppIpFnc *pstFnc = &pstIp->fnc;
    memset(&stCtx, 0, sizeof(t_StVppCtx));
    prep_port(&param, VPP_RESOLUTION_HD, get_supported_fmt(pstIp));
    init_vpp(&stCtx);

    ctrl.mode = HQV_MODE_AUTO;

    pv = send_init(pstFnc, &stTestCtx, &stCtx, 0, VPP_TRUE, VPP_TRUE, VPP_TRUE);
    DVP_ASSERT_PTR_NNULL(pv);

    // Test invalid params in INITED state.
    test_get_buffer_requirements_invalid(pstFnc, pv);

    move_from_inited_to_active(pstFnc, pv, ctrl, param);

    // Test invalid params in ACTIVE state.
    test_get_buffer_requirements_invalid(pstFnc, pv);

    // Cleanup.
    u32 = send_close(pstFnc, pv);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    send_term(pstFnc, pv);

    term_vpp(&stCtx);
}

TEST(RegIpApi_GetBufReqInvalid)
{
    foreach_ip_do(TEST_CB_FNC(RegIpApi_GetBufReqInvalid));
}

TEST_CB_DECL(RegIpApi_FlushBasic, pstIp)
{
    /*
     * Call Flush() on input and output port, in INITED and ACTIVE state, with
     * no queued buffers, to see FLUSH_DONE event return.
     */
    uint32_t u32;
    uint32_t u32FlushCnt = 1;
    void *pv = NULL;
    t_StVppCtx stCtx;
    struct vpp_port_param param;
    struct hqv_control ctrl;

    const t_StVppIpFnc *pstFnc = &pstIp->fnc;
    memset(&stCtx, 0, sizeof(t_StVppCtx));
    prep_port(&param, VPP_RESOLUTION_HD, get_supported_fmt(pstIp));
    init_vpp(&stCtx);

    ctrl.mode = HQV_MODE_AUTO;

    // Setup into INITED state.
    pv = send_init(pstFnc, &stTestCtx, &stCtx, 0, VPP_TRUE, VPP_TRUE, VPP_TRUE);
    DVP_ASSERT_PTR_NNULL(pv);

    // Valid to Flush() in INITED state.
    u32 = send_flush(pstFnc, pv, VPP_PORT_INPUT);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    u32 = send_flush(pstFnc, pv, VPP_PORT_OUTPUT);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    wait_for_flush_done(&stTestCtx, VPP_PORT_INPUT, u32FlushCnt);
    wait_for_flush_done(&stTestCtx, VPP_PORT_OUTPUT, u32FlushCnt);

    pthread_mutex_lock(&stTestCtx.mutex);
    DVP_ASSERT_EQUAL(stIpTestCtx.u32FlushDoneInCnt, u32FlushCnt);
    DVP_ASSERT_EQUAL(stIpTestCtx.u32FlushDoneOutCnt, u32FlushCnt);
    stIpTestCtx.u32FlushDoneInCnt = 0;
    stIpTestCtx.u32FlushDoneOutCnt = 0;
    pthread_mutex_unlock(&stTestCtx.mutex);

    // Finish setup into ACTIVE state.
    move_from_inited_to_active(pstFnc, pv, ctrl, param);

    // Valid to Flush() in ACTIVE state.
    u32 = send_flush(pstFnc, pv, VPP_PORT_INPUT);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    u32 = send_flush(pstFnc, pv, VPP_PORT_OUTPUT);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    wait_for_flush_done(&stTestCtx, VPP_PORT_INPUT, u32FlushCnt);
    wait_for_flush_done(&stTestCtx, VPP_PORT_OUTPUT, u32FlushCnt);

    pthread_mutex_lock(&stTestCtx.mutex);
    DVP_ASSERT_EQUAL(stIpTestCtx.u32FlushDoneInCnt, u32FlushCnt);
    DVP_ASSERT_EQUAL(stIpTestCtx.u32FlushDoneOutCnt, u32FlushCnt);
    stIpTestCtx.u32FlushDoneInCnt = 0;
    stIpTestCtx.u32FlushDoneOutCnt = 0;
    pthread_mutex_unlock(&stTestCtx.mutex);

    // Cleanup.
    u32 = send_close(pstFnc, pv);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // Valid to Flush() in INITED state.
    u32 = send_flush(pstFnc, pv, VPP_PORT_INPUT);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    u32 = send_flush(pstFnc, pv, VPP_PORT_OUTPUT);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    wait_for_flush_done(&stTestCtx, VPP_PORT_INPUT, u32FlushCnt);
    wait_for_flush_done(&stTestCtx, VPP_PORT_OUTPUT, u32FlushCnt);

    pthread_mutex_lock(&stTestCtx.mutex);
    DVP_ASSERT_EQUAL(stIpTestCtx.u32FlushDoneInCnt, u32FlushCnt);
    DVP_ASSERT_EQUAL(stIpTestCtx.u32FlushDoneOutCnt, u32FlushCnt);
    stIpTestCtx.u32FlushDoneInCnt = 0;
    stIpTestCtx.u32FlushDoneOutCnt = 0;
    pthread_mutex_unlock(&stTestCtx.mutex);

    send_term(pstFnc, pv);

    term_vpp(&stCtx);
}

TEST(RegIpApi_FlushBasic)
{
    foreach_ip_do(TEST_CB_FNC(RegIpApi_FlushBasic));
}

TEST_CB_DECL(RegIpApi_FlushInput, pstIp)
{
    /*
     * Calls Flush() after queueing only input buffers to the IP block.
     * Expecting all buffers to be flushed back.
     */
    uint32_t u32FlushCnt = 1;
    uint32_t u32BufCnt = 10;

    testcase_flush_buffers(pstIp, &stTestCtx, VPP_PORT_INPUT,
                           u32FlushCnt, u32BufCnt);
}

TEST(RegIpApi_FlushInput)
{
    foreach_ip_do(TEST_CB_FNC(RegIpApi_FlushInput));
}

TEST_CB_DECL(RegIpApi_FlushOutput, pstIp)
{
    /*
     * Calls Flush() after queueing only output buffers to the IP block.
     * Expecting all buffers to be flushed back.
     */
    uint32_t u32FlushCnt = 1;
    uint32_t u32BufCnt = 10;

    testcase_flush_buffers(pstIp, &stTestCtx, VPP_PORT_OUTPUT,
                           u32FlushCnt, u32BufCnt);
}

TEST(RegIpApi_FlushOutput)
{
    foreach_ip_do(TEST_CB_FNC(RegIpApi_FlushOutput));
}

TEST_CB_DECL(RegIpApi_FlushInputMultiple, pstIp)
{
    /*
     * Calls Flush() multiple times after queueing only input buffers.
     * Expecting all buffers to be flushed back, as well as receive multiple
     * FLUSH_DONE events.
     */
    uint32_t u32FlushCnt = 10;
    uint32_t u32BufCnt = 10;

    testcase_flush_buffers(pstIp, &stTestCtx, VPP_PORT_INPUT,
                           u32FlushCnt, u32BufCnt);
}

TEST(RegIpApi_FlushInputMultiple)
{
    foreach_ip_do(TEST_CB_FNC(RegIpApi_FlushInputMultiple));
}

TEST_CB_DECL(RegIpApi_FlushOutputMultiple, pstIp)
{
    /*
     * Calls Flush() multiple times after queueing only output buffers.
     * Expecting all buffers to be flushed back, as well as receive multiple
     * FLUSH_DONE events.
     */
    uint32_t u32FlushCnt = 10;
    uint32_t u32BufCnt = 10;

    testcase_flush_buffers(pstIp, &stTestCtx, VPP_PORT_OUTPUT,
                           u32FlushCnt, u32BufCnt);
}

TEST(RegIpApi_FlushOutputMultiple)
{
    foreach_ip_do(TEST_CB_FNC(RegIpApi_FlushOutputMultiple));
}

TEST_CB_DECL(RegIpApi_FlushInvalid, pstIp)
{
    /*
     * Calls Flush() with invalid parameters for all IP blocks.
     */
    uint32_t u32;
    uint32_t u32FlushCnt = 1;
    void *pv;
    struct hqv_control ctrl;
    struct ip_test_ctx *pstIpTestCtx;
    const t_StVppIpFnc *pstFnc;

    pstFnc = &pstIp->fnc;
    pstIpTestCtx = (struct ip_test_ctx *)stTestCtx.pPrivateCtx;
    ctrl.mode = HQV_MODE_AUTO;
    prep_port(&stTestCtx.port_param, VPP_RESOLUTION_HD,
              get_supported_fmt(pstIp));
    memset(&stTestCtx.stVppCtx, 0, sizeof(t_StVppCtx));
    init_vpp(&stTestCtx.stVppCtx);

    pv = send_init(pstFnc, &stTestCtx, &stTestCtx.stVppCtx, 0, VPP_TRUE,
                   VPP_TRUE, VPP_TRUE);
    DVP_ASSERT_PTR_NNULL(pv);

    // Invalid cases in INITED state.
    test_flush_invalid(pstFnc, pv);

    move_from_inited_to_active(pstFnc, pv, ctrl, stTestCtx.port_param);

    // Invalid cases in ACTIVE state.
    test_flush_invalid(pstFnc, pv);

    // Valid case.
    u32 = send_flush(pstFnc, pv, VPP_PORT_INPUT);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    wait_for_flush_done(&stTestCtx, VPP_PORT_INPUT, u32FlushCnt);

    pthread_mutex_lock(&stTestCtx.mutex);
    DVP_ASSERT_EQUAL(pstIpTestCtx->u32FlushDoneInCnt, u32FlushCnt);
    pthread_mutex_unlock(&stTestCtx.mutex);

    term_test(pstFnc, &stTestCtx, pv, VPP_FALSE);
}

TEST(RegIpApi_FlushInvalid)
{
    foreach_ip_do(TEST_CB_FNC(RegIpApi_FlushInvalid));
}

TEST_CB_DECL(RegIpApi_DrainBasic, pstIp)
{
    /*
     * Calls Drain() in ACTIVE state with valid parameters, with no buffers
     * queued, to see the DRAIN_DONE event return successfully.
     */
    testcase_drain_no_buffers(pstIp, &stTestCtx, 1);
}

TEST(RegIpApi_DrainBasic)
{
    foreach_ip_do(TEST_CB_FNC(RegIpApi_DrainBasic));
}

TEST_CB_DECL(RegIpApi_DrainMultiple, pstIp)
{
    /*
     * Call Drain() multiple times, with no queued buffers, to see multiple
     * DRAIN_DONE events.
     */
    testcase_drain_no_buffers(pstIp, &stTestCtx, 10);
}

TEST(RegIpApi_DrainMultiple)
{
    foreach_ip_do(TEST_CB_FNC(RegIpApi_DrainMultiple));
}

TEST_CB_DECL(RegIpApi_DrainWithBuffers, pstIp)
{
    /*
     * Call Drain() after queueing some input buffers, and then start queueing
     * output buffers to be processed. Expecting the DRAIN_DONE event after
     * the last input buffer is returned.
     */
    uint32_t u32;
    uint32_t u32Cnt;
    uint32_t u32InBufCnt = 5;
    uint32_t u32OutBufCnt = u32InBufCnt;
    void *pv;
    struct hqv_control ctrl;
    const t_StVppIpFnc *pstFnc = &pstIp->fnc;

    ctrl.mode = HQV_MODE_AUTO;
    prep_port(&stTestCtx.port_param, VPP_RESOLUTION_HD,
              get_supported_fmt(pstIp));
    pv = init_test(pstFnc, &stTestCtx, ctrl, stTestCtx.port_param);
    DVP_ASSERT_PTR_NNULL(pv);

    // Bypass is expected in FRC, because it bypasses half of its buffers.
    if (pstIp->eIp == VPP_IP_FRC)
    {
        pthread_mutex_lock(&stTestCtx.mutex);
        stIpTestCtx.bBypassExpected = VPP_TRUE;
        pthread_mutex_unlock(&stTestCtx.mutex);
    }

    u32 = init_tctx_buf_pool(&stTestCtx, u32InBufCnt + u32OutBufCnt, VPP_FALSE);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    for (u32Cnt = 0; u32Cnt < u32InBufCnt; u32Cnt++)
    {
        u32 = get_and_send_buf(pstFnc, &stTestCtx, pv, VPP_PORT_INPUT,
                               eVppBufType_Progressive, 0, 0, &stTestCtx.mutex);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
    }

    u32 = send_drain(pstFnc, pv);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    pthread_mutex_lock(&stTestCtx.mutex);
    while (stTestCtx.u32InHoldCnt > 0)
    {
        if (stTestCtx.u32OutHoldCnt < u32OutBufCnt)
        {
            u32 = get_and_send_buf(pstFnc, &stTestCtx, pv, VPP_PORT_OUTPUT,
                                   eVppBufType_Progressive, 0, 0, NULL);
            DVP_ASSERT_EQUAL(u32, VPP_OK);
        }
        else
            pthread_cond_wait(&stTestCtx.cond, &stTestCtx.mutex);
    }
    pthread_mutex_unlock(&stTestCtx.mutex);

    wait_for_drain_done(&stTestCtx, 1);

    u32 = send_flush(pstFnc, pv, VPP_PORT_OUTPUT);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    wait_for_flush_done(&stTestCtx, VPP_PORT_OUTPUT, 1);

    term_test(pstFnc, &stTestCtx, pv, VPP_TRUE);
}

TEST(RegIpApi_DrainWithBuffers)
{
    foreach_ip_do(TEST_CB_FNC(RegIpApi_DrainWithBuffers));
}

TEST_CB_DECL(RegIpApi_DrainInvalid, pstIp)
{
    /*
     * Call Drain() with invalid parameters, in invalid states.
     */
    uint32_t u32;
    void *pv;
    const t_StVppIpFnc *pstFnc;
    struct hqv_control ctrl;

    pstFnc = &pstIp->fnc;
    ctrl.mode = HQV_MODE_AUTO;
    prep_port(&stTestCtx.port_param, VPP_RESOLUTION_HD,
              get_supported_fmt(pstIp));
    memset(&stTestCtx.stVppCtx, 0, sizeof(t_StVppCtx));
    init_vpp(&stTestCtx.stVppCtx);

    pv = send_init(pstFnc, &stTestCtx, &stTestCtx.stVppCtx, 0, VPP_TRUE,
                   VPP_TRUE, VPP_TRUE);
    DVP_ASSERT_PTR_NNULL(pv);

    // Invalid case: in INITED state.
    u32 = send_drain(pstFnc, pv);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_STATE);

    move_from_inited_to_active(pstFnc, pv, ctrl, stTestCtx.port_param);

    // Invalid case: null ctx pointer.
    u32 = send_drain(pstFnc, NULL);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);

    // Valid case.
    u32 = send_drain(pstFnc, pv);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    wait_for_drain_done(&stTestCtx, 1);

    u32 = send_close(pstFnc, pv);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // Invalid case: after close(), in INITED state.
    u32 = send_drain(pstFnc, pv);
    DVP_ASSERT_EQUAL(u32, VPP_ERR_STATE);

    send_term(pstFnc, pv);

    term_vpp(&stTestCtx.stVppCtx);
}

TEST(RegIpApi_DrainInvalid)
{
    foreach_ip_do(TEST_CB_FNC(RegIpApi_DrainInvalid));
}

TEST_CB_DECL(RegIpApi_ReconfigureBasic, pstIp)
{
    /*
     * Call Reconfigure() with valid parameters in a valid condition. Only runs
     * if the IP block has a Reconfigure() implemented. If not, will skip.
     */
    uint32_t u32;
    void *pv;
    struct hqv_control ctrl;
    const t_StVppIpFnc *pstFnc = &pstIp->fnc;

    if (pstFnc->reconfigure)
    {
        ctrl.mode = HQV_MODE_AUTO;
        memset(&stTestCtx.stVppCtx, 0, sizeof(t_StVppCtx));
        prep_port(&stTestCtx.port_param, VPP_RESOLUTION_HD,
                  get_supported_fmt(pstIp));
        init_vpp(&stTestCtx.stVppCtx);

        pv = init_test(pstFnc, &stTestCtx, ctrl, stTestCtx.port_param);
        DVP_ASSERT_PTR_NNULL(pv);

        u32 = send_reconfigure(pstFnc, pv, stTestCtx.port_param,
                               stTestCtx.port_param);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        term_test(pstFnc, &stTestCtx, pv, VPP_FALSE);
    }
}

TEST(RegIpApi_ReconfigureBasic)
{
    foreach_ip_do(TEST_CB_FNC(RegIpApi_ReconfigureBasic));
}

TEST_CB_DECL(RegIpApi_ReconfigureInvalid, pstIp)
{
    /*
     * Call Reconfigure() with valid parameters in a valid condition. Only runs
     * if the IP block has a Reconfigure() implemented. If not, will skip.
     */
    uint32_t u32;
    uint32_t u32Temp;
    void *pv;
    enum vpp_port ePort = VPP_PORT_INPUT;
    struct hqv_control ctrl;
    struct bufnode *pstNode;
    const t_StVppIpFnc *pstFnc = &pstIp->fnc;

    if (pstFnc->reconfigure)
    {
        ctrl.mode = HQV_MODE_AUTO;
        memset(&stTestCtx.stVppCtx, 0, sizeof(t_StVppCtx));
        prep_port(&stTestCtx.port_param, VPP_RESOLUTION_HD,
                  get_supported_fmt(pstIp));
        init_vpp(&stTestCtx.stVppCtx);

        u32 = init_tctx_buf_pool(&stTestCtx, 1, VPP_FALSE);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        pstNode = get_filled_buf(&stTestCtx, ePort, eVppBufType_Progressive,
                                 0, 0, &stTestCtx.mutex);
        DVP_ASSERT_PTR_NNULL(pstNode);

        pv = send_init(pstFnc, &stTestCtx, &stTestCtx.stVppCtx, 0, VPP_TRUE,
                       VPP_TRUE, VPP_TRUE);
        DVP_ASSERT_PTR_NNULL(pv);

        // Invalid case: in INITED state.
        u32 = send_reconfigure(pstFnc, pv, stTestCtx.port_param,
                               stTestCtx.port_param);
        DVP_ASSERT_EQUAL(u32, VPP_ERR_STATE);

        move_from_inited_to_active(pstFnc, pv, ctrl, stTestCtx.port_param);

        // Invalid case: null ctx pointer.
        u32 = send_reconfigure(pstFnc, NULL, stTestCtx.port_param,
                               stTestCtx.port_param);
        DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);

        // Invalid case: invalid colour format.
        u32Temp = stTestCtx.port_param.fmt;
        stTestCtx.port_param.fmt = VPP_COLOR_FORMAT_MAX;
        u32 = send_reconfigure(pstFnc, pv, stTestCtx.port_param,
                               stTestCtx.port_param);
        DVP_ASSERT_EQUAL(u32, VPP_ERR_PARAM);
        stTestCtx.port_param.fmt = u32Temp;

        // Invalid case: with an input buffer queued.
        u32 = send_queue_buf(pstFnc, pv, ePort, pstNode->pIntBuf);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        u32 = send_reconfigure(pstFnc, pv, stTestCtx.port_param,
                               stTestCtx.port_param);
        DVP_ASSERT_EQUAL(u32, VPP_ERR_STATE);

        u32 = send_flush(pstFnc, pv, ePort);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
        wait_for_flush_done(&stTestCtx, ePort, 1);

        u32 = send_close(pstFnc, pv);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        // Invalid case: in INITED state, after Close().
        u32 = send_reconfigure(pstFnc, pv, stTestCtx.port_param,
                               stTestCtx.port_param);
        DVP_ASSERT_EQUAL(u32, VPP_ERR_STATE);

        send_term(pstFnc, pv);

        u32 = free_tctx_buf_pool(&stTestCtx, VPP_FALSE);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        term_vpp(&stTestCtx.stVppCtx);
    }
}

TEST(RegIpApi_ReconfigureInvalid)
{
    foreach_ip_do(TEST_CB_FNC(RegIpApi_ReconfigureInvalid));
}

/************************************************************************
 * Global Functions
 ***********************************************************************/
TEST_CASES RegTests[] = {
    TEST_CASE(RegIpApi_ValidateFncPtrs),
    TEST_CASE(RegIpApi_InitTermBasic),
    TEST_CASE(RegIpApi_InitTerm),
    TEST_CASE(RegIpApi_InitOpenCfgCloseTerm),
    TEST_CASE(RegIpApi_OpenCloseOpenClose),
    TEST_CASE(RegIpApi_OpenWithoutSetCtrl),
    TEST_CASE(RegIpApi_OpenCloseInvalid),
    TEST_CASE(RegIpApi_SetCtrlInvalid),
    TEST_CASE(RegIpApi_ResetCtrlBasic),
    TEST_CASE(RegIpApi_ResetCtrlInvalid),
    TEST_CASE(RegIpApi_SetParamInvalid),
    TEST_CASE(RegIpApi_TermWithoutOpen),
    TEST_CASE(RegIpApi_TermWithoutClose),
    TEST_CASE(RegIpApi_TermWithInputBuffers),
    TEST_CASE(RegIpApi_TermWithOutputBuffers),
    TEST_CASE(RegIpApi_QueueBufBypassFlag),
    TEST_CASE(RegIpApi_QueueBufBypassFilledLenZero),
    TEST_CASE(RegIpApi_QueueBufInvalid),
    TEST_CASE(RegIpApi_GetBufReqBasic),
    TEST_CASE(RegIpApi_GetBufReqValid),
    TEST_CASE(RegIpApi_GetBufReqInvalid),
    TEST_CASE(RegIpApi_FlushBasic),
    TEST_CASE(RegIpApi_FlushInput),
    TEST_CASE(RegIpApi_FlushOutput),
    TEST_CASE(RegIpApi_FlushInputMultiple),
    TEST_CASE(RegIpApi_FlushOutputMultiple),
    TEST_CASE(RegIpApi_FlushInvalid),
    TEST_CASE(RegIpApi_DrainBasic),
    TEST_CASE(RegIpApi_DrainMultiple),
    TEST_CASE(RegIpApi_DrainWithBuffers),
    TEST_CASE(RegIpApi_DrainInvalid),
    TEST_CASE(RegIpApi_ReconfigureBasic),
    TEST_CASE(RegIpApi_ReconfigureInvalid),
    TEST_CASE_NULL(),
};

TEST_SUITE(RegSuite,
           "RegTests",
           RegSuiteInit,
           RegSuiteTerm,
           RegTestInit,
           RegTestTerm,
           RegTests);
