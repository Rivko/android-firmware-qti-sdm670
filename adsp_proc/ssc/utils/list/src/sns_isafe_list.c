/*=============================================================================
  @file sns_isafe_list.c

  A simple linked list implementation for use by the Sensors Framework.

  Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include <stddef.h>
#include "sns_assert.h"
#include "sns_isafe_list.h"
#include "sns_island.h"
#include "sns_island_util.h"
#include "sns_types.h"

/*=============================================================================
  Public Function Definitions
  ===========================================================================*/

SNS_SECTION(".text.sns") sns_rc
sns_isafe_list_item_init(sns_isafe_list_item *this, void *item)
{
  SNS_ASSERT(NULL != this);
  SNS_ASSERT(NULL != item);

  if(!sns_island_is_island_ptr((intptr_t)this))
  {
    SNS_ISLAND_EXIT();
  }

  this->next = NULL;
  this->prev = NULL;
  this->list = NULL;
  this->item = item;

  return SNS_RC_SUCCESS;
}

SNS_SECTION(".text.sns") void*
sns_isafe_list_item_get_data(sns_isafe_list_item *this)
{
  if(NULL == this)
    return NULL;

  if(!sns_island_is_island_ptr((intptr_t)this) ||
     !sns_island_is_island_ptr((intptr_t)this->item))
  {
    SNS_ISLAND_EXIT();
  }

  SNS_ASSERT(NULL != this->item);
  return this->item;
}

SNS_SECTION(".text.sns") bool
sns_isafe_list_item_present(struct sns_isafe_list_item const *this)
{
  if(!sns_island_is_island_ptr((intptr_t)this))
  {
    SNS_ISLAND_EXIT();
  }

  return NULL != this->list;
}

SNS_SECTION(".text.sns") void
sns_isafe_list_item_remove(sns_isafe_list_item *this)
{
  if(sns_isafe_list_item_present(this))
  {
    if(!sns_island_is_island_ptr((intptr_t)this->list) ||
       !sns_island_is_island_ptr((intptr_t)this->next) ||
       !sns_island_is_island_ptr((intptr_t)this->prev))
    {
      SNS_ISLAND_EXIT();
    }

    this->prev->next = this->next;
    this->next->prev = this->prev;

    --this->list->cnt;

    this->prev = NULL;
    this->next = NULL;
    this->list = NULL;
  }
}

SNS_SECTION(".text.sns") sns_rc
sns_isafe_list_init(sns_isafe_list*this)
{
  if(!sns_island_is_island_ptr((intptr_t)this))
  {
    SNS_ISLAND_EXIT();
  }

  this->item.item = NULL;
  this->item.prev = &this->item;
  this->item.next = &this->item;
  this->item.list = this;
  this->cnt = 0;

  return SNS_RC_SUCCESS;
}

SNS_SECTION(".text.sns") sns_isafe_list_item *
sns_isafe_list_iter_init(sns_isafe_list_iter *this, sns_isafe_list*list, bool head)
{
  if(!sns_island_is_island_ptr((intptr_t)list) ||
     !sns_island_is_island_ptr((intptr_t)this))
  {
    SNS_ISLAND_EXIT();
  }
  this->check_island = (SNS_ISLAND_STATE_IN_ISLAND == sns_island_get_island_state());
  this->curr = head ? list->item.next : list->item.prev;
  this->list = list;
  this->always_check_island = false;

  return (this->curr != &this->list->item)? this->curr : NULL;
}

SNS_SECTION(".text.sns") sns_rc
sns_isafe_list_iter_insert(sns_isafe_list_iter *this, sns_isafe_list_item *item, bool after)
{
  if(this->check_island &&
    (!sns_island_is_island_ptr((intptr_t)this)             ||
     !sns_island_is_island_ptr((intptr_t)this->curr)       ||
     !sns_island_is_island_ptr((intptr_t)this->curr->next) ||
     !sns_island_is_island_ptr((intptr_t)this->list)       ||
     !sns_island_is_island_ptr((intptr_t)item)             ||
     !sns_island_is_island_ptr((intptr_t)item->next)       ||
     !sns_island_is_island_ptr((intptr_t)item->prev)       ||
     !sns_island_is_island_ptr((intptr_t)item->list)))
  {
    SNS_ISLAND_EXIT();
    this->check_island = this->always_check_island;
  }
  SNS_ASSERT(NULL != item && NULL != item->item);

  if(sns_isafe_list_item_present(item))
    return SNS_RC_FAILED;

  if(after)
  {
    item->next = this->curr->next;
    item->prev = this->curr;
    this->curr->next = item;
    item->next->prev = item;
  }
  else
  {
    item->prev = this->curr->prev;
    item->next = this->curr;
    this->curr->prev = item;
    item->prev->next = item;
  }

  item->list = this->list;
  ++this->list->cnt;

  return SNS_RC_SUCCESS;
}

SNS_SECTION(".text.sns") sns_isafe_list_item*
sns_isafe_list_iter_remove(sns_isafe_list_iter *this)
{
  if(this->check_island &&
    (!sns_island_is_island_ptr((intptr_t)this) ||
     !sns_island_is_island_ptr((intptr_t)this->curr)))
  {
    SNS_ISLAND_EXIT();
    this->check_island = this->always_check_island;
  }

  sns_isafe_list_item *rv = this->curr;
  SNS_ASSERT(&this->list->item != rv);
  SNS_ASSERT(NULL != rv->item);

  this->curr = rv->next;

  sns_isafe_list_item_remove(rv);
  return rv;
}

SNS_SECTION(".text.sns") uint32_t
sns_isafe_list_iter_len(sns_isafe_list_iter *this)
{
  if(this->check_island &&
    (!sns_island_is_island_ptr((intptr_t)this) ||
     !sns_island_is_island_ptr((intptr_t)this->list)))
  {
    SNS_ISLAND_EXIT();
    this->check_island = this->always_check_island;
  }

  return this->list->cnt;
}

SNS_SECTION(".text.sns") sns_isafe_list_item*
sns_isafe_list_iter_curr(sns_isafe_list_iter *this)
{
  if(this->check_island &&
    (!sns_island_is_island_ptr((intptr_t)this) ||
     !sns_island_is_island_ptr((intptr_t)this->list) ||
     !sns_island_is_island_ptr((intptr_t)this->curr)))
  {
    SNS_ISLAND_EXIT();
    this->check_island = this->always_check_island;
  }

  return (this->curr != &this->list->item) ? this->curr : NULL;
}

SNS_SECTION(".text.sns") sns_isafe_list_item*
sns_isafe_list_iter_advance(sns_isafe_list_iter *this)
{
  if(this->check_island &&
    (!sns_island_is_island_ptr((intptr_t)this) ||
     !sns_island_is_island_ptr((intptr_t)this->list) ||
     !sns_island_is_island_ptr((intptr_t)this->curr)))
  {
    SNS_ISLAND_EXIT();
    this->check_island = this->always_check_island;
  }

  this->curr = this->curr->next;
  return (this->curr != &this->list->item) ? this->curr : NULL;
}

SNS_SECTION(".text.sns") sns_isafe_list_item*
sns_isafe_list_iter_return(sns_isafe_list_iter *this)
{
  if(this->check_island &&
    (!sns_island_is_island_ptr((intptr_t)this) ||
     !sns_island_is_island_ptr((intptr_t)this->list) ||
     !sns_island_is_island_ptr((intptr_t)this->curr)))
  {
    SNS_ISLAND_EXIT();
    this->check_island = this->always_check_island;
  }

  this->curr = this->curr->prev;
  return (this->curr != &this->list->item) ? this->curr : NULL;
}

SNS_SECTION(".text.sns") void
sns_isafe_list_iter_set_always_check_island_ptrs(sns_isafe_list_iter *this,
                                           bool always_check)
{
  SNS_ASSERT(NULL != this);
  if(!sns_island_is_island_ptr((intptr_t)this))
  {
    SNS_ISLAND_EXIT();
  }
  this->always_check_island = always_check;
  this->check_island = always_check;
}

SNS_SECTION(".text.sns") void *
sns_isafe_list_iter_get_curr_data(sns_isafe_list_iter *this)
{
  void *ret_val = NULL;
  if(this->check_island &&
    (!sns_island_is_island_ptr((intptr_t)this) ||
     !sns_island_is_island_ptr((intptr_t)this->list) ||
     !sns_island_is_island_ptr((intptr_t)this->curr) ||
     !sns_island_is_island_ptr((intptr_t)this->curr->item)))
  {
    SNS_ISLAND_EXIT();
    this->check_island = this->always_check_island;
  }

  if(this->curr != &this->list->item)
  {
    SNS_ASSERT(NULL != this->curr->item);
    ret_val = this->curr->item;
  }
  return ret_val;
}
