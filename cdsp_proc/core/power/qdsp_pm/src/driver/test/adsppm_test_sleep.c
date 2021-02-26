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


#define SLEEP_01_NAME    "sleep_01"
#define SLEEP_01_DETAILS "Req-Rel, all core/instance combination"

static MMPM_STATUS TestAllCoresSleeLatencypRequest(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    MmpmCoreIdType coreId;
    MmpmCoreInstanceIdType instanceId;
    uint32 clientId;

    for(coreId = MMPM_CORE_ID_LPASS_ADSP; coreId < MMPM_CORE_ID_LPASS_END; coreId++)
    {
        for(instanceId = MMPM_CORE_INSTANCE_0; instanceId < MMPM_CORE_INSTANCE_MAX; instanceId++)
        {
            if(AsicInfo_isCoreInstanceAvailable(coreId, instanceId))
            {
                clientId = Test_Register(coreId, instanceId, SLEEP_01_NAME);
                if(clientId)
                {
                    sts = Test_RequestSleepLatency(clientId, 5);
                    if(MMPM_STATUS_SUCCESS != sts)
                    {
                        retSts = MMPM_STATUS_FAILED;
                    }
                    else
                    {
                        sts = Test_ReleaseResource(clientId, MMPM_RSC_ID_SLEEP_LATENCY);
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
            }
        }
    }

    return retSts;
}

/**
 * @ingroup Sleep
 * @test sleep_01
 * @brief Req-Rel, all core/instance combination
 */
static MMPM_STATUS Test_SleepLatency_01(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(SLEEP_01_NAME, SLEEP_01_DETAILS);

    sts = TestAllCoresSleeLatencypRequest();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_Sleep, SLEEP_01_NAME, sts);
    return sts;
}


#define SLEEP_02_NAME    "sleep_02"
#define SLEEP_02_DETAILS "Req-Rel, 2 same coreID/instanceID clients"

static AdsppmTestType testsleeplatency02[] =
{
    {1, 2, MMPM_CORE_ID_LPASS_ADSP, MMPM_CORE_INSTANCE_0,
        {REG, REQSLATENCY}
    },
    {2, 2, MMPM_CORE_ID_LPASS_ADSP, MMPM_CORE_INSTANCE_0,
        {REG, REQSLATENCY}
    },
    {1, 2, MMPM_CORE_ID_LPASS_ADSP, MMPM_CORE_INSTANCE_0,
        {RELSPATENCY, DREG}
    },
    {2, 2, MMPM_CORE_ID_LPASS_ADSP, MMPM_CORE_INSTANCE_0,
        {RELSPATENCY, DREG}
    }
};

/**
 * @ingroup Sleep
 * @test sleep_02
 * @brief Req-Rel, 2 same coreID/instanceID clients
 */
static MMPM_STATUS Test_SleepLatency_02(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, testSts[MAX_TEST][MAX_TEST_SEQUENCE];
    uint32 numTest = 0;
    ADSPPMTEST_LOG_TEST_HEADER(SLEEP_02_NAME, SLEEP_02_DETAILS);

    numTest = sizeof(testsleeplatency02)/sizeof(AdsppmTestType);
    sts = Test_InvokeTest(testsleeplatency02, numTest, testSts);

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_Sleep, SLEEP_02_NAME, sts);
    return sts;
}


#define SLEEP_03_NAME    "sleep_03"
#define SLEEP_03_DETAILS "Req-Rel-Rel, expect NO errors"

static AdsppmTestType testsleeplatency03[] =
{
    {1, 5, MMPM_CORE_ID_LPASS_ADSP, MMPM_CORE_INSTANCE_0,
        {REG, REQSLATENCY, RELSPATENCY, RELSPATENCY, DREG}
    }
};

/**
 * @ingroup Sleep
 * @test sleep_03
 * @brief Req-Rel-Rel,expect NO errors
 */
static MMPM_STATUS Test_SleepLatency_03(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, testSts[MAX_TEST][MAX_TEST_SEQUENCE];
    uint32 numTest = 0;
    ADSPPMTEST_LOG_TEST_HEADER(SLEEP_03_NAME, SLEEP_03_DETAILS);

    numTest = sizeof(testsleeplatency03)/sizeof(AdsppmTestType);
    sts = Test_InvokeTest(testsleeplatency03, numTest, testSts);

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_Sleep, SLEEP_03_NAME, sts);
    return sts;
}


#define SLEEP_04_NAME    "sleep_04"
#define SLEEP_04_DETAILS "Invalid client ID Req, expect failure Req"

static MMPM_STATUS TestSleepInvalidClientId(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    uint32 clientId = 0;
    MmpmCoreIdType coreId = MMPM_CORE_ID_LPASS_ADSP;
    MmpmCoreInstanceIdType instanceId = MMPM_CORE_INSTANCE_0;

    clientId = Test_Register(coreId, instanceId, SLEEP_04_NAME);

    if(clientId)
    {
        sts = Test_Deregister(clientId);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            retSts = MMPM_STATUS_FAILED;
        }

        sts = Test_RequestSleepLatency(clientId, 5);
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

    return retSts;
}

/**
 * @ingroup Sleep
 * @test sleep_04
 * @brief Invalid client ID Req, expect failure Req
 */
static MMPM_STATUS Test_SleepLatency_04(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(SLEEP_04_NAME, SLEEP_04_DETAILS);

    sts = TestSleepInvalidClientId();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_Sleep, SLEEP_04_NAME, sts);
    return sts;
}


#define SLEEP_05_NAME    "sleep_05"
#define SLEEP_05_DETAILS "Zero latency Req, expect failure Req"

static MMPM_STATUS TestSleepZeroLatency(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    uint32 clientId = 0;
    MmpmCoreIdType coreId = MMPM_CORE_ID_LPASS_ADSP;
    MmpmCoreInstanceIdType instanceId = MMPM_CORE_INSTANCE_0;

    clientId = Test_Register(coreId, instanceId, SLEEP_05_NAME);

    if(clientId)
    {
        sts = Test_RequestSleepLatency(clientId, 0);
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
 * @ingroup Sleep
 * @test sleep_05
 * @brief Invalid client ID Req, expect failure Req
 */
static MMPM_STATUS Test_SleepLatency_05(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(SLEEP_05_NAME, SLEEP_05_DETAILS);

    sts = TestSleepZeroLatency();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_Sleep, SLEEP_05_NAME, sts);
    return sts;
}


void Test_SleepLatency()
{
    // Req-Rel, all core/instance combination
    Test_SleepLatency_01();

    // Req-Rel, 2 same coreID/instanceID clients
    Test_SleepLatency_02();

    // Req-Rel-Rel,expect NO errors
    Test_SleepLatency_03();

    // Invalid client ID Req, expect failure Req
    Test_SleepLatency_04();

    // Invalid client ID Req, expect failure Req
    Test_SleepLatency_05();
}
