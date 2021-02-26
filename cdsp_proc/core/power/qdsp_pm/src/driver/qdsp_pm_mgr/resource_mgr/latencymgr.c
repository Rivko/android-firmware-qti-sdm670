/*
* Copyright (c) 2014 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
 * latencymgr.c
 *
 *  Created on: Feb 13, 2012
 *      Author: yrusakov
 */

#include "adsppm.h"
#include "adsppm_utils.h"
#include "core.h"
#include "core_internal.h"
#include "requestmgr.h"
#include "latencymgr.h"
#include "hal_sleep.h"


/**
 * Definition of the Power Manager Context.
 */
typedef struct
{
    DALSYSSyncHandle lock; //!< Lock for the sleep context
    uint32           latencyUs[2]; //!< current/previous latency
    uint32           activeStateIndex; //!< active state index
} sleepMgrCtxType;

static sleepMgrCtxType gSleepMgrCtx; //!< Global context for the power manager


Adsppm_Status SLEEP_Init(void)
{
    Adsppm_Status result = Adsppm_Status_Success;
    gSleepMgrCtx.latencyUs[0] = HAL_SLEEP_LATENCY_DONT_CARE;
    gSleepMgrCtx.latencyUs[1] = HAL_SLEEP_LATENCY_DONT_CARE;
    gSleepMgrCtx.activeStateIndex = 0;
    if(DAL_SUCCESS != DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE, &gSleepMgrCtx.lock, NULL))
    {
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "Failed to create lock");
        result = Adsppm_Status_Failed;
    }
    return result;
}


Adsppm_Status SLEEP_ProcessRequest(coreUtils_Q_Type *pSlpReqQ)
{
    Adsppm_Status result = Adsppm_Status_Success;
    RMHandleType *pRMHandle = NULL;
    
    if(NULL == pSlpReqQ)
    {
        result = Adsppm_Status_BadParm;
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "Bad parameter");
    }
    else
    {
        if(NULL == gSleepMgrCtx.lock)
        {
            result = Adsppm_Status_NotInitialized;
            ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "Not initialized");
        }
        else
        {
            int index = 0;
            adsppm_lock(gSleepMgrCtx.lock);
            ADSPPM_LOCK_RESOURCE(Adsppm_Rsc_Id_Sleep_Latency);
            // Clear current state
            gSleepMgrCtx.latencyUs[gSleepMgrCtx.activeStateIndex] = HAL_SLEEP_LATENCY_DONT_CARE;

            // Loop through all clients
            pRMHandle = (RMHandleType *)coreUtils_Q_Check(pSlpReqQ);
            for(index = 0; index < pSlpReqQ->nCnt; index++)
            {
                if(NULL != pRMHandle)
                {
                    if((RM_Valid == pRMHandle->validFlag) && (NULL != pRMHandle->pRequestData))
                    {
                        // If request is valid that means that power has to be On
                        gSleepMgrCtx.latencyUs[gSleepMgrCtx.activeStateIndex] = MIN(
                            gSleepMgrCtx.latencyUs[gSleepMgrCtx.activeStateIndex],
                            *((uint32*)pRMHandle->pRequestData));
                    }
                }
                pRMHandle = (RMHandleType *)coreUtils_Q_Next(pSlpReqQ, &pRMHandle->link);
            }

            ADSPPM_UNLOCK_RESOURCE(Adsppm_Rsc_Id_Sleep_Latency);

            ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,
                "Aggregated latency value: %d",
                gSleepMgrCtx.latencyUs[gSleepMgrCtx.activeStateIndex]);

            if(gSleepMgrCtx.latencyUs[gSleepMgrCtx.activeStateIndex] !=
                gSleepMgrCtx.latencyUs[gSleepMgrCtx.activeStateIndex^1])
            {
                // Only Call Sleep wrapper if the state changes
                result = Slp_RequestLatency(gSleepMgrCtx.latencyUs[gSleepMgrCtx.activeStateIndex]);
            }

            // Flip active/previous request
            gSleepMgrCtx.activeStateIndex ^= 1;
            adsppm_unlock(gSleepMgrCtx.lock);
        }
    }
    
    return result;
}

