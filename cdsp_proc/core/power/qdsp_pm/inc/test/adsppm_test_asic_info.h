/*
* Copyright (c) 2013-2015 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

#ifndef ADSPPM_TEST_ASIC_INFO_H
#define ADSPPM_TEST_ASIC_INFO_H

#include "mmpm.h"

#define MAX_AHB_BW   614400000

typedef enum
{
    MIN_SVS,
    LOW_SVS,
    SVS,
    SVS2,
    NOM,
    TURBO
} AsicInfo_VoltageLevel;

typedef struct
{
    uint32 frequency;
    AsicInfo_VoltageLevel voltage; 
} AsicInfo_ClockVoltageType;

typedef struct
{
    uint32 numFreq;
    const AsicInfo_ClockVoltageType *pFreqVoltPlan;
} AsicInfo_ClockFreqVoltPlanType;

typedef struct
{
    uint32 mips2clockFactor;
    uint32 numThreads;
} AsicInfo_MipsInfoType;

typedef struct 
{
    uint64 bwThreshold;     // Honest BW threshold (bytes/sec)
    uint64 bwVote;          // Compensated Vote(bytes/sec), -1 means use honest vote
    uint32 latencyVote;     // latency in (nanoSec), 0 means no latency vote needed
    uint32 snocFloorVote;   // snoc Floor Vote from ADSPPM (Mhz)
    uint32 honestFlag;      // Flag indicating Honest/dishonest voting
} AsicInfo_CompensatedDdrBwTable;

typedef struct
{
    uint32 num;
    const AsicInfo_CompensatedDdrBwTable *table;
} AsicInfo_CompensatedDdrBwTableType;

typedef struct
{
    uint32 threshold;
    uint32 concurrencyFactorArray[3];
} AsicInfo_BwConcurrencySettingsTable;

typedef struct 
{
    uint64 bwThreshold;     // Honest BW threshold (bytes/sec)
    uint64 bwVote;          // Compensated Vote(bytes/sec), -1 means use honest vote
} AsicInfo_CompensatedAhbBwTableEntry;

typedef struct
{
    uint32 num;
    const AsicInfo_CompensatedAhbBwTableEntry *table;
} AsicInfo_CompensatedAhbBwTableType;

typedef struct
{
    const AsicInfo_BwConcurrencySettingsTable *table;
} AsicInfo_BwConcurrencySettingsTableType;

// Check core instance availability
boolean AsicInfo_isCoreInstanceAvailable(MmpmCoreIdType coreId, MmpmCoreInstanceIdType instanceId);

// Check core clock availability
boolean AsicInfo_isCoreClockAvailable(MmpmClkIdLpassType coreClkId);

// Check bus port availability
boolean AsicInfo_isBusPortAvailable(MmpmBwPortIdType busPortId);

// Check memory availability
boolean AsicInfo_isMemoryAvailable(MmpmMemIdType memId);

// Provide clock frequency (in Hz) voltage level plan
const AsicInfo_ClockFreqVoltPlanType *AsicInfo_getClockFreqVoltPlan(MmpmClkIdLpassType clkId);

// Provide MIPS related information
const AsicInfo_MipsInfoType *AsicInfo_getMipsInfo(void);

// Get Compensated DDR BW table
const AsicInfo_CompensatedDdrBwTableType *AsicInfo_getCompensatedDdrBwInfo(void);

// Get BW Concurrency Settings Table
const AsicInfo_BwConcurrencySettingsTableType *AsicInfo_getBwConcurrencyInfo(void);

// Get Compensated AHB BW table
const AsicInfo_CompensatedAhbBwTableType *AsicInfo_getCompensatedAhbBwInfo(void);
#endif /* ADSPPM_TEST_ASIC_INFO_H */
