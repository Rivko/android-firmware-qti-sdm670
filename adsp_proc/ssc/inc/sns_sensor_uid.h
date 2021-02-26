#pragma once
/*=============================================================================
  @file sns_sensor_uid.h

  A SUID uniquely identifies a specific Sensor.
  E.g. Different Accel drivers will use different and unique SUIDs.
  Each Sensor (driver or algorithm) has hard-coded SUIDs
  A driver loaded multiple times, may need to advertise different SUIDs for each

  Query message for clients
  Clients specify a particular attribute they want to match against
  I.e. “Datatype” = “Accelerometer”
  Framework determines the list of Sensors that have this matching attribute
  And returns this list of SUIDs in the response message

  All communication from the client <-> Sensor will use this SUID
  Send individual query to each SUID for additional attributes

  @note: All utilities in this file can be used in island mode.

  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>

/*=============================================================================
  Preprocessor Definitions
  ===========================================================================*/

/* PRINTF type specifier used in confjunction with SNS_PRI_SUID */
#define PRIsuid "08"PRIx32" %08"PRIx32" %08"PRIx32" %08"PRIx32
/* PRINTF argument used in junction with PRIsuid type specifier */
#define SNS_PRI_SUID(suid) \
  ((uint32_t)(suid)->sensor_uid[15] << 24) + ((uint32_t)(suid)->sensor_uid[14] << 16) + ((uint32_t)(suid)->sensor_uid[13] << 8) + (uint32_t)(suid)->sensor_uid[12], \
  ((uint32_t)(suid)->sensor_uid[11] << 24) + ((uint32_t)(suid)->sensor_uid[10] << 16) + ((uint32_t)(suid)->sensor_uid[9] << 8) + (uint32_t)(suid)->sensor_uid[8], \
  ((uint32_t)(suid)->sensor_uid[7] << 24) + ((uint32_t)(suid)->sensor_uid[6] << 16) + ((uint32_t)(suid)->sensor_uid[5] << 8) + (uint32_t)(suid)->sensor_uid[4], \
  ((uint32_t)(suid)->sensor_uid[3] << 24) + ((uint32_t)(suid)->sensor_uid[2] << 16) + ((uint32_t)(suid)->sensor_uid[1] << 8) + (uint32_t)(suid)->sensor_uid[0]

/*=============================================================================
  Type Definitions
  ===========================================================================*/

typedef struct sns_sensor_uid
{
  uint8_t sensor_uid[16];
} sns_sensor_uid;

/*=============================================================================
  Public Function Declarations
  ===========================================================================*/

/**
 * Determine whether the two SUIDs are equivalent.
 *
 * @return True if equivalent, false otherwise
 */
bool sns_sensor_uid_compare(struct sns_sensor_uid const *uid1,
                            struct sns_sensor_uid const *uid2);
