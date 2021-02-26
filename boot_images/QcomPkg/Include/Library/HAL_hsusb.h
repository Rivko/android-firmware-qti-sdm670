#ifndef HAL_HSUSB
#define HAL_HSUSB
/*=============================================================================

       Hardware Abstraction Layer High-Speed USB

=============================================================================*/
/**
@file HAL_hsusb.h

Hardware Abstraction Layer (HAL) external interface for the HS-USB core.

This file contains the definitions of the HS-USB identification,
capability, and operational registers, as well as mandatory APIs.

*/
/*=============================================================================
Copyright © 2008-2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary and Confidential.
=============================================================================*/

/*=============================================================================

EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when      who      what, where, why
--------  ---      ---------------------------------------------------------
08/17/11  sbryksin AHB/GEN_CONFIG settings changes for 8960
03/31/11  sniedzie Add AHB addresses.
06/24/10  lo       (Tech Pubs) Edited/added Doxygen comments and markup.
06/29/07  bt       Integrate MTP (IUSBDriver) feature into Main Line

=============================================================================*/


/* -----------------------------------------------------------------------
** Includes
** ----------------------------------------------------------------------- */
#include "com_dtypes.h"

/** @ingroup hsusb_qc_api
    @{
*/

/* -----------------------------------------------------------------------
**                           TYPES
** ----------------------------------------------------------------------- */


/*
* HAL_HSUSB_Regs
*/
/* Identification Registers */
#define HAL_HSUSB_ID_ADDR                             0x00000000
#define HAL_HSUSB_ID_REVISON_BMSK                       0xff0000
#define HAL_HSUSB_ID_REVISON_SHFT                           0x10
#define HAL_HSUSB_ID_NID_BMSK                             0x3f00
#define HAL_HSUSB_ID_NID_SHFT                                0x8
#define HAL_HSUSB_ID_ID_BMSK                                0x3f
#define HAL_HSUSB_ID_ID_SHFT                                   0

#define HAL_HSUSB_HWGENERAL_ADDR                      0x00000004
#define HAL_HSUSB_HWGENERAL_SM_BMSK                        0x200
#define HAL_HSUSB_HWGENERAL_SM_SHFT                          0x9
#define HAL_HSUSB_HWGENERAL_PHYM_BMSK                      0x1c0
#define HAL_HSUSB_HWGENERAL_PHYM_SHFT                        0x6
#define HAL_HSUSB_HWGENERAL_PHYW_BMSK                       0x30
#define HAL_HSUSB_HWGENERAL_PHYW_SHFT                        0x4
#define HAL_HSUSB_HWGENERAL_BWT_BMSK                         0x8
#define HAL_HSUSB_HWGENERAL_BWT_SHFT                         0x3
#define HAL_HSUSB_HWGENERAL_CLCK_BMSK                        0x6
#define HAL_HSUSB_HWGENERAL_CLCK_SHFT                        0x1
#define HAL_HSUSB_HWGENERAL_RT_BMSK                          0x1
#define HAL_HSUSB_HWGENERAL_RT_SHFT                            0

#define HAL_HSUSB_HWHOST_ADDR                         0x00000008
#define HAL_HSUSB_HWHOST_TTPER_BMSK                   0xff000000
#define HAL_HSUSB_HWHOST_TTPER_SHFT                         0x18
#define HAL_HSUSB_HWHOST_TTASY_BMSK                     0xff0000
#define HAL_HSUSB_HWHOST_TTASY_SHFT                         0x10
#define HAL_HSUSB_HWHOST_NPORT_BMSK                          0xe
#define HAL_HSUSB_HWHOST_NPORT_SHFT                          0x1
#define HAL_HSUSB_HWHOST_HC_BMSK                             0x1
#define HAL_HSUSB_HWHOST_HC_SHFT                               0

#define HAL_HSUSB_HWDEVICE_ADDR                       0x0000000c
#define HAL_HSUSB_HWDEVICE_DEVEP_BMSK                       0x3e
#define HAL_HSUSB_HWDEVICE_DEVEP_SHFT                        0x1
#define HAL_HSUSB_HWDEVICE_DC_BMSK                           0x1
#define HAL_HSUSB_HWDEVICE_DC_SHFT                             0

#define HAL_HSUSB_HWTXBUF_ADDR                        0x00000010
#define HAL_HSUSB_HWTXBUF_TXLCR_BMSK                  0x80000000
#define HAL_HSUSB_HWTXBUF_TXLCR_SHFT                        0x1f
#define HAL_HSUSB_HWTXBUF_TXCHANADD_BMSK                0xff0000
#define HAL_HSUSB_HWTXBUF_TXCHANADD_SHFT                    0x10
#define HAL_HSUSB_HWTXBUF_TXADD_BMSK                      0xff00
#define HAL_HSUSB_HWTXBUF_TXADD_SHFT                         0x8
#define HAL_HSUSB_HWTXBUF_TCBURST_BMSK                      0xff
#define HAL_HSUSB_HWTXBUF_TCBURST_SHFT                         0

#define HAL_HSUSB_HWRXBUF_ADDR                        0x00000014
#define HAL_HSUSB_HWRXBUF_RX_ADD_BMSK                     0xff00
#define HAL_HSUSB_HWRXBUF_RX_ADD_SHFT                        0x8
#define HAL_HSUSB_HWRXBUF_RX_BURST_BMSK                     0xff
#define HAL_HSUSB_HWRXBUF_RX_BURST_SHFT                        0

#define HAL_HSUSB_GPTIMER0LD_ADDR                     0x00000080
#define HAL_HSUSB_GPTIMER0LD_GPTLD_BMSK                 0xffffff
#define HAL_HSUSB_GPTIMER0LD_GPTLD_SHFT                        0

#define HAL_HSUSB_GPTIMER0CTRL_ADDR                   0x00000084
#define HAL_HSUSB_GPTIMER0CTRL_GTPRUN_BMSK            0x80000000
#define HAL_HSUSB_GPTIMER0CTRL_GTPRUN_SHFT                  0x1f
#define HAL_HSUSB_GPTIMER0CTRL_GPTRST_BMSK            0x40000000
#define HAL_HSUSB_GPTIMER0CTRL_GPTRST_SHFT                  0x1e
#define HAL_HSUSB_GPTIMER0CTRL_GPTMODE_BMSK            0x1000000
#define HAL_HSUSB_GPTIMER0CTRL_GPTMODE_SHFT                 0x18
#define HAL_HSUSB_GPTIMER0CTRL_GPTCNT_BMSK              0xffffff
#define HAL_HSUSB_GPTIMER0CTRL_GPTCNT_SHFT                     0

#define HAL_HSUSB_GPTIMER1LD_ADDR                     0x00000088
#define HAL_HSUSB_GPTIMER1LD_GPTLD_BMSK                 0xffffff
#define HAL_HSUSB_GPTIMER1LD_GPTLD_SHFT                        0

#define HAL_HSUSB_GPTIMER1CTRL_ADDR                   0x0000008c
#define HAL_HSUSB_GPTIMER1CTRL_GTPRUN_BMSK            0x80000000
#define HAL_HSUSB_GPTIMER1CTRL_GTPRUN_SHFT                  0x1f
#define HAL_HSUSB_GPTIMER1CTRL_GPTRST_BMSK            0x40000000
#define HAL_HSUSB_GPTIMER1CTRL_GPTRST_SHFT                  0x1e
#define HAL_HSUSB_GPTIMER1CTRL_GPTMODE_BMSK            0x1000000
#define HAL_HSUSB_GPTIMER1CTRL_GPTMODE_SHFT                 0x18
#define HAL_HSUSB_GPTIMER1CTRL_GPTCNT_BMSK              0xffffff
#define HAL_HSUSB_GPTIMER1CTRL_GPTCNT_SHFT                     0

#define HAL_HSUSB_AHB_BURST_ADDR                      0x00000090

#define HAL_HSUSB_AHB_MODE_ADDR                       0x00000098

#define HAL_HSUSB_GEN_CONFIG_ADDR                     0x0000009C

#define HAL_HSUSB_GEN_CONFIG2_ADDR                    0x000000A0

/* Capability Registers */
#define HAL_HSUSB_CAPLENGTH_ADDR                      0x00000100
#define HAL_HSUSB_CAPLENGTH_CAPLENGTH_BMSK                  0xff
#define HAL_HSUSB_CAPLENGTH_CAPLENGTH_SHFT                     0

#define HAL_HSUSB_HCIVERSION_ADDR                     0x00000102
#define HAL_HSUSB_HCIVERSION_HCIVERSION_BMSK              0xffff
#define HAL_HSUSB_HCIVERSION_HCIVERSION_SHFT                   0

#define HAL_HSUSB_HCSPARAMS_ADDR                      0x00000104
#define HAL_HSUSB_HCSPARAMS_N_TT_BMSK                  0xf000000
#define HAL_HSUSB_HCSPARAMS_N_TT_SHFT                       0x18
#define HAL_HSUSB_HCSPARAMS_N_PTT_BMSK                  0xf00000
#define HAL_HSUSB_HCSPARAMS_N_PTT_SHFT                      0x14
#define HAL_HSUSB_HCSPARAMS_PI_BMSK                      0x10000
#define HAL_HSUSB_HCSPARAMS_PI_SHFT                         0x10
#define HAL_HSUSB_HCSPARAMS_N_CC_BMSK                     0xf000
#define HAL_HSUSB_HCSPARAMS_N_CC_SHFT                        0xc
#define HAL_HSUSB_HCSPARAMS_N_PCC_BMSK                     0xf00
#define HAL_HSUSB_HCSPARAMS_N_PCC_SHFT                       0x8
#define HAL_HSUSB_HCSPARAMS_PPC_BMSK                        0x10
#define HAL_HSUSB_HCSPARAMS_PPC_SHFT                         0x4
#define HAL_HSUSB_HCSPARAMS_N_PORTS_BMSK                     0xf
#define HAL_HSUSB_HCSPARAMS_N_PORTS_SHFT                       0

#define HAL_HSUSB_HCCPARAMS_ADDR                      0x00000108
#define HAL_HSUSB_HCCPARAMS_EECP_BMSK                     0xff00
#define HAL_HSUSB_HCCPARAMS_EECP_SHFT                        0x8
#define HAL_HSUSB_HCCPARAMS_IST_BMSK                        0xf0
#define HAL_HSUSB_HCCPARAMS_IST_SHFT                         0x4
#define HAL_HSUSB_HCCPARAMS_ASP_BMSK                         0x4
#define HAL_HSUSB_HCCPARAMS_ASP_SHFT                         0x2
#define HAL_HSUSB_HCCPARAMS_PFL_BMSK                         0x2
#define HAL_HSUSB_HCCPARAMS_PFL_SHFT                         0x1
#define HAL_HSUSB_HCCPARAMS_ADC_BMSK                         0x1
#define HAL_HSUSB_HCCPARAMS_ADC_SHFT                           0

#define HAL_HSUSB_DCIVERSION_ADDR                     0x00000120
#define HAL_HSUSB_DCIVERSION_DCIVERSION_BMSK              0xffff
#define HAL_HSUSB_DCIVERSION_DCIVERSION_SHFT                   0

#define HAL_HSUSB_DCCPARAMS_ADDR                      0x00000124
#define HAL_HSUSB_DCCPARAMS_HC_BMSK                        0x100
#define HAL_HSUSB_DCCPARAMS_HC_SHFT                          0x8
#define HAL_HSUSB_DCCPARAMS_DC_BMSK                         0x80
#define HAL_HSUSB_DCCPARAMS_DC_SHFT                          0x7
#define HAL_HSUSB_DCCPARAMS_DEN_BMSK                        0x1f
#define HAL_HSUSB_DCCPARAMS_DEN_SHFT                           0

/* Operational Registers */
#define HAL_HSUSB_USBCMD_ADDR                         0x00000140
#define HAL_HSUSB_USBCMD_RST_CTRL_BMSK                0x80000000
#define HAL_HSUSB_USBCMD_RST_CTRL_SHFT                      0x1f
#define HAL_HSUSB_USBCMD_ULPI_STP_CTRL_BMSK           0x40000000
#define HAL_HSUSB_USBCMD_ULPI_STP_CTRL_SHFT                 0x1e
#define HAL_HSUSB_USBCMD_ASYNC_INTR_CTRL_BMSK         0x20000000
#define HAL_HSUSB_USBCMD_ASYNC_INTR_CTRL_SHFT               0x1d
#define HAL_HSUSB_USBCMD_ITC_BMSK                       0xff0000
#define HAL_HSUSB_USBCMD_ITC_SHFT                           0x10
#define HAL_HSUSB_USBCMD_FS2_BMSK                         0x8000
#define HAL_HSUSB_USBCMD_FS2_SHFT                            0xf
#define HAL_HSUSB_USBCMD_SUTW_BMSK                        0x2000
#define HAL_HSUSB_USBCMD_SUTW_SHFT                           0xd
#define HAL_HSUSB_USBCMD_ATDTW_BMSK                       0x1000
#define HAL_HSUSB_USBCMD_ATDTW_SHFT                          0xc
#define HAL_HSUSB_USBCMD_ASPE_BMSK                         0x800
#define HAL_HSUSB_USBCMD_ASPE_SHFT                           0xb
#define HAL_HSUSB_USBCMD_ASP_BMSK                          0x300
#define HAL_HSUSB_USBCMD_ASP_SHFT                            0x8
#define HAL_HSUSB_USBCMD_LR_BMSK                            0x80
#define HAL_HSUSB_USBCMD_LR_SHFT                             0x7
#define HAL_HSUSB_USBCMD_IAA_BMSK                           0x40
#define HAL_HSUSB_USBCMD_IAA_SHFT                            0x6
#define HAL_HSUSB_USBCMD_ASE_BMSK                           0x20
#define HAL_HSUSB_USBCMD_ASE_SHFT                            0x5
#define HAL_HSUSB_USBCMD_PSE_BMSK                           0x10
#define HAL_HSUSB_USBCMD_PSE_SHFT                            0x4
#define HAL_HSUSB_USBCMD_FS1_BMSK                            0x8
#define HAL_HSUSB_USBCMD_FS1_SHFT                            0x3
#define HAL_HSUSB_USBCMD_FS0_BMSK                            0x4
#define HAL_HSUSB_USBCMD_FS0_SHFT                            0x2
#define HAL_HSUSB_USBCMD_RST_BMSK                            0x2
#define HAL_HSUSB_USBCMD_RST_SHFT                            0x1
#define HAL_HSUSB_USBCMD_RS_BMSK                             0x1
#define HAL_HSUSB_USBCMD_RS_SHFT                               0

#define HAL_HSUSB_USBSTS_ADDR                         0x00000144
#define HAL_HSUSB_USBSTS_ULPI_INTR_BMSK               0x80000000
#define HAL_HSUSB_USBSTS_ULPI_INTR_SHFT                     0x1f
#define HAL_HSUSB_USBSTS_TI1_BMSK                      0x2000000
#define HAL_HSUSB_USBSTS_TI1_SHFT                           0x19
#define HAL_HSUSB_USBSTS_TI0_BMSK                      0x1000000
#define HAL_HSUSB_USBSTS_TI0_SHFT                           0x18
#define HAL_HSUSB_USBSTS_UPI_BMSK                        0x80000
#define HAL_HSUSB_USBSTS_UPI_SHFT                           0x13
#define HAL_HSUSB_USBSTS_UAI_BMSK                        0x40000
#define HAL_HSUSB_USBSTS_UAI_SHFT                           0x12
#define HAL_HSUSB_USBSTS_NAKI_BMSK                       0x10000
#define HAL_HSUSB_USBSTS_NAKI_SHFT                          0x10
#define HAL_HSUSB_USBSTS_AS_BMSK                          0x8000
#define HAL_HSUSB_USBSTS_AS_SHFT                             0xf
#define HAL_HSUSB_USBSTS_PS_BMSK                          0x4000
#define HAL_HSUSB_USBSTS_PS_SHFT                             0xe
#define HAL_HSUSB_USBSTS_RCL_BMSK                         0x2000
#define HAL_HSUSB_USBSTS_RCL_SHFT                            0xd
#define HAL_HSUSB_USBSTS_HCH_BMSK                         0x1000
#define HAL_HSUSB_USBSTS_HCH_SHFT                            0xc
#define HAL_HSUSB_USBSTS_ULPII_BMSK                        0x400
#define HAL_HSUSB_USBSTS_ULPII_SHFT                          0xa
#define HAL_HSUSB_USBSTS_SLI_BMSK                          0x100
#define HAL_HSUSB_USBSTS_SLI_SHFT                            0x8
#define HAL_HSUSB_USBSTS_SRI_BMSK                           0x80
#define HAL_HSUSB_USBSTS_SRI_SHFT                            0x7
#define HAL_HSUSB_USBSTS_URI_BMSK                           0x40
#define HAL_HSUSB_USBSTS_URI_SHFT                            0x6
#define HAL_HSUSB_USBSTS_AAI_BMSK                           0x20
#define HAL_HSUSB_USBSTS_AAI_SHFT                            0x5
#define HAL_HSUSB_USBSTS_SEI_BMSK                           0x10
#define HAL_HSUSB_USBSTS_SEI_SHFT                            0x4
#define HAL_HSUSB_USBSTS_FRI_BMSK                            0x8
#define HAL_HSUSB_USBSTS_FRI_SHFT                            0x3
#define HAL_HSUSB_USBSTS_PCI_BMSK                            0x4
#define HAL_HSUSB_USBSTS_PCI_SHFT                            0x2
#define HAL_HSUSB_USBSTS_UEI_BMSK                            0x2
#define HAL_HSUSB_USBSTS_UEI_SHFT                            0x1
#define HAL_HSUSB_USBSTS_UI_BMSK                             0x1
#define HAL_HSUSB_USBSTS_UI_SHFT                               0

#define HAL_HSUSB_USBINTR_ADDR                        0x00000148
#define HAL_HSUSB_USBINTR_ULPI_INTR_EN_BMSK           0x80000000
#define HAL_HSUSB_USBINTR_ULPI_INTR_EN_SHFT                 0x1f
#define HAL_HSUSB_USBINTR_TIE1_BMSK                    0x2000000
#define HAL_HSUSB_USBINTR_TIE1_SHFT                         0x19
#define HAL_HSUSB_USBINTR_TIE0_BMSK                    0x1000000
#define HAL_HSUSB_USBINTR_TIE0_SHFT                         0x18
#define HAL_HSUSB_USBINTR_UPIE_BMSK                      0x80000
#define HAL_HSUSB_USBINTR_UPIE_SHFT                         0x13
#define HAL_HSUSB_USBINTR_UAIE_BMSK                      0x40000
#define HAL_HSUSB_USBINTR_UAIE_SHFT                         0x12
#define HAL_HSUSB_USBINTR_NAKE_BMSK                      0x10000
#define HAL_HSUSB_USBINTR_NAKE_SHFT                         0x10
#define HAL_HSUSB_USBINTR_ULPIE_BMSK                       0x400
#define HAL_HSUSB_USBINTR_ULPIE_SHFT                         0xa
#define HAL_HSUSB_USBINTR_SLE_BMSK                         0x100
#define HAL_HSUSB_USBINTR_SLE_SHFT                           0x8
#define HAL_HSUSB_USBINTR_SRE_BMSK                          0x80
#define HAL_HSUSB_USBINTR_SRE_SHFT                           0x7
#define HAL_HSUSB_USBINTR_URE_BMSK                          0x40
#define HAL_HSUSB_USBINTR_URE_SHFT                           0x6
#define HAL_HSUSB_USBINTR_AAE_BMSK                          0x20
#define HAL_HSUSB_USBINTR_AAE_SHFT                           0x5
#define HAL_HSUSB_USBINTR_SEE_BMSK                          0x10
#define HAL_HSUSB_USBINTR_SEE_SHFT                           0x4
#define HAL_HSUSB_USBINTR_FRE_BMSK                           0x8
#define HAL_HSUSB_USBINTR_FRE_SHFT                           0x3
#define HAL_HSUSB_USBINTR_PCE_BMSK                           0x4
#define HAL_HSUSB_USBINTR_PCE_SHFT                           0x2
#define HAL_HSUSB_USBINTR_UEE_BMSK                           0x2
#define HAL_HSUSB_USBINTR_UEE_SHFT                           0x1
#define HAL_HSUSB_USBINTR_UE_BMSK                            0x1
#define HAL_HSUSB_USBINTR_UE_SHFT                              0

#define HAL_HSUSB_FRINDEX_ADDR                        0x0000014c
#define HAL_HSUSB_FRINDEX_FRINDEX_BMSK                    0x3fff
#define HAL_HSUSB_FRINDEX_FRINDEX_SHFT                         0

#define HAL_HSUSB_PERIODICLISTBASE_ADDR               0x00000154
#define HAL_HSUSB_PERIODICLISTBASE_PERBASE_BMSK       0xfffff000
#define HAL_HSUSB_PERIODICLISTBASE_PERBASE_SHFT              0xc

#define HAL_HSUSB_DEVICEADDR_ADDR                     0x00000154
#define HAL_HSUSB_DEVICEADDR_USBADR_BMSK              0xfe000000
#define HAL_HSUSB_DEVICEADDR_USBADR_SHFT                    0x19
#define HAL_HSUSB_DEVICEADDR_USBADRA_BMSK              0x1000000
#define HAL_HSUSB_DEVICEADDR_USBADRA_SHFT                   0x18

#define HAL_HSUSB_ASYNCLISTADDR_ADDR                  0x00000158
#define HAL_HSUSB_ASYNCLISTADDR_ASYBASE_BMSK          0xffffffe0
#define HAL_HSUSB_ASYNCLISTADDR_ASYBASE_SHFT                 0x5

#define HAL_HSUSB_ENDPOINTLISTADDR_ADDR               0x00000158
#define HAL_HSUSB_ENDPOINTLISTADDR_EPBASE_BMSK        0xfffff800
#define HAL_HSUSB_ENDPOINTLISTADDR_EPBASE_SHFT               0xb

#define HAL_HSUSB_TTCTRL_ADDR                         0x0000015c
#define HAL_HSUSB_TTCTRL_TTHA_BMSK                    0x7f000000
#define HAL_HSUSB_TTCTRL_TTHA_SHFT                          0x18

#define HAL_HSUSB_BURSTSIZE_ADDR                      0x00000160
#define HAL_HSUSB_BURSTSIZE_TXPBURST_BMSK                 0xff00
#define HAL_HSUSB_BURSTSIZE_TXPBURST_SHFT                    0x8
#define HAL_HSUSB_BURSTSIZE_RXPBURST_BMSK                   0xff
#define HAL_HSUSB_BURSTSIZE_RXPBURST_SHFT                      0

#define HAL_HSUSB_TXFILLTUNING_ADDR                   0x00000164
#define HAL_HSUSB_TXFILLTUNING_TXFIFOTHRES_BMSK         0x3f0000
#define HAL_HSUSB_TXFILLTUNING_TXFIFOTHRES_SHFT             0x10
#define HAL_HSUSB_TXFILLTUNING_TXSCHHEALTH_BMSK           0x1f00
#define HAL_HSUSB_TXFILLTUNING_TXSCHHEALTH_SHFT              0x8
#define HAL_HSUSB_TXFILLTUNING_TXSCHOH_BMSK                 0xff
#define HAL_HSUSB_TXFILLTUNING_TXSCHOH_SHFT                    0

#define HAL_HSUSB_ULPI_VIEWPORT_ADDR                  0x00000170
#define HAL_HSUSB_ULPI_VIEWPORT_ULPIWU_BMSK           0x80000000
#define HAL_HSUSB_ULPI_VIEWPORT_ULPIWU_SHFT                 0x1f
#define HAL_HSUSB_ULPI_VIEWPORT_ULPIRUN_BMSK          0x40000000
#define HAL_HSUSB_ULPI_VIEWPORT_ULPIRUN_SHFT                0x1e
#define HAL_HSUSB_ULPI_VIEWPORT_ULPIRW_BMSK           0x20000000
#define HAL_HSUSB_ULPI_VIEWPORT_ULPIRW_SHFT                 0x1d
#define HAL_HSUSB_ULPI_VIEWPORT_ULPISS_BMSK            0x8000000
#define HAL_HSUSB_ULPI_VIEWPORT_ULPISS_SHFT                 0x1b
#define HAL_HSUSB_ULPI_VIEWPORT_ULPIPORT_BMSK          0x7000000
#define HAL_HSUSB_ULPI_VIEWPORT_ULPIPORT_SHFT               0x18
#define HAL_HSUSB_ULPI_VIEWPORT_ULPIADDR_BMSK           0xff0000
#define HAL_HSUSB_ULPI_VIEWPORT_ULPIADDR_SHFT               0x10
#define HAL_HSUSB_ULPI_VIEWPORT_ULPIDATRD_BMSK            0xff00
#define HAL_HSUSB_ULPI_VIEWPORT_ULPIDATRD_SHFT               0x8
#define HAL_HSUSB_ULPI_VIEWPORT_ULPIDATWR_BMSK              0xff
#define HAL_HSUSB_ULPI_VIEWPORT_ULPIDATWR_SHFT                 0

#define HAL_HSUSB_ENDPTNAK_ADDR                       0x00000178
#define HAL_HSUSB_ENDPTNAK_EPTN_BMSK                  0xffff0000
#define HAL_HSUSB_ENDPTNAK_EPTN_SHFT                        0x10
#define HAL_HSUSB_ENDPTNAK_EPRN_BMSK                      0xffff
#define HAL_HSUSB_ENDPTNAK_EPRN_SHFT                           0

#define HAL_HSUSB_ENDPTNAKEN_ADDR                     0x0000017c
#define HAL_HSUSB_ENDPTNAKEN_EPTNE_BMSK               0xffff0000
#define HAL_HSUSB_ENDPTNAKEN_EPTNE_SHFT                     0x10
#define HAL_HSUSB_ENDPTNAKEN_EPRNE_BMSK                   0xffff
#define HAL_HSUSB_ENDPTNAKEN_EPRNE_SHFT                        0

#define HAL_HSUSB_PORTSC_ADDR(n)          (0x00000184 + 4 * (n))  /* 0<=n<=7 */
#define HAL_HSUSB_PORTSC_PTS_BMSK                     0xc0000000
#define HAL_HSUSB_PORTSC_PTS_SERIAL_BMSK              0xc0000000
#define HAL_HSUSB_PORTSC_PTS_ULPI_BMSK                0x80000000
#define HAL_HSUSB_PORTSC_PTS_SHFT                           0x1e
#define HAL_HSUSB_PORTSC_STS_BMSK                     0x20000000
#define HAL_HSUSB_PORTSC_STS_SHFT                           0x1d
#define HAL_HSUSB_PORTSC_PTW_BMSK                     0x10000000
#define HAL_HSUSB_PORTSC_PTW_SHFT                           0x1c
#define HAL_HSUSB_PORTSC_PSPD_BMSK                     0xc000000
#define HAL_HSUSB_PORTSC_PSPD_SHFT                          0x1a
#define HAL_HSUSB_PORTSC_PFSC_BMSK                     0x1000000
#define HAL_HSUSB_PORTSC_PFSC_SHFT                          0x18
#define HAL_HSUSB_PORTSC_PHCD_BMSK                      0x800000
#define HAL_HSUSB_PORTSC_PHCD_SHFT                          0x17
#define HAL_HSUSB_PORTSC_WKOC_BMSK                      0x400000
#define HAL_HSUSB_PORTSC_WKOC_SHFT                          0x16
#define HAL_HSUSB_PORTSC_WKDS_BMSK                      0x200000
#define HAL_HSUSB_PORTSC_WKDS_SHFT                          0x15
#define HAL_HSUSB_PORTSC_WKCN_BMSK                      0x100000
#define HAL_HSUSB_PORTSC_WKCN_SHFT                          0x14
#define HAL_HSUSB_PORTSC_PTC_BMSK                        0xf0000
#define HAL_HSUSB_PORTSC_PTC_SHFT                           0x10
#define HAL_HSUSB_PORTSC_PIC_BMSK                         0xc000
#define HAL_HSUSB_PORTSC_PIC_SHFT                            0xe
#define HAL_HSUSB_PORTSC_PO_BMSK                          0x2000
#define HAL_HSUSB_PORTSC_PO_SHFT                             0xd
#define HAL_HSUSB_PORTSC_PP_BMSK                          0x1000
#define HAL_HSUSB_PORTSC_PP_SHFT                             0xc
#define HAL_HSUSB_PORTSC_LS_BMSK                           0xc00
#define HAL_HSUSB_PORTSC_LS_SHFT                             0xa
#define HAL_HSUSB_PORTSC_HSP_BMSK                          0x200
#define HAL_HSUSB_PORTSC_HSP_SHFT                            0x9
#define HAL_HSUSB_PORTSC_PR_BMSK                           0x100
#define HAL_HSUSB_PORTSC_PR_SHFT                             0x8
#define HAL_HSUSB_PORTSC_SUSP_BMSK                          0x80
#define HAL_HSUSB_PORTSC_SUSP_SHFT                           0x7
#define HAL_HSUSB_PORTSC_FPR_BMSK                           0x40
#define HAL_HSUSB_PORTSC_FPR_SHFT                            0x6
#define HAL_HSUSB_PORTSC_OCC_BMSK                           0x20
#define HAL_HSUSB_PORTSC_OCC_SHFT                            0x5
#define HAL_HSUSB_PORTSC_OCA_BMSK                           0x10
#define HAL_HSUSB_PORTSC_OCA_SHFT                            0x4
#define HAL_HSUSB_PORTSC_PEC_BMSK                            0x8
#define HAL_HSUSB_PORTSC_PEC_SHFT                            0x3
#define HAL_HSUSB_PORTSC_PE_BMSK                             0x4
#define HAL_HSUSB_PORTSC_PE_SHFT                             0x2
#define HAL_HSUSB_PORTSC_CSC_BMSK                            0x2
#define HAL_HSUSB_PORTSC_CSC_SHFT                            0x1
#define HAL_HSUSB_PORTSC_CCS_BMSK                            0x1
#define HAL_HSUSB_PORTSC_CCS_SHFT                              0

#define HAL_HSUSB_OTGSC_ADDR                          0x000001a4
#define HAL_HSUSB_OTGSC_DPIE_BMSK                     0x40000000
#define HAL_HSUSB_OTGSC_DPIE_SHFT                           0x1e
#define HAL_HSUSB_OTGSC_1MSE_BMSK                     0x20000000
#define HAL_HSUSB_OTGSC_1MSE_SHFT                           0x1d
#define HAL_HSUSB_OTGSC_BSEIE_BMSK                    0x10000000
#define HAL_HSUSB_OTGSC_BSEIE_SHFT                          0x1c
#define HAL_HSUSB_OTGSC_BSVIE_BMSK                     0x8000000
#define HAL_HSUSB_OTGSC_BSVIE_SHFT                          0x1b
#define HAL_HSUSB_OTGSC_ASVIE_BMSK                     0x4000000
#define HAL_HSUSB_OTGSC_ASVIE_SHFT                          0x1a
#define HAL_HSUSB_OTGSC_AVVIE_BMSK                     0x2000000
#define HAL_HSUSB_OTGSC_AVVIE_SHFT                          0x19
#define HAL_HSUSB_OTGSC_IDIE_BMSK                      0x1000000
#define HAL_HSUSB_OTGSC_IDIE_SHFT                           0x18
#define HAL_HSUSB_OTGSC_DPIS_BMSK                       0x400000
#define HAL_HSUSB_OTGSC_DPIS_SHFT                           0x16
#define HAL_HSUSB_OTGSC_1MSS_BMSK                       0x200000
#define HAL_HSUSB_OTGSC_1MSS_SHFT                           0x15
#define HAL_HSUSB_OTGSC_BSEIS_BMSK                      0x100000
#define HAL_HSUSB_OTGSC_BSEIS_SHFT                          0x14
#define HAL_HSUSB_OTGSC_BSVIS_BMSK                       0x80000
#define HAL_HSUSB_OTGSC_BSVIS_SHFT                          0x13
#define HAL_HSUSB_OTGSC_ASVIS_BMSK                       0x40000
#define HAL_HSUSB_OTGSC_ASVIS_SHFT                          0x12
#define HAL_HSUSB_OTGSC_AVVIS_BMSK                       0x20000
#define HAL_HSUSB_OTGSC_AVVIS_SHFT                          0x11
#define HAL_HSUSB_OTGSC_IDIS_BMSK                        0x10000
#define HAL_HSUSB_OTGSC_IDIS_SHFT                           0x10
#define HAL_HSUSB_OTGSC_DPS_BMSK                          0x4000
#define HAL_HSUSB_OTGSC_DPS_SHFT                             0xe
#define HAL_HSUSB_OTGSC_1MST_BMSK                         0x2000
#define HAL_HSUSB_OTGSC_1MST_SHFT                            0xd
#define HAL_HSUSB_OTGSC_BSE_BMSK                          0x1000
#define HAL_HSUSB_OTGSC_BSE_SHFT                             0xc
#define HAL_HSUSB_OTGSC_BSV_BMSK                           0x800
#define HAL_HSUSB_OTGSC_BSV_SHFT                             0xb
#define HAL_HSUSB_OTGSC_ASV_BMSK                           0x400
#define HAL_HSUSB_OTGSC_ASV_SHFT                             0xa
#define HAL_HSUSB_OTGSC_AVV_BMSK                           0x200
#define HAL_HSUSB_OTGSC_AVV_SHFT                             0x9
#define HAL_HSUSB_OTGSC_ID_BMSK                            0x100
#define HAL_HSUSB_OTGSC_ID_SHFT                              0x8
#define HAL_HSUSB_OTGSC_HABA_BMSK                           0x80
#define HAL_HSUSB_OTGSC_HABA_SHFT                            0x7
#define HAL_HSUSB_OTGSC_HADP_BMSK                           0x40
#define HAL_HSUSB_OTGSC_HADP_SHFT                            0x6
#define HAL_HSUSB_OTGSC_IDPU_BMSK                           0x20
#define HAL_HSUSB_OTGSC_IDPU_SHFT                            0x5
#define HAL_HSUSB_OTGSC_DP_BMSK                             0x10
#define HAL_HSUSB_OTGSC_DP_SHFT                              0x4
#define HAL_HSUSB_OTGSC_OT_BMSK                              0x8
#define HAL_HSUSB_OTGSC_OT_SHFT                              0x3
#define HAL_HSUSB_OTGSC_HAAR_BMSK                            0x4
#define HAL_HSUSB_OTGSC_HAAR_SHFT                            0x2
#define HAL_HSUSB_OTGSC_VC_BMSK                              0x2
#define HAL_HSUSB_OTGSC_VC_SHFT                              0x1
#define HAL_HSUSB_OTGSC_VD_BMSK                              0x1
#define HAL_HSUSB_OTGSC_VD_SHFT                                0

#define HAL_HSUSB_USBMODE_ADDR                        0x000001a8
#define HAL_HSUSB_USBMODE_VBPS_BMSK                         0x20
#define HAL_HSUSB_USBMODE_VBPS_SHFT                          0x5
#define HAL_HSUSB_USBMODE_SDIS_BMSK                         0x10
#define HAL_HSUSB_USBMODE_SDIS_SHFT                          0x4
#define HAL_HSUSB_USBMODE_SLOM_BMSK                          0x8
#define HAL_HSUSB_USBMODE_SLOM_SHFT                          0x3
#define HAL_HSUSB_USBMODE_ES_BMSK                            0x4
#define HAL_HSUSB_USBMODE_ES_SHFT                            0x2
#define HAL_HSUSB_USBMODE_CM_BMSK                            0x3
#define HAL_HSUSB_USBMODE_CM_HOST_BMSK                       0x3
#define HAL_HSUSB_USBMODE_CM_DEVICE_BMSK                     0x2
#define HAL_HSUSB_USBMODE_CM_IDLE_BMSK                       0x0
#define HAL_HSUSB_USBMODE_CM_SHFT                              0

#define HAL_HSUSB_ENPDTSETUPSTAT_ADDR                 0x000001ac
#define HAL_HSUSB_ENPDTSETUPSTAT_ENDPTSETUPSTAT_BMSK      0xffff
#define HAL_HSUSB_ENPDTSETUPSTAT_ENDPTSETUPSTAT_SHFT           0

#define HAL_HSUSB_ENDPTPRIME_ADDR                     0x000001b0
#define HAL_HSUSB_ENDPTPRIME_PETB_BMSK                0xffff0000
#define HAL_HSUSB_ENDPTPRIME_PETB_SHFT                      0x10
#define HAL_HSUSB_ENDPTPRIME_PERB_BMSK                    0xffff
#define HAL_HSUSB_ENDPTPRIME_PERB_SHFT                         0

#define HAL_HSUSB_ENDPTFLUSH_ADDR                     0x000001b4
#define HAL_HSUSB_ENDPTFLUSH_FETB_BMSK                0xffff0000
#define HAL_HSUSB_ENDPTFLUSH_FETB_SHFT                      0x10
#define HAL_HSUSB_ENDPTFLUSH_FERB_BMSK                    0xffff
#define HAL_HSUSB_ENDPTFLUSH_FERB_SHFT                         0

#define HAL_HSUSB_ENDPTSTAT_ADDR                      0x000001b8
#define HAL_HSUSB_ENDPTSTAT_ETBR_BMSK                 0xffff0000
#define HAL_HSUSB_ENDPTSTAT_ETBR_SHFT                       0x10
#define HAL_HSUSB_ENDPTSTAT_ERBR_BMSK                     0xffff
#define HAL_HSUSB_ENDPTSTAT_ERBR_SHFT                          0

#define HAL_HSUSB_ENDPTCOMPLETE_ADDR                  0x000001bc
#define HAL_HSUSB_ENDPTCOMPLETE_ETCE_BMSK             0xffff0000
#define HAL_HSUSB_ENDPTCOMPLETE_ETCE_SHFT                   0x10
#define HAL_HSUSB_ENDPTCOMPLETE_ERCE_BMSK                 0xffff
#define HAL_HSUSB_ENDPTCOMPLETE_ERCE_SHFT                      0

#define HAL_HSUSB_ENDPTCTRL0_ADDR                     0x000001c0
#define HAL_HSUSB_ENDPTCTRL0_TXE_BMSK                   0x800000
#define HAL_HSUSB_ENDPTCTRL0_TXE_SHFT                       0x17
#define HAL_HSUSB_ENDPTCTRL0_TXT_BMSK                    0xc0000
#define HAL_HSUSB_ENDPTCTRL0_TXT_SHFT                       0x12
#define HAL_HSUSB_ENDPTCTRL0_TXS_BMSK                    0x10000
#define HAL_HSUSB_ENDPTCTRL0_TXS_SHFT                       0x10
#define HAL_HSUSB_ENDPTCTRL0_RXE_BMSK                       0x80
#define HAL_HSUSB_ENDPTCTRL0_RXE_SHFT                        0x7
#define HAL_HSUSB_ENDPTCTRL0_RXT_BMSK                        0xc
#define HAL_HSUSB_ENDPTCTRL0_RXT_SHFT                        0x2
#define HAL_HSUSB_ENDPTCTRL0_RXS_BMSK                        0x1
#define HAL_HSUSB_ENDPTCTRL0_RXS_SHFT                          0

#define HAL_HSUSB_ENDPTCTRL_ADDR(n)       (0x000001c0 + 4 * (n))  /* 1<=n<=15 */
#define HAL_HSUSB_ENDPTCTRL_TXE_BMSK                    0x800000
#define HAL_HSUSB_ENDPTCTRL_TXE_SHFT                        0x17
#define HAL_HSUSB_ENDPTCTRL_TXR_BMSK                    0x400000
#define HAL_HSUSB_ENDPTCTRL_TXR_SHFT                        0x16
#define HAL_HSUSB_ENDPTCTRL_TXI_BMSK                    0x200000
#define HAL_HSUSB_ENDPTCTRL_TXI_SHFT                        0x15
#define HAL_HSUSB_ENDPTCTRL_TXT_BMSK                     0xc0000
#define HAL_HSUSB_ENDPTCTRL_TXT_SHFT                        0x12
#define HAL_HSUSB_ENDPTCTRL_TXD_BMSK                     0x20000
#define HAL_HSUSB_ENDPTCTRL_TXD_SHFT                        0x11
#define HAL_HSUSB_ENDPTCTRL_TXS_BMSK                     0x10000
#define HAL_HSUSB_ENDPTCTRL_TXS_SHFT                        0x10
#define HAL_HSUSB_ENDPTCTRL_RXE_BMSK                        0x80
#define HAL_HSUSB_ENDPTCTRL_RXE_SHFT                         0x7
#define HAL_HSUSB_ENDPTCTRL_RXR_BMSK                        0x40
#define HAL_HSUSB_ENDPTCTRL_RXR_SHFT                         0x6
#define HAL_HSUSB_ENDPTCTRL_RXI_BMSK                        0x20
#define HAL_HSUSB_ENDPTCTRL_RXI_SHFT                         0x5
#define HAL_HSUSB_ENDPTCTRL_RXT_BMSK                         0xc
#define HAL_HSUSB_ENDPTCTRL_RXT_SHFT                         0x2
#define HAL_HSUSB_ENDPTCTRL_RXD_BMSK                         0x2
#define HAL_HSUSB_ENDPTCTRL_RXD_SHFT                         0x1
#define HAL_HSUSB_ENDPTCTRL_RXS_BMSK                         0x1
#define HAL_HSUSB_ENDPTCTRL_RXS_SHFT                           0

/**
Enumeration of possible error types for a HS-USB core.
*/
typedef enum
{
  HAL_HSUSB_Success,
  /**< -- No errors. */
  HAL_HSUSB_NotSupported,
  /**< -- Function is not supported by the HS-USB core. */
  HAL_HSUSB_WrongPort,
  /**< -- Port is out of the scope reported by
          HAL_hsusb_SupportedPortsQty(). */
  HSUSB_WrongPhyInterface,
  /**< -- PHY interface is not supported by the HS-USB core. */
  HSUSB_TimeoutExpired,
  /**< -- Maximum timeout has expired. */
  HSUSB_BadParam
  /**< -- Invalid parameter value. */
} HAL_HSUSB_ErrorType;

/** Enumeration of possible PHY interface types for a HS-USB core.
*/
typedef enum
{
  HAL_HSUSB_PHY_IF_ULPI,
  /**< -- ULPI connection. */
  HAL_HSUSB_PHY_IF_SERIAL,
  /**< -- Serial connection. */
  HAL_HSUSB_PHY_IF_FS_INTERCHIP,
  /**< -- Full-Speed interchip connection (no PHY). */
  HAL_HSUSB_PHY_IF_HS_INTERCHIP
  /**< -- High-Speed interchip connection (no PHY). */
} HAL_HSUSB_PhyInterfaceType;

/** Enumeration of possible PHY connection types for a HS-USB core.
*/
typedef enum
{
  HAL_HSUSB_PHY_INTEGRATED,
  /**< -- PHY is integrated. */
  HAL_HSUSB_PHY_EXTERNAL
  /**< -- PHY is external. */
} HAL_HSUSB_PhyConnectionType;

/** Enumeration of possible PHY characteristics.
*/
typedef struct
{
  HAL_HSUSB_PhyInterfaceType  PhyInterfaceType;
  /**< Interface type for how the PHY interconnects to the USB core. */
  HAL_HSUSB_PhyConnectionType PhyConnectionType;
  /**< Description of whether the PHY is internal or external. */
} HAL_HSUSB_PhyType;

/** Enumeration of modes of the AMBA AHB master interface m_hburst signal.
*/
typedef enum
{
  HAL_HSUSB_AHB_BURST_INCR          = 0x0, /**< &nbsp; */
  HAL_HSUSB_AHB_BURST_INCR4_SINGLE  = 0x1, /**< &nbsp; */
  HAL_HSUSB_AHB_BURST_INCR8_SINGLE  = 0x2, /**< &nbsp; */
  HAL_HSUSB_AHB_BURST_INCR16_SINGLE = 0x3, /**< &nbsp; */
  HAL_HSUSB_AHB_BURST_INCR4_MULTI   = 0x5, /**< &nbsp; */
  HAL_HSUSB_AHB_BURST_INCR8_MULTI   = 0x6, /**< &nbsp; */
  HAL_HSUSB_AHB_BURST_INCR16_MULTI  = 0x7  /**< &nbsp; */
} HAL_HSUSB_AhbBurstMode;

/** Enumeration of the HPROT signal modes.
*/
typedef enum
{
  HAL_HSUSB_HPROT_NONPOSTED = 0x0,
  /**< -- HPROT signal out of the XTOR is 0001, and all transactions are
          non-posted. */
  HAL_HSUSB_HPROT_POSTED    = 0x1,
  /**< -- HPROT signal out of the XTOR is 0101, and all transactions are
          posted. */
  HAL_HSUSB_HPROT_ALT       = 0x2
  /**< -- HPROT signal out of the XTOR alternates between the above states
          according to the context of the access.\ Control structures are
	      non-posted while data are posted. */
} HAL_HSUSB_HprotMode;

/** Enumeration of possible PHY types for a HS-USB core.
*/
typedef enum
{
  HAL_HSUSB_TEST_BUS_DISABLED    = 0x0,  /**< -- Test bus is disabled. */
  HAL_HSUSB_TEST_BUS_KSM         = 0x1,  /**< -- Key State Machine. */
  HAL_HSUSB_TEST_BUS_DMA_ENG_0   = 0x2,  /**< &nbsp; */
  HAL_HSUSB_TEST_BUS_DMA_ENG_1   = 0x3,  /**< &nbsp; */
  HAL_HSUSB_TEST_BUS_DMA_ENG_2   = 0x4,  /**< &nbsp; */
  HAL_HSUSB_TEST_BUS_PROT_ENG_0  = 0x5,  /**< &nbsp; */
  HAL_HSUSB_TEST_BUS_PROT_ENG_1  = 0x6,  /**< &nbsp; */
  HAL_HSUSB_TEST_BUS_PROT_ENG_2  = 0x7,  /**< &nbsp; */
  HAL_HSUSB_TEST_BUS_PORT_CTRL_0 = 0x8,  /**< &nbsp; */
  HAL_HSUSB_TEST_BUS_PORT_CTRL_1 = 0x9,  /**< &nbsp; */
  HAL_HSUSB_TEST_BUS_TX_BUFFER   = 0xa,  /**< &nbsp; */
  HAL_HSUSB_TEST_BUS_RX_BUFFER   = 0xb,  /**< &nbsp; */
  HAL_HSUSB_TEST_BUS_OTG         = 0xc,  /**< -- OTG. */
  HAL_HSUSB_TEST_BUS_HSIC_1      = 0xd,  /**< -- As in hsic_test_bus1. */
  HAL_HSUSB_TEST_BUS_HSIC_2      = 0xe,  /**< -- As in hsic_test_bus2. */
  HAL_HSUSB_TEST_BUS_RESERVED_15 = 0xf,  /**< &nbsp; */
  HAL_HSUSB_TEST_BUS_DMA_ENG_3   = 0x10, /**< -- As in dma_eng_3 dma_dev_sm_2. */
  HAL_HSUSB_TEST_BUS_DMA_ENG_4   = 0x11  /**< -- As in dma_eng_4 dma_traf. */
} HAL_HSUSB_TestBusSelectorType;

/* -----------------------------------------------------------------------
**                           INTERFACES
** ----------------------------------------------------------------------- */

/* 
* C++ wrapper
*/
#ifdef __cplusplus
extern "C" {
#endif

/* Mandatory APIs */

/*=============================================================================
FUNCTION      HAL_hsusb_Init
=============================================================================*/
/**
Initializes the HS-USB core hardware before the use of any other function after
power-up or a call to HAL_hsusb_Reset().

@dependencies
None.

@side effects
None.

@return
None.
*/
void    HAL_hsusb_Init(void);

/*=============================================================================
FUNCTION      HAL_hsusb_Reset
=============================================================================*/
/**
Returns the HS-USB hardware to its power-on status.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
None.
*/
void    HAL_hsusb_Reset(void);

/* Special APIs */

/*=============================================================================
FUNCTION      HAL_hsusb_SupportedCoresQty
=============================================================================*/
/**
Reports the number of supported HS-USB cores.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
The number of HS-USB cores attached to the target.
*/
uint8   HAL_hsusb_SupportedCoresQty(void);

/*=============================================================================
FUNCTION      HAL_hsusb_HWversion
=============================================================================*/
/**
Reports the hardware version for a specified HS-USB core.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
						HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
The hardware version for the specified HS-USB core.
*/
uint16  HAL_hsusb_HWversion(uint8 instance);

/*=============================================================================
FUNCTION      HAL_hsusb_SupportedPortsQty
=============================================================================*/
/**
Reports the number of supported HS-USB ports.

@param instance [IN] -- Determines which HS-USB core to use. This value should 
                        be one less than reported by 
						HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
The number of ports attached to the specified HS-USB core.
*/
uint8   HAL_hsusb_SupportedPortsQty(uint8 instance);

/*=============================================================================
FUNCTION      HAL_hsusb_SupportedPhyType
=============================================================================*/
/**
Reports which PHY type is supported by a specified HS-USB core port.

@param instance [IN] -- Determines which HS-USB core to use. This value should 
                        be one less than reported by 
						HAL_hsusb_SupportedCoresQty().
@param port     [IN] -- Determines which HS-USB core port to use. This value 
                        should be one less than reported by 
						HAL_hsusb_SupportedPortsQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
The PHY type supported by the specified HS-USB core port.
*/
HAL_HSUSB_PhyType* HAL_hsusb_SupportedPhyType(uint8 instance, uint8 port);

/*=============================================================================
FUNCTION      HAL_hsusb_GetLastError
=============================================================================*/
/**
Gets the last error code of the HS-USB core recorded from a HS-USB HAL failure.
This function is intended for debug purposes.

@param instance [IN] -- Determines which HS-USB core to use. This value should 
                        be one less than reported by 
						HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
The last error code of the HS-USB core recorded from a HS-USB HAL failure.
*/
HAL_HSUSB_ErrorType HAL_hsusb_GetLastError(uint8 instance);

/*=============================================================================
FUNCTION      HAL_hsusb_ReadUlpiPhy
=============================================================================*/
/**
Reads from a ULPI PHY register of the HS-USB core port.

@note This function is valid for HS-USB hardware version 1 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should 
                        be one less than reported by 
						HAL_hsusb_SupportedCoresQty().
@param port     [IN] -- Determines which HS-USB core port to use. This value 
                        should be one less than reported by 
						HAL_hsusb_SupportedPortsQty().
@param reg      [IN] -- Determines the register address. This value must be in 
                        the address ranges defined by the hardware 
						specification.
@param data    [OUT] -- Returns the read data.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
HAL_HSUSB_Success           -- The returned data value is valid.\n
HAL_HSUSB_WrongPort         -- The port is out of the scope reported by
                               HAL_hsusb_SupportedPortsQty().\n
HAL_HSUSB_WrongPhyInterface -- The PHY interface is not ULPI.\n
HAL_HSUSB_TimeoutExpired    -- The maximum timeout has expired.
*/
HAL_HSUSB_ErrorType HAL_hsusb_ReadUlpiPhy(uint8 instance, uint8 port, uint8 reg, uint8* data);

/*=============================================================================
FUNCTION      HAL_hsusb_WriteUlpiPhy
=============================================================================*/
/**
Writes to a ULPI PHY register of the HS-USB core port.

@note This function is valid for HS-USB hardware version 1 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should 
                        be one less than reported by 
						HAL_hsusb_SupportedCoresQty().
@param port     [IN] -- Determines which HS-USB core port to use. This value 
                        should be should be one less than reported by
                        HAL_hsusb_SupportedPortsQty().
@param reg      [IN] -- Determines the register address. This value must be in 
                        the address ranges defined by the hardware 
						specification.
@param data     [IN] -- Determines the write data.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
HAL_HSUSB_Success           -- The returned data value is valid.\n
HAL_HSUSB_WrongPort         -- The port is out of the scope reported by
                               HAL_hsusb_SupportedPortsQty().\n
HAL_HSUSB_WrongPhyInterface -- The PHY interface is not ULPI.\n
HAL_HSUSB_TimeoutExpired    -- The maximum timeout has expired.
*/
HAL_HSUSB_ErrorType HAL_hsusb_WriteUlpiPhy(uint8 instance, uint8 port, uint8 reg, uint8 data);

/*=============================================================================
FUNCTION      HAL_hsusb_Read8
=============================================================================*/
/**
Reads 8 bits from the HS-USB core register.

@param instance [IN] -- Determines which HS-USB core to use. This value should 
                        be one less than reported by 
						HAL_hsusb_SupportedCoresQty().
@param offset   [IN] -- Determines the register offset. This value must be 
                        aligned at the 32-bit boundary and in the address 
						ranges defined by the hardware specification.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
The read data.
*/
uint8   HAL_hsusb_Read8(uint8 instance, uint32 offset);

/*=============================================================================
FUNCTION      HAL_hsusb_Read16
=============================================================================*/
/**
Reads 16 bits from the HS-USB core register.

@param instance [IN] -- Determines which HS-USB core to use. This value should 
                        be one less than reported by 
						HAL_hsusb_SupportedCoresQty().
@param offset   [IN] -- Determines the register offset. This value must be 
                        aligned at the 32-bit boundary and in the address 
						ranges defined by the hardware specification.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
The read data.
*/
uint16  HAL_hsusb_Read16(uint8 instance, uint32 offset);

/*=============================================================================
FUNCTION      HAL_hsusb_Read32
=============================================================================*/
/**
Reads 32 bits from the HS-USB core register.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
						HAL_hsusb_SupportedCoresQty().
@param offset   [IN] -- Determines the register offset. This value must be 
                        aligned at the 32-bit boundary and in the address 
						ranges defined by the hardware specification.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
The read data.
*/
uint32  HAL_hsusb_Read32(uint8 instance, uint32 offset);

/*=============================================================================
FUNCTION      HAL_hsusb_Write8
=============================================================================*/
/**
Writes 8 bits to the HS-USB core register.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
						HAL_hsusb_SupportedCoresQty().
@param offset   [IN] -- Determines the register offset. This value must be 
                        aligned at the 32-bit boundary and in the address 
						ranges defined by the hardware specification.
@param data     [IN] -- Determines the write data.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
None.
*/
void    HAL_hsusb_Write8(uint8 instance, uint32 offset, uint8 data);

/*=============================================================================
FUNCTION      HAL_hsusb_Write16
=============================================================================*/
/**
Writes 16 bits to the HS-USB core register.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
						HAL_hsusb_SupportedCoresQty().
@param offset   [IN] -- Determines the register offset. This value must be
                        aligned at the 32-bit boundary and in the address 
						ranges defined by the hardware specification.
@param data     [IN] -- Determines the write data.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
None.
*/
void    HAL_hsusb_Write16(uint8 instance, uint32 offset, uint16 data);

/*=============================================================================
FUNCTION      HAL_hsusb_Write32
=============================================================================*/
/**
Writes 32 bits to the HS-USB core register.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
						HAL_hsusb_SupportedCoresQty().
@param offset   [IN] -- Determines the register offset. This value must be
                        aligned at the 32-bit boundary and in the address 
						ranges defined by the hardware specification.
@param data     [IN] -- Determines the write data.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
None.
*/
void    HAL_hsusb_Write32(uint8 instance, uint32 offset, uint32 data);

/*=============================================================================
FUNCTION      HAL_hsusb_GetBits32
=============================================================================*/
/**
Gets the bits in the HS-USB core 32-bit register.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
						HAL_hsusb_SupportedCoresQty().
@param offset   [IN] -- Determines the register offset. This value must be
                        aligned at the 32-bit boundary and in the address 
						ranges defined by the hardware specification.
@param bits     [IN] -- Determines the bits to be retrieved.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
The read data masked with \<bits\>.
*/
uint32  HAL_hsusb_GetBits32(uint8 instance, uint32 offset, uint32 bits);

/*=============================================================================
FUNCTION      HAL_hsusb_SetBits32
=============================================================================*/
/**
Sets bits in the HS-USB core 32-bit register.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
						HAL_hsusb_SupportedCoresQty().
@param offset   [IN] -- Determines the register offset. This value must be
                        aligned at the 32-bit boundary and in the address 
						ranges defined by the hardware specification.
@param bits     [IN] -- Determines the bits to be set.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
None.
*/
void    HAL_hsusb_SetBits32(uint8 instance, uint32 offset, uint32 bits);

/*=============================================================================
FUNCTION      HAL_hsusb_ClearBits32
=============================================================================*/
/**
Clears bits in the HS-USB core 32-bit register.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
						HAL_hsusb_SupportedCoresQty().
@param offset   [IN] -- Determines the register offset. This value must be
                        aligned at the 32-bit boundary and in the address 
						ranges defined by the hardware specification.
@param bits     [IN] -- Determines the bits to be cleared.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
None.
*/
void    HAL_hsusb_ClearBits32(uint8 instance, uint32 offset, uint32 bits);

/*=============================================================================
FUNCTION      HAL_hsusb_ClearAndSetBits32
=============================================================================*/
/**
Clears and then sets bits in the HS-USB core 32-bit register.

@param instance  [IN] -- Determines which HS-USB core to use. This value should
                         be one less than reported by 
						 HAL_hsusb_SupportedCoresQty().
@param offset    [IN] -- Determines the register offset. This value must be
                        aligned at the 32-bit boundary and in the address 
						ranges defined by the hardware specification.
@param clearBits [IN] -- Determines the bits to be cleared.
@param setBits   [IN] -- Determines the bits to be set.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
None.
*/
void    HAL_hsusb_ClearAndSetBits32(uint8 instance, uint32 offset, uint32 clearBits, uint32 setBits);

/*=============================================================================
FUNCTION      HAL_hsusb_GetAhbBurstMode
=============================================================================*/
/**
Reports the mode of the AMBA AHB master interface m_hburst signal.

@note This function is valid for HS-USB hardware version 2 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
The mode of the AMBA AHB master interface m_hburst signal.
*/
HAL_HSUSB_AhbBurstMode HAL_hsusb_GetAhbBurstMode(uint8 instance);

/*=============================================================================
FUNCTION      HAL_hsusb_SetAhbBurstMode
=============================================================================*/
/**
Sets the mode of the AMBA AHB master interface m_hburst signal.

@note This function is valid for HS-USB hardware version 2 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param eMode    [IN] -- Determines mode of the AMBA AHB master interface 
                        m_hburst signal.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
None.
*/
void    HAL_hsusb_SetAhbBurstMode(uint8 instance, HAL_HSUSB_AhbBurstMode eMode);

/*=============================================================================
FUNCTION      HAL_hsusb_GetSe0GlitchFixCtrl
=============================================================================*/
/**
Reports whether the SE0 glitch fix mechanism is activated.

@note This function is valid for HS-USB hardware version 2 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
TRUE  -- The SE0 glitch fix mechanism is activated.\n
FALSE -- The SE0 glitch fix mechanism is not activated.
*/
boolean HAL_hsusb_GetSe0GlitchFixCtrl(uint8 instance);

/*=============================================================================
FUNCTION      HAL_hsusb_SetSe0GlitchFixCtrl
=============================================================================*/
/**
Activates/deactivates the SE0 glitch fix mechanism.

@note This function is valid for HS-USB hardware version 2 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- Determines the requested action. Values are:
                        - TRUE -- Activate the SE0 glitch fix mechanism.
                        - FALSE -- Deactivate the SE0 glitch fix mechanism.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
None.
*/
void    HAL_hsusb_SetSe0GlitchFixCtrl(uint8 instance, boolean state);

/*=============================================================================
FUNCTION      HAL_hsusb_GetFs3Wire2WireSelect
=============================================================================*/
/**
Reports whether the two-wire interface on the fs_dat and fs_se0 pins is enabled
or disabled.

@note This function is valid for HS-USB hardware version 2 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
TRUE  -- The two-wire interface on the fs_dat and fs_se0 pins is enabled.\n
FALSE -- The two-wire interface on the fs_dat and fs_se0 pins is disabled.
*/
boolean HAL_hsusb_GetFs3Wire2WireSelect(uint8 instance);

/*=============================================================================
FUNCTION      HAL_hsusb_SetFs3Wire2WireSelect
=============================================================================*/
/**
Activates/deactivates the two-wire interface on the fs_dat and fs_se0 pins.

@note This function is valid for HS-USB hardware version 2 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- Determines the requested action. Values are:
                        - TRUE -- Enable the two wire interface on the fs_dat 
						          and fs_se0 pins.
                        - FALSE -- Disable the two wire interface on the fs_dat
						           and fs_se0 pins.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
None.
*/
void    HAL_hsusb_SetFs3Wire2WireSelect(uint8 instance, boolean state);

/*=============================================================================
FUNCTION      HAL_hsusb_GetXtorBypass
=============================================================================*/
/**
Reports whether the AHB Transactor is bypassed.

@note This function is valid for HS-USB hardware version 1 only.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
TRUE  -- The AHB Transactor is bypassed.\n
FALSE -- The AHB Transactor is not bypassed.
*/
boolean HAL_hsusb_GetXtorBypass(uint8 instance);

/*=============================================================================
FUNCTION      HAL_hsusb_SetXtorBypass
=============================================================================*/
/**
Sets the AHB Transactor bypass mode.

@note This function is valid for HS-USB hardware version 1 only.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- Determines the requested action. Values are:
                        - TRUE -- Bypass the AHB Transactor.
                        - FALSE -- Do not bypass the AHB Transactor.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
None.
*/
void    HAL_hsusb_SetXtorBypass(uint8 instance, boolean state);

/*=============================================================================
FUNCTION      HAL_hsusb_GetPostedWrites
=============================================================================*/
/**
Reports the USB core AHB posted data writes mode.

@note This function is valid for HS-USB hardware version 1 only.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
TRUE  -- The AHB master interface enables posted data writes.\n
FALSE -- The AHB master interface disables posted data writes.
*/
boolean HAL_hsusb_GetPostedWrites(uint8 instance);

/*=============================================================================
FUNCTION      HAL_hsusb_SetPostedWrites
=============================================================================*/
/**
Sets the USB core AHB posted data writes mode.

@note This function is valid for HS-USB hardware version 1 only.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- Determines the requested action. Values are:
                        - TRUE -- The AHB master interface enables posted data
                                  writes.
                        - FALSE -- The AHB master interface disables posted
						           data writes.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
None.
*/
void    HAL_hsusb_SetPostedWrites(uint8 instance, boolean state);

/*=============================================================================
FUNCTION      HAL_hsusb_GetGrantStolen
=============================================================================*/
/**
Reports whether the arbiter removed the hgrant signal before completing a
transaction.

@note This function is valid for HS-USB hardware version 2 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
TRUE  -- The arbiter removed the hgrant signal before completing a
         transaction.\n
FALSE -- The arbiter did not remove the hgrant signal before completing a
         transaction.
*/
boolean HAL_hsusb_GetGrantStolen(uint8 instance);

/*=============================================================================
FUNCTION      HAL_hsusb_SetGrantStolen
=============================================================================*/
/**
Sets the arbiter to remove the hgrant signal before completing a transaction.

@note This function is valid for HS-USB hardware version 2 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- Determines the requested action. Values are:
                        - TRUE -- Enable the arbiter to remove the hgrant
						          signal before completing a transaction.
                        - FALSE -- Disable the arbiter from removing the
						           hgrant signal before completing a
								   transaction.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
None.
*/
void    HAL_hsusb_SetGrantStolen(uint8 instance, boolean state);

/*=============================================================================
FUNCTION      HAL_hsusb_GetHprotMode
=============================================================================*/
/**
Reports the HPROT signal mode.

@note This function is valid for HS-USB hardware version 2 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
The HPROT signal mode.
*/
HAL_HSUSB_HprotMode HAL_hsusb_GetHprotMode(uint8 instance);

/*=============================================================================
FUNCTION      HAL_hsusb_SetHprotMode
=============================================================================*/
/**
Sets the HPROT signal mode.

@note This function is valid for HS-USB hardware version 2 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param eMode    [IN] -- Determines the HPROT signal mode.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
None.
*/
void    HAL_hsusb_SetHprotMode(uint8 instance, HAL_HSUSB_HprotMode eMode);

/*=============================================================================
FUNCTION      HAL_hsusb_GetIncrOverride
=============================================================================*/
/**
Reports the USB core INCR bursts transformation.

@note This function is valid for HS-USB hardware version 5 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
TRUE  -- All INCR bursts from the USB core will be internally transformed into
         SINGLE transfers.\n
FALSE -- If the USB core issues an INCR burst, it will propagate to the
         external master AHB port.
*/
boolean HAL_hsusb_GetIncrOverride(uint8 instance);

/*=============================================================================
FUNCTION      HAL_hsusb_SetIncrOverride
=============================================================================*/
/**
Sets the USB core INCR bursts transformation.

@note This function is valid for HS-USB hardware version 5 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- Determines the requested action. Values are:
                        - TRUE -- All INCR bursts from the USB core will be
                                  internally transformed into SINGLE transfers.
                        - FALSE -- If the USB core issues an INCR burst, it
                                   propogates to the external master AHB port.
							    
@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
None.
*/
void    HAL_hsusb_SetIncrOverride(uint8 instance, boolean state);

/*=============================================================================
FUNCTION      HAL_hsusb_GetAsyncBridgesBypass
=============================================================================*/
/**
Reports the asynchronous bridge bypass mode on the master AHB interface.

@note This function is valid for HS-USB hardware version 5 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
TRUE  -- The asynchronous bridge on the master AHB interface is bypassed.\n
FALSE -- The asynchronous bridge on the master AHB interface is issued.
*/
boolean HAL_hsusb_GetAsyncBridgesBypass(uint8 instance);

/*=============================================================================
FUNCTION      HAL_hsusb_SetAsyncBridgesBypass
=============================================================================*/
/**
Sets the asynchronous bridge bypass mode on the master AHB interface.

@note This function is valid for HS-USB hardware version 5 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- Determines the requested action. Values are:
                        - TRUE -- The asynchronous bridge on the master AHB
                                  interface is bypassed.
                        - FALSE -- The asynchronous bridge on the master AHB
                                   interface is issued.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
None.
*/
void    HAL_hsusb_SetAsyncBridgesBypass(uint8 instance, boolean state);

/*=============================================================================
FUNCTION      HAL_hsusb_GetTestBusSelector
=============================================================================*/
/**
Reports which group of the internal signals is routed to the test bus.

@note This function is valid for HS-USB hardware version 2 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
The group of the internal signals that is routed to the test bus.
*/
HAL_HSUSB_TestBusSelectorType HAL_hsusb_GetTestBusSelector(uint8 instance);

/*=============================================================================
FUNCTION      HAL_hsusb_SetTestBusSelector
=============================================================================*/
/**
Sets the group of internal signals that are to be routed to the test bus.

@note This function is valid for HS-USB hardware version 2 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param eMode    [IN] -- Determines the group of internal signals that are to be
                        routed to the test bus.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
None.
*/
void    HAL_hsusb_SetTestBusSelector(uint8 instance, HAL_HSUSB_TestBusSelectorType eMode);

/*=============================================================================
FUNCTION      HAL_hsusb_GetTxBufCollisionMode
=============================================================================*/
/**
Reports the Tx buffer collision mode of the controller. 

@note This function is valid for HS-USB hardware version 4 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
TRUE  -- Tx buffer collision direct mode.\n
FALSE -- Tx buffer collision revert mode.
*/
boolean HAL_hsusb_GetTxBufCollisionMode(uint8 instance);

/*=============================================================================
FUNCTION      HAL_hsusb_SetTxBufCollisionMode
=============================================================================*/
/**
Sets the Tx buffer collision mode of the controller.

@note This function is valid for HS-USB hardware version 4 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- Determines the requested action. Values are:
                        - TRUE -- Tx buffer collision direct mode.
                        - FALSE -- Tx buffer collision revert mode.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
None.
*/
void    HAL_hsusb_SetTxBufCollisionMode(uint8 instance, boolean state);

/*=============================================================================
FUNCTION      HAL_hsusb_GetStreamRxBypassMode
=============================================================================*/
/**
Reports the Rx streaming bypass mode.

@note This function is valid for HS-USB hardware version 5 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
TRUE  -- Rx streaming bypass is enabled.\n
FALSE -- Rx streaming bypass is disabled.
*/
boolean HAL_hsusb_GetStreamRxBypassMode(uint8 instance);

/*=============================================================================
FUNCTION      HAL_hsusb_SetStreamRxBypassMode
=============================================================================*/
/**
Sets the Rx streaming bypass mode.

@note This function is valid for HS-USB hardware version 5 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- Determines the requested action. Values are:
                        - TRUE -- Enable Rx streaming bypass.
                        - FALSE -- Disable Rx streaming bypass.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
None.
*/
void    HAL_hsusb_SetStreamRxBypassMode(uint8 instance, boolean state);

/*=============================================================================
FUNCTION      HAL_hsusb_GetStreamTxAutoMode
=============================================================================*/
/**
Reports the Tx streaming automatic determination mode.

@note This function is valid for HS-USB hardware version 5 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
TRUE  -- Tx streaming automatic determination is enabled.\n
FALSE -- Tx streaming automatic determination is disabled.
*/
boolean HAL_hsusb_GetStreamTxAutoMode(uint8 instance);

/*=============================================================================
FUNCTION      HAL_hsusb_SetStreamTxAutoMode
=============================================================================*/
/**
Sets the Tx streaming automatic determination mode.

@note This function is valid for HS-USB hardware version 5 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- Determines the requested action. Values are:
                        - TRUE -- Enable Tx streaming automatic determination.
                        - FALSE -- Disable Tx streaming automatic determination.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
None.
*/
void    HAL_hsusb_SetStreamTxAutoMode(uint8 instance, boolean state);

/*=============================================================================
FUNCTION      HAL_hsusb_GetHostSimTimersStdMode
=============================================================================*/
/**
Reports the setting of the timers used for USB reset in ULPI mode.

@note This function is valid for HS-USB hardware version 1 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
TRUE  -- The timers used for USB reset in ULPI mode are enabled.\n
FALSE -- The timers used for USB reset in ULPI mode are disabled.
*/
boolean HAL_hsusb_GetHostSimTimersStdMode(uint8 instance);

/*=============================================================================
FUNCTION      HAL_hsusb_SetHostSimTimersStdMode
=============================================================================*/
/**
Sets the timers used for USB reset in ULPI mode.

@note This function is valid for HS-USB hardware version 1 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- Determines the requested action. Values are:
                        - TRUE -- Enable the timers used for USB reset in ULPI
                                  mode.
                        - FALSE -- Disable the timers used for USB reset in
						           ULPI mode.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
None.
*/
void    HAL_hsusb_SetHostSimTimersStdMode(uint8 instance, boolean state);

/*=============================================================================
FUNCTION      HAL_hsusb_GetHostSimTimersSuspMode
=============================================================================*/
/**
Reports the timers used for the USB suspend process mode.

@note This function is valid for HS-USB hardware version 2 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
TRUE  -- The timers used for the USB suspend process are set short for faster
         simulation and ATE times.\n
FALSE -- The timers used for the USB suspend process are set according to the
         USB specification.
*/
boolean HAL_hsusb_GetHostSimTimersSuspMode(uint8 instance);

/*=============================================================================
FUNCTION      HAL_hsusb_SetHostSimTimersSuspMode
=============================================================================*/
/**
Sets the timers used for the USB suspend process mode.

@note This function is valid for HS-USB hardware version 2 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- Determines the requested action. Values are:
                        - TRUE  -- Set the timers used for the USB suspend
						           process short for faster simulation and ATE
								   times.
                        - FALSE -- Set the timers used for the USB suspend
						           process according to the USB specification.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
None.
*/
void    HAL_hsusb_SetHostSimTimersSuspMode(uint8 instance, boolean state);

/*=============================================================================
FUNCTION      HAL_hsusb_GetDevicePEStateReset
=============================================================================*/
/**
Reports whether an automatic reset of Device PE State is enabled.

@note This function is valid for HS-USB hardware version 5 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
TRUE  -- An automatic reset of Device PE State is enabled.\n
FALSE -- An automatic reset of Device PE State is disabled.
*/
boolean HAL_hsusb_GetDevicePEStateReset(uint8 instance);

/*=============================================================================
FUNCTION      HAL_hsusb_SetDevicePEStateReset
=============================================================================*/
/**
Enables/disables an automatic reset of Device PE State.

@note This function is valid for HS-USB hardware version 5 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- Determines the requested action. Values are:
                        - TRUE  -- Enable an automatic reset of Device PE
						           state.
                        - FALSE -- Disable an automatic reset of Device PE
						           state.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
None.
*/
void    HAL_hsusb_SetDevicePEStateReset(uint8 instance, boolean state);

/*=============================================================================
FUNCTION      HAL_hsusb_GetIsochronousBugFix
=============================================================================*/
/**
Reports whether the Isochronous bug fix is enabled.

@note This function is valid for HS-USB hardware version 5 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
TRUE  -- The Isochronous bug fix is enabled.\n
FALSE -- The Isochronous bug fix is disabled.
*/
boolean HAL_hsusb_GetIsochronousBugFix(uint8 instance);

/*=============================================================================
FUNCTION      HAL_hsusb_SetIsochronousBugFix
=============================================================================*/
/**
Enables/disables the Isochronous bug fix.

@note This function is valid for HS-USB hardware version 5 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- Determines the requested action. Values are:
                        - TRUE  -- Enable the Isochronous bug fix.
                        - FALSE -- Disable the Isochronous bug fix.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
None.
*/
void    HAL_hsusb_SetIsochronousBugFix(uint8 instance, boolean state);

/*=============================================================================
FUNCTION      HAL_hsusb_GetAHBBufferableDataAccess
=============================================================================*/
/**
Reports whether AHB-bufferable data access is enabled.

@note This function is valid for HS-USB hardware version 6 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
TRUE  -- AHB-bufferable data access is enabled.\n
FALSE -- AHB-bufferable data access is disabled.
*/
boolean HAL_hsusb_GetAHBBufferableDataAccess(uint8 instance);

/*=============================================================================
FUNCTION      HAL_hsusb_SetAHBBufferableDataAccess
=============================================================================*/
/**
Enables/disables AHB-bufferable data access.

@note This function is valid for HS-USB hardware version 6 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- Determines the requested action. Values are:
                        - TRUE -- Enable AHB-bufferable data access.
                        - FALSE -- Disable AHB-bufferable data access.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
None.
*/
void    HAL_hsusb_SetAHBBufferableDataAccess(uint8 instance, boolean state);

/*=============================================================================
FUNCTION      HAL_hsusb_GetLegacyMode
=============================================================================*/
/**
Reports whether Legacy mode is enabled.

@note This function is valid for HS-USB hardware version 6 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
TRUE  -- Legacy mode is enabled.\n
FALSE -- Legacy mode is disabled.
*/
boolean HAL_hsusb_GetLegacyMode(uint8 instance);

/*=============================================================================
FUNCTION      HAL_hsusb_SetLegacyMode
=============================================================================*/
/**
Enables/disables Legacy mode.

@note This function is valid for HS-USB hardware version 6 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- Determines the requested action. Values are:
                        - TRUE -- Enable Legacy mode.
                        - FALSE -- Disable Legacy mode.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
None.
*/
void    HAL_hsusb_SetLegacyMode(uint8 instance, boolean state);

/*=============================================================================
FUNCTION:      HAL_hsusb_GetEndpointPipeID
=============================================================================*/
/**
Reports the endpoint pipe ID number.

@note This function is valid for HS-USB hardware version 6 and above.

@param instance  [IN] -- Determines which HS-USB core to use. This value should
                         be one less than reported by 
                         HAL_hsusb_SupportedCoresQty().
@param ep        [IN] -- Determines the endpoint number. Value is between 1 and
                         15.
@param direction [IN] -- Determines the endpoint direction. Values are:
                         - TRUE -- IN endpoint.
                         - FALSE -- OUT endpoint.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
The endpoint pipe number. Values are 0-0x1F.
*/
uint8   HAL_hsusb_GetEndpointPipeID(uint8 instance, uint8 ep, boolean direction);

/*=============================================================================
FUNCTION      HAL_hsusb_SetEndpointPipeID
=============================================================================*/
/**
Sets the endpoint pipe ID number.

@note This function is valid for HS-USB hardware version 6 and above.

@param instance  [IN] -- Determines which HS-USB core to use. This value should
                         be one less than reported by 
                         HAL_hsusb_SupportedCoresQty().
@param ep        [IN] -- Determines the endpoint number. Value is between 1 and
                         15.
@param direction [IN] -- Determines the endpoint direction. Values are:
                         - TRUE -- IN endpoint.
                         - FALSE -- OUT endpoint.
@param id        [IN] -- Determines the endpoint pipe number. Values are 0-0x1F.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
None.
*/
void    HAL_hsusb_SetEndpointPipeID(uint8 instance, uint8 ep, boolean direction, uint8 id);

/*=============================================================================
FUNCTION:      HAL_hsusb_GetSessVldOperation
=============================================================================*/
/**
Reports whether Link Controller operation is enabled after switching the
interface from serial to ULPI.

@note This function is valid for HS-USB hardware version 7 and above.

@param instance  [IN] -- Determines which HS-USB core to use. This value should
                         be one less than reported by 
                         HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
TRUE  -- Link Controller operation is enabled.\n
FALSE -- Link Controller operation is disabled.
*/
boolean HAL_hsusb_GetSessVldOperation(uint8 instance);

/*=============================================================================
FUNCTION      HAL_hsusb_SetSessVldOperation
=============================================================================*/
/**
Enables/disables Link Controller operation after switching the interface from
serial to ULPI.

@note This function is valid for HS-USB hardware version 7 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- Determines the requested action. Values are:
                        - TRUE -- Enable Link Controller operation.
                        - FALSE -- Disable Link Controller operation.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
None.
*/
void    HAL_hsusb_SetSessVldOperation(uint8 instance, boolean state);

/*=============================================================================
FUNCTION:      HAL_hsusb_GetSessVldStatus
=============================================================================*/
/**
Reports the PHY SESS_VLD status.

@note This function is valid for HS-USB hardware version 7 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
TRUE  -- The PHY SESS_VLD status is set.\n
FALSE -- The PHY SESS_VLD status is not set.
*/
boolean HAL_hsusb_GetSessVldStatus(uint8 instance);

/*=============================================================================
FUNCTION      HAL_hsusb_SetSessVldStatus
=============================================================================*/
/**
Sets/clears the PHY SESS_VLD status.

@note This function is valid for HS-USB hardware version 7 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- Determines the requested action. Values are:
                        - TRUE -- Set the PHY SESS_VLD status.
                        - FALSE -- Clear the PHY SESS_VLD status.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
None.
*/
void    HAL_hsusb_SetSessVldStatus(uint8 instance, boolean state);

/*=============================================================================
FUNCTION:      HAL_hsusb_GetSessVldChangeStatus
=============================================================================*/
/**
Reports whether the PHY SESS_VLD status has changed.

@note This function is valid for HS-USB hardware version 7 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
TRUE  -- The PHY SESS_VLD status has changed.\n
FALSE -- The PHY SESS_VLD status has not changed.
*/
boolean HAL_hsusb_GetSessVldChangeStatus(uint8 instance);

/*=============================================================================
FUNCTION      HAL_hsusb_SetSessVldChangeStatus
=============================================================================*/
/**
Sets/clears the PHY SESS_VLD status change.

@note This function is valid for HS-USB hardware version 7 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- Determines the requested action. Values are:
                        - TRUE -- Set the PHY SESS_VLD status change.
                        - FALSE -- Clear the PHY SESS_VLD status change.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
None.
*/
void    HAL_hsusb_SetSessVldChangeStatus(uint8 instance, boolean state);

/*=============================================================================
FUNCTION:      HAL_hsusb_GetSessVldChangeInterrupt
=============================================================================*/
/**
Reports whether the PHY SESS_VLD status change interrupt is enabled.

@note This function is valid for HS-USB hardware version 7 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
TRUE  -- The PHY SESS_VLD status change interrupt is enabled.\n
FALSE -- The PHY SESS_VLD status change interrupt is disabled.
*/
boolean HAL_hsusb_GetSessVldChangeInterrupt(uint8 instance);

/*=============================================================================
FUNCTION      HAL_hsusb_SetSessVldChangeInterrupt
=============================================================================*/
/**
Enables/disables the PHY SESS_VLD status change interrupt.

@note This function is valid for HS-USB hardware version 7 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- Determines the requested action. Values are:
                        - TRUE -- Enable the PHY SESS_VLD status change
						          interrupt.
                        - FALSE -- Disable the PHY SESS_VLD status change
						           interrupt.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
None.
*/
void    HAL_hsusb_SetSessVldChangeInterrupt(uint8 instance, boolean state);

/*=============================================================================
FUNCTION:      HAL_hsusb_GetSysClkSw
=============================================================================*/
/**
Reports whether the USB core is voting for USB_SYSTEM_CLK.

@note This function is valid for HS-USB hardware version 7 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
TRUE  -- The USB core is voting for USB_SYSTEM_CLK.\n
FALSE -- The USB core is not voting for USB_SYSTEM_CLK.
*/
boolean HAL_hsusb_GetSysClkSw(uint8 instance);

/*=============================================================================
FUNCTION      HAL_hsusb_SetSysClkSw
=============================================================================*/
/**
Enables/disables the USB core for/from voting for USB_SYSTEM_CLK.

@note This function is valid for HS-USB hardware version 7 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- Determines the requested action. Values are:
                        - TRUE -- Enable the USB core to vote for
						          USB_SYSTEM_CLK.
                        - FALSE -- Disable the USB core from voting for
                              USB_SYSTEM_CLK.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
None.
*/
void    HAL_hsusb_SetSysClkSw(uint8 instance, boolean state);

/*=============================================================================
FUNCTION:      HAL_hsusb_GetResumeEndInterrupt
=============================================================================*/
/**
Reports whether the port change interrupt at Resume End in Host mode is
enabled.

@note This function is valid for HS-USB hardware version 7 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
TRUE  -- The port change interrupt at Resume End in Host mode is enabled.\n
FALSE -- The port change interrupt at Resume End in Host mode is disabled.
*/
boolean HAL_hsusb_GetResumeEndInterrupt(uint8 instance);

/*=============================================================================
FUNCTION      HAL_hsusb_SetResumeEndInterrupt
=============================================================================*/
/**
Enables/disables the port change interrupt at Resume End in Host mode.

@note This function is valid for HS-USB hardware version 7 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- Determines the requested action. Values are:
                        - TRUE -- Enable the port change interrupt at Resume
						          End in Host mode.
                        - FALSE -- Disable the port change interrupt at Resume
						           End in Host mode.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
None.
*/
void    HAL_hsusb_SetResumeEndInterrupt(uint8 instance, boolean state);

/*=============================================================================
FUNCTION:      HAL_hsusb_GetSuspendWrongOpmode
=============================================================================*/
/**
Reports whether the USB core asserts opmode with the wrong value when the core
enters a Suspend state.

@note This function is valid for HS-USB hardware version 7 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
TRUE  -- The USB core does not assert opmode with the wrong value when the core
         enters a Suspend state.\n
FALSE -- The USB core asserts opmode with the wrong value when the core enters
         a Suspend state.
*/
boolean HAL_hsusb_GetSuspendWrongOpmode(uint8 instance);

/*=============================================================================
FUNCTION      HAL_hsusb_SetSuspendWrongOpmode
=============================================================================*/
/**
Enables/disables the USB core to assert opmode with the wrong value when the
core enters a Suspend state.

@note This function is valid for HS-USB hardware version 7 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- Determines the requested action. Values are:
                        - TRUE -- Disable the USB core from asserting opmode
						          with the wrong value when the core enters a
								  Suspend state.
                        - FALSE -- Enable the USB core to assert opmode with
						           the wrong value when the core enters a
								   Suspend state.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
None.
*/
void    HAL_hsusb_SetSuspendWrongOpmode(uint8 instance, boolean state);

/*=============================================================================
FUNCTION      HAL_hsusb_GetNoSofRxFifoFull
=============================================================================*/
/**
Returns TRUE if the Rx FIFO is not full, or FALSE when the Rx FIFO is full
and the USB core can start generating SOF.

@note This function is valid for HS-USB hardware version 7 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
TRUE  -- The Rx FIFO is not full.\n
FALSE -- The Rx FIFO is full and the USB core can start generating SOF.
*/
boolean HAL_hsusb_GetNoSofRxFifoFull(uint8 instance);

/*=============================================================================
FUNCTION      HAL_hsusb_SetNoSofRxFifoFull
=============================================================================*/
/**
Enables/disables whether the USB core will wait until the Rx FIFO is not full
and to generate SOF when it is full. 

@note This function is valid for HS-USB hardware version 7 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- Determines the requested action. Values are:
                        - TRUE -- Disable the USB core from waiting until the
						          Rx FIFO becomes not full and to generate SOF
								  when it is full.
                        - FALSE -- Enable the USB core to wait until the Rx
						           FIFO becomes not full and to generate SOF
								   when it is full.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
None.
*/
void    HAL_hsusb_SetNoSofRxFifoFull(uint8 instance, boolean state);

/*=============================================================================
FUNCTION:      HAL_hsusb_GetIsoInZlpPrime
=============================================================================*/
/**
Reports whether the USB core replies with a zero-length packet before the last
token IN is received, when using ISO IN endpoints with MULT=3 and low bandwidth
system bus access.

@note This function is valid for HS-USB hardware version 7 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
TRUE  -- The USB core replies with a zero-length packet before the last token
         IN is received, when using ISO IN endpoints with MULT=3 and low 
         bandwidth system bus access.\n
FALSE -- The USB USB core does not repliy with a zero-length packet before the
         last token IN is received when using ISO IN endpoints with MULT=3 and
         low bandwidth system bus access.
*/
boolean HAL_hsusb_GetIsoInZlpPrime(uint8 instance);

/*=============================================================================
FUNCTION      HAL_hsusb_SetIsoInZlpPrime
=============================================================================*/
/**
Enables/disables the USB core to reply with a zero-length packet before the
last token IN is received when using ISO IN endpoints with MULT=3 and low
bandwidth system bus access.

@note This function is valid for HS-USB hardware version 7 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- Determines the requested action. Values are:
                        - TRUE -- Enable the USB core to reply with a zero-length
                                  packet before the last token IN is received
								  when using ISO IN endpoints with MULT=3 and
								  low bandwidth system bus access.
                        - FALSE -- Disable the USB core from replying with a
				                   zero-length packet before the last token IN
								   is received when using ISO IN endpoints with
                                   MULT=3 and low bandwidth system bus access.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
None.
*/
void    HAL_hsusb_SetIsoInZlpPrime(uint8 instance, boolean state);

/*=============================================================================
FUNCTION:      HAL_hsusb_GetDatalinePulsingReset
=============================================================================*/
/**
Reports whether the USB core recognizes the attached device's attempts to
perform a Session Request Protocol by using data-line pulsing in Host mode when
VBUS is turned off.

@note This function is valid for HS-USB hardware version 7 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
TRUE  -- The USB core recognizes the attached device's attempts to perform a
         Session Request Protocol by using data-line pulsing in Host mode when
         the VBUS is turned off.\n
FALSE -- The USB core does not recognize the attached device's attempts to
         perform a Session Request Protocol by using data-line pulsing in Host
         mode when the VBUS is turned off.
*/
boolean HAL_hsusb_GetDatalinePulsingReset(uint8 instance);

/*=============================================================================
FUNCTION      HAL_hsusb_SetDatalinePulsingReset
=============================================================================*/
/**
Enables/disables the USB core to recognize the attached device's attempts to
perform a Session Request Protocol by using data-line pulsing in Host mode when
VBUS is turned off for the HS-USB core usb_otg_hs_p3q3r26 implementation.

@note This function is valid for HS-USB hardware version 7 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- Determines the requested action. Values are:
                        - TRUE -- Enable the USB core to recognize the attached
                                  device's attempts to perform a Session
								  Request Protocol by using data-line pulsing
								  in Host mode when the VBUS is turned off.
                        - FALSE -- Disable the USB core from recognizing the
                                   attached device's attempts to perform a
								   Session Request Protocol by using data-line
								   pulsing in Host mode when the VBUS is turned
								   off.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
None.
*/
void    HAL_hsusb_SetDatalinePulsingReset(uint8 instance, boolean state);

/*=============================================================================
FUNCTION:      HAL_hsusb_GetDevicePortChirpKTime
=============================================================================*/
/**
Reports the Device Port Control state Machine Chirp K time.

@note This function is valid for HS-USB hardware version 7 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
TRUE  -- The Device Port Control state machine drives Chirp K for 2 ms.\n
FALSE -- The Device Port Control state machine drives Chirp K for 1 ms.
*/
boolean HAL_hsusb_GetDevicePortChirpKTime(uint8 instance);

/*=============================================================================
FUNCTION      HAL_hsusb_SetDevicePortChirpKTime
=============================================================================*/
/**
Sets the Device Port Control state machine Chirp K time.

@note This function is valid for HS-USB hardware version 7 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- Determines the requested action. Values are:
                        - TRUE -- Set the Device Port Control state machine
						          Chirp K time to 2 ms.
                        - FALSE -- Set the Device Port Control state machine
						           Chirp K time to 1 ms.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
None.
*/
void    HAL_hsusb_SetDevicePortChirpKTime(uint8 instance, boolean state);

/*=============================================================================
FUNCTION:      HAL_hsusb_GetDevicePortPreChirpKTime
=============================================================================*/
/**
Reports the Device Port Control state machine waiting time from USB Reset
detection until it starts driving Chirp K.

@note This function is valid for HS-USB hardware version 7 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return 
TRUE  -- The Device Port Control state machine waits 1.5 ms from USB Reset
         detection until it starts driving Chirp K.\n
FALSE -- The Device Port Control state machine waits 2.5 ms from USB Reset
         detection until it starts driving Chirp K.
*/
boolean HAL_hsusb_GetDevicePortPreChirpKTime(uint8 instance);

/*=============================================================================
FUNCTION      HAL_hsusb_SetDevicePortPreChirpKTime
=============================================================================*/
/**
Sets the Device Port Control state machine waiting time from USB Reset
detection until it starts driving Chirp K.

@note This function is valid for HS-USB hardware version 7 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- Determines the requested action. Values are:
                        - TRUE -- Set the Device Port Control state machine to
						          wait 1.5 ms from USB Reset detection before
								  it starts driving Chirp K.
                        - FALSE -- Set the Device Port Control state machine to
						           wait 2.5 ms from USB Reset detection before
								   it starts driving Chirp K.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
None.
*/
void    HAL_hsusb_SetDevicePortPreChirpKTime(uint8 instance, boolean state);

/*=============================================================================
FUNCTION:      HAL_hsusb_GetSessVldControl
=============================================================================*/
/**
Reports whether the Link Controller sess_vld signal is controlled by bit 25 of
the USBCMD register or if it is received from the PHY.

@note This function is valid for HS-USB hardware version 7 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
TRUE  -- The Link Controller sess_vld signal is controlled by bit 25 of the
         USBCMD register.\n
FALSE -- The Link Controller sess_vld signal is received from the PHY.
*/
boolean HAL_hsusb_GetSessVldControl(uint8 instance);

/*=============================================================================
FUNCTION      HAL_hsusb_SetSessVldControl
=============================================================================*/
/**
Sets the Link Controller sess_vld signal control.

@note This function is valid for HS-USB hardware version 7 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- Determines the requested action. Values are:
                        - TRUE -- Set the Link Controller sess_vld signal to be
                                  controlled by bit 25 of the USBCMD register.
                        - FALSE -- Set the Link Controller sess_vld signal to
						           be received from the PHY.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
None.
*/
void    HAL_hsusb_SetSessVldControl(uint8 instance, boolean state);

/*=============================================================================
FUNCTION      HAL_hsusb_GetULPIResetState
=============================================================================*/
/**
This function gets the reset state of the ULPI-to-UTMI bridge.

@note This function is valid for HS-USB hardware version 8 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return 
TRUE  -- ULPI-to-UTMI bridge is in reset state.
FALSE -- ULPI-to-UTMI bridge is not in reset state.
*/
boolean HAL_hsusb_GetULPIResetState(uint8 instance);

/*=============================================================================
FUNCTION      HAL_hsusb_SetULPIResetState
=============================================================================*/
/**
Sets the reset state of the ULPI-to-UTMI bridge.

@note This function is valid for HS-USB hardware version 8 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- Determines the requested action. Values are:
                        - TRUE -- Put ULPI-to-UTMI bridge in reset state.
                        - FALSE -- Take the ULPI-to-UTMI bridge out of reset state.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
None.
*/
void HAL_hsusb_SetULPIResetState(uint8 instance, boolean state);

/*=============================================================================
FUNCTION      HAL_hsusb_GetBurnInTestEnable
=============================================================================*/
/**
This function gets the burn-in test enable bit state.

@note This function is valid for HS-USB hardware version 8 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return 
TRUE  -- Burn-in test is enabled.
FALSE -- Burn-in test is disabled.
*/
boolean HAL_hsusb_GetBurnInTestEnable(uint8 instance);

/*=============================================================================
FUNCTION      HAL_hsusb_SetBurnInTestEnable
=============================================================================*/
/**
This function sets the burn-in test enable bit state.

@note This function is valid for HS-USB hardware version 8 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- Determines the requested action. Values are:
                        - TRUE -- Enable burn-in test.
                        - FALSE -- Disable burn-in test.
@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
None.
*/
void HAL_hsusb_SetBurnInTestEnable(uint8 instance, boolean state);

/*=============================================================================
FUNCTION      HAL_hsusb_GetIDDQTestEnable
=============================================================================*/
/**
This function gets the IDDQ test enable bit state.

@note This function is valid for HS-USB hardware version 8 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return 
TRUE  -- IDDQ test is enabled.
FALSE -- IDDQ test is disabled.
*/
boolean HAL_hsusb_GetIDDQTestEnable(uint8 instance);

/*=============================================================================
FUNCTION      HAL_hsusb_SetIDDQTestEnable
=============================================================================*/
/**
Set or clear the IDDQ test enable bit.

@note This function is valid for HS-USB hardware version 8 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- Determines the requested action. Values are:
                        - TRUE -- Enable IDDQ test.
                        - FALSE -- Disable IDDQ test.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
None.
*/
void HAL_hsusb_SetIDDQTestEnable(uint8 instance, boolean state);

/* ===========================================================================
**
**  HAL_hsusb_GetRetentionModeState
**
**  Description:
**    This function gets the PHY retention mode state.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 8 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**               This value should be at least one less than the number
**               reported by the HAL_hsusb_SupportedCoresQty() function.
**
**  Return:
**    TRUE    - PHY is in retention mode.
**    FALSE   - PHY is not in retention mode.
**
** ======================================================================== */
boolean HAL_hsusb_GetRetentionModeState(uint8 instance);

/* ===========================================================================
**
**  HAL_hsusb_SetRetentionModeState
**
**  Description:
**    This function sets or clears IDDQ test enable bit.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 8 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**               This value should be at least one less than the number
**               reported by the HAL_hsusb_SupportedCoresQty() function.
**    state    - Determines the retention mode state.
**               Values are:
**                 TRUE  - Enter the PHY into retention mode.
**                 FALSE - Exit the PHY from retention mode.
**
**  Return:
**    None.
**
** ======================================================================== */
void HAL_hsusb_SetRetentionModeState(uint8 instance, boolean state);

/* ===========================================================================
**
**  HAL_hsusb_GetPHYResetState
**
**  Description:
**    This function gets the PHY retention mode state.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 8 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**               This value should be at least one less than the number
**               reported by the HAL_hsusb_SupportedCoresQty() function.
**
**  Return:
**    TRUE    - PHY is in retention mode.
**    FALSE   - PHY is not in retention mode.
**
** ======================================================================== */
boolean HAL_hsusb_GetPHYResetState(uint8 instance);

/* ===========================================================================
**
**  HAL_hsusb_SetPHYResetState
**
**  Description:
**    This function the PHY in and out of reset state.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 8 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**               This value should be at least one less than the number
**               reported by the HAL_hsusb_SupportedCoresQty() function.
**    state    - Determines the reset state.
**               Values are:
**                 TRUE  - Enter the PHY into reset state.
**                 FALSE - Exit the PHY from reset state.
**
**  Return:
**    None.
**
** ======================================================================== */
void    HAL_hsusb_SetPHYResetState(uint8 instance, boolean state);

/* ===========================================================================
**
**  HAL_hsusb_GetPHYRefClkFrequency
**
**  Description:
**    This function gets the PHY reference clock frequency selection.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 8 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**               This value should be at least one less than the number
**               reported by the HAL_hsusb_SupportedCoresQty() function.
**
**  Return:
**    PHY reference clock frequency selection.
**
** ======================================================================== */
uint8 HAL_hsusb_GetPHYRefClkFrequency(uint8 instance);

/* ===========================================================================
**
**  HAL_hsusb_SetPHYRefClkFrequency
**
**  Description:
**    This function sets the PHY reference clock frequency selection.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 8 and above.
**
**  Parameters:
**    instance   - Determines which HS-USB core to use. 
**                 This value should be at least one less than the number
**                 reported by the HAL_hsusb_SupportedCoresQty() function.
**    fsel_value - PHY Reference clock frequency selection.
**
**  Return:
**    None.
**
** ======================================================================== */
void HAL_hsusb_SetPHYRefClkFrequency(uint8 instance, uint8 fsel_value);

/* ===========================================================================
**
**  HAL_hsusb_GetIDHVIntEnState
**
**  Description:
**    This function gets the High-Voltage ID interrupt enable state.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 8 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**               This value should be at least one less than the number
**               reported by the HAL_hsusb_SupportedCoresQty() function.
**
**  Return:
**    TRUE    - High-Voltage ID interrupt is enabled.
**    FALSE   - High-Voltage ID interrupt is disabled.
**
** ======================================================================== */
boolean HAL_hsusb_GetIDHVIntEnState(uint8 instance);

/* ===========================================================================
**
**  HAL_hsusb_SetIDHVIntEnState
**
**  Description:
**    This function sets the High-Voltage ID interrupt enable state.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 8 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**               This value should be at least one less than the number
**               reported by the HAL_hsusb_SupportedCoresQty() function.
**
**    state    - Determines the ID HV interrupt enable state.
**               Values are:
**                 TRUE  - Interrupt is enabled.
**                 FALSE - Interrupt is disabled.
**
**  Return: None.
**
** ======================================================================== */
void HAL_hsusb_SetIDHVIntEnState(uint8 instance, boolean state);

/* ===========================================================================
**
**  HAL_hsusb_GetOTGSessVldHVIntEnState
**
**  Description:
**    This function gets the High-Voltage OTG Session-Valid interrupt 
**    enable state.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 8 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**               This value should be at least one less than the number
**               reported by the HAL_hsusb_SupportedCoresQty() function.
**
**  Return:
**    TRUE    - High-Voltage OTG Session-Valid interrupt is enabled.
**    FALSE   - High-Voltage OTG Session-Valid interrupt is disabled.
**
** ======================================================================== */
boolean HAL_hsusb_GetOTGSessVldHVIntEnState(uint8 instance);

/* ===========================================================================
**
**  HAL_hsusb_SetOTGSessVldHVIntEnState
**
**  Description:
**    This function sets the High-Voltage OTG Session-Valid interrupt 
**    enable state.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 8 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**               This value should be at least one less than the number
**               reported by the HAL_hsusb_SupportedCoresQty() function.
**
**    state    - Determines the session-valid HV interrupt enable state.
**               Values are:
**                 TRUE  - Interrupt is enabled.
**                 FALSE - Interrupt is disabled.
**
** ======================================================================== */
void HAL_hsusb_SetOTGSessVldHVIntEnState(uint8 instance, boolean state);

/* ===========================================================================
**
**  HAL_hsusb_GetMPMHVClampEnState
**
**  Description:
**    This function gets the High-Voltage interrupts MPM clamp enable state. 
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 8 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**               This value should be at least one less than the number
**               reported by the HAL_hsusb_SupportedCoresQty() function.
**
**  Return:
**    TRUE    - MPM High-Voltage clamp enable bit is set.
**    FALSE   - MPM High-Voltage clamp enable bit is cleared.
**
** ======================================================================== */
boolean HAL_hsusb_GetMPMHVClampEnState(uint8 instance);

/* ===========================================================================
**
**  HAL_hsusb_SetMPMHVClampEnState
**
**  Description:
**    This function sets the High-Voltage interrupts MPM clamp enable state. 
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 8 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**               This value should be at least one less than the number
**               reported by the HAL_hsusb_SupportedCoresQty() function.
**
**    state    - Determines the High-Voltage interrupts MPM clamp enable  state.
**               Values are:
**                 TRUE  - High-Voltage interrupts MPM clamp is enabled.
**                 FALSE - High-Voltage interrupts MPM clamp is disabled.
**
** ======================================================================== */
void HAL_hsusb_SetMPMHVClampEnState(uint8 instance, boolean state);

/* ===========================================================================
**
**  HAL_hsusb_GetOTGSessionVldHVClampEnStateImplp3q3r28
**
**  Description:
**    This function gets the High-Voltage OTG Session-Valid interrupt 
**    clamp enable state. 
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 8 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**               This value should be at least one less than the number
**               reported by the HAL_hsusb_SupportedCoresQty() function.
**
**  Return:
**    TRUE    - MPM High-Voltage clamp enable bit is set.
**    FALSE   - MPM High-Voltage clamp enable bit is cleared.
**
** ======================================================================== */
boolean HAL_hsusb_GetOTGSessionVldHVClampEnState(uint8 instance);

/* ===========================================================================
**
**  HAL_hsusb_SetOTGSessionVldHVClampEnState
**
**  Description:
**    This function sets the High-Voltage OTG Session-Valid interrupt 
**    clamp enable state. 
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 8 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**               This value should be at least one less than the number
**               reported by the HAL_hsusb_SupportedCoresQty() function.
**
**    state    - Determines the OTG Session-Valid High-Voltage interrupt 
**               clamp enable state.
**               Values are:
**                 TRUE  - OTG Session-Valid High-Voltage interrupt clamp 
**                         is enabled.
**                 FALSE - OTG Session-Valid High-Voltage interrupt clamp 
**                         is disabled.
**
** ======================================================================== */
void 
HAL_hsusb_SetOTGSessionVldHVClampEnState(uint8 instance, boolean state);

/* ===========================================================================
**
**  HAL_hsusb_GetIDHVClampEnState
**
**  Description:
**    This function gets the High-Voltage OTG Session-Valid interrupt 
**    clamp enable state. 
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 8 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**               This value should be at least one less than the number
**               reported by the HAL_hsusb_SupportedCoresQty() function.
**
**  Return:
**    TRUE    - MPM High-Voltage clamp enable bit is set.
**    FALSE   - MPM High-Voltage clamp enable bit is cleared.
**
** ======================================================================== */
boolean HAL_hsusb_GetIDHVClampEnState(uint8 instance);

/* ===========================================================================
**
**  HAL_hsusb_SetIDHVClampEnState
**
**  Description:
**    This function sets the High-Voltage ID interrupt clamp enable state. 
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 8 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**               This value should be at least one less than the number
**               reported by the HAL_hsusb_SupportedCoresQty() function.
**
**    state    - Determines the ID High-Voltage interrupt 
**               clamp enable state.
**               Values are:
**                 TRUE  - ID High-Voltage interrupt clamp is enabled.
**                 FALSE - ID High-Voltage interrupt clamp is disabled.
**
** ======================================================================== */
void HAL_hsusb_SetIDHVClampEnState(uint8 instance, boolean state);

/* ===========================================================================
**
**  HAL_hsusb_GetScratchRAMn
**
**  Description:
**    This function gets the contents of the scratch registers.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 8 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**               This value should be at least one less than the number
**               reported by the HAL_hsusb_SupportedCoresQty() function.
**    n        - The index of the scratch register (0 .. 15).
**
**  Return:
**    The contents of the scratch register.
**    
**
** ======================================================================== */
uint32 HAL_hsusb_GetScratchRAMn(uint8 instance, uint8 n);

/* ===========================================================================
**
**  HAL_hsusb_SetScratchRAMn
**
**  Description:
**    This function gets the contents of the scratch registers.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 8 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**               This value should be at least one less than the number
**               reported by the HAL_hsusb_SupportedCoresQty() function.
**    n        - The index of the scratch register (0 .. 15).
**    data     - Data to be written to the scratch register.
**
**  Return: 
**    LastError is set 
**
** ======================================================================== */
void HAL_hsusb_SetScratchRAMn(uint8 instance, uint8 n, uint32 data);

/* ===========================================================================
**
**  HAL_hsusb_GetUSBHsTxDepth
**
**  Description:
**    This function gets the USB Transmit depth value.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 8 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**               This value should be at least one less than the number
**               reported by the HAL_hsusb_SupportedCoresQty() function.
**
**  Return:
**    Current USB Transmit depth value.
**
** ======================================================================== */
uint16 HAL_hsusb_GetUSBHsTxDepth(uint8 instance);

/* ===========================================================================
**
**  HAL_hsusb_GetUSBHsRxDepth
**
**  Description:
**    This function gets the USB receive depth value.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 8 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**               This value should be at least one less than the number
**               reported by the HAL_hsusb_SupportedCoresQty() function.
**
**  Return:
**    Current USB receive depth value.
**
** ======================================================================== */
uint16 HAL_hsusb_GetUSBHsRxDepth(uint8 instance);

/* ===========================================================================
**
**  HAL_hsusb_GetUTMIPhySwIfEn
**
**  Description:
**    This function gets the ULPI register interface enable state. 
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 8 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**               This value should be at least one less than the number
**               reported by the HAL_hsusb_SupportedCoresQty() function.
**
**  Return:
**    TRUE    - ULPI register interface is enabled.
**    FALSE   - ULPI register interface is disabled.
**
** ======================================================================== */
boolean HAL_hsusb_GetUTMIPhySwIfEn(uint8 instance);

/* ===========================================================================
**
**  HAL_hsusb_GetUseHSIC
**
**  Description:
**    This function gets the use HSIC bit state. 
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 8 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**               This value should be at least one less than the number
**               reported by the HAL_hsusb_SupportedCoresQty() function.
**
**  Return:
**    TRUE    - HSIC interface enabled.
**    FALSE   - HSIC interface is disabled.
**
** ======================================================================== */
boolean HAL_hsusb_GetUseHSIC(uint8 instance);

/* ===========================================================================
**
**  HAL_hsusb_GetUseSPSImplp3q3r28
**
**  Description:
**    This function gets the use SPS bit state. 
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 8 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**               This value should be at least one less than the number
**               reported by the HAL_hsusb_SupportedCoresQty() function.
**
**  Return:
**    TRUE    - SPS interface enabled.
**    FALSE   - SPS interface is disabled.
**
** ======================================================================== */
boolean HAL_hsusb_GetUseSPS(uint8 instance);

/* ===========================================================================
**
**  HAL_hsusb_GetMaxPipes
**
**  Description:
**    This function gets the max pipes bit state. 
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 8 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**               This value should be at least one less than the number
**               reported by the HAL_hsusb_SupportedCoresQty() function.
**
**  Return:
**    The USB core's max pipes value.
**
** ======================================================================== */
uint8 HAL_hsusb_GetMaxPipes(uint8 instance);

/* ===========================================================================
**
**  HAL_hsusb_GetUSBHSDevEP
**
**  Description:
**    This function gets the max pipes bit state. 
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 8 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**               This value should be at least one less than the number
**               reported by the HAL_hsusb_SupportedCoresQty() function.
**
**  Return:
**    The USB core's number of endpoints.
**
** ======================================================================== */
uint8 HAL_hsusb_GetUSBHSDevEP(uint8 instance);

/* ===========================================================================
**
**  HAL_hsusb_GetULPISerialEn
**
**  Description:
**    This function gets the ULPI serial mode enable bit state.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 8 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**               This value should be at least one less than the number
**               reported by the HAL_hsusb_SupportedCoresQty() function.
**
**  Return:
**    TRUE    - ULPI serial mode is enabled.
**    FALSE   - ULPI serial mode is disabled.
**
** ======================================================================== */
boolean HAL_hsusb_GetULPISerialEn(uint8 instance);

/* ===========================================================================
**
**  HAL_hsusb_SetULPISerialEn
**
**  Description:
**    This function sets the ULPI serial mode enable bit state.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 8 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**               This value should be at least one less than the number
**               reported by the HAL_hsusb_SupportedCoresQty() function.
**
**    state    - Determines the ULPI serial mode enable state.
**               Values are:
**                 TRUE  - Enable ULPI serial mode.
**                 FALSE - Disable ULPI serial mode.
**
** ======================================================================== */
void HAL_hsusb_SetULPISerialEn(uint8 instance, boolean state);

/* ===========================================================================
**
**  HAL_hsusb_GetScratchRAMEn
**
**  Description:
**    This function gets the scratch RAM enable bit state.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 8 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**               This value should be at least one less than the number
**               reported by the HAL_hsusb_SupportedCoresQty() function.
**
**  Return:
**    TRUE    - Scratch RAM is enabled.
**    FALSE   - Scratch RAM is disabled.
**
** ======================================================================== */
boolean HAL_hsusb_GetScratchRAMEn(uint8 instance);

/* ===========================================================================
**
**  HAL_hsusb_SetScratchRAMEn
**
**  Description:
**    This function sets the scratch RAM enable bit state.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 8 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**               This value should be at least one less than the number
**               reported by the HAL_hsusb_SupportedCoresQty() function.
**
**    state    - Determines the scratch RAM enable state.
**               Values are:
**                 TRUE  - Enable scratch RAM.
**                 FALSE - Disable scratch RAM.
**
** ======================================================================== */
void HAL_hsusb_SetScratchRAMEn(uint8 instance, boolean state);

/* ===========================================================================
**
**  HAL_hsusb_GetSessVldCtrlEn
**
**  Description:
**    This function gets the session-valid control enable bit state.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 8 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**               This value should be at least one less than the number
**               reported by the HAL_hsusb_SupportedCoresQty() function.
**
**  Return:
**    TRUE    - Session-valid control is enabled.
**    FALSE   - Session-valid control is disabled.
**
** ======================================================================== */
boolean HAL_hsusb_GetSessVldCtrlEn(uint8 instance);

/* ===========================================================================
**
**  HAL_hsusb_SetSessVldCtrlEn
**
**  Description:
**    This function sets the session-valid control enable bit state.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 8 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**               This value should be at least one less than the number
**               reported by the HAL_hsusb_SupportedCoresQty() function.
**
**    state    - Determines the OTG Session-Valid control enable state.
**               Values are:
**                 TRUE  - Enable session-valid control.
**                 FALSE - Disable session-valid control.
**
** ======================================================================== */
void HAL_hsusb_SetSessVldCtrlEn(uint8 instance, boolean state);

/* ===========================================================================
**
**  HAL_hsusb_GetPhyAltInt
**
**  Description:
**    This function gets the non-USB interrupt status.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 8 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**               This value should be at least one less than the number
**               reported by the HAL_hsusb_SupportedCoresQty() function.
**
**  Return:
**    TRUE    - Non-USB interrupt is bit set.
**    FALSE   - Non-USB interrupt is bit cleared.
**
** ======================================================================== */
boolean HAL_hsusb_GetPhyAltInt(uint8 instance);

/* ===========================================================================
**
**  HAL_hsusb_ClearPhyAltInt
**
**  Description:
**    This function clears the non-USB interrupt bit.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 8 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**               This value should be at least one less than the number
**               reported by the HAL_hsusb_SupportedCoresQty() function.
**
**
** ======================================================================== */
void HAL_hsusb_ClearPhyAltInt(uint8 instance);

/* ===========================================================================
**
**  HAL_hsusb_GetULPIForce
**
**  Description:
**    This function gets the ULPI read\write force bit state.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 8 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**               This value should be at least one less than the number
**               reported by the HAL_hsusb_SupportedCoresQty() function.
**
**  Return:
**    TRUE    - ULPI force read\write is enabled.
**    FALSE   - ULPI force read\write is disabled.
**
** ======================================================================== */
boolean HAL_hsusb_GetULPIForce(uint8 instance);

/* ===========================================================================
**
**  HAL_hsusb_SetULPIForce
**
**  Description:
**    This function sets the ULPI read\write force bit state
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 8 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**               This value should be at least one less than the number
**               reported by the HAL_hsusb_SupportedCoresQty() function.
**
**    state    - Determines the ULPI force read\write state.
**               Values are:
**                 TRUE  - Enable ULPI force read\write.
**                 FALSE - Disable ULPI force read\write.
**
** ======================================================================== */
void HAL_hsusb_SetULPIForce(uint8 instance, boolean state);


/*=============================================================================
FUNCTION:      HAL_hsusb_GetTxEpPrimeL1Exit
=============================================================================*/
/**
Reports whether remote wakeup in L1 state when SW starts priming specific
endpoint is enabled.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param ep       [IN] -- The endpoint number

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
TRUE  -- Remote wakeup when SW starts priming specific endpoint is enabled
FALSE -- Remote wakeup when SW starts priming specific endpoint is disabled
*/
boolean HAL_hsusb_GetTxEpPrimeL1Exit(uint8 instance, uint8 ep);

/*=============================================================================
FUNCTION:      HAL_hsusb_SetTxEpPrimeL1Exit
=============================================================================*/
/**
Sets whether remote wakeup in L1 state when SW starts priming specific
endpoint is enabled.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param ep       [IN] -- The endpoint number
@param state    [IN] -- TRUE enables remote wakeup as SW starts priming specific endpoint. 
                        FALSE disables remote wakeup as SW starts priming specific endpoint.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
None.
*/
void HAL_hsusb_SetTxEpPrimeL1Exit(uint8 instance, uint8 ep, uint8 state);
    
/*=============================================================================
FUNCTION:      HAL_hsusb_GetTxEpPrimeL1Enable
=============================================================================*/
/**
Reports whether the transition to L1 state can occur when a specific endpoint
is primed

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param ep       [IN] -- The endpoint number

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
TRUE  -- Refrain from transitioning to L1 state whenever the particular TX
         endpoint is primed
FALSE -- Transition to L1 state is allowed even if the particular TX endpoint
         is primed
*/
boolean HAL_hsusb_GetTxEpPrimeL1Enable(uint8 instance, uint8 ep);

/*=============================================================================
FUNCTION:      HAL_hsusb_SetTxEpPrimeL1Enable
=============================================================================*/
/**
Sets whether the transition to L1 state can occur when a specific endpoint
is primed

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param ep       [IN] -- The endpoint number
@param state    [IN] -- TRUE to refrain from transitioning to L1 state whenever
                        the particular TX endpoing is primed.
                        FALSE to allow transition to L1 state even when the 
                        particular endpoint is primed 

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
None.
*/
void HAL_hsusb_SetTxEpPrimeL1Enable(uint8 instance, uint8 ep, boolean state);

/*=============================================================================
FUNCTION:      HAL_hsusb_GetL1PllPwrDwnEnable
=============================================================================*/
/**
Reports whether power down of 480 MHz PLL in L1 state is enabled

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
TRUE  -- Power down of 480 MHz PLL in L1 state is enabled
FALSE -- Power down of 480 MHz PLL in L1 state is disabled
*/
boolean HAL_hsusb_GetL1PllPwrDwnEnable(uint8 instance);

/*=============================================================================
FUNCTION:      HAL_hsusb_SetL1PllPwrDwnEnable
=============================================================================*/
/**
Sets whether power down of 480 MHz PLL in L1 state is enabled

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- TRUE to enable
                        FALSE to disable

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
TRUE  -- Power down of 480 MHz PLL in L1 state is enabled
FALSE -- Power down of 480 MHz PLL in L1 state is disabled
*/
void HAL_hsusb_SetL1PllPwrDwnEnable(uint8 instance, boolean state);


/*=============================================================================
FUNCTION:      HAL_hsusb_GetL1PhyLPMEnable
=============================================================================*/
/**
Reports whether ULPI Low Power Mode is enabled in L1 state

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
TRUE  -- ULPI Low Power Mode in L1 state is enabled
FALSE -- ULPI Low Power Mode in L1 state is disabled 
*/
boolean HAL_hsusb_GetL1PhyLPMEnable(uint8 instance);

/*=============================================================================
FUNCTION:      HAL_hsusb_SetL1PhyLPMEnable
=============================================================================*/
/**
Sets whether ULPI Low Power Mode is enabled in L1 state

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- TRUE to enable
                        FALSE to disable

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
None.
*/
void HAL_hsusb_SetL1PhyLPMEnable(uint8 instance, boolean state);

/*=============================================================================
FUNCTION:      HAL_hsusb_GetL1GateAhbClkEnable
=============================================================================*/
/**
Reports whether clock request signaling is enabled for usb_ahb_clk in L1 state


@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
TRUE  -- Clock request signaling is enabled for usb_ahb_clk in L1 state is enabled
FALSE -- Clock request signaling is enabled for usb_ahb_clk in L1 state disabled 
*/
boolean HAL_hsusb_GetL1GateAhbClkEnable(uint8 instance);

/*=============================================================================
FUNCTION:      HAL_hsusb_SetL1GateAhbClkEnable
=============================================================================*/
/**
Sets whether clock request signaling is enabled for usb_ahb_clk in L1 state

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- TRUE to enable
                        FALSE to disable

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
None.
*/
void HAL_hsusb_SetL1GateAhbClkEnable(uint8 instance, boolean state);

/*=============================================================================
FUNCTION:      HAL_hsusb_GetL1GateFsXcvrClkEnable
=============================================================================*/
/**
Reports whether clock gating of usb_fs_xcvr_clk in L1 state is enabled

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return

TRUE  -- Clock gating of usb_fs_xcvr_clk in L1 state is enabled
FALSE -- Clock gating of usb_fs_xcvr_clk in L1 state is disabled
*/
boolean HAL_hsusb_GetL1GateFsXcvrClkEnable(uint8 instance);

/*=============================================================================
FUNCTION:      HAL_hsusb_SetL1GateFsXcvrClkEnable
=============================================================================*/
/**
Sets whether clock gating of usb_fs_xcvr_clk in L1 state is enabled

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- TRUE to enable
                        FALSE to disable

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
None.
*/
void HAL_hsusb_SetL1GateFsXcvrClkEnable(uint8 instance, boolean state);

/*=============================================================================
FUNCTION:      HAL_hsusb_GetL1GateSysClkEnable
=============================================================================*/
/**
Reports whether clock gating of usb_system_clk in L1 state is enabled


@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return

TRUE  -- Clock gating of usb_system_clk in L1 state is enabled
FALSE -- Clock gating of usb_system_clk in L1 state is disabled
*/
boolean HAL_hsusb_GetL1GateSysClkEnable(uint8 instance);

/*=============================================================================
FUNCTION:      HAL_hsusb_SetL1GateSysClkEnable
=============================================================================*/
/**
Sets whether clock gating of usb_system_clk in L1 state is enabled

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- TRUE to enable
                        FALSE to disable

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
None.
*/
void HAL_hsusb_SetL1GateSysClkEnable(uint8 instance, boolean state);

/*=============================================================================
FUNCTION:      HAL_hsusb_GetL1GateXcvrClkEnable
=============================================================================*/
/**
Reports whether clock gating of usb_xcvr_clk in L1 state is enabled

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return

TRUE  -- Clock gating of usb_xcvr_clk in L1 state is enabled
FALSE -- Clock gating of usb_xcvr_clk in L1 state is disabled
*/
boolean HAL_hsusb_GetL1GateXcvrClkEnable(uint8 instance);

/*=============================================================================
FUNCTION:      HAL_hsusb_SetL1GateXcvrClkEnable
=============================================================================*/
/**
Sets whether clock gating of usb_xcvr_clk in L1 state is enabled

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- TRUE to enable
                        FALSE to disable

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
None.
*/
void HAL_hsusb_SetL1GateXcvrClkEnable(uint8 instance, boolean state);

/*=============================================================================
FUNCTION:      HAL_hsusb_GetL1RemoteWakeupEnable
=============================================================================*/
/**
Reports whether Remote Wakeup in L1 state is enabled

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return

TRUE  -- Link controller can initiate Remote Wakeup.
FALSE -- Link controller never initiates Remote Wakeup in L1 state.
*/
boolean HAL_hsusb_GetL1RemoteWakeupEnable(uint8 instance);

/*=============================================================================
FUNCTION:      HAL_hsusb_SetL1RemoteWakeupEnable
=============================================================================*/
/**
Sets whether Remote Wakeup in L1 state is enabled

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- TRUE for link controller can initiate Remote Wakeup.
                        FALSE for link controller never initiates Remote Wakeup in L1 state.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
None.
*/
void HAL_hsusb_SetL1RemoteWakeupEnable(uint8 instance, boolean state);

/*=============================================================================
FUNCTION:      HAL_hsusb_GetL1LPMEnable
=============================================================================*/
/**
Reports whether Link Power Management Support is enabled

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return

TRUE  -- Link Power Management support is enabled
FALSE -- Link Power Management support is disabled. No response to LPM extended
         transactions.
*/
boolean HAL_hsusb_GetL1LPMEnable(uint8 instance);

/*=============================================================================
FUNCTION:      HAL_hsusb_SetL1LPMEnable
=============================================================================*/
/**
Sets whether Link Power Management Support is enabled

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- TRUE for enabling Link Power Management
                        FALSE for disabling Link Power Management and disable
                        hardware response to LPM extended transactions.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
None.
*/
void HAL_hsusb_SetL1LPMEnable(uint8 instance, boolean state);

/*=============================================================================
FUNCTION:      HAL_hsusb_GetL1PllTurnoffMinHird
=============================================================================*/
/**
Reports the minimum expected HIRD value from host that enables HW mechanism for
turning off the 480 MHz PLL

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
Default value is 50us.
*/
uint8 HAL_hsusb_GetL1PllTurnoffMinHird(uint8 instance);

/*=============================================================================
FUNCTION:      HAL_hsusb_SetL1PllTurnoffMinHird
=============================================================================*/
/**
Sets the minimum expected HIRD value from host that enables HW mechanism for
turning off the 480 MHz PLL

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param minHIRD [IN]  -- Minimum expected HIRD value to use

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
None.
*/
void HAL_hsusb_SetL1PllTurnoffMinHird(uint8 instance, uint8 minHIRD);

/*=============================================================================
FUNCTION:      HAL_hsusb_GetL1LongEntryCount
=============================================================================*/
/**
Reports the count of number of exits from L1 where duration in L1 is > 200us

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
Count of number of exits from L1 where duration in L1 is > 200us
*/
uint16 HAL_hsusb_GetL1LongEntryCount(uint8 instance);

/*=============================================================================
FUNCTION:      HAL_hsusb_GetL1ShortEntryCount
=============================================================================*/
/**
Reports the count of number of exits from L1 where duration in L1 is <= 200us

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
Count of number of exits from L1 where duration in L1 is <= 200us
*/
uint16 HAL_hsusb_GetL1ShortEntryCount(uint8 instance);

/*=============================================================================
FUNCTION:      HAL_hsusb_ClearL1EntryCounts
=============================================================================*/
/**
Clears the L1 entry counters

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
None.
*/
void HAL_hsusb_ClearL1EntryCounts(uint8 instance);


/*=============================================================================
FUNCTION:      HAL_hsusb_GetL1RemoteWakeupTime
=============================================================================*/
/**
Reports the duration that the device drives remote wakeup

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
The duration that the device drives remote wakeup, defaults to 50us.
*/
uint8 HAL_hsusb_GetL1RemoteWakeupTime(uint8 instance);

/*=============================================================================
FUNCTION:      HAL_hsusb_SetL1RemoteWakeupTime
=============================================================================*/
/**
Sets the duration that the device drives remote wakeup

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param timeInUS [IN] -- Duration that the device should drive remote wakeup                       

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
None.
*/
void HAL_hsusb_SetL1RemoteWakeupTime(uint8 instance, uint8 timeInUS);

/*=============================================================================
FUNCTION:      HAL_hsusb_GetL1FPR
=============================================================================*/
/**
Reports whether L1 force port resume is currently active

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
TRUE  -- Software is driving force port resume
FALSE -- Software is not driving force port resume 
*/
boolean HAL_hsusb_GetL1FPR(uint8 instance);

/*=============================================================================
FUNCTION:      HAL_hsusb_SetL1FPR
=============================================================================*/
/**
Sets whether L1 force port resume is currently active

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state [IN]    -- TRUE for software driving force port resume signaling

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
None.
*/
void HAL_hsusb_SetL1FPR(uint8 instance, boolean state);

/*=============================================================================
FUNCTION:      HAL_hsusb_GetHSICClkPllBypassNL
=============================================================================*/
/**
Disables PLL analog logic. Active low. Connects to bypassnl
input of NT_PLL.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
TRUE  -- PLL analog logic is not disabled
FALSE -- PLL analog logic is disabled
*/
boolean HAL_hsusb_GetHSICClkPllBypassNL(uint8 instance);

/*=============================================================================
FUNCTION:      HAL_hsusb_GetHSICClkPllReset
=============================================================================*/
/**
Resets all FF in PLL. Active low. Connects to reset_n input of
NT_PLL

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
TRUE  -- FF in PLL is not resetting
FALSE -- FF in PLL is resetting
*/
boolean HAL_hsusb_GetHSICClkPllReset(uint8 instance);

/*=============================================================================
FUNCTION:      HAL_hsusb_GetHSICClkGate
=============================================================================*/
/**
Clock gating of hsic_clk and ulpi_clk, without turning off HSIC
PLL

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
TRUE  -- Clock gating of hsic_clk and ulpi_clk are active
FALSE -- Clock gating of hsic_clk and ulpi_clk are not active
*/
boolean HAL_hsusb_GetHSICClkGate(uint8 instance);

/*=============================================================================
FUNCTION:      HAL_hsusb_GetSysClkGate
=============================================================================*/
/**
Clock gating of cc_usb_system_clk

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
TRUE  -- Clock gating of cc_usb_system_clk is active
FALSE -- Clock gating of cc_usb_system_clk is not active
*/
boolean HAL_hsusb_GetSysClkGate(uint8 instance);

/*=============================================================================
FUNCTION:      HAL_hsusb_GetAHBClkGate
=============================================================================*/
/**
Clock gating of usb_ahb_clk

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
TRUE  -- Clock gating of usb_ahb_clk is active
FALSE -- Clock gating of usb_ahb_clk is not active
*/
boolean HAL_hsusb_GetAHBClkGate(uint8 instance);

/*=============================================================================
FUNCTION:      HAL_hsusb_GetL1State
=============================================================================*/
/**
Reports whether device is in L1 state. 

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
TRUE  -- Link Controller and HSIC PHY are in L1 state
FALSE -- Link Controller and HSIC PHY are not in L1 state
*/
boolean HAL_hsusb_GetL1State(uint8 instance);

/*=============================================================================
FUNCTION:      HAL_hsusb_GetDebugL1Enable
=============================================================================*/
/**
Reports whether DEBUG counters operation is enabled

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
TRUE  -- Debug counters are enabled
FALSE -- Debug counters are disabled
*/
boolean HAL_hsusb_GetDebugL1Enable(uint8 instance);

/*=============================================================================
FUNCTION:      HAL_hsusb_SetDebugL1Enable
=============================================================================*/
/**
Sets whether DEBUG counters operation is enabled

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state [IN]    -- TRUE to enable debug counters
                        FALSE to disable debug counters

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
None.
*/
void HAL_hsusb_SetDebugL1Enable(uint8 instance, boolean state);

/*=============================================================================
FUNCTION:      HAL_hsusb_GetBRemoteWake
=============================================================================*/
/**
Reports whether host allows device to wakeup host upon any meaningful
application specific event.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
TRUE  -- Device can initiate remote wakeup
FALSE -- Disables the device from initiating remote wakeup
*/
boolean HAL_hsusb_GetBRemoteWake(uint8 instance);

/*=============================================================================
FUNCTION:      HAL_hsusb_GetHIRD
=============================================================================*/
/**
Reports the host initiated resume duration

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
Value of the host initiated resume duration.
*/
uint8 HAL_hsusb_GetHIRD(uint8 instance);

#ifdef __cplusplus
}
#endif

/** @} */ /* endgroup hsusb_qc_api */

#endif /* HAL_HSUSB */
