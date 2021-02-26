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


#define MEMPOWER_01_NAME    "mempower_01"
#define MEMPOWER_01_DETAILS "Req-Rel, all memory/power state combination"

static MMPM_STATUS TestAllMemPowerStatesRequest(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    MmpmCoreIdType coreId = MMPM_CORE_ID_LPASS_ADSP;
    MmpmCoreInstanceIdType instanceId = MMPM_CORE_INSTANCE_0;
    MmpmMemIdType memId;
    MmpmMemPowerStateType powerState;
    uint32 clientId;

    clientId = Test_Register(coreId, instanceId, MEMPOWER_01_NAME);
    if(clientId)
    {
        for(memId = MMPM_MEM_OCMEM; memId < MMPM_MEM_MAX; memId++)
        {
            for(powerState = MMPM_MEM_POWER_OFF; powerState < MMPM_MEM_POWER_MAX; powerState++)
            {
                sts = Test_RequestMemoryPower(clientId, memId, powerState);
                if(MMPM_STATUS_SUCCESS != sts)
                {
                    retSts = MMPM_STATUS_FAILED;
                }
                else
                {
                    sts = Test_ReleaseResource(clientId, MMPM_RSC_ID_MEM_POWER);
                    if(MMPM_STATUS_SUCCESS != sts)
                    {
                        retSts = MMPM_STATUS_FAILED;
                    }
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
 * @ingroup MemPower
 * @test mempower_01
 * @brief Req-Rel, all memory/power state combination
 */
static MMPM_STATUS Test_MemoryPower_01(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(MEMPOWER_01_NAME, MEMPOWER_01_DETAILS);

    sts = TestAllMemPowerStatesRequest();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_MemoryPower, MEMPOWER_01_NAME, sts);
    return sts;
}


#define MEMPOWER_02_NAME    "mempower_02"
#define MEMPOWER_02_DETAILS "Req-Rel, 2 same memID/powerState clients, expect last Rel to disable Memory Power"

static AdsppmTestType testmempwr02[] =
{
    {1, 2, MMPM_CORE_ID_LPASS_DML, MMPM_CORE_INSTANCE_0,
        {REG, REQMEMPWR}
    },
    {2, 2, MMPM_CORE_ID_LPASS_DML, MMPM_CORE_INSTANCE_0,
        {REG, REQMEMPWR}
    },
    {1, 2, MMPM_CORE_ID_LPASS_DML, MMPM_CORE_INSTANCE_0,
        {RELMEMPWR, DREG}
    },
    {2, 2, MMPM_CORE_ID_LPASS_DML, MMPM_CORE_INSTANCE_0,
        {RELMEMPWR, DREG}
    }
};

/**
 * @ingroup MemPower
 * @test mempower_02
 * @brief Req-Rel, 2 same memID/powerState clients, expect last Rel to disable Memory Power
 */
static MMPM_STATUS Test_MemoryPower_02(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, testSts[MAX_TEST][MAX_TEST_SEQUENCE];
    uint32 numTest = 0;
    ADSPPMTEST_LOG_TEST_HEADER(MEMPOWER_02_NAME, MEMPOWER_02_DETAILS);

    numTest = sizeof(testmempwr02)/sizeof(AdsppmTestType);
    sts = Test_InvokeTest(testmempwr02, numTest, testSts);

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_MemoryPower, MEMPOWER_02_NAME, sts);
    return sts;
}


#define MEMPOWER_03_NAME    "mempower_03"
#define MEMPOWER_03_DETAILS "Req-Rel-Rel, expect NO errors"

static AdsppmTestType testmempwr03[] =
{
    {1, 4, MMPM_CORE_ID_LPASS_SRAM, MMPM_CORE_INSTANCE_0,
        {REG, REQMEMPWR, RELMEMPWR, RELMEMPWR, DREG}
    }
};

/**
 * @ingroup MemPower
 * @test mempower_03
 * @brief Req-Rel-Rel, expect NO errors
 */
static MMPM_STATUS Test_MemoryPower_03(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, testSts[MAX_TEST][MAX_TEST_SEQUENCE];
    uint32 numTest = 0;
    ADSPPMTEST_LOG_TEST_HEADER(MEMPOWER_03_NAME, MEMPOWER_03_DETAILS);

    numTest = sizeof(testmempwr03)/sizeof(AdsppmTestType);
    sts = Test_InvokeTest(testmempwr03, numTest, testSts);

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_MemoryPower, MEMPOWER_03_NAME, sts);
    return sts;
}


#define MEMPOWER_04_NAME    "mempower_04"
#define MEMPOWER_04_DETAILS "Invalid client ID Req, expect failure Req"

static MMPM_STATUS TestMemoryPowerInvalidClientId(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    uint32 clientId = 0;
    MmpmCoreIdType coreId = MMPM_CORE_ID_LPASS_ADSP;
    MmpmCoreInstanceIdType instanceId = MMPM_CORE_INSTANCE_0;

    clientId = Test_Register(coreId, instanceId, MEMPOWER_04_NAME);

    if(clientId)
    {
        sts = Test_Deregister(clientId);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            retSts = MMPM_STATUS_FAILED;
        }

        sts = Test_RequestMemoryPower(
            clientId, MMPM_MEM_LPASS_LPM, MMPM_MEM_POWER_ACTIVE);
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
 * @ingroup MemPower
 * @test mempower_04
 * @brief Invalid client ID Req, expect failure Req
 */
static MMPM_STATUS Test_MemoryPower_04(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(MEMPOWER_04_NAME, MEMPOWER_04_DETAILS);

    sts = TestMemoryPowerInvalidClientId();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_MemoryPower, MEMPOWER_04_NAME, sts);
    return sts;
}


#define MEMPOWER_05_NAME    "mempower_05"
#define MEMPOWER_05_DETAILS "Invalid parameters Req, expect failure Req"

static MMPM_STATUS TestMemoryPowerInvalidMemoryPowerParam(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    uint32 clientId = 0;
    MmpmCoreIdType coreId = MMPM_CORE_ID_LPASS_DML;
    MmpmCoreInstanceIdType instanceId = MMPM_CORE_INSTANCE_0;

    clientId = Test_Register(coreId, instanceId, MEMPOWER_05_NAME);

    if(clientId)
    {
        // Bad memory
        ADSPPMTEST_LOG_PRINTF_0(
            ADSPPMTEST_LOG_LEVEL_INFO,
            "[INFO] Test enum none memory");
        sts = Test_RequestMemoryPower(
            clientId, MMPM_MEM_NONE, MMPM_MEM_POWER_ACTIVE);
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

        // Bad memory
        ADSPPMTEST_LOG_PRINTF_0(
            ADSPPMTEST_LOG_LEVEL_INFO,
            "[INFO] Test enum max memory");
        sts = Test_RequestMemoryPower(
            clientId, MMPM_MEM_MAX, MMPM_MEM_POWER_ACTIVE);
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

        // Bad powerState
        ADSPPMTEST_LOG_PRINTF_0(
            ADSPPMTEST_LOG_LEVEL_INFO,
            "[INFO] Test enum none powerState");
        sts = Test_RequestMemoryPower(
            clientId, MMPM_MEM_LPASS_LPM, MMPM_MEM_POWER_NONE);
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

        // Bad powerState
        ADSPPMTEST_LOG_PRINTF_0(
            ADSPPMTEST_LOG_LEVEL_INFO,
            "[INFO] Test enum max powerState");
        sts = Test_RequestMemoryPower(
            clientId, MMPM_MEM_LPASS_LPM, MMPM_MEM_POWER_MAX);
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
 * @ingroup MemPower
 * @test mempower_05
 * @brief Invalid parameters Req, expect failure Req
 */
static MMPM_STATUS Test_MemoryPower_05(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(MEMPOWER_05_NAME, MEMPOWER_05_DETAILS);

    sts = TestMemoryPowerInvalidMemoryPowerParam();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_MemoryPower, MEMPOWER_05_NAME, sts);
    return sts;
}


void Test_MemoryPower(void)
{
    // Req-Rel, all memory/power state combination
    Test_MemoryPower_01();

    // Req-Rel, 2 same memID/powerState clients, expect last Rel to disable Memory Power
    Test_MemoryPower_02();

    // Req-Rel-Rel, expect NO errors
    Test_MemoryPower_03();

    // Invalid client ID Req, expect failure Req
    Test_MemoryPower_04();

    // Invalid parameters Req, expect failure Req
    Test_MemoryPower_05();
}
