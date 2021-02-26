/**
    @file  spi_devcfg.c
    @brief device configuration data
 */
/*=============================================================================
            Copyright (c) 2017 Qualcomm Technologies, Incorporated.
                              All rights reserved.
              Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

#include "spi_devcfg.h"

/* Enable the SPI cores that are used on this image here.
 * On SDM845, there are 16 available SPI instances with index from SPI_1 to SPI_16.
 * For security, only GSI mode is allowed. 
 * Only use GPI instances allocated to TZ image (See qupv3_gpii_perms in file:
 * TZBuild/core/settings/buses/qup_accesscontrol/hoya/config/QUPAC_845_Access.c
 * for details of which GPI instances can be used from Trustzone.
*/
#define TZ_USE_SPI_16
#define TZ_USE_SPI_1

typedef struct num_cores { uint32 num; } num_cores;

const char *common_clocks_qup0 [] =
{
    "gcc_qupv3_wrap0_core_2x_clk",
    "gcc_qupv3_wrap0_core_clk",
    "gcc_qupv3_wrap_0_s_ahb_clk",
    "gcc_qupv3_wrap_0_m_ahb_clk",
    NULL,
};

const char *common_clocks_qup1 [] =
{
    "gcc_qupv3_wrap1_core_2x_clk",
    "gcc_qupv3_wrap1_core_clk",
    "gcc_qupv3_wrap_1_s_ahb_clk",
    "gcc_qupv3_wrap_1_m_ahb_clk",
    NULL,
};

typedef PACKED struct _gpi_ring_elem
{
   uint32 dword_0;
   uint32 dword_1;
   uint32 dword_2;
   uint32 ctrl;
}gpi_ring_elem;

#define MAX_TX_TRE_LIST_SIZE_PER_CORE 8
#define MAX_RX_TRE_LIST_SIZE_PER_CORE 8

typedef struct
{
  char     *gpio_str_name;
  uint32    config[3];
  uint32    gpio_id;
} spi_plat_gpio_properties;

#define TX_RING_ALIGN (MAX_TX_TRE_LIST_SIZE_PER_CORE << 4)
#define RX_RING_ALIGN (MAX_RX_TRE_LIST_SIZE_PER_CORE << 4)

//GPIO configuration definitions
#define TLMM_GPIO_INPUT 0
#define TLMM_GPIO_OUTPUT 1

#define TLMM_GPIO_NO_PULL    0
#define TLMM_GPIO_PULL_DOWN  0x1
#define TLMM_GPIO_KEEPER     0x2
#define TLMM_GPIO_PULL_UP    0x3

#define TLMM_GPIO_2MA     0
#define TLMM_GPIO_4MA     1
#define TLMM_GPIO_6MA     2
#define TLMM_GPIO_8MA     3
#define TLMM_GPIO_10MA    4
#define TLMM_GPIO_12MA    5
#define TLMM_GPIO_14MA    6
#define TLMM_GPIO_16MA    7

#ifdef TZ_USE_SPI_1
static gpi_ring_elem gpi_rx_tre_list_base_1 [MAX_RX_TRE_LIST_SIZE_PER_CORE] __attribute__ ((aligned (RX_RING_ALIGN)));
static gpi_ring_elem gpi_tx_tre_list_base_1 [MAX_TX_TRE_LIST_SIZE_PER_CORE] __attribute__ ((aligned (TX_RING_ALIGN)));
static spi_plat_gpio_properties tz_spi_1_miso = {"qup_l0[0]", { TLMM_GPIO_INPUT,  TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA }, 00 };
static spi_plat_gpio_properties tz_spi_1_mosi = {"qup_l1[0]", { TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA }, 00 };
static spi_plat_gpio_properties tz_spi_1_clk  = {"qup_l2[0]", { TLMM_GPIO_OUTPUT, TLMM_GPIO_NO_PULL,   TLMM_GPIO_6MA }, 00 };
static spi_plat_gpio_properties tz_spi_1_cs_0 = {"qup_l3[0]", { TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA }, 00 };
#endif

#ifdef TZ_USE_SPI_2
static gpi_ring_elem gpi_rx_tre_list_base_2 [MAX_RX_TRE_LIST_SIZE_PER_CORE] __attribute__ ((aligned (RX_RING_ALIGN)));
static gpi_ring_elem gpi_tx_tre_list_base_2 [MAX_TX_TRE_LIST_SIZE_PER_CORE] __attribute__ ((aligned (TX_RING_ALIGN)));
static spi_plat_gpio_properties tz_spi_2_miso = {"qup_l0[1]", { TLMM_GPIO_INPUT,  TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA }, 00 };
static spi_plat_gpio_properties tz_spi_2_mosi = {"qup_l1[1]", { TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA }, 00 };
static spi_plat_gpio_properties tz_spi_2_clk  = {"qup_l2[1]", { TLMM_GPIO_OUTPUT, TLMM_GPIO_NO_PULL,   TLMM_GPIO_6MA }, 00 };
static spi_plat_gpio_properties tz_spi_2_cs_0 = {"qup_l3[1]", { TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA }, 00 };
#endif

#ifdef TZ_USE_SPI_3
static gpi_ring_elem gpi_rx_tre_list_base_3 [MAX_RX_TRE_LIST_SIZE_PER_CORE] __attribute__ ((aligned (RX_RING_ALIGN)));
static gpi_ring_elem gpi_tx_tre_list_base_3 [MAX_TX_TRE_LIST_SIZE_PER_CORE] __attribute__ ((aligned (TX_RING_ALIGN)));
static spi_plat_gpio_properties tz_spi_3_miso = {"qup_l0[2]", { TLMM_GPIO_INPUT,  TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA }, 00 };
static spi_plat_gpio_properties tz_spi_3_mosi = {"qup_l1[2]", { TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA }, 00 };
static spi_plat_gpio_properties tz_spi_3_clk  = {"qup_l2[2]", { TLMM_GPIO_OUTPUT, TLMM_GPIO_NO_PULL,   TLMM_GPIO_6MA }, 00 };
static spi_plat_gpio_properties tz_spi_3_cs_0 = {"qup_l3[2]", { TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA }, 00 };
#endif

#ifdef TZ_USE_SPI_4
static gpi_ring_elem gpi_rx_tre_list_base_4 [MAX_RX_TRE_LIST_SIZE_PER_CORE] __attribute__ ((aligned (RX_RING_ALIGN)));
static gpi_ring_elem gpi_tx_tre_list_base_4 [MAX_TX_TRE_LIST_SIZE_PER_CORE] __attribute__ ((aligned (TX_RING_ALIGN)));
static spi_plat_gpio_properties tz_spi_4_miso = {"qup_l0[3]", { TLMM_GPIO_INPUT,  TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA }, 00 };
static spi_plat_gpio_properties tz_spi_4_mosi = {"qup_l1[3]", { TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA }, 00 };
static spi_plat_gpio_properties tz_spi_4_clk  = {"qup_l2[3]", { TLMM_GPIO_OUTPUT, TLMM_GPIO_NO_PULL,   TLMM_GPIO_6MA }, 00 };
static spi_plat_gpio_properties tz_spi_4_cs_0 = {"qup_l3[3]", { TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA }, 00 };
#endif

#ifdef TZ_USE_SPI_5
static gpi_ring_elem gpi_rx_tre_list_base_5 [MAX_RX_TRE_LIST_SIZE_PER_CORE] __attribute__ ((aligned (RX_RING_ALIGN)));
static gpi_ring_elem gpi_tx_tre_list_base_5 [MAX_TX_TRE_LIST_SIZE_PER_CORE] __attribute__ ((aligned (TX_RING_ALIGN)));
static spi_plat_gpio_properties tz_spi_5_miso = {"qup_l0[4]", { TLMM_GPIO_INPUT,  TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA }, 00 };
static spi_plat_gpio_properties tz_spi_5_mosi = {"qup_l1[4]", { TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA }, 00 };
static spi_plat_gpio_properties tz_spi_5_clk  = {"qup_l2[4]", { TLMM_GPIO_OUTPUT, TLMM_GPIO_NO_PULL,   TLMM_GPIO_6MA }, 00 };
static spi_plat_gpio_properties tz_spi_5_cs_0 = {"qup_l3[4]", { TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA }, 00 };
#endif

#ifdef TZ_USE_SPI_6
static gpi_ring_elem gpi_rx_tre_list_base_6 [MAX_RX_TRE_LIST_SIZE_PER_CORE] __attribute__ ((aligned (RX_RING_ALIGN)));
static gpi_ring_elem gpi_tx_tre_list_base_6 [MAX_TX_TRE_LIST_SIZE_PER_CORE] __attribute__ ((aligned (TX_RING_ALIGN)));
static spi_plat_gpio_properties tz_spi_6_miso = {"qup_l0[5]", { TLMM_GPIO_INPUT,  TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA }, 00 };
static spi_plat_gpio_properties tz_spi_6_mosi = {"qup_l1[5]", { TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA }, 00 };
static spi_plat_gpio_properties tz_spi_6_clk  = {"qup_l2[5]", { TLMM_GPIO_OUTPUT, TLMM_GPIO_NO_PULL,   TLMM_GPIO_6MA }, 00 };
static spi_plat_gpio_properties tz_spi_6_cs_0 = {"qup_l3[5]", { TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA }, 00 };
#endif

#ifdef TZ_USE_SPI_7
static gpi_ring_elem gpi_rx_tre_list_base_7 [MAX_RX_TRE_LIST_SIZE_PER_CORE] __attribute__ ((aligned (RX_RING_ALIGN)));
static gpi_ring_elem gpi_tx_tre_list_base_7 [MAX_TX_TRE_LIST_SIZE_PER_CORE] __attribute__ ((aligned (TX_RING_ALIGN)));
static spi_plat_gpio_properties tz_spi_7_miso = {"qup_l0[6]", { TLMM_GPIO_INPUT,  TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA }, 00 };
static spi_plat_gpio_properties tz_spi_7_mosi = {"qup_l1[6]", { TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA }, 00 };
static spi_plat_gpio_properties tz_spi_7_clk  = {"qup_l2[6]", { TLMM_GPIO_OUTPUT, TLMM_GPIO_NO_PULL,   TLMM_GPIO_6MA }, 00 };
static spi_plat_gpio_properties tz_spi_7_cs_0 = {"qup_l3[6]", { TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA }, 00 };
#endif

#ifdef TZ_USE_SPI_8
static gpi_ring_elem gpi_rx_tre_list_base_8 [MAX_RX_TRE_LIST_SIZE_PER_CORE] __attribute__ ((aligned (RX_RING_ALIGN)));
static gpi_ring_elem gpi_tx_tre_list_base_8 [MAX_TX_TRE_LIST_SIZE_PER_CORE] __attribute__ ((aligned (TX_RING_ALIGN)));
static spi_plat_gpio_properties tz_spi_8_miso = {"qup_l0[7]", { TLMM_GPIO_INPUT,  TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA }, 00 };
static spi_plat_gpio_properties tz_spi_8_mosi = {"qup_l1[7]", { TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA }, 00 };
static spi_plat_gpio_properties tz_spi_8_clk  = {"qup_l2[7]", { TLMM_GPIO_OUTPUT, TLMM_GPIO_NO_PULL,   TLMM_GPIO_6MA }, 00 };
static spi_plat_gpio_properties tz_spi_8_cs_0 = {"qup_l3[7]", { TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA }, 00 };
#endif

#ifdef TZ_USE_SPI_9
static gpi_ring_elem gpi_rx_tre_list_base_9 [MAX_RX_TRE_LIST_SIZE_PER_CORE] __attribute__ ((aligned (RX_RING_ALIGN)));
static gpi_ring_elem gpi_tx_tre_list_base_9 [MAX_TX_TRE_LIST_SIZE_PER_CORE] __attribute__ ((aligned (TX_RING_ALIGN)));
static spi_plat_gpio_properties tz_spi_9_miso = {"qup_l0[8]", { TLMM_GPIO_INPUT,  TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA }, 00 };
static spi_plat_gpio_properties tz_spi_9_mosi = {"qup_l1[8]", { TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA }, 00 };
static spi_plat_gpio_properties tz_spi_9_clk  = {"qup_l2[8]", { TLMM_GPIO_OUTPUT, TLMM_GPIO_NO_PULL,   TLMM_GPIO_6MA }, 00 };
static spi_plat_gpio_properties tz_spi_9_cs_0 = {"qup_l3[8]", { TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA }, 00 };
#endif

#ifdef TZ_USE_SPI_10
static gpi_ring_elem gpi_rx_tre_list_base_10 [MAX_RX_TRE_LIST_SIZE_PER_CORE] __attribute__ ((aligned (RX_RING_ALIGN)));
static gpi_ring_elem gpi_tx_tre_list_base_10 [MAX_TX_TRE_LIST_SIZE_PER_CORE] __attribute__ ((aligned (TX_RING_ALIGN)));
static spi_plat_gpio_properties tz_spi_10_miso = {"qup_l0[9]", { TLMM_GPIO_INPUT,  TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA }, 00 };
static spi_plat_gpio_properties tz_spi_10_mosi = {"qup_l1[9]", { TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA }, 00 };
static spi_plat_gpio_properties tz_spi_10_clk  = {"qup_l2[9]", { TLMM_GPIO_OUTPUT, TLMM_GPIO_NO_PULL,   TLMM_GPIO_6MA }, 00 };
static spi_plat_gpio_properties tz_spi_10_cs_0 = {"qup_l3[9]", { TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA }, 00 };
#endif

#ifdef TZ_USE_SPI_11
static gpi_ring_elem gpi_rx_tre_list_base_11 [MAX_RX_TRE_LIST_SIZE_PER_CORE] __attribute__ ((aligned (RX_RING_ALIGN)));
static gpi_ring_elem gpi_tx_tre_list_base_11 [MAX_TX_TRE_LIST_SIZE_PER_CORE] __attribute__ ((aligned (TX_RING_ALIGN)));
static spi_plat_gpio_properties tz_spi_11_miso = {"qup_l0[10]", { TLMM_GPIO_INPUT,  TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA }, 00 };
static spi_plat_gpio_properties tz_spi_11_mosi = {"qup_l1[10]", { TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA }, 00 };
static spi_plat_gpio_properties tz_spi_11_clk  = {"qup_l2[10]", { TLMM_GPIO_OUTPUT, TLMM_GPIO_NO_PULL,   TLMM_GPIO_6MA }, 00 };
static spi_plat_gpio_properties tz_spi_11_cs_0 = {"qup_l3[10]", { TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA }, 00 };
#endif

#ifdef TZ_USE_SPI_12
static gpi_ring_elem gpi_rx_tre_list_base_12 [MAX_RX_TRE_LIST_SIZE_PER_CORE] __attribute__ ((aligned (RX_RING_ALIGN)));
static gpi_ring_elem gpi_tx_tre_list_base_12 [MAX_TX_TRE_LIST_SIZE_PER_CORE] __attribute__ ((aligned (TX_RING_ALIGN)));
static spi_plat_gpio_properties tz_spi_12_miso = {"qup_l0[11]", { TLMM_GPIO_INPUT,  TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA }, 00 };
static spi_plat_gpio_properties tz_spi_12_mosi = {"qup_l1[11]", { TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA }, 00 };
static spi_plat_gpio_properties tz_spi_12_clk  = {"qup_l2[11]", { TLMM_GPIO_OUTPUT, TLMM_GPIO_NO_PULL,   TLMM_GPIO_6MA }, 00 };
static spi_plat_gpio_properties tz_spi_12_cs_0 = {"qup_l3[11]", { TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA }, 00 };
#endif

#ifdef TZ_USE_SPI_13
static gpi_ring_elem gpi_rx_tre_list_base_13 [MAX_RX_TRE_LIST_SIZE_PER_CORE] __attribute__ ((aligned (RX_RING_ALIGN)));
static gpi_ring_elem gpi_tx_tre_list_base_13 [MAX_TX_TRE_LIST_SIZE_PER_CORE] __attribute__ ((aligned (TX_RING_ALIGN)));
static spi_plat_gpio_properties tz_spi_13_miso = {"qup_l0[12]", { TLMM_GPIO_INPUT,  TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA }, 00 };
static spi_plat_gpio_properties tz_spi_13_mosi = {"qup_l1[12]", { TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA }, 00 };
static spi_plat_gpio_properties tz_spi_13_clk  = {"qup_l2[12]", { TLMM_GPIO_OUTPUT, TLMM_GPIO_NO_PULL,   TLMM_GPIO_6MA }, 00 };
static spi_plat_gpio_properties tz_spi_13_cs_0 = {"qup_l3[12]", { TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA }, 00 };
#endif

#ifdef TZ_USE_SPI_14
static gpi_ring_elem gpi_rx_tre_list_base_14 [MAX_RX_TRE_LIST_SIZE_PER_CORE] __attribute__ ((aligned (RX_RING_ALIGN)));
static gpi_ring_elem gpi_tx_tre_list_base_14 [MAX_TX_TRE_LIST_SIZE_PER_CORE] __attribute__ ((aligned (TX_RING_ALIGN)));
static spi_plat_gpio_properties tz_spi_14_miso = {"qup_l0[13]", { TLMM_GPIO_INPUT,  TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA }, 00 };
static spi_plat_gpio_properties tz_spi_14_mosi = {"qup_l1[13]", { TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA }, 00 };
static spi_plat_gpio_properties tz_spi_14_clk  = {"qup_l2[13]", { TLMM_GPIO_OUTPUT, TLMM_GPIO_NO_PULL,   TLMM_GPIO_6MA }, 00 };
static spi_plat_gpio_properties tz_spi_14_cs_0 = {"qup_l3[13]", { TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA }, 00 };
#endif

#ifdef TZ_USE_SPI_15
static gpi_ring_elem gpi_rx_tre_list_base_15 [MAX_RX_TRE_LIST_SIZE_PER_CORE] __attribute__ ((aligned (RX_RING_ALIGN)));
static gpi_ring_elem gpi_tx_tre_list_base_15 [MAX_TX_TRE_LIST_SIZE_PER_CORE] __attribute__ ((aligned (TX_RING_ALIGN)));
static spi_plat_gpio_properties tz_spi_15_miso = {"qup_l0[14]", { TLMM_GPIO_INPUT,  TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA }, 00 };
static spi_plat_gpio_properties tz_spi_15_mosi = {"qup_l1[14]", { TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA }, 00 };
static spi_plat_gpio_properties tz_spi_15_clk  = {"qup_l2[14]", { TLMM_GPIO_OUTPUT, TLMM_GPIO_NO_PULL,   TLMM_GPIO_6MA }, 00 };
static spi_plat_gpio_properties tz_spi_15_cs_0 = {"qup_l3[14]", { TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA }, 00 };
#endif

#ifdef TZ_USE_SPI_16
static gpi_ring_elem gpi_rx_tre_list_base_16 [MAX_RX_TRE_LIST_SIZE_PER_CORE] __attribute__ ((aligned (RX_RING_ALIGN)));
static gpi_ring_elem gpi_tx_tre_list_base_16 [MAX_TX_TRE_LIST_SIZE_PER_CORE] __attribute__ ((aligned (TX_RING_ALIGN)));
static spi_plat_gpio_properties tz_spi_16_miso = {"qup_l0[15]", { TLMM_GPIO_INPUT,  TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA }, 00 };
static spi_plat_gpio_properties tz_spi_16_mosi = {"qup_l1[15]", { TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA }, 00 };
static spi_plat_gpio_properties tz_spi_16_clk  = {"qup_l2[15]", { TLMM_GPIO_OUTPUT, TLMM_GPIO_NO_PULL,   TLMM_GPIO_6MA }, 00 };
static spi_plat_gpio_properties tz_spi_16_cs_0 = {"qup_l3[15]", { TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA }, 00 };
#endif

spi_plat_device_config spi_device_config[] =
{
#ifdef TZ_USE_SPI_1
    {
        .core_base_addr              = (uint8 *) 0x00800000,
        .core_offset                 = 0x00080000,
        .core_index                  = 0,
        .core_irq                    = 0, //not used, we use GSI callback
        .polled_mode                 = TRUE,
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 8,  //See notes at the top of this file
        .se_clock_dfs_index          = 1,
        .se_clock_frequency          = 19200,
        .se_clock_name               = "gcc_qupv3_wrap0_s0_clk",
        .common_clocks_name          = (char **)common_clocks_qup0,

        .miso_encoding                = (void*)&tz_spi_1_miso,
        .mosi_encoding                = (void*)&tz_spi_1_mosi,
        .clk_encoding                 = (void*)&tz_spi_1_clk,
        .cs_encoding                  = {(void*)&tz_spi_1_cs_0, 0, 0, 0},

        .tx_dma_ring_base            = gpi_tx_tre_list_base_1,
        .rx_dma_ring_base            = gpi_rx_tre_list_base_1,
		.miso_sampling_ctrl_set      = FALSE,
    },
#endif
#ifdef TZ_USE_SPI_2
    {
        .core_base_addr              = (uint8 *) 0x00080000,
        .core_offset                 = 0x00084000,
        .core_index                  = 1,
        .core_irq                    = 0, //not used, we use GSI callback
        .polled_mode                 = TRUE,
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 8,
        .se_clock_dfs_index          = 1,
        .se_clock_frequency          = 19200,
        .se_clock_name               = "gcc_qupv3_wrap0_s1_clk",
        .common_clocks_name          = (char **)common_clocks_qup0,

        .miso_encoding                = (void*)&tz_spi_2_miso,
        .mosi_encoding                = (void*)&tz_spi_2_mosi,
        .clk_encoding                 = (void*)&tz_spi_2_clk,
        .cs_encoding                  = {(void*)&tz_spi_2_cs_0, 0, 0, 0},

        .tx_dma_ring_base            = gpi_tx_tre_list_base_2,
        .rx_dma_ring_base            = gpi_rx_tre_list_base_2,
		.miso_sampling_ctrl_set      = FALSE,
    },
#endif
#ifdef TZ_USE_SPI_3
    {
        .core_base_addr              = (uint8 *) 0x00080000,
        .core_offset                 = 0x00084000,
        .core_index                  = 2,
        .core_irq                    = 0, //not used, we use GSI callback
        .polled_mode                 = TRUE,
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 8,
        .se_clock_dfs_index          = 1,
        .se_clock_frequency          = 19200,
        .se_clock_name               = "gcc_qupv3_wrap0_s2_clk",
        .common_clocks_name          = (char **)common_clocks_qup0,

        .miso_encoding                = (void*)&tz_spi_3_miso,
        .mosi_encoding                = (void*)&tz_spi_3_mosi,
        .clk_encoding                 = (void*)&tz_spi_3_clk,
        .cs_encoding                  = {(void*)&tz_spi_3_cs_0, 0, 0, 0},

        .tx_dma_ring_base            = gpi_tx_tre_list_base_3,
        .rx_dma_ring_base            = gpi_rx_tre_list_base_3,
		.miso_sampling_ctrl_set      = FALSE,
    },
#endif
#ifdef TZ_USE_SPI_4
    {
        .core_base_addr              = (uint8 *) 0x00080000,
        .core_offset                 = 0x00084000,
        .core_index                  = 3,
        .core_irq                    = 0, //not used, we use GSI callback
        .polled_mode                 = TRUE,
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 8,
        .se_clock_dfs_index          = 1,
        .se_clock_frequency          = 19200,
        .se_clock_name               = "gcc_qupv3_wrap0_s3_clk",
        .common_clocks_name          = (char **)common_clocks_qup0,

        .miso_encoding                = (void*)&tz_spi_4_miso,
        .mosi_encoding                = (void*)&tz_spi_4_mosi,
        .clk_encoding                 = (void*)&tz_spi_4_clk,
        .cs_encoding                  = {(void*)&tz_spi_4_cs_0, 0, 0, 0},

        .tx_dma_ring_base            = gpi_tx_tre_list_base_4,
        .rx_dma_ring_base            = gpi_rx_tre_list_base_4,
		.miso_sampling_ctrl_set      = FALSE,
    },
#endif
#ifdef TZ_USE_SPI_5
    {
        .core_base_addr              = (uint8 *) 0x00080000,
        .core_offset                 = 0x00084000,
        .core_index                  = 4,
        .core_irq                    = 0, //not used, we use GSI callback
        .polled_mode                 = TRUE,
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 8,
        .se_clock_dfs_index          = 1,
        .se_clock_frequency          = 19200,
        .se_clock_name               = "gcc_qupv3_wrap0_s4_clk",
        .common_clocks_name          = (char **)common_clocks_qup0,

        .miso_encoding                = (void*)&tz_spi_5_miso,
        .mosi_encoding                = (void*)&tz_spi_5_mosi,
        .clk_encoding                 = (void*)&tz_spi_5_clk,
        .cs_encoding                  = {(void*)&tz_spi_5_cs_0, 0, 0, 0},

        .tx_dma_ring_base            = gpi_tx_tre_list_base_5,
        .rx_dma_ring_base            = gpi_rx_tre_list_base_5,
		.miso_sampling_ctrl_set      = FALSE,
    },
#endif
#ifdef TZ_USE_SPI_6
    {
        .core_base_addr              = (uint8 *) 0x00080000,
        .core_offset                 = 0x00084000,
        .core_index                  = 5,
        .core_irq                    = 0, //not used, we use GSI callback
        .polled_mode                 = TRUE,
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 8,
        .se_clock_dfs_index          = 1,
        .se_clock_frequency          = 19200,
        .se_clock_name               = "gcc_qupv3_wrap0_s5_clk",
        .common_clocks_name          = (char **)common_clocks_qup0,

        .miso_encoding                = (void*)&tz_spi_6_miso,
        .mosi_encoding                = (void*)&tz_spi_6_mosi,
        .clk_encoding                 = (void*)&tz_spi_6_clk,
        .cs_encoding                  = {(void*)&tz_spi_6_cs_0, 0, 0, 0},

        .tx_dma_ring_base            = gpi_tx_tre_list_base_6,
        .rx_dma_ring_base            = gpi_rx_tre_list_base_6,
		.miso_sampling_ctrl_set      = FALSE,
    },
#endif
#ifdef TZ_USE_SPI_7
    {
        .core_base_addr              = (uint8 *) 0x00080000,
        .core_offset                 = 0x00084000,
        .core_index                  = 6,
        .core_irq                    = 0, //not used, we use GSI callback
        .polled_mode                 = TRUE,
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 8,
        .se_clock_dfs_index          = 1,
        .se_clock_frequency          = 19200,
        .se_clock_name               = "gcc_qupv3_wrap0_s6_clk",
        .common_clocks_name          = (char **)common_clocks_qup0,

        .miso_encoding                = (void*)&tz_spi_7_miso,
        .mosi_encoding                = (void*)&tz_spi_7_mosi,
        .clk_encoding                 = (void*)&tz_spi_7_clk,
        .cs_encoding                  = {(void*)&tz_spi_7_cs_0, 0, 0, 0},

        .tx_dma_ring_base            = gpi_tx_tre_list_base_7,
        .rx_dma_ring_base            = gpi_rx_tre_list_base_7,
		.miso_sampling_ctrl_set      = FALSE,
    },
#endif
#ifdef TZ_USE_SPI_8
    {
        .core_base_addr              = (uint8 *) 0x00080000,
        .core_offset                 = 0x00084000,
        .core_index                  = 7,
        .core_irq                    = 0, //not used, we use GSI callback
        .polled_mode                 = TRUE,
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 8,
        .se_clock_dfs_index          = 1,
        .se_clock_frequency          = 19200,
        .se_clock_name               = "gcc_qupv3_wrap0_s7_clk",
        .common_clocks_name          = (char **)common_clocks_qup0,

        .miso_encoding                = (void*)&tz_spi_8_miso,
        .mosi_encoding                = (void*)&tz_spi_8_mosi,
        .clk_encoding                 = (void*)&tz_spi_8_clk,
        .cs_encoding                  = {(void*)&tz_spi_8_cs_0, 0, 0, 0},

        .tx_dma_ring_base            = gpi_tx_tre_list_base_8,
        .rx_dma_ring_base            = gpi_rx_tre_list_base_8,
		.miso_sampling_ctrl_set      = FALSE,
    },
#endif
#ifdef TZ_USE_SPI_9
    {
        .core_base_addr              = (uint8 *) 0x00A00000,
        .core_offset                 = 0x00080000,
        .core_index                  = 8,
        .core_irq                    = 0, //not used, we use GSI callback
        .polled_mode                 = TRUE,
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 9,
        .se_clock_dfs_index          = 1,
        .se_clock_frequency          = 19200,
        .se_clock_name               = "gcc_qupv3_wrap1_s0_clk",
        .common_clocks_name          = (char **)common_clocks_qup1,

        .miso_encoding                = (void*)&tz_spi_9_miso,
        .mosi_encoding                = (void*)&tz_spi_9_mosi,
        .clk_encoding                 = (void*)&tz_spi_9_clk,
        .cs_encoding                  = {(void*)&tz_spi_9_cs_0, 0, 0, 0},

        .tx_dma_ring_base            = gpi_tx_tre_list_base_9,
        .rx_dma_ring_base            = gpi_rx_tre_list_base_9,
		.miso_sampling_ctrl_set      = FALSE,
    },
#endif		
#ifdef TZ_USE_SPI_10
    {
        .core_base_addr              = (uint8 *) 0x00A00000,
        .core_offset                 = 0x00084000,
        .core_index                  = 9,
        .core_irq                    = 0, //not used, we use GSI callback
        .polled_mode                 = TRUE,
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 9,
        .se_clock_dfs_index          = 1,
        .se_clock_frequency          = 19200,
        .se_clock_name               = "gcc_qupv3_wrap1_s1_clk",
        .common_clocks_name          = (char **)common_clocks_qup1,

        .miso_encoding                = (void*)&tz_spi_10_miso,
        .mosi_encoding                = (void*)&tz_spi_10_mosi,
        .clk_encoding                 = (void*)&tz_spi_10_clk,
        .cs_encoding                  = {(void*)&tz_spi_10_cs_0, 0, 0, 0},

        .tx_dma_ring_base            = gpi_tx_tre_list_base_10,
        .rx_dma_ring_base            = gpi_rx_tre_list_base_10,
		.miso_sampling_ctrl_set      = FALSE,
    },
#endif		
#ifdef TZ_USE_SPI_11
    {
        .core_base_addr              = (uint8 *) 0x00A00000,
        .core_offset                 = 0x00088000,
        .core_index                  = 10,
        .core_irq                    = 0, //not used, we use GSI callback
        .polled_mode                 = TRUE,
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 9,
        .se_clock_dfs_index          = 1,
        .se_clock_frequency          = 19200,
        .se_clock_name               = "gcc_qupv3_wrap1_s2_clk",
        .common_clocks_name          = (char **)common_clocks_qup1,

        .miso_encoding                = (void*)&tz_spi_11_miso,
        .mosi_encoding                = (void*)&tz_spi_11_mosi,
        .clk_encoding                 = (void*)&tz_spi_11_clk,
        .cs_encoding                  = {(void*)&tz_spi_11_cs_0, 0, 0, 0},

        .tx_dma_ring_base            = gpi_tx_tre_list_base_11,
        .rx_dma_ring_base            = gpi_rx_tre_list_base_11,
		.miso_sampling_ctrl_set      = FALSE,
    },
#endif		
#ifdef TZ_USE_SPI_12
    {
        .core_base_addr              = (uint8 *) 0x00A00000,
        .core_offset                 = 0x0008C000,
        .core_index                  = 11,
        .core_irq                    = 0, //not used, we use GSI callback
        .polled_mode                 = TRUE,
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 9,
        .se_clock_dfs_index          = 1,
        .se_clock_frequency          = 19200,
        .se_clock_name               = "gcc_qupv3_wrap1_s3_clk",
        .common_clocks_name          = (char **)common_clocks_qup1,

        .miso_encoding                = (void*)&tz_spi_12_miso,
        .mosi_encoding                = (void*)&tz_spi_12_mosi,
        .clk_encoding                 = (void*)&tz_spi_12_clk,
        .cs_encoding                  = {(void*)&tz_spi_12_cs_0, 0, 0, 0},

        .tx_dma_ring_base            = gpi_tx_tre_list_base_12,
        .rx_dma_ring_base            = gpi_rx_tre_list_base_12,
		.miso_sampling_ctrl_set      = FALSE,
    },
#endif		
#ifdef TZ_USE_SPI_13
    {
        .core_base_addr              = (uint8 *) 0x00A00000,
        .core_offset                 = 0x00090000,
        .core_index                  = 12,
        .core_irq                    = 0, //not used, we use GSI callback
        .polled_mode                 = TRUE,
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 9,
        .se_clock_dfs_index          = 1,
        .se_clock_frequency          = 19200,
        .se_clock_name               = "gcc_qupv3_wrap1_s4_clk",
        .common_clocks_name          = (char **)common_clocks_qup1,

        .miso_encoding                = (void*)&tz_spi_13_miso,
        .mosi_encoding                = (void*)&tz_spi_13_mosi,
        .clk_encoding                 = (void*)&tz_spi_13_clk,
        .cs_encoding                  = {(void*)&tz_spi_13_cs_0, 0, 0, 0},

        .tx_dma_ring_base            = gpi_tx_tre_list_base_13,
        .rx_dma_ring_base            = gpi_rx_tre_list_base_13,
		.miso_sampling_ctrl_set      = FALSE,
    },
#endif	
#ifdef TZ_USE_SPI_14
    {
        .core_base_addr              = (uint8 *) 0x00A00000,
        .core_offset                 = 0x00094000,
        .core_index                  = 13,
        .core_irq                    = 0, //not used, we use GSI callback
        .polled_mode                 = TRUE,
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 9,
        .se_clock_dfs_index          = 1,
        .se_clock_frequency          = 19200,
        .se_clock_name               = "gcc_qupv3_wrap1_s5_clk",
        .common_clocks_name          = (char **)common_clocks_qup1,

        .miso_encoding                = (void*)&tz_spi_14_miso,
        .mosi_encoding                = (void*)&tz_spi_14_mosi,
        .clk_encoding                 = (void*)&tz_spi_14_clk,
        .cs_encoding                  = {(void*)&tz_spi_14_cs_0, 0, 0, 0},

        .tx_dma_ring_base            = gpi_tx_tre_list_base_14,
        .rx_dma_ring_base            = gpi_rx_tre_list_base_14,
		.miso_sampling_ctrl_set      = FALSE,
    },
#endif	
#ifdef TZ_USE_SPI_15
    {
        .core_base_addr              = (uint8 *) 0x00A00000,
        .core_offset                 = 0x00098000,
        .core_index                  = 14,
        .core_irq                    = 0, //not used, we use GSI callback
        .polled_mode                 = TRUE,
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 9,
        .se_clock_dfs_index          = 1,
        .se_clock_frequency          = 19200,
        .se_clock_name               = "gcc_qupv3_wrap1_s6_clk",
        .common_clocks_name          = (char **)common_clocks_qup1,

        .miso_encoding                = (void*)&tz_spi_15_miso,
        .mosi_encoding                = (void*)&tz_spi_15_mosi,
        .clk_encoding                 = (void*)&tz_spi_15_clk,
        .cs_encoding                  = {(void*)&tz_spi_15_cs_0, 0, 0, 0},

        .tx_dma_ring_base            = gpi_tx_tre_list_base_15,
        .rx_dma_ring_base            = gpi_rx_tre_list_base_15,
		.miso_sampling_ctrl_set      = FALSE,
    },
#endif
#ifdef TZ_USE_SPI_16
    {
        .core_base_addr              = (uint8 *) 0x00A00000,
        .core_offset                 = 0x0009C000,
        .core_index                  = 15,
        .core_irq                    = 0, //not used, we use GSI callback
        .polled_mode                 = TRUE,
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 9,
        .se_clock_dfs_index          = 1,
        .se_clock_frequency          = 19200,
        .se_clock_name               = "gcc_qupv3_wrap1_s7_clk",
        .common_clocks_name          = (char **)common_clocks_qup1,

        .miso_encoding                = (void*)&tz_spi_16_miso,
        .mosi_encoding                = (void*)&tz_spi_16_mosi,
        .clk_encoding                 = (void*)&tz_spi_16_clk,
        .cs_encoding                  = {(void*)&tz_spi_16_cs_0, 0, 0, 0},

        .tx_dma_ring_base            = gpi_tx_tre_list_base_16,
        .rx_dma_ring_base            = gpi_rx_tre_list_base_16,
		.miso_sampling_ctrl_set      = FALSE,
    },
#endif
};

#define NUM_SPI_CORES_IN_USE (sizeof(spi_device_config)/sizeof(spi_plat_device_config))
num_cores num_spi_cores = { NUM_SPI_CORES_IN_USE };
