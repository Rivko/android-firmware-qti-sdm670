/*=======================================================================*//**
 * @file        qusb_phy_settings.c
 * @author:     kameya
 * @date        19-Jan-2016
 *
 * @brief       QUSB (Qualcomm High-Speed USB) High Speed(HS) and 
 *              Super Speed (SS) PHY settings.
 *
 * @details     This file contains settings of the HS PHY.
 *
 * @note        
 *
 *              Copyright 2016-2017 Qualcomm Technologies Incorporated.
 *              All Rights Reserved.
 *              Qualcomm Confidential and Proprietary
 * 
*//*========================================================================*/


// ===========================================================================
// 
//                            EDIT HISTORY FOR FILE
//   This section contains comments describing changes made to the module.
//   Notice that changes are listed in reverse chronological order.
// 
// 
// when          who     what, where, why
// ----------   -----    ----------------------------------------------------------
// 2017-07-28    pm      Update PHY from latest HPGs
// 2017-06-06    pm      Updated SS Phy sequence for primary port from Latest HPG 
// 2017-04-26    amitg   Updated QUSB2 Phy confirguration for Host mode low speed devices
// 2017-03-30    pm      Added Configuration for Secondary Core
// 2016-01-19    kameya  First Draft
// 
// ===========================================================================

//----------------------------------------------------------------------------
// Include Files
//----------------------------------------------------------------------------
#include "comdef.h" // basic types as byte word uint32 etc
#include "SDM670Pkg/Library/QusbDciCommonLib/qusb_dci_common_hwio_845.h"
#include "qusb_dci_common.h"

//----------------------------------------------------------------------------
// Static Variable Definitions
//----------------------------------------------------------------------------

//============================================================================
// QUSB High-Speed Primary PHY Configuration Array
//============================================================================

ALIGN(4)
static const uint32 qusb_dci_hs_phy_cfg_address[QUSB_DCI_HS_PHY_CFG_ARRAY_ENTRY_CNT] = 
{
  HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_PLL_ANALOG_CONTROLS_TWO_ADDR,                // 00 : (0x03),
  HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_PLL_CLOCK_INVERTERS_ADDR,                    // 01 : (0x7C),
  HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_PLL_CMODE_ADDR,                              // 02 : (0x80),
  HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_PLL_PLL_LOCK_DELAY_ADDR,                     // 03 : (0x0A),
  //-------------------------------------------------------------------------------------------
  HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_PLL_PLL_DIGITAL_TIMERS_TWO_ADDR,             // 04 : (0x19),
  HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_TUNE1_ADDR,                                  // 05 : (0x45),
  HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_TUNE2_ADDR,                                  // 06 : (0x29),
  HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_TUNE5_ADDR,                                  // 07 : (0x03),  
  //-------------------------------------------------------------------------------------------
  HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_IMP_CTRL1_ADDR,                              // 08 : (0x08),
  HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_PLL_BIAS_CONTROL_2_ADDR                      // 09 : (0x28),
};

ALIGN(4)
static const uint8 qusb_dci_hs_phy_cfg_value[QUSB_DCI_HS_PHY_CFG_ARRAY_ENTRY_CNT] = 
{
  /* HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_PLL_ANALOG_CONTROLS_TWO_ADDR,        00 */ (0x03),
  /* HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_PLL_CLOCK_INVERTERS_ADDR,            01 */ (0x7C),
  /* HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_PLL_CMODE_ADDR,                      02 */ (0x80),
  /* HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_PLL_PLL_LOCK_DELAY_ADDR,             03 */ (0x0A),
  //------------------------------------------------------------------------------------
  /* HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_PLL_PLL_DIGITAL_TIMERS_TWO_ADDR,     04 */ (0x19),
  /* HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_TUNE1_ADDR,                          05 */ (0x45),
  /* HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_TUNE2_ADDR,                          06 */ (0x29),
  /* HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_TUNE5_ADDR,                          07 */ (0x03),  
  //------------------------------------------------------------------------------------
  /* HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_IMP_CTRL1_ADDR,                      08 */ (0x08),
  /* HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_PLL_BIAS_CONTROL_2_ADDR              09 */ (0x28),
};

//============================================================================
// QUSB High-Speed Secondary PHY Configuration Array
//============================================================================

ALIGN(4)
static const uint32 qusb_dci_hs_phy_sec_cfg_address[QUSB_DCI_HS_PHY_CFG_ARRAY_ENTRY_CNT] = 
{
  HWIO_PERIPH_SS_QUSB2PHY_SEC_QUSB2PHY_PLL_ANALOG_CONTROLS_TWO_ADDR,                // 00 : (0x03),
  HWIO_PERIPH_SS_QUSB2PHY_SEC_QUSB2PHY_PLL_CLOCK_INVERTERS_ADDR,                    // 01 : (0x7C),
  HWIO_PERIPH_SS_QUSB2PHY_SEC_QUSB2PHY_PLL_CMODE_ADDR,                              // 02 : (0x80),
  HWIO_PERIPH_SS_QUSB2PHY_SEC_QUSB2PHY_PLL_PLL_LOCK_DELAY_ADDR,                     // 03 : (0x0A),
  //-------------------------------------------------------------------------------------------
  HWIO_PERIPH_SS_QUSB2PHY_SEC_QUSB2PHY_PLL_PLL_DIGITAL_TIMERS_TWO_ADDR,             // 04 : (0x19),
  HWIO_PERIPH_SS_QUSB2PHY_SEC_QUSB2PHY_TUNE1_ADDR,                                  // 05 : (0x31),
  HWIO_PERIPH_SS_QUSB2PHY_SEC_QUSB2PHY_TUNE2_ADDR,                                  // 06 : (0x29),
  HWIO_PERIPH_SS_QUSB2PHY_SEC_QUSB2PHY_TUNE5_ADDR,                                  // 07 : (0x03),  
  //-------------------------------------------------------------------------------------------
  HWIO_PERIPH_SS_QUSB2PHY_SEC_QUSB2PHY_IMP_CTRL1_ADDR,                              // 08 : (0x00),
  HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_PLL_BIAS_CONTROL_2_ADDR                          // 09 : (0x20),
};

ALIGN(4)
static const uint8 qusb_dci_hs_phy_sec_cfg_value[QUSB_DCI_HS_PHY_CFG_ARRAY_ENTRY_CNT] = 
{
  /* HWIO_PERIPH_SS_QUSB2PHY_SEC_QUSB2PHY_PLL_ANALOG_CONTROLS_TWO_ADDR,        00 */ (0x03),
  /* HWIO_PERIPH_SS_QUSB2PHY_SEC_QUSB2PHY_PLL_CLOCK_INVERTERS_ADDR,            01 */ (0x7C),
  /* HWIO_PERIPH_SS_QUSB2PHY_SEC_QUSB2PHY_PLL_CMODE_ADDR,                      02 */ (0x80),
  /* HWIO_PERIPH_SS_QUSB2PHY_SEC_QUSB2PHY_PLL_PLL_LOCK_DELAY_ADDR,             03 */ (0x0A),
  //------------------------------------------------------------------------------------
  /* HWIO_PERIPH_SS_QUSB2PHY_SEC_QUSB2PHY_PLL_PLL_DIGITAL_TIMERS_TWO_ADDR,     04 */ (0x19),
  /* HWIO_PERIPH_SS_QUSB2PHY_SEC_QUSB2PHY_TUNE1_ADDR,                          05 */ (0x31),
  /* HWIO_PERIPH_SS_QUSB2PHY_SEC_QUSB2PHY_TUNE2_ADDR,                          06 */ (0x29),
  /* HWIO_PERIPH_SS_QUSB2PHY_SEC_QUSB2PHY_TUNE5_ADDR,                          07 */ (0x03),  
  //------------------------------------------------------------------------------------
  /* HWIO_PERIPH_SS_QUSB2PHY_SEC_QUSB2PHY_IMP_CTRL1_ADDR,                      08 */ (0x00),
  /* HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_PLL_BIAS_CONTROL_2_ADDR                  09 */ (0x20),
};


//============================================================================
// QUSB Super-Speed Primary PHY Configuration Array
//============================================================================

ALIGN(4)
static const uint32 qusb_dci_ss_phy_cfg_address_common[QUSB_DCI_SS_PHY_COMMON_CFG_ARRAY_ENTRY_CNT] =
{
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_DP_COM_POWER_DOWN_CTRL_ADDR,                      // 000  : (0x01)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_DP_COM_SW_RESET_ADDR,                             // 001  : (0x00)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_POWER_DOWN_CONTROL_ADDR,                      // 002  : (0x01)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_PLL_IVCO_ADDR,                        // 003  : (0x07)
  //------------------------------------------------------------------------------------------------
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_SYSCLK_EN_SEL_ADDR,                   // 004  : (0x14)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_BIAS_EN_CLKBUFLR_EN_ADDR,             // 005  : (0x08)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_CLK_SELECT_ADDR,                      // 006  : (0x30)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_SYS_CLK_CTRL_ADDR,                    // 007  : (0x02) 
  //------------------------------------------------------------------------------------------------
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_RESETSM_CNTRL2_ADDR,                  // 008  : (0x08)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_CMN_CONFIG_ADDR,                      // 009  : (0x16)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_SVS_MODE_CLK_SEL_ADDR,                // 010  : (0x01)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_HSCLK_SEL_ADDR,                       // 011  : (0x80)
  //------------------------------------------------------------------------------------------------
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_DEC_START_MODE0_ADDR,                 // 012  : (0x82)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_DIV_FRAC_START1_MODE0_ADDR,           // 013  : (0xab)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_DIV_FRAC_START2_MODE0_ADDR,           // 014  : (0xea)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_DIV_FRAC_START3_MODE0_ADDR,           // 015  : (0x02)
  //------------------------------------------------------------------------------------------------
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_CP_CTRL_MODE0_ADDR,                   // 016  : (0x06)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_PLL_RCTRL_MODE0_ADDR,                 // 017  : (0x16)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_PLL_CCTRL_MODE0_ADDR,                 // 018  : (0x36)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_INTEGLOOP_GAIN1_MODE0_ADDR,           // 019  : (0x00)
  //------------------------------------------------------------------------------------------------
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_INTEGLOOP_GAIN0_MODE0_ADDR,           // 020  : (0x3f)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_VCO_TUNE2_MODE0_ADDR,                 // 021  : (0x01)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_VCO_TUNE1_MODE0_ADDR,                 // 022  : (0xc9)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_CORECLK_DIV_MODE0_ADDR,               // 023  : (0x0a)
  //------------------------------------------------------------------------------------------------
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_LOCK_CMP3_MODE0_ADDR,                 // 024  : (0x00)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_LOCK_CMP2_MODE0_ADDR,                 // 025  : (0x34)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_LOCK_CMP1_MODE0_ADDR,                 // 026  : (0x15)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_LOCK_CMP_EN_ADDR,                     // 027  : (0x04)
  //------------------------------------------------------------------------------------------------
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_CORE_CLK_EN_ADDR,                     // 028  : (0x00)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_LOCK_CMP_CFG_ADDR,                    // 029  : (0x00)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_VCO_TUNE_MAP_ADDR,                    // 030  : (0x00)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_SYSCLK_BUF_ENABLE_ADDR,               // 031  : (0x0a)
  //------------------------------------------------------------------------------------------------
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_SSC_EN_CENTER_ADDR,                   // 032  : (0x01)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_SSC_PER1_ADDR,                        // 033  : (0x31)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_SSC_PER2_ADDR,                        // 034  : (0x01)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_SSC_ADJ_PER1_ADDR,                    // 035  : (0x00)
  //------------------------------------------------------------------------------------------------
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_SSC_ADJ_PER2_ADDR,                    // 036  : (0x00)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_SSC_STEP_SIZE1_ADDR,                  // 037  : (0x85)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_SSC_STEP_SIZE2_ADDR,                  // 038  : (0x07)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_RXA_UCDR_FASTLOCK_FO_GAIN_ADDR,           // 039  : (0x0b)
  //------------------------------------------------------------------------------------------------
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_RXA_RX_EQU_ADAPTOR_CNTRL2_ADDR,           // 040  : (0x0f)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_RXA_RX_EQU_ADAPTOR_CNTRL3_ADDR,           // 041  : (0x4e)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_RXA_RX_EQU_ADAPTOR_CNTRL4_ADDR,           // 042  : (0x18)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_RXA_RX_EQ_OFFSET_ADAPTOR_CNTRL1_ADDR,     // 043  : (0x77)
  //------------------------------------------------------------------------------------------------
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_RXA_RX_OFFSET_ADAPTOR_CNTRL2_ADDR,        // 044  : (0x80)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_RXA_SIGDET_CNTRL_ADDR,                    // 045  : (0x03)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_RXA_SIGDET_DEGLITCH_CNTRL_ADDR,           // 046  : (0x16)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_RXA_RX_MODE_00_ADDR,                      //      : (0x05)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_RXA_VGA_CAL_CNTRL2_ADDR,                  //      : (0x03)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_RXB_UCDR_FASTLOCK_FO_GAIN_ADDR,           // 047  : (0x0b)
  //------------------------------------------------------------------------------------------------
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_RXB_RX_EQU_ADAPTOR_CNTRL2_ADDR,           // 048  : (0x0f)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_RXB_RX_EQU_ADAPTOR_CNTRL3_ADDR,           // 049  : (0x4e)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_RXB_RX_EQU_ADAPTOR_CNTRL4_ADDR,           // 050  : (0x18)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_RXB_RX_EQ_OFFSET_ADAPTOR_CNTRL1_ADDR,     // 051  : (0x77)
  //------------------------------------------------------------------------------------------------
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_RXB_RX_OFFSET_ADAPTOR_CNTRL2_ADDR,        // 052  : (0x80)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_RXB_SIGDET_CNTRL_ADDR,                    // 053  : (0x03)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_RXB_SIGDET_DEGLITCH_CNTRL_ADDR,           // 054  : (0x16)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_RXB_RX_MODE_00_ADDR,                      //      : (0x05)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_RXB_VGA_CAL_CNTRL2_ADDR,                  //      : (0x03)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_TXA_HIGHZ_DRVR_EN_ADDR,                   // 055  : (0x10)
  //------------------------------------------------------------------------------------------------
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_TXA_RCV_DETECT_LVL_2_ADDR,                // 056  : (0x12)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_TXA_LANE_MODE_1_ADDR,                     // 057  : (0x16)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_TXA_RES_CODE_LANE_OFFSET_RX_ADDR,         // 058  : (0x09)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_TXA_RES_CODE_LANE_OFFSET_TX_ADDR,         // 059  : (0x06)
  //------------------------------------------------------------------------------------------------
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_TXB_HIGHZ_DRVR_EN_ADDR,                   // 060  : (0x10)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_TXB_RCV_DETECT_LVL_2_ADDR,                // 061  : (0x12)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_TXB_LANE_MODE_1_ADDR,                     // 062  : (0x16)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_TXB_RES_CODE_LANE_OFFSET_RX_ADDR,         // 063  : (0x09)
  //------------------------------------------------------------------------------------------------
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_TXB_RES_CODE_LANE_OFFSET_TX_ADDR,         // 064  : (0x06)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_FLL_CNTRL2_ADDR,                              // 065  : (0x83)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_FLL_CNT_VAL_L_ADDR,                           // 066  : (0x09)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_FLL_CNT_VAL_H_TOL_ADDR,                       // 067  : (0xa2)
  //------------------------------------------------------------------------------------------------
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_FLL_MAN_CODE_ADDR,                            // 068  : (0x40)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_FLL_CNTRL1_ADDR,                              // 069  : (0x02)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_LOCK_DETECT_CONFIG1_ADDR,                     // 070  : (0xd1)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_LOCK_DETECT_CONFIG2_ADDR,                     // 071  : (0x1f)
  //------------------------------------------------------------------------------------------------
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_LOCK_DETECT_CONFIG3_ADDR,                     // 072  : (0x47)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_POWER_STATE_CONFIG2_ADDR,                     // 073  : (0x1b)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_RXA_UCDR_SO_SATURATION_AND_ENABLE_ADDR,   // 074  : (0x75)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_RXB_UCDR_SO_SATURATION_AND_ENABLE_ADDR,   // 075  : (0x75)
  //------------------------------------------------------------------------------------------------
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_RX_SIGDET_LVL_ADDR,                           // 076  : (0xCC)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_TXMGN_V0_ADDR,                                // 077  : (0x9F)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_TXMGN_V1_ADDR,                                // 078  : (0x9F)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_TXMGN_V2_ADDR,                                // 079  : (0xB7)
  //------------------------------------------------------------------------------------------------
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_TXMGN_V3_ADDR,                                // 080  : (0x4E)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_TXMGN_V4_ADDR,                                // 081  : (0x65)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_TXMGN_LS_ADDR,                                // 082  : (0x6B)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_TXDEEMPH_M6DB_V0_ADDR,                        // 083  : (0x15)
  //------------------------------------------------------------------------------------------------
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_TXDEEMPH_M3P5DB_V0_ADDR,                      // 084  : (0x0D)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_TXDEEMPH_M6DB_V1_ADDR,                        // 085  : (0x15)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_TXDEEMPH_M3P5DB_V1_ADDR,                      // 086  : (0x0D)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_TXDEEMPH_M6DB_V2_ADDR,                        // 087  : (0x15)
  //------------------------------------------------------------------------------------------------
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_TXDEEMPH_M3P5DB_V2_ADDR,                      // 088  : (0x0D)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_TXDEEMPH_M6DB_V3_ADDR,                        // 089  : (0x15)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_TXDEEMPH_M3P5DB_V3_ADDR,                      // 090  : (0x1D)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_TXDEEMPH_M6DB_V4_ADDR,                        // 091  : (0x15)
  //------------------------------------------------------------------------------------------------
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_TXDEEMPH_M3P5DB_V4_ADDR,                      // 092  : (0x0D)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_TXDEEMPH_M6DB_LS_ADDR,                        // 093  : (0x15)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_TXDEEMPH_M3P5DB_LS_ADDR,                      // 094  : (0x0D)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_REFGEN_REQ_CONFIG1_ADDR,                      //      : (0x21)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_REFGEN_REQ_CONFIG2_ADDR,                      //      : (0x60)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_RATE_SLEW_CNTRL_ADDR,                         // 095  : (0x02)
  //------------------------------------------------------------------------------------------------
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_PWRUP_RESET_DLY_TIME_AUXCLK_ADDR,             // 096  : (0x04)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_TSYNC_RSYNC_TIME_ADDR,                        // 097  : (0x44)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_RCVR_DTCT_DLY_P1U2_L_ADDR,                    // 098  : (0xE7)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_RCVR_DTCT_DLY_P1U2_H_ADDR,                    // 099  : (0x03)
  //------------------------------------------------------------------------------------------------
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_RCVR_DTCT_DLY_U3_L_ADDR,                      // 100  : (0x40)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_RCVR_DTCT_DLY_U3_H_ADDR,                      // 101  : (0x00)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_RXEQTRAINING_WAIT_TIME_ADDR,                  // 102  : (0x75)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_LFPS_TX_ECSTART_EQTLOCK_ADDR,                 // 103  : (0x86)
  //------------------------------------------------------------------------------------------------
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_RXEQTRAINING_RUN_TIME_ADDR,                   // 104  : (0x13)
  HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_LFPS_DET_HIGH_COUNT_VAL_ADDR,                 // 105  : (0x04)
  QUSB_HWIO_ADDR_END,                                                               // 106  : (0x00)
  QUSB_HWIO_ADDR_END,                                                               // 107  : (0x00)
};

ALIGN(4)
static const uint8 qusb_dci_ss_phy_cfg_value_common[QUSB_DCI_SS_PHY_COMMON_CFG_ARRAY_ENTRY_CNT] =
{
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_DP_COM_POWER_DOWN_CTRL_ADDR,                       000  */ (0x01),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_DP_COM_SW_RESET_ADDR,                              001  */ (0x00),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_POWER_DOWN_CONTROL_ADDR,                       002  */ (0x01),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_PLL_IVCO_ADDR,                         003  */ (0x07),
  //------------------------------------------------------------------------------------------------
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_SYSCLK_EN_SEL_ADDR,                    004  */ (0x14),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_BIAS_EN_CLKBUFLR_EN_ADDR,              005  */ (0x08),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_CLK_SELECT_ADDR,                       006  */ (0x30),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_SYS_CLK_CTRL_ADDR,                     007  */ (0x02), 
  //------------------------------------------------------------------------------------------------
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_RESETSM_CNTRL2_ADDR,                   008  */ (0x08),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_CMN_CONFIG_ADDR,                       009  */ (0x16),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_SVS_MODE_CLK_SEL_ADDR,                 010  */ (0x01),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_HSCLK_SEL_ADDR,                        011  */ (0x80),
  //------------------------------------------------------------------------------------------------
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_DEC_START_MODE0_ADDR,                  012  */ (0x82),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_DIV_FRAC_START1_MODE0_ADDR,            013  */ (0xab),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_DIV_FRAC_START2_MODE0_ADDR,            014  */ (0xea),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_DIV_FRAC_START3_MODE0_ADDR,            015  */ (0x02),
  //------------------------------------------------------------------------------------------------
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_CP_CTRL_MODE0_ADDR,                    016  */ (0x06),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_PLL_RCTRL_MODE0_ADDR,                  017  */ (0x16),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_PLL_CCTRL_MODE0_ADDR,                  018  */ (0x36),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_INTEGLOOP_GAIN1_MODE0_ADDR,            019  */ (0x00),
  //------------------------------------------------------------------------------------------------
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_INTEGLOOP_GAIN0_MODE0_ADDR,            020  */ (0x3f),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_VCO_TUNE2_MODE0_ADDR,                  021  */ (0x01),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_VCO_TUNE1_MODE0_ADDR,                  022  */ (0xc9),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_CORECLK_DIV_MODE0_ADDR,                023  */ (0x0a),
  //------------------------------------------------------------------------------------------------
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_LOCK_CMP3_MODE0_ADDR,                  024  */ (0x00),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_LOCK_CMP2_MODE0_ADDR,                  025  */ (0x34),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_LOCK_CMP1_MODE0_ADDR,                  026  */ (0x15),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_LOCK_CMP_EN_ADDR,                      027  */ (0x04),
  //------------------------------------------------------------------------------------------------
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_CORE_CLK_EN_ADDR,                      028  */ (0x00),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_LOCK_CMP_CFG_ADDR,                     029  */ (0x00),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_VCO_TUNE_MAP_ADDR,                     030  */ (0x00),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_SYSCLK_BUF_ENABLE_ADDR,                031  */ (0x0a),
  //------------------------------------------------------------------------------------------------
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_SSC_EN_CENTER_ADDR,                    032  */ (0x01),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_SSC_PER1_ADDR,                         033  */ (0x31),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_SSC_PER2_ADDR,                         034  */ (0x01),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_SSC_ADJ_PER1_ADDR,                     035  */ (0x00),
  //------------------------------------------------------------------------------------------------
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_SSC_ADJ_PER2_ADDR,                     036  */ (0x00),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_SSC_STEP_SIZE1_ADDR,                   037  */ (0x85),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_COM_SSC_STEP_SIZE2_ADDR,                   038  */ (0x07),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_RXA_UCDR_FASTLOCK_FO_GAIN_ADDR,            039  */ (0x0b),
  //------------------------------------------------------------------------------------------------
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_RXA_RX_EQU_ADAPTOR_CNTRL2_ADDR,            040  */ (0x0f),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_RXA_RX_EQU_ADAPTOR_CNTRL3_ADDR,            041  */ (0x4e),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_RXA_RX_EQU_ADAPTOR_CNTRL4_ADDR,            042  */ (0x18),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_RXA_RX_EQ_OFFSET_ADAPTOR_CNTRL1_ADDR,      043  */ (0x77),
  //------------------------------------------------------------------------------------------------
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_RXA_RX_OFFSET_ADAPTOR_CNTRL2_ADDR,         044  */ (0x80),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_RXA_SIGDET_CNTRL_ADDR,                     045  */ (0x03),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_RXA_SIGDET_DEGLITCH_CNTRL_ADDR,            046  */ (0x16),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_RXA_RX_MODE_00_ADDR,                       //   */ (0x05),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_RXA_VGA_CAL_CNTRL2_ADDR,                   //   */ (0x03),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_RXB_UCDR_FASTLOCK_FO_GAIN_ADDR,            047  */ (0x0b),
  //------------------------------------------------------------------------------------------------
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_RXB_RX_EQU_ADAPTOR_CNTRL2_ADDR,            048  */ (0x0f),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_RXB_RX_EQU_ADAPTOR_CNTRL3_ADDR,            049  */ (0x4e),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_RXB_RX_EQU_ADAPTOR_CNTRL4_ADDR,            050  */ (0x18),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_RXB_RX_EQ_OFFSET_ADAPTOR_CNTRL1_ADDR,      051  */ (0x77),
  //------------------------------------------------------------------------------------------------
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_RXB_RX_OFFSET_ADAPTOR_CNTRL2_ADDR,         052  */ (0x80),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_RXB_SIGDET_CNTRL_ADDR,                     053  */ (0x03),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_RXB_SIGDET_DEGLITCH_CNTRL_ADDR,            054  */ (0x16),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_RXB_RX_MODE_00_ADDR,                            */ (0x05),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_RXB_VGA_CAL_CNTRL2_ADDR,                        */ (0x03),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_TXA_HIGHZ_DRVR_EN_ADDR,                    055  */ (0x10),
  //------------------------------------------------------------------------------------------------
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_TXA_RCV_DETECT_LVL_2_ADDR,                 056  */ (0x12),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_TXA_LANE_MODE_1_ADDR,                      057  */ (0x16),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_TXA_RES_CODE_LANE_OFFSET_RX_ADDR,          058  */ (0x09),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_TXA_RES_CODE_LANE_OFFSET_TX_ADDR,          059  */ (0x06),
  //------------------------------------------------------------------------------------------------
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_TXB_HIGHZ_DRVR_EN_ADDR,                    060  */ (0x10),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_TXB_RCV_DETECT_LVL_2_ADDR,                 061  */ (0x12),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_TXB_LANE_MODE_1_ADDR,                      062  */ (0x16),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_TXB_RES_CODE_LANE_OFFSET_RX_ADDR,          063  */ (0x09),
  //------------------------------------------------------------------------------------------------
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_TXB_RES_CODE_LANE_OFFSET_TX_ADDR,          064  */ (0x06),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_FLL_CNTRL2_ADDR,                               065  */ (0x83),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_FLL_CNT_VAL_L_ADDR,                            066  */ (0x09),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_FLL_CNT_VAL_H_TOL_ADDR,                        067  */ (0xa2),
  //------------------------------------------------------------------------------------------------
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_FLL_MAN_CODE_ADDR,                             068  */ (0x40),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_FLL_CNTRL1_ADDR,                               069  */ (0x02),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_LOCK_DETECT_CONFIG1_ADDR,                      070  */ (0xd1),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_LOCK_DETECT_CONFIG2_ADDR,                      071  */ (0x1f),
  //------------------------------------------------------------------------------------------------
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_LOCK_DETECT_CONFIG3_ADDR,                      072  */ (0x47),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_POWER_STATE_CONFIG2_ADDR,                      073  */ (0x1b),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_RXA_UCDR_SO_SATURATION_AND_ENABLE_ADDR,    074  */ (0x75),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_QSERDES_RXB_UCDR_SO_SATURATION_AND_ENABLE_ADDR,    075  */ (0x75),
  //------------------------------------------------------------------------------------------------
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_RX_SIGDET_LVL_ADDR,                            076  */ (0xCC),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_TXMGN_V0_ADDR,                                 077  */ (0x9F),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_TXMGN_V1_ADDR,                                 078  */ (0x9F),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_TXMGN_V2_ADDR,                                 079  */ (0xB7),
  //------------------------------------------------------------------------------------------------
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_TXMGN_V3_ADDR,                                 080  */ (0x4E),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_TXMGN_V4_ADDR,                                 081  */ (0x65),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_TXMGN_LS_ADDR,                                 082  */ (0x6B),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_TXDEEMPH_M6DB_V0_ADDR,                         083  */ (0x15),
  //------------------------------------------------------------------------------------------------
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_TXDEEMPH_M3P5DB_V0_ADDR,                       084  */ (0x0D),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_TXDEEMPH_M6DB_V1_ADDR,                         085  */ (0x15),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_TXDEEMPH_M3P5DB_V1_ADDR,                       086  */ (0x0D),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_TXDEEMPH_M6DB_V2_ADDR,                         087  */ (0x15),
  //------------------------------------------------------------------------------------------------
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_TXDEEMPH_M3P5DB_V2_ADDR,                       088  */ (0x0D),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_TXDEEMPH_M6DB_V3_ADDR,                         089  */ (0x15),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_TXDEEMPH_M3P5DB_V3_ADDR,                       090  */ (0x1D),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_TXDEEMPH_M6DB_V4_ADDR,                         091  */ (0x15),
  //------------------------------------------------------------------------------------------------
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_TXDEEMPH_M3P5DB_V4_ADDR,                       092  */ (0x0D),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_TXDEEMPH_M6DB_LS_ADDR,                         093  */ (0x15),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_TXDEEMPH_M3P5DB_LS_ADDR,                       094  */ (0x0D),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_REFGEN_REQ_CONFIG1_ADDR,                            */ (0x21),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_REFGEN_REQ_CONFIG2_ADDR,                            */ (0x60),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_RATE_SLEW_CNTRL_ADDR,                          095  */ (0x02),
  //------------------------------------------------------------------------------------------------
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_PWRUP_RESET_DLY_TIME_AUXCLK_ADDR,              096  */ (0x04),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_TSYNC_RSYNC_TIME_ADDR,                         097  */ (0x44),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_RCVR_DTCT_DLY_P1U2_L_ADDR,                     098  */ (0xE7),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_RCVR_DTCT_DLY_P1U2_H_ADDR,                     099  */ (0x03),
  //------------------------------------------------------------------------------------------------
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_RCVR_DTCT_DLY_U3_L_ADDR,                       100  */ (0x40),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_RCVR_DTCT_DLY_U3_H_ADDR,                       101  */ (0x00),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_RXEQTRAINING_WAIT_TIME_ADDR,                   102  */ (0x75),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_LFPS_TX_ECSTART_EQTLOCK_ADDR,                  103  */ (0x86),
  //------------------------------------------------------------------------------------------------
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_RXEQTRAINING_RUN_TIME_ADDR,                    104  */ (0x13),
  /* HWIO_PERIPH_SS_USB3_DP_PHY_USB3_PCS_LFPS_DET_HIGH_COUNT_VAL_ADDR,                  105  */ (0x04),
  /* QUSB_HWIO_ADDR_END,                                                                106  */ (0x00),
  /* QUSB_HWIO_ADDR_END,                                                                107  */ (0x00),
};


//============================================================================
// QUSB Super-Speed Secondary PHY Configuration Array
//============================================================================

ALIGN(4)
static const uint32 qusb_dci_ss_phy_sec_cfg_address_common[QUSB_DCI_SS_PHY_SEC_COMMON_CFG_ARRAY_ENTRY_CNT] =
{
  HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_POWER_DOWN_CONTROL_ADDR,            // 001  : (0x01)
  HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_PLL_IVCO_ADDR,                            // 002  : (0x07)
  HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_SYSCLK_EN_SEL_ADDR,                       // 003  : (0x14)
  HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_BIAS_EN_CLKBUFLR_EN_ADDR,                 // 004  : (0x04)
  //------------------------------------------------------------------------------------------------
  HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_CLK_SELECT_ADDR,                          // 005  : (0x30)
  HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_SYS_CLK_CTRL_ADDR,                        // 006  : (0x02)  
  HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_RESETSM_CNTRL2_ADDR,                      // 007  : (0x08)
  HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_CMN_CONFIG_ADDR,                          // 008  : (0x06)
  //------------------------------------------------------------------------------------------------
  HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_SVS_MODE_CLK_SEL_ADDR,                    // 009  : (0x01)
  HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_HSCLK_SEL_ADDR,                           // 010  : (0x80)  
  HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_DEC_START_MODE0_ADDR,                     // 011  : (0x82)
  HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_DIV_FRAC_START1_MODE0_ADDR,               // 012  : (0xAB)
  //------------------------------------------------------------------------------------------------
  HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_DIV_FRAC_START2_MODE0_ADDR,               // 013  : (0xEA)
  HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_DIV_FRAC_START3_MODE0_ADDR,               // 014  : (0x02)
  HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_CP_CTRL_MODE0_ADDR,                       // 015  : (0x06)
  HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_PLL_RCTRL_MODE0_ADDR,                     // 016  : (0x16)
  //------------------------------------------------------------------------------------------------
  HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_PLL_CCTRL_MODE0_ADDR,                     // 017  : (0x36)
  HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_INTEGLOOP_GAIN1_MODE0_ADDR,               // 018  : (0x00)
  HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_INTEGLOOP_GAIN0_MODE0_ADDR,               // 019  : (0x3F)
  HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_VCO_TUNE2_MODE0_ADDR,                     // 020  : (0x01)
  //------------------------------------------------------------------------------------------------
  HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_VCO_TUNE1_MODE0_ADDR,                     // 021  : (0xC9) 
  HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_CORECLK_DIV_MODE0_ADDR,                   // 022  : (0x0A)
  HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_LOCK_CMP3_MODE0_ADDR,                     // 023  : (0x00)
  HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_LOCK_CMP2_MODE0_ADDR,                     // 024  : (0x34)
  //------------------------------------------------------------------------------------------------
  HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_LOCK_CMP1_MODE0_ADDR,                     // 025  : (0x15)  
  HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_LOCK_CMP_EN_ADDR,                         // 026  : (0x04)
  HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_CORE_CLK_EN_ADDR,                         // 027  : (0x00)
  HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_LOCK_CMP_CFG_ADDR,                        // 028  : (0x00)
  //------------------------------------------------------------------------------------------------
  HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_VCO_TUNE_MAP_ADDR,                        // 029  : (0x00)
  HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_SYSCLK_BUF_ENABLE_ADDR,                   // 030  : (0x0A)
  HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_INTEGLOOP_INITVAL_ADDR,                   // 031  : (0x80)  
  HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_SSC_EN_CENTER_ADDR,                       // 032  : (0x01)
  //------------------------------------------------------------------------------------------------
  HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_SSC_PER1_ADDR,                            // 033  : (0x31)
  HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_SSC_PER2_ADDR,                            // 034  : (0x01)
  HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_SSC_ADJ_PER1_ADDR,                        // 035  : (0x00)
  HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_SSC_ADJ_PER2_ADDR,                        // 036  : (0x00)
  //------------------------------------------------------------------------------------------------  
  HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_SSC_STEP_SIZE1_ADDR,                      // 037  : (0x85)
  HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_SSC_STEP_SIZE2_ADDR,                      // 038  : (0x07)
  HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_RX_VGA_CAL_CNTRL2_ADDR,                       // 039  : (0x0C)
  HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_RX_RX_MODE_00_ADDR,                           // 040  : (0x59)
  //------------------------------------------------------------------------------------------------  
  HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_RX_UCDR_FASTLOCK_FO_GAIN_ADDR,                // 041  : (0x0B)
  HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_RX_RX_EQU_ADAPTOR_CNTRL2_ADDR,                // 042  : (0x0E)
  HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_RX_RX_EQU_ADAPTOR_CNTRL3_ADDR,                // 043  : (0x4E)
  HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_RX_RX_EQU_ADAPTOR_CNTRL4_ADDR,                // 044  : (0x18)  
  //------------------------------------------------------------------------------------------------  
  HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_RX_RX_EQ_OFFSET_ADAPTOR_CNTRL1_ADDR,          // 045  : (0x77)
  HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_RX_RX_OFFSET_ADAPTOR_CNTRL2_ADDR,             // 046  : (0x80)  
  HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_RX_SIGDET_CNTRL_ADDR,                         // 047  : (0x03)
  HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_RX_SIGDET_DEGLITCH_CNTRL_ADDR,                // 048  : (0x1C)  
  //------------------------------------------------------------------------------------------------  
  HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_RX_UCDR_SO_SATURATION_AND_ENABLE_ADDR,        // 049  : (0x75)
  HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_RX_UCDR_PI_CONTROLS_ADDR,                     // 050  : (0x80)
  HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_RX_UCDR_FO_GAIN_ADDR,                         // 051  : (0x0A)
  HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_RX_UCDR_SO_GAIN_ADDR,                         // 052  : (0x06)  
  //------------------------------------------------------------------------------------------------
  HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_RX_SIGDET_ENABLES_ADDR,                       // 053  : (0x00)  
  HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_TX_HIGHZ_DRVR_EN_ADDR,                        // 054  : (0x10)
  HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_TX_RCV_DETECT_LVL_2_ADDR,                     // 055  : (0x12)
  HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_TX_LANE_MODE_1_ADDR,                          // 056  : (0xC6)  
  //------------------------------------------------------------------------------------------------  
  HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_TX_RES_CODE_LANE_OFFSET_RX_ADDR,              // 057  : (0x06)  //Based on Si char
  HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_TX_RES_CODE_LANE_OFFSET_TX_ADDR,              // 058  : (0x06)  //Based on si char
  HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_FLL_CNTRL2_ADDR,                    // 059  : (0x83)
  HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_FLL_CNT_VAL_L_ADDR,                 // 060  : (0x09)
  //------------------------------------------------------------------------------------------------
  HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_FLL_CNT_VAL_H_TOL_ADDR,             // 061  : (0xA2)
  HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_FLL_MAN_CODE_ADDR,                  // 062  : (0x40)  
  HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_FLL_CNTRL1_ADDR,                    // 063  : (0x02)
  HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_POWER_STATE_CONFIG2_ADDR,           // 064  : (0x1B)  
  //------------------------------------------------------------------------------------------------  
  HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_TXMGN_V0_ADDR,                      // 065  : (0x9F)
  HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_TXMGN_V1_ADDR,                      // 066  : (0x9F)
  HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_TXMGN_V2_ADDR,                      // 067  : (0xB5)
  HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_TXMGN_V3_ADDR,                      // 068  : (0x4C)  
  //------------------------------------------------------------------------------------------------  
  HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_TXMGN_V4_ADDR,                      // 069  : (0x64)
  HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_TXMGN_LS_ADDR,                      // 070  : (0x6A)
  HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_TXDEEMPH_M6DB_V0_ADDR,              // 071  : (0x15)
  HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_TXDEEMPH_M3P5DB_V0_ADDR,            // 072  : (0x0D)
  //------------------------------------------------------------------------------------------------
  HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_TXDEEMPH_M6DB_V1_ADDR,              // 073  : (0x15)
  HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_TXDEEMPH_M3P5DB_V1_ADDR,            // 074  : (0x0D)
  HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_TXDEEMPH_M6DB_V2_ADDR,              // 075  : (0x15)
  HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_TXDEEMPH_M3P5DB_V2_ADDR,            // 076  : (0x0D)
  //------------------------------------------------------------------------------------------------
  HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_TXDEEMPH_M6DB_V3_ADDR,              // 077  : (0x15)
  HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_TXDEEMPH_M3P5DB_V3_ADDR,            // 078  : (0x0D)
  HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_TXDEEMPH_M6DB_V4_ADDR,              // 079  : (0x15)
  HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_TXDEEMPH_M3P5DB_V4_ADDR,            // 080  : (0x0D)
  //------------------------------------------------------------------------------------------------
  HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_TXDEEMPH_M6DB_LS_ADDR,              // 081  : (0x15)
  HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_TXDEEMPH_M3P5DB_LS_ADDR,            // 082  : (0x0D) 
  HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_RATE_SLEW_CNTRL_ADDR,               // 083  : (0x02)
  HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_PWRUP_RESET_DLY_TIME_AUXCLK_ADDR,   // 084  : (0x04)
  //------------------------------------------------------------------------------------------------
  HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_TSYNC_RSYNC_TIME_ADDR,              // 085  : (0x44)
  HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_LOCK_DETECT_CONFIG1_ADDR,           // 086  : (0xD1)
  HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_LOCK_DETECT_CONFIG2_ADDR,           // 087  : (0x1F)
  HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_LOCK_DETECT_CONFIG3_ADDR,           // 088  : (0x47)
  //------------------------------------------------------------------------------------------------  
  HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_RCVR_DTCT_DLY_P1U2_L_ADDR,          // 089  : (0xE7)
  HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_RCVR_DTCT_DLY_P1U2_H_ADDR,          // 090  : (0x03)
  HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_RCVR_DTCT_DLY_U3_L_ADDR,            // 091  : (0x40)
  HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_RCVR_DTCT_DLY_U3_H_ADDR,            // 092  : (0x00)  
  //------------------------------------------------------------------------------------------------ 
  HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_RX_SIGDET_LVL_ADDR,                 // 093  : (0xCC)
  HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_RXEQTRAINING_WAIT_TIME_ADDR,        // 094  : (0x75)
  HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_LFPS_TX_ECSTART_EQTLOCK_ADDR,       // 095  : (0x86)
  HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_RXEQTRAINING_RUN_TIME_ADDR,         // 096  : (0x13)
  //------------------------------------------------------------------------------------------------  
  HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_REFGEN_REQ_CONFIG1_ADDR,            // 097  : (0x21)
  HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_REFGEN_REQ_CONFIG2_ADDR,            // 098  : (0x60)
  QUSB_HWIO_ADDR_EMPTY,                                                             // 099  : (0x00)
  QUSB_HWIO_ADDR_EMPTY,                                                             // 100  : (0x00)
};

ALIGN(4)
static const uint8 qusb_dci_ss_phy_sec_cfg_value_common[QUSB_DCI_SS_PHY_SEC_COMMON_CFG_ARRAY_ENTRY_CNT] =
{
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_POWER_DOWN_CONTROL_ADDR,             001  */ (0x01),
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_PLL_IVCO_ADDR,                             002  */ (0x07),
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_SYSCLK_EN_SEL_ADDR,                        003  */ (0x14),
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_BIAS_EN_CLKBUFLR_EN_ADDR,                  004  */ (0x04),
  //------------------------------------------------------------------------------------------------
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_CLK_SELECT_ADDR,                           005  */ (0x30),
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_SYS_CLK_CTRL_ADDR,                         006  */ (0x02),  
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_RESETSM_CNTRL2_ADDR,                       007  */ (0x08),
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_CMN_CONFIG_ADDR,                           008  */ (0x06),
  //------------------------------------------------------------------------------------------------
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_SVS_MODE_CLK_SEL_ADDR,                     009  */ (0x01),
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_HSCLK_SEL_ADDR,                            010  */ (0x80),  
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_DEC_START_MODE0_ADDR,                      011  */ (0x82),
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_DIV_FRAC_START1_MODE0_ADDR,                012  */ (0xAB),
  //------------------------------------------------------------------------------------------------
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_DIV_FRAC_START2_MODE0_ADDR,                013  */ (0xEA),
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_DIV_FRAC_START3_MODE0_ADDR,                014  */ (0x02),
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_CP_CTRL_MODE0_ADDR,                        015  */ (0x06),
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_PLL_RCTRL_MODE0_ADDR,                      016  */ (0x16),
  //------------------------------------------------------------------------------------------------
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_PLL_CCTRL_MODE0_ADDR,                      017  */ (0x36),
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_INTEGLOOP_GAIN1_MODE0_ADDR,                018  */ (0x00),
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_INTEGLOOP_GAIN0_MODE0_ADDR,                019  */ (0x3F),
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_VCO_TUNE2_MODE0_ADDR,                      020  */ (0x01),
  //------------------------------------------------------------------------------------------------
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_VCO_TUNE1_MODE0_ADDR,                      021  */ (0xC9), 
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_CORECLK_DIV_MODE0_ADDR,                    022  */ (0x0A),
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_LOCK_CMP3_MODE0_ADDR,                      023  */ (0x00),
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_LOCK_CMP2_MODE0_ADDR,                      024  */ (0x34),
  //------------------------------------------------------------------------------------------------
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_LOCK_CMP1_MODE0_ADDR,                      025  */ (0x15),  
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_LOCK_CMP_EN_ADDR,                          026  */ (0x04),
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_CORE_CLK_EN_ADDR,                          027  */ (0x00),
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_LOCK_CMP_CFG_ADDR,                         028  */ (0x00),
  //------------------------------------------------------------------------------------------------
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_VCO_TUNE_MAP_ADDR,                         029  */ (0x00),
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_SYSCLK_BUF_ENABLE_ADDR,                    030  */ (0x0A),
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_INTEGLOOP_INITVAL_ADDR,                    031  */ (0x80),  
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_SSC_EN_CENTER_ADDR,                        032  */ (0x01),
  //------------------------------------------------------------------------------------------------
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_SSC_PER1_ADDR,                             033  */ (0x31),
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_SSC_PER2_ADDR,                             034  */ (0x01),
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_SSC_ADJ_PER1_ADDR,                         035  */ (0x00),
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_SSC_ADJ_PER2_ADDR,                         036  */ (0x00),
  //------------------------------------------------------------------------------------------------  
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_SSC_STEP_SIZE1_ADDR,                       037  */ (0x85),
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_COM_SSC_STEP_SIZE2_ADDR,                       038  */ (0x07),
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_RX_VGA_CAL_CNTRL2_ADDR,                        039  */ (0x0C),
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_RX_RX_MODE_00_ADDR,                            040  */ (0x59),
  //------------------------------------------------------------------------------------------------  
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_RX_UCDR_FASTLOCK_FO_GAIN_ADDR,                 041  */ (0x0B),
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_RX_RX_EQU_ADAPTOR_CNTRL2_ADDR,                 042  */ (0x0E),
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_RX_RX_EQU_ADAPTOR_CNTRL3_ADDR,                 043  */ (0x4E),
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_RX_RX_EQU_ADAPTOR_CNTRL4_ADDR,                 044  */ (0x18),  
  //------------------------------------------------------------------------------------------------  
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_RX_RX_EQ_OFFSET_ADAPTOR_CNTRL1_ADDR,           045  */ (0x77),
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_RX_RX_OFFSET_ADAPTOR_CNTRL2_ADDR,              046  */ (0x80),  
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_RX_SIGDET_CNTRL_ADDR,                          047  */ (0x03),
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_RX_SIGDET_DEGLITCH_CNTRL_ADDR,                 048  */ (0x1C),  
  //------------------------------------------------------------------------------------------------  
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_RX_UCDR_SO_SATURATION_AND_ENABLE_ADDR,         049  */ (0x75),
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_RX_UCDR_PI_CONTROLS_ADDR,                      050  */ (0x80),
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_RX_UCDR_FO_GAIN_ADDR,                          051  */ (0x0A),
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_RX_UCDR_SO_GAIN_ADDR,                          052  */ (0x06),  
  //------------------------------------------------------------------------------------------------
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_RX_SIGDET_ENABLES_ADDR,                        053  */ (0x00),  
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_TX_HIGHZ_DRVR_EN_ADDR,                         054  */ (0x10),
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_TX_RCV_DETECT_LVL_2_ADDR,                      055  */ (0x12),
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_TX_LANE_MODE_1_ADDR,                           056  */ (0xC6),  
  //------------------------------------------------------------------------------------------------  
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_TX_RES_CODE_LANE_OFFSET_RX_ADDR,               057  */ (0x06),  //Based on Si char
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_QSERDES_TX_RES_CODE_LANE_OFFSET_TX_ADDR,               058  */ (0x06),  //Based on si char
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_FLL_CNTRL2_ADDR,                     059  */ (0x83),
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_FLL_CNT_VAL_L_ADDR,                  060  */ (0x09),
  //------------------------------------------------------------------------------------------------
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_FLL_CNT_VAL_H_TOL_ADDR,              061  */ (0xA2),
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_FLL_MAN_CODE_ADDR,                   062  */ (0x40),  
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_FLL_CNTRL1_ADDR,                     063  */ (0x02),
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_POWER_STATE_CONFIG2_ADDR,            064  */ (0x1B),  
  //------------------------------------------------------------------------------------------------  
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_TXMGN_V0_ADDR,                       065  */ (0x9F),
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_TXMGN_V1_ADDR,                       066  */ (0x9F),
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_TXMGN_V2_ADDR,                       067  */ (0xB5),
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_TXMGN_V3_ADDR,                       068  */ (0x4C),  
  //------------------------------------------------------------------------------------------------  
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_TXMGN_V4_ADDR,                       069  */ (0x64),
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_TXMGN_LS_ADDR,                       070  */ (0x6A),
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_TXDEEMPH_M6DB_V0_ADDR,               071  */ (0x15),
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_TXDEEMPH_M3P5DB_V0_ADDR,             072  */ (0x0D),
  //------------------------------------------------------------------------------------------------
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_TXDEEMPH_M6DB_V1_ADDR,               073  */ (0x15),
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_TXDEEMPH_M3P5DB_V1_ADDR,             074  */ (0x0D),
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_TXDEEMPH_M6DB_V2_ADDR,               075  */ (0x15),
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_TXDEEMPH_M3P5DB_V2_ADDR,             076  */ (0x0D),
  //------------------------------------------------------------------------------------------------
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_TXDEEMPH_M6DB_V3_ADDR,               077  */ (0x15),
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_TXDEEMPH_M3P5DB_V3_ADDR,             078  */ (0x0D),
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_TXDEEMPH_M6DB_V4_ADDR,               079  */ (0x15),
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_TXDEEMPH_M3P5DB_V4_ADDR,             080  */ (0x0D),
  //------------------------------------------------------------------------------------------------
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_TXDEEMPH_M6DB_LS_ADDR,               081  */ (0x15),
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_TXDEEMPH_M3P5DB_LS_ADDR,             082  */ (0x0D), 
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_RATE_SLEW_CNTRL_ADDR,                083  */ (0x02),
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_PWRUP_RESET_DLY_TIME_AUXCLK_ADDR,    084  */ (0x04),
  //------------------------------------------------------------------------------------------------
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_TSYNC_RSYNC_TIME_ADDR,               085  */ (0x44),
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_LOCK_DETECT_CONFIG1_ADDR,            086  */ (0xD1),
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_LOCK_DETECT_CONFIG2_ADDR,            087  */ (0x1F),
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_LOCK_DETECT_CONFIG3_ADDR,            088  */ (0x47),
  //------------------------------------------------------------------------------------------------  
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_RCVR_DTCT_DLY_P1U2_L_ADDR,           089  */ (0xE7),
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_RCVR_DTCT_DLY_P1U2_H_ADDR,           090  */ (0x03),
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_RCVR_DTCT_DLY_U3_L_ADDR,             091  */ (0x40),
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_RCVR_DTCT_DLY_U3_H_ADDR,             092  */ (0x00),  
  //------------------------------------------------------------------------------------------------ 
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_RX_SIGDET_LVL_ADDR,                  093  */ (0xCC),
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_RXEQTRAINING_WAIT_TIME_ADDR,         094  */ (0x75),
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_LFPS_TX_ECSTART_EQTLOCK_ADDR,        095  */ (0x86),
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_RXEQTRAINING_RUN_TIME_ADDR,          096  */ (0x13),
  //------------------------------------------------------------------------------------------------  
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_REFGEN_REQ_CONFIG1_ADDR,             097  */ (0x21),
  /* HWIO_PERIPH_SS_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_REFGEN_REQ_CONFIG2_ADDR,             098  */ (0x60),
  /* QUSB_HWIO_ADDR_EMPTY,                                                              099  */ (0x00),
  /* QUSB_HWIO_ADDR_EMPTY,                                                              100  */ (0x00),
};

