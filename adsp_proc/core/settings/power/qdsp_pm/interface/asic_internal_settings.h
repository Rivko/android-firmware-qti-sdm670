/*
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
@file: asic_internal.h
@brief: Contains internal Asic Config Manager's settings definitions.

$Header: //components/rel/core.qdsp6/2.1/settings/power/qdsp_pm/interface/asic_internal_settings.h#2 $
*/

#ifndef ASIC_INTERNAL_SETTINGS_H_
#define ASIC_INTERNAL_SETTINGS_H_

#include "asic_settings.h"


#define ASIC_CONFIG_DEVICE_NAME "/core/power/adsppm"

typedef struct
{
    const int numElements;
    const AsicCoreDescType *pCores;
} AsicCoreDescriptorArrayType;

typedef struct
{
    const int numElements;
    const AsicMemDescriptorType *pPwrDomainDesc;
} AsicMemDescriptorArrayType;

typedef struct
{
    const int numElements;
    const AsicClkDescriptorType *pClocks;
} AsicClockDescriptorArrayType;

typedef struct
{
    const int numElements;
    const AsicBusPortDescriptorType *pPorts;
} AsicBusPortDescriptorArrayType;

typedef struct
{
    const int numElements;
    const AdsppmBusRouteType *pRoutes;
} AsicBusRouteArrayType;

typedef struct
{
    const int numElements;
    const uint64 *pRegProgSpeeds;
} AsicRegProgSpeedArrayType;

typedef struct
{
    const int numElements;
    const AsicPowerDomainDescriptorType *pPwrDomains;
} AsicPwrDescriptorArrayType;

typedef struct
{
    const int numElements;
    const AsicCompensatedDdrBwTableEntryType *pRows;
} AsicCompensatedDdrBwTableType;

typedef struct
{
    const int numElements;
    const AsicSnocVoteTableEntryType *pRows;
} AsicSnocVoteTableType;


typedef struct
{
    uint64 bwThreshold;         // honest BW threshold (bytes/sec)
    uint64 bwVote;              // compensated Vote (bytes/sec)
} AsicCompensatedAhbBwTableEntryType;

typedef struct
{
    const int numElements;
    const AsicCompensatedAhbBwTableEntryType *pRows;
} AsicCompensatedAhbBwTableType;

typedef struct
{
    uint32 mppsThreshold;       // row selected if total MPPS <= mppThreshold
    AsicCppFactorsType cpp;
} AsicCppFactorsTableRowType;

typedef struct
{
    const int numElements;
    const AsicCppFactorsTableRowType *pRows;
} AsicCppFactorsTableType;

typedef struct
{
    const int numElements;
    const uint32 *leakage;
} AsicThreadLeakageArrayType;

typedef struct
{
    const int numElements;
    const uint32 *pData;
} AsicAudioVoiceCppTrendFactorsType;

typedef struct
{
    uint32 adspFreqHz;  // threshold for ADSP frequency
    uint32 ahbFreqHz;  // corresponding AHB frequency when <= ADSP threshold
} AsicAdspToAhbFreqTableEntry;

typedef struct
{
    uint32 cachesize;  // current cache size
    uint32 scalingfactor;  // corresponding fudging factor
} AsicAdspCacheSizeBWScalingTableEntry;

typedef struct
{
    const int numElements;
    const AsicAdspCacheSizeBWScalingTableEntry *pRows;
} AsicAdspCacheSizeBWScalingTableType;

typedef struct
{
    const int numElements;
    const AsicSystemCacheConfigType *pRows;
} AsicAdspSystemCacheConfigTableType;


typedef struct
{
    const int numElements;
    const AsicAdspToAhbFreqTableEntry *pRows;
} AsicAdspToAhbFreqTableType;

typedef struct
{
    MmpmClientClassType clientClass;  // client class
    uint32 mppsThreshold;  // corresponding mpps threshold
} AsicAdspCacheSizeMPPSThresholdTableEntry;

typedef struct
{
    const int numElements;
    const AsicAdspCacheSizeMPPSThresholdTableEntry *pRows;
} AsicAdspCacheSizeMPPSThresholdTableType;

#endif /* ASIC_INTERNAL_SETTINGS_H_ */

