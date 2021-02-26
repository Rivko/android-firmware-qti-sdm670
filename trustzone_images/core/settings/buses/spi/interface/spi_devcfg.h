/**
    @file  spi_devcfg.h
    @brief device configuration implementation
 */
/*=============================================================================
            Copyright (c) 2017 Qualcomm Technologies, Incorporated.
                              All rights reserved.
              Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

#ifndef __SPI_DEVCFG_H__
#define __SPI_DEVCFG_H__

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
    char       *se_clock_name;
    char      **common_clocks_name;

    void       *miso_encoding;
    void       *mosi_encoding;
    void       *clk_encoding;
    void       *cs_encoding[SPI_NUM_CHIP_SELECTS];

    void       *tx_dma_ring_base;
    void       *rx_dma_ring_base;
/*
Control where the SPI master samples the incoming data.
To compensate for line delay, SPI master samples on negedge (protocol says posedge). 
To not do this compensation, set the bit below.
*/
    boolean    miso_sampling_ctrl_set;
} spi_plat_device_config;

#endif /*__SPI_DEVCFG_H__*/
