/**
    @file  fw_devcfg.c
    @brief device firmware settings
 */
/*=============================================================================
          Copyright (c) 2017 Qualcomm Technologies, Incorporated.
                            All rights reserved.
            Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

#include "fw_devcfg.h"
#include "msmhwiobase.h"

#define SSC_QUPV3_GSI_TOP (LPASS_BASE + 0x00600000)

#define SE0_PROTOCOL SE_PROTOCOL_I2C
#define SE1_PROTOCOL SE_PROTOCOL_SPI
#define SE2_PROTOCOL SE_PROTOCOL_SPI
#define SE3_PROTOCOL SE_PROTOCOL_UART
#define SE4_PROTOCOL SE_PROTOCOL_UART
#define SE5_PROTOCOL SE_PROTOCOL_I2C

qup_cfg ssc_qup_cfg =
{
    .qup_base       = (uint8 *) SSC_QUPV3_GSI_TOP,
    .common_offset  = 0xC0000,
    .load_fw        = TRUE,
};

se_cfg se0_cfg =
{
    .offset     = 0x80000, 
    .protocol   = SE0_PROTOCOL,
    .mode       = GSI,
    .load_fw    = TRUE,
    .dfs_mode   = TRUE,
};

se_cfg se1_cfg =
{
    .offset     = 0x84000,
    .protocol   = SE1_PROTOCOL,
    .mode       = GSI,
    .load_fw    = TRUE,
    .dfs_mode   = TRUE,
};

se_cfg se2_cfg =
{
    .offset     = 0x88000, 
    .protocol   = SE2_PROTOCOL,
    .mode       = GSI,
    .load_fw    = TRUE,
    .dfs_mode   = TRUE,
};

se_cfg se3_cfg =
{
    .offset     = 0x8C000, 
    .protocol   = SE3_PROTOCOL,
    .mode       = FIFO,
    .load_fw    = TRUE,
    .dfs_mode   = FALSE,
};

se_cfg se4_cfg =
{
    .offset     = 0x90000, 
    .protocol   = SE4_PROTOCOL,
    .mode       = FIFO,
    .load_fw    = TRUE,
    .dfs_mode   = FALSE,
};

se_cfg se5_cfg =
{
    .offset     = 0x94000, 
    .protocol   = SE5_PROTOCOL,
    .mode       = GSI,
    .load_fw    = FALSE,
    .dfs_mode   = FALSE,
};
