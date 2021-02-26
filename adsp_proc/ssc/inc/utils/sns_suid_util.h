#pragma once
/*=============================================================================
  @file sns_suid_util.h

  A simple utility to look-up dependent SUIDs.  Intended for use by Sensors;
  is a replacement for/wrapper around sending requests to the SUID Lookup
  Sensor directly.

  The Sensor developer should add SNS_SUID_LOOKUP_DATA as field to their Sensor
  state structure, specifying the number of data types they intend to look-up;
  this should correspond to the number of times they plan to call
  sns_suid_lookup_add().  Number should be chosen precisely: larger than
  necessary will result in wasted memory; if smaller the requested data types
  will not be discovered.

  If the Sensor is expected to choose amongst multiple SUIDs of the same
  data type, they may register a sns_suid_lookup_cb within SNS_SUID_LOOKUP_INIT.
  If so they will receive one callback per SUID discovered, and be given
  the attributes event for it.  The Sensor must return true/false whether it
  wishes to store and use this SUID.

  @note: All utilities in this file can be used in island mode.

  Copyright (c) 2017 - 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include <stdint.h>
#include "sns_data_stream.h"
#include "sns_mem_util.h"
#include "sns_sensor_uid.h"
#include "sns_types.h"

/*=============================================================================
  Forward Declarations
  ===========================================================================*/

struct sns_sensor;
struct sns_sensor_event;

/*=============================================================================
  Macro Definitions
  ===========================================================================*/

/**
 * Anonymous structure definition generator.
 *
 * @param[i] cnt Number of data types to look-up
 */
#define SNS_SUID_LOOKUP_DATA(cnt) \
struct  \
{ \
  uint16_t sensors_cnt; \
  uint16_t suid_idx; \
  sns_suid_lookup_cb suid_cb; \
  sns_data_stream *suid_stream; \
  sns_data_stream *attr_stream; \
  struct \
  { \
    char *data_type; \
    sns_sensor_uid suid; \
  } sensors[cnt]; \
}

/*=============================================================================
  Type Definitions
  ===========================================================================*/

/**
 * Optional callback registered by Sensor.
 *
 * @param[i] sensor Same Sensor as provided in sns_suid_lookup_add
 * @param[i] data_type Data type under review
 * @param[i] event Encoded attribute event
 *
 * @return True to save this SUID; false if to continue search
 */
typedef bool (*sns_suid_lookup_cb)
  (struct sns_sensor *const sensor, char const *data_type,
   struct sns_sensor_event *event);

/*=============================================================================
  Function Declarations
  ===========================================================================*/

/**
 * Initialize the data created by SNS_SUID_LOOKUP_DATA.
 *
 * @param[i] suid_lookup_data Data created by SNS_SUID_LOOKUP_DATA
 * @param[i] cb Optional callback function; see sns_suid_lookup_cb
 */
#define SNS_SUID_LOOKUP_INIT(suid_lookup_data, cb) \
do { \
  sns_memzero(&suid_lookup_data, sizeof(suid_lookup_data)); \
  suid_lookup_data.sensors_cnt = ARR_SIZE(suid_lookup_data.sensors); \
  suid_lookup_data.suid_cb = cb; \
} while(false)

/**
 * Deinitialize the data initialized by SNS_SUID_LOOKUP_INIT.  Closes all
 * data streams but leaves all retrieved SUIDs accessible.
 *
 * Should not be called if Sensor is required to act upon subsequently
 * missing dependencies (i.e. a dependency goes away).
 *
 * @param[i] sensor Use same Sensor as provided in sns_suid_lookup_add
 * @param[i] suid_lookup_data Data created by SNS_SUID_LOOKUP_DATA
 */
void sns_suid_lookup_deinit(struct sns_sensor *sensor, void *suid_lookup_data);

/**
 * Initiate SUID lookup for the default sensor of a data type.
 *
 * @param[i] sensor
 * @param[i] suid_lookup_data Lookup data initialized by SNS_SUID_LOOKUP_INIT
 * @param[i] data_type Null-terminated data type string to lookup
 */
void sns_suid_lookup_add(struct sns_sensor *sensor, void *suid_lookup_data,
    char *data_type);

/**
 * Get the SUID already looked-up and associated with data type.  Not
 * reliable until sns_suid_lookup_complete returns true.
 *
 * @param[i] suid_lookup_data Data on which sns_suid_lookup_add was called
 * @param[i] data_type Null-terminated data type string to lookup
 * @param[o] suid Sensor UID if found (optional)
 *
 * @return True if SUID has been found; false otherwise
 */
bool sns_suid_lookup_get(void *suid_lookup_data,
    char *data_type, sns_sensor_uid *suid);

/**
 * Whether the SUID lookup process has completed successfully.
 *
 * @param[i] suid_lookup_data Data on which sns_suid_lookup_add was called
 *
 * @return true if all requested data_type SUIDs were found; false otherwise
 */
bool sns_suid_lookup_complete(void *suid_lookup_data);

/**
 * Handle a SUID or attribute event.  Must be called within the Sensor's
 * notify_event function.
 *
 * @param[i] sensor Use same Sensor as provided in sns_suid_lookup_add
 * @param[i] suid_lookup_data Lookup data initialized by SNS_SUID_LOOKUP_INIT
 */
void sns_suid_lookup_handle(struct sns_sensor *sensor, void *suid_lookup_data);

