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
#include "qurt_qdi_constants.h"
#include "qurt_qdi_driver.h"
#include "qurt_qdi.h"
#include "adsppm_qdi.h"


#define STRESS_TEST_LOOP 30
#define MAX_MMPM_CLIENTS 64

extern const MmpmReqClockDomainTestParamTableType gReqClkDomainParam[];


#define STRESS_01_NAME    "stress_01"
#define STRESS_01_DETAILS "Multiple Reg->Multiple Dreg, twice (2 loops)"

static MMPM_STATUS TestStressRegDreg(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    uint32 loop, index, clientId[STRESS_TEST_LOOP];
    MmpmCoreIdType coreId = MMPM_CORE_ID_LPASS_ADSP;
    MmpmCoreInstanceIdType instanceId = MMPM_CORE_INSTANCE_0;

    for(loop = 0; loop < 2; loop++)
    {
        ADSPPMTEST_LOG_PRINTF_1(
            ADSPPMTEST_LOG_LEVEL_INFO,
            "[INFO] Loop %u", loop);

        // Test MMPM_Register_Ext
        for(index = 0; index < STRESS_TEST_LOOP; index++)
        {
            clientId[index] = 0;
            clientId[index] = Test_Register(coreId, instanceId, STRESS_01_NAME);
            if(!clientId[index])
            {
                ADSPPMTEST_LOG_PRINTF_2(
                    ADSPPMTEST_LOG_LEVEL_ERROR,
                    "[ERROR] Register failed at loop %u:%u", loop, index);
                retSts = MMPM_STATUS_FAILED;
            }
        }

        // Test MMPM_Deregister_Ext
        for(index = 0; index < STRESS_TEST_LOOP; index++)
        {
            if(clientId[index])
            {
                sts = Test_Deregister(clientId[index]);
                if(MMPM_STATUS_SUCCESS != sts)
                {
                    ADSPPMTEST_LOG_PRINTF_2(
                        ADSPPMTEST_LOG_LEVEL_ERROR,
                        "[ERROR] Deregister failed at loop %u:%u", loop, index);
                    retSts = MMPM_STATUS_FAILED;
                }
            }
        }
    }

    return retSts;
}

/**
 * @ingroup Stress
 * @test stress_01
 * @brief Multiple Reg->Multiple Dreg, twice (2 loops)
 */
static MMPM_STATUS Test_Stress_01_System(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(STRESS_01_NAME, STRESS_01_DETAILS);

    sts = TestStressRegDreg();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_Stress, STRESS_01_NAME, sts);
    return sts;
}


#define STRESS_02_NAME    "stress_02"
#define STRESS_02_DETAILS "Core Clock Req-Rel, 2 clients, same core clock ID, different frequency, multiple times"

static MMPM_STATUS TestStressMultiClientMultiClockReqRel(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    uint32 clientId1 = 0, clientId2 = 0, loop;
    uint32 clkFreq1, clkFreq2, clkFreq;
    const AsicInfo_ClockFreqVoltPlanType *pClockFreqVoltPlan = NULL;
    MmpmCoreIdType coreId = MMPM_CORE_ID_LPASS_HWRSMP;
    MmpmCoreInstanceIdType instanceId = MMPM_CORE_INSTANCE_0;
    ClkTestType clk;

    clientId1 = Test_Register(coreId, instanceId, STRESS_02_NAME"_1");
    clientId2 = Test_Register(coreId, instanceId, STRESS_02_NAME"_2");

    if(clientId1 && clientId2)
    {
        pClockFreqVoltPlan = AsicInfo_getClockFreqVoltPlan(MMPM_CLK_ID_LPASS_HWRSP_CORE);

        clk.clkId = MMPM_CLK_ID_LPASS_HWRSP_CORE;
        clk.freqMatch = MMPM_FREQ_AT_LEAST;
        clkFreq1 = pClockFreqVoltPlan->pFreqVoltPlan[0].frequency;
        clkFreq2 = pClockFreqVoltPlan->pFreqVoltPlan[1].frequency;

        for(loop = 0; (loop < STRESS_TEST_LOOP) && (MMPM_STATUS_SUCCESS == retSts); loop++)
        {
            clk.freq = clkFreq1;
            sts = Test_RequestCoreClock(clientId1, &clk, 1);
            if(MMPM_STATUS_SUCCESS != sts)
            {
                ADSPPMTEST_LOG_PRINTF_1(
                    ADSPPMTEST_LOG_LEVEL_ERROR,
                    "[ERROR] clientID 1 Request, loop %u", loop);
                retSts = MMPM_STATUS_FAILED;
            }
            else
            {
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
                            "[ERROR] Lower clock setting @ Req: setClkFreq = %u Hz, readClkFreq = %u Hz",
                            clkFreq1, clkFreq);
                        retSts = MMPM_STATUS_FAILED;
                    }
                }
            }

            clk.freq = clkFreq2;
            sts = Test_RequestCoreClock(clientId2, &clk, 1);
            if(MMPM_STATUS_SUCCESS != sts)
            {
                ADSPPMTEST_LOG_PRINTF_1(
                    ADSPPMTEST_LOG_LEVEL_ERROR,
                    "[ERROR] clientID 2 Request, loop %u", loop);
                retSts = MMPM_STATUS_FAILED;
            }
            else
            {
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
                            "[ERROR] Higher clock setting @ Rel: setClkFreq = %u Hz, readClkFreq = %u Hz",
                            clkFreq2, clkFreq);
                        retSts = MMPM_STATUS_FAILED;
                    }
                }
            }

            sts = Test_ReleaseResource(clientId2, MMPM_RSC_ID_CORE_CLK);
            if(MMPM_STATUS_SUCCESS != sts)
            {
                ADSPPMTEST_LOG_PRINTF_1(
                    ADSPPMTEST_LOG_LEVEL_ERROR,
                    "[ERROR] clientID 2 Release, loop %u", loop);
                retSts = MMPM_STATUS_FAILED;
            }
            else
            {
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
            }

            sts = Test_ReleaseResource(clientId1, MMPM_RSC_ID_CORE_CLK);
            if(MMPM_STATUS_SUCCESS != sts)
            {
                ADSPPMTEST_LOG_PRINTF_1(
                    ADSPPMTEST_LOG_LEVEL_ERROR,
                    "[ERROR] clientID 1 Release, loop %u", loop);
                retSts = MMPM_STATUS_FAILED;
            }
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
 * @ingroup Stress
 * @test stress_02
 * @brief Core Clock Req-Rel, 2 clients, same core clock ID, different frequency, multiple times
 */
static MMPM_STATUS Test_Stress_02_CoreClock(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(STRESS_02_NAME, STRESS_02_DETAILS);

    sts = TestStressMultiClientMultiClockReqRel();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_Stress, STRESS_02_NAME, sts);
    return sts;
}


#define STRESS_03_NAME    "stress_03"
#define STRESS_03_DETAILS "Core Clock Req-Rel, single client, multiple times"

static MMPM_STATUS TestStressSingleClientMultiClockReqRel(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    uint32 clientId = 0, loop, clkFreq;
    const AsicInfo_ClockFreqVoltPlanType *pClockFreqVoltPlan = NULL;
    MmpmCoreIdType coreId = MMPM_CORE_ID_LPASS_SLIMBUS;
    MmpmCoreInstanceIdType instanceId = MMPM_CORE_INSTANCE_0;
    ClkTestType clk;

    clientId = Test_Register(coreId, instanceId, STRESS_03_NAME);

    if(clientId)
    {
        pClockFreqVoltPlan = AsicInfo_getClockFreqVoltPlan(MMPM_CLK_ID_LPASS_SLIMBUS_CORE);

        clk.clkId = MMPM_CLK_ID_LPASS_SLIMBUS_CORE;
        clk.freq = pClockFreqVoltPlan->pFreqVoltPlan[0].frequency;
        clk.freqMatch = MMPM_FREQ_AT_LEAST;

        for(loop = 0; (loop < STRESS_TEST_LOOP) && (MMPM_STATUS_SUCCESS == retSts); loop++)
        {
            sts = Test_RequestCoreClock(clientId, &clk, 1);
            if(MMPM_STATUS_SUCCESS != sts)
            {
                ADSPPMTEST_LOG_PRINTF_1(
                    ADSPPMTEST_LOG_LEVEL_ERROR,
                    "[ERROR] Request, loop %u", loop);
                retSts = MMPM_STATUS_FAILED;
            }
            else
            {
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
                            "[ERROR] Lower clock setting: setClkFreq = %u Hz, readClkFreq = %u Hz",
                            clk.freq, clkFreq);
                        retSts = MMPM_STATUS_FAILED;
                    }
                }
            }

            sts = Test_ReleaseResource(clientId, MMPM_RSC_ID_CORE_CLK);
            if(MMPM_STATUS_SUCCESS != sts)
            {
                ADSPPMTEST_LOG_PRINTF_1(
                    ADSPPMTEST_LOG_LEVEL_ERROR,
                    "[ERROR] Release, loop %u", loop);
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

/**
 * @ingroup Stress
 * @test stress_03
 * @brief Core Clock Req-Rel, single client, multiple times
 */
static MMPM_STATUS Test_Stress_03_CoreClock(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(STRESS_03_NAME, STRESS_03_DETAILS);

    sts = TestStressSingleClientMultiClockReqRel();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_Stress, STRESS_03_NAME, sts);
    return sts;
}


#define STRESS_04_NAME    "stress_04"
#define STRESS_04_DETAILS "Core Clock Domain Req-Rel, 2 clients, same core clock ID, different sources, multiple times"

static MMPM_STATUS TestStressMultiClientMultiClockDomainReqRel(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    uint32 index, clientId1 = 0, clientId2 = 0, loop;
    MmpmCoreIdType coreId;
    MmpmCoreInstanceIdType instanceId = MMPM_CORE_INSTANCE_0;
    ClkDomainTestType clkDomain;

    // Get a coreId that will yield a valid clk domain req
    for(coreId = MMPM_CORE_ID_LPASS_ADSP; MMPM_CORE_ID_LPASS_END > coreId; coreId++)
    {
        if(AsicInfo_isCoreInstanceAvailable(coreId, instanceId))
        {
            index = coreId - MMPM_CORE_ID_LPASS_START;
            if(gReqClkDomainParam[index].numClks)
            {
                if(AsicInfo_isCoreClockAvailable(gReqClkDomainParam[index].clk[0].clkId))
                {
                    break;
                }
            }
        }
    }

    if(MMPM_CORE_ID_LPASS_END > coreId)
    {
        clientId1 = Test_Register(coreId, instanceId, STRESS_04_NAME"_1");
        clientId2 = Test_Register(coreId, instanceId, STRESS_04_NAME"_2");

        if(clientId1 && clientId2)
        {
            clkDomain.clkId = gReqClkDomainParam[index].clk[0].clkId;
            clkDomain.clkFreqHz = 200000000;

            for(loop = 0; (loop < STRESS_TEST_LOOP) && (MMPM_STATUS_SUCCESS == retSts); loop++)
            {
                clkDomain.clkDomainSrc = MMPM_CLK_DOMAIN_SRC_ID_AUDIOPLL;
                sts = Test_RequestCoreClockDomain(clientId1, &clkDomain, 1);
                if(MMPM_STATUS_SUCCESS != sts)
                {
                    ADSPPMTEST_LOG_PRINTF_1(
                        ADSPPMTEST_LOG_LEVEL_ERROR,
                        "[ERROR] clientID 1 Request, loop %u", loop);
                    retSts = MMPM_STATUS_FAILED;
                }

                //TODO: read/compare clock

                clkDomain.clkDomainSrc = MMPM_CLK_DOMAIN_SRC_ID_PRIUSPLL;
                sts = Test_RequestCoreClockDomain(clientId2, &clkDomain, 1);
                if(MMPM_STATUS_SUCCESS != sts)
                {
                    ADSPPMTEST_LOG_PRINTF_1(
                        ADSPPMTEST_LOG_LEVEL_ERROR,
                        "[ERROR] clientID 2 Request, loop %u", loop);
                    retSts = MMPM_STATUS_FAILED;
                }

                //TODO: read/compare clock

                sts = Test_ReleaseResource(clientId2, MMPM_RSC_ID_CORE_CLK_DOMAIN);
                if(MMPM_STATUS_SUCCESS != sts)
                {
                    ADSPPMTEST_LOG_PRINTF_1(
                        ADSPPMTEST_LOG_LEVEL_ERROR,
                        "[ERROR] clientID 2 Release, loop %u", loop);
                    retSts = MMPM_STATUS_FAILED;
                }

                //TODO: read/compare clock

                sts = Test_ReleaseResource(clientId1, MMPM_RSC_ID_CORE_CLK_DOMAIN);
                if(MMPM_STATUS_SUCCESS != sts)
                {
                    ADSPPMTEST_LOG_PRINTF_1(
                        ADSPPMTEST_LOG_LEVEL_ERROR,
                        "[ERROR] clientID 1 Release, loop %u", loop);
                    retSts = MMPM_STATUS_FAILED;
                }

                //TODO: read/compare clock
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
    }
    else
    {
        ADSPPMTEST_LOG_PRINTF_0(
            ADSPPMTEST_LOG_LEVEL_ERROR,
            "[ERROR] No clkID with at least 2 freq in freq plan available");
        retSts = MMPM_STATUS_FAILED;
    }

    return retSts;
}

/**
 * @ingroup Stress
 * @test stress_04
 * @brief Core Clock Domain Req-Rel, 2 clients, same core clock ID, different sources, multiple times
 */
static MMPM_STATUS Test_Stress_04_CoreClockDomain(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(STRESS_04_NAME, STRESS_04_DETAILS);

    sts = TestStressMultiClientMultiClockDomainReqRel();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_Stress, STRESS_04_NAME, sts);
    return sts;
}


#define STRESS_05_NAME    "stress_05"
#define STRESS_05_DETAILS "Core Clock Domain Req-Rel, single client, multiple times"

static MMPM_STATUS TestStressSingleClientMultiClockDomainReqRel(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    uint32 index, clientId = 0, loop;
    MmpmCoreIdType coreId;
    MmpmCoreInstanceIdType instanceId = MMPM_CORE_INSTANCE_0;
    ClkDomainTestType clkDomain;

    // Get a coreId that will yield a valid clk domain req
    for(coreId = MMPM_CORE_ID_LPASS_ADSP; MMPM_CORE_ID_LPASS_END > coreId; coreId++)
    {
        if(AsicInfo_isCoreInstanceAvailable(coreId, instanceId))
        {
            index = coreId - MMPM_CORE_ID_LPASS_START;
            if(gReqClkDomainParam[index].numClks)
            {
                if(AsicInfo_isCoreClockAvailable(gReqClkDomainParam[index].clk[0].clkId))
                {
                    break;
                }
            }
        }
    }

    if(MMPM_CORE_ID_LPASS_END > coreId)
    {
        clientId = Test_Register(coreId, instanceId, STRESS_05_NAME);

        if(clientId)
        {
            clkDomain.clkId = gReqClkDomainParam[index].clk[0].clkId;
            clkDomain.clkFreqHz = 200000000;
            clkDomain.clkDomainSrc = MMPM_CLK_DOMAIN_SRC_ID_PRIUSPLL;

            for(loop = 0; (loop < STRESS_TEST_LOOP) && (MMPM_STATUS_SUCCESS == retSts); loop++)
            {
                sts = Test_RequestCoreClockDomain(clientId, &clkDomain, 1);
                if(MMPM_STATUS_SUCCESS != sts)
                {
                    ADSPPMTEST_LOG_PRINTF_1(
                        ADSPPMTEST_LOG_LEVEL_ERROR,
                        "[ERROR] Request, loop %u", loop);
                    retSts = MMPM_STATUS_FAILED;
                }

                //TODO: read/compare clock

                sts = Test_ReleaseResource(clientId, MMPM_RSC_ID_CORE_CLK_DOMAIN);
                if(MMPM_STATUS_SUCCESS != sts)
                {
                    ADSPPMTEST_LOG_PRINTF_1(
                        ADSPPMTEST_LOG_LEVEL_ERROR,
                        "[ERROR] Release, loop %u", loop);
                    retSts = MMPM_STATUS_FAILED;
                }

                //TODO: read/compare clock
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
    else
    {
        ADSPPMTEST_LOG_PRINTF_0(
            ADSPPMTEST_LOG_LEVEL_ERROR,
            "[ERROR] No clkID available");
        retSts = MMPM_STATUS_FAILED;
    }

    return retSts;
}

/**
 * @ingroup Stress
 * @test stress_05
 * @brief Core Clock Domain Req-Rel, single client, multiple times
 */
static MMPM_STATUS Test_Stress_05_CoreClockDomain(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(STRESS_05_NAME, STRESS_05_DETAILS);

    sts = TestStressSingleClientMultiClockDomainReqRel();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_Stress, STRESS_05_NAME, sts);
    return sts;
}


#define STRESS_06_NAME    "stress_06"
#define STRESS_06_DETAILS "MIPS Req-Rel, 2 clients, same coreID/instanceID/MIPS, multiple times"

static MMPM_STATUS TestStressMultiClientMultiMipsReqRel(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    uint32 clientId1 = 0, clientId2 = 0, loop;
    MmpmCoreIdType coreId = MMPM_CORE_ID_LPASS_ADSP;
    MmpmCoreInstanceIdType instanceId = MMPM_CORE_INSTANCE_0;

    clientId1 = Test_Register(coreId, instanceId, STRESS_06_NAME"_1");
    clientId2 = Test_Register(coreId, instanceId, STRESS_06_NAME"_2");

    if(clientId1 && clientId2)
    {
        for(loop = 0; (loop < STRESS_TEST_LOOP) && (MMPM_STATUS_SUCCESS == retSts); loop++)
        {
            sts = Test_RequestMips(clientId1, 200);
            if(MMPM_STATUS_SUCCESS != sts)
            {
                ADSPPMTEST_LOG_PRINTF_1(
                    ADSPPMTEST_LOG_LEVEL_ERROR,
                    "[ERROR] clientID 1 Request, loop %u", loop);
                retSts = MMPM_STATUS_FAILED;
            }

            sts = Test_RequestMips(clientId2, 200);
            if(MMPM_STATUS_SUCCESS != sts)
            {
                ADSPPMTEST_LOG_PRINTF_1(
                    ADSPPMTEST_LOG_LEVEL_ERROR,
                    "[ERROR] clientID 2 Request, loop %u", loop);
                retSts = MMPM_STATUS_FAILED;
            }

            sts = Test_ReleaseResource(clientId2, MMPM_RSC_ID_MIPS_EXT);
            if(MMPM_STATUS_SUCCESS != sts)
            {
                ADSPPMTEST_LOG_PRINTF_1(
                    ADSPPMTEST_LOG_LEVEL_ERROR,
                    "[ERROR] clientID 2 Release, loop %u", loop);
                retSts = MMPM_STATUS_FAILED;
            }

            sts = Test_ReleaseResource(clientId1, MMPM_RSC_ID_MIPS_EXT);
            if(MMPM_STATUS_SUCCESS != sts)
            {
                ADSPPMTEST_LOG_PRINTF_1(
                    ADSPPMTEST_LOG_LEVEL_ERROR,
                    "[ERROR] clientID 1 Release, loop %u", loop);
                retSts = MMPM_STATUS_FAILED;
            }
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
 * @ingroup Stress
 * @test stress_06
 * @brief MIPS Req-Rel, 2 clients, same coreID/instanceID/MIPS, multiple times
 */
static MMPM_STATUS Test_Stress_06_Mips(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(STRESS_06_NAME, STRESS_06_DETAILS);

    sts = TestStressMultiClientMultiMipsReqRel();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_Stress, STRESS_06_NAME, sts);
    return sts;
}


#define STRESS_07_NAME    "stress_07"
#define STRESS_07_DETAILS "MIPS Req-Rel, single client, multiple times"

static MMPM_STATUS TestStressSingleClientMultiMipsReqRel(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    uint32 clientId = 0, loop;
    MmpmCoreIdType coreId = MMPM_CORE_ID_LPASS_ADSP;
    MmpmCoreInstanceIdType instanceId = MMPM_CORE_INSTANCE_0;

    clientId = Test_Register(coreId, instanceId, STRESS_07_NAME);

    if(clientId)
    {
        for(loop = 0; (loop < STRESS_TEST_LOOP) && (MMPM_STATUS_SUCCESS == retSts); loop++)
        {
            sts = Test_RequestMips(clientId, 200);
            if(MMPM_STATUS_SUCCESS != sts)
            {
                ADSPPMTEST_LOG_PRINTF_1(
                    ADSPPMTEST_LOG_LEVEL_ERROR,
                    "[ERROR] Request, loop %u", loop);
                retSts = MMPM_STATUS_FAILED;
            }

            //TODO: read/compare clock

            sts = Test_ReleaseResource(clientId, MMPM_RSC_ID_MIPS_EXT);
            if(MMPM_STATUS_SUCCESS != sts)
            {
                ADSPPMTEST_LOG_PRINTF_1(
                    ADSPPMTEST_LOG_LEVEL_ERROR,
                    "[ERROR] Release, loop %u", loop);
                retSts = MMPM_STATUS_FAILED;
            }

            //TODO: read/compare clock
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
 * @ingroup Stress
 * @test stress_07
 * @brief MIPS Req-Rel, single client, multiple times
 */
static MMPM_STATUS Test_Stress_07_Mips(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(STRESS_07_NAME, STRESS_07_DETAILS);

    sts = TestStressSingleClientMultiMipsReqRel();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_Stress, STRESS_07_NAME, sts);
    return sts;
}


#define STRESS_08_NAME    "stress_08"
#define STRESS_08_DETAILS "Sleep Latency Req-Rel, 2 clients, same coreID/instanceID/microsec, multiple times"

static MMPM_STATUS TestStressMultiClientMultiSleepReqRel(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    uint32 clientId1 = 0, clientId2 = 0, loop;
    MmpmCoreIdType coreId = MMPM_CORE_ID_LPASS_ADSP;
    MmpmCoreInstanceIdType instanceId = MMPM_CORE_INSTANCE_0;

    clientId1 = Test_Register(coreId, instanceId, STRESS_08_NAME"_1");
    clientId2 = Test_Register(coreId, instanceId, STRESS_08_NAME"_2");

    if(clientId1 && clientId2)
    {
        for(loop = 0; (loop < STRESS_TEST_LOOP) && (MMPM_STATUS_SUCCESS == retSts); loop++)
        {
            sts = Test_RequestSleepLatency(clientId1, 5);
            if(MMPM_STATUS_SUCCESS != sts)
            {
                ADSPPMTEST_LOG_PRINTF_1(
                    ADSPPMTEST_LOG_LEVEL_ERROR,
                    "[ERROR] clientID 1 Request, loop %u", loop);
                retSts = MMPM_STATUS_FAILED;
            }

            sts = Test_RequestSleepLatency(clientId2, 50);
            if(MMPM_STATUS_SUCCESS != sts)
            {
                ADSPPMTEST_LOG_PRINTF_1(
                    ADSPPMTEST_LOG_LEVEL_ERROR,
                    "[ERROR] clientID 2 Request, loop %u", loop);
                retSts = MMPM_STATUS_FAILED;
            }

            sts = Test_ReleaseResource(clientId1, MMPM_RSC_ID_SLEEP_LATENCY);
            if(MMPM_STATUS_SUCCESS != sts)
            {
                ADSPPMTEST_LOG_PRINTF_1(
                    ADSPPMTEST_LOG_LEVEL_ERROR,
                    "[ERROR] clientID 1 Release, loop %u", loop);
                retSts = MMPM_STATUS_FAILED;
            }

            sts = Test_ReleaseResource(clientId2, MMPM_RSC_ID_SLEEP_LATENCY);
            if(MMPM_STATUS_SUCCESS != sts)
            {
                ADSPPMTEST_LOG_PRINTF_1(
                    ADSPPMTEST_LOG_LEVEL_ERROR,
                    "[ERROR] clientID 2 Release, loop %u", loop);
                retSts = MMPM_STATUS_FAILED;
            }
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
 * @ingroup Stress
 * @test stress_08
 * @brief Sleep Latency Req-Rel, 2 clients, same coreID/instanceID/microsec, multiple times
 */
static MMPM_STATUS Test_Stress_08_SleepLatency(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(STRESS_08_NAME, STRESS_08_DETAILS);

    sts = TestStressMultiClientMultiSleepReqRel();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_Stress, STRESS_08_NAME, sts);
    return sts;
}


#define STRESS_09_NAME    "stress_09"
#define STRESS_09_DETAILS "Sleep Latency Req-Rel, single client, multiple times"

static MMPM_STATUS TestStressSingleClientMultiSleepReqRel(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    uint32 clientId = 0, loop;
    MmpmCoreIdType coreId = MMPM_CORE_ID_LPASS_ADSP;
    MmpmCoreInstanceIdType instanceId = MMPM_CORE_INSTANCE_0;

    clientId = Test_Register(coreId, instanceId, STRESS_09_NAME);

    if(clientId)
    {
        for(loop = 0; (loop < STRESS_TEST_LOOP) && (MMPM_STATUS_SUCCESS == retSts); loop++)
        {
            sts = Test_RequestSleepLatency(clientId, 5);
            if(MMPM_STATUS_SUCCESS != sts)
            {
                ADSPPMTEST_LOG_PRINTF_1(
                    ADSPPMTEST_LOG_LEVEL_ERROR,
                    "[ERROR] Request, loop %u", loop);
                retSts = MMPM_STATUS_FAILED;
            }

            sts = Test_ReleaseResource(clientId, MMPM_RSC_ID_SLEEP_LATENCY);
            if(MMPM_STATUS_SUCCESS != sts)
            {
                ADSPPMTEST_LOG_PRINTF_1(
                    ADSPPMTEST_LOG_LEVEL_ERROR,
                    "[ERROR] Release, loop %u", loop);
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

/**
 * @ingroup Stress
 * @test stress_09
 * @brief Sleep Latency Req-Rel, single client, multiple times
 */
static MMPM_STATUS Test_Stress_09_SleepLatency(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(STRESS_09_NAME, STRESS_09_DETAILS);

    sts = TestStressSingleClientMultiSleepReqRel();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_Stress, STRESS_09_NAME, sts);
    return sts;
}


#define STRESS_10_NAME    "stress_10"
#define STRESS_10_DETAILS "Bandwidth Req-Rel, 2 clients, different bus route, multiple times"

static MMPM_STATUS TestStressMultiClientMultiBwReqRel(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    uint32 clientId1 = 0, clientId2 = 0, loop;
    MmpmCoreIdType coreId = MMPM_CORE_ID_LPASS_ADSP;
    MmpmCoreInstanceIdType instanceId = MMPM_CORE_INSTANCE_0;
    BwReqTestType bw1, bw2;

    memset(&bw1, 0, sizeof(BwReqTestType_Ext));
    memset(&bw2, 0, sizeof(BwReqTestType_Ext));
    
    clientId1 = Test_Register(coreId, instanceId, STRESS_10_NAME"_1");
    clientId2 = Test_Register(coreId, instanceId, STRESS_10_NAME"_2");

    if(clientId1 && clientId2)
    {
        bw1.masterPort = MMPM_BW_PORT_ID_ADSP_MASTER;
        bw1.slavePort = MMPM_BW_PORT_ID_DML_SLAVE;
        bw1.bwVal = 100000000;
        bw1.usagePercent = 100;
        bw1.usageType = MMPM_BW_USAGE_LPASS_DSP;

        bw2.masterPort = MMPM_BW_PORT_ID_DML_MASTER;
        bw2.slavePort = MMPM_BW_PORT_ID_AIF_SLAVE;
        bw2.bwVal = 100000000;
        bw2.usagePercent = 100;
        bw2.usageType = MMPM_BW_USAGE_LPASS_DMA;

        for(loop = 0; (loop < STRESS_TEST_LOOP) && (MMPM_STATUS_SUCCESS == retSts); loop++)
        {
            sts = Test_RequestBandwidth(clientId1, &bw1, 1, NULL);
            if(MMPM_STATUS_SUCCESS != sts)
            {
                ADSPPMTEST_LOG_PRINTF_1(
                    ADSPPMTEST_LOG_LEVEL_ERROR,
                    "[ERROR] clientID 1 Request, loop %u", loop);
                retSts = MMPM_STATUS_FAILED;
            }

            //TODO verify AHB clocks

            sts = Test_RequestBandwidth(clientId2, &bw2, 1, NULL);
            if(MMPM_STATUS_SUCCESS != sts)
            {
                ADSPPMTEST_LOG_PRINTF_1(
                    ADSPPMTEST_LOG_LEVEL_ERROR,
                    "[ERROR] clientID 2 Request, loop %u", loop);
                retSts = MMPM_STATUS_FAILED;
            }

            //TODO verify AHB clocks

            sts = Test_ReleaseResource(clientId1, MMPM_RSC_ID_GENERIC_BW);
            if(MMPM_STATUS_SUCCESS != sts)
            {
                ADSPPMTEST_LOG_PRINTF_1(
                    ADSPPMTEST_LOG_LEVEL_ERROR,
                    "[ERROR] clientID 1 Release, loop %u", loop);
                retSts = MMPM_STATUS_FAILED;
            }

            //TODO verify AHB clocks

            sts = Test_ReleaseResource(clientId2, MMPM_RSC_ID_GENERIC_BW);
            if(MMPM_STATUS_SUCCESS != sts)
            {
                ADSPPMTEST_LOG_PRINTF_1(
                    ADSPPMTEST_LOG_LEVEL_ERROR,
                    "[ERROR] clientID 2 Release, loop %u", loop);
                retSts = MMPM_STATUS_FAILED;
            }

            //TODO verify AHB clocks
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
 * @ingroup Stress
 * @test stress_10
 * @brief Bandwidth Req-Rel, 2 clients, different bus route, multiple times
 */
static MMPM_STATUS Test_Stress_10_Bandwidth(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(STRESS_10_NAME, STRESS_10_DETAILS);

    sts = TestStressMultiClientMultiBwReqRel();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_Stress, STRESS_10_NAME, sts);
    return sts;
}


#define STRESS_11_NAME    "stress_11"
#define STRESS_11_DETAILS "Bandwidth Req-Rel, single client, multiple times"

static MMPM_STATUS TestStressSingleClientMultiBwReqRel(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    uint32 clientId = 0, loop;
    MmpmCoreIdType coreId = MMPM_CORE_ID_LPASS_ADSP;
    MmpmCoreInstanceIdType instanceId = MMPM_CORE_INSTANCE_0;
    BwReqTestType bw;

    memset(&bw, 0, sizeof(BwReqTestType));

    clientId = Test_Register(coreId, instanceId, STRESS_11_NAME);

    if(clientId)
    {
        bw.masterPort = MMPM_BW_PORT_ID_ADSP_MASTER;
        bw.slavePort = MMPM_BW_PORT_ID_DML_SLAVE;
        bw.bwVal = 100000000;
        bw.usagePercent = 100;
        bw.usageType = MMPM_BW_USAGE_LPASS_DSP;

        for(loop = 0; (loop < STRESS_TEST_LOOP) && (MMPM_STATUS_SUCCESS == retSts); loop++)
        {
            sts = Test_RequestBandwidth(clientId, &bw, 1, NULL);
            if(MMPM_STATUS_SUCCESS != sts)
            {
                ADSPPMTEST_LOG_PRINTF_1(
                    ADSPPMTEST_LOG_LEVEL_ERROR,
                    "[ERROR] Request, loop %u", loop);
                retSts = MMPM_STATUS_FAILED;
            }

            //TODO verify AHB clocks

            sts = Test_ReleaseResource(clientId, MMPM_RSC_ID_GENERIC_BW);
            if(MMPM_STATUS_SUCCESS != sts)
            {
                ADSPPMTEST_LOG_PRINTF_1(
                    ADSPPMTEST_LOG_LEVEL_ERROR,
                    "[ERROR] Release, loop %u", loop);
                retSts = MMPM_STATUS_FAILED;
            }

            //TODO verify AHB clocks
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
 * @ingroup Stress
 * @test stress_11
 * @brief Bandwidth Req-Rel, single client, multiple times
 */
static MMPM_STATUS Test_Stress_11_Bandwidth(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(STRESS_11_NAME, STRESS_11_DETAILS);

    sts = TestStressSingleClientMultiBwReqRel();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_Stress, STRESS_11_NAME, sts);
    return sts;
}


#define STRESS_12_NAME    "stress_12"
#define STRESS_12_DETAILS "Reg Prog Req-Rel, 2 clients, same coreID/instanceID/speed, multiple times"

static MMPM_STATUS TestStressMultiClientMultiRegProgReqRel(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    uint32 clientId1 = 0, clientId2 = 0, loop;
    MmpmCoreIdType coreId = MMPM_CORE_ID_LPASS_DML;
    MmpmCoreInstanceIdType instanceId = MMPM_CORE_INSTANCE_0;

    clientId1 = Test_Register(coreId, instanceId, STRESS_12_NAME"_1");
    clientId2 = Test_Register(coreId, instanceId, STRESS_12_NAME"_2");

    if(clientId1 && clientId2)
    {
        for(loop = 0; (loop < STRESS_TEST_LOOP) && (MMPM_STATUS_SUCCESS == retSts); loop++)
        {
            sts = Test_RequestRegProg(clientId1, MMPM_REG_PROG_NORM);
            if(MMPM_STATUS_SUCCESS != sts)
            {
                ADSPPMTEST_LOG_PRINTF_1(
                    ADSPPMTEST_LOG_LEVEL_ERROR,
                    "[ERROR] clientID 1 Request, loop %u", loop);
                retSts = MMPM_STATUS_FAILED;
            }

            //TODO : verify AHB clock

            sts = Test_RequestRegProg(clientId2, MMPM_REG_PROG_FAST);
            if(MMPM_STATUS_SUCCESS != sts)
            {
                ADSPPMTEST_LOG_PRINTF_1(
                    ADSPPMTEST_LOG_LEVEL_ERROR,
                    "[ERROR] clientID 2 Request, loop %u", loop);
                retSts = MMPM_STATUS_FAILED;
            }

            //TODO verify AHB clock

            sts = Test_ReleaseResource(clientId2, MMPM_RSC_ID_REG_PROG);
            if(MMPM_STATUS_SUCCESS != sts)
            {
                ADSPPMTEST_LOG_PRINTF_1(
                    ADSPPMTEST_LOG_LEVEL_ERROR,
                    "[ERROR] clientID 2 Release, loop %u", loop);
                retSts = MMPM_STATUS_FAILED;
            }

            //TODO verify AHB clocks

            sts = Test_ReleaseResource(clientId1, MMPM_RSC_ID_REG_PROG);
            if(MMPM_STATUS_SUCCESS != sts)
            {
                ADSPPMTEST_LOG_PRINTF_1(
                    ADSPPMTEST_LOG_LEVEL_ERROR,
                    "[ERROR] clientID 1 Release, loop %u", loop);
                retSts = MMPM_STATUS_FAILED;
            }

            //TODO verify AHB clocks
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
 * @ingroup Stress
 * @test stress_12
 * @brief Reg Prog Req-Rel, 2 clients, same coreID/instanceID/speed, multiple times
 */
static MMPM_STATUS Test_Stress_12_RegProg(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(STRESS_12_NAME, STRESS_12_DETAILS);

    sts = TestStressMultiClientMultiRegProgReqRel();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_Stress, STRESS_12_NAME, sts);
    return sts;
}


#define STRESS_13_NAME    "stress_13"
#define STRESS_13_DETAILS "Reg Prog Req-Rel, single client, multiple times"

static MMPM_STATUS TestStressSingleClientMultiRegProgReqRel(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    uint32 clientId = 0, loop;
    MmpmCoreIdType coreId = MMPM_CORE_ID_LPASS_LPM;
    MmpmCoreInstanceIdType instanceId = MMPM_CORE_INSTANCE_0;

    clientId = Test_Register(coreId, instanceId, STRESS_13_NAME);

    if(clientId)
    {
        for(loop = 0; (loop < STRESS_TEST_LOOP) && (MMPM_STATUS_SUCCESS == retSts); loop++)
        {
            sts = Test_RequestRegProg(clientId, MMPM_REG_PROG_FAST);
            if(MMPM_STATUS_SUCCESS != sts)
            {
                ADSPPMTEST_LOG_PRINTF_1(
                    ADSPPMTEST_LOG_LEVEL_ERROR,
                    "[ERROR] Request, loop %u", loop);
                retSts = MMPM_STATUS_FAILED;
            }

            //TODO: verify AHB clocks

            sts = Test_ReleaseResource(clientId, MMPM_RSC_ID_REG_PROG);
            if(MMPM_STATUS_SUCCESS != sts)
            {
                ADSPPMTEST_LOG_PRINTF_1(
                    ADSPPMTEST_LOG_LEVEL_ERROR,
                    "[ERROR] Release, loop %u", loop);
                retSts = MMPM_STATUS_FAILED;
            }

            //TODO: verify AHB clocks
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
 * @ingroup Stress
 * @test stress_13
 * @brief Reg Prog Req-Rel, single client, multiple times
 */
static MMPM_STATUS Test_Stress_13_RegProg(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(STRESS_13_NAME, STRESS_13_DETAILS);

    sts = TestStressSingleClientMultiRegProgReqRel();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_Stress, STRESS_13_NAME, sts);
    return sts;
}


#define STRESS_14_NAME    "stress_14"
#define STRESS_14_DETAILS "Power Req-Rel, 2 clients, same coreID/instanceID, multiple times"

static MMPM_STATUS TestStressMultiClientMultiPowerReqRel(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    uint32 clientId1 = 0, clientId2 = 0, loop;
    MmpmCoreIdType coreId = MMPM_CORE_ID_LPASS_DML;
    MmpmCoreInstanceIdType instanceId = MMPM_CORE_INSTANCE_0;

    clientId1 = Test_Register(coreId, instanceId, STRESS_14_NAME"_1");
    clientId2 = Test_Register(coreId, instanceId, STRESS_14_NAME"_2");

    if(clientId1 && clientId2)
    {
        for(loop = 0; (loop < STRESS_TEST_LOOP) && (MMPM_STATUS_SUCCESS == retSts); loop++)
        {
            sts = Test_RequestPower(clientId1);
            if(MMPM_STATUS_SUCCESS != sts)
            {
                ADSPPMTEST_LOG_PRINTF_1(
                    ADSPPMTEST_LOG_LEVEL_ERROR,
                    "[ERROR] clientID 1 Request, loop %u", loop);
                retSts = MMPM_STATUS_FAILED;
            }

            sts = Test_RequestPower(clientId2);
            if(MMPM_STATUS_SUCCESS != sts)
            {
                ADSPPMTEST_LOG_PRINTF_1(
                    ADSPPMTEST_LOG_LEVEL_ERROR,
                    "[ERROR] clientID 2 Request, loop %u", loop);
                retSts = MMPM_STATUS_FAILED;
            }

            sts = Test_ReleaseResource(clientId1, MMPM_RSC_ID_POWER);
            if(MMPM_STATUS_SUCCESS != sts)
            {
                ADSPPMTEST_LOG_PRINTF_1(
                    ADSPPMTEST_LOG_LEVEL_ERROR,
                    "[ERROR] clientID 1 Release, loop %u", loop);
                retSts = MMPM_STATUS_FAILED;
            }

            sts = Test_ReleaseResource(clientId2, MMPM_RSC_ID_POWER);
            if (MMPM_STATUS_SUCCESS != sts)
            {
                ADSPPMTEST_LOG_PRINTF_1(
                    ADSPPMTEST_LOG_LEVEL_ERROR,
                    "[ERROR] clientID 2 Release, loop %u", loop);
                retSts = MMPM_STATUS_FAILED;
            }
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
 * @ingroup Stress
 * @test stress_14
 * @brief Power Req-Rel, 2 clients, same coreID/instanceID, multiple times
 */
static MMPM_STATUS Test_Stress_14_Power(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(STRESS_14_NAME, STRESS_14_DETAILS);

    sts = TestStressMultiClientMultiPowerReqRel();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_Stress, STRESS_14_NAME, sts);
    return sts;
}


#define STRESS_15_NAME    "stress_15"
#define STRESS_15_DETAILS "Power Req-Rel, single client, multiple times"

static MMPM_STATUS TestStressSingleClientMultiPowerReqRel(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    uint32 clientId = 0, loop;
    MmpmCoreIdType coreId = MMPM_CORE_ID_LPASS_ADSP;
    MmpmCoreInstanceIdType instanceId = MMPM_CORE_INSTANCE_0;

    clientId = Test_Register(coreId, instanceId, STRESS_15_NAME);

    if(clientId)
    {
        for(loop = 0; (loop < STRESS_TEST_LOOP) && (MMPM_STATUS_SUCCESS == retSts); loop++)
        {
            sts = Test_RequestPower(clientId);
            if(MMPM_STATUS_SUCCESS != sts)
            {
                ADSPPMTEST_LOG_PRINTF_1(
                    ADSPPMTEST_LOG_LEVEL_ERROR,
                    "[ERROR] Request, loop %u", loop);
                retSts = MMPM_STATUS_FAILED;
            }

            sts = Test_ReleaseResource(clientId, MMPM_RSC_ID_POWER);
            if(MMPM_STATUS_SUCCESS != sts)
            {
                ADSPPMTEST_LOG_PRINTF_1(
                    ADSPPMTEST_LOG_LEVEL_ERROR,
                    "[ERROR] Release, loop %u", loop);
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

/**
 * @ingroup Stress
 * @test stress_15
 * @brief Power Req-Rel, single client, multiple times
 */
static MMPM_STATUS Test_Stress_15_Power(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(STRESS_15_NAME, STRESS_15_DETAILS);

    sts = TestStressSingleClientMultiPowerReqRel();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_Stress, STRESS_15_NAME, sts);
    return sts;
}


#define STRESS_16_NAME    "stress_16"
#define STRESS_16_DETAILS "Memory Power Req-Rel, 2 clients, same coreID/instanceID, multiple times"

static MMPM_STATUS TestStressMultiClientMultiMemPowerReqRel(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    uint32 clientId1 = 0, clientId2 = 0, loop;
    MmpmCoreIdType coreId = MMPM_CORE_ID_LPASS_DML;
    MmpmCoreInstanceIdType instanceId = MMPM_CORE_INSTANCE_0;

    clientId1 = Test_Register(coreId, instanceId, STRESS_16_NAME"_1");
    clientId2 = Test_Register(coreId, instanceId, STRESS_16_NAME"_2");

    if(clientId1 && clientId2)
    {
        for(loop = 0; (loop < STRESS_TEST_LOOP) && (MMPM_STATUS_SUCCESS == retSts); loop++)
        {
            sts = Test_RequestMemoryPower(
                clientId1, MMPM_MEM_LPASS_LPM, MMPM_MEM_POWER_RETENTION);
            if(MMPM_STATUS_SUCCESS != sts)
            {
                ADSPPMTEST_LOG_PRINTF_1(
                    ADSPPMTEST_LOG_LEVEL_ERROR,
                    "[ERROR] clientID 1 Request, loop %u", loop);
                retSts = MMPM_STATUS_FAILED;
            }

            sts = Test_RequestMemoryPower(
                clientId2, MMPM_MEM_LPASS_LPM, MMPM_MEM_POWER_ACTIVE);
            if(MMPM_STATUS_SUCCESS != sts)
            {
                ADSPPMTEST_LOG_PRINTF_1(
                    ADSPPMTEST_LOG_LEVEL_ERROR,
                    "[ERROR] clientID 2 Request, loop %u", loop);
                retSts = MMPM_STATUS_FAILED;
            }

            sts = Test_ReleaseResource(clientId1, MMPM_RSC_ID_MEM_POWER);
            if(MMPM_STATUS_SUCCESS != sts)
            {
                ADSPPMTEST_LOG_PRINTF_1(
                    ADSPPMTEST_LOG_LEVEL_ERROR,
                    "[ERROR] clientID 1 Release, loop %u", loop);
                retSts = MMPM_STATUS_FAILED;
            }

            sts = Test_ReleaseResource(clientId2, MMPM_RSC_ID_MEM_POWER);
            if (MMPM_STATUS_SUCCESS != sts)
            {
                ADSPPMTEST_LOG_PRINTF_1(
                    ADSPPMTEST_LOG_LEVEL_ERROR,
                    "[ERROR] clientID 2 Release, loop %u", loop);
                retSts = MMPM_STATUS_FAILED;
            }
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
 * @ingroup Stress
 * @test stress_16
 * @brief Memory Power Req-Rel, 2 clients, same coreID/instanceID, multiple times
 */
static MMPM_STATUS Test_Stress_16_MemoryPower(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(STRESS_16_NAME, STRESS_16_DETAILS);

    sts = TestStressMultiClientMultiMemPowerReqRel();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_Stress, STRESS_16_NAME, sts);
    return sts;
}


#define STRESS_17_NAME    "stress_17"
#define STRESS_17_DETAILS "Power Req-Rel, single client, multiple times"

static MMPM_STATUS TestStressSingleClientMultiMemPowerReqRel(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    uint32 clientId = 0, loop;
    MmpmCoreIdType coreId = MMPM_CORE_ID_LPASS_ADSP;
    MmpmCoreInstanceIdType instanceId = MMPM_CORE_INSTANCE_0;

    clientId = Test_Register(coreId, instanceId, STRESS_17_NAME);

    if(clientId)
    {
        for(loop = 0; (loop < STRESS_TEST_LOOP) && (MMPM_STATUS_SUCCESS == retSts); loop++)
        {
            sts = Test_RequestMemoryPower(
                clientId, MMPM_MEM_LPASS_LPM, MMPM_MEM_POWER_RETENTION);
            if(MMPM_STATUS_SUCCESS != sts)
            {
                ADSPPMTEST_LOG_PRINTF_1(
                    ADSPPMTEST_LOG_LEVEL_ERROR,
                    "[ERROR] Request, loop %u", loop);
                retSts = MMPM_STATUS_FAILED;
            }

            sts = Test_ReleaseResource(clientId, MMPM_RSC_ID_MEM_POWER);
            if(MMPM_STATUS_SUCCESS != sts)
            {
                ADSPPMTEST_LOG_PRINTF_1(
                    ADSPPMTEST_LOG_LEVEL_ERROR,
                    "[ERROR] Release, loop %u", loop);
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

/**
 * @ingroup Stress
 * @test stress_17
 * @brief Power Req-Rel, single client, multiple times
 */
static MMPM_STATUS Test_Stress_17_MemoryPower(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(STRESS_17_NAME, STRESS_17_DETAILS);

    sts = TestStressSingleClientMultiMemPowerReqRel();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_Stress, STRESS_17_NAME, sts);
    return sts;
}

#define STRESS_18_NAME    "stress_18"
#define STRESS_18_DETAILS "Ext Bandwidth Req-Rel, single client, multiple times"

static MMPM_STATUS TestStressSingleClientMultiBwReqRelExt(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    uint32 clientId = 0, loop;
    MmpmCoreIdType coreId = MMPM_CORE_ID_LPASS_ADSP;
    MmpmCoreInstanceIdType instanceId = MMPM_CORE_INSTANCE_0;
    BwReqTestType_Ext bw;

    memset(&bw,0, sizeof(BwReqTestType_Ext));

    clientId = Test_Register(coreId, instanceId, STRESS_18_NAME);

    if(clientId)
    {
        bw.masterPort = MMPM_BW_PORT_ID_ADSP_MASTER;
        bw.slavePort = MMPM_BW_PORT_ID_DDR_SLAVE;
        bw.bwVal = 100000000;
        bw.usagePercent = 100;
        bw.usageType = MMPM_BW_USAGE_LPASS_DSP;

        for(loop = 0; (loop < STRESS_TEST_LOOP) && (MMPM_STATUS_SUCCESS == retSts); loop++)
        {
            sts = Test_RequestBandwidthExt(clientId, &bw, 1);
            if(MMPM_STATUS_SUCCESS != sts)
            {
                ADSPPMTEST_LOG_PRINTF_1(
                    ADSPPMTEST_LOG_LEVEL_ERROR,
                    "[ERROR] Request, loop %u", loop);
                retSts = MMPM_STATUS_FAILED;
            }

            sts = Test_ReleaseResource(clientId, MMPM_RSC_ID_GENERIC_BW_EXT);
            if(MMPM_STATUS_SUCCESS != sts)
            {
                ADSPPMTEST_LOG_PRINTF_1(
                    ADSPPMTEST_LOG_LEVEL_ERROR,
                    "[ERROR] Release, loop %u", loop);
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

/**
 * @ingroup Stress
 * @test stress_18
 * @brief Ext Bandwidth Req-Rel, single client, multiple times
 */
static MMPM_STATUS Test_Stress_18_ExtBandwidth(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(STRESS_18_NAME, STRESS_18_DETAILS);

    sts = TestStressSingleClientMultiBwReqRelExt();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_Stress, STRESS_18_NAME, sts);
    return sts;
}

#define STRESS_19_NAME     "PD Restart"
#define STRESS_19_DETAILS  "Test PD Restart Cleanup"

static int testPdRestart(uint32 iterations)
{
    uint32 i = 0;
    int handle;

    for(i = 0; i < iterations; i++)
    {
        handle = qurt_qdi_open(ADSPPM_QDI_DRV_NAME);
        if(handle < 0)
        {
            ADSPPMTEST_LOG_PRINTF_0(ADSPPMTEST_LOG_LEVEL_ERROR,
                "Failed to open QDI client for testing");
            goto cleanup_none;
        }
        
        MmpmRegParamType regParams;
        regParams.rev = MMPM_REVISION;
        regParams.coreId = MMPM_CORE_ID_LPASS_ADSP;
        regParams.instanceId = MMPM_CORE_INSTANCE_0;
        regParams.pClientName = "QDI_TEST_CLIENT";
        regParams.pwrCtrlFlag = 0;
        regParams.callBackFlag = 0;
        regParams.MMPM_Callback = NULL;
        regParams.cbFcnStackSize = 0;
        
        uint32 clientId = 0;
        int retVal = qurt_qdi_handle_invoke(handle, ADSPPM_REGISTER,
            &regParams, &clientId);
        if(retVal < 0)
        {
            ADSPPMTEST_LOG_PRINTF_0(ADSPPMTEST_LOG_LEVEL_ERROR,
                "Failed to create MMPM client");
            goto cleanup_handle;
        }
        
        // Should clean up MMPM clientId
        qurt_qdi_close(handle);
    }
        
    return 1;
    
cleanup_handle:
    qurt_qdi_close(handle);
    
cleanup_none:
    return 0;
}

void Test_Stress_19_PdRestart(void)
{
    int testResult = 1;
    
    ADSPPMTEST_LOG_TEST_HEADER(STRESS_19_NAME, STRESS_19_DETAILS);

    // Arbitrary number of iterations, but 1000 should cause problems if any
    // exist
    testResult = testPdRestart(1000);
    
    MMPM_STATUS sts = testResult ? MMPM_STATUS_SUCCESS : MMPM_STATUS_FAILED;
    
    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_Stress, STRESS_19_NAME, sts);
}

void Test_Stress(void)
{
    // Multiple Reg->Multiple Dreg, twice (2 loops)
    Test_Stress_01_System();

    // Core Clock Req-Rel, 2 clients, same core clock ID, different frequency, multiple times
    Test_Stress_02_CoreClock();

    // Core Clock Req-Rel, single client, multiple times
    Test_Stress_03_CoreClock();

    // Core Clock Domain Req-Rel, 2 clients, same core clock ID, different sources, multiple times
    //Test_Stress_04_CoreClockDomain();

    // Core Clock Domain Req-Rel, single client, multiple times
    //Test_Stress_05_CoreClockDomain();

    // MIPS Req-Rel, 2 clients, same coreID/instanceID/MIPS, multiple times
    Test_Stress_06_Mips();

    // MIPS Req-Rel, single client, multiple times
    Test_Stress_07_Mips();

    // Sleep Latency Req-Rel, 2 clients, same coreID/instanceID/microsec, multiple times
    Test_Stress_08_SleepLatency();

    // Sleep Latency Req-Rel, single client, multiple times
    Test_Stress_09_SleepLatency();

    // Bandwidth Req-Rel, 2 clients, different bus route, multiple times
    Test_Stress_10_Bandwidth();

    // Bandwidth Req-Rel, single client, multiple times
    Test_Stress_11_Bandwidth();

    // Reg Prog Req-Rel, 2 clients, same coreID/instanceID/speed, multiple times
    Test_Stress_12_RegProg();

    // Reg Prog Req-Rel, single client, multiple times
    Test_Stress_13_RegProg();

    // Power Req-Rel, 2 clients, same coreID/instanceID, multiple times
    Test_Stress_14_Power();

    // Power Req-Rel, single client, multiple times
    Test_Stress_15_Power();

    // Memory Power Req-Rel, 2 clients, same coreID/instanceID, multiple times
    Test_Stress_16_MemoryPower();

    // Power Req-Rel, single client, multiple times
    Test_Stress_17_MemoryPower();

    // Ext Bandwidth Req-Rel, single client, multiple times
    Test_Stress_18_ExtBandwidth();

    // Test PD Restart Cleanup
    Test_Stress_19_PdRestart();

}

