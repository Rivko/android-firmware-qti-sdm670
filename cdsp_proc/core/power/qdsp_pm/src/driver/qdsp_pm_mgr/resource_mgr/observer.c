/*
* Copyright (c) 2015 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

#include "adsppm.h"
#include "adsppm_utils.h"
#include "observer.h"
#include "stdlib.h"


// Node for linked list of observer state info
typedef struct
{
    coreUtils_Q_LinkType link; // must be first field
    uint32 eventId;
    AdsppmEventHandler handler;
    void* handlerParam;
} AdsppmObserverListNodeType;

// Structure used for storing information needed to search through the list
// of observer nodes
typedef struct
{
    uint32 eventId;
    AdsppmEventHandler handler;
} AdsppmObserverCompareInfo;

Adsppm_Status observer_init(AdsppmObserverStateType* self)
{

    if(self == NULL)
        return Adsppm_Status_BadParm;

    memset(self, 0, sizeof(AdsppmObserverStateType));

    coreUtils_Q_Init(&self->observers);

    if(DAL_SUCCESS != DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE,
        &self->lock, NULL))
    {
        return Adsppm_Status_Failed;
    }

    return Adsppm_Status_Success;
}

static int matchObserverNode(AdsppmObserverListNodeType* node,
    AdsppmObserverCompareInfo* compareInfo)
{
    int match = 0;
    if(node && compareInfo && (node->eventId == compareInfo->eventId) &&
        (node->handler == compareInfo->handler))
    {
        match = 1;
    }
    return match;
}

// Assume lock is already obtained
static AdsppmObserverListNodeType* getNode(AdsppmObserverStateType* self,
    uint32 eventId, AdsppmEventHandler handler)
{
    AdsppmObserverCompareInfo compareInfo;
    compareInfo.eventId = eventId;
    compareInfo.handler = handler;
    return (AdsppmObserverListNodeType*) coreUtils_Q_LinearSearch(
        &self->observers, (coreUtils_Q_CompareFuncType) matchObserverNode,
        (void*) &compareInfo);
}

Adsppm_Status observer_add(AdsppmObserverStateType* self, uint32 eventId,
    AdsppmEventHandler handler, void* param)
{

    AdsppmObserverListNodeType* newNode;
    Adsppm_Status status = Adsppm_Status_Failed;

    adsppm_lock(self->lock);

    newNode = (AdsppmObserverListNodeType*)
        malloc(sizeof(AdsppmObserverListNodeType));
    if(newNode)
    {
        newNode->eventId = eventId;
        newNode->handler = handler;
        newNode->handlerParam = param;
        coreUtils_Q_Link(newNode, &newNode->link);
        coreUtils_Q_Put(&self->observers, &newNode->link);
        status = Adsppm_Status_Success;
    }

    adsppm_unlock(self->lock);

    return status;

}

Adsppm_Status observer_remove(AdsppmObserverStateType* self, uint32 eventId,
    AdsppmEventHandler handler)
{

    AdsppmObserverListNodeType* node;
    Adsppm_Status status = Adsppm_Status_Failed;

    adsppm_lock(self->lock);

    node = getNode(self, eventId, handler);
    if(node)
    {
        coreUtils_Q_Delete(&self->observers, &node->link);
        free(node);
        status = Adsppm_Status_Success;
    }

    adsppm_unlock(self->lock);

    return status;

}

Adsppm_Status observer_notify(AdsppmObserverStateType* self, uint32 eventId)
{

    AdsppmObserverListNodeType* node;
    Adsppm_Status status = Adsppm_Status_Success;

    adsppm_lock(self->lock);

    node = (AdsppmObserverListNodeType*) coreUtils_Q_Check(&self->observers);
    while(node)
    {
        if(node->handler && (node->eventId == eventId))
            node->handler(node->eventId, node->handlerParam);
        node = (AdsppmObserverListNodeType*)
            coreUtils_Q_Next(&self->observers, &node->link);
    }

    adsppm_unlock(self->lock);

    return status;

}


