/*
* Copyright (c) 2015 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

// Client ID list
// Maintains a dynamic and thread-safe list of client IDs.

#include <pthread.h>
#include "adsppm_types.h"

typedef struct ClientIdList ClientIdList;
struct ClientIdList
{
    uint32 *array;
    size_t length;
    pthread_mutex_t lock;
}; 

extern void ClientIdListInit(ClientIdList *self); 

extern void ClientIdListCleanup(ClientIdList *self); 

extern size_t ClientIdListLength(ClientIdList *self); 

extern int ClientIdListInsertEnd(ClientIdList *self, uint32 *item); 

extern uint32 *ClientIdListGet(ClientIdList *self, size_t index, uint32 *item); 

extern size_t ClientIdListRemoveItem(ClientIdList *self, uint32 *item); 

