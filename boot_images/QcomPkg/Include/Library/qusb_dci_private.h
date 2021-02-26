#ifndef QUSB_DCI_PRIVATE_H
#define QUSB_DCI_PRIVATE_H

/*=======================================================================*//**
 * @file        qusb_dci_private.h
 * @author:     amirs
 * @date        14-July-2008
 *
 * @brief       QUSB (Qualcomm High-Speed USB) DCI (Device-Controller-Interface) Private defines, types and API.
 *
 * @details     The Chip-Idea USB Hardware is managed via descriptors called dQH and dTD.
 *              The dQH describes some of the endpoint configuration
 *              The dTD describes the transfer parameters.
 *              
 * @ref         ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core".
 *              Chapter 7 Device Data Structures
 *
 *              Copyright (c) 2008-2010 Qualcomm Technologies Incorporated.
 *              All Rights Reserved.
 *              Qualcomm Confidential and Proprietary
 * 
*//*========================================================================*/

//===========================================================================
// 
//                            EDIT HISTORY FOR FILE
//   This section contains comments describing changes made to the module.
//   Notice that changes are listed in reverse chronological order.
// 
// 
// when       who      what, where, why
// --------   ---      ----------------------------------------------------------
// 07/14/08   amirs    First Draft
// 08/08/08   amirs    First working DLOAD process
// 08/11/08   amirs    Add files and functions headers according to coding standard.
// 08/30/10   dpatnaik Port QUSB to RPM, add ULPI based charger detection
// ===========================================================================

//----------------------------------------------------------------------------
// Include Files
//----------------------------------------------------------------------------
#include "comdef.h"     // common defines and types
#include "qusb_log.h"
#include "qusb_dcd.h"
//----------------------------------------------------------------------------
// Preprocessor Definitions and Constants
//----------------------------------------------------------------------------

//+++++++++++++++++++++++++++++++++
// DCI PRIVATE FLAGS
//+++++++++++++++++++++++++++++++++


#define DCI_ASSERT(val)  qusb_error_log(val, 0, __LINE__);


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

#define DCI_MAX_CORES     1 /**Currently we support only SNPS Core**/
#define DCI_SNPS_CORE     0x0

// DCI Error Codes
#define DCI_OK                      0
#define DCI_ERROR           0xDEADBEEF

#define DCI_SETUP_BUFFER_SIZE   8

#define DCI_MAX_ENDPOINTS 2
#define DCI_MAX_DIRECTION 2  // RX=OUT=0,TX=IN=1

#define QUSB_EP0_MAX_PACKET_SIZE_SUPER        512
#define QUSB_EP0_MAX_PACKET_SIZE_HIGH          64
#define QUSB_EP0_MAX_PACKET_SIZE_FULL          64
#define QUSB_EP0_MAX_PACKET_SIZE_LOW  8

#define QUSB_EP0_MAX_PACKET_SIZE_SUPER_BMAX0  0x9
#define QUSB_EVT_BUFFER_UNIT_SIZE           0x4

//Endpoint command related enable/disable MACROS
#define QUSB_EXECUTE_EP_GENERIC_COMMAND     0x1 /* Bit to enable the execution of Generic command*/
#define QUSB_EP_FIFO_NUMBER                 0x0 /* Bits to indicate the fifo_num to use*/
#define QUSB_ENABLE_XFER_NOT_READY          0x1
#define QUSB_DISABLE_XFER_NOT_READY         0x0
#define QUSB_ENABLE_XFER_COMPLETE           0x1
#define QUSB_DISABLE_XFER_COMPLETE          0x0

// Define burst size value for Bulk endpoints.
#define QUSB_BULK_IN_BURST_SIZE             QUSB_BURST_SIZE_4
#define QUSB_BULK_OUT_BURST_SIZE            QUSB_BURST_SIZE_4

//Define the Burst Size Parameters 
#define QUSB_BURST_SIZE_1                   0x0
#define QUSB_BURST_SIZE_2                   0x1
#define QUSB_BURST_SIZE_3                   0x2
#define QUSB_BURST_SIZE_4                   0x3
#define QUSB_BURST_SIZE_5                   0x4
#define QUSB_BURST_SIZE_6                   0x5
#define QUSB_BURST_SIZE_7                   0x6
#define QUSB_BURST_SIZE_8                   0x7
#define QUSB_BURST_SIZE_9                   0x8
#define QUSB_BURST_SIZE_10                  0x9
#define QUSB_BURST_SIZE_11                  0xA
#define QUSB_BURST_SIZE_12                  0xB
#define QUSB_BURST_SIZE_13                  0xC
#define QUSB_BURST_SIZE_14                  0xD
#define QUSB_BURST_SIZE_15                  0xE                   
#define QUSB_BURST_SIZE_16                  0xF



#define DCI_EP0     0

#define QUSB_SNPS_HW_CORE_ID                          0x5533250A
#define QUSB_CSFTRST_TIME_OUT 	                    1000
#define QUSB_PHY_TIME_OUT			                    1000
#define QUSB_PARAMETER_OVERRIDE_X_VALUE               0x00D191A4
#define DCI_ZLP_TRB                                     0x1
#define VAL_TO_BOOL(x)                                  ((boolean)((x) != 0))

//----------------------------------------------------------------------------
// Data Writing Macros
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Type Declarations
//----------------------------------------------------------------------------


struct qusb_dcd_dsc_device;

typedef struct 
{
  struct qusb_dcd_dsc_device*  dcd_context_ptr;    // For DCD internal use (ask by Vladimir)
} core_info_t;

typedef enum
{
  HW_REV_INVALID=0,
  HW_REV_1_0,
  HW_REV_2_0,
  HW_REV_3_0,
} qusb_hw_rev;

//----------------------------------------------------------------------------
// Function Declarations and Documentation
//----------------------------------------------------------------------------
 
//----------------------------------------------------------------------------
// Target Specific Function Declarations and Documentation
//----------------------------------------------------------------------------
void qusb_dci_configure_usb30(qusb_max_speed_required_t speed_required); /*Initialize the USB3.0 Controller for target*/
void qusb_dci_configure_device_mode(qusb_max_speed_required_t speed_required);

// ------------------------------------------------------------------------------------------------
// Global Variables
// ------------------------------------------------------------------------------------------------

extern core_info_t dci_core_info[DCI_MAX_CORES];

/****************************************USB3***********************************/

/***************************************ERRORS**********************************/
#define QUSB_ERR_NOT_SUPPORTED                        (1)
#define QUSB_ERR_HW_FAILURE                           (2)
#define QUSB_ERR_BAD_PARAM                            (3)
#define QUSB_ERR_URB_SIGNATURE                        (4)
#define QUSB_ERR_SETUP_PENDING                        (5)
#define QUSB_ERR_TIMER_IS_RUNNING                     (6)
#define QUSB_ERR_INVALID_OPERATION                    (7)
#define QUSB_ERR_EP_CMD_ACTIVE                        (8)
#define QUSB_ERR_INVALID_COMMAND                      (9)


/*******************************************************************************
 * TRBs
 ******************************************************************************/
#define DCI_MAX_TRBS 1 
#define QUSB_TRB_ALIGN_MASK                      (QUSB_TRB_ALIGN_SIZE - 1)
#define QUSB_TRB_MAX_BUFFER_SIZE                 0xFFFFFF   
#define QUSB_TRB_MAX_OUT_XFER_BUFFER_SIZE        0xFFFC00

typedef enum trb_ctl_type {
  QUSB_TRB_CTL_NORMAL = 1,
  QUSB_TRB_CTL_CONTROL_SETUP,
  QUSB_TRB_CTL_CONTROL_STATUS_2,
  QUSB_TRB_CTL_CONTROL_STATUS_3,
  QUSB_TRB_CTL_CONTROL_DATA,
  QUSB_TRB_CTL_ISOCHRONOUS_FIRST,
  QUSB_TRB_CTL_ISOCHRONOUS,
  QUSB_TRB_CTL_LINK_TRB
} trb_ctl_type_t;


/*******************************************************************************
 * Events
 ******************************************************************************/

#define EVENT_BUFFER_SIZE 240

#define QUSB_DEV_INTR_LINE 0

#define QUSB_EVT_TYPE_EP  0
#define QUSB_EVT_TYPE_NON_EP 1

#define QUSB_TRB_ALIGN_SIZE                      16


/* SNPS 6.2.8.1 , Table 6-48 */
struct qusb_event_depevt
{	
  uint32	event_type:1;
  uint32	phy_ep_number:5;
  uint32	ep_xfer_event:4;
  uint32	rsvd1:2;
  uint32	event_status:4;
  uint32	event_param:16;
};

  /* Control EP Status */
#define QUSB_DEPEVT_STATUS_CONTROL_SETUP	   0
#define QUSB_DDEPEVT_STATUS_CONTROL_DATA	   1
#define QUSB_DDEPEVT_STATUS_CONTROL_STATUS   2

#define QUSB_EVT_LINK_STATE 0xF
#define QUSB_EVT_INFO_SS 1
#define QUSB_EVT_INFO_HS 0
#define QUSB_EVT_INFO_SPEED_TYPE(x)  (((x >> 4) & 0x1))  
 
/* SNPS 6.2.8.2 , Table 6-49 */
struct qusb_event_devt
{	
  uint32	event_type:1;
  uint32	dev_spec_event:7;
  uint32	dev_event:4;
  uint32	rsvd1:4;
  uint32	event_info:9;
  uint32	rsvd2:7;
};
 

union qusb_event {
  uint32		generic;
  struct qusb_event_depevt	depevt;
  struct qusb_event_devt    devt;
};

typedef struct qusb_event_buffer_info {
  //events are generally 4Bytes long, exception of 1 event, which is 5 Bytes Long
  void 		*buf;
  uint32		cur_pos;
}qusb_event_buffer_info_t;


typedef enum qusb_evt_dev_evt_t{
  QUSB_EVT_DEV_DISCONNECT_EVENT = 0,
  QUSB_EVT_DEV_USB_RESET,
  QUSB_EVT_DEV_CONNECT_DONE,
  QUSB_EVT_DEV_USB_LINK_STATE_CHANGE,
  QUSB_EVT_DEV_REMOTE_WAKE_UP,
  QUSB_EVT_DEV_HIBERNATION_REQUEST,
  QUSB_EVT_DEV_SUSPEND_ENTRY,
  QUSB_EVT_DEV_START_OF_FRAME,
  QUSB_EVT_DEV_RESERVED,
  QUSB_EVT_DEV_ERRATIC_ERROR,
  QUSB_EVT_DEV_CMD_COMPLETE,
  QUSB_EVT_DEV_EVENT_BUFFER_OVERFLOW,
  QUSB_EVT_DEV_VNDR_DEV_TST_LMP_RCVED
} qusb_evt_dev_evt_type;


typedef enum qusb_evt_ep_evt_t{
  QUSB_EVT_EP_RSV0 = 0,
  QUSB_EVT_EP_XFER_COMPLETE,
  QUSB_EVT_EP_XFER_IN_PROGRESS,
  QUSB_EVT_EP_XFER_NOT_READY,
  QUSB_EVT_EP_RSV1,
  QUSB_EVT_EP_RSV2,
  QUSB_EVT_EP_STREAM,
  QUSB_EVT_EP_CMD_COMPLETE
} qusb_evt_ep_evt_type;

typedef enum
{
  QUSB_DCI_U1_INIT_ENABLE,
  QUSB_DCI_U2_INIT_ENABLE,
  QUSB_DCI_U1_U2_ACCPET_ENABLE,
} qusb_dci_u1_u2_event_type;



/*******************************************************************************
 * Endpoint Commands
 ******************************************************************************/

typedef struct qusb_ep_cmd_reg {
  uint32                    cmd_typ              :4;
  uint32                    rsvd0                :4;
  uint32                    cmd_ioc              :1;
  uint32                    rsvd1                :1;
  uint32                    cmd_act              :1;
  uint32                    hi_pri_force_rm      :1;
  uint32                    cmd_status           :4;
  uint32                    cmd_param            :16;
} qusb_ep_cmd_reg_t;

typedef struct qusb_ep_cmd_template {
  uint32                    parameter2;
  uint32                    parameter1;
  uint32                    parameter0;
  union 
  {
    uint32                  dword;
    qusb_ep_cmd_reg_t     reg;
  } cmd;
} qusb_ep_cmd_template_t;

#define QUSB_EP_CMD_SET_EP_CONFIG_VAL                          0x01
#define QUSB_EP_CMD_SET_EP_CONFIG_USB_ENDPT_NUM_SHIFT          0x1
#define QUSB_EP_CMD_SET_EP_CONFIG_USB_ENDPT_DIR_SHIFT          0x0
#define QUSB_EP_CMD_SET_EP_CONFIG_CONFIG_ACT_INIT              0
#define QUSB_EP_CMD_SET_EP_CONFIG_CONFIG_ACT_RESTORE           1
#define QUSB_EP_CMD_SET_EP_CONFIG_CONFIG_ACT_MODIFY            2
#define QUSB_EP_CMD_SET_EP_CONFIG_EP_TYPE_CTRL                 0
#define QUSB_EP_CMD_SET_EP_CONFIG_EP_TYPE_ISOC                 1
#define QUSB_EP_CMD_SET_EP_CONFIG_EP_TYPE_BULK                 2
#define QUSB_EP_CMD_SET_EP_CONFIG_EP_TYPE_INTR                 3

typedef struct qusb_ep_cmd_set_ep_config {
  uint32                    Rsvd0;
  uint32                    intr_num            :5;
  uint32                    Rsvd1               :3;
  uint32                    xfer_cmpl_en        :1;
  uint32                    xfer_in_progess     :1;
  uint32                    xfer_nrdy_en        :1;
  uint32                    Rsvd2               :1;
  uint32                    rsvd3               :1;
  uint32                    strm_evt_en         :1;
  uint32                    rsvd4               :1;
  uint32                    ebc                 :1;
  uint32                    bInterval_m1        :8;
  uint32                    strm_cap            :1;
  uint32                    usb_ep_num          :5;
  uint32                    bulk_based          :1;
  uint32                    fifo_based          :1;
  uint32                    rsvd5               :1;
  uint32                    ep_type             :2;
  uint32                    max_packet_sz       :11;
  uint32                    rsvd6               :3;
  uint32                    fifo_num            :5;
  uint32                    brst_siz            :4;
  uint32                    rsvd7               :4;
  uint32                    config_action       :2;

  qusb_ep_cmd_reg_t            cmd;
} qusb_ep_cmd_set_ep_config_t;


#define QUSB_EP_CMD_SET_EP_XFER_RSC_CONFIG_VAL   (0x02)
#define QUSB_EP_CMD_GET_EP_STATE_VAL             (0x03)
#define QUSB_EP_CMD_SET_STALL_VAL                (0x04)
#define QUSB_EP_CMD_CLEAR_STALL_VAL              (0x05)
#define QUSB_EP_CMD_START_XFER_VAL               (0x06)
#define QUSB_EP_CMD_UPDATE_XFER_VAL              (0x07)
#define QUSB_EP_CMD_END_XFER_VAL                 (0x08)
#define QUSB_EP_CMD_START_NEW_CONFIG_VAL         (0x09)

typedef struct qusb_ep_cmd_set_ep_xfer_rsc_config {
  uint32                    rsvd0;
  uint32                    Rsvd1;
  uint32                    num_xfer_res        :16;
  uint32                    rsvd2               :16;
  qusb_ep_cmd_reg_t         cmd;
} qusb_ep_cmd_set_ep_xfer_rsc_config_t;



typedef struct qusb_ep_cmd_get_ep_state {
  uint32                    rsvd0;
  uint32                    rsvd1;
  uint32                    rsvd2;
  qusb_ep_cmd_reg_t       cmd;
} qusb_ep_cmd_get_ep_state_t;


typedef struct qusb_ep_cmd_set_stall {
  uint32                    rsvd0;
  uint32                    rsvd1;
  uint32                    rsvd2;
  qusb_ep_cmd_reg_t       cmd;
} qusb_ep_cmd_set_stall_t;

typedef struct qusb_ep_cmd_clear_stall {
  uint32                    rsvd0;
  uint32                    rsvd1;
  uint32                    rsvd2;
  qusb_ep_cmd_reg_t            cmd;
} qusb_ep_cmd_clear_stall_t;


typedef struct qusb_ep_cmd_start_xfer {
  uint32                    rsvd0;
  uint32                    td_address_lo;
  uint32                    td_address_hi;
  qusb_ep_cmd_reg_t            cmd;
} qusb_ep_cmd_start_xfer_t;


typedef struct qusb_ep_cmd_update_xfer {
  uint32                    rsvd0;
  uint32                    rsvd1;
  uint32                    rsvd2;
  qusb_ep_cmd_reg_t            cmd;
} qusb_ep_cmd_update_xfer_t;


typedef struct qusb_ep_cmd_end_xfer {
  uint32                    rsvd0;
  uint32                    rsvd1;
  uint32                    rsvd2;
  qusb_ep_cmd_reg_t            cmd;
} qusb_ep_cmd_end_xfer_t;


typedef struct qusb_ep_cmd_start_new_config {
  uint32                    rsvd0;
  uint32                    rsvd1;
  uint32                    rsvd2;
  qusb_ep_cmd_reg_t            cmd;
} qusb_ep_cmd_start_new_config_t;




typedef union qusb_ep_cmd {
  qusb_ep_cmd_template_t                 template;
  qusb_ep_cmd_set_ep_config_t            set_ep_config;
  qusb_ep_cmd_set_ep_xfer_rsc_config_t   set_ep_xfer_rsc_config;
  qusb_ep_cmd_get_ep_state_t             get_ep_state;
  qusb_ep_cmd_set_stall_t                set_stall;
  qusb_ep_cmd_clear_stall_t              clear_stall;
  qusb_ep_cmd_start_xfer_t               start_xfer;
  qusb_ep_cmd_update_xfer_t              update_xfer;
  qusb_ep_cmd_end_xfer_t                 end_xfer;
  qusb_ep_cmd_start_new_config_t         start_new_config;
} qusb_ep_cmd_t;

/*******************************************************************************
 * Device Commands
 ******************************************************************************/
typedef struct qusb_dev_cmd_reg {
  uint32                    cmd_typ                     :8;
  uint32                    cmd_ioc                     :1;
  uint32                    rsvd1                       :1;
  uint32                    cmd_act                     :1;
  uint32                    rsvd2                       :21; 
} qusb_dev_cmd_reg_t;

typedef struct qusb_dev_cmd_template {
  uint32                    parameter;
  union
  {
    uint32                  dword;
    qusb_dev_cmd_reg_t    reg;    
  } cmd;
} qusb_dev_cmd_template_t;

#define QUSB_DEV_CMD_TX_SET_LINK_FN_LMP_VAL            0x01
#define QUSB_DEV_CMD_SET_PERIODIC_PARAMS_VAL           0x02
#define QUSB_DEV_CMD_TX_FN_WAKE_DEV_NOTIFY_VAL         0x03
#define QUSB_DEV_CMD_SET_SCRATCHPAD_BUF_LO_VAL         0x04
#define QUSB_DEV_CMD_SET_SCRATCHPAD_BUF_HI_VAL         0x05
#define QUSB_DEV_CMD_TX_FN_HOST_REQ_NOTIFY_VAL         0x06
#define QUSB_DEV_CMD_TX_DEVICE_NOTIFY_VAL              0x07
#define QUSB_DEV_CMD_SELECTED_FIFO_FLUSH_VAL           0x09
#define QUSB_DEV_CMD_ALL_FIFO_FLUSH_VAL                0x0A
#define QUSB_DEV_CMD_SET_EP_NRDY_VAL                   0x0C
#define QUSB_DEV_CMD_RUN_SOC_LOOPBACK_TEST_VAL         0x10

typedef struct qusb_dev_cmd_tx_set_link_fn_lmp {
  uint32                    force_link_pm_accept        :1;
  uint32                    rsvd                        :31;
  qusb_dev_cmd_reg_t      cmd;
} qusb_dev_cmd_tx_set_link_fn_lmp_t;

typedef struct qusb_dev_cmd_set_periodic_params {
  uint32                    system_exit_latency         :10;
  uint32                    rsvd                        :22;
  qusb_dev_cmd_reg_t      cmd;
} qusb_dev_cmd_set_periodic_params_t; 

typedef struct qusb_dev_cmd_tx_fn_wake_dev_notify {
  uint32                    int_f_num                   :8;
  uint32                    rsvd                        :24;
  qusb_dev_cmd_reg_t      cmd;
} qusb_dev_cmd_tx_fn_wake_dev_notify_t;

typedef struct qusb_dev_cmd_set_scratchpad_buf_lo {
  uint32                    address_lo;
  qusb_dev_cmd_reg_t      cmd;
} qusb_dev_cmd_set_scratchpad_buf_lo_t;

typedef struct qusb_dev_cmd_set_scratchpad_buf_hi {
  uint32                    address_hi;
  qusb_dev_cmd_reg_t      cmd;
} qusb_dev_cmd_set_scratchpad_buf_hi_t;

typedef struct qusb_dev_cmd_tx_fn_host_req_notify {
  uint32                    initiate                    :1;
  uint32                    confirm                     :1;
  uint32                    rsvd                        :30;
  qusb_dev_cmd_reg_t      cmd;
} qusb_dev_cmd_tx_fn_host_req_notify_t;

typedef struct qusb_dev_cmd_tx_device_notify {
  uint32                    notification_type           :4;
  uint32                    notification_params         :28;
  qusb_dev_cmd_reg_t      cmd;
} qusb_dev_cmd_tx_device_notify_t;

typedef struct qusb_dev_cmd_selected_fifo_flush {
  uint32                    fifo_number                 :5;
  uint32                    tx_rx_fifo                  :1;
  uint32                    rsvd                        :26;
  qusb_dev_cmd_reg_t      cmd;
} qusb_dev_cmd_selected_fifo_flush_t;

typedef struct qusb_dev_cmd_all_fifo_flush {
  uint32                    rsvd;
  qusb_dev_cmd_reg_t      cmd;
} qusb_dev_cmd_all_fifo_flush_t;

typedef struct qusb_dev_cmd_set_ep_nrdy {
  uint32                    phys_ep_number              :5;
  uint32                    rsvd0                       :27;
  qusb_dev_cmd_reg_t      cmd;
} qusb_dev_cmd_set_ep_nrdy_t;

typedef struct qusb_dev_cmd_run_soc_loopback_test {
  uint32                    loopback_enable             :1;
  uint32                    rsvd                        :31;
  qusb_dev_cmd_reg_t      cmd;
} qusb_dev_cmd_run_soc_loopback_test_t;

typedef union qusb_dev_cmd {
  qusb_dev_cmd_template_t                 template;
  qusb_dev_cmd_tx_set_link_fn_lmp_t       set_link_fn_lmp; 
  qusb_dev_cmd_set_periodic_params_t      set_periodic_params; 
  qusb_dev_cmd_tx_fn_wake_dev_notify_t    tx_fn_wake_dev_notify;
  qusb_dev_cmd_set_scratchpad_buf_lo_t    set_scratchpad_buf_lo;
  qusb_dev_cmd_set_scratchpad_buf_hi_t    set_scratchpad_buf_hi;
  qusb_dev_cmd_tx_fn_host_req_notify_t    tx_fn_host_req_notify;
  qusb_dev_cmd_tx_device_notify_t         tx_device_notify;
  qusb_dev_cmd_selected_fifo_flush_t      selected_fifo_flush;
  qusb_dev_cmd_all_fifo_flush_t           all_fifo_flush;
  qusb_dev_cmd_set_ep_nrdy_t              set_ep_nrdy;
  qusb_dev_cmd_run_soc_loopback_test_t    run_soc_loopback_test;
}qusb_dev_cmd_t;


#define QUSB_EP_INDEX(ep,dir)                    (((ep) * 2) + (dir))
#define QUSB_PHY_EP_TO_DIR(x)                    (x%2)
#define QUSB_PHY_EP_TO_EP(x)                      (((x+1) >> 1 ) - (x%2))


typedef struct qusb_trb {
  uint32                    buf_ptr_lo;
  uint32                    buf_ptr_hi;
  uint32                    buf_size            :24;
  uint32                    pcm1                :2;
  uint32                    rsvd0               :2;
  uint32                    trbsts              :4;
  uint32                    hwo                 :1;
  uint32                    lst                 :1;
  uint32                    chn                 :1;
  uint32                    csp                 :1;
  uint32                    trb_ctl             :6;
  uint32                    isp_imi             :1;
  uint32                    ioc                 :1;
  uint32                    rsvd1               :2;
  uint32                    sid_sofn            :16;
  uint32                    rsvd2               :2;
}qusb_trb_t;




typedef struct qusb_trb_ring_info {
  void                      *buf;
  uint32                    buf_size;
  qusb_urb_t                *urb_ptr;
  uint32                    ep_state;
} qusb_trb_ring_t;

#define QUSB_EP_ACTIVE  ( 1 << 0 )
#define QUSB_EP_HALTED  ( 1 << 1 ) 

#define QUSB_EP0_OUT (1 << 0) 
#define QUSB_EP0_IN  (1 << 1)
#define QUSB_EP1_OUT (1 << 2) 
#define QUSB_EP1_IN  (1 << 3)

#endif /*QUSB_DCI_PRIVATE_H*/
