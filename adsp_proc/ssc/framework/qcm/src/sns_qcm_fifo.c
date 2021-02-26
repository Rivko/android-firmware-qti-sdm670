/*=============================================================================
  @file sns_qcm_fifo.c

  It is worth noting that the QSocket Client Manager only ever attempts to
  send (or drop) all data from an event fifo.  Therefore, if one thread is
  presently removing entries, it will presumably do so until either all events
  are removed, or it reaches some error it cannot overcome (i.e. QSocket/QMI
  busy).  Hence, this module can simply pretend as though the fifo is empty,
  when any other thread concurrently attempts to remove an event.

  Copyright (c) 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/
#include <stdint.h>
#include "qurt_thread.h"
#include "sns_assert.h"
#include "sns_isafe_list.h"
#include "sns_island.h"
#include "sns_mem_util.h"
#include "sns_memmgr.h"
#include "sns_osa_lock.h"
#include "sns_qcm_fifo.h"
#include "sns_sensor_event.h"
#include "sns_time.h"
#include "sns_types.h"

/*=============================================================================
  Macros and Constants
  ===========================================================================*/

/**
 * If the gap between the oldest and newest events on the FIFO exceed the
 * flush_period, this module is expected to drop that oldest event.  In order
 * to avoid race conditions with the batch timer, we provide a little padding
 * here.
 */
#define FLUSH_PERIOD_PAD 500

/*=============================================================================
  Data Type Definitions
  ===========================================================================*/

/* A client stream event FIFO.  In theory each stream will have two of these:
 * one for recurring events which may be dropped; and another for control/
 * non-recurring events, which may not be dropped. */
typedef struct sns_qcm_fifo
{
  sns_isafe_list fifo;
  /* Thread ID which is currently in the removing entries, but has not yet
   * called commit; only one thread may be in this state. */
  intptr_t remove_tid;
  /* Iterator used by the removing thread; to correctly handle new events
   * added to the fifo on concurrent threads, this iterator always points
   * to the entry prior to the last one returned to the caller. */
  sns_isafe_list_iter remove_iter;
  sns_time flush_period;
  /* Protects all state in this structure */
  sns_osa_lock lock;
} sns_qcm_fifo;

/* A single event within a given fifo.  Placed in sns_qcm_fifo::fifo. */
typedef struct sns_qcm_fifo_entry
{
  sns_isafe_list_item item;
  sns_sensor_event event;
} sns_qcm_fifo_entry;

/*=============================================================================
  Function Definitions
  ===========================================================================*/

SNS_SECTION(".text.sns") void
sns_qcm_fifo_init(sns_qcm_fifo **fifo_out, sns_time flush_period)
{
  sns_qcm_fifo *fifo = *fifo_out;

  if(NULL == fifo)
  {
    sns_osa_lock_attr lattr;
    sns_rc rc;

    fifo = sns_malloc(SNS_HEAP_ISLAND, sizeof(*fifo));
    if(NULL == fifo)
    {
      SNS_ISLAND_EXIT();
      fifo = sns_malloc(SNS_HEAP_MAIN, sizeof(*fifo));
    }
    SNS_ASSERT(NULL != fifo);

    sns_isafe_list_init(&fifo->fifo);

    rc = sns_osa_lock_attr_init(&lattr);
    SNS_ASSERT(SNS_RC_SUCCESS == rc);
#ifdef SNS_ISLAND_INCLUDE_QCM
    rc = sns_osa_lock_attr_set_memory_partition(&lattr, SNS_OSA_MEM_TYPE_ISLAND);
#else
    rc = sns_osa_lock_attr_set_memory_partition(&lattr, SNS_OSA_MEM_TYPE_NORMAL);
#endif
    SNS_ASSERT(SNS_RC_SUCCESS == rc);
    sns_osa_lock_init(&lattr, &fifo->lock);
    SNS_ASSERT(SNS_RC_SUCCESS == rc);

    *fifo_out = fifo;
  }

  fifo->flush_period = flush_period;
}

SNS_SECTION(".text.sns") void
sns_qcm_fifo_insert(sns_qcm_fifo *fifo, sns_sensor_event *event)
{
  sns_isafe_list_iter iter;
  sns_qcm_fifo_entry *entry;
  size_t event_size = sizeof(*event) + event->event_len - sizeof(event->event);

  if(!sns_island_is_island_ptr((intptr_t)fifo))
    SNS_ISLAND_EXIT();

  entry = sns_malloc(SNS_HEAP_ISLAND, sizeof(*entry) + event_size);
  if(NULL == entry)
  {
    SNS_ISLAND_EXIT();
    entry = sns_malloc(SNS_HEAP_MAIN, sizeof(*entry) + event_size);
  }
  SNS_ASSERT(NULL != entry);

  sns_memscpy(&entry->event, event_size, event, event_size);
  sns_isafe_list_item_init(&entry->item, entry);

  sns_osa_lock_acquire(&fifo->lock);
  sns_isafe_list_iter_init(&iter, &fifo->fifo, false);
  sns_isafe_list_iter_insert(&iter, &entry->item, true);

  // PEND: Potential optimization: Store oldest event timestamp
  if(0 == fifo->remove_tid)
  {
    sns_qcm_fifo_entry *temp;
    do
    {
      sns_isafe_list_iter_init(&iter, &fifo->fifo, true);
      temp = sns_isafe_list_item_get_data(sns_isafe_list_iter_curr(&iter));

      if(temp->event.timestamp > event->timestamp ||
          event->timestamp - temp->event.timestamp < fifo->flush_period + FLUSH_PERIOD_PAD)
        break;

      sns_isafe_list_iter_remove(&iter);
      sns_free(temp);
    } while(NULL != sns_isafe_list_iter_curr(&iter));
  }
  sns_osa_lock_release(&fifo->lock);
}

SNS_SECTION(".text.sns") sns_sensor_event*
sns_qcm_fifo_remove(sns_qcm_fifo *fifo, bool remove)
{
  sns_qcm_fifo_entry *entry = NULL;
  qurt_thread_t thread_id = qurt_thread_get_id();

  if(!sns_island_is_island_ptr((intptr_t)fifo))
    SNS_ISLAND_EXIT();

  sns_osa_lock_acquire(&fifo->lock);
  if(0 == fifo->remove_tid || thread_id == fifo->remove_tid)
  {
    if(0 == fifo->remove_tid)
    {
      sns_isafe_list_iter_init(&fifo->remove_iter, &fifo->fifo, true);
      sns_isafe_list_iter_return(&fifo->remove_iter);
    }
    fifo->remove_tid = thread_id;

    if(remove)
      sns_isafe_list_iter_advance(&fifo->remove_iter);

    sns_isafe_list_iter_advance(&fifo->remove_iter);
    entry = sns_isafe_list_item_get_data(sns_isafe_list_iter_curr(&fifo->remove_iter));
    sns_isafe_list_iter_return(&fifo->remove_iter);
  }
  sns_osa_lock_release(&fifo->lock);

  return NULL == entry ? NULL : &entry->event;
}

SNS_SECTION(".text.sns") void
sns_qcm_fifo_commit(sns_qcm_fifo *fifo, sns_qcm_commit_type commit)
{
  qurt_thread_t thread_id = qurt_thread_get_id();

  if(!sns_island_is_island_ptr((intptr_t)fifo))
    SNS_ISLAND_EXIT();

  sns_osa_lock_acquire(&fifo->lock);
  if(thread_id == fifo->remove_tid)
  {
    if(SNS_QCM_COMMIT_DONE == commit)
    {
      sns_isafe_list_iter iter;
      sns_isafe_list_item *remove_head, *curr;
      bool done = false;

      sns_isafe_list_iter_init(&iter, &fifo->fifo, true);
      remove_head = sns_isafe_list_iter_curr(&fifo->remove_iter);
      while(NULL != (curr = sns_isafe_list_iter_curr(&iter)) && !done)
      {
        if(curr == remove_head)
          done = true;
        sns_free(sns_isafe_list_item_get_data(sns_isafe_list_iter_remove(&iter)));
      }
    }
    if(SNS_QCM_COMMIT_UNDO == commit)
    {
      sns_isafe_list_iter_init(&fifo->remove_iter, &fifo->fifo, true);
      sns_isafe_list_iter_return(&fifo->remove_iter);
    }
    else
      fifo->remove_tid = 0;
  }
  sns_osa_lock_release(&fifo->lock);
}

SNS_SECTION(".text.sns") void
sns_qcm_fifo_deinit(sns_qcm_fifo *fifo)
{
  if(!sns_island_is_island_ptr((intptr_t)fifo))
    SNS_ISLAND_EXIT();

  while(NULL != sns_qcm_fifo_remove(fifo, true))
    ;
  sns_qcm_fifo_commit(fifo, SNS_QCM_COMMIT_DONE);
  sns_osa_lock_deinit(&fifo->lock);
  sns_free(fifo);
}
