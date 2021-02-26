/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */
#pragma once

int32_t focaltech_i2cReadReg(const uint16_t address, void *buf, const uint32_t bufLen);

int32_t focaltech_i2cWriteReg(const uint16_t address, void *buf, const uint32_t bufLen);

int32_t focaltech_i2cOpen(void);

int32_t focaltech_i2cClose(void);
