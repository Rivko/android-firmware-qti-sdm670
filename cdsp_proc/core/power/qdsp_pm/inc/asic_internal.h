/*
* Copyright (c) 2015, 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
@file: asic_internal.h
@brief: Contains internal Asic Config Manager's definitions.

$Header: //components/rel/core.qdsp6/2.1.c4/power/qdsp_pm/inc/asic_internal.h#1 $
*/

#ifndef ASIC_INTERNAL_H_
#define ASIC_INTERNAL_H_

#include "asic.h"
#include "asic_internal_settings.h"


typedef struct
{
    int numElements;
    AsicMemDescriptorFullType pMemories[Adsppm_Mem_Max];
} AsicMemDescriptorFullArrayType;

/**
 * Enumeration to indicate function set used for this chip
 */
typedef enum
{
    AsicFnSetId_Default = 0 //!< AsicFnID_Default
} AsicFnSetIdType;

typedef struct
{
    AsicFnSetIdType functionSet;
    uint32  debugLevel;
    uint32  adspHwThreadNumber;
    uint64  adsppmLprTimeoutValue; //!< LPR timeout value in QTimer ticks (19.2Mhz)
    uint32  chipVersion;
    AsicHwioRegRangeType *lpassRegRange; //!< Register range for LPASS
    AsicHwioRegRangeType *l2ConfigRegRange; //!< Register range for L2 config
    AsicCoreDescriptorArrayType *cores;
    AsicMemDescriptorFullArrayType memories;
    AsicClockDescriptorArrayType *clocks;
    AsicBusPortDescriptorArrayType *busPorts;
    AsicBusRouteArrayType *extBusRoutes;
    AsicBusRouteArrayType *mipsBusRoutes;
    AsicRegProgSpeedArrayType *regProgSpeeds;
    AsicPwrDescriptorArrayType *pwrDomains;
    AsicCompensatedDdrBwTableType *compensatedDdrBwTable;
    AsicCompensatedAhbBwTableType *compensatedAhbBwTable[AsicBusType_Max];
    AsicThreadLoadingInfoTableType *threadLoadingInfoTable;
    AsicAudioVoiceCppTrendFactorsType *audioVoiceCppFactors;
    AsicCppFactorsTableType *audioCppFactorsTable;
    AsicCppFactorsTableType *voiceCppFactorsTable;
    AsicThreadLeakageArrayType *threadLeakage;
    AsicCachePartitionConfigTableType *cachePartitionConfigTable;
    AsicBwConcurrencySettingsType* bwConcurrencySettings;
    AsicAdspToAhbFreqTableType* adspToAhbFreqTable[AsicBusType_Max];
    AsicAdspCacheSizeMPPSThresholdTableType *cachesizeMPPSThresholdTable;
    AsicAdspCacheSizeBWScalingTableType*  cacheSizeBWscalingTable;
    AsicAdspSystemCacheConfigTableType*  systemcacheConfigTable;
    AsicSnocVoteTableType *snocBwVoteTable;
    Adsppm_Status (*pFn_GetBWFromMips)(AdsppmMIPSToBWAggregateType *pMipsAggregateData);
    Adsppm_Status (*pFn_GetClockFromMips)(AdsppmMIPSToClockAggregateType *pMipsAggregateData);
    Adsppm_Status (*pFn_GetClockFromBW)(uint32 *pClocks, AdsppmBusBWDataIbAbType *pAHBBwData);
    Adsppm_Status (*pFn_BusBWAggregate)(AdsppmBusBWDataIbAbType *pAggregateBwIbAbValue, AdsppmBusBWDataType *pBwValue);
    AsicFeatureDescType features[AsicFeatureId_enum_max]; //!< Feature enablement status
} AsicConfigType;

typedef struct
{
    uint32 numClients;
    uint32 factor;
} AsicFactorType;

/**
 * @fn asicGetAsicConfig - fill in ASIC configuration from Device Config image
 * @param pConfig - [OUT] pointer to ASIC configuration structure
 * @return completion status
 */
Adsppm_Status asicGetAsicConfig(AsicConfigType *pConfig);


#endif /* ASIC_INTERNAL_H_ */

