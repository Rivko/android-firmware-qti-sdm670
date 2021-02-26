/**
    @file  i2c_devcfg.c
    @brief device configuration data
 */
/*=============================================================================
            Copyright (c) 2017 Qualcomm Technologies, Incorporated.
                              All rights reserved.
              Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

#include "i2c_config.h"
#include "gpi.h"
#include "i2c_iface.h"
#include "qurt.h"

#define ENABLE_I2C_01

#define QUPV3_CORE_BASE_ADDRESS 0x62600000

const char *common_clocks_str_0 [] =
{
    "scc_qupv3_2xcore_clk",
    "scc_qupv3_core_clk",
    "scc_qupv3_s_hclk_clk",
    "scc_qupv3_m_hclk_clk",
    NULL,
};

const char *se_clocks_str_0 [] =
{
    "scc_qupv3_se0_clk",
    "scc_qupv3_se1_clk",
    "scc_qupv3_se2_clk",
    "scc_qupv3_se3_clk",
    "scc_qupv3_se4_clk",
    "scc_qupv3_se5_clk",
};

const char *se_src_clocks_str_0 [] =
{
    "scc_qupv3_se0_clk_src",
    "scc_qupv3_se1_clk_src",
    "scc_qupv3_se2_clk_src",
    "scc_qupv3_se3_clk_src",
    "scc_qupv3_se4_clk_src",
    "scc_qupv3_se5_clk_src",
};

plat_clock_config clk_cfg[] =
{
    // src-freq, speed, div, cycle, high, low
    {     19200,   100,   7,    26,   10,  11 },
    {     19200,   400,   2,    24,    5,  12 },
    {     19200,  1000,   1,    18,    3,   9 },
    {    100000,  8000,   2,     0,    3,   5 },
    {    120000, 12500,   1,     0,    8,   6 },
    {         0,     0,   0,     0,    0,   0 },
};

#define NUM_I2C_CORES_IN_USE (sizeof(i2c_device_config)/sizeof(plat_device_config))

static plat_device_config i2c_device_config[] =
{
#ifdef ENABLE_I2C_01
    {
        .core_base_addr              = (uint8 *) QUPV3_CORE_BASE_ADDRESS,
        .core_offset                 = 0x00080000,
        .qupv3_instance              = QUP_SSC,
        .core_index                  = 1,
        .core_irq                    = 86,
#ifdef POLL_MODE_BUS_DRIVERS
        .polled_mode                 = TRUE,
#else		
	    .polled_mode                 = FALSE,
#endif
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 0,

        .active_scl_encoding         = 0x1c001,
        .active_sda_encoding         = 0x1c011,
        .inactive_scl_encoding       = 0x00000,
        .inactive_sda_encoding       = 0x00010,
        .gpio_exclusive_access       = TRUE,

        .tcsr_base_addr              = (uint8 *) 0x01FC0000,
        .tcsr_reg_offset             = 0x00000000,
        .tcsr_reg_value              = 0x00000000,

        .common_clocks               = (uint8 **) common_clocks_str_0,
        .se_clock                    = (uint8 **) (se_clocks_str_0     + 0),
        .se_src_clock                = (uint8 **) (se_src_clocks_str_0 + 0),
        .resource_votes              = 0,

        .clock_config                = clk_cfg,
    },
#endif
#ifdef ENABLE_I2C_02
    {
        .core_base_addr              = (uint8 *) QUPV3_CORE_BASE_ADDRESS,
        .core_offset                 = 0x00084000,
        .qupv3_instance              = QUP_SSC,
        .core_index                  = 2,
        .core_irq                    = 87,
#ifdef POLL_MODE_BUS_DRIVERS
        .polled_mode                 = TRUE,
#else		
	    .polled_mode                 = FALSE,
#endif
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 1,

        .active_scl_encoding         = 0x1c021,
        .active_sda_encoding         = 0x1c031,
        .inactive_scl_encoding       = 0x00020,
        .inactive_sda_encoding       = 0x00030,
        .gpio_exclusive_access       = TRUE,

        .tcsr_base_addr              = (uint8 *) 0x01FC0000,
        .tcsr_reg_offset             = 0x00000000,
        .tcsr_reg_value              = 0x00000000,

        .common_clocks               = (uint8 **) common_clocks_str_0,
        .se_clock                    = (uint8 **) (se_clocks_str_0     + 1),
        .se_src_clock                = (uint8 **) (se_src_clocks_str_0 + 1),
        .resource_votes              = 0,

        .clock_config                = clk_cfg,
    },
#endif
#ifdef ENABLE_I2C_03
    {
        .core_base_addr              = (uint8 *) QUPV3_CORE_BASE_ADDRESS,
        .core_offset                 = 0x00088000,
        .qupv3_instance              = QUP_SSC,
        .core_index                  = 3,
        .core_irq                    = 88,
#ifdef POLL_MODE_BUS_DRIVERS
        .polled_mode                 = TRUE,
#else		
	    .polled_mode                 = FALSE,
#endif
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 2,

        .active_scl_encoding         = 0x1c081,
        .active_sda_encoding         = 0x1c091,
        .inactive_scl_encoding       = 0x00080,
        .inactive_sda_encoding       = 0x00090,
        .gpio_exclusive_access       = TRUE,

        .tcsr_base_addr              = (uint8 *) 0x01FC0000,
        .tcsr_reg_offset             = 0x00000000,
        .tcsr_reg_value              = 0x00000000,

        .common_clocks               = (uint8 **) common_clocks_str_0,
        .se_clock                    = (uint8 **) (se_clocks_str_0     + 2),
        .se_src_clock                = (uint8 **) (se_src_clocks_str_0 + 2),
        .resource_votes              = 0,

        .clock_config                = clk_cfg,
    },
#endif
#ifdef ENABLE_I2C_04
    {
        .core_base_addr              = (uint8 *) QUPV3_CORE_BASE_ADDRESS,
        .core_offset                 = 0x0008C000,
        .qupv3_instance              = QUP_SSC,
        .core_index                  = 4,
        .core_irq                    = 103,
#ifdef POLL_MODE_BUS_DRIVERS
        .polled_mode                 = TRUE,
#else		
	    .polled_mode                 = FALSE,
#endif
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 3,

        .active_scl_encoding         = 0x1c103,
        .active_sda_encoding         = 0x1c113,
        .inactive_scl_encoding       = 0x00100,
        .inactive_sda_encoding       = 0x00110,
        .gpio_exclusive_access       = TRUE,

        .tcsr_base_addr              = (uint8 *) 0x01FC0000,
        .tcsr_reg_offset             = 0x00000000,
        .tcsr_reg_value              = 0x00000000,

        .common_clocks               = (uint8 **) common_clocks_str_0,
        .se_clock                    = (uint8 **) (se_clocks_str_0     + 3),
        .se_src_clock                = (uint8 **) (se_src_clocks_str_0 + 3),
        .resource_votes              = 0,

        .clock_config                = clk_cfg,
    },
#endif
#ifdef ENABLE_I2C_05
    {
        .core_base_addr              = (uint8 *) QUPV3_CORE_BASE_ADDRESS,
        .core_offset                 = 0x00090000,
        .qupv3_instance              = QUP_SSC,
        .core_index                  = 5,
        .core_irq                    = 104,
#ifdef POLL_MODE_BUS_DRIVERS
        .polled_mode                 = TRUE,
#else		
	    .polled_mode                 = FALSE,
#endif
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 4,

        .active_scl_encoding         = 0x1c102,
        .active_sda_encoding         = 0x1c112,
        .inactive_scl_encoding       = 0x00100,
        .inactive_sda_encoding       = 0x00110,
        .gpio_exclusive_access       = TRUE,

        .tcsr_base_addr              = (uint8 *) 0x01FC0000,
        .tcsr_reg_offset             = 0x00000000,
        .tcsr_reg_value              = 0x00000000,

        .common_clocks               = (uint8 **) common_clocks_str_0,
        .se_clock                    = (uint8 **) (se_clocks_str_0     + 4),
        .se_src_clock                = (uint8 **) (se_src_clocks_str_0 + 4),
        .resource_votes              = 0,

        .clock_config                = clk_cfg,
    },
#endif
#ifdef ENABLE_I2C_06
    {
        .core_base_addr              = (uint8 *) QUPV3_CORE_BASE_ADDRESS,
        .core_offset                 = 0x00094000,
        .qupv3_instance              = QUP_SSC,
        .core_index                  = 6,
        .core_irq                    = 105,
#ifdef POLL_MODE_BUS_DRIVERS
        .polled_mode                 = TRUE,
#else		
	    .polled_mode                 = FALSE,
#endif
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 5,

        .active_scl_encoding         = 0x1c0e2,
        .active_sda_encoding         = 0x1c0f2,
        .inactive_scl_encoding       = 0x000e0,
        .inactive_sda_encoding       = 0x000f0,
        .gpio_exclusive_access       = TRUE,

        .tcsr_base_addr              = (uint8 *) 0x01FC0000,
        .tcsr_reg_offset             = 0x00000000,
        .tcsr_reg_value              = 0x00000000,

        .common_clocks               = (uint8 **) common_clocks_str_0,
        .se_clock                    = (uint8 **) (se_clocks_str_0     + 5),
        .se_src_clock                = (uint8 **) (se_src_clocks_str_0 + 5),
        .resource_votes              = 0,

        .clock_config                = clk_cfg,
    },
#endif
};

#define NUM_I2C_CLIENTS         16

bus_iface_hw_ctxt    bus_gpi_hw_ctxt [NUM_I2C_CORES_IN_USE];
hw_ctxt              bus_hw_ctxt     [NUM_I2C_CORES_IN_USE];
qurt_mutex_t         bus_crit_sec    [NUM_I2C_CORES_IN_USE];
qurt_mutex_t         bus_q_mutex     [NUM_I2C_CORES_IN_USE];
qurt_signal_t        bus_signal      [NUM_I2C_CORES_IN_USE + NUM_I2C_CLIENTS];
client_ctxt          bus_client_ctxt [NUM_I2C_CLIENTS];
bus_iface_io_ctxt    bus_gpi_io_ctxt [NUM_I2C_CLIENTS];

uint32 plat_get_num_initialized_cores (void)
{
    return NUM_I2C_CORES_IN_USE;
}

uint32 plat_get_num_clients (void)
{
    return NUM_I2C_CLIENTS;
}

void *plat_get_device_config_base (void)
{
    return (void *) i2c_device_config;
}
