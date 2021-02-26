/*
* Copyright (c) 2015 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
@file: clientmgr.c
@brief: ADSPPM client manager implementation.

$Header: //components/rel/core.qdsp6/2.1/power/qdsp_pm/src/driver/qdsp_pm_mgr/resource_mgr/clientmgr.c#2 $
*/

#include "adsppm.h"
#include "adsppm_utils.h"
#include "clientmgr.h"
#include "observer.h"
#include "coreUtils.h"
#include "stdlib.h"
#include "stddef.h"
#include "qurt_memory.h"

// Node for linked list of clients
typedef struct
{
    coreUtils_Q_LinkType link;
    AdsppmClientType client;
} AdsppmClientListNodeType;

// All state info for the Client Manager
typedef struct
{
    DALSYSSyncHandle lock;
    uint32 numClients;
    // Mask used to assign next clientId
    uint32 clientIdNumMask[((MAX_NUM_OF_ADSPPM_CLIENTS - 1) / 32) + 1];
    uint32 clientIdNum;                         // used to assign next clientId
    coreUtils_Q_Type clients;                   // list of registered clients
    MmpmClientClassType aggregateClientClass;
    AdsppmClientsDcvsParticipationType clientsDcvsParticipation;
    AdsppmObserverStateType observerState;
} AdsppmClientManagerType;

static AdsppmClientManagerType gAdsppmClientManager;

///////////////////////////////////////////////////////////////////////////////
// Static functions
///////////////////////////////////////////////////////////////////////////////

// Notify all registered observers of the specified event
static void cmNotify(AdsppmClientManagerEventId eventId)
{
    AdsppmClientManagerType* self = &gAdsppmClientManager;
    ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,
        "updating observers on event %d", eventId);
    observer_notify(&self->observerState, eventId);
    return;
}

static AdsppmClientListNodeType* getNode(const AdsppmClientType* client)
{
    AdsppmClientListNodeType* clientNode = NULL;
    if(client != NULL)
    {
        clientNode = (AdsppmClientListNodeType*)
            ((char*)client - offsetof(AdsppmClientListNodeType, client));
    }
    return clientNode;
}

static int matchClient(AdsppmClientListNodeType* clientNode, uint32* clientId)
{
    int match = 0;
    if(clientNode && (clientNode->client.clientId == *clientId))
    {
        match = 1;
    }
    return match;
}

// Assume lock is already obtained
static AdsppmClientType* getClient(uint32 clientId)
{
    AdsppmClientManagerType* self = &gAdsppmClientManager;
    AdsppmClientListNodeType* clientNode = NULL;

    clientNode = coreUtils_Q_LinearSearch(&self->clients,
        (coreUtils_Q_CompareFuncType)matchClient, (void*) &clientId);

    return clientNode ? (&clientNode->client) : NULL;
}

// Assume lock is already obtained
static const AdsppmClientType* getFirstClient()
{
    AdsppmClientManagerType* self = &gAdsppmClientManager;
    AdsppmClientType* firstClient = NULL;

    if(coreUtils_Q_Cnt(&self->clients) > 0)
    {
        AdsppmClientListNodeType* node =
            (AdsppmClientListNodeType*) coreUtils_Q_Check(&self->clients);
        if(node != NULL)
            firstClient = &node->client;
    }

    return firstClient;
}

// Assume lock is already obtained
static const AdsppmClientType* getNextClient(const AdsppmClientType* current)
{
    AdsppmClientManagerType* self = &gAdsppmClientManager;
    AdsppmClientListNodeType* currNode = NULL;
    AdsppmClientListNodeType* nextNode = NULL;
    AdsppmClientType* nextClient = NULL;

    currNode = getNode(current);
    if(currNode != NULL)
    {
        nextNode = coreUtils_Q_Next(&self->clients, &currNode->link);
        if(nextNode != NULL)
        {
            nextClient = &nextNode->client;
        }
    }

    return nextClient;
}

static void evaluateUseCaseActive(AdsppmClientType* client)
{
    if(client)
    {
        client->useCaseActiveFlag =
            client->bwActiveFlag || client->mipsActiveFlag;
    }
}

// Evaluate aggregate use case classes of clients which are active
static void evaluateOverallUseCaseActive(boolean *notifyFlag)
{
    AdsppmClientManagerType* self = &gAdsppmClientManager;
    const AdsppmClientType* client = NULL;
    MmpmClientClassType newAggregateClientClass = MMPM_UNKNOWN_CLIENT_CLASS;

    client = getFirstClient();
    while(client != NULL)
    {
        if(client->useCaseActiveFlag)
        {
            newAggregateClientClass |= client->clientClass;
        }
        client = getNextClient(client);
    }

    if(newAggregateClientClass != self->aggregateClientClass)
    {
        self->aggregateClientClass = newAggregateClientClass;
        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,
            "new aggregated client use case class: %d",
            self->aggregateClientClass);
        if(NULL != notifyFlag)
            *notifyFlag = TRUE;
    }

    return;
}

static void updateChangeInDcvsParticipation(
    uint32 newParamValue,
    uint32* pCurrentParamValue,
    boolean* notifyEvent)
{
    if(*pCurrentParamValue != newParamValue)
    {
        *pCurrentParamValue = newParamValue;
        *notifyEvent = TRUE;
    }
}

// Evaluate DCVS participation
// Assume lock is obtained
static boolean evaluateDcvsParticipation(void)
{
    AdsppmClientManagerType* self = &gAdsppmClientManager;
    const AdsppmClientType* client = NULL;
    AdsppmClientsDcvsParticipationType clientsDcvsParticipation;
    boolean notifyEvent = FALSE;

    if(ACM_GetFeatureStatus(AsicFeatureId_DcvsControl) ==
        AsicFeatureState_Enabled)
    {
        clientsDcvsParticipation.numOfAdspDdrBwActiveDcvsClients = 0;
        clientsDcvsParticipation.numOfMipsActiveDcvsClients = 0;
        //clientsDcvsParticipation.numOfCoreDdrBwActiveDcvsClients = 0;

        client = getFirstClient();
        while(client != NULL)
        {
            if(client->dcvsParticipation.enable)
            {
                if(client->adspDdrBwActiveFlag)
                    clientsDcvsParticipation.numOfAdspDdrBwActiveDcvsClients++;

                //if(client->coreDdrBwActiveFlag)
                //    clientsDcvsParticipation.numOfCoreDdrBwActiveDcvsClients++;

                if(client->mipsActiveFlag)
                    clientsDcvsParticipation.numOfMipsActiveDcvsClients++;
            }
            client = getNextClient(client);
        }

        updateChangeInDcvsParticipation(
            clientsDcvsParticipation.numOfAdspDdrBwActiveDcvsClients,
            &(self->clientsDcvsParticipation.numOfAdspDdrBwActiveDcvsClients),
            &notifyEvent);
        //updateChangeInDcvsParticipation(
        //    clientsDcvsParticipation.numOfCoreDdrBwActiveDcvsClients,
        //    &(self->clientsDcvsParticipation.numOfCoreDdrBwActiveDcvsClients),
        //    &notifyEvent);
        updateChangeInDcvsParticipation(
            clientsDcvsParticipation.numOfMipsActiveDcvsClients,
            &(self->clientsDcvsParticipation.numOfMipsActiveDcvsClients),
            &notifyEvent);

        if(notifyEvent)
        {
            ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_INFO,
                "change in clients' DCVS participation:"
                " #DdrBwDcvsClients=%u, #MipsDcvsClients=%u",
                clientsDcvsParticipation.numOfAdspDdrBwActiveDcvsClients,
                //clientsDcvsParticipation.numOfCoreDdrBwActiveDcvsClients,
                clientsDcvsParticipation.numOfMipsActiveDcvsClients);
        }
    }

    return notifyEvent;
}

static void updateClientIdMask(uint32 clientId, boolean active)
{
    AdsppmClientManagerType* self = &gAdsppmClientManager;
    uint32 majorIndex = (clientId - 1) / 32;
    uint32 minorIndex = (clientId - 1) % 32;
    if(majorIndex < ADSPPM_ARRAY_SIZE(self->clientIdNumMask))
    {
        if(active)
        {
            self->clientIdNumMask[majorIndex] =
                self->clientIdNumMask[majorIndex] | (1 << minorIndex);
        }
        else
        {
            self->clientIdNumMask[majorIndex] =
                self->clientIdNumMask[majorIndex] & (~(1 << minorIndex));
        }
    }
}

/**
 * @fn generateClientId
 * @brief - generate ADSPPM client ID
 * @param clientId - uint32 pointer to location of client ID
 * @return ADSPPM status
 */
static Adsppm_Status generateClientId(uint32 *clientId)
{
    AdsppmClientManagerType* self = &gAdsppmClientManager;
    Adsppm_Status sts = Adsppm_Status_Success;
    if(MAX_NUM_OF_ADSPPM_CLIENTS == self->clientIdNum)
    {
        uint32 index = 0;
        for(index = 0; index < MAX_NUM_OF_ADSPPM_CLIENTS; index++)
        {
            uint32 majorIndex = index / 32;
            uint32 minorIndex = index % 32;
            if((majorIndex < ADSPPM_ARRAY_SIZE(self->clientIdNumMask)) &&
                (0 == (self->clientIdNumMask[majorIndex] & (1 << minorIndex))))
            {
                *clientId = index + 1;
                break;
            }
        }
        if(MAX_NUM_OF_ADSPPM_CLIENTS == index)
        {
            sts = Adsppm_Status_Failed;
            ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_ERROR,
                "can't find available clientID max=%u, numofclients=%u",
                MAX_NUM_OF_ADSPPM_CLIENTS,
                self->numClients);
        }
    }
    else
    {
        self->clientIdNum++;
        *clientId = self->clientIdNum;
    }
    if(Adsppm_Status_Success == sts)
    {
        updateClientIdMask(*clientId, TRUE);
    }
    return sts;
}

///////////////////////////////////////////////////////////////////////////////
// Public functions
///////////////////////////////////////////////////////////////////////////////

Adsppm_Status CM_Init(void)
{
    ADSPPM_LOG_FUNC_PROFILESTART;
    AdsppmClientManagerType* self = &gAdsppmClientManager;

    memset(self, 0, sizeof(AdsppmClientManagerType));

    coreUtils_Q_Init(&self->clients);

    if(DAL_SUCCESS != DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE,
        &self->lock, NULL))
    {
        ADSPPM_LOG_FUNC_PROFILEEND;
        return Adsppm_Status_Failed;
    }

    self->aggregateClientClass = MMPM_UNKNOWN_CLIENT_CLASS;

    if(observer_init(&self->observerState) != Adsppm_Status_Success)
    {
        ADSPPM_LOG_FUNC_PROFILEEND;
        return Adsppm_Status_Failed;
    }

    ADSPPM_LOG_FUNC_PROFILEEND;
    return Adsppm_Status_Success;
}

const AdsppmClientType* CM_AddClient(AdsppmNewClientInfoType* clientInfo)
{
    ADSPPM_LOG_FUNC_PROFILESTART;
    AdsppmClientManagerType* self = &gAdsppmClientManager;
    AdsppmClientListNodeType* newNode;
    uint32 newClientId = 0;
    boolean notifyFlag = FALSE;
    Adsppm_Status status;

    adsppm_lock(self->lock);

    // Check max number of clients
    if(MAX_NUM_OF_ADSPPM_CLIENTS == self->numClients)
    {
        // No more room for clients
        ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_ERROR,
            "Registered clients exceed max=%u, clients=%u",
            MAX_NUM_OF_ADSPPM_CLIENTS, self->numClients);
        adsppm_unlock(self->lock);
        ADSPPM_LOG_FUNC_PROFILEEND;
        return NULL;
    }

    status = generateClientId(&newClientId);

    if(status != Adsppm_Status_Success)
    {
        adsppm_unlock(self->lock);
        ADSPPM_LOG_FUNC_PROFILEEND;
        return NULL;
    }

    newNode =
        (AdsppmClientListNodeType*) malloc(sizeof(AdsppmClientListNodeType));
    if(newNode == NULL)
    {
        adsppm_unlock(self->lock);
        ADSPPM_LOG_FUNC_PROFILEEND;
        return NULL;
    }

    memset(newNode, 0, sizeof(AdsppmClientListNodeType));

    // Copy new client info to newNode->client, and set client ID
    newNode->client.clientId = newClientId;
    newNode->client.coreId = clientInfo->coreId;
    newNode->client.instanceId = clientInfo->instanceId;
    if(clientInfo->clientName)
    {
        strlcpy(newNode->client.clientName, clientInfo->clientName,
            MAX_ADSPPM_CLIENT_NAME);
    }
    newNode->client.callBackFlag = clientInfo->callBackFlag;
    newNode->client.callBack = clientInfo->callBack;
    newNode->client.periodicityType = clientInfo->periodicityType;
    newNode->client.clientClass = clientInfo->clientClass;
    newNode->client.dcvsParticipation = clientInfo->dcvsParticipation;
    newNode->client.bwActiveFlag = FALSE;
    newNode->client.adspDdrBwActiveFlag = FALSE;
    //newNode->client.coreDdrBwActiveFlag = FALSE;
    newNode->client.mipsActiveFlag = FALSE;
    newNode->client.useCaseActiveFlag = FALSE;
    newNode->client.pSetParamConfig = NULL;

    coreUtils_Q_Link(newNode, &newNode->link); // init node
    coreUtils_Q_Put(&self->clients, &newNode->link); // add node to list

    ADSPPM_LOG_PRINTF_3(ADSPPM_LOG_LEVEL_INFO,
        "Client %s (type %u) registered with ID %u",
        newNode->client.clientName, newNode->client.periodicityType,
        newNode->client.clientId);

    // Update the counter for registered clients
    self->numClients += 1;

    // We could skip these under the assumption that new clients will never be
    // active at this point
    evaluateUseCaseActive(&newNode->client);
    evaluateOverallUseCaseActive(&notifyFlag);

    adsppm_unlock(self->lock);

    if(notifyFlag)
        cmNotify(CM_ACTIVE_CLASS_CHANGE_EVENT);

    ADSPPM_LOG_FUNC_PROFILEEND;
    return &newNode->client;
}

const AdsppmClientType* CM_GetClient(uint32 clientId)
{
    ADSPPM_LOG_FUNC_PROFILESTART;
    AdsppmClientManagerType* self = &gAdsppmClientManager;
    AdsppmClientType* client;

    adsppm_lock(self->lock);
    client = getClient(clientId);
    adsppm_unlock(self->lock);

    ADSPPM_LOG_FUNC_PROFILEEND;
    return client;
}

Adsppm_Status CM_RemoveClient(uint32 clientId)
{
    ADSPPM_LOG_FUNC_PROFILESTART;
    AdsppmClientManagerType* self = &gAdsppmClientManager;
    AdsppmClientType* client;
    AdsppmClientListNodeType* clientNode;

    adsppm_lock(self->lock);

    client = getClient(clientId);
    clientNode = getNode(client);

    if(clientNode == NULL)
    {
        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
            "cannot remove - invalid client ID %d", clientId);
        adsppm_unlock(self->lock);
        ADSPPM_LOG_FUNC_PROFILEEND;
        return Adsppm_Status_Failed;
    }

    ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO, "removing client ID %d",
        clientId);

    coreUtils_Q_Delete(&self->clients, &clientNode->link);

    if(clientNode->client.pSetParamConfig != NULL)
    {
        free(clientNode->client.pSetParamConfig);
    }

    free((void*) clientNode);

    // Update mask used to allocate client IDs
    updateClientIdMask(clientId, FALSE);
    // Update the counter for registered clients
    self->numClients -= 1;

    adsppm_unlock(self->lock);

    ADSPPM_LOG_FUNC_PROFILEEND;
    return Adsppm_Status_Success;
}

Adsppm_Status CM_SetParameter(uint32 clientId,
    AdsppmParameterConfigType* pParamConfigData)
{
    AdsppmClientManagerType* self = &gAdsppmClientManager;
    Adsppm_Status sts = Adsppm_Status_Success;
    AdsppmClientType* client = NULL;
    ADSPPM_LOG_FUNC_PROFILESTART;

    adsppm_lock(self->lock);

    if(pParamConfigData == NULL)
    {
        adsppm_unlock(self->lock);
        ADSPPM_LOG_FUNC_PROFILEEND;
        return Adsppm_Status_BadParm;
    }

    client = getClient(clientId);
    if(client == NULL)
    {
        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO_EXT,
            "clientId=%u is invalid", clientId);
        adsppm_unlock(self->lock);
        ADSPPM_LOG_FUNC_PROFILEEND;
        return Adsppm_Status_Failed;
    }

    switch(pParamConfigData->paramId)
    {
        case Adsppm_Param_Id_Client_Ocmem_Map:
        case Adsppm_Param_Id_Resource_Limit:
        default:
            sts = Adsppm_Status_UnSupported;
            ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO_EXT,
                "Parameter %u not supported", pParamConfigData->paramId);
            break;
    }

    adsppm_unlock(self->lock);

    ADSPPM_LOG_FUNC_PROFILEEND;
    return sts;
}

Adsppm_Status CM_SetProperty(uint32 clientId, AdsppmClientPropertyId prop,
    uint32 value)
{
    ADSPPM_LOG_FUNC_PROFILESTART;
    AdsppmClientManagerType* self = &gAdsppmClientManager;
    AdsppmClientType* client;
    boolean changeInDcvsParticipation = FALSE;
    boolean changeInUseCaseActivity   = FALSE;

    adsppm_lock(self->lock);

    client = getClient(clientId);
    if(client == NULL)
    {
        adsppm_unlock(self->lock);
        ADSPPM_LOG_FUNC_PROFILEEND;
        return Adsppm_Status_Failed;
    }

    switch(prop)
    {
        case CM_BW_REQ_ACTIVE_PROPERTY:
        {
            boolean newValue = (boolean) value;
            if(client->bwActiveFlag != newValue)
            {
                client->bwActiveFlag = newValue;
                evaluateUseCaseActive(client);
                evaluateOverallUseCaseActive(&changeInUseCaseActivity);
            }
            break;
        }
        case CM_ADSP_DDR_BW_REQ_ACTIVE_PROPERTY:
        {
            boolean newValue = (boolean) value;
            if(client->adspDdrBwActiveFlag != newValue)
            {
                client->adspDdrBwActiveFlag = newValue;
                changeInDcvsParticipation =
                    evaluateDcvsParticipation();
            }
            break;
        }
        /*
        case CM_CORE_DDR_BW_REQ_ACTIVE_PROPERTY:
        {
            boolean newValue = (boolean) value;
            if(client->coreDdrBwActiveFlag != newValue)
            {
                client->coreDdrBwActiveFlag = newValue;
                changeInDcvsParticipation =
                    evaluateDcvsParticipation();
            }
            break;
        }
        */
        case CM_MIPS_REQ_ACTIVE_PROPERTY:
        {
            boolean newValue = (boolean) value;
            if(client->mipsActiveFlag != newValue)
            {
                client->mipsActiveFlag = newValue;
                evaluateUseCaseActive(client);
                evaluateOverallUseCaseActive(&changeInUseCaseActivity);
                changeInDcvsParticipation =
                    evaluateDcvsParticipation();
            }
            break;
        }
        case CM_CLASS_PROPERTY:
        {
            MmpmClientClassType newValue = (MmpmClientClassType) value;
            if(client->clientClass != newValue)
            {
                client->clientClass = newValue;
                evaluateOverallUseCaseActive(&changeInUseCaseActivity);
            }
            break;
        }
        case CM_CACHE_LINE_LOCK_PROPERTY:
        {
            MmpmL2CacheLineLockParameterType* lockParam =
                (MmpmL2CacheLineLockParameterType*) value;
            client->cacheLockParam = *lockParam; // copy locking params
            break;
        }
        case CM_DCVS_PARTICIPATION_PROPERTY:
        {
            MmpmDcvsParticipationType* dcvsParticipation =
                (MmpmDcvsParticipationType*) value;
            // Copy DCVS participation
            client->dcvsParticipation = *dcvsParticipation;
            changeInDcvsParticipation = evaluateDcvsParticipation();
            break;
        }
        case CM_STREAM_ID_PROPERTY:
        {
            uint32 newValue = (uint32) value;
            if(client->streamIdMask != newValue)
            {
                client->streamIdMask = newValue;
            }
            break;
        }

        default:
            break;

    }

    adsppm_unlock(self->lock);

    if(changeInUseCaseActivity)
        cmNotify(CM_ACTIVE_CLASS_CHANGE_EVENT);

    if(changeInDcvsParticipation)
        cmNotify(CM_DCVS_PARTICIPATION_CHANGE_EVENT);

    ADSPPM_LOG_FUNC_PROFILEEND;
    return Adsppm_Status_Success;
}

uint32 CM_GetActiveUseCaseClasses()
{
    ADSPPM_LOG_FUNC_PROFILESTART;
    AdsppmClientManagerType* self = &gAdsppmClientManager;
    uint32 result;

    adsppm_lock(self->lock);
    result = self->aggregateClientClass;
    adsppm_unlock(self->lock);

    ADSPPM_LOG_FUNC_PROFILEEND;
    return result;
}

AdsppmClientsDcvsParticipationType CM_GetClientsDcvsParticipation()
{
    ADSPPM_LOG_FUNC_PROFILESTART;
    AdsppmClientManagerType* self = &gAdsppmClientManager;
    AdsppmClientsDcvsParticipationType result;

    adsppm_lock(self->lock);
    result = self->clientsDcvsParticipation;
    adsppm_unlock(self->lock);
    ADSPPM_LOG_FUNC_PROFILEEND;

    return result;
}

Adsppm_Status CM_RegisterEvent(AdsppmClientManagerEventId event,
    AdsppmClientEventHandler handler, void* param)
{
    ADSPPM_LOG_FUNC_PROFILESTART;
    AdsppmClientManagerType* self = &gAdsppmClientManager;
    
    // Assuming AdsppmClientEventHandler can be cast to AdsppmEventHandler
    Adsppm_Status ret_val = observer_add(&self->observerState, event,
        (AdsppmEventHandler) handler, param);
    ADSPPM_LOG_FUNC_PROFILEEND;
    return ret_val;
}

Adsppm_Status CM_UnregisterEvent(AdsppmClientManagerEventId event,
    AdsppmClientEventHandler handler)
{
    ADSPPM_LOG_FUNC_PROFILESTART;
    AdsppmClientManagerType* self = &gAdsppmClientManager;
    // Assuming AdsppmClientEventHandler can be cast to AdsppmEventHandler
    Adsppm_Status ret_val = observer_remove(&self->observerState, event,
        (AdsppmEventHandler) handler);
    ADSPPM_LOG_FUNC_PROFILEEND;
    return ret_val;
}

