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

#define ENABLE_I2C_15

#define QUPV3_0_CORE_BASE_ADDRESS 0x00800000
#define QUPV3_1_CORE_BASE_ADDRESS 0x00A00000

typedef struct num_cores { uint32 num; } num_cores;

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

plat_device_config i2c_device_config[] =
{
#ifdef ENABLE_I2C_01
    {
        .core_base_addr              = (uint8 *) QUPV3_0_CORE_BASE_ADDRESS,
        .core_offset                 = 0x00080000,
        .qupv3_instance              = 0,
        .core_index                  = 1,
        .core_irq                    = 633,
        .polled_mode                 = TRUE,
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 0,

        .sda_encoding                = 0x00018001,
        .scl_encoding                = 0x00018011,

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
        .qupv3_instance              = 0,
        .core_index                  = 2,
        .core_irq                    = 634,
        .polled_mode                 = TRUE,
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 1,

        .sda_encoding                = 0x00018112,
        .scl_encoding                = 0x00018122,

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
        .qupv3_instance              = 0,
        .core_index                  = 3,
        .core_irq                    = 635,
        .polled_mode                 = TRUE,
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 2,

        .sda_encoding                = 0x000181B1,
        .scl_encoding                = 0x000181C1,

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
        .qupv3_instance              = 0,
        .core_index                  = 4,
        .core_irq                    = 636,
        .polled_mode                 = TRUE,
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 3,

        .sda_encoding                = 0x00018291,
        .scl_encoding                = 0x00018301,

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
        .qupv3_instance              = 0,
        .core_index                  = 5,
        .core_irq                    = 637,
        .polled_mode                 = TRUE,
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 4,

        .sda_encoding                = 0x00018592,
        .scl_encoding                = 0x000185A3,

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
        .qupv3_instance              = 0,
        .core_index                  = 6,
        .core_irq                    = 638,
        .polled_mode                 = TRUE,
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 5,

        .sda_encoding                = 0x00018551,
        .scl_encoding                = 0x00018561,

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
        .qupv3_instance              = 0,
        .core_index                  = 7,
        .core_irq                    = 639,
        .polled_mode                 = TRUE,
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 6,

        .sda_encoding                = 0x000182D1,
        .scl_encoding                = 0x000182E1,

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
        .qupv3_instance              = 0,
        .core_index                  = 8,
        .core_irq                    = 640,
        .polled_mode                 = TRUE,
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 7,

        .sda_encoding                = 0x000185D3,
        .scl_encoding                = 0x000185E3,

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
        .qupv3_instance              = 1,
        .core_index                  = 9,
        .core_irq                    = 385,
        .polled_mode                 = TRUE,
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 8,

        .sda_encoding                = 0x00018412,
        .scl_encoding                = 0x00018422,

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
        .qupv3_instance              = 1,
        .core_index                  = 10,
        .core_irq                    = 386,
        .polled_mode                 = TRUE,
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 8,

        .sda_encoding                = 0x00018041,
        .scl_encoding                = 0x00018051,

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
        .qupv3_instance              = 1,
        .core_index                  = 11,
        .core_irq                    = 387,
        .polled_mode                 = TRUE,
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 10,

        .sda_encoding                = 0x00018371,
        .scl_encoding                = 0x00018381,

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
        .qupv3_instance              = 1,
        .core_index                  = 12,
        .core_irq                    = 388,
        .polled_mode                 = TRUE,
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 11,

        .sda_encoding                = 0x000181F1,
        .scl_encoding                = 0x00018201,

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
        .qupv3_instance              = 1,
        .core_index                  = 13,
        .core_irq                    = 389,
        .polled_mode                 = TRUE,
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 12,

        .sda_encoding                = 0x00018311,
        .scl_encoding                = 0x00018321,

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
        .qupv3_instance              = 1,
        .core_index                  = 14,
        .core_irq                    = 390,
        .polled_mode                 = TRUE,
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 13,

        .sda_encoding                = 0x00018692,
        .scl_encoding                = 0x000186A2,

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
        .qupv3_instance              = 1,
        .core_index                  = 15,
        .core_irq                    = 391,
        .polled_mode                 = TRUE,
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 14,

        .sda_encoding                = 0x00018312,
        .scl_encoding                = 0x00018322,

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
        .qupv3_instance              = 1,
        .core_index                  = 16,
        .core_irq                    = 392,
        .polled_mode                 = TRUE,
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 15,

        .sda_encoding                = 0x00018512,
        .scl_encoding                = 0x00018522,

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

num_cores num_i2c_cores = { NUM_I2C_CORES_IN_USE };
