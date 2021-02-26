//============================================================================
/**
 * @file        qusb_dci_Sdm670.c
 * @author      shreyasr
 * @date        19-June-2013
 *
 * @brief       QUSB (Qualcomm High-Speed USB) DCI (Device-Controller-Interface) PHY specific handling.
 *
 * @details      There might be different PHY types for different targets.
 *               When using 3-wired Full-Speed PHY the USB Core (Link) cannot conntrol the D+/D-.
 * 
 * @note 
 * 
 * @ref         Design Ware Controller spec "DWC_usb3_databook.pdf".
 *
 *              Copyright (c) 2013-2015, 2017 QUALCOMM Technologies Incorporated.
 *              All Rights Reserved.
 *              Qualcomm Confidential and Proprietary
 * 
 */
//============================================================================

// ===========================================================================
// 
//                            EDIT HISTORY FOR FILE
//   This section contains comments describing changes made to the module.
//   Notice that changes are listed in reverse chronological order.
// 
// 
// when          who     what, where, why
// ----------   -----    ----------------------------------------------------------
// 2017-02-02   pm       Added UsbSharedlib
// 2015-02-26   ck       Removed include of TARGET_DEF_H
// 2013-06-19   shreyasr Added changes for MDM9x35
// 2012-04-14   tnk      Added flat file changes 
// 2010-04-14   yli      MSM8960 branch
// 2008-09-03   amirs    First Draft
// 
// ===========================================================================

//----------------------------------------------------------------------------
// Include Files
//----------------------------------------------------------------------------
#include "qusb_dci_common_hwio_845.h"
#include "qusb_dci_common.h"
#include "qusb_dci_api.h"         // DCI API Prototypes
#include "qusb_dci_private.h"     // dQH_t
#include "qusb_log.h" 
#include "ClockBoot.h"
#include "com_dtypes.h"           // common defines - basic types as uint32 etc
#include "HALhwio.h"              // For OUTF and INF definitions
#include "DALSys.h"
#include "pm_schg_usb.h"          // Type-C Lane Detection


//----------------------------------------------------------------------------
// Preprocessor Definitions and Constants
//----------------------------------------------------------------------------

#define QUSB_DCI_MAX_TX_FIFO_SIZE  (0xA65)
#define QUSB_DCI_MAX_TX_FIFO_NUM   (16)

//----------------------------------------------------------------------------
// Type Declarations
//----------------------------------------------------------------------------
typedef struct
{
  uint16 base_addr;
  uint16 depth;
} qusb_dci_tx_fifo_t;

//----------------------------------------------------------------------------
// Global Data Definitions
//----------------------------------------------------------------------------
core_info_t dci_core_info[DCI_MAX_CORES];

//----------------------------------------------------------------------------
// Static Variable Definitions
//----------------------------------------------------------------------------

#define QUSB_DCI_PMI670_INDEX                                (0)

// Total timeout for USB cable to get connected after entering RAMDUMP
// delay_us_per_loop
#define QUSB_DCI_VBUS_HIGH_TIMEOUT_DELAY_US_PER_LOOP          (100)
#define QUSB_DCI_VBUS_HIGH_TIMEOUT_LOOP_COUNT                 (300000)

// Total timeout for CC logic to finish for SS LANE detection is
// delay_us_per_loop * loop_count.   Currently 400ms from VBUS detect
#define QUSB_DCI_SS_LANE_TIMEOUT_DELAY_US_PER_LOOP            (100)
#define QUSB_DCI_SS_LANE_TIMEOUT_LOOP_COUNT                   (4000)

// Super Speed PHY lane: zero for lane0.  nonzero for lane1. Default: 0
static uint32 qusb_dci_ss_lane            = 0; 

/*

Formula to calculate optimal TxFIFO size: TxFIFOSize (in mdwidth) = (BURST_SIZE*(1024+mdwidth))+mdwidth
Mdwidth = 64bit bus / 8bits = 8byte.

BULK IN EP:
* Total TxFIFO size in MSM8996 in (mdwidth) = 0xA65 or 21288 bytes i.e. ~20Kbytes
* In SBL, TxFIFO size is set to HW default i.e. 0x184(mdwidth) or ~3Kbytes. If device advertises NumP=4, this might cause flow control (throughput loss) by setting EOB=Y on every third packet.
* For IN EP, NumP=4/8 and FIFOSize = 0x205/0x40A i.e. ~4/8Kbytes, seems optimal. 
* Device has to honor host’s(receiver’s) NumP. Since Win7 xHCI advertises NumP=4, NumP > 4 does not seem to make much difference.
* Usage of TxFIFO:
  * Since each TxFIFO maps to GTxFIFOSiz(n) register, I have selected linear mapping for TxFIFO.
  * Thus Ctrl endpoint TX/RX uses same FIFONum = 0.
 
BULK OUT EP:
* Total shared RxFIFO size in MSM8996 (mdwidth) = 0x305 or 6Kbytes.
* For OUT EP, FIFOSize is shared and fixed to 0x305 i.e. ~6Kbytes hence use NumP=6.
* In SBL, DCFG.NumP = 4 by default. Setting NumP=6 optimizes use of RxFIFO further.
* NumP used for RX endpoint needs to be programmed in DCFG.NumP

*/

static qusb_dci_tx_fifo_t qusb_dci_tx_fifo[QUSB_DCI_MAX_TX_FIFO_NUM] = 
{
  {0x0000, 0x0042}, // 0
  {0x0042, 0x040A}, // 1
  {0x044C, 0x0000}, // 2
  {0x044C, 0x0000}, // 3
  {0x044C, 0x0000}, // 4
  {0x044C, 0x0000}, // 5
  {0x044C, 0x0000}, // 6
  {0x044C, 0x0000}, // 7
  {0x044C, 0x0000}, // 8
  {0x044C, 0x0000}, // 9
  {0x044C, 0x0000}, // 10
  {0x044C, 0x0000}, // 11
  {0x044C, 0x0000}, // 12
  {0x044C, 0x0000}, // 13
  {0x044C, 0x0000}, // 14
  {0x044C, 0x0000}, // 15
};

//----------------------------------------------------------------------------
// Static Function Declarations and Definitions
//----------------------------------------------------------------------------

static boolean qusb_dci_allocate_tx_fifo(void)
{
  uint32 base_addr;
  uint32 depth;
  uint32* addr;
  int i;

  addr = (uint32*)HWIO_USB3_PRI_GTXFIFOSIZ_REGS_p_GTXFIFOSIZ0_ADDR(0);
  
  for(i=0; i<QUSB_DCI_MAX_TX_FIFO_NUM; i++)
  {
    base_addr = (qusb_dci_tx_fifo[i].base_addr << HWIO_USB3_PRI_GTXFIFOSIZ_REGS_p_GTXFIFOSIZ0_TXFSTADDR_N_SHFT) 
                 & HWIO_USB3_PRI_GTXFIFOSIZ_REGS_p_GTXFIFOSIZ0_TXFSTADDR_N_BMSK;
    depth = (qusb_dci_tx_fifo[i].depth << HWIO_USB3_PRI_GTXFIFOSIZ_REGS_p_GTXFIFOSIZ0_TXFDEP_N_SHFT) 
            & HWIO_USB3_PRI_GTXFIFOSIZ_REGS_p_GTXFIFOSIZ0_TXFDEP_N_BMSK;
    addr[i] = (base_addr | depth);
    if((qusb_dci_tx_fifo[i].base_addr+qusb_dci_tx_fifo[i].depth)>QUSB_DCI_MAX_TX_FIFO_SIZE)
    {
      return FALSE;
    }
  }
  return TRUE;
}

//----------------------------------------------------------------------------
// Externalized Function Definitions
//----------------------------------------------------------------------------


//============================================================================
/**
 * @function    qusb_get_shared_imem_address
 * 
 * @brief   	Gets the shared imem address based on the hardware version
 * 
 * @note    	None          
 *  
 * @param   	None
 *
 * @return 		uint32 A 32-Bit Address
 *
 * @ref 
 *
 */
// ===========================================================================
uint64 qusb_get_shared_imem_address()
{
  return QUSB_DLOAD_INFO_ADDR_IN_IMEM;
}

// ===========================================================================
/**
 * @function    qusb_dci_enable_usb30_clocks
 * 
 * @brief   This function will be used to turn ON the USB3.0 clocks
 * 
 * @param   None
 * 
 * @return  TRUE or FALSE depending on success or failure.
 * 
 */
// ===========================================================================
boolean qusb_dci_enable_usb30_clocks(void)
{
  boolean var = TRUE;
#if !defined(BUILD_HOSTDL) && !defined(BUILD_EHOSTDL) && !defined(FEATURE_EMMCBLD)
  var = Clock_InitUSB();
#endif

  /** Enable HS PHY reference clock, 
   ** In case USB BOOT Clock Replace API turns has it turned off  **/
  return (var);
}

// ===========================================================================
/**
 * @function    qusb_dci_disable_usb30_clocks
 * 
 * @brief   This function will be used to turn OFF the USB3.0 clocks
 * 
 * @param   None
 * 
 * @return  TRUE or FALSE depending on success or failure.
 * 
 */
// ===========================================================================
boolean qusb_dci_disable_usb30_clocks(void)
{
  boolean var = TRUE; 
#if !defined(BUILD_HOSTDL) && !defined(BUILD_EHOSTDL) && !defined(FEATURE_EMMCBLD)
  var = Clock_DisableUSB();
#endif
  return (var);
}

// ===========================================================================
/**
 * @function    qusb_dci_enable_usb30_power
 * 
 * @brief   This function enables the power domain for SNPS controller
 *
 * @param   TRUE: It enables the s/w Power Collapse 
 *          FALSE:  IT disables the s/w Power Collapse
 * 
 * @details  Used for enabling the power domain for SNPS controller
 * 
 * @return  TRUE or FALSE depending on success or failure.
 * 
 */
// ===========================================================================
void qusb_dci_enable_usb30_power_collapse(boolean enable)
{
#if !defined(BUILD_HOSTDL) && !defined(BUILD_EHOSTDL) && !defined(FEATURE_EMMCBLD)
  Clock_Usb30EnableSWCollapse(enable);
#endif
}

// ===========================================================================
/**
 * @function    qusb_dci_is_usb30_sw_collapsed
 * 
 * @brief   This function reads back if  the power domain for SNPS 
            controller is power collapsed.
 * 
 * @details Used fr cheking if USB core was initialized in PBL.
 *          If not then single enumeration cannot continue, 
 *          USB core needs to be re-enumerated.
 * 
 * @return  TRUE or FALSE depending on status of SW collapse bit.
 * 
 */
// ===========================================================================
boolean qusb_dci_is_usb30_sw_collapsed(void)
{
#if !defined(BUILD_HOSTDL) && !defined(BUILD_EHOSTDL) && !defined(FEATURE_EMMCBLD)
  return Clock_Usb30GetSWCollapse();
#else
  return FALSE;
#endif
}

// ===========================================================================
/**
 * @function    qusb_dci_get_hardware_id
 * 
 * @brief   This function will be used for getting the SNPS h/w id.
 * 
 * @param   None
 * 
 * @return  uint32 - hardware id. 
 * 
 */
// ===========================================================================
uint32 qusb_dci_get_hardware_id(void)
{
  uint32 hw_id =0x0;
  hw_id = HWIO_USB3_PRI_GSNPSID_INM(HWIO_USB3_PRI_GSNPSID_SYNOPSYSID_BMSK);
  return hw_id;
}

// ------------------------------------------------------------------------------------------------
// Core Initialization APIs
// ------------------------------------------------------------------------------------------------

// ===========================================================================
/**
 * @function    qusb_dci_configure_device_mode
 * 
 * @brief   This is used for configuring the Device config Register post reset.
 * 
 * @param   qusb_max_speed_required_t Maximum speed at which the device can operate
 * 
 * @return  None.
 * 
 */
// ===========================================================================
void qusb_dci_configure_device_mode(qusb_max_speed_required_t speed_required)
{

  qusb_device_connect_speed_t connect_speed;

  switch(speed_required)
  {
    case QUSB_MAX_SPEED_SUPER:
    {
      connect_speed = QUSB_SUPER_SPEED;
    }
    break;
    
    case QUSB_MAX_SPEED_HIGH:
    {
      connect_speed = QUSB_HIGH_SPEED;
    }
    break;

    default:
    {
      connect_speed = QUSB_FULL_SPEED;
    }
  }

  // Configure the Minimum negotiatiated Speed to  HS: 0x0 , SS: 0x4
  HWIO_USB3_PRI_DCFG_OUTM(HWIO_USB3_PRI_DCFG_DEVSPD_BMSK, (connect_speed << HWIO_USB3_PRI_DCFG_DEVSPD_SHFT));

  /*set the device address to 0x0 after reset*/ 
  HWIO_USB3_PRI_DCFG_OUTM(HWIO_USB3_PRI_DCFG_DEVADDR_BMSK,(0x0 << HWIO_USB3_PRI_DCFG_DEVADDR_SHFT)); 

  /*LPM Capability EN: 0x1, DIS:0x0*/
#ifdef FEATURE_QUSB_USBCV_MODE
  HWIO_USB3_PRI_DCFG_OUTM(HWIO_USB3_PRI_DCFG_LPMCAP_BMSK,(0x1 << HWIO_USB3_PRI_DCFG_LPMCAP_SHFT));
#else
  HWIO_USB3_PRI_DCFG_OUTM(HWIO_USB3_PRI_DCFG_LPMCAP_BMSK,(0x0 << HWIO_USB3_PRI_DCFG_LPMCAP_SHFT));
#endif

  // Update NumP based on default RxFIFO
  HWIO_USB3_PRI_DCFG_OUTM(HWIO_USB3_PRI_DCFG_NUMP_BMSK,(QUSB_BULK_OUT_BURST_SIZE << HWIO_USB3_PRI_DCFG_NUMP_SHFT));

  // Allocate TxFIFO based on table.
  if(FALSE == qusb_dci_allocate_tx_fifo())
  {
    qusb_error_log(DCI_INSUFFCIENT_HW_RSRC_LOG, 0,__LINE__);
  }

  /* Enable Device mode operation */
  HWIO_USB3_PRI_GCTL_OUTM(HWIO_USB3_PRI_GCTL_PRTCAPDIR_BMSK, (0x2 << HWIO_USB3_PRI_GCTL_PRTCAPDIR_SHFT )); 

  return;
}


// ===========================================================================
/**
 * @function    qusb_dci_configure_usb30
 * 
 * @brief   API used for PHY configuration in device Mode for USB3.0 
 *  
 * @details This API is used for configuring based on the HPG specifications 
 * 			Please follow the same steps for configuration 
 * 
 * @param   qusb_max_speed_required_t Maximum speed at which the device should
 *          operate. 
 * 
 * @return  None.
 * 
 */
// ===========================================================================
void qusb_dci_configure_usb30(qusb_max_speed_required_t speed_required)
{

  // Set DELAYP1TRANS to 0 
  HWIO_USB3_PRI_GUSB3PIPECTL_REGS_p_GUSB3PIPECTL_OUTMI(0, HWIO_USB3_PRI_GUSB3PIPECTL_REGS_p_GUSB3PIPECTL_DELAYP1TRANS_BMSK,
                               ((uint32) 0x0 << HWIO_USB3_PRI_GUSB3PIPECTL_REGS_p_GUSB3PIPECTL_DELAYP1TRANS_SHFT));
  qusb_dci_delay_us(100);

  // Controller Configuration to Peripheral mode 
  qusb_dci_configure_device_mode(speed_required);

  return;
}

// ===========================================================================
/**
 * @function    qusb_ss_phy_init
 * 
 * @brief   API used for initializing the SS PHY 
 *  
 * @details This is used for initializing the SNPS controller PHY and QMP PHY 
 * 			Configure the QMP PHY as per Table 4-1 of the QMP PHY HPG 
 * 
 * @param   None.
 * 
 * @return  TRUE if successful else FALSE.
 * 
 */
// ===========================================================================
boolean qusb_ss_phy_init(void)
{
  return usb_shared_ss_phy_init(qusb_dci_ss_lane);
}

// ===========================================================================
/**
 * @function    qusb_dci_deinit_hs_phy
 * 
 * @brief   API used to de-initialize the High Speed PHY.
 * 
 * @param   None.
 * 
 * @return  None.
 * 
 */
// ===========================================================================
void qusb_dci_deinit_hs_phy(void)
{
  //Clear the D+ Pull Up Assertion
  HWIO_USB3_PRI_HS_PHY_CTRL_OUTM(HWIO_USB3_PRI_HS_PHY_CTRL_UTMI_OTG_VBUS_VALID_BMSK, (0x0 << HWIO_USB3_PRI_HS_PHY_CTRL_UTMI_OTG_VBUS_VALID_SHFT));
  HWIO_USB3_PRI_HS_PHY_CTRL_OUTM(HWIO_USB3_PRI_HS_PHY_CTRL_SW_SESSVLD_SEL_BMSK, (0x0 << HWIO_USB3_PRI_HS_PHY_CTRL_SW_SESSVLD_SEL_SHFT));
}
// ===========================================================================
/**
 * @function    qusb_dci_enable_vbus_valid
 * 
 * @brief       API used to enable VBUS using s/w control
 * 
 * @param   qusb_max_speed_required_t - Maximum Speed required to be configured
 * 
 * @return  None.
 * 
 */
// ===========================================================================
void qusb_dci_enable_vbus_valid(qusb_max_speed_required_t speed_required)
{

  HWIO_USB3_PRI_HS_PHY_CTRL_OUTM(HWIO_USB3_PRI_HS_PHY_CTRL_UTMI_OTG_VBUS_VALID_BMSK, (0x1 << HWIO_USB3_PRI_HS_PHY_CTRL_UTMI_OTG_VBUS_VALID_SHFT));
  HWIO_USB3_PRI_HS_PHY_CTRL_OUTM(HWIO_USB3_PRI_HS_PHY_CTRL_SW_SESSVLD_SEL_BMSK, (0x1 << HWIO_USB3_PRI_HS_PHY_CTRL_SW_SESSVLD_SEL_SHFT));

  /* If we want to enable SUPER SPEED then also set LANE PWR PRESENT bit */
  if(speed_required == QUSB_MAX_SPEED_SUPER)
  {
    HWIO_USB3_PRI_SS_PHY_CTRL_OUTM(HWIO_USB3_PRI_SS_PHY_CTRL_LANE0_PWR_PRESENT_BMSK, (0x1 << HWIO_USB3_PRI_SS_PHY_CTRL_LANE0_PWR_PRESENT_SHFT));
  }

  return;
}

// ===========================================================================
/**
 * @function    qusb_dci_check_ss_phy_presence
 * 
 * @brief   This function will check if 0.9V analog supply to SS PHY is on.
 * 
 * @param   None
 * 
 * @return  TRUE:  If analog supply to SS PHY on.
 *          FALSE: If analog supply to SS PHY off.
 * 
 */
// ===========================================================================
boolean qusb_dci_ss_phy_present(void)
{
  return TRUE;
}


void qusb_dci_typec_lane_detect(void)
{
#if !defined(BUILD_HOSTDL) && !defined(BUILD_EHOSTDL) && !defined(FEATURE_EMMCBLD)
  //========================================================================
  // Use PMIC API to get Type C's SS Lane Information
  //========================================================================

  pm_schg_usb_typec_status_type typec_status  = {0};
  uint32                        loop_count    = 0;


  for (loop_count = 0; loop_count < QUSB_DCI_VBUS_HIGH_TIMEOUT_LOOP_COUNT; loop_count++)
  {
    if (PM_ERR_FLAG__SUCCESS != pm_schg_usb_get_typec_status(QUSB_DCI_PMI670_INDEX, &typec_status))
    {
      qusb_uart_log("vbus_pm_err",0);
      return;
    }
    if (typec_status.vbus_sts)
    {
      break;
    }
    qusb_dci_delay_us(QUSB_DCI_VBUS_HIGH_TIMEOUT_DELAY_US_PER_LOOP);
  }
  if (loop_count >= QUSB_DCI_VBUS_HIGH_TIMEOUT_LOOP_COUNT)
  {
    qusb_uart_log("ss_lane_vbus_timeout", typec_status.cc_out_sts);
  }

  for (loop_count = 0; loop_count < QUSB_DCI_SS_LANE_TIMEOUT_LOOP_COUNT; loop_count++)
  {
    if (PM_ERR_FLAG__SUCCESS != pm_schg_usb_get_typec_status(QUSB_DCI_PMI670_INDEX, &typec_status))
    {
      qusb_uart_log("ss_lane_pm_err",0);
      return;
    }
    if (typec_status.ccout_out_hiz_sts)
    {
      break;
    }
    qusb_dci_delay_us(QUSB_DCI_SS_LANE_TIMEOUT_DELAY_US_PER_LOOP);
  }
  
  if (loop_count >= QUSB_DCI_SS_LANE_TIMEOUT_LOOP_COUNT)
  {
    qusb_uart_log("ss_lane_cc_timeout", typec_status.cc_out_sts);
  }
  else
  {
    switch (typec_status.cc_out_sts)
    {
      case PM_TYPEC_PLUG_UNFLIP:
      {
        // Select 1st lane
        qusb_uart_log("ss_lane_1st", 0);
        qusb_dci_ss_lane = 0;
      }
      break;
      
      case PM_TYPEC_PLUG_FLIP:
      {
        // Select 2nd lane
        qusb_uart_log("ss_lane_2nd", 0);
        qusb_dci_ss_lane = 1;
      }
      break;

      default:
      {
        qusb_uart_log("ss_lane_invalid", typec_status.cc_out_sts);
      }
    }
  }
#endif // !defined(BUILD_HOSTDL) && !defined(BUILD_EHOSTDL) && !defined(FEATURE_EMMCBLD)
}



