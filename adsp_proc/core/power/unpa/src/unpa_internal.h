/*==============================================================================
  FILE:         unpa_internal.h
  
  OVERVIEW:     uNPA Internal data structures, types etc. 

  DEPENDENCIES: None

                Copyright (c) 2016 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/unpa/src/unpa_internal.h#1 $
$DateTime: 2017/07/21 22:10:47 $
==============================================================================*/
#ifndef _UNPA_INTERNAL_H_
#define _UNPA_INTERNAL_H_

#include "comdef.h"
#include "qurt_mutex.h"
#include "unpa_resource.h"

/*==============================================================================
                              MACRO DEFINITIONS
 =============================================================================*/
#define UNPA_MAX_STUBS 3
#define UNPA_LOG(x)

/* Kernel specific MUTEX macros */
#define UNPA_MUTEX_INIT(lock)     qurt_mutex_init(lock)
#define UNPA_MUTEX_LOCK(lock)     qurt_mutex_lock(lock)
#define UNPA_MUTEX_TRY_LOCK(lock) qurt_mutex_try_lock(lock)
#define UNPA_MUTEX_UNLOCK(lock)   qurt_mutex_unlock(lock)

/* Client attributes */
#define UNPA_CLIENT_ON_HEAP 0x1

/* Event attributes */
#define UNPA_EVENT_ON_HEAP 0x1

/*==============================================================================
                                TYPE DEFINITIONS
 =============================================================================*/
typedef uint32 UNPA_LOG_T;

typedef struct unpa_struct_s
{
  unpa_resource *resources;               /* The list of defined UNPA resources */
  const char    *stubs[UNPA_MAX_STUBS];   /* The list of stubbed UNPA resources */
  qurt_mutex_t   lock;                    /* Mutex to serialise access to the above lists */
  UNPA_LOG_T     log;                     /* UNPA activity is logged into this log */
} unpa_struct;

/*==============================================================================
                              EXTERNAL REFERENCES
 =============================================================================*/
extern unpa_struct unpa;

/*==============================================================================
                              GLOBAL FUNCTIONS
 =============================================================================*/
/**
 * unpa_resourceHasAttribute
 * 
 * @brief Returns TRUE if the resource has the given attribute.
 * 
 * @param resource:
 * @param attribute:
 */
uint32 unpa_resourceHasAttribute(unpa_resource *resource, uint32 attribute);

/**
 * unpa_getResource
 *  
 * @brief Returns the unpa_resource* with the given name or NULL. 
 *  
 * @param resourceName:
 */
unpa_resource* unpa_getResource(const char *resourceName);

/**
 * unpa_updateResource 
 *  
 * @brief Process the request from client 
 *  
 * @param client: 
 */
void unpa_updateResource(unpa_client *client);

/** 
 * unpa_getSupportedClientTypes 
 *  
 * @brief Returns a mask with client types supported by the 
 *        given (UNPA-provided) update function
 *  
 * @param updateFcn 
 */
uint32 unpa_getSupportedClientTypes(unpa_resource_update_fcn updateFcn);

/** 
 * unpaEvent_dispatch 
 *  
 * @brief Calls registered callbacks for a registered node event
 *  
 * @param updateFcn 
 */
void unpaEvent_dispatch(unpa_resource  *resource,
                        unpa_event_type eventType, void *eventData);
                        
#endif /* _UNPA_INTERNAL_H_ */
