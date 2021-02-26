//============================================================================
/**
 * @file        qusb_dci.c
 * @author      shreyasr
 * @date        13-Sep-2013
 *
 * @brief       QUSB (Qualcomm Super-Speed USB) DCI (Device-Controller-Interface) implementation.
 *
 * @details     The QUSB (Qualcomm Super-Speed USB) DCI (Device-Controller-Interface) 
 *              acts like a HAL (Hardware Abtraction Layer) for Synopsys USB Hardware.
 *              The DCI should supply generic API that hides USB Hardware registers access from the upper layers.
 *              It should not be aware of Chapter-9 or specific function drivers (Mass-Storage , CDC , HID etc).
 *              It should be simple without any management algorithms.
 *              
 *              The USB Device Core Driver (DCD) shall use the DCI to implement
 *              the USB protocol chapter 9 enumeration,
 *              and handling of special BUS signaling : Reset / Suspend / Resume.
 * 
 * @note        This DCI is implementing the Synopsys hardware core.
 *              The DCI depends on the USB Core (a.k.a LINK) connection to the PHY:
 *              When a 3-wired PHY is connected (Full-Speed only) then the LINK
 *              can not control the VBUS D+ wire via the Run/Stop bit.
 * 
 * @ref         
 *
 *              Copyright (c) 2013-2015 Qualcomm Technologies Incorporated.
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
// when       who      what, where, why
// --------   ---      ----------------------------------------------------------
// 09/13/13   shreyasr Refactored the SNPS USB3 code for integration to Luxor
// 05/15/13   rthoorpu Modified the code for SNPS USB3 support. 
// 
// ===========================================================================

//----------------------------------------------------------------------------
// Include Files
//----------------------------------------------------------------------------
#include "qusb_dci_common_hwio_845.h"
#include "busywait.h"    
#include "qusb_dcd.h"
#include "LoaderUtils.h"
#include "com_dtypes.h"           // common defines - basic types as uint32 etc
#include "qusb_dci_api.h"         // DCI API Prototypes
#include "qusb_dci_private.h"     // dQH_t
#include "qusb_log.h"
#include "qusb_urb.h"             // URB structure - qusb_urb_t
#include "qusb_ch9.h"   
#include <Library/ArmLib.h>       // ARM data memory barrier (_dmb) API
#include "qusb_ldr_utils.h"
#include "pmic.h"
#include "pm_schg_usb.h"
#include "qusb_dci_common.h"

//----------------------------------------------------------------------------
// Preprocessor Definitions and Constants
//----------------------------------------------------------------------------
#define QUSB_CMDACT_DELAY        100
#define QUSB_QSRAM_BASE                         (USB3_PRI_USB30_QSRAM_REGS_REG_BASE)
#define QUSB_COOKIE_SIZE                        (0x4)
 
#define QUSB_SINGLE_BOOT_ENUM_ADDR              (QUSB_QSRAM_BASE)
#define QUSB_REMOTE_WAKEUP_COOKIE               (QUSB_QSRAM_BASE + (1*QUSB_COOKIE_SIZE))
#define QUSB_EVT_BUF_CUR_POS_COOKIE             (QUSB_QSRAM_BASE + (2*QUSB_COOKIE_SIZE))
#define QUSB_QSRAM_EVENT_BUFFER                 (QUSB_QSRAM_BASE + (4*QUSB_COOKIE_SIZE))

#define QUSB_PMI_670_INDEX                      (0)

#define QUSB_EP_XFER_RSC_INDEX_BMASK            (0x7F)

//----------------------------------------------------------------------------
// Type Declarations
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
// Global Data Definitions
//----------------------------------------------------------------------------

boolean qusb_cached_event_buffer = FALSE;

//----------------------------------------------------------------------------
// Static Variable Definitions
//----------------------------------------------------------------------------

static qusb_dci_callback_table_t *dci_callback_table_ptr = NULL;
static qusb_event_buffer_info_t qusb_evt_buf_info;
__QUSB_DMA_ZI__  qusb_trb_ring_t qusb_trb_ring[DCI_MAX_ENDPOINTS][DCI_MAX_DIRECTION];  // 2 ep * 2 dir
static uint32 qusb_dci_max_packet_size = 0;
static uint32 qusb_dci_ep0_max_packet_size = 0;
__QUSB_DMA_ZI__ uint8 qusb_control_tx_buffer[QUSB_MAX_CTRL_PKT_SIZE];

// TRB must be 16 bytes aligned 
ALIGN(16)
__QUSB_DMA_ZI__ qusb_trb_t qusb_trb_list[DCI_MAX_ENDPOINTS][DCI_MAX_DIRECTION][DCI_MAX_TRBS+DCI_ZLP_TRB];  // 4 ep * 2 dir * TDs, extra one for TX ZLP

//----------------------------------------------------------------------------
// Static Functions 
//----------------------------------------------------------------------------
static void qusb_dci_handle_ctrl_transfers(struct qusb_event_depevt *depevt);
static void qusb_dci_handle_transfer_complete(struct qusb_event_depevt *depevt);
static void qusb_dci_handle_usb_reset(void);
static void qusb_dci_handle_connect_done(void);
static void qusb_dci_handle_usb_disconnect(void);
static qusb_link_state_t qusb_dci_get_link_state(void); 
static void qusb_dci_handle_link_status_change(uint32 state);
void qusb_dci_handle_suspend_entry(void);
static void qusb_dci_handle_wakeup(void);
static uint32 qusb_dci_submit_trb_data(uint8 ep, uint8 dir);
static uint32 qusb_dci_transfer_data_request(uint8 ep, uint8 dir,void* buffer_ptr,uint32 byte_count, uint8 send_zlp, trb_ctl_type_t trb_ctrl);
static uint32 qusb_dci_get_max_packet_size(void);
void qusb_dci_core_init(struct qusb_dcd_dsc_device* dcd_context_ptr);
static uint32 qusb_dci_write_ep_cmd(uint32 ep, uint32 dir, qusb_ep_cmd_t *ep_cmd, uint8 *rsc_idx);
static uint32 qusb_dci_write_dev_cmd(qusb_dev_cmd_t *dev_cmd);
static void qusb_dci_event_buffer_init(void);
void qusb_dci_ep0_start_xfer(struct qusb_dcd_dsc_device *dcd_dev);
static void qusb_dci_handle_ep_event(struct qusb_event_depevt *depevt);
static void qusb_dci_handle_dev_event(struct qusb_event_devt  *devevt);
static void qusb_dci_prepare_trbs(uint32 ep, uint32 dir, uint64 buf_addr, uint32 byte_count, uint8 send_zlp, uint8 trb_cntrl);
static void qusb_dci_ctrl_xfer_state_machine(qusb_urb_t* urb_ptr, trb_ctl_type_t *trb_ctl);
static uint32 qusb_dci_submit_request(uint8 ep, uint8 dir);

static boolean qusb_set_stall_on_xfer_nrdy_wrong_dir = TRUE;

//----------------------------------------------------------------------------
// Superspeed API implementation
//----------------------------------------------------------------------------

// Pre-silicon definitions 
#define VUINT32             volatile uint32
#define QRBTC2_USB_BASE     (0x0A728000)
#define USER_REG_BASE       (0x0A720000)

#define QRBTC_USB2_PLL      (QRBTC2_USB_BASE  + 0x00001404)
#define QRTBC_USB2_PLLTEST1 (QRBTC2_USB_BASE  + 0x00001408)
#define QRBTC_USB2_PLLCTRL1 (QRBTC2_USB_BASE  + 0x00001410)
#define QRBTC_USB2_PLLCTRL2 (QRBTC2_USB_BASE  + 0x00001414)
#define QRBTC_USB2_PLLCTRL3 (QRBTC2_USB_BASE  + 0x00001418)

#define RUMI_HS2_ADDR       (USER_REG_BASE)
#define HS2_USB_CLK_CTRL    (RUMI_HS2_ADDR + 0x20)
#define HS2_CLK_STATUS_SEL  (RUMI_HS2_ADDR + 0x14)

#define QRBTC_CALIB_VALUE   0x1A0         //0x1A0 value specific to SDM845
#define RUMI_RESET_ADDR     (HS2_USB_CLK_CTRL)
#define RUMI_REST_VAL       (0x1 << 20) 
#define RUMI_PLL_DCM_ADDR   (HS2_CLK_STATUS_SEL)

void qusb_pre_init(void)
{
    /** Initialize the PHY on RUMI **/ 
    /** STEP 1: Initialize the PLL **/
    *(VUINT32*)QRBTC_USB2_PLL = 0x00000019;        //[1,4]HWIO_QRBTC_USB2_PLL_ADDR
    qusb_dci_delay_us(10);

    *(VUINT32*)QRBTC_USB2_PLLCTRL2 = 0x00000020;   //[5,14]HWIO_USB2_PLLCTRL2_ADDR
    qusb_dci_delay_us(10);

    *(VUINT32*)QRBTC_USB2_PLLCTRL1 = 0x00000079;   //[4,10]HWIO_USB2_PLLCTRL1_ADDR
    qusb_dci_delay_us(10);

    *(VUINT32*)QRBTC_USB2_PLLCTRL3 = 0x00000000;   //[6,18]HWIO_USB2_PLLCTRL3_ADDR
    qusb_dci_delay_us(10);

    *(VUINT32*)QRBTC_USB2_PLL = 0x00000099;        //[1,4]HWIO_QRBTC_USB2_PLL_ADDR
    qusb_dci_delay_us(10);
    
    *(VUINT32*)QRTBC_USB2_PLLTEST1 = 0x00000004;   //[2,8]HWIO_USB2_PLLTEST1_ADDR
    qusb_dci_delay_us(10);

    *(VUINT32*)QRBTC_USB2_PLL = 0x000000D9;        //[1,4]HWIO_QRBTC_USB2_PLL_ADDR
    qusb_dci_delay_us(100); // Allow QRBTC to lock PLL

    /**STEP 2: PLL setting **/
    *(VUINT32*)RUMI_PLL_DCM_ADDR = 0x5; 
    qusb_dci_delay_us(10);


    /**STEP 3: Perform the RUMI PLL Reset **/
    //Assert the Reset 
    *(VUINT32*)RUMI_RESET_ADDR = RUMI_REST_VAL;
    qusb_dci_delay_us(10);
    //De-Assert the Reset Bit 
    *(VUINT32*)RUMI_RESET_ADDR = 0x0; 
    qusb_dci_delay_us(10);
    //Shift the DCM 
    *(VUINT32*)RUMI_RESET_ADDR = QRBTC_CALIB_VALUE;
    qusb_dci_delay_us(10);

    qusb_dci_delay_ms(10);
}

static uint32 qusb_dci_init_rumi(struct qusb_dcd_dsc_device *dcd_context_ptr)
{
  uint32 index = 0x0;
  if(qusb_dci_skip_enumeration())
  {
    // Carryover enumeration from PBL 
    // Restore event buffer current position from PBL cookie
    qusb_dci_event_buffer_restore();
  
    // Update device address and speed set in PBL.
    dcd_context_ptr->address = qusb_dci_get_address();
    dcd_context_ptr->speed   = (qusb_device_connect_speed_t)qusb_dci_get_speed();
  
    // Notify FD layer to allocate appropriate descriptors based on speed
    dcd_context_ptr->notify(dcd_context_ptr, QUSB_DCD_EVT_SPEED);
  
    // Cancel previous pending transfer on EP0
    qusb_dci_cancel_ep0();
  
    qusb_dcd_set_config(dcd_context_ptr, 1);
  
    // Queue control endpoint buffer
    qusb_dci_ep0_start_xfer(dcd_context_ptr);
  
    // Drive remote wake up if link is in suspended state
    dcd_context_ptr->remote_wakeup_enabled = TRUE;
    qusb_dci_handle_suspend_entry();
  
    return DCI_OK;
  }
  
   qusb_pre_init();
  
   HWIO_USB3_PRI_GCTL_OUTM(HWIO_USB3_PRI_GCTL_PRTCAPDIR_BMSK, (0x2 << HWIO_USB3_PRI_GCTL_PRTCAPDIR_SHFT));
  
   /*Reset the controller*/
   HWIO_USB3_PRI_DCTL_OUTM(HWIO_USB3_PRI_DCTL_CSFTRST_BMSK,(0x1 << HWIO_USB3_PRI_DCTL_CSFTRST_SHFT));
  
   /*Check to see if the core is reset successfully or not*/
   for(index = 0x0 ; index < QUSB_CSFTRST_TIME_OUT ; index++)
   {
     if(0x0 == HWIO_USB3_PRI_DCTL_INM(HWIO_USB3_PRI_DCTL_CSFTRST_BMSK))
     {
       break;
     }
     qusb_dci_delay_ms(1);
   }
  
   /** Reset Device core **/ 
   if(index == QUSB_CSFTRST_TIME_OUT)
   {
     DCI_ASSERT(DCI_RESET_DEVICE_ERROR_LOG);
   }
  
   qusb_dci_configure_device_mode(QUSB_MAX_SPEED_HIGH);
   qusb_dci_core_init(dcd_context_ptr);    
  
   /* Start the controller */
   qusb_dci_set_run_stop(TRUE);
  
   qusb_dci_enable_vbus_valid(dcd_context_ptr->speed_required);

   return DCI_OK;
}

// ===========================================================================
/**
 * @function    qusb_dci_handle_u1_u2_enable
 * 
 * @brief   This function will enable link transition to U1 or U2 state.
 * 
 * @param   Device context.
 * @param   Type of event.
 * @param   Enable/Disable.
 * 
 * @return  None.
 *
 * 
 */
// ===========================================================================
void qusb_dci_handle_u1_u2_enable(struct qusb_dcd_dsc_device* dev, qusb_dci_u1_u2_event_type event, boolean enable)
{
  enable = enable ? 1 : 0;

  switch(event)
  {
    case QUSB_DCI_U1_INIT_ENABLE:
    {
#ifdef FEATURE_QUSB_USBCV_MODE
      HWIO_USB3_PRI_DCTL_OUTM(HWIO_USB3_PRI_DCTL_INITU1ENA_BMSK, enable << HWIO_USB3_PRI_DCTL_INITU1ENA_SHFT);
#endif
       dev->u1_enabled = enable;
    }
    break;
    
    case QUSB_DCI_U2_INIT_ENABLE:
    {
#ifdef FEATURE_QUSB_USBCV_MODE
      HWIO_USB3_PRI_DCTL_OUTM(HWIO_USB3_PRI_DCTL_INITU2ENA_BMSK, enable << HWIO_USB3_PRI_DCTL_INITU2ENA_SHFT);
#endif
       dev->u2_enabled = enable;
    }
    break;

    case QUSB_DCI_U1_U2_ACCPET_ENABLE:
    {
#ifdef FEATURE_QUSB_USBCV_MODE
      HWIO_USB3_PRI_DCTL_OUTM(HWIO_USB3_PRI_DCTL_ACCEPTU1ENA_BMSK, enable << HWIO_USB3_PRI_DCTL_ACCEPTU1ENA_SHFT);
      HWIO_USB3_PRI_DCTL_OUTM(HWIO_USB3_PRI_DCTL_ACCEPTU2ENA_BMSK, enable << HWIO_USB3_PRI_DCTL_ACCEPTU2ENA_SHFT);
#endif
    }
    break;

    default:
      qusb_error_log(DCI_DEFAULT_CASE_LOG, 0, __LINE__);
  }
}


// ===========================================================================
/**
 * @function    qhsusb_dci_enable_test_mode
 * 
 * @brief   Enables the USB test mode.
 * 
 * @note   
 *  
 * @param   
 *
 * @return 
 *
 * @ref SNPS spec :
 *      Table 6-61 DCTL(4:1)
 *
 */
// ===========================================================================

void qusb_dci_handle_test_mode(struct qusb_dcd_dsc_device* dev)
{ 
   switch(dev->test_mode) 
   {
     case USB_PORT_TEST_J_STATE:
     case USB_PORT_TEST_K_STATE:
     case USB_PORT_TEST_SE0_NAK:
     case USB_PORT_TEST_PACKET:
     case USB_PORT_TEST_FORCE_ENABLE:
       HWIO_USB3_PRI_DCTL_OUTM(HWIO_USB3_PRI_DCTL_TSTCTL_BMSK,(dev->test_mode << HWIO_USB3_PRI_DCTL_TSTCTL_SHFT));
       break;

     case USB_PORT_TEST_DISABLE:
       HWIO_USB3_PRI_DCTL_OUTM(HWIO_USB3_PRI_DCTL_TSTCTL_BMSK,(dev->test_mode << HWIO_USB3_PRI_DCTL_TSTCTL_SHFT));
       dev->qusb_test_mode_enabled = FALSE;
       break;

     default:
       qusb_log(DCI_TEST_MODE_INVALID_LOG, 0, dev->test_mode);
       break;
  }
}

// ===========================================================================
/**
 * @function    qusb_dci_init
 * 
 * @brief   This function will be called by the DCD to initialize the core
 * 
 * @param   * dcd_context_ptr   - DCD context pointer. It is not NULL 
 *  
 * @param   core_id             - Core ID of the controller
 *  
 *  
 * @param   *callback_table_ptr - Function Call back Pointer. It is not NULL 
 * 
 * @param 	*callback_table_ptr - Ch9 callback
 * 
 * @return  None.
 * 
 */
// ===========================================================================
uint32 qusb_dci_init(uint32 core_id, struct qusb_dcd_dsc_device* dcd_context_ptr, 
                       qusb_dci_callback_table_t *callback_table_ptr)
{
  uint32 status=DCI_OK;

  qusb_log(DCI_INIT_LOG, 0, 0x0);

  if (NULL == dcd_context_ptr || NULL == callback_table_ptr)
  {
    qusb_error_log(DCI_NULL_POINTER_ERROR_LOG, 0, __LINE__);
    return DCI_ERROR; 
  }

  /*  Save the Callbcak Table Pointer and DCD context */
  dci_callback_table_ptr = callback_table_ptr;
  dci_core_info[core_id].dcd_context_ptr = dcd_context_ptr;

  if (qusb_dci_get_platform() == QUSB_DCI_PLATFORM_RUMI)
  {
    return qusb_dci_init_rumi(dcd_context_ptr);
  }

  if (qusb_dci_skip_enumeration())
  {
    // Carryover enumeration from PBL 
    // Restore event buffer current position from PBL cookie
    qusb_dci_event_buffer_restore();

    // Update device address and speed set in PBL.
    dcd_context_ptr->address = qusb_dci_get_address();
    dcd_context_ptr->speed   = (qusb_device_connect_speed_t)qusb_dci_get_speed();

    // Notify FD layer to allocate appropriate descriptors based on speed
    dcd_context_ptr->notify(dcd_context_ptr, QUSB_DCD_EVT_SPEED);

    qusb_dcd_set_config(dcd_context_ptr, 1);

    // Queue control endpoint buffer
    qusb_dci_ep0_start_xfer(dcd_context_ptr);

    // Drive remote wake up if link is in suspended state
    dcd_context_ptr->remote_wakeup_enabled = TRUE;
    qusb_dci_handle_suspend_entry();

    return DCI_OK;
  }
 
  /** It is not guaranteed that the SNPS core is always power collapsed **/ 
  /** Put it to power collapse and then bring out of power collapse**/
  qusb_dci_enable_usb30_power_collapse(TRUE); /*Put it to power collapse explicitly*/ 

  /** Turn ON the USB Boot Clock **/
  qusb_dci_enable_usb30_clocks(); 

  /* Keep EUD in bypass mode */
  qusb_dci_eud_init();

  /** Detect the right SS PHY lane, after CC completion */
  qusb_dci_typec_lane_detect();

  qusb_dci_enable_usb2_ldo(TRUE);

  /** Bring the controller out of Power Collapse**/
  qusb_dci_enable_usb30_power_collapse(FALSE);

  /** Turn OFF USB Boot Clock **/
  qusb_dci_disable_usb30_clocks(); 

  /** Hard Reset the USB Link and PHYs using GCC control **/
  qusb_dci_usb30_gcc_reset(); 

  /** Turn ON the USB Boot Clock **/
  qusb_dci_enable_usb30_clocks(); 

  qusb_dci_ahb2phy_cfg();
  
  // Initialize SS PHY only SS PHY is present and operation mode required is SS.
  {
    boolean select_utmi_config = TRUE; // Fall back to HS Only UTMI config by default.

    if(QUSB_MAX_SPEED_SUPER == dcd_context_ptr->speed_required) 
    {
      if(qusb_dci_ss_phy_present())
      {
        select_utmi_config = (FALSE == qusb_ss_phy_init()); // Fall back to HS only UTMI config if SS PHY initialization fails.  
      }
      else
      {
        // This is necessary to make bcdUSB as 0x200 in descriptors when SS PHY is absent.
        // If bcdUSB is not advertised to be 0x200, "device can perform faster" message will pop-up.
        dcd_context_ptr->speed_required = QUSB_MAX_SPEED_HIGH;
      }
    }

    if(select_utmi_config)
    {
      qusb_dci_select_utmi_clk();
    }
  }

  /** Tune PHY2 Parameters here as needed 
   ** Even if PLL did not lock on phy initialization, continue programming 
   ** since in many cases it eventually may lock later **/
  (void)qusb_hs_phy_init();

  /** Configure and initialize the CORE **/
  qusb_dci_configure_usb30(dcd_context_ptr->speed_required);

  qusb_dci_core_init(dcd_context_ptr);

  /* Start the controller */
  qusb_dci_set_run_stop(TRUE);

  /** Enable VBUS using s/w control **/
  qusb_dci_enable_vbus_valid(dcd_context_ptr->speed_required);
  
  return status; 
}
//============================================================================
/**
 * @function  qusb_dci_core_init
 *
 * @brief
 *    This function initializes the USB device controller according to the SNPS spec:
 *    1.
 *  @Note : Endpoint 0 is designed as a control endpoint only and does not need to be
 *      configured using ENDPTCTRL0 register.
 *      It is also not necessary to initially prime Endpoint 0
 *      because the first packet received will always be a setup packet. 
 *      The contents of the first setup packet will require a response
 *      in accordance with USB device framework (Chapter 9) command set.
 *
 * @param dcd_context_ptr    
 *              A pointer to a DCD initernal context. This is not NULL.
 *
 * @return status
 *              0 = ok , otherwise error.
 *
 * @ref SNPS spec "":
 *      Chapter 8.1.1 
 *      HPG 4.4. 2 ,  steps 18 -19
 *      
 *
 */
//============================================================================
void qusb_dci_core_init(struct qusb_dcd_dsc_device* dcd_context_ptr)
{
  uint32 ep0, dir;
  qusb_ep_cmd_t ep_cmd;
  qusb_log(DCI_CORE_INIT_LOG, 0, 0x0);

  if (NULL == dcd_context_ptr)
  {
    qusb_error_log(DCI_NULL_POINTER_ERROR_LOG, 0, __LINE__);
    return;
  }

  /* Initialize of  the core ( SNPS 8.1.1 ,  HPG 4.4. 2 ,  steps 18 -19 ) */
  memset(&ep_cmd, 0, sizeof(ep_cmd));

  /* Initialize event buffers */
  qusb_dci_event_buffer_init();

  /* Initialize and configure  EP info ( for EP0 OUT, EP0 IN ) */
  ep0 = DCI_EP0; 
  /* Issue DEPSTARTCFG  ( SNPS 6.3.2.5.8  and Table 8-1) */
  ep_cmd.start_new_config.cmd.cmd_typ = QUSB_EP_CMD_START_NEW_CONFIG_VAL;
  ep_cmd.start_new_config.cmd.cmd_act = 1 ;

  if (DCI_OK != qusb_dci_write_ep_cmd(ep0, DIR_RX, &ep_cmd, NULL))
  {
    qusb_error_log(DCI_CORE_INIT_WRITE_EP_CMD_ERROR_LOG, 0, 0);
  } 

  for (dir =0 ; dir < DCI_MAX_DIRECTION; dir++) 
  {

    memset(&ep_cmd, 0, sizeof(ep_cmd));

    /* Set EP config (SNPS 6.3.2.5.1 , Table 6-69) */
    ep_cmd.set_ep_config.cmd.cmd_typ = QUSB_EP_CMD_SET_EP_CONFIG_VAL;
    ep_cmd.set_ep_config.cmd.cmd_act = QUSB_EXECUTE_EP_GENERIC_COMMAND;
    ep_cmd.set_ep_config.usb_ep_num =
    (ep0 << QUSB_EP_CMD_SET_EP_CONFIG_USB_ENDPT_NUM_SHIFT) |
    (dir << QUSB_EP_CMD_SET_EP_CONFIG_USB_ENDPT_DIR_SHIFT);
    ep_cmd.set_ep_config.fifo_num  = QUSB_EP_FIFO_NUMBER; // FIFO # of CTRL IN/OUT both should be set to same value.
    ep_cmd.set_ep_config.xfer_nrdy_en = QUSB_ENABLE_XFER_NOT_READY;
    ep_cmd.set_ep_config.xfer_cmpl_en = QUSB_ENABLE_XFER_COMPLETE;
    ep_cmd.set_ep_config.max_packet_sz = QUSB_EP0_MAX_PACKET_SIZE_SUPER;
    ep_cmd.set_ep_config.brst_siz = QUSB_BURST_SIZE_1;
    ep_cmd.set_ep_config.ep_type = QUSB_EP_TYPE_CONTROL;
    ep_cmd.set_ep_config.intr_num = QUSB_DEV_INTR_LINE;

    /* Push the EP config */
    if(DCI_OK != qusb_dci_write_ep_cmd(ep0, dir, &ep_cmd, NULL))
    {
      qusb_error_log(DCI_CORE_INIT_WRITE_EP_CMD_ERROR_LOG, 0, 0);
    }

    memset(&ep_cmd, 0, sizeof(ep_cmd));  

    /* Config EP xfer resorces  (SNPS 6.3.2.5.2 , Table 6-70) */
    ep_cmd.set_ep_xfer_rsc_config.cmd.cmd_typ = QUSB_EP_CMD_SET_EP_XFER_RSC_CONFIG_VAL;
    ep_cmd.set_ep_xfer_rsc_config.cmd.cmd_act = QUSB_EXECUTE_EP_GENERIC_COMMAND;
    ep_cmd.set_ep_xfer_rsc_config.num_xfer_res = 1; /*Bit field needs to be set to 1*/

    //Push the EP cmd
    if(DCI_OK != qusb_dci_write_ep_cmd(ep0, dir, &ep_cmd, NULL))
    {
      qusb_error_log(DCI_CORE_INIT_WRITE_EP_CMD_ERROR_LOG, 0, 0);
    }

    /* Initialize the TRBs for EP0 OUT */
    if( dir == DIR_RX) 
    {
      qusb_dci_ep0_start_xfer(dcd_context_ptr);
    }
  }

  /*Enable the event interrupts*/
  HWIO_USB3_PRI_DEVTEN_OUT(
      HWIO_USB3_PRI_DEVTEN_CONNECTDONEEVTEN_BMSK
      | HWIO_USB3_PRI_DEVTEN_USBRSTEVTEN_BMSK
      | HWIO_USB3_PRI_DEVTEN_DISSCONNEVTEN_BMSK
      | HWIO_USB3_PRI_DEVTEN_U3L2L1SUSPEN_BMSK);

  // Link event HWIO_DEVTEN_ULSTCNGEN_BMSK is not enabled since it 
  // causes U0 event to get generated too frequently overwriting other events

  /* Enable EP0 IN and OUT endpoints */
  HWIO_USB3_PRI_DALEPENA_OUTM(HWIO_USB3_PRI_DALEPENA_USBACTEP_BMSK, (0x3 << HWIO_USB3_PRI_DALEPENA_USBACTEP_SHFT));
  HWIO_USB3_PRI_GSBUSCFG1_OUTM(HWIO_USB3_PRI_GSBUSCFG1_PIPETRANSLIMIT_BMSK, (0xE << HWIO_USB3_PRI_GSBUSCFG1_PIPETRANSLIMIT_SHFT));
  HWIO_USB3_PRI_DCTL_OUTM(HWIO_USB3_PRI_DCTL_LPM_NYET_THRES_BMSK, (0x8 << HWIO_USB3_PRI_DCTL_LPM_NYET_THRES_SHFT));
  HWIO_USB3_PRI_DCTL_OUTM(HWIO_USB3_PRI_DCTL_HIRDTHRES_BMSK, (0x7 << HWIO_USB3_PRI_DCTL_HIRDTHRES_SHFT));

  return;

}
//============================================================================
/**
 * @function  qusb_dci_write_ep_cmd
 *
 * @brief
 *    This function updates the USB EP data with the device controller 
 *    
 * @param ep - The endpoint number for which the command is being written
 * @param dir - Direction of the endpoint IN/OUT
 * @param ep_cmd - The command type for the Endpoint. Ref.6.3.2.5
 *
 * @return status
 *              0 = ok , otherwise error.
 *
 * @ref SNPS spec "":
 *      Chapter 6.3.2.5 " "
 *      
 *
 */
//============================================================================
static uint32 qusb_dci_write_ep_cmd(uint32 ep, uint32 dir, qusb_ep_cmd_t *ep_cmd, uint8 *rsc_idx_ptr)
{
  uint8 ep_index;
  uint32 reg_val = 0;
  uint32 delay_count = 0;
  uint16 cmd_params;

  if (NULL == ep_cmd)
  {
    qusb_error_log(DCI_NULL_POINTER_ERROR_LOG, 0, __LINE__);
    return DCI_ERROR; 
  }

  ep_index = QUSB_EP_INDEX(ep, dir); /* index cannot exceed 32 */

  /* make sure a command isnt executing currently */
  if (HWIO_USB3_PRI_DEPCMDPAR2_REGS_p_DEPCMD_INMI(ep_index,HWIO_USB3_PRI_DEPCMDPAR2_REGS_p_DEPCMD_CMDACT_BMSK))
  {
    qusb_error_log(DCI_EP_CMD_ACTIVE_ERROR_LOG, 0, ep_index);
  }

  /* write the command and parameter registers */
  switch (ep_cmd->template.cmd.reg.cmd_typ) 
  {
    case QUSB_EP_CMD_SET_EP_CONFIG_VAL:
    case QUSB_EP_CMD_START_XFER_VAL:
    // write command parameter 1 register and fall through
    HWIO_USB3_PRI_DEPCMDPAR2_REGS_p_DEPCMDPAR1_OUTI(ep_index,ep_cmd->template.parameter1);

    case QUSB_EP_CMD_SET_EP_XFER_RSC_CONFIG_VAL:
    // write command parameter 0 register and fall through
    HWIO_USB3_PRI_DEPCMDPAR2_REGS_p_DEPCMDPAR0_OUTI(ep_index,ep_cmd->template.parameter0);

    case QUSB_EP_CMD_GET_EP_STATE_VAL:
    case QUSB_EP_CMD_SET_STALL_VAL:
    case QUSB_EP_CMD_CLEAR_STALL_VAL:
    case QUSB_EP_CMD_UPDATE_XFER_VAL:
    case QUSB_EP_CMD_END_XFER_VAL:
    case QUSB_EP_CMD_START_NEW_CONFIG_VAL:
    // write the command register
    HWIO_USB3_PRI_DEPCMDPAR2_REGS_p_DEPCMD_OUTI(ep_index,ep_cmd->template.cmd.dword);
    break;

    default:
    qusb_error_log(DCI_EP_CMD_INVALID_ERROR_LOG, 0, ep_cmd->template.cmd.reg.cmd_typ);
    return QUSB_ERR_INVALID_COMMAND;
  }

  /* poll until cmdact becomes zero */
  do 
  {
    reg_val = HWIO_USB3_PRI_DEPCMDPAR2_REGS_p_DEPCMD_INMI(ep_index, HWIO_USB3_PRI_DEPCMDPAR2_REGS_p_DEPCMD_CMDACT_BMSK);
    if (!(reg_val & HWIO_USB3_PRI_DEPCMDPAR2_REGS_p_DEPCMD_CMDACT_BMSK))
    {
      if((NULL != rsc_idx_ptr) && (ep_cmd->template.cmd.reg.cmd_typ == QUSB_EP_CMD_START_XFER_VAL))
      {
        cmd_params = HWIO_USB3_PRI_DEPCMDPAR2_REGS_p_DEPCMD_INMI(ep_index, HWIO_USB3_PRI_DEPCMDPAR2_REGS_p_DEPCMD_COMMANDPARAM_BMSK) 
                      >> HWIO_USB3_PRI_DEPCMDPAR2_REGS_p_DEPCMD_COMMANDPARAM_SHFT;
        *rsc_idx_ptr = (uint8)(cmd_params & QUSB_EP_XFER_RSC_INDEX_BMASK);
        qusb_log(EP_COMMAND_COMPLETE_LOG, 0, *rsc_idx_ptr);
      }
      return DCI_OK;
    }
    qusb_dci_delay_us(100);
    delay_count ++;
  }while (delay_count < QUSB_CMDACT_DELAY);

  qusb_uart_log("ep_cmd_write failed, cmd_type", ep_cmd->template.cmd.reg.cmd_typ);
  return DCI_ERROR;
}
//============================================================================
/**
 * @function  qusb_dci_write_dev_cmd
 *
 * @brief
 *    This function updates the USB EP data with the device controller 
 *    
 * @param ep_cmd
 *
 * @return status
 *              0 = ok , otherwise error.
 *
 * @ref SNPS spec "":
 *      Chapter 6.3.2.5 " "
 *      
 *
 */
//============================================================================
static uint32 qusb_dci_write_dev_cmd(qusb_dev_cmd_t *dev_cmd)
{
  uint32 reg_val = 0x0;
  uint32 delay_count = 0;
  if(NULL == dev_cmd)
  {
    qusb_error_log(DCI_NULL_POINTER_ERROR_LOG, 0, __LINE__);
    return DCI_ERROR;
  }

    /*Check if the command execution isn't in progress*/
  if(HWIO_USB3_PRI_DGCMD_INM(HWIO_USB3_PRI_DGCMD_CMDACT_BMSK))
  {
    qusb_error_log(DCI_DEV_CMD_ACTIVE_ERROR_LOG, 0, 0);
  }

  /*write the command and parameter register*/
  switch(dev_cmd->template.cmd.reg.cmd_typ)
  {
    case QUSB_DEV_CMD_TX_SET_LINK_FN_LMP_VAL:            
    case QUSB_DEV_CMD_SET_PERIODIC_PARAMS_VAL:           
    case QUSB_DEV_CMD_TX_FN_WAKE_DEV_NOTIFY_VAL:         
    case QUSB_DEV_CMD_SET_SCRATCHPAD_BUF_LO_VAL:         
    case QUSB_DEV_CMD_SET_SCRATCHPAD_BUF_HI_VAL:         
    case QUSB_DEV_CMD_TX_FN_HOST_REQ_NOTIFY_VAL:         
    case QUSB_DEV_CMD_TX_DEVICE_NOTIFY_VAL:              
    case QUSB_DEV_CMD_SELECTED_FIFO_FLUSH_VAL:
      /*write the parameter*/
      HWIO_USB3_PRI_DGCMDPAR_OUT(dev_cmd->template.parameter); 
      /*write the command register*/ 
      HWIO_USB3_PRI_DGCMD_OUT(dev_cmd->template.cmd.dword);
      break;
    case QUSB_DEV_CMD_ALL_FIFO_FLUSH_VAL:
      /*write the command register*/ 
      HWIO_USB3_PRI_DGCMD_OUT(dev_cmd->template.cmd.dword);
    break;
    default:
    qusb_error_log(DCI_DEV_CMD_INVALID_ERROR_LOG, 0, dev_cmd->template.cmd.reg.cmd_typ);
    return QUSB_ERR_INVALID_COMMAND;			
  }

  /**Poll until the CMDACT becomes 0**/
  do
  {
    reg_val = HWIO_USB3_PRI_DGCMD_INM(HWIO_USB3_PRI_DGCMD_CMDACT_BMSK);
    if(!(reg_val & HWIO_USB3_PRI_DGCMD_CMDACT_BMSK))
    {
      return DCI_OK;
    }

    qusb_dci_delay_us(100);  
    delay_count ++;
  }while(delay_count < QUSB_CMDACT_DELAY); 

  return DCI_ERROR; 
}
//============================================================================
/**
 * @function  qusb_dci_event_buffer_restore
 *
 * @brief
 *    This function restores the USB device controller event buffer.
 *    To be used during single enumeration.
 *    
 * @param None
 *
 * @return None
 *
 *      
 *
 */
//============================================================================
void qusb_dci_event_buffer_restore(void)
{
  qusb_evt_buf_info.buf = (uint8 *)QUSB_QSRAM_EVENT_BUFFER;

  // Read the event buffer current position cookie
  qusb_dci_read_evt_buff_cur_pos_cookie(&qusb_evt_buf_info.cur_pos);

  qusb_log(DCI_EVENT_BUFFER_RESTORE_LOG, 0, qusb_evt_buf_info.cur_pos);

}
//============================================================================
/**
 * @function  qusb_dci_event_buffer_init
 *
 * @brief
 *    This function initializes the USB device controller event buffers:
 *    
 * @param None
 *
 * @return None
 *
 * @ref SNPS spec "":
 *      Chapter 8.2.2 " "
 *      
 *
 */
//============================================================================
static void qusb_dci_event_buffer_init(void)
{
  qusb_evt_buf_info.buf = (uint8 *)QUSB_QSRAM_EVENT_BUFFER;
  qusb_evt_buf_info.cur_pos = 0x0; 

  qusb_log(DCI_EVENT_BUFFER_INIT_LOG, 0, 0x0);

  HWIO_USB3_PRI_GEVNTADRLO_REGS_p_GEVNTADRLO_OUTMI(0, HWIO_USB3_PRI_GEVNTADRLO_REGS_p_GEVNTADRLO_EVNTADRLO_BMSK, 
                                               (((uint32)(qusb_evt_buf_info.buf)) << HWIO_USB3_PRI_GEVNTADRLO_REGS_p_GEVNTADRLO_EVNTADRLO_SHFT));
    
  HWIO_USB3_PRI_GEVNTADRLO_REGS_p_GEVNTADRHI_OUTMI(0, HWIO_USB3_PRI_GEVNTADRLO_REGS_p_GEVNTADRHI_EVNTADRHI_BMSK ,
                                               0x0 << HWIO_USB3_PRI_GEVNTADRLO_REGS_p_GEVNTADRHI_EVNTADRHI_SHFT );
    
  HWIO_USB3_PRI_GEVNTADRLO_REGS_p_GEVNTSIZ_OUTMI(0, HWIO_USB3_PRI_GEVNTADRLO_REGS_p_GEVNTSIZ_EVENTSIZ_BMSK, 
                                               EVENT_BUFFER_SIZE << HWIO_USB3_PRI_GEVNTADRLO_REGS_p_GEVNTSIZ_EVENTSIZ_SHFT );
    
  HWIO_USB3_PRI_GEVNTADRLO_REGS_p_GEVNTCOUNT_OUTMI(0,HWIO_USB3_PRI_GEVNTADRLO_REGS_p_GEVNTCOUNT_EVNTCOUNT_BMSK,
                                               0x0 << HWIO_USB3_PRI_GEVNTADRLO_REGS_p_GEVNTCOUNT_EVNTCOUNT_SHFT );
}

//============================================================================
/**
 * @function qusb_dci_ep0_start_xfer
 * @brief   Start a new ep0 transfer for receiving setup.
 *
 * @param qusb_dcd_dsc_device, pointer to device structure which is not NULL    
 *
 * @return  None.
 *
 * @ref SNPS spec 
 *
 */
//============================================================================
void qusb_dci_ep0_start_xfer(struct qusb_dcd_dsc_device *dcd_dev)
{
    if (NULL == dcd_dev)
    {
      qusb_error_log(DCI_NULL_POINTER_ERROR_LOG, 0, __LINE__);
      return;
    }
    
  dcd_dev->ctrl_state = EP0_STATE_SETUP_TRB;
  dcd_dev->ctrl_data_dir = DIR_NONE;
  //We use TRB for setup buffer 
  qusb_dcd_control_transfer(dcd_dev, DIR_RX, NULL, DCI_SETUP_BUFFER_SIZE);

}

// ===========================================================================
/**
 * @function    qusb_dci_set_run_stop
 * 
 * @brief   API used to set or clear Run Stop Bit.
 * 
 * @param  Set
 *               Value for setting or clearing run stop
 * @return  None.
 * 
 */
// ===========================================================================
void qusb_dci_set_run_stop(boolean set)
{
  qusb_log(DCI_SET_RUN_STOP_LOG, 0, set);
  HWIO_USB3_PRI_DCTL_OUTM(HWIO_USB3_PRI_DCTL_RUN_STOP_BMSK,(set << HWIO_USB3_PRI_DCTL_RUN_STOP_SHFT));
}

//============================================================================
/**
 * @function qusb_dci_handle_ep_event
 * @brief   Check for any pending endpoint events  and call the relevant callbacks.
 *
 * @param qusb_event_depevt - Device Endpoint specific events.    
 *
 * @return  none.
 *
 * @ref SNPS spec 
 *      Chapter 8.2.2  
 *      Chapter 6.2.8.1 DEPEVT
 *
 */
//============================================================================
static void qusb_dci_handle_ep_event(struct qusb_event_depevt *depevt)
{
    uint8 ep_num = 0x0;
    
  if (NULL == depevt)
  {
    qusb_error_log(DCI_NULL_POINTER_ERROR_LOG, 0, __LINE__);
    return;
  }

  ep_num = depevt->phy_ep_number;

  if(ep_num < 2)
  {
    qusb_dci_handle_ctrl_transfers(depevt);
  }
  else
  {
    /* Bulk transfers */
    switch(depevt->ep_xfer_event)
    {
      case QUSB_EVT_EP_XFER_COMPLETE: 
        qusb_dci_handle_transfer_complete(depevt);
      break;

      case QUSB_EVT_EP_XFER_IN_PROGRESS: 
      break;

      case QUSB_EVT_EP_XFER_NOT_READY: 
      break;

      case QUSB_EVT_EP_CMD_COMPLETE:
        qusb_bus_log(EP_COMMAND_COMPLETE_INTR_LOG, 0, ep_num);
      break;
    }
  }
}

//============================================================================
/**
 * @function qusb_dci_handle_dev_event
 * @brief   Check for any pending dev events  and call the relevant callbacks.
 *          There are some requests which the current SBL driver doesn't 
 *          support. All those events are simply not handled. 
 *
 * @param qusb_event_devt    
 *
 * @return  none.
 *
 * @ref SNPS spec 
 *      Chapter 8.2.2  
 *      Chapter 6.2.8.2 DEVT
 *
 */
//============================================================================
static void qusb_dci_handle_dev_event(struct qusb_event_devt  *devevt)
{
  
  if (NULL == devevt)
  {
    qusb_error_log(DCI_NULL_POINTER_ERROR_LOG, 0, __LINE__);
    return;
  }
  
  qusb_log(DCI_HANDLE_DEV_EVENT_LOG, 0, devevt->dev_event);
  
  switch(devevt->dev_event)
  {
    case QUSB_EVT_DEV_USB_RESET:
      qusb_dci_handle_usb_reset();
    break;

    case QUSB_EVT_DEV_CONNECT_DONE:
      qusb_dci_handle_connect_done();
    break;

    case QUSB_EVT_DEV_USB_LINK_STATE_CHANGE:
      qusb_dci_handle_link_status_change(devevt->event_info);
    break;   

    case QUSB_EVT_DEV_DISCONNECT_EVENT:
      qusb_dci_handle_usb_disconnect();
    break;

    case QUSB_EVT_DEV_REMOTE_WAKE_UP:
      qusb_dci_handle_wakeup();
    break;

    case QUSB_EVT_DEV_HIBERNATION_REQUEST:
    break;

    case QUSB_EVT_DEV_SUSPEND_ENTRY:
      qusb_dci_handle_suspend_entry();
    break;

    case QUSB_EVT_DEV_START_OF_FRAME:
    break;

    case QUSB_EVT_DEV_ERRATIC_ERROR:
      qusb_error_log(DCI_ERRATIC_ERROR_EVENT_LOG, 0, 0);
    break;

    case QUSB_EVT_DEV_CMD_COMPLETE:
    break;

    case QUSB_EVT_DEV_EVENT_BUFFER_OVERFLOW:
      qusb_error_log(DCI_EVT_BUFFER_OVERFLOW_EVENT_LOG, 0, 0);
      while(1); // This is error condition and this should not happen.
    break;

    case QUSB_EVT_DEV_VNDR_DEV_TST_LMP_RCVED:
    break;

    default:
    break;

  }
}

// ===========================================================================
/**
 * @function    qusb_vbus_detect
 * 
 * @brief   This function will check Vbus is present using PMIC API.
 * 
 * @param   None
 * 
 * @return  TRUE: Vbus is detected.
 *          FALSE: Vbus is not detected.
 * 
 */
// ===========================================================================

boolean qusb_vbus_detect(void)
{
  boolean is_vbus_high = FALSE;
  pm_err_flag_type pm_status;

  // Read Vbus state using PMIC CHGPTH API.
  pm_status = pm_schg_usb_irq_status(QUSB_PMI_670_INDEX, PM_SCHG_USB_IRQ_USBIN_PLUGIN, 
                                              PM_IRQ_STATUS_RT, &is_vbus_high);
  if(PM_ERR_FLAG__SUCCESS == pm_status)
  {
    return (is_vbus_high);
  }
  else
  {
    qusb_log(DCI_PMIC_API_FAILURE_LOG, 0, pm_status);
    return FALSE;
  }
}


//============================================================================
/**
 * @function qusb_dci_poll_events
 * @brief   Check for any pending events (interrupts are disabled) and call the relevant callbacks.
 *
 *   When an event occurs within the core, hardware checks the enable bit that corresponds to the event 
 *   to decide whether the event will be written to the Event Buffer or not. 
 *   The Event Buffer contains one of the following types of information:
 *      -  Endpoint-Specific Event (DEPEVT)
 *      -  Device-Specific Event (DEVT)
 *   
 *   Number of events ( interrupters)  -  1
 *   Event buffer size                         -  1024 bytes
 *   Number entries in each buffer      -  256 (each event is 4 bytes long ) 
 *   initial event count                        -  0
 *   
 *   Logic : 
 *   check for the event count. If the event count is greater than zero, check for the event type ( EP or Dev ) and handle 
 *   the corresponding event. Update the HW about the number of events ( in bytes) SW has processed. 
 *   Move to the next 4 bytes (i.e. next event) in the current event and repeat it until there are no pending events.
 *               
 * @return  none.
 *
 * @ref SNPS spec 
 *      Chapter 8.2.2 , 6.2.7.3 ,  6.2.8
 *
 */
//============================================================================
void qusb_dci_poll_events(void)
{
  uint32 evt_byte_count;
  uint32 cur_buf_pos;

  if(QUSB_DCI_PLATFORM_SILICON == qusb_dci_get_platform())
  {
    // Disabling the performing SAHARA RESET on disconnect 
    // and only allow it during BUS RESET due to cable reconnect
    static boolean vbus_state=OFF;

    if(vbus_state != qusb_vbus_detect())
    {
      vbus_state = ((~vbus_state) & 0x1);
      qusb_error_log(DCI_VBUS_STATE_CHANGE_LOG, 0, vbus_state);
      (vbus_state) ? qusb_uart_log("VBUS High!", 0) : qusb_uart_log("VBUS Low", 0);


      if(OFF == vbus_state)
      {
        qusb_error_log(DCI_VBUS_REMOVAL_LOG, 0, 0);
        qusb_dci_handle_usb_disconnect();
      }

    }
  }

  /* Get the event count  ( number of valid bytes ) from the event buffer - SNPS 6.2.7.3 */
  evt_byte_count = HWIO_USB3_PRI_GEVNTADRLO_REGS_p_GEVNTCOUNT_INMI(0,
    HWIO_USB3_PRI_GEVNTADRLO_REGS_p_GEVNTCOUNT_EVNTCOUNT_BMSK);

  if (!evt_byte_count)
  {
    //No new events in buffer 
    return;
  }

  while (evt_byte_count > 0) 
  {
    union qusb_event event;

    /* position of the event to be handled in the current event buffer */
    cur_buf_pos = qusb_evt_buf_info.cur_pos;

    if(qusb_cached_event_buffer)
    {
      qusb_dci_dcache_inval(((uint8*)qusb_evt_buf_info.buf + cur_buf_pos), sizeof(union qusb_event));
    }

    /* Move the event buffer to the new location */
    event.generic = *(uint32 *)((uint8 *)qusb_evt_buf_info.buf + cur_buf_pos );

    /* EP specific Event */
    if(event.depevt.event_type == QUSB_EVT_TYPE_EP)
    { 
      qusb_dci_handle_ep_event(&event.depevt);
    }
    else if(event.devt.event_type == QUSB_EVT_TYPE_NON_EP && !(event.devt.dev_spec_event) )
    {
      /* Dev specific Event  */
      /* Event type should be "Non Ep'' and bits-7:1  should be Zero ( SNPS pg. 478, Table-649 ) */
      qusb_dci_handle_dev_event(&event.devt);
    }
    else
    {
      qusb_error_log(DCI_EVENT_ERROR_LOG,0,event.devt.dev_spec_event);
    }

    /* Move to the next Event in the current buffer */
    /* Event buffer is 1K in size and each Event entry is 4 bytes long. We wrap at 1K. */
    /* Wrap the circular buffer at EVENT_BUFFER_SIZE to ensure there is no overflow */
    qusb_evt_buf_info.cur_pos = (cur_buf_pos + QUSB_EVT_BUFFER_UNIT_SIZE ) % EVENT_BUFFER_SIZE ;
    evt_byte_count -= QUSB_EVT_BUFFER_UNIT_SIZE;

    /* SW indicates to HW how many events it processed by writing the byte count (we process 4 at a time ) */
    HWIO_USB3_PRI_GEVNTADRLO_REGS_p_GEVNTCOUNT_OUTMI(0,
            HWIO_USB3_PRI_GEVNTADRLO_REGS_p_GEVNTCOUNT_EVNTCOUNT_BMSK,
            QUSB_EVT_BUFFER_UNIT_SIZE << HWIO_USB3_PRI_GEVNTADRLO_REGS_p_GEVNTCOUNT_EVNTCOUNT_SHFT );

  }
}
// ===========================================================================
/**
 * @function    qusb_dci_handle_ctrl_transfers
 * 
 * @brief       This function handles control transfers
 * 
 * @details 
 *         
 *  @note : After receiving a new setup packet ( with xfer complete ) dci delegates the handling to dcd.
 *              Setup is processed and corresponding DATA and STATUS3 (3 stage)  or STATUS2 stages (2 stage) requests 
 *              are queued but are submitted to the HW only after receiving XFER_NRDY event for the particular stage.
 * 
 * @param *depevt - Pointer to Control Endpoint Events    
 *
 * @return  none.
 *
 * @see qusb_dci_poll_events
 *
 * @ref SNPS spec 
 *     Chapter 8.4 Control transfer programming model (2.3 hardware and later)
 *     Chapter 8.5.2 Operational Model For Setup Transfers
 *
 */
// ===========================================================================
void qusb_dci_handle_ctrl_transfers(struct qusb_event_depevt *depevt)
{
  qusb_evt_ep_evt_type    ep_evt_type;
  uint8 ep,dir;
  uint8 phy_ep_num;
  struct qusb_dcd_dsc_device *dcd_dev;
  qusb_urb_t *urb_ptr;

  if (NULL != depevt)
  {
    phy_ep_num = depevt->phy_ep_number;
    ep  = QUSB_PHY_EP_TO_EP(phy_ep_num);   
    dir = QUSB_PHY_EP_TO_DIR(phy_ep_num);

    /* EP event completion type*/
    ep_evt_type = (qusb_evt_ep_evt_type)depevt->ep_xfer_event;
  }
  else
  {
    qusb_error_log(DCI_NULL_POINTER_ERROR_LOG, 0, __LINE__);
    return;
  }

  dcd_dev = dci_core_info[DCI_SNPS_CORE].dcd_context_ptr;
  urb_ptr = qusb_trb_ring[ep][dir].urb_ptr;
  
  /* Re-initialize URB transfer index */
  if(NULL == urb_ptr)
  {
    qusb_error_log(DCI_NULL_POINTER_ERROR_LOG, 0, __LINE__);
    return;
  }

  switch (ep_evt_type) 
  {
    case QUSB_EVT_EP_XFER_COMPLETE:

      urb_ptr->transfer_index = QUSB_RSC_IDX_NOT_SET;

      /* Setup stage complete  */
      if (dcd_dev->ctrl_state == EP0_STATE_SETUP_TRB)
      {
        dcd_dev->ctrl_state = EP0_STATE_INTRPT_SETUP;   
        qusb_bus_log(XFER_COMPLETE_SETUP_LOG, 0, depevt->event_status);
        qusb_dci_handle_transfer_complete(depevt);
        dci_callback_table_ptr->setup_callback_fn(dcd_dev, &qusb_trb_list[0][0][0]);
      }
      /* Data stage complete   */
      else if (dcd_dev->ctrl_state == EP0_STATE_DATA_IN_PROGRESS)
      {
        /* Host aborted DATA stage, then start over */
        if(depevt->event_status == 0)
        {
          dcd_dev->ctrl_state = EP0_STATE_SETUP_TRB;
          qusb_dci_ep0_start_xfer(dcd_dev);
        }   
        else
        {
          qusb_bus_log(XFER_COMPLETE_DATA_LOG, 0, depevt->event_status);
          qusb_dci_handle_transfer_complete(depevt);
          dcd_dev->ctrl_state = EP0_STATE_STATUS_3;
        }
      }
      else if (dcd_dev->ctrl_state == EP0_STATE_DATA)
      {
        qusb_bus_log(XFER_COMPLETE_NOT_EXPECTED_IN_EP0_STATE_DATA_LOG, 0, depevt->event_status);
      }
      /* status stage complete */
      else if (dcd_dev->ctrl_state == EP0_STATE_STATUS_2 ||
                dcd_dev->ctrl_state == EP0_STATE_STATUS_3)
      {
        qusb_bus_log(XFER_COMPLETE_STATUS_LOG, 0, depevt->event_status);
        qusb_dci_handle_transfer_complete(depevt);
        dcd_dev->ctrl_state = EP0_STATE_SETUP_TRB;
        /* start a new setup */
        qusb_dci_ep0_start_xfer(dcd_dev);
      }
      else
      {
        /* Default handler returns stall for restarting state machine */
        dcd_dev->ctrl_state = EP0_STATE_SETUP_TRB;
        qusb_dci_stall_endpoint(ep, DIR_RX); 
      }

    break; /* QUSB_EVT_EP_XFER_COMPLETE */

    case QUSB_EVT_EP_XFER_NOT_READY: 

      /* We are expecting a xfercomplete but, instead, we receive xferNotReady(Setup), 
      * we should STALL and restart  the control transfers.  
      *  Otherwise, we just continue waiting for the XferComplete event. 
      */
      if(dcd_dev->ctrl_state == EP0_STATE_SETUP_TRB)
      {
        qusb_bus_log(XFER_NOT_READY_IN_SETUP_STAGE_ERROR_LOG, 0, dir);
        dcd_dev->ctrl_state = EP0_STATE_SETUP_TRB;
      }
      else if(dcd_dev->ctrl_state == EP0_STATE_DATA)
      {
        if (depevt->event_status == QUSB_DDEPEVT_STATUS_CONTROL_DATA)
        {
          qusb_bus_log(XFER_NOT_READY_DATA_LOG, 0, dir);
          if(dcd_dev->ctrl_data_dir == dir)
          {
            dcd_dev->ctrl_state = EP0_STATE_DATA_IN_PROGRESS;
            qusb_dci_submit_request(ep, dir);  
          }
          else
          {
            qusb_bus_log(XFER_NOT_READY_INCORRECT_DIR_ERROR_LOG, 0, dcd_dev->ctrl_data_dir);
            if(qusb_set_stall_on_xfer_nrdy_wrong_dir == TRUE)
            {
              dcd_dev->ctrl_state = EP0_STATE_SETUP_TRB;
              qusb_dci_stall_endpoint(ep, DIR_RX); 
            }
          }
        }
        else if (depevt->event_status == QUSB_DDEPEVT_STATUS_CONTROL_STATUS)
        {
          qusb_bus_log(XFER_NOT_READY_STATUS_IN_DATA_STAGE_ERROR_LOG, 0, depevt->event_status);
          dcd_dev->ctrl_state = EP0_STATE_SETUP_TRB;
          qusb_dci_stall_endpoint(ep, DIR_RX); 
        }
        else
        {
          qusb_bus_log(INVALID_EVENT_STATUS_ERROR_LOG, 0, depevt->event_status);
        }
      }
      else if(dcd_dev->ctrl_state == EP0_STATE_DATA_IN_PROGRESS)
      {
        if (depevt->event_status == QUSB_DDEPEVT_STATUS_CONTROL_DATA)
        {
          qusb_bus_log(XFER_NOT_READY_DATA_STILL_IN_PROGRESS_LOG, 0, depevt->event_status);
        }
        else
        {
          qusb_bus_log(INVALID_EVENT_STATUS_ERROR_LOG, 0, depevt->event_status);
        }
      }
      else if (dcd_dev->ctrl_state == EP0_STATE_STATUS_2)
      {
        if (depevt->event_status == QUSB_DDEPEVT_STATUS_CONTROL_STATUS)
        {
          qusb_bus_log(XFER_NOT_READY_STATUS2_LOG, 0, dir);
          if(dir == DIR_TX)
          {
            qusb_dci_submit_request(ep, dir);
          }
          else
          {
            qusb_bus_log(XFER_NOT_READY_INCORRECT_DIR_STATUS2_ERROR_LOG, 0, dcd_dev->ctrl_data_dir);
            dcd_dev->ctrl_state = EP0_STATE_SETUP_TRB;
            qusb_dci_stall_endpoint(ep, DIR_RX); 
          }
        }
        else
        {
          qusb_bus_log(XFER_NOT_READY_DATA_IN_STATUS2_STAGE_ERROR_LOG, 0, depevt->event_status);
          dcd_dev->ctrl_state = EP0_STATE_SETUP_TRB;
          qusb_dci_stall_endpoint(ep, DIR_RX); 
        }
      }
      else if (dcd_dev->ctrl_state == EP0_STATE_STATUS_3)
      {
        if (depevt->event_status == QUSB_DDEPEVT_STATUS_CONTROL_STATUS)
        {
          qusb_bus_log(XFER_NOT_READY_STATUS3_LOG, 0, dir); 
          if(dcd_dev->ctrl_data_dir != dir)
          {
            qusb_dci_submit_request(ep, dir);  
          }
          else
          {
            qusb_bus_log(XFER_NOT_READY_INCORRECT_DIR_STATUS3_ERROR_LOG, 0, dcd_dev->ctrl_data_dir);
            if(qusb_set_stall_on_xfer_nrdy_wrong_dir == TRUE)
            {
              dcd_dev->ctrl_state = EP0_STATE_SETUP_TRB;
              qusb_dci_stall_endpoint(ep, DIR_RX); 
            }
          }
        }
        else
        {
          qusb_bus_log(XFER_NOT_READY_DATA_IN_STATUS3_STAGE_ERROR_LOG, 0, depevt->event_status);
          dcd_dev->ctrl_state = EP0_STATE_SETUP_TRB;
          qusb_dci_stall_endpoint(ep, DIR_RX); 
        }
      }
      else
      {
        /* Default handler returns stall for restarting state machine */
        dcd_dev->ctrl_state = EP0_STATE_SETUP_TRB;
        qusb_dci_stall_endpoint(ep, DIR_RX); 
      }
    break; /* QUSB_EVT_EP_XFER_NOT_READY */

    case QUSB_EVT_EP_CMD_COMPLETE:
      qusb_bus_log(EP_COMMAND_COMPLETE_INTR_LOG, 0, ep);
    break;

    default:
       qusb_error_log(DCI_DEFAULT_CASE_LOG, 0, ep_evt_type);
    break;
  }
  return;
}

// ===========================================================================
/**
 * @function    qusb_dci_handle_transfer_complete
 * 
 * @brief       This function handles transfer complete event.
 * 
 * @details     
 *      After a TRB has been initialized and submitted to the HW the device controller will execute the transfer 
 *      upon the hostinitiated request.  
 *      The DCD will be notified with a USB interrupt if the Interrupt On Complete bit was set or alternately,
 *      the DCD can poll the endpoint complete register to find when the dTD had been executed.
 *      
 * 
 * @param *depevt - Pointer to Device Endpoint Events   
 *
 * @return  none.
 *
 * @see qusb_dci_poll_events
 *
 * @ref 
 */
// ===========================================================================
static void qusb_dci_handle_transfer_complete(struct qusb_event_depevt *depevt)
{
  uint8  ep;
  uint8  dir;
  qusb_urb_t *urb_ptr;
  uint8 phy_ep_num; 
  struct qusb_dcd_dsc_device *dsc_dev = dci_core_info[DCI_SNPS_CORE].dcd_context_ptr;

  if (NULL == depevt)
  {
    qusb_error_log(DCI_NULL_POINTER_ERROR_LOG, 0, __LINE__);
    return;
  }

  phy_ep_num = depevt->phy_ep_number;
  ep  = QUSB_PHY_EP_TO_EP(phy_ep_num);    
  dir = QUSB_PHY_EP_TO_DIR(phy_ep_num);
  if((ep >= DCI_MAX_ENDPOINTS)|| (dir >= DCI_MAX_DIRECTION)) 
  {
    qusb_error_log(DCI_EVT_BUFFER_OVERFLOW_EVENT_LOG, 0, __LINE__);
    return;
  }

  /* Update URB */
  urb_ptr = qusb_trb_ring[ep][dir].urb_ptr;
  if(NULL == urb_ptr)
  {
    qusb_error_log(DCI_HANDLE_TRANSFER_COMPLETE_URB_PTR_NULL_ERROR_LOG, 0, 0);
    return;
  }

  /* For Rx,  We queue TRB with max_packet_size,when the data length  is less than max packet */
  if(dir == DIR_RX)
  {
    /* BULK EP */
    if( ep > 0 && (ep < DCI_MAX_ENDPOINTS))
    {
      if(urb_ptr->transfer_length < qusb_dci_max_packet_size)
      {
        /*buf_size is updated by h/w and is always less than max_packet size*/
        urb_ptr->actual_length = qusb_dci_max_packet_size - qusb_trb_list[ep][dir][0].buf_size;
      } 
      else
      {
        /*Buffer size would always be less than or equal to transfer_length*/
        urb_ptr->actual_length = urb_ptr->transfer_length - qusb_trb_list[ep][dir][0].buf_size;
      }

      // Invalidate RX data buffer from cache before calling completion callback
      qusb_dci_dcache_inval((void*)urb_ptr->transfer_buffer_ptr, urb_ptr->actual_length);
    }
    /* Control EP */
    else
    {
      if(urb_ptr->transfer_length < qusb_dci_ep0_max_packet_size)
      {
        /*buf_size is updated by h/w and is always less than max_packet_size */
        urb_ptr->actual_length = qusb_dci_ep0_max_packet_size - qusb_trb_list[ep][dir][0].buf_size;
      } 
      else
      {
        /*Buffer size would always be less than or equal to transfer_length*/
        urb_ptr->actual_length = urb_ptr->transfer_length - qusb_trb_list[ep][dir][0].buf_size;
      }
    }
  }
  /* DIR_TX */
  else 
  {
    urb_ptr->actual_length = urb_ptr->transfer_length - qusb_trb_list[ep][dir][0].buf_size;
  }

  urb_ptr->transfer_status = QUSB_URB_STATUS_COMPLETE_OK;
  urb_ptr->transfer_index = QUSB_RSC_IDX_NOT_SET;

  if((uint64)urb_ptr->transfer_buffer_ptr == 0x0B000000)
  {
    qusb_uart_log("PIMEM dump complete, size", urb_ptr->actual_length);
  }


  /* URB completion callbacks */
  if(urb_ptr->complete_callback != NULL) 
  {
    urb_ptr->complete_callback(urb_ptr);
  }        
  
  /* Enabling the TEST mode should happen after the completion of Status stage of Set_Feature (TEST) */
  if((dsc_dev) && (dsc_dev->qusb_test_mode_enabled == TRUE))
  {
    qusb_dci_handle_test_mode(dsc_dev);
  }
}

// ===========================================================================
/**
 * @function    qusb_dci_handle_usb_reset
 * 
 * @brief       This function handles USB Reset event.
 * 
 * @details     
 *      A bus reset is used by the host to initialize downstream devices. 
 *      When a bus reset is detected, the DEVICE CONTROLLER will:
 *       1. Renegotiate its attachment speed, 
 *       2. Reset the device address to 0, 
 *       3. Notify the DCD by interrupt (assuming the USB Reset Interrupt Enable is set). 
 *      
 *      After a reset is received, all endpoints (except endpoint 0) are disabled 
 *      and any primed transactions will be canceled by the device controller. 
 *      
 *      The DCD must perform the following tasks when a reset is received:
 *       1. If a control transfer is still in progress, complete it and get the core into the “Setup a Control-Setup TRB / Start Transfer” state
 *       2. Issue a DEPENDXFER command for any active transfers (except for the default control endpoint0).
 *       3. Issue a DEPCSTALL (ClearStall) command for any endpoint that was put into STALL mode prior to the USB Reset.
 *       4. Set DevAddr to 0. 
 * 
 * @param none    
 *
 * @return  none.
 *
 * @see qusb_dci_poll_events
 *
 * @ref SNPS spec 
 *       Chapter 8.1.2 initialization on USB Reset
 */
// ===========================================================================
static void qusb_dci_handle_usb_reset(void)
{
  uint8 ep, dir;

  struct qusb_dcd_dsc_device *dcd_dev = dci_core_info[DCI_SNPS_CORE].dcd_context_ptr; 

  if (NULL == dcd_dev)
  {
    qusb_error_log(DCI_NULL_POINTER_ERROR_LOG, 0, __LINE__);
    return;
  }

  qusb_log(DCI_HANDLE_USB_RESET_LOG, 0, dcd_dev->ctrl_state);

  // If devie was in configured state, treat this as disconnect
  if(dcd_dev->qusb_is_configured)
  {
    qusb_dci_handle_usb_disconnect();
  }

  // Cancel any pending control transfers.
  ep = (dcd_dev->ctrl_state == EP0_STATE_SETUP_TRB) ? 1 : 0;

  for (; ep < DCI_MAX_ENDPOINTS; ep++ )
  {
    for (dir = 0; dir < DCI_MAX_DIRECTION; dir++)
    {
      /* End all of the unfinished transfers */
      if(qusb_trb_ring[ep][dir].urb_ptr != NULL)
      {
        qusb_dci_cancel_transfer(ep, dir);
      }

      /* Clear stall for any non-control halted EPs */
      if((ep != 0) && (qusb_trb_ring[ep][dir].ep_state & QUSB_EP_HALTED))
      {
        qusb_dci_unstall_endpoint(ep, dir);
      }
    }
  }

  qusb_dci_handle_u1_u2_enable(dcd_dev, QUSB_DCI_U1_INIT_ENABLE, FALSE);
  qusb_dci_handle_u1_u2_enable(dcd_dev, QUSB_DCI_U2_INIT_ENABLE, FALSE);
  qusb_dci_handle_u1_u2_enable(dcd_dev, QUSB_DCI_U1_U2_ACCPET_ENABLE, FALSE);

  /* reset complete callback */
  dci_callback_table_ptr->usb_reset_callback_fn(dcd_dev);
}

// ===========================================================================
/**
 * @function    qusb_dci_handle_connect_done
 * 
 * @brief       This function handles USB Connect Done event.
 * 
 * @details     
 *      
 *      The DCD must perform the following tasks when a connect done  is received:
 *       1. Read DSTS register to obtain the connection speed
 *       2. Program the RAMClkSel field to select the correct clock for the RAM clock domain. This field is
 *           reset to 0 after USB reset, so it must be reprogrammed each time on Connect Done.
 *       3. Issue a DEPCFG command (with Config Action set to “Modify”) for physical endpoints 0 & 1 with a max packet size
 *           based on speed.
 *       4. Depending on the connected speed, write to the other PHY’s control register to suspend it. 
 *       5. (optional) Based on the new MaxPacketSize of IN endpoint 0, software may choose to re-allocate
 *           the TX FIFO sizes by writing to these registers.
 * 
 * @param None    
 *
 * @return  none.
 *
 * @see qusb_dci_poll_events
 *
 * @ref SNPS spec 
 *       Chapter 8.1.3 initialization on Connect Done
 */
// ===========================================================================
static void qusb_dci_handle_connect_done(void)
{
  uint8 ep0, dir;   
  qusb_ep_cmd_t ep_cmd;
  struct qusb_dcd_dsc_device *dcd_dev = dci_core_info[DCI_SNPS_CORE].dcd_context_ptr;

  if (NULL == dcd_dev)
  {
    qusb_error_log(DCI_NULL_POINTER_ERROR_LOG, 0, __LINE__);
    return;
  }

  // Read DSTS to determine the speed
  dcd_dev->speed = (qusb_device_connect_speed_t)HWIO_USB3_PRI_DSTS_INM(HWIO_USB3_PRI_DSTS_CONNECTSPD_BMSK);

  qusb_log(DCI_HANDLE_CONNECT_DONE_LOG, 0, dcd_dev->ctrl_state);

  if( dcd_dev->speed > QUSB_SUPER_SPEED)
  {
    qusb_error_log(DCI_UNSUPPORTED_SPEED_LOG, 0, dcd_dev->speed);
  }

  //  Program RAMClkSel
  HWIO_USB3_PRI_GCTL_OUTM(HWIO_USB3_PRI_GCTL_RAMCLKSEL_BMSK, 0x0 << HWIO_USB3_PRI_GCTL_RAMCLKSEL_SHFT); 

  //Initialize and configure  EP0 info as is from qusb_dcd_core_init()  but with a Max packet size based on the " speed "
  ep0 = DCI_EP0;
  for (dir =0 ; dir < DCI_MAX_DIRECTION; dir++) 
  {

    memset(&ep_cmd, 0, sizeof(ep_cmd));

    //Set EP config (SNPS 6.3.2.5.1 , Table 6-69)
    ep_cmd.set_ep_config.cmd.cmd_typ = QUSB_EP_CMD_SET_EP_CONFIG_VAL;
    ep_cmd.set_ep_config.cmd.cmd_act = QUSB_EXECUTE_EP_GENERIC_COMMAND;

    ep_cmd.set_ep_config.usb_ep_num =
      (ep0 << QUSB_EP_CMD_SET_EP_CONFIG_USB_ENDPT_NUM_SHIFT) |
        (dir << QUSB_EP_CMD_SET_EP_CONFIG_USB_ENDPT_DIR_SHIFT);
    ep_cmd.set_ep_config.fifo_num  = QUSB_EP_FIFO_NUMBER; 
    ep_cmd.set_ep_config.xfer_nrdy_en = QUSB_ENABLE_XFER_NOT_READY;
    ep_cmd.set_ep_config.xfer_cmpl_en = QUSB_ENABLE_XFER_COMPLETE;
    if(dcd_dev->speed == QUSB_SUPER_SPEED)
    {
      ep_cmd.set_ep_config.max_packet_sz = QUSB_EP0_MAX_PACKET_SIZE_SUPER;
    } 
    else if (dcd_dev->speed == QUSB_LOW_SPEED)
    {
      ep_cmd.set_ep_config.max_packet_sz = QUSB_EP0_MAX_PACKET_SIZE_LOW; 
    }
    else
    {
      ep_cmd.set_ep_config.max_packet_sz = QUSB_EP0_MAX_PACKET_SIZE_HIGH;
    }
    ep_cmd.set_ep_config.brst_siz = QUSB_BURST_SIZE_1;
    ep_cmd.set_ep_config.ep_type = QUSB_EP_TYPE_CONTROL;
    ep_cmd.set_ep_config.intr_num = QUSB_DEV_INTR_LINE;
    ep_cmd.set_ep_config.config_action = QUSB_EP_CMD_SET_EP_CONFIG_CONFIG_ACT_MODIFY;

    //Push the EP config 
    if(DCI_OK != qusb_dci_write_ep_cmd(ep0, dir, &ep_cmd, NULL))
    {
      qusb_error_log(DCI_HANDLE_CONNECT_DONE_WRITE_EP_CMD_ERROR_LOG, 0, 0);
    }
  }  // end for(dir<DCI_MAX_DIRECTION)


  if (dcd_dev->speed == QUSB_SUPER_SPEED)
  {
    HWIO_USB3_PRI_SS_PHY_CTRL_OUTM(HWIO_USB3_PRI_SS_PHY_CTRL_LANE0_PWR_PRESENT_BMSK,
                    (uint32 )0x0 << HWIO_USB3_PRI_SS_PHY_CTRL_LANE0_PWR_PRESENT_SHFT);
  }

  //Notify the client
  dcd_dev->is_attached = TRUE;

  /* If any pending control transfer stall state machine to recover */
  if (dcd_dev->ctrl_state != EP0_STATE_SETUP_TRB)
  {
    qusb_uart_log("recover ctrl state machine.", 0);
    qusb_error_log(DCI_CTRL_SM_ERROR_LOG, 0, dcd_dev->ctrl_state);
    qusb_dci_stall_endpoint(0, DIR_RX);
  }

  // Notify DCD layer about port status change.
  qusb_dcd_port_status_changed(dcd_dev);

}

// ===========================================================================
/**
 * @function    qusb_dci_handle_usb_disconnect
 * 
 * @brief       This function handles USB device disconnect
 * 
 * @details     When the application receives a Disconnect event,
 *                  it must set DCTL[8:5] to 5. Other than this, the core does not require any initialization. 
 * 
 * @param       None    
 *
 * @return  none.
 *
 * @see qusb_dci_poll_events
 *
 * @ref SNPS spec 
 *       Chapter 8.1.7 ( pg. 566)
 */
// ===========================================================================
static void qusb_dci_handle_usb_disconnect(void)
{
#ifdef FEATURE_QUSB_USBCV_MODE
  qusb_uart_log("usbcv_mode_ignore_disc", 0);
#else
  uint8  ep, dir;
  struct qusb_dcd_dsc_device *dcd_dev = dci_core_info[DCI_SNPS_CORE].dcd_context_ptr;
  qusb_log(DCI_HANDLE_USB_DISCONNECT_LOG, 0, 0x0);

  if (NULL == dcd_dev)
  {
    qusb_error_log(DCI_NULL_POINTER_ERROR_LOG, 0, __LINE__);
    return;
  }

  if(dcd_dev->speed == QUSB_SUPER_SPEED )
  {
    HWIO_USB3_PRI_DCTL_OUTM(HWIO_USB3_PRI_DCTL_ULSTCHNGREQ_BMSK, LS_RX_DET << HWIO_USB3_PRI_DCTL_ULSTCHNGREQ_SHFT);
  }

  for (ep = 0; ep < DCI_MAX_ENDPOINTS; ep++ )
  {
    for (dir = 0; dir < DCI_MAX_DIRECTION; dir++)
    {
      /* End all of the unfinished transfers */
      if(qusb_trb_ring[ep][dir].urb_ptr != NULL)
      {
        qusb_dci_cancel_transfer(ep, dir);
      }
    }
  }
  dcd_dev->speed = QUSB_UNKNOWN_SPEED;      
  dcd_dev->is_attached = FALSE;
  dcd_dev->qusb_is_disconnected = TRUE;

  dci_callback_table_ptr->usb_disconnect_callback_fn(dcd_dev);
#endif // FEATURE_QUSB_USBCV_MODE
} 

// ===========================================================================
/**
 * @function    qusb_dci_get_link_state
 * 
 * @brief       This function returns USB Link status.
 * 
 * @details     
 * 
 * @param    
 *
 * @return  qusb_link_state_t
 *
 * @see 
 *
 * @ref SNPS spec 
 *        DSTS ( bits 21-18) , Table 6-60 (pg. 499)
 *       
 */
// ===========================================================================
static qusb_link_state_t qusb_dci_get_link_state(void)
{
  qusb_link_state_t link_state;
  /* Read the DSTS to detrmine the Actual state. */
  link_state = (qusb_link_state_t)(HWIO_USB3_PRI_DSTS_INM(HWIO_USB3_PRI_DSTS_USBLNKST_BMSK) >> HWIO_USB3_PRI_DSTS_USBLNKST_SHFT);
  qusb_log(DCI_GET_LINK_STATE_LOG, 0, link_state);
  return link_state;
}

// ===========================================================================
/**
 * @function    qusb_dci_handle_link_status_change
 * 
 * @brief       This function handles USB Link status changed.
 * 
 * @details     
 * 
 * @param   state - Link State    
 *
 * @return  none.
 *
 * @see qusb_dci_handle_dev_event
 *
 * @ref SNPS spec 
 *       
 */
// ===========================================================================
static void qusb_dci_handle_link_status_change(uint32 state)
{
  struct qusb_dcd_dsc_device *dcd_dev = dci_core_info[DCI_SNPS_CORE].dcd_context_ptr;
  
  if (NULL == dcd_dev)
  {
    qusb_error_log(DCI_NULL_POINTER_ERROR_LOG, 0, __LINE__);
    return;
  }

  //Update the Link State 
  dcd_dev->link_state = (qusb_link_state_t)(state & QUSB_EVT_LINK_STATE);
  qusb_log(DCI_HANDLE_LINK_STATUS_CHANGE_LOG, 0, dcd_dev->link_state);
    
  switch(dcd_dev->link_state)
  {
    case LS_U0:
      if(dcd_dev->is_suspended == TRUE)
      {
        qusb_dci_handle_wakeup();
        // Clear the ULSTCHGEN event interrupts
        HWIO_USB3_PRI_DEVTEN_OUTM(HWIO_USB3_PRI_DEVTEN_ULSTCNGEN_BMSK, 0x0 << HWIO_USB3_PRI_DEVTEN_ULSTCNGEN_SHFT);
      }
    break;

    case LS_SS_DIS:
      if(dcd_dev->is_attached == TRUE)
      {
        qusb_dci_handle_usb_disconnect();
      }
    break;	

    // Other Events are not Handled 
    default:
    break;
  }
}

// ===========================================================================
/**
 * @function    qusb_dci_handle_suspend_entry
 * 
 * @brief       This function handles USB suspend entry event
 * 
 * @details     
 *  This event provides a notification that the link has gone to a suspend state (L2, U3, or L1). This event is
 *  generated when hibernation mode is disabled. The existing Link State Change event (3) provides the same
 *  information, but is generated for every link state change. 
 *
 *  Note: Enable suspend event, Sec. 6.3.1.3 ( set bit 6,  DEVTEN registers )
 * 
 * @param       None    
 *
 * @return  none.
 *
 * @see qusb_dci_handle_dev_event
 *
 * @ref SNPS spec 
 *   Sec. 6.3.1.3 , 
 *   Table 6-60 ( Pg. 500) - DSTS ( bits 21-18)
 *   
 */
// ===========================================================================
void qusb_dci_handle_suspend_entry(void)
{
  qusb_link_state_t link_state;
  struct qusb_dcd_dsc_device *dcd_dev = dci_core_info[DCI_SNPS_CORE].dcd_context_ptr;
  qusb_log(DCI_HANDLE_SUSPEND_ENTRY_LOG, 0, 0x0);
    
  if (NULL == dcd_dev)
  {
    qusb_error_log(DCI_NULL_POINTER_ERROR_LOG, 0, __LINE__);
    return;
  }
  /* Read the DSTS to detrmine the Actual state 
  *Link state event also gives the state of the Link 
  */
  link_state = qusb_dci_get_link_state();
  dcd_dev->link_state = link_state;

  if(link_state == LS_U3)
  {
    dcd_dev->is_suspended = TRUE;

    if ( dcd_dev->remote_wakeup_enabled == TRUE)
    {
      dci_callback_table_ptr->usb_suspend_callback_fn(dcd_dev); 

      /* Set the wakeup cookie for SBL to drive resume, in case we miss the Resume/Wakeup in PBL*/
      *(uint32 *)QUSB_REMOTE_WAKEUP_COOKIE = QUSB_WAKEUP_COOKIE_VAL;

      // Set the ULSTCHGEN event interrupts
      HWIO_USB3_PRI_DEVTEN_OUTM(HWIO_USB3_PRI_DEVTEN_ULSTCNGEN_BMSK, 0x1 << HWIO_USB3_PRI_DEVTEN_ULSTCNGEN_SHFT);

      /* Triggering Remote wakeup */
      qusb_dci_delay_ms(100);
      HWIO_USB3_PRI_DCTL_OUTM(HWIO_USB3_PRI_DCTL_ULSTCHNGREQ_BMSK, LS_RECOV << HWIO_USB3_PRI_DCTL_ULSTCHNGREQ_SHFT);
      qusb_dci_delay_ms(10);
    }   

    // If we are suspended without enumeration, user disconnect will try to renumerate 
    //  which is not supported at the moment
    //

  }
}

// ===========================================================================
/**
 * @function    qusb_dci_handle_wakeup
 * 
 * @brief       This function handles wakeup event
 * 
 * @details     
 *  This event provides a notification that when there is a resume on the bus . The existing Link State moves to Active.
 *
 *  Note: 
 * 
 * @param       None    
 *
 * @return  none.
 *
 * @see dci_handle_dev_event
 *
 * @ref SNPS spec 
 *   Sec. 6.3.1.3 , 
 *   Table 6-60 ( Pg. 500) - DSTS ( bits 21-18)
 *   
 */
// ===========================================================================
static void qusb_dci_handle_wakeup(void)
{
  struct qusb_dcd_dsc_device *dcd_dev = dci_core_info[DCI_SNPS_CORE].dcd_context_ptr;
  qusb_log(DCI_HANDLE_WAKEUP_LOG, 0, 0x0);

  if (NULL == dcd_dev)
  {
    qusb_error_log(DCI_NULL_POINTER_ERROR_LOG, 0, __LINE__);
    return;
  }

  /* Read the DSTS to detrmine the Actual state.
   * Link state event also gives the state of the Link
   */
  if(dcd_dev->link_state == LS_U3)
  {
    dcd_dev->link_state = LS_U0;
  }

  /* Clear the wakeup cookie */
  *(uint32 *)QUSB_REMOTE_WAKEUP_COOKIE = 0x0;

  dcd_dev->is_suspended = FALSE;
} 
// ===========================================================================
/**
 * @function    qusb_dci_get_address
 * 
 * @brief   Read adress of the device from register.
 * 
 * @note    Function to read set address from device register.
 *  
 * @param   None.
 *
 */
// ===========================================================================
uint8 qusb_dci_get_address(void)
{  
  return (HWIO_USB3_PRI_DCFG_INM(HWIO_USB3_PRI_DCFG_DEVADDR_BMSK) >> HWIO_USB3_PRI_DCFG_DEVADDR_SHFT);
}
// ===========================================================================
/**
 * @function    qusb_dci_set_address
 * 
 * @brief       This function set the device address
 * 
 * @details     
 *       
 * @param address
 *      New Device Address.
 * 
 * @return  none.
 *
 * @ref 
 */
// ===========================================================================
void qusb_dci_set_address(uint8 address)
{
  qusb_log(DCI_SET_ADDRESS_LOG, 0, address);
  HWIO_USB3_PRI_DCFG_OUTM(HWIO_USB3_PRI_DCFG_DEVADDR_BMSK, address << HWIO_USB3_PRI_DCFG_DEVADDR_SHFT);
}
// ===========================================================================
/**
 * @function    qusb_dci_stall_endpoint
 * 
 * @brief       This function stall an endpoint
 * 
 * @details     
 *      There are two occasions where the device controller may need to return to the host a STALL.
 *  
 *      The first occasion is the FUNCTIONAL stall, 
 *      which is a condition set by the DCD as described in the USB 2.0 device framework (chapter 9). 
 *      A functional stall is only used on noncontrol endpoints and can be enabled in the device
 *      controller by setting the endpoint stall bit in the ENDPTCTRLx register 
 *      associated with the given endpoint and the given direction. 
 *      
 *      In a functional stall condition, the device controller will continue to return STALL responses to all
 *      transactions occurring on the respective endpoint and direction until the endpoint stall bit is cleared by the DCD.
 *      
 *      A PROTOCOL stall, unlike a function stall, is used on control endpoints is automatically cleared by the device
 *      controller at the start of a new control transaction (setup phase). 
 *      When enabling a protocol stall, the DCD should enable the stall bits (both directions) as a pair. 
 *
 *  
 * @param ep    
 *          endpoint number
 * 
 * @param dir    
 *          endpoint direction (1=TX=IN,0=RX=OUT)
 * 
 * @return status
 *              0 = ok , otherwise error.
 * 
 * @ref SNPS spec 
 *   Chapter 8.2.7 Page 579
 *   Chapter 6.3.2.5.4 Page 512
 */
// ===========================================================================
uint32 qusb_dci_stall_endpoint(uint8 ep, uint8 dir)
{
  uint32 status=DCI_OK;
  qusb_ep_cmd_t ep_cmd;

  struct qusb_dcd_dsc_device *dcd_dev = dci_core_info[DCI_SNPS_CORE].dcd_context_ptr;

  qusb_log(DCI_STALL_ENDPOINT_LOG, 0, ep);
  if (NULL == dcd_dev)
  {
    qusb_error_log(DCI_NULL_POINTER_ERROR_LOG, 0, __LINE__);
    return(DCI_ERROR);
  }
  if((ep >= DCI_MAX_ENDPOINTS) || (dir >= DCI_MAX_DIRECTION))
  {
    qusb_error_log(DCI_MEMORY_OUT_OF_BOUNDS_ERROR_LOG, 0, __LINE__);
    return(DCI_ERROR);
  }
  memset(&ep_cmd, 0, sizeof(ep_cmd));

  ep_cmd.set_stall.cmd.cmd_typ = QUSB_EP_CMD_SET_STALL_VAL;
  ep_cmd.set_ep_config.cmd.cmd_act = QUSB_EXECUTE_EP_GENERIC_COMMAND;

  status = qusb_dci_write_ep_cmd(ep, dir, &ep_cmd, NULL); 
  qusb_trb_ring[ep][dir].ep_state |= QUSB_EP_HALTED;

  /*Go for next setup */
  if(ep == 0)
  {
    qusb_dci_ep0_start_xfer(dcd_dev);
  }

  return(status);

}

// ===========================================================================
/**
 * @function    qusb_dci_unstall_endpoint
 * 
 * @brief       This function unstall a stalled endpoint
 * 
 * @details
 *      There are two occasions where the device controller may need to return to the host a STALL.
 *  
 *      The first occasion is the FUNCTIONAL stall, 
 *      which is a condition set by the DCD as described in the USB 2.0 device framework (chapter 9). 
 *      A functional stall is only used on noncontrol endpoints and can be enabled in the device
 *      controller by setting the endpoint stall bit in the ENDPTCTRLx register 
 *      associated with the given endpoint and the given direction. 
 *      
 *      In a functional stall condition, the device controller will continue to return STALL responses to all
 *      transactions occurring on the respective endpoint and direction until the endpoint stall bit is cleared by the DCD.
 *      
 *      A PROTOCOL stall, unlike a function stall, is used on control endpoints is automatically cleared by the device
 *      controller at the start of a new control transaction (setup phase). 
 *      When enabling a protocol stall, the DCD should enable the stall bits (both directions) as a pair. 
 *      A single write to the ENDPTCTRLx register can ensure that both stall bits are set at the same instant.
 *  
 * @note : Any write to the ENDPTCTRLx register during operational mode must preserve the endpoint type field (i.e. perform a readmodifywrite).
 *  
 * @note : Reset Data-Toggle as mentioned in Chapter 8.3.3.1 Data Toggle Reset
 *    
 *
 * @param ep    
 *          endpoint number
 * 
 * @param dir    
 *          endpoint direction (1=TX=IN,0=RX=OUT)
 * 
 * @return status
 *              0 = ok , otherwise error.
 * 
 * @see qusb_dci_stall_endpoint
 *
 * @ref SNPS spec 
 *   Chapter 8.2.7 Page 579
 *   Chapter 6.3.2.5.4 Page 512
 */
// ===========================================================================
uint32 qusb_dci_unstall_endpoint(uint8 ep, uint8 dir)
{
  uint32 status=DCI_OK;
  qusb_ep_cmd_t ep_cmd;
  qusb_log(DCI_UNSTALL_ENDPOINT_LOG, 0, ep);
  if((ep < DCI_MAX_ENDPOINTS) && (dir < DCI_MAX_DIRECTION))
  {
    memset(&ep_cmd, 0, sizeof(ep_cmd));
    ep_cmd.clear_stall.cmd.cmd_typ = QUSB_EP_CMD_CLEAR_STALL_VAL;
    ep_cmd.set_ep_config.cmd.cmd_act = 1;
    status = qusb_dci_write_ep_cmd(ep, dir, &ep_cmd, NULL);
    qusb_trb_ring[ep][dir].ep_state &= ~QUSB_EP_HALTED;
  }
  else
  {
    qusb_error_log(DCI_MEMORY_OUT_OF_BOUNDS_ERROR_LOG, 0, __LINE__);
    status = DCI_ERROR; 
  }
  return(status);
}

// ===========================================================================
/**
 * @function    qusb_dci_config_endpoint
 * 
 * @brief       This function configures the endpoint
 * 
 * @details     
 * 
 * @note        This function should be called on "Set Configuration" USB Standard Command.
 * 
 *
 * @param ep    
 *          endpoint number
 * @param dir    
 *          endpoint direction (1=TX,0=RX)
 * @param ep_type 
 *          endpoint type:    
 *          QUSB_EP_TYPE_CONTROL      0
 *          QUSB_EP_TYPE_ISO          1
 *          QUSB_EP_TYPE_BULK         2    
 *          QUSB_EP_TYPE_INTERRUPT    3
 * @param max_packet_length    
 *          maximum packet length - depends on the connection speed and the Hardware FIFOs.
 * @param zero_length_transfer    
 *          if set to TRUE, and the transfer length is a multiply of the max_packet_length
 *          then a zero lenfth packet is added.
 * 
 * @return None
 * 
 * @ref SNPS spec
 *       
 */
// ===========================================================================
void qusb_dci_config_endpoint(uint8 ep, uint8 dir, 
                                  uint8 ep_type, uint16 max_packet_length, 
                                  boolean zero_length_transfer)
{
  uint32 val;
  qusb_ep_cmd_t ep_cmd;

  memset(&ep_cmd, 0, sizeof(ep_cmd));

  ep_cmd.start_new_config.cmd.cmd_typ = QUSB_EP_CMD_SET_EP_CONFIG_VAL;
  ep_cmd.start_new_config.cmd.cmd_act = 1 ;
  ep_cmd.set_ep_config.usb_ep_num = 
  (ep << QUSB_EP_CMD_SET_EP_CONFIG_USB_ENDPT_NUM_SHIFT) |
  (dir << QUSB_EP_CMD_SET_EP_CONFIG_USB_ENDPT_DIR_SHIFT);

  /*FIFO cfg is only needed for IN EPs. SNPS Table 6-69: DEPCFG */
  if (dir == DIR_TX)
  {
    ep_cmd.set_ep_config.fifo_num  = ep;
  }   
  ep_cmd.set_ep_config.xfer_nrdy_en = QUSB_DISABLE_XFER_NOT_READY;
  ep_cmd.set_ep_config.xfer_in_progess= 1;
  ep_cmd.set_ep_config.xfer_cmpl_en = QUSB_ENABLE_XFER_COMPLETE;
  ep_cmd.set_ep_config.max_packet_sz = max_packet_length;
  
  ep_cmd.set_ep_config.brst_siz = (dir == DIR_TX)? QUSB_BULK_IN_BURST_SIZE : QUSB_BULK_OUT_BURST_SIZE;
  ep_cmd.set_ep_config.ep_type = ep_type;
  ep_cmd.set_ep_config.intr_num = QUSB_DEV_INTR_LINE;

  /* Push the EP config */
  if (DCI_OK != qusb_dci_write_ep_cmd(ep, dir, &ep_cmd, NULL))
  {
    qusb_error_log(DCI_CONFIG_EP_WRITE_EP_CMD_ERROR_LOG, 0, 0);
  }

  memset(&ep_cmd, 0, sizeof(ep_cmd));  

  /*Config EP xfer resorces  (SNPS 6.3.2.5.2 , Table 6-70) */
  ep_cmd.set_ep_xfer_rsc_config.cmd.cmd_typ = QUSB_EP_CMD_SET_EP_XFER_RSC_CONFIG_VAL;
  ep_cmd.set_ep_xfer_rsc_config.cmd.cmd_act = QUSB_EXECUTE_EP_GENERIC_COMMAND;
  ep_cmd.set_ep_xfer_rsc_config.num_xfer_res = 1;

  /* Push the EP cmd */
  if (DCI_OK != qusb_dci_write_ep_cmd(ep, dir, &ep_cmd, NULL))
  {
    qusb_error_log(DCI_CONFIG_EP_WRITE_EP_CMD_ERROR_LOG, 0, 0);
  }

  /* Enable EP */
  val = HWIO_USB3_PRI_DALEPENA_IN;
  val |= (1 << QUSB_EP_INDEX(ep, dir));
  HWIO_USB3_PRI_DALEPENA_OUT(val);

  return;
}

// ===========================================================================
/**
 * @function    qusb_dci_unconfig_endpoint
 * 
 * @brief       This function de-configures the endpoint
 * 
 * @details     
 * 
 * @note        This function should be called on "Set Configuration" USB Standard Command.
 * 
 * @note        caution: if one endpoint direction is enabled and the paired endpoint of opposite
 *              direction is disabled
 *              then the unused direction type must be changed from the default controltype
 *              to any other type (ie. bulktype).
 *              leaving an unconfigured endpoint control will cause undefined behavior 
 *              for the data pid tracking on the active endpoint/direction.
 * 
 * @param ep    
 *          endpoint number
 * 
 * @param dir    
 *          endpoint direction (1=TX,0=RX)
 * 
 * @return status
 *              0 = ok , otherwise error.
 * 
 * @see qusb_dci_config_endpoint
 * 
 * @ref SNPS spec 
 *        
 */
// ===========================================================================
uint32 qusb_dci_unconfig_endpoint( uint8 ep, uint8 dir)
{
  uint32 status=DCI_OK;
  uint32 val;
  /* Disable EP  */
  val = HWIO_USB3_PRI_DALEPENA_IN;
  val &= ~(1 << QUSB_EP_INDEX(ep, dir));
  HWIO_USB3_PRI_DALEPENA_OUT(val);

  return status;

}
// ===========================================================================
/**
 * @function    qusb_dci_submit_trb_data
 * 
 * @brief       This function initate rx/tx dir data transfer for an EP
 * 
 * @details     
 * 
 * @param ep    
 *          endpoint number
 * @param dir    
 *          endpoint direction (1=TX,0=RX)
 *
 * @return status
 *              0 = ok , otherwise error.
 * 
 * @ref SNPS spec 
 *
 * */  
// ===========================================================================
static uint32 qusb_dci_submit_trb_data(uint8 ep, uint8 dir)
{
  qusb_ep_cmd_t start_cmd;
  qusb_urb_t *urb_ptr = qusb_trb_ring[ep][dir].urb_ptr;

  if(NULL == urb_ptr)
  {
    qusb_error_log(DCI_NULL_POINTER_ERROR_LOG, 0, __LINE__);
    return (DCI_ERROR);
  }

  memset(&start_cmd, 0, sizeof(start_cmd));

  /* Execute a start transfer command on the endpoint */
  /* TRB ring for the transfer should have been initialized. */
  start_cmd.start_xfer.td_address_lo  = (uint64)qusb_trb_ring[ep][dir].buf & 0xFFFFFFFF;
  start_cmd.start_xfer.td_address_hi  = (uint64)qusb_trb_ring[ep][dir].buf >> 32 & 0xFFFFFFFF;
  start_cmd.start_xfer.cmd.cmd_typ   = QUSB_EP_CMD_START_XFER_VAL;
  // Disable EP_CMD_COMPLTE Interrupt.
  start_cmd.start_xfer.cmd.cmd_ioc   = 0;
  start_cmd.start_xfer.cmd.cmd_act   = QUSB_EXECUTE_EP_GENERIC_COMMAND;
  start_cmd.start_xfer.cmd.cmd_param = 0;

  if(DCI_OK != qusb_dci_write_ep_cmd(ep, dir, &start_cmd, &(urb_ptr->transfer_index)))
  {
    qusb_error_log(DCI_SUBMIT_TRB_DATA_WRITE_EP_CMD_ERROR_LOG, 0, 0);
    return (DCI_ERROR);
  }

  return(DCI_OK);
}
//============================================================================
/**
 * @function  qusb_dci_prepare_trbs
 *
 * @brief
 *    This function initializes the TRB structures for an EP:
 *    
 *  @Note : 
 *      
 *
 * @param ep
 *            EP number
 * @param dir  
 *            EP direction 
 * @param trb_cntrl
 *            Type of TRB 
 * @param buf_addr
 *            data buffer supplied by upper laer
 * @param byte_count
 *              Number of bytes 
 * @param send_zlp
 *            whether zlp should be sent
 *
 * @return None
 *              
 *
 * @ref SNPS spec "":
 *      Chapter 7.1
 *      Chapter 8.2.3
 *      
 *
 */
//============================================================================
static void qusb_dci_prepare_trbs(uint32 ep, uint32 dir, uint64 buf_addr, uint32 byte_count, uint8 send_zlp, uint8 trb_cntrl)
{
  uint32  xfer_len = byte_count;
  uint32  total_bytes_for_TRB;
  qusb_trb_t *trb;
  qusb_trb_ring_t *trb_ring;
  uint32 i = 0 ;

  if((ep >= DCI_MAX_ENDPOINTS) || (dir >= DCI_MAX_DIRECTION))
  {
    qusb_error_log(DCI_MEMORY_OUT_OF_BOUNDS_ERROR_LOG, 0, __LINE__);
    return;
  }

  trb_ring = &qusb_trb_ring[ep][dir];

  /* Initialize TRB ring */
  trb_ring->buf = (void *) &qusb_trb_list[ep][dir][0];
  trb_ring->buf_size = DCI_MAX_TRBS * sizeof(qusb_trb_t);

  /* Get the first TRB */
  trb= (qusb_trb_t *)trb_ring->buf;

  /* Flush/Invalidate data buffer on TX/RX for Bulk EP */
  if( ep > 0 && (ep < DCI_MAX_ENDPOINTS))
  {
    switch(dir)
    {
      case DIR_RX:
      {
        qusb_dci_dcache_inval((void*)buf_addr, byte_count);
      }
      break;

      case DIR_TX:
      {
        qusb_dci_dcache_flush((void*)buf_addr, byte_count);
      }
      break;

      default:
        qusb_error_log(DCI_DEFAULT_CASE_LOG, 0, __LINE__);
    }
  }

  /* Normal TRB  */
  do
  {
    total_bytes_for_TRB = MIN (byte_count, QUSB_TRB_MAX_BUFFER_SIZE);
    memset(trb, 0, sizeof(qusb_trb_t));

    if(trb_cntrl == QUSB_TRB_CTL_CONTROL_SETUP)
    {
      trb->buf_ptr_lo = (uint64)(trb_ring->buf)  & 0xFFFFFFFF;
      trb->buf_ptr_hi = ((uint64)(trb_ring->buf)  >> 32) & 0xFFFFFFFF;
    }
    else
    {
      // For DLOAD mode zero is a valid buffer address.
      trb->buf_ptr_lo = (uint32)(buf_addr & 0xFFFFFFFF);
      trb->buf_ptr_hi = (uint32)((buf_addr >> 32) & 0xFFFFFFFF);
    }

    /* For Rx, we always queue a TRB with max packet size, if size is less than max packet */
    if(trb_cntrl == QUSB_TRB_CTL_NORMAL)
    {
      if((dir == DIR_RX) && (total_bytes_for_TRB < qusb_dci_max_packet_size))
      {
        trb->buf_size = qusb_dci_max_packet_size;
      }
      else
      {
        trb->buf_size = total_bytes_for_TRB;
      }
    } 
    else
    {
      trb->buf_size   = total_bytes_for_TRB;
    }

    /* Assuming the EP0 Rx Data in DATA stage will never be beyond the Max packet size */
    if((trb_cntrl == QUSB_TRB_CTL_CONTROL_DATA) && (dir == DIR_RX))
    {
      trb->buf_size = qusb_dci_ep0_max_packet_size;
    }

    trb->pcm1       = 0;
    trb->rsvd0      = 0;
    trb->trbsts     = 0;
    trb->hwo        = 1;
    trb->lst        = 0;
    trb->chn        = 1;
    trb->csp        = 0;
    trb->trb_ctl    = trb_cntrl;
    trb->ioc        = 0;
    trb->isp_imi    = 1;
    trb->rsvd1      = 0;
    trb->sid_sofn   = 0;
    trb->rsvd2      = 0;

    byte_count -= total_bytes_for_TRB; /*total_bytes_for_TRB < byte_count*/
    buf_addr += total_bytes_for_TRB;   /*size check is done before passing the buffer and byte count*/ 
                    /*becomes 0x0 */

    i++;

  }while ( byte_count > 0 && i < DCI_MAX_TRBS);

  /* Zero length TRB, Append an ZLP if byte_count is a multiple of max packet length or  ZLP for EP0 status stage */
  if((send_zlp == 1) && ((((xfer_len % qusb_dci_get_max_packet_size()) == 0) && (xfer_len != 0))
         ||((ep == 0) && ( dir == DIR_TX)&& ((xfer_len % qusb_dci_ep0_max_packet_size) == 0))))
  {
    trb = (qusb_trb_t *) &qusb_trb_list[ep][dir][i];
    memset(trb, 0, sizeof(qusb_trb_t));

    trb->buf_ptr_lo = 0;
    trb->buf_ptr_hi = 0;
    trb->buf_size  = 0;
    trb->pcm1     = 0;
    trb->rsvd0    = 0;
    trb->trbsts   = 0;
    trb->hwo      = 1;
    trb->lst      = 1;
    trb->chn      = 0;
    trb->csp      = 0;
    trb->trb_ctl   = trb_cntrl;
    trb->isp_imi  = 1;
    trb->ioc      = 0;
    trb->rsvd1    = 0;
    trb->sid_sofn = 0;
    trb->rsvd2    = 0;
    qusb_log(DCI_PREPARE_TRB_ZLP_LOG, 0, i);
  }

  /*  Trb now points to the last TRB created above, set appropriate data for last TRB of buffer descriptor */
  trb->chn = 0;
  trb->lst = 1;
  trb->ioc = 1;

}

// ===========================================================================
/**
 * @function    qusb_dci_transfer_data_request
 * 
 * @brief       This function queues rx/dir data transfer
 * 
 * @details     
 * 
 *  The behaviors of the device controller for interrupt and bulk endpoints are identical. 
 *  All valid IN and OUT transactions to bulk pipes will handshake with a NAK unless the endpoint had been primed. 
 *  Once the endpoint has been primed, data delivery will commence.
 *  
 * @param ep    
 *          endpoint number
 * @param dir    
 *          endpoint direction (1=TX,0=RX)
 * @param buffer_ptr    
 *          pointer to buffer
 * @param byte_count    
 *          number of bytes to send (dir) 
 *          or
 *          maximum number of bytes to receive (rx) 
 * @param send_zlp    
 *          specify whether to transmit a ZLP at the end of the transfer.
  * @param trb_ctrl    
 *          type of the TRB ( SNPS pg. 547 table 7.1, bits 9:4).
 *
 * @return status
 *              0 = ok , otherwise error.
 * 
 * @ref SNPS spec 
 *
 * */  
// ===========================================================================
static uint32 qusb_dci_transfer_data_request(uint8 ep, uint8 dir,void* buffer_ptr,uint32 byte_count, uint8 send_zlp, trb_ctl_type_t trb_ctrl)
{
  (ep == 0) ? qusb_bus_log(DCI_TRANSFER_DATA_REQUEST_LOG, 0, byte_count) : qusb_log(DCI_TRANSFER_DATA_REQUEST_LOG, 0, byte_count);
  /* max transfer to be less than to fit in num of TRBs */
  if (byte_count > (DCI_MAX_TRBS * QUSB_TRB_MAX_BUFFER_SIZE)) 
  {
    qusb_error_log(DCI_TRANSFER_SIZE_TOO_LARGE_LOG, 0, byte_count);
    return(QUSB_ERR_BAD_PARAM);
  }
  if((ep >= DCI_MAX_ENDPOINTS) || (dir >= DCI_MAX_DIRECTION))
  {
    qusb_error_log(DCI_MEMORY_OUT_OF_BOUNDS_ERROR_LOG, 0, __LINE__);
    return(QUSB_ERR_BAD_PARAM);
  }

  // buffer can be NULL for SETUP on CONTROL EP
  // i.e. qusb_dcd_control_transfer(dcd_dev, DIR_RX, NULL, DCI_SETUP_BUFFER_SIZE);     

  /*  Intialize TRB ring and update the TRB structure with the data */
  qusb_dci_prepare_trbs(ep, dir, (uint64)buffer_ptr, byte_count, send_zlp, trb_ctrl);

  ArmDataMemoryBarrier();

  /* Submit the TRB for setup or normal */
  /* For all the other control requests, we submit to the HW , after xfer_nrdy  is received */
  if ((trb_ctrl == QUSB_TRB_CTL_CONTROL_SETUP) || (trb_ctrl == QUSB_TRB_CTL_NORMAL))
  {
    /* push the TRB into the HW */
    qusb_dci_submit_trb_data(ep, dir);
  }
  if((uint64)buffer_ptr == 0x0B000000)
  {
    qusb_uart_log("start PIMEM dump, size", byte_count);
  }
  return(DCI_OK);
}

// ===========================================================================
/**
 * @function    qusb_dci_ctrl_xfer_state_machine
 * 
 * @brief       This function updates the state of the control transfer and TRB  
 *              type that should be programmed while initiating a control 
 *              transfer.
 * 
 * @details     
 *              
 * 
 * @note    
 *
 * @param urb_ptr
 *              A pointer to URB structure.
 * @param urb_ptr
 *              pointer to TRB control state.
 *
 * 
 * @return status
 *              0 = ok , otherwise error.
 * 
 * @see transfer_data_request
 * 
 */
// ==========================================================================
static void qusb_dci_ctrl_xfer_state_machine(qusb_urb_t *urb_ptr, trb_ctl_type_t *trb_ctl)
{
   
    if ((NULL == urb_ptr) || (NULL == trb_ctl))
    {
        qusb_error_log(DCI_NULL_POINTER_ERROR_LOG, 0, __LINE__);
        return;
    }     

  switch (urb_ptr->usb_device->ctrl_state)
  {
    case EP0_STATE_SETUP_TRB:
      *trb_ctl = QUSB_TRB_CTL_CONTROL_SETUP;
    break;

    case EP0_STATE_INTRPT_SETUP:

      if (urb_ptr->transfer_length > 0 )
      {
        urb_ptr->usb_device->ctrl_state = EP0_STATE_DATA;
        *trb_ctl = QUSB_TRB_CTL_CONTROL_DATA;
      }
      else
      {
        urb_ptr->usb_device->ctrl_state = EP0_STATE_STATUS_2;
        *trb_ctl = QUSB_TRB_CTL_CONTROL_STATUS_2;
      }     
    break;

    case EP0_STATE_STATUS_2:
      *trb_ctl = QUSB_TRB_CTL_CONTROL_STATUS_2;
    break;

    case EP0_STATE_DATA:
      *trb_ctl = QUSB_TRB_CTL_CONTROL_STATUS_3;
    break;

    case EP0_STATE_STATUS_3:
      *trb_ctl = QUSB_TRB_CTL_CONTROL_STATUS_3;
    break;

    default:
      // No action needed
      break;
  }
}

// ===========================================================================
/**
 * @function    qusb_dci_submit_request
 * 
 * @brief       This function initiates data transfer according to URB.
 * 
 * @details     
 *              This function calls submit_trb_data() and updates the URB.
 * 
 * @note        The USB is a Master-Slave protocol where the Device is the Slave.
 *              The Device's data is actually sent/received on the USB bus 
 *              only when the Host sends SETUP/IN/OUT token.
 *              Therefore, the caller only starts a REQUEST.
 *              A callback is called upon complete/error detection.
 *              The caller May poll the status in the URB.
 * 
 * @param ep
 *              ep number.
 
 * @param dir
 *              ep direction.
 * 
 * @return status
 *              0 = ok , otherwise error.
 * 
 * @see transfer_data_request
 * 
 */
// ===========================================================================
static uint32 qusb_dci_submit_request(uint8 ep, uint8 dir)
{
  uint32  status=0;

  qusb_bus_log(DCI_CTRL_XFER_TO_HW_LOG, 0, QUSB_EP_INDEX(ep, dir));

  if((ep >= DCI_MAX_ENDPOINTS) || (dir >= DCI_MAX_DIRECTION))
  {
    qusb_error_log(DCI_MEMORY_OUT_OF_BOUNDS_ERROR_LOG, 0, __LINE__);
    status = DCI_ERROR; 
    return(status);
  }
  /* push the TRB into the HW */
  status = qusb_dci_submit_trb_data(ep, dir);

  qusb_trb_ring[ep][dir].urb_ptr->transfer_status = QUSB_URB_STATUS_ACTIVE;

  return(status);
}

// ===========================================================================
/**
 * @function    qusb_dci_transfer_request
 * 
 * @brief       This function ques data transfer according to URB.
 * 
 * @details     
 *              This function calls transfer_data_request() and update the URB.
 * 
 * @note        The USB is a Master-Slave protocol where the Device is the Slave.
 *              The Device's data is actually sent/received on the USB bus 
 *              only when the Host send SETUP/IN/OUT token.
 *              Therefore, the caller only start a REQUEST.
 *              A callback is called upon complete/error detection.
 *              The caller May poll the status in the URB.
 * 
 * @param urb_ptr
 *              A pointer to URB structure.
 * 
 * @return status
 *              0 = ok , otherwise error.
 * 
 * @see transfer_data_request
 * 
 */
// ===========================================================================
uint32 qusb_dci_transfer_request(qusb_urb_t* urb_ptr)
{
  uint32  status=0;
  uint8   ep=0;
  boolean dir=FALSE;
  trb_ctl_type_t trb_ctl;

  if (NULL == urb_ptr)
  {
    qusb_error_log(DCI_NULL_POINTER_ERROR_LOG, 0, __LINE__);
    return(DCI_ERROR);
  }

  ep = urb_ptr->endpoint_address & 0x0F;
  dir = VAL_TO_BOOL(urb_ptr->endpoint_address & 0x80);

  if (ep >= DCI_MAX_ENDPOINTS || dir >= DCI_MAX_DIRECTION)
  {
    return(QUSB_ERR_BAD_PARAM);
  }

  /* update the state machine for EP0 and get the TRB type to be sent */
  if (ep == 0)
  {
    qusb_dci_ctrl_xfer_state_machine(urb_ptr, &trb_ctl);

    // Copy TX response buffer to generic ZI buffer
    if(DIR_TX == dir)
    {
      memscpy(&qusb_control_tx_buffer, QUSB_MAX_CTRL_PKT_SIZE,
            urb_ptr->transfer_buffer_ptr, urb_ptr->transfer_length);

      urb_ptr->transfer_buffer_ptr = &qusb_control_tx_buffer;
    }
  }
  else
  {
    trb_ctl = QUSB_TRB_CTL_NORMAL;
  }

  /* Set TRB to point at the URB */
  qusb_trb_ring[ep][dir].urb_ptr = urb_ptr ;

  status = qusb_dci_transfer_data_request(ep, dir, 
                       urb_ptr->transfer_buffer_ptr, 
                       urb_ptr->transfer_length, 
                       urb_ptr->send_zlp, 
                       trb_ctl);

  if ((status == 0) && (ep < DCI_MAX_ENDPOINTS) && (dir < DCI_MAX_DIRECTION))
  {
    /* Mark as queued */
    urb_ptr->transfer_status =  (trb_ctl > QUSB_TRB_CTL_CONTROL_SETUP ) ? 
                                  QUSB_URB_STATUS_QUEUED : QUSB_URB_STATUS_ACTIVE ;
    urb_ptr->actual_length   = 0;

  }
  else
  {
    qusb_trb_ring[ep][dir].urb_ptr = NULL ;
    urb_ptr->transfer_status = QUSB_URB_STATUS_COMPLETE_ERROR;
  }

  return(status);
}

// ===========================================================================
/**
 * @function    qusb_dci_cancel_ep0
 * 
 * @brief       This function terminates an active transfers on EP0
 * 
 * @details     
 * 
 * @see qusb_dci_shutdown
 * 
 * @ref SNPS Spec
 */
// ===========================================================================
uint32 qusb_dci_cancel_ep0(void)
{
  qusb_ep_cmd_t ep_cmd;

  memset(&ep_cmd, 0, sizeof(ep_cmd));
  ep_cmd.end_xfer.cmd.cmd_typ = QUSB_EP_CMD_END_XFER_VAL;
  ep_cmd.set_ep_config.cmd.cmd_act = 1;
  ep_cmd.set_ep_config.cmd.hi_pri_force_rm = 1;
  ep_cmd.set_ep_config.cmd.cmd_param= 0;

  if(DCI_OK != qusb_dci_write_ep_cmd(0, 0, &ep_cmd, NULL))
  {
    qusb_error_log(DCI_CANCEL_TRANSFER_WRITE_EP_CMD_ERROR_LOG, 0, 0);
  }
  return 0;
}

// ===========================================================================
/**
 * @function    qusb_dci_cancel_transfer
 * 
 * @brief       This function terminates an active transfers
 * 
 * @details     
 * 
 * @param core_id    
 *
 * @param ep    
 *          endpoint number
 * @param dir    
 *          endpoint direction (1=TX,0=RX)
 * 
 * @return status
 *              0 = ok , otherwise error.
 * 
 * @see qusb_dci_shutdown
 * 
 * @ref SNPS Spec
 */
// ===========================================================================
uint32 qusb_dci_cancel_transfer(uint8 ep, uint8 dir)
{

  qusb_ep_cmd_t ep_cmd;
  uint8 res_index;
  qusb_urb_t *urb_ptr;

  if((ep >= DCI_MAX_ENDPOINTS) || (dir >= DCI_MAX_DIRECTION))
  {
    qusb_error_log(DCI_MEMORY_OUT_OF_BOUNDS_ERROR_LOG, 0, __LINE__);
    return(QUSB_ERR_BAD_PARAM);
  }

  urb_ptr = qusb_trb_ring[ep][dir].urb_ptr;
  if(NULL == urb_ptr)
  {
    qusb_error_log(DCI_CANCEL_TRANSFER_URB_NULL_ERROR_LOG, 0, 0);
    return DCI_ERROR;
  }

  res_index = urb_ptr->transfer_index;
  qusb_log(DCI_CANCEL_TRANSFER_LOG, 0, ep);
  if((urb_ptr->transfer_status == QUSB_URB_STATUS_ACTIVE)&& (res_index != QUSB_RSC_IDX_NOT_SET))
  {
    memset(&ep_cmd, 0, sizeof(ep_cmd));

    ep_cmd.end_xfer.cmd.cmd_typ = QUSB_EP_CMD_END_XFER_VAL;
    ep_cmd.set_ep_config.cmd.cmd_act = 1;
    ep_cmd.set_ep_config.cmd.hi_pri_force_rm = 1;
    ep_cmd.set_ep_config.cmd.cmd_param= res_index;

    if(DCI_OK != qusb_dci_write_ep_cmd(ep, dir, &ep_cmd, NULL))
    {
      qusb_error_log(DCI_CANCEL_TRANSFER_WRITE_EP_CMD_ERROR_LOG, 0, 0);
    }

    if(ep == 0)
    {
      qusb_log(DCI_CANCEL_TRANSFER_LOG, (1+ep+dir), res_index);
      (dir == DIR_RX)? qusb_uart_log("endxfer EP0 OUT", 0) : qusb_uart_log("endxfer EP0 IN", 0);
    }
    urb_ptr->transfer_status = QUSB_URB_STATUS_COMPLETE_ERROR;
  }
  urb_ptr->transfer_index = QUSB_RSC_IDX_NOT_SET;

  return(DCI_OK);
}

// ===========================================================================
/**
 * @function    qusb_dci_shutdown
 * 
 * @brief       This function cancel any active transfer.
 * 
 * @note        This function is called at the end of the DLOAD program before jumping to ARMPROG.
 *              Since ep#1 rx is still active , it need to be canceled.
 * 
 * @param 
 *
 * @return status
 *              0 = ok , otherwise error.
 * 
 * @see qusb_dci_cancel_transfer
 * 
 * @ref SNPS spec.
 *      Chapter 8.1.8  Device Initiated disconnect
 */
// ===========================================================================
uint32 qusb_dci_shutdown(void)
{
  uint8  ep, dir;
  struct qusb_dcd_dsc_device *dcd_dev = dci_core_info[DCI_SNPS_CORE].dcd_context_ptr;

  qusb_log(DCI_SHUTDOWN_LOG, 0, 0x0);
  for (ep = 0; ep < DCI_MAX_ENDPOINTS; ep++ )
  {
    for (dir = 0; dir < DCI_MAX_DIRECTION; dir++)
    {
      /* End all of the unfinished transfers */
      if(qusb_trb_ring[ep][dir].urb_ptr != NULL)
      {
        qusb_dci_cancel_transfer(ep, dir);
      }
    }
  }

  if(dcd_dev->shutdown_mode == QUSB_SINGLE_ENUM_MODE)
  {
    qusb_dci_write_evt_buff_cur_pos_cookie(qusb_evt_buf_info.cur_pos);
  }

  return(DCI_OK);
}

//============================================================================
/**
 * @function  qusb_dci_get_speed
 *
 * @brief Report the USB connection speed.
 *
 * @Note : Although the device is high-speed, the Host might be full-speed.
 *
 * @Note : The USB HW can be forced to Full-Speed.
 *
 * @param None 
 * @return Speed
 *              QUSB_SPEED_FULL        
 *              QUSB_SPEED_LOW         
 *              QUSB_SPEED_HIGH        
 *              QUSB_SPEED_FULL
 *              QUSB_SPEED_UNKNOWN     
 *
 * @ref SNPS spec 
 *       Chapter 6.3.1.4 DSTS ( bits 0-2)
 *
 */
//============================================================================
uint32 qusb_dci_get_speed(void)
{
  uint32 speed;
  /* Read DSTS to determine the speed */
  speed = (qusb_device_connect_speed_t)HWIO_USB3_PRI_DSTS_INM(HWIO_USB3_PRI_DSTS_CONNECTSPD_BMSK);
  qusb_log(DCI_GET_SPEED_LOG, 0, speed);
  return(speed);
}
//============================================================================
/**
 * @function  qusb_dci_set_max_packet_size
 *
 * @brief Sets the maximum packet size of EP0 to be used
 *
 * @param max_packet_size  
 *
 * @return None  
 *
 * @ref SNPS spec 
 *       
 */
//============================================================================
void qusb_dci_set_max_packet_size(uint32 max_packet_size)
{
  qusb_log(DCI_SET_MAX_PACKET_SIZE_LOG, 0, max_packet_size);
  qusb_dci_max_packet_size = max_packet_size;
  qusb_dci_ep0_max_packet_size =( max_packet_size == 1024 ) ? QUSB_EP0_MAX_PACKET_SIZE_SUPER :
                                                  QUSB_EP0_MAX_PACKET_SIZE_HIGH;
}
//============================================================================
/**
 * @function  qusb_dci_get_max_packet_size
 *
 * @brief Returns the maximum packet size of EP0 to be used
 *
 * @param None 
 *
 * @return uint32 - max_packet_size   
 *
 * @ref SNPS spec 
 *       
 */
//============================================================================
static uint32 qusb_dci_get_max_packet_size(void)
{
  qusb_log(DCI_GET_MAX_PACKET_SIZE_LOG, 0, qusb_dci_max_packet_size);
  return qusb_dci_max_packet_size;
}

// ===========================================================================
/**
 * @function    qusb_dci_handle_set_select
 * 
 * @brief   This function will be used for specifying the system exit latency
 *          in USB. Currently Power management is not supported in SBL driver
 *          so it is safe to set all these values to 0x0. 
 * 
 * @param   None
 * 
 * @return  None
 * 
 */
// ===========================================================================
void qusb_dci_handle_set_select(void)
{
  qusb_dev_cmd_t dev_cmd;	
  qusb_log(DCI_HANDLE_SET_SELECT_LOG, 0, 0x0);
  memset(&dev_cmd, 0, sizeof(dev_cmd));
  dev_cmd.set_periodic_params.cmd.cmd_typ = QUSB_DEV_CMD_SET_PERIODIC_PARAMS_VAL; 
  dev_cmd.set_periodic_params.cmd.cmd_act = 0x1; 
  dev_cmd.set_periodic_params.system_exit_latency = 0x00; 

  qusb_dci_write_dev_cmd(&dev_cmd); 

  return;	
}
// ===========================================================================
/**
 * @function    qusb_dci_write_enum_cookie
 * 
 * @brief   API is used to write the single enumeration Cookie
 * 
 * @note    This API makes a Write to QSRAM register to store the 
 *          Single Enumeration Cookie 
 *  
 * @param   uint32 : The Cookie Value to be written 
 *
 */
// ===========================================================================
void qusb_dci_write_enum_cookie(uint32 val)
{
  *(uint32 *)QUSB_SINGLE_BOOT_ENUM_ADDR = val;
}

// ===========================================================================
/**
 * @function    qusb_dci_read_enum_cookie
 * 
 * @brief   API is used to read the single enumeration Cookie
 * 
 * @note    This API makes a read from QSRAM register to get the 
 *          Single Enumeration Cookie 
 *  
 * @param   uint32 : The Cookie Value to be read 
 *
 */
// ===========================================================================
void qusb_dci_read_enum_cookie(uint32 *cookie)
{
  *cookie = (*(uint32 *)(QUSB_SINGLE_BOOT_ENUM_ADDR));
}
// ===========================================================================
/**
 * @function    qusb_dci_write_evt_buff_cur_pos_cookie
 * 
 * @brief   API is used to write current position of the event buffer
 * 
 * @note    This API makes a write to QSRAM register to save 
 *          current position of event buffer to a Cookie.
 *  
 * @param   uint32 : Cookie Value to write 
 *
 */
// ===========================================================================
void qusb_dci_write_evt_buff_cur_pos_cookie(uint32 val)
{
  *(uint32 *)QUSB_EVT_BUF_CUR_POS_COOKIE = val;
}

// ===========================================================================
/**
 * @function    qusb_dci_read_evt_buff_cur_pos_cookie
 * 
 * @brief   API is used to read current position of the event buffer
 * 
 * @note    This API makes a read from QSRAM register to get the 
 *          current position of event buffer from a Cookie.
 *  
 * @param   uint32 : The Cookie Value to be read 
 *
 */
// ===========================================================================
void qusb_dci_read_evt_buff_cur_pos_cookie(uint32 *cookie)
{
  *cookie = (*(uint32 *)(QUSB_EVT_BUF_CUR_POS_COOKIE));
}
// ===========================================================================
/**
 * @function    dci_skip_enumeration
 * 
 * @brief   Determine whether enumeration in subsequent stages can be skipped
 * 
 * @note    Wrapper function to lookup whether enumeration in subsequent stages can be skipped          
 *  
 * @param   TRUE signifies enumeration is skipped
 *
 */
// ===========================================================================
boolean qusb_dci_skip_enumeration(void)
{

  uint32 data = 0;
  static uint32 qusb_cookie_single_enum = QUSB_COOKIE_SINGLE_ENUM_UNINIT_VALUE;

  if (qusb_cookie_single_enum == QUSB_COOKIE_SINGLE_ENUM_UNINIT_VALUE)
  {
    if( FALSE == qusb_dci_is_usb30_sw_collapsed() )
    {
      /*Read the cookie*/
      qusb_dci_read_enum_cookie(&qusb_cookie_single_enum);

      /*Clear  the cookie */
      qusb_dci_write_enum_cookie(QUSB_COOKIE_NULL_VAL);
    }
    else
    {
      qusb_cookie_single_enum = QUSB_COOKIE_NULL_VAL;
    }

    /*Set the cookie ( for Debug ) */
    //qusb_dci_write_enum_cookie(QUSB_SINGLE_BOOT_ENUM_MAGIC_DL);
  }

  /*assign the cookie to data*/
  data = qusb_cookie_single_enum; 
  if (data == QUSB_SINGLE_BOOT_ENUM_MAGIC_DL)
  {
    qusb_log(DCI_SKIP_ENUMERATION_LOG,0,TRUE);
    qusb_log(SINGLE_BOOT_ENUM_MAGIC_DL_LOG,0,0);
    return TRUE;
  }
  else
  {
    qusb_log(DCI_SKIP_ENUMERATION_LOG,0,FALSE);
    return FALSE;
  }
}
