/*==============================================================================
  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All rights reserved. Qualcomm Proprietary and Confidential.
==============================================================================*/

#include "hashtable.h"

#include "adsp_error_codes.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "AEEstd.h"
#ifndef ADSP_AMDB_TEST
#include "stringl.h"
#include "qurt_elite_memory.h"
#else
#include "test_main.h"
#endif

static void *hashtable_malloc(size_t bytes)
{
   return qurt_elite_memory_malloc(bytes, QURT_ELITE_HEAP_DEFAULT);
}

static void hashtable_free(void *ptr)
{
   qurt_elite_memory_free(ptr);
}

// 32 bit magic FNV prime and init values
#define FNV_32_PRIME 0x01000193
#define FNV_32_INIT  0x811C9DC5 //2166136261
static __inline uint32_t fnv_hash(const void *void_ptr, int len, uint32_t h)
{
  const uint8_t *byte_ptr = (uint8_t*)void_ptr;

  while (len--)
  {
    h *= FNV_32_PRIME;
    h ^= (uint32_t)*byte_ptr++;
  }
  return h;
}

static __inline int hashtable_equalkey(const void *key_ptr1, int key_size1,
                                       const void *key_ptr2, int key_size2)
{
  if (key_size1 != key_size2)
  {
    return 0;
  }
  return (0 == memcmp(key_ptr1, key_ptr2, key_size1));
}

// assign 22 bits for the table index and 10 for the linkpos
#define TABLE_POS(x) (x >> 10)
#define LINK_POS(y)  (y & 0x3FF)
#define HASH_ENUM_ST(x,y) (x << 10 | (y & 0x3FF))

hash_node *hashtable_getnext(hashtable *me, uint32_t *pos_ptr)
{
  hash_node *node = 0;
  int nPos = *pos_ptr;
  int table_pos = TABLE_POS(nPos);
  int link_pos = LINK_POS(nPos);
  int i = 0;

  if (table_pos >= me->table_size)
  {
    return 0;
  }

  node = me->table_ptr[table_pos];
  while (i < link_pos && node)
  {
    node = node->next_ptr;
    i++;
  }

  if (node)
  {
    *pos_ptr = HASH_ENUM_ST(table_pos, (link_pos+1));
    return node;
  }

  // if node is not available, move to the next position
  table_pos++;

  while (table_pos < me->table_size)
  {
    if (me->table_ptr[table_pos])
    {
      *pos_ptr = HASH_ENUM_ST(table_pos, 1);
      return me->table_ptr[table_pos];
    }
    table_pos++;
  }

  return 0;
}

static hash_node **hashtable_findpos(hashtable *me, const void *key_ptr, int key_size)
{
  hash_node **node_ptrptr;
  uint32_t nHash;
  uint32_t nPos;

  nHash = fnv_hash(key_ptr, key_size, FNV_32_INIT);
  nPos = nHash & me->mask_size; // Only take enough bits as the table
                                // size from the hash

  for (node_ptrptr = &me->table_ptr[nPos];
      (*node_ptrptr) && !hashtable_equalkey((*node_ptrptr)->key_ptr,
                                            (*node_ptrptr)->key_size,
                                            key_ptr, key_size);
      node_ptrptr = &(*node_ptrptr)->next_ptr)
  {
  };

  return node_ptrptr;
}

hash_node *hashtable_find(hashtable *me, const void *key_ptr, int key_size)
{
  hash_node **node_ptrptr = hashtable_findpos(me, key_ptr, key_size);
  return *node_ptrptr;
}

static int hashtable_internalinsert(hashtable* me, hash_node* node_ptr)
{
  hash_node** node_ptrptr = hashtable_findpos(me, node_ptr->key_ptr,
                                              node_ptr->key_size);
  if ((hash_node*)0 != *node_ptrptr)
  {
    hash_node* dup_ptr;
    // identical nodes not allowed, will result in non-terminating linked list
    for (dup_ptr = *node_ptrptr; 0 != dup_ptr; dup_ptr = dup_ptr->next_ptr)
    {
      if (node_ptr == dup_ptr)
      {
        return ADSP_EALREADY;
      }
    }

    node_ptr->next_ptr = (*node_ptrptr);
    *node_ptrptr = node_ptr;
  }
  else
  {
    // add this new node at the beginning
    *node_ptrptr = node_ptr;
    node_ptr->next_ptr = 0;

    me->num_items++; // increment number of hashed keys
  }

  me->num_nodes++; // increment number of total nodes

  return ADSP_EOK;
}

static int hashtable_grow(hashtable *me)
{
  int new_table_size = me->table_size * me->resize_factor;
  int old_table_size;
  hash_node **new_table_ptr, **old_table_ptr;
  int i;
  int err;

  new_table_ptr = (hash_node**)hashtable_malloc(new_table_size * sizeof(hash_node*));
  if (0 == new_table_ptr)
  {
    err = ADSP_ENOMEMORY;
    return err;
  }

  memset(new_table_ptr, 0, new_table_size * sizeof(hash_node*));
  old_table_ptr = me->table_ptr;
  me->table_ptr = new_table_ptr;

  old_table_size  = me->table_size;
  me->table_size = new_table_size;
  me->mask_size  = me->table_size - 1;

  for (i = 0; i < old_table_size; i++)
  {
    hash_node* node = old_table_ptr[i]; // get list head

    // Reverse the linked list so that nodes with the same key are added in the order in which they were added to the old hash table.
    {
       hash_node *current_node = node;
       hash_node *prev_node = NULL;
       while(current_node)
       {
          hash_node *next_node = current_node->next_ptr;
          current_node->next_ptr = prev_node;
          prev_node = current_node;
          current_node = next_node;
       }
       node = prev_node;
    }

    while (node) // re-insert all items in old head
    {
      hash_node* nodeInsert = node;
      node = node->next_ptr; // insert whacks next_ptr
      (void) hashtable_internalinsert(me, nodeInsert);
    }
  }

  hashtable_free(old_table_ptr);

  return ADSP_EOK;
}

int hashtable_remove_all(hashtable *me)
{
  int i;

  for (i = 0; i < me->table_size; i++)
  {
    hash_node *node = me->table_ptr[i];

    while (node)
    {
      hash_node *free_node = node;

      node = node->next_ptr;

      if (me->free_f)
      {
        me->free_f(me->free_context_ptr, free_node);
      }
    }
    me->table_ptr[i] = 0;
  }

  me->num_items = 0;
  me->num_nodes = 0;

  return ADSP_EOK;
}

int hashtable_remove(hashtable *me, const void* key_ptr, int key_size,
                     hash_node *node_ptr)
{
  hash_node **node_ptrptr;

  node_ptrptr = hashtable_findpos(me, key_ptr, key_size);
  if (node_ptrptr && *node_ptrptr)
  {
    hash_node *free_node = *node_ptrptr;
    hash_node *next_ptr = (*node_ptrptr)->next_ptr;

    // decrement number total nodes
    me->num_nodes--;

    if (0 == node_ptr || node_ptr == free_node)
    {
      *node_ptrptr = next_ptr;
      // decrement num items in the table
      me->num_items--;
    }
    else
    {
      // remove a specific dup
      hash_node* free_nodeDup = free_node->next_ptr;

      while (free_nodeDup)
      {
        if (node_ptr == free_nodeDup)
        {
          free_node->next_ptr = free_nodeDup->next_ptr;
          free_node = free_nodeDup;
          break;
        }
        free_node = free_nodeDup;
        free_nodeDup = free_nodeDup->next_ptr;
      }
    }

    if (me->free_f)
    {
      me->free_f(me->free_context_ptr, free_node);
    }

    return ADSP_EOK;
  }

  return ADSP_EFAILED;
}

int hashtable_insert(hashtable *me, hash_node *node_ptr)
{
  int err;

  // TODO remove
  node_ptr->next_ptr = 0;
  err = hashtable_internalinsert(me, node_ptr);
  if (ADSP_EOK == err)
  {
    if (me->num_items > me->table_size * me->resize_factor)
    {
      (void)hashtable_grow(me);
    }
  }

  return err;
}

int hashtable_init(hashtable *me, int table_size,
                   int resize_factor, node_free_f* free_f, void *void_ptr)
{
  int err = ADSP_EOK;
  uint32_t log_size = 0;

  memset(me, 0, sizeof(hashtable));

  me->num_nodes = 0;
  me->num_items = 0;

  me->free_f = free_f;
  me->free_context_ptr = void_ptr;

  // make it a power of 2...
  // if its not a power of 2, shorten it to the closest previous 2th power
  while (0 != (table_size >>= 1))
  {
    log_size++;
  }

  table_size = (uint32_t)1 << log_size;
  if (me->num_items || 0 == table_size)
  {
    return ADSP_EUNSUPPORTED;
  }

  if (0 >= resize_factor)
  {
    return ADSP_EUNSUPPORTED;
  }

  me->resize_factor = resize_factor;

  if (table_size > me->table_size)
  {
    me->table_ptr = (hash_node**)hashtable_malloc(table_size * sizeof(hash_node*));
    if (0 != me->table_ptr)
    {
      memset(me->table_ptr, 0, table_size * sizeof(hash_node*));
      me->table_size = table_size;
      me->mask_size  = table_size - 1;
    }
    else
    {
      err = ADSP_ENOMEMORY;
    }
  }

  return err;
}

void hashtable_deinit(hashtable* me)
{
  (void)hashtable_remove_all(me);
  if (me->table_ptr)
  {
    hashtable_free(me->table_ptr);
  }
}

