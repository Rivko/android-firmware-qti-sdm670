/*===========================================================================

  inq.h - intrusive queue container

  Copyright (c) 2012-2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#ifndef INQ_H
#define INQ_H

typedef struct inq_s inq_t;

typedef struct
{
    void *next;
} inq_link_t;

inq_t *inq_create(unsigned link_offset);

void inq_destroy(inq_t *self);

unsigned inq_size(inq_t *self);

void inq_append(inq_t *self, void *object);

void *inq_pop(inq_t *self);

void *inq_head(inq_t *self);

typedef struct
{
    inq_t *inq;
    void  *last;
    void  *current;
} inq_iterator_t;

void inq_it_begin(inq_t *self, inq_iterator_t *it);

void inq_it_next(inq_iterator_t *it);
void *inq_it_deref(inq_iterator_t *it);
void inq_it_insert(inq_iterator_t *it, void *object);
void *inq_it_delete(inq_iterator_t *it);

#endif // INQ_H

