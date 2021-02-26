/*==============================================================================
  FILE:         unpa.c
  
  OVERVIEW:     Init, resource definition routines in the UNPA framework

  DEPENDENCIES: None

                Copyright (c) 2016 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/unpa/src/framework/unpa.c#1 $
$DateTime: 2018/07/30 01:21:36 $
==============================================================================*/
#include <stdlib.h>
#include <string.h>
#include "comdef.h"
#include "CoreVerify.h"
#include "unpa_internal.h"
#include "unpa_resource.h"

/*==============================================================================
                             EXTERNAL REFERENCES
 =============================================================================*/
void unpa_qdiDriverInit(void);

/*==============================================================================
                              GLOBAL VARIABLES
 =============================================================================*/
unpa_struct unpa;

/*==============================================================================
                       PRIVATE FUNCTIONS & RESOURCES
 =============================================================================*/
static unpa_resource_state unpa_nullUpdateFcn(unpa_resource *resource, 
                                              unpa_client   *client)
{
  return 0;
}
 
static unpa_resource_state unpa_nullDriverFcn(unpa_resource      *resource, 
                                              unpa_client        *client,
                                              unpa_resource_state state)
{
  return state;
}

static unpa_resource_definition unpa_null_resource_defn =
{
  "STUB",
  unpa_nullUpdateFcn,
  unpa_nullDriverFcn,
  0xFFFFFFFF,
  UNPA_ALL_CLIENT_TYPES,
  UNPA_RESOURCE_DEFAULT,
};

static unpa_resource unpa_null_resource; 

/*==============================================================================
                              GLOBAL FUNCTIONS
 =============================================================================*/
/** 
 * unpa_init
 * 
 * @brief Initializes the uNPA framework.
 */ 
void unpa_init(void)
{
  UNPA_MUTEX_INIT(&unpa.lock);
  
  // Init log here
  
  unpa_initResource(&unpa_null_resource_defn, 0, &unpa_null_resource);
  
#ifdef UNPA_MULTI_PD
  unpa_qdiDriverInit();
#endif
}

/* 
 * unpa_resourceHasAttribute
 */
uint32 unpa_resourceHasAttribute(unpa_resource *resource, uint32 attribute)
{
  return (resource->definition->attributes & attribute);
}

/*
 * unpa_getResource
 */
unpa_resource* unpa_getResource(const char *resourceName )
{
  unpa_resource *resource = unpa.resources;
  
  UNPA_MUTEX_LOCK(&unpa.lock);
  
  while (resource)
  {
    if (0 == strncmp(resourceName, resource->definition->name, UNPA_MAX_NAME_LEN))
    {
      break;
    }
    resource = resource->next;
  }
  
  /* If we don't see a defined resource, look among stubs */
  if (!resource)
  {
    int32 i;
    for (i = 0; i < UNPA_MAX_STUBS && unpa.stubs[i] != NULL; ++i)
    {
      if (0 == strncmp(resourceName, unpa.stubs[i], UNPA_MAX_NAME_LEN))
      {
        resource = &unpa_null_resource;
        break;
      }
    }
  }
  
  UNPA_MUTEX_UNLOCK(&unpa.lock);
  
  return resource;
}

/*
 * unpa_newResource
 */ 
unpa_resource* unpa_newResource(unpa_resource_definition *definition,
                                unpa_resource_state       initialState,
                                unpa_resource            *resource)
{
  unpa_client client;
  
  CORE_VERIFY_PTR(definition);
  CORE_VERIFY(strlen(definition->name) < UNPA_MAX_NAME_LEN);
  
  /* Verify that no resource with the given name is already defined/stubbed */
  CORE_VERIFY(unpa_getResource(definition->name) == NULL);
  
  if (!resource)
  {
    resource = (unpa_resource *)malloc(sizeof(unpa_resource));
    CORE_VERIFY_PTR(resource);
  }
  
  memset(resource, 0, sizeof(unpa_resource));
  
  resource->definition = definition;
  resource->active_max = definition->max;
  
  UNPA_MUTEX_INIT(&resource->lock);
  
  /* Call into resource with the special INITIALIZE client */
  memset(&client, 0, sizeof(unpa_client));
  client.type = UNPA_CLIENT_INITIALIZE;
  client.pending_request.val = initialState;
  
  resource->active_state = definition->driver_fcn(resource, &client, initialState);

  /* Link in resource */
  UNPA_MUTEX_LOCK(&unpa.lock);
  resource->next = unpa.resources;
  unpa.resources = resource;
  UNPA_MUTEX_UNLOCK(&unpa.lock);
  
  return resource;
}

/* 
 * unpa_stubResource
 */ 
void unpa_stubResource(const char *resourceName)
{
  unpa_resource *resource;
  int32 i;
  
  CORE_VERIFY_PTR(resourceName);
  CORE_VERIFY(strlen(resourceName) < UNPA_MAX_NAME_LEN);
  
  UNPA_MUTEX_LOCK(&unpa.lock);

  /* Verify that no such resource is already defined/stubbed */  
  resource = unpa_getResource(resourceName);
  CORE_VERIFY(resource == NULL);
  
  for (i = 0; i < UNPA_MAX_STUBS; ++i)
  {
    if (unpa.stubs[i] == NULL)
    {
      unpa.stubs[i] = resourceName;
      break;
    }
  }
  
  /* If we didn't find a slot, we have too many stubs */
  CORE_VERIFY(i != UNPA_MAX_STUBS);
  
  UNPA_MUTEX_UNLOCK(&unpa.lock);

  return;
}
