/*==========================================================================
@file npa_remote_publish_resource.h

NPA Remote API to publish one or multiple resources to a different address space

Copyright (c) 2010-2012 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

$Header: //components/rel/core.qdsp6/2.1/power/npa/src/remote/npa_remote_publish_resource.h#1 $
============================================================================*/
#ifndef NPA_REMOTE_PUBLISH_RESOURCE_H
#define NPA_REMOTE_PUBLISH_RESOURCE_H

#include "npa_resource.h"
#include "npa_remote_resource.h"

typedef struct npa_remote_published_resource
{
  struct npa_remote_published_resource *next, *prev;
  npa_link                             *local_link;  
  unsigned int                          remote_domains;
} npa_remote_published_resource;

extern npa_remote_published_resource *npa_remote_published_resource_list;

#endif /* NPA_REMOTE_PUBLISH_RESOURCE_H */
