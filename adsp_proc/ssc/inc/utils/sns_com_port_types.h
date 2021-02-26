#pragma once
/**
 * sns_com_port_types.h
 *
 * Common definitions used by synch_com_port (SCP) and
 * asynch_com_port (ACP).
 *
 * Copyright (c) 2016 - 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <inttypes.h>
#include <stdbool.h>

/**
 * Types of register addresses.
 */
typedef enum
{
  SNS_REG_ADDR_8_BIT,
  SNS_REG_ADDR_16_BIT,
  SNS_REG_ADDR_32_BIT,
  /* Additional register types will be added here. */
} sns_reg_addr_type;

/**
 * Types of communication ports.
 */
typedef enum
{
  SNS_BUS_MIN  =  0,
  SNS_BUS_I2C  =  SNS_BUS_MIN,
  SNS_BUS_SPI  =  1,
  SNS_BUS_UART =  2, // DEPRECATED. Please use the async_uart sensor
  SNS_BUS_I3C  =  3,
  SNS_BUS_MAX  =  SNS_BUS_I3C,
} sns_bus_type;

typedef struct
{
  sns_bus_type       bus_type;           /* Bus type from sns_bus_type.*/
  uint32_t           slave_control;      /* Slave Address for I2C/I3C. Chip Select for SPI.*/
  sns_reg_addr_type  reg_addr_type;      /* Register address type for the slave.*/
  uint32_t           min_bus_speed_KHz;  /* Minimum bus clock supported by slave in kHz.*/
  uint32_t           max_bus_speed_KHz;  /* Maximum bus clock supported by slave in kHz.*/
  uint8_t            bus_instance;       /* Platform bus instance number (BLSP number).*/

} sns_com_port_config;

/**
 * Read or Write transaction on a port
 */
typedef struct
{
  bool             is_write;     /* true for a write operation. false for read.*/
  uint32_t         reg_addr;     /* Register address for tranfer.*/
  uint32_t         bytes;        /* Number of bytes to read or write */
  uint8_t          *buffer;      /* Buffer to read or write */
} sns_port_vector;

