#pragma once
/*=============================================================================
  @file sns_suid_sensor.h

  The Framework Sensor manages all SUID and attribute requests.  It is
  technically part of the Framework, but it modelled separately in order
  to avoid special cases within the event-passing code.

  Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include "sns_isafe_list.h"
#include "sns_suid.pb.h"

/*=============================================================================
  Type Definitions
  ===========================================================================*/

/**
 * A SUID Sensor Instance services all requests for a single client.  An entry
 * per data type will be stored in the following format within the Instance.
 */
typedef struct sns_suid_lookup_request
{
  sns_isafe_list_item list_entry;

  /* Register for updates to the list of SUIDs advertising data_type */
  bool register_updates;
  /* request is only for default SUID */
  bool default_only;
  /* Length of data_type array */
  uint32_t data_type_len;
  /* Client-specified character string data type to look-up;
   * Will always be null-terminated. */
  char data_type[];
} sns_suid_lookup_request;

typedef struct sns_suid_sensor_instance_config
{
  /* List of active, registered requests from a particular client */
  sns_isafe_list requests;
} sns_suid_sensor_instance_config;

typedef struct sns_suid_sensor_state
{
  struct sns_data_stream *reg_stream;
  /* Default sensor attributes, as retrieved from registry (sns_def_sensor_info) */
  sns_isafe_list def_sensor_list;
} sns_suid_sensor_state;

/*=============================================================================
  Public Function Declarations
  ===========================================================================*/

/**
 * The Attribute Service shall monitor for when a sensor advertises "Available"
 * when it was not so previously.  At that time, it shall call this function.
 */
void sns_suid_sensor_apprise(char const *data_type);
