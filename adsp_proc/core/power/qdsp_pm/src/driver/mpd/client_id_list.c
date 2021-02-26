/*
* Copyright (c) 2015 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

#include <stdlib.h>
#include <string.h>
#include "client_id_list.h"

///////////////////////////////////////////////////////////////////////////////
// Static functions 
///////////////////////////////////////////////////////////////////////////////

static int uint32compare(uint32 *left, uint32 *right)
{
    if((*left) < (*right)) { return -1; }
    if((*left) > (*right)) { return 1; }
    return 0;
}

static void ClientIdListFreeIndex(ClientIdList *self, size_t index,
    size_t count)
{
    self->length -= count;
    memmove(&(self->array[index]), &(self->array[index + count]),
        (self->length - index) * sizeof(uint32));
    if(self->length > 0)
    {
        uint32 *newArray = (uint32 *)((realloc))(self->array,
            self->length * sizeof(uint32));
        self->array = (newArray != NULL) ? newArray : self->array;
    }
    else
    {
        ((free))(self->array);
        self->array = NULL;
    }
} 

static uint32 *ClientIdListAllocEnd(ClientIdList *self, size_t count)
{
    size_t index = self->length;
    uint32 *newArray = (uint32 *)((realloc))(self->array,
        (index + count) * sizeof(uint32));
    if(newArray == NULL)
    {   
        return NULL;
    }
    self->array = newArray;
    self->length += count;
    return &(self->array[index]);
} 

static uint32 *ClientIdListGetArray(ClientIdList *self)
{
    return self->array;
} 

///////////////////////////////////////////////////////////////////////////////
// Public functions 
///////////////////////////////////////////////////////////////////////////////

void ClientIdListInit(ClientIdList *self)
{
    self->array = NULL;
    self->length = 0;
    pthread_mutex_init(&self->lock, NULL);
} 

void ClientIdListCleanup(ClientIdList *self)
{
    if(self->array)
    {
        ((free))(self->array);
        self->array = NULL;
    }
    self->length = 0;
    pthread_mutex_destroy(&self->lock);
} 

size_t ClientIdListLength(ClientIdList *self)
{
    size_t length;
    pthread_mutex_lock(&self->lock);
    length = self->length;
    pthread_mutex_unlock(&self->lock);
    return length;
} 

int ClientIdListInsertEnd(ClientIdList *self, uint32 *item)
{
    pthread_mutex_lock(&self->lock);
    uint32 *pItem = ClientIdListAllocEnd(self, 1);
    if(pItem != NULL)
    {
        (*pItem) = (*item);
    }
    pthread_mutex_unlock(&self->lock);
    return (pItem != NULL);
} 

uint32 *ClientIdListGet(ClientIdList *self, size_t index, uint32 *item)
{
    uint32 *array;
    pthread_mutex_lock(&self->lock);
    array = ClientIdListGetArray(self);
    (*item) = array[index];
    pthread_mutex_unlock(&self->lock);
    return item;
} 

size_t ClientIdListRemoveItemRange(ClientIdList *self, size_t index,
    size_t endIndex, uint32 *item)
{
    uint32 *array;
    pthread_mutex_lock(&self->lock);
    array = ClientIdListGetArray(self);
    while(index < endIndex)
    {
        if(((uint32compare))(item, &(array[index])) == 0)
        {
            ClientIdListFreeIndex(self, index, 1);
            pthread_mutex_unlock(&self->lock);
            return 1;
        }
        index++;
    }
    pthread_mutex_unlock(&self->lock);
    return 0;
} 

size_t ClientIdListRemoveItem(ClientIdList *self, uint32 *item)
{
    return ClientIdListRemoveItemRange(self, 0, ClientIdListLength(self), item);
} 

