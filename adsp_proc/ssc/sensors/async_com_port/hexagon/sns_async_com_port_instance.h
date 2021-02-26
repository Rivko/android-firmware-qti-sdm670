#pragma once
/*=============================================================================
  @file sns_async_com_port_instance.h

  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/
/*=============================================================================
  Include Files
  ===========================================================================*/
#include "sns_async_com_port.h"
#include "sns_isafe_list.h"
#include "sns_signal.h"

/*=============================================================================
  Type definitions
  ===========================================================================*/

typedef struct
{
  sns_isafe_list_item list_entry;
  uint32_t num_vectors;
  sns_port_vector *vectors;
}sns_async_com_port_queued_transaction;

typedef struct
{
  /* A unique number used by the client to identify this port */
  uint32_t port_handle;

  /* Port handle used by the com port utilities */
  void  *priv_handle;

  /* Port configuration for this instance */
  sns_com_port_config config;

  /* Number of bytes actually transferred over the com port. This is populated
   * in the callback from the ascp utilities
   */
  uint32_t xfer_size;

  /* Error status for the transfer. This is populated in the callback from the
   * ascp utility
   */
  sns_rc rw_err;

  /* The current transaction that is being executed */
  sns_async_com_port_queued_transaction *current_tx;

  /* Previous vector handled - for debug*/
  sns_port_vector pre_vector;

  /* Timestamp of the last transfer over the com port. This is populated in the
   * callback from the ascp utilities
   */
  sns_time last_rw_timestamp;

  /* Timestamp of the last com port event that was sent out.*/
  sns_time last_rw_event_timestamp;

  /* Thread handle provided by the signal utility. */
  sns_signal_thread *signal_thread;

  /* Signal flag assigned by the signal utility */
  uint32_t signal_flag;

  /* Queue of transactions pending for this instance. The head of this queue is the
   * next transaction to be executed.
   */
  sns_isafe_list pending_transactions;

} sns_async_com_port_instance_state;
