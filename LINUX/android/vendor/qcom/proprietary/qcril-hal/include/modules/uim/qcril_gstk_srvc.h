/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include "framework/legacy.h"
#include "modules/uim/GstkRilRequestMessage.h"


/*===========================================================================
                   CONSTANTS
===========================================================================*/
#define QMI_CAT_ENVELOPE_DATA_MAX_LENGTH      258
#define QMI_CAT_TERMINAL_RESPONSE_MAX_LENGTH  255
#define QMI_CAT_TERMINAL_PROFILE_MAX_LENGTH   80
#define QMI_CAT_SCWS_DATA_MAX_LENGTH          1000
#define QMI_CAT_TERMINAL_RESPONSE_MAX_LENGTH  255
#define QMI_CAT_RAW_PROACTIVE_CMD_MAX_LENGTH  258
#define QMI_CAT_RAW_ENV_RSP_DATA_MAX_LENGTH   255
#define QMI_CAT_DCS_ENCODED_STRING_MAX_LENGTH 255

/* COMMAND TYPE INFORMATION
** STK command values
** Refernece - Type of Command Section 8.6 and 9.4 3GPP 31.111 v 3.3.
**
** This is used in the command details to indicate what proactive command is
** being issued from SIM/USIM to the mobile.
*/
#define QCRIL_GSTK_QMI_CMD_STK_REFRESH                0x01
#define QCRIL_GSTK_QMI_CMD_STK_MORE_TIME              0x02
#define QCRIL_GSTK_QMI_CMD_STK_POLL_INTERVAL          0x03
#define QCRIL_GSTK_QMI_CMD_STK_POLLING_OFF            0x04
#define QCRIL_GSTK_QMI_CMD_STK_SET_UP_EVENT_LIST      0x05
#define QCRIL_GSTK_QMI_CMD_STK_SET_UP_CALL            0x10
#define QCRIL_GSTK_QMI_CMD_STK_SEND_SS                0x11
#define QCRIL_GSTK_QMI_CMD_STK_USSD                   0x12
#define QCRIL_GSTK_QMI_CMD_STK_SEND_SHORT_MESSAGE     0x13
#define QCRIL_GSTK_QMI_CMD_STK_SEND_DTMF              0x14
#define QCRIL_GSTK_QMI_CMD_STK_LAUNCH_BROWSER         0x15
#define QCRIL_GSTK_QMI_CMD_STK_PLAY_TONE              0x20
#define QCRIL_GSTK_QMI_CMD_STK_DISPLAY_TEXT           0x21
#define QCRIL_GSTK_QMI_CMD_STK_GET_INKEY              0x22
#define QCRIL_GSTK_QMI_CMD_STK_GET_INPUT              0x23
#define QCRIL_GSTK_QMI_CMD_STK_SELECT_ITEM            0x24
#define QCRIL_GSTK_QMI_CMD_STK_SET_UP_MENU            0x25
#define QCRIL_GSTK_QMI_CMD_STK_PROVIDE_LOCAL_INFO     0x26
#define QCRIL_GSTK_QMI_CMD_STK_TIMER_MANAGEMENT       0x27
#define QCRIL_GSTK_QMI_CMD_STK_SET_UP_IDLE_MODE_TEXT  0x28
#define QCRIL_GSTK_QMI_CMD_STK_LANG_NOTIFICATION      0x35
#define QCRIL_GSTK_QMI_CMD_STK_OPEN_CHANNEL           0x40
#define QCRIL_GSTK_QMI_CMD_STK_CLOSE_CHANNEL          0x41
#define QCRIL_GSTK_QMI_CMD_STK_RECEIVE_DATA           0x42
#define QCRIL_GSTK_QMI_CMD_STK_SEND_DATA              0x43
#define QCRIL_GSTK_QMI_CMD_STK_GET_CHANNEL_STATUS     0x44
#define QCRIL_GSTK_QMI_CMD_STK_ACTIVATE               0x70
#define QCRIL_GSTK_QMI_CMD_STK_CONTACTLESS_STATE      0x71
#define QCRIL_GSTK_QMI_CMD_STK_END_OF_PROACTIVE_SES   0x81

/* not supported  */
#define QCRIL_GSTK_QMI_CMD_STK_PERFORM_CARD_APDU      0x30
#define QCRIL_GSTK_QMI_CMD_STK_POWER_ON_CARD          0x31
#define QCRIL_GSTK_QMI_CMD_STK_POWER_OFF_CARD         0x32
#define QCRIL_GSTK_QMI_CMD_STK_GET_READER_STATUS      0x33
#define QCRIL_GSTK_QMI_CMD_STK_RUN_AT_COMMAND         0x34

/* Index for invalid slot */
#define QCRIL_GSTK_INVALID_SLOT_INDEX_VALUE      0xFF

#define QCRIL_GSTK_MAX_CARD_COUNT                3

/* Mandatory TLV sizes to help validate proactive command data */
#define QCRIL_GSTK_QMI_COMMAND_PROLOGUE_SIZE    2
#define QCRIL_GSTK_QMI_COMMAND_DETAILS_SIZE     5
#define QCRIL_GSTK_QMI_DEVICE_IDENTITIES_SIZE   4

#define QCRIL_GSTK_QMI_COMMAND_MIN_SIZE         (QCRIL_GSTK_QMI_COMMAND_PROLOGUE_SIZE + \
                                                 QCRIL_GSTK_QMI_COMMAND_DETAILS_SIZE  + \
                                                 QCRIL_GSTK_QMI_DEVICE_IDENTITIES_SIZE)

#define QCRIL_GSTK_QMI_RETURN_IF_OUT_OF_RANGE(length,min,max)               \
  if ((length < min) || (length > max))                                     \
  {                                                                         \
    QCRIL_LOG_ERROR("Length out ot range: 0x%x, discarding TLV", length);   \
    return FALSE;                                                           \
  }                                                                         \


/*===========================================================================
                     ENUMS
===========================================================================*/
/*===========================================================================
  QMI CAT end point module Request ids
===========================================================================*/
typedef enum
{
  QCRIL_GSTK_REQUSET_SET_EVENT_REPORT,
  QCRIL_GSTK_REQUSET_CHECK_CONFIGURATION,
  QCRIL_GSTK_REQUEST_SEND_ENVELOPE_COMMAND,
  QCRIL_GSTK_REQUEST_SEND_TERMINAL_RESPONSE,
  QCRIL_GSTK_REQUEST_SEND_EVENT_CONFIRMATION,
  QCRIL_GSTK_REQUEST_SCWS_OPEN_CHANNEL,
  QCRIL_GSTK_REQUEST_SCWS_CLOSE_CHANNEL,
  QCRIL_GSTK_REQUEST_SCWS_SEND_DATA,
  QCRIL_GSTK_REQUEST_SCWS_DATA_AVAILABLE,
  QCRIL_GSTK_REQUEST_SCWS_CHANNEL_STATUS,
  QCRIL_GSTK_REQUEST_GET_CACHED_CMD
} qcril_gstk_request_type;

/*===========================================================================
  QMI CAT Response ids
===========================================================================*/
typedef enum
{
  QMI_CAT_SRVC_INVALID_RSP_MSG,
  QMI_CAT_SRVC_SEND_ENVELOPE_COMMAND_RSP_MSG,
  QMI_CAT_SRVC_SEND_TERMINAL_RESPONSE_RSP_MSG,
  QMI_CAT_SRVC_SCWS_DATA_AVAILABLE_RSP_MSG,
} qmi_cat_rsp_id_type;

/*===========================================================================
  QMI slots
===========================================================================*/
typedef enum
{
  QMI_CAT_SLOT1 = 0x01,
  QMI_CAT_SLOT2 = 0x02,
  QMI_CAT_SLOT3 = 0x03,
  QMI_CAT_SLOT4 = 0x04,
  QMI_CAT_SLOT5 = 0x05
} qmi_cat_slot_type;

/*===========================================================================
  Event confiramtion types
===========================================================================*/
typedef enum
{
  QMI_CAT_DISPLAY,
  QMI_CAT_CONFIRM
} qmi_cat_evt_cnf_type;

/*===========================================================================
  SCWS channel states
===========================================================================*/
typedef enum
{
  QMI_CAT_SCWS_CHANNEL_CLOSED_STATE      = 0x00,
  QMI_CAT_SCWS_CHANNEL_LISTEN_STATE      = 0x01,
  QMI_CAT_SCWS_CHANNEL_ESTABLISHED_STATE = 0x02
} qmi_cat_scws_channel_state_type;

/*===========================================================================
  Cached command ids
===========================================================================*/
typedef enum
{
  QMI_CAT_CACHED_COMMAND_ID_SETUP_MENU       = 0x01, /**<  Setup menu  */
  QMI_CAT_CACHED_COMMAND_ID_SETUP_EVENT_LIST = 0x02, /**<  Setup event list  */
  QMI_CAT_CACHED_COMMAND_ID_SETUP_IDLE_TEXT  = 0x03  /**<  Setup Idle mode text  */
} qmi_cat_cached_command_id_type;

/*===========================================================================
  Indication command ids
===========================================================================*/
typedef enum
{
  QMI_CAT_SRVC_UP_IND_MSG,
  QMI_CAT_SRVC_DOWN_IND_MSG,
  QMI_CAT_EVENT_REPORT_IND,
  QMI_CAT_SEND_TR_IND,
  QMI_CAT_SEND_EVENLOPE_CMD_IND,
  QMI_CAT_SCWS_OPEN_CHANNEL_IND,
  QMI_CAT_SCWS_CLOSE_CHANNEL_IND,
  QMI_CAT_SCWS_SEND_DATA_IND
} qmi_cat_indication_id_type;

/*===========================================================================
  Alpha DC type
===========================================================================*/
typedef enum
{
  QMI_CAT_DCS_7BIT_GSM  = 0x00, 
  QMI_CAT_DCS_8BIT_GSM  = 0x01, 
  QMI_CAT_DCS_8BIT_UCS2 = 0x02,
} qmi_cat_alpha_dcs_type;

/*===========================================================================
                     STRUCTURES/UNIONS
===========================================================================*/
/*===========================================================================
  GSTK orignal request data type
===========================================================================*/
typedef struct
{
  std::shared_ptr<GstkRilRequestMessage> req_ptr;
  qcril_gstk_request_type                qmi_req_id;
  bool                                   is_scws;                                
  union
  {
    uint32_t                             bip_id;
  }                                      data;
} qcril_gstk_original_request_type;

/*===========================================================================
  Envelope command request data type
===========================================================================*/
typedef struct
{
  qmi_cat_slot_type      slot_id;
  uint32_t               token;
  uint32_t               envelope_data_len;
  uint8_t                envelope_data[QMI_CAT_ENVELOPE_DATA_MAX_LENGTH];
} qcril_gstk_request_envelope_cmd_type;

/*===========================================================================
  Terminal response request data type
===========================================================================*/
typedef struct
{
  qmi_cat_slot_type  slot_id;
  uint32_t           uim_ref_id;
  uint32_t           token;
  uint32_t           terminal_response_len;
  uint8_t            terminal_response[QMI_CAT_TERMINAL_RESPONSE_MAX_LENGTH];
} qmi_cat_terminal_response_type;

/*===========================================================================
  Event confirmation request data type
===========================================================================*/
typedef struct
{
  qmi_cat_slot_type     slot;
  qmi_cat_evt_cnf_type  evt_data;
  boolean               cnf_data;
} qmi_cat_event_confirmation_req_msg;

/*===========================================================================
  SCWS channel info data type
===========================================================================*/
typedef struct
{
  qmi_cat_slot_type               slot;
  uint32_t                        ch_id;
  qmi_cat_scws_channel_state_type channel_status;
} qmi_cat_scws_channel_info_msg;

/*===========================================================================
  SCWS send data request data type
===========================================================================*/
typedef struct
{
  qmi_cat_slot_type               slot;
  uint32_t                        ch_id;
  uint8_t                         result;
} qmi_cat_scws_send_data_req_msg;

/*===========================================================================
  SCWS data available request data type
===========================================================================*/
typedef struct
{
  qmi_cat_slot_type     slot;
  uint32_t              ch_id;
  uint32_t              data_len;
  uint8_t               data[QMI_CAT_SCWS_DATA_MAX_LENGTH];
  uint16_t              remaining_data_len;
} qmi_cat_scws_data_available_type;

/*===========================================================================
  Cached proactive command request data type
===========================================================================*/
typedef struct
{
  qmi_cat_slot_type                  slot;
  qmi_cat_cached_command_id_type     cmd_id;
} qcril_gstk_request_cached_cmd_type;

/*===========================================================================
 Set event report request data types
===========================================================================*/
typedef struct
{
  uint8_t             slot;
  uint32_t            evt_mask;
  uint32_t            dec_evt_mask;
} qcril_gstk_request_set_evt_rpt_type;

/*===========================================================================
  QMI CAT end point module request data strucutre
===========================================================================*/
typedef union
{
  qcril_gstk_request_envelope_cmd_type   env_data;
  qmi_cat_terminal_response_type         tr_data;
  qmi_cat_event_confirmation_req_msg     evt_cnf_data;
  qmi_cat_scws_send_data_req_msg         send_data;
  qmi_cat_scws_channel_info_msg          ch_data;
  qmi_cat_scws_data_available_type       data_avail;
  qcril_gstk_request_cached_cmd_type     cached_cmd;
  qcril_gstk_request_set_evt_rpt_type    evt_rpt;
} qcril_gstk_request_data_type;

/*===========================================================================
  Envelope response data type
===========================================================================*/
typedef struct
{
  uint8_t  sw1;
  uint8_t  sw2;
  uint32_t env_resp_data_len;
  uint8_t  env_resp_data[QMI_CAT_RAW_ENV_RSP_DATA_MAX_LENGTH];
} qmi_cat_envelope_resp_type; 

/*===========================================================================
  Terminal response response data type
===========================================================================*/
typedef struct
{
  uint8_t  sw1;
  uint8_t  sw2;
  uint32_t tr_response_len;
  uint8_t  tr_response[QMI_CAT_TERMINAL_RESPONSE_MAX_LENGTH];
} qmi_cat_terminal_resp_type;

/*===========================================================================
  Cached Proactive command response data type
===========================================================================*/
typedef struct
{
  uint32_t                         cmd_len;
  uint8_t                          cmd_data[QMI_CAT_RAW_PROACTIVE_CMD_MAX_LENGTH];
} qmi_cat_cached_cmd_resp_type;

/*===========================================================================
  Async callback response data structure
===========================================================================*/
typedef struct
{
  int                                               transp_err;
  int                                               qmi_err;
  qmi_cat_rsp_id_type                               rsp_id;
  std::shared_ptr<qcril_gstk_original_request_type> orig_ptr;
  union
  {
   qmi_cat_terminal_resp_type               tr_rsp_data;
   qmi_cat_envelope_resp_type               env_rsp_data;
  }                                         rsp_data;
} qmi_cat_rsp_data_type;

/*===========================================================================
  Sync response data structure
===========================================================================*/
typedef struct
{
  int                                       qmi_err_code;
  union
  {
    qmi_cat_cached_cmd_resp_type             cached_rsp_data;
  } rsp_data;
} qmi_cat_sync_rsp_data_type;

/*===========================================================================
  Event report message structure
===========================================================================*/
typedef struct
{
  boolean           is_end_session;
  uint32_t          unique_ref_id;
  uint32_t          cmd_len;
  uint8_t           cmd_type;
  uint8_t           cmd_data[QMI_CAT_RAW_PROACTIVE_CMD_MAX_LENGTH];
} qmi_cat_evt_rpt_ind_msg;

/*===========================================================================
  SCWS OPEN Channel indication message structure
===========================================================================*/
typedef struct
{
  uint32_t                ch_id;
  uint16_t                port;
  uint16_t                buffer_size;
  qmi_cat_alpha_dcs_type  dcs;
  uint32_t                text_len;
  uint8_t                 text[QMI_CAT_DCS_ENCODED_STRING_MAX_LENGTH];
} qmi_cat_scws_open_ch_ind_msg;

/*===========================================================================
  SCWS CLOSE Channel indication message structure
===========================================================================*/
typedef struct
{
  uint32_t                         ch_id;
  qmi_cat_scws_channel_state_type  state;
} qmi_cat_scws_close_ch_ind_msg;

/*===========================================================================
  SCWS Send data indication message structure
===========================================================================*/
typedef struct
{
  uint32_t ch_id;
  uint8_t total_packets;
  uint8_t current_packet;
  uint32_t data_len;
  uint8_t data[QMI_CAT_SCWS_DATA_MAX_LENGTH];
} qmi_cat_scws_send_data_ind_msg;

/*===========================================================================
  Indication params data structure
===========================================================================*/
typedef struct
{
  uint8_t                      slot;
  union
  {
    qmi_cat_evt_rpt_ind_msg        evt_rpt;
    qmi_cat_scws_open_ch_ind_msg   scws_open_ch;
    qmi_cat_scws_close_ch_ind_msg  scws_close_ch;
    qmi_cat_scws_send_data_ind_msg scws_send_data;
  } ind_data;
} qcril_gstk_qmi_ind_params_type;

