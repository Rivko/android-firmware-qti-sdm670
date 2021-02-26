#ifndef QSEE_SPI_H
#define QSEE_SPI_H



/**
@file qsee_spi.h
@brief Provide SPI API wrappers
*/

/*===========================================================================
  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header:
  $DateTime:
  $Author:

when         who      what, where, why
--------     ---      ------------------------------------
06/14/2017   vmk      Updated for QUP v3
08/05/2016   dpk      Updated SENSOR SPI device IDs.
11/03/2015   dpk      Support for slave number
02/23/2015   dpk      Updated with changes to tzbsp_spi.h
05/12/2013   ag       Initial version.

===========================================================================*/

#include <stdint.h>

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/


/** @brief Devices supported by the interface. These device IDs map on to the
 *         QUP SE cores supported by the Hardware*/
typedef enum
{
  QSEE_SPI_DEVICE_1,      /* QUP0 SE 0*/
  QSEE_SPI_DEVICE_2,      /* QUP0 SE 1*/
  QSEE_SPI_DEVICE_3,      /* QUP0 SE 2*/
  QSEE_SPI_DEVICE_4,      /* QUP0 SE 3*/
  QSEE_SPI_DEVICE_5,      /* QUP0 SE 4*/
  QSEE_SPI_DEVICE_6,      /* QUP0 SE 5*/
  QSEE_SPI_DEVICE_7,      /* QUP0 SE 6*/
  QSEE_SPI_DEVICE_8,      /* QUP0 SE 7*/
  QSEE_SPI_DEVICE_9,      /* QUP1 SE 0*/
  QSEE_SPI_DEVICE_10,     /* QUP1 SE 1*/
  QSEE_SPI_DEVICE_11,     /* QUP1 SE 2*/
  QSEE_SPI_DEVICE_12,     /* QUP1 SE 3*/
  QSEE_SPI_DEVICE_13,     /* QUP1 SE 4*/
  QSEE_SPI_DEVICE_14,     /* QUP1 SE 5*/
  QSEE_SPI_DEVICE_15,     /* QUP1 SE 6*/
  QSEE_SPI_DEVICE_16,     /* QUP1 SE 7*/

  QSEE_SPI_SENSOR_DEVICE_1,  /* SENSOR SPI_1 */
  QSEE_SPI_SENSOR_DEVICE_2,  /* SENSOR SPI_2 */
  QSEE_SPI_SENSOR_DEVICE_3,  /* SENSOR SPI_3 */
  QSEE_SPI_DEVICE_COUNT
  /**< Used to check the bounds of this enumeration. */
} qsee_spi_device_id_t;


/**
 In the idle state whether the SPI clk is high or low.
*/
typedef enum
{
   QSEE_SPI_CLK_IDLE_LOW,/**< CLK signal is low when idle.*/
   QSEE_SPI_CLK_IDLE_HIGH/**< CLK signal is high when idle.*/
}qsee_spi_clock_polarity_t;

/**
 In idle state whether the Chip Select is high or low.
*/
typedef enum
{
   QSEE_SPI_CS_ACTIVE_LOW,/**< During idle state CS  line is held low*/
   QSEE_SPI_CS_ACTIVE_HIGH/**< During idle state CS line is held high*/
}qsee_spi_cs_polarity_t;

/**
Shift mode(CPHA), detemines which signal (input or output) is sampled first.
*/
typedef enum
{
   QSEE_SPI_INPUT_FIRST_MODE,/**< In both Master and slave input Bit is shifted in first.*/
   QSEE_SPI_OUTPUT_FIRST_MODE/**< In both Master and slave  output Bit is shifted in first*/
}qsee_spi_shift_mode_t;

/**
 List of values that inidicates, whether the Chip select is always active or if it's brought back to its idle state
 after N clock cycles.
*/
typedef enum
{
   QSEE_SPI_CS_DEASSERT,/**< CS is deasserted after transferring data for N clock cycles*/
   QSEE_SPI_CS_KEEP_ASSERTED/**< CS is asserted as long as the core is in Run state*/
}qsee_spi_cs_mode_t;

/**
 SPI Clock is always on or turned off after every transaction, this enum is deprecated.
 The driver ignores the value passed.
*/
typedef enum
{

   QSEE_SPI_CLK_NORMAL, /**< Turns off SPI CLK during idle state.*/
   QSEE_SPI_CLK_ALWAYS_ON/**< Runs SPI CLK during idle state.*/
}qsee_spi_clock_mode_t;
/** @brief Structure that is used for SPI configuration. */

typedef struct
{
  /**< SPI clock frequency */
  uint32_t max_freq;

  /**< Clock polarity  type to be used for the SPI core.*/
  qsee_spi_clock_polarity_t spi_clk_polarity;

  /**< Shift mode(CPHA) type to be used for SPI core.*/
  qsee_spi_shift_mode_t spi_shift_mode;

  /**< CS polarity type to be used for the SPI core.*/
  qsee_spi_cs_polarity_t spi_cs_polarity;

  /**< CS Mode to be used for the SPI core.*/
  qsee_spi_cs_mode_t spi_cs_mode;

  /**< Clock mode type to be used for the SPI core.*/
  qsee_spi_clock_mode_t spi_clk_always_on;

  /**< SPI bits per word, any value from 3 to 31*/
  uint8_t spi_bits_per_word;

  /**< Put the Device in High Performance */
  uint8_t hs_mode;

  /**< Put the Device in loop back test*/
  uint8_t loopback;

  /**< Slave index from 0 to 3, if mulitple SPI devices are connected to the same master */
  uint8_t spi_slave_index;

  /**< The minimum delay between two word(N-bit) transfers */
  uint32_t deassertion_time_ns;
}qsee_spi_config_t;

typedef struct
{
 void* buf_addr;
 /**<buff address for read or write data*/
 uint32_t buf_len;
 /**<size in bytes*/
 uint32_t total_bytes;
 /**<total bytes successfully transfered on SPI Bus*/
}qsee_spi_transaction_info_t;

/**
 * Open the device, and perform some HW intialization \n
 *
 * @param[in] device_id The SPI device ID to attach to
 *
 * @return 0 on success, negative on failure
 */
int qsee_spi_open(qsee_spi_device_id_t device_id);

/**
 * Reads data from SPI bus.
 *
 * @param[in] device_id The SPI device ID to attach to
 * @param[in] p_config The desired SPI configuration
 * @param[in] p_read_info Read buffer information
 *
 * @return 0 on success, negative on failure
 */
int qsee_spi_read(
    qsee_spi_device_id_t device_id,
    const qsee_spi_config_t* p_config,
    qsee_spi_transaction_info_t* p_read_info
);

/**
 * Writes data on SPI bus.
 *
 * @param[in] device_id The SPI device ID to attach to
 * @param[in] p_config The desired SPI configuration
 * @param[in] p_write_info Write buffer information
 *
 * @return 0 on success, negative on failure
 */
int qsee_spi_write(
    qsee_spi_device_id_t device_id,
    const qsee_spi_config_t* p_config,
    qsee_spi_transaction_info_t* p_write_info
);

/**
 * Writes data on SPI bus.
 *
 * @param[in] device_id The SPI device ID to attach to
 * @param[in] p_config The desired SPI configuration
 * @param[in] p_write_info Write buffer information
 * @param[in] p_read_info Read buffer information
 *
 * @return 0 on success, negative on failure
 */
int qsee_spi_full_duplex(
    qsee_spi_device_id_t device_id,
    const qsee_spi_config_t* p_config,
    qsee_spi_transaction_info_t* p_write_info,
    qsee_spi_transaction_info_t* p_read_info
);

/**
 * Transfer access to the SPI bus to the calling application.
 *
 *   2. Open the device \n
 *   3. Lock the SPI bus for exclusive access \n
 *
 * @param[in] device_id The SPI device ID to attach to
 *
 * @return 0 on success, negative on failure
 */

int qsee_spi_close(qsee_spi_device_id_t device_id);

#endif
