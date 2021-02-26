#pragma once
/*=============================================================================
  @file sns_list.h

  A simple linked list implementation for use by the Sensors Framework.

  Iterators (and their associated API functions) are required for all access
  and modifications to the list.  Any number of iterators may be simultaneously
  active for a list.  Adding items to a list is guaranteed synchronous and
  thread-safe.  Removing items from a list may block if the item is in-use
  by another iterator.

  Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include <stdbool.h>
#include <stdint.h>
#include "sns_rc.h"

/*=============================================================================
  Forward Declarations
  ===========================================================================*/
struct sns_osa_lock;
struct sns_list_item;

/*=============================================================================
  Type Definitions
  ===========================================================================*/

typedef struct sns_list_item
{
  void *item;
  /* Pointer to subsequent item on the list; nullptr if the head */
  struct sns_list_item *next;
  /* Pointer to previous item on the list; nullptr if the tail */
  struct sns_list_item *prev;
  /* Reference to the containing list; nullptr if not on any list */
  struct sns_list *list;
} sns_list_item;

typedef struct sns_list
{
  /* Contains head (item::next) and tail (item::prev) of the list;
   * item::item always NULL */
  sns_list_item item;
  /* Number of entries presently collected within this list */
  uint32_t cnt;
} sns_list;

/**
 * An accessor class for the List.  All mutator and accessor actions
 * must be done through the Iterator interface.
 */
typedef struct sns_list_iter
{
  /* Entry is used by the list to manage this iterator */
  struct sns_list_item *list_entry;

  /* Current Entry of this iterator.  May be NULL. */
  struct sns_list_item *curr;
  /* List over which this iterates */
  struct sns_list *list;
} sns_list_iter;

/*=============================================================================
  Public Functions
  ===========================================================================*/

/**
 * Initialize a list to its empty state.
 *
 * @param[i] this This List
 *
 * @return
 * SNS_RC_SUCCESS
 */
sns_rc sns_list_init(struct sns_list *this);

/**
 * Initialize a list item.
 *
 * @param[i] this Item in this List
 * @param[i] item Item initialization data
 *
 * @return
 * SNS_RC_SUCCESS
 */
sns_rc sns_list_item_init(sns_list_item *this, void *item);

/**
 * Remove an item from a list.
 *
 * @param[i] this List item to be removed if present on a list
 */
void sns_list_item_remove(sns_list_item *this);

/**
 * Return a pointer to the data associated with this list item.
 *
 * @param[i] this Item in this List
 *
 * @return Data associated with this List Item
 */
void* sns_list_item_get_data(struct sns_list_item *this);

/**
 * Check whether this item is presently on any list.
 *
 * @param[i] this Item whose list presence to check
 *
 * @return True if on any list; false otherwise
 */
bool sns_list_item_present(struct sns_list_item const *this);

/**
 * Create an iterator for this List.
 *
 * @param[i] this Iterator for this List
 * @param[i] list this List
 * @param[i] head Initialize iterator to first item in the list; otherwise last
 *
 * @return First item if head is True. Last Item if head is False.
 *         NULL if list is empty
 */
sns_list_item * sns_list_iter_init(sns_list_iter *this,
                          struct sns_list *list,
                          bool head);

/**
 * Add this item to the specified List; advance iterator only if
 * the List was previously empty.
 *
 * @param[i] this Iterator for this List
 * @param[i] item Item to be inserted in this list
 * @param[i] after If true insert after current item, false enter before
 *
 * @return
 * SNS_RC_SUCCESS
 */
sns_rc sns_list_iter_insert(sns_list_iter *this,
                            struct sns_list_item *item,
                            bool after);

/**
 * Remove the current Entry.  Advance iterator to next; if next is NULL,
 * advance to prev.
 *
 * @param[i] this Iterator for this List
 *
 * @return Item removed from this List
 */
sns_list_item* sns_list_iter_remove(sns_list_iter *this);

/**
 * Return the current length of the associated List.
 *
 * @param[i] this Iterator for this List
 *
 * @return Length of this List
 */
uint32_t sns_list_iter_len(sns_list_iter *this);

/**
 * Return the current list_item.  Will be NULL if current item is invalid.
 * E.g. If the list is empty, or has iterated past the head/tail.
 *
 * @param[i] this Iterator for this List
 *
 * @return Current item in this List
 */
struct sns_list_item* sns_list_iter_curr(sns_list_iter *this);

/**
 * Advance the iterator to the next item in the list.
 *
 * @param[i] this Iterator for this List
 *
 * @return Next item in this List
 */
struct sns_list_item* sns_list_iter_advance(sns_list_iter *this);

/**
 * Return the iterator to the previous item in the list.
 *
 * @param[i] this Iterator for this List
 *
 * @return Previous item in this List
 */
struct sns_list_item* sns_list_iter_return(sns_list_iter *this);

/**
 * Return a pointer to the data associated with the current list item.
 *
 * This is equivalent to the operation:
 *   sns_list_item_get_data(sns_list_iter_curr(this))
 *
 * @param[i] this Iterator for this List
 *
 * @return Data pointer to the current item in this List
 */
void *
sns_list_iter_get_curr_data(sns_list_iter *this);
