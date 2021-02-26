/*==============================================================================
  FILE:         unpa_event.c
  
  OVERVIEW:     Routines to create, destroy and manage UNPA events

  DEPENDENCIES: None

                Copyright (c) 2016 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/unpa/src/framework/unpa_event.c#1 $
$DateTime: 2018/07/30 01:21:36 $
==============================================================================*/
#include <stdlib.h>
#include "comdef.h"
#include "unpa.h"
#include "unpa_event.h"
#include "CoreVerify.h"
#include "unpa_internal.h"

/*==============================================================================
                              GLOBAL FUNCTIONS
 =============================================================================*/
/* 
 * unpaEvent_newCallback
 */
unpa_event_handle unpaEvent_newCallback(const char    *resourceName,
                                        uint32         eventTypes,
                                        unpa_event_cb  callback, 
                                        void          *cbData,
                                        unpa_event    *event)
{
  unpa_resource *resource;
  
  CORE_VERIFY_PTR(resourceName);
  CORE_VERIFY(eventTypes < UNPA_MAX_EVENT_TYPES);
  CORE_VERIFY_PTR(callback);
  
  resource = unpa_getResource(resourceName);

  if (resource)
  {
    if (!event)
    {
      event = (unpa_event *)calloc(1, sizeof(unpa_event));
      CORE_VERIFY_PTR(event);
      event->attr = UNPA_EVENT_ON_HEAP;
    }
    else
    {
      memset(event, 0, sizeof(unpa_event));
    }

    event->types    = eventTypes;
    event->callback = callback;
    event->cbdata   = cbData;
    
    UNPA_MUTEX_LOCK(&resource->lock);
    
    /* Link in event */
    if (!resource->events)
    {
      resource->events = event;
    }
    else
    {
      unpa_event *ep = resource->events;

      while (ep->next)
      {
        ep = ep->next;
      }
      ep->next = event;
    }
    
    UNPA_MUTEX_UNLOCK(&resource->lock);

    return event;
  }

  return NULL;
}

/* 
 * unpaEvent_removeCallback
 */
void unpaEvent_removeCallback(const char *resourceName, unpa_event_cb callback)
{
  unpa_resource *resource;
  
  CORE_VERIFY_PTR(resourceName);
  CORE_VERIFY_PTR(callback);
  
  resource = unpa_getResource(resourceName);

  if (resource)
  {
    UNPA_MUTEX_LOCK(&resource->lock);

    if (resource->events)
    {
      unpa_event *event = resource->events;
      unpa_event *temp  = NULL;
      
      if (event->callback == callback)
      {
        temp             = event;
        resource->events = temp->next;
      }
      else
      {
        while (event->next)
        {
          if (event->next->callback == callback)
          {
            temp        = event->next;
            event->next = temp->next;
            break;
          }
          event = event->next;
        }
      }
      
      if (temp && (temp->attr & UNPA_EVENT_ON_HEAP))
      {
        free(temp);
      }
    }

    UNPA_MUTEX_UNLOCK(&resource->lock);
  }

  return;
}

/* 
 * unpaEvent_dispatch
 */
void unpaEvent_dispatch(unpa_resource  *resource,
                        unpa_event_type eventType, void *eventData)
{
  unpa_event *event = resource->events;
  
  while (event)
  {
    if (event->types & eventType)
    {
      event->callback(eventType, eventData, event->cbdata);
    }
    event = event->next;
  }

  return;
}
