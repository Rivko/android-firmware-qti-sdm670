/*
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
@file: asic.h
@brief: ASIC Configuration manager's header file
        This file contains ASIC Configuration Manager settings-specific declarations
        exposed to all components within ADSPPM.
        The header will not be exposed outside of ADSPPM.

$Header: //components/rel/core.qdsp6/2.1.c4/settings/power/qdsp_pm/interface/asic_settings.h#1 $
*/

#ifndef ASIC_SETTINGS_H_
#define ASIC_SETTINGS_H_

#include "icbarb.h"
#include "DDIIPCInt.h"
#include "mmpm.h"
#include "qurt_memory.h"

/******************************************************************************************/
/*               ASIC Config Mgr type definitions                                         */
/******************************************************************************************/
/**
 * @enum AsicHwRscIdType - defines HW resources, which will be controlled by resource managers.
 * The values can be combined in a bitmap
 */
typedef enum
{
    AsicRsc_None        = 0x0,
    AsicRsc_Power_Mem   = 0x1,
    AsicRsc_Power_ADSP  = 0x2,
    AsicRsc_Power_Core  = 0x4,
    AsicRsc_MIPS_Clk    = 0x8,
    AsicRsc_MIPS_BW     = 0x10,
    AsicRsc_BW_Internal = 0x20,
    AsicRsc_BW_External = 0x40,
    AsicRsc_Core_Clk    = 0x80,
    AsicRsc_Latency     = 0x100,
    AsicRsc_Thermal     = 0x200,
    AsicRsc_Power_Ocmem = 0x400,
    AsicRsc_Power_Hvx   = 0x800,
    AsicRsc_EnumForce32bit = 0x7fffffff  //!< Forcing enum to be 32 bit
} AsicHwRscIdType;

/*************************************************************************************************/
/*                            Clock definitions                                                  */
/*************************************************************************************************/
/**
 * @enum AsicClkTypeType - defines how a particular clock is exposed by ClkRegime
 */
typedef enum
{
    AsicClk_TypeNone = 0,
    AsicClk_TypeNpa,          //!< Clock exposed as an NPA node
    AsicClk_TypeDalFreqSet,   //!< Clock exposed through DALSys and allows to set frequency
    AsicClk_TypeDalEnable,    //!< Clock exposed through DALSys and only allows to enable/disable it
    AsicClk_TypeInternalCGC,  //!< Clock is enable/disable type and controlled by ADSPPM internally
                                //!< Internal CGC's are physically inside LPASS Core
                                //!< Thus control interface is not provided by Clock driver
    AsicClk_TypeCBC,          //!< Some Internal CGCs have a matching CBC in LCC
                                //!< Both need to be enabled to enable clock branch
                                //!< Clock exposed through DALSys and only allows to enable/disable it
                                //!< These type was added to avoid double enable/disable calls at init
                                //!< To be enabled/disabled based on calls to the matching CGC
    AsicClk_TypeDalDomainSrc
} AsicClkTypeType;

/**
 * @enum AsicClkCntlTypeType - defines how a particular clock is controlled
 */
typedef enum
{
    AsicClk_CntlNone = 0,
    AsicClk_CntlOff,          //!< Clock is set to OFF at init time and never turned ON
    AsicClk_CntlAlwaysON,     //!< Clock is set to ON at init time and never turned OFF
    AsicClk_CntlAlwaysON_DCG, //!< If DCG feature is enabled set clock to HW control,
                                //!< else, treat it as a AsicClk_CntlAlwaysON clock
    AsicClk_CntlSW,           //!< Clock is SW clock gated based on client requests
    AsicClk_CntlSW_DCG        //!< If DCG feature is enabled set clock to HW control,
                                //!< else, treat it as a AsicClk_CntlSW clock
} AsicClkCntlTypeType;

/**
 * @enum AsicClkMemRetentionType - defines how a memory core for particular clock is controlled
 */
typedef enum
{
    AsicClk_MemCntlNone = 0,
    AsicClk_MemCntlAlwaysRetain, //!< Memory core for this clock should be always retained.
} AsicClkMemRetentionType;

typedef struct
{
    uint32 baseAddr;
    uint32 physAddr;
    uint32 size;
} AsicHwioRegRangeType;

/**
 * @struct AsicClkHwIoInfoType - defines info structure storing register level information about HWIO type clocks
 */
typedef struct
{
    uint32 regOffset;  //!< CGC register address offset
    uint32 enableMask; //!< Mask for the enable field
    uint32 hwctlMask;  //!< Mask for HW control field
    uint32 stateMask;  //!< Mask for clock state field. If 0 - state check is skipped
} AsicClkHwIoInfoType;

/**
 * @struct AsicClkDesctiptorType - defines a descriptor for a particular clock.
 * The descriptor will be used by ClkRegime Wrapper to determine proper handling
 * of the requests for a particular clock.
 */
typedef struct
{
    AdsppmClkIdType clkId;                  //!< Internal ADSPPM Clock Id
    AsicClkTypeType clkType;                //!< Type of the clock as exposed by ClkRegime
    AsicClkCntlTypeType clkCntlType;        //!< Type of clock control scheme
    AsicClkMemRetentionType clkMemCntlType; //!< How memory core is controlled
    union
    {
        AsicClkHwIoInfoType hwioInfo;          //!< Register information for HWIO type clocks
        char clkName[MAX_ADSPPM_CLIENT_NAME]; //!< Name of the clock as exposed by ClkRegime
    } clkInfo;
    AdsppmClkIdType clkSrcId;               //!< Clock Id for frequency setting if the clock is enable/disable only
    AdsppmMemIdType memoryId;               //!< Memory ID if the clock is sourcing memory
} AsicClkDescriptorType;

/**
 * @struct AsicClockArrayType - defines a generic structure wchich members are a
 * pointer to an array of clock Ids and the number of clock IDs in it.
 */
typedef struct
{
    const uint32 numClocks;
    const AdsppmClkIdType *pClocks;
} AsicClockArrayType;

/**
 * @enum AsicBusPortConnectionType - specifies which of the buses the port is
 * connected to. The enum is a bitmap and the values can be combined together
 * if an endpoint has more than one connection.
 */
typedef enum
{
    AsicBusPort_None = 0,
    AsicBusPort_AhbE_M = 0x1,     //!< Master on AHB-E
    AsicBusPort_AhbE_S = 0x2,     //!< Slave on AHB-E
    AsicBusPort_AhbI_M = 0x4,     //!< Master on AHB-I
    AsicBusPort_AhbI_S = 0x8,     //!< Slave on AHB-I
    AsicBusPort_AhbX_M = 0x10,    //!< Master on AHB-X
    AsicBusPort_AhbX_S = 0x20,    //!< Slave on AHB-X
    AsicBusPort_Ahb_Any = 0x3F,   //!< Any connection on AHB
    AsicBusPort_Ext_M = 0x40,     //!< External Master
    AsicBusPort_Ext_S = 0x80,     //!< External Slave
    AsicBusPort_Ext_Any = 0xC0,   //!< Any external connection
    AsicBusPort_Any = 0xFF
} AsicBusPortConnectionType;

/**
 * @struct AsicBusPortDescriptorType - Bus port descriptor defining connections of the port and corresponding controls.
 */
typedef struct
{
    AdsppmBusPortIdType port;
    AsicBusPortConnectionType portConnection; //!< Defines a bitmap of port connections to the buses
    AdsppmClkIdType busClk;                   //!< Bus clock for the port. Applicable only for AHB connected ports
    AsicClockArrayType regProgClocks;         //!< Ponter to array of clocks for register programming
    union
    {
        ICBId_MasterType icbarbMaster;        //!< ICBArb master definition for the port. Applicable only for external master connections
        ICBId_SlaveType  icbarbSlave;         //!< ICBArb slave definition for the port. Applicable only for external slave connections
    } icbarbId;                               //!< Descriptor for the port.  ICB definitions for external ports and for NOC port for internal ones
    AdsppmBusPortIdType accessPort;           //!< Access port. Port used to access from/to external NOC
} AsicBusPortDescriptorType;

/**
 * @struct AsicBusPortArrayType - defines a generic structure wchich members are a
 * pointer to an array of bus port Ids and the number of bus port IDs in it.
 */
typedef struct
{
    const uint32 numBusPorts;
    const AdsppmBusPortIdType *pBusPorts;
} AsicBusPortArrayType;

typedef struct
{
    uint64 bwThreshold;         // Honest BW threshold (bytes/sec)
    uint64 bwAbVote;            // Compensated Ab vote
    uint64 bwIbVote;            // Compensated Ib
    uint32 latencyVote;         // latency in (nanoSec), 0 means no latency vote needed
    uint32 snocFloorVoteMhz;    //snoc floor vote
    uint32 honestFlag;          // Flag indicating Honest/dishonest voting
} AsicCompensatedDdrBwTableEntryType;

typedef struct
{
    uint64 bwThreshold;         // Honest BW threshold (bytes/sec)
    uint64 snocVoteAb;          // snoc Floor Vote from ADSPPM (Mhz)
    uint64 snocVoteIb;          // snoc Floor Vote from ADSPPM (Mhz)
	uint32 latencyVote;         // latency in (nanoSec), 0 means no latency vote needed
    uint32 falTolerance;        // Fal Tolerance value from ADSPPM
    uint32 idleTime;            // Idle Time value from ADSPPM  
    uint32 honestFlag;          // Flag indicating Honest/dishonest voting
} AsicSnocVoteTableEntryType;


/*************************************************************************************************/
/*                            Power definitions                                                  */
/*************************************************************************************************/
typedef enum
{
    AsicPowerDomain_AON = 0,       //!< Always On
    AsicPowerDomain_Adsp = 1,      //!< ADSP (Q6SS) Power Island
    AsicPowerDomain_LpassCore = 2, //!< LPASS Core Power Island
    AsicPowerDomain_Lpm = 3,       //!< LPM memory core
    AsicPowerDomain_SRam = 4,      //!< Sensors RAM Memory core
    AsicPowerDomain_Ocmem = 5,     //!< OCMEM
    AsicPowerDomain_Hvx = 6,       //!< HVX
    AsicPowerDomain_ComputeCore=7,
    AsicPowerDomain_EnumMax
} AsicPowerDomainType;

typedef struct
{
    AsicPowerDomainType pwrDomain;
    char pwrDomainName[MAX_ADSPPM_CLIENT_NAME];
    AsicHwRscIdType pwrDomainType;
    AdsppmClkIdType clkId;
    DalIPCIntInterruptType intrReinitTrigger;   //!< Interrupt Id to trigger re-initialization (TZ) after power up
    uint32 intrReinitDone;                      //!< Interrupt Id acknowledging re-init (TZ) completion after power up
    AsicClockArrayType securityClocks;          //!< Pointer to the array of security clocks for the domain
} AsicPowerDomainDescriptorType;

/*************************************************************************************************/
/*                            Core definitions                                                   */
/*************************************************************************************************/
/**
 * @struct AsicCoreDescType - Defines entry in the ASIC config table defining Core Descriptor
 */
typedef struct
{
    AdsppmCoreIdType coreId;
    AsicHwRscIdType hwResourceId[Adsppm_Rsc_Id_Max]; //!< Array of HW Resource Ids by Resource Id
    AsicPowerDomainType pwrDomain;
    AsicClockArrayType coreClockInstances;
    AsicBusPortArrayType masterBusPortInstances;
    AsicBusPortArrayType slaveBusPortInstances;
    AdsppmInstanceIdType numInstances;
    //TODO: include aggregation function pointer here?
    //Asic_Rsc_AggregationFnType aggregateFn;
} AsicCoreDescType;

/*************************************************************************************************/
/*                            Memory type definitions                                            */
/*************************************************************************************************/
/**
 * @struct AsicMemDescriptorType - Defines entry in the ASIC config table defining Memory Descriptor
 */
typedef struct
{
    AdsppmMemIdType memId;
    AsicPowerDomainType pwrDomain;
} AsicMemDescriptorType;

/*****************************************************************************/
/*                  Cache partitioning configuration definitions             */
/*****************************************************************************/

typedef struct
{
    MmpmClientClassType aggregateClass;
    qurt_cache_partition_size_t mainPartSize;
} AsicCachePartitionConfigType;

typedef struct
{
    MmpmClientClassType clientClass;
    uint32   useCaseId;
} AsicSystemCacheConfigType;

typedef struct
{
    const uint32 numEntries;
    const AsicCachePartitionConfigType* pConfigEntries;
} AsicCachePartitionConfigTableType;

typedef struct
{
    // Concurrency is considered active if the number of ADSP->DDR BW voters
    // is greather than this threshold
    uint32 adspDdrConcurrencyVotersThreshold;
    // Ab DDR BW threshold value
    uint64 adspDdrCompensatedBWThreshold;
    // The factor to be applied to ADSP->DDR BW votes when concurrency is
    // active. This value will be divided by 256.
    uint32 adspDdrConcurrencyFactorArray[3];
} AsicBwConcurrencySettingsType;

/******************************************************************************************/
/*                            Feature definitions                                         */
/******************************************************************************************/
typedef enum
{
    AsicFeatureId_Adsp_Clock_Scaling,
    AsicFeatureId_Adsp_LowTemp_Voltage_Restriction,
    AsicFeatureId_Adsp_PC,
    AsicFeatureId_Ahb_Scaling,               //!< Feature to indicate AHBI/X scaling based on BW Votes
    AsicFeatureId_Ahb_Sw_CG,
    AsicFeatureId_Ahb_DCG,
    AsicFeatureId_LpassCore_PC,
    AsicFeatureId_LpassCore_PC_TZ_Handshake,
    AsicFeatureId_Bus_Scaling,
    AsicFeatureId_CoreClk_Scaling,           //!< Feature to control core clocks. If disabled the clocks will be forced to max
    AsicFeatureId_Min_Adsp_BW_Vote,          //!< Feature to issue min ext BW vote to support min MIPS
    AsicFeatureId_InitialState,              //!< Feature to set up initial state during init. If enabled ADSPPM will bring down all resources to the min state
    AsicFeatureId_TimelineOptimisationMips,  //!< Feature to enable MIPS vote optimization for periodic clients
    AsicFeatureId_TimelineOptimisationBw,    //!< Feature to enable BW vote optimization for periodic clients
    AsicFeatureId_TimelineOptimisationAhb,   //!< Feature to enable AHB clock optimization for periodic clients
    AsicFeatureId_LpassClkSleepOptimization, //!< Feature to enable gating of some of the LPASS clocks when Q6 is going into sleep
    AsicFeatureId_LPMRetention,              //!< Feature to control LPM retention
    AsicFeatureId_DomainCoreClocks,          //!< Feature to enable/disable domain core clocks voting
    AsicFeatureId_CachePartitionControl,     //!< Feature to enable/disable Dynamic cache partitioning
    AsicFeatureId_DcvsControl,               //!< Feature to enable/disable DCVS from ADSPPM
    AsicFeatureId_AhbE_Q6Scaling,            //!< Feature to indicate AHBE scaling based on Q6 Frequency
    AsicFeatureId_CacheSizeMPPSThreshold,    //!< Feature to enable/disable MPPS-based cache size
    AsicFeatureId_CacheSizeBWScaling,        //!< Feature to enable/disable scaling of external compensated BW vote
    AsicFeatureId_SystemCacheSupport,        //!< feature to support System Cache
    AsicFeatureId_ComputeDSP,                //!< Feature to indicate this is TuringSS, Compute Processor
    AsicFeatureId_SlpiDSP,                   //!< Feature to indicate this is SensorsSS, Slpi Processor
    AsicFeatureId_RPMh,                      //!< Feature to indicate this is RPMh architecture
    AsicFeatureId_Alc,                       //!< Feature to indicate Alc feature support
    AsicFeatureId_AhbIX_Q6Scaling,           //!< Feature to indicate AHB-IX scaling based on Q6 Frequency
    AsicFeatureId_AhbE_VoteScaling,          //!< Feature to indicate AHBE scaling based on BW Votes
    AsicFeatureId_LpassSlpiDSP,              //!< Feature to indicate that LPASS & Sensors are part of same subsystem
    AsicFeatureId_enum_max,
    AsicFeatureId_force_32bit = 0x7FFFFFFF
} AsicFeatureIdType;

typedef enum
{
    AsicFeatureState_Disabled, //!< Feature completely disabled
    AsicFeatureState_Enabled,  //!< Feature enabled
    AsicFeatureState_Limited   //!< Feature behaviour limited
} AsicFeatureStateType;

typedef struct
{
    AsicFeatureStateType state;
    uint64 min;
    uint64 max;
} AsicFeatureDescType;

typedef struct
{
    uint32 cpp[4];      // [0] one   thread active CPP * 1000
                        // [1] two   thread active CPP * 1000
                        // [2] three thread active CPP * 1000
                        // [3] four  thread active CPP * 1000
} AsicCppFactorsType;

typedef struct
{
    uint32 oneThreadActive;
    uint32 twoThreadActive;
    uint32 threeThreadActive;
    uint32 fourThreadActive;
} AsicThreadLoadingInfoType;

typedef struct
{
    const int numElements;
    const AsicThreadLoadingInfoType *pRows;
} AsicThreadLoadingInfoTableType;

#endif /* ASIC_SETTINGS_H_ */
