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

#define QUPV3_0_CORE_BASE_ADDRESS 0x00800000
#define QUPV3_1_CORE_BASE_ADDRESS 0x00A00000

const char *common_clocks_str_0 [] =
{
    "gcc_qupv3_wrap0_core_2x_clk",
    "gcc_qupv3_wrap0_core_clk",
    "gcc_qupv3_wrap_0_s_ahb_clk",
    "gcc_qupv3_wrap_0_m_ahb_clk",
    NULL,
};

const char *common_clocks_str_1 [] =
{
    "gcc_qupv3_wrap1_core_2x_clk",
    "gcc_qupv3_wrap1_core_clk",
    "gcc_qupv3_wrap_1_s_ahb_clk",
    "gcc_qupv3_wrap_1_m_ahb_clk",
    NULL,
};

const char *se_clocks_str_0 [] =
{
    "gcc_qupv3_wrap0_s0_clk",
    "gcc_qupv3_wrap0_s1_clk",
    "gcc_qupv3_wrap0_s2_clk",
    "gcc_qupv3_wrap0_s3_clk",
    "gcc_qupv3_wrap0_s4_clk",
    "gcc_qupv3_wrap0_s5_clk",
    "gcc_qupv3_wrap0_s6_clk",
    "gcc_qupv3_wrap0_s7_clk",
};

const char *se_clocks_str_1 [] =
{
    "gcc_qupv3_wrap1_s0_clk",
    "gcc_qupv3_wrap1_s1_clk",
    "gcc_qupv3_wrap1_s2_clk",
    "gcc_qupv3_wrap1_s3_clk",
    "gcc_qupv3_wrap1_s4_clk",
    "gcc_qupv3_wrap1_s5_clk",
    "gcc_qupv3_wrap1_s6_clk",
    "gcc_qupv3_wrap1_s7_clk",
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
        .core_base_addr              = (uint8 *) QUPV3_0_CORE_BASE_ADDRESS,
        .core_offset                 = 0x00080000,
        .qupv3_instance              = QUP_0,
        .core_index                  = 1,
        .core_irq                    = 0,
        .polled_mode                 = FALSE,
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 0,

        .scl_encoding                = 0x00018001,
        .sda_encoding                = 0x00018011,

        .tcsr_base_addr              = (uint8 *) 0x01FC0000,
        .tcsr_reg_offset             = 0x00000000,
        .tcsr_reg_value              = 0x00000000,

        .common_clocks               = (uint8 **) common_clocks_str_0,
        .se_clock                    = (uint8 **) (se_clocks_str_0 + 0),
        .se_src_clock                = NULL,
        .resource_votes              = 0,

        .clock_config                = clk_cfg,
    },
#endif
#ifdef ENABLE_I2C_02
    {
        .core_base_addr              = (uint8 *) QUPV3_0_CORE_BASE_ADDRESS,
        .core_offset                 = 0x00084000,
        .qupv3_instance              = QUP_0,
        .core_index                  = 2,
        .core_irq                    = 0,
        .polled_mode                 = FALSE,
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 1,

        .scl_encoding                = 0x00018112,
        .sda_encoding                = 0x00018122,

        .tcsr_base_addr              = (uint8 *) 0x01FC0000,
        .tcsr_reg_offset             = 0x00000000,
        .tcsr_reg_value              = 0x00000000,

        .common_clocks               = (uint8 **) common_clocks_str_0,
        .se_clock                    = (uint8 **) (se_clocks_str_0 + 1),
        .se_src_clock                = NULL,
        .resource_votes              = 0,

        .clock_config                = clk_cfg,
    },
#endif
#ifdef ENABLE_I2C_03
    {
        .core_base_addr              = (uint8 *) QUPV3_0_CORE_BASE_ADDRESS,
        .core_offset                 = 0x00088000,
        .qupv3_instance              = QUP_0,
        .core_index                  = 3,
        .core_irq                    = 0,
        .polled_mode                 = FALSE,
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 2,

        .scl_encoding                = 0x000181B1,
        .sda_encoding                = 0x000181C1,

        .tcsr_base_addr              = (uint8 *) 0x01FC0000,
        .tcsr_reg_offset             = 0x00000000,
        .tcsr_reg_value              = 0x00000000,

        .common_clocks               = (uint8 **) common_clocks_str_0,
        .se_clock                    = (uint8 **) (se_clocks_str_0 + 2),
        .se_src_clock                = NULL,
        .resource_votes              = 0,

        .clock_config                = clk_cfg,
    },
#endif
#ifdef ENABLE_I2C_04
    {
        .core_base_addr              = (uint8 *) QUPV3_0_CORE_BASE_ADDRESS,
        .core_offset                 = 0x0008C000,
        .qupv3_instance              = QUP_0,
        .core_index                  = 4,
        .core_irq                    = 0,
        .polled_mode                 = FALSE,
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 3,

        .scl_encoding                = 0x00018291,
        .sda_encoding                = 0x00018301,

        .tcsr_base_addr              = (uint8 *) 0x01FC0000,
        .tcsr_reg_offset             = 0x00000000,
        .tcsr_reg_value              = 0x00000000,

        .common_clocks               = (uint8 **) common_clocks_str_0,
        .se_clock                    = (uint8 **) (se_clocks_str_0 + 3),
        .se_src_clock                = NULL,
        .resource_votes              = 0,

        .clock_config                = clk_cfg,
    },
#endif
#ifdef ENABLE_I2C_05
    {
        .core_base_addr              = (uint8 *) QUPV3_0_CORE_BASE_ADDRESS,
        .core_offset                 = 0x00090000,
        .qupv3_instance              = QUP_0,
        .core_index                  = 5,
        .core_irq                    = 0,
        .polled_mode                 = FALSE,
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 4,

        .scl_encoding                = 0x00018592,
        .sda_encoding                = 0x000185A3,

        .tcsr_base_addr              = (uint8 *) 0x01FC0000,
        .tcsr_reg_offset             = 0x00000000,
        .tcsr_reg_value              = 0x00000000,

        .common_clocks               = (uint8 **) common_clocks_str_0,
        .se_clock                    = (uint8 **) (se_clocks_str_0 + 4),
        .se_src_clock                = NULL,
        .resource_votes              = 0,

        .clock_config                = clk_cfg,
    },
#endif

#ifdef ENABLE_I2C_06
    {
        .core_base_addr              = (uint8 *) QUPV3_0_CORE_BASE_ADDRESS,
        .core_offset                 = 0x00094000,
        .qupv3_instance              = QUP_0,
        .core_index                  = 6,
        .core_irq                    = 0,
        .polled_mode                 = FALSE,
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 5,

        .scl_encoding                = 0x00018551,
        .sda_encoding                = 0x00018561,

        .tcsr_base_addr              = (uint8 *) 0x01FC0000,
        .tcsr_reg_offset             = 0x00000000,
        .tcsr_reg_value              = 0x00000000,

        .common_clocks               = (uint8 **) common_clocks_str_0,
        .se_clock                    = (uint8 **) (se_clocks_str_0 + 5),
        .se_src_clock                = NULL,
        .resource_votes              = 0,

        .clock_config                = clk_cfg,
    },
#endif

#ifdef ENABLE_I2C_07
    {
        .core_base_addr              = (uint8 *) QUPV3_0_CORE_BASE_ADDRESS,
        .core_offset                 = 0x00098000,
        .qupv3_instance              = QUP_0,
        .core_index                  = 7,
        .core_irq                    = 0,
        .polled_mode                 = FALSE,
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 6,

        .scl_encoding                = 0x000182D1,
        .sda_encoding                = 0x000182E1,

        .tcsr_base_addr              = (uint8 *) 0x01FC0000,
        .tcsr_reg_offset             = 0x00000000,
        .tcsr_reg_value              = 0x00000000,

        .common_clocks               = (uint8 **) common_clocks_str_0,
        .se_clock                    = (uint8 **) (se_clocks_str_0 + 6),
        .se_src_clock                = NULL,
        .resource_votes              = 0,

        .clock_config                = clk_cfg,
    },
#endif

#ifdef ENABLE_I2C_08
    {
        .core_base_addr              = (uint8 *) QUPV3_0_CORE_BASE_ADDRESS,
        .core_offset                 = 0x0009C000,
        .qupv3_instance              = QUP_0,
        .core_index                  = 8,
        .core_irq                    = 0,
        .polled_mode                 = FALSE,
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 7,

        .scl_encoding                = 0x000185D3,
        .sda_encoding                = 0x000185E3,

        .tcsr_base_addr              = (uint8 *) 0x01FC0000,
        .tcsr_reg_offset             = 0x00000000,
        .tcsr_reg_value              = 0x00000000,

        .common_clocks               = (uint8 **) common_clocks_str_0,
        .se_clock                    = (uint8 **) (se_clocks_str_0 + 7),
        .se_src_clock                = NULL,
        .resource_votes              = 0,

        .clock_config                = clk_cfg,
    },
#endif

#ifdef ENABLE_I2C_09
    {
        .core_base_addr              = (uint8 *) QUPV3_1_CORE_BASE_ADDRESS,
        .core_offset                 = 0x00080000,
        .qupv3_instance              = QUP_1,
        .core_index                  = 9,
        .core_irq                    = 0,
        .polled_mode                 = FALSE,
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 8,

        .scl_encoding                = 0x00018412,
        .sda_encoding                = 0x00018422,

        .tcsr_base_addr              = (uint8 *) 0x01FC0000,
        .tcsr_reg_offset             = 0x00000000,
        .tcsr_reg_value              = 0x00000000,

        .common_clocks               = (uint8 **) common_clocks_str_1,
        .se_clock                    = (uint8 **) (se_clocks_str_1 + 0),
        .se_src_clock                = NULL,
        .resource_votes              = 0,

        .clock_config                = clk_cfg,
    },
#endif

#ifdef ENABLE_I2C_10
    {
        .core_base_addr              = (uint8 *) QUPV3_1_CORE_BASE_ADDRESS,
        .core_offset                 = 0x00084000,
        .qupv3_instance              = QUP_1,
        .core_index                  = 10,
        .core_irq                    = 0,
        .polled_mode                 = FALSE,
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 9,

        .scl_encoding                = 0x00018041,
        .sda_encoding                = 0x00018051,

        .tcsr_base_addr              = (uint8 *) 0x01FC0000,
        .tcsr_reg_offset             = 0x00000000,
        .tcsr_reg_value              = 0x00000000,

        .common_clocks               = (uint8 **) common_clocks_str_1,
        .se_clock                    = (uint8 **) (se_clocks_str_1 + 1),
        .se_src_clock                = NULL,
        .resource_votes              = 0,

        .clock_config                = clk_cfg,
    },
#endif

#ifdef ENABLE_I2C_11
    {
        .core_base_addr              = (uint8 *) QUPV3_1_CORE_BASE_ADDRESS,
        .core_offset                 = 0x00088000,
        .qupv3_instance              = QUP_1,
        .core_index                  = 11,
        .core_irq                    = 0,
        .polled_mode                 = FALSE,
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 10,

        .scl_encoding                = 0x00018351,
        .sda_encoding                = 0x00018361,

        .tcsr_base_addr              = (uint8 *) 0x01FC0000,
        .tcsr_reg_offset             = 0x00000000,
        .tcsr_reg_value              = 0x00000000,

        .common_clocks               = (uint8 **) common_clocks_str_1,
        .se_clock                    = (uint8 **) (se_clocks_str_1 + 2),
        .se_src_clock                = NULL,
        .resource_votes              = 0,

        .clock_config                = clk_cfg,
    },
#endif

#ifdef ENABLE_I2C_12
    {
        .core_base_addr              = (uint8 *) QUPV3_1_CORE_BASE_ADDRESS,
        .core_offset                 = 0x0008C000,
        .qupv3_instance              = QUP_1,
        .core_index                  = 12,
        .core_irq                    = 0,
        .polled_mode                 = FALSE,
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 11,

        .scl_encoding                = 0x000181F1,
        .sda_encoding                = 0x00018201,

        .tcsr_base_addr              = (uint8 *) 0x01FC0000,
        .tcsr_reg_offset             = 0x00000000,
        .tcsr_reg_value              = 0x00000000,

        .common_clocks               = (uint8 **) common_clocks_str_1,
        .se_clock                    = (uint8 **) (se_clocks_str_1 + 3),
        .se_src_clock                = NULL,
        .resource_votes              = 0,

        .clock_config                = clk_cfg,
    },
#endif

#ifdef ENABLE_I2C_13
    {
        .core_base_addr              = (uint8 *) QUPV3_1_CORE_BASE_ADDRESS,
        .core_offset                 = 0x00090000,
        .qupv3_instance              = QUP_1,
        .core_index                  = 13,
        .core_irq                    = 0,
        .polled_mode                 = FALSE,
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 12,

        .scl_encoding                = 0x00018311,
        .sda_encoding                = 0x00018321,

        .tcsr_base_addr              = (uint8 *) 0x01FC0000,
        .tcsr_reg_offset             = 0x00000000,
        .tcsr_reg_value              = 0x00000000,

        .common_clocks               = (uint8 **) common_clocks_str_1,
        .se_clock                    = (uint8 **) (se_clocks_str_1 + 4),
        .se_src_clock                = NULL,
        .resource_votes              = 0,

        .clock_config                = clk_cfg,
    },
#endif

#ifdef ENABLE_I2C_14
    {
        .core_base_addr              = (uint8 *) QUPV3_1_CORE_BASE_ADDRESS,
        .core_offset                 = 0x00094000,
        .qupv3_instance              = QUP_1,
        .core_index                  = 14,
        .core_irq                    = 0,
        .polled_mode                 = FALSE,
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 13,

        .scl_encoding                = 0x00018692,
        .sda_encoding                = 0x000186A2,

        .tcsr_base_addr              = (uint8 *) 0x01FC0000,
        .tcsr_reg_offset             = 0x00000000,
        .tcsr_reg_value              = 0x00000000,

        .common_clocks               = (uint8 **) common_clocks_str_1,
        .se_clock                    = (uint8 **) (se_clocks_str_1 + 5),
        .se_src_clock                = NULL,
        .resource_votes              = 0,

        .clock_config                = clk_cfg,
    },
#endif

#ifdef ENABLE_I2C_15
    {
        .core_base_addr              = (uint8 *) QUPV3_1_CORE_BASE_ADDRESS,
        .core_offset                 = 0x00098000,
        .qupv3_instance              = QUP_1,
        .core_index                  = 15,
        .core_irq                    = 0,
        .polled_mode                 = FALSE,
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 14,

        .scl_encoding                = 0x00018312,
        .sda_encoding                = 0x00018322,

        .tcsr_base_addr              = (uint8 *) 0x01FC0000,
        .tcsr_reg_offset             = 0x00000000,
        .tcsr_reg_value              = 0x00000000,

        .common_clocks               = (uint8 **) common_clocks_str_1,
        .se_clock                    = (uint8 **) (se_clocks_str_1 + 6),
        .se_src_clock                = NULL,
        .resource_votes              = 0,

        .clock_config                = clk_cfg,
    },
#endif

#ifdef ENABLE_I2C_16
    {
        .core_base_addr              = (uint8 *) QUPV3_1_CORE_BASE_ADDRESS,
        .core_offset                 = 0x0009C000,
        .qupv3_instance              = QUP_1,
        .core_index                  = 16,
        .core_irq                    = 0,
        .polled_mode                 = FALSE,
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 15,

        .scl_encoding                = 0x00018512,
        .sda_encoding                = 0x00018522,

        .tcsr_base_addr              = (uint8 *) 0x01FC0000,
        .tcsr_reg_offset             = 0x00000000,
        .tcsr_reg_value              = 0x00000000,

        .common_clocks               = (uint8 **) common_clocks_str_1,
        .se_clock                    = (uint8 **) (se_clocks_str_1 + 7),
        .se_src_clock                = NULL,
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
