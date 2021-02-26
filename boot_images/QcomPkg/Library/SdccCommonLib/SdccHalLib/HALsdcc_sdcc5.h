#ifndef HALSDCC_SDCC5_H
#define HALSDCC_SDCC5_H

/**********************************************************************
 * HALsdcc_sdcc5.h
 *
 * This is a PRIVATE header file for internal use of sdcc HAL and is
 * not intended to be included by any other module other than sdcc
 * HAL.
 *
 * This header file declares internal functions common to all SDCC5
 * HAL implementations.
 *
 * Copyright (c) 2014-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential 
 *
 **********************************************************************/

/*======================================================================

                        EDIT HISTORY FOR MODULE

when         who     what, where, why
--------     ---     ---------------------------------------------------
2017-11-06   sb      Update prg,rclk delay config to fix data CRC errors
2017-06-12   jt      Adjust overwrite of SD card capabilities  
2017-05-12   sb      Add eMMC support for SDM670  
2017-04-28   jt      Fix offsets for Napali
2016-12-12   jt      Remove legacy code  
2016-01-18   eo      Reduced sdcc code size
2015-11-19   jt      Correct SW reset sequence 
2015-09-18   whc     Added V3 register address change
2015-08-16   bn      Fixed FF_CLK_SW_RST_DIS not being set for HS400
2015-07-15   bn      Fixed SW Reset, added busywait(), added SDHCi's FLOW Control
2015-03-30   bn      Fixed 64-bit address range adma issue
2015-03-09   bn      Fixed Caps1 Bits[24:31] being overwritten by SetCaps
2015-02-27   rm      Added HAL_sdhci_DLLClockEnable
2015-02-12   dg      Runtime fixes
2015-01-26   rm      Integraged HS200/HS400 support for eMMC from UEFI 3.0
2014-09-24   bn      Added HAL_sdhci_SetCapabilities()
2014-06-18   bn      Initial version. Branched from 8994 UEFI
=======================================================================*/

#include "SdccHal.h"
#include <Library/PcdLib.h>
#include <api/systemdrivers/busywait.h>

#define HAL_SDCC_CONTROLLER_COUNT    4
#define HAL_SDHCI_POLL_MAX           10000

/* Define the offset of each of the register from base - SDHCI mode*/
#define HWIO_SDC_HC_REG_0_2_REG_IDX                           (0x0000)
#define HWIO_SDC_HC_REG_4_REG_IDX                             (0x0004)
#define HWIO_SDC_HC_REG_6_REG_IDX                             (0x0006)
#define HWIO_SDC_HC_REG_8_A_REG_IDX                           (0x0008)
#define HWIO_SDC_HC_REG_C_REG_IDX                             (0x000C)
#define HWIO_SDC_HC_REG_E_REG_IDX                             (0x000E)
#define HWIO_SDC_HC_REG_10_12_REG_IDX                         (0x0010)
#define HWIO_SDC_HC_REG_14_16_REG_IDX                         (0x0014)
#define HWIO_SDC_HC_REG_18_1A_REG_IDX                         (0x0018)
#define HWIO_SDC_HC_REG_1C_1E_REG_IDX                         (0x001C)
#define HWIO_SDC_HC_REG_20_22_REG_IDX                         (0x0020)
#define HWIO_SDC_HC_REG_24_26_REG_IDX                         (0x0024)
#define HWIO_SDC_HC_REG_28_REG_IDX                            (0x0028)
#define HWIO_SDC_HC_REG_29_REG_IDX                            (0x0029)
#define HWIO_SDC_HC_REG_2A_REG_IDX                            (0x002A)
#define HWIO_SDC_HC_REG_2B_REG_IDX                            (0x002B)
#define HWIO_SDC_HC_REG_2C_REG_IDX                            (0x002C)
#define HWIO_SDC_HC_REG_2E_REG_IDX                            (0x002E)
#define HWIO_SDC_HC_REG_2F_REG_IDX                            (0x002F)
#define HWIO_SDC_HC_REG_30_32_REG_IDX                         (0x0030)
#define HWIO_SDC_HC_REG_34_36_REG_IDX                         (0x0034)
#define HWIO_SDC_HC_REG_38_3A_REG_IDX                         (0x0038)
#define HWIO_SDC_HC_REG_3C_REG_IDX                            (0x003C)
#define HWIO_SDC_HC_REG_3E_REG_IDX                            (0x003E)
#define HWIO_SDC_HC_REG_40_42_REG_IDX                         (0x0040)
#define HWIO_SDC_HC_REG_44_46_REG_IDX                         (0x0044)
#define HWIO_SDC_HC_REG_48_4A_REG_IDX                         (0x0048)
#define HWIO_SDC_HC_REG_50_52_REG_IDX                         (0x0050)
#define HWIO_SDC_HC_REG_54_56_REG_IDX                         (0x0054)
#define HWIO_SDC_HC_REG_58_5A_REG_IDX                         (0x0058)
#define HWIO_SDC_HC_REG_5C_5E_REG_IDX                         (0x005C)
#define HWIO_SDC_HC_REG_60_62_REG_IDX                         (0x0060)
#define HWIO_SDC_HC_REG_64_66_REG_IDX                         (0x0064)
#define HWIO_SDC_HC_REG_68_6A_REG_IDX                         (0x0068)
#define HWIO_SDC_HC_REG_6C_6E_REG_IDX                         (0x006C)
#define HWIO_SDC_HC_REG_E0_E2_REG_IDX                         (0x00E0)
#define HWIO_SDC_HC_REG_FC_FE_REG_IDX                         (0x00FC)
#define HWIO_SDC_HC_REG_DLL_CONFIG_REG_IDX                    (0x0200)
#define HWIO_SDC_HC_REG_DLL_TEST_CTL_REG_IDX                  (0x0204)
#define HWIO_SDC_HC_REG_DLL_STATUS_REG_IDX                    (0x0208)
#define HWIO_SDC_HC_VENDOR_SPECIFIC_FUNC_REG_IDX              (0x020C)
#define HWIO_SDC_HC_VENDOR_SPECIFIC_FUNC2_REG_IDX             (0x0210)
#define HWIO_SDC_HC_VENDOR_SPECIFIC_ADMA_ERR_ADDR0_REG_IDX    (0x0214)
#define HWIO_SDC_HC_VENDOR_CAPS0_REG_IDX                      (0x021C)
#define HWIO_SDC_HC_VENDOR_CAPS1_REG_IDX                      (0x0220)
#define HWIO_SDC_HC_VENDOR_SPECIFIC_DDR200_CFG_REG_IDX        (0x0224) 
#define HWIO_SDC_HC_VENDOR_SPECIFIC_PWRCTL_STATUS_REG_IDX     (0x0240)
#define HWIO_SDC_HC_VENDOR_SPECIFIC_PWRCTL_MASK_REG_IDX       (0x0244)
#define HWIO_SDC_HC_VENDOR_SPECIFIC_PWRCTL_CLEAR_REG_IDX      (0x0248)
#define HWIO_SDC_HC_VENDOR_SPECIFIC_PWRCTL_CTL_REG_IDX        (0x024C)
#define HWIO_SDC_HC_VENDOR_SPECIFIC_FUNC3_REG_IDX             (0x0250) 
#define HWIO_SDC_HC_REG_DLL_CONFIG_2_REG_IDX                  (0x0254)
#define HWIO_SDC_HC_REG_DLL_CONFIG_3_REG_IDX                  (0x0258)
#define HWIO_SDC_HC_REG_DDR_CONFIG_REG_IDX                    (0x025C)

/* Transfer Mode Register */
#define HWIO_HC_REG_TRANS_MODE_MULTI_SINGLE_BLK_SEL_SHIFT     0x5
#define HWIO_HC_REG_TRANS_MODE_DATA_DIRECTION_SEL_SHIFT       0x4
#define HWIO_HC_REG_TRANS_MODE_AUTO_CMD_EN_SHIFT              0x2
#define HWIO_HC_REG_TRANS_MODE_BLK_CNT_EN_SHIFT               0x1

/* Command Register */
#define HWIO_HC_REG_CMD_INDX_SHIFT                            0x8
#define HWIO_HC_REG_CMD_TYPE_SHIFT                            0x6
#define HWIO_HC_REG_CMD_DATA_PRESENT_SEL_SHIFT                0x5
#define HWIO_HC_REG_CMD_INDX_CHECK_SHIFT                      0x4
#define HWIO_HC_REG_CMD_CRC_CHECK_SHIFT                       0x3

/* Host Control 1 Register */
#define HWIO_HC_REG_HST_CTL1_EXTENDED_DATA_TRANS_WIDTH_BMSK   0x20
#define HWIO_HC_REG_HST_CTL1_DMA_SEL_BMSK                     0x18
#define HWIO_HC_REG_HST_CTL1_DATA_TRANS_WIDTH_BMSK            0x2
#define HC_REG_HST_CTL1_DATA_TRANS_WIDTH_8_BIT_BMSK           0x20
#define HC_REG_HST_CTL1_DATA_TRANS_WIDTH_4_BIT_BMSK           0x2
#define HC_REG_HST_CTL1_DATA_TRANS_WIDTH_1_BIT_BMSK           0x0
#define HC_REG_HST_CTL1_64_BIT_ADMA2_ENABLE_BMSK              0x18
#define HC_REG_HST_CTL1_32_BIT_ADMA2_ENABLE_BMSK              0x10

/* Power Control Register */
#define HWIO_HC_REG_PWR_CTL_SD_BUS_VOLTAGE_SEL_BMSK           0xE 
#define HWIO_HC_REG_PWR_CTL_SD_BUS_PWR_BMSK                   0x1  

/* Clock Control Register */
#define HWIO_HC_REG_CLK_CTL_SDCLK_FREQ_SEL_SHFT               0x8
#define HWIO_HC_REG_CLK_CTL_SDCLK_FREQ_SEL_MSB_SHFT           0x6
#define HWIO_HC_REG_CLK_CTL_SDCLK_EN_BMSK                     0x4   
#define HWIO_HC_REG_CLK_CTL_SDCLK_EN_SHFT                     0x2
#define HWIO_HC_REG_CLK_CTL_INTERNAL_CLK_STABLE_BMSK          0x2 
#define HWIO_HC_REG_CLK_CTL_INTERNAL_CLK_STABLE_SHFT          0x1
#define HWIO_HC_REG_CLK_CTL_INTERNAL_CLK_EN_BMSK              0x1 
#define HWIO_HC_REG_CLK_CTL_INTERNAL_CLK_EN_SHFT              0x0

/* Host Control 2 Register */
#define HWIO_HC_REG_HST_CTL2_UHS_MODE_SEL_BMSK                0x7

/* DLL Config Register */
#define HWIO_HC_REG_DLL_CONFIG_DLL_RST_BMSK                   0x40000000
#define HWIO_HC_REG_DLL_CONFIG_DLL_RST_SHFT                   0x1e
#define HWIO_HC_REG_DLL_CONFIG_PDN_BMSK                       0x20000000
#define HWIO_HC_REG_DLL_CONFIG_PDN_SHFT                       0x1d
#define HWIO_HC_REG_DLL_CONFIG_MCLK_FREQ_BMSK                 0x7000000
#define HWIO_HC_REG_DLL_CONFIG_MCLK_FREQ_SHFT                 0x18
#define HWIO_HC_REG_DLL_CONFIG_CDR_SELEXT_BMSK                0xf00000
#define HWIO_HC_REG_DLL_CONFIG_CDR_SELEXT_SHFT                0x14
#define HWIO_HC_REG_DLL_CONFIG_CDR_EXT_EN_BMSK                0x80000
#define HWIO_HC_REG_DLL_CONFIG_CDR_EXT_EN_SHFT                0x13
#define HWIO_HC_REG_DLL_CONFIG_CK_OUT_EN_BMSK                 0x40000
#define HWIO_HC_REG_DLL_CONFIG_CK_OUT_EN_SHFT                 0x12
#define HWIO_HC_REG_DLL_CONFIG_CDR_EN_BMSK                    0x20000
#define HWIO_HC_REG_DLL_CONFIG_CDR_EN_SHFT                    0x11
#define HWIO_HC_REG_DLL_CONFIG_DLL_EN_BMSK                    0x10000
#define HWIO_HC_REG_DLL_CONFIG_DLL_EN_SHFT                    0x10
#define HWIO_HC_REG_DLL_CONFIG_CMD_DAT_TRACK_SEL_BMSK         0x1
#define HC_DLL_CONFIG_CK_OUT_EN_SET                           (1UL << HWIO_HC_REG_DLL_CONFIG_CK_OUT_EN_SHFT)

/* DLL Status Register */
#define HWIO_HC_REG_DLL_STATUS_DDR_DLL_LOCK_JDR_BMSK          0x800
#define HWIO_HC_REG_DLL_STATUS_DLL_LOCK_BMSK                  0x80
#define HWIO_HC_REG_DLL_STATUS_DLL_LOCK_SHFT                  0x7
#define HWIO_HC_REG_DLL_STATUS_CDR_PHASE_BMSK                 0x78
#define HWIO_HC_REG_DLL_STATUS_CDR_PHASE_SHFT                 0x3
#define HC_DLL_STATUS_DLL_LOCK                                (1UL << HWIO_HC_REG_DLL_STATUS_DLL_LOCK_SHFT)

#define HWIO_HC_VENDOR_SPECIFIC_FUNC_HC_SELECT_IN_BMSK        0x380000
#define HWIO_HC_VENDOR_SPECIFIC_FUNC_HC_SELECT_IN_SHFT        0x13
#define HWIO_HC_VENDOR_SPECIFIC_FUNC_HC_SELECT_IN_EN_BMSK     0x40000
#define HWIO_HC_VENDOR_SPECIFIC_FUNC_HC_SELECT_IN_EN_SHIFT    0x12
#define HWIO_HC_VENDOR_SPECIFIC_FUNC_SDC4_MCLK_SEL_BMSK       0x300 
#define HWIO_HC_VENDOR_SPECIFIC_FUNC_SDC4_MCLK_SEL_SHFT       0x8
#define HWIO_HC_VENDOR_SPECIFIC_FUNC_PWRSAVE_BMSK             0x2
#define HWIO_HC_VENDOR_SPECIFIC_FUNC_PWRSAVE_SHIFT            0x1
#define HC_VENDOR_SPECIFIC_PWRSAVE_SET                        (1UL << HWIO_HC_VENDOR_SPECIFIC_FUNC_PWRSAVE_SHIFT)

#define HWIO_HC_VENDOR_SPECIFIC_FUNC_FLOW_EN_BMSK             0x4
#define HWIO_HC_VENDOR_SPECIFIC_FUNC_FLOW_EN_SHIFT            0x2

#define HC_VENDOR_SPECIFIC_FLOW_EN_SET                        (1UL << HWIO_HC_VENDOR_SPECIFIC_FUNC_FLOW_EN_SHIFT)
#define HC_VENDOR_SPECIFIC_FLOW_EN_CLEAR                      (0UL << HWIO_HC_VENDOR_SPECIFIC_FUNC_FLOW_EN_SHIFT)

#define HWIO_HC_VENDOR_SPECIFIC_DDR200_CFG_CMDIN_RCLK_EN_BMSK           0x2
#define HWIO_HC_VENDOR_SPECIFIC_DDR200_CFG_CMDIN_RCLK_EN_SHFT           0x1

#define HWIO_HC_VENDOR_SPECIFIC_PWRCTL_CLEAR_BUS_ON_BMSK                0x2 
#define HWIO_HC_VENDOR_SPECIFIC_PWRCTL_CLEAR_BUS_OFF_BMSK               0x1 

/* Vendor Specific Pwrctl Registers */
#define HWIO_HC_VENDOR_SPECIFIC_PWRCTL_CTL_IO_SIG_SWITCH_SUCCESS_BMSK   0x4 
#define HWIO_HC_VENDOR_SPECIFIC_PWRCTL_CTL_BUS_ON_OFF_SUCCESS_BMSK      0x1 

/* Vendor Specific Function 3 Register */
#define HWIO_HC_VENDOR_SPECIFIC_FUNC3_PWRSAVE_DLL_BMSK                  0x8
#define HWIO_HC_VENDOR_SPECIFIC_FUNC3_PWRSAVE_DLL_SHFT                  0x3

/* DLL Config 2 Register */
#define HWIO_HC_REG_DLL_CONFIG_2_DLL_CLOCK_DISABLE_BMSK                 0x200000
#define HWIO_HC_REG_DLL_CONFIG_2_DDR_CAL_EN_BMSK                        0x1

/* DDR Config Register */
#define HWIO_HC_REG_DDR_CONFIG_PRG_RCLK_DLY_BMSK                        0x1ff
#define HWIO_HC_REG_DDR_CONFIG_PRG_RCLK_DLY_SHFT                        0x0


/* Define the required HWIO input/out macro mapping */
#define HWIO_SDC_REG_IN(reg, b) \
            (in_dword((HWIO_SDC_##reg##_REG_IDX+b)))
#define HWIO_SDC_REG_OUT(reg, b, v) \
            (out_dword((HWIO_SDC_##reg##_REG_IDX+b),v))
#define HWIO_SDC_WORD_OUT(reg, b, v) \
            (out_word((HWIO_SDC_##reg##_REG_IDX+b),v))
#define HWIO_SDC_WORD_IN(reg, b) \
            (in_word((HWIO_SDC_##reg##_REG_IDX+b)))
#define HWIO_SDC_BYTE_IN(reg, b) \
            (in_byte((HWIO_SDC_##reg##_REG_IDX+b)))
#define HWIO_SDC_BYTE_OUT(reg, b, v) \
            (out_byte((HWIO_SDC_##reg##_REG_IDX+b),v))

/* Table that stores the register location */
extern UINTN sdcc_reg_hc[]; 

/* Inline function that implements the simple I/Os */

inline void
HAL_sdcc_Delay(UINT32 us)
{
   busywait(us);
}

inline void
HAL_sdhci_FlowControl (const UINT32 nControllerId, BOOLEAN flow_enable)
{
  UINT32 val = HWIO_SDC_REG_IN (HC_VENDOR_SPECIFIC_FUNC, (UINTN)sdcc_reg_hc[nControllerId]);

  if (flow_enable)
  {
    /* Enable Flow Control */  
    val |= HC_VENDOR_SPECIFIC_FLOW_EN_SET;
  }
  else
  {
    /* Disable Flow Control */
    val &= ~HC_VENDOR_SPECIFIC_FLOW_EN_SET;
  }

  HWIO_SDC_REG_OUT (HC_VENDOR_SPECIFIC_FUNC, (UINTN)sdcc_reg_hc[nControllerId], val);
}

inline void
HAL_sdhci_PowerSave (const UINT32 nControllerId, BOOLEAN bEnable)
{
  UINT32 val = HWIO_SDC_REG_IN (HC_VENDOR_SPECIFIC_FUNC, (UINTN)sdcc_reg_hc[nControllerId]);

  if (bEnable)
  {
     val |= HC_VENDOR_SPECIFIC_PWRSAVE_SET;
  }
  else
  {
     val &= ~HC_VENDOR_SPECIFIC_PWRSAVE_SET;
  }
  HWIO_SDC_REG_OUT (HC_VENDOR_SPECIFIC_FUNC, (UINTN)sdcc_reg_hc[nControllerId], val);
}

/**
 * Reg Offset: 0x2F. 8-bit register
 * Issues software resets for CMD line, DAT line, or all software reset
 * Set to 1 to initiate a reset. Software must wait for the bit to be cleared
 * indicating the reset has completed.
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 *
 * @param ResetType
 *   indicates which reset to perform
 */
inline void
HAL_sdhci_SoftwareReset (const UINT32 nControllerId,
                         enum HAL_sdhci_reset ResetType)
{
  BOOLEAN power_on = FALSE; 
  UINT8 val = (UINT8)ResetType;
  UINT32 count = HAL_SDHCI_POLL_MAX;

  power_on = HWIO_SDC_BYTE_IN (HC_REG_29, 
       (UINTN)sdcc_reg_hc[nControllerId]) & HWIO_HC_REG_PWR_CTL_SD_BUS_PWR_BMSK;

  HWIO_SDC_BYTE_OUT (HC_REG_2F, (UINTN)sdcc_reg_hc[nControllerId], val);

  if (power_on && (HAL_SDHCI_RESET_ALL == ResetType))
  {
    HAL_sdhci_HandlePowerInterrupts(nControllerId); 
  }

  while (count--)
  {
    if (0 == (HWIO_SDC_BYTE_IN (HC_REG_2F, (UINTN)sdcc_reg_hc[nControllerId]) & val))
    {
      break;
    }
  }
}

inline void
HAL_sdhci_DllMclkSet (const UINT32 nControllerId, UINT32 mclk_freq)
{
   UINT32 reg_value = 0;
   reg_value = HWIO_SDC_REG_IN(HC_REG_DLL_CONFIG, (UINTN)sdcc_reg_hc[nControllerId]);

   reg_value &= ~HWIO_HC_REG_DLL_CONFIG_MCLK_FREQ_BMSK;
   reg_value |= (mclk_freq << HWIO_HC_REG_DLL_CONFIG_MCLK_FREQ_SHFT);

   HWIO_SDC_REG_OUT (HC_REG_DLL_CONFIG, (UINTN)sdcc_reg_hc[nControllerId], reg_value);
}

inline void
HAL_sdhci_DllRstPdn (const UINT32 nControllerId, BOOLEAN set_clr)
{
   UINT32 reg_value = 0;
   reg_value = HWIO_SDC_REG_IN(HC_REG_DLL_CONFIG, (UINTN)sdcc_reg_hc[nControllerId]);

   if(set_clr)
   {
      reg_value |= HWIO_HC_REG_DLL_CONFIG_DLL_RST_BMSK;
      reg_value |= HWIO_HC_REG_DLL_CONFIG_PDN_BMSK;
   }
   else
   {
      reg_value &= ~HWIO_HC_REG_DLL_CONFIG_DLL_RST_BMSK;
      reg_value &= ~HWIO_HC_REG_DLL_CONFIG_PDN_BMSK;
   }

   HWIO_SDC_REG_OUT (HC_REG_DLL_CONFIG, (UINTN)sdcc_reg_hc[nControllerId], reg_value);
}

inline void
HAL_sdhci_DllEnable (const UINT32 nControllerId)
{
   /* Set DLL_EN bit to 1. */
   UINT32 reg_value = 0;
   reg_value = HWIO_SDC_REG_IN(HC_REG_DLL_CONFIG, (UINTN)sdcc_reg_hc[nControllerId]);
   reg_value |= HWIO_HC_REG_DLL_CONFIG_DLL_EN_BMSK;
   HWIO_SDC_REG_OUT (HC_REG_DLL_CONFIG, (UINTN)sdcc_reg_hc[nControllerId], reg_value);
}

inline void
HAL_sdhci_DllCkOutEnable (const UINT32 nControllerId)
{
   UINT32 reg_value = 0;
   reg_value = HWIO_SDC_REG_IN(HC_REG_DLL_CONFIG, (UINTN)sdcc_reg_hc[nControllerId]);
   reg_value |= HWIO_HC_REG_DLL_CONFIG_CK_OUT_EN_BMSK;
   HWIO_SDC_REG_OUT (HC_REG_DLL_CONFIG, (UINTN)sdcc_reg_hc[nControllerId], reg_value);
}

inline void
HAL_sdhci_CdrEnable (const UINT32 nControllerId, BOOLEAN bEnable)
{
   UINT32 reg_value = 0;
   reg_value = HWIO_SDC_REG_IN(HC_REG_DLL_CONFIG, (UINTN)sdcc_reg_hc[nControllerId]);

   if (bEnable)
   {
     reg_value |= HWIO_HC_REG_DLL_CONFIG_CDR_EN_BMSK;
   }
   else
   {
     reg_value &= ~HWIO_HC_REG_DLL_CONFIG_CDR_EN_BMSK;
   }

   HWIO_SDC_REG_OUT (HC_REG_DLL_CONFIG, (UINTN)sdcc_reg_hc[nControllerId], reg_value);
}

#define HAL_SDCC_SUCCESS   0
#define HAL_SDCC_ERROR    -1

inline int HAL_sdhci_DllPollCkOutEn(const UINT32 nControllerId, BOOLEAN bEnable)   
{
   UINT32 wait_cnt = 50;
   UINT32 ck_out_en;

   do
   {
      ck_out_en = HWIO_SDC_REG_IN(HC_REG_DLL_CONFIG, (UINTN)sdcc_reg_hc[nControllerId]);
      if(bEnable)
      {
         if(ck_out_en & HC_DLL_CONFIG_CK_OUT_EN_SET)
            break;
      }
      else
      {
         if(!(ck_out_en & HC_DLL_CONFIG_CK_OUT_EN_SET))
            break;
      }

      HAL_sdcc_Delay(1);
   }while (--wait_cnt);

   if(wait_cnt == 0)
   {
      //time out ck_out_en, return -1 indicating error
      return HAL_SDCC_ERROR;
   }

   return HAL_SDCC_SUCCESS;
}

inline int HAL_sdhci_DllPollStatusLock(const UINT32 nControllerId)
{
   UINT32 wait_cnt = 50;
   UINT32 val;

   /* Wait until DLL_LOCK bit of HC_REG_DLL_STATUS register becomes '1' */
   /* max. wait for 50us sec for LOCK bit to be set */

   do
   {
      val = HWIO_SDC_REG_IN(HC_REG_DLL_STATUS, (UINTN)sdcc_reg_hc[nControllerId]);
      if(val & HC_DLL_STATUS_DLL_LOCK)
      {
         //	DLL lock success
         break;
      }

      /* wait for 1us before polling again */
      HAL_sdcc_Delay(1);
   } while (--wait_cnt);

   if(wait_cnt == 0)
   {
       //time out DLL failed to lock, return -1 indicating error
       return HAL_SDCC_ERROR;
   }

   return HAL_SDCC_SUCCESS;
}

int HAL_sdhci_ConfigCmDllPhase(const UINT32 nControllerId, INT8 phase)
{
   const UINT8 grey_coded_phase_table[] = {0x0, 0x1, 0x3, 0x2, 0x6, 0x7, 0x5, 0x4,
      0xC, 0xD, 0xF, 0xE, 0xA, 0xB, 0x9, 0x8};
   UINT32 config;
   int rc = HAL_SDCC_SUCCESS;

   config = HWIO_SDC_REG_IN(HC_REG_DLL_CONFIG, (UINTN)sdcc_reg_hc[nControllerId]);
   config &= ~(HWIO_HC_REG_DLL_CONFIG_CDR_EN_BMSK | HWIO_HC_REG_DLL_CONFIG_CK_OUT_EN_BMSK);
   config |= (HWIO_HC_REG_DLL_CONFIG_CDR_EXT_EN_BMSK | HWIO_HC_REG_DLL_CONFIG_DLL_EN_BMSK);
   HWIO_SDC_REG_OUT (HC_REG_DLL_CONFIG, (UINTN)sdcc_reg_hc[nControllerId], config);

   /* Wait until CK_OUT_EN bit of HC_REG_DLL_CONFIG register becomes '0' */
   rc = HAL_sdhci_DllPollCkOutEn(nControllerId, FALSE);
   if (rc != HAL_SDCC_SUCCESS)
   {
      goto Exit;
   }

   /*
    * Write the selected DLL clock output phase (0 ... 15)
    * to CDR_SELEXT bit field of HC_REG_DLL_CONFIG register.
    */
   config = HWIO_SDC_REG_IN(HC_REG_DLL_CONFIG, (UINTN)sdcc_reg_hc[nControllerId]);
   config &= ~HWIO_HC_REG_DLL_CONFIG_CDR_SELEXT_BMSK;
   config |= ((grey_coded_phase_table[phase]) << HWIO_HC_REG_DLL_CONFIG_CDR_SELEXT_SHFT);
   HWIO_SDC_REG_OUT (HC_REG_DLL_CONFIG, (UINTN)sdcc_reg_hc[nControllerId], config);

   /* Set CK_OUT_EN bit of HC_REG_DLL_CONFIG register to 1. */
   HAL_sdhci_DllCkOutEnable(nControllerId);
   
   /* Wait until CK_OUT_EN bit of HC_REG_DLL_CONFIG register becomes '1' */
   rc = HAL_sdhci_DllPollCkOutEn(nControllerId, TRUE);
   if (rc != HAL_SDCC_SUCCESS)
   {
      goto Exit;
   }

   config = HWIO_SDC_REG_IN(HC_REG_DLL_CONFIG, (UINTN)sdcc_reg_hc[nControllerId]);
   config |= HWIO_HC_REG_DLL_CONFIG_CDR_EN_BMSK;
   config &= ~HWIO_HC_REG_DLL_CONFIG_CDR_EXT_EN_BMSK;
   HWIO_SDC_REG_OUT (HC_REG_DLL_CONFIG, (UINTN)sdcc_reg_hc[nControllerId], config);

Exit:
   return rc;
}

/**
 * Reg Offset: 0x0E. 16-bit register
 * Controls the SD/eMMC command generation.
 * Software should check the CMD and DAT Inhibit bits in 
 * the Present State register before issuing the command 
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 * 
 * @param hal_cmd
 *   Contains the command index as well as other command attributes
 */
inline void 
HAL_sdhci_SetCommand (const UINT32 nControllerId, 
                        const struct HAL_sdhci_Cmd *hal_cmd)
{
  UINT16 command = 0;
  command = ((UINT16)(hal_cmd->cmd_idx) << HWIO_HC_REG_CMD_INDX_SHIFT) |
              (hal_cmd->cmd_type << HWIO_HC_REG_CMD_TYPE_SHIFT) |
              (hal_cmd->data_present << HWIO_HC_REG_CMD_DATA_PRESENT_SEL_SHIFT) |
              (hal_cmd->idx_check << HWIO_HC_REG_CMD_INDX_CHECK_SHIFT) |
              (hal_cmd->crc_check << HWIO_HC_REG_CMD_CRC_CHECK_SHIFT) |
              (hal_cmd->resp_type);

  HWIO_SDC_WORD_OUT (HC_REG_E, (UINTN)sdcc_reg_hc[nControllerId], command);
}

/**
 * Reg Offset: 0x00. 32-bit register 
 * Used with Auto CMD23 feature. it sets the 32-bit block count value to the
 * argument of the CMD23 while executing Auto CMD23
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 * 
 * @param nArg2
 *   32-bit value to be set to the Argument 2 register
 */
inline void
HAL_sdhci_SetArg2 (const UINT32 nControllerId, UINT32 nArg2)
{
  HWIO_SDC_REG_OUT (HC_REG_0_2, (UINTN)sdcc_reg_hc[nControllerId], nArg2);
}

/**
 * Reg Offset: 0x04. 16-bit register
 * Configures number of bytes in a data block
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 * 
 * @param nBlockSize
 *   Indicates the transfer block size
 */
inline void
HAL_sdhci_SetBlockSize (const UINT32 nControllerId, UINT16 nBlockSize)
{
  HWIO_SDC_WORD_OUT (HC_REG_4, (UINTN)sdcc_reg_hc[nControllerId], nBlockSize);
}

/**
 * Reg Offset: 0x06. 16-bit register
 * Configures the number of blocks in the transfer
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 * 
 * @param nBlockCount
 *   Indicates the number of blocks in the transfer
 */
inline void
HAL_sdhci_SetBlockCount (const UINT32 nControllerId, UINT16 nBlockCount)
{
  HWIO_SDC_WORD_OUT (HC_REG_6, (UINTN)sdcc_reg_hc[nControllerId], nBlockCount);
}

/**
 * Reg Offset: 0x08. 32-bit register
 * Sets the SD Argument
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 * 
 * @param nArg
 *   Arguments for SD/eMMC commands
 */
inline void
HAL_sdhci_SetCmdArg (const UINT32 nControllerId, UINT32 nArg)
{
  HWIO_SDC_REG_OUT (HC_REG_8_A, (UINTN)sdcc_reg_hc[nControllerId], nArg);
}

/**
 * Reg Offset: 0x0C. 16-bit register
 * Controls the operation of data transfer. The Host Driver must set the 
 * Transfer Mode Register before issuing a command which transfers data. 
 * Writes to this reigster are ignored when the DAT Command Inhibit bit 
 * in the Present State register is set.
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 * 
 * @param arg
 *   Arguments for SD/eMMC commands
 */
inline void
HAL_sdhci_SetTransferMode (const UINT32 nControllerId, 
                           const struct HAL_sdhci_TransMode *hal_mode)
{
  UINT16 reg_value = 0;

  reg_value = (hal_mode->multiblock) << HWIO_HC_REG_TRANS_MODE_MULTI_SINGLE_BLK_SEL_SHIFT |
              (hal_mode->direction) << HWIO_HC_REG_TRANS_MODE_DATA_DIRECTION_SEL_SHIFT |
              (hal_mode->autocmd_en) << HWIO_HC_REG_TRANS_MODE_AUTO_CMD_EN_SHIFT |
              (hal_mode->blk_count_en) << HWIO_HC_REG_TRANS_MODE_BLK_CNT_EN_SHIFT |
              (hal_mode->dma_en);

  HWIO_SDC_WORD_OUT (HC_REG_C, (UINTN)sdcc_reg_hc[nControllerId], 
                    (UINT8)((reg_value &0x00FF)));
}

/**
 * Reg Offset: 0x10-0x1F. 48-bit register
 * Reads the response from the memory card
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 * 
 * @param resp
 *   [output] The response from the card is stored in this buffer
 *
 * @param_resp bLongResp
 *   Indicates to read a short or long response
 */
inline void 
HAL_sdhci_GetResponse (const UINT32 nControllerId, UINT32 *resp, 
                       BOOLEAN bLongResp)
{
  /* R1,R1b,R3-R6 normal command responses */
  resp[0] = HWIO_SDC_REG_IN (HC_REG_10_12, (UINTN)sdcc_reg_hc[nControllerId]);

  if (TRUE == bLongResp)
  {
    UINT32 uR2Resp[4];
    resp[1] = HWIO_SDC_REG_IN (HC_REG_14_16, (UINTN)sdcc_reg_hc[nControllerId]);
    resp[2] = HWIO_SDC_REG_IN (HC_REG_18_1A, (UINTN)sdcc_reg_hc[nControllerId]);
    resp[3] = HWIO_SDC_REG_IN (HC_REG_1C_1E, (UINTN)sdcc_reg_hc[nControllerId]);

    /* the response bits in SDHC mode are offset when compared to legacy SDCC. 
     * To keep the rest of the driver compatible, adjust the responses to match 
     * legacy response. */
    uR2Resp[0] = ((resp[3] << 8) & 0xFFFFFF00) | ((resp[2] >> 24) & 0x000000FF);
    uR2Resp[1] = ((resp[2] << 8) & 0xFFFFFF00) | ((resp[1] >> 24) & 0x000000FF);
    uR2Resp[2] = ((resp[1] << 8) & 0xFFFFFF00) | ((resp[0] >> 24) & 0x000000FF);
    uR2Resp[3] = ((resp[0] << 8) & 0xFFFFFF00);
    resp[0] = uR2Resp[0];
    resp[1] = uR2Resp[1];
    resp[2] = uR2Resp[2];
    resp[3] = uR2Resp[3];
  }
}

/**
 * Reg Offset: 0x20. 32-bit register
 * Reads the data from the internal buffer. Used to read the data from
 * the memory device in FIFO mode
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 */
inline UINT32
HAL_sdhci_ReadDataBufPort (const UINT32 nControllerId)
{
  return HWIO_SDC_REG_IN (HC_REG_20_22, (UINTN)sdcc_reg_hc[nControllerId]);
}

/**
 * Reg Offset: 0x20. 32-bit register
 * Writes the data to the internal buffer. Used to write the data to 
 * the memory device in FIFO mode
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 *
 * @param nVal
 *   32-bit value to be written to the internal buffer which will 
 *   be sent to the memory device
 */
inline void
HAL_sdhci_WriteDataBufPort (const UINT32 nControllerId, UINT32 nVal)
{
  HWIO_SDC_REG_OUT (HC_REG_20_22, (UINTN)sdcc_reg_hc[nControllerId], nVal);
}

/**
 * Reg Offset: 0x24. 32-bit register
 * Reads the Present State register which gives the status of the host controller
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 *
 * @return
 *   32-bit Present State register
 */
inline UINT32
HAL_sdhci_GetPresentState (const UINT32 nControllerId)
{
  return HWIO_SDC_REG_IN (HC_REG_24_26, (UINTN)sdcc_reg_hc[nControllerId]);
}

/**
 * Reg Offset: 0x28. 8-bit register
 * Sets the Bus Width to 1, 4 or 8-bit bus width
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 *
 * @param BusWidth
 */
inline void
HAL_sdhci_SetBusWidth (const UINT32 nControllerId, 
                       enum HAL_sdcc_BusWidth BusWidth)
{
  UINT8 reg_value = HWIO_SDC_BYTE_IN (HC_REG_28, (UINTN)sdcc_reg_hc[nControllerId]);
  switch (BusWidth)
  {
  case HAL_SDCC_BUS_WIDTH_1_BIT:
    HWIO_SDC_BYTE_OUT (HC_REG_28, (UINTN)sdcc_reg_hc[nControllerId],
       (reg_value & ~(HWIO_HC_REG_HST_CTL1_DATA_TRANS_WIDTH_BMSK |
        HWIO_HC_REG_HST_CTL1_EXTENDED_DATA_TRANS_WIDTH_BMSK)) | 
        HC_REG_HST_CTL1_DATA_TRANS_WIDTH_1_BIT_BMSK);
    break;

  case HAL_SDCC_BUS_WIDTH_4_BIT:
    HWIO_SDC_BYTE_OUT (HC_REG_28, (UINTN)sdcc_reg_hc[nControllerId],
        (reg_value & ~(HWIO_HC_REG_HST_CTL1_DATA_TRANS_WIDTH_BMSK |
         HWIO_HC_REG_HST_CTL1_EXTENDED_DATA_TRANS_WIDTH_BMSK)) |
         HC_REG_HST_CTL1_DATA_TRANS_WIDTH_4_BIT_BMSK);
    break;

  case HAL_SDCC_BUS_WIDTH_8_BIT:   	
    HWIO_SDC_BYTE_OUT (HC_REG_28, (UINTN)sdcc_reg_hc[nControllerId],
        (reg_value & ~HWIO_HC_REG_HST_CTL1_EXTENDED_DATA_TRANS_WIDTH_BMSK) |
         HC_REG_HST_CTL1_DATA_TRANS_WIDTH_8_BIT_BMSK);
    break;

  default:
    break;
  }
}

/**
 * Reg Offset: 0x28. 8-bit register
 * Enables 32-bit ADMA2 mode
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 *
 * @param bEnable
 *   Set to TRUE to enable AMDA2 mode. FALSE to enable SDMA mode
 */
inline void
HAL_sdhci_ADMAEnable (const UINT32 nControllerId, BOOLEAN bEnable)
{
  UINT8 reg_value = HWIO_SDC_BYTE_IN (HC_REG_28, (UINTN)sdcc_reg_hc[nControllerId]);
  UINT8 adma_sel_bit = 0;

  if (sizeof (UINTN) == 8)
  {
    /* 64-bit mode */
    adma_sel_bit = bEnable ? HC_REG_HST_CTL1_64_BIT_ADMA2_ENABLE_BMSK : 0;
  }
  else
  {
    /* 32-bit mode */
    adma_sel_bit = bEnable ? HC_REG_HST_CTL1_32_BIT_ADMA2_ENABLE_BMSK : 0;
  }

  HWIO_SDC_BYTE_OUT (HC_REG_28, (UINTN)sdcc_reg_hc[nControllerId],
                    ((reg_value & ~HWIO_HC_REG_HST_CTL1_DMA_SEL_BMSK) | adma_sel_bit));                  
}

/**
 * Handle the power interrupts according to the SWI.
 * See the MCI PWRCTR Status/Clear/Control registers in the SWI for details.
 * When there's a bus power event (bus power on/off), the corresponding
 * PWRCTL status bit is set. Software needs to clear the status bits and sets 
 * the corresponding "success" bit in the PWRCLT_CLT register to indiate 
 * the power events have been handled successfully.
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 */
inline void
HAL_sdhci_HandlePowerInterrupts (UINT32 nControllerId)
{
  UINT32 reg_value = 0;
  UINT32 count = HAL_SDHCI_POLL_MAX;
  
  do
  {
    reg_value = HWIO_SDC_REG_IN (HC_VENDOR_SPECIFIC_PWRCTL_STATUS, 
                                 (UINTN)sdcc_reg_hc[nControllerId]);
    if (0 != reg_value)
    {
       break;
    }

    HAL_sdcc_Delay(1);
	
  }while (count--);

  /* Clear power interrupt and indicate successfull power switch */
  HWIO_SDC_REG_OUT (HC_VENDOR_SPECIFIC_PWRCTL_CLEAR, (UINTN)sdcc_reg_hc[nControllerId], reg_value);

  /* wait for interrupt to be cleared */
  count = HAL_SDHCI_POLL_MAX;
  do
  {
    if (0 == HWIO_SDC_REG_IN (HC_VENDOR_SPECIFIC_PWRCTL_STATUS, 
                              (UINTN)sdcc_reg_hc[nControllerId]))
    {
      break;
    }

    HAL_sdcc_Delay(1);	

  }while (count--);
  
  if (reg_value & (HWIO_HC_VENDOR_SPECIFIC_PWRCTL_CLEAR_BUS_OFF_BMSK | 
                   HWIO_HC_VENDOR_SPECIFIC_PWRCTL_CLEAR_BUS_ON_BMSK))
  {
    HWIO_SDC_REG_OUT (HC_VENDOR_SPECIFIC_PWRCTL_CTL, (UINTN)sdcc_reg_hc[nControllerId], 
                      HWIO_HC_VENDOR_SPECIFIC_PWRCTL_CTL_BUS_ON_OFF_SUCCESS_BMSK);
  }
  else
  {
    HWIO_SDC_REG_OUT (HC_VENDOR_SPECIFIC_PWRCTL_CTL, (UINTN)sdcc_reg_hc[nControllerId], 
                      HWIO_HC_VENDOR_SPECIFIC_PWRCTL_CTL_IO_SIG_SWITCH_SUCCESS_BMSK);
  }

  return;
}

/**
 * Reg Offset: 0x29. 8-bit register
 * Selects the bus voltage level for the memory device. 
 * The supported voltage levels info can be retrieved from the card's 
 * capabilities register 
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 *
 * @param cVoltage
 *   bus voltage level to be used
 */
inline void 
HAL_sdhci_VoltageSelect (UINT32 nControllerId, 
                         HAL_sdhci_BusVolSelect cVoltage)
{
  UINT8 val = HWIO_SDC_BYTE_IN (HC_REG_29, (UINTN)sdcc_reg_hc[nControllerId]);
  /* Clear the voltage field and set the new voltage level */
  val = (val & ~HWIO_HC_REG_PWR_CTL_SD_BUS_VOLTAGE_SEL_BMSK) | cVoltage;
  HWIO_SDC_BYTE_OUT (HC_REG_29, (UINTN)sdcc_reg_hc[nControllerId], val);
}

/**
 * Reg Offset: 0x29. 8-bit register
 * Turns the bus power on or off
 * Software should set the voltage level before turn on the power
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 *
 * @param bEnable
 *   TRUE to turn on the bus power. FALSE to turn it off
 */
inline void
HAL_sdhci_SetPower (UINT32 nControllerId, BOOLEAN bEnable)
{
  UINT8 val = HWIO_SDC_BYTE_IN (HC_REG_29, (UINTN)sdcc_reg_hc[nControllerId]);
  /* Clear the Bus Power bit mask and set the new value */
  val = (val & ~HWIO_HC_REG_PWR_CTL_SD_BUS_PWR_BMSK) | (UINT8)bEnable;
  HWIO_SDC_BYTE_OUT (HC_REG_29, (UINTN)sdcc_reg_hc[nControllerId], val);
}

/**
 * Reg Offset: 0x29. 8-bit register
 * Reads the current bus power state.
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 *
 * @return
 *   8-bit value of the Power Control Register (offset 0x29)
 */
inline UINT8 
HAL_sdhci_GetPowerState (UINT32 nControllerId)
{
  UINT8 val = HWIO_SDC_BYTE_IN (HC_REG_29, (UINTN)sdcc_reg_hc[nControllerId]);
  /* HC_REG_29. Bit0 = BusPower. Bit[1:3] = Voltage Select */
  val = val & (HWIO_HC_REG_PWR_CTL_SD_BUS_PWR_BMSK | 
               HWIO_HC_REG_PWR_CTL_SD_BUS_VOLTAGE_SEL_BMSK);
  return val;
}

/**
 * Reg Offset: 0x2C. 16-bit register
 * Set the SD Clock frequency using 10-bit divider
 * The base frequency and the divider are used to set the SD clock.
 * The base frequency is retrieved from the Capabilities register
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 *
 * @nDivider
 *   10-bit divider of the Power Control Register (offset 0x29)
 */
inline void
HAL_sdhci_SetClock_10Bits_Mode (UINT32 nControllerId, UINT32 nDivider)
{
  UINT16 val = 0;
  /* The LSB bits of divider are Bits[8:15] */
  val = (nDivider & 0xFF) << HWIO_HC_REG_CLK_CTL_SDCLK_FREQ_SEL_SHFT;
  /* The MSB bits of the divider are Bits[6:7] */
  val |= ((nDivider & 0x300) >> 8) << HWIO_HC_REG_CLK_CTL_SDCLK_FREQ_SEL_MSB_SHFT;
  /* Enable internal clk */
  val |= HWIO_HC_REG_CLK_CTL_INTERNAL_CLK_EN_BMSK;
  HWIO_SDC_WORD_OUT (HC_REG_2C, (UINTN)sdcc_reg_hc[nControllerId], val);
}

/**
 * Reg Offset: 0x2C. 16-bit register
 * Enables or stops the SD Clock. Software must stop the SD Clock before
 * making any change to the Clock Frequency Select
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 *
 * @param bEnable
 *   TRUE to enable or FALSE to disable SD Clock
 */
inline void
HAL_sdhci_EnableSDClk (UINT32 nControllerId, BOOLEAN bEnable)
{
  UINT8 val = HWIO_SDC_BYTE_IN (HC_REG_2C, (UINTN)sdcc_reg_hc[nControllerId]);
  val = (val & ~HWIO_HC_REG_CLK_CTL_SDCLK_EN_BMSK) | 
  	((UINT8)bEnable << HWIO_HC_REG_CLK_CTL_SDCLK_EN_SHFT);
  HWIO_SDC_BYTE_OUT (HC_REG_2C, (UINTN)sdcc_reg_hc[nControllerId], val);
}

/**
 * Reg Offset: 0x2C. 16-bit register
 * Checks if the internal clock is stable. Software must check the internal 
 * clock stable before enabling the SD Clock
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 *
 * @return
 *   TRUE indicates the internal clock is stable. FALSE otherwise
 */
inline BOOLEAN
HAL_sdhci_IsClkStable (UINT32 nControllerId)
{
  UINT8 val = HWIO_SDC_BYTE_IN (HC_REG_2C, (UINTN)sdcc_reg_hc[nControllerId]);
  return (0 != (val & HWIO_HC_REG_CLK_CTL_INTERNAL_CLK_STABLE_BMSK));
}

/**
 * Reg Offset: 0x2E. 8-bit register
 * Sets the timeout counter for DAT line timeout. Timeout clock frequency 
 * will be generated by dividing the base clock TMCLK value by this value
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 *
 * @param nTimeoutCounterValue
 *   8-bit value for the DAT line timeout counter
 */
inline void
HAL_sdhci_SetTimeout (const UINT32 nControllerId, UINT8 TimeoutCounterValue)
{
  HWIO_SDC_BYTE_OUT (HC_REG_2E, (UINTN)sdcc_reg_hc[nControllerId], TimeoutCounterValue);
}

/**
 * Reg Offset: 0x30-0x32. This API accesses to two 16-bit registers.
 * Read the Normal Interrupt Status Register and 
 * the Error Interrupt Status Register
 * These status bits are enabled by the Interrupt Status Enable Registers.
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 *
 * @returns
 * 32-bit value: 
 * Bits[0-15]: Reg 0x30. Normal Interrupt Status Register
 * Bits[16-31]: Reg 0x32. Error Interrupt Status Register
 */
inline UINT32 
HAL_sdhci_GetInterruptStatus(const UINT32 nControllerId)
{
  return HWIO_SDC_REG_IN (HC_REG_30_32, (UINTN)sdcc_reg_hc[nControllerId]);
}

/**
 * Reg Offset: 0x30-0x32. This API accesses to two 16-bit registers.
 * Clears the Normal Interrupt Status Register and 
 * the Error Interrupt Status Register
 * Writing a 1 clears the bit. Writing 0 keeps the bit unchanged.
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 * 
 * @param nMask
 *   Bit-wise mask of the bits to be cleared
 */
inline void
HAL_sdhci_ResetInterruptStatus(const UINT32 nControllerId, UINT32 nMask)
{
  UINT32 count = HAL_SDHCI_POLL_MAX;
  UINT32 reg_value = 0;
  
  nMask &= HAL_SDHCI_INTERRUPT_MSK_ALL;
  do
  {
    HWIO_SDC_REG_OUT (HC_REG_30_32, (UINTN)sdcc_reg_hc[nControllerId], nMask);

    reg_value = HAL_sdhci_GetInterruptStatus(nControllerId);
    if (0 == (nMask & reg_value))
    {
      break;
    }

    HAL_sdcc_Delay(1);	
  }
  while (count--);
}

/**
 * Reg Offset: 0x34-0x36. This API accesses to two 16-bit registers.
 * Enables or Disables the status of the events described in 
 * the Interrupt Status Registers. 
 * Writing a 1 to the Interrupt Status Enable bits indicates that when 
 * the events occur the corresponding interrupt status bit is set 
 * (Note that the interrupt is not triggered unless 
 * the corresponding Interrupt Signal Enable bit is set.)
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 * 
 * @param mask
 *   Bit-wise mask of the status to be enabled/disabled
 * 
 * @param bEnable
 *   Indicates the operation to be carried out.
 *   If set to 1, enable the status bits that are indicated in the nMask param
 *   If clear to 0, disable the status bits that are indicated in the nMask param.
 */
inline void 
HAL_sdhci_InterruptStatusEnable(const UINT32 nControllerId, 
                                UINT32 nMask, BOOLEAN bEnable)
{
  UINT32 reg_value = HWIO_SDC_REG_IN (HC_REG_34_36, (UINTN)sdcc_reg_hc[nControllerId]);
  UINT32 count = HAL_SDHCI_POLL_MAX;
  
  if (FALSE == bEnable)
  {
    HWIO_SDC_REG_OUT (HC_REG_34_36, (UINTN)sdcc_reg_hc[nControllerId],
                     (reg_value & (~nMask)));
    return;
  }  

  do
  {
    HWIO_SDC_REG_OUT (HC_REG_34_36, (UINTN)sdcc_reg_hc[nControllerId], 
                     ((reg_value & (~nMask)) | nMask));

    reg_value = HWIO_SDC_REG_IN (HC_REG_34_36, (UINTN)sdcc_reg_hc[nControllerId]) & nMask;

    if (reg_value == nMask)
    {
      break;
    }

    HAL_sdcc_Delay(1);
  } while (count--);
}

/**
 * Reg Offset: 0x38 - 0x3A. This API accesses to two 16-bit registers.
 * Enables/Disables the Normal Interrupt Signal (0x38) and the Error 
 * Interrupt Signal (0x3A). Writing a 1 to the bits in the Interrupt Signal Enable
 * indicates that if the corresponding Interrupt Status Enable bit is set, 
 * the interrupt would be triggered to the system
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 * 
 * @param nMask
 *   Bit-wise mask of the interrupt signal to be enabled or disabled
 * 
 * @param bEnable
 *   Indicates the operation to be carried out.
 *   If set to 1, enable the bit-wise nMask param as the interrupt triggered events
 *   If clear to 0, disable the bit-wise nMask param as the interrupt triggered events
 */
inline void 
HAL_sdhci_InterruptSignalEnable(const UINT32 nControllerId, 
                                UINT32 nMask, BOOLEAN bEnable)
{
  UINT32 reg_value = HWIO_SDC_REG_IN (HC_REG_38_3A, (UINTN)sdcc_reg_hc[nControllerId]);
  
  if (bEnable)
  {
    HWIO_SDC_REG_OUT (HC_REG_38_3A, (UINTN)sdcc_reg_hc[nControllerId],
                     (reg_value & (~nMask)) | nMask);
  }
  else
  {
    HWIO_SDC_REG_OUT (HC_REG_38_3A, (UINTN)sdcc_reg_hc[nControllerId], 
                     (reg_value & (~nMask)));
  }
}

/**
 * Reg Offset: 0x3E. 16-bit register
 * Change UHS Mode 
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 * @param mode
 *   UHS mode selected
 */
inline void
HAL_sdhci_SetUHSMode(UINT32 nControllerId, UINT16 mode)
{
  UINT16 val = HWIO_SDC_WORD_IN (HC_REG_3E, (UINTN)sdcc_reg_hc[nControllerId]);

  val = ((val & ~(UINT16)HWIO_HC_REG_HST_CTL2_UHS_MODE_SEL_BMSK) | mode); 

  HWIO_SDC_WORD_OUT (HC_REG_3E, (UINTN)sdcc_reg_hc[nControllerId], val);
}

/**
 * Reg Offset: 0x40. 64-bit register
 * Retrieves the Capabilities. The capabilities provides information 
 * specific to host controller implemenations
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 *
 * @param Capabilities
 *   Host controller capabilities
 */
inline void 
HAL_sdhci_GetCapabilities(const UINT32 nControllerId, 
                         struct HAL_sdhci_Capabilities *Capabilities)
{
  Capabilities->HAL_sdhci_Reg_40_42 = 
               HWIO_SDC_REG_IN (HC_REG_40_42, (UINTN)sdcc_reg_hc[nControllerId]);
  Capabilities->HAL_sdhci_Reg_44_46 = 
               HWIO_SDC_REG_IN (HC_REG_44_46, (UINTN)sdcc_reg_hc[nControllerId]);
}

/**
 * Reg Offset: 0x58. 32-bit register
 * Sets the descriptor table address for the 32-bit Address ADMA
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 *
 * @param nAddress
 *   Specifies the address of the descriptor table for 32-bit ADMA
 *
 */
inline void 
HAL_sdhci_SetADMASystemAddress(const UINT32 nControllerId, UINT32 nAddress)
{
  HWIO_SDC_REG_OUT (HC_REG_58_5A, (UINTN)sdcc_reg_hc[nControllerId], nAddress);
}

/**
 * Reg Offset: 0x5C. 32-bit register
 * Sets the descriptor table address for the 64-bit Address ADMA
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 *
 * @param nAddress
 *   Specifies the address of the descriptor table for 64-bit ADMA 
 *
 */
inline void
HAL_sdhci_SetADMASystemAddress64bit(const UINT32 nControllerId, UINT32 nAddress)
{
  HWIO_SDC_REG_OUT (HC_REG_5C_5E, (UINTN)sdcc_reg_hc[nControllerId], nAddress);
}

/**
 * Register SDCC_HC_REG_DLL_CONFIG
 * Disables the DLL clock
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 */
inline void
HAL_sdhci_DisableDLLClock (const UINT32 nControllerId)
{
  UINT32 val = HWIO_SDC_REG_IN (HC_REG_DLL_CONFIG, (UINTN)sdcc_reg_hc[nControllerId]);

  HWIO_SDC_REG_OUT (HC_REG_DLL_CONFIG, (UINTN)sdcc_reg_hc[nControllerId],
                   (val | HWIO_HC_REG_DLL_CONFIG_PDN_BMSK));

  HWIO_SDC_REG_OUT (HC_REG_DLL_CONFIG, (UINTN)sdcc_reg_hc[nControllerId],
                   (val | HWIO_HC_REG_DLL_CONFIG_DLL_RST_BMSK | HWIO_HC_REG_DLL_CONFIG_PDN_BMSK));
}

/**
 * Reg Offset: 0x11C and 0x120.
 * Set the SDHCi Capabitilities. 
 * Each SDCC controller supports a specific set of features.
 * Setting the bits in the HC_VENDOR_SPECIFIC_CAPABILITIES0 and 
 * HC_VENDOR_SPECIFIC_CAPABILITIES1 to indicate the features that are supported
 * in this SDCC controller.
 * Note that the setting of the obove mentioned registers will be reflected in the read-only bits of
 * the Capbilities registers (register offset 0x40-0x46)
 * 
 */
void HAL_sdhci_SetCapabilities (const UINT32 nControllerId)
{
  UINT32 caps0 = 0, caps1 = 0;;
  UINT32 eMMCSlotNum = PcdGet32(SDCCEmmcSlotNumber);
  UINT32 ExternalSDSlotNum = PcdGet32(SDCCExtSlotNumber);
  UINT32 reg_value = 0;

  caps0 = HWIO_SDC_REG_IN (HC_REG_40_42, (UINTN)sdcc_reg_hc[nControllerId]);
  caps1 = HWIO_SDC_REG_IN (HC_REG_44_46, (UINTN)sdcc_reg_hc[nControllerId]);  

  if ((nControllerId == eMMCSlotNum) ||
      (nControllerId == ExternalSDSlotNum))
  {
    /* In addition to the default setting of the Capabilities, */
    /* make sure the following features are supported */

    /* Support ADMA2 */
    caps0 &= ~HAL_SDHCI_CAPS_ADMA2_SUPPORT_BMSK;
    caps0 |= HAL_SDHCI_CAPS_ADMA2_SUPPORT_BMSK;

    /* Support High Speed */
    caps0 &= ~HAL_SDHCI_CAPS_HS_SUPPORT_BMSK;
    caps0 |= HAL_SDHCI_CAPS_HS_SUPPORT_BMSK;

    /* Support SDR50 */
    caps1 &= ~HAL_SDHCI_CAPS_SDR_50_SUPPORT_BMSK;
    caps1 |= HAL_SDHCI_CAPS_SDR_50_SUPPORT_BMSK;

    /* Support SDR104 */
    caps1 &= ~HAL_SDHCI_CAPS_SDR_104_SUPPORT_BMSK;
    caps1 |= HAL_SDHCI_CAPS_SDR_104_SUPPORT_BMSK;

    /* Support DDR50 */
    caps1 &= ~HAL_SDHCI_CAPS_DDR_50_SUPPORT_BMSK;
    caps1 |= HAL_SDHCI_CAPS_DDR_50_SUPPORT_BMSK;
  }
  
  if (nControllerId == eMMCSlotNum)
  {
    /* Support 8-bit bus */
    caps0 &= ~HAL_SDHCI_CAPS_SUPPORT_8_BIT_BMSK;
    caps0 |= HAL_SDHCI_CAPS_SUPPORT_8_BIT_BMSK;

    /* Support 1.8V for eMMC only. No support for 3.0V and 3.3V */
    caps0 &= ~HAL_SDHCI_CAPS_VOLTAGE_SUPPORT_3_0V_BMSK;
    caps0 &= ~HAL_SDHCI_CAPS_VOLTAGE_SUPPORT_3_3V_BMSK;

    caps0 &= ~HAL_SDHCI_CAPS_VOLTAGE_SUPPORT_1_8V_BMSK;
    caps0 |= HAL_SDHCI_CAPS_VOLTAGE_SUPPORT_1_8V_BMSK;

    /* Embedded slot */
    caps0 &= ~HAL_SDHCI_CAPS_SLOT_TYPE_BMSK;
    caps0 |= HAL_SDHCI_CAPS_SLOT_TYPE_EMBEDDED_SLOT;
    
    /* 64-bit system bus support */
    caps0 &= ~HAL_SDHCI_CAPS_SYS_BUS_SUPPORT_64_BIT_BMSK;
    caps0 |= HAL_SDHCI_CAPS_SYS_BUS_SUPPORT_64_BIT_BMSK;
     
    /* Update to max supported clock */
    caps0 &= ~HAL_SDHCI_CAPS_BASE_SDCLK_FREQ_BMSK;
    caps0 |= HAL_SDHCI_CAPS_BASE_SDCLK_FREQ_200MHZ;    
  }
  else if (nControllerId == ExternalSDSlotNum)
  {
    /* Support 1.8V and 3.0V for external SD card only. */
    caps0 &= ~HAL_SDHCI_CAPS_VOLTAGE_SUPPORT_3_0V_BMSK;
    caps0 |= HAL_SDHCI_CAPS_VOLTAGE_SUPPORT_3_0V_BMSK;

    caps0 &= ~HAL_SDHCI_CAPS_VOLTAGE_SUPPORT_1_8V_BMSK;
    caps0 |= HAL_SDHCI_CAPS_VOLTAGE_SUPPORT_1_8V_BMSK;

    /* External SD slot */
    caps0 &= ~HAL_SDHCI_CAPS_SLOT_TYPE_BMSK;
    caps0 |= HAL_SDHCI_CAPS_SLOT_TYPE_SD_SLOT;
  }
  
  HWIO_SDC_REG_OUT (HC_VENDOR_CAPS0, (UINTN)sdcc_reg_hc[nControllerId], caps0);

  /* Only update Caps1 Bits[0:23]. Retain Bits[24:31] which is SPEC_VER_NUM contents */
  caps1 &= HAL_SDHCI_CAPS1_BMSK;
  reg_value =  HWIO_SDC_REG_IN (HC_VENDOR_CAPS1, (UINTN)sdcc_reg_hc[nControllerId]);
  reg_value &= ~HAL_SDHCI_CAPS1_BMSK;
  reg_value |= caps1;

  HWIO_SDC_REG_OUT (HC_VENDOR_CAPS1, (UINTN)sdcc_reg_hc[nControllerId], reg_value);
  
  return;
}

/**
 * Register SDCC_HC_REG_DLL_CONFIG 
 * Enable CMD line tracking
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 */
inline void
HAL_sdhci_EnableCmdLineTracking (const UINT32 nControllerId)
{
  UINT32 val = HWIO_SDC_REG_IN (HC_REG_DLL_CONFIG, (UINTN)sdcc_reg_hc[nControllerId]);

  HWIO_SDC_REG_OUT (HC_REG_DLL_CONFIG, (UINTN)sdcc_reg_hc[nControllerId],
                   (val | HWIO_HC_REG_DLL_CONFIG_CMD_DAT_TRACK_SEL_BMSK));
}

/**
 * Register HC_VENDOR_SPECIFIC_DDR200_CFG 
 * Enable CMDIN RCLK
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 */
inline void
HAL_sdhci_EnableCmdInRclk (const UINT32 nControllerId)
{
  UINT32 val = HWIO_SDC_REG_IN (HC_VENDOR_SPECIFIC_DDR200_CFG, (UINTN)sdcc_reg_hc[nControllerId]);

  val = val | HWIO_HC_VENDOR_SPECIFIC_DDR200_CFG_CMDIN_RCLK_EN_BMSK; 

  HWIO_SDC_REG_OUT (HC_VENDOR_SPECIFIC_DDR200_CFG, (UINTN)sdcc_reg_hc[nControllerId], val);
}

/**
 * Register SDCC_HC_VENDOR_SPECIFIC_FUNC 
 * Change and Enable Select In
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId
 * @param select_in
 *   Select In Value
 */
inline void
HAL_sdhci_HCSelectIn (const UINT32 nControllerId, UINT32 select_in)
{
  UINT32 val = HWIO_SDC_REG_IN (HC_VENDOR_SPECIFIC_FUNC, (UINTN)sdcc_reg_hc[nControllerId]);

  val = (val & ~HWIO_HC_VENDOR_SPECIFIC_FUNC_HC_SELECT_IN_BMSK) | 
        (select_in << HWIO_HC_VENDOR_SPECIFIC_FUNC_HC_SELECT_IN_SHFT);

  HWIO_SDC_REG_OUT (HC_VENDOR_SPECIFIC_FUNC, (UINTN)sdcc_reg_hc[nControllerId], val); 

  val = val | HWIO_HC_VENDOR_SPECIFIC_FUNC_HC_SELECT_IN_EN_BMSK; 

  HWIO_SDC_REG_OUT (HC_VENDOR_SPECIFIC_FUNC, (UINTN)sdcc_reg_hc[nControllerId], val);
}

/**
 * Register SDCC_HC_VENDOR_SPECIFIC_FUNC 
 * Disable Select In
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId
 */
inline void
HAL_sdhci_HCSelectInDisable (const UINT32 nControllerId)
{
  UINT32 val = HWIO_SDC_REG_IN (HC_VENDOR_SPECIFIC_FUNC, (UINTN)sdcc_reg_hc[nControllerId]);

  val = val & ~HWIO_HC_VENDOR_SPECIFIC_FUNC_HC_SELECT_IN_EN_BMSK; 

  HWIO_SDC_REG_OUT (HC_VENDOR_SPECIFIC_FUNC, (UINTN)sdcc_reg_hc[nControllerId], val);
}

/**
 * Register SDCC_HC_VENDOR_SPECIFIC_FUNC 
 * Change SDC4 MCLK Select value
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId
 * @param mclk_sel_value 
 *   MCLK Select value  
 */
inline void
HAL_sdhci_Sdc4MclkSelect (const UINT32 nControllerId, UINT32 mclk_sel_value)
{
  UINT32 val = HWIO_SDC_REG_IN (HC_VENDOR_SPECIFIC_FUNC, (UINTN)sdcc_reg_hc[nControllerId]);

  val = (val & ~HWIO_HC_VENDOR_SPECIFIC_FUNC_SDC4_MCLK_SEL_BMSK) | 
        (mclk_sel_value << HWIO_HC_VENDOR_SPECIFIC_FUNC_SDC4_MCLK_SEL_SHFT);

  HWIO_SDC_REG_OUT (HC_VENDOR_SPECIFIC_FUNC, (UINTN)sdcc_reg_hc[nControllerId], val);  
}

#define HC_REG_DDR_CONFIG_VALUE       0xC3040873

/**
 * Register SDCC_HC_REG_DDR_CONFIG 
 * Configuration of ddr_config input of DLL
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 *
 */
inline void
HAL_sdhci_ConfigDDR (const UINT32 nControllerId, UINT8 rclk_delay)
{
   UINT32 val = 0, hw_version = 0;

   hw_version = in_dword (0x1FC8000);

   /* WA for SDM670 MSM.1.0and QCS605 MSM.1.0 */
   if ((hw_version == 0x60040100) || (hw_version == 0x60050100))
   {
     HWIO_SDC_REG_OUT (HC_REG_DDR_CONFIG, (UINTN)sdcc_reg_hc[nControllerId], HC_REG_DDR_CONFIG_VALUE);
   }
   else
   {
     val = HWIO_SDC_REG_IN (HC_REG_DDR_CONFIG, (UINTN)sdcc_reg_hc[nControllerId]);

     val = (val & ~HWIO_HC_REG_DDR_CONFIG_PRG_RCLK_DLY_BMSK) | rclk_delay;

     HWIO_SDC_REG_OUT (HC_REG_DDR_CONFIG, (UINTN)sdcc_reg_hc[nControllerId], val);
   }
}

/**
 * Register SDCC_HC_REG_DLL_CONFIG_2 
 * Enables/disables DDR Calibration  
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 *  
 * @param calib_en
 *   DDR calibration is finished or not
 *
 */
inline void
HAL_sdhci_CalibrateDDR (const UINT32 nControllerId, BOOLEAN calib_en)
{
   UINT32 val = HWIO_SDC_REG_IN(HC_REG_DLL_CONFIG_2, (UINTN)sdcc_reg_hc[nControllerId]);

   HWIO_SDC_REG_OUT (HC_REG_DLL_CONFIG_2, (UINTN)sdcc_reg_hc[nControllerId], 
                    (val & ~HWIO_HC_REG_DLL_CONFIG_2_DDR_CAL_EN_BMSK) | (UINT8)calib_en);
}

/**
 * Register SDCC_HC_REG_DLL_STATUS 
 * return true if DDR_DLL_LOCK_JDR is set 
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 *
 * @return
 *   TRUE indicates DDR_DLL_LOCK_JDR is set. FALSE otherwise
 */
inline BOOLEAN
HAL_sdhci_DDRDllLock (const UINT32 nControllerId)
{
   UINT32 val = HWIO_SDC_REG_IN(HC_REG_DLL_STATUS, (UINTN)sdcc_reg_hc[nControllerId]);
   return (0 != (val & HWIO_HC_REG_DLL_STATUS_DDR_DLL_LOCK_JDR_BMSK));
}

/**
 * Register SDCC_HC_VENDOR_SPECIFIC_FUNC3 
 * Enable/disable PWRSAVE DLL
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 *  
 * @param pwr_save
 *   Enable/disable PWRSAVE DLL
 *   
 */
inline void
HAL_sdhci_PowerSaveDll (const UINT32 nControllerId, BOOLEAN pwr_save)
{
   UINT32 val = HWIO_SDC_REG_IN(HC_VENDOR_SPECIFIC_FUNC3, (UINTN)sdcc_reg_hc[nControllerId]);

   HWIO_SDC_REG_OUT (HC_VENDOR_SPECIFIC_FUNC3, (UINTN)sdcc_reg_hc[nControllerId], 
                    (val & ~HWIO_HC_VENDOR_SPECIFIC_FUNC3_PWRSAVE_DLL_BMSK) | 
                    ((UINT8)pwr_save << HWIO_HC_VENDOR_SPECIFIC_FUNC3_PWRSAVE_DLL_SHFT));
}

/**
 * Register SDCC_HC_REG_DLL_CONFIG
 * Clear DLL_RST bit
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 *  
 */
inline void
HAL_sdhci_ClearDLLRST (const UINT32 nControllerId)
{
   UINT32 val = HWIO_SDC_REG_IN(HC_REG_DLL_CONFIG, (UINTN)sdcc_reg_hc[nControllerId]);
   
   HWIO_SDC_REG_OUT (HC_REG_DLL_CONFIG, 
                    (UINTN)sdcc_reg_hc[nControllerId], 
                    (val & ~HWIO_HC_REG_DLL_CONFIG_DLL_RST_BMSK));
}

/**
 * Register SDCC_HC_REG_DLL_CONFIG_2 
 * Enables/disables DLL clock
 *
 * @param nControllerId
 *   SD controller number, as described in \ref nControllerId .
 *  
 * @param bEnable
 *   Enable or disable DLL clock
 *
 */
inline void
HAL_sdhci_DLLClockEnable (const UINT32 nControllerId, BOOLEAN bEnable)
{
   UINT32 val = HWIO_SDC_REG_IN(HC_REG_DLL_CONFIG_2, (UINTN)sdcc_reg_hc[nControllerId]);

   if (bEnable)
   {
      /* Clear DLL_CLOCK_DISABLE to enable DLL */
      val &= ~HWIO_HC_REG_DLL_CONFIG_2_DLL_CLOCK_DISABLE_BMSK;	  
   }
   else
   {
      /* Set DLL_CLOCK_DISABLE to disable DLL */
      val |= HWIO_HC_REG_DLL_CONFIG_2_DLL_CLOCK_DISABLE_BMSK;
   }   
   HWIO_SDC_REG_OUT (HC_REG_DLL_CONFIG_2, (UINTN)sdcc_reg_hc[nControllerId], val);
}

#endif /* !HALSDCC_SDCC5_H */

