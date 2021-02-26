/*
===========================================================================

FILE:         Slimbus_adsp_670.c

DESCRIPTION:  This file implements the SLIMbus board support data.

===========================================================================

                             Edit History


when       who     what, where, why
--------   ---     -------------------------------------------------------- 
09/15/17   DPK     Header file inclusion for the uTLMM API's.
08/31/17   DPK     Initial revision for Warlock ADSP.

===========================================================================
             Copyright (c) 2017 QUALCOMM Technologies, Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================
*/

#include "TlmmDefs.h"
#include "mmpm.h"
#include "SlimBusBsp.h"
#include "uTlmm.h"

/* Slimbus BSP data */
SlimBusBSPType SlimBusBSP[] =
{
  {
    4,
    "SLIMBUS",
    { 0x00, 0x00, 0x90, 0x02, 0x17, 0x02 },
    "LPASS",
    0x00DC0000,
    0x62DC0000,
    0x62D84000,
    11,
    12,
    0,
    { UTLMM_GPIO_CFG(18, 1, DAL_GPIO_INPUT, DAL_GPIO_KEEPER, DAL_GPIO_2MA),
      UTLMM_GPIO_CFG(20, 1, DAL_GPIO_INPUT, DAL_GPIO_KEEPER, DAL_GPIO_4MA),
      UTLMM_GPIO_CFG(21, 1, DAL_GPIO_INPUT, DAL_GPIO_KEEPER, DAL_GPIO_4MA)},
    20,
    { 4, 4, 4, 4, 4, 4, 4, 4, 2, 2, 2, 2, 2, 2, 2, 2 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2 },  /* Clock gears 0 through 10 */
    1
  }
};

const SlimBusDeviceDalProps sbDeviceProps[] = 
{
    {0xc0, {0x00, 0x00, 0x90, 0x02, 0x17, 0x02}, 0x01},
    {0xc1, {0x00, 0x01, 0x90, 0x02, 0x17, 0x02}, 0x01},
    {0xc2, {0x00, 0x03, 0x90, 0x02, 0x17, 0x02}, 0x01},
    {0xc3, {0x00, 0x04, 0x90, 0x02, 0x17, 0x02}, 0x01},
    {0xc4, {0x00, 0x05, 0x90, 0x02, 0x17, 0x02}, 0x03},
    {0xca, {0x00, 0x00, 0xa0, 0x01, 0x17, 0x02}, 0x01},
    {0xcb, {0x00, 0x01, 0xa0, 0x01, 0x17, 0x02}, 0x03},
    {0xcc, {0x00, 0x00, 0x30, 0x01, 0x17, 0x02}, 0x01},
    {0xcd, {0x00, 0x01, 0x30, 0x01, 0x17, 0x02}, 0x01},
    {0xce, {0x00, 0x00, 0x50, 0x02, 0x17, 0x02}, 0x01},
    {0xcf, {0x00, 0x01, 0x50, 0x02, 0x17, 0x02}, 0x03}
};

const uint32 sbNumDeviceProps = sizeof(sbDeviceProps) / sizeof(SlimBusDeviceDalProps);

/* Slimbus BSP data */
SlimBusBSPType SlimBusBSP2[] =
{
  {
    4,
    "SLIMBUS_QCA",
    { 0x01, 0x00, 0x90, 0x02, 0x17, 0x02 },
    "LPASS",
    0x00E40000,
    0x62E40000,
    0x62E04000,
    13,
    14,
    0,
    { UTLMM_GPIO_CFG(31, 1, DAL_GPIO_INPUT, DAL_GPIO_KEEPER, DAL_GPIO_2MA),
      UTLMM_GPIO_CFG(30, 1, DAL_GPIO_INPUT, DAL_GPIO_KEEPER, DAL_GPIO_4MA) },
    30,
    { 2, 2, 2, 2, 2, 2 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2 },  /* Clock gears 0 through 10 */
    1
  }
};

const SlimBusDeviceDalProps sbDeviceProps2[] = 
{
    {0xc5, {0x01, 0x00, 0x90, 0x02, 0x17, 0x02}, 0x01},
    {0xc6, {0x01, 0x01, 0x90, 0x02, 0x17, 0x02}, 0x01},
    {0xc7, {0x01, 0x03, 0x90, 0x02, 0x17, 0x02}, 0x01},
    {0xc8, {0x01, 0x04, 0x90, 0x02, 0x17, 0x02}, 0x01},
    {0xc9, {0x01, 0x05, 0x90, 0x02, 0x17, 0x02}, 0x01},
    {0xce, {0x00, 0x00, 0x20, 0x02, 0x17, 0x02}, 0x01},
    {0xcf, {0x00, 0x01, 0x20, 0x02, 0x17, 0x02}, 0x01}
};

const uint32 sbNumDeviceProps2 = sizeof(sbDeviceProps2) / sizeof(SlimBusDeviceDalProps);

const MmpmRegParamType sbMmpmRegParam = 
{
  MMPM_REVISION,
  MMPM_CORE_ID_LPASS_SLIMBUS,
  MMPM_CORE_INSTANCE_0,
  "slimbus",
  PWR_CTRL_NONE,
  CALLBACK_NONE,
  NULL,
  0
};


const MmpmRegParamType sbMmpmRegParam2 = 
{
  MMPM_REVISION,
  MMPM_CORE_ID_LPASS_SLIMBUS,
  MMPM_CORE_INSTANCE_1,
  "slimbus_qca",
  PWR_CTRL_NONE,
  CALLBACK_NONE,
  NULL,
  0
};

