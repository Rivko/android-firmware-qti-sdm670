/*==============================================================================
@file CoreEventNotification.h

This file provides interface for generic Event Notification API.

Copyright (c) 2009-2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

$Header: //components/rel/core.qdsp6/2.1/power/utils/inc/CoreEventNotification.h#1 $
==============================================================================*/
#ifndef CORE_EVENT_NOTIFICATION_H
#define CORE_EVENT_NOTIFICATION_H

#include "CorePool.h"
#include "DALSys.h"

/**
   CORE_EVENT_PRIORITY defines priorities for callbacks triggered
   by the Core_Event API.
*/
typedef enum
{
   CORE_EVENT_PRIORITY_HIGHEST,/**< Highest Priority, Fired First */
   CORE_EVENT_PRIORITY_NORMAL, /**< Normal Priority, Fired Second */
   CORE_EVENT_PRIORITY_LOWEST, /**< Lowest Priority, Fired Last */
   CORE_EVENT_PRIORITY_COUNT,  /**< Maximum number of valid priorities */
} CORE_EVENT_PRIORITY;

/**
   @brief Function signature of the Core Event Callback API

   @param event: Event which cause callback to be called
   @param server_data : pointer to server side data which
   is passed to client on event triggering
   @param client_data : pointer to client side data which
   is passed back to client on event triggering
*/
typedef void (*Core_EventCallback)(uint32 event, void* server_data, void* client_data);

typedef struct
{
   Core_EventCallback cb; /* Pointer to Callback Function */
   void* client_data;     /* Client specific data for cb */
} CBData;

/* Linked List Specific Data Struct */
typedef struct CBList
{
   CBData data;
   struct CBList* next;
   struct CBList* prev;
}CBList;

typedef struct
{
   uint32 max_event;
   CBList** e_list;
   void* cb_data;
   CorePoolType cb_pool;
   DALSYSSyncHandle lock;
} Core_Event;

typedef Core_Event* Core_EventHandle;

/**
   @brief Function to initialize a Core_Event handle

   Core_Event API requires that events can be be enumerated as
   uint32 sized integers which start as zero. The valid range of 
   events allowable after initialization is from zero to 
   (num_events - 1). Including both 0 and (num_events - 1)

   Initialization will fail and return a NULL handle if num_events 
   are zero or if Core_Event pointers points to invalid memory.

   @param pe: Pointer to un-initialized Event object
   @param num_events: maximum event number for callback
   @return Valid handle to Core_Event struct or NULL
   if initialization fails
*/
Core_EventHandle Core_EventInitialize(Core_Event* pe,
                                      uint32 num_events);

/**
   @brief Function to Register a Callback with the Event API

   This function requires an initialized Core_EventHandle h.
   The event should be the range of 0 and maximum event id as 
   registered by Core_EventInitialize(). Multiple callbacks can 
   be registered for a given event. An optional client_data can
   be passed. This data is passed to the callback function when
   events are fired. Successful registration returns 1. If any 
   of the parameters are invalid or ambigious then return value 
   is 0 and further behavior is undefined.

   @param pe: Pointer to the initialized Core_Event structure.
   This pointer is returned by Core_EventInitialize()
   @param event: event for which notification is required.
   Valid range is 0 to (num_events - 1) 
   @param cb: Callback which is required to be called when event
   fires. cb must point to a valid function.
   @param priority: priority with which the callback should be
   invoked once event has fired. No order is defined for callbacks 
   which have the same priority.
   @param client_data: data which can be provided to callback
   function on event triggering.
   @return On successful registration of the event the return 
   value of this function is TRUE. Otherwise FALSE is returned.
*/
DALBOOL Core_EventRegister(Core_EventHandle h,
                           uint32 event,
                           Core_EventCallback cb,
                           CORE_EVENT_PRIORITY priority,
                           void* client_data);

/**
   @brief Function to De-Register a Callback with the Event API

   This function requires an initialized Core_EventHandle h.
   The event should be the range of 0 and maximum event id as 
   registered by Core_EventInitialize(). client_data, priority 
   and cb values should be identical to the values used during 
   Core_EventRegister(). Successful de-registration of callback
   returns 1. If any of the parameters are invalid or ambigious 
   then return value is 0 and further behavior is undefined.

   @param h: Pointer to the initialized Core_Event structure.
   This pointer is returned by Core_EventInitialize()
   @param event: event for which deregistration is required.
   Valid range is 0 to (num_events - 1) 
   @param cb: Callback which had been successfully registered
   for this event. cb must point to a valid function.
   @param priority: priority with which the callback was 
   registered.
   @param client_data: client_data which was used to register 
   the callback.
   @return Successful registration returns a value of TRUE. 
   Otherwise FALSE is returned.
*/
DALBOOL Core_EventDeRegister(Core_EventHandle h,
                             uint32 event,
                             Core_EventCallback cb,
                             CORE_EVENT_PRIORITY priority,
                             void* client_data);
/**
   @brief Function to Notify Callbcks of event occurances

   This function is triggered by the server side functionality. 
   To trigger event callbacks with the registered clients.
   Core_EventHandle must be previously obtained using 
   Core_EventInitialize. 

   @param h: Handle to an initialized Core_Event object.
   @param event: event for which callbacks are required to fire.
   @param server_data: pointer to data passed from event server to 
   every client callback which as registered fro this event.
   @return number of events fired.
*/
uint32 Core_EventNotify(Core_EventHandle h,
                        uint32 event,
                        void* server_data);

#endif /*CORE_EVENT_NOTIFICATION_H*/
