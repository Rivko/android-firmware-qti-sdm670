/*
* Copyright (c) 2015 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
@file: asic_config_mgr.c
@brief: File contains definitions of all generic ASIC Configuration Manager's
        functions.

$Header: //components/rel/core.qdsp6/2.1/power/qdsp_pm/src/driver/qdsp_pm_mgr/request_mgr/asic_config_mgr.c#4 $
*/

#include "adsppm.h"
#include "adsppm_utils.h"
#include "asic.h"
#include "asic_internal.h"
#include "DALSys.h"
#include "DDIChipInfo.h"
#include "DALDeviceId.h"
#include "asic_common_func.h"
#include "qurt_memory.h"


AsicConfigType g_AsicConfig; //!< global structure holding ASIC configuration

#define ASIC_SCLK_FREQ_KHZ 32
#define ASIC_QCLK_FREQ_KHZ 19200
#define ASIC_MPPS_TO_MIPS_MARGIN 1

/**
 * this global variable is used for change debug level on
 * runtime to get adsppm ulog default ulog level is 3.
 * #define ADSPPM_LOG_LEVEL_PROFILE  0   profiling log message, set gDEBUGLEVEL = ADSPPM_LOG_LEVEL_MAX to get this debug level
 * #define ADSPPM_LOG_LEVEL_ERROR    1   Error log messages
 * #define ADSPPM_LOG_LEVEL_WARNING  2   Warnings
 * #define ADSPPM_LOG_LEVEL_INFO     3   Critical info messages
 * #define ADSPPM_LOG_LEVEL_INFO_EXT 4   Extended info messages
 * #define ADSPPM_LOG_LEVEL_FN_ENTRY 5   Function entry/exit messages
 * #define ADSPPM_LOG_LEVEL_DEBUG    6   All debug messages
 * #define ADSPPM_LOG_LEVEL_MAX      7
 */
static volatile uint32 gDEBUGLEVEL = 0;

/**
 * this global variable is used for enable/disable ADSPPM power feature before MMPM_Init
 *  gADSPPMPOWERFEATURE = 0xff  get feature definition from config file
 *  gADSPPMPOWERFEATURE = 0     disable all adsppm power feature
 *  gADSPPMPOWERFEATURE = 1     enable all adsppm power feature except some features not supported.
 *
 * @author alisonc (3/1/2013)
 */
static volatile uint32 gADSPPMPOWERFEATURE = 0xff;


Adsppm_Status asicInitConfigData(AsicConfigType *pConfig)
{
    Adsppm_Status result = Adsppm_Status_Success;

    // Get Chip configuration from Device Config image
    result = asicGetAsicConfig(pConfig);

    // Fill in ASIC specific functions
    // Only default functions are used at the moment
    switch(pConfig->functionSet)
    {
    case AsicFnSetId_Default:
        pConfig->pFn_GetBWFromMips = getBWFromMips;
        pConfig->pFn_GetClockFromMips = getClockFromMips;
        pConfig->pFn_GetClockFromBW = getClockFromBW;
        pConfig->pFn_BusBWAggregate = aggregateBusBW;
        break;

    default:
        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
            "ERROR: Unsupported function set ID %u", pConfig->functionSet);
        result = Adsppm_Status_Failed;
        break;
    }

    return result;
}

static const AsicCppFactorsType* asicLookupCppFactors(
    AsicCppFactorsTableType* cppTable, uint32 mppsTotal)
{
    const AsicCppFactorsType* result = NULL;
    if((cppTable != NULL) && (cppTable->numElements > 0))
    {
        uint32 index = 0;
        while(index < cppTable->numElements)
        {
            if(mppsTotal <= cppTable->pRows[index].mppsThreshold)
            {
                result = &cppTable->pRows[index].cpp;
                ADSPPM_LOG_PRINTF_4(ADSPPM_LOG_LEVEL_DEBUG,
                    "selected CPPs: [%u %u %u %u]",
                    result->cpp[0],
                    result->cpp[1],
                    result->cpp[2],
                    result->cpp[3]);
                break;
            }
            index += 1;
        }
    }
    return result;
}

const AsicCppFactorsType* ACM_GetAudioCppFactors(uint32 mppsTotal)
{
    return asicLookupCppFactors(g_AsicConfig.audioCppFactorsTable, mppsTotal);
}

const AsicCppFactorsType* ACM_GetVoiceCppFactors(uint32 mppsTotal)
{
    return asicLookupCppFactors(g_AsicConfig.voiceCppFactorsTable, mppsTotal);
}


/**
 * @fn ACM_Init - initializes global ASIC configuration structure
 *
 * Function gets ASIC information (Family, ID, version) from ChipInfo.
 * The ASIC information is then used to populate the global configuration with
 * ASIC-specific data.
 *
 * @return completion status
 */
Adsppm_Status ACM_Init()
{
    Adsppm_Status result = Adsppm_Status_Success;
    
    memset(&g_AsicConfig, 0, sizeof(g_AsicConfig));

    // Populate chip version
    g_AsicConfig.chipVersion = DalChipInfo_ChipVersion();

    // Populate ASIC config with chip-specific data
    // All properties will be versioned in the device config XML,
    // so no need to know about chip family and revision
    result = asicInitConfigData(&g_AsicConfig);
   
    return result;
}

/**
 * @fn ACM_IsClkIdValid - get ClkId present in the global clkId array
 *
 * Function gets ...
 *
 * @return completion status
 */
boolean ACM_IsClkIdValid(AdsppmClkIdType clk)
{
    boolean present  = FALSE;
    uint32 idx =0;

	if((AdsppmClk_None < clk) && (clk < AdsppmClk_EnumMax))
	{
		for (idx = 0; idx < g_AsicConfig.clocks->numElements; idx++)
		{
			if (clk == g_AsicConfig.clocks->pClocks[idx].clkId)
			{
				present = TRUE;
			    break;
		   }
		}
	}

    return present;
}

/**
 * @fn ACM_GetCoreIdIdx - get CoreId index in global coreId array
 *
 * Function gets ...
 *
 * @return completion status
 */
Adsppm_Status ACM_GetCoreIdIdx(AdsppmCoreIdType core , uint32 *coreIdx)
{
    Adsppm_Status result  = Adsppm_Status_Failed;
    uint32 idx =0;

	if((Adsppm_Core_Id_None < core) && (core < Adsppm_Core_Id_Max))
	{
		for (idx = 0; idx < g_AsicConfig.cores->numElements; idx++)
		{
			if (core == g_AsicConfig.cores->pCores[idx].coreId)
			{
				*coreIdx =idx;
				result = Adsppm_Status_Success;
				break;
			}
		}
	}
	if (Adsppm_Status_Failed == result)
		ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR, "CoreID %d not found!!!!!!", core);

	return result;
}

/**
 * @fn ACM_GetClkIdx - get ClkId index in global clkId array
 *
 * Function gets ...
 *
 * @return completion status
 */
Adsppm_Status ACM_GetClkIdx(AdsppmClkIdType clk , uint32 *clkIdx)
{
    Adsppm_Status result  = Adsppm_Status_Failed;
    uint32 idx = 0;

	if((AdsppmClk_None < clk) && (clk < AdsppmClk_EnumMax))
	{
		for (idx = 0; idx < g_AsicConfig.clocks->numElements; idx++)
		{
			if (clk == g_AsicConfig.clocks->pClocks[idx].clkId)
			{
				*clkIdx = idx;
				result = Adsppm_Status_Success;
				break;
			}
		}
	}
	if (Adsppm_Status_Failed == result)
		ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR, "clkId %d not found!!!!!!", clk);

    return result;
}

/**
 * @fn ACM_GetHwRscId - get HwResource ID
 *
 * Function gets ...
 *
 * @return completion status
 */
AsicHwRscIdType ACM_GetHwRscId(AdsppmCoreIdType core, AdsppmRscIdType rsc)
{
    AsicHwRscIdType resType = AsicRsc_None;
    uint32 coreIdx= 0;

    if (ACM_GetCoreIdIdx(core , &coreIdx) == Adsppm_Status_Failed)
	    return resType;

    if((rsc > Adsppm_Rsc_Id_None) && (rsc < Adsppm_Rsc_Id_Max))
    {
        resType = g_AsicConfig.cores->pCores[coreIdx].hwResourceId[rsc];
    }
    return resType;
}


/* TODO switch to using this instead of direct function calls
Asic_Rsc_AggregationFnType ACM_GetAggregationFn(AsicHwRscIdType, AdsppmCoreIdType)
{
    return NULL;
}*/


/**
 * Request number of supported routes (master/slave pairs) for external BW requests
 * @return number of supported routes (master/slave pairs) for external BW requests
 */
int ACMBus_GetNumberOfExtRoutes(void)
{
    return g_AsicConfig.extBusRoutes->numElements;
}


/**
 * Request supported routes (master/slave pairs) for external BW requests
 * @param pExtRoutes - [OUT] array to be filled with supported routes (master/slave pairs) for external BW requests
 */
void ACMBus_GetExtRoutes(AsicBusExtRouteType *pExtRoutes)
{
    int i = 0;
    const AsicBusPortDescriptorType *pMasterBusPortDescriptor = NULL;
    const AsicBusPortDescriptorType *pSlaveBusPortDescriptor = NULL;
    if(NULL != pExtRoutes)
    {
        for(i = 0; i < g_AsicConfig.extBusRoutes->numElements; i++)
        {
            pExtRoutes[i].masterPort.adsppmMaster = g_AsicConfig.extBusRoutes->pRoutes[i].masterPort;
            pMasterBusPortDescriptor = ACMBus_GetPortDescriptor(pExtRoutes[i].masterPort.adsppmMaster);
            pExtRoutes[i].slavePort.adsppmSlave = g_AsicConfig.extBusRoutes->pRoutes[i].slavePort;
            pSlaveBusPortDescriptor = ACMBus_GetPortDescriptor(pExtRoutes[i].slavePort.adsppmSlave);
            if((NULL != pMasterBusPortDescriptor) && (NULL != pSlaveBusPortDescriptor))
            {
                pExtRoutes[i].masterPort.icbarbMaster = pMasterBusPortDescriptor->icbarbId.icbarbMaster;
                pExtRoutes[i].slavePort.icbarbSlave = pSlaveBusPortDescriptor->icbarbId.icbarbSlave;
            }
        }
    }
}


/**
 * Get core clock ID for corresponding core instance
 * @param coreId
 * @param instanceId
 * @param coreClock
 * @return if core clock is associated to this core, return associated core instance core clock ID, else no change
 */
AdsppmClkIdType ACMClk_GetInstanceCoreClockId(AdsppmCoreIdType coreId, AdsppmInstanceIdType instanceId, AdsppmClkIdType coreClock)
{
    AdsppmClkIdType newClock = coreClock;

    uint32 coreIdx= 0;

    if (ACM_GetCoreIdIdx(coreId , &coreIdx) == Adsppm_Status_Failed)
	    return newClock;

    if((Adsppm_Instance_Id_0 < instanceId) && (Adsppm_Instance_Id_Max > instanceId) &&
        (AdsppmClk_HwRsp_Core <= coreClock) && (AdsppmClk_EnumMax > coreClock))
    {
        if((g_AsicConfig.cores->pCores[coreIdx].coreClockInstances.numClocks > (instanceId - Adsppm_Instance_Id_0)) &&
            (g_AsicConfig.cores->pCores[coreIdx].coreClockInstances.pClocks[0] == coreClock))
        {
            newClock = g_AsicConfig.cores->pCores[coreIdx].coreClockInstances.pClocks[instanceId - Adsppm_Instance_Id_0];
        }
    }
    return newClock;
}


/**
 * Get master bus port ID for corresponding core instance
 * @param coreId
 * @param instanceId
 * @param masterBusPort
 * @return if master bus port is associated to this core, return associated core instance master bus port ID, else no change
 */
AdsppmBusPortIdType ACMBus_GetInstanceMasterBusPortId(AdsppmCoreIdType coreId, AdsppmInstanceIdType instanceId, AdsppmBusPortIdType masterBusPort)
{
    AdsppmBusPortIdType newMaster = masterBusPort;

    uint32 coreIdx= 0;

    if (ACM_GetCoreIdIdx(coreId , &coreIdx) == Adsppm_Status_Failed)
	    return newMaster;

    if((Adsppm_Instance_Id_0 < instanceId) && (Adsppm_Instance_Id_Max > instanceId) &&
        (AdsppmBusPort_None < masterBusPort) && (AdsppmBusPort_EnumMax > masterBusPort))
    {
        if((g_AsicConfig.cores->pCores[coreIdx].masterBusPortInstances.numBusPorts > (instanceId - Adsppm_Instance_Id_0)) &&
            (g_AsicConfig.cores->pCores[coreIdx].masterBusPortInstances.pBusPorts[0] == masterBusPort))
        {
            newMaster = g_AsicConfig.cores->pCores[coreIdx].masterBusPortInstances.pBusPorts[instanceId - Adsppm_Instance_Id_0];
        }
    }
    return newMaster;
}


/**
 * Get slave bus port ID for corresponding core instance
 * @param coreId
 * @param instanceId
 * @param slaveBusPort
 * @return if slave bus port is associated to this core, return associated core instance slave bus port ID, else no change
 */
AdsppmBusPortIdType ACMBus_GetInstanceSlaveBusPortId(AdsppmCoreIdType coreId, AdsppmInstanceIdType instanceId, AdsppmBusPortIdType slaveBusPort)
{
    AdsppmBusPortIdType newSlave = slaveBusPort;
    uint32 coreIdx= 0;

    if (ACM_GetCoreIdIdx(coreId , &coreIdx) == Adsppm_Status_Failed)
	    return newSlave;

    if((Adsppm_Instance_Id_0 < instanceId) && (Adsppm_Instance_Id_Max > instanceId) &&
        (AdsppmBusPort_None < slaveBusPort) && (AdsppmBusPort_EnumMax > slaveBusPort))
    {
        if((g_AsicConfig.cores->pCores[coreIdx].slaveBusPortInstances.numBusPorts > (instanceId - Adsppm_Instance_Id_0)) &&
            (g_AsicConfig.cores->pCores[coreIdx].slaveBusPortInstances.pBusPorts[0] == slaveBusPort))
        {
            newSlave = g_AsicConfig.cores->pCores[coreIdx].slaveBusPortInstances.pBusPorts[instanceId - Adsppm_Instance_Id_0];
        }
    }
    return newSlave;
}


/**
 * @fn ACMBus_GetCoreSlavePort - returns slave bus port associated with the core the client is registered for
 * @param coreId
 * @param instanceId
 * @return slave bus port ID associated with the registered core
 */
AdsppmBusPortIdType ACMBus_GetCoreSlavePort(AdsppmCoreIdType coreId, AdsppmInstanceIdType instanceId)
{
    AdsppmBusPortIdType port = AdsppmBusPort_None;

    uint32 coreIdx= 0;

    if (ACM_GetCoreIdIdx(coreId , &coreIdx) == Adsppm_Status_Failed)
	    return port;

    if((Adsppm_Instance_Id_None < instanceId) && (Adsppm_Instance_Id_Max > instanceId))
    {
        if(g_AsicConfig.cores->pCores[coreIdx].slaveBusPortInstances.numBusPorts > (instanceId - Adsppm_Instance_Id_0))
        {
            port = g_AsicConfig.cores->pCores[coreIdx].slaveBusPortInstances.pBusPorts[instanceId - Adsppm_Instance_Id_0];
        }
    }
    return port;
}


/**
 * Get register programming speed
 * @param Register Programming Match
 * @return Speed in bytes per sec
 */
uint64 ACMBus_GetRegProgSpeed(AdsppmRegProgMatchType match)
{
    uint64 speed = 0;
    if((match > Adsppm_RegProg_None) && (match < Adsppm_RegProg_Enum_Max))
    {
        speed = g_AsicConfig.regProgSpeeds->pRegProgSpeeds[match];
    }
    return speed;
}


/**
 * Request number of supported routes (master/slave pairs) for MIPS BW requests
 * @return number of supported routes (master/slave pairs) for MIPS BW requests
 */
const int ACMBus_GetNumberOfMipsBwRoutes(void)
{
    return g_AsicConfig.mipsBusRoutes->numElements;
}


/**
 * Request list supported routes (master/slave pairs) for MIPS BW requests
 * @return pointer to array of supported routes (master/slave pairs) for MIPS BW requests
 */
const AdsppmBusRouteType *ACMBus_GetMipsBwRoutes(void)
{
    return g_AsicConfig.mipsBusRoutes->pRoutes;
}

/**
 * @fn ACM_GetBusPortIdx - get BusPortId index in global busId array
 *
 * Function gets ...
 *
 * @return completion status
 */
Adsppm_Status ACM_GetBusPortIdx(AdsppmBusPortIdType port , uint32 *portIdx)
{
    Adsppm_Status result  = Adsppm_Status_Failed;
    uint32 idx =0;

	if((AdsppmBusPort_None <= port) && (port < AdsppmBusPort_EnumMax))
	{
		for (idx = 0; idx < g_AsicConfig.busPorts->numElements; idx++)
		{
			if (port == g_AsicConfig.busPorts->pPorts[idx].port)
			{
				*portIdx =idx;
				result = Adsppm_Status_Success;
				break;
			}
		}
    }
	if (Adsppm_Status_Failed == result)
		ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR, "portId %d not found!!!!!!", port);

	return result;
}

/**
 * Returns pointer to the the descriptor for the specified bus port.
 * @param port
 * @return pointer to the bus port descriptor
 */
const AsicBusPortDescriptorType *ACMBus_GetPortDescriptor(AdsppmBusPortIdType port)
{
    const AsicBusPortDescriptorType *pDesc = NULL;
    uint32 portIdx= 0;

    if (ACM_GetBusPortIdx(port , &portIdx) == Adsppm_Status_Failed)
	    return pDesc;

    pDesc = &(g_AsicConfig.busPorts->pPorts[portIdx]);

    return pDesc;
}


/**
 * @fn ACMBus_GetPortConnections
 * @brief Returns bus port connections for the specified bus port.
 * @param port - AdsppmBusPortIdType
 * @return bus port connections - AsicBusPortConnectionType
 */
AsicBusPortConnectionType ACMBus_GetPortConnections(AdsppmBusPortIdType port)
{
    const AsicBusPortDescriptorType* desc = ACMBus_GetPortDescriptor(port);
    return desc == NULL ? AsicBusPort_None : desc->portConnection;
}


/**
 * Get type of the clock
 * @param Clock Id
 * @return Type of the clock (enable/disable, frequency set, npa)
 */
AsicClkTypeType ACMClk_GetClockType(AdsppmClkIdType clk)
{
    AsicClkTypeType clkType = AsicClk_TypeNone;
    uint32 clkIdx= 0;

    if (ACM_GetClkIdx(clk , &clkIdx) == Adsppm_Status_Failed)
	    return clkType;

    clkType = g_AsicConfig.clocks->pClocks[clkIdx].clkType;

    return clkType;
}

/**
 * Request number of clock descriptors
 * @return number of clock descriptors
 */
const int ACMClk_GetNumberOfClockDescriptors(void)
{
    return g_AsicConfig.clocks->numElements;
}


/**
 * Get descriptor the clock by passing the index
 * @param Clock Index
 * @return Type of the clock (enable/disable, frequency set, npa)
 */
const AsicClkDescriptorType *ACMClk_GetClockDescriptor(uint32 clkDesIndex)
{
    const AsicClkDescriptorType *pDesc = NULL;

    if(clkDesIndex < g_AsicConfig.clocks->numElements)
    {
        pDesc = &(g_AsicConfig.clocks->pClocks[clkDesIndex]);
    }

    return pDesc;
}

/**
 * Get descriptor the clock clock ID
 * @param Clock Id
 * @return Type of the clock (enable/disable, frequency set, npa)
 */
const AsicClkDescriptorType *ACMClk_GetClockDescriptorById(AdsppmClkIdType clk)
{
    const AsicClkDescriptorType *pDesc = NULL;
    uint32 clkIdx= 0;

    if (ACM_GetClkIdx(clk , &clkIdx) == Adsppm_Status_Failed)
	    return pDesc;

    pDesc = &(g_AsicConfig.clocks->pClocks[clkIdx]);

    return pDesc;
}

/**
 * @fn ACM_IsPwrDomainValid - get pwrDomain present in the global pwr array
 *
 * Function gets ...
 *
 * @return completion status
 */
boolean ACM_IsPwrDomainValid(AsicPowerDomainType domain)
{
    boolean present  = FALSE;
    uint32 idx =0;

	if((AsicPowerDomain_AON < domain) && (domain < AsicPowerDomain_EnumMax))
	{
		for (idx = 0; idx < g_AsicConfig.pwrDomains->numElements; idx++)
		{
			if (domain == g_AsicConfig.pwrDomains->pPwrDomains[idx].pwrDomain)
			{
				present = TRUE;
			    break;
		   }
		}
	}

    return present;
}

/**
 * @fn ACM_GetPwrDomainIdx - get pwrDomain index in global pwrDomain array
 *
 * Function gets ...
 *
 * @return completion status
 */
Adsppm_Status ACM_GetPwrDomainIdx(AsicPowerDomainType domain , uint32 *domainIdx)
{
    Adsppm_Status result  = Adsppm_Status_Failed;
    uint32 idx = 0;

	if((domain > AsicPowerDomain_AON) && (domain < AsicPowerDomain_EnumMax))
	 {
		for (idx = 0; idx < g_AsicConfig.pwrDomains->numElements; idx++)
		{
			if (domain == g_AsicConfig.pwrDomains->pPwrDomains[idx].pwrDomain)
			{
				*domainIdx = idx;
				result = Adsppm_Status_Success;
				break;
			}
		}
	}
	if (Adsppm_Status_Failed == result)
		ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR, "PwrdomainId %d not found!!!!!!", domain);

    return result;
}

/**
 * Request number of pwrDomain descriptors
 * @return number of pwrDomain descriptors
 */
const int ACMClk_GetNumberOfPwrDomainDescriptors(void)
{
    return g_AsicConfig.pwrDomains->numElements;
}

/**
 * Get info about the power domain by passing index
 * @param Domain Index
 * @return Power Domain Info
 */
const AsicPowerDomainDescriptorType *ACMClk_GetPwrDomainDescriptor(uint32 domainIndex)
{
    const AsicPowerDomainDescriptorType *pDesc = NULL;

    if(domainIndex < AsicPowerDomain_EnumMax)
    {
        pDesc = &(g_AsicConfig.pwrDomains->pPwrDomains[domainIndex]);
    }
    return pDesc;
}

/**
 * Get info about the power domain by passing Id
 * @param Domain Id
 * @return Power Domain Info
 */
const AsicPowerDomainDescriptorType *ACMClk_GetPwrDomainDescriptorById(AsicPowerDomainType domain)
{
    const AsicPowerDomainDescriptorType *pDesc = NULL;
    uint32 domainIdx= 0;

    if (ACM_GetPwrDomainIdx(domain , &domainIdx) == Adsppm_Status_Failed)
	    return pDesc;

	pDesc = &(g_AsicConfig.pwrDomains->pPwrDomains[domainIdx]);

    return pDesc;
}

/**
 * Get power domain for the specified core
 * @param Core ID
 * @return Power Domain
 */
AsicPowerDomainType ACMPwr_GetPowerDomain(AdsppmCoreIdType core)
{
    AsicPowerDomainType domain = AsicPowerDomain_EnumMax;

    uint32 coreIdx= 0;

    if (ACM_GetCoreIdIdx(core , &coreIdx) == Adsppm_Status_Failed)
	    return domain;

    domain = g_AsicConfig.cores->pCores[coreIdx].pwrDomain;

    return domain;
}

/**
 * @fn ACM_GetMemIdIdx - get MemoryId index in global memory array
 *
 * Function gets ...
 *
 * @return completion status
 */
Adsppm_Status ACM_GetMemIdIdx(AdsppmMemIdType mem, uint32 *memIdx)
{
    Adsppm_Status result  = Adsppm_Status_Failed;
    uint32 idx =0;

    if((mem > Adsppm_Mem_None) && (mem < Adsppm_Mem_Max))
	{
		for (idx = 0; idx < g_AsicConfig.memories.numElements; idx++)
		{
			if (mem == g_AsicConfig.memories.pMemories[idx].pDescriptor->memId)
			{
				*memIdx =idx;
				result = Adsppm_Status_Success;
				break;
			}
		}
	}
	if (Adsppm_Status_Failed == result)
		ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR, "memId %d not found!!!!!!", mem);

	return result;
}

/**
 * @fn ACM_GetMemoryDescriptor: Get the descriptor based on mem type
 * @param Memory Id
 * @return Memory Descrip
 */
AsicMemDescriptorFullType *ACM_GetMemoryDescriptorById(AdsppmMemIdType mem)
{
    AsicMemDescriptorFullType *pDesc = NULL;
    uint32 memIdx= 0;

    if (ACM_GetMemIdIdx(mem , &memIdx) == Adsppm_Status_Failed)
	    return pDesc;

    pDesc = &(g_AsicConfig.memories.pMemories[memIdx]);

    return pDesc;
}


/**
 * Get address range for the specified memory
 * @param Memory ID
 * @return address range
 */
AsicAddressRangeType *ACM_GetVirtMemAddressRange(AdsppmMemIdType mem)
{
    AsicAddressRangeType *addrRange = NULL;
    uint32 memIdx= 0;

    if (ACM_GetMemIdIdx(mem , &memIdx) == Adsppm_Status_Failed)
	    return addrRange;

    addrRange = &g_AsicConfig.memories.pMemories[memIdx].virtAddr;

    return addrRange;
}

AsicCachePartitionConfigTableType* ACM_GetCachePartitionConfig()
{
    return g_AsicConfig.cachePartitionConfigTable;
}

/**
 * Set address range for the specified memory
 * @param mem - Memory ID
 * @param addr - start address
 * @param size - address range size
 * @return operation status
 */
AdsppmStatusType ACM_SetVirtMemAddressRange(AdsppmMemIdType mem, uint64 addr, uint32 size)
{
    AdsppmStatusType result = Adsppm_Status_BadParm;
    uint32 memIdx= 0;

    if (ACM_GetMemIdIdx(mem , &memIdx) == Adsppm_Status_Failed)
	    return result;

	g_AsicConfig.memories.pMemories[memIdx].virtAddr.startAddr = addr;
    g_AsicConfig.memories.pMemories[memIdx].virtAddr.size = size;
    result = Adsppm_Status_Success;

    return result;
}


/**
 * Get power domain for the specified memory
 * @param Memory ID
 * @return Power Domain
 */
AsicPowerDomainType ACMPwr_GetMemPowerDomain(AdsppmMemIdType mem)
{
    AsicPowerDomainType domain = AsicPowerDomain_EnumMax;
    uint32 memIdx= 0;

    if (ACM_GetMemIdIdx(mem , &memIdx) == Adsppm_Status_Failed)
	    return domain;

    domain = g_AsicConfig.memories.pMemories[memIdx].pDescriptor->pwrDomain;

    return domain;
}


/**
 * Get power resource type  for the specified memory
 * @param Memory ID
 * @return Power Domain
 */
AsicHwRscIdType ACMPwr_GetMemPowerType(AdsppmMemIdType mem)
{
    AsicHwRscIdType rsc = AsicRsc_None;
	const AsicPowerDomainDescriptorType *pPwrDesc = NULL;
    uint32 memIdx= 0;

    if (ACM_GetMemIdIdx(mem , &memIdx) == Adsppm_Status_Failed)
	    return rsc;

	pPwrDesc = ACMClk_GetPwrDomainDescriptorById(g_AsicConfig.memories.pMemories[memIdx].pDescriptor->pwrDomain);
	if(NULL != pPwrDesc)
		rsc = pPwrDesc->pwrDomainType;

    return rsc;
}


/**
 * Get descriptor for the specified core
 * @param Core ID
 * @return pointer to the descriptor for this core
 */
const AsicCoreDescType *ACM_GetCoreDescriptorById(AdsppmCoreIdType core)
{
    const AsicCoreDescType *pDesc = NULL;

    uint32 coreIdx= 0;

    if (ACM_GetCoreIdIdx(core , &coreIdx) == Adsppm_Status_Failed)
	    return pDesc;

    pDesc = &(g_AsicConfig.cores->pCores[coreIdx]);
    return pDesc;
}


Adsppm_Status ACM_GetBWFromMips(AdsppmMIPSToBWAggregateType *pMipsAggregateData)
{
    Adsppm_Status result = Adsppm_Status_Failed;
    if(NULL != g_AsicConfig.pFn_GetBWFromMips)
    {
        result = g_AsicConfig.pFn_GetBWFromMips(pMipsAggregateData);
    }
    return result;
}


Adsppm_Status ACM_GetClockFromMips(AdsppmMIPSToClockAggregateType *pMipsAggregateData)
{
    Adsppm_Status result = Adsppm_Status_Failed;
    if(NULL != g_AsicConfig.pFn_GetClockFromMips)
    {
        result = g_AsicConfig.pFn_GetClockFromMips(pMipsAggregateData);
    }
    return result;
}

uint32 ACM_GetMipsFromMpps(uint32 mppsTotal, uint32 numDominantThreads)
{
    uint32 mipsFromMpps = 0;
    AsicThreadLoadingInfoType info;
    AsicAudioVoiceCppTrendFactorsType *cppFactors = g_AsicConfig.audioVoiceCppFactors;
    AsicThreadLoadingInfoTableType *table = g_AsicConfig.threadLoadingInfoTable;

    if((numDominantThreads > 0) &&
        (table != NULL) &&
        (table->numElements > 0) &&
        (cppFactors != NULL)
        )
    {
        info.oneThreadActive =
            (mppsTotal * table->pRows[numDominantThreads - 1].oneThreadActive)/100;
        info.twoThreadActive =
            (mppsTotal * table->pRows[numDominantThreads - 1].twoThreadActive)/100;
        info.threeThreadActive =
            (mppsTotal * table->pRows[numDominantThreads - 1].threeThreadActive)/100;
        info.fourThreadActive =
            (mppsTotal * table->pRows[numDominantThreads - 1].fourThreadActive)/100;

        mipsFromMpps =
            (info.oneThreadActive   * cppFactors->pData[0]) +
            (info.twoThreadActive   * cppFactors->pData[1]) +
            (info.threeThreadActive * cppFactors->pData[2]) +
            (info.fourThreadActive  * cppFactors->pData[3]) ;

        // Scale Mips, as CPP used above is *1000
        mipsFromMpps /= 1000;

        // Additional Margin, covered by ADSPPM
        mipsFromMpps *= ASIC_MPPS_TO_MIPS_MARGIN;
    }

    ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_INFO,
        "MPPS: %u -> MIPS %u", mppsTotal, mipsFromMpps);

    return mipsFromMpps;
}

Adsppm_Status ACM_GetClockFromBW(uint32 *pClock, AdsppmBusBWDataIbAbType *pAHBBwData)
{
    Adsppm_Status result = Adsppm_Status_Failed;
    if(NULL != g_AsicConfig.pFn_GetClockFromBW)
    {
        result = g_AsicConfig.pFn_GetClockFromBW(pClock, pAHBBwData);
    }
    return result;
}

  uint32 ACM_GetThreadLeakage(uint32 thread)
  {
      if (thread > g_AsicConfig.threadLeakage->numElements) {
          ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_WARNING,
                  "Thread#%d greater than MAX Thread#%d",
                  thread, g_AsicConfig.threadLeakage->numElements);
          return 0;
      }
      return g_AsicConfig.threadLeakage->leakage[thread];
  }

void ACM_GetCompensatedDdrBwTableEntry(uint64 vote, AsicCompensatedDdrBwTableEntryType *pTableEntry)
{
    uint32 index;
    AsicCompensatedDdrBwTableType *table = g_AsicConfig.compensatedDdrBwTable;

    if(NULL != pTableEntry)
    {
        memset(pTableEntry, 0, sizeof(AsicCompensatedDdrBwTableEntryType));
        pTableEntry->bwAbVote = vote;
        if((vote > 0) &&
            !IsPeriodicUseCase() &&
            (table != NULL) &&
            (table->numElements > 0)
            )
        {
            for(index = 0; index < table->numElements; index++)
            {
                if(vote <= table->pRows[index].bwThreshold)
                {
                    pTableEntry->bwThreshold = table->pRows[index].bwThreshold;
                    if (table->pRows[index].honestFlag == 1)
                    {
                        pTableEntry->bwAbVote = vote;
                    }
                    else
                    {
                        pTableEntry->bwAbVote = table->pRows[index].bwAbVote;
                    }
                    pTableEntry->latencyVote       = table->pRows[index].latencyVote;
                    pTableEntry->snocFloorVoteMhz  = table->pRows[index].snocFloorVoteMhz;
                    pTableEntry->honestFlag        = table->pRows[index].honestFlag;
                    pTableEntry->bwIbVote          = table->pRows[index].bwIbVote;
                    break;
                }
            }

            ADSPPM_LOG_PRINTF_6(ADSPPM_LOG_LEVEL_INFO,
                " Selected BW Entry Info : Honest Vote %llu bwThreshold %llu Compensated bwVote %llu",
                (uint32) vote, // two params for %llu
                (uint32) (vote >> 32),
                (uint32) pTableEntry->bwThreshold, // two params for %llu
                (uint32) (pTableEntry->bwThreshold >> 32),
                (uint32) pTableEntry->bwAbVote, // two params for %llu
                (uint32) (pTableEntry->bwAbVote >> 32));

            ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_INFO,
                " Selected BW Entry Info : latencyVote %u snocFloorVoteMhz %u",
                (uint32) pTableEntry->latencyVote,
                (uint32) pTableEntry->snocFloorVoteMhz);
        }
    }
}

void ACM_GetSnocVoteTableEntry(uint64 vote, AsicSnocVoteTableEntryType *pTableEntry)
{
    uint32 index;
    AsicSnocVoteTableType *table = g_AsicConfig.snocBwVoteTable;

    if(NULL != pTableEntry)
    {
        memset(pTableEntry, 0, sizeof(AsicSnocVoteTableEntryType));
        
        if((vote > 0) &&
            (table != NULL) &&
            (table->numElements > 0)
            )
        {
            for(index = 0; index < table->numElements; index++)
            {
                if(vote <= table->pRows[index].bwThreshold)
                {
                    pTableEntry->bwThreshold = table->pRows[index].bwThreshold;
                    pTableEntry->snocVoteAb  = table->pRows[index].snocVoteAb;
                    pTableEntry->snocVoteIb  = table->pRows[index].snocVoteIb;
					pTableEntry->latencyVote  = table->pRows[index].latencyVote;
                    pTableEntry->falTolerance = table->pRows[index].falTolerance;
                    pTableEntry->idleTime     = table->pRows[index].idleTime;
                    pTableEntry->honestFlag   = table->pRows[index].honestFlag;
                    break;
                }
            }


            ADSPPM_LOG_PRINTF_4(ADSPPM_LOG_LEVEL_INFO,
                " Snoc Selected BW Entry Info : snocVoteAb %llu snocVoteIb %llu",
                (uint32)(pTableEntry->snocVoteAb),
                (uint32) (pTableEntry->snocVoteAb >> 32),
                (uint32) (pTableEntry->snocVoteIb),
                (uint32) (pTableEntry->snocVoteIb >> 32));
        }
    }
}



void ACM_ApplyAdspDdrBwConcurrencyFactor(
    uint32 numVotes, uint64 maxBw, uint64* pAb, uint64* pIb, AsicCompensatedBWFudgeFactor *compensatedBWFudgeFactorValues)
{
    AsicBwConcurrencySettingsType* settings =
        g_AsicConfig.bwConcurrencySettings;

    compensatedBWFudgeFactorValues->floorFudgeFactor = 0;
    compensatedBWFudgeFactorValues->ceilingFudgeFactor = 0;

    ADSPPM_LOG_PRINTF_7(ADSPPM_LOG_LEVEL_INFO,
        "Before Concurrency Adjustment - Num of Voters %u Max BW %llu Ab %llu Ib %llu",
        (uint32) numVotes,
        (uint32) maxBw, // two params for %llu
        (uint32) (maxBw >> 32),
        (uint32) *pAb, // two params for %llu
        (uint32) (*pAb >> 32),
        (uint32) *pIb, // two params for %llu
        (uint32) (*pIb >> 32));

    // Concurrent Clients <= Threshold, fudgefactor = 1
    if(settings && (numVotes <= settings->adspDdrConcurrencyVotersThreshold))
    {
        *pAb = (*pAb * settings->adspDdrConcurrencyFactorArray[0])/ 256;
        *pIb = (*pIb * settings->adspDdrConcurrencyFactorArray[0])/ 256;
    }
    else if(settings && (*pAb < settings->adspDdrCompensatedBWThreshold) && (numVotes > settings->adspDdrConcurrencyVotersThreshold))
    {
        // Concurrent Clients > Threshold, maxbw/totalBw > 90% fudgefactor = 1.5
        if (((maxBw * 100)/ *pAb) >= 90 )
        {
            *pAb = (*pAb * settings->adspDdrConcurrencyFactorArray[1])/ 256;
            *pIb = (*pIb * settings->adspDdrConcurrencyFactorArray[1])/ 256;
        }
        // Concurrent Clients > Threshold, maxbw/totalBw < 90% fudgefactor = 2
        else
        {
            *pAb = (*pAb * settings->adspDdrConcurrencyFactorArray[2])/ 256;
            *pIb = (*pIb * settings->adspDdrConcurrencyFactorArray[2])/ 256;
        }
        //Ensure that floor/ceiling FF is zero
        compensatedBWFudgeFactorValues->floorFudgeFactor = 0;
        compensatedBWFudgeFactorValues->ceilingFudgeFactor = 0;
    }
    else if(settings)
    {
        //v2 algorithm of fudge factor has to be applied
        //In this case, only ceiling and floor FFs are updated. Real calculation is done afterwards
        compensatedBWFudgeFactorValues->floorFudgeFactor = settings->adspDdrConcurrencyFactorArray[1];
        compensatedBWFudgeFactorValues->ceilingFudgeFactor = settings->adspDdrConcurrencyFactorArray[2];

    }

    ADSPPM_LOG_PRINTF_8(ADSPPM_LOG_LEVEL_INFO,
        "After Concurrency Adjustment - Num of Voters %u Max BW %llu Ab %llu Ib %llu floorFF %u",
        (uint32) numVotes,
        (uint32) maxBw, // two params for %llu
        (uint32) (maxBw >> 32),
        (uint32) *pAb, // two params for %llu
        (uint32) (*pAb >> 32),
        (uint32) *pIb, // two params for %llu
        (uint32) (*pIb >> 32),
        (uint32) compensatedBWFudgeFactorValues->floorFudgeFactor);
}


AsicBwConcurrencySettingsType* ACM_GetAdspDdrBwConcurrencySettings(void)
{
	return g_AsicConfig.bwConcurrencySettings;
}

Adsppm_Status ACM_ApplyAhbBwCompensation(uint64 bwVoteIn, AsicBusType busType, uint64* pBwVoteOut)
{
    AdsppmStatusType result = Adsppm_Status_Success;
    uint32 index;
    AsicCompensatedAhbBwTableType *table = g_AsicConfig.compensatedAhbBwTable[busType];
    
    if(pBwVoteOut == NULL)
        return Adsppm_Status_BadParm;

    *pBwVoteOut = bwVoteIn;

    if((bwVoteIn > 0) &&
        (table != NULL) &&
        (table->numElements > 0))
    {
        for(index = 0; index < table->numElements; index++)
        {
            if(bwVoteIn <= table->pRows[index].bwThreshold)
            {
                uint64 lookupResult = table->pRows[index].bwVote;
                if(lookupResult != -1)
                {
                    *pBwVoteOut = lookupResult;
                    ADSPPM_LOG_PRINTF_5(ADSPPM_LOG_LEVEL_INFO,
                        "AHB BW compensation for Bus %u: aggregated vote %llu, "
                        "compensated %llu",
                        busType,
                        (uint32) bwVoteIn, // two params for %llu
                        (uint32) (bwVoteIn >> 32),
                        (uint32) lookupResult, // two params for %llu
                        (uint32) (lookupResult >> 32));
                }
                else
                {
                    // -1 result means output should be set to the honest
                    // vote; this is already set as default
                    // *pBwVoteOut = bwVoteIn;
                    ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_INFO,
                        "no AHB BW compensation");
                }
                break;
            }
        }
    }

    return result;
}

/**
 * @fn ACM_BusBWAggregate - Aggregate bw. So far the function is common for all chips
 * @param input: AdsppmBusBWDataType *pBwValue
 * @param output: AdsppmBusBWDataIbAbType *pAggregateBwIbAbValue
 * @return Adsppm_Status
 */
Adsppm_Status ACM_BusBWAggregate(AdsppmBusBWDataIbAbType *pAggregateBwIbAbValue, AdsppmBusBWDataType *pBwValue)
{
    Adsppm_Status result = Adsppm_Status_Failed;
    if(NULL != g_AsicConfig.pFn_BusBWAggregate)
    {
        result = g_AsicConfig.pFn_BusBWAggregate(pAggregateBwIbAbValue, pBwValue);
    }
    return result;
}

Adsppm_Status ACM_GetAhbFromAdspFreq(uint32 adspClockFreq, AsicBusType busType, uint32* ahbClockFreq)
{
    Adsppm_Status result = Adsppm_Status_Failed;
    AsicAdspToAhbFreqTableType* table = g_AsicConfig.adspToAhbFreqTable[busType]; ;
   
    if((ahbClockFreq != NULL) && (table != NULL) && (table->pRows != NULL))
    {
        uint32 index = 0;
        for(index = 0; index < table->numElements; index++)
        {
            if(adspClockFreq <= table->pRows[index].adspFreqHz)
            {
                break;
            }
        }
        // Use the highest AHB freq if ADSP freq is larger than any table entry
        if(index >= table->numElements)
            index = table->numElements - 1;
        *ahbClockFreq = table->pRows[index].ahbFreqHz;
        result = Adsppm_Status_Success;
    }
    return result;
}

/**
* Get MPPS threshold for different client classes
*/
Adsppm_Status ACM_GetMPPSThresholdfromClientClass(MmpmClientClassType clientClass, uint32* mppsThreshold)
{
    Adsppm_Status result = Adsppm_Status_Failed;
    AsicAdspCacheSizeMPPSThresholdTableType* table = g_AsicConfig.cachesizeMPPSThresholdTable;

    if(mppsThreshold)
        {
            *mppsThreshold = 0;
        }
    else
        return result;
    
    if((table != NULL) && (table->pRows != NULL))
    {
        uint32 index = 0;
        for(index = 0; index < table->numElements; index++)
        {
            if(clientClass == table->pRows[index].clientClass)
            {
                *mppsThreshold = table->pRows[index].mppsThreshold;            
                break;
            }
        }
       result = Adsppm_Status_Success;
    }
    return result;

}

/**
* Get System Cache UsecaseId from client class
* Input : Client Class; Aggregated client class 
*         Priority Id; mentioned in enum in mmpm.h
*         Usecase ID pointer
* Output : Adsppm_Status
*/
Adsppm_Status ACM_SystemCacheUsecaseIdfromClientClass(uint32 clientClass,uint32* usecaseId)
{
    Adsppm_Status result = Adsppm_Status_Failed;
    AsicAdspSystemCacheConfigTableType* table = g_AsicConfig.systemcacheConfigTable;

    if(usecaseId == NULL)
      {
        return result;        
      }

    ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,"In asic call, clientClass %u",clientClass);       

    *usecaseId = 0;

    if((table != NULL) && (table->pRows != NULL))
    {
        uint32 index = 0;
        for(index = 0; index < table->numElements; index++)
        {
            if((clientClass & table->pRows[index].clientClass))
            {
                *usecaseId = table->pRows[index].useCaseId;            
                result = Adsppm_Status_Success;
                break;
            }
        }
    }
   
    return result;
}

Adsppm_Status ACM_GetBWScalingFactorFromCacheSize(uint32 cachesize, uint32* scalingfactor)
{
    Adsppm_Status result = Adsppm_Status_Failed;
    AsicAdspCacheSizeBWScalingTableType* table = g_AsicConfig.cacheSizeBWscalingTable;
    if((table != NULL) && (table->pRows != NULL))
    {
        uint32 index = 0;
        for(index = 0; index < table->numElements; index++)
        {
            if(cachesize == table->pRows[index].cachesize)
            {
                break;
            }
        }
        //Error Check just making sure that there is a valid index
        if(index >= table->numElements)
           index--;
        *scalingfactor = table->pRows[index].scalingfactor;
        result = Adsppm_Status_Success;
    }
    else
        *scalingfactor = 100;
    	
    return result;

}
/**
 * Get feature enablement status for a particular feature
 */
AsicFeatureStateType ACM_GetFeatureStatus(AsicFeatureIdType featureId)
{
    return g_AsicConfig.features[featureId].state;
}


/**
 * Get feature descriptor for a particular feature
 */
AsicFeatureDescType *ACM_GetFeatureDescriptor(AsicFeatureIdType featureId)
{
    return &g_AsicConfig.features[featureId];
}


/**
 * Adjust parameter value based on feature enablement and configuration data
 * @param featureId - feature ID.
 * @param value - value, which needs to be adjusted
 * @return - adjusted value
 */
uint64 ACM_AdjustParamValue(AsicFeatureIdType featureId, uint64 value)
{
    AsicFeatureDescType *pDesc = ACM_GetFeatureDescriptor(featureId);
    uint64 result = value;

    switch(pDesc->state)
    {
    case AsicFeatureState_Enabled:
    case AsicFeatureState_Limited:
        //apply min/max limitation
        //Limit from the top
        result = MIN(value, pDesc->max);
        //Limit from bottom
        result = MAX(result, pDesc->min);
        break;

    case AsicFeatureState_Disabled:
        //use max for the parameter
        result = pDesc->max;
        break;

    default:
        break;
    }

    return result;
}


uint32 ACM_GetDebugLevel(void)
{
    return g_AsicConfig.debugLevel;
}


/**
 * Convert time in ms to sclk
 * @param ms - time in ms.
 * @return - number of sclks
 */
uint64 ACM_ms_to_sclk(uint64 ms)
{
    return ms * ASIC_SCLK_FREQ_KHZ;
}


/**
 * Convert time in us to qclk (QTimer clock ticks)
 * @param us - time in us.
 * @return - number of qclks
 */
uint64 ACM_us_to_qclk(uint64 us)
{
    return (us * ASIC_QCLK_FREQ_KHZ)/1000;
}


uint32 ACM_GetHwThreadNumber(void)
{
    return g_AsicConfig.adspHwThreadNumber;
}


/**
 * Get LPASS HWIO base address
 * @return HWIO base address
 */
AsicHwioRegRangeType *ACM_GetLpassRegRange(void)
{
   return g_AsicConfig.lpassRegRange;
}


AsicHwioRegRangeType *ACM_GetL2ConfigRegRange(void)
{
   return g_AsicConfig.l2ConfigRegRange;
}

/**
 * Get a timeout value in ms for MIPS/BW optimization LPR
 * @return timeout value in ms
 */
uint64 ACM_GetLprVoteTimeoutValue(void)
{
    return g_AsicConfig.adsppmLprTimeoutValue;
}


static AsicHwioRegRangeType defaultHwioBase = {0, 0, 0};


#define DAL_GET_PROPERTY_VALUE_WITH_CHECK(propertyString)                                                      \
{                                                                                                              \
    if(result == Adsppm_Status_Success)                                                                        \
    {                                                                                                          \
        if(DAL_SUCCESS != DALSYS_GetPropertyValue(adsppm_property, propertyString, 0, &system_property_var))   \
        {                                                                                                      \
            ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "Unable to retrieve " propertyString " from config");  \
            result = Adsppm_Status_Failed;                                                                     \
        }                                                                                                      \
    }                                                                                                          \
}


#define DAL_GET_PROPERTY_VALUE_NO_CHECK(propertyString)                                                           \
{                                                                                                                 \
    dalresult = DALSYS_GetPropertyValue(adsppm_property, propertyString, 0, &system_property_var);                \
    if(DAL_SUCCESS != dalresult)                                                                                  \
    {                                                                                                             \
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, propertyString " is not specified in config, using default"); \
    }                                                                                                             \
}


#define CHECK_ARRAY_SIZE(a, b)                                                 \
{                                                                              \
    if(b != a)                                                                 \
    {                                                                          \
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, #a " is not of size " #b); \
        result = Adsppm_Status_Failed;                                         \
    }                                                                          \
}


Adsppm_Status asicGetAsicConfig(AsicConfigType *pConfig)
{
    Adsppm_Status result = Adsppm_Status_Success;
    DALSYSPropertyVar system_property_var;
    DALResult dalresult = DAL_SUCCESS;
    AsicFeatureStateType featureState = AsicFeatureState_Disabled;
    AsicFeatureIdType featureIndex = AsicFeatureId_Adsp_Clock_Scaling;
    int i = 0;

    // Set default values
    pConfig->debugLevel = ADSPPM_LOG_LEVEL;
    pConfig->adspHwThreadNumber = ADSP_HW_THREAD_NUMBER_DEFAULT;
    pConfig->adsppmLprTimeoutValue = ADSPPM_LPR_VOTE_TIMEOUT_DEFAULT;
    pConfig->functionSet = AsicFnSetId_Default;
    pConfig->lpassRegRange = &defaultHwioBase;
    pConfig->l2ConfigRegRange = &defaultHwioBase;

    // Declare property handle variable
    DALSYS_PROPERTY_HANDLE_DECLARE(adsppm_property);
    // Get the property handle
    DALSYS_GetDALPropertyHandleStr(ASIC_CONFIG_DEVICE_NAME, adsppm_property);

    DAL_GET_PROPERTY_VALUE_NO_CHECK("FUNCTION_SET")
    if(dalresult == DAL_SUCCESS)
    {
        pConfig->functionSet = system_property_var.Val.dwVal;
    }

    if(!gDEBUGLEVEL)
    {
        DAL_GET_PROPERTY_VALUE_NO_CHECK("DEBUG_LEVEL")
        if(dalresult == DAL_SUCCESS)
        {
            pConfig->debugLevel = system_property_var.Val.dwVal;
        }
    }
    else
    {
        if(gDEBUGLEVEL < ADSPPM_LOG_LEVEL_MAX)
        {
            pConfig->debugLevel = gDEBUGLEVEL;
            ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
                "DEBUG_LEVEL is set to %u", pConfig->debugLevel);
        }
        else if(ADSPPM_LOG_LEVEL_MAX == gDEBUGLEVEL)
        {
            pConfig->debugLevel = 0;
            ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
                "DEBUG_LEVEL is set to %u, profiling", pConfig->debugLevel);
        }
        else
        {
            ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
                "gDEBUGLEVEL setting is wrong = %u", gDEBUGLEVEL);
        }
    }

    DAL_GET_PROPERTY_VALUE_NO_CHECK("THREAD_NUMBER")
    if(dalresult == DAL_SUCCESS)
    {
        pConfig->adspHwThreadNumber = system_property_var.Val.dwVal;
    }

    DAL_GET_PROPERTY_VALUE_NO_CHECK("OVERHANG_VOTE_TIMEOUT_MS")
    if(dalresult == DAL_SUCCESS)
    {
        // Convert ms into QTImer ticks
        pConfig->adsppmLprTimeoutValue =
            ACM_us_to_qclk(system_property_var.Val.dwVal * 1000);
    }

    DAL_GET_PROPERTY_VALUE_NO_CHECK("LPASS_REG_RANGE")
    if(dalresult == DAL_SUCCESS)
    {
        pConfig->lpassRegRange = (AsicHwioRegRangeType *) system_property_var.Val.pStruct;
    }

    DAL_GET_PROPERTY_VALUE_NO_CHECK("L2_CONFIG_REG_RANGE")
    if(dalresult == DAL_SUCCESS)
    {
        pConfig->l2ConfigRegRange = (AsicHwioRegRangeType *) system_property_var.Val.pStruct;
    }

    DAL_GET_PROPERTY_VALUE_WITH_CHECK("CORE_DESCRIPTORS")
    if(result == Adsppm_Status_Success)
    {
        pConfig->cores = (AsicCoreDescriptorArrayType *)system_property_var.Val.pStruct;

        for(i = 0; i < pConfig->cores->numElements; i++)
        {
            AdsppmInstanceIdType numInstances = pConfig->cores->pCores[i].numInstances;
            if(!(((numInstances == pConfig->cores->pCores[i].coreClockInstances.numClocks) ||
                    (0 == pConfig->cores->pCores[i].coreClockInstances.numClocks)) &&
                ((numInstances == pConfig->cores->pCores[i].masterBusPortInstances.numBusPorts) ||
                    (0 == pConfig->cores->pCores[i].masterBusPortInstances.numBusPorts)) &&
                ((numInstances == pConfig->cores->pCores[i].slaveBusPortInstances.numBusPorts) ||
                    (0 == pConfig->cores->pCores[i].slaveBusPortInstances.numBusPorts))))
            {
                ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
                    "invalid array size of resource instances, coreId=%u", i);
                result = Adsppm_Status_Failed;
                break;
            }
        }
    }

    DAL_GET_PROPERTY_VALUE_WITH_CHECK("MEMORY_DESCRIPTORS")
    if(result == Adsppm_Status_Success)
    {
        AsicMemDescriptorArrayType *pMemories = (AsicMemDescriptorArrayType *)system_property_var.Val.pStruct;
        pConfig->memories.numElements = pMemories->numElements;

        // Get addresses for each memory in config array
        for(i = 0; i < pConfig->memories.numElements; i++)
        {
            AsicMemDescriptorFullType *pMem = &pConfig->memories.pMemories[i];
            pMem->pDescriptor = &pMemories->pPwrDomainDesc[i];
            // Initialize address range with zeroes
            pMem->virtAddr.startAddr = 0;
            // We will not flush cache for memories with size of 0
            pMem->virtAddr.size = 0;
        }
    }

    DAL_GET_PROPERTY_VALUE_WITH_CHECK("CLOCK_DESCRIPTORS")
    if(result == Adsppm_Status_Success)
    {
        pConfig->clocks = (AsicClockDescriptorArrayType *)system_property_var.Val.pStruct;
    }

    DAL_GET_PROPERTY_VALUE_WITH_CHECK("BUS_PORT_DESCRIPTORS")
    if(result == Adsppm_Status_Success)
    {
        pConfig->busPorts = (AsicBusPortDescriptorArrayType *)system_property_var.Val.pStruct;
    }

    DAL_GET_PROPERTY_VALUE_WITH_CHECK("EXTERNAL_BUS_ROUTES")
    if(result == Adsppm_Status_Success)
    {
        pConfig->extBusRoutes = (AsicBusRouteArrayType *)system_property_var.Val.pStruct;
    }

    DAL_GET_PROPERTY_VALUE_WITH_CHECK("MIPS_BUS_ROUTES")
    if(result == Adsppm_Status_Success)
    {
        pConfig->mipsBusRoutes = (AsicBusRouteArrayType *)system_property_var.Val.pStruct;
    }

    DAL_GET_PROPERTY_VALUE_WITH_CHECK("REGISTER_PROGRAMMING_SPEEDS")
    if(result == Adsppm_Status_Success)
    {
        pConfig->regProgSpeeds = (AsicRegProgSpeedArrayType *)system_property_var.Val.pStruct;
        CHECK_ARRAY_SIZE(pConfig->regProgSpeeds->numElements, Adsppm_RegProg_Enum_Max)
    }

    DAL_GET_PROPERTY_VALUE_WITH_CHECK("POWER_DOMAIN_DESCRIPTORS")
    if(result == Adsppm_Status_Success)
    {
        pConfig->pwrDomains = (AsicPwrDescriptorArrayType *)system_property_var.Val.pStruct;
    }

    DAL_GET_PROPERTY_VALUE_WITH_CHECK("COMPENSATED_DDR_BW_TABLE")
    if(result == Adsppm_Status_Success)
    {
        pConfig->compensatedDdrBwTable = (AsicCompensatedDdrBwTableType *)system_property_var.Val.pStruct;
    }

    DAL_GET_PROPERTY_VALUE_WITH_CHECK("COMPENSATED_AHBIX_BW_TABLE")
    if(result == Adsppm_Status_Success)
    {
        pConfig->compensatedAhbBwTable[AsicBusType_AhbIx] = (AsicCompensatedAhbBwTableType *)system_property_var.Val.pStruct;
    }
    
    DAL_GET_PROPERTY_VALUE_WITH_CHECK("COMPENSATED_AHBE_BW_TABLE")
    if(result == Adsppm_Status_Success)
    {
        pConfig->compensatedAhbBwTable[AsicBusType_AhbE] = (AsicCompensatedAhbBwTableType *)system_property_var.Val.pStruct;
    }

    DAL_GET_PROPERTY_VALUE_WITH_CHECK("THREAD_LOADING_DATA")
    if(result == Adsppm_Status_Success)
    {
        pConfig->threadLoadingInfoTable = (AsicThreadLoadingInfoTableType *)system_property_var.Val.pStruct;
    }

    DAL_GET_PROPERTY_VALUE_WITH_CHECK("CPP_FACTORS")
    if(result == Adsppm_Status_Success)
    {
        pConfig->audioVoiceCppFactors = (AsicAudioVoiceCppTrendFactorsType *)system_property_var.Val.pStruct;
    }

    DAL_GET_PROPERTY_VALUE_WITH_CHECK("CACHE_PARTITION_CONFIG_TABLE")
    if(result == Adsppm_Status_Success)
    {
        pConfig->cachePartitionConfigTable =
            (AsicCachePartitionConfigTableType*)system_property_var.Val.pStruct;
    }

    DAL_GET_PROPERTY_VALUE_WITH_CHECK("BW_CONCURRENCY_SETTINGS")
    if(result == Adsppm_Status_Success)
    {
        pConfig->bwConcurrencySettings = (AsicBwConcurrencySettingsType *)system_property_var.Val.pStruct;
    }

	if(g_AsicConfig.chipVersion < DALCHIPINFO_VERSION(2, 0))
	{
		DAL_GET_PROPERTY_VALUE_WITH_CHECK("ADSP_TO_AHBE_FREQ_TABLE_V1")
	}
	else if(g_AsicConfig.chipVersion < DALCHIPINFO_VERSION(3, 0))
	{
		DAL_GET_PROPERTY_VALUE_WITH_CHECK("ADSP_TO_AHBE_FREQ_TABLE_V1")
	}
	else // V3 and above
	{
		DAL_GET_PROPERTY_VALUE_WITH_CHECK("ADSP_TO_AHBE_FREQ_TABLE_V1")
	}
	if(result == Adsppm_Status_Success)
	{
		pConfig->adspToAhbFreqTable[AsicBusType_AhbE] =
			(AsicAdspToAhbFreqTableType *)system_property_var.Val.pStruct;
	}
    

	if(g_AsicConfig.chipVersion < DALCHIPINFO_VERSION(2, 0))
	{
		DAL_GET_PROPERTY_VALUE_WITH_CHECK("ADSP_TO_AHBIX_FREQ_TABLE_V1")
	}
	else if(g_AsicConfig.chipVersion < DALCHIPINFO_VERSION(3, 0))
	{
		//Since freq plan is same between v1 and v2; keeping as v1
		DAL_GET_PROPERTY_VALUE_WITH_CHECK("ADSP_TO_AHBIX_FREQ_TABLE_V1")
	}
	else // V3 and above
	{
		DAL_GET_PROPERTY_VALUE_WITH_CHECK("ADSP_TO_AHBIX_FREQ_TABLE_V1")
	}

	if(result == Adsppm_Status_Success)
	{
		pConfig->adspToAhbFreqTable[AsicBusType_AhbIx] =
			(AsicAdspToAhbFreqTableType *)system_property_var.Val.pStruct;
	}


    DAL_GET_PROPERTY_VALUE_WITH_CHECK("ADSP_CACHE_SIZE_MPPS_THRESHOLD_TABLE")
    if(result == Adsppm_Status_Success)
    {  
        pConfig->cachesizeMPPSThresholdTable = (AsicAdspCacheSizeMPPSThresholdTableType *)system_property_var.Val.pStruct;
    }

    DAL_GET_PROPERTY_VALUE_WITH_CHECK("ADSP_CACHE_SIZE_BW_SCALING_TABLE")
    if(result == Adsppm_Status_Success)
    {
        pConfig->cacheSizeBWscalingTable = (AsicAdspCacheSizeBWScalingTableType *)system_property_var.Val.pStruct;
    }

    DAL_GET_PROPERTY_VALUE_WITH_CHECK("ADSP_SNOC_BW_VOTING_TABLE")
    if(result == Adsppm_Status_Success)
    {
        pConfig->snocBwVoteTable = (AsicSnocVoteTableType *)system_property_var.Val.pStruct;
    }

    DAL_GET_PROPERTY_VALUE_WITH_CHECK("SYSTEM_CACHE_CONFIG_TABLE")
    if(result == Adsppm_Status_Success)
    {
        pConfig->systemcacheConfigTable = (AsicAdspSystemCacheConfigTableType *)system_property_var.Val.pStruct;
    }   
    if(result != Adsppm_Status_Failed)
        {
            if(0xff == gADSPPMPOWERFEATURE)
            {
                DAL_GET_PROPERTY_VALUE_WITH_CHECK("FEATURE_DESCRIPTORS")
                if(result == Adsppm_Status_Success)
                    {
                    memscpy(
                        pConfig->features,
                        AsicFeatureId_enum_max * sizeof(AsicFeatureDescType),
                        system_property_var.Val.pStruct,
                        AsicFeatureId_enum_max * sizeof(AsicFeatureDescType));
                     }
             }
            else
            {
                if(0 == gADSPPMPOWERFEATURE)
                {
                    featureState = AsicFeatureState_Disabled;
                }
                else if(1 == gADSPPMPOWERFEATURE)
                {
                    featureState = AsicFeatureState_Enabled;
                }
                for(featureIndex = AsicFeatureId_Adsp_Clock_Scaling; featureIndex < AsicFeatureId_enum_max; featureIndex++)
                {
                    pConfig->features[featureIndex].state = featureState;
                }
                // Some special feature need to disable now
                pConfig->features[AsicFeatureId_Ahb_DCG].state = AsicFeatureState_Disabled;
                pConfig->features[AsicFeatureId_LpassCore_PC].state = AsicFeatureState_Disabled;
                pConfig->features[AsicFeatureId_LpassCore_PC_TZ_Handshake].state = AsicFeatureState_Disabled;
            }
    }
    
    if((ACM_GetFeatureStatus(AsicFeatureId_SlpiDSP) == AsicFeatureState_Disabled) &&
        (ACM_GetFeatureStatus(AsicFeatureId_ComputeDSP) == AsicFeatureState_Disabled))
    {
       DAL_GET_PROPERTY_VALUE_WITH_CHECK("AUDIO_CPP_FACTORS")
       if(result == Adsppm_Status_Success)
       {
         pConfig->audioCppFactorsTable = (AsicCppFactorsTableType *)system_property_var.Val.pStruct;
       }

       DAL_GET_PROPERTY_VALUE_WITH_CHECK("VOICE_CPP_FACTORS")
       if(result == Adsppm_Status_Success)
       {
         pConfig->voiceCppFactorsTable = (AsicCppFactorsTableType *)system_property_var.Val.pStruct;
       }

       DAL_GET_PROPERTY_VALUE_WITH_CHECK("THREAD_LEAKAGE_DATA")
       if(result == Adsppm_Status_Success)
       {
         pConfig->threadLeakage = (AsicThreadLeakageArrayType *)system_property_var.Val.pStruct;
       }
    }
    return result;
}

