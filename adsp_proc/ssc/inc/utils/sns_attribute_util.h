#pragma once
/*=============================================================================
  @file sns_attribute_util.h

  Utility functions provided by Qualcomm for use by Sensors to handle and
  publish attributes.

  @note: All utilities in this file can be used in island mode.

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include "pb_decode.h"
#include "sns_std.pb.h"
#include "sns_std_sensor.pb.h"

/*=============================================================================
  Macros
  ===========================================================================*/

/* Macro to shorten the default initializer */
#define SNS_ATTR sns_std_attr_value_data_init_default
#define SNS_DEFAULT_MIN_RATE (5.0f)
#define SNS_DEFAULT_MAX_RATE (200.0f)

/*=============================================================================
  Forward Declarations
  ===========================================================================*/

struct sns_sensor;

/*=============================================================================
  Public Function Declarations
  ===========================================================================*/

/**
 * Encode an array of attribute values into an attribute.  This function is
 * intended to be used when publishing attributes.
 *
 * @param[i] arg Type pb_buffer_arg (.buf = array, .buf_len = array length)
 *
 * @see pb_callback_s::encode
 */
bool sns_pb_encode_attr_cb(pb_ostream_t *stream, const pb_field_t *field,
    void *const *arg);

/**
 * Publish a simple attribute using the Attribute Service.
 *
 * @param[i] sensor Whose attributes to publish
 * @param[i] attribute_id  ID corresponding to attribute
 * @param[i] values Array of decoded attribute values
 * @param[i] values_len Number of entries in values array
 * @param[i] completed Whether this is the last attribute in this publish set
 */
void sns_publish_attribute(struct sns_sensor *const sensor,
    uint32_t attribute_id, struct _sns_std_attr_value_data const *values,
    uint32_t values_len, bool completed);
