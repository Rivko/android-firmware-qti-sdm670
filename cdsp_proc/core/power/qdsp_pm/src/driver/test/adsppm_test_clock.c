/*
* Copyright (c) 2014 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

#include "mmpm.h"
#include "adsppm_test.h"
#include "adsppm_test_param.h"
#include "adsppm_test_utils.h"
#include "adsppm_test_asic_info.h"


#define CORECLOCK_01_NAME    "coreclock_01"
#define CORECLOCK_01_DETAILS "Req-Rel, all core/instance/clock combination"

static MMPM_STATUS TestAllCoresClockRequest(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    uint32 clientId = 0;
    MmpmCoreIdType coreId;
    MmpmCoreInstanceIdType instanceId;
    MmpmClkIdLpassType clkId;
    ClkTestType clkReq;

    for(coreId = MMPM_CORE_ID_LPASS_ADSP; coreId < MMPM_CORE_ID_LPASS_END; coreId++)
    {
        for(instanceId = MMPM_CORE_INSTANCE_0; instanceId < MMPM_CORE_INSTANCE_MAX; instanceId++)
        {
            if(AsicInfo_isCoreInstanceAvailable(coreId, instanceId))
            {
                clientId = Test_Register(coreId, instanceId, CORECLOCK_01_NAME);
                if(clientId)
                {
                    for(clkId = MMPM_CLK_ID_LPASS_HWRSP_CORE; clkId < MMPM_CLK_ID_LPASS_ADSP_CORE; clkId++)
                    {
                        if(AsicInfo_isCoreClockAvailable(clkId))
                        {
                            clkReq.clkId = clkId;
                            clkReq.freq = 19000000;
                            clkReq.freqMatch = MMPM_FREQ_AT_LEAST;

                            sts = Test_RequestCoreClock(clientId, &clkReq, 1);
                            if(MMPM_STATUS_SUCCESS != sts)
                            {
                                retSts = MMPM_STATUS_FAILED;
                            }

                            sts = Test_ReleaseResource(clientId, MMPM_RSC_ID_CORE_CLK);
                            if(MMPM_STATUS_SUCCESS != sts)
                            {
                                retSts = MMPM_STATUS_FAILED;
                            }
                        }
                    }

                    sts = Test_Deregister(clientId);
                    if(MMPM_STATUS_SUCCESS != sts)
                    {
                        retSts = MMPM_STATUS_FAILED;
                    }
                }
                else
                {
                    retSts = MMPM_STATUS_FAILED;
                }
            }
        }
    }

    return retSts;
}

/**
 * @ingroup CoreClock
 * @test coreclock_01
 * @brief Req-Rel, all core/instance/clock combination
 */
static MMPM_STATUS Test_CoreClock_01(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(CORECLOCK_01_NAME, CORECLOCK_01_DETAILS);

    sts = TestAllCoresClockRequest();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_CoreClock, CORECLOCK_01_NAME, sts);
    return sts;
}


#define CORECLOCK_02_NAME    "coreclock_02"
#define CORECLOCK_02_DETAILS "All core clocks frequency plan"

static MMPM_STATUS TestClkFreqPlan(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    uint32 numFreq, clientId = 0, readClkFreq;
    MmpmClkIdLpassType clkId;
    const AsicInfo_ClockFreqVoltPlanType *pFreqPlan = NULL;
    MmpmCoreIdType coreId = MMPM_CORE_ID_LPASS_ADSP;
    MmpmCoreInstanceIdType instanceId = MMPM_CORE_INSTANCE_0;
    ClkTestType clk;

    clientId = Test_Register(coreId, instanceId, CORECLOCK_02_NAME);
    if(0 != clientId)
    {
        for(clkId = MMPM_CLK_ID_LPASS_HWRSP_CORE; clkId < MMPM_CLK_ID_LPASS_ADSP_CORE; clkId++)
        {
            pFreqPlan = AsicInfo_getClockFreqVoltPlan(clkId);

            clk.freqMatch = MMPM_FREQ_AT_LEAST;
            clk.clkId = clkId;

            for(numFreq = 0; numFreq < pFreqPlan->numFreq; numFreq++)
            {
                clk.freq = pFreqPlan->pFreqVoltPlan[numFreq].frequency;

                sts = Test_RequestCoreClock(clientId, &clk, 1);
                if(MMPM_STATUS_SUCCESS != sts)
                {
                    retSts = MMPM_STATUS_FAILED;
                }
                else
                {
                    sts = Test_InfoClk(coreId, instanceId, clk.clkId, &readClkFreq);
                    if(MMPM_STATUS_SUCCESS != sts)
                    {
                        retSts = MMPM_STATUS_FAILED;
                    }
                    else
                    {
                        if(!Test_CompareFreq(clk.freq, readClkFreq))
                        {
                            ADSPPMTEST_LOG_PRINTF_3(
                                ADSPPMTEST_LOG_LEVEL_ERROR,
                                "[ERROR] clkID(%u): setClkFreq = %u Hz, readClkFreq = %u Hz",
                                clkId, clk.freq, readClkFreq);
                            retSts = MMPM_STATUS_FAILED;
                        }
                    }
                }

                sts = Test_ReleaseResource(clientId, MMPM_RSC_ID_CORE_CLK);
                if(MMPM_STATUS_SUCCESS != sts)
                {
                    retSts = MMPM_STATUS_FAILED;
                }
            }
        }

        sts = Test_Deregister(clientId);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            retSts = MMPM_STATUS_FAILED;
        }
    }
    else
    {
        retSts = MMPM_STATUS_FAILED;
    }

    return retSts;
}

/**
 * @ingroup CoreClock
 * @test coreclock_02
 * @brief All core clocks frequency plan
 */
static MMPM_STATUS Test_CoreClock_02(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(CORECLOCK_02_NAME, CORECLOCK_02_DETAILS);

    sts = TestClkFreqPlan();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_CoreClock, CORECLOCK_02_NAME, sts);
    return sts;
}


#define CORECLOCK_03_NAME    "coreclock_03"
#define CORECLOCK_03_DETAILS "Req-Rel, 2 same coreID/instanceID clients"

static AdsppmTestType testcoreclock03[] =
{
    {1, 2, MMPM_CORE_ID_LPASS_SLIMBUS, MMPM_CORE_INSTANCE_0,
        {REG, REQCLK}
    },
    {2, 2, MMPM_CORE_ID_LPASS_SLIMBUS, MMPM_CORE_INSTANCE_0,
        {REG, REQCLK}
    },
    {1, 2, MMPM_CORE_ID_LPASS_SLIMBUS, MMPM_CORE_INSTANCE_0,
        {RELCLK, DREG}
    },
    {2, 2, MMPM_CORE_ID_LPASS_SLIMBUS, MMPM_CORE_INSTANCE_0,
        {RELCLK, DREG}
    }
};

/**
 * @ingroup CoreClock
 * @test coreclock_03
 * @brief Req-Rel, 2 same coreID/instanceID clients
 */
static MMPM_STATUS Test_CoreClock_03(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, testSts[MAX_TEST][MAX_TEST_SEQUENCE];
    uint32 numTest = 0;
    ADSPPMTEST_LOG_TEST_HEADER(CORECLOCK_03_NAME, CORECLOCK_03_DETAILS);

    numTest = sizeof(testcoreclock03)/sizeof(AdsppmTestType);
    sts = Test_InvokeTest(testcoreclock03, numTest, testSts);

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_CoreClock, CORECLOCK_03_NAME, sts);
    return sts;
}


#define CORECLOCK_04_NAME    "coreclock_04"
#define CORECLOCK_04_DETAILS "Req-Rel-Rel, expect NO errors"

static AdsppmTestType testcoreclock04[] =
{
    {1, 5, MMPM_CORE_ID_LPASS_SLIMBUS, MMPM_CORE_INSTANCE_0,
        {REG, REQCLK, RELCLK, RELCLK, DREG}
    }
};

/**
 * @ingroup CoreClock
 * @test coreclock_04
 * @brief Req-Rel-Rel, expect NO errors
 */
static MMPM_STATUS Test_CoreClock_04(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, testSts[MAX_TEST][MAX_TEST_SEQUENCE];
    uint32 numTest = 0;
    ADSPPMTEST_LOG_TEST_HEADER(CORECLOCK_04_NAME, CORECLOCK_04_DETAILS);

    numTest = sizeof(testcoreclock04)/sizeof(AdsppmTestType);
    sts = Test_InvokeTest(testcoreclock04, numTest, testSts);

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_CoreClock, CORECLOCK_04_NAME, sts);
    return sts;
}


#define CORECLOCK_05_NAME    "coreclock_05"
#define CORECLOCK_05_DETAILS "Rel with no Req, expect NO errors"

static AdsppmTestType testcoreclock05[] =
{
    {1, 3, MMPM_CORE_ID_LPASS_SLIMBUS, MMPM_CORE_INSTANCE_0,
        {REG, RELCLK, DREG}
    }
};

/**
 * @ingroup CoreClock
 * @test coreclock_05
 * @brief Rel with no Req, expect NO errors
 */
static MMPM_STATUS Test_CoreClock_05(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, testSts[MAX_TEST][MAX_TEST_SEQUENCE];
    uint32 numTest = 0;
    ADSPPMTEST_LOG_TEST_HEADER(CORECLOCK_05_NAME, CORECLOCK_05_DETAILS);

    numTest = sizeof(testcoreclock05)/sizeof(AdsppmTestType);
    sts = Test_InvokeTest(testcoreclock05, numTest, testSts);

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_CoreClock, CORECLOCK_05_NAME, sts);
    return sts;
}


#define CORECLOCK_06_NAME    "coreclock_06"
#define CORECLOCK_06_DETAILS "Req different frequencies from same client, expect clock to be set at last Req frequency before Rel"

static MMPM_STATUS TestCoreClockDiffFreqFromSameClient(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    uint32 clientId = 0, clkFreq = 0;
    const AsicInfo_ClockFreqVoltPlanType *pFreqPlan = NULL;
    MmpmCoreIdType coreId = MMPM_CORE_ID_LPASS_HWRSMP;
    MmpmCoreInstanceIdType instanceId = MMPM_CORE_INSTANCE_0;
    ClkTestType clk;

    clientId = Test_Register(coreId, instanceId, CORECLOCK_06_NAME);
    pFreqPlan = AsicInfo_getClockFreqVoltPlan(MMPM_CLK_ID_LPASS_HWRSP_CORE);

    clk.clkId = MMPM_CLK_ID_LPASS_HWRSP_CORE;
    clk.freqMatch = MMPM_FREQ_AT_LEAST;

    if(clientId)
    {
        clk.freq = pFreqPlan->pFreqVoltPlan[0].frequency;
        sts = Test_RequestCoreClock(clientId, &clk, 1);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            retSts = MMPM_STATUS_FAILED;
        }

        clk.freq = pFreqPlan->pFreqVoltPlan[1].frequency;
        sts = Test_RequestCoreClock(clientId, &clk, 1);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            retSts = MMPM_STATUS_FAILED;
        }

        sts = Test_InfoClk(coreId, instanceId, clk.clkId, &clkFreq);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            retSts = MMPM_STATUS_FAILED;
        }
        else
        {
            if(!Test_CompareFreq(clk.freq, clkFreq))
            {
                ADSPPMTEST_LOG_PRINTF_2(
                    ADSPPMTEST_LOG_LEVEL_ERROR,
                    "[ERROR] setClkFreq = %u Hz, readClkFreq = %u Hz",
                    clk.freq, clkFreq);
                retSts = MMPM_STATUS_FAILED;
            }
        }

        sts = Test_ReleaseResource(clientId, MMPM_RSC_ID_CORE_CLK);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            retSts = MMPM_STATUS_FAILED;
        }

        sts = Test_Deregister(clientId);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            retSts = MMPM_STATUS_FAILED;
        }
    }
    else
    {
        retSts = MMPM_STATUS_FAILED;
    }

    return retSts;
}

/**
 * @ingroup CoreClock
 * @test coreclock_06
 * @brief Req different frequencies from same client, expect clock to be set at last Req frequency before Rel
 */
static MMPM_STATUS Test_CoreClock_06(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(CORECLOCK_06_NAME, CORECLOCK_06_DETAILS);

    sts = TestCoreClockDiffFreqFromSameClient();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_CoreClock, CORECLOCK_06_NAME, sts);
    return sts;
}


#define CORECLOCK_07_NAME    "coreclock_07"
#define CORECLOCK_07_DETAILS "Req different frequencies from different clients, expect clock to be set at highest frequency before Rel"

static MMPM_STATUS TestCoreClockDiffFreqFromDiffClients(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    uint32 clientId1 = 0, clientId2 = 0, clkFreq = 0;
    uint32 clkFreq1, clkFreq2;
    const AsicInfo_ClockFreqVoltPlanType *pFreqPlan = NULL;
    MmpmCoreIdType coreId = MMPM_CORE_ID_LPASS_HWRSMP;
    MmpmCoreInstanceIdType instanceId = MMPM_CORE_INSTANCE_0;
    ClkTestType clk;

    clientId1 = Test_Register(coreId, instanceId, CORECLOCK_07_NAME"_1");
    clientId2 = Test_Register(coreId, instanceId, CORECLOCK_07_NAME"_2");
    pFreqPlan = AsicInfo_getClockFreqVoltPlan(MMPM_CLK_ID_LPASS_HWRSP_CORE);

    clk.clkId = MMPM_CLK_ID_LPASS_HWRSP_CORE;
    clk.freqMatch = MMPM_FREQ_AT_LEAST;
    clkFreq1 = pFreqPlan->pFreqVoltPlan[0].frequency;
    clkFreq2 = pFreqPlan->pFreqVoltPlan[1].frequency;

    if(clientId1 && clientId2)
    {
        clk.freq = clkFreq1;
        sts = Test_RequestCoreClock(clientId1, &clk, 1);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            retSts = MMPM_STATUS_FAILED;
        }

        clk.freq = clkFreq2;

        sts = Test_RequestCoreClock(clientId2, &clk, 1);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            retSts = MMPM_STATUS_FAILED;
        }

        sts = Test_InfoClk(coreId, instanceId, clk.clkId, &clkFreq);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            retSts = MMPM_STATUS_FAILED;
        }
        else
        {
            if(!Test_CompareFreq(clkFreq2, clkFreq))
            {
                ADSPPMTEST_LOG_PRINTF_2(
                    ADSPPMTEST_LOG_LEVEL_ERROR,
                    "[ERROR] Higher clock setting: setClkFreq = %u Hz, readClkFreq = %u Hz",
                    clkFreq2, clkFreq);
                retSts = MMPM_STATUS_FAILED;
            }
        }

        sts = Test_ReleaseResource(clientId2, MMPM_RSC_ID_CORE_CLK);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            retSts = MMPM_STATUS_FAILED;
        }

        sts = Test_InfoClk(coreId, instanceId, clk.clkId, &clkFreq);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            retSts = MMPM_STATUS_FAILED;
        }
        else
        {
            if(!Test_CompareFreq(clkFreq1, clkFreq))
            {
                ADSPPMTEST_LOG_PRINTF_2(
                    ADSPPMTEST_LOG_LEVEL_ERROR,
                    "[ERROR] Lower clock setting: setClkFreq = %u Hz, readClkFreq = %u Hz",
                    clkFreq1, clkFreq);
                retSts = MMPM_STATUS_FAILED;
            }
        }

        sts = Test_ReleaseResource(clientId1, MMPM_RSC_ID_CORE_CLK);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            retSts = MMPM_STATUS_FAILED;
        }

        sts = Test_Deregister(clientId1);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            retSts = MMPM_STATUS_FAILED;
        }

        sts = Test_Deregister(clientId2);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            retSts = MMPM_STATUS_FAILED;
        }
    }
    else
    {
        Test_Deregister(clientId1);
        Test_Deregister(clientId2);
        retSts = MMPM_STATUS_FAILED;
    }

    return retSts;
}

/**
 * @ingroup CoreClock
 * @test coreclock_07
 * @brief Req different frequencies from different clients, expect clock to be set at highest frequency before Rel
 */
static MMPM_STATUS Test_CoreClock_07(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(CORECLOCK_07_NAME, CORECLOCK_07_DETAILS);

    sts = TestCoreClockDiffFreqFromDiffClients();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_CoreClock, CORECLOCK_07_NAME, sts);
    return sts;
}


#define CORECLOCK_08_NAME    "coreclock_08"
#define CORECLOCK_08_DETAILS "Req same frequency from different clients, expect clock to be set at such frequency before Rel"

static MMPM_STATUS TestCoreClockSameFreqDiffClients(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    uint32 clientId1 = 0, clientId2 = 0, clkFreq = 0;
    const AsicInfo_ClockFreqVoltPlanType *pFreqPlan = NULL;
    MmpmCoreIdType coreId = MMPM_CORE_ID_LPASS_HWRSMP;
    MmpmCoreInstanceIdType instanceId = MMPM_CORE_INSTANCE_0;
    ClkTestType clk;

    clientId1 = Test_Register(coreId, instanceId, CORECLOCK_08_NAME"_1");
    clientId2 = Test_Register(coreId, instanceId, CORECLOCK_08_NAME"_2");
    pFreqPlan = AsicInfo_getClockFreqVoltPlan(MMPM_CLK_ID_LPASS_HWRSP_CORE);

    if(clientId1 && clientId2)
    {
        clk.clkId = MMPM_CLK_ID_LPASS_HWRSP_CORE;
        clk.freq = pFreqPlan->pFreqVoltPlan[1].frequency;
        clk.freqMatch = MMPM_FREQ_AT_LEAST;

        sts = Test_RequestCoreClock(clientId1, &clk, 1);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            retSts = MMPM_STATUS_FAILED;
        }

        sts = Test_RequestCoreClock(clientId2, &clk, 1);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            retSts = MMPM_STATUS_FAILED;
        }

        sts = Test_InfoClk(coreId, instanceId, clk.clkId, &clkFreq);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            retSts = MMPM_STATUS_FAILED;
        }
        else
        {
            if(!Test_CompareFreq(clk.freq, clkFreq))
            {
                ADSPPMTEST_LOG_PRINTF_2(
                    ADSPPMTEST_LOG_LEVEL_ERROR,
                    "[ERROR] setClkFreq = %u Hz, readClkFreq = %u Hz",
                    clk.freq, clkFreq);
                retSts = MMPM_STATUS_FAILED;
            }
        }

        sts = Test_ReleaseResource(clientId1, MMPM_RSC_ID_CORE_CLK);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            retSts = MMPM_STATUS_FAILED;
        }

        sts = Test_ReleaseResource(clientId2, MMPM_RSC_ID_CORE_CLK);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            retSts = MMPM_STATUS_FAILED;
        }

        sts = Test_Deregister(clientId1);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            retSts = MMPM_STATUS_FAILED;
        }

        sts = Test_Deregister(clientId2);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            retSts = MMPM_STATUS_FAILED;
        }
    }
    else
    {
        Test_Deregister(clientId1);
        Test_Deregister(clientId2);
        retSts = MMPM_STATUS_FAILED;
    }

    return retSts;
}

/**
 * @ingroup CoreClock
 * @test coreclock_08
 * @brief Req same frequency from different clients, expect clock to be set at such frequency before Rel
 */
static MMPM_STATUS Test_CoreClock_08(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(CORECLOCK_08_NAME, CORECLOCK_08_DETAILS);

    sts = TestCoreClockSameFreqDiffClients();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_CoreClock, CORECLOCK_08_NAME, sts);
    return sts;
}


#define CORECLOCK_09_NAME    "coreclock_09"
#define CORECLOCK_09_DETAILS "Invalid client ID Req, expect failure Req"

static MMPM_STATUS TestCoreClockInvalidClientId(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    uint32 clientId = 0;
    MmpmCoreIdType coreId = MMPM_CORE_ID_LPASS_ADSP;
    MmpmCoreInstanceIdType instanceId = MMPM_CORE_INSTANCE_0;
    ClkTestType clk;
    MmpmClkIdLpassType clkId;

    // Find a valid core clock ID
    for(clkId = MMPM_CLK_ID_LPASS_HWRSP_CORE; MMPM_CLK_ID_LPASS_ADSP_CORE > clkId; clkId++)
    {
        if(AsicInfo_isCoreClockAvailable(clkId))
        {
            break;
        }
    }

    if(MMPM_CLK_ID_LPASS_ADSP_CORE > clkId)
    {
        clientId = Test_Register(coreId, instanceId, CORECLOCK_09_NAME);

        if(clientId)
        {
            clk.clkId = clkId;
            clk.freq = 10000000;
            clk.freqMatch = MMPM_FREQ_AT_LEAST;

            sts = Test_Deregister(clientId);
            if(MMPM_STATUS_SUCCESS != sts)
            {
                retSts = MMPM_STATUS_FAILED;
            }

            sts = Test_RequestCoreClock(clientId, &clk, 1);
            if(MMPM_STATUS_SUCCESS == sts)
            {
                ADSPPMTEST_LOG_PRINTF_0(
                    ADSPPMTEST_LOG_LEVEL_ERROR,
                    "[ERROR] Request did not fail as intended");
                retSts = MMPM_STATUS_FAILED;
            }
            else
            {
                ADSPPMTEST_LOG_PRINTF_0(
                    ADSPPMTEST_LOG_LEVEL_ERROR,
                    "[PASS] Failed return is intended");
            }
        }
        else
        {
            retSts = MMPM_STATUS_FAILED;
        }
    }
    else
    {
        ADSPPMTEST_LOG_PRINTF_0(
            ADSPPMTEST_LOG_LEVEL_ERROR,
            "[ERROR] Unable to find valid Core Clock ID");
        retSts = MMPM_STATUS_FAILED;
    }

    return retSts;
}

/**
 * @ingroup CoreClock
 * @test coreclock_09
 * @brief Invalid client ID Req, expect failure Req
 */
static MMPM_STATUS Test_CoreClock_09(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(CORECLOCK_09_NAME, CORECLOCK_09_DETAILS);

    sts = TestCoreClockInvalidClientId();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_CoreClock, CORECLOCK_09_NAME, sts);
    return sts;
}


#define CORECLOCK_10_NAME    "coreclock_10"
#define CORECLOCK_10_DETAILS "Invalid parameters Req, expect failure Req"

static MMPM_STATUS TestCoreClockInvalidClockParam(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    uint32 clientId = 0;
    MmpmCoreIdType coreId = MMPM_CORE_ID_LPASS_SLIMBUS;
    MmpmCoreInstanceIdType instanceId = MMPM_CORE_INSTANCE_0;
    ClkTestType clk;

    clientId = Test_Register(coreId, instanceId, CORECLOCK_10_NAME);

    clk.clkId = MMPM_CLK_ID_LPASS_SLIMBUS_CORE;
    clk.freq = 10000000;
    clk.freqMatch = MMPM_FREQ_AT_LEAST;

    if(clientId)
    {
        // Bad freqMatch
        ADSPPMTEST_LOG_PRINTF_0(
            ADSPPMTEST_LOG_LEVEL_INFO, "[INFO] Test invalid freqMatch");
        clk.freqMatch = MMPM_FREQ_MAX;
        sts = Test_RequestCoreClock(clientId, &clk, 1);
        if(MMPM_STATUS_SUCCESS == sts)
        {
            ADSPPMTEST_LOG_PRINTF_0(
                ADSPPMTEST_LOG_LEVEL_ERROR,
                "[ERROR] Request returned success when expecting failure");
            retSts = MMPM_STATUS_FAILED;
        }
        else
        {
            ADSPPMTEST_LOG_PRINTF_0(
                ADSPPMTEST_LOG_LEVEL_ERROR,
                "[PASS] Failed return is intended");
        }
        clk.freqMatch = MMPM_FREQ_AT_LEAST;

        // Bad clkId
        ADSPPMTEST_LOG_PRINTF_0(
            ADSPPMTEST_LOG_LEVEL_INFO, "[INFO] Test invalid clkId");
        clk.clkId += 1000;
        sts = Test_RequestCoreClock(clientId, &clk, 1);
        if(MMPM_STATUS_SUCCESS == sts)
        {
            ADSPPMTEST_LOG_PRINTF_0(
                ADSPPMTEST_LOG_LEVEL_ERROR,
                "[ERROR] Request returned success when expecting failure");
            retSts = MMPM_STATUS_FAILED;
        }
        else
        {
            ADSPPMTEST_LOG_PRINTF_0(
                ADSPPMTEST_LOG_LEVEL_ERROR,
                "[PASS] Failed return is intended");
        }
        clk.clkId = MMPM_CLK_ID_LPASS_SLIMBUS_CORE;

        sts = Test_Deregister(clientId);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            retSts = MMPM_STATUS_FAILED;
        }
    }
    else
    {
        retSts = MMPM_STATUS_FAILED;
    }

    return retSts;
}

/**
 * @ingroup CoreClock
 * @test coreclock_10
 * @brief Invalid parameters Req, expect failure Req
 */
static MMPM_STATUS Test_CoreClock_10(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(CORECLOCK_10_NAME, CORECLOCK_10_DETAILS);

    sts = TestCoreClockInvalidClockParam();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_CoreClock, CORECLOCK_10_NAME, sts);
    return sts;
}


void Test_CoreClock()
{
    // Req-Rel, all core/instance/clock combination
    Test_CoreClock_01();

    // All core clocks frequency plan
    Test_CoreClock_02();

    // Req-Rel, 2 same coreID/instanceID clients
    Test_CoreClock_03();

    // Req-Rel-Rel, expect NO errors
    Test_CoreClock_04();

    // Rel with no Req, expect NO errors
    Test_CoreClock_05();

    // Req different frequencies from same client, expect clock to be set at last Req frequency before Rel
    Test_CoreClock_06();

    // Req different frequencies from different clients, expect clock to be set at highest frequency before Rel
    Test_CoreClock_07();

    // Req same frequency from different clients, expect clock to be set at such frequency before Rel
    Test_CoreClock_08();

    // Invalid client ID Req, expect failure Req
    Test_CoreClock_09();

    // Invalid parameters Req, expect failure Req
    Test_CoreClock_10();
}


/***************************************************************************/
/****************** Test Code for CDSP is in below section *****************/
/***************************************************************************/

static MMPM_STATUS TestCDSPCoresClockRequest(MmpmCoreIdType coreId,
                                             MmpmClkIdComputeType clkId )
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    uint32 clientId = 0;

    ClkTestType clkReq;

    clientId = Test_Register(coreId, MMPM_CORE_INSTANCE_0, "CoreClock");
    if(clientId)
    {
       if(AsicInfo_isCoreClockAvailable((MmpmClkIdLpassType)clkId))
       {
           clkReq.clkId = (MmpmClkIdLpassType)clkId;
           clkReq.freq = 19000000;
           clkReq.freqMatch = MMPM_FREQ_AT_LEAST;

           sts = Test_RequestCoreClock(clientId, &clkReq, 1);
           if(MMPM_STATUS_SUCCESS != sts)
           {
               retSts = MMPM_STATUS_FAILED;
           }

           sts = Test_ReleaseResource(clientId, MMPM_RSC_ID_CORE_CLK);
           if(MMPM_STATUS_SUCCESS != sts)
           {
               retSts = MMPM_STATUS_FAILED;
           }

       }

       sts = Test_Deregister(clientId);
       if(MMPM_STATUS_SUCCESS != sts)
       {
           retSts = MMPM_STATUS_FAILED;
       }
    }
    else
    {
        retSts = MMPM_STATUS_FAILED;
    }
    return retSts;
}



#define CDSP_CORECLOCK_10_NAME    "coreclock_1"
#define CDSP_CORECLOCK_10_DETAILS "CDSP request VAPSS Core Clock"
#define CDSP_CORECLOCK_ID 0x00CC0001

void Test_CDSP_CoreClock_01( void )
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(CDSP_CORECLOCK_10_NAME, CDSP_CORECLOCK_10_DETAILS);

    sts = TestCDSPCoresClockRequest(MMPM_CORE_ID_COMPUTE_HCP, MMPM_CLK_ID_COMPUTE_VAPSS_CORE);
    if(MMPM_STATUS_SUCCESS == sts)
        sts = TestCDSPCoresClockRequest(MMPM_CORE_ID_COMPUTE_DMA, MMPM_CLK_ID_COMPUTE_VAPSS_CORE);

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_Power, CDSP_CORECLOCK_10_NAME, sts);
}

void Test_CDSP_CoreClock( void )
{
    Test_CDSP_CoreClock_01();
}

/***************************************************************************/
/****************** Test Code for SLPI is in below section *****************/
/***************************************************************************/

static MMPM_STATUS TestSlpiCoresClockRequest(MmpmCoreIdType coreId,
                                                        MmpmClkIdSlpiType clkId )
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    uint32 clientId = 0;

    ClkTestType clkReq;

    clientId = Test_Register(coreId, MMPM_CORE_INSTANCE_0, "CoreClock");
    if(clientId)
    {
       if(AsicInfo_isCoreClockAvailable((MmpmClkIdLpassType)clkId))
       {
           clkReq.clkId = (MmpmClkIdLpassType)clkId;
           clkReq.freq = 19000000;
           clkReq.freqMatch = MMPM_FREQ_AT_LEAST;

           sts = Test_RequestCoreClock(clientId, &clkReq, 1);
           if(MMPM_STATUS_SUCCESS != sts)
           {
               retSts = MMPM_STATUS_FAILED;
           }

           sts = Test_ReleaseResource(clientId, MMPM_RSC_ID_CORE_CLK);
           if(MMPM_STATUS_SUCCESS != sts)
           {
               retSts = MMPM_STATUS_FAILED;
           }

       }

       sts = Test_Deregister(clientId);
       if(MMPM_STATUS_SUCCESS != sts)
       {
           retSts = MMPM_STATUS_FAILED;
       }
    }
    else
    {
        retSts = MMPM_STATUS_FAILED;
    }
    return retSts;
}



#define SLPI_CORECLOCK_10_NAME    "coreclock_1"
#define SLPI_CORECLOCK_10_DETAILS "Slpi request SDC/Qup/CCD Core Clock"
#define SLPI_CORECLOCK_ID 0x00CC0001

void Test_Slpi_CoreClock_01( void )
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(SLPI_CORECLOCK_10_NAME, SLPI_CORECLOCK_10_DETAILS);

    sts = TestSlpiCoresClockRequest(MMPM_CORE_ID_SLPI_SDC, MMPM_CLK_ID_SLPI_SDC_CORE);
    if(MMPM_STATUS_SUCCESS == sts)
        sts = TestSlpiCoresClockRequest(MMPM_CORE_ID_SLPI_QUP, MMPM_CLK_ID_SLPI_QUP_CORE);
    if(MMPM_STATUS_SUCCESS == sts)
        sts = TestSlpiCoresClockRequest(MMPM_CORE_ID_SLPI_SRAM, MMPM_CLK_ID_SLPI_SRAM_CORE);
    if(MMPM_STATUS_SUCCESS == sts)
        sts = TestSlpiCoresClockRequest(MMPM_CORE_ID_SLPI_CCD,MMPM_CLK_ID_SLPI_CCD_CORE);

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_CoreClock, SLPI_CORECLOCK_10_NAME, sts);
}

void Test_Slpi_CoreClock( void )
{
    Test_Slpi_CoreClock_01();
}

