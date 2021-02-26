/*=========================================================================
@file npa_list.c

NPA linked list routines. Should be in utils.

Copyright (c) 2009-2015, Qualcomm Technologies, Inc.
All Rights Reserved.

==========================================================================*/

#include "npa_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

void npa_list_Init( const npa_pool_config *config )
{
  CorePoolConfigType  pool_config;

  CORE_VERIFY_PTR( config );

  if ( NULL == npa.list_pool.hLock )
  {
    pool_config.size       = sizeof(npa_list_elem);
    pool_config.initial    = config->initial;
    pool_config.subsequent = config->subsequent;
    Core_PoolConfig( &npa.list_pool, &pool_config );
  }
}

void npa_list_PushFront( npa_list_header *list, void *data )
{
  npa_list_elem *elem;
  
  elem = (npa_list_elem *)Core_PoolAlloc( &npa.list_pool );
  CORE_VERIFY_PTR( elem );
  elem->data = data;
  elem->next = list->head;
  if ( list->head == NULL )
  {
    list->tail = elem;
  }
  list->head = elem;
}

void *npa_list_PopFront( npa_list_header *list )
{
  npa_list_elem *elem = list->head;
  if ( elem )
  {
    void *data = elem->data;
    list->head = elem->next;
    if ( list->head == NULL )
    {
      list->tail = NULL;
    }
    Core_PoolFree( &npa.list_pool, elem );
    return data;
  }
  return NULL;
}

void npa_list_PushBack( npa_list_header *list, void *data )
{
  npa_list_elem *elem;
  
  elem = (npa_list_elem *)Core_PoolAlloc( &npa.list_pool );
  CORE_VERIFY_PTR( elem );
  elem->data = data;
  elem->next = NULL;
  if ( list->tail )
  {
    list->tail->next = elem;
    list->tail = elem;
  }
  else
  {
    list->tail = list->head = elem;
  }
}

void* npa_list_Find( npa_list_header *list, void *data )
{
  npa_list_elem *it = list->head;
  while ( it )
  {
    if ( it->data == data )
    {
      return it->data;
    }
    it = it->next;
  }
  return NULL;
}

void npa_list_Remove( npa_list_header *list, void *data )
{
  npa_list_elem *prev, *next, *elem;

  prev = NULL;
  next = list->head;
  while ( NULL != next )
  {
    if ( next->data == data )
    {
      if ( NULL == prev )
      {
        list->head = next->next;
      }
      else
      {
        prev->next = next->next;
      }
      elem = next;
      next = elem->next;
      if ( next == NULL )
      {
        list->tail = prev;
      }
      Core_PoolFree( &npa.list_pool, elem );
    }
    else
    {
      prev = next;
      next = next->next;
    }
  }
}

void npa_list_Apply( npa_list_header *list, void (*fcn)(void *data) )
{
  npa_list_elem *elem;
  for ( elem = list->head; elem != NULL; elem = elem->next )
  {
    fcn(elem->data);
  }
}

void npa_list_Apply2( npa_list_header *list, 
                      void (*fcn)(void *data, void *data2), void *data2 )
{
  npa_list_elem *elem;
  for ( elem = list->head; elem != NULL; elem = elem->next )
  {
    fcn( elem->data, data2 );
  }
}

void npa_list_Clear( npa_list_header *list )
{
  npa_list_elem *elem, *next;

  next = list->head;
  while ( NULL != next )
  {
    elem = next;
    next = next->next;
    Core_PoolFree( &npa.list_pool, elem );
  }
  list->head = NULL;
  list->tail = NULL;
}

void npa_list_ApplyAndClear( npa_list_header *list, void (*fcn)(void *data) )
{
  npa_list_elem *elem, *next;

  next = list->head;
  while ( NULL != next )
  {
    elem = next;
    next = next->next;
    fcn(elem->data);
    Core_PoolFree( &npa.list_pool, elem );
  }
  list->head = NULL;
  list->tail = NULL;
}

void npa_list_SwapLists( npa_list_header *list1, npa_list_header *list2 )
{
  npa_list_header temp = *list1;
  *list1 = *list2;
  *list2 = temp;
}

unsigned int npa_list_IsEmpty( npa_list_header *list )
{
  return ( NULL == list->head );
}

/* Return element at head of list, without popping it off */
void *npa_list_PeekFront( npa_list_header *list )
{
  return ( list->head != NULL ? list->head->data : NULL );
}

#ifdef __cplusplus
}
#endif
