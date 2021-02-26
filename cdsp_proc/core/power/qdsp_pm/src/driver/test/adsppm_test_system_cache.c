/*
* Copyright (c) 2015 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
@file: adsppm_test_system_cache.c
@brief: Implementation of ADSPPM test module of System Cache.

$Header: //components/rel/core.qdsp6/2.1.c4/power/qdsp_pm/src/driver/test/adsppm_test_system_cache.c#1 $
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


#define SYSTEM_CACHE_2_NAME    "system_cache_2"
#define SYSTEM_CACHE_2_DETAILS "2 clients DDR BW Req-Rel to test LPASS core sys cache SCID programming"

static MMPM_STATUS TestSystemCacheLpassCoreScid(void)
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

    clientId1 = Test_Register(coreId, instanceId, SYSTEM_CACHE_2_NAME"_1");

    clientId2 = Test_Register(coreId, instanceId, SYSTEM_CACHE_2_NAME"_2");

    memset(&bw,0,sizeof(BwReqTestType_Ext));

    if(clientId1 && clientId2)
    {
        bw.masterPort = MMPM_BW_PORT_ID_DML_MASTER;
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
                       clientclasstype =  MMPM_AUDIO_CLIENT_CLASS;
                       break;
                  }
                 // Set class
                 sts = Test_SetClientClass(clientId, clientclasstype);
                 if(MMPM_STATUS_SUCCESS != sts)
                 {
                      ADSPPMTEST_LOG_PRINTF_1(
                          ADSPPMTEST_LOG_LEVEL_ERROR,
                          "[ERROR] Set Client Class, SYstem Cache LPASS Core case %u ",numclients);
                      retSts = MMPM_STATUS_FAILED;
                      goto TestSystemCache_error;
                 }

                 sts = Test_RequestBandwidthExt(clientId, &bw, 1);
                 if(MMPM_STATUS_SUCCESS != sts)
                 {
                      ADSPPMTEST_LOG_PRINTF_1(
                          ADSPPMTEST_LOG_LEVEL_ERROR,
                          "[ERROR] Ext Request, System Cache LPASS Core case %u",numclients);
                      retSts = MMPM_STATUS_FAILED;
                      goto TestSystemCache_error;
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
                          "[ERROR] Ext Release, System Cache case %u",numclients);
                          retSts = MMPM_STATUS_FAILED;
                      goto TestSystemCache_error;
                 }
         }
     }
    else
    {
           ADSPPMTEST_LOG_PRINTF_2(
                ADSPPMTEST_LOG_LEVEL_ERROR,
                "[ERROR] Client creation, System Cache clientId1 = %u, ClientID2 = %u ",clientId1, clientId2);
           retSts = MMPM_STATUS_FAILED;
    }
TestSystemCache_error:

    if(clientId1!= 0)
        Test_Deregister(clientId1);

    if(clientId2!= 0)
        Test_Deregister(clientId2);

    return retSts;
}


#define SYSTEM_CACHE_1_NAME    "system_cache_1"
#define SYSTEM_CACHE_1_DETAILS "2 clients DDR BW Req-Rel to test Q6 sys cache SCID programming"

static MMPM_STATUS TestSystemCacheQ6Scid(void)
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

    clientId1 = Test_Register(coreId, instanceId, SYSTEM_CACHE_1_NAME"_1");

    clientId2 = Test_Register(coreId, instanceId, SYSTEM_CACHE_1_NAME"_2");

    memset(&bw,0,sizeof(BwReqTestType_Ext));

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
                       clientclasstype =  MMPM_VOICE_CLIENT_CLASS;
                       break;
                  }
                 // Set class
                 sts = Test_SetClientClass(clientId, clientclasstype);
                 if(MMPM_STATUS_SUCCESS != sts)
                 {
                      ADSPPMTEST_LOG_PRINTF_1(
                          ADSPPMTEST_LOG_LEVEL_ERROR,
                          "[ERROR] Set Client Class, System Cache case %u ",numclients);
                      retSts = MMPM_STATUS_FAILED;
                      goto TestSystemCache_error;
                 }
                 // Set Stream Id
                 uint32 streamIdMask = 0x321;
                 MmpmSysCacheStreamId streamId;
                 streamId.streamIdMask = streamIdMask;
                 sts = Test_SetSysCacheStreamId(clientId, streamId);
                 if(MMPM_STATUS_SUCCESS != sts)
                 {
                      ADSPPMTEST_LOG_PRINTF_1(
                          ADSPPMTEST_LOG_LEVEL_ERROR,
                          "[ERROR] Set Stream Id %u ",numclients);
                      retSts = MMPM_STATUS_FAILED;
                      goto TestSystemCache_error;
                 }

                 sts = Test_RequestBandwidthExt(clientId, &bw, 1);
                 if(MMPM_STATUS_SUCCESS != sts)
                 {
                      ADSPPMTEST_LOG_PRINTF_1(
                          ADSPPMTEST_LOG_LEVEL_ERROR,
                          "[ERROR] Ext Request, System Cache case %u",numclients);
                      retSts = MMPM_STATUS_FAILED;
                      goto TestSystemCache_error;
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
                          "[ERROR] Ext Release, System Cache case %u",numclients);
                          retSts = MMPM_STATUS_FAILED;
                      goto TestSystemCache_error;
                 }
         }
     }
    else
    {
           ADSPPMTEST_LOG_PRINTF_2(
                ADSPPMTEST_LOG_LEVEL_ERROR,
                "[ERROR] Client creation, System Cache clientId1 = %u, ClientID2 = %u ",clientId1, clientId2);
           retSts = MMPM_STATUS_FAILED;
    }
TestSystemCache_error:

    if(clientId1!= 0)
        Test_Deregister(clientId1);

    if(clientId2!= 0)
        Test_Deregister(clientId2);

    return retSts;
}



#define CDSP_SYSTEM_CACHE_1_NAME    "cdsp_system_cache_1"
#define CDSP_SYSTEM_CACHE_1_DETAILS "2 clients DDR BW Req-Rel to test Q6 sys cache SCID programming"

static MMPM_STATUS TestCDSPSystemCacheQ6Scid(void)
{
#define   TOTAL_NUMCLIENTS    (2)

    MMPM_STATUS sts = MMPM_STATUS_FAILED, retSts = MMPM_STATUS_SUCCESS;
    BwReqTestType_Ext bw;
    uint32 clientId=0, clientId1 = 0, clientId2 = 0;
    uint64 bwVal = 10000000ull;
    MmpmCoreIdType coreId = MMPM_CORE_ID_COMPUTE_DMA;
    MmpmCoreInstanceIdType instanceId = MMPM_CORE_INSTANCE_0;
    MmpmClientClassType clientclasstype =  MMPM_UNKNOWN_CLIENT_CLASS;
    uint32 numclients=0;

    clientId1 = Test_Register(coreId, instanceId, CDSP_SYSTEM_CACHE_1_NAME"_1");

    clientId2 = Test_Register(coreId, instanceId, CDSP_SYSTEM_CACHE_1_NAME"_2");

    memset(&bw,0,sizeof(BwReqTestType_Ext));

    if(clientId1 && clientId2)
    {
        bw.masterPort = MMPM_BW_PORT_ID_Q6DSP_MASTER;
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
                    clientclasstype =  MMPM_COMPUTE_CLIENT_CLASS;
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
                      "[ERROR] Set Client Class, System Cache case %u ",numclients);
                 retSts = MMPM_STATUS_FAILED;
                 goto TestSystemCache_error;
            }
            // Set Stream Id
            uint32 streamIdMask = 0x320;
            MmpmSysCacheStreamId streamId;
            streamId.streamIdMask = streamIdMask;
            sts = Test_SetSysCacheStreamId(clientId, streamId);
            if(MMPM_STATUS_SUCCESS != sts)
            {
                 ADSPPMTEST_LOG_PRINTF_1(
                     ADSPPMTEST_LOG_LEVEL_ERROR,
                     "[ERROR] Set Stream Id %u ",numclients);
                 retSts = MMPM_STATUS_FAILED;
                 goto TestSystemCache_error;
            }

            sts = Test_RequestBandwidthExt(clientId, &bw, 1);
            if(MMPM_STATUS_SUCCESS != sts)
            {
                ADSPPMTEST_LOG_PRINTF_1(
                      ADSPPMTEST_LOG_LEVEL_ERROR,
                          "[ERROR] Ext Request, System Cache case %u",numclients);
                      retSts = MMPM_STATUS_FAILED;
                goto TestSystemCache_error;
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
                          "[ERROR] Ext Release, System Cache case %u",numclients);
                          retSts = MMPM_STATUS_FAILED;
                      goto TestSystemCache_error;
                 }
         }
    }
    else
    {
           ADSPPMTEST_LOG_PRINTF_2(
                ADSPPMTEST_LOG_LEVEL_ERROR,
                "[ERROR] Client creation, System Cache clientId1 = %u, ClientID2 = %u ",clientId1, clientId2);
           retSts = MMPM_STATUS_FAILED;
    }
TestSystemCache_error:

    if(clientId1!= 0)
        Test_Deregister(clientId1);

    if(clientId2!= 0)
        Test_Deregister(clientId2);

    return retSts;
}


static MMPM_STATUS Test_SystemCache_1(void)
{
      MMPM_STATUS sts = MMPM_STATUS_FAILED;
      ADSPPMTEST_LOG_TEST_HEADER(SYSTEM_CACHE_1_NAME,
                  SYSTEM_CACHE_1_DETAILS);

      sts = TestSystemCacheQ6Scid();

      ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_SystemCache,
                  SYSTEM_CACHE_1_NAME, sts);
      return sts;
}

static MMPM_STATUS Test_SystemCache_2(void)
{
      MMPM_STATUS sts = MMPM_STATUS_FAILED;
      ADSPPMTEST_LOG_TEST_HEADER(SYSTEM_CACHE_2_NAME,
                  SYSTEM_CACHE_2_DETAILS);

      sts = TestSystemCacheLpassCoreScid();

      ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_SystemCache,
                  SYSTEM_CACHE_2_NAME, sts);
      return sts;
}

void Test_SystemCache(void)
{
    Test_SystemCache_1();
    Test_SystemCache_2();
}

static MMPM_STATUS Test_CDSP_SystemCache_1(void)
{
      MMPM_STATUS sts = MMPM_STATUS_FAILED;
      ADSPPMTEST_LOG_TEST_HEADER(CDSP_SYSTEM_CACHE_1_NAME,
                  CDSP_SYSTEM_CACHE_1_DETAILS);

      sts = TestCDSPSystemCacheQ6Scid();

      ADSPPMTEST_LOG_TEST_RESULT(AdsppmTestId_SystemCache,
                  CDSP_SYSTEM_CACHE_1_NAME, sts);
      return sts;
}

void Test_CDSP_SystemCache(void)
{
    Test_CDSP_SystemCache_1();
}
