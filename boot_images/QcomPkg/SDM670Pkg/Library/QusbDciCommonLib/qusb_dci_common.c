/*=======================================================================*//**
 * @file        qusb_dci_common.c
 * @author:     kameya
 * @date        28-Dec-2015
 *
 * @brief       QUSB common parts of DCI shared by QusbFedlLib & QusbTargetLib.
 *
 * @details     This file contains the implementation of basic APIs to be used
 *              by both QusbTargetLib and QusbFedlLib
 *
 * @note        
 *
 *              Copyright 2015, 2017 QUALCOMM Technologies Incorporated.
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
// 2017-02-02    pm      Add UsbSharedLib
// 2015-12-28    kameya  First Draft
// 
// ===========================================================================

//----------------------------------------------------------------------------
// Include Files
//----------------------------------------------------------------------------
#include "comdef.h"    // basic types as byte word uint32 etc
#include "busywait.h"  // busywait APIs
#include "DALSys.h"    // To get phy settings
#include "qusb_dci_common.h"
#include "qusb_dci_common_hwio_845.h"
#include "qusb_log.h"
#include "PlatformInfo.h"
#include "UsbSharedLib.h"
#include "pm_ldo.h"

//defines for enabling the 1.5k pull up 
#define QUSB2PHY_PORT_UTMI_CTRL1_SUSPEND_N_EN     (HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_DEBUG_CTRL3_FORCED_UTMI_SUSPEND_N_BMSK)
#define QUSB2PHY_PORT_UTMI_CTRL1_TERM_SELECT_EN   (HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_DEBUG_CTRL3_FORCED_UTMI_TERM_SELECT_BMSK)
#define QUSB2PHY_PORT_UTMI_CTRL1_XCVR_SELECT_FS   (0x1 << HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_DEBUG_CTRL3_FORCED_UTMI_XCVR_SELECT_SHFT)
#define QUSB2PHY_PORT_UTMI_CTRL1_OP_MODE          (0x0 << HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_DEBUG_CTRL3_FORCED_UTMI_OP_MODE_SHFT)
#define QUSB2PHY_PORT_UTMI_CTRL1_OP_NON_MODE      (0x1 << HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_DEBUG_CTRL3_FORCED_UTMI_OP_MODE_SHFT)
#define QUSB2PHY_PORT_UTMI_USE_UTMI_FORCED        (0x1 << HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_DEBUG_CTRL3_USE_FORCED_UTMI_SHFT)

#define QUSB_LINESTATE_CHECK_DELAY                (0x5)
#define QUSB_LINESTATE_CHECK_RETRY_CNT            (10000)  //50msec is the total wait time to deal with a bad cable

#define QUSB_DCI_PMD670_INDEX                    (1)

// ---------------------------------------------------------------------------
// EUD Tuning Parameter register table ++
// ---------------------------------------------------------------------------
#define DIV4(a)                         ((uint8)((a)>>2))
#define QUSB_DCI_EUD_PHY_REG_ARRAY_CNT  (12)

typedef struct 
{
  uint32  eud_addr_reg;
  uint8   phy_addr;
  uint32  eud_val_reg;
  uint8   phy_val;
} qusb_dci_eud_phy_reg_array_t;

static qusb_dci_eud_phy_reg_array_t qusb_dci_eud_phy_reg_array[QUSB_DCI_EUD_PHY_REG_ARRAY_CNT] =
{
  { HWIO_PERIPH_SS_EUD_Q2_TUNE_ADR00_ADDR, DIV4(HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_PWR_CTRL1_OFFS),
    HWIO_PERIPH_SS_EUD_Q2_TUNE_DAT00_ADDR, 0x23 },
  { HWIO_PERIPH_SS_EUD_Q2_TUNE_ADR01_ADDR, DIV4(HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_PLL_ANALOG_CONTROLS_TWO_OFFS),
    HWIO_PERIPH_SS_EUD_Q2_TUNE_DAT01_ADDR, 0x03 },
  { HWIO_PERIPH_SS_EUD_Q2_TUNE_ADR02_ADDR, DIV4(HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_PLL_CLOCK_INVERTERS_OFFS),
    HWIO_PERIPH_SS_EUD_Q2_TUNE_DAT02_ADDR, 0x7C },
  { HWIO_PERIPH_SS_EUD_Q2_TUNE_ADR03_ADDR, DIV4(HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_PLL_CMODE_OFFS),
    HWIO_PERIPH_SS_EUD_Q2_TUNE_DAT03_ADDR, 0x80 },
  { HWIO_PERIPH_SS_EUD_Q2_TUNE_ADR04_ADDR, DIV4(HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_PLL_PLL_LOCK_DELAY_OFFS),
    HWIO_PERIPH_SS_EUD_Q2_TUNE_DAT04_ADDR, 0x0A },
  { HWIO_PERIPH_SS_EUD_Q2_TUNE_ADR05_ADDR, DIV4(HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_PLL_PLL_DIGITAL_TIMERS_TWO_OFFS),
    HWIO_PERIPH_SS_EUD_Q2_TUNE_DAT05_ADDR, 0x19 },
  { HWIO_PERIPH_SS_EUD_Q2_TUNE_ADR06_ADDR, DIV4(HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_TUNE1_OFFS), 
    HWIO_PERIPH_SS_EUD_Q2_TUNE_DAT06_ADDR, 0x45 },
  { HWIO_PERIPH_SS_EUD_Q2_TUNE_ADR07_ADDR, DIV4(HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_TUNE2_OFFS), 
    HWIO_PERIPH_SS_EUD_Q2_TUNE_DAT07_ADDR, 0x29 },
  { HWIO_PERIPH_SS_EUD_Q2_TUNE_ADR08_ADDR, DIV4(HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_TUNE5_OFFS), 
    HWIO_PERIPH_SS_EUD_Q2_TUNE_DAT08_ADDR, 0x03 },	
  { HWIO_PERIPH_SS_EUD_Q2_TUNE_ADR09_ADDR, DIV4(HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_IMP_CTRL1_OFFS), 
    HWIO_PERIPH_SS_EUD_Q2_TUNE_DAT09_ADDR, 0x08 },
  { HWIO_PERIPH_SS_EUD_Q2_TUNE_ADR10_ADDR, DIV4(HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_PLL_BIAS_CONTROL_2_OFFS),
    HWIO_PERIPH_SS_EUD_Q2_TUNE_DAT10_ADDR, 0x28},
  { HWIO_PERIPH_SS_EUD_Q2_TUNE_ADR11_ADDR, DIV4(HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_PWR_CTRL1_OFFS),
    HWIO_PERIPH_SS_EUD_Q2_TUNE_DAT11_ADDR, 0x22 },	
};
// ---------------------------------------------------------------------------
// EUD Tuning Parameter register table --
// ---------------------------------------------------------------------------

//============================================================================

/**
* @function  qusb_dci_get_platform
*
* @brief Read platforminfo from DAL API and cache it.
*
* @Note : None.
*
* @param None.
*
* @return None.
*
*/
//============================================================================
qusb_dci_platform_type qusb_dci_get_platform(void)
{
  static qusb_dci_platform_type qusb_platform = QUSB_DCI_PLATFORM_UNKNOWN;
  DalPlatformInfoPlatformType platform;

  if (qusb_platform == QUSB_DCI_PLATFORM_UNKNOWN)
  {
    platform = PlatformInfo_Platform();

    switch(platform)
    {
      case DALPLATFORMINFO_TYPE_RUMI:
      {
        qusb_platform = QUSB_DCI_PLATFORM_RUMI;
      }
      break;
      
      case DALPLATFORMINFO_TYPE_VIRTIO:
      {
        qusb_platform = QUSB_DCI_PLATFORM_VIRTIO;
      }
      break;

      default:
        qusb_platform = QUSB_DCI_PLATFORM_SILICON;
    }
  }

  return qusb_platform;
}



//============================================================================

/**
* @function  qusb_dci_delay_ms
*
* @brief Perform delay in milliseconds.
*
* @Note : The USB Timers can not be used before the core is initialized.
*
* @param milliseconds
*
* @return none
*
*/
//============================================================================
void qusb_dci_delay_ms(uint32 msecs)
{
  int counter;

  for (counter = 0; counter < msecs; counter++)
  {
    busywait(1000); // 1 ms
  }   
}


//============================================================================
/**
* @function  qusb_dci_delay_us
*
* @brief Perform delay in microseconds.
*
* @Note : The USB Timers can not be used before the core is initialized.
*
* @param microseconds
*
* @return none
*
*/
//============================================================================
void qusb_dci_delay_us(uint32 usecs)
{
  busywait( usecs );
}

// ===========================================================================
/**
 * @function    qusb_hs_phy_gcc_reset
 * 
 * @brief   API used for resetting High Speed QUSB2 PHY using GCC control
 *  
 * @details API is used for resetting High Speed QUSB2 PHY using GCC control
 * 
 * @param   None.
 * 
 * @return  None.
 * 
 */
// ===========================================================================
void qusb_hs_phy_gcc_reset(void)
{
  HWIO_GCC_QUSB2PHY_PRIM_BCR_OUTM(HWIO_GCC_QUSB2PHY_PRIM_BCR_BLK_ARES_BMSK,
    (0x1 << HWIO_GCC_QUSB2PHY_PRIM_BCR_BLK_ARES_SHFT));

  qusb_dci_delay_us(100);

  HWIO_GCC_QUSB2PHY_PRIM_BCR_OUTM(HWIO_GCC_QUSB2PHY_PRIM_BCR_BLK_ARES_BMSK,
    (0x0 << HWIO_GCC_QUSB2PHY_PRIM_BCR_BLK_ARES_SHFT));
}

// ===========================================================================
/**
 * @function    qusb_dci_usb30_gcc_reset
 * 
 * @brief   API used for resetting the Link and PHYs using GCC control
 *  
 * @details This API is used for resetting the Link and PHYs using clock control 
 * 
 * @param   None.
 * 
 * @return  None.
 * 
 */
// ===========================================================================
void qusb_dci_usb30_gcc_reset(void)
{
  // Reset SNPS Link controller 
  HWIO_GCC_USB30_PRIM_BCR_OUTM(HWIO_GCC_USB30_PRIM_BCR_BLK_ARES_BMSK,(0x1 << HWIO_GCC_USB30_PRIM_BCR_BLK_ARES_SHFT));

  qusb_dci_delay_us(100);

  HWIO_GCC_USB30_PRIM_BCR_OUTM(HWIO_GCC_USB30_PRIM_BCR_BLK_ARES_BMSK,(0x0 << HWIO_GCC_USB30_PRIM_BCR_BLK_ARES_SHFT));

  // Reset QUSB (USB 2.0) and QMP (USB 3.0) PHYs 
  HWIO_GCC_QUSB2PHY_PRIM_BCR_OUTM(HWIO_GCC_QUSB2PHY_PRIM_BCR_BLK_ARES_BMSK,(0x1 << HWIO_GCC_QUSB2PHY_PRIM_BCR_BLK_ARES_SHFT));
  HWIO_GCC_USB3_PHY_PRIM_BCR_OUTM(HWIO_GCC_USB3_PHY_PRIM_BCR_BLK_ARES_BMSK,(0x1 << HWIO_GCC_USB3_PHY_PRIM_BCR_BLK_ARES_SHFT));
  HWIO_GCC_USB3PHY_PHY_PRIM_BCR_OUTM(HWIO_GCC_USB3PHY_PHY_PRIM_BCR_BLK_ARES_BMSK, (0x1 << HWIO_GCC_USB3PHY_PHY_PRIM_BCR_BLK_ARES_SHFT));

  qusb_dci_delay_us(100);

  HWIO_GCC_USB3PHY_PHY_PRIM_BCR_OUTM(HWIO_GCC_USB3PHY_PHY_PRIM_BCR_BLK_ARES_BMSK, (0x0 << HWIO_GCC_USB3PHY_PHY_PRIM_BCR_BLK_ARES_SHFT));
  HWIO_GCC_USB3_PHY_PRIM_BCR_OUTM(HWIO_GCC_USB3_PHY_PRIM_BCR_BLK_ARES_BMSK,(0x0 << HWIO_GCC_USB3_PHY_PRIM_BCR_BLK_ARES_SHFT));
  HWIO_GCC_QUSB2PHY_PRIM_BCR_OUTM(HWIO_GCC_QUSB2PHY_PRIM_BCR_BLK_ARES_BMSK,(0x0 << HWIO_GCC_QUSB2PHY_PRIM_BCR_BLK_ARES_SHFT));
  
  return; 
}

// ===========================================================================
/**
 * @function    qusb_hs_phy_refclk_enable
 * 
 * @brief   This function will be used to enable / disable HS PHY reference clock.
 * 
 * @param  TRUE or FALSE depending on enable or disable.
 * 
 * @return  None.
 * 
 */
// ===========================================================================
void qusb_hs_phy_refclk_enable(boolean enable)
{
   HWIO_GCC_RX1_USB2_CLKREF_EN_OUTM(HWIO_GCC_RX1_USB2_CLKREF_EN_RX1_USB2_ENABLE_BMSK,
    enable << HWIO_GCC_RX1_USB2_CLKREF_EN_RX1_USB2_ENABLE_SHFT);

  HWIO_GCC_RX1_USB2_CLKREF_EN_OUTM(HWIO_GCC_RX1_USB2_CLKREF_EN_CREF_ENABLE_BMSK,
    enable << HWIO_GCC_RX1_USB2_CLKREF_EN_CREF_ENABLE_SHFT);
}

// ===========================================================================
/**
 * @function    qusb_dci_select_utmi_clk
 * 
 * @brief   This is used for configuring the core to UTMI clock instead of pipe
 *          clock.  This needs to be called when there is no SS USB PHY.
 * 
 * @param   None.
 * 
 * @return  None.
 * 
 */
// ===========================================================================
void qusb_dci_select_utmi_clk(void)
{
  qusb_error_log(QUSB_DCI_SELECT_UTMI_CLK_LOG, 0, 0);
  
  // If operating without SS PHY, follow this sequence to disable 
  // pipe clock requirement
  HWIO_USB3_PRI_GENERAL_CFG_OUTM(HWIO_USB3_PRI_GENERAL_CFG_PIPE_UTMI_CLK_DIS_BMSK,
                      0x1 << HWIO_USB3_PRI_GENERAL_CFG_PIPE_UTMI_CLK_DIS_SHFT);

  qusb_dci_delay_us(100);

  HWIO_USB3_PRI_GENERAL_CFG_OUTM(HWIO_USB3_PRI_GENERAL_CFG_PIPE_UTMI_CLK_SEL_BMSK,
                      0x1 << HWIO_USB3_PRI_GENERAL_CFG_PIPE_UTMI_CLK_SEL_SHFT);

  qusb_dci_delay_us(100);
  
  HWIO_USB3_PRI_GENERAL_CFG_OUTM(HWIO_USB3_PRI_GENERAL_CFG_PIPE3_PHYSTATUS_SW_BMSK,
    0x1 << HWIO_USB3_PRI_GENERAL_CFG_PIPE3_PHYSTATUS_SW_SHFT);

  qusb_dci_delay_us(100);

  HWIO_USB3_PRI_GENERAL_CFG_OUTM(HWIO_USB3_PRI_GENERAL_CFG_PIPE_UTMI_CLK_DIS_BMSK,
                      0x0 << HWIO_USB3_PRI_GENERAL_CFG_PIPE_UTMI_CLK_DIS_SHFT);
}

// ===========================================================================
/**
 * @function    qusb_dci_hs_phy_update_hstx_trim
 * 
 * @brief   This function will update TUNE2 HSTX_TRIM register bits if feature is enabled.
 * 
 * @param   None.
 * 
 * @return  None.
 * 
 */
// ===========================================================================
void qusb_dci_hs_phy_update_hstx_trim(void)
{
  uint8 hstx_trim_val = 
    (HWIO_QFPROM_CORR_CALIB_ROW0_LSB_INM(HWIO_QFPROM_CORR_CALIB_ROW0_LSB_QUSB_PORT0_HSTX_TRIM_LSB_BMSK) 
    >> HWIO_QFPROM_CORR_CALIB_ROW0_LSB_QUSB_PORT0_HSTX_TRIM_LSB_SHFT);

  if(hstx_trim_val)
  {
    HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_TUNE1_OUTM(
        HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_TUNE1_HSTX_TRIM_BMSK,
        hstx_trim_val << HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_TUNE1_HSTX_TRIM_SHFT);
  }
}

// =============================================================================
/**
 * @function    qusb_dci_ahb2phy_cfg
 * 
 * @brief   Configures AHB2PHY read/write states to be 1 wait cycle each.
 * 
 * @param   None.
 * 
 * @return  None.
 * 
 */
// =============================================================================
void qusb_dci_ahb2phy_cfg(void)
{
  qusb_dci_delay_us(10);
  
  // Write 0x11 to AHB2PHY bridge CSR PERIPH_SS_AHB2PHY_TOP_CFG so that 
  // writes and reads to/from the PHY use one wait state.
  // This is essential to operate at nominal freq with lower CX rail voltages.
  HWIO_PERIPH_SS_AHB2PHY_NORTH_AHB2PHY_TOP_CFG_OUT(0x11);
}

// ===========================================================================
/**
 * @function    qusb_hs_phy_init
 * 
 * @brief   API used to initialize the High Speed PHY.
 * 
 * @param   None.
 * 
 * @return  TRUE if PHY initializes successfully, FALSE otherwise.
 * 
 */
// ===========================================================================
boolean qusb_hs_phy_init(void)
{
  return usb_shared_hs_phy_init();
}

// ===========================================================================
/**
 * @function    qusb_dci_dp_gnd_check
 * 
 * @brief   API used to check if D+ is grounded.
 * 
 * @param   None.
 * 
 * @return  None.
 * 
 */
// ===========================================================================
boolean qusb_dci_dp_gnd_check(void)
{
  uint32 is_dp_low      = 0x0;
  int32  retries        = 0x0;
  uint32 enable_fs_xvr  = 0x0;

  // Bring PHY out of power down
  HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_PWR_CTRL1_OUTM(
    HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_PWR_CTRL1_POWER_DOWN_BMSK,
    (0x0 << HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_PWR_CTRL1_POWER_DOWN_SHFT));

  // Disable the D+ Pull down 
  HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_DEBUG_CTRL4_OUTM(
    HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_DEBUG_CTRL4_FORCED_UTMI_DPPULLDOWN_BMSK, 
    (0x0 << HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_DEBUG_CTRL4_FORCED_UTMI_DPPULLDOWN_SHFT ));

  // Enable the D- Pull down 
  HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_DEBUG_CTRL4_OUTM(
    HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_DEBUG_CTRL4_FORCED_UTMI_DMPULLDOWN_BMSK, 
    (0x1 << HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_DEBUG_CTRL4_FORCED_UTMI_DMPULLDOWN_SHFT ));

  // Select the FS transceiver and enable the 1.5k pull-up. 
  enable_fs_xvr = (QUSB2PHY_PORT_UTMI_CTRL1_SUSPEND_N_EN 
                   | QUSB2PHY_PORT_UTMI_CTRL1_TERM_SELECT_EN
                   | QUSB2PHY_PORT_UTMI_CTRL1_XCVR_SELECT_FS
                   | QUSB2PHY_PORT_UTMI_CTRL1_OP_MODE
                   | QUSB2PHY_PORT_UTMI_USE_UTMI_FORCED); 

  HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_DEBUG_CTRL3_OUT(enable_fs_xvr);

  // Disable CLAMP to get valid linestate output
  HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_PWR_CTRL1_OUTM(
    HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_PWR_CTRL1_CLAMP_N_EN_BMSK,
    (0x1 << HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_PWR_CTRL1_CLAMP_N_EN_SHFT));

  // Enable D+ LEVEL LOW interrupt: DPSE_INTR_HIGH_SEL = 0, DPSE_INTR_EN = 1
  HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_INTR_CTRL_OUTM(
    HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_INTR_CTRL_DPSE_INTR_HIGH_SEL_BMSK,
    (0x0 << HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_INTR_CTRL_DPSE_INTR_HIGH_SEL_SHFT));

  HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_INTR_CTRL_OUTM(
    HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_INTR_CTRL_DPSE_INTR_EN_BMSK,
    (0x1 << HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_INTR_CTRL_DPSE_INTR_EN_SHFT));
    
  do
  {
    //wait for 5 microseconds 
    qusb_dci_delay_us(QUSB_LINESTATE_CHECK_DELAY); 
    
    //read the linestate
    is_dp_low = HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_INTR_STAT_INM(
                  HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_INTR_STAT_DPSE_INTERRUPT_BMSK) 
               >> HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_INTR_STAT_DPSE_INTERRUPT_SHFT;
    
    //if D+ is still high, then the device should not enter the EDL Mode 
    if (is_dp_low == 0)
    {
      break; 
    }
    
    retries++;

  } while (retries < QUSB_LINESTATE_CHECK_RETRY_CNT);

  // Reset value for D+ interrupt
  HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_INTR_CTRL_OUT(HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_INTR_CTRL_POR);
  // Reset DEBUG_CRTL3 (disable XCVR)
  HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_DEBUG_CTRL3_OUT(HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_DEBUG_CTRL3_POR);
  // Reset DEBUG_CTRL4 (put pull-down into PHY control)
  HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_DEBUG_CTRL4_OUT(HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_DEBUG_CTRL4_POR);
  // Reset PWR_CTRL1 (clamp_n=0, power_down=1)
  HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_PWR_CTRL1_OUT(HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_PWR_CTRL1_POR);

  if (is_dp_low == 0)
  {
    qusb_error_log(DCI_SKIP_FEDL_LOG, 0, is_dp_low);
    return FALSE;
  }

  //D+ is connected to the GND
  qusb_error_log(DCI_ENTER_FEDL_LOG, 0, is_dp_low);
  return TRUE;
}

// =============================================================================
/**
 * @function    qusb_hs_phy_nondrive_cfg
 * 
 * @brief   Function used to configure High Speed PHY in non-drive mode.
 * 
 * @param   None.
 * 
 * @return  None.
 * 
 */
// =============================================================================
void qusb_hs_phy_nondrive_cfg(void)
{
  uint32 enable_fs_xvr = 0x0;

  //Disable the D+ Pull down 
  HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_DEBUG_CTRL4_OUTM(
    HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_DEBUG_CTRL4_FORCED_UTMI_DPPULLDOWN_BMSK, 
    (0x0 << HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_DEBUG_CTRL4_FORCED_UTMI_DPPULLDOWN_SHFT ));

  // Disable the D- Pull down 
  HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_DEBUG_CTRL4_OUTM(
    HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_DEBUG_CTRL4_FORCED_UTMI_DMPULLDOWN_BMSK, 
    (0x0 << HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_DEBUG_CTRL4_FORCED_UTMI_DMPULLDOWN_SHFT ));

  // Select the FS transceiver and put into non-drive mode
  // Putting phy to suspend as SUSPEND_N = 0 (power on default)
  enable_fs_xvr = (QUSB2PHY_PORT_UTMI_CTRL1_TERM_SELECT_EN
                   | QUSB2PHY_PORT_UTMI_CTRL1_XCVR_SELECT_FS
                   | QUSB2PHY_PORT_UTMI_CTRL1_OP_NON_MODE
                   | QUSB2PHY_PORT_UTMI_USE_UTMI_FORCED); 

  HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_DEBUG_CTRL3_OUT(enable_fs_xvr);

  qusb_dci_delay_us(10);

  // Keep PHY in power down
  HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_PWR_CTRL1_OUTM(
    HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_PWR_CTRL1_POWER_DOWN_BMSK,
    (0x1 << HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_PWR_CTRL1_POWER_DOWN_SHFT));
}


// ===========================================================================
/**
 * @function    qusb_dci_eud_phy_tune
 * 
 * @brief       This function will populate QUSB2PHY registers onto EUD mode 
 *              manager CSR.
 * 
 * @param   none.
 * 
 * @return  none.
 * 
 */
// ===========================================================================
void qusb_dci_eud_phy_tune(void)
{
  qusb_dci_eud_phy_reg_array_t *array         = NULL;
  uint32                        index         = 0;
  uint8                         hstx_trim_val = 0x0;

  for (; index < QUSB_DCI_EUD_PHY_REG_ARRAY_CNT; index++)
  {
    array = &(qusb_dci_eud_phy_reg_array[index]);

    // For TUNE1[HSTX_TRIM], read the fuse value and update the table
    if (DIV4(HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_TUNE1_OFFS) == array->phy_addr)
    {
      hstx_trim_val = 
        (HWIO_QFPROM_CORR_CALIB_ROW0_LSB_INM(HWIO_QFPROM_CORR_CALIB_ROW0_LSB_QUSB_PORT0_HSTX_TRIM_LSB_BMSK) 
        >> HWIO_QFPROM_CORR_CALIB_ROW0_LSB_QUSB_PORT0_HSTX_TRIM_LSB_SHFT);
      
      if(hstx_trim_val)
      {
        out_byte_masked_ns(&(array->phy_val),
          HWIO_PERIPH_SS_QUSB2PHY_QUSB2PHY_TUNE1_HSTX_TRIM_BMSK,
          hstx_trim_val, 
          array->phy_val);
      }
    }
    
    out_byte((uint8*)(uint64)array->eud_addr_reg, array->phy_addr);
    out_byte((uint8*)(uint64)array->eud_val_reg, array->phy_val);
  }     
}


// ===========================================================================
/**
 * @function    qusb_dci_eud_init
 * 
 * @brief       This function wirte to sw_attch bit of EUD to prevent 
 *              transition to safe mode.
 * 
 * @param   none.
 * 
 * @return  none.
 * 
 */
// ===========================================================================
void qusb_dci_eud_init(void)
{
  // Program EUD sw_attach to prevent transition to safe mode
  if (HWIO_PERIPH_SS_EUD_SW_ATTACH_DET_INM(
        HWIO_PERIPH_SS_EUD_SW_ATTACH_DET_SW_ATTACH_DET_0_0_BMSK))
  {
    HWIO_PERIPH_SS_EUD_SW_ATTACH_DET_OUTM(
      HWIO_PERIPH_SS_EUD_SW_ATTACH_DET_SW_ATTACH_DET_0_0_BMSK, 0);
    qusb_dci_delay_ms(1);
  }
  
  HWIO_PERIPH_SS_EUD_SW_ATTACH_DET_OUTM(
    HWIO_PERIPH_SS_EUD_SW_ATTACH_DET_SW_ATTACH_DET_0_0_BMSK,
    (0x1 << HWIO_PERIPH_SS_EUD_SW_ATTACH_DET_SW_ATTACH_DET_0_0_SHFT));
}


// ===========================================================================
/**
 * @function    qusb_dci_enable_usb2_ldo
 * 
 * @brief   This function will be used to turn ON the USB2 LDO
 * 
 * @param   None
 * 
 * @return  TRUE or FALSE depending on success or failure.
 * 
 */
// ===========================================================================
void qusb_dci_enable_usb2_ldo(boolean enable)
{ 
#if  !defined(BUILD_HOSTDL) && !defined(BUILD_EHOSTDL) && !defined(FEATURE_EMMCBLD)
  pm_err_flag_type  pm_err;
  pm_on_off_type    pm_on_off = PM_OFF;
  
  if (enable)
  {
    pm_on_off = PM_ON;
  }
  
  pm_err = pm_ldo_sw_enable(QUSB_DCI_PMD670_INDEX, PM_LDO_7, pm_on_off);

  if (PM_ERR_FLAG__SUCCESS != pm_err)
  {
    qusb_uart_log("3V LDO failed", (uint32)pm_err);
  }

  // Enable in case of debugging
  // qusb_uart_log("3V LDO", enable);
#else
  (void)enable;  // Suppress warning
#endif
}

void qusb_dci_update_serial_number(uint32 serial_num)
{
  HWIO_PERIPH_SS_EUD_DEV_ID_3_OUT( serial_num &  0x000000FF);
  HWIO_PERIPH_SS_EUD_DEV_ID_2_OUT((serial_num &  0x0000FF00)>>8);
  HWIO_PERIPH_SS_EUD_DEV_ID_1_OUT((serial_num &  0x00FF0000)>>16);
  HWIO_PERIPH_SS_EUD_DEV_ID_0_OUT((serial_num &  0xFF000000)>>24);
}


