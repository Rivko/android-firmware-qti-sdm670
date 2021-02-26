/*
* Copyright (c) 2014 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

#include "DALSys.h"
#include "rcinit.h"
#include "asic.h"
#include "mmpm.h"
#include "adsppm.h"
#include "busmgr.h"
#include "cachemgr.h"
#include "adsppm_utils.h"
#include "adsppm_test.h"
#include "adsppm_test_utils.h"
#include "adsppm_test_verify.h"
#include "adsppm_test_asic_info.h"
#include "clientmgr.h"
#include "requestmgr.h"
#include "hal_clkrgm.h"
#include "npa.h"
#include "CoreVerify.h"
#include "coreUtils.h"


/**
 * ADSPPM Test Verify
 *
 * This file contains all the verification related functions. It
 * breaks down to three secontions:
 * 1. DECLARATIONS : contains function/static declaration and
 * macros
 *
 * 2. DATA STRUCTURE: contains the data structure used in
 * testing.
 *
 * 3. FUNCTIONS & API: Contains functions and API
 * implementation.
 */

/************************** FORWARD DECLARATION ***************************
 ========================================================================*/
/**
 * @enum BUSMgrBwTypeType - define bus bw type
 */
typedef enum
{
    BUSMgr_Bw_None              = 0,
    BUSMgr_Bw_Internal_Only     = 1,
    BUSMgr_Bw_External_Only     = 2,
    BUSMgr_Bw_Internal_External = 3,
    BUSMgr_Bw_ForceTo8bits      = 0x7f
} BUSMgrBwTypeType;

typedef struct
{

} AdsppmUT_Bus_Aggr_Params;


/* Extern functions */
extern BUSMgrBwTypeType busMgrGetBwType(AsicBusPortConnectionType masterPortConnection, AsicBusPortConnectionType slavePortConnection);

/* Verify functions: Use to verify the results compared with the value which ADSPPM requested */
void AdsppmHWNotSupported(uint32 clientId, MmpmRscExtParamType * params,
                          void *expectedParams, AdsppmStatusType * result);
void AdsppmHWRegProgVerify(uint32 clientId, MmpmRscExtParamType * params,
                           void *expectedParams, AdsppmStatusType * result);
void AdsppmHWCoreClkVerify(uint32 clientId, MmpmRscExtParamType * params,
                           void *expectedParams, AdsppmStatusType * result);
void AdsppmHWCoreClkDomainVerify(uint32 clientId, MmpmRscExtParamType * params,
                                 void *expectedParams, AdsppmStatusType * result);
void AdsppmHWSleepLatencyVerify(uint32 clientId, MmpmRscExtParamType * params,
                                void *expectedParams, AdsppmStatusType * result);
void AdsppmHWGenericBWVerify(uint32 clientId, MmpmRscExtParamType * params,
                             void *expectedParams, AdsppmStatusType * result);
void AdsppmHwMipsVerify(uint32 clientId, MmpmRscExtParamType * params,
                        void *expectedParams, AdsppmStatusType * result);
void AdsppmHWMemPowerVerify(uint32 clientId, MmpmRscExtParamType * params,
                            void *expectedParams, AdsppmStatusType * result);


/* Read functions: Use to read the value from NPA, etc. nodes */
void AdsppmUTHWRead_Clk(uint32 * hwValue, void * hwParams);
void AdsppmUTHWRead_BW_BIMC(uint32 * hwValue, void * hwParams);
void AdsppmUTHWRead_BW_SNOC(uint32 * hwValue, void * hwParams);
void AdsppmUTHWRead_AHBIX(uint32 * hwValue, void * hwParams);
void AdsppmUTHWRead_AHBE(uint32 * hwValue, void * hwParams);
void AdsppmUTHWRead_Sleep_Latency(uint32 * hwValue, void * hwParams);


/* Compute functions: Compute from ADSPPM Inputs to Hardware read values */
boolean AdsppmTest_Bus_Compute(MmpmRscExtParamType * reqParams, void *expectedParams,
                               void * infoParams, uint32 *result);
boolean AdsppmTest_AHBIX_Compute(MmpmRscExtParamType * reqParams, void *expectedParams,
                                 void * infoParams, uint32 *result);
boolean AdsppmTest_MIPS_Compute(MmpmRscExtParamType * reqParams, void *expectedParams,
                                 void * infoParams, uint32 *result);

/* Other functions */
BUSMgrBwTypeType AdsppmTest_GetBusRouteType(MmpmRscExtParamType * params, const AdsppmClientType * client);
void AdsppmTest_Set_Bus_Impacted(BUSMgrBwTypeType busType, MmpmBwPortIdType masterPort, MmpmBwPortIdType slavePort,
                                 boolean *is_snoc, boolean *is_bimc, boolean *is_ahb);
void AdsppmTest_Get_CoreInstanceId(uint32 clientId, MmpmCoreInstanceIdType *instanceId, MmpmCoreIdType *coreId);
uint32 AdsppmTest_Set_Bus_Bandwidth(AdsppmResrcID resrcId, MmpmBwPortIdType masterPort, MmpmBwPortIdType slavePort);
void AdsppmTest_ExtBW_Aggregation(AdsppmBusBWDataIbAbType *bwData, AdsppmUT_Bus_Params *busInfo);
void AdsppmTest_IntBW_Aggregation(AdsppmBusBWDataIbAbType *bwData);

/* Extern */
extern uint32 mipsMgr_CalcNumDominantThreads( uint32 mppsTotal, uint32 maxMpps, uint32 secondMaxMpps );
extern void BusMgr_ApplyAdspDdrBwConcurrencyFactor(uint32 numVotes, uint64 maxBw, uint64* pAb, uint64* pIb,
    AsicCompensatedBWFudgeFactor *compensatedBWFudgeFactorValues, AsicBwConcurrencySettingsType* settings );

/************************ DATA STRUCTURE SECTION *************************
 ========================================================================*/

/* Verify function that correspond to External Clients. */

#if (defined(ADSPPM_TEST_845) && defined(ADSPPM_TEST_COMPUTE))
const AdsppmUTExtRscPropertiesType AdsppmUTExtRscProperties[MMPM_RSC_ID_MAX] =
{
    [MMPM_RSC_ID_CORE_CLK]        = { AdsppmHWCoreClkVerify },
    [MMPM_RSC_ID_MIPS_EXT]        = { AdsppmHwMipsVerify },
    [MMPM_RSC_ID_MPPS]            = { AdsppmHwMipsVerify },

    [MMPM_RSC_ID_GENERIC_BW]      = { AdsppmHWNotSupported },
    [MMPM_RSC_ID_GENERIC_BW_EXT]  = { AdsppmHWNotSupported },
    [MMPM_RSC_ID_REG_PROG]        = { AdsppmHWNotSupported },
    [MMPM_RSC_ID_CORE_CLK_DOMAIN] = { AdsppmHWNotSupported },
    [MMPM_RSC_ID_SLEEP_LATENCY]   = { AdsppmHWNotSupported },
    [MMPM_RSC_ID_MEM_POWER]       = { AdsppmHWNotSupported },
    [MMPM_RSC_ID_VREG]            = { AdsppmHWNotSupported },
    [MMPM_RSC_ID_MEM_BW]          = { AdsppmHWNotSupported },
    [MMPM_RSC_ID_AXI_EN]          = { AdsppmHWNotSupported },
    [MMPM_RSC_ID_MIPS]            = { AdsppmHWNotSupported },
    [MMPM_RSC_ID_ACTIVE_STATE]    = { AdsppmHWNotSupported },
    [MMPM_RSC_ID_PMIC_GPIO]       = { AdsppmHWNotSupported },
    [MMPM_RSC_ID_RESET]           = { AdsppmHWNotSupported },
    [MMPM_RSC_ID_THERMAL]         = { AdsppmHWNotSupported },
};
#elif (defined(ADSPPM_TEST_845) && defined(ADSPPM_TEST_SLPI))
/* defined(ADSPPM_TEST_670) : AsicFeatureId_LpassSlpiDSP*/
const AdsppmUTExtRscPropertiesType AdsppmUTExtRscProperties[MMPM_RSC_ID_MAX] =
{
    [MMPM_RSC_ID_CORE_CLK]        = { AdsppmHWCoreClkVerify },
    [MMPM_RSC_ID_MIPS_EXT]        = { AdsppmHwMipsVerify },
    [MMPM_RSC_ID_MPPS]            = { AdsppmHwMipsVerify },

    [MMPM_RSC_ID_GENERIC_BW]      = { AdsppmHWNotSupported },
    [MMPM_RSC_ID_GENERIC_BW_EXT]  = { AdsppmHWNotSupported },
    [MMPM_RSC_ID_REG_PROG]        = { AdsppmHWNotSupported },
    [MMPM_RSC_ID_CORE_CLK_DOMAIN] = { AdsppmHWNotSupported },
    [MMPM_RSC_ID_SLEEP_LATENCY]   = { AdsppmHWNotSupported },
    [MMPM_RSC_ID_MEM_POWER]       = { AdsppmHWNotSupported },
    [MMPM_RSC_ID_VREG]            = { AdsppmHWNotSupported },
    [MMPM_RSC_ID_MEM_BW]          = { AdsppmHWNotSupported },
    [MMPM_RSC_ID_AXI_EN]          = { AdsppmHWNotSupported },
    [MMPM_RSC_ID_MIPS]            = { AdsppmHWNotSupported },
    [MMPM_RSC_ID_ACTIVE_STATE]    = { AdsppmHWNotSupported },
    [MMPM_RSC_ID_PMIC_GPIO]       = { AdsppmHWNotSupported },
    [MMPM_RSC_ID_RESET]           = { AdsppmHWNotSupported },
    [MMPM_RSC_ID_THERMAL]         = { AdsppmHWNotSupported },
};


#else
const AdsppmUTExtRscPropertiesType AdsppmUTExtRscProperties[MMPM_RSC_ID_MAX] =
{
    [MMPM_RSC_ID_REG_PROG]        = { AdsppmHWRegProgVerify },
    [MMPM_RSC_ID_CORE_CLK]        = { AdsppmHWCoreClkVerify },
    [MMPM_RSC_ID_CORE_CLK_DOMAIN] = { AdsppmHWCoreClkDomainVerify },
    [MMPM_RSC_ID_SLEEP_LATENCY]   = { AdsppmHWSleepLatencyVerify },
    [MMPM_RSC_ID_MEM_POWER]       = { AdsppmHWMemPowerVerify, },
    [MMPM_RSC_ID_MIPS_EXT]        = { AdsppmHwMipsVerify },
    [MMPM_RSC_ID_MPPS]            = { AdsppmHwMipsVerify },
    [MMPM_RSC_ID_GENERIC_BW]      = { AdsppmHWGenericBWVerify },
    [MMPM_RSC_ID_GENERIC_BW_EXT]  = { AdsppmHWGenericBWVerify },

    [MMPM_RSC_ID_VREG]            = { AdsppmHWNotSupported },
    [MMPM_RSC_ID_MEM_BW]          = { AdsppmHWNotSupported },
    [MMPM_RSC_ID_AXI_EN]          = { AdsppmHWNotSupported },
    [MMPM_RSC_ID_MIPS]            = { AdsppmHWNotSupported },
    [MMPM_RSC_ID_ACTIVE_STATE]    = { AdsppmHWNotSupported },
    [MMPM_RSC_ID_PMIC_GPIO]       = { AdsppmHWNotSupported },
    [MMPM_RSC_ID_RESET]           = { AdsppmHWNotSupported },
    [MMPM_RSC_ID_THERMAL]         = { AdsppmHWNotSupported },

};
#endif

#if (defined(ADSPPM_TEST_845) || defined(ADSPPM_TEST_670))
/* Internal Resources that ADSPPM controls */
const AdsppmUTRscPropertiesType AdsppmUTRscProperties[ADSPPM_RESRC_MAX] =
{
    [ADSPPM_RESRC_CORE_CLK] =
    {
        AdsppmUTHWRead_Clk,
        NULL,
        ADSPPM_RESRC_CLK_RGM_TYPE,
        ""
    },

    [ADSPPM_RESRC_LPASS_CLK] =
    {
        AdsppmUTHWRead_Clk,
        AdsppmTest_MIPS_Compute,
        ADSPPM_RESRC_CLK_RGM_TYPE,
        ""
    },

    /*
    [ADSPPM_RESRC_BW_BIMC] =
    {
        AdsppmUTHWRead_BW_BIMC,
        AdsppmTest_Bus_Compute,
        ADSPPM_RESRC_NPA_TYPE,
        "/clk/bimc",
    },

    [ADSPPM_RESRC_BW_SNOC] =
    {
        AdsppmUTHWRead_BW_SNOC,
        AdsppmTest_Bus_Compute,
        ADSPPM_RESRC_NPA_TYPE,
        "/clk/snoc",
    },
    */

    [ADSPPM_RESRC_AHBIX_CLK] =
    {
        AdsppmUTHWRead_AHBIX,
        AdsppmTest_AHBIX_Compute,
        ADSPPM_RESRC_CLK_RGM_TYPE,
        ""
    },

    [ADSPPM_RESRC_AHBE_CLK] =
    {
        AdsppmUTHWRead_AHBE,
        NULL,
        ADSPPM_RESRC_CLK_RGM_TYPE,
        ""
    },

    [ADSPPM_RESRC_SLEEP_LATENCY] =
    {
        AdsppmUTHWRead_Sleep_Latency,
        NULL,
        ADSPPM_RESRC_NPA_TYPE,
        "/core/cpu/latency/usec",
    },

};
#else
/* Internal Resources that ADSPPM controls */
const AdsppmUTRscPropertiesType AdsppmUTRscProperties[ADSPPM_RESRC_MAX] =
{
    [ADSPPM_RESRC_CORE_CLK] =
    {
        AdsppmUTHWRead_Clk,
        NULL,
        ADSPPM_RESRC_CLK_RGM_TYPE,
        ""
    },

    [ADSPPM_RESRC_LPASS_CLK] =
    {
        AdsppmUTHWRead_Clk,
        AdsppmTest_MIPS_Compute,
        ADSPPM_RESRC_CLK_RGM_TYPE,
        ""
    },

    [ADSPPM_RESRC_BW_BIMC] =
    {
        AdsppmUTHWRead_BW_BIMC,
        AdsppmTest_Bus_Compute,
        ADSPPM_RESRC_NPA_TYPE,
        "/clk/bimc",
    },

    [ADSPPM_RESRC_BW_SNOC] =
    {
        AdsppmUTHWRead_BW_SNOC,
        AdsppmTest_Bus_Compute,
        ADSPPM_RESRC_NPA_TYPE,
        "/clk/snoc",
    },

    [ADSPPM_RESRC_AHBIX_CLK] =
    {
        AdsppmUTHWRead_AHBIX,
        AdsppmTest_AHBIX_Compute,
        ADSPPM_RESRC_CLK_RGM_TYPE,
        ""
    },

    [ADSPPM_RESRC_AHBE_CLK] =
    {
        AdsppmUTHWRead_AHBE,
        NULL,
        ADSPPM_RESRC_CLK_RGM_TYPE,
        ""
    },

    [ADSPPM_RESRC_SLEEP_LATENCY] =
    {
        AdsppmUTHWRead_Sleep_Latency,
        NULL,
        ADSPPM_RESRC_NPA_TYPE,
        "/core/cpu/latency/usec",
    },

};
#endif


/* This contains the NPA Client Handle */
npa_query_handle AdsppmNPAClientHandle[ADSPPM_RESRC_MAX] = {0};


/************************ FUNCTION & API SECTION *************************
 ========================================================================*/

void AdsppmUTInit(void)
{
    byte resrcId;

    ADSPPMTEST_LOG_PRINTF_0(ADSPPMTEST_LOG_LEVEL_INFO,"ADSPPM_UT: INIT ");

    /* Initialize NPA Client handle */
    for(resrcId=0; resrcId < ADSPPM_RESRC_MAX; resrcId++ )
    {
        if(AdsppmUTRscProperties[resrcId].type & ADSPPM_RESRC_NPA_TYPE)
        {
            ADSPPMTEST_LOG_PRINTF_1(ADSPPMTEST_LOG_LEVEL_INFO,"ADSPPM_UT: Creating node to %s",
                                    AdsppmUTRscProperties[resrcId].npaNodeName);

            AdsppmNPAClientHandle[resrcId] =
                npa_create_query_handle( AdsppmUTRscProperties[resrcId].npaNodeName );

            if( AdsppmNPAClientHandle[resrcId] == NULL)
            {
                PRINT_ERROR_1("ADSPPM_UT: could not create client for node %x", resrcId);
            }
        }
    }
}


/* Section 1.  HARDWARE READ FUNCTIONS */

void AdsppmUTHWRead_Clk(uint32 * hwValue, void * hwParams)
{
    uint32 readClkFreq;
    MMPM_STATUS            sts;
    AdsppmUT_Clk_Params   *clkParams;
    MmpmCoreIdType         coreId;
    MmpmCoreInstanceIdType instanceId;
    MmpmClkIdLpassType     clkId;

    clkParams = (AdsppmUT_Clk_Params*)hwParams;
    coreId = clkParams->coreId;
    instanceId = clkParams->instanceId;
    clkId = clkParams->clkId;

    sts = Test_InfoClk(coreId, instanceId, clkId, &readClkFreq);
    if(MMPM_STATUS_SUCCESS != sts)
    {
        return;
    }
    else
    {
        *hwValue = readClkFreq;
        ADSPPMTEST_LOG_PRINTF_2(ADSPPMTEST_LOG_LEVEL_INFO,"ADSPPM_UT: Read Clk=%d, Freq=%d",
                                clkId, readClkFreq);
    }
}


void AdsppmUTHWRead_BW_BIMC(uint32 * hwValue, void * hwParams)
{
    if ((AdsppmUTRscProperties[ADSPPM_RESRC_BW_BIMC].type & ADSPPM_RESRC_NPA_TYPE) &&
        (AdsppmNPAClientHandle[ADSPPM_RESRC_BW_BIMC] != NULL))
    {
        /* read the value off NPA */
        npa_query_type result = {0};
        npa_query_status status;

        memset(&result, 0, sizeof(result));
        status = npa_query(AdsppmNPAClientHandle[ADSPPM_RESRC_BW_BIMC],
                           NPA_QUERY_CURRENT_STATE,
                           &result);

        if(status == NPA_QUERY_SUCCESS)
        {
           ADSPPMTEST_LOG_PRINTF_3(ADSPPMTEST_LOG_LEVEL_INFO,"ADSPPM_UT: Read BW BIMC, type=%u, state=%u, value=%u",
                                   result.type, result.data.state, result.data.value);
           *hwValue = result.data.state;
        }
        else
        {
            PRINT_ERROR_0("NPA query invalid for resrc ADSPPM_RESRC_BW_BIMC");
        }
    }
}


void AdsppmUTHWRead_BW_SNOC(uint32 * hwValue, void * hwParams)
{
    if ((AdsppmUTRscProperties[ADSPPM_RESRC_BW_SNOC].type & ADSPPM_RESRC_NPA_TYPE) &&
        (AdsppmNPAClientHandle[ADSPPM_RESRC_BW_SNOC] != NULL))
    {
        /* read the value off NPA */
        npa_query_type result = {0};
        npa_query_status status;

        /* Read SNOC votes */
        memset(&result, 0, sizeof(result));
        status = npa_query(AdsppmNPAClientHandle[ADSPPM_RESRC_BW_SNOC],
                           NPA_QUERY_CURRENT_STATE,
                           &result);

        if(status == NPA_QUERY_SUCCESS)
        {
           ADSPPMTEST_LOG_PRINTF_3(ADSPPMTEST_LOG_LEVEL_INFO,"ADSPPM_UT: Read BW SNOC, type=%u, state=%u, value=%u",
                                   result.type, result.data.state, result.data.value);
           *hwValue = result.data.state;
        }
        else
        {
            PRINT_ERROR_0("NPA query invalid for resrc ADSPPM_RESRC_BW_SNOC");
        }
    }
}


void AdsppmUTHWRead_AHBIX(uint32 * hwValue, void * hwParams)
{
    uint32 readClkFreq;
    AdsppmStatusType sts;
    sts = ClkRgm_GetClockFrequency(AdsppmClk_Ahb_Root, &readClkFreq);

    if(MMPM_STATUS_SUCCESS != sts)
    {
        return;
    }
    else
    {
        *hwValue = readClkFreq;
        //ADSPPMTEST_LOG_PRINTF_1(ADSPPMTEST_LOG_LEVEL_ERROR,"ADSPPM_UT: Read BW AHBIX=%d", readClkFreq);
    }
}

void AdsppmUTHWRead_AHBE(uint32 * hwValue, void * hwParams)
{
    uint32 readClkFreq;
    Adsppm_Status sts;
    sts = ClkRgm_GetClockFrequency(AdsppmClk_AhbE_Hclk, &readClkFreq);

    if(Adsppm_Status_Success != sts)
    {
        return;
    }
    else
    {
        *hwValue = readClkFreq;
        //ADSPPMTEST_LOG_PRINTF_1(ADSPPMTEST_LOG_LEVEL_INFO,"ADSPPM_UT: Read BW AHBE=%d", readClkFreq);
    }
}

void AdsppmUTHWRead_Sleep_Latency(uint32 * hwValue, void * hwParams)
{
    if ((AdsppmUTRscProperties[ADSPPM_RESRC_SLEEP_LATENCY].type & ADSPPM_RESRC_NPA_TYPE) &&
        (AdsppmNPAClientHandle[ADSPPM_RESRC_SLEEP_LATENCY] != NULL))
    {
        /* read the value off NPA */
        npa_query_type result = {0};
        npa_query_status status;

        memset(&result, 0, sizeof(result));
        status = npa_query(AdsppmNPAClientHandle[ADSPPM_RESRC_SLEEP_LATENCY],
                           NPA_QUERY_CURRENT_STATE,
                           &result);

        if(status == NPA_QUERY_SUCCESS)
        {
           ADSPPMTEST_LOG_PRINTF_3(ADSPPMTEST_LOG_LEVEL_INFO,"ADSPPM_UT: Read Sleep Latency, type=%u, state=%u, value=%u",
                                   result.type, result.data.state, result.data.value);
           *hwValue = result.data.value;
        }
        else
        {
            PRINT_ERROR_0("NPA query invalid for resrc ADSPPM_RESRC_SLEEP_LATENCY");
        }
    }
}

/* Section 2.  ADSPPM VERIFY FUNCTIONS */


void AdsppmHWNotSupported(uint32 clientId, MmpmRscExtParamType * params,
                          void *expectedParams, AdsppmStatusType * result)
{
    ADSPPMTEST_LOG_PRINTF_0(ADSPPMTEST_LOG_LEVEL_ERROR,"ADSPPM_UT: HW not supported");
    return;
}

void AdsppmHWRegProgVerify(uint32 clientId, MmpmRscExtParamType * params,
                           void *expectedParams, AdsppmStatusType * result)
{
    uint32 hwValue;

    //Read BW ICBARB
    AdsppmUTRscProperties[ADSPPM_RESRC_BW_BIMC].utHWReadFn(&hwValue, NULL);
}

void AdsppmHWCoreClkVerify(uint32 clientId, MmpmRscExtParamType * params,
                           void *expectedParams, AdsppmStatusType * result)
{
    uint32 hwValue;
    AdsppmUT_Clk_Params    clkParams;
    uint32                 freq;

    AdsppmTest_Get_CoreInstanceId(clientId,&(clkParams.instanceId),&(clkParams.coreId));

    if(ACM_GetFeatureStatus(AsicFeatureId_ComputeDSP) == AsicFeatureState_Enabled)
        clkParams.clkId = (MmpmClkIdLpassType)params->pReqArray->rscParam.pCoreClk->pClkArray[0].clkId.clkIdCompute;
    else if(ACM_GetFeatureStatus(AsicFeatureId_SlpiDSP) == AsicFeatureState_Enabled) /* (ACM_GetFeatureStatus(AsicFeatureId_LpassSlpiDSP) == AsicFeatureState_Enabled) */
        clkParams.clkId = (MmpmClkIdLpassType)params->pReqArray->rscParam.pCoreClk->pClkArray[0].clkId.clkIdSlpi;
    else
        clkParams.clkId = params->pReqArray->rscParam.pCoreClk->pClkArray[0].clkId.clkIdLpass;

    freq = params->pReqArray->rscParam.pCoreClk->pClkArray[0].clkFreqHz;

    AdsppmUTRscProperties[ADSPPM_RESRC_CORE_CLK].utHWReadFn(&hwValue, &clkParams);
    ADSPPMTEST_LOG_PRINTF_2(ADSPPMTEST_LOG_LEVEL_INFO,"ADSPPM_UT: Verify Core Clk In=%u, Out=%u", freq ,hwValue);

}

void AdsppmHWCoreClkDomainVerify(uint32 clientId, MmpmRscExtParamType * params,
                                 void *expectedParams, AdsppmStatusType * result)
{
    uint32 hwValue;
    AdsppmUT_Clk_Params    clkParams;
    //MmpmClkDomainSrcIdType clkDomainSrcId;
    MmpmClkDomainSrcIdLpassType clkDomainSrcIdLpass;

    AdsppmTest_Get_CoreInstanceId(clientId,&(clkParams.instanceId),&(clkParams.coreId));

    /* Get the Clock Domain Src for LPASS*/
    clkParams.clkId = params->pReqArray->rscParam.pCoreClkDomain->pClkDomainArray[0].clkId.clkIdLpass;

    clkDomainSrcIdLpass = params->pReqArray->rscParam.pCoreClkDomain->pClkDomainArray[0].clkDomainSrc.clkDomainSrcIdLpass;

    AdsppmUTRscProperties[ADSPPM_RESRC_LPASS_CLK].utHWReadFn(&hwValue, &clkParams);

}

void AdsppmHWSleepLatencyVerify(uint32 clientId, MmpmRscExtParamType * params,
                                void *expectedParams, AdsppmStatusType * result)
{
    uint32 hwValue;
    uint32 latency;

    //Compute latency (MIN Aggregation)
    latency = params->pReqArray->rscParam.sleepMicroSec;
    AdsppmUTRscProperties[ADSPPM_RESRC_SLEEP_LATENCY].utHWReadFn(&hwValue, NULL);

    ADSPPMTEST_LOG_PRINTF_2(ADSPPMTEST_LOG_LEVEL_INFO,"ADSPPM_UT: Verify Latency In=%d, Out=%d", latency ,hwValue);
}

void AdsppmHWMemPowerVerify(uint32 clientId, MmpmRscExtParamType * params,
                            void *expectedParams, AdsppmStatusType * result)
{

}

void AdsppmHwMipsVerify(uint32 clientId, MmpmRscExtParamType * params,
                        void *expectedParams, AdsppmStatusType * result)
{
    uint32 hwValue;
    AdsppmUT_Clk_Params clkParams;
    uint32 setClkFreq;
    uint32 newAhbeClockFreq;

    // Read Q6 clock frequency, which CoreID is LPASS_ADSP and InstanceID.
    clkParams.clkId = MMPM_CLK_ID_LPASS_ADSP_CORE;
    AdsppmTest_Get_CoreInstanceId(clientId,&(clkParams.instanceId),&(clkParams.coreId));

    /* Compute and read */
    AdsppmUTRscProperties[ADSPPM_RESRC_LPASS_CLK].utHWComputeFn(params, expectedParams,
                                                                NULL, &setClkFreq);
    AdsppmUTRscProperties[ADSPPM_RESRC_LPASS_CLK].utHWReadFn(&hwValue, &clkParams);

    ADSPPMTEST_LOG_PRINTF_2(ADSPPMTEST_LOG_LEVEL_INFO,"ADSPPM_UT: Verify LPASS expected=%u, hw=%u", setClkFreq ,hwValue);
    if(setClkFreq >= hwValue)
    {
        *result = Adsppm_Status_Failed;
    }


    //Read AHB-E bus
    if ((ACM_GetAhbFromAdspFreq((setClkFreq*1000), AsicBusType_AhbE, &newAhbeClockFreq) == Adsppm_Status_Success) &&
        (AdsppmUTRscProperties[ADSPPM_RESRC_AHBE_CLK].utHWReadFn != NULL))
    {
        uint32 ahbeHWValue;
        AdsppmUTRscProperties[ADSPPM_RESRC_AHBE_CLK].utHWReadFn(&ahbeHWValue, NULL);

        ADSPPMTEST_LOG_PRINTF_2(ADSPPMTEST_LOG_LEVEL_INFO,"ADSPPM_UT: Verify AHB-E-Clk=%u, ahbeHWValue=%u", newAhbeClockFreq ,ahbeHWValue);
        if(newAhbeClockFreq != ahbeHWValue)
        {
            *result = Adsppm_Status_Failed;
        }
    }

    //Read BW information
    //AdsppmUTRscProperties[ADSPPM_RESRC_BW_BIMC].utHWReadFn(&hwValue, NULL);
}


void AdsppmHWGenericBWVerify(uint32 clientId, MmpmRscExtParamType * params,
                             void *expectedParams, AdsppmStatusType * result)
{
    uint32 hwValue;
    boolean clk_bimc_impacted, clk_snoc_impacted, internal_ahb_impacted;

    MmpmBwPortIdType       masterPort;
    MmpmBwPortIdType       slavePort;
    const AdsppmClientType *client = CM_GetClient(clientId);
    BUSMgrBwTypeType       busType;
    AdsppmUT_Bus_Params    busInfo;

    /* Preset result */
    *result = Adsppm_Status_UnSupported;
    clk_bimc_impacted = FALSE;
    clk_snoc_impacted = FALSE;
    internal_ahb_impacted = FALSE;

    /* TO DO: This only check 1 BW request, which's not complete. It should loops and check all */
    busType = AdsppmTest_GetBusRouteType(params, client);
    masterPort = params->pReqArray->rscParam.pGenBwReq->pBandWidthArray->busRoute.masterPort;
    slavePort = params->pReqArray->rscParam.pGenBwReq->pBandWidthArray->busRoute.slavePort;
    AdsppmTest_Set_Bus_Impacted(busType,masterPort,slavePort,
                                &clk_snoc_impacted, &clk_bimc_impacted, &internal_ahb_impacted);

    //Verify on BIMC if BIMC is impacted
    if(clk_bimc_impacted)
    {
        uint32 expected;
        uint32 bimc_width = AdsppmTest_Set_Bus_Bandwidth(ADSPPM_RESRC_BW_BIMC, masterPort, slavePort);
        busInfo.busBW = bimc_width;
        busInfo.busID = ADSPPM_RESRC_BW_BIMC;

        AdsppmUTRscProperties[ADSPPM_RESRC_BW_BIMC].utHWReadFn(&hwValue, NULL);

        AdsppmUTRscProperties[ADSPPM_RESRC_BW_BIMC].utHWComputeFn(params, NULL, (void*)&busInfo, &expected);

        ADSPPMTEST_LOG_PRINTF_2(ADSPPMTEST_LOG_LEVEL_INFO,"ADSPPM_UT: Verify BIMC hw=%d, expected=%d", hwValue ,expected);
        if (expected != hwValue)
            *result = Adsppm_Status_Failed;

    }

    //Verify on SNOC if SNOC is impacted
    if(clk_snoc_impacted)
    {
        uint32 expected;
        uint32 snoc_width = AdsppmTest_Set_Bus_Bandwidth(ADSPPM_RESRC_BW_SNOC, masterPort, slavePort);
        busInfo.busBW = snoc_width;
        busInfo.busID = ADSPPM_RESRC_BW_SNOC;

        AdsppmUTRscProperties[ADSPPM_RESRC_BW_SNOC].utHWReadFn(&hwValue, NULL);

        AdsppmUTRscProperties[ADSPPM_RESRC_BW_SNOC].utHWComputeFn(params, NULL, (void*)&busInfo, &expected);

        ADSPPMTEST_LOG_PRINTF_4(ADSPPMTEST_LOG_LEVEL_INFO,"ADSPPM_UT: SNOC: bwBpS=%u, master=%u, slave=%u, width=%u",
                                (uint32)params->pReqArray->rscParam.pGenBwReq->pBandWidthArray->bwValue.busBwValue.bwBytePerSec,
                                masterPort, slavePort, snoc_width);
        ADSPPMTEST_LOG_PRINTF_2(ADSPPMTEST_LOG_LEVEL_INFO,"ADSPPM_UT: Verify SNOC: hw=%d, expected=%d",
                                hwValue ,expected);

        if(expected !=  hwValue)
        {
            *result = Adsppm_Status_Failed;
        }
    }

    //Verify on AHB if AHB is impacted
    if(internal_ahb_impacted)
    {
        uint32 expected = 0;

        //Read BW AHBIX
        AdsppmUTRscProperties[ADSPPM_RESRC_AHBIX_CLK].utHWReadFn(&hwValue, NULL);
        AdsppmUTRscProperties[ADSPPM_RESRC_AHBIX_CLK].utHWComputeFn(params, expectedParams,
                                                                    NULL, &expected);

        ADSPPMTEST_LOG_PRINTF_2(ADSPPMTEST_LOG_LEVEL_INFO,"ADSPPM_UT: Verify AHBIX expected=%u, hwread=%u",
                                expected, hwValue);

        if(expected !=  hwValue)
        {
            *result = Adsppm_Status_Failed;
        }

        //Read BW AHBE
        //AdsppmUTRscProperties[ADSPPM_RESRC_AHBE_CLK].utHWReadFn(&hwValue, NULL);
        //ADSPPMTEST_LOG_PRINTF_0(ADSPPMTEST_LOG_LEVEL_INFO,"ADSPPM_UT DEBUG: Verify AHBE");
    }
}

/* Section 3. Compute */

boolean AdsppmTest_Bus_Compute(MmpmRscExtParamType * reqParams, void *expectedParams,
                               void * infoParams, uint32 *result)
{
    uint32 bus_width;
    uint64 expectedVal;
    AdsppmUT_Bus_Params *busInfo;
    AdsppmBusBWDataIbAbType  bwAggData;
    uint64 bwVote = 0;

    if(reqParams == NULL || infoParams == NULL)
    {
        return FALSE;
    }

    busInfo = (AdsppmUT_Bus_Params*)infoParams;
    /* This hw value will return in KHz. To compare, we need to compare in Hz.
    Frequency (cycle/second) = Bandwitdth (bytes/seconds) / 8(bytes/cycle) */
    bus_width = busInfo->busBW;

    if(expectedParams != NULL)
    {
        uint64 *pExpectedVal;
        pExpectedVal = (uint64*)(expectedParams);
        bwVote = *pExpectedVal;
        expectedVal = bwVote / bus_width;
        expectedVal = (uint32)(expectedVal / 1000);
    }
    else
    {
        bwAggData.Ab = bwAggData.Ib = 0;
        bwAggData.latencyNs = 0;

        //bwVote = reqParams->pReqArray->rscParam.pGenBwReq->pBandWidthArray->bwValue.busBwValue.bwBytePerSec;
        AdsppmTest_ExtBW_Aggregation(&bwAggData, busInfo);
    }

    if(busInfo->busID == ADSPPM_RESRC_BW_SNOC)
    {
        bwVote = MAX (bwAggData.Ab, bwAggData.Ib);
        /* Convert from Mbytes/Second to Frequency */
        expectedVal = bwVote / bus_width;
        /* Convert to Khz */
        expectedVal = (uint32)(expectedVal / 1000);
        expectedVal = MAX((busInfo->snocFloor)*1000,expectedVal);
    }
    else if(busInfo->busID == ADSPPM_RESRC_BW_BIMC)
    {
        uint64   ab, ib;
        /* Formula for BIMC in 8996:
           ab = agg-ab * (153/100) * 1/2  */
        ab = ((bwAggData.Ab / 100)*153)/2;
        ib = bwAggData.Ib;
        bwVote = MAX(ab,ib);

        ADSPPMTEST_LOG_PRINTF_3(ADSPPMTEST_LOG_LEVEL_INFO, "BIMC Calculation: curr_ab=%u, curr_ib=%u, bw=%u ",
                                (uint32)ab, (uint32)ib, bus_width );

        /* Convert from Mbytes/Second to Frequency */
        expectedVal = bwVote / bus_width;
        /* Convert to Khz */
        expectedVal = (uint32)(expectedVal / 1000);
    }

    *result = expectedVal;

    return TRUE;
}


boolean AdsppmTest_AHBIX_Compute(MmpmRscExtParamType * reqParams, void *expectedParams,
                                 void * infoParams, uint32 *result)
{
    //AdsppmCoreCtxType             *pAdsppmCoreCtx = GetAdsppmCoreContext();
    //coreUtils_Q_Type              *pBWReqQ;
    //RMHandleType                  *pRMHandle;
    AdsppmBusBWDataIbAbType        AHBBwData;
    //uint32                         index, bwNum;

    /* Problem with using aggregated info is that we're not testing directly from the input
       Problem with using bwValue from the input is that
       we don't have the logic to do aggregation, & feature adjustment.
    */
    if(expectedParams == NULL)
    {
        AHBBwData.Ab = 0;
        AHBBwData.Ib = 0;
        AHBBwData.latencyNs = 0;
        /* Parse through all clients and do aggregation */
        AdsppmTest_IntBW_Aggregation(&AHBBwData);
        ACM_GetClockFromBW(result, &AHBBwData);
    }
    else
    {
        AdsppmUT_Verify_BW_Params * expected = (AdsppmUT_Verify_BW_Params*)expectedParams;
        AHBBwData.Ab = expected->Ab;
        AHBBwData.Ib = expected->Ib;
        AHBBwData.latencyNs = expected->latencyNs;
        ACM_GetClockFromBW(result, &AHBBwData);
    }
    if(result != NULL && (*result < ADSPPM_TEST_19_MHZ))
    {
        *result = ADSPPM_TEST_19_MHZ;
    }
    return TRUE;
}


boolean AdsppmTest_MIPS_Compute(MmpmRscExtParamType * reqParams, void *expectedParams,
                                 void * infoParams, uint32 *result)
{
    AdsppmCoreCtxType             *pAdsppmCoreCtx = GetAdsppmCoreContext();
    coreUtils_Q_Type              *pMipsReqQ;
    RMHandleType                  *pRMHandle;
    const AsicInfo_MipsInfoType   *pMipsInfo = NULL;
    uint32 index;
    uint32 mips = 0;
    uint32 mppsTotal = 0; uint32 qdspFloor = 0;
    uint32 mppsMax =0; uint32 mppsSecondMax =0;
    uint32 mipsPerThread = 0;
    uint32 setClkFreq = 0;
    AdsppmMIPSToClockAggregateType pMipsToClockData;

    if(expectedParams != NULL)
    {
        uint32 *pMips = (uint32*)(expectedParams);
        mips = *pMips;
    }
    else
    {
        //mips = reqParams->pReqArray->rscParam.pMipsExt->mipsTotal;
        pMipsReqQ =  &(pAdsppmCoreCtx->rscReqQ[Adsppm_Rsc_Id_Mips-1]);
        pRMHandle = (RMHandleType *) coreUtils_Q_Check(pMipsReqQ);
        // Go through all client request
        for(index = 0; (index < pMipsReqQ->nCnt) && (NULL != pRMHandle); index++)
        {
            if(RM_Valid == pRMHandle->validFlag)
            {
                if((NULL == (AdsppmQ6ClockRequestInfoType *) (pRMHandle->pRequestData)) ||
                    (NULL == pRMHandle->pClient))
                {
                    break;
                }
                else
                {
                    AdsppmQ6ClockRequestInfoType *pQ6ReqData = (AdsppmQ6ClockRequestInfoType *) (pRMHandle->pRequestData);
                    if(pQ6ReqData->usageType == Adsppm_Q6ClockRequestUsageType_Mips)
                    {
                        AdsppmMipsRequestType *pMipsReqData = &(pQ6ReqData->AdsppmQ6ClockRequestType.mipsRequestData);
                        if((pMipsReqData->mipsTotal > 0) || (pMipsReqData->mipsPerThread > 0))
                        {
                            //Aggregate mips total
                            mips += pMipsReqData->mipsTotal;
                            mipsPerThread += pMipsReqData->mipsPerThread;
                        }
                    }
                    else if(pQ6ReqData->usageType == Adsppm_Q6ClockRequestUsageType_Mpps)
                    {
                        AdsppmMppsRequestType *pMppsReqData = &(pQ6ReqData->AdsppmQ6ClockRequestType.mppsRequestData);
                        if((pMppsReqData->mppsTotal > 0) || (pMppsReqData->adspFloorClock > 0))
                        {
                            //find mpps
                            if(mppsMax == 0 && pMppsReqData->mppsTotal != 0)  //set Max
                            {
                                mppsMax = pMppsReqData->mppsTotal;
                            }
                            else if(pMppsReqData->mppsTotal > mppsMax && pMppsReqData->mppsTotal != 0) //Replace max
                            {
                                mppsSecondMax = mppsMax;
                                mppsMax = pMppsReqData->mppsTotal;
                            }
                            else if(pMppsReqData->mppsTotal > mppsSecondMax && pMppsReqData->mppsTotal != 0) //Replace 2nd
                            {
                                mppsSecondMax = pMppsReqData->mppsTotal;
                            }

                            //Find Max Floor
                            if(pMppsReqData->adspFloorClock != 0 && (qdspFloor == 0 || pMppsReqData->adspFloorClock > qdspFloor ))
                            {
                                qdspFloor = pMppsReqData->adspFloorClock;
                            }
                            //Aggregate mips total
                            mppsTotal += pMppsReqData->mppsTotal;


                        }
                    }
                }
            }
            pRMHandle = (RMHandleType *) coreUtils_Q_Next(pMipsReqQ, &pRMHandle->link);
        }

        // Calculate MIPS from MPPS (taking account threadloading and CPP factor),
        // then aggregate MPPS & MIPS itself into MIPS
        mips += ACM_GetMipsFromMpps( mppsTotal, mipsMgr_CalcNumDominantThreads(mppsTotal,mppsMax,mppsSecondMax));
        pMipsToClockData.mipsData.mipsTotal = mips;

        pMipsToClockData.mipsData.mipsPerThread = MAX(
            pMipsToClockData.mipsData.mipsPerThread,
            qdspFloor / ACM_GetHwThreadNumber());
    }

    /* If failed, do manual conversion */
    if(ACM_GetClockFromMips(&pMipsToClockData) != Adsppm_Status_Success)
    {
        /* Converts from total mips to clk frequency */
        pMipsInfo = AsicInfo_getMipsInfo();
        setClkFreq = (mips*1000000) / (256/pMipsInfo->mips2clockFactor);
    }
    else
    {
        setClkFreq = pMipsToClockData.qDSP6Clock;
    }

    /* Convert to Khz */
    *result = setClkFreq / 1000;

    return TRUE;
}


/* Section 4. APIs */

boolean Adsppm_Verify(uint32 clientId, MmpmRscExtParamType * params, void * expectedParams )
{
    AdsppmStatusType result = 0;
    MmpmRscIdType resrcId;

    /* Extract Resource ID */
    resrcId = params->pReqArray->rscId;

    if(resrcId > MMPM_RSC_ID_MAX)
    {
        return FALSE;
    }

    AdsppmUTExtRscProperties[resrcId].utHWVerifyFn(clientId, params, expectedParams, &result);

    /* Todo: make a compare function for each resource */
    if(result == Adsppm_Status_Success || result == Adsppm_Status_UnSupported)
    {
       return TRUE;
    }
    return FALSE;

}

/* Section C. Common functions below */

void AdsppmTest_Get_CoreInstanceId(uint32 clientId, MmpmCoreInstanceIdType *instanceId, MmpmCoreIdType *coreId)
{
    const AdsppmClientType *client = CM_GetClient(clientId);

    /* Read Clock from Clk Rgm */
    *coreId = (MmpmCoreIdType)(client->coreId);
    *instanceId = (MmpmCoreInstanceIdType)client->instanceId;

}

BUSMgrBwTypeType AdsppmTest_GetBusRouteType(MmpmRscExtParamType * params, const AdsppmClientType * client)
{
    AdsppmBusPortIdType      masterPort;
    AdsppmBusPortIdType      slavePort;
    const AsicBusPortDescriptorType *pMPortDescriptor;
    const AsicBusPortDescriptorType *pSPortDescriptor;
    BUSMgrBwTypeType         busMgrBwType = BUSMgr_Bw_None;

    /* Obtain master and slave ports */
    masterPort = AdsppmMapMMPMPortIdToADSPPMPortID(
                 params->pReqArray->rscParam.pGenBwReq->pBandWidthArray->busRoute.masterPort);
    slavePort = AdsppmMapMMPMPortIdToADSPPMPortID(
                params->pReqArray->rscParam.pGenBwReq->pBandWidthArray->busRoute.slavePort);

    /* Map to the Master Port per Instance. As some Master have multiple instances */
    masterPort = ACMBus_GetInstanceMasterBusPortId(client->coreId, client->instanceId,
                                                   masterPort);
    slavePort = ACMBus_GetInstanceMasterBusPortId(client->coreId, client->instanceId,
                                                  slavePort);
    /* Get the bus descriptor for this ports */
    pMPortDescriptor = ACMBus_GetPortDescriptor(masterPort);
    pSPortDescriptor = ACMBus_GetPortDescriptor(slavePort);

    if(pMPortDescriptor->portConnection != AsicBusPort_None &&
       pSPortDescriptor->portConnection != AsicBusPort_None)
    {
        busMgrBwType = busMgrGetBwType(
            pMPortDescriptor->portConnection,
            pSPortDescriptor->portConnection);
    }

    ADSPPMTEST_LOG_PRINTF_5(ADSPPMTEST_LOG_LEVEL_INFO,"ADSPPM_UT: mPort=%d, sPort=%d, mCon=0x%x, sCon=0x%x, busType =%d",
                            masterPort, slavePort,
                            pMPortDescriptor->portConnection,
                            pSPortDescriptor->portConnection, busMgrBwType);

    return busMgrBwType;

}

/* This function determine if SNOC / BIMC or AHB get impacted */
void AdsppmTest_Set_Bus_Impacted(BUSMgrBwTypeType busType, MmpmBwPortIdType masterPort, MmpmBwPortIdType slavePort,
                                 boolean *is_snoc, boolean *is_bimc, boolean *is_ahb)
{
    if(busType == BUSMgr_Bw_External_Only ||
       busType == BUSMgr_Bw_Internal_External )
    {
        if(slavePort == MMPM_BW_PORT_ID_DDR_SLAVE)
        {
            *is_bimc = TRUE;
            *is_snoc = TRUE;
        }
        if(slavePort == MMPM_BW_PORT_ID_PERIFNOC_SLAVE)
        {
            *is_snoc = TRUE;
        }
    }

    if(busType == BUSMgr_Bw_Internal_Only ||
       busType == BUSMgr_Bw_Internal_External)
    {
        *is_ahb = TRUE;
    }
}


uint32 AdsppmTest_Set_Bus_Bandwidth(AdsppmResrcID resrcId, MmpmBwPortIdType masterPort, MmpmBwPortIdType slavePort)
{
    uint32 bandwidth = ADSPPM_TEST_BIMC_REGULAR_WIDTH_LIMIT;

    /* Set SNOC Bandwidth - this might changes from target to targets */
    if (resrcId == ADSPPM_RESRC_BW_SNOC )
    {
        if(masterPort == MMPM_BW_PORT_ID_ADSP_MASTER &&
           slavePort == MMPM_BW_PORT_ID_DDR_SLAVE)
        {
            bandwidth = ADSPPM_TEST_ADSP_DDR_WIDTH_LIMIT;
        }
        else
        {
            bandwidth = ADSPPM_TEST_SNOC_REGULAR_WIDTH_LIMIT;
        }
    }
    /* Set BIMC bus width bytes/second */
    else if(resrcId == ADSPPM_RESRC_BW_BIMC )
    {
        bandwidth = ADSPPM_TEST_BIMC_REGULAR_WIDTH_LIMIT;
    }

    return bandwidth;
}

void AdsppmTest_ExtBW_Aggregation(AdsppmBusBWDataIbAbType *bwData, AdsppmUT_Bus_Params *busInfo)
{
    AdsppmCoreCtxType             *pAdsppmCoreCtx = GetAdsppmCoreContext();
    coreUtils_Q_Type              *pBWReqQ;
    RMHandleType                  *pRMHandle;
    uint32                         index, bwNum;
    AdsppmBusBWDataIbAbType        genericBwData, compensatedBwData;
    uint64                         maxBWVote = 0;
    uint32                         numBWAdspDdrVote = 0;
    boolean                        isCompensatedVote = FALSE;
    AsicCompensatedBWFudgeFactor   compensatedBWFudgeFactorValues;
    AsicBwConcurrencySettingsType* settings;


    /* Initialize */
    busInfo->snocFloor      = 0;
    genericBwData.Ab        = genericBwData.Ib = 0;
    genericBwData.latencyNs = 0;
    compensatedBwData.Ab    = compensatedBwData.Ib = 0;
    compensatedBwData.latencyNs = 0;

    /* Parse through all clients */
    pBWReqQ   =  &(pAdsppmCoreCtx->rscReqQ[Adsppm_Rsc_Id_BW-1]);
    pRMHandle = (RMHandleType *) coreUtils_Q_Check(pBWReqQ);

    // Go through all client request
    for(index = 0; (index < pBWReqQ->nCnt) && (NULL != pRMHandle); index++)
    {
        if(RM_Valid == pRMHandle->validFlag)
        {
            if((NULL == (AdsppmBwReqType *) (pRMHandle->pRequestData)) ||
                (NULL == pRMHandle->pClient))
            {
                break;
            }
            else
            {
                AdsppmBwReqType *pBwReqData = (AdsppmBwReqType *) (pRMHandle->pRequestData);
                // Go through each bw request for this client
                //ADSPPMTEST_LOG_PRINTF_1(ADSPPMTEST_LOG_LEVEL_INFO, "Parsing clients, bwNum=%u",pBwReqData->numOfBw);
                for(bwNum = 0; (bwNum < pBwReqData->numOfBw); bwNum++)
                {

                    AdsppmBusBWRequestValueType *pBwRequest = &pBwReqData->pBwArray[bwNum];

                    if(pBwReqData->requestClass == Adsppm_BwReqClass_Compensated )
                    {
                        if(pBwRequest->busRoute.masterPort == AdsppmBusPort_Adsp_Master &&
                           pBwRequest->busRoute.slavePort == AdsppmBusPort_Ddr_Slave)
                        {
                            isCompensatedVote = TRUE;
                            numBWAdspDdrVote++;
                            maxBWVote = MAX(maxBWVote, bwData->Ib);
                        }
                        ACM_BusBWAggregate(&compensatedBwData, &(pBwRequest->bwValue));
                        ADSPPMTEST_LOG_PRINTF_4(ADSPPMTEST_LOG_LEVEL_INFO, "Aggregate Compensated  BW: curr_ab=%u, curr_ib=%llu, num_bw=%u ",
                                             (uint32)compensatedBwData.Ab,
                                             (uint32)compensatedBwData.Ib, (uint32)(compensatedBwData.Ib>>32), bwNum );
                    }
                    else
                    {
                        ACM_BusBWAggregate(&genericBwData, &(pBwRequest->bwValue));
                        ADSPPMTEST_LOG_PRINTF_4(ADSPPMTEST_LOG_LEVEL_INFO, "Aggregate Generic  BW: curr_ab=%u, curr_ib=%llu, num_bw=%u ",
                                             (uint32)genericBwData.Ab,
                                             (uint32)genericBwData.Ib, (uint32)(genericBwData.Ib>>32), bwNum );
                    }
                }
            }
        }
        pRMHandle = (RMHandleType *) coreUtils_Q_Next(pBWReqQ, &pRMHandle->link);
    }

    // Apply BW concurrency factor for compensated vote.
    // NOTE:  Ab & Ib might not be reflect the right Ab & Ib for a particular routes...
    // They're total Ab & Ib, so might not work when a client request for multiple routes.
    // If user specify very complex scenarios, it's better to have user explicit use the expected parameters
    if(isCompensatedVote)
    {
        AsicCompensatedDdrBwTableEntryType  tableEntry;
        uint32 cacheSize = 0;

        // Get BW concurrency settings table
  settings = ACM_GetAdspDdrBwConcurrencySettings();
        BusMgr_ApplyAdspDdrBwConcurrencyFactor( numBWAdspDdrVote, maxBWVote,
                                             &(compensatedBwData.Ab), &(compensatedBwData.Ib),
                                             &compensatedBWFudgeFactorValues,settings);

        /* Apply scaling factor */
        if (Adsppm_Status_Success == CPM_GetCachePartitionSize(&cacheSize))
        {
            uint32 scalingFactor = 100;
            ACM_GetBWScalingFactorFromCacheSize(cacheSize, &scalingFactor);
            compensatedBwData.Ab = ((compensatedBwData.Ab * (uint64) scalingFactor)/100ull);
            compensatedBwData.Ib = ((compensatedBwData.Ib  * (uint64) scalingFactor)/100ull);
        }

        /* Readjust for compensated vote for DDR */
        ACM_GetCompensatedDdrBwTableEntry( MAX(compensatedBwData.Ab, compensatedBwData.Ib), &tableEntry);
        if(!tableEntry.honestFlag && (tableEntry.bwAbVote != -1))
        {
            compensatedBwData.Ab *= 2;
            compensatedBwData.Ib  = tableEntry.bwAbVote;
        }
        
        //if(gAdsppmTestContex.testTarget == ADSPPM_845)
        {
           if(!tableEntry.honestFlag && (tableEntry.bwAbVote != -1))
              compensatedBwData.Ab = tableEntry.bwAbVote;

           compensatedBwData.Ib  = tableEntry.bwIbVote;
        }
        if(busInfo->busID == ADSPPM_RESRC_BW_SNOC &&
           tableEntry.snocFloorVoteMhz != 0)
        {
            busInfo->snocFloor = tableEntry.snocFloorVoteMhz;
        }
    }

    /* Now aggregated between Generic & Compensated */
    bwData->Ab = compensatedBwData.Ab + genericBwData.Ab;
    bwData->Ib = MAX(compensatedBwData.Ib, genericBwData.Ib);
    bwData->latencyNs = MIN(compensatedBwData.latencyNs, genericBwData.latencyNs);

    ADSPPMTEST_LOG_PRINTF_5(ADSPPMTEST_LOG_LEVEL_INFO, "Aggregate Final Gen-BW: curr_ab=%llu, curr_ib=%llu, lat=%u ",
                           (uint32)genericBwData.Ab, (uint32)(genericBwData.Ab>>32),
                           (uint32)genericBwData.Ib, (uint32)(genericBwData.Ib>>32), genericBwData.latencyNs);

    ADSPPMTEST_LOG_PRINTF_5(ADSPPMTEST_LOG_LEVEL_INFO, "Aggregate Final Com-BW: curr_ab=%llu, curr_ib=%llu, lat=%u ",
                           (uint32)compensatedBwData.Ab, (uint32)(compensatedBwData.Ab>>32),
                           (uint32)compensatedBwData.Ib, (uint32)(compensatedBwData.Ib>>32), compensatedBwData.latencyNs);

    ADSPPMTEST_LOG_PRINTF_5(ADSPPMTEST_LOG_LEVEL_INFO, "Aggregate Final BW: curr_ab=%llu, curr_ib=%llu, lat=%u ",
                           (uint32)bwData->Ab, (uint32)(bwData->Ab>>32),
                           (uint32)bwData->Ib, (uint32)(bwData->Ib>>32), bwData->latencyNs);
}



void AdsppmTest_IntBW_Aggregation(AdsppmBusBWDataIbAbType *bwData)
{
    AdsppmCoreCtxType             *pAdsppmCoreCtx = GetAdsppmCoreContext();
    coreUtils_Q_Type              *pBWReqQ;
    RMHandleType                  *pRMHandle;
    uint32                         index, bwNum;
    AdsppmBusBWDataIbAbType        genericBwData, adspCompBwData, nonAdspCompBwData;
    //uint64                         maxBWVote = 0;
    //uint32                         numBWAdspDdrVote = 0;
    boolean                        isCompensatedVote = FALSE;


    /* Initialize */
    genericBwData.Ab            = genericBwData.Ib = 0;
    genericBwData.latencyNs     = 0;
    adspCompBwData.Ab           = adspCompBwData.Ib = 0;
    adspCompBwData.latencyNs    = 0;
    nonAdspCompBwData.Ab        = nonAdspCompBwData.Ib = 0;
    nonAdspCompBwData.latencyNs = 0;

    /* Parse through all clients */
    pBWReqQ   =  &(pAdsppmCoreCtx->rscReqQ[Adsppm_Rsc_Id_BW-1]);
    pRMHandle = (RMHandleType *) coreUtils_Q_Check(pBWReqQ);

    // Go through all client request
    for(index = 0; (index < pBWReqQ->nCnt) && (NULL != pRMHandle); index++)
    {
        if(RM_Valid == pRMHandle->validFlag)
        {
            if((NULL == (AdsppmBwReqType *) (pRMHandle->pRequestData)) ||
                (NULL == pRMHandle->pClient))
            {
                break;
            }
            else
            {
                AdsppmBwReqType *pBwReqData = (AdsppmBwReqType *) (pRMHandle->pRequestData);
                // Go through each bw request for this client
                //ADSPPMTEST_LOG_PRINTF_1(ADSPPMTEST_LOG_LEVEL_INFO, "Parsing clients, bwNum=%u",pBwReqData->numOfBw);
                for(bwNum = 0; (bwNum < pBwReqData->numOfBw); bwNum++)
                {
                    AdsppmBusBWRequestValueType *pBwRequest = &pBwReqData->pBwArray[bwNum];

                    if(pBwReqData->requestClass == Adsppm_BwReqClass_Compensated )
                    {
                        if(pBwRequest->busRoute.masterPort == AdsppmBusPort_Adsp_Master)
                        {
                            isCompensatedVote = TRUE;
                            ACM_BusBWAggregate(&adspCompBwData, &(pBwRequest->bwValue));
                            //numBWAdspDdrVote++;
                            //maxBWVote = MAX(maxBWVote, bwData->Ib);
                            ADSPPMTEST_LOG_PRINTF_4(ADSPPMTEST_LOG_LEVEL_INFO, "Aggregate Compensated Int-BW: curr_ab=%u, curr_ib=%llu, num_bw=%u ",
                                                 (uint32)adspCompBwData.Ab,
                                                 (uint32)adspCompBwData.Ib, (uint32)(adspCompBwData.Ib>>32), bwNum );

                        }
                        else
                        {
                            ACM_BusBWAggregate(&nonAdspCompBwData, &(pBwRequest->bwValue));
                            ADSPPMTEST_LOG_PRINTF_4(ADSPPMTEST_LOG_LEVEL_INFO, "Aggregate Compensated Int-BW: curr_ab=%u, curr_ib=%llu, num_bw=%u ",
                                                 (uint32)nonAdspCompBwData.Ab,
                                                 (uint32)nonAdspCompBwData.Ib, (uint32)(nonAdspCompBwData.Ib>>32), bwNum );

                        }
                    }
                    else
                    {
                        ACM_BusBWAggregate(&genericBwData, &(pBwRequest->bwValue));
                        ADSPPMTEST_LOG_PRINTF_4(ADSPPMTEST_LOG_LEVEL_INFO, "Aggregate Generic  Int-BW: curr_ab=%u, curr_ib=%llu, num_bw=%u ",
                                             (uint32)genericBwData.Ab,
                                             (uint32)genericBwData.Ib, (uint32)(genericBwData.Ib>>32), bwNum );
                    }
                }
            }
        }
        pRMHandle = (RMHandleType *) coreUtils_Q_Next(pBWReqQ, &pRMHandle->link);
    }

    // Apply compensated votes
    if(isCompensatedVote)
    {
        ACM_ApplyAhbBwCompensation(adspCompBwData.Ab, AsicBusType_AhbIx,    &(bwData->Ab));
        bwData->Ab += nonAdspCompBwData.Ab;
        ACM_ApplyAhbBwCompensation(adspCompBwData.Ib, AsicBusType_AhbIx,    &(bwData->Ib));
        bwData->Ib = MAX(bwData->Ib, nonAdspCompBwData.Ab);
    }

    /* Now aggregated between Generic & Compensated */
    bwData->Ab       += genericBwData.Ab;
    bwData->Ib        = MAX(bwData->Ib, genericBwData.Ib);
    bwData->latencyNs = MIN(bwData->latencyNs, genericBwData.latencyNs);

    //Aggregated applied adjustment
    bwData->Ib = ACM_AdjustParamValue(AsicFeatureId_Ahb_Scaling, bwData->Ib);


    ADSPPMTEST_LOG_PRINTF_5(ADSPPMTEST_LOG_LEVEL_INFO, "Aggregate Final Int-BW: curr_ab=%llu, curr_ib=%llu, lat=%u ",
                           (uint32)bwData->Ab, (uint32)(bwData->Ab>>32),
                           (uint32)bwData->Ib, (uint32)(bwData->Ib>>32), bwData->latencyNs);
}

