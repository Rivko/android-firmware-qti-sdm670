/*
* Copyright (c) 2015 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
@file: adsppm_test_dcvs_aggregation.c
@brief: Implementation of ADSPPM test module for DCVS aggregation.

$Header: //components/rel/core.qdsp6/2.1.c4/power/qdsp_pm/src/driver/test/adsppm_test_dcvs_aggregation.c#1 $
*/

#include "mmpm.h"
#include "asic.h"
#include "adsppm_dcvs.h"
#include "adsppm_test.h"
#include "adsppm_test_param.h"
#include "adsppm_test_utils.h"
#include "adsppm_test_asic_info.h"


#define DCVSAGGREGATION_TEST(num, sts) \
    ADSPPMTEST_LOG_TEST_HEADER(DCVS_AGGREGATION_##num##_NAME, \
        DCVS_AGGREGATION_##num##_DETAILS) \
    sts = Test_DcvsAggregation_##num(); \
    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_DcvsAggregation, \
        DCVS_AGGREGATION_##num##_NAME, sts) \

static uint64 bwVal = 10000000ULL;

static BwReqTestType bwReq =
{
    .masterPort = MMPM_BW_PORT_ID_ADSP_MASTER,
    .slavePort = MMPM_BW_PORT_ID_DDR_SLAVE,
    .bwVal = 10000000ULL,
    .usagePercent = 100,
    .usageType = MMPM_BW_USAGE_LPASS_EXT_CPU,
};

static uint32 mipsVal = 200;


static MMPM_STATUS changeDcvsParticipationToUpDown(uint32 clientId)
{

    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    MmpmDcvsParticipationType dcvsParticipation;
    MmpmParameterConfigType paramConfig;

    dcvsParticipation.enable = TRUE;
    dcvsParticipation.enableOpt = MMPM_DCVS_ADJUST_UP_DOWN;
    paramConfig.paramId = MMPM_PARAM_ID_DCVS_PARTICIPATION;
    paramConfig.pParamConfig = (void*) &dcvsParticipation;

    sts = MMPM_SetParameter(clientId, &paramConfig);

    return sts;

}


static MMPM_STATUS changeDcvsParticipationToUpOnly(uint32 clientId)
{

    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    MmpmDcvsParticipationType dcvsParticipation;
    MmpmParameterConfigType paramConfig;

    dcvsParticipation.enable = TRUE;
    dcvsParticipation.enableOpt = MMPM_DCVS_ADJUST_ONLY_UP;
    paramConfig.paramId = MMPM_PARAM_ID_DCVS_PARTICIPATION;
    paramConfig.pParamConfig = (void*) &dcvsParticipation;

    sts = MMPM_SetParameter(clientId, &paramConfig);

    return sts;

}


static MMPM_STATUS changeDcvsParticipationToDisable(uint32 clientId)
{

    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    MmpmDcvsParticipationType dcvsParticipation;
    MmpmParameterConfigType paramConfig;

    dcvsParticipation.enable = FALSE;
    dcvsParticipation.enableOpt = MMPM_DCVS_ADJUST_UP_DOWN;
    paramConfig.paramId = MMPM_PARAM_ID_DCVS_PARTICIPATION;
    paramConfig.pParamConfig = (void*) &dcvsParticipation;

    sts = MMPM_SetParameter(clientId, &paramConfig);

    return sts;

}

static MMPM_STATUS mockDcvsIbAbVote(uint64 iB, uint64 aB,
                                    AdsppmDcvsResourceId rsrcId)
{

    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPM_DCVS_STATUS dcvsSts = ADSPPM_DCVS_STATUS_FAILED;
    AdsppmDcvsResourceVoteType rscVote;

    rscVote.adsp2DdrBwInIbAb.iBw = iB;
    rscVote.adsp2DdrBwInIbAb.aBw = aB;

    dcvsSts = adsppm_dcvs_request(rsrcId, &rscVote);

    if(dcvsSts == ADSPPM_DCVS_STATUS_SUCCESS)
        sts = MMPM_STATUS_SUCCESS;

    return sts;

}

static MMPM_STATUS mockDcvsAdspClockVote(uint32 adspClock)
{

    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPM_DCVS_STATUS dcvsSts = ADSPPM_DCVS_STATUS_FAILED;
    AdsppmDcvsResourceVoteType rscVote;

    rscVote.adspClockInHz = adspClock * 1000000;

    dcvsSts = adsppm_dcvs_request(ADSPPM_DCVS_RSC_ID_ADSP_CLOCK, &rscVote);

    if(dcvsSts == ADSPPM_DCVS_STATUS_SUCCESS)
        sts = MMPM_STATUS_SUCCESS;

    return sts;

}

static MMPM_STATUS getDcvsEnableDisableFlag(boolean* dcvsEnableDisableFlag)
{

    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    MmpmInfoDataType infoData;

    infoData.infoId = MMPM_INFO_ID_DCVS_STATE;
    infoData.coreId = MMPM_CORE_ID_LPASS_ADSP;
    infoData.instanceId = MMPM_CORE_INSTANCE_0;
    infoData.info.dcvsState = FALSE;

    sts = MMPM_GetInfo(&infoData);
    if(sts != MMPM_STATUS_SUCCESS)
    {
        ADSPPMTEST_LOG_PRINTF_0(ADSPPMTEST_LOG_LEVEL_ERROR,
            "[ERROR] DCVS enable/disable flag query failed");
    }
    else
    {
        *dcvsEnableDisableFlag = infoData.info.dcvsState;
    }

    return sts;

}

static MMPM_STATUS checkDcvsEnableDisableFlag(boolean expectedFlag)
{

    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    boolean currentFlag = FALSE;

    sts = getDcvsEnableDisableFlag(&currentFlag);
    if(sts == MMPM_STATUS_SUCCESS)
    {
        if(expectedFlag != currentFlag)
        {
            sts = MMPM_STATUS_FAILED;
            ADSPPMTEST_LOG_PRINTF_2(ADSPPMTEST_LOG_LEVEL_ERROR,
                "[ERROR] unexpected DCVS enable/disable flag, "
                "query=%s, expected=%s",
                currentFlag?"TRUE":"FALSE",
                expectedFlag?"TRUE":"FALSE");
        }
    }

    return sts;

}

static MMPM_STATUS getDcvsDdrBwInfo(
    AdsppmInfoDcvsAdspDdrBwType *pDcvsAdspDdrBwInfo,
    AdsppmInfoPrivateTypeType    privateInfoType)
{

    if(pDcvsAdspDdrBwInfo == NULL)
    {
        ADSPPMTEST_LOG_PRINTF_0(ADSPPMTEST_LOG_LEVEL_ERROR,
            "[ERROR] DCVS ADSP DDR BW info query: NULL pointer");
        return MMPM_STATUS_FAILED;
    }

    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    MmpmInfoDataType infoData;
    AdsppmInfoPrivateType privateInfo;

    privateInfo.type = privateInfoType;
    infoData.infoId = MMPM_INFO_ID_EXT_REQ;
    infoData.coreId = MMPM_CORE_ID_LPASS_ADSP;
    infoData.instanceId = MMPM_CORE_INSTANCE_0;
    infoData.info.pExtInfo = &privateInfo;

    sts = MMPM_GetInfo(&infoData);
    if(sts != MMPM_STATUS_SUCCESS)
    {
        ADSPPMTEST_LOG_PRINTF_0(ADSPPMTEST_LOG_LEVEL_ERROR,
            "[ERROR] DCVS ADSP DDR BW info query failed");
    }
    else
    {
        *pDcvsAdspDdrBwInfo = privateInfo.adspDdrBw;
    }

    return sts;

}

static MMPM_STATUS checkDcvsDdrBwDcvsVote(
       uint64 expectedDcvsVoteIb, uint64 expectedDcvsVoteAb,
       AdsppmInfoPrivateTypeType  privateInfoType)
{

    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    AdsppmInfoDcvsAdspDdrBwType dcvsAdspDdrBwInfo;
    AdsppmBusBWDataIbAbType queryDcvsVote;

    sts = getDcvsDdrBwInfo(&dcvsAdspDdrBwInfo, privateInfoType);
    if(sts == MMPM_STATUS_SUCCESS)
    {
        queryDcvsVote = dcvsAdspDdrBwInfo.dcvsVote;

        if(queryDcvsVote.Ib != expectedDcvsVoteIb)
        {
            sts = MMPM_STATUS_FAILED;
            ADSPPMTEST_LOG_PRINTF_5(ADSPPMTEST_LOG_LEVEL_ERROR,
                "[ERROR] unexpected DCVS type %u DDR-BW dcvsVote IB, "
                "query=%llu, expected=%llu", privateInfoType,
                (uint32) queryDcvsVote.Ib,
                (uint32) (queryDcvsVote.Ib >> 32),
                (uint32) expectedDcvsVoteIb,
                (uint32) (expectedDcvsVoteIb >> 32));
        }

        if(queryDcvsVote.Ab != expectedDcvsVoteAb)
        {
            sts = MMPM_STATUS_FAILED;
            ADSPPMTEST_LOG_PRINTF_5(ADSPPMTEST_LOG_LEVEL_ERROR,
                "[ERROR] unexpected DCVS type %u DDR BW dcvsVote AB, "
                "query=%llu, expected=%llu",privateInfoType,
                (uint32) queryDcvsVote.Ab,
                (uint32) (queryDcvsVote.Ab >> 32),
                (uint32) expectedDcvsVoteAb,
                (uint32) (expectedDcvsVoteAb >> 32));
        }
    }

    return sts;
}

static MMPM_STATUS checkDcvsAdspDdrBwDcvsVote(
    uint64 expectedDcvsVoteIb, uint64 expectedDcvsVoteAb)

{
    return checkDcvsDdrBwDcvsVote( expectedDcvsVoteIb, expectedDcvsVoteAb,
                                   ADSPPM_PRIVATE_INFO_ADSPDDR_BW );
}

static MMPM_STATUS checkDcvsDdrBwClientsFloorVote(
    uint64 expectedClientsFloorVoteIb, uint64 expectedClientsFloorVoteAb,
    AdsppmInfoPrivateTypeType  privateInfoType )
{

    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    AdsppmInfoDcvsAdspDdrBwType dcvsAdspDdrBwInfo;
    AdsppmBusBWDataIbAbType queryClientsFloorVote;

    sts = getDcvsDdrBwInfo(&dcvsAdspDdrBwInfo,privateInfoType);
    if(sts == MMPM_STATUS_SUCCESS)
    {
        queryClientsFloorVote = dcvsAdspDdrBwInfo.clientsFloorVote;

        if(queryClientsFloorVote.Ib != expectedClientsFloorVoteIb)
        {
            sts = MMPM_STATUS_FAILED;
            ADSPPMTEST_LOG_PRINTF_5(ADSPPMTEST_LOG_LEVEL_ERROR,
                "[ERROR] unexpected DCVS type %u DDR BW clientsFloorVote IB, "
                "query=%llu, expected=%llu",privateInfoType,
                (uint32) queryClientsFloorVote.Ib,
                (uint32) (queryClientsFloorVote.Ib >> 32),
                (uint32) expectedClientsFloorVoteIb,
                (uint32) (expectedClientsFloorVoteIb >> 32));
        }

        if(queryClientsFloorVote.Ab != expectedClientsFloorVoteAb)
        {
            sts = MMPM_STATUS_FAILED;
            ADSPPMTEST_LOG_PRINTF_5(ADSPPMTEST_LOG_LEVEL_ERROR,
                "[ERROR] unexpected DCVS type %u DDR BW clientsFloorVote AB, "
                "query=%llu, expected=%llu", privateInfoType,
                (uint32) queryClientsFloorVote.Ab,
                (uint32) (queryClientsFloorVote.Ab >> 32),
                (uint32) expectedClientsFloorVoteAb,
                (uint32) (expectedClientsFloorVoteAb >> 32));
        }
    }

    return sts;
}

static MMPM_STATUS checkDcvsAdspDdrBwClientsFloorVote(
    uint64 expectedClientsFloorVoteIb, uint64 expectedClientsFloorVoteAb)
{
    return checkDcvsDdrBwClientsFloorVote(expectedClientsFloorVoteIb,
                                              expectedClientsFloorVoteAb,
                                              ADSPPM_PRIVATE_INFO_ADSPDDR_BW);
}

static MMPM_STATUS checkDcvsDdrBwClientsFinalVote(
    uint64 expectedClientsFinalVoteIb, uint64 expectedClientsFinalVoteAb,
    AdsppmInfoPrivateTypeType  privateInfoType)
{

    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    AdsppmInfoDcvsAdspDdrBwType dcvsAdspDdrBwInfo;
    AdsppmBusBWDataIbAbType queryClientsFinalVote;

    sts = getDcvsDdrBwInfo(&dcvsAdspDdrBwInfo, privateInfoType);
    if(sts == MMPM_STATUS_SUCCESS)
    {
        queryClientsFinalVote = dcvsAdspDdrBwInfo.clientsFinalVote;

        if(queryClientsFinalVote.Ib != expectedClientsFinalVoteIb)
        {
            sts = MMPM_STATUS_FAILED;
            ADSPPMTEST_LOG_PRINTF_5(ADSPPMTEST_LOG_LEVEL_ERROR,
                "[ERROR] unexpected DCVS type %u DDR BW clientsFinalVote IB, "
                "query=%llu, expected=%llu",privateInfoType,
                (uint32) queryClientsFinalVote.Ib,
                (uint32) (queryClientsFinalVote.Ib >> 32),
                (uint32) expectedClientsFinalVoteIb,
                (uint32) (expectedClientsFinalVoteIb >> 32));
        }

        if(queryClientsFinalVote.Ab != expectedClientsFinalVoteAb)
        {
            sts = MMPM_STATUS_FAILED;
            ADSPPMTEST_LOG_PRINTF_5(ADSPPMTEST_LOG_LEVEL_ERROR,
                "[ERROR] unexpected DCVS type %u DDR BW clientsFinalVote AB, "
                "query=%llu, expected=%llu", privateInfoType,
                (uint32) queryClientsFinalVote.Ab,
                (uint32) (queryClientsFinalVote.Ab >> 32),
                (uint32) expectedClientsFinalVoteAb,
                (uint32) (expectedClientsFinalVoteAb >> 32));
        }
    }

    return sts;
}


static MMPM_STATUS checkDcvsAdspDdrBwClientsFinalVote(
    uint64 expectedClientsFinalVoteIb, uint64 expectedClientsFinalVoteAb)
{
    return checkDcvsDdrBwClientsFinalVote(expectedClientsFinalVoteIb,
                                          expectedClientsFinalVoteAb,
                                          ADSPPM_PRIVATE_INFO_ADSPDDR_BW);
}

static MMPM_STATUS checkDcvsDdrBwFinalVoteToNpa(
    uint64 expectedFinalVoteToNpaIb, uint64 expectedFinalVoteToNpaAb,
    AdsppmInfoPrivateTypeType  privateInfoType)
{

    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    AdsppmInfoDcvsAdspDdrBwType dcvsAdspDdrBwInfo;
    AdsppmBusBWDataIbAbType queryFinalVoteToNpa;

    sts = getDcvsDdrBwInfo(&dcvsAdspDdrBwInfo, privateInfoType);
    if(sts == MMPM_STATUS_SUCCESS)
    {
        queryFinalVoteToNpa = dcvsAdspDdrBwInfo.finalVoteToNpa;

        if(queryFinalVoteToNpa.Ib != expectedFinalVoteToNpaIb)
        {
            sts = MMPM_STATUS_FAILED;
            ADSPPMTEST_LOG_PRINTF_5(ADSPPMTEST_LOG_LEVEL_ERROR,
                "[ERROR] unexpected DCVS type %u DDR BW finalVoteToNpa IB, "
                "query=%llu, expected=%llu", privateInfoType,
                (uint32) queryFinalVoteToNpa.Ib,
                (uint32) (queryFinalVoteToNpa.Ib >> 32),
                (uint32) expectedFinalVoteToNpaIb,
                (uint32) (expectedFinalVoteToNpaIb >> 32));
        }

        if(queryFinalVoteToNpa.Ab != expectedFinalVoteToNpaAb)
        {
            sts = MMPM_STATUS_FAILED;
            ADSPPMTEST_LOG_PRINTF_5(ADSPPMTEST_LOG_LEVEL_ERROR,
                "[ERROR] unexpected DCVS type %u DDR BW finalVoteToNpa AB, "
                "query=%llu, expected=%llu", privateInfoType,
                (uint32) queryFinalVoteToNpa.Ab,
                (uint32) (queryFinalVoteToNpa.Ab >> 32),
                (uint32) expectedFinalVoteToNpaAb,
                (uint32) (expectedFinalVoteToNpaAb >> 32));
        }
    }

    return sts;
}

static MMPM_STATUS checkDcvsAdspDdrBwFinalVoteToNpa(
    uint64 expectedFinalVoteToNpaIb, uint64 expectedFinalVoteToNpaAb)
{
    return checkDcvsDdrBwFinalVoteToNpa(expectedFinalVoteToNpaIb,
                                            expectedFinalVoteToNpaAb,
                                            ADSPPM_PRIVATE_INFO_ADSPDDR_BW);
}

static MMPM_STATUS getDcvsAdspClockInfo(
    AdsppmInfoDcvsAdspClockType *pDcvsAdspClockInfo)
{

    if(pDcvsAdspClockInfo == NULL)
    {
        ADSPPMTEST_LOG_PRINTF_0(ADSPPMTEST_LOG_LEVEL_ERROR,
            "[ERROR] DCVS ADSP clock info query: NULL pointer");
        return MMPM_STATUS_FAILED;
    }

    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    MmpmInfoDataType infoData;
    AdsppmInfoPrivateType privateInfo;

    privateInfo.type = ADSPPM_PRIVATE_INFO_ADSPCLOCK;
    infoData.infoId = MMPM_INFO_ID_EXT_REQ;
    infoData.coreId = MMPM_CORE_ID_LPASS_ADSP;
    infoData.instanceId = MMPM_CORE_INSTANCE_0;
    infoData.info.pExtInfo = &privateInfo;

    sts = MMPM_GetInfo(&infoData);
    if(sts != MMPM_STATUS_SUCCESS)
    {
        ADSPPMTEST_LOG_PRINTF_0(ADSPPMTEST_LOG_LEVEL_ERROR,
            "[ERROR] DCVS ADSP clock info query failed");
    }
    else
    {
        *pDcvsAdspClockInfo = privateInfo.adspClock;
    }

    return sts;

}

static MMPM_STATUS checkDcvsAdspClockDcvsVote(
    uint32 expectedDcvsVote)
{

    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    AdsppmInfoDcvsAdspClockType dcvsAdspClockInfo;
    uint32 queryDcvsVote;

    sts = getDcvsAdspClockInfo(&dcvsAdspClockInfo);
    if(sts == MMPM_STATUS_SUCCESS)
    {
        queryDcvsVote = dcvsAdspClockInfo.dcvsVote;

        if(queryDcvsVote != expectedDcvsVote * 1000000)
        {
            sts = MMPM_STATUS_FAILED;
            ADSPPMTEST_LOG_PRINTF_2(ADSPPMTEST_LOG_LEVEL_ERROR,
                "[ERROR] unexpected DCVS ADSP Clock dcvsVote, "
                "query=%u, expected=%u",
                queryDcvsVote, expectedDcvsVote * 1000000);
        }
    }

    return sts;

}

static MMPM_STATUS checkDcvsAdspClockClientsFloorVote(
    uint32 expectedClientsFloorVote)
{

    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    AdsppmInfoDcvsAdspClockType dcvsAdspClockInfo;
    uint32 queryClientsFloorVote;

    sts = getDcvsAdspClockInfo(&dcvsAdspClockInfo);
    if(sts == MMPM_STATUS_SUCCESS)
    {
        queryClientsFloorVote = dcvsAdspClockInfo.clientsFloorVote;

        /* Get the base vote for ADSP clock */
        expectedClientsFloorVote =
            ACM_AdjustParamValue(AsicFeatureId_Adsp_Clock_Scaling, expectedClientsFloorVote);

        if(queryClientsFloorVote != expectedClientsFloorVote * 1000000)
        {

            sts = MMPM_STATUS_FAILED;
            ADSPPMTEST_LOG_PRINTF_2(ADSPPMTEST_LOG_LEVEL_ERROR,
                "[ERROR] unexpected DCVS ADSP Clock clientsFloorVote, "
                "query=%u, expected=%u",
                queryClientsFloorVote, expectedClientsFloorVote * 1000000);
        }
    }

    return sts;

}

static MMPM_STATUS checkDcvsAdspClockClientsFinalVote(
    uint32 expectedClientsFinalVote)
{

    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    AdsppmInfoDcvsAdspClockType dcvsAdspClockInfo;
    uint32 queryClientsFinalVote;

    sts = getDcvsAdspClockInfo(&dcvsAdspClockInfo);
    if(sts == MMPM_STATUS_SUCCESS)
    {
        queryClientsFinalVote = dcvsAdspClockInfo.clientsFinalVote;
        expectedClientsFinalVote =
            ACM_AdjustParamValue(AsicFeatureId_Adsp_Clock_Scaling, expectedClientsFinalVote);

        if(queryClientsFinalVote != expectedClientsFinalVote * 1000000)
        {
            sts = MMPM_STATUS_FAILED;
            ADSPPMTEST_LOG_PRINTF_2(ADSPPMTEST_LOG_LEVEL_ERROR,
                "[ERROR] unexpected DCVS ADSP Clock clientsFinalVote, "
                "query=%u, expected=%u",
                queryClientsFinalVote, expectedClientsFinalVote * 1000000);
        }
    }

    return sts;

}

static MMPM_STATUS checkDcvsAdspClockFinalVoteToNpa(
    uint32 expectedFinalVoteToNpa)
{

    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    AdsppmInfoDcvsAdspClockType dcvsAdspClockInfo;
    uint32 queryFinalVoteToNpa;

    sts = getDcvsAdspClockInfo(&dcvsAdspClockInfo);
    if(sts == MMPM_STATUS_SUCCESS)
    {
        queryFinalVoteToNpa = dcvsAdspClockInfo.finalVoteToNpa;

        /* Get the base vote for ADSP clock */
        expectedFinalVoteToNpa =
            ACM_AdjustParamValue(AsicFeatureId_Adsp_Clock_Scaling, expectedFinalVoteToNpa);

        if(queryFinalVoteToNpa != expectedFinalVoteToNpa * 1000000)
        {
            sts = MMPM_STATUS_FAILED;
            ADSPPMTEST_LOG_PRINTF_2(ADSPPMTEST_LOG_LEVEL_ERROR,
                "[ERROR] unexpected DCVS ADSP Clock finalVoteToNpa, "
                "query=%u, expected=%u",
                queryFinalVoteToNpa, expectedFinalVoteToNpa * 1000000);
        }
    }

    return sts;

}


#define DCVS_AGGREGATION_01_NAME    "dcvs_aggregation_01"
#define DCVS_AGGREGATION_01_DETAILS "Test ADSPPM enable/disable DCVS due to clients BW vote"

MMPM_STATUS Test_DcvsAggregation_01(void)
{

    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    uint32 clientId = 0;

    // Client registration
    clientId = Test_Register(MMPM_CORE_ID_LPASS_ADSP, MMPM_CORE_INSTANCE_0,
        DCVS_AGGREGATION_01_NAME);
    if(clientId == 0)
        goto Test_DcvsAggregation_01_error;

    // Change client DCVS participation to enable DCVS
    sts = changeDcvsParticipationToUpDown(clientId);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_01_error;

    // Check DCVS is disabled
    sts = checkDcvsEnableDisableFlag(FALSE);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_01_error;

    // Request DDR BW to enable DCVS
    sts = Test_RequestBandwidth(clientId, &bwReq, 1, NULL);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_01_error;

    // Check DCVS is enabled
    sts = checkDcvsEnableDisableFlag(TRUE);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_01_error;

    // Release DDR BW to disable DCVS
    sts = Test_ReleaseResource(clientId, MMPM_RSC_ID_GENERIC_BW);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_01_error;

    // Check DCVS is disabled
    sts = checkDcvsEnableDisableFlag(FALSE);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_01_error;

    // De-register client
    sts = Test_Deregister(clientId);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_01_error;

    return MMPM_STATUS_SUCCESS;

    Test_DcvsAggregation_01_error:

    if(clientId)
        Test_Deregister(clientId);

    return MMPM_STATUS_FAILED;

}

#define DCVS_AGGREGATION_02_NAME    "dcvs_aggregation_02"
#define DCVS_AGGREGATION_02_DETAILS "Test ADSPPM enable/disable DCVS due to clients MIPS vote"

MMPM_STATUS Test_DcvsAggregation_02(void)
{

    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    uint32 clientId = 0;

    // Client registration
    clientId = Test_Register(MMPM_CORE_ID_LPASS_ADSP, MMPM_CORE_INSTANCE_0,
        DCVS_AGGREGATION_02_NAME);
    if(clientId == 0)
        goto Test_DcvsAggregation_02_error;

    // Change client DCVS participation to enable DCVS
    sts = changeDcvsParticipationToUpDown(clientId);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_02_error;

    // Check DCVS is disabled
    sts = checkDcvsEnableDisableFlag(FALSE);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_02_error;

    // Request MIPS to enable DCVS
    sts = Test_RequestMips(clientId, mipsVal);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_02_error;

    // Check DCVS is enabled
    sts = checkDcvsEnableDisableFlag(TRUE);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_02_error;

    // Release MIPS to disable DCVS
    sts = Test_ReleaseResource(clientId, MMPM_RSC_ID_MIPS_EXT);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_02_error;

    // Check DCVS is disabled
    sts = checkDcvsEnableDisableFlag(FALSE);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_02_error;

    // De-register client
    sts = Test_Deregister(clientId);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_02_error;

    return MMPM_STATUS_SUCCESS;

    Test_DcvsAggregation_02_error:

    if(clientId)
        Test_Deregister(clientId);

    return MMPM_STATUS_FAILED;

}

#define DCVS_AGGREGATION_03_NAME    "dcvs_aggregation_03"
#define DCVS_AGGREGATION_03_DETAILS "Test ADSPPM floor BW aggregation"

MMPM_STATUS Test_DcvsAggregation_03(void)
{

    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    uint32 clientId1 = 0, clientId2 = 0;

    // Client1 registration
    clientId1 = Test_Register(MMPM_CORE_ID_LPASS_ADSP, MMPM_CORE_INSTANCE_0,
        DCVS_AGGREGATION_03_NAME"_c1");
    if(clientId1 == 0)
        goto Test_DcvsAggregation_03_error;

    // Change client1 DCVS participation to enable DCVS and Up Down
    sts = changeDcvsParticipationToUpDown(clientId1);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_03_error;

    // Client2 registration
    clientId2 = Test_Register(MMPM_CORE_ID_LPASS_ADSP, MMPM_CORE_INSTANCE_0,
        DCVS_AGGREGATION_03_NAME"_c2");
    if(clientId2 == 0)
        goto Test_DcvsAggregation_03_error;

    // Change client2 DCVS participation to disable DCVS
    sts = changeDcvsParticipationToDisable(clientId2);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_03_error;

    // Client1 requests DDR BW
    sts = Test_RequestBandwidth(clientId1, &bwReq, 1, NULL);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_03_error;

    // Client2 requests DDR BW
    sts = Test_RequestBandwidth(clientId2, &bwReq, 1, NULL);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_03_error;

    // Check DCVS is enabled
    sts = checkDcvsEnableDisableFlag(TRUE);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_03_error;

    // Check clientsFloorVote
    sts = checkDcvsAdspDdrBwClientsFloorVote(bwVal, bwVal);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_03_error;

    // Check clientsFinalVote
    sts = checkDcvsAdspDdrBwClientsFinalVote(bwVal, bwVal*2);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_03_error;

    // De-register client1
    sts = Test_Deregister(clientId1);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_03_error;

    // De-register client2
    sts = Test_Deregister(clientId2);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_03_error;

    return MMPM_STATUS_SUCCESS;

    Test_DcvsAggregation_03_error:

    if(clientId1)
        Test_Deregister(clientId1);

    if(clientId2)
        Test_Deregister(clientId2);

    return MMPM_STATUS_FAILED;

}

#define DCVS_AGGREGATION_04_NAME    "dcvs_aggregation_04"
#define DCVS_AGGREGATION_04_DETAILS "Test ADSPPM floor ADSP clock aggregation"

MMPM_STATUS Test_DcvsAggregation_04(void)
{

    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    uint32 clientId1 = 0, clientId2 = 0;

    // Client1 registration
    clientId1 = Test_Register(MMPM_CORE_ID_LPASS_ADSP, MMPM_CORE_INSTANCE_0,
        DCVS_AGGREGATION_04_NAME"_c1");
    if(clientId1 == 0)
        goto Test_DcvsAggregation_04_error;

    // Change client1 DCVS participation to enable DCVS and Up Down
    sts = changeDcvsParticipationToUpDown(clientId1);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_04_error;

    // Client2 registration
    clientId2 = Test_Register(MMPM_CORE_ID_LPASS_ADSP, MMPM_CORE_INSTANCE_0,
        DCVS_AGGREGATION_04_NAME"_c2");
    if(clientId2 == 0)
        goto Test_DcvsAggregation_04_error;

    // Change client2 DCVS participation to disable DCVS
    sts = changeDcvsParticipationToDisable(clientId2);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_04_error;

    // Client1 requests MIPS
    sts = Test_RequestMips(clientId1, mipsVal);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_04_error;

    // Client2 requests MIPS
    sts = Test_RequestMips(clientId2, mipsVal);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_04_error;

    // Check clientsFloorVote
    sts = checkDcvsAdspClockClientsFloorVote(mipsVal);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_04_error;

    // Check clientsFinalVote
    sts = checkDcvsAdspClockClientsFinalVote(mipsVal*2);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_04_error;

    // De-register client1
    sts = Test_Deregister(clientId1);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_04_error;

    // De-register client2
    sts = Test_Deregister(clientId2);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_04_error;

    return MMPM_STATUS_SUCCESS;

    Test_DcvsAggregation_04_error:

    if(clientId1)
        Test_Deregister(clientId1);

    if(clientId2)
        Test_Deregister(clientId2);

    return MMPM_STATUS_FAILED;

}

#define DCVS_AGGREGATION_05_NAME    "dcvs_aggregation_05"
#define DCVS_AGGREGATION_05_DETAILS "Test DCVS higher than current BW vote"

MMPM_STATUS Test_DcvsAggregation_05(void)
{

    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    uint32 clientId = 0;

    // Client registration
    clientId = Test_Register(MMPM_CORE_ID_LPASS_ADSP, MMPM_CORE_INSTANCE_0,
        DCVS_AGGREGATION_05_NAME);
    if(clientId == 0)
        goto Test_DcvsAggregation_05_error;

    // Change client DCVS participation to enable DCVS and Up Down
    sts = changeDcvsParticipationToUpDown(clientId);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_05_error;

    // Request DDR BW to enable DCVS
    sts = Test_RequestBandwidth(clientId, &bwReq, 1, NULL);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_05_error;

    // Mock up DCVS ADSP DDR BW vote
    sts = mockDcvsIbAbVote(bwVal*2, bwVal*2, ADSPPM_DCVS_RSC_ID_ADSP_DDR_BW);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_05_error;

    // Check dcvsVote
    sts = checkDcvsAdspDdrBwDcvsVote(bwVal*2, bwVal*2);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_05_error;

    // Check finalVoteToNpa
    sts = checkDcvsAdspDdrBwFinalVoteToNpa(bwVal*2, bwVal*2);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_05_error;

    // De-register client
    sts = Test_Deregister(clientId);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_05_error;

    return MMPM_STATUS_SUCCESS;

    Test_DcvsAggregation_05_error:

    if(clientId)
        Test_Deregister(clientId);

    return MMPM_STATUS_FAILED;

}

#define DCVS_AGGREGATION_06_NAME    "dcvs_aggregation_06"
#define DCVS_AGGREGATION_06_DETAILS "Test DCVS lower than current BW vote"

MMPM_STATUS Test_DcvsAggregation_06(void)
{

    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    uint32 clientId = 0;

    // Client registration
    clientId = Test_Register(MMPM_CORE_ID_LPASS_ADSP, MMPM_CORE_INSTANCE_0,
        DCVS_AGGREGATION_06_NAME);
    if(clientId == 0)
        goto Test_DcvsAggregation_06_error;

    // Change client DCVS participation to enable DCVS and Up Down
    sts = changeDcvsParticipationToUpDown(clientId);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_06_error;

    // Request DDR BW to enable DCVS
    sts = Test_RequestBandwidth(clientId, &bwReq, 1, NULL);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_06_error;

    // Mock up DCVS ADSP DDR BW vote
    sts = mockDcvsIbAbVote(bwVal/2, bwVal/2, ADSPPM_DCVS_RSC_ID_ADSP_DDR_BW);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_06_error;

    // Check dcvsVote
    sts = checkDcvsAdspDdrBwDcvsVote(bwVal/2, bwVal/2);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_06_error;

    // Check finalVoteToNpa
    sts = checkDcvsAdspDdrBwFinalVoteToNpa(bwVal/2, bwVal/2);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_06_error;

    // De-register client
    sts = Test_Deregister(clientId);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_06_error;

    return MMPM_STATUS_SUCCESS;

    Test_DcvsAggregation_06_error:

    if(clientId)
        Test_Deregister(clientId);

    return MMPM_STATUS_FAILED;

}

#define DCVS_AGGREGATION_07_NAME    "dcvs_aggregation_07"
#define DCVS_AGGREGATION_07_DETAILS "Test DCVS higher than current ADSP clock vote"

MMPM_STATUS Test_DcvsAggregation_07(void)
{

    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    uint32 clientId = 0;

    // Client registration
    clientId = Test_Register(MMPM_CORE_ID_LPASS_ADSP, MMPM_CORE_INSTANCE_0,
        DCVS_AGGREGATION_07_NAME);
    if(clientId == 0)
        goto Test_DcvsAggregation_07_error;

    // Change client DCVS participation to enable DCVS and Up Down
    sts = changeDcvsParticipationToUpDown(clientId);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_07_error;

    // Request MIPS to enable DCVS
    sts = Test_RequestMips(clientId, mipsVal);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_07_error;

    // Mock up DCVS ADSP Clock vote
    sts = mockDcvsAdspClockVote(mipsVal*2);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_07_error;

    // Check dcvsVote
    sts = checkDcvsAdspClockDcvsVote(mipsVal*2);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_07_error;

    // Check finalVoteToNpa
    sts = checkDcvsAdspClockFinalVoteToNpa(mipsVal*2);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_07_error;

    // De-register client
    sts = Test_Deregister(clientId);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_07_error;

    return MMPM_STATUS_SUCCESS;

    Test_DcvsAggregation_07_error:

    if(clientId)
        Test_Deregister(clientId);

    return MMPM_STATUS_FAILED;

}

#define DCVS_AGGREGATION_08_NAME    "dcvs_aggregation_08"
#define DCVS_AGGREGATION_08_DETAILS "Test DCVS lower than current ADSP clock vote"

MMPM_STATUS Test_DcvsAggregation_08(void)
{

    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    uint32 clientId = 0;

    // Client registration
    clientId = Test_Register(MMPM_CORE_ID_LPASS_ADSP, MMPM_CORE_INSTANCE_0,
        DCVS_AGGREGATION_08_NAME);
    if(clientId == 0)
        goto Test_DcvsAggregation_08_error;

    // Change client DCVS participation to enable DCVS and Up Down
    sts = changeDcvsParticipationToUpDown(clientId);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_08_error;

    // Request MIPS to enable DCVS
    sts = Test_RequestMips(clientId, mipsVal);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_08_error;

    // Mock up DCVS ADSP Clock vote
    sts = mockDcvsAdspClockVote(mipsVal/2);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_08_error;

    // Check dcvsVote
    sts = checkDcvsAdspClockDcvsVote(mipsVal/2);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_08_error;

    // Check finalVoteToNpa
    sts = checkDcvsAdspClockFinalVoteToNpa(mipsVal/2);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_08_error;

    // De-register client
    sts = Test_Deregister(clientId);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_08_error;

    return MMPM_STATUS_SUCCESS;

    Test_DcvsAggregation_08_error:

    if(clientId)
        Test_Deregister(clientId);

    return MMPM_STATUS_FAILED;

}

#define DCVS_AGGREGATION_09_NAME    "dcvs_aggregation_09"
#define DCVS_AGGREGATION_09_DETAILS "Test aggregation due to new client BW vote"

MMPM_STATUS Test_DcvsAggregation_09(void)
{

    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    uint32 clientId1 = 0, clientId2 = 0;

    // Client1 registration
    clientId1 = Test_Register(MMPM_CORE_ID_LPASS_ADSP, MMPM_CORE_INSTANCE_0,
        DCVS_AGGREGATION_09_NAME"_c1");
    if(clientId1 == 0)
        goto Test_DcvsAggregation_09_error;

    // Change client1 DCVS participation to enable DCVS and Up Only
    sts = changeDcvsParticipationToUpOnly(clientId1);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_09_error;

    // Client2 registration
    clientId2 = Test_Register(MMPM_CORE_ID_LPASS_ADSP, MMPM_CORE_INSTANCE_0,
        DCVS_AGGREGATION_09_NAME"_c2");
    if(clientId2 == 0)
        goto Test_DcvsAggregation_09_error;

    // Change client2 DCVS participation to enable DCVS and Up Down
    sts = changeDcvsParticipationToUpDown(clientId2);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_09_error;

    // Client1 requests DDR BW
    sts = Test_RequestBandwidth(clientId1, &bwReq, 1, NULL);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_09_error;

    // Mock up DCVS ADSP DDR BW vote
    sts = mockDcvsIbAbVote(bwVal*2, bwVal*2, ADSPPM_DCVS_RSC_ID_ADSP_DDR_BW);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_09_error;

    // Check dcvsVote
    sts = checkDcvsAdspDdrBwDcvsVote(bwVal*2, bwVal*2);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_09_error;

    // Check clientsFloorVote
    sts = checkDcvsAdspDdrBwClientsFloorVote(bwVal, bwVal);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_09_error;

    // Check clientsFinalVote
    sts = checkDcvsAdspDdrBwClientsFinalVote(bwVal, bwVal);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_09_error;

    // Check finalVoteToNpa
    sts = checkDcvsAdspDdrBwFinalVoteToNpa(bwVal*2, bwVal*2);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_09_error;

    // Client2 requests DDR BW
    sts = Test_RequestBandwidth(clientId2, &bwReq, 1, NULL);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_09_error;

    // Check clientsFloorVote
    sts = checkDcvsAdspDdrBwClientsFloorVote(bwVal, bwVal);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_09_error;

    // Check clientsFinalVote
    sts = checkDcvsAdspDdrBwClientsFinalVote(bwVal, bwVal*2);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_09_error;

    // Check finalVoteToNpa
    sts = checkDcvsAdspDdrBwFinalVoteToNpa(bwVal*2, bwVal*2);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_09_error;

    // De-register client1
    sts = Test_Deregister(clientId1);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_09_error;

    // De-register client2
    sts = Test_Deregister(clientId2);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_09_error;

    return MMPM_STATUS_SUCCESS;

    Test_DcvsAggregation_09_error:

    if(clientId1)
        Test_Deregister(clientId1);

    if(clientId2)
        Test_Deregister(clientId2);

    return MMPM_STATUS_FAILED;

}

#define DCVS_AGGREGATION_10_NAME    "dcvs_aggregation_10"
#define DCVS_AGGREGATION_10_DETAILS "Test aggregation due to new client MIPS vote"

MMPM_STATUS Test_DcvsAggregation_10(void)
{

    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    uint32 clientId1 = 0, clientId2 = 0;

    // Client1 registration
    clientId1 = Test_Register(MMPM_CORE_ID_LPASS_ADSP, MMPM_CORE_INSTANCE_0,
        DCVS_AGGREGATION_10_NAME"_c1");
    if(clientId1 == 0)
        goto Test_DcvsAggregation_10_error;

    // Change client1 DCVS participation to enable DCVS and Up Only
    sts = changeDcvsParticipationToUpOnly(clientId1);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_10_error;

    // Client2 registration
    clientId2 = Test_Register(MMPM_CORE_ID_LPASS_ADSP, MMPM_CORE_INSTANCE_0,
        DCVS_AGGREGATION_10_NAME"_c2");
    if(clientId2 == 0)
        goto Test_DcvsAggregation_10_error;

    // Change client2 DCVS participation to enable DCVS and Up Down
    sts = changeDcvsParticipationToUpDown(clientId2);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_10_error;

    // Client1 requests MIPS
    sts = Test_RequestMips(clientId1, mipsVal);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_10_error;

    // Mock up DCVS ADSP Clock vote
    sts = mockDcvsAdspClockVote(mipsVal*2);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_10_error;

    // Check dcvsVote
    sts = checkDcvsAdspClockDcvsVote(mipsVal*2);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_10_error;

    // Check clientsFloorVote
    sts = checkDcvsAdspClockClientsFloorVote(mipsVal);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_10_error;

    // Check clientsFinalVote
    sts = checkDcvsAdspClockClientsFinalVote(mipsVal);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_10_error;

    // Check finalVoteToNpa
    sts = checkDcvsAdspClockFinalVoteToNpa(mipsVal*2);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_10_error;

    // Client2 requests MIPS
    sts = Test_RequestMips(clientId2, mipsVal);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_10_error;

    // Check clientsFloorVote
    sts = checkDcvsAdspClockClientsFloorVote(mipsVal);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_10_error;

    // Check clientsFinalVote
    sts = checkDcvsAdspClockClientsFinalVote(mipsVal*2);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_10_error;

    // Check finalVoteToNpa
    sts = checkDcvsAdspClockFinalVoteToNpa(mipsVal*2);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_10_error;

    // De-register client1
    sts = Test_Deregister(clientId1);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_10_error;

    // De-register client2
    sts = Test_Deregister(clientId2);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_10_error;

    return MMPM_STATUS_SUCCESS;

    Test_DcvsAggregation_10_error:

    if(clientId1)
        Test_Deregister(clientId1);

    if(clientId2)
        Test_Deregister(clientId2);

    return MMPM_STATUS_FAILED;

}

#define DCVS_AGGREGATION_11_NAME    "dcvs_aggregation_11"
#define DCVS_AGGREGATION_11_DETAILS "Test aggregation due to releasing BW vote"

MMPM_STATUS Test_DcvsAggregation_11(void)
{

    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    uint32 clientId1 = 0, clientId2 = 0;

    // Client1 registration
    clientId1 = Test_Register(MMPM_CORE_ID_LPASS_ADSP, MMPM_CORE_INSTANCE_0,
        DCVS_AGGREGATION_09_NAME"_c1");
    if(clientId1 == 0)
        goto Test_DcvsAggregation_11_error;

    // Change client1 DCVS participation to enable DCVS and Up Only
    sts = changeDcvsParticipationToUpOnly(clientId1);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_11_error;

    // Client2 registration
    clientId2 = Test_Register(MMPM_CORE_ID_LPASS_ADSP, MMPM_CORE_INSTANCE_0,
        DCVS_AGGREGATION_09_NAME"_c2");
    if(clientId2 == 0)
        goto Test_DcvsAggregation_11_error;

    // Change client2 DCVS participation to enable DCVS and Up Down
    sts = changeDcvsParticipationToUpDown(clientId2);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_11_error;

    // Client1 requests DDR BW
    sts = Test_RequestBandwidth(clientId1, &bwReq, 1, NULL);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_11_error;

    // Client2 requests DDR BW
    sts = Test_RequestBandwidth(clientId2, &bwReq, 1, NULL);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_11_error;

    // Mock up DCVS ADSP DDR BW vote
    sts = mockDcvsIbAbVote(bwVal*2, bwVal*2, ADSPPM_DCVS_RSC_ID_ADSP_DDR_BW);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_11_error;

    // Check dcvsVote
    sts = checkDcvsAdspDdrBwDcvsVote(bwVal*2, bwVal*2);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_11_error;

    // Check clientsFloorVote
    sts = checkDcvsAdspDdrBwClientsFloorVote(bwVal, bwVal);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_11_error;

    // Check clientsFinalVote
    sts = checkDcvsAdspDdrBwClientsFinalVote(bwVal, bwVal*2);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_11_error;

    // Check finalVoteToNpa
    sts = checkDcvsAdspDdrBwFinalVoteToNpa(bwVal*2, bwVal*2);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_11_error;

    // Client2 releases DDR BW
    sts = Test_ReleaseResource(clientId2, MMPM_RSC_ID_GENERIC_BW);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_11_error;

    // Check clientsFloorVote
    sts = checkDcvsAdspDdrBwClientsFloorVote(bwVal, bwVal);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_11_error;

    // Check clientsFinalVote
    sts = checkDcvsAdspDdrBwClientsFinalVote(bwVal, bwVal);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_11_error;

    // Check finalVoteToNpa
    sts = checkDcvsAdspDdrBwFinalVoteToNpa(bwVal*2, bwVal*2);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_11_error;

    // De-register client1
    sts = Test_Deregister(clientId1);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_11_error;

    // De-register client2
    sts = Test_Deregister(clientId2);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_11_error;

    return MMPM_STATUS_SUCCESS;

    Test_DcvsAggregation_11_error:

    if(clientId1)
        Test_Deregister(clientId1);

    if(clientId2)
        Test_Deregister(clientId2);

    return MMPM_STATUS_FAILED;

}

#define DCVS_AGGREGATION_12_NAME    "dcvs_aggregation_12"
#define DCVS_AGGREGATION_12_DETAILS "Test aggregation due to releasing MIPS vote"

MMPM_STATUS Test_DcvsAggregation_12(void)
{

    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    uint32 clientId1 = 0, clientId2 = 0;

    // Client1 registration
    clientId1 = Test_Register(MMPM_CORE_ID_LPASS_ADSP, MMPM_CORE_INSTANCE_0,
        DCVS_AGGREGATION_10_NAME"_c1");
    if(clientId1 == 0)
        goto Test_DcvsAggregation_12_error;

    // Change client1 DCVS participation to enable DCVS and Up Only
    sts = changeDcvsParticipationToUpOnly(clientId1);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_12_error;

    // Client2 registration
    clientId2 = Test_Register(MMPM_CORE_ID_LPASS_ADSP, MMPM_CORE_INSTANCE_0,
        DCVS_AGGREGATION_10_NAME"_c2");
    if(clientId2 == 0)
        goto Test_DcvsAggregation_12_error;

    // Change client2 DCVS participation to enable DCVS and Up Down
    sts = changeDcvsParticipationToUpDown(clientId2);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_12_error;

    // Client1 requests MIPS
    sts = Test_RequestMips(clientId1, mipsVal);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_12_error;

    // Client2 requests MIPS
    sts = Test_RequestMips(clientId2, mipsVal);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_12_error;

    // Mock up DCVS ADSP Clock vote
    sts = mockDcvsAdspClockVote(mipsVal*2);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_12_error;

    // Check dcvsVote
    sts = checkDcvsAdspClockDcvsVote(mipsVal*2);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_12_error;

    // Check clientsFloorVote
    sts = checkDcvsAdspClockClientsFloorVote(mipsVal);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_12_error;

    // Check clientsFinalVote
    sts = checkDcvsAdspClockClientsFinalVote(mipsVal*2);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_12_error;

    // Check finalVoteToNpa
    sts = checkDcvsAdspClockFinalVoteToNpa(mipsVal*2);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_12_error;

    // Client2 releases MIPS
    sts = Test_ReleaseResource(clientId2, MMPM_RSC_ID_MIPS_EXT);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_12_error;

    // Check clientsFloorVote
    sts = checkDcvsAdspClockClientsFloorVote(mipsVal);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_12_error;

    // Check clientsFinalVote
    sts = checkDcvsAdspClockClientsFinalVote(mipsVal);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_12_error;

    // Check finalVoteToNpa
    sts = checkDcvsAdspClockFinalVoteToNpa(mipsVal*2);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_12_error;

    // De-register client1
    sts = Test_Deregister(clientId1);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_12_error;

    // De-register client2
    sts = Test_Deregister(clientId2);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_12_error;

    return MMPM_STATUS_SUCCESS;

    Test_DcvsAggregation_12_error:

    if(clientId1)
        Test_Deregister(clientId1);

    if(clientId2)
        Test_Deregister(clientId2);

    return MMPM_STATUS_FAILED;

}

#define DCVS_AGGREGATION_13_NAME    "dcvs_aggregation_13"
#define DCVS_AGGREGATION_13_DETAILS "Test DCVS ADSP DDR BW vote of zero"

MMPM_STATUS Test_DcvsAggregation_13(void)
{

    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    uint32 clientId = 0;

    // Client registration
    clientId = Test_Register(MMPM_CORE_ID_LPASS_ADSP, MMPM_CORE_INSTANCE_0,
        DCVS_AGGREGATION_13_NAME);
    if(clientId == 0)
        goto Test_DcvsAggregation_13_error;

    // Change client DCVS participation to enable DCVS and Up Down
    sts = changeDcvsParticipationToUpDown(clientId);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_13_error;

    // Request DDR BW to enable DCVS
    sts = Test_RequestBandwidth(clientId, &bwReq, 1, NULL);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_13_error;

    // Mock up DCVS ADSP DDR BW vote
    sts = mockDcvsIbAbVote(bwVal/2, bwVal/2, ADSPPM_DCVS_RSC_ID_ADSP_DDR_BW);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_13_error;

    // Check dcvsVote
    sts = checkDcvsAdspDdrBwDcvsVote(bwVal/2, bwVal/2);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_13_error;

    // Check clientsFinalVote
    sts = checkDcvsAdspDdrBwClientsFinalVote(bwVal, bwVal);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_13_error;

    // Check finalVoteToNpa
    sts = checkDcvsAdspDdrBwFinalVoteToNpa(bwVal/2, bwVal/2);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_13_error;

    // Mock up DCVS ADSP DDR BW vote to zero
    sts = mockDcvsIbAbVote(0, 0, ADSPPM_DCVS_RSC_ID_ADSP_DDR_BW);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_13_error;

    // Check dcvsVote
    sts = checkDcvsAdspDdrBwDcvsVote(0 ,0);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_13_error;

    // Check clientsFinalVote
    sts = checkDcvsAdspDdrBwClientsFinalVote(bwVal, bwVal);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_13_error;

    // Check finalVoteToNpa
    sts = checkDcvsAdspDdrBwFinalVoteToNpa(bwVal, bwVal);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_13_error;

    // De-register client
    sts = Test_Deregister(clientId);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_13_error;

    return MMPM_STATUS_SUCCESS;

    Test_DcvsAggregation_13_error:

    if(clientId)
        Test_Deregister(clientId);

    return MMPM_STATUS_FAILED;

}

#define DCVS_AGGREGATION_14_NAME    "dcvs_aggregation_14"
#define DCVS_AGGREGATION_14_DETAILS "Test DCVS ADSP Clock vote of zero"

MMPM_STATUS Test_DcvsAggregation_14(void)
{

    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    uint32 clientId = 0;

    // Client registration
    clientId = Test_Register(MMPM_CORE_ID_LPASS_ADSP, MMPM_CORE_INSTANCE_0,
        DCVS_AGGREGATION_14_NAME);
    if(clientId == 0)
        goto Test_DcvsAggregation_14_error;

    // Change client DCVS participation to enable DCVS and Up Down
    sts = changeDcvsParticipationToUpDown(clientId);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_14_error;

    // Request MIPS to enable DCVS
    sts = Test_RequestMips(clientId, mipsVal);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_14_error;

    // Mock up DCVS ADSP Clock vote
    sts = mockDcvsAdspClockVote(mipsVal/2);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_14_error;

    // Check dcvsVote
    sts = checkDcvsAdspClockDcvsVote(mipsVal/2);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_14_error;

    // Check clientsFinalVote
    sts = checkDcvsAdspClockClientsFinalVote(mipsVal);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_14_error;

    // Check finalVoteToNpa
    sts = checkDcvsAdspClockFinalVoteToNpa(mipsVal/2);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_14_error;

    // Mock up DCVS ADSP Clock vote to zero
    sts = mockDcvsAdspClockVote(0);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_14_error;

    // Check dcvsVote
    sts = checkDcvsAdspClockDcvsVote(0);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_14_error;

    // Check clientsFinalVote
    sts = checkDcvsAdspClockClientsFinalVote(mipsVal);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_14_error;

    // Check finalVoteToNpa
    sts = checkDcvsAdspClockFinalVoteToNpa(mipsVal);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_14_error;

    // De-register client
    sts = Test_Deregister(clientId);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_14_error;

    return MMPM_STATUS_SUCCESS;

    Test_DcvsAggregation_14_error:

    if(clientId)
        Test_Deregister(clientId);

    return MMPM_STATUS_FAILED;

}

#define DCVS_AGGREGATION_15_NAME    "dcvs_aggregation_15"
#define DCVS_AGGREGATION_15_DETAILS "Test DCVS disable updates ADSP clock vote to NPA"

MMPM_STATUS Test_DcvsAggregation_15(void)
{

    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    uint32 clientId1 = 0, clientId2 = 0;

    // Client1 registration
    clientId1 = Test_Register(MMPM_CORE_ID_LPASS_ADSP, MMPM_CORE_INSTANCE_0,
        DCVS_AGGREGATION_15_NAME"_c1");
    if(clientId1 == 0)
        goto Test_DcvsAggregation_15_error;

    // Change client1 DCVS participation to disable DCVS
    sts = changeDcvsParticipationToDisable(clientId1);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_15_error;

    // Client2 registration
    clientId2 = Test_Register(MMPM_CORE_ID_LPASS_ADSP, MMPM_CORE_INSTANCE_0,
        DCVS_AGGREGATION_15_NAME"_c2");
    if(clientId2 == 0)
        goto Test_DcvsAggregation_15_error;

    // Change client2 DCVS participation to enable DCVS and Up Down
    sts = changeDcvsParticipationToUpDown(clientId2);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_15_error;

    // Client1 requests MIPS
    sts = Test_RequestMips(clientId1, mipsVal);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_15_error;

    // Client2 Request DDR BW to enable DCVS
    sts = Test_RequestBandwidth(clientId2, &bwReq, 1, NULL);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_15_error;

    // Mock up DCVS ADSP Clock vote
    sts = mockDcvsAdspClockVote(mipsVal*2);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_15_error;

    // Check dcvsVote
    sts = checkDcvsAdspClockDcvsVote(mipsVal*2);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_15_error;

    // Check finalVoteToNpa
    sts = checkDcvsAdspClockFinalVoteToNpa(mipsVal*2);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_15_error;

    // De-register client2
    sts = Test_Deregister(clientId2);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_15_error;

    // Check finalVoteToNpa
    sts = checkDcvsAdspClockFinalVoteToNpa(mipsVal);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_15_error;

    // De-register client1
    sts = Test_Deregister(clientId1);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_15_error;

    return MMPM_STATUS_SUCCESS;

    Test_DcvsAggregation_15_error:

    if(clientId1)
        Test_Deregister(clientId1);

    if(clientId2)
        Test_Deregister(clientId2);

    return MMPM_STATUS_FAILED;

}

#define DCVS_AGGREGATION_16_NAME    "dcvs_aggregation_16"
#define DCVS_AGGREGATION_16_DETAILS "Test DCVS disable updates ADSP DDR BW vote to NPA"

MMPM_STATUS Test_DcvsAggregation_16(void)
{

    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    uint32 clientId1 = 0, clientId2 = 0;

    // Client1 registration
    clientId1 = Test_Register(MMPM_CORE_ID_LPASS_ADSP, MMPM_CORE_INSTANCE_0,
        DCVS_AGGREGATION_16_NAME"_c1");
    if(clientId1 == 0)
        goto Test_DcvsAggregation_16_error;

    // Change client1 DCVS participation to disable DCVS
    sts = changeDcvsParticipationToDisable(clientId1);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_16_error;

    // Client2 registration
    clientId2 = Test_Register(MMPM_CORE_ID_LPASS_ADSP, MMPM_CORE_INSTANCE_0,
        DCVS_AGGREGATION_15_NAME"_c2");
    if(clientId2 == 0)
        goto Test_DcvsAggregation_16_error;

    // Change client2 DCVS participation to enable DCVS and Up Down
    sts = changeDcvsParticipationToUpDown(clientId2);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_16_error;

    // Client1 requests DDR BW
    sts = Test_RequestBandwidth(clientId1, &bwReq, 1, NULL);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_16_error;

    // Client2 Request MIPS to enable DCVS
    sts = Test_RequestMips(clientId2, mipsVal);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_16_error;

    // Mock up DCVS ADSP DDR BW vote
    sts = mockDcvsIbAbVote(bwVal*2, bwVal*2, ADSPPM_DCVS_RSC_ID_ADSP_DDR_BW);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_16_error;

    // Check dcvsVote
    sts = checkDcvsAdspDdrBwDcvsVote(bwVal*2, bwVal*2);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_16_error;

    // Check finalVoteToNpa
    sts = checkDcvsAdspDdrBwFinalVoteToNpa(bwVal*2, bwVal*2);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_16_error;

    // De-register client2
    sts = Test_Deregister(clientId2);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_16_error;

    // Check finalVoteToNpa
    sts = checkDcvsAdspDdrBwFinalVoteToNpa(bwVal, bwVal);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_16_error;

    // De-register client1
    sts = Test_Deregister(clientId1);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_16_error;

    return MMPM_STATUS_SUCCESS;

    Test_DcvsAggregation_16_error:

    if(clientId1)
        Test_Deregister(clientId1);

    if(clientId2)
        Test_Deregister(clientId2);

    return MMPM_STATUS_FAILED;

}

#define DCVS_AGGREGATION_17_NAME    "dcvs_aggregation_17"
#define DCVS_AGGREGATION_17_DETAILS "Test Clients Floor Vote update when enable UpDown DCVS"

MMPM_STATUS Test_DcvsAggregation_17(void)
{

    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    uint32 clientId = 0;

    // Client registration
    clientId = Test_Register(MMPM_CORE_ID_LPASS_ADSP, MMPM_CORE_INSTANCE_0,
        DCVS_AGGREGATION_17_NAME);
    if(clientId == 0)
        goto Test_DcvsAggregation_17_error;

    // Change client DCVS participation to disable DCVS
    sts = changeDcvsParticipationToDisable(clientId);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_17_error;

    // Check DCVS is disabled
    sts = checkDcvsEnableDisableFlag(FALSE);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_17_error;

    // Request DDR BW
    sts = Test_RequestBandwidth(clientId, &bwReq, 1, NULL);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_17_error;

  // Check clientsFloorVote
    sts = checkDcvsAdspDdrBwClientsFloorVote(bwVal, bwVal);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_17_error;

  // Check DCVS is disabled
    sts = checkDcvsEnableDisableFlag(FALSE);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_17_error;

  // Change client DCVS participation to enable DCVS and Up Down
    sts = changeDcvsParticipationToUpDown(clientId);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_17_error;

    // Check DCVS is enabled
    sts = checkDcvsEnableDisableFlag(TRUE);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_17_error;

  // Check clientsFloorVote
    sts = checkDcvsAdspDdrBwClientsFloorVote(0, 0);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_17_error;

    // Release DDR BW to disable DCVS
    sts = Test_ReleaseResource(clientId, MMPM_RSC_ID_GENERIC_BW);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_17_error;

    // Check DCVS is disabled
    sts = checkDcvsEnableDisableFlag(FALSE);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_17_error;

    // De-register client
    sts = Test_Deregister(clientId);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_DcvsAggregation_17_error;

    return MMPM_STATUS_SUCCESS;

    Test_DcvsAggregation_17_error:

    if(clientId)
        Test_Deregister(clientId);

    return MMPM_STATUS_FAILED;

}


#define DCVS_CDSP_DCVS_01_NAME    "cdsp_dcvs_01"
#define DCVS_CDSP_DCVS_01_DETAILS "Test HCP bandwidth vote"

MMPM_STATUS Test_cdsp_dcvs_01(void)
{

    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    uint32 clientId1 = 0;
    uint64 hcpBWVal = 10000000ULL;
    BwReqTestType hcpBWReq =
    {
        .masterPort = MMPM_BW_PORT_ID_VAPSS_HCP_MASTER,
        .slavePort = MMPM_BW_PORT_ID_DDR_SLAVE,
        .bwVal = 10000000ULL,
        .usagePercent = 100,
        .usageType = MMPM_BW_USAGE_LPASS_EXT_CPU,
    };

    // Client1 registration
    clientId1 = Test_Register(MMPM_CORE_ID_LPASS_ADSP, MMPM_CORE_INSTANCE_0,
        DCVS_CDSP_DCVS_01_NAME"_c1");
    if(clientId1 == 0)
        goto Test_CDSP_Dcvs_01_error;

    // Change client1 DCVS participation to enable DCVS and Up Only
    sts = changeDcvsParticipationToUpOnly(clientId1);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_CDSP_Dcvs_01_error;

    // Client1 requests DDR BW
    sts = Test_RequestBandwidth(clientId1, &hcpBWReq, 1, NULL);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_CDSP_Dcvs_01_error;

    // Mock up DCVS ADSP DDR BW vote
    sts = mockDcvsIbAbVote(hcpBWVal*2, hcpBWVal*2, ADSPPM_DCVS_RSC_ID_HCP_DDR_BW);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_CDSP_Dcvs_01_error;

    // Check HCP dcvsVote
    sts = checkDcvsDdrBwDcvsVote(hcpBWVal*2, hcpBWVal*2, ADSPPM_PRIVATE_INFO_HCPDDR_BW);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_CDSP_Dcvs_01_error;

    // Check clientsFloorVote
    sts = checkDcvsDdrBwClientsFloorVote(hcpBWVal, hcpBWVal, ADSPPM_PRIVATE_INFO_HCPDDR_BW);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_CDSP_Dcvs_01_error;

    // Check clientsFinalVote
    sts = checkDcvsDdrBwClientsFinalVote(bwVal, bwVal, ADSPPM_PRIVATE_INFO_HCPDDR_BW);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_CDSP_Dcvs_01_error;

    // Check finalVoteToNpa
    sts = checkDcvsDdrBwFinalVoteToNpa(bwVal*2, bwVal*2, ADSPPM_PRIVATE_INFO_HCPDDR_BW);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_CDSP_Dcvs_01_error;

    // De-register client1
    sts = Test_Deregister(clientId1);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_CDSP_Dcvs_01_error;

    return MMPM_STATUS_SUCCESS;

    Test_CDSP_Dcvs_01_error:

    if(clientId1)
        Test_Deregister(clientId1);

    return MMPM_STATUS_FAILED;
}



#define DCVS_CDSP_DCVS_02_NAME    "cdsp_dcvs_01"
#define DCVS_CDSP_DCVS_02_DETAILS "Test DMA bandwidth vote"

MMPM_STATUS Test_cdsp_dcvs_02(void)
{

    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    uint32 clientId1 = 0;
    uint64 dmaBWVal = 10000000ULL;
    BwReqTestType hcpBWReq =
    {
        .masterPort = MMPM_BW_PORT_ID_VAPSS_DMA_MASTER,
        .slavePort = MMPM_BW_PORT_ID_DDR_SLAVE,
        .bwVal = 10000000ULL,
        .usagePercent = 100,
        .usageType = MMPM_BW_USAGE_LPASS_EXT_CPU,
    };

    // Client1 registration
    clientId1 = Test_Register(MMPM_CORE_ID_LPASS_ADSP, MMPM_CORE_INSTANCE_0,
        DCVS_CDSP_DCVS_01_NAME"_c1");
    if(clientId1 == 0)
        goto Test_CDSP_Dcvs_01_error;

    // Change client1 DCVS participation to enable DCVS and Up Only
    sts = changeDcvsParticipationToUpOnly(clientId1);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_CDSP_Dcvs_01_error;

    // Client1 requests DDR BW
    sts = Test_RequestBandwidth(clientId1, &hcpBWReq, 1, NULL);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_CDSP_Dcvs_01_error;

    // Mock up DCVS ADSP DDR BW vote
    sts = mockDcvsIbAbVote(dmaBWVal*2, dmaBWVal*2, ADSPPM_DCVS_RSC_ID_DMA_DDR_BW);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_CDSP_Dcvs_01_error;

    // Check HCP dcvsVote
    sts = checkDcvsDdrBwDcvsVote(dmaBWVal*2, dmaBWVal*2, ADSPPM_PRIVATE_INFO_DMADDR_BW);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_CDSP_Dcvs_01_error;

    // Check clientsFloorVote
    sts = checkDcvsDdrBwClientsFloorVote(dmaBWVal, dmaBWVal, ADSPPM_PRIVATE_INFO_DMADDR_BW);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_CDSP_Dcvs_01_error;

    // Check clientsFinalVote
    sts = checkDcvsDdrBwClientsFinalVote(bwVal, bwVal, ADSPPM_PRIVATE_INFO_DMADDR_BW);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_CDSP_Dcvs_01_error;

    // Check finalVoteToNpa
    sts = checkDcvsDdrBwFinalVoteToNpa(bwVal*2, bwVal*2, ADSPPM_PRIVATE_INFO_DMADDR_BW);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_CDSP_Dcvs_01_error;

    // De-register client1
    sts = Test_Deregister(clientId1);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_CDSP_Dcvs_01_error;

    return MMPM_STATUS_SUCCESS;

    Test_CDSP_Dcvs_01_error:

    if(clientId1)
        Test_Deregister(clientId1);

    return MMPM_STATUS_FAILED;
}





















#if 0
#define DCVS_CDSP_DCVS_01_NAME    "cdsp_dcvs_01"
#define DCVS_CDSP_DCVS_01_DETAILS "Test HCP bandwidth vote"

MMPM_STATUS Test_cdsp_dcvs_01(void)
{

    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    uint32 clientId1 = 0, clientId2 = 0;

    BwReqTestType hcpBWReq =
    {
        .masterPort = MMPM_BW_PORT_ID_VAPSS_HCP_MASTER,
        .slavePort = MMPM_BW_PORT_ID_DDR_SLAVE,
        .bwVal = 10000000ULL,
        .usagePercent = 100,
        .usageType = MMPM_BW_USAGE_LPASS_EXT_CPU,
    };


    // Client1 registration
    clientId1 = Test_Register(MMPM_CORE_ID_LPASS_ADSP, MMPM_CORE_INSTANCE_0,
        DCVS_CDSP_DCVS_01_NAME"_c1");
    if(clientId1 == 0)
        goto Test_CDSP_Dcvs_01_error;

    // Change client1 DCVS participation to enable DCVS and Up Only
    sts = changeDcvsParticipationToUpOnly(clientId1);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_CDSP_Dcvs_01_error;

    // Client2 registration
    clientId2 = Test_Register(MMPM_CORE_ID_LPASS_ADSP, MMPM_CORE_INSTANCE_0,
        DCVS_CDSP_DCVS_01_NAME"_c2");
    if(clientId2 == 0)
        goto Test_CDSP_Dcvs_01_error;

    // Change client2 DCVS participation to enable DCVS and Up Down
    sts = changeDcvsParticipationToUpDown(clientId2);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_CDSP_Dcvs_01_error;

    // Client1 requests DDR BW
    sts = Test_RequestBandwidth(clientId1, &bwReq, 1, NULL);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_CDSP_Dcvs_01_error;

    // Mock up DCVS ADSP DDR BW vote
    sts = mockDcvsIbAbVote(bwVal*2, bwVal*2);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_CDSP_Dcvs_01_error;

    // Check dcvsVote
    sts = checkDcvsAdspDdrBwDcvsVote(bwVal*2, bwVal*2);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_CDSP_Dcvs_01_error;

    // Check clientsFloorVote
    sts = checkDcvsAdspDdrBwClientsFloorVote(bwVal, bwVal);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_CDSP_Dcvs_01_error;

    // Check clientsFinalVote
    sts = checkDcvsAdspDdrBwClientsFinalVote(bwVal, bwVal);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_CDSP_Dcvs_01_error;

    // Check finalVoteToNpa
    sts = checkDcvsAdspDdrBwFinalVoteToNpa(bwVal*2, bwVal*2);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_CDSP_Dcvs_01_error;

    // Client2 requests DDR BW
    sts = Test_RequestBandwidth(clientId2, &bwReq, 1, NULL);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_CDSP_Dcvs_01_error;

    // Check clientsFloorVote
    sts = checkDcvsAdspDdrBwClientsFloorVote(bwVal, bwVal);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_CDSP_Dcvs_01_error;

    // Check clientsFinalVote
    sts = checkDcvsAdspDdrBwClientsFinalVote(bwVal, bwVal*2);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_CDSP_Dcvs_01_error;

    // Check finalVoteToNpa
    sts = checkDcvsAdspDdrBwFinalVoteToNpa(bwVal*2, bwVal*2);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_CDSP_Dcvs_01_error;

    // De-register client1
    sts = Test_Deregister(clientId1);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_CDSP_Dcvs_01_error;

    // De-register client2
    sts = Test_Deregister(clientId2);
    if(sts != MMPM_STATUS_SUCCESS)
        goto Test_CDSP_Dcvs_01_error;

    return MMPM_STATUS_SUCCESS;

    Test_CDSP_Dcvs_01_error:

    if(clientId1)
        Test_Deregister(clientId1);

    if(clientId2)
        Test_Deregister(clientId2);

    return MMPM_STATUS_FAILED;

}
#endif

void Test_DcvsAggregation()
{

    MMPM_STATUS sts = MMPM_STATUS_FAILED;

    DCVSAGGREGATION_TEST(01, sts)
    DCVSAGGREGATION_TEST(02, sts)
    DCVSAGGREGATION_TEST(03, sts)
    DCVSAGGREGATION_TEST(04, sts)
    DCVSAGGREGATION_TEST(05, sts)
    DCVSAGGREGATION_TEST(06, sts)
    DCVSAGGREGATION_TEST(07, sts)
    //DCVSAGGREGATION_TEST(08, sts)
    DCVSAGGREGATION_TEST(09, sts)
    DCVSAGGREGATION_TEST(10, sts)
    DCVSAGGREGATION_TEST(11, sts)
    DCVSAGGREGATION_TEST(12, sts)
    DCVSAGGREGATION_TEST(13, sts)
    //DCVSAGGREGATION_TEST(14, sts)
    DCVSAGGREGATION_TEST(15, sts)
    DCVSAGGREGATION_TEST(16, sts)
    DCVSAGGREGATION_TEST(17, sts)

}

void Test_CDSP_DcvsAggregation()
{

    MMPM_STATUS sts = MMPM_STATUS_FAILED;

    /* For test 1-17, it's the same as ADSP because they specify
       the path from Q6 --> DDR
    */
    DCVSAGGREGATION_TEST(01, sts)
    DCVSAGGREGATION_TEST(02, sts)
    DCVSAGGREGATION_TEST(03, sts)
    DCVSAGGREGATION_TEST(04, sts)
    DCVSAGGREGATION_TEST(05, sts)
    DCVSAGGREGATION_TEST(06, sts)
    DCVSAGGREGATION_TEST(07, sts)
    //DCVSAGGREGATION_TEST(08, sts)
    DCVSAGGREGATION_TEST(09, sts)
    DCVSAGGREGATION_TEST(10, sts)
    DCVSAGGREGATION_TEST(11, sts)
    DCVSAGGREGATION_TEST(12, sts)
    DCVSAGGREGATION_TEST(13, sts)
    //DCVSAGGREGATION_TEST(14, sts)
    DCVSAGGREGATION_TEST(15, sts)
    DCVSAGGREGATION_TEST(16, sts)
    DCVSAGGREGATION_TEST(17, sts)

    ADSPPMTEST_LOG_TEST_HEADER(DCVS_CDSP_DCVS_01_NAME, DCVS_CDSP_DCVS_01_DETAILS )
    sts = Test_cdsp_dcvs_01();
    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_DcvsAggregation, DCVS_CDSP_DCVS_01_NAME, sts)

    ADSPPMTEST_LOG_TEST_HEADER(DCVS_CDSP_DCVS_02_NAME, DCVS_CDSP_DCVS_02_DETAILS )
    sts = Test_cdsp_dcvs_02();
    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_DcvsAggregation, DCVS_CDSP_DCVS_02_NAME, sts)

}
