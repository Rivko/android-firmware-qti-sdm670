#pragma once
/*=============================================================================
  @file sns_qcm_fifo.h

  Specialized FIFO for event storage.  Present implementation uses a linked
  list of event objects allocated from the general heap manager.  A subsequent
  optimization will be to use circular buffers: this will significantly
  decrease per-event and per-fifo overhead.

  PEND: Additional future optimizations:
  - Presently data will accumulate in Island memory until all island memory
  is exhausted.  At which time data will continue to accumulate in DDR.  This
  is acceptable for the present use-case, which should primarily reside in
  Island mode.  Optimization: Events are automatically moved to/from
  island/ddr as necessary.
  - Presently QCM is only used by co-located SSC clients.  Therefore there is
  no benefit to batching data any further than the physical FIFO supports.
  However to support general use cases, upon reaching a DDR low memory
  threshold, a callback should be invoked to QCM which will send all stored
  data.

  Copyright (c) 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/
#include <stdint.h>
#include "sns_time.h"

/*=============================================================================
  Data Type Definitions
  ===========================================================================*/

struct sns_qcm_fifo;
struct sns_sensor_event;

typedef enum sns_qcm_commit_type
{
  /* Free removed events; release hold */
  SNS_QCM_COMMIT_DONE,
  /* Reset remove iterator; keep exclusive hold */
  SNS_QCM_COMMIT_UNDO,
  /* Reset remove iterator; release exclusive hold */
  SNS_QCM_COMMIT_RELEASE,
} sns_qcm_commit_type;

/*=============================================================================
  Function Declarations
  ===========================================================================*/

/**
 * Initialize a new buffer space.  Typically each <client, suid> pair
 * will have its own buffer for efficiency.
 *
 * If *fifo is non-NULL, simply update the flush_period.
 *
 * @param[io] fifo
 * @param[i] flush_period
 */
void sns_qcm_fifo_init(struct sns_qcm_fifo **fifo, sns_time flush_period);

/**
 * Push a new event to the tail of the buffer.
 *
 * Oldest sample may be dropped based on flush_period.
 */
void sns_qcm_fifo_insert(struct sns_qcm_fifo *fifo, struct sns_sensor_event *event);

/**
 * Retrieve the oldest event from the buffer.  Must be subsequently followed
 * by a sns_qcm_fifo_commit: memory is only freed at that time.
 *
 * @param[i] remove Whether to remove this entry off of the fifo;
 *                  Returns the next entry in the FIFO
 */
struct sns_sensor_event* sns_qcm_fifo_remove(struct sns_qcm_fifo *fifo, bool remove);

/**
 * Permanently destroy and free all events removed since the last commit.
 *
 * @param[i] commit Commit type
 */
void sns_qcm_fifo_commit(struct sns_qcm_fifo *fifo, sns_qcm_commit_type commit);

/**
 * Deinitialize the fifo, and clear all events.  Client is responsible for
 * ensuring that all references to this FIFO have first been removed.
 */
void sns_qcm_fifo_deinit(struct sns_qcm_fifo *fifo);
