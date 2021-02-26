#ifndef TRAMP_H
#define TRAMP_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  T R A M P O L I N E    S E R V I C E S

GENERAL DESCRIPTION
  This file contains the external declarations for the trampoline services
  used to manage the interrupt controller.

EXTERNALIZED FUNCTIONS
  tramp_register_isr
  tramp_deregister_isr
  tramp_set_isr
  tramp_is_interrupt_enabled
  tramp_is_interrupt_pending
  tramp_is_interrupt_set
  tramp_clear_interrupt
  tramp_set_interrupt_trigger
  tramp_set_interrupt_port
  tramp_set_interrupt_priority
  tramp_init
  tramp_is_any_interrupt_pending
  tramp_are_interrupts_pending
  tramp_trigger_interrupt
  tramp_send_interrupt
  tramp_sleep
  tramp_wakeup
  tramp_shutdown
  tramp_kill_ists
  tramp_from_sysintr
  tramp_to_sysintr

DESCRIPTION
  All the declarations and definitions necessary to support interaction
  with the QUALCOMM ASIC interrupt controllers.

Copyright (c) 2007 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1/api/systemdrivers/tramp.h#1 $ 
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/20/09   jz      Add 8 more pairs of Q6SW and Q6FW interrupts.
07/08/09   gfr     Added tramp_set_interrupt_priority
05/08/09   gfr     Added support for setting the interrupt port.
04/22/09   gfr     Fix the tramp ISR numbers.
01/27/08   gfr     Remove customer.h, move tramp_set_interrupt_trigger here.
01/20/09   gfr     Added JPEG_IRQ.
12/15/08   gfr     Added legacy mappings for UART1DM.
11/10/08   gfr     Moved AU private APIs to tramp_v.h
11/05/08   gfr     Set MDSP_EUL interrupt differently on 8200.
10/08/08   gfr     Added LPA interrupts.
07/14/08   gfr     Added A2M_PWR_DOWN and A2M_PWR_UP interrupts.
06/11/08   gfr     Removed tramp_select_source API.
06/11/08   gfr     Added missing 8200 QDSP6 interrupts.
06/04/08   gfr     Tramp 3.0 and support for 8200.
05/15/08   gfr     Added TRAMP_HAS_REGISTER_ISR_SUPPORT flag.
04/28/08   gfr     Added legacy mapping for CDMA_STMR_INT3.
04/29/08    th     Added tramp_to_sysintr and tramp_is_interrupt_enabled
02/29/08   gfr     Renamed tramp_register to tramp_register_isr.
01/03/08   gfr     Added SPSS2ADSP interrupt.
11/07/07   gfr     Define TRAMP_SLEEP_INT1_ISR as UMTS for 6k.
09/19/07   gfr     Allow inclusion from C++, GPIO support for WinCE.
09/27/07   gfr     Fixed name of tramp_check_interrupts
09/25/07   gfr     Legacy CDMA IRQ support.
09/19/07   gfr     Legacy SDC and UART2DM mappings.
09/17/07   gfr     Legacy MDDI IRQ support.
09/12/07   gfr     Support for WinMobile.
09/10/07   gfr     Support for 6270 interrupts.
09/07/07   gfr     Support for GPIO interrupts.
09/01/07   gfr     Added new interrupts for MSM6246.
08/31/07   gfr     Added TRAMP_ADSP legacy defines.
08/30/07   gfr     Added TRAMP_I2C_ISR legacy define.
08/27/07   gfr     Fixed ADSP interrupt logical mapping.
07/17/07   gfr     Make ISR always take the IRQ number.
07/28/07   gfr     Initial version.

===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif


/*===========================================================================

                             INCLUDE FILES

===========================================================================*/


#include "comdef.h"



/*===========================================================================

                          FEATURE DECLARATIONS

===========================================================================*/


/*
 * Define some feature support flags for clients.
 */
#define TRAMP_VERSION 30
#define TRAMP_HAS_CLEAR_INTERRUPT
#define TRAMP_HAS_SEND_INTERRUPT
#define TRAMP_HAS_GPIO_SUPPORT
#define TRAMP_HAS_WINCE_SUPPORT
#define TRAMP_HAS_REGISTER_ISR_SUPPORT



/*===========================================================================

                         TYPE DECLARATIONS

===========================================================================*/


/*
 * tramp_irq_type
 *
 * The list of interrupts supported by Tramp.  Note that not all of the
 * interrupts are supported by the hardware.
 * Developer note: any change to this enumeration must be reflected in the
 * tramp_irq_table declared in tramp_data.c
 */
typedef enum
{
  TRAMP_NULL_IRQ                                   = 0,

  /* CDMA interrupts */
  TRAMP_CDMA_RTC_COMPARE_1_IRQ                     = 1,
  TRAMP_CDMA_RTC_COMPARE_2_IRQ                     = 2,
  TRAMP_CDMA_RTC_ROLLOVER_IRQ                      = 3,
  TRAMP_CDMA_RTC_IRQ                               = 4,
  TRAMP_CDMA_SYS_TIME_GP_COMPARE_IRQ               = 5,
  TRAMP_CDMA_PN_ROLLOVER_1X_IRQ                    = 6,
  TRAMP_CDMA_PN_ROLLOVER_HDR_IRQ                   = 7,
  TRAMP_CDMA_MOD_TX_SLOT_1X_IRQ                    = 8,
  TRAMP_CDMA_MOD_TX_SLOT_HDR_IRQ                   = 9,
  TRAMP_CDMA_MOD_TX_SLOT_IRQ                       = 10,
  TRAMP_CDMA_MOD_TX_FRAME_1X_IRQ                   = 11,
  TRAMP_CDMA_MOD_TX_FRAME_HDR_IRQ                  = 12,
  TRAMP_CDMA_MOD_TX_FRAME_IRQ                      = 13,
  TRAMP_CDMA_DEC_DONE_IRQ                          = 14,
  TRAMP_CDMA_QUICK_PAGING_IRQ                      = 15,
  TRAMP_CDMA_OFFLINE_DONE_IRQ                      = 16,
  TRAMP_CDMA_TIME_1_IRQ                            = 17,
  TRAMP_CDMA_TIME_2_IRQ                            = 18,
  TRAMP_CDMA_EVEN_SEC_IRQ                          = 19,

  /* UMTS interrupts */
  TRAMP_UMTS_STMR_TX_10MS_IRQ                      = 20,
  TRAMP_UMTS_STMR_EVENT_IRQ                        = 21,
  TRAMP_UMTS_STMR_0_IRQ                            = 22,
  TRAMP_UMTS_STMR_1_IRQ                            = 23,
  TRAMP_UMTS_STMR_2_IRQ                            = 24,
  TRAMP_UMTS_STMR_3_IRQ                            = 25,
  TRAMP_UMTS_F9_IRQ                                = 26,
  TRAMP_UMTS_BACKUP_IRQ                            = 27,
  TRAMP_UMTS_F8_IRQ                                = 272,

  /* UMTS interrupts */
  TRAMP_OFDMA_STMR_0_IRQ                           = 273,
  TRAMP_OFDMA_STMR_1_IRQ                           = 274,
  
  /* GSM interrupts */
  TRAMP_EDGE_ENCRYPT_DONE_IRQ                      = 28,
  TRAMP_GSM_STMR_IRQ                               = 29,

  /* Sleep interrupts */
  TRAMP_SLEEP_GSM_IRQ                              = 30,
  TRAMP_SLEEP_1X_IRQ                               = 31,
  TRAMP_SLEEP_HDR_IRQ                              = 32,
  TRAMP_SLEEP_UMTS_IRQ                             = 33,
  TRAMP_SLEEP_FEE_IRQ                              = 34,
  TRAMP_SLEEP_TIMER_0_IRQ                          = 35,
  TRAMP_SLEEP_TIMER_1_IRQ                          = 36,
  TRAMP_SLEEP_TIMETICK_IRQ                         = 37,
  TRAMP_SLEEP_OFDMA_IRQ                            = 275,

  /* GPS interrupts */
  TRAMP_GPS_CMP_IRQ                                = 38,
  TRAMP_GPS_BC_IRQ                                 = 39,
  TRAMP_MMGPS_HOST_IRQ                             = 40,
  TRAMP_GPS_TS_IRQ                                 = 276,

  /* System-online interrupts */
  TRAMP_1X_SYS_ONLINE_IRQ                          = 41,
  TRAMP_HDR_SYS_ONLINE_IRQ                         = 42,
  TRAMP_UMTS_SYS_ONLINE_IRQ                        = 43,
  TRAMP_GSM_SYS_ONLINE_IRQ                         = 277,
  TRAMP_OFDMA_SYS_ONLINE_IRQ                       = 278,

  /* MDSP interrupts */
  TRAMP_MDSP_0_IRQ                                 = 44,
  TRAMP_MDSP_1_IRQ                                 = 45,

  /* SBI interrupts */
  TRAMP_MODEM_SBI0_IRQ                             = 46,
  TRAMP_MODEM_SBI1_IRQ                             = 47,
  TRAMP_MODEM_SBI_IRQ                              = 48,
  TRAMP_MODEM_SSBI0_IRQ                            = 49,
  TRAMP_MODEM_SSBI1_IRQ                            = 50,
  TRAMP_MODEM_SSBI2_IRQ                            = 51,
  TRAMP_TX_SBI_IRQ                                 = 52,
  TRAMP_TX_SSBI_IRQ                                = 53,
  TRAMP_PMIC_SSBI_IRQ                              = 54,
  TRAMP_ADIE_SSBI_IRQ                              = 55,
  TRAMP_GPIO_SSBI_IRQ                              = 56,

  /* USB interrupts */
  TRAMP_USB_OTG_IRQ                                = 57,
  TRAMP_USB_HS_IRQ                                 = 58,
  TRAMP_USB_HSIC_IRQ                               = 241,
  TRAMP_USB_HS2_IRQ                                = 294,
  TRAMP_USB_HS3_IRQ                                = 295,

  /* MDDI interrupts */
  TRAMP_MDDI_PRI_IRQ                               = 59,
  TRAMP_MDDI_EXT_IRQ                               = 60,
  TRAMP_MDDI_CLIENT_IRQ                            = 61,

  /* Data mover interrupts */
  TRAMP_MDM_IRQ                                    = 62,
  TRAMP_ADM_IRQ                                    = 63,
  TRAMP_ADM_1_IRQ                                  = 242,

  /* GPIO group interrupts */
  TRAMP_GPIO_GROUP1_IRQ                            = 64,
  TRAMP_GPIO_GROUP2_IRQ                            = 65,

  /* Bus error interrupts */
  TRAMP_IMEM_BUS_ERR_IRQ                           = 66,
  TRAMP_AXI_BUS_ERR_IRQ                            = 67,
  TRAMP_PRPH_BUS_ERR_IRQ                           = 68,
  TRAMP_MSS_BUS_ERR_IRQ                            = 69,
  TRAMP_SMI_MPU_ERR_IRQ                            = 70,
  TRAMP_EBI1_MPU_ERR_IRQ                           = 71,
  TRAMP_SYS_AHB_MPU_ERR_IRQ                        = 72,
  TRAMP_APP_AHB_MPU_ERR_IRQ                        = 73,
  TRAMP_DMA_AHB_MPU_ERR_IRQ                        = 74,

  /* Apps-to-modem interrups */
  TRAMP_A2M_0_IRQ                                  = 75,
  TRAMP_A2M_1_IRQ                                  = 76,
  TRAMP_A2M_2_IRQ                                  = 77,
  TRAMP_A2M_3_IRQ                                  = 78,
  TRAMP_A2M_4_IRQ                                  = 79,
  TRAMP_A2M_5_IRQ                                  = 80,
  TRAMP_A2M_6_IRQ                                  = 81,
  TRAMP_A2M_7_IRQ                                  = 237,
  TRAMP_A2M_8_IRQ                                  = 238,
  TRAMP_A2M_9_IRQ                                  = 239,
  TRAMP_A2M_PWR_DOWN_IRQ                           = 82,
  TRAMP_A2M_PWR_UP_IRQ                             = 83,

  /* Modem-to-apps interrups */
  TRAMP_M2A_0_IRQ                                  = 84,
  TRAMP_M2A_1_IRQ                                  = 85,
  TRAMP_M2A_2_IRQ                                  = 86,
  TRAMP_M2A_3_IRQ                                  = 87,
  TRAMP_M2A_4_IRQ                                  = 88,
  TRAMP_M2A_5_IRQ                                  = 89,
  TRAMP_M2A_6_IRQ                                  = 90,
  TRAMP_M2A_SOFTRESET_IRQ                          = 91,

  /* ARM9 Outgoing Interrupts (destinations vary) */
  TRAMP_A9_0_IRQ                                   = 92,
  TRAMP_A9_1_IRQ                                   = 93,
  TRAMP_A9_2_IRQ                                   = 94,
  TRAMP_A9_3_IRQ                                   = 95,
  TRAMP_A9_4_IRQ                                   = 279,
  TRAMP_A9_5_IRQ                                   = 280,  

  /* Scorpion-to-ADSP interrupts */
  TRAMP_SPSS2ADSP_IRQ                              = 96,

  /* QDSP6 outgoing interrupts */
  TRAMP_Q6A_0_IRQ                                  = 97,
  TRAMP_Q6A_1_IRQ                                  = 98,
  TRAMP_Q6A_2_IRQ                                  = 99,
  TRAMP_Q6A_3_IRQ                                  = 100,
  TRAMP_Q6A_4_IRQ                                  = 101,
  TRAMP_Q6A_5_IRQ                                  = 243,
  TRAMP_Q6A_6_IRQ                                  = 245,
  TRAMP_Q6A_7_IRQ                                  = 246,
  TRAMP_Q6A_8_IRQ                                  = 247,
  TRAMP_Q6A_9_IRQ                                  = 248,
  TRAMP_Q6A_10_IRQ                                 = 249,
  TRAMP_Q6A_IDLE_IRQ                               = 102,
  TRAMP_Q6B_0_IRQ                                  = 103,
  TRAMP_Q6B_1_IRQ                                  = 104,
  TRAMP_Q6B_2_IRQ                                  = 105,
  TRAMP_Q6B_3_IRQ                                  = 106,
  TRAMP_Q6B_4_IRQ                                  = 107,
  TRAMP_Q6B_5_IRQ                                  = 244,
  TRAMP_Q6B_6_IRQ                                  = 284,
  TRAMP_Q6B_7_IRQ                                  = 285,
  TRAMP_Q6B_8_IRQ                                  = 286,
  TRAMP_Q6B_9_IRQ                                  = 287,
  TRAMP_Q6B_10_IRQ                                 = 288,
  TRAMP_Q6B_IDLE_IRQ                               = 108,

  /* ADSP interrupts */
  TRAMP_ADSP_0_IRQ                                 = 109,
  TRAMP_ADSP_1_IRQ                                 = 110,
  TRAMP_ADSP_2_IRQ                                 = 111,

  /* Secure Digital Card interrupt */
  TRAMP_SDC1_0_IRQ                                 = 112,
  TRAMP_SDC1_1_IRQ                                 = 113,
  TRAMP_SDC1_MMC_IRQ                               = 114,
  TRAMP_SDC1_MMC_ERR_IRQ                           = 115,
  TRAMP_SDC2_0_IRQ                                 = 116,
  TRAMP_SDC2_1_IRQ                                 = 117,
  TRAMP_SDC2_MMC_IRQ                               = 118,
  TRAMP_SDC2_MMC_ERR_IRQ                           = 119,
  TRAMP_SDC3_0_IRQ                                 = 120,
  TRAMP_SDC3_1_IRQ                                 = 121,
  TRAMP_SDC4_0_IRQ                                 = 122,
  TRAMP_SDC4_1_IRQ                                 = 123,
  TRAMP_SDC5_0_IRQ                                 = 325,
  TRAMP_SDC5_1_IRQ                                 = 326,

  /* SDIO Controller Interrupts */
  TRAMP_SDIOC1_IN_IRQ                              = 124,
  TRAMP_SDIOC1_OUT_IRQ                             = 125,
  TRAMP_SDIOC1_ERROR_IRQ                           = 126,
  TRAMP_SDIOC1_RESET_IRQ                           = 127,
  TRAMP_SDIOC2_IN_IRQ                              = 128,
  TRAMP_SDIOC2_OUT_IRQ                             = 129,
  TRAMP_SDIOC2_ERROR_IRQ                           = 130,
  TRAMP_SDIOC2_RESET_IRQ                           = 131,
  TRAMP_SDIOC1_IRQ                                 = 336,
  TRAMP_SDIOC1_BAM_IRQ                             = 337,
  TRAMP_SDIOC2_IRQ                                 = 338,
  TRAMP_SDIOC2_BAM_IRQ                             = 339,

  /* UART interrupts */
  TRAMP_UART1_IRQ                                  = 132,
  TRAMP_UART1_RX_IRQ                               = 133,
  TRAMP_UART2_IRQ                                  = 134,
  TRAMP_UART2_RX_IRQ                               = 135,
  TRAMP_UART3_IRQ                                  = 136,
  TRAMP_UART3_RX_IRQ                               = 137,
  TRAMP_UART1DM_IRQ                                = 138,
  TRAMP_UART1DM_RX_IRQ                             = 139,
  TRAMP_UART2DM_IRQ                                = 140,
  TRAMP_UART2DM_RX_IRQ                             = 141,

  /* GSBI interrupts */
  TRAMP_GSBI0_UART_IRQ                             = 180,
  TRAMP_GSBI1_UART_IRQ                             = 181,
  TRAMP_GSBI2_UART_IRQ                             = 182,
  TRAMP_GSBI3_UART_IRQ                             = 183,
  TRAMP_GSBI4_UART_IRQ                             = 184,

  /* Touchscreen interrupts */
  TRAMP_TCHSCRN_1_IRQ                              = 142,
  TRAMP_TCHSCRN_2_IRQ                              = 143,
  TRAMP_TCHSCRN_SSBI_IRQ                           = 144,

  /* NAND controller interrupts */
  TRAMP_NAND_WR_ERR_DONE_IRQ                       = 145,
  TRAMP_NAND_OP_DONE_IRQ                           = 146,
  TRAMP_NAND_EBI2_GPIO_IRQ                         = 147,

  /* Video controller interrupts */
  TRAMP_VDC_AXI_IRQ                                = 148,
  TRAMP_VDC_DB_IRQ                                 = 149,
  TRAMP_VDC_MEC_IRQ                                = 150,

  /* Audio out interrupts */
  TRAMP_AUDIO_OUT_0_IRQ                            = 151,
  TRAMP_AUDIO_OUT_1_IRQ                            = 152,
  TRAMP_AUDIO_OUT_2_IRQ                            = 372,

  /* UI interrupts */
  TRAMP_HEADSET_DETECT_IRQ                         = 153,
  TRAMP_KEYSENSE_IRQ                               = 154,
  TRAMP_PEN_DOWN_IRQ                               = 155,

  /* Timer interrupts */
  TRAMP_GP_TIMER_IRQ                               = 156,
  TRAMP_DEBUG_TIMER_IRQ                            = 157,

  /* ARM11 subsystem interrupts */
  TRAMP_A11S_PMU_IRQ                               = 158,
  TRAMP_A11S_DMA_IRQ                               = 159,

  /* Scorpion interrupts */
  TRAMP_SC_AVS_REQ_DOWN_IRQ                        = 160,
  TRAMP_SC_AVS_REQ_UP_IRQ                          = 161,
  TRAMP_SC_DBG_RDTRFULL_IRQ                        = 162,
  TRAMP_SC_DBG_WDTRFULL_IRQ                        = 163,
  TRAMP_SC_DMA_NON_SECURE_IRQ                      = 164,
  TRAMP_SC_DMA_SECURE_IRQ                          = 165,
  TRAMP_SC_PERF_MONITOR_IRQ                        = 166,
  TRAMP_SC_PLL_CTL_DONE_IRQ                        = 167,
  TRAMP_SC_TEMP_SENSOR_IRQ                         = 168,

  /* AHB Arbiter interrupts */
  TRAMP_SYS_AHB_ARBITER_PTR_WRAP_IRQ               = 169,
  TRAMP_APP_AHB_ARBITER_PTR_WRAP_IRQ               = 170,
  TRAMP_DMA_AHB_ARBITER_PTR_WRAP_IRQ               = 171,

  /* SPI interrupts */
  TRAMP_SPI_INPUT_IRQ                              = 172,
  TRAMP_GSBI0_SPI_INPUT_IRQ                        = 185,
  TRAMP_GSBI1_SPI_INPUT_IRQ                        = 186,
  TRAMP_GSBI2_SPI_INPUT_IRQ                        = 187,
  TRAMP_GSBI3_SPI_INPUT_IRQ                        = 188,
  TRAMP_GSBI4_SPI_INPUT_IRQ                        = 189,
  TRAMP_SPI_OUTPUT_IRQ                             = 173,
  TRAMP_GSBI0_SPI_OUTPUT_IRQ                       = 190,
  TRAMP_GSBI1_SPI_OUTPUT_IRQ                       = 191,
  TRAMP_GSBI2_SPI_OUTPUT_IRQ                       = 192,
  TRAMP_GSBI3_SPI_OUTPUT_IRQ                       = 193,
  TRAMP_GSBI4_SPI_OUTPUT_IRQ                       = 194,
  TRAMP_SPI_ERROR_IRQ                              = 174,
  TRAMP_GSBI0_SPI_ERROR_IRQ                        = 195,
  TRAMP_GSBI1_SPI_ERROR_IRQ                        = 196,
  TRAMP_GSBI2_SPI_ERROR_IRQ                        = 197,
  TRAMP_GSBI3_SPI_ERROR_IRQ                        = 198,
  TRAMP_GSBI4_SPI_ERROR_IRQ                        = 199,

  /* QUP interrupts */
  TRAMP_GSBI_QUP_INPUT_IRQ                         = 291,
  TRAMP_GSBI_QUP_OUTPUT_IRQ                        = 292,
  TRAMP_GSBI_QUP_ERROR_IRQ                         = 293,
  TRAMP_GSBI1_QUP_IRQ                              = 313,
  TRAMP_GSBI2_QUP_IRQ                              = 314,
  TRAMP_GSBI3_QUP_IRQ                              = 315,
  TRAMP_GSBI4_QUP_IRQ                              = 316,
  TRAMP_GSBI5_QUP_IRQ                              = 317,
  TRAMP_GSBI6_QUP_IRQ                              = 318,
  TRAMP_GSBI7_QUP_IRQ                              = 319,
  TRAMP_GSBI8_QUP_IRQ                              = 320,
  TRAMP_GSBI9_QUP_IRQ                              = 321,
  TRAMP_GSBI10_QUP_IRQ                             = 322,
  TRAMP_GSBI11_QUP_IRQ                             = 323,
  TRAMP_GSBI12_QUP_IRQ                             = 324,

  /* I2C interrupts */
  TRAMP_I2C_IRQ                                    = 200,
  TRAMP_I2C2_IRQ                                   = 282,
  TRAMP_GSBI0_I2C_IRQ                              = 201,
  TRAMP_GSBI1_I2C_IRQ                              = 202,
  TRAMP_GSBI2_I2C_IRQ                              = 203,
  TRAMP_GSBI3_I2C_IRQ                              = 204,
  TRAMP_GSBI4_I2C_IRQ                              = 205,

  /* AUX CODEC ONES interrupts */
  TRAMP_AUX_CODEC_ONES_IRQ                         = 206,
  TRAMP_GSBI0_AUX_CODEC_ONES_IRQ                   = 207,
  TRAMP_GSBI1_AUX_CODEC_ONES_IRQ                   = 208,
  TRAMP_GSBI2_AUX_CODEC_ONES_IRQ                   = 209,
  TRAMP_GSBI3_AUX_CODEC_ONES_IRQ                   = 210,
  TRAMP_GSBI4_AUX_CODEC_ONES_IRQ                   = 211,

  /* Combined/Multi-source/Level-2 Modem Interrupts*/
  TRAMP_MODEM_COMB_0_IRQ                           = 175,
  TRAMP_MODEM_COMB_1_IRQ                           = 176,
  TRAMP_MODEM_COMB_2_IRQ                           = 177,
  TRAMP_MODEM_COMB_3_IRQ                           = 178,
  TRAMP_MODEM_COMB_4_IRQ                           = 179,
  TRAMP_MODEM_COMB_5_IRQ                           = 240,
  TRAMP_MODEM_COMB_6_IRQ                           = 250,
  TRAMP_MODEM_COMB_7_IRQ                           = 251,
  TRAMP_MODEM_COMB_8_IRQ                           = 252,
  TRAMP_MODEM_COMB_9_IRQ                           = 253,
  TRAMP_MODEM_COMB_10_IRQ                          = 254,
  TRAMP_MODEM_COMB_11_IRQ                          = 255,
  TRAMP_MODEM_COMB_12_IRQ                          = 256,
  TRAMP_MODEM_COMB_13_IRQ                          = 257,
  TRAMP_MODEM_COMB_14_IRQ                          = 258,
  TRAMP_MODEM_COMB_15_IRQ                          = 259,
  TRAMP_MODEM_COMB_16_IRQ                          = 260,
  TRAMP_MODEM_COMB_17_IRQ                          = 261,
  TRAMP_MODEM_COMB_18_IRQ                          = 262,

  /* Miscellaneous interrupts */
  TRAMP_ADC_EOC_IRQ                                = 212,
  TRAMP_BROADCAST_MODEM_IRQ                        = 213,
  TRAMP_BT_WAKEUP_IRQ                              = 214,
  TRAMP_BUS_PERF_MONITOR_IRQ                       = 215,
  TRAMP_CAMIF_OVERFLOW_IRQ                         = 216,
  TRAMP_CRYPTO_ENGINE_IRQ                          = 217,
  TRAMP_CRYPTO_ENGINE_MSS_IRQ                      = 218,
  TRAMP_CSI_IRQ                                    = 335,
  TRAMP_GRAPHICS_IRQ                               = 219,
  TRAMP_MDP_IRQ                                    = 220,
  TRAMP_MFC720_IRQ                                 = 289,
  TRAMP_JPEG_IRQ                                   = 221,
  TRAMP_MIDI_IRQ                                   = 222,
  TRAMP_MPM_IRQ                                    = 223,
  TRAMP_MUSIM_IRQ                                  = 224,
  TRAMP_P2_IRQ                                     = 225,
  TRAMP_PMIC_IRQ                                   = 226,
  TRAMP_SECURITY_IRQ                               = 227,
  TRAMP_TIMETICK_IRQ                               = 228,
  TRAMP_TSIF_IRQ                                   = 229,
  TRAMP_VFE_IRQ                                    = 230,
  TRAMP_VPE_IRQ                                    = 290,
  TRAMP_WATCHDOG_IRQ                               = 231,
  TRAMP_UXMC_RDM_IRQ                               = 232,
  TRAMP_HBFC_ACK_IRQ                               = 233,
  TRAMP_HBFC_NACK_IRQ                              = 234,
  TRAMP_FM_RDS_IRQ                                 = 235,
  TRAMP_EXT_FM_IRQ                                 = 236,
  TRAMP_TV_ENC_IRQ                                 = 237,
  TRAMP_GNSS_DM_MARM_IRQ                           = 263,
  TRAMP_AHB2GEN_BRIDGE_IRQ                         = 264,
  TRAMP_L2CC_EVENT_MONITOR_IRQ                     = 283,

  /* LPA interrupts */
  TRAMP_LPA_2_IRQ                                  = 238,
  TRAMP_LPA_3_IRQ                                  = 239,

  /* A2 interrupts */
  TRAMP_A2_UL_PHY_IRQ                              = 265,
  TRAMP_A2_DL_PHY_IRQ                              = 266,
  TRAMP_A2_UL_PER_IRQ                              = 267,
  TRAMP_A2_DL_PER_IRQ                              = 268,
  TRAMP_A2_UL_SEC_IRQ                              = 269,
  TRAMP_A2_FRAG_MOD_IRQ                            = 270,
  TRAMP_A2_FRAG_APP_IRQ                            = 271,

  /* RF DIRECTOR */
  TRAMP_RF_DIRECTOR_IRQ                            = 281,
  TRAMP_TEMP_SENSOR_IRQ                            = 296,

  /* DIRECT CONNECT IRQS FOR GPIOS */
  TRAMP_GPIO_DIRECT_CONNECT_0_IRQ                  = 297,  
  TRAMP_GPIO_DIRECT_CONNECT_1_IRQ                  = 298,      
  TRAMP_GPIO_DIRECT_CONNECT_2_IRQ                  = 299,     
  TRAMP_GPIO_DIRECT_CONNECT_3_IRQ                  = 300,     
  TRAMP_GPIO_DIRECT_CONNECT_4_IRQ                  = 301,     
  TRAMP_GPIO_DIRECT_CONNECT_5_IRQ                  = 302,     
  TRAMP_GPIO_DIRECT_CONNECT_6_IRQ                  = 303,     
  TRAMP_GPIO_DIRECT_CONNECT_7_IRQ                  = 304,     
  TRAMP_GPIO_DIRECT_CONNECT_8_IRQ                  = 305,     
  TRAMP_GPIO_DIRECT_CONNECT_9_IRQ                  = 306,     
  TRAMP_GPIO_DIRECT_CONNECT_10_IRQ                 = 307,  
  TRAMP_GPIO_DIRECT_CONNECT_11_IRQ                 = 308,  
  TRAMP_GPIO_DIRECT_CONNECT_12_IRQ                 = 309,  
  TRAMP_GPIO_DIRECT_CONNECT_13_IRQ                 = 310,  
  TRAMP_GPIO_DIRECT_CONNECT_14_IRQ                 = 311,  
  TRAMP_GPIO_DIRECT_CONNECT_15_IRQ                 = 312,

  /* RPM to MSS IRQS */
  TRAMP_RPM2MSS_0_IRQ                              = 327,
  TRAMP_RPM2MSS_1_IRQ                              = 328,
  TRAMP_RPM2MSS_2_IRQ                              = 329,
  TRAMP_RPM2MSS_3_IRQ                              = 330,

  TRAMP_GRAPHICS_2D_IRQ                            = 331,  
  TRAMP_GRAPHICS_3D_IRQ                            = 332,
  TRAMP_Q6POWERUP_DONE_IRQ                         = 333,
  TRAMP_Q6DOG_EXPIRED_IRQ                          = 334,
  /* SPS IRQs*/
  TRAMP_WATCHDOG_2_IRQ                             = 340,
  /* SPS to APPS */
  TRAMP_SPS2A_0_IRQ                                = 341, 
  TRAMP_SPS2A_1_IRQ                                = 342, 
  /* APPS to SPS */
  TRAMP_A2SPS_0_IRQ                                = 343,
  TRAMP_A2SPS_1_IRQ                                = 344, 
  /* MSS to SPS */
  TRAMP_MSS2SPS_0_IRQ                              = 345, 
  TRAMP_MSS2SPS_1_IRQ                              = 346,
  /* SPS to MSS */
  TRAMP_SPS2MSS_0_IRQ                              = 347, 
  TRAMP_SPS2MSS_1_IRQ                              = 348,
  /* SPS to RPM */
  TRAMP_SPS2RPM_0_IRQ                              = 349, 
  TRAMP_SPS2RPM_1_IRQ                              = 350, 
  TRAMP_SPS2RPM_2_IRQ                              = 351, 
  /* RPM to SPS */
  TRAMP_RPM2SPS_0_IRQ                              = 352, 
  TRAMP_RPM2SPS_1_IRQ                              = 353, 
  TRAMP_RPM2SPS_2_IRQ                              = 354,                                 
  TRAMP_SMMU_GFX2D0_CB_SC_NON_SECURE_IRQ           = 355,
  TRAMP_SMMU_GFX3D_CB_SC_NON_SECURE_IRQ            = 356,
  TRAMP_SMMU_IJPEG_CB_SC_NON_SECURE_IRQ            = 357,
  TRAMP_SMMU_JPEGD_CB_SC_NON_SECURE_IRQ            = 358,
  TRAMP_SMMU_MDP0_CB_SC_NON_SECURE_IRQ             = 359,
  TRAMP_SMMU_MDP1_CB_SC_NON_SECURE_IRQ             = 360,
  TRAMP_SMMU_ROT_CB_SC_NON_SECURE_IRQ              = 361,
  TRAMP_SMMU_VCODEC_A_CB_NON_SECURE_IRQ            = 362,
  TRAMP_SMMU_VCODEC_B_CB_NON_SECURE_IRQ            = 363,
  TRAMP_SMMU_VFE_CB_SC_NON_SECURE_IRQ              = 364,
  TRAMP_SMMU_VPE_CB_SC_NON_SECURE_IRQ              = 365, 
  TRAMP_GSBI_QUP_IRQ                               = 366,
  TRAMP_TSIF1_IRQ                                  = 367,
  TRAMP_FABRIC_IRQ                                 = 368,
  TRAMP_TSIF2_IRQ                                  = 369,
  TRAMP_TSIF3_IRQ                                  = 370,
  TRAMP_TSIF4_IRQ                                   = 371,
  /* RPM to APPS IRQS */
  TRAMP_RPM2A_0_IRQ                                = 373,
  TRAMP_RPM2A_1_IRQ                                = 374,
  TRAMP_RPM2A_2_IRQ                                = 375,
  TRAMP_RPM2A_3_IRQ                                = 376,
  TRAMP_BAM1_IRQ                                   = 377,
  TRAMP_BAM2_IRQ                                   = 378,
  TRAMP_BAM3_IRQ                                   = 379,
  TRAMP_BAM4_IRQ                                   = 380,
  TRAMP_BAM5_IRQ                                   = 381,  
  TRAMP_BAM6_IRQ                                   = 382,  
  TRAMP_HDMI_IRQ                                   = 383,
  TRAMP_DSI_IRQ                                    = 384,
  TRAMP_TLMM_IRQ                                   = 385,

  TRAMP_RPM2RIVA_0_IRQ                             = 386,
  TRAMP_RPM2RIVA_1_IRQ                             = 387,
  TRAMP_RPM2RIVA_2_IRQ                             = 388,
  TRAMP_RPM2RIVA_3_IRQ                             = 389,

  TRAMP_WATCHDOG_3_IRQ                           = 390,
  TRAMP_SLEEP_3_IRQ                                   = 391,
  TRAMP_WLAN_0_IRQ                                   = 392,
  TRAMP_WLAN_1_IRQ                                   = 393,
  TRAMP_WLAN_2_IRQ                                   = 394,

  TRAMP_A2RIVA_0_IRQ                                 = 395,
  TRAMP_A2RIVA_1_IRQ                                 = 396,
  TRAMP_A2RIVA_2_IRQ                                 = 397,
  TRAMP_A2RIVA_3_IRQ                                 = 398,
  TRAMP_A2RIVA_4_IRQ                                 = 399,
  TRAMP_A2RIVA_5_IRQ                                 = 400,
  TRAMP_A2RIVA_6_IRQ                                 = 401,
  TRAMP_A2RIVA_7_IRQ                                 = 402,
  TRAMP_M2RIVA_0_IRQ                                = 403,
  TRAMP_M2RIVA_1_IRQ                                 = 404,
  TRAMP_M2RIVA_2_IRQ                                 = 405,
  TRAMP_Q2RIVA_0_IRQ                                 = 406,
  TRAMP_Q2RIVA_1_IRQ                                 = 407,
  TRAMP_CSI_0_IRQ                                       = 408,
  TRAMP_JPEG_0_IRQ                                     = 409,
  TRAMP_IMEM_0_IRQ                                    = 410,
  TRAMP_IMEM_1_IRQ                                    = 411,
  TRAMP_FABRIC_0_IRQ                                 = 412,
  TRAMP_PM_0_IRQ                                       = 413,
  TRAMP_PM_1_IRQ                                       = 414,
  TRAMP_GP_0_IRQ                                       = 415,
  TRAMP_GP_1_IRQ                                       = 416,
  TRAMP_GP_2_IRQ                                       = 417,
  TRAMP_GP_3_IRQ                                       = 418,
  TRAMP_GP_4_IRQ                                       = 419,
  TRAMP_GP_5_IRQ                                       = 420,
  TRAMP_GP_6_IRQ                                       = 421,
  TRAMP_GP_7_IRQ                                       = 422,
  TRAMP_GP_8_IRQ                                       = 423,
  TRAMP_GP_9_IRQ                                       = 424,
  TRAMP_GP_10_IRQ                                     = 425,
  TRAMP_GP_11_IRQ                                     = 426,
  TRAMP_GP_12_IRQ                                     = 427,
  TRAMP_GP_13_IRQ                                     = 428,
  TRAMP_GP_14_IRQ                                     = 429,
  TRAMP_GP_15_IRQ                                     = 430,
  TRAMP_GP_16_IRQ                                     = 431,
  TRAMP_GP_17_IRQ                                     = 432,
  TRAMP_GP_18_IRQ                                     = 433,
  TRAMP_GP_19_IRQ                                     = 434,
  TRAMP_GP_20_IRQ                                     = 435,
  TRAMP_GP_21_IRQ                                     = 436,
  TRAMP_GP_22_IRQ                                     = 437,
  TRAMP_GP_23_IRQ                                     = 438,
  TRAMP_GP_24_IRQ                                     = 439,
  TRAMP_GP_25_IRQ                                     = 440,
  TRAMP_GP_26_IRQ                                     = 441,
  TRAMP_GP_27_IRQ                                     = 442,
  TRAMP_GP_28_IRQ                                     = 443,
  TRAMP_GP_29_IRQ                                     = 444,
  TRAMP_GP_30_IRQ                                     = 445,
  TRAMP_GP_31_IRQ                                     = 446,
  TRAMP_GP_32_IRQ                                     = 447,
  TRAMP_GP_33_IRQ                                     = 448,
  TRAMP_GP_34_IRQ                                     = 449,
  TRAMP_GP_35_IRQ                                     = 450,
  TRAMP_GP_36_IRQ                                     = 451,
  TRAMP_GP_37_IRQ                                     = 452,
  TRAMP_GP_38_IRQ                                     = 453,
  TRAMP_GP_39_IRQ                                     = 454,
  TRAMP_GP_40_IRQ                                     = 455,
  TRAMP_GP_41_IRQ                                     = 456,
  TRAMP_GP_42_IRQ                                     = 457,
  TRAMP_GP_43_IRQ                                     = 458,
  TRAMP_GP_44_IRQ                                     = 459,
  TRAMP_GP_45_IRQ                                     = 460,
  TRAMP_GP_46_IRQ                                     = 461,

  TRAMP_GP_47_IRQ                                     = 462,
  TRAMP_GP_48_IRQ                                     = 463,
  TRAMP_GP_49_IRQ                                     = 464,

  TRAMP_GSBI5_UART_IRQ                            = 465,
  TRAMP_GSBI6_UART_IRQ                            = 466,
  TRAMP_GSBI7_UART_IRQ                            = 467,
  TRAMP_GSBI8_UART_IRQ                            = 468,
  TRAMP_GSBI9_UART_IRQ                            = 469,
  TRAMP_GSBI10_UART_IRQ                           = 470,
  TRAMP_GSBI11_UART_IRQ                           = 471,
  TRAMP_GSBI12_UART_IRQ                           = 472,
  TRAMP_SATA_IRQ                                  = 473,
  TRAMP_SSBI1_0_IRQ                               = 474,
  TRAMP_SSBI1_1_IRQ                               = 475,
  TRAMP_SSBI2_0_IRQ                               = 476,
  TRAMP_SSBI2_1_IRQ                               = 477,
  TRAMP_ADM0_0_IRQ                                = 478,
  TRAMP_ADM0_1_IRQ                                = 479,
  TRAMP_ADM0_2_IRQ                                = 480,
  TRAMP_ADM0_3_IRQ                                = 481,
  TRAMP_ADM1_0_IRQ                                = 482,
  TRAMP_ADM1_1_IRQ                                = 483,
  TRAMP_ADM1_2_IRQ                                = 484,
  TRAMP_ADM1_3_IRQ                                = 485,
  TRAMP_WATCHDOG4_IRQ                             = 486,
  TRAMP_WATCHDOG5_IRQ                             = 487,
  TRAMP_WATCHDOG6_IRQ                             = 488,
  TRAMP_GP_50_IRQ                                 = 489,
  TRAMP_GP_51_IRQ                                 = 490,
  TRAMP_GP_52_IRQ                                 = 491,
  TRAMP_GP_53_IRQ                                 = 492,
  TRAMP_GP_54_IRQ                                 = 493,
  TRAMP_GP_55_IRQ                                 = 494,
  TRAMP_GP_56_IRQ                                 = 495,
  TRAMP_GP_57_IRQ                                 = 496,
  TRAMP_RIVA2A_0_IRQ                              = 497,
  TRAMP_RIVA2A_1_IRQ                              = 498,
  TRAMP_RIVA2A_2_IRQ                              = 499,
  TRAMP_RIVA2A_3_IRQ                              = 500,
  TRAMP_RIVA2A_4_IRQ                              = 501,
  TRAMP_RIVA2A_5_IRQ                              = 502,
  TRAMP_RIVA2A_6_IRQ                              = 503,
  TRAMP_RIVA2A_7_IRQ                              = 504,
  TRAMP_RIVA2A_8_IRQ                              = 505,
  TRAMP_RIVA2A_9_IRQ                              = 506,
  TRAMP_RIVA2A_10_IRQ                             = 507,
  TRAMP_RIVA2A_11_IRQ                             = 508,
  TRAMP_ROTATOR_IRQ                               = 509,
  TRAMP_ETDSP_IRQ                                 = 510,
  TRAMP_GPIO_DIRECT_CONNECT_16_IRQ                = 511,
  TRAMP_GPIO_DIRECT_CONNECT_17_IRQ                = 512,
  TRAMP_GPIO_DIRECT_CONNECT_18_IRQ                = 513,
  TRAMP_GPIO_DIRECT_CONNECT_19_IRQ                = 514,
  TRAMP_GPIO_DIRECT_CONNECT_20_IRQ                = 515,
  TRAMP_GPIO_DIRECT_CONNECT_21_IRQ                = 516,
  TRAMP_RIVA2M_0_IRQ                              = 517,
  TRAMP_RIVA2M_1_IRQ                              = 518,
  TRAMP_RIVA2M_2_IRQ                              = 519,
  TRAMP_LPASS2M_0_IRQ                             = 520,
  TRAMP_LPASS2M_1_IRQ                             = 521,
  TRAMP_LPASS2M_2_IRQ                             = 522,
  TRAMP_LPASS2M_3_IRQ                             = 523,
  TRAMP_SPS2MSS_2_IRQ                             = 524,
  TRAMP_SPS2MSS_3_IRQ                             = 525,
  TRAMP_Q6POWERDN_DONE_IRQ                        = 526,
  TRAMP_TDS_RX_STMR_INT_IRQ                       = 527,
  TRAMP_TDS_WALL_STMR_ST_EVENT_INT_IRQ            = 528,
  TRAMP_TASKQ0_IRQ                                = 529,
  TRAMP_TASKQ1_IRQ                                = 530,
  TRAMP_TASKQ2_IRQ                                = 531,
  TRAMP_TASKQ3_IRQ                                = 532,
  TRAMP_TASKQ4_IRQ                                = 533,
  TRAMP_TASKQ5_IRQ                                = 534,
  TRAMP_TASKQ6_IRQ                                = 535,
  TRAMP_TASKQ7_IRQ                                = 536,
  TRAMP_TASKQ8_IRQ                                = 537,
  TRAMP_TASKQ9_IRQ                                = 538,
  TRAMP_TASKQ10_IRQ                               = 539,
  TRAMP_TASKQ11_IRQ                               = 540,
  TRAMP_TASKQ12_IRQ                               = 541,
  TRAMP_TASKQ13_IRQ                               = 542,
  TRAMP_UGPT_IRQ                                  = 543,
  TRAMP_Q6FW_O_STMR_1_IRQ                         = 544,
  TRAMP_GPS_0_IRQ                                 = 545,
  TRAMP_GPS_1_IRQ                                 = 546,
  TRAMP_A5_COMM_RX_IRQ                            = 547,
  TRAMP_A5_COMM_TX_IRQ                            = 548,
  TRAMP_A5_PMU_IRQ                                = 549,
  TRAMP_SLEEP_TDSCDMA_IRQ                         = 550,
  TRAMP_TDSCDMA_SYS_ONLINE_IRQ                    = 551,
  TRAMP_SPDM_RT_3_IRQ                             = 552,
  TRAMP_NUM_IRQS                                  = 553
}tramp_irq_type;

/*
 * tramp_trigger_type
 *
 * Interrupt trigger type.
 *
 *  TRAMP_TRIGGER_HIGH:     positive level triggered
 *  TRAMP_TRIGGER_LOW:      negative level triggered
 *  TRAMP_TRIGGER_RISING:   positive edge triggered
 *  TRAMP_TRIGGER_FALLING:  negative edge triggered
 */
typedef enum
{
  TRAMP_TRIGGER_HIGH,
  TRAMP_TRIGGER_LOW,
  TRAMP_TRIGGER_RISING,
  TRAMP_TRIGGER_FALLING
} tramp_trigger_type;


/*
 * tramp_port_type
 *
 * Possible destination ports for an incoming interrupt.
 *
 *  TRAMP_PORT_IRQ:       Come on IRQ line (exclusive with TRAMP_PORT_FIQ).
 *  TRAMP_PORT_FIQ:       Come on FIQ line (exclusive with TRAMP_PORT_IRQ).
 *  TRAMP_PORT_SECURE:    Secure domain (exclusive with NONSECURE).
 *  TRAMP_PORT_NONSECURE: Nonsecure domain (exclusive with SECURE).
 */
typedef enum
{
  TRAMP_PORT_IRQ,
  TRAMP_PORT_FIQ,
  TRAMP_PORT_SECURE,
  TRAMP_PORT_NONSECURE
} tramp_port_type;


/*
 * tramp_sleep_type
 *
 * The type of sleep that will be performed.
 *
 *  TRAMP_NO_POWER_COLLAPSE: power collapse will not be done, or did not happen
 *  TRAMP_POWER_COLLAPSE:    power collapse will be done, or did happen
 */
typedef enum
{
  TRAMP_NO_POWER_COLLAPSE,
  TRAMP_POWER_COLLAPSE
} tramp_sleep_type;



/*
 * tramp_priority_type
 *
 * Generic type for the priority of an interrupt.  Interpretation depends
 * on which platform we are running on.
 *
 */
typedef uint32 tramp_priority_type;


/*
 * tramp_void_handler_type
 *
 * Pointer to a simple isr function that takes no parameters.  Used by
 * legacy code using tramp_set_isr.
 */
typedef void (*tramp_void_handler_type) (void);


/*
 * tramp_handler_type
 *
 * Pointer to a isr function that takes a given parameter as an argument
 * (typically the IRQ number).
 */
typedef void (*tramp_handler_type) (uint32 param);



/*===========================================================================

                         DEPENDANT INCLUDE FILES

===========================================================================*/

/*
 * Include the GPIO interrupt definitions which depend on the above
 * tramp definitions.
 */
#include "tramp_gpio.h"



/*===========================================================================

                         FUNCTION DECLARATIONS

===========================================================================*/


/*==========================================================================

  FUNCTION      TRAMP_SET_ISR

  DESCRIPTION   Installs or uninstalls an ISR and enables or disables
                the relevant interrupt line.
                Note this function is obselete and should be replaced with
                tramp_register_isr/tramp_deregister_isr when possible.

  PARAMETERS    irq - which interrupt request line
                isr - the handler, if NULL interrupt is disabled

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/

void tramp_set_isr
(
  tramp_irq_type           irq,
  tramp_void_handler_type  isr
);


/*==========================================================================

  FUNCTION      TRAMP_REGISTER_ISR

  DESCRIPTION   Registers an ISR and enables the relevant interrupt line.

  PARAMETERS    irq   - which interrupt request line
                isr   - the handler, which takes "param" as an argument
                param - the argument to pass to the ISR

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/

void tramp_register_isr
(
  tramp_irq_type     irq,
  tramp_handler_type isr,
  uint32             param
);


/*==========================================================================

  FUNCTION      TRAMP_DEREGISTER_ISR

  DESCRIPTION   Deregisters an ISR and disables the relevant interrupt
                line.

  PARAMETERS    irq - which interrupt request line
                isr - the handler to remove

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/

void tramp_deregister_isr
(
  tramp_irq_type       irq,
  tramp_handler_type   isr
);


/*==========================================================================

  FUNCTION      TRAMP_IS_INTERRUPT_ENABLED

  DESCRIPTION   This function is used to determine if an interrupt is
                enabled on the PIC

  PARAMETERS    The tramp interrupt number.

  DEPENDENCIES  Should only be invoked from the OAL (i.e. in the kernel).
                tramp_init must have been previously called.

  RETURN VALUE  TRUE/FALSE

  SIDE EFFECTS  None.

==========================================================================*/

boolean tramp_is_interrupt_enabled
(
  tramp_irq_type irq
);


/*==========================================================================

  FUNCTION      TRAMP_IS_INTERRUPT_PENDING

  DESCRIPTION   Checks if an interrupt is pending to be serviced, in
                other words is set and enabled.
                It only makes sense to call this function from a context
                where the interrupt in question is locked out, e.g.
                from a higher priority interrupt context, or from inside
                an INTLOCK region.

  PARAMETERS    irq - which interrupt request line

  DEPENDENCIES  None.

  RETURN VALUE  TRUE if the interrupt is pending, FALSE otherwise.

  SIDE EFFECTS  None.

==========================================================================*/

boolean tramp_is_interrupt_pending
(
  tramp_irq_type irq
);


/*==========================================================================

  FUNCTION      TRAMP_IS_INTERRUPT_SET

  DESCRIPTION   Checks if an interrupt is set, which is different from
                pending in that the interrupt may not actually be enabled.
                It only makes sense to call this function from a context
                where the interrupt in question is locked out, e.g.
                from a higher priority interrupt context, from inside
                an INTLOCK region, or with the interrupt disabled.

  PARAMETERS    irq - which interrupt request line

  DEPENDENCIES  None.

  RETURN VALUE  TRUE if the interrupt is set, FALSE otherwise.

  SIDE EFFECTS  None.

==========================================================================*/

boolean tramp_is_interrupt_set
(
  tramp_irq_type irq
);


/*==========================================================================

  FUNCTION      TRAMP_CLEAR_INTERRUPT

  DESCRIPTION   Clears an interrupt.  Should only be used if an interrupt
                must be cleared outside of the "tramp_isr" context.

  PARAMETERS    irq - interrupt to clear

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/

void tramp_clear_interrupt
(
  tramp_irq_type irq
);


/*==========================================================================

  FUNCTION      TRAMP_SET_INTERRUPT_TRIGGER

  DESCRIPTION   Sets the trigger for an interrupt.

  PARAMETERS    irq - Interrupt to configure.
                trigger - Trigger type.

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/

void tramp_set_interrupt_trigger
(
  tramp_irq_type     irq,
  tramp_trigger_type trigger
);


/*==========================================================================

  FUNCTION      TRAMP_SET_INTERRUPT_PRIORITY

  DESCRIPTION   Sets the priority for an interrupt.

  PARAMETERS    irq      - Interrupt to configure.
                priority - Priority.

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/

void tramp_set_interrupt_priority
(
  tramp_irq_type      irq,
  tramp_priority_type priority
);


/*==========================================================================

  FUNCTION      TRAMP_SET_INTERRUPT_PORT

  DESCRIPTION   Sets the port for an interrupt.

  PARAMETERS    irq - Interrupt to configure.
                port - Interrupt port.

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/

void tramp_set_interrupt_port
(
  tramp_irq_type  irq,
  tramp_port_type port
);


/*==========================================================================

  FUNCTION      TRAMP_INIT

  DESCRIPTION   Initialized the trampoline services.

  PARAMETERS    None.

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/

void tramp_init (void);


/*==========================================================================

  FUNCTION      TRAMP_IS_ANY_INTERRUPT_PENDING

  DESCRIPTION   Checks if any interrupt is pending to be serviced, in
                other words is set and enabled.
                It only makes sense to call this function from a context
                where the interrupt in question is locked out, e.g.
                from a higher priority interrupt context, or from inside
                an INTLOCK region.

  PARAMETERS    None.

  DEPENDENCIES  None.

  RETURN VALUE  TRUE if any interrupts are pending, FALSE otherwise.

  SIDE EFFECTS  None.

==========================================================================*/

boolean tramp_is_any_interrupt_pending (void);


/*==========================================================================

  FUNCTION      TRAMP_ARE_INTERRUPTS_PENDING

  DESCRIPTION   Checks if any of the given interrupts are pending to be
                serviced, in other words is set and enabled.
                It only makes sense to call this function from a context
                where the interrupt in question is locked out, e.g.
                from a higher priority interrupt context, or from inside
                an INTLOCK region.

  PARAMETERS    irqs     - array of interrupt request lines
                num_irqs - size of the above array

  DEPENDENCIES  None.

  RETURN VALUE  TRUE if any of the given interrupts is pending,
                FALSE otherwise.

  SIDE EFFECTS  None.

==========================================================================*/

boolean tramp_are_interrupts_pending
(
  tramp_irq_type irqs[],
  uint32         num_irqs
);


/*==========================================================================

  FUNCTION      TRAMP_TRIGGER_INTERRUPT

  DESCRIPTION   This function causes an interrupt to trigger, or "fire".
                Note it will not work for level-triggered, or second-level
                interrupts.

  PARAMETERS    irq - interrupt to set

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/

void tramp_trigger_interrupt
(
  tramp_irq_type irq
);


/*==========================================================================

  FUNCTION      TRAMP_SEND_INTERRUPT

  DESCRIPTION   This function causes an interrupt to be sent to another
                processor.

  PARAMETERS    irq - interrupt to send

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/

void tramp_send_interrupt
(
  tramp_irq_type irq
);


/*==========================================================================

  FUNCTION      TRAMP_SLEEP

  DESCRIPTION   Puts tramp and the PIC hardware to sleep.  Once we are
                asleep, no more IRQs will be asserted by the PIC until
                tramp_wakeup is called.  FIQ will still be generated.

  PARAMETERS    mode - indicates if we are going to power collapse

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/

void tramp_sleep
(
  tramp_sleep_type mode
);


/*==========================================================================

  FUNCTION      TRAMP_WAKEUP

  DESCRIPTION   Wakeups and restore the PIC hardware.

  PARAMETERS    mode - indicates if we are are restoring from power collapse

  DEPENDENCIES  tramp_sleep must have been called previously.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/

void tramp_wakeup
(
  tramp_sleep_type mode
);


/*==========================================================================

  FUNCTION      TRAMP_SHUTDOWN

  DESCRIPTION   This function shuts-down tramp and the interrupt controller
                hardware in preparation for a non-hardware restart.
                It essentially restores the hardware to the init state.
                After invoking this function tramp_init must be called to
                restore functionality.

  PARAMETERS    None.

  DEPENDENCIES  Must be called with interrupts locked.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/

void tramp_shutdown (void);


/*==========================================================================

  FUNCTION      TRAMP_KILL_ISTS

  DESCRIPTION   This function is used to kill all the ISTs.  It is meant
                to be called during independent processor restart (AMSS
                only restart).

  PARAMETERS    None.

  DEPENDENCIES  Must be called from an IST context with interrupts locked.
                After calling this function, tramp_init must be called
                to restart the trampoline.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/

void tramp_kill_ists (void);


/*==========================================================================

  FUNCTION      TRAMP_FROM_SYSINTR

  DESCRIPTION   This function is used to convert from a Window CE system
                interrupt number to a tramp IRQ number.

  PARAMETERS    sys_intr - the windows CE interrupt number

  DEPENDENCIES  None.

  RETURN VALUE  The tramp interrupt number.

  SIDE EFFECTS  None.

==========================================================================*/

tramp_irq_type tramp_from_sysintr
(
  uint32 sys_intr
);

/*==========================================================================

  FUNCTION      TRAMP_TO_SYSINTR

  DESCRIPTION   This function is used to convert to a Window CE system
                interrupt number from a tramp IRQ number.

  PARAMETERS    The tramp interrupt number.

  DEPENDENCIES  Should only be invoked from the OAL (i.e. in the kernel).
                tramp_init must have been previously called.

  RETURN VALUE  sys_intr - the windows CE interrupt number

  SIDE EFFECTS  None.

==========================================================================*/

uint32 tramp_to_sysintr
(
  tramp_irq_type irq
);

/*==========================================================================

  FUNCTION      TRAMP_MAP_MPM_INTERRUPT

  DESCRIPTION   This function maps the given interrupt to the MPM interrupt
                type if it is a wakeup interrupt.

  PARAMETERS    tramp_irq_type irq : The interrupt name.
                uint32     mpm_irq : The MPM id.
 
  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/

void tramp_map_mpm_interrupt
(
  tramp_irq_type irq,
  uint32         mpm_irq
);

/*===========================================================================

                            LEGACY DECLARATIONS

===========================================================================*/


/*
 * Legacy feature
 */
#define FEATURE_TRAMP_HAS_CLEAR_INTERRUPT


/*
 * Legacy interrupt names
 */
#define TRAMP_SLEEP_TIMER_1_ISR    TRAMP_SLEEP_TIMER_1_IRQ
#define SLEEP_TIMER_INT1           TRAMP_SLEEP_TIMER_1_IRQ
#define TRAMP_SLEEP_TIMER1_ISR     TRAMP_SLEEP_TIMER_1_IRQ
#define TRAMP_SLEEP_TIME_TICK_ISR  TRAMP_SLEEP_TIMETICK_IRQ
#define TRAMP_SLEEP_FEE_ISR        TRAMP_SLEEP_FEE_IRQ
#define SLEEP_FEE_INT              TRAMP_SLEEP_FEE_IRQ
#define TRAMP_SLEEP_INT0_ISR       TRAMP_SLEEP_GSM_IRQ
#define TRAMP_SLEEP_INT2_ISR       TRAMP_SLEEP_HDR_IRQ
#define TRAMP_SLEEP_INT3_ISR       TRAMP_SLEEP_UMTS_IRQ
#define SLEEP_CTL_INT              TRAMP_SLEEP_UMTS_IRQ
#define TRAMP_TIME_TICK_ISR        TRAMP_TIMETICK_IRQ
#define TRAMP_TICK_ISR             TRAMP_SLEEP_TIMER_0_IRQ
#define TRAMP_TICK2_ISR            TRAMP_SLEEP_TIMER_1_IRQ
#define TRAMP_MWDOG_ISR            TRAMP_WATCHDOG_IRQ
#define GSM_MICRO_INT              TRAMP_GSM_STMR_IRQ
#define TRAMP_GSM_MICRO_ISR        TRAMP_GSM_STMR_IRQ
#define TRAMP_A2M_0_ISR            TRAMP_A2M_0_IRQ
#define TRAMP_A2M_4_ISR            TRAMP_A2M_4_IRQ
#define TRAMP_A2M_5_ISR            TRAMP_A2M_5_IRQ
#define TRAMP_A2M_6_ISR            TRAMP_A2M_6_IRQ
#define TRAMP_M2A_0_ISR            TRAMP_M2A_0_IRQ
#define TRAMP_M2A_1_ISR            TRAMP_M2A_1_IRQ
#define TRAMP_M2A_2_ISR            TRAMP_M2A_2_IRQ
#define TRAMP_M2A_3_ISR            TRAMP_M2A_3_IRQ
#define TRAMP_M2A_4_ISR            TRAMP_M2A_4_IRQ
#define TRAMP_M2A_5_ISR            TRAMP_M2A_5_IRQ
#define TRAMP_M2A_6_ISR            TRAMP_M2A_6_IRQ
#define TRAMP_MDDI_EXT_ISR         TRAMP_MDDI_EXT_IRQ
#define TRAMP_MDDI_PRI_ISR         TRAMP_MDDI_PRI_IRQ
#define TRAMP_MDDI_HOST_ISR        TRAMP_MDDI_PRI_IRQ
#define TRAMP_MDDI_CLIENT_ISR      TRAMP_MDDI_CLIENT_IRQ
#define TRAMP_USB_ISR              TRAMP_USB_OTG_IRQ
#define TRAMP_USB_OTG_ISR          TRAMP_USB_OTG_IRQ
#define TRAMP_I2CC_ISR             TRAMP_I2C_IRQ
#define TRAMP_SDC1_0_ISR           TRAMP_SDC1_0_IRQ
#define TRAMP_SDC1_1_ISR           TRAMP_SDC1_1_IRQ
#define TRAMP_SDC2_0_ISR           TRAMP_SDC2_0_IRQ
#define TRAMP_SDC2_1_ISR           TRAMP_SDC2_1_IRQ
#define TRAMP_SDC3_0_ISR           TRAMP_SDC3_0_IRQ
#define TRAMP_SDC3_1_ISR           TRAMP_SDC3_1_IRQ
#define TRAMP_SDC4_0_ISR           TRAMP_SDC4_0_IRQ
#define TRAMP_SDC4_1_ISR           TRAMP_SDC4_1_IRQ
#define TRAMP_SDCC_0_ISR           TRAMP_SDC1_0_IRQ
#define TRAMP_SDCC_1_ISR           TRAMP_SDC1_1_IRQ
#define TRAMP_ADSP_INT0_ISR        TRAMP_ADSP_0_IRQ
#define TRAMP_ADSP_INT1_ISR        TRAMP_ADSP_1_IRQ
#define TRAMP_ADSP_INT2_ISR        TRAMP_ADSP_2_IRQ
#define TRAMP_ADSP_A9A11_ISR       TRAMP_ADSP_2_IRQ
#define TRAMP_ADSP_A11_ISR         TRAMP_ADSP_1_IRQ
#define TRAMP_UART_ISR             TRAMP_UART1_IRQ
#define TRAMP_UART1_ISR            TRAMP_UART1_IRQ
#define TRAMP_UART2_ISR            TRAMP_UART2_IRQ
#define TRAMP_UART3_ISR            TRAMP_UART3_IRQ
#define TRAMP_UART1_RX_ISR         TRAMP_UART1_RX_IRQ
#define TRAMP_UART2_RX_ISR         TRAMP_UART2_RX_IRQ
#define TRAMP_UART3_RX_ISR         TRAMP_UART3_RX_IRQ
#define TRAMP_DP_RX_DATA_ISR       TRAMP_UART1_RX_IRQ
#define TRAMP_DP_RX_DATA2_ISR      TRAMP_UART2_RX_IRQ
#define TRAMP_DP_RX_DATA3_ISR      TRAMP_UART3_RX_IRQ
#define TRAMP_UART1_DM_ISR         TRAMP_UART1DM_IRQ
#define TRAMP_UART1_DM_RX_ISR      TRAMP_UART1DM_RX_IRQ
#define TRAMP_DM_UART_ISR          TRAMP_UART1DM_IRQ
#define TRAMP_DM_DP_RX_DATA_ISR    TRAMP_UART1DM_RX_IRQ
#define TRAMP_DM_UART2_ISR         TRAMP_UART2DM_IRQ
#define TRAMP_DM_DP_RX_DATA2_ISR   TRAMP_UART2DM_RX_IRQ
#define TRAMP_UART1DM_ISR          TRAMP_UART1DM_IRQ
#define TRAMP_UART1DM_DP_RX_DATA_ISR TRAMP_UART1DM_RX_IRQ
#define TRAMP_UART2_DM_ISR         TRAMP_UART2DM_IRQ
#define TRAMP_UART2_DM_RX_ISR      TRAMP_UART2DM_RX_IRQ
#define TRAMP_KEYSENSE_ISR         TRAMP_KEYSENSE_IRQ
#define TRAMP_KEY_SENSE_ISR        TRAMP_KEYSENSE_IRQ
#define KEYSESNSE_INT              TRAMP_KEY_SENSE_ISR
#define TRAMP_AD_HSSD_ISR          TRAMP_HEADSET_DETECT_IRQ
#define TRAMP_HSSD_ISR             TRAMP_HEADSET_DETECT_IRQ
#define TRAMP_NAND_WR_ERR_DONE_ISR TRAMP_NAND_WR_ERR_DONE_IRQ
#define TRAMP_NAND_WR_ER_DONE_ISR  TRAMP_NAND_WR_ERR_DONE_IRQ
#define TRAMP_NAND_OP_DONE_ISR     TRAMP_NAND_OP_DONE_IRQ
#define TRAMP_TCHSCRN1_ISR         TRAMP_TCHSCRN_1_IRQ
#define TRAMP_TCHSCRN2_ISR         TRAMP_TCHSCRN_2_IRQ
#define TRAMP_TCHSCRN_SSBI_ISR     TRAMP_TCHSCRN_SSBI_IRQ
#define TRAMP_TS_APPS_1_ISR        TRAMP_TCHSCRN_1_IRQ
#define TRAMP_TS_APPS_2_ISR        TRAMP_TCHSCRN_2_IRQ
#define TRAMP_SBI_TS_SSBI_ISR      TRAMP_TCHSCRN_SSBI_IRQ
#define TRAMP_USB_HS_ISR           TRAMP_USB_HS_IRQ
#define TRAMP_ADM_MOD_INT_ISR      TRAMP_ADM_IRQ
#define TRAMP_ADM_AARM_ISR         TRAMP_ADM_IRQ
#define TRAMP_MDM_INT_ISR          TRAMP_MDM_IRQ
#define TRAMP_MDM_MARM_ISR         TRAMP_MDM_IRQ
#define TRAMP_MPM_WAKEUP_ISR       TRAMP_MPM_IRQ
#define TRAMP_MDSP_INT_ISR         TRAMP_MDSP_0_IRQ
#define TRAMP_MDSP_ISR             TRAMP_MDSP_0_IRQ
#define TRAMP_GPIO_GROUP_ISR       TRAMP_GPIO_GROUP1_IRQ
#define TRAMP_GPIO_GROUP0_ISR      TRAMP_GPIO_GROUP1_IRQ
#define TRAMP_GPIO_GROUP1_ISR      TRAMP_GPIO_GROUP2_IRQ
#define TRAMP_GPIO_GROUP2_ISR      TRAMP_GPIO_GROUP2_IRQ
#define TRAMP_CDMA_STMR0_ISR       TRAMP_UMTS_STMR_0_IRQ
#define TRAMP_CDMA_STMR1_ISR       TRAMP_UMTS_STMR_1_IRQ
#define TRAMP_CDMA_STMR2_ISR       TRAMP_UMTS_STMR_2_IRQ
#define CDMA_STMR_INT0             TRAMP_UMTS_STMR_0_IRQ
#define CDMA_STMR_INT1             TRAMP_UMTS_STMR_1_IRQ
#define CDMA_STMR_INT2             TRAMP_UMTS_STMR_2_IRQ
#define CDMA_STMR_INT3             TRAMP_UMTS_STMR_3_IRQ
#define TRAMP_RX_ISR               TRAMP_CDMA_DEC_DONE_IRQ
#define TRAMP_SYS_TIME_INT1        TRAMP_CDMA_TIME_1_IRQ
#define TRAMP_SYS_TIME_INT2        TRAMP_CDMA_TIME_2_IRQ
#define TRAMP_PNROLL_ISR           TRAMP_CDMA_PN_ROLLOVER_1X_IRQ
#define TRAMP_HDEM_FRAME_ISR       TRAMP_CDMA_PN_ROLLOVER_HDR_IRQ
#define TRAMP_COMP1_INT            TRAMP_CDMA_RTC_COMPARE_1_IRQ
#define TRAMP_RX_QP_ISR            TRAMP_CDMA_QUICK_PAGING_IRQ
#define TRAMP_HMOD_EVEN_SEC_ISR    TRAMP_CDMA_EVEN_SEC_IRQ
#define TRAMP_TX_ISR               TRAMP_CDMA_MOD_TX_FRAME_1X_IRQ
#define TRAMP_GRAPHICS_ISR         TRAMP_GRAPHICS_IRQ
#define TRAMP_SOFTRESET_ISR        TRAMP_M2A_SOFTRESET_IRQ
#define TRAMP_MDP_ISR              TRAMP_MDP_IRQ
#define TRAMP_ADSP_INT1_ISR        TRAMP_ADSP_1_IRQ
#define TRAMP_PWB_I2C_ISR          TRAMP_I2C_IRQ
#define TRAMP_I2C_ISR              TRAMP_I2C_IRQ
#define TRAMP_BT_WAKEUP_ISR        TRAMP_BT_WAKEUP_IRQ
#define TRAMP_MMGPS_HOST_ISR       TRAMP_MMGPS_HOST_IRQ
#define TRAMP_125_MS_ISR           TRAMP_CDMA_MOD_TX_SLOT_1X_IRQ
#define TRAMP_PER_SLOT_ISR         TRAMP_CDMA_MOD_TX_SLOT_HDR_IRQ
#define TRAMP_1X_TX_FR_ISR         TRAMP_CDMA_MOD_TX_FRAME_1X_IRQ
#define TRAMP_HDR_FRAME_ISR        TRAMP_CDMA_MOD_TX_FRAME_HDR_IRQ
#define TRAMP_UN_SUPPORTED_ISR     TRAMP_NULL_IRQ
#define TRAMP_NUM_INTS             TRAMP_NUM_IRQS
#define TRAMP_ISR_CNT              TRAMP_NUM_IRQS

/*
 * Legacy interrupt types
 */
typedef tramp_irq_type          tramp_isr_type;
typedef tramp_irq_type          tramp_int_num_type;
typedef tramp_void_handler_type tramp_isr_ptr_type;
typedef tramp_void_handler_type isr_ptr_type;
typedef tramp_handler_type      tramp_invoke_isr_ptr_type;

/*
 * Legacy function names
 */
#define tramp_check_raw_interrupt  tramp_is_interrupt_set
#define tramp_check_interrupts     tramp_are_interrupts_pending
#define tramp_set_mode(x)
#define tramp_cleanup              tramp_shutdown
#define tramp_set_interrupt        tramp_trigger_interrupt


#ifdef __cplusplus
}
#endif

#endif  /* TRAMP_H */

