#pragma once
/*=============================================================================
  @file sns_sensor_util_internal.h

  Internal utility functions for use by Qualcomm Sensor Developers.

  @note: All utilities in this file can be used in island mode.

  Copyright (c) 2017 - 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

typedef struct
{
  sns_sensor *sensor;
  int32_t attr_id;
  int32_t attr_value;
}sns_sensor_util_attrib;

/*
 * Consolidate the standard request from among all clients of the instance
 * Find minimum batch period ,maximum flush period, max batch flag and flush only flag
 *
 * @param[i] instance The sensor instance
 * @param[i] suid The suid for which clients are being evaluated
 * @param[o] std_req The standard request to be modified
 *
 * @return sns_rc SNS_RC_SUCCESS  if consolidated request was successfully determined
 *                SNS_RC_FAILED   if there was an error in decoding the requests
 *                SNS_RC_INVALID_STATE  if the instance for this suid has no clients
 */
sns_rc
sns_sensor_util_find_consolidated_req(sns_sensor_instance *instance,
                                      sns_sensor_uid const *suid,
                                      sns_std_request* std_req);

/**
* Update batching requests to sensor instances. This function is for sensors
* using sns_std_request and sns_std_sensor_config, and it should be called
* when removing/adding new sensor instances. It will call the curr_inst's set_client_config
* function using the request with the smallest batch and flush period among all
* the requests of the instance
*
* @param[i] this: Sensor whose instances are to be updated
* @param[i] curr_inst: A instance of the current sensor
*
* @return sns_rc SNS_RC_SUCCESS if instance is successfully updated with new
*                               batching parameters
*                SNS_RC_FAILED  if decode or encode fails
*                SNS_RC_INVALID_VALUE if no request in the instance request queue
*/
sns_rc
sns_sensor_util_update_std_sensor_batching(
  sns_sensor *const this,
  sns_sensor_instance *curr_inst);

/**
* Compare if new request can reuse existing instance. If a instance has the same
* sample_rate, same flag for flush_only and max_batch, it can be reused.
*
* @param[i] inst_config The instance configuration
* @param[i] new_req The new request received
* @param[i] exist_req The existing request being serviced by the instance
*
* @return true new request can be serviced by the existing instance
*/
bool sns_sensor_util_instance_match(float const *sample_rate,
  struct sns_request const *new_req, struct sns_request const *exist_req);

/**
* Decode sensor attribute value, currently only support int. 
*
* @param[i] stream The stream to decode attribute value from
* @param[i] field  The format of the attribute value
* @param[i] arg    The argument defined by caller
*
* @return True if decoded successfully
*/
bool sns_sensor_util_decode_attr_value(pb_istream_t *stream,
    const pb_field_t *field, void **arg);

/**
* Decode sensor attribute. 
*
* @param[i] stream The stream to decode attribute value from
* @param[i] field  The format of the attribute value
* @param[i] arg    The argument defined by caller
*
* @return True if decoded successfully
*/
bool sns_sensor_util_decode_attr(pb_istream_t *stream,
    const pb_field_t *field, void **arg);

