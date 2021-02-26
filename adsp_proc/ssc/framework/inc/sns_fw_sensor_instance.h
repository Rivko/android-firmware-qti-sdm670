#pragma once
/*=============================================================================
  @file sns_fw_sensor_instance.h

  Sensor Instance state only available to the Framework.

  Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include "sns_cstruct_extn.h"
#include "sns_diag_print.h"
#include "sns_fw_diag_types.h"
#include "sns_isafe_list.h"
#include "sns_island_util.h"
#include "sns_osa_lock.h"
#include "sns_printf.h"
#include "sns_sensor_instance.h"

/*=============================================================================
  Forward Declarations
  ===========================================================================*/

struct sns_fw_sensor;

/*=============================================================================
  Type Definitions
  ===========================================================================*/

/* A list of clients for a specific Sensor */
typedef struct sns_client_req_list
{
  /* Sensor requested by client_requests */
  sns_sensor_uid const *suid;
  /* Clients for this Instance, specific to this SUID; see sns_fw_request */
  sns_isafe_list client_requests;
  /* Add to sns_fw_sensor_instance::client_req_lists
   *
   * Each request contains the associated data stream on which the request
   * was received, and on which all outgoing events will be sent. */
  sns_isafe_list_item list_entry;
  /* Iterator used by Sensor within sns_sensor_instance_cb::get_client_request */
  sns_isafe_list_iter iter;
} sns_client_req_list;

typedef struct sns_fw_sensor_instance
{
  sns_sensor_instance instance;

  /**
   * A list of SUIDs this Instance is presently supporting.  For each, a list
   * of client requests serviced by this Instance.  For most Sensors this list
   * will contain a single entry, where that entry contains a list of all
   * client requests.  For some Sensors (likely Drivers), this list will
   * contain an entry per associated Sensor.
   *
   * The configuration of this Sensor Instance may be a logical combination
   * these ClientRequests.
   *
   * @see sns_client_req_list
   */
  sns_isafe_list client_req_lists;

  /* Reference to the associated Sensor */
  struct sns_fw_sensor *sensor;

  /* Used by Sensor to add to sns_sensor::instances */
  sns_isafe_list_item list_entry;

  /* The Sensor has called sns_event_service_api::alloc_event, but not
   * sns_event_service_api::publish_event. */
  struct sns_fw_sensor_event *unpublished_event;

  /* All active data streams incoming to this Sensor Instance. */
  sns_isafe_list data_streams;

  /* Used by framework to determine if Sensor instance can run in Island mode.
     When set to SNS_ISLAND_STATE_IN_ISLAND, instance will be operated in island mode.
     When set to SNS_ISLAND_STATE_NOT_IN_ISLAND, instance will never be operated in island mode.
     When set to SNS_ISLAND_STATE_ISLAND_DISABLED, instance will not be operated in island mode but
      can be set to SNS_ISLAND_STATE_IN_ISLAND at a future time when all data streams associated with
      instance is available in island mode. */
  sns_island_state island_operation;

  /* State of removal process. Once removal process is started,
     * no further API function calls will be made . */
  enum 
  {
    /* Instance is active */
    SNS_INST_REMOVING_NOT_START = 0,
    /* Removal process started */
    SNS_INST_REMOVING_START     = 1,
    /* Removal process started and waiting all data streaming to complete deinitialization */
    SNS_INST_REMOVING_WAITING   = 2,
  } inst_removing;

  /* Diag configuration */
  sns_diag_src_instance diag_header;

  /* C structure extentions */
  sns_cstruct_extn extn;
} sns_fw_sensor_instance;

/*=============================================================================
  Public Function Declarations
  ===========================================================================*/

/* See sns_sensor_cb::create_instance
 *
 * @note sensor::library::lock must be acquired prior to calling this function.
 * This will already be the case if init_sensor_instance is called within a
 * Sensor API function.
 */
sns_sensor_instance* sns_sensor_instance_init(sns_sensor *instance,
    uint32_t state_len);

/**
 * Deinit a Sensor Instance; begins the process of removing the Instance.
 * Clean-up all requests, incoming and outgoing data streams.
 *
 * @note If function returns false, sns_sensor_instance_destroy will be
 * called once all incoming data streams have been closed.
 *
 * @note instance::sensor::library::state_lock must be
 * acquired prior to calling this function.
 *
 * @param[i] instance Sensor Instance to be removed
 */
void sns_sensor_instance_deinit(sns_sensor_instance *instance);

/**
 * Once all incoming data streams to a Sensor Instance have been closed and
 * removed (and hence there are no further pending events), free the instance
 * memory.
 *
 * @param[i] instance Sensor Instance to be removed
 */
void sns_sensor_instance_delete(sns_fw_sensor_instance *instance);

/**
 * Initialize static state in the sns_fw_sensor_instance module.
 *
 * @return
 * SNS_RC_FAILED - Catastrophic error has occurred; restart Framework
 * SNS_RC_SUCCESS
 */
sns_rc sns_sensor_instance_init_fw(void);

/**
 * Returns number of client requests in
 * instance::client_requests for a particular SUID.
 *
 * @note The sensor_instance->lock must be held while caling this function.
 *
 * @param[i] fw_instance reference to the Sensor Instance
 * @param[i] sensor_uid Sensor UID for client request; or NULL for all
 *
 * @return Client count
 */
int32_t sns_instance_client_cnt(sns_fw_sensor_instance *instance,
    sns_sensor_uid const *sensor_uid);

/**
 * Attempt to set the island mode of a Sensor Instance.  Specifying
 * disabled will always succeed; enabled may fail due to other state
 * constraints.
 *
 * @param[i] instance Instance to be set
 * @param[i] enabled Attempt to mark as in Island Mode;
 *                   Mark as temporarily unable to support island mode
 *
 * @return True upon successful set; false otherwise
 */
bool sns_sensor_instance_set_island(sns_fw_sensor_instance *instance,
    bool enabled);
