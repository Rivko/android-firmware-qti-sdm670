/*
===========================================================================

FILE:         slimbus_bsp_data.c

DESCRIPTION:  This file implements the SLIMbus board support data.

===========================================================================

                             Edit History

$Header: //components/rel/core.qdsp6/2.1.c4/settings/buses/slimbus/config/slimbus_adsp_830.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------- 
08/11/16   MJS     Add configurable SVS levels.
07/27/16   MJS     Initial revision for Napali ADSP.

===========================================================================
             Copyright (c) 2016 QUALCOMM Technologies, Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================
*/

#include "TlmmDefs.h"
#include "mmpm.h"
#include "SlimBusBsp.h"


/* Slimbus BSP data */
SlimBusBSPType SlimBusBSP[] =
{
  {
    4,
    "SLIMBUS",
    { 0x00, 0x00, 0x70, 0x02, 0x17, 0x02 },
    "LPASS",
    0x001c0000,
    0x171c0000,
    0x17184000,
    11,
    12,
    0,
    { DAL_GPIO_CFG(70, 1, DAL_GPIO_INPUT, DAL_GPIO_KEEPER, DAL_GPIO_2MA),
      DAL_GPIO_CFG(71, 1, DAL_GPIO_INPUT, DAL_GPIO_KEEPER, DAL_GPIO_4MA),
      DAL_GPIO_CFG(72, 1, DAL_GPIO_INPUT, DAL_GPIO_KEEPER, DAL_GPIO_4MA) },
    71,
    { 4, 4, 4, 4, 4, 4, 4, 4, 2, 2, 2, 2, 2, 2, 2, 2 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2 }  /* Clock gears 0 through 10 */
  }
};

const SlimBusDeviceDalProps sbDeviceProps[] = 
{
    {0xc0, {0x00, 0x00, 0x70, 0x02, 0x17, 0x02}, 0x01},
    {0xc1, {0x00, 0x01, 0x70, 0x02, 0x17, 0x02}, 0x01},
    {0xc2, {0x00, 0x03, 0x70, 0x02, 0x17, 0x02}, 0x01},
    {0xc3, {0x00, 0x04, 0x70, 0x02, 0x17, 0x02}, 0x01},
    {0xc4, {0x00, 0x05, 0x70, 0x02, 0x17, 0x02}, 0x03},
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
    { 0x01, 0x00, 0x70, 0x02, 0x17, 0x02 },
    "LPASS",
    0x00240000,
    0x17240000,
    0x17204000,
    86,
    87,
    0,
    { DAL_GPIO_CFG(74, 1, DAL_GPIO_INPUT, DAL_GPIO_KEEPER, DAL_GPIO_2MA),
      DAL_GPIO_CFG(73, 1, DAL_GPIO_INPUT, DAL_GPIO_KEEPER, DAL_GPIO_4MA) },
    73,
    { 2, 2, 2, 2, 2, 2 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2 }  /* Clock gears 0 through 10 */
  }
};

const SlimBusDeviceDalProps sbDeviceProps2[] = 
{
    {0xc5, {0x01, 0x00, 0x70, 0x02, 0x17, 0x02}, 0x01},
    {0xc6, {0x01, 0x01, 0x70, 0x02, 0x17, 0x02}, 0x01},
    {0xc7, {0x01, 0x03, 0x70, 0x02, 0x17, 0x02}, 0x01},
    {0xc8, {0x01, 0x04, 0x70, 0x02, 0x17, 0x02}, 0x01},
    {0xc9, {0x01, 0x05, 0x70, 0x02, 0x17, 0x02}, 0x01},
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

