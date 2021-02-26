/*=============================================================================   
    @file  i2c_devcfg.c
    @brief I2C Device Config data
   
    Copyright (c) 2017, 2019 Qualcomm Technologies, Incorporated.
                        All rights reserved.
    Qualcomm Technologies, Confidential and Proprietary.
===============================================================================*/

/*=============================================================================
                              EDIT HISTORY
 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 08/08/17   vg      Added common base addr 
 07/06/17   vg      Copyright updated 
=============================================================================*/

#include <i2c_config.h>
#include "gpi.h"


#define QUPV3_0_CORE_BASE_ADDRESS        0x00800000
#define QUPV3_0_CORE_COMMON_BASE_ADDRESS 0x008C0000
#define QUPV3_1_CORE_BASE_ADDRESS        0x00A00000
#define QUPV3_1_CORE_COMMON_BASE_ADDRESS 0x00AC0000

#define ENABLE_I2C_04
#define ENABLE_I2C_10
#define ENABLE_I2C_11

#define TLMM_GPIO_CFG(gpio, func, dir, pull, drive) \
                          (((gpio) & 0x3FF) << 4  | \
                           ((func) & 0xF  ) << 0  | \
                           ((dir)  & 0x1  ) << 14 | \
                           ((pull) & 0x3  ) << 15 | \
                           ((drive)& 0xF  ) << 17)

#define TLMM_GPIO_INPUT     0x0
#define TLMM_GPIO_PULL_UP   0x3
#define TLMM_GPIO_2MA       0x0

//
// CONFIGURATION START ============================================
//

// ag: need to check on uage
#define QUP_0	0x0
#define QUP_1	0x1


// set GPIOs for the defined cores
#define TOP_I2C_01_SCL TLMM_GPIO_CFG(0,  1, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_UP, TLMM_GPIO_2MA)
#define TOP_I2C_01_SDA TLMM_GPIO_CFG(1,  1, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_UP, TLMM_GPIO_2MA)

#define TOP_I2C_02_SCL TLMM_GPIO_CFG(17,  2, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_UP, TLMM_GPIO_2MA)
#define TOP_I2C_02_SDA TLMM_GPIO_CFG(18,  2, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_UP, TLMM_GPIO_2MA)

#define TOP_I2C_03_SCL TLMM_GPIO_CFG(27,  1, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_UP, TLMM_GPIO_2MA)
#define TOP_I2C_03_SDA TLMM_GPIO_CFG(28,  1, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_UP, TLMM_GPIO_2MA)

#define TOP_I2C_04_SCL TLMM_GPIO_CFG(41,  1, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_UP, TLMM_GPIO_2MA)
#define TOP_I2C_04_SDA TLMM_GPIO_CFG(42,  1, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_UP, TLMM_GPIO_2MA)

#define TOP_I2C_05_SCL TLMM_GPIO_CFG(89,  2, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_UP, TLMM_GPIO_2MA)
#define TOP_I2C_05_SDA TLMM_GPIO_CFG(90,  3, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_UP, TLMM_GPIO_2MA)

#define TOP_I2C_06_SCL TLMM_GPIO_CFG(85,  1, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_UP, TLMM_GPIO_2MA)
#define TOP_I2C_06_SDA TLMM_GPIO_CFG(86,  1, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_UP, TLMM_GPIO_2MA)

#define TOP_I2C_07_SCL TLMM_GPIO_CFG(45,  1, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_UP, TLMM_GPIO_2MA)
#define TOP_I2C_07_SDA TLMM_GPIO_CFG(46,  1, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_UP, TLMM_GPIO_2MA)

#define TOP_I2C_08_SCL TLMM_GPIO_CFG(93,  3, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_UP, TLMM_GPIO_2MA)
#define TOP_I2C_08_SDA TLMM_GPIO_CFG(94,  3, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_UP, TLMM_GPIO_2MA)

#define TOP_I2C_09_SCL TLMM_GPIO_CFG(65,  2, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_UP, TLMM_GPIO_2MA)
#define TOP_I2C_09_SDA TLMM_GPIO_CFG(66,  2, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_UP, TLMM_GPIO_2MA)

#define TOP_I2C_10_SCL TLMM_GPIO_CFG(6,  1, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_UP, TLMM_GPIO_2MA)
#define TOP_I2C_10_SDA TLMM_GPIO_CFG(7,  1, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_UP, TLMM_GPIO_2MA)

#define TOP_I2C_11_SCL TLMM_GPIO_CFG(55,  1, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_UP, TLMM_GPIO_2MA)
#define TOP_I2C_11_SDA TLMM_GPIO_CFG(56,  1, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_UP, TLMM_GPIO_2MA)

#define TOP_I2C_12_SCL TLMM_GPIO_CFG(31,  1, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_UP, TLMM_GPIO_2MA)
#define TOP_I2C_12_SDA TLMM_GPIO_CFG(32,  1, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_UP, TLMM_GPIO_2MA)

#define TOP_I2C_13_SCL TLMM_GPIO_CFG(49,  1, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_UP, TLMM_GPIO_2MA)
#define TOP_I2C_13_SDA TLMM_GPIO_CFG(50,  1, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_UP, TLMM_GPIO_2MA)

#define TOP_I2C_14_SCL TLMM_GPIO_CFG(105,  2, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_UP, TLMM_GPIO_2MA)
#define TOP_I2C_14_SDA TLMM_GPIO_CFG(106,  2, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_UP, TLMM_GPIO_2MA)

#define TOP_I2C_15_SCL TLMM_GPIO_CFG(33,  2, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_UP, TLMM_GPIO_2MA)
#define TOP_I2C_15_SDA TLMM_GPIO_CFG(34,  2, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_UP, TLMM_GPIO_2MA)

#define TOP_I2C_16_SCL TLMM_GPIO_CFG(81,  2, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_UP, TLMM_GPIO_2MA)
#define TOP_I2C_16_SDA TLMM_GPIO_CFG(82,  2, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_UP, TLMM_GPIO_2MA)


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
    {  19200,   100,   7,    26,   10,  11 },
    {  19200,   400,   2,    24,    5,  12 },
    {  19200,  1000,   1,    18,    3,   9 },
    {  19200,  3840,   1,     4,    8,   8 }, // RUMI I3C
    {  19200,  4000,   1,     4,    8,   8 }, // I3C open drain configuration
    {  0,         0,   0,     0,    0,   0 },
};


#ifdef ENABLE_I2C_01
plat_device_config i2c_device_config_01 = 
{
    .core_base_addr              = (uint8 *) QUPV3_0_CORE_BASE_ADDRESS,
	.common_base_addr            = (uint8 *) QUPV3_0_CORE_COMMON_BASE_ADDRESS,
    .core_offset                 = 0x00080000,
    .qupv3_instance              = QUP_0,
    .core_index                  = 1,
    .core_irq                    = 0,
    .polled_mode                 = TRUE,
    .min_data_length_for_dma     = 0,

    .gpi_index                   = 0,

    .scl_encoding                = TOP_I2C_01_SCL,
    .sda_encoding                = TOP_I2C_01_SDA,

    .tcsr_base_addr              = (uint8 *) 0x01FC0000,
    .tcsr_reg_offset             = 0x00000000,
    .tcsr_reg_value              = 0x00000000,

    .common_clocks               = (uint8 **) common_clocks_str_0,
    .se_clock                    = (uint8 **) (se_clocks_str_0 + 0),

    .clock_config                = clk_cfg,
};
#endif

#ifdef ENABLE_I2C_02
plat_device_config i2c_device_config_02 = 
{
    .core_base_addr              = (uint8 *) QUPV3_0_CORE_BASE_ADDRESS,
    .common_base_addr            = (uint8 *) QUPV3_0_CORE_COMMON_BASE_ADDRESS,
	.core_offset                 = 0x00084000,
    .qupv3_instance              = QUP_0,
    .core_index                  = 2,
    .core_irq                    = 0,
    .polled_mode                 = TRUE,
    .min_data_length_for_dma     = 0,

    .gpi_index                   = 1,

    .scl_encoding                = TOP_I2C_02_SCL,
    .sda_encoding                = TOP_I2C_02_SDA,

    .tcsr_base_addr              = (uint8 *) 0x01FC0000,
    .tcsr_reg_offset             = 0x00000000,
    .tcsr_reg_value              = 0x00000000,

    .common_clocks               = (uint8 **) common_clocks_str_0,
    .se_clock                    = (uint8 **) (se_clocks_str_0 + 1),

    .clock_config                = clk_cfg,
};
#endif

#ifdef ENABLE_I2C_03
plat_device_config i2c_device_config_03 = 
{
    .core_base_addr              = (uint8 *) QUPV3_0_CORE_BASE_ADDRESS,
	.common_base_addr            = (uint8 *) QUPV3_0_CORE_COMMON_BASE_ADDRESS,
    .core_offset                 = 0x00088000,
    .qupv3_instance              = QUP_0,
    .core_index                  = 3,
    .core_irq                    = 0,
    .polled_mode                 = TRUE,
    .min_data_length_for_dma     = 0,

    .gpi_index                   = 2,

    .scl_encoding                = TOP_I2C_03_SCL,
    .sda_encoding                = TOP_I2C_03_SDA,

    .tcsr_base_addr              = (uint8 *) 0x01FC0000,
    .tcsr_reg_offset             = 0x00000000,
    .tcsr_reg_value              = 0x00000000,

    .common_clocks               = (uint8 **) common_clocks_str_0,
    .se_clock                    = (uint8 **) (se_clocks_str_0 + 2),

    .clock_config                = clk_cfg,
};
#endif

#ifdef ENABLE_I2C_04
plat_device_config i2c_device_config_04 = 
{
    .core_base_addr              = (uint8 *) QUPV3_0_CORE_BASE_ADDRESS,
	.common_base_addr            = (uint8 *) QUPV3_0_CORE_COMMON_BASE_ADDRESS,
    .core_offset                 = 0x0008C000,
    .qupv3_instance              = QUP_0,
    .core_index                  = 4,
    .core_irq                    = 0,
    .polled_mode                 = TRUE,
    .min_data_length_for_dma     = 0,

    .gpi_index                   = 3,

    .scl_encoding                = TOP_I2C_04_SCL,
    .sda_encoding                = TOP_I2C_04_SDA,

    .tcsr_base_addr              = (uint8 *) 0x01FC0000,
    .tcsr_reg_offset             = 0x00000000,
    .tcsr_reg_value              = 0x00000000,

    .common_clocks               = (uint8 **) common_clocks_str_0,
    .se_clock                    = (uint8 **) (se_clocks_str_0 + 3),

    .clock_config                = clk_cfg,
};
#endif

#ifdef ENABLE_I2C_05
plat_device_config i2c_device_config_05 = 
{
    .core_base_addr              = (uint8 *) QUPV3_0_CORE_BASE_ADDRESS,
	.common_base_addr            = (uint8 *) QUPV3_0_CORE_COMMON_BASE_ADDRESS,
    .core_offset                 = 0x00090000,
    .qupv3_instance              = QUP_0,
    .core_index                  = 5,
    .core_irq                    = 0,
    .polled_mode                 = TRUE,
    .min_data_length_for_dma     = 0,

    .gpi_index                   = 4,

    .scl_encoding                = TOP_I2C_05_SCL,
    .sda_encoding                = TOP_I2C_05_SDA,

    .tcsr_base_addr              = (uint8 *) 0x01FC0000,
    .tcsr_reg_offset             = 0x00000000,
    .tcsr_reg_value              = 0x00000000,

    .common_clocks               = (uint8 **) common_clocks_str_0,
    .se_clock                    = (uint8 **) (se_clocks_str_0 + 4),

    .clock_config                = clk_cfg,
};
#endif

#ifdef ENABLE_I2C_06
plat_device_config i2c_device_config_06 = 
{
    .core_base_addr              = (uint8 *) QUPV3_0_CORE_BASE_ADDRESS,
	.common_base_addr            = (uint8 *) QUPV3_0_CORE_COMMON_BASE_ADDRESS,
    .core_offset                 = 0x00094000,
    .qupv3_instance              = QUP_0,
    .core_index                  = 6,
    .core_irq                    = 0,
    .polled_mode                 = TRUE,
    .min_data_length_for_dma     = 0,

    .gpi_index                   = 5,

    .scl_encoding                = TOP_I2C_06_SCL,
    .sda_encoding                = TOP_I2C_06_SDA,

    .tcsr_base_addr              = (uint8 *) 0x01FC0000,
    .tcsr_reg_offset             = 0x00000000,
    .tcsr_reg_value              = 0x00000000,

    .common_clocks               = (uint8 **) common_clocks_str_0,
    .se_clock                    = (uint8 **) (se_clocks_str_0 + 5),

    .clock_config                = clk_cfg,
};
#endif

#ifdef ENABLE_I2C_07
plat_device_config i2c_device_config_07 = 
{
    .core_base_addr              = (uint8 *) QUPV3_0_CORE_BASE_ADDRESS,
	.common_base_addr            = (uint8 *) QUPV3_0_CORE_COMMON_BASE_ADDRESS,
    .core_offset                 = 0x00098000,
    .qupv3_instance              = QUP_0,
    .core_index                  = 7,
    .core_irq                    = 0,
    .polled_mode                 = TRUE,
    .min_data_length_for_dma     = 0,

    .gpi_index                   = 6,

    .scl_encoding                = TOP_I2C_07_SCL,
    .sda_encoding                = TOP_I2C_07_SDA,

    .tcsr_base_addr              = (uint8 *) 0x01FC0000,
    .tcsr_reg_offset             = 0x00000000,
    .tcsr_reg_value              = 0x00000000,

    .common_clocks               = (uint8 **) common_clocks_str_0,
    .se_clock                    = (uint8 **) (se_clocks_str_0 + 6),

    .clock_config                = clk_cfg,
};
#endif

#ifdef ENABLE_I2C_08
plat_device_config i2c_device_config_08 = 
{
    .core_base_addr              = (uint8 *) QUPV3_0_CORE_BASE_ADDRESS,
	.common_base_addr            = (uint8 *) QUPV3_0_CORE_COMMON_BASE_ADDRESS,
    .core_offset                 = 0x0009C000,
    .qupv3_instance              = QUP_0,
    .core_index                  = 8,
    .core_irq                    = 0,
    .polled_mode                 = TRUE,
    .min_data_length_for_dma     = 0,

    .gpi_index                   = 7,

    .scl_encoding                = TOP_I2C_08_SCL,
    .sda_encoding                = TOP_I2C_08_SDA,

    .tcsr_base_addr              = (uint8 *) 0x01FC0000,
    .tcsr_reg_offset             = 0x00000000,
    .tcsr_reg_value              = 0x00000000,

    .common_clocks               = (uint8 **) common_clocks_str_0,
    .se_clock                    = (uint8 **) (se_clocks_str_0 + 7),

    .clock_config                = clk_cfg,
};
#endif

#ifdef ENABLE_I2C_09
plat_device_config i2c_device_config_09 = 
{
    .core_base_addr              = (uint8 *) QUPV3_1_CORE_BASE_ADDRESS,
	.common_base_addr            = (uint8 *) QUPV3_1_CORE_COMMON_BASE_ADDRESS,
    .core_offset                 = 0x00080000,
    .qupv3_instance              = QUP_1,
    .core_index                  = 9,
    .core_irq                    = 0,
    .polled_mode                 = TRUE,
    .min_data_length_for_dma     = 0,

    .gpi_index                   = 8,

    .scl_encoding                = TOP_I2C_09_SCL,
    .sda_encoding                = TOP_I2C_09_SDA,

    .tcsr_base_addr              = (uint8 *) 0x01FC0000,
    .tcsr_reg_offset             = 0x00000000,
    .tcsr_reg_value              = 0x00000000,

    .common_clocks               = (uint8 **) common_clocks_str_1,
    .se_clock                    = (uint8 **) (se_clocks_str_1 + 0),

    .clock_config                = clk_cfg,
};
#endif

#ifdef ENABLE_I2C_10
plat_device_config i2c_device_config_10 = 
{
    .core_base_addr              = (uint8 *) QUPV3_1_CORE_BASE_ADDRESS,
	.common_base_addr            = (uint8 *) QUPV3_1_CORE_COMMON_BASE_ADDRESS,
    .core_offset                 = 0x00084000,
    .qupv3_instance              = QUP_1,
    .core_index                  = 10,
    .core_irq                    = 0,
    .polled_mode                 = TRUE,
    .min_data_length_for_dma     = 0,

    .gpi_index                   = 9,

    .scl_encoding                = TOP_I2C_10_SCL,
    .sda_encoding                = TOP_I2C_10_SDA,

    .tcsr_base_addr              = (uint8 *) 0x01FC0000,
    .tcsr_reg_offset             = 0x00000000,
    .tcsr_reg_value              = 0x00000000,

    .common_clocks               = (uint8 **) common_clocks_str_1,
    .se_clock                    = (uint8 **) (se_clocks_str_1 + 1),

    .clock_config                = clk_cfg,
};
#endif

#ifdef ENABLE_I2C_11
plat_device_config i2c_device_config_11 = 
{
    .core_base_addr              = (uint8 *) QUPV3_1_CORE_BASE_ADDRESS,
	.common_base_addr            = (uint8 *) QUPV3_1_CORE_COMMON_BASE_ADDRESS,
    .core_offset                 = 0x00088000,
    .qupv3_instance              = QUP_1,
    .core_index                  = 11,
    .core_irq                    = 0,
    .polled_mode                 = TRUE,
    .min_data_length_for_dma     = 0,

    .gpi_index                   = 10,

    .scl_encoding                = TOP_I2C_11_SCL,
    .sda_encoding                = TOP_I2C_11_SDA,

    .tcsr_base_addr              = (uint8 *) 0x01FC0000,
    .tcsr_reg_offset             = 0x00000000,
    .tcsr_reg_value              = 0x00000000,

    .common_clocks               = (uint8 **) common_clocks_str_1,
    .se_clock                    = (uint8 **) (se_clocks_str_1 + 2),

    .clock_config                = clk_cfg,
};
#endif

#ifdef ENABLE_I2C_12
plat_device_config i2c_device_config_12 = 
{
    .core_base_addr              = (uint8 *) QUPV3_1_CORE_BASE_ADDRESS,
	.common_base_addr            = (uint8 *) QUPV3_1_CORE_COMMON_BASE_ADDRESS,	
    .core_offset                 = 0x0008C000,
    .qupv3_instance              = QUP_1,
    .core_index                  = 12,
    .core_irq                    = 0,
    .polled_mode                 = TRUE,
    .min_data_length_for_dma     = 0,

    .gpi_index                   = 11,

    .scl_encoding                = TOP_I2C_12_SCL,
    .sda_encoding                = TOP_I2C_12_SDA,

    .tcsr_base_addr              = (uint8 *) 0x01FC0000,
    .tcsr_reg_offset             = 0x00000000,
    .tcsr_reg_value              = 0x00000000,

    .common_clocks               = (uint8 **) common_clocks_str_1,
    .se_clock                    = (uint8 **) (se_clocks_str_1 + 3),

    .clock_config                = clk_cfg,
};
#endif

#ifdef ENABLE_I2C_13
plat_device_config i2c_device_config_13 = 
{
    .core_base_addr              = (uint8 *) QUPV3_1_CORE_BASE_ADDRESS,
	.common_base_addr            = (uint8 *) QUPV3_1_CORE_COMMON_BASE_ADDRESS,
    .core_offset                 = 0x00090000,
    .qupv3_instance              = QUP_1,
    .core_index                  = 13,
    .core_irq                    = 0,
    .polled_mode                 = TRUE,
    .min_data_length_for_dma     = 0,

    .gpi_index                   = 12,

    .scl_encoding                = TOP_I2C_13_SCL,
    .sda_encoding                = TOP_I2C_13_SDA,

    .tcsr_base_addr              = (uint8 *) 0x01FC0000,
    .tcsr_reg_offset             = 0x00000000,
    .tcsr_reg_value              = 0x00000000,

    .common_clocks               = (uint8 **) common_clocks_str_1,
    .se_clock                    = (uint8 **) (se_clocks_str_1 + 4),

    .clock_config                = clk_cfg,
};
#endif

#ifdef ENABLE_I2C_14
plat_device_config i2c_device_config_14 = 
{
    .core_base_addr              = (uint8 *) QUPV3_1_CORE_BASE_ADDRESS,
	.common_base_addr            = (uint8 *) QUPV3_1_CORE_COMMON_BASE_ADDRESS,
    .core_offset                 = 0x00094000,
    .qupv3_instance              = QUP_1,
    .core_index                  = 14,
    .core_irq                    = 0,
    .polled_mode                 = TRUE,
    .min_data_length_for_dma     = 0,

    .gpi_index                   = 13,

    .scl_encoding                = TOP_I2C_14_SCL,
    .sda_encoding                = TOP_I2C_14_SDA,

    .tcsr_base_addr              = (uint8 *) 0x01FC0000,
    .tcsr_reg_offset             = 0x00000000,
    .tcsr_reg_value              = 0x00000000,

    .common_clocks               = (uint8 **) common_clocks_str_1,
    .se_clock                    = (uint8 **) (se_clocks_str_1 + 5),

    .clock_config                = clk_cfg,
};
#endif

#ifdef ENABLE_I2C_15
plat_device_config i2c_device_config_15 = 
{
    .core_base_addr              = (uint8 *) QUPV3_1_CORE_BASE_ADDRESS,
	.common_base_addr            = (uint8 *) QUPV3_1_CORE_COMMON_BASE_ADDRESS,
    .core_offset                 = 0x00098000,
    .qupv3_instance              = QUP_1,
    .core_index                  = 15,
    .core_irq                    = 0,
    .polled_mode                 = TRUE,
    .min_data_length_for_dma     = 0,

    .gpi_index                   = 14,

    .scl_encoding                = TOP_I2C_15_SCL,
    .sda_encoding                = TOP_I2C_15_SDA,

    .tcsr_base_addr              = (uint8 *) 0x01FC0000,
    .tcsr_reg_offset             = 0x00000000,
    .tcsr_reg_value              = 0x00000000,

    .common_clocks               = (uint8 **) common_clocks_str_1,
    .se_clock                    = (uint8 **) (se_clocks_str_1 + 6),

    .clock_config                = clk_cfg,
};
#endif

#ifdef ENABLE_I2C_16
plat_device_config i2c_device_config_16 = 
{
    .core_base_addr              = (uint8 *) QUPV3_1_CORE_BASE_ADDRESS,
	.common_base_addr            = (uint8 *) QUPV3_1_CORE_COMMON_BASE_ADDRESS,
    .core_offset                 = 0x0009C000,
    .qupv3_instance              = QUP_1,
    .core_index                  = 16,
    .core_irq                    = 0,
    .polled_mode                 = TRUE,
    .min_data_length_for_dma     = 0,

    .gpi_index                   = 15,

    .scl_encoding                = TOP_I2C_16_SCL,
    .sda_encoding                = TOP_I2C_16_SDA,

    .tcsr_base_addr              = (uint8 *) 0x01FC0000,
    .tcsr_reg_offset             = 0x00000000,
    .tcsr_reg_value              = 0x00000000,

    .common_clocks               = (uint8 **) common_clocks_str_1,
    .se_clock                    = (uint8 **) (se_clocks_str_1 + 7),

    .clock_config                = clk_cfg,
};
#endif





