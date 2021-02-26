#ifndef __HAL_CLK_TEST_H__
#define __HAL_CLK_TEST_H__


/*
==============================================================================

FILE:         HALclkTest.h

DESCRIPTION:
  This file contains the ids for configuring the debug clock circuitry
  for the clock HAL.


==============================================================================

$Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Library/ClockTargetLib/HALclkTest.h#1 $

==============================================================================
            Copyright (c) 2016 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
==============================================================================

*/

/*============================================================================

             DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/


/* ============================================================================
**    Defines
** ==========================================================================*/

/*
 * Masks used during test clock frequency calculation setup.
 */
#define HAL_CLK_TEST_TYPE_MASK    (0xF0000)
#define HAL_CLK_TEST_SEL_MASK     (0x07FFF)


/*
 * GCC debug clocks
 */
#define HAL_CLK_GCC_TEST                                              (1  << 0x10)


#define HAL_CLK_GCC_TEST_GCC_SYS_NOC_GC_AXI_CLK                            (HAL_CLK_GCC_TEST | 6    )
#define HAL_CLK_GCC_TEST_GCC_SYS_NOC_AXI_CLK                               (HAL_CLK_GCC_TEST | 7    )
#define HAL_CLK_GCC_TEST_GCC_SYS_NOC_SOUTH_AXI_CLK                         (HAL_CLK_GCC_TEST | 8    )
#define HAL_CLK_GCC_TEST_GCC_SYS_NOC_NORTH_AXI_CLK                         (HAL_CLK_GCC_TEST | 9    )
#define HAL_CLK_GCC_TEST_GCC_SYS_NOC_EAST_AXI_CLK						   (HAL_CLK_GCC_TEST | 10   )
#define HAL_CLK_GCC_TEST_GCC_SYS_NOC_QDSS_STM_AXI_CLK                      (HAL_CLK_GCC_TEST | 11   )
#define HAL_CLK_GCC_TEST_GCC_SYS_NOC_CPUSS_AHB_CLK                         (HAL_CLK_GCC_TEST | 12   )
#define HAL_CLK_GCC_TEST_GCC_SYS_NOC_AHB_CFG_CLK                           (HAL_CLK_GCC_TEST | 13   )
#define HAL_CLK_GCC_TEST_GCC_SYS_NOC_AT_CLK                                (HAL_CLK_GCC_TEST | 14   )
#define HAL_CLK_GCC_TEST_GCC_SYS_NOC_MONAQ_AXI_CLK                         (HAL_CLK_GCC_TEST | 15   )
#define HAL_CLK_GCC_TEST_GCC_SNOC_QOSGEN_EXTREF_CLK                        (HAL_CLK_GCC_TEST | 16   )
#define HAL_CLK_GCC_TEST_GCC_SYS_NOC_SF_AXI_CLK                            (HAL_CLK_GCC_TEST | 17   )
#define HAL_CLK_GCC_TEST_GCC_CNOC_PERIPH_SOUTH_CLK                         (HAL_CLK_GCC_TEST | 18   )
#define HAL_CLK_GCC_TEST_GCC_CNOC_PERIPH_NORTH_CLK                         (HAL_CLK_GCC_TEST | 19   )
#define HAL_CLK_GCC_TEST_GCC_CNOC_PERIPH_EAST_CLK                          (HAL_CLK_GCC_TEST | 20   )
#define HAL_CLK_GCC_TEST_GCC_CFG_NOC_AHB_CLK                               (HAL_CLK_GCC_TEST | 21   )
#define HAL_CLK_GCC_TEST_GCC_CFG_NOC_WEST_AHB_CLK                          (HAL_CLK_GCC_TEST | 22   )
#define HAL_CLK_GCC_TEST_GCC_CFG_NOC_NORTH_AHB_CLK                         (HAL_CLK_GCC_TEST | 23   )
#define HAL_CLK_GCC_TEST_GCC_CFG_NOC_EAST_AHB_CLK                          (HAL_CLK_GCC_TEST | 24   )
#define HAL_CLK_GCC_TEST_GCC_CFG_NOC_SOUTH_AHB_CLK                         (HAL_CLK_GCC_TEST | 25   )
#define HAL_CLK_GCC_TEST_GCC_CFG_NOC_MONAQ_AHB_CLK                         (HAL_CLK_GCC_TEST | 26   )
#define HAL_CLK_GCC_TEST_GCC_CFG_NOC_MMNOC_AHB_CLK                         (HAL_CLK_GCC_TEST | 27   )
#define HAL_CLK_GCC_TEST_GCC_CFG_NOC_TIC_CLK                               (HAL_CLK_GCC_TEST | 28   )
#define HAL_CLK_GCC_TEST_GCC_CFG_NOC_USB3_PRIM_AXI_CLK                     (HAL_CLK_GCC_TEST | 29   )
#define HAL_CLK_GCC_TEST_GCC_NOC_WEST_DCD_XO_CLK                           (HAL_CLK_GCC_TEST | 31   )
#define HAL_CLK_GCC_TEST_GCC_NOC_NORTH_DCD_XO_CLK                          (HAL_CLK_GCC_TEST | 32   )
#define HAL_CLK_GCC_TEST_GCC_NOC_SOUTH_DCD_XO_CLK                          (HAL_CLK_GCC_TEST | 33   )
#define HAL_CLK_GCC_TEST_GCC_NOC_EAST_DCD_XO_CLK                           (HAL_CLK_GCC_TEST | 34   )
#define HAL_CLK_GCC_TEST_GCC_NOC_CENTER_DCD_XO_CLK                         (HAL_CLK_GCC_TEST | 35   )
#define HAL_CLK_GCC_TEST_GCC_NOC_MONAQ_DCD_XO_CLK                          (HAL_CLK_GCC_TEST | 36   )
#define HAL_CLK_GCC_TEST_GCC_CFG_NOC_AH2PHY_XO_CLK                         (HAL_CLK_GCC_TEST | 37   )
#define HAL_CLK_GCC_TEST_GCC_TIC_CLK                                       (HAL_CLK_GCC_TEST | 38   )
#define HAL_CLK_GCC_TEST_GCC_TIC_CFG_AHB_CLK                               (HAL_CLK_GCC_TEST | 39   )
#define HAL_CLK_GCC_TEST_GCC_IMEM_AXI_CLK                                  (HAL_CLK_GCC_TEST | 40   )
#define HAL_CLK_GCC_TEST_GCC_IMEM_CFG_AHB_CLK                              (HAL_CLK_GCC_TEST | 41   )
#define HAL_CLK_GCC_TEST_GCC_SYS_NOC_SF_TCU_CLK                            (HAL_CLK_GCC_TEST | 42   )
#define HAL_CLK_GCC_TEST_GCC_MMU_TCU_CLK                                   (HAL_CLK_GCC_TEST | 43   )
#define HAL_CLK_GCC_TEST_GCC_AGGRE_NOC_AUDIO_TBU_CLK                       (HAL_CLK_GCC_TEST | 44   )
#define HAL_CLK_GCC_TEST_GCC_AGGRE_NOC_TBU1_CLK                            (HAL_CLK_GCC_TEST | 46   )
#define HAL_CLK_GCC_TEST_GCC_AGGRE_NOC_TBU2_CLK                            (HAL_CLK_GCC_TEST | 47   )
#define HAL_CLK_GCC_TEST_GCC_MMNOC_TBU_SF_CLK                              (HAL_CLK_GCC_TEST | 48   )
#define HAL_CLK_GCC_TEST_GCC_MMNOC_TBU_HF0_CLK                             (HAL_CLK_GCC_TEST | 49   )
#define HAL_CLK_GCC_TEST_GCC_MMNOC_TBU_HF1_CLK                             (HAL_CLK_GCC_TEST | 50   )
#define HAL_CLK_GCC_TEST_GCC_MMNOC_AT_CLK                                  (HAL_CLK_GCC_TEST | 51   )
#define HAL_CLK_GCC_TEST_GCC_MMNOC_AHB_CFG_CLK                             (HAL_CLK_GCC_TEST | 52   )
#define HAL_CLK_GCC_TEST_GCC_NOC_MMNOC_DCD_XO_CLK                          (HAL_CLK_GCC_TEST | 53   )
#define HAL_CLK_GCC_TEST_GCC_MMNOC_HF_QX_CLK                               (HAL_CLK_GCC_TEST | 54   )
#define HAL_CLK_GCC_TEST_GCC_MMNOC_SF_QX_CLK                               (HAL_CLK_GCC_TEST | 55   )
#define HAL_CLK_GCC_TEST_GCC_MMNOC_QOSGEN_EXTREF_CLK                       (HAL_CLK_GCC_TEST | 56   )
#define HAL_CLK_GCC_TEST_GCC_VIDEO_AHB_CLK                                 (HAL_CLK_GCC_TEST | 57   )
#define HAL_CLK_GCC_TEST_GCC_CAMERA_AHB_CLK                                (HAL_CLK_GCC_TEST | 58   )
#define HAL_CLK_GCC_TEST_GCC_DISP_AHB_CLK                                  (HAL_CLK_GCC_TEST | 59   )
#define HAL_CLK_GCC_TEST_GCC_QMIP_VIDEO_AHB_CLK                            (HAL_CLK_GCC_TEST | 60   )
#define HAL_CLK_GCC_TEST_GCC_QMIP_CAMERA_AHB_CLK                           (HAL_CLK_GCC_TEST | 61   )
#define HAL_CLK_GCC_TEST_GCC_QMIP_DISP_AHB_CLK                             (HAL_CLK_GCC_TEST | 62   )
#define HAL_CLK_GCC_TEST_GCC_VIDEO_AXI_CLK                                 (HAL_CLK_GCC_TEST | 63   )
#define HAL_CLK_GCC_TEST_GCC_CAMERA_AXI_CLK                                (HAL_CLK_GCC_TEST | 64   )
#define HAL_CLK_GCC_TEST_GCC_DISP_AXI_CLK                                  (HAL_CLK_GCC_TEST | 65   )
#define HAL_CLK_GCC_TEST_GCC_VIDEO_XO_CLK                                  (HAL_CLK_GCC_TEST | 66   )
#define HAL_CLK_GCC_TEST_GCC_CAMERA_XO_CLK                                 (HAL_CLK_GCC_TEST | 67   )
#define HAL_CLK_GCC_TEST_GCC_DISP_XO_CLK                                   (HAL_CLK_GCC_TEST | 68   )
#define HAL_CLK_GCC_TEST_GCC_MMSS_AT_CLK                                   (HAL_CLK_GCC_TEST | 69   )
#define HAL_CLK_GCC_TEST_GCC_MMSS_QM_CORE_CLK                              (HAL_CLK_GCC_TEST | 73   )
#define HAL_CLK_GCC_TEST_GCC_MMSS_TRIG_CLK                                 (HAL_CLK_GCC_TEST | 74   )
#define HAL_CLK_GCC_TEST_GCC_MMSS_QM_AHB_CLK                               (HAL_CLK_GCC_TEST | 75   )
#define HAL_CLK_GCC_TEST_GCC_DISP_GPLL0_CLK_SRC                            (HAL_CLK_GCC_TEST | 76   )
#define HAL_CLK_GCC_TEST_GCC_DISP_GPLL0_DIV_CLK_SRC                        (HAL_CLK_GCC_TEST | 77   )
#define HAL_CLK_GCC_TEST_GCC_SYS_NOC_PIMEM_AXI_CLK                         (HAL_CLK_GCC_TEST | 78   )
#define HAL_CLK_GCC_TEST_GCC_PIMEM_AXI_CLK                                 (HAL_CLK_GCC_TEST | 79   )
#define HAL_CLK_GCC_TEST_GCC_PIMEM_AHB_CLK                                 (HAL_CLK_GCC_TEST | 80   )
#define HAL_CLK_GCC_TEST_GCC_QDSS_DAP_AHB_CLK                              (HAL_CLK_GCC_TEST | 81   )
#define HAL_CLK_GCC_TEST_GCC_QDSS_CFG_AHB_CLK                              (HAL_CLK_GCC_TEST | 82   )
#define HAL_CLK_GCC_TEST_GCC_QDSS_CENTER_AT_CLK                            (HAL_CLK_GCC_TEST | 83   )
#define HAL_CLK_GCC_TEST_GCC_SOUTH_AT_CLK                                  (HAL_CLK_GCC_TEST | 84   )
#define HAL_CLK_GCC_TEST_GCC_EAST_AT_CLK                                   (HAL_CLK_GCC_TEST | 85   )
#define HAL_CLK_GCC_TEST_GCC_NORTH_AT_CLK                                  (HAL_CLK_GCC_TEST | 86   )
#define HAL_CLK_GCC_TEST_GCC_QDSS_ETR_USB_CLK                              (HAL_CLK_GCC_TEST | 87   )
#define HAL_CLK_GCC_TEST_GCC_QDSS_STM_CLK                                  (HAL_CLK_GCC_TEST | 88   )
#define HAL_CLK_GCC_TEST_GCC_QDSS_TRACECLKIN_CLK                           (HAL_CLK_GCC_TEST | 89   )
#define HAL_CLK_GCC_TEST_GCC_QDSS_TSCTR_CLK                                (HAL_CLK_GCC_TEST | 90   )
#define HAL_CLK_GCC_TEST_GCC_QDSS_TRIG_CLK                                 (HAL_CLK_GCC_TEST | 91   )
#define HAL_CLK_GCC_TEST_GCC_QDSS_DAP_CLK                                  (HAL_CLK_GCC_TEST | 92   )
#define HAL_CLK_GCC_TEST_GCC_APB_CLK                                       (HAL_CLK_GCC_TEST | 93   )
#define HAL_CLK_GCC_TEST_GCC_QDSS_XO_CLK                                   (HAL_CLK_GCC_TEST | 94   )
#define HAL_CLK_GCC_TEST_GCC_USB30_PRIM_MASTER_CLK                         (HAL_CLK_GCC_TEST | 95   )
#define HAL_CLK_GCC_TEST_GCC_USB30_PRIM_SLEEP_CLK                          (HAL_CLK_GCC_TEST | 96   )
#define HAL_CLK_GCC_TEST_GCC_USB30_PRIM_MOCK_UTMI_CLK                      (HAL_CLK_GCC_TEST | 97   )
#define HAL_CLK_GCC_TEST_GCC_USB3_PRIM_PHY_AUX_CLK                         (HAL_CLK_GCC_TEST | 98   )
#define HAL_CLK_GCC_TEST_GCC_USB3_PRIM_PHY_COM_AUX_CLK                     (HAL_CLK_GCC_TEST | 99   )
#define HAL_CLK_GCC_TEST_GCC_USB3_PRIM_PHY_PIPE_CLK						   (HAL_CLK_GCC_TEST | 100  )
#define HAL_CLK_GCC_TEST_GCC_USB_PHY_CFG_AHB2PHY_CLK                       (HAL_CLK_GCC_TEST | 111  )
#define HAL_CLK_GCC_TEST_GCC_SDCC2_APPS_CLK                                (HAL_CLK_GCC_TEST | 112  )
#define HAL_CLK_GCC_TEST_GCC_SDCC2_AHB_CLK                                 (HAL_CLK_GCC_TEST | 113  )
#define HAL_CLK_GCC_TEST_GCC_SDCC4_APPS_CLK                                (HAL_CLK_GCC_TEST | 114  )
#define HAL_CLK_GCC_TEST_GCC_SDCC4_AHB_CLK                                 (HAL_CLK_GCC_TEST | 115  )
#define HAL_CLK_GCC_TEST_GCC_QUPV3_WRAP_0_M_AHB_CLK                        (HAL_CLK_GCC_TEST | 116  )
#define HAL_CLK_GCC_TEST_GCC_QUPV3_WRAP_0_S_AHB_CLK                        (HAL_CLK_GCC_TEST | 117  )
#define HAL_CLK_GCC_TEST_GCC_QUPV3_WRAP0_CORE_CLK                          (HAL_CLK_GCC_TEST | 118  )
#define HAL_CLK_GCC_TEST_GCC_QUPV3_WRAP0_CORE_2X_CLK                       (HAL_CLK_GCC_TEST | 119  )
#define HAL_CLK_GCC_TEST_GCC_QUPV3_WRAP0_S0_CLK                            (HAL_CLK_GCC_TEST | 120  )
#define HAL_CLK_GCC_TEST_GCC_QUPV3_WRAP0_S1_CLK                            (HAL_CLK_GCC_TEST | 121  )
#define HAL_CLK_GCC_TEST_GCC_QUPV3_WRAP0_S2_CLK                            (HAL_CLK_GCC_TEST | 122  )
#define HAL_CLK_GCC_TEST_GCC_QUPV3_WRAP0_S3_CLK                            (HAL_CLK_GCC_TEST | 123  )
#define HAL_CLK_GCC_TEST_GCC_QUPV3_WRAP0_S4_CLK                            (HAL_CLK_GCC_TEST | 124  )
#define HAL_CLK_GCC_TEST_GCC_QUPV3_WRAP0_S5_CLK                            (HAL_CLK_GCC_TEST | 125  )
#define HAL_CLK_GCC_TEST_GCC_QUPV3_WRAP0_S6_CLK                            (HAL_CLK_GCC_TEST | 126  )
#define HAL_CLK_GCC_TEST_GCC_QUPV3_WRAP0_S7_CLK                            (HAL_CLK_GCC_TEST | 127  )
#define HAL_CLK_GCC_TEST_GCC_QUPV3_WRAP1_CORE_2X_CLK                       (HAL_CLK_GCC_TEST | 128  )
#define HAL_CLK_GCC_TEST_GCC_QUPV3_WRAP1_CORE_CLK                          (HAL_CLK_GCC_TEST | 129  )
#define HAL_CLK_GCC_TEST_GCC_QUPV3_WRAP_1_M_AHB_CLK                        (HAL_CLK_GCC_TEST | 130  )
#define HAL_CLK_GCC_TEST_GCC_QUPV3_WRAP_1_S_AHB_CLK                        (HAL_CLK_GCC_TEST | 131  )
#define HAL_CLK_GCC_TEST_GCC_QUPV3_WRAP1_S0_CLK                            (HAL_CLK_GCC_TEST | 132  )
#define HAL_CLK_GCC_TEST_GCC_QUPV3_WRAP1_S1_CLK                            (HAL_CLK_GCC_TEST | 133  )
#define HAL_CLK_GCC_TEST_GCC_QUPV3_WRAP1_S2_CLK                            (HAL_CLK_GCC_TEST | 134  )
#define HAL_CLK_GCC_TEST_GCC_QUPV3_WRAP1_S3_CLK                            (HAL_CLK_GCC_TEST | 135  )
#define HAL_CLK_GCC_TEST_GCC_QUPV3_WRAP1_S4_CLK                            (HAL_CLK_GCC_TEST | 136  )
#define HAL_CLK_GCC_TEST_GCC_QUPV3_WRAP1_S5_CLK                            (HAL_CLK_GCC_TEST | 137  )
#define HAL_CLK_GCC_TEST_GCC_QUPV3_WRAP1_S6_CLK                            (HAL_CLK_GCC_TEST | 138  )
#define HAL_CLK_GCC_TEST_GCC_QUPV3_WRAP1_S7_CLK                            (HAL_CLK_GCC_TEST | 139  )
#define HAL_CLK_GCC_TEST_GCC_PDM_AHB_CLK                                   (HAL_CLK_GCC_TEST | 140  )
#define HAL_CLK_GCC_TEST_GCC_PDM_XO4_CLK                                   (HAL_CLK_GCC_TEST | 141  )
#define HAL_CLK_GCC_TEST_GCC_PDM2_CLK                                      (HAL_CLK_GCC_TEST | 142  )
#define HAL_CLK_GCC_TEST_GCC_PRNG_AHB_CLK                                  (HAL_CLK_GCC_TEST | 143  )
#define HAL_CLK_GCC_TEST_GCC_TSIF_AHB_CLK                                  (HAL_CLK_GCC_TEST | 144  )
#define HAL_CLK_GCC_TEST_GCC_TSIF_REF_CLK                                  (HAL_CLK_GCC_TEST | 145  )
#define HAL_CLK_GCC_TEST_GCC_TSIF_INACTIVITY_TIMERS_CLK                    (HAL_CLK_GCC_TEST | 146  )
#define HAL_CLK_GCC_TEST_GCC_TCSR_AHB_CLK                                  (HAL_CLK_GCC_TEST | 147  )
#define HAL_CLK_GCC_TEST_GCC_BOOT_ROM_AHB_CLK                              (HAL_CLK_GCC_TEST | 148  )
#define HAL_CLK_GCC_TEST_GCC_TLMM_NORTH_AHB_CLK                            (HAL_CLK_GCC_TEST | 149  )
#define HAL_CLK_GCC_TEST_GCC_TLMM_SOUTH_AHB_CLK                            (HAL_CLK_GCC_TEST | 150  )
#define HAL_CLK_GCC_TEST_GCC_TLMM_EAST_AHB_CLK                             (HAL_CLK_GCC_TEST | 151  )
#define HAL_CLK_GCC_TEST_GCC_TLMM_CLK                                      (HAL_CLK_GCC_TEST | 152  )
#define HAL_CLK_GCC_TEST_GCC_AOSS_CNOC_AHB_CLK                             (HAL_CLK_GCC_TEST | 153  )
#define HAL_CLK_GCC_TEST_GCC_AOSS_AT_CLK                                   (HAL_CLK_GCC_TEST | 154  )
#define HAL_CLK_GCC_TEST_GCC_SEC_CTRL_ACC_CLK                              (HAL_CLK_GCC_TEST | 156  )
#define HAL_CLK_GCC_TEST_GCC_SEC_CTRL_AHB_CLK                              (HAL_CLK_GCC_TEST | 157  )
#define HAL_CLK_GCC_TEST_GCC_SEC_CTRL_CLK                                  (HAL_CLK_GCC_TEST | 158  )
#define HAL_CLK_GCC_TEST_GCC_SEC_CTRL_SENSE_CLK                            (HAL_CLK_GCC_TEST | 159  )
#define HAL_CLK_GCC_TEST_GCC_SEC_CTRL_BOOT_ROM_PATCH_CLK                   (HAL_CLK_GCC_TEST | 160  )
#define HAL_CLK_GCC_TEST_GCC_SPDM_CFG_AHB_CLK                              (HAL_CLK_GCC_TEST | 161  )
#define HAL_CLK_GCC_TEST_GCC_SPDM_MSTR_AHB_CLK                             (HAL_CLK_GCC_TEST | 162  )
#define HAL_CLK_GCC_TEST_GCC_SPDM_FF_CLK                                   (HAL_CLK_GCC_TEST | 163  )
#define HAL_CLK_GCC_TEST_GCC_SPDM_MEMNOC_CY_CLK                            (HAL_CLK_GCC_TEST | 164  )
#define HAL_CLK_GCC_TEST_GCC_SPDM_SNOC_CY_CLK                              (HAL_CLK_GCC_TEST | 165  )
#define HAL_CLK_GCC_TEST_GCC_SPDM_PNOC_CY_CLK                              (HAL_CLK_GCC_TEST | 166  )
#define HAL_CLK_GCC_TEST_GCC_CE1_CLK                                       (HAL_CLK_GCC_TEST | 167  )
#define HAL_CLK_GCC_TEST_GCC_CE1_AXI_CLK                                   (HAL_CLK_GCC_TEST | 168  )
#define HAL_CLK_GCC_TEST_GCC_CE1_AHB_CLK                                   (HAL_CLK_GCC_TEST | 169  )
#define HAL_CLK_GCC_TEST_GCC_AHB_CLK                                       (HAL_CLK_GCC_TEST | 170  )
#define HAL_CLK_GCC_TEST_GCC_XO_CLK                                        (HAL_CLK_GCC_TEST | 171  )
#define HAL_CLK_GCC_TEST_GCC_XO_DIV4_CLK                                   (HAL_CLK_GCC_TEST | 172  )
#define HAL_CLK_GCC_TEST_GCC_SLEEP_CLK                                     (HAL_CLK_GCC_TEST | 173  )
#define HAL_CLK_GCC_TEST_GCC_DDRSS_MMNOC_SF_QX_CLK                         (HAL_CLK_GCC_TEST | 174  )
#define HAL_CLK_GCC_TEST_GCC_DDRSS_MMNOC_HF_QX_CLK                         (HAL_CLK_GCC_TEST | 175  )
#define HAL_CLK_GCC_TEST_GCC_DDRSS_TCU_CLK                                 (HAL_CLK_GCC_TEST | 176  )
#define HAL_CLK_GCC_TEST_GCC_DDRSS_SYS_NOC_GC_AXI_CLK                      (HAL_CLK_GCC_TEST | 177  )
#define HAL_CLK_GCC_TEST_GCC_DDRSS_SYS_NOC_SF_AXI_CLK                      (HAL_CLK_GCC_TEST | 178  )
#define HAL_CLK_GCC_TEST_GCC_DDRSS_SYS_NOC_SLAVE_AXI_CLK                   (HAL_CLK_GCC_TEST | 179  )
#define HAL_CLK_GCC_TEST_GCC_DDRSS_XO_CLK                                  (HAL_CLK_GCC_TEST | 180  )
#define HAL_CLK_GCC_TEST_GCC_DDRSS_CFG_AHB_CLK                             (HAL_CLK_GCC_TEST | 181  )
#define HAL_CLK_GCC_TEST_GCC_DDRSS_SLEEP_CLK                               (HAL_CLK_GCC_TEST | 182  )
#define HAL_CLK_GCC_TEST_GCC_MEMNOC_CLK                                    (HAL_CLK_GCC_TEST | 183  )
#define HAL_CLK_GCC_TEST_GCC_DDRSS_TURING_AXI_CLK                          (HAL_CLK_GCC_TEST | 184  )
#define HAL_CLK_GCC_TEST_GCC_DDRSS_MSS_Q6_AXI_CLK                          (HAL_CLK_GCC_TEST | 185  )
#define HAL_CLK_GCC_TEST_GCC_DDRSS_AT_CLK                                  (HAL_CLK_GCC_TEST | 186  )
#define HAL_CLK_GCC_TEST_GCC_DDRSS_GPU_AXI_CLK                             (HAL_CLK_GCC_TEST | 187  )
#define HAL_CLK_GCC_TEST_GCC_SHRM_CLK                                      (HAL_CLK_GCC_TEST | 188  )
#define HAL_CLK_GCC_TEST_GCC_DNOC_CFG_CLK                                  (HAL_CLK_GCC_TEST | 189  )
#define HAL_CLK_GCC_TEST_GCC_DDR_I_HCLK                                    (HAL_CLK_GCC_TEST | 190  )
#define HAL_CLK_GCC_TEST_GCC_DDRMC_CH0_CLK                                 (HAL_CLK_GCC_TEST | 191  )
#define HAL_CLK_GCC_TEST_GCC_DDRMC_CH1_CLK                                 (HAL_CLK_GCC_TEST | 192  )
#define HAL_CLK_GCC_TEST_GCC_SHRM_CFG_AHB_CLK                              (HAL_CLK_GCC_TEST | 193  )
#define HAL_CLK_GCC_TEST_GCC_LPASS_Q6_AXI_CLK                              (HAL_CLK_GCC_TEST | 195  )
#define HAL_CLK_GCC_TEST_GCC_LPASS_CORE_AXIM_CLK                           (HAL_CLK_GCC_TEST | 196  )
#define HAL_CLK_GCC_TEST_GCC_LPASS_SWAY_CLK                                (HAL_CLK_GCC_TEST | 197  )
#define HAL_CLK_GCC_TEST_GCC_TURING_TBU_CLK                                (HAL_CLK_GCC_TEST | 201  )
#define HAL_CLK_GCC_TEST_GCC_TURING_AXI_CLK                                (HAL_CLK_GCC_TEST | 202  )
#define HAL_CLK_GCC_TEST_GCC_TURING_CFG_AHB_CLK                            (HAL_CLK_GCC_TEST | 203  )
#define HAL_CLK_GCC_TEST_GCC_TURING_AT_CLK                                 (HAL_CLK_GCC_TEST | 204  )
#define HAL_CLK_GCC_TEST_GCC_CPUSS_AHB_CLK                                 (HAL_CLK_GCC_TEST | 206  )
#define HAL_CLK_GCC_TEST_GCC_CPUSS_GNOC_CLK                                (HAL_CLK_GCC_TEST | 207  )
#define HAL_CLK_GCC_TEST_GCC_CPUSS_RBCPR_CLK                               (HAL_CLK_GCC_TEST | 208  )
#define HAL_CLK_GCC_TEST_GCC_CPUSS_TRIG_CLK                                (HAL_CLK_GCC_TEST | 209  )
#define HAL_CLK_GCC_TEST_GCC_CPUSS_AT_CLK                                  (HAL_CLK_GCC_TEST | 210  )
#define HAL_CLK_GCC_TEST_GCC_CPUSS_DVM_BUS_CLK                             (HAL_CLK_GCC_TEST | 211  )
#define HAL_CLK_GCC_TEST_GCC_APSS_QDSS_TSCTR_CLK                           (HAL_CLK_GCC_TEST | 212  )
#define HAL_CLK_GCC_TEST_GCC_APSS_QDSS_APB_CLK                             (HAL_CLK_GCC_TEST | 213  )
#define HAL_CLK_GCC_TEST_GCC_NOC_BUS_TIMEOUT_EXTREF_CLK                    (HAL_CLK_GCC_TEST | 215  )
#define HAL_CLK_GCC_TEST_GCC_RBCPR_CX_CLK                                  (HAL_CLK_GCC_TEST | 216  )
#define HAL_CLK_GCC_TEST_GCC_RBCPR_CX_AHB_CLK                              (HAL_CLK_GCC_TEST | 217  )
#define HAL_CLK_GCC_TEST_GCC_RBCPR_MX_CLK                                  (HAL_CLK_GCC_TEST | 218  )
#define HAL_CLK_GCC_TEST_GCC_RBCPR_MX_AHB_CLK                              (HAL_CLK_GCC_TEST | 219  )
#define HAL_CLK_GCC_TEST_GCC_GP1_CLK                                       (HAL_CLK_GCC_TEST | 222  )
#define HAL_CLK_GCC_TEST_GCC_GP2_CLK                                       (HAL_CLK_GCC_TEST | 223  )
#define HAL_CLK_GCC_TEST_GCC_GP3_CLK                                       (HAL_CLK_GCC_TEST | 224  )
#define HAL_CLK_GCC_TEST_GCC_UFS_PHY_AXI_CLK                               (HAL_CLK_GCC_TEST | 251  )
#define HAL_CLK_GCC_TEST_GCC_UFS_PHY_AHB_CLK                               (HAL_CLK_GCC_TEST | 252  )
#define HAL_CLK_GCC_TEST_GCC_UFS_PHY_TX_SYMBOL_0_CLK                       (HAL_CLK_GCC_TEST | 253  )
#define HAL_CLK_GCC_TEST_GCC_UFS_PHY_RX_SYMBOL_0_CLK                       (HAL_CLK_GCC_TEST | 254  )
#define HAL_CLK_GCC_TEST_GCC_UFS_PHY_UNIPRO_CORE_CLK                       (HAL_CLK_GCC_TEST | 257  )
#define HAL_CLK_GCC_TEST_GCC_UFS_PHY_ICE_CORE_CLK                          (HAL_CLK_GCC_TEST | 258  )
#define HAL_CLK_GCC_TEST_GCC_UFS_PHY_PHY_AUX_CLK                           (HAL_CLK_GCC_TEST | 259  )
#define HAL_CLK_GCC_TEST_GCC_SSC_CNOC_MPU_CLK                              (HAL_CLK_GCC_TEST | 265  )
#define HAL_CLK_GCC_TEST_GCC_SSC_XO_CLK                                    (HAL_CLK_GCC_TEST | 266  )
#define HAL_CLK_GCC_TEST_GCC_VDDCX_VS_CLK                                  (HAL_CLK_GCC_TEST | 268  )
#define HAL_CLK_GCC_TEST_GCC_VDDMX_VS_CLK                                  (HAL_CLK_GCC_TEST | 269  )
#define HAL_CLK_GCC_TEST_GCC_VDDA_VS_CLK                                   (HAL_CLK_GCC_TEST | 270  )
#define HAL_CLK_GCC_TEST_GCC_VS_CTRL_CLK                                   (HAL_CLK_GCC_TEST | 271  )
#define HAL_CLK_GCC_TEST_GCC_VS_CTRL_AHB_CLK                               (HAL_CLK_GCC_TEST | 272  )
#define HAL_CLK_GCC_TEST_GCC_MSS_VS_CLK                                    (HAL_CLK_GCC_TEST | 273  )
#define HAL_CLK_GCC_TEST_GCC_GPU_VS_CLK                                    (HAL_CLK_GCC_TEST | 274  )
#define HAL_CLK_GCC_TEST_GCC_APC_VS_CLK                                    (HAL_CLK_GCC_TEST | 275  )
#define HAL_CLK_GCC_TEST_GCC_AGGRE_NOC_SOUTH_AHB_CFG_CLK                   (HAL_CLK_GCC_TEST | 277  )
#define HAL_CLK_GCC_TEST_GCC_AGGRE_NOC_AHB_CLK                             (HAL_CLK_GCC_TEST | 278  )
#define HAL_CLK_GCC_TEST_GCC_AGGRE_CNOC_PERIPH_AHB_CLK                     (HAL_CLK_GCC_TEST | 279  )
#define HAL_CLK_GCC_TEST_GCC_AGGRE_NOC_SF_EAST_AXI_CLK                     (HAL_CLK_GCC_TEST | 280  )
#define HAL_CLK_GCC_TEST_GCC_AGGRE_NOC_SF_NORTH_AXI_CLK                    (HAL_CLK_GCC_TEST | 281  )
#define HAL_CLK_GCC_TEST_GCC_AGGRE_NOC_SF_CENTER_AXI_CLK                   (HAL_CLK_GCC_TEST | 282  )
#define HAL_CLK_GCC_TEST_GCC_AGGRE_USB3_PRIM_AXI_CLK                       (HAL_CLK_GCC_TEST | 283  )
#define HAL_CLK_GCC_TEST_GCC_AGGRE_UFS_PHY_AXI_CLK                         (HAL_CLK_GCC_TEST | 285  )
#define HAL_CLK_GCC_TEST_GCC_AGGRE_NOC_QOSGEN_EXTREF_CLK                   (HAL_CLK_GCC_TEST | 287  )
#define HAL_CLK_GCC_TEST_GCC_AGGRE_NOC_CENTER_AXI_CLK                      (HAL_CLK_GCC_TEST | 288  )
#define HAL_CLK_GCC_TEST_GCC_AGGRE_NOC_WEST_AXI_CLK                        (HAL_CLK_GCC_TEST | 289  )
#define HAL_CLK_GCC_TEST_GCC_AGGRE_NOC_SOUTH_AXI_CLK                       (HAL_CLK_GCC_TEST | 290  )
#define HAL_CLK_GCC_TEST_GCC_AGGRE_NOC_EAST_AXI_CLK                        (HAL_CLK_GCC_TEST | 291  )
#define HAL_CLK_GCC_TEST_GCC_AGGRE_NOC_NORTH_AXI_CLK                       (HAL_CLK_GCC_TEST | 292  )
#define HAL_CLK_GCC_TEST_GCC_AGGRE_NOC_MONAQ_AXI_CLK                       (HAL_CLK_GCC_TEST | 293  )
#define HAL_CLK_GCC_TEST_GCC_AGGRE_NOC_IPA_CLK                             (HAL_CLK_GCC_TEST | 294  )
#define HAL_CLK_GCC_TEST_GCC_DCC_AHB_CLK                                   (HAL_CLK_GCC_TEST | 295  )
#define HAL_CLK_GCC_TEST_GCC_IPA_2X_CLK                                    (HAL_CLK_GCC_TEST | 296  )
#define HAL_CLK_GCC_TEST_GCC_IPA_CLK                                       (HAL_CLK_GCC_TEST | 297  )
#define HAL_CLK_GCC_TEST_GCC_IPA_AHB_CLK                                   (HAL_CLK_GCC_TEST | 298  )
#define HAL_CLK_GCC_TEST_GCC_IPA_SLEEP_CLK                                 (HAL_CLK_GCC_TEST | 299  )
#define HAL_CLK_GCC_TEST_GCC_IPA_APB_CLK                                   (HAL_CLK_GCC_TEST | 300  )
#define HAL_CLK_GCC_TEST_GCC_MSS_CFG_AHB_CLK                               (HAL_CLK_GCC_TEST | 301  )
#define HAL_CLK_GCC_TEST_GCC_MSS_MFAB_AXIS_CLK                             (HAL_CLK_GCC_TEST | 302  )
#define HAL_CLK_GCC_TEST_GCC_MSS_AXIS2_CLK                                 (HAL_CLK_GCC_TEST | 303  )
#define HAL_CLK_GCC_TEST_GCC_MSS_NAV_AXI_CLK                               (HAL_CLK_GCC_TEST | 304  )
#define HAL_CLK_GCC_TEST_GCC_MSS_TRIG_CLK                                  (HAL_CLK_GCC_TEST | 305  )
#define HAL_CLK_GCC_TEST_GCC_MSS_AT_CLK                                    (HAL_CLK_GCC_TEST | 306  )
#define HAL_CLK_GCC_TEST_GCC_MSS_GPLL0_DIV_CLK_SRC                         (HAL_CLK_GCC_TEST | 307  )
#define HAL_CLK_GCC_TEST_GCC_MSS_SNOC_AXI_CLK                              (HAL_CLK_GCC_TEST | 308  )
#define HAL_CLK_GCC_TEST_GCC_MSS_Q6_MEMNOC_AXI_CLK                         (HAL_CLK_GCC_TEST | 309  )
#define HAL_CLK_GCC_TEST_GCC_GLM_AHB_CLK                                   (HAL_CLK_GCC_TEST | 311  )
#define HAL_CLK_GCC_TEST_GCC_GLM_CLK                                       (HAL_CLK_GCC_TEST | 312  )
#define HAL_CLK_GCC_TEST_GCC_GLM_XO_CLK                                    (HAL_CLK_GCC_TEST | 313  )
#define HAL_CLK_GCC_TEST_GCC_QREFS_VBG_CAL_CLK                             (HAL_CLK_GCC_TEST | 314  )
#define HAL_CLK_GCC_TEST_GCC_WCSS_AHB_S0_CLK                               (HAL_CLK_GCC_TEST | 315  )
#define HAL_CLK_GCC_TEST_GCC_WCSS_AXI_M_CLK                                (HAL_CLK_GCC_TEST | 316  )
#define HAL_CLK_GCC_TEST_GCC_WCSS_ECAHB_CLK                                (HAL_CLK_GCC_TEST | 317  )
#define HAL_CLK_GCC_TEST_GCC_WCSS_SHDREG_AHB_CLK                           (HAL_CLK_GCC_TEST | 318  )
#define HAL_CLK_GCC_TEST_GCC_WCSS_AT_CLK                                   (HAL_CLK_GCC_TEST | 319  )
#define HAL_CLK_GCC_TEST_GCC_WCSS_APB_CLK                                  (HAL_CLK_GCC_TEST | 320  )
#define HAL_CLK_GCC_TEST_GCC_GPU_CFG_AHB_CLK                               (HAL_CLK_GCC_TEST | 322  )
#define HAL_CLK_GCC_TEST_GCC_GPU_AT_CLK                                    (HAL_CLK_GCC_TEST | 323  )
#define HAL_CLK_GCC_TEST_GCC_GPU_MEMNOC_GFX_CLK                            (HAL_CLK_GCC_TEST | 325  )
#define HAL_CLK_GCC_TEST_GCC_GPU_TRIG_CLK                                  (HAL_CLK_GCC_TEST | 326  )
#define HAL_CLK_GCC_TEST_GCC_GPU_SNOC_DVM_GFX_CLK                          (HAL_CLK_GCC_TEST | 327  )
#define HAL_CLK_GCC_TEST_GCC_GPU_GPLL0_CLK_SRC                             (HAL_CLK_GCC_TEST | 328  )
#define HAL_CLK_GCC_TEST_GCC_GPU_GPLL0_DIV_CLK_SRC                         (HAL_CLK_GCC_TEST | 329  )
#define HAL_CLK_GCC_TEST_GCC_AHB2PHY_SOUTH_CLK                             (HAL_CLK_GCC_TEST | 342  )
#define HAL_CLK_GCC_TEST_GCC_CM_PHY_REFGEN1_CLK                            (HAL_CLK_GCC_TEST | 343  )
#define HAL_CLK_GCC_TEST_GCC_CM_PHY_REFGEN2_CLK                            (HAL_CLK_GCC_TEST | 344  )
#define HAL_CLK_GCC_TEST_GCC_TLMM_WEST_AHB_CLK                             (HAL_CLK_GCC_TEST | 345  )
#define HAL_CLK_GCC_TEST_GCC_TURING_TRIG_CLK                               (HAL_CLK_GCC_TEST | 346  )
#define HAL_CLK_GCC_TEST_GCC_SDCC1_APPS_CLK                                (HAL_CLK_GCC_TEST | 347  )
#define HAL_CLK_GCC_TEST_GCC_SDCC1_AHB_CLK                                 (HAL_CLK_GCC_TEST | 348  )
#define HAL_CLK_GCC_TEST_GCC_SDCC1_ICE_CORE_CLK                            (HAL_CLK_GCC_TEST | 349  )
#define HAL_CLK_GCC_TEST_GCC_AGGRE_CNOC_PERIPH_EAST_CLK                    (HAL_CLK_GCC_TEST | 350  )

/*
		Mux Select values for other subsystem clocks 
		
 BI_TCXO										   1
 CORE_PI_SLEEP_CLK                                 2
 TIC_CLK                                           3
 AUD_REF_CLK                                       4
 CORE_BI_PLL_TEST_SE                               5
 CAMERA_GCC_DEBUG_CLK                              70
 MDSS_GCC_DEBUG_CLK                                71
 VIDEO_GCC_DEBUG_CLK                               72
 USB3_PHY_WRAPPER_GCC_USB30_PIPE_CLK               107
 QUSB2PHY_SEC_GCC_USB30_UTMI_CLK                   109
 QUSB2PHY_PRIM_GCC_USB30_UTMI_CLK                  110
 AOSS_GCC_DEBUG_CLK                                155
 DDRSS_GCC_DEBUG_CLK                               194
 LPASS_GCC_DEBUG_CLK                               200
 TURING_GCC_DEBUG_CLK                              205
 CPUSS_GCC_DEBUG_CLK                               214
 QUSB2PHY_GCC_CLK_TEST_PRIM                        220
 UFS_PHY_TX_SYMBOL_0_CLK                           255
 UFS_PHY_RX_SYMBOL_0_CLK                           256
 SLEEP_CLK                                         276
 MSS_GCC_DEBUG_CLK                                 310
 WCSS_GCC_DEBUG_CLK                                321
 GPU_GCC_DEBUG_CLK                                 324
 NAV_GCC_DEBUG_CLK                                 341
*/

#endif // __HAL_CLK_TEST_H__