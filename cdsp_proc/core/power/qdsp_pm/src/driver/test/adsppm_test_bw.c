/*
* Copyright (c) 2013-2015 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

#include <limits.h>
#include "mmpm.h"
#include "adsppm_test.h"
#include "adsppm_test_verify.h"
#include "adsppm_test_param.h"
#include "adsppm_test_utils.h"
#include "adsppm_test_asic_info.h"


// Aggregate BW votes with no BW compensation table applied
static void calculateBwNoCompensation( BwReqTestType_Ext *bwTable, uint32 numBw, uint64 *ab, uint64 *ib)
{
    int i;
    BwReqTestType_Ext bw;
    uint32 aggr_ib = 0;
    uint32 aggr_ab = 0;

    // Aggregate compensated and uncompensated votes without separating them
    for ( i = 0; i < numBw ; i++)
    {
        bw = bwTable[i];
        aggr_ib = (bw.bwVal > aggr_ib ? bw.bwVal : aggr_ib);
        aggr_ab += bw.bwVal;
    }

    *ab = aggr_ab;
    *ib = aggr_ib;
}

static void calculateDdrBwFromTable( BwReqTestType_Ext *bwTable, uint32 numBw, uint64 *ab, uint64 *ib)
{
    int i,numClients = 0,honestAbFlag =0,honestIbFlag=0;
    uint64 maxClientBwVote=0,total_ab=0,total_ib=0,total_ab_generic=0,
    total_ib_generic=0,total_ab_ext=0,total_ib_ext=0,lookupAb=0,lookupIb=0;
    const AsicInfo_CompensatedDdrBwTableType *ddrBwTableptr;
    const AsicInfo_BwConcurrencySettingsTableType *bwConcurrencyTable;
    BwReqTestType_Ext bw;

    // Aggregate compensated and uncompensated votes separately
    for ( i = 0; i < numBw ; i++)
    {
        bw = bwTable[i];

        if ( bw.type == Bw_Generic )
        {
            total_ib_generic =  ( bw.bwVal > total_ib_generic ? bw.bwVal : total_ib_generic);
            total_ab_generic += bw.bwVal;
        }
        else
        {
            total_ib_ext = (bw.bwVal > total_ib_ext ? bw.bwVal : total_ib_ext);
            total_ab_ext += bw.bwVal;
            maxClientBwVote = (maxClientBwVote > bw.bwVal ? maxClientBwVote : bw.bwVal);
            numClients++;
        }
    }

    /* Get BW Concurency Settings Table */
    bwConcurrencyTable = AsicInfo_getBwConcurrencyInfo();

    if(numClients > bwConcurrencyTable->table->threshold)
    {
        // Concurrent Clients > Threshold, maxbw/totalBw > 90% fudgefactor = 1.5
        if (((maxClientBwVote * 100)/ total_ab_ext) >= 90 )
        {
            total_ab_ext = (total_ab_ext * bwConcurrencyTable->table->concurrencyFactorArray[1])/ 256;
            total_ib_ext = (total_ib_ext * bwConcurrencyTable->table->concurrencyFactorArray[1])/ 256;
        }
        // Concurrent Clients > Threshold, maxbw/totalBw < 90% fudgefactor = 2
        else
        {
            total_ab_ext = (total_ab_ext * bwConcurrencyTable->table->concurrencyFactorArray[2])/ 256;
            total_ib_ext = (total_ib_ext * bwConcurrencyTable->table->concurrencyFactorArray[2])/ 256;
        }
    }
    /* else Do nothing */


    /* Get DDr BW table from Asic Info */
    ddrBwTableptr = AsicInfo_getCompensatedDdrBwInfo();

    if (NULL != ddrBwTableptr)
    {
        // Update Ab from ddrBw table
        for (i=0; i < ddrBwTableptr->num ; i++)
        {
            if (total_ab_ext <= ddrBwTableptr->table[i].bwThreshold)
            {
                lookupAb = ddrBwTableptr->table[i].bwVote;
                honestAbFlag = ddrBwTableptr->table[i].honestFlag;
                break;
            }
                }
        // Update Ib from ddrBw table
        for (i=0; i < ddrBwTableptr->num ; i++)
        {
            if (total_ib_ext <= ddrBwTableptr->table[i].bwThreshold)
            {
                lookupIb = ddrBwTableptr->table[i].bwVote;
                honestIbFlag = ddrBwTableptr->table[i].honestFlag;
                break;
            }
        }
    }

    /* Check if both dishonest vote */
    if((honestAbFlag == 0) && (honestIbFlag == 0))
    {
        total_ab_ext = lookupAb;
        total_ib_ext = lookupIb;
    }
    else
    {
        // Honest votes are used
    }

    // Final aggregration of compensated and uncompensated
    total_ab = total_ab_ext +  total_ab_generic;
    total_ib = total_ib_ext > total_ib_generic ? total_ib_ext : total_ib_generic;

    *ab = total_ab;
    *ib = total_ib;
}

static void calculateAhbBwFromTable( BwReqTestType_Ext *bwTable, uint32 numBw, uint64 *pAbResult, uint64 *pIbResult)
{

    int i;
    uint64 total_ab = 0, total_ib = 0, total_ab_generic = 0,
        total_ib_generic = 0, total_ab_ext = 0, total_ib_ext = 0,
        lookupAb = 0, lookupIb = 0;
    const AsicInfo_CompensatedAhbBwTableType *ahbBwTableptr;
    BwReqTestType_Ext bw;

    // Aggregate compensated and uncompensated votes separately
    for(i = 0; i < numBw; i++)
    {
        bw = bwTable[i];
        if ( bw.type == Bw_Generic )
        {
            total_ib_generic =  ( bw.bwVal > total_ib_generic ? bw.bwVal : total_ib_generic);
            total_ab_generic += bw.bwVal;
        }
        else
        {
            total_ib_ext = (bw.bwVal > total_ib_ext ? bw.bwVal : total_ib_ext);
            total_ab_ext += bw.bwVal;
        }
    }

    // Get AHB BW table from Asic Info
    ahbBwTableptr = AsicInfo_getCompensatedAhbBwInfo();

    if(NULL != ahbBwTableptr)
    {
        // Update Ab from table
        for(i = 0; i < ahbBwTableptr->num; i++)
        {
            if(total_ab_ext <= ahbBwTableptr->table[i].bwThreshold)
            {
                lookupAb = ahbBwTableptr->table[i].bwVote;
                break;
            }
        }
        // Update Ib from ddrBw table
        for(i=0; i < ahbBwTableptr->num; i++)
        {
            if(total_ib_ext <= ahbBwTableptr->table[i].bwThreshold)
            {
                lookupIb = ahbBwTableptr->table[i].bwVote;
                break;
            }
        }
    }

    // Honest votes are kept if lookup returns -1;
    // otherwise use lookup result
    if(lookupAb != -1)
        total_ab_ext = lookupAb;
    if(lookupIb != -1)
        total_ib_ext = lookupIb;

    // Final aggregration of compensated and uncompensated
    total_ab = total_ab_ext +  total_ab_generic;
    total_ib = total_ib_ext > total_ib_generic ? total_ib_ext : total_ib_generic;

    if(total_ib >=  MAX_AHB_BW)
      total_ib =  MAX_AHB_BW;

    *pAbResult = total_ab;
    *pIbResult = total_ib;

}

static uint32 getAhbFreqFromBw(uint64 bw)
{
    const AsicInfo_ClockFreqVoltPlanType *pFreqPlan = NULL;
    uint32 i;
    uint32 ahbFreq = bw/4;
    pFreqPlan = AsicInfo_getClockFreqVoltPlan(MMPM_CLK_ID_LPASS_AHB_ROOT);
    if(ahbFreq >= pFreqPlan->pFreqVoltPlan[pFreqPlan->numFreq - 1].frequency)
    {
        ahbFreq = pFreqPlan->pFreqVoltPlan[pFreqPlan->numFreq - 1].frequency;
    }
    else
    {
        for(i = 0; i < pFreqPlan->numFreq; i++)
        {
            if(ahbFreq <= pFreqPlan->pFreqVoltPlan[i].frequency)
            {
                ahbFreq = pFreqPlan->pFreqVoltPlan[i].frequency;
                break;
            }
        }
    }
    return ahbFreq;
}


#define BANDWIDTH_01_NAME    "bandwidth_01"
#define BANDWIDTH_01_DETAILS "Req-Rel, all core/instance/master/slave/usage combination"

static MMPM_STATUS TestAllCoresBwRequest(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    MmpmCoreIdType coreId;
    MmpmCoreInstanceIdType instanceId;
    uint32 clientId = 0;
    MmpmBwPortIdType master, slave;
    MmpmBwUsageLpassType usage;
    BwReqTestType bwReq;
    uint32 validRoute;

    for(coreId = MMPM_CORE_ID_LPASS_ADSP; coreId < MMPM_CORE_ID_LPASS_END; coreId++)
    {
        for(instanceId = MMPM_CORE_INSTANCE_0; instanceId < MMPM_CORE_INSTANCE_MAX; instanceId++)
        {
            if(AsicInfo_isCoreInstanceAvailable(coreId, instanceId))
            {
                clientId = Test_Register(coreId, instanceId, BANDWIDTH_01_NAME);
                ADSPPMTEST_LOG_PRINTF_3(ADSPPMTEST_LOG_LEVEL_INFO,"ADSPPM_UT: BW TEST: core=%d, instance=%d, client=%d",
                                        coreId, instanceId, clientId);

                if(clientId)
                {
                    for(master = MMPM_BW_PORT_ID_ADSP_MASTER; master < MMPM_BW_PORT_ID_VAPSS_HCP_MASTER; master++)
                    {
                        if(AsicInfo_isBusPortAvailable(master))
                        {
                            for(slave = MMPM_BW_PORT_ID_DML_SLAVE; slave < MMPM_BW_PORT_ID_CDSP_TCM_SLAVE; slave++)
                            {
                                if(AsicInfo_isBusPortAvailable(slave))
                                {
                                    validRoute = 1;

                                    if((MMPM_BW_PORT_ID_PERIFNOC_SLAVE == slave) && (MMPM_BW_PORT_ID_ADSP_MASTER != master))
                                    {
                                        validRoute = 0;
                                    }
                                    if((MMPM_BW_PORT_ID_OCMEM_SLAVE == slave) || (MMPM_BW_PORT_ID_EXT_AHB_SLAVE == slave))
                                    {
                                        validRoute = 0;
                                    }
                                    if((MMPM_BW_PORT_ID_EXT_AHB_MASTER == master) && (MMPM_BW_PORT_ID_DDR_SLAVE != slave))
                                    {
                                        validRoute = 0;
                                    }

                                    if(validRoute)
                                    {
                                        for(usage = MMPM_BW_USAGE_LPASS_DSP; usage < MMPM_BW_USAGE_LPASS_ENUM_MAX; usage++)
                                        {
                                            bwReq.masterPort = master;
                                            bwReq.slavePort = slave;
                                            bwReq.bwVal = 100000000;
                                            bwReq.usagePercent = 100;
                                            bwReq.usageType = usage;

                                            sts = Test_RequestBandwidth(clientId, &bwReq, 1, NULL);
                                            if(MMPM_STATUS_SUCCESS != sts)
                                            {
                                                retSts = MMPM_STATUS_FAILED;
												ADSPPMTEST_LOG_PRINTF_3(ADSPPMTEST_LOG_LEVEL_ERROR,
												"RequestBW failed %u M = 0x%x S = 0x%x",
												sts, master, slave);
                                            }
                                            else
                                            {
                                                sts = Test_ReleaseResource(clientId, MMPM_RSC_ID_GENERIC_BW);
                                                if(MMPM_STATUS_SUCCESS != sts)
                                                {
                                                    retSts = MMPM_STATUS_FAILED;
													ADSPPMTEST_LOG_PRINTF_3(ADSPPMTEST_LOG_LEVEL_ERROR,
													"ReleaseRsc Failed %u M = %u S = %u",
													sts, master, slave);
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    sts = Test_Deregister(clientId);
                    if(MMPM_STATUS_SUCCESS != sts)
                    {
                        retSts = MMPM_STATUS_FAILED;
						ADSPPMTEST_LOG_PRINTF_3(ADSPPMTEST_LOG_LEVEL_ERROR,
						"Deregister failed %u M = %u S = %u",
						sts, master, slave);
                    }
                }
                else
                {
                    retSts = MMPM_STATUS_FAILED;
					ADSPPMTEST_LOG_PRINTF_3(ADSPPMTEST_LOG_LEVEL_ERROR,
					"ClientId failed %u M = %u S = %u",
					sts, master, slave);
                }
            }
        }
    }

    return retSts;
}

/**
 * @ingroup Bandwidth
 * @test bandwidth_01
 * @brief Req-Rel, all core/instance/master/slave/usage combination
 */
static MMPM_STATUS Test_Bandwidth_01(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(BANDWIDTH_01_NAME, BANDWIDTH_01_DETAILS);

    sts = TestAllCoresBwRequest();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_Bandwidth, BANDWIDTH_01_NAME, sts);
    return sts;
}


#define BANDWIDTH_02_NAME    "bandwidth_02"
#define BANDWIDTH_02_DETAILS "Req-Rel DDR BW, all core/instance/master combination"

static MMPM_STATUS TestBandwidthReqRelDdrBw(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    MmpmCoreIdType coreId;
    MmpmCoreInstanceIdType instanceId;
    uint32 clientId = 0;
    BwReqTestType bw;
    MmpmBwPortIdType master;
    MmpmBwUsageLpassType usage;

    for(coreId = MMPM_CORE_ID_LPASS_ADSP; coreId < MMPM_CORE_ID_LPASS_END; coreId++)
    {
        for(instanceId = MMPM_CORE_INSTANCE_0; instanceId < MMPM_CORE_INSTANCE_MAX; instanceId++)
        {
            if(AsicInfo_isCoreInstanceAvailable(coreId, instanceId))
            {
                clientId = Test_Register(coreId, instanceId, BANDWIDTH_02_NAME);
                if(clientId)
                {
                    bw.slavePort = MMPM_BW_PORT_ID_DDR_SLAVE;
                    bw.bwVal = 76000000*8;
                    bw.usagePercent = 100;

                    for(master = MMPM_BW_PORT_ID_ADSP_MASTER; master < MMPM_BW_PORT_ID_VAPSS_HCP_MASTER; master++)
                    {
                        if(AsicInfo_isBusPortAvailable(master))
                        {
                            for(usage = MMPM_BW_USAGE_LPASS_DSP; usage < MMPM_BW_USAGE_LPASS_ENUM_MAX; usage++)
                            {
                                bw.masterPort = master;
                                bw.usageType = usage;

                                sts = Test_RequestBandwidth(clientId, &bw, 1, NULL);
                                if(MMPM_STATUS_SUCCESS != sts)
                                {
                                    retSts = MMPM_STATUS_FAILED;
                                }

                                sts = Test_ReleaseResource(clientId, MMPM_RSC_ID_GENERIC_BW);
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
 * @ingroup Bandwidth
 * @test bandwidth_02
 * @brief Req-Rel DDR BW, all core/instance/master combination
 */
static MMPM_STATUS Test_Bandwidth_02(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(BANDWIDTH_02_NAME, BANDWIDTH_02_DETAILS);

    sts = TestBandwidthReqRelDdrBw();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_Bandwidth, BANDWIDTH_02_NAME, sts);
    return sts;
}


#define BANDWIDTH_03_NAME    "bandwidth_03"
#define BANDWIDTH_03_DETAILS "Req-Rel PNOC BW, all core/instance/master combination"

static MMPM_STATUS TestBandwidthReqRelPnocBw(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    MmpmCoreIdType coreId;
    MmpmCoreInstanceIdType instanceId;
    uint32 clientId = 0;
    BwReqTestType bw;
    MmpmBwUsageLpassType usage;

    for(coreId = MMPM_CORE_ID_LPASS_ADSP; coreId < MMPM_CORE_ID_LPASS_END; coreId++)
    {
        for(instanceId = MMPM_CORE_INSTANCE_0; instanceId < MMPM_CORE_INSTANCE_MAX; instanceId++)
        {
            if(AsicInfo_isCoreInstanceAvailable(coreId, instanceId))
            {
                clientId = Test_Register(coreId, instanceId, BANDWIDTH_03_NAME);
                if(clientId)
                {
                    bw.masterPort = MMPM_BW_PORT_ID_ADSP_MASTER;
                    bw.slavePort = MMPM_BW_PORT_ID_PERIFNOC_SLAVE;
                    bw.bwVal = 76000000*8;
                    bw.usagePercent = 100;

                    for(usage = MMPM_BW_USAGE_LPASS_DSP; usage < MMPM_BW_USAGE_LPASS_ENUM_MAX; usage++)
                    {
                        bw.usageType = usage;

                        sts = Test_RequestBandwidth(clientId, &bw, 1, NULL);
                        if(MMPM_STATUS_SUCCESS != sts)
                        {
                            retSts = MMPM_STATUS_FAILED;
                        }

                        sts = Test_ReleaseResource(clientId, MMPM_RSC_ID_GENERIC_BW);
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
 * @ingroup Bandwidth
 * @test bandwidth_03
 * @brief Req-Rel PNOC BW, all core/instance/master combination
 */
static MMPM_STATUS Test_Bandwidth_03(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(BANDWIDTH_03_NAME, BANDWIDTH_03_DETAILS);

    sts = TestBandwidthReqRelPnocBw();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_Bandwidth, BANDWIDTH_03_NAME, sts);
    return sts;
}


#define BANDWIDTH_04_NAME    "bandwidth_04"
#define BANDWIDTH_04_DETAILS "Req-Rel, 2 same coreID/instanceID clients"

static AdsppmTestType testbandwidth04[] =
{
    {1, 2, MMPM_CORE_ID_LPASS_DML, MMPM_CORE_INSTANCE_0,
        {REG, REQBW}
    },
    {2, 2, MMPM_CORE_ID_LPASS_DML, MMPM_CORE_INSTANCE_0,
        {REG, REQBW}
    },
    {1, 2, MMPM_CORE_ID_LPASS_DML, MMPM_CORE_INSTANCE_0,
        {RELBW, DREG}
    },
    {2, 2, MMPM_CORE_ID_LPASS_DML, MMPM_CORE_INSTANCE_0,
        {RELBW, DREG}
    }
};

/**
 * @ingroup Bandwidth
 * @test bandwidth_04
 * @brief Req-Rel, 2 same coreID/instanceID clients
 */
static MMPM_STATUS Test_Bandwidth_04(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, testSts[MAX_TEST][MAX_TEST_SEQUENCE];
    uint32 numTest = 0;
    ADSPPMTEST_LOG_TEST_HEADER(BANDWIDTH_04_NAME, BANDWIDTH_04_DETAILS);

    numTest = sizeof(testbandwidth04)/sizeof(AdsppmTestType);
    sts = Test_InvokeTest(testbandwidth04, numTest, testSts);

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_Bandwidth, BANDWIDTH_04_NAME, sts);
    return sts;
}


#define BANDWIDTH_05_NAME    "bandwidth_05"
#define BANDWIDTH_05_DETAILS "Req-Rel-Rel, expect NO errors"

static AdsppmTestType testbandwidth05[] =
{
    {1, 5, MMPM_CORE_ID_LPASS_AIF, MMPM_CORE_INSTANCE_0,
        {REG, REQBW, RELBW, RELBW, DREG}
    }
};

/**
 * @ingroup Bandwidth
 * @test bandwidth_05
 * @brief Req-Rel-Rel, expect NO errors
 */
static MMPM_STATUS Test_Bandwidth_05(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, testSts[MAX_TEST][MAX_TEST_SEQUENCE];
    uint32 numTest = 0;
    ADSPPMTEST_LOG_TEST_HEADER(BANDWIDTH_05_NAME, BANDWIDTH_05_DETAILS);

    numTest = sizeof(testbandwidth05)/sizeof(AdsppmTestType);
    sts = Test_InvokeTest(testbandwidth05, numTest, testSts);

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_Bandwidth, BANDWIDTH_05_NAME, sts);
    return sts;
}


#define BANDWIDTH_06_NAME    "bandwidth_06"
#define BANDWIDTH_06_DETAILS "Rel with no Req, expect NO errors"

static AdsppmTestType testbandwidth06[] =
{
    {1, 3, MMPM_CORE_ID_LPASS_ADSP, MMPM_CORE_INSTANCE_0,
        {REG, RELBW, DREG}
    }
};

/**
 * @ingroup Bandwidth
 * @test bandwidth_06
 * @brief Rel with no Req, expect NO errors
 */
static MMPM_STATUS Test_Bandwidth_06(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, testSts[MAX_TEST][MAX_TEST_SEQUENCE];
    uint32 numTest = 0;
    ADSPPMTEST_LOG_TEST_HEADER(BANDWIDTH_06_NAME, BANDWIDTH_06_DETAILS);

    numTest = sizeof(testbandwidth06)/sizeof(AdsppmTestType);
    sts = Test_InvokeTest(testbandwidth06, numTest, testSts);

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_Bandwidth, BANDWIDTH_06_NAME, sts);
    return sts;
}


#define BANDWIDTH_07_NAME    "bandwidth_07"
#define BANDWIDTH_07_DETAILS "Client 1 Req, client 2 Req-Rel; client 1 Rel should not affect client 1 Req"

static MMPM_STATUS TestBandwidthCl1ReqCl2ReqRel(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    BwReqTestType bw;
    uint32 clientId1 = 0, clientId2 = 0, setFreq, readFreq;
    uint64 bwVal = 100000000ull;
    AdsppmUT_Verify_BW_Params   expectedBWVal;
    MmpmCoreIdType coreId = MMPM_CORE_ID_LPASS_DML;
    MmpmCoreInstanceIdType instanceId = MMPM_CORE_INSTANCE_0;

    expectedBWVal.Ab = 0;
    expectedBWVal.Ib = 0;
    expectedBWVal.latencyNs = 0;


    clientId1 = Test_Register(coreId, instanceId, BANDWIDTH_07_NAME"_1");
    clientId2 = Test_Register(coreId, instanceId, BANDWIDTH_07_NAME"_2");

    if(clientId1 && clientId2)
    {
        bw.masterPort = MMPM_BW_PORT_ID_DML_MASTER;
        bw.slavePort = MMPM_BW_PORT_ID_AIF_SLAVE;
        bw.bwVal = bwVal;
        bw.usagePercent = 100;
        bw.usageType = MMPM_BW_USAGE_LPASS_DMA;

        expectedBWVal.Ab = bwVal;
        expectedBWVal.Ib = bwVal;

        sts = Test_RequestBandwidth(clientId1, &bw, 1, (void*)&expectedBWVal);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            retSts = MMPM_STATUS_FAILED;
        }

        bw.slavePort = MMPM_BW_PORT_ID_LPM_SLAVE;

        expectedBWVal.Ab += bwVal;
        sts = Test_RequestBandwidth(clientId2, &bw, 1, (void*)&expectedBWVal);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            retSts = MMPM_STATUS_FAILED;
        }

        expectedBWVal.Ab -= bwVal;
        sts = Test_ReleaseResource(clientId2, MMPM_RSC_ID_GENERIC_BW);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            retSts = MMPM_STATUS_FAILED;
        }

        // Query AHB clock frequency
        if(retSts == MMPM_STATUS_SUCCESS)
        {
            sts = Test_InfoClk(coreId, instanceId, MMPM_CLK_ID_LPASS_AHB_ROOT, &readFreq);
            if(MMPM_STATUS_SUCCESS != sts)
            {
                retSts = MMPM_STATUS_FAILED;
            }
            else
            {
                setFreq = getAhbFreqFromBw(bwVal);

                if(!Test_CompareFreq(setFreq, readFreq))
                {
                    ADSPPMTEST_LOG_PRINTF_2(
                        ADSPPMTEST_LOG_LEVEL_ERROR,
                        "[ERROR] Higher clock setting: setClkFreq = %u Hz, readClkFreq = %u Hz",
                        setFreq, readFreq);
                    retSts = MMPM_STATUS_FAILED;
                }
            }
        }

        expectedBWVal.Ab = expectedBWVal.Ib  = 19000000ull;
        sts = Test_ReleaseResource(clientId1, MMPM_RSC_ID_GENERIC_BW);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            retSts = MMPM_STATUS_FAILED;
        }

        // Query AHB clock frequency
        if(retSts == MMPM_STATUS_SUCCESS)
        {
            sts = Test_InfoClk(coreId, instanceId, MMPM_CLK_ID_LPASS_AHB_ROOT, &readFreq);
            if(MMPM_STATUS_SUCCESS != sts)
            {
                retSts = MMPM_STATUS_FAILED;
            }
            else
            {
                setFreq = getAhbFreqFromBw(19000000ull*4ull);

                if(!Test_CompareFreq(setFreq, readFreq))
                {
                    ADSPPMTEST_LOG_PRINTF_2(
                        ADSPPMTEST_LOG_LEVEL_ERROR,
                        "[ERROR] Lower clock setting: setClkFreq = %u Hz, readClkFreq = %u Hz",
                        setFreq, readFreq);
                    retSts = MMPM_STATUS_FAILED;
                }
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
 * @ingroup Bandwidth
 * @test bandwidth_07
 * @brief Client 1 Req, client 2 Req-Rel; client 2 Rel should not affect client 1 Req
 */
static MMPM_STATUS Test_Bandwidth_07(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(BANDWIDTH_07_NAME, BANDWIDTH_07_DETAILS);

    sts = TestBandwidthCl1ReqCl2ReqRel();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_Bandwidth, BANDWIDTH_07_NAME, sts);
    return sts;
}


#define BANDWIDTH_08_NAME    "bandwidth_08"
#define BANDWIDTH_08_DETAILS "2 clients Req-Rel"

static MMPM_STATUS TestBandwidthCl1ReqRelCl2ReqRel(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    BwReqTestType bw;
    uint32 clientId1 = 0, clientId2 = 0, setFreq, readFreq;
    uint64 bwVal = 100000000ull;
    AdsppmUT_Verify_BW_Params expectedBWVal;
    MmpmCoreIdType coreId = MMPM_CORE_ID_LPASS_AIF;
    MmpmCoreInstanceIdType instanceId = MMPM_CORE_INSTANCE_0;

    memset(&bw, 0, sizeof(BwReqTestType));

    expectedBWVal.Ab = 0;
    expectedBWVal.Ib = 0;
    expectedBWVal.latencyNs = 0;

    clientId1 = Test_Register(coreId, instanceId, BANDWIDTH_08_NAME"_1");
    clientId2 = Test_Register(coreId, instanceId, BANDWIDTH_08_NAME"_2");

    if(clientId1 && clientId2)
    {
        bw.masterPort = MMPM_BW_PORT_ID_AIF_MASTER;
        bw.slavePort = MMPM_BW_PORT_ID_DML_SLAVE;
        bw.bwVal = bwVal;
        bw.usagePercent = 100;
        bw.usageType = MMPM_BW_USAGE_LPASS_DSP;

        expectedBWVal.Ab = expectedBWVal.Ib = bwVal;
        sts = Test_RequestBandwidth(clientId1, &bw, 1, (void*)&expectedBWVal);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            retSts = MMPM_STATUS_FAILED;
        }

        bw.slavePort = MMPM_BW_PORT_ID_SLIMBUS_SLAVE;

        expectedBWVal.Ab += bwVal;
        sts = Test_RequestBandwidth(clientId2, &bw, 1, (void*)&expectedBWVal);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            retSts = MMPM_STATUS_FAILED;
        }

        // Query AHB clock frequency
        if(retSts == MMPM_STATUS_SUCCESS)
        {
            sts = Test_InfoClk(coreId, instanceId, MMPM_CLK_ID_LPASS_AHB_ROOT, &readFreq);
            if(MMPM_STATUS_SUCCESS != sts)
            {
                retSts = MMPM_STATUS_FAILED;
            }
            else
            {
                setFreq = getAhbFreqFromBw(bwVal*2ull);

                if(!Test_CompareFreq(setFreq, readFreq))
                {
                    ADSPPMTEST_LOG_PRINTF_2(
                        ADSPPMTEST_LOG_LEVEL_ERROR,
                        "[ERROR] Higher clock setting: setClkFreq = %u Hz, readClkFreq = %u Hz",
                        setFreq, readFreq);
                    retSts = MMPM_STATUS_FAILED;
                }
            }
        }

        expectedBWVal.Ab -= bwVal;
        sts = Test_ReleaseResource(clientId1, MMPM_RSC_ID_GENERIC_BW);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            retSts = MMPM_STATUS_FAILED;
        }

        expectedBWVal.Ab = expectedBWVal.Ib = 19000000ull;
        sts = Test_ReleaseResource(clientId2, MMPM_RSC_ID_GENERIC_BW);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            retSts = MMPM_STATUS_FAILED;
        }

        // Query AHB clock frequency
        if(retSts == MMPM_STATUS_SUCCESS)
        {
            sts = Test_InfoClk(coreId, instanceId, MMPM_CLK_ID_LPASS_AHB_ROOT, &readFreq);
            if(MMPM_STATUS_SUCCESS != sts)
            {
                retSts = MMPM_STATUS_FAILED;
            }
            else
            {
                setFreq = getAhbFreqFromBw(19000000ull*4ull);

                if(!Test_CompareFreq(setFreq, readFreq))
                {
                    ADSPPMTEST_LOG_PRINTF_2(
                        ADSPPMTEST_LOG_LEVEL_ERROR,
                        "[ERROR] Lower clock setting: setClkFreq = %u Hz, readClkFreq = %u Hz",
                        setFreq, readFreq);
                    retSts = MMPM_STATUS_FAILED;
                }
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
 * @ingroup Bandwidth
 * @test bandwidth_08
 * @brief 2 clients Req-Rel
 */
static MMPM_STATUS Test_Bandwidth_08(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(BANDWIDTH_08_NAME, BANDWIDTH_08_DETAILS);

    sts = TestBandwidthCl1ReqRelCl2ReqRel();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_Bandwidth, BANDWIDTH_08_NAME, sts);
    return sts;
}


#define BANDWIDTH_09_NAME    "bandwidth_09"
#define BANDWIDTH_09_DETAILS "Multiple clients Req-Rel"

static MMPM_STATUS TestBandwidthMultiClientReqRel(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    uint32 numBw = 0, numClient = 0, setFreq, readFreq, i;
    BwReqTestType bw[MAX_BW_REQUEST] = {};
    uint32 clientId[MAX_TEST_CLIENT] = {};
    MmpmCoreIdType coreId;
    MmpmCoreInstanceIdType instanceId;

    // Loop through all available coreID/instanceID combinations and request for bandwidth
    for(coreId = MMPM_CORE_ID_LPASS_ADSP; coreId < MMPM_CORE_ID_LPASS_LPM; coreId++)
    {
        for(instanceId = MMPM_CORE_INSTANCE_0; instanceId < MMPM_CORE_INSTANCE_MAX; instanceId++)
        {
            if(AsicInfo_isCoreInstanceAvailable(coreId, instanceId))
            {
                clientId[numClient] = Test_Register(coreId, instanceId, BANDWIDTH_09_NAME);
                if(clientId[numClient])
                {
                    GetReqBandwidthParam(coreId, bw, &numBw);

                    sts = Test_RequestBandwidth(clientId[numClient], &bw[0], numBw, NULL);
                    if(MMPM_STATUS_SUCCESS != sts)
                    {
                        retSts = MMPM_STATUS_FAILED;
                    }

                    numClient++;
                }
                else
                {
                    retSts = MMPM_STATUS_FAILED;
                }
            }
        }
    }

    // Query AHB clock frequency
    if(retSts == MMPM_STATUS_SUCCESS)
    {
        sts = Test_InfoClk(coreId, instanceId, MMPM_CLK_ID_LPASS_AHB_ROOT, &readFreq);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            retSts = MMPM_STATUS_FAILED;
        }
        else
        {
            setFreq = getAhbFreqFromBw(0xffffffffull);

            if(!Test_CompareFreq(setFreq, readFreq))
            {
                ADSPPMTEST_LOG_PRINTF_3(
                    ADSPPMTEST_LOG_LEVEL_ERROR,
                    "[ERROR] Higher clock setting: setClkFreq = %u Hz, readClkFreq = %u Hz core %u",
                    setFreq, readFreq, coreId-1);
                retSts = MMPM_STATUS_FAILED;
            }
        }
    }

    // Release and Deregister all clients
    if(0 < numClient)
    {
        for(i = 0; i < numClient; i++)
        {
            sts = Test_ReleaseResource(clientId[i], MMPM_RSC_ID_GENERIC_BW);
            if(MMPM_STATUS_SUCCESS != sts)
            {
                retSts = MMPM_STATUS_FAILED;
            }

            sts = Test_Deregister(clientId[i]);
            if(MMPM_STATUS_SUCCESS != sts)
            {
                retSts = MMPM_STATUS_FAILED;
            }
        }
    }

    // Query AHB clock frequency
    if(retSts == MMPM_STATUS_SUCCESS)
    {
        sts = Test_InfoClk(coreId, instanceId, MMPM_CLK_ID_LPASS_AHB_ROOT, &readFreq);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            retSts = MMPM_STATUS_FAILED;
        }
        else
        {
            setFreq = getAhbFreqFromBw(19000000ull*4ull);

            if(!Test_CompareFreq(setFreq, readFreq))
            {
                ADSPPMTEST_LOG_PRINTF_2(
                    ADSPPMTEST_LOG_LEVEL_ERROR,
                    "[ERROR] Lower clock setting: setClkFreq = %u Hz, readClkFreq = %u Hz",
                    setFreq, readFreq);
                retSts = MMPM_STATUS_FAILED;
            }
        }
    }

    return retSts;
}

/**
 * @ingroup Bandwidth
 * @test bandwidth_09
 * @brief Multiple clients Req-Rel
 */
MMPM_STATUS Test_Bandwidth_09(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(BANDWIDTH_09_NAME, BANDWIDTH_09_DETAILS);

    sts = TestBandwidthMultiClientReqRel();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_Bandwidth, BANDWIDTH_09_NAME, sts);
    return sts;
}


#define BANDWIDTH_10_NAME    "bandwidth_10"
#define BANDWIDTH_10_DETAILS "Multiple same client Reqs are updated"

static MMPM_STATUS TestBandwidthSameClientMultiReq(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    uint32 clientId = 0, setFreq, readFreq, i;
    MmpmCoreIdType coreId = MMPM_CORE_ID_LPASS_ADSP;
    MmpmCoreInstanceIdType instanceId = MMPM_CORE_INSTANCE_0;
    BwReqTestType bw;

    memset(&bw, 0, sizeof(BwReqTestType));

    clientId = Test_Register(coreId, instanceId, BANDWIDTH_10_NAME);

    if(clientId)
    {
        bw.masterPort = MMPM_BW_PORT_ID_ADSP_MASTER;
        bw.slavePort = MMPM_BW_PORT_ID_DML_SLAVE;
        bw.bwVal = 100000000;
        bw.usagePercent = 100;
        bw.usageType = MMPM_BW_USAGE_LPASS_DSP;

        for(i = 0; i < 5; i++)
        {
            bw.bwVal = 100000000 * (i + 1);

            sts = Test_RequestBandwidth(clientId, &bw, 1, NULL);
            if(MMPM_STATUS_SUCCESS != sts)
            {
                retSts = MMPM_STATUS_FAILED;
                break;
            }
            else
            {
                sts = Test_InfoClk(coreId, instanceId, MMPM_CLK_ID_LPASS_AHB_ROOT, &readFreq);
                if(MMPM_STATUS_SUCCESS != sts)
                {
                    retSts = MMPM_STATUS_FAILED;
                }
                else
                {
                    setFreq = getAhbFreqFromBw(bw.bwVal);

                    if(!Test_CompareFreq(setFreq, readFreq))
                    {
                        ADSPPMTEST_LOG_PRINTF_2(
                            ADSPPMTEST_LOG_LEVEL_ERROR,
                            "[ERROR] setClkFreq = %u Hz, readClkFreq = %u Hz",
                            setFreq, readFreq);
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
 * @ingroup Bandwidth
 * @test bandwidth_10
 * @brief Multiple same client Reqs are updated
 */
static MMPM_STATUS Test_Bandwidth_10(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(BANDWIDTH_10_NAME, BANDWIDTH_10_DETAILS);

    sts = TestBandwidthSameClientMultiReq();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_Bandwidth, BANDWIDTH_10_NAME, sts);
    return sts;
}


#define BANDWIDTH_11_NAME    "bandwidth_11"
#define BANDWIDTH_11_DETAILS "Invalid client ID Req, expect failure Req"

static MMPM_STATUS TestBandwidthInvalidClientId(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    uint32 clientId = 0;
    MmpmCoreIdType coreId = MMPM_CORE_ID_LPASS_ADSP;
    MmpmCoreInstanceIdType instanceId = MMPM_CORE_INSTANCE_0;
    BwReqTestType bw;

    memset(&bw, 0, sizeof(BwReqTestType));

    clientId = Test_Register(coreId, instanceId, BANDWIDTH_11_NAME);

    if(clientId)
    {
        bw.masterPort = MMPM_BW_PORT_ID_ADSP_MASTER;
        bw.slavePort = MMPM_BW_PORT_ID_DML_SLAVE;
        bw.bwVal = 60000000;
        bw.usagePercent = 100;
        bw.usageType = MMPM_BW_USAGE_LPASS_DSP;

        sts = Test_Deregister(clientId);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            retSts = MMPM_STATUS_FAILED;
        }

        sts = Test_RequestBandwidth(clientId, &bw, 1, NULL);
        if(MMPM_STATUS_SUCCESS == sts)
        {
            ADSPPMTEST_LOG_PRINTF_0(ADSPPMTEST_LOG_LEVEL_ERROR,
                "[ERROR] Request did not fail as intended");
            retSts = MMPM_STATUS_FAILED;
        }
        else
        {
            ADSPPMTEST_LOG_PRINTF_0(ADSPPMTEST_LOG_LEVEL_ERROR,
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
 * @ingroup Bandwidth
 * @test bandwidth_11
 * @brief Invalid client ID Req, expect failure Req
 */
static MMPM_STATUS Test_Bandwidth_11(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(BANDWIDTH_11_NAME, BANDWIDTH_11_DETAILS);

    sts = TestBandwidthInvalidClientId();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_Bandwidth, BANDWIDTH_11_NAME, sts);
    return sts;
}


#define BANDWIDTH_12_NAME    "bandwidth_12"
#define BANDWIDTH_12_DETAILS "Invalid parameters Req, expect failure Req"

static MMPM_STATUS TestBandwidthInvalidBwParam(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    BwReqTestType bw;
    uint32 clientId = 0;
    MmpmCoreIdType coreId = MMPM_CORE_ID_LPASS_ADSP;
    MmpmCoreInstanceIdType instanceId = MMPM_CORE_INSTANCE_0;

    memset(&bw, 0, sizeof(BwReqTestType));
    clientId = Test_Register(coreId, instanceId, BANDWIDTH_12_NAME);

    if(clientId)
    {
        bw.masterPort = MMPM_BW_PORT_ID_ADSP_MASTER;
        bw.slavePort = MMPM_BW_PORT_ID_DML_SLAVE;
        bw.bwVal = 60000000;
        bw.usagePercent = 100;
        bw.usageType = MMPM_BW_USAGE_LPASS_DSP;

        //Bad masterPort
        ADSPPMTEST_LOG_PRINTF_0(ADSPPMTEST_LOG_LEVEL_INFO,
            "[INFO] Test invalid masterPort");
        bw.masterPort = 255;
        sts = Test_RequestBandwidth(clientId, &bw, 1, NULL);
        if(MMPM_STATUS_SUCCESS == sts)
        {
            ADSPPMTEST_LOG_PRINTF_0(ADSPPMTEST_LOG_LEVEL_ERROR,
                "[ERROR] Request returned success when expecting failure");
            retSts = MMPM_STATUS_FAILED;
        }
        else
        {
            ADSPPMTEST_LOG_PRINTF_0(ADSPPMTEST_LOG_LEVEL_ERROR,
                "[PASS] Failed return is intended");
        }
        bw.masterPort = MMPM_BW_PORT_ID_ADSP_MASTER;

        //Bad slavePort
        ADSPPMTEST_LOG_PRINTF_0(ADSPPMTEST_LOG_LEVEL_INFO,
            "[INFO] Test invalid slavePort");
        bw.slavePort = 255;
        sts = Test_RequestBandwidth(clientId, &bw, 1, NULL);
        if(MMPM_STATUS_SUCCESS == sts)
        {
            ADSPPMTEST_LOG_PRINTF_0(ADSPPMTEST_LOG_LEVEL_ERROR,
                "[ERROR] Request returned success when expecting failure");
            retSts = MMPM_STATUS_FAILED;
        }
        else
        {
            ADSPPMTEST_LOG_PRINTF_0(ADSPPMTEST_LOG_LEVEL_ERROR,
                "[PASS] Failed return is intended");
        }
        bw.slavePort = MMPM_BW_PORT_ID_DML_SLAVE;

        // Bad usageType
        ADSPPMTEST_LOG_PRINTF_0(ADSPPMTEST_LOG_LEVEL_INFO,
            "[INFO] Test invalid usageType");
        bw.usageType = 100;
        sts = Test_RequestBandwidth(clientId, &bw, 1, NULL);
        if(MMPM_STATUS_SUCCESS == sts)
        {
            ADSPPMTEST_LOG_PRINTF_0(ADSPPMTEST_LOG_LEVEL_ERROR,
                "[ERROR] Request returned success when expecting failure");
            retSts = MMPM_STATUS_FAILED;
        }
        else
        {
            ADSPPMTEST_LOG_PRINTF_0(ADSPPMTEST_LOG_LEVEL_ERROR,
                "[PASS] Failed return is intended");
        }
        bw.usageType = MMPM_BW_USAGE_LPASS_DSP;

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
 * @ingroup Bandwidth
 * @test bandwidth_12
 * @brief Invalid parameters Req, expect failure Req
 */
static MMPM_STATUS Test_Bandwidth_12(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(BANDWIDTH_12_NAME, BANDWIDTH_12_DETAILS);

    sts = TestBandwidthInvalidBwParam();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_Bandwidth, BANDWIDTH_12_NAME, sts);
    return sts;
}


#define BANDWIDTH_13_NAME    "bandwidth_13"
#define BANDWIDTH_13_DETAILS "LPASS AHB clock frequency plan"

static MMPM_STATUS TestBandwidthLpassAhbFreqPlan(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    uint32 clientId = 0, setFreq, readFreq, i;
    const AsicInfo_ClockFreqVoltPlanType *pFreqPlan = NULL;
    MmpmCoreIdType coreId = MMPM_CORE_ID_LPASS_ADSP;
    MmpmCoreInstanceIdType instanceId = MMPM_CORE_INSTANCE_0;
    BwReqTestType bw;

    memset(&bw, 0, sizeof(BwReqTestType));
    clientId = Test_Register(coreId, instanceId, BANDWIDTH_13_NAME);

    if(clientId)
    {
        pFreqPlan = AsicInfo_getClockFreqVoltPlan(MMPM_CLK_ID_LPASS_AHB_ROOT);

        bw.masterPort = MMPM_BW_PORT_ID_ADSP_MASTER;
        bw.slavePort = MMPM_BW_PORT_ID_DML_SLAVE;
        bw.usagePercent = 100;
        bw.usageType = MMPM_BW_USAGE_LPASS_DSP;

        for(i = 0; i < pFreqPlan->numFreq; i++)
        {
            setFreq = pFreqPlan->pFreqVoltPlan[i].frequency;
            bw.bwVal = setFreq * 4;

            sts = Test_RequestBandwidth(clientId, &bw, 1, NULL);
            if(MMPM_STATUS_SUCCESS != sts)
            {
                retSts = MMPM_STATUS_FAILED;
                break;
            }
            else
            {
                sts = Test_InfoClk(coreId, instanceId, MMPM_CLK_ID_LPASS_AHB_ROOT, &readFreq);
                if(MMPM_STATUS_SUCCESS != sts)
                {
                    retSts = MMPM_STATUS_FAILED;
                }
                else
                {
                    if(!Test_CompareFreq(setFreq, readFreq))
                    {
                        ADSPPMTEST_LOG_PRINTF_2(
                            ADSPPMTEST_LOG_LEVEL_ERROR,
                            "[ERROR] setClkFreq = %u Hz, readClkFreq = %u Hz",
                            setFreq, readFreq);
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
 * @ingroup Bandwidth
 * @test bandwidth_13
 * @brief LPASS AHB clock frequency plan
 */
static MMPM_STATUS Test_Bandwidth_13(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(BANDWIDTH_13_NAME, BANDWIDTH_13_DETAILS);

    sts = TestBandwidthLpassAhbFreqPlan();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_Bandwidth, BANDWIDTH_13_NAME, sts);
    return sts;
}


#define BANDWIDTH_14_NAME    "bandwidth_14"
#define BANDWIDTH_14_DETAILS "Zero BW request"

static MMPM_STATUS TestBandwidthZeroBwReq(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    uint32 clientId = 0, setFreq, readFreq;
    const AsicInfo_ClockFreqVoltPlanType *pFreqPlan = NULL;
    MmpmCoreIdType coreId = MMPM_CORE_ID_LPASS_ADSP;
    MmpmCoreInstanceIdType instanceId = MMPM_CORE_INSTANCE_0;
    BwReqTestType bw;

    memset(&bw, 0, sizeof(BwReqTestType));
    clientId = Test_Register(coreId, instanceId, BANDWIDTH_14_NAME);

    if(clientId)
    {
        pFreqPlan = AsicInfo_getClockFreqVoltPlan(MMPM_CLK_ID_LPASS_AHB_ROOT);

        bw.masterPort = MMPM_BW_PORT_ID_ADSP_MASTER;
        bw.slavePort = MMPM_BW_PORT_ID_DML_SLAVE;
        bw.usagePercent = 100;
        bw.usageType = MMPM_BW_USAGE_LPASS_DSP;

        // Request higher clock setting
        setFreq = pFreqPlan->pFreqVoltPlan[pFreqPlan->numFreq - 1].frequency;
        bw.bwVal = setFreq * 4;

        sts = Test_RequestBandwidth(clientId, &bw, 1, NULL);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            retSts = MMPM_STATUS_FAILED;
        }
        else
        {
            sts = Test_InfoClk(coreId, instanceId, MMPM_CLK_ID_LPASS_AHB_ROOT, &readFreq);
            if(MMPM_STATUS_SUCCESS != sts)
            {
                retSts = MMPM_STATUS_FAILED;
            }
            else
            {
                if(!Test_CompareFreq(setFreq, readFreq))
                {
                    ADSPPMTEST_LOG_PRINTF_2(
                        ADSPPMTEST_LOG_LEVEL_ERROR,
                        "[ERROR] Higher clock setting: setClkFreq = %u Hz, readClkFreq = %u Hz",
                        setFreq, readFreq);
                    retSts = MMPM_STATUS_FAILED;
                }
            }
        }

        // Request Zero BW, should release resource
        setFreq = pFreqPlan->pFreqVoltPlan[0].frequency;
        bw.bwVal = 0;

        sts = Test_RequestBandwidth(clientId, &bw, 1, NULL);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            ADSPPMTEST_LOG_PRINTF_0(
                        ADSPPMTEST_LOG_LEVEL_ERROR,
                        "[ERROR] Test_RequestBandwidth size 0 failed in bw 14 test case");
            retSts = MMPM_STATUS_FAILED;
        }
        else
        {
            sts = Test_InfoClk(coreId, instanceId, MMPM_CLK_ID_LPASS_AHB_ROOT, &readFreq);
            if(MMPM_STATUS_SUCCESS != sts)
            {
                ADSPPMTEST_LOG_PRINTF_0(
                        ADSPPMTEST_LOG_LEVEL_ERROR,
                        "[ERROR] Test Infoclock failed in bw 14 test case");
                retSts = MMPM_STATUS_FAILED;
            }
            else
            {
                if(!Test_CompareFreq(setFreq, readFreq))
                {
                    ADSPPMTEST_LOG_PRINTF_2(
                        ADSPPMTEST_LOG_LEVEL_ERROR,
                        "[ERROR] Lower clock setting: setClkFreq = %u Hz, readClkFreq = %u Hz",
                        setFreq, readFreq);
                    retSts = MMPM_STATUS_FAILED;
                }
            }
        }

        sts = Test_Deregister(clientId);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            ADSPPMTEST_LOG_PRINTF_0(
                        ADSPPMTEST_LOG_LEVEL_ERROR,
                        "[ERROR] Test_DeRegister failed in bw 14 test case");
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
 * @ingroup Bandwidth
 * @test bandwidth_14
 * @brief Zero BW request
 */
static MMPM_STATUS Test_Bandwidth_14(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(BANDWIDTH_14_NAME, BANDWIDTH_14_DETAILS);

    sts = TestBandwidthZeroBwReq();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_Bandwidth, BANDWIDTH_14_NAME, sts);
    return sts;
}

#define BANDWIDTH_15_NAME    "bandwidth_15"
#define BANDWIDTH_15_DETAILS "Req-Rel DDR BW, all core/instance/master/usage combination"

static MMPM_STATUS TestAllCoresDdrBwRequest(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    MmpmCoreIdType coreId;
    MmpmCoreInstanceIdType instanceId;
    uint32 clientId = 0;
    MmpmBwPortIdType master, slave = MMPM_BW_PORT_ID_DDR_SLAVE;
    MmpmBwUsageLpassType usage;
    BwReqTestType_Ext bwReq;
    memset(&bwReq, 0, sizeof(BwReqTestType_Ext));

    for(coreId = MMPM_CORE_ID_LPASS_ADSP; coreId < MMPM_CORE_ID_LPASS_END; coreId++)
    {
        for(instanceId = MMPM_CORE_INSTANCE_0; instanceId < MMPM_CORE_INSTANCE_MAX; instanceId++)
        {
            if(AsicInfo_isCoreInstanceAvailable(coreId, instanceId))
            {
                clientId = Test_Register(coreId, instanceId, BANDWIDTH_15_NAME);
                if(clientId)
                {
                    for(master = MMPM_BW_PORT_ID_ADSP_MASTER; master < MMPM_BW_PORT_ID_VAPSS_HCP_MASTER; master++)
                    {
                        if(AsicInfo_isBusPortAvailable(master))
                        {
                            for(usage = MMPM_BW_USAGE_LPASS_DSP; usage < MMPM_BW_USAGE_LPASS_ENUM_MAX; usage++)
                            {
                                bwReq.masterPort = master;
                                bwReq.slavePort = slave;
                                bwReq.bwVal = 100000000;
                                bwReq.usagePercent = 100;
                                bwReq.usageType = usage;

                                sts = Test_RequestBandwidthExt(clientId, &bwReq, 1);
                                if(MMPM_STATUS_SUCCESS != sts)
                                {
                                    retSts = MMPM_STATUS_FAILED;
                                }
                                else
                                {
                                    sts = Test_ReleaseResource(clientId, MMPM_RSC_ID_GENERIC_BW_EXT);
                                    if(MMPM_STATUS_SUCCESS != sts)
                                    {
                                        retSts = MMPM_STATUS_FAILED;
                                    }
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
 * @ingroup Bandwidth
 * @test bandwidth_15
 * @brief Req-Rel DDR BW, all core/instance/master/usage combination
 */
static MMPM_STATUS Test_Bandwidth_15(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(BANDWIDTH_15_NAME, BANDWIDTH_15_DETAILS);

    sts = TestAllCoresDdrBwRequest();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_Bandwidth, BANDWIDTH_15_NAME, sts);
    return sts;
}

#define BANDWIDTH_16_NAME    "bandwidth_16"
#define BANDWIDTH_16_DETAILS "Req Honest/Dishonest Q6 to DDR BW, check the final ab/ib"
static MMPM_STATUS TestBandwidthReqRelQ6DdrBwExt(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    MmpmCoreIdType coreId = MMPM_CORE_ID_LPASS_ADSP;
    uint32 numBw,i;
    uint64 totalAb =0,totalIb =0;
    AdsppmInfoAggregatedBwType bwInfo;
    MmpmCoreInstanceIdType instanceId = MMPM_CORE_INSTANCE_0;
    uint32 clientIdArray[MAX_BW_REQUEST];
    MmpmReqBandwidthTestParamTableTypeExt bwTable;


    /* Get the DDR bandwidth Params */
    GetReqDdrBandwidthParam( bwTable.bwExt, &numBw, 0);

    /* Register the BW Clients */
    for ( i = 0; i < numBw; i++)
    {
        clientIdArray[i] = Test_Register(coreId, instanceId, BANDWIDTH_16_NAME);
        if (!clientIdArray[i])
        {
            retSts = MMPM_STATUS_FAILED;
        }
    }

    /* Request for Bandwidth */
    for ( i = 0; i < numBw; i++)
    {
        if(clientIdArray[i])
        {
            sts = Test_RequestBandwidthExt( clientIdArray[i], &(bwTable.bwExt[i]), 1);
            if(MMPM_STATUS_SUCCESS != sts)
            {
                retSts = MMPM_STATUS_FAILED;
            }
        }
    }

    /* Now Get the actual DDR BW info */
    sts = Test_InfoBw(coreId, instanceId, &bwInfo);
    if(MMPM_STATUS_SUCCESS != sts)
    {
       retSts = MMPM_STATUS_FAILED;
    }

    /* Calculate the ib/ab votes from the table */
    calculateDdrBwFromTable( &(bwTable.bwExt[0]), numBw, &totalAb, &totalIb);

    /* Check actual aggregated BW values are greater or equal to expected values */
    /* This is just a sanity check since other clients may be running and cause
       queried results to be higher than expected */
    if (bwInfo.adspDdrBwAb < totalAb || bwInfo.adspDdrBwIb < totalIb)
    {
        retSts = MMPM_STATUS_FAILED;
        ADSPPMTEST_LOG_PRINTF_4(
                        ADSPPMTEST_LOG_LEVEL_ERROR,
                        "[ERROR] Final BW Values are too low - Expected ab = %u Expected ib = %u Final ab = %u Final ib = %u",
                        (uint32)totalAb, (uint32)totalIb, (uint32)(bwInfo.adspDdrBwAb), (uint32)(bwInfo.adspDdrBwIb));
    }
    else
    {
        ADSPPMTEST_LOG_PRINTF_4(
                        ADSPPMTEST_LOG_LEVEL_INFO,
                        "[PASS] Final BW Values are sane - Expected ab = %u Expected ib = %u Final ab = %u Final ib = %u",
                        (uint32)totalAb, (uint32)totalIb, (uint32)(bwInfo.adspDdrBwAb), (uint32)(bwInfo.adspDdrBwIb));
    }


    /* Release the BW */
    for ( i = 0; i < numBw; i++)
    {
        if(clientIdArray[i])
        {
            if(bwTable.bwExt[i].type == Bw_Generic)
            {
                sts = Test_ReleaseResource(clientIdArray[i], MMPM_RSC_ID_GENERIC_BW);
                if(MMPM_STATUS_SUCCESS != sts)
                {
                    retSts = MMPM_STATUS_FAILED;
                }
            }
            else
            {
                sts = Test_ReleaseResource(clientIdArray[i], MMPM_RSC_ID_GENERIC_BW_EXT);
                if(MMPM_STATUS_SUCCESS != sts)
                {
                    retSts = MMPM_STATUS_FAILED;
                }
            }
        }
    }

    /* Deregister Client */
    for ( i = 0; i < numBw ; i++)
    {
        sts = Test_Deregister(clientIdArray[i]);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            retSts = MMPM_STATUS_FAILED;
        }
    }

    return retSts;
}

/**
 * @ingroup Bandwidth
 * @test bandwidth_16
 * @brief Req Honest/Dishonest Q6 to DDR BW, check the final ab/ib
 */
static MMPM_STATUS Test_Bandwidth_16(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(BANDWIDTH_16_NAME, BANDWIDTH_16_DETAILS);

    sts = TestBandwidthReqRelQ6DdrBwExt();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_Bandwidth, BANDWIDTH_16_NAME, sts);
    return sts;
}

#define BANDWIDTH_17_NAME    "bandwidth_17"
#define BANDWIDTH_17_DETAILS "Req-Rel Honest/Dishonest LPASS to DDR BW, check the final ab/ib "
static MMPM_STATUS TestBandwidthReqRelLpassDdrBwExt(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    MmpmCoreIdType coreId = MMPM_CORE_ID_LPASS_ADSP;
    uint32 numBw,i;
    AdsppmInfoAggregatedBwType bwInfo;
    uint64 totalAb =0,totalIb =0;
    MmpmCoreInstanceIdType instanceId = MMPM_CORE_INSTANCE_0;
    uint32 clientIdArray[MAX_BW_REQUEST];
    MmpmReqBandwidthTestParamTableTypeExt bwTable;

    memset(&bwTable,0,sizeof(MmpmReqBandwidthTestParamTableTypeExt));

    /* Get the DDR bandwidth Params */
    GetReqDdrBandwidthParam( bwTable.bwExt, &numBw, 1);

    /*Register the BW Clients */
    for ( i = 0; i < numBw; i++)
    {
        clientIdArray[i] = Test_Register(coreId, instanceId, BANDWIDTH_16_NAME);
        if (!clientIdArray[i])
        {
            retSts = MMPM_STATUS_FAILED;
        }
    }

    /* Request for Bandwidth */
    for ( i = 0; i < numBw; i++)
    {
        if(clientIdArray[i])
        {
            sts = Test_RequestBandwidthExt( clientIdArray[i], &(bwTable.bwExt[i]), 1);
            if(MMPM_STATUS_SUCCESS != sts)
            {
                retSts = MMPM_STATUS_FAILED;
            }
        }
    }

    /* Now Get the actual DDR BW info */
    sts = Test_InfoBw(coreId, instanceId, &bwInfo);
    if(MMPM_STATUS_SUCCESS != sts)
    {
       retSts = MMPM_STATUS_FAILED;
    }

    /* Calculate the ib/ab votes with no compensation, since
       no compensation should be applied for LPASS->DDR BW votes */
    calculateBwNoCompensation( &(bwTable.bwExt[0]), numBw, &totalAb, &totalIb);

    /* Check actual aggregated BW values are greater or equal to expected values */
    /* This is just a sanity check since other clients may be running and cause
       queried results to be higher than expected */
    if (bwInfo.extAhbDdrBwAb < totalAb || bwInfo.extAhbDdrBwIb < totalIb)
    {
        retSts = MMPM_STATUS_FAILED;
        ADSPPMTEST_LOG_PRINTF_4(
                        ADSPPMTEST_LOG_LEVEL_ERROR,
                        "[ERROR] Final BW Values are too low - Expected ab = %u Expected ib = %u Final ab = %u Final ib = %u",
                        (uint32)totalAb, (uint32)totalIb, (uint32)(bwInfo.extAhbDdrBwAb), (uint32)(bwInfo.extAhbDdrBwIb));
    }
    else
    {
        ADSPPMTEST_LOG_PRINTF_4(
                        ADSPPMTEST_LOG_LEVEL_INFO,
                        "[PASS] Final BW Values are sane - Expected ab = %u Expected ib = %u Final ab = %u Final ib = %u",
                        (uint32)totalAb, (uint32)totalIb, (uint32)(bwInfo.extAhbDdrBwAb), (uint32)(bwInfo.extAhbDdrBwIb));
    }

    /* Release the BW */
    for ( i = 0; i < numBw; i++)
    {
        if(clientIdArray[i])
        {
            if(bwTable.bwExt[i].type == Bw_Generic)
            {
                sts = Test_ReleaseResource(clientIdArray[i], MMPM_RSC_ID_GENERIC_BW);
                if(MMPM_STATUS_SUCCESS != sts)
                {
                    retSts = MMPM_STATUS_FAILED;
                }
            }
            else
            {
                sts = Test_ReleaseResource(clientIdArray[i], MMPM_RSC_ID_GENERIC_BW_EXT);
                if(MMPM_STATUS_SUCCESS != sts)
                {
                    retSts = MMPM_STATUS_FAILED;
                }
            }
        }
    }

    /* Deregister Client */
    for ( i = 0; i < numBw ; i++)
    {
        sts = Test_Deregister(clientIdArray[i]);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            retSts = MMPM_STATUS_FAILED;
        }
    }

    return retSts;
}

/**
 * @ingroup Bandwidth
 * @test bandwidth_17
 * @brief Req-Rel Honest/Dishonest LPASS to DDR BW,check the final ab/ib
 */
static MMPM_STATUS Test_Bandwidth_17(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(BANDWIDTH_17_NAME, BANDWIDTH_17_DETAILS);

    sts = TestBandwidthReqRelLpassDdrBwExt();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_Bandwidth, BANDWIDTH_17_NAME, sts);
    return sts;
}


#define BANDWIDTH_18_NAME    "bandwidth_18"
#define BANDWIDTH_18_DETAILS "Req-Rel very large LPASS to DDR BW, query the result using GetInfo"
MMPM_STATUS TestBandwidthReqRelLargeBwAndQuery(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    uint32 clientId = 0;
    MmpmCoreIdType coreId = MMPM_CORE_ID_LPASS_ADSP;
    MmpmCoreInstanceIdType instanceId = MMPM_CORE_INSTANCE_0;
    BwReqTestType_Ext bw;

    memset(&bw,0,sizeof(BwReqTestType_Ext));
    clientId = Test_Register(coreId, instanceId, BANDWIDTH_18_NAME);

    if(clientId)
    {
        bw.masterPort = MMPM_BW_PORT_ID_ADSP_MASTER;
        bw.slavePort = MMPM_BW_PORT_ID_DDR_SLAVE;
        // Set a very high BW which exceeds 32-bits, to test the 64-bit path
        // of the BW vote and subsequent query
        bw.bwVal = (uint64) 1500 * 8 * 1000000; // 12 billion
        bw.usagePercent = 100;
        bw.usageType = MMPM_BW_USAGE_LPASS_DSP;
        bw.type = Bw_Generic_Ext;

        // Request BW
        sts = Test_RequestBandwidthExt(clientId, &bw, 1);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            retSts = MMPM_STATUS_FAILED;
        }
        else
        {
            // Query the total requested BW using GetInfo()
            MmpmInfoDataType infoData;
            MmpmInfoBwExtType bwExtInfo;
            infoData.infoId = MMPM_INFO_ID_BW_EXT;
            infoData.coreId = MMPM_CORE_ID_LPASS_DML;
            infoData.instanceId = MMPM_CORE_INSTANCE_0;
            bwExtInfo.busRoute.masterPort = MMPM_BW_PORT_ID_ADSP_MASTER;
            bwExtInfo.busRoute.slavePort = MMPM_BW_PORT_ID_DDR_SLAVE;
            bwExtInfo.totalBw = 0; // output
            infoData.info.pBwExtInfo = &bwExtInfo;
            sts = MMPM_GetInfo(&infoData);
            if(sts == MMPM_STATUS_SUCCESS)
            {
                // Check result of GetInfo()
                if(bwExtInfo.totalBw < bw.bwVal)
                {
                    PRINT_ERROR_4("MMPM_GetInfo() returned total BW value "
                        "which is less than expected: 0x%x%x (expected at "
                        "least 0x%x%x",
                        (uint32) (bwExtInfo.totalBw >> 32),
                        (uint32) bwExtInfo.totalBw,
                        (uint32) (bw.bwVal >> 32),
                        (uint32) bwExtInfo.totalBw);
                    retSts = MMPM_STATUS_FAILED;
                }
            }
            else
            {
                PRINT_ERROR_1("MMPM_GetInfo() failed; %u", sts);
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
 * @ingroup Bandwidth
 * @test bandwidth_18
 * @brief Req-Rel very large LPASS to DDR BW, query the result using GetInfo
 */
static MMPM_STATUS Test_Bandwidth_18(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(BANDWIDTH_18_NAME, BANDWIDTH_18_DETAILS);

    sts = TestBandwidthReqRelLargeBwAndQuery();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_Bandwidth, BANDWIDTH_18_NAME, sts);
    return sts;
}

#define BANDWIDTH_19_NAME    "bandwidth_19"
#define BANDWIDTH_19_DETAILS "Req compensated ADSP to LPASS BW, check the final ab/ib"
static MMPM_STATUS TestBandwidthReqRelAdspLpassBwExt(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    MmpmCoreIdType coreId = MMPM_CORE_ID_LPASS_ADSP;
    uint32 numBw, i;
    uint64 totalAb = 0, totalIb = 0;
    AdsppmInfoAggregatedBwType bwInfo;
    MmpmCoreInstanceIdType instanceId = MMPM_CORE_INSTANCE_0;
    uint32 clientIdArray[MAX_BW_REQUEST];
    MmpmReqBandwidthTestParamTableTypeExt bwTable;

    /* Get the internal bandwidth Params */
    GetReqInternalBandwidthParam(bwTable.bwExt, &numBw, 0);

    /* Register the BW Clients */
    for(i = 0; i < numBw; i++)
    {
        clientIdArray[i] = Test_Register(coreId, instanceId, BANDWIDTH_19_NAME);
        if (!clientIdArray[i])
        {
            retSts = MMPM_STATUS_FAILED;
        }
    }

    /* Request for Bandwidth */
    for(i = 0; i < numBw; i++)
    {
        if(clientIdArray[i])
        {
            sts = Test_RequestBandwidthExt(clientIdArray[i], &(bwTable.bwExt[i]), 1);
            if(MMPM_STATUS_SUCCESS != sts)
            {
                retSts = MMPM_STATUS_FAILED;
            }
        }
    }

    /* Now Get the actual DDR BW info */
    sts = Test_InfoBw(coreId, instanceId, &bwInfo);
    if(MMPM_STATUS_SUCCESS != sts)
    {
       retSts = MMPM_STATUS_FAILED;
    }

    /* Calculate the ib/ab votes from the table */
    calculateAhbBwFromTable(&(bwTable.bwExt[0]), numBw, &totalAb, &totalIb);

    /* Check actual aggregated BW values are greater or equal to expected values */
    /* This is just a sanity check since other clients may be running and cause
       queried results to be higher than expected */
    if (bwInfo.intAhbBwAb < totalAb || bwInfo.intAhbBwIb < totalIb)
    {
        retSts = MMPM_STATUS_FAILED;
        ADSPPMTEST_LOG_PRINTF_4(
            ADSPPMTEST_LOG_LEVEL_ERROR,
            "[ERROR] Final BW Values are too low - Expected ab = %u Expected ib = %u Final ab = %u Final ib = %u",
            (uint32)totalAb, (uint32)totalIb, (uint32)(bwInfo.intAhbBwAb), (uint32)(bwInfo.intAhbBwIb));
    }
    else
    {
        ADSPPMTEST_LOG_PRINTF_4(
            ADSPPMTEST_LOG_LEVEL_INFO,
            "[PASS] Final BW Values are sane - Expected ab = %u Expected ib = %u Final ab = %u Final ib = %u",
            (uint32)totalAb, (uint32)totalIb, (uint32)(bwInfo.intAhbBwAb), (uint32)(bwInfo.intAhbBwIb));
    }

    /* Release the BW */
    for(i = 0; i < numBw; i++)
    {
        if(clientIdArray[i])
        {
            if(bwTable.bwExt[i].type == Bw_Generic)
            {
                sts = Test_ReleaseResource(clientIdArray[i], MMPM_RSC_ID_GENERIC_BW);
                if(MMPM_STATUS_SUCCESS != sts)
                {
                    retSts = MMPM_STATUS_FAILED;
                }
            }
            else
            {
                sts = Test_ReleaseResource(clientIdArray[i], MMPM_RSC_ID_GENERIC_BW_EXT);
                if(MMPM_STATUS_SUCCESS != sts)
                {
                    retSts = MMPM_STATUS_FAILED;
                }
            }
        }
    }

    /* Deregister Client */
    for(i = 0; i < numBw; i++)
    {
        sts = Test_Deregister(clientIdArray[i]);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            retSts = MMPM_STATUS_FAILED;
        }
    }

    return retSts;
}

/**
 * @ingroup Bandwidth
 * @test bandwidth_19
 * @brief Req compensated ADSP to DDR BW, check the final ab/ib
 */
static MMPM_STATUS Test_Bandwidth_19(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(BANDWIDTH_19_NAME, BANDWIDTH_19_DETAILS);

    sts = TestBandwidthReqRelAdspLpassBwExt();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_Bandwidth, BANDWIDTH_19_NAME, sts);
    return sts;
}

#define BANDWIDTH_20_NAME    "bandwidth_20"
#define BANDWIDTH_20_DETAILS "Req BW with invalid 'numOfBw' parameter"
static MMPM_STATUS TestBandwidthInvalidNumber(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    MmpmCoreIdType coreId = MMPM_CORE_ID_LPASS_ADSP;
    MmpmCoreInstanceIdType instanceId = MMPM_CORE_INSTANCE_0;
    uint32 clientId;
    MmpmRscParamType mmpmRscParam;
    MmpmRscExtParamType mmpmRscExtParam;
    MmpmGenBwValType bandWidthArray[1];
    MmpmGenBwReqType mmpmBwReqParam;
    MMPM_STATUS detailSts = MMPM_STATUS_FAILED;

    clientId = Test_Register(coreId, instanceId, BANDWIDTH_20_NAME);
    if(clientId == 0)
        goto TestBandwidthInvalidNumber_error;

    bandWidthArray[0].busRoute.masterPort = MMPM_BW_PORT_ID_ADSP_MASTER;
    bandWidthArray[0].busRoute.slavePort = MMPM_BW_PORT_ID_DDR_SLAVE;
    bandWidthArray[0].bwValue.busBwValue.bwBytePerSec = 1000;
    bandWidthArray[0].bwValue.busBwValue.usagePercentage = 10;
    bandWidthArray[0].bwValue.busBwValue.usageType= MMPM_BW_USAGE_LPASS_DSP;

    // BW request with bogus 'numOfBw' parameter to force
    // 1. overflow detection in QDI layer (MPD mode)
    //    or
    // 2. check against MAX_NUM_OF_ADSPPM_BW_ARRAY in API layer (SPD mode)
    mmpmBwReqParam.numOfBw = UINT_MAX / 2;
    mmpmBwReqParam.pBandWidthArray = bandWidthArray;
    mmpmRscParam.rscId = MMPM_RSC_ID_GENERIC_BW;
    mmpmRscParam.rscParam.pGenBwReq = &mmpmBwReqParam;
    mmpmRscExtParam.apiType = MMPM_API_TYPE_SYNC;
    mmpmRscExtParam.numOfReq = 1;
    mmpmRscExtParam.pReqArray = &mmpmRscParam;
    mmpmRscExtParam.pStsArray = &detailSts;

    sts = MMPM_Request_Ext(clientId, &mmpmRscExtParam);
    // Expecting failure
    if(MMPM_STATUS_SUCCESS == sts)
        goto TestBandwidthInvalidNumber_error;

    // Cleanup
    sts = Test_Deregister(clientId);
    if(MMPM_STATUS_SUCCESS != sts)
        goto TestBandwidthInvalidNumber_error;

    return retSts;

TestBandwidthInvalidNumber_error:

    if(clientId != 0)
        Test_Deregister(clientId);

    return MMPM_STATUS_FAILED;

}

/**
 * @ingroup Bandwidth
 * @test bandwidth_20
 * @brief Req BW with invalid 'numOfBw' parameter
 */
static MMPM_STATUS Test_Bandwidth_20(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(BANDWIDTH_20_NAME, BANDWIDTH_20_DETAILS);

    sts = TestBandwidthInvalidNumber();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_Bandwidth, BANDWIDTH_20_NAME, sts);
    return sts;
}

void Test_Bandwidth(void)
{


    // Req-Rel, all core/instance/master/slave/usage combination
    Test_Bandwidth_01();

    // Req-Rel DDR BW, all core/instance/master combination
    Test_Bandwidth_02();

    // Req-Rel PNOC BW, all core/instance/master combination
    Test_Bandwidth_03();

    // Req-Rel, 2 same coreID/instanceID clients
    Test_Bandwidth_04();

    // Req-Rel-Rel, expect NO errors
    Test_Bandwidth_05();

    // Rel with no Req,expect NO errors
    Test_Bandwidth_06();

    // Client 1 Req, client 2 Req-Rel; client 2 Rel should not affect client 1 Req
    Test_Bandwidth_07();

    // 2 clients Req-Rel
    Test_Bandwidth_08();

    // Multiple clients Req-Rel
    Test_Bandwidth_09();

    // Multiple same client Reqs are updated
    Test_Bandwidth_10();

    // Invalid client ID Req, expect failure Req
    Test_Bandwidth_11();

    // Invalid parameters Req, expect failure Req
    Test_Bandwidth_12();

    // LPASS AHB clock frequency plan
    Test_Bandwidth_13();

    // Zero BW request
    //Test_Bandwidth_14();

    // Req-Rel DDR BW, all core/instance/master/usage combination
    Test_Bandwidth_15();

    // DisHonest Q6 to DDR BW calculation verifying
    Test_Bandwidth_16();

    // Honest-DisHonest LPASS to DDR BW calculation verifying
    Test_Bandwidth_17();

    // Req-Rel very large LPASS to DDR BW, query the result using GetInfo
    Test_Bandwidth_18();

    // Req compensated ADSP to LPASS BW, check the final ab/ib
    Test_Bandwidth_19();

    // Req BW with invalid 'numOfBw' parameter
    Test_Bandwidth_20();

}



static MMPM_STATUS TestCDSPBandwidthReqRel( MmpmCoreIdType   coreId,
                                            MmpmBwPortIdType master,
                                            MmpmBwPortIdType slave )
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    uint32 clientId = 0;
    BwReqTestType bw;

    clientId = Test_Register(coreId, MMPM_CORE_INSTANCE_0, "SLPI/CDSP_BW");
    if(clientId)
    {

        bw.slavePort = slave;
        bw.masterPort = master;
        bw.bwVal = 76000000*8;
        bw.usagePercent = 100;
        bw.usageType = MMPM_BW_USAGE_LPASS_DSP; //default

        sts = Test_RequestBandwidth(clientId, &bw, 1, NULL);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            retSts = MMPM_STATUS_FAILED;
        }

        sts = Test_ReleaseResource(clientId, MMPM_RSC_ID_GENERIC_BW);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            retSts = MMPM_STATUS_FAILED;
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
 * @ingroup BW Test
 * @test cdsp_bw_01
 * @brief Req-Rel, all core/instance combination
 */
#define CDSP_BW_01_NAME "cdsp_bw_01"
#define CDSP_BW_01_DETAILS "test all q6 -> ddr bw votes"

static void Test_CDSP_Q6_DDR_BW(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(CDSP_BW_01_NAME, CDSP_BW_01_DETAILS);

    sts = TestCDSPBandwidthReqRel(MMPM_CORE_ID_Q6_DSP,
                                  MMPM_BW_PORT_ID_Q6DSP_MASTER,
                                  MMPM_BW_PORT_ID_DDR_SLAVE);

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_Bandwidth, CDSP_BW_01_NAME, sts);
    return;
}


/**
 * @ingroup BW Test
 * @test cdsp_bw_01
 * @brief Req-Rel, all core/instance combination
 */
#define CDSP_BW_02_NAME "cdsp_bw_02"
#define CDSP_BW_02_DETAILS "test all HCP -> ddr bw votes"

static void Test_CDSP_HCP_DDR_BW(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(CDSP_BW_02_NAME, CDSP_BW_02_DETAILS);

    sts = TestCDSPBandwidthReqRel(MMPM_CORE_ID_COMPUTE_HCP,
                                  MMPM_BW_PORT_ID_VAPSS_HCP_MASTER,
                                  MMPM_BW_PORT_ID_DDR_SLAVE);

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_Bandwidth, CDSP_BW_02_NAME, sts);
    return;
}


/**
 * @ingroup BW Test
 * @test cdsp_bw_01
 * @brief Req-Rel, all core/instance combination
 */
#define CDSP_BW_03_NAME "cdsp_bw_03"
#define CDSP_BW_03_DETAILS "test all DMA -> ddr bw votes"

static void Test_CDSP_DMA_DDR_BW(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(CDSP_BW_03_NAME, CDSP_BW_03_DETAILS);

    sts = TestCDSPBandwidthReqRel(MMPM_CORE_ID_COMPUTE_DMA,
                                  MMPM_BW_PORT_ID_VAPSS_DMA_MASTER,
                                  MMPM_BW_PORT_ID_DDR_SLAVE);

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_Bandwidth, CDSP_BW_03_NAME, sts);
    return;
}



/**
 * @ingroup BW Test
 * @test cdsp_bw_01
 * @brief Req-Rel, all core/instance combination
 */
#define CDSP_BW_04_NAME "cdsp_bw_04"
#define CDSP_BW_04_DETAILS "test all DMA -> TCM bw votes"

static void Test_CDSP_DMA_TCM_BW(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(CDSP_BW_04_NAME, CDSP_BW_04_DETAILS);

    sts = TestCDSPBandwidthReqRel(MMPM_CORE_ID_COMPUTE_DMA,
                                  MMPM_BW_PORT_ID_VAPSS_DMA_MASTER,
                                  MMPM_BW_PORT_ID_CDSP_TCM_SLAVE);

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_Bandwidth, CDSP_BW_04_NAME, sts);
    return;
}

/* bandwithd tests for CDSPPM */
void Test_CDSP_Bandwidth(void )
{
    Test_CDSP_Q6_DDR_BW();
    Test_CDSP_HCP_DDR_BW();
    Test_CDSP_DMA_DDR_BW();
    Test_CDSP_DMA_TCM_BW();
}

//SLPI BW Tests



static MMPM_STATUS TestSlpiBandwidthReqRel( MmpmCoreIdType   coreId,
                                            MmpmBwPortIdType master,
                                            MmpmBwPortIdType slave )
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    uint32 clientId = 0;
    BwReqTestType bw;

    clientId = Test_Register(coreId, MMPM_CORE_INSTANCE_0, "Slpi_BW");
    if(clientId)
    {

        bw.slavePort = slave;
        bw.masterPort = master;
        bw.bwVal = 76000000*8;
        bw.usagePercent = 100;
        bw.usageType = MMPM_BW_USAGE_LPASS_DSP; //default

        sts = Test_RequestBandwidth(clientId, &bw, 1, NULL);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            retSts = MMPM_STATUS_FAILED;
        }

        sts = Test_ReleaseResource(clientId, MMPM_RSC_ID_GENERIC_BW_EXT);
        if(MMPM_STATUS_SUCCESS != sts)
        {
            retSts = MMPM_STATUS_FAILED;
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
 * @ingroup BW Test
 * @test slpi_bw_01
 * @brief Req-Rel, all core/instance combination
 */
#define SLPI_BW_01_NAME    "slpi_bw_01"
#define SLPI_BW_01_DETAILS "test all q6 -> ddr bw votes"

static void Test_Slpi_Q6_DDR_BW(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(SLPI_BW_01_NAME, SLPI_BW_01_DETAILS);

    sts = TestSlpiBandwidthReqRel(MMPM_CORE_ID_Q6_DSP,
                                  MMPM_BW_PORT_ID_Q6DSP_MASTER,
                                  MMPM_BW_PORT_ID_DDR_SLAVE);

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_Bandwidth, SLPI_BW_01_NAME, sts);
    return;
}


/**
 * @ingroup BW Test
 * @test slpi_bw_01
 * @brief Req-Rel, all core/instance combination
 */
#define SLPI_BW_02_NAME "slpi_bw_02"
#define SLPI_BW_02_DETAILS "test all Qup -> ddr bw votes"

static void Test_Slpi_Qup_DDR_BW(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(SLPI_BW_02_NAME, SLPI_BW_02_DETAILS);

    sts = TestSlpiBandwidthReqRel(MMPM_CORE_ID_SLPI_QUP,
                                  MMPM_BW_PORT_ID_SLPI_QUP_MASTER,
                                  MMPM_BW_PORT_ID_DDR_SLAVE);

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_Bandwidth, SLPI_BW_02_NAME, sts);
    return;
}


/**
 * @ingroup BW Test
 * @test slpi_bw_01
 * @brief Req-Rel, all core/instance combination
 */
#define SLPI_BW_03_NAME "slpi_bw_03"
#define SLPI_BW_03_DETAILS "test all SDC -> Pram bw votes"

static void Test_Slpi_Sdc_Pram_BW(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(SLPI_BW_03_NAME, SLPI_BW_03_DETAILS);

    sts = TestCDSPBandwidthReqRel(MMPM_CORE_ID_SLPI_SDC,
                                  MMPM_BW_PORT_ID_SLPI_SDC_MASTER,
                                  MMPM_BW_PORT_ID_SLPI_SRAM_SLAVE);

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_Bandwidth, SLPI_BW_03_NAME, sts);
    return;
}


/**
 * @ingroup BW Test
 * @test slpi_bw_01
 * @brief Req-Rel, all core/instance combination
 */
#define SLPI_BW_04_NAME    "slpi_bw_04"
#define SLPI_BW_04_DETAILS "test all Qup -> Pram bw votes"

static void Test_Slpi_Qup_Pram_BW(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(SLPI_BW_04_NAME, SLPI_BW_04_DETAILS);

    sts = TestCDSPBandwidthReqRel(MMPM_CORE_ID_SLPI_SDC,
                                  MMPM_BW_PORT_ID_SLPI_QUP_MASTER,
                                  MMPM_BW_PORT_ID_SLPI_SRAM_SLAVE);

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_Bandwidth, SLPI_BW_04_NAME, sts);
    return;
}


/**
 * @ingroup BW Test
 * @test slpi_bw_05
 * @brief Req-Rel, all core/instance combination
 */
#define SLPI_BW_05_NAME    "slpi_bw_05"
#define SLPI_BW_05_DETAILS "test Q6 -> Pram bw votes"

static void Test_Slpi_Q6_Pram_BW(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(SLPI_BW_05_NAME, SLPI_BW_05_DETAILS);

    sts = TestCDSPBandwidthReqRel(MMPM_CORE_ID_Q6_DSP,
                                  MMPM_BW_PORT_ID_Q6DSP_MASTER,
                                  MMPM_BW_PORT_ID_SLPI_SRAM_SLAVE);

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_Bandwidth, SLPI_BW_05_NAME, sts);
    return;
}

/**
 * @ingroup BW Test
 * @test slpi_bw_06
 * @brief Req-Rel, all core/instance combination
 */
#define SLPI_BW_06_NAME    "slpi_bw_06"
#define SLPI_BW_06_DETAILS "test SDC -> DDR bw votes"

static void Test_Slpi_Sdc_DDR_BW(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(SLPI_BW_06_NAME, SLPI_BW_06_DETAILS);

    sts = TestCDSPBandwidthReqRel(MMPM_CORE_ID_SLPI_SDC,
                                  MMPM_BW_PORT_ID_SLPI_SDC_MASTER,
                                  MMPM_BW_PORT_ID_DDR_SLAVE);

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_Bandwidth, SLPI_BW_06_NAME, sts);
    return;
}

/* bandwithd tests for SCPM */
void Test_Slpi_Bandwidth(void )
{
    Test_Slpi_Q6_DDR_BW();
    Test_Slpi_Qup_DDR_BW();
    Test_Slpi_Sdc_Pram_BW();
    Test_Slpi_Qup_Pram_BW();
    Test_Slpi_Q6_Pram_BW();
	Test_Slpi_Sdc_DDR_BW();
}
