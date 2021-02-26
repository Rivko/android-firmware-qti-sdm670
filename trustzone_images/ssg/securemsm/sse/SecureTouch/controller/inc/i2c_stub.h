/*
 * Copyright (c) 2013-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#pragma once

#include <stdint.h>

/** Functions representing a controller
 * */
struct i2cFunc {
  int32_t (*i2cReadReg)(const uint16_t address, void *buf, const uint32_t bufLen);
  int32_t (*i2cWriteReg)(const uint16_t address, void *buf, const uint32_t bufLen);
  int32_t (*i2cOpen)(void);
  int32_t (*i2cClose)(void);
};
