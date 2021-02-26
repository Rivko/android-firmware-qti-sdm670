/*=============================================================================
  @file sns_list.c

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
#include "sns_island.h"
#include "sns_island_util.h"
#include "sns_list.h"
#include "sns_types.h"

/*=============================================================================
  Public Function Definitions
  ===========================================================================*/

SNS_SECTION(".text.sns") sns_rc
sns_list_item_init(sns_list_item *this, void *item)
{
  SNS_ASSERT(NULL != this);
  SNS_ASSERT(NULL != item);

  this->next = NULL;
  this->prev = NULL;
  this->list = NULL;
  this->item = item;

  return SNS_RC_SUCCESS;
}

SNS_SECTION(".text.sns") void*
sns_list_item_get_data(sns_list_item *this)
{
  if(NULL == this)
    return NULL;

  SNS_ASSERT(NULL != this->item);
  return this->item;
}

SNS_SECTION(".text.sns") bool
sns_list_item_present(struct sns_list_item const *this)
{
  return NULL != this->list;
}

SNS_SECTION(".text.sns") void
sns_list_item_remove(sns_list_item *this)
{

  if(sns_list_item_present(this))
  {
    this->prev->next = this->next;
    this->next->prev = this->prev;

    --this->list->cnt;

    this->prev = NULL;
    this->next = NULL;
    this->list = NULL;
  }
}

SNS_SECTION(".text.sns") sns_rc
sns_list_init(sns_list *this)
{

  this->item.item = NULL;
  this->item.prev = &this->item;
  this->item.next = &this->item;
  this->item.list = this;
  this->cnt = 0;

  return SNS_RC_SUCCESS;
}

SNS_SECTION(".text.sns") sns_list_item *
sns_list_iter_init(sns_list_iter *this, sns_list *list, bool head)
{
  this->curr = head ? list->item.next : list->item.prev;
  this->list = list;
  return (this->curr != &this->list->item)? this->curr : NULL;
}

SNS_SECTION(".text.sns") sns_rc
sns_list_iter_insert(sns_list_iter *this, sns_list_item *item, bool after)
{
  SNS_ASSERT(NULL != item && NULL != item->item);

  if(sns_list_item_present(item))
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

SNS_SECTION(".text.sns") sns_list_item*
sns_list_iter_remove(sns_list_iter *this)
{
  sns_list_item *rv = this->curr;
  SNS_ASSERT(&this->list->item != rv);
  SNS_ASSERT(NULL != rv->item);

  this->curr = rv->next;

  sns_list_item_remove(rv);
  return rv;
}

SNS_SECTION(".text.sns") uint32_t
sns_list_iter_len(sns_list_iter *this)
{
  return this->list->cnt;
}

SNS_SECTION(".text.sns") sns_list_item*
sns_list_iter_curr(sns_list_iter *this)
{
  return (this->curr != &this->list->item) ? this->curr : NULL;
}

SNS_SECTION(".text.sns") sns_list_item*
sns_list_iter_advance(sns_list_iter *this)
{
  this->curr = this->curr->next;
  return (this->curr != &this->list->item) ? this->curr : NULL;
}

SNS_SECTION(".text.sns") sns_list_item*
sns_list_iter_return(sns_list_iter *this)
{
  this->curr = this->curr->prev;
  return (this->curr != &this->list->item) ? this->curr : NULL;
}

SNS_SECTION(".text.sns") void *
sns_list_iter_get_curr_data(sns_list_iter *this)
{
  void *ret_val = NULL;
  if(this->curr != &this->list->item)
  {
    SNS_ASSERT(NULL != this->curr->item);
    ret_val = this->curr->item;
  }
  return ret_val;
}
