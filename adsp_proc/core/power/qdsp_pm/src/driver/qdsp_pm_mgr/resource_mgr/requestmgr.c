/*
* Copyright (c) 2015 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
@file: requestmgr.c
@brief: ADSPPM request manager implementation.

$Header: //components/rel/core.qdsp6/2.1/power/qdsp_pm/src/driver/qdsp_pm_mgr/resource_mgr/requestmgr.c#2 $
*/

#include "adsppm.h"
#include "adsppm_utils.h"
#include "core.h"
#include "core_internal.h"
#include "coreUtils.h"
#include "requestmgr.h"
#include "clkmgr.h"
#include "mipsmgr.h"
#include "busmgr.h"
#include "pwrmgr.h"
#include "latencymgr.h"
#include "thermalmgr.h"
#include "stdlib.h"
#include "mempwrmgr.h"
#include "clientmgr.h"


/**
 * @fn matchRequestState - check if the RMHandleType matches the given clientId
 */
static int matchRequestState(RMHandleType *pRMHandle, uint32 *clientId)
{
    int match = 0;
    if(pRMHandle->clientId == *clientId)
    {
        match = 1;
    }
    return match;
}

static RMHandleType* getRequestState(uint32 clientId, AdsppmRscIdType resourceId)
{
    if((resourceId <= Adsppm_Rsc_Id_None) ||
        (resourceId >= Adsppm_Rsc_Id_Max))
    {
        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
            "Invalid resourceId (%u)", resourceId);
        return NULL;
    }

    AdsppmCoreCtxType *pAdsppmCoreCtx = GetAdsppmCoreContext();
    return coreUtils_Q_LinearSearch(
        &pAdsppmCoreCtx->rscReqQ[resourceId - 1],
        (coreUtils_Q_CompareFuncType) matchRequestState,
        (void *) &clientId);
}

/**
 * @fn getRequestDataSize - return size of request data structure per resource
 */
static uint32 getRequestDataSize(AdsppmRscIdType resourceId)
{
    uint32 result = 0;
    switch(resourceId)
    {
    case Adsppm_Rsc_Id_Power:
        result = sizeof(uint32);
        break;
    case Adsppm_Rsc_Id_Core_Clk:
        result = sizeof(AdsppmClkRequestType);
        break;
    case Adsppm_Rsc_Id_BW:
        result = sizeof(AdsppmBwReqType);
        break;
    case Adsppm_Rsc_Id_Mips:
        result = sizeof(AdsppmQ6ClockRequestInfoType);
        break;
    case Adsppm_Rsc_Id_Sleep_Latency:
        result = sizeof(uint32);
        break;
    case Adsppm_Rsc_Id_Thermal:
        result = sizeof(uint32);
        break;
    case Adsppm_Rsc_Id_MemPower:
        result = sizeof(AdsppmMemPowerReqParamType);
        break;
    case Adsppm_Rsc_Id_Core_Clk_Domain:
        result = sizeof(AdsppmClkDomainReqType);
        break;
    default :
        break;
    }
    return result;
}


/**
 * @fn getRequestData_ExtSize - return size of extended request data structure per resource
 */
static uint32 getRequestData_ExtSize(AdsppmRscIdType resourceId)
{
    uint32 result = 0;
    switch(resourceId)
    {
    case Adsppm_Rsc_Id_Power:
        break;
    case Adsppm_Rsc_Id_Core_Clk:
        break;
    case Adsppm_Rsc_Id_BW:
        result = sizeof(AdsppmBusBWRequestValueType);
        break;
    case Adsppm_Rsc_Id_Mips:
        break;
    case Adsppm_Rsc_Id_Sleep_Latency:
        break;
    case Adsppm_Rsc_Id_Thermal:
        break;
    case Adsppm_Rsc_Id_MemPower:
        break;
    case Adsppm_Rsc_Id_Core_Clk_Domain:
        break;
    default :
        break;
    }
    return result;
}


static void updateRequestLifecycleState(AdsppmRscIdType resourceId)
{

    if((resourceId <= Adsppm_Rsc_Id_None) ||
        (resourceId >= Adsppm_Rsc_Id_Max))
    {
        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
            "Invalid resourceId (%u)", resourceId);
        return;
    }

    AdsppmCoreCtxType *pAdsppmCoreCtx = GetAdsppmCoreContext();
    coreUtils_Q_Type* pRequestQueue = &pAdsppmCoreCtx->rscReqQ[resourceId - 1];
    RMHandleType* pRequest = NULL;
    boolean isValid = FALSE;

    pRequest = (RMHandleType*) coreUtils_Q_Check(pRequestQueue);
    while(pRequest)
    {
        isValid = (pRequest->validFlag == RM_Valid) ||
            (pRequest->validFlag_Ext == RM_Valid);
        if(pRequest->lifecycleState == NEW_REQUEST)
        {
            pRequest->lifecycleState =
                isValid ? ACCEPTED_REQUEST : UNDEFINED_REQUEST;
        }
        else if(pRequest->lifecycleState == RELEASED_REQUEST)
        {
            // A released request may not be fully released if validFlag or
            // validFlag_Ext are still high, in which case we move the state
            // back to 'accepted'.  A fully released request goes to
            // 'undefined'.
            pRequest->lifecycleState =
                isValid ? ACCEPTED_REQUEST : UNDEFINED_REQUEST;
        }
        pRequest = (RMHandleType*) coreUtils_Q_Next(pRequestQueue,
            &pRequest->link);
    }

    return;

}

/**
 * @fn RM_Init
 * @Brief - init mutex for RM
 * @param input AdsppmCoreIdType coreId - core Id in LPASS
 * @param input uint32 *callback - call back function for this client.
 * @return uint32 clientId
 */
Adsppm_Status RM_Init(void)
{
    ADSPPM_LOG_FUNC_PROFILESTART;
    Adsppm_Status sts = Adsppm_Status_Success;
    AdsppmCoreCtxType *pAdsppmCoreCtx = GetAdsppmCoreContext();
    AdsppmRscIdType index;
    for(index = Adsppm_Rsc_Id_Start; index < Adsppm_Rsc_Id_Max; index++)
    {
        coreUtils_Q_Init(&pAdsppmCoreCtx->rscReqQ[index-1]);
        if(DAL_SUCCESS != DALSYS_SyncCreate(
            DALSYS_SYNC_ATTR_RESOURCE,
            (DALSYSSyncHandle *)&pAdsppmCoreCtx->resourceLock[index-1],
            NULL))
        {
            sts = Adsppm_Status_Failed;
            break;
        }
    }
    ADSPPM_LOG_FUNC_PROFILEEND;
    return sts;
}


Adsppm_Status RM_SetupClient(const AdsppmClientType* client)
{

    ADSPPM_LOG_FUNC_PROFILESTART;
    Adsppm_Status status = Adsppm_Status_Success;
    AdsppmCoreCtxType *pAdsppmCoreCtx = GetAdsppmCoreContext();
    AdsppmRscIdType resourceId = Adsppm_Rsc_Id_None;
    RMHandleType *pRMHandle = NULL;

    // Allocate RMCtx for each resource and put into resource RMCtx Queue
    for(resourceId = Adsppm_Rsc_Id_Start; resourceId < Adsppm_Rsc_Id_Max; resourceId++)
    {
        if(NULL == pAdsppmCoreCtx->resourceLock[resourceId-1])
        {
            status = Adsppm_Status_NotInitialized;
            ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
                "Lock for resourceId%u is NULL", resourceId);
        }
        else
        {
            ADSPPM_LOCK_RESOURCE(resourceId);
            pRMHandle = malloc(sizeof(RMHandleType));
            if(NULL == pRMHandle)
            {
                status = Adsppm_Status_Failed;
                ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,
                    "Malloc failed for pRMHandle");
                ADSPPM_UNLOCK_RESOURCE(resourceId);
                break;
            }
            else
            {
                memset(pRMHandle, 0, sizeof(RMHandleType));
                pRMHandle->pRequestData = malloc(getRequestDataSize(resourceId));
                if(NULL == pRMHandle->pRequestData)
                {
                    status = Adsppm_Status_Failed;
                    ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,
                        "Malloc failed for pRMHandle->pRequestData");
                    free(pRMHandle);
                    ADSPPM_UNLOCK_RESOURCE(resourceId);
                    break;
                }
                else
                {
                    memset(pRMHandle->pRequestData, 0, getRequestDataSize(resourceId));
                    if(getRequestData_ExtSize(resourceId))
                    {
                        pRMHandle->pRequestData_Ext = malloc(getRequestData_ExtSize(resourceId));
                        if(NULL == pRMHandle->pRequestData_Ext)
                        {
                             status = Adsppm_Status_Failed;
                             ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,
                                "Malloc failed for pRMHandle->pRequestData");
                             free(pRMHandle->pRequestData);
                             free(pRMHandle);
                             ADSPPM_UNLOCK_RESOURCE(resourceId);
                             break;
                        }
                        else
                        {
                            memset(pRMHandle->pRequestData_Ext, 0, getRequestData_ExtSize(resourceId));
                        }
                    }
                    pRMHandle->clientId = client->clientId;
                    pRMHandle->resourceId = resourceId;
                    pRMHandle->pClient = client;
                    pRMHandle->lifecycleState = UNDEFINED_REQUEST;

                    coreUtils_Q_Link(pRMHandle, &pRMHandle->link);
                    coreUtils_Q_Put(&(pAdsppmCoreCtx->rscReqQ[resourceId-1]), &pRMHandle->link);
                }
            }
            ADSPPM_UNLOCK_RESOURCE(resourceId);
        }
    }
    ADSPPM_LOG_FUNC_PROFILEEND;
    return status;

}

/**
 * @fn RM_GetResourceInfo
 * @Brief -  based on clientid, retrieve information about coreID and callback function.
 * @param input uint32 clientId - client ID.
 * @param output AdsppmCoreIdType *pCoreId - core ID that client registered.
 * @param output uint32 *callback - client call back function pointer.
 * @return Adsppm_Status_BadParm
 * @return Adsppm_Status_Success
 */
Adsppm_Status RM_GetResourceInfo(AdsppmRscIdType rsc_Id, void *pInfo, AdsppmInfoType infoType)
{
    ADSPPM_LOG_FUNC_PROFILESTART;
    Adsppm_Status sts = Adsppm_Status_Success;
    AsicFeatureDescType *pDesc = NULL;
    if(NULL == pInfo)
    {
        sts = Adsppm_Status_NullPointer;
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "NULL pointer for pInfo");
    }
    else
    {
        switch(rsc_Id)
        {
        case Adsppm_Rsc_Id_Core_Clk:
            if(Adsppm_Info_Type_Current_Value == infoType)
            {
                sts = CLK_GetInfo((AdsppmInfoClkFreqType *) pInfo);
            }
            else if(Adsppm_Info_Type_Max_Value == infoType)
            {
                pDesc = ACM_GetFeatureDescriptor(AsicFeatureId_CoreClk_Scaling);
                ((AdsppmInfoClkFreqType *) pInfo)->clkFreqHz = pDesc->max;
            }
            else
            {
                sts = Adsppm_Status_UnSupported;
            }
            break;

        case Adsppm_Rsc_Id_Mips:
            if(Adsppm_Info_Type_Current_Value == infoType)
            {
                ((MmpmInfoMppsType *) pInfo)->aggregateMpps =
                    MIPS_GetMppsAggregationInfo(
                        ((MmpmInfoMppsType *) pInfo)->clientClasses);
            }
            else if(Adsppm_Info_Type_Max_Value == infoType)
            {
                pDesc = ACM_GetFeatureDescriptor(AsicFeatureId_Adsp_Clock_Scaling);
                *(uint32 *)pInfo = pDesc->max;
            }
            else
            {
                sts = Adsppm_Status_UnSupported;
            }
            break;

        case Adsppm_Rsc_Id_BW:
            if(Adsppm_Info_Type_Max_Value == infoType)
            {
                uint64 maxBwVote=0,maxQ6Vote=0;
                
                pDesc = ACM_GetFeatureDescriptor(AsicFeatureId_Ahb_Scaling);
                if(pDesc->state ==  AsicFeatureState_Enabled)
                    maxBwVote = pDesc->max;
                pDesc = ACM_GetFeatureDescriptor(AsicFeatureId_AhbIX_Q6Scaling);
                if(pDesc->state ==  AsicFeatureState_Enabled)
                    maxQ6Vote = pDesc->max;
                 
                *(uint64 *)pInfo = MAX(maxBwVote,maxQ6Vote);
            }
            else if(Adsppm_Info_Type_Current_Value == infoType)
            {
                sts = BusMgr_GetAggregatedBwInfo((AdsppmInfoAggregatedBwType *)pInfo);
            }
            else
            {
                sts = Adsppm_Status_UnSupported;
            }
            break;

        default:
            sts = Adsppm_Status_UnSupported;
            break;
        }
    }
    ADSPPM_LOG_FUNC_PROFILEEND;
    return sts;
}


Adsppm_Status RM_TeardownClient(uint32 clientId)
{

    Adsppm_Status sts = Adsppm_Status_Success;
    AdsppmRscIdType rscId;
    RMHandleType *pRMHandle = NULL;
    ADSPPM_LOG_FUNC_PROFILESTART;

    ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO, "tearing down clientId=%u",
        clientId);

    if(clientId == 0)
    {
        sts = Adsppm_Status_BadParm;
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "Invalid ClientId 0");
    }
    else
    {
        // Release outstanding requests and
        // remove resource request handles from queues
        for(rscId = Adsppm_Rsc_Id_Start; rscId < Adsppm_Rsc_Id_Max; rscId++)
        {
            ADSPPM_LOCK_RESOURCE(rscId);

            pRMHandle = getRequestState(clientId, rscId);
            if(NULL != pRMHandle)
            {
                if(pRMHandle->validFlag == RM_Valid ||
                    pRMHandle->validFlag_Ext == RM_Valid)
                {
                    pRMHandle->validFlag = RM_InValid;
                    pRMHandle->validFlag_Ext = RM_InValid;
                    pRMHandle->lifecycleState = RELEASED_REQUEST;
                    RM_RequestResource(rscId);
                }

                if(NULL != pRMHandle->pRequestData)
                {
                    free(pRMHandle->pRequestData);
                    pRMHandle->pRequestData = NULL;
                }

                if(NULL != pRMHandle->pRequestData_Ext)
                {
                    free(pRMHandle->pRequestData_Ext);
                    pRMHandle->pRequestData_Ext = NULL;
                }

                coreUtils_Q_Delete(
                    &(GetAdsppmCoreContext()->rscReqQ[rscId-1]),
                    &pRMHandle->link);
                free(pRMHandle);
            }

            ADSPPM_UNLOCK_RESOURCE(rscId);
        }
    }

    ADSPPM_LOG_FUNC_PROFILEEND;
    return sts;

}


/**
 * @fn RM_AddRequest
 * @Brief - fill in the client’s request data for the specified resource and mark the request as valid.
 * @param input uint32 clientId - client ID.
 * @param input AdsppmRscIdType resourceId - resource ID that client request.
 * @param input void* pAdsppmReqData - pointer for request data, it'll cast to different data structure based on resource ID.
 * @param input uint32 extData - if non-zero extended data will be updated instead of request data.
 * @param input boolean syncRequest - TRUE if the client has requested synchronous response, FALSE if asynchronous
 * @return Adsppm_Status_BadParm
 * @return Adsppm_Status_NotInitialized
 * @return Adsppm_Status_Success
 */
Adsppm_Status RM_AddRequest(
    uint32 clientId, AdsppmRscIdType resourceId, void *pAdsppmReqData,
    uint32 extData, boolean syncRequest)
{
    ADSPPM_LOG_FUNC_PROFILESTART;
    Adsppm_Status sts = Adsppm_Status_Success;
    AdsppmCoreCtxType *pAdsppmCoreCtx = GetAdsppmCoreContext();
    RMHandleType *pRMHandle = NULL;

    if((0 == clientId) ||
        (Adsppm_Rsc_Id_None == resourceId) ||
        (Adsppm_Rsc_Id_Max <= resourceId) ||
        (NULL == pAdsppmReqData))
    {
        sts = Adsppm_Status_BadParm;
        ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_ERROR,
            "Invalid clientId or resourceId,clientId=%u, resourceId=%u",
            clientId, resourceId);
    }
    else
    {
        if(NULL == pAdsppmCoreCtx->resourceLock[resourceId-1])
        {
            sts = Adsppm_Status_NotInitialized;
            ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
                "Lock for resourceId%u is NULL", resourceId);
        }
        else
        {
            ADSPPM_LOCK_RESOURCE(resourceId);
            pRMHandle = getRequestState(clientId, resourceId);
            if(NULL == pRMHandle)
            {
                sts = Adsppm_Status_BadParm;
                ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_ERROR,
                    "can't find RMHandle: clientId=%u, resourceId=%u",
                    clientId, resourceId);
            }
            else
            {
                if(NULL == pRMHandle->pClient)
                {
                    sts = Adsppm_Status_BadParm;
                    ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_ERROR,
                        "pClient is NULL: clientId=%u, resourceId=%u",
                        clientId, resourceId);
                }
                else
                {
                    ADSPPM_LOG_PRINTF_3(ADSPPM_LOG_LEVEL_INFO,
                        "Adding request: clientId=%u (%s), resourceId=%u",
                        clientId, pRMHandle->pClient->clientName, resourceId);
                    if((pRMHandle->clientId == clientId) && (pRMHandle->resourceId == resourceId))
                    {
                        // Need allocate item for first time new resource request from client ID
                        if(0 == extData)
                        {
                            if(NULL != pRMHandle->pRequestData)
                            {
                                memscpy(
                                    pRMHandle->pRequestData,
                                    getRequestDataSize(resourceId),
                                    pAdsppmReqData,
                                    getRequestDataSize(resourceId));
                                pRMHandle->validFlag = RM_Valid;
                                ADSPPM_LOG_PRINTF_3(ADSPPM_LOG_LEVEL_INFO_EXT,
                                    "Set request valid for clientId=%u (%s), resourceId=%u",
                                    clientId, pRMHandle->pClient->clientName, resourceId);
                            }
                        }
                        else
                        {
                            if(NULL != pRMHandle->pRequestData_Ext)
                            {
                                memscpy(
                                    pRMHandle->pRequestData_Ext,
                                    getRequestData_ExtSize(resourceId),
                                    pAdsppmReqData,
                                    getRequestData_ExtSize(resourceId));
                                pRMHandle->validFlag_Ext = RM_Valid;
                                ADSPPM_LOG_PRINTF_3(ADSPPM_LOG_LEVEL_INFO_EXT,
                                    "Set ext request valid for clientId=%u (%s), resourceId=%u",
                                    clientId, pRMHandle->pClient->clientName, resourceId);
                            }
                        }
                        pRMHandle->lifecycleState = NEW_REQUEST;
                        pRMHandle->synchronousRequest = syncRequest;
                    }
                    else
                    {
                        sts = Adsppm_Status_BadParm;
                        ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_ERROR,
                            "release request with invalid clientId=%u, resourceid=%u",
                            clientId, resourceId);
                    }
                }
            }
            ADSPPM_UNLOCK_RESOURCE(resourceId);
        }
    }
    ADSPPM_LOG_FUNC_PROFILEEND;
    return sts;
}


/**
 * @fn RM_ReleaseRequest
 * @Brief - mark request for the client/resource pair as invalid.
 * @param input uint32 clientId - client ID.
 * @param input AdsppmRscIdType resourceId - resource ID that client request.
 * @param input uint32 extData - if non-zero extended data will be updated instead of request data.
 * @return Adsppm_Status_BadParm
 * @return Adsppm_Status_Success
 */
Adsppm_Status RM_ReleaseRequest(uint32 clientId, AdsppmRscIdType resourceId, uint32 extData)
{
    ADSPPM_LOG_FUNC_PROFILESTART;
    Adsppm_Status sts = Adsppm_Status_Success;
    AdsppmCoreCtxType *pAdsppmCoreCtx = GetAdsppmCoreContext();
    RMHandleType *pRMHandle = NULL;

    if((0 == clientId) || (0 == resourceId) || (Adsppm_Rsc_Id_Max-1 < resourceId))
    {
        sts = Adsppm_Status_BadParm;
        ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_ERROR,
            "Invalid clientId or resourceId: clientId=%u, resourceId=%u",
            clientId, resourceId);
    }
    else
    {
        if(NULL == pAdsppmCoreCtx->resourceLock[resourceId-1])
        {
            sts = Adsppm_Status_NotInitialized;
            ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
                "Lock for resourceId%u is NULL", resourceId);
        }
        else
        {
            ADSPPM_LOCK_RESOURCE(resourceId);
            pRMHandle = getRequestState(clientId, resourceId);
            if(NULL == pRMHandle)
            {
                sts = Adsppm_Status_BadParm;
                ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_ERROR,
                    "can't find RMCtx: clientId=%u, resourceId=%u",
                    clientId, resourceId);
            }
            else
            {
                if(NULL == pRMHandle->pClient)
                {
                    sts = Adsppm_Status_BadParm;
                    ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_ERROR,
                        "pClient is NULL: clientId=%u, resourceId=%u",
                        clientId, resourceId);
                }
                else
                {
                    ADSPPM_LOG_PRINTF_3(ADSPPM_LOG_LEVEL_INFO,
                        "Releasing request: clientId=%u (%s), resourceId=%u",
                        clientId, pRMHandle->pClient->clientName, resourceId);
                    if((pRMHandle->clientId == clientId) && (pRMHandle->resourceId == resourceId))
                    {
                        if(0 == extData)
                        {
                            pRMHandle->validFlag = RM_InValid;
                            ADSPPM_LOG_PRINTF_3(ADSPPM_LOG_LEVEL_INFO_EXT,
                                "Set request to invalid: clientId=%u (%s), resourceId=%u",
                                clientId, pRMHandle->pClient->clientName, resourceId);
                        }
                        else
                        {
                            pRMHandle->validFlag_Ext = RM_InValid;
                            ADSPPM_LOG_PRINTF_3(ADSPPM_LOG_LEVEL_INFO_EXT,
                                "Set ext request to invalid: clientId=%u (%s), resourceId=%u",
                                clientId, pRMHandle->pClient->clientName, resourceId);
                        }
                        pRMHandle->lifecycleState = RELEASED_REQUEST;
                    }
                    else
                    {
                        sts = Adsppm_Status_BadParm;
                        ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_INFO_EXT,
                            "release request with invalid clientId=%u, resourceid=%u",
                            clientId, resourceId);
                    }
                }
            }
            ADSPPM_UNLOCK_RESOURCE(resourceId);
        }
    }

    ADSPPM_LOG_FUNC_PROFILEEND;
    return sts;
}


/**
 * @fn RM_RequestResource
 * @Brief - send request to resource managers, based on resource ID.
 * @param input AdsppmRscIdType resourceId - resource ID that client request.
 * @return Adsppm_Status_BadParm
 * @return Adsppm_Status_Success
 */
Adsppm_Status RM_RequestResource(AdsppmRscIdType resourceId)
{
    ADSPPM_LOG_FUNC_PROFILESTART;
    Adsppm_Status sts = Adsppm_Status_Success;
    AdsppmCoreCtxType *pAdsppmCoreCtx = GetAdsppmCoreContext();

    ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO_EXT,
        "Request resourceId=%u", resourceId);
    if((Adsppm_Rsc_Id_None == resourceId) || (Adsppm_Rsc_Id_Max <= resourceId))
    {
        sts = Adsppm_Status_BadParm;
        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
            "Invalid resourceId=%u", resourceId);
    }
    else
    {
        ADSPPM_LOCK_RESOURCE(resourceId);

        switch(resourceId)
        {
        case Adsppm_Rsc_Id_Core_Clk:
            sts = CLK_ProcessRequest(
                &(pAdsppmCoreCtx->rscReqQ[Adsppm_Rsc_Id_Core_Clk-1]));
            break;

        case Adsppm_Rsc_Id_Power:
            sts = PWR_ProcessRequest(
                &(pAdsppmCoreCtx->rscReqQ[Adsppm_Rsc_Id_Power-1]));
            break;

        case Adsppm_Rsc_Id_Sleep_Latency:
            sts = SLEEP_ProcessRequest(
                &(pAdsppmCoreCtx->rscReqQ[Adsppm_Rsc_Id_Sleep_Latency-1]));
            break;

        case Adsppm_Rsc_Id_Mips:
            sts = MIPS_ProcessRequest(
                &(pAdsppmCoreCtx->rscReqQ[Adsppm_Rsc_Id_Mips-1]));
            break;

        case Adsppm_Rsc_Id_BW:
            sts = BusMgr_ProcessRequest(
                &(pAdsppmCoreCtx->rscReqQ[Adsppm_Rsc_Id_BW-1]));
            break;

        case Adsppm_Rsc_Id_Thermal:
            sts = THERMAL_ProcessRequest(
                &(pAdsppmCoreCtx->rscReqQ[Adsppm_Rsc_Id_Thermal-1]));
            break;

        case Adsppm_Rsc_Id_MemPower:
            sts = MEMPWR_ProcessRequest(
                &(pAdsppmCoreCtx->rscReqQ[Adsppm_Rsc_Id_MemPower-1]));
            break;

        case Adsppm_Rsc_Id_Core_Clk_Domain:
            sts = CLKDomain_ProcessRequest(
                &(pAdsppmCoreCtx->rscReqQ[Adsppm_Rsc_Id_Core_Clk_Domain-1]));
            break;

        default:
            sts = Adsppm_Status_UnSupported;
            ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
                "Unsupported resourceId=%u", resourceId);
            break;
        } // Switch resource Id

        updateRequestLifecycleState(resourceId);

        ADSPPM_UNLOCK_RESOURCE(resourceId);
    } // ResoureId out of range?

    ADSPPM_LOG_FUNC_PROFILEEND;
    return sts;
}


