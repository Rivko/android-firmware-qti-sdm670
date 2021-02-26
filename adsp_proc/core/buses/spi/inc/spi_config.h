/** 
    @file  SPI_config.h
    @brief interface to device configuration
 */
/*=============================================================================
            Copyright (c) 2016-2017 Qualcomm Technologies, Incorporated.
                              All rights reserved.
              Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

#ifndef __SPI_CONFIG_H__
#define __SPI_CONFIG_H__

#include "comdef.h"

#define SPI_NUM_CHIP_SELECTS   4
typedef struct spi_plat_device_config
{
    uint8      *core_base_addr;
    uint32      core_offset;
    uint8       core_index;
    uint16      core_irq;
    boolean     polled_mode;
    uint16      min_data_length_for_dma;

    uint8       gpi_index;
    uint8       se_clock_dfs_index;
    uint32      se_clock_frequency;

	boolean     exclusive;
    uint32      miso_encoding;
    uint32      mosi_encoding;
	uint32      clk_encoding;
	uint32      cs_encoding[SPI_NUM_CHIP_SELECTS];

    uint32      sleep_miso_encoding;
    uint32      sleep_mosi_encoding;
	uint32      sleep_clk_encoding;
	uint32      sleep_cs_encoding[SPI_NUM_CHIP_SELECTS];

    void       *tx_dma_ring_base;
    void       *rx_dma_ring_base;

	uint32      resource_votes;
} spi_plat_device_config;

#endif /*__SPI_CONFIG_H__*/
