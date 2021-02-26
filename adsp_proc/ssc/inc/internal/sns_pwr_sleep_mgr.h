#pragma once
/** ======================================================================================
  @file sns_pwr_sleep_mgr.h

  @brief Sensors Power Sleep Manager

  Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Id: //components/rel/ssc.slpi/3.2/inc/internal/sns_pwr_sleep_mgr.h#2 $
  $DateTime: 2018/05/24 18:16:33 $
  $Change: 16250947 $
====================================================================================== **/


/**
*****************************************************************************************
                               Includes
*****************************************************************************************
*/
#include <stdbool.h>
#include <stdint.h>
#include "sns_rc.h"
#include "sns_sensor_event.h"
#include "sns_sensor_instance.h"
/**
*****************************************************************************************
                                  Constants & Macros
*****************************************************************************************
*/
#define USEC_PER_SEC 1000000

/* For motion gated usecases, need to specify a large wakeup interval to the sleep driver
 * to differentiate from the no sensor client usecase
 */
#define SNS_PWR_SLEEP_MGR_MAX_WAKEUP_INTVL_USEC 1000000

/* During island mode operation, enable/disable sleep modes in island mode based on
 * wakeup rate in island mode. Threshold set based on sleep driver latency 
 */
#define SNS_PWR_SLEEP_MGR_ISLAND_SLEEP_MODE_THRESHOLD_HZ 400
/**
*****************************************************************************************
                               Forward Declarations
*****************************************************************************************
*/

struct sns_fw_sensor_instance;

/**
*****************************************************************************************
                                  Public Functions
*****************************************************************************************
*/

/**
 * Initialize the sensors power sleep manager.
 * Create handles to the sleep driver NPA nodes.
 *
 * @return
 * SNS_RC_SUCCESS
 */
sns_rc
sns_pwr_sleep_mgr_init(void);

/**
 * Check for physical sensor config events. For physical sensor config
 * events add instance to the power sleep manager instance list
 *
 * @param[i] instance              sensor instance
 * @param[i] event                 sensor event
 * @param[i] is_physical_sensor    true if physical sensor, false otherwise
 *
 * @return
 * SNS_RC_SUCCESS - operation successful
 * SNS_RC_FAILED  - event decode failed
 */
sns_rc
sns_pwr_sleep_mgr_check_config_event(sns_sensor_instance *instance,
    sns_sensor_event const *event, bool is_physical_sensor);

/**
 * On deinitilization of a sensor instance, remove the instance from the sleep
 * power manager instance list. Remove the instance wakeup rate from the aggregated
 * Q6 wakeup rate. Update the sleep driver with the new Q6 wakeup rate
 *
 * @param[i] instance   sensor instance
 */
void
sns_pwr_sleep_mgr_sensor_instance_deinit(struct sns_fw_sensor_instance *instance);

/**
 * Set normal mode sleep latency. 
 * Configure how fast the system must respond to a normal mode wakeup interrupt 
 *
 * @param[i] latency_us   Allowed latency in microseconds
 */
void
sns_pwr_sleep_mgr_set_latency(uint32_t latency_us);

/**
 * Unset normal mode sleep latency. 
 */
void
sns_pwr_sleep_mgr_release_latency();

/**
 * Set/reset island mode enabled flag in power sleep manager*
 *
 * @param[i] island_enabled   True if island is enabled, False otherwise
 */
void
sns_pwr_sleep_mgr_set_island_mode_enabled(bool island_enabled);
