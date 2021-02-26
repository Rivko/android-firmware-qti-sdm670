/**
    @file  spi_devcfg.h
    @brief device configuration implementation
 */
/*=============================================================================
            Copyright (c) 2016-2017 Qualcomm Technologies, Incorporated.
                              All rights reserved.
              Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

#ifndef __SPI_DEVCFG_H__
#define __SPI_DEVCFG_H__

#include "spi_config.h"
#include "spi_driver.h"
#include "spi_gpi.h"
#include "qurt.h"

//
// CONFIGURATION START ============================================
//

// enable the cores that are used on this image
#define SSC_USE_SPI_02
#define SSC_USE_SPI_03

#define TLMM_GPIO_CFG(gpio, func, dir, pull, drive) \
                          (((gpio) & 0x3FF) << 4  | \
                           ((func) & 0xF  ) << 0  | \
                           ((dir)  & 0x1  ) << 14 | \
                           ((pull) & 0x3  ) << 15 | \
                           ((drive)& 0xF  ) << 17)

#define TLMM_GPIO_INPUT      0
#define TLMM_GPIO_OUTPUT     1
#define TLMM_GPIO_NO_PULL    0
#define TLMM_GPIO_PULL_DOWN  1
#define TLMM_GPIO_PULL_UP    3
#define TLMM_GPIO_6MA        2


// set GPIOs for the defined cores
// TLMM_GPIO_CFG(gpio, func, dir, pull, drive)
#define SSC_SPI_02_MISO TLMM_GPIO_CFG(2,  1, TLMM_GPIO_INPUT,  TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)
#define SSC_SPI_02_MOSI TLMM_GPIO_CFG(3,  1, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)
#define SSC_SPI_02_CLK  TLMM_GPIO_CFG(4,  1, TLMM_GPIO_OUTPUT, TLMM_GPIO_NO_PULL,   TLMM_GPIO_6MA)
#define SSC_SPI_02_CS_0 TLMM_GPIO_CFG(5,  1, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)
#define SSC_SPI_02_CS_1 TLMM_GPIO_CFG(6,  1, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)
#define SSC_SPI_02_CS_2 TLMM_GPIO_CFG(7,  1, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)


#define SSC_SPI_03_MISO TLMM_GPIO_CFG(8,  1, TLMM_GPIO_INPUT,  TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)
#define SSC_SPI_03_MOSI TLMM_GPIO_CFG(9,  1, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)
#define SSC_SPI_03_CLK  TLMM_GPIO_CFG(10, 1, TLMM_GPIO_OUTPUT, TLMM_GPIO_NO_PULL,   TLMM_GPIO_6MA)
#define SSC_SPI_03_CS_0 TLMM_GPIO_CFG(11, 1, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)
//
// CONFIGURATION END   ============================================
//

#define TX_RING_ALIGN (MAX_TX_TRE_LIST_SIZE_PER_CORE << 4)
#define RX_RING_ALIGN (MAX_RX_TRE_LIST_SIZE_PER_CORE << 4)

//#define SSC_USE_SPI_03

#ifdef SSC_USE_SPI_03
static gpi_ring_elem gpi_rx_tre_list_base_03 [MAX_TX_TRE_LIST_SIZE_PER_CORE] __attribute__ ((aligned (TX_RING_ALIGN)));
static gpi_ring_elem gpi_tx_tre_list_base_03 [MAX_TX_TRE_LIST_SIZE_PER_CORE] __attribute__ ((aligned (RX_RING_ALIGN)));
#endif

#ifdef SSC_USE_SPI_02
static gpi_ring_elem gpi_rx_tre_list_base_02 [MAX_RX_TRE_LIST_SIZE_PER_CORE] __attribute__ ((aligned (RX_RING_ALIGN)));
static gpi_ring_elem gpi_tx_tre_list_base_02 [MAX_TX_TRE_LIST_SIZE_PER_CORE] __attribute__ ((aligned (TX_RING_ALIGN)));
#endif

#define NUM_SPI_CORES_IN_USE (sizeof(spi_device_config)/sizeof(spi_plat_device_config))

static spi_plat_device_config spi_device_config[] =
{
#ifdef SSC_USE_SPI_03
    {
        .core_base_addr              = (uint8 *) 0x5A00000,
        .core_offset                 = 0x00040000,
        .core_index                  = 2,
        .core_irq                    = 0, //not used, we use GSI callback
        .polled_mode                 = TRUE,
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 2, //See "GPII_Mappings_Sample.xlsx"
        .se_clock_dfs_index          = 1,
        .se_clock_frequency          = 19200,

        .miso_encoding                = SSC_SPI_03_MISO,
        .mosi_encoding                = SSC_SPI_03_MOSI,
        .clk_encoding                 = SSC_SPI_03_CLK,
        .cs_encoding                  = {SSC_SPI_03_CS_0, 0, 0, 0},

        .tcsr_base_addr              = (uint8 *) 0x01FC0000,
        .tcsr_reg_offset             = 0x00000000,
        .tcsr_reg_value              = 0x00000000,

        .tx_dma_ring_base            = gpi_tx_tre_list_base_03,
        .rx_dma_ring_base            = gpi_rx_tre_list_base_03,
    },
#endif
#ifdef SSC_USE_SPI_02
    {
        .core_base_addr              = (uint8 *) 0x5A00000,
        .core_offset                 = 0x00080000,
        .core_index                  = 1,
        .core_irq                    = 0,   //not used, we use GSI callback
        .polled_mode                 = TRUE,
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 1,
        .se_clock_dfs_index          = 1,
        .se_clock_frequency          = 19200,

        .miso_encoding                = SSC_SPI_02_MISO,
        .mosi_encoding                = SSC_SPI_02_MOSI,
        .clk_encoding                 = SSC_SPI_02_CLK,
        .cs_encoding                  = {SSC_SPI_02_CS_0, SSC_SPI_02_CS_1, SSC_SPI_02_CS_2, 0},

        .tcsr_base_addr              = (uint8 *) 0x01FC0000,
        .tcsr_reg_offset             = 0x00000000,
        .tcsr_reg_value              = 0x00000000,

        .tx_dma_ring_base            = gpi_tx_tre_list_base_02,
        .rx_dma_ring_base            = gpi_rx_tre_list_base_02,
    },
#endif
};

#define NUM_SPI_CLIENTS 4

static spi_hw_ctxt          spi_bus_hw_ctxt [NUM_SPI_CORES_IN_USE];
static qurt_mutex_t         spi_bus_crit_sec[NUM_SPI_CORES_IN_USE];
static qurt_signal_t        spi_bus_signal[NUM_SPI_CORES_IN_USE];

static spi_gpi_ctxt         spi_bus_gpi_ctxt[NUM_SPI_CORES_IN_USE];
static spi_client_ctxt      spi_bus_client_ctxt [NUM_SPI_CORES_IN_USE * NUM_SPI_CLIENTS];
static spi_gpi_io_ctxt      spi_bus_gpi_io_ctxt [NUM_SPI_CORES_IN_USE * NUM_SPI_CLIENTS];

#endif /*__SPI_DEVCFG_H__*/
