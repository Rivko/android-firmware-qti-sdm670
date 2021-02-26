/*!
 * @file test_ip.c
 *
 * @cr
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
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
#include "vpp_callback.h"
#include "vpp_reg.h"
#include "vpp_ip.h"

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
TEST_SUITE_INIT(IpSuiteInit)
{
}

TEST_SUITE_TERM(IpSuiteTerm)
{
}

TEST_SETUP(IpTestInit)
{
}

TEST_CLEANUP(IpTestTerm)
{
}

TEST(IpQueueFuzz)
{
    uint32_t u32;
    t_StVppIpCmdQueue stQ;
    t_StVppIpCmd stCmd;

    u32 = u32VppIp_CmdQueueInit(NULL, 10);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    u32 = u32VppIp_CmdQueueInit(NULL, 129348102);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    u32 = u32VppIp_CmdQueueInit(&stQ, 0xffffffff);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    u32 = u32VppIp_CmdQueueTerm(NULL);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    u32 = u32VppIp_CmdGet(NULL, NULL, VPP_TRUE);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    u32 = u32VppIp_CmdGet(NULL, NULL, VPP_FALSE);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    u32 = u32VppIp_CmdGet(&stQ, NULL, VPP_FALSE);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    u32 = u32VppIp_CmdGet(NULL, &stCmd, VPP_FALSE);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    u32 = u32VppIp_CmdGet(&stQ, NULL, VPP_TRUE);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    u32 = u32VppIp_CmdGet(NULL, &stCmd, VPP_TRUE);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    u32 = u32VppIp_CmdPut(NULL, stCmd);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    u32 = u32VppIp_CmdCnt(NULL);
    DVP_ASSERT_EQUAL(u32, 0);
}

t_EVppIpCmd aeCmd[] = {
    VPP_IP_CMD_THREAD_EXIT,
    VPP_IP_CMD_INIT,
    VPP_IP_CMD_TERM,
    VPP_IP_CMD_OPEN,
    VPP_IP_CMD_CLOSE,
    VPP_IP_CMD_FLUSH,
    VPP_IP_CMD_DRAIN,
    VPP_IP_CMD_STOP,
    VPP_IP_CMD_DRAIN,
    VPP_IP_CMD_STOP,
    VPP_IP_CMD_UPDATE_GLOBAL_PARAM,
    VPP_IP_CMD_UPDATE_GLOBAL_PARAM,
    VPP_IP_CMD_OPEN,
    VPP_IP_CMD_CLOSE,
    VPP_IP_CMD_FLUSH,
};

TEST(IpQueueBasic)
{
    uint32_t u32, i;
    t_StVppIpCmdQueue stQ;
    t_StVppIpCmd stCmd;
    uint32_t u32Sz = sizeof(aeCmd) / sizeof(t_EVppIpCmd);

    u32 = u32VppIp_CmdQueueInit(&stQ, u32Sz);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = u32VppIp_CmdCnt(&stQ);
    DVP_ASSERT_EQUAL(u32, 0);

    // normal, insert up to the maximum
    for (i = 0; i < u32Sz; i++)
    {
        stCmd.eCmd = aeCmd[i];
        u32 = u32VppIp_CmdPut(&stQ, stCmd);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        u32 = u32VppIp_CmdCnt(&stQ);
        DVP_ASSERT_EQUAL(u32, i + 1);
    }

    // queue is full, everything should be rejected
    for (i = 0; i < u32Sz; i++)
    {
        stCmd.eCmd = aeCmd[i];
        u32 = u32VppIp_CmdPut(&stQ, stCmd);
        DVP_ASSERT_NEQUAL(u32, VPP_OK);

        u32 = u32VppIp_CmdCnt(&stQ);
        DVP_ASSERT_EQUAL(u32, u32Sz);
    }

    // pull from the queue, everything that we get should be in the same order
    // that we queued
    for (i = 0; i < u32Sz; i++)
    {
        u32 = u32VppIp_CmdGet(&stQ, &stCmd, VPP_FALSE);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        u32 = u32VppIp_CmdCnt(&stQ);
        DVP_ASSERT_EQUAL(u32, u32Sz - i - 1);

        DVP_ASSERT_EQUAL(stCmd.eCmd, aeCmd[i]);
    }

    u32 = u32VppIp_CmdQueueTerm(&stQ);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
}

typedef struct {
    uint32_t bBlocking;
    uint32_t u32QueueSize;
    uint32_t u32ExpectedCount;
    t_StVppIpCmdQueue *pstCmdQ;
    pthread_t thread;

} t_StTestIpThreadWorker;

static void *vpIpQueueThreadedAccessWorker(void *pv)
{
    t_StTestIpThreadWorker *pstThreadWorker = (t_StTestIpThreadWorker *)pv;

    if (!pstThreadWorker)
    {
        DVP_ASSERT_FAIL();
        return NULL;
    }

    uint32_t u32;
    uint32_t u32Idx;
    uint32_t u32DequeuedCnt = 0;
    t_StVppIpCmd stCmd;
    t_EVppIpCmd eExpectedCmd;

    while (u32DequeuedCnt < pstThreadWorker->u32ExpectedCount)
    {
        if (pstThreadWorker->bBlocking)
        {
            u32 = u32VppIp_CmdGet(pstThreadWorker->pstCmdQ, &stCmd, VPP_TRUE);
            LOGI("pulling command (blocking), u32=%u", u32);
            if (u32 != VPP_OK)
                DVP_ASSERT_FAIL();
        }
        else
        {
            while (VPP_OK != u32VppIp_CmdGet(pstThreadWorker->pstCmdQ, &stCmd, VPP_FALSE));
        }

        u32Idx = u32DequeuedCnt % (sizeof(aeCmd) / sizeof(t_EVppIpCmd));
        eExpectedCmd = aeCmd[u32Idx];
        DVP_ASSERT_EQUAL(eExpectedCmd, stCmd.eCmd);
        u32DequeuedCnt++;
        LOGI("u32DequeuedCnt=%u", u32DequeuedCnt);

    }
    return NULL;
}

static void vIpQueueThreadedAccess(uint32_t bBlocking)
{
    // Spawn a worker thread and dequeue from the queue in that thread,
    // without explicitly locking a mutex. Expect that the items that are
    // dequeued from the queue are in the same order that they are put in.
    int rc;
    uint32_t i;
    uint32_t u32;
    uint32_t u32Idx;
    t_StVppIpCmd stCmd;
    t_StVppIpCmdQueue stCmdQ;
    t_StTestIpThreadWorker stThreadWorker;

    uint32_t u32Sz = sizeof(aeCmd) / sizeof(t_EVppIpCmd);
    uint32_t u32InsertCnt = 100000;

    memset(&stThreadWorker, 0, sizeof(stThreadWorker));
    stThreadWorker.bBlocking = bBlocking;
    stThreadWorker.pstCmdQ = &stCmdQ;
    stThreadWorker.u32QueueSize = u32Sz;
    stThreadWorker.u32ExpectedCount = u32InsertCnt;

    u32 = u32VppIp_CmdQueueInit(&stCmdQ, u32Sz);
    if (u32 != VPP_OK)
    {
        DVP_ASSERT_FAIL();
        return;
    }

    rc = pthread_create(&stThreadWorker.thread, NULL,
                        vpIpQueueThreadedAccessWorker, &stThreadWorker);
    if (rc)
    {
        DVP_ASSERT_FAIL();
        goto ERR_CREATE_THREAD;
    }

    for (i = 0; i < u32InsertCnt; i++)
    {
        u32Idx = i % u32Sz;
        stCmd.eCmd = aeCmd[u32Idx];

        LOGI("inserting command at u32Idx=%u, cnt=%u", u32Idx, i);

        while (u32VppIp_CmdCnt(&stCmdQ) == u32Sz);
        u32 = u32VppIp_CmdPut(&stCmdQ, stCmd);
        if (u32 != VPP_OK)
        {
            DVP_ASSERT_FAIL();
            break;
        }
    }

    if (i != u32InsertCnt)
    {
        // pthread_cancel doesn't exist in bionic libc... what do? How can we
        // kill the thread?
        LOGE("ERROR: i != u32InsertCnt, i=%u, u32InsertCnt=%u", i, u32InsertCnt);
    }
    else
    {
        pthread_join(stThreadWorker.thread, NULL);
    }

ERR_CREATE_THREAD:
    u32 = u32VppIp_CmdQueueTerm(&stCmdQ);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
}

TEST(IpQueueThreadedAccessBlocking)
{
    vIpQueueThreadedAccess(VPP_TRUE);
}

TEST(IpQueueThreadedAccessNonBlocking)
{
    vIpQueueThreadedAccess(VPP_FALSE);
}

#define PORT_BASIC_CNT 20
TEST(IpPortBasic)
{
    t_StVppIpPort stPort;

    t_StVppBuf *pstBuf, *pstBufPeek;
    t_StVppBuf astBuf[PORT_BASIC_CNT];

    uint32_t u32, i;

    // Basic parameter checking
    u32 = u32VppIp_PortInit(NULL);
    DVP_ASSERT_TRUE(u32 != VPP_OK);

    u32 = u32VppIp_PortTerm(NULL);
    DVP_ASSERT_TRUE(u32 != VPP_OK);

    u32 = u32VppIp_PortBufGet(NULL, NULL, NULL);
    DVP_ASSERT_TRUE(u32 != VPP_OK);

    u32 = u32VppIp_PortBufPut(NULL, NULL, NULL, NULL);
    DVP_ASSERT_TRUE(u32 != VPP_OK);

    // Init / Term with valid port
    u32 = u32VppIp_PortInit(&stPort);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = u32VppIp_PortTerm(&stPort);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // Get / put behaviour with valid port
    u32 = u32VppIp_PortInit(&stPort);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = u32VppIp_PortBufGet(&stPort, NULL, NULL);
    DVP_ASSERT_TRUE(u32 != VPP_OK);

    u32 = u32VppIp_PortBufGet(&stPort, &pstBuf, NULL);
    DVP_ASSERT_TRUE(u32 != VPP_OK);
    DVP_ASSERT_PTR_NULL(pstBuf);

    u32 = u32VppIp_PortBufPut(&stPort, NULL, NULL, NULL);
    DVP_ASSERT_TRUE(u32 != VPP_OK);

    for (i = 0; i < PORT_BASIC_CNT; i++)
    {
        u32 = u32VppIp_PortBufPut(&stPort, &astBuf[i], NULL, NULL);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        pstBufPeek =  pstVppIp_PortBufPeek(&stPort, 0, NULL);
        DVP_ASSERT_PTR_EQUAL(&astBuf[i], pstBufPeek);

        u32 = u32VppIp_PortBufGet(&stPort, &pstBuf, NULL);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        DVP_ASSERT_PTR_EQUAL(pstBuf, &astBuf[i]);
    }

    for (i = 0; i < PORT_BASIC_CNT; i++)
    {
        u32 = u32VppIp_PortBufPut(&stPort, &astBuf[i], NULL, NULL);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
    }

    for (i = 0; i < PORT_BASIC_CNT; i++)
    {
        pstBufPeek =  pstVppIp_PortBufPeek(&stPort, i, NULL);
        DVP_ASSERT_PTR_EQUAL(pstBufPeek, &astBuf[i]);
    }

    for (i = 0; i < PORT_BASIC_CNT; i++)
    {
        u32 = u32VppIp_PortBufGet(&stPort, &pstBuf, NULL);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        DVP_ASSERT_PTR_EQUAL(pstBuf, &astBuf[i]);
    }

    // should be empty at this point
    for (i = 0; i < PORT_BASIC_CNT; i++)
    {
        u32 = u32VppIp_PortBufGet(&stPort, &pstBuf, NULL);
        DVP_ASSERT_NEQUAL(u32, VPP_OK);
        DVP_ASSERT_PTR_NULL(pstBuf);

        pstBufPeek =  pstVppIp_PortBufPeek(&stPort, i, NULL);
        DVP_ASSERT_PTR_NULL(pstBufPeek);
    }

    u32 = u32VppIp_PortTerm(&stPort);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
}

typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    pthread_t thread;
    t_StVppIpPort stPort;
    t_StVppBuf astBuf[PORT_BASIC_CNT];
    uint32_t bRdy;
} t_StPortCondSignal;

static void *vpIpPortCondSignal(void *pv)
{
    t_StPortCondSignal *pstCb = (t_StPortCondSignal *)pv;

    uint32_t u32;
    t_StVppBuf *pstBuf;
    uint32_t u32Cnt = 0;


    while (u32Cnt < PORT_BASIC_CNT)
    {
        pthread_mutex_lock(&pstCb->mutex);
        pstCb->bRdy = VPP_TRUE;
        if (!u32VppBufPool_Cnt(&pstCb->stPort.stPendingQ))
            pthread_cond_wait(&pstCb->cond, &pstCb->mutex);
        pthread_mutex_unlock(&pstCb->mutex);
        u32 = u32VppIp_PortBufGet(&pstCb->stPort, &pstBuf, &pstCb->mutex);

        if (u32 != VPP_OK)
            continue;

        DVP_ASSERT_PTR_NNULL(pstBuf);
        DVP_ASSERT_PTR_EQUAL(pstBuf, &pstCb->astBuf[u32Cnt]);

        u32Cnt++;
    }

    return NULL;
}

TEST(IpPortCondSignal)
{
    uint32_t u32, i;
    int rc = 0;
    t_StPortCondSignal stCb;

    rc = pthread_mutex_init(&stCb.mutex, NULL);
    DVP_ASSERT_EQUAL(rc, 0);

    rc = pthread_cond_init(&stCb.cond, NULL);
    DVP_ASSERT_EQUAL(rc, 0);

    rc = pthread_create(&stCb.thread, NULL, vpIpPortCondSignal, &stCb);
    if (rc)
    {
        DVP_ASSERT_FAIL();
        goto ERR_THREAD_CREATE;
    }

    for (i = 0; i < PORT_BASIC_CNT; i++)
    {
        u32 = u32VppIp_PortBufPut(&stCb.stPort, &stCb.astBuf[i],
                                  &stCb.mutex, &stCb.cond);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
    }

    pthread_join(stCb.thread, NULL);

ERR_THREAD_CREATE:
    pthread_cond_destroy(&stCb.cond);
    pthread_mutex_destroy(&stCb.mutex);
}
/************************************************************************
 * Global Functions
 ***********************************************************************/
TEST_CASES IpTests[] = {
    TEST_CASE(IpQueueFuzz),
    TEST_CASE(IpQueueBasic),
    TEST_CASE(IpQueueThreadedAccessBlocking),
    TEST_CASE(IpQueueThreadedAccessNonBlocking),
    TEST_CASE(IpPortBasic),
    TEST_CASE(IpPortCondSignal),
    TEST_CASE_NULL(),
};

TEST_SUITE(IpSuite,
           "Ip Tests",
           IpSuiteInit,
           IpSuiteTerm,
           IpTestInit,
           IpTestTerm,
           IpTests);
