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


static const BwReqTestType testBwArray[] =
{
    {MMPM_BW_PORT_ID_ADSP_MASTER,    MMPM_BW_PORT_ID_DML_SLAVE,     200000000ull, 100, MMPM_BW_USAGE_LPASS_DSP},
    {MMPM_BW_PORT_ID_DML_MASTER,     MMPM_BW_PORT_ID_AIF_SLAVE,     200000000ull, 100, MMPM_BW_USAGE_LPASS_DMA},
    {MMPM_BW_PORT_ID_AIF_MASTER,     MMPM_BW_PORT_ID_SLIMBUS_SLAVE, 200000000ull, 100, MMPM_BW_USAGE_LPASS_DSP},
    {MMPM_BW_PORT_ID_SLIMBUS_MASTER, MMPM_BW_PORT_ID_MIDI_SLAVE,    200000000ull, 100, MMPM_BW_USAGE_LPASS_DMA},
    {MMPM_BW_PORT_ID_HWRSMP_MASTER,  MMPM_BW_PORT_ID_AVSYNC_SLAVE,  200000000ull, 100, MMPM_BW_USAGE_LPASS_DMA},
    {MMPM_BW_PORT_ID_SPDIF_MASTER,   MMPM_BW_PORT_ID_LPM_SLAVE,     200000000ull, 100, MMPM_BW_USAGE_LPASS_DSP},
    {MMPM_BW_PORT_ID_HDMIRX_MASTER,  MMPM_BW_PORT_ID_SRAM_SLAVE,    200000000ull, 100, MMPM_BW_USAGE_LPASS_DMA},
    {MMPM_BW_PORT_ID_SIF_MASTER,     MMPM_BW_PORT_ID_SPDIF_SLAVE,   200000000ull, 100, MMPM_BW_USAGE_LPASS_DSP},
};


#define BUNDLE_01_NAME    "bundle_01"
#define BUNDLE_01_DETAILS "Req-Rel"

static MMPM_STATUS TestBundleReqRel(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    uint32 clientId, numOfReqs, i, coreClkId;
    const AsicInfo_ClockFreqVoltPlanType *pFreqPlan = NULL;

    clientId = Test_Register(MMPM_CORE_ID_LPASS_DML, MMPM_CORE_INSTANCE_0, BUNDLE_01_NAME);

    if(clientId)
    {
        MmpmRscExtParamType mmpmRscExtParam;
        MmpmRscParamType mmpmRscParam[8];
        MMPM_STATUS stsArray[8];
        MmpmClkReqType mmpmClkReq;
        MmpmClkValType mmpmClkValArray[8];
        MmpmGenBwReqType mmpmBwReq;
        MmpmGenBwValType mmpmBwArray[8];

        // rscId = MMPM_RSC_ID_POWER
        {
            mmpmRscParam[0].rscId = MMPM_RSC_ID_POWER;
        }

        // rscId = MMPM_RSC_ID_REG_PROG
        {
            mmpmRscParam[1].rscId = MMPM_RSC_ID_REG_PROG;
            mmpmRscParam[1].rscParam.regProgMatch = MMPM_REG_PROG_NORM;
        }

        // rscId = MMPM_RSC_ID_GENERIC_BW
        {
            numOfReqs = 0;

            for(i = 0; i < (sizeof(testBwArray)/sizeof(testBwArray[0])); i++)
            {
                if((AsicInfo_isBusPortAvailable(testBwArray[i].masterPort)) &&
                    (AsicInfo_isBusPortAvailable(testBwArray[i].slavePort)))
                {
                    mmpmBwArray[numOfReqs].busRoute.masterPort = testBwArray[i].masterPort;
                    mmpmBwArray[numOfReqs].busRoute.slavePort = testBwArray[i].slavePort;
                    mmpmBwArray[numOfReqs].bwValue.busBwValue.bwBytePerSec = testBwArray[i].bwVal;
                    mmpmBwArray[numOfReqs].bwValue.busBwValue.usagePercentage = testBwArray[i].usagePercent;
                    mmpmBwArray[numOfReqs].bwValue.busBwValue.usageType = testBwArray[i].usageType;
                    numOfReqs++;
                }
            }

            mmpmBwReq.numOfBw = numOfReqs;
            mmpmBwReq.pBandWidthArray = mmpmBwArray;

            mmpmRscParam[2].rscId = MMPM_RSC_ID_GENERIC_BW;
            mmpmRscParam[2].rscParam.pGenBwReq = &mmpmBwReq;
        }

        // rscId = MMPM_RSC_ID_CORE_CLK
        {
            numOfReqs = 0;

            for(coreClkId = MMPM_CLK_ID_LPASS_HWRSP_CORE; coreClkId < MMPM_CLK_ID_LPASS_ADSP_CORE; coreClkId++)
            {
                if(AsicInfo_isCoreClockAvailable(coreClkId))
                {
                    pFreqPlan = AsicInfo_getClockFreqVoltPlan(coreClkId);
                    mmpmClkValArray[numOfReqs].clkId.clkIdLpass = coreClkId;
                    mmpmClkValArray[numOfReqs].clkFreqHz =
                        pFreqPlan->pFreqVoltPlan[pFreqPlan->numFreq -1].frequency;
                    mmpmClkValArray[numOfReqs].freqMatch = MMPM_FREQ_AT_LEAST;
                    numOfReqs++;
                }
            }

            mmpmClkReq.numOfClk = numOfReqs;
            mmpmClkReq.pClkArray = mmpmClkValArray;

            mmpmRscParam[3].rscId = MMPM_RSC_ID_CORE_CLK;
            mmpmRscParam[3].rscParam.pCoreClk = &mmpmClkReq;
        }

        mmpmRscExtParam.apiType = MMPM_API_TYPE_SYNC;
        mmpmRscExtParam.numOfReq = 4;
        mmpmRscExtParam.pReqArray = mmpmRscParam;
        mmpmRscExtParam.pStsArray = stsArray;

        sts = MMPM_Request_Ext(clientId, &mmpmRscExtParam);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            ADSPPMTEST_LOG_PRINTF_0(
                ADSPPMTEST_LOG_LEVEL_ERROR,
                "[ERROR] Error in MMPM_Request_Ext");
            retSts = MMPM_STATUS_FAILED;
        }
        else
        {
            sts = MMPM_Release_Ext(clientId, &mmpmRscExtParam);
            if(MMPM_STATUS_SUCCESS != sts)
            {
                ADSPPMTEST_LOG_PRINTF_0(
                    ADSPPMTEST_LOG_LEVEL_ERROR,
                    "[ERROR] Error in MMPM_Release_Ext");
                retSts = MMPM_STATUS_FAILED;
            }
        }

        sts = Test_Deregister(clientId);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            retSts = MMPM_STATUS_FAILED;
        }
    }

    return retSts;
}

/**
 * @ingroup Bundle
 * @test bundle_01
 * @brief Req-Rel
 */
static MMPM_STATUS Test_Bundle_01(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(BUNDLE_01_NAME, BUNDLE_01_DETAILS);

    sts = TestBundleReqRel();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_Bundle, BUNDLE_01_NAME, sts);
    return sts;
}


#define BUNDLE_02_NAME    "bundle_02"
#define BUNDLE_02_DETAILS "Test for QDI server alignment issues"

static MMPM_STATUS TestBundleReqRelMisalign(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    uint32 clientId, numOfReqs, i, coreClkId;
    const AsicInfo_ClockFreqVoltPlanType *pFreqPlan = NULL;

    clientId = Test_Register(MMPM_CORE_ID_LPASS_DML, MMPM_CORE_INSTANCE_0, BUNDLE_02_NAME);

    if(clientId)
    {
        MmpmRscExtParamType mmpmRscExtParam;
        MmpmRscParamType mmpmRscParam[8];
        MMPM_STATUS stsArray[8];
        MmpmClkReqType mmpmClkReq;
        MmpmClkValType mmpmClkValArray[8];
        MmpmGenBwReqType mmpmBwReq;
        MmpmGenBwValType mmpmBwArray[8];

        // rscId = MMPM_RSC_ID_POWER
        {
            mmpmRscParam[0].rscId = MMPM_RSC_ID_POWER;
        }

        // rscId = MMPM_RSC_ID_REG_PROG
        {
            mmpmRscParam[1].rscId = MMPM_RSC_ID_REG_PROG;
            mmpmRscParam[1].rscParam.regProgMatch = MMPM_REG_PROG_NORM;
        }

        // rscId = MMPM_RSC_ID_CORE_CLK
        {
            numOfReqs = 0;

            for(coreClkId = MMPM_CLK_ID_LPASS_HWRSP_CORE; coreClkId < MMPM_CLK_ID_LPASS_ADSP_CORE; coreClkId++)
            {
                if(AsicInfo_isCoreClockAvailable(coreClkId))
                {
                    pFreqPlan = AsicInfo_getClockFreqVoltPlan(coreClkId);
                    mmpmClkValArray[numOfReqs].clkId.clkIdLpass = coreClkId;
                    mmpmClkValArray[numOfReqs].clkFreqHz =
                        pFreqPlan->pFreqVoltPlan[pFreqPlan->numFreq -1].frequency;
                    mmpmClkValArray[numOfReqs].freqMatch = MMPM_FREQ_AT_LEAST;
                    numOfReqs++;
                }
            }

            mmpmClkReq.numOfClk = numOfReqs;
            mmpmClkReq.pClkArray = mmpmClkValArray;

            mmpmRscParam[2].rscId = MMPM_RSC_ID_CORE_CLK;
            mmpmRscParam[2].rscParam.pCoreClk = &mmpmClkReq;
        }

        // rscId = MMPM_RSC_ID_GENERIC_BW
        {
            numOfReqs = 0;

            for(i = 0; i < (sizeof(testBwArray)/sizeof(testBwArray[0])); i++)
            {
                if((AsicInfo_isBusPortAvailable(testBwArray[i].masterPort)) &&
                    (AsicInfo_isBusPortAvailable(testBwArray[i].slavePort)))
                {
                    mmpmBwArray[numOfReqs].busRoute.masterPort = testBwArray[i].masterPort;
                    mmpmBwArray[numOfReqs].busRoute.slavePort = testBwArray[i].slavePort;
                    mmpmBwArray[numOfReqs].bwValue.busBwValue.bwBytePerSec = testBwArray[i].bwVal;
                    mmpmBwArray[numOfReqs].bwValue.busBwValue.usagePercentage = testBwArray[i].usagePercent;
                    mmpmBwArray[numOfReqs].bwValue.busBwValue.usageType = testBwArray[i].usageType;
                    numOfReqs++;
                }
            }

            mmpmBwReq.numOfBw = numOfReqs;
            mmpmBwReq.pBandWidthArray = mmpmBwArray;

            mmpmRscParam[3].rscId = MMPM_RSC_ID_GENERIC_BW;
            mmpmRscParam[3].rscParam.pGenBwReq = &mmpmBwReq;
        }

        mmpmRscExtParam.apiType = MMPM_API_TYPE_SYNC;
        mmpmRscExtParam.numOfReq = 4;
        mmpmRscExtParam.pReqArray = mmpmRscParam;
        mmpmRscExtParam.pStsArray = stsArray;

        sts = MMPM_Request_Ext(clientId, &mmpmRscExtParam);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            ADSPPMTEST_LOG_PRINTF_0(
                ADSPPMTEST_LOG_LEVEL_ERROR,
                "[ERROR] Error in MMPM_Request_Ext");
            retSts = MMPM_STATUS_FAILED;
        }
        else
        {
            sts = MMPM_Release_Ext(clientId, &mmpmRscExtParam);
            if(MMPM_STATUS_SUCCESS != sts)
            {
                ADSPPMTEST_LOG_PRINTF_0(
                    ADSPPMTEST_LOG_LEVEL_ERROR,
                    "[ERROR] Error in MMPM_Release_Ext");
                retSts = MMPM_STATUS_FAILED;
            }
        }

        sts = Test_Deregister(clientId);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            retSts = MMPM_STATUS_FAILED;
        }
    }

    return retSts;
}

/**
 * @ingroup Bundle
 * @test bundle_02
 * @brief Test for QDI server alignment issues
 */
static MMPM_STATUS Test_Bundle_02(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(BUNDLE_02_NAME, BUNDLE_02_DETAILS);

    sts = TestBundleReqRelMisalign();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_Bundle, BUNDLE_02_NAME, sts);
    return sts;
}


void Test_Bundle(void)
{
    // Req-Rel
    Test_Bundle_01();

    // Test for QDI server alignment issues
    Test_Bundle_02();
}

