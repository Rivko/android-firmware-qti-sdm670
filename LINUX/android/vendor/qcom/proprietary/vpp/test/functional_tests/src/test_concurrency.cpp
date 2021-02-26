/*!
 * @file test_concurrency.cpp
 *
 * @cr
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.

 * @services    <Describe what this is supposed to test>
 */

#include <sys/types.h>
#include <inttypes.h>
#include <string.h>
#include <stdio.h>

#include "dvpTest.h"
#include "dvpTest_tb.h"

#include "test_utils.h"
#include "buf_pool.h"

#define VPP_LOG_TAG     VPP_LOG_FT_CONCURRENCY_TAG
#define VPP_LOG_MODULE  VPP_LOG_FT_CONCURRENCY
#include "vpp_dbg.h"
#include "vpp.h"
#include "vpp_utils.h"
#include "test_stub.h"

/************************************************************************
 * Local definitions
 ***********************************************************************/
#define BUFFER_POOL_SZ 20

#define FUNCTEST_CONC_BASIC           (1 << 16)

enum
{
    OUT_FLUSH_DONE,
    OUT_FLUSH_REQUEST,
    OUT_FLUSH_ACTIVE,
};

#define BYPASS_START       (1 << 0)
#define BYPASS_STOP        (1 << 1)
#define BYPASS_FORCE       (1 << 2)
#define BYPASS_FRAMERATE   (1 << 3)
#define BYPASS_DI_ON       (1 << 4)

/************************************************************************
 * Local static variables
 ***********************************************************************/
extern struct test_ctx stTestCtx;
extern struct test_ctx *pstTestCtx;

// TODO: move this into test_ctx (but need to first get a way to get from
// a struct bufnode to test_ctx.
static struct ts_mgr tsmgr1;
static struct ts_mgr tsmgr2;
static struct frame_mgr frametypemgr1;
static struct frame_mgr frametypemgr2;
// static struct flag_mgr flagmgr;

/************************************************************************
 * Forward Declarations
 ************************************************************************/

/************************************************************************
 * Local Functions
 ***********************************************************************/
#define DUMP_MEM_BUF(buf) LOGI("-->"#buf"{fd=%d, offset=%u, alloc_len=%u, filled_len=%u", buf.fd, buf.offset, buf.alloc_len, buf.filled_len)
static void dump_vpp_buf(struct vpp_buffer *buf, const char *str)
{
    LOGI("%s: flags=0x%x, ts=%" PRIu64"", str, buf->flags, buf->timestamp);
    DUMP_MEM_BUF(buf->pixel);
    DUMP_MEM_BUF(buf->extradata);
}


/************************************************************************
 * Callback Functions
 ***********************************************************************/
/*
static void buffer_exchange_ibd1_cb(void *pv, struct vpp_buffer *buf)
{
    struct bufnode *pBufNode = (struct bufnode *)buf->cookie;

    pthread_mutex_lock(&pstTestCtx->mutex);
    pstTestCtx->u32InRxCnt += 1;
    pstTestCtx->u32InHoldCnt -= 1;
    pthread_mutex_unlock(&pstTestCtx->mutex);

    pBufNode->pExtBuf->flags = 0;
    LOGI("IBD, InRx %d; InHold %d", pstTestCtx->u32InRxCnt, pstTestCtx->u32InHoldCnt);
    // dump_vpp_buf(buf, "ibd");
    put_buf(pBufNode->pPool, pBufNode);
    pthread_cond_signal(&pstTestCtx->cond);
}

static void buffer_exchange_obd1_cb(void *pv, struct vpp_buffer *buf)
{
    struct bufnode *pBufNode = (struct bufnode *)buf->cookie;
    uint32_t u32;

    pthread_mutex_lock(&pstTestCtx->mutex);

    pstTestCtx->u32OutRxCnt += 1;

    if (pBufNode->port_owner == VPP_PORT_INPUT)
    {
        pstTestCtx->u32InHoldCnt -= 1;
    }
    else if (pBufNode->port_owner == VPP_PORT_OUTPUT)
    {
        pstTestCtx->u32OutHoldCnt -= 1;
    }
    else
    {
        DVP_ASSERT_FAIL();
    }
    LOGI("OBD, OutRx %d; OutHold %d", pstTestCtx->u32OutRxCnt, pstTestCtx->u32OutHoldCnt);

    dump_vpp_buf(buf, "obd");

    if(u32OutFlushFlag == OUT_FLUSH_ACTIVE)
        DVP_ASSERT_EQUAL(pBufNode->pExtBuf->pixel.filled_len, 0);

    if (pBufNode->pExtBuf->pixel.filled_len)
    {
        ts_put_ts(&tsmgr1, pBufNode->pExtBuf->timestamp);
        if(pBufNode->pExtBuf->flags)
        {
            u32 = flag_dequeue(&flagmgr, pBufNode->pExtBuf->flags, (pstTestCtx->u32OutRxCnt - 1));
            DVP_ASSERT_EQUAL(u32, VPP_OK);
            pBufNode->pExtBuf->flags = 0;
        }
        if(pstTestCtx->u32OutRxCnt < (pstTestCtx->u32FramesOut + 1))
        {
            u32 = dump_buf(pBufNode);
            DVP_ASSERT_EQUAL(u32, VPP_OK);
        }
    }
    else
    {
        if(u32OutFlushFlag == OUT_FLUSH_REQUEST)
            u32OutFlushFlag = OUT_FLUSH_ACTIVE;
        else if(u32OutFlushFlag == OUT_FLUSH_DONE)
        {
            // Buffer was returned unfilled (Possible CL error)
            ts_put_ts(&tsmgr1, pBufNode->pExtBuf->timestamp);
            u32ErrorBufferCount++;
            LOGD("Empty output buffer received, error buffer count = %d", u32ErrorBufferCount);
        }

        if((u32OutFlushFlag == OUT_FLUSH_REQUEST) || (u32OutFlushFlag == OUT_FLUSH_ACTIVE))
            LOGD("Flushed output buffer received, out hold count = %d", pstTestCtx->u32OutHoldCnt);
    }
    pthread_mutex_unlock(&pstTestCtx->mutex);

    put_buf(pBufNode->pPool, pBufNode);
    pthread_cond_signal(&pstTestCtx->cond);
}

static void buffer_exchange_ibd2_cb(void *pv, struct vpp_buffer *buf)
{
    struct bufnode *pBufNode = (struct bufnode *)buf->cookie;

    pthread_mutex_lock(&pstTestCtx->mutex);
    pstTestCtx->u32InRxCnt += 1;
    pstTestCtx->u32InHoldCnt -= 1;
    pthread_mutex_unlock(&pstTestCtx->mutex);

    pBufNode->pExtBuf->flags = 0;
    LOGI("IBD, InRx %d; InHold %d", pstTestCtx->u32InRxCnt, pstTestCtx->u32InHoldCnt);
    // dump_vpp_buf(buf, "ibd");
    put_buf(pBufNode->pPool, pBufNode);
    pthread_cond_signal(&pstTestCtx->cond);
}

static void buffer_exchange_obd2_cb(void *pv, struct vpp_buffer *buf)
{
    struct bufnode *pBufNode = (struct bufnode *)buf->cookie;
    uint32_t u32;

    pthread_mutex_lock(&pstTestCtx->mutex);

    pstTestCtx->u32OutRxCnt += 1;

    if (pBufNode->port_owner == VPP_PORT_INPUT)
    {
        pstTestCtx->u32InHoldCnt -= 1;
    }
    else if (pBufNode->port_owner == VPP_PORT_OUTPUT)
    {
        pstTestCtx->u32OutHoldCnt -= 1;
    }
    else
    {
        DVP_ASSERT_FAIL();
    }
    LOGI("OBD, OutRx %d; OutHold %d", pstTestCtx->u32OutRxCnt, pstTestCtx->u32OutHoldCnt);

    dump_vpp_buf(buf, "obd");

    if(u32OutFlushFlag == OUT_FLUSH_ACTIVE)
        DVP_ASSERT_EQUAL(pBufNode->pExtBuf->pixel.filled_len, 0);

    if (pBufNode->pExtBuf->pixel.filled_len)
    {
        ts_put_ts(&tsmgr2, pBufNode->pExtBuf->timestamp);
        if(pBufNode->pExtBuf->flags)
        {
            u32 = flag_dequeue(&flagmgr, pBufNode->pExtBuf->flags, (pstTestCtx->u32OutRxCnt - 1));
            DVP_ASSERT_EQUAL(u32, VPP_OK);
            pBufNode->pExtBuf->flags = 0;
        }
        if(pstTestCtx->u32OutRxCnt < (pstTestCtx->u32FramesOut + 1))
        {
            u32 = dump_buf(pBufNode);
            DVP_ASSERT_EQUAL(u32, VPP_OK);
        }
    }
    else
    {
        if(u32OutFlushFlag == OUT_FLUSH_REQUEST)
            u32OutFlushFlag = OUT_FLUSH_ACTIVE;
        else if(u32OutFlushFlag == OUT_FLUSH_DONE)
        {
            // Buffer was returned unfilled (Possible CL error)
            ts_put_ts(&tsmgr2, pBufNode->pExtBuf->timestamp);
            u32ErrorBufferCount++;
            LOGD("Empty output buffer received, error buffer count = %d", u32ErrorBufferCount);
        }

        if((u32OutFlushFlag == OUT_FLUSH_REQUEST) || (u32OutFlushFlag == OUT_FLUSH_ACTIVE))
            LOGD("Flushed output buffer received, out hold count = %d", pstTestCtx->u32OutHoldCnt);
    }
    pthread_mutex_unlock(&pstTestCtx->mutex);

    put_buf(pBufNode->pPool, pBufNode);
    pthread_cond_signal(&pstTestCtx->cond);
}



static void buffer_exchange_evt_cb(void *pv, struct vpp_event e)
{
    uint32_t u32EventType;
    enum vpp_port port;

    u32EventType = e.type;

    switch(e.type)
    {
        case VPP_EVENT_FLUSH_DONE:
            port = e.flush_done.port;
            if(port == VPP_PORT_INPUT)
            {
                LOGD("Input flush done event received, input hold %d, output hold %d",
                     pstTestCtx->u32InHoldCnt, pstTestCtx->u32OutHoldCnt);
                DVP_ASSERT_EQUAL(pstTestCtx->u32InHoldCnt, 0);
                u32InputFlushDoneFlag = 1;
            }
            else if(port == VPP_PORT_OUTPUT)
            {
                LOGD("Output flush done event received, output hold %d, input hold %d",
                     pstTestCtx->u32OutHoldCnt, pstTestCtx->u32InHoldCnt);
                DVP_ASSERT_EQUAL(pstTestCtx->u32OutHoldCnt, 0);

                pthread_mutex_lock(&pstTestCtx->mutex);
                u32OutFlushFlag = OUT_FLUSH_DONE;
                pthread_mutex_unlock(&pstTestCtx->mutex);
                u32OutputFlushDoneFlag = 1;
            }
            else
            {
                LOGE("Flush done event returned invalid port %d", port);
                DVP_ASSERT_FAIL();
            }
            break;
        case VPP_EVENT_RECONFIG_DONE:
                LOGD("Reconfig done event");
            u32ReconfigDoneFlag = 1;
            pthread_cond_signal(&pstTestCtx->cond);
            break;
        case VPP_EVENT_ERROR:
            break;
        default:
            break;
    }
    pthread_cond_signal(&pstTestCtx->cond);
}
*/


static void buffer_exchange_ibd1_cb(__unused void *pv, struct vpp_buffer *buf)
{
    struct bufnode *pBufNode = (struct bufnode *)buf->cookie;
    struct functional_test_ctx *pFuncTestCtx;
    t_StSessionParams *pstSessionParams;

    pFuncTestCtx = (struct functional_test_ctx *)pstTestCtx->pPrivateCtx;
    pstSessionParams = &pFuncTestCtx->astSessionParams[0];

    pthread_mutex_lock(&pstTestCtx->mutex);
    pstSessionParams->u32InRxCnt += 1;
    pstSessionParams->u32InHoldCnt -= 1;

    if(pstSessionParams->u32BypassStartCount)
    {
        pstSessionParams->u32BypassStartCount--;
        if(pstSessionParams->u32BypassFlag & BYPASS_DI_ON)
            pstSessionParams->u32ObdPending = 2;
        else
            pstSessionParams->u32ObdPending = 1;
        LOGD("Bypass[0] starting in %d buffers", pstSessionParams->u32BypassStartCount);
    }
    if(pstSessionParams->u32BypassStopCount)
    {
        pstSessionParams->u32BypassStopCount--;
        LOGD("Bypass[0] stopping in %d buffers", pstSessionParams->u32BypassStopCount);
    }

    pthread_mutex_unlock(&pstTestCtx->mutex);

    pBufNode->pExtBuf->flags = 0;
    LOGI("IBD[0], InRx %d; InHold %d", pstSessionParams->u32InRxCnt, pstSessionParams->u32InHoldCnt);
    // dump_vpp_buf(buf, "ibd");
    put_buf(pBufNode->pPool, pBufNode);
    pthread_cond_signal(&pstTestCtx->cond);
}

static void buffer_exchange_obd1_cb(__unused void *pv, struct vpp_buffer *buf)
{
    struct bufnode *pBufNode = (struct bufnode *)buf->cookie;
    uint32_t u32;
    struct functional_test_ctx *pFuncTestCtx;
    t_StSessionParams *pstSessionParams;
    t_EVppBufType eFrameType = eVppBufType_Progressive;

    pFuncTestCtx = (struct functional_test_ctx *)pstTestCtx->pPrivateCtx;
    pstSessionParams = &pFuncTestCtx->astSessionParams[0];

    pthread_mutex_lock(&pstTestCtx->mutex);

    pstSessionParams->u32OutRxCnt += 1;

    if((pstSessionParams->u32BypassStopCount == 0) && (pstSessionParams->u32BypassFlag & BYPASS_STOP))
    {
        pstSessionParams->u32BypassFlag &= ~(BYPASS_STOP | BYPASS_START | BYPASS_FRAMERATE | BYPASS_DI_ON);
    }

    if((((pstSessionParams->u32BypassStartCount == 0) && (pstSessionParams->u32BypassFlag & BYPASS_START)
        && (pstSessionParams->u32ObdPending == 0)) || (pstSessionParams->u32BypassFlag & BYPASS_FORCE)) && (pstSessionParams->u32OutFlushFlag == 0))
    {
        DVP_ASSERT_EQUAL((pBufNode->port_owner),VPP_PORT_INPUT);
    }
    else
    {
        if(!pstSessionParams->u32ExpectedErrBufCnt)
            DVP_ASSERT_EQUAL((pBufNode->port_owner),VPP_PORT_OUTPUT);
    }

    if (pBufNode->port_owner == VPP_PORT_INPUT)
    {
        pstSessionParams->u32InHoldCnt -= 1;
        LOGI("Bypassed[0] buffer received");
        if(pstSessionParams->u32BypassFlag & BYPASS_START)
        {
            if(pstSessionParams->u32BypassStopCount)
            {
                pstSessionParams->u32BypassStopCount--;
                LOGD("Bypass[0] stopping in %d buffers", pstSessionParams->u32BypassStopCount);
            }
        }
    }
    else if (pBufNode->port_owner == VPP_PORT_OUTPUT)
    {
        pstSessionParams->u32OutHoldCnt -= 1;
    }
    else
    {
        DVP_ASSERT_FAIL();
    }

    LOGI("OBD[0], OutRx %d; OutHold %d", pstSessionParams->u32OutRxCnt, pstSessionParams->u32OutHoldCnt);

    dump_vpp_buf(buf, "obd");

    if(pstSessionParams->u32OutFlushFlag == OUT_FLUSH_ACTIVE)
        DVP_ASSERT_EQUAL(pBufNode->pExtBuf->pixel.filled_len, 0);

    if (pBufNode->pExtBuf->pixel.filled_len)
    {
        ts_put_ts(&tsmgr1, pBufNode->pExtBuf->timestamp);
        validate_extradata_integrity(pBufNode);
        u32 = u32VppBuf_GetFrameTypeExtradata(pBufNode->pIntBuf, VPP_EXTERNAL_EXTRADATA_TYPE, &eFrameType);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
        if((pstSessionParams->u32ExpectedErrBufCnt) && (pBufNode->port_owner == VPP_PORT_INPUT))
            frametype_dequeue(&frametypemgr1, eFrameType, 1);
        else
            frametype_dequeue(&frametypemgr1, eFrameType, 0);
/*
        if((pBufNode->pExtBuf->flags) & 0x0F)
        {
            u32 = flag_dequeue(&flagmgr, pBufNode->pExtBuf->flags, (pstTestCtx->u32OutRxCnt - 1));
            DVP_ASSERT_EQUAL(u32, VPP_OK);
            pBufNode->pExtBuf->flags = 0;
        }
*/
        if(pstSessionParams->u32OutRxCnt < (pstTestCtx->u32FramesOut + 1))
        {
            u32 = dump_buf(pBufNode);
            // DVP_ASSERT_EQUAL(u32, VPP_OK);
            LOGE_IF(u32 != VPP_OK, "Unable to dump_buf");
        }
    }
    else
    {
        if(pstSessionParams->u32OutFlushFlag == OUT_FLUSH_REQUEST)
            pstSessionParams->u32OutFlushFlag = OUT_FLUSH_ACTIVE;
        else if(pstSessionParams->u32OutFlushFlag == OUT_FLUSH_DONE)
        {
            // Buffer was returned unfilled
            if(!pstSessionParams->u32ExpectedErrBufCnt)
            {
                ts_put_ts(&tsmgr1, pBufNode->pExtBuf->timestamp);
                LOGE("Unexpected empty buffer on OBD[0]");
                DVP_ASSERT_FAIL();
            }
            else
                pstSessionParams->u32ExpectedErrBufCnt--;
            LOGD("Session[0] Empty output buffer received");
        }

        if((pstSessionParams->u32OutFlushFlag == OUT_FLUSH_REQUEST) || (pstSessionParams->u32OutFlushFlag == OUT_FLUSH_ACTIVE))
            LOGD("Session[0] Flushed output buffer received, out hold count = %d", pstSessionParams->u32OutHoldCnt);
    }

    if(pstSessionParams->u32ObdPending)
        pstSessionParams->u32ObdPending--;

    pthread_mutex_unlock(&pstTestCtx->mutex);

    put_buf(pBufNode->pPool, pBufNode);
    pthread_cond_signal(&pstTestCtx->cond);
}

static void buffer_exchange_evt1_cb(__unused void *pv, struct vpp_event e)
{
    uint32_t u32EventType;
    enum vpp_port port;
    struct functional_test_ctx *pFuncTestCtx;
    t_StSessionParams *pstSessionParams;

    pFuncTestCtx = (struct functional_test_ctx *)pstTestCtx->pPrivateCtx;
    pstSessionParams = &pFuncTestCtx->astSessionParams[0];

    u32EventType = e.type;

    switch(e.type)
    {
        case VPP_EVENT_FLUSH_DONE:
            port = e.flush_done.port;
            if(port == VPP_PORT_INPUT)
            {
                LOGD("Input[0] flush done event received, input hold %d, output hold %d",
                     pstSessionParams->u32InHoldCnt, pstSessionParams->u32OutHoldCnt);
                DVP_ASSERT_EQUAL(pstSessionParams->u32InHoldCnt, 0);
                pstSessionParams->u32InputFlushDoneFlag = 1;
            }
            else if(port == VPP_PORT_OUTPUT)
            {
                LOGD("Output[0] flush done event received, output hold %d, input hold %d",
                     pstSessionParams->u32OutHoldCnt, pstSessionParams->u32InHoldCnt);
                DVP_ASSERT_EQUAL(pstSessionParams->u32OutHoldCnt, 0);

                pthread_mutex_lock(&pstTestCtx->mutex);
                pstSessionParams->u32OutFlushFlag = OUT_FLUSH_DONE;
                pthread_mutex_unlock(&pstTestCtx->mutex);
                pstSessionParams->u32OutputFlushDoneFlag = 1;
            }
            else
            {
                LOGE("Flush[0] done event returned invalid port %d", port);
                DVP_ASSERT_FAIL();
            }
            break;
        case VPP_EVENT_RECONFIG_DONE:
                LOGD("Reconfig[0] done event");
            pstSessionParams->u32ReconfigDoneFlag = 1;
            pthread_cond_signal(&pstTestCtx->cond);
            break;
        case VPP_EVENT_ERROR:
            break;
        default:
            break;
    }
    pthread_cond_signal(&pstTestCtx->cond);
}


static void buffer_exchange_ibd2_cb(__unused void *pv, struct vpp_buffer *buf)
{
    struct bufnode *pBufNode = (struct bufnode *)buf->cookie;
    struct functional_test_ctx *pFuncTestCtx;
    t_StSessionParams *pstSessionParams;

    pFuncTestCtx = (struct functional_test_ctx *)pstTestCtx->pPrivateCtx;
    pstSessionParams = &pFuncTestCtx->astSessionParams[1];

    pthread_mutex_lock(&pstTestCtx->mutex);
    pstSessionParams->u32InRxCnt += 1;
    pstSessionParams->u32InHoldCnt -= 1;

    if(pstSessionParams->u32BypassStartCount)
    {
        pstSessionParams->u32BypassStartCount--;
        if(pstSessionParams->u32BypassFlag & BYPASS_DI_ON)
            pstSessionParams->u32ObdPending = 2;
        else
            pstSessionParams->u32ObdPending = 1;
        LOGD("Bypass[1] starting in %d buffers", pstSessionParams->u32BypassStartCount);
    }
    if(pstSessionParams->u32BypassStopCount)
    {
        pstSessionParams->u32BypassStopCount--;
        LOGD("Bypass[1] stopping in %d buffers", pstSessionParams->u32BypassStopCount);
    }

    pthread_mutex_unlock(&pstTestCtx->mutex);

    pBufNode->pExtBuf->flags = 0;
    LOGI("IBD[1], InRx %d; InHold %d", pstSessionParams->u32InRxCnt, pstSessionParams->u32InHoldCnt);
    // dump_vpp_buf(buf, "ibd");
    put_buf(pBufNode->pPool, pBufNode);
    pthread_cond_signal(&pstTestCtx->cond);
}

static void buffer_exchange_obd2_cb(__unused void *pv, struct vpp_buffer *buf)
{
    struct bufnode *pBufNode = (struct bufnode *)buf->cookie;
    uint32_t u32;
    struct functional_test_ctx *pFuncTestCtx;
    t_StSessionParams *pstSessionParams;
    t_EVppBufType eFrameType = eVppBufType_Progressive;

    pFuncTestCtx = (struct functional_test_ctx *)pstTestCtx->pPrivateCtx;
    pstSessionParams = &pFuncTestCtx->astSessionParams[1];

    pthread_mutex_lock(&pstTestCtx->mutex);

    pstSessionParams->u32OutRxCnt += 1;

    if((pstSessionParams->u32BypassStopCount == 0) && (pstSessionParams->u32BypassFlag & BYPASS_STOP))
    {
        pstSessionParams->u32BypassFlag &= ~(BYPASS_STOP | BYPASS_START | BYPASS_FRAMERATE | BYPASS_DI_ON);
    }

    if((((pstSessionParams->u32BypassStartCount == 0) && (pstSessionParams->u32BypassFlag & BYPASS_START)
        && (pstSessionParams->u32ObdPending == 0)) || (pstSessionParams->u32BypassFlag & BYPASS_FORCE)) && (pstSessionParams->u32OutFlushFlag == 0))
    {
        DVP_ASSERT_EQUAL((pBufNode->port_owner),VPP_PORT_INPUT);
    }
    else
    {
        if(!pstSessionParams->u32ExpectedErrBufCnt)
            DVP_ASSERT_EQUAL((pBufNode->port_owner),VPP_PORT_OUTPUT);
    }

    if (pBufNode->port_owner == VPP_PORT_INPUT)
    {
        pstSessionParams->u32InHoldCnt -= 1;
        LOGI("Bypassed[1] buffer received");
        if(pstSessionParams->u32BypassFlag & BYPASS_START)
        {
            if(pstSessionParams->u32BypassStopCount)
            {
                pstSessionParams->u32BypassStopCount--;
                LOGD("Bypass[1] stopping in %d buffers", pstSessionParams->u32BypassStopCount);
            }
        }
    }
    else if (pBufNode->port_owner == VPP_PORT_OUTPUT)
    {
        pstSessionParams->u32OutHoldCnt -= 1;
    }
    else
    {
        DVP_ASSERT_FAIL();
    }

    LOGI("OBD[1], OutRx %d; OutHold %d", pstSessionParams->u32OutRxCnt, pstSessionParams->u32OutHoldCnt);

    dump_vpp_buf(buf, "obd");

    if(pstSessionParams->u32OutFlushFlag == OUT_FLUSH_ACTIVE)
        DVP_ASSERT_EQUAL(pBufNode->pExtBuf->pixel.filled_len, 0);

    if (pBufNode->pExtBuf->pixel.filled_len)
    {
        ts_put_ts(&tsmgr2, pBufNode->pExtBuf->timestamp);
        validate_extradata_integrity(pBufNode);
        u32 = u32VppBuf_GetFrameTypeExtradata(pBufNode->pIntBuf, VPP_EXTERNAL_EXTRADATA_TYPE, &eFrameType);
        if((pstSessionParams->u32ExpectedErrBufCnt) && (pBufNode->port_owner == VPP_PORT_INPUT))
            frametype_dequeue(&frametypemgr2, eFrameType, 1);
        else
            frametype_dequeue(&frametypemgr2, eFrameType, 0);
/*
        if((pBufNode->pExtBuf->flags) & 0x0F)
        {
            u32 = flag_dequeue(&flagmgr, pBufNode->pExtBuf->flags, (pstTestCtx->u32OutRxCnt - 1));
            DVP_ASSERT_EQUAL(u32, VPP_OK);
            pBufNode->pExtBuf->flags = 0;
        }
*/
        if(pstSessionParams->u32OutRxCnt < (pstTestCtx->u32FramesOut + 1))
        {
            u32 = dump_buf(pBufNode);
            // DVP_ASSERT_EQUAL(u32, VPP_OK);
            LOGE_IF(u32 != VPP_OK, "Unable to dump_buf");
        }
    }
    else
    {
        if(pstSessionParams->u32OutFlushFlag == OUT_FLUSH_REQUEST)
            pstSessionParams->u32OutFlushFlag = OUT_FLUSH_ACTIVE;
        else if(pstSessionParams->u32OutFlushFlag == OUT_FLUSH_DONE)
        {
            // Buffer was returned unfilled
            if(!pstSessionParams->u32ExpectedErrBufCnt)
            {
                ts_put_ts(&tsmgr2, pBufNode->pExtBuf->timestamp);
                LOGE("Unexpected empty buffer on OBD[1]");
                DVP_ASSERT_FAIL();
            }
            else
                pstSessionParams->u32ExpectedErrBufCnt--;
            LOGD("Session[1] Empty output buffer received");
        }

        if((pstSessionParams->u32OutFlushFlag == OUT_FLUSH_REQUEST) || (pstSessionParams->u32OutFlushFlag == OUT_FLUSH_ACTIVE))
            LOGD("Session[1] Flushed output buffer received, out hold count = %d", pstSessionParams->u32OutHoldCnt);
    }

    if(pstSessionParams->u32ObdPending)
        pstSessionParams->u32ObdPending--;

    pthread_mutex_unlock(&pstTestCtx->mutex);

    put_buf(pBufNode->pPool, pBufNode);
    pthread_cond_signal(&pstTestCtx->cond);
}

static void buffer_exchange_evt2_cb(__unused void *pv, struct vpp_event e)
{
    uint32_t u32EventType;
    enum vpp_port port;
    struct functional_test_ctx *pFuncTestCtx;
    t_StSessionParams *pstSessionParams;

    pFuncTestCtx = (struct functional_test_ctx *)pstTestCtx->pPrivateCtx;
    pstSessionParams = &pFuncTestCtx->astSessionParams[1];

    u32EventType = e.type;

    switch(e.type)
    {
        case VPP_EVENT_FLUSH_DONE:
            port = e.flush_done.port;
            if(port == VPP_PORT_INPUT)
            {
                LOGD("Input[1] flush done event received, input hold %d, output hold %d",
                     pstSessionParams->u32InHoldCnt, pstSessionParams->u32OutHoldCnt);
                DVP_ASSERT_EQUAL(pstSessionParams->u32InHoldCnt, 0);
                pstSessionParams->u32InputFlushDoneFlag = 1;
            }
            else if(port == VPP_PORT_OUTPUT)
            {
                LOGD("Output[1] flush done event received, output hold %d, input hold %d",
                     pstSessionParams->u32OutHoldCnt, pstSessionParams->u32InHoldCnt);
                DVP_ASSERT_EQUAL(pstSessionParams->u32OutHoldCnt, 0);

                pthread_mutex_lock(&pstTestCtx->mutex);
                pstSessionParams->u32OutFlushFlag = OUT_FLUSH_DONE;
                pthread_mutex_unlock(&pstTestCtx->mutex);
                pstSessionParams->u32OutputFlushDoneFlag = 1;
            }
            else
            {
                LOGE("Flush[1] done event returned invalid port %d", port);
                DVP_ASSERT_FAIL();
            }
            break;
        case VPP_EVENT_RECONFIG_DONE:
                LOGD("Reconfig[1] done event");
            pstSessionParams->u32ReconfigDoneFlag = 1;
            pthread_cond_signal(&pstTestCtx->cond);
            break;
        case VPP_EVENT_ERROR:
            break;
        default:
            break;
    }
    pthread_cond_signal(&pstTestCtx->cond);
}


/************************************************************************
 * Test Helper Functions
 ***********************************************************************/
/*
static void rotate_buffers(void *ctx, struct buf_pool *pool, uint32_t u32FramesOutCnt, uint32_t mode, struct ts_mgr *tsmgr)
{
    uint32_t u32Ret;
    struct bufnode *pNode;
    enum vpp_port port, temp;
    uint32_t u32Temp = (BUFFER_POOL_SZ/2);
    uint32_t u32InQueueCnt = 0;
    uint32_t u32OutQueueCnt = 0;
    u32FramesOutCnt += pstTestCtx->u32OutRxCnt;

    if(mode == ROTATE_HALF_OUT_HALF_IN)
        temp = VPP_PORT_OUTPUT;
    else
        temp = VPP_PORT_INPUT;

    while((pstTestCtx->u32OutRxCnt < u32FramesOutCnt) || (u32InQueueCnt != u32OutQueueCnt))
    {

        pthread_mutex_lock(&pstTestCtx->mutex);
        while (NULL == (pNode = get_buf(pool)))
            pthread_cond_wait(&pstTestCtx->cond, &pstTestCtx->mutex);
        pthread_mutex_unlock(&pstTestCtx->mutex);

        pthread_mutex_lock(&pstTestCtx->mutex);

        if(mode == ROTATE_OUT_IN)
        {
            // OUTPUT, INPUT, OUTPUT, INPUT, etc
            if ((pstTestCtx->u32OutPutCnt == 0) || (pstTestCtx->u32OutPutCnt <= pstTestCtx->u32InPutCnt))
                port = VPP_PORT_OUTPUT;
            else
                port = VPP_PORT_INPUT;
        }
        else if(mode == ROTATE_IN_OUT)
        {
            // INPUT, OUTPUT, INPUT, OUTPUT,  etc
            if ((pstTestCtx->u32InPutCnt == 0) || (pstTestCtx->u32InPutCnt <= pstTestCtx->u32OutPutCnt))
                port = VPP_PORT_INPUT;
            else
                port = VPP_PORT_OUTPUT;
        }
        else if(mode == ROTATE_HALF_OUT_HALF_IN)
        {
            // Queue half the pool size in a row on OUTPUT, than queue other half on INPUT, repeat
            if(pstTestCtx->u32OutPutCnt == pstTestCtx->u32InPutCnt)
            {
                temp = VPP_PORT_OUTPUT;
                port = VPP_PORT_OUTPUT;
            }
            else if(((pstTestCtx->u32OutPutCnt - pstTestCtx->u32InPutCnt) < (BUFFER_POOL_SZ/2)) &&
                      (temp == VPP_PORT_OUTPUT))
            {
                port = VPP_PORT_OUTPUT;
            }
            else
            {
                temp = VPP_PORT_INPUT;
                port = VPP_PORT_INPUT;
            }
        }
        else
        {
            // Queue half the pool size in a row on INPUT, than queue other half on OUTPUT, repeat
            if(pstTestCtx->u32InPutCnt == pstTestCtx->u32OutPutCnt)
            {
                temp = VPP_PORT_INPUT;
                port = VPP_PORT_INPUT;
            }
            else if(((pstTestCtx->u32InPutCnt - pstTestCtx->u32OutPutCnt) < (BUFFER_POOL_SZ/2)) &&
                      (temp == VPP_PORT_INPUT))
            {
                port = VPP_PORT_INPUT;
            }
            else
            {
                temp = VPP_PORT_OUTPUT;
                port = VPP_PORT_OUTPUT;
            }
        }

        if(port == VPP_PORT_OUTPUT)
        {
            LOGI("out_put=%u, in_put=%u, q_output fd=%d",
                 pstTestCtx->u32OutPutCnt, pstTestCtx->u32InPutCnt,
                 pNode->pExtBuf->pixel.fd);
            pNode->port_owner = VPP_PORT_OUTPUT;
            pstTestCtx->u32OutPutCnt += 1;
            pstTestCtx->u32OutHoldCnt += 1;
            u32OutQueueCnt++;

        }
        else
        {
            LOGI("out_put=%u, in_put=%u, q_input fd=%d",
                 pstTestCtx->u32OutPutCnt, pstTestCtx->u32InPutCnt,
                 pNode->pExtBuf->pixel.fd);
            pNode->pExtBuf->timestamp = ts_get_next(tsmgr);
            pNode->port_owner = VPP_PORT_INPUT;
            u32Ret = fill_buf(pNode);
            DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
            LOGE_IF(u32Ret != VPP_OK, "Fill buffer failed. May be processing garbage");


            pstTestCtx->u32InPutCnt += 1;
            pstTestCtx->u32InHoldCnt += 1;
            u32InQueueCnt++;
        }

        pthread_mutex_unlock(&pstTestCtx->mutex);

        u32Ret = VPP_QUEUE_BUF(ctx, port, pNode->pExtBuf);
        DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
        LOGE_IF(u32Ret != VPP_OK, "u32Ret=%u", u32Ret);
    }
}
*/
static void rotate_buffers(void *ctx, uint32_t u32Index, struct buf_pool *pool, uint32_t u32FramesOutCnt,
                           struct ts_mgr *tsmgr, struct frame_mgr *frametypemgr, uint32_t u32OutFactor,
                           uint32_t u32FrameType)
{
    uint32_t u32Ret;
    struct bufnode *pNode;
    enum vpp_port port;
    uint32_t u32InQueueCnt = 0;
    uint32_t u32OutQueueCnt = 0;
#ifndef VPP_SERVICE
    uint32_t u32Temp;
    t_StVppCb *pstCb = ((t_StVppCtx *)ctx)->pstVppCb;
#endif
    struct functional_test_ctx *pFuncTestCtx;
    t_StSessionParams *pstSessionParams;

    pFuncTestCtx = (struct functional_test_ctx *)pstTestCtx->pPrivateCtx;

    if(u32Index > MAX_SESSIONS)
        u32Index = MAX_SESSIONS;

    pstSessionParams = &pFuncTestCtx->astSessionParams[u32Index];

    u32FramesOutCnt += pstSessionParams->u32OutRxCnt;


    if((u32OutFactor == 0) || (u32FrameType == eVppBufType_Progressive))
        u32OutFactor = 1;

    tsmgr->u32OuttoInRatio = u32OutFactor;

    while((pstSessionParams->u32OutRxCnt < u32FramesOutCnt) ||
          (u32InQueueCnt * u32OutFactor != u32OutQueueCnt))
    {

        pthread_mutex_lock(&pstTestCtx->mutex);
        while (NULL == (pNode = get_buf(pool)))
            pthread_cond_wait(&pstTestCtx->cond, &pstTestCtx->mutex);
        pthread_mutex_unlock(&pstTestCtx->mutex);

        pthread_mutex_lock(&pstTestCtx->mutex);

        // OUTPUT, INPUT, OUTPUT, INPUT, etc
        if ((pstSessionParams->u32OutPutCnt == 0) || ((pstSessionParams->u32OutPutCnt / u32OutFactor) <= pstSessionParams->u32InPutCnt))
            port = VPP_PORT_OUTPUT;
        else
            port = VPP_PORT_INPUT;

        if(pstSessionParams->u32OutRxCnt >= u32FramesOutCnt)
        {
            // If expected buffers already, override above logic above to avoid deadlock
            if((u32InQueueCnt * u32OutFactor) < u32OutQueueCnt)
                port = VPP_PORT_INPUT;
            else
                port = VPP_PORT_OUTPUT;
        }

        if(port == VPP_PORT_OUTPUT)
        {
            LOGI("Session[%d]out_put=%u, in_put=%u, q_output fd=%d",u32Index,
                 pstSessionParams->u32OutPutCnt, pstSessionParams->u32InPutCnt,
                 pNode->pExtBuf->pixel.fd);
            pNode->port_owner = VPP_PORT_OUTPUT;
            pstSessionParams->u32OutPutCnt += 1;
            pstSessionParams->u32OutHoldCnt += 1;
            u32OutQueueCnt++;

        }
        else
        {
            LOGI("Session[%d]out_put=%u, in_put=%u, q_input fd=%d", u32Index,
                 pstSessionParams->u32OutPutCnt, pstSessionParams->u32InPutCnt,
                 pNode->pExtBuf->pixel.fd);
            pNode->pExtBuf->timestamp = ts_get_next(tsmgr);
            pNode->port_owner = VPP_PORT_INPUT;
            u32Ret = fill_buf(pNode);
            LOGE_IF(u32Ret != VPP_OK, "Fill buffer failed. May be processing garbage");

            u32Ret = fill_extra_data(pNode, u32FrameType, 0);
            LOGE_IF(u32Ret != VPP_OK, "Fill buffer metadata failed.");

            pstSessionParams->u32InPutCnt += 1;
            pstSessionParams->u32InHoldCnt += 1;
            u32InQueueCnt++;
        }
        pthread_mutex_unlock(&pstTestCtx->mutex);

        u32Ret = VPP_QUEUE_BUF(ctx, port, pNode->pExtBuf);

        pthread_mutex_lock(&pstTestCtx->mutex);
        if(port == VPP_PORT_INPUT)
        {
#ifndef VPP_SERVICE
            if((pstCb->stFrameRate.u32Bypass == 1) && ((pstSessionParams->u32BypassFlag & BYPASS_START) == 0))
            {
                pstSessionParams->u32BypassFlag |= (BYPASS_START | BYPASS_FRAMERATE);

                if((get_uc_algo_enabled(ctx, HQV_CONTROL_DI)) && (u32FrameType != eVppBufType_Progressive)
                   && (u32OutFactor == 2))
                {
                    pstSessionParams->u32BypassFlag |= BYPASS_DI_ON;
                }
                if(pstSessionParams->u32InHoldCnt > 0)
                    pstSessionParams->u32BypassStartCount = (pstSessionParams->u32InHoldCnt - 1);
                else
                    pstSessionParams->u32BypassStartCount = 0;
            }
            if((pstCb->stFrameRate.u32Bypass == 0) && (pstSessionParams->u32BypassFlag & BYPASS_START)
               && ((pstSessionParams->u32BypassFlag & BYPASS_STOP) == 0) && (pstSessionParams->u32BypassFlag & BYPASS_FRAMERATE))
            {
                pstSessionParams->u32BypassFlag |= BYPASS_STOP;

                if(pstSessionParams->u32InHoldCnt > 0)
                    pstSessionParams->u32BypassStopCount = (pstSessionParams->u32InHoldCnt - 1);
            }

            if((get_uc_algo_enabled(ctx, HQV_CONTROL_DI)) && (u32FrameType != eVppBufType_Progressive)
               && (u32OutFactor == 2) && (pstCb->stFrameRate.u32Bypass == 0))
#else
            if((get_uc_algo_enabled(ctx, HQV_CONTROL_DI)) && (u32FrameType != eVppBufType_Progressive)
               && (u32OutFactor == 2))
#endif
            {
                frametype_queue(frametypemgr, eVppBufType_Progressive);
                frametype_queue(frametypemgr, eVppBufType_Progressive);
            }
            else
            {
                frametype_queue(frametypemgr, u32FrameType);
            }

#ifndef VPP_SERVICE
            // Need to clean up ts and frametype queue for bypass if DI is on
            if(pstCb->stFrameRate.u32Bypass == 1)
            {
                if(tsmgr->u32OuttoInRatio == 2)
                {
                    pthread_mutex_lock(&tsmgr->mutex);
                    u32Temp = tsmgr->stQ.entries;
                    if(u32Temp > 0)
                    {
                        tsmgr->stQ.entries--;
                        u32Temp = tsmgr->stQ.head;
                        LOGD("THROWING OUT TS %" PRIu64"", tsmgr->pu64Ts[u32Temp]);
                        if(u32Temp)
                            u32Temp--;
                        else
                            u32Temp = tsmgr->stQ.size - 1;
                        tsmgr->stQ.head = u32Temp;
                    }
                    pthread_mutex_unlock(&tsmgr->mutex);
                }
            }
#endif
        }
        pthread_mutex_unlock(&pstTestCtx->mutex);

        DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
        LOGE_IF(u32Ret != VPP_OK, "u32Ret=%u", u32Ret);
    }
}

/*
static uint32_t queue_buffer(void *ctx, enum vpp_port port, struct ts_mgr *tsmgr)
{
    uint32_t u32Ret;
    struct bufnode *pNode;

    pthread_mutex_lock(&pstTestCtx->mutex);
    while (NULL == (pNode = get_buf(pstTestCtx->buf_pool)))
        pthread_cond_wait(&pstTestCtx->cond, &pstTestCtx->mutex);
    pthread_mutex_unlock(&pstTestCtx->mutex);

    pthread_mutex_lock(&pstTestCtx->mutex);

    if(port == VPP_PORT_OUTPUT)
    {
        LOGI("out_put=%u, in_put=%u, q_output fd=%d",
            pstTestCtx->u32OutPutCnt, pstTestCtx->u32InPutCnt,
            pNode->pExtBuf->pixel.fd);
        pNode->port_owner = VPP_PORT_OUTPUT;
        pstTestCtx->u32OutPutCnt += 1;
        pstTestCtx->u32OutHoldCnt += 1;

    }
    else
    {
        LOGI("out_put=%u, in_put=%u, q_input fd=%d",
            pstTestCtx->u32OutPutCnt, pstTestCtx->u32InPutCnt,
            pNode->pExtBuf->pixel.fd);
        pNode->pExtBuf->timestamp = ts_get_next(tsmgr);
        pNode->port_owner = VPP_PORT_INPUT;
        u32Ret = fill_buf(pNode);
        DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
        LOGE_IF(u32Ret != VPP_OK, "Fill buffer failed. May be processing garbage");


        pstTestCtx->u32InPutCnt += 1;
        pstTestCtx->u32InHoldCnt += 1;
    }
    pthread_mutex_unlock(&pstTestCtx->mutex);

    u32Ret = VPP_QUEUE_BUF(ctx, port, pNode->pExtBuf);

    return u32Ret;
}
*/

static void set_default_hqv_controls()
{
    memset(&pstTestCtx->hqvCtrl, 0, sizeof(t_StVppHqvCtrl));

    pstTestCtx->u32hqvCtrlSetFlag |= (1 << HQV_CONTROL_CADE);
    pstTestCtx->hqvCtrl.mode = HQV_MODE_MANUAL;
    pstTestCtx->hqvCtrl.cade.mode = HQV_MODE_AUTO;
}


/************************************************************************
 * Test Functions
 ***********************************************************************/
TEST_SUITE_INIT(ConcurrencySuiteInit)
{
}

TEST_SUITE_TERM(ConcurrencySuiteTerm)
{
}

TEST_SETUP(ConcurrencyTestInit)
{
    uint32_t u32, i;
    struct functional_test_ctx *pFuncTestCtx;

    pFuncTestCtx = (struct functional_test_ctx *)pstTestCtx->pPrivateCtx;


    u32 = init_buf_pool(&pstTestCtx->stVppCtx, &pstTestCtx->buf_pool,
                        &pstTestCtx->params, BUFFER_POOL_SZ, VPP_TRUE);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = ts_mgr_init(&tsmgr1, TS_FREQ_30HZ, BUFFER_POOL_SZ);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    u32 = ts_mgr_init(&tsmgr2, TS_FREQ_30HZ, BUFFER_POOL_SZ);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    u32 = frametype_mgr_init(&frametypemgr1, BUFFER_POOL_SZ * 2);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
    u32 = frametype_mgr_init(&frametypemgr2, BUFFER_POOL_SZ * 2);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    pstTestCtx->buf_pool->u32RdIdx = 0;
    pstTestCtx->buf_pool->u32WrIdx = 0;
    pstTestCtx->u32InRxCnt = 0;
    pstTestCtx->u32OutRxCnt = 0;
    pstTestCtx->u32InPutCnt = 0;
    pstTestCtx->u32OutPutCnt = 0;
    pstTestCtx->u32InHoldCnt = 0;
    pstTestCtx->u32OutHoldCnt = 0;

    for(i = 0; i < MAX_SESSIONS; i++)
    {
        memset(&pFuncTestCtx->astSessionParams[i], 0, sizeof(t_StSessionParams));
    }
}

TEST_CLEANUP(ConcurrencyTestTerm)
{
    struct functional_test_ctx *pFuncTestCtx;

    pFuncTestCtx = (struct functional_test_ctx *)pstTestCtx->pPrivateCtx;

    ts_mgr_destroy(&tsmgr1);
    ts_mgr_destroy(&tsmgr2);
    frametype_mgr_destroy(&frametypemgr1);
    frametype_mgr_destroy(&frametypemgr2);

    free_buf_pool(pstTestCtx->buf_pool, VPP_TRUE);
}


TEST(ConcurrencyBasic_Testcase)
{
    void *p1, *p2;
    struct vpp_callbacks stCb;
    struct vpp_requirements req1, req2;
    uint32_t u32MulFactor1, u32MulFactor2;
    struct vpp_port_param param;
    t_StVppHqvCtrl hqvCtrlTemp;
    uint32_t u32, u32Frames, u32FrameType1, u32FrameType2,u32TestMask, u32hqvCtrlSetFlagTemp;
    struct functional_test_ctx *pFuncTestCtx;

    pFuncTestCtx = (struct functional_test_ctx *)pstTestCtx->pPrivateCtx;
    u32TestMask = pFuncTestCtx->u32TestMask;

    if(u32TestMask & FUNCTEST_CONC_BASIC)
    {

        u32 = init_buf_pool(&pstTestCtx->stVppCtx, &pFuncTestCtx->reconfig_streams[0].reconfig_pool,
                           &pFuncTestCtx->reconfig_streams[0].reconfig_pool_params,
                           BUFFER_POOL_SZ, VPP_FALSE);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        // Callbacks
        stCb.input_buffer_done = buffer_exchange_ibd1_cb,
        stCb.output_buffer_done = buffer_exchange_obd1_cb,
        stCb.vpp_event = buffer_exchange_evt1_cb,

        // Port parameters
        param = pstTestCtx->port_param;

        p1 = VPP_INIT(get_session_flags(pstTestCtx), stCb);
        DVP_ASSERT_PTR_NNULL(p1);

            // Callbacks
        stCb.input_buffer_done = buffer_exchange_ibd2_cb,
        stCb.output_buffer_done = buffer_exchange_obd2_cb,
        stCb.vpp_event = buffer_exchange_evt2_cb,
        p2 = VPP_INIT(get_session_flags(pstTestCtx), stCb);
        DVP_ASSERT_PTR_NNULL(p2);


        memcpy(&hqvCtrlTemp, &pstTestCtx->hqvCtrl, sizeof(hqvCtrlTemp));
        u32hqvCtrlSetFlagTemp = pstTestCtx->u32hqvCtrlSetFlag;

        if(pstTestCtx->u32hqvCtrlSetFlag == 0)
        {
            set_default_hqv_controls();
        }

        u32 = setTestVppHqvCtrl(p1, pstTestCtx, &req1);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
/*
        pstTestCtx->u32hqvCtrlSetFlag &= ~(1 << HQV_CONTROL_AIE);
        pstTestCtx->u32hqvCtrlSetFlag |= (1 << HQV_CONTROL_CNR);
        pstTestCtx->hqvCtrl.mode = HQV_MODE_MANUAL;
        pstTestCtx->hqvCtrl.cnr.mode = HQV_MODE_MANUAL;
        pstTestCtx->hqvCtrl.cnr.level = 50;
*/

        if(pFuncTestCtx->u32hqvCtrlSetFlag)
        {
            memcpy(&pstTestCtx->hqvCtrl, &pFuncTestCtx->hqvCtrl, sizeof(t_StVppHqvCtrl));
            pstTestCtx->u32hqvCtrlSetFlag = pFuncTestCtx->u32hqvCtrlSetFlag;
        }
        u32 = setTestVppHqvCtrl(p2, pstTestCtx, &req2);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        if(pstTestCtx->buf_pool->params.eBufferType == eVppBufType_Interleaved_BFF)
            u32FrameType1 = eVppBufType_Interleaved_BFF;
        else if(pstTestCtx->buf_pool->params.eBufferType == eVppBufType_Interleaved_TFF)
            u32FrameType1 = eVppBufType_Interleaved_TFF;
        else
            u32FrameType1 = eVppBufType_Progressive;

        if(pFuncTestCtx->reconfig_streams[0].reconfig_pool_params.eBufferType == eVppBufType_Interleaved_BFF)
            u32FrameType2 = eVppBufType_Interleaved_BFF;
        else if(pFuncTestCtx->reconfig_streams[0].reconfig_pool_params.eBufferType == eVppBufType_Interleaved_TFF)
            u32FrameType2 = eVppBufType_Interleaved_TFF;
        else
            u32FrameType2 = eVppBufType_Progressive;

        u32MulFactor1 = u32GetMulFactor(&param, &req1);

        u32 = VPP_SET_PARAM(p1, VPP_PORT_INPUT, param);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        u32 = VPP_SET_PARAM(p1, VPP_PORT_OUTPUT, param);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        param = pFuncTestCtx->reconfig_streams[0].port_param;

        u32MulFactor2 = u32GetMulFactor(&param, &req2);

        u32 = VPP_SET_PARAM(p2, VPP_PORT_INPUT, param);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        u32 = VPP_SET_PARAM(p2, VPP_PORT_OUTPUT, param);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        if(u32 == VPP_OK)
        {
            snprintf(pstTestCtx->buf_pool->params.cOutputName, MAX_FILE_SEG_LEN,
                     "Concurrency_%s", pstTestCtx->params.cOutputName);
            if(pstTestCtx->u32FramesOut)
                u32Frames = pstTestCtx->u32FramesOut;
            else
            {
                u32Frames = 20;
                pstTestCtx->u32FramesOut = u32Frames;
            }
            rotate_buffers(p1, 0, pstTestCtx->buf_pool, 10, &tsmgr1, &frametypemgr1,
                           u32MulFactor1, u32FrameType1);
            snprintf(pFuncTestCtx->reconfig_streams[0].reconfig_pool->params.cOutputName,
                     MAX_FILE_SEG_LEN, "Concurrency_%s",
                     pFuncTestCtx->reconfig_streams[0].reconfig_pool_params.cOutputName);
            rotate_buffers(p2, 1, pFuncTestCtx->reconfig_streams[0].reconfig_pool, 10, &tsmgr2,
                           &frametypemgr2, u32MulFactor2, u32FrameType2);
            rotate_buffers(p1, 0, pstTestCtx->buf_pool, 10, &tsmgr1, &frametypemgr1,
                           u32MulFactor1, u32FrameType1);
            rotate_buffers(p2, 1, pFuncTestCtx->reconfig_streams[0].reconfig_pool, 10, &tsmgr2,
                           &frametypemgr2, u32MulFactor2, u32FrameType2);
            rotate_buffers(p1, 0, pstTestCtx->buf_pool, 10, &tsmgr1, &frametypemgr1,
                           u32MulFactor1, u32FrameType1);
            rotate_buffers(p2, 1, pFuncTestCtx->reconfig_streams[0].reconfig_pool, 10, &tsmgr2,
                           &frametypemgr2, u32MulFactor2, u32FrameType2);
        }

        memcpy(&pstTestCtx->hqvCtrl, &hqvCtrlTemp, sizeof(t_StVppHqvCtrl));
        pstTestCtx->u32hqvCtrlSetFlag = u32hqvCtrlSetFlagTemp;

        pFuncTestCtx->astSessionParams[0].u32OutFlushFlag = OUT_FLUSH_REQUEST;
        pFuncTestCtx->astSessionParams[1].u32OutFlushFlag = OUT_FLUSH_REQUEST;

        VPP_TERM(p1);
        VPP_TERM(p2);

        free_buf_pool(pFuncTestCtx->reconfig_streams[0].reconfig_pool, VPP_FALSE);
    }
}

/*
TEST(ConcurrencyReconfigure_Testcase)
{
    void *p1, *p2;
    struct vpp_callbacks stCb;
    struct vpp_requirements req1, req2;
    struct vpp_port_param param;
    uint32_t u32, u32Frames, i, u32FrameType1, u32FrameType2;
    uint32_t u32ResetHqvCtrlFlag = 0;
    struct functional_test_ctx *pFuncTestCtx;

    pFuncTestCtx = (struct functional_test_ctx *)pstTestCtx->pPrivateCtx;

    for(i = 0; i < pFuncTestCtx->u32StreamCount; i++)
    {
        u32 = init_buf_pool(&pstTestCtx->stVppCtx, &pFuncTestCtx->reconfig_streams[i].reconfig_pool,
                           &pFuncTestCtx->reconfig_streams[i].reconfig_pool_params,
                           BUFFER_POOL_SZ, VPP_FALSE);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
    }

    // Callbacks
    stCb.input_buffer_done = buffer_exchange_ibd1_cb,
    stCb.output_buffer_done = buffer_exchange_obd1_cb,
    stCb.vpp_event = buffer_exchange_evt1_cb,

    // Port parameters
    param = pstTestCtx->port_param;

    p1 = VPP_INIT(get_session_flags(pstTestCtx), stCb);
    DVP_ASSERT_PTR_NNULL(p1);

        // Callbacks
    stCb.input_buffer_done = buffer_exchange_ibd2_cb,
    stCb.output_buffer_done = buffer_exchange_obd2_cb,
    stCb.vpp_event = buffer_exchange_evt2_cb,
    p2 = VPP_INIT(get_session_flags(pstTestCtx), stCb);
    DVP_ASSERT_PTR_NNULL(p2);


    if(pstTestCtx->u32hqvCtrlSetFlag == 0)
    {
        set_default_hqv_controls();
        u32ResetHqvCtrlFlag = 1;
    }

    u32 = setTestVppHqvCtrl(p1, pstTestCtx, &req1);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = setTestVppHqvCtrl(p2, pstTestCtx, &req2);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    if(get_uc_algo_enabled(p1, HQV_CONTROL_DI))
    {
        if(pstTestCtx->buf_pool->params.eBufferType == eVppBufType_Interleaved_BFF)
            u32FrameType1 = eVppBufType_Interleaved_BFF;
        else
            u32FrameType1 = eVppBufType_Interleaved_TFF;
    }
    else
        u32FrameType1 = eVppBufType_Progressive;


    if(get_uc_algo_enabled(p2, HQV_CONTROL_DI))
    {
        if(pFuncTestCtx->reconfig_streams[0].reconfig_pool_params.eBufferType == eVppBufType_Interleaved_BFF)
            u32FrameType2 = eVppBufType_Interleaved_BFF;
        else
            u32FrameType2 = eVppBufType_Interleaved_TFF;
    }
    else
        u32FrameType2 = eVppBufType_Progressive;


    u32 = VPP_SET_PARAM(p1, VPP_PORT_INPUT, param);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = VPP_SET_PARAM(p1, VPP_PORT_OUTPUT, param);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    param = pFuncTestCtx->reconfig_streams[0].port_param;
    u32 = VPP_SET_PARAM(p2, VPP_PORT_INPUT, param);

    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = VPP_SET_PARAM(p2, VPP_PORT_OUTPUT, param);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    if(u32 == VPP_OK)
    {
        snprintf(pstTestCtx->buf_pool->params.cOutputName, MAX_FILE_SEG_LEN,
                 "Concurrency_%s", pstTestCtx->params.cOutputName);
        if(pstTestCtx->u32FramesOut)
            u32Frames = pstTestCtx->u32FramesOut;
        else
        {
            u32Frames = 20;
            pstTestCtx->u32FramesOut = u32Frames;
        }
        rotate_buffers(p1, pstTestCtx->buf_pool, 10, &tsmgr1, &frametypemgr1, req1.out_factor.mul, u32FrameType1);
        snprintf(pFuncTestCtx->reconfig_streams[0].reconfig_pool->params.cOutputName,
                 MAX_FILE_SEG_LEN, "Concurrency_%s", pFuncTestCtx->reconfig_streams[0].reconfig_pool_params.cOutputName);
        rotate_buffers(p2, pFuncTestCtx->reconfig_streams[0].reconfig_pool, 10, &tsmgr2, &frametypemgr2, req2.out_factor.mul, u32FrameType2);
        rotate_buffers(p1, pstTestCtx->buf_pool, 10, &tsmgr1, &frametypemgr1, req1.out_factor.mul, u32FrameType1);
        rotate_buffers(p2, pFuncTestCtx->reconfig_streams[0].reconfig_pool, 10, &tsmgr2, &frametypemgr2, req2.out_factor.mul, u32FrameType2);
        rotate_buffers(p1, pstTestCtx->buf_pool, 10, &tsmgr1, &frametypemgr1, req1.out_factor.mul, u32FrameType1);
        rotate_buffers(p2, pFuncTestCtx->reconfig_streams[0].reconfig_pool, 10, &tsmgr2, &frametypemgr2, req2.out_factor.mul, u32FrameType2);
    }

    if(u32ResetHqvCtrlFlag)
        pstTestCtx->u32hqvCtrlSetFlag = 0;

    u32OutFlushFlag = OUT_FLUSH_REQUEST;

    for(i = 0; i < pFuncTestCtx->u32StreamCount; i++)
    {
        free_buf_pool(pFuncTestCtx->reconfig_streams[i].reconfig_pool, VPP_FALSE);
    }


    VPP_TERM(p1);
    VPP_TERM(p2);
}
*/
/************************************************************************
 * Global Functions
 ***********************************************************************/
TEST_CASES ConcurrencyTests[] = {
    TEST_CASE(ConcurrencyBasic_Testcase),
    TEST_CASE_NULL(),
};

TEST_SUITE(ConcurrencySuite,
           "Concurrency Tests",
           ConcurrencySuiteInit,
           ConcurrencySuiteTerm,
           ConcurrencyTestInit,
           ConcurrencyTestTerm,
           ConcurrencyTests);