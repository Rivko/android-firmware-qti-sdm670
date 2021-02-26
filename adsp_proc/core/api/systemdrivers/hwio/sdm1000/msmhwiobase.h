#ifndef __MSMHWIOBASE_H__
#define __MSMHWIOBASE_H__
/*
===========================================================================
*/
/**
  @file msmhwiobase.h
  @brief Auto-generated HWIO base include file.
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

  $Header: //components/rel/core.qdsp6/2.1/api/systemdrivers/hwio/sdm1000/msmhwiobase.h#1 $
  $DateTime: 2017/07/21 22:10:47 $
  $Author: pwbldsvc $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * BASE: CLK_CTL
 *--------------------------------------------------------------------------*/

#define CLK_CTL_BASE                                                0xe0100000
#define CLK_CTL_BASE_SIZE                                           0x00200000
#define CLK_CTL_BASE_PHYS                                           0x00100000

/*----------------------------------------------------------------------------
 * BASE: SPDM_WRAPPER_TOP
 *--------------------------------------------------------------------------*/

#define SPDM_WRAPPER_TOP_BASE                                       0xe0420000
#define SPDM_WRAPPER_TOP_BASE_SIZE                                  0x00008000
#define SPDM_WRAPPER_TOP_BASE_PHYS                                  0x00620000

/*----------------------------------------------------------------------------
 * BASE: SECURITY_CONTROL
 *--------------------------------------------------------------------------*/

#define SECURITY_CONTROL_BASE                                       0xe0580000
#define SECURITY_CONTROL_BASE_SIZE                                  0x00010000
#define SECURITY_CONTROL_BASE_PHYS                                  0x00780000

/*----------------------------------------------------------------------------
 * BASE: PRNG_CFG_PRNG_TOP
 *--------------------------------------------------------------------------*/

#define PRNG_CFG_PRNG_TOP_BASE                                      0xe0690000
#define PRNG_CFG_PRNG_TOP_BASE_SIZE                                 0x00010000
#define PRNG_CFG_PRNG_TOP_BASE_PHYS                                 0x00790000

/*----------------------------------------------------------------------------
 * BASE: QUPV3_0_QUPV3_ID_1
 *--------------------------------------------------------------------------*/

#define QUPV3_0_QUPV3_ID_1_BASE                                     0xe0700000
#define QUPV3_0_QUPV3_ID_1_BASE_SIZE                                0x000c7000
#define QUPV3_0_QUPV3_ID_1_BASE_PHYS                                0x00800000

/*----------------------------------------------------------------------------
 * BASE: PERIPH_SS_AHB2PHY_NORTH
 *--------------------------------------------------------------------------*/

#define PERIPH_SS_AHB2PHY_NORTH_BASE                                0xe08f0000
#define PERIPH_SS_AHB2PHY_NORTH_BASE_SIZE                           0x00010000
#define PERIPH_SS_AHB2PHY_NORTH_BASE_PHYS                           0x00ff0000

/*----------------------------------------------------------------------------
 * BASE: CONFIG_NOC_WRAPPER
 *--------------------------------------------------------------------------*/

#define CONFIG_NOC_WRAPPER_BASE                                     0xe0900000
#define CONFIG_NOC_WRAPPER_BASE_SIZE                                0x00028000
#define CONFIG_NOC_WRAPPER_BASE_PHYS                                0x01500000

/*----------------------------------------------------------------------------
 * BASE: SYSTEM_NOC
 *--------------------------------------------------------------------------*/

#define SYSTEM_NOC_BASE                                             0xe0a20000
#define SYSTEM_NOC_BASE_SIZE                                        0x00018200
#define SYSTEM_NOC_BASE_PHYS                                        0x01620000

/*----------------------------------------------------------------------------
 * BASE: AGGRE_NOC_AGGRE_NOC
 *--------------------------------------------------------------------------*/

#define AGGRE_NOC_AGGRE_NOC_BASE                                    0xe0ee0000
#define AGGRE_NOC_AGGRE_NOC_BASE_SIZE                               0x0005c000
#define AGGRE_NOC_AGGRE_NOC_BASE_PHYS                               0x016e0000

/*----------------------------------------------------------------------------
 * BASE: CORE_TOP_CSR
 *--------------------------------------------------------------------------*/

#define CORE_TOP_CSR_BASE                                           0xe1000000
#define CORE_TOP_CSR_BASE_SIZE                                      0x00100000
#define CORE_TOP_CSR_BASE_PHYS                                      0x01f00000

/*----------------------------------------------------------------------------
 * BASE: SSC
 *--------------------------------------------------------------------------*/

#define SSC_BASE                                                    0xe2000000
#define SSC_BASE_SIZE                                               0x00c00000
#define SSC_BASE_PHYS                                               0x02000000

/*----------------------------------------------------------------------------
 * BASE: TLMM_WEST
 *--------------------------------------------------------------------------*/

#define TLMM_WEST_BASE                                              0xe3100000
#define TLMM_WEST_BASE_SIZE                                         0x00300000
#define TLMM_WEST_BASE_PHYS                                         0x03100000

/*----------------------------------------------------------------------------
 * BASE: TLMM_EAST
 *--------------------------------------------------------------------------*/

#define TLMM_EAST_BASE                                              0xe3500000
#define TLMM_EAST_BASE_SIZE                                         0x00300000
#define TLMM_EAST_BASE_PHYS                                         0x03500000

/*----------------------------------------------------------------------------
 * BASE: TLMM_SOUTH
 *--------------------------------------------------------------------------*/

#define TLMM_SOUTH_BASE                                             0xe3900000
#define TLMM_SOUTH_BASE_SIZE                                        0x00300000
#define TLMM_SOUTH_BASE_PHYS                                        0x03d00000

/*----------------------------------------------------------------------------
 * BASE: QDSS_SOC_DBG
 *--------------------------------------------------------------------------*/

#define QDSS_SOC_DBG_BASE                                           0xe4000000
#define QDSS_SOC_DBG_BASE_SIZE                                      0x02000000
#define QDSS_SOC_DBG_BASE_PHYS                                      0x06000000

/*----------------------------------------------------------------------------
 * BASE: TURING
 *--------------------------------------------------------------------------*/

#define TURING_BASE                                                 0xe6000000
#define TURING_BASE_SIZE                                            0x003c1000
#define TURING_BASE_PHYS                                            0x08000000

/*----------------------------------------------------------------------------
 * BASE: PERIPH_SS_SDC2_SDCC5_TOP
 *--------------------------------------------------------------------------*/

#define PERIPH_SS_SDC2_SDCC5_TOP_BASE                               0xe6400000
#define PERIPH_SS_SDC2_SDCC5_TOP_BASE_SIZE                          0x00020000
#define PERIPH_SS_SDC2_SDCC5_TOP_BASE_PHYS                          0x08800000

/*----------------------------------------------------------------------------
 * BASE: PERIPH_SS_SDC4_SDCC5_TOP
 *--------------------------------------------------------------------------*/

#define PERIPH_SS_SDC4_SDCC5_TOP_BASE                               0xe6540000
#define PERIPH_SS_SDC4_SDCC5_TOP_BASE_SIZE                          0x00020000
#define PERIPH_SS_SDC4_SDCC5_TOP_BASE_PHYS                          0x08840000

/*----------------------------------------------------------------------------
 * BASE: PERIPH_SS_TSIF_TSIF_12SEG_WRAPPER
 *--------------------------------------------------------------------------*/

#define PERIPH_SS_TSIF_TSIF_12SEG_WRAPPER_BASE                      0xe6680000
#define PERIPH_SS_TSIF_TSIF_12SEG_WRAPPER_BASE_SIZE                 0x00040000
#define PERIPH_SS_TSIF_TSIF_12SEG_WRAPPER_BASE_PHYS                 0x08880000

/*----------------------------------------------------------------------------
 * BASE: PERIPH_SS_PDM_PERPH_WEB
 *--------------------------------------------------------------------------*/

#define PERIPH_SS_PDM_PERPH_WEB_BASE                                0xe67d0000
#define PERIPH_SS_PDM_PERPH_WEB_BASE_SIZE                           0x00004000
#define PERIPH_SS_PDM_PERPH_WEB_BASE_PHYS                           0x088d0000

/*----------------------------------------------------------------------------
 * BASE: DDR_SS
 *--------------------------------------------------------------------------*/

#define DDR_SS_BASE                                                 0xe7000000
#define DDR_SS_BASE_SIZE                                            0x00800000
#define DDR_SS_BASE_PHYS                                            0x09000000

/*----------------------------------------------------------------------------
 * BASE: USB3_PRI_USB30_PRIM
 *--------------------------------------------------------------------------*/

#define USB3_PRI_USB30_PRIM_BASE                                    0xe8200000
#define USB3_PRI_USB30_PRIM_BASE_SIZE                               0x00200000
#define USB3_PRI_USB30_PRIM_BASE_PHYS                               0x0a600000

/*----------------------------------------------------------------------------
 * BASE: AOSS
 *--------------------------------------------------------------------------*/

#define AOSS_BASE                                                   0xe9000000
#define AOSS_BASE_SIZE                                              0x04000000
#define AOSS_BASE_PHYS                                              0x0b000000

/*----------------------------------------------------------------------------
 * BASE: AOP_SS_MSG_RAM_START_ADDRESS
 *--------------------------------------------------------------------------*/

#define AOP_SS_MSG_RAM_START_ADDRESS_BASE                           0xed000000
#define AOP_SS_MSG_RAM_START_ADDRESS_BASE_SIZE                      0x00100000
#define AOP_SS_MSG_RAM_START_ADDRESS_BASE_PHYS                      0x0c300000

/*----------------------------------------------------------------------------
 * BASE: LPASS
 *--------------------------------------------------------------------------*/

#define LPASS_BASE                                                  0xee000000
#define LPASS_BASE_SIZE                                             0x003b1000
#define LPASS_BASE_PHYS                                             0x17000000


#endif /* __MSMHWIOBASE_H__ */
