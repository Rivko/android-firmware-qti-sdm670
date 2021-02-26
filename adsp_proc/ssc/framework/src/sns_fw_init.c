/*=============================================================================
  @file sns_fw_init.c

  Single entry point to the Sensors Framework.  Initializes all other core
  Framework modules, and all known services.

  Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include "sns_assert.h"
#include "sns_island.h"
#include "sns_printf_int.h"
#include "sns_fw_attribute_service.h"
#include "sns_fw_event_service.h"
#include "sns_fw_diag_service.h"
#include "sns_fw_sensor.h"
#include "sns_fw_sensor_instance.h"
#include "sns_fw_service_manager.h"
#include "sns_island.h"
#include "sns_island_util.h"
#include "sns_memmgr.h"
#include "sns_printf_int.h"
#include "sns_pwr_scpm_mgr.h"
#include "sns_pwr_sleep_mgr.h"
#include "sns_rc.h"
#include "sns_signal.h"
#include "sns_thread_manager.h"

/*=============================================================================
  External Data Declarations
  ===========================================================================*/

extern const sns_register_entry sns_register_sensor_list[];
extern const uint32_t sns_register_sensor_list_len;
extern sns_sensor_cb sensor_cb;

/*=============================================================================
  Static Function Definitions
  ===========================================================================*/

/**
 * Load and initialize all Sensors statically-compiled with the SLPI image.
 * Dynamically loaded Sensors will be loaded later.
 */
static sns_rc
register_static_sensors(void)
{
  sns_register_cb reg_cb = (sns_register_cb)
  {
    .struct_len = sizeof(reg_cb),
    .init_sensor = &sns_sensor_init
  };

  for(int i = 0; i < sns_register_sensor_list_len; i++)
  {
    SNS_PRINTF(LOW, sns_fw_printf, "Register Sensor from static list %i",
        sns_register_sensor_list[i].cnt);
    for(int j = 0; j < sns_register_sensor_list[i].cnt; j++)
    {
      sns_isafe_list_iter iter;
      sns_sensor_library *library =
        sns_sensor_library_init(sns_register_sensor_list[i].func, j);

      sns_osa_lock_acquire(library->library_lock);
      sns_register_sensor_list[i].func(&reg_cb);
      library->removing = SNS_LIBRARY_ACTIVE;
      sns_sensor_library_start(library);

      sns_isafe_list_iter_init(&iter, &library->sensors, true);
      if(0 == sns_isafe_list_iter_len(&iter))
      {
        sns_sensor_library_deinit(library);
        sns_osa_lock_release(library->library_lock);
        sns_sensor_library_delete(library);
      }
      else
        sns_osa_lock_release(library->library_lock);
    }
  }

  return SNS_RC_SUCCESS;
}

/*=============================================================================
  Public Function Declarations
  ===========================================================================*/

/**
 * Initialize the Sensors Framework; all state, threads, and sensors.
 *
 * @return -1 upon error
 */
int
sns_fw_init(void)
{
  sns_rc rc;
  SNS_PRINTF(LOW, sns_fw_printf, "Init started");
  rc = sns_island_init();
  SNS_ASSERT(SNS_RC_SUCCESS == rc);
  rc = sns_thread_manager_init();
  SNS_ASSERT(SNS_RC_SUCCESS == rc);
  rc = sns_sensor_init_fw();
  SNS_ASSERT(SNS_RC_SUCCESS == rc);
  rc = sns_pwr_sleep_mgr_init();
  SNS_ASSERT(SNS_RC_SUCCESS == rc);
  rc = sns_pwr_scpm_mgr_init();
  SNS_ASSERT(SNS_RC_SUCCESS == rc);

  rc = sns_service_manager_init();
  SNS_ASSERT(SNS_RC_SUCCESS == rc);

  rc = sns_sensor_instance_init_fw();
  SNS_ASSERT(SNS_RC_SUCCESS == rc);
  rc = register_static_sensors();
  SNS_ASSERT(SNS_RC_SUCCESS == rc);
  SNS_PRINTF(LOW, sns_fw_printf, "Init Finished");

  // Re-enable the thread manager; disabled by default in sns_thread_manager_init
  sns_thread_manager_disable(false);

  return 0;
}
