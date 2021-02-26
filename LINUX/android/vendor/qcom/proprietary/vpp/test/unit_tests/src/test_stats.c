/*!
 * @file test_stats.c
 *
 * @cr
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.

 * @services
 */

#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "dvpTest.h"
#include "dvpTest_tb.h"

#define VPP_LOG_TAG     VPP_LOG_UT_STATS_TAG
#define VPP_LOG_MODULE  VPP_LOG_UT_STATS
#include "vpp_dbg.h"
#include "vpp.h"
#include "vpp_ctx.h"
#include "vpp_stats.h"

/************************************************************************
 * Local definitions
 ***********************************************************************/
enum {
    TEST_STATS_NUMBER_ONE,
    TEST_STATS_NUMBER_TWO,
    TEST_STATS_NUMBER_THREE,
    TEST_STATS_NUMBER_FOUR,
    TEST_STATS_NUMBER_FIVE,
    TEST_STATS_MAX,
};

enum {
    TEST_STATS_NUMBER_SIX = TEST_STATS_MAX,
    TEST_STATS_NUMBER_SEVEN,
};

enum {
    TEST_STATS_ALFA,
    TEST_STATS_BRAVO,
    TEST_STATS_CHARLIE,
    TEST_STATS_DELTA,
    TEST_STATS_ECHO,
    TEST_STATS_FOXTROT,
    TEST_STATS_GOLF,
    TEST_STATS_HOTEL,
    TEST_STATS_INDIA,
    TEST_STATS_JULIETT,
    TEST_STATS_KILO,
    TEST_STATS_LIMA,
    TEST_STATS_MIKE,
    TEST_STATS_NOVEMBER,
    TEST_STATS_OSCAR,
    TEST_STATS_PAPA,
    TEST_STATS_QUEBEC,
    TEST_STATS_ROMEO,
    TEST_STATS_SIRRA,
    TEST_STATS_TANGO,
    TEST_STATS_UNIFORM,
    TEST_STATS_VICTOR,
    TEST_STATS_WHISKEY,
    TEST_STATS_XRAY,
    TEST_STATS_YANKEE,
    TEST_STATS_ZULU,
};

/************************************************************************
 * Local static variables
 ***********************************************************************/
static const t_StVppStatsConfig astStatsCfg[] = {
    VPP_PROF_DECL(TEST_STATS_NUMBER_ONE, 1, 1),
    VPP_PROF_DECL(TEST_STATS_NUMBER_TWO, 2, 1),
    VPP_PROF_DECL(TEST_STATS_NUMBER_THREE, 3, 1),
    VPP_PROF_DECL(TEST_STATS_NUMBER_FOUR, 4, 1),
    VPP_PROF_DECL(TEST_STATS_NUMBER_FIVE, 5, 1),
};

static const t_StVppStatsConfig astStatsMultiCfg[] = {
    VPP_PROF_DECL(TEST_STATS_ALFA, 1, 1),
    VPP_PROF_DECL(TEST_STATS_BRAVO, 1, 1),
    VPP_PROF_DECL(TEST_STATS_CHARLIE, 1, 1),
    VPP_PROF_DECL(TEST_STATS_DELTA, 1, 1),
    VPP_PROF_DECL(TEST_STATS_NOVEMBER, 1, 1),
    VPP_PROF_DECL(TEST_STATS_ZULU, 1, 1),
    VPP_PROF_DECL(TEST_STATS_GOLF, 1, 1),
};

static const uint32_t u32StatsCnt = VPP_STATS_CNT(astStatsCfg);

/************************************************************************
 * Forward Declarations
 ************************************************************************/

/************************************************************************
 * Local Functions
 ***********************************************************************/
static void do_start_stop(t_StVppCtx *pstCtx, void *pv,
                          uint32_t u32Key, uint32_t u32Cnt)
{
    // Issue start and stop for a u32Key, u32Cnt times.
    uint32_t i, u32;
    for (i = 0; i < u32Cnt; i++)
    {
        u32 = u32VppStats_Start(pstCtx, pv, u32Key);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        usleep(rand() % 2000);

        u32VppStats_Stop(pstCtx, pv, u32Key);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
    }
}

static t_StVppStat *find_stat_in_blk(t_StVppStatBlock *pstBlk, uint32_t u32Key)
{
    // Explicitly copied... really don't want to expose this function as non
    // static to other parts of code.

    if (!pstBlk)
        return NULL;

    uint32_t i;
    for (i = 0; i < pstBlk->u32StatCnt; i++)
    {
        if (pstBlk->pstStat[i].stCfg.u32Key == u32Key)
            return &pstBlk->pstStat[i];
    }
    return NULL;
}

/************************************************************************
 * Test Functions
 ***********************************************************************/
TEST_SUITE_INIT(StatsSuiteInit)
{
}

TEST_SUITE_TERM(StatsSuiteTerm)
{
}

TEST_SETUP(StatsTestInit)
{
}

TEST_CLEANUP(StatsTestTerm)
{
}

TEST(StatsSanity)
{
    void *pv;
    uint32_t u32;
    t_StVppCtx stCtx;
    t_StVppStatBlock stStatBlk;

    pv = &stStatBlk;

    u32 = u32VppStats_Init(NULL);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    u32 = u32VppStats_Term(NULL);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    u32 = u32VppStats_Register(NULL, NULL, 0, NULL);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    u32 = u32VppStats_Register(&stCtx, NULL, 0, NULL);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    u32 = u32VppStats_Register(&stCtx, astStatsCfg, 0, NULL);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    u32 = u32VppStats_Register(&stCtx, astStatsCfg, u32StatsCnt, NULL);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    u32 = u32VppStats_Register(NULL, astStatsCfg, u32StatsCnt, NULL);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    u32 = u32VppStats_Register(NULL, NULL, u32StatsCnt, NULL);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    u32 = u32VppStats_Unregister(NULL, NULL);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    u32 = u32VppStats_Unregister(&stCtx, NULL);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    u32 = u32VppStats_Unregister(NULL, &pv);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    u32 = u32VppStats_Start(NULL, NULL, 0);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    u32 = u32VppStats_Start(&stCtx, NULL, 0);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    u32 = u32VppStats_Start(NULL, pv, 0);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    u32 = u32VppStats_Start(&stCtx, pv, 0);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    u32 = u32VppStats_Stop(NULL, NULL, 0);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    u32 = u32VppStats_Stop(&stCtx, NULL, 0);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    u32 = u32VppStats_Stop(NULL, pv, 0);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    u32 = u32VppStats_Stop(&stCtx, pv, 0);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    u32 = u32VppStats_ResetBlock(NULL, NULL);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    u32 = u32VppStats_ResetBlock(&stCtx, NULL);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    u32 = u32VppStats_ResetBlock(NULL, pv);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    u32 = u32VppStats_ResetBlock(&stCtx, pv);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    u32 = u32VppStats_ResetStat(NULL, NULL, 0);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    u32 = u32VppStats_ResetStat(&stCtx, NULL, 0);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    u32 = u32VppStats_ResetStat(NULL, pv, 0);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    u32 = u32VppStats_ResetStat(&stCtx, pv, 0);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);
}

TEST(StatsBasic)
{
    uint32_t u32;
    void *pv = NULL;
    t_StVppCtx stCtx;

    memset(&stCtx, 0, sizeof(stCtx));

    u32 = u32VppStats_Init(&stCtx);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = u32VppStats_Register(&stCtx, astStatsCfg, u32StatsCnt, &pv);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = u32VppStats_Start(&stCtx, pv, TEST_STATS_NUMBER_TWO);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = u32VppStats_Stop(&stCtx, pv, TEST_STATS_NUMBER_TWO);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = u32VppStats_Unregister(&stCtx, pv);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = u32VppStats_Term(&stCtx);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
}

TEST(StatsBadState)
{
    uint32_t u32;
    void *pv = NULL;
    t_StVppCtx stCtx;

    memset(&stCtx, 0, sizeof(stCtx));

    u32 = u32VppStats_Init(&stCtx);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = u32VppStats_Register(&stCtx, astStatsCfg, u32StatsCnt, &pv);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = u32VppStats_Start(&stCtx, pv, TEST_STATS_NUMBER_TWO);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = u32VppStats_Stop(&stCtx, pv, TEST_STATS_NUMBER_TWO);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = u32VppStats_Unregister(&stCtx, pv);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // Already unregistered, this should fail
    u32 = u32VppStats_Unregister(&stCtx, pv);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    u32 = u32VppStats_Start(&stCtx, pv, TEST_STATS_NUMBER_TWO);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    u32 = u32VppStats_Stop(&stCtx, pv, TEST_STATS_NUMBER_TWO);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    u32 = u32VppStats_ResetBlock(&stCtx, pv);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    u32 = u32VppStats_ResetStat(&stCtx, pv, TEST_STATS_NUMBER_TWO);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    u32 = u32VppStats_Term(&stCtx);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
}

TEST(StatsNotRegistered)
{
    uint32_t u32;
    void *pv = NULL;
    t_StVppCtx stCtx;

    memset(&stCtx, 0, sizeof(stCtx));

    u32 = u32VppStats_Init(&stCtx);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = u32VppStats_SetEnable(&stCtx, VPP_TRUE);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = u32VppStats_Register(&stCtx, astStatsCfg, u32StatsCnt, &pv);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = u32VppStats_Start(&stCtx, pv, TEST_STATS_NUMBER_SIX);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    u32 = u32VppStats_Start(&stCtx, pv, TEST_STATS_NUMBER_SEVEN);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    u32 = u32VppStats_Stop(&stCtx, pv, TEST_STATS_NUMBER_SEVEN);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    u32 = u32VppStats_Stop(&stCtx, pv, TEST_STATS_NUMBER_SIX);
    DVP_ASSERT_NEQUAL(u32, VPP_OK);

    u32 = u32VppStats_Unregister(&stCtx, pv);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = u32VppStats_Term(&stCtx);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
}

TEST(StatsVisual)
{
    // This test requires visual inspection of logcat, since the messages
    // are printed there...
    uint32_t u32, i;
    void *pv = NULL;
    t_StVppCtx stCtx;

    memset(&stCtx, 0, sizeof(stCtx));

    u32 = u32VppStats_Init(&stCtx);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = u32VppStats_SetEnable(&stCtx, VPP_TRUE);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = u32VppStats_Register(&stCtx, astStatsCfg, u32StatsCnt, &pv);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // Frequency is equal to the number, so for each of these, should
    // only expect to see one log print for the corresponding entry
    for (i = 0; i < TEST_STATS_MAX; i++)
        do_start_stop(&stCtx, pv, i, i + 1);

    LOGI("---- Expectation here is that the below printed log messages will\n"
         "---- have double the count as the above");

    // Run it again. Expect to see all the counts are double what they were
    // previously, but the number of prints should just be once more per log.
    for (i = 0; i < TEST_STATS_MAX; i++)
        do_start_stop(&stCtx, pv, i, i + 1);

    // Reset the entire block and send it again
    u32VppStats_ResetBlock(&stCtx, pv);

    LOGI("---- The stats block has just been reset. The expectation will\n"
         "---- be that the below log messages have same count as the first\n"
         "---- set of log messages");

    for (i = 0; i < TEST_STATS_MAX; i++)
        do_start_stop(&stCtx, pv, i, i + 1);

    LOGI("---- Expectation here is that the below printed log messages will\n"
         "---- have double the count as the above");

    for (i = 0; i < TEST_STATS_MAX; i++)
        do_start_stop(&stCtx, pv, i, i + 1);

    // Reset just one of them
    u32VppStats_ResetStat(&stCtx, pv, TEST_STATS_NUMBER_ONE);

    LOGI("---- Expectation here is that the below log messages will have\n"
         "---- triple the counts as the first set of messages, with the\n"
         "---- exception of the first stat, which should have been reset\n"
         "---- and should have a count of 1");

    for (i = 0; i < TEST_STATS_MAX; i++)
        do_start_stop(&stCtx, pv, i, i + 1);

    u32 = u32VppStats_Unregister(&stCtx, pv);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32 = u32VppStats_Term(&stCtx);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
}

#define STATS_MULTIPLE_BASIC_CNT 4
TEST(StatsMultipleBasic)
{
    uint32_t i, j;
    uint32_t u32;
    uint32_t u32Tmp;
    uint32_t u32StatsCnt;
    uint32_t u32ExpCnt, u32Key;
    t_StVppCtx stCtx;
    t_StVppStatBlock *pstStatBlk;
    t_StVppStat *pstStat;

    memset(&stCtx, 0, sizeof(stCtx));

    void *apv[STATS_MULTIPLE_BASIC_CNT] = {0};

    u32 = u32VppStats_Init(&stCtx);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    u32StatsCnt = VPP_STATS_CNT(astStatsMultiCfg);

    u32 = u32VppStats_SetEnable(&stCtx, VPP_TRUE);
    DVP_ASSERT_EQUAL(u32, VPP_OK);

    // Register and start a timer. It is expected that stats blocks can be
    // added whenever, and that the state of one block/stat does not impact
    // the overall stats
    for (i = 0; i < STATS_MULTIPLE_BASIC_CNT; i++)
    {
        u32 = u32VppStats_Register(&stCtx, astStatsMultiCfg, u32StatsCnt, &apv[i]);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
        DVP_ASSERT_PTR_NNULL(apv[i]);

        u32Tmp = i < u32StatsCnt ? i : u32StatsCnt - 1;
        u32 = u32VppStats_Start(&stCtx, apv[i], astStatsMultiCfg[u32Tmp].u32Key);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        if (i)
            DVP_ASSERT_PTR_NEQUAL(apv[i], apv[i - 1]);
    }

    // Stop the stats in the same order that they were started
    for (i = 0; i < STATS_MULTIPLE_BASIC_CNT; i++)
    {
        u32Tmp = i < u32StatsCnt ? i : u32StatsCnt - 1;
        u32 = u32VppStats_Stop(&stCtx, apv[i], astStatsMultiCfg[u32Tmp].u32Key);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
    }

    // Up until this point, each context has been called with a start for the
    // nth key that is defined in the list of stats (i.e. for context 1, key 1
    // has a start/stop, context 2 has start/stop for 2, context 3 has
    // start/stop for 3rd item in the config array, etc. So the below code will
    // iterate over each context, and foreach context, do a start/stop pair for
    // each key that is enabled in that list. The result that is display should
    // be as follows: for each block up until the total number of blocks, the
    // count on each key that shares an index with the block will be two, and
    // the rest of the counts should be 1. For keys for which the index in the
    // configuration list exceeds the max number of enabled blocks, those keys
    // shall be one.
    for (i = 0; i < STATS_MULTIPLE_BASIC_CNT; i++)
    {
        LOGE("---- divider ----");
        for (j = 0; j < u32StatsCnt; j++)
        {
            u32Key = astStatsMultiCfg[j].u32Key;
            do_start_stop(&stCtx, apv[i], u32Key, 1);

            // Check in code that the counts are correct (requires white box
            // knowledge of the implementation)
            pstStatBlk = (t_StVppStatBlock *)apv[i];
            pstStat = find_stat_in_blk(pstStatBlk, u32Key);
            if (!pstStat)
            {
                DVP_ASSERT_FAIL();
                break;
            }
            u32ExpCnt = i == j ? 2 : 1;
            DVP_ASSERT_EQUAL(pstStat->u32Cnt, u32ExpCnt);
        }
    }

    // Reset the nth stat in the nth block, and check
    for (i = 0; i < STATS_MULTIPLE_BASIC_CNT; i++)
    {
        if (i >= u32StatsCnt)
            break;

        u32Key = astStatsMultiCfg[i].u32Key;
        u32 = u32VppStats_ResetStat(&stCtx, apv[i], u32Key);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        pstStatBlk = (t_StVppStatBlock *)apv[i];
        pstStat = find_stat_in_blk(pstStatBlk, u32Key);
        if (!pstStat)
        {
            DVP_ASSERT_FAIL();
            break;
        }
        DVP_ASSERT_EQUAL(pstStat->u32Cnt, 0);
    }

    // Reset each block
    for (i = 0; i < STATS_MULTIPLE_BASIC_CNT; i++)
    {
        u32 = u32VppStats_ResetBlock(&stCtx, apv[i]);
        DVP_ASSERT_EQUAL(u32, VPP_OK);

        pstStatBlk = (t_StVppStatBlock *)apv[i];

        for (j = 0; j < pstStatBlk->u32StatCnt; j++)
        {
            pstStat = &pstStatBlk->pstStat[j];
            DVP_ASSERT_EQUAL(0, pstStat->u32Cnt);
            DVP_ASSERT_EQUAL(0, pstStat->u32TotalTime);
            DVP_ASSERT_EQUAL(0, pstStat->u32Average);
            DVP_ASSERT_EQUAL(0, pstStat->u32Max);
            DVP_ASSERT_EQUAL(0, pstStat->u32Min);
        }
    }

    // Unregister and quit
    for (i = 0; i < STATS_MULTIPLE_BASIC_CNT; i++)
    {
        u32 = u32VppStats_Unregister(&stCtx, apv[i]);
        DVP_ASSERT_EQUAL(u32, VPP_OK);
    }

    u32 = u32VppStats_Term(&stCtx);
    DVP_ASSERT_EQUAL(u32, VPP_OK);
}

/************************************************************************
 * Global Functions
 ***********************************************************************/
TEST_CASES StatsTests[] = {
    TEST_CASE(StatsSanity),
    TEST_CASE(StatsBasic),
    TEST_CASE(StatsBadState),
    TEST_CASE(StatsNotRegistered),
    TEST_CASE(StatsVisual),
    TEST_CASE(StatsMultipleBasic),
    TEST_CASE_NULL(),
};

TEST_SUITE(StatsSuite,
           "Stats Tests",
           StatsSuiteInit,
           StatsSuiteTerm,
           StatsTestInit,
           StatsTestTerm,
           StatsTests);
