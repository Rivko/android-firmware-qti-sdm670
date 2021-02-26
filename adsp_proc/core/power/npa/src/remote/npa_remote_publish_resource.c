/*============================================================================
@file npa_remote_publish_resource.c

NPA Remote API to publish one or multiple resources to a different address space

Copyright (c) 2010-2013 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

$Header: //components/rel/core.qdsp6/2.1/power/npa/src/remote/npa_remote_publish_resource.c#1 $
============================================================================*/
#include "npa_internal.h"
#include "npa_inlines.h"
#include "npa_remote_publish_resource.h"

#define npa_remote_published_resource_pool (npa.link_pool)
CORE_STATIC_VERIFY( sizeof(npa_remote_published_resource) <= sizeof(npa_link) );

/* List of all published resources */
npa_remote_published_resource *npa_remote_published_resource_list;

npa_remote_published_resource*
npa_remote_find_published_resource( const char *resource_name )
{
  npa_remote_published_resource *pubr = npa_remote_published_resource_list;

  CORE_VERIFY_PTR( resource_name );

  while ( pubr )
  {
    if ( 0 == strncmp( resource_name, pubr->local_link->name, 
                       NPA_MAX_STR_LEN + 1 ) )
    {
      return pubr;
    }
    pubr = pubr->next;
  }
  return NULL;
}

void npa_remote_publish_resource_cb( void *context, unsigned int event_type,
                                     void *data, unsigned int data_size )
{
  npa_link *resource_link;
  npa_remote_published_resource *pubr;
  unsigned int remote_domains = (unsigned int)context;
  const char *resource_name = (const char *)data;

  resource_link = npa_find_resource_link( resource_name );

  /* Use the graph lock to serialise. For now */
  npa_lock_graph();

  pubr = npa_remote_find_published_resource( resource_name );
  if ( !pubr )
  {
    /* Resource has not been published before */
    pubr = Core_PoolAlloc( &npa_remote_published_resource_pool );
    CORE_VERIFY_PTR( pubr );
    pubr->local_link = resource_link;
    pubr->remote_domains = remote_domains;
    pubr->next = npa_remote_published_resource_list;
    pubr->prev = NULL;

    if ( npa_remote_published_resource_list )
    {
      npa_remote_published_resource_list->prev = pubr;
    }

    npa_remote_published_resource_list = pubr;
  }
  else
  {
    pubr->remote_domains |= remote_domains;
  }

  npa_unlock_graph();

  ULOG_RT_PRINTF_2( npa.log, 
                    "\tpublish complete (resource: \"%s\") (domains: 0x%08x)",
                    resource_name, pubr->remote_domains );
}

void npa_remote_publish_resource( unsigned int remote_domains,
                                  const char *resource_name )
{
  CORE_VERIFY_PTR( resource_name );
  CORE_VERIFY( remote_domains != 0 );
  
  ULOG_RT_PRINTF_2( npa.log, 
                    "npa_remote_publish_resource (resource: \"%s\") "
                    "(domains: 0x%08x)", resource_name, remote_domains );

  npa_resource_available_cb( resource_name, 
                             npa_remote_publish_resource_cb,
                             (void *)remote_domains );
}

void npa_remote_publish_resources( unsigned int remote_domains,
                                   unsigned int num_resources,
                                   const char *resources[] )
{
  int i;

  CORE_VERIFY( num_resources != 0 && resources != NULL );

  for ( i = 0; i < num_resources; i++ )
  {
    npa_remote_publish_resource( remote_domains, resources[i] );
  }
}
