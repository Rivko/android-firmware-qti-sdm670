#ifndef HASHTABLE_H
#define HASHTABLE_H
/*==============================================================================
  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All rights reserved. Qualcomm Proprietary and Confidential.
==============================================================================*/

#include "mmdefs.h"

typedef struct hash_node hash_node;
struct hash_node {
   hash_node *next_ptr;
   int key_size;
   const void* key_ptr;
};

typedef void node_free_f(void *free_context_ptr, hash_node *node);

typedef struct hashtable hashtable;
struct hashtable {
   hash_node **table_ptr;
   int table_size;
   int mask_size;

   uint32_t resize_factor;

   node_free_f* free_f;
   void *free_context_ptr;
   uint32_t num_nodes; // Number of total nodes in hash, including dups
   uint32_t num_items; // Number of items hashed
};

hash_node *hashtable_getnext(hashtable *me, uint32_t *pos_ptr);
hash_node *hashtable_find(hashtable *me, const void *key_ptr, int key_size);
int hashtable_remove_all(hashtable *me);
int hashtable_remove(hashtable *me, const void *key_ptr, int key_size,
                     hash_node *node);
int hashtable_insert(hashtable *me, hash_node *node);

int hashtable_init(hashtable *me, int table_size, int resize_factor,
                   node_free_f* free_f, void *void_ptr);
void hashtable_deinit(hashtable *me);

#endif // HASHTABLE_H
