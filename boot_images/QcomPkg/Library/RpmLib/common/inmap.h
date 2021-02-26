/*===========================================================================

  inmap.h - intrusive map container (internally, an avl tree)

  Copyright (c) 2012-2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#ifndef INMAP_H
#define INMAP_H

typedef struct inmap_s inmap_t;

typedef struct
{
    void    *left;
    void    *right;
    unsigned height;
} inmap_link_t;

inmap_t *inmap_create(unsigned link_offset, unsigned key_offset);

void inmap_destroy(inmap_t *self);

unsigned inmap_size(inmap_t *self);

unsigned inmap_height(inmap_t *self);

typedef struct
{
    unsigned inserted;
    void    *object;
} inmap_insert_t;

// Inserts the item into the tree.  If an item with that key already exists, the item is not inserted.
// Returns an inmap_insert_t with the following properties:
//   - If an existing object of the same key was found:
//       - "inserted" = 0
//       - "object" = points to pre-existing object
//
//   - If no object was found:
//       - "inserted" = 1
//       - "object" points to the newly inserted object
inmap_insert_t inmap_insert(inmap_t *self, void *object);

void *inmap_search(inmap_t *self, unsigned key);

typedef void (*inmap_visit_func)(void *object, void *context);

// In-order tree traversal.
void inmap_visit(inmap_t *self, inmap_visit_func visit_func, void *context);

// Post-order tree traversal.
void inmap_visit_postorder(inmap_t *self, inmap_visit_func visit_func, void *context);

#endif // INMAP_H

