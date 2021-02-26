/*!
 * @file test_queue.c
 *
 * @cr
 * Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.

 * @services    Tests the queue data structure
 */

#include <stdlib.h>
#include <string.h>

#include "dvpTest.h"
#include "dvpTest_tb.h"

#define VPP_LOG_TAG     VPP_LOG_UT_QUEUE_TAG
#define VPP_LOG_MODULE  VPP_LOG_UT_QUEUE
#include "vpp_core.h"
#include "vpp_dbg.h"
#include "vpp_queue.h"

/************************************************************************
 * Local definitions
 ***********************************************************************/

/************************************************************************
 * Local static variables
 ***********************************************************************/
static t_StVppQueue q;
static t_StVppQueue *pq = &q;
/************************************************************************
 * Forward Declarations
 ************************************************************************/

/************************************************************************
 * Local Functions
 ***********************************************************************/

/************************************************************************
 * Test Functions
 ***********************************************************************/
TEST_SUITE_INIT(QueueSuiteInit)
{
}

TEST_SUITE_TERM(QueueSuiteTerm)
{
}

TEST_SETUP(QueueTestInit)
{
    memset(pq, 0, sizeof(t_StVppQueue));
}

TEST_CLEANUP(QueueTestTerm)
{
}

TEST(Queue_TestFull)
{
    int32_t idx;
    uint32_t i;
    uint32_t u32Sz = 5;
    vpp_queue_init(pq, u32Sz);

    for (i = 0; i < u32Sz; i++)
    {
        idx = vpp_queue_enqueue(pq);
        DVP_ASSERT_TRUE(idx >= 0);
        LOGI("iteration: %d, idx_ret=%d\n", i, idx);
    }
    idx = vpp_queue_enqueue(pq);
    DVP_ASSERT_EQUAL(-1, idx);
}

TEST(Queue_TestRemove)
{
    int32_t in, out;
    uint32_t i;
    uint32_t u32Sz = 5;
    uint32_t u32Insertions = 10;

    vpp_queue_init(pq, u32Sz);

    for (i = 0; i < u32Insertions; i++)
    {
        in = vpp_queue_enqueue(pq);
        DVP_ASSERT_TRUE(in >= 0);

        out = vpp_queue_dequeue(pq);
        DVP_ASSERT_TRUE(out >= 0);

        DVP_ASSERT_EQUAL(in, out);
        LOGI("in=%d, out=%d", in, out);
    }
}

TEST(Queue_TestRemoveEmpty)
{
    int32_t idx, prev;
    uint32_t i;
    uint32_t u32Enqueue = 5;
    uint32_t u32Dequeue = 10;

    vpp_queue_init(pq, u32Enqueue);

    for (i = 0; i < u32Enqueue; i++)
    {
        vpp_queue_enqueue(pq);
    }

    prev = -1;
    for (i = 0; i < u32Enqueue; i++)
    {
        idx = vpp_queue_dequeue(pq);
        LOGI("idx=%d, prev=%d", idx, prev);
        DVP_ASSERT_TRUE(idx >= 0);
        DVP_ASSERT_TRUE(prev != idx);
        prev = idx;
    }

    for (i = 0; i < u32Dequeue - u32Enqueue; i++)
    {
        LOGI("starting fail case");
        idx = vpp_queue_dequeue(pq);
        LOGI("idx=%d", idx);
        DVP_ASSERT_TRUE(idx < 0);
    }
}

TEST(Queue_TestPeek)
{
    int32_t idx, out_idx;
    uint32_t i;
    uint32_t u32Sz = 5;

    int32_t *ps32Idx;

    ps32Idx = calloc(sizeof(int32_t), u32Sz);
    if (!ps32Idx)
    {
        DVP_ASSERT_FAIL();
        return;
    }

    vpp_queue_init(pq, u32Sz);

    for (i = 0; i < u32Sz; i++)
    {
        idx = vpp_queue_enqueue(pq);
        DVP_ASSERT_TRUE(idx >= 0);
        LOGI("iteration: %d, idx_ret=%d\n", i, idx);
        ps32Idx[i] = idx;
    }
    for (i = 0; i < u32Sz; i++)
    {
        idx = vpp_queue_peek_head(pq);
        DVP_ASSERT_TRUE(idx >= 0);
        DVP_ASSERT_EQUAL(idx, ps32Idx[i]);

        out_idx = vpp_queue_dequeue(pq);
        DVP_ASSERT_TRUE(out_idx >= 0);
        DVP_ASSERT_EQUAL(out_idx, idx);
    }
}

/************************************************************************
 * Global Functions
 ***********************************************************************/
TEST_CASES QueueTests[] = {
    TEST_CASE(Queue_TestFull),
    TEST_CASE(Queue_TestRemove),
    TEST_CASE(Queue_TestRemoveEmpty),
    TEST_CASE(Queue_TestPeek),
    TEST_CASE_NULL(),
};

TEST_SUITE(QueueSuite,
        "Queue Tests",
        QueueSuiteInit,
        QueueSuiteTerm,
        QueueTestInit,
        QueueTestTerm,
        QueueTests);
