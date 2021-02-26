/*!
 * @file test_buffer_exchange.cpp
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

#define VPP_LOG_TAG     VPP_LOG_FT_BUFFER_EXCHANGE_TAG
#define VPP_LOG_MODULE  VPP_LOG_FT_BUFFER_EXCHANGE
#include "vpp_dbg.h"
#include "vpp.h"
#include "vpp_utils.h"
#include "vpp_uc.h"
#include "test_stub.h"

/************************************************************************
 * Local definitions
 ***********************************************************************/
#define BUFFER_POOL_SZ 20
#define STRESS_TEST_FRAMES 10000

#define FUNCTEST_ROTATE_BASIC     (1 << 0)
#define FUNCTEST_ROTATE_ALL       (1 << 1)
#define FUNCTEST_SETCTRL          (1 << 2)
#define FUNCTEST_FLAG             (1 << 3)
#define FUNCTEST_FLUSH            (1 << 4)
#define FUNCTEST_RECONFIG_BASIC   (1 << 5)
#define FUNCTEST_RECONFIG_ALL     (1 << 6)
#define FUNCTEST_BYPASS           (1 << 7)
#define FUNCTEST_STRESS           (1 << 8)

enum
{
    ROTATE_OUT_IN,  // Queue 1 on the output, then 1 on input
    ROTATE_IN_OUT,  // Queue 1 on the input, then 1 on output
    ROTATE_HALF_OUT_HALF_IN, // Queue half buffers on output, then half on input
    ROTATE_HALF_IN_HALF_OUT,    // Queue half buffers on input, then half on output
};

enum
{
    FIELD_ORDER_PROGRESSIVE,
    FIELD_ORDER_TFF,
    FIELD_ORDER_BFF,
};

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

#define VPP_BUFFER_FLAG_TEMP_INTERLACED (1 << 8)
#define VPP_BUFFER_FLAG_TEMP_TFF  (1 << 9)

/************************************************************************
 * Local static variables
 ***********************************************************************/
extern struct test_ctx stTestCtx;
extern struct test_ctx *pstTestCtx;

// TODO: move this into test_ctx (but need to first get a way to get from
// a struct bufnode to test_ctx.
static struct ts_mgr tsmgr;
static struct flag_mgr flagmgr;
static struct frame_mgr frametypemgr;
static uint32_t u32OutFlushFlag;
static uint32_t u32ReconfigDoneFlag;
static uint32_t u32BypassFlag;
static uint32_t u32BypassStartCount;
static uint32_t u32BypassStopCount;
static uint32_t u32OutputFlushDoneFlag;
static uint32_t u32InputFlushDoneFlag;
static uint32_t u32ExpectedErrBufCnt;

static uint32_t u32ObdPending;
static uint32_t bUsecaseBypassesInput;

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

static void buffer_exchange_ibd_cb(__unused void *pv, struct vpp_buffer *buf)
{
    struct bufnode *pBufNode = (struct bufnode *)buf->cookie;

    pthread_mutex_lock(&pstTestCtx->mutex);
    pstTestCtx->u32InRxCnt += 1;
    pstTestCtx->u32InHoldCnt -= 1;

    if(u32BypassStartCount)
    {
        u32BypassStartCount--;
        if(u32BypassFlag & BYPASS_DI_ON)
            u32ObdPending = 2;
        else
            u32ObdPending = 1;
        LOGD("Bypass starting in %d buffers", u32BypassStartCount);
    }
    if(u32BypassStopCount)
    {
        u32BypassStopCount--;
        LOGD("Bypass stopping in %d buffers", u32BypassStopCount);
    }

    pthread_mutex_unlock(&pstTestCtx->mutex);

    pBufNode->pExtBuf->flags = 0;
    LOGI("IBD, InRx %d; InHold %d", pstTestCtx->u32InRxCnt, pstTestCtx->u32InHoldCnt);
    // dump_vpp_buf(buf, "ibd");
    put_buf(pBufNode->pPool, pBufNode);
    pthread_cond_signal(&pstTestCtx->cond);
}

static void buffer_exchange_obd_cb(__unused void *pv, struct vpp_buffer *buf)
{
    struct bufnode *pBufNode = (struct bufnode *)buf->cookie;
    uint32_t u32;
    t_EVppBufType eFrameType = eVppBufType_Progressive;

    pthread_mutex_lock(&pstTestCtx->mutex);

    pstTestCtx->u32OutRxCnt += 1;

    if((u32BypassStopCount == 0) && (u32BypassFlag & BYPASS_STOP))
    {
        u32BypassFlag &= ~(BYPASS_STOP | BYPASS_START | BYPASS_FRAMERATE | BYPASS_DI_ON);
    }

    if (((((u32BypassStartCount == 0) && (u32BypassFlag & BYPASS_START)
        && (u32ObdPending == 0)) || (u32BypassFlag & BYPASS_FORCE)) && (u32OutFlushFlag == 0)) &&
        !bUsecaseBypassesInput)
    {
        DVP_ASSERT_EQUAL((pBufNode->port_owner),VPP_PORT_INPUT);
    }
    else
    {
        if ((!u32ExpectedErrBufCnt) && !bUsecaseBypassesInput)
            DVP_ASSERT_EQUAL((pBufNode->port_owner),VPP_PORT_OUTPUT);
    }

    if (pBufNode->port_owner == VPP_PORT_INPUT)
    {
        pstTestCtx->u32InHoldCnt -= 1;
        LOGI("Bypassed buffer received");
        if(u32BypassFlag & BYPASS_START)
        {
            if(u32BypassStopCount)
            {
                u32BypassStopCount--;
                LOGD("Bypass stopping in %d buffers", u32BypassStopCount);
            }
        }
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
        ts_put_ts(&tsmgr, pBufNode->pExtBuf->timestamp);
        validate_extradata_integrity(pBufNode);
        u32 = u32VppBuf_GetFrameTypeExtradata(pBufNode->pIntBuf, VPP_EXTERNAL_EXTRADATA_TYPE, &eFrameType);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
        if (((u32ExpectedErrBufCnt) && (pBufNode->port_owner == VPP_PORT_INPUT)) ||
            bUsecaseBypassesInput)
            frametype_dequeue(&frametypemgr, eFrameType, 1);
        else
            frametype_dequeue(&frametypemgr, eFrameType, 0);

        if(((pBufNode->pExtBuf->flags) & 0x0F) && !bUsecaseBypassesInput)
        {
            u32 = flag_dequeue(&flagmgr, pBufNode->pExtBuf->flags, (pstTestCtx->u32OutRxCnt - 1));
            DVP_ASSERT_EQUAL(u32, VPP_OK);
            pBufNode->pExtBuf->flags = 0;
        }
        if(pstTestCtx->u32OutRxCnt < (pstTestCtx->u32FramesOut + 1))
        {
            u32 = dump_buf(pBufNode);
            // DVP_ASSERT_EQUAL(u32, VPP_OK);
            LOGE_IF(u32 != VPP_OK, "Unable to dump_buf");
        }
    }
    else
    {
        if(u32OutFlushFlag == OUT_FLUSH_REQUEST)
            u32OutFlushFlag = OUT_FLUSH_ACTIVE;
        else if(u32OutFlushFlag == OUT_FLUSH_DONE)
        {
            // Buffer was returned unfilled
            if(!u32ExpectedErrBufCnt)
            {
                ts_put_ts(&tsmgr, pBufNode->pExtBuf->timestamp);
                LOGE("Unexpected empty buffer on OBD");
                DVP_ASSERT_FAIL();
            }
            else
                u32ExpectedErrBufCnt--;
            LOGD("Empty output buffer received");
        }

        if((u32OutFlushFlag == OUT_FLUSH_REQUEST) || (u32OutFlushFlag == OUT_FLUSH_ACTIVE))
            LOGD("Flushed output buffer received, out hold count = %d", pstTestCtx->u32OutHoldCnt);
    }

    if(u32ObdPending)
        u32ObdPending--;

    pthread_mutex_unlock(&pstTestCtx->mutex);

    put_buf(pBufNode->pPool, pBufNode);
    pthread_cond_signal(&pstTestCtx->cond);
}

static void buffer_exchange_evt_cb(__unused void *pv, struct vpp_event e)
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
static void rotate_buffers(void *ctx, uint32_t u32FramesOutCnt,
                           uint32_t mode, uint32_t u32OutFactor,
                           uint32_t u32FrameType)
{
    uint32_t u32Ret;
    struct bufnode *pNode;
    enum vpp_port port, temp;
    uint32_t u32InQueueCnt = 0;
    uint32_t u32OutQueueCnt = 0;
#ifndef VPP_SERVICE
    uint32_t u32Temp;
    t_StVppCb *pstCb = ((t_StVppCtx *)ctx)->pstVppCb;
#endif

    u32FramesOutCnt += pstTestCtx->u32OutRxCnt;
    if(mode == ROTATE_HALF_OUT_HALF_IN)
        temp = VPP_PORT_OUTPUT;
    else
        temp = VPP_PORT_INPUT;

    if(((u32OutFactor == 0) || (u32FrameType == eVppBufType_Progressive)) &&
       !bUsecaseBypassesInput)
        u32OutFactor = 1;

    if(u32OutFactor != 1)
        mode = ROTATE_OUT_IN;

    tsmgr.u32OuttoInRatio = u32OutFactor;

    while((pstTestCtx->u32OutRxCnt < u32FramesOutCnt) ||
          ((u32InQueueCnt * u32OutFactor != u32OutQueueCnt) &&
          !bUsecaseBypassesInput))
    {

        pthread_mutex_lock(&pstTestCtx->mutex);
        while (NULL == (pNode = get_buf(pstTestCtx->buf_pool)))
            pthread_cond_wait(&pstTestCtx->cond, &pstTestCtx->mutex);
        pthread_mutex_unlock(&pstTestCtx->mutex);

        pthread_mutex_lock(&pstTestCtx->mutex);

        if(mode == ROTATE_OUT_IN)
        {
            // OUTPUT, INPUT, OUTPUT, INPUT, etc
            if (((pstTestCtx->u32OutPutCnt == 0) ||
                ((pstTestCtx->u32OutPutCnt / u32OutFactor) <= pstTestCtx->u32InPutCnt)) &&
                (pstTestCtx->u32OutHoldCnt < (pstTestCtx->u32InHoldCnt + 2)))
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
        else //ROTATE_HALF_IN_HALF_OUT
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

        if(pstTestCtx->u32OutRxCnt >= u32FramesOutCnt)
        {
            // If expected buffers already, override above logic above to avoid deadlock
            if((u32InQueueCnt * u32OutFactor) < u32OutQueueCnt)
                port = VPP_PORT_INPUT;
            else
                port = VPP_PORT_OUTPUT;
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

            if (pNode->pIntBuf->stPixel.pvBase)
                memset(pNode->pIntBuf->stPixel.pvBase, 0,
                       pNode->pIntBuf->stPixel.u32AllocLen);
        }
        else
        {
            LOGI("out_put=%u, in_put=%u, q_input fd=%d",
                 pstTestCtx->u32OutPutCnt, pstTestCtx->u32InPutCnt,
                 pNode->pExtBuf->pixel.fd);
            pNode->pExtBuf->timestamp = ts_get_next(&tsmgr);
            pNode->port_owner = VPP_PORT_INPUT;
            u32Ret = fill_buf(pNode);
            LOGE_IF(u32Ret != VPP_OK, "Fill buffer failed. May be processing garbage");

            u32Ret = fill_extra_data(pNode, u32FrameType, 0);
            LOGE_IF(u32Ret != VPP_OK, "Fill buffer metadata failed.");

            pstTestCtx->u32InPutCnt += 1;
            pstTestCtx->u32InHoldCnt += 1;
            u32InQueueCnt++;
        }
        pthread_mutex_unlock(&pstTestCtx->mutex);

        u32Ret = VPP_QUEUE_BUF(ctx, port, pNode->pExtBuf);

        pthread_mutex_lock(&pstTestCtx->mutex);
        if(port == VPP_PORT_INPUT)
        {
#ifndef VPP_SERVICE
            if((pstCb->stFrameRate.u32Bypass == 1) && ((u32BypassFlag & BYPASS_START) == 0))
            {
                u32BypassFlag |= (BYPASS_START | BYPASS_FRAMERATE);

                if((get_uc_algo_enabled(ctx, HQV_CONTROL_DI)) && (u32FrameType != eVppBufType_Progressive)
                   && (u32OutFactor == 2))
                {
                    u32BypassFlag |= BYPASS_DI_ON;
                }
                if(pstTestCtx->u32InHoldCnt > 0)
                    u32BypassStartCount = (pstTestCtx->u32InHoldCnt - 1);
                else
                    u32BypassStartCount = 0;
            }
            if((pstCb->stFrameRate.u32Bypass == 0) && (u32BypassFlag & BYPASS_START)
               && ((u32BypassFlag & BYPASS_STOP) == 0) && (u32BypassFlag & BYPASS_FRAMERATE))
            {
                u32BypassFlag |= BYPASS_STOP;

                if(pstTestCtx->u32InHoldCnt > 0)
                    u32BypassStopCount = (pstTestCtx->u32InHoldCnt - 1);
            }
            if((get_uc_algo_enabled(ctx, HQV_CONTROL_DI)) && (u32FrameType != eVppBufType_Progressive)
               && (u32OutFactor == 2) && (pstCb->stFrameRate.u32Bypass == 0))
#else
            if((get_uc_algo_enabled(ctx, HQV_CONTROL_DI)) && (u32FrameType != eVppBufType_Progressive)
               && (u32OutFactor == 2))
#endif
            {
                frametype_queue(&frametypemgr, eVppBufType_Progressive);
                frametype_queue(&frametypemgr, eVppBufType_Progressive);
            }
            else
            {
                frametype_queue(&frametypemgr, u32FrameType);
            }
#ifndef VPP_SERVICE
            // Need to clean up ts and frametype queue for bypass if DI is on
            if(pstCb->stFrameRate.u32Bypass == 1)
            {
                if(tsmgr.u32OuttoInRatio == 2)
                {
                    pthread_mutex_lock(&tsmgr.mutex);
                    u32Temp = tsmgr.stQ.entries;
                    if(u32Temp > 0)
                    {
                        tsmgr.stQ.entries--;
                        u32Temp = tsmgr.stQ.head;
                        LOGD("THROWING OUT TS %" PRIu64"", tsmgr.pu64Ts[u32Temp]);
                        if(u32Temp)
                            u32Temp--;
                        else
                            u32Temp = tsmgr.stQ.size - 1;
                        tsmgr.stQ.head = u32Temp;
                    }
                    pthread_mutex_unlock(&tsmgr.mutex);
                }
            }
#endif
        }
        pthread_mutex_unlock(&pstTestCtx->mutex);

        DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
        LOGE_IF(u32Ret != VPP_OK, "u32Ret=%u", u32Ret);
    }
}

static uint32_t rotate_buffers_insert_flag(void *ctx, uint32_t u32FlagCnt, uint32_t u32Flags, uint32_t u32OutFactor, uint32_t u32Bypass, uint32_t u32FrameType)
{
    uint32_t u32Ret;
    struct bufnode *pNode;
    enum vpp_port port;
    uint32_t u32InCount = 0;
    uint32_t u32BufNumber = 0;

    if((u32OutFactor == 0) || (u32FrameType == eVppBufType_Progressive))
        u32OutFactor = 1;

    tsmgr.u32OuttoInRatio = u32OutFactor;

    while(u32InCount <= u32FlagCnt)
    {

        pthread_mutex_lock(&pstTestCtx->mutex);
        while (NULL == (pNode = get_buf(pstTestCtx->buf_pool)))
            pthread_cond_wait(&pstTestCtx->cond, &pstTestCtx->mutex);
        pthread_mutex_unlock(&pstTestCtx->mutex);

        pthread_mutex_lock(&pstTestCtx->mutex);

        if ((pstTestCtx->u32OutPutCnt == 0) || ((pstTestCtx->u32OutPutCnt / u32OutFactor) <= pstTestCtx->u32InPutCnt))
            port = VPP_PORT_OUTPUT;
        else
            port = VPP_PORT_INPUT;

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
            pNode->pExtBuf->timestamp = ts_get_next(&tsmgr);
            pNode->port_owner = VPP_PORT_INPUT;

            if(u32FlagCnt == u32InCount)
            {
                pNode->pExtBuf->flags = u32Flags;
                u32BufNumber = pstTestCtx->u32InPutCnt;
            }
            u32Ret = fill_buf(pNode);
            // DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
            LOGE_IF(u32Ret != VPP_OK, "Fill buffer failed. May be processing garbage");

            u32Ret = fill_extra_data(pNode, u32FrameType, 0);
            LOGE_IF(u32Ret != VPP_OK, "Fill buffer metadata failed.");

            if((get_uc_algo_enabled(ctx, HQV_CONTROL_DI)) && (u32FrameType != eVppBufType_Progressive)
               && (u32OutFactor == 2))
            {
                frametype_queue(&frametypemgr, eVppBufType_Progressive);
                frametype_queue(&frametypemgr, eVppBufType_Progressive);
            }
            else
            {
                frametype_queue(&frametypemgr, u32FrameType);
            }

            pstTestCtx->u32InPutCnt += 1;
            pstTestCtx->u32InHoldCnt += 1;
            u32InCount++;
        }

        if(port == VPP_PORT_INPUT)
        {
            if((u32Bypass == 1) && ((u32BypassFlag & BYPASS_START) == 0))
            {
                u32BypassFlag |= BYPASS_START;
                if(pstTestCtx->u32InHoldCnt > 0)
                    u32BypassStartCount = (pstTestCtx->u32InHoldCnt - 1);
                else
                    u32BypassStartCount = 0;
            }
            if((u32Bypass == 0) && (u32BypassFlag & BYPASS_START)
               && ((u32BypassFlag & BYPASS_STOP) == 0))
            {
                u32BypassFlag |= BYPASS_STOP;

                if(pstTestCtx->u32InHoldCnt > 0)
                    u32BypassStopCount = (pstTestCtx->u32InHoldCnt -1);
            }
        }

        pthread_mutex_unlock(&pstTestCtx->mutex);

        u32Ret = VPP_QUEUE_BUF(ctx, port, pNode->pExtBuf);
        DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
        LOGE_IF(u32Ret != VPP_OK, "u32Ret=%u", u32Ret);
    }
    return u32BufNumber;
}

static uint32_t queue_buffer(void *ctx, enum vpp_port port, uint32_t u32OutFactor, uint32_t u32FrameType)
{
    uint32_t u32Ret;
    struct bufnode *pNode;

    if((u32OutFactor == 0) || (u32FrameType == eVppBufType_Progressive))
        u32OutFactor = 1;

    tsmgr.u32OuttoInRatio = u32OutFactor;

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
       pNode->pExtBuf->timestamp = ts_get_next(&tsmgr);
       pNode->port_owner = VPP_PORT_INPUT;
       u32Ret = fill_buf(pNode);
       // DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
       LOGE_IF(u32Ret != VPP_OK, "Fill buffer failed. May be processing garbage");

       u32Ret = fill_extra_data(pNode, u32FrameType, 0);
       LOGE_IF(u32Ret != VPP_OK, "Fill buffer metadata failed.");

       if((get_uc_algo_enabled(ctx, HQV_CONTROL_DI)) && (u32FrameType != eVppBufType_Progressive)
          && (u32OutFactor == 2) && !(u32BypassFlag & BYPASS_FORCE))
       {
           frametype_queue(&frametypemgr, eVppBufType_Progressive);
           frametype_queue(&frametypemgr, eVppBufType_Progressive);
       }
       else
       {
           frametype_queue(&frametypemgr, u32FrameType);
       }

       pstTestCtx->u32InPutCnt += 1;
       pstTestCtx->u32InHoldCnt += 1;
   }
   pthread_mutex_unlock(&pstTestCtx->mutex);

    u32Ret = VPP_QUEUE_BUF(ctx, port, pNode->pExtBuf);

    return u32Ret;
}

static void set_default_hqv_controls()
{
    memset(&pstTestCtx->hqvCtrl, 0, sizeof(t_StVppHqvCtrl));

    pstTestCtx->u32hqvCtrlSetFlag |= (1 << HQV_CONTROL_CADE);
    pstTestCtx->hqvCtrl.mode = HQV_MODE_MANUAL;
    pstTestCtx->hqvCtrl.cade.mode = HQV_MODE_AUTO;
}

static void wait_reconfig_done(void *ctx, uint32_t u32OutFactor, uint32_t u32FrameType)
{
    VPP_RET_VOID_IF_NULL(ctx);

    pthread_mutex_lock(&pstTestCtx->mutex);
    while(u32ReconfigDoneFlag == 0)
    {
        pthread_mutex_unlock(&pstTestCtx->mutex);
        while (pstTestCtx->u32InHoldCnt)
        {
            if (pstTestCtx->u32OutHoldCnt < u32OutFactor)
            {
                queue_buffer(ctx, VPP_PORT_OUTPUT, u32OutFactor, u32FrameType);
            }
        }
        pthread_mutex_lock(&pstTestCtx->mutex);
        pthread_cond_wait(&pstTestCtx->cond, &pstTestCtx->mutex);
    }
    pthread_mutex_unlock(&pstTestCtx->mutex);
}

static uint32_t bUsecase_Bypasses(struct test_ctx *pTestCtx)
{
    VPP_RET_IF_NULL(pTestCtx, VPP_TRUE);

    // FRC bypasses input to output as part of normal processing if it's the only control enabled
    if ((pTestCtx->u32hqvCtrlSetFlag & ((1 << HQV_CONTROL_CUST) - 1)) ==
        (1 << HQV_CONTROL_FRC))
    {
        if (pTestCtx->hqvCtrl.frc.mode != HQV_FRC_MODE_OFF)
            return VPP_TRUE;
    }
    return VPP_FALSE;
}

/************************************************************************
 * Test Functions
 ***********************************************************************/
TEST_SUITE_INIT(BufferExchangeSuiteInit)
{
}

TEST_SUITE_TERM(BufferExchangeSuiteTerm)
{
}

TEST_SETUP(BufferExchangeTestInit)
{
    uint32_t u32;

    u32 = init_buf_pool(&pstTestCtx->stVppCtx, &pstTestCtx->buf_pool,
                        &pstTestCtx->params, BUFFER_POOL_SZ, VPP_TRUE);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = ts_mgr_init(&tsmgr, TS_FREQ_7HZ, BUFFER_POOL_SZ);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = frametype_mgr_init(&frametypemgr, BUFFER_POOL_SZ * 2);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    pstTestCtx->buf_pool->u32RdIdx = 0;
    pstTestCtx->buf_pool->u32WrIdx = 0;
    pstTestCtx->u32InRxCnt = 0;
    pstTestCtx->u32OutRxCnt = 0;
    pstTestCtx->u32InPutCnt = 0;
    pstTestCtx->u32OutPutCnt = 0;
    pstTestCtx->u32InHoldCnt = 0;
    pstTestCtx->u32OutHoldCnt = 0;
    u32OutFlushFlag = OUT_FLUSH_DONE;
    u32BypassFlag = 0;
    u32BypassStartCount = 0;
    u32BypassStopCount = 0;
    u32ExpectedErrBufCnt = 0;
    bUsecaseBypassesInput = VPP_FALSE;

}

TEST_CLEANUP(BufferExchangeTestTerm)
{
    ts_mgr_destroy(&tsmgr);
    frametype_mgr_destroy(&frametypemgr);
    free_buf_pool(pstTestCtx->buf_pool, VPP_TRUE);
}


TEST(BufferExchange1_Testcase)
{
    void *p;
    struct vpp_callbacks stCb;
    struct vpp_requirements req;
    struct vpp_port_param param;
    uint32_t u32, u32Frames, u32FrameType, u32MulFactor;
    uint32_t u32ResetHqvCtrlFlag = 0;
    struct functional_test_ctx *pFuncTestCtx;
    uint32_t u32TestMask;

    pFuncTestCtx = (struct functional_test_ctx *)pstTestCtx->pPrivateCtx;
    u32TestMask = pFuncTestCtx->u32TestMask;

    if((u32TestMask & FUNCTEST_ROTATE_BASIC) || (u32TestMask & FUNCTEST_ROTATE_ALL))
    {
        // Callbacks
        stCb.input_buffer_done = buffer_exchange_ibd_cb,
        stCb.output_buffer_done = buffer_exchange_obd_cb,
        stCb.vpp_event = buffer_exchange_evt_cb,

        // Port parameters
        param = pstTestCtx->port_param;

        p = VPP_INIT(get_session_flags(pstTestCtx), stCb);
        DVP_ASSERT_PTR_NNULL(p);

        if(pstTestCtx->u32hqvCtrlSetFlag == 0)
        {
            set_default_hqv_controls();
            u32ResetHqvCtrlFlag = 1;
        }

        u32 = setTestVppHqvCtrl(p, pstTestCtx, &req);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
        bUsecaseBypassesInput = bUsecase_Bypasses(pstTestCtx);

        u32MulFactor = u32GetMulFactor(&param, &req);

        if(pstTestCtx->buf_pool->params.eBufferType == eVppBufType_Interleaved_BFF)
            u32FrameType = eVppBufType_Interleaved_BFF;
        else if(pstTestCtx->buf_pool->params.eBufferType == eVppBufType_Interleaved_TFF)
            u32FrameType = eVppBufType_Interleaved_TFF;
        else
            u32FrameType = eVppBufType_Progressive;

        u32 = VPP_SET_PARAM(p, VPP_PORT_INPUT, param);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        u32 = VPP_SET_PARAM(p, VPP_PORT_OUTPUT, param);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        if(u32 == VPP_OK)
        {
            snprintf(pstTestCtx->buf_pool->params.cOutputName, MAX_FILE_SEG_LEN,
                     "Rotate1_%s", pstTestCtx->params.cOutputName);
            if(pstTestCtx->u32FramesOut)
                u32Frames = pstTestCtx->u32FramesOut;
            else
            {
                u32Frames = 20;
                pstTestCtx->u32FramesOut = u32Frames;
            }
            rotate_buffers(p, u32Frames, ROTATE_OUT_IN, u32MulFactor, u32FrameType);
        }

        if(u32ResetHqvCtrlFlag)
            pstTestCtx->u32hqvCtrlSetFlag = 0;

        u32OutFlushFlag = OUT_FLUSH_REQUEST;
        VPP_TERM(p);
    }
    else
    {
        LOGI("TEST SKIPPED.");
    }
}

TEST(BufferExchange2_Testcase)
{
    void *p;
    struct vpp_callbacks stCb;
    struct vpp_requirements req;
    struct vpp_port_param param;
    uint32_t u32, u32Frames, u32FrameType, u32MulFactor;
    uint32_t u32ResetHqvCtrlFlag = 0;
    struct functional_test_ctx *pFuncTestCtx;
    uint32_t u32TestMask;

    pFuncTestCtx = (struct functional_test_ctx *)pstTestCtx->pPrivateCtx;
    u32TestMask = pFuncTestCtx->u32TestMask;

    if(u32TestMask & FUNCTEST_ROTATE_ALL)
    {
        // Callbacks
        stCb.input_buffer_done = buffer_exchange_ibd_cb,
        stCb.output_buffer_done = buffer_exchange_obd_cb,
        stCb.vpp_event = buffer_exchange_evt_cb,

        // Port parameters
        param = pstTestCtx->port_param;

        p = VPP_INIT(get_session_flags(pstTestCtx), stCb);
        DVP_ASSERT_PTR_NNULL(p);

        if(pstTestCtx->u32hqvCtrlSetFlag == 0)
        {
            set_default_hqv_controls();
            u32ResetHqvCtrlFlag = 1;
        }

        u32 = setTestVppHqvCtrl(p, pstTestCtx, &req);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
        bUsecaseBypassesInput = bUsecase_Bypasses(pstTestCtx);

        u32MulFactor = u32GetMulFactor(&param, &req);

        if(pstTestCtx->buf_pool->params.eBufferType == eVppBufType_Interleaved_BFF)
            u32FrameType = eVppBufType_Interleaved_BFF;
        else if(pstTestCtx->buf_pool->params.eBufferType == eVppBufType_Interleaved_TFF)
            u32FrameType = eVppBufType_Interleaved_TFF;
        else
            u32FrameType = eVppBufType_Progressive;

        u32 = VPP_SET_PARAM(p, VPP_PORT_INPUT, param);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        u32 = VPP_SET_PARAM(p, VPP_PORT_OUTPUT, param);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        if(u32 == VPP_OK)
        {
            snprintf(pstTestCtx->buf_pool->params.cOutputName, MAX_FILE_SEG_LEN,
                     "Rotate2_%s", pstTestCtx->params.cOutputName);
            if(pstTestCtx->u32FramesOut)
                u32Frames = pstTestCtx->u32FramesOut;
            else
            {
                u32Frames = 20;
                pstTestCtx->u32FramesOut = u32Frames;
            }
            rotate_buffers(p, u32Frames, ROTATE_IN_OUT, u32MulFactor, u32FrameType);
        }

        if(u32ResetHqvCtrlFlag)
            pstTestCtx->u32hqvCtrlSetFlag = 0;

        u32OutFlushFlag = OUT_FLUSH_REQUEST;
        VPP_TERM(p);
    }
    else
    {
        LOGI("TEST SKIPPED.");
    }
}

TEST(BufferExchange3_Testcase)
{
    void *p;
    struct vpp_callbacks stCb;
    struct vpp_requirements req;
    struct vpp_port_param param;
    uint32_t u32, u32Frames, u32FrameType, u32MulFactor;
    uint32_t u32ResetHqvCtrlFlag = 0;
    struct functional_test_ctx *pFuncTestCtx;
    uint32_t u32TestMask;

    pFuncTestCtx = (struct functional_test_ctx *)pstTestCtx->pPrivateCtx;
    u32TestMask = pFuncTestCtx->u32TestMask;

    if(u32TestMask & FUNCTEST_ROTATE_ALL)
    {
        // Callbacks
        stCb.input_buffer_done = buffer_exchange_ibd_cb,
        stCb.output_buffer_done = buffer_exchange_obd_cb,
        stCb.vpp_event = buffer_exchange_evt_cb,

        // Port parameters
        param = pstTestCtx->port_param;

        p = VPP_INIT(get_session_flags(pstTestCtx), stCb);
        DVP_ASSERT_PTR_NNULL(p);

        if(pstTestCtx->u32hqvCtrlSetFlag == 0)
        {
            set_default_hqv_controls();
            u32ResetHqvCtrlFlag = 1;
        }

        u32 = setTestVppHqvCtrl(p, pstTestCtx, &req);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
        bUsecaseBypassesInput = bUsecase_Bypasses(pstTestCtx);

        u32MulFactor = u32GetMulFactor(&param, &req);

        if(pstTestCtx->buf_pool->params.eBufferType == eVppBufType_Interleaved_BFF)
            u32FrameType = eVppBufType_Interleaved_BFF;
        else if(pstTestCtx->buf_pool->params.eBufferType == eVppBufType_Interleaved_TFF)
            u32FrameType = eVppBufType_Interleaved_TFF;
        else
            u32FrameType = eVppBufType_Progressive;

        u32 = VPP_SET_PARAM(p, VPP_PORT_INPUT, param);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        u32 = VPP_SET_PARAM(p, VPP_PORT_OUTPUT, param);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        if(u32 == VPP_OK)
        {
            snprintf(pstTestCtx->buf_pool->params.cOutputName, MAX_FILE_SEG_LEN,
                     "Rotate3_%s", pstTestCtx->params.cOutputName);
            if(pstTestCtx->u32FramesOut)
                u32Frames = pstTestCtx->u32FramesOut;
            else
            {
                u32Frames = 20;
                pstTestCtx->u32FramesOut = u32Frames;
            }
            rotate_buffers(p, u32Frames, ROTATE_HALF_OUT_HALF_IN, u32MulFactor,
                           u32FrameType);
        }

        if(u32ResetHqvCtrlFlag)
            pstTestCtx->u32hqvCtrlSetFlag = 0;

        u32OutFlushFlag = OUT_FLUSH_REQUEST;
        VPP_TERM(p);
    }
    else
    {
        LOGI("TEST SKIPPED.");
    }
}

TEST(BufferExchange4_Testcase)
{
    void *p;
    struct vpp_callbacks stCb;
    struct vpp_requirements req;
    struct vpp_port_param param;
    uint32_t u32, u32Frames, u32FrameType, u32MulFactor;
    uint32_t u32ResetHqvCtrlFlag = 0;
    struct functional_test_ctx *pFuncTestCtx;
    uint32_t u32TestMask;

    pFuncTestCtx = (struct functional_test_ctx *)pstTestCtx->pPrivateCtx;
    u32TestMask = pFuncTestCtx->u32TestMask;

    if(u32TestMask & FUNCTEST_ROTATE_ALL)
    {
        // Callbacks
        stCb.input_buffer_done = buffer_exchange_ibd_cb,
        stCb.output_buffer_done = buffer_exchange_obd_cb,
        stCb.vpp_event = buffer_exchange_evt_cb,

        // Port parameters
        param = pstTestCtx->port_param;

        p = VPP_INIT(get_session_flags(pstTestCtx), stCb);
        DVP_ASSERT_PTR_NNULL(p);

        if(pstTestCtx->u32hqvCtrlSetFlag == 0)
        {
            set_default_hqv_controls();
            u32ResetHqvCtrlFlag = 1;
        }

        u32 = setTestVppHqvCtrl(p, pstTestCtx, &req);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
        bUsecaseBypassesInput = bUsecase_Bypasses(pstTestCtx);

        u32MulFactor = u32GetMulFactor(&param, &req);

        if(pstTestCtx->buf_pool->params.eBufferType == eVppBufType_Interleaved_BFF)
            u32FrameType = eVppBufType_Interleaved_BFF;
        else if(pstTestCtx->buf_pool->params.eBufferType == eVppBufType_Interleaved_TFF)
            u32FrameType = eVppBufType_Interleaved_TFF;
        else
            u32FrameType = eVppBufType_Progressive;

        u32 = VPP_SET_PARAM(p, VPP_PORT_INPUT, param);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        u32 = VPP_SET_PARAM(p, VPP_PORT_OUTPUT, param);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        if(u32 == VPP_OK)
        {
            snprintf(pstTestCtx->buf_pool->params.cOutputName, MAX_FILE_SEG_LEN,
                     "Rotate4_%s", pstTestCtx->params.cOutputName);
            if(pstTestCtx->u32FramesOut)
                u32Frames = pstTestCtx->u32FramesOut;
            else
            {
                u32Frames = 20;
                pstTestCtx->u32FramesOut = u32Frames;
            }
            rotate_buffers(p, u32Frames, ROTATE_HALF_IN_HALF_OUT, u32MulFactor,
                           u32FrameType);
        }

        if(u32ResetHqvCtrlFlag)
            pstTestCtx->u32hqvCtrlSetFlag = 0;

        u32OutFlushFlag = OUT_FLUSH_REQUEST;
        VPP_TERM(p);
    }
    else
    {
        LOGI("TEST SKIPPED.");
    }
}

TEST(BufferExchangeSetCtrl_Testcase)
{
    void *p;
    struct vpp_callbacks stCb;
    t_StVppHqvCtrl hqvCtrlTemp;
    struct vpp_requirements req;
    struct vpp_port_param param;
    uint32_t u32, u32Frames, u32TempFrames, u32hqvCtrlSetFlagTemp, u32FrameType, u32MulFactor;
    struct functional_test_ctx *pFuncTestCtx;
    uint32_t u32TestMask;

    pFuncTestCtx = (struct functional_test_ctx *)pstTestCtx->pPrivateCtx;
    u32TestMask = pFuncTestCtx->u32TestMask;

    if(u32TestMask & FUNCTEST_SETCTRL)
    {
        // Callbacks
        stCb.input_buffer_done = buffer_exchange_ibd_cb,
        stCb.output_buffer_done = buffer_exchange_obd_cb,
        stCb.vpp_event = buffer_exchange_evt_cb,

        // Port parameters
        param = pstTestCtx->port_param;

        p = VPP_INIT(get_session_flags(pstTestCtx), stCb);
        DVP_ASSERT_PTR_NNULL(p);

        memcpy(&hqvCtrlTemp, &pstTestCtx->hqvCtrl, sizeof(hqvCtrlTemp));
        u32hqvCtrlSetFlagTemp = pstTestCtx->u32hqvCtrlSetFlag;

        if(pstTestCtx->u32hqvCtrlSetFlag == 0)
        {
            set_default_hqv_controls();
        }

        u32 = setTestVppHqvCtrl(p, pstTestCtx, &req);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
        bUsecaseBypassesInput = bUsecase_Bypasses(pstTestCtx);

        u32MulFactor = u32GetMulFactor(&param, &req);

        if(pstTestCtx->buf_pool->params.eBufferType == eVppBufType_Interleaved_BFF)
            u32FrameType = eVppBufType_Interleaved_BFF;
        else if(pstTestCtx->buf_pool->params.eBufferType == eVppBufType_Interleaved_TFF)
            u32FrameType = eVppBufType_Interleaved_TFF;
        else
            u32FrameType = eVppBufType_Progressive;

        u32 = VPP_SET_PARAM(p, VPP_PORT_INPUT, param);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        u32 = VPP_SET_PARAM(p, VPP_PORT_OUTPUT, param);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
        if(u32 == VPP_OK)
        {
            u32TempFrames = pstTestCtx->u32FramesOut;
            snprintf(pstTestCtx->buf_pool->params.cOutputName, MAX_FILE_SEG_LEN,
                     "SetCtrl_%s", pstTestCtx->params.cOutputName);
            if(pstTestCtx->u32FramesOut >= 20) // Test needs at least 20 frames
                u32Frames = pstTestCtx->u32FramesOut;
            else
            {
                u32Frames = 20;
                pstTestCtx->u32FramesOut = u32Frames;
            }
            rotate_buffers(p, 3, ROTATE_OUT_IN, u32MulFactor, u32FrameType);
            if((get_uc_algo_enabled(p, HQV_CONTROL_CADE)) &&
               (pstTestCtx->hqvCtrl.mode == HQV_MODE_MANUAL))
            {
                pstTestCtx->hqvCtrl.mode = HQV_MODE_MANUAL;
                pstTestCtx->u32hqvCtrlSetFlag |= (1 << HQV_CONTROL_CADE);
                pstTestCtx->hqvCtrl.cade.mode = HQV_MODE_MANUAL;
                pstTestCtx->hqvCtrl.cade.cade_level = 0;
                u32 = setTestVppHqvCtrl(p, pstTestCtx, &req);
                DVP_ASSERT_EQUAL(u32, VPP_OK);
                rotate_buffers(p, 3, ROTATE_OUT_IN, u32MulFactor, u32FrameType);

                pstTestCtx->hqvCtrl.mode = HQV_MODE_MANUAL;
                pstTestCtx->u32hqvCtrlSetFlag |= (1 << HQV_CONTROL_CADE);
                pstTestCtx->hqvCtrl.cade.mode = HQV_MODE_MANUAL;
                pstTestCtx->hqvCtrl.cade.cade_level = 10000; // Should accept this and make max
                u32 = setTestVppHqvCtrl(p, pstTestCtx, &req);
                DVP_ASSERT_EQUAL(u32, VPP_OK);
                rotate_buffers(p, 3, ROTATE_OUT_IN, u32MulFactor, u32FrameType);

                pstTestCtx->hqvCtrl.cade.mode = HQV_MODE_MANUAL;
                pstTestCtx->hqvCtrl.cade.cade_level = 50;
                u32 = setTestVppHqvCtrl(p, pstTestCtx, &req);
                DVP_ASSERT_EQUAL(u32, VPP_OK);
                rotate_buffers(p, 3, ROTATE_OUT_IN, u32MulFactor, u32FrameType);

                pstTestCtx->hqvCtrl.cade.mode = (enum hqv_mode)5;  // Should reject this value
                pstTestCtx->hqvCtrl.cade.cade_level = 100;
                u32 = setTestVppHqvCtrl(p, pstTestCtx, &req);
                DVP_ASSERT_NEQUAL(u32, VPP_OK);
                rotate_buffers(p, 3, ROTATE_OUT_IN, u32MulFactor, u32FrameType);

                pstTestCtx->hqvCtrl.cade.mode = HQV_MODE_MANUAL;
                pstTestCtx->hqvCtrl.cade.cade_level = 25;
                u32 = setTestVppHqvCtrl(p, pstTestCtx, &req);
                DVP_ASSERT_EQUAL(u32, VPP_OK);
                rotate_buffers(p, 3, ROTATE_OUT_IN, u32MulFactor, u32FrameType);

                // Should reject setting TNR in CADE
                if(!get_uc_algo_enabled(p, HQV_CONTROL_TNR))
                {
                    pstTestCtx->u32hqvCtrlSetFlag &= ~(1 << HQV_CONTROL_CADE);
                    pstTestCtx->u32hqvCtrlSetFlag |= (1 << HQV_CONTROL_TNR);
                    pstTestCtx->hqvCtrl.mode = HQV_MODE_MANUAL;
                    pstTestCtx->hqvCtrl.tnr.mode = HQV_MODE_MANUAL;
                    pstTestCtx->hqvCtrl.tnr.level = 10;
                    u32 = setTestVppHqvCtrl(p, pstTestCtx, &req);
                    DVP_ASSERT_NEQUAL(u32, VPP_OK);
                }

                // Should reject turning off CADE while running
                pstTestCtx->u32hqvCtrlSetFlag &= ~(1 << HQV_CONTROL_TNR);
                pstTestCtx->u32hqvCtrlSetFlag |= (1 << HQV_CONTROL_CADE);
                pstTestCtx->hqvCtrl.cade.mode = HQV_MODE_OFF;
                pstTestCtx->hqvCtrl.cade.cade_level = 100;
                u32 = setTestVppHqvCtrl(p, pstTestCtx, &req);
                DVP_ASSERT_NEQUAL(u32, VPP_OK);
            }
            else if((get_uc_algo_enabled(p, HQV_CONTROL_AIE)) && (pstTestCtx->hqvCtrl.mode == HQV_MODE_MANUAL))
            {
                pstTestCtx->hqvCtrl.mode = HQV_MODE_MANUAL;
                pstTestCtx->u32hqvCtrlSetFlag |= (1 << HQV_CONTROL_AIE);
                pstTestCtx->hqvCtrl.aie.mode = HQV_MODE_MANUAL;
                pstTestCtx->hqvCtrl.aie.cade_level = 0;
                u32 = setTestVppHqvCtrl(p, pstTestCtx, &req);
                DVP_ASSERT_EQUAL(u32, VPP_OK);
                rotate_buffers(p, 3, ROTATE_OUT_IN, u32MulFactor, u32FrameType);

                pstTestCtx->hqvCtrl.aie.cade_level = 10000; // Should accept this and make max
                u32 = setTestVppHqvCtrl(p, pstTestCtx, &req);
                DVP_ASSERT_EQUAL(u32, VPP_OK);
                rotate_buffers(p, 3, ROTATE_OUT_IN, u32MulFactor, u32FrameType);

                pstTestCtx->hqvCtrl.aie.cade_level = 50;
                u32 = setTestVppHqvCtrl(p, pstTestCtx, &req);
                DVP_ASSERT_EQUAL(u32, VPP_OK);
                rotate_buffers(p, 3, ROTATE_OUT_IN, u32MulFactor, u32FrameType);

                pstTestCtx->hqvCtrl.aie.mode = (enum hqv_mode)5;  // Should reject this value
                pstTestCtx->hqvCtrl.aie.cade_level = 100;
                u32 = setTestVppHqvCtrl(p, pstTestCtx, &req);
                DVP_ASSERT_NEQUAL(u32, VPP_OK);
                rotate_buffers(p, 3, ROTATE_OUT_IN, u32MulFactor, u32FrameType);

                pstTestCtx->hqvCtrl.aie.mode = HQV_MODE_MANUAL;
                pstTestCtx->hqvCtrl.aie.cade_level = 25;
                u32 = setTestVppHqvCtrl(p, pstTestCtx, &req);
                DVP_ASSERT_EQUAL(u32, VPP_OK);
                rotate_buffers(p, 3, ROTATE_OUT_IN, u32MulFactor, u32FrameType);

                // Should reject setting TNR in AIE
                if(!get_uc_algo_enabled(p, HQV_CONTROL_TNR))
                {
                    pstTestCtx->u32hqvCtrlSetFlag &= ~(1 << HQV_CONTROL_AIE);
                    pstTestCtx->u32hqvCtrlSetFlag |= (1 << HQV_CONTROL_TNR);
                    pstTestCtx->hqvCtrl.mode = HQV_MODE_MANUAL;
                    pstTestCtx->hqvCtrl.tnr.mode = HQV_MODE_MANUAL;
                    pstTestCtx->hqvCtrl.tnr.level = 10;
                    u32 = setTestVppHqvCtrl(p, pstTestCtx, &req);
                    DVP_ASSERT_NEQUAL(u32, VPP_OK);
                }

                // Should reject turning off CADE while running
                pstTestCtx->u32hqvCtrlSetFlag &= ~(1 << HQV_CONTROL_TNR);
                pstTestCtx->u32hqvCtrlSetFlag |= (1 << HQV_CONTROL_AIE);
                pstTestCtx->hqvCtrl.aie.mode = HQV_MODE_OFF;
                pstTestCtx->hqvCtrl.aie.cade_level = 100;
                u32 = setTestVppHqvCtrl(p, pstTestCtx, &req);
                DVP_ASSERT_NEQUAL(u32, VPP_OK);
            }
            else if((get_uc_algo_enabled(p, HQV_CONTROL_DI)) &&
                    (pstTestCtx->hqvCtrl.mode == HQV_MODE_MANUAL))
            {
                pstTestCtx->hqvCtrl.mode = HQV_MODE_MANUAL;
                pstTestCtx->u32hqvCtrlSetFlag |= (1 << HQV_CONTROL_DI);
                pstTestCtx->hqvCtrl.di.mode = HQV_DI_MODE_AUTO;
                u32 = setTestVppHqvCtrl(p, pstTestCtx, &req);
                DVP_ASSERT_EQUAL(u32, VPP_OK);
                rotate_buffers(p, 3, ROTATE_OUT_IN, u32MulFactor, u32FrameType);

                pstTestCtx->u32hqvCtrlSetFlag |= (1 << HQV_CONTROL_DI);
                pstTestCtx->hqvCtrl.di.mode = HQV_DI_MODE_VIDEO_3F;
                u32 = setTestVppHqvCtrl(p, pstTestCtx, &req);
                DVP_ASSERT_EQUAL(u32, VPP_OK);
                rotate_buffers(p, 3, ROTATE_OUT_IN, u32MulFactor, u32FrameType);

                pstTestCtx->hqvCtrl.di.mode = HQV_DI_MODE_VIDEO_1F; // Should accept this
                u32 = setTestVppHqvCtrl(p, pstTestCtx, &req);
                DVP_ASSERT_EQUAL(u32, VPP_OK);
                rotate_buffers(p, 3, ROTATE_OUT_IN, u32MulFactor, u32FrameType);

                // Should reject setting TNR in DI
                if(!get_uc_algo_enabled(p, HQV_CONTROL_TNR))
                {
                    pstTestCtx->u32hqvCtrlSetFlag &= ~(1 << HQV_CONTROL_DI);
                    pstTestCtx->u32hqvCtrlSetFlag |= (1 << HQV_CONTROL_TNR);
                    pstTestCtx->hqvCtrl.mode = HQV_MODE_MANUAL;
                    pstTestCtx->hqvCtrl.tnr.mode = HQV_MODE_MANUAL;
                    pstTestCtx->hqvCtrl.tnr.level = 10;
                    u32 = setTestVppHqvCtrl(p, pstTestCtx, &req);
                    DVP_ASSERT_NEQUAL(u32, VPP_OK);
                }

                // Should reject turning off DI while running
                pstTestCtx->u32hqvCtrlSetFlag &= ~(1 << HQV_CONTROL_TNR);
                pstTestCtx->u32hqvCtrlSetFlag |= (1 << HQV_CONTROL_DI);
                pstTestCtx->hqvCtrl.di.mode = HQV_DI_MODE_OFF;
                u32 = setTestVppHqvCtrl(p, pstTestCtx, &req);
                DVP_ASSERT_NEQUAL(u32, VPP_OK);
            }


            if(pstTestCtx->u32FramesOut > pstTestCtx->u32OutRxCnt)
                rotate_buffers(p, pstTestCtx->u32FramesOut - pstTestCtx->u32OutRxCnt, ROTATE_OUT_IN,
                               u32MulFactor, u32FrameType);

            pstTestCtx->u32FramesOut = u32TempFrames;
        }

        memcpy(&pstTestCtx->hqvCtrl, &hqvCtrlTemp, sizeof(hqvCtrlTemp));
        pstTestCtx->hqvCtrl.mode = hqvCtrlTemp.mode;
        pstTestCtx->u32hqvCtrlSetFlag = u32hqvCtrlSetFlagTemp;

        u32OutFlushFlag = OUT_FLUSH_REQUEST;
        VPP_TERM(p);
    }
    else
    {
        LOGI("TEST SKIPPED.");
    }
}

TEST(BufferExchangeFlag_Testcase)
{
    void *p;
    struct vpp_callbacks stCb;
    struct vpp_requirements req;
    struct vpp_port_param param;
    uint32_t u32, u32Frames, u32TempFrames, u32Flags, u32FrameType;
    uint32_t u32InputBufferNum, u32ExpectedBuffer, u32MulFactor;
    uint32_t u32ResetHqvCtrlFlag = 0;
    uint32_t u32FieldOrder = FIELD_ORDER_TFF;
    struct functional_test_ctx *pFuncTestCtx;
    uint32_t u32TestMask;

    pFuncTestCtx = (struct functional_test_ctx *)pstTestCtx->pPrivateCtx;
    u32TestMask = pFuncTestCtx->u32TestMask;

    if(u32TestMask & FUNCTEST_FLAG)
    {
        u32 = flag_mgr_init(&flagmgr, BUFFER_POOL_SZ);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        // Callbacks
        stCb.input_buffer_done = buffer_exchange_ibd_cb,
        stCb.output_buffer_done = buffer_exchange_obd_cb,
        stCb.vpp_event = buffer_exchange_evt_cb,

        // Port parameters
        param = pstTestCtx->port_param;

        p = VPP_INIT(get_session_flags(pstTestCtx), stCb);
        DVP_ASSERT_PTR_NNULL(p);

        if(pstTestCtx->u32hqvCtrlSetFlag == 0)
        {
            set_default_hqv_controls();
            u32ResetHqvCtrlFlag = 1;
        }

        u32 = setTestVppHqvCtrl(p, pstTestCtx, &req);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
        bUsecaseBypassesInput = bUsecase_Bypasses(pstTestCtx);

        u32MulFactor = u32GetMulFactor(&param, &req);

        if(pstTestCtx->buf_pool->params.eBufferType == eVppBufType_Interleaved_BFF)
        {
            u32FrameType = eVppBufType_Interleaved_BFF;
            u32FieldOrder = FIELD_ORDER_BFF;
        }
        else if(pstTestCtx->buf_pool->params.eBufferType == eVppBufType_Interleaved_TFF)
        {
            u32FrameType = eVppBufType_Interleaved_TFF;
            u32FieldOrder = FIELD_ORDER_TFF;
        }
        else
            u32FrameType = eVppBufType_Progressive;

        u32 = VPP_SET_PARAM(p, VPP_PORT_INPUT, param);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        u32 = VPP_SET_PARAM(p, VPP_PORT_OUTPUT, param);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        if(u32 == VPP_OK)
        {
            u32TempFrames = pstTestCtx->u32FramesOut;
            snprintf(pstTestCtx->buf_pool->params.cOutputName, MAX_FILE_SEG_LEN,
                     "SetFlag_%s", pstTestCtx->params.cOutputName);
            if(pstTestCtx->u32FramesOut >= 40) // Test needs at least 20 frames
                u32Frames = pstTestCtx->u32FramesOut;
            else
            {
                u32Frames = 40;
                pstTestCtx->u32FramesOut = u32Frames;
            }
            u32Flags = VPP_BUFFER_FLAG_SYNCFRAME | VPP_BUFFER_FLAG_EXTRADATA;
            u32InputBufferNum = rotate_buffers_insert_flag(p, 2, u32Flags, u32MulFactor, 0,
                                                           u32FrameType);
            if((u32MulFactor == 2) && (u32FieldOrder == FIELD_ORDER_TFF))
                u32ExpectedBuffer = (u32InputBufferNum * 2) - 1;
            else if((u32MulFactor == 2) && (u32FieldOrder == FIELD_ORDER_BFF))
                u32ExpectedBuffer = (u32InputBufferNum * 2);
            else
                u32ExpectedBuffer = u32InputBufferNum;
            u32 = flag_queue(&flagmgr, u32Flags, u32ExpectedBuffer);
            DVP_ASSERT_EQUAL(u32, VPP_OK);

            u32Flags = VPP_BUFFER_FLAG_DATACORRUPT | VPP_BUFFER_FLAG_EXTRADATA;
            u32InputBufferNum = rotate_buffers_insert_flag(p, 1, u32Flags, u32MulFactor, 0,
                                                           u32FrameType);
            if((u32MulFactor == 2) && (u32FieldOrder == FIELD_ORDER_TFF))
                u32ExpectedBuffer = (u32InputBufferNum * 2) - 3;
            else if((u32MulFactor == 2) && (u32FieldOrder == FIELD_ORDER_BFF))
                u32ExpectedBuffer = (u32InputBufferNum * 2) - 2;
            else
                u32ExpectedBuffer = u32InputBufferNum;
            u32 = flag_queue(&flagmgr, u32Flags, u32ExpectedBuffer);
            DVP_ASSERT_EQUAL(u32, VPP_OK);

            u32Flags = VPP_BUFFER_FLAG_EOS | VPP_BUFFER_FLAG_EXTRADATA;
            u32InputBufferNum = rotate_buffers_insert_flag(p, 8, u32Flags, u32MulFactor, 0,
                                                           u32FrameType);
            if(u32MulFactor == 2)
                u32ExpectedBuffer = (u32InputBufferNum * 2) + 1;
            else
                u32ExpectedBuffer = u32InputBufferNum;
            u32 = flag_queue(&flagmgr, u32Flags, u32ExpectedBuffer);
            DVP_ASSERT_EQUAL(u32, VPP_OK);

            if(get_uc_algo_enabled(p, HQV_CONTROL_DI))
            {
                LOGD("Testing flags with BFF");
                u32FrameType = eVppBufType_Interleaved_BFF;
                u32FieldOrder = FIELD_ORDER_BFF;

                u32Flags = VPP_BUFFER_FLAG_SYNCFRAME;
                u32InputBufferNum = rotate_buffers_insert_flag(p, 2, u32Flags, u32MulFactor,
                                                               0, u32FrameType);
                if((u32MulFactor == 2) && (u32FieldOrder == FIELD_ORDER_TFF))
                    u32ExpectedBuffer = (u32InputBufferNum * 2) - 1;
                else if((u32MulFactor == 2) && (u32FieldOrder == FIELD_ORDER_BFF))
                    u32ExpectedBuffer = (u32InputBufferNum * 2);
                else
                    u32ExpectedBuffer = u32InputBufferNum;
                u32 = flag_queue(&flagmgr, u32Flags, u32ExpectedBuffer);
                DVP_ASSERT_EQUAL(u32, VPP_OK);

                u32Flags = VPP_BUFFER_FLAG_DATACORRUPT;
                u32InputBufferNum = rotate_buffers_insert_flag(p, 1, u32Flags, u32MulFactor,
                                                               0, u32FrameType);
                if((u32MulFactor == 2) && (u32FieldOrder == FIELD_ORDER_TFF))
                    u32ExpectedBuffer = (u32InputBufferNum * 2) - 3;
                else if((u32MulFactor == 2) && (u32FieldOrder == FIELD_ORDER_BFF))
                    u32ExpectedBuffer = (u32InputBufferNum * 2) - 2;
                else
                    u32ExpectedBuffer = u32InputBufferNum;
                u32 = flag_queue(&flagmgr, u32Flags, u32ExpectedBuffer);
                DVP_ASSERT_EQUAL(u32, VPP_OK);

                u32Flags = VPP_BUFFER_FLAG_EOS;
                u32InputBufferNum = rotate_buffers_insert_flag(p, 8, u32Flags, u32MulFactor,
                                                               0, u32FrameType);
                if(u32MulFactor == 2)
                    u32ExpectedBuffer = (u32InputBufferNum * 2) + 1;
                else
                    u32ExpectedBuffer = u32InputBufferNum;
                u32 = flag_queue(&flagmgr, u32Flags, u32ExpectedBuffer);
                DVP_ASSERT_EQUAL(u32, VPP_OK);
            }

            if(pstTestCtx->u32FramesOut > pstTestCtx->u32OutRxCnt)
                rotate_buffers(p, pstTestCtx->u32FramesOut - pstTestCtx->u32OutRxCnt, ROTATE_OUT_IN,
                               u32MulFactor, u32FrameType);

            pstTestCtx->u32FramesOut = u32TempFrames;
        }

        if(u32ResetHqvCtrlFlag)
            pstTestCtx->u32hqvCtrlSetFlag = 0;

        u32OutFlushFlag = OUT_FLUSH_REQUEST;
        VPP_TERM(p);

        flag_mgr_destroy(&flagmgr);
    }
    else
    {
        LOGI("TEST SKIPPED.");
    }
}

TEST(BufferExchangeFlush_Testcase)
{
    void *p;
    struct vpp_callbacks stCb;
    struct vpp_requirements req;
    struct vpp_port_param param;
    uint32_t u32, u32Frames, u32TempFrames, u32FrameType, u32MulFactor;
    uint32_t u32ResetHqvCtrlFlag = 0;
    struct functional_test_ctx *pFuncTestCtx;
    uint32_t u32TestMask;

    pFuncTestCtx = (struct functional_test_ctx *)pstTestCtx->pPrivateCtx;
    u32TestMask = pFuncTestCtx->u32TestMask;

    if(u32TestMask & FUNCTEST_FLUSH)
    {
        // Callbacks
        stCb.input_buffer_done = buffer_exchange_ibd_cb,
        stCb.output_buffer_done = buffer_exchange_obd_cb,
        stCb.vpp_event = buffer_exchange_evt_cb,

        // Port parameters
        param = pstTestCtx->port_param;

        p = VPP_INIT(get_session_flags(pstTestCtx), stCb);
        DVP_ASSERT_PTR_NNULL(p);

        if(pstTestCtx->u32hqvCtrlSetFlag == 0)
        {
            set_default_hqv_controls();
            u32ResetHqvCtrlFlag = 1;
        }

        u32 = setTestVppHqvCtrl(p, pstTestCtx, &req);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
        bUsecaseBypassesInput = bUsecase_Bypasses(pstTestCtx);

        u32MulFactor = u32GetMulFactor(&param, &req);

        if(pstTestCtx->buf_pool->params.eBufferType == eVppBufType_Interleaved_BFF)
            u32FrameType = eVppBufType_Interleaved_BFF;
        else if(pstTestCtx->buf_pool->params.eBufferType == eVppBufType_Interleaved_TFF)
            u32FrameType = eVppBufType_Interleaved_TFF;
        else
            u32FrameType = eVppBufType_Progressive;

        // Flushing before active should return an error;
        u32 = VPP_FLUSH(p, VPP_PORT_INPUT);
        DVP_ASSERT_NEQUAL(u32, VPP_OK);
        u32OutFlushFlag = OUT_FLUSH_REQUEST;
        u32 = VPP_FLUSH(p, VPP_PORT_OUTPUT);
        DVP_ASSERT_NEQUAL(u32, VPP_OK);


        u32 = VPP_SET_PARAM(p, VPP_PORT_INPUT, param);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        u32 = VPP_SET_PARAM(p, VPP_PORT_OUTPUT, param);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        // Flushing empty queues after active should be ok;
        u32 = VPP_FLUSH(p, VPP_PORT_INPUT);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
        pthread_mutex_lock(&pstTestCtx->mutex);
        pthread_cond_wait(&pstTestCtx->cond, &pstTestCtx->mutex);
        pthread_mutex_unlock(&pstTestCtx->mutex);

        u32OutFlushFlag = OUT_FLUSH_REQUEST;
        u32 = VPP_FLUSH(p, VPP_PORT_OUTPUT);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
        pthread_mutex_lock(&pstTestCtx->mutex);
        pthread_cond_wait(&pstTestCtx->cond, &pstTestCtx->mutex);
        pthread_mutex_unlock(&pstTestCtx->mutex);

        if(u32 == VPP_OK)
        {
            u32TempFrames = pstTestCtx->u32FramesOut;
            snprintf(pstTestCtx->buf_pool->params.cOutputName, MAX_FILE_SEG_LEN,
                     "Flush_%s", pstTestCtx->params.cOutputName);
            if(pstTestCtx->u32FramesOut >= 60) // Test needs at least 60 frames
                u32Frames = pstTestCtx->u32FramesOut;
            else
            {
                u32Frames = 60;
                pstTestCtx->u32FramesOut = u32Frames;
            }
            u32InputFlushDoneFlag = 0;
            u32OutputFlushDoneFlag = 0;
            rotate_buffers(p, 10, ROTATE_IN_OUT, u32MulFactor, u32FrameType);
            u32 = VPP_FLUSH(p, VPP_PORT_INPUT);
            DVP_ASSERT_EQUAL(u32, VPP_OK);
            u32OutFlushFlag = OUT_FLUSH_REQUEST;
            u32 = VPP_FLUSH(p, VPP_PORT_OUTPUT);
            DVP_ASSERT_EQUAL(u32, VPP_OK);

            pthread_mutex_lock(&pstTestCtx->mutex);
            while((u32InputFlushDoneFlag == 0) || (u32OutputFlushDoneFlag == 0))
                pthread_cond_wait(&pstTestCtx->cond, &pstTestCtx->mutex);
            pthread_mutex_unlock(&pstTestCtx->mutex);

            // Restart timestamp
            ts_mgr_destroy(&tsmgr);
            u32 = ts_mgr_init(&tsmgr, TS_FREQ_7HZ, BUFFER_POOL_SZ);
            DVP_ASSERT_EQUAL(u32, VPP_OK);

            // Continue processing
            u32InputFlushDoneFlag = 0;
            rotate_buffers(p, 10, ROTATE_IN_OUT, u32MulFactor, u32FrameType);
            // Flush input only
            u32 = VPP_FLUSH(p, VPP_PORT_INPUT);
            DVP_ASSERT_EQUAL(u32, VPP_OK);

            pthread_mutex_lock(&pstTestCtx->mutex);
            while(u32InputFlushDoneFlag == 0)
                pthread_cond_wait(&pstTestCtx->cond, &pstTestCtx->mutex);
            pthread_mutex_unlock(&pstTestCtx->mutex);

            // Restart timestamp
            ts_mgr_destroy(&tsmgr);
            u32 = ts_mgr_init(&tsmgr, TS_FREQ_7HZ, BUFFER_POOL_SZ);
            DVP_ASSERT_EQUAL(u32, VPP_OK);

            // Continue processing
            u32OutputFlushDoneFlag = 0;
            rotate_buffers(p, 10, ROTATE_IN_OUT, u32MulFactor, u32FrameType);
            // Flush output only
            u32OutFlushFlag = OUT_FLUSH_REQUEST;
            u32 = VPP_FLUSH(p, VPP_PORT_OUTPUT);
            DVP_ASSERT_EQUAL(u32, VPP_OK);

            pthread_mutex_lock(&pstTestCtx->mutex);
            while(u32OutputFlushDoneFlag == 0)
                pthread_cond_wait(&pstTestCtx->cond, &pstTestCtx->mutex);
            pthread_mutex_unlock(&pstTestCtx->mutex);

            // Continue processing, don't need to restart timestamp since input not flushed
            if(pstTestCtx->u32FramesOut > pstTestCtx->u32OutRxCnt)
                rotate_buffers(p, pstTestCtx->u32FramesOut - pstTestCtx->u32OutRxCnt, ROTATE_OUT_IN,
                               u32MulFactor, u32FrameType);

            pstTestCtx->u32FramesOut = u32TempFrames;

        }

        if(u32ResetHqvCtrlFlag)
            pstTestCtx->u32hqvCtrlSetFlag = 0;

        u32OutFlushFlag = OUT_FLUSH_REQUEST;
        VPP_TERM(p);
    }
    else
    {
        LOGI("TEST SKIPPED.");
    }
}

TEST(BufferExchangeReconfigure_Testcase)
{
    void *p;
    struct vpp_callbacks stCb;
    struct vpp_requirements req;
    struct vpp_port_param param;
    t_StVppHqvCtrl hqvCtrlTemp;
    uint32_t u32, u32Frames, u32TempFrames, i, j, u32hqvCtrlSetFlagTemp, u32FrameType, u32MulFactor;
    struct buf_pool *buf_pool_temp;
    struct buf_pool_params buf_pool_params_temp;
    struct functional_test_ctx *pFuncTestCtx;
    uint32_t u32TestMask;

    pFuncTestCtx = (struct functional_test_ctx *)pstTestCtx->pPrivateCtx;
    u32TestMask = pFuncTestCtx->u32TestMask;

    if((u32TestMask & FUNCTEST_RECONFIG_BASIC) || (u32TestMask & FUNCTEST_RECONFIG_ALL))
    {
        for(i = 0; i < pFuncTestCtx->u32StreamCount; i++)
        {
            u32 = init_buf_pool(&pstTestCtx->stVppCtx, &pFuncTestCtx->reconfig_streams[i].reconfig_pool,
                               &pFuncTestCtx->reconfig_streams[i].reconfig_pool_params,
                               BUFFER_POOL_SZ, VPP_FALSE);
            DVP_ASSERT_EQUAL(u32, VPP_OK);
        }

        u32ReconfigDoneFlag = 0;

        // Callbacks
        stCb.input_buffer_done = buffer_exchange_ibd_cb,
        stCb.output_buffer_done = buffer_exchange_obd_cb,
        stCb.vpp_event = buffer_exchange_evt_cb,

        // Port parameters
        param = pstTestCtx->port_param;

        p = VPP_INIT(get_session_flags(pstTestCtx), stCb);
        DVP_ASSERT_PTR_NNULL(p);

        memcpy(&hqvCtrlTemp, &pstTestCtx->hqvCtrl, sizeof(hqvCtrlTemp));
        u32hqvCtrlSetFlagTemp = pstTestCtx->u32hqvCtrlSetFlag;

        if(pstTestCtx->u32hqvCtrlSetFlag == 0)
        {
            set_default_hqv_controls();
        }

        u32 = setTestVppHqvCtrl(p, pstTestCtx, &req);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
        bUsecaseBypassesInput = bUsecase_Bypasses(pstTestCtx);

        u32MulFactor = u32GetMulFactor(&param, &req);

        if(pstTestCtx->buf_pool->params.eBufferType == eVppBufType_Interleaved_BFF)
            u32FrameType = eVppBufType_Interleaved_BFF;
        else if(pstTestCtx->buf_pool->params.eBufferType == eVppBufType_Interleaved_TFF)
            u32FrameType = eVppBufType_Interleaved_TFF;
        else
            u32FrameType = eVppBufType_Progressive;

        u32 = VPP_SET_PARAM(p, VPP_PORT_INPUT, param);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        u32 = VPP_SET_PARAM(p, VPP_PORT_OUTPUT, param);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        if(u32 == VPP_OK)
        {
            u32TempFrames = pstTestCtx->u32FramesOut;
            if(pstTestCtx->u32FramesOut >= 100) // Test needs at least 100 frames
                u32Frames = pstTestCtx->u32FramesOut;
            else
            {
                u32Frames = 100;
                pstTestCtx->u32FramesOut = u32Frames;
            }
            snprintf(pstTestCtx->buf_pool->params.cOutputName, MAX_FILE_SEG_LEN,
                     "Reconfig_%s", pstTestCtx->params.cOutputName);
            rotate_buffers(p, 10, ROTATE_OUT_IN, u32MulFactor, u32FrameType);

            buf_pool_temp = pstTestCtx->buf_pool;
            buf_pool_params_temp = pstTestCtx->params;

            for(i = 0; i < 2; i++)
            {
                for(j=0; j < pFuncTestCtx->u32StreamCount; j++)
                {
                    u32ReconfigDoneFlag = 0;
                    u32OutputFlushDoneFlag = 0;
                    u32InputFlushDoneFlag = 0;
                    param = pFuncTestCtx->reconfig_streams[j].port_param;

                    if((param.width > 1920) || (param.height > 1080))
                        continue;
                    u32 = VPP_RECONFIG(p, param, param);
                    wait_reconfig_done(p, u32MulFactor, u32FrameType);

                    u32MulFactor = u32GetMulFactor(&param, &req);
                    LOGD("Reconfigured new input stream: %dx%d", param.width, param.height);
                    u32 = VPP_FLUSH(p, VPP_PORT_INPUT);
                    DVP_ASSERT_EQUAL(u32, VPP_OK);
                    u32OutFlushFlag = OUT_FLUSH_REQUEST;
                    u32 = VPP_FLUSH(p, VPP_PORT_OUTPUT);
                    DVP_ASSERT_EQUAL(u32, VPP_OK);

                    pthread_mutex_lock(&pstTestCtx->mutex);
                    while((u32OutputFlushDoneFlag == 0) || (u32InputFlushDoneFlag == 0))
                        pthread_cond_wait(&pstTestCtx->cond, &pstTestCtx->mutex);
                    pthread_mutex_unlock(&pstTestCtx->mutex);

                    pstTestCtx->buf_pool = pFuncTestCtx->reconfig_streams[j].reconfig_pool;
                    pstTestCtx->params = pFuncTestCtx->reconfig_streams[j].reconfig_pool_params;
                    pstTestCtx->buf_pool->u32WrIdx = 0;
                    snprintf(pFuncTestCtx->reconfig_streams[j].reconfig_pool->params.cOutputName,
                            MAX_FILE_SEG_LEN, "Reconfig%d_%s", i,
                            pFuncTestCtx->reconfig_streams[j].reconfig_pool_params.cOutputName);
                    rotate_buffers(p, 10, ROTATE_OUT_IN, u32MulFactor, u32FrameType);
                }
            }

            u32ReconfigDoneFlag = 0;
            u32OutputFlushDoneFlag = 0;
            u32InputFlushDoneFlag = 0;

            param = pstTestCtx->port_param;
            u32 = VPP_RECONFIG(p, param, param);

            // Set control during reconfigure should be ok
            if((get_uc_algo_enabled(p, HQV_CONTROL_CADE)) && (pstTestCtx->hqvCtrl.mode == HQV_MODE_MANUAL))
            {
                LOGD("Setting CADE to Manual Level 100 during reconfig.");
                pstTestCtx->hqvCtrl.mode = HQV_MODE_MANUAL;
                pstTestCtx->u32hqvCtrlSetFlag |= (1 << HQV_CONTROL_CADE);
                pstTestCtx->hqvCtrl.cade.mode = HQV_MODE_MANUAL;
                pstTestCtx->hqvCtrl.cade.cade_level = 100;
                u32 = setTestVppHqvCtrl(p, pstTestCtx, &req);
                DVP_ASSERT_EQUAL(u32, VPP_OK);
            }
            else if((get_uc_algo_enabled(p, HQV_CONTROL_AIE)) && (pstTestCtx->hqvCtrl.mode == HQV_MODE_MANUAL))
            {
                LOGD("Setting CADE to Manual Level 100 during reconfig.");
                pstTestCtx->hqvCtrl.mode = HQV_MODE_MANUAL;
                pstTestCtx->u32hqvCtrlSetFlag |= (1 << HQV_CONTROL_AIE);
                pstTestCtx->hqvCtrl.aie.mode = HQV_MODE_MANUAL;
                pstTestCtx->hqvCtrl.aie.cade_level = 100;
                u32 = setTestVppHqvCtrl(p, pstTestCtx, &req);
                DVP_ASSERT_EQUAL(u32, VPP_OK);
            }
            wait_reconfig_done(p, u32MulFactor, u32FrameType);

            u32MulFactor = u32GetMulFactor(&param, &req);

            LOGD("Reconfigured new input stream: %dx%d", param.width, param.height);
            u32 = VPP_FLUSH(p, VPP_PORT_INPUT);
            DVP_ASSERT_EQUAL(u32, VPP_OK);
            u32OutFlushFlag = OUT_FLUSH_REQUEST;
            u32 = VPP_FLUSH(p, VPP_PORT_OUTPUT);
            DVP_ASSERT_EQUAL(u32, VPP_OK);

            pthread_mutex_lock(&pstTestCtx->mutex);
            while((u32OutputFlushDoneFlag == 0) || (u32InputFlushDoneFlag == 0))
                pthread_cond_wait(&pstTestCtx->cond, &pstTestCtx->mutex);
            pthread_mutex_unlock(&pstTestCtx->mutex);

            pstTestCtx->buf_pool = buf_pool_temp;
            pstTestCtx->params = buf_pool_params_temp;

            if(pstTestCtx->u32FramesOut > pstTestCtx->u32OutRxCnt)
                rotate_buffers(p, pstTestCtx->u32FramesOut - pstTestCtx->u32OutRxCnt, ROTATE_OUT_IN,
                               u32MulFactor, u32FrameType);

            pstTestCtx->u32FramesOut = u32TempFrames;

        }

        memcpy(&pstTestCtx->hqvCtrl, &hqvCtrlTemp, sizeof(hqvCtrlTemp));
        pstTestCtx->u32hqvCtrlSetFlag = u32hqvCtrlSetFlagTemp;

        u32OutFlushFlag = OUT_FLUSH_REQUEST;
        VPP_TERM(p);

        for(i = 0; i < pFuncTestCtx->u32StreamCount; i++)
        {
            free_buf_pool(pFuncTestCtx->reconfig_streams[i].reconfig_pool, VPP_FALSE);
        }
    }
    else
    {
        LOGI("TEST SKIPPED.");
    }
}

TEST(BufferExchangeReconfigError_Testcase)
{
    void *p;
    struct vpp_callbacks stCb;
    struct vpp_requirements req;
    struct vpp_port_param param;
    struct vpp_buffer fakebuf;
    uint32_t u32, u32Frames, u32TempFrames, u32TempFileType, i, u32MulFactor;
    struct buf_pool *buf_pool_temp;
    struct buf_pool_params buf_pool_params_temp;
    uint32_t u32OldBuffSize, u32FrameType;
    uint32_t u32ResetHqvCtrlFlag = 0;
    struct functional_test_ctx *pFuncTestCtx;
    uint32_t u32TestMask;

    pFuncTestCtx = (struct functional_test_ctx *)pstTestCtx->pPrivateCtx;
    u32TestMask = pFuncTestCtx->u32TestMask;

    if(u32TestMask & FUNCTEST_RECONFIG_ALL)
    {
        for(i = 0; i < pFuncTestCtx->u32StreamCount; i++)
        {
            u32 = init_buf_pool(&pstTestCtx->stVppCtx, &pFuncTestCtx->reconfig_streams[i].reconfig_pool,
                               &pFuncTestCtx->reconfig_streams[i].reconfig_pool_params,
                               BUFFER_POOL_SZ, VPP_FALSE);
            DVP_ASSERT_EQUAL(u32, VPP_OK);
        }

        // Callbacks
        stCb.input_buffer_done = buffer_exchange_ibd_cb,
        stCb.output_buffer_done = buffer_exchange_obd_cb,
        stCb.vpp_event = buffer_exchange_evt_cb,

        // Port parameters
        param = pstTestCtx->port_param;

        p = VPP_INIT(get_session_flags(pstTestCtx), stCb);
        DVP_ASSERT_PTR_NNULL(p);

        if(pstTestCtx->u32hqvCtrlSetFlag == 0)
        {
            set_default_hqv_controls();
            u32ResetHqvCtrlFlag = 1;
        }

        u32 = setTestVppHqvCtrl(p, pstTestCtx, &req);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
        bUsecaseBypassesInput = bUsecase_Bypasses(pstTestCtx);

        u32MulFactor = u32GetMulFactor(&param, &req);

        if(pstTestCtx->buf_pool->params.eBufferType == eVppBufType_Interleaved_BFF)
            u32FrameType = eVppBufType_Interleaved_BFF;
        else if(pstTestCtx->buf_pool->params.eBufferType == eVppBufType_Interleaved_TFF)
            u32FrameType = eVppBufType_Interleaved_TFF;
        else
            u32FrameType = eVppBufType_Progressive;

        u32 = VPP_SET_PARAM(p, VPP_PORT_INPUT, param);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        u32 = VPP_SET_PARAM(p, VPP_PORT_OUTPUT, param);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        if(u32 == VPP_OK)
        {
            u32TempFileType = pstTestCtx->buf_pool->params.eOutputFileType;
            pstTestCtx->buf_pool->params.eOutputFileType = FILE_TYPE_NONE;
            u32TempFrames = pstTestCtx->u32FramesOut;
            if(pstTestCtx->u32FramesOut >= 100) // Test needs at least 100 frames
                u32Frames = pstTestCtx->u32FramesOut;
            else
            {
                u32Frames = 100;
                pstTestCtx->u32FramesOut = u32Frames;
            }
            snprintf(pstTestCtx->buf_pool->params.cOutputName, MAX_FILE_SEG_LEN,
                     "ReconfigError_%s", pstTestCtx->params.cOutputName);
            rotate_buffers(p, 10, ROTATE_OUT_IN, u32MulFactor, u32FrameType);
            // Queue an extra buffer on the output
            u32 = queue_buffer(p, VPP_PORT_OUTPUT, u32MulFactor, u32FrameType);
            DVP_ASSERT_EQUAL(u32, VPP_OK);
            LOGE_IF(u32 != VPP_OK, "Queue buffer returned=%u", u32);

            buf_pool_temp = pstTestCtx->buf_pool;
            buf_pool_params_temp = pstTestCtx->params;

            for(i=0; i < pFuncTestCtx->u32StreamCount; i++)
            {
                u32ReconfigDoneFlag = 0;
                u32OldBuffSize = param.width * param.height;
                param = pFuncTestCtx->reconfig_streams[i].port_param;
                if((param.width > 1920) || (param.height > 1080))
                    continue;

                u32 = VPP_RECONFIG(p, param, param);
                wait_reconfig_done(p, u32MulFactor, u32FrameType);

                u32MulFactor = u32GetMulFactor(&param, &req);
                LOGD("Reconfigured %dx%d, still %d old output buffers queued. PURPOSELY NOT FLUSHED",
                     param.width, param.height, pstTestCtx->u32OutHoldCnt);
                if(param.height * param.width > u32OldBuffSize)
                    u32ExpectedErrBufCnt += pstTestCtx->u32OutHoldCnt;

                pstTestCtx->buf_pool = pFuncTestCtx->reconfig_streams[i].reconfig_pool;
                pstTestCtx->params = pFuncTestCtx->reconfig_streams[i].reconfig_pool_params;
                pstTestCtx->buf_pool->u32WrIdx = 0;
                snprintf(pFuncTestCtx->reconfig_streams[i].reconfig_pool->params.cOutputName,
                         MAX_FILE_SEG_LEN, "ReconfigError_%s",
                         pFuncTestCtx->reconfig_streams[i].reconfig_pool_params.cOutputName);
                pstTestCtx->buf_pool->params.eOutputFileType = FILE_TYPE_NONE;
                // Queue an extra buffer on the input
                u32 = queue_buffer(p, VPP_PORT_INPUT, u32MulFactor, u32FrameType);
                DVP_ASSERT_EQUAL(u32, VPP_OK);
                LOGE_IF(u32 != VPP_OK, "Queue buffer returned=%u", u32);

                rotate_buffers(p, 10, ROTATE_OUT_IN, u32MulFactor, u32FrameType);

                // Queue an extra buffer on the output
                u32 = queue_buffer(p, VPP_PORT_OUTPUT, u32MulFactor, u32FrameType);
                u32 = queue_buffer(p, VPP_PORT_OUTPUT, u32MulFactor, u32FrameType);
                DVP_ASSERT_EQUAL(u32, VPP_OK);
                LOGE_IF(u32 != VPP_OK, "Queue buffer returned=%u", u32);
            }

            u32ReconfigDoneFlag = 0;
            u32OutputFlushDoneFlag = 0;
            u32InputFlushDoneFlag = 0;

            param = pstTestCtx->port_param;
            u32 = VPP_RECONFIG(p, param, param);

            u32MulFactor = u32GetMulFactor(&param, &req);

#ifdef VPP_SERVICE
            /* Fake buffer params needs to be initialized as these params are accessed in the HIDL layer*/
            fakebuf.pvGralloc = nullptr;
            fakebuf.pixel.fd = fakebuf.extradata.fd = -1;
#endif
            // Queue an input buffer during reconfig. Should return error. Use a fake buffer to avoid timestamp errors
            u32 = VPP_QUEUE_BUF(p, VPP_PORT_INPUT, &fakebuf);
            DVP_ASSERT_NEQUAL(u32, VPP_OK);

            // Queue an output buffer during reconfig is allowed.
            u32 = queue_buffer(p, VPP_PORT_OUTPUT, u32MulFactor, u32FrameType);
            DVP_ASSERT_EQUAL(u32, VPP_OK);

            wait_reconfig_done(p, u32MulFactor, u32FrameType);

            LOGD("Reconfigured new input stream: %dx%d", param.width, param.height);
            u32 = VPP_FLUSH(p, VPP_PORT_INPUT);
            DVP_ASSERT_EQUAL(u32, VPP_OK);
            u32OutFlushFlag = OUT_FLUSH_REQUEST;
            u32 = VPP_FLUSH(p, VPP_PORT_OUTPUT);
            DVP_ASSERT_EQUAL(u32, VPP_OK);

            pthread_mutex_lock(&pstTestCtx->mutex);
            while((u32OutputFlushDoneFlag == 0) || (u32InputFlushDoneFlag == 0))
                pthread_cond_wait(&pstTestCtx->cond, &pstTestCtx->mutex);
            pthread_mutex_unlock(&pstTestCtx->mutex);

            pstTestCtx->buf_pool = buf_pool_temp;
            pstTestCtx->params = buf_pool_params_temp;

            u32 = queue_buffer(p, VPP_PORT_INPUT, u32MulFactor, u32FrameType);
            DVP_ASSERT_EQUAL(u32, VPP_OK);

            if(pstTestCtx->u32FramesOut > pstTestCtx->u32OutRxCnt)
                rotate_buffers(p, pstTestCtx->u32FramesOut - pstTestCtx->u32OutRxCnt, ROTATE_OUT_IN,
                               u32MulFactor, u32FrameType);

            pstTestCtx->u32FramesOut = u32TempFrames;
            pstTestCtx->buf_pool->params.eOutputFileType = (enum file_type)u32TempFileType;
            for(i=0; i < pFuncTestCtx->u32StreamCount; i++)
            {
                pFuncTestCtx->reconfig_streams[i].reconfig_pool->params.eOutputFileType = (enum file_type)u32TempFileType;
            }

        }

        if(u32ResetHqvCtrlFlag)
            pstTestCtx->u32hqvCtrlSetFlag = 0;

    //    LOGD("Received %d error output buffers, expected %d", u32ErrorBufferCount, u32ExpectedErrBufCnt);
    //    DVP_ASSERT_EQUAL(u32ExpectedErrBufCnt, u32ErrorBufferCount);

        u32OutFlushFlag = OUT_FLUSH_REQUEST;
        VPP_TERM(p);

        for(i = 0; i < pFuncTestCtx->u32StreamCount; i++)
        {
            free_buf_pool(pFuncTestCtx->reconfig_streams[i].reconfig_pool, VPP_FALSE);
        }
    }
    else
    {
        LOGI("TEST SKIPPED.");
    }
}


TEST(BufferExchangeStress_Testcase)
{
    void *p;
    struct vpp_callbacks stCb;
    struct vpp_requirements req;
    struct vpp_port_param param;
    uint32_t u32, u32Frames, u32TempFrames, u32TempFileType, u32FrameType, u32MulFactor;
    uint32_t u32ResetHqvCtrlFlag = 0;
    struct functional_test_ctx *pFuncTestCtx;
    uint32_t u32TestMask;

    pFuncTestCtx = (struct functional_test_ctx *)pstTestCtx->pPrivateCtx;
    u32TestMask = pFuncTestCtx->u32TestMask;

    if(u32TestMask & FUNCTEST_STRESS)
    {
        // Callbacks
        stCb.input_buffer_done = buffer_exchange_ibd_cb,
        stCb.output_buffer_done = buffer_exchange_obd_cb,
        stCb.vpp_event = buffer_exchange_evt_cb,

        // Port parameters
        param = pstTestCtx->port_param;

        p = VPP_INIT(get_session_flags(pstTestCtx), stCb);
        DVP_ASSERT_PTR_NNULL(p);

        if(pstTestCtx->u32hqvCtrlSetFlag == 0)
        {
            set_default_hqv_controls();
            u32ResetHqvCtrlFlag = 1;
        }

        u32 = setTestVppHqvCtrl(p, pstTestCtx, &req);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
        bUsecaseBypassesInput = bUsecase_Bypasses(pstTestCtx);

        u32MulFactor = u32GetMulFactor(&param, &req);

        if(pstTestCtx->buf_pool->params.eBufferType == eVppBufType_Interleaved_BFF)
            u32FrameType = eVppBufType_Interleaved_BFF;
        else if(pstTestCtx->buf_pool->params.eBufferType == eVppBufType_Interleaved_TFF)
            u32FrameType = eVppBufType_Interleaved_TFF;
        else
            u32FrameType = eVppBufType_Progressive;

        u32 = VPP_SET_PARAM(p, VPP_PORT_INPUT, param);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        u32 = VPP_SET_PARAM(p, VPP_PORT_OUTPUT, param);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        if(u32 == VPP_OK)
        {
            u32TempFileType = pstTestCtx->buf_pool->params.eOutputFileType;
            pstTestCtx->buf_pool->params.eOutputFileType = FILE_TYPE_NONE;
            u32Frames = STRESS_TEST_FRAMES;
            u32TempFrames = pstTestCtx->u32FramesOut;
            pstTestCtx->u32FramesOut = u32Frames;

            rotate_buffers(p, u32Frames, ROTATE_OUT_IN, u32MulFactor, u32FrameType);

            pstTestCtx->u32FramesOut = u32TempFrames;
            pstTestCtx->buf_pool->params.eOutputFileType = (enum file_type)u32TempFileType;
        }

        if(u32ResetHqvCtrlFlag)
            pstTestCtx->u32hqvCtrlSetFlag = 0;

        u32OutFlushFlag = OUT_FLUSH_REQUEST;
        VPP_TERM(p);
    }
    else
    {
        LOGI("TEST SKIPPED.");
    }
}

#ifndef VPP_SERVICE
TEST(BufferExchangeFrameRateBypass_Testcase)
{
    void *p;
    struct vpp_callbacks stCb;
    struct vpp_requirements req;
    struct vpp_port_param param;
    uint32_t u32, u32Frames, u32TempFrames, u32FrameType, u32MulFactor;
    uint32_t u32ResetHqvCtrlFlag = 0;
    struct functional_test_ctx *pFuncTestCtx;
    uint32_t u32TestMask;

    pFuncTestCtx = (struct functional_test_ctx *)pstTestCtx->pPrivateCtx;
    u32TestMask = pFuncTestCtx->u32TestMask;

    if(u32TestMask & FUNCTEST_BYPASS)
    {
        // Callbacks
        stCb.input_buffer_done = buffer_exchange_ibd_cb,
        stCb.output_buffer_done = buffer_exchange_obd_cb,
        stCb.vpp_event = buffer_exchange_evt_cb,

        // Port parameters
        param = pstTestCtx->port_param;

        p = VPP_INIT(get_session_flags(pstTestCtx), stCb);
        DVP_ASSERT_PTR_NNULL(p);

        if(pstTestCtx->u32hqvCtrlSetFlag == 0)
        {
            set_default_hqv_controls();
            u32ResetHqvCtrlFlag = 1;
        }

        u32 = setTestVppHqvCtrl(p, pstTestCtx, &req);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
        bUsecaseBypassesInput = bUsecase_Bypasses(pstTestCtx);

        u32MulFactor = u32GetMulFactor(&param, &req);

        if(pstTestCtx->buf_pool->params.eBufferType == eVppBufType_Interleaved_BFF)
            u32FrameType = eVppBufType_Interleaved_BFF;
        else if(pstTestCtx->buf_pool->params.eBufferType == eVppBufType_Interleaved_TFF)
            u32FrameType = eVppBufType_Interleaved_TFF;
        else
            u32FrameType = eVppBufType_Progressive;

        u32 = VPP_SET_PARAM(p, VPP_PORT_INPUT, param);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        u32 = VPP_SET_PARAM(p, VPP_PORT_OUTPUT, param);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        if(u32 == VPP_OK)
        {
            u32TempFrames = pstTestCtx->u32FramesOut;
            snprintf(pstTestCtx->buf_pool->params.cOutputName, MAX_FILE_SEG_LEN,
                     "FramerateBypass_%s", pstTestCtx->params.cOutputName);
            if(pstTestCtx->u32FramesOut >= 50)
                u32Frames = pstTestCtx->u32FramesOut;
            else
            {
                u32Frames = 50;
                pstTestCtx->u32FramesOut = u32Frames;
            }
            rotate_buffers(p, 5, ROTATE_OUT_IN, u32MulFactor, u32FrameType);

            tsmgr.freq = TS_FREQ_120HZ;
            tsmgr.u64TsStep = 1000000 / tsmgr.freq;
            LOGD("Changing to 120Hz");
            rotate_buffers(p, 5, ROTATE_OUT_IN, u32MulFactor, u32FrameType);

            tsmgr.freq = TS_FREQ_7HZ;
            tsmgr.u64TsStep = 1000000 / tsmgr.freq;
            LOGD("Changing to 7Hz");
            if(pstTestCtx->u32FramesOut > pstTestCtx->u32OutRxCnt)
                rotate_buffers(p, pstTestCtx->u32FramesOut - pstTestCtx->u32OutRxCnt, ROTATE_OUT_IN,
                               u32MulFactor, u32FrameType);

            pstTestCtx->u32FramesOut = u32TempFrames;
        }

        if(u32ResetHqvCtrlFlag)
            pstTestCtx->u32hqvCtrlSetFlag = 0;

        u32OutFlushFlag = OUT_FLUSH_REQUEST;
        VPP_TERM(p);
    }
    else
    {
        LOGI("TEST SKIPPED.");
    }
}
#endif

TEST(BufferExchangeNoUcBypass_Testcase)
{
    void *p;
    struct vpp_callbacks stCb;
    struct vpp_requirements req;
    struct vpp_port_param param;
    t_StVppHqvCtrl hqvCtrlTemp;
    uint32_t u32, u32Frames, u32hqvCtrlSetFlagTemp, u32MulFactor, i;
    struct functional_test_ctx *pFuncTestCtx;
    uint32_t u32TestMask;

    pFuncTestCtx = (struct functional_test_ctx *)pstTestCtx->pPrivateCtx;
    u32TestMask = pFuncTestCtx->u32TestMask;

    if(u32TestMask & FUNCTEST_BYPASS)
    {
        // Callbacks
        stCb.input_buffer_done = buffer_exchange_ibd_cb,
        stCb.output_buffer_done = buffer_exchange_obd_cb,
        stCb.vpp_event = buffer_exchange_evt_cb,

        // Port parameters
        param = pstTestCtx->port_param;

        p = VPP_INIT(get_session_flags(pstTestCtx), stCb);
        DVP_ASSERT_PTR_NNULL(p);

        memcpy(&hqvCtrlTemp, &pstTestCtx->hqvCtrl, sizeof(hqvCtrlTemp));
        u32hqvCtrlSetFlagTemp = pstTestCtx->u32hqvCtrlSetFlag;

        pstTestCtx->hqvCtrl.mode = HQV_MODE_MANUAL;
        pstTestCtx->hqvCtrl.aie.mode = HQV_MODE_AUTO;
        pstTestCtx->hqvCtrl.tnr.mode = HQV_MODE_AUTO;
        pstTestCtx->hqvCtrl.cnr.mode = HQV_MODE_AUTO;
        pstTestCtx->u32hqvCtrlSetFlag |= (1 << HQV_CONTROL_AIE) | (1 << HQV_CONTROL_TNR) | (1 << HQV_CONTROL_CNR);
        u32 = setTestVppHqvCtrl(p, pstTestCtx, &req);
        DVP_ASSERT_NEQUAL(u32, VPP_OK);

        u32 = VPP_SET_PARAM(p, VPP_PORT_INPUT, param);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        u32 = VPP_SET_PARAM(p, VPP_PORT_OUTPUT, param);
        DVP_ASSERT_NEQUAL(u32, VPP_OK);

        u32MulFactor = u32GetMulFactor(&param, &req);

        snprintf(pstTestCtx->buf_pool->params.cOutputName, MAX_FILE_SEG_LEN,
                 "NoUcBypass_%s", pstTestCtx->params.cOutputName);
        if(pstTestCtx->u32FramesOut)
            u32Frames = pstTestCtx->u32FramesOut;
        else
        {
            u32Frames = 20;
            pstTestCtx->u32FramesOut = u32Frames;
        }
        u32BypassFlag |= BYPASS_FORCE;
        rotate_buffers(p, 5, ROTATE_OUT_IN, 1, eVppBufType_Progressive);
        pthread_mutex_lock(&pstTestCtx->mutex);
        while(pstTestCtx->u32InHoldCnt != 0)
            pthread_cond_wait(&pstTestCtx->cond, &pstTestCtx->mutex);
        pthread_mutex_unlock(&pstTestCtx->mutex);
        for (i = 0; i < 20; i++)
        {
            queue_buffer(p, VPP_PORT_INPUT, u32MulFactor, eVppBufType_Progressive);
        }
        u32 = VPP_FLUSH(p, VPP_PORT_INPUT);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
        pthread_mutex_lock(&pstTestCtx->mutex);
        while(pstTestCtx->u32InHoldCnt != 0)
            pthread_cond_wait(&pstTestCtx->cond, &pstTestCtx->mutex);
        pthread_mutex_unlock(&pstTestCtx->mutex);

        memcpy(&pstTestCtx->hqvCtrl, &hqvCtrlTemp, sizeof(hqvCtrlTemp));
        pstTestCtx->u32hqvCtrlSetFlag = u32hqvCtrlSetFlagTemp;

        u32OutFlushFlag = OUT_FLUSH_REQUEST;
        VPP_TERM(p);
    }
    else
    {
        LOGI("TEST SKIPPED.");
    }
}

TEST(BufferExchangeInterlacedBypass_Testcase)
{
    void *p;
    struct vpp_callbacks stCb;
    struct vpp_requirements req;
    struct vpp_port_param param;
    t_StVppHqvCtrl hqvCtrlTemp;
    uint32_t u32, u32Frames, u32TempFrames, u32hqvCtrlSetFlagTemp, u32MulFactor;
    uint32_t u32ResetHqvCtrlFlag = 0;
    struct functional_test_ctx *pFuncTestCtx;
    uint32_t u32TestMask;

    pFuncTestCtx = (struct functional_test_ctx *)pstTestCtx->pPrivateCtx;
    u32TestMask = pFuncTestCtx->u32TestMask;

    if(u32TestMask & FUNCTEST_BYPASS)
    {
        // Callbacks
        stCb.input_buffer_done = buffer_exchange_ibd_cb,
        stCb.output_buffer_done = buffer_exchange_obd_cb,
        stCb.vpp_event = buffer_exchange_evt_cb,

        // Port parameters
        param = pstTestCtx->port_param;

        p = VPP_INIT(get_session_flags(pstTestCtx), stCb);
        DVP_ASSERT_PTR_NNULL(p);

        memcpy(&hqvCtrlTemp, &pstTestCtx->hqvCtrl, sizeof(hqvCtrlTemp));
        u32hqvCtrlSetFlagTemp = pstTestCtx->u32hqvCtrlSetFlag;

        if(pstTestCtx->u32hqvCtrlSetFlag == 0)
        {
            set_default_hqv_controls();
            u32ResetHqvCtrlFlag = 1;
        }

#if defined(VPP_TARGET_USES_HVX)
        pstTestCtx->hqvCtrl.mode = HQV_MODE_MANUAL;
        pstTestCtx->hqvCtrl.aie.mode = HQV_MODE_AUTO;
        pstTestCtx->u32hqvCtrlSetFlag = (1 << HQV_CONTROL_AIE);
#else
        // Currently only HVX supports DI
        // 1. AUTO mode will inevitably force bypass due to interlaced inputs
        // 2. Explicitly unset the DI ctrl in case it's set through cmd line
        pstTestCtx->u32hqvCtrlSetFlag &= ~(1 << HQV_CONTROL_DI);
#endif
        u32 = setTestVppHqvCtrl(p, pstTestCtx, &req);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
        bUsecaseBypassesInput = bUsecase_Bypasses(pstTestCtx);

        u32MulFactor = u32GetMulFactor(&param, &req);

        u32 = VPP_SET_PARAM(p, VPP_PORT_INPUT, param);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        u32 = VPP_SET_PARAM(p, VPP_PORT_OUTPUT, param);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        if(u32 == VPP_OK)
        {
            snprintf(pstTestCtx->buf_pool->params.cOutputName, MAX_FILE_SEG_LEN,
                     "InterlacedBypass_%s", pstTestCtx->params.cOutputName);
            u32TempFrames = pstTestCtx->u32FramesOut;
            if(pstTestCtx->u32FramesOut >= 30)
                u32Frames = pstTestCtx->u32FramesOut;
            else
            {
                u32Frames = 30;
                pstTestCtx->u32FramesOut = u32Frames;
            }

            rotate_buffers(p, 2, ROTATE_OUT_IN, 1, eVppBufType_Progressive);
            rotate_buffers_insert_flag(p, 0, 0, 1, 1, eVppBufType_Interleaved_TFF);
            rotate_buffers_insert_flag(p, 0, 0, 1, 1, eVppBufType_Interleaved_TFF);
            rotate_buffers_insert_flag(p, 0, 0, 1, 1, eVppBufType_Interleaved_TFF);
            rotate_buffers_insert_flag(p, 0, 0, 1, 1, eVppBufType_Interleaved_TFF);
            rotate_buffers_insert_flag(p, 0, 0, 1, 1, eVppBufType_Interleaved_TFF);
            rotate_buffers_insert_flag(p, 0, 0, 1, 1, eVppBufType_Interleaved_TFF);
            rotate_buffers_insert_flag(p, 0, 0, 1, 1, eVppBufType_Interleaved_TFF);
            rotate_buffers_insert_flag(p, 0, 0, 1, 1, eVppBufType_Interleaved_TFF);
            rotate_buffers_insert_flag(p, 0, 0, 1, 1, eVppBufType_Interleaved_TFF);
            rotate_buffers_insert_flag(p, 0, 0, 1, 1, eVppBufType_Interleaved_TFF);
            rotate_buffers_insert_flag(p, 0, 0, 1, 1, eVppBufType_Interleaved_TFF);
            rotate_buffers_insert_flag(p, 0, 0, 1, 1, eVppBufType_Interleaved_TFF);
            rotate_buffers_insert_flag(p, 0, 0, 1, 0, eVppBufType_Progressive);
            if(pstTestCtx->u32FramesOut > pstTestCtx->u32OutRxCnt)
                rotate_buffers(p, pstTestCtx->u32FramesOut - pstTestCtx->u32OutRxCnt, ROTATE_OUT_IN,
                               u32MulFactor, eVppBufType_Progressive);

            memcpy(&pstTestCtx->hqvCtrl, &hqvCtrlTemp, sizeof(hqvCtrlTemp));
            pstTestCtx->u32hqvCtrlSetFlag = u32hqvCtrlSetFlagTemp;
            pstTestCtx->u32FramesOut = u32TempFrames;
        }
        u32OutFlushFlag = OUT_FLUSH_REQUEST;
        VPP_TERM(p);
    }
    else
    {
        LOGI("TEST SKIPPED.");
    }
}


TEST(BufferExchangeExtraStreamBypass_Testcase)
{
    void *p, *p2, *p3;
    struct vpp_callbacks stCb;
    struct vpp_requirements req;
    struct vpp_port_param param;
    uint32_t u32, u32Frames, u32FrameType, u32MulFactor;
    uint32_t u32ResetHqvCtrlFlag = 0;
    struct functional_test_ctx *pFuncTestCtx;
    uint32_t u32TestMask;

    pFuncTestCtx = (struct functional_test_ctx *)pstTestCtx->pPrivateCtx;
    u32TestMask = pFuncTestCtx->u32TestMask;

    if(u32TestMask & FUNCTEST_BYPASS)
    {
        // Callbacks
        stCb.input_buffer_done = buffer_exchange_ibd_cb,
        stCb.output_buffer_done = buffer_exchange_obd_cb,
        stCb.vpp_event = buffer_exchange_evt_cb,

        // Port parameters
        param = pstTestCtx->port_param;

        p = VPP_INIT(get_session_flags(pstTestCtx), stCb);
        p2 = VPP_INIT(get_session_flags(pstTestCtx), stCb);
        p3 = VPP_INIT(get_session_flags(pstTestCtx), stCb);
        DVP_ASSERT_PTR_NNULL(p);
        DVP_ASSERT_PTR_NNULL(p2);
        DVP_ASSERT_PTR_NNULL(p3);

        if(pstTestCtx->u32hqvCtrlSetFlag == 0)
        {
            set_default_hqv_controls();
            u32ResetHqvCtrlFlag = 1;
        }

        u32 = setTestVppHqvCtrl(p2, pstTestCtx, &req);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
        bUsecaseBypassesInput = bUsecase_Bypasses(pstTestCtx);

        if(pstTestCtx->buf_pool->params.eBufferType == eVppBufType_Interleaved_BFF)
            u32FrameType = eVppBufType_Interleaved_BFF;
        else if(pstTestCtx->buf_pool->params.eBufferType == eVppBufType_Interleaved_TFF)
            u32FrameType = eVppBufType_Interleaved_TFF;
        else
            u32FrameType = eVppBufType_Progressive;

        u32 = VPP_SET_PARAM(p2, VPP_PORT_INPUT, param);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        u32 = VPP_SET_PARAM(p2, VPP_PORT_OUTPUT, param);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        u32 = setTestVppHqvCtrl(p3, pstTestCtx, &req);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        u32 = VPP_SET_PARAM(p3, VPP_PORT_INPUT, param);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        u32 = VPP_SET_PARAM(p3, VPP_PORT_OUTPUT, param);

        u32 = setTestVppHqvCtrl(p, pstTestCtx, &req);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        u32 = VPP_SET_PARAM(p, VPP_PORT_INPUT, param);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        u32 = VPP_SET_PARAM(p, VPP_PORT_OUTPUT, param);
        if (u32 != VPP_OK)
        {
            u32MulFactor = u32GetMulFactor(&param, &req);

            snprintf(pstTestCtx->buf_pool->params.cOutputName, MAX_FILE_SEG_LEN,
                     "ExtraStreamBypass_%s", pstTestCtx->params.cOutputName);
            if(pstTestCtx->u32FramesOut)
                u32Frames = pstTestCtx->u32FramesOut;
            else
            {
                u32Frames = 20;
                pstTestCtx->u32FramesOut = u32Frames;
            }
            u32BypassFlag |= BYPASS_FORCE;
            rotate_buffers(p, 10, ROTATE_OUT_IN, u32MulFactor, u32FrameType);
            pthread_mutex_lock(&pstTestCtx->mutex);
            while(pstTestCtx->u32InHoldCnt != 0)
                pthread_cond_wait(&pstTestCtx->cond, &pstTestCtx->mutex);
            pthread_mutex_unlock(&pstTestCtx->mutex);

            if(u32ResetHqvCtrlFlag)
                pstTestCtx->u32hqvCtrlSetFlag = 0;


            u32OutFlushFlag = OUT_FLUSH_REQUEST;
            u32 = VPP_FLUSH(p, VPP_PORT_INPUT);
            DVP_ASSERT_EQUAL(u32, VPP_OK);
            pthread_mutex_lock(&pstTestCtx->mutex);
            while(pstTestCtx->u32InHoldCnt != 0)
                pthread_cond_wait(&pstTestCtx->cond, &pstTestCtx->mutex);
            pthread_mutex_unlock(&pstTestCtx->mutex);
        }
        VPP_TERM(p);
        VPP_TERM(p2);
        VPP_TERM(p3);
    }
    else
    {
        LOGI("TEST SKIPPED.");
    }
}

// This test starts with initial stream, then tries to find a reconfigure stream above max
// supported to test bypass. Sets max + 1 size if not found.
TEST(BufferExchangeReconfigBypass_Testcase)
{
    void *p;
    struct vpp_callbacks stCb;
    struct vpp_requirements req;
    struct vpp_port_param param;
    uint32_t u32, u32Frames, u32TempFrames, i, u32FrameType, u32MulFactor;
    uint32_t u32ResetHqvCtrlFlag = 0;
    struct buf_pool *buf_pool_temp;
    struct buf_pool_params buf_pool_params_temp;
    struct functional_test_ctx *pFuncTestCtx;
    uint32_t u32TestMask, u32MaxRes, u32MaxH, u32MaxW;
#ifndef VPP_SERVICE
    t_StVppUsecase *pstUc;
#endif

    pFuncTestCtx = (struct functional_test_ctx *)pstTestCtx->pPrivateCtx;
    u32TestMask = pFuncTestCtx->u32TestMask;

    if((u32TestMask & FUNCTEST_BYPASS) || (u32TestMask & FUNCTEST_RECONFIG_ALL))
    {
        for(i = 0; i < pFuncTestCtx->u32StreamCount; i++)
        {
            u32 = init_buf_pool(&pstTestCtx->stVppCtx, &pFuncTestCtx->reconfig_streams[i].reconfig_pool,
                               &pFuncTestCtx->reconfig_streams[i].reconfig_pool_params,
                               BUFFER_POOL_SZ, VPP_FALSE);
            DVP_ASSERT_EQUAL(u32, VPP_OK);
        }

        // Callbacks
        stCb.input_buffer_done = buffer_exchange_ibd_cb,
        stCb.output_buffer_done = buffer_exchange_obd_cb,
        stCb.vpp_event = buffer_exchange_evt_cb,

        // Port parameters
        param = pstTestCtx->port_param;

        p = VPP_INIT(get_session_flags(pstTestCtx), stCb);
        DVP_ASSERT_PTR_NNULL(p);

        if(pstTestCtx->u32hqvCtrlSetFlag == 0)
        {
            set_default_hqv_controls();
            u32ResetHqvCtrlFlag = 1;
        }

        u32 = setTestVppHqvCtrl(p, pstTestCtx, &req);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
        bUsecaseBypassesInput = bUsecase_Bypasses(pstTestCtx);

        u32MulFactor = u32GetMulFactor(&param, &req);

        if(pstTestCtx->buf_pool->params.eBufferType == eVppBufType_Interleaved_BFF)
            u32FrameType = eVppBufType_Interleaved_BFF;
        else if(pstTestCtx->buf_pool->params.eBufferType == eVppBufType_Interleaved_TFF)
            u32FrameType = eVppBufType_Interleaved_TFF;
        else
            u32FrameType = eVppBufType_Progressive;

        u32 = VPP_SET_PARAM(p, VPP_PORT_INPUT, param);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        u32 = VPP_SET_PARAM(p, VPP_PORT_OUTPUT, param);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        if(u32 == VPP_OK)
        {
            u32TempFrames = pstTestCtx->u32FramesOut;
            buf_pool_temp = pstTestCtx->buf_pool;
            buf_pool_params_temp = pstTestCtx->params;

            snprintf(pstTestCtx->buf_pool->params.cOutputName, MAX_FILE_SEG_LEN,
                     "ReconfigBypass_%s", pstTestCtx->params.cOutputName);
            if(pstTestCtx->u32FramesOut >= 50)
                u32Frames = pstTestCtx->u32FramesOut;
            else
            {
                u32Frames = 50;
                pstTestCtx->u32FramesOut = u32Frames;
            }
            rotate_buffers(p, 5, ROTATE_OUT_IN, u32MulFactor, u32FrameType);

#ifndef VPP_SERVICE
            pstUc = ((t_StVppCtx *)p)->pstVppCb->pstUc;
            u32MaxRes = u32VppUsecase_GetMaxResForUc(pstUc);
#else
            u32MaxRes = VPP_RESOLUTION_MAX - 1;
#endif
            u32MaxW = u32GetMaxWidthForRes((t_EVppRes)u32MaxRes);
            u32MaxH = u32GetMaxHeightForRes ((t_EVppRes)u32MaxRes);
            for(i=0; i < pFuncTestCtx->u32StreamCount; i++)
            {
                param = pFuncTestCtx->reconfig_streams[i].port_param;
                if((param.width > u32MaxW) || (param.height > u32MaxH))
                    break;
            }
            if(i >= pFuncTestCtx->u32StreamCount)
            {
                param.width = u32MaxW + 1;
                param.height = u32MaxH + 1;
            }

            u32ReconfigDoneFlag = 0;
            u32OutputFlushDoneFlag = 0;
            u32InputFlushDoneFlag = 0;
            u32 = VPP_RECONFIG(p, param, param);
            wait_reconfig_done(p, u32MulFactor, u32FrameType);

            LOGD("Reconfigured new input stream: %dx%d", param.width, param.height);
            u32 = VPP_FLUSH(p, VPP_PORT_INPUT);
            DVP_ASSERT_EQUAL(u32, VPP_OK);
            u32OutFlushFlag = OUT_FLUSH_REQUEST;
            u32 = VPP_FLUSH(p, VPP_PORT_OUTPUT);
            DVP_ASSERT_EQUAL(u32, VPP_OK);

            pthread_mutex_lock(&pstTestCtx->mutex);
            while((u32OutputFlushDoneFlag == 0) || (u32InputFlushDoneFlag == 0))
                pthread_cond_wait(&pstTestCtx->cond, &pstTestCtx->mutex);
            pthread_mutex_unlock(&pstTestCtx->mutex);

            if(i < pFuncTestCtx->u32StreamCount)
            {
                pstTestCtx->buf_pool = pFuncTestCtx->reconfig_streams[i].reconfig_pool;
                pstTestCtx->params = pFuncTestCtx->reconfig_streams[i].reconfig_pool_params;
                pstTestCtx->buf_pool->u32WrIdx = 0;
                snprintf(pFuncTestCtx->reconfig_streams[i].reconfig_pool->params.cOutputName,
                         MAX_FILE_SEG_LEN, "ReconfigBypass_%s",
                         pFuncTestCtx->reconfig_streams[i].reconfig_pool_params.cOutputName);
            }

            u32BypassFlag |= BYPASS_FORCE;
            //rotate_buffers(p, 4, ROTATE_OUT_IN, u32MulFactor, u32FrameType);
            queue_buffer(p, VPP_PORT_OUTPUT, 1, u32FrameType);
            queue_buffer(p, VPP_PORT_OUTPUT, 1, u32FrameType);
            queue_buffer(p, VPP_PORT_OUTPUT, 1, u32FrameType);
            queue_buffer(p, VPP_PORT_OUTPUT, 1, u32FrameType);
            queue_buffer(p, VPP_PORT_OUTPUT, 1, u32FrameType);
            queue_buffer(p, VPP_PORT_INPUT, 1, u32FrameType);
            queue_buffer(p, VPP_PORT_INPUT, 1, u32FrameType);
            queue_buffer(p, VPP_PORT_INPUT, 1, u32FrameType);
            queue_buffer(p, VPP_PORT_INPUT, 1, u32FrameType);
            queue_buffer(p, VPP_PORT_INPUT, 1, u32FrameType);

            u32ReconfigDoneFlag = 0;
            u32OutputFlushDoneFlag = 0;
            u32InputFlushDoneFlag = 0;

            param = pstTestCtx->port_param;
            u32 = VPP_RECONFIG(p, param, param);

            wait_reconfig_done(p, 1, u32FrameType);

            LOGD("Reconfigured new input stream: %dx%d", param.width, param.height);
            u32 = VPP_FLUSH(p, VPP_PORT_INPUT);
            DVP_ASSERT_EQUAL(u32, VPP_OK);
            u32OutFlushFlag = OUT_FLUSH_REQUEST;
            u32 = VPP_FLUSH(p, VPP_PORT_OUTPUT);
            DVP_ASSERT_EQUAL(u32, VPP_OK);

            pthread_mutex_lock(&pstTestCtx->mutex);
            while((u32OutputFlushDoneFlag == 0) || (u32InputFlushDoneFlag == 0))
                pthread_cond_wait(&pstTestCtx->cond, &pstTestCtx->mutex);
            pthread_mutex_unlock(&pstTestCtx->mutex);

            pstTestCtx->buf_pool = buf_pool_temp;
            pstTestCtx->params = buf_pool_params_temp;

            u32BypassFlag &= ~BYPASS_FORCE;
            if(pstTestCtx->u32FramesOut > pstTestCtx->u32OutRxCnt)
                rotate_buffers(p, pstTestCtx->u32FramesOut - pstTestCtx->u32OutRxCnt, ROTATE_OUT_IN,
                               u32MulFactor, u32FrameType);

            pstTestCtx->u32FramesOut = u32TempFrames;

        }
        if(u32ResetHqvCtrlFlag)
            pstTestCtx->u32hqvCtrlSetFlag = 0;

        u32OutFlushFlag = OUT_FLUSH_REQUEST;
        VPP_TERM(p);

        for(i = 0; i < pFuncTestCtx->u32StreamCount; i++)
        {
            free_buf_pool(pFuncTestCtx->reconfig_streams[i].reconfig_pool, VPP_FALSE);
        }
    }
    else
    {
        LOGI("TEST SKIPPED.");
    }
}

// This test starts with finding a reconfigure stream above max supported to test bypass at init.
// Reconfigures to default stream after which should stay in bypass regardless of resolution
TEST(BufferExchangeReconfigBypass2_Testcase)
{
    void *p;
    struct vpp_callbacks stCb;
    struct vpp_requirements req;
    struct vpp_port_param param;
    uint32_t u32, u32Frames, i, u32FrameType;
    uint32_t u32ResetHqvCtrlFlag = 0;
    struct buf_pool *buf_pool_temp;
    struct buf_pool_params buf_pool_params_temp;
    struct functional_test_ctx *pFuncTestCtx;
    uint32_t u32TestMask, u32MaxRes, u32MaxH, u32MaxW;
#ifndef VPP_SERVICE
    t_StVppUsecase *pstUc;
#endif

    pFuncTestCtx = (struct functional_test_ctx *)pstTestCtx->pPrivateCtx;
    u32TestMask = pFuncTestCtx->u32TestMask;

    if((u32TestMask & FUNCTEST_BYPASS) || (u32TestMask & FUNCTEST_RECONFIG_ALL))
    {
        for(i = 0; i < pFuncTestCtx->u32StreamCount; i++)
        {
            u32 = init_buf_pool(&pstTestCtx->stVppCtx, &pFuncTestCtx->reconfig_streams[i].reconfig_pool,
                               &pFuncTestCtx->reconfig_streams[i].reconfig_pool_params,
                               BUFFER_POOL_SZ, VPP_FALSE);
            DVP_ASSERT_EQUAL(u32, VPP_OK);
        }

        // Callbacks
        stCb.input_buffer_done = buffer_exchange_ibd_cb,
        stCb.output_buffer_done = buffer_exchange_obd_cb,
        stCb.vpp_event = buffer_exchange_evt_cb,

        // Port parameters
        param = pstTestCtx->port_param;

        p = VPP_INIT(get_session_flags(pstTestCtx), stCb);
        DVP_ASSERT_PTR_NNULL(p);

        if(pstTestCtx->u32hqvCtrlSetFlag == 0)
        {
            set_default_hqv_controls();
            u32ResetHqvCtrlFlag = 1;
        }

        u32 = setTestVppHqvCtrl(p, pstTestCtx, &req);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
        bUsecaseBypassesInput = bUsecase_Bypasses(pstTestCtx);

        if(pstTestCtx->buf_pool->params.eBufferType == eVppBufType_Interleaved_BFF)
            u32FrameType = eVppBufType_Interleaved_BFF;
        else if(pstTestCtx->buf_pool->params.eBufferType == eVppBufType_Interleaved_TFF)
            u32FrameType = eVppBufType_Interleaved_TFF;
        else
            u32FrameType = eVppBufType_Progressive;

        buf_pool_temp = pstTestCtx->buf_pool;
        buf_pool_params_temp = pstTestCtx->params;

#ifndef VPP_SERVICE
        pstUc = ((t_StVppCtx *)p)->pstVppCb->pstUc;
        u32MaxRes = u32VppUsecase_GetMaxResForUc(pstUc);
#else
        u32MaxRes = VPP_RESOLUTION_MAX - 1;
#endif
        u32MaxW = u32GetMaxWidthForRes((t_EVppRes)u32MaxRes);
        u32MaxH = u32GetMaxHeightForRes ((t_EVppRes)u32MaxRes);
        for(i=0; i < pFuncTestCtx->u32StreamCount; i++)
        {
            param = pFuncTestCtx->reconfig_streams[i].port_param;
            if((param.width > u32MaxW) || (param.height > u32MaxH))
                break;
        }
        if(i < pFuncTestCtx->u32StreamCount)
        {
            pstTestCtx->buf_pool = pFuncTestCtx->reconfig_streams[i].reconfig_pool;
            pstTestCtx->params = pFuncTestCtx->reconfig_streams[i].reconfig_pool_params;
            pstTestCtx->buf_pool->u32WrIdx = 0;
            snprintf(pFuncTestCtx->reconfig_streams[i].reconfig_pool->params.cOutputName,
                     MAX_FILE_SEG_LEN, "ReconfigBypass2_%s",
                     pFuncTestCtx->reconfig_streams[i].reconfig_pool_params.cOutputName);
        }
        else
        {
            param.width = u32MaxW + 1;
            param.height = u32MaxH + 1;
        }

        u32 = VPP_SET_PARAM(p, VPP_PORT_INPUT, param);

        DVP_ASSERT_EQUAL(u32, VPP_OK);

        u32 = VPP_SET_PARAM(p, VPP_PORT_OUTPUT, param);
        DVP_ASSERT_NEQUAL(u32, VPP_OK);


        if(pstTestCtx->u32FramesOut)
            u32Frames = pstTestCtx->u32FramesOut;
        else
        {
            u32Frames = 20;
            pstTestCtx->u32FramesOut = u32Frames;
        }
        u32BypassFlag |= BYPASS_FORCE;
        queue_buffer(p, VPP_PORT_OUTPUT, 1, u32FrameType);
        queue_buffer(p, VPP_PORT_OUTPUT, 1, u32FrameType);
        queue_buffer(p, VPP_PORT_OUTPUT, 1, u32FrameType);
        queue_buffer(p, VPP_PORT_OUTPUT, 1, u32FrameType);
        queue_buffer(p, VPP_PORT_OUTPUT, 1, u32FrameType);
        queue_buffer(p, VPP_PORT_INPUT, 1, u32FrameType);
        queue_buffer(p, VPP_PORT_INPUT, 1, u32FrameType);
        queue_buffer(p, VPP_PORT_INPUT, 1, u32FrameType);
        queue_buffer(p, VPP_PORT_INPUT, 1, u32FrameType);
        queue_buffer(p, VPP_PORT_INPUT, 1, u32FrameType);

        u32ReconfigDoneFlag = 0;
        u32OutputFlushDoneFlag = 0;
        u32InputFlushDoneFlag = 0;

        param = pstTestCtx->port_param;
        u32 = VPP_RECONFIG(p, param, param);

        wait_reconfig_done(p, 1, u32FrameType);

        LOGD("Reconfigured new input stream: %dx%d", param.width, param.height);
        u32 = VPP_FLUSH(p, VPP_PORT_INPUT);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
        u32OutFlushFlag = OUT_FLUSH_REQUEST;
        u32 = VPP_FLUSH(p, VPP_PORT_OUTPUT);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        pthread_mutex_lock(&pstTestCtx->mutex);
        while((u32OutputFlushDoneFlag == 0) || (u32InputFlushDoneFlag == 0))
            pthread_cond_wait(&pstTestCtx->cond, &pstTestCtx->mutex);
        pthread_mutex_unlock(&pstTestCtx->mutex);

        pstTestCtx->buf_pool = buf_pool_temp;
        pstTestCtx->params = buf_pool_params_temp;
        snprintf(pstTestCtx->buf_pool->params.cOutputName, MAX_FILE_SEG_LEN,
                 "ReconfigBypass2_%s", pstTestCtx->params.cOutputName);

        rotate_buffers(p, 5, ROTATE_OUT_IN, 1, u32FrameType);

        pthread_mutex_lock(&pstTestCtx->mutex);
        while(pstTestCtx->u32InHoldCnt != 0)
            pthread_cond_wait(&pstTestCtx->cond, &pstTestCtx->mutex);
        pthread_mutex_unlock(&pstTestCtx->mutex);


        if(u32ResetHqvCtrlFlag)
            pstTestCtx->u32hqvCtrlSetFlag = 0;

        u32OutFlushFlag = OUT_FLUSH_REQUEST;
        VPP_TERM(p);

        for(i = 0; i < pFuncTestCtx->u32StreamCount; i++)
        {
            free_buf_pool(pFuncTestCtx->reconfig_streams[i].reconfig_pool, VPP_FALSE);
        }
    }
    else
    {
        LOGI("TEST SKIPPED.");
    }
}

/************************************************************************
 * Global Functions
 ***********************************************************************/
TEST_CASES BufferExchangeTests[] = {
    TEST_CASE(BufferExchange1_Testcase),
    TEST_CASE(BufferExchange2_Testcase),
    TEST_CASE(BufferExchange3_Testcase),
    TEST_CASE(BufferExchange4_Testcase),
    TEST_CASE(BufferExchangeSetCtrl_Testcase),
    TEST_CASE(BufferExchangeFlag_Testcase), //Fails DI
    TEST_CASE(BufferExchangeFlush_Testcase),
    TEST_CASE(BufferExchangeReconfigure_Testcase),
    TEST_CASE(BufferExchangeReconfigError_Testcase), //Fails HVX/
#ifndef VPP_SERVICE
    TEST_CASE(BufferExchangeFrameRateBypass_Testcase),
#endif
    TEST_CASE(BufferExchangeNoUcBypass_Testcase),
    TEST_CASE(BufferExchangeInterlacedBypass_Testcase),
    TEST_CASE(BufferExchangeExtraStreamBypass_Testcase), // Extra stream may crash HVX on open
    TEST_CASE(BufferExchangeReconfigBypass_Testcase),
    TEST_CASE(BufferExchangeReconfigBypass2_Testcase),
    TEST_CASE(BufferExchangeStress_Testcase),
    TEST_CASE_NULL(),
};

TEST_SUITE(BufferExchangeSuite,
           "BufferExchange Tests",
           BufferExchangeSuiteInit,
           BufferExchangeSuiteTerm,
           BufferExchangeTestInit,
           BufferExchangeTestTerm,
           BufferExchangeTests);
