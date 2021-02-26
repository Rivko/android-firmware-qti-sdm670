/**
    @file  spi_devcfg.h
    @brief device configuration implementation
 */
/*=============================================================================
            Copyright (c) 2017-2018 Qualcomm Technologies, Incorporated.
                              All rights reserved.
              Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

/*==================================================================================================
Edit History

$Header: 

when       who          what, where, why
--------   --------     --------------------------------------------------------
07/20/18   ska          Disabled exclusive flag to avoid power leakage issue
08/01/17   ska          Ported for sdm670

==================================================================================================*/

#ifndef __SPI_DEVCFG_H__
#define __SPI_DEVCFG_H__

#include "spi_config.h"
#include "spi_driver.h"
#include "spi_gpi.h"
#include "qurt.h"

//
// CONFIGURATION START ============================================
//

// Enable the cores that are used on this image
/* Note that below GPIO has been shared between SE's so we need to make sure there is no conflict. 
 GPIO 05, 1st & 2nd
 GPIO 06, 1st & 2nd
 GPIO 14, 5th & 6th
 GPIO 15, 5th & 6th
 GPIO 16, 4th, 5th & 6th
 GPIO 17, 4th, 5th & 6th
 */
 
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
#define TLMM_GPIO_KEEPER     2
#define TLMM_GPIO_PULL_UP    3
#define TLMM_GPIO_2MA        0
#define TLMM_GPIO_6MA        2


// set GPIOs for the defined cores
// TLMM_GPIO_CFG(gpio, func, dir, pull, drive)

#define SSC_SPI_01_MISO TLMM_GPIO_CFG(0,  1, TLMM_GPIO_INPUT,  TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)
#define SSC_SPI_01_MOSI TLMM_GPIO_CFG(1,  1, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)
#define SSC_SPI_01_CLK  TLMM_GPIO_CFG(6,  3, TLMM_GPIO_OUTPUT, TLMM_GPIO_NO_PULL,   TLMM_GPIO_6MA)
#define SSC_SPI_01_CS_0 TLMM_GPIO_CFG(5,  2, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)

#define SSC_SPI_01_MISO_SLEEP TLMM_GPIO_CFG(0,  0, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SSC_SPI_01_MOSI_SLEEP TLMM_GPIO_CFG(1,  0, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SSC_SPI_01_CLK_SLEEP  TLMM_GPIO_CFG(6,  0, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SSC_SPI_01_CS_0_SLEEP TLMM_GPIO_CFG(5,  0, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)


#define SSC_SPI_02_MISO TLMM_GPIO_CFG(2,  1, TLMM_GPIO_INPUT,  TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)
#define SSC_SPI_02_MOSI TLMM_GPIO_CFG(3,  1, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)
#define SSC_SPI_02_CLK  TLMM_GPIO_CFG(4,  1, TLMM_GPIO_OUTPUT, TLMM_GPIO_NO_PULL,   TLMM_GPIO_6MA)
#define SSC_SPI_02_CS_0 TLMM_GPIO_CFG(5,  1, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)
#define SSC_SPI_02_CS_1 TLMM_GPIO_CFG(6,  1, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)
#define SSC_SPI_02_CS_2 TLMM_GPIO_CFG(7,  1, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)

#define SSC_SPI_02_MISO_SLEEP TLMM_GPIO_CFG(2,  0, TLMM_GPIO_INPUT, TLMM_GPIO_NO_PULL, TLMM_GPIO_2MA)
#define SSC_SPI_02_MOSI_SLEEP TLMM_GPIO_CFG(3,  0, TLMM_GPIO_INPUT, TLMM_GPIO_NO_PULL, TLMM_GPIO_2MA)
#define SSC_SPI_02_CLK_SLEEP  TLMM_GPIO_CFG(4,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SSC_SPI_02_CS_0_SLEEP TLMM_GPIO_CFG(5,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SSC_SPI_02_CS_1_SLEEP TLMM_GPIO_CFG(6,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SSC_SPI_02_CS_2_SLEEP TLMM_GPIO_CFG(7,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)


#define SSC_SPI_03_MISO TLMM_GPIO_CFG(8,  1, TLMM_GPIO_INPUT,  TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)
#define SSC_SPI_03_MOSI TLMM_GPIO_CFG(9,  1, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)
#define SSC_SPI_03_CLK  TLMM_GPIO_CFG(10, 1, TLMM_GPIO_OUTPUT, TLMM_GPIO_NO_PULL,   TLMM_GPIO_6MA)
#define SSC_SPI_03_CS_0 TLMM_GPIO_CFG(11, 1, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)

#define SSC_SPI_03_MISO_SLEEP TLMM_GPIO_CFG(8,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SSC_SPI_03_MOSI_SLEEP TLMM_GPIO_CFG(9,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SSC_SPI_03_CLK_SLEEP  TLMM_GPIO_CFG(10, 0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SSC_SPI_03_CS_0_SLEEP TLMM_GPIO_CFG(11, 0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)


#define SSC_SPI_04_MISO TLMM_GPIO_CFG(16, 3, TLMM_GPIO_INPUT,  TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)
#define SSC_SPI_04_MOSI TLMM_GPIO_CFG(17, 3, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)
#define SSC_SPI_04_CLK  TLMM_GPIO_CFG(12, 1, TLMM_GPIO_OUTPUT, TLMM_GPIO_NO_PULL,   TLMM_GPIO_6MA)
#define SSC_SPI_04_CS_0 TLMM_GPIO_CFG(13, 1, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)

#define SSC_SPI_04_MISO_SLEEP TLMM_GPIO_CFG(16, 0, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SSC_SPI_04_MOSI_SLEEP TLMM_GPIO_CFG(17, 0, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SSC_SPI_04_CLK_SLEEP  TLMM_GPIO_CFG(12, 0, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SSC_SPI_04_CS_0_SLEEP TLMM_GPIO_CFG(13, 0, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)

#define SSC_SPI_05_MISO TLMM_GPIO_CFG(16, 2, TLMM_GPIO_INPUT,  TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)
#define SSC_SPI_05_MOSI TLMM_GPIO_CFG(17, 2, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)
#define SSC_SPI_05_CLK  TLMM_GPIO_CFG(14, 1, TLMM_GPIO_OUTPUT, TLMM_GPIO_NO_PULL,   TLMM_GPIO_6MA)
#define SSC_SPI_05_CS_0 TLMM_GPIO_CFG(15, 1, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)

#define SSC_SPI_05_MISO_SLEEP TLMM_GPIO_CFG(16, 0, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SSC_SPI_05_MOSI_SLEEP TLMM_GPIO_CFG(17, 0, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SSC_SPI_05_CLK_SLEEP  TLMM_GPIO_CFG(14, 0, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SSC_SPI_05_CS_0_SLEEP TLMM_GPIO_CFG(15, 0, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)


#define SSC_SPI_06_MISO TLMM_GPIO_CFG(14, 2, TLMM_GPIO_INPUT,  TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)
#define SSC_SPI_06_MOSI TLMM_GPIO_CFG(15, 2, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)
#define SSC_SPI_06_CLK  TLMM_GPIO_CFG(16, 1, TLMM_GPIO_OUTPUT, TLMM_GPIO_NO_PULL,   TLMM_GPIO_6MA)
#define SSC_SPI_06_CS_0 TLMM_GPIO_CFG(17, 1, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)

#define SSC_SPI_06_MISO_SLEEP TLMM_GPIO_CFG(14, 0, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SSC_SPI_06_MOSI_SLEEP TLMM_GPIO_CFG(15, 0, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SSC_SPI_06_CLK_SLEEP  TLMM_GPIO_CFG(16, 0, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SSC_SPI_06_CS_0_SLEEP TLMM_GPIO_CFG(17, 0, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)

//
// CONFIGURATION END   ============================================
//

#define TX_RING_ALIGN (MAX_TX_TRE_LIST_SIZE_PER_CORE << 4)
#define RX_RING_ALIGN (MAX_RX_TRE_LIST_SIZE_PER_CORE << 4)


#ifdef SSC_USE_SPI_01
static gpi_ring_elem gpi_rx_tre_list_base_01 [MAX_RX_TRE_LIST_SIZE_PER_CORE] __attribute__ ((aligned (TX_RING_ALIGN)));
static gpi_ring_elem gpi_tx_tre_list_base_01 [MAX_TX_TRE_LIST_SIZE_PER_CORE] __attribute__ ((aligned (RX_RING_ALIGN)));
#endif

#ifdef SSC_USE_SPI_02
static gpi_ring_elem gpi_rx_tre_list_base_02 [MAX_RX_TRE_LIST_SIZE_PER_CORE] __attribute__ ((aligned (RX_RING_ALIGN)));
static gpi_ring_elem gpi_tx_tre_list_base_02 [MAX_TX_TRE_LIST_SIZE_PER_CORE] __attribute__ ((aligned (TX_RING_ALIGN)));
#endif

#ifdef SSC_USE_SPI_03
static gpi_ring_elem gpi_rx_tre_list_base_03 [MAX_RX_TRE_LIST_SIZE_PER_CORE] __attribute__ ((aligned (TX_RING_ALIGN)));
static gpi_ring_elem gpi_tx_tre_list_base_03 [MAX_TX_TRE_LIST_SIZE_PER_CORE] __attribute__ ((aligned (RX_RING_ALIGN)));
#endif

#ifdef SSC_USE_SPI_04
static gpi_ring_elem gpi_rx_tre_list_base_04 [MAX_RX_TRE_LIST_SIZE_PER_CORE] __attribute__ ((aligned (TX_RING_ALIGN)));
static gpi_ring_elem gpi_tx_tre_list_base_04 [MAX_TX_TRE_LIST_SIZE_PER_CORE] __attribute__ ((aligned (RX_RING_ALIGN)));
#endif

#ifdef SSC_USE_SPI_05
static gpi_ring_elem gpi_rx_tre_list_base_05 [MAX_RX_TRE_LIST_SIZE_PER_CORE] __attribute__ ((aligned (TX_RING_ALIGN)));
static gpi_ring_elem gpi_tx_tre_list_base_05 [MAX_TX_TRE_LIST_SIZE_PER_CORE] __attribute__ ((aligned (RX_RING_ALIGN)));
#endif

#ifdef SSC_USE_SPI_06
static gpi_ring_elem gpi_rx_tre_list_base_06 [MAX_RX_TRE_LIST_SIZE_PER_CORE] __attribute__ ((aligned (TX_RING_ALIGN)));
static gpi_ring_elem gpi_tx_tre_list_base_06 [MAX_TX_TRE_LIST_SIZE_PER_CORE] __attribute__ ((aligned (RX_RING_ALIGN)));
#endif

#define NUM_SPI_CORES_IN_USE (sizeof(spi_device_config)/sizeof(spi_plat_device_config))

static spi_plat_device_config spi_device_config[] =
{
#ifdef SSC_USE_SPI_01
    {
        .core_base_addr              = (uint8 *) 0x62600000,
        .core_offset                 = 0x00080000,
        .core_index                  = 0,
        .core_irq                    = 0,   //not used, we use GSI callback
#ifdef POLL_MODE_BUS_DRIVERS
        .polled_mode                 = TRUE,
#else		
	    .polled_mode                 = FALSE,
#endif
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 0,
        .se_clock_dfs_index          = 0,
        .se_clock_frequency          = 19200,

        .miso_encoding                = SSC_SPI_01_MISO,
        .mosi_encoding                = SSC_SPI_01_MOSI,
        .clk_encoding                 = SSC_SPI_01_CLK,
        .cs_encoding                = {SSC_SPI_01_CS_0, 0, 0, 0},
        
        .sleep_miso_encoding          = SSC_SPI_01_MISO_SLEEP,
        .sleep_mosi_encoding          = SSC_SPI_01_MOSI_SLEEP,
        .sleep_clk_encoding           = SSC_SPI_01_CLK_SLEEP,
        .sleep_cs_encoding            = {SSC_SPI_01_CS_0_SLEEP, 0, 0, 0},


        .tx_dma_ring_base            = gpi_tx_tre_list_base_01,
        .rx_dma_ring_base            = gpi_rx_tre_list_base_01,
    },
#endif
#ifdef SSC_USE_SPI_02
    {
        .core_base_addr              = (uint8 *) 0x62600000,
        .core_offset                 = 0x00084000,
        .core_index                  = 1,
        .core_irq                    = 0,   //not used, we use GSI callback
#ifdef POLL_MODE_BUS_DRIVERS
        .polled_mode                 = TRUE,
#else		
	    .polled_mode                 = FALSE,
#endif
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 1,
        .se_clock_dfs_index          = 0,
        .se_clock_frequency          = 19200,

		.exclusive                    = FALSE,  //Is this SPI instance also used by another EE?
        .miso_encoding                = SSC_SPI_02_MISO,
        .mosi_encoding                = SSC_SPI_02_MOSI,
        .clk_encoding                 = SSC_SPI_02_CLK,
        .cs_encoding                = {SSC_SPI_02_CS_0, SSC_SPI_02_CS_1, SSC_SPI_02_CS_2, 0},

        .sleep_miso_encoding          = SSC_SPI_02_MISO_SLEEP,
        .sleep_mosi_encoding          = SSC_SPI_02_MOSI_SLEEP,
        .sleep_clk_encoding           = SSC_SPI_02_CLK_SLEEP,
        .sleep_cs_encoding            = {SSC_SPI_02_CS_0_SLEEP, SSC_SPI_02_CS_1_SLEEP, SSC_SPI_02_CS_2_SLEEP, 0},

        .tx_dma_ring_base            = gpi_tx_tre_list_base_02,
        .rx_dma_ring_base            = gpi_rx_tre_list_base_02,
    },
#endif
#ifdef SSC_USE_SPI_03
    {
        .core_base_addr              = (uint8 *) 0x62600000,
        .core_offset                 = 0x00088000,
        .core_index                  = 2,
        .core_irq                    = 0, //not used, we use GSI callback
#ifdef POLL_MODE_BUS_DRIVERS
        .polled_mode                 = TRUE,
#else		
	    .polled_mode                 = FALSE,
#endif	
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 2, //See "GPII_Mappings_Sample.xlsx"
        .se_clock_dfs_index          = 0,
        .se_clock_frequency          = 19200,

		.exclusive                    = FALSE,  //Is this SPI instance also used by another EE?
        .miso_encoding                = SSC_SPI_03_MISO,
        .mosi_encoding                = SSC_SPI_03_MOSI,
        .clk_encoding                 = SSC_SPI_03_CLK,
        .cs_encoding                = {SSC_SPI_03_CS_0, 0, 0, 0},

        .sleep_miso_encoding          = SSC_SPI_03_MISO_SLEEP,
        .sleep_mosi_encoding          = SSC_SPI_03_MOSI_SLEEP,
        .sleep_clk_encoding           = SSC_SPI_03_CLK_SLEEP,
        .sleep_cs_encoding            = {SSC_SPI_03_CS_0_SLEEP, 0, 0, 0},

        .tx_dma_ring_base            = gpi_tx_tre_list_base_03,
        .rx_dma_ring_base            = gpi_rx_tre_list_base_03,
    },
#endif
#ifdef SSC_USE_SPI_04
    {
        .core_base_addr              = (uint8 *) 0x62600000,
        .core_offset                 = 0x0008C000,
        .core_index                  = 3,
        .core_irq                    = 0, //not used, we use GSI callback
#ifdef POLL_MODE_BUS_DRIVERS
        .polled_mode                 = TRUE,
#else		
	    .polled_mode                 = FALSE,
#endif
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 3, //See "GPII_Mappings_Sample.xlsx"
        .se_clock_dfs_index          = 0,
        .se_clock_frequency          = 19200,

        .miso_encoding                = SSC_SPI_04_MISO,
        .mosi_encoding                = SSC_SPI_04_MOSI,
        .clk_encoding                 = SSC_SPI_04_CLK,
        .cs_encoding                = {SSC_SPI_04_CS_0, 0, 0, 0},
        
        .sleep_miso_encoding          = SSC_SPI_04_MISO_SLEEP,
        .sleep_mosi_encoding          = SSC_SPI_04_MOSI_SLEEP,
        .sleep_clk_encoding           = SSC_SPI_04_CLK_SLEEP,
        .sleep_cs_encoding            = {SSC_SPI_04_CS_0_SLEEP, 0, 0, 0},

        .tx_dma_ring_base            = gpi_tx_tre_list_base_04,
        .rx_dma_ring_base            = gpi_rx_tre_list_base_04,
    },
#endif	
#ifdef SSC_USE_SPI_05
    {
        .core_base_addr              = (uint8 *) 0x62600000,
        .core_offset                 = 0x00090000,
        .core_index                  = 4,
        .core_irq                    = 0, //not used, we use GSI callback
#ifdef POLL_MODE_BUS_DRIVERS
        .polled_mode                 = TRUE,
#else		
	    .polled_mode                 = FALSE,
#endif
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 4, //See "GPII_Mappings_Sample.xlsx"
        .se_clock_dfs_index          = 0,
        .se_clock_frequency          = 19200,

        .miso_encoding                = SSC_SPI_05_MISO,
        .mosi_encoding                = SSC_SPI_05_MOSI,
        .clk_encoding                 = SSC_SPI_05_CLK,
        .cs_encoding                = {SSC_SPI_05_CS_0, 0, 0, 0},
        
        .sleep_miso_encoding          = SSC_SPI_05_MISO_SLEEP,
        .sleep_mosi_encoding          = SSC_SPI_05_MOSI_SLEEP,
        .sleep_clk_encoding           = SSC_SPI_05_CLK_SLEEP,
        .sleep_cs_encoding            = {SSC_SPI_05_CS_0_SLEEP, 0, 0, 0},

        .tx_dma_ring_base            = gpi_tx_tre_list_base_05,
        .rx_dma_ring_base            = gpi_rx_tre_list_base_05,
    },
#endif
#ifdef SSC_USE_SPI_06	
    {
        .core_base_addr              = (uint8 *) 0x62600000,
        .core_offset                 = 0x00094000,
        .core_index                  = 5,
        .core_irq                    = 0, //not used, we use GSI callback
#ifdef POLL_MODE_BUS_DRIVERS
        .polled_mode                 = TRUE,
#else		
	    .polled_mode                 = FALSE,
#endif
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 5, //See "GPII_Mappings_Sample.xlsx"
        .se_clock_dfs_index          = 0,
        .se_clock_frequency          = 19200,

        .miso_encoding                = SSC_SPI_06_MISO,
        .mosi_encoding                = SSC_SPI_06_MOSI,
        .clk_encoding                 = SSC_SPI_06_CLK,
        .cs_encoding                  = {SSC_SPI_06_CS_0, 0, 0, 0},
        
        
        .sleep_miso_encoding          = SSC_SPI_06_MISO_SLEEP,
        .sleep_mosi_encoding          = SSC_SPI_06_MOSI_SLEEP,
        .sleep_clk_encoding           = SSC_SPI_06_CLK_SLEEP,
        .sleep_cs_encoding            = {SSC_SPI_06_CS_0_SLEEP, 0, 0, 0},

        .tx_dma_ring_base            = gpi_tx_tre_list_base_06,
        .rx_dma_ring_base            = gpi_rx_tre_list_base_06,
    },
#endif	
};

#define NUM_SPI_CLIENTS 4

static spi_hw_ctxt          spi_bus_hw_ctxt [NUM_SPI_CORES_IN_USE];
static qurt_mutex_t         spi_bus_crit_sec[NUM_SPI_CORES_IN_USE];
static qurt_signal_t        spi_bus_signal[NUM_SPI_CORES_IN_USE];
static qurt_mutex_t         spi_bus_queue_lock[NUM_SPI_CORES_IN_USE];

static spi_gpi_ctxt         spi_bus_gpi_ctxt[NUM_SPI_CORES_IN_USE];
static spi_client_ctxt      spi_bus_client_ctxt [NUM_SPI_CORES_IN_USE * NUM_SPI_CLIENTS];
static spi_gpi_io_ctxt      spi_bus_gpi_io_ctxt [NUM_SPI_CORES_IN_USE * NUM_SPI_CLIENTS];

#endif /*__SPI_DEVCFG_H__*/
