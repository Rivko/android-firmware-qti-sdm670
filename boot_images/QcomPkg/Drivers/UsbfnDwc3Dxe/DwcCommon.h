/**
 * @defgroup CommonLayer Common Layer
 *
 * @brief Synopsys USB function driver code common to UEFI and Windows drivers
 *
 * The common layer provides a minimal set of services at the lowest level of
 * the driver for both the UEFI USB function driver and Windows USB function
 * driver stack.  Its primary concerns are controller event handling and
 * processing of Transfer Request Blocks (TRBs) for transferring data on
 * endpoints.
 */
/**
 * @file DwcCommon.h
 *
 * @ingroup CommonLayer
 *
 * @brief Device mode common layer header.
 *
 * This file defines the common layer for the Synopsys USB 3.0 controller
 * device mode low level functionality.
 *
 * Endpoint and device commands are not executed by the functions of this
 * layer, except in the transfer related functions.
 *
 * @copyright Copyright (C) 2013-2017 by QUALCOMM Technologies, Inc.  All Rights Reserved.
 */
/*==============================================================================
 EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 02/09/17   ck      Work around the issue that LPM Not Rejected During Control Transfer
 11/01/16   ck      Enhance debug message and conditionally fix the STALL issue
 01/22/16   ck      Port USB SetConfig fix
 12/18/14   ck      Address software & hardware transfer cancel notification issue
 06/26/14   amitg   Address 64 bit issue for FFU enumeration
 06/17/14   ck      fix typo in the comment
 08/23/13   mliss   Initial import
 =============================================================================*/

#ifndef _DWC_COMMON_H_
#define _DWC_COMMON_H_

#include "DwcPlatform.h"


// encode an error value by setting the upper bit (make a negative integer)
#define DWC_ENCODE_ERROR(x)                           (0x80000000 | (x))

// test for error status
#define DWC_ERROR(x)                                  ((x) < 0)

// status value enumeration
#define DWC_SUCCESS                                   0
#define DWC_UNSUPPORTED                               DWC_ENCODE_ERROR(1)
#define DWC_INVALID_PARAMETER                         DWC_ENCODE_ERROR(2)
#define DWC_INVALID_COMMAND_TYPE                      DWC_ENCODE_ERROR(3)
#define DWC_ALREADY_INITIALIZED                       DWC_ENCODE_ERROR(4)
#define DWC_OUT_OF_MEMORY                             DWC_ENCODE_ERROR(5)
#define DWC_DEVICE_ERROR                              DWC_ENCODE_ERROR(6)
#define DWC_COMMAND_ACTIVE                            DWC_ENCODE_ERROR(7)
#define DWC_INDEX_OUT_OF_RANGE                        DWC_ENCODE_ERROR(8)
#define DWC_TIMEOUT                                   DWC_ENCODE_ERROR(9)
#define DWC_EVT_PENDING                               DWC_ENCODE_ERROR(10)
#define DWC_TRB_RING_FULL                             DWC_ENCODE_ERROR(11)
#define DWC_ENDPOINT_UNINITIALIZED                    DWC_ENCODE_ERROR(12)
#define DWC_ENDPOINT_ACTIVE                           DWC_ENCODE_ERROR(13)
#define DWC_ENDPOINT_NOT_ACTIVE                       DWC_ENCODE_ERROR(14)
#define DWC_ENDPOINT_INVALID_STATE                    DWC_ENCODE_ERROR(15)
#define DWC_INVALID_DEVICE_STATE                      DWC_ENCODE_ERROR(16)

// usb related
#define DWC_USB_MAX_ENDPOINTS                         32      // physical endpoints
#define DWC_USB_MAX_ENDPOINT_NUMBER                   15
#define DWC_USB_MAXIMUM_MAXPKTSIZE                    1024

// event buffer related
#define DWC_EVENT_BUFFER_ALIGN_SIZE                   4
#define DWC_EVENT_BUFFER_ALIGN_MASK                   (DWC_EVENT_BUFFER_ALIGN_SIZE - 1)
#define DWC_EVENT_BUFFER_SIZE_INT_MASK                0x80000000
#define DWC_DEVICE_EVENT_BUFFER_INDEX                 0
#define DWC_BYTES_PER_EVENT                           4
#define DWC_BYTES_PER_LMP_RX_EVENT                    8
#define DWC_EVT_BUFFER_DISABLE_INTR                   0
#define DWC_EVT_BUFFER_ENABLE_INTR                    1
#define DWC_EP_EVT_CMDCMPLT_PARAM_TYPE_BMSK           0xF00
#define DWC_EP_EVT_CMDCMPLT_PARAM_TYPE_SHFT           0x8
#define DWC_EP_EVT_CMDCMPLT_PARAM_RSCID_BMSK          0x7F
#define DWC_EP_EVT_CMDCMPLT_PARAM_RSCID_SHFT          0x0
#define DWC_EP_EVT_EVENTSTATUS_SUCCESS                0
#define DWC_EP_EVT_EVENTSTATUS_STAGE_BMSK             0x3
#define DWC_EP_EVT_EVENTSTATUS_STAGE_DATA             0x1
#define DWC_EP_EVT_EVENTSTATUS_STAGE_STATUS           0x2
#define DWC_EP_NUM_DEVICE_EVENT                       253     // endpoint number used to reference device events

// common layer implementation related
#define DWC_EP_INDEX(Ep,Dir)                          (((Ep)<<1) + (Dir))
#define DWC_EP_FROM_INDEX(Index,Ep,Dir)               {Dir = (Index&1); Ep = (UINT8)((Index-Dir)>>1);}
#define DWC_EP_RESOURCE_ID_INVALID                    255
#define DWC_EP_EVENT_BUFFER_ID_INVALID                254

#define DWC_DEVICE_NO_STREAM_ID                       0       // used for StreamID when the ep is non-streaming
#define DWC_POLL_TIMEOUT                              10000

// Maximum retry attempts if command active detected
#define DWC_MAX_SPIN_COUNT                            1000

#define DWC_EP_TYPE_MASK_CONTROL                      (1 << DWC_EP_TYPE_CONTROL)
#define DWC_EP_TYPE_MASK_BULK                         (1 << DWC_EP_TYPE_BULK)
#define DWC_EP_TYPE_MASK_BULK_STREAM                  (1 << DWC_EP_TYPE_BULK_STREAM)
#define DWC_EP_TYPE_MASK_INTERRUPT                    (1 << DWC_EP_TYPE_INTERRUPT)
#define DWC_EP_TYPE_MASK_ISOCHRONOUS                  (1 << DWC_EP_TYPE_ISOCHRONOUS)
#define DWC_EP_TYPE_MASK_APERIODIC_DATA               (DWC_EP_TYPE_MASK_BULK        | \
                                                       DWC_EP_TYPE_MASK_BULK_STREAM | \
                                                       DWC_EP_TYPE_MASK_INTERRUPT)
#define DWC_EP_TYPE_MASK_MAX                          (DWC_EP_TYPE_MASK_BULK        | \
                                                       DWC_EP_TYPE_MASK_BULK_STREAM | \
                                                       DWC_EP_TYPE_MASK_INTERRUPT   | \
                                                       DWC_EP_TYPE_MASK_ISOCHRONOUS | \
                                                       DWC_EP_TYPE_MASK_CONTROL     )

// device command values
#define DWC_DEV_CMD_SET_PERIODIC_PARAMS_VAL           0x02
#define DWC_DEV_CMD_SET_SCRATCHPAD_BUF_LO_VAL         0x04
#define DWC_DEV_CMD_SET_SCRATCHPAD_BUF_HI_VAL         0x05
#define DWC_DEV_CMD_TX_DEVICE_NOTIFY_VAL              0x07
#define DWC_DEV_CMD_SELECTED_FIFO_FLUSH_VAL           0x09
#define DWC_DEV_CMD_ALL_FIFO_FLUSH_VAL                0x0A
#define DWC_DEV_CMD_SET_EP_NRDY_VAL                   0x0C
#define DWC_DEV_CMD_RUN_SOC_LOOPBACK_TEST_VAL         0x10

// endpoint command values
#define DWC_EP_CMD_SET_EP_CONFIG_VAL                  0x01
#define DWC_EP_CMD_SET_EP_XFER_RSC_CONFIG_VAL         0x02
#define DWC_EP_CMD_GET_EP_STATE_VAL                   0x03
#define DWC_EP_CMD_SET_STALL_VAL                      0x04
#define DWC_EP_CMD_CLEAR_STALL_VAL                    0x05
#define DWC_EP_CMD_START_XFER_VAL                     0x06
#define DWC_EP_CMD_UPDATE_XFER_VAL                    0x07
#define DWC_EP_CMD_END_XFER_VAL                       0x08
#define DWC_EP_CMD_START_NEW_CONFIG_VAL               0x09

// set endpoint config command fields
#define DWC_EP_CMD_SET_EP_CONFIG_USB_ENDPT_NUM_SHIFT  0x1
#define DWC_EP_CMD_SET_EP_CONFIG_USB_ENDPT_DIR_SHIFT  0x0
#define DWC_EP_CMD_SET_EP_CONFIG_CONFIG_ACT_INIT      0
#define DWC_EP_CMD_SET_EP_CONFIG_CONFIG_ACT_RESOTRE   1
#define DWC_EP_CMD_SET_EP_CONFIG_CONFIG_ACT_MODIFY    2

// TRB related
#define DWC_TRB_ALIGN_SIZE                            16
#define DWC_TRB_ALIGN_MASK                            (DWC_TRB_ALIGN_SIZE - 1)
#define DWC_TRB_MAX_BUFFER_SIZE                       0xFFFFFF   // 16 MB - 1
#define DWC_TRB_MAX_OUT_XFER_BUFFER_SIZE              0xFFFC00
#define DWC_TRB_RING_MIN_BUFFER_SIZE                  (3 * sizeof(DWC_TRB))

// EP Event related
#define DWC_EVT_DEV_CMD_COMPLETE_CMD_TYPE_SHIFT       8
#define DWC_EVT_EP_XFERCMPLT_EVTSTS_SHORT_PKT_BMSK    0x2
#define DWC_EVT_EP_XFERCMPLT_EVTSTS_SHORT_PKT_SHFT    1

//
// enumeration types
//

//! Hardware core information type
#pragma pack  (push,1)
typedef union _CORE_INFO {
    struct {
        UINT16   CoreID;
        UINT16   ReleaseID;
    } Part;
    UINT32  SynopsysID;
} DWC_CORE_INFO;
#pragma  pack (pop)

#define DWC_CORE_ID          (0x5533)  // 'U3'
#define DWC_CORE_RELEASE_V1  (0x210a)  // 2.10a
#define DWC_CORE_RELEASE_V2  (0x230a)  // 2.30a
#define DWC_CORE_RELEASE_V3  (0x250a)  // 2.50a

//! USB Connected Speed
typedef enum _DWC_DEVICE_CONNECT_SPEED{
  DWC_SUPER_SPEED = 4,
  DWC_HIGH_SPEED  = 0,
  DWC_FULL_SPEED  = 1,
  DWC_LOW_SPEED   = 2,
  DWC_FULL_SPEED_48 = 3
} DWC_DEVICE_CONNECT_SPEED;

//! USB Link State
typedef enum _DWC_USB_LINK_STATE{
  // SuperSpeed link states
  DWC_LS_U0 = 0,
  DWC_LS_U1,
  DWC_LS_U2,
  DWC_LS_U3,
  DWC_LS_SS_DIS,
  DWC_LS_RX_DET,
  DWC_LS_SS_INACT,
  DWC_LS_POLL,
  DWC_LS_RECOV,
  DWC_LS_HRESET,
  DWC_LS_CMPLY,
  DWC_LS_LPBK,
  DWC_LS_RESUME_RESET = 0xF,
  // HighSpeed link states
  DWC_LS_ON             = 0,
  DWC_LS_SLEEP          = 2,
  DWC_LS_SUSPEND,
  DWC_LS_DIS,
  DWC_LS_EARLY_SUSPEND,
  DWC_LS_RESET          = 0xE,
  DWC_LS_RESUME,
  // Default init state
  DWC_LS_DEFAULT_INIT   = 0x10
} DWC_USB_LINK_STATE;

//! Hardware Endpoint transfer type
typedef enum _DWC_ENDPOINT_TYPE {
  DWC_EP_TYPE_CONTROL = 0,
  DWC_EP_TYPE_BULK,
  DWC_EP_TYPE_BULK_STREAM,
  DWC_EP_TYPE_INTERRUPT,
  DWC_EP_TYPE_ISOCHRONOUS,
  DWC_EP_TYPE_MAX
} DWC_ENDPOINT_TYPE;

//! Hardware Endpoint direction type
typedef enum _DWC_ENDPOINT_DIR {
  DWC_EP_DIR_OUT = 0,
  DWC_EP_DIR_RX  = DWC_EP_DIR_OUT,
  DWC_EP_DIR_IN  = 1,
  DWC_EP_DIR_TX  = DWC_EP_DIR_IN,
  DWC_EP_DIR_MAX
} DWC_ENDPOINT_DIR;

//! Hardware Endpoint state type
typedef enum _DWC_EP_STATE {
  DWC_EP_STATE_UNINITIALIZED,
  DWC_EP_STATE_INACTIVE,
  DWC_EP_STATE_XFER_IN_PROGRESS,
  DWC_EP_STATE_STALLED,
  DWC_EP_STATE_START_XFER,
  DWC_EP_STATE_STALLED_XFER_PENDING
} DWC_EP_STATE;

//! Hardware Device event type
typedef enum _DWC_EVT_DEV_EVT_TYPE {
  DWC_EVT_DEV_DISCONNECT_EVENT = 0,
  DWC_EVT_DEV_USB_RESET,
  DWC_EVT_DEV_CONNECT_DONE,
  DWC_EVT_DEV_USB_LINK_STATE_CHANGE,
  DWC_EVT_DEV_RESUME_REMOTE_WAKE_UP,
  DWC_EVT_DEV_HIBERNATION_REQUEST,
  DWC_EVT_DEV_U3L2L1_SUSPEND,
  DWC_EVT_DEV_START_OF_FRAME,
  DWC_EVT_DEV_RSV0,
  DWC_EVT_DEV_ERRATIC_ERROR,
  DWC_EVT_DEV_CMD_COMPLETE,
  DWC_EVT_DEV_EVENT_BUFFER_OVERFLOW,
  DWC_EVT_DEV_VNDR_DEV_TST_LMP_RCVED,
  DWC_EVT_DEV_MAX
} DWC_EVT_DEV_EVT_TYPE;

//! Hardware Endpoint event type
typedef enum _DWC_EVT_EP_EVT_TYPE {
  DWC_EVT_EP_RSV0 = 0,
  DWC_EVT_EP_XFER_COMPLETE,
  DWC_EVT_EP_XFER_IN_PROGRESS,
  DWC_EVT_EP_XFER_NOT_READY,
  DWC_EVT_EP_RSV1,
  DWC_EVT_EP_RSV2,
  DWC_EVT_EP_STREAM,
  DWC_EVT_EP_EP_CMD_COMPLETE
} DWC_EVT_EP_EVT_TYPE;

//! USB TRB control message type
typedef enum _TRB_CTL_TYPE {
  DWC_TRB_CTL_INVALID = 0,
  DWC_TRB_CTL_NORMAL,
  DWC_TRB_CTL_CONTROL_SETUP,
  DWC_TRB_CTL_CONTROL_STATUS_2,
  DWC_TRB_CTL_CONTROL_STATUS_3,
  DWC_TRB_CTL_CONTROL_DATA,
  DWC_TRB_CTL_ISOCHRONOUS_FIRST,
  DWC_TRB_CTL_ISOCHRONOUS,
  DWC_TRB_CTL_LINK_TRB
} TRB_CTL_TYPE;

//! USB TRB status type
typedef enum _TRB_STS_TYPE {
  DWC_TRB_STS_OK = 0,
  DWC_TRB_STS_MISSED_ISOC,
  DWC_TRB_STS_SETUP_PENDING
} TRB_STS_TYPE;

//! USB charger state type
typedef enum _DWC_CHG_STATE_TYPE {
    DWC_CHG_ST_START,
    DWC_CHG_ST_BSESS_VALID,
    DWC_CHG_ST_WAIT_FOR_DCD,
    DWC_CHG_ST_CHARGING_PORT_DETECTION,
    DWC_CHG_ST_CHARGING_PORT_DETECTED,
    DWC_CHG_ST_SDP,
    DWC_CHG_ST_CDP,
    DWC_CHG_ST_DCP,
    DWC_CHG_ST_INVALID_CHARGER,
    DWC_CHG_ST_DONE,
    DWC_CHG_ST_END
} DWC_CHG_STATE_TYPE;

//! USB charger port type
// Refer to Battery Charging Specification Revision 1.2
// for the definition of the following port types.
typedef enum _DWC_CHG_PORT_TYPE {
    DWC_CHG_PORT_SDP,
    DWC_CHG_PORT_CDP,
    DWC_CHG_PORT_DCP,
    DWC_CHG_PORT_INVALID,
    DWC_CHG_PORT_UNKNOWN
} DWC_CHG_PORT_TYPE;


//
// common layer data structures
//

// The following definitions are derived from the Synopsys
// DWC USB3 databook for IP version 2.30a.  Later IP versions
// may necessitate updates to these structures.

//! Hardware register description type
typedef struct _DWC_REG_DESC {
  UINT32                    Offset;
  CONST CHAR8               *Name;
} DWC_REG_DESC;

//! Hardware Transfer Request Block type
struct _DWC_TRB {
  UINT32                    BufPtrLo;
  UINT32                    BufPtrHi;

  UINT32                    BufSize             :24;
  UINT32                    PCM1                :2;
  UINT32                    Rsvd0               :2;
  UINT32                    TRBSTS              :4;

  UINT32                    HWO                 :1;
  UINT32                    LST                 :1;
  UINT32                    CHN                 :1;
  UINT32                    CSP                 :1;
  UINT32                    TRBCTL              :6;
  UINT32                    ISP_IMI             :1;
  UINT32                    IOC                 :1;
  UINT32                    Rsvd1               :2;
  UINT32                    SID_SOFN            :16;
  UINT32                    Rsvd2               :2;
};

//! Hardware address type
typedef struct _DWC_ADDRESS {
  UINTN          Virtual;
  UINTN          Physical;
} DWC_ADDRESS;

//! Hardware USB request block type
struct _DWC_URB {
  UINT8                     Ep;
  UINT8                     EpDir;
  UINT8                     TrbCtl;
  UINT16                    StreamID;
  UINT32                    TotalTransactLen;
  BOOLEAN                   UseZlt;
  UINT32                    TotalBytesXfered;
  UINT8                     NumTRBs;
  UINT32                    SGLen;
  UINT32                    SGBytesXfered;
  UINT8                     XferTRBSTS;
  DWC_TRB                   *XferDesc;
  SCATTER_GATHER_LIST       *SGList;
};

//! Hardware USB TRB ring state
typedef struct _DWC_TRB_RING {
  DWC_ADDRESS               BufAddress;
  VOID                      *Buf;
  UINT32                    BufSize;
  DWC_TRB                   *Enq;
  DWC_TRB                   *Deq;
  UINT32                    NumTRBs;
} DWC_TRB_RING;

//! Hardware USB Endpoint state
typedef struct _DWC_EP {
  DWC_EP_STATE              State;
  UINT8                     ResourceID;
  UINT8                     EvtBufferID;
  DWC_TRB_RING              *Ring;
  DWC_URB                   *Urb;
  UINT32                    MaxPktSize;
} DWC_EP;

//! Hardware event buffer
typedef struct _DWC_EVENT_BUFFER {
  VOID                      *Buf;
  UINT32                    *NextEvt;
  UINT32                    Size;
} DWC_EVENT_BUFFER;

//! Hardware device state
struct _DWC_DEVICE {
  UINT32                    CoreBaseAddr;
  DWC_CORE_INFO             CoreInfo;
  UINT8                     DwcUsb3DeviceNumInt;
  UINT8                     EvtBufferCount;
  DWC_EVENT_BUFFER          *EvtBuffer;
  DWC_EP                    Ep[DWC_USB_MAX_ENDPOINTS];
};

//
//! Device command structures
//
typedef struct _DWC_DEV_CMD_REG {
  UINT32                    CmdTyp              :8;
  UINT32                    CmdIOC              :1;
  UINT32                    Rsvd1               :1;
  UINT32                    CmdAct              :1;
  UINT32                    Rsvd2               :4;
  UINT32                    CmdStatus           :1;
  UINT32                    Rsvd3               :16;
} DWC_DEV_CMD_REG;

typedef struct _DWC_DEV_CMD_TEMPLATE {
  UINT32                    Parameter;
  union {
    UINT32                  Dword;
    DWC_DEV_CMD_REG         Reg;
  } Cmd;
} DWC_DEV_CMD_TEMPLATE;

typedef struct _DWC_DEV_CMD_SET_PERIODIC_PARAMS {
  UINT32                    SystemExitLatency   :10;
  UINT32                    Rsvd                :22;

  DWC_DEV_CMD_REG           Cmd;
} DWC_DEV_CMD_SET_PERIODIC_PARAMS;

typedef struct _DWC_DEV_CMD_SET_SCRATCHPAD_BUF_LO {
  UINT32                    AddressLo;

  DWC_DEV_CMD_REG           Cmd;
} DWC_DEV_CMD_SET_SCRATCHPAD_BUF_LO;

typedef struct _DWC_DEV_CMD_SET_SCRATCHPAD_BUF_HI {
  UINT32                    AddressHi;

  DWC_DEV_CMD_REG           Cmd;
} DWC_DEV_CMD_SET_SCRATCHPAD_BUF_HI;

typedef struct _DWC_DEV_CMD_TX_DEVICE_NOTIFY {
  UINT32                    NotificationType    :4;
  UINT32                    NotificationParams  :28;

  DWC_DEV_CMD_REG           Cmd;
} DWC_DEV_CMD_TX_DEVICE_NOTIFY;

typedef struct _DWC_DEV_CMD_SELECTED_FIFO_FLUSH {
  UINT32                    FifoNumber          :5;
  UINT32                    TxRxFifo            :1;
  UINT32                    Rsvd                :26;

  DWC_DEV_CMD_REG           Cmd;
} DWC_DEV_CMD_SELECTED_FIFO_FLUSH;

typedef struct _DWC_DEV_CMD_ALL_FIFO_FLUSH {
  UINT32                    Rsvd;

  DWC_DEV_CMD_REG           Cmd;
} DWC_DEV_CMD_ALL_FIFO_FLUSH;

typedef struct _DWC_DEV_CMD_SET_EP_NRDY {
  UINT32                    PhysEPNumber        :5;
  UINT32                    Rsvd0               :27;

  DWC_DEV_CMD_REG           Cmd;
} DWC_DEV_CMD_SET_EP_NRDY;

typedef struct _DWC_DEV_CMD_RUN_SOC_LOOPBACK_TEST {
  UINT32                    LoopbackEnable      :1;
  UINT32                    Rsvd                :31;

  DWC_DEV_CMD_REG           Cmd;
} DWC_DEV_CMD_RUN_SOC_LOOPBACK_TEST;

//! Hardware Device command type
typedef union _DWC_DEV_CMD {
  DWC_DEV_CMD_TEMPLATE                Template;
  DWC_DEV_CMD_SET_PERIODIC_PARAMS     SetPeriodicParams;
  DWC_DEV_CMD_SET_SCRATCHPAD_BUF_LO   SetScratchpadBufLo;
  DWC_DEV_CMD_SET_SCRATCHPAD_BUF_HI   SetScratchpadBufHi;
  DWC_DEV_CMD_TX_DEVICE_NOTIFY        TxDeviceNotify;
  DWC_DEV_CMD_SELECTED_FIFO_FLUSH     SelFifoFlush;
  DWC_DEV_CMD_ALL_FIFO_FLUSH          AllFifoFlush;
  DWC_DEV_CMD_SET_EP_NRDY             SetEPNrdy;
  DWC_DEV_CMD_RUN_SOC_LOOPBACK_TEST   RunSocLoopbackTest;
} DWC_DEV_CMD;

//
// endpoint command structures
//
typedef struct _DWC_EP_CMD_REG {
  UINT32                    CmdTyp              :4;
  UINT32                    Rsvd0               :4;
  UINT32                    CmdIOC              :1;
  UINT32                    Rsvd1               :1;
  UINT32                    CmdAct              :1;
  UINT32                    HiPri_ForceRM       :1;
  UINT32                    CmdStatus           :4;
  UINT32                    CmdParam            :16;
} DWC_EP_CMD_REG;

typedef struct _DWC_EP_CMD_TEMPLATE {
  UINT32                    Parameter2;
  UINT32                    Parameter1;
  UINT32                    Parameter0;
  union {
    UINT32                  Dword;
    DWC_EP_CMD_REG          Reg;
  } Cmd;
} DWC_EP_CMD_TEMPLATE;

typedef struct _DWC_EP_CMD_NO_PARAMETERS {
  UINT32                    Rsvd0;

  UINT32                    Rsvd1;

  UINT32                    Rsvd2;

  DWC_EP_CMD_REG            Cmd;
} DWC_EP_CMD_NO_PARAMETERS;

typedef struct _DWC_EP_CMD_SET_EP_CONFIG {
  UINT32                    RestoreEpState;

  UINT32                    IntrNum             :5;
  UINT32                    Rsvd1               :3;
  UINT32                    XferCmplEn          :1;
  UINT32                    XferInProgEn        :1;
  UINT32                    XferNRdyEn          :1;
  UINT32                    Rsvd2               :1;
  UINT32                    Rsvd3               :1;
  UINT32                    StrmEvtEn           :1;
  UINT32                    Rsvd4               :1;
  UINT32                    EBC                 :1;
  UINT32                    bInterval_m1        :8;
  UINT32                    StrmCap             :1;
  UINT32                    UsbEpNum            :5;
  UINT32                    BulkBased           :1;
  UINT32                    FifoBased           :1;

  UINT32                    Rsvd5               :1;
  UINT32                    EPType              :2;
  UINT32                    MaxPacketSz         :11;
  UINT32                    Rsvd6               :3;
  UINT32                    FifoNum             :5;
  UINT32                    BrstSiz             :4;
  UINT32                    Rsvd7               :4;
  UINT32                    ConfigAction        :2;

  DWC_EP_CMD_REG            Cmd;
} DWC_EP_CMD_SET_EP_CONFIG;

typedef struct _DWC_EP_CMD_SET_EP_XFER_RSC_CONFIG {
  UINT32                    Rsvd0;

  UINT32                    Rsvd1;

  UINT32                    NumXferRes          :16;
  UINT32                    Rsvd2               :16;

  DWC_EP_CMD_REG            Cmd;
} DWC_EP_CMD_SET_EP_XFER_RSC_CONFIG;

typedef DWC_EP_CMD_NO_PARAMETERS    DWC_EP_CMD_GET_EP_STATE;

typedef DWC_EP_CMD_NO_PARAMETERS    DWC_EP_CMD_SET_STALL;

typedef DWC_EP_CMD_NO_PARAMETERS    DWC_EP_CMD_CLEAR_STALL;

typedef struct _DWC_EP_CMD_START_XFER {
  UINT32                    Rsvd0;

  UINT32                    TDAddressLo;

  UINT32                    TDAddressHi;

  DWC_EP_CMD_REG            Cmd;
} DWC_EP_CMD_START_XFER;

typedef DWC_EP_CMD_NO_PARAMETERS    DWC_EP_CMD_UPDATE_XFER;

typedef DWC_EP_CMD_NO_PARAMETERS    DWC_EP_CMD_END_XFER;

typedef DWC_EP_CMD_NO_PARAMETERS    DWC_EP_CMD_START_NEW_CONFIG;

typedef union _DWC_EP_CMD {
  DWC_EP_CMD_TEMPLATE                 Template;
  DWC_EP_CMD_SET_EP_CONFIG            SetEPConfig;
  DWC_EP_CMD_SET_EP_XFER_RSC_CONFIG   SetEPXferRscConfig;
  DWC_EP_CMD_GET_EP_STATE             GetEPState;
  DWC_EP_CMD_SET_STALL                SetStall;
  DWC_EP_CMD_CLEAR_STALL              ClearStall;
  DWC_EP_CMD_START_XFER               StartXfer;
  DWC_EP_CMD_UPDATE_XFER              UpdateXfer;
  DWC_EP_CMD_END_XFER                 EndXfer;
  DWC_EP_CMD_START_NEW_CONFIG         StartNewConfig;
} DWC_EP_CMD;

//
// events
//
typedef struct _DWC_EVT_TEMPLATE {
  UINT32                    IsDeviceEvent       :1;
  UINT32                    Rsvd0               :31;
  UINT64                    Rsvd1;
  VOID                      *Payload;
} DWC_EVT_TEMPLATE;

typedef struct _DWC_EVT_DEV {
  UINT32                    IsDeviceEvent       :1;
  UINT32                    DevSpecific         :7;
  UINT32                    EventType           :4;
  UINT32                    Rsvd0               :4;
  UINT32                    EventInfo           :9;
  UINT32                    Rsvd1               :7;
  UINT64                    LmpData;
  VOID                      *Payload;
} DWC_EVT_DEV, *PDWC_EVT_DEV;

typedef struct _DWC_EVT_EP {
  UINT32                    IsDeviceEvent       :1;
  UINT32                    PhysEPNum           :5;
  UINT32                    EventType           :4;
  UINT32                    Rsvd                :2;
  UINT32                    EventStatus         :4;
  UINT32                    EventParam          :16;
  UINT64                    Rsvd2;
  VOID                      *Payload;
} DWC_EVT_EP, *PDWC_EVT_EP;

typedef union _DWC_EVT {
  DWC_EVT_TEMPLATE          Template;
  DWC_EVT_DEV               Device;
  DWC_EVT_EP                Endpoint;
} DWC_EVT, *PDWC_EVT;

//
// other types
//
typedef UINT8  DWC_ENDPOINT_TYPE_MASK;


/**
 * @brief Dump a set of hardware registers
 *
 * Dump RegCount registers from the RegsToDump array of {offset, description} pairs.
 *
 * An example definition of an array of registers to dump for this function follows:
 *
 * @code
 * STATIC DWC_REG_DESC gDumpRegs[] =
 * {
 *   { DWC_OFFS(DCTL),            "DCTL           " },
 *   { DWC_OFFS(GSBUSCFG0),       "GSBUSCFG0      " },
 *   { DWC_OFFS(GSBUSCFG1),       "GSBUSCFG1      " },
 *   { DWC_OFFS(GTXTHRCFG),       "GTXTHRCFG      " },
 *   { DWC_OFFS(GRXTHRCFG),       "GRXTHRCFG      " },
 *   { DWC_OFFS(GUID),            "GUID           " },
 *   { DWC_OFFSI(GUSB2PHYCFG,0),  "GUSB2PHYCFG[0] " },
 *   { DWC_OFFSI(GUSB3PIPECTL,0), "GUSB3PIPECTL[0]" },
 *   { DWC_OFFSI(GTXFIFOSIZ,0),   "GTXFIFOSIZ[0]  " },
 *   { DWC_OFFSI(GTXFIFOSIZ,1),   "GTXFIFOSIZ[1]  " },
 *   { DWC_OFFSI(GTXFIFOSIZ,2),   "GTXFIFOSIZ[2]  " },
 *   { DWC_OFFSI(GTXFIFOSIZ,3),   "GTXFIFOSIZ[3]  " },
 *   { DWC_OFFSI(GRXFIFOSIZ,0),   "GRXFIFOSIZ[0]  " },
 *   { DWC_OFFS(GCTL),            "GCTL           " },
 *   { DWC_OFFS(DCFG),            "DCFG           " },
 *   { DWC_OFFS(DEVTEN),          "DEVTEN         " },
 *   { DWC_OFFS(DALEPENA),        "DALEPENA       " },
 *   { DWC_OFFS(DSTS),            "DSTS           " }
 * };
 * @endcode
 *
 * @param [in] Dev          Device instance pointer
 * @param [in] RegsToDump   Array of register descriptions
 * @param [in] RegCount     Number of elements in RegsToDump
 */
VOID
DwcDumpRegs (
  IN  DWC_DEVICE            *Dev,
  IN  DWC_REG_DESC          RegsToDump[],
  IN  UINT32                RegCount
  );

/**
 * @brief Initialize an instance of the software device
 *
 * Initialize an instance of the software device.  This function <b>MUST</b>
 * be called before any other Dwc function and does not configure the hardware
 * in any way.
 *
 * @param [out] Dev           Pointer to instance of device to initialize
 * @param [in]  BaseAddr      Base physical address of the controller core
 *
 * @return DWC_SUCCESS        Initialization successful
 * @return others             Failed to initialize
 */
DWC_STATUS
DwcInitialize (
  OUT DWC_DEVICE            *Dev,
  IN  UINTN                 BaseAddr
  );

/**
 * @brief Clean up a device as part of stopping the driver
 *
 * Clean up data referenced by the device and free memory allocated internally
 * by the common layer functions.  This function should be called prior to the
 * freeing of any memory passed as buffers to common layer functions and after
 * the controller has been stopped.  After calling this function on a device
 * no other common layer functions may be called without a complete
 * reinitialization.
 *
 * @param [in] Dev          The device to clean up
 *
 * @return DWC_SUCCESS      The device and any buffers are ready to be freed
 * @return others           Failed to clean up internal data
 */
DWC_STATUS
DwcCleanDevice (
  IN DWC_DEVICE             *Dev
  );

/**
 * @brief Get USB core version information
 *
 * Query the USB3.0 controller core for version information.
 *
 * @param [in] Dev          Device instance pointer
 * @param [out]Info         Version information pointer
 *
 * @return DWC_SUCCESS      Version informaiton is valid
 * @return others           Error occurred
 */
DWC_STATUS
DwcGetCoreInfo (
  IN  DWC_DEVICE     *Dev,
  OUT DWC_CORE_INFO  *Info 
  );

/**
 * @brief Configure an event buffer
 *
 * An event buffer must be a multiple of 4 bytes long and be aligned to the
 * size of the event buffer.  The index of the event buffer used is returned
 * for later use with DwcConfigureEP.  Event buffer index 0 is always used
 * for device events regardless of any endpoints that may send events to the
 * same buffer.
 *
 * @param [in]  Dev             Device instance pointer
 * @param [in]  EvtBuffer       Address of memory allocated for the event buffer
 * @param [in]  EvtBufferSize   Size in bytes of EvtBuffer
 * @param [in]  EnableInterrupt Enable/disable the hardware event interrupt
 * @param [out] EvtBufferIndex  Index of event buffer allocated
 *
 * @return DWC_SUCCESS          Event buffer configured
 * @return others               Failed to configure event buffer
 */
DWC_STATUS
DwcConfigureEventBuffer (
  IN  DWC_DEVICE            *Dev,
  IN  DWC_ADDRESS           EvtBuffer,
  IN  UINT32                EvtBufferSize,
  IN  BOOLEAN               EnableInterrupt,
  OUT UINT8                 *EvtBufferIndex
  );

/**
 * @brief Configure a USB endpoint
 *
 * Configure an endpoint in the software device for a particular direction of
 * traffic and set up buffer space for TRBs on this endpoint.  The TRB buffer
 * must be aligned to 16 byte boundary and be a multiple of 16 bytes long.
 * DwcConfigureEventBuffer must be called prior to this function to allocate
 * an event buffer.  An index returned from DwcConfigureEventBuffer must be
 * passed to this function to associate events from this endpoint to that event
 * buffer.  Any number of endpoints can use the same event buffer if desired.
 *
 * NOTE: This function does <b>NOT</b> execute any endpoint commands and should
 * only be called once to set up the TRB buffer and establish the mapping
 * between an event buffer and an endpoint. This function <b>MUST</b> be called
 * after DwcConfigureEventBuffer.
 *
 * @param [in] Dev              Device instance pointer
 * @param [in] Ep               The logical USB endpoint number to configure and enable
 * @param [in] EpDir            Direction for the endpoint
 * @param [in] MaxPktSize       Max packet size of the endpoint
 * @param [in] EvtBufferIndex   Index of event buffer to associate with this ep
 * @param [in] TrbBuffer        Address of buffer space for TRBs on this endpoint
 * @param [in] TrbBufferSize    Size in bytes of TrbBuffer
 *
 * @return DWC_SUCCESS          Endpoint configured
 * @return others               Failed to configure endpoint
 */
DWC_STATUS
DwcConfigureEP (
  IN  DWC_DEVICE            *Dev,
  IN  UINT8                 Ep,
  IN  UINT8                 EpDir,
  IN  UINT32                MaxPktSize,
  IN  UINT8                 EvtBufferIndex,
  IN  DWC_ADDRESS           TrbBuffer,
  IN  UINT32                TrbBufferSize
  );

/**
 * @brief Update the maximum packet size associated with the endpoint
 *
 * Update the maximum packet size associated with the endpoint
 *
 * @param [in] Dev              Device instance pointer
 * @param [in] Ep               The locical USB endpoint number to configure and enable
 * @param [in] EpDir            Direction for the endpoint
 * @param [in] MaxPktSize       Max packet size of the endpoint
 *
 * @return DWC_SUCCESS          Endpoint configured
 * @return others               Failed to update the maximum packet size for the endpoint
 */
DWC_STATUS
DwcUpdateEPMaxPktSize(
    IN  DWC_DEVICE            *Dev,
    IN  UINT8                 Ep,
    IN  UINT8                 EpDir,
    IN  UINT32                MaxPktSize
);

/**
 * @brief Set the stall state of an endpoint
 *
 * Routine to set the stall state of a logical USB endpoint.  This function must be
 * called to set the stall state rather than executing the endpoint command manually
 * in order for the common layer to keep the endpoint state up to date.
 *
 * @param [in] Dev              Device instance pointer
 * @param [in] Ep               The locical USB endpoint number to configure and enable
 * @param [in] EpDir            Direction for the endpoint
 * @param [in] IsStall          True to stall, false to clear stall
 *
 * @return DWC_SUCCESS
 * @return others
 */
DWC_STATUS
DwcSetEndpointStallState(
  IN  DWC_DEVICE            *Dev,
  IN  UINT8                 Ep,
  IN  UINT8                 EpDir,
  IN  BOOLEAN               IsStall
);

/**
 * @brief Get the endpoint state
 *
 * Get the state of the endpoint
 *
 * @param [in]  Dev             Device instance pointer
 * @param [in]  Ep              The logical USB endpoint number to query state
 * @param [in]  EpDir           Direction for the endpoint
 * @param [out] EpState         State of specified endpoint
 *
 * @return DWC_SUCCESS          PhysicalAddress is valid
 * @return others               Operation failed
 */
DWC_STATUS
DwcGetEPState (
  IN  DWC_DEVICE            *Dev,
  IN  UINT8                 Ep,
  IN  UINT8                 EpDir,
  OUT DWC_EP_STATE          *EpState
);

/**
 * @brief One-time preparation of an URB for a USB transaction
 *
 * Initialize an URB structure for a new USB transaction.  This function should
 * only be called once to start a transaction with specific properties.  The
 * DwcPrepareURB function must be called after this to populate the URB with
 * data buffers for the transfer.
 *
 * @param [in]     Dev                     Device instance pointer
 * @param [in,out] Urb                     URB to be initialized for transaction
 * @param [in]     Ep                      Logical USB endpoint number of transaction
 * @param [in]     EpDir                   Direction of transfer on endpoint
 * @param [in]     TrbCtl                  Value for the TRBCTL field of the TRBs
 * @param [in]     StreamID                Stream ID if streams enabled, otherwise zero
 * @param [in]     TotalTransactionLength  Length of overall transaction (possibly multiple SGLists)
 * @param [in]     UseZlt                  Use Zero-Length-Termination for transaction
 *
 * @return DWC_SUCCESS                     Urb has been initialized
 * @return others                          Urb is invalid
 */
DWC_STATUS
DwcPrepareURB (
  IN     DWC_DEVICE         *Dev,
  IN OUT DWC_URB            *Urb,
  IN     UINT8              Ep,
  IN     UINT8              EpDir,
  IN     UINT8              TrbCtl,
  IN     UINT16             StreamID,
  IN     UINT32             TotalTransactionLength,
  IN     BOOLEAN            UseZlt
  );

/**
 * @brief Update an Urb with a new Scatter/Gather list
 *
 * Take an URB that has been initialized <b>once per transaction</b> with DwcPrepareURB,
 * and update its buffer descriptor with information from the supplied scatter/gather
 * list.  A call to this function should be followed by a call to DwcSubmitURB to
 * notify the hardware of the changes.  This process may be repeated with the same
 * urb as many times as necessary to fully complete the transaction.  If TRUE was specified
 * for UzeZlt in the call to DwcPrepareUrb for an OUT Endpoint, the SGList's last element
 * must contain a max packet sized buffer.
 *
 * @param [in]     Dev      Device instance pointer
 * @param [in,out] Urb      URB to be initialized for transfer
 * @param [in]     SGList   Scatter/gather list describing data buffers
 *
 * @return DWC_SUCCESS      Urb has been initialized
 * @return others           Urb is invalid
 */
DWC_STATUS
DwcUpdateURB (
  IN     DWC_DEVICE             *Dev,
  IN OUT DWC_URB                *Urb,
  IN     SCATTER_GATHER_LIST    *SGList
  );

/**
 * @brief Submit an URB for transfer
 *
 * Submit a previously prepared URB (from DwcUpdateURB) to the hardware for
 * transfer.  This function can be called to start a new transaction with the
 * controller stopped (i.e. the RunStop bit is clear).  Depending on the state
 * of the URB this function will either execute a StartXfer or UpdateXfer
 * endpoint command.  After successful execution of this function an endpoint
 * command complete event will be generated.
 *
 * @param [in] Dev          Device instance pointer
 * @param [in] Urb          URB to submit
 *
 * @return DWC_SUCCESS      Urb has been submitted to the hardware
 * @return others           Failed to submit Urb
 */
DWC_STATUS
DwcSubmitURB (
  IN  DWC_DEVICE            *Dev,
  IN  DWC_URB               *Urb
  );

/**
 * @brief Cancel the transfer on the endpoint
 *
 * Cancel the pending transfer on the endpoint specified, if possible.
 * After successful execution of this function an endpoint command complete
 * event will be generated in response to the EndXfer command used to
 * cancel the transfer.
 *
 * @param [in] Dev          Device instance pointer
 * @param [in] Ep           Logical USB endpoint number
 * @param [in] EpDir        USB endpoint direction
 *
 * @return DWC_SUCCESS      Pending transfers have been canceled
 * @return others           Failed to cancel transfers
 */
DWC_STATUS
DwcCancelTransferByEp (
  IN  DWC_DEVICE            *Dev,
  IN  UINT8                 Ep,
  IN  UINT8                 EpDir
 );

/**
 * @brief Cancel the transfer on the endpoint
 *
 * Same as DwcCancelTransferByEp with the USB endpoint and direction
 * specified in the Urb used.
 *
 * @param [in] Dev          Device instance pointer
 * @param [in] Urb          Urb to cancel
 *
 * @return DWC_SUCCESS      Pending Transfers have been canceled
 * @return others           Failed to cancel transfers
 */
DWC_STATUS
DwcCancelTransferByUrb (
  IN  DWC_DEVICE            *Dev,
  IN  DWC_URB               *Urb
 );

/**
 * @brief Cancel all the active transfers except physical endpoint 0 & 1
 *
 * Cancel all the active transfers except physical endpoint 0 & 1.
 *
 * @param [in] Dev          Device instance pointer
 *
 * @return DWC_SUCCESS      All pending transfers have been canceled
 * @return others           Failed to cancel at least one pending transfer
 */
DWC_STATUS
DwcCancelAllTransfers (
  IN  DWC_DEVICE            *Dev
  );

/**
 * @brief Execute a device command
 *
 * Execute a device command by writing the appropriate hardware registers.
 * A command completion event will be sent by the hardware with the execution
 * status.
 *
 * @param [in] Dev           Device instance pointer
 * @param [in] Cmd           DWC_DEV_CMD initialized for a specific command
 */
DWC_STATUS
DwcWriteDeviceCommand (
  IN DWC_DEVICE             *Dev,
  IN DWC_DEV_CMD            *Cmd
  );

/**
 * @brief Execute an endpoint command
 *
 * Execute an endpoint command for a particular USB endpoint number and
 * direction by writing the appropriate hardware registers.  A command
 * completion event will be sent by the hardware with the execution status.
 *
 * @param [in] Dev           Device instance pointer
 * @param [in] Ep            Logical USB endpoint number
 * @param [in] EpDir         Endpoint direction
 * @param [in] Cmd           DWC_EP_CMD initialized for a specific command
 */
DWC_STATUS
DwcWriteEndpointCommand (
  IN DWC_DEVICE             *Dev,
  IN UINT8                  Ep,
  IN UINT8                  EpDir,
  IN DWC_EP_CMD             *Cmd
  );

/**
 * @brief Get the number of unread events
 *
 * Get the number of unread events in an event buffer for the device.  The
 * endpoint and its direction implicity index the desired event buffer based
 * on the EvtBufferIndex passed to DwcConfigureEP.  The constant
 * "DWC_EP_NUM_DEVICE_EVENT" is used to reference the event buffer used for
 * device events.  This function <b>MUST</b> be called and return a value
 * greater than 0 before DwcGetNextEvent can be called to actually retrieve
 * an event (this is mandated by the hardware).
 *
 * @param [in]  Dev             Device instance pointer
 * @param [in]  Ep              Endpoint of the desired event buffer
 * @param [in]  EpDir           Endpoind direction of the desired event buffer
 * @param [out] EvtCount        Number of pending events in buffer
 *
 * @return DWC_SUCCESS          Returned EvtCount is valid
 * @return others               Failed to get event count for event buffer
 */
DWC_STATUS
DwcGetEventCount (
  IN  DWC_DEVICE            *Dev,
  IN  UINT8                 Ep,
  IN  UINT8                 EpDir,
  OUT UINT32                *EvtCount
  );

/**
 * @brief Retrieve an event
 *
 * Get the next pending event from an event buffer.  The endpoint and direction
 * are used as in DwcGetEventCount, which <b>MUST</b> be called and return a
 * value greater than 0 before this function can be used to retrieve an event
 * (this is mandated by the hardware).
 *
 * @param [in]  Dev             Device instance pointer
 * @param [in]  Ep              Endpoint of the desired event buffer
 * @param [in]  EpDir           Endpoint direction of the desired event buffer
 * @param [out] Evt             Next unread event from event buffer
 *
 * @return DWC_SUCCESS          Returned Evt is valid
 * @return others               Failed to get event count for event buffer
 */
DWC_STATUS
DwcGetNextEvent (
  IN  DWC_DEVICE            *Dev,
  IN  UINT8                 Ep,
  IN  UINT8                 EpDir,
  OUT DWC_EVT               *Evt
  );


/**
 * @brief Dump the TRB ring associated with an endpoint
 *
 * Dump the TRB ring data structure associated with an endpoint
 *
 * @param [in] Dev          Device instance pointer
 * @param [in] Ep           Logical USB endpoint number
 * @param [in] EpDir        USB endpoint direction
 *
 */
VOID
DwcDumpTrbRing (
  IN  DWC_DEVICE            *Dev,
  IN  UINT8                 Ep,
  IN  UINT8                 EpDir
  );

/**
 * @brief Dump the TRB ring and current TRB associated with an endpoint
 *
 * Dump the TRB ring data structure and current TRB pointer associated with an
 * endpoint.
 *
 * @param [in] Dev          Device instance pointer
 * @param [in] Ep           Logical USB endpoint number
 * @param [in] EpDir        USB endpoint direction
 * @param [in] Id           Location in the code the funciont is called
 * @param [in] Trb          Current Trb
 */
VOID
DwcDumpTrbRingEx (  
  IN  DWC_DEVICE            *Dev,
  IN  UINT8                 Ep,
  IN  UINT8                 EpDir,
  IN  UINT32                Id,
  IN  DWC_TRB               *Trb
  );

/**
 * @brief Reset endpoint state to DWC_EP_STATE_UNINITIALIZED
 *
 * The function reset the state of the non control endpoints back DWC_EP_STATE_UNINITIALIZED
 * as the endpoint has not yet been initialized. 
 *
 * @param [in] Dev          The device whose ep state to be reset
 *
 * @return DWC_SUCCESS      The ep state has been reset correctly.
 * @return others           Failed to clean up internal data
 */
DWC_STATUS
DwcDeconfigureNonCtrlEPs (
  IN DWC_DEVICE             *Dev
  );
  
#endif /* _DWC_COMMON_H_ */

