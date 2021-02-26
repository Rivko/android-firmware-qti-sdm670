/*===========================================================================

  vect.h - dynamic array of (void *)

  Copyright (c) 2012-2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#ifndef VECT_H
#define VECT_H

typedef struct vect_s vect_t;

vect_t *vect_create(void);

void vect_destroy(vect_t *self);

unsigned vect_size(vect_t *self);

void vect_resize(vect_t *self, unsigned new_size);

void vect_push(vect_t *self, void *element);

void *vect_at(vect_t *self, unsigned idx);

void vect_set(vect_t *self, unsigned idx, void *element);

#endif // VECT_H

