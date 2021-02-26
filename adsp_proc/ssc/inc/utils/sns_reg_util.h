#pragma once
/*=============================================================================
  @file sns_reg_util.h

  A simple utility to parse incoming registry data.  Intended for use by
  Sensors; is a replacement for/wrapper around decoding registry events
  manually by the Sensor directly.

  The Sensor developer should create a registry data stream normally.
  Within notify_event, sns_reg_event_handle should be called each time.

  Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include <stdbool.h>
#include <stdint.h>
#include "sns_rc.h"
#include "sns_types.h"

/*=============================================================================
  Forward Declarations
  ===========================================================================*/

struct sns_data_stream;

/*=============================================================================
  Type Definitions
  ===========================================================================*/

/* Type of registry data to be parsed; positive numbers indicate the string
 * buffer length. */
typedef enum
{
  SNS_REG_TYPE_INT = -1,
  SNS_REG_TYPE_FLT = -2,
} sns_reg_type;

/* Registry Item */
typedef struct sns_reg_util_table_item
{
  char const *name;
  /* Item type; Positive values refer to string length of data*/
  sns_reg_type type;
  /* Where to copy the registry item value */
  void *data;
} sns_reg_util_table_item;

/* Registry Group */
typedef struct sns_reg_util_table
{
  /* Full name of this registry group;
   * If empty, will match any registry group */
  char const *name;
  /* Length of items array */
  uint32_t items_len;
  /* One entry per registry item for processing */
  sns_reg_util_table_item *items;
  /* True once this registry group has been received and parsed */
  bool group_recv;
} sns_reg_util_table;

/*=============================================================================
  Function Declarations
  ===========================================================================*/

/**
 * Handle all pending events on the registry data stream.  Must be called within
 * the Sensor's notify_event function.
 *
 * @param[i] stream Open registry data stream
 * @param[i] tables Client-provided tables to store decoded registry items
 * @param[i] tables_len Length of the tables array
 *
 * @return
 * SNS_RC_SUCCESS - One or more registry groups were successfully parsed
 * SNS_RC_NOT_AVAILABLE - Registry stream inactive or no pending events
 * SNS_RC_INVALID_STATE - Received error event from registry;
 *                        Implies that registry data will be unavailable
 */
sns_rc sns_reg_util_handle(struct sns_data_stream *stream,
    sns_reg_util_table *tables, uint32_t tables_len);
