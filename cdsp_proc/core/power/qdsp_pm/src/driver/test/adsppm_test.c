/*
* Copyright (c) 2015 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
@file: adsppm_test.c
@brief: Implementation of ADSPPM wrapper for calls to MMPM.

$Header: //components/rel/core.qdsp6/2.1.c4/power/qdsp_pm/src/driver/test/adsppm_test.c#1 $
*/

#include "mmpm.h"
#include "adsppm_test.h"
#include "adsppm_test_param.h"
#include "adsppm_test_utils.h"
#include "adsppm_test_asic_info.h"
#include "adsppm.h"
#include "adsppm_test_verify.h"
#include "asic.h"

//#include "sysmon_dcvs_int.h"

static int callback_count = 0;

int adsppmTest_Callback_Count(void)
{
    return callback_count;
}

static void adsppmTest_DeregisterAllExistingClients(void)
{

    for(uint32 clientId = 1; clientId < MAX_NUM_OF_ADSPPM_CLIENTS; clientId++)
        MMPM_Deregister_Ext(clientId);

}

void RunAllTests(void)
{

    // Disable SysMon DCVS
    /* TODO: Till SysMon DCVS gets here
    sysmon_dcvs_disable_ext();
    */

    // Deregister all existing ADSPPM clients
    ////// CATL - Remove for testing
    adsppmTest_DeregisterAllExistingClients();

    // Turn ON LPASS CORE
    // For most clocks to be ON, LPASS CORE needs to be ON
    uint32 clientId = 0;

    if(ACM_GetFeatureStatus(AsicFeatureId_ComputeDSP) == AsicFeatureState_Enabled)
    {
        Test_CDSP_System();
        Test_CDSP_Power();
        Test_CDSP_CoreClock();
        Test_CDSP_Mips();
        Test_CDSP_Bandwidth();
        Test_CDSP_DcvsAggregation();
        Test_CDSP_SystemCache();
    }
    else if( ACM_GetFeatureStatus(AsicFeatureId_SlpiDSP) == AsicFeatureState_Enabled )
	/* (ACM_GetFeatureStatus(AsicFeatureId_LpassSlpiDSP) == AsicFeatureState_Enabled) */
    {
        Test_Slpi_System();
        Test_Slpi_CoreClock();
        Test_Slpi_Mips();
        Test_Slpi_Bandwidth();
    }
    else
    {
        clientId = Test_Register(MMPM_CORE_ID_LPASS_CORE, MMPM_CORE_INSTANCE_0,
            "RunAllTests");

        if(clientId)
        {
            Test_RequestPower(clientId);
            Test_System();
            Test_CoreClock();
            Test_DomainClock();
            Test_Mips();
            Test_SleepLatency();
            Test_Bandwidth();
            Test_RegisterProgramming();
            Test_Power();
            Test_MemoryPower();
            Test_Bundle();
            Test_Stress();
            Test_Profile();
            Test_CachePartitioning();
            Test_DcvsAggregation();
            Test_SystemCache();
        }
    }
}


// Get test parameter and issue the call
MMPM_STATUS Test_InvokeTest(
    AdsppmTestType test[],
    uint32 numTest,
    MMPM_STATUS testSts[][MAX_TEST_SEQUENCE])
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    MMPM_STATUS retSts = MMPM_STATUS_SUCCESS;
    uint32 clientId[MAX_TEST_CLIENT] = {0};

    ClkTestType clk[MAX_CLK_REQUEST];
    ClkDomainTestType clkDomain[MAX_CLK_REQUEST];
    BwReqTestType bw[MAX_BW_REQUEST];
    uint32 testIndex, testSeqIndex;
    uint32 numClks, mips, numBw, microSec, clkFreq;
    MmpmRegProgMatchType match;
    MmpmClkIdLpassType clkId;
    MmpmMemIdType memType;
    MmpmMemPowerStateType powerState;
    MmpmCoreIdType coreId;
    MmpmCoreInstanceIdType instanceId;
    uint32 *pClientId = NULL;

    for(testIndex = 0; testIndex < numTest; testIndex++)
    {
        coreId = test[testIndex].coreId;
        instanceId = test[testIndex].instanceId;
        pClientId = &(clientId[test[testIndex].clientNum]);

        if(AsicInfo_isCoreInstanceAvailable(coreId, instanceId))
        {
            for(testSeqIndex = 0;
                testSeqIndex < test[testIndex].numTestSeq;
                testSeqIndex++)
            {
                testSts[testIndex][testSeqIndex] = MMPM_STATUS_SUCCESS;

                // Reset to failed by default after each test
                sts = MMPM_STATUS_FAILED;

                PRINT_INFOEXT_4(
                    "TestAPI %u, Test#%u TestSeq#%u coreId#%u",
                    test[testIndex].testSeq[testSeqIndex],
                    testIndex,
                    testSeqIndex,
                    test[testIndex].coreId);

                switch(test[testIndex].testSeq[testSeqIndex])
                {
                case REG:
                    *pClientId = Test_Register(coreId, instanceId, "InvokeTest");
                    sts = (*pClientId)?MMPM_STATUS_SUCCESS:MMPM_STATUS_FAILED;
                    break;

                case REQCLK:
                    GetReqClockParam(coreId, clk, &numClks);
                    sts = Test_RequestCoreClock(*pClientId, clk, numClks);
                    break;

                case REQCLKDOMAIN:
                    GetReqClockDomainParam(coreId, clkDomain, &numClks);
                    sts = Test_RequestCoreClockDomain(*pClientId, clkDomain, numClks);
                    break;

                case REQMIPS:
                    GetReqMipsParam(coreId, &mips);
                    sts = Test_RequestMips(*pClientId, mips);
                    break;

                case REQSLATENCY:
                    GetReqSleepLatencyParam(coreId, &microSec);
                    sts = Test_RequestSleepLatency(*pClientId, microSec);
                    break;

                case REQBW:
                    GetReqBandwidthParam(coreId, bw, &numBw);
                    sts = Test_RequestBandwidth(*pClientId, &bw[0], numBw, NULL);
                    break;

                case REQREGP:
                    GetReqRegProgParam(coreId, &match);
                    sts = Test_RequestRegProg(*pClientId, match);
                    break;

                case REQPWR:
                    sts = Test_RequestPower(*pClientId);
                    break;

                case REQMEMPWR:
                    GetReqMemPowerParam(coreId, &memType, &powerState);
                    sts = Test_RequestMemoryPower(*pClientId, memType, powerState);
                    break;

                case RELCLK:
                    sts = Test_ReleaseResource(*pClientId, MMPM_RSC_ID_CORE_CLK);
                    break;

                case RELCLKDOMAIN:
                    sts = Test_ReleaseResource(*pClientId, MMPM_RSC_ID_CORE_CLK_DOMAIN);
                    break;

                case RELMIPS:
                    sts = Test_ReleaseResource(*pClientId, MMPM_RSC_ID_MIPS_EXT);
                    break;

                case RELSPATENCY:
                    sts = Test_ReleaseResource(*pClientId, MMPM_RSC_ID_SLEEP_LATENCY);
                    break;

                case RELBW:
                    sts = Test_ReleaseResource(*pClientId, MMPM_RSC_ID_GENERIC_BW);
                    break;

                case RELREGP:
                    sts = Test_ReleaseResource(*pClientId, MMPM_RSC_ID_REG_PROG);
                    break;

                case RELPWR:
                    sts = Test_ReleaseResource(*pClientId, MMPM_RSC_ID_POWER);
                    break;

                case RELMEMPWR:
                    sts = Test_ReleaseResource(*pClientId, MMPM_RSC_ID_MEM_POWER);
                    break;

                case DREG:
                    sts = Test_Deregister(*pClientId);
                    *pClientId = 0;
                    break;

                case INFOCLK:
                    GetInfoClkParam(coreId, &clkId);
                    sts = MMPM_STATUS_SUCCESS;
                    break;

                case INFOPWR:
                    sts = MMPM_STATUS_SUCCESS;
                    break;

                case INFOAHBCLK:
                    sts = Test_InfoClk(coreId, instanceId, MMPM_CLK_ID_LPASS_AHB_ROOT, &clkFreq);
                    break;

                case INFOQDSPCLK:
                    sts = Test_InfoClk(coreId, instanceId, MMPM_CLK_ID_LPASS_ADSP_CORE, &clkFreq);
                    break;

                case THERML:
                    sts = MMPM_STATUS_SUCCESS;
                    break;

                case SETPARAM:
                    break;

                default:
                    break;
                }

                if(MMPM_STATUS_SUCCESS != sts)
                {
                    if(MMPM_STATUS_UNSUPPORTED == sts)
                    {
                        PRINT_NA_4(
                            "TestAPI %u, Test#%u TestSeq#%u coreId#%u",
                            test[testIndex].testSeq[testSeqIndex],
                            testIndex,
                            testSeqIndex,
                            test[testIndex].coreId);
                    }
                    else
                    {
                        PRINT_ERROR_4(
                            "TestAPI %u, Test#%u TestSeq#%u coreId#%u",
                            test[testIndex].testSeq[testSeqIndex],
                            testIndex,
                            testSeqIndex,
                            test[testIndex].coreId);
                        retSts = MMPM_STATUS_FAILED;
                    }
                    testSts[testIndex][testSeqIndex] = sts;
                }
            }
        }
    }

    return retSts;
}


uint32 testCallbackFunc(MmpmCallbackParamType *pCbParam)
{
    callback_count++;

    ADSPPMTEST_LOG_PRINTF_2(ADSPPMTEST_LOG_LEVEL_RESULT,
        "Async callback func: tag=%u, result=%u",
        ((MmpmCompletionCallbackDataType *) (pCbParam->callbackData))->reqTag,
        ((MmpmCompletionCallbackDataType *) (pCbParam->callbackData))->result);

    return 0;
}


uint32 Test_Register(
    MmpmCoreIdType coreId,
    MmpmCoreInstanceIdType instanceId,
    char *clientName)
{
    MmpmRegParamType mmpmRegParam;
    uint32 clientId = 0;

    if(MAX_LEN_CLIENT_NAME <= (sizeof(clientName)/sizeof(clientName[0])))
    {
        PRINT_ERROR_1("Client name is too long, max length %u", MAX_LEN_CLIENT_NAME);
    }
    else
    {
        mmpmRegParam.rev = MMPM_REVISION;
        mmpmRegParam.coreId = coreId;
        mmpmRegParam.instanceId = instanceId;
        mmpmRegParam.pClientName = (char *) clientName;
        mmpmRegParam.pwrCtrlFlag = PWR_CTRL_NONE;
        mmpmRegParam.callBackFlag = CALLBACK_REQUEST_COMPLETE;
        mmpmRegParam.MMPM_Callback = testCallbackFunc;
        PRINT_INFOEXT_2("coreID %u instanceID %u", coreId, instanceId);

        clientId = MMPM_Register_Ext(&mmpmRegParam);
        if(clientId)
        {
            PRINT_PASS_3(
                "clientID %u for coreID %u instanceID %u",
                clientId, mmpmRegParam.coreId, mmpmRegParam.instanceId);
        }
        else
        {
            PRINT_ERROR_2(
                "coreID %u instanceID %u",
                mmpmRegParam.coreId, mmpmRegParam.instanceId);
        }
    }

    return clientId;
}


MMPM_STATUS Test_RequestCoreClock(
    uint32 clientId,
    ClkTestType clk[],
    uint32 numClk)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    MMPM_STATUS detailSts = MMPM_STATUS_FAILED;
    MmpmRscParamType mmpmRscParam;
    MmpmRscExtParamType mmpmRscExtParam;
    MmpmClkValType mmpmClkValType[MAX_CLK_REQUEST];
    MmpmClkReqType mmpmClkReqType;
    uint32 i;

    if(clientId)
    {
        for(i = 0; i < numClk; i++)
        {
            mmpmClkValType[i].clkId.clkIdLpass = (MmpmClkIdLpassType)clk[i].clkId;
            mmpmClkValType[i].clkFreqHz = clk[i].freq;
            mmpmClkValType[i].freqMatch = clk[i].freqMatch;
            PRINT_INFOEXT_4(
                "clkNum=%u, clockID=%u, Freq=%u Hz, Match=%u",
                i, clk[i].clkId, clk[i].freq, clk[i].freqMatch);
        }
        mmpmClkReqType.numOfClk = numClk;
        mmpmClkReqType.pClkArray = mmpmClkValType;
        mmpmRscParam.rscId = MMPM_RSC_ID_CORE_CLK;
        mmpmRscParam.rscParam.pCoreClk = &mmpmClkReqType;
        mmpmRscExtParam.apiType = MMPM_API_TYPE_SYNC;
        mmpmRscExtParam.numOfReq = 1;
        mmpmRscExtParam.pReqArray = &mmpmRscParam;
        mmpmRscExtParam.pStsArray = &detailSts;

        sts = MMPM_Request_Ext(clientId, &mmpmRscExtParam);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            PRINT_ERROR_2("clientID %u, returned %u", clientId, detailSts);
        }
        else
        {
            if (Adsppm_Verify(clientId,&mmpmRscExtParam,NULL) != TRUE)
            {
                sts = MMPM_STATUS_FAILED;
            }
            else
            {
                PRINT_PASS_1("clientID %u", clientId);
            }
        }
    }
    else
    {
        // This is an error in the test code
        PRINT_ERROR_0("Invalid ADSPPM client ID");
    }

    return sts;
}


MMPM_STATUS Test_RequestCoreClockDomain(
    uint32 clientId,
    ClkDomainTestType clk[],
    uint32 numClk)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    MMPM_STATUS detailSts = MMPM_STATUS_FAILED;
    MmpmRscParamType mmpmRscParam;
    MmpmRscExtParamType mmpmRscExtParam;
    MmpmClkDomainType mmpmClkDomainValType[MAX_CLK_REQUEST];
    MmpmClkDomainReqType mmpmClkDomainReqType;
    uint32 i;

    if(clientId)
    {
        for(i = 0; i < numClk; i++)
        {
            mmpmClkDomainValType[i].clkId.clkIdLpass = (MmpmClkIdLpassType) clk[i].clkId;
            mmpmClkDomainValType[i].clkFreqHz = clk[i].clkFreqHz;
            mmpmClkDomainValType[i].clkDomainSrc.clkDomainSrcIdLpass = clk[i].clkDomainSrc;
            PRINT_INFOEXT_4(
                "clkNum=%u, clockID=%u, clkFreqHz=%u Hz, clkDomainSrc=%u",
                i, clk[i].clkId, clk[i].clkFreqHz, clk[i].clkDomainSrc);
        }
        mmpmClkDomainReqType.numOfClk = numClk;
        mmpmClkDomainReqType.pClkDomainArray = mmpmClkDomainValType;
        mmpmRscParam.rscId = MMPM_RSC_ID_CORE_CLK_DOMAIN;
        mmpmRscParam.rscParam.pCoreClkDomain = &mmpmClkDomainReqType;
        mmpmRscExtParam.apiType = MMPM_API_TYPE_SYNC;
        mmpmRscExtParam.numOfReq = 1;
        mmpmRscExtParam.pReqArray = &mmpmRscParam;
        mmpmRscExtParam.pStsArray = &detailSts;

        sts = MMPM_Request_Ext(clientId, &mmpmRscExtParam);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            if(MMPM_STATUS_UNSUPPORTED == detailSts)
            {
                sts = MMPM_STATUS_SUCCESS;
                PRINT_NA_0(
                    "Clock Domain Requests are not supported in this chipset");
            }
            else
            {
                PRINT_ERROR_2("clientID %u, returned %u", clientId, detailSts);
            }
        }
        else
        {
            PRINT_PASS_1("clientID %u", clientId);
        }
    }
    else
    {
        // This is an error in the test code
        PRINT_ERROR_0("Invalid ADSPPM client ID");
    }

    return sts;
}


MMPM_STATUS Test_RequestMips(
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
        mmpmRscExtParam.apiType = MMPM_API_TYPE_SYNC;
        mmpmRscExtParam.numOfReq = 1;
        mmpmRscExtParam.pReqArray = &mmpmRscParam;
        mmpmRscExtParam.pStsArray = &detailSts;
        PRINT_INFOEXT_2("clientID %u, mips %u", clientId, mips);

        sts = MMPM_Request_Ext(clientId, &mmpmRscExtParam);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            PRINT_ERROR_2("clientID %u, returned %u", clientId, detailSts);
        }
        /* If request is honored, verify the result */
        else
        {
            Adsppm_Verify(clientId, &mmpmRscExtParam, NULL);
        }

    }
    else
    {
        // This is an error in the test code
        PRINT_ERROR_0("Invalid ADSPPM client ID");
    }

    return sts;
}

MMPM_STATUS Test_RequestMpps(
    uint32 clientId,
    MmpmMppsReqType *pMppsReq)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    MMPM_STATUS detailSts = MMPM_STATUS_FAILED;
    MmpmRscParamType mmpmRscParam;
    MmpmRscExtParamType mmpmRscExtParam;

    if(clientId)
    {
        mmpmRscParam.rscId = MMPM_RSC_ID_MPPS;
        mmpmRscParam.rscParam.pMppsReq = pMppsReq;
        mmpmRscExtParam.apiType = MMPM_API_TYPE_SYNC;
        mmpmRscExtParam.numOfReq = 1;
        mmpmRscExtParam.pReqArray = &mmpmRscParam;
        mmpmRscExtParam.pStsArray = &detailSts;
        PRINT_INFOEXT_2("clientID %u, mpps %u",
            clientId, pMppsReq->mppsTotal);

        sts = MMPM_Request_Ext(clientId, &mmpmRscExtParam);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            PRINT_ERROR_2("clientID %u, returned %u",
                clientId, detailSts);
        }
        /* If request is honored, verify the result */
        else
        {
            Adsppm_Verify(clientId, &mmpmRscExtParam, NULL);
        }
    }
    else
    {
        // This is an error in the test code
        PRINT_ERROR_0("Invalid ADSPPM client ID");
    }

    return sts;
}

MMPM_STATUS Test_RequestSleepLatency(
    uint32 clientId,
    uint32 microSec)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    MMPM_STATUS detailSts = MMPM_STATUS_FAILED;
    MmpmRscParamType mmpmRscParam;
    MmpmRscExtParamType mmpmRscExtParam;

    if(clientId)
    {
        mmpmRscParam.rscId = MMPM_RSC_ID_SLEEP_LATENCY;
        mmpmRscParam.rscParam.sleepMicroSec = microSec;
        mmpmRscExtParam.apiType = MMPM_API_TYPE_SYNC;
        mmpmRscExtParam.numOfReq = 1;
        mmpmRscExtParam.pReqArray = &mmpmRscParam;
        mmpmRscExtParam.pStsArray = &detailSts;
        PRINT_INFOEXT_2("clientID %u, microSec %u", clientId, microSec);

        sts = MMPM_Request_Ext(clientId, &mmpmRscExtParam);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            PRINT_ERROR_2("clientID %u, returned %u", clientId, detailSts);
        }
        else
        {
            Adsppm_Verify(clientId, &mmpmRscExtParam, NULL);
            PRINT_PASS_2("clientID %u, microSec %u", clientId, microSec);
        }
    }
    else
    {
        // This is an error in the test code
        PRINT_ERROR_0("Invalid ADSPPM client ID");
    }

    return sts;
}


MMPM_STATUS Test_RequestBandwidth(
    uint32 clientId,
    BwReqTestType *bw,
    uint32 numBw,
    void * expectedParams)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    MMPM_STATUS detailSts = MMPM_STATUS_FAILED;
    MmpmRscParamType mmpmRscParam;
    MmpmRscExtParamType mmpmRscExtParam;
    MmpmGenBwValType bandWidthArray[MAX_BW_REQUEST];
    MmpmGenBwReqType mmpmBwReqParam;
    uint32 i;

    if(clientId)
    {
        for(i = 0; i < numBw; i++)
        {
            bandWidthArray[i].busRoute.masterPort = bw[i].masterPort;
            bandWidthArray[i].busRoute.slavePort = bw[i].slavePort;
            bandWidthArray[i].bwValue.busBwValue.bwBytePerSec = bw[i].bwVal;
            bandWidthArray[i].bwValue.busBwValue.usagePercentage = bw[i].usagePercent;
            bandWidthArray[i].bwValue.busBwValue.usageType= bw[i].usageType;
            PRINT_INFOEXT_4(
                "numBw=%u, masterID=%u, slaveID=%u, bwBytePerSec=%u",
                i, bw[i].masterPort, bw[i].slavePort, bw[i].bwVal);
        }
        mmpmBwReqParam.numOfBw = numBw;
        mmpmBwReqParam.pBandWidthArray = bandWidthArray;
        mmpmRscParam.rscId = MMPM_RSC_ID_GENERIC_BW;
        mmpmRscParam.rscParam.pGenBwReq = &mmpmBwReqParam;
        mmpmRscExtParam.apiType = MMPM_API_TYPE_SYNC;
        mmpmRscExtParam.numOfReq = 1;
        mmpmRscExtParam.pReqArray = &mmpmRscParam;
        mmpmRscExtParam.pStsArray = &detailSts;

        sts = MMPM_Request_Ext(clientId, &mmpmRscExtParam);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            //PRINT_ERROR_2("clientID %u, returned %u", clientId, detailSts);
        }
        /* If request is honored, verify the result */
        else
        {
            if (Adsppm_Verify(clientId, &mmpmRscExtParam, expectedParams) == FALSE)
                sts = MMPM_STATUS_FAILED;
        }
    }
    else
    {
        // This is an error in the test code
        PRINT_ERROR_0("Invalid ADSPPM client ID");
    }

    return sts;
}

MMPM_STATUS Test_RequestBandwidthExt(uint32 clientId, BwReqTestType_Ext *bw, uint32 numBw)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, detailSts = MMPM_STATUS_FAILED;
    MmpmRscParamType mmpmRscParam;
    MmpmRscExtParamType mmpmRscExtParam;
    MmpmGenBwValType bandWidthArray[MAX_BW_REQUEST];
    MmpmGenBwReqType mmpmBwReqParam;
    uint32 i;

    if(clientId)
    {
        for(i = 0; i < numBw; i++)
        {
            bandWidthArray[i].busRoute.masterPort = bw[i].masterPort;
            bandWidthArray[i].busRoute.slavePort = bw[i].slavePort;
            bandWidthArray[i].bwValue.busBwValue.bwBytePerSec = bw[i].bwVal;
            bandWidthArray[i].bwValue.busBwValue.usagePercentage = bw[i].usagePercent;
            bandWidthArray[i].bwValue.busBwValue.usageType= bw[i].usageType;

            /*PRINT_ERROR_4(
                "numBw=%u, masterID=%u, slaveID=%u, bwBytePerSec=%u",
                i, bw[i].masterPort, bw[i].slavePort, bw[i].bwVal);
            PRINT_ERROR_2(
                "priorityId=%u streamId=%u",
                bandWidthArray[i].priorityId,bandWidthArray[i].streamId );*/
        }
        mmpmRscParam.rscId =
            bw->type == Bw_Generic_Ext ? MMPM_RSC_ID_GENERIC_BW_EXT : MMPM_RSC_ID_GENERIC_BW;
        mmpmBwReqParam.numOfBw = numBw;
        mmpmBwReqParam.pBandWidthArray = bandWidthArray;
        mmpmRscParam.rscParam.pGenBwReq = &mmpmBwReqParam;
        mmpmRscExtParam.apiType = MMPM_API_TYPE_SYNC;
        mmpmRscExtParam.numOfReq = 1;
        mmpmRscExtParam.pReqArray = &mmpmRscParam;
        mmpmRscExtParam.pStsArray = &detailSts;

        sts = MMPM_Request_Ext(clientId, &mmpmRscExtParam);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            PRINT_ERROR_2("clientID %u, returned %u", clientId, detailSts);
        }
        else
        {
            Adsppm_Verify(clientId, &mmpmRscExtParam, NULL);
        }
    }
    else
    {
        // This is an error in the test code
        PRINT_ERROR_0("Invalid ADSPPM client ID");
    }

    return sts;
}


MMPM_STATUS Test_RequestRegProg(
    uint32 clientId,
    MmpmRegProgMatchType match)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    MMPM_STATUS detailSts = MMPM_STATUS_FAILED;
    MmpmRscParamType mmpmRscParam;
    MmpmRscExtParamType mmpmRscExtParam;

    if(clientId)
    {
        mmpmRscParam.rscId = MMPM_RSC_ID_REG_PROG;
        mmpmRscParam.rscParam.regProgMatch = match;
        mmpmRscExtParam.apiType = MMPM_API_TYPE_SYNC;
        mmpmRscExtParam.numOfReq = 1;
        mmpmRscExtParam.pReqArray = &mmpmRscParam;
        mmpmRscExtParam.pStsArray = &detailSts;
        PRINT_INFOEXT_2("clientID %u, RegProgMatch %u", clientId, match);

        sts = MMPM_Request_Ext(clientId, &mmpmRscExtParam);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            if(MMPM_STATUS_UNSUPPORTED == detailSts)
            {
                sts = MMPM_STATUS_SUCCESS;
                PRINT_NA_1(
                    "Register programming not supported for this clientID %u",
                    clientId);
            }
            else
            {
                PRINT_ERROR_2("clientID %u, returned %u", clientId, detailSts);
            }
        }
        else
        {
            PRINT_PASS_1("clientID %u", clientId);
        }
    }
    else
    {
        // This is an error in the test code
        PRINT_ERROR_0("Invalid ADSPPM client ID");
    }

    return sts;
}


MMPM_STATUS Test_RequestPower(uint32 clientId)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    MMPM_STATUS detailSts = MMPM_STATUS_FAILED;
    MmpmRscParamType mmpmRscParam;
    MmpmRscExtParamType mmpmRscExtParam;

    if(clientId)
    {
        mmpmRscParam.rscId = MMPM_RSC_ID_POWER;
        mmpmRscExtParam.apiType = MMPM_API_TYPE_SYNC;
        mmpmRscExtParam.numOfReq = 1;
        mmpmRscExtParam.pReqArray = &mmpmRscParam;
        mmpmRscExtParam.pStsArray = &detailSts;
        PRINT_INFOEXT_1("clientID %u", clientId);

        sts = MMPM_Request_Ext(clientId, &mmpmRscExtParam);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            PRINT_ERROR_2("clientID %u, returned %u", clientId, detailSts);
        }
        else
        {
            PRINT_PASS_1("clientID %u", clientId);
        }
    }
    else
    {
        // This is an error in the test code
        PRINT_ERROR_0("Invalid ADSPPM client ID");
    }

    return sts;
}


MMPM_STATUS Test_RequestMemoryPower(
    uint32 clientId,
    MmpmMemIdType memType,
    MmpmMemPowerStateType powerState)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    MMPM_STATUS detailSts = MMPM_STATUS_FAILED;
    MmpmRscParamType mmpmRscParam;
    MmpmRscExtParamType mmpmRscExtParam;
    MmpmMemPowerReqParamType memPwrReq;

    if(clientId)
    {
        memPwrReq.memory = memType;
        memPwrReq.powerState = powerState;
        mmpmRscParam.rscId = MMPM_RSC_ID_MEM_POWER;
        mmpmRscParam.rscParam.pMemPowerState = &memPwrReq;
        mmpmRscExtParam.apiType = MMPM_API_TYPE_SYNC;
        mmpmRscExtParam.numOfReq = 1;
        mmpmRscExtParam.pReqArray = &mmpmRscParam;
        mmpmRscExtParam.pStsArray = &detailSts;
        PRINT_INFOEXT_3(
            "clientID %u, memType %u powerState %u",
            clientId, memType, powerState);

        sts = MMPM_Request_Ext(clientId, &mmpmRscExtParam);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            if(MMPM_STATUS_UNSUPPORTED == detailSts)
            {
                sts = MMPM_STATUS_SUCCESS;
                PRINT_NA_3(
                    "clientID %u, memType %u powerState %u not supported in this chipset",
                    clientId, memType, powerState);
            }
            else
            {
                PRINT_ERROR_4(
                    "clientID %u, memType %u powerState %u returned %u",
                    clientId, memType, powerState, detailSts);
            }
        }
        else
        {
            PRINT_PASS_3(
                "clientID %u, memType %u powerState %u",
                clientId, memType, powerState);
        }
    }
    else
    {
        // This is an error in the test code
        PRINT_ERROR_0("Invalid ADSPPM client ID");
    }

    return sts;
}


MMPM_STATUS Test_ReleaseResource(
    uint32 clientId,
    MmpmRscIdType resourceId)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    MMPM_STATUS detailSts = MMPM_STATUS_FAILED;
    MmpmRscParamType mmpmRscParam;
    MmpmRscExtParamType mmpmRscExtParam;

    if(clientId)
    {
        mmpmRscParam.rscId = resourceId;
        mmpmRscExtParam.apiType = MMPM_API_TYPE_SYNC;
        mmpmRscExtParam.numOfReq = 1;
        mmpmRscExtParam.pReqArray = &mmpmRscParam;
        mmpmRscExtParam.pStsArray = &detailSts;
        PRINT_INFOEXT_2("clientID %u resourceID %u", clientId, resourceId);

        sts = MMPM_Release_Ext(clientId, &mmpmRscExtParam);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            if(MMPM_STATUS_UNSUPPORTED == detailSts)
            {
                sts = MMPM_STATUS_SUCCESS;
                PRINT_NA_1(
                    "Resource %u Request is not supported in this chipset",
                    resourceId);
            }
            else
            {
                PRINT_ERROR_3(
                    "clientID %u resourceID %u, returned %u",
                    clientId, resourceId, detailSts);
            }
        }
        else
        {
            PRINT_PASS_2("clientID %u resourceID %u", clientId, resourceId);
        }
    }
    else
    {
        // This is an error in the test code
        PRINT_ERROR_1("resourceID %u, Invalid ADSPPM client ID", resourceId);
    }

    return sts;
}


MMPM_STATUS Test_Deregister(uint32 clientId)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;

    PRINT_INFOEXT_1("clientID %u", clientId);
    if(clientId)
    {
        sts = MMPM_Deregister_Ext(clientId);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            PRINT_ERROR_1("clientID %u", clientId);
        }
        else
        {
            PRINT_PASS_1("clientID %u", clientId);
        }
    }
    else
    {
        PRINT_ERROR_0("Invalid ADSPPM client ID");
    }

    return sts;
}


MMPM_STATUS Test_InfoClk(
    MmpmCoreIdType coreId,
    MmpmCoreInstanceIdType instanceId,
    uint32 clkId, //MmpmClkIdLpassType or MmpmClkIdComputeType or MmpmClkIdSlpiType
    uint32 *clkFreq)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    MmpmInfoDataType infoData;
    MmpmInfoClkFreqType retClkData;

    retClkData.clkId = clkId;
    retClkData.clkFreqHz = 0;
    retClkData.forceMeasure = 1;
    infoData.infoId = MMPM_INFO_ID_CLK_FREQ;
    infoData.coreId = coreId;
    infoData.instanceId = instanceId;
    infoData.info.pInfoClkFreqType = &retClkData;

    sts = MMPM_GetInfo(&infoData);
    if(MMPM_STATUS_SUCCESS != sts)
    {
        PRINT_ERROR_2("coreID %u instanceID %u", coreId, instanceId);
    }
    else
    {
        *clkFreq = retClkData.clkFreqHz;
        PRINT_PASS_2("ClkId = %u @ %u Hz", clkId, *clkFreq);
    }

    return sts;
}


MMPM_STATUS Test_InfoMaxMips(
    MmpmCoreIdType coreId,
    MmpmCoreInstanceIdType instanceId,
    uint32 *mipsValue)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    MmpmInfoDataType infoData;

    infoData.infoId = MMPM_INFO_ID_MIPS_MAX;
    infoData.coreId = coreId;
    infoData.instanceId = instanceId;
    infoData.info.mipsValue = 0;

    sts = MMPM_GetInfo(&infoData);
    if(MMPM_STATUS_SUCCESS != sts)
    {
        PRINT_ERROR_2("coreID %u instanceID %u", coreId, instanceId);
    }
    else
    {
        *mipsValue = infoData.info.mipsValue;
        PRINT_PASS_1("max mips = %u", *mipsValue);
    }

    return sts;
}


MMPM_STATUS Test_InfoMpps(
    MmpmInfoMppsType *pMppsInfo)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    MmpmInfoDataType infoData;

    infoData.infoId = MMPM_INFO_ID_MPPS;
    infoData.coreId = MMPM_CORE_ID_LPASS_ADSP;
    infoData.instanceId = MMPM_CORE_INSTANCE_0;
    infoData.info.pMppsInfo = pMppsInfo;

    sts = MMPM_GetInfo(&infoData);
    if(sts != MMPM_STATUS_SUCCESS)
    {
        PRINT_ERROR_0("Get MPPS info failed");
    }
    else
    {
        PRINT_PASS_1("aggregated MPPS = %u",
            pMppsInfo->aggregateMpps);
    }

    return sts;
}

MMPM_STATUS Test_InfoBw(
    MmpmCoreIdType coreId,
    MmpmCoreInstanceIdType instanceId,
    AdsppmInfoAggregatedBwType *bwInfo)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    AdsppmInfoPrivateType privateInfo;
    AdsppmInfoAggregatedBwType *getBwInfo;
    MmpmInfoDataType infoData;
    infoData.infoId = MMPM_INFO_ID_EXT_REQ;
    infoData.coreId = coreId;
    infoData.instanceId = instanceId;
    privateInfo.type = ADSPPM_PRIVATE_INFO_AGGREGATED_BW;
    infoData.info.pExtInfo = &privateInfo;

    sts = MMPM_GetInfo(&infoData);
    if(MMPM_STATUS_SUCCESS != sts)
    {
        PRINT_ERROR_2("coreID %u instanceID %u GetInfo Failed", coreId, instanceId);
    }
    else
    {
        getBwInfo =
            &(((AdsppmInfoPrivateType *) infoData.info.pExtInfo)->aggregatedBw);
        *bwInfo = *getBwInfo;
        PRINT_PASS_4("adspDdrBwAb = %u adspDdrBwIb = %u extAhbDDrBwAb = %u extAhbDdrBwIb = %u",
            (uint32)bwInfo->adspDdrBwAb, (uint32)bwInfo->adspDdrBwIb,
            (uint32)bwInfo->extAhbDdrBwAb, (uint32)bwInfo->extAhbDdrBwIb);
    }

    return sts;
}

MMPM_STATUS Test_InfoMaxBw(
    MmpmCoreIdType coreId,
    MmpmCoreInstanceIdType instanceId,
    uint64 *bwValue)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    MmpmInfoDataType infoData;

    infoData.infoId = MMPM_INFO_ID_BW_MAX;
    infoData.coreId = coreId;
    infoData.instanceId = instanceId;
    infoData.info.bwValue = 0;

    sts = MMPM_GetInfo(&infoData);
    if(MMPM_STATUS_SUCCESS != sts)
    {
        PRINT_ERROR_2("coreID %u instanceID %u", coreId, instanceId);
    }
    else
    {
        *bwValue = infoData.info.bwValue;
        PRINT_PASS_1("max bw = %u MB/s", *bwValue);
    }

    return sts;
}

MMPM_STATUS Test_InfoAhb(
    MmpmCoreIdType coreId,
    MmpmCoreInstanceIdType instanceId,
    AdsppmInfoAhbType *ahbInfo)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    AdsppmInfoPrivateType privateInfo;
    MmpmInfoDataType infoData;
    infoData.infoId = MMPM_INFO_ID_EXT_REQ;
    infoData.coreId = coreId;
    infoData.instanceId = instanceId;
    privateInfo.type = ADSPPM_PRIVATE_INFO_AHB;
    infoData.info.pExtInfo = &privateInfo;

    if(ahbInfo == NULL)
        return MMPM_STATUS_FAILED;

    sts = MMPM_GetInfo(&infoData);
    if(MMPM_STATUS_SUCCESS != sts)
    {
        PRINT_ERROR_2("coreID %u instanceID %u GetInfo Failed", coreId, instanceId);
    }
    else
    {
        *ahbInfo = ((AdsppmInfoPrivateType *) infoData.info.pExtInfo)->ahb;
        PRINT_PASS_1("ahbeFreqHz = %u", (uint32)ahbInfo->ahbeFreqHz);
    }

    return sts;
}

MMPM_STATUS Test_SetClientClass(
    uint32 clientId,
    MmpmClientClassType clientClass)
{

    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    MmpmParameterConfigType paramConfig;

    if(clientId)
    {
        paramConfig.paramId = MMPM_PARAM_ID_CLIENT_CLASS;
        paramConfig.pParamConfig = &clientClass;
        PRINT_INFOEXT_1("clientID %u", clientId);

        sts = MMPM_SetParameter(clientId, &paramConfig);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            PRINT_ERROR_1("clientID %u failed", clientId);
        }
        else
        {
            PRINT_PASS_1("clientID %u", clientId);
        }
    }
    else
    {
        // This is an error in the test code
        PRINT_ERROR_0("Invalid ADSPPM client ID");
    }

    return sts;

}

MMPM_STATUS Test_SetSysCacheStreamId(
    uint32 clientId,
    MmpmSysCacheStreamId streamId)
{

    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    MmpmParameterConfigType paramConfig;

    if(clientId)
    {
        paramConfig.paramId = MMPM_PARAM_ID_SYSCACHE_STREAM_ID;
        paramConfig.pParamConfig = &streamId;
        PRINT_INFOEXT_1("clientID %u", clientId);

        sts = MMPM_SetParameter(clientId, &paramConfig);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            PRINT_ERROR_1("clientID %u failed", clientId);
        }
        else
        {
            PRINT_PASS_1("clientID %u", streamId);
        }
    }
    else
    {
        // This is an error in the test code
        PRINT_ERROR_0("Invalid ADSPPM client ID");
    }

    return sts;

}


MMPM_STATUS Test_Thermal()
{
    MMPM_STATUS sts = MMPM_STATUS_SUCCESS;
    return sts;
}


boolean Test_CompareFreq(
    uint32 setClockFreq,
    uint32 readClockFreq)
{
    return((setClockFreq/10000000-readClockFreq/10000000) == 0);
}

