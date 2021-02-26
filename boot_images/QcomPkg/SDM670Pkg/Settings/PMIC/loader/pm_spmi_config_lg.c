/*! \file
*  \n
*  \brief  pm_spmi_config.c
*  \n
*  \n This file contains pmic configuration data specific for SPMI Controller's
      Peripheral for SDM670 device..
*  \n
*  \n &copy; Copyright 2015-2017 QUALCOMM Technologies, Incorporated, All Rights Reserved
*/
/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Settings/PMIC/loader/pm_spmi_config_lg.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/25/20   at      Updated channel SID for PM8005 PON (CR#2685521)
07/23/17   sv      Updated to support SDM670 Target.
03/27/17   aab     Updated it based on reviewed Access control documentt. Generated from reviewed document and done some manual grouping
03/06/17   aab     Updated it to use new structure: SpmiBusCfg_OwnerMask
01/16/17   aab     Created to support SDM845
========================================================================== */

/*-------------------------------------------------------------------------
* Include Files
* ----------------------------------------------------------------------*/

#include "pm_device.h"
#include "SpmiCfg.h"
#include "SpmiBusCfg.h" 

/*-------------------------------------------------------------------------
* Preprocessor Definitions and Constants
* ----------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
* Static Variable Definitions
* ----------------------------------------------------------------------*/

/* PMIC MMU/SMMU configuration table. */

SpmiCfg_ChannelCfg pm_spmi_channel_cfg [] =
{
/*SID, Periph ID, IRQ Owner,    Periph Owner */ 
  /* PM660 */
  {0, 0x01, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* REVID */
  {0, 0x04, SPMI_OWNER_AOP, SPMI_OWNER_AOP|SPMI_OWNER_TZ |SPMI_OWNER_MSS|SPMI_OWNER_APPS}, /* BUS */
  {0, 0x05, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* INT */
  {0, 0x06, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* SPMI */
  {0, 0x08, SPMI_OWNER_APPS, SPMI_OWNER_AOP|SPMI_OWNER_TZ| SPMI_OWNER_APPS }, /* PON */
  {0, 0x09, SPMI_OWNER_AOP, SPMI_OWNER_AOP|SPMI_OWNER_APPS    }, /* MISC */
  {0, 0x0B, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* VREF_MSM */
  {0, 0x10, SPMI_OWNER_APPS, SPMI_OWNER_APPS    }, /* SCHG_CHGR */
  {0, 0x11, SPMI_OWNER_APPS, SPMI_OWNER_APPS    }, /* SCHG_OTG */
  {0, 0x12, SPMI_OWNER_APPS, SPMI_OWNER_APPS    }, /* SCHG_BATIF */
  {0, 0x13, SPMI_OWNER_APPS, SPMI_OWNER_APPS    }, /* SCHG_USB */
  {0, 0x14, SPMI_OWNER_APPS, SPMI_OWNER_APPS    }, /* SCHG_DC */
  {0, 0x15, SPMI_OWNER_APPS, SPMI_OWNER_APPS    }, /* QNOVO */
  {0, 0x16, SPMI_OWNER_APPS, SPMI_OWNER_APPS    }, /* SCHG_MISC */
  {0, 0x17, SPMI_OWNER_APPS, SPMI_OWNER_APPS    }, /* USB_PD_PHY */
  {0, 0x18, SPMI_OWNER_APPS, SPMI_OWNER_APPS    }, /* SCHG_OTP_HDR */
  {0, 0x19, SPMI_OWNER_APPS, SPMI_OWNER_APPS    }, /* SCHG_CHGR_FREQ */
  {0, 0x1B, SPMI_OWNER_APPS, SPMI_OWNER_APPS    }, /* BSI */
  {0, 0x1C, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* BUA_2UICC_DUAL_BATT_ALARM */
  {0, 0x24, SPMI_OWNER_APPS, SPMI_OWNER_APPS    }, /* TEMP_ALARM */
  {0, 0x28, SPMI_OWNER_APPS, SPMI_OWNER_APPS    }, /* COIN */
  {0, 0x2C, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* MBG1 */
  {0, 0x31, SPMI_OWNER_APPS, SPMI_OWNER_APPS    }, /* VADC_HC1_USR */
  {0, 0x32, SPMI_OWNER_MSS, SPMI_OWNER_MSS    }, /* VADC_HC2_MDM */
  {0, 0x34, SPMI_OWNER_APPS, SPMI_OWNER_APPS    }, /* VADC_HC7_BTM8 */
  {0, 0x35, SPMI_OWNER_APPS, SPMI_OWNER_APPS    }, /* VADC_HC5_VBAT */
  {0, 0x37, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* VADC_HC4_CAL */
  {0, 0x38, SPMI_OWNER_MSS, SPMI_OWNER_MSS    }, /* VADC_HC9_BTM_2 */
  {0, 0x39, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* VADC_HC10_CMN */
  {0, 0x3C, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* VDDGEN */
  {0, 0x40, SPMI_OWNER_APPS, SPMI_OWNER_APPS    }, /* FG_BATT_SOC */
  {0, 0x41, SPMI_OWNER_APPS, SPMI_OWNER_APPS    }, /* FG_BATT_INFO */
  {0, 0x42, SPMI_OWNER_APPS, SPMI_OWNER_APPS    }, /* FG_BCL */
  {0, 0x43, SPMI_OWNER_APPS, SPMI_OWNER_APPS    }, /* FG_LMH */
  {0, 0x44, SPMI_OWNER_APPS, SPMI_OWNER_APPS    }, /* FG_MEM_IF */
  {0, 0x45, SPMI_OWNER_APPS, SPMI_OWNER_APPS    }, /* FG_ADC_RR */
  {0, 0x46, SPMI_OWNER_APPS, SPMI_OWNER_APPS    }, /* FG_FT_HDR */
  {0, 0x48, SPMI_OWNER_APPS, SPMI_OWNER_APPS    }, /* FG_DMA0 */
  {0, 0x49, SPMI_OWNER_APPS, SPMI_OWNER_APPS    }, /* FG_DMA1 */
  {0, 0x4A, SPMI_OWNER_APPS, SPMI_OWNER_APPS    }, /* FG_DMA2 */
  {0, 0x4B, SPMI_OWNER_APPS, SPMI_OWNER_APPS    }, /* FG_DMA3 */
  {0, 0x50, SPMI_OWNER_AOP, SPMI_OWNER_AOP|SPMI_OWNER_MSS    }, /* XO */
  {0, 0x51, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* LN_BB_CLK1 */
  {0, 0x52, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* LN_BB_CLK2 */
  {0, 0x53, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* LN_BB_CLK3 */
  {0, 0x54, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* RF_CLK1 */
  {0, 0x55, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* RF_CLK2 */
  {0, 0x59, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* CLK_DIST */
  {0, 0x5A, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* SLEEP_CLK1 */
  {0, 0x5B, SPMI_OWNER_APPS, SPMI_OWNER_APPS    }, /* DIV_CLK1 */
  {0, 0x60, SPMI_OWNER_TZ, SPMI_OWNER_TZ    }, /* RTC_RW */
  {0, 0x61, SPMI_OWNER_APPS, SPMI_OWNER_APPS    }, /* RTC_ALARM */
  {0, 0x62, SPMI_OWNER_APPS, SPMI_OWNER_APPS    }, /* RTC_TIMER */
  {0, 0x70, SPMI_OWNER_TZ, SPMI_OWNER_TZ    }, /* PBS_CORE */
  {0, 0x71, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* PBS_CLIENT0 */
  {0, 0x72, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* PBS_CLIENT1 */
  {0, 0x73, SPMI_OWNER_MSS, SPMI_OWNER_MSS    }, /* PBS_CLIENT2 */
  {0, 0x74, SPMI_OWNER_APPS, SPMI_OWNER_APPS    }, /* PBS_CLIENT3 */
  {0, 0xC0, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* GPIO01 */
  {0, 0xC1, SPMI_OWNER_APPS, SPMI_OWNER_APPS    }, /* GPIO02 */
  {0, 0xC2, SPMI_OWNER_APPS, SPMI_OWNER_APPS    }, /* GPIO03 */
  {0, 0xC3, SPMI_OWNER_APPS, SPMI_OWNER_APPS    }, /* GPIO04 */
  {0, 0xC4, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* GPIO05 */
  {0, 0xC5, SPMI_OWNER_APPS, SPMI_OWNER_APPS    }, /* GPIO06 */
  {0, 0xC6, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* GPIO07 */
  {0, 0xC7, SPMI_OWNER_APPS, SPMI_OWNER_APPS    }, /* GPIO08 */
  {0, 0xC8, SPMI_OWNER_APPS, SPMI_OWNER_APPS    }, /* GPIO09 */
  {0, 0xC9, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* GPIO10 */
  {0, 0xCA, SPMI_OWNER_APPS, SPMI_OWNER_APPS    }, /* GPIO11 */
  {0, 0xCB, SPMI_OWNER_APPS, SPMI_OWNER_APPS    }, /* GPIO12 */
  {0, 0xCC, SPMI_OWNER_APPS, SPMI_OWNER_APPS    }, /* GPIO13 */
  {1, 0x01, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* REVID1 */
  {1, 0x10, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* BUCK_CMN_MOD */
  {1, 0x14, SPMI_OWNER_APPS, SPMI_OWNER_APPS  }, /* S1_CTRL */
  {1, 0x15, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* S1_PS */
  {1, 0x16, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* S1_FREQ */
  {1, 0x17, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* S2_CTRL */
  {1, 0x18, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* S2_PS */
  {1, 0x19, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* S2_FREQ */
  {1, 0x1A, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* S3_CTRL */
  {1, 0x1B, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* S3_PS */
  {1, 0x1C, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* S3_FREQ */
  {1, 0x1D, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* S4_CTRL */
  {1, 0x1E, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* S4_PS */
  {1, 0x1F, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* S4_FREQ */
  {1, 0x20, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* S5_CTRL */
  {1, 0x21, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* S5_PS */
  {1, 0x22, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* S5_FREQ */
  {1, 0x23, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* S6_CTRL */
  {1, 0x24, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* S6_PS */
  {1, 0x25, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* S6_FREQ */
  {1, 0x40, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* LDO01 */
  {1, 0x41, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* LDO02 */
  {1, 0x42, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* LDO03 */
  {1, 0x44, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* LDO05 */
  {1, 0x45, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* LDO06 */
  {1, 0x46, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* LDO07 */
  {1, 0x47, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* LDO08 */
  {1, 0x48, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* LDO09 */
  {1, 0x49, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* LDO10 */
  {1, 0x4A, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* LDO11 */
  {1, 0x4B, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* LDO12 */
  {1, 0x4C, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* LDO13 */
  {1, 0x4D, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* LDO14 */
  {1, 0x4E, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* LDO15 */
  {1, 0x4F, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* LDO16 */
  {1, 0x50, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* LDO17 */
  {1, 0x51, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* LDO18 */
  {1, 0x52, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* LDO19 */
  {1, 0xC0, SPMI_OWNER_APPS, SPMI_OWNER_APPS    }, /* HAPTICS */
  {14, 0x72, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* PBS_CLIENT1 */
  /* PM8005 */
  {4, 0x01, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* REVID */
  {4, 0x04, SPMI_OWNER_AOP, SPMI_OWNER_AOP|SPMI_OWNER_TZ |SPMI_OWNER_MSS|SPMI_OWNER_APPS }, /* BUS */
  {4, 0x05, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* INT */
  {4, 0x06, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* SPMI */
  {4, 0x08, SPMI_OWNER_APPS, SPMI_OWNER_AOP|SPMI_OWNER_TZ| SPMI_OWNER_APPS }, /* PON */
  {4, 0x09, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* MISC */
  {4, 0x24, SPMI_OWNER_APPS, SPMI_OWNER_APPS    }, /* TEMP_ALARM */
  {4, 0x2C, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* MBG1 */
  {4, 0x3C, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* VDDGEN */
  {4, 0x59, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* CLK_DIST */
  {4, 0x70, SPMI_OWNER_TZ, SPMI_OWNER_TZ    }, /* PBS_CORE */
  {4, 0x71, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* PBS_CLIENT0 */
  {4, 0x72, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* PBS_CLIENT1 */
  {4, 0xC0, SPMI_OWNER_APPS, SPMI_OWNER_APPS    }, /* GPIO01 */
  {4, 0xC1, SPMI_OWNER_APPS, SPMI_OWNER_APPS    }, /* GPIO02 */
  {4, 0xC2, SPMI_OWNER_APPS, SPMI_OWNER_APPS    }, /* GPIO03 */
  {4, 0xC3, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* GPIO04 */
  {5, 0x01, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* REVID1 */
  {5, 0x10, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* BUCK_CMN_MOD */
  {5, 0x14, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* S1_CTRL */
  {5, 0x15, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* S1_PS */
  {5, 0x16, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* S1_FREQ */
  {5, 0x17, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* S2_CTRL */
  {5, 0x18, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* S2_PS */
  {5, 0x19, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* S2_FREQ */
  {5, 0x1A, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* S3_CTRL */
  {5, 0x1B, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* S3_PS */
  {5, 0x1C, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* S3_FREQ */
  {5, 0x1D, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* S4_CTRL */
  {5, 0x1E, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* S4_PS */
  {5, 0x1F, SPMI_OWNER_AOP, SPMI_OWNER_AOP    }, /* S4_FREQ */
};


/* Number of spmi channels config entries. . */
uint32 pm_spmi_channel_cfg_size[] = {sizeof(pm_spmi_channel_cfg) / sizeof(SpmiCfg_ChannelCfg)};
