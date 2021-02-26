/*
* Copyright (c) 2015 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
@file: adsppm_test_system.c
@brief: Implementation of ADSPPM test module of generic calls to MMPM.
        Generic calls include registration, de-registration, get info.

$Header: //components/rel/core.qdsp6/2.1.c4/power/qdsp_pm/src/driver/test/adsppm_test_system.c#1 $
*/

#include "mmpm.h"
#include "adsppm_test.h"
#include "adsppm_test_param.h"
#include "adsppm_test_utils.h"
#include "adsppm_test_asic_info.h"
#include "DALSys.h"


#define SYSTEM_01_NAME    "system_01"
#define SYSTEM_01_DETAILS "Reg-Dreg, all core/instance combination"

static MMPM_STATUS TestRegDregAllCoreInstance(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    MmpmCoreIdType coreId;
    MmpmCoreInstanceIdType instanceId;
    uint32 clientId = 0;

    for(coreId = MMPM_CORE_ID_LPASS_ADSP; coreId < MMPM_CORE_ID_LPASS_END; coreId++)
    {
        for(instanceId = MMPM_CORE_INSTANCE_0; instanceId < MMPM_CORE_INSTANCE_MAX; instanceId++)
        {
            if(AsicInfo_isCoreInstanceAvailable(coreId, instanceId))
            {
                clientId = Test_Register(coreId, instanceId, SYSTEM_01_NAME);
                if(clientId)
                {
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
 * @ingroup System
 * @test system_01
 * @brief Reg-Dreg, all core/instance combination
 */
static MMPM_STATUS Test_System_01(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(SYSTEM_01_NAME, SYSTEM_01_DETAILS);

    sts = TestRegDregAllCoreInstance();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_System, SYSTEM_01_NAME, sts);
    return sts;
}


#define SYSTEM_02_NAME    "system_02"
#define SYSTEM_02_DETAILS "Invalid parameters Reg, expect failure Reg"

static MMPM_STATUS TestSysBadDereg(void)
{
    MMPM_STATUS retSts = MMPM_STATUS_SUCCESS;
    uint32 clientId = 0;

    // MMPM_CORE_ID_NONE and MMPM_CORE_INSTANCE_NONE
    ADSPPMTEST_LOG_PRINTF_0(
        ADSPPMTEST_LOG_LEVEL_INFO,
        "[INFO] Test enum none coreID and enum none instanceID");
    clientId = Test_Register(
        MMPM_CORE_ID_NONE, MMPM_CORE_INSTANCE_NONE, SYSTEM_02_NAME);
    if(clientId)
    {
        ADSPPMTEST_LOG_PRINTF_0(
            ADSPPMTEST_LOG_LEVEL_ERROR,
            "[ERROR] Request did not fail as intended");
        Test_Deregister(clientId);
        retSts = MMPM_STATUS_FAILED;
    }
    else
    {
        ADSPPMTEST_LOG_PRINTF_0(
            ADSPPMTEST_LOG_LEVEL_ERROR,
            "[PASS] Failed return is intended");
    }

    // MMPM_CORE_ID_MAX and MMPM_CORE_INSTANCE_MAX
    ADSPPMTEST_LOG_PRINTF_0(
        ADSPPMTEST_LOG_LEVEL_INFO,
        "[INFO] Test enum max coreID and enum max instanceID");
    clientId = Test_Register(
        MMPM_CORE_ID_MAX, MMPM_CORE_INSTANCE_MAX, SYSTEM_02_NAME);
    if(clientId)
    {
        ADSPPMTEST_LOG_PRINTF_0(
            ADSPPMTEST_LOG_LEVEL_ERROR,
            "[ERROR] Request did not fail as intended");
        Test_Deregister(clientId);
        retSts = MMPM_STATUS_FAILED;
    }
    else
    {
        ADSPPMTEST_LOG_PRINTF_0(
            ADSPPMTEST_LOG_LEVEL_ERROR,
            "[PASS] Failed return is intended");
    }

    // Invalid coreID
    ADSPPMTEST_LOG_PRINTF_0(
        ADSPPMTEST_LOG_LEVEL_INFO,
        "[INFO] Test invalid coreID");
    clientId = Test_Register(
        MMPM_CORE_ID_MDP, MMPM_CORE_INSTANCE_0, SYSTEM_02_NAME);
    if(clientId)
    {
        ADSPPMTEST_LOG_PRINTF_0(
            ADSPPMTEST_LOG_LEVEL_ERROR,
            "[ERROR] Request did not fail as intended");
        Test_Deregister(clientId);
        retSts = MMPM_STATUS_FAILED;
    }
    else
    {
        ADSPPMTEST_LOG_PRINTF_0(
            ADSPPMTEST_LOG_LEVEL_ERROR,
            "[PASS] Failed return is intended");
    }

    // Invalid instanceID
    ADSPPMTEST_LOG_PRINTF_0(
        ADSPPMTEST_LOG_LEVEL_INFO,
        "[INFO] Test invalid instanceID");
    clientId = Test_Register(
        MMPM_CORE_ID_LPASS_ADSP, MMPM_CORE_INSTANCE_MAX, SYSTEM_02_NAME);
    if(clientId)
    {
        ADSPPMTEST_LOG_PRINTF_0(
            ADSPPMTEST_LOG_LEVEL_ERROR,
            "[ERROR] Request did not fail as intended");
        Test_Deregister(clientId);
        retSts = MMPM_STATUS_FAILED;
    }
    else
    {
        ADSPPMTEST_LOG_PRINTF_0(
            ADSPPMTEST_LOG_LEVEL_ERROR,
            "[PASS] Failed return is intended");
    }

    return retSts;
}

/**
 * @ingroup System
 * @test system_02
 * @brief Invalid parameters Reg, expect failure Reg
 */
static MMPM_STATUS Test_System_02(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(SYSTEM_02_NAME, SYSTEM_02_DETAILS);

    sts = TestSysBadDereg();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_System, SYSTEM_02_NAME, sts);
    return sts;
}


#define SYSTEM_03_NAME    "system_03"
#define SYSTEM_03_DETAILS "Invalid client ID Dreg, expect failure Dreg"

static MMPM_STATUS TestSystemDregBadClientId(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    uint32 clientId1 = 0, clientId2 = 0;
    MmpmCoreIdType coreId = MMPM_CORE_ID_LPASS_ADSP;
    MmpmCoreInstanceIdType instanceId = MMPM_CORE_INSTANCE_0;

    clientId1 = Test_Register(coreId, instanceId, SYSTEM_03_NAME"_1");
    clientId2 = Test_Register(coreId, instanceId, SYSTEM_03_NAME"_2");

    if(clientId1 && clientId2)
    {
        sts = Test_Deregister(clientId1);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            retSts = MMPM_STATUS_FAILED;
        }
        else
        {
            ADSPPMTEST_LOG_PRINTF_0(
                ADSPPMTEST_LOG_LEVEL_INFO,
                "[INFO] Test invalid clientID");
            sts = Test_Deregister(clientId1);
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
 * @ingroup System
 * @test system_03
 * @brief Invalid client ID Dreg, expect failure Dreg
 */
static MMPM_STATUS Test_System_03(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(SYSTEM_03_NAME, SYSTEM_03_DETAILS);

    sts = TestSystemDregBadClientId();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_System, SYSTEM_03_NAME, sts);
    return sts;
}


#define SYSTEM_04_NAME    "system_04"
#define SYSTEM_04_DETAILS "Test MMPM_GetInfo API"

static MMPM_STATUS TestSystemGetInfo(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    uint32 clkFreq = 0;
    uint32 clientId = 0;

    //
    // Get DSP core frequency
    //

    sts = Test_InfoClk(
        MMPM_CORE_ID_LPASS_ADSP,
        MMPM_CORE_INSTANCE_0,
        MMPM_CLK_ID_LPASS_ADSP_CORE,
        &clkFreq);
    if(MMPM_STATUS_SUCCESS != sts)
    {
        goto TestSystemGetInfo_error;
    }
    else
    {
        if(clkFreq == 0)
        {
            goto TestSystemGetInfo_error;
        }
    }

    return sts;

TestSystemGetInfo_error:

    if(clientId != 0)
        Test_Deregister(clientId);

    return MMPM_STATUS_FAILED;

}

/**
 * @ingroup System
 * @test system_04
 * @brief Test MMPM_GetInfo API
 */
static MMPM_STATUS Test_System_04(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(SYSTEM_04_NAME, SYSTEM_04_DETAILS);

    sts = TestSystemGetInfo();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_System, SYSTEM_04_NAME, sts);
    return sts;
}


#define SYSTEM_05_NAME    "system_05"
#define SYSTEM_05_DETAILS "Test callback from asynchronous Req after a client de-registers"

static uint32 callbackCount = 0;

uint32 ADSPPMTEST_System05Callback(MmpmCallbackParamType *pCbParam)
{
    callbackCount++;
    ADSPPMTEST_LOG_PRINTF_1(ADSPPMTEST_LOG_LEVEL_INFO,
        "[INFO] Callback successful, callbackCount(%u)",
        callbackCount);
    return 0;
}

static MMPM_STATUS TestCallback(void)
{

    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    MMPM_STATUS detailSts = MMPM_STATUS_FAILED;
    MmpmRegParamType mmpmRegParam;
    MmpmRscParamType mmpmRscParam;
    MmpmRscExtParamType mmpmRscExtParam;
    uint32 clientId1 = 0;
    uint32 clientId2 = 0;

    mmpmRegParam.rev = MMPM_REVISION;
    mmpmRegParam.coreId = MMPM_CORE_ID_LPASS_CORE;
    mmpmRegParam.instanceId = MMPM_CORE_INSTANCE_0;
    mmpmRegParam.pClientName = SYSTEM_05_NAME;
    mmpmRegParam.pwrCtrlFlag = PWR_CTRL_NONE;
    mmpmRegParam.callBackFlag = CALLBACK_REQUEST_COMPLETE;
    mmpmRegParam.MMPM_Callback = ADSPPMTEST_System05Callback;

    mmpmRscParam.rscId = MMPM_RSC_ID_POWER;
    mmpmRscExtParam.apiType = MMPM_API_TYPE_ASYNC;
    mmpmRscExtParam.numOfReq = 1;
    mmpmRscExtParam.pReqArray = &mmpmRscParam;
    mmpmRscExtParam.pStsArray = &detailSts;

    // Register first client with callback defined for this test
    clientId1 = MMPM_Register_Ext(&mmpmRegParam);
    if(clientId1 == 0)
        goto TestCallback_error;

    // Register second client with callback defined for this test
    clientId2 = MMPM_Register_Ext(&mmpmRegParam);
    if(clientId2 == 0)
        goto TestCallback_error;

    // De-register second client
    sts = Test_Deregister(clientId2);
    if(sts != MMPM_STATUS_SUCCESS)
        goto TestCallback_error;
    clientId2 = 0;

    // Request resource
    sts = MMPM_Request_Ext(clientId1, &mmpmRscExtParam);
    if(sts != MMPM_STATUS_SUCCESS)
        goto TestCallback_error;

    // Wait for 500 usec
    DALSYS_BusyWait(500);

    // Check callback counts
    if(callbackCount != 1)
    {
        ADSPPMTEST_LOG_PRINTF_1(ADSPPMTEST_LOG_LEVEL_ERROR,
            "[ERROR] callback after request did not happen, callbackCount(%u)",
            callbackCount);
        goto TestCallback_error;
    }

    // Release resource
    sts = MMPM_Release_Ext(clientId1, &mmpmRscExtParam);
    if(sts != MMPM_STATUS_SUCCESS)
        goto TestCallback_error;

    // Wait for 500 usec
    DALSYS_BusyWait(500);

    // Check callback counts
    if(callbackCount != 2)
    {
        ADSPPMTEST_LOG_PRINTF_1(ADSPPMTEST_LOG_LEVEL_ERROR,
            "[ERROR] callback after release did not happen, callbackCount(%u)",
            callbackCount);
        goto TestCallback_error;
    }

    // De-register first client
    sts = Test_Deregister(clientId1);
    if(sts != MMPM_STATUS_SUCCESS)
        goto TestCallback_error;

    return MMPM_STATUS_SUCCESS;

TestCallback_error:

    if(clientId1 != 0)
        Test_Deregister(clientId1);

    if(clientId2 != 0)
        Test_Deregister(clientId2);

    return MMPM_STATUS_FAILED;

}

/**
 * @ingroup System
 * @test system_05
 * @brief Test callback from asynchronous Req after a client de-registers
 */
static MMPM_STATUS Test_System_05(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(SYSTEM_05_NAME, SYSTEM_05_DETAILS);

    sts = TestCallback();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_System, SYSTEM_05_NAME, sts);
    return sts;
}


void Test_System(void)
{
    // Reg-Dreg, all core/instance combination
    Test_System_01();

    // Invalid parameters Reg, expect failure Reg
    Test_System_02();

    // Invalid client ID Dreg, expect failure Dreg
    Test_System_03();

    // Test MMPM_GetInfo API
    Test_System_04();

    // Test callback from asynchronous Req after a client de-registers
    //Test_System_05();
}



/***************************************************************************/
/****************** Test Code for CDSP is in below section *****************/
/***************************************************************************/

/*********** COMMON FUNCTIONS *************/


#define CDSP_SYSTEM_01_NAME    "cdsp_system_01"
#define CDSP_SYSTEM_01_DETAILS "Reg-Dreg, all core combination"
#define CDSP_SYSTEM_01_ID      0x00C10001
static MMPM_STATUS Test_CDSP_System_Reg_DeReg(MmpmCoreIdType coreId)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    uint32 clientId = 0;
    ADSPPMTEST_LOG_TEST_HEADER(CDSP_SYSTEM_01_NAME, CDSP_SYSTEM_01_DETAILS);


    clientId = Test_Register(coreId, MMPM_CORE_INSTANCE_0, CDSP_SYSTEM_01_NAME);
    if(clientId)
    {
       sts = Test_Deregister(clientId);
       if(MMPM_STATUS_SUCCESS != sts)
       {
           sts = MMPM_STATUS_FAILED;
       }
    }
    else
    {
        sts = MMPM_STATUS_FAILED;
    }

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_System, CDSP_SYSTEM_01_NAME, sts);
    return sts;
}


void Test_CDSP_System(void)
{
    Test_CDSP_System_Reg_DeReg(MMPM_CORE_ID_COMPUTE_HCP);
    Test_CDSP_System_Reg_DeReg(MMPM_CORE_ID_COMPUTE_DMA);
}

/***************************************************************************/
/****************** Test Code for SLPI is in below section *****************/
/***************************************************************************/

/*********** COMMON FUNCTIONS *************/


#define SLPI_SYSTEM_01_NAME    "slpi_system_01"
#define SLPI_SYSTEM_01_DETAILS "Reg-Dreg, all core combination"
#define SLPI_SYSTEM_01_ID      0x00C10001
static MMPM_STATUS Test_Slpi_System_Reg_DeReg(MmpmCoreIdType coreId)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    uint32 clientId = 0;
    ADSPPMTEST_LOG_TEST_HEADER(SLPI_SYSTEM_01_NAME, SLPI_SYSTEM_01_DETAILS);


    clientId = Test_Register(coreId, MMPM_CORE_INSTANCE_0, SLPI_SYSTEM_01_NAME);
    if(clientId)
    {
       sts = Test_Deregister(clientId);
       if(MMPM_STATUS_SUCCESS != sts)
       {
           sts = MMPM_STATUS_FAILED;
       }
    }
    else
    {
        sts = MMPM_STATUS_FAILED;
    }

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_System, SLPI_SYSTEM_01_NAME, sts);
    return sts;
}


void Test_Slpi_System(void)
{
    Test_Slpi_System_Reg_DeReg(MMPM_CORE_ID_SLPI_SDC);
    Test_Slpi_System_Reg_DeReg(MMPM_CORE_ID_SLPI_QUP);
    Test_Slpi_System_Reg_DeReg(MMPM_CORE_ID_SLPI_SRAM);
    Test_Slpi_System_Reg_DeReg(MMPM_CORE_ID_SLPI_CCD);

}

