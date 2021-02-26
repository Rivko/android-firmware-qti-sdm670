#ifndef DIAGCOMM_CTRL_H 
#define DIAGCOMM_CTRL_H
/*==========================================================================

              Diagnostic Communications for Diag Control Channel

General Description

Copyright (c) 2010-2017 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

===========================================================================*/

/*===========================================================================

                              Edit History

 $Header: //components/rel/core.qdsp6/2.1.c4/services/diag/DCM/common/src/diagcomm_ctrl.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
05/30/17   gn      Changes to support command deregistration
05/05/17   ph      Configure buffering modes for user PD send buffers.
03/03/17   sp      Removed packed void * parameters
01/25/17   rs      Re-try sending control packets using bit mask and signal and by-passing the control heap usage.
10/24/16   gn      Changes to support common PW for ADSP, CDSP and SLPI
07/22/16   gn      Removal of pointer passing from peripheral processors to APPS processor
05/12/16   ph      SMD references removal and code clean up.
04/08/15   ps      Diag over sockets and DSM removal 
06/18/15   sa      Added support for time sync feature.
07/06/14   ph      Added support for Diag over STM
04/10/14   sr      Support for peripheral buffering mode  
04/01/14   rs      Added support for build mask centralization.
11/22/13   sr      Removed PACK from pointer references 
02/11/13   sr      Mainling Diag central routing  
01/03/13   sg      Command Response Channel separation
03/02/12   sg      Added function to return the channel connected status
                   for the ctrl port 
11/15/12   is      Support for preset masks
08/29/12   sa      Added max supported tx/rx chained packet size
03/02/12   sg      Added function to return the channel connected status
                   for the ctrl port
10/07/11   is      Support sending log mask per equipment id
09/19/11   is      Optimized sending F3 "set mask" requests from Master Diag
09/15/11   is      Modifed "set mask" via CTRL msgs fot EA support
09/12/11   is      Master Diag forwards "set mask" requests via CTRL msgs
08/11/11   wjg     Added function to handle processing DIAG_CTRL_MSG_SIG 
04/18/11   is      Ensure CTRL msg sizes are constant
03/04/11   is      Support for Diag over SMD-Lite
12/12/10   mad     Added DIAG_CTRL_MSG_DIAGDATA and associated structures for dual mask.
                   Added diagcomm_ctrl_set_port_id().
10/12/10   mad     Changed diag_ctrl_msg_diagmode_type structure 
09/21/10   mad     Added message to control diag sleep-vote, buffering 
                   modality etc:  
09/14/10   is      New control channel and central routing features 

===========================================================================*/

#include "comdef.h"
#include "diagcomm_io.h"     /* For diagcomm_io_conn_type, diagcomm_enum_port_type */
#include "diaglogi.h"        /* For EQUIP_ID_MAX */
#include "diagpkti.h"
#include "servreg_locator.h"

/* ------------------------------------------------------------------------
** Typedefs and defines
** ------------------------------------------------------------------------ */

#define DIAG_CTRL_MSG_REG            1   // Message passing for registration commands
#define DIAG_CTRL_MSG_DTR            2   // Message passing for DTR events
#define DIAG_CTRL_MSG_DIAGMODE       3   // Control Diag sleep vote, buffering etc
#define DIAG_CTRL_MSG_DIAGDATA       4   // Diag data based on "light" diag mask
#define DIAG_CTRL_MSG_LOG_MASK       5   // Deprecated
#define DIAG_CTRL_MSG_EVENT_MASK     6   // Deprecated
#define DIAG_CTRL_MSG_F3_MASK        7   // Deprecated
#define DIAG_CTRL_MSG_FEATURE        8   // Send diag internal feature mask 'diag_int_feature_mask'
#define DIAG_CTRL_MSG_EQUIP_LOG_MASK 9   // Send Diag log mask for a particular equip id (Still sent by HLOS Apps)
#define DIAG_CTRL_MSG_EVENT_MASK_V2  10  // Send Diag event mask (Still sent by HLOS Apps)
#define DIAG_CTRL_MSG_F3_MASK_V2     11  // Send Diag F3 mask (Still sent by HLOS Apps)
#define DIAG_CTRL_MSG_NUM_PRESETS    12  // Send # of preset masks supported
#define DIAG_CTRL_MSG_SET_CUR_PRESET 13  // Send current preset index to use
#define DIAG_CTRL_MSG_EQUIP_LOG_MASK_V3 14  // Upgrade of DIAG_CTRL_MSG_EQUIP_LOG_MASK supporting presets
#define DIAG_CTRL_MSG_EVENT_MASK_V3  15  // Upgrade of DIAG_CTRL_MSG_EVENT_MASK_V2 supporting presets 
#define DIAG_CTRL_MSG_F3_MASK_V3     16  // Upgrade of DIAG_CTRL_MSG_F3_MASK_V2 supporting presets
#define DIAG_CTRL_MSG_BUF_TX_MODE    17  // Set streaming vs buffering modes
#define DIAG_CTRL_MSG_BUF_DRAIN      18  // Drain bufering pool immediately
#define DIAG_CTRL_MSG_BUF_SET_WM_PER 19  // Set high and low wm percentages for buffering modes
#define DIAG_CTRL_MSG_STM_PKT        21  // Change the mode of Diag to route traffic over STM.
#define DIAG_CTRL_BT_EVENT_ID        22  // Report last Event-ID supported for build-mask centralization
#define DIAG_CTRL_BT_LOG_RANGE       23  // Report log code ranges supported for build-mask centralization
#define DIAG_CTRL_BT_MSG_RANGE       24  // Report msg ssid ranges supported for build-mask centralization
#define DIAG_CTRL_BT_MSG_MASK        25  // Report all msg build time masks supported for build-mask centralization
#define DIAG_CTRL_MSG_DRAIN_STATUS   26  // Buffering mode drain status notification
#define DIAG_CTRL_MSG_DEREG          27  // Message passing for registration commands
#define DIAG_CTRL_MSG_PERIPH_BUF_SIZE        28   // Send buffer size used for Peripheral buffering
#define DIAG_CTRL_MSG_TIME_API       31  //Update the time API to save the time in diag packets.
#define DIAG_CTRL_PD_STATUS          30
#define DIAG_CTRL_MSG_DIAGID         33

#define DIAG_CTRL_MSG_REG_VER              1   // Version # for CTRL_MSG_REG
#define DIAG_CTRL_MSG_DTR_VER              1   // Version # for CTRL_MSG_DTR
#define DIAG_CTRL_MSG_DIAGMODE_VER_1       1   // Version # for DIAG_CTRL_MSG_MODE
#define DIAG_CTRL_MSG_DIAGMODE_VER_2       2   // Version # for DIAG_CTRL_MSG_MODE
#define DIAG_CTRL_MSG_DIAGDATA_VER         1   // Version # for DIAG_CTRL_MSG_DIAGDATA
#define DIAG_CTRL_MSG_BT_MASK_VER          1   // Version # for BT mask centralization                                               
#define DIAG_CTRL_MSG_BUF_TX_MODE_VER_1    1   // Version # for DIAG_CTRL_MSG_BUF_TX_MODE
#define DIAG_CTRL_MSG_BUF_TX_MODE_VER_2    2   // Version # for DIAG_CTRL_MSG_BUF_TX_MODE
#define DIAG_CTRL_MSG_BUF_DRAIN_VER_1      1   // Version # for DIAG_CTRL_MSG_BUF_DRAIN
#define DIAG_CTRL_MSG_BUF_DRAIN_VER_2      2   // Version # for DIAG_CTRL_MSG_BUF_DRAIN
#define DIAG_CTRL_MSG_BUF_SET_WM_PER_VER_1 1   // Version # for DIAG_CTRL_MSG_BUF_SET_WM_PER
#define DIAG_CTRL_MSG_BUF_SET_WM_PER_VER_2 2   // Version # for DIAG_CTRL_MSG_BUF_SET_WM_PER
#define DIAG_CTRL_MSG_DRAIN_STATUS_VER     1   // Version # for drain status notification
#define DIAG_CTRL_MSG_DIAG_STM_VER         1   // Version # for DIAG_CTRL_MSG_STM_PKT
#define DIAG_CTRL_MSG_DIAGID_VER           1
#define DIAG_CTRL_MSG_DEREG_VER            1   // Version # for CTRL_MSG_DEREG

#define DIAG_CTRL_MASK_INVALID            0
#define DIAG_CTRL_MASK_ALL_DISABLED       1
#define DIAG_CTRL_MASK_ALL_ENABLED        2
#define DIAG_CTRL_MASK_VALID              3

/* Proc IDs used in control packets */
#define DIAG_CTRL_MODEM_PROC              0
#define DIAG_CTRL_ADSP_PROC               1
#define DIAG_CTRL_WCNSS_PROC              2
#define DIAG_CTRL_SLPI_PROC               3
#define DIAG_CTRL_WDSP_PROC               4
#define DIAG_CTRL_CDSP_PROC               5
#define DIAG_CTRL_ALL_PROC                0xFF

#define DIAG_MAX_CHAINED_DSM_ITEM         8


#define DIAG_SET_CTRLMASK_BIT(X)		(diag_resend_ctrlpkt_mask |= (1 << X))    // To set ctrlmask bit
#define DIAG_CLEAR_CTRLMASK_BIT(X)		(diag_resend_ctrlpkt_mask &= ~(1 << X))	 // To clear ctrlmask bit
#define DIAG_IS_CTRLMASK_BIT_SET(X)		(diag_resend_ctrlpkt_mask & (1 << X))     // to check if particular bit (X) in ctrlmask is set (i.e. 1)

/* defines for bit positions of control packets to re-send in the mask variable

  Bit Mapping of bit mask to a control packet. Keeping it consistent among various codebases.
  Mapping with none means other code base has that position filled coressponding to particular 
  a control packet which is not present in this codebase

*/
#define DIAG_CTRL_MSG_FEATURE_BIT_POS 		0
#define DIAG_CTRL_MSG_NUM_PRESETS_BIT_POS   1
#define DIAG_CTRL_MSG_REG_BIT_POS			2
#define DIAG_CTRL_MSG_DEREG_BIT_POS			3
#define DIAG_CTRL_BT_EVENT_ID_BIT_POS    	4
#define DIAG_CTRL_BT_LOG_RANGE_BIT_POS 		5
#define DIAG_CTRL_BT_MSG_RANGE_BIT_POS 		6
#define DIAG_CTRL_BT_MSG_MASK_BIT_POS 		7
#define DIAG_CTRL_PD_STATUS_BIT_POS 		8
#define DIAG_CTRL_MSG_PERIPH_BUF_BIT_POS    9

/******************************************************************************
  Bit-mask variable used for re-sending the control packets.
  we have 13 control pkts as of now that need to be resent whenever required.

  Bit Mapping 0f bit mask to a control packet:

  Bit  - Control Packet
  0    - DIAG_CTRL_MSG_FEATURE_BIT_POS	
  1    - DIAG_CTRL_MSG_NUM_PRESETS_BIT_POS
  2    - DIAG_CTRL_MSG_REG_BIT_POS
  3    - DIAG_CTRL_MSG_DEREG_BIT_POS
  4    - DIAG_CTRL_BT_EVENT_ID_BIT_POS 
  5    - DIAG_CTRL_BT_LOG_RANGE_BIT_POS 
  6    - DIAG_CTRL_BT_MSG_RANGE_BIT_POS
  7    - DIAG_CTRL_BT_MSG_MASK_BIT_POS
  8    - DIAG_CTRL_PD_STATUS_BIT_POS


********************************************************************************/
extern uint16 diag_resend_ctrlpkt_mask;   
 
typedef PACK(struct)
{
  word cmd_code_lo;                   // Minimum Command code value
  word cmd_code_hi;                   // Maximum Command code value
} diag_ctrl_msg_user_table_entry_type;

typedef PACK(struct)
{
  uint32 version;                      // Version # of this msg type
  uint16 cmd_code;                     // The cmd code
  word subsysid;                       // The subsystem ID
  word count;                          // # of cmd entries
  uint16 port;                         // The smd port (ie: remote proc) to route request to
} diag_ctrl_msg_reg_type;

typedef PACK(struct)
{
  uint32 version;                      // Version # of this msg type
  uint16 cmd_code;                     // The cmd code
  word subsysid;                       // The subsystem ID
  word count;                          // # of cmd entries
  diag_ctrl_msg_user_table_entry_type * user_tbl;
} diag_ctrl_msg_dereg_type;

typedef PACK(struct)
{
  uint32 version;                      // Version # of this msg type
  boolean dte_ready_asserted;          // DTR ready state
} diag_ctrl_msg_dtr_type;

typedef PACK(struct)
{
   uint32 version;               /* Version # of this msg type is 1 */
   uint32 sleep_vote;            /* Set to zero to disable diag sleep voting, 1 to enable diag sleep voting */
   uint32 real_time;             /* Set to zero to disable real-time logging (i.e. prevent frequent APPS wake-ups etc),
                                          1 to enable real-time logging */
   uint32 use_nrt_values;        /* Can be used for testing. If set, the values below are used to tune diag.
                                          If zero, those values are ignored and diag chooses its own parameters */
   uint32 commit_threshold;
   uint32 sleep_threshold;
   uint32 sleep_time;
   uint32 drain_timer_val;
   uint32 event_stale_timer_val;
} diag_ctrl_msg_diagmode_type;

typedef PACK(struct)
{
   uint32 version;               /* Version # of this msg type is 2*/
   uint32 sleep_vote;            /* Set to zero to disable diag sleep voting, 1 to enable diag sleep voting */
   uint32 real_time;             /* Set to zero to disable real-time logging (i.e. prevent frequent APPS wake-ups etc),
                                          1 to enable real-time logging */
   uint32 use_nrt_values;        /* Can be used for testing. If set, the values below are used to tune diag.
                                          If zero, those values are ignored and diag chooses its own parameters */
   uint32 commit_threshold;
   uint32 sleep_threshold;
   uint32 sleep_time;
   uint32 drain_timer_val;
   uint32 event_stale_timer_val;
   uint8  diagID;                      /* diagID of the peripheral/PD */
} diag_ctrl_msg_diagmode_v2_type;

typedef PACK(struct)
{
   uint32 version;                     /* Version # of this message */
   uint32 ctrl_data_stream_num;        /* if later we need more than one control data stream */
} diag_ctrl_msg_diagdata_type;

typedef PACK(struct)
{
  uint8 stream_id;
  uint32 log_mask_size;               /* LOG_MASK_SIZE */
  byte log_mask[LOG_MASK_SIZE];       /* The log mask */
} diag_ctrl_msg_log_mask_type;

typedef PACK(struct)
{
  uint8 stream_id;
  uint8 status;
  uint8 event_config;                                 /* Event reporting config indicator */
  uint32 event_mask_size;                             /* EVENT_MASK_SIZE */
  //unsigned char event_mask[EVENT_MASK_SIZE];        /* The event mask */
} diag_ctrl_msg_event_mask_type;

typedef PACK(struct)
{
  uint8 stream_id;
  uint8 preset_id;
  uint8 status;
  uint8 event_config;                                 /* Event reporting config indicator */
  uint32 event_mask_size;                             /* EVENT_MASK_SIZE */
  //unsigned char event_mask[EVENT_MASK_SIZE];        /* The event mask */
} diag_ctrl_msg_event_mask_v3_type;

typedef PACK(struct)
{
  uint8 stream_id;
  uint8 status;
  uint8 msg_mode;                                      
  uint16 ssid_first;                                  /* Start of range of supported SSIDs */
  uint16 ssid_last;                                   /* Last SSID in range */
  uint32 msg_mask_size;                               /* Count of rt_mask_array (ssid_last - ssid_first + 1) */
  //uint32 *rt_mask_array;                            /* Array of (ssid_last - ssid_first + 1) masks */
} diag_ctrl_msg_f3_mask_type;

typedef PACK(struct)
{
  uint8 stream_id;
  uint8 preset_id;
  uint8 status;
  uint8 msg_mode;                                      
  uint16 ssid_first;                                  /* Start of range of supported SSIDs */
  uint16 ssid_last;                                   /* Last SSID in range */
  uint32 msg_mask_size;                               /* Count of rt_mask_array (ssid_last - ssid_first + 1) */
  //uint32 *rt_mask_array;                            /* Array of (ssid_last - ssid_first + 1) masks */
} diag_ctrl_msg_f3_mask_v3_type;

typedef PACK(struct)
{
  uint32 mask_len;                                   /* Length of 'mask' (DIAG_INT_FEATURE_MASK_LEN) */
  byte mask[DIAG_INT_FEATURE_MASK_LEN];              /* Diag internal feature mask */
} diag_ctrl_msg_feature_type;

typedef PACK(struct)
{
  uint8 stream_id;
  uint8 status;
  uint8 equip_id;
  uint32 last_item;                   /* Last log code for this equip_id */
  uint32 log_mask_size;               /* Size of log mask stored in log_mask[] */
  //byte log_mask[log_mask_size];     /* The log mask for equip_id */
} diag_ctrl_msg_equip_log_mask_type;

typedef PACK(struct)
{
  uint8 stream_id;
  uint8 preset_id;
  uint8 status;
  uint8 equip_id;
  uint32 last_item;                   /* Last log code for this equip_id */
  uint32 log_mask_size;               /* Size of log mask stored in log_mask[] */
  //byte log_mask[log_mask_size];     /* The log mask for equip_id */
} diag_ctrl_msg_equip_log_mask_v3_type;

typedef PACK(struct)
{
  uint8 num_presets;
} diag_ctrl_msg_num_presets_type;

typedef PACK(struct)
{
  uint8 preset_id;
} diag_ctrl_msg_cur_preset_type;

typedef PACK(struct)
{
  uint32 version;                      // Version of this msg type is 1
  uint8 stream_id;
  uint8 mode;
} diag_ctrl_msg_buf_tx_mode_type;

typedef PACK(struct)
{
  uint32 version;                      // Version of this msg type is 2
  uint8 diag_id;
  uint8 stream_id;
  uint8 mode;
} diag_ctrl_msg_buf_tx_mode_v2_type;

typedef PACK(struct)
{
  uint32    version;                      // Version # of this msg type 1
  uint8 stream_id;
} diag_ctrl_msg_buf_drain_type;

typedef PACK(struct)
{
  uint32    version;                      // Version # of this msg type 2
  uint8     diag_id;
  uint8     stream_id;
} diag_ctrl_msg_buf_drain_v2_type;

typedef PACK(struct)
{
  uint32 version;                      // Version # of this msg type 1
  uint8 stream_id;
  uint8 high_wm_value;                 // High watermark percentage value
  uint8 low_wm_value;                  // Low watermark percentage value
} diag_ctrl_msg_buf_wm_value_type;

typedef PACK(struct)
{
  uint32 version;                      // Version # of this msg type 2
  uint8  diag_id;
  uint8  stream_id;
  uint8  high_wm_value;                 // High watermark percentage value
  uint8  low_wm_value;                  // Low watermark percentage value
} diag_ctrl_msg_buf_wm_value_v2_type;


/* Build time last event id reporting packet format (22) */
typedef PACK(struct)
{
    uint32  version;             /* version number of this control packet type */
    uint16  event_last_id;       /* Last Event Id supported by the peripheral */

}diag_ctrl_event_last_id_type;

/* Definition of LOG_RANGES */
typedef PACK(struct)
{
    uint32 equip_id;       /* Equipment ID */
    uint32 num_items;      /* Number of items supported for this Equipment ID */

} log_range_type;


/* Build time log range reporting packet format (23) */
typedef PACK(struct)
{
    uint32      version;             /* version number of this control packet type */
    uint32      log_last_equipid;
    uint32      num_equipid_ranges;  /* Number of Equip ID ranges reported in this packet */
    log_range_type  ranges[LOG_EQUIP_ID_LAST + 1];  /* Array of EquipID and supported log-code range: NUM_RANGES * 8 */
} diag_ctrl_log_range_type;

/* Definition of msg SSID_RANGES */
typedef PACK(struct)
{
    uint16 ssid_first;
    uint16 ssid_last;
} msg_ssid_range_type;

/* Build time msg SSID range reporting packet format (24) */
/* This is used for sending only Msg SSID ranges. It doesn't include build time masks */
typedef PACK(struct)
{
    uint32          version;                /* version number of this control packet type */
    uint32          num_ssid_ranges;        /* Number of msg SSID ranges reported in this packet */
    msg_ssid_range_type  ssid_ranges[MSG_MASK_TBL_CNT];         /* Array of SSID range reported in this packet */
} diag_ctrl_bt_msg_ssid_range_type;

/* Build time msg mask reporting packet format (25) */
/* This is used for sending build mask for Msg SSID's. Each SSID range uses the below packet
   format for sending the build mask */
typedef PACK(struct)
{
    uint32          version;            /* version number of this control packet type */
    uint32          range_cnt;          /* Number of msg SSID ranges for which build mask is reported in this packet */
    msg_ssid_range_type  ssid_range;    /* SSID Range and Build Mask */
   /* Followed by build mask for above SSID_RANGE */
} diag_ctrl_bt_msg_mask_type;

typedef PACK(struct)
{
   uint32          version;            /* version number of this control packet type */
   uint8           peripheral_id;      /* peripheral id - Modem 0, ADSP 1, WCNSS 2*/
} diag_ctrl_msg_drain_status_type;

typedef PACK(struct)
{
  uint32 version;                     /* version number of this control packet type */
  uint32 size;                        /* size of the memory buffer used for peripheral buffering */
} diag_ctrl_msg_periph_buf_size_type;

typedef PACK(struct)
{
  uint32  version;
  uint8   ctrl_mode;
}diag_ctrl_msg_diag_stm_pkt_type;

typedef PACK(struct)
{
  uint32 version; /* Version number of this control packet */ 
  uint8 time_api;
}diag_ctrl_msg_time_api_type;

typedef PACK(struct)
{
  uint32 version;                       // Version # of this msg type
  uint32 diagID;                        // diagid for the process
  //char process_name[];                // Place holder for NULL terminated string with process name
} diag_ctrl_msg_diagID_type;



typedef PACK(union)
{
  diag_ctrl_msg_reg_type ctrl_msg_reg;                             /* DIAG_CTRL_MSG_REG */
  diag_ctrl_msg_dtr_type ctrl_msg_dtr;                             /* DIAG_CTRL_MSG_DTR */
  diag_ctrl_msg_diagmode_type ctrl_msg_diagmode;                   /* DIAG_CTRL_MSG_DIAGMODE */
  diag_ctrl_msg_diagmode_v2_type ctrl_msg_diagmode_v2;             /* DIAG_CTRL_MSG_DIAGMODE V2 */
  diag_ctrl_msg_diagdata_type ctrl_msg_diagdata;                   /* DIAG_CTRL_MSG_DIAGDATA */
  diag_ctrl_msg_event_mask_type ctrl_msg_event_mask;               /* DIAG_CTRL_MSG_EVENT_MASK_V2 */
  diag_ctrl_msg_f3_mask_type ctrl_msg_f3_mask;                     /* DIAG_CTRL_MSG_F3_MASK_V2 */
  diag_ctrl_msg_feature_type ctrl_msg_feature;                     /* DIAG_CTRL_MSG_FEATURE */
  diag_ctrl_msg_equip_log_mask_type ctrl_msg_equip_log_mask;       /* DIAG_CTRL_MSG_EQUIP_LOG_MASK */
  diag_ctrl_msg_num_presets_type ctrl_msg_num_presets;             /* DIAG_CTRL_MSG_NUM_PRESETS */
  diag_ctrl_msg_cur_preset_type cur_preset;                        /* DIAG_CTRL_MSG_SET_CUR_PRESET */
  diag_ctrl_msg_event_mask_v3_type ctrl_msg_event_mask_v3;         /* DIAG_CTRL_MSG_EVENT_MASK_V3 */
  diag_ctrl_msg_equip_log_mask_v3_type ctrl_msg_equip_log_mask_v3; /* DIAG_CTRL_MSG_EQUIP_LOG_MASK_V3 */
  diag_ctrl_msg_f3_mask_v3_type ctrl_msg_f3_mask_v3;               /* DIAG_CTRL_MSG_F3_MASK_V3 */
  diag_ctrl_msg_buf_tx_mode_type ctrl_msg_buf_mode;                /* DIAG_CTRL_MSG_BUF_TX_MODE */
  diag_ctrl_msg_buf_tx_mode_v2_type ctrl_msg_buf_mode_v2;          /* DIAG_CTRL_MSG_BUF_TX_MODE V2 */
  diag_ctrl_msg_buf_drain_type ctrl_msg_buf_drain;                 /* DIAG_CTRL_MSG_BUF_DRAIN */
  diag_ctrl_msg_buf_drain_v2_type ctrl_msg_buf_drain_v2;           /* DIAG_CTRL_MSG_BUF_DRAIN V2 */
  diag_ctrl_msg_buf_wm_value_type ctrl_msg_buf_wm_value;           /* DIAG_CTRL_MSG_BUF_SET_WM_PER */
  diag_ctrl_msg_buf_wm_value_v2_type ctrl_msg_buf_wm_value_v2;     /* DIAG_CTRL_MSG_BUF_SET_WM_PER V2*/
  diag_ctrl_event_last_id_type ctrl_msg_bt_event_last_id;          /* DIAG_CTRL_EVENT_LAST_ID */
  diag_ctrl_log_range_type	ctrl_msg_bt_log_range;                 /* DIAG_CTRL_LOG_RANGE */
  diag_ctrl_bt_msg_ssid_range_type ctrl_bt_msg_ssid_range;         /* DIAG_CTRL_BT_MSG_SSID_RANGE */
  diag_ctrl_bt_msg_mask_type ctrl_bt_msg_mask;                     /* DIAG_CTRL_BT_MSG_MASK */
  diag_ctrl_msg_dereg_type ctrl_msg_dereg;                           /* DIAG_CTRL_MSG_DEREG */
  diag_ctrl_msg_diag_stm_pkt_type ctrl_msg_stm_mode;
  diag_ctrl_msg_drain_status_type ctrl_drain_status;               /* DIAG_CTRL_MSG_DRAIN_STATUS */
  diag_ctrl_msg_periph_buf_size_type ctrl_msg_mem_buf_size;         /* DIAG_CTRL_MSG_PERIPH_BUF_SIZE */
  diag_ctrl_msg_time_api_type    ctrl_msg_time_api;                /* DIAG_CTRL_SET_TIME_API */  
  diag_ctrl_msg_diagID_type ctrl_msg_diagID;                         /* DIAG_CTRL_MSG_DIAGID*/ 
} diag_ctrl_msg_data_type;

#define DIAGCOMM_CTRL_MSG_HDR_SIZE 8   /* Size of cmd_type + data_len in bytes */
typedef PACK(struct)
{
  uint32 cmd_type;
  uint32 data_len;
  diag_ctrl_msg_data_type data;
} diag_ctrl_msg_type;

typedef enum
{
   DIAG_CTRL_USER_PKT_ID_0 = 0, /*corresponds to void* user_pkt*/
   DIAG_CTRL_PKT_USER_CMD_REG = 1, /*corresponds to diagcomm_ctrl_lsm_cmd_reg_type struct*/
   DIAG_CTRL_PKT_CORE_CMD_REG = 2, /*corresponds to diag_ctrl_user_tbl_type struct*/
}diag_ctrl_user_pkt_type;

typedef struct{
   uint16 count;
   diagpkt_master_table_q_type *q_item;
}diagcomm_ctrl_lsm_cmd_reg_type;

typedef enum
{
    DIAG_CTRL_INFO_FWD_STATUS_NOT_SENT = 0,
    DIAG_CTRL_INFO_FWD_STATUS_QUEUED = 1,
    DIAG_CTRL_INFO_FWD_STATUS_SENT = 2,
	DIAG_CTRL_INFO_FWD_STATUS_DEREGISTERED = 3,
} diag_ctrl_info_fwd_status;

/* ------------------------------------------------------------------------
** Function prototypes
** ------------------------------------------------------------------------ */

/* Generic - CTRL specific */
uint16 diagcomm_ctrl_port_id( diagcomm_port_type port_type, diagcomm_enum_port_type ctrl_channel );
boolean diagcomm_ctrl_status( diagcomm_port_type port_type, diagcomm_enum_port_type ctrl_channel );
boolean diagcomm_ctrl_channel_status( diagcomm_port_type port_type, diagcomm_enum_port_type ctrl_channel );
boolean diagcomm_ctrl_open( diagcomm_port_type port_type, diagcomm_enum_port_type ctrl_channel );
void diagcomm_ctrl_close( diagcomm_port_type port_type, diagcomm_enum_port_type ctrl_channel );
void diagcomm_ctrl_init( diagcomm_port_type port_type, diagcomm_enum_port_type port_num );

void diagcomm_process_ctrl( void );


void diagcomm_ctrl_send_num_presets( void );
uint32
diagcomm_ctrl_send_mult_pkt( void * ctrl_pkt, 
                             uint32 ctrl_pktlen, 
                             const void * user_pkt, 
                             uint32 user_pktlen,
			                 diag_ctrl_user_pkt_type user_pkt_type,
                                    diagcomm_port_type port_type,
                                    diagcomm_enum_port_type ctrl_channel, 
                             boolean send_now,
                             boolean * is_sent );

void diagpkt_process_ctrl_msg( void * ctrl_pkt, uint32 ctrl_pkt_len,
                               diagcomm_port_type port_type,
                               diagcomm_enum_port_type ctrl_channel );
                               
void diag_ctrl_send_log_range_ctrl_pkt(void);

void diagcomm_ctrl_send_buf_retry(diagcomm_enum_port_type port_type);

#endif //DIAGCOMM_CTRL_H
