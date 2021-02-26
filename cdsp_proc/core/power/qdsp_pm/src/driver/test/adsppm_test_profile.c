/*
* Copyright (c) 2014 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

#include <limits.h>
#include "mmpm.h"
#include "adsppm_test.h"
#include "adsppm_test_param.h"
#include "adsppm_test_utils.h"
#include "adsppm_test_asic_info.h"


#define PROFILE_01_NAME    "profile_01"
#define PROFILE_01_DETAILS "Profile register/deregister duration"

static MMPM_STATUS TestProfileRegDereg(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    uint32 clientId = 0;
    MmpmCoreIdType coreId = MMPM_CORE_ID_LPASS_ADSP;
    MmpmCoreInstanceIdType instanceId = MMPM_CORE_INSTANCE_0;

    MmpmRegParamType mmpmRegParam;

    mmpmRegParam.rev = MMPM_REVISION;
    mmpmRegParam.coreId = coreId;
    mmpmRegParam.instanceId = instanceId;
    mmpmRegParam.pClientName = PROFILE_01_NAME;
    mmpmRegParam.pwrCtrlFlag = PWR_CTRL_NONE;
    mmpmRegParam.callBackFlag = CALLBACK_NONE;
    mmpmRegParam.MMPM_Callback = NULL;

    // Profile register duration
    ADSPPMTEST_LOG_PRINTF_0(
        ADSPPMTEST_LOG_LEVEL_RESULT, "[INFO] Profile register duration");

    clientId = MMPM_Register_Ext(&mmpmRegParam);

    if(clientId == 0)
    {
        ADSPPMTEST_LOG_PRINTF_0(
            ADSPPMTEST_LOG_LEVEL_ERROR, "[ERROR] register failed");
        retSts = MMPM_STATUS_FAILED;
    }

    // Profile deregister duration
    ADSPPMTEST_LOG_PRINTF_0(
        ADSPPMTEST_LOG_LEVEL_RESULT, "[INFO] Profile deregister duration");

    sts = MMPM_Deregister_Ext(clientId);

    if(MMPM_STATUS_SUCCESS != sts)
    {
        ADSPPMTEST_LOG_PRINTF_0(
            ADSPPMTEST_LOG_LEVEL_ERROR, "[ERROR] deregister failed");
        retSts = MMPM_STATUS_FAILED;
    }

    return retSts;
}

/**
 * @ingroup Profile
 * @test profile_01
 * @brief Profile register/deregister duration
 */
static MMPM_STATUS Test_Profile_01(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(PROFILE_01_NAME, PROFILE_01_DETAILS);
    ADSPPMTEST_LOG_PROFILESTART(PROFILE_01_NAME);

    sts = TestProfileRegDereg();

    ADSPPMTEST_LOG_PROFILEEND(PROFILE_01_NAME);
    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_Profile, PROFILE_01_NAME, sts);
    return sts;
}


#define PROFILE_02_NAME    "profile_02"
#define PROFILE_02_DETAILS "Profile Core Clock request duration"

static MMPM_STATUS TestProfileCoreClockReq(uint32 clientId)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;

    MMPM_STATUS detailSts = MMPM_STATUS_FAILED;
    MmpmRscParamType mmpmRscParam;
    MmpmRscExtParamType mmpmRscExtParam;
    MmpmClkValType mmpmClkValType;
    MmpmClkReqType mmpmClkReqType;


    if(clientId)
    {
        mmpmClkValType.clkId.clkIdLpass = MMPM_CLK_ID_LPASS_HWRSP_CORE;
        mmpmClkValType.freqMatch = MMPM_FREQ_AT_LEAST;

        mmpmClkReqType.numOfClk = 1;
        mmpmClkReqType.pClkArray = &mmpmClkValType;
        mmpmRscParam.rscId = MMPM_RSC_ID_CORE_CLK;
        mmpmRscParam.rscParam.pCoreClk = &mmpmClkReqType;
        mmpmRscExtParam.apiType = MMPM_API_TYPE_SYNC;
        mmpmRscExtParam.numOfReq = 1;
        mmpmRscExtParam.pReqArray = &mmpmRscParam;
        mmpmRscExtParam.pStsArray = &detailSts;

        // Profile request duration with NO voltage level change
        ADSPPMTEST_LOG_PRINTF_0(
            ADSPPMTEST_LOG_LEVEL_RESULT,
            "[INFO] Profile Core Clock request duration with NO voltage level change");

        mmpmClkValType.clkFreqHz = 19000000;

        sts = MMPM_Request_Ext(clientId, &mmpmRscExtParam);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            ADSPPMTEST_LOG_PRINTF_0(
                ADSPPMTEST_LOG_LEVEL_ERROR, "[ERROR] Request failed");
            retSts = MMPM_STATUS_FAILED;
        }

        sts = Test_ReleaseResource(clientId, MMPM_RSC_ID_CORE_CLK);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            retSts = MMPM_STATUS_FAILED;
        }

        // Profile request duration with voltage level change
        ADSPPMTEST_LOG_PRINTF_0(
            ADSPPMTEST_LOG_LEVEL_RESULT,
            "[INFO] Profile Core Clock request duration with voltage level change");

        mmpmClkValType.clkFreqHz = UINT_MAX;

        sts = MMPM_Request_Ext(clientId, &mmpmRscExtParam);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            ADSPPMTEST_LOG_PRINTF_0(
                ADSPPMTEST_LOG_LEVEL_ERROR, "[ERROR] Request failed");
            retSts = MMPM_STATUS_FAILED;
        }

        sts = Test_ReleaseResource(clientId, MMPM_RSC_ID_CORE_CLK);
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
 * @ingroup Profile
 * @test profile_02
 * @brief Profile Core Clock request duration
 */
static MMPM_STATUS Test_Profile_02(uint32 clientId)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(PROFILE_02_NAME, PROFILE_02_DETAILS);
    ADSPPMTEST_LOG_PROFILESTART(PROFILE_02_NAME);

    sts = TestProfileCoreClockReq(clientId);

    ADSPPMTEST_LOG_PROFILEEND(PROFILE_02_NAME);
    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_Profile, PROFILE_02_NAME, sts);
    return sts;
}


#define PROFILE_03_NAME    "profile_03"
#define PROFILE_03_DETAILS "Profile Power request duration"

static MMPM_STATUS TestProfilePowerReq(uint32 clientId_Core, uint32 clientId_AdspHvx)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;

    MMPM_STATUS detailSts = MMPM_STATUS_FAILED;
    MmpmRscParamType mmpmRscParam;
    MmpmRscExtParamType mmpmRscExtParam;

    mmpmRscParam.rscId = MMPM_RSC_ID_POWER;
    mmpmRscExtParam.apiType = MMPM_API_TYPE_SYNC;
    mmpmRscExtParam.numOfReq = 1;
    mmpmRscExtParam.pReqArray = &mmpmRscParam;
    mmpmRscExtParam.pStsArray = &detailSts;

    // LPASS Core Power
    uint32 clientId = clientId_Core;
    if(clientId)
    {
        ADSPPMTEST_LOG_PRINTF_0(
            ADSPPMTEST_LOG_LEVEL_RESULT,
            "[INFO] Profile LPASS Core Power request duration");

        sts = MMPM_Request_Ext(clientId, &mmpmRscExtParam);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            ADSPPMTEST_LOG_PRINTF_0(
                ADSPPMTEST_LOG_LEVEL_ERROR, "[ERROR] Request failed");
            retSts = MMPM_STATUS_FAILED;
        }

        sts = Test_ReleaseResource(clientId, MMPM_RSC_ID_POWER);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            retSts = MMPM_STATUS_FAILED;
        }
    }
    else
    {
        retSts = MMPM_STATUS_FAILED;
    }

    // ADSP HVX Power
    clientId = clientId_AdspHvx;

    if(clientId)
    {
        ADSPPMTEST_LOG_PRINTF_0(
            ADSPPMTEST_LOG_LEVEL_RESULT,
            "[INFO] Profile ADSP HVX Power request duration");

        sts = MMPM_Request_Ext(clientId, &mmpmRscExtParam);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            ADSPPMTEST_LOG_PRINTF_0(
                ADSPPMTEST_LOG_LEVEL_ERROR, "[ERROR] Request failed");
            retSts = MMPM_STATUS_FAILED;
        }

        sts = Test_ReleaseResource(clientId, MMPM_RSC_ID_POWER);
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
 * @ingroup Profile
 * @test profile_03
 * @brief Profile Power request duration
 */
static MMPM_STATUS Test_Profile_03(uint32 clientId_Core, uint32 clientId_AdspHvx)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(PROFILE_03_NAME, PROFILE_03_DETAILS);
    ADSPPMTEST_LOG_PROFILESTART(PROFILE_03_NAME);

    sts = TestProfilePowerReq(clientId_Core, clientId_AdspHvx);

    ADSPPMTEST_LOG_PROFILEEND(PROFILE_03_NAME);
    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_Profile, PROFILE_03_NAME, sts);
    return sts;
}


#define PROFILE_04_NAME    "profile_04"
#define PROFILE_04_DETAILS "Profile Register Programming request duration"

static MMPM_STATUS TestProfileRegProgReq(uint32 clientId)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;

    MMPM_STATUS detailSts = MMPM_STATUS_FAILED;
    MmpmRscParamType mmpmRscParam;
    MmpmRscExtParamType mmpmRscExtParam;

    if(clientId)
    {
        mmpmRscParam.rscId = MMPM_RSC_ID_REG_PROG;
        mmpmRscParam.rscParam.regProgMatch = MMPM_REG_PROG_NORM;
        mmpmRscExtParam.apiType = MMPM_API_TYPE_SYNC;
        mmpmRscExtParam.numOfReq = 1;
        mmpmRscExtParam.pReqArray = &mmpmRscParam;
        mmpmRscExtParam.pStsArray = &detailSts;

        // Profile Register Programming request @ Normal speed
        ADSPPMTEST_LOG_PRINTF_0(
            ADSPPMTEST_LOG_LEVEL_RESULT,
            "[INFO] Profile Register Programming request @ Normal speed");

        mmpmRscParam.rscParam.regProgMatch = MMPM_REG_PROG_NORM;

        sts = MMPM_Request_Ext(clientId, &mmpmRscExtParam);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            ADSPPMTEST_LOG_PRINTF_0(
                ADSPPMTEST_LOG_LEVEL_ERROR, "[ERROR] Request failed");
            retSts = MMPM_STATUS_FAILED;
        }

        sts = Test_ReleaseResource(clientId, MMPM_RSC_ID_REG_PROG);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            retSts = MMPM_STATUS_FAILED;
        }

        // Profile Register Programming request @ Fast speed
        ADSPPMTEST_LOG_PRINTF_0(
            ADSPPMTEST_LOG_LEVEL_RESULT,
            "[INFO] Profile Register Programming request @ Fast speed");

        mmpmRscParam.rscParam.regProgMatch = MMPM_REG_PROG_FAST;

        sts = MMPM_Request_Ext(clientId, &mmpmRscExtParam);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            ADSPPMTEST_LOG_PRINTF_0(
                ADSPPMTEST_LOG_LEVEL_ERROR, "[ERROR] Request failed");
            retSts = MMPM_STATUS_FAILED;
        }

        sts = Test_ReleaseResource(clientId, MMPM_RSC_ID_REG_PROG);
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
 * @ingroup Profile
 * @test profile_04
 * @brief Profile Register Programming request duration
 */
static MMPM_STATUS Test_Profile_04(uint32 clientId)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(PROFILE_04_NAME, PROFILE_04_DETAILS);
    ADSPPMTEST_LOG_PROFILESTART(PROFILE_04_NAME);

    sts = TestProfileRegProgReq(clientId);

    ADSPPMTEST_LOG_PROFILEEND(PROFILE_04_NAME);
    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_Profile, PROFILE_04_NAME, sts);
    return sts;
}


#define PROFILE_05_NAME    "profile_05"
#define PROFILE_05_DETAILS "Profile BW request duration"

static MMPM_STATUS TestProfileBwReq(uint32 clientId)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    MMPM_STATUS detailSts = MMPM_STATUS_FAILED;
    MmpmRscParamType mmpmRscParam;
    MmpmRscExtParamType mmpmRscExtParam;
    MmpmGenBwValType bandWidthArray;
    MmpmGenBwReqType mmpmBwReqParam;

    if(clientId)
    {
        mmpmBwReqParam.numOfBw = 1;
        mmpmBwReqParam.pBandWidthArray = &bandWidthArray;
        mmpmRscParam.rscId = MMPM_RSC_ID_GENERIC_BW;
        mmpmRscParam.rscParam.pGenBwReq = &mmpmBwReqParam;
        mmpmRscExtParam.apiType = MMPM_API_TYPE_SYNC;
        mmpmRscExtParam.numOfReq = 1;
        mmpmRscExtParam.pReqArray = &mmpmRscParam;
        mmpmRscExtParam.pStsArray = &detailSts;

        // AHB transaction within LPASS core
        bandWidthArray.busRoute.masterPort = MMPM_BW_PORT_ID_DML_MASTER;
        bandWidthArray.busRoute.slavePort = MMPM_BW_PORT_ID_LPM_SLAVE;
        bandWidthArray.bwValue.busBwValue.usagePercentage = 100;
        bandWidthArray.bwValue.busBwValue.usageType = MMPM_BW_USAGE_LPASS_DMA;

        // Low BW request (no voltage level change)
        ADSPPMTEST_LOG_PRINTF_0(
            ADSPPMTEST_LOG_LEVEL_RESULT,
            "[INFO] AHB transaction within LPASS core: Low BW");

        bandWidthArray.bwValue.busBwValue.bwBytePerSec = 1024;

        sts = MMPM_Request_Ext(clientId, &mmpmRscExtParam);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            ADSPPMTEST_LOG_PRINTF_0(
                ADSPPMTEST_LOG_LEVEL_ERROR, "[ERROR] Request failed");
            retSts = MMPM_STATUS_FAILED;
        }

        sts = Test_ReleaseResource(clientId, MMPM_RSC_ID_GENERIC_BW);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            retSts = MMPM_STATUS_FAILED;
        }

        // High BW request (with voltage level change)
        ADSPPMTEST_LOG_PRINTF_0(
            ADSPPMTEST_LOG_LEVEL_RESULT,
            "[INFO] AHB transaction within LPASS core: High BW");

        bandWidthArray.bwValue.busBwValue.bwBytePerSec = ULLONG_MAX;

        sts = MMPM_Request_Ext(clientId, &mmpmRscExtParam);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            ADSPPMTEST_LOG_PRINTF_0(
                ADSPPMTEST_LOG_LEVEL_ERROR, "[ERROR] Request failed");
            retSts = MMPM_STATUS_FAILED;
        }

        sts = Test_ReleaseResource(clientId, MMPM_RSC_ID_GENERIC_BW);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            retSts = MMPM_STATUS_FAILED;
        }

        // ADSP transaction to DDR
        bandWidthArray.busRoute.masterPort = MMPM_BW_PORT_ID_ADSP_MASTER;
        bandWidthArray.busRoute.slavePort = MMPM_BW_PORT_ID_DDR_SLAVE;
        bandWidthArray.bwValue.busBwValue.usagePercentage = 100;
        bandWidthArray.bwValue.busBwValue.usageType = MMPM_BW_USAGE_LPASS_DSP;

        // Low BW request (no voltage level change)
        ADSPPMTEST_LOG_PRINTF_0(
            ADSPPMTEST_LOG_LEVEL_RESULT,
            "[INFO] ADSP transaction to DDR: Low BW");

        bandWidthArray.bwValue.busBwValue.bwBytePerSec = 1024;

        sts = MMPM_Request_Ext(clientId, &mmpmRscExtParam);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            ADSPPMTEST_LOG_PRINTF_0(
                ADSPPMTEST_LOG_LEVEL_ERROR, "[ERROR] Request failed");
            retSts = MMPM_STATUS_FAILED;
        }

        sts = Test_ReleaseResource(clientId, MMPM_RSC_ID_GENERIC_BW);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            retSts = MMPM_STATUS_FAILED;
        }

        // High BW request (with voltage level change)
        ADSPPMTEST_LOG_PRINTF_0(
            ADSPPMTEST_LOG_LEVEL_RESULT,
            "[INFO] ADSP transaction to DDR: High BW");

        bandWidthArray.bwValue.busBwValue.bwBytePerSec = ULLONG_MAX;

        sts = MMPM_Request_Ext(clientId, &mmpmRscExtParam);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            ADSPPMTEST_LOG_PRINTF_0(
                ADSPPMTEST_LOG_LEVEL_ERROR, "[ERROR] Request failed");
            retSts = MMPM_STATUS_FAILED;
        }

        sts = Test_ReleaseResource(clientId, MMPM_RSC_ID_GENERIC_BW);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            retSts = MMPM_STATUS_FAILED;
        }

        // LPASS core transaction to DDR
        bandWidthArray.busRoute.masterPort = MMPM_BW_PORT_ID_SLIMBUS_MASTER;
        bandWidthArray.busRoute.slavePort = MMPM_BW_PORT_ID_DDR_SLAVE;
        bandWidthArray.bwValue.busBwValue.usagePercentage = 100;
        bandWidthArray.bwValue.busBwValue.usageType = MMPM_BW_USAGE_LPASS_EXT_CPU;

        // Low BW request (no voltage level change)
        ADSPPMTEST_LOG_PRINTF_0(
            ADSPPMTEST_LOG_LEVEL_RESULT,
            "[INFO] LPASS core transaction to DDR: Low BW");

        bandWidthArray.bwValue.busBwValue.bwBytePerSec = 1024;

        sts = MMPM_Request_Ext(clientId, &mmpmRscExtParam);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            ADSPPMTEST_LOG_PRINTF_0(
                ADSPPMTEST_LOG_LEVEL_ERROR, "[ERROR] Request failed");
            retSts = MMPM_STATUS_FAILED;
        }

        sts = Test_ReleaseResource(clientId, MMPM_RSC_ID_GENERIC_BW);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            retSts = MMPM_STATUS_FAILED;
        }

        // High BW request (with voltage level change)
        ADSPPMTEST_LOG_PRINTF_0(
            ADSPPMTEST_LOG_LEVEL_RESULT,
            "[INFO] LPASS core transaction to DDR: High BW");

        bandWidthArray.bwValue.busBwValue.bwBytePerSec = ULLONG_MAX;

        sts = MMPM_Request_Ext(clientId, &mmpmRscExtParam);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            ADSPPMTEST_LOG_PRINTF_0(
                ADSPPMTEST_LOG_LEVEL_ERROR, "[ERROR] Request failed");
            retSts = MMPM_STATUS_FAILED;
        }

        sts = Test_ReleaseResource(clientId, MMPM_RSC_ID_GENERIC_BW);
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
 * @ingroup Profile
 * @test profile_05
 * @brief Profile BW request duration
 */
static MMPM_STATUS Test_Profile_05(uint32 clientId)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(PROFILE_05_NAME, PROFILE_05_DETAILS);
    ADSPPMTEST_LOG_PROFILESTART(PROFILE_05_NAME);

    sts = TestProfileBwReq(clientId);

    ADSPPMTEST_LOG_PROFILEEND(PROFILE_05_NAME);
    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_Profile, PROFILE_05_NAME, sts);
    return sts;
}


#define PROFILE_06_NAME    "profile_06"
#define PROFILE_06_DETAILS "Profile MIPS request duration"

static MMPM_STATUS TestProfileMipsReq(uint32 clientId)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    MMPM_STATUS detailSts = MMPM_STATUS_FAILED;
    MmpmRscParamType mmpmRscParam;
    MmpmRscExtParamType mmpmRscExtParam;
    MmpmMipsReqType mmpmMipsParam;


    if(clientId)
    {
        mmpmMipsParam.codeLocation = MMPM_BW_PORT_ID_DDR_SLAVE;
        mmpmMipsParam.reqOperation = MMPM_MIPS_REQUEST_CPU_CLOCK_ONLY;

        mmpmRscParam.rscId = MMPM_RSC_ID_MIPS_EXT;
        mmpmRscParam.rscParam.pMipsExt = &mmpmMipsParam;
        mmpmRscExtParam.apiType = MMPM_API_TYPE_SYNC;
        mmpmRscExtParam.numOfReq = 1;
        mmpmRscExtParam.pReqArray = &mmpmRscParam;
        mmpmRscExtParam.pStsArray = &detailSts;

        // Profile request duration with NO voltage level change
        ADSPPMTEST_LOG_PRINTF_0(
            ADSPPMTEST_LOG_LEVEL_RESULT,
            "[INFO] Profile MIPS request duration with NO voltage level change");

        mmpmMipsParam.mipsTotal = 1;
        mmpmMipsParam.mipsPerThread = 1/(AsicInfo_getMipsInfo()->numThreads);

        sts = MMPM_Request_Ext(clientId, &mmpmRscExtParam);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            ADSPPMTEST_LOG_PRINTF_0(
                ADSPPMTEST_LOG_LEVEL_ERROR, "[ERROR] Request failed");
            retSts = MMPM_STATUS_FAILED;
        }

        sts = Test_ReleaseResource(clientId, MMPM_RSC_ID_MIPS_EXT);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            retSts = MMPM_STATUS_FAILED;
        }

        // Profile request duration with voltage level change
        ADSPPMTEST_LOG_PRINTF_0(
            ADSPPMTEST_LOG_LEVEL_RESULT,
            "[INFO] Profile MIPS request duration with voltage level change");

        mmpmMipsParam.mipsTotal = UINT_MAX;
        mmpmMipsParam.mipsPerThread = UINT_MAX/(AsicInfo_getMipsInfo()->numThreads);

        sts = MMPM_Request_Ext(clientId, &mmpmRscExtParam);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            ADSPPMTEST_LOG_PRINTF_0(
                ADSPPMTEST_LOG_LEVEL_ERROR, "[ERROR] Request failed");
            retSts = MMPM_STATUS_FAILED;
        }

        sts = Test_ReleaseResource(clientId, MMPM_RSC_ID_MIPS_EXT);
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
 * @ingroup Profile
 * @test profile_06
 * @brief Profile MIPS request duration
 */
static MMPM_STATUS Test_Profile_06(uint32 clientId)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(PROFILE_06_NAME, PROFILE_06_DETAILS);
    ADSPPMTEST_LOG_PROFILESTART(PROFILE_06_NAME);

    sts = TestProfileMipsReq(clientId);

    ADSPPMTEST_LOG_PROFILEEND(PROFILE_06_NAME);
    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_Profile, PROFILE_06_NAME, sts);
    return sts;
}


#define PROFILE_07_NAME    "profile_07"
#define PROFILE_07_DETAILS "Profile Sleep Latency request duration"

static MMPM_STATUS TestProfileSleepReq(uint32 clientId)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;

    MMPM_STATUS detailSts = MMPM_STATUS_FAILED;
    MmpmRscParamType mmpmRscParam;
    MmpmRscExtParamType mmpmRscExtParam;

    if(clientId)
    {
        mmpmRscParam.rscId = MMPM_RSC_ID_SLEEP_LATENCY;
        mmpmRscParam.rscParam.sleepMicroSec = 10;
        mmpmRscExtParam.apiType = MMPM_API_TYPE_SYNC;
        mmpmRscExtParam.numOfReq = 1;
        mmpmRscExtParam.pReqArray = &mmpmRscParam;
        mmpmRscExtParam.pStsArray = &detailSts;

        // Profile request duration
        ADSPPMTEST_LOG_PRINTF_0(
            ADSPPMTEST_LOG_LEVEL_RESULT,
            "[INFO] Profile Sleep Latency request duration");

        sts = MMPM_Request_Ext(clientId, &mmpmRscExtParam);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            ADSPPMTEST_LOG_PRINTF_0(
                ADSPPMTEST_LOG_LEVEL_ERROR, "[ERROR] Request failed");
            retSts = MMPM_STATUS_FAILED;
        }

        sts = Test_ReleaseResource(clientId, MMPM_RSC_ID_SLEEP_LATENCY);
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
 * @ingroup Profile
 * @test profile_07
 * @brief Profile Sleep Latency request duration
 */
static MMPM_STATUS Test_Profile_07(uint32 clientId)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(PROFILE_07_NAME, PROFILE_07_DETAILS);
    ADSPPMTEST_LOG_PROFILESTART(PROFILE_07_NAME);

    sts = TestProfileSleepReq(clientId);

    ADSPPMTEST_LOG_PROFILEEND(PROFILE_07_NAME);
    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_Profile, PROFILE_07_NAME, sts);
    return sts;
}

static MMPM_STATUS Test_Profile_Request_High_Mips(uint32 clientId)
{
    // Request high MIPS to force the DSP clock to MAX
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    int mips = 1000;

    if (clientId)
    {
        ADSPPMTEST_LOG_PRINTF_1(ADSPPMTEST_LOG_LEVEL_RESULT, "Test_Profile Using client Id: %d", clientId);
        sts = Test_RequestMips(clientId, mips);
        if (MMPM_STATUS_SUCCESS == sts)
        {
            ADSPPMTEST_LOG_PRINTF_1(ADSPPMTEST_LOG_LEVEL_RESULT, "Using MIPS: %u", mips);
        }
        else
        {
            ADSPPMTEST_LOG_PRINTF_0(ADSPPMTEST_LOG_LEVEL_ERROR, "Using MIPS: default");
        }
    }
    else
    {
        ADSPPMTEST_LOG_PRINTF_0(ADSPPMTEST_LOG_LEVEL_ERROR, "[ERROR] Received clientId 0.");
    }

    return sts;
}

void Test_Profile(void)
{
    // Force the frequency to MAX for all shared clients
    MMPM_STATUS sts_Core = MMPM_STATUS_FAILED;
    MMPM_STATUS sts_Adsp = MMPM_STATUS_FAILED;
    MMPM_STATUS sts_AdspHvx = MMPM_STATUS_FAILED;
    MMPM_STATUS sts_Slimbus = MMPM_STATUS_FAILED;

    uint32 clientId_Core;      // Shared client for MMPM_CORE_ID_LPASS_CORE
    uint32 clientId_Adsp;      // Shared client for MMPM_CORE_ID_LPASS_ADSP
    uint32 clientId_AdspHvx;   // Shared client for MMPM_CORE_ID_LPASS_ADSP_HVX
    uint32 clientId_Slimbus;   // Shared client for MMPM_CORE_ID_LPASS_SLIMBUS

    clientId_Core = Test_Register(MMPM_CORE_ID_LPASS_CORE, MMPM_CORE_INSTANCE_0, "TestProfile_LPASS_CORE");
    if (clientId_Core)
    {
        ADSPPMTEST_LOG_PRINTF_1(ADSPPMTEST_LOG_LEVEL_ERROR, "CORE clientId = %d", clientId_Core);
        sts_Core = Test_Profile_Request_High_Mips(clientId_Core);
    }
    
    clientId_Adsp = Test_Register(MMPM_CORE_ID_LPASS_ADSP, MMPM_CORE_INSTANCE_0, "TestProfile_LPASS_ADSP");
    if (clientId_Adsp)
    {
        ADSPPMTEST_LOG_PRINTF_1(ADSPPMTEST_LOG_LEVEL_ERROR, "ADSP clientId = %d", clientId_Adsp);
    }

    clientId_AdspHvx = Test_Register(MMPM_CORE_ID_LPASS_ADSP_HVX, MMPM_CORE_INSTANCE_0, "TestProfile_LPASS_ADSP_HVX");
    if (clientId_AdspHvx)
    {
        ADSPPMTEST_LOG_PRINTF_1(ADSPPMTEST_LOG_LEVEL_ERROR, "ADSP_HVX clientId = %d", clientId_AdspHvx);
    }

    clientId_Slimbus = Test_Register(MMPM_CORE_ID_LPASS_SLIMBUS, MMPM_CORE_INSTANCE_0, "TestProfile_LPASS_SLIMBUS");
    if (clientId_Slimbus)
    {
        ADSPPMTEST_LOG_PRINTF_1(ADSPPMTEST_LOG_LEVEL_ERROR, "SLIMBUS clientId = %d", clientId_Slimbus);
    }

    // Profile register/deregister duration
    Test_Profile_01();

    // Profile Core Clock request duration
    Test_Profile_02(clientId_Adsp);

    // Profile Power request duration
    Test_Profile_03(clientId_Core, clientId_AdspHvx);

    // Profile Register Programming request duration
    Test_Profile_04(clientId_Slimbus);

    // Profile BW request duration
    Test_Profile_05(clientId_Adsp);

    // Profile MIPS request duration
    Test_Profile_06(clientId_Adsp);

    // Profile Sleep Latency request duration
    Test_Profile_07(clientId_Adsp);

    // Release the MAX MIPS requests and deregister shared clients
    if (clientId_Core && (MMPM_STATUS_SUCCESS == sts_Core))
    {
        sts_Core = Test_ReleaseResource(clientId_Core, MMPM_RSC_ID_MIPS_EXT);
        sts_Core = Test_Deregister(clientId_Core);
    }
    if (clientId_Adsp && (MMPM_STATUS_SUCCESS == sts_Adsp))
    {
        sts_Adsp = Test_Deregister(clientId_Adsp);
    }
    if (clientId_AdspHvx && (MMPM_STATUS_SUCCESS == sts_AdspHvx))
    {
        sts_AdspHvx = Test_Deregister(clientId_AdspHvx);
    }
    if (clientId_Slimbus && (MMPM_STATUS_SUCCESS == sts_Slimbus))
    {
        sts_Slimbus = Test_Deregister(clientId_Slimbus);
    }
}

