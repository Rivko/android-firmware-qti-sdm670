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


#define POWER_01_NAME    "power_01"
#define POWER_01_DETAILS "Req-Rel, all core/instance combination"

static MMPM_STATUS TestAllCoresPowerRequest(void)
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
                clientId = Test_Register(coreId, instanceId, POWER_01_NAME);
                if(clientId)
                {
                    sts = Test_RequestPower(clientId);
                    if(MMPM_STATUS_SUCCESS != sts)
                    {
                        retSts = MMPM_STATUS_FAILED;
                    }
                    else
                    {
                        sts = Test_ReleaseResource(clientId, MMPM_RSC_ID_POWER);
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
 * @ingroup Power
 * @test power_01
 * @brief Req-Rel, all core/instance combination
 */
static MMPM_STATUS Test_Power_01(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(POWER_01_NAME, POWER_01_DETAILS);

    sts = TestAllCoresPowerRequest();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_Power, POWER_01_NAME, sts);
    return sts;
}


#define POWER_02_NAME    "power_02"
#define POWER_02_DETAILS "Req-Rel, 2 same ADSP coreID/instanceID clients"

static AdsppmTestType testpower02[] =
{
    {1, 2, MMPM_CORE_ID_LPASS_ADSP, MMPM_CORE_INSTANCE_0,
        {REG, REQPWR}
    },
    {2, 2, MMPM_CORE_ID_LPASS_ADSP, MMPM_CORE_INSTANCE_0,
        {REG, REQPWR}
    },
    {1, 2, MMPM_CORE_ID_LPASS_ADSP, MMPM_CORE_INSTANCE_0,
        {RELPWR, DREG}
    },
    {2, 2, MMPM_CORE_ID_LPASS_ADSP, MMPM_CORE_INSTANCE_0,
        {RELPWR, DREG}
    }
};

/**
 * @ingroup Power
 * @test power_02
 * @brief Req-Rel, 2 same ADSP coreID/instanceID clients
 */
static MMPM_STATUS Test_Power_02(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, testSts[MAX_TEST][MAX_TEST_SEQUENCE];
    uint32 numTest = 0;
    ADSPPMTEST_LOG_TEST_HEADER(POWER_02_NAME, POWER_02_DETAILS);

    numTest = sizeof(testpower02)/sizeof(AdsppmTestType);
    sts = Test_InvokeTest(testpower02, numTest, testSts);

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_Power, POWER_02_NAME, sts);
    return sts;
}


#define POWER_03_NAME    "power_03"
#define POWER_03_DETAILS "Req-Rel, 2 same LPASS Core coreID/instanceID clients"

static AdsppmTestType testpower03[] =
{
    {1, 2, MMPM_CORE_ID_LPASS_DML, MMPM_CORE_INSTANCE_0,
        {REG, REQPWR}
    },
    {2, 2, MMPM_CORE_ID_LPASS_DML, MMPM_CORE_INSTANCE_0,
        {REG, REQPWR}
    },
    {1, 2, MMPM_CORE_ID_LPASS_DML, MMPM_CORE_INSTANCE_0,
        {RELPWR, DREG}
    },
    {2, 2, MMPM_CORE_ID_LPASS_DML, MMPM_CORE_INSTANCE_0,
        {RELPWR, DREG}
    }
};

/**
 * @ingroup Power
 * @test power_03
 * @brief Req-Rel, 2 same LPASS Core coreID/instanceID clients
 */
static MMPM_STATUS Test_Power_03(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, testSts[MAX_TEST][MAX_TEST_SEQUENCE];
    uint32 numTest = 0;
    ADSPPMTEST_LOG_TEST_HEADER(POWER_03_NAME, POWER_03_DETAILS);

    numTest = sizeof(testpower03)/sizeof(AdsppmTestType);
    sts = Test_InvokeTest(testpower03, numTest, testSts);

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_Power, POWER_03_NAME, sts);
    return sts;
}


#define POWER_04_NAME    "power_04"
#define POWER_04_DETAILS "Req-Rel-Rel,expect NO errors"

static AdsppmTestType testpower04[] =
{
    {1, 5, MMPM_CORE_ID_LPASS_ADSP, MMPM_CORE_INSTANCE_0,
        {REG, REQPWR, RELPWR, RELPWR, DREG}
    },
    {2, 5, MMPM_CORE_ID_LPASS_DML, MMPM_CORE_INSTANCE_0,
        {REG, REQPWR, RELPWR, RELPWR, DREG}
    }
};

/**
 * @ingroup Power
 * @test power_04
 * @brief Req-Rel-Rel,expect NO errors
 */
static MMPM_STATUS Test_Power_04(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, testSts[MAX_TEST][MAX_TEST_SEQUENCE];
    uint32 numTest = 0;
    ADSPPMTEST_LOG_TEST_HEADER(POWER_04_NAME, POWER_04_DETAILS);

    numTest = sizeof(testpower04)/sizeof(AdsppmTestType);
    sts = Test_InvokeTest(testpower04, numTest, testSts);

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_Power, POWER_04_NAME, sts);
    return sts;
}


#define POWER_05_NAME    "power_05"
#define POWER_05_DETAILS "Invalid client ID Req, expect failure Req"

static MMPM_STATUS TestPowerInvalidClientId(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    uint32 clientId = 0;
    MmpmCoreIdType coreId = MMPM_CORE_ID_LPASS_ADSP;
    MmpmCoreInstanceIdType instanceId = MMPM_CORE_INSTANCE_0;

    clientId = Test_Register(coreId, instanceId, POWER_05_NAME);

    if(clientId)
    {
        sts = Test_Deregister(clientId);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            retSts = MMPM_STATUS_FAILED;
        }

        sts = Test_RequestPower(clientId);
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
 * @ingroup Power
 * @test power_05
 * @brief Invalid client ID Req, expect failure Req
 */
static MMPM_STATUS Test_Power_05(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(POWER_05_NAME, POWER_05_DETAILS);

    sts = TestPowerInvalidClientId();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_Power, POWER_05_NAME, sts);
    return sts;
}


void Test_Power(void)
{
    // Req-Rel, all core/instance combination
    Test_Power_01();

    // Req-Rel, 2 same ADSP coreID/instanceID clients
    Test_Power_02();

    // Req-Rel, 2 same LPASS Core coreID/instanceID clients
    Test_Power_03();

    // Req-Rel-Rel,expect NO errors
    Test_Power_04();

    // Invalid client ID Req, expect failure Req
    Test_Power_05();
}


/***************************************************************************/
/****************** Test Code for CDSP is in below section *****************/
/***************************************************************************/

/******************* UTILITIES FUNCTION ******************/
/*********************************************************/



#define CDSP_POWER_01_NAME    "cdsp_power_01"
#define CDSP_POWER_01_DETAILS "Reg-Dreg, all core combination"
#define CDSP_POWER_01_ID      0x00C20001

static MMPM_STATUS TestUtil_CDSPPowerRequest(MmpmCoreIdType coreId)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    uint32 clientId;

    clientId = Test_Register(coreId, MMPM_CORE_INSTANCE_0, "POWER_TEST");
    if(clientId)
    {
        sts = Test_RequestPower(clientId);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            retSts = MMPM_STATUS_FAILED;
        }
        else
        {
            sts = Test_ReleaseResource(clientId, MMPM_RSC_ID_POWER);
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

static void Test_CDSP_Power_Req_Rel(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(CDSP_POWER_01_NAME, CDSP_POWER_01_DETAILS);

    sts = TestUtil_CDSPPowerRequest(MMPM_CORE_ID_COMPUTE_HCP);
    if(MMPM_STATUS_SUCCESS == sts)
        sts = TestUtil_CDSPPowerRequest(MMPM_CORE_ID_COMPUTE_DMA);

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_Power, CDSP_POWER_01_NAME, sts);
}

void Test_CDSP_Power( void )
{
   Test_CDSP_Power_Req_Rel();
}
