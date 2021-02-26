/*===========================================================================

  Copyright (c) 2016-2017 , 2018 by Qualcomm Technologies, Inc. All Rights Reserved
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/


#ifdef _MSC_VER
  // enable memory leak detection under msvc
  #define _CRTDBG_MAP_ALLOC
  #include <stdlib.h>
  #include <crtdbg.h>
#endif

#include <stdlib.h>
#include <string.h>

#include "rpmh_os.h"
#include "inq.h"

struct inq_s
{
  unsigned link_offset;
  unsigned size;
  void *head;
  void *tail;
};

inq_t *inq_create (unsigned link_offset)
{
  inq_t *self = rpmh_malloc (sizeof(inq_t));
  rpmh_core_verify_ptr (self);
  memset (self, 0, sizeof(inq_t));

  self->link_offset = link_offset;
  return self;
}

void inq_destroy (inq_t *self)
{
  rpmh_free(self);
}

unsigned inq_size (inq_t *self)
{
  return self->size;
}

static inq_link_t *get_link (inq_t *self, void *object)
{
  return (inq_link_t *)(((char *)object) + self->link_offset);
}

void inq_append (inq_t *self, void *object)
{
  if (!self->tail)
  {
    self->head = object;
    self->tail = object;
    get_link(self, object)->next = 0;
  }
  else
  {
    get_link (self, self->tail)->next = object;
    get_link (self, object)->next     = 0;
    self->tail                        = object;
  }

  ++(self->size);
}

void *inq_pop (inq_t *self)
{
  void *object;

  if (!self->size)
  {	  
    return NULL;
  }

  object = self->head;
  self->head = get_link(self, object)->next;
  if (!self->head)
  {	  
    self->tail = 0;
  }    

  get_link (self, object)->next = 0;

  --(self->size);
  return object;
}

void *inq_head (inq_t *self)
{
  return self->head;
}

void inq_it_begin (inq_t *self, inq_iterator_t *it)
{
  it->inq     = self;
  it->last    = NULL;
  it->current = self->head;
}

void inq_it_next( inq_iterator_t *it )
{
  it->last = it->current;
  if( it->last )
  {
    it->current = get_link( it->inq, it->last )->next;
  }    
}

void *inq_it_deref (inq_iterator_t *it)
{
  return it->current;
}

void inq_it_insert (inq_iterator_t *it, void *object)
{
  // If we're off the back of the list, insert == append.
  if (!it->current)
  {
    it->last = it->inq->tail;
    inq_append(it->inq, object);
    it->current = object;
    return;
  }

  // We might also be at the head of the list, but even if we are we know the
  // list isn't empty (since that would be handled by the case above).
  if (!it->last)
  {
    rpmh_core_verify (it->current == it->inq->head);
    
    get_link (it->inq, object)->next = it->inq->head;
    it->inq->head = object;
  }
  else
  {
    // Middle of the list; simple enough.
    get_link (it->inq, object)->next   = it->current;
    get_link (it->inq, it->last)->next = object;
  }
      
  it->current = object;
  ++(it->inq->size);
}

void *inq_it_delete (inq_iterator_t *it)
{
  void *deleted;

  if (!it->current)
  {	  
    return NULL;
  }    

  if (!it->last)
  {
    rpmh_core_verify (it->current == it->inq->head);

    deleted = inq_pop (it->inq);
    it->current = it->inq->head;
  }
  else
  {
    deleted = it->current;

    get_link (it->inq, it->last)->next = get_link (it->inq, it->current)->next;
    it->current = get_link (it->inq, it->current)->next;

    if (deleted == it->inq->tail)
    {	    
      it->inq->tail = it->last;
    }      

    --(it->inq->size);
  }

  return deleted;
}
