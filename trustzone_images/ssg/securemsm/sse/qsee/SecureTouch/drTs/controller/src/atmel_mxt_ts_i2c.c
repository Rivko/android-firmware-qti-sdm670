/*
 * Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#include "sselog.h"
#include <stdint.h>
#include <IxErrno.h>
#include "qsee_i2c.h"
#include "i2c_stub.h"
#include "atmel_mxt_ts_defs.h"


static qsee_i2c_bus_config_t g_busConfig =
{
  100,                              // uFsBusFreqInKhz;
  0,                                // uHsBusFreqInKhz; not used
  QSEE_I2C_STANDARD_INTERFACE,     // eInterface;  not used
  2500                              // nMaxBusAcquireWaitTimeMsec;
};

static qsee_i2c_slave_device_config_t g_deviceConfig = {
  .slave_address = I2C_ADDRESS,
  .slave_address_option = QSEE_I2C_7_BIT_SLAVE_ADDRESS,
  .slave_device_type = QSEE_I2C_MEMORY_ADDRESS_DEVICE,
  .read_option = QSEE_I2C_GEN_STOP_START_BEFORE_READ,
  .byte_transfer_wait_time_usec = 2500
};

static qsee_i2c_config_t g_i2c_config = {
  .p_bus_config = &g_busConfig,
  .p_slave_config = &g_deviceConfig
};

int32_t atmel_i2cReadReg(const uint16_t address, void *buf, const uint32_t bufLen)
{
  int32_t ret = 0;
  uint32_t retry = 1000;
  qsee_i2c_transaction_info_t  readInfo = {
    .start_addr = ((address >> 8) & 0x00FF) | ((address & 0x00FF) << 8),
    .p_buf = (unsigned char*)buf,
    .buf_len = bufLen,
    .total_bytes = 0
  };
  do {
    ret = qsee_i2c_read(
      I2C_DEVICE,
      &g_i2c_config,
      &readInfo
      );
    if (ret == -E_DEV_FAILURE) {
      LOG_E("E_DEV_FAILURE on I2C bus, retrying (%u)", retry);
    } else if (ret) {
      LOG_E("qsee_i2c_read: 0x%08X", ret);
    }
  } while ((retry-- > 0) && (ret == -E_DEV_FAILURE));
  return ret;
}

int32_t atmel_i2cOpen(void)
{
  int32_t ret = 0;
  uint32_t retry = 1000;
  do {
    ret = qsee_i2c_open(I2C_DEVICE);
    if (ret == -E_DEV_FAILURE) {
      LOG_E("E_DEV_FAILURE on I2C bus, retrying (%u)", retry);
    } else if (ret) {
      LOG_E("qsee_i2c_open: 0x%08X", ret);
    }
  } while ((retry-- > 0) && (ret == -E_DEV_FAILURE));
  return ret;
}

int32_t atmel_i2cClose(void)
{
  int32_t ret = 0;
  uint32_t retry = 1000;
  do {
    ret = qsee_i2c_close(I2C_DEVICE);
    if (ret == -E_DEV_FAILURE) {
      LOG_E("E_DEV_FAILURE on I2C bus, retrying (%u)", retry);
    } else if (ret) {
      LOG_E("qsee_i2c_open: 0x%08X", ret);
    }
  } while ((retry-- > 0) && (ret == -E_DEV_FAILURE));
  return ret;
}
