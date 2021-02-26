/*
* Copyright (c) 2015 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
@file: asic.h
@brief: ASIC Configuration manager's header file
        This file contains ASIC Configuration Manager specific declarations
        exposed to all components within ADSPPM.
        The header will not be exposed outside of ADSPPM.

$Header: //components/rel/core.qdsp6/2.1/power/qdsp_pm/inc/asic.h#3 $
*/

#ifndef ASIC_H_
#define ASIC_H_

#include "adsppm.h"
#include "icbarb.h"
#include "mmpm.h"
#include "asic_settings.h"


/**
 * Default log level set for ADSPPM code
 * Note: overridden by device config value
 */
#define ADSPPM_LOG_LEVEL ADSPPM_LOG_LEVEL_DEBUG

/**
 * Number of HW threads
 * Note: overridden by device config value
 */
#define ADSP_HW_THREAD_NUMBER_DEFAULT 3

/**
 * Default delay for overhang requests in QTImer ticks.
 * Corresponds to 5ms
 */
#define ADSPPM_LPR_VOTE_TIMEOUT_DEFAULT 96000

#define ADSPPM_BW_USAGE_FACTOR  (256)

#define PERCENTS_IN_A_WHOLE  (256)


#define NPA_SCHEDULER_NODE_NAME "/init/npa_scheduled_requests"


/**
 * Type definition for Aggregation function pointer
 * @param pInputData - [IN] pointer to resource-specific list of the requests
 * @param pOutputData - [OUT] pointer to resource-specific value to return aggregation results
 */
typedef void (*Asic_Rsc_AggregationFnType)(void *pInputData, void *pOutputData);

/*************************************************************************************************/
/*                            Bus definitions                                                    */
/*************************************************************************************************/
typedef struct
{
    AdsppmBusPortIdType adsppmMaster;
    ICBId_MasterType  icbarbMaster;
} AsicBusExtMasterType;

typedef struct
{
    AdsppmBusPortIdType adsppmSlave;
    ICBId_SlaveType  icbarbSlave;
} AsicBusExtSlaveType;

typedef struct
{
    AsicBusExtMasterType masterPort;
    AsicBusExtSlaveType slavePort;
} AsicBusExtRouteType;

/*************************************************************************************************/
/*                            Memory type definitions                                            */
/*************************************************************************************************/
typedef struct
{
    uint64 startAddr; //!< Start address
    uint32 size;      //!< memory range size
} AsicAddressRangeType;

/**
 * @struct AsicMemDescFullType - Defines entry in the ASIC config table defining Memory Descriptor
 */
typedef struct
{
    const AsicMemDescriptorType *pDescriptor; //!< pointer to memory descriptor
    AsicAddressRangeType virtAddr;            //!< virtual address range
} AsicMemDescriptorFullType;

/*****************************************************************************/
/*                  Cache partitioning configuration definitions             */
/*****************************************************************************/
typedef struct
{
    uint32 floorFudgeFactor;
    uint32 ceilingFudgeFactor;
} AsicCompensatedBWFudgeFactor;

typedef struct
{
    uint32 leakage[ADSP_HW_THREAD_NUMBER_DEFAULT];
} AsicThreadLeakageType;

typedef enum
{
    AsicBusType_AhbIx,
    AsicBusType_AhbE,
    AsicBusType_Max
} AsicBusType;

/******************************************************************************************/
/*               Restricted functions definitions                                         */
/******************************************************************************************/
Adsppm_Status ACM_Init(void);

uint32 ACM_GetDebugLevel(void);

AsicHwRscIdType ACM_GetHwRscId(AdsppmCoreIdType, AdsppmRscIdType);

//Asic_Rsc_AggregationFnType ACM_GetAggregationFn(AsicHwRscIdType, AdsppmCoreIdType);

/**
 * @fn ACM_BusBWAggregate - Aggregate bw
 */
Adsppm_Status ACM_BusBWAggregate(AdsppmBusBWDataIbAbType *pAggregateBwIbAbValue, AdsppmBusBWDataType *pBwValue);

Adsppm_Status ACM_GetBWFromMips(AdsppmMIPSToBWAggregateType *pMipsAggregateData);

Adsppm_Status ACM_GetClockFromMips(AdsppmMIPSToClockAggregateType *pMipsAggregateData);

Adsppm_Status ACM_GetClockFromBW(uint32 *pClock, AdsppmBusBWDataIbAbType *pAHBBwData);

void ACM_GetCompensatedDdrBwTableEntry( uint64 vote, AsicCompensatedDdrBwTableEntryType *pTableEntry);
void ACM_GetSnocVoteTableEntry(uint64 vote,AsicSnocVoteTableEntryType * pTableEntry);


void ACM_ApplyAdspDdrBwConcurrencyFactor(uint32 numVotes, uint64 maxBw, uint64* pAb, uint64* pIb, AsicCompensatedBWFudgeFactor *compensatedBWFudgeFactorValues);

uint32 ACM_GetMipsFromMpps(uint32 mppsTotal, uint32 numDominantThreads);

const AsicCppFactorsType* ACM_GetAudioCppFactors(uint32 mppsTotal);
const AsicCppFactorsType* ACM_GetVoiceCppFactors(uint32 mppsTotal);
const AsicThreadLoadingInfoType* ACM_GetThreadLoading(uint32 numDominantThreads);

Adsppm_Status ACM_ApplyAhbBwCompensation(uint64 bwVoteIn, AsicBusType busType, uint64* pBwVoteOut);

Adsppm_Status ACM_GetAhbFromAdspFreq(uint32 adspClockFreq, AsicBusType busType, uint32* ahbClockFreq);

Adsppm_Status ACM_GetAhbIxFromAdspFreq(uint32 adspClockFreq, uint32* ahbClockFreq);

Adsppm_Status ACM_GetMPPSThresholdfromClientClass(MmpmClientClassType clientClass, uint32* mppsThreshold);

Adsppm_Status ACM_GetBWScalingFactorFromCacheSize(uint32 cachesize, uint32* scalingfactor);

Adsppm_Status ACM_SystemCacheUsecaseIdfromClientClass(uint32 clientClass,uint32* usecaseId);


/**
 * Request number of supported routes (master/slave pairs) for external BW requests
 * @return number of supported routes (master/slave pairs) for external BW requests
 */
int ACMBus_GetNumberOfExtRoutes(void);

/**
 * Request supported routes (master/slave pairs) for external BW requests
 * @param pExtRoutes - [OUT] array to be filled with supported routes (master/slave pairs) for external BW requests
 */
void ACMBus_GetExtRoutes(AsicBusExtRouteType *pExtRoutes);

/**
 * Get core clock ID for corresponding core instance
 * @param coreId
 * @param instanceId
 * @param coreClock
 * @return if core clock is associated to this core, return associated core instance core clock ID
 *         else return core clock ID passed in coreClock
 */
AdsppmClkIdType ACMClk_GetInstanceCoreClockId(AdsppmCoreIdType coreId, AdsppmInstanceIdType instanceId, AdsppmClkIdType coreClock);

/**
 * Get master bus port ID for corresponding core instance
 * @param coreId
 * @param instanceId
 * @param masterBusPort
 * @return if master bus port is associated to this core, return associated core instance master bus port ID
 *         else return bus port ID passed in masterBusPort
 */
AdsppmBusPortIdType ACMBus_GetInstanceMasterBusPortId(AdsppmCoreIdType coreId, AdsppmInstanceIdType instanceId, AdsppmBusPortIdType masterBusPort);

/**
 * Get slave bus port ID for corresponding core instance
 * @param coreId
 * @param instanceId
 * @param slaveBusPort
 * @return if slave bus port is associated to this core, return associated core instance slave bus port ID
 *         else return bus port ID passed in slaveBusPort
 */
AdsppmBusPortIdType ACMBus_GetInstanceSlaveBusPortId(AdsppmCoreIdType coreId, AdsppmInstanceIdType instanceId, AdsppmBusPortIdType slaveBusPort);

/**
 * @fn ACMBus_GetCoreSlavePort - returns slave bus port associated with the core the client is registered for
 * @param coreId
 * @param instanceId
 * @return slave bus port ID associated with the registered core
 */
AdsppmBusPortIdType ACMBus_GetCoreSlavePort(AdsppmCoreIdType coreId, AdsppmInstanceIdType instanceId);

/**
 * Returns pointer to the the descriptor for the specified bus port.
 * @param port
 * @return pointer to the bus port descriptor
 */
const AsicBusPortDescriptorType *ACMBus_GetPortDescriptor(AdsppmBusPortIdType);

/**
 * @fn ACMBus_GetPortConnections
 * @brief Returns bus port connections for the specified bus port.
 * @param port - AdsppmBusPortIdType
 * @return busPortConnections - AsicBusPortConnectionType
 */
AsicBusPortConnectionType ACMBus_GetPortConnections(AdsppmBusPortIdType port);

/**
 * Get type of the clock
 * @param Clock Id
 * @return Type of the clock (enable/disable, frequency set, npa)
 */
AsicClkTypeType ACMClk_GetClockType(AdsppmClkIdType);

/**
 * @fn ACM_IsClkIdValid - get ClkId present in the global clkId array
 *
 * Function gets ...
 *
 * @return completion status
 */
boolean ACM_IsClkIdValid(AdsppmClkIdType clk);

/**
 * @fn ACM_GetCoreIdIdx - get CoreId index in global coreId array
 *
 * Function gets ...
 *
 * @return completion status
 */
Adsppm_Status ACM_GetCoreIdIdx(AdsppmCoreIdType core , uint32 *coreIdx);

/**
 * @fn ACM_GetClkIdx - get ClkId index in global clkId array
 *
 * Function gets ...
 *
 * @return completion status
 */
Adsppm_Status ACM_GetClkIdx(AdsppmClkIdType clk , uint32 *clkIdx);

/**
 * Request number of clock descriptors
 * @return number of clock descriptors
 */
const int ACMClk_GetNumberOfClockDescriptors(void);

/**
 * Get descriptor the clock
 * @param Clock Id
 * @return Type of the clock (enable/disable, frequency set, npa)
 */
const AsicClkDescriptorType *ACMClk_GetClockDescriptor(uint32 clkDesIndex);

/**
 * Get descriptor the clock
 * @param Clock Id
 * @return Type of the clock (enable/disable, frequency set, npa)
 */
const AsicClkDescriptorType *ACMClk_GetClockDescriptorById(AdsppmClkIdType);

/**
 * @fn ACM_IsPwrDomainValid - get pwrDomain present in the global pwr array
 *
 * Function gets ...
 *
 * @return completion status
 */
boolean ACM_IsPwrDomainValid(AsicPowerDomainType domain);

/**
 * @fn ACM_GetPwrDomainIdx - get pwrDomain index in global pwrDomain array
 *
 * Function gets ...
 *
 * @return completion status
 */
Adsppm_Status ACM_GetPwrDomainIdx(AsicPowerDomainType domain , uint32 *domainIdx);

/**
 * Request number of pwrDomain descriptors
 * @return number of pwrDomain descriptors
 */
const int ACMClk_GetNumberOfPwrDomainDescriptors(void);

/**
 * Get info about the power domain
 * @param Domain Id
 * @return Power Domain Info
 */
const AsicPowerDomainDescriptorType *ACMClk_GetPwrDomainDescriptor(uint32);

/**
 * Get power domain for the specified core
 * @param Core ID
 * @return Power Domain
 */
AsicPowerDomainType ACMPwr_GetPowerDomain(AdsppmCoreIdType);

/**
 * Get power domain for the specified memory
 * @param Memory ID
 * @return Power Domain
 */
AsicPowerDomainType ACMPwr_GetMemPowerDomain(AdsppmMemIdType mem);

/**
 * Get power resource type  for the specified memory
 * @param Memory ID
 * @return Power Domain
 */
AsicHwRscIdType ACMPwr_GetMemPowerType(AdsppmMemIdType mem);

/**
 * Get descriptor for the specified core
 * @param Core ID
 * @return pointer to the descriptor for this core
 */
const AsicCoreDescType *ACM_GetCoreDescriptorById(AdsppmCoreIdType);

/**
 * Get Interrupt ID for the TZ secure interrupt
 */
uint32 ACM_GetTZSecureInterrupt(void);

/**
 * Get feature enablement status for a particular feature
 */
AsicFeatureStateType ACM_GetFeatureStatus(AsicFeatureIdType);

/**
 * Get feature descriptor for a particular feature
 */
AsicFeatureDescType *ACM_GetFeatureDescriptor(AsicFeatureIdType featureId);

/**
 * Adjust parameter value based on feature enablement and configuration data
 * @param featureId - feature ID.
 * @param value - value, which needs to be adjusted
 * @return - adjusted value
 */
uint64 ACM_AdjustParamValue(AsicFeatureIdType featureId, uint64 value);

/**
 * Convert time in ms to sclk
 * @param ms - time in ms.
 * @return - number of sclks
 */
uint64 ACM_ms_to_sclk(uint64 ms);

/**
 * Convert time in us to qclk (QTimer clock ticks)
 * @param us - time in us.
 * @return - number of qclks
 */
uint64 ACM_us_to_qclk(uint64 us);

/**
 * Request number of supported routes (master/slave pairs) for MIPS BW requests
 * @return number of supported routes (master/slave pairs) for MIPS BW requests
 */
const int ACMBus_GetNumberOfMipsBwRoutes(void);

/**
 * Request list supported routes (master/slave pairs) for MIPS BW requests
 * @return pointer to array of supported routes (master/slave pairs) for MIPS BW requests
 */
const AdsppmBusRouteType *ACMBus_GetMipsBwRoutes(void);

/**
 * Get number of HW threads for the ASIC's ADSP
 * @return number of threads
 */
uint32 ACM_GetHwThreadNumber(void);

/**
 * Get a timeout value in ms for MIPS/BW optimization LPR
 * @return timeout value in ms
 */
uint64 ACM_GetLprVoteTimeoutValue(void);

/**
 * Get LPASS HWIO base address
 * @return pointer to the structure containing HWIO base address
 */
AsicHwioRegRangeType *ACM_GetLpassRegRange(void);

/**
 * Get L2 cache configuration registers address range
 * @return pointer to the structure containing registers address range
 */
AsicHwioRegRangeType *ACM_GetL2ConfigRegRange(void);

/**
 * Get info about the power domain
 * @param Memory Id
 * @return Memory Info
 */
AsicMemDescriptorFullType *ACM_GetMemoryDescriptor(AdsppmMemIdType mem);

/**
 * Get address range for the specified memory
 * @param Memory ID
 * @return address range
 */
AsicAddressRangeType *ACM_GetVirtMemAddressRange(AdsppmMemIdType mem);

/**
 * Set address range for the specified memory
 * @param mem - Memory ID
 * @param addr - start address
 * @param size - address range size
 * @return operation status
 */
AdsppmStatusType ACM_SetVirtMemAddressRange(AdsppmMemIdType mem, uint64 addr, uint32 size);

AsicCachePartitionConfigTableType* ACM_GetCachePartitionConfig(void);

/**
 * Apply scaling of 64-bit input:
 * output = input * num / denom
 * UINT_MAX is returned if result would overflow 32-bit unsigned integer
 */
uint64 scale64(uint64 input, uint32 num, uint32 denom);

/**
 * Apply scaling of 32-bit input:
 * output = input * num / denom
 * UINT_MAX is returned if result would overflow 32-bit unsigned integer
 */
uint32 scale32(uint32 input, uint32 num, uint32 denom);

AsicBwConcurrencySettingsType* ACM_GetAdspDdrBwConcurrencySettings(void);

uint32 ACM_GetThreadLeakage(uint32 thread);

#endif /* ASIC_H_ */

