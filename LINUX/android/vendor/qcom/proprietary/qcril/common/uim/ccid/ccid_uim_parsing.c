
/*===========================================================================

  Copyright (c) 2011-2012, 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/02/12   at      Updated handling of ICC power on request
10/20/11   at      Initial version
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "ccid_uim_parsing.h"
#include "ccid_uim_daemon.h"


/*===========================================================================

                           MACROS

===========================================================================*/

/* Logging macros */
#define LOGI(...) fprintf(stderr, "INFO:  " __VA_ARGS__)
#define LOGE(...) fprintf(stderr, "ERROR: " __VA_ARGS__)

/* BULKOUT Message types */
#define CCID_UIM_MSG_BULKOUT_SET_PARAMS             0x61
#define CCID_UIM_MSG_BULKOUT_ICC_POWERON            0x62
#define CCID_UIM_MSG_BULKOUT_ICC_POWEROFF           0x63
#define CCID_UIM_MSG_BULKOUT_GET_SLOT_STATUS        0x65
#define CCID_UIM_MSG_BULKOUT_ESCAPE                 0x6B
#define CCID_UIM_MSG_BULKOUT_GET_PARAMS             0x6C
#define CCID_UIM_MSG_BULKOUT_XFR_BLOCK              0x6F
#define CCID_UIM_MSG_BULKOUT_ABORT                  0x72

/* BULKIN Message types */
#define CCID_UIM_MSG_BULKIN_DATABLOCK               0x80
#define CCID_UIM_MSG_BULKIN_SLOT_STATUS             0x81
#define CCID_UIM_MSG_BULKIN_PARAMS                  0x82
#define CCID_UIM_MSG_BULKIN_ESCAPE                  0x83

/* CTRL Message types */
#define CCID_UIM_MSG_CTRLIN_ABORT                   0x01
#define CCID_UIM_MSG_CTRLIN_GET_CLCK_FREQ           0x02
#define CCID_UIM_MSG_CTRLIN_GET_DATA_RATES          0x03

/* Fields for Slot status */
#define CCID_UIM_ICC_STATUS_ICC_PRESENT_ACTIVE      0x00
#define CCID_UIM_ICC_STATUS_ICC_PRESENT_INACTIVE    0x01
#define CCID_UIM_ICC_STATUS_NO_ICC_PRESENT          0x02
#define CCID_UIM_CMD_STATUS_NO_ERROR                0x00
#define CCID_UIM_CMD_STATUS_FAILED                  0x10
#define CCID_UIM_CMD_STATUS_TIME_EXTN               0x20

/* Fields for Slot error */
#define CCID_UIM_SLOT_ERR_NO_ERROR                  0x00
#define CCID_UIM_SLOT_ERR_CMD_NOT_SUPPORTED         0x00
#define CCID_UIM_SLOT_ERR_INCORRECT_PARAM_SLOT      0x05
#define CCID_UIM_SLOT_ERR_HW_ERROR                  0xFB
#define CCID_UIM_SLOT_ERR_ICC_MUTE                  0xFE

/* BULKIN sizes */
#define CCID_UIM_BULKIN_SIZE_PARAMS                  5
#define CCID_UIM_BULKIN_SIZE_COMMON                 10

/* Values for checking input APDU bytes */
#define CCID_UIM_APDU_VALUE_INS_BYTE_SELECT         0xA4
#define CCID_UIM_APDU_VALUE_INS_BYTE_STATUS         0xF2
#define CCID_UIM_APDU_VALUE_INS_BYTE_AUTH_EVEN      0x88
#define CCID_UIM_APDU_VALUE_INS_BYTE_AUTH_ODD       0x89
#define CCID_UIM_APDU_VALUE_INS_BYTE_GET_RESPONSE   0xC0
#define CCID_UIM_APDU_VALUE_PROCEDURE_BYTE_1        0x61
#define CCID_UIM_APDU_VALUE_NORMAL_SW1              0x90
#define CCID_UIM_APDU_VALUE_NORMAL_SW2              0x00

#define CCID_UIM_APDU_MIN_TOTAL_LEN_BYTES           0x04
#define CCID_UIM_APDU_LEN_SW_BYTES                  0x02

/*===========================================================================

                           GLOBALS

===========================================================================*/


/*===========================================================================

                               INTERNAL FUNCTIONS

===========================================================================*/

/*=========================================================================

  FUNCTION:  ccid_uim_allocate_cmd_info

===========================================================================*/
/*!
    @brief
      Allocated memory for a particular CCID command request.

    @return
      Pointer to the allocated command info.
*/
/*=========================================================================*/
static ccid_uim_cmd_info_type * ccid_uim_allocate_cmd_info
(
  uint8     req_msg_type,
  uint8     slot,
  uint8     sequence
)
{
  ccid_uim_cmd_info_type   * cmd_info_ptr = NULL;

  cmd_info_ptr = (ccid_uim_cmd_info_type*)malloc(sizeof(ccid_uim_cmd_info_type));
  if (cmd_info_ptr)
  {
    cmd_info_ptr->req_msg_type = req_msg_type;
    cmd_info_ptr->slot         = slot;
    cmd_info_ptr->sequence     = sequence;
  }

  return cmd_info_ptr;
} /* ccid_uim_allocate_cmd_info */


/*=========================================================================

  FUNCTION:  ccid_uim_convert_ccid_slot_to_qmi_slot

===========================================================================*/
/*!
    @brief
      Converts CCID slot info to QMI slot type.

    @return
      QMI Slot type.
*/
/*=========================================================================*/
static uim_slot_enum_v01 ccid_uim_convert_ccid_slot_to_qmi_slot
(
  unsigned char     ccid_slot
)
{
  uim_slot_enum_v01  qmi_slot_type = UIM_SLOT_ENUM_MAX_ENUM_VAL_V01;

  switch (ccid_slot)
  {
    case 0x00:
      qmi_slot_type = UIM_SLOT_1_V01;
      break;
    case 0x01:
      qmi_slot_type = UIM_SLOT_2_V01;
      break;
    default:
      LOGE("Unsupported slot: 0x%x \n", ccid_slot);
      break;
  }

  return qmi_slot_type;
} /* ccid_uim_convert_ccid_slot_to_qmi_slot */


/*=========================================================================

  FUNCTION:  ccid_uim_check_slot_error

===========================================================================*/
/*!
    @brief
      Checks if slot is in error. If yes, it updates the BULKIN response
      parameters appropriately.

    @return
      TRUE if error was detected, else FALSE.
*/
/*=========================================================================*/
static boolean ccid_uim_check_slot_error
(
  struct ccid_uim_bulk_out_header   * bulkout_req_ptr,
  struct ccid_uim_bulk_in_header    * bulkin_rsp_ptr
)
{
  uim_slot_enum_v01      qmi_slot_type = UIM_SLOT_ENUM_MAX_ENUM_VAL_V01;

  assert(bulkout_req_ptr != NULL);
  assert(bulkin_rsp_ptr != NULL);

  /* Convert to QMI slot type */
  qmi_slot_type = ccid_uim_convert_ccid_slot_to_qmi_slot(bulkout_req_ptr->slot);

  /* If error in parsing slot, update error response */
  if (((qmi_slot_type != UIM_SLOT_1_V01) && (qmi_slot_type != UIM_SLOT_2_V01)) ||
      (bulkout_req_ptr->slot >= QMI_UIM_CARDS_MAX_V01))
  {
    LOGE("Invalid input, qmi_slot_type: 0x%x, bulkout_req_ptr->slot: 0x%x\n",
         qmi_slot_type, bulkout_req_ptr->slot);
    bulkin_rsp_ptr->error  = CCID_UIM_SLOT_ERR_INCORRECT_PARAM_SLOT;
    bulkin_rsp_ptr->status = CCID_UIM_ICC_STATUS_NO_ICC_PRESENT |
                                CCID_UIM_CMD_STATUS_FAILED;
    return TRUE;
  }

  /* If card error for requested slot, update error response */
  if (ccid_uim.card_info[bulkout_req_ptr->slot].sap_state !=
        UIM_SAP_STATE_CONNECTED_SUCCESSFULLY_V01)
  {
    LOGE("sap_state is not connected: 0x%x \n",
         ccid_uim.card_info[bulkout_req_ptr->slot].sap_state);
    bulkin_rsp_ptr->error  = CCID_UIM_SLOT_ERR_ICC_MUTE;
    bulkin_rsp_ptr->status = CCID_UIM_ICC_STATUS_NO_ICC_PRESENT |
                                CCID_UIM_CMD_STATUS_FAILED;
    return TRUE;
  }

  return FALSE;
} /* ccid_uim_check_slot_error */


/*=========================================================================

  FUNCTION:  ccid_uim_handle_slot_status

===========================================================================*/
/*!
    @brief
      Handles slot status BULKOUT request.

    @return
     TRUE if immediate BULKIN response is to be sent, else FALSE.
*/
/*=========================================================================*/
static boolean ccid_uim_handle_slot_status
(
  struct ccid_uim_bulk_out_header   * bulkout_req_ptr,
  struct ccid_uim_bulk_in_header    * bulkin_rsp_ptr
)
{
  assert(bulkout_req_ptr != NULL);
  assert(bulkin_rsp_ptr != NULL);

  /* Check for slot param */
  if (ccid_uim_check_slot_error(bulkout_req_ptr, bulkin_rsp_ptr) == TRUE)
  {
    return TRUE;
  }

  /* Update successful parameters */
  bulkin_rsp_ptr->error   = CCID_UIM_SLOT_ERR_NO_ERROR;
  bulkin_rsp_ptr->status  = CCID_UIM_ICC_STATUS_ICC_PRESENT_ACTIVE |
                               CCID_UIM_CMD_STATUS_NO_ERROR;
  return TRUE;
} /* ccid_uim_handle_slot_status */


/*=========================================================================

  FUNCTION:  ccid_uim_handle_escape

===========================================================================*/
/*!
    @brief
      Handles escape BULKOUT request.

    @return
      TRUE if immediate BULKIN response is to be sent, else FALSE.
*/
/*=========================================================================*/
static boolean ccid_uim_handle_escape
(
  struct ccid_uim_bulk_out_header   * bulkout_req_ptr,
  struct ccid_uim_bulk_in_header    * bulkin_rsp_ptr
)
{
  assert(bulkout_req_ptr != NULL);
  assert(bulkin_rsp_ptr != NULL);

  /* Check for slot param */
  if (ccid_uim_check_slot_error(bulkout_req_ptr, bulkin_rsp_ptr) == TRUE)
  {
    return TRUE;
  }

  bulkin_rsp_ptr->error    = CCID_UIM_SLOT_ERR_CMD_NOT_SUPPORTED;
  bulkin_rsp_ptr->status   = CCID_UIM_ICC_STATUS_ICC_PRESENT_ACTIVE |
                              CCID_UIM_CMD_STATUS_FAILED;
  return TRUE;
} /* ccid_uim_handle_escape */


/*=========================================================================

  FUNCTION:  ccid_uim_handle_params

===========================================================================*/
/*!
    @brief
      Handles get/set parameters BULKOUT request.

    @return
      TRUE if immediate BULKIN response is to be sent, else FALSE.
*/
/*=========================================================================*/
static boolean ccid_uim_handle_params
(
  struct ccid_uim_bulk_out_header   * bulkout_req_ptr,
  struct ccid_uim_bulk_in_header    * bulkin_rsp_ptr
)
{
  assert(bulkout_req_ptr != NULL);
  assert(bulkin_rsp_ptr != NULL);

  /* Check for slot param */
  if (ccid_uim_check_slot_error(bulkout_req_ptr, bulkin_rsp_ptr) == TRUE)
  {
    return TRUE;
  }

  /* Update successful values */
  bulkin_rsp_ptr->error    = CCID_UIM_SLOT_ERR_NO_ERROR;
  bulkin_rsp_ptr->status   = CCID_UIM_ICC_STATUS_ICC_PRESENT_ACTIVE |
                              CCID_UIM_CMD_STATUS_NO_ERROR;

  /* We support only T=0 protocol */
  bulkin_rsp_ptr->specific = 0;
  bulkin_rsp_ptr->length   = CCID_UIM_BULKIN_SIZE_PARAMS;

  /* Hardcoded parameters currently, Todo:
     1. Parse ATR for the protocol data structure to be sent
     2. For set params, save it & send this new params when requested */
  bulkin_rsp_ptr->data[0]  = 0x01;
  bulkin_rsp_ptr->data[1]  = 0x00;
  bulkin_rsp_ptr->data[2]  = 0x00;
  bulkin_rsp_ptr->data[3]  = 0x0A;
  bulkin_rsp_ptr->data[4]  = 0x00;

  return TRUE;
} /* ccid_uim_handle_params */


/*=========================================================================

  FUNCTION:  ccid_uim_handle_sap_request

===========================================================================*/
/*!
    @brief
      Handles BULKOUT request for power on & transfer block. These are the
      2 requests that might need to be sent to modem. On successful cases,
      the BULKIN response will be sent in the QMI callback received.

    @return
      TRUE if immediate BULKIN response is to be sent, else FALSE.
*/
/*=========================================================================*/
static boolean ccid_uim_handle_sap_request
(
  struct ccid_uim_bulk_out_header   * bulkout_req_ptr,
  struct ccid_uim_bulk_in_header    * bulkin_rsp_ptr
)
{
  uint16                          i               = 0;
  boolean                         send_response   = FALSE;
  ccid_uim_cmd_info_type        * cmd_info_ptr    = NULL;
  uim_sap_request_req_msg_v01     sap_request_req;

  assert(bulkout_req_ptr != NULL);
  assert(bulkin_rsp_ptr != NULL);

  /* Check for slot param */
  if (ccid_uim_check_slot_error(bulkout_req_ptr, bulkin_rsp_ptr) == TRUE)
  {
    return TRUE;
  }

  /* Allocate command info pointer, it must be freed in the callback */
  cmd_info_ptr = ccid_uim_allocate_cmd_info(bulkout_req_ptr->message_type,
                                            bulkout_req_ptr->slot,
                                            bulkout_req_ptr->seq_number);
  if (cmd_info_ptr == NULL)
  {
    LOGE("Not able to allocate cmd_info_ptr !\n");
    return TRUE;
  }

  switch (bulkout_req_ptr->message_type)
  {
    case CCID_UIM_MSG_BULKOUT_ICC_POWERON:
      /* Power ON request is handled in 2 steps:
         1. Issue a SAP warm reset, synchronous call used here
         2. On a successful warm reset, fetch the ATR & return the response */
      memset(&sap_request_req, 0, sizeof(uim_sap_request_req_msg_v01));
      sap_request_req.sap_request.sap_request = UIM_SAP_REQUEST_RESET_SIM_V01;
      sap_request_req.sap_request.slot = ccid_uim_convert_ccid_slot_to_qmi_slot(
                                            bulkout_req_ptr->slot);

      /* Send response on error for any of the below QMI calls */
      send_response = TRUE;

      if(ccid_uim_send_sap_request(&sap_request_req, cmd_info_ptr, FALSE))
      {
        /* Upon successful SAP reset, fetch the ATR */
        sap_request_req.sap_request.sap_request = UIM_SAP_REQUEST_GET_ATR_V01;
        if(ccid_uim_send_sap_request(&sap_request_req, cmd_info_ptr, TRUE))
        {
          send_response = FALSE;
        }
      }
      break;

    case CCID_UIM_MSG_BULKOUT_XFR_BLOCK:
      if ((bulkout_req_ptr->length < CCID_UIM_APDU_MIN_TOTAL_LEN_BYTES) ||
          (bulkout_req_ptr->length > DATA_SIZE))
      {
        LOGE("Invalid input, bulkout_req_ptr->length: 0x%x \n",
             (unsigned int)bulkout_req_ptr->length);
        free(cmd_info_ptr);
        return TRUE;
      }

      LOGI("Input APDU: ");
      for(i = 0; i < bulkout_req_ptr->length && i < DATA_SIZE; i++)
      {
        fprintf(stderr, "0x%x ", bulkout_req_ptr->apdu[i]);
      }
      LOGI("\n");

      /* Send APDU request to modem */
      memset(&sap_request_req, 0, sizeof(uim_sap_request_req_msg_v01));
      sap_request_req.sap_request.slot = ccid_uim_convert_ccid_slot_to_qmi_slot(
                                           bulkout_req_ptr->slot);
      sap_request_req.sap_request.sap_request = UIM_SAP_REQUEST_SEND_APDU_V01;
      sap_request_req.apdu_valid              = TRUE;
      memcpy(sap_request_req.apdu, bulkout_req_ptr->apdu, bulkout_req_ptr->length);
      sap_request_req.apdu_len                = bulkout_req_ptr->length;

      if(ccid_uim_send_sap_request(&sap_request_req, cmd_info_ptr, TRUE))
      {
        send_response = FALSE;
      }
      break;

    default:
      LOGE("Unsupported message type: 0x%x !\n", bulkout_req_ptr->message_type);
      send_response = TRUE;
  }

  /* On error cases, free the command info pointer */
  if ((send_response == TRUE) && (cmd_info_ptr != NULL))
  {
    free(cmd_info_ptr);
    cmd_info_ptr = NULL;
  }

  return send_response;
} /* ccid_uim_handle_sap_request */


/*===========================================================================

                             INTERFACE FUNCTIONS

===========================================================================*/
/*=========================================================================

  FUNCTION:  ccid_uim_process_pc_to_rdr_bulk

===========================================================================*/
/*!
    @brief
      Performs the parsing and processing of BULK_OUT requests from BULK
      device node.

    @return
      None.
*/
/*=========================================================================*/
void ccid_uim_process_pc_to_rdr_bulk
(
  int           fd,
  uint8       * rx_buffer_ptr,
  uint16        rx_buffer_len
)
{
  uint16                          bulkin_size      = 0;
  boolean                         send_response    = FALSE;
  struct ccid_uim_bulk_out_header   * bulkout_req_ptr  = NULL;
  struct ccid_uim_bulk_in_header      bulkin_rsp;

  /* Cast input pointer */
  bulkout_req_ptr = (struct ccid_uim_bulk_out_header*)rx_buffer_ptr;

  if ((bulkout_req_ptr == NULL) ||
      (rx_buffer_len > sizeof(struct ccid_uim_bulk_out_header)))
  {
    LOGE("Invalid input, bulkout_req_ptr: %p, rx_buffer_len: 0x%x \n",
         bulkout_req_ptr, rx_buffer_len);
    return;
  }

  LOGI("\n");
  LOGI("BULK_OUT :: message_type: 0x%x, slot: 0x%x, seq_number: %d, length: %lu\n",
       bulkout_req_ptr->message_type,
       bulkout_req_ptr->slot,
       bulkout_req_ptr->seq_number,
       bulkout_req_ptr->length);
  LOGI("\n");

  /* Update common fields with default values  */
  memset(&bulkin_rsp, 0, sizeof(struct ccid_uim_bulk_in_header));
  bulkin_rsp.slot   = bulkout_req_ptr->slot;
  bulkin_rsp.seq_number    = bulkout_req_ptr->seq_number;
  bulkin_rsp.error  = CCID_UIM_SLOT_ERR_HW_ERROR;
  bulkin_rsp.status = CCID_UIM_ICC_STATUS_ICC_PRESENT_INACTIVE |
                                    CCID_UIM_CMD_STATUS_FAILED;

  /* BULKIN size is common for all except get/set params */
  bulkin_size = CCID_UIM_BULKIN_SIZE_COMMON;

  /* Determine the actual request type */
  switch (bulkout_req_ptr->message_type)
  {
    case CCID_UIM_MSG_BULKOUT_ICC_POWERON:
      bulkin_rsp.message_type = CCID_UIM_MSG_BULKIN_DATABLOCK;
      send_response = ccid_uim_handle_sap_request(bulkout_req_ptr,
                                                  &bulkin_rsp);
      bulkin_size  += bulkin_rsp.length;
      break;

    case CCID_UIM_MSG_BULKOUT_XFR_BLOCK:
      bulkin_rsp.message_type = CCID_UIM_MSG_BULKIN_DATABLOCK;
      send_response = ccid_uim_handle_sap_request(bulkout_req_ptr,
                                                  &bulkin_rsp);
      bulkin_size  += bulkin_rsp.length;
      break;

    case CCID_UIM_MSG_BULKOUT_ICC_POWEROFF:
      /* We just fake a successful response here. We send a SAP disconnect
         only on daemon exit */
      bulkin_rsp.message_type = CCID_UIM_MSG_BULKIN_SLOT_STATUS;
      bulkin_rsp.error       = CCID_UIM_SLOT_ERR_NO_ERROR;
      bulkin_rsp.status      = CCID_UIM_ICC_STATUS_ICC_PRESENT_INACTIVE |
                                  CCID_UIM_CMD_STATUS_NO_ERROR;
      send_response           = TRUE;
      break;

    case CCID_UIM_MSG_BULKOUT_GET_PARAMS:
    case CCID_UIM_MSG_BULKOUT_SET_PARAMS:
      bulkin_size += CCID_UIM_BULKIN_SIZE_PARAMS;
      bulkin_rsp.message_type = CCID_UIM_MSG_BULKIN_PARAMS;
      send_response = ccid_uim_handle_params(bulkout_req_ptr,
                                             &bulkin_rsp);
      break;

    case CCID_UIM_MSG_BULKOUT_ESCAPE:
      bulkin_rsp.message_type = CCID_UIM_MSG_BULKIN_ESCAPE;
      send_response = ccid_uim_handle_escape(bulkout_req_ptr,
                                             &bulkin_rsp);
      break;

    case CCID_UIM_MSG_BULKOUT_GET_SLOT_STATUS:
      bulkin_rsp.message_type = CCID_UIM_MSG_BULKIN_SLOT_STATUS;
      send_response = ccid_uim_handle_slot_status(bulkout_req_ptr,
                                                  &bulkin_rsp);
      break;

    case CCID_UIM_MSG_BULKOUT_ABORT:
      bulkin_rsp.message_type = CCID_UIM_MSG_BULKIN_SLOT_STATUS;
      /* Todo - Update global, sync with CTRL thread's abort */
      break;

    default:
      LOGE("Unsupported bulkout message, cannot send response !\n");
      break;
  }

  /* If required, send the response rightaway. For cases where QMI API is called,
     BULKIN response is sent in the asynchronous callback */
  if (send_response)
  {
    LOGI("\n");
    LOGI("BULK_IN  :: message_type: 0x%x, slot: 0x%x, seq_number: %d, ",
         bulkin_rsp.message_type,
         bulkin_rsp.slot,
         bulkin_rsp.seq_number);
    LOGI("status: 0x%x, error: 0x%x, length: 0x%x \n",
         bulkin_rsp.status,
         bulkin_rsp.error,
         (unsigned int)bulkin_rsp.length);
    LOGI("\n");

    /* Nothing we can do if there was an error writing */
    ccid_uim_send_write(fd,
                        (uint8*)&bulkin_rsp,
                        bulkin_size);
  }
} /* ccid_uim_process_pc_to_rdr_bulk */


/*=========================================================================

  FUNCTION:  ccid_uim_process_pc_to_rdr_ctrl

===========================================================================*/
/*!
  @brief
    Performs the parsing and processing of CTRL request from CTRL device
    node.

  @return
    None
*/
/*=========================================================================*/
void ccid_uim_process_pc_to_rdr_ctrl
(
  int           fd,
  uint8       * rx_buffer_ptr,
  uint16        rx_buffer_len
)
{
  (void) fd;

  if ((rx_buffer_ptr == NULL) ||
      (rx_buffer_len <= 0) ||
      (rx_buffer_len > CCID_UIM_CTRL_BUFFER_SIZE))
  {
    LOGE("Invalid input, rx_buffer_ptr: %p, rx_buffer_len: 0x%x \n",
         rx_buffer_ptr, rx_buffer_len);
    return;
  }

  /* Note: we currently support only ABORT */
  if (rx_buffer_ptr[0] != CCID_UIM_MSG_CTRLIN_ABORT)
  {
    LOGE("Unsupported CTRL message: 0x%x \n", rx_buffer_ptr[0]);
    return;
  }

  /* Todo (used in abort) - Update global command state */

} /* ccid_uim_process_pc_to_rdr_ctrl */


/*=========================================================================

  FUNCTION:  ccid_uim_process_rdr_to_pc_bulk

===========================================================================*/
/*!
  @brief
    Performs the parsing and processing of response from QMI.

  @return
    None
*/
/*=========================================================================*/
void ccid_uim_process_rdr_to_pc_bulk
(
  int                            fd,
  uim_sap_request_resp_msg_v01 * qmi_response_ptr,
  ccid_uim_cmd_info_type       * cmd_info_ptr,
  qmi_client_error_type          transp_err
)
{
  uint16                          i = 0;
  uint16                          bulkin_size = 0;
  struct ccid_uim_bulk_in_header      bulkin_rsp;

  assert(qmi_response_ptr != NULL);

  if(cmd_info_ptr == NULL)
  {
    LOGE("Invalid cmd_info_ptr, dropping response \n");
    return;
  }

  LOGI("ccid_uim_process_rdr_to_pc_bulk for message_type : 0x%x \n",
       cmd_info_ptr->req_msg_type);

  /* Init response struct with common data  */
  memset(&bulkin_rsp, 0, sizeof(struct ccid_uim_bulk_in_header));
  bulkin_rsp.slot   = cmd_info_ptr->slot;
  bulkin_rsp.seq_number    = cmd_info_ptr->sequence;
  bulkin_rsp.error  = CCID_UIM_SLOT_ERR_HW_ERROR;
  bulkin_rsp.status = CCID_UIM_ICC_STATUS_ICC_PRESENT_INACTIVE |
                         CCID_UIM_CMD_STATUS_FAILED;

  /* Update the minimum BULKIN size */
  bulkin_size = CCID_UIM_BULKIN_SIZE_COMMON;

  if ((transp_err != QMI_NO_ERR) ||
      (qmi_response_ptr->resp.result != QMI_RESULT_SUCCESS_V01))
  {
    LOGE("Error in response, transp_err: 0x%x, result: 0x%x \n",
         transp_err, qmi_response_ptr->resp.result);
  }
  else
  {
    switch (cmd_info_ptr->req_msg_type)
    {
      case CCID_UIM_MSG_BULKOUT_ICC_POWERON:
        bulkin_rsp.message_type = CCID_UIM_MSG_BULKIN_DATABLOCK;
        if ((qmi_response_ptr->ATR_value_valid) &&
            (qmi_response_ptr->ATR_value_len > 0) &&
            (qmi_response_ptr->ATR_value_len <= QMI_UIM_ATR_DATA_MAX_V01) &&
            (qmi_response_ptr->ATR_value_len <= DATA_SIZE))
        {
          bulkin_size += qmi_response_ptr->ATR_value_len;
          bulkin_rsp.error  = CCID_UIM_SLOT_ERR_NO_ERROR;
          bulkin_rsp.status = CCID_UIM_ICC_STATUS_ICC_PRESENT_ACTIVE |
                                 CCID_UIM_CMD_STATUS_NO_ERROR;
          bulkin_rsp.length = qmi_response_ptr->ATR_value_len;
          memcpy(bulkin_rsp.data, qmi_response_ptr->ATR_value, bulkin_rsp.length);
        }
        break;

      case CCID_UIM_MSG_BULKOUT_XFR_BLOCK:
        bulkin_rsp.message_type = CCID_UIM_MSG_BULKIN_DATABLOCK;
        if ((qmi_response_ptr->apdu_valid) &&
            (qmi_response_ptr->apdu_len > 0) &&
            (qmi_response_ptr->apdu_len <= QMI_UIM_APDU_DATA_MAX_V01) &&
            (qmi_response_ptr->apdu_len <= DATA_SIZE))
        {
          /* Regular response processing */
          bulkin_size            += qmi_response_ptr->apdu_len;
          bulkin_rsp.length      = qmi_response_ptr->apdu_len;
          memcpy(bulkin_rsp.data, qmi_response_ptr->apdu, bulkin_rsp.length);

          /* Update successful status bytes */
          bulkin_rsp.error       = CCID_UIM_SLOT_ERR_NO_ERROR;
          bulkin_rsp.status      = CCID_UIM_ICC_STATUS_ICC_PRESENT_ACTIVE |
                                      CCID_UIM_CMD_STATUS_NO_ERROR;
        }
        break;

      default:
        LOGE("Unsupported req_msg_type = 0x%x", cmd_info_ptr->req_msg_type);
        free(cmd_info_ptr);
        return;
    }
  }

  /* Log BULK_IN response */
  LOGI("\n");
  LOGI("BULK_IN  :: message_type: 0x%x, slot: 0x%x, seq_number: %d, ",
         bulkin_rsp.message_type,
         bulkin_rsp.slot,
         bulkin_rsp.seq_number);
  LOGI("status: 0x%x, error: 0x%x, length: 0x%x \n",
         bulkin_rsp.status,
         bulkin_rsp.error,
         (unsigned int)bulkin_rsp.length);
  LOGI("Output APDU: ");

  for(i = 0; i < bulkin_rsp.length; i++)
  {
    fprintf(stderr, "0x%x ", bulkin_rsp.data[i]);
  }
  LOGI("\n");

  /* Send the response on the BULK node.
     Nothing we can do if there was an error writing */
  ccid_uim_send_write(fd,
                      (uint8*)&bulkin_rsp,
                      bulkin_size);

  /* Free command info pointer allocated in the request */
  free(cmd_info_ptr);
  cmd_info_ptr = NULL;

} /* ccid_uim_process_rdr_to_pc_bulk */

