/*
* Copyright (c) 2014 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
 * hal_clkrgm.c
 *
 *  Created on: Feb 13, 2012
 *      Author: yrusakov
 */

#include "adsppm.h"
#include "adsppm_utils.h"
#include "hal_clkrgm.h"
#include "hal_interrupt.h"
#include "hal_sleep.h"
#include "asic.h"
#include "asic_internal.h"
#include "DDIClock.h"
#include "DALStdDef.h"
#include "DALSys.h"
#include "DALDeviceId.h"
#include "ClockDefs.h"
#include "npa.h"
#include "hal_hwio_clkctrl.h"
#include "string.h"
#include "qurt_memory.h"
#include "stdlib.h"
#include "DDIVCS.h"
#include "sysmon_hvx_qdsppm.h"
#include "sleep_solver.h"
#include "DDIChipInfo.h"
#include "sleep_npa.h"

#define ADSPPM_ADSP_POWER_ENABLE 1

#ifdef CDSP_HVX_LPR
npa_client_handle gHvxNpaHandle;   
DalDeviceHandle gVcsHvxHandle;
boolean gHvxTurnedOffRequested = FALSE;
#endif
extern AsicConfigType g_AsicConfig; 


// This global flag set to 1 to disable AsicFeatureId_LpassCore_PC_TZ_Handshake feature for standalone adsp image loading
static volatile uint32 gDISABLETZHANDSHAKE = 0;

typedef struct
{
    const AsicClkDescriptorType *pClkDescriptor; //!< Pointer to the clock descriptor from ACM
    union
    {
        ClockIdType       clkId;     //!< Clock Id for DAL clocks
        npa_client_handle npaHandle; //!< NPA handle for NPA clocks
    } clkHandle;
    uint32                      currentFreq;     //!< Clock frequency currently set
    Hal_ClkRgmEnableDisableType currentState;    //!< Clock enablement state
    Hal_ClkRgmClkSrcType        currentSrc;      //!< Clock source
    uint32                      requestCount;    //!< Number of clients requesting this clock
} HalClk_ClockDescriptorType;

typedef struct
{
    const AsicPowerDomainDescriptorType *pPwrDescriptor;
    union
    {
        ClockPowerDomainIdType domainId;
        npa_client_handle      npaHandle;
        DalDeviceHandle        vcsHandle;
    } pwrHandle;
    DALSYSEventHandle                   evtSecure;          //!< Event to signal that XPU configuration has been applied by TZ
    Hal_ClkRgmEnableDisableType         *pSecurityClkState; //!< Array to keep state of the security clocks for TZ handshake
} HalClk_PwrDomainDescriptorType;

typedef struct
{
    DALSYSSyncHandle               lock;
    DalDeviceHandle                *pHandle;
    HalClk_ClockDescriptorType     clockDesc[AdsppmClk_EnumMax]; /* stored in same sequence as of clocks_array_<target> in configdata */
    HalClk_PwrDomainDescriptorType pwrDesc[AsicPowerDomain_EnumMax]; /* stored in same sequence as of pwrDomains_array_<target> in configdata */
	
	npa_client_handle npaMaxHandle; //* NPA Max handle for placing the limit request for /clk/cpu
} Hal_ClkRgmCtxType;

static Hal_ClkRgmCtxType gClkRgmCtx;

static void NpaADSPPowerAvailableCb(void *context, unsigned int event_type, void *data, unsigned int data_size);

static void NpaHvxPowerAvailableCb(void *context, unsigned int event_type, void *data, unsigned int data_size);


static Adsppm_Status clkrgm_MemCache_Flush(AdsppmMemIdType mem)
{
    Adsppm_Status result = Adsppm_Status_Success;
    AsicAddressRangeType *pAddrRange = ACM_GetVirtMemAddressRange(mem);
    if(NULL != pAddrRange)
    {
        if(pAddrRange->size > 0)
        {
            // Flush memory
            int res = QURT_EOK;
            ADSPPM_LOG_PRINTF_3(ADSPPM_LOG_LEVEL_INFO,
                "Flushing cache for memory %u (addr = 0x%X, size = %u)",
                mem, (uint32)pAddrRange->startAddr, pAddrRange->size);
            res = qurt_mem_cache_clean(pAddrRange->startAddr,
                pAddrRange->size, QURT_MEM_CACHE_FLUSH, QURT_MEM_DCACHE);

            if(res != QURT_EOK)
            {
                ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
                    "Cannot flush cache for memory %u", mem);
                result = Adsppm_Status_Failed;
            }
        }
        else
        {
            ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,
                "Skipping cache flush for memory %u (mapping not specified)", mem);
        }
    }
    else
    {
        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
            "NULL descriptor for memory ID %u", mem);
        result = Adsppm_Status_ResourceNotFound;
    }
    return result;
}


// Call Clock Regime to enable clock
static Adsppm_Status clkrgm_EnableDalClock(HalClk_ClockDescriptorType *pClkHalDesc)
{
    Adsppm_Status result = Adsppm_Status_Success;
    DALResult dalResult = DAL_SUCCESS;
    dalResult = DalClock_EnableClock(gClkRgmCtx.pHandle, pClkHalDesc->clkHandle.clkId);
    if(DAL_SUCCESS != dalResult)
    {
        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
            "DalClock_EnableClock returned failure: %s",
            pClkHalDesc->pClkDescriptor->clkInfo.clkName);
        result = Adsppm_Status_Failed;
    }
    else
    {
        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,
            "Enabled %s",
            pClkHalDesc->pClkDescriptor->clkInfo.clkName);
    }
    return result;
}


// Call Clock Regime to disable clock
static Adsppm_Status clkrgm_DisableDalClock(HalClk_ClockDescriptorType *pClkHalDesc)
{
    Adsppm_Status result = Adsppm_Status_Success;
    DALResult dalResult = DAL_SUCCESS;
    dalResult = DalClock_DisableClock(gClkRgmCtx.pHandle, pClkHalDesc->clkHandle.clkId);
    if(DAL_SUCCESS != dalResult)
    {
        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
            "DalClock_DisableClock returned failure: %s",
            pClkHalDesc->pClkDescriptor->clkInfo.clkName);
        result = Adsppm_Status_Failed;
    }
    else
    {
        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,
            "Disabled %s",
            pClkHalDesc->pClkDescriptor->clkInfo.clkName);
    }
    return result;
}


// Call Clock Regime to enable HW control
static Adsppm_Status clkrgm_EnableDalClockHwCtl(HalClk_ClockDescriptorType *pClkHalDesc)
{
    Adsppm_Status result = Adsppm_Status_Success;
    DALResult dalResult = DAL_SUCCESS;
    dalResult = DalClock_ConfigClock(
        gClkRgmCtx.pHandle, pClkHalDesc->clkHandle.clkId, CLOCK_CONFIG_LPASS__HW_CTL_ON);
    if(DAL_SUCCESS != dalResult)
    {
        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
            "DalClock_ConfigClock returned failure: %s",
            pClkHalDesc->pClkDescriptor->clkInfo.clkName);
        result = Adsppm_Status_Failed;
    }
    else
    {
        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO_EXT,
            "Enabled DCG for %s",
            pClkHalDesc->pClkDescriptor->clkInfo.clkName);
    }
    return result;
}


// Call Clock Regime to disable HW control
static Adsppm_Status clkrgm_DisableDalClockHwCtl(HalClk_ClockDescriptorType *pClkHalDesc)
{
    Adsppm_Status result = Adsppm_Status_Success;
    DALResult dalResult = DAL_SUCCESS;
    dalResult = DalClock_ConfigClock(
        gClkRgmCtx.pHandle, pClkHalDesc->clkHandle.clkId, CLOCK_CONFIG_LPASS__HW_CTL_OFF);
    if(DAL_SUCCESS != dalResult)
    {
        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
            "DalClock_ConfigClock returned failure: %s",
            pClkHalDesc->pClkDescriptor->clkInfo.clkName);
        result = Adsppm_Status_Failed;
    }
    else
    {
        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO_EXT,
            "Disabled DCG for %s",
            pClkHalDesc->pClkDescriptor->clkInfo.clkName);
    }
    return result;
}


static Adsppm_Status clk_EnableClock(AdsppmClkIdType clkId, HalClk_ClockDescriptorType *pClkHalDesc)
{
    Adsppm_Status result = Adsppm_Status_UnSupported;
    switch(pClkHalDesc->pClkDescriptor->clkType)
    {
    case AsicClk_TypeDalFreqSet:
    case AsicClk_TypeDalEnable:
    case AsicClk_TypeCBC:
        result = clkrgm_EnableDalClock(pClkHalDesc);
        break;
    case AsicClk_TypeInternalCGC:
        result = HalHwIo_EnableClock(clkId);
        break;
    default:
        break;
    }
    if(Adsppm_Status_Success == result)
    {
        ADSPPM_QDSS_EVENT_1(ADSPPM_CLK_ENABLE, clkId);
        pClkHalDesc->currentState = Hal_ClkRgmEnable;
    }
    return result;
}


static Adsppm_Status clk_DisableClock(AdsppmClkIdType clkId, HalClk_ClockDescriptorType *pClkHalDesc)
{
    Adsppm_Status result = Adsppm_Status_Success;
    if((Adsppm_Mem_None != pClkHalDesc->pClkDescriptor->memoryId))
    {
        // Flush cache before disabling memory clock
        result = clkrgm_MemCache_Flush(pClkHalDesc->pClkDescriptor->memoryId);
    }
    if(Adsppm_Status_Success == result)
    {
        switch(pClkHalDesc->pClkDescriptor->clkType)
        {
        case AsicClk_TypeDalFreqSet:
        case AsicClk_TypeDalEnable:
        case AsicClk_TypeCBC:
            result = clkrgm_DisableDalClock(pClkHalDesc);
            break;
        case AsicClk_TypeInternalCGC:
            result = HalHwIo_DisableClock(clkId);
            break;
        default:
            result = Adsppm_Status_UnSupported;
            break;
        }
    }
    if(Adsppm_Status_Success == result)
    {
        ADSPPM_QDSS_EVENT_1(ADSPPM_CLK_DISABLE, clkId);
        pClkHalDesc->currentState = Hal_ClkRgmDisable;
    }
    return result;
}


static Adsppm_Status clkrgm_EnableClockHwCtl(AdsppmClkIdType clkId, HalClk_ClockDescriptorType *pClkHalDesc)
{
    Adsppm_Status result = Adsppm_Status_UnSupported;
    switch(pClkHalDesc->pClkDescriptor->clkType)
    {
    case AsicClk_TypeDalFreqSet:
    case AsicClk_TypeDalEnable:
    case AsicClk_TypeCBC:
        result = clkrgm_EnableDalClockHwCtl(pClkHalDesc);
        break;
    case AsicClk_TypeInternalCGC:
        result = HalHwIo_EnableHWClockCntl(clkId);
        break;
    default:
        break;
    }
    return result;
}


static Adsppm_Status clkrgm_DisableClockHwCtl(AdsppmClkIdType clkId, HalClk_ClockDescriptorType *pClkHalDesc)
{
    Adsppm_Status result = Adsppm_Status_UnSupported;
    switch(pClkHalDesc->pClkDescriptor->clkType)
    {
    case AsicClk_TypeDalFreqSet:
    case AsicClk_TypeDalEnable:
    case AsicClk_TypeCBC:
        result = clkrgm_DisableDalClockHwCtl(pClkHalDesc);
        break;
    case AsicClk_TypeInternalCGC:
        result = HalHwIo_DisableHWClockCntl(clkId);
        break;
    default:
        break;
    }
    return result;
}


static Adsppm_Status clkrgm_EnableClockBranch(AdsppmClkIdType clkId, HalClk_ClockDescriptorType *pClkHalDesc)
{
    Adsppm_Status result = Adsppm_Status_Success;
    switch(pClkHalDesc->pClkDescriptor->clkCntlType)
    {
    case AsicClk_CntlAlwaysON_DCG:
    case AsicClk_CntlSW_DCG:
        // Enable clock if DCG feature is not enabled
        if(AsicFeatureState_Enabled == ACM_GetFeatureStatus(AsicFeatureId_Ahb_DCG))
        {
            pClkHalDesc->currentState = Hal_ClkRgmEnable;
            break;
        }
        // No break statement here to proceed to disable clock
    case AsicClk_CntlAlwaysON:
    case AsicClk_CntlSW:
        result = clk_EnableClock(clkId, pClkHalDesc);
        break;
    default:
        // Don't enable Always OFF Clocks
        break;
    }
    return result;
}


static Adsppm_Status clkrgm_DisableClockBranch(AdsppmClkIdType clkId, HalClk_ClockDescriptorType *pClkHalDesc)
{
    Adsppm_Status result = Adsppm_Status_Success;
    switch(pClkHalDesc->pClkDescriptor->clkCntlType)
    {
    case AsicClk_CntlSW_DCG:
        // Disable clock if DCG feature is not enabled
        if(AsicFeatureState_Enabled == ACM_GetFeatureStatus(AsicFeatureId_Ahb_DCG))
        {
            pClkHalDesc->currentState = Hal_ClkRgmDisable;
            break;
        }
        // No break statement here to proceed to disable clock
    case AsicClk_CntlOff:
    case AsicClk_CntlSW:
        result = clk_DisableClock(clkId, pClkHalDesc);
        break;
    default:
        // Don't disable AON clocks,
        // or AON DCG clocks if DCG is enabled
        break;
    }
    return result;
}


static Adsppm_Status clkrgm_MemCore_Retention(AdsppmClkIdType clkId, Hal_ClkRgmMemStateType pwrState)
{
    Adsppm_Status result = Adsppm_Status_Success;
    DALResult dalResult = DAL_SUCCESS;
    Hal_ClkRgmEnableDisableType currentClkState = Hal_ClkRgmEnable;
    uint32 clkIdx =0;

    if((AdsppmClk_None < clkId) && (AdsppmClk_EnumMax > clkId))
    {
        if (ACM_GetClkIdx(clkId , &clkIdx) == Adsppm_Status_Failed)
            return Adsppm_Status_Failed;

        HalClk_ClockDescriptorType *pClkHalDesc = &gClkRgmCtx.clockDesc[clkIdx];

        currentClkState = pClkHalDesc->currentState;

        //Call clock driver to enable the clock
        if(Hal_ClkRgmEnable != currentClkState)
        {
            result = clkrgm_EnableClockBranch(clkId, pClkHalDesc);
        }
        //Call clock regime to enable/disable mem core retention
        if(Adsppm_Status_Success == result)
        {
            ADSPPM_QDSS_EVENT_2(ADSPPM_MEM_PWR, clkId, pwrState);
            if(Hal_ClkRgmMemRetain == pwrState)
            {
                ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,
                    "Memory retention for %s",
                    pClkHalDesc->pClkDescriptor->clkInfo.clkName);
                dalResult = DalClock_ConfigClock(gClkRgmCtx.pHandle,
                    pClkHalDesc->clkHandle.clkId, CLOCK_CONFIG_LPASS_CORE_MEM_ON);
            }
            else
            {
                ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,
                    "Memory collapse for %s",
                    pClkHalDesc->pClkDescriptor->clkInfo.clkName);
                dalResult = DalClock_ConfigClock(gClkRgmCtx.pHandle,
                    pClkHalDesc->clkHandle.clkId , CLOCK_CONFIG_LPASS_CORE_MEM_OFF);
            }
            if(DAL_SUCCESS != dalResult)
            {
                ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,
                    "DalClock_ConfigClock returned failure");
                result = Adsppm_Status_Failed;
            }
        }
        //restore previous clock state
        if(Adsppm_Status_Success == result)
        {
            if(Hal_ClkRgmEnable != currentClkState)
            {
                result = clkrgm_DisableClockBranch(clkId, pClkHalDesc);
            }
        }
    }
    else
    {
        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
            "AdsppmClkId out of range for ClkId : %u",clkId);
        result = Adsppm_Status_Failed;
    }

    return result;
}

static Adsppm_Status clkrgm_InitPowerDomains(void)
{
    Adsppm_Status result = Adsppm_Status_Success;
    DALResult dalResult = DAL_SUCCESS;
    uint32 i = 0;
    uint32 numPwrDomainDesc = 0;
    uint32 pwrDomainDescIdx = 0;

	numPwrDomainDesc = ACMClk_GetNumberOfPwrDomainDescriptors();

    // Get information for each power domain from ClkRegime
    for(i = 0; (Adsppm_Status_Success == result) && (i < numPwrDomainDesc); i++)
    {
        HalClk_PwrDomainDescriptorType *pPwrHalDesc = &gClkRgmCtx.pwrDesc[i];
        pPwrHalDesc->pPwrDescriptor = ACMClk_GetPwrDomainDescriptor(i);

        if(NULL != pPwrHalDesc->pPwrDescriptor)
        {
            switch(pPwrHalDesc->pPwrDescriptor->pwrDomainType)
            {
            case AsicRsc_Power_ADSP:
                // Register with NPA node for vdd voting
                // Require ClkRegime to be initialized before ADSPPM,
                // so the npa clocks have to be available at this point
                if(strncmp(pPwrHalDesc->pPwrDescriptor->pwrDomainName, "", MAX_ADSPPM_CLIENT_NAME))
                {
                    npa_resource_available_cb(pPwrHalDesc->pPwrDescriptor->pwrDomainName, NpaADSPPowerAvailableCb, NULL);
                }
                break;
            case AsicRsc_Power_Mem:
                break;
            case AsicRsc_Power_Core:
                // Get domain ID from ClkRegime based on clock name
                // Ignore domains with empty name
                if(strncmp(pPwrHalDesc->pPwrDescriptor->pwrDomainName, "", MAX_ADSPPM_CLIENT_NAME))
                {
                    dalResult = DalClock_GetPowerDomainId(
                        gClkRgmCtx.pHandle, pPwrHalDesc->pPwrDescriptor->pwrDomainName, &(pPwrHalDesc->pwrHandle.domainId));
                    if(DAL_SUCCESS != dalResult)
                    {
                        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
                            "Failed to get Power domain ID for %s",
                            pPwrHalDesc->pPwrDescriptor->pwrDomainName);
                        result = Adsppm_Status_Failed;
                    }
                    else
                    {
                        if((AsicFeatureState_Enabled == ACM_GetFeatureStatus(AsicFeatureId_LpassCore_PC_TZ_Handshake)) &&
                            (0 == gDISABLETZHANDSHAKE))
                        {
                            // Create Dal event
                            dalResult = DALSYS_EventCreate(DALSYS_EVENT_ATTR_CLIENT_DEFAULT, &(pPwrHalDesc->evtSecure), NULL);
                            if(DAL_SUCCESS != dalResult)
                            {
                                ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
                                    "Failed to create Event for %s",
                                    pPwrHalDesc->pPwrDescriptor->pwrDomainName);
                                result = Adsppm_Status_Failed;
                            }
                            else
                            {
                                // Register event and Enable TZ ack interrupt
                                result = Intr_RegisterEvent(pPwrHalDesc->pPwrDescriptor->intrReinitDone, pPwrHalDesc->evtSecure);
                                if(Adsppm_Status_Success == result)
                                {
                                    // Allocate memory for Security clock states
                                    pPwrHalDesc->pSecurityClkState =
                                        malloc(sizeof(Hal_ClkRgmEnableDisableType) *
                                        pPwrHalDesc->pPwrDescriptor->securityClocks.numClocks);
                                    if(NULL == pPwrHalDesc->pSecurityClkState)
                                    {
                                        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,
                                            "Failed to allocate memory for pSecurityClkState");
                                        result = Adsppm_Status_Failed;
                                    }
                                }
                            }
                        }
                    }
                }
                break;
            case AsicRsc_Power_Hvx:
                
                if (ACM_GetPwrDomainIdx(AsicPowerDomain_Hvx , &pwrDomainDescIdx) == Adsppm_Status_Failed)
                    break;
                
                // HVX power is controlled through DAL API implemented by the VCS driver.
                if(strncmp(pPwrHalDesc->pPwrDescriptor->pwrDomainName, "", MAX_ADSPPM_CLIENT_NAME))
                {
                    DALResult dalResult = DAL_SUCCESS;
                    DalDeviceHandle *hVCS;
                    dalResult = DAL_StringDeviceAttach(pPwrHalDesc->pPwrDescriptor->pwrDomainName, &hVCS);
                    if(dalResult != DAL_SUCCESS)
                    {
                      ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
                                          "Failed to get dal/vcs handle for power domain %u", AsicRsc_Power_Hvx);
                      break;
                    }
                    pPwrHalDesc->pwrHandle.vcsHandle = *hVCS;
#ifdef CDSP_HVX_LPR
                    gVcsHvxHandle = pPwrHalDesc->pwrHandle.vcsHandle;
                    gHvxNpaHandle = npa_create_sync_client(
                                                         "/CDSPPM/HVX/LPR",
                                                         "CDSPPM_HVX_WA",
                                                         NPA_CLIENT_REQUIRED);
                    if(NULL == gHvxNpaHandle)
                    {
                       ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,
                                               "Failed to create client for HVX sleep lpr");
                    }    
#endif                    
                }
                break;
            default:
                break;
            }
        }
        else
        {
            ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
                "Failed to get Pwr descriptor for power domain %u", i);
            result = Adsppm_Status_Failed;
        }
    }

    return result;
}


static Adsppm_Status clkrgm_InitClocks(void)
{
    Adsppm_Status result = Adsppm_Status_Success;
    DALResult dalResult = DAL_SUCCESS;
    uint32 i = 0;
    uint32 numClkDesc = 0;

	numClkDesc = ACMClk_GetNumberOfClockDescriptors();
	for (i = 0; (Adsppm_Status_Success == result) && (i < numClkDesc); i++)
	{
        HalClk_ClockDescriptorType *pClkHalDesc = &gClkRgmCtx.clockDesc[i];
        pClkHalDesc->pClkDescriptor = ACMClk_GetClockDescriptor(i);
        pClkHalDesc->currentFreq = 0;
        pClkHalDesc->currentSrc = Hal_ClkRgmClkSrc_Primary;
        pClkHalDesc->requestCount = 0;

        if(NULL != pClkHalDesc->pClkDescriptor)
        {
            switch(pClkHalDesc->pClkDescriptor->clkType)
            {
            case AsicClk_TypeNpa:
                // Register with NPA node
                // Require ClkRegime to be initialized before ADSPPM,
                // so the npa clocks have to be available at this point
                //npa_resource_available_cb(pClk->pClkDescriptor->clkName, NpaClkAvailableCb, NULL);
                pClkHalDesc->clkHandle.npaHandle = npa_create_sync_client(
                    pClkHalDesc->pClkDescriptor->clkInfo.clkName, "ADSPPM", NPA_CLIENT_REQUIRED);
                if(NULL == pClkHalDesc->clkHandle.npaHandle)
                {
                    ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
                        "Failed to create NPA client for %s",
                        pClkHalDesc->pClkDescriptor->clkInfo.clkName);
                    result = Adsppm_Status_Failed;
                }
				
				// Register with NPA node
                // Require ClkRegime to be initialized before ADSPPM,
                // so the npa clocks have to be available at this point
                //npa_resource_available_cb(pClk->pClkDescriptor->clkName, NpaClkAvailableCb, NULL);
                gClkRgmCtx.npaMaxHandle = npa_create_sync_client(
                    pClkHalDesc->pClkDescriptor->clkInfo.clkName, "ADSPPM_MAX", NPA_CLIENT_LIMIT_MAX);
                if(NULL == gClkRgmCtx.npaMaxHandle)
                {
                    ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
                        "Failed to create Max NPA client for %s",
                        pClkHalDesc->pClkDescriptor->clkInfo.clkName);
                    result = Adsppm_Status_Failed;
                }
                break;
            case AsicClk_TypeDalFreqSet:
            case AsicClk_TypeDalEnable:
            case AsicClk_TypeCBC:
            case AsicClk_TypeDalDomainSrc:
                // Get clock Id from ClkRegime based on clock name
                dalResult = DalClock_GetClockId(
                    gClkRgmCtx.pHandle, pClkHalDesc->pClkDescriptor->clkInfo.clkName, &(pClkHalDesc->clkHandle.clkId));
                if(DAL_SUCCESS != dalResult)
                {
                    ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
                        "Failed to get ClkId for %s",
                        pClkHalDesc->pClkDescriptor->clkInfo.clkName);
                    result = Adsppm_Status_Failed;
                }
                break;
            default:
                break;
            }
        }
        else
        {
            ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
                "Failed to get Clock descriptor for clock %u", i);
            result = Adsppm_Status_Failed;
        }
    }

    // Set initial state for each clock
    // It needs to be set in a different for loop from the one above
    // Reason is that there are clock source dependencies among clocks
	for (i = 0; (Adsppm_Status_Success == result) && (i < numClkDesc); i++)
	{
        HalClk_ClockDescriptorType *pClkHalDesc = &gClkRgmCtx.clockDesc[i];

        // Only proceed for clocks that are present in the chip
        // Otherwise just skip the setting
        if(pClkHalDesc->pClkDescriptor->clkType != AsicClk_TypeNone)
        {
            // Enable/Disable HW control if clock is of DCG type
            if((pClkHalDesc->pClkDescriptor->clkCntlType == AsicClk_CntlAlwaysON_DCG) ||
                (pClkHalDesc->pClkDescriptor->clkCntlType == AsicClk_CntlSW_DCG))
            {
                result = (AsicFeatureState_Enabled == ACM_GetFeatureStatus(AsicFeatureId_Ahb_DCG))?
                    clkrgm_EnableClockHwCtl(pClkHalDesc->pClkDescriptor->clkId, pClkHalDesc):
                    clkrgm_DisableClockHwCtl(pClkHalDesc->pClkDescriptor->clkId, pClkHalDesc);
            }

            // Do not touch AsicClk_TypeCBC clocks
            // They will be enabled/disabled from calls on matching CGCs
            if((Adsppm_Status_Success == result) &&
                (pClkHalDesc->pClkDescriptor->clkType != AsicClk_TypeCBC))
            {
                switch(pClkHalDesc->pClkDescriptor->clkCntlType)
                {
                case AsicClk_CntlOff:
                    // Disable AOFF clock
                    pClkHalDesc->currentState = Hal_ClkRgmEnable;
                    result = ClkRgm_EnableClock(pClkHalDesc->pClkDescriptor->clkId, Hal_ClkRgmDisable);
                    break;
                case AsicClk_CntlAlwaysON:
                case AsicClk_CntlAlwaysON_DCG:
                    // Enable AON clock
                    // If DCG is enabled, call is ignore downstream
                    pClkHalDesc->currentState = Hal_ClkRgmDisable;
                    result = ClkRgm_EnableClock(pClkHalDesc->pClkDescriptor->clkId, Hal_ClkRgmEnable);
                    break;
                case AsicClk_CntlSW:
                case AsicClk_CntlSW_DCG:
                    // Disable clock
                    // If DCG is enabled, call is ignore downstream
                    pClkHalDesc->currentState = Hal_ClkRgmEnable;
                    result = ClkRgm_EnableClock(pClkHalDesc->pClkDescriptor->clkId, Hal_ClkRgmDisable);
                    break;
                default:
                    // Don't touch other clocks here
                    break;
                }
            }

            if(Adsppm_Status_Success == result)
            {
                switch(pClkHalDesc->pClkDescriptor->clkMemCntlType)
                {
                case AsicClk_MemCntlAlwaysRetain:
                    // Call clock regime to enable mem core retention
                    result = clkrgm_MemCore_Retention(pClkHalDesc->pClkDescriptor->clkId, Hal_ClkRgmMemRetain);
                    break;
                default:
                    // Don't touch other clocks here
                    break;
                }
            }
        }
    }

    return result;
}


Adsppm_Status ClkRgm_Init(void)
{
    Adsppm_Status result = Adsppm_Status_Success;
    DALResult dalResult = DAL_SUCCESS;
    ADSPPM_LOG_FUNC_PROFILESTART;

    // Create mutex
    dalResult = DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE, (DALSYSSyncHandle*) &(gClkRgmCtx.lock), NULL);
    if(DAL_SUCCESS != dalResult)
    {
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "Failed to create lock");
        result = Adsppm_Status_NotInitialized;
    }
    else
    {
        // Register handle for Clock Regime
        dalResult = DAL_ClockDeviceAttach(DALDEVICEID_CLOCK, &(gClkRgmCtx.pHandle));
        if(DAL_SUCCESS != dalResult)
        {
            ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "DAL_ClockDeviceAttach failed");
            result = Adsppm_Status_Failed;
        }
        else
        {
            // Initialize Power Domains
            result = clkrgm_InitPowerDomains();
            if(Adsppm_Status_Success != result)
            {
                ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "clkrgm_InitPowerDomains failed");
            }
            else
            {
                // Initialize Clocks
                result = clkrgm_InitClocks();
                if(Adsppm_Status_Success != result)
                {
                    ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "clkrgm_InitClocks failed");
                }
            }
        }

        // Destroy lock if init failed
        if(Adsppm_Status_Success != result)
        {
            DALSYS_DestroyObject((DALSYSSyncHandle *) &(gClkRgmCtx.lock));
            gClkRgmCtx.lock = NULL;
        }
    }

    ADSPPM_LOG_FUNC_PROFILEEND;
    return result;
}


static void NpaADSPPowerAvailableCb(void *context, unsigned int event_type, void *data, unsigned int data_size)
{
    ADSPPM_LOG_FUNC_PROFILESTART;
    uint32 pwrDomainDescIdx =0;

    if (ACM_GetPwrDomainIdx(AsicPowerDomain_Adsp , &pwrDomainDescIdx) == Adsppm_Status_Failed)
		return;

    HalClk_PwrDomainDescriptorType *pPwr = &gClkRgmCtx.pwrDesc[pwrDomainDescIdx];
    pPwr->pwrHandle.npaHandle = npa_create_sync_client(
        pPwr->pPwrDescriptor->pwrDomainName,
        "adsppm_adsp_power",
        NPA_CLIENT_REQUIRED);
    if(NULL == pPwr->pwrHandle.npaHandle)
    {
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,
            "Failed to create client for ADSP power");
    }
    ADSPPM_LOG_FUNC_PROFILEEND;
}


static void NpaHvxPowerAvailableCb(void *context, unsigned int event_type, void *data, unsigned int data_size)
{
    ADSPPM_LOG_FUNC_PROFILESTART;
    uint32 pwrDomainDescIdx =0;

    if (ACM_GetPwrDomainIdx(AsicPowerDomain_Hvx , &pwrDomainDescIdx) == Adsppm_Status_Failed)
		return;

    HalClk_PwrDomainDescriptorType *pPwr = &gClkRgmCtx.pwrDesc[pwrDomainDescIdx];
    pPwr->pwrHandle.npaHandle = npa_create_sync_client(
        pPwr->pPwrDescriptor->pwrDomainName,
        "adsppm_hvx_power",
        NPA_CLIENT_REQUIRED);
    if(NULL == pPwr->pwrHandle.npaHandle)
    {
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,
            "Failed to create client for HVX power");
    }
    ADSPPM_LOG_FUNC_PROFILEEND;
}


/**
 * @fn ClkRgm_CalcClockFrequency - get clock frequency through clock regime calculate from oscillator
 * @param pFreq - out put frequency in Hz
 */
Adsppm_Status ClkRgm_CalcClockFrequency(AdsppmClkIdType clkId, uint32 *pFreq)
{
    Adsppm_Status result = Adsppm_Status_Success;
    npa_query_type sQueryResult;
    npa_query_status eNpaStatus = NPA_QUERY_SUCCESS;
    DALResult dalResult = DAL_SUCCESS;
    uint32 clkIdx =0;

    ADSPPM_LOG_FUNC_PROFILESTART;

    if((AdsppmClk_None < clkId) && (AdsppmClk_EnumMax > clkId) && (NULL != pFreq))
    {
        if(NULL != gClkRgmCtx.lock)
        {
            if (ACM_GetClkIdx(clkId , &clkIdx) == Adsppm_Status_Failed)
                return Adsppm_Status_Failed;

            HalClk_ClockDescriptorType *pClkHalDesc = &gClkRgmCtx.clockDesc[clkIdx];

            if((NULL != pClkHalDesc->pClkDescriptor) && (AdsppmClk_None != pClkHalDesc->pClkDescriptor->clkId))
            {
                if(pClkHalDesc->currentState == Hal_ClkRgmEnable)
                {
                    switch(pClkHalDesc->pClkDescriptor->clkType)
                    {
                    case AsicClk_TypeNpa:
                        eNpaStatus = npa_query_by_name(
                            pClkHalDesc->pClkDescriptor->clkInfo.clkName, NPA_QUERY_CURRENT_STATE, &sQueryResult);
                        if(0 != eNpaStatus)
                        {
                            ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
                                "npa_query_by_name: Failed due to NPA error %u", eNpaStatus);
                            result = Adsppm_Status_Failed;
                        }
                        else
                        {
                            *pFreq = sQueryResult.data.state;
                        }
                        break;
                    default:
                        if(pClkHalDesc->clkHandle.clkId != NULL)
                        {
                            // TODO: Use calulate clock now, later on will use getfreq api from clock regime
                            dalResult = DalClock_GetClockFrequency(
                                gClkRgmCtx.pHandle, pClkHalDesc->clkHandle.clkId, pFreq);
                            if(DAL_SUCCESS != dalResult)
                            {
                                ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,
                                    "DalClock_GetClockFrequency returned failure");
                                result = Adsppm_Status_Failed;
                            }
                            else
                            {
                                ADSPPM_LOG_PRINTF_3(ADSPPM_LOG_LEVEL_INFO,
                                    "Got clock frequency %u for %s. currentset freq=%u",
                                    *pFreq, pClkHalDesc->pClkDescriptor->clkInfo.clkName, pClkHalDesc->currentFreq);
                            }
                        }
                        else
                        {
                            if(AdsppmClk_None != pClkHalDesc->pClkDescriptor->clkSrcId)
                            {
                                //Recursive call here.
                                //We are relying on the lock to work in recursive cases as it is stated in the API description
                                result = ClkRgm_CalcClockFrequency(pClkHalDesc->pClkDescriptor->clkSrcId, pFreq);
                            }
                        }
                        break;
                    }
                }
                else
                {
                    *pFreq = 0;
                }
            }
            else
            {
                ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_INFO, "bad pClk->pClkDescriptor->clkId");
                result = Adsppm_Status_BadParm;
            }
        }
        else
        {
            result = Adsppm_Status_NotInitialized;
        }
    }
    else
    {
        result = Adsppm_Status_BadParm;
        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR, "bad clk or pFreq. ClkId : %u",clkId);
    }

    ADSPPM_LOG_FUNC_PROFILEEND;
    return result;
}


Adsppm_Status ClkRgm_GetClockFrequency(AdsppmClkIdType clkId, uint32 *pFreq)
{
    Adsppm_Status result = Adsppm_Status_Success;
    npa_query_type sQueryResult;
    npa_query_status eNpaStatus = NPA_QUERY_SUCCESS;
    uint32 clkIdx =0;

    ADSPPM_LOG_FUNC_PROFILESTART;

    if((AdsppmClk_None < clkId) && (AdsppmClk_EnumMax > clkId) && (NULL != pFreq))
    {
        if(NULL != gClkRgmCtx.lock)
        {
            if (ACM_GetClkIdx(clkId , &clkIdx) == Adsppm_Status_Failed)
                return Adsppm_Status_Failed;

            HalClk_ClockDescriptorType *pClkHalDesc = &gClkRgmCtx.clockDesc[clkIdx];

            if(pClkHalDesc->currentState == Hal_ClkRgmEnable)
            {
                switch(pClkHalDesc->pClkDescriptor->clkType)
                {
                case AsicClk_TypeNpa:
                    eNpaStatus = npa_query_by_name(
                        pClkHalDesc->pClkDescriptor->clkInfo.clkName, NPA_QUERY_CURRENT_STATE, &sQueryResult);
                    if(0 != eNpaStatus)
                    {
                        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
                            "npa_query_by_name: Failed due to NPA error %u", eNpaStatus);
                        result = Adsppm_Status_Failed;
                    }
                    else
                    {
                        *pFreq = sQueryResult.data.state;
                    }
                    break;
                default:
                    *pFreq = pClkHalDesc->currentFreq;
                    break;
                }
            }
            else
            {
                *pFreq = 0;
            }
        }
        else
        {
            result = Adsppm_Status_NotInitialized;
        }
    }
    else
    {
        result = Adsppm_Status_NullPointer;
    }

    ADSPPM_LOG_FUNC_PROFILEEND;
    return result;
}


/**
 * @fn ClkRgm_SetClock - Set clock frequency via ClkRegime
 * @param freq - frequency in Hz
 */
Adsppm_Status ClkRgm_SetClock(AdsppmClkIdType clkId, uint32 freq)
{
    Adsppm_Status result = Adsppm_Status_Success;
    uint32 clkIdx =0;

    ADSPPM_LOG_FUNC_PROFILESTART;

    if((AdsppmClk_None < clkId) && (AdsppmClk_EnumMax > clkId))
    {
        if(NULL != gClkRgmCtx.lock)
        {
            if (ACM_GetClkIdx(clkId , &clkIdx) == Adsppm_Status_Failed)
                return Adsppm_Status_Failed;

            HalClk_ClockDescriptorType *pClkHalDesc = &gClkRgmCtx.clockDesc[clkIdx];

            DALResult dalResult = DAL_SUCCESS;
            if(NULL != pClkHalDesc->pClkDescriptor)
            {
                //Lock HW layer
                adsppm_lock(gClkRgmCtx.lock);

                if(pClkHalDesc->currentFreq != freq)
                {
                    uint32 previousFreq = pClkHalDesc->currentFreq;
                    switch(pClkHalDesc->pClkDescriptor->clkType)
                    {
                    case AsicClk_TypeNpa:
                        //Use NPA node
                        if(NULL != pClkHalDesc->clkHandle.npaHandle)
                        {
                            if(0 < freq)
                            {
                                ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_INFO,
                                    "NPA request for %s, state: %u",
                                    pClkHalDesc->pClkDescriptor->clkInfo.clkName, freq);
                                ADSPPM_QDSS_EVENT_3(ADSPPM_CLK_SET, clkId, freq, freq);
                                //issue NPA request for specified frequency
                                //Translate from Hz to kHz first
                                npa_issue_required_request(pClkHalDesc->clkHandle.npaHandle, freq/1000);
                            }
                        }
                        else
                        {
                            ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "NULL NPA handle");
                            result = Adsppm_Status_Failed;
                        }
                        break;
                    case AsicClk_TypeDalEnable:
                    case AsicClk_TypeInternalCGC:
                    case AsicClk_TypeCBC:
                        if(0 < freq)
                        {
                            // Use clock source from clock descriptor
                            if(AdsppmClk_None != pClkHalDesc->pClkDescriptor->clkSrcId)
                            {
                                ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_INFO,
                                    "Requested freq %u for %s",
                                    freq, pClkHalDesc->pClkDescriptor->clkInfo.clkName);
                                result = ClkRgm_SetClock(pClkHalDesc->pClkDescriptor->clkSrcId, freq);
                            }
                        }
                        break;
                    case AsicClk_TypeDalFreqSet:
                    case AsicClk_TypeDalDomainSrc:
                        //Call clock regime to set at least requested frequency
                        //Disable clock branch if 0 frequency is requested
                        if(0 < freq)
                        {
                            ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_INFO,
                                        "freq requested %u for %s",
                                        freq, pClkHalDesc->pClkDescriptor->clkInfo.clkName);
                            //attempt to set frequency at least as requested
                            dalResult = DalClock_SetClockFrequency(
                                gClkRgmCtx.pHandle, pClkHalDesc->clkHandle.clkId,
                                freq, CLOCK_FREQUENCY_HZ_AT_LEAST, &pClkHalDesc->currentFreq);
                            if(DAL_SUCCESS != dalResult)
                            {
                                // Try to request cap the frequency
                                // if the previous request has failed
                                // because of exceeding max for this clock
                                dalResult = DalClock_SetClockFrequency(
                                    gClkRgmCtx.pHandle, pClkHalDesc->clkHandle.clkId,
                                    freq, CLOCK_FREQUENCY_HZ_CLOSEST, &pClkHalDesc->currentFreq);
                                if(DAL_SUCCESS != dalResult)
                                {
                                    ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,
                                        "DalClock_SetClockFrequency returned failure");
                                    result = Adsppm_Status_Failed;
                                }
                                else
                                {
                                    ADSPPM_LOG_PRINTF_3(ADSPPM_LOG_LEVEL_INFO,
                                        "Failed to set freq %u for %s. Capped freq=%u",
                                        freq, pClkHalDesc->pClkDescriptor->clkInfo.clkName, pClkHalDesc->currentFreq);
                                }
                            }
                            else
                            {
                                ADSPPM_LOG_PRINTF_3(ADSPPM_LOG_LEVEL_INFO,
                                    "Requested frequency %u for %s. Set freq=%u",
                                    freq, pClkHalDesc->pClkDescriptor->clkInfo.clkName, pClkHalDesc->currentFreq);
                                ADSPPM_QDSS_EVENT_3(ADSPPM_CLK_SET,
                                    pClkHalDesc->pClkDescriptor->clkId, freq, pClkHalDesc->currentFreq);
                            }
                        }
                        break;
                    default:
                        break;
                    }

                    if(Adsppm_Status_Success == result)
                    {
                        pClkHalDesc->currentFreq = freq;
                        if((0 < freq) && (0 == previousFreq))
                        {
                            result = ClkRgm_EnableClock(clkId, Hal_ClkRgmEnable);
                        }
                        if(0 == freq)
                        {
                            result = ClkRgm_EnableClock(clkId, Hal_ClkRgmDisable);
                        }
                    }
                }

                //Unlock HW layer
                adsppm_unlock(gClkRgmCtx.lock);
            }
            else
            {
                ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "invalid HalClk_ClockDescriptor");
                result = Adsppm_Status_BadParm;
            }
        }
        else
        {
            result = Adsppm_Status_NotInitialized;
        }
    }
    else
    {
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "AdsppmClkId out of range");
        result = Adsppm_Status_BadParm;
    }

    ADSPPM_LOG_FUNC_PROFILEEND;
    return result;
}

/**
 * @fn ClkRgm_SetMaxClock - Set max clock frequency via ClkRegime
 * @param freq - frequency in Hz
 */
Adsppm_Status ClkRgm_SetMaxClock(AdsppmClkIdType clkId, uint32 freq)
{
    Adsppm_Status result = Adsppm_Status_Success;
    uint32 clkIdx =0;

    ADSPPM_LOG_FUNC_PROFILESTART;

    if((AdsppmClk_None < clkId) && (AdsppmClk_EnumMax > clkId))
    {
        if(NULL != gClkRgmCtx.lock)
        {
            if (ACM_GetClkIdx(clkId , &clkIdx) == Adsppm_Status_Failed)
                return Adsppm_Status_Failed;

            HalClk_ClockDescriptorType *pClkHalDesc = &gClkRgmCtx.clockDesc[clkIdx];

            if(NULL != pClkHalDesc->pClkDescriptor)
            {
                //Lock HW layer
                adsppm_lock(gClkRgmCtx.lock);

				switch(pClkHalDesc->pClkDescriptor->clkType)
				{
				case AsicClk_TypeNpa:
					//Use NPA node
					if(NULL != gClkRgmCtx.npaMaxHandle)
					{
							ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_INFO,
								"NPA Limit Max request for %s, state: %u",
								pClkHalDesc->pClkDescriptor->clkInfo.clkName, freq);
							ADSPPM_QDSS_EVENT_3(ADSPPM_CLK_SET, clkId, freq, freq);
							//issue NPA request for specified frequency
							//Translate from Hz to kHz first
							npa_issue_limit_max_request(gClkRgmCtx.npaMaxHandle, freq/1000);
					}
					else
					{
						ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "NULL NPA handle");
						result = Adsppm_Status_Failed;
					}
					break;
				case AsicClk_TypeDalEnable:
				case AsicClk_TypeInternalCGC:
				case AsicClk_TypeCBC:
				case AsicClk_TypeDalFreqSet:
				case AsicClk_TypeDalDomainSrc:
					
					break;
				default:
					break;
				}

                //Unlock HW layer
                adsppm_unlock(gClkRgmCtx.lock);
            }
            else
            {
                ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "invalid HalClk_ClockDescriptor");
                result = Adsppm_Status_BadParm;
            }
        }
        else
        {
            result = Adsppm_Status_NotInitialized;
        }
    }
    else
    {
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "AdsppmClkId out of range");
        result = Adsppm_Status_BadParm;
    }

    ADSPPM_LOG_FUNC_PROFILEEND;
    return result;
}


Adsppm_Status ClkRgm_EnableClock(AdsppmClkIdType clkId, Hal_ClkRgmEnableDisableType stateRequested)
{
    Adsppm_Status result = Adsppm_Status_Success;
    Hal_ClkRgmEnableDisableType state = stateRequested;
    uint32 clkIdx =0;

    ADSPPM_LOG_FUNC_PROFILESTART;

    if((AdsppmClk_None < clkId) && (AdsppmClk_EnumMax > clkId))
    {
        if(NULL != gClkRgmCtx.lock)
        {
            if (ACM_GetClkIdx(clkId , &clkIdx) == Adsppm_Status_Failed)
                return Adsppm_Status_Failed;

            HalClk_ClockDescriptorType *pClkHalDesc = &gClkRgmCtx.clockDesc[clkIdx];

            if(NULL != pClkHalDesc->pClkDescriptor)
            {
                // Lock HW layer
                adsppm_lock(gClkRgmCtx.lock);

                // Update reference count
                ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_INFO_EXT,
                    "Clock=%s, requested state=%u",
                    pClkHalDesc->pClkDescriptor->clkInfo.clkName, stateRequested);
                ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_DEBUG,
                    "current counter=%u, current state=%u",
                    pClkHalDesc->requestCount, pClkHalDesc->currentState);
                if(Hal_ClkRgmEnable == stateRequested)
                {
                    pClkHalDesc->requestCount++;
                }
                else
                {
                    if(pClkHalDesc->requestCount > 0) pClkHalDesc->requestCount--;
                    //Override the requested state if the request counter is not 0
                    if(pClkHalDesc->requestCount > 0) state = Hal_ClkRgmEnable;
                }
                ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_DEBUG,
                    "resulting counter=%u, resulting state=%u",
                    pClkHalDesc->requestCount, state);

                if(pClkHalDesc->currentState != state)
                {
                    switch(pClkHalDesc->pClkDescriptor->clkType)
                    {
                    case AsicClk_TypeNpa:
                        // Use NPA node
                        // Require ClkRegime to be initialized before ADSPPM,
                        // so the npa clocks have to be available at this point
                        if(NULL != pClkHalDesc->clkHandle.npaHandle)
                        {
                            if(Hal_ClkRgmEnable == state)
                            {
                                if(0 == pClkHalDesc->currentFreq)
                                {
                                    npa_query_type query;
                                    query.type = NPA_QUERY_TYPE_VALUE;
                                    // Get min frequency for this clock
                                    if(NPA_QUERY_SUCCESS == npa_query_by_client(
                                        pClkHalDesc->clkHandle.npaHandle, CLOCK_NPA_QUERY_MIN_FREQ_KHZ, &query))
                                    {
                                        ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_INFO,
                                            "NPA request for %s, state: %u",
                                            pClkHalDesc->pClkDescriptor->clkInfo.clkName, query.data.value);
                                        ADSPPM_QDSS_EVENT_3(ADSPPM_CLK_SET,
                                            clkId, query.data.value, query.data.value);
                                        // Issue NPA request for min frequency
                                        npa_issue_required_request(pClkHalDesc->clkHandle.npaHandle, query.data.value);
                                        pClkHalDesc->currentFreq = query.data.value;
                                        pClkHalDesc->currentState = Hal_ClkRgmEnable;
                                    }
                                    else
                                    {
                                        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,
                                            "NPA query returned failure");
                                        result = Adsppm_Status_Failed;
                                    }
                                }
                                else
                                {
                                    pClkHalDesc->currentState = Hal_ClkRgmEnable;
                                }
                            }
                            else
                            {
                                ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,
                                    "Complete NPA request for %s",
                                    pClkHalDesc->pClkDescriptor->clkInfo.clkName);
                                ADSPPM_QDSS_EVENT_3(ADSPPM_CLK_SET, clkId, 0, 0);

                                // Release NPA request
                                npa_complete_request(pClkHalDesc->clkHandle.npaHandle);
                                pClkHalDesc->currentFreq = 0;
                                pClkHalDesc->currentState = Hal_ClkRgmDisable;
                            }
                        }
                        else
                        {
                            ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
                                "NULL npa handle for %s",
                                pClkHalDesc->pClkDescriptor->clkInfo.clkName);
                            result = Adsppm_Status_Failed;
                        }
                        break;
                    case AsicClk_TypeInternalCGC:
                        // Enable clock source
                        if(AdsppmClk_None != pClkHalDesc->pClkDescriptor->clkSrcId)
                        {
                            // Recursive call here
                            // We are relying on the lock to work in recursive cases
                            // as it is stated in the API description
                            result = ClkRgm_EnableClock(pClkHalDesc->pClkDescriptor->clkSrcId, state);
                            if(Adsppm_Status_Success != result)
                            {
                                // Only break in the case of failure
                                break;
                            }
                        }
                        // No break statement here as we need to continue and enable/disable the clock branch
                    case AsicClk_TypeDalFreqSet:
                    case AsicClk_TypeDalEnable:
                    case AsicClk_TypeCBC:
                    case AsicClk_TypeDalDomainSrc:
                        if(Hal_ClkRgmEnable == state)
                        {
                            result = clkrgm_EnableClockBranch(clkId, pClkHalDesc);
                        }
                        else
                        {
                            result = clkrgm_DisableClockBranch(clkId, pClkHalDesc);
                        }
                        break;
                    default:
                        break;
                    }
                }

                // Unlock HW layer
                adsppm_unlock(gClkRgmCtx.lock);
            }
            else
            {
                ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
                    "invalid HalClk_ClockDescriptor for ClkId: %u",clkId);
                result = Adsppm_Status_BadParm;
            }
        }
        else
        {
            result = Adsppm_Status_NotInitialized;
        }
    }
    else
    {
        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
            "AdsppmClkId %u out of range",clkId);
        result = Adsppm_Status_BadParm;
    }

    ADSPPM_LOG_FUNC_PROFILEEND;
    return result;
}


Adsppm_Status ClkRgm_SelectClkSource(AdsppmClkIdType clkId, Hal_ClkRgmClkSrcType clkSrc)
{
    Adsppm_Status result = Adsppm_Status_Success;
    uint32 clkIdx =0;

    ADSPPM_LOG_FUNC_PROFILESTART;
    if((AdsppmClk_None < clkId) && (AdsppmClk_EnumMax > clkId))
    {
        if(NULL != gClkRgmCtx.lock)
        {
            if (ACM_GetClkIdx(clkId , &clkIdx) == Adsppm_Status_Failed)
                return Adsppm_Status_Failed;

            HalClk_ClockDescriptorType *pClkHalDesc = &gClkRgmCtx.clockDesc[clkIdx];

            if(NULL != pClkHalDesc->pClkDescriptor)
            {
                //Lock HW layer
                adsppm_lock(gClkRgmCtx.lock);

                if(pClkHalDesc->currentSrc != clkSrc)
                {
                    switch(pClkHalDesc->pClkDescriptor->clkType)
                    {
                    case AsicClk_TypeNpa:
                    case AsicClk_TypeDalFreqSet:
                    case AsicClk_TypeDalEnable:
                    case AsicClk_TypeDalDomainSrc:
                        ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_INFO,
                            "Requested clkSrc %u for %s",
                            clkSrc, pClkHalDesc->pClkDescriptor->clkInfo.clkName);
                        if(DAL_SUCCESS !=
                            DalClock_SelectClockSource(
                                gClkRgmCtx.pHandle, pClkHalDesc->clkHandle.clkId, (ClockSourceType) clkSrc))
                        {
                            ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
                                "ClkRegime returned failure for clksrc select for ClkId: %u",clkId);
                            result = Adsppm_Status_Failed;
                        }
                        else
                        {
                            pClkHalDesc->currentSrc = clkSrc;
                        }
                        break;
                    default:
                        break;
                    }
                }

                //Unlock HW layer
                adsppm_unlock(gClkRgmCtx.lock);
            }
            else
            {
                ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
                    "invalid HalClk_ClockDescriptor for ClkId: %u",clkId);
                result = Adsppm_Status_BadParm;
            }
        }
        else
        {
            result = Adsppm_Status_NotInitialized;
        }
    }
    else
    {
        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
            "AdsppmClkId out of range for ClkId: %u",clkId);
        result = Adsppm_Status_BadParm;
    }
    ADSPPM_LOG_FUNC_PROFILEEND;
    return result;
}


/**
 * @fn clkrgm_ReinitPwrDomain - trigger an interrupt to notify the power island is UP and wait for acknowledgment
 * @param pPwrHalDesc - pointer to power domain descriptor in Hal
 */
static Adsppm_Status clkrgm_ReinitPwrDomain(HalClk_PwrDomainDescriptorType *pPwrHalDesc)
{
    Adsppm_Status result = Adsppm_Status_Success;
    int i = 0;
    ADSPPM_LOG_FUNC_PROFILESTART;

    if((AsicFeatureState_Enabled == ACM_GetFeatureStatus(AsicFeatureId_LpassCore_PC_TZ_Handshake)) &&
        (0 == gDISABLETZHANDSHAKE))
    {
        if((pPwrHalDesc->pPwrDescriptor->intrReinitTrigger < DALIPCINT_NUM_INTS) &&
            (pPwrHalDesc->pPwrDescriptor->intrReinitDone > 0))
        {
            ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_INFO_EXT, "Acquiring event ownership");
            //Take event ownership so we can wait on the event
            if(DAL_SUCCESS != DALSYS_EventCtrl(pPwrHalDesc->evtSecure, DALSYS_EVENT_CTRL_ACCQUIRE_OWNERSHIP))
            {
                ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "Failed to acquire the event ownership");
                result = Adsppm_Status_Failed;
            }
            else
            {
                //Only try handshake if interrupts are specified
                if(DAL_SUCCESS != DALSYS_EventCtrl(pPwrHalDesc->evtSecure, DALSYS_EVENT_CTRL_RESET))
                {
                    ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "Failed to reset event before interrupt");
                    result = Adsppm_Status_Failed;
                }
                else
                {
                    //Enable clocks required for xPU access
                    //remember state of the clocks before enabling
                    if(NULL != pPwrHalDesc->pPwrDescriptor->securityClocks.pClocks)
                    {
                        for(i = 0; (i < pPwrHalDesc->pPwrDescriptor->securityClocks.numClocks) &&
                            (Adsppm_Status_Success == result); i++)
                        {
                            pPwrHalDesc->pSecurityClkState[i] =
                                gClkRgmCtx.clockDesc[pPwrHalDesc->pPwrDescriptor->securityClocks.pClocks[i]].currentState;
                            result = ClkRgm_EnableClock(
                                pPwrHalDesc->pPwrDescriptor->securityClocks.pClocks[i], Hal_ClkRgmEnable);
                        }
                    }
                    if(Adsppm_Status_Success == result)
                    {
                        //Trigger TZ interrupt
                        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_INFO, "Triggering TZ interrupt");
                        result = Intr_TriggerInterrupt(pPwrHalDesc->pPwrDescriptor->intrReinitTrigger);
                        if(Adsppm_Status_Success == result)
                        {
                            ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_INFO_EXT, "Waiting for TZ ack...");
                            if(DAL_SUCCESS ==DALSYS_EventWait(pPwrHalDesc->evtSecure))
                            {
                                ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_INFO, "Ack received from TZ");
                                if(DAL_SUCCESS != DALSYS_EventCtrl(pPwrHalDesc->evtSecure, DALSYS_EVENT_CTRL_RESET))
                                {
                                    ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,"Failed to reset event");
                                    result = Adsppm_Status_Failed;
                                }
                            }
                            else
                            {
                                ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "Event wait failed");
                                result = Adsppm_Status_Failed;
                            }
                        }
                        if(NULL != pPwrHalDesc->pPwrDescriptor->securityClocks.pClocks)
                        {
                            //Restore original state ofr clocks required for xPU access
                            for(i = 0; (i < pPwrHalDesc->pPwrDescriptor->securityClocks.numClocks) &&
                                (Adsppm_Status_Success == result); i++)
                            {
                                result = ClkRgm_EnableClock(
                                    pPwrHalDesc->pPwrDescriptor->securityClocks.pClocks[i],
                                    pPwrHalDesc->pSecurityClkState[i]);
                            }
                        }
                    }
                }
            }
        }
    }

    ADSPPM_LOG_FUNC_PROFILEEND;
    return result;
}


Adsppm_Status ClkRgm_RequestPower(AsicPowerDomainType pwrDomainId, Hal_ClkRgmEnableDisableType state)
{
    Adsppm_Status result = Adsppm_Status_Success;
    ADSPPM_LOG_FUNC_PROFILESTART;

    if(pwrDomainId < AsicPowerDomain_EnumMax)
    {
        uint32 pwrDomainDescIdx =0;

	    if (ACM_GetPwrDomainIdx(pwrDomainId , &pwrDomainDescIdx) == Adsppm_Status_Failed)
			return Adsppm_Status_BadParm;

        if(NULL != gClkRgmCtx.lock)
        {
            HalClk_PwrDomainDescriptorType *pPwrHalDesc = &gClkRgmCtx.pwrDesc[pwrDomainDescIdx];
            DALResult dalResult = DAL_SUCCESS;
            if(NULL != pPwrHalDesc->pPwrDescriptor)
            {
                //Lock HW layer
                adsppm_lock(gClkRgmCtx.lock);

                switch(pPwrHalDesc->pPwrDescriptor->pwrDomainType)
                {
                case AsicRsc_Power_ADSP:
                    // Use NPA node
                    // Require ClkRegime to be initialized before ADSPPM,
                    // so the npa clocks have to be available at this point
                    // Disabling voting now as the node is not available
                    if(NULL != pPwrHalDesc->pwrHandle.npaHandle)
                    {
                        if(Hal_ClkRgmEnable == state)
                        {
                            //issue NPA request for power
                            npa_issue_required_request(pPwrHalDesc->pwrHandle.npaHandle, ADSPPM_ADSP_POWER_ENABLE);
                            ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_INFO,
                                "Enabled ADSP power domain, NPA node: %s @ %u",
                                pPwrHalDesc->pPwrDescriptor->pwrDomainName, ADSPPM_ADSP_POWER_ENABLE);
                            ADSPPM_QDSS_EVENT_1(ADSPPM_PWR_REQ, pwrDomainId);
                        }
                        else
                        {
                            //release NPA request
                            npa_complete_request(pPwrHalDesc->pwrHandle.npaHandle);
                            ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,
                                "Disabled ADSP power domain, NPA node: %s",
                                pPwrHalDesc->pPwrDescriptor->pwrDomainName);
                            ADSPPM_QDSS_EVENT_1(ADSPPM_PWR_REL, pwrDomainId);
                        }
                    }
                    else
                    {
                        //result = Adsppm_Status_Failed;
                        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "ADSP power NPA handle is NULL");
                    }
                    break;
                case AsicRsc_Power_Mem:
                    //Call clock regime to enable/disable mem core retention
                    result = clkrgm_MemCore_Retention(
                        pPwrHalDesc->pPwrDescriptor->clkId,
                        (state == Hal_ClkRgmEnable)?Hal_ClkRgmMemRetain:Hal_ClkRgmMemCollapse);
                    break;
                case AsicRsc_Power_Core:
                    //Call clock regime to enable/disable power domain
                    //only call disable if the LPASS_Core PC feature is enabled
                    //otherwise keep LPASS Core power enabled
                    if(Hal_ClkRgmEnable == state)
                    {
                        dalResult = DalClock_EnablePowerDomain(gClkRgmCtx.pHandle, pPwrHalDesc->pwrHandle.domainId);
                        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,
                            "Enabled power domain %s", pPwrHalDesc->pPwrDescriptor->pwrDomainName);
                        ADSPPM_QDSS_EVENT_1(ADSPPM_PWR_REQ, pwrDomainId);
                        if(DAL_SUCCESS == dalResult)
                        {
#ifndef VAPSS_GDHS_CLOSE
                          if(Adsppm_Status_NotInitialized == Slp_RequestRpmLpm (SLEEP_RPM_LPR_DISABLE_ALL))
                          {
                             ADSPPM_LOG_PRINTF_0 (ADSPPM_LOG_LEVEL_INFO,
                                                 "Disabling Subsystem-level modes failed");
                             result = Adsppm_Status_NotInitialized;
                          }
#endif                         
                          //Trigger TZ to reprogram LPASS Core xPUs
                          result = clkrgm_ReinitPwrDomain(pPwrHalDesc);
                        }

                        // CR 806459: ADSPPM to set SLIMBus CGC in LPASS core under SW control
                        //            and enable it during LPASS core power up
                        // HW Bug: SLIMBus CGC gets busy signal from only SLIMBus, but not from BAM.
                        //         During Enumeration, BAM becomes active before the SLIMBus
                        //         and needs the CGC ON, but since it is in HW control
                        //         and the reason above, CGC won't turn ON.
                        // SW WA: To disable HW control on LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CGCR
                        //        and set clock enable bit high at every LPASS core power up.
                        //        This ensures CGC is ON when LPASS core is up.
                        //        According to HW team, power impact is minimal.
                        //
                        // Disable HW control
						if(ACM_IsClkIdValid(AdsppmClk_Slimbus_Hclk))
	                        HalHwIo_DisableHWClockCntl(AdsppmClk_Slimbus_Hclk);
						if(ACM_IsClkIdValid(AdsppmClk_Slimbus2_Hclk))
	                        HalHwIo_DisableHWClockCntl(AdsppmClk_Slimbus2_Hclk);
                        // Enable clock
						if(ACM_IsClkIdValid(AdsppmClk_Slimbus_Hclk))
	                        HalHwIo_EnableClock(AdsppmClk_Slimbus_Hclk);
						if(ACM_IsClkIdValid(AdsppmClk_Slimbus2_Hclk))
	                        HalHwIo_EnableClock(AdsppmClk_Slimbus2_Hclk);
                        // END of CR 806459
                    }
                    else
                    {
#ifdef VAPSS_GDHS_CLOSE
                        dalResult = DalClock_DisablePowerDomain(gClkRgmCtx.pHandle, pPwrHalDesc->pwrHandle.domainId);
                        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,
                            "Disabled power domain %s", pPwrHalDesc->pPwrDescriptor->pwrDomainName);
                        ADSPPM_QDSS_EVENT_1(ADSPPM_PWR_REL, pwrDomainId);							
							

#else
	                    dalResult = DalClock_DisablePowerDomain(gClkRgmCtx.pHandle, pPwrHalDesc->pwrHandle.domainId);
                        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,
                            "Disabled power domain %s", pPwrHalDesc->pPwrDescriptor->pwrDomainName);
                        ADSPPM_QDSS_EVENT_1(ADSPPM_PWR_REL, pwrDomainId);
						if(Adsppm_Status_NotInitialized == Slp_RequestRpmLpm (SLEEP_RPM_LPR_ALLOW_ALL))
                        {
                            ADSPPM_LOG_PRINTF_0 (ADSPPM_LOG_LEVEL_INFO,
                                                 "Enabling Subsystem-level modes failed");
                            result = Adsppm_Status_NotInitialized;
                        }
#endif
                    }
                    if(DAL_SUCCESS != dalResult)
                    {
                        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "ClkRegime returned failure");
                        result = Adsppm_Status_Failed;
                    }
                    break;

                case AsicRsc_Power_Hvx:

                        if(Hal_ClkRgmEnable == state)
                        {
                            // Issue request for power
                            if (DAL_ERROR == DalVCS_EnableHVX(&pPwrHalDesc->pwrHandle.vcsHandle))
                            {
                              ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,
                                                  "Enabled HVX power domain failed"); 
                              break;
                            }
                            ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_INFO,
                                "Enabled HVX power domain");
                            sysmon_qdsppm_hvxthrottle_config();
                            ADSPPM_QDSS_EVENT_1(ADSPPM_PWR_REQ, pwrDomainId);
#ifdef CDSP_HVX_LPR
                            gHvxTurnedOffRequested = FALSE;
                            // Issue NPA request for sleep LPR
                            npa_issue_required_request(gHvxNpaHandle, 1);
                            ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_INFO,
                                "Enabled HVX power domain, Sleep higher latency modes enabled");

#endif
                        }
                        else
                        {
#ifdef CDSP_HVX_LPR
                           // Release NPA request
                           if (DAL_ERROR == DalVCS_DisableHVX(&pPwrHalDesc->pwrHandle.vcsHandle))
                           {
                             ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,
                                              "Disabled HVX power domain failed"); 
                             break;
                           }
                           ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_INFO,
                                                 "Disabled HVX power domain");
                           ADSPPM_QDSS_EVENT_1(ADSPPM_PWR_REL, pwrDomainId);
                           // Issue NPA request for sleep LPR
                           npa_complete_request (gHvxNpaHandle);
                           ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_INFO,
                                               " Sleep higher latency modes disabled");

                           gHvxTurnedOffRequested = TRUE;
                           ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_INFO,
                                                   " Client requested for HVX off, but no action");
                            
#else
                            // Release NPA request
                            if (DAL_ERROR == DalVCS_DisableHVX(&pPwrHalDesc->pwrHandle.vcsHandle))
                            {
                              ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,
                                               "Disabled HVX power domain failed"); 
                              break;
                            }

                               ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_INFO,
                                                     "Disabled HVX power domain");
                            ADSPPM_QDSS_EVENT_1(ADSPPM_PWR_REL, pwrDomainId);

                            
#endif

                        }


                    break;

                default:
                    break;
                }

                //Unlock HW layer
                adsppm_unlock(gClkRgmCtx.lock);
            }
            else
            {
                ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR, "invalid HalClk_PwrDomainDescriptor for PowerDomainId: %u",pwrDomainId);
                result = Adsppm_Status_BadParm;
            }
        }
        else
        {
            ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "Not initialized");
            result = Adsppm_Status_NotInitialized;
        }
    }
    else
    {
        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR, "AsicPowerDomainId out of range for PwrDomainId : %u",pwrDomainId);
        result = Adsppm_Status_BadParm;
    }

    ADSPPM_LOG_FUNC_PROFILEEND;
    return result;
}


Adsppm_Status ClkRgm_MemoryControl(AdsppmMemIdType memCore, Hal_ClkRgmMemStateType pwrState)
{
    Adsppm_Status result = Adsppm_Status_Success;
    uint32 pwrDomainDescIdx =0;

    ADSPPM_LOG_FUNC_PROFILESTART;

    if((Adsppm_Mem_None < memCore) && (Adsppm_Mem_Max > memCore))
    {
        if(NULL != gClkRgmCtx.lock)
        {
            AsicPowerDomainType pwrDomain = ACMPwr_GetMemPowerDomain(memCore);

			if (ACM_GetPwrDomainIdx(pwrDomain , &pwrDomainDescIdx) == Adsppm_Status_Failed)
				return Adsppm_Status_BadParm;

            adsppm_lock(gClkRgmCtx.lock);
            //Call clock regime to enable/disable mem core retention
            result = clkrgm_MemCore_Retention(
                gClkRgmCtx.pwrDesc[pwrDomainDescIdx].pPwrDescriptor->clkId, pwrState);
            adsppm_unlock(gClkRgmCtx.lock);
        }
        else
        {
            ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "Not initialized");
            result = Adsppm_Status_NotInitialized;
        }
    }
    else
    {
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "AdsppmMemId out of range");
        result = Adsppm_Status_BadParm;
    }
    ADSPPM_LOG_FUNC_PROFILEEND;
    return result;
}

