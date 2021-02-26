/*============================================================================
  @file sns_island_hexagon_mdsp.c

  @brief
  Island control for Sensors of SEE on MDSP.

  Copyright (c) 2017 - 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  ============================================================================*/

/*============================================================================
  INCLUDES
  ============================================================================*/

#include "sns_island.h"
#include "sns_island_service.h"
#include "sns_island_util.h"
#include "sns_sensor_instance.h"
#include "sns_sensor_uid.h"
#include "sns_types.h"

/*============================================================================
  Functions
  ============================================================================*/

static sns_island_service island_service;

sns_rc sns_island_init()
{
  return SNS_RC_SUCCESS;
}

sns_rc sns_island_disable()
{
  return SNS_RC_SUCCESS;
}

sns_rc sns_island_enable()
{
  return SNS_RC_SUCCESS;
}

sns_rc sns_island_exit_internal()
{
  return SNS_RC_NOT_SUPPORTED;
}

sns_rc sns_island_generate_and_commit_state_log( uint64_t req_id )
{
  UNUSED_VAR(req_id);
  return SNS_RC_NOT_SUPPORTED;
}

sns_rc sns_island_register_status_signal(
  sns_osa_thread *thread,
  sns_osa_thread_sigs signal)
{
  UNUSED_VAR(thread);
  UNUSED_VAR(signal);
  return SNS_RC_NOT_SUPPORTED;
}

sns_island_state sns_island_get_island_state()
{
  return SNS_ISLAND_STATE_ISLAND_DISABLED;
}

sns_rc sns_island_log_trace(
  uint64_t user_defined_id )
{
  UNUSED_VAR(user_defined_id);
  return SNS_RC_NOT_SUPPORTED;
}

bool sns_island_is_island_ptr(intptr_t ptr)
{
  UNUSED_VAR(ptr);
  return false;
}

bool sns_instance_streams_in_island(sns_sensor_instance *instance)
{
  UNUSED_VAR(instance);
  return false;
}

sns_island_client_handle sns_island_aggregator_register_client( const char * client_name)
{
  UNUSED_VAR(client_name);
  return NULL;
}

void sns_island_aggregator_deregister_client(sns_island_client_handle client_handle)
{
  UNUSED_VAR(client_handle);
}

sns_rc sns_island_block(sns_island_client_handle client_handle)
{
  UNUSED_VAR(client_handle);
  return SNS_RC_NOT_SUPPORTED;
}

sns_rc sns_island_unblock(sns_island_client_handle client_handle)
{
  UNUSED_VAR(client_handle);
  return SNS_RC_NOT_SUPPORTED;
}

sns_rc sensor_island_exit(sns_island_service *this,
                          struct sns_sensor const* sensor)
{
  UNUSED_VAR(this);
  UNUSED_VAR(sensor);
  return SNS_RC_FAILED;
}

sns_rc sensor_instance_island_exit(sns_island_service *this,
                                   struct sns_sensor_instance const* instance)
{
  UNUSED_VAR(this);
  UNUSED_VAR(instance);
  return SNS_RC_FAILED;
}

sns_rc island_log_trace(sns_island_service *this,
                        uint64_t user_defined_id)
{
  UNUSED_VAR(this);
  UNUSED_VAR(user_defined_id);
  return SNS_RC_FAILED;
}

/**
 * Island Service API.
 */
static sns_island_service_api  island_service_api =
{
  .struct_len = sizeof(sns_island_service_api),
  .sensor_island_exit = &sensor_island_exit,
  .sensor_instance_island_exit = &sensor_instance_island_exit,
  .island_log_trace = &island_log_trace
};

sns_service* sns_island_service_init(void)
{
  /** Island util must be initialized before island service init */
  island_service.api = &island_service_api;
  island_service.service.type = SNS_ISLAND_SERVICE;
  island_service.service.version = 1;
  return &island_service.service;
}

bool sns_island_enable()
{
  //DO NOTHING
  return false;
}

