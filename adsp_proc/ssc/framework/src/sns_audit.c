/*=============================================================================
  @file sns_audit.c

  Periodically the SEE should perform an audit of its data structures,
  verifying all invariants.  This file contains functions checking all known
  invariants.

  Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include "sns_assert.h"
#include "sns_fw_sensor.h"
#include "sns_fw_sensor_instance.h"
#include "sns_island.h"
#include "sns_printf_int.h"
#include "sns_profiler.h"
#include "sns_types.h"

/*=============================================================================
  Public Function Definitions
  ===========================================================================*/

#if 0
/**
 * Confirm that all Sensor Instances have at least one client.
 */
SNS_SECTION(".text.sns") void
sns_audit_clientless_instances(sns_sensor_library *library)
{
  SNS_PROFILE(SNS_STREAM_SERVICE_REMOVE_CLIENTLESS_INSTANCES_START, 0);
  sns_isafe_list_iter iter;

  sns_osa_lock_acquire(library->library_lock);
  for(sns_isafe_list_iter_init(&iter, &library->sensors, true);
      NULL != sns_isafe_list_iter_curr(&iter);
      sns_isafe_list_iter_advance(&iter))
  {
    sns_sensor *sensor = (sns_sensor*)
      sns_isafe_list_item_get_data(sns_isafe_list_iter_curr(&iter));
    sns_sensor_instance *inst;

    for(inst = sensor->cb->get_sensor_instance(sensor, true);
        inst != NULL;
        inst = sensor->cb->get_sensor_instance(sensor, false))
    {
      sns_fw_sensor_instance *fw_inst = (sns_fw_sensor_instance*)inst;
      if(0 == sns_instance_client_cnt(fw_inst, NULL) &&
          (fw_inst->inst_removing == SNS_INST_REMOVING_NOT_START))
      {
        SNS_PRINTF(sns_fw_printf, (HIGH, 1, "Clientless instance " SNS_DIAG_PTR, sensor);
        // PEND: Rather than assert, remove Sensor
        SNS_ASSERT(false);
      }
    }
  }
  sns_osa_lock_release(library->library_lock);
  SNS_PROFILE(SNS_STREAM_SERVICE_REMOVE_CLIENTLESS_INSTANCES_END, 0);
}
#endif
