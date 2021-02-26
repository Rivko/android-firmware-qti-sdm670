#ifndef _UNPA_EVENT_H_
#define _UNPA_EVENT_H_
/*==============================================================================
  FILE:         unpa_event.h
  
  OVERVIEW:     uNPA event client APIs.

  DEPENDENCIES: None

                Copyright (c) 2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/api/power/unpa/unpa_event.h#1 $
$DateTime: 2018/07/30 01:21:36 $
==============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

#include "comdef.h"

/*==============================================================================
                                TYPE DEFINITIONS
 =============================================================================*/

/* Event types. Each event type is represented by a single bit, so enum values
   must be powers-of-two */
typedef enum
{
  /* Indicates that the driver function of a resource was invoked and that the
     resource may have changed state. event_data is resource->active_state */
  UNPA_POST_CHANGE_EVENT = 0x1,
  
  /* Used for a sanity check in add_event_callback; must be the last element
     in the enum */
  UNPA_MAX_EVENT_TYPES,
} unpa_event_type;

typedef void (*unpa_event_cb)(unpa_event_type eventType, void *eventData, void *cbData);

typedef struct unpa_event
{
  /* Types of events this data structure is registered against */
  uint32             types : 24;
  
  /* Special attributes of this event data structure */
  uint32             attr : 8;
  
  /* Callback to be invoked when an event in "types" occurs */
  unpa_event_cb      callback;
  
  /* Opaque data to be passed into callback */
  void              *cbdata;
  
  struct unpa_event *next;
} unpa_event;
 
typedef unpa_event* unpa_event_handle;

/*==============================================================================
                              GLOBAL FUNCTIONS
 =============================================================================*/
/** 
 * unpaEvent_addCallback
 *
 * @brief Register a callback with a resource, to be dispatched when the
 *        resource encounters one of the events in "eventTypes" (see the
 *        unpa_event_type enum). The callback is dispatched with eventType,
 *        eventData (if any) and "cbData" as arguments. An unpa_event data
 *        structure is allocated, initialized and linked to the resource.
 *
 * @param resourceName: Name of the resource to register the event with.
 * @param eventTypes:   Bitset of one or more event types from unpa_event_type
 * @param callback:     Callback to be dispatched upon occurrence of event
 * @param cbdata:       Pointer to any data the caller wants passed into callback.
 *
 * @return An event handle or NULL, if the event could not be registered
 */
#define unpaEvent_addCallback(resourceName, eventTypes, callback, cbData) \
          unpaEvent_newCallback(resourceName, eventTypes, callback, cbData, NULL)

/** 
 * unpaEvent_initCallback
 *
 * @brief Register a callback with a resource, to be dispatched when the
 *        resource encounters one of the events in "eventTypes" (see the
 *        unpa_event_type enum). The callback is dispatched with eventType,
 *        eventData (if any) and "cbData" as arguments. The passed in 
 *        unpa_event data structure is initialized and linked to the resource.
 *
 * @param resourceName: Name of the resource to register the event with.
 * @param eventTypes:   Bitset of one or more event types from unpa_event_type
 * @param callback:     Callback to be dispatched upon occurrence of event
 * @param cbData:       Pointer to any data the caller wants passed into callback.
 * @param event:        Pointer to a (uninitialized, static) unpa_event data structure.
 *
 * @return "event" or NULL, if the event could not be registered
 */
#define unpaEvent_initCallback(resourceName, eventTypes, callback, cbData, event) \
          unpaEvent_newCallback(resourceName, eventTypes, callback, cbData, event)

/** 
 * unpaEvent_newCallback
 *
 * @brief Do not use this routine directly. Instead, use either
 * unpaEvent_addCallback or unpaEvent_initCallback depending on whether you want
 * UNPA to allocate a unpa_event data structure or pass in one.
 */            
unpa_event_handle unpaEvent_newCallback(const char    *resourceName,
                                        uint32         eventTypes,
                                        unpa_event_cb  callback, 
                                        void          *cbdata,
                                        unpa_event    *event);

/** 
 * unpaEvent_removeCallback
 *
 * @brief Remove a previously registered callback from resource. 
 * This routine may be used with events that were allocated (addCallback) or
 * just initialized (initCallback).
 *
 * @param resourceName: Name of the resource to deregister the event from.
 * @param callback:     Callback used to identify the event to remove/deregister
 */    
void unpaEvent_removeCallback(const char *resourceName, unpa_event_cb callback);

#ifdef __cplusplus
}
#endif

#endif /* _UNPA_EVENT_H_ */
