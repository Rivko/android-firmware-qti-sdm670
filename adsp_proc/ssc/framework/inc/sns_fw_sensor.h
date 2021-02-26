#pragma once
/*=============================================================================
  @file sns_fw_sensor.h

  Sensor state and functions only available within the Framework.

  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/
#include "sns_cstruct_extn.h"
#include "sns_fw_diag_types.h"
#include "sns_isafe_list.h"
#include "sns_island_util.h"
#include "sns_osa_lock.h"
#include "sns_register.h"
#include "sns_sensor.h"

/*=============================================================================
  Forward Declarations
  ===========================================================================*/

struct sns_sensor_library;

/*=============================================================================
  Type Definitions
  ===========================================================================*/

/**
 * Sensor state used by the Framework; not accessible to Sensor developers.
 *
 * All state is protected by library_lock.
 */
typedef struct sns_fw_sensor
{
  sns_sensor sensor;

  /* All active data streams incoming to this Sensor. */
  sns_isafe_list data_streams;

  /* List of active Instances for this Sensor  */
  sns_isafe_list sensor_instances;
  /* Iterator used by Sensors using sns_sensor_cb::get_sensor_instance */
  sns_isafe_list_iter instances_iter;

  /* Kept on sns_sensor_library::sensors */
  sns_isafe_list_item list_entry;

  /* Library containing this Sensor */
  struct sns_sensor_library *library;

  /* Whether the Sensor supports island mode operation */
  sns_island_state island_operation;

  /* Set to true if this Sensor is in the process of being removed */
  bool removing;

  /* Pointer to debug configuration for the specific sensor data type within
     the diag service.  sns_diag_lock must be acquired prior to accessing
     this field. */
  sns_diag_src_config diag_config;

  /* Sensor state managed by the attribute service */
  struct sns_attribute_info *attr_info;

  /* Manage buffers appended to this struct */
  sns_cstruct_extn extn;
} sns_fw_sensor;

/**
 * A statically or dynamically loaded library which contains one or more
 * Sensors.  Practically, there will be a object of this type per each
 * sns_register_sensors() function call.
 */
typedef struct sns_sensor_library
{
  /* Kept on sns_sensor.c::libraries */
  sns_isafe_list_item list_entry;

  /* Protects the list of sensors; Protects the state of associated Sensors; this mutex must be acquired
   * prior to calling or accessing any Sensor in this library. */
  sns_osa_lock *library_lock;

  /* List of Sensors contained within this library */
  sns_isafe_list sensors;
  /* Iterator used by Sensors using sns_sensor_cb::get_library_sensor */
  sns_isafe_list_iter sensors_iter;

  /* Registration index of this copy of the library */
  uint32_t registration_index;

  /* Protected by library_lock */
  enum {
    /* Library sensors are being initialized */
    SNS_LIBRARY_INIT = 0,
    /* All initialization has completed */
    SNS_LIBRARY_ACTIVE = 1,
    /* Library is waiting for all Sensors to complete deinitialization */
    SNS_LIBRARY_REMOVE = 2,
  } removing;

  /* Registration function associated with this library */
  sns_register_sensors register_func;
} sns_sensor_library;

/* Callback function used in sns_sensor_foreach() */
typedef bool (*sns_sensor_foreach_func)(sns_fw_sensor *sensor, void *arg);

/*=============================================================================
  Public Function Declarations
  ===========================================================================*/

/* See sns_register_cb::init_sensor */
sns_rc sns_sensor_init(uint32_t state_len, struct sns_sensor_api const *sensor_api,
                       struct sns_sensor_instance_api const *instance_api);

/**
 * Deinitialize and free a Sensor and all associated state.  Recursively
 * deinit all active Sensor Instances and their associated state.
 * Remove this Sensor from the associated Library's list.
 *
 * @note library::state_lock and library::sensor_lock must be held
 *
 * @param[i] sensor
 *
 * @return
 * SNS_RC_SUCCESS
 */
sns_rc sns_sensor_deinit(sns_fw_sensor *sensor);

/**
 * Check whether a Sensor is ready to be removed/deleted, and do so now
 * if possible.
 *
 * @return true if deleted; false otherwise
 */
bool sns_sensor_delete(sns_fw_sensor *sensor);

/**
 * Delete a Sensor Library if it is being removed and no longer contains
 * any Sensors.
 *
 * @return True if library was freed; false otherwise
 */
bool sns_sensor_library_delete(sns_sensor_library *library);

/**
 * Allocation, initialize, and add a new library object to the libraries
 * list.
 *
 * @param[i] register_func Registration function entry point into the library
 * @param[i] registration_index How many copies of this library to register
 *
 * @return Newly allocated library reference; NULL if OOM
 */
sns_sensor_library* sns_sensor_library_init(
    sns_register_sensors register_func, uint32_t registration_index);

/**
 * Start all Sensors contained by the library.
 */
void sns_sensor_library_start(sns_sensor_library *library);

/**
 * Deinitialize and free a Library and all associated state.  Recursively
 * deinit all active Sensor and Sensor Instances.
 * Remove this Library from the global list of active libraries.
 *
 * @param[i] library
 *
 * @return
 * SNS_RC_SUCCESS
 */
sns_rc sns_sensor_library_deinit(sns_sensor_library *library);

/**
 * Initialize static state within this file.
 *
 * @return
 * SNS_RC_FAILED - Catastrophic error has occurred; restart Framework
 * SNS_RC_SUCCESS
 */
sns_rc sns_sensor_init_fw(void);

/**
 * For each sensor on the system, call func with arg.
 *
 * @note This function cannot be called while holding *any* library_lock.
 *
 * @param[i] func Function called for each known Sensor
 * @param[i] arg User-specified argument to be used in func callback
 *
 * @return False if func() returned false for a given sensor input
 */
bool sns_sensor_foreach(sns_sensor_foreach_func func, void *arg);

/**
 * Attempt to set the island mode of a Sensor.  Specifying
 * disabled will always succeed; enabled may fail due to other state
 * constraints.
 *
 * @param[i] sensor Sensor to be set
 * @param[i] enabled Attempt to mark as in Island Mode;
 *                   Mark as temporarily unable to support island mode
 *
 * @return True upon successful set; false otherwise
 */
bool sns_sensor_set_island(sns_fw_sensor *sensor, bool enabled);
