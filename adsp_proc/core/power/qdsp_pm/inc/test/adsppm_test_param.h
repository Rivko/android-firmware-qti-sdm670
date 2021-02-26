/*
* Copyright (c) 2014 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

#ifndef ADSPPM_TEST_PARAM_H
#define ADSPPM_TEST_PARAM_H

#include "mmpm.h"
#include "adsppm_test.h"


// Request memory power parameter table
typedef struct
{
    MmpmMemIdType         memId;
    MmpmMemPowerStateType powerState;
} MmpmReqMemPowerTestParamTableType;

// Request clock parameter table
typedef struct
{
    uint32      numClks;
    ClkTestType clk[MAX_CLK_REQUEST];
} MmpmReqClockTestParamTableType;

// Request clock domain parameter table
typedef struct
{
    uint32            numClks;
    ClkDomainTestType clk[MAX_CLK_REQUEST];
} MmpmReqClockDomainTestParamTableType;

// Request bandwidth parameter table
typedef struct
{
    uint32        numBw;
    BwReqTestType bw[MAX_BW_REQUEST];
} MmpmReqBandwidthTestParamTableType;

// Request DDR bandwidth Ext parameter table
typedef struct
{
    uint32        numBw;
    BwReqTestType_Ext bwExt[MAX_BW_REQUEST];
} MmpmReqBandwidthTestParamTableTypeExt;

// Frequency plan table
typedef struct
{
    uint32             numFreq;
    MmpmClkIdLpassType clkId;
    uint32             freq[16];
} FreqPlanParam;

typedef struct
{
    uint32        numClks;
    FreqPlanParam clkParam[MAX_CLK_REQUEST];
} MmpmFreqPlanTestParamTableType;


void GetReqClockParam(MmpmCoreIdType coreId, ClkTestType clk[], uint32 *numClks);
void GetReqClockDomainParam(MmpmCoreIdType coreId, ClkDomainTestType clk[], uint32 *numClks);
void GetReqMipsParam(MmpmCoreIdType coreId, uint32 *mips);
void GetReqRegProgParam(MmpmCoreIdType coreId, MmpmRegProgMatchType *match);
void GetReqBandwidthParam(MmpmCoreIdType coreId, BwReqTestType bw[], uint32 *numBw);
void GetReqDdrBandwidthParam(BwReqTestType_Ext bw[], uint32 *numBw, uint32 index);
void GetReqInternalBandwidthParam(BwReqTestType_Ext bw[], uint32 *numBw, uint32 index);
void GetReqSleepLatencyParam(MmpmCoreIdType coreId, uint32 *microSec);
void GetReqMemPowerParam(MmpmCoreIdType coreId, MmpmMemIdType *memId, MmpmMemPowerStateType *powerState);
void GetInfoClkParam(MmpmCoreIdType coreId, MmpmClkIdLpassType *clkId);


#endif /* ADSPPM_TEST_PARAM_H */
