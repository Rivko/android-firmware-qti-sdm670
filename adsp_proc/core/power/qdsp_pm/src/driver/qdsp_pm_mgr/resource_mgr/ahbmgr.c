/*
* Copyright (c) 2015 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
@file: ahbmgr.c
@brief: ADSPPM ahb manager implementation.

$Header: //components/rel/core.qdsp6/2.1/power/qdsp_pm/src/driver/qdsp_pm_mgr/resource_mgr/ahbmgr.c#4 $
*/

#include "adsppm.h"
#include "adsppm_utils.h"
#include "asic.h"
#include "ahbmgr.h"
#include "core_internal.h"
#include "hal_clkrgm.h"
#include "adspclkmgr.h"
#include "asic.h"
#include "asic_internal.h"
#include "pwrmgr.h"


#define AHBMGR_XO_FREQ_HZ 19200000


/**
 * @struct ahb management context
 */
typedef struct
{
   uint32           preAHBClock;        //!< AHBIX Clock currently set
   uint32           activeAHBClock;     //!< AHBIX Clock calculated based on aggregatedAHBClock accounting for execution state (periodic vs non-periodic)
   uint32           aggregatedAHBClock; //!< AHBIX Clock calculated based on BW requirements only for all clients
   uint32           nonSuppressibleAHBClock; //!< AHBIX Clock calculated based on BW requirements only for non-suppressible clients
   uint32           AhbIX_Q6Clock;       //!< AHBIX Current Q6 vote on AHB-IX
   
   uint32           preAhbEClock;         //!< AHBE/Noc Clock currently set
   uint32           aggrNocAHBEClock ;    //!< AHBE/Noc Clock based on BW requirements only for all clients
   uint32           AhbE_Q6Clock;         //!< AHBE/Noc Current Q6 vote on AHB-E
   
   DALSYSSyncHandle lock;
} ahbMgrCtxType;

static ahbMgrCtxType gAhbMgrCtx;


static Adsppm_Status ahbm_SetAHBIXClock(ahbMgrCtxType* self, uint32 ahbixClock)
{
    Adsppm_Status sts = Adsppm_Status_Success;
    if (self->preAHBClock != ahbixClock &&
       (ACM_IsClkIdValid(AdsppmClk_VAPSS_TCMS) || ACM_IsClkIdValid(AdsppmClk_Ahb_Root)))
    {
        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO_EXT, "AHBIX clock: %u", ahbixClock);

        if(ACM_GetFeatureStatus(AsicFeatureId_ComputeDSP) == AsicFeatureState_Enabled)
        {
            if (ACM_IsClkIdValid(AdsppmClk_VAPSS_TCMS))
                sts = ClkRgm_SetClock(AdsppmClk_VAPSS_TCMS, ahbixClock);
        }
        else
        {
            if (ACM_IsClkIdValid(AdsppmClk_Ahb_Root))
                sts = ClkRgm_SetClock(AdsppmClk_Ahb_Root, ahbixClock);
        }

        if(sts == Adsppm_Status_Success)
        {
            self->preAHBClock = ahbixClock;
        }
    }
    return sts;
}

static Adsppm_Status ahbm_SetAHBEClock(ahbMgrCtxType* self, uint32 ahbeClock)
{
    Adsppm_Status sts = Adsppm_Status_Success;
    if((self->preAhbEClock != ahbeClock) && ACM_IsClkIdValid(AdsppmClk_AhbE_Hclk))
    {
        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO_EXT, "AHBE clock: %u", ahbeClock);
        sts = ClkRgm_SetClock(AdsppmClk_AhbE_Hclk, ahbeClock);
        if(sts == Adsppm_Status_Success)
        {
            self->preAhbEClock = ahbeClock;
        }
    }
    return sts;
}
       

static void tryAhbFrequencyUpdate(ahbMgrCtxType* self, AsicBusType busType)
{
    Adsppm_Status sts = Adsppm_Status_Success;
    
    uint32 adspFreq = 0;
    uint32 newAhbClockFreq = 0;
     
    
    // Get current ADSP frequency
    if(AdspClkMgr_GetAdspClockFreqVote(&adspFreq) == Adsppm_Status_Success)
    {
        // Look up AHB-IX/AHBE frequency using ADSP frequency
        if(ACM_GetAhbFromAdspFreq(adspFreq, busType, &newAhbClockFreq) ==
            Adsppm_Status_Success)
        {
            adsppm_lock(self->lock);
            
            switch(busType){
        
                case AsicBusType_AhbIx:
                    self->AhbIX_Q6Clock = newAhbClockFreq;
                    
                    ADSPPM_LOG_PRINTF_3(ADSPPM_LOG_LEVEL_INFO,
                    "ADSP: %d Hz -> AHBIX vote: %d Hz, Final AHBIX vote:%d Hz", adspFreq, newAhbClockFreq, 
                                                                                  MAX(self->activeAHBClock, newAhbClockFreq));
                    
                    sts = ahbm_SetAHBIXClock(self, MAX(self->activeAHBClock, newAhbClockFreq));
                    break;
                    
                case AsicBusType_AhbE:
                    self->AhbE_Q6Clock = newAhbClockFreq;
                    
                    ADSPPM_LOG_PRINTF_3(ADSPPM_LOG_LEVEL_INFO,
                    "ADSP: %d Hz -> AHBE vote: %d Hz, Final AHBE vote:%d Hz", adspFreq, newAhbClockFreq, 
                                                                                  MAX(self->aggrNocAHBEClock, newAhbClockFreq));
                    
                    sts = ahbm_SetAHBEClock(self, MAX(self->aggrNocAHBEClock, newAhbClockFreq));
                    break;
                default:
                    ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,"Invalid Bus Type");
                    break;
            }
            
            adsppm_unlock(self->lock);
        }
    }

}

static void AHBM_scaleAhb(ahbMgrCtxType* self){
        
    if(ACM_GetFeatureStatus(AsicFeatureId_AhbIX_Q6Scaling) ==
        AsicFeatureState_Enabled && pwrMgr_lpassCoreEnabled())
    {
        tryAhbFrequencyUpdate(self, AsicBusType_AhbIx);
    }

    if(ACM_GetFeatureStatus(AsicFeatureId_AhbE_Q6Scaling) ==
        AsicFeatureState_Enabled)
    {
        tryAhbFrequencyUpdate(self, AsicBusType_AhbE);
    }   
}


// Handler for ADSP clock change events.  This updates the AHB bus frequency
// if there is a change required due to the ADSP clock change.
static void onAdspClockChange(AdsppmAdspClkManagerEventId event, void* param)
{
    ahbMgrCtxType* self = (ahbMgrCtxType*) param;
    if(event == ADSPCLKMGR_CLOCK_CHANGE_EVENT)
    {
        AHBM_scaleAhb(self);
    }
}


/**
 * @fn AHBM_Init - Initialize the AHB Manager.  Requires the ADSP Clock Manager
 *                 to be initialized already.
 * @return Adsppm_Status_Failed
 * @return Adsppm_Status_Success
 */
Adsppm_Status AHBM_Init(void)
{
    Adsppm_Status sts = Adsppm_Status_Success;
    ahbMgrCtxType* self = &gAhbMgrCtx;
    
    self->preAHBClock = 0;
    self->preAhbEClock = 0;
    self->activeAHBClock = 0;
    self->aggregatedAHBClock = 0;
    self->nonSuppressibleAHBClock = 0;
    self->AhbIX_Q6Clock = 0;
    self->AhbE_Q6Clock = 0;
    self->aggrNocAHBEClock = 0;
    
    self->lock = NULL;

    if(DAL_SUCCESS != DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE, (DALSYSSyncHandle *)&self->lock, NULL))
    {
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "Failed to create lock");
        sts = Adsppm_Status_Failed;
    }
    
    AHBM_scaleAhb(self);
    
    if((ACM_GetFeatureStatus(AsicFeatureId_AhbIX_Q6Scaling) == AsicFeatureState_Enabled) ||
       (ACM_GetFeatureStatus(AsicFeatureId_AhbE_Q6Scaling) == AsicFeatureState_Enabled))
    {
        // Register for any subsequent changes to ADSP core frequency if any of the feature flag is enabled
        AdspClkMgr_RegisterEvent(ADSPCLKMGR_CLOCK_CHANGE_EVENT,
            onAdspClockChange,self);
    }
    
    return sts;
}

Adsppm_Status AHBM_SetSleepAHBClock(void)
{
    Adsppm_Status sts = Adsppm_Status_Success;
    ahbMgrCtxType* self = &gAhbMgrCtx;

    // Scale down AHBIX clock

    // WORKAROUND: for HW issue QCTDD00667975
    // Slimbus requires AHB bus freq to be at least 7.4 MHz even in sleep
    // The following logic uses adsppm internal counters to check whether slimbus core clocks are ON
    // If they are found to be ON, choose the max between 7.4 MHz and current aggregated ahb freq
    // Otherwise, use current aggregated ahb freq

    uint32 slimbusCore1ClockFreq = 0;
    uint32 slimbusCore2ClockFreq = 0;
    uint32 sleepAhbClockFreq = self->nonSuppressibleAHBClock;

    // Note: Targets with no Slimbus HW will return Freq = 0 on ClkRgm_GetClockFrequency() call
	if (ACM_IsClkIdValid(AdsppmClk_Slimbus_Core))
		ClkRgm_GetClockFrequency(AdsppmClk_Slimbus_Core, &slimbusCore1ClockFreq);

	if (ACM_IsClkIdValid(AdsppmClk_Slimbus2_Core))
		ClkRgm_GetClockFrequency(AdsppmClk_Slimbus2_Core, &slimbusCore2ClockFreq);

	sleepAhbClockFreq = ((0 < slimbusCore1ClockFreq) || (0 < slimbusCore2ClockFreq))?
				MAX(self->nonSuppressibleAHBClock, 7400000):
				self->nonSuppressibleAHBClock;
    
	sts = ahbm_SetAHBIXClock(self, sleepAhbClockFreq);

    // Scale down AHBE clock
    if(sts == Adsppm_Status_Success)
    {
        sts = ahbm_SetAHBEClock(self, AHBMGR_XO_FREQ_HZ);
    }

    return sts;
}

Adsppm_Status AHBM_SetActiveAHBClock(void)
{
    Adsppm_Status sts = Adsppm_Status_Success;
    ahbMgrCtxType* self = &gAhbMgrCtx;

    ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,
                           "Setting AHB-IX=%u",
                           (uint32)MAX(self->activeAHBClock,self->AhbIX_Q6Clock));
    // Restore AHBIX clock
    sts = ahbm_SetAHBIXClock(self, MAX(self->activeAHBClock,self->AhbIX_Q6Clock));
    

    // Restore AHBE clock
    if(sts == Adsppm_Status_Success)
    {
         ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,
                             "Setting AHB-E=%u",
                             (uint32)MAX(self->aggrNocAHBEClock,self->AhbE_Q6Clock));

        sts = ahbm_SetAHBEClock(self, MAX(self->aggrNocAHBEClock,self->AhbE_Q6Clock));
    }

    return sts;
}

Adsppm_Status AHBM_AdjustAndSetAHBClock(void)
{
    Adsppm_Status sts = Adsppm_Status_Success;
    adsppm_lock(gAhbMgrCtx.lock);

    // Adjust AHB clock if ADSP is active
    // The timeline optimization by keeping AHB clock not lower than predefined frequency (19.2MHz) is applied
    // also to non-periodic clients in order to compensate for inadequate BW voting from audio clients.
    // Periodic usecases may have different (higher) AHB clock requirements to optimize their timeline
    if((AsicFeatureState_Enabled == ACM_GetFeatureStatus(AsicFeatureId_TimelineOptimisationAhb)) && IsPeriodicUseCase())
    {
        // Select baseline or aggregated value if it is higher
        gAhbMgrCtx.activeAHBClock =
            ACM_AdjustParamValue(AsicFeatureId_TimelineOptimisationAhb, gAhbMgrCtx.aggregatedAHBClock);
    }
    else
    {
        gAhbMgrCtx.activeAHBClock = gAhbMgrCtx.aggregatedAHBClock;
    }

    sts = AHBM_SetActiveAHBClock();
    adsppm_unlock(gAhbMgrCtx.lock);
    return sts;
}

/**
 * @fn AHBM_ProcessRequest - Convert AHB bandwidth  to AHB clock
 * @param input AdsppmBusBWDataIbAbType *pCurAHBBwData
 * @param input AdsppmBusBWDataIbAbType *pNonSuppressibleAhbBW
 * @return Adsppm_Status
 */
Adsppm_Status AHBM_ProcessRequest(AdsppmBusBWDataIbAbType *pCurAHBBwData, AdsppmBusBWDataIbAbType *pNonSuppressibleAhbBW,AdsppmBusBWDataIbAbType *pNocAhbBW)
{
    Adsppm_Status sts = Adsppm_Status_Success;

    if((NULL == pCurAHBBwData) || (NULL == pNonSuppressibleAhbBW))
    {
        ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_ERROR,
            "pCurAHBBwData=%u, pNonSuppressibleAhbBW=%u",
            pCurAHBBwData, pNonSuppressibleAhbBW);
        sts = Adsppm_Status_BadParm;
    }
    else
    {
        if(NULL == gAhbMgrCtx.lock)
        {
            ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "Not initialized");
            sts = Adsppm_Status_NotInitialized;
        }
        else
        {
            adsppm_lock(gAhbMgrCtx.lock);
            
            ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_INFO,
                "Aggregated BW: Ab=%u, Ib=%u",
                (uint32)(pCurAHBBwData->Ab), (uint32)(pCurAHBBwData->Ib));
            ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_INFO,
                "Aggregated non-suppressible BW: Ab=%u, Ib=%u",
                (uint32)(pNonSuppressibleAhbBW->Ab), (uint32)(pNonSuppressibleAhbBW->Ib));
            
            ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_INFO,
                "Aggregated Noc BW: Ab=%u, Ib=%u",
                (uint32)(pNocAhbBW->Ab), (uint32)(pNocAhbBW->Ib));

            sts = ACM_GetClockFromBW(&gAhbMgrCtx.nonSuppressibleAHBClock, pNonSuppressibleAhbBW);
            if(Adsppm_Status_Success == sts)
            {
                // Adjust BW based on feature config
                pCurAHBBwData->Ib = ACM_AdjustParamValue(AsicFeatureId_Ahb_Scaling, pCurAHBBwData->Ib);
                sts = ACM_GetClockFromBW(&gAhbMgrCtx.aggregatedAHBClock, pCurAHBBwData);
            }
            if((ACM_GetFeatureStatus( AsicFeatureId_AhbE_VoteScaling) == AsicFeatureState_Enabled) &&
                Adsppm_Status_Success == sts)
            {
                sts = ACM_GetClockFromBW(&gAhbMgrCtx.aggrNocAHBEClock, pNocAhbBW);
            }

            adsppm_unlock(gAhbMgrCtx.lock);

            if(Adsppm_Status_Success == sts)
            {
                sts = AHBM_AdjustAndSetAHBClock();
            }
        }
    }

    return sts;
}

Adsppm_Status AHBM_GetInfo(AdsppmInfoAhbType* ahbInfo)
{
    Adsppm_Status status = Adsppm_Status_Failed;
    if(ahbInfo != NULL)
    {
        ahbMgrCtxType* self = &gAhbMgrCtx;
        adsppm_lock(self->lock);
        ahbInfo->ahbeFreqHz = self->preAhbEClock;
        status = Adsppm_Status_Success;
        adsppm_unlock(self->lock);
    }
    return status;
}

