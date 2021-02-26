#pragma once
/**
 * @file sns_lsm6ds3_dae_if.h
 *
 * DAE sensor interface
 *
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 **/

#include <stdint.h>
#include "sns_data_stream.h"
#include "sns_sensor_instance.h"
#include "sns_stream_service.h"

struct sns_stream_service;
struct sns_data_stream;
struct lsm6ds3_instance_state;

typedef enum
{
  IDLE,
  STREAM_STARTING,
  STREAMING,
  STREAM_STOPPING,

} lsm6ds3_dae_if_state;

typedef struct
{
  struct sns_data_stream *stream;
  const char             *nano_hal_vtable_name;
  lsm6ds3_dae_if_state   state;
  bool                   stream_usable:1;
  bool                   flushing_hw:1;
  bool                   flushing_data:1;
} lsm6ds3_dae_stream;

typedef struct lsm6ds3_dae_if_info
{
  lsm6ds3_dae_stream   ag;
  lsm6ds3_dae_stream   temp;
} lsm6ds3_dae_if_info;

bool lsm6ds3_dae_if_available(sns_sensor_instance *this);

sns_rc lsm6ds3_dae_if_init(
  sns_sensor_instance  *const this,
  struct sns_stream_service  *stream_mgr,
  sns_sensor_uid             *dae_suid);

void lsm6ds3_dae_if_deinit(
  struct lsm6ds3_instance_state *state,
  struct sns_stream_service     *stream_mgr);

bool lsm6ds3_dae_if_stop_streaming(sns_sensor_instance *this);

bool lsm6ds3_dae_if_start_streaming(sns_sensor_instance *this);

bool lsm6ds3_dae_if_flush_hw(sns_sensor_instance *this);

bool lsm6ds3_dae_if_flush_samples(sns_sensor_instance *this);

void lsm6ds3_dae_if_process_events(sns_sensor_instance *this);

