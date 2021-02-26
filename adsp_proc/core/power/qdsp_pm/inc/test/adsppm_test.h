/*
* Copyright (c) 2015 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
@file: adsppm_test.h
@brief: Header for ADSPPM wrapper for calls to MMPM.

$Header: //components/rel/core.qdsp6/2.1/power/qdsp_pm/inc/test/adsppm_test.h#1 $
*/

#ifndef ADSPPM_TEST_H
#define ADSPPM_TEST_H

#include "mmpm.h"
#include "adsppm.h"


#define MAX_CLK_REQUEST      8
#define MAX_BW_REQUEST       16
#define MAX_TEST_CLIENT      100
#define MAX_LEN_CLIENT_NAME  32
#define MAX_TEST             25
#define MAX_TEST_SEQUENCE    25


typedef enum
{
    REG,
    REQCLK,
    REQCLKDOMAIN,
    REQMIPS,
    REQSLATENCY,
    REQBW,
    REQREGP,
    REQPWR,
    REQMEMPWR,
    RELCLK,
    RELCLKDOMAIN,
    RELMIPS,
    RELSPATENCY,
    RELBW,
    RELREGP,
    RELPWR,
    RELMEMPWR,
    DREG,
    INFOCLK,
    INFOPWR,
    INFOAHBCLK,
    INFOQDSPCLK,
    THERML,
    SETPARAM,
    MAX_NUM_TEST
} TestSequenceType;

typedef enum
{
    Bw_Generic,
    Bw_Generic_Ext
} Bwtype ;

// Test are defined in this structure
typedef struct
{
    uint32                 clientNum;
    uint32                 numTestSeq;
    MmpmCoreIdType         coreId;
    MmpmCoreInstanceIdType instanceId;
    TestSequenceType       testSeq[MAX_TEST];
} AdsppmTestType;

typedef struct
{
    MmpmClkIdLpassType clkId;
    uint32             freq; //in Hz
    MmpmFreqMatchType  freqMatch;
} ClkTestType;

typedef struct
{
    MmpmClkIdLpassType          clkId;
    uint32                      clkFreqHz; //in Hz
    MmpmClkDomainSrcIdLpassType clkDomainSrc;
} ClkDomainTestType;

typedef struct
{
    MmpmBwPortIdType     masterPort;
    MmpmBwPortIdType     slavePort;
    uint64               bwVal;
    uint32               usagePercent;
    MmpmBwUsageLpassType usageType;
} BwReqTestType;

typedef struct
{
    MmpmBwPortIdType     masterPort;
    MmpmBwPortIdType     slavePort;
    Bwtype               type;
    uint64               bwVal;             // Actual bw requested by client
    uint32               usagePercent;
    MmpmBwUsageLpassType usageType;
} BwReqTestType_Ext;

typedef struct
{
    uint32                mipsTotal;
    uint32                mipsPerThread;
    MmpmBwPortIdType      codeLocation;
    MmpmMipsRequestFnType reqOperation;
} MipsReqTestType;

typedef struct
{
    MmpmMemIdType         memId;
    MmpmMemPowerStateType powerState;
} MemPwrReqTestType;

void Test_System(void);
void Test_CoreClock(void);
void Test_DomainClock(void);
void Test_Mips(void);
void Test_SleepLatency(void);
void Test_Bandwidth(void);
void Test_RegisterProgramming(void);
void Test_Power(void);
void Test_MemoryPower(void);
void Test_Bundle(void);
void Test_Stress(void);
void Test_Profile(void);
void Test_CachePartitioning(void);
void Test_DcvsAggregation(void);
void RunAllTests(void);

MMPM_STATUS Test_InvokeTest(
    AdsppmTestType test[],
    uint32 numTest,
    MMPM_STATUS testSts[][MAX_TEST_SEQUENCE]);

uint32 Test_Register(
    MmpmCoreIdType coreId,
    MmpmCoreInstanceIdType instanceId,
    char *clientName);

MMPM_STATUS Test_RequestCoreClock(
    uint32 clientId,
    ClkTestType clk[],
    uint32 numClk);

MMPM_STATUS Test_RequestCoreClockDomain(
    uint32 clientId,
    ClkDomainTestType clk[],
    uint32 numClk);

MMPM_STATUS Test_RequestMips(
    uint32 clientId,
    uint32 mips);

MMPM_STATUS Test_RequestMpps(
    uint32 clientId,
    MmpmMppsReqType *pMppsReq);

MMPM_STATUS Test_RequestSleepLatency(
    uint32 clientId,
    uint32 microSec);

MMPM_STATUS Test_RequestBandwidth(
    uint32 clientId,
    BwReqTestType *bw,
    uint32 numBw,
    void * expectedParams);

MMPM_STATUS Test_RequestBandwidthExt(
    uint32 clientId,
    BwReqTestType_Ext *bw,
    uint32 numBw);

MMPM_STATUS Test_RequestRegProg(
    uint32 clientId,
    MmpmRegProgMatchType match);

MMPM_STATUS Test_RequestPower(uint32 clientId);

MMPM_STATUS Test_RequestMemoryPower(
    uint32 clientId,
    MmpmMemIdType memType,
    MmpmMemPowerStateType powerState);

MMPM_STATUS Test_ReleaseResource(
    uint32 clientId,
    MmpmRscIdType resourceId);

MMPM_STATUS Test_Deregister(
    uint32 clientId);

MMPM_STATUS Test_InfoClk(
    MmpmCoreIdType coreId,
    MmpmCoreInstanceIdType instanceId,
    uint32 clkId, //MmpmClkIdComputeType || MmpmClkIdLpassType
    uint32 *clkFreq);

MMPM_STATUS Test_InfoMaxMips(
    MmpmCoreIdType coreId,
    MmpmCoreInstanceIdType instanceId,
    uint32 *mipsInfo);

MMPM_STATUS Test_InfoMaxBw(
    MmpmCoreIdType coreId,
    MmpmCoreInstanceIdType instanceId,
    uint64 *bwInfo);

MMPM_STATUS Test_InfoMpps(
    MmpmInfoMppsType *pMppsInfo);

int adsppmTest_Callback_Count(void);

MMPM_STATUS Test_SetClientClass(
    uint32 clientId,
    MmpmClientClassType clientClass);

MMPM_STATUS Test_SetSysCacheStreamId(
    uint32 clientId,
    MmpmSysCacheStreamId streamId);

MMPM_STATUS Test_InfoBw(
    MmpmCoreIdType coreId,
    MmpmCoreInstanceIdType instanceId,
    AdsppmInfoAggregatedBwType *bwInfo);

MMPM_STATUS Test_InfoAhb(
    MmpmCoreIdType coreId,
    MmpmCoreInstanceIdType instanceId,
    AdsppmInfoAhbType *ahbInfo);

MMPM_STATUS Test_Thermal(void);

boolean Test_CompareFreq(
    uint32 setClockFreq,
    uint32 readClockFreq);

void Test_SystemCache(void);

/* CDSPPM Test Cases */
void Test_CDSP_System( void );
void Test_CDSP_Power( void );
void Test_CDSP_CoreClock();
void Test_CDSP_Mips( void );
void Test_CDSP_Bandwidth( void );
void Test_CDSP_DcvsAggregation( void );
void Test_CDSP_SystemCache( void );

/* SLPI Test Cases */
void Test_Slpi_System(void);
void Test_Slpi_CoreClock(void);
void Test_Slpi_Mips(void);
void Test_Slpi_Bandwidth(void);

#endif /* ADSPPM_TEST_H */

