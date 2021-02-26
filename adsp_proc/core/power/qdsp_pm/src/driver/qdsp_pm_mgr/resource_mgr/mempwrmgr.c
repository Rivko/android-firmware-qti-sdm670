/*
* Copyright (c) 2014 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
 * pwrmgr.c
 *
 *  Created on: Feb 13, 2012
 *      Author: yrusakov
 */

#include "adsppm.h"
#include "adsppm_utils.h"
#include "asic.h"
#include "mempwrmgr.h"
#include "DALSys.h"
#include "hal_clkrgm.h"
#include "requestmgr.h"


/**
 * Definition of the mem Power Manager Context.
 */
typedef struct
{
    DALSYSSyncHandle        lock;
    AdsppmMemPowerStateType memPwrState[Adsppm_Mem_Max - Adsppm_Mem_Lpass_LPM][2]; // Only for internal memories
    uint32                  activeStateIndex;
} memPwrMgrCtxType;

static memPwrMgrCtxType gMemPwrMgrCtx; //!< Global context for the mem power manager


Adsppm_Status MEMPWR_Init(void)
{
    Adsppm_Status result = Adsppm_Status_Success;
    uint32 j;

    gMemPwrMgrCtx.activeStateIndex = 0;
    if(DAL_SUCCESS != DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE, &gMemPwrMgrCtx.lock, NULL))
    {
        result = Adsppm_Status_Failed;
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "Failed to create mutex");
    }
    for(j = 0; j < (Adsppm_Mem_Max - Adsppm_Mem_Lpass_LPM); j++)
    {
        gMemPwrMgrCtx.memPwrState[j][gMemPwrMgrCtx.activeStateIndex] = Adsppm_Mem_Power_Off;
        gMemPwrMgrCtx.memPwrState[j][gMemPwrMgrCtx.activeStateIndex^1] = Adsppm_Mem_Power_Off;
    }
    
    return result;
}


Adsppm_Status MEMPWR_ProcessRequest(coreUtils_Q_Type *pMemPwrReqQ)
{
    Adsppm_Status result = Adsppm_Status_Success;
    RMHandleType *pRMHandle = NULL;

    if(NULL == pMemPwrReqQ)
    {
        result = Adsppm_Status_BadParm;
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "Bad parameter");
    }
    else
    {
        if(NULL != gMemPwrMgrCtx.lock)
        {
            int index = 0;
            adsppm_lock(gMemPwrMgrCtx.lock);
            ADSPPM_LOCK_RESOURCE(Adsppm_Rsc_Id_MemPower);
            for(index = 0; index < (Adsppm_Mem_Max - Adsppm_Mem_Lpass_LPM); index++)
            {
                gMemPwrMgrCtx.memPwrState[index][gMemPwrMgrCtx.activeStateIndex] = Adsppm_Mem_Power_Off;
            }

            // Loop through all clients link list
            pRMHandle = (RMHandleType *)coreUtils_Q_Check(pMemPwrReqQ);
            for(index = 0; index < pMemPwrReqQ->nCnt; index++)
            {
                if(NULL != pRMHandle)
                {
                    if(RM_Valid == pRMHandle->validFlag)
                    {
                        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_DEBUG,
                            "Client %u: request valid", pRMHandle->clientId);
                        if(NULL == ((AdsppmMemPowerReqParamType *)(pRMHandle->pRequestData)))
                        {
                            result = Adsppm_Status_BadParm;
                            ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "pRequestData=NULL");
                            break;
                        }
                        else
                        {
                            AdsppmMemPowerReqParamType *pReqData = (AdsppmMemPowerReqParamType *)pRMHandle->pRequestData;
                            if((Adsppm_Mem_None < pReqData->memory) && (Adsppm_Mem_Max > pReqData->memory))
                            {
                                switch(ACMPwr_GetMemPowerType(pReqData->memory))
                                {
                                case AsicRsc_Power_Mem:
                                    ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_INFO_EXT,
                                        "Mem %u, state %u", pReqData->memory, pReqData->powerState);
                                    if(pReqData->memory >= Adsppm_Mem_Lpass_LPM)
                                    {
                                        gMemPwrMgrCtx.memPwrState[pReqData->memory-Adsppm_Mem_Lpass_LPM][gMemPwrMgrCtx.activeStateIndex] =
                                            MAX(gMemPwrMgrCtx.memPwrState[pReqData->memory-Adsppm_Mem_Lpass_LPM][gMemPwrMgrCtx.activeStateIndex],
                                            pReqData->powerState);
                                    }
                                    else
                                    {
                                        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
                                            "Mem %u Shouldn't be here", pReqData->memory);
                                    }
                                    break;
                                case AsicRsc_Power_Ocmem:
                                    result = Adsppm_Status_UnSupported;
                                    break;
                                default:
                                    break;
                                }
                            }
                            else
                            {
                                result = Adsppm_Status_BadParm;
                                ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
                                    "Invalid Mem type %u", pReqData->memory);
                            }
                        }
                    }
                    pRMHandle = (RMHandleType *)coreUtils_Q_Next(pMemPwrReqQ, &pRMHandle->link);
                }
            }
            ADSPPM_UNLOCK_RESOURCE(Adsppm_Rsc_Id_MemPower);
            if(Adsppm_Status_Success == result)
            {
                // Process internal memories
                for(index = 0; index < Adsppm_Mem_Max - Adsppm_Mem_Lpass_LPM; index++)
                {
                    if(gMemPwrMgrCtx.memPwrState[index][gMemPwrMgrCtx.activeStateIndex] !=
                        gMemPwrMgrCtx.memPwrState[index][gMemPwrMgrCtx.activeStateIndex^1])
                    {
                        ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_INFO,
                            "Requesting power state %u for memory %u",
                            gMemPwrMgrCtx.memPwrState[index][gMemPwrMgrCtx.activeStateIndex],
                            index + Adsppm_Mem_Lpass_LPM);
                        if(Adsppm_Mem_Power_Off == gMemPwrMgrCtx.memPwrState[index][gMemPwrMgrCtx.activeStateIndex])
                        {
                            result = ClkRgm_MemoryControl(index + Adsppm_Mem_Lpass_LPM, Hal_ClkRgmMemCollapse);
                        }
                        else
                        {
                            result = ClkRgm_MemoryControl(index + Adsppm_Mem_Lpass_LPM, Hal_ClkRgmMemRetain);
                        }
                        if(Adsppm_Status_Success != result)
                        {
                            ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
                                "Error requesting power state for memory %u",
                                index + Adsppm_Mem_Lpass_LPM);
                            break;
                        }
                    }
                }

                // Flip active/previous request
                gMemPwrMgrCtx.activeStateIndex ^= 1;
            }
            adsppm_unlock(gMemPwrMgrCtx.lock);
        }
        else
        {
            result = Adsppm_Status_NotInitialized;
            ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "Not initialized");
        }
    }

    return result;
}

