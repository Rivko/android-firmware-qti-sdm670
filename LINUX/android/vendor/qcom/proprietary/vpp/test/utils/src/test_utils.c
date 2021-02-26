/*!
 * @file test_utils.c
 *
 * @cr
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.

 * @services
 *
 *
 */

#include <inttypes.h>
#include <sys/types.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include "dvpTest.h"
#include "dvpTest_tb.h"

#include "vpp_dbg.h"
#include "vpp_def.h"
#include "vpp_utils.h"

#include "test_utils.h"
#include "vpp_queue.h"

/************************************************************************
 * Local definitions
 ***********************************************************************/

#define VPP_LOG_LVL_POOL    VPP_LOG_LVL_INFO
#define VPP_LOG_LVL_MOCK    VPP_LOG_LVL_DEBUG
#define VPP_LOG_LVL_OTHER   VPP_LOG_LVL_DEBUG

#define VPP_LOG(which) (VPP_LOG_LVL_##which << (VPP_LOG_FLD_WID*VPP_LOG_TEST_UTILS_##which))

/************************************************************************
 * Local static variables
 ***********************************************************************/
uint64_t u64LogLevelTestUtils = VPP_LOG(POOL) | VPP_LOG(MOCK) | VPP_LOG(OTHER);
/************************************************************************
 * Forward Declarations
 ************************************************************************/

/************************************************************************
 * Local Functions
 ***********************************************************************/

/************************************************************************
 * Timestamp Manager Global Functions
 ***********************************************************************/
#define ERR_TS (-1)
uint32_t ts_mgr_init(struct ts_mgr *pTsMgr, enum ts_freq freq, uint32_t u32Cnt)
{
    int rc;
    int32_t s32;

    if (!pTsMgr)
        return VPP_ERR;

    memset(pTsMgr, 0, sizeof(struct ts_mgr));

    rc = pthread_mutex_init(&pTsMgr->mutex, NULL);
    if (rc)
    {
        LOGE("unable to init mutex");
        goto ERR_MUTEX_INIT;
    }

    s32 = vpp_queue_init(&pTsMgr->stQ, u32Cnt);
    if (s32 != 0)
    {
        LOGE("unable to init queue");
        goto ERR_MUTEX_INIT;
    }

    pTsMgr->pu64Ts = calloc(u32Cnt, sizeof(uint64_t));
    if (!pTsMgr)
    {
        LOGE("unable to allocate nodes");
        goto ERR_MALLOC;
    }

    pTsMgr->freq = freq;
    pTsMgr->u32OuttoInRatio = 1;
    pTsMgr->u64TsStep = 1000000 / pTsMgr->freq;

    return VPP_OK;

ERR_MALLOC:
    vpp_queue_term(&pTsMgr->stQ);
    pthread_mutex_destroy(&pTsMgr->mutex);

ERR_MUTEX_INIT:
    return VPP_ERR;
}

void ts_mgr_destroy(struct ts_mgr *pTsMgr)
{
    if (!pTsMgr)
        return;

    vpp_queue_term(&pTsMgr->stQ);

    pthread_mutex_destroy(&pTsMgr->mutex);

    if (pTsMgr->pu64Ts)
        free (pTsMgr->pu64Ts);
}

uint64_t ts_get_next(struct ts_mgr *pTsMgr)
{
    int32_t s32Idx = -1;
    uint32_t i;
    uint64_t u64Ts = 0;

    if(!pTsMgr)
        return ERR_TS;

    pthread_mutex_lock(&pTsMgr->mutex);

    if (pTsMgr->bEnableAsserts)
    {
        s32Idx = vpp_queue_enqueue(&pTsMgr->stQ);
        if (s32Idx < 0)
        {
            pthread_mutex_unlock(&pTsMgr->mutex);
            LOGE("ts dunable to get queue index, idx=%d", s32Idx);
            return ERR_TS;
        }
    }

    pTsMgr->u64CurrentTs += pTsMgr->u64TsStep;
    LOGI("getting timestamp: ts=%"PRIu64"", pTsMgr->u64CurrentTs);

    if (pTsMgr->bEnableAsserts)
    {
        pTsMgr->pu64Ts[s32Idx] = pTsMgr->u64CurrentTs;

        for(i = 1; i < (pTsMgr->u32OuttoInRatio); i++)
        {
            s32Idx = vpp_queue_enqueue(&pTsMgr->stQ);
            if (s32Idx < 0)
            {
                pthread_mutex_unlock(&pTsMgr->mutex);
                LOGE("ts unable to get queue index, idx=%d", s32Idx);
                return ERR_TS;
            }
            pTsMgr->pu64Ts[s32Idx] = pTsMgr->u64CurrentTs + (pTsMgr->u64TsStep * i) / (pTsMgr->u32OuttoInRatio);

        }
    }

    u64Ts = pTsMgr->u64CurrentTs;

    pthread_mutex_unlock(&pTsMgr->mutex);

    return u64Ts;
}

uint32_t ts_put_ts(struct ts_mgr *pTsMgr, uint64_t u64Ts)
{
    int32_t s32Idx;
    uint32_t bEqual;
    uint64_t u64Expected, u64Expected2, u64Peek;

    if(!pTsMgr)
        return ERR_TS;

    if (!pTsMgr->bEnableAsserts)
        return VPP_OK;

    pthread_mutex_lock(&pTsMgr->mutex);
    s32Idx = vpp_queue_dequeue(&pTsMgr->stQ);
    if (s32Idx < 0)
    {
        pthread_mutex_unlock(&pTsMgr->mutex);
        LOGE("ts unable to get dequeue index, idx=%d", s32Idx);
        return ERR_TS;
    }

    pthread_mutex_unlock(&pTsMgr->mutex);
    u64Expected = pTsMgr->pu64Ts[s32Idx];
    bEqual = u64Expected == u64Ts ? VPP_TRUE : VPP_FALSE;

    if(bEqual != VPP_TRUE)
    {
        u64Expected2 = pTsMgr->u64TsLastDequeue + 1;
        bEqual = u64Expected2 == u64Ts ? VPP_TRUE : VPP_FALSE;
        if(bEqual != VPP_TRUE)
        {
                s32Idx = vpp_queue_peek_head(&pTsMgr->stQ);
                if (s32Idx >= 0)
                {
                    u64Peek = pTsMgr->pu64Ts[s32Idx];
                    u64Expected2 = (pTsMgr->u64TsLastDequeue + u64Peek)/2;
                    bEqual = u64Expected2 == u64Ts ? VPP_TRUE : VPP_FALSE;
                }
        }
    }
    pTsMgr->u64TsLastDequeue = u64Expected;

    DVP_ASSERT_TRUE(bEqual);

    LOGE_IF(bEqual != VPP_TRUE, "expected ts=%"PRIu64", got ts=%"PRIu64"",
            u64Expected, u64Ts);

    return bEqual;
}

uint32_t ts_get_freq(struct ts_mgr *pTsMgr)
{
    if(!pTsMgr)
        return 0;

    return pTsMgr->freq;
}

/************************************************************************
 * Flag Manager Global Functions
 ***********************************************************************/
uint32_t flag_mgr_init(struct flag_mgr *pFlagMgr, uint32_t u32Cnt)
{
    int rc;
    int32_t s32;

    if (!pFlagMgr)
        return VPP_ERR;

    memset(pFlagMgr, 0, sizeof(struct flag_mgr));

    rc = pthread_mutex_init(&pFlagMgr->mutex, NULL);
    if (rc)
    {
        LOGE("unable to init mutex");
        goto ERR_MUTEX_INIT;
    }

    s32 = vpp_queue_init(&pFlagMgr->stQ, u32Cnt);
    if (s32 != 0)
    {
        LOGE("unable to init queue");
        goto ERR_MUTEX_INIT;
    }

    pFlagMgr->pu32Flags = calloc(u32Cnt, sizeof(uint32_t));
    if (!pFlagMgr->pu32Flags)
    {
        LOGE("unable to allocate flag nodes");
        goto ERR_MALLOC;
    }

    pFlagMgr->pu32ExpectedBuffer = calloc(u32Cnt, sizeof(uint32_t));
    if (!pFlagMgr->pu32ExpectedBuffer)
    {
        LOGE("unable to allocate buffer count nodes");
        goto ERR_MALLOC;
    }

    return VPP_OK;

ERR_MALLOC:
    vpp_queue_term(&pFlagMgr->stQ);
    pthread_mutex_destroy(&pFlagMgr->mutex);

ERR_MUTEX_INIT:
    return VPP_ERR;
}

void flag_mgr_destroy(struct flag_mgr *pFlagMgr)
{
    if (!pFlagMgr)
        return;

    vpp_queue_term(&pFlagMgr->stQ);

    pthread_mutex_destroy(&pFlagMgr->mutex);

    if (pFlagMgr->pu32Flags)
        free (pFlagMgr->pu32Flags);

    if (pFlagMgr->pu32ExpectedBuffer)
        free (pFlagMgr->pu32ExpectedBuffer);
}

uint32_t flag_queue(struct flag_mgr *pFlagMgr, uint32_t u32Flags, uint32_t u32ExpectedBuf)
{
    int32_t s32Idx;

    if(!pFlagMgr)
        return VPP_ERR;
    pthread_mutex_lock(&pFlagMgr->mutex);
    s32Idx = vpp_queue_enqueue(&pFlagMgr->stQ);
    if (s32Idx < 0)
    {
        pthread_mutex_unlock(&pFlagMgr->mutex);
        LOGE("flags unable to get queue index, idx=%d", s32Idx);
        return VPP_ERR;
    }
    pFlagMgr->pu32Flags[s32Idx] = u32Flags;
    pFlagMgr->pu32ExpectedBuffer[s32Idx] = u32ExpectedBuf;

    pthread_mutex_unlock(&pFlagMgr->mutex);

    LOGD("Queued index %d: flags %d, expecting output buffer %d", s32Idx, u32Flags, u32ExpectedBuf);

    return VPP_OK;
}

uint32_t flag_dequeue(struct flag_mgr *pFlagMgr, uint32_t u32Flags, uint32_t u32Buf)
{
    int32_t s32Idx;
    uint32_t u32ExpectedFlags, u32ExpectedBuf;

    if(!pFlagMgr)
        return VPP_ERR;

    pthread_mutex_lock(&pFlagMgr->mutex);
    s32Idx = vpp_queue_dequeue(&pFlagMgr->stQ);
    if (s32Idx < 0)
    {
        pthread_mutex_unlock(&pFlagMgr->mutex);
        LOGE("flags unable to get dequeue index, idx=%d", s32Idx);
        return VPP_ERR;
    }
    u32ExpectedFlags = pFlagMgr->pu32Flags[s32Idx];
    u32ExpectedBuf = pFlagMgr->pu32ExpectedBuffer[s32Idx];

    pthread_mutex_unlock(&pFlagMgr->mutex);

    DVP_ASSERT_EQUAL(u32ExpectedFlags, u32Flags);
    DVP_ASSERT_EQUAL(u32ExpectedBuf, u32Buf);

    if((u32ExpectedFlags != u32Flags) || (u32ExpectedBuf != u32Buf))
    {
        LOGE("ERROR! Expected index %d: flags %d on buffer %d! Received flags %d on buffer %d",
             s32Idx, u32ExpectedFlags, u32ExpectedBuf, u32Flags, u32Buf);
        return VPP_ERR;
    }
    else
    {
        LOGD("Received flags %d on output buffer %d",
             u32Flags, u32Buf);
        return VPP_OK;
    }

}

/************************************************************************
 * Frame Type Manager Global Functions
 ***********************************************************************/
uint32_t frametype_mgr_init(struct frame_mgr *pFrameMgr, uint32_t u32Cnt)
{
    int rc;
    int32_t s32;

    if (!pFrameMgr)
        return VPP_ERR;

    memset(pFrameMgr, 0, sizeof(struct frame_mgr));

    rc = pthread_mutex_init(&pFrameMgr->mutex, NULL);
    if (rc)
    {
        LOGE("unable to init mutex");
        goto ERR_MUTEX_INIT;
    }

    s32 = vpp_queue_init(&pFrameMgr->stQ, u32Cnt);
    if (s32 != 0)
    {
        LOGE("unable to init queue");
        goto ERR_MUTEX_INIT;
    }

    pFrameMgr->pu32FrameType = calloc(u32Cnt, sizeof(uint32_t));
    if (!pFrameMgr->pu32FrameType)
    {
        LOGE("unable to allocate frame type nodes");
        goto ERR_MALLOC;
    }

    return VPP_OK;

ERR_MALLOC:
    vpp_queue_term(&pFrameMgr->stQ);
    pthread_mutex_destroy(&pFrameMgr->mutex);

ERR_MUTEX_INIT:
    return VPP_ERR;
}

void frametype_mgr_destroy(struct frame_mgr *pFrameMgr)
{
    if (!pFrameMgr)
        return;

    vpp_queue_term(&pFrameMgr->stQ);

    pthread_mutex_destroy(&pFrameMgr->mutex);

    if (pFrameMgr->pu32FrameType)
        free (pFrameMgr->pu32FrameType);

}

uint32_t frametype_queue(struct frame_mgr *pFrameMgr, uint32_t u32FrameType)
{
    int32_t s32Idx;

    if(!pFrameMgr)
        return VPP_ERR;
    pthread_mutex_lock(&pFrameMgr->mutex);
    s32Idx = vpp_queue_enqueue(&pFrameMgr->stQ);
    if (s32Idx < 0)
    {
        pthread_mutex_unlock(&pFrameMgr->mutex);
        LOGE("frames unable to get queue index, idx=%d", s32Idx);
        return VPP_ERR;
    }
    pFrameMgr->pu32FrameType[s32Idx] = u32FrameType;

    pthread_mutex_unlock(&pFrameMgr->mutex);

    LOGD("Queued index %d: frame type %d", s32Idx, u32FrameType);

    return VPP_OK;
}

uint32_t frametype_dequeue(struct frame_mgr *pFrameMgr, uint32_t u32FrameType, uint32_t bIgnoreMatch)
{
    int32_t s32Idx;
    uint32_t u32ExpectedFrameType;

    if(!pFrameMgr)
        return VPP_ERR;

    pthread_mutex_lock(&pFrameMgr->mutex);
    s32Idx = vpp_queue_dequeue(&pFrameMgr->stQ);
    if (s32Idx < 0)
    {
        pthread_mutex_unlock(&pFrameMgr->mutex);
        LOGE("frames unable to get dequeue index, idx=%d", s32Idx);
        return VPP_ERR;
    }
    u32ExpectedFrameType = pFrameMgr->pu32FrameType[s32Idx];

    pthread_mutex_unlock(&pFrameMgr->mutex);


    if((u32ExpectedFrameType != u32FrameType) && (bIgnoreMatch == 0))
    {
        DVP_ASSERT_EQUAL(u32ExpectedFrameType, u32FrameType);
        LOGE("ERROR! Index %d: Expected Frametype %d! Received Frametype %d",
             s32Idx, u32ExpectedFrameType, u32FrameType);
        return VPP_ERR;
    }
    else
    {
        LOGD("Received Frametype %d", u32FrameType);
        return VPP_OK;
    }
}

/************************************************************************
 * Global Functions
 ***********************************************************************/
uint32_t tctx_common_init(struct test_ctx *pCtx)
{
    int rc;

    if (pCtx == NULL)
    {
        LOGE("%s(): NULL poter!", __func__);
        return VPP_ERR;
    }

    memset((void*)pCtx, 0, sizeof(struct test_ctx));

    rc = pthread_mutex_init(&pCtx->mutex, NULL);
    if (rc)
        goto ERR_MUTEX_INIT;

    rc = pthread_cond_init(&pCtx->cond, NULL);
    if (rc)
        goto ERR_COND_INIT;

    return VPP_OK;

ERR_COND_INIT:
    pthread_mutex_destroy(&pCtx->mutex);

ERR_MUTEX_INIT:
    return VPP_ERR;
}

uint32_t tctx_common_destroy(struct test_ctx *pCtx)
{
    if (!pCtx)
        return VPP_OK;

    pthread_cond_destroy(&pCtx->cond);
    pthread_mutex_destroy(&pCtx->mutex);

    return VPP_OK;
}

uint32_t tctx_common_reset_cnt(struct test_ctx *pCtx)
{
    if (!pCtx)
        return VPP_OK;

    pCtx->u32InRxCnt = 0;
    pCtx->u32OutRxCnt = 0;
    pCtx->u32InPutCnt = 0;
    pCtx->u32OutPutCnt = 0;
    pCtx->u32InHoldCnt = 0;
    pCtx->u32OutHoldCnt = 0;
    pCtx->u32InEosRxCnt = 0;
    pCtx->u32OutEosRxCnt = 0;

    return VPP_OK;
}

void set_port_params(struct vpp_port_param *p,
                     uint32_t w, uint32_t h, enum vpp_color_format fmt)
{
    if (!p)
        return;

    p->width = w;
    p->height = h;
    p->stride = u32VppUtils_CalculateStride(w, fmt);
    p->scanlines = u32VppUtils_CalculateScanlines(h, fmt);
    p->fmt = fmt;
}

void tctx_set_port_params(struct test_ctx *pCtx, uint32_t width,
                          uint32_t height, enum vpp_color_format fmt)
{
    if (!pCtx)
        return;

    set_port_params(&pCtx->port_param, width, height, fmt);
}

uint32_t get_session_flags(struct test_ctx *pCtx)
{
    uint32_t u32Flags = 0;

    if (pCtx->eProtection == PROTECTION_ZONE_SECURE)
        u32Flags |= VPP_SESSION_SECURE;

    return u32Flags;
}
uint32_t u32GetMaxWidthForRes(t_EVppRes eRes)
{
    uint32_t u32Max = 0;
    switch (eRes)
    {
        case VPP_RESOLUTION_SD:     u32Max = RES_SD_MAX_W;  break;
        case VPP_RESOLUTION_HD:     u32Max = RES_HD_MAX_W;  break;
        case VPP_RESOLUTION_FHD:    u32Max = RES_FHD_MAX_W; break;
        case VPP_RESOLUTION_UHD:    u32Max = RES_UHD_MAX_W; break;
        case VPP_RESOLUTION_MAX:
        default:
            break;
    }

    LOGI("returning width=%u", u32Max);
    return u32Max;
}

uint32_t u32GetMaxHeightForRes(t_EVppRes eRes)
{
    uint32_t u32Max = 0;
    switch (eRes)
    {
        case VPP_RESOLUTION_SD:     u32Max = RES_SD_MAX_H;  break;
        case VPP_RESOLUTION_HD:     u32Max = RES_HD_MAX_H;  break;
        case VPP_RESOLUTION_FHD:    u32Max = RES_FHD_MAX_H; break;
        case VPP_RESOLUTION_UHD:    u32Max = RES_UHD_MAX_H; break;
        case VPP_RESOLUTION_MAX:
        default:
            break;
    }

    LOGI("returning height=%u", u32Max);
    return u32Max;
}

uint32_t u32GetMulFactor(struct vpp_port_param *pstParam, struct vpp_requirements *pstReq)
{
    uint32_t u32Resolution;
    uint32_t u32MulFactor = 1;

    if (!pstParam || !pstReq)
    {
        LOGE("%s: Null input: pstParam %p, pstReq %p", __func__, pstParam, pstReq);
        DVP_ASSERT_FAIL();
        return u32MulFactor;
    }

    u32Resolution = u32VppUtils_GetVppResolution(pstParam);
    if (u32Resolution >= VPP_RESOLUTION_MAX)
    {
        LOGE("Resolution %u x %u not supported", pstParam->width, pstParam->height);
        DVP_ASSERT_FAIL();
    }
    else
        u32MulFactor = pstReq->out_factor[u32Resolution].mul;

    return u32MulFactor;
}
