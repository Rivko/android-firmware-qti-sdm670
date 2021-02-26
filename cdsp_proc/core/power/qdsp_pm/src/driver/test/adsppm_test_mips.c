/*
* Copyright (c) 2015 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
@file: adsppm_test_mips.c
@brief: Implementation of ADSPPM test module of MIPS/MPPS requests.

$Header: //components/rel/core.qdsp6/2.1.c4/power/qdsp_pm/src/driver/test/adsppm_test_mips.c#1 $
*/

#include "mmpm.h"
#include "adsppm_test.h"
#include "adsppm_test_param.h"
#include "adsppm_test_utils.h"
#include "adsppm_test_asic_info.h"
#include "adsppm_utils.h"


#define MIPS_01_NAME    "mips_01"
#define MIPS_01_DETAILS "Req-Rel, all core/instance combination"

static MMPM_STATUS TestAllCoresMipsRequest(void)
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
                clientId = Test_Register(coreId, instanceId, MIPS_01_NAME);
                if(clientId)
                {
                    sts = Test_RequestMips(clientId, 200);
                    if(MMPM_STATUS_SUCCESS != sts)
                    {
                        retSts = MMPM_STATUS_FAILED;
                    }
                    else
                    {
                        sts = Test_ReleaseResource(clientId, MMPM_RSC_ID_MIPS_EXT);
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
 * @ingroup MIPS
 * @test mips_01
 * @brief Req-Rel, all core/instance combination
 */
static MMPM_STATUS Test_Mips_01(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(MIPS_01_NAME, MIPS_01_DETAILS);

    sts = TestAllCoresMipsRequest();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_MIPS, MIPS_01_NAME, sts);
    return sts;
}


#define MIPS_02_NAME    "mips_02"
#define MIPS_02_DETAILS "Req-Rel, 2 same coreID/instanceID clients, expect aggregated MIPS"

static AdsppmTestType testmips02[] =
{
    {1, 2, MMPM_CORE_ID_LPASS_ADSP, MMPM_CORE_INSTANCE_0,
        {REG, REQMIPS}
    },
    {2, 2, MMPM_CORE_ID_LPASS_ADSP, MMPM_CORE_INSTANCE_0,
        {REG, REQMIPS}
    },
    {1, 2, MMPM_CORE_ID_LPASS_ADSP, MMPM_CORE_INSTANCE_0,
        {RELMIPS, DREG}
    },
    {2, 2, MMPM_CORE_ID_LPASS_ADSP, MMPM_CORE_INSTANCE_0,
        {RELMIPS, DREG}
    }
};

/**
 * @ingroup MIPS
 * @test mips_02
 * @brief Req-Rel, 2 same coreID/instanceID clients, expect aggregated MIPS
 */
static MMPM_STATUS Test_Mips_02(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, testSts[MAX_TEST][MAX_TEST_SEQUENCE];
    uint32 numTest = 0;
    ADSPPMTEST_LOG_TEST_HEADER(MIPS_02_NAME, MIPS_02_DETAILS);

    numTest = sizeof(testmips02)/sizeof(AdsppmTestType);
    sts = Test_InvokeTest(testmips02, numTest, testSts);

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_MIPS, MIPS_02_NAME, sts);
    return sts;
}


#define MIPS_03_NAME    "mips_03"
#define MIPS_03_DETAILS "Req-Rel-Rel, expect NO errors"

static AdsppmTestType testmips03[] =
{
    {1, 5, MMPM_CORE_ID_LPASS_ADSP, MMPM_CORE_INSTANCE_0,
        {REG, REQMIPS, RELMIPS, RELMIPS,  DREG}
    }
};

/**
 * @ingroup MIPS
 * @test mips_03
 * @brief Req-Rel-Rel, expect NO errors
 */
static MMPM_STATUS Test_Mips_03(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, testSts[MAX_TEST][MAX_TEST_SEQUENCE];
    uint32 numTest = 0;
    ADSPPMTEST_LOG_TEST_HEADER(MIPS_03_NAME, MIPS_03_DETAILS);

    numTest = sizeof(testmips03)/sizeof(AdsppmTestType);
    sts = Test_InvokeTest(testmips03, numTest, testSts);

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_MIPS, MIPS_03_NAME, sts);
    return sts;
}


#define MIPS_04_NAME    "mips_04"
#define MIPS_04_DETAILS "Invalid client ID Req, expect failure Req"

static MMPM_STATUS TestMipsInvalidClientId(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    uint32 clientId = 0;
    MmpmCoreIdType coreId = MMPM_CORE_ID_LPASS_ADSP;
    MmpmCoreInstanceIdType instanceId = MMPM_CORE_INSTANCE_0;

    clientId = Test_Register(coreId, instanceId, MIPS_04_NAME);

    if(clientId)
    {
        sts = Test_Deregister(clientId);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            retSts = MMPM_STATUS_FAILED;
        }

        sts = Test_RequestMips(clientId, 400);
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
 * @ingroup MIPS
 * @test mips_04
 * @brief Invalid client ID Req, expect failure Req
 */
static MMPM_STATUS Test_Mips_04(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(MIPS_04_NAME, MIPS_04_DETAILS);

    sts = TestMipsInvalidClientId();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_MIPS, MIPS_04_NAME, sts);
    return sts;
}


#define MIPS_05_NAME    "mips_05"
#define MIPS_05_DETAILS "Q6 core clock frequency plan"

static MMPM_STATUS TestQ6CoreClockFreqPlan(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    uint32 clientId = 0;
    uint32 index, mips, setClkFreq, readClkFreq;
    const AsicInfo_MipsInfoType *pMipsInfo = NULL;
    const AsicInfo_ClockFreqVoltPlanType *pQ6FreqPlan = NULL;
    MmpmCoreIdType coreId = MMPM_CORE_ID_LPASS_ADSP;
    MmpmCoreInstanceIdType instanceId = MMPM_CORE_INSTANCE_0;

    clientId = Test_Register(coreId, instanceId, MIPS_05_NAME);

    if(clientId)
    {
        pMipsInfo = AsicInfo_getMipsInfo();
        pQ6FreqPlan = AsicInfo_getClockFreqVoltPlan(MMPM_CLK_ID_LPASS_ADSP_CORE);

        for(index = 0; index < pQ6FreqPlan->numFreq; index++)
        {
            setClkFreq = pQ6FreqPlan->pFreqVoltPlan[index].frequency;
            mips = setClkFreq/1000000*256/pMipsInfo->mips2clockFactor;

            sts = Test_RequestMips(clientId, mips);
            if(MMPM_STATUS_SUCCESS != sts)
            {
                retSts = MMPM_STATUS_FAILED;
            }
            else
            {
                // Read Q6 clock frequency
                sts = Test_InfoClk(coreId, instanceId, MMPM_CLK_ID_LPASS_ADSP_CORE, &readClkFreq);
                if(MMPM_STATUS_SUCCESS != sts)
                {
                    retSts = MMPM_STATUS_FAILED;
                }
                else
                {
                    if(!Test_CompareFreq(setClkFreq, readClkFreq * 1000))
                    {
                        ADSPPMTEST_LOG_PRINTF_2(
                            ADSPPMTEST_LOG_LEVEL_ERROR,
                            "[ERROR] setClkFreq = %u Hz, readClkFreq = %u Hz",
                            setClkFreq, readClkFreq * 1000);
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
 * @ingroup MIPS
 * @test mips_05
 * @brief Q6 core clock frequency plan
 */
static MMPM_STATUS Test_Mips_05(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(MIPS_05_NAME, MIPS_05_DETAILS);

    sts = TestQ6CoreClockFreqPlan();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_MIPS, MIPS_05_NAME, sts);
    return sts;
}


#define MIPS_06_NAME    "mips_06"
#define MIPS_06_DETAILS "Very large and casted negative MIPS value should result in highest Q6 freq"

static MMPM_STATUS TestVeryLargeAndNegativeMipsReq(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    uint32 clientId = 0;
    uint32 mips, setClkFreq, readClkFreq;
    int negativeMips;
    const AsicInfo_ClockFreqVoltPlanType *pQ6FreqPlan = NULL;
    MmpmCoreIdType coreId = MMPM_CORE_ID_LPASS_ADSP;
    MmpmCoreInstanceIdType instanceId = MMPM_CORE_INSTANCE_0;

    clientId = Test_Register(coreId, instanceId, MIPS_06_NAME);

    if(clientId)
    {
        pQ6FreqPlan = AsicInfo_getClockFreqVoltPlan(MMPM_CLK_ID_LPASS_ADSP_CORE);
        setClkFreq = pQ6FreqPlan->pFreqVoltPlan[pQ6FreqPlan->numFreq - 1].frequency;

        // Very large MIPS value
        mips = 0xFFFFFFFF;

        sts = Test_RequestMips(clientId, mips);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            retSts = MMPM_STATUS_FAILED;
        }
        else
        {
            // Read Q6 clock frequency
            sts = Test_InfoClk(coreId, instanceId, MMPM_CLK_ID_LPASS_ADSP_CORE, &readClkFreq);
            if(MMPM_STATUS_SUCCESS != sts)
            {
                retSts = MMPM_STATUS_FAILED;
            }
            else
            {
                if(!Test_CompareFreq(setClkFreq, readClkFreq * 1000))
                {
                    ADSPPMTEST_LOG_PRINTF_2(
                        ADSPPMTEST_LOG_LEVEL_ERROR,
                        "[ERROR] setClkFreq = %u Hz, readClkFreq = %u Hz",
                        setClkFreq, readClkFreq * 1000);
                    retSts = MMPM_STATUS_FAILED;
                }
            }

            sts = Test_ReleaseResource(clientId, MMPM_RSC_ID_MIPS_EXT);
            if(MMPM_STATUS_SUCCESS != sts)
            {
                retSts = MMPM_STATUS_FAILED;
            }
        }

        // Very large MIPS value 2
        mips = 0x800000F0;

        sts = Test_RequestMips(clientId, mips);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            retSts = MMPM_STATUS_FAILED;
        }
        else
        {
            // Read Q6 clock frequency
            sts = Test_InfoClk(coreId, instanceId, MMPM_CLK_ID_LPASS_ADSP_CORE, &readClkFreq);
            if(MMPM_STATUS_SUCCESS != sts)
            {
                retSts = MMPM_STATUS_FAILED;
            }
            else
            {
                if(!Test_CompareFreq(setClkFreq, readClkFreq * 1000))
                {
                    ADSPPMTEST_LOG_PRINTF_2(
                        ADSPPMTEST_LOG_LEVEL_ERROR,
                        "[ERROR] setClkFreq = %u Hz, readClkFreq = %u Hz",
                        setClkFreq, readClkFreq * 1000);
                    retSts = MMPM_STATUS_FAILED;
                }
            }

            sts = Test_ReleaseResource(clientId, MMPM_RSC_ID_MIPS_EXT);
            if(MMPM_STATUS_SUCCESS != sts)
            {
                retSts = MMPM_STATUS_FAILED;
            }
        }

        // Casted Negative MIPS value
        negativeMips = -240;
        mips = (uint32)negativeMips;

        sts = Test_RequestMips(clientId, mips);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            retSts = MMPM_STATUS_FAILED;
        }
        else
        {
            // Read Q6 clock frequency
            sts = Test_InfoClk(coreId, instanceId, MMPM_CLK_ID_LPASS_ADSP_CORE, &readClkFreq);
            if(MMPM_STATUS_SUCCESS != sts)
            {
                retSts = MMPM_STATUS_FAILED;
            }
            else
            {
                if(!Test_CompareFreq(setClkFreq, readClkFreq * 1000))
                {
                    ADSPPMTEST_LOG_PRINTF_2(
                        ADSPPMTEST_LOG_LEVEL_ERROR,
                        "[ERROR] setClkFreq = %u Hz, readClkFreq = %u Hz",
                        setClkFreq, readClkFreq * 1000);
                    retSts = MMPM_STATUS_FAILED;
                }
            }

            sts = Test_ReleaseResource(clientId, MMPM_RSC_ID_MIPS_EXT);
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

/**
 * @ingroup MIPS
 * @test mips_06
 * @brief Very large and casted negative MIPS value should result in highest Q6 freq
 */
static MMPM_STATUS Test_Mips_06(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(MIPS_06_NAME, MIPS_06_DETAILS);

    sts = TestVeryLargeAndNegativeMipsReq();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_MIPS, MIPS_06_NAME, sts);
    return sts;
}

#define MPPS_07_NAME    "mpps_07"
#define MPPS_07_DETAILS "Req-Rel all core/instances combination for MPPS"

static MMPM_STATUS TestAllCoresMppsRequest(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    MmpmCoreIdType coreId;
    MmpmCoreInstanceIdType instanceId;
    uint32 clientId;
    MmpmMppsReqType mppsReq;

    mppsReq.mppsTotal = 200;
    // Don't care about floor clock for this test
    mppsReq.adspFloorClock = 0;

    for(coreId = MMPM_CORE_ID_LPASS_ADSP; coreId < MMPM_CORE_ID_LPASS_END; coreId++)
    {
        for(instanceId = MMPM_CORE_INSTANCE_0; instanceId < MMPM_CORE_INSTANCE_MAX; instanceId++)
        {
            if(AsicInfo_isCoreInstanceAvailable(coreId, instanceId))
            {
                clientId = Test_Register(coreId, instanceId, MPPS_07_NAME);
                if(clientId)
                {
                    sts = Test_RequestMpps(clientId, &mppsReq);
                    if(MMPM_STATUS_SUCCESS != sts)
                    {
                        retSts = MMPM_STATUS_FAILED;
                    }
                    else
                    {
                        sts = Test_ReleaseResource(clientId, MMPM_RSC_ID_MPPS);
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
 * @ingroup MIPS
 * @test mpps_07
 * @brief Req-Rel all core/instances combination for MPPS
 */
static MMPM_STATUS Test_Mpps_07(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(MPPS_07_NAME, MPPS_07_DETAILS);

    sts = TestAllCoresMppsRequest();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_MIPS, MPPS_07_NAME, sts);
    return sts;
}

#define MPPS_08_NAME    "mpps_08"
#define MPPS_08_DETAILS "Test MPPS query"

typedef struct
{
    uint32 clientId;
    MmpmClientClassType clientClassId;
} ClientClassTestType;

static ClientClassTestType clientClassId[] =
{
    {0, MMPM_AUDIO_CLIENT_CLASS},
    {0, MMPM_VOICE_CLIENT_CLASS},
    {0, MMPM_COMPUTE_CLIENT_CLASS},
    {0, MMPM_STREAMING_1HVX_CLIENT_CLASS},
    {0, MMPM_STREAMING_2HVX_CLIENT_CLASS}
};

static MMPM_STATUS TestMppsQuery(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    MmpmMppsReqType mppsReq;
    MmpmInfoMppsType mppsInfo;
    uint32 clientClasses = 0;
    uint32 index = 0;
    uint32 numOfClientClasses = sizeof(clientClassId)/sizeof(clientClassId[0]);

    mppsReq.mppsTotal = 100;
    // Don't care about floor clock for this test
    mppsReq.adspFloorClock = 0;

    for(index = 0; index < numOfClientClasses; index++)
    {
        clientClassId[index].clientId = Test_Register(
            MMPM_CORE_ID_LPASS_ADSP,
            MMPM_CORE_INSTANCE_0,
            MPPS_08_NAME);
        if(clientClassId[index].clientId == 0)
            goto TestMppsQuery_error;

        sts = Test_SetClientClass(
            clientClassId[index].clientId,
            clientClassId[index].clientClassId);
        if(sts != MMPM_STATUS_SUCCESS)
            goto TestMppsQuery_error;

        sts = Test_RequestMpps(clientClassId[index].clientId, &mppsReq);
        if(sts != MMPM_STATUS_SUCCESS)
            goto TestMppsQuery_error;

        mppsInfo.clientClasses = clientClassId[index].clientClassId;
        mppsInfo.aggregateMpps = 0;
        sts = Test_InfoMpps(&mppsInfo);
        if(sts != MMPM_STATUS_SUCCESS)
            goto TestMppsQuery_error;

        if(mppsInfo.aggregateMpps < mppsReq.mppsTotal)
        {
            ADSPPMTEST_LOG_PRINTF_0(ADSPPMTEST_LOG_LEVEL_ERROR,
                "[ERROR] unexpected aggregate MPPS returned from query");
            goto TestMppsQuery_error;
        }

        clientClasses |= clientClassId[index].clientClassId;
    }

    mppsInfo.clientClasses = clientClasses;
    mppsInfo.aggregateMpps = 0;
    sts = Test_InfoMpps(&mppsInfo);
    if(sts != MMPM_STATUS_SUCCESS)
        goto TestMppsQuery_error;

    if(mppsInfo.aggregateMpps <
        mppsReq.mppsTotal * numOfClientClasses)
    {
        ADSPPMTEST_LOG_PRINTF_0(ADSPPMTEST_LOG_LEVEL_ERROR,
            "[ERROR] unexpected aggregate MPPS returned from query");
        goto TestMppsQuery_error;
    }

    for(index = 0; index < numOfClientClasses; index++)
    {
        sts = Test_Deregister(clientClassId[index].clientId);
        if(sts != MMPM_STATUS_SUCCESS)
            goto TestMppsQuery_error;
    }

    return sts;

TestMppsQuery_error:

    for(index = 0; index < numOfClientClasses; index++)
        if(clientClassId[index].clientId)
            Test_Deregister(clientClassId[index].clientId);

    return MMPM_STATUS_FAILED;

}

/**
 * @ingroup MIPS
 * @test mpps_08
 * @brief Test MPPS query
 */
static MMPM_STATUS Test_Mpps_08(void)
{

    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(MPPS_08_NAME, MPPS_08_DETAILS);

    sts = TestMppsQuery();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_MIPS, MPPS_08_NAME, sts);
    return sts;

}

#define MPPS_09_NAME    "mpps_09"
#define MPPS_09_DETAILS "Scaling of AHB-E frequency with ADSP frequency"

typedef struct
{
    uint32 q6FreqHz;    // DSP clock setting
    uint32 ahbeFreqHz;  // expected AHB-E frequency
} q6AhbFreqPair;

static MMPM_STATUS TestQ6AhbeScaling(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    uint32 clientId = 0;
    MmpmCoreIdType coreId = MMPM_CORE_ID_LPASS_ADSP;
    MmpmCoreInstanceIdType instanceId = MMPM_CORE_INSTANCE_0;
    AdsppmInfoAhbType ahbInfo;
    q6AhbFreqPair table[] =
        {
            //
            // Warning: Target-specific and version-specific data here.
            //          Data below is for 8996 v2 and would need update for
            //          other versions.
            //
            //
            //         Q6,  expected AHBE
            {   288000000,       25600000 }, // MinSVS f-max
            {   460800000,       51200000 }, // lowSVS f-max
            {   614400000,       102400000 }, // svs f-max
            {   787200000,       102400000 }, // svs_L1 f-max
            {   940800000,       204800000 }, // nominal ADSP f-max
            {   1190400000,      204800000 }, // turbo


            {   100000000,       25600000 },
            {   200000000,       25600000 },
            {   300000000,       51200000 },
            {   400000000,       51200000 },
            {   500000000,       102400000},
            {   600000000,      102400000 },
            {   700000000,      102400000 },
            {   800000000,      102400000 },
            {  1000000000,      204800000 },
            {  2000000000,      204800000 },
            {  3000000000,      204800000 },
        };
    uint32 i;
    MmpmMppsReqType mppsReq;

    clientId = Test_Register(coreId, instanceId, MIPS_04_NAME);

    if(clientId == 0)
        goto TestQ6AhbeScaling_error;

    for(i = 0; i < ADSPPM_ARRAY_SIZE(table); i++)
    {

        // Request DSP floor clock (adspFloor is in MHz units)
        mppsReq.mppsTotal = 0;
        mppsReq.adspFloorClock = (table[i].q6FreqHz / 1000000);
        sts = Test_RequestMpps(clientId, &mppsReq);
        if(MMPM_STATUS_SUCCESS != sts)
            goto TestQ6AhbeScaling_error;

        // Query AHB-E freq
        sts = Test_InfoAhb(coreId, instanceId, &ahbInfo);
        if(MMPM_STATUS_SUCCESS != sts)
            goto TestQ6AhbeScaling_error;

        ADSPPMTEST_LOG_PRINTF_2(ADSPPMTEST_LOG_LEVEL_INFO,
            "AHB-E: expected %u, actual %u Hz",
            table[i].ahbeFreqHz,
            ahbInfo.ahbeFreqHz);

        // Check expected AHB-E freq
        // Can only check if actual freq is too low, since other clients
        // may have MIPS/MPPS requests concurrently
        if(ahbInfo.ahbeFreqHz < table[i].ahbeFreqHz)
            goto TestQ6AhbeScaling_error;

    }

    sts = Test_Deregister(clientId);
    if(MMPM_STATUS_SUCCESS != sts)
    {
        retSts = MMPM_STATUS_FAILED;
    }

    return retSts;

TestQ6AhbeScaling_error:
    Test_Deregister(clientId);

    return MMPM_STATUS_FAILED;

}

/**
 * @ingroup MIPS
 * @test mpps_09
 * @brief Test scaling of AHB-E frequency with ADSP frequency
 */
static MMPM_STATUS Test_Mpps_09(void)
{

    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(MPPS_09_NAME, MPPS_09_DETAILS);

    sts = TestQ6AhbeScaling();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_MIPS, MPPS_09_NAME, sts);
    return sts;

}


#define MIPS_10_NAME    "mips_10"
#define MIPS_10_DETAILS "API latency for clock scaling"


MMPM_STATUS Test_RequestMips_Async(
    uint32 clientId,
    uint32 mips)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    MMPM_STATUS detailSts = MMPM_STATUS_FAILED;
    MmpmRscParamType mmpmRscParam;
    MmpmRscExtParamType mmpmRscExtParam;
    MmpmMipsReqType mmpmMipsParam;

    if(clientId)
    {
        mmpmMipsParam.mipsTotal = mips;
        mmpmMipsParam.mipsPerThread = mips/(AsicInfo_getMipsInfo()->numThreads);
        mmpmMipsParam.codeLocation = MMPM_BW_PORT_ID_ADSP_MASTER;
        mmpmMipsParam.reqOperation = MMPM_MIPS_REQUEST_CPU_CLOCK_ONLY;
        mmpmRscParam.rscId = MMPM_RSC_ID_MIPS_EXT;
        mmpmRscParam.rscParam.pMipsExt = &mmpmMipsParam;
        mmpmRscExtParam.apiType = MMPM_API_TYPE_ASYNC;
        mmpmRscExtParam.numOfReq = 1;
        mmpmRscExtParam.pReqArray = &mmpmRscParam;
        mmpmRscExtParam.pStsArray = &detailSts;
        PRINT_INFOEXT_2("clientID %u, mips %u", clientId, mips);

        sts = MMPM_Request_Ext(clientId, &mmpmRscExtParam);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            PRINT_ERROR_2("clientID %u, returned %u", clientId, detailSts);
        }
    }
    else
    {
        // This is an error in the test code
        PRINT_ERROR_0("Invalid ADSPPM client ID");
    }

    return sts;
}

static MMPM_STATUS TestQ6CoreClockSVSTURBO(MmpmApiType APIType)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    uint32 clientId = 0;
    uint32 mips, setClkFreq, readClkFreq;
    const AsicInfo_MipsInfoType *pMipsInfo = NULL;
    const AsicInfo_ClockFreqVoltPlanType *pQ6FreqPlan = NULL;
    MmpmCoreIdType coreId = MMPM_CORE_ID_LPASS_ADSP;
    MmpmCoreInstanceIdType instanceId = MMPM_CORE_INSTANCE_0;

    clientId = Test_Register(coreId, instanceId, MIPS_10_NAME);

    if(clientId)
    {
        pMipsInfo = AsicInfo_getMipsInfo();
        pQ6FreqPlan = AsicInfo_getClockFreqVoltPlan(MMPM_CLK_ID_LPASS_ADSP_CORE);


        //for SVS
        setClkFreq = pQ6FreqPlan->pFreqVoltPlan[1].frequency;
        mips = setClkFreq/1000000*256/pMipsInfo->mips2clockFactor;

        sts = Test_RequestMips(clientId, mips);

        if(MMPM_STATUS_SUCCESS != sts)
        {
            retSts = MMPM_STATUS_FAILED;
        }
        else
        {
            // Read Q6 clock frequency
            sts = Test_InfoClk(coreId, instanceId, MMPM_CLK_ID_LPASS_ADSP_CORE, &readClkFreq);
            if(MMPM_STATUS_SUCCESS != sts)
            {
                retSts = MMPM_STATUS_FAILED;
            }
            else
            {
                if(!Test_CompareFreq(setClkFreq, readClkFreq * 1000))
                {
                    ADSPPMTEST_LOG_PRINTF_2(
                        ADSPPMTEST_LOG_LEVEL_ERROR,
                        "[ERROR] setClkFreq = %u Hz, readClkFreq = %u Hz",
                        setClkFreq, readClkFreq * 1000);
                    retSts = MMPM_STATUS_FAILED;
                }
            }
        }

        //================================ moved to TURBO =================================//
        setClkFreq = pQ6FreqPlan->pFreqVoltPlan[pQ6FreqPlan->numFreq-1].frequency;
        mips = setClkFreq/1000000*256/pMipsInfo->mips2clockFactor;

        ADSPPMTEST_LOG_PRINTF_0(ADSPPMTEST_LOG_LEVEL_RESULT,"Test MIPS #10: SVS->TURBO starts.");

        if(APIType == MMPM_API_TYPE_ASYNC)
        {
            sts = Test_RequestMips_Async(clientId,mips);
        }
        else
        {
            sts = Test_RequestMips(clientId, mips);
        }

        ADSPPMTEST_LOG_PRINTF_0(ADSPPMTEST_LOG_LEVEL_RESULT,"Test MIPS #10: SVS->TURBO ends.");

        if(MMPM_STATUS_SUCCESS != sts)
        {
            retSts = MMPM_STATUS_FAILED;
        }
        else
        {
            // Read Q6 clock frequency
            sts = Test_InfoClk(coreId, instanceId, MMPM_CLK_ID_LPASS_ADSP_CORE, &readClkFreq);
            if(MMPM_STATUS_SUCCESS != sts)
            {
                retSts = MMPM_STATUS_FAILED;
            }
            else
            {
                if(!Test_CompareFreq(setClkFreq, readClkFreq * 1000))
                {
                    ADSPPMTEST_LOG_PRINTF_2(
                        ADSPPMTEST_LOG_LEVEL_ERROR,
                        "[ERROR] setClkFreq = %u Hz, readClkFreq = %u Hz",
                        setClkFreq, readClkFreq * 1000);
                    retSts = MMPM_STATUS_FAILED;
                }
            }
        }

        //================================ move back to SVS =================================//
        setClkFreq = pQ6FreqPlan->pFreqVoltPlan[1].frequency;
        mips = setClkFreq/1000000*256/pMipsInfo->mips2clockFactor;

        ADSPPMTEST_LOG_PRINTF_0(ADSPPMTEST_LOG_LEVEL_RESULT, "Test MIPS #10: TURBO->SVS level.");

        //Time stamp will be checked at call backed.
        if(APIType == MMPM_API_TYPE_ASYNC)
        {
            sts = Test_RequestMips_Async(clientId,mips);
        }
        else
        {
            sts = Test_RequestMips(clientId, mips);

            ADSPPMTEST_LOG_PRINTF_0(ADSPPMTEST_LOG_LEVEL_RESULT,"Test MIPS #10: TURBO->SVS level.");
        }

        if(MMPM_STATUS_SUCCESS != sts)
        {
            retSts = MMPM_STATUS_FAILED;
        }
        else
        {
            // Read Q6 clock frequency
            sts = Test_InfoClk(coreId, instanceId, MMPM_CLK_ID_LPASS_ADSP_CORE, &readClkFreq);
            if(MMPM_STATUS_SUCCESS != sts)
            {
                retSts = MMPM_STATUS_FAILED;
            }
            else
            {
                if(!Test_CompareFreq(setClkFreq, readClkFreq * 1000))
                {
                    ADSPPMTEST_LOG_PRINTF_2(
                        ADSPPMTEST_LOG_LEVEL_ERROR,
                        "[ERROR] setClkFreq = %u Hz, readClkFreq = %u Hz",
                        setClkFreq, readClkFreq * 1000);
                    retSts = MMPM_STATUS_FAILED;
                }
            }
        }

        if(APIType == MMPM_API_TYPE_ASYNC)
        {
            // Wait for 500 usec
            DALSYS_BusyWait(500);

            if (adsppmTest_Callback_Count() != 2)
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

/**
 * @ingroup MIPS
 * @test mips_05
 * @brief Q6 core clock frequency plan
 */
static MMPM_STATUS Test_Mips_10(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(MIPS_10_NAME, MIPS_10_DETAILS);

    sts = TestQ6CoreClockSVSTURBO(MMPM_API_TYPE_SYNC);

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_MIPS, MIPS_10_NAME, sts);
    return sts;
}


#define MIPS_11_NAME    "mips_11"
#define MIPS_11_DETAILS "API latency for clock scaling through Asynch Request"

/**
 * @ingroup MIPS
 * @test mips_05
 * @brief Q6 core clock frequency plan
 */
static MMPM_STATUS Test_Mips_11(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(MIPS_11_NAME, MIPS_11_DETAILS);

    sts = TestQ6CoreClockSVSTURBO(MMPM_API_TYPE_ASYNC);

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_MIPS, MIPS_11_NAME, sts);
    return sts;
}


#define MPPS_12_NAME    "mpps_12"
#define MPPS_12_DETAILS "Req and Test MPPS req from Audio & Voice Clients."

static MMPM_STATUS TestAudioVoiceMppsRequest(void)
{
    MMPM_STATUS retSts = MMPM_STATUS_SUCCESS;
    MmpmInfoMppsType mppsInfo;
    uint32 numReq = 11, i = 0, mppsTotal = 0;

    typedef struct {
        MmpmCoreIdType coreId;
        MmpmCoreInstanceIdType instanceId;
        uint32 clientId;
        char clientName[MAX_LEN_CLIENT_NAME];
        MmpmClientClassType clientClass;
        MmpmMppsReqType mppsReq;
    } mppsReqArrayType;

    mppsReqArrayType req[] =
    {
        {
            MMPM_CORE_ID_LPASS_ADSP, MMPM_CORE_INSTANCE_0,
            0, "MXAR", MMPM_AUDIO_CLIENT_CLASS,
            {7, 0}
        },
        {
            MMPM_CORE_ID_LPASS_ADSP, MMPM_CORE_INSTANCE_0,
            0, "ADM", MMPM_AUDIO_CLIENT_CLASS,
            {19, 0}
        },
        {
            MMPM_CORE_ID_LPASS_ADSP, MMPM_CORE_INSTANCE_0,
            0, "MVM_CCM", MMPM_VOICE_CLIENT_CLASS,
            {102, 0}
        },
        {
            MMPM_CORE_ID_LPASS_ADSP, MMPM_CORE_INSTANCE_0,
            0, "Afe[4000]", MMPM_AUDIO_CLIENT_CLASS,
            {30, 0}
        },
        {
            MMPM_CORE_ID_LPASS_ADSP, MMPM_CORE_INSTANCE_0,
            0, "Afe[4001]", MMPM_AUDIO_CLIENT_CLASS,
            {2, 0}
        },
        {
            MMPM_CORE_ID_LPASS_ADSP, MMPM_CORE_INSTANCE_0,
            0, "ADec1b", MMPM_AUDIO_CLIENT_CLASS,
            {23, 0}
        },
        {
            MMPM_CORE_ID_LPASS_ADSP, MMPM_CORE_INSTANCE_0,
            0, "AA1E", MMPM_AUDIO_CLIENT_CLASS,
            {7, 0}
        },
        {
            MMPM_CORE_ID_LPASS_ADSP, MMPM_CORE_INSTANCE_0,
            0, "CC20", MMPM_AUDIO_CLIENT_CLASS,
            {7, 0}
        },
        {
            MMPM_CORE_ID_LPASS_ADSP, MMPM_CORE_INSTANCE_0,
            0, "ADec22", MMPM_AUDIO_CLIENT_CLASS,
            {6, 0}
        },
        {
            MMPM_CORE_ID_LPASS_ADSP, MMPM_CORE_INSTANCE_0,
            0, "AA25", MMPM_AUDIO_CLIENT_CLASS,
            {3, 0}
        },
        {
            MMPM_CORE_ID_LPASS_ADSP, MMPM_CORE_INSTANCE_0,
            0, "AEnc", MMPM_AUDIO_CLIENT_CLASS,
            {4, 0}
        },
    };

    for (i = 0; i < numReq; i++) {
        req[i].clientId = Test_Register(req[i].coreId, req[i].instanceId,
                                        req[i].clientName);
        if (!req[i].clientId) {
            PRINT_ERROR_1("Failed to register client %s", req[i].clientName);
            retSts = MMPM_STATUS_FAILED;
            goto dereg;
        }

        if (Test_SetClientClass(req[i].clientId, req[i].clientClass) !=
                MMPM_STATUS_SUCCESS) {
            PRINT_ERROR_1("Failed to set client class for %s",
                            req[i].clientName);
            retSts = MMPM_STATUS_FAILED;
            goto dereg;
        }

        if (Test_RequestMpps(req[i].clientId, &req[i].mppsReq) !=
                MMPM_STATUS_SUCCESS) {
            PRINT_ERROR_1("Failed MPPS req from client %s",
                            req[i].clientName);
            retSts = MMPM_STATUS_FAILED;
            goto release;
        }
        mppsTotal += req[i].mppsReq.mppsTotal;
    }

    mppsInfo.clientClasses = MMPM_AUDIO_CLIENT_CLASS |
                                MMPM_VOICE_CLIENT_CLASS |
                                MMPM_COMPUTE_CLIENT_CLASS |
                                MMPM_STREAMING_1HVX_CLIENT_CLASS |
                                MMPM_STREAMING_2HVX_CLIENT_CLASS;
    mppsInfo.aggregateMpps = 0;
    if (Test_InfoMpps(&mppsInfo) != MMPM_STATUS_SUCCESS)
        PRINT_ERROR_0("Failed to get MPPS info");

    PRINT_INFO_2("MPPS: Request Total = %d; Aggregated Total = %d",
                mppsTotal, mppsInfo.aggregateMpps);

release:
    for (i = 0; i < numReq; i++)
        if (req[i].clientId)
            Test_ReleaseResource(req[i].clientId, MMPM_RSC_ID_MPPS);
dereg:
    for (i = 0; i < numReq; i++)
        if (req[i].clientId)
            Test_Deregister(req[i].clientId);

    return retSts;
}

/**
 * @ingroup MIPS
 * @test mpps_12
 * @brief Req and Test MPPS req from Audio & Voice Clients.
 */
static MMPM_STATUS Test_Mpps_12(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(MPPS_12_NAME, MPPS_12_DETAILS);

    sts = TestAudioVoiceMppsRequest();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_MIPS, MPPS_12_NAME, sts);
    return sts;
}

void Test_Mips()
{
    // Req-Rel, all core/instance combination
    Test_Mips_01();

    // Req-Rel, 2 same coreID/instanceID clients, expect aggregated MIPS
    Test_Mips_02();

    // Req-Rel-Rel, expect NO errors
    Test_Mips_03();

    // Invalid client ID Req, expect failure Req
    Test_Mips_04();

    // Q6 core clock frequency plan
    Test_Mips_05();

    // Very large and casted negative MIPS value should result in highest Q6 freq
    Test_Mips_06();

    // Req-Rel all core/instances combination for MPPS
    Test_Mpps_07();

    // Test MPPS query
    Test_Mpps_08();

    // Test scaling of AHB-E frequency with ADSP frequency
    Test_Mpps_09();

    // Test latency
    Test_Mips_10();

    // Test latency
    //Test_Mips_11();
    // Test MPPS request for specific MHz o/p
    Test_Mpps_12();

}


/***************************************************************************/
/****************** Test Code for CDSP is in below section *****************/
/***************************************************************************/

/******************* UTILITIES FUNCTION ******************/
/*********************************************************/

static MMPM_STATUS TestCDSPCoresMipsRequest(MmpmCoreIdType coreId)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    uint32 clientId;

    clientId = Test_Register(coreId, MMPM_CORE_INSTANCE_0, "CDSP_MIPS");
    if(clientId)
    {
        sts = Test_RequestMips(clientId, 200);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            retSts = MMPM_STATUS_FAILED;
        }
        else
        {
            sts = Test_ReleaseResource(clientId, MMPM_RSC_ID_MIPS_EXT);
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

/**
 * @ingroup MIPS
 * @test cdsp_mips_01
 * @brief Req-Rel, all core/instance combination
 */
#define CDSP_MIPS_01_NAME "cdsp_mips_01"
#define CDSP_MIPS_01_DETAILS "test all cdsp cores mips request/release"

static void Test_CDSP_Mips_01(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(CDSP_MIPS_01_NAME, CDSP_MIPS_01_DETAILS);

    sts = TestCDSPCoresMipsRequest(MMPM_CORE_ID_Q6_DSP);

    if(MMPM_STATUS_SUCCESS == sts)
        sts = TestCDSPCoresMipsRequest(MMPM_CORE_ID_COMPUTE_HCP);

    if(MMPM_STATUS_SUCCESS == sts)
        sts = TestCDSPCoresMipsRequest(MMPM_CORE_ID_COMPUTE_DMA);


    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_MIPS, CDSP_MIPS_01_NAME, sts);
    return;
}



void Test_CDSP_Mips(void)
{
  Test_CDSP_Mips_01();
}

/***************************************************************************/
/****************** Test Code for SLPI is in below section *****************/
/***************************************************************************/

/******************* UTILITIES FUNCTION ******************/
/*********************************************************/

static MMPM_STATUS TestSlpiCoresMipsRequest(MmpmCoreIdType coreId)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    uint32 clientId;

    clientId = Test_Register(coreId, MMPM_CORE_INSTANCE_0, "SLPI_MIPS");
    if(clientId)
    {
        sts = Test_RequestMips(clientId, 300);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            retSts = MMPM_STATUS_FAILED;
        }
        else
        {
            sts = Test_ReleaseResource(clientId, MMPM_RSC_ID_MIPS_EXT);
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

/**
 * @ingroup MIPS
 * @test cdsp_mips_01
 * @brief Req-Rel, all core/instance combination
 */
#define SLPI_MIPS_01_NAME "slpi_mips_01"
#define SLPI_MIPS_01_DETAILS "test Q6 core mips request/release"

static void Test_Slpi_Mips_01(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(SLPI_MIPS_01_NAME, SLPI_MIPS_01_DETAILS);

    sts = TestSlpiCoresMipsRequest(MMPM_CORE_ID_Q6_DSP);

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_MIPS, SLPI_MIPS_01_NAME, sts);
    return;
}



void Test_Slpi_Mips(void)
{
  Test_Slpi_Mips_01();
}

