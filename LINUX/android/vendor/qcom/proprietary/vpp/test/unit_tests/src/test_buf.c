/*!
 * @file test_buf.c
 *
 * @cr
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.

 * @services    Tests the internal buffer pool
 */

#include <sys/types.h>
#include <media/msm_vidc.h>
#include <media/msm_media_info.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "dvpTest.h"
#include "dvpTest_tb.h"

#define VPP_LOG_TAG     VPP_LOG_UT_BUF_TAG
#define VPP_LOG_MODULE  VPP_LOG_UT_BUF
#include "vpp_dbg.h"
#include "vpp_utils.h"
#include "vpp_buf.h"

#include "test_utils.h"
#include "buf_pool.h"

/************************************************************************
 * Local definitions
 ***********************************************************************/

/************************************************************************
 * Local static variables
 ***********************************************************************/

/************************************************************************
 * Forward Declarations
 ************************************************************************/

/************************************************************************
 * Local Functions
 ***********************************************************************/

/************************************************************************
 * Test Functions
 ***********************************************************************/
TEST_SUITE_INIT(BufSuiteInit)
{
}

TEST_SUITE_TERM(BufSuiteTerm)
{
}

TEST_SETUP(BufTestInit)
{
}

TEST_CLEANUP(BufTestTerm)
{
}

TEST(BufPool_TestGet)
{
    uint32_t u32Ret;
    t_StVppBuf *pstBuf;
    t_StVppBufPool stPool;

    u32Ret = u32VppBufPool_Init(&stPool);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    // Invalid pool
    pstBuf = pstVppBufPool_Get(NULL);
    DVP_ASSERT_PTR_NULL(pstBuf);

    // 0 count pool
    pstBuf = pstVppBufPool_Get(&stPool);
    DVP_ASSERT_PTR_NULL(pstBuf);

    u32Ret = u32VppBufPool_Term(&stPool);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
}

TEST(BufPool_TestPut)
{
    uint32_t u32Ret;
    t_StVppBuf stBuf;
    t_StVppBuf *pstBuf;
    t_StVppBufPool stPool;

    u32Ret = u32VppBufPool_Init(&stPool);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    // Invalid buf and pool
    u32Ret = u32VppBufPool_Put(NULL, NULL);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);

    // Invalid pool
    u32Ret = u32VppBufPool_Put(NULL, &stBuf);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);

    // Invalid buf
    u32Ret = u32VppBufPool_Put(&stPool, NULL);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);

    // Valid
    u32Ret = u32VppBufPool_Put(&stPool, &stBuf);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    // Can we get it back?
    pstBuf = pstVppBufPool_Get(&stPool);
    DVP_ASSERT_PTR_NNULL(pstBuf);
    DVP_ASSERT_PTR_EQUAL(pstBuf, &stBuf);

    // Make sure we don't get back more then we put in
    pstBuf = pstVppBufPool_Get(&stPool);
    DVP_ASSERT_PTR_NULL(pstBuf);

    u32Ret = u32VppBufPool_Term(&stPool);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
}

#define BUF_CNT_TEST_CNT 10
TEST(BufPool_TestPutFront)
{
    uint32_t u32Ret, i;
    t_StVppBufPool stPool;
    t_StVppBuf stBuf, *pstBuf;

    t_StVppBuf astBuf[BUF_CNT_TEST_CNT];

    u32Ret = u32VppBufPool_Init(&stPool);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    // Invalid buf and pool
    u32Ret = u32VppBufPool_PutFront(NULL, NULL);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);

    // Invalid pool
    u32Ret = u32VppBufPool_PutFront(NULL, &stBuf);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);

    // Invalid buf
    u32Ret = u32VppBufPool_PutFront(&stPool, NULL);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);

    // Valid
    u32Ret = u32VppBufPool_Put(&stPool, &stBuf);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    DVP_ASSERT_EQUAL(1, u32VppBufPool_Cnt(&stPool));

    // Can we get it back?
    pstBuf = pstVppBufPool_Get(&stPool);
    DVP_ASSERT_PTR_NNULL(pstBuf);
    DVP_ASSERT_PTR_EQUAL(pstBuf, &stBuf);

    DVP_ASSERT_EQUAL(0, u32VppBufPool_Cnt(&stPool));

    // Make sure we don't get back more then we put in
    pstBuf = pstVppBufPool_Get(&stPool);
    DVP_ASSERT_PTR_NULL(pstBuf);

    for (i = 0; i < BUF_CNT_TEST_CNT; i++)
    {
        u32Ret = u32VppBufPool_PutFront(&stPool, &astBuf[i]);
        DVP_ASSERT_EQUAL(i + 1, u32VppBufPool_Cnt(&stPool));
        pstBuf = pstVppBufPool_Peek(&stPool, 0);
        DVP_ASSERT_EQUAL(pstBuf, &astBuf[i]);
    }

    for (i = 0; i < BUF_CNT_TEST_CNT; i++)
    {
        pstBuf = pstVppBufPool_Peek(&stPool, i);
        DVP_ASSERT_PTR_EQUAL(pstBuf, &astBuf[BUF_CNT_TEST_CNT - i - 1]);
    }

    for (i = 0; i < BUF_CNT_TEST_CNT; i++)
    {
        pstBuf = pstVppBufPool_Get(&stPool);
        DVP_ASSERT_PTR_EQUAL(pstBuf, &astBuf[BUF_CNT_TEST_CNT - i - 1]);

        DVP_ASSERT_EQUAL(BUF_CNT_TEST_CNT - i - 1, u32VppBufPool_Cnt(&stPool));
    }

    u32Ret = u32VppBufPool_Term(&stPool);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
}

TEST(BufPool_TestPutGetCnt)
{
    uint32_t u32Ret, u32Cnt, i, j;
    t_StVppBuf astBuf[BUF_CNT_TEST_CNT];
    t_StVppBuf *pstBuf;

    t_StVppBufPool stPool;
    u32Ret = u32VppBufPool_Init(&stPool);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    // Nothing in pool
    u32Cnt = u32VppBufPool_Cnt(&stPool);
    DVP_ASSERT_EQUAL(u32Cnt, 0);

    // Make sure as we queue, we get right count
    for (i = 0; i < BUF_CNT_TEST_CNT; i++)
    {
        u32Ret = u32VppBufPool_Put(&stPool, &astBuf[i]);
        DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

        u32Cnt = u32VppBufPool_Cnt(&stPool);
        DVP_ASSERT_EQUAL(u32Cnt, i + 1);
    }

    // Make sure as we dequeue we get right count
    for (i = BUF_CNT_TEST_CNT; i > 0; i--)
    {
        pstBuf = pstVppBufPool_Get(&stPool);
        DVP_ASSERT_PTR_NNULL(pstBuf);

        // Make sure that we find the buffer from the original list
        for (j = 0; j < BUF_CNT_TEST_CNT; j++)
        {
            if (pstBuf == &astBuf[j])
                break;
        }
        DVP_ASSERT_TRUE(j < BUF_CNT_TEST_CNT);

        u32Cnt = u32VppBufPool_Cnt(&stPool);
        DVP_ASSERT_EQUAL(u32Cnt, i - 1);
    }

    // should be none left in the pool
    u32Cnt = u32VppBufPool_Cnt(&stPool);
    DVP_ASSERT_EQUAL(u32Cnt, 0);

    // Queue one, dequeue one. Make sure that they are equal.
    for (i = 0; i < BUF_CNT_TEST_CNT; i++)
    {
        u32Ret = u32VppBufPool_Put(&stPool, &astBuf[i]);
        DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

        u32Cnt = u32VppBufPool_Cnt(&stPool);
        DVP_ASSERT_EQUAL(u32Cnt, 1);

        pstBuf = pstVppBufPool_Get(&stPool);
        DVP_ASSERT_PTR_NNULL(pstBuf);
        DVP_ASSERT_PTR_EQUAL(pstBuf, &astBuf[i]);

        u32Cnt = u32VppBufPool_Cnt(&stPool);
        DVP_ASSERT_EQUAL(u32Cnt, 0);
    }

    u32Ret = u32VppBufPool_Term(&stPool);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
}

TEST(BufPool_TestPeek)
{
    uint32_t u32Ret,i=0,u32Cnt;
    t_StVppBuf stBuf;
    t_StVppBuf *pstBuf;
    t_StVppBufPool stPool;

    t_StVppBuf astBuf[BUF_CNT_TEST_CNT];

    u32Ret = u32VppBufPool_Init(&stPool);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    // Invalid pool
    pstBuf = pstVppBufPool_Peek(NULL,0);
    DVP_ASSERT_PTR_NULL(pstBuf);

    // 0 count pool
    pstBuf = pstVppBufPool_Peek(&stPool,0);
    DVP_ASSERT_PTR_NULL(pstBuf);

    // Valid
    u32Ret = u32VppBufPool_Put(&stPool, &stBuf);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    // Can we get it back?
    pstBuf = pstVppBufPool_Peek(&stPool,0);
    DVP_ASSERT_PTR_NNULL(pstBuf);
    DVP_ASSERT_PTR_EQUAL(pstBuf, &stBuf);

    //If index out of range
    pstBuf = pstVppBufPool_Peek(&stPool,1);
    DVP_ASSERT_PTR_NULL(pstBuf);

    // Can we get it back?
    pstBuf = pstVppBufPool_Get(&stPool);
    DVP_ASSERT_PTR_NNULL(pstBuf);
    DVP_ASSERT_PTR_EQUAL(pstBuf, &stBuf);

    // Make sure as we queue, we get right count
    for (i = 0; i < BUF_CNT_TEST_CNT; i++)
    {
        u32Ret = u32VppBufPool_Put(&stPool, &astBuf[i]);
        DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

        u32Cnt = u32VppBufPool_Cnt(&stPool);
        DVP_ASSERT_EQUAL(u32Cnt, i + 1);
    }

    // Make sure as we peek we get right count
    for (i = 0; i < BUF_CNT_TEST_CNT; i++)
    {
        pstBuf = pstVppBufPool_Peek(&stPool,i);
        DVP_ASSERT_PTR_NNULL(pstBuf);
        DVP_ASSERT_TRUE(pstBuf == &astBuf[i]);

        //u32Cnt remains the same
        u32Cnt = u32VppBufPool_Cnt(&stPool);
        DVP_ASSERT_EQUAL(u32Cnt, BUF_CNT_TEST_CNT);
    }

    //If index out of range
    pstBuf = pstVppBufPool_Peek(&stPool,BUF_CNT_TEST_CNT);
    DVP_ASSERT_PTR_NULL(pstBuf);
    pstBuf = pstVppBufPool_Peek(&stPool,100+BUF_CNT_TEST_CNT);
    DVP_ASSERT_PTR_NULL(pstBuf);

    u32Ret = u32VppBufPool_Term(&stPool);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
}

TEST(Buf_TestInitTerm)
{
    t_StVppCtx stCtx;
    uint32_t u32Ret;

    memset(&stCtx, 0, sizeof(t_StVppCtx));

    u32Ret = u32VppBuf_Init(NULL, 0, 0);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);

    u32Ret = u32VppBuf_Term(NULL);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);

    u32Ret = u32VppBuf_Init(&stCtx, 0, 0);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    u32Ret = u32VppBuf_Term(&stCtx);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
}

TEST(Buf_TestGetPut)
{
    t_StVppCtx stCtx;
    uint32_t u32Ret;
    uint32_t u32MaxCnt;

    struct vpp_buffer vpp_buf;
    struct vpp_buffer *pvpp_buf;
    t_StVppBuf *pstVppBuf;

    memset(&vpp_buf, 0, sizeof(struct vpp_buffer));
    memset(&stCtx, 0, sizeof(t_StVppCtx));

    u32Ret = u32VppBuf_Init(&stCtx, 0, 0);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    u32MaxCnt = stCtx.pstBufCb->stPool.u32Cnt;

    // Invalid Get cases
    u32Ret = u32VppBuf_InternalGet(NULL, NULL, VPP_PORT_INPUT, NULL);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppBuf_InternalGet(NULL, NULL, VPP_PORT_INPUT, &pstVppBuf);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppBuf_InternalGet(NULL, &vpp_buf, VPP_PORT_INPUT, NULL);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppBuf_InternalGet(NULL, &vpp_buf, VPP_PORT_INPUT, &pstVppBuf);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppBuf_InternalGet(&stCtx, NULL, VPP_PORT_INPUT, NULL);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppBuf_InternalGet(&stCtx, NULL, VPP_PORT_INPUT, &pstVppBuf);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppBuf_InternalGet(&stCtx, &vpp_buf, VPP_PORT_INPUT, NULL);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppBuf_InternalGet(&stCtx, &vpp_buf, VPP_PORT_MAX, &pstVppBuf);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppBuf_InternalGet(&stCtx, &vpp_buf, 92013849, &pstVppBuf);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);

    // Invalid Put cases
    u32Ret = u32VppBuf_InternalPut(NULL, NULL, NULL, VPP_FALSE);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppBuf_InternalPut(NULL, NULL, &pvpp_buf, VPP_FALSE);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppBuf_InternalPut(NULL, pstVppBuf, NULL, VPP_FALSE);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppBuf_InternalPut(NULL, pstVppBuf, &pvpp_buf, VPP_FALSE);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppBuf_InternalPut(&stCtx, NULL, NULL, VPP_FALSE);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppBuf_InternalPut(&stCtx, NULL, &pvpp_buf, VPP_FALSE);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppBuf_InternalPut(&stCtx, pstVppBuf, NULL, VPP_FALSE);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);

    u32Ret = u32VppBuf_InternalPut(NULL, NULL, NULL, VPP_TRUE);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppBuf_InternalPut(NULL, NULL, &pvpp_buf, VPP_TRUE);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppBuf_InternalPut(NULL, pstVppBuf, NULL, VPP_TRUE);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppBuf_InternalPut(NULL, pstVppBuf, &pvpp_buf, VPP_TRUE);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppBuf_InternalPut(&stCtx, NULL, NULL, VPP_TRUE);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppBuf_InternalPut(&stCtx, NULL, &pvpp_buf, VPP_TRUE);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);
    u32Ret = u32VppBuf_InternalPut(&stCtx, pstVppBuf, NULL, VPP_TRUE);
    DVP_ASSERT_NEQUAL(u32Ret, VPP_OK);

    // Valid
    u32Ret = u32VppBuf_InternalGet(&stCtx, &vpp_buf, VPP_PORT_INPUT, &pstVppBuf);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    if (pstVppBuf)
    {
        // Since we passed in garbage for vpp_buf, should expect that nothing
        // was mapped properly
        DVP_ASSERT_EQUAL(0, pstVppBuf->stExtra.u32MappedLen);
        DVP_ASSERT_EQUAL(0, pstVppBuf->stPixel.u32MappedLen);

        u32Ret = u32VppBuf_InternalPut(&stCtx, pstVppBuf, &pvpp_buf, VPP_FALSE);
        DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
        DVP_ASSERT_PTR_EQUAL(pvpp_buf, &vpp_buf);
    }
    else
    {
        DVP_ASSERT_FAIL();
    }


    u32Ret = u32VppBuf_Term(&stCtx);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
}

void Buf_TestGetPutIon_Internal(t_StVppUsecase *pstUc, uint32_t bIsBufMapped)
{
    uint32_t u32Ret;
    uint32_t u32MaxCnt;

    struct test_ctx tctx;

    tctx_common_init(&tctx);
    tctx_set_port_params(&tctx, 1920, 1080, VPP_COLOR_FORMAT_NV12_VENUS);
    buf_params_init_default(&tctx.params, &tctx.port_param);

    u32Ret = u32VppBuf_Init(&tctx.stVppCtx, 0, 0);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    u32Ret = u32VppBuf_SetCurrentUsecase(&tctx.stVppCtx, pstUc);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    // Get the maximum number of buffers that VPP can hold at any given time
    u32MaxCnt = 24;//tctx.stVppCtx.pstBufCb->stPool.u32Cnt;

    // Allocate that max number
    u32Ret = init_buf_pool(&tctx.stVppCtx, &tctx.buf_pool, &tctx.params, u32MaxCnt, VPP_TRUE);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
    LOGD("POOL %p", tctx.buf_pool);
    uint32_t i;

    // Buffer holders to store buffers we get from test_buf_pool and what
    // is returned from vpp_core_buf_pool
    struct vpp_buffer **ppVppBufferHolder;
    t_StVppBuf **ppstIntBufHolder;
    void **ppvCookieHolder;

    // Buffer from test pool
    struct bufnode *pBufNode = NULL;

    // Buffer pointers for what we get from InternalGet/Put
    t_StVppBuf *pstIntBuf;
    struct vpp_buffer *pVppBuf;

    ppVppBufferHolder = calloc(u32MaxCnt, sizeof(struct vpp_buffer *));
    ppstIntBufHolder = calloc(u32MaxCnt, sizeof(struct t_StVppBuffer *));
    ppvCookieHolder = calloc(u32MaxCnt, sizeof(void *));
    DVP_ASSERT_PTR_NNULL(ppVppBufferHolder);
    DVP_ASSERT_PTR_NNULL(ppstIntBufHolder);
    DVP_ASSERT_PTR_NNULL(ppvCookieHolder);

    if (!ppVppBufferHolder || !ppstIntBufHolder || !ppvCookieHolder)
    {
        DVP_ASSERT_FAIL();
        goto cleanup_test_get_put_ion;
    }

    for (i = 0; i < u32MaxCnt; i++)
    {
        pBufNode = get_buf(tctx.buf_pool);
        if (!pBufNode)
        {
            DVP_ASSERT_FAIL();
            break;
        }

        u32Ret = u32VppBuf_InternalGet(&tctx.stVppCtx, pBufNode->pExtBuf,
                                       VPP_PORT_INPUT, &pstIntBuf);
        DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
        if (!pstIntBuf)
        {
            DVP_ASSERT_FAIL();
        }
        else
        {
            DVP_ASSERT_PTR_EQUAL(pstIntBuf->pBuf, pBufNode->pExtBuf);
            if (bIsBufMapped)
            {
                DVP_ASSERT_EQUAL(pstIntBuf->stPixel.u32MappedLen, pBufNode->pIntBuf->stPixel.u32MappedLen);
                DVP_ASSERT_PTR_NNULL(pstIntBuf->stPixel.pvBase);
            }
            else
            {
                DVP_ASSERT_EQUAL(pstIntBuf->stPixel.u32MappedLen, 0);
                DVP_ASSERT_PTR_NULL(pstIntBuf->stPixel.pvBase);
            }

            // Use cookie to store pointer to the test_buf_node from which this
            // buffer is from
            pBufNode->pExtBuf->cookie = (void *)pBufNode;

            // Store the vpp_buf so we can validate that we got back the correct
            // one when we return the buffers
            ppVppBufferHolder[i] = pBufNode->pExtBuf;
            ppstIntBufHolder[i] = pstIntBuf;
            ppvCookieHolder[i] = (void *)pBufNode;
        }
    }

    // Put back the buffers and make sure that we get the correct ones
    for (i = 0; i < u32MaxCnt; i++)
    {
        pstIntBuf = ppstIntBufHolder[i];
        if (!pstIntBuf)
        {
            DVP_ASSERT_FAIL();
            continue;
        }
        u32Ret = u32VppBuf_InternalPut(&tctx.stVppCtx, pstIntBuf, &pVppBuf, VPP_FALSE);

        DVP_ASSERT_EQUAL(u32Ret, VPP_OK);
        DVP_ASSERT_PTR_NNULL(pVppBuf);
        DVP_ASSERT_PTR_EQUAL(pVppBuf, ppVppBufferHolder[i]);

        pBufNode = (struct bufnode *)pVppBuf->cookie;
        DVP_ASSERT_PTR_EQUAL(pBufNode, ppvCookieHolder[i]);

        put_buf(tctx.buf_pool, pBufNode);
    }

cleanup_test_get_put_ion:

    if (ppvCookieHolder) free(ppvCookieHolder);
    if (ppstIntBufHolder) free(ppstIntBufHolder);
    if (ppVppBufferHolder) free(ppVppBufferHolder);

    u32Ret = u32VppBuf_Term(&tctx.stVppCtx);
    DVP_ASSERT_EQUAL(u32Ret, VPP_OK);

    free_buf_pool(tctx.buf_pool, VPP_TRUE);
    tctx_common_destroy(&tctx);
}

TEST(Buf_TestGetPutIon)
{
    t_StVppUsecase stUc;
    t_StVppIpBlock stIpGpu;
    t_StVppUsecaseIp stComposition;

    memset(&stUc, 0, sizeof(stUc));
    memset(&stIpGpu, 0, sizeof(stIpGpu));
    stUc.composition = &stComposition;

    // Set up IP + UC
    stIpGpu.name = "TEST_GPU";
    stIpGpu.eIp = VPP_IP_GPU;
    stUc.name = "TEST_USECASE";
    stUc.composition[0].ip = &stIpGpu;
    stUc.u32IpCnt = 1;

    stUc.bRequireMappedPxBuf = VPP_FALSE;
    stIpGpu.capabilities.bRequireMappedPxBuf = VPP_FALSE;


    // | UC |IP | Map |
    // |--------------|
    // |  - | - |  N  |
    // |  N | N |  N  |
    // |  N | Y |  Y  |
    // |  Y | N |  Y  |
    // |  Y | Y |  Y  |


    Buf_TestGetPutIon_Internal(NULL, VPP_FALSE);

    stUc.bRequireMappedPxBuf = VPP_FALSE;
    stIpGpu.capabilities.bRequireMappedPxBuf = VPP_FALSE;
    Buf_TestGetPutIon_Internal(&stUc, VPP_FALSE);

    stUc.bRequireMappedPxBuf = VPP_FALSE;
    stIpGpu.capabilities.bRequireMappedPxBuf = VPP_TRUE;
    Buf_TestGetPutIon_Internal(&stUc, VPP_TRUE);

    stUc.bRequireMappedPxBuf = VPP_TRUE;
    stIpGpu.capabilities.bRequireMappedPxBuf = VPP_FALSE;
    Buf_TestGetPutIon_Internal(&stUc, VPP_TRUE);

    stUc.bRequireMappedPxBuf = VPP_TRUE;
    stIpGpu.capabilities.bRequireMappedPxBuf = VPP_TRUE;
    Buf_TestGetPutIon_Internal(&stUc, VPP_TRUE);
}

void check_get_frame_type_single(t_StVppBuf *pstIntBuf, t_EVppBufType eType,
                                 uint32_t u32Offset)
{
    uint32_t u32 = VPP_ERR;
    void *pv;
    t_StVppBuf stBufOrig;
    uint32_t u32AvailSz, u32BytesWritten = 0;
    t_EVppBufType eTypeRx;

    if (!pstIntBuf)
        return;

    // store original settings so we can restore them
    memcpy(&stBufOrig, pstIntBuf, sizeof(t_StVppBuf));

    pv = (char *)pstIntBuf->stExtra.pvBase + u32Offset;
    u32AvailSz = pstIntBuf->stExtra.u32AllocLen - u32Offset;

    LOGD("stExtra.pvBase=%p, pv=%p, offset=%u", pstIntBuf->stExtra.pvBase, pv, u32Offset);

    u32BytesWritten = write_extradata_buf(pv, eType, 0, u32AvailSz);
    LOGD("u32BytesWritten=%u", u32BytesWritten);

    pstIntBuf->stExtra.u32FilledLen = u32BytesWritten;

    // Adjust pv to the new address
    pstIntBuf->stExtra.u32ValidLen = pstIntBuf->stExtra.u32MappedLen - u32Offset;
    pstIntBuf->stExtra.u32MappedLen = pstIntBuf->stExtra.u32ValidLen;
    pstIntBuf->stExtra.pvBase = pv;

    if (u32BytesWritten)
    {
        u32 = u32VppBuf_GetFrameTypeExtradata(pstIntBuf, VPP_EXTERNAL_EXTRADATA_TYPE, &eTypeRx);
        DVP_ASSERT_EQUAL(eTypeRx, eType);
    }
    else
    {
        u32 = u32VppBuf_GetFrameTypeExtradata(pstIntBuf, VPP_EXTERNAL_EXTRADATA_TYPE, &eTypeRx);
        DVP_ASSERT_EQUAL(eTypeRx, eVppBufType_Progressive);
    }
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    memcpy(pstIntBuf, &stBufOrig, sizeof(t_StVppBuf));
}

void check_get_frame_type(t_StVppBuf *pstIntBuf, uint32_t u32Offset)
{
    LOGD("====================");
    memset(pstIntBuf->stExtra.pvBase, 0, pstIntBuf->stExtra.u32MappedLen);
    check_get_frame_type_single(pstIntBuf, eVppBufType_Progressive, u32Offset);
    memset(pstIntBuf->stExtra.pvBase, 0, pstIntBuf->stExtra.u32MappedLen);
    check_get_frame_type_single(pstIntBuf, eVppBufType_Interleaved_TFF, u32Offset);
    memset(pstIntBuf->stExtra.pvBase, 0, pstIntBuf->stExtra.u32MappedLen);
    check_get_frame_type_single(pstIntBuf, eVppBufType_Interleaved_BFF, u32Offset);
    memset(pstIntBuf->stExtra.pvBase, 0, pstIntBuf->stExtra.u32MappedLen);
    check_get_frame_type_single(pstIntBuf, eVppBufType_Frame_TFF, u32Offset);
    memset(pstIntBuf->stExtra.pvBase, 0, pstIntBuf->stExtra.u32MappedLen);
    check_get_frame_type_single(pstIntBuf, eVppBufType_Frame_BFF, u32Offset);
}

static void init_mem_buf(t_StVppMemBuf *pstMemBuf, int fd, void *pv,
                         uint32_t u32AllocLen)
{
    if (!pstMemBuf)
        return;

    pstMemBuf->fd = fd;
    pstMemBuf->u32AllocLen = u32AllocLen;
    pstMemBuf->u32FilledLen = 0;
    pstMemBuf->u32Offset = 0;
    pstMemBuf->u32ValidLen = u32AllocLen;
    pstMemBuf->pvBase = pv;
    pstMemBuf->priv.pvPa = pv;
    pstMemBuf->priv.u32PaOffset = 0;
    pstMemBuf->priv.u32PaMappedLen = u32AllocLen;
    pstMemBuf->u32MappedLen = u32AllocLen;
}

TEST(Buf_GetFrameType)
{
    t_StVppBuf stIntBuf, *pstIntBuf;
    struct vpp_buffer stVppBuf, *pstVppBuf;
    uint32_t u32AllocLen;
    void *pv;

    pstIntBuf = &stIntBuf;
    pstVppBuf = &stVppBuf;
    u32AllocLen = VENUS_EXTRADATA_SIZE(1920, 1080);

    memset(pstIntBuf, 0, sizeof(t_StVppBuf));
    memset(pstVppBuf, 0, sizeof(struct vpp_buffer));

    // Establish relationship between external and internal buffer
    pstIntBuf->pBuf = pstVppBuf;

    // Fill out fields
    pstVppBuf->extradata.fd = 1;
    pstVppBuf->extradata.offset = 0;
    pstVppBuf->extradata.alloc_len = u32AllocLen;
    pstVppBuf->extradata.filled_len = 0;

    pv = calloc(1, u32AllocLen);
    if (!pv)
    {
        DVP_ASSERT_FAIL();
        return;
    }

    init_mem_buf(&pstIntBuf->stExtra, 1, pv, u32AllocLen);

    check_get_frame_type(pstIntBuf, 0);
    check_get_frame_type(pstIntBuf, 64);
    check_get_frame_type(pstIntBuf, 128);
    check_get_frame_type(pstIntBuf, 512);
    check_get_frame_type(pstIntBuf, 4096);
    check_get_frame_type(pstIntBuf, 8192);

    free(pv);
}

TEST(Buf_CopyExtradata)
{
    uint32_t u32;
    void *pv1, *pv2;
    uint32_t u32AllocLen;
    t_StVppBuf stIntBufSrc, stIntBufDst;
    struct vpp_buffer stVppBufSrc, stVppBufDst;
    t_EVppBufType eGetType, eExpectedType;

    u32AllocLen = VENUS_EXTRADATA_SIZE(1920, 1080);

    memset(&stIntBufSrc, 0, sizeof(t_StVppBuf));
    memset(&stIntBufDst, 0, sizeof(t_StVppBuf));
    memset(&stVppBufSrc, 0, sizeof(struct vpp_buffer));
    memset(&stVppBufDst, 0, sizeof(struct vpp_buffer));

    stIntBufSrc.pBuf = &stVppBufSrc;
    stIntBufDst.pBuf = &stVppBufDst;

    // source buffer
    stVppBufSrc.extradata.fd = 1;
    stVppBufSrc.extradata.offset = 0;
    stVppBufSrc.extradata.alloc_len = u32AllocLen;
    stVppBufSrc.extradata.filled_len = 0;

    // destination buffer
    stVppBufDst.extradata.fd = 2;
    stVppBufDst.extradata.offset = 0;
    stVppBufDst.extradata.alloc_len = u32AllocLen;
    stVppBufDst.extradata.filled_len = 0;

    pv1 = calloc(1, u32AllocLen);
    pv2 = calloc(1, u32AllocLen);
    if (!pv1 || !pv2)
    {
        if (pv1) free(pv1);
        if (pv2) free(pv2);
        DVP_ASSERT_FAIL();
        return;
    }

    init_mem_buf(&stIntBufSrc.stExtra, 1, pv1, u32AllocLen);
    init_mem_buf(&stIntBufDst.stExtra, 2, pv2, u32AllocLen);

    fill_extra_data_int_buf(&stIntBufSrc, eVppBufType_Progressive, 0);

    eExpectedType = eVppBufType_Progressive;
    u32VppBuf_CopyExtradata(&stIntBufSrc, &stIntBufDst, eExpectedType, VPP_EXTERNAL_EXTRADATA_TYPE);
    u32 = u32VppBuf_GetFrameTypeExtradata(&stIntBufDst, VPP_EXTERNAL_EXTRADATA_TYPE, &eGetType);
    DVP_ASSERT_EQUAL(eGetType, eExpectedType);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    eExpectedType = eVppBufType_Interleaved_TFF;
    u32VppBuf_CopyExtradata(&stIntBufSrc, &stIntBufDst, eExpectedType, VPP_EXTERNAL_EXTRADATA_TYPE);
    u32 = u32VppBuf_GetFrameTypeExtradata(&stIntBufDst, VPP_EXTERNAL_EXTRADATA_TYPE, &eGetType);
    DVP_ASSERT_EQUAL(eGetType, eExpectedType);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    eExpectedType = eVppBufType_Interleaved_BFF;
    u32VppBuf_CopyExtradata(&stIntBufSrc, &stIntBufDst, eExpectedType, VPP_EXTERNAL_EXTRADATA_TYPE);
    u32 = u32VppBuf_GetFrameTypeExtradata(&stIntBufDst, VPP_EXTERNAL_EXTRADATA_TYPE, &eGetType);
    DVP_ASSERT_EQUAL(eGetType, eExpectedType);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    eExpectedType = eVppBufType_Frame_TFF;
    u32VppBuf_CopyExtradata(&stIntBufSrc, &stIntBufDst, eExpectedType, VPP_EXTERNAL_EXTRADATA_TYPE);
    u32 = u32VppBuf_GetFrameTypeExtradata(&stIntBufDst, VPP_EXTERNAL_EXTRADATA_TYPE, &eGetType);
    DVP_ASSERT_EQUAL(eGetType, eExpectedType);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    eExpectedType = eVppBufType_Frame_BFF;
    u32VppBuf_CopyExtradata(&stIntBufSrc, &stIntBufDst, eExpectedType, VPP_EXTERNAL_EXTRADATA_TYPE);
    u32 = u32VppBuf_GetFrameTypeExtradata(&stIntBufDst, VPP_EXTERNAL_EXTRADATA_TYPE, &eGetType);
    DVP_ASSERT_EQUAL(eGetType, eExpectedType);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    free(pv1);
    free(pv2);
}

#define EX_FLAG(_s, _pb)    VPP_FLAG_##_s((_pb)->pBuf->flags, VPP_BUFFER_FLAG_EXTRADATA)
#define EX_ASSERT_SET(_pb)  DVP_ASSERT_TRUE(VPP_FLAG_IS_SET((_pb)->pBuf->flags, VPP_BUFFER_FLAG_EXTRADATA))
#define EX_ASSERT_CLR(_pb)  DVP_ASSERT_FALSE(VPP_FLAG_IS_SET((_pb)->pBuf->flags, VPP_BUFFER_FLAG_EXTRADATA))

TEST(Buf_PropagateExtradata)
{
    uint32_t u32;
    void *pv1, *pv2;
    uint32_t u32AllocLen;
    t_StVppBuf stIntBufSrc, stIntBufDst;
    struct vpp_buffer stVppBufSrc, stVppBufDst;
    t_EVppBufType eGetType, eExpectedType;

    u32AllocLen = VENUS_EXTRADATA_SIZE(1920, 1080);

    memset(&stIntBufSrc, 0, sizeof(t_StVppBuf));
    memset(&stIntBufDst, 0, sizeof(t_StVppBuf));
    memset(&stVppBufSrc, 0, sizeof(struct vpp_buffer));
    memset(&stVppBufDst, 0, sizeof(struct vpp_buffer));

    stIntBufSrc.pBuf = &stVppBufSrc;
    stIntBufDst.pBuf = &stVppBufDst;

    // source buffer
    stVppBufSrc.extradata.fd = 1;
    stVppBufSrc.extradata.offset = 0;
    stVppBufSrc.extradata.alloc_len = u32AllocLen;
    stVppBufSrc.extradata.filled_len = 0;

    // destination buffer
    stVppBufDst.extradata.fd = 2;
    stVppBufDst.extradata.offset = 0;
    stVppBufDst.extradata.alloc_len = u32AllocLen;
    stVppBufDst.extradata.filled_len = 0;

    pv1 = calloc(1, u32AllocLen);
    pv2 = calloc(1, u32AllocLen);
    if (!pv1 || !pv2)
    {
        if (pv1) free(pv1);
        if (pv2) free(pv2);
        DVP_ASSERT_FAIL();
        return;
    }

    init_mem_buf(&stIntBufSrc.stExtra, 1, pv1, u32AllocLen);
    init_mem_buf(&stIntBufDst.stExtra, 2, pv2, u32AllocLen);

    fill_extra_data_int_buf(&stIntBufSrc, eVppBufType_Progressive, 0);

    // Basic
    DVP_ASSERT_EQUAL(VPP_ERR_PARAM,
                     u32VppBuf_PropagateExtradata(NULL, NULL,
                                                  eVppBufType_Progressive,
                                                  VPP_EXTERNAL_EXTRADATA_TYPE));
    DVP_ASSERT_EQUAL(VPP_ERR_PARAM,
                     u32VppBuf_PropagateExtradata(&stIntBufSrc, NULL,
                                                  eVppBufType_Progressive,
                                                  VPP_EXTERNAL_EXTRADATA_TYPE));
    DVP_ASSERT_EQUAL(VPP_ERR_PARAM,
                     u32VppBuf_PropagateExtradata(NULL, &stIntBufDst,
                                                  eVppBufType_Progressive,
                                                  VPP_EXTERNAL_EXTRADATA_TYPE));

    // Flag prop - input not set, so should not propagate
    EX_FLAG(CLR, &stIntBufSrc);
    EX_FLAG(CLR, &stIntBufDst);
    u32 = u32VppBuf_PropagateExtradata(&stIntBufSrc, &stIntBufDst,
                                       eVppBufType_Progressive,
                                       VPP_EXTERNAL_EXTRADATA_TYPE);
    EX_ASSERT_CLR(&stIntBufDst);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // Flag prop - input not set, output set, expect it to be cleared.
    EX_FLAG(CLR, &stIntBufSrc);
    EX_FLAG(SET, &stIntBufDst);
    u32 = u32VppBuf_PropagateExtradata(&stIntBufSrc, &stIntBufDst,
                                       eVppBufType_Progressive,
                                       VPP_EXTERNAL_EXTRADATA_TYPE);
    EX_ASSERT_CLR(&stIntBufDst);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // Flag prop - input set, output cleared, expect it to be set
    EX_FLAG(SET, &stIntBufSrc);
    EX_FLAG(CLR, &stIntBufDst);
    u32 = u32VppBuf_PropagateExtradata(&stIntBufSrc, &stIntBufDst,
                                       eVppBufType_Progressive,
                                       VPP_EXTERNAL_EXTRADATA_TYPE);
    EX_ASSERT_SET(&stIntBufDst);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // Check that propagation is correct based on frame type (same tests as
    // copy extradata).
    eExpectedType = eVppBufType_Progressive;
    u32VppBuf_PropagateExtradata(&stIntBufSrc, &stIntBufDst,
                                 eExpectedType, VPP_EXTERNAL_EXTRADATA_TYPE);
    u32 = u32VppBuf_GetFrameTypeExtradata(&stIntBufDst, VPP_EXTERNAL_EXTRADATA_TYPE, &eGetType);
    DVP_ASSERT_EQUAL(eGetType, eExpectedType);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    eExpectedType = eVppBufType_Interleaved_TFF;
    u32VppBuf_PropagateExtradata(&stIntBufSrc, &stIntBufDst,
                                 eExpectedType, VPP_EXTERNAL_EXTRADATA_TYPE);
    u32 = u32VppBuf_GetFrameTypeExtradata(&stIntBufDst, VPP_EXTERNAL_EXTRADATA_TYPE, &eGetType);
    DVP_ASSERT_EQUAL(eGetType, eExpectedType);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    eExpectedType = eVppBufType_Interleaved_BFF;
    u32VppBuf_PropagateExtradata(&stIntBufSrc, &stIntBufDst,
                                 eExpectedType, VPP_EXTERNAL_EXTRADATA_TYPE);
    u32 = u32VppBuf_GetFrameTypeExtradata(&stIntBufDst, VPP_EXTERNAL_EXTRADATA_TYPE, &eGetType);
    DVP_ASSERT_EQUAL(eGetType, eExpectedType);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    eExpectedType = eVppBufType_Frame_TFF;
    u32VppBuf_PropagateExtradata(&stIntBufSrc, &stIntBufDst,
                                 eExpectedType, VPP_EXTERNAL_EXTRADATA_TYPE);
    u32 = u32VppBuf_GetFrameTypeExtradata(&stIntBufDst, VPP_EXTERNAL_EXTRADATA_TYPE, &eGetType);
    DVP_ASSERT_EQUAL(eGetType, eExpectedType);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    eExpectedType = eVppBufType_Frame_BFF;
    u32VppBuf_PropagateExtradata(&stIntBufSrc, &stIntBufDst,
                                 eExpectedType, VPP_EXTERNAL_EXTRADATA_TYPE);
    u32 = u32VppBuf_GetFrameTypeExtradata(&stIntBufDst, VPP_EXTERNAL_EXTRADATA_TYPE, &eGetType);
    DVP_ASSERT_EQUAL(eGetType, eExpectedType);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    free(pv1);
    free(pv2);
}

TEST(Buf_GetSetTs)
{
    uint32_t i;
    uint64_t ts_set = 0, ts_get = 12309;
    struct vpp_buffer ext_buf;
    t_StVppBuf int_buf;

    memset(&ext_buf, 0, sizeof(ext_buf));
    memset(&int_buf, 0, sizeof(int_buf));

    int_buf.pBuf = &ext_buf;

    // Basic
    ts_get = u64VppBuf_GetTs(NULL);
    DVP_ASSERT_EQUAL(ts_get, 0);

    // Sanity - no crash
    vVppBuf_SetTs(NULL, 0);
    vVppBuf_SetTs(NULL, 12031239);
    vVppBuf_SetTs(NULL, (uint64_t)-1293128);

    // Check that if client sets timestamp, it is correct from internal buf
    for (i = 0; i < 2000; i++)
    {
        ts_set = rand();
        ext_buf.timestamp = ts_set;
        ts_get = u64VppBuf_GetTs(&int_buf);
        DVP_ASSERT_EQUAL(ts_get, ts_set);
    }

    // check that if using SetTs function, it is correct in GetTs and client
    for (i = 0; i < 2000; i++)
    {
        ts_set = rand();
        vVppBuf_SetTs(&int_buf, ts_set);
        DVP_ASSERT_EQUAL(ts_set, ext_buf.timestamp);

        ts_get = u64VppBuf_GetTs(&int_buf);
        DVP_ASSERT_EQUAL(ts_get, ts_set);
    }
}

static uint32_t u32RemoveMatchingPred(t_StVppBuf *pstBuf, void *pv)
{
    if (pstBuf == (t_StVppBuf*)pv)
        return VPP_TRUE;

    return VPP_FALSE;
}

TEST(Buf_RemoveMatching)
{
    uint32_t u32;
    t_StVppBuf *pstBuf;
    t_StVppBufPool stPool;
    t_StVppBuf astBuf[BUF_CNT_TEST_CNT];

    fpBufMatchPred fp = u32RemoveMatchingPred;

    u32 = u32VppBufPool_Init(&stPool);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // Sanity check parameters
    pstBuf = pstVppBufPool_RemoveMatching(NULL, NULL, NULL);
    DVP_ASSERT_PTR_NULL(pstBuf);

    // predicate is required
    pstBuf = pstVppBufPool_RemoveMatching(&stPool, NULL, NULL);
    DVP_ASSERT_PTR_NULL(pstBuf);

    // bufpool is required
    pstBuf = pstVppBufPool_RemoveMatching(NULL, fp, NULL);
    DVP_ASSERT_PTR_NULL(pstBuf);

    // empty pool
    pstBuf = pstVppBufPool_RemoveMatching(&stPool, fp, NULL);
    DVP_ASSERT_PTR_NULL(pstBuf);

    // single node
    {
        u32 = u32VppBufPool_Put(&stPool, &astBuf[0]);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        pstBuf = pstVppBufPool_RemoveMatching(&stPool, fp, &astBuf[0]);
        DVP_ASSERT_PTR_EQUAL(pstBuf, &astBuf[0]);

        u32 = u32VppBufPool_Cnt(&stPool);
        DVP_ASSERT_EQUAL(u32, 0);

        pstBuf = pstVppBufPool_Get(&stPool);
        DVP_ASSERT_PTR_NULL(pstBuf);
    }

    // two nodes
    {
        // insert 0, 1, remove 0, 1
        u32 = u32VppBufPool_Put(&stPool, &astBuf[0]);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
        u32 = u32VppBufPool_Put(&stPool, &astBuf[1]);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
        pstBuf = pstVppBufPool_RemoveMatching(&stPool, fp, &astBuf[0]);
        DVP_ASSERT_PTR_EQUAL(pstBuf, &astBuf[0]);
        pstBuf = pstVppBufPool_RemoveMatching(&stPool, fp, &astBuf[1]);
        DVP_ASSERT_PTR_EQUAL(pstBuf, &astBuf[1]);

        // insert 2, 3, remove 3, 2
        u32 = u32VppBufPool_Put(&stPool, &astBuf[2]);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
        u32 = u32VppBufPool_Put(&stPool, &astBuf[3]);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
        pstBuf = pstVppBufPool_RemoveMatching(&stPool, fp, &astBuf[3]);
        DVP_ASSERT_PTR_EQUAL(pstBuf, &astBuf[3]);
        pstBuf = pstVppBufPool_RemoveMatching(&stPool, fp, &astBuf[2]);
        DVP_ASSERT_PTR_EQUAL(pstBuf, &astBuf[2]);

        // sanity check
        u32 = u32VppBufPool_Cnt(&stPool);
        DVP_ASSERT_EQUAL(u32, 0);
        pstBuf = pstVppBufPool_Get(&stPool);
        DVP_ASSERT_PTR_NULL(pstBuf);
    }

    // middle nodes
    {
        uint32_t i;
        for (i = 0; i < BUF_CNT_TEST_CNT; i++)
        {
            u32 = u32VppBufPool_Put(&stPool, &astBuf[i]);
            DVP_ASSERT_EQUAL(u32, VPP_OK);
        }

        // remove 5 to max
        for (i = 5; i < BUF_CNT_TEST_CNT; i++)
        {
            pstBuf = pstVppBufPool_RemoveMatching(&stPool, fp, &astBuf[i]);
            DVP_ASSERT_PTR_EQUAL(pstBuf, &astBuf[i]);
        }

        // remove 4 down to 1
        for (i = 4; i != 0; i--)
        {
            pstBuf = pstVppBufPool_RemoveMatching(&stPool, fp, &astBuf[i]);
            DVP_ASSERT_PTR_EQUAL(pstBuf, &astBuf[i]);
        }
        u32 = u32VppBufPool_Cnt(&stPool);
        DVP_ASSERT_EQUAL(u32, 1);

        pstBuf = pstVppBufPool_RemoveMatching(&stPool, fp, &astBuf[0]);
        DVP_ASSERT_PTR_EQUAL(pstBuf, &astBuf[0]);

        u32 = u32VppBufPool_Cnt(&stPool);
        DVP_ASSERT_EQUAL(u32, 0);
    }

    u32 = u32VppBufPool_Term(&stPool);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
}

/************************************************************************
 * Global Functions
 ***********************************************************************/
TEST_CASES BufTests[] = {
    TEST_CASE(BufPool_TestGet),
    TEST_CASE(BufPool_TestPut),
    TEST_CASE(BufPool_TestPutFront),
    TEST_CASE(BufPool_TestPutGetCnt),
    TEST_CASE(BufPool_TestPeek),
    TEST_CASE(Buf_TestInitTerm),
    TEST_CASE(Buf_TestGetPut),
    TEST_CASE(Buf_TestGetPutIon),
    TEST_CASE(Buf_GetFrameType),
    TEST_CASE(Buf_CopyExtradata),
    TEST_CASE(Buf_PropagateExtradata),
    TEST_CASE(Buf_GetSetTs),
    TEST_CASE(Buf_RemoveMatching),
    TEST_CASE_NULL(),
};

TEST_SUITE(BufSuite,
           "BufTests",
           BufSuiteInit,
           BufSuiteTerm,
           BufTestInit,
           BufTestTerm,
           BufTests);
