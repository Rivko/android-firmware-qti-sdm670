/**
 * sns_pwr_rail_manager.c
 *
 * The Power Rail Management Utility implementation for x86 simulation
 *
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * $Id: //components/rel/ssc.slpi/3.2/utils/pwr_rail_mgr/x86/sns_pwr_rail_service_x86.c#1 $
 * $DateTime: 2018/02/01 03:15:18 $
 * $Change: 15367748 $
 *
 **/

#include <stdint.h>
#include "sns_fw_pwr_rail_service.h"
#include "sns_pwr_rail_service.h"
#include "sns_time.h"
#include "sns_types.h"

/** Version:
 */
static const uint16_t sns_pwr_rail_service_version = 1;

/**
 * Private state definition.
 */
struct sns_fw_pwr_rail_service
{
  sns_pwr_rail_service service;
} ;

/**
 * Private state of the Power Rail Manager Service.
 */
static sns_fw_pwr_rail_service pwr_rail_service;

/**
 * See sns_pwr_rail_service.h
 */
sns_rc sns_vote_power_rail_update(sns_pwr_rail_service *this,
                                  struct sns_sensor const* sensor,
                                  sns_rail_config const* rails_config,
                                  sns_time* rail_on_timestamp)
{
  UNUSED_VAR(this);
  UNUSED_VAR(sensor);
  UNUSED_VAR(rails_config);
  if(rail_on_timestamp)
  {
    *rail_on_timestamp = sns_get_system_time();
  }

  return SNS_RC_SUCCESS;
}

/**
 * See sns_pwr_rail_service.h
 */
sns_rc sns_register_power_rails(sns_pwr_rail_service *this,
                                sns_rail_config const* rail_config)
{
  UNUSED_VAR(this);
  UNUSED_VAR(rail_config);
  return SNS_RC_SUCCESS;
}

/**
 * Power rail manager API.
 */
static sns_pwr_rail_service_api pwr_rail_service_api =
{
  .struct_len = sizeof(sns_pwr_rail_service_api),
  .sns_register_power_rails = &sns_register_power_rails,
  .sns_vote_power_rail_update = &sns_vote_power_rail_update
};

/**
 * See sns_fw_pwr_rail_service.h
 */
sns_fw_pwr_rail_service* sns_pwr_rail_service_init(void)
{
  pwr_rail_service.service.api = &pwr_rail_service_api;
  pwr_rail_service.service.service.type = SNS_POWER_RAIL_SERVICE;
  pwr_rail_service.service.service.version = sns_pwr_rail_service_version;

  return &pwr_rail_service;
}

