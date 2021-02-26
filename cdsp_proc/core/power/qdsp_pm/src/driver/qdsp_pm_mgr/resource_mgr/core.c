/*
* Copyright (c) 2014 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
 * core.c
 *
 *  Created on: Feb 13, 2012
 *      Author: yrusakov
 */

#include "core.h"
#include "adsppm_utils.h"
#include "core_internal.h"
#include "clkmgr.h"
#include "mipsmgr.h"
#include "pwrmgr.h"
#include "latencymgr.h"
#include "thermalmgr.h"
#include "requestmgr.h"
#include "busmgr.h"
#include "ahbmgr.h"
#include "mempwrmgr.h"
#include "clientmgr.h"
#include "cachemgr.h"
#include "qosmgr.h"
#include "dcvsmgr.h"
#include "extbwmgr.h"
#include "adspclkmgr.h"
#include "usleepmgr.h"


typedef struct
{
    Adsppm_Status (*func)(void);
    AdsppmInitStateType state;
} AdsppmInitFunctionInfoType;

// Table of initialization functions for all ADSPPM sub-modules
static AdsppmInitFunctionInfoType initFunctions[] =
{
    { RM_Init,          Adsppm_State_CoreRMInit },
    { MIPS_Init,        Adsppm_State_CoreMIPSInit },
    { BusMgr_Init,      Adsppm_State_CoreBUSInit },
    { PWR_Init,         Adsppm_State_CorePWRInit },
    { CLK_Init,         Adsppm_State_CoreCLKInit },
    { SLEEP_Init,       Adsppm_State_CoreSLEEPInit },
    { THERMAL_Init,     Adsppm_State_CoreTHERMALInit },
    { MEMPWR_Init,      Adsppm_State_CoreMEMPWRInit },
    { CM_Init,          Adsppm_State_CoreCMInit },
    { CPM_Init,         Adsppm_State_CoreCPMInit },
    { DcvsMgr_Init,     Adsppm_State_CoreDCVSInit },
    { ExtBwMgr_Init,    Adsppm_State_CoreEXTBWInit },
    { AdspClkMgr_Init,  Adsppm_State_CoreADSPCLKInit },
    { busMgrRegisterEvents,  Adsppm_State_CoreBMRegisterEvent },
    { AHBM_Init,        Adsppm_State_CoreAHBMInit },
    { QCM_Init,         Adsppm_State_CoreQCMInit },
    { scpmInit,             Adsppm_State_CoreSysCacheInit },
    { USleepMgr_Init,   Adsppm_State_CoreUSLEEPInit}
};

static AdsppmCoreCtxType gAdsppmCoreCtx;

extern uint32 gADSPPMINITSTATE;


/**
 * @fn GetAdsppmCoreContext - retrieves pointer to the global ADSPPM context
 * @return pointer to the global ADSPPM context
 */
AdsppmCoreCtxType *GetAdsppmCoreContext(void)
{
    return &gAdsppmCoreCtx;
}


uint32 IsPeriodicUseCase(void)
{
    return gAdsppmCoreCtx.periodicUseCase;
}


void SetPeriodicUseCase(uint32 periodic)
{
    gAdsppmCoreCtx.periodicUseCase = periodic;
}


/**
 * @fn Core_EnterSleep - Prepare LPASS resources for power collapse.
 * Currently it will just ramp down AHB clock.
 * TODO: Handle the clock gates as well
 * @return completion status
 */
Adsppm_Status Core_EnterSleep(void)
{
    Adsppm_Status result = Adsppm_Status_Success;
	
#ifdef BUSMGR_AHB_BW_SUPPORT    
    result = AHBM_SetSleepAHBClock();
#endif
    
    return result;
}


/**
 * @fn Core_ExitSleep - Restore LPASS resources after power collapse.
 * Currently it will just ramp up AHB clock.
 * TODO: Handle the clock gates as well
 * @return completion status
 */
Adsppm_Status Core_ExitSleep(void)
{
    Adsppm_Status result = Adsppm_Status_Success;
	
#ifdef BUSMGR_AHB_BW_SUPPORT    
    result = AHBM_SetActiveAHBClock();
#endif
    
    return result;
}


/**
 * @fn Core_Init - Initialize all components within ADSPPM Core
 * @return completion status @see Adsppm_Status
 */
Adsppm_Status Core_Init(void)
{
    Adsppm_Status result = Adsppm_Status_Success;
    
    memset(&gAdsppmCoreCtx, 0, sizeof(AdsppmCoreCtxType));

    // Set periodic usecase by default to ensure quick powerdown
    gAdsppmCoreCtx.periodicUseCase = 1;

    // Initialize sub-components of ADSPPM
    for(int n = 0; n < ADSPPM_ARRAY_SIZE(initFunctions); n++)
    {
        result = initFunctions[n].func();
        if(result == Adsppm_Status_Success)
        {
            gADSPPMINITSTATE |= initFunctions[n].state;
        }
        else
        {
            break;
        }
    }

    if((Adsppm_Status_Success == result) &&
        (ACM_GetFeatureStatus(AsicFeatureId_InitialState) == 
            AsicFeatureState_Enabled))
    {
        AdsppmRscIdType rsc;
        for(rsc = Adsppm_Rsc_Id_Start; rsc < Adsppm_Rsc_Id_Max; rsc++)
        {
            result = RM_RequestResource(rsc);
            if(Adsppm_Status_Success != result)
            {
                // Put failed resource ID in high 4 bits
                gADSPPMINITSTATE |= rsc << 28;
                break;
            }
        }
    }

    return result;
}

