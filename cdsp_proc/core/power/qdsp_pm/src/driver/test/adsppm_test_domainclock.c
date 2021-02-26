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


#define DOMAINCLOCK_01_NAME    "domainclock_01"
#define DOMAINCLOCK_01_DETAILS "Req-Rel, all core/instance/domain clock combination"

static MMPM_STATUS TestAllCoresCoreClockDomainRequest(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    uint32 clientId = 0;
    MmpmCoreIdType coreId;
    MmpmCoreInstanceIdType instanceId;
    MmpmClkIdLpassType clkId;
    MmpmClkDomainSrcIdLpassType clkDomainSrcId;
    ClkDomainTestType clkReq;

    for(coreId = MMPM_CORE_ID_LPASS_ADSP; coreId < MMPM_CORE_ID_LPASS_END; coreId++)
    {
        for(instanceId = MMPM_CORE_INSTANCE_0; instanceId < MMPM_CORE_INSTANCE_MAX; instanceId++)
        {
            if(AsicInfo_isCoreInstanceAvailable(coreId, instanceId))
            {
                clientId = Test_Register(coreId, instanceId, DOMAINCLOCK_01_NAME);
                if(clientId)
                {
                    for(clkId = MMPM_CLK_ID_LPASS_HWRSP_CORE; clkId < MMPM_CLK_ID_LPASS_ADSP_CORE; clkId++)
                    {
                        if(AsicInfo_isCoreClockAvailable(clkId))
                        {
                            for(clkDomainSrcId = MMPM_CLK_DOMAIN_SRC_ID_Q6PLL; MMPM_CLK_DOMAIN_SRC_ID_LPASS_MAX > clkDomainSrcId; clkDomainSrcId++)
                            {
                                clkReq.clkId = clkId;
                                clkReq.clkFreqHz = 200000000;
                                clkReq.clkDomainSrc = clkDomainSrcId;

                                sts = Test_RequestCoreClockDomain(clientId, &clkReq, 1);
                                if(MMPM_STATUS_SUCCESS != sts)
                                {
                                    retSts = MMPM_STATUS_FAILED;
                                }

                                sts = Test_ReleaseResource(clientId, MMPM_RSC_ID_CORE_CLK_DOMAIN);
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
            }
        }
    }

    return retSts;
}

/**
 * @ingroupDomainClock
 * @test domainclock_01
 * @brief Req-Rel, all core/instance/domain clock combination
 */
static MMPM_STATUS Test_DomainClock_01(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(DOMAINCLOCK_01_NAME, DOMAINCLOCK_01_DETAILS);

    sts = TestAllCoresCoreClockDomainRequest();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_DomainClock, DOMAINCLOCK_01_NAME, sts);
    return sts;
}


#define DOMAINCLOCK_02_NAME    "domainclock_02"
#define DOMAINCLOCK_02_DETAILS "Req-Rel, 2 same coreID/instanceID clients"

static AdsppmTestType testcoreclockdomain02[] =
{
    {1, 2, MMPM_CORE_ID_LPASS_AVSYNC, MMPM_CORE_INSTANCE_0,
        {REG, REQCLKDOMAIN}
    },
    {2, 2, MMPM_CORE_ID_LPASS_AVSYNC, MMPM_CORE_INSTANCE_0,
        {REG, REQCLKDOMAIN}
    },
    {1, 2, MMPM_CORE_ID_LPASS_AVSYNC, MMPM_CORE_INSTANCE_0,
        {RELCLKDOMAIN, DREG}
    },
    {2, 2, MMPM_CORE_ID_LPASS_AVSYNC, MMPM_CORE_INSTANCE_0,
        {RELCLKDOMAIN, DREG}
    }
};

/**
 * @ingroupDomainClock
 * @test domainclock_02
 * @brief Req-Rel, 2 same coreID/instanceID clients
 */
static MMPM_STATUS Test_DomainClock_02(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, testSts[MAX_TEST][MAX_TEST_SEQUENCE];
    uint32 numTest = 0;
    ADSPPMTEST_LOG_TEST_HEADER(DOMAINCLOCK_02_NAME, DOMAINCLOCK_02_DETAILS);

    numTest = sizeof(testcoreclockdomain02)/sizeof(AdsppmTestType);
    sts = Test_InvokeTest(testcoreclockdomain02, numTest, testSts);

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_DomainClock, DOMAINCLOCK_02_NAME, sts);
    return sts;
}


#define DOMAINCLOCK_03_NAME    "domainclock_03"
#define DOMAINCLOCK_03_DETAILS "Req-Rel-Rel, expect NO errors"

static AdsppmTestType testcoreclockdomain03[] =
{
    {1, 5, MMPM_CORE_ID_LPASS_AVSYNC, MMPM_CORE_INSTANCE_0,
        {REG, REQCLKDOMAIN, RELCLKDOMAIN, RELCLKDOMAIN, DREG}
    }
};

/**
 * @ingroupDomainClock
 * @test domainclock_03
 * @brief Req-Rel-Rel, expect NO errors
 */
static MMPM_STATUS Test_DomainClock_03(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, testSts[MAX_TEST][MAX_TEST_SEQUENCE];
    uint32 numTest = 0;
    ADSPPMTEST_LOG_TEST_HEADER(DOMAINCLOCK_03_NAME, DOMAINCLOCK_03_DETAILS);

    numTest = sizeof(testcoreclockdomain03)/sizeof(AdsppmTestType);
    sts = Test_InvokeTest(testcoreclockdomain03, numTest, testSts);

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_DomainClock, DOMAINCLOCK_03_NAME, sts);
    return sts;
}


#define DOMAINCLOCK_04_NAME    "domainclock_04"
#define DOMAINCLOCK_04_DETAILS "Invalid client ID Req, expect failure Req"

static MMPM_STATUS TestCoreClockDomainInvalidClientId(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    uint32 clientId = 0;
    MmpmCoreIdType coreId = MMPM_CORE_ID_LPASS_ADSP;
    MmpmCoreInstanceIdType instanceId = MMPM_CORE_INSTANCE_0;
    ClkDomainTestType clkDom;
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
        clientId = Test_Register(coreId, instanceId, DOMAINCLOCK_04_NAME);

        if(clientId)
        {
            clkDom.clkId = clkId;
            clkDom.clkFreqHz = 100000000;
            clkDom.clkDomainSrc = MMPM_CLK_DOMAIN_SRC_ID_Q6PLL;

            sts = Test_Deregister(clientId);
            if(MMPM_STATUS_SUCCESS != sts)
            {
                retSts = MMPM_STATUS_FAILED;
            }

            sts = Test_RequestCoreClockDomain(clientId, &clkDom, 1);
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
 * @ingroupDomainClock
 * @test domainclock_04
 * @brief Invalid client ID Req, expect failure Req
 */
static MMPM_STATUS Test_DomainClock_04(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(DOMAINCLOCK_04_NAME, DOMAINCLOCK_04_DETAILS);

    sts = TestCoreClockDomainInvalidClientId();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_DomainClock, DOMAINCLOCK_04_NAME, sts);
    return sts;
}


#define DOMAINCLOCK_05_NAME    "domainclock_05"
#define DOMAINCLOCK_05_DETAILS "Invalid parameters Req, expect failure Req"

static MMPM_STATUS TestCoreClockDomainInvalidCoreClockDomainParam(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    uint32 clientId = 0;
    MmpmCoreIdType coreId = MMPM_CORE_ID_LPASS_AVSYNC;
    MmpmCoreInstanceIdType instanceId = MMPM_CORE_INSTANCE_0;
    ClkDomainTestType clkDom;
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
        clientId = Test_Register(coreId, instanceId, DOMAINCLOCK_03_NAME);
        clkDom.clkId = clkId;
        clkDom.clkFreqHz = 100000000;
        clkDom.clkDomainSrc = MMPM_CLK_DOMAIN_SRC_ID_Q6PLL;

        if(clientId)
        {
            // Bad clkId
            ADSPPMTEST_LOG_PRINTF_0(
                ADSPPMTEST_LOG_LEVEL_INFO,
                "[INFO] Test enum none clkId");
            clkDom.clkId = MMPM_CLK_ID_LPASS_NONE;
            sts = Test_RequestCoreClockDomain(clientId, &clkDom, 1);
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
            clkDom.clkId = clkId;

            // Bad clkId
            ADSPPMTEST_LOG_PRINTF_0(
                ADSPPMTEST_LOG_LEVEL_INFO,
                "[INFO] Test enum max clkId");
            clkDom.clkId = MMPM_CLK_ID_LPASS_ENUM_MAX;
            sts = Test_RequestCoreClockDomain(clientId, &clkDom, 1);
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
            clkDom.clkId = clkId;

            // Bad clkDomainSrc
            ADSPPMTEST_LOG_PRINTF_0(
                ADSPPMTEST_LOG_LEVEL_INFO,
                "[INFO] Test enum none clkDomainSrc");
            clkDom.clkDomainSrc = MMPM_CLK_DOMAIN_SRC_ID_LPASS_NONE;
            sts = Test_RequestCoreClockDomain(clientId, &clkDom, 1);
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
            clkDom.clkDomainSrc = MMPM_CLK_DOMAIN_SRC_ID_Q6PLL;

            // Bad clkDomainSrc
            ADSPPMTEST_LOG_PRINTF_0(
                ADSPPMTEST_LOG_LEVEL_INFO,
                "[INFO] Test enum max clkDomainSrc");
            clkDom.clkDomainSrc = MMPM_CLK_DOMAIN_SRC_ID_LPASS_MAX;
            sts = Test_RequestCoreClockDomain(clientId, &clkDom, 1);
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
            clkDom.clkDomainSrc = MMPM_CLK_DOMAIN_SRC_ID_Q6PLL;

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
 * @ingroupDomainClock
 * @test domainclock_05
 * @brief Invalid parameters Req, expect failure Req
 */
static MMPM_STATUS Test_DomainClock_05(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(DOMAINCLOCK_05_NAME, DOMAINCLOCK_05_DETAILS);

    sts = TestCoreClockDomainInvalidCoreClockDomainParam();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_DomainClock, DOMAINCLOCK_05_NAME, sts);
    return sts;
}


void Test_DomainClock()
{
    // Req-Rel, all core/instance/domain clock combination
    Test_DomainClock_01();

    // Req-Rel, 2 same coreID/instanceID clients
    Test_DomainClock_02();

    // Req-Rel-Rel, expect NO errors
    Test_DomainClock_03();

    // Invalid client ID Req, expect failure Req
    //Test_DomainClock_04();

    // Invalid parameters Req, expect failure Req
    //Test_DomainClock_05();
}
