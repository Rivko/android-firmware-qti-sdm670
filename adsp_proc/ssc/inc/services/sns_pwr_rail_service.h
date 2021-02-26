#pragma once
/**
 * sns_pwr_rail_manager.h
 *
 * Power rail management API.
 *
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * $Id: //components/rel/ssc.slpi/3.2/inc/services/sns_pwr_rail_service.h#1 $
 * $DateTime: 2018/02/01 03:15:18 $
 * $Change: 15367748 $
 *
 **/
#include <stdint.h>
#include <stdlib.h>
#include "sns_rc.h"
#include "sns_service.h"
#include "sns_time.h"

#define RAIL_NAME_STRING_SIZE_MAX 30
#define RAIL_NUMBER_MAX 2

/**  Forward Declarations. */
struct sns_pwr_rail_service_api;
struct sns_sensor;

/**  Power rail states provided by this service. */
typedef enum
{
  /** OFF state.
   *  Must be used when there are no active Sensors Instances.
   */
  SNS_RAIL_OFF,

  /** Low Power Mode.
   *  Must be used as an ON state only by accel sensor drivers.
   */
  SNS_RAIL_ON_LPM,

  /** Normal Power Mode.
   *  This is the ON state used by all other sensors.
   */
  SNS_RAIL_ON_NPM
} sns_power_rail_state;

/**  Rail name definition */
typedef struct sns_rail_name
{
  char   name[RAIL_NAME_STRING_SIZE_MAX];
} sns_rail_name;

/**  Rail configuration definition. */
typedef struct
{
  /** Overall vote for all sensor rails.
   *  At any time, all rails for a sensor can have a single
   *  state from sns_power_rail_state.
   */
  sns_power_rail_state  rail_vote;

  /** Number of rails in sns_rail_name array.
   */
  uint8_t               num_of_rails;

  /** Array of rail names.
   *  These are power rails connected to the sensor HW.
   */ 
  sns_rail_name         rails[RAIL_NUMBER_MAX];

} sns_rail_config;

/** Sensors Power Rail Service type definitions. */
typedef struct sns_pwr_rail_service {
  sns_service service;
  struct sns_pwr_rail_service_api *api;
} sns_pwr_rail_service;

typedef struct sns_pwr_rail_service_api
{
  size_t  struct_len;

  /**
  *  Register power rails for a physical sensor.
  *
  *  All physical sensors typically have one or more power rails
  *  that supply power to the sensor hardware. Power rail
  *  configuration for all sensors shall be defined in the
  *  non-volatile registry. After the sensor driver fetches rail
  *  information from the registry, it shall register all rails
  *  using this API. Rail registration must happen only once for
  *  any rail.
  *  
  *  @param[i] this                  Power rail service reference.
  *  @param[i] rail_config           Rail config being registered.
  *
  *  @return
  *  SNS_RC_INVALID_VALUE - input rail config is invalid
  *  SNS_RC_SUCCESS - rail registration successful
  */
  sns_rc (*sns_register_power_rails)(sns_pwr_rail_service* this,
                                     sns_rail_config const* rail_config);

  /**
  *  Vote for a power rail status change.
  *
  *  Each physical sensor driver uses this API to turn it's
  *  sensors power rails ON when there is an active client
  *  request. It is also the driver's responsibility to vote for
  *  power rails OFF when all clients requests are disabled (i.e.
  *  there are no Sensors Instances present).
  *
  *  @param[i] this               Power rail service reference.
  *  @param[i] sensor             Sensor requesting rail update.
  *  @param[i] rails_config       Rails and their status change
  *                               vote.
  *  @param[o] rail_on_timestamp   Timestamp in ticks when the
  *                               rail was turned ON. When the
  *                               rails are being turned OFF, the
  *                               client can choose not to get
  *                               rail_on_timestamp information.
  *
  *  @return
  *  SNS_RC_NOT_SUPPORTED - requested rail is not registered
  *  SNS_RC_SUCCESS - operation successful
  */
  sns_rc (*sns_vote_power_rail_update)(sns_pwr_rail_service* this,
                                       struct sns_sensor const* sensor,
                                       sns_rail_config const* rails_config,
                                       sns_time* rail_on_timestamp);

} sns_pwr_rail_service_api;

