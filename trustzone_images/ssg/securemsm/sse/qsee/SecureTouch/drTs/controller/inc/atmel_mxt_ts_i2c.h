/*
 * Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */
#pragma once

int32_t atmel_i2cReadReg(const uint16_t address, void *buf, const uint32_t bufLen);

int32_t atmel_i2cOpen(void);

int32_t atmel_i2cClose(void);
