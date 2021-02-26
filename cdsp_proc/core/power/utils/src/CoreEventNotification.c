/*============================================================================
@file CoreEventNotification.c

Implements a generic Event Notification API

Copyright (c) 2009-2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

$Header: //components/rel/core.qdsp6/2.1.c4/power/utils/src/CoreEventNotification.c#1 $
============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

#include "DALSys.h"
#include "CoreVerify.h"
#include "CoreEventNotification.h"

/**
   @brief Find CBData element in linked list pointed by list

   @param list pointer to head of linked list
   @param pdata pointer to data element to be searched
   @return pointer to linked-list element if found null otherwise
*/
static CBList* CBList_FindCb(CBList* list, CBData* pdata)
{
   CBList* curr = list;

   while(curr && memcmp((void*)&(curr->data), (void*)pdata, sizeof(CBData)) != 0)
   {
      curr = curr->next;
   };

   return curr;
}

/**
   @brief Insert CBData element in linked list pointed by phead

   @param phead head of linked list
   @param pdata pointer to data element to be inserted
   @return pointer to inserted element
*/
static CBList* CBList_InsertCb(CBList** phead, CBList* first)
{
   CBList* second;

   /* List is empty */
   if(*phead == NULL)
   {
      *phead = first;
   }
   /* Insert Before the first element */
   else 
   {
      second = *phead;
      *phead = first;
      first->next = second;
      second->prev = first;
   }
   return *phead;
}

/**
   @brief Delete CBData element in linked list pointed by phead

   @param phead head of linked list
   @param pdata pointer to data element to be deleted
*/
static void CBList_DeleteCb(CBList** phead, CBList* elem)
{
   CBList* left;
   CBList* right;

   /* Check if elem is first */
   if(*phead == elem)
   {
      *phead = elem->next;
      (*phead)->prev = (CBList*)NULL;
   }
   /* Delete from the middle of a list */
   else
   {
      left = elem->prev;
      right = elem->next;
      left->next = right;
      /* check if elem is last on the list */
      if(right)
      {
         right->prev = left;
      }
   }
   return;
}

static void* Core_EventPoolAllocator(unsigned int size)
{
   void* data = NULL;

   /*Allocate space */
   if(DALSYS_Malloc( size, &data) != DAL_SUCCESS)
   {
      return NULL;
   }
   return data;
}

static DALBOOL Core_EventPoolInit(Core_Event* pe,
                                  uint32 e_size,
                                  uint32 e_num)
{
   uint32 pool_block_size = CORE_POOL_BLOCK_SIZE(e_num,e_size);

   /*Allocate space for call-back Pool */
   if(DALSYS_Malloc( pool_block_size, &(pe->cb_data)) != DAL_SUCCESS
      || NULL == pe->cb_data) /* Klocworks */
   {
      return FALSE;
   }
   
   /*Initialize memory to NULL */
   memset(pe->cb_data, 0, pool_block_size);

   /*Initialize Pool */
   Core_PoolInit(&(pe->cb_pool), sizeof(CBList),
                 pe->cb_data, pool_block_size);

   /*Assign Allocator */
   Core_PoolUseAllocatorFcn(&(pe->cb_pool), Core_EventPoolAllocator);

   return TRUE;

}

static void* Core_EventPoolAlloc(Core_Event* pe)
{
   return Core_PoolAlloc(&(pe->cb_pool));
}

static void Core_EventPoolFree(Core_Event* pe, void* pdata)
{
   Core_PoolFree(&(pe->cb_pool),pdata);
   return;
}

Core_EventHandle Core_EventInitialize(Core_Event* pe,
                                      uint32 num_events)
{
   uint32 n_elem = 0;

   CORE_VERIFY_PTR(pe);

   /*Validate Input*/
   if(num_events == 0)
   {
      return NULL;
   }
   
   n_elem = (num_events) * CORE_EVENT_PRIORITY_COUNT;

   /*Allocate Array of CBList */
   if(DALSYS_Malloc( n_elem * sizeof(CBList*), 
                     (void**)&(pe->e_list)) != DAL_SUCCESS
      || NULL == pe->e_list)  /* Klocworks */
   {
      return NULL;
   }
   
   /*Initialize all CBList* to NULL */
   memset(pe->e_list, 0, n_elem * sizeof(CBList*));

   /*Allocate space for call-back Pool, one callback per event */
   if(!Core_EventPoolInit(pe, sizeof(CBList), n_elem))
   {
      return NULL;
   }
   
   /* Initialize our lock */
   DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE, &(pe->lock), NULL);

   /*Save Event Information */
   pe->max_event = num_events - 1;

   return pe;
}

/** 
    @brief this function looks-up the head of the CBList

    For CoreEventHandle: h->max_event = m;
    CORE_EVENT_PRIORITY_COUNT         = 3;

    The call-back event dispatch mechanism looks up a 
    data struct which looks like below:
    --------------------------------------------
    index         h->e_list            CBList**
    --------------------------------------------
    0      event_0_priority_HIGHEST     head_0_H
    1      event_0_priority_NORMAL      head_0_N
    2      event_0_priority_LOWEST      head_0_L
    3      event_1_priority_HIGHEST     NULL
    4      event_1_priority_NORMAL      NULL
    5      event_1_priority_LOWEST      head_1_L
    ...
    3*m+1  event_m_priority_HIGHEST     head_m_H
    3*m+2  event_m_priority_NORMAL      head_m_N
    3*m+3  event_m_priority_LOWEST      head_m_L
    --------------------------------------------

    @param h valid handle to Event
    @param e valid event for h
    @param p priority of event
    @return pointer to ehad of linked list

*/
static CBList** Core_EventIndexCBList(Core_EventHandle h, 
                                      uint32 e, 
                                      CORE_EVENT_PRIORITY p)
{
   return h->e_list + (e)* CORE_EVENT_PRIORITY_COUNT + p;
}

/*
  @brief Function to Register a Callback with the Event API
*/
DALBOOL Core_EventRegister(Core_EventHandle h,
                           uint32 event,
                           Core_EventCallback cb,
                           CORE_EVENT_PRIORITY priority,
                           void* client_data)
{
   DALBOOL ret = FALSE;
   CBData data;
   CBList** phead;
   CBList* pnew;

   /* Validate input */
   if(!h || !cb || priority >= CORE_EVENT_PRIORITY_COUNT)
   {
      return ret;
   }
   
   if(event > h->max_event)
   {
      return ret;
   }
   
   data.cb = cb;
   data.client_data = client_data; 

   /* convert event id into array index */
   phead = Core_EventIndexCBList(h,event,priority);

   /* Serialize */
   DALSYS_SyncEnter(h->lock);

   if(CBList_FindCb(*phead, &data) == NULL) 
   {
      pnew = (CBList *)Core_EventPoolAlloc(h);
      if(pnew)
      {
         pnew->next = pnew->prev = NULL;
         pnew->data = data;
         CBList_InsertCb(phead, pnew);
         ret = TRUE;
      }
   }

   /* Un Serialize */
   DALSYS_SyncLeave(h->lock);

   return ret;
}

/*
  @brief Function to De-Register a Callback with the Event API
*/
DALBOOL Core_EventDeRegister(Core_EventHandle h,
                             uint32 event,
                             Core_EventCallback cb,
                             CORE_EVENT_PRIORITY priority,
                             void* client_data)
{
   DALBOOL ret = FALSE;    
   CBData data;
   CBList*  pelem;
   CBList** phead;

   /* Validate input */
   if(!h || !cb || priority >= CORE_EVENT_PRIORITY_COUNT)
   {
      return ret;
   }
   
   if(event > h->max_event)
   {
      return ret;
   }
   
   data.cb = cb;
   data.client_data = client_data;

   /* convert event id into array index */
   phead = Core_EventIndexCBList(h,event,priority);

   /* Serialize */
   DALSYS_SyncEnter(h->lock);

   pelem = CBList_FindCb(*phead, &data);

   if(pelem)
   {
      CBList_DeleteCb(phead, pelem);
      Core_EventPoolFree(h, (void*)pelem);
      ret = TRUE;
   }

   /* Un Serialize */
   DALSYS_SyncLeave(h->lock);

   return ret;
}

/*
  @brief Function to Notify Callbcks of event occurances
*/
uint32 Core_EventNotify(Core_EventHandle h, 
                        uint32 event, 
                        void* server_data)
{
   CBList** phead;
   CBList* elem;
   uint32 events_fired = 0;
   unsigned int priority;

   CORE_VERIFY_PTR(h);

   /* Validate input */
   if(event > h->max_event)
   {
      return 0;
   }
   
   /* Serialize */
   DALSYS_SyncEnter(h->lock);

   /* Scan over all priorities */
   for(priority = CORE_EVENT_PRIORITY_HIGHEST;
       priority < CORE_EVENT_PRIORITY_COUNT;
       priority++)
   {
      phead = Core_EventIndexCBList(h,event,(CORE_EVENT_PRIORITY)priority);
      elem = *phead;
      /* Scan over linked list at each priority */
      while(elem)
      {
         elem->data.cb(event, server_data, elem->data.client_data);
         events_fired++;
         elem = elem->next;
      }
   }

   /* Un Serialize */
   DALSYS_SyncLeave(h->lock);

   return events_fired;
}

#ifdef __cplusplus
}
#endif
