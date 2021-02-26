
/*
===========================================================================
*/
/**
  @file HWIOBaseMap.c
  @brief Auto-generated HWIO Device Configuration base file.

  DESCRIPTION:
    This file contains Device Configuration data structures for mapping
    physical and virtual memory for HWIO blocks.
*/
/*
  ===========================================================================

  Copyright (c) 2017 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

  ===========================================================================

  $Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Settings/HWIO/core/HWIOBaseMap.c#1 $
  $DateTime: 2017/09/18 09:26:13 $
  $Author: pwbldsvc $

  ===========================================================================
*/

/*=========================================================================
      Include Files
==========================================================================*/

#include "Drivers/HWIODxe/DalHWIO.h"
#include "msmhwiobase.h"

/*=========================================================================
      Data Definitions
==========================================================================*/

static HWIOModuleType HWIOModules_AOP_SS_MSG_RAM_DRV0[] =
{
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_AOP_SS_MSG_RAM_DRV1[] =
{
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_AOP_SS_MSG_RAM_DRV2[] =
{
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_AOP_SS_MSG_RAM_DRV3[] =
{
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_AOP_SS_MSG_RAM_DRV4[] =
{
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_SSC_MPU_CFG_SSC_MPU_WRAPPER[] =
{
  { "SSC_MPU_CFG_SSC_MPU_CFG_SSC_MPU_CFG_MPU32Q2N7S1V0_10_CL36M35L11_AHB", 0x00000000, 0x00001500 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_AOP_SS_MSG_RAM_DRV5[] =
{
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_AOP_SS_MSG_RAM_DRV6[] =
{
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_AOP_SS_MSG_RAM_DRV7[] =
{
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_AOP_SS_MSG_RAM_DRV8[] =
{
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_AOP_SS_MSG_RAM_DRV9[] =
{
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_AOP_SS_MSG_RAM_DRV10[] =
{
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_AOP_SS_MSG_RAM_DRV11[] =
{
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_AOP_SS_MSG_RAM_DRV12[] =
{
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_AOP_SS_MSG_RAM_DRV13[] =
{
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_AOP_SS_MSG_RAM_DRV14[] =
{
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_AOP_SS_MSG_RAM_DRV15[] =
{
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_CLK_CTL[] =
{
  { "GCC_CLK_CTL_REG",                             0x00000000, 0x001f0000 },
  { "GCC_RPU_RPU32Q2N7S1V0_200_CL36L12",           0x001f0000, 0x00007400 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_BOOT_ROM_START_ADDRESS[] =
{
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_BOOT_ROM[] =
{
  { "BOOT_ROM_MPU32Q2N7S1V0_4_CL36M17L10_AHB",     0x000fe000, 0x00001200 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_RAMBLUR_PIMEM[] =
{
  { "RAMBLUR_PIMEM_REGS",                          0x00000000, 0x00004000 },
  { "RAMBLUR_PIMEM_APU_APU32Q2N7S1V1_2_CL36",      0x00004000, 0x00001100 },
  { "RAMBLUR_PIMEM_MPU_MPU32Q2N7S1V1_8_CL36M25L10_AXI", 0x00006000, 0x00001400 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_OCIMEM_WRAPPER_CSR[] =
{
  { "OCIMEM_CSR",                                  0x00000000, 0x00000800 },
  { "OCIMEM_APU",                                  0x00002000, 0x00001100 },
  { "OCIMEM_MPU",                                  0x00004000, 0x00001400 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_SPDM_WRAPPER_TOP[] =
{
  { "SPDM_SPDM_CREG",                              0x00000000, 0x00000120 },
  { "SPDM_SPDM_OLEM",                              0x00001000, 0x0000015c },
  { "SPDM_SPDM_RTEM",                              0x00002000, 0x00000318 },
  { "SPDM_SPDM_SREG",                              0x00004000, 0x00000120 },
  { "SPDM_APU32Q2N7S1V0_1_CL36",                   0x00006000, 0x00001080 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_CX_CPR3[] =
{
  { "CX_CPR3",                                     0x00000000, 0x00004000 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_MX_CPR3[] =
{
  { "MX_CPR3",                                     0x00000000, 0x00004000 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_SECURITY_CONTROL[] =
{
  { "SECURITY_CONTROL_CORE",                       0x00000000, 0x00007000 },
  { "SECURE_CHANNEL",                              0x00008000, 0x00003000 },
  { "SEC_CTRL_APU_APU32Q2N7S1V0_25_CL36",          0x0000e000, 0x00001c80 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_PRNG_CFG_PRNG_TOP[] =
{
  { "PRNG_CFG_CM_CM_PRNG_CM",                      0x00000000, 0x00001000 },
  { "PRNG_CFG_TZ_TZ_PRNG_TZ",                      0x00001000, 0x00001000 },
  { "PRNG_CFG_MSA_MSA_PRNG_SUB",                   0x00002000, 0x00001000 },
  { "PRNG_CFG_EE2_EE2_PRNG_SUB",                   0x00003000, 0x00001000 },
  { "PRNG_CFG_EE3_EE3_PRNG_SUB",                   0x00004000, 0x00001000 },
  { "PRNG_CFG_EE4_EE4_PRNG_SUB",                   0x00005000, 0x00001000 },
  { "PRNG_CFG_EE5_EE5_PRNG_SUB",                   0x00006000, 0x00001000 },
  { "PRNG_CFG_EE6_EE6_PRNG_SUB",                   0x00007000, 0x00001000 },
  { "PRNG_CFG_EE7_EE7_PRNG_SUB",                   0x00008000, 0x00001000 },
  { "PRNG_CFG_EE8_EE8_PRNG_SUB",                   0x00009000, 0x00001000 },
  { "PRNG_CFG_EE9_EE9_PRNG_SUB",                   0x0000a000, 0x00001000 },
  { "PRNG_CFG_EE10_EE10_PRNG_SUB",                 0x0000b000, 0x00001000 },
  { "PRNG_CFG_EE11_EE11_PRNG_SUB",                 0x0000c000, 0x00001000 },
  { "PRNG_CFG_EE12_EE12_PRNG_SUB",                 0x0000d000, 0x00001000 },
  { "PRNG_CFG_EE13_EE13_PRNG_SUB",                 0x0000e000, 0x00001000 },
  { "PRNG_CFG_EE14_EE14_PRNG_SUB",                 0x0000f000, 0x00001000 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_GLM_WRAPPER[] =
{
  { "GLM_PRIVATE",                                 0x00001000, 0x00001000 },
  { "GLM_PRIVATE_LLM_PRIVATE",                     0x00001000, 0x00000800 },
  { "GLM_PUBLIC",                                  0x00000000, 0x00001000 },
  { "GLM_PUBLIC_LLM_PUBLIC",                       0x00000000, 0x00000800 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_PERIPH_SS_SDC1_SDCC5_TOP[] =
{
  { "PERIPH_SS_SDC1_SDCC_SDCC5_HC",                0x00004000, 0x00001000 },
  { "PERIPH_SS_SDC1_SDCC_SDCC5_HC_CMDQ",           0x00005000, 0x00001000 },
  { "PERIPH_SS_SDC1_SDCC_ICE",                     0x00008000, 0x00008000 },
  { "PERIPH_SS_SDC1_SDCC_ICE_REGS",                0x00008000, 0x00002000 },
  { "PERIPH_SS_SDC1_SDCC_ICE_LUT_KEYS",            0x0000a000, 0x00002000 },
  { "PERIPH_SS_SDC1_SDCC_ICE_XPU3",                0x0000c000, 0x00001200 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_QUPV3_0_QUPV3_ID_1[] =
{
  { "QUPV3_0_GSI_TOP",                             0x00000000, 0x00060000 },
  { "QUPV3_0_GSI",                                 0x00004000, 0x00050000 },
  { "QUPV3_0_XPU3",                                0x00000000, 0x00003000 },
  { "QUPV3_0_QUPV3_SE_WRAPPER",                    0x00080000, 0x00020000 },
  { "QUPV3_0_SE0",                                 0x00080000, 0x00004000 },
  { "QUPV3_0_SE0_GENI4_CFG",                       0x00080000, 0x00000100 },
  { "QUPV3_0_SE0_GENI4_IMAGE_REGS",                0x00080100, 0x00000500 },
  { "QUPV3_0_SE0_GENI4_DATA",                      0x00080600, 0x00000600 },
  { "QUPV3_0_SE0_QUPV3_SE_DMA",                    0x00080c00, 0x00000400 },
  { "QUPV3_0_SE0_GENI4_IMAGE",                     0x00081000, 0x00001000 },
  { "QUPV3_0_SE0_QUPV3_SEC",                       0x00082000, 0x00001000 },
  { "QUPV3_0_SE1",                                 0x00084000, 0x00004000 },
  { "QUPV3_0_SE1_GENI4_CFG",                       0x00084000, 0x00000100 },
  { "QUPV3_0_SE1_GENI4_IMAGE_REGS",                0x00084100, 0x00000500 },
  { "QUPV3_0_SE1_GENI4_DATA",                      0x00084600, 0x00000600 },
  { "QUPV3_0_SE1_QUPV3_SE_DMA",                    0x00084c00, 0x00000400 },
  { "QUPV3_0_SE1_GENI4_IMAGE",                     0x00085000, 0x00001000 },
  { "QUPV3_0_SE1_QUPV3_SEC",                       0x00086000, 0x00001000 },
  { "QUPV3_0_SE2",                                 0x00088000, 0x00004000 },
  { "QUPV3_0_SE2_GENI4_CFG",                       0x00088000, 0x00000100 },
  { "QUPV3_0_SE2_GENI4_IMAGE_REGS",                0x00088100, 0x00000500 },
  { "QUPV3_0_SE2_GENI4_DATA",                      0x00088600, 0x00000600 },
  { "QUPV3_0_SE2_QUPV3_SE_DMA",                    0x00088c00, 0x00000400 },
  { "QUPV3_0_SE2_GENI4_IMAGE",                     0x00089000, 0x00001000 },
  { "QUPV3_0_SE2_QUPV3_SEC",                       0x0008a000, 0x00001000 },
  { "QUPV3_0_SE3",                                 0x0008c000, 0x00004000 },
  { "QUPV3_0_SE3_GENI4_CFG",                       0x0008c000, 0x00000100 },
  { "QUPV3_0_SE3_GENI4_IMAGE_REGS",                0x0008c100, 0x00000500 },
  { "QUPV3_0_SE3_GENI4_DATA",                      0x0008c600, 0x00000600 },
  { "QUPV3_0_SE3_QUPV3_SE_DMA",                    0x0008cc00, 0x00000400 },
  { "QUPV3_0_SE3_GENI4_IMAGE",                     0x0008d000, 0x00001000 },
  { "QUPV3_0_SE3_QUPV3_SEC",                       0x0008e000, 0x00001000 },
  { "QUPV3_0_SE4",                                 0x00090000, 0x00004000 },
  { "QUPV3_0_SE4_GENI4_CFG",                       0x00090000, 0x00000100 },
  { "QUPV3_0_SE4_GENI4_IMAGE_REGS",                0x00090100, 0x00000500 },
  { "QUPV3_0_SE4_GENI4_DATA",                      0x00090600, 0x00000600 },
  { "QUPV3_0_SE4_QUPV3_SE_DMA",                    0x00090c00, 0x00000400 },
  { "QUPV3_0_SE4_GENI4_IMAGE",                     0x00091000, 0x00001000 },
  { "QUPV3_0_SE4_QUPV3_SEC",                       0x00092000, 0x00001000 },
  { "QUPV3_0_SE5",                                 0x00094000, 0x00004000 },
  { "QUPV3_0_SE5_GENI4_CFG",                       0x00094000, 0x00000100 },
  { "QUPV3_0_SE5_GENI4_IMAGE_REGS",                0x00094100, 0x00000500 },
  { "QUPV3_0_SE5_GENI4_DATA",                      0x00094600, 0x00000600 },
  { "QUPV3_0_SE5_QUPV3_SE_DMA",                    0x00094c00, 0x00000400 },
  { "QUPV3_0_SE5_GENI4_IMAGE",                     0x00095000, 0x00001000 },
  { "QUPV3_0_SE5_QUPV3_SEC",                       0x00096000, 0x00001000 },
  { "QUPV3_0_SE6",                                 0x00098000, 0x00004000 },
  { "QUPV3_0_SE6_GENI4_CFG",                       0x00098000, 0x00000100 },
  { "QUPV3_0_SE6_GENI4_IMAGE_REGS",                0x00098100, 0x00000500 },
  { "QUPV3_0_SE6_GENI4_DATA",                      0x00098600, 0x00000600 },
  { "QUPV3_0_SE6_QUPV3_SE_DMA",                    0x00098c00, 0x00000400 },
  { "QUPV3_0_SE6_GENI4_IMAGE",                     0x00099000, 0x00001000 },
  { "QUPV3_0_SE6_QUPV3_SEC",                       0x0009a000, 0x00001000 },
  { "QUPV3_0_SE7",                                 0x0009c000, 0x00004000 },
  { "QUPV3_0_SE7_GENI4_CFG",                       0x0009c000, 0x00000100 },
  { "QUPV3_0_SE7_GENI4_IMAGE_REGS",                0x0009c100, 0x00000500 },
  { "QUPV3_0_SE7_GENI4_DATA",                      0x0009c600, 0x00000600 },
  { "QUPV3_0_SE7_QUPV3_SE_DMA",                    0x0009cc00, 0x00000400 },
  { "QUPV3_0_SE7_GENI4_IMAGE",                     0x0009d000, 0x00001000 },
  { "QUPV3_0_SE7_QUPV3_SEC",                       0x0009e000, 0x00001000 },
  { "QUPV3_0_QUPV3_COMMON",                        0x000c0000, 0x00002000 },
  { "QUPV3_0_VMIDMT_SMR_48_SSD6_SID6_MA48_SP48_36", 0x000c6000, 0x00001000 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_QUPV3_1_QUPV3_ID_1[] =
{
  { "QUPV3_1_GSI_TOP",                             0x00000000, 0x00060000 },
  { "QUPV3_1_GSI",                                 0x00004000, 0x00050000 },
  { "QUPV3_1_XPU3",                                0x00000000, 0x00003000 },
  { "QUPV3_1_QUPV3_SE_WRAPPER",                    0x00080000, 0x00020000 },
  { "QUPV3_1_SE0",                                 0x00080000, 0x00004000 },
  { "QUPV3_1_SE0_GENI4_CFG",                       0x00080000, 0x00000100 },
  { "QUPV3_1_SE0_GENI4_IMAGE_REGS",                0x00080100, 0x00000500 },
  { "QUPV3_1_SE0_GENI4_DATA",                      0x00080600, 0x00000600 },
  { "QUPV3_1_SE0_QUPV3_SE_DMA",                    0x00080c00, 0x00000400 },
  { "QUPV3_1_SE0_GENI4_IMAGE",                     0x00081000, 0x00001000 },
  { "QUPV3_1_SE0_QUPV3_SEC",                       0x00082000, 0x00001000 },
  { "QUPV3_1_SE1",                                 0x00084000, 0x00004000 },
  { "QUPV3_1_SE1_GENI4_CFG",                       0x00084000, 0x00000100 },
  { "QUPV3_1_SE1_GENI4_IMAGE_REGS",                0x00084100, 0x00000500 },
  { "QUPV3_1_SE1_GENI4_DATA",                      0x00084600, 0x00000600 },
  { "QUPV3_1_SE1_QUPV3_SE_DMA",                    0x00084c00, 0x00000400 },
  { "QUPV3_1_SE1_GENI4_IMAGE",                     0x00085000, 0x00001000 },
  { "QUPV3_1_SE1_QUPV3_SEC",                       0x00086000, 0x00001000 },
  { "QUPV3_1_SE2",                                 0x00088000, 0x00004000 },
  { "QUPV3_1_SE2_GENI4_CFG",                       0x00088000, 0x00000100 },
  { "QUPV3_1_SE2_GENI4_IMAGE_REGS",                0x00088100, 0x00000500 },
  { "QUPV3_1_SE2_GENI4_DATA",                      0x00088600, 0x00000600 },
  { "QUPV3_1_SE2_QUPV3_SE_DMA",                    0x00088c00, 0x00000400 },
  { "QUPV3_1_SE2_GENI4_IMAGE",                     0x00089000, 0x00001000 },
  { "QUPV3_1_SE2_QUPV3_SEC",                       0x0008a000, 0x00001000 },
  { "QUPV3_1_SE3",                                 0x0008c000, 0x00004000 },
  { "QUPV3_1_SE3_GENI4_CFG",                       0x0008c000, 0x00000100 },
  { "QUPV3_1_SE3_GENI4_IMAGE_REGS",                0x0008c100, 0x00000500 },
  { "QUPV3_1_SE3_GENI4_DATA",                      0x0008c600, 0x00000600 },
  { "QUPV3_1_SE3_QUPV3_SE_DMA",                    0x0008cc00, 0x00000400 },
  { "QUPV3_1_SE3_GENI4_IMAGE",                     0x0008d000, 0x00001000 },
  { "QUPV3_1_SE3_QUPV3_SEC",                       0x0008e000, 0x00001000 },
  { "QUPV3_1_SE4",                                 0x00090000, 0x00004000 },
  { "QUPV3_1_SE4_GENI4_CFG",                       0x00090000, 0x00000100 },
  { "QUPV3_1_SE4_GENI4_IMAGE_REGS",                0x00090100, 0x00000500 },
  { "QUPV3_1_SE4_GENI4_DATA",                      0x00090600, 0x00000600 },
  { "QUPV3_1_SE4_QUPV3_SE_DMA",                    0x00090c00, 0x00000400 },
  { "QUPV3_1_SE4_GENI4_IMAGE",                     0x00091000, 0x00001000 },
  { "QUPV3_1_SE4_QUPV3_SEC",                       0x00092000, 0x00001000 },
  { "QUPV3_1_SE5",                                 0x00094000, 0x00004000 },
  { "QUPV3_1_SE5_GENI4_CFG",                       0x00094000, 0x00000100 },
  { "QUPV3_1_SE5_GENI4_IMAGE_REGS",                0x00094100, 0x00000500 },
  { "QUPV3_1_SE5_GENI4_DATA",                      0x00094600, 0x00000600 },
  { "QUPV3_1_SE5_QUPV3_SE_DMA",                    0x00094c00, 0x00000400 },
  { "QUPV3_1_SE5_GENI4_IMAGE",                     0x00095000, 0x00001000 },
  { "QUPV3_1_SE5_QUPV3_SEC",                       0x00096000, 0x00001000 },
  { "QUPV3_1_SE6",                                 0x00098000, 0x00004000 },
  { "QUPV3_1_SE6_GENI4_CFG",                       0x00098000, 0x00000100 },
  { "QUPV3_1_SE6_GENI4_IMAGE_REGS",                0x00098100, 0x00000500 },
  { "QUPV3_1_SE6_GENI4_DATA",                      0x00098600, 0x00000600 },
  { "QUPV3_1_SE6_QUPV3_SE_DMA",                    0x00098c00, 0x00000400 },
  { "QUPV3_1_SE6_GENI4_IMAGE",                     0x00099000, 0x00001000 },
  { "QUPV3_1_SE6_QUPV3_SEC",                       0x0009a000, 0x00001000 },
  { "QUPV3_1_SE7",                                 0x0009c000, 0x00004000 },
  { "QUPV3_1_SE7_GENI4_CFG",                       0x0009c000, 0x00000100 },
  { "QUPV3_1_SE7_GENI4_IMAGE_REGS",                0x0009c100, 0x00000500 },
  { "QUPV3_1_SE7_GENI4_DATA",                      0x0009c600, 0x00000600 },
  { "QUPV3_1_SE7_QUPV3_SE_DMA",                    0x0009cc00, 0x00000400 },
  { "QUPV3_1_SE7_GENI4_IMAGE",                     0x0009d000, 0x00001000 },
  { "QUPV3_1_SE7_QUPV3_SEC",                       0x0009e000, 0x00001000 },
  { "QUPV3_1_QUPV3_COMMON",                        0x000c0000, 0x00002000 },
  { "QUPV3_1_VMIDMT_SMR_48_SSD6_SID6_MA48_SP48_36", 0x000c6000, 0x00001000 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_AHB2PHY_SOUTH[] =
{
  { "A_CM_QREFS_VBG_INT_VREF_SW",                  0x00000000, 0x00000080 },
  { "PHY_REFGEN_SOUTH_CM_PHY_REFGEN",              0x00001000, 0x00000060 },
  { "AHB2PHY_SOUTH_AHB2PHY_SWMAN",                 0x00002000, 0x00000400 },
  { "AHB2PHY_SOUTH_AHB2PHY_BROADCAST_SWMAN",       0x00003000, 0x00000400 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_TITAN_THROTTLE_THROTTLE_WRAPPER_3[] =
{
  { "TITAN_THROTTLE_THROTTLE_0_THROTTLE_0_THROTTLE", 0x00000000, 0x00000100 },
  { "TITAN_THROTTLE_THROTTLE_1_THROTTLE_1_THROTTLE", 0x00000200, 0x00000100 },
  { "TITAN_THROTTLE_THROTTLE_2_THROTTLE_2_THROTTLE", 0x00000400, 0x00000100 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_MDSS_THROTTLE_THROTTLE_WRAPPER_3[] =
{
  { "MDSS_THROTTLE_THROTTLE_0_THROTTLE_0_THROTTLE", 0x00000000, 0x00000100 },
  { "MDSS_THROTTLE_THROTTLE_1_THROTTLE_1_THROTTLE", 0x00000200, 0x00000100 },
  { "MDSS_THROTTLE_THROTTLE_2_THROTTLE_2_THROTTLE", 0x00000400, 0x00000100 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_VIDEO_THROTTLE_THROTTLE_BWMON_WRAPPER_2_VENUS[] =
{
  { "VIDEO_THROTTLE_THROTTLE_0_THROTTLE_0_THROTTLE", 0x00000000, 0x00000100 },
  { "VIDEO_THROTTLE_BWMON_0_BWMON_0_BWMON_1_2",    0x00000100, 0x00000600 },
  { "VIDEO_THROTTLE_THROTTLE_1_THROTTLE_1_THROTTLE", 0x00000700, 0x00000100 },
  { "VIDEO_THROTTLE_BWMON_1_BWMON_1_BWMON_1_2",    0x00000800, 0x00000600 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_DCC_RAM_START_ADDRESS[] =
{
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_QC_DCC_CSAFB9399A[] =
{
  { "DCC_APU",                                     0x00000000, 0x00001800 },
  { "DCC_CFG",                                     0x00002000, 0x00001000 },
  { "DCC_RAM",                                     0x00008000, 0x00008000 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_QM[] =
{
  { "QM",                                          0x00000000, 0x00004000 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_QM_MPU_CFG_QM_MPU_WRAPPER[] =
{
  { "QM_MPU_CFG_QM_MPU_CFG_QM_MPU_CFG_MPU32Q2N7S1V0_4_CL36M23L12_AHB", 0x00000000, 0x00001200 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_DDR_SS[] =
{
  { "LLCC0_LLCC",                                  0x00000000, 0x00050000 },
  { "LLCC0_TRP",                                   0x00000000, 0x00028000 },
  { "LLCC0_COMMON",                                0x00030000, 0x00001000 },
  { "LLCC0_PERFMON",                               0x00031000, 0x00001000 },
  { "LLCC0_FEWC",                                  0x00032000, 0x00002000 },
  { "LLCC0_FEAC",                                  0x00034000, 0x00004000 },
  { "LLCC0_BERC",                                  0x00038000, 0x00002000 },
  { "LLCC0_FERC",                                  0x0003a000, 0x00002000 },
  { "LLCC0_BIST",                                  0x0003c000, 0x00001000 },
  { "LLCC0_AMON",                                  0x0003d000, 0x00001000 },
  { "LLCC0_PMGR",                                  0x0003e000, 0x00002000 },
  { "LLCC0_DRP",                                   0x00040000, 0x00005000 },
  { "LLCC0_BEAC",                                  0x00048000, 0x00004000 },
  { "LLCC0_LLCC_MPU",                              0x0004e000, 0x00001c80 },
  { "LLCC1_LLCC",                                  0x00080000, 0x00050000 },
  { "LLCC1_TRP",                                   0x00080000, 0x00028000 },
  { "LLCC1_COMMON",                                0x000b0000, 0x00001000 },
  { "LLCC1_PERFMON",                               0x000b1000, 0x00001000 },
  { "LLCC1_FEWC",                                  0x000b2000, 0x00002000 },
  { "LLCC1_FEAC",                                  0x000b4000, 0x00004000 },
  { "LLCC1_BERC",                                  0x000b8000, 0x00002000 },
  { "LLCC1_FERC",                                  0x000ba000, 0x00002000 },
  { "LLCC1_BIST",                                  0x000bc000, 0x00001000 },
  { "LLCC1_AMON",                                  0x000bd000, 0x00001000 },
  { "LLCC1_PMGR",                                  0x000be000, 0x00002000 },
  { "LLCC1_DRP",                                   0x000c0000, 0x00005000 },
  { "LLCC1_BEAC",                                  0x000c8000, 0x00004000 },
  { "LLCC1_LLCC_MPU",                              0x000ce000, 0x00001c80 },
  { "LLCC_BROADCAST_LLCC",                         0x00200000, 0x00050000 },
  { "LLCC_BROADCAST_TRP",                          0x00200000, 0x00028000 },
  { "LLCC_BROADCAST_COMMON",                       0x00230000, 0x00001000 },
  { "LLCC_BROADCAST_PERFMON",                      0x00231000, 0x00001000 },
  { "LLCC_BROADCAST_FEWC",                         0x00232000, 0x00002000 },
  { "LLCC_BROADCAST_FEAC",                         0x00234000, 0x00004000 },
  { "LLCC_BROADCAST_BERC",                         0x00238000, 0x00002000 },
  { "LLCC_BROADCAST_FERC",                         0x0023a000, 0x00002000 },
  { "LLCC_BROADCAST_BIST",                         0x0023c000, 0x00001000 },
  { "LLCC_BROADCAST_AMON",                         0x0023d000, 0x00001000 },
  { "LLCC_BROADCAST_PMGR",                         0x0023e000, 0x00002000 },
  { "LLCC_BROADCAST_DRP",                          0x00240000, 0x00005000 },
  { "LLCC_BROADCAST_BEAC",                         0x00248000, 0x00004000 },
  { "LLCC_BROADCAST_LLCC_MPU",                     0x0024e000, 0x00001c80 },
  { "CABO0_CABO",                                  0x00060000, 0x00010000 },
  { "CABO0_CABO_GLOBAL",                           0x00060000, 0x00001000 },
  { "CABO0_CABO_ISU",                              0x00061000, 0x00001000 },
  { "CABO0_CABO_CBU_TOP",                          0x00062000, 0x00001000 },
  { "CABO0_CABO_MPE",                              0x00063000, 0x00001000 },
  { "CABO0_CABO_DTC",                              0x00064000, 0x00001000 },
  { "CABO0_CABO_SHKE",                             0x00065000, 0x00001000 },
  { "CABO0_CABO_OSU",                              0x00066000, 0x00001000 },
  { "CABO0_CABO_PXI",                              0x00068000, 0x00001000 },
  { "CABO0_CABO_INTERRUPT",                        0x00069000, 0x00001000 },
  { "CABO0_CABO_PERFMON",                          0x0006b000, 0x00001000 },
  { "CABO0_CABO_GLOBAL_SEC",                       0x0006d000, 0x00001000 },
  { "CABO0_CABO_PXI_SECURE",                       0x0006e000, 0x00001000 },
  { "CABO1_CABO",                                  0x000e0000, 0x00010000 },
  { "CABO1_CABO_GLOBAL",                           0x000e0000, 0x00001000 },
  { "CABO1_CABO_ISU",                              0x000e1000, 0x00001000 },
  { "CABO1_CABO_CBU_TOP",                          0x000e2000, 0x00001000 },
  { "CABO1_CABO_MPE",                              0x000e3000, 0x00001000 },
  { "CABO1_CABO_DTC",                              0x000e4000, 0x00001000 },
  { "CABO1_CABO_SHKE",                             0x000e5000, 0x00001000 },
  { "CABO1_CABO_OSU",                              0x000e6000, 0x00001000 },
  { "CABO1_CABO_PXI",                              0x000e8000, 0x00001000 },
  { "CABO1_CABO_INTERRUPT",                        0x000e9000, 0x00001000 },
  { "CABO1_CABO_PERFMON",                          0x000eb000, 0x00001000 },
  { "CABO1_CABO_GLOBAL_SEC",                       0x000ed000, 0x00001000 },
  { "CABO1_CABO_PXI_SECURE",                       0x000ee000, 0x00001000 },
  { "CABO_BROADCAST_CABO",                         0x00260000, 0x00010000 },
  { "CABO_BROADCAST_CABO_GLOBAL",                  0x00260000, 0x00001000 },
  { "CABO_BROADCAST_CABO_ISU",                     0x00261000, 0x00001000 },
  { "CABO_BROADCAST_CABO_CBU_TOP",                 0x00262000, 0x00001000 },
  { "CABO_BROADCAST_CABO_MPE",                     0x00263000, 0x00001000 },
  { "CABO_BROADCAST_CABO_DTC",                     0x00264000, 0x00001000 },
  { "CABO_BROADCAST_CABO_SHKE",                    0x00265000, 0x00001000 },
  { "CABO_BROADCAST_CABO_OSU",                     0x00266000, 0x00001000 },
  { "CABO_BROADCAST_CABO_PXI",                     0x00268000, 0x00001000 },
  { "CABO_BROADCAST_CABO_INTERRUPT",               0x00269000, 0x00001000 },
  { "CABO_BROADCAST_CABO_PERFMON",                 0x0026b000, 0x00001000 },
  { "CABO_BROADCAST_CABO_GLOBAL_SEC",              0x0026d000, 0x00001000 },
  { "CABO_BROADCAST_CABO_PXI_SECURE",              0x0026e000, 0x00001000 },
  { "MEMNOC_MEM_NOC_HM",                           0x00280000, 0x00027200 },
  { "MEMNOC_MEM_NOC",                              0x00280000, 0x00027200 },
  { "MEMNOC_MPU32Q2N7S1V0_16_CL36M35L12_AHB",      0x002b0000, 0x00001800 },
  { "MCCC_MCCC_MSTR",                              0x00330000, 0x00000800 },
  { "MCCC_CH0_MCCC_SLV",                           0x00050000, 0x00000800 },
  { "MCCC_CH1_MCCC_SLV",                           0x000d0000, 0x00000800 },
  { "MCCC_CH2_MCCC_SLV",                           0x00150000, 0x00000800 },
  { "MCCC_CH3_MCCC_SLV",                           0x001d0000, 0x00000800 },
  { "MCCC_BROADCAST_MCCC_SLV",                     0x00250000, 0x00000800 },
  { "SHRM_CSR_SHRM_CSR",                           0x002d0000, 0x00002000 },
  { "SHRM_MEM_SHRM",                               0x002e0000, 0x00010000 },
  { "DDRPHY_CH0_CA_DDR_PHY",                       0x00300000, 0x00001000 },
  { "DDRPHY_CH0_DQ0_DDR_PHY",                      0x00301000, 0x00001000 },
  { "DDRPHY_CH0_DQ1_DDR_PHY",                      0x00302000, 0x00001000 },
  { "DDRPHY_CH1_CA_DDR_PHY",                       0x00303000, 0x00001000 },
  { "DDRPHY_CH1_DQ0_DDR_PHY",                      0x00304000, 0x00001000 },
  { "DDRPHY_CH1_DQ1_DDR_PHY",                      0x00305000, 0x00001000 },
  { "DDRCC_CH01_DDR_CC",                           0x00306000, 0x00001000 },
  { "DDR_REG_DDR_SS_REGS",                         0x00340000, 0x00000400 },
  { "DDR_TG0_TG",                                  0x00342000, 0x00000100 },
  { "DDR_TG1_TG",                                  0x00344000, 0x00000100 },
  { "DDR_TG2_TG",                                  0x00346000, 0x00000100 },
  { "DPC_REG_DPCC",                                0x00348000, 0x00002000 },
  { "DPC_REG_DPCC_DPCC_DPCC_REG",                  0x00348000, 0x00001000 },
  { "DDR_TZ_DDR_SS_TZ",                            0x0034a000, 0x00001000 },
  { "DDRSS_AHB2PHY0_AHB2PHY_SWMAN",                0x0030e000, 0x00000400 },
  { "DDRSS_AHB2PHY_BROADCAST_AHB2PHY_BROADCAST_SWMAN1", 0x0032f000, 0x00001000 },
  { "APP_BWMON_THROTTLE_BWMON_WRAPPER_2_CPU_GPU",  0x00336000, 0x00001000 },
  { "APP_BWMON_THROTTLE_0_THROTTLE_0_THROTTLE",    0x00336000, 0x00000100 },
  { "APP_BWMON_THROTTLE_1_THROTTLE_1_THROTTLE",    0x00336200, 0x00000100 },
  { "APP_BWMON_BWMON_BWMON_BWMON_1_2",             0x00336300, 0x00000600 },
  { "GPU_BWMON_THROTTLE_BWMON_WRAPPER_2_CPU_GPU",  0x00337000, 0x00001000 },
  { "GPU_BWMON_THROTTLE_0_THROTTLE_0_THROTTLE",    0x00337000, 0x00000100 },
  { "GPU_BWMON_THROTTLE_1_THROTTLE_1_THROTTLE",    0x00337200, 0x00000100 },
  { "GPU_BWMON_BWMON_BWMON_BWMON_1_2",             0x00337300, 0x00000600 },
  { "CDSP_BWMON_THROTTLE_BWMON_WRAPPER_CDSP",      0x00338000, 0x00001000 },
  { "CDSP_BWMON_THROTTLE_0_THROTTLE_0_THROTTLE",   0x00338000, 0x00000100 },
  { "CDSP_BWMON_BWMON_BWMON_BWMON_1_2",            0x00338100, 0x00000600 },
  { "DC_NOC_BROADCAST_MPU_MPU32Q2N7S1V0_40_CL36M24L12_AHB", 0x00360000, 0x00002400 },
  { "DC_NOC_NON_BROADCAST_MPU_MPU32Q2N7S1V0_16_CL36M24L12_AHB", 0x00334000, 0x00001800 },
  { "DC_NOC_DC_NOC",                               0x003e0000, 0x00000400 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_CONFIG_NOC_WRAPPER[] =
{
  { "CNOC_SNOC_MPU_CFG",                           0x00020000, 0x00001c00 },
  { "CNOC_SS_MPU_CFG",                             0x00024000, 0x00001d80 },
  { "CNOC_AOSS_MPU_CFG",                           0x00026000, 0x00001800 },
  { "CNOC_SERVICE_NETWORK",                        0x00000000, 0x00005080 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_SYSTEM_NOC[] =
{
  { "SYSTEM_NOC",                                  0x00000000, 0x00016080 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_AGGRE_NOC_AGGRE_NOC[] =
{
  { "AGGRE_NOC_A1NOC_SERVICE_NETWORK",             0x00000000, 0x0001f080 },
  { "AGGRE_NOC_A2NOC_SERVICE_NETWORK",             0x00020000, 0x0001f280 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_MMSS_NOC[] =
{
  { "MMSS_NOC",                                    0x00000000, 0x0001c100 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_UFS_MEM_UFS_1LANE_REGS[] =
{
  { "UFS_MEM_UFS",                                 0x00004000, 0x00003000 },
  { "UFS_MEM_MPHY_UFS",                            0x00007000, 0x00000e00 },
  { "UFS_MEM_QSERDES_COM_QSERDES_COM_UFS_QMP_PLL", 0x00007000, 0x0000018c },
  { "UFS_MEM_QSERDES_TX0_QSERDES_TX0_UFS_QMP_TX",  0x00007400, 0x00000128 },
  { "UFS_MEM_QSERDES_RX0_QSERDES_RX0_UFS_QMP_RX",  0x00007600, 0x000001fc },
  { "UFS_MEM_UFS_PHY_UFS_PHY_UFS_PCS",             0x00007c00, 0x000001dc },
  { "UFS_MEM_ICE",                                 0x00010000, 0x00008000 },
  { "UFS_MEM_ICE_REGS",                            0x00010000, 0x00002000 },
  { "UFS_MEM_ICE_LUT_KEYS",                        0x00012000, 0x00002000 },
  { "UFS_MEM_ICE_XPU3",                            0x00014000, 0x00001200 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_CRYPTO0_CRYPTO_TOP[] =
{
  { "CRYPTO0_CRYPTO",                              0x0003a000, 0x00006000 },
  { "CRYPTO0_CRYPTO_BAM",                          0x00004000, 0x00024000 },
  { "CRYPTO0_CRYPTO_BAM_XPU3_BAM",                 0x00001000, 0x00003000 },
  { "CRYPTO0_CRYPTO_BAM_VMIDMT_BAM",               0x00000000, 0x00001000 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_IPA_0_IPA_WRAPPER[] =
{
  { "IPA_0_IPA_VMIDMT",                            0x00030000, 0x00001000 },
  { "IPA_0_IPA",                                   0x00040000, 0x00020000 },
  { "IPA_0_IPA_UC",                                0x00060000, 0x00014000 },
  { "IPA_0_IPA_UC_IPA_UC_RAM",                     0x00060000, 0x00008000 },
  { "IPA_0_IPA_UC_IPA_UC_PER",                     0x00070000, 0x00002000 },
  { "IPA_0_IPA_UC_IPA_UC_MBOX",                    0x00072000, 0x00002000 },
  { "IPA_0_GSI_TOP",                               0x00000000, 0x00030000 },
  { "IPA_0_GSI_TOP_GSI",                           0x00004000, 0x00028000 },
  { "IPA_0_GSI_TOP_XPU3",                          0x00000000, 0x00002000 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_CORE_TOP_CSR[] =
{
  { "TCSR_MUTEX_RPU32Q2N7S1V0_64_CL36L12",         0x00000000, 0x00003000 },
  { "TCSR_TCSR_MUTEX",                             0x00040000, 0x00040000 },
  { "TCSR_REGS_RPU32Q2N7S1V0_48_CL36L12",          0x00080000, 0x00002800 },
  { "TCSR_TCSR_REGS",                              0x000c0000, 0x00030000 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_TLMM[] =
{
  { "TLMM_XPU_NORTH_XN",                           0x00400000, 0x00006780 },
  { "TLMM_NORTH",                                  0x00500000, 0x00300000 },
  { "TLMM_XPU_SOUTH_XS",                           0x00800000, 0x00006780 },
  { "TLMM_SOUTH",                                  0x00900000, 0x00300000 },
  { "TLMM_XPU_WEST_XW",                            0x00000000, 0x00006780 },
  { "TLMM_WEST",                                   0x00100000, 0x00300000 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_MODEM_TOP[] =
{
  { "MPSS_TOP",                                    0x00180000, 0x00180000 },
  { "MPSS_RMB",                                    0x00180000, 0x00010000 },
  { "MSS_UIM0_UART_DM",                            0x00190000, 0x00000200 },
  { "MSS_UIM1_UART_DM",                            0x00194000, 0x00000200 },
  { "MSS_UIM2_UART_DM",                            0x00198000, 0x00000200 },
  { "MSS_UIM3_UART_DM",                            0x0019c000, 0x00000200 },
  { "MSS_CXM_UART_DM",                             0x001a0000, 0x00000204 },
  { "MPSS_PERPH",                                  0x001a8000, 0x00008000 },
  { "MSS_CONF_BUS_TIMEOUT",                        0x001b0000, 0x00001000 },
  { "MSS_CPR3",                                    0x001b1000, 0x00001000 },
  { "MSS_MGPI",                                    0x001b2000, 0x00000128 },
  { "MSS_ENCRYPT_ACC",                             0x001b3000, 0x00000080 },
  { "MSS_STMR",                                    0x001b4000, 0x000000fd },
  { "MSS_RFC",                                     0x001b8000, 0x00008000 },
  { "MSS_PDMEM",                                   0x001b8000, 0x00006000 },
  { "MSS_RFC_SWI",                                 0x001bfc00, 0x00000400 },
  { "MSS_CRYPTO_TOP",                              0x001c0000, 0x00040000 },
  { "MSS_CRYPTO",                                  0x001fa000, 0x00006000 },
  { "MSS_CRYPTO_BAM",                              0x001c4000, 0x00024000 },
  { "MSS_MSS_RSCC",                                0x00200000, 0x00020000 },
  { "MSS_RSCC_RSCC_RSC",                           0x00200000, 0x00020000 },
  { "MSS_NAV",                                     0x00300000, 0x000f888d },
  { "MSS_NAV_WTR_BASE",                            0x00300000, 0x00001900 },
  { "MSS_NAV_TOP_BASE",                            0x00301900, 0x00000700 },
  { "MSS_NAV_BB_BASE",                             0x00302000, 0x000c6000 },
  { "MSS_QDSP6V65SS_PUB",                          0x00080000, 0x00004040 },
  { "MSS_QDSP6V65SS_PRIVATE",                      0x00100000, 0x00080000 },
  { "MSS_QDSP6V65SS_CSR",                          0x00100000, 0x0000c23d },
  { "MSS_QDSP6V65SS_L2VIC",                        0x00110000, 0x00001000 },
  { "MSS_QDSP6SS_QDSP6SS_QTMR_AC",                 0x00120000, 0x00001000 },
  { "MSS_QDSP6SS_QTMR_F0_0",                       0x00121000, 0x00001000 },
  { "MSS_QDSP6SS_QTMR_F1_1",                       0x00122000, 0x00001000 },
  { "MSS_QDSP6SS_QTMR_F2_2",                       0x00123000, 0x00001000 },
  { "MSS_QDSP6SS_QDSP6V65SS_RSCC",                 0x00130000, 0x00010000 },
  { "MSS_QDSP6SS_RSCC_RSCC_RSC",                   0x00130000, 0x00010000 },
  { "MSS_QDSP6SS_QDSP6SS_CPR3",                    0x00140000, 0x00004000 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_MSS_QDSP6_TCM[] =
{
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_GFX[] =
{
  { "GPU_RBBM_BLK_DEC0",                           0x00000000, 0x00001800 },
  { "GPU_RBBM_BLK_DEC1",                           0x0003d000, 0x00003000 },
  { "GPU_CP_BLK_DEC0",                             0x00002000, 0x00001000 },
  { "GPU_CP_BLK_DEC1",                             0x0003c000, 0x00001000 },
  { "GPU_VSC_BLK_DEC0",                            0x00003000, 0x00000800 },
  { "GPU_UCHE_BLK_DEC0",                           0x00003800, 0x00000800 },
  { "GPU_GRAS_BLK_DEC0",                           0x00020000, 0x00001000 },
  { "GPU_GRAS_BLK_DEC1",                           0x00021000, 0x00000800 },
  { "GPU_GRAS_BLK_DEC2",                           0x00021800, 0x00000800 },
  { "GPU_RB_BLK_DEC0",                             0x00022000, 0x00001000 },
  { "GPU_RB_BLK_DEC1",                             0x00023000, 0x00000800 },
  { "GPU_RB_BLK_DEC2",                             0x00023800, 0x00000800 },
  { "GPU_VPC_BLK_DEC0",                            0x00024000, 0x00000400 },
  { "GPU_VPC_BLK_DEC1",                            0x00024400, 0x00000400 },
  { "GPU_VPC_BLK_DEC2",                            0x00024800, 0x00000400 },
  { "GPU_VPC_BLK_DEC3",                            0x00024c00, 0x00000400 },
  { "GPU_VPC_BLK_DEC4",                            0x00025000, 0x00000200 },
  { "GPU_VPC_BLK_DEC5",                            0x00025200, 0x00000200 },
  { "GPU_VPC_BLK_DEC6",                            0x00025400, 0x00000200 },
  { "GPU_VPC_BLK_DEC7",                            0x00025600, 0x00000200 },
  { "GPU_VPC_BLK_DEC8",                            0x00025800, 0x00000800 },
  { "GPU_VPC_BLK_DEC9",                            0x00035000, 0x00000800 },
  { "GPU_PC_BLK_DEC0",                             0x00026000, 0x00000600 },
  { "GPU_PC_BLK_DEC1",                             0x00026600, 0x00000600 },
  { "GPU_PC_BLK_DEC2",                             0x00026c00, 0x00000400 },
  { "GPU_PC_BLK_DEC3",                             0x00027000, 0x00000300 },
  { "GPU_PC_BLK_DEC4",                             0x00027300, 0x00000300 },
  { "GPU_PC_BLK_DEC5",                             0x00027600, 0x00000200 },
  { "GPU_PC_BLK_DEC6",                             0x00027800, 0x00000800 },
  { "GPU_VFD_BLK_DEC0",                            0x00028000, 0x00001000 },
  { "GPU_VFD_BLK_DEC1",                            0x00029000, 0x00000800 },
  { "GPU_VFD_BLK_DEC2",                            0x00029800, 0x00000800 },
  { "GPU_VFD_BLK_DEC3",                            0x00034800, 0x00000800 },
  { "GPU_SP_BLK_DEC0",                             0x0002a000, 0x00000600 },
  { "GPU_SP_BLK_DEC1",                             0x0002a600, 0x00000600 },
  { "GPU_SP_BLK_DEC2",                             0x0002ac00, 0x00000400 },
  { "GPU_SP_BLK_DEC3",                             0x0002b000, 0x00000300 },
  { "GPU_SP_BLK_DEC4",                             0x0002b300, 0x00000300 },
  { "GPU_SP_BLK_DEC5",                             0x0002b600, 0x00000200 },
  { "GPU_SP_BLK_DEC6",                             0x0002b800, 0x00000800 },
  { "GPU_TPL1_BLK_DEC0",                           0x0002c000, 0x00000600 },
  { "GPU_TPL1_BLK_DEC1",                           0x0002c600, 0x00000600 },
  { "GPU_TPL1_BLK_DEC2",                           0x0002cc00, 0x00000400 },
  { "GPU_TPL1_BLK_DEC3",                           0x0002d000, 0x00000300 },
  { "GPU_TPL1_BLK_DEC4",                           0x0002d300, 0x00000300 },
  { "GPU_TPL1_BLK_DEC5",                           0x0002d600, 0x00000200 },
  { "GPU_TPL1_BLK_DEC6",                           0x0002d800, 0x00000800 },
  { "GPU_HLSQ_BLK_DEC0",                           0x0002e000, 0x00000600 },
  { "GPU_HLSQ_BLK_DEC1",                           0x0002e600, 0x00000600 },
  { "GPU_HLSQ_BLK_DEC2",                           0x0002ec00, 0x00000400 },
  { "GPU_HLSQ_BLK_DEC3",                           0x0002f000, 0x00000300 },
  { "GPU_HLSQ_BLK_DEC4",                           0x0002f300, 0x00000300 },
  { "GPU_HLSQ_BLK_DEC5",                           0x0002f600, 0x00000200 },
  { "GPU_HLSQ_BLK_DEC6",                           0x0002f800, 0x00002800 },
  { "GPU_HLSQ_BLK_DEC7",                           0x00032000, 0x00002800 },
  { "GPU_GMU_AO_BLK_DEC0",                         0x0008ec00, 0x00001400 },
  { "GPU_GMU_CX_BLK_DEC0",                         0x0007d000, 0x0000c000 },
  { "GPU_GMU_GX_BLK_DEC0",                         0x0006a000, 0x00000800 },
  { "GPU_DBGC_BLK_DEC0",                           0x00001800, 0x00000800 },
  { "GPU_CX_DBGC_BLK_DEC0",                        0x00061000, 0x00000800 },
  { "GPU_BROADCAST_BLK_DEC0",                      0x00035800, 0x00000400 },
  { "GPU_BROADCAST_BLK_DEC1",                      0x00035c00, 0x00000400 },
  { "GPU_SMMU_APERTURE_BLK_DEC0",                  0x00060000, 0x00001000 },
  { "GPU_CX_MISC_BLK_DEC0",                        0x0009e000, 0x00001000 },
  { "GPU_GBIF_BLK_DEC0",                           0x0000f000, 0x00001000 },
  { "GPU_SMMU",                                    0x00040000, 0x00010000 },
  { "GPU_SMMU_GLOBAL0",                            0x00040000, 0x00001000 },
  { "GPU_SMMU_GLOBAL1",                            0x00041000, 0x00001000 },
  { "GPU_SMMU_IMPL_DEF0",                          0x00042000, 0x00001000 },
  { "GPU_SMMU_PERF_MON",                           0x00043000, 0x00001000 },
  { "GPU_SMMU_SSD",                                0x00044000, 0x00001000 },
  { "GPU_SMMU_IMPL_DEF1",                          0x00046000, 0x00001000 },
  { "GPU_A_CB0_CB0_SMMU_CONTEXT_BANK",             0x00048000, 0x00001000 },
  { "GPU_A_CB1_CB1_SMMU_CONTEXT_BANK",             0x00049000, 0x00001000 },
  { "GPU_A_CB2_CB2_SMMU_CONTEXT_BANK",             0x0004a000, 0x00001000 },
  { "GPU_A_CB3_CB3_SMMU_CONTEXT_BANK",             0x0004b000, 0x00001000 },
  { "GPU_A_CB4_CB4_SMMU_CONTEXT_BANK",             0x0004c000, 0x00001000 },
  { "GPU_A_CB5_CB5_SMMU_CONTEXT_BANK",             0x0004d000, 0x00001000 },
  { "GPU_A_CB6_CB6_SMMU_CONTEXT_BANK",             0x0004e000, 0x00001000 },
  { "GPU_A_CB7_CB7_SMMU_CONTEXT_BANK",             0x0004f000, 0x00001000 },
  { "GPUCC",                                       0x00090000, 0x00009000 },
  { "GPUCC_GPU_CC_GPU_CC_GPU_CC_REG",              0x00090000, 0x00009000 },
  { "GPUCC_GPU_CC_ACD_GPU_CC_ACD_ACD",             0x00099000, 0x00000800 },
  { "GPU_CPR",                                     0x0009a000, 0x00004000 },
  { "GPU_RSCC",                                    0x0008d000, 0x00001c00 },
  { "GPU_RSCC_RSCC_RSC",                           0x0008d000, 0x00010000 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_GDPM_RAM_START_ADDRESS[] =
{
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_GPUCX_ITCM[] =
{
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_GPUCX_DTCM[] =
{
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_QDSS_SOC_DBG[] =
{
  { "QDSS_APSS_APSS_APSS_DBG",                     0x01000000, 0x00800000 },
  { "QDSS_APSS_APSS_APB_APSS_APB_KRYO3SS_TOP_DEBUG_BLOCK", 0x01000000, 0x00741000 },
  { "QDSS_APSS_DL_DL_DRAGONLINK_SLV_CS8254B64",    0x01860000, 0x00003000 },
  { "QDSS_APSS_DL_TPDM0_TPDM0_TPDM_ATB32_DSB64_CSF8B13A8B", 0x01860000, 0x00001000 },
  { "QDSS_APSS_DL_TPDM0_TPDM_ATB32_DSB64_CSF8B13A8B_SUB", 0x01860280, 0x00000d80 },
  { "QDSS_APSS_DL_TPDM0_TPDM_ATB32_DSB64_CSF8B13A8B_GPR", 0x01860000, 0x0000027d },
  { "QDSS_APSS_DL_CTI0_CTI0_QC_CTI_CORE",          0x01861000, 0x00001000 },
  { "QDSS_APSS_DL_TPDA_TPDA_TPDA_S1_W32_D2_M32_CS78201FC2", 0x01862000, 0x00001000 },
  { "QDSS_APSS_APSS_APSS_DEBUG_UI",                0x01890000, 0x00001000 },
  { "QDSS_APSS_APSS_APSS_APB_SDC_CSF58A8734",      0x01870000, 0x00001000 },
  { "QDSS_APSS_DL_OLC_DL_OLC_DRAGONLINK_SLV_CS2CE2C3AD", 0x01830000, 0x00003000 },
  { "QDSS_APSS_DL_OLC_TPDM0_TPDM0_TPDM_ATB32_CMB64_CSE183A91F", 0x01830000, 0x00001000 },
  { "QDSS_APSS_DL_OLC_TPDM0_TPDM_ATB32_CMB64_CSE183A91F_SUB", 0x01830280, 0x00000d80 },
  { "QDSS_APSS_DL_OLC_TPDM0_TPDM_ATB32_CMB64_CSE183A91F_GPR", 0x01830000, 0x0000027d },
  { "QDSS_APSS_DL_OLC_CTI0_CTI0_QC_CTI_CORE",      0x01831000, 0x00001000 },
  { "QDSS_APSS_DL_OLC_TPDA_TPDA_TPDA_S1_W32_D2_M64_CSE87E6F2", 0x01832000, 0x00001000 },
  { "QDSS_APSS_CTI_0_CTI_0_CSCTI",                 0x018e0000, 0x00001000 },
  { "QDSS_APSS_CTI_1_CTI_1_CSCTI",                 0x018f0000, 0x00001000 },
  { "QDSS_APSS_CTI_2_CTI_2_CSCTI",                 0x01900000, 0x00001000 },
  { "QDSS_APSS_FUN_FUN_CXATBFUNNEL_128W8SP",       0x01810000, 0x00001000 },
  { "QDSS_APSS_FUN_ATB_FUN_ATB_CXATBFUNNEL_128W8SP", 0x01800000, 0x00001000 },
  { "QDSS_APSS_APSS_GNOC_APSS_GNOC_GLADIATOR_NOC", 0x01840000, 0x0000d080 },
  { "QDSS_APSS_LLM_TPDM_SILVER_LLM_TPDM_SILVER_TPDM_ATB32_APCLK_CMB32_CSE533A018", 0x018a0000, 0x00001000 },
  { "QDSS_APSS_LLM_TPDM_SILVER_TPDM_ATB32_APCLK_CMB32_CSE533A018_SUB", 0x018a0280, 0x00000d80 },
  { "QDSS_APSS_LLM_TPDM_SILVER_TPDM_ATB32_APCLK_CMB32_CSE533A018_GPR", 0x018a0000, 0x0000027d },
  { "QDSS_APSS_LLM_TPDM_GOLD_LLM_TPDM_GOLD_TPDM_ATB32_APCLK_CMB32_CSE533A018", 0x018b0000, 0x00001000 },
  { "QDSS_APSS_LLM_TPDM_GOLD_TPDM_ATB32_APCLK_CMB32_CSE533A018_SUB", 0x018b0280, 0x00000d80 },
  { "QDSS_APSS_LLM_TPDM_GOLD_TPDM_ATB32_APCLK_CMB32_CSE533A018_GPR", 0x018b0000, 0x0000027d },
  { "QDSS_APSS_LLM_TPDA_SILVER_LLM_TPDA_SILVER_TPDA_S1_W32_D2_M32_CS78201FC2", 0x018c0000, 0x00001000 },
  { "QDSS_APSS_LLM_TPDA_GOLD_LLM_TPDA_GOLD_TPDA_S1_W32_D2_M32_CS78201FC2", 0x018d0000, 0x00001000 },
  { "QDSS_DLCT_DLCT_DRAGONLINK_SLV_CSF2C0153D",    0x00c28000, 0x00003000 },
  { "QDSS_DLCT_TPDM0_TPDM0_TPDM_ATB32_DSB128_CS44C77AE0", 0x00c28000, 0x00001000 },
  { "QDSS_DLCT_TPDM0_TPDM_ATB32_DSB128_CS44C77AE0_SUB", 0x00c28280, 0x00000d80 },
  { "QDSS_DLCT_TPDM0_TPDM_ATB32_DSB128_CS44C77AE0_GPR", 0x00c28000, 0x0000027d },
  { "QDSS_DLCT_CTI0_CTI0_QC_CTI_CORE",             0x00c29000, 0x00001000 },
  { "QDSS_DLCT_CTI1_CTI1_QC_CTI_CORE",             0x00c2a000, 0x00001000 },
  { "QDSS_VENUS_A9_VENUS_A9_A9ETM",                0x00c21000, 0x00001000 },
  { "QDSS_VENUS_CTI_VENUS_CTI_CSCTI",              0x00c20000, 0x00001000 },
  { "QDSS_TITAN_TITAN_A5SS",                       0x00c10000, 0x00008000 },
  { "QDSS_TITAN_CA5",                              0x00c10000, 0x00002000 },
  { "QDSS_TITAN_B_B_ETMA5",                        0x00c12000, 0x00001000 },
  { "QDSS_TITAN_A_A_CSCTI",                        0x00c13000, 0x00001000 },
  { "QDSS_DLMM_DLMM_DRAGONLINK_SLV_CS2DF5D592",    0x00c08000, 0x00003000 },
  { "QDSS_DLMM_TPDM0_TPDM0_TPDM_ATB32_DSB128_CS44C77AE0", 0x00c08000, 0x00001000 },
  { "QDSS_DLMM_TPDM0_TPDM_ATB32_DSB128_CS44C77AE0_SUB", 0x00c08280, 0x00000d80 },
  { "QDSS_DLMM_TPDM0_TPDM_ATB32_DSB128_CS44C77AE0_GPR", 0x00c08000, 0x0000027d },
  { "QDSS_DLMM_CTI0_CTI0_QC_CTI_CORE",             0x00c09000, 0x00001000 },
  { "QDSS_DLMM_CTI1_CTI1_QC_CTI_CORE",             0x00c0a000, 0x00001000 },
  { "QDSS_DLMM_QATBFUNNEL_QATBFUNNEL_QATBFUNNEL_32W8SP", 0x00c0b000, 0x00001000 },
  { "QDSS_DDR_SDC_DDR_SDC_APB_SDC_CS7056C9E1",     0x00c00000, 0x00001000 },
  { "QDSS_PMIC_ARB_PMIC_ARB_PMIC_ARB_DEBUG",       0x00b22000, 0x0000005d },
  { "QDSS_AOP_M3_CTI_AOP_M3_CTI_CSCTI",            0x00b21000, 0x00001000 },
  { "QDSS_RPMH_SDC_RPMH_SDC_APB_SDC_CSC6F9FF3C",   0x00b20000, 0x00001000 },
  { "QDSS_SSC_SSC_SSC_DBG",                        0x00b10000, 0x00010000 },
  { "QDSS_SSC_CTI_SDC_2_CTI_SDC_2_CSCTI",          0x00b10000, 0x00001000 },
  { "QDSS_SSC_CTI_1_CTI_1_CSCTI",                  0x00b11000, 0x00001000 },
  { "QDSS_SSC_STM_STM_CXSTM_2_4_4_TRUE",           0x00b13000, 0x00001000 },
  { "QDSS_SSC_FUN0_FUN0_CXATBFUNNEL_32W8SP",       0x00b14000, 0x00001000 },
  { "QDSS_SSC_SSC_ETFETB_SSC_ETFETB_CXTMC_F32W16K", 0x00b15000, 0x00001000 },
  { "QDSS_SSC_DL_DL_DRAGONLINK_SLV_ATB32_DSB32",   0x00b16000, 0x00002000 },
  { "QDSS_SSC_DL_TPDM0_TPDM0_TPDM_ATB32_DSB32_CS3493730B", 0x00b16000, 0x00001000 },
  { "QDSS_SSC_DL_TPDM0_TPDM_ATB32_DSB32_CS3493730B_SUB", 0x00b16280, 0x00000d80 },
  { "QDSS_SSC_DL_TPDM0_TPDM_ATB32_DSB32_CS3493730B_GPR", 0x00b16000, 0x0000027d },
  { "QDSS_SSC_DL_TPDA_TPDA_TPDA_S1_W32_D2_M32_CS9FAD450C", 0x00b17000, 0x00001000 },
  { "QDSS_SSC_QDSP6_QDSP6_QDSP6_DBG",              0x00b18000, 0x00004000 },
  { "QDSS_SSC_QDSP6_CTI_0_CTI_0_CSCTI",            0x00b1b000, 0x00001000 },
  { "QDSS_SSC_CTI_4_CTI_4_CSCTI",                  0x00b1e000, 0x00001000 },
  { "QDSS_SSC_CTI_6_CTI_6_CSCTI",                  0x00b1f000, 0x00001000 },
  { "QDSS_SWAO_SWAO_SWAO_DEBUG",                   0x00b00000, 0x00010000 },
  { "QDSS_SWAO_TPDA_TPDA_TPDA_S2_W128_D8_M64_CSE2C17BFE", 0x00b01000, 0x00001000 },
  { "QDSS_SWAO_TPDM0_TPDM0_TPDM_ATB128_MCMB4_CS6E88EB34", 0x00b02000, 0x00001000 },
  { "QDSS_SWAO_TPDM0_TPDM_ATB128_MCMB4_CS6E88EB34_SUB", 0x00b02280, 0x00000d80 },
  { "QDSS_SWAO_TPDM0_TPDM_ATB128_MCMB4_CS6E88EB34_GPR", 0x00b02000, 0x0000027d },
  { "QDSS_SWAO_TPDM1_TPDM1_TPDM_ATB32_DSB64_CSF8B13A8B", 0x00b03000, 0x00001000 },
  { "QDSS_SWAO_TPDM1_TPDM_ATB32_DSB64_CSF8B13A8B_SUB", 0x00b03280, 0x00000d80 },
  { "QDSS_SWAO_TPDM1_TPDM_ATB32_DSB64_CSF8B13A8B_GPR", 0x00b03000, 0x0000027d },
  { "QDSS_SWAO_SWAO_CTI0_SWAO_CTI0_CSCTI",         0x00b04000, 0x00001000 },
  { "QDSS_SWAO_SWAO_CTI1_SWAO_CTI1_CSCTI",         0x00b05000, 0x00001000 },
  { "QDSS_SWAO_SWAO_CTI2_SWAO_CTI2_CSCTI",         0x00b06000, 0x00001000 },
  { "QDSS_SWAO_SWAO_CTI3_SWAO_CTI3_CSCTI",         0x00b07000, 0x00001000 },
  { "QDSS_SWAO_FUN0_FUN0_CXATBFUNNEL_128W8SP",     0x00b08000, 0x00001000 },
  { "QDSS_SWAO_TMC_TMC_CXTMC_F128W32K",            0x00b09000, 0x00001000 },
  { "QDSS_SWAO_REPLIC_REPLIC_CXATBREPLICATOR_64WP", 0x00b0a000, 0x00001000 },
  { "QDSS_SWAO_IPCBMON_TGU_IPCBMON_TGU_QC_TGU_APCLK_CS5E8D06A4", 0x00b0c000, 0x00001000 },
  { "QDSS_SWAO_SWAOCSR_SWAOCSR_SWAOCSR",           0x00b0e000, 0x00001000 },
  { "QDSS_TPDMNT_TPDMNT_TPDM_ATB32_APCLK_DSB128_CS498A5AE1", 0x00a24000, 0x00001000 },
  { "QDSS_TPDMNT_TPDM_ATB32_APCLK_DSB128_CS498A5AE1_SUB", 0x00a24280, 0x00000d80 },
  { "QDSS_TPDMNT_TPDM_ATB32_APCLK_DSB128_CS498A5AE1_GPR", 0x00a24000, 0x0000027d },
  { "QDSS_DDRSS_DDRSS_DDR_SS_APB",                 0x009e0000, 0x00018000 },
  { "QDSS_DDRSS_DDR_DL0_DDR_DL0_DRAGONLINK_SLV_CS312FFBBC", 0x009e0000, 0x00003000 },
  { "QDSS_DDRSS_DDR_DL0_TPDM0_TPDM0_TPDM_ATB32_CMB32_DSB256_CSB373EF4A", 0x009e0000, 0x00001000 },
  { "QDSS_DDRSS_DDR_DL0_TPDM0_TPDM_ATB32_CMB32_DSB256_CSB373EF4A_SUB", 0x009e0280, 0x00000d80 },
  { "QDSS_DDRSS_DDR_DL0_TPDM0_TPDM_ATB32_CMB32_DSB256_CSB373EF4A_GPR", 0x009e0000, 0x0000027d },
  { "QDSS_DDRSS_DDR_DL0_CTI0_CTI0_QC_CTI_CORE",    0x009e1000, 0x00001000 },
  { "QDSS_DDRSS_DDR_DL0_QATBFUNNEL_QATBFUNNEL_QATBFUNNEL_32W8SP", 0x009e2000, 0x00001000 },
  { "QDSS_DDRSS_DDR_DL1_DDR_DL1_DRAGONLINK_SLV_CS254E60D9", 0x009e4000, 0x00004000 },
  { "QDSS_DDRSS_DDR_DL1_CTI0_CTI0_QC_CTI_CORE",    0x009e4000, 0x00001000 },
  { "QDSS_DDRSS_DDR_DL1_CTI1_CTI1_QC_CTI_CORE",    0x009e5000, 0x00001000 },
  { "QDSS_DDRSS_DDR_DL1_CTI2_CTI2_QC_CTI_CORE",    0x009e6000, 0x00001000 },
  { "QDSS_DDRSS_DDR_DL1_QATBFUNNEL_QATBFUNNEL_QATBFUNNEL_32W8SP", 0x009e7000, 0x00001000 },
  { "QDSS_DDRSS_DDR_SHRM_DDR_SHRM_SHRM_APB",       0x009e8000, 0x00004000 },
  { "QDSS_DDRSS_CH0_TGU0_CH0_TGU0_QC_TGU_APCLK_TC_CS734CAB6E", 0x009ec000, 0x00001000 },
  { "QDSS_DDRSS_CH0_TGU1_CH0_TGU1_QC_TGU_APCLK_TC_CS734CAB6E", 0x009ed000, 0x00001000 },
  { "QDSS_DDRSS_CH0_TGU2_CH0_TGU2_QC_TGU_APCLK_TC_CSFA82A8A7", 0x009ee000, 0x00001000 },
  { "QDSS_DDRSS_CH1_TGU0_CH1_TGU0_QC_TGU_APCLK_TC_CS734CAB6E", 0x009ef000, 0x00001000 },
  { "QDSS_DDRSS_CH1_TGU1_CH1_TGU1_QC_TGU_APCLK_TC_CS734CAB6E", 0x009f0000, 0x00001000 },
  { "QDSS_DDRSS_CH1_TGU2_CH1_TGU2_QC_TGU_APCLK_TC_CSFA82A8A7", 0x009f1000, 0x00001000 },
  { "QDSS_DDRSS_CH2_TGU0_CH2_TGU0_QC_TGU_APCLK_TC_CS734CAB6E", 0x009f2000, 0x00001000 },
  { "QDSS_DDRSS_CH2_TGU1_CH2_TGU1_QC_TGU_APCLK_TC_CS734CAB6E", 0x009f3000, 0x00001000 },
  { "QDSS_DDRSS_CH2_TGU2_CH2_TGU2_QC_TGU_APCLK_TC_CSFA82A8A7", 0x009f4000, 0x00001000 },
  { "QDSS_DDRSS_CH3_TGU0_CH3_TGU0_QC_TGU_APCLK_TC_CS734CAB6E", 0x009f5000, 0x00001000 },
  { "QDSS_DDRSS_CH3_TGU1_CH3_TGU1_QC_TGU_APCLK_TC_CS734CAB6E", 0x009f6000, 0x00001000 },
  { "QDSS_DDRSS_CH3_TGU2_CH3_TGU2_QC_TGU_APCLK_TC_CSFA82A8A7", 0x009f7000, 0x00001000 },
  { "QDSS_QM_QM_TPDM_ATB32_APCLK_GPRCLK_BC32_DSB128_CSFE693679", 0x009d0000, 0x00001000 },
  { "QDSS_QM_TPDM_ATB32_APCLK_GPRCLK_BC32_DSB128_CSFE693679_SUB", 0x009d0280, 0x00000d80 },
  { "QDSS_QM_TPDM_ATB32_APCLK_GPRCLK_BC32_DSB128_CSFE693679_GPR", 0x009d0000, 0x0000027d },
  { "QDSS_DLST_DLST_DRAGONLINK_SLV_CS42574369",    0x009c0000, 0x00002000 },
  { "QDSS_DLST_TPDM0_TPDM0_TPDM_ATB32_DSB128_CSE15E997B", 0x009c0000, 0x00001000 },
  { "QDSS_DLST_TPDM0_TPDM_ATB32_DSB128_CSE15E997B_SUB", 0x009c0280, 0x00000d80 },
  { "QDSS_DLST_TPDM0_TPDM_ATB32_DSB128_CSE15E997B_GPR", 0x009c0000, 0x0000027d },
  { "QDSS_DLST_QATBFUNNEL_QATBFUNNEL_QATBFUNNEL_64W8SP", 0x009c1000, 0x00001000 },
  { "QDSS_WCSS_WCSS_WCSS_DBG",                     0x00998000, 0x00020000 },
  { "QDSS_WCSS_ROM_ROM_WCSS_DBG_DAPROM",           0x00998000, 0x00001000 },
  { "QDSS_WCSS_CSR_CSR_WCSS_DBG_CSR",              0x00999000, 0x00001000 },
  { "QDSS_WCSS_TSGEN_TSGEN_CXTSGEN",               0x0099a000, 0x00001000 },
  { "QDSS_WCSS_TPDM_TPDM_TPDM_ATB64_GPRCLK_CMB40_DSB256_CSC0E9EC9D", 0x0099c000, 0x00001000 },
  { "QDSS_WCSS_TPDM_TPDM_ATB64_GPRCLK_CMB40_DSB256_CSC0E9EC9D_SUB", 0x0099c280, 0x00000d80 },
  { "QDSS_WCSS_TPDM_TPDM_ATB64_GPRCLK_CMB40_DSB256_CSC0E9EC9D_GPR", 0x0099c000, 0x0000027d },
  { "QDSS_WCSS_TPDA_TPDA_TPDA_S1_W128_D8_M64_CS5CD79236", 0x0099d000, 0x00001000 },
  { "QDSS_WCSS_FUN_FUN_CXATBFUNNEL_128W2SP",       0x0099e000, 0x00001000 },
  { "QDSS_WCSS_ETFETB_ETFETB_CXTMC_F128W8K",       0x0099f000, 0x00001000 },
  { "QDSS_WCSS_CTI0_CTI0_CSCTI",                   0x009a4000, 0x00001000 },
  { "QDSS_WCSS_CTI1_CTI1_CSCTI",                   0x009a5000, 0x00001000 },
  { "QDSS_WCSS_CTI2_CTI2_CSCTI",                   0x009a6000, 0x00001000 },
  { "QDSS_WCSS_GPR_GPR_CXGPR_8M",                  0x009b6000, 0x00001000 },
  { "QDSS_WCSS_DSBMUX_DSBMUX_WCSS_DBG_DSBMUX",     0x009b7000, 0x00001000 },
  { "QDSS_NAV_NAV_NAV_DBG",                        0x00990000, 0x00002000 },
  { "QDSS_NAV_DLSLV_DLSLV_DRAGONLINK_SLV_CS84036DC7", 0x00990000, 0x00002000 },
  { "QDSS_NAV_DLSLV_TPDM0_TPDM0_TPDM_ATB32_GPRCLK_CMB32_CS5A073C07", 0x00990000, 0x00001000 },
  { "QDSS_NAV_DLSLV_TPDM0_TPDM_ATB32_GPRCLK_CMB32_CS5A073C07_SUB", 0x00990280, 0x00000d80 },
  { "QDSS_NAV_DLSLV_TPDM0_TPDM_ATB32_GPRCLK_CMB32_CS5A073C07_GPR", 0x00990000, 0x0000027d },
  { "QDSS_NAV_DLSLV_TPDA_TPDA_TPDA_S1_W32_D2_M32_CS78201FC2", 0x00991000, 0x00001000 },
  { "QDSS_A6X_A6X_GFX_DBG",                        0x00900000, 0x00080000 },
  { "QDSS_A6X_SP0_ISDB_SP0_ISDB_GFX_ISDB_0",       0x00900000, 0x00001000 },
  { "QDSS_A6X_HLSQ_ISDB_HLSQ_ISDB_ISDB_HLSQ_REG_DB", 0x00910000, 0x00001000 },
  { "QDSS_A6X_TPDM_TPDM_TPDM_ATB32_APCLK_DSB32_CS6666B348", 0x00940000, 0x00001000 },
  { "QDSS_A6X_TPDM_TPDM_ATB32_APCLK_DSB32_CS6666B348_SUB", 0x00940280, 0x00000d80 },
  { "QDSS_A6X_TPDM_TPDM_ATB32_APCLK_DSB32_CS6666B348_GPR", 0x00940000, 0x0000027d },
  { "QDSS_A6X_ISDB_CTI_ISDB_CTI_CSCTI",            0x00941000, 0x00001000 },
  { "QDSS_A6X_GPMU_CTI_GPMU_CTI_CSCTI",            0x00942000, 0x00001000 },
  { "QDSS_A6X_CX_DBGC_ATBF_CX_DBGC_ATBF_CXATBFUNNEL_128W4SP", 0x00943000, 0x00001000 },
  { "QDSS_A6X_CX_DRGN_ATBF_CX_DRGN_ATBF_QATBFUNNEL_32W8SP", 0x00944000, 0x00001000 },
  { "QDSS_DCC_DCC_QC_DCC_CSAFB9399A_APB",          0x00870000, 0x00001000 },
  { "QDSS_DCC_DCC_TPDM_DCC_TPDM_TPDM_ATB8_ATCLK_CMB32_CS6DCF0636", 0x00870000, 0x00001000 },
  { "QDSS_DCC_DCC_TPDM_TPDM_ATB8_ATCLK_CMB32_CS6DCF0636_SUB", 0x00870280, 0x00000d80 },
  { "QDSS_DCC_DCC_TPDM_TPDM_ATB8_ATCLK_CMB32_CS6DCF0636_GPR", 0x00870000, 0x0000027d },
  { "QDSS_TURING_APB_TURING_APB_TURING_APB",       0x00860000, 0x00008000 },
  { "QDSS_TURING_APB_TURING_TURING_DRAGONLINK_SLV_CS4C56784E", 0x00860000, 0x00002000 },
  { "QDSS_TURING_APB_TURING_TPDM0_TPDM0_TPDM_ATB32_DSB32_CS3493730B", 0x00860000, 0x00001000 },
  { "QDSS_TURING_APB_TURING_TPDM0_TPDM_ATB32_DSB32_CS3493730B_SUB", 0x00860280, 0x00000d80 },
  { "QDSS_TURING_APB_TURING_TPDM0_TPDM_ATB32_DSB32_CS3493730B_GPR", 0x00860000, 0x0000027d },
  { "QDSS_TURING_APB_TURING_QATBFUNNEL_QATBFUNNEL_QATBFUNNEL_64W8SP", 0x00861000, 0x00001000 },
  { "QDSS_TURING_APB_TURING_CTI_TURING_CTI_CSCTI", 0x00867000, 0x00001000 },
  { "QDSS_APB2JTAG_APB2JTAG_APB2JTAG",             0x00858000, 0x00001000 },
  { "QDSS_PIMEM_PIMEM_RAMBLUR_PIMEM_APB",          0x00850000, 0x00001000 },
  { "QDSS_PIMEM_PIMEM_TPDM_PIMEM_TPDM_TPDM_ATB64_APCLK_GPRCLK_BC8_TC2_CMB64_DSB64_CSAF4CCF0", 0x00850000, 0x00001000 },
  { "QDSS_PIMEM_PIMEM_TPDM_TPDM_ATB64_APCLK_GPRCLK_BC8_TC2_CMB64_DSB64_CSAF4CCF0_SUB", 0x00850280, 0x00000d80 },
  { "QDSS_PIMEM_PIMEM_TPDM_TPDM_ATB64_APCLK_GPRCLK_BC8_TC2_CMB64_DSB64_CSAF4CCF0_GPR", 0x00850000, 0x0000027d },
  { "QDSS_PRNG_PRNG_PRNG_DBG",                     0x0084c000, 0x00004000 },
  { "QDSS_PRNG_PRNG_TPDM_PRNG_TPDM_TPDM_ATB32_APCLK_CMB32_CSA14715CD", 0x0084c000, 0x00001000 },
  { "QDSS_PRNG_PRNG_TPDM_TPDM_ATB32_APCLK_CMB32_CSA14715CD_SUB", 0x0084c280, 0x00000d80 },
  { "QDSS_PRNG_PRNG_TPDM_TPDM_ATB32_APCLK_CMB32_CSA14715CD_GPR", 0x0084c000, 0x0000027d },
  { "QDSS_VSENSE_VSENSE_VSENSE_CONTROLLER_DBG",    0x00840000, 0x00001000 },
  { "QDSS_VSENSE_TPDM_ATB8_ATCLK_CMB32_CS6DCF0636", 0x00840000, 0x00001000 },
  { "QDSS_VSENSE_TPDM_ATB8_ATCLK_CMB32_CS6DCF0636_SUB", 0x00840280, 0x00000d80 },
  { "QDSS_VSENSE_TPDM_ATB8_ATCLK_CMB32_CS6DCF0636_GPR", 0x00840000, 0x0000027d },
  { "QDSS_MSS_MSS_MSS_DBG",                        0x00830000, 0x00010000 },
  { "QDSS_MSS_DL_SLV_DL_SLV_DRAGONLINK_SLV_CS3DB134EF", 0x00830000, 0x00003000 },
  { "QDSS_MSS_DL_SLV_TPDM0_TPDM0_TPDM_ATB64_DSB128_CS9D17457E", 0x00830000, 0x00001000 },
  { "QDSS_MSS_DL_SLV_TPDM0_TPDM_ATB64_DSB128_CS9D17457E_SUB", 0x00830280, 0x00000d80 },
  { "QDSS_MSS_DL_SLV_TPDM0_TPDM_ATB64_DSB128_CS9D17457E_GPR", 0x00830000, 0x0000027d },
  { "QDSS_MSS_DL_SLV_TPDA_TPDA_TPDA_S1_W64_D8_M64_CS3BABB64A", 0x00831000, 0x00001000 },
  { "QDSS_MSS_DL_SLV_QATBFUNNEL_QATBFUNNEL_QATBFUNNEL_64W8SP", 0x00832000, 0x00001000 },
  { "QDSS_MSS_TPDA_TPDA_TPDA_S1_W64_D8_M64_CS3BABB64A", 0x00833000, 0x00001000 },
  { "QDSS_MSS_TPDM_TPDM_TPDM_ATB64_CMB14_DSB48_CS6B1A88C1", 0x00834000, 0x00001000 },
  { "QDSS_MSS_TPDM_TPDM_ATB64_CMB14_DSB48_CS6B1A88C1_SUB", 0x00834280, 0x00000d80 },
  { "QDSS_MSS_TPDM_TPDM_ATB64_CMB14_DSB48_CS6B1A88C1_GPR", 0x00834000, 0x0000027d },
  { "QDSS_MSS_QDSP6_CTI_QDSP6_CTI_CSCTI",          0x0083b000, 0x00001000 },
  { "QDSS_QDSS",                                   0x00000000, 0x00800000 },
  { "QDSS_QDSS_QDSS_APB",                          0x00000000, 0x00080000 },
  { "QDSS_QDSS_QDSS_CSR",                          0x00001000, 0x00001000 },
  { "QDSS_QDSS_CXSTM_8_32_32_TRUE",                0x00002000, 0x00001000 },
  { "QDSS_QDSS_QDSS_DL_SLV_QDSS_DL_SLV_DRAGONLINK_SLV_CS761FB224", 0x00004000, 0x00002000 },
  { "QDSS_QDSS_QDSS_DL_SLV_TPDA_TPDA_TPDA_S15_W64_D8_M64_CSB3C5C13B", 0x00004000, 0x00001000 },
  { "QDSS_QDSS_QDSS_DL_SLV_QATBFUNNEL_QATBFUNNEL_QATBFUNNEL_64W8SP", 0x00005000, 0x00001000 },
  { "QDSS_QDSS_TPDM0_TPDM0_TPDM_ATB64_DSB256_CSB5ABA515", 0x00006000, 0x00001000 },
  { "QDSS_QDSS_TPDM0_TPDM_ATB64_DSB256_CSB5ABA515_SUB", 0x00006280, 0x00000d80 },
  { "QDSS_QDSS_TPDM0_TPDM_ATB64_DSB256_CSB5ABA515_GPR", 0x00006000, 0x0000027d },
  { "QDSS_QDSS_TGU_HWE_TGU_HWE_QC_TGU_APCLK_TC_CSE0B89182", 0x0000e000, 0x00001000 },
  { "QDSS_QDSS_CTI0_CTI0_CSCTI",                   0x00010000, 0x00001000 },
  { "QDSS_QDSS_CTI1_CTI1_CSCTI",                   0x00011000, 0x00001000 },
  { "QDSS_QDSS_CTI2_CTI2_CSCTI",                   0x00012000, 0x00001000 },
  { "QDSS_QDSS_CTI3_CTI3_CSCTI",                   0x00013000, 0x00001000 },
  { "QDSS_QDSS_CTI4_CTI4_CSCTI",                   0x00014000, 0x00001000 },
  { "QDSS_QDSS_CTI5_CTI5_CSCTI",                   0x00015000, 0x00001000 },
  { "QDSS_QDSS_CTI6_CTI6_CSCTI",                   0x00016000, 0x00001000 },
  { "QDSS_QDSS_CTI7_CTI7_CSCTI",                   0x00017000, 0x00001000 },
  { "QDSS_QDSS_CTI8_CTI8_CSCTI",                   0x00018000, 0x00001000 },
  { "QDSS_QDSS_CTI9_CTI9_CSCTI",                   0x00019000, 0x00001000 },
  { "QDSS_QDSS_CTI10_CTI10_CSCTI",                 0x0001a000, 0x00001000 },
  { "QDSS_QDSS_CTI11_CTI11_CSCTI",                 0x0001b000, 0x00001000 },
  { "QDSS_QDSS_CTI12_CTI12_CSCTI",                 0x0001c000, 0x00001000 },
  { "QDSS_QDSS_CTI13_CTI13_CSCTI",                 0x0001d000, 0x00001000 },
  { "QDSS_QDSS_CTI14_CTI14_CSCTI",                 0x0001e000, 0x00001000 },
  { "QDSS_QDSS_CTI15_CTI15_CSCTI",                 0x0001f000, 0x00001000 },
  { "QDSS_QDSS_CSTPIU_CSTPIU_CSTPIU",              0x00040000, 0x00001000 },
  { "QDSS_QDSS_IN_FUN0_IN_FUN0_CXATBFUNNEL_128W8SP", 0x00041000, 0x00001000 },
  { "QDSS_QDSS_REPL64_REPL64_CXATBREPLICATOR_64WP", 0x00046000, 0x00001000 },
  { "QDSS_QDSS_ETR_ETR_CXTMC_R64W32D",             0x00048000, 0x00001000 },
  { "QDSS_QDSS_IN_FUN1_IN_FUN1_CXATBFUNNEL_128W8SP", 0x00042000, 0x00001000 },
  { "QDSS_QDSS_IN_FUN2_IN_FUN2_CXATBFUNNEL_128W8SP", 0x00043000, 0x00001000 },
  { "QDSS_QDSS_MERG_FUN_MERG_FUN_CXATBFUNNEL_128W3SP", 0x00045000, 0x00001000 },
  { "QDSS_QDSS_ETFETB_ETFETB_CXTMC_F128W64K",      0x00047000, 0x00001000 },
  { "QDSS_QDSS_VMIDETR_VMIDETR_VMIDMT_IDX_2_SSD1", 0x00049000, 0x00001000 },
  { "QDSS_QDSS_NDPBAM_NDPBAM_BAM_NDP_TOP_AUTO_SCALE_V2_0", 0x00060000, 0x00019000 },
  { "QDSS_QDSS_NDPBAM_BAM",                        0x00064000, 0x00015000 },
  { "QDSS_QDSS_QDSS_AHB",                          0x00080000, 0x00001000 },
  { "QDSS_QDSS_VMIDDAP_VMIDDAP_VMIDMT_IDX_2_SSD1", 0x00080000, 0x00001000 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_TURING[] =
{
  { "TURING_CC",                                   0x00000000, 0x00048000 },
  { "TURING_TCSR",                                 0x00080000, 0x0000f000 },
  { "TURING_QSM_DMA_TURING_QSM_VAPSS_DMA",         0x0009c000, 0x00003000 },
  { "TURING_QSM_DMA_QOS_QOS_GENERIC",              0x0009c000, 0x00000100 },
  { "TURING_QSM_DMA_QOS_QOS_DEBUG",                0x0009c100, 0x00000200 },
  { "TURING_QSM_DMA_QOS_QOS_HEALTH",               0x0009c500, 0x00000100 },
  { "TURING_QSM_DMA_QOS_QOS_INTERRUPTS",           0x0009d000, 0x00002000 },
  { "TURING_QSM_Q6",                               0x000a0000, 0x00003000 },
  { "TURING_QOS_QOS_GENERIC",                      0x000a0000, 0x00000100 },
  { "TURING_QOS_QOS_DEBUG",                        0x000a0100, 0x00000200 },
  { "TURING_QOS_QOS_DANGER",                       0x000a0300, 0x00000200 },
  { "TURING_QOS_QOS_HEALTH",                       0x000a0500, 0x00000100 },
  { "TURING_QOS_QOS_INTERRUPTS",                   0x000a1000, 0x00002000 },
  { "TURING_COMPUTE_RSCC",                         0x000a4000, 0x00004000 },
  { "TURING_RSCC_RSCC_RSC",                        0x000a4000, 0x00010000 },
  { "TURING_AHBE_TIME",                            0x000b7000, 0x00001000 },
  { "TURING_QDSP6V65SS",                           0x00300000, 0x00100000 },
  { "TURING_QDSP6V65SS_PUB",                       0x00300000, 0x00004040 },
  { "TURING_QDSP6V65SS_PRIVATE",                   0x00380000, 0x00080000 },
  { "TURING_QDSP6V65SS_CSR",                       0x00380000, 0x0000c23d },
  { "TURING_QDSP6V65SS_L2VIC",                     0x00390000, 0x00001000 },
  { "TURING_QDSP6SS_QDSP6SS_QTMR_AC",              0x003a0000, 0x00001000 },
  { "TURING_QDSP6SS_QTMR_F0_0",                    0x003a1000, 0x00001000 },
  { "TURING_QDSP6SS_QTMR_F1_1",                    0x003a2000, 0x00001000 },
  { "TURING_QDSP6SS_QTMR_F2_2",                    0x003a3000, 0x00001000 },
  { "TURING_QDSP6SS_QDSP6V65SS_RSCC",              0x003b0000, 0x00010000 },
  { "TURING_QDSP6SS_RSCC_RSCC_RSC",                0x003b0000, 0x00010000 },
  { "TURING_QDSP6SS_QDSP6SS_CPR3",                 0x003c0000, 0x00004000 },
  { "TURING_VAPSS",                                0x002c0000, 0x00040000 },
  { "TURING_UBWCDMA_TOP",                          0x002c0000, 0x00010000 },
  { "TURING_UBWCDMA",                              0x002c0000, 0x00010000 },
  { "TURING_VAPSS_NOC",                            0x002d0000, 0x00001600 },
  { "TURING_CDSP_NOC",                             0x000a8000, 0x00001400 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_PERIPH_SS_SDC2_SDCC5_TOP[] =
{
  { "PERIPH_SS_SDC2_SDCC_SDCC5_HC",                0x00004000, 0x00001000 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_PERIPH_SS_SDC4_SDCC5_TOP[] =
{
  { "PERIPH_SS_SDC4_SDCC_SDCC5_HC",                0x00004000, 0x00001000 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_PERIPH_SS_TSIF_TSIF_12SEG_WRAPPER[] =
{
  { "PERIPH_SS_TSIF_TSIF_TSIF_BAM_LITE_TOP_AUTO_SCALE_V2_0_XPU3", 0x00000000, 0x00027000 },
  { "PERIPH_SS_TSIF_TSIF_BAM",                     0x00004000, 0x00023000 },
  { "PERIPH_SS_TSIF_TSIF_0_TSIF_0_TSIF",           0x00027000, 0x00000200 },
  { "PERIPH_SS_TSIF_TSIF_1_TSIF_1_TSIF",           0x00028000, 0x00000200 },
  { "PERIPH_SS_TSIF_TSIF_TSIF_TSPP",               0x00029000, 0x00001000 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_VSENSE_CONTROLLER[] =
{
  { "VSENSE_CONTROLLER",                           0x00000000, 0x00001000 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_PERIPH_SS_PDM_PERPH_WEB[] =
{
  { "PERIPH_SS_PDM_WEB_TCXO4",                     0x00000000, 0x00004000 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_PERIPH_SS_AHB2PHY_NORTH[] =
{
  { "PERIPH_SS_AHB2PHY_NORTH_AHB2PHY_NORTH_AHB2PHY_BROADCAST_SWMAN", 0x0000f000, 0x00000400 },
  { "PERIPH_SS_AHB2PHY_NORTH_AHB2PHY_NORTH_AHB2PHY_SWMAN", 0x0000e000, 0x00000400 },
  { "PERIPH_SS_USB3_DP_PHY_USB3_DP_PHY_CM_USB3_DP_CON_SW", 0x00008000, 0x00003000 },
  { "PERIPH_SS_USB3_DP_PHY_USB3_DP_COM_USB3_DP_COM_USB3_DP_COM", 0x00008000, 0x00000038 },
  { "PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_USB3_QSERDES_COM_USB3_DP_QMP_PLL", 0x00009000, 0x0000018c },
  { "PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_TXA_USB3_QSERDES_TXA_USB3_DP_QMP_TX", 0x00009200, 0x00000128 },
  { "PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_RXA_USB3_QSERDES_RXA_USB3_DP_QMP_RX", 0x00009400, 0x00000200 },
  { "PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_TXB_USB3_QSERDES_TXB_USB3_DP_QMP_TX", 0x00009600, 0x00000128 },
  { "PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_RXB_USB3_QSERDES_RXB_USB3_DP_QMP_RX", 0x00009800, 0x00000200 },
  { "PERIPH_SS_USB3_DP_PHY_USB3_PCS_MISC_USB3_PCS_MISC_USB3_PCS_MISC", 0x00009a00, 0x00000018 },
  { "PERIPH_SS_USB3_DP_PHY_USB3_PCS_USB3_PCS_USB3_PCS", 0x00009c00, 0x00000218 },
  { "PERIPH_SS_USB3_DP_PHY_DP_QSERDES_COM_DP_QSERDES_COM_USB3_DP_QMP_PLL", 0x0000a000, 0x0000018c },
  { "PERIPH_SS_USB3_DP_PHY_DP_QSERDES_TX0_DP_QSERDES_TX0_USB3_DP_QMP_TX", 0x0000a200, 0x00000128 },
  { "PERIPH_SS_USB3_DP_PHY_DP_QSERDES_RX0_DP_QSERDES_RX0_USB3_DP_QMP_RX", 0x0000a400, 0x00000200 },
  { "PERIPH_SS_USB3_DP_PHY_DP_QSERDES_TX1_DP_QSERDES_TX1_USB3_DP_QMP_TX", 0x0000a600, 0x00000128 },
  { "PERIPH_SS_USB3_DP_PHY_DP_QSERDES_RX1_DP_QSERDES_RX1_USB3_DP_QMP_RX", 0x0000a800, 0x00000200 },
  { "PERIPH_SS_USB3_DP_PHY_DP_DP_DP_PHY",          0x0000aa00, 0x0000017c },
  { "PERIPH_SS_PHY_REFGEN_NORTH_PHY_REFGEN_NORTH_CM_PHY_REFGEN", 0x00007000, 0x00000060 },
  { "PERIPH_SS_QUSB2PHY_QUSB2PHY_CM_QUSB2PHY_1PORT", 0x00002000, 0x00000400 },
  { "PERIPH_SS_QUSB2PHY_QUSB2PHY_PLL_QUSB2PHY_PLL_QUSB2PHY_QLINK_COM", 0x00002000, 0x000001e0 },
  { "PERIPH_SS_QUSB2PHY_QUSB2PHY_QUSB2PHY_QUSB2PHY_DIG", 0x00002200, 0x000000b0 },
  { "PERIPH_SS_EUD_EUD_EUD",                       0x00000000, 0x00002000 },
  { "PERIPH_SS_EUD_ACORE",                         0x00000000, 0x00001000 },
  { "PERIPH_SS_EUD_MODE_MANAGER",                  0x00001000, 0x00001000 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_USB3_PRI_USB30_PRIM[] =
{
  { "USB3_PRI_DWC_USB3",                           0x00000000, 0x0000cd00 },
  { "USB3_PRI_USB30_QSRAM_REGS",                   0x000fc000, 0x00000100 },
  { "USB3_PRI_USB30_QSCRATCH",                     0x000f8800, 0x00000400 },
  { "USB3_PRI_USB30_DBM_REGFILE",                  0x000f8000, 0x00000400 },
  { "USB3_PRI_USB30_BAM",                          0x00104000, 0x00017000 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_VIDEO_SS_WRAPPER[] =
{
  { "VENUS",                                       0x00000000, 0x00100000 },
  { "TOP_BASE",                                    0x00000000, 0x00000400 },
  { "VCODEC_TOP",                                  0x00000000, 0x00000400 },
  { "VSP_BASE",                                    0x00000400, 0x00000c00 },
  { "VCODEC_VSP_TOP",                              0x00000400, 0x00000400 },
  { "VCODEC_VSP_CCE",                              0x00000800, 0x00000400 },
  { "VCODEC_VSP_BSE",                              0x00000c00, 0x00000400 },
  { "VPP_BASE",                                    0x00001000, 0x00001c00 },
  { "VCODEC_VPP_TOP",                              0x00001000, 0x00000400 },
  { "VCODEC_VPP_SE",                               0x00001400, 0x00000400 },
  { "VCODEC_VPP_PE",                               0x00001800, 0x00000400 },
  { "VCODEC_VPP_TE",                               0x00001c00, 0x00000400 },
  { "VCODEC_VPP_FE",                               0x00002000, 0x00000400 },
  { "VCODEC_VPP_ISE",                              0x00002400, 0x00000400 },
  { "VCODEC_VPP_VPSS",                             0x00002800, 0x00000400 },
  { "DMA_BASE",                                    0x00003800, 0x00000800 },
  { "VCODEC_DMA",                                  0x00003800, 0x00000800 },
  { "VIDEO_NOC",                                   0x00004000, 0x00000580 },
  { "VCODEC_CORE1_TOP_BASE",                       0x00008000, 0x00000400 },
  { "VCODEC_CORE1_VCODEC_TOP",                     0x00008000, 0x00000400 },
  { "VCODEC_CORE1_VSP_BASE",                       0x00008400, 0x00000c00 },
  { "VCODEC_CORE1_VCODEC_VSP_TOP",                 0x00008400, 0x00000400 },
  { "VCODEC_CORE1_VCODEC_VSP_CCE",                 0x00008800, 0x00000400 },
  { "VCODEC_CORE1_VCODEC_VSP_BSE",                 0x00008c00, 0x00000400 },
  { "VCODEC_CORE1_VPP_BASE",                       0x00009000, 0x00001c00 },
  { "VCODEC_CORE1_VCODEC_VPP_TOP",                 0x00009000, 0x00000400 },
  { "VCODEC_CORE1_VCODEC_VPP_SE",                  0x00009400, 0x00000400 },
  { "VCODEC_CORE1_VCODEC_VPP_PE",                  0x00009800, 0x00000400 },
  { "VCODEC_CORE1_VCODEC_VPP_TE",                  0x00009c00, 0x00000400 },
  { "VCODEC_CORE1_VCODEC_VPP_FE",                  0x0000a000, 0x00000400 },
  { "VCODEC_CORE1_VCODEC_VPP_ISE",                 0x0000a400, 0x00000400 },
  { "VCODEC_CORE1_VCODEC_VPP_VPSS",                0x0000a800, 0x00000400 },
  { "VCODEC_CORE1_DMA_BASE",                       0x0000b800, 0x00000800 },
  { "VCODEC_CORE1_VCODEC_DMA",                     0x0000b800, 0x00000800 },
  { "VCODEC_CORE1_VIDEO_NOC",                      0x0000c000, 0x00000580 },
  { "VENUS_CPU_BASE",                              0x000c0000, 0x00020000 },
  { "CPU_WD",                                      0x000c1000, 0x00001000 },
  { "CPU_TM",                                      0x000c2000, 0x00001000 },
  { "CPU_CS",                                      0x000d2000, 0x00001000 },
  { "CPU_IQ",                                      0x000df000, 0x00001000 },
  { "VENUS_WRAPPER_BASE",                          0x000e0000, 0x00004000 },
  { "VCODEC_VENUS_WRAPPER_TOP",                    0x000e0000, 0x00004000 },
  { "VIDEO_CC",                                    0x00100000, 0x00020000 },
  { "VIDEO_CC_VIDEO_CC_REG",                       0x00100000, 0x00010000 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_TITAN_SS[] =
{
  { "TITAN",                                       0x00000000, 0x00100000 },
  { "TITAN_A_ICP",                                 0x00000000, 0x00020000 },
  { "TITAN_A_ICP_QGIC2",                           0x00000000, 0x00006000 },
  { "TITAN_A_ICP_QTMR",                            0x00008000, 0x00005000 },
  { "TITAN_A_ICP_QTMR_QTMR_AC",                    0x00008000, 0x00001000 },
  { "TITAN_A_ICP_QTMR_QTMR_V1",                    0x00009000, 0x00001000 },
  { "TITAN_A_ICP_QTMR_QTMR_V2",                    0x0000a000, 0x00001000 },
  { "TITAN_A_ICP_QTMR_QTMR_WDT_CTL",               0x0000b000, 0x00001000 },
  { "TITAN_A_ICP_QTMR_QTMR_WDT_REFRESH",           0x0000c000, 0x00001000 },
  { "TITAN_A_ICP_SIERRA_A5",                       0x00010000, 0x00008000 },
  { "TITAN_A_ICP_CSR",                             0x00018000, 0x00003000 },
  { "TITAN_A_CPAS_SEC_CPAS_TOP_SECURE",            0x00030000, 0x00010000 },
  { "TITAN_A_CPAS_0_CPAS_TOP_0",                   0x00040000, 0x00001000 },
  { "TITAN_A_CAMNOC",                              0x00042000, 0x00005000 },
  { "TITAN_A_CPAS_CDM",                            0x00048000, 0x00001000 },
  { "TITAN_A_CCI",                                 0x0004a000, 0x00004000 },
  { "TITAN_A_JPEG_0",                              0x0004e000, 0x00004000 },
  { "TITAN_A_JPEG_3",                              0x00052000, 0x00004000 },
  { "TITAN_A_FD_FD_WRAPPER",                       0x0005a000, 0x00004000 },
  { "TITAN_A_FD_A_FD",                             0x0005a000, 0x00001000 },
  { "TITAN_A_FD_A_FD_MISC",                        0x0005b000, 0x00000400 },
  { "TITAN_A_XPU3",                                0x00060000, 0x00001100 },
  { "TITAN_A_PHY_0",                               0x00065000, 0x00001000 },
  { "TITAN_A_PHY_0_CSIPHY_LN0_CSIPHY_LN0_CSI_2PHASE_DATALN", 0x00065000, 0x000000dc },
  { "TITAN_A_PHY_0_CSIPHY_LN1_CSIPHY_LN1_CSI_3PHASE_DATALN", 0x00065100, 0x00000100 },
  { "TITAN_A_PHY_0_CSIPHY_LN2_CSIPHY_LN2_CSI_2PHASE_DATALN", 0x00065200, 0x000000dc },
  { "TITAN_A_PHY_0_CSIPHY_LN3_CSIPHY_LN3_CSI_3PHASE_DATALN", 0x00065300, 0x00000100 },
  { "TITAN_A_PHY_0_CSIPHY_LN4_CSIPHY_LN4_CSI_2PHASE_DATALN", 0x00065400, 0x000000dc },
  { "TITAN_A_PHY_0_CSIPHY_LN5_CSIPHY_LN5_CSI_3PHASE_DATALN", 0x00065500, 0x00000100 },
  { "TITAN_A_PHY_0_CSIPHY_LN6_CSIPHY_LN6_CSI_2PHASE_DATALN", 0x00065600, 0x000000dc },
  { "TITAN_A_PHY_0_CSIPHY_LNCK_CSIPHY_LNCK_CSI_2PHASE_DATALN", 0x00065700, 0x000000dc },
  { "TITAN_A_PHY_0_CSIPHY_CMN_CSIPHY_CMN_CSI_3PHASE_COMMON", 0x00065800, 0x00000100 },
  { "TITAN_A_PHY_1",                               0x00066000, 0x00001000 },
  { "TITAN_A_PHY_1_CSIPHY_LN0_CSIPHY_LN0_CSI_2PHASE_DATALN", 0x00066000, 0x000000dc },
  { "TITAN_A_PHY_1_CSIPHY_LN1_CSIPHY_LN1_CSI_3PHASE_DATALN", 0x00066100, 0x00000100 },
  { "TITAN_A_PHY_1_CSIPHY_LN2_CSIPHY_LN2_CSI_2PHASE_DATALN", 0x00066200, 0x000000dc },
  { "TITAN_A_PHY_1_CSIPHY_LN3_CSIPHY_LN3_CSI_3PHASE_DATALN", 0x00066300, 0x00000100 },
  { "TITAN_A_PHY_1_CSIPHY_LN4_CSIPHY_LN4_CSI_2PHASE_DATALN", 0x00066400, 0x000000dc },
  { "TITAN_A_PHY_1_CSIPHY_LN5_CSIPHY_LN5_CSI_3PHASE_DATALN", 0x00066500, 0x00000100 },
  { "TITAN_A_PHY_1_CSIPHY_LN6_CSIPHY_LN6_CSI_2PHASE_DATALN", 0x00066600, 0x000000dc },
  { "TITAN_A_PHY_1_CSIPHY_LNCK_CSIPHY_LNCK_CSI_2PHASE_DATALN", 0x00066700, 0x000000dc },
  { "TITAN_A_PHY_1_CSIPHY_CMN_CSIPHY_CMN_CSI_3PHASE_COMMON", 0x00066800, 0x00000100 },
  { "TITAN_A_PHY_2",                               0x00067000, 0x00001000 },
  { "TITAN_A_PHY_2_CSIPHY_LN0_CSIPHY_LN0_CSI_2PHASE_DATALN", 0x00067000, 0x000000dc },
  { "TITAN_A_PHY_2_CSIPHY_LN1_CSIPHY_LN1_CSI_3PHASE_DATALN", 0x00067100, 0x00000100 },
  { "TITAN_A_PHY_2_CSIPHY_LN2_CSIPHY_LN2_CSI_2PHASE_DATALN", 0x00067200, 0x000000dc },
  { "TITAN_A_PHY_2_CSIPHY_LN3_CSIPHY_LN3_CSI_3PHASE_DATALN", 0x00067300, 0x00000100 },
  { "TITAN_A_PHY_2_CSIPHY_LN4_CSIPHY_LN4_CSI_2PHASE_DATALN", 0x00067400, 0x000000dc },
  { "TITAN_A_PHY_2_CSIPHY_LN5_CSIPHY_LN5_CSI_3PHASE_DATALN", 0x00067500, 0x00000100 },
  { "TITAN_A_PHY_2_CSIPHY_LN6_CSIPHY_LN6_CSI_2PHASE_DATALN", 0x00067600, 0x000000dc },
  { "TITAN_A_PHY_2_CSIPHY_LNCK_CSIPHY_LNCK_CSI_2PHASE_DATALN", 0x00067700, 0x000000dc },
  { "TITAN_A_PHY_2_CSIPHY_CMN_CSIPHY_CMN_CSI_3PHASE_COMMON", 0x00067800, 0x00000100 },
  { "TITAN_A_PHY_3",                               0x00068000, 0x00001000 },
  { "TITAN_A_PHY_3_CSIPHY_LN0_CSIPHY_LN0_CSI_2PHASE_DATALN", 0x00068000, 0x000000dc },
  { "TITAN_A_PHY_3_CSIPHY_LN2_CSIPHY_LN2_CSI_2PHASE_DATALN", 0x00068200, 0x000000dc },
  { "TITAN_A_PHY_3_CSIPHY_LNCK_CSIPHY_LNCK_CSI_2PHASE_DATALN", 0x00068700, 0x000000dc },
  { "TITAN_A_PHY_3_CSIPHY_CMN_CSIPHY_CMN_CSI_3PHASE_COMMON", 0x00068800, 0x00000100 },
  { "TITAN_A_LRME",                                0x0006b000, 0x00001000 },
  { "TITAN_A_LRME_LRME_CLC_CLC_LRME_0",            0x0006b000, 0x00000200 },
  { "TITAN_A_LRME_LRME_BUS_RD_CLC_BUS_RD_0",       0x0006b200, 0x00000300 },
  { "TITAN_A_LRME_LRME_BUS_WR_CLC_BUS_WR_0",       0x0006b500, 0x00000400 },
  { "TITAN_A_LRME_LRME_TOP_LRME_TOP_REG_0",        0x0006b900, 0x00000100 },
  { "TITAN_A_BPS_0",                               0x0006f000, 0x00008000 },
  { "TITAN_A_BPS_0_CDM",                           0x0006f000, 0x00001000 },
  { "TITAN_A_BPS_0_BPS_TOP",                       0x00070000, 0x00000200 },
  { "TITAN_A_BPS_0_BPS",                           0x00070200, 0x00005400 },
  { "TITAN_A_BPS_0_CLC_PD2R_CLC_PD2R_0",           0x00070200, 0x00000200 },
  { "TITAN_A_BPS_0_CLC_DEMUX_CLC_DEMUX_0",         0x00070400, 0x00000200 },
  { "TITAN_A_BPS_0_CLC_PEDESTAL_CLC_PEDESTAL_0",   0x00070600, 0x00000200 },
  { "TITAN_A_BPS_0_CLC_LINEARIZATION_CLC_LINEARIZATION_0", 0x00070800, 0x00000200 },
  { "TITAN_A_BPS_0_CLC_BPC_PDPC_CLC_BPC_PDPC_0",   0x00070a00, 0x00000200 },
  { "TITAN_A_BPS_0_CLC_HDR_RECON_CLC_HDR_RECON_0", 0x00070c00, 0x00000200 },
  { "TITAN_A_BPS_0_CLC_HDR_MAC_CLC_HDR_MAC_0",     0x00070e00, 0x00000200 },
  { "TITAN_A_BPS_0_CLC_GIC_CLC_GIC_0",             0x00071000, 0x00000200 },
  { "TITAN_A_BPS_0_CLC_ABF_CLC_ABF_0",             0x00071200, 0x00000200 },
  { "TITAN_A_BPS_0_CLC_CROP_RND_CLAMP_BAYER_CLC_CROP_RND_CLAMP_0", 0x00071400, 0x00000200 },
  { "TITAN_A_BPS_0_CLC_LENS_ROLLOFF_CLC_LENS_ROLLOFF_0", 0x00071600, 0x00000200 },
  { "TITAN_A_BPS_0_CLC_DEMOSAIC_CLC_DEMO_0",       0x00071800, 0x00000200 },
  { "TITAN_A_BPS_0_CLC_STATS_BG_CLC_STATS_BG_0",   0x00071a00, 0x00000200 },
  { "TITAN_A_BPS_0_CLC_STATS_HDR_BHIST_CLC_STATS_HDR_BHIST_0", 0x00071c00, 0x00000200 },
  { "TITAN_A_BPS_0_CLC_COLOR_CORRECT_CLC_COLOR_CORRECT_0", 0x00071e00, 0x00000200 },
  { "TITAN_A_BPS_0_CLC_GTM_CLC_GTM_0",             0x00072000, 0x00000200 },
  { "TITAN_A_BPS_0_CLC_GLUT_CLC_GLUT_0",           0x00072200, 0x00000200 },
  { "TITAN_A_BPS_0_CLC_COLOR_XFORM_CLC_COLOR_XFORM_0", 0x00072400, 0x00000200 },
  { "TITAN_A_BPS_0_CLC_CROP_RND_CLAMP_YUV_Y_CLC_CROP_RND_CLAMP_1", 0x00072600, 0x00000200 },
  { "TITAN_A_BPS_0_CLC_CROP_RND_CLAMP_YUV_C_CLC_CROP_RND_CLAMP_2", 0x00072800, 0x00000200 },
  { "TITAN_A_BPS_0_CLC_CHROMA_SUBSAMPLE_CLC_DOWNSCALE_MN_C_0", 0x00072a00, 0x00000200 },
  { "TITAN_A_BPS_0_CLC_HNR_CLC_HNR_0",             0x00072c00, 0x00000200 },
  { "TITAN_A_BPS_0_CLC_CROP_RND_CLAMP_PRE_DOWNSCALE_Y_FULL_DS4_OUT_CLC_CROP_RND_CLAMP_3", 0x00072e00, 0x00000200 },
  { "TITAN_A_BPS_0_CLC_CROP_RND_CLAMP_PRE_DOWNSCALE_C_FULL_DS4_OUT_CLC_CROP_RND_CLAMP_5", 0x00073000, 0x00000200 },
  { "TITAN_A_BPS_0_CLC_DOWNSCALE_4TO1_Y_FULL_DS4_OUT_CLC_DOWNSCALE_4TO1_Y_0", 0x00073200, 0x00000200 },
  { "TITAN_A_BPS_0_CLC_DOWNSCALE_4TO1_C_FULL_DS4_OUT_CLC_DOWNSCALE_4TO1_C_0", 0x00073400, 0x00000200 },
  { "TITAN_A_BPS_0_CLC_CROP_RND_CLAMP_POST_DOWNSCALE_Y_FULL_DS4_OUT_CLC_CROP_RND_CLAMP_4", 0x00073600, 0x00000200 },
  { "TITAN_A_BPS_0_CLC_CROP_RND_CLAMP_POST_DOWNSCALE_C_FULL_DS4_OUT_CLC_CROP_RND_CLAMP_6", 0x00073800, 0x00000200 },
  { "TITAN_A_BPS_0_CLC_R2PD_FULL_DS4_OUT_CLC_R2PD_0", 0x00073a00, 0x00000200 },
  { "TITAN_A_BPS_0_CLC_CROP_RND_CLAMP_PRE_DOWNSCALE_Y_DS16_OUT_CLC_CROP_RND_CLAMP_7", 0x00073c00, 0x00000200 },
  { "TITAN_A_BPS_0_CLC_CROP_RND_CLAMP_PRE_DOWNSCALE_C_DS16_OUT_CLC_CROP_RND_CLAMP_9", 0x00073e00, 0x00000200 },
  { "TITAN_A_BPS_0_CLC_DOWNSCALE_4TO1_Y_DS16_OUT_CLC_DOWNSCALE_4TO1_Y_1", 0x00074000, 0x00000200 },
  { "TITAN_A_BPS_0_CLC_DOWNSCALE_4TO1_C_DS16_OUT_CLC_DOWNSCALE_4TO1_C_1", 0x00074200, 0x00000200 },
  { "TITAN_A_BPS_0_CLC_CROP_RND_CLAMP_POST_DOWNSCALE_Y_DS16_OUT_CLC_CROP_RND_CLAMP_8", 0x00074400, 0x00000200 },
  { "TITAN_A_BPS_0_CLC_CROP_RND_CLAMP_POST_DOWNSCALE_C_DS16_OUT_CLC_CROP_RND_CLAMP_10", 0x00074600, 0x00000200 },
  { "TITAN_A_BPS_0_CLC_R2PD_DS16_OUT_CLC_R2PD_1",  0x00074800, 0x00000200 },
  { "TITAN_A_BPS_0_CLC_CROP_RND_CLAMP_PRE_DOWNSCALE_Y_REG_OUT_CLC_CROP_RND_CLAMP_11", 0x00074a00, 0x00000200 },
  { "TITAN_A_BPS_0_CLC_CROP_RND_CLAMP_PRE_DOWNSCALE_C_REG_OUT_CLC_CROP_RND_CLAMP_13", 0x00074c00, 0x00000200 },
  { "TITAN_A_BPS_0_CLC_DOWNSCALE_MN_Y_REG_OUT_CLC_DOWNSCALE_MN_Y_0", 0x00074e00, 0x00000200 },
  { "TITAN_A_BPS_0_CLC_DOWNSCALE_MN_C_REG_OUT_CLC_DOWNSCALE_MN_C_1", 0x00075000, 0x00000200 },
  { "TITAN_A_BPS_0_CLC_CROP_RND_CLAMP_POST_DOWNSCALE_Y_REG_OUT_CLC_CROP_RND_CLAMP_12", 0x00075200, 0x00000200 },
  { "TITAN_A_BPS_0_CLC_CROP_RND_CLAMP_POST_DOWNSCALE_C_REG_OUT_CLC_CROP_RND_CLAMP_14", 0x00075400, 0x00000200 },
  { "TITAN_A_BPS_0_BUS_RD_CLC_BUS_RD",             0x00075600, 0x00000200 },
  { "TITAN_A_BPS_0_BUS_WR_CLC_BUS_WR",             0x00075800, 0x00000900 },
  { "TITAN_A_BPS_0_QOS_CLC_QOS",                   0x00076100, 0x00000200 },
  { "TITAN_A_IPE_0",                               0x00087000, 0x0000a000 },
  { "TITAN_A_IPE_0_CDM",                           0x00087000, 0x00001000 },
  { "TITAN_A_IPE_0_TOP_IPE_TOP",                   0x00088000, 0x00000200 },
  { "TITAN_A_IPE_0_NPS",                           0x00088200, 0x00004000 },
  { "TITAN_A_IPE_0_NPS_CLC_ICA_0",                 0x00088200, 0x00000400 },
  { "TITAN_A_IPE_0_NPS_CLC_ICA_1",                 0x00088600, 0x00000400 },
  { "TITAN_A_IPE_0_NPS_CLC_R2B_0",                 0x00088a00, 0x00000200 },
  { "TITAN_A_IPE_0_NPS_CLC_ANR_CLC_ANR_0",         0x00088c00, 0x00001000 },
  { "TITAN_A_IPE_0_NPS_CLC_R2B_1",                 0x00089c00, 0x00000200 },
  { "TITAN_A_IPE_0_NPS_CLC_TF_CLC_TF_0",           0x00089e00, 0x00000200 },
  { "TITAN_A_IPE_0_NPS_CLC_B2R_CLC_B2R_0",         0x0008a000, 0x00000200 },
  { "TITAN_A_IPE_0_NPS_CLC_CROP_RND_CLAMP_Y_PRE_DOWNSCALE_CLC_CROP_RND_CLAMP_2", 0x0008a200, 0x00000200 },
  { "TITAN_A_IPE_0_NPS_CLC_CROP_RND_CLAMP_C_PRE_DOWNSCALE_CLC_CROP_RND_CLAMP_4", 0x0008a400, 0x00000200 },
  { "TITAN_A_IPE_0_NPS_CLC_DOWNSCALE_4TO1_Y_CLC_DOWNSCALE_4TO1_Y_0", 0x0008a600, 0x00000200 },
  { "TITAN_A_IPE_0_NPS_CLC_DOWNSCALE_4TO1_C_CLC_DOWNSCALE_4TO1_C_0", 0x0008a800, 0x00000200 },
  { "TITAN_A_IPE_0_NPS_CLC_CROP_RND_CLAMP_Y_POST_DOWNSCALE_CLC_CROP_RND_CLAMP_3", 0x0008aa00, 0x00000200 },
  { "TITAN_A_IPE_0_NPS_CLC_CROP_RND_CLAMP_C_POST_DOWNSCALE_CLC_CROP_RND_CLAMP_5", 0x0008ac00, 0x00000200 },
  { "TITAN_A_IPE_0_NPS_CLC_R2PD_CLC_R2PD_0",       0x0008ae00, 0x00000200 },
  { "TITAN_A_IPE_0_NPS_CLC_CROP_RND_CLAMP_TF_Y_CLC_CROP_RND_CLAMP_0", 0x0008b000, 0x00000200 },
  { "TITAN_A_IPE_0_NPS_CLC_CROP_RND_CLAMP_TF_C_CLC_CROP_RND_CLAMP_1", 0x0008b200, 0x00000200 },
  { "TITAN_A_IPE_0_PPS",                           0x0008c200, 0x00003800 },
  { "TITAN_A_IPE_0_PPS_CLC_CAC_CLC_CAC_0",         0x0008c200, 0x00000200 },
  { "TITAN_A_IPE_0_PPS_CLC_CROP_RND_CLAMP_Y_PRE_CHROMA_UPSAMPLE_CLC_CROP_RND_CLAMP_0", 0x0008c400, 0x00000200 },
  { "TITAN_A_IPE_0_PPS_CLC_CROP_RND_CLAMP_C_PRE_CHROMA_UPSAMPLE_CLC_CROP_RND_CLAMP_1", 0x0008c600, 0x00000200 },
  { "TITAN_A_IPE_0_PPS_CLC_CHROMA_UP_CLC_CHROMA_UP_0", 0x0008c800, 0x00000200 },
  { "TITAN_A_IPE_0_PPS_CLC_COLOR_XFORM_CLC_COLOR_XFORM_0", 0x0008ca00, 0x00000200 },
  { "TITAN_A_IPE_0_PPS_CLC_LTM_CLC_LTM_0",         0x0008cc00, 0x00000200 },
  { "TITAN_A_IPE_0_PPS_CLC_COLOR_CORRECT_CLC_COLOR_CORRECT_0", 0x0008ce00, 0x00000200 },
  { "TITAN_A_IPE_0_PPS_CLC_GLUT_CLC_GLUT_0",       0x0008d000, 0x00000200 },
  { "TITAN_A_IPE_0_PPS_CLC_2D_LUT_CLC_LUT2D_0",    0x0008d200, 0x00000200 },
  { "TITAN_A_IPE_0_PPS_CLC_CHROMA_ENHAN_CLC_CHROMA_ENHAN_0", 0x0008d400, 0x00000200 },
  { "TITAN_A_IPE_0_PPS_CLC_CHROMA_SUP_CLC_CHROMA_SUP_0", 0x0008d600, 0x00000200 },
  { "TITAN_A_IPE_0_PPS_CLC_SKIN_ENHAN_CLC_SKIN_ENHAN_0", 0x0008d800, 0x00000200 },
  { "TITAN_A_IPE_0_PPS_CLC_ASF_CLC_ASF_0",         0x0008da00, 0x00000200 },
  { "TITAN_A_IPE_0_PPS_CLC_CROP_RND_CLAMP_YUV_PRE_UPSCALE_CLC_CROP_RND_CLAMP_2", 0x0008dc00, 0x00000200 },
  { "TITAN_A_IPE_0_PPS_CLC_UPSCALE_CLC_UPSCALE_0", 0x0008de00, 0x00000200 },
  { "TITAN_A_IPE_0_PPS_CLC_GRA_CLC_GRA_0",         0x0008e000, 0x00000200 },
  { "TITAN_A_IPE_0_PPS_CLC_CROP_RND_CLAMP_Y_PRE_DOWNSCALE_MN_VIDEO_CLC_CROP_RND_CLAMP_3", 0x0008e200, 0x00000200 },
  { "TITAN_A_IPE_0_PPS_CLC_CROP_RND_CLAMP_C_PRE_DOWNSCALE_MN_VIDEO_CLC_CROP_RND_CLAMP_5", 0x0008e400, 0x00000200 },
  { "TITAN_A_IPE_0_PPS_CLC_DOWNSCALE_MN_Y_VIDEO_CLC_DOWNSCALE_MN_Y_0", 0x0008e600, 0x00000200 },
  { "TITAN_A_IPE_0_PPS_CLC_DOWNSCALE_MN_C_VIDEO_CLC_DOWNSCALE_MN_C_0", 0x0008e800, 0x00000200 },
  { "TITAN_A_IPE_0_PPS_CLC_CROP_RND_CLAMP_Y_POST_DOWNSCALE_MN_VIDEO_CLC_CROP_RND_CLAMP_4", 0x0008ea00, 0x00000200 },
  { "TITAN_A_IPE_0_PPS_CLC_CROP_RND_CLAMP_C_POST_DOWNSCALE_MN_VIDEO_CLC_CROP_RND_CLAMP_6", 0x0008ec00, 0x00000200 },
  { "TITAN_A_IPE_0_PPS_CLC_CROP_RND_CLAMP_Y_PRE_DOWNSCALE_MN_DISP_CLC_CROP_RND_CLAMP_7", 0x0008ee00, 0x00000200 },
  { "TITAN_A_IPE_0_PPS_CLC_CROP_RND_CLAMP_C_PRE_DOWNSCALE_MN_DISP_CLC_CROP_RND_CLAMP_9", 0x0008f000, 0x00000200 },
  { "TITAN_A_IPE_0_PPS_CLC_DOWNSCALE_MN_Y_DISP_CLC_DOWNSCALE_MN_Y_1", 0x0008f200, 0x00000200 },
  { "TITAN_A_IPE_0_PPS_CLC_DOWNSCALE_MN_C_DISP_CLC_DOWNSCALE_MN_C_1", 0x0008f400, 0x00000200 },
  { "TITAN_A_IPE_0_PPS_CLC_CROP_RND_CLAMP_Y_POST_DOWNSCALE_MN_DISP_CLC_CROP_RND_CLAMP_8", 0x0008f600, 0x00000200 },
  { "TITAN_A_IPE_0_PPS_CLC_CROP_RND_CLAMP_C_POST_DOWNSCALE_MN_DISP_CLC_CROP_RND_CLAMP_10", 0x0008f800, 0x00000200 },
  { "TITAN_A_IPE_0_BUS_RD_CLC_BUS_RD",             0x0008fa00, 0x00000600 },
  { "TITAN_A_IPE_0_BUS_WR_CLC_BUS_WR",             0x00090000, 0x00000c00 },
  { "TITAN_A_IPE_0_QOS_CLC_QOS",                   0x00090c00, 0x00000200 },
  { "TITAN_A_IPE_1",                               0x00091000, 0x0000a000 },
  { "TITAN_A_IPE_1_CDM",                           0x00091000, 0x00001000 },
  { "TITAN_A_IPE_1_TOP_IPE_TOP",                   0x00092000, 0x00000200 },
  { "TITAN_A_IPE_1_NPS",                           0x00092200, 0x00004000 },
  { "TITAN_A_IPE_1_NPS_CLC_ICA_0",                 0x00092200, 0x00000400 },
  { "TITAN_A_IPE_1_NPS_CLC_ICA_1",                 0x00092600, 0x00000400 },
  { "TITAN_A_IPE_1_NPS_CLC_R2B_0",                 0x00092a00, 0x00000200 },
  { "TITAN_A_IPE_1_NPS_CLC_ANR_CLC_ANR_0",         0x00092c00, 0x00001000 },
  { "TITAN_A_IPE_1_NPS_CLC_R2B_1",                 0x00093c00, 0x00000200 },
  { "TITAN_A_IPE_1_NPS_CLC_TF_CLC_TF_0",           0x00093e00, 0x00000200 },
  { "TITAN_A_IPE_1_NPS_CLC_B2R_CLC_B2R_0",         0x00094000, 0x00000200 },
  { "TITAN_A_IPE_1_NPS_CLC_CROP_RND_CLAMP_Y_PRE_DOWNSCALE_CLC_CROP_RND_CLAMP_2", 0x00094200, 0x00000200 },
  { "TITAN_A_IPE_1_NPS_CLC_CROP_RND_CLAMP_C_PRE_DOWNSCALE_CLC_CROP_RND_CLAMP_4", 0x00094400, 0x00000200 },
  { "TITAN_A_IPE_1_NPS_CLC_DOWNSCALE_4TO1_Y_CLC_DOWNSCALE_4TO1_Y_0", 0x00094600, 0x00000200 },
  { "TITAN_A_IPE_1_NPS_CLC_DOWNSCALE_4TO1_C_CLC_DOWNSCALE_4TO1_C_0", 0x00094800, 0x00000200 },
  { "TITAN_A_IPE_1_NPS_CLC_CROP_RND_CLAMP_Y_POST_DOWNSCALE_CLC_CROP_RND_CLAMP_3", 0x00094a00, 0x00000200 },
  { "TITAN_A_IPE_1_NPS_CLC_CROP_RND_CLAMP_C_POST_DOWNSCALE_CLC_CROP_RND_CLAMP_5", 0x00094c00, 0x00000200 },
  { "TITAN_A_IPE_1_NPS_CLC_R2PD_CLC_R2PD_0",       0x00094e00, 0x00000200 },
  { "TITAN_A_IPE_1_NPS_CLC_CROP_RND_CLAMP_TF_Y_CLC_CROP_RND_CLAMP_0", 0x00095000, 0x00000200 },
  { "TITAN_A_IPE_1_NPS_CLC_CROP_RND_CLAMP_TF_C_CLC_CROP_RND_CLAMP_1", 0x00095200, 0x00000200 },
  { "TITAN_A_IPE_1_PPS",                           0x00096200, 0x00003800 },
  { "TITAN_A_IPE_1_PPS_CLC_CAC_CLC_CAC_0",         0x00096200, 0x00000200 },
  { "TITAN_A_IPE_1_PPS_CLC_CROP_RND_CLAMP_Y_PRE_CHROMA_UPSAMPLE_CLC_CROP_RND_CLAMP_0", 0x00096400, 0x00000200 },
  { "TITAN_A_IPE_1_PPS_CLC_CROP_RND_CLAMP_C_PRE_CHROMA_UPSAMPLE_CLC_CROP_RND_CLAMP_1", 0x00096600, 0x00000200 },
  { "TITAN_A_IPE_1_PPS_CLC_CHROMA_UP_CLC_CHROMA_UP_0", 0x00096800, 0x00000200 },
  { "TITAN_A_IPE_1_PPS_CLC_COLOR_XFORM_CLC_COLOR_XFORM_0", 0x00096a00, 0x00000200 },
  { "TITAN_A_IPE_1_PPS_CLC_LTM_CLC_LTM_0",         0x00096c00, 0x00000200 },
  { "TITAN_A_IPE_1_PPS_CLC_COLOR_CORRECT_CLC_COLOR_CORRECT_0", 0x00096e00, 0x00000200 },
  { "TITAN_A_IPE_1_PPS_CLC_GLUT_CLC_GLUT_0",       0x00097000, 0x00000200 },
  { "TITAN_A_IPE_1_PPS_CLC_2D_LUT_CLC_LUT2D_0",    0x00097200, 0x00000200 },
  { "TITAN_A_IPE_1_PPS_CLC_CHROMA_ENHAN_CLC_CHROMA_ENHAN_0", 0x00097400, 0x00000200 },
  { "TITAN_A_IPE_1_PPS_CLC_CHROMA_SUP_CLC_CHROMA_SUP_0", 0x00097600, 0x00000200 },
  { "TITAN_A_IPE_1_PPS_CLC_SKIN_ENHAN_CLC_SKIN_ENHAN_0", 0x00097800, 0x00000200 },
  { "TITAN_A_IPE_1_PPS_CLC_ASF_CLC_ASF_0",         0x00097a00, 0x00000200 },
  { "TITAN_A_IPE_1_PPS_CLC_CROP_RND_CLAMP_YUV_PRE_UPSCALE_CLC_CROP_RND_CLAMP_2", 0x00097c00, 0x00000200 },
  { "TITAN_A_IPE_1_PPS_CLC_UPSCALE_CLC_UPSCALE_0", 0x00097e00, 0x00000200 },
  { "TITAN_A_IPE_1_PPS_CLC_GRA_CLC_GRA_0",         0x00098000, 0x00000200 },
  { "TITAN_A_IPE_1_PPS_CLC_CROP_RND_CLAMP_Y_PRE_DOWNSCALE_MN_VIDEO_CLC_CROP_RND_CLAMP_3", 0x00098200, 0x00000200 },
  { "TITAN_A_IPE_1_PPS_CLC_CROP_RND_CLAMP_C_PRE_DOWNSCALE_MN_VIDEO_CLC_CROP_RND_CLAMP_5", 0x00098400, 0x00000200 },
  { "TITAN_A_IPE_1_PPS_CLC_DOWNSCALE_MN_Y_VIDEO_CLC_DOWNSCALE_MN_Y_0", 0x00098600, 0x00000200 },
  { "TITAN_A_IPE_1_PPS_CLC_DOWNSCALE_MN_C_VIDEO_CLC_DOWNSCALE_MN_C_0", 0x00098800, 0x00000200 },
  { "TITAN_A_IPE_1_PPS_CLC_CROP_RND_CLAMP_Y_POST_DOWNSCALE_MN_VIDEO_CLC_CROP_RND_CLAMP_4", 0x00098a00, 0x00000200 },
  { "TITAN_A_IPE_1_PPS_CLC_CROP_RND_CLAMP_C_POST_DOWNSCALE_MN_VIDEO_CLC_CROP_RND_CLAMP_6", 0x00098c00, 0x00000200 },
  { "TITAN_A_IPE_1_PPS_CLC_CROP_RND_CLAMP_Y_PRE_DOWNSCALE_MN_DISP_CLC_CROP_RND_CLAMP_7", 0x00098e00, 0x00000200 },
  { "TITAN_A_IPE_1_PPS_CLC_CROP_RND_CLAMP_C_PRE_DOWNSCALE_MN_DISP_CLC_CROP_RND_CLAMP_9", 0x00099000, 0x00000200 },
  { "TITAN_A_IPE_1_PPS_CLC_DOWNSCALE_MN_Y_DISP_CLC_DOWNSCALE_MN_Y_1", 0x00099200, 0x00000200 },
  { "TITAN_A_IPE_1_PPS_CLC_DOWNSCALE_MN_C_DISP_CLC_DOWNSCALE_MN_C_1", 0x00099400, 0x00000200 },
  { "TITAN_A_IPE_1_PPS_CLC_CROP_RND_CLAMP_Y_POST_DOWNSCALE_MN_DISP_CLC_CROP_RND_CLAMP_8", 0x00099600, 0x00000200 },
  { "TITAN_A_IPE_1_PPS_CLC_CROP_RND_CLAMP_C_POST_DOWNSCALE_MN_DISP_CLC_CROP_RND_CLAMP_10", 0x00099800, 0x00000200 },
  { "TITAN_A_IPE_1_BUS_RD_CLC_BUS_RD",             0x00099a00, 0x00000600 },
  { "TITAN_A_IPE_1_BUS_WR_CLC_BUS_WR",             0x0009a000, 0x00000c00 },
  { "TITAN_A_IPE_1_QOS_CLC_QOS",                   0x0009ac00, 0x00000200 },
  { "TITAN_A_IFE_0",                               0x000af000, 0x00005000 },
  { "TITAN_A_IFE_0_VFE_IFE",                       0x000af000, 0x00001000 },
  { "TITAN_A_IFE_0_BUS_WR_IFE_BUS_WR",             0x000b1000, 0x00001600 },
  { "TITAN_A_IFE_0_CSID",                          0x000b3000, 0x00001000 },
  { "TITAN_A_IFE_1",                               0x000b6000, 0x00005000 },
  { "TITAN_A_IFE_1_VFE_IFE",                       0x000b6000, 0x00001000 },
  { "TITAN_A_IFE_1_BUS_WR_IFE_BUS_WR",             0x000b8000, 0x00001600 },
  { "TITAN_A_IFE_1_CSID",                          0x000ba000, 0x00001000 },
  { "TITAN_A_IFE_LITE",                            0x000c4000, 0x00005000 },
  { "TITAN_A_IFE_LITE_VFE_IFE",                    0x000c4000, 0x00001000 },
  { "TITAN_A_IFE_LITE_BUS_WR_IFE_BUS_WR",          0x000c6000, 0x00000600 },
  { "TITAN_A_IFE_LITE_CSID",                       0x000c8000, 0x00001000 },
  { "CAM_CC",                                      0x00100000, 0x00020000 },
  { "CAM_CC_CAM_CC_REG",                           0x00100000, 0x00010000 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_MDSS[] =
{
  { "MDSS_HW",                                     0x00000000, 0x00001000 },
  { "MDP",                                         0x00001000, 0x0008f000 },
  { "MDP_SSPP_TOP0",                               0x00001000, 0x00000030 },
  { "MDP_SMP_TOP0",                                0x00001030, 0x000001b8 },
  { "MDP_SSPP_TOP1",                               0x000011e8, 0x00000114 },
  { "MDP_SMP_TOP1",                                0x000012fc, 0x00000004 },
  { "MDP_DSPP_TOP0",                               0x00001300, 0x00000080 },
  { "MDP_PERIPH_TOP0",                             0x00001380, 0x00000028 },
  { "MDP_SSPP_TOP2",                               0x000013a8, 0x00000048 },
  { "MDP_PERIPH_TOP1",                             0x000013f0, 0x00000c10 },
  { "MDP_CTL_0",                                   0x00002000, 0x00000100 },
  { "MDP_CTL_1",                                   0x00002200, 0x00000100 },
  { "MDP_CTL_2",                                   0x00002400, 0x00000100 },
  { "MDP_CTL_3",                                   0x00002600, 0x00000100 },
  { "MDP_CTL_4",                                   0x00002800, 0x00000100 },
  { "MDP_VP_0_VIG_0_SSPP",                         0x00005000, 0x00000200 },
  { "MDP_VP_0_VIG_0",                              0x00005200, 0x00001d80 },
  { "MDP_VP_0_VIG_0_QSEED3",                       0x00005a00, 0x00000800 },
  { "MDP_VP_0_VIG_0_QSEED3_COEF_LUT",              0x00005b00, 0x00000700 },
  { "MDP_VP_0_VIG_0_PA_HIST",                      0x00006200, 0x00000800 },
  { "MDP_VP_0_VIG_0_PA_MEMCOL",                    0x00006280, 0x00000080 },
  { "MDP_VP_0_VIG_0_PA_SZONE",                     0x00006300, 0x00000020 },
  { "MDP_VP_0_VIG_0_LUTV_RAM",                     0x00006400, 0x00000200 },
  { "MDP_VP_0_VIG_0_HIST_RAM",                     0x00006600, 0x00000400 },
  { "MDP_VP_0_VIG_0_CSC_1",                        0x00006a00, 0x00000100 },
  { "MDP_VP_0_VIG_0_GAMUT_10B",                    0x00006d00, 0x00000200 },
  { "MDP_VP_0_VIG_1_SSPP",                         0x00007000, 0x00000200 },
  { "MDP_VP_0_VIG_1",                              0x00007200, 0x00001d80 },
  { "MDP_VP_0_VIG_1_QSEED3",                       0x00007a00, 0x00000800 },
  { "MDP_VP_0_VIG_1_QSEED3_COEF_LUT",              0x00007b00, 0x00000700 },
  { "MDP_VP_0_VIG_1_PA_HIST",                      0x00008200, 0x00000800 },
  { "MDP_VP_0_VIG_1_PA_MEMCOL",                    0x00008280, 0x00000080 },
  { "MDP_VP_0_VIG_1_PA_SZONE",                     0x00008300, 0x00000020 },
  { "MDP_VP_0_VIG_1_LUTV_RAM",                     0x00008400, 0x00000200 },
  { "MDP_VP_0_VIG_1_HIST_RAM",                     0x00008600, 0x00000400 },
  { "MDP_VP_0_VIG_1_CSC_1",                        0x00008a00, 0x00000100 },
  { "MDP_VP_0_VIG_1_GAMUT_10B",                    0x00008d00, 0x00000200 },
  { "MDP_VP_0_VIG_2_SSPP",                         0x00009000, 0x00000200 },
  { "MDP_VP_0_VIG_2",                              0x00009200, 0x00001d80 },
  { "MDP_VP_0_VIG_2_QSEED3",                       0x00009a00, 0x00000800 },
  { "MDP_VP_0_VIG_2_QSEED3_COEF_LUT",              0x00009b00, 0x00000700 },
  { "MDP_VP_0_VIG_2_PA_HIST",                      0x0000a200, 0x00000800 },
  { "MDP_VP_0_VIG_2_PA_MEMCOL",                    0x0000a280, 0x00000080 },
  { "MDP_VP_0_VIG_2_PA_SZONE",                     0x0000a300, 0x00000020 },
  { "MDP_VP_0_VIG_2_LUTV_RAM",                     0x0000a400, 0x00000200 },
  { "MDP_VP_0_VIG_2_HIST_RAM",                     0x0000a600, 0x00000400 },
  { "MDP_VP_0_VIG_2_CSC_1",                        0x0000aa00, 0x00000100 },
  { "MDP_VP_0_VIG_2_GAMUT_10B",                    0x0000ad00, 0x00000200 },
  { "MDP_VP_0_VIG_3_SSPP",                         0x0000b000, 0x00000200 },
  { "MDP_VP_0_VIG_3",                              0x0000b200, 0x00001d80 },
  { "MDP_VP_0_VIG_3_QSEED3",                       0x0000ba00, 0x00000800 },
  { "MDP_VP_0_VIG_3_QSEED3_COEF_LUT",              0x0000bb00, 0x00000700 },
  { "MDP_VP_0_VIG_3_PA_HIST",                      0x0000c200, 0x00000800 },
  { "MDP_VP_0_VIG_3_PA_MEMCOL",                    0x0000c280, 0x00000080 },
  { "MDP_VP_0_VIG_3_PA_SZONE",                     0x0000c300, 0x00000020 },
  { "MDP_VP_0_VIG_3_LUTV_RAM",                     0x0000c400, 0x00000200 },
  { "MDP_VP_0_VIG_3_HIST_RAM",                     0x0000c600, 0x00000400 },
  { "MDP_VP_0_VIG_3_CSC_1",                        0x0000ca00, 0x00000100 },
  { "MDP_VP_0_VIG_3_GAMUT_10B",                    0x0000cd00, 0x00000200 },
  { "MDP_VP_0_RGB_0_SSPP",                         0x00015000, 0x00000200 },
  { "MDP_VP_0_RGB_0",                              0x00015200, 0x00000400 },
  { "MDP_VP_0_RGB_1_SSPP",                         0x00017000, 0x00000200 },
  { "MDP_VP_0_RGB_1",                              0x00017200, 0x00000400 },
  { "MDP_VP_0_RGB_2_SSPP",                         0x00019000, 0x00000200 },
  { "MDP_VP_0_RGB_2",                              0x00019200, 0x00000400 },
  { "MDP_VP_0_RGB_3_SSPP",                         0x0001b000, 0x00000200 },
  { "MDP_VP_0_RGB_3",                              0x0001b200, 0x00000400 },
  { "MDP_VP_0_DMA_0_SSPP",                         0x00025000, 0x00000200 },
  { "MDP_VP_0_DMA_0",                              0x00025200, 0x00000200 },
  { "MDP_VP_0_DMA_1_SSPP",                         0x00027000, 0x00000200 },
  { "MDP_VP_0_DMA_1",                              0x00027200, 0x00000200 },
  { "MDP_VP_0_DMA_2_SSPP",                         0x00029000, 0x00000200 },
  { "MDP_VP_0_DMA_2",                              0x00029200, 0x00000200 },
  { "MDP_VP_0_DMA_3_SSPP",                         0x0002b000, 0x00000200 },
  { "MDP_VP_0_DMA_3",                              0x0002b200, 0x00000200 },
  { "MDP_VP_0_CURSOR_0",                           0x00035000, 0x00000400 },
  { "MDP_VP_0_CURSOR_0_SSPP",                      0x00035000, 0x00000200 },
  { "MDP_VP_0_CURSOR_1",                           0x00037000, 0x00000400 },
  { "MDP_VP_0_CURSOR_1_SSPP",                      0x00037000, 0x00000200 },
  { "MDP_VP_0_LAYER_0",                            0x00045000, 0x00000400 },
  { "MDP_VP_0_LAYER_1",                            0x00046000, 0x00000400 },
  { "MDP_VP_0_LAYER_2",                            0x00047000, 0x00000400 },
  { "MDP_VP_0_LAYER_3",                            0x00048000, 0x00000400 },
  { "MDP_VP_0_LAYER_4",                            0x00049000, 0x00000400 },
  { "MDP_VP_0_LAYER_5",                            0x0004a000, 0x00000400 },
  { "MDP_VP_0_DSPP_0",                             0x00055000, 0x00001800 },
  { "MDP_VP_0_DSPP_0_GAMUT",                       0x00056000, 0x00000200 },
  { "MDP_VP_0_DSPP_0_PA_HIST",                     0x00055800, 0x00000800 },
  { "MDP_VP_0_DSPP_0_PA_MEMCOL",                   0x00055880, 0x00000080 },
  { "MDP_VP_0_DSPP_0_PA_SZONE",                    0x00055900, 0x00000020 },
  { "MDP_VP_0_DSPP_0_LUTV_RAM",                    0x00055a00, 0x00000200 },
  { "MDP_VP_0_DSPP_0_HIST_RAM",                    0x00055c00, 0x00000400 },
  { "MDP_VP_0_DSPP_0_PCC",                         0x00056700, 0x00000090 },
  { "MDP_VP_0_DSPP_0_PGC",                         0x000567c0, 0x00000040 },
  { "MDP_VP_0_DSPP_1",                             0x00057000, 0x00001800 },
  { "MDP_VP_0_DSPP_1_GAMUT",                       0x00058000, 0x00000200 },
  { "MDP_VP_0_DSPP_1_PA_HIST",                     0x00057800, 0x00000800 },
  { "MDP_VP_0_DSPP_1_PA_MEMCOL",                   0x00057880, 0x00000080 },
  { "MDP_VP_0_DSPP_1_PA_SZONE",                    0x00057900, 0x00000020 },
  { "MDP_VP_0_DSPP_1_LUTV_RAM",                    0x00057a00, 0x00000200 },
  { "MDP_VP_0_DSPP_1_HIST_RAM",                    0x00057c00, 0x00000400 },
  { "MDP_VP_0_DSPP_1_PCC",                         0x00058700, 0x00000090 },
  { "MDP_VP_0_DSPP_1_PGC",                         0x000587c0, 0x00000040 },
  { "MDP_VP_0_DSPP_2",                             0x00059000, 0x00001800 },
  { "MDP_VP_0_DSPP_2_GAMUT",                       0x0005a000, 0x00000200 },
  { "MDP_VP_0_DSPP_2_PA_HIST",                     0x00059800, 0x00000800 },
  { "MDP_VP_0_DSPP_2_PA_MEMCOL",                   0x00059880, 0x00000080 },
  { "MDP_VP_0_DSPP_2_PA_SZONE",                    0x00059900, 0x00000020 },
  { "MDP_VP_0_DSPP_2_LUTV_RAM",                    0x00059a00, 0x00000200 },
  { "MDP_VP_0_DSPP_2_HIST_RAM",                    0x00059c00, 0x00000400 },
  { "MDP_VP_0_DSPP_2_PCC",                         0x0005a700, 0x00000090 },
  { "MDP_VP_0_DSPP_2_PGC",                         0x0005a7c0, 0x00000040 },
  { "MDP_VP_0_DSPP_3",                             0x0005b000, 0x00001800 },
  { "MDP_VP_0_DSPP_3_GAMUT",                       0x0005c000, 0x00000200 },
  { "MDP_VP_0_DSPP_3_PA_HIST",                     0x0005b800, 0x00000800 },
  { "MDP_VP_0_DSPP_3_PA_MEMCOL",                   0x0005b880, 0x00000080 },
  { "MDP_VP_0_DSPP_3_PA_SZONE",                    0x0005b900, 0x00000020 },
  { "MDP_VP_0_DSPP_3_LUTV_RAM",                    0x0005ba00, 0x00000200 },
  { "MDP_VP_0_DSPP_3_HIST_RAM",                    0x0005bc00, 0x00000400 },
  { "MDP_VP_0_DSPP_3_PCC",                         0x0005c700, 0x00000090 },
  { "MDP_VP_0_DSPP_3_PGC",                         0x0005c7c0, 0x00000040 },
  { "MDP_DEST_SCALER_0",                           0x00061000, 0x00003000 },
  { "MDP_DEST_SCALER_0_DEST_SCL0",                 0x00061800, 0x00000800 },
  { "MDP_DEST_SCALER_0_DEST_SCL0_COEF_LUT",        0x00061900, 0x00000700 },
  { "MDP_DEST_SCALER_0_DEST_SCL1",                 0x00062000, 0x00000800 },
  { "MDP_DEST_SCALER_0_DEST_SCL1_COEF_LUT",        0x00062100, 0x00000700 },
  { "MDP_WB_0",                                    0x00065000, 0x00000400 },
  { "MDP_WB_1",                                    0x00065800, 0x00000400 },
  { "MDP_WB_2",                                    0x00066000, 0x00000400 },
  { "MDP_INTF_0",                                  0x0006b000, 0x00000300 },
  { "MDP_INTF_1",                                  0x0006b800, 0x00000300 },
  { "MDP_INTF_2",                                  0x0006c000, 0x00000300 },
  { "MDP_INTF_3",                                  0x0006c800, 0x00000300 },
  { "MDP_INTF_4",                                  0x0006d000, 0x00000300 },
  { "MDP_PP_0",                                    0x00071000, 0x00000100 },
  { "MDP_PP_1",                                    0x00071800, 0x00000100 },
  { "MDP_PP_2",                                    0x00072000, 0x00000100 },
  { "MDP_PP_3",                                    0x00072800, 0x00000100 },
  { "MDP_PP_4",                                    0x00073000, 0x00000100 },
  { "MDP_PPB_0",                                   0x00074000, 0x00000100 },
  { "MDP_PPB_0_DITHER",                            0x000740e0, 0x00000020 },
  { "MDP_PPB_1",                                   0x00074800, 0x00000100 },
  { "MDP_PPB_1_DITHER",                            0x000748e0, 0x00000020 },
  { "MDP_PPB_2",                                   0x00075000, 0x00000100 },
  { "MDP_PPB_2_DITHER",                            0x000750e0, 0x00000020 },
  { "MDP_PPB_3",                                   0x00075800, 0x00000100 },
  { "MDP_PPB_3_DITHER",                            0x000758e0, 0x00000020 },
  { "MDP_ENCR_NS_0",                               0x00077000, 0x00000010 },
  { "MDP_ENCR_S_0",                                0x00077400, 0x00000030 },
  { "MDP_CDM_0",                                   0x0007a200, 0x00000240 },
  { "MDP_CDM_0_CSC_10",                            0x0007a200, 0x00000100 },
  { "MDP_CDM_0_CDWN2",                             0x0007a300, 0x00000100 },
  { "MDP_CDM_0_HDMI_PACK",                         0x0007a400, 0x00000010 },
  { "MDP_AD4_0",                                   0x0007d000, 0x00000800 },
  { "MDP_AD4_1",                                   0x0007e000, 0x00000800 },
  { "MDP_DSC_0",                                   0x00081000, 0x00000200 },
  { "MDP_DSC_1",                                   0x00081400, 0x00000200 },
  { "MDP_DSC_2",                                   0x00081800, 0x00000200 },
  { "MDP_DSC_3",                                   0x00081c00, 0x00000200 },
  { "MDP_SCM3_0",                                  0x00081600, 0x00000020 },
  { "MDP_QDSS",                                    0x00081a00, 0x00000200 },
  { "DP",                                          0x00090000, 0x00001600 },
  { "DP_AHBCLK",                                   0x00090000, 0x00000200 },
  { "DP_AUXCLK",                                   0x00090200, 0x00000200 },
  { "DP_LCLK",                                     0x00090400, 0x00000600 },
  { "DP_P0CLK",                                    0x00090a00, 0x00000600 },
  { "DP_P1CLK",                                    0x00091000, 0x00000600 },
  { "DSI_0",                                       0x00094000, 0x00001000 },
  { "DSI_0_DSI",                                   0x00094000, 0x00000400 },
  { "DSI_0_PHY",                                   0x00094400, 0x00000800 },
  { "DSI_0_PHY_DSIPHY_CMN_DSIPHY_CMN_DSI_COMMON",  0x00094400, 0x0000012c },
  { "DSI_0_PHY_DSIPHY_DLN0_DSIPHY_DLN0_DSI_DATALN", 0x00094600, 0x00000080 },
  { "DSI_0_PHY_DSIPHY_DLN1_DSIPHY_DLN1_DSI_DATALN", 0x00094680, 0x00000080 },
  { "DSI_0_PHY_DSIPHY_DLN2_DSIPHY_DLN2_DSI_DATALN", 0x00094700, 0x00000080 },
  { "DSI_0_PHY_DSIPHY_DLN3_DSIPHY_DLN3_DSI_DATALN", 0x00094780, 0x00000080 },
  { "DSI_0_PHY_DSIPHY_CKLN_DSIPHY_CKLN_DSI_DATALN", 0x00094800, 0x00000080 },
  { "DSI_0_PHY_DSIPHY_PLL_DSIPHY_PLL_DSI_QLINK_COM", 0x00094a00, 0x000001e0 },
  { "DSI_1",                                       0x00096000, 0x00001000 },
  { "DSI_1_DSI",                                   0x00096000, 0x00000400 },
  { "DSI_1_PHY",                                   0x00096400, 0x00000800 },
  { "DSI_1_PHY_DSIPHY_CMN_DSIPHY_CMN_DSI_COMMON",  0x00096400, 0x0000012c },
  { "DSI_1_PHY_DSIPHY_DLN0_DSIPHY_DLN0_DSI_DATALN", 0x00096600, 0x00000080 },
  { "DSI_1_PHY_DSIPHY_DLN1_DSIPHY_DLN1_DSI_DATALN", 0x00096680, 0x00000080 },
  { "DSI_1_PHY_DSIPHY_DLN2_DSIPHY_DLN2_DSI_DATALN", 0x00096700, 0x00000080 },
  { "DSI_1_PHY_DSIPHY_DLN3_DSIPHY_DLN3_DSI_DATALN", 0x00096780, 0x00000080 },
  { "DSI_1_PHY_DSIPHY_CKLN_DSIPHY_CKLN_DSI_DATALN", 0x00096800, 0x00000080 },
  { "DSI_1_PHY_DSIPHY_PLL_DSIPHY_PLL_DSI_QLINK_COM", 0x00096a00, 0x000001e0 },
  { "HDMI",                                        0x000a0000, 0x00001300 },
  { "HDMI_PHY_QSERDES_COM_QSERDES_COM_HDMI_COM",   0x000a0600, 0x0000018c },
  { "HDMI_PHY_QSERDES_TX_L0_QSERDES_TX_L0_HDMI_TX", 0x000a0a00, 0x00000038 },
  { "HDMI_PHY_QSERDES_TX_L1_QSERDES_TX_L1_HDMI_TX", 0x000a0c00, 0x00000038 },
  { "HDMI_PHY_QSERDES_TX_L2_QSERDES_TX_L2_HDMI_TX", 0x000a0e00, 0x00000038 },
  { "HDMI_PHY_QSERDES_TX_L3_QSERDES_TX_L3_HDMI_TX", 0x000a1000, 0x00000038 },
  { "HDMI_PHY",                                    0x000a1200, 0x000000e8 },
  { "MHL",                                         0x000a8000, 0x00000800 },
  { "MHL_CBUS",                                    0x000a8400, 0x00000100 },
  { "MHL_PHY",                                     0x000a8500, 0x00000124 },
  { "MHL_PHY_PLL",                                 0x000a8700, 0x000000f4 },
  { "ROT",                                         0x000a8800, 0x00002900 },
  { "ROT_ROTTOP",                                  0x000a8800, 0x00000100 },
  { "ROT_SSPP",                                    0x000a8900, 0x00000200 },
  { "ROT_WB",                                      0x000a8b00, 0x00000300 },
  { "ROT_REGDMA",                                  0x000a8e00, 0x00002100 },
  { "ROT_REGDMA_RAM",                              0x000a8e00, 0x00002000 },
  { "ROT_REGDMA_CSR",                              0x000aae00, 0x00000100 },
  { "ROT_QDSS",                                    0x000aaf00, 0x00000200 },
  { "LUT_DMA",                                     0x000ac000, 0x00000100 },
  { "VBIF",                                        0x000b0000, 0x00003000 },
  { "VBIF_MDSS_VBIF_SDE",                          0x000b0000, 0x00003000 },
  { "VBIF_NRT",                                    0x000b8000, 0x00003000 },
  { "VBIF_NRT_MDSS_VBIF_ROT",                      0x000b8000, 0x00003000 },
  { "MDSS_XPU",                                    0x000c8000, 0x00001400 },
  { "MDSS_SEC",                                    0x000d0000, 0x00030000 },
  { "MDSS_SEC_HDCP_SEC_TZ_ONLY",                   0x000d0000, 0x00001000 },
  { "MDSS_SEC_DP_HDCP_SEC_TZ_ONLY",                0x000d1000, 0x00001000 },
  { "MDSS_SEC_HDCP_SEC_TZ_HV",                     0x000d8000, 0x00001000 },
  { "MDSS_SEC_DSI_0_SEC",                          0x000d9000, 0x00001000 },
  { "MDSS_SEC_DSI_1_SEC",                          0x000da000, 0x00001000 },
  { "MDSS_SEC_DP_HDCP_SEC_TZ_HV",                  0x000db000, 0x00001000 },
  { "MDSS_SEC_HDCP_SEC_TZ_HV_HLOS",                0x000e0000, 0x00001000 },
  { "MDSS_SEC_DP_HDCP_SEC_TZ_HV_HLOS",             0x000e1000, 0x00001000 },
  { "MDSS_SEC_HDCP_SEC_DBG",                       0x000f0000, 0x00001000 },
  { "MDSS_SEC_DP_HDCP_SEC_DBG",                    0x000f1000, 0x00001000 },
  { "DISP_CC",                                     0x00100000, 0x00020000 },
  { "DISP_CC_DISP_CC_REG",                         0x00100000, 0x00020000 },
  { "SDE_RSCC_WRAPPER",                            0x00120000, 0x00020000 },
  { "SDE_SDE_RSCC",                                0x00120000, 0x00010000 },
  { "SDE_RSCC_RSCC_RSC",                           0x00120000, 0x00010000 },
  { "SDE_RSCC_WRAPPER_LOCAL",                      0x00130000, 0x00001000 },
  { "SDE_RSCC_QTMR_AC",                            0x00131000, 0x00001000 },
  { "SDE_RSCC_F0_QTMR_V1",                         0x00132000, 0x00001000 },
  { "SDE_RSCC_F1_QTMR_V1",                         0x00133000, 0x00001000 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_AOP_CODE_RAM_START_ADDRESS[] =
{
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_AOSS[] =
{
  { "AOP",                                         0x00000000, 0x00100000 },
  { "AOP_DEC",                                     0x00080000, 0x00002000 },
  { "AOP_QTMR_AC",                                 0x00090000, 0x00001000 },
  { "AOP_F0_QTMR_V1_F0",                           0x000a0000, 0x00001000 },
  { "AOP_F1_QTMR_V1_F1",                           0x000b0000, 0x00001000 },
  { "AOP_VMIDMT",                                  0x000c0000, 0x00001000 },
  { "AOP_RSCC",                                    0x000d0000, 0x00010000 },
  { "AOP_RSCC_RSCC_RSC",                           0x000d0000, 0x00010000 },
  { "RPMH",                                        0x00200000, 0x01010000 },
  { "RPMH_PDC_PDC_WRAPPER",                        0x00200000, 0x00400000 },
  { "RPMH_PDC_APPS_APPS_PDC",                      0x00200000, 0x00030000 },
  { "RPMH_PDC_APPS_APPS_PDC_SEQ_MEM",              0x00400000, 0x00010000 },
  { "RPMH_PDC_SP_SP_PDC",                          0x00230000, 0x00010000 },
  { "RPMH_PDC_SP_SP_PDC_SEQ_MEM",                  0x00430000, 0x00010000 },
  { "RPMH_PDC_AUDIO_AUDIO_PDC",                    0x00240000, 0x00010000 },
  { "RPMH_PDC_AUDIO_AUDIO_PDC_SEQ_MEM",            0x00440000, 0x00010000 },
  { "RPMH_PDC_SENSORS_SENSORS_PDC",                0x00250000, 0x00010000 },
  { "RPMH_PDC_SENSORS_SENSORS_PDC_SEQ_MEM",        0x00450000, 0x00010000 },
  { "RPMH_PDC_AOP_AOP_PDC",                        0x00260000, 0x00010000 },
  { "RPMH_PDC_AOP_AOP_PDC_SEQ_MEM",                0x00460000, 0x00010000 },
  { "RPMH_PDC_DEBUG_DEBUG_PDC",                    0x00270000, 0x00010000 },
  { "RPMH_PDC_DEBUG_DEBUG_PDC_SEQ_MEM",            0x00470000, 0x00010000 },
  { "RPMH_PDC_GPU_GPU_PDC",                        0x00280000, 0x00010000 },
  { "RPMH_PDC_GPU_GPU_PDC_SEQ_MEM",                0x00480000, 0x00010000 },
  { "RPMH_PDC_DISPLAY_DISPLAY_PDC",                0x00290000, 0x00010000 },
  { "RPMH_PDC_DISPLAY_DISPLAY_PDC_SEQ_MEM",        0x00490000, 0x00010000 },
  { "RPMH_PDC_COMPUTE_COMPUTE_PDC",                0x002a0000, 0x00010000 },
  { "RPMH_PDC_COMPUTE_COMPUTE_PDC_SEQ_MEM",        0x004a0000, 0x00010000 },
  { "RPMH_PDC_MODEM_MODEM_PDC",                    0x002b0000, 0x00020000 },
  { "RPMH_PDC_MODEM_MODEM_PDC_SEQ_MEM",            0x004b0000, 0x00010000 },
  { "RPMH_PDC_PDC_GLOBAL",                         0x002e0000, 0x00020000 },
  { "RPMH_ARC_ARC_WRAPPER",                        0x00600000, 0x00040000 },
  { "RPMH_ARC_ARC_VT",                             0x00600000, 0x001e0000 },
  { "RPMH_ARC_ARC_RM",                             0x007e0000, 0x0000d000 },
  { "RPMH_ARC_ARC_WRAPPER_CSR",                    0x007ed000, 0x00000100 },
  { "RPMH_ARC_ARC_ITC",                            0x007ed100, 0x00000181 },
  { "RPMH_ARC_ARC_ACC",                            0x007ed300, 0x00000100 },
  { "RPMH_ARC_ARC_DA",                             0x007ed400, 0x00000400 },
  { "RPMH_ARC_ARC_IRQ",                            0x007ed800, 0x00000400 },
  { "RPMH_ARC_ARC_PV",                             0x007ee000, 0x00000a00 },
  { "RPMH_ARC_ARC_SW_OVERRIDE",                    0x007eea00, 0x00000500 },
  { "RPMH_ARC_ARC_SEQ_MEM",                        0x007ef000, 0x00001000 },
  { "RPMH_VRM_VRM",                                0x00800000, 0x00200000 },
  { "RPMH_VRM_VRM_VOTETABLE",                      0x00800000, 0x001e0000 },
  { "RPMH_VRM_VRM_RES_CFG",                        0x009e0000, 0x00010000 },
  { "RPMH_VRM_VRM_TYPE_CFG",                       0x009f0000, 0x00001000 },
  { "RPMH_VRM_VRM_INTERRUPT",                      0x009f1000, 0x00001000 },
  { "RPMH_VRM_VRM_STATUS",                         0x009f2000, 0x00001000 },
  { "RPMH_VRM_VRM_SEQMEM",                         0x009f3000, 0x00002000 },
  { "RPMH_VRM_VRM_CFG",                            0x009f5000, 0x00000800 },
  { "RPMH_BCM_BCM_TOP",                            0x00a00000, 0x00200000 },
  { "RPMH_BCM_BCM_VOTETABLE",                      0x00a00000, 0x00100000 },
  { "RPMH_BCM_BCM_ND_CFG",                         0x00be0000, 0x00003000 },
  { "RPMH_BCM_BCM_CD_CFG",                         0x00be3000, 0x00001000 },
  { "RPMH_BCM_BCM_SEQ_CFG",                        0x00be4000, 0x00001200 },
  { "RPMH_BCM_BCM_TCS_CFG",                        0x00be5400, 0x00001400 },
  { "RPMH_BCM_BCM_AU_CFG",                         0x00be6800, 0x00000184 },
  { "RPMH_BCM_BCM_CFG",                            0x00be7000, 0x00000200 },
  { "RPMH_BCM_BCM_STATUS",                         0x00bf0000, 0x00001f00 },
  { "RPMH_BCM_BCM_INTERRUPT",                      0x00bf1f00, 0x00000100 },
  { "RPMH_BCM_BCM_AU_MEM",                         0x00bf2000, 0x00000400 },
  { "RPMH_BCM_BCM_SEQ_MEM",                        0x00bf2400, 0x00000100 },
  { "RPMH_BCM_BCM_SW_OVERRIDE",                    0x00bf2600, 0x00000800 },
  { "RPMH_DDRAUX_DDR_AUX_TOP",                     0x00c00000, 0x00200000 },
  { "RPMH_DDRAUX_DDR_AUX_TCS_CFG",                 0x00c00000, 0x00000200 },
  { "RPMH_DDRAUX_DDR_AUX_SEQ_MEM",                 0x00c00200, 0x00000500 },
  { "RPMH_DDRAUX_DDR_AUX",                         0x00c00700, 0x00000900 },
  { "RPMH_CPRF_CPRF",                              0x01200000, 0x00010000 },
  { "AOSS_PERIPHERALS",                            0x01210000, 0x00080000 },
  { "G_CTRL_CNTR",                                 0x01210000, 0x00001000 },
  { "G_RD_CNTR",                                   0x01220000, 0x00001000 },
  { "SLP_CNTR",                                    0x01221000, 0x00001000 },
  { "TSENS0",                                      0x01222000, 0x00001000 },
  { "TSENS1",                                      0x01223000, 0x00001000 },
  { "WDOG",                                        0x01230000, 0x00000020 },
  { "SSCAON",                                      0x01240000, 0x00000010 },
  { "WCSSAON",                                     0x01250000, 0x00000010 },
  { "AOSS_MISC",                                   0x01260000, 0x00003000 },
  { "TSENS0_TM",                                   0x01263000, 0x00001000 },
  { "PSHOLD",                                      0x01264000, 0x00001000 },
  { "TSENS1_TM",                                   0x01265000, 0x00001000 },
  { "VDD_AOSS_APM",                                0x01266000, 0x00000100 },
  { "VDD_CX_APM",                                  0x01267000, 0x00000100 },
  { "VDD_SSCMX_APM",                               0x01268000, 0x00000100 },
  { "AOSS_MPU_MPU32Q2N7S1V1_20_CL36M31L12_AHB",    0x01290000, 0x00001a00 },
  { "AOSS_CC",                                     0x012a0000, 0x00060000 },
  { "AOSS_CC_AOSS_CC_REG",                         0x012a0000, 0x00060000 },
  { "PMIC_ARB",                                    0x01400000, 0x02800000 },
  { "SPMI_CFG_TOP",                                0x01400000, 0x00030000 },
  { "SPMI_GENI_CFG",                               0x0140a000, 0x00000700 },
  { "SPMI_CFG",                                    0x0140a700, 0x00025900 },
  { "SPMI_PIC_OWNER",                              0x03700000, 0x000a0000 },
  { "PMIC_ARB_MGPI",                               0x01460000, 0x00040004 },
  { "PMIC_ARB_MPU32Q2N7S1V1_25_CL36M27L12_AHB",    0x01430000, 0x00001c80 },
  { "PMIC_ARB_CORE",                               0x01440000, 0x00010000 },
  { "PMIC_ARB_CORE_REGISTERS",                     0x01600000, 0x02000000 },
  { "PMIC_ARB_CORE_REGISTERS_OBS",                 0x03600000, 0x00100000 },
  { "OBS_ACC",                                     0x01450000, 0x00000400 },
  { "SWAO_DEBUG",                                  0x03c00000, 0x00010000 },
  { "TPDA_TPDA_S2_W128_D8_M64_CSE2C17BFE",         0x03c01000, 0x00001000 },
  { "TPDM0_TPDM_ATB128_MCMB4_CS6E88EB34",          0x03c02000, 0x00001000 },
  { "TPDM0_TPDM_ATB128_MCMB4_CS6E88EB34_SUB",      0x03c02280, 0x00000d80 },
  { "TPDM0_TPDM_ATB128_MCMB4_CS6E88EB34_GPR",      0x03c02000, 0x0000027d },
  { "TPDM1_TPDM_ATB32_DSB64_CSF8B13A8B",           0x03c03000, 0x00001000 },
  { "TPDM1_TPDM_ATB32_DSB64_CSF8B13A8B_SUB",       0x03c03280, 0x00000d80 },
  { "TPDM1_TPDM_ATB32_DSB64_CSF8B13A8B_GPR",       0x03c03000, 0x0000027d },
  { "SWAO_CTI0_CSCTI",                             0x03c04000, 0x00001000 },
  { "SWAO_CTI1_CSCTI",                             0x03c05000, 0x00001000 },
  { "SWAO_CTI2_CSCTI",                             0x03c06000, 0x00001000 },
  { "SWAO_CTI3_CSCTI",                             0x03c07000, 0x00001000 },
  { "FUN0_CXATBFUNNEL_128W8SP",                    0x03c08000, 0x00001000 },
  { "TMC_CXTMC_F128W32K",                          0x03c09000, 0x00001000 },
  { "REPLIC_CXATBREPLICATOR_64WP",                 0x03c0a000, 0x00001000 },
  { "IPCBMON_TGU_QC_TGU_APCLK_CS5E8D06A4",         0x03c0c000, 0x00001000 },
  { "SWAOCSR_SWAOCSR",                             0x03c0e000, 0x00001000 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_AOP_DATA_RAM_START_ADDRESS[] =
{
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_AOP_SS_MSG_RAM_START_ADDRESS[] =
{
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_MODEM[] =
{
  { "MODEM_VUIC",                                  0x00000000, 0x00200000 },
  { "QLSS_QLBR_TOP",                               0x00020000, 0x00010000 },
  { "QLSS_QLBR_PDMEM",                             0x00020000, 0x00004000 },
  { "QLSS_SEQ",                                    0x00024000, 0x00000200 },
  { "QLSS_QLBR",                                   0x00024200, 0x00000200 },
  { "QLSS_QDBG",                                   0x00024400, 0x00000200 },
  { "MCDMA_X0_3_VUSS2_TOP",                        0x00040000, 0x00020000 },
  { "MCDMA_X0_3_VUSS_CSR",                         0x00040000, 0x00000800 },
  { "MCDMA_VUSS_DMEM_X4X8",                        0x00041000, 0x00004000 },
  { "SVUSS1_X8_2_VUSS1_TOP",                       0x00060000, 0x00020000 },
  { "SVUSS1_X8_2_VUSS_CSR",                        0x00060000, 0x00000800 },
  { "SVUSS1_PMEMSS_CSR",                           0x00060800, 0x00000800 },
  { "SVUSS1_VUSS_DMEM_X4X8",                       0x00061000, 0x00004000 },
  { "SVUSS2_ENC_TOP",                              0x00080000, 0x00020000 },
  { "SVUSS2_VUSS_CSR",                             0x00080000, 0x00000800 },
  { "SVUSS2_VUSS_DMEM_ENC",                        0x00081000, 0x0000d800 },
  { "SVUSS3_X4_1_VUSS2_TOP",                       0x000a0000, 0x00020000 },
  { "SVUSS3_X4_1_VUSS_CSR",                        0x000a0000, 0x00000800 },
  { "SVUSS3_VUSS_DMEM_X4X8",                       0x000a1000, 0x00004000 },
  { "DEMOD1_X0_2_VUSS1_TOP",                       0x000c0000, 0x00020000 },
  { "DEMOD1_X0_2_VUSS_CSR",                        0x000c0000, 0x00000800 },
  { "DEMOD1_PMEMSS_CSR",                           0x000c0800, 0x00000800 },
  { "DEMOD1_VUSS_DMEM_8K",                         0x000c1000, 0x00008000 },
  { "CCH1_CCHP_TOP",                               0x000e0000, 0x00020000 },
  { "CCH1_X0_1_VUSS_CSR",                          0x000e0000, 0x00000800 },
  { "CCH1_VUSS_DMEM_CCHP",                         0x000e1000, 0x00008000 },
  { "DEC1_DECODER",                                0x00100000, 0x00000380 },
  { "VUSS1_X8_0_VUSS1_TOP",                        0x00120000, 0x00020000 },
  { "VUSS1_X8_0_VUSS_CSR",                         0x00120000, 0x00000800 },
  { "VUSS1_PMEMSS_256K_CSR",                       0x00120800, 0x00000800 },
  { "VUSS1_VUSS_DMEM_8K",                          0x00121000, 0x00008000 },
  { "VUSS2_X8_0_VUSS2_TOP",                        0x00140000, 0x00020000 },
  { "VUSS2_X8_0_VUSS_CSR",                         0x00140000, 0x00000800 },
  { "VUSS2_VUSS_DMEM_8K",                          0x00141000, 0x00008000 },
  { "DEMOD2_X0_2_VUSS1_TOP",                       0x00160000, 0x00020000 },
  { "DEMOD2_X0_2_VUSS_CSR",                        0x00160000, 0x00000800 },
  { "DEMOD2_PMEMSS_CSR",                           0x00160800, 0x00000800 },
  { "DEMOD2_VUSS_DMEM_8K",                         0x00161000, 0x00008000 },
  { "CCH2_CCHP_TOP",                               0x00180000, 0x00020000 },
  { "CCH2_X0_1_VUSS_CSR",                          0x00180000, 0x00000800 },
  { "CCH2_VUSS_DMEM_CCHP",                         0x00181000, 0x00008000 },
  { "DEC2_DECODER",                                0x001a0000, 0x00000380 },
  { "VUSS3_X8_0_VUSS1_TOP",                        0x001c0000, 0x00020000 },
  { "VUSS3_X8_0_VUSS_CSR",                         0x001c0000, 0x00000800 },
  { "VUSS3_PMEMSS_256K_CSR",                       0x001c0800, 0x00000800 },
  { "VUSS3_VUSS_DMEM_8K",                          0x001c1000, 0x00008000 },
  { "VUSS4_X8_0_VUSS2_TOP",                        0x001e0000, 0x00020000 },
  { "VUSS4_X8_0_VUSS_CSR",                         0x001e0000, 0x00000800 },
  { "VUSS4_VUSS_DMEM_8K",                          0x001e1000, 0x00008000 },
  { "MPMU_TOP",                                    0x00300000, 0x00100000 },
  { "MODEM_MPMU_CSR",                              0x00300400, 0x00000400 },
  { "MTC_TOP",                                     0x00500000, 0x00010000 },
  { "MCDMA",                                       0x00500400, 0x00000400 },
  { "A2",                                          0x00500800, 0x00000400 },
  { "DBG",                                         0x00500c00, 0x00000400 },
  { "MTC_BRDG",                                    0x00501000, 0x00000400 },
  { "A2_MEM",                                      0x00502000, 0x00002000 },
  { "MCDMA_TS_TRIF",                               0x00504000, 0x00000c00 },
  { "MNOC_AXI_BR_CSR",                             0x00400000, 0x00100000 },
  { "MNOC_AXI_BR_MEM",                             0x01000000, 0x01000000 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_MSS_NAV_MS_MPU_CFG_MPU32Q2N7S1V0_8_CL36M35L12_AHB[] =
{
  { "MSS_NAV_MS_MPU_CFG_MPU32Q2N7S1V0_8_CL36M35L12_AHB", 0x00000000, 0x00001400 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_A1_NOC_MS_MPU_CFG_MPU32Q2N7S1V0_8_CL36M35L12_AHB[] =
{
  { "A1_NOC_MS_MPU_CFG_MPU32Q2N7S1V0_8_CL36M35L12_AHB", 0x00000000, 0x00001400 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_A2_NOC_MS_MPU_CFG_MPU32Q2N7S1V0_16_CL36M35L12_AHB[] =
{
  { "A2_NOC_MS_MPU_CFG_MPU32Q2N7S1V0_16_CL36M35L12_AHB", 0x00000000, 0x00001800 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_SYSTEM_IMEM[] =
{
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_BOOT_IMEM[] =
{
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_SMMU_500_APPS_REG_WRAPPER[] =
{
  { "APPS_MMU500_SMMU_APP",                        0x00000000, 0x00080000 },
  { "SMMU_SS_APPS_LOCAL",                          0x000c0000, 0x00040000 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_SC_TCM[] =
{
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_QDSS_STM[] =
{
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_APSS_HM[] =
{
  { "APSS_BANKED_APSS_CPU_IPM",                    0x00000000, 0x00010000 },
  { "GOLD_APSS_SAW4_1_1_WRAPPER",                  0x00030000, 0x00001000 },
  { "GOLD_SAW4_1_1_SRM",                           0x00030000, 0x00000800 },
  { "GOLD_SAW4_1_1_AVS",                           0x00030800, 0x00000400 },
  { "GOLD_SAW4_1_1_STS",                           0x00030c00, 0x00000400 },
  { "APSS_APSS_CL_IPM",                            0x00010000, 0x00010000 },
  { "APSS_APSS_SEQ_MEM",                           0x000a0000, 0x00010000 },
  { "SILVER_APSS_SAW4_1_1_WRAPPER",                0x00040000, 0x00001000 },
  { "SILVER_SAW4_1_1_SRM",                         0x00040000, 0x00000800 },
  { "SILVER_SAW4_1_1_AVS",                         0x00040800, 0x00000400 },
  { "SILVER_SAW4_1_1_STS",                         0x00040c00, 0x00000400 },
  { "GOLD_ISENSE_CONTROLLER",                      0x00070000, 0x00002000 },
  { "APSS_GNOC_GLADIATOR_NOC",                     0x00100000, 0x0000d080 },
  { "SILVER_ACD",                                  0x00120000, 0x00010000 },
  { "GOLD_ACD",                                    0x00110000, 0x00010000 },
  { "L3_ACD",                                      0x00130000, 0x00010000 },
  { "APSS_APSS_MISC_CLK",                          0x00170000, 0x00001000 },
  { "SILVER_APSS_KRYO_CLK_CTL",                    0x000c0000, 0x00001000 },
  { "GOLD_APSS_KRYO_CLK_CTL",                      0x000b0000, 0x00001000 },
  { "L3_APSS_KRYO_CLK_CTL",                        0x000d0000, 0x00001000 },
  { "APSS_WDT_TMR1_APSS_WDT",                      0x00180000, 0x00001000 },
  { "APSS_SHARED_APSS_SHARED",                     0x00190000, 0x00001000 },
  { "APSS_RSC_APPS_RSCC",                          0x001c0000, 0x00030000 },
  { "APSS_RSC_RSCC_RSCC_RSC",                      0x001c0000, 0x00030000 },
  { "APSS_GIC600_GICD_APSS",                       0x00200000, 0x00170000 },
  { "APSS_ACTPM_WRAP",                             0x00520000, 0x000b0000 },
  { "APSS_OSM_OSM_OSM_WRAPPER",                    0x00540000, 0x00020000 },
  { "APSS_OSM_TOP",                                0x00540000, 0x00000800 },
  { "APSS_OSM_RAIL0_OSM_RAIL0_TOP",                0x00540800, 0x00000800 },
  { "APSS_OSM_RAIL0_CLKDOM0_OSM_RAIL0_CLKDOM0_TOP", 0x00541000, 0x00001400 },
  { "APSS_OSM_RAIL0_CLKDOM0_SEQ",                  0x00542400, 0x00000c00 },
  { "APSS_OSM_RAIL0_CLKDOM1_OSM_RAIL0_CLKDOM1_TOP", 0x00543000, 0x00001400 },
  { "APSS_OSM_RAIL0_CLKDOM1_SEQ",                  0x00544400, 0x00000c00 },
  { "APSS_OSM_RAIL1_OSM_RAIL1_TOP",                0x00545000, 0x00000800 },
  { "APSS_OSM_RAIL1_CLKDOM0_OSM_RAIL1_CLKDOM0_TOP", 0x00545800, 0x00001400 },
  { "APSS_OSM_RAIL1_CLKDOM0_SEQ",                  0x00546c00, 0x00000c00 },
  { "APSS_GOLD_GOLD_CPRH_WRAPPER",                 0x005b0000, 0x00004000 },
  { "APSS_SILVER_SILVER_CPRH_WRAPPER",             0x005c0000, 0x00004000 },
  { "APSS_GOLD_GOLD_LLM_PRI_WRAPPER",              0x00590000, 0x00008000 },
  { "APSS_GOLD_LLM_PRIVATE",                       0x00591000, 0x00000800 },
  { "APSS_GOLD_LLM_PRIVATE_LLM_PRIVATE_LMH_DCVS",  0x00591800, 0x00000401 },
  { "APSS_SILVER_SILVER_LLM_PRI_WRAPPER",          0x00598000, 0x00008000 },
  { "APSS_SILVER_LLM_PRIVATE",                     0x00599000, 0x00000800 },
  { "APSS_SILVER_LLM_PRIVATE_LLM_PRIVATE_LMH_DCVS", 0x00599800, 0x00000401 },
  { "APSS_GOLD_GOLD_LLM_PUB_WRAPPER",              0x00570000, 0x00008000 },
  { "APSS_GOLD_LLM_PUBLIC",                        0x00570000, 0x00000800 },
  { "APSS_GOLD_LLM_PUBLIC_LLM_PUBLIC_LMH_DCVS_PUB", 0x00570800, 0x00000401 },
  { "APSS_SILVER_SILVER_LLM_PUB_WRAPPER",          0x00578000, 0x00008000 },
  { "APSS_SILVER_LLM_PUBLIC",                      0x00578000, 0x00000800 },
  { "APSS_SILVER_LLM_PUBLIC_LLM_PUBLIC_LMH_DCVS_PUB", 0x00578800, 0x00000401 },
  { "APSS_APM_WRAP",                               0x00520000, 0x00001000 },
  { "APSS_QTMR1_QTMR_AC",                          0x00400000, 0x00001000 },
  { "APSS_QTMR1_F0V1_QTMR_V1",                     0x00410000, 0x00001000 },
  { "APSS_QTMR1_F0V2_QTMR_V2",                     0x00420000, 0x00001000 },
  { "APSS_QTMR1_F1V1_QTMR_V1",                     0x00430000, 0x00001000 },
  { "APSS_QTMR1_F2V1_QTMR_V1",                     0x00440000, 0x00001000 },
  { "APSS_QTMR1_F3V1_QTMR_V1",                     0x00450000, 0x00001000 },
  { "APSS_QTMR1_F4V1_QTMR_V1",                     0x00460000, 0x00001000 },
  { "APSS_QTMR1_F5V1_QTMR_V1",                     0x00470000, 0x00001000 },
  { "APSS_QTMR1_F6V1_QTMR_V1",                     0x00480000, 0x00001000 },
  { "APSS_QTMR0_QTMR_AC",                          0x00490000, 0x00001000 },
  { "APSS_ALIAS_4_APSS_CPU_IPM",                   0x00640000, 0x00010000 },
  { "APSS_QTMR0_F0V1_QTMR_V1",                     0x004a0000, 0x00001000 },
  { "APSS_ALIAS_5_APSS_CPU_IPM",                   0x00650000, 0x00010000 },
  { "APSS_QTMR0_F0V2_QTMR_V2",                     0x004b0000, 0x00001000 },
  { "APSS_ALIAS_6_APSS_CPU_IPM",                   0x00660000, 0x00010000 },
  { "APSS_QTMR0_F1V1_QTMR_V1",                     0x004c0000, 0x00001000 },
  { "APSS_ALIAS_7_APSS_CPU_IPM",                   0x00670000, 0x00010000 },
  { "APSS_QTMR0_F2V1_QTMR_V1",                     0x004d0000, 0x00001000 },
  { "APSS_ALIAS_0_APSS_CPU_IPM",                   0x00600000, 0x00010000 },
  { "APSS_QTMR0_F3V1_QTMR_V1",                     0x004e0000, 0x00001000 },
  { "APSS_ALIAS_1_APSS_CPU_IPM",                   0x00610000, 0x00010000 },
  { "APSS_QTMR0_F4V1_QTMR_V1",                     0x004f0000, 0x00001000 },
  { "APSS_ALIAS_2_APSS_CPU_IPM",                   0x00620000, 0x00010000 },
  { "APSS_QTMR0_F5V1_QTMR_V1",                     0x00500000, 0x00001000 },
  { "APSS_ALIAS_3_APSS_CPU_IPM",                   0x00630000, 0x00010000 },
  { "APSS_QTMR0_F6V1_QTMR_V1",                     0x00510000, 0x00001000 },
  { "GOLD_VOLTAGE_SENSOR",                         0x001f8000, 0x00000100 },
  { "SILVER_VOLTAGE_SENSOR",                       0x001f0000, 0x00000100 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_WCSS_WRAPPER[] =
{
  { "WCSS_ECAHB",                                  0x00000000, 0x00000400 },
  { "WCSS_ECAHB_TSLV",                             0x00010000, 0x00001000 },
  { "WCSS_SR",                                     0x00020000, 0x00020000 },
  { "WCSS_HM",                                     0x00080000, 0x00780000 },
  { "WCSS_HM_A_PMM_PMM_DEC",                       0x00080000, 0x00000268 },
  { "WCSS_HM_A_WCSS_WAHB_AHB_SS_WFSS_AHB_SS_DEC",  0x00100000, 0x00000400 },
  { "WCSS_HM_A_WCSS_WAHB_AHB_TSLV_WFSS_WAHB_AHB_TSLV", 0x00101000, 0x00001000 },
  { "WCSS_HM_A_NOC_CFG_WIFI_NOC",                  0x00180000, 0x00008000 },
  { "WCSS_HM_A_WCSS_CLK_CTL_WCSS_CC_REG_DEC",      0x001d0000, 0x00010000 },
  { "WCSS_HM_A_WIFI_APB_1",                        0x00200000, 0x00080000 },
  { "WCSS_HM_A_WIFI_APB_1_A_WMAC_T0_WMAC_TOP_REG_T0", 0x00200000, 0x00020000 },
  { "WCSS_HM_A_WIFI_APB_1_A_WMAC_T0_MAC_PDG_REG",  0x00210000, 0x00000400 },
  { "WCSS_HM_A_WIFI_APB_1_A_WMAC_T0_MAC_TXDMA_REG", 0x00210400, 0x00000400 },
  { "WCSS_HM_A_WIFI_APB_1_A_WMAC_T0_MAC_RXDMA_REG", 0x00210800, 0x00000800 },
  { "WCSS_HM_A_WIFI_APB_1_A_WMAC_T0_MAC_MCMN_REG", 0x00211000, 0x00001000 },
  { "WCSS_HM_A_WIFI_APB_1_A_WMAC_T0_MAC_RXPCU_REG", 0x00212000, 0x00004000 },
  { "WCSS_HM_A_WIFI_APB_1_A_WMAC_T0_MAC_TXPCU_REG", 0x00216000, 0x00004000 },
  { "WCSS_HM_A_WIFI_APB_1_A_WMAC_T0_MAC_AMPI_REG", 0x0021a000, 0x00001000 },
  { "WCSS_HM_A_WIFI_APB_1_A_WMAC_T0_MAC_RXOLE_REG", 0x0021b000, 0x00000200 },
  { "WCSS_HM_A_WIFI_APB_1_A_WMAC_T0_A_RXOLE_PARSER_MAC_RXOLE_PARSER_REG", 0x0021b200, 0x00000400 },
  { "WCSS_HM_A_WIFI_APB_1_A_WMAC_T0_MAC_CCE_REG",  0x0021b600, 0x00000a00 },
  { "WCSS_HM_A_WIFI_APB_1_A_WMAC_T0_MAC_TXOLE_REG", 0x0021c000, 0x00000400 },
  { "WCSS_HM_A_WIFI_APB_1_A_WMAC_T0_A_TXOLE_PARSER_MAC_TXOLE_PARSER_REG", 0x0021c400, 0x00000c00 },
  { "WCSS_HM_A_WIFI_APB_1_A_WMAC_T0_MAC_RRI_REG",  0x0021d000, 0x00002000 },
  { "WCSS_HM_A_WIFI_APB_1_A_WMAC_T0_MAC_CRYPTO_REG", 0x0021f000, 0x00000400 },
  { "WCSS_HM_A_WIFI_APB_1_A_WMAC_T0_MAC_HWSCH_REG", 0x0021f400, 0x00000800 },
  { "WCSS_HM_A_WIFI_APB_1_A_WMAC_T0_MAC_MXI_REG",  0x0021fc00, 0x00000400 },
  { "WCSS_HM_A_WIFI_APB_1_A_WMAC_T1_WMAC_TOP_REG_T1", 0x00220000, 0x00020000 },
  { "WCSS_HM_A_WIFI_APB_1_A_WMAC_T1_MAC_PDG_REG",  0x00230000, 0x00000400 },
  { "WCSS_HM_A_WIFI_APB_1_A_WMAC_T1_MAC_TXDMA_REG", 0x00230400, 0x00000400 },
  { "WCSS_HM_A_WIFI_APB_1_A_WMAC_T1_MAC_RXDMA_REG", 0x00230800, 0x00000800 },
  { "WCSS_HM_A_WIFI_APB_1_A_WMAC_T1_MAC_MCMN_REG", 0x00231000, 0x00001000 },
  { "WCSS_HM_A_WIFI_APB_1_A_WMAC_T1_MAC_RXPCU_REG", 0x00232000, 0x00004000 },
  { "WCSS_HM_A_WIFI_APB_1_A_WMAC_T1_MAC_TXPCU_REG", 0x00236000, 0x00004000 },
  { "WCSS_HM_A_WIFI_APB_1_A_WMAC_T1_MAC_AMPI_REG", 0x0023a000, 0x00001000 },
  { "WCSS_HM_A_WIFI_APB_1_A_WMAC_T1_MAC_RXOLE_REG", 0x0023b000, 0x00000200 },
  { "WCSS_HM_A_WIFI_APB_1_A_WMAC_T1_A_RXOLE_PARSER_MAC_RXOLE_PARSER_REG", 0x0023b200, 0x00000400 },
  { "WCSS_HM_A_WIFI_APB_1_A_WMAC_T1_MAC_CCE_REG",  0x0023b600, 0x00000a00 },
  { "WCSS_HM_A_WIFI_APB_1_A_WMAC_T1_MAC_TXOLE_REG", 0x0023c000, 0x00000400 },
  { "WCSS_HM_A_WIFI_APB_1_A_WMAC_T1_A_TXOLE_PARSER_MAC_TXOLE_PARSER_REG", 0x0023c400, 0x00000c00 },
  { "WCSS_HM_A_WIFI_APB_1_A_WMAC_T1_MAC_RRI_REG",  0x0023d000, 0x00002000 },
  { "WCSS_HM_A_WIFI_APB_1_A_WMAC_T1_MAC_CRYPTO_REG", 0x0023f000, 0x00000400 },
  { "WCSS_HM_A_WIFI_APB_1_A_WMAC_T1_MAC_HWSCH_REG", 0x0023f400, 0x00000800 },
  { "WCSS_HM_A_WIFI_APB_1_A_WMAC_T1_MAC_MXI_REG",  0x0023fc00, 0x00000400 },
  { "WCSS_HM_A_WIFI_APB_1_A_WFSS_CE0_WFSS_CE_0_REG", 0x00240000, 0x00001000 },
  { "WCSS_HM_A_WIFI_APB_1_A_WFSS_CE1_WFSS_CE_1_REG", 0x00241000, 0x00001000 },
  { "WCSS_HM_A_WIFI_APB_1_A_WFSS_CE2_WFSS_CE_2_REG", 0x00242000, 0x00001000 },
  { "WCSS_HM_A_WIFI_APB_1_A_WFSS_CE3_WFSS_CE_3_REG", 0x00243000, 0x00001000 },
  { "WCSS_HM_A_WIFI_APB_1_A_WFSS_CE4_WFSS_CE_4_REG", 0x00244000, 0x00001000 },
  { "WCSS_HM_A_WIFI_APB_1_A_WFSS_CE5_WFSS_CE_5_REG", 0x00245000, 0x00001000 },
  { "WCSS_HM_A_WIFI_APB_1_A_WFSS_CE6_WFSS_CE_6_REG", 0x00246000, 0x00001000 },
  { "WCSS_HM_A_WIFI_APB_1_A_WFSS_CE7_WFSS_CE_7_REG", 0x00247000, 0x00001000 },
  { "WCSS_HM_A_WIFI_APB_1_A_WFSS_CE8_WFSS_CE_8_REG", 0x00248000, 0x00001000 },
  { "WCSS_HM_A_WIFI_APB_1_A_WFSS_CE9_WFSS_CE_9_REG", 0x00249000, 0x00001000 },
  { "WCSS_HM_A_WIFI_APB_1_A_WFSS_CE10_WFSS_CE_10_REG", 0x0024a000, 0x00001000 },
  { "WCSS_HM_A_WIFI_APB_1_A_WFSS_CE11_WFSS_CE_11_REG", 0x0024b000, 0x00001000 },
  { "WCSS_HM_A_WIFI_APB_1_A_WFSS_CE_COMMON_WRAPPER", 0x0024c000, 0x00001000 },
  { "WCSS_HM_A_WIFI_APB_1_A_WFSS_CE_SECURE_WRAPPER", 0x0024d000, 0x00001000 },
  { "WCSS_HM_A_WIFI_APB_1_A_MAC_CMN_PARSER_WFSS_CMN_PARSER_REG", 0x0024e000, 0x00001000 },
  { "WCSS_HM_A_WIFI_APB_1_A_MAC_TRC_MAC_TRC_REG",  0x00250000, 0x00001000 },
  { "WCSS_HM_A_WIFI_APB_1_A_CXC_TOP_CXC_TOP_REG",  0x00260000, 0x00002000 },
  { "WCSS_HM_A_WIFI_APB_1_A_CXC_TOP_CXC_BMH_REG",  0x00260000, 0x00000400 },
  { "WCSS_HM_A_WIFI_APB_1_A_CXC_TOP_CXC_LCMH_REG", 0x00260400, 0x00000400 },
  { "WCSS_HM_A_WIFI_APB_1_A_CXC_TOP_CXC_MCIBASIC_REG", 0x00260800, 0x00000200 },
  { "WCSS_HM_A_WIFI_APB_1_A_CXC_TOP_CXC_LMH_REG",  0x00260a00, 0x00000200 },
  { "WCSS_HM_A_WIFI_APB_1_A_CXC_TOP_CXC_SMH_REG",  0x00260c00, 0x00000200 },
  { "WCSS_HM_A_WIFI_APB_1_A_CXC_TOP_CXC_PMH_REG",  0x00260e00, 0x00000200 },
  { "WCSS_HM_A_WIFI_APB_1_A_WCSS_WAHB_APB_TSLV_WFSS_WAHB_APB_TSLV", 0x00272000, 0x00001000 },
  { "WCSS_HM_A_WIFI_APB_2",                        0x002c0000, 0x00003000 },
  { "WCSS_HM_A_WIFI_APB_2_A_RFACTRL_CH0_RFACTRL_CH0_REG", 0x002c0000, 0x00001000 },
  { "WCSS_HM_A_WIFI_APB_2_A_RFACTRL_CH1_RFACTRL_CH1_REG", 0x002c1000, 0x00001000 },
  { "WCSS_HM_A_WIFI_APB_2_A_BBANA_RFACTRL_BBANA_RFACTRL_REG", 0x002c2000, 0x00001000 },
  { "WCSS_HM_WLANDAC_1",                           0x002c3000, 0x00000100 },
  { "WCSS_HM_WLANDAC_2",                           0x002c4000, 0x00000100 },
  { "WCSS_HM_A_WCSS_DBG",                          0x002d0000, 0x00020000 },
  { "WCSS_HM_A_WCSS_DBG_ROM_ROM_WCSS_DBG_DAPROM",  0x002d0000, 0x00001000 },
  { "WCSS_HM_A_WCSS_DBG_CSR_CSR_WCSS_DBG_CSR",     0x002d1000, 0x00001000 },
  { "WCSS_HM_A_WCSS_DBG_TSGEN_TSGEN_CXTSGEN",      0x002d2000, 0x00001000 },
  { "WCSS_HM_A_WCSS_DBG_TPDM_TPDM_TPDM_ATB64_GPRCLK_CMB40_DSB256_CSC0E9EC9D", 0x002d4000, 0x00001000 },
  { "WCSS_HM_A_WCSS_DBG_TPDM_TPDM_ATB64_GPRCLK_CMB40_DSB256_CSC0E9EC9D_SUB", 0x002d4280, 0x00000d80 },
  { "WCSS_HM_A_WCSS_DBG_TPDM_TPDM_ATB64_GPRCLK_CMB40_DSB256_CSC0E9EC9D_GPR", 0x002d4000, 0x0000027d },
  { "WCSS_HM_A_WCSS_DBG_TPDA_TPDA_TPDA_S1_W128_D8_M64_CS5CD79236", 0x002d5000, 0x00001000 },
  { "WCSS_HM_A_WCSS_DBG_FUN_FUN_CXATBFUNNEL_128W2SP", 0x002d6000, 0x00001000 },
  { "WCSS_HM_A_WCSS_DBG_ETFETB_ETFETB_CXTMC_F128W8K", 0x002d7000, 0x00001000 },
  { "WCSS_HM_A_WCSS_DBG_CTI0_CTI0_CSCTI",          0x002dc000, 0x00001000 },
  { "WCSS_HM_A_WCSS_DBG_CTI1_CTI1_CSCTI",          0x002dd000, 0x00001000 },
  { "WCSS_HM_A_WCSS_DBG_CTI2_CTI2_CSCTI",          0x002de000, 0x00001000 },
  { "WCSS_HM_A_WCSS_DBG_GPR_GPR_CXGPR_8M",         0x002ee000, 0x00001000 },
  { "WCSS_HM_A_WCSS_DBG_DSBMUX_DSBMUX_WCSS_DBG_DSBMUX", 0x002ef000, 0x00001000 },
  { "WCSS_HM_A_WIFI_APB_3",                        0x002f0000, 0x00110000 },
  { "WCSS_HM_A_WIFI_APB_3_A_WCMN_MAC_WCMN_REG",    0x002f0000, 0x00004000 },
  { "WCSS_HM_A_WIFI_APB_3_A_WFAC_T0_BB_REG_MAP_T0", 0x00300000, 0x00060300 },
  { "WCSS_HM_A_WIFI_APB_3_A_WFAC_T0_RXB_REG_MAP_ADR80PLUS", 0x00300000, 0x00000040 },
  { "WCSS_HM_A_WIFI_APB_3_A_WFAC_T0_AGC_REG_MAP_ADR80PLUS", 0x00300100, 0x00000700 },
  { "WCSS_HM_A_WIFI_APB_3_A_WFAC_T0_RXFE_REG_MAP_ADR80PLUS", 0x00301900, 0x00000c00 },
  { "WCSS_HM_A_WIFI_APB_3_A_WFAC_T0_TXTD_REG_MAP_ADR80PLUS", 0x00302500, 0x00002000 },
  { "WCSS_HM_A_WIFI_APB_3_A_WFAC_T0_TPC_REG_MAP_ADR80PLUS", 0x00304500, 0x00001000 },
  { "WCSS_HM_A_WIFI_APB_3_A_WFAC_T0_RFCNTL_REG_MAP_ADR80PLUS", 0x00305500, 0x00000e00 },
  { "WCSS_HM_A_WIFI_APB_3_A_WFAC_T0_AGC1_REG_MAP_ADR80PLUS", 0x00307000, 0x00000400 },
  { "WCSS_HM_A_WIFI_APB_3_A_WFAC_T0_RXFE1_REG_MAP_ADR80PLUS", 0x00308800, 0x00000c00 },
  { "WCSS_HM_A_WIFI_APB_3_A_WFAC_T0_TXTD1_REG_MAP_ADR80PLUS", 0x00309400, 0x00002000 },
  { "WCSS_HM_A_WIFI_APB_3_A_WFAC_T0_TPC1_REG_MAP_ADR80PLUS", 0x0030b400, 0x00001000 },
  { "WCSS_HM_A_WIFI_APB_3_A_WFAC_T0_RFCNTL1_REG_MAP_ADR80PLUS", 0x0030c400, 0x00000e00 },
  { "WCSS_HM_A_WIFI_APB_3_A_WFAC_T0_RSFD_REG_MAP_ADR80PLUS", 0x00340500, 0x00001400 },
  { "WCSS_HM_A_WIFI_APB_3_A_WFAC_T0_SM_REG_MAP_ADR80PLUS", 0x00346300, 0x00000200 },
  { "WCSS_HM_A_WIFI_APB_3_A_WFAC_T0_TXBE_REG_MAP_ADR80PLUS", 0x00346500, 0x00000800 },
  { "WCSS_HM_A_WIFI_APB_3_A_WFAC_T0_RSFD1_REG_MAP_ADR80PLUS", 0x00347400, 0x00001400 },
  { "WCSS_HM_A_WIFI_APB_3_A_WFAC_T0_TXBE1_REG_MAP_ADR80PLUS", 0x0034d400, 0x00000800 },
  { "WCSS_HM_A_WIFI_APB_3_A_WFAC_T0_SVD_REG_MAP_ADR80PLUS", 0x0035e000, 0x00002000 },
  { "WCSS_HM_A_WIFI_APB_3_A_WFAC_T0_DEC_REG_MAP_ADR80PLUS", 0x00360100, 0x00000200 },
  { "WCSS_HM_A_WIFI_APB_3_A_WFAC_T1_BB_REG_MAP_T1", 0x00380000, 0x00060300 },
  { "WCSS_HM_A_WIFI_APB_3_A_WFAC_T1_RXB_REG_MAP_1RX", 0x00380000, 0x00000040 },
  { "WCSS_HM_A_WIFI_APB_3_A_WFAC_T1_AGC_REG_MAP_1RX", 0x00380100, 0x00000700 },
  { "WCSS_HM_A_WIFI_APB_3_A_WFAC_T1_RXFE_REG_MAP_1RX", 0x00381900, 0x00000c00 },
  { "WCSS_HM_A_WIFI_APB_3_A_WFAC_T1_TXTD_REG_MAP_1RX", 0x00382500, 0x00002000 },
  { "WCSS_HM_A_WIFI_APB_3_A_WFAC_T1_TPC_REG_MAP_1RX", 0x00384500, 0x00001000 },
  { "WCSS_HM_A_WIFI_APB_3_A_WFAC_T1_RFCNTL_REG_MAP_1RX", 0x00385500, 0x00000e00 },
  { "WCSS_HM_A_WIFI_APB_3_A_WFAC_T1_RSFD_REG_MAP_1RX", 0x003c0500, 0x00001400 },
  { "WCSS_HM_A_WIFI_APB_3_A_WFAC_T1_SM_REG_MAP_1RX", 0x003c6300, 0x00000200 },
  { "WCSS_HM_A_WIFI_APB_3_A_WFAC_T1_TXBE_REG_MAP_1RX", 0x003c6500, 0x00000800 },
  { "WCSS_HM_A_WIFI_APB_3_A_WFAC_T1_SVD_REG_MAP_1RX", 0x003de000, 0x00002000 },
  { "WCSS_HM_A_WIFI_APB_3_A_WFAC_T1_DEC_REG_MAP_1RX", 0x003e0100, 0x00000200 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_PIMEM[] =
{
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_SSC_QDSP6_TCM[] =
{
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_LPASS[] =
{
  { "SSC_QDSP6V65SS",                              0x00400000, 0x00100000 },
  { "SSC_QDSP6V65SS_PUB",                          0x00400000, 0x00004040 },
  { "SSC_QDSP6V65SS_PRIVATE",                      0x00480000, 0x00080000 },
  { "SSC_QDSP6V65SS_CSR",                          0x00480000, 0x0000c23d },
  { "SSC_QDSP6V65SS_L2VIC",                        0x00490000, 0x00001000 },
  { "SSC_QDSP6SS_QDSP6SS_QTMR_AC",                 0x004a0000, 0x00001000 },
  { "SSC_QDSP6SS_QTMR_F0_0",                       0x004a1000, 0x00001000 },
  { "SSC_QDSP6SS_QTMR_F1_1",                       0x004a2000, 0x00001000 },
  { "SSC_QDSP6SS_QTMR_F2_2",                       0x004a3000, 0x00001000 },
  { "SSC_QDSP6SS_QDSP6V65SS_RSCC",                 0x004b0000, 0x00010000 },
  { "SSC_QDSP6SS_RSCC_RSCC_RSC",                   0x004b0000, 0x00010000 },
  { "SSC_QUPV3_QUPV3_ID_0",                        0x00600000, 0x00100000 },
  { "SSC_QUPV3_GSI_TOP",                           0x00600000, 0x00060000 },
  { "SSC_QUPV3_GSI",                               0x00604000, 0x00050000 },
  { "SSC_QUPV3_XPU3",                              0x00600000, 0x00003000 },
  { "SSC_QUPV3_QUPV3_SE_WRAPPER",                  0x00680000, 0x00020000 },
  { "SSC_QUPV3_SE0",                               0x00680000, 0x00004000 },
  { "SSC_QUPV3_SE0_GENI4_CFG",                     0x00680000, 0x00000100 },
  { "SSC_QUPV3_SE0_GENI4_IMAGE_REGS",              0x00680100, 0x00000500 },
  { "SSC_QUPV3_SE0_GENI4_DATA",                    0x00680600, 0x00000600 },
  { "SSC_QUPV3_SE0_QUPV3_SE_DMA",                  0x00680c00, 0x00000400 },
  { "SSC_QUPV3_SE0_GENI4_IMAGE",                   0x00681000, 0x00001000 },
  { "SSC_QUPV3_SE0_QUPV3_SEC",                     0x00682000, 0x00001000 },
  { "SSC_QUPV3_SE1",                               0x00684000, 0x00004000 },
  { "SSC_QUPV3_SE1_GENI4_CFG",                     0x00684000, 0x00000100 },
  { "SSC_QUPV3_SE1_GENI4_IMAGE_REGS",              0x00684100, 0x00000500 },
  { "SSC_QUPV3_SE1_GENI4_DATA",                    0x00684600, 0x00000600 },
  { "SSC_QUPV3_SE1_QUPV3_SE_DMA",                  0x00684c00, 0x00000400 },
  { "SSC_QUPV3_SE1_GENI4_IMAGE",                   0x00685000, 0x00001000 },
  { "SSC_QUPV3_SE1_QUPV3_SEC",                     0x00686000, 0x00001000 },
  { "SSC_QUPV3_SE2",                               0x00688000, 0x00004000 },
  { "SSC_QUPV3_SE2_GENI4_CFG",                     0x00688000, 0x00000100 },
  { "SSC_QUPV3_SE2_GENI4_IMAGE_REGS",              0x00688100, 0x00000500 },
  { "SSC_QUPV3_SE2_GENI4_DATA",                    0x00688600, 0x00000600 },
  { "SSC_QUPV3_SE2_QUPV3_SE_DMA",                  0x00688c00, 0x00000400 },
  { "SSC_QUPV3_SE2_GENI4_IMAGE",                   0x00689000, 0x00001000 },
  { "SSC_QUPV3_SE2_QUPV3_SEC",                     0x0068a000, 0x00001000 },
  { "SSC_QUPV3_SE3",                               0x0068c000, 0x00004000 },
  { "SSC_QUPV3_SE3_GENI4_CFG",                     0x0068c000, 0x00000100 },
  { "SSC_QUPV3_SE3_GENI4_IMAGE_REGS",              0x0068c100, 0x00000500 },
  { "SSC_QUPV3_SE3_GENI4_DATA",                    0x0068c600, 0x00000600 },
  { "SSC_QUPV3_SE3_QUPV3_SE_DMA",                  0x0068cc00, 0x00000400 },
  { "SSC_QUPV3_SE3_GENI4_IMAGE",                   0x0068d000, 0x00001000 },
  { "SSC_QUPV3_SE3_QUPV3_SEC",                     0x0068e000, 0x00001000 },
  { "SSC_QUPV3_SE4",                               0x00690000, 0x00004000 },
  { "SSC_QUPV3_SE4_GENI4_CFG",                     0x00690000, 0x00000100 },
  { "SSC_QUPV3_SE4_GENI4_IMAGE_REGS",              0x00690100, 0x00000500 },
  { "SSC_QUPV3_SE4_GENI4_DATA",                    0x00690600, 0x00000600 },
  { "SSC_QUPV3_SE4_QUPV3_SE_DMA",                  0x00690c00, 0x00000400 },
  { "SSC_QUPV3_SE4_GENI4_IMAGE",                   0x00691000, 0x00001000 },
  { "SSC_QUPV3_SE4_QUPV3_SEC",                     0x00692000, 0x00001000 },
  { "SSC_QUPV3_SE5",                               0x00694000, 0x00004000 },
  { "SSC_QUPV3_SE5_GENI4_CFG",                     0x00694000, 0x00000100 },
  { "SSC_QUPV3_SE5_GENI4_IMAGE_REGS",              0x00694100, 0x00000500 },
  { "SSC_QUPV3_SE5_GENI4_DATA",                    0x00694600, 0x00000600 },
  { "SSC_QUPV3_SE5_QUPV3_SE_DMA",                  0x00694c00, 0x00000400 },
  { "SSC_QUPV3_SE5_GENI4_IMAGE",                   0x00695000, 0x00001000 },
  { "SSC_QUPV3_SE5_QUPV3_SEC",                     0x00696000, 0x00001000 },
  { "SSC_QUPV3_QUPV3_COMMON",                      0x006c0000, 0x00002000 },
  { "SSC_QUPV3_VMIDMT_SMR_48_SSD6_SID6_MA48_SP48_36", 0x006c6000, 0x00001000 },
  { "SSC_SDC",                                     0x00800000, 0x00100000 },
  { "SSC_SDC_DEC",                                 0x00880000, 0x00002000 },
  { "SSC_SDC_VMIDMT",                              0x008c0000, 0x00001000 },
  { "LPASS_STM",                                   0x00a00000, 0x00100000 },
  { "MCC",                                         0x00b00000, 0x00080000 },
  { "SCC_MCC_SSC_REG",                             0x00b00000, 0x00010000 },
  { "SSC_MCC_SCC",                                 0x00b10000, 0x00030000 },
  { "SSC_SCC_SCC_REG",                             0x00b10000, 0x00030000 },
  { "SSC_LPASS_TLMM_CSR",                          0x00b40000, 0x00020000 },
  { "LPASS_LPASS_QSM",                             0x00b60000, 0x00003000 },
  { "LPASS_QOS_QOS_LPASS_GENERIC",                 0x00b60000, 0x00000100 },
  { "LPASS_QOS_QOS_LPASS_DANGER",                  0x00b60300, 0x00000200 },
  { "LPASS_QOS_QOS_LPASS_INTERRUPTS",              0x00b61000, 0x00002000 },
  { "LPASS_QOS_HEALTH_CORE_HEALTH_CORE",           0x00b63000, 0x00000100 },
  { "LPASS_LPASS_TCSR_REGS",                       0x00b68000, 0x0000b000 },
  { "CCD",                                         0x00b80000, 0x00010000 },
  { "SSC_SSC_LPA_RSCC",                            0x00b90000, 0x00010000 },
  { "SSC_RSCC_RSCC_RSC",                           0x00b90000, 0x00010000 },
  { "SSC_LPASS_LPM",                               0x00ba0000, 0x00006800 },
  { "SSC_QTMR_AC",                                 0x00ba8000, 0x00001000 },
  { "SSC_QTMR_F0_0",                               0x00ba9000, 0x00001000 },
  { "SSC_QTMR_F1_1",                               0x00baa000, 0x00001000 },
  { "SSC_Q6_MPU_MPU32Q2N7S1V0_12_CL36M35L11_AHB",  0x00bac000, 0x00001600 },
  { "SSC_SDC_MPU_MPU32Q2N7S1V0_12_CL36M35L11_AHB", 0x00bae000, 0x00001600 },
  { "SSC_CX_VOLTAGE_SENSOR",                       0x00bb0000, 0x00000100 },
  { "SSC_MX_VOLTAGE_SENSOR",                       0x00bb1000, 0x00000100 },
  { "SSC_NOC",                                     0x00be0000, 0x00012080 },
  { "LPASS_CC",                                    0x00c00000, 0x00050000 },
  { "LPASS_CSR",                                   0x00cc0000, 0x00026000 },
  { "LPASS_AHBI_TIME",                             0x00cf6000, 0x00001000 },
  { "LPASS_LPASS_SYNC_WRAPPER",                    0x00cf7000, 0x00000ffd },
  { "LPASS_AVTIMER",                               0x00cf7000, 0x00000100 },
  { "LPASS_HDMITX",                                0x00cf8000, 0x00007000 },
  { "LPASS_ATIMER",                                0x00cff000, 0x00000100 },
  { "LPASS_LPASS_LPM",                             0x00d20000, 0x00010000 },
  { "LPASS_RESAMPLER",                             0x00d38000, 0x00008000 },
  { "LPASS_AHBL_TIME",                             0x00d43000, 0x00001000 },
  { "LPASS_AUD_SBMASTER0_BASE",                    0x00d80000, 0x00080000 },
  { "LPASS_AUD_SB_SLIMBUS_BAM_LITE",               0x00d80000, 0x0002e000 },
  { "LPASS_AUD_SB_BAM",                            0x00d84000, 0x0002a000 },
  { "LPASS_AUD_SLIMBUS",                           0x00dc0000, 0x0002c000 },
  { "LPASS_QCA_SBMASTER0_BASE",                    0x00e00000, 0x00080000 },
  { "LPASS_QCA_SB_SLIMBUS_BAM_LITE",               0x00e00000, 0x00024000 },
  { "LPASS_QCA_SB_BAM",                            0x00e04000, 0x00020000 },
  { "LPASS_QCA_SLIMBUS",                           0x00e40000, 0x0002c000 },
  { "LPASS_LPA_IF",                                0x00e80000, 0x00027ffd },
  { "LPASS_AUDIO_CODEC",                           0x00ec0000, 0x00000314 },
  { "LPASS_SOUND_WIRE",                            0x00ec1000, 0x00000400 },
  { "LPASS_PAGE",                                  0x00ec1000, 0x00000004 },
  { "LPASS_CDC_TX9",                               0x00ec1304, 0x0000000c },
  { "LPASS_CDC_TX10",                              0x00ec1314, 0x0000000c },
  { "LPASS_CDC_TX11",                              0x00ec1324, 0x0000000c },
  { "LPASS_CDC_TX12",                              0x00ec1334, 0x0000000c },
  { "LPASS_CDC_COMPANDER7",                        0x00ec1024, 0x00000020 },
  { "LPASS_CDC_COMPANDER8",                        0x00ec1044, 0x00000020 },
  { "LPASS_CDC_RX7",                               0x00ec11a4, 0x00000048 },
  { "LPASS_CDC_RX8",                               0x00ec1384, 0x00000048 },
  { "LPASS_CDC_BOOST0",                            0x00ec1064, 0x00000010 },
  { "LPASS_CDC_BOOST1",                            0x00ec1084, 0x00000010 },
  { "LPASS_SWR_AHB_BRIDGE_SWR_AHB_BRIDGE_SWR_AHB_BRIDGE", 0x00ec10a4, 0x00000048 },
  { "LPASS_CDC_CLK_RST_CTRL_CDC_CLK_RST_CTRL_CDC_CLK_RST_CTRL", 0x00ec1104, 0x0000000c },
  { "LPASS_SWR_TOP_SWR_TOP_SWR_TOP",               0x00ec1204, 0x00000038 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_SDC_CODE_RAM_START_ADDRESS[] =
{
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_SDC_DATA_RAM_START_ADDRESS[] =
{
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_SSC_RAM_START_ADDRESS[] =
{
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_LPASS_LPM[] =
{
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_DDR_SPACE[] =
{
  { NULL, 0, 0 }
};

HWIOPhysRegionType HWIOBaseMap[] =
{
  {
    "AOP_SS_MSG_RAM_DRV0",
    (DALSYSMemAddr)0x00000000,
    0x100000000,
    (DALSYSMemAddr)0x00000000,
    HWIOModules_AOP_SS_MSG_RAM_DRV0
  },
  {
    "AOP_SS_MSG_RAM_DRV1",
    (DALSYSMemAddr)0x00010000,
    0x100000000,
    (DALSYSMemAddr)0x00010000,
    HWIOModules_AOP_SS_MSG_RAM_DRV1
  },
  {
    "AOP_SS_MSG_RAM_DRV2",
    (DALSYSMemAddr)0x00020000,
    0x100000000,
    (DALSYSMemAddr)0x00020000,
    HWIOModules_AOP_SS_MSG_RAM_DRV2
  },
  {
    "AOP_SS_MSG_RAM_DRV3",
    (DALSYSMemAddr)0x00030000,
    0x100000000,
    (DALSYSMemAddr)0x00030000,
    HWIOModules_AOP_SS_MSG_RAM_DRV3
  },
  {
    "AOP_SS_MSG_RAM_DRV4",
    (DALSYSMemAddr)0x00040000,
    0x100000000,
    (DALSYSMemAddr)0x00040000,
    HWIOModules_AOP_SS_MSG_RAM_DRV4
  },
  {
    "SSC_MPU_CFG_SSC_MPU_WRAPPER",
    (DALSYSMemAddr)0x0004a000,
    0x00001500,
    (DALSYSMemAddr)0x0004a000,
    HWIOModules_SSC_MPU_CFG_SSC_MPU_WRAPPER
  },
  {
    "AOP_SS_MSG_RAM_DRV5",
    (DALSYSMemAddr)0x00050000,
    0x100000000,
    (DALSYSMemAddr)0x00050000,
    HWIOModules_AOP_SS_MSG_RAM_DRV5
  },
  {
    "AOP_SS_MSG_RAM_DRV6",
    (DALSYSMemAddr)0x00060000,
    0x100000000,
    (DALSYSMemAddr)0x00060000,
    HWIOModules_AOP_SS_MSG_RAM_DRV6
  },
  {
    "AOP_SS_MSG_RAM_DRV7",
    (DALSYSMemAddr)0x00070000,
    0x100000000,
    (DALSYSMemAddr)0x00070000,
    HWIOModules_AOP_SS_MSG_RAM_DRV7
  },
  {
    "AOP_SS_MSG_RAM_DRV8",
    (DALSYSMemAddr)0x00080000,
    0x100000000,
    (DALSYSMemAddr)0x00080000,
    HWIOModules_AOP_SS_MSG_RAM_DRV8
  },
  {
    "AOP_SS_MSG_RAM_DRV9",
    (DALSYSMemAddr)0x00090000,
    0x100000000,
    (DALSYSMemAddr)0x00090000,
    HWIOModules_AOP_SS_MSG_RAM_DRV9
  },
  {
    "AOP_SS_MSG_RAM_DRV10",
    (DALSYSMemAddr)0x000a0000,
    0x100000000,
    (DALSYSMemAddr)0x000a0000,
    HWIOModules_AOP_SS_MSG_RAM_DRV10
  },
  {
    "AOP_SS_MSG_RAM_DRV11",
    (DALSYSMemAddr)0x000b0000,
    0x100000000,
    (DALSYSMemAddr)0x000b0000,
    HWIOModules_AOP_SS_MSG_RAM_DRV11
  },
  {
    "AOP_SS_MSG_RAM_DRV12",
    (DALSYSMemAddr)0x000c0000,
    0x100000000,
    (DALSYSMemAddr)0x000c0000,
    HWIOModules_AOP_SS_MSG_RAM_DRV12
  },
  {
    "AOP_SS_MSG_RAM_DRV13",
    (DALSYSMemAddr)0x000d0000,
    0x100000000,
    (DALSYSMemAddr)0x000d0000,
    HWIOModules_AOP_SS_MSG_RAM_DRV13
  },
  {
    "AOP_SS_MSG_RAM_DRV14",
    (DALSYSMemAddr)0x000e0000,
    0x100000000,
    (DALSYSMemAddr)0x000e0000,
    HWIOModules_AOP_SS_MSG_RAM_DRV14
  },
  {
    "AOP_SS_MSG_RAM_DRV15",
    (DALSYSMemAddr)0x000f0000,
    0x100000000,
    (DALSYSMemAddr)0x000f0000,
    HWIOModules_AOP_SS_MSG_RAM_DRV15
  },
  {
    "CLK_CTL",
    (DALSYSMemAddr)0x00100000,
    0x00200000,
    (DALSYSMemAddr)0x00100000,
    HWIOModules_CLK_CTL
  },
  {
    "BOOT_ROM_START_ADDRESS",
    (DALSYSMemAddr)0x00300000,
    0x100000000,
    (DALSYSMemAddr)0x00300000,
    HWIOModules_BOOT_ROM_START_ADDRESS
  },
  {
    "BOOT_ROM",
    (DALSYSMemAddr)0x00300000,
    0x00100000,
    (DALSYSMemAddr)0x00300000,
    HWIOModules_BOOT_ROM
  },
  {
    "RAMBLUR_PIMEM",
    (DALSYSMemAddr)0x00610000,
    0x00008000,
    (DALSYSMemAddr)0x00610000,
    HWIOModules_RAMBLUR_PIMEM
  },
  {
    "OCIMEM_WRAPPER_CSR",
    (DALSYSMemAddr)0x00618000,
    0x00006000,
    (DALSYSMemAddr)0x00618000,
    HWIOModules_OCIMEM_WRAPPER_CSR
  },
  {
    "SPDM_WRAPPER_TOP",
    (DALSYSMemAddr)0x00620000,
    0x00008000,
    (DALSYSMemAddr)0x00620000,
    HWIOModules_SPDM_WRAPPER_TOP
  },
  {
    "CX_CPR3",
    (DALSYSMemAddr)0x00628000,
    0x00004000,
    (DALSYSMemAddr)0x00628000,
    HWIOModules_CX_CPR3
  },
  {
    "MX_CPR3",
    (DALSYSMemAddr)0x0062c000,
    0x00004000,
    (DALSYSMemAddr)0x0062c000,
    HWIOModules_MX_CPR3
  },
  {
    "SECURITY_CONTROL",
    (DALSYSMemAddr)0x00780000,
    0x00010000,
    (DALSYSMemAddr)0x00780000,
    HWIOModules_SECURITY_CONTROL
  },
  {
    "PRNG_CFG_PRNG_TOP",
    (DALSYSMemAddr)0x00790000,
    0x00010000,
    (DALSYSMemAddr)0x00790000,
    HWIOModules_PRNG_CFG_PRNG_TOP
  },
  {
    "GLM_WRAPPER",
    (DALSYSMemAddr)0x007a0000,
    0x00002000,
    (DALSYSMemAddr)0x007a0000,
    HWIOModules_GLM_WRAPPER
  },
  {
    "PERIPH_SS_SDC1_SDCC5_TOP",
    (DALSYSMemAddr)0x007c0000,
    0x00020000,
    (DALSYSMemAddr)0x007c0000,
    HWIOModules_PERIPH_SS_SDC1_SDCC5_TOP
  },
  {
    "QUPV3_0_QUPV3_ID_1",
    (DALSYSMemAddr)0x00800000,
    0x00100000,
    (DALSYSMemAddr)0x00800000,
    HWIOModules_QUPV3_0_QUPV3_ID_1
  },
  {
    "QUPV3_1_QUPV3_ID_1",
    (DALSYSMemAddr)0x00a00000,
    0x00100000,
    (DALSYSMemAddr)0x00a00000,
    HWIOModules_QUPV3_1_QUPV3_ID_1
  },
  {
    "AHB2PHY_SOUTH",
    (DALSYSMemAddr)0x00ff0000,
    0x00004000,
    (DALSYSMemAddr)0x00ff0000,
    HWIOModules_AHB2PHY_SOUTH
  },
  {
    "TITAN_THROTTLE_THROTTLE_WRAPPER_3",
    (DALSYSMemAddr)0x01000000,
    0x00001000,
    (DALSYSMemAddr)0x01000000,
    HWIOModules_TITAN_THROTTLE_THROTTLE_WRAPPER_3
  },
  {
    "MDSS_THROTTLE_THROTTLE_WRAPPER_3",
    (DALSYSMemAddr)0x01001000,
    0x00001000,
    (DALSYSMemAddr)0x01001000,
    HWIOModules_MDSS_THROTTLE_THROTTLE_WRAPPER_3
  },
  {
    "VIDEO_THROTTLE_THROTTLE_BWMON_WRAPPER_2_VENUS",
    (DALSYSMemAddr)0x01002000,
    0x00001000,
    (DALSYSMemAddr)0x01002000,
    HWIOModules_VIDEO_THROTTLE_THROTTLE_BWMON_WRAPPER_2_VENUS
  },
  {
    "DCC_RAM_START_ADDRESS",
    (DALSYSMemAddr)0x010a0000,
    0x100000000,
    (DALSYSMemAddr)0x010a0000,
    HWIOModules_DCC_RAM_START_ADDRESS
  },
  {
    "QC_DCC_CSAFB9399A",
    (DALSYSMemAddr)0x010a0000,
    0x00010000,
    (DALSYSMemAddr)0x010a0000,
    HWIOModules_QC_DCC_CSAFB9399A
  },
  {
    "QM",
    (DALSYSMemAddr)0x010b8000,
    0x00004000,
    (DALSYSMemAddr)0x010b8000,
    HWIOModules_QM
  },
  {
    "QM_MPU_CFG_QM_MPU_WRAPPER",
    (DALSYSMemAddr)0x010bc000,
    0x00001200,
    (DALSYSMemAddr)0x010bc000,
    HWIOModules_QM_MPU_CFG_QM_MPU_WRAPPER
  },
  {
    "DDR_SS",
    (DALSYSMemAddr)0x01100000,
    0x00400000,
    (DALSYSMemAddr)0x01100000,
    HWIOModules_DDR_SS
  },
  {
    "CONFIG_NOC_WRAPPER",
    (DALSYSMemAddr)0x01500000,
    0x00028000,
    (DALSYSMemAddr)0x01500000,
    HWIOModules_CONFIG_NOC_WRAPPER
  },
  {
    "SYSTEM_NOC",
    (DALSYSMemAddr)0x01620000,
    0x00016080,
    (DALSYSMemAddr)0x01620000,
    HWIOModules_SYSTEM_NOC
  },
  {
    "AGGRE_NOC_AGGRE_NOC",
    (DALSYSMemAddr)0x016e0000,
    0x00040000,
    (DALSYSMemAddr)0x016e0000,
    HWIOModules_AGGRE_NOC_AGGRE_NOC
  },
  {
    "MMSS_NOC",
    (DALSYSMemAddr)0x01740000,
    0x0001c100,
    (DALSYSMemAddr)0x01740000,
    HWIOModules_MMSS_NOC
  },
  {
    "UFS_MEM_UFS_1LANE_REGS",
    (DALSYSMemAddr)0x01d80000,
    0x00020000,
    (DALSYSMemAddr)0x01d80000,
    HWIOModules_UFS_MEM_UFS_1LANE_REGS
  },
  {
    "CRYPTO0_CRYPTO_TOP",
    (DALSYSMemAddr)0x01dc0000,
    0x00040000,
    (DALSYSMemAddr)0x01dc0000,
    HWIOModules_CRYPTO0_CRYPTO_TOP
  },
  {
    "IPA_0_IPA_WRAPPER",
    (DALSYSMemAddr)0x01e00000,
    0x00100000,
    (DALSYSMemAddr)0x01e00000,
    HWIOModules_IPA_0_IPA_WRAPPER
  },
  {
    "CORE_TOP_CSR",
    (DALSYSMemAddr)0x01f00000,
    0x00100000,
    (DALSYSMemAddr)0x01f00000,
    HWIOModules_CORE_TOP_CSR
  },
  {
    "TLMM",
    (DALSYSMemAddr)0x03400000,
    0x00c00000,
    (DALSYSMemAddr)0x03400000,
    HWIOModules_TLMM
  },
  {
    "MODEM_TOP",
    (DALSYSMemAddr)0x04000000,
    0x01000000,
    (DALSYSMemAddr)0x04000000,
    HWIOModules_MODEM_TOP
  },
  {
    "MSS_QDSP6_TCM",
    (DALSYSMemAddr)0x04400000,
    0x100000000,
    (DALSYSMemAddr)0x04400000,
    HWIOModules_MSS_QDSP6_TCM
  },
  {
    "GFX",
    (DALSYSMemAddr)0x05000000,
    0x000a5001,
    (DALSYSMemAddr)0x05000000,
    HWIOModules_GFX
  },
  {
    "GDPM_RAM_START_ADDRESS",
    (DALSYSMemAddr)0x0506c000,
    0x100000000,
    (DALSYSMemAddr)0x0506c000,
    HWIOModules_GDPM_RAM_START_ADDRESS
  },
  {
    "GPUCX_ITCM",
    (DALSYSMemAddr)0x0506d000,
    0x100000000,
    (DALSYSMemAddr)0x0506d000,
    HWIOModules_GPUCX_ITCM
  },
  {
    "GPUCX_DTCM",
    (DALSYSMemAddr)0x05071000,
    0x100000000,
    (DALSYSMemAddr)0x05071000,
    HWIOModules_GPUCX_DTCM
  },
  {
    "QDSS_SOC_DBG",
    (DALSYSMemAddr)0x06000000,
    0x02000000,
    (DALSYSMemAddr)0x06000000,
    HWIOModules_QDSS_SOC_DBG
  },
  {
    "TURING",
    (DALSYSMemAddr)0x08000000,
    0x00800000,
    (DALSYSMemAddr)0x08000000,
    HWIOModules_TURING
  },
  {
    "PERIPH_SS_SDC2_SDCC5_TOP",
    (DALSYSMemAddr)0x08800000,
    0x00020000,
    (DALSYSMemAddr)0x08800000,
    HWIOModules_PERIPH_SS_SDC2_SDCC5_TOP
  },
  {
    "PERIPH_SS_SDC4_SDCC5_TOP",
    (DALSYSMemAddr)0x08840000,
    0x00020000,
    (DALSYSMemAddr)0x08840000,
    HWIOModules_PERIPH_SS_SDC4_SDCC5_TOP
  },
  {
    "PERIPH_SS_TSIF_TSIF_12SEG_WRAPPER",
    (DALSYSMemAddr)0x08880000,
    0x00040000,
    (DALSYSMemAddr)0x08880000,
    HWIOModules_PERIPH_SS_TSIF_TSIF_12SEG_WRAPPER
  },
  {
    "VSENSE_CONTROLLER",
    (DALSYSMemAddr)0x088c8000,
    0x00001000,
    (DALSYSMemAddr)0x088c8000,
    HWIOModules_VSENSE_CONTROLLER
  },
  {
    "PERIPH_SS_PDM_PERPH_WEB",
    (DALSYSMemAddr)0x088d0000,
    0x00004000,
    (DALSYSMemAddr)0x088d0000,
    HWIOModules_PERIPH_SS_PDM_PERPH_WEB
  },
  {
    "PERIPH_SS_AHB2PHY_NORTH",
    (DALSYSMemAddr)0x088e0000,
    0x00010000,
    (DALSYSMemAddr)0x088e0000,
    HWIOModules_PERIPH_SS_AHB2PHY_NORTH
  },
  {
    "USB3_PRI_USB30_PRIM",
    (DALSYSMemAddr)0x0a600000,
    0x00200000,
    (DALSYSMemAddr)0x0a600000,
    HWIOModules_USB3_PRI_USB30_PRIM
  },
  {
    "VIDEO_SS_WRAPPER",
    (DALSYSMemAddr)0x0aa00000,
    0x00200000,
    (DALSYSMemAddr)0x0aa00000,
    HWIOModules_VIDEO_SS_WRAPPER
  },
  {
    "TITAN_SS",
    (DALSYSMemAddr)0x0ac00000,
    0x00121000,
    (DALSYSMemAddr)0x0ac00000,
    HWIOModules_TITAN_SS
  },
  {
    "MDSS",
    (DALSYSMemAddr)0x0ae00000,
    0x00100000,
    (DALSYSMemAddr)0x0ae00000,
    HWIOModules_MDSS
  },
  {
    "AOP_CODE_RAM_START_ADDRESS",
    (DALSYSMemAddr)0x0b000000,
    0x100000000,
    (DALSYSMemAddr)0x0b000000,
    HWIOModules_AOP_CODE_RAM_START_ADDRESS
  },
  {
    "AOSS",
    (DALSYSMemAddr)0x0b000000,
    0x04000000,
    (DALSYSMemAddr)0x0b000000,
    HWIOModules_AOSS
  },
  {
    "AOP_DATA_RAM_START_ADDRESS",
    (DALSYSMemAddr)0x0b0e0000,
    0x100000000,
    (DALSYSMemAddr)0x0b0e0000,
    HWIOModules_AOP_DATA_RAM_START_ADDRESS
  },
  {
    "AOP_SS_MSG_RAM_START_ADDRESS",
    (DALSYSMemAddr)0x0c300000,
    0x100000000,
    (DALSYSMemAddr)0x0c300000,
    HWIOModules_AOP_SS_MSG_RAM_START_ADDRESS
  },
  {
    "MODEM",
    (DALSYSMemAddr)0x10000000,
    0x02000000,
    (DALSYSMemAddr)0x10000000,
    HWIOModules_MODEM
  },
  {
    "MSS_NAV_MS_MPU_CFG_MPU32Q2N7S1V0_8_CL36M35L12_AHB",
    (DALSYSMemAddr)0x14400000,
    0x00001400,
    (DALSYSMemAddr)0x14400000,
    HWIOModules_MSS_NAV_MS_MPU_CFG_MPU32Q2N7S1V0_8_CL36M35L12_AHB
  },
  {
    "A1_NOC_MS_MPU_CFG_MPU32Q2N7S1V0_8_CL36M35L12_AHB",
    (DALSYSMemAddr)0x14402000,
    0x00001400,
    (DALSYSMemAddr)0x14402000,
    HWIOModules_A1_NOC_MS_MPU_CFG_MPU32Q2N7S1V0_8_CL36M35L12_AHB
  },
  {
    "A2_NOC_MS_MPU_CFG_MPU32Q2N7S1V0_16_CL36M35L12_AHB",
    (DALSYSMemAddr)0x14404000,
    0x00001800,
    (DALSYSMemAddr)0x14404000,
    HWIOModules_A2_NOC_MS_MPU_CFG_MPU32Q2N7S1V0_16_CL36M35L12_AHB
  },
  {
    "SYSTEM_IMEM",
    (DALSYSMemAddr)0x14680000,
    0x100000000,
    (DALSYSMemAddr)0x14680000,
    HWIOModules_SYSTEM_IMEM
  },
  {
    "BOOT_IMEM",
    (DALSYSMemAddr)0x14800000,
    0x100000000,
    (DALSYSMemAddr)0x14800000,
    HWIOModules_BOOT_IMEM
  },
  {
    "SMMU_500_APPS_REG_WRAPPER",
    (DALSYSMemAddr)0x15000000,
    0x00110000,
    (DALSYSMemAddr)0x15000000,
    HWIOModules_SMMU_500_APPS_REG_WRAPPER
  },
  {
    "SC_TCM",
    (DALSYSMemAddr)0x15800000,
    0x100000000,
    (DALSYSMemAddr)0x15800000,
    HWIOModules_SC_TCM
  },
  {
    "QDSS_STM",
    (DALSYSMemAddr)0x16000000,
    0x100000000,
    (DALSYSMemAddr)0x16000000,
    HWIOModules_QDSS_STM
  },
  {
    "APSS_HM",
    (DALSYSMemAddr)0x17800000,
    0x00800000,
    (DALSYSMemAddr)0x17800000,
    HWIOModules_APSS_HM
  },
  {
    "WCSS_WRAPPER",
    (DALSYSMemAddr)0x18800000,
    0x00800000,
    (DALSYSMemAddr)0x18800000,
    HWIOModules_WCSS_WRAPPER
  },
  {
    "PIMEM",
    (DALSYSMemAddr)0x1c000000,
    0x100000000,
    (DALSYSMemAddr)0x1c000000,
    HWIOModules_PIMEM
  },
  {
    "SSC_QDSP6_TCM",
    (DALSYSMemAddr)0x62000000,
    0x100000000,
    (DALSYSMemAddr)0x62000000,
    HWIOModules_SSC_QDSP6_TCM
  },
  {
    "LPASS",
    (DALSYSMemAddr)0x62000000,
    0x01000000,
    (DALSYSMemAddr)0x62000000,
    HWIOModules_LPASS
  },
  {
    "SDC_CODE_RAM_START_ADDRESS",
    (DALSYSMemAddr)0x62800000,
    0x100000000,
    (DALSYSMemAddr)0x62800000,
    HWIOModules_SDC_CODE_RAM_START_ADDRESS
  },
  {
    "SDC_DATA_RAM_START_ADDRESS",
    (DALSYSMemAddr)0x628e0000,
    0x100000000,
    (DALSYSMemAddr)0x628e0000,
    HWIOModules_SDC_DATA_RAM_START_ADDRESS
  },
  {
    "SSC_RAM_START_ADDRESS",
    (DALSYSMemAddr)0x62ba0000,
    0x100000000,
    (DALSYSMemAddr)0x62ba0000,
    HWIOModules_SSC_RAM_START_ADDRESS
  },
  {
    "LPASS_LPM",
    (DALSYSMemAddr)0x62d20000,
    0x100000000,
    (DALSYSMemAddr)0x62d20000,
    HWIOModules_LPASS_LPM
  },
  {
    "DDR_SPACE",
    (DALSYSMemAddr)0x80000000,
    0x100000000,
    (DALSYSMemAddr)0x80000000,
    HWIOModules_DDR_SPACE
  },
  { NULL, 0, 0, 0, NULL }
};

