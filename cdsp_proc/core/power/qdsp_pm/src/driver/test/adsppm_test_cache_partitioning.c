/*
* Copyright (c) 2015 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
@file: adsppm_test_cache_partitioning.c
@brief: Implementation of ADSPPM test module of cache partitioning.

$Header: //components/rel/core.qdsp6/2.1.c4/power/qdsp_pm/src/driver/test/adsppm_test_cache_partitioning.c#1 $
*/

#include "mmpm.h"
#include "adsppm_test.h"
#include "adsppm_test_param.h"
#include "adsppm_test_utils.h"
#include "adsppm_test_asic_info.h"
#include "qurt_error.h"
#include "qurt_sem.h"
#include "qurt_thread.h"
#include "qurt_timer.h"
#include "stdlib.h"

static boolean checkClass(MmpmClientClassType actual,
    MmpmClientClassType expected)
{
    // Check if 'actual' has the 'expected' client classes.  Can't use '=='
    // since we aren't preventing other clients from concurrently setting their
    // class and causing 'actual' to change
    return ((actual & expected) == expected);
}

static MMPM_STATUS setLineLocking(uint32 clientId, void* startAddr, uint32 size,
    uint32 throttleBlockSize, uint32 throttlePauseUs)
{
    MmpmParameterConfigType paramConfig;
    MmpmL2CacheLineLockParameterType lockParam;
    memset(&lockParam, 0, sizeof(MmpmL2CacheLineLockParameterType));
    lockParam.startAddress = startAddr;
    lockParam.size = size;
    lockParam.throttleBlockSize = throttleBlockSize;
    lockParam.throttlePauseUs = throttlePauseUs;
    paramConfig.paramId = MMPM_PARAM_ID_L2_CACHE_LINE_LOCK;
    paramConfig.pParamConfig = &lockParam;
    return MMPM_SetParameter(clientId, &paramConfig);
}

static MMPM_STATUS getAggregateClass(MmpmClientClassType* pClientClass)
{
    MMPM_STATUS status;
    MmpmInfoDataType infoData;
    infoData.infoId = MMPM_INFO_ID_AGGREGATE_CLIENT_CLASS;
    infoData.coreId = MMPM_CORE_ID_LPASS_DML;
    infoData.instanceId = MMPM_CORE_INSTANCE_0;
    infoData.info.aggregateClientClass = MMPM_UNKNOWN_CLIENT_CLASS; // output
    status = MMPM_GetInfo(&infoData);
    if(pClientClass)
        *pClientClass = infoData.info.aggregateClientClass;
    return status;
}

#define CACHE_PARTITIONING_01_NAME    "cache_partitioning_01"
#define CACHE_PARTITIONING_01_DETAILS "Single client classes"

static MMPM_STATUS TestSingleClientClasses(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    uint32 clientId;
    BwReqTestType bwReq;
    MmpmClientClassType clientClass;

    clientId = Test_Register(MMPM_CORE_ID_LPASS_DML, MMPM_CORE_INSTANCE_0,
        CACHE_PARTITIONING_01_NAME);

    if(clientId == 0)
        goto TestSingleClientClasses_error;

    bwReq.masterPort = MMPM_BW_PORT_ID_ADSP_MASTER;
    bwReq.slavePort = MMPM_BW_PORT_ID_DML_SLAVE;
    bwReq.bwVal = 10;
    bwReq.usagePercent = 100;
    bwReq.usageType = MMPM_BW_USAGE_LPASS_DSP;

    // Set audio class
    sts = Test_SetClientClass(clientId, MMPM_AUDIO_CLIENT_CLASS);
    if(MMPM_STATUS_SUCCESS != sts)
        goto TestSingleClientClasses_error;

    // Request BW to make this client active
    sts = Test_RequestBandwidth(clientId, &bwReq, 1, NULL);
    if(MMPM_STATUS_SUCCESS != sts)
        goto TestSingleClientClasses_error;

    sts = getAggregateClass(&clientClass);
    if(MMPM_STATUS_SUCCESS != sts)
        goto TestSingleClientClasses_error;

    if(!checkClass(clientClass, MMPM_AUDIO_CLIENT_CLASS))
    {
        ADSPPMTEST_LOG_PRINTF_0(ADSPPMTEST_LOG_LEVEL_ERROR,
            "[ERROR] unexpected aggregate client class returned from query");
        goto TestSingleClientClasses_error;
    }

    // Set voice class

    sts = Test_SetClientClass(clientId, MMPM_VOICE_CLIENT_CLASS);
    if(MMPM_STATUS_SUCCESS != sts)
        goto TestSingleClientClasses_error;

    sts = getAggregateClass(&clientClass);
    if(MMPM_STATUS_SUCCESS != sts)
        goto TestSingleClientClasses_error;

    if(!checkClass(clientClass, MMPM_VOICE_CLIENT_CLASS))
    {
        ADSPPMTEST_LOG_PRINTF_0(ADSPPMTEST_LOG_LEVEL_ERROR,
            "[ERROR] unexpected aggregate client class returned from query");
        goto TestSingleClientClasses_error;
    }

    // Set compute class

    sts = Test_SetClientClass(clientId, MMPM_COMPUTE_CLIENT_CLASS);
    if(MMPM_STATUS_SUCCESS != sts)
        goto TestSingleClientClasses_error;

    sts = getAggregateClass(&clientClass);
    if(MMPM_STATUS_SUCCESS != sts)
        goto TestSingleClientClasses_error;

    if(!checkClass(clientClass, MMPM_COMPUTE_CLIENT_CLASS))
    {
        ADSPPMTEST_LOG_PRINTF_0(ADSPPMTEST_LOG_LEVEL_ERROR,
            "[ERROR] unexpected aggregate client class returned from query");
        goto TestSingleClientClasses_error;
    }

    // Set streaming class

    sts = Test_SetClientClass(clientId, MMPM_STREAMING_1HVX_CLIENT_CLASS);
    if(MMPM_STATUS_SUCCESS != sts)
        goto TestSingleClientClasses_error;

    sts = getAggregateClass(&clientClass);
    if(MMPM_STATUS_SUCCESS != sts)
        goto TestSingleClientClasses_error;

    if(!checkClass(clientClass, MMPM_STREAMING_1HVX_CLIENT_CLASS))
    {
        ADSPPMTEST_LOG_PRINTF_0(ADSPPMTEST_LOG_LEVEL_ERROR,
            "[ERROR] unexpected aggregate client class returned from query");
        goto TestSingleClientClasses_error;
    }

    sts = Test_Deregister(clientId);
    if(MMPM_STATUS_SUCCESS != sts)
        goto TestSingleClientClasses_error;

    return retSts;

TestSingleClientClasses_error:

    if(clientId != 0)
        Test_Deregister(clientId);

    return MMPM_STATUS_FAILED;

}

static MMPM_STATUS Test_CachePartitioning_01(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(CACHE_PARTITIONING_01_NAME,
        CACHE_PARTITIONING_01_DETAILS);

    sts = TestSingleClientClasses();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_CachePartitioning,
        CACHE_PARTITIONING_01_NAME, sts);
    return sts;
}

#define CACHE_PARTITIONING_02_NAME    "cache_partitioning_02"
#define CACHE_PARTITIONING_02_DETAILS "Concurrent client classes"

static MMPM_STATUS TestConcurrentClientClasses(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    const uint32 N = 4;
    uint32 clientId[N];
    BwReqTestType bwReq;
    MmpmClientClassType clientClass[N];
    uint32 i;

    // Array of unique classes
    clientClass[0] = MMPM_AUDIO_CLIENT_CLASS;
    clientClass[1] = MMPM_VOICE_CLIENT_CLASS;
    clientClass[2] = MMPM_STREAMING_1HVX_CLIENT_CLASS;
    clientClass[3] = MMPM_COMPUTE_CLIENT_CLASS;

    // Setup clients
    for(i = 0; i < N; i++)
    {
        clientId[i]= Test_Register(MMPM_CORE_ID_LPASS_DML, MMPM_CORE_INSTANCE_0,
            CACHE_PARTITIONING_02_NAME);
        if(clientId[i] == 0)
            goto TestConcurrentClientClasses_error;
        // Set class
        sts = Test_SetClientClass(clientId[i], clientClass[i]);
        if(MMPM_STATUS_SUCCESS != sts)
            goto TestConcurrentClientClasses_error;
    }

    // Setup BW request
    bwReq.masterPort = MMPM_BW_PORT_ID_ADSP_MASTER;
    bwReq.slavePort = MMPM_BW_PORT_ID_DML_SLAVE;
    bwReq.bwVal = 10;
    bwReq.usagePercent = 100;
    bwReq.usageType = MMPM_BW_USAGE_LPASS_DSP;

    for(i = 0; i < N; i++)
    {
        MmpmClientClassType aggClass = 0;
        MmpmClientClassType queriedClass = 0;
        
        // Request BW to make client active
        sts = Test_RequestBandwidth(clientId[i], &bwReq, 1, NULL);
        if(MMPM_STATUS_SUCCESS != sts)
            goto TestConcurrentClientClasses_error;

        // Calc expected aggregate class
        for(int ii = 0; ii <= i; ii++)
            aggClass |= clientClass[ii];

        // Check aggregate class
        sts = getAggregateClass(&queriedClass);
        if(MMPM_STATUS_SUCCESS != sts)
            goto TestConcurrentClientClasses_error;

        if(!checkClass(queriedClass, aggClass))
        {
            ADSPPMTEST_LOG_PRINTF_2(ADSPPMTEST_LOG_LEVEL_ERROR,
                "[ERROR] unexpected aggregate client class returned from "
                "query: got %d, expected %d", queriedClass, aggClass);
            goto TestConcurrentClientClasses_error;
        }
    }

    // Cleanup
    for(i = 0; i < N; i++)
    {
        sts = Test_Deregister(clientId[i]);
        if(MMPM_STATUS_SUCCESS != sts)
            goto TestConcurrentClientClasses_error;
    }

    return retSts;

TestConcurrentClientClasses_error:

    for(i = 0; i < N; i++)
    {
        if(clientId[i] != 0)
            Test_Deregister(clientId[i]);
    }

    return MMPM_STATUS_FAILED;

}

static MMPM_STATUS Test_CachePartitioning_02(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(CACHE_PARTITIONING_02_NAME,
        CACHE_PARTITIONING_02_DETAILS);

    sts = TestConcurrentClientClasses();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_CachePartitioning,
        CACHE_PARTITIONING_02_NAME, sts);
    return sts;
}

#define CACHE_PARTITIONING_03_NAME    "cache_partitioning_03"
#define CACHE_PARTITIONING_03_DETAILS "Deactivated client class"

static MMPM_STATUS TestDeactivatedClientClasses(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    uint32 clientId;
    MmpmClientClassType clientClass;

    clientId = Test_Register(MMPM_CORE_ID_LPASS_DML, MMPM_CORE_INSTANCE_0,
        CACHE_PARTITIONING_03_NAME);

    if(clientId == 0)
        goto TestDeactivatedClientClasses_error;

    // Set compute class
    sts = Test_SetClientClass(clientId, MMPM_COMPUTE_CLIENT_CLASS);
    if(MMPM_STATUS_SUCCESS != sts)
        goto TestDeactivatedClientClasses_error;

    // Request MIPS to make this client active
    sts = Test_RequestMips(clientId, 1000 /* MIPS */);
    if(MMPM_STATUS_SUCCESS != sts)
        goto TestDeactivatedClientClasses_error;

    // Check the aggregate client class
    sts = getAggregateClass(&clientClass);
    if(MMPM_STATUS_SUCCESS != sts)
        goto TestDeactivatedClientClasses_error;

    if(!checkClass(clientClass, MMPM_COMPUTE_CLIENT_CLASS))
    {
        ADSPPMTEST_LOG_PRINTF_0(ADSPPMTEST_LOG_LEVEL_ERROR,
            "[ERROR] unexpected aggregate client class returned from query");
        goto TestDeactivatedClientClasses_error;
    }

    // Remove MIPS request and check aggregate class again
    sts = Test_ReleaseResource(clientId, MMPM_RSC_ID_MIPS_EXT);
    if(MMPM_STATUS_SUCCESS != sts)
        goto TestDeactivatedClientClasses_error;

    sts = getAggregateClass(&clientClass);
    if(MMPM_STATUS_SUCCESS != sts)
        goto TestDeactivatedClientClasses_error;

    // Expecting compute class to be inactive; may not be true if real clients
    // are active as this test is run
    if(checkClass(clientClass, MMPM_COMPUTE_CLIENT_CLASS))
    {
        ADSPPMTEST_LOG_PRINTF_0(ADSPPMTEST_LOG_LEVEL_ERROR,
            "[ERROR] unexpected aggregate client class returned from query");
        goto TestDeactivatedClientClasses_error;
    }

    // Cleanup
    sts = Test_Deregister(clientId);
    if(MMPM_STATUS_SUCCESS != sts)
        goto TestDeactivatedClientClasses_error;

    return retSts;

TestDeactivatedClientClasses_error:

    if(clientId != 0)
        Test_Deregister(clientId);

    return MMPM_STATUS_FAILED;

}

static MMPM_STATUS Test_CachePartitioning_03(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(CACHE_PARTITIONING_03_NAME,
        CACHE_PARTITIONING_03_DETAILS);

    sts = TestDeactivatedClientClasses();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_CachePartitioning,
        CACHE_PARTITIONING_03_NAME, sts);
    return sts;
}

#define CACHE_PARTITIONING_04_NAME    "cache_partitioning_04"
#define CACHE_PARTITIONING_04_DETAILS "Enable line locking"

static MMPM_STATUS TestLineLocking(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    uint32 clientId;
    const uint32 dummyBufferSize = 64;
    char dummyBuffer[dummyBufferSize];
    void* dummyBufferAligned;
    uint32 blockSize = 0, pauseUs = 0;

    clientId = Test_Register(MMPM_CORE_ID_LPASS_DML, MMPM_CORE_INSTANCE_0,
        CACHE_PARTITIONING_04_NAME);

    if(clientId == 0)
        goto TestLineLocking_error;

    // Align dummy buffer to 32-byte boundary; this is a requirement for
    // underlying qurt API.
    dummyBufferAligned = (void*) (((uint32) dummyBuffer + 31) & ~0x1f);

    // Enable line locking
    sts = setLineLocking(clientId, dummyBufferAligned, 32, blockSize, pauseUs);
    if(MMPM_STATUS_SUCCESS != sts)
        goto TestLineLocking_error;

    // Disable line locking w/ 0 address
    sts = setLineLocking(clientId, 0, 32, blockSize, pauseUs);
    if(MMPM_STATUS_SUCCESS != sts)
        goto TestLineLocking_error;

    // Enable line locking
    sts = setLineLocking(clientId, dummyBufferAligned, 32, blockSize, pauseUs);
    if(MMPM_STATUS_SUCCESS != sts)
        goto TestLineLocking_error;

    // Disable line locking w/ 0 size
    sts = setLineLocking(clientId, dummyBufferAligned, 0, blockSize, pauseUs);
    if(MMPM_STATUS_SUCCESS != sts)
        goto TestLineLocking_error;

    // Enable line locking.  To be automatically unlocked upon deregister
    sts = setLineLocking(clientId, dummyBufferAligned, 32, blockSize, pauseUs);
    if(MMPM_STATUS_SUCCESS != sts)
        goto TestLineLocking_error;

    // Cleanup
    sts = Test_Deregister(clientId);
    if(MMPM_STATUS_SUCCESS != sts)
        goto TestLineLocking_error;

    return retSts;

TestLineLocking_error:

    if(clientId != 0)
        Test_Deregister(clientId);

    return MMPM_STATUS_FAILED;

}

static MMPM_STATUS Test_CachePartitioning_04(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(CACHE_PARTITIONING_04_NAME,
        CACHE_PARTITIONING_04_DETAILS);

    sts = TestLineLocking();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_CachePartitioning,
        CACHE_PARTITIONING_04_NAME, sts);
    return sts;
}

#define CACHE_PARTITIONING_05_NAME    "cache_partitioning_05"
#define CACHE_PARTITIONING_05_DETAILS "Deactivated client class using 0 MIPS vote"

static MMPM_STATUS TestZeroMipsDeactivatedClientClasses(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    uint32 clientId;
    MmpmClientClassType clientClass;

    clientId = Test_Register(MMPM_CORE_ID_LPASS_DML, MMPM_CORE_INSTANCE_0,
        CACHE_PARTITIONING_05_NAME);

    if(clientId == 0)
        goto TestZeroMipsDeactivatedClientClasses_error;

    // Set compute class
    sts = Test_SetClientClass(clientId, MMPM_COMPUTE_CLIENT_CLASS);
    if(MMPM_STATUS_SUCCESS != sts)
        goto TestZeroMipsDeactivatedClientClasses_error;

    // Request MIPS to make this client active
    sts = Test_RequestMips(clientId, 1000 /* MIPS */);
    if(MMPM_STATUS_SUCCESS != sts)
        goto TestZeroMipsDeactivatedClientClasses_error;

    // Check the aggregate client class
    sts = getAggregateClass(&clientClass);
    if(MMPM_STATUS_SUCCESS != sts)
        goto TestZeroMipsDeactivatedClientClasses_error;

    if(!checkClass(clientClass, MMPM_COMPUTE_CLIENT_CLASS))
    {
        ADSPPMTEST_LOG_PRINTF_0(ADSPPMTEST_LOG_LEVEL_ERROR,
            "[ERROR] unexpected aggregate client class returned from query");
        goto TestZeroMipsDeactivatedClientClasses_error;
    }

    // Request 0 MIPS and check aggregate class again
    sts = Test_RequestMips(clientId, 0);
    if(MMPM_STATUS_SUCCESS != sts)
        goto TestZeroMipsDeactivatedClientClasses_error;

    sts = getAggregateClass(&clientClass);
    if(MMPM_STATUS_SUCCESS != sts)
        goto TestZeroMipsDeactivatedClientClasses_error;

    // Expecting compute class to be inactive; may not be true if real clients
    // are active as this test is run
    if(checkClass(clientClass, MMPM_COMPUTE_CLIENT_CLASS))
    {
        ADSPPMTEST_LOG_PRINTF_0(ADSPPMTEST_LOG_LEVEL_ERROR,
            "[ERROR] unexpected aggregate client class returned from query");
        goto TestZeroMipsDeactivatedClientClasses_error;
    }

    // Cleanup
    sts = Test_Deregister(clientId);
    if(MMPM_STATUS_SUCCESS != sts)
        goto TestZeroMipsDeactivatedClientClasses_error;

    return retSts;

TestZeroMipsDeactivatedClientClasses_error:

    if(clientId != 0)
        Test_Deregister(clientId);

    return MMPM_STATUS_FAILED;

}

static MMPM_STATUS Test_CachePartitioning_05(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(CACHE_PARTITIONING_05_NAME,
        CACHE_PARTITIONING_05_DETAILS);

    sts = TestZeroMipsDeactivatedClientClasses();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_CachePartitioning,
        CACHE_PARTITIONING_05_NAME, sts);
    return sts;
}

#define CACHE_PARTITIONING_06_NAME    "cache_partitioning_06"
#define CACHE_PARTITIONING_06_DETAILS "Deactivated client class using 0 BW vote"

static MMPM_STATUS TestZeroBwDeactivatedClientClasses(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    uint32 clientId;
    MmpmClientClassType clientClass;
    BwReqTestType bwReq;

    clientId = Test_Register(MMPM_CORE_ID_LPASS_DML, MMPM_CORE_INSTANCE_0,
        CACHE_PARTITIONING_06_NAME);

    if(clientId == 0)
        goto TestZeroBwDeactivatedClientClasses_error;

    // Set compute class
    sts = Test_SetClientClass(clientId, MMPM_COMPUTE_CLIENT_CLASS);
    if(MMPM_STATUS_SUCCESS != sts)
        goto TestZeroBwDeactivatedClientClasses_error;

    // Request BW to make this client active
    bwReq.masterPort = MMPM_BW_PORT_ID_ADSP_MASTER;
    bwReq.slavePort = MMPM_BW_PORT_ID_DML_SLAVE;
    bwReq.bwVal = 10;
    bwReq.usagePercent = 100;
    bwReq.usageType = MMPM_BW_USAGE_LPASS_DSP;
    sts = Test_RequestBandwidth(clientId, &bwReq, 1, NULL);
    if(MMPM_STATUS_SUCCESS != sts)
        goto TestZeroBwDeactivatedClientClasses_error;

    // Check the aggregate client class
    sts = getAggregateClass(&clientClass);
    if(MMPM_STATUS_SUCCESS != sts)
        goto TestZeroBwDeactivatedClientClasses_error;

    if(!checkClass(clientClass, MMPM_COMPUTE_CLIENT_CLASS))
    {
        ADSPPMTEST_LOG_PRINTF_0(ADSPPMTEST_LOG_LEVEL_ERROR,
            "[ERROR] unexpected aggregate client class returned from query");
        goto TestZeroBwDeactivatedClientClasses_error;
    }

    // Request 0 BW and check aggregate class again
    bwReq.masterPort = MMPM_BW_PORT_ID_ADSP_MASTER;
    bwReq.slavePort = MMPM_BW_PORT_ID_DML_SLAVE;
    bwReq.bwVal = 0;
    bwReq.usagePercent = 100;
    bwReq.usageType = MMPM_BW_USAGE_LPASS_DSP;
    sts = Test_RequestBandwidth(clientId, &bwReq, 1, NULL);
    if(MMPM_STATUS_SUCCESS != sts)
        goto TestZeroBwDeactivatedClientClasses_error;

    sts = getAggregateClass(&clientClass);
    if(MMPM_STATUS_SUCCESS != sts)
        goto TestZeroBwDeactivatedClientClasses_error;

    // Expecting compute class to be inactive; may not be true if real clients
    // are active as this test is run
    if(checkClass(clientClass, MMPM_COMPUTE_CLIENT_CLASS))
    {
        ADSPPMTEST_LOG_PRINTF_0(ADSPPMTEST_LOG_LEVEL_ERROR,
            "[ERROR] unexpected aggregate client class returned from query");
        goto TestZeroBwDeactivatedClientClasses_error;
    }

    // Cleanup
    sts = Test_Deregister(clientId);
    if(MMPM_STATUS_SUCCESS != sts)
        goto TestZeroBwDeactivatedClientClasses_error;

    return retSts;

TestZeroBwDeactivatedClientClasses_error:

    if(clientId != 0)
        Test_Deregister(clientId);

    return MMPM_STATUS_FAILED;

}

static MMPM_STATUS Test_CachePartitioning_06(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(CACHE_PARTITIONING_06_NAME,
        CACHE_PARTITIONING_06_DETAILS);

    sts = TestZeroBwDeactivatedClientClasses();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_CachePartitioning,
        CACHE_PARTITIONING_06_NAME, sts);
    return sts;
}

#define CACHE_PARTITIONING_07_NAME    "cache_partitioning_07"
#define CACHE_PARTITIONING_07_DETAILS "Deactivated client class using BW and reg programming vote"

static MMPM_STATUS TestRegProgAndBwDeactivatedClientClasses(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    uint32 clientId;
    MmpmClientClassType clientClass;
    BwReqTestType bwReq;

    clientId = Test_Register(MMPM_CORE_ID_LPASS_DML, MMPM_CORE_INSTANCE_0,
        CACHE_PARTITIONING_07_NAME);

    if(clientId == 0)
        goto TestRegProgAndBwDeactivatedClientClasses_error;

    // Set compute class
    sts = Test_SetClientClass(clientId, MMPM_COMPUTE_CLIENT_CLASS);
    if(MMPM_STATUS_SUCCESS != sts)
        goto TestRegProgAndBwDeactivatedClientClasses_error;

    // Request BW to make this client active
    bwReq.masterPort = MMPM_BW_PORT_ID_ADSP_MASTER;
    bwReq.slavePort = MMPM_BW_PORT_ID_DML_SLAVE;
    bwReq.bwVal = 10;
    bwReq.usagePercent = 100;
    bwReq.usageType = MMPM_BW_USAGE_LPASS_DSP;
    sts = Test_RequestBandwidth(clientId, &bwReq, 1, NULL);
    if(MMPM_STATUS_SUCCESS != sts)
        goto TestRegProgAndBwDeactivatedClientClasses_error;

    // Check the aggregate client class
    sts = getAggregateClass(&clientClass);
    if(MMPM_STATUS_SUCCESS != sts)
        goto TestRegProgAndBwDeactivatedClientClasses_error;

    if(!checkClass(clientClass, MMPM_COMPUTE_CLIENT_CLASS))
    {
        ADSPPMTEST_LOG_PRINTF_0(ADSPPMTEST_LOG_LEVEL_ERROR,
            "[ERROR] unexpected aggregate client class returned from query");
        goto TestRegProgAndBwDeactivatedClientClasses_error;
    }

    // Request register programming
    sts = Test_RequestRegProg(clientId, MMPM_REG_PROG_NORM);
    if(MMPM_STATUS_SUCCESS != sts)
        goto TestRegProgAndBwDeactivatedClientClasses_error;

    sts = getAggregateClass(&clientClass);
    if(MMPM_STATUS_SUCCESS != sts)
        goto TestRegProgAndBwDeactivatedClientClasses_error;

    // Class should still be compute
    if(!checkClass(clientClass, MMPM_COMPUTE_CLIENT_CLASS))
    {
        ADSPPMTEST_LOG_PRINTF_0(ADSPPMTEST_LOG_LEVEL_ERROR,
            "[ERROR] unexpected aggregate client class returned from query");
        goto TestRegProgAndBwDeactivatedClientClasses_error;
    }

    // Release BW request
    sts = Test_ReleaseResource(clientId, MMPM_RSC_ID_GENERIC_BW);
    if(MMPM_STATUS_SUCCESS != sts)
        goto TestRegProgAndBwDeactivatedClientClasses_error;

    sts = getAggregateClass(&clientClass);
    if(MMPM_STATUS_SUCCESS != sts)
        goto TestRegProgAndBwDeactivatedClientClasses_error;

    // Class should still be compute since reg prog request is still active
    if(!checkClass(clientClass, MMPM_COMPUTE_CLIENT_CLASS))
    {
        ADSPPMTEST_LOG_PRINTF_0(ADSPPMTEST_LOG_LEVEL_ERROR,
            "[ERROR] unexpected aggregate client class returned from query");
        goto TestRegProgAndBwDeactivatedClientClasses_error;
    }

    // Release reg programming request; now client should be considered inactive
    sts = Test_ReleaseResource(clientId, MMPM_RSC_ID_REG_PROG);
    if(MMPM_STATUS_SUCCESS != sts)
        goto TestRegProgAndBwDeactivatedClientClasses_error;

    sts = getAggregateClass(&clientClass);
    if(MMPM_STATUS_SUCCESS != sts)
        goto TestRegProgAndBwDeactivatedClientClasses_error;

    // Expecting compute class to be inactive; may not be true if real clients
    // are active as this test is run
    if(checkClass(clientClass, MMPM_COMPUTE_CLIENT_CLASS))
    {
        ADSPPMTEST_LOG_PRINTF_0(ADSPPMTEST_LOG_LEVEL_ERROR,
            "[ERROR] unexpected aggregate client class returned from query");
        goto TestRegProgAndBwDeactivatedClientClasses_error;
    }

    // Cleanup
    sts = Test_Deregister(clientId);
    if(MMPM_STATUS_SUCCESS != sts)
        goto TestRegProgAndBwDeactivatedClientClasses_error;

    return retSts;

TestRegProgAndBwDeactivatedClientClasses_error:

    if(clientId != 0)
        Test_Deregister(clientId);

    return MMPM_STATUS_FAILED;

}

static MMPM_STATUS Test_CachePartitioning_07(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(CACHE_PARTITIONING_07_NAME,
        CACHE_PARTITIONING_07_DETAILS);

    sts = TestRegProgAndBwDeactivatedClientClasses();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_CachePartitioning,
        CACHE_PARTITIONING_07_NAME, sts);
    return sts;
}

#define CACHE_PARTITIONING_08_NAME    "cache_partitioning_08"
#define CACHE_PARTITIONING_08_DETAILS "Enable line locking with throttling"

// Shared data between worker thread and main thread for test 8
typedef struct
{
    MMPM_STATUS status;
    qurt_sem_t sem; // signals completion
} test8_data_t;
static test8_data_t gTest8Data;

// Entry point for thread that does line locking with throttling
static void LineLockWorker(void* arg)
{

    test8_data_t* testData = (test8_data_t*) arg;
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    uint32 clientId;
    const uint32 dummyBufferSize = 256;
    // Extra allocated to deal with alignment later
    char dummyBuffer[2 * dummyBufferSize];
    void* dummyBufferAligned;
    uint32 blockSize = 32;
    uint32 pauseUs = 4000; // 4 ms

    clientId = Test_Register(MMPM_CORE_ID_LPASS_DML, MMPM_CORE_INSTANCE_0,
        CACHE_PARTITIONING_08_NAME " (worker)");

    if(clientId == 0)
        goto LineLockWorker_error;

    // Align dummy buffer to 256-byte boundary; this is a requirement for
    // underlying qurt API.  Address must be aligned to size of locked region.
    dummyBufferAligned = (void*) (((uint32) dummyBuffer + dummyBufferSize - 1)
        & ~0xff);

    // Enable line locking with throttling.  To be automatically unlocked upon
    // deregister.
    sts = setLineLocking(clientId, dummyBufferAligned, dummyBufferSize,
        blockSize, pauseUs);
    if(MMPM_STATUS_SUCCESS != sts)
        goto LineLockWorker_error;

    // Cleanup
    sts = Test_Deregister(clientId);
    if(MMPM_STATUS_SUCCESS != sts)
        goto LineLockWorker_error;

    testData->status = retSts;

    qurt_sem_up(&testData->sem);

    return;

LineLockWorker_error:

    if(clientId != 0)
        Test_Deregister(clientId);

    testData->status = MMPM_STATUS_FAILED;

    qurt_sem_up(&testData->sem);

    return;

};

static MMPM_STATUS TestLineLockingWithThrottling(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    uint32 clientId;
    test8_data_t* testData = (test8_data_t*) &gTest8Data;
    qurt_thread_t threadId;
    qurt_thread_attr_t threadAttr;
    void* myStack = NULL;
    const uint32 myStackSize = 2048;
    boolean threadLaunched = FALSE;

    BwReqTestType bwReq;

    // Test data setup
    qurt_sem_init(&testData->sem);
    qurt_sem_init_val(&testData->sem, 0);
    testData->status = MMPM_STATUS_FAILED;

    // Work thread setup

    myStack = malloc(myStackSize);
    if(myStack == NULL)
    {
        ADSPPMTEST_LOG_PRINTF_0(ADSPPMTEST_LOG_LEVEL_ERROR,
            "[ERROR] stack allocation failed");
        goto TestLineLockingWithThrottling_error;
    }
    qurt_thread_attr_init(&threadAttr);
    qurt_thread_attr_set_stack_addr(&threadAttr, myStack);
    qurt_thread_attr_set_stack_size(&threadAttr, myStackSize);

    clientId = Test_Register(MMPM_CORE_ID_LPASS_DML, MMPM_CORE_INSTANCE_0,
        CACHE_PARTITIONING_08_NAME);

    if(clientId == 0)
        goto TestLineLockingWithThrottling_error;

    // Populate BW request
    bwReq.masterPort = MMPM_BW_PORT_ID_ADSP_MASTER;
    bwReq.slavePort = MMPM_BW_PORT_ID_DML_SLAVE;
    bwReq.bwVal = 10;
    bwReq.usagePercent = 100;
    bwReq.usageType = MMPM_BW_USAGE_LPASS_DSP;

    // Set compute class
    sts = Test_SetClientClass(clientId, MMPM_COMPUTE_CLIENT_CLASS);
    if(MMPM_STATUS_SUCCESS != sts)
        goto TestLineLockingWithThrottling_error;

    // Request BW to make this client active
    // This should activate cache partitioning
    sts = Test_RequestBandwidth(clientId, &bwReq, 1, NULL);
    if(MMPM_STATUS_SUCCESS != sts)
        goto TestLineLockingWithThrottling_error;

    // Launch worker thread which does line locking with a separate client
    if(qurt_thread_create(&threadId, &threadAttr, LineLockWorker, testData) !=
        QURT_EOK)
    {
        ADSPPMTEST_LOG_PRINTF_0(ADSPPMTEST_LOG_LEVEL_ERROR,
            "[ERROR] thread creation failed");
        goto TestLineLockingWithThrottling_error;
    }
    else
    {
        threadLaunched = TRUE;
    }

    // Wait for spawned thread to active.  Within 2 ms its expected to be in
    // the middle of a throttled line locking request.
    qurt_timer_sleep(2 * 1000); // 2 ms

    // Release BW request
    // This should cause cache partitioning to go back to default.  Partition
    // update is expected to be deferred due to concurrent line locking
    // request.
    sts = Test_ReleaseResource(clientId, MMPM_RSC_ID_GENERIC_BW);
    if(MMPM_STATUS_SUCCESS != sts)
        goto TestLineLockingWithThrottling_error;

    sts = Test_Deregister(clientId);
    if(MMPM_STATUS_SUCCESS != sts)
        goto TestLineLockingWithThrottling_error;

    // Wait until completion of worker thread
    qurt_sem_down(&testData->sem);

    if(myStack != NULL)
        free(myStack);

    return testData->status;

TestLineLockingWithThrottling_error:

    if(threadLaunched)
    {
        // Wait until completion of worker thread
        qurt_sem_down(&testData->sem);
    }

    if(myStack != NULL)
        free(myStack);

    return MMPM_STATUS_FAILED;
}

static MMPM_STATUS Test_CachePartitioning_08(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(CACHE_PARTITIONING_08_NAME,
        CACHE_PARTITIONING_08_DETAILS);

    sts = TestLineLockingWithThrottling();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_CachePartitioning,
        CACHE_PARTITIONING_08_NAME, sts);
    return sts;
}


#define CACHE_PARTITIONING_09_NAME    "cache_partitioning_09"
#define CACHE_PARTITIONING_09_DETAILS "Concurrent audio/compute client class along with MPPS vote"

static MMPM_STATUS TestConcurrentClientClasseswithMPPSvote(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    const uint32 N = 2;
    uint32 clientId[N];
    BwReqTestType bwReq;
    MmpmClientClassType clientClass[N];
    uint32 i;
    MmpmMppsReqType mppsReq;

    mppsReq.mppsTotal = 200;
    // Don't care about floor clock for this test
    mppsReq.adspFloorClock = 0;

    // Array of unique classes
    clientClass[0] = MMPM_AUDIO_CLIENT_CLASS;
    clientClass[1] = MMPM_COMPUTE_CLIENT_CLASS;

    // Setup clients
    for(i = 0; i < N; i++)
    {
        clientId[i]= Test_Register(MMPM_CORE_ID_LPASS_DML, MMPM_CORE_INSTANCE_0,
            CACHE_PARTITIONING_09_NAME);
        if(clientId[i] == 0)
            goto TestConcurrentClientClasses_error;
        // Set class
        sts = Test_SetClientClass(clientId[i], clientClass[i]);
        if(MMPM_STATUS_SUCCESS != sts)
            goto TestConcurrentClientClasses_error;
    }

    // Setup BW request
    bwReq.masterPort = MMPM_BW_PORT_ID_ADSP_MASTER;
    bwReq.slavePort = MMPM_BW_PORT_ID_DML_SLAVE;
    bwReq.bwVal = 10;
    bwReq.usagePercent = 100;
    bwReq.usageType = MMPM_BW_USAGE_LPASS_DSP;

    for(i = 0; i < N; i++)
    {
        MmpmClientClassType aggClass = 0;
        MmpmClientClassType queriedClass = 0;
        
        if(!i)
            {
               // Request MPPS to make client active
               sts = Test_RequestMpps(clientId[i], &mppsReq);
               if(MMPM_STATUS_SUCCESS != sts)
                 {
                    retSts = MMPM_STATUS_FAILED;
                    goto TestConcurrentClientClasses_error;					
                 }                      	
            }
        else
        	{
		        // Request BW to make client active
                sts = Test_RequestBandwidth(clientId[i], &bwReq, 1, NULL);
                if(MMPM_STATUS_SUCCESS != sts)
                    goto TestConcurrentClientClasses_error;
        	}
        // Calc expected aggregate class
        for(int ii = 0; ii <= i; ii++)
              aggClass |= clientClass[ii];

        // Check aggregate class
        sts = getAggregateClass(&queriedClass);
        if(MMPM_STATUS_SUCCESS != sts)
               goto TestConcurrentClientClasses_error;
        if(!checkClass(queriedClass, aggClass))
          {
               ADSPPMTEST_LOG_PRINTF_2(ADSPPMTEST_LOG_LEVEL_ERROR,
               "[ERROR] unexpected aggregate client class returned from "
                "query: got %d, expected %d", queriedClass, aggClass);
                 goto TestConcurrentClientClasses_error;
          }
    }

    // Cleanup
    for(i = 0; i < N; i++)
    {
        sts = Test_Deregister(clientId[i]);
        if(MMPM_STATUS_SUCCESS != sts)
            goto TestConcurrentClientClasses_error;
    }

    return retSts;

TestConcurrentClientClasses_error:

    for(i = 0; i < N; i++)
    {
        if(clientId[i] != 0)
            Test_Deregister(clientId[i]);
    }

    return MMPM_STATUS_FAILED;

}

static MMPM_STATUS Test_CachePartitioning_09(void)
{
    MMPM_STATUS sts = MMPM_STATUS_FAILED;
    ADSPPMTEST_LOG_TEST_HEADER(CACHE_PARTITIONING_09_NAME,
        CACHE_PARTITIONING_09_DETAILS);

    sts = TestConcurrentClientClasseswithMPPSvote();

    ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_CachePartitioning,
        CACHE_PARTITIONING_09_NAME, sts);
    return sts;
}

#define CACHE_PARTITIONING_10_NAME    "cache_partitioning_10"
#define CACHE_PARTITIONING_10_DETAILS "2 clients DDR BW Req-Rel to change cache size"

static MMPM_STATUS TestCacheSizeBandwidthScaling(void)
{
#define   TOTAL_NUMCLIENTS    (2)

    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    BwReqTestType_Ext bw;
    uint32 clientId=0, clientId1 = 0, clientId2 = 0;
    uint64 bwVal = 10000000ull;
    MmpmCoreIdType coreId = MMPM_CORE_ID_LPASS_DML;
    MmpmCoreInstanceIdType instanceId = MMPM_CORE_INSTANCE_0;
    MmpmClientClassType clientclasstype =  MMPM_UNKNOWN_CLIENT_CLASS;
    uint32 numclients=0;	

    clientId1 = Test_Register(coreId, instanceId, CACHE_PARTITIONING_09_NAME"_1");

    clientId2 = Test_Register(coreId, instanceId, CACHE_PARTITIONING_09_NAME"_2");
  	
    if(clientId1 && clientId2)
    {
        bw.masterPort = MMPM_BW_PORT_ID_ADSP_MASTER;
        bw.slavePort = MMPM_BW_PORT_ID_DDR_SLAVE;
        bw.usagePercent = 100;
        bw.usageType = MMPM_BW_USAGE_LPASS_DSP;
        bw.type = Bw_Generic_Ext;		

        for (numclients =0; numclients < TOTAL_NUMCLIENTS; numclients++)
          {
                switch (numclients)
                  {
                    case 0:
                       clientId = clientId1;
                       bw.bwVal = bwVal;   
                       clientclasstype =  MMPM_AUDIO_CLIENT_CLASS;
                       break;
                    case 1:
                    default:
                       clientId = clientId2;
                       bw.bwVal = 60000000ull; 
                       clientclasstype =  MMPM_COMPUTE_CLIENT_CLASS;
                       break;
                  }
                 // Set class
                 sts = Test_SetClientClass(clientId, clientclasstype);  
                 if(MMPM_STATUS_SUCCESS != sts)
                 {
                      ADSPPMTEST_LOG_PRINTF_1(
                          ADSPPMTEST_LOG_LEVEL_ERROR,
                          "[ERROR] Set Client Class, Cache Size BW scaling case %u ",numclients); 	
                      retSts = MMPM_STATUS_FAILED;  	
                      goto TestCacheSizeBandwidthScaling_error;    
                 }

                 sts = Test_RequestBandwidthExt(clientId, &bw, 1);
                 if(MMPM_STATUS_SUCCESS != sts)
                 {
                      ADSPPMTEST_LOG_PRINTF_1(
                          ADSPPMTEST_LOG_LEVEL_ERROR,
                          "[ERROR] Ext Request, Cache Size BW scaling case %u",numclients);
                      retSts = MMPM_STATUS_FAILED;
                      goto TestCacheSizeBandwidthScaling_error;				
                 }


         }
         for (numclients =0; numclients < TOTAL_NUMCLIENTS; numclients++)
         {
             switch (numclients)
                {
                    case 0:
                        clientId = clientId1;
                        break;
                    case 1:
                    default:
                        clientId = clientId2;
                        break;
                 }

                 sts = Test_ReleaseResource(clientId, MMPM_RSC_ID_GENERIC_BW_EXT);
                 if(MMPM_STATUS_SUCCESS != sts)
                 {
                      ADSPPMTEST_LOG_PRINTF_1(
                      ADSPPMTEST_LOG_LEVEL_ERROR,
                          "[ERROR] Ext Release, Cache Size BW scaling case %u",numclients);
                          retSts = MMPM_STATUS_FAILED;
                      goto TestCacheSizeBandwidthScaling_error;				   
                 }
         }
     }
    else
   	{
           ADSPPMTEST_LOG_PRINTF_2(
                ADSPPMTEST_LOG_LEVEL_ERROR,
                "[ERROR] Client creation, Cache Size BW scaling clientId1 = %u, ClientID2 = %u ",clientId1, clientId2);
           retSts = MMPM_STATUS_FAILED;
   	}
TestCacheSizeBandwidthScaling_error:

    if(clientId1!= 0)
        Test_Deregister(clientId1);

    if(clientId2!= 0)
        Test_Deregister(clientId2);

    return retSts;	
}

static MMPM_STATUS Test_CachePartitioning_10(void)
{
      MMPM_STATUS sts = MMPM_STATUS_FAILED;
      ADSPPMTEST_LOG_TEST_HEADER(CACHE_PARTITIONING_10_NAME,
                  CACHE_PARTITIONING_10_DETAILS);
		
      sts = TestCacheSizeBandwidthScaling();
		
      ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_CachePartitioning,
                  CACHE_PARTITIONING_10_NAME, sts);
      return sts;
}

void Test_CachePartitioning(void)
{
    Test_CachePartitioning_01();
    Test_CachePartitioning_02();
    Test_CachePartitioning_03();
    Test_CachePartitioning_04();
    Test_CachePartitioning_05();
    Test_CachePartitioning_06();
    Test_CachePartitioning_07();
    Test_CachePartitioning_08();
    Test_CachePartitioning_09();
    Test_CachePartitioning_10();	
}

