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

#include "spi_config.h"
#include "spi_driver.h"
#include "spi_gpi.h"
#include "Tlmm.h"
#include "rex.h"

//
// CONFIGURATION START ============================================
//

// enable the cores that are used on this image
#define SSC_USE_SPI_02

// set GPIOs for the defined cores
// TLMM_GPIO_CFG(gpio, func, dir, pull, drive) 
#define SSC_SPI_02_MISO TLMM_GPIO_CFG(2,  1, TLMM_GPIO_INPUT,  TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)
#define SSC_SPI_02_MOSI TLMM_GPIO_CFG(3,  1, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)
#define SSC_SPI_02_CLK  TLMM_GPIO_CFG(4,  1, TLMM_GPIO_OUTPUT, TLMM_GPIO_NO_PULL,   TLMM_GPIO_6MA)
#define SSC_SPI_02_CS_0 TLMM_GPIO_CFG(5,  1, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)
#define SSC_SPI_02_CS_1 TLMM_GPIO_CFG(6,  1, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)
#define SSC_SPI_02_CS_2 TLMM_GPIO_CFG(7,  1, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)

//
// CONFIGURATION END   ============================================
//
#define TX_RING_ALIGN (MAX_TX_TRE_LIST_SIZE_PER_CORE << 4)
#define RX_RING_ALIGN (MAX_RX_TRE_LIST_SIZE_PER_CORE << 4)

#ifdef SSC_USE_SPI_02
static gpi_ring_elem gpi_rx_tre_list_base_02 [MAX_RX_TRE_LIST_SIZE_PER_CORE] __attribute__ ((aligned (RX_RING_ALIGN)));
static gpi_ring_elem gpi_tx_tre_list_base_02 [MAX_TX_TRE_LIST_SIZE_PER_CORE] __attribute__ ((aligned (TX_RING_ALIGN)));
#endif

#define NUM_SPI_CORES_IN_USE (sizeof(spi_device_config)/sizeof(spi_plat_device_config))

static spi_plat_device_config spi_device_config[] = 
{
#ifdef SSC_USE_SPI_02
    {
        .core_base_addr              = (uint8 *) 0x0,
        .core_offset                 = 0x0,
        .core_index                  = 0,
        .core_irq                    = 0,   //not used, we use GSI callback
        .polled_mode                 = FALSE,
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 0,
        .se_clock_dfs_index          = 0,
        .se_clock_frequency          = 0,

        .miso_encoding                = SSC_SPI_02_MISO,
        .mosi_encoding                = SSC_SPI_02_MOSI,
		.clk_encoding                 = SSC_SPI_02_CLK,
		.cs_encoding                = {SSC_SPI_02_CS_0, SSC_SPI_02_CS_1, SSC_SPI_02_CS_2, 0, 0, 0, 0},

        .tcsr_base_addr              = (uint8 *) 0x0,
        .tcsr_reg_offset             = 0x00000000,
        .tcsr_reg_value              = 0x00000000,

        .tx_dma_ring_base            = gpi_tx_tre_list_base_02,
        .rx_dma_ring_base            = gpi_rx_tre_list_base_02,
    },
#endif
};

#define NUM_SPI_CLIENTS 4

static spi_hw_ctxt          spi_bus_hw_ctxt [NUM_SPI_CORES_IN_USE];
static rex_crit_sect_type   spi_bus_crit_sec[NUM_SPI_CORES_IN_USE];
static spi_gpi_ctxt         spi_bus_gpi_ctxt[NUM_SPI_CORES_IN_USE];
static spi_client_ctxt      spi_bus_client_ctxt [NUM_SPI_CLIENTS];
static spi_gpi_io_ctxt      spi_bus_gpi_io_ctxt [NUM_SPI_CLIENTS];

#endif /*__SPI_DEVCFG_H__*/
